#include "CvGameCoreDLL.h"
#include "CvBuildingInfo.h"
#include "CvCity.h"
#include "CvGameAI.h"
#include "CvGame.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvImprovementInfo.h"
#include "CvBonusInfo.h"
#include "CvMap.h"
#include "CvMapExternal.h"
#include "CvPlayerAI.h"
#include "CvPlot.h"
#include "CvPython.h"
#include "CvSelectionGroup.h"
#include "CvTeamAI.h"
#include "CvUnit.h"
#include "CvDLLFAStarIFaceBase.h"
#include "CheckSum.h"
#include "FAStarNode.h"


#define PATH_MOVEMENT_WEIGHT									(1000)
#define PATH_RIVER_WEIGHT										(100)
#define PATH_CITY_WEIGHT										(100)
#define PATH_DEFENSE_WEIGHT										(10)
#define PATH_TERRITORY_WEIGHT									(3)
#define PATH_STEP_WEIGHT										(2)
#define PATH_STRAIGHT_WEIGHT									(1)
#define	PATH_ENEMY_CITY_WEIGHT									(500)
#define PATH_DAMAGE_WEIGHT										(2000)

CvPlot* plotCity(int iX, int iY, int iIndex)
{
	return GC.getMap().plot((iX + GC.getCityPlotX()[iIndex]), (iY + GC.getCityPlotY()[iIndex]));
}

int plotCityXY(int iDX, int iDY)
{
	if (abs(iDX) > CITY_PLOTS_RADIUS || abs(iDY) > CITY_PLOTS_RADIUS)
	{
		return -1;
	}
	return GC.getXYCityPlot((iDX + CITY_PLOTS_RADIUS), (iDY + CITY_PLOTS_RADIUS));
}

int plotCityXY(const CvCity* pCity, const CvPlot* pPlot)
{
	return plotCityXY(dxWrap(pPlot->getX() - pCity->getX()), dyWrap(pPlot->getY() - pCity->getY()));
}

DirectionTypes cardinalDirectionToDirection(CardinalDirectionTypes eCard)
{
	switch (eCard)
	{
	case CARDINALDIRECTION_NORTH:
		return DIRECTION_NORTH;
	case CARDINALDIRECTION_EAST:
		return DIRECTION_EAST;
	case CARDINALDIRECTION_SOUTH:
		return DIRECTION_SOUTH;
	case CARDINALDIRECTION_WEST:
		return DIRECTION_WEST;
	}
	return NO_DIRECTION;
}

bool isCardinalDirection(DirectionTypes eDirection)
{
	switch( eDirection )
	{
	case DIRECTION_EAST:
	case DIRECTION_NORTH:
	case DIRECTION_SOUTH:
	case DIRECTION_WEST:
		return true;
	}
	return false;
}

DirectionTypes estimateDirection(int iDX, int iDY)
{
	PROFILE_EXTRA_FUNC();
	const int displacementSize = 8;
	static float sqrt2 = 1 / sqrt(2.0f);
	//													N			NE			E			SE				S			SW				W			NW
	static float displacements[displacementSize][2] = {{0, 1}, {sqrt2, sqrt2}, {1, 0}, {sqrt2, -sqrt2}, {0, -1}, {-sqrt2, -sqrt2}, {-1, 0}, {-sqrt2, sqrt2}};
	float maximum = 0;
	int maximumIndex = -1;
	for(int i=0;i<displacementSize;i++)
	{
		const float dotProduct = iDX * displacements[i][0] + iDY * displacements[i][1];
		if(dotProduct > maximum)
		{
			maximum = dotProduct;
			maximumIndex = i;
		}
	}

	return (DirectionTypes) maximumIndex;
}

DirectionTypes estimateDirection(const CvPlot* pFromPlot, const CvPlot* pToPlot)
{
	FAssertMsg(pFromPlot != NULL && pToPlot != NULL, "Both plots must be valid to estimate direction the direction between them");
	return estimateDirection(dxWrap(pToPlot->getX() - pFromPlot->getX()), dyWrap(pToPlot->getY() - pFromPlot->getY()));
}


float directionAngle( DirectionTypes eDirection )
{
	switch( eDirection )
	{
	case DIRECTION_NORTHEAST:	return fM_PI * 0.25f;
	case DIRECTION_EAST:		return fM_PI * 0.5f;
	case DIRECTION_SOUTHEAST:	return fM_PI * 0.75f;
	case DIRECTION_SOUTH:		return fM_PI * 1.0f;
	case DIRECTION_SOUTHWEST:	return fM_PI * 1.25f;
	case DIRECTION_WEST:		return fM_PI * 1.5f;
	case DIRECTION_NORTHWEST:	return fM_PI * 1.75f;
	default:
	case DIRECTION_NORTH:		return 0.0f;
	}
}

bool atWar(TeamTypes eTeamA, TeamTypes eTeamB)
{
	if ((eTeamA == NO_TEAM) || (eTeamB == NO_TEAM))
	{
		return false;
	}
	//FAssert(GET_TEAM(eTeamA).isAtWar(eTeamB) == GET_TEAM(eTeamB).isAtWar(eTeamA));
	FAssert((eTeamA != eTeamB) || !(GET_TEAM(eTeamA).isAtWar(eTeamB)));

	return GET_TEAM(eTeamA).isAtWar(eTeamB);
}

bool isNonAlly(TeamTypes eOurTeam, TeamTypes eTheirTeam)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eOurTeam);

	if (eTheirTeam == NO_TEAM)
	{
		return true;
	}

	return (atWar(eOurTeam, eTheirTeam) || GET_TEAM(eTheirTeam).canDeclareWar(eOurTeam));
}

bool isPotentialEnemy(TeamTypes eOurTeam, TeamTypes eTheirTeam)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eOurTeam);
	return (
		eTheirTeam != NO_TEAM
		&&
		(
			atWar(eOurTeam, eTheirTeam)
			||
			GET_TEAM(eOurTeam).AI_isSneakAttackReady(eTheirTeam)
			&&
			GET_TEAM(eOurTeam).canDeclareWar(eTheirTeam)
		)
	);
}

CvCity* getCity(IDInfo city)
{
	if (city.eOwner >= 0 && city.eOwner < MAX_PLAYERS)
	{
		return GET_PLAYER((PlayerTypes)city.eOwner).getCity(city.iID);
	}
	return NULL;
}

CvUnit* getUnit(IDInfo unit)
{
	if (unit.eOwner >= 0 && unit.eOwner < MAX_PLAYERS)
	{
		return GET_PLAYER((PlayerTypes)unit.eOwner).getUnit(unit.iID);
	}
	return NULL;
}

bool isBeforeUnitCycle(const CvUnit* pFirstUnit, const CvUnit* pSecondUnit)
{
	FAssert(pFirstUnit != NULL);
	FAssert(pSecondUnit != NULL);
	FAssert(pFirstUnit != pSecondUnit);

	if (pFirstUnit->getOwner() != pSecondUnit->getOwner())
	{
		return (pFirstUnit->getOwner() < pSecondUnit->getOwner());
	}

	if (pFirstUnit->getDomainType() != pSecondUnit->getDomainType())
	{
		return (pFirstUnit->getDomainType() < pSecondUnit->getDomainType());
	}

	if (pFirstUnit->baseCombatStr() != pSecondUnit->baseCombatStr())
	{
		return (pFirstUnit->baseCombatStr() > pSecondUnit->baseCombatStr());
	}

	if (pFirstUnit->getUnitType() != pSecondUnit->getUnitType())
	{
		return (pFirstUnit->getUnitType() > pSecondUnit->getUnitType());
	}

	if (pFirstUnit->getLevel() != pSecondUnit->getLevel())
	{
		return (pFirstUnit->getLevel() > pSecondUnit->getLevel());
	}

	if (pFirstUnit->getExperience() != pSecondUnit->getExperience())
	{
		return (pFirstUnit->getExperience() > pSecondUnit->getExperience());
	}

	return (pFirstUnit->getID() < pSecondUnit->getID());
}



int getScoreValueOfTech(TechTypes eTech)
{
	return 1 + GC.getTechInfo(eTech).getEra();
}

int getWonderScore(BuildingTypes eWonder)
{
	return isLimitedWonder(eWonder) ? 6 : 1;
}

ImprovementTypes finalImprovementUpgrade(ImprovementTypes eImprovement, int iCount)
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), eImprovement);

	if (iCount > GC.getNumImprovementInfos())
	{
		return NO_IMPROVEMENT;
	}

	if (GC.getImprovementInfo(eImprovement).getImprovementUpgrade() != NO_IMPROVEMENT)
	{
		return finalImprovementUpgrade(GC.getImprovementInfo(eImprovement).getImprovementUpgrade(), (iCount + 1));
	}
	return eImprovement;
}

int getWorldSizeMaxConscript(CivicTypes eCivic)
{
	int iMaxConscript = GC.getCivicInfo(eCivic).getMaxConscript();

	iMaxConscript *= std::max(0, (GC.getWorldInfo(GC.getMap().getWorldSize()).getMaxConscriptModifier() + 100));
	iMaxConscript /= 100;

	return iMaxConscript;
}

bool isReligionTech(TechTypes eTech)
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if (GC.getReligionInfo((ReligionTypes)iI).getTechPrereq() == eTech)
		{
			return true;
		}
	}
	return false;
}

bool isCorporationTech(TechTypes eTech)
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (GC.getCorporationInfo((CorporationTypes)iI).getTechPrereq() == eTech)
		{
			return true;
		}
	}
	return false;
}

bool isTechRequiredForUnit(TechTypes eTech, UnitTypes eUnit)
{
	const CvUnitInfo& info = GC.getUnitInfo(eUnit);

	if (info.getPrereqAndTech() == eTech)
	{
		return true;
	}
	return algo::any_of_equal(info.getPrereqAndTechs(), eTech);
}

bool isTechRequiredForBuilding(TechTypes eTech, BuildingTypes eBuilding)
{
	const CvBuildingInfo& info = GC.getBuildingInfo(eBuilding);

	if (info.getPrereqAndTech() == eTech
	|| algo::any_of_equal(info.getPrereqAndTechs(), eTech))
	{
		return true;
	}

	const SpecialBuildingTypes eSpecial = info.getSpecialBuilding();
	if (NO_SPECIALBUILDING != eSpecial && GC.getSpecialBuildingInfo(eSpecial).getTechPrereq() == eTech)
	{
		return true;
	}

	return false;
}

bool isTechRequiredForProject(TechTypes eTech, ProjectTypes eProject)
{
	if (GC.getProjectInfo(eProject).getTechPrereq() == eTech)
	{
		return true;
	}

	return false;
}

bool isWorldUnit(UnitTypes eUnit)
{
	return GC.getUnitInfo(eUnit).getMaxGlobalInstances() != -1;
}
bool isNationalUnit(UnitTypes eUnit)
{
	return GC.getUnitInfo(eUnit).getMaxPlayerInstances() != -1;
}
bool isLimitedUnit(UnitTypes eUnit)
{
	return (isWorldUnit(eUnit) || isNationalUnit(eUnit));
}

bool isWorldWonder(BuildingTypes building)
{
	return GC.getBuildingInfo(building).getMaxGlobalInstances() != -1;
}

bool isTeamWonder(BuildingTypes building)
{
	return GC.getBuildingInfo(building).getMaxTeamInstances() != -1;
}

bool isNationalWonder(BuildingTypes building)
{
	return GC.getBuildingInfo(building).getMaxPlayerInstances() != -1;
}

bool isNationalWonderGroup(BuildingTypes building)
{
	const SpecialBuildingTypes eSpecialBuilding = GC.getBuildingInfo(building).getSpecialBuilding();
	return eSpecialBuilding != NO_SPECIALBUILDING && GC.getSpecialBuildingInfo(eSpecialBuilding).getMaxPlayerInstances() != -1;
}

bool isNationalWonderGroupSpecialBuilding(SpecialBuildingTypes eSpecialBuilding)
{
	return GC.getSpecialBuildingInfo(eSpecialBuilding).getMaxPlayerInstances() != -1;
}

bool isLimitedWonder(BuildingTypes eBuilding)
{
	return isWorldWonder(eBuilding) || isTeamWonder(eBuilding) || isNationalWonder(eBuilding);
}

int limitedWonderLimit(BuildingTypes eBuilding)
{
	int iCount = 0;
	bool bIsLimited = false;
	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	int iMax = kBuilding.getMaxGlobalInstances();
	if (iMax != -1)
	{
		iCount += iMax;
		bIsLimited = true;
	}

	iMax = kBuilding.getMaxTeamInstances();
	if (iMax != -1)
	{
		iCount += iMax;
		bIsLimited = true;
	}

	iMax = kBuilding.getMaxPlayerInstances();
	if (iMax != -1)
	{
		iCount += iMax;
		bIsLimited = true;
	}

	const SpecialBuildingTypes eSpecialBuilding = kBuilding.getSpecialBuilding();
	if (eSpecialBuilding != NO_SPECIALBUILDING)
	{
		iMax = GC.getSpecialBuildingInfo(eSpecialBuilding).getMaxPlayerInstances();
		if (iMax != -1)
		{
			iCount += iMax;
			bIsLimited = true;
		}
	}

	return bIsLimited ? iCount : -1;
}

bool isWorldProject(ProjectTypes eProject)
{
	return (GC.getProjectInfo(eProject).getMaxGlobalInstances() != -1);
}

bool isTeamProject(ProjectTypes eProject)
{
	return (GC.getProjectInfo(eProject).getMaxTeamInstances() != -1);
}

bool isLimitedProject(ProjectTypes eProject)
{
	return (isWorldProject(eProject) || isTeamProject(eProject));
}

// FUNCTION: getBinomialCoefficient
// Needed for getCombatOdds
// Returns int value, being the possible number of combinations
// of k draws out of a population of n
// Written by DeepO
// Modified by Jason Winokur to keep the intermediate factorials small
int64_t getBinomialCoefficient(int iN, int iK)
{
	PROFILE_EXTRA_FUNC();
	int64_t iTemp = 1;
	//take advantage of symmetry in combination, eg. 15C12 = 15C3
	iK = std::min(iK, iN - iK);

	//eg. 15C3 = (15 * 14 * 13) / (1 * 2 * 3) = 15 / 1 * 14 / 2 * 13 / 3 = 455
	for (int i = 1; i <= iK; i++)
	{
		iTemp = iTemp * (iN - i + 1) / i;
	}
	return iTemp;
}

// FUNCTION: getCombatOdds
// Calculates combat odds, given two units
// Returns value from 0-1000
// Written by DeepO
int getCombatOdds(const CvUnit* pAttacker, const CvUnit* pDefender)
{
	PROFILE_EXTRA_FUNC();

	// setup battle, calculate strengths and odds

	//TB Combat Mod begin
	//Added ST
	int iAttackerStrength = pAttacker->currCombatStr(NULL, NULL);
	int iAttackerFirepower = pAttacker->currFirepower(NULL, NULL);
	int iDefenderStrength = pDefender->currCombatStr(pDefender->plot(), pAttacker);
	int iDefenderFirepower = pDefender->currFirepower(pDefender->plot(), pAttacker);

#ifdef STRENGTH_IN_NUMBERS
	if (GC.getGame().isOption(GAMEOPTION_COMBAT_STRENGTH_IN_NUMBERS))
	{
		const int iAttackerSupportStrength = pAttacker->getAttackerSupportValue();
		iAttackerStrength += iAttackerSupportStrength;
		iAttackerFirepower += iAttackerSupportStrength;
		const int iDefenderSupportStrength = pDefender->getDefenderSupportValue(pAttacker);
		iDefenderStrength += iDefenderSupportStrength;
		iDefenderFirepower += iDefenderSupportStrength;
	}
#endif // STRENGTH_IN_NUMBERS

	FAssert(iAttackerStrength + iDefenderStrength > 0);
	FAssert(iAttackerFirepower + iDefenderFirepower > 0);

	const int iDefenderInitialOdds = GC.getCOMBAT_DIE_SIDES() * iDefenderStrength / std::max(1, iAttackerStrength + iDefenderStrength);
	const int iDefenderHitOdds = (
		std::max(
			5,
			iDefenderInitialOdds
			+
			iDefenderInitialOdds
			*
			(pDefender->precisionVSOpponentProbTotal(pAttacker) - pAttacker->dodgeVSOpponentProbTotal(pDefender))
			/
			100
		)
	);
	const int iAttackerInitialOdds = GC.getCOMBAT_DIE_SIDES() - iDefenderInitialOdds;
	const int iAttackerHitOdds = (
		std::max(
			5,
			iAttackerInitialOdds
			+
			iAttackerInitialOdds
			*
			(pAttacker->precisionVSOpponentProbTotal(pDefender) - pDefender->dodgeVSOpponentProbTotal(pAttacker))
			/
			100
		)
	);
	const int iDefenderOdds = ((iDefenderHitOdds - iAttackerHitOdds) + GC.getCOMBAT_DIE_SIDES()) / 2;
	const int iAttackerOdds = ((iAttackerHitOdds - iDefenderHitOdds) + GC.getCOMBAT_DIE_SIDES()) / 2;
	//TB Combat Mods end
	if (iDefenderOdds == 0)
	{
		return 1000;
	}
	if (iAttackerOdds == 0)
	{
		return 0;
	}
	int iOdds = 0;
	const int iStrengthFactor = (iAttackerFirepower + iDefenderFirepower + 1) / 2;

	// calculate damage done in one round
	//TB Combat Mods (Armor Compare)
	const int iDamageToAttackerBase = GC.getCOMBAT_DAMAGE() * (iDefenderFirepower + iStrengthFactor) / std::max(1, iAttackerFirepower + iStrengthFactor);
	const int iDamageToDefenderBase = GC.getCOMBAT_DAMAGE() * (iAttackerFirepower + iStrengthFactor) / std::max(1, iDefenderFirepower + iStrengthFactor);

	const int iDamageToAttacker = (
		std::max(
			1,
			(
				100
				* iDamageToAttackerBase
				+ iDamageToAttackerBase
				* pDefender->damageModifierTotal()
			)
			*
			(
				100
				-
				range(
					pAttacker->armorVSOpponentProbTotal(pDefender)
					-
					pDefender->punctureVSOpponentProbTotal(pAttacker)
					, 0
					, 95
				)
			)
			/
			100
		)
	);
	const int iDamageToDefender = (
		std::max(
			1,
			(
				100
				* iDamageToDefenderBase
				+ iDamageToDefenderBase
				* pAttacker->damageModifierTotal()
			)
			*
			(
				100
				-
				range(
					pDefender->armorVSOpponentProbTotal(pAttacker)
					-
					pAttacker->punctureVSOpponentProbTotal(pDefender)
					, 0
					, 95
				)
			)
			/
			100
		)
	);
	//TB Combat Mods (Armor Compare) end

	// calculate needed rounds.
	// Needed rounds = round_up(health/damage)
	int iNeededRoundsAttacker = (std::max(0, 100*pDefender->getHP() + 100*pAttacker->combatLimit(pDefender) - 100*pDefender->getMaxHP()) + iDamageToDefender - 100) / iDamageToDefender;
	int iNeededRoundsDefender = (100*pAttacker->getHP() + iDamageToAttacker - 100) / iDamageToAttacker;
	if (iNeededRoundsAttacker > 20 * iNeededRoundsDefender || iNeededRoundsDefender > 20 * iNeededRoundsAttacker || iNeededRoundsAttacker + iNeededRoundsDefender > 100)
	{
		// Solves inaccuracies that arise when combat involves an extremely weak unit like a pigeon
		//	Where a very strong unit would get max XP from combat against the very weak one because the combat odds
		//	here would evaluate to less than 1% chance of winning due to the math for fOddsAfterEvent breaking down.
		// The tiny overall reduction in accuracy by doing this for these edge cases is worth the efficiency boost and math integrity.
		iNeededRoundsAttacker /= 10;
		iNeededRoundsDefender /= 10;
	}
	if (iNeededRoundsAttacker < 0) iNeededRoundsAttacker = 1;
	if (iNeededRoundsDefender < 0) iNeededRoundsDefender = 1;
	const int iMaxRounds = iNeededRoundsAttacker + iNeededRoundsDefender - 1;

	// calculate possible first strikes distribution.
	// We can't use the getCombatFirstStrikes() function (only one result, no distribution), so we need to mimic it.
	const int iAttackerLowFS = pDefender->immuneToFirstStrikes() ? 0 : pAttacker->firstStrikes();
	const int iAttackerHighFS = pDefender->immuneToFirstStrikes() ? 0 : pAttacker->firstStrikes() + pAttacker->chanceFirstStrikes();

	const int iDefenderLowFS = (pAttacker->immuneToFirstStrikes()) ? 0 : pDefender->firstStrikes();
	const int iDefenderHighFS = (pAttacker->immuneToFirstStrikes()) ? 0 : (pDefender->firstStrikes() + pDefender->chanceFirstStrikes());

	// For every possible first strike event, calculate the odds of combat.
	// Then, add these to the total, weighted to the chance of that first strike event occurring
	for (int i1 = iAttackerLowFS; i1 < iAttackerHighFS + 1; i1++)
	{
		for (int i2 = iDefenderLowFS; i2 < iDefenderHighFS + 1; i2++)
		{
			// for every possible combination of fs results, calculate the chance

			if (i1 >= i2)
			{
				// Attacker gets more or equal first strikes than defender
				const int iFirstStrikes = i1 - i2;

				// For every possible first strike getting hit, calculate both
				// the chance of that event happening, as well as the rest of
				// the chance assuming the event has happened. Multiply these
				// together to get the total chance (Bayes rule).
				// i3 counts the number of successful first strikes
				for (int i3 = 0; i3 < iFirstStrikes + 1; i3++)
				{
					// event: i3 first strikes hit the defender

					// calculate chance of i3 first strikes hitting: fOddsEvent
					// f(k;n,p)=C(n,k)*(p^k)*((1-p)^(n-k))
					// this needs to be in floating point math
					const float fOddsEvent = (
						((float)getBinomialCoefficient(iFirstStrikes, i3))
						* pow((((float)iAttackerOdds) / GC.getCOMBAT_DIE_SIDES()), i3)
						* pow(1.0f - ((float)iAttackerOdds) / GC.getCOMBAT_DIE_SIDES(), iFirstStrikes - i3)
					);
					// calculate chance assuming i3 first strike hits: fOddsAfterEvent
					float fOddsAfterEvent = 1;

					if (i3 < iNeededRoundsAttacker)
					{
						// odds for _at_least_ (iNeededRoundsAttacker - i3) (the remaining hits the attacker needs to make)
						// out of (iMaxRounds - i3) (the left over rounds) is the sum of each _exact_ draw
						fOddsAfterEvent = 0;

						for (int i4 = iNeededRoundsAttacker - i3; i4 < iMaxRounds - i3 + 1; i4++)
						{
							// odds of exactly i4 out of (iMaxRounds - i3) draws.
							// f(k;n,p)=C(n,k)*(p^k)*((1-p)^(n-k))
							// this needs to be in floating point math
							fOddsAfterEvent += (
								((float)getBinomialCoefficient(iMaxRounds - i3, i4))
								* pow(((float)iAttackerOdds) / GC.getCOMBAT_DIE_SIDES(), i4)
								* pow(1.0f - ((float)iAttackerOdds) / GC.getCOMBAT_DIE_SIDES(), iMaxRounds - i3 - i4)
							);
						}
					}
					// Multiply these together, round them properly, and add
					// the result to the total iOdds
					iOdds += (int)(1000 * (fOddsEvent * fOddsAfterEvent + 0.0005));
				}
			}
			else // (i1 < i2)
			{
				// Attacker gets less first strikes than defender
				const int iFirstStrikes = i2 - i1;

				// For every possible first strike getting hit, calculate both
				// the chance of that event happening, as well as the rest of
				// the chance assuming the event has happened. Multiply these
				// together to get the total chance (Bayes rule).
				// i3 counts the number of successful first strikes
				for (int i3 = 0; i3 < (iFirstStrikes + 1); i3++)
				{
					// event: i3 first strikes hit the defender

					// First of all, check if the attacker is still alive.
					// Otherwise, no further calculations need to occur
					if (i3 < iNeededRoundsDefender)
					{
						// Calculate chance of i3 first strikes hitting: fOddsEvent
						// f(k;n,p)=C(n,k)*(p^k)*((1-p)^(n-k))
						// This needs to be in floating point math
						const float fOddsEvent = ((float)getBinomialCoefficient(iFirstStrikes, i3)) * pow((((float)iDefenderOdds) / GC.getCOMBAT_DIE_SIDES()), i3) * pow((1.0f - (((float)iDefenderOdds) / GC.getCOMBAT_DIE_SIDES())), (iFirstStrikes - i3));

						// calculate chance assuming i3 first strike hits: fOddsAfterEvent
						float fOddsAfterEvent = 0;

						// odds for _at_least_ iNeededRoundsAttacker (the remaining hits the attacker needs to make)
						// out of (iMaxRounds - i3) (the left over rounds) is the sum of each _exact_ draw
						for (int i4 = iNeededRoundsAttacker; i4 < iMaxRounds - i3 + 1; i4++)
						{
							// Odds of exactly i4 out of (iMaxRounds - i3) draws.
							// f(k;n,p)=C(n,k)*(p^k)*((1-p)^(n-k))
							// This needs to be in floating point math
							fOddsAfterEvent += (
								((float)getBinomialCoefficient(iMaxRounds - i3, i4))
								* pow((((float)iAttackerOdds) / GC.getCOMBAT_DIE_SIDES()), i4)
								* pow(1.0f - ((float)iAttackerOdds) / GC.getCOMBAT_DIE_SIDES(), iMaxRounds - i3 - i4)
							);
						}
						// Multiply these together, round them properly, and add the result to the total iOdds
						iOdds += (int)(1000 * (fOddsEvent*fOddsAfterEvent + 0.0005));
					}
				}
			}
		}
	}

	// Weigh the total to the number of possible combinations of first strikes events
	// note: the integer math breaks down when #FS > 656 (with a die size of 1000)
	iOdds /= ((pDefender->immuneToFirstStrikes() ? 0 : pAttacker->chanceFirstStrikes()) + 1) * ((pAttacker->immuneToFirstStrikes() ? 0 : pDefender->chanceFirstStrikes()) + 1);

	FASSERT_BOUNDS(0, 1001, iOdds);
	return iOdds;
}

/*************************************************************************************************/
/** ADVANCED COMBAT ODDS                      11/7/09                           PieceOfMind      */
/** BEGIN                                                                       v1.1             */
/*************************************************************************************************/

//Calculates the probability of a particular combat outcome
//Returns a float value (between 0 and 1)
//Written by PieceOfMind
//n_A = hits taken by attacker, n_D = hits taken by defender.
float getCombatOddsSpecific(const CvUnit* pAttacker, const CvUnit* pDefender, int n_A, int n_D)
{
	PROFILE_EXTRA_FUNC();

	//TB Combat Mods (Armor Compare)
	int iAttackerStrength = pAttacker->currCombatStr(NULL, NULL);
	int iAttackerFirepower = pAttacker->currFirepower(NULL, NULL);
	int iDefenderStrength = pDefender->currCombatStr(pDefender->plot(), pAttacker);
	int iDefenderFirepower = pDefender->currFirepower(pDefender->plot(), pAttacker);

#ifdef STRENGTH_IN_NUMBERS
	if (GC.getGame().isOption(GAMEOPTION_COMBAT_STRENGTH_IN_NUMBERS))
	{
		const int iDefenderSupportStrength = pDefender->getDefenderSupportValue(pAttacker);
		const int iAttackerSupportStrength = pAttacker->getAttackerSupportValue();
		iAttackerStrength += iAttackerSupportStrength;
		iAttackerFirepower += iAttackerSupportStrength;
		iDefenderStrength += iDefenderSupportStrength;
		iDefenderFirepower += iDefenderSupportStrength;
	}
#endif // STRENGTH_IN_NUMBERS

	//TB Combat Mods End

	const int iStrengthFactor = ((iAttackerFirepower + iDefenderFirepower + 1) / 2);

	const int iDamageToAttackerBase = GC.getCOMBAT_DAMAGE() * (iDefenderFirepower + iStrengthFactor) / std::max(1, iAttackerFirepower + iStrengthFactor);
	const int iDamageToDefenderBase = GC.getCOMBAT_DAMAGE() * (iAttackerFirepower + iStrengthFactor) / std::max(1, iDefenderFirepower + iStrengthFactor);

	const int iDamageToAttacker = (
		std::max(
			1,
			(iDamageToAttackerBase + iDamageToAttackerBase * pDefender->damageModifierTotal() / 100)
			*
			(100 - range(pAttacker->armorVSOpponentProbTotal(pDefender) - pDefender->punctureVSOpponentProbTotal(pAttacker), 0, 95))
			/
			100
		)
	);
	const int iDamageToDefender = (
		std::max(
			1,
			(iDamageToDefenderBase + iDamageToDefenderBase * pAttacker->damageModifierTotal() / 100)
			*
			(100 - range(pDefender->armorVSOpponentProbTotal(pAttacker) - pAttacker->punctureVSOpponentProbTotal(pDefender), 0, 95))
			/
			100
		)
	);

	//TB Combat Mods begin (Dodge/Precision)

	const int iDefenderInitialOdds = GC.getCOMBAT_DIE_SIDES() * iDefenderStrength / (iAttackerStrength + iDefenderStrength);
	const int iAttackerInitialOdds = GC.getCOMBAT_DIE_SIDES() - iDefenderInitialOdds;

	const int iDefenderHitOdds = (
		std::max(
			5,
			iDefenderInitialOdds
			+
			(
				pDefender->precisionVSOpponentProbTotal(pAttacker)
				-
				pAttacker->dodgeVSOpponentProbTotal(pDefender)
			)
			*
			iDefenderInitialOdds / 100
		)
	);
	const int iAttackerHitOdds = (
		std::max(
			5,
			iAttackerInitialOdds
			+
			(
				pAttacker->precisionVSOpponentProbTotal(pDefender)
				-
				pDefender->dodgeVSOpponentProbTotal(pAttacker)
			)
			* iAttackerInitialOdds / 100
		)
	);
	int iDefenderOdds = ((iDefenderHitOdds - iAttackerHitOdds) + GC.getCOMBAT_DIE_SIDES()) / 2;
	int iAttackerOdds = ((iAttackerHitOdds - iDefenderHitOdds) + GC.getCOMBAT_DIE_SIDES()) / 2;

	//TB Combat Mods end (Dodge/Precision)

	if (GC.getDefineINT("ACO_IgnoreBarbFreeWins")==0)
	{
		if (pDefender->isHominid())
		{
			//defender is barbarian
			if (!GET_PLAYER(pAttacker->getOwner()).isHominid() && GET_PLAYER(pAttacker->getOwner()).getWinsVsBarbs() < GC.getHandicapInfo(GET_PLAYER(pAttacker->getOwner()).getHandicapType()).getFreeWinsVsBarbs())
			{
				//attacker is not barb and attacker player has free wins left
				//I have assumed in the following code only one of the units (attacker and defender) can be a barbarian

				iDefenderOdds = std::min((10 * GC.getCOMBAT_DIE_SIDES()) / 100, iDefenderOdds);
				iAttackerOdds = std::max((90 * GC.getCOMBAT_DIE_SIDES()) / 100, iAttackerOdds);
			}
		}
		else if (pAttacker->isHominid())
		{
			//attacker is barbarian
			if (!GET_PLAYER(pDefender->getOwner()).isHominid() && GET_PLAYER(pDefender->getOwner()).getWinsVsBarbs() < GC.getHandicapInfo(GET_PLAYER(pDefender->getOwner()).getHandicapType()).getFreeWinsVsBarbs())
			{
				//defender is not barbarian and defender has free wins left and attacker is barbarian
				iAttackerOdds = std::min((10 * GC.getCOMBAT_DIE_SIDES()) / 100, iAttackerOdds);
				iDefenderOdds = std::max((90 * GC.getCOMBAT_DIE_SIDES()) / 100, iDefenderOdds);
			}
		}
	}

	const int iNeededRoundsAttacker = (pDefender->getHP() - pDefender->getMaxHP() + pAttacker->combatLimit(pDefender) - (((pAttacker->combatLimit(pDefender))==pDefender->getMaxHP())?1:0))/iDamageToDefender + 1;
	const int iNeededRoundsDefender = (pAttacker->getHP() + iDamageToAttacker - 1 ) / iDamageToAttacker;

	const int N_D = (
		(
			std::max(0, pDefender->getHP() + pAttacker->combatLimit(pDefender) - pDefender->getMaxHP())
			+ iDamageToDefender
			- (pAttacker->combatLimit(pDefender) == pDefender->getMaxHP())
		)
		/ iDamageToDefender
	);
	const int N_A = 1 + (pAttacker->getHP() - 1) / iDamageToAttacker;

	// TB Combat Mods:
	int z;
	const int expectedrndcnt = std::min(iNeededRoundsDefender, iNeededRoundsAttacker);
	// Determine Attack Withdraw odds
	{
		const int iWithdrawal = range(pAttacker->withdrawVSOpponentProbTotal(pDefender, pDefender->plot()) - pDefender->pursuitVSOpponentProbTotal(pAttacker), 0, 100);

		int y = iWithdrawal;
		z = iWithdrawal;

		for (int Time = expectedrndcnt * pAttacker->earlyWithdrawTotal() / 100; Time > 0; --Time)
		{
			z += iWithdrawal * y / 100;
			y  = iWithdrawal * (100 - z) / 100; // Prob next round is prob per round times prob you haven't already
		}
	}
	const float RetreatOdds = std::min(z, 100) / 100.0f;

	//  Determine Attack Knockback odds
	{
		const int iKnockback = range(pAttacker->knockbackVSOpponentProbTotal(pDefender) - pDefender->unyieldingTotal(), 0, 100);

		int y = iKnockback;
		z = iKnockback;

		for (int Time = pAttacker->knockbackRetriesTotal(); Time > -1; --Time)
		{
			z += iKnockback * y / 100;
			y  = iKnockback * (100 - z) / 100; // Prob next round is prob per round times prob you haven't already
		}
	}
	const float KnockbackOdds = std::min(z, 100) / 100.0f;

	//  Determine Defensive Withdrawal odds
	{
		const int iDefWithdrawal = range(pDefender->withdrawVSOpponentProbTotal(pAttacker, pDefender->plot()) - pAttacker->pursuitVSOpponentProbTotal(pDefender), 0, 100);

		int y = iDefWithdrawal;
		z = iDefWithdrawal;

		for (int Time = expectedrndcnt * pDefender->earlyWithdrawTotal() / 100; Time > 0; --Time)
		{
			z += iDefWithdrawal * y / 100;
			y  = iDefWithdrawal * (100 - z) / 100; // Prob next round is prob per round times prob you haven't already
		}
	}
	const float DefRetreatOdds = std::min(z, 100) / 100.0f;

	// Fortify, Repel Odds
	{
		const int iRepel = (
			range(
				pDefender->repelVSOpponentProbTotal(pAttacker)
				+
				range(pDefender->fortifyRepelModifier() - pAttacker->overrunTotal(), 0, 100)
				-
				pAttacker->unyieldingTotal()
				, 0
				, 100
			)
		);
		int y = iRepel;
		z = iRepel;

		for (int Time = pDefender->repelRetriesTotal(); Time > -1; --Time)
		{
			z += iRepel * y / 100;
			y  = iRepel * (100 - z) / 100;	//	Prob next round is prob per round times prob you haven't already
		}
	}
	const float RepelOdds = std::min(z, 100) / 100.0f;
	//TB Combat Mods End

	const int AttFSnet = (pDefender->immuneToFirstStrikes() ? 0 : pAttacker->firstStrikes()) - (pAttacker->immuneToFirstStrikes() ? 0 : pDefender->firstStrikes());
	const int AttFSC = pDefender->immuneToFirstStrikes() ? 0 : pAttacker->chanceFirstStrikes();
	const int DefFSC = (pAttacker->immuneToFirstStrikes()) ? 0 : (pDefender->chanceFirstStrikes());

	const float P_A = ((float)iAttackerOdds) / GC.getDefineINT("COMBAT_DIE_SIDES");
	const float P_D = ((float)iDefenderOdds) / GC.getDefineINT("COMBAT_DIE_SIDES");
	float answer = 0.0f;
	// (1) Defender dies or is taken to combat limit
	if (n_A < N_A && n_D == iNeededRoundsAttacker)
	{
		float sum1 = 0.0f;
		for (int i = (-AttFSnet-AttFSC < 1 ? 1 : -AttFSnet - AttFSC); i <= DefFSC - AttFSnet; i++)
		{
			for (int j = 0; j <= i; j++)
			{
				if (n_A >= j)
				{
					sum1 += (float)getBinomialCoefficient(i,j) * pow(P_A,(float)(i-j)) * getBinomialCoefficient(iNeededRoundsAttacker-1+n_A-j,iNeededRoundsAttacker-1);
				}
			}
		}
		sum1 *= pow(P_D,(float)n_A)*pow(P_A,(float)iNeededRoundsAttacker);
		answer += sum1;

		float sum2 = 0.0f;

		for (int i = (0 < AttFSnet - DefFSC ? AttFSnet - DefFSC : 0); i <= AttFSnet + AttFSC; i++)
		{
			for (int j = 0; j <= i; j++)
			{
				if (N_D > j)
				{
					sum2 = sum2 + getBinomialCoefficient(n_A+iNeededRoundsAttacker-j-1,n_A) * (float)getBinomialCoefficient(i,j) * pow(P_A,(float)iNeededRoundsAttacker) * pow(P_D,(float)(n_A+i-j));
				}
				else if (n_A == 0)
				{
					sum2 = sum2 + (float)getBinomialCoefficient(i,j) * pow(P_A,(float)j) * pow(P_D,(float)(i-j));
				}
				else sum2 = sum2 + 0.0f;
			}
		}
		answer += sum2;
	}
	// (2) Attacker dies!
	else if (n_D < N_D && n_A == N_A)
	{
		float sum1 = 0.0f;
		for (int i = (-AttFSnet - AttFSC < 1 ? 1 : -AttFSnet - AttFSC); i <= DefFSC - AttFSnet; i++)
		{
			for (int j = 0; j <= i; j++)
			{
				if (N_A > j)
				{
					sum1 += getBinomialCoefficient(n_D+N_A-j-1,n_D) * (float)getBinomialCoefficient(i,j) * pow(P_D,(float)(N_A)) * pow(P_A,(float)(n_D+i-j));
				}
				else if (n_D == 0)
				{
					sum1 += (float)getBinomialCoefficient(i,j) * pow(P_D,(float)(j)) * pow(P_A,(float)(i-j));
				}
			}
		}
		answer += sum1;

		float sum2 = 0.0f;

		for (int i = (0 < AttFSnet-DefFSC ? AttFSnet - DefFSC : 0); i <= AttFSnet + AttFSC; i++)
		{
			for (int j = 0; j <= i; j++)
			{
				if (n_D >= j)
				{
					sum2 += (float)getBinomialCoefficient(i,j) * pow(P_D,(float)(i-j)) * getBinomialCoefficient(N_A-1+n_D-j,N_A-1);
				}
			}
		}
		sum2 *= pow(P_A,(float)(n_D))*pow(P_D,(float)(N_A));
		answer += sum2;
		//TB Combat Mods (Repel & Knockback)
		answer = answer * (1.0f - RetreatOdds) * (1.0f - RepelOdds) * (1.0f - KnockbackOdds) * (1.0f - DefRetreatOdds);
		//TB Combat Mods End
	}
	//TB Combat Mods - (3) Attacker retreats, is repelled or knocks opponent back!
	else if (n_A == (N_A-1) && n_D < N_D)
	{
		float sum1 = 0.0f;
		for (int i = (AttFSnet+AttFSC>-1?1:-AttFSnet-AttFSC); i <= DefFSC - AttFSnet; i++)
		{
			for (int j = 0; j <= i; j++)
			{
				if (N_A > j)
				{
					sum1 += getBinomialCoefficient(n_D+N_A-j-1,n_D) * (float)getBinomialCoefficient(i,j) * pow(P_D,(float)(N_A)) * pow(P_A,(float)(n_D+i-j));
				}
				else if (n_D == 0)
				{
					sum1 += (float)getBinomialCoefficient(i,j) * pow(P_D,(float)(j)) * pow(P_A,(float)(i-j));
				}
			}
		}
		answer += sum1;

		float sum2 = 0.0f;
		for (int i = (0 < AttFSnet-DefFSC ? AttFSnet-DefFSC : 0); i <= AttFSnet + AttFSC; i++)
		{
			for (int j = 0; j <= i; j++)
			{
				if (n_D >= j)
				{
					sum2 += (float)getBinomialCoefficient(i,j) * pow(P_D,(float)(i-j)) * getBinomialCoefficient(N_A-1+n_D-j,N_A-1);
				}
			}
		}
		sum2 *= pow(P_A,(float)(n_D))*pow(P_D,(float)(N_A));
		answer += sum2;
		//TB Combat Mods (Repel & Knockback)
		answer = answer * RetreatOdds * RepelOdds * KnockbackOdds * DefRetreatOdds;
		//TB Combat Mods End
	}
	else FErrorMsg("Unexpected value.  Process should not reach here.");

	// dividing by (t+w+1) as is necessary
	return answer / ((float)(AttFSC + DefFSC + 1));
}
// !getCombatOddsSpecific

// I had to add this function to the header file CvGameCoreUtils.h
/*************************************************************************************************/
/** ADVANCED COMBAT ODDS                      11/7/09                           PieceOfMind      */
/** END                                                                                          */
/*************************************************************************************************/

int getEspionageModifier(TeamTypes eOurTeam, TeamTypes eTargetTeam)
{
	FAssert(eOurTeam != eTargetTeam);
	FAssert(eOurTeam != BARBARIAN_TEAM);
	FAssert(eTargetTeam != BARBARIAN_TEAM);

	int iTargetPoints = GET_TEAM(eTargetTeam).getEspionagePointsEver();
	int iOurPoints = GET_TEAM(eOurTeam).getEspionagePointsEver();

	int iModifier = GC.getDefineINT("ESPIONAGE_SPENDING_MULTIPLIER") * (2 * iTargetPoints + iOurPoints);
	iModifier /= std::max(1, iTargetPoints + 2 * iOurPoints);
	return iModifier;
}

void setTradeItem(TradeData* pItem, TradeableItems eItemType, int iData)
{
	pItem->m_eItemType = eItemType;
	pItem->m_iData = iData;
	pItem->m_bOffering = false;
	pItem->m_bHidden = false;
}


TechTypes getDiscoveryTech(const UnitTypes eUnit, const PlayerTypes ePlayer)
{
	PROFILE_FUNC();
	FEnsureMsg(ePlayer != NO_PLAYER, "Player must be valid for this function");

	CvPlayerAI& player = GET_PLAYER(ePlayer);
	const CvTeam& team = GET_TEAM(player.getTeam());

	TechTypes eBestTech = NO_TECH;
	int iBestValue = 0;

	foreach_(const TechTypes eTechX, team.getAdjacentResearch())
	{
		if (player.canResearch(eTechX))
		{
			int iValue = 0;

			for (int iJ = 0; iJ < GC.getNumFlavorTypes(); iJ++)
			{
				iValue += GC.getTechInfo(eTechX).getFlavorValue(iJ) * GC.getUnitInfo(eUnit).getFlavorValue(iJ);
			}

			if (iValue > 0)
			{
				iValue *= 10;
				iValue += player.AI_TechValueCached(eTechX, player.isHumanPlayer());

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestTech = eTechX;
				}
			}
		}
	}
	return eBestTech;
}


void setListHelp(wchar_t* szBuffer, const wchar_t* szStart, const wchar_t* szItem, const wchar_t* szSeparator, bool bFirst)
{
	if (bFirst)
	{
		wcscat(szBuffer, szStart);
	}
	else
	{
		wcscat(szBuffer, szSeparator);
	}

	wcscat(szBuffer, szItem);
}

void setListHelp(CvWString& szBuffer, const wchar_t* szStart, const wchar_t* szItem, const wchar_t* szSeparator, bool bFirst)
{
	if (bFirst)
	{
		szBuffer += szStart;
	}
	else
	{
		szBuffer += szSeparator;
	}

	szBuffer += szItem;
}

void setListHelp(CvWStringBuffer& szBuffer, const wchar_t* szStart, const wchar_t* szItem, const wchar_t* szSeparator, bool bFirst)
{
	if (bFirst)
	{
		szBuffer.append(szStart);
	}
	else
	{
		szBuffer.append(szSeparator);
	}

	szBuffer.append(szItem);
}

bool PUF_isGroupHead(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return (pUnit->isGroupHead());
}

bool PUF_isPlayer(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	return (pUnit->getOwner() == iData1);
}

bool PUF_isTeam(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	return (pUnit->getTeam() == iData1);
}

bool PUF_isCombatTeam(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	FASSERT_NOT_NEGATIVE(iData2);

	return (GET_PLAYER(pUnit->getCombatOwner((TeamTypes)iData2, pUnit->plot())).getTeam() == iData1 && !pUnit->isInvisible((TeamTypes)iData2, false, false));
}

bool PUF_isOtherPlayer(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	return (pUnit->getOwner() != iData1);
}

bool PUF_isOtherTeam(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	const TeamTypes eTeam = GET_PLAYER((PlayerTypes)iData1).getTeam();
	if (pUnit->canCoexistWithTeamOnPlot(eTeam, *pUnit->plot()))
	{
		return false;
	}

	return (pUnit->getTeam() != eTeam);
}

bool PUF_isEnemy(const CvUnit* pUnit, int otherPlayer, int otherUnitAlwaysHostile, const CvUnit* otherUnit)
{
	FASSERT_NOT_NEGATIVE(otherPlayer);
	FASSERT_NOT_NEGATIVE(otherUnitAlwaysHostile);

	const TeamTypes eOtherTeam = GET_PLAYER((PlayerTypes)otherPlayer).getTeam();

	if (pUnit->canCoexistWithTeam(eOtherTeam))
	{
		return false;
	}

	const TeamTypes eOurTeam = GET_PLAYER(pUnit->getCombatOwner(eOtherTeam, pUnit->plot())).getTeam();

	return (otherUnitAlwaysHostile ? eOtherTeam != eOurTeam : atWar(eOtherTeam, eOurTeam));
}

bool PUF_isEnemyTarget(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	FASSERT_NOT_NEGATIVE(iData2);

	const TeamTypes eOtherTeam = GET_PLAYER((PlayerTypes)iData1).getTeam();

	if (pUnit->canCoexistWithTeam(eOtherTeam))
	{
		return false;
	}
	if (!pUnit->isTargetOf(*pThis))
	{
		return false;
	}

	const TeamTypes eOurTeam = GET_PLAYER(pUnit->getCombatOwner(eOtherTeam, pUnit->plot())).getTeam();

	return (iData2 ? eOtherTeam != eOurTeam : atWar(eOtherTeam, eOurTeam));
}

bool PUF_isParadrop(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return pUnit->canParadrop(pUnit->plot());
}

bool PUF_isNonAlly(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	FASSERT_NOT_NEGATIVE(iData2);

	const TeamTypes eOtherTeam = GET_PLAYER((PlayerTypes)iData1).getTeam();

	if (pUnit->canCoexistWithTeamOnPlot(eOtherTeam, *pUnit->plot()))
	{
		return false;
	}

	const TeamTypes eOurTeam = GET_PLAYER(pUnit->getCombatOwner(eOtherTeam, pUnit->plot())).getTeam();

	return (iData2 ? eOtherTeam == eOurTeam : isNonAlly(eOtherTeam, eOurTeam));
}

bool PUF_isVisible(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	return !pUnit->isInvisible(GET_PLAYER((PlayerTypes)iData1).getTeam(), false);
}

bool PUF_isVisibleDebug(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	return !pUnit->isInvisible(GET_PLAYER((PlayerTypes)iData1).getTeam(), true);
}

bool PUF_canSiege(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	return (pUnit->canSiege(GET_PLAYER((PlayerTypes)iData1).getTeam()));
}

bool PUF_isPotentialEnemy(const CvUnit* pDefender, int pAttackerTeam, int pAttackerAlwaysHostile, const CvUnit* pAttacker)
{
	FASSERT_NOT_NEGATIVE(pAttackerTeam);
	FASSERT_NOT_NEGATIVE(pAttackerAlwaysHostile);

	const bool bAssassinate = (pDefender->isAssassin() || pAttacker->isAssassin()) && pDefender->plot() == pAttacker->plot();

	if (pDefender->canCoexistWithAttacker(*pAttacker, false, bAssassinate))
	{
		return false;
	}
	const TeamTypes eAttackerTeam = GET_PLAYER((PlayerTypes)pAttackerTeam).getTeam();
	const TeamTypes eOurTeam = GET_PLAYER(pDefender->getCombatOwner(eAttackerTeam, pDefender->plot())).getTeam();
	return (pAttackerAlwaysHostile ? eAttackerTeam != eOurTeam : isPotentialEnemy(eAttackerTeam, eOurTeam));
}

bool PUF_canDeclareWar( const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	FASSERT_NOT_NEGATIVE(iData2);

	const TeamTypes eOtherTeam = GET_PLAYER((PlayerTypes)iData1).getTeam();

	if (pUnit->canCoexistWithTeamOnPlot(eOtherTeam, *pUnit->plot()))
	{
		return false;
	}

	const TeamTypes eOurTeam = GET_PLAYER(pUnit->getCombatOwner(eOtherTeam, pUnit->plot())).getTeam();

	return (iData2 ? false : GET_TEAM(eOtherTeam).canDeclareWar(eOurTeam));
}

bool PUF_canDefend(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return pUnit->canDefend();
}

bool PUF_isCriminal(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return (pUnit->getInsidiousnessTotal(true) > 0);
}

bool PUF_isWantedCriminal(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return (pUnit->isWanted());
}

bool PUF_isCityGarrison(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	if ( pUnit->getGroup()->AI_isCityGarrison(GET_PLAYER(pUnit->getOwner()).getCity(iData1)) )
	{
		const bool bAllowAnyDefenders = (bool)iData2;

		if ( bAllowAnyDefenders )
		{
			return true;
		}
		else
		{
			const UnitAITypes eUnitAI = pUnit->AI_getUnitAIType();

			return !pUnit->noDefensiveBonus() && eUnitAI != UNITAI_ATTACK_CITY && eUnitAI != UNITAI_ATTACK;
		}
	}

	return false;
}

bool PUF_cannotDefend(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return !(pUnit->canDefend());
}

bool PUF_canDefendGroupHead(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return (PUF_canDefend(pUnit, iData1, iData2) && PUF_isGroupHead(pUnit, iData1, iData2));
}

bool PUF_canDefendEnemyNoAnimal(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	FASSERT_NOT_NEGATIVE(iData2);
	return (!pUnit->isAnimal() && PUF_canDefend(pUnit, iData1, iData2) && PUF_isEnemy(pUnit, iData1, iData2));
}

bool PUF_canDefendEnemy(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	FASSERT_NOT_NEGATIVE(iData2);
	return (PUF_canDefend(pUnit, iData1, iData2) && PUF_isEnemy(pUnit, iData1, iData2));
}

bool PUF_canDefendPotentialEnemyAgainst(const CvUnit* pUnit, int otherTeam, int otherAlwaysHostile, const CvUnit* otherUnit)
{
	FASSERT_NOT_NEGATIVE(otherTeam);
	return (PUF_canDefend(pUnit) && PUF_isPotentialEnemy(pUnit, otherTeam, otherAlwaysHostile, otherUnit));
}

bool PUF_canDefenselessPotentialEnemyAgainst(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	return ((!PUF_canDefend(pUnit, iData1, iData2)) && PUF_isPotentialEnemy(pUnit, iData1, iData2, pThis));
}

bool PUF_canAirAttack(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return pUnit->canAirAttack();
}

bool PUF_canAirDefend(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return pUnit->canAirDefend();
}

bool PUF_isInBattle(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return pUnit->isInBattle();
}

bool PUF_isAnimal( const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return pUnit->isAnimal();
}

bool PUF_isMilitaryHappiness(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return pUnit->isMilitaryHappiness();
}

bool PUF_isInvestigate(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return pUnit->isInvestigate();
}

bool PUF_isCounterSpy(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return pUnit->isCounterSpy();
}

bool PUF_isSpy(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return pUnit->isSpy();
}

bool PUF_isDomainType(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	return (pUnit->getDomainType() == iData1);
}

bool PUF_isUnitType(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	return (pUnit->getUnitType() == iData1);
}

bool PUF_isHealUnitCombatType(const CvUnit* pUnit, /*UnitCombatType*/int iData1, /*DomainType*/int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	return ((pUnit->isHasUnitCombat((UnitCombatTypes)iData1)) && (iData2 == NO_DOMAIN ? true : (pUnit->getDomainType() == (DomainTypes)iData2)));
}

bool PUF_isMountainLeader(const CvUnit* pUnit, /*Null*/int iData1, /*Null*/int iData2, const CvUnit* pThis)
{
	return (pUnit->isCanLeadThroughPeaks());
}

bool PUF_isInjuredUnitCombatType(const CvUnit* pUnit, /*UnitCombatType*/int iData1, /*DomainType*/int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	return ((pUnit->isHasUnitCombat((UnitCombatTypes)iData1) && pUnit->getDamage() > 0) && (iData2 == NO_DOMAIN ? true : (pUnit->getDomainType() == (DomainTypes)iData2)));
}

bool PUF_isUnitAIType(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	return (pUnit->AI_getUnitAIType() == iData1);
}

bool PUF_isCityAIType(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return pUnit->AI_isCityAIType();
}

bool PUF_isNotCityAIType(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return !(PUF_isCityAIType(pUnit, iData1, iData2));
}

bool PUF_isSelected(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return pUnit->IsSelected();
}

bool PUF_makeInfoBarDirty(CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	pUnit->setInfoBarDirty(true);
	return true;
}

bool PUF_isNoMission(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return (pUnit->getGroup()->getActivityType() != ACTIVITY_MISSION);
}

bool PUF_isMissionAI(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return (pUnit->getGroup()->AI_getMissionAIType() == (MissionAITypes)iData1);
}

/********************************************************************************/
/* 	Worker Counting						03.08.2010				Fuyu			*/
/********************************************************************************/
bool PUF_isNoMissionAI(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return (pUnit->getGroup()->AI_getMissionAIType() == NO_MISSIONAI);
}
/********************************************************************************/
/* 	Worker Counting												END 			*/
/********************************************************************************/

bool PUF_isFiniteRange(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return ((pUnit->getDomainType() != DOMAIN_AIR) || (pUnit->getUnitInfo().getAirRange() > 0));
}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      01/15/09                                jdog5000      */
/*                                                                                              */
/* General AI                                                                                   */
/************************************************************************************************/
bool PUF_isAvailableUnitAITypeGroupie(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return ((PUF_isUnitAITypeGroupie(pUnit,iData1,iData2)) && !(pUnit->isCargo()));
}

bool PUF_isUnitAITypeGroupie(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	const CvUnit* pGroupHead = pUnit->getGroup()->getHeadUnit();
	return (PUF_isUnitAIType(pGroupHead,iData1,iData2));
}

bool PUF_isFiniteRangeAndNotJustProduced(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	return (PUF_isFiniteRange(pUnit,iData1,iData2) && ((GC.getGame().getGameTurn() - pUnit->getGameTurnCreated()) > 1));
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

/************************************************************************************************/
/* Afforess	                  Start		 08/19/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
bool PUF_isMissionary(const CvUnit* pUnit, int /*ReligionTypes*/ iData1, int /*PlayerTypes*/ iData2, const CvUnit* pThis)
{
	if (iData2 != -1 && pUnit->getOwner() != iData2)
	{
		return false;
	}
	if (!pUnit->canSpread(pUnit->plot(), (ReligionTypes)iData1, false))
	{
		return false;
	}
	return true;
}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
#ifdef OUTBREAKS_AND_AFFLICTIONS
bool PUF_isAfflicted(const CvUnit* pUnit, int /*PromotionLineTypes*/ iData1, int iData2, const CvUnit* pThis)
{
	return pUnit->hasAfflictionLine((PromotionLineTypes)iData1);
}
#endif

bool PUF_isTunneledEnemy(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	FASSERT_NOT_NEGATIVE(iData2);

	const TeamTypes eOtherTeam = GET_PLAYER((PlayerTypes)iData1).getTeam();
	const TeamTypes eOurTeam = GET_PLAYER(pUnit->getCombatOwner(eOtherTeam, pUnit->plot())).getTeam();

	bool bAnswer = (iData2 ? eOtherTeam != eOurTeam : atWar(eOtherTeam, eOurTeam));

	if (bAnswer)
	{
		if (pUnit->getDomainType() == DOMAIN_SEA || pUnit->canMoveAllTerrain())
		{
			bAnswer = false;
		}
	}
	return (bAnswer);
}

bool PUF_isNonTunneledEnemy(const CvUnit* pUnit, int iData1, int iData2, const CvUnit* pThis)
{
	FASSERT_NOT_NEGATIVE(iData1);
	FASSERT_NOT_NEGATIVE(iData2);

	const TeamTypes eOtherTeam = GET_PLAYER((PlayerTypes)iData1).getTeam();
	const TeamTypes eOurTeam = GET_PLAYER(pUnit->getCombatOwner(eOtherTeam, pUnit->plot())).getTeam();

	bool bAnswer = (iData2 ? eOtherTeam != eOurTeam : atWar(eOtherTeam, eOurTeam));

	if (bAnswer)
	{
		if (pUnit->getDomainType() == DOMAIN_LAND && !pUnit->canMoveAllTerrain())
		{
			bAnswer = false;
		}
	}
	return (bAnswer);
}

int potentialIrrigation(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	if (parent == NULL)
	{
		return TRUE;
	}

	return ((GC.getMap().plotSorenINLINE(node->m_iX, node->m_iY)->isPotentialIrrigation()) ? TRUE : FALSE);
}


int checkFreshWater(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	if (data == ASNL_ADDCLOSED)
	{
		if (GC.getMap().plotSorenINLINE(node->m_iX, node->m_iY)->isFreshWater())
		{
			*((bool *)pointer) = true;
		}
	}

	return 1;
}


int changeIrrigated(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	if (data == ASNL_ADDCLOSED)
	{
		GC.getMap().plotSorenINLINE(node->m_iX, node->m_iY)->setIrrigated(*((bool *)pointer));
	}
	return 1;
}

int pathDestValid(int iToX, int iToY, const void* pointer, FAStar* finder)
{
	PROFILE_FUNC();

	const CvPlot* pToPlot = GC.getMapExternal().plot(iToX, iToY);
	const CvPlot* pFromPlot = GC.getMapExternal().plot(gDLL->getFAStarIFace()->GetStartX(finder), gDLL->getFAStarIFace()->GetStartY(finder));

	//	Safety valve since minidumps have shown that this (unknown how) can still occassionally
	//	happen (attempt to generate a path that starts or ends off the viewport)
	if (pToPlot == NULL || pFromPlot == NULL)
	{
		FErrorMsg("Both plots must be valid");
		return false;
	}

	CvSelectionGroup* pSelectionGroup = ((CvSelectionGroup *)pointer);

	if (pSelectionGroup->atPlot(pToPlot))
	{
		return true;
	}

	if (pSelectionGroup->getDomainType() == DOMAIN_IMMOBILE)
	{
		return false;
	}

	//OutputDebugString(CvString::format("PathDestValid (%d,%d)\n", iToX, iToY).c_str());
	//TB OOS Debug
	if (!pSelectionGroup->AI_isControlled() || GET_PLAYER(pSelectionGroup->getHeadOwner()).isHumanPlayer())
	{
		gDLL->getFAStarIFace()->ForceReset(finder);
#ifdef PATHFINDING_CACHE
		CvSelectionGroup::setGroupToCacheFor(pSelectionGroup);
#endif // PATHFINDING_CACHE
		return pSelectionGroup->generatePath(pFromPlot, pToPlot, gDLL->getFAStarIFace()->GetInfo(finder), false, NULL, MAX_INT);
	}
	bool bDummy;
	return NewPathDestValid(pSelectionGroup, iToX, iToY, gDLL->getFAStarIFace()->GetInfo(finder), bDummy);
}


int pathHeuristic(int iFromX, int iFromY, int iToX, int iToY)
{
	return plotDistance(iFromX, iFromY, iToX, iToY) * PATH_MOVEMENT_WEIGHT;
}

bool pathValidInternal(const CvPlot* pPlot, bool bCheckVisibleDanger, const CvSelectionGroup* pSelectionGroup, int iFlags)
{
	if (pSelectionGroup->atPlot(pPlot))
	{
		return true;
	}

	// Unrevealed tiles can only be terminal nodes or else we're expoiting knowledge of land mass shapes that we should not have
	if (!pPlot->isRevealed(pSelectionGroup->getHeadTeam(), false))
	{
		return false;
	}

	if (!moveToValid(pSelectionGroup, pPlot, iFlags))
	{
		return false;
	}

	if (bCheckVisibleDanger && GET_PLAYER(pSelectionGroup->getHeadOwner()).AI_getAnyPlotDanger(pPlot))
	{
		return false;
	}

	return true;
}


int pathValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	PROFILE_FUNC();

	if (parent == NULL)
	{
		return TRUE;
	}

	const CvPlot* pFromPlot = GC.getMapExternal().plot(parent->m_iX, parent->m_iY);
	const CvPlot* pToPlot = GC.getMapExternal().plot(node->m_iX, node->m_iY);

	const CvSelectionGroup* pSelectionGroup = ((CvSelectionGroup *)pointer);

#ifdef USE_OLD_PATH_GENERATOR
	// XXX might want to take this out...
	if (pSelectionGroup->getDomainType() == DOMAIN_SEA)
	{
		PROFILE("pathValid domain sea");

//#if 0
//		//	Optimisation short-circuit some invalid pathing choices quickly
//		if (!pToPlot->isWater() && !pSelectionGroup->canMoveAllTerrain() && !pToPlot->isCanMoveSeaUnits())
//		{
//			if (!pToPlot->isCity())
//			{
//				return FALSE;
//			}
//		}
//#endif
		//	Can't cross diagonally across 'land'
		if (pFromPlot->isWater() && pToPlot->isWater())
		{
			if (!(GC.getMap().plot(pFromPlot->getX(), pToPlot->getY())->isWater()) && !(GC.getMap().plot(pToPlot->getX(), pFromPlot->getY())->isWater()))
			{
				if( !(pSelectionGroup->canMoveAllTerrain()) )
				{
					return FALSE;
				}
			}
		}
	}

	//	KOSHLING MOD - none of the rest of the calculation depends on pToPlot,
	//	so we can cache the results from one request for each parent/pFromPlot.
	bool bCheckNonInvisibleDanger = !(gDLL->getFAStarIFace()->GetInfo(finder) & MOVE_IGNORE_DANGER) &&
									pSelectionGroup->AI_isControlled() &&
									((parent->m_iData2 > 1) || (parent->m_iData1 == 0));

	bool bResult;
	if ( !pFromPlot->HaveCachedPathValidityResult( pSelectionGroup, bCheckNonInvisibleDanger, bResult ) )
	{
		bResult = pathValidInternal(pFromPlot, bCheckNonInvisibleDanger, pSelectionGroup, gDLL->getFAStarIFace()->GetInfo(finder));

		pFromPlot->CachePathValidityResult( pSelectionGroup, bCheckNonInvisibleDanger, bResult );
	}

	return bResult;
#else
	bool bResult;

	if ( pFromPlot != NULL && pToPlot != NULL )
	{
		bResult = ((CvSelectionGroup *)pointer)->getPath().containsNode(pFromPlot) || ((CvSelectionGroup *)pointer)->getPath().containsNode(pToPlot);
	}
	else
	{
		bResult = false;
	}

	//OutputDebugString(CvString::format("PathValid (%d,%d)->(%d,%d): [%d]\n", pFromPlot->getX(), pFromPlot->getY(), pToPlot->getX(), pToPlot->getY(),bResult).c_str());
	if (!pSelectionGroup->AI_isControlled())
	{
		//OutputDebugString("Force reset finder\n");
		gDLL->getFAStarIFace()->ForceReset(finder);
	}

	return bResult;
#endif
}


int pathAdd(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	PROFILE_FUNC();

	const CvSelectionGroup* pSelectionGroup = ((const CvSelectionGroup*)pointer);
	FAssert(pSelectionGroup->getNumUnits() > 0);

	int iTurns = 1;
	int iMoves = MAX_INT;

	if (data == ASNC_INITIALADD)
	{
		const bool bMaxMoves = (gDLL->getFAStarIFace()->GetInfo(finder) & MOVE_MAX_MOVES);
		if (bMaxMoves)
		{
			iMoves = 0;
		}

		foreach_(const CvUnit* pLoopUnit, pSelectionGroup->units())
		{
			if (bMaxMoves)
			{
				iMoves = std::max(iMoves, pLoopUnit->maxMoves());
			}
			else
			{
				iMoves = std::min(iMoves, pLoopUnit->movesLeft());
			}
		}
	}
	else
	{
		const CvPlot* pFromPlot = GC.getMapExternal().plot(parent->m_iX, parent->m_iY);
		FAssertMsg(pFromPlot != NULL, "FromPlot is not assigned a valid value");
		const CvPlot* pToPlot = GC.getMapExternal().plot(node->m_iX, node->m_iY);
		FAssertMsg(pToPlot != NULL, "ToPlot is not assigned a valid value");

		const int iStartMoves = parent->m_iData1;
		iTurns = parent->m_iData2;
		if (iStartMoves == 0)
		{
			iTurns++;
		}

		iMoves = pSelectionGroup->movesRemainingAfterMovingTo((iStartMoves == 0 ? -1 : iStartMoves), pFromPlot, pToPlot);

//#if 0
//		if ( gDLL->getFAStarIFace()->GetDestX(finder) == pToPlot->getX() && gDLL->getFAStarIFace()->GetDestY(finder) == pToPlot->getY() )
//		{
//			if (!pSelectionGroup->AI_isControlled())
//			{
//				OutputDebugString("Force reset find at possible end of path\n");
//				gDLL->getFAStarIFace()->ForceReset(finder);
//			}
//		}
//#endif
	}

	FASSERT_NOT_NEGATIVE(iMoves);

	node->m_iData1 = iMoves;
	node->m_iData2 = iTurns;

//#if 0
//	if ( parent != NULL )
//	{
//		OutputDebugString(CvString::format("PathAdd (%d,%d)->(%d,%d)\n", parent->m_iX, parent->m_iY, node->m_iX, node->m_iY).c_str());
//	}
//	else
//	{
//		OutputDebugString(CvString::format("PathAdd NULL->(%d,%d)\n", node->m_iX, node->m_iY).c_str());
//	}
//#endif

	return 1;
}




int pathCost(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	const CvPlot* pFromPlot = GC.getMapExternal().plot(parent->m_iX, parent->m_iY);
	FAssert(pFromPlot != NULL);
	const CvPlot* pToPlot = GC.getMapExternal().plot(node->m_iX, node->m_iY);
	FAssert(pToPlot != NULL);

#ifdef USE_OLD_PATH_GENERATOR
	PROFILE_FUNC();

	int iCost;
	int iMovesLeft;
	int iMax;

	int iWorstCost = MAX_INT;
	int iWorstMovesLeft = MAX_INT;
	int iWorstMax = MAX_INT;

	int iWorstMovement = MAX_INT;
	int iLargestBaseCost = 0;
	int iSmallestBaseCost = MAX_INT;
	int iLargestMax = 0;
	int iMaxTerrainDamage = 0;
	static bool bHasAlwaysHostileUnit;
	static bool bHasCanFightUnit;
	static bool bHasCanAttackUnit;
	static bool bHasDefensiveBonusUnit;
	static CvSelectionGroup* gLastSelectionGroup = NULL;
	bool bHasVisibleEnemy = false;
#define	MAX_UNIQUE_UNIT_CATEGORIES_CONSIDERED	3
	int	unitCharacteristics[MAX_UNIQUE_UNIT_CATEGORIES_CONSIDERED];
	int	movesLeft[MAX_UNIQUE_UNIT_CATEGORIES_CONSIDERED];
	int numUniqueUnitCategories = 0;
	bool bTrace = false;
	static bool gTracePathSummary = false;
	TeamTypes eTeam = pSelectionGroup->getHeadTeam();
	bool bIsAIControlled = pSelectionGroup->AI_isControlled();
	bool bUseAIPathing = bIsAIControlled;
	int iNodeCost = 0;
	int iEdgeCost = 0;
	int iCachedEndTurnEdgeCost;
	int iCachedNonEndTurnEdgeCost;
	int iCachedEndTurnNodeCost;
	int iCachedNonEndTurnNodeCost;
	int iExtraNodeCost = 0;
	int iExtraEdgeCost = 0;
	bool bHaveNonEndTurnCachedEdgeValue = false;
	bool bHaveEndTurnCachedEdgeValue = false;
	bool bCheckedEndTurnEdgeCache = false;
	bool bCheckedNonEndTurnEdgeCache = false;
	bool bIsTerminalNode = gDLL->getFAStarIFace()->IsPathDest(finder, pToPlot->getX(), pToPlot->getY());
	bool bEndsTurn = false;

	if ( parent->m_iData1 == 0 || parent->m_iData1 > 2*GC.getMOVE_DENOMINATOR() )
	{
		bHaveNonEndTurnCachedEdgeValue = pSelectionGroup->HaveCachedPathEdgeCosts(pFromPlot, pToPlot, false, iCachedNonEndTurnEdgeCost, iSmallestBaseCost, iLargestBaseCost, iCachedNonEndTurnNodeCost );
		bCheckedNonEndTurnEdgeCache = true;
	}
	else
	{
		bHaveEndTurnCachedEdgeValue = pSelectionGroup->HaveCachedPathEdgeCosts(pFromPlot, pToPlot, true, iCachedEndTurnEdgeCost, iSmallestBaseCost, iLargestBaseCost, iCachedEndTurnNodeCost );
		bCheckedEndTurnEdgeCache = true;
	}

	if ( bCheckedNonEndTurnEdgeCache && !bHaveNonEndTurnCachedEdgeValue )
	{
		bHaveEndTurnCachedEdgeValue = pSelectionGroup->HaveCachedPathEdgeCosts(pFromPlot, pToPlot, true, iCachedEndTurnEdgeCost, iSmallestBaseCost, iLargestBaseCost, iCachedEndTurnNodeCost );
		bCheckedEndTurnEdgeCache = true;
	}
	else if ( bCheckedEndTurnEdgeCache && !bHaveEndTurnCachedEdgeValue )
	{
		bHaveNonEndTurnCachedEdgeValue = pSelectionGroup->HaveCachedPathEdgeCosts(pFromPlot, pToPlot, false, iCachedNonEndTurnEdgeCost, iSmallestBaseCost, iLargestBaseCost, iCachedNonEndTurnNodeCost );
		bCheckedNonEndTurnEdgeCache = true;
	}

	{
		bool bDoesntEndTurn = true;

		//	Either cache has valid movement costs
		if ( bHaveEndTurnCachedEdgeValue || bHaveNonEndTurnCachedEdgeValue )
		{
			int iMaxMovesLeft = 0;

			//PROFILE("pathCost.CachedData");

			if (parent->m_iData1 > 0)
			{
				iMaxMovesLeft = parent->m_iData1;
			}
			else
			{
				foreach_(const CvUnit* pLoopUnit, pSelectionGroup->units())
				{
					if (parent->m_iData1 > 0)
					{
						iMax = parent->m_iData1;
					}
					else
					{
						iMax = pLoopUnit->maxMoves();
					}

					if ( iMax > iMaxMovesLeft )
					{
						iMaxMovesLeft = iMax;
					}

					if ( iMax <= iSmallestBaseCost )
					{
						bEndsTurn = true;
						break;
					}
					else if ( iMax <= iLargestBaseCost )
					{
						bDoesntEndTurn = false;
					}
				}
			}

			if ( iMaxMovesLeft <= iLargestBaseCost )
			{
				bEndsTurn = true;
			}
			else if ( bDoesntEndTurn )
			{
				iLargestMax = iMaxMovesLeft - iLargestBaseCost;
			}

			if ( bEndsTurn )
			{
//#if 0
//				PROFILE("pathCost.endTurnDetermined");
//
//				if ( bHaveEndTurnCachedEdgeValue )
//				{
//					PROFILE("pathCost.endTurnDetermined.Cached");
//				}
//#endif
				bDoesntEndTurn = false;
			}
//#if 0
//			else if ( bDoesntEndTurn )
//			{
//				PROFILE("pathCost.nonEndTurnDetermined");
//
//				if ( bHaveNonEndTurnCachedEdgeValue )
//				{
//					PROFILE("pathCost.NonEndTurnDetermined.Cached");
//				}
//			}
//#endif
		}
		else
		{
			bDoesntEndTurn = false;
		}

		if ( bEndsTurn && !bCheckedEndTurnEdgeCache )
		{
			bHaveEndTurnCachedEdgeValue = pSelectionGroup->HaveCachedPathEdgeCosts(pFromPlot, pToPlot, true, iCachedEndTurnEdgeCost, iSmallestBaseCost, iLargestBaseCost, iCachedEndTurnNodeCost );
		}
		else if ( !bEndsTurn && !bCheckedNonEndTurnEdgeCache )
		{
			bHaveNonEndTurnCachedEdgeValue = pSelectionGroup->HaveCachedPathEdgeCosts(pFromPlot, pToPlot, false, iCachedNonEndTurnEdgeCost, iSmallestBaseCost, iLargestBaseCost, iCachedNonEndTurnNodeCost );
		}

		//	Do we need to calculate the base characteristics or do we have everything we neeed cached?
		if ( (!bEndsTurn || !bHaveEndTurnCachedEdgeValue) && (!bDoesntEndTurn || !bHaveNonEndTurnCachedEdgeValue) )
		{
			//PROFILE("pathCost.BaseCharacteristics");

			if ( gLastSelectionGroup != pSelectionGroup )
			{
				bHasAlwaysHostileUnit = false;
				bHasCanFightUnit = false;
				bHasCanAttackUnit = false;
				bHasDefensiveBonusUnit = false;

				gLastSelectionGroup = pSelectionGroup;
			}

			foreach_(const CvUnit* pLoopUnit, pSelectionGroup->units())
			{
				if (parent->m_iData1 > 0)
				{
					iMax = parent->m_iData1;
				}
				else
				{
					iMax = pLoopUnit->maxMoves();
				}

				int unitMovementCharacteristics = pLoopUnit->getMovementCharacteristicsHash();

				//	If we've already considred a unit with these characteristics no need to
				//	check this one too
				for(int iI = 0; iI < numUniqueUnitCategories; iI++)
				{
					if ( unitMovementCharacteristics == unitCharacteristics[iI] )
					{
						break;
					}
				}

				if ( iI == numUniqueUnitCategories )
				{
					if ( !bHasAlwaysHostileUnit && pLoopUnit->isAlwaysHostile(NULL) )
					{
						bHasAlwaysHostileUnit = true;
					}

					if ( !bHasCanFightUnit && pLoopUnit->canFight())
					{
						bHasCanFightUnit = true;
					}

					if ( bHasCanFightUnit && !bHasCanAttackUnit && pLoopUnit->canAttack())
					{
						bHasCanAttackUnit = true;
					}

					if (bHasCanAttackUnit && !bHasVisibleEnemy && pToPlot->isVisibleEnemyDefender(pLoopUnit))
					{
						bHasVisibleEnemy = true;
					}

					if ( !bHasDefensiveBonusUnit && !pLoopUnit->noDefensiveBonus() )
					{
						bHasDefensiveBonusUnit = true;
					}

					if ( pToPlot->getTerrainTurnDamage(pLoopUnit) > iMaxTerrainDamage )
					{
						iMaxTerrainDamage = pToPlot->getTerrainTurnDamage(pLoopUnit);
					}

					iCost = pToPlot->movementCost(pLoopUnit, pFromPlot);

					iMovesLeft = std::max(0, (iMax - iCost));

					if ( iMovesLeft < iWorstMovement )
					{
						iWorstMovement = iMovesLeft;
					}

					if ( iMax > iLargestMax )
					{
						iLargestMax = iMax;
					}

					if ( iCost > iLargestBaseCost )
					{
						iLargestBaseCost = iCost;
					}

					if ( iCost < iSmallestBaseCost )
					{
						iSmallestBaseCost = iCost;
					}

					if ( numUniqueUnitCategories < MAX_UNIQUE_UNIT_CATEGORIES_CONSIDERED )
					{
						movesLeft[numUniqueUnitCategories] = iMax;
						unitCharacteristics[numUniqueUnitCategories++] = unitMovementCharacteristics;
					}
				}
				else if ( movesLeft[iI] > iMax )
				{
					iCost = pToPlot->movementCost(pLoopUnit, pFromPlot);

					iMovesLeft = std::max(0, (iMax - iCost));

					if ( iMovesLeft < iWorstMovement )
					{
						iWorstMovement = iMovesLeft;
					}

					movesLeft[iI] = iMovesLeft;
				}
			}
		}
		else
		{
			if ( bEndsTurn )
			{
				iWorstMovement = 0;
			}
		}
	}

	iCost = iLargestBaseCost;

	bool bIsEndTurn = (iWorstMovement == 0 || bIsTerminalNode);

	if ( bTrace )
	{
		OutputDebugString(CvString::format("Base cost (%d,%d)->(%d,%d): %d\n", pFromPlot->getX(), pFromPlot->getY(), pToPlot->getX(), pToPlot->getY(), iCost).c_str());
	}

	//	Node costs
	if ( bIsEndTurn )
	{
		//PROFILE("pathCost.EndTurn.Node");

		if (!bHaveEndTurnCachedEdgeValue)
		{
			if (pToPlot->getTeam() != eTeam)
			{
				iExtraNodeCost += PATH_TERRITORY_WEIGHT;
			}

			// Damage caused by features (mods)
			if (0 != GC.getPATH_DAMAGE_WEIGHT())
			{
				if (pToPlot->getFeatureType() != NO_FEATURE)
				{
					iExtraNodeCost += (GC.getPATH_DAMAGE_WEIGHT() * std::max(0, GC.getFeatureInfo(pToPlot->getFeatureType()).getTurnDamage())) / GC.getMAX_HIT_POINTS();
				}

				if (iMaxTerrainDamage > 0)
				{
					iExtraNodeCost += (GC.getPATH_DAMAGE_WEIGHT() * std::max(0, iMaxTerrainDamage * 2)) / GC.getMAX_HIT_POINTS();
				}
			}

			// Add additional cost for ending turn in or adjacent to enemy territory based on flags
			if (gDLL->getFAStarIFace()->GetInfo(finder) & MOVE_AVOID_ENEMY_WEIGHT_3)
			{
				if (pToPlot->isOwned() && ((GET_TEAM(eTeam).AI_getWarPlan(pToPlot->getTeam()) != NO_WARPLAN) || (pToPlot->getTeam() != eTeam && bHasAlwaysHostileUnit)))
				{
					iNodeCost += iCost*3;
				}
				else
				{
					foreach_(const CvPlot* pAdjacentPlot, pToPlot->adjacent())
					{
						if (pAdjacentPlot->isOwned() && (atWar(pAdjacentPlot->getTeam(), eTeam) || (pAdjacentPlot->getTeam() != eTeam && bHasAlwaysHostileUnit)))
						{
							iNodeCost += (iCost*3)/2;
						}
					}
				}
			}
			else if (gDLL->getFAStarIFace()->GetInfo(finder) & MOVE_AVOID_ENEMY_WEIGHT_2)
			{
				if (pToPlot->isOwned() && ((GET_TEAM(eTeam).AI_getWarPlan(pToPlot->getTeam()) != NO_WARPLAN) || (pToPlot->getTeam() != eTeam && bHasAlwaysHostileUnit)))
				{
					iNodeCost += iCost*2;
				}
				else
				{
					foreach_(const CvPlot* pAdjacentPlot, pToPlot->adjacent())
					{
						if (pAdjacentPlot->isOwned() && (atWar(pAdjacentPlot->getTeam(), eTeam) || (pAdjacentPlot->getTeam() != eTeam && bHasAlwaysHostileUnit)))
						{
							iNodeCost += (iCost*4)/3;
						}
					}
				}
			}

			if (bHasCanFightUnit && bUseAIPathing)
			{
				int iDefenseCost = (PATH_DEFENSE_WEIGHT * std::max(0, (200 - (!bHasDefensiveBonusUnit ? 0 : pSelectionGroup->defensiveModifierAtPlot(pToPlot)))))/100;

				//	Take more care to path defensively in neutral territory, and still more in enemy territory
				if ( pToPlot->getTeam() == NO_TEAM )
				{
					iDefenseCost *= 2;
				}
				else if ( atWar(pToPlot->getTeam(), eTeam) )
				{
					iDefenseCost *= 4;
				}
				if ( bTrace )
				{
					OutputDebugString(CvString::format("End turn defense cost: %d\n", iDefenseCost).c_str());
				}

				iExtraNodeCost += iDefenseCost;

				//	If we finish a move next to an enemy city or fort accout for city defense damage and add
				//	a ganeral avoidance cost
				int iCityAdjacencyCost = 0;

				foreach_(const CvPlot* pAdjacentPlot, pToPlot->adjacent())
				{
					if((const CvCity* pAdjacentCity = pAdjacentPlot->getPlotCity()) != NULL || pAdjacentPlot->isActsAsCity())
					{
						if ( atWar(pAdjacentPlot->getTeam(), eTeam) )
						{
							if ( pAdjacentCity != NULL )
							{
								iCityAdjacencyCost +=  (2 * GC.getPATH_DAMAGE_WEIGHT() * std::max(0, pAdjacentCity->getAdjacentDamagePercent())) / GC.getMAX_HIT_POINTS();
							}

							iCityAdjacencyCost += PATH_ENEMY_CITY_WEIGHT;
						}
					}
				}

				if ( bTrace )
				{
					OutputDebugString(CvString::format("End turn city adjacency cost: %d\n", iCityAdjacencyCost).c_str());
				}

				iExtraNodeCost += iCityAdjacencyCost;

				//	If this is the end of the first turn (only) also evaluate whether we end
				//	up next to enemy stack that look dangerous and cost that in
				//
				//	Sadly the current game pathing engine can't cope with eavluating costs in a way
				//	dependent on the turn count into a path because once calculated it wil cache the
				//	edge traversal cost, and mis-use it in another context, so we accoutn the cost for all visible
				//	enemy units wherever they occur in the path
				//if ( parent->m_iData2 == 1 && parent->m_iData1 != 0 )//&& !gDLL->getFAStarIFace()->IsPathDest(finder, pToPlot->getX(), pToPlot->getY()) )
				{
					if ( bTrace )
					{
						OutputDebugString(CvString::format("Evaluating end-turn unit danger\n").c_str());
					}

					int iUnitAdjacencyCost = 0;

					foreach_(const CvPlot* pAdjacentPlot, pToPlot->adjacent())
					{
						if( pSelectionGroup->getArea() == pAdjacentPlot->getArea() &&
							pAdjacentPlot->getVisibleEnemyDefender(pSelectionGroup->getHeadOwner()))
						{
							int iRatioToUnitStack = pSelectionGroup->AI_compareStacks(pAdjacentPlot, false);

							if ( iRatioToUnitStack < 120 )
							{
								iUnitAdjacencyCost += (PATH_MOVEMENT_WEIGHT * 100)/std::max(1,iRatioToUnitStack);
							}
						}
					}

					if ( bTrace )
					{
						OutputDebugString(CvString::format("End turn unit adjacency cost: %d\n", iUnitAdjacencyCost).c_str());
					}

					iExtraNodeCost += iUnitAdjacencyCost;
				}
			}

			iNodeCost += iExtraNodeCost*GC.getMOVE_DENOMINATOR();
		}
		else
		{
			iNodeCost = iCachedEndTurnNodeCost;
		}
	}
	else
	{
		//	There are no non-end-turn node costs currently
	}

	//	Edge costs
	if ( bIsEndTurn )
	{
		//PROFILE("pathCost.EndTurn.Edge");

		if (!bHaveEndTurnCachedEdgeValue)
		{
			//	Account for artifical turn end due to reaching destination
			iEdgeCost += (PATH_MOVEMENT_WEIGHT * (iLargestMax + ((iWorstMovement == 0 || !bIsAIControlled) ? 0 : iCost)));

			if (bUseAIPathing)
			{
				if (bHasCanAttackUnit)
				{
					if (bIsTerminalNode)
					{
						if (bHasVisibleEnemy)
						{
							iExtraEdgeCost += (PATH_DEFENSE_WEIGHT * std::max(0, (200 - (!bHasDefensiveBonusUnit ? 0 : pFromPlot->defenseModifier(eTeam, false)))))/100;

							if (!(pFromPlot->isCity()))
							{
								iExtraEdgeCost += PATH_CITY_WEIGHT;
							}

							if (pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot)))
							{
								if (!(pLoopUnit->isRiver()))
								{
									iExtraEdgeCost += (PATH_RIVER_WEIGHT * -(GC.getRIVER_ATTACK_MODIFIER()))/100;
									iExtraEdgeCost += (PATH_MOVEMENT_WEIGHT * iMovesLeft)/GC.getMOVE_DENOMINATOR();
								}
							}
						}
					}
				}
			}

			iEdgeCost += iExtraEdgeCost*GC.getMOVE_DENOMINATOR();

			pSelectionGroup->CachePathEdgeCosts(pFromPlot, pToPlot, true, iEdgeCost, iSmallestBaseCost, iLargestBaseCost, iNodeCost);
		}
		else
		{
			iEdgeCost = iCachedEndTurnEdgeCost;
		}
	}
	else
	{
		//	Not worth checking the cache for this calculation
		iEdgeCost += (PATH_MOVEMENT_WEIGHT * iCost);

		//	But IS worth caching for the movement costs
		if ( !bHaveNonEndTurnCachedEdgeValue )
		{
			pSelectionGroup->CachePathEdgeCosts(pFromPlot, pToPlot, false, iEdgeCost, iSmallestBaseCost, iLargestBaseCost, iNodeCost);
		}
	}

	iWorstCost = iEdgeCost + iNodeCost;

	if ( bTrace || gTracePathSummary )
	{
		OutputDebugString(CvString::format("Final costs (%d,%d)->(%d,%d)[%s]: (E=%d,N=%d]=%d\n", pFromPlot->getX(), pFromPlot->getY(), pToPlot->getX(), pToPlot->getY(), (bIsEndTurn ? "E" : "NE"), iEdgeCost, iNodeCost, iWorstCost).c_str());
	}

	FAssert(iWorstCost != MAX_INT);

	iWorstCost += PATH_STEP_WEIGHT;

	if ( !bUseAIPathing )
	{
		//	Humans do this backwards to favour 'natural' paths rather than zig zags
		if ((pFromPlot->getX() != pToPlot->getX()) && (pFromPlot->getY() != pToPlot->getY()))
		{
			iWorstCost += PATH_STRAIGHT_WEIGHT;
		}
	}
	else
	{
		if ((pFromPlot->getX() == pToPlot->getX()) || (pFromPlot->getY() == pToPlot->getY()))
		{
			iWorstCost += PATH_STRAIGHT_WEIGHT;
		}
	}

	FAssert(iWorstCost > 0);

	return iWorstCost;
#else
	int iResult = ((const CvSelectionGroup *)pointer)->getPath().containsEdge(pFromPlot,pToPlot) ? 1 : 10000;
	//OutputDebugString(CvString::format("PathCost (%d,%d)->(%d,%d): [%d]\n", pFromPlot->getX(), pFromPlot->getY(), pToPlot->getX(), pToPlot->getY(),iResult).c_str());

	return iResult;
#endif
}

//	Calback functions for the new path generator

//	Heuristic cost
int	NewPathHeuristicFunc(const CvSelectionGroup* pGroup, int iFromX, int iFromY, int iToX, int iToY, int& iLimitCost)
{
	PROFILE_EXTRA_FUNC();
	//PROFILE_FUNC();

	int iStepDistance = stepDistance(iFromX, iFromY, iToX, iToY);
	int	iHeuristicCost = GC.getMOVE_DENOMINATOR()*PATH_MOVEMENT_WEIGHT*iStepDistance;

	if ( iStepDistance > 0 )
	{
		switch(pGroup->getDomainType())
		{
		case DOMAIN_SEA:
		case DOMAIN_AIR:
			//	There are no routes to reduce costs for sea or air
			iLimitCost = iHeuristicCost;
			break;
		case DOMAIN_LAND:
			{
				RouteTypes eBestRoute = GET_PLAYER(pGroup->getHeadOwner()).getBestRoute();

				if ( eBestRoute != NO_ROUTE )
				{
					int iRouteCost = GC.getRouteInfo(eBestRoute).getMovementCost();

					iLimitCost = (iHeuristicCost*iRouteCost)/GC.getMOVE_DENOMINATOR();

					int iMin = MAX_INT;

					foreach_(const CvUnit* pLoopUnit, pGroup->units())
					{
						if (iMin > pLoopUnit->maxMoves())
						{
							iMin = pLoopUnit->maxMoves();
						}
					}

					int iPotentialExtraMoveEvery = iMin/iRouteCost;
					int iMoves = 0;
					if (iMin != 0)
					{
						iMoves = (iRouteCost*stepDistance(iFromX, iFromY, iToX, iToY) + iMin - 1)/iMin;
					}
					else
					{
						iMoves = MAX_INT;
					}
					int iReducedMoves = iMoves - iMoves/(1+iPotentialExtraMoveEvery);

					iLimitCost = (iLimitCost*iReducedMoves)/iMoves;
				}
				else
				{
					iLimitCost = iHeuristicCost;
				}
			}
			break;
		}
	}
	else
	{
		iLimitCost = iHeuristicCost;
	}

	return iHeuristicCost;
}

//	Actual edge cost
int	NewPathCostFunc(const CvPathGeneratorBase* generator, const CvSelectionGroup* pSelectionGroup, int iFromX, int iFromY, int iToX, int iToY, int iFlags, int& iMovementRemaining, int iPathTurns, int& iToNodeCost, bool bIsTerminalNode)
{
	PROFILE_FUNC();

	const CvUnit* pLoopUnit;
	int iMax;

	static const CvSelectionGroup* gLastSelectionGroup = NULL;

	const CvPlot* pFromPlot = GC.getMap().plotSorenINLINE(iFromX, iFromY);
	FAssert(pFromPlot != NULL);
	const CvPlot* pToPlot = GC.getMap().plotSorenINLINE(iToX, iToY);
	FAssert(pToPlot != NULL);

	int iWorstCost = MAX_INT;

	int iWorstMovement = MAX_INT;
	int iLargestBaseCost = -1;
	int iSmallestBaseCost = MAX_INT;
	int iLargestMax = 0;
	int iMaxTerrainDamage = 0;
	static bool bHasAlwaysHostileUnit;
	static bool bHasCanFightUnit;
	static bool bHasCanAttackUnit;
	static bool bHasDefensiveBonusUnit;
	static int iMaxMovement;
	bool bHasVisibleEnemy = false;
#define	MAX_UNIQUE_UNIT_CATEGORIES_CONSIDERED	3
	int	unitCharacteristics[MAX_UNIQUE_UNIT_CATEGORIES_CONSIDERED];
	int	movesLeft[MAX_UNIQUE_UNIT_CATEGORIES_CONSIDERED];
	int numUniqueUnitCategories = 0;
	bool bTrace = false;
	static bool gTracePathSummary = false;
	TeamTypes eTeam = pSelectionGroup->getHeadTeam();
	bool bIsAIControlled = pSelectionGroup->AI_isControlled();
	bool bUseAIPathing = (bIsAIControlled || generator->useAIPathingAlways());
	int iNodeCost = 0;
	int iEdgeCost = 0;
	int iCachedEndTurnEdgeCost;
	int iCachedNonEndTurnEdgeCost;
	int iCachedEndTurnNodeCost;
	int iCachedNonEndTurnNodeCost;
	int iExtraNodeCost = 0;
	int iExtraEdgeCost = 0;
	bool bHaveNonEndTurnCachedEdgeValue = false;
	bool bHaveEndTurnCachedEdgeValue = false;
	bool bCheckedEndTurnEdgeCache = false;
	bool bCheckedNonEndTurnEdgeCache = false;
	bool bEndsTurn = false;
	int iInitialMovementRemaining = -1;

#ifdef PATHFINDING_CACHE
	if ( iMovementRemaining == 0 || iMovementRemaining > 2*GC.getMOVE_DENOMINATOR() )
	{
		bHaveNonEndTurnCachedEdgeValue = pSelectionGroup->HaveCachedPathEdgeCosts(pFromPlot, pToPlot, false, iCachedNonEndTurnEdgeCost, iSmallestBaseCost, iLargestBaseCost, iCachedNonEndTurnNodeCost );
		bCheckedNonEndTurnEdgeCache = true;
	}
	else
	{
		bHaveEndTurnCachedEdgeValue = pSelectionGroup->HaveCachedPathEdgeCosts(pFromPlot, pToPlot, true, iCachedEndTurnEdgeCost, iSmallestBaseCost, iLargestBaseCost, iCachedEndTurnNodeCost );
		bCheckedEndTurnEdgeCache = true;
	}

	if ( bCheckedNonEndTurnEdgeCache && !bHaveNonEndTurnCachedEdgeValue )
	{
		bHaveEndTurnCachedEdgeValue = pSelectionGroup->HaveCachedPathEdgeCosts(pFromPlot, pToPlot, true, iCachedEndTurnEdgeCost, iSmallestBaseCost, iLargestBaseCost, iCachedEndTurnNodeCost );
		bCheckedEndTurnEdgeCache = true;
	}
	else if ( bCheckedEndTurnEdgeCache && !bHaveEndTurnCachedEdgeValue )
	{
		bHaveNonEndTurnCachedEdgeValue = pSelectionGroup->HaveCachedPathEdgeCosts(pFromPlot, pToPlot, false, iCachedNonEndTurnEdgeCost, iSmallestBaseCost, iLargestBaseCost, iCachedNonEndTurnNodeCost );
		bCheckedNonEndTurnEdgeCache = true;
	}
#endif // PATHFINDING_CACHE

	{
		bool bDoesntEndTurn = true;

		//	Either cache has valid movement costs
		if ( bHaveEndTurnCachedEdgeValue || bHaveNonEndTurnCachedEdgeValue )
		{
			int iMaxMovesLeft = 0;
			int iMinMovesLeft = MAX_INT;

			//PROFILE("pathCost.CachedData");

			if (iMovementRemaining > 0)
			{
				iMaxMovesLeft = iMovementRemaining;
				iMinMovesLeft = iMovementRemaining;
			}
			else
			{
				foreach_(pLoopUnit, pSelectionGroup->units())
				{
					iMax = pLoopUnit->maxMoves();

					if ( iMax > iMaxMovesLeft )
					{
						iMaxMovesLeft = iMax;
					}

					if ( iMax < iMinMovesLeft )
					{
						iMinMovesLeft = iMax;
					}

					if ( iMax <= iSmallestBaseCost )
					{
						bEndsTurn = true;
					}
					else if ( iMax <= iLargestBaseCost )
					{
						bDoesntEndTurn = false;
					}
				}
			}

			iInitialMovementRemaining = iMinMovesLeft;

			if ( iMaxMovesLeft <= iLargestBaseCost )
			{
				bEndsTurn = true;
			}
			else if ( bDoesntEndTurn )
			{
				iLargestMax = iMaxMovesLeft - iLargestBaseCost;
			}

			if ( bEndsTurn )
			{
//#if 0
//				PROFILE("pathCost.endTurnDetermined");
//
//				if ( bHaveEndTurnCachedEdgeValue )
//				{
//					PROFILE("pathCost.endTurnDetermined.Cached");
//				}
//#endif
				bDoesntEndTurn = false;
			}
//#if 0
//			else if ( bDoesntEndTurn )
//			{
//				PROFILE("pathCost.nonEndTurnDetermined");
//
//				if ( bHaveNonEndTurnCachedEdgeValue )
//				{
//					PROFILE("pathCost.NonEndTurnDetermined.Cached");
//				}
//			}
//#endif
		}
		else
		{
			bDoesntEndTurn = false;
		}

#ifdef PATHFINDING_CACHE
		if (bEndsTurn && !bCheckedEndTurnEdgeCache)
		{
			bHaveEndTurnCachedEdgeValue = pSelectionGroup->HaveCachedPathEdgeCosts(pFromPlot, pToPlot, true, iCachedEndTurnEdgeCost, iSmallestBaseCost, iLargestBaseCost, iCachedEndTurnNodeCost );
		}
		else if (!bEndsTurn && !bCheckedNonEndTurnEdgeCache)
		{
			bHaveNonEndTurnCachedEdgeValue = pSelectionGroup->HaveCachedPathEdgeCosts(pFromPlot, pToPlot, false, iCachedNonEndTurnEdgeCost, iSmallestBaseCost, iLargestBaseCost, iCachedNonEndTurnNodeCost );
		}
#endif // PATHFINDING_CACHE

		// Do we need to calculate the base characteristics or do we have everything we neeed cached?
		if ((!bEndsTurn && !bIsTerminalNode || !bHaveEndTurnCachedEdgeValue)
		&& (!bDoesntEndTurn || bIsTerminalNode || !bHaveNonEndTurnCachedEdgeValue))
		{
			bool bNeedMaxMovement;

			//PROFILE("pathCost.BaseCharacteristics");

			//	Koshling - the UI uses a pseudo group with id 0 and REUSES the same object, so
			//	comparign object pointers does not guarantee equality of entity!
			if (gLastSelectionGroup != pSelectionGroup || pSelectionGroup->getID() == 0)
			{
				bHasAlwaysHostileUnit = false;
				bHasCanFightUnit = false;
				bHasCanAttackUnit = false;
				bHasDefensiveBonusUnit = false;
				iMaxMovement = MAX_INT;
				bNeedMaxMovement = true;

				gLastSelectionGroup = pSelectionGroup;
			}
			else
			{
				bNeedMaxMovement = true;
			}

			iInitialMovementRemaining = MAX_INT;

			foreach_(pLoopUnit, pSelectionGroup->units())
			{
				if (bNeedMaxMovement || iMovementRemaining == 0)
				{
					iMax = pLoopUnit->maxMoves();

					if (iMax < iMaxMovement)
					{
						iMaxMovement = iMax;
					}
				}
				if (iMovementRemaining > 0)
				{
					iMax = iMovementRemaining;
					iInitialMovementRemaining = iMax;
				}
				else if (iMax < iInitialMovementRemaining)
				{
					iInitialMovementRemaining = iMax;
				}

				int unitMovementCharacteristics = pLoopUnit->getMovementCharacteristicsHash();

				// If we've already considred a unit with these characteristics no need to check this one too
				for (int iI = 0; iI < numUniqueUnitCategories; iI++)
				{
					if (unitMovementCharacteristics == unitCharacteristics[iI])
					{
						break;
					}
				}

				if ( iI == numUniqueUnitCategories )
				{
					if ( !bHasAlwaysHostileUnit && pLoopUnit->isAlwaysHostile(NULL) )
					{
						bHasAlwaysHostileUnit = true;
					}

					if ( !bHasCanFightUnit && pLoopUnit->canFight())
					{
						bHasCanFightUnit = true;
					}

					if ( bHasCanFightUnit && !bHasCanAttackUnit && pLoopUnit->canAttack())
					{
						bHasCanAttackUnit = true;
					}

					if (bHasCanAttackUnit && !bHasVisibleEnemy && pToPlot->isVisibleEnemyDefender(pLoopUnit))
					{
						bHasVisibleEnemy = true;
					}

					if (!bHasDefensiveBonusUnit && !pLoopUnit->noDefensiveBonus())
					{
						bHasDefensiveBonusUnit = true;
					}

					{
						const int pPlotDamage = pToPlot->getTerrainTurnDamage(pLoopUnit);
						if (pPlotDamage > iMaxTerrainDamage)
						{
							iMaxTerrainDamage = pPlotDamage;
						}
					}
					const int iCost = pToPlot->movementCost(pLoopUnit, pFromPlot);

					const int iMovesLeft = std::max(0, iMax - iCost);

					if (iMovesLeft < iWorstMovement)
					{
						iWorstMovement = iMovesLeft;
					}

					if (iMax > iLargestMax)
					{
						iLargestMax = iMax;
					}

					if (iCost > iLargestBaseCost)
					{
						iLargestBaseCost = iCost;
					}

					if (iCost < iSmallestBaseCost)
					{
						iSmallestBaseCost = iCost;
					}

					if ( numUniqueUnitCategories < MAX_UNIQUE_UNIT_CATEGORIES_CONSIDERED )
					{
						movesLeft[numUniqueUnitCategories] = iMax;
						unitCharacteristics[numUniqueUnitCategories++] = unitMovementCharacteristics;
					}
				}
				else if (movesLeft[iI] > iMax)
				{
					const int iMovesLeft = std::max(0, iMax -  pToPlot->movementCost(pLoopUnit, pFromPlot));

					if (iMovesLeft < iWorstMovement)
					{
						iWorstMovement = iMovesLeft;
					}
					movesLeft[iI] = iMovesLeft;
				}
			}
		}
		else if (bEndsTurn)
		{
			iWorstMovement = 0;
		}
		else
		{
			iWorstMovement = iInitialMovementRemaining - iLargestBaseCost;
		}
	}

	//	This always should have been set up
	FAssert(iInitialMovementRemaining != -1);
	FAssert(iLargestBaseCost >= 0);
	FAssert(iWorstMovement != MAX_INT);

	iMovementRemaining = iWorstMovement;

	const bool bIsEndTurn = iWorstMovement == 0 || bIsTerminalNode;
	const int iCost = iLargestBaseCost;
	const int iMovementUsedUp = bIsEndTurn ? iMaxMovement : iInitialMovementRemaining - iMovementRemaining;

	if (bTrace)
	{
		OutputDebugString(CvString::format("Base cost (%d,%d)->(%d,%d): %d\n", pFromPlot->getX(), pFromPlot->getY(), pToPlot->getX(), pToPlot->getY(), iCost).c_str());
	}

	//	Node costs
	if (bIsEndTurn)
	{
		//PROFILE("pathCost.EndTurn.Node");

		if (!bHaveEndTurnCachedEdgeValue)
		{
			if (pToPlot->getTeam() != eTeam)
			{
				iExtraNodeCost += PATH_TERRITORY_WEIGHT;
			}

			// Damage caused by features (mods)
			if (0 != GC.getPATH_DAMAGE_WEIGHT())
			{
				if (pToPlot->getFeatureType() != NO_FEATURE)
				{
					iExtraNodeCost += (GC.getPATH_DAMAGE_WEIGHT() * std::max(0, pToPlot->getFeatureTurnDamage())) / GC.getMAX_HIT_POINTS();
				}
				if (iMaxTerrainDamage > 0)
				{
					iExtraNodeCost += (GC.getPATH_DAMAGE_WEIGHT() * std::max(0, iMaxTerrainDamage * 2)) / GC.getMAX_HIT_POINTS();
				}
			}
			// Add additional cost for ending turn in or adjacent to enemy territory based on flags
			if (iFlags & MOVE_AVOID_ENEMY_WEIGHT_3)
			{
				if (pToPlot->isOwned() && ((GET_TEAM(eTeam).AI_getWarPlan(pToPlot->getTeam()) != NO_WARPLAN) || (pToPlot->getTeam() != eTeam && bHasAlwaysHostileUnit)))
				{
					iNodeCost += iCost*3;
				}
				else
				{
					foreach_(const CvPlot* pAdjacentPlot, pToPlot->adjacent())
					{
						if (pAdjacentPlot->isOwned() && (atWar(pAdjacentPlot->getTeam(), eTeam) || (pAdjacentPlot->getTeam() != eTeam && bHasAlwaysHostileUnit)))
						{
							iNodeCost += (iCost*3)/2;
						}
					}
				}
			}
			else if (iFlags & MOVE_AVOID_ENEMY_WEIGHT_2)
			{
				if (pToPlot->isOwned() && ((GET_TEAM(eTeam).AI_getWarPlan(pToPlot->getTeam()) != NO_WARPLAN) || (pToPlot->getTeam() != eTeam && bHasAlwaysHostileUnit)))
				{
					iNodeCost += iCost*2;
				}
				else
				{
					foreach_(const CvPlot* pAdjacentPlot, pToPlot->adjacent())
					{
						if (pAdjacentPlot->isOwned() && (atWar(pAdjacentPlot->getTeam(), eTeam) || (pAdjacentPlot->getTeam() != eTeam && bHasAlwaysHostileUnit)))
						{
							iNodeCost += (iCost*4)/3;
						}
					}
				}
			}

			if (bHasCanFightUnit && bUseAIPathing)
			{
				int iDefenseCost = (PATH_DEFENSE_WEIGHT * std::max(0, (200 - (!bHasDefensiveBonusUnit ? 0 : pSelectionGroup->defensiveModifierAtPlot(pToPlot)))))/100;

				//	Take more care to path defensively in neutral territory, and still more in enemy territory
				if ( pToPlot->getTeam() == NO_TEAM )
				{
					iDefenseCost *= 2;
				}
				else if ( atWar(pToPlot->getTeam(), eTeam) )
				{
					iDefenseCost *= 4;
				}
				if ( bTrace )
				{
					OutputDebugString(CvString::format("End turn defense cost: %d\n", iDefenseCost).c_str());
				}

				iExtraNodeCost += iDefenseCost;

				//	If we finish a move next to an enemy city or fort account for city defense damage and add
				//	a general avoidance cost
				int iCityAdjacencyCost = 0;

				foreach_(const CvPlot* pAdjacentPlot, pToPlot->adjacent())
				{
					const CvCity* pAdjacentCity = pAdjacentPlot->getPlotCity();
					if (pAdjacentCity != NULL || pAdjacentPlot->isActsAsCity())
					{
						if ( atWar(pAdjacentPlot->getTeam(), eTeam) )
						{
							if ( pAdjacentCity != NULL )
							{
								iCityAdjacencyCost +=  (2 * GC.getPATH_DAMAGE_WEIGHT() * std::max(0, pAdjacentCity->getAdjacentDamagePercent())) / GC.getMAX_HIT_POINTS();
							}

							iCityAdjacencyCost += PATH_ENEMY_CITY_WEIGHT;
						}
					}
				}

				if ( bTrace )
				{
					OutputDebugString(CvString::format("End turn city adjacency cost: %d\n", iCityAdjacencyCost).c_str());
				}

				iExtraNodeCost += iCityAdjacencyCost;

				// We only consider enemies within 2 turns of our current location, as we
				// can't predict where they will go anyway.
				// TODO: We should be considering fortified enemies, those holding forts, and those
				// in cities though regardless of how far away they are. We shouldn't expect them to move.
				static const int UNIT_ADJUST_HORIZON = 2;
				if ( iPathTurns <= UNIT_ADJUST_HORIZON )
				{
					if ( bTrace )
					{
						OutputDebugString(CvString::format("Evaluating end-turn unit danger\n").c_str());
					}

					int iUnitAdjacencyCost = 0;

					foreach_(const CvPlot* pAdjacentPlot, pToPlot->adjacent())
					{
						if((bIsAIControlled || pAdjacentPlot != generator->getTerminalPlot()) &&	//	For the human player don't count ending turn next to what we intend to attack as bad
							pSelectionGroup->getArea() == pAdjacentPlot->getArea() &&
							pAdjacentPlot->isVisible(eTeam, false) &&
							pAdjacentPlot->getVisibleEnemyDefender(pSelectionGroup->getHeadOwner()))
						{
							int iRatioToUnitStack = pSelectionGroup->AI_compareStacks(pAdjacentPlot,
								//CvPathGenerator::IsMaxPerformance()?
								//StackCompare::Fast
								//:
								StackCompare::None
							);

							if ( iRatioToUnitStack < 120 )
							{
								iUnitAdjacencyCost += (PATH_MOVEMENT_WEIGHT * 100)/std::max(1,iRatioToUnitStack);
							}
						}
					}

					if ( bTrace )
					{
						OutputDebugString(CvString::format("End turn unit adjacency cost: %d\n", iUnitAdjacencyCost).c_str());
					}

					iExtraNodeCost += iUnitAdjacencyCost;
				}
			}

			iNodeCost += iExtraNodeCost*GC.getMOVE_DENOMINATOR();
		}
		else
		{
			iNodeCost = iCachedEndTurnNodeCost;
		}
	}
	else
	{
		//	There are no non-end-turn node costs currently
	}

	//	Edge costs
	if (bIsEndTurn)
	{
		//PROFILE("pathCost.EndTurn.Edge");

		FAssert(iWorstMovement != MAX_INT);
		//	Account for artifical turn end due to reaching destination except in human case
		//	where we may not really want to end here
		if (bIsAIControlled && iWorstMovement > 0)
		{
			//iMovementUsedUp += iWorstMovement - 1;
		}

		if (bHaveEndTurnCachedEdgeValue)
		{
			iEdgeCost = iCachedEndTurnEdgeCost;
		}
		else
		{
			if (bUseAIPathing && bHasCanAttackUnit && bIsTerminalNode && bHasVisibleEnemy)
			{
				iExtraEdgeCost += (PATH_DEFENSE_WEIGHT * std::max(0, (200 - (!bHasDefensiveBonusUnit ? 0 : pFromPlot->defenseModifier(eTeam, false)))))/100;

				if (!pFromPlot->isCity())
				{
					iExtraEdgeCost += PATH_CITY_WEIGHT;
				}

				if (pFromPlot->isRiverCrossing(directionXY(pFromPlot, pToPlot)) && !pLoopUnit->isRiver())
				{
					iExtraEdgeCost += PATH_RIVER_WEIGHT * -GC.getRIVER_ATTACK_MODIFIER() / 100;
					// AIAndy: This might be reachable without iMovesLeft initialised. Check needed.
					iExtraEdgeCost += PATH_MOVEMENT_WEIGHT * iWorstMovement / GC.getMOVE_DENOMINATOR();
				}
			}

			iEdgeCost += iExtraEdgeCost*GC.getMOVE_DENOMINATOR();

#ifdef PATHFINDING_CACHE
			if ( iInitialMovementRemaining - iLargestBaseCost == iMovementRemaining )
			{
				pSelectionGroup->CachePathEdgeCosts(pFromPlot, pToPlot, true, iEdgeCost, iSmallestBaseCost, iLargestBaseCost, iNodeCost);
			}
#endif // PATHFINDING_CACHE
		}
	}
#ifdef PATHFINDING_CACHE
	else if (!bHaveNonEndTurnCachedEdgeValue && iInitialMovementRemaining - iLargestBaseCost == iMovementRemaining)
	{
		//	Cache the movement costs
		pSelectionGroup->CachePathEdgeCosts(pFromPlot, pToPlot, false, iEdgeCost, iSmallestBaseCost, iLargestBaseCost, iNodeCost);
	}
#endif // PATHFINDING_CACHE

	//	Cost is the edge cost (cached) + the node cost (cached) + the used movement cost (calculated)
	iWorstCost = iEdgeCost + iNodeCost + (iMovementUsedUp*PATH_MOVEMENT_WEIGHT);

	if (bTrace || gTracePathSummary)
	{
		OutputDebugString(CvString::format("Final costs (%d,%d)->(%d,%d)[%s]: (E=%d,N=%d]=%d\n", pFromPlot->getX(), pFromPlot->getY(), pToPlot->getX(), pToPlot->getY(), (bIsEndTurn ? "E" : "NE"), iEdgeCost, iNodeCost, iWorstCost).c_str());
	}

	FAssert(iWorstCost != MAX_INT);

	iWorstCost += PATH_STEP_WEIGHT;

	if (!bUseAIPathing)
	{
		// Humans do this backwards to favour 'natural' paths rather than zig zags
		if ((pFromPlot->getX() != pToPlot->getX()) && (pFromPlot->getY() != pToPlot->getY()))
		{
			iWorstCost += PATH_STRAIGHT_WEIGHT;
		}
	}
	else if ((pFromPlot->getX() == pToPlot->getX()) || (pFromPlot->getY() == pToPlot->getY()))
	{
		iWorstCost += PATH_STRAIGHT_WEIGHT;
	}

	FAssert(iWorstCost > 0);

	iToNodeCost = iNodeCost;

	return iWorstCost;
}

bool NewPathDestValid(const CvSelectionGroup* pSelectionGroup, int iToX, int iToY, int iFlags, bool& bRequiresWar)
{
	PROFILE_FUNC();

	bRequiresWar = false;

	const CvPlot* pToPlot = GC.getMap().plotSorenINLINE(iToX, iToY);
	FAssert(pToPlot != NULL);

	if (pSelectionGroup->atPlot(pToPlot))
	{
		return true;
	}

	if (pSelectionGroup->getDomainType() == DOMAIN_IMMOBILE)
	{
		return false;
	}

	const bool bAIControl = pSelectionGroup->AI_isControlled();

	if (bAIControl)
	{
		if (pSelectionGroup->getDomainType() == DOMAIN_LAND)
		{
			const int iGroupAreaID = pSelectionGroup->getArea();

			if (pToPlot->getArea() != iGroupAreaID && !pSelectionGroup->canMoveAllTerrain() && !pToPlot->isAdjacentToArea(iGroupAreaID))
			{
				return false;
			}
		}

		if (!(iFlags & MOVE_IGNORE_DANGER)
		&& !pSelectionGroup->canFight() && !pSelectionGroup->alwaysInvisible()
		&& GET_PLAYER(pSelectionGroup->getHeadOwner()).AI_getAnyPlotDanger(pToPlot))
		{
			return false;
		}
	}

	if (bAIControl || pToPlot->isRevealed(pSelectionGroup->getHeadTeam(), false))
	{
		if (pSelectionGroup->isAmphibPlot(pToPlot))
		{
			foreach_(const CvUnit* pLoopUnit1, pSelectionGroup->units())
			{
				if (pLoopUnit1->hasCargo() && pLoopUnit1->getDomainCargo() == DOMAIN_LAND)
				{
					foreach_(const CvUnit* pLoopUnit2, pLoopUnit1->plot()->units())
					{
						if (pLoopUnit2->getTransportUnit() == pLoopUnit1 && pLoopUnit2->isGroupHead()
						&& pLoopUnit2->getGroup()->canEnterOrAttackPlot(pToPlot, (pSelectionGroup->AI_isDeclareWar(pToPlot) || (iFlags & MOVE_DECLARE_WAR))))
						{
							bRequiresWar = !pLoopUnit2->getGroup()->canEnterOrAttackPlot(pToPlot);
							return true;
						}
					}
				}
			}
			return false;
		}

		if (!pSelectionGroup->canEnterOrAttackPlot(pToPlot, pSelectionGroup->AI_isDeclareWar(pToPlot) || (iFlags & MOVE_DECLARE_WAR)))
		{
			return false;
		}

		bRequiresWar = !pSelectionGroup->canEnterOrAttackPlot(pToPlot);
		return true;
	}

	return true;
}

bool NewPathTurnEndValidityCheckRequired(const CvSelectionGroup* pSelectionGroup, int iFlags)
{
	return (
		!(iFlags & MOVE_IGNORE_DANGER)
		&& pSelectionGroup->AI_isControlled()
		&& !pSelectionGroup->canFight()
		&& !pSelectionGroup->alwaysInvisible()
	);
}

//	Edge validity
bool ContextFreeNewPathValidFunc(const CvSelectionGroup* pSelectionGroup, int iFromX, int iFromY, int iToX, int iToY, int iFlags, bool isTerminus, bool bMoveTerminationChecksOnly, int iPathTurns, bool* pbToNodeInvalidity, bool* pbValidAsTerminus)
{
	PROFILE_FUNC();

	const PlayerTypes eOwner = pSelectionGroup->getHeadOwner();

	const CvPlot* pFromPlot = GC.getMap().plotSorenINLINE(iFromX,iFromY);
	FAssert(pFromPlot != NULL);
	CvPlot* pToPlot = GC.getMap().plotSorenINLINE(iToX, iToY);
	FAssert(pToPlot != NULL);

	if (pbValidAsTerminus != NULL)
	{
		*pbValidAsTerminus = false;
	}

	if (!bMoveTerminationChecksOnly)
	{
		switch(pSelectionGroup->getDomainType())
		{
			case DOMAIN_SEA:
			{
				{
					PROFILE("pathValid domain sea");
//			#if 0
//					// Optimisation short-circuit some invalid pathing choices quickly
//					if (!pToPlot->isWater() && !pSelectionGroup->canMoveAllTerrain() && !pToPlot->isCanMoveSeaUnits() && !pToPlot->isCity())
//					{
//						return false;
//					}
//			#endif
					// Can't cross diagonally across 'land'
					if (pFromPlot->isWater() && pToPlot->isWater()
					&& !GC.getMap().plot(pFromPlot->getX(), pToPlot->getY())->isWater() && !GC.getMap().plot(pToPlot->getX(), pFromPlot->getY())->isWater()
					&& !pSelectionGroup->canMoveAllTerrain())
					{
						return false;
					}
				}
				break;
			}
			case DOMAIN_LAND:
			{
				if ((iFlags & MOVE_NO_LAND_UNITS_ACROSS_WATER) != 0 && pToPlot->isWater())
				{
					return false;
				}
				break;
			}
			default: break;
		}

		if (GC.getGame().isOption(GAMEOPTION_UNSUPPORTED_ZONE_OF_CONTROL))
		{
			const TeamTypes eTeam = pSelectionGroup->getHeadTeam();

			// ZoC don't apply into cities of the unit owner
			if (pToPlot->getPlotCity() == NULL || pToPlot->getPlotCity()->getTeam() != eTeam)
			{
				// Fort ZoC
				const PlayerTypes eDefender = pFromPlot->controlsAdjacentZOCSource(eTeam);
				if (eDefender != NO_PLAYER)
				{
					const CvPlot* pZoneOfControl = pFromPlot->isInFortControl(true, eDefender, eOwner);
					const CvPlot* pForwardZoneOfControl = pToPlot->isInFortControl(true, eDefender, eOwner);
					if (pZoneOfControl != NULL && pForwardZoneOfControl != NULL
					&& pZoneOfControl == pToPlot->isInFortControl(true, eDefender, eOwner, pZoneOfControl)
					&& !pSelectionGroup->canIgnoreZoneofControl())
					{
						return false;
					}
				}
				// City ZoC
				if (pFromPlot->isInCityZoneOfControl(eOwner) && pToPlot->isInCityZoneOfControl(eOwner) && !pSelectionGroup->canIgnoreZoneofControl())
				{
					return false;
				}
			}
			// Promotion ZoC
			if (GC.getGame().isAnyoneHasUnitZoneOfControl() && pFromPlot->isInUnitZoneOfControl(eOwner)
			&& pToPlot->isInUnitZoneOfControl(eOwner) && !pSelectionGroup->canIgnoreZoneofControl())
			{
				return false;
			}
		}
	}

	if (isTerminus)
	{
		//	Need to prevent false failures when the terminal
		//	plot contains an enemy unit (and we don't have the flag to allow movement through enemies
		//	(which should not prevent attacking enemies in the terminal plot)
		return true;
	}

	if ( pbValidAsTerminus != NULL )
	{
		*pbValidAsTerminus = true;
	}

	//	KOSHLING MOD - none of the rest of the calculation depends on pFromPlot,
	//	so we can cache the results from one request for each pToPlot.
	bool bCheckNonInvisibleDanger = !(iFlags & MOVE_IGNORE_DANGER) &&
									pSelectionGroup->AI_isControlled() &&
									bMoveTerminationChecksOnly &&
									!(pSelectionGroup->canFight()) &&
									!(pSelectionGroup->alwaysInvisible());

	if ( bMoveTerminationChecksOnly && !bCheckNonInvisibleDanger )
	{
		return true;
	}

	bool bResult;

	if (pSelectionGroup->isHuman())
	{
		bResult = pathValidInternal(pToPlot, bCheckNonInvisibleDanger, pSelectionGroup, iFlags);
	}
	else
	{
		unsigned int iEntityId = 0;

		CheckSum(iEntityId, pSelectionGroup->getHeadUnit()->getID());
		CheckSum(iEntityId, (int)eOwner);
		CheckSum(iEntityId, iFlags);

#ifdef PATHFINDING_VALIDITY_CACHE
		if (!pToPlot->HaveCachedPathValidityResult((void*)iEntityId, bCheckNonInvisibleDanger, bResult))
#endif // PATHFINDING_VALIDITY_CACHE
		{
			bResult = pathValidInternal(pToPlot, bCheckNonInvisibleDanger, pSelectionGroup, iFlags);

#ifdef PATHFINDING_VALIDITY_CACHE
			pToPlot->CachePathValidityResult((void*)iEntityId, bCheckNonInvisibleDanger, bResult);
#endif // PATHFINDING_VALIDITY_CACHE
		}
	}

	if ( !bCheckNonInvisibleDanger && pbToNodeInvalidity != NULL )
	{
		*pbToNodeInvalidity = !bResult;
	}

	return bResult;
}

bool NewPathValidFunc(const CvSelectionGroup* pSelectionGroup, int iFromX, int iFromY, int iToX, int iToY, int iFlags, bool isTerminus, bool bMoveTerminationChecksOnly, int iPathTurns, bool& bToNodeInvalidity)
{
	bool bDummy;
	return ContextFreeNewPathValidFunc(pSelectionGroup, iFromX, iFromY, iToX, iToY, iFlags, isTerminus, bMoveTerminationChecksOnly, iPathTurns, &bToNodeInvalidity, &bDummy);
}

bool moveToValid(const CvSelectionGroup* pSelectionGroup, const CvPlot* pPlot, int iFlags)
{
	if (iFlags & MOVE_SAFE_TERRITORY)
	{
		PROFILE("pathValid move save");

		if (pPlot->isOwned() && pPlot->getTeam() != pSelectionGroup->getHeadTeam())
		{
			return false;
		}
	}

	if (iFlags & MOVE_OUR_TERRITORY)
	{
		PROFILE("pathValid move save");

		if (!pPlot->isOwned() || pPlot->getTeam() != pSelectionGroup->getHeadTeam())
		{
			return false;
		}
	}

	if (iFlags & MOVE_NO_ENEMY_TERRITORY)
	{
		PROFILE("pathValid no enemy");

		if (pPlot->isOwned() && atWar(pPlot->getTeam(), pSelectionGroup->getHeadTeam()))
		{
			return false;
		}
	}

	{
		PROFILE("pathValid move through");

		if (iFlags & MOVE_THROUGH_ENEMY)
		{
			if (!pSelectionGroup->canEnterOrAttackPlot(pPlot))
			{
				return false;
			}
		}
		else if (!pSelectionGroup->canMoveThrough(pPlot, (iFlags & MOVE_TERMINUS_DECLARES_WAR) != 0))
		{
			return false;
		}
	}

	if ((iFlags &  MOVE_AVOID_ENEMY_UNITS))
	{
		if (((CvUnitAI*)pSelectionGroup->getHeadUnit())->exposedToDanger(pPlot, 80))
		{
			return false;
		}
	}

	if (!pSelectionGroup->canDefend())
	{
		if ((iFlags & MOVE_RECONSIDER_ON_LEAVING_OWNED))
		{
			if (pPlot->getTeam() != pSelectionGroup->getHeadTeam())
			{
				return false;
			}
		}
		if ((iFlags & MOVE_WITH_CAUTION))
		{
			// If the next plot we'd go to has a danger count above a threshold
			//	consider it not safe and abort so we can reconsider
			if (pPlot->getDangerCount(pSelectionGroup->getHeadTeam()) > 20
			// Also for non-owned territory check for nearby enemies
			||  pPlot->getTeam() != pSelectionGroup->getHeadTeam()
			&&  GET_PLAYER(pSelectionGroup->getOwner()).AI_getVisiblePlotDanger(pPlot, 2, false))
			{
				return false;
			}
		}
	}

	if ((iFlags & MOVE_HEAL_AS_NEEDED25)
	&&  pSelectionGroup->getHeadUnit()->getDamagePercent() > 25
	&&  pSelectionGroup->plot()->getTotalTurnDamage(pSelectionGroup) <= 0
	&& !GET_PLAYER(pSelectionGroup->getOwner()).AI_getVisiblePlotDanger(pSelectionGroup->plot(), 2, false))
	{
		return false;
	}

	return true;
}

int stepDestValid(int iToX, int iToY, const void* pointer, FAStar* finder)
{
	PROFILE_FUNC();

	const CvPlot* pFromPlot = GC.getMap().plotSorenINLINE(gDLL->getFAStarIFace()->GetStartX(finder), gDLL->getFAStarIFace()->GetStartY(finder));
	FAssert(pFromPlot != NULL);
	const CvPlot* pToPlot = GC.getMap().plotSorenINLINE(iToX, iToY);
	FAssert(pToPlot != NULL);

	if (pFromPlot->area() != pToPlot->area())
	{
		//	If the info int is not -1 it is the plot coord of a land plot to allow transition to
		//	from a water plot (used to find assault locations for transports)
		if ( (pFromPlot->getPlotType() == PLOT_OCEAN) != (pToPlot->getPlotType() == PLOT_OCEAN) )
		{
			const int iPlot = gDLL->getFAStarIFace()->GetInfo(finder);

			if ( iPlot != -1 )
			{
				const CvPlot* destPlot = GC.getMap().plotByIndex(iPlot);

				if ( destPlot == pToPlot )
				{
					return TRUE;
				}
			}
		}
		return FALSE;
	}

	return TRUE;
}


int stepHeuristic(int iFromX, int iFromY, int iToX, int iToY)
{
	return stepDistance(iFromX, iFromY, iToX, iToY);
}


int stepCost(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	return 1;
}


int stepValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	if (parent == NULL)
	{
		return TRUE;
	}

	const CvPlot* pNewPlot = GC.getMap().plotSorenINLINE(node->m_iX, node->m_iY);

	if (pNewPlot->isImpassable())
	{
		return FALSE;
	}
	// Super Forts *choke*
	const int iInvalidPlot = gDLL->getFAStarIFace()->GetInfo(finder);
	if (iInvalidPlot > 0
	// 1 is subtracted because 1 was added earlier to avoid a conflict with index 0
	&& pNewPlot == GC.getMap().plotByIndex(iInvalidPlot - 1))
	{
		return FALSE;
	}
	// ! Super Forts

	const CvPlot* pFromPlot = GC.getMap().plotSorenINLINE(parent->m_iX, parent->m_iY);
	if (pFromPlot->area() != pNewPlot->area())
	{
		return FALSE;
	}

	// Don't count diagonal hops across land isthmus
	if (pFromPlot->isWater() && pNewPlot->isWater()
	&& !GC.getMap().plot(parent->m_iX, node->m_iY)->isWater()
	&& !GC.getMap().plot(node->m_iX, parent->m_iY)->isWater())
	{
		return FALSE;
	}
	return TRUE;
}


// Find paths that a team's units could follow without declaring war
int teamStepValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	if (parent == NULL)
	{
		return TRUE;
	}

	const CvPlot* pNewPlot = GC.getMap().plotSorenINLINE(node->m_iX, node->m_iY);

	const CvPlot* pFromPlot = GC.getMap().plotSorenINLINE(parent->m_iX, parent->m_iY);
	if (pFromPlot->area() != pNewPlot->area())
	{
		return FALSE;
	}

	// Don't count diagonal hops across land isthmus
	if (pFromPlot->isWater() && pNewPlot->isWater()
	&& !GC.getMap().plot(parent->m_iX, node->m_iY)->isWater()
	&& !GC.getMap().plot(node->m_iX, parent->m_iY)->isWater())
	{
		return FALSE;
	}

	const TeamTypes ePlotTeam = pNewPlot->getTeam();

	if (ePlotTeam == NO_TEAM)
	{
		return TRUE;
	}
	const std::vector<TeamTypes> teamVec = *((std::vector<TeamTypes> *)pointer);

	if (ePlotTeam == teamVec[1])
	{
		return TRUE;
	}
	const CvTeamAI& kTeam = GET_TEAM(teamVec[0]);

	if (kTeam.isFriendlyTerritory(ePlotTeam))
	{
		return TRUE;
	}

	if (kTeam.AI_getWarPlan(ePlotTeam) != NO_WARPLAN)
	{
		return TRUE;
	}

	if (kTeam.isOpenBorders(ePlotTeam))
	{
		return TRUE;
	}

	return FALSE;
}


int stepAdd(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	if (data == ASNC_INITIALADD)
	{
		node->m_iData1 = 0;
	}
	else
	{
		node->m_iData1 = (parent->m_iData1 + 1);
	}

	FASSERT_NOT_NEGATIVE(node->m_iData1);

	return 1;
}


int routeValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	if (parent == NULL)
	{
		return true;
	}
	const CvPlot* pNewPlot = GC.getMap().plotSorenINLINE(node->m_iX, node->m_iY);

	const PlayerTypes ePlayer = (PlayerTypes)gDLL->getFAStarIFace()->GetInfo(finder);

	return (
		(!pNewPlot->isOwned() || pNewPlot->getTeam() == GET_PLAYER(ePlayer).getTeam())
		&&
		pNewPlot->getRouteType() == GET_PLAYER(ePlayer).getBestRoute(pNewPlot)
	);
}


int borderValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	if (parent == NULL)
	{
		return TRUE;
	}

	//const CvPlot* pOldPlot = GC.getMap().plotSorenINLINE(parent->m_iX, parent->m_iY);

	const CvPlot* pNewPlot = GC.getMap().plotSorenINLINE(node->m_iX, node->m_iY);

	if (pNewPlot->getTeam() == GET_PLAYER((PlayerTypes)gDLL->getFAStarIFace()->GetInfo(finder)).getTeam())
	{
		if (GC.getMap().plotSorenINLINE(gDLL->getFAStarIFace()->GetDestX(finder), gDLL->getFAStarIFace()->GetDestY(finder))->isWater())
		{
			return pNewPlot->isWater() || pNewPlot->isActsAsCity();
		}
		return !pNewPlot->isWater();
	}
	return FALSE;
}


int areaValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	if (parent == NULL)
	{
		return TRUE;
	}
	return GC.getMap().plotSorenINLINE(parent->m_iX, parent->m_iY)->isWater() == GC.getMap().plotSorenINLINE(node->m_iX, node->m_iY)->isWater() ? TRUE : FALSE;

/* BBAI TODO: Why doesn't this work to break water and ice into separate area?

	if (GC.getMap().plotSorenINLINE(parent->m_iX, parent->m_iY)->isWater() != GC.getMap().plotSorenINLINE(node->m_iX, node->m_iY)->isWater())
	{
		return FALSE;
	}

	// Ice blocks become their own area
	if (GC.getMap().plotSorenINLINE(parent->m_iX, parent->m_iY)->isWater() && GC.getMap().plotSorenINLINE(node->m_iX, node->m_iY)->isWater())
	{
		if (GC.getMap().plotSorenINLINE(parent->m_iX, parent->m_iY)->isImpassable() != GC.getMap().plotSorenINLINE(node->m_iX, node->m_iY)->isImpassable())
		{
			return FALSE;
		}
	}

	return TRUE;
*/
}


int joinArea(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	if (data == ASNL_ADDCLOSED)
	{
		GC.getMap().plotSorenINLINE(node->m_iX, node->m_iY)->setArea(gDLL->getFAStarIFace()->GetInfo(finder));
	}

	return 1;
}


int plotGroupValid(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	if (parent == NULL)
	{
		return TRUE;
	}

	const CvPlot* pOldPlot = GC.getMap().plotSorenINLINE(parent->m_iX, parent->m_iY);
	const CvPlot* pNewPlot = GC.getMap().plotSorenINLINE(node->m_iX, node->m_iY);

	const PlayerTypes ePlayer = ((PlayerTypes)(gDLL->getFAStarIFace()->GetInfo(finder)));
	const TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();

	if (pOldPlot->getPlotGroup(ePlayer) == pNewPlot->getPlotGroup(ePlayer)
	&& pNewPlot->isTradeNetwork(eTeam)
	&& pNewPlot->isTradeNetworkConnected(pOldPlot, eTeam))
	{
		return TRUE;
	}
	return FALSE;
}


int countRegion(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	if (data == ASNL_ADDCLOSED)
	{
		(*((int*)pointer))++;
	}
	return 1;
}

int countPlotGroup(FAStarNode* parent, FAStarNode* node, int data, const void* pointer, FAStar* finder)
{
	PROFILE_FUNC();

	if (data == ASNL_ADDCLOSED)
	{
		CvPlot* pPlot = GC.getMap().plotSorenINLINE(node->m_iX, node->m_iY);
		plotGroupCheckInfo* checkInfo = (plotGroupCheckInfo*)pointer;

		pPlot->m_groupGenerationNumber = checkInfo->groupGenerationNumber;
		checkInfo->hashInfo.allNodesHash ^= pPlot->getZobristContribution();

		if (pPlot->isCity() || pPlot->getImprovementType() != NO_IMPROVEMENT && pPlot->getBonusType() != NO_BONUS)
		{
			checkInfo->hashInfo.resourceNodesHash ^= pPlot->getZobristContribution();
		}
	}

	return 1;
}


int baseYieldToSymbol(int iNumYieldTypes, int iYieldStack)
{
	return iNumYieldTypes * GC.getDefineINT("MAX_YIELD_STACK") + iYieldStack;
}


bool isPickableName(const char* szName)
{
	return !szName || _tcsicmp(&szName[_tcslen(szName)-6], "NOPICK");
}


// create an array of shuffled numbers
int* shuffle(int iNum, CvRandom& rand)
{
	int* piShuffle = new int[iNum];
	shuffleArray(piShuffle, iNum, rand);
	return piShuffle;
}


void shuffleArray(int* piShuffle, int iNum, CvRandom& rand)
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < iNum; iI++)
	{
		piShuffle[iI] = iI;
	}
	shuffle(piShuffle, iNum, rand);
}

void shuffle(int* piShuffle, int iNum, CvRandom& rand)
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < iNum; iI++)
	{
		const int iJ = iI + rand.get(iNum - iI);

		if (iI != iJ)
		{
			const int iTemp = piShuffle[iI];
			piShuffle[iI] = piShuffle[iJ];
			piShuffle[iJ] = iTemp;
		}
	}
}

#define NUM_ERAS 14

struct EraYearRange
{
    int startYear;
    int endYear;
};

void initEraYearRanges(EraYearRange eraYearRanges[NUM_ERAS])
{
	eraYearRanges[0].startYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_PREHISTORIC_START");
	eraYearRanges[0].endYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_PREHISTORIC_END");

	eraYearRanges[1].startYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_ANCIENT_START");
	eraYearRanges[1].endYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_ANCIENT_END");

	eraYearRanges[2].startYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_CLASSIC_START");
	eraYearRanges[2].endYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_CLASSIC_END");

	eraYearRanges[3].startYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_MEDIEVAL_START");
	eraYearRanges[3].endYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_MEDIEVAL_END");

	eraYearRanges[4].startYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_RENAISSANCE_START");
	eraYearRanges[4].endYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_RENAISSANCE_END");

	eraYearRanges[5].startYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_INDUSTRIAL_START");
	eraYearRanges[5].endYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_INDUSTRIAL_END");

	eraYearRanges[6].startYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_ATOMIC_START");
	eraYearRanges[6].endYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_ATOMIC_END");

	eraYearRanges[7].startYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_INFORMATION_START");
	eraYearRanges[7].endYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_INFORMATION_END");

	eraYearRanges[8].startYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_NANOTECH_START");
	eraYearRanges[8].endYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_NANOTECH_END");

	eraYearRanges[9].startYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_TRANSHUMAN_START");
	eraYearRanges[9].endYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_TRANSHUMAN_END");

	eraYearRanges[10].startYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_GALACTIC_START");
	eraYearRanges[10].endYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_GALACTIC_END");

	eraYearRanges[11].startYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_COSMIC_START");
	eraYearRanges[11].endYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_COSMIC_END");

	eraYearRanges[12].startYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_TRANSCENDENT_START");
	eraYearRanges[12].endYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_TRANSCENDENT_END");

	eraYearRanges[13].startYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_FUTURE_START");
	eraYearRanges[13].endYear = GC.getDefineINT("HISTORICAL_ACCURATE_ERA_RANGE_FUTURE_END");
}
double customDatePolynomial(double x) {
	return tanh(30.0 * x) * pow(x, 0.5);
}

void calculatePlayerInfluence(double playerInfluences[NUM_ERAS], const int totalTechsInEra[NUM_ERAS], const int playerTechsInEra[NUM_ERAS])
{
    for (int iEra = 0; iEra < NUM_ERAS; ++iEra)
    {
        if (totalTechsInEra[iEra] == 0)
        {
            playerInfluences[iEra] = 0.0;
        }
        else
        {
            //playerInfluences[iEra] = pow(static_cast<double>(playerTechsInEra[iEra]) / totalTechsInEra[iEra], 0.5);
			playerInfluences[iEra] = customDatePolynomial(static_cast<double>(playerTechsInEra[iEra]) / totalTechsInEra[iEra]);
        }
        //logging::logMsg("C2C.log", "[CALENDAR] Player influence in era %d: %f", iEra, playerInfluences[iEra]);
    }
}

void calculateTotalInfluence(double totalInfluences[NUM_ERAS], double weight[NUM_ERAS])
{
    int numPlayers = 0;

    for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
    {
        CvPlayer& player = GET_PLAYER((PlayerTypes)iPlayer);
		CvTeam& teammm = GET_TEAM(player.getTeam());

        if (player.isAlive() && (player.isHuman() || player.isNormalAI()))
        {
            numPlayers++;
            int playerTechsInEra[NUM_ERAS] = {0};
			int totalTechsInEra[NUM_ERAS] = {0};
            //double iResearchProgress = team.getResearchProgress(eTech);
            for (int iTech = 0; iTech < GC.getNumTechInfos(); ++iTech)
            {
                TechTypes eTech = (TechTypes)iTech;
                EraTypes eEra = (EraTypes)GC.getTechInfo(eTech).getEra();

                if (eEra >= 0 && eEra < GC.getNumEraInfos())
                {
					CvTeam& team = GET_TEAM(player.getTeam());
					totalTechsInEra[eEra] += team.getResearchCost(eTech);
					int iResearchProgress = 0;
                    if (GET_TEAM(player.getTeam()).isHasTech(eTech)) 
                    {
                        //playerTechsInEra[eEra]++;
						//playerTechsInEra[eEra] += GC.getTechInfo(eTech).getResearchCost();
						playerTechsInEra[eEra] += team.getResearchCost(eTech);
                    }
					else
					{
						iResearchProgress = GET_TEAM(player.getTeam()).getResearchProgress(player.getCurrentResearch());
					}

					// If there is progress, add the player's current research progress
					if (iResearchProgress > 0 && player.getCurrentResearch() == eTech) {
						playerTechsInEra[eEra] += iResearchProgress;
						//logging::logMsg("C2C.log", "[TECH] Player %d research tech %S, progress = %d", iPlayer, GC.getTechInfo(eTech).getDescription(), iResearchProgress);
					}
                }
            }
            double playerInfluences[NUM_ERAS] = {0.0};
			//const char* playerName = player.getName();
			//logging::logMsg("C2C.log", "[TECH] its Player %S influence", player.getName());
            calculatePlayerInfluence(playerInfluences, totalTechsInEra, playerTechsInEra);

            for (int iEra = 0; iEra < NUM_ERAS; ++iEra)
            {
                totalInfluences[iEra] += playerInfluences[iEra];
            }
        }
    }

    for (int iEra = 0; iEra < NUM_ERAS; ++iEra)
    {
        totalInfluences[iEra] /= numPlayers; 
        weight[iEra] = totalInfluences[iEra] * pow(5.0, iEra); 
        //logging::logMsg("C2C.log", "[CALENDAR] Era %d: Total Influence = %f, Weight = %f", iEra, totalInfluences[iEra], weight[iEra]);
    }
}

int calculateYearFromInfluence(int eraStart, int eraEnd, double influence)
{
    return eraStart + static_cast<int>((eraEnd - eraStart) * influence);
}

int calculateTickFromInfluence(int eraStart, int eraEnd, double influence)
{
	return (abs(200000 + eraStart) * 360) + static_cast<int>((eraEnd - eraStart) * influence * 360);
}

int calculateCurrentTick()
{
	int totalTechsInEra[NUM_ERAS] = { 0 };
	double totalInfluences[NUM_ERAS] = { 0.0 };
	double weight[NUM_ERAS] = { 0.0 };
	EraYearRange eraYearRanges[NUM_ERAS];

	initEraYearRanges(eraYearRanges); //initialize era ranges from XML global defines
	calculateTotalInfluence(totalInfluences, weight); //calculate influence from players techs

	double weightedTicks = 0.0;
	double totalWeight = 0.0;

	for (int iEra = 0; iEra < NUM_ERAS; ++iEra)
	{
		int tick = calculateTickFromInfluence(eraYearRanges[iEra].startYear, eraYearRanges[iEra].endYear, totalInfluences[iEra]);
		weightedTicks += tick * weight[iEra];
		totalWeight += weight[iEra];
		logging::logMsg("C2C.log", "[CALENDAR] Era %d: Year = %d", iEra, tick);
	}
	if (weightedTicks == 0.0 && totalWeight == 0.0)
	{
		weightedTicks = 0;
		totalWeight = 1.0;
	}
	int currentTick = static_cast<int>(weightedTicks / totalWeight);
	int target_year = -200000 + (currentTick / 360);
	logging::logMsg("C2C.log", "[CALENDAR] Weighted Ticks = %f, Total Weight = %f, Current Ticks = %d", weightedTicks, totalWeight, currentTick);
	logging::logMsg("C2C.log", "[CALENDAR] TARGET_YEAR = %d", target_year);
	return currentTick;
}

int getTurnYearForGame(int iGameTurn, int iStartYear, CalendarTypes eCalendar, GameSpeedTypes eSpeed)
{
	if (eCalendar == CALENDAR_DEFAULT)
	{
		if (iGameTurn == GC.getGame().getGameTurn())
		{
			//logging::logMsg("C2C.log", "[CALENDAR] year: %d", GC.getGame().getCurrentDate().getYear());
			return GC.getGame().getCurrentDate().getYear();
		}
		
		return CvDate::getDate(iGameTurn, eSpeed).getYear();
	}
	//logging::logMsg("C2C.log", "[CALENDAR] its getTurnMonthForGame");
	return getTurnMonthForGame(iGameTurn, iStartYear, eCalendar, eSpeed) / std::max(1, GC.getNumMonthInfos());
}


int getTurnMonthForGame(int iGameTurn, int iStartYear, CalendarTypes eCalendar, GameSpeedTypes eSpeed)
{
	//int iTurnCount;
	//int iI;
	CvDate date;

	int iTurnMonth = iStartYear * GC.getNumMonthInfos();

	switch (eCalendar)
	{
	case CALENDAR_DEFAULT:
	case CALENDAR_NO_SEASONS:
		/*iTurnCount = 0;

		for (iI = 0; iI < GC.getGameSpeedInfo(eSpeed).getNumTurnIncrements(); iI++)
		{
			if (iGameTurn > (iTurnCount + GC.getGameSpeedInfo(eSpeed).getGameTurnInfo(iI).iNumGameTurnsPerIncrement))
			{
				iTurnMonth += (GC.getGameSpeedInfo(eSpeed).getGameTurnInfo(iI).iMonthIncrement * GC.getGameSpeedInfo(eSpeed).getGameTurnInfo(iI).iNumGameTurnsPerIncrement);
				iTurnCount += GC.getGameSpeedInfo(eSpeed).getGameTurnInfo(iI).iNumGameTurnsPerIncrement;
			}
			else
			{
				iTurnMonth += (GC.getGameSpeedInfo(eSpeed).getGameTurnInfo(iI).iMonthIncrement * (iGameTurn - iTurnCount));
				iTurnCount += (iGameTurn - iTurnCount);
				break;
			}
		}

		if (iGameTurn > iTurnCount)
		{
			iTurnMonth += (GC.getGameSpeedInfo(eSpeed).getGameTurnInfo(GC.getGameSpeedInfo(eSpeed).getNumTurnIncrements() - 1).iMonthIncrement * (iGameTurn - iTurnCount));
		}*/
		if (iGameTurn == GC.getGame().getGameTurn())
		{
			date = GC.getGame().getCurrentDate();
		}
		else
		{
			date = CvDate::getDate(iGameTurn, eSpeed);
		}
		return date.getYear() * 12 + date.getMonth();

		break;

	case CALENDAR_BI_YEARLY:
		iTurnMonth += (2 * iGameTurn * GC.getNumMonthInfos());
		break;

	case CALENDAR_YEARS:
	case CALENDAR_TURNS:
		iTurnMonth += iGameTurn * GC.getNumMonthInfos();
		break;

	case CALENDAR_SEASONS:
		iTurnMonth += (iGameTurn * GC.getNumMonthInfos()) / std::max(1, GC.getNumSeasonInfos());
		break;

	case CALENDAR_MONTHS:
		iTurnMonth += iGameTurn;
		break;

	case CALENDAR_WEEKS:
		iTurnMonth += iGameTurn / std::max(1, GC.getDefineINT("WEEKS_PER_MONTHS"));
		break;

	default:
		FErrorMsg("error");
	}

	return iTurnMonth;
}

// these string functions should only be used under chipotle cheat code (not internationalized)

void getDirectionTypeString(CvWString& szString, DirectionTypes eDirectionType)
{
	switch (eDirectionType)
	{
	case NO_DIRECTION: szString = L"NO_DIRECTION"; break;

	case DIRECTION_NORTH: szString = L"north"; break;
	case DIRECTION_NORTHEAST: szString = L"northeast"; break;
	case DIRECTION_EAST: szString = L"east"; break;
	case DIRECTION_SOUTHEAST: szString = L"southeast"; break;
	case DIRECTION_SOUTH: szString = L"south"; break;
	case DIRECTION_SOUTHWEST: szString = L"southwest"; break;
	case DIRECTION_WEST: szString = L"west"; break;
	case DIRECTION_NORTHWEST: szString = L"northwest"; break;

	default: szString = CvWString::format(L"UNKNOWN_DIRECTION(%d)", eDirectionType); break;
	}
}

void getCardinalDirectionTypeString(CvWString& szString, CardinalDirectionTypes eDirectionType)
{
	getDirectionTypeString(szString, cardinalDirectionToDirection(eDirectionType));
}

void getActivityTypeString(CvWString& szString, ActivityTypes eActivityType)
{
	switch (eActivityType)
	{
	case NO_ACTIVITY: szString = L"NO_ACTIVITY"; break;

	case ACTIVITY_AWAKE: szString = L"ACTIVITY_AWAKE"; break;
	case ACTIVITY_HOLD: szString = L"ACTIVITY_HOLD"; break;
	case ACTIVITY_SLEEP: szString = L"ACTIVITY_SLEEP"; break;
	case ACTIVITY_HEAL: szString = L"ACTIVITY_HEAL"; break;
	case ACTIVITY_SENTRY: szString = L"ACTIVITY_SENTRY"; break;
#ifdef _MOD_SENTRY
	case ACTIVITY_SENTRY_WHILE_HEAL: szString = L"ACTIVITY_SENTRY_WHILE_HEAL"; break;
	case ACTIVITY_SENTRY_NAVAL_UNITS: szString = L"ACTIVITY_SENTRY_NAVAL_UNITS"; break;
	case ACTIVITY_SENTRY_LAND_UNITS: szString = L"ACTIVITY_SENTRY_LAND_UNITS"; break;
#endif
	case ACTIVITY_INTERCEPT: szString = L"ACTIVITY_INTERCEPT"; break;
	case ACTIVITY_MISSION: szString = L"ACTIVITY_MISSION"; break;
	case ACTIVITY_PATROL: szString = L"ACTIVITY_PATROL";  break;
	case ACTIVITY_PLUNDER: szString = L"ACTIVITY_PLUNDER";  break;

	default: szString = CvWString::format(L"UNKNOWN_ACTIVITY(%d)", eActivityType); break;
	}
}

void getMissionTypeString(CvWString& szString, MissionTypes eMissionType)
{
	switch (eMissionType)
	{
	case NO_MISSION: szString = L"NO_MISSION"; break;

	case MISSION_MOVE_TO: szString = L"MISSION_MOVE_TO"; break;
#ifdef _MOD_SENTRY
	case MISSION_MOVE_TO_SENTRY: szString = L"MISSION_MOVE_TO_SENTRY"; break;
#endif
	case MISSION_ROUTE_TO: szString = L"MISSION_ROUTE_TO"; break;
	case MISSION_MOVE_TO_UNIT: szString = L"MISSION_MOVE_TO_UNIT"; break;
	case MISSION_SKIP: szString = L"MISSION_SKIP"; break;
	case MISSION_SLEEP: szString = L"MISSION_SLEEP"; break;
	case MISSION_FORTIFY: szString = L"MISSION_FORTIFY"; break;
	case MISSION_BUILDUP: szString = L"MISSION_BUILDUP"; break;
	case MISSION_PLUNDER: szString = L"MISSION_PLUNDER"; break;
	case MISSION_AIRPATROL: szString = L"MISSION_AIRPATROL"; break;
	case MISSION_SEAPATROL: szString = L"MISSION_SEAPATROL"; break;
	case MISSION_HEAL: szString = L"MISSION_HEAL"; break;
	case MISSION_SENTRY: szString = L"MISSION_SENTRY"; break;
#ifdef _MOD_SENTRY
	case MISSION_SENTRY_WHILE_HEAL: szString = L"MISSION_SENTRY_WHILE_HEAL"; break;
	case MISSION_SENTRY_NAVAL_UNITS: szString = L"MISSION_SENTRY_NAVAL_UNITS"; break;
	case MISSION_SENTRY_LAND_UNITS: szString = L"MISSION_SENTRY_LAND_UNITS"; break;
#endif
	case MISSION_AIRLIFT: szString = L"MISSION_AIRLIFT"; break;
	case MISSION_NUKE: szString = L"MISSION_NUKE"; break;
	case MISSION_RECON: szString = L"MISSION_RECON"; break;
	case MISSION_PARADROP: szString = L"MISSION_PARADROP"; break;
	case MISSION_AIRBOMB: szString = L"MISSION_AIRBOMB"; break;
	case MISSION_BOMBARD: szString = L"MISSION_BOMBARD"; break;
	case MISSION_PILLAGE: szString = L"MISSION_PILLAGE"; break;
	case MISSION_SABOTAGE: szString = L"MISSION_SABOTAGE"; break;
	case MISSION_DESTROY: szString = L"MISSION_DESTROY"; break;
	case MISSION_STEAL_PLANS: szString = L"MISSION_STEAL_PLANS"; break;
	case MISSION_FOUND: szString = L"MISSION_FOUND"; break;
	case MISSION_SPREAD: szString = L"MISSION_SPREAD"; break;
	case MISSION_SPREAD_CORPORATION: szString = L"MISSION_SPREAD_CORPORATION"; break;
	case MISSION_JOIN: szString = L"MISSION_JOIN"; break;
	case MISSION_CONSTRUCT: szString = L"MISSION_CONSTRUCT"; break;
	case MISSION_HERITAGE: szString = L"MISSION_HERITAGE"; break;
	case MISSION_DISCOVER: szString = L"MISSION_DISCOVER"; break;
	case MISSION_HURRY: szString = L"MISSION_HURRY"; break;
	case MISSION_TRADE: szString = L"MISSION_TRADE"; break;
	case MISSION_GREAT_WORK: szString = L"MISSION_GREAT_WORK"; break;
	case MISSION_INFILTRATE: szString = L"MISSION_INFILTRATE"; break;
	case MISSION_GOLDEN_AGE: szString = L"MISSION_GOLDEN_AGE"; break;
	case MISSION_BUILD: szString = L"MISSION_BUILD"; break;
	case MISSION_LEAD: szString = L"MISSION_LEAD"; break;
	case MISSION_ESPIONAGE: szString = L"MISSION_ESPIONAGE"; break;
	case MISSION_DIE_ANIMATION: szString = L"MISSION_DIE_ANIMATION"; break;
	case MISSION_BEGIN_COMBAT: szString = L"MISSION_BEGIN_COMBAT"; break;
	case MISSION_END_COMBAT: szString = L"MISSION_END_COMBAT"; break;
	case MISSION_AIRSTRIKE: szString = L"MISSION_AIRSTRIKE"; break;
	case MISSION_SURRENDER: szString = L"MISSION_SURRENDER"; break;
	case MISSION_CAPTURED: szString = L"MISSION_CAPTURED"; break;
	case MISSION_IDLE: szString = L"MISSION_IDLE"; break;
	case MISSION_DIE: szString = L"MISSION_DIE"; break;
	case MISSION_DAMAGE: szString = L"MISSION_DAMAGE"; break;
	case MISSION_MULTI_SELECT: szString = L"MISSION_MULTI_SELECT"; break;
	case MISSION_MULTI_DESELECT: szString = L"MISSION_MULTI_DESELECT"; break;
	case MISSION_HURRY_FOOD: szString = L"MISSION_HURRY_FOOD"; break;
	case MISSION_INQUISITION: szString = L"MISSION_INQUISITION"; break;
	case MISSION_CLAIM_TERRITORY: szString = L"MISSION_CLAIM_TERRITORY"; break;
	case MISSION_ESPIONAGE_SLEEP: szString = L"MISSION_ESPIONAGE_SLEEP"; break;
	case MISSION_GREAT_COMMANDER: szString = L"MISSION_GREAT_COMMANDER"; break;
	case MISSION_GREAT_COMMODORE: szString = L"MISSION_GREAT_COMMODORE"; break;
	case MISSION_SHADOW: szString = L"MISSION_SHADOW"; break;
	case MISSION_GOTO: szString = L"MISSION_GOTO"; break;
	case MISSION_BUTCHER: szString = L"MISSION_BUTCHER"; break;
	case MISSION_DIPLOMAT_ASSIMULATE_IND_PEOPLE: szString = L"MISSION_DIPLOMAT_ASSIMULATE_IND_PEOPLE"; break;
	case MISSION_DIPLOMAT_PRAISE_IND_PEOPLE: szString = L"MISSION_DIPLOMAT_PRAISE_IND_PEOPLE"; break;
	case MISSION_DIPLOMAT_SPEAK_TO_BARBARIAN_LEADERS: szString = L"MISSION_DIPLOMAT_SPEAK_TO_BARBARIAN_LEADERS"; break;
	case MISSION_DIPLOMAT_SPREAD_RELIGION: szString = L"MISSION_DIPLOMAT_SPREAD_RELIGION"; break;
	case MISSION_LAWYER_REMOVE_CORPORATIONS: szString = L"MISSION_LAWYER_REMOVE_CORPORATIONS"; break;
	case MISSION_JOIN_CITY_POPULATION: szString = L"MISSION_JOIN_CITY_POPULATION"; break;
	case MISSION_CURE: szString = L"MISSION_CURE"; break;
	case MISSION_AUTO_BUILDUP: szString = L"MISSION_AUTO_BUILDUP"; break;
	case MISSION_HEAL_BUILDUP: szString = L"MISSION_HEAL_BUILDUP"; break;
	case MISSION_AMBUSH: szString = L"MISSION_AMBUSH"; break;
	case MISSION_ASSASSINATE: szString = L"MISSION_ASSASSINATE"; break;
	case MISSION_ENTERTAIN_CITY: szString = L"MISSION_ENTERTAIN_CITY"; break;
	case MISSION_HURRY_PRODUCTION_CARAVAN: szString = L"MISSION_HURRY_PRODUCTION_CARAVAN"; break;
	case MISSION_CAPTIVE_UPGRADE_TO_SETTLER: szString = L"MISSION_CAPTIVE_UPGRADE_TO_SETTLER"; break;
	case MISSION_CAPTIVE_UPGRADE_TO_GATHERER: szString = L"MISSION_CAPTIVE_UPGRADE_TO_GATHERER"; break;
	case MISSION_CAPTIVE_UPGRADE_TO_WORKER: szString = L"MISSION_CAPTIVE_UPGRADE_TO_WORKER"; break;
	case MISSION_CAPTIVE_UPGRADE_TO_IMMIGRANT: szString = L"MISSION_CAPTIVE_UPGRADE_TO_IMMIGRANT"; break;
	case MISSION_CAPTIVE_UPGRADE_TO_STONE_THROWER: szString = L"MISSION_CAPTIVE_UPGRADE_TO_STONE_THROWER"; break;
	case MISSION_CAPTIVE_UPGRADE_TO_ARCHER: szString = L"MISSION_CAPTIVE_UPGRADE_TO_ARCHER"; break;
	case MISSION_CAPTIVE_UPGRADE_TO_AXEMAN: szString = L"MISSION_CAPTIVE_UPGRADE_TO_AXEMAN"; break;
	case MISSION_CAPTIVE_UPGRADE_TO_SPEARMAN: szString = L"MISSION_CAPTIVE_UPGRADE_TO_SPEARMAN"; break;
	case MISSION_CAPTIVE_UPGRADE_TO_NEANDERTHAL_WARRIOR: szString = L"MISSION_CAPTIVE_UPGRADE_TO_NEANDERTHAL_WARRIOR"; break;
	case MISSION_SELL_CAPTIVE: szString = L"MISSION_SELL_CAPTIVE"; break;
	case MISSION_FREE_CAPTIVE: szString = L"MISSION_FREE_CAPTIVE"; break;
	case MISSION_SLAY_ANIMAL: szString = L"MISSION_SLAY_ANIMAL"; break;
	case MISSION_JOIN_CITY_FREED_SLAVE: szString = L"MISSION_JOIN_CITY_FREED_SLAVE"; break;
	case MISSION_RECORD_TALE: szString = L"MISSION_RECORD_TALE"; break;
	case MISSION_RECORD_TALE_ORAL: szString = L"MISSION_RECORD_TALE_ORAL"; break;
	case MISSION_RECORD_TALE_WRITTEN: szString = L"MISSION_RECORD_TALE_WRITTEN"; break;
	case MISSION_ANIMAL_COMBAT: szString = L"MISSION_ANIMAL_COMBAT"; break;
	case MISSION_ANIMAL_STUDY: szString = L"MISSION_ANIMAL_STUDY"; break;
	case MISSION_ANIMAL_SACRIFICE: szString = L"MISSION_ANIMAL_SACRIFICE"; break;
	case MISSION_BUILD_DOMESTICATED_HERD: szString = L"MISSION_BUILD_DOMESTICATED_HERD"; break;
	case MISSION_RANGE_ATTACK: szString = L"MISSION_RANGE_ATTACK"; break;
	case MISSION_AIRBOMB1: szString = L"MISSION_AIRBOMB1"; break;
	case MISSION_AIRBOMB2: szString = L"MISSION_AIRBOMB2"; break;
	case MISSION_AIRBOMB3: szString = L"MISSION_AIRBOMB3"; break;
	case MISSION_AIRBOMB4: szString = L"MISSION_AIRBOMB4"; break;
	case MISSION_AIRBOMB5: szString = L"MISSION_AIRBOMB5"; break;
	case MISSION_RBOMBARD: szString = L"MISSION_RBOMBARD"; break;
	case MISSION_FENGAGE: szString = L"MISSION_FENGAGE"; break;
	case MISSION_CAPTIVE_UPGRADE_TO_NEANDERTHAL_GATHERER: szString = L"MISSION_CAPTIVE_UPGRADE_TO_NEANDERTHAL_GATHERER"; break;
	case MISSION_CAPTIVE_UPGRADE_TO_NEANDERTHAL_TRACKER: szString = L"MISSION_CAPTIVE_UPGRADE_TO_NEANDERTHAL_TRACKER"; break;

	default: szString = CvWString::format(L"UNKOWN_MISSION(%d)", eMissionType); break;
	}
}

void getMissionAIString(CvWString& szString, MissionAITypes eMissionAI)
{
	switch (eMissionAI)
	{
	case NO_MISSIONAI: szString = L"NO_MISSIONAI"; break;

	case MISSIONAI_SHADOW: szString = L"MISSIONAI_SHADOW"; break;
	case MISSIONAI_GROUP: szString = L"MISSIONAI_GROUP"; break;
	case MISSIONAI_LOAD_ASSAULT: szString = L"MISSIONAI_LOAD_ASSAULT"; break;
	case MISSIONAI_LOAD_SETTLER: szString = L"MISSIONAI_LOAD_SETTLER"; break;
	case MISSIONAI_LOAD_SPECIAL: szString = L"MISSIONAI_LOAD_SPECIAL"; break;
	case MISSIONAI_GUARD_CITY: szString = L"MISSIONAI_GUARD_CITY"; break;
	case MISSIONAI_GUARD_BONUS: szString = L"MISSIONAI_GUARD_BONUS"; break;
	case MISSIONAI_GUARD_SPY: szString = L"MISSIONAI_GUARD_SPY"; break;
	case MISSIONAI_ATTACK_SPY: szString = L"MISSIONAI_ATTACK_SPY"; break;
	case MISSIONAI_SPREAD: szString = L"MISSIONAI_SPREAD"; break;
	case MISSIONAI_CONSTRUCT: szString = L"MISSIONAI_CONSTRUCT"; break;
	case MISSIONAI_HURRY: szString = L"MISSIONAI_HURRY"; break;
	case MISSIONAI_GREAT_WORK: szString = L"MISSIONAI_GREAT_WORK"; break;
	case MISSIONAI_EXPLORE: szString = L"MISSIONAI_EXPLORE"; break;
	case MISSIONAI_BLOCKADE: szString = L"MISSIONAI_BLOCKADE"; break;
	case MISSIONAI_PILLAGE: szString = L"MISSIONAI_PILLAGE"; break;
	case MISSIONAI_FOUND: szString = L"MISSIONAI_FOUND"; break;
	case MISSIONAI_BUILD: szString = L"MISSIONAI_BUILD"; break;
	case MISSIONAI_ASSAULT: szString = L"MISSIONAI_ASSAULT"; break;
	case MISSIONAI_CARRIER: szString = L"MISSIONAI_CARRIER"; break;
	case MISSIONAI_PICKUP: szString = L"MISSIONAI_PICKUP"; break;
	case MISSIONAI_CLAIM_TERRITORY: szString = L"MISSIONAI_CLAIM_TERRITORY"; break;
	case MISSIONAI_HURRY_FOOD: szString = L"MISSIONAI_HURRY_FOOD"; break;
	case MISSIONAI_INQUISITION: szString = L"MISSIONAI_INQUISITION"; break;
	case MISSIONAI_CONTRACT: szString = L"MISSIONAI_CONTRACT"; break;
	case MISSIONAI_CONTRACT_UNIT: szString = L"MISSIONAI_CONTRACT_UNIT"; break;
	case MISSIONAI_DELIBERATE_KILL: szString = L"MISSIONAI_DELIBERATE_KILL"; break;
	case MISSIONAI_REGROUP: szString = L"MISSIONAI_REGROUP"; break;
	case MISSIONAI_HEAL_SUPPORT: szString = L"MISSIONAI_HEAL_SUPPORT"; break;
	case MISSIONAI_PROPERTY_CONTROL_RESPONSE: szString = L"MISSIONAI_PROPERTY_CONTROL_RESPONSE"; break;
	case MISSIONAI_PROPERTY_CONTROL_MAINTAIN: szString = L"MISSIONAI_PROPERTY_CONTROL_MAINTAIN"; break;
	case MISSIONAI_INVESTIGATOR_RESPONSE: szString = L"MISSIONAI_INVESTIGATOR_RESPONSE"; break;
	case MISSIONAI_INVESTIGATOR_MAINTAIN: szString = L"MISSIONAI_INVESTIGATOR_MAINTAIN"; break;
	case MISSIONAI_INFILTRATOR_MAINTAIN: szString = L"MISSIONAI_INFILTRATOR_MAINTAIN"; break;
	case MISSIONAI_SEE_INVISIBLE_MAINTAIN: szString = L"MISSIONAI_SEE_INVISIBLE_MAINTAIN"; break;
	case MISSIONAI_SEE_INVISIBLE_SEA_MAINTAIN: szString = L"MISSIONAI_SEE_INVISIBLE_SEA_MAINTAIN"; break;
	case MISSIONAI_WAIT_FOR_ESCORT: szString = L"MISSIONAI_WAIT_FOR_ESCORT"; break;
	case MISSIONAI_WAIT_FOR_SEE_INVISIBLE: szString = L"MISSIONAI_WAIT_FOR_SEE_INVISIBLE"; break;
	case MISSIONAI_GUARD_TRADE_NET: szString = L"MISSIONAI_GUARD_TRADE_NET"; break;
	case MISSIONAI_SPREAD_CORPORATION: szString = L"MISSIONAI_SPREAD_CORPORATION"; break;

	default: szString = CvWString::format(L"UNKNOWN_MISSION_AI(%d)", eMissionAI); break;
	}
}

void getUnitAIString(CvWString& szString, UnitAITypes eUnitAI)
{
	// note, GC.getUnitAIInfo(eUnitAI).getDescription() is a international friendly way to get string (but it will be longer)

	switch (eUnitAI)
	{
	case NO_UNITAI: szString = L""; break;

	case UNITAI_UNKNOWN: szString = L"Unknown"; break;
	case UNITAI_ANIMAL: szString = L"Animal"; break;
	case UNITAI_SETTLE: szString = L"Settle"; break;
	case UNITAI_WORKER: szString = L"Worker"; break;
	case UNITAI_ATTACK: szString = L"Attack"; break;
	case UNITAI_ATTACK_CITY: szString = L"Attack city"; break;
	case UNITAI_COLLATERAL: szString = L"Collateral"; break;
	case UNITAI_PILLAGE: szString = L"Pillage"; break;
	case UNITAI_RESERVE: szString = L"Reserve"; break;
	case UNITAI_COUNTER: szString = L"Counter"; break;
	case UNITAI_CITY_DEFENSE: szString = L"City defense"; break;
	case UNITAI_CITY_COUNTER: szString = L"City counter"; break;
	case UNITAI_CITY_SPECIAL: szString = L"City special"; break;
	case UNITAI_EXPLORE: szString = L"Explore"; break;
	case UNITAI_MISSIONARY: szString = L"Missionary"; break;
	case UNITAI_PROPHET: szString = L"Prophet"; break;
	case UNITAI_ARTIST: szString = L"Artist"; break;
	case UNITAI_SCIENTIST: szString = L"Scientist"; break;
	case UNITAI_GENERAL: szString = L"General"; break;
	case UNITAI_HUNTER: szString = L"Hunter"; break;
	case UNITAI_GREAT_HUNTER: szString = L"Great hunter"; break;
	case UNITAI_GREAT_ADMIRAL: szString = L"Great admiral"; break;
	case UNITAI_MERCHANT: szString = L"Merchant"; break;
	case UNITAI_ENGINEER: szString = L"Engineer"; break;
	case UNITAI_SPY: szString = L"Spy"; break;
	case UNITAI_ICBM: szString = L"Icbm"; break;
	case UNITAI_WORKER_SEA: szString = L"Worker sea"; break;
	case UNITAI_ATTACK_SEA: szString = L"Attack sea"; break;
	case UNITAI_RESERVE_SEA: szString = L"Reserve sea"; break;
	case UNITAI_ESCORT_SEA: szString = L"Escort sea"; break;
	case UNITAI_EXPLORE_SEA: szString = L"Explore sea"; break;
	case UNITAI_ASSAULT_SEA: szString = L"Assault sea"; break;
	case UNITAI_SETTLER_SEA: szString = L"Settler sea"; break;
	case UNITAI_MISSIONARY_SEA: szString = L"Missionary sea"; break;
	case UNITAI_SPY_SEA: szString = L"Spy sea"; break;
	case UNITAI_CARRIER_SEA: szString = L"Carrier sea"; break;
	case UNITAI_MISSILE_CARRIER_SEA: szString = L"Missile carrier"; break;
	case UNITAI_PIRATE_SEA: szString = L"Pirate sea"; break;
	case UNITAI_ATTACK_AIR: szString = L"Attack air"; break;
	case UNITAI_DEFENSE_AIR: szString = L"Defense air"; break;
	case UNITAI_CARRIER_AIR: szString = L"Carrier air"; break;
	case UNITAI_PARADROP: szString = L"Paradrop"; break;
	case UNITAI_ATTACK_CITY_LEMMING: szString = L"Attack city lemming"; break;
	case UNITAI_PROPERTY_CONTROL: szString = L"Property control"; break;
	case UNITAI_HEALER: szString = L"Healer"; break;
	case UNITAI_PROPERTY_CONTROL_SEA: szString = L"Property control sea"; break;
	case UNITAI_HEALER_SEA: szString = L"Healer sea"; break;
	case UNITAI_HUNTER_ESCORT: szString = L"Hunter escort"; break;
	case UNITAI_BARB_CRIMINAL: szString = L"Barb criminal"; break;
	case UNITAI_INVESTIGATOR: szString = L"Investigator"; break;
	case UNITAI_INFILTRATOR: szString = L"Infiltrator"; break;
	case UNITAI_SEE_INVISIBLE: szString = L"See invisible"; break;
	case UNITAI_SEE_INVISIBLE_SEA: szString = L"See invisible sea"; break;
	case UNITAI_ESCORT: szString = L"Escort"; break;
	case UNITAI_MISSILE_AIR: szString = L"Missile air"; break;
	case UNITAI_PILLAGE_COUNTER: szString = L"Pillage counter"; break;
	case UNITAI_SUBDUED_ANIMAL: szString = L"Subdued animal"; break;

	default: szString = CvWString::format(L"unknown(%d)", eUnitAI); break;
	}
}

/*
 * Calculates the experience needed to reach the next level after the given level.
 */
int calcBaseExpNeeded(const int iLevel, const PlayerTypes ePlayer)
{
	int iThreshold = 99 + (iLevel*iLevel + 1) * (100 + GET_PLAYER(ePlayer).getLevelExperienceModifier());

	if (GC.getGame().isOption(GAMEOPTION_UNIT_MORE_XP_TO_LEVEL))
	{
		iThreshold *= GC.getDefineINT("MORE_XP_TO_LEVEL_MODIFIER");
		iThreshold /= 100;
	}
	return iThreshold / 100;
}

/*
 * Calculates the level for a unit with the given experience.
 */
int calculateLevel(const int iExperience, const PlayerTypes ePlayer)
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(ePlayer != NO_PLAYER, "ePlayer must be a valid player");

	if (iExperience <= 0)
	{
		return 1;
	}

	int iLevel = 1;
	while (true)
	{
		const int iNextLevelExperience = calcBaseExpNeeded(iLevel, ePlayer);
		if (iNextLevelExperience > iExperience)
		{
			break;
		}
		++iLevel;
		if (iNextLevelExperience == iExperience)
		{
			break;
		}
	}
	return iLevel;
}


DirectionTypes getOppositeDirection(DirectionTypes eDirection)
{
	switch(eDirection)
	{
	case DIRECTION_NORTH:
		return DIRECTION_SOUTH;
	case DIRECTION_NORTHEAST:
		return DIRECTION_SOUTHWEST;
	case DIRECTION_EAST:
		return DIRECTION_WEST;
	case DIRECTION_SOUTHEAST:
		return DIRECTION_NORTHWEST;
	case DIRECTION_SOUTH:
		return DIRECTION_NORTH;
	case DIRECTION_SOUTHWEST:
		return DIRECTION_NORTHEAST;
	case DIRECTION_WEST:
		return DIRECTION_EAST;
	case DIRECTION_NORTHWEST:
		return DIRECTION_SOUTHEAST;
	default:
		return NO_DIRECTION;
	}
}

bool isAdjacentDirection(DirectionTypes eFacingDirection, DirectionTypes eOtherDirection)
{
	switch(eFacingDirection)
	{
	case DIRECTION_NORTH:
		return eOtherDirection == DIRECTION_NORTHEAST || eOtherDirection == DIRECTION_NORTHWEST;
	case DIRECTION_NORTHEAST:
		return eOtherDirection == DIRECTION_NORTH || eOtherDirection == DIRECTION_EAST;
	case DIRECTION_EAST:
		return eOtherDirection == DIRECTION_NORTHEAST || eOtherDirection == DIRECTION_SOUTHEAST;
	case DIRECTION_SOUTHEAST:
		return eOtherDirection == DIRECTION_EAST || eOtherDirection == DIRECTION_SOUTH;
	case DIRECTION_SOUTH:
		return eOtherDirection == DIRECTION_SOUTHEAST || eOtherDirection == DIRECTION_SOUTHWEST;
	case DIRECTION_SOUTHWEST:
		return eOtherDirection == DIRECTION_SOUTH || eOtherDirection == DIRECTION_WEST;
	case DIRECTION_WEST:
		return eOtherDirection == DIRECTION_SOUTHWEST || eOtherDirection == DIRECTION_NORTHWEST;
	case DIRECTION_NORTHWEST:
		return eOtherDirection == DIRECTION_NORTH || eOtherDirection == DIRECTION_WEST;
	}
	return false;
}


//	Koshling - abstract treaty length from the define int to allow scaling
int getTreatyLength()
{
	int iResult = GC.getDefineINT("PEACE_TREATY_LENGTH");

	iResult *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
	iResult /= 100;

	return std::max(1, iResult);
}

void CvChecksum::add(int i)
{
	PROFILE_EXTRA_FUNC();
	union { int value; uint8_t bytes[4]; } data;
	data.value = i;
	for(UINT i = 0; i < sizeof(data.bytes); i++)
	{
		add(data.bytes[i]);
	}
}

void CvChecksum::add(uint8_t b)
{
	uint8_t cipher = (b ^ (r >> 8));
	r = (cipher + r) * c1 + c2;
	sum = (sum << 8) + ((sum >> 24) ^ cipher);
}

#include "CyArgsList.h"

void AddDLLMessage(
	PlayerTypes ePlayer, bool bForce, int iLength, CvWString szString, const char* pszSound,
	InterfaceMessageTypes eType, LPCSTR pszIcon, ColorTypes eFlashColor,
	int iFlashX, int iFlashY, bool bShowOffScreenArrows, bool bShowOnScreenArrows)
{
#ifdef _DEBUG
	OutputDebugString(CvString::format("DLLMessage: %S\n", szString.c_str()).c_str());
#else
	if (bForce)
	{
		OutputDebugString(CvString::format("DLLMessage: %S\n", szString.c_str()).c_str());
	}
#endif
	Cy::call(
		PYScreensModule, "sendMessage",
		Cy::Args()
		<< szString
		<< ePlayer
		<< iLength
		<< (pszIcon != NULL ? pszIcon : "")
		<< eFlashColor
		<< iFlashX
		<< iFlashY
		<< bShowOffScreenArrows
		<< bShowOnScreenArrows
		<< eType
		<< (pszSound != NULL ? pszSound : "")
		<< bForce
	);
}


// Toffer - If a value with decimals can be negative, it needs to be handled something like this to get the minus sign correctly in text.
void makeValueString(CvWString& szValue, const int iValue, const bool bWholeNumberCutDecimals, int iNumDecimals)
{
	if (bWholeNumberCutDecimals && iValue % 100 == 0)
	{
		szValue.Format(L"%d", iValue / 100);
	}
	else
	{
		//iNumDecimals = range(iNumDecimals, 0, 9);
		if (iValue < 0)
		{
			if (iValue / 100 == 0)
			{
				szValue.Format(L"-0.%02d", abs(iValue) % 100);
			}
			else szValue.Format(L"%d.%02d", iValue / 100, abs(iValue) % 100);
		}
		else szValue.Format(L"%d.%02d", iValue / 100, iValue % 100);
	}
}
// ! Toffer

	// Scoring helper function to attenuate score by distance, with optional boost for current plot
int applyDistanceScoringFactor(int score, const CvPlot* sourcePlot, const CvPlot* targetPlot, const int currentPlotBoost)
{
	FAssert(sourcePlot != NULL);
	FAssert(targetPlot != NULL);

	if (score > 0)
	{
		if (sourcePlot != targetPlot)
		{
			// Has to be > 0 because we aren't on the same plot...
			PlayerTypes Refplayer = sourcePlot->getOwner();
			int dist_break = 8;

			//if plot isn't owned, grab the first player (to get current era)
			if (Refplayer == NO_PLAYER)
			{
				for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
				{
					if (GET_PLAYER((PlayerTypes)iPlayer).isAlive())
					{
						Refplayer = (PlayerTypes)iPlayer;
						break;
					}
				}
			}
			dist_break += (int)GET_PLAYER(Refplayer).getCurrentEra();  //Calvitix TODO : dynamic, depending on map size ?

			const int dist = stepDistance(sourcePlot->getX(), sourcePlot->getY(), targetPlot->getX(), targetPlot->getY());
			float d0 = 5.0f;
			float p = 0.4f;
			float expfact = 1.5f;
			if (dist <= dist_break)
			{
				score = static_cast<int>(score * pow(d0 / (d0 + dist), p));
			}
			else
			{
				float extra_dist = static_cast<float>(dist - dist_break);
				score = static_cast<int>(score * pow(d0 / (d0 + dist), p) * std::exp(-0.1 * pow(extra_dist, expfact)));
			}
		}
		else
		{
			score *= currentPlotBoost;
		}
	}
	return score;
}


//
// === Partie 1 : encodage bitwise (a ∈ [1,100000], b ∈ [0,9999]) ===
//
int encodeACDateturn(int decodeda, int b) {
	
	if (decodeda < -400000 || decodeda > 7000) throw std::out_of_range("a hors limites [-400000,6000]");
	int a = inverseMapValue(decodeda * -1);
	if (a < 1) a = 1;
	if (a > 100000) a = 100000;// throw std::out_of_range("a hors limites [1,100000]");
	if (b < 0) b = 0;
	if (b > 9999) b = 9999;
	return (a << 14) | b;  // 14 bits réservés pour b
}

int decodeACDate(int n) {
	int codedDate = n >> 14;
	
	int decodedDate = mapValue(codedDate) * -1 ;
	
	if (abs(decodedDate) > 10000)
		return (decodedDate / 100) * 100;
	if (abs(decodedDate) > 1000)
		return (decodedDate / 10) * 10;
	return decodedDate;
}

int decodeACTurn(int n) {
	int codedTurn = n & ((1 << 14) - 1);
	return codedTurn;

}

//
// === Partie 2 : mapping non-linéaire pour b ===
//

// Paramètres globaux
const int N = 9999;
const double ymin = -7000.0;
const double ymax = 400000.0;
const double A = ymax - ymin;

// Portion linéaire pour obtenir un pas ≈ 15 sur [-6000,6000]
const double t0 = 800.0 / 9999.0;
const double f = (5000.0 - ymin) / A;

// Constante k calculée numériquement (assure continuité de la dérivée)
const double k = 1.8314001073353914;
const double expk = std::exp(k);

// fonction auxiliaire S(t)
static double S_of_t(double t) {
	if (t <= t0) {
		return (f / t0) * t;
	}
	else {
		double u = (t - t0) / (1.0 - t0);
		double frac = (std::exp(k * u) - 1.0) / (expk - 1.0);
		return f + (1.0 - f) * frac;
	}
}

// conversion avant : x ∈ [0,9999] → y ∈ [-6000,400000]
int mapValue(int x) {
	if (x < 0) x = 0;
	if (x > N) x = N;
	double t = static_cast<double>(x) / static_cast<double>(N);
	double y = ymin + A * S_of_t(t);
	return static_cast<int>(y >= 0 ? y + 0.5 : y - 0.5);
}

// conversion inverse : y ∈ [-6000,400000] → x ∈ [0,9999]
int inverseMapValue(int y) {
	if (y < ymin) return 0;
	if (y > ymax) return N;

	// recherche dichotomique sur x
	int low = 0, high = N;
	while (low < high) {
		int mid = (low + high) / 2;
		int val = mapValue(mid);
		if (val < y) {
			low = mid + 1;
		}
		else {
			high = mid;
		}
	}
	return low; // plus petit x tel que mapValue(x) >= y
}


CvString MissionAITypeToString(MissionAITypes eMissionAI)
{
	switch (eMissionAI)
	{
	case NO_MISSIONAI:                         return "NO_MISSIONAI";
	case MISSIONAI_SHADOW:                     return "MISSIONAI_SHADOW";
	case MISSIONAI_GROUP:                      return "MISSIONAI_GROUP";
	case MISSIONAI_LOAD_ASSAULT:               return "MISSIONAI_LOAD_ASSAULT";
	case MISSIONAI_LOAD_SETTLER:               return "MISSIONAI_LOAD_SETTLER";
	case MISSIONAI_LOAD_SPECIAL:               return "MISSIONAI_LOAD_SPECIAL";
	case MISSIONAI_GUARD_CITY:                 return "MISSIONAI_GUARD_CITY";
	case MISSIONAI_GUARD_BONUS:                return "MISSIONAI_GUARD_BONUS";
	case MISSIONAI_GUARD_TRADE_NET:            return "MISSIONAI_GUARD_TRADE_NET";
	case MISSIONAI_GUARD_SPY:                  return "MISSIONAI_GUARD_SPY";
	case MISSIONAI_ATTACK_SPY:                 return "MISSIONAI_ATTACK_SPY";
	case MISSIONAI_SPREAD:                     return "MISSIONAI_SPREAD";
	case MISSIONAI_SPREAD_CORPORATION:         return "MISSIONAI_SPREAD_CORPORATION";
	case MISSIONAI_CONSTRUCT:                  return "MISSIONAI_CONSTRUCT";
	case MISSIONAI_HURRY:                      return "MISSIONAI_HURRY";
	case MISSIONAI_GREAT_WORK:                 return "MISSIONAI_GREAT_WORK";
	case MISSIONAI_EXPLORE:                    return "MISSIONAI_EXPLORE";
	case MISSIONAI_BLOCKADE:                   return "MISSIONAI_BLOCKADE";
	case MISSIONAI_PILLAGE:                    return "MISSIONAI_PILLAGE";
	case MISSIONAI_FOUND:                      return "MISSIONAI_FOUND";
	case MISSIONAI_BUILD:                      return "MISSIONAI_BUILD";
	case MISSIONAI_ASSAULT:                    return "MISSIONAI_ASSAULT";
	case MISSIONAI_CARRIER:                    return "MISSIONAI_CARRIER";
	case MISSIONAI_PICKUP:                     return "MISSIONAI_PICKUP";
	case MISSIONAI_INQUISITION:                return "MISSIONAI_INQUISITION";
	case MISSIONAI_CLAIM_TERRITORY:            return "MISSIONAI_CLAIM_TERRITORY";
	case MISSIONAI_HURRY_FOOD:                 return "MISSIONAI_HURRY_FOOD";
	case MISSIONAI_CONTRACT:                   return "MISSIONAI_CONTRACT";
	case MISSIONAI_CONTRACT_UNIT:              return "MISSIONAI_CONTRACT_UNIT";
	case MISSIONAI_DELIBERATE_KILL:            return "MISSIONAI_DELIBERATE_KILL";
	case MISSIONAI_REGROUP:                    return "MISSIONAI_REGROUP";
	case MISSIONAI_HEAL_SUPPORT:               return "MISSIONAI_HEAL_SUPPORT";
	case MISSIONAI_PROPERTY_CONTROL_RESPONSE:  return "MISSIONAI_PROPERTY_CONTROL_RESPONSE";
	case MISSIONAI_PROPERTY_CONTROL_MAINTAIN:  return "MISSIONAI_PROPERTY_CONTROL_MAINTAIN";
	case MISSIONAI_INVESTIGATOR_RESPONSE:      return "MISSIONAI_INVESTIGATOR_RESPONSE";
	case MISSIONAI_INVESTIGATOR_MAINTAIN:      return "MISSIONAI_INVESTIGATOR_MAINTAIN";
	case MISSIONAI_INFILTRATOR_MAINTAIN:       return "MISSIONAI_INFILTRATOR_MAINTAIN";
	case MISSIONAI_SEE_INVISIBLE_MAINTAIN:     return "MISSIONAI_SEE_INVISIBLE_MAINTAIN";
	case MISSIONAI_SEE_INVISIBLE_SEA_MAINTAIN: return "MISSIONAI_SEE_INVISIBLE_SEA_MAINTAIN";
	case MISSIONAI_WAIT_FOR_ESCORT:            return "MISSIONAI_WAIT_FOR_ESCORT";
	case MISSIONAI_WAIT_FOR_SEE_INVISIBLE:     return "MISSIONAI_WAIT_FOR_SEE_INVISIBLE";
	default:                                   return "UNKNOWN_MISSIONAI";
	}
}

CvString MissionAITypeToDescription(MissionAITypes eMissionAI)
{
	switch (eMissionAI)
	{
	case NO_MISSIONAI:                         return " --- ";
	case MISSIONAI_SHADOW:                     return "Shadow another unit";
	case MISSIONAI_GROUP:                      return "Join or regroup with a unit group";
	case MISSIONAI_LOAD_ASSAULT:               return "Load onto assault transport";
	case MISSIONAI_LOAD_SETTLER:               return "Load settler onto transport";
	case MISSIONAI_LOAD_SPECIAL:               return "Load special unit";
	case MISSIONAI_GUARD_CITY:                 return "Guard a city";
	case MISSIONAI_GUARD_BONUS:                return "Guard a resource";
	case MISSIONAI_GUARD_TRADE_NET:            return "Guard trade network";
	case MISSIONAI_GUARD_SPY:                  return "Guard against spies";
	case MISSIONAI_ATTACK_SPY:                 return "Hunt down enemy spies";
	case MISSIONAI_SPREAD:                     return "Spread religion";
	case MISSIONAI_SPREAD_CORPORATION:         return "Spread corporation";
	case MISSIONAI_CONSTRUCT:                  return "Construct building or wonder";
	case MISSIONAI_HURRY:                      return "Hurry production";
	case MISSIONAI_GREAT_WORK:                 return "Perform a great work";
	case MISSIONAI_EXPLORE:                    return "Explore territory";
	case MISSIONAI_BLOCKADE:                   return "Blockade coast or city";
	case MISSIONAI_PILLAGE:                    return "Pillage enemy improvements";
	case MISSIONAI_FOUND:                      return "Found a city";
	case MISSIONAI_BUILD:                      return "Build improvement";
	case MISSIONAI_ASSAULT:                    return "Lead an assault";
	case MISSIONAI_CARRIER:                    return "Operate as carrier";
	case MISSIONAI_PICKUP:                     return "Pick up unit(s)";
	case MISSIONAI_INQUISITION:                return "Conduct inquisition";
	case MISSIONAI_CLAIM_TERRITORY:            return "Claim new territory";
	case MISSIONAI_HURRY_FOOD:                 return "Hurry food supply";
	case MISSIONAI_CONTRACT:                   return "Fulfill contract";
	case MISSIONAI_CONTRACT_UNIT:              return "Provide contracted unit";
	case MISSIONAI_DELIBERATE_KILL:            return "Targeted killing";
	case MISSIONAI_REGROUP:                    return "Regroup with allies";
	case MISSIONAI_HEAL_SUPPORT:               return "Heal and support units";
	case MISSIONAI_PROPERTY_CONTROL_RESPONSE:  return "Move to Respond to property issues";
	case MISSIONAI_PROPERTY_CONTROL_MAINTAIN:  return "Maintain property control";
	case MISSIONAI_INVESTIGATOR_RESPONSE:      return "Respond to investigation";
	case MISSIONAI_INVESTIGATOR_MAINTAIN:      return "Maintain investigation role";
	case MISSIONAI_INFILTRATOR_MAINTAIN:       return "Maintain infiltration role";
	case MISSIONAI_SEE_INVISIBLE_MAINTAIN:     return "Maintain invisible detection";
	case MISSIONAI_SEE_INVISIBLE_SEA_MAINTAIN: return "Maintain sea invisible detection";
	case MISSIONAI_WAIT_FOR_ESCORT:            return "Wait for escort";
	case MISSIONAI_WAIT_FOR_SEE_INVISIBLE:     return "Wait for see-invisible escort";
	default:                                   return "Unknown mission AI";
	}
}

CvString AutomateTypeToDescription(AutomateTypes eAutomateAI)
{
	switch (eAutomateAI)
	{
	case NO_AUTOMATE:              return "---";
	case AUTOMATE_BUILD:           return "Construct improvements";
	case AUTOMATE_NETWORK:         return "Build and maintain networks";
	case AUTOMATE_CITY:            return "Manage city automation";
	case AUTOMATE_EXPLORE:         return "Explore the map";
	case AUTOMATE_RELIGION:        return "Spread religion";
	case AUTOMATE_PILLAGE:         return "Pillage enemy lands";
	case AUTOMATE_HUNT:            return "Hunt down enemies";
	case AUTOMATE_CITY_DEFENSE:    return "Defend the city";
	case AUTOMATE_BORDER_PATROL:   return "Patrol borders";
	case AUTOMATE_HURRY:           return "Hurry production";
	case AUTOMATE_PIRATE:          return "Raid and plunder at sea";
	case AUTOMATE_AIRSTRIKE:       return "Conduct air strikes";
	case AUTOMATE_AIRBOMB:         return "Bomb enemy targets";
	case AUTOMATE_AIR_RECON:       return "Perform air reconnaissance";
	case AUTOMATE_UPGRADING:       return "Upgrade unit";
	case AUTOMATE_CANCEL_UPGRADING:return "Cancel unit upgrade";
	case AUTOMATE_PROMOTIONS:      return "Apply promotions";
	case AUTOMATE_CANCEL_PROMOTIONS:return "Cancel promotions";
	case AUTOMATE_SHADOW:          return "Shadow enemy units";
	default:                       return "---";
	}
}

char remove_diacritic_char(unsigned char c) {
	switch (c) {
	case 'à': case 'á': case 'â': case 'ã': case 'ä': case 'å':
	case 'À': case 'Á': case 'Â': case 'Ã': case 'Ä': case 'Å':
		return 'a';
	case 'è': case 'é': case 'ê': case 'ë':
	case 'È': case 'É': case 'Ê': case 'Ë':
		return 'e';
	case 'ì': case 'í': case 'î': case 'ï':
	case 'Ì': case 'Í': case 'Î': case 'Ï':
		return 'i';
	case 'ò': case 'ó': case 'ô': case 'õ': case 'ö':
	case 'Ò': case 'Ó': case 'Ô': case 'Õ': case 'Ö':
		return 'o';
	case 'ù': case 'ú': case 'û': case 'ü':
	case 'Ù': case 'Ú': case 'Û': case 'Ü':
		return 'u';
	case 'ç': case 'Ç':
		return 'c';
	case 'ñ': case 'Ñ':
		return 'n';
	default:
		return c;
	}
}

CvString remove_diacritics(const CvString& input) {
	CvString output;
	for (CvString::const_iterator it = input.begin(); it != input.end(); ++it) {
		unsigned char c = *it;
		output += remove_diacritic_char(c);
	}
	return output;
}