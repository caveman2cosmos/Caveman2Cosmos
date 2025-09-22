// teamAI.cpp


#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvArea.h"
#include "CvBuildingInfo.h"
#include "CvBonusInfo.h"
#include "CvCity.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvMap.h"
#include "CvPlayerAI.h"
#include "CvPlot.h"
#include "CvPython.h"
#include "CvTeamAI.h"

// statics

CvTeamAI* CvTeamAI::m_aTeams = NULL;

void CvTeamAI::initStatics()
{
	PROFILE_EXTRA_FUNC();
	m_aTeams = new CvTeamAI[MAX_TEAMS];
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aTeams[iI].m_eID = ((TeamTypes)iI);
	}
}

void CvTeamAI::freeStatics()
{
	SAFE_DELETE_ARRAY(m_aTeams);
}

CvTeamAI& CvTeamAI::getTeam(TeamTypes eTeam)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);
	return m_aTeams[eTeam];
}


// Public Functions...

CvTeamAI::CvTeamAI()
{
	m_aiWarPlanStateCounter = new int[MAX_TEAMS];
	m_aiAtWarCounter = new int[MAX_TEAMS];
	m_aiAtPeaceCounter = new int[MAX_TEAMS];
	m_aiHasMetCounter = new int[MAX_TEAMS];
	m_aiOpenBordersCounter = new int[MAX_TEAMS];
	m_aiDefensivePactCounter = new int[MAX_TEAMS];
	m_aiShareWarCounter = new int[MAX_TEAMS];
	m_aiWarSuccess = new int[MAX_TEAMS];
	m_aiEnemyPeacetimeTradeValue = new int[MAX_TEAMS];
	m_aiEnemyPeacetimeGrantValue = new int[MAX_TEAMS];
	m_aeWarPlan = new WarPlanTypes[MAX_TEAMS];


	AI_reset(true);
}


CvTeamAI::~CvTeamAI()
{
	AI_uninit();

	SAFE_DELETE_ARRAY(m_aiWarPlanStateCounter);
	SAFE_DELETE_ARRAY(m_aiAtWarCounter);
	SAFE_DELETE_ARRAY(m_aiAtPeaceCounter);
	SAFE_DELETE_ARRAY(m_aiHasMetCounter);
	SAFE_DELETE_ARRAY(m_aiOpenBordersCounter);
	SAFE_DELETE_ARRAY(m_aiDefensivePactCounter);
	SAFE_DELETE_ARRAY(m_aiShareWarCounter);
	SAFE_DELETE_ARRAY(m_aiWarSuccess);
	SAFE_DELETE_ARRAY(m_aiEnemyPeacetimeTradeValue);
	SAFE_DELETE_ARRAY(m_aiEnemyPeacetimeGrantValue);
	SAFE_DELETE_ARRAY(m_aeWarPlan);
}


void CvTeamAI::AI_init()
{
	AI_reset(false);

	//--------------------------------
	// Init other game data
}

void CvTeamAI::AI_uninit() {}

void CvTeamAI::AI_reset(bool bConstructor)
{
	PROFILE_EXTRA_FUNC();
	AI_uninit();

	m_eWorstEnemy = NO_TEAM;
	m_tradeTechValuesCachedTurn = -1;

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiWarPlanStateCounter[iI] = 0;
		m_aiAtWarCounter[iI] = 0;
		m_aiAtPeaceCounter[iI] = 0;
		m_aiHasMetCounter[iI] = 0;
		m_aiOpenBordersCounter[iI] = 0;
		m_aiDefensivePactCounter[iI] = 0;
		m_aiShareWarCounter[iI] = 0;
		m_aiWarSuccess[iI] = 0;
		m_aiEnemyPeacetimeTradeValue[iI] = 0;
		m_aiEnemyPeacetimeGrantValue[iI] = 0;
		m_aeWarPlan[iI] = NO_WARPLAN;
		m_endWarValueCache[iI] = - 1;

		if (!bConstructor && getID() != NO_TEAM)
		{
			TeamTypes eLoopTeam = (TeamTypes) iI;
			CvTeamAI& kLoopTeam = GET_TEAM(eLoopTeam);
			kLoopTeam.m_aiWarPlanStateCounter[getID()] = 0;
			kLoopTeam.m_aiAtWarCounter[getID()] = 0;
			kLoopTeam.m_aiAtPeaceCounter[getID()] = 0;
			kLoopTeam.m_aiHasMetCounter[getID()] = 0;
			kLoopTeam.m_aiOpenBordersCounter[getID()] = 0;
			kLoopTeam.m_aiDefensivePactCounter[getID()] = 0;
			kLoopTeam.m_aiShareWarCounter[getID()] = 0;
			kLoopTeam.m_aiWarSuccess[getID()] = 0;
			kLoopTeam.m_aiEnemyPeacetimeTradeValue[getID()] = 0;
			kLoopTeam.m_aiEnemyPeacetimeGrantValue[getID()] = 0;
			kLoopTeam.m_aeWarPlan[getID()] = NO_WARPLAN;
		}
	}

	m_iNoTechTradeThreshold = 0;
	m_iTechTradeKnownPercent = 0;
	m_iMaxWarRand = 0;
	m_iMaxWarNearbyPowerRatio = 0;
	m_iMaxWarDistantPowerRatio = 0;
	m_iMaxWarMinAdjacentLandPercent = 0;
	m_iLimitedWarRand = 0;
	m_iLimitedWarPowerRatio = 0;
	m_iDogpileWarRand = 0;
	m_iMakePeaceRand = 0;
#ifdef	ENABLE_FOGWAR_DECAY
	m_bPermanentMapLand = false;
	m_bPermanentMapSea = false;
	m_iDefaultDecay = DEFAULT_DECAY;
#endif
}


void CvTeamAI::AI_doTurnPre()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		m_endWarValueCache[iI] = -1;
	}
	AI_doCounter();
}


void CvTeamAI::AI_doTurnPost()
{
	AI_updateWorstEnemy();
	AI_updateAreaStragies(false);

	if (isHuman() || isNPC() || isMinorCiv())
	{
		return;
	}
	AI_doWar();
}


void CvTeamAI::AI_makeAssignWorkDirty()
{
	PROFILE_EXTRA_FUNC();
	if (isNPC())
	{
		FAssertMsg(GET_PLAYER(getLeaderID()).isAlive(), "Should be alive");
		// NPC is always a team of one.
		GET_PLAYER(getLeaderID()).AI_makeAssignWorkDirty();
		return;
	}
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			GET_PLAYER((PlayerTypes)iI).AI_makeAssignWorkDirty();
		}
	}
}


// Find plot strength of teammates and potentially vassals
int CvTeamAI::AI_getOurPlotStrength(const CvPlot* pPlot, const int iRange, const bool bDefensiveBonuses, const bool bTestMoves, const bool bIncludeVassals) const
{
	PROFILE_EXTRA_FUNC();
	if (isNPC())
	{
		FAssertMsg(GET_PLAYER(getLeaderID()).isAlive(), "Should be alive");
		// NPC is always a team of one.
		return GET_PLAYER(getLeaderID()).AI_getOurPlotStrength(pPlot, iRange, bDefensiveBonuses, bTestMoves);
	}
	int iPlotStrength = 0;

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		const CvPlayerAI& player = GET_PLAYER((PlayerTypes)iI);

		if (player.isAlive()
		&& (player.getTeam() == getID() || bIncludeVassals && GET_TEAM(player.getTeam()).isVassal(getID())))
		{
			iPlotStrength += player.AI_getOurPlotStrength(pPlot, iRange, bDefensiveBonuses, bTestMoves);
		}
	}
	return iPlotStrength;
}



void CvTeamAI::AI_updateAreaStragies(const bool bTargets)
{
	PROFILE_EXTRA_FUNC();
	if (!GC.getGame().isFinalInitialized())
	{
		return;
	}

	foreach_(CvArea* pLoopArea, GC.getMap().areas())
	{
		pLoopArea->setAreaAIType(getID(), AI_calculateAreaAIType(pLoopArea));
	}

	if (bTargets)
	{
		AI_updateAreaTargets();
	}

	if (!isNPC())
	{
		AI_updateDiploCache();
	}
}


void CvTeamAI::AI_updateAreaTargets()
{
	PROFILE_EXTRA_FUNC();
	if (isNPC())
	{
		FAssertMsg(GET_PLAYER(getLeaderID()).isAlive(), "Should be alive");
		// NPC is always a team of one.
		GET_PLAYER(getLeaderID()).AI_updateAreaTargets();
		return;
	}
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			GET_PLAYER((PlayerTypes)iI).AI_updateAreaTargets();
		}
	}
}


int CvTeamAI::AI_countMilitaryWeight(const CvArea* pArea) const
{
	PROFILE_EXTRA_FUNC();
	if (isNPC())
	{
		FAssertMsg(GET_PLAYER(getLeaderID()).isAlive(), "Should be alive");
		// NPC is always a team of one.
		return GET_PLAYER(getLeaderID()).AI_militaryWeight(pArea);
	}
	int iCount = 0;

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iCount += GET_PLAYER((PlayerTypes)iI).AI_militaryWeight(pArea);
		}
	}
	return iCount;
}


bool CvTeamAI::AI_isAnyCapitalAreaAlone() const
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(!isNPC(), "NPC doesn't have a capital area!");

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID())
		&&  GET_PLAYER((PlayerTypes)iI).AI_isCapitalAreaAlone())
		{
			return true;
		}
	}
	return false;
}


bool CvTeamAI::AI_isPrimaryArea(const CvArea* pArea) const
{
	PROFILE_EXTRA_FUNC();
	if (isNPC())
	{
		FAssertMsg(GET_PLAYER(getLeaderID()).isAlive(), "Should be alive");
		// NPC is always a team of one.
		return GET_PLAYER(getLeaderID()).AI_isPrimaryArea(pArea);
	}

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID())
		&&  GET_PLAYER((PlayerTypes)iI).AI_isPrimaryArea(pArea))
		{
			return true;
		}
	}
	return false;
}


bool CvTeamAI::AI_hasCitiesInPrimaryArea(const TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	foreach_(const CvArea* pLoopArea, GC.getMap().areas())
	{
		if (AI_isPrimaryArea(pLoopArea) && GET_TEAM(eTeam).countNumCitiesByArea(pLoopArea))
		{
			return true;
		}
	}
	return false;
}


AreaAITypes CvTeamAI::AI_calculateAreaAIType(const CvArea* pArea, const bool bPreparingTotal) const
{
	PROFILE_FUNC();

	if (pArea->isWater())
	{
		return AREAAI_NEUTRAL;
	}

	if (isNPC())
	{
		if (pArea->getNumCities() - pArea->getCitiesPerPlayer(BARBARIAN_PLAYER) == 0)
		{
			return AREAAI_ASSAULT; // Toffer - Basically means that it should naval invade
		}
		if (  countNumAIUnitsByArea(pArea, UNITAI_ATTACK)
			+ countNumAIUnitsByArea(pArea, UNITAI_ATTACK_CITY)
			+ countNumAIUnitsByArea(pArea, UNITAI_PILLAGE)
			>
			2 * AI_countMilitaryWeight(pArea))
		{
			return AREAAI_OFFENSIVE;
		}
		return AREAAI_MASSING;
	}

	bool bRecentAttack = false;
	bool bTargets = false;
	bool bChosenTargets = false;
	bool bDeclaredTargets = false;
	bool bAssault = false;
	bool bPreparingAssault = false;

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			// Minor civs will declare war on all, but won't get has met set so that they don't show in score board
			if (AI_getWarPlan((TeamTypes)iI) != NO_WARPLAN && isHasMet((TeamTypes)iI))
			{
				FAssert(((TeamTypes)iI) != getID());
				FAssert(isHasMet((TeamTypes)iI));

				if (AI_getWarPlan((TeamTypes)iI) == WARPLAN_ATTACKED_RECENT)
				{
					FAssert(isAtWar((TeamTypes)iI));
					bRecentAttack = true;
				}

				if (GET_TEAM((TeamTypes)iI).countNumCitiesByArea(pArea) > 0 || GET_TEAM((TeamTypes)iI).countNumUnitsByArea(pArea) > 4)
				{
					bTargets = true;

					if (AI_isChosenWar((TeamTypes)iI))
					{
						bChosenTargets = true;

						if (isAtWar((TeamTypes)iI) ? AI_getAtWarCounter((TeamTypes)iI) < 10 : AI_isSneakAttackReady((TeamTypes)iI))
						{
							bDeclaredTargets = true;
						}
					}
				}
				else
				{
					bAssault = true;
					if (AI_isSneakAttackPreparing((TeamTypes)iI))
					{
						bPreparingAssault = true;
					}
				}
			}
		}
	}
	const int iAreaCities = countNumCitiesByArea(pArea);

	if (bTargets && iAreaCities > 0 && isAtWar())
	{
		if (AI_isPrimaryArea(pArea) && AI_getWarSuccessCapitulationRatio() < -50)
		{
			// Use defensive stance if enemy has strong presence, we've been losing badly
			if (countPowerByArea(pArea) < countEnemyPowerByArea(pArea))
			{
				return AREAAI_DEFENSIVE;
			}
		}
		// Use defensive area AI right after invasions, to force vulnerable captured cities to be better defended
		else if (2*countPowerByArea(pArea) < countEnemyPowerByArea(pArea))
		{
			return AREAAI_DEFENSIVE;
		}
	}

	if (bDeclaredTargets)
	{
		return AREAAI_OFFENSIVE;
	}

	if (bTargets)
	{
		// AI_countMilitaryWeight is based on this team's pop and cities;
		// if this team is the biggest, it will over estimate needed units.
		int iMilitaryWeight = AI_countMilitaryWeight(pArea);
		int iCount = 1;

		for (int iJ = 0; iJ < MAX_PC_TEAMS; iJ++)
		{
			if (GET_TEAM((TeamTypes)iJ).isAlive() && iJ != getID()
			&& !GET_TEAM((TeamTypes)iJ).isMinorCiv()
			&& AI_getWarPlan((TeamTypes)iJ) != NO_WARPLAN)
			{
				iMilitaryWeight += GET_TEAM((TeamTypes)iJ).AI_countMilitaryWeight(pArea);
				iCount++;

				if (GET_TEAM((TeamTypes)iJ).isAVassal())
				{
					for (int iK = 0; iK < MAX_PC_TEAMS; iK++)
					{
						if (GET_TEAM((TeamTypes)iK).isAlive() && iK != getID()
						&& GET_TEAM((TeamTypes)iJ).isVassal((TeamTypes)iK))
						{
							iMilitaryWeight += GET_TEAM((TeamTypes)iK).AI_countMilitaryWeight(pArea);
						}
					}
				}
			}
		}

		iMilitaryWeight /= iCount;
		if (
		(
			countNumAIUnitsByArea(pArea, UNITAI_ATTACK) +
			countNumAIUnitsByArea(pArea, UNITAI_ATTACK_CITY) +
			countNumAIUnitsByArea(pArea, UNITAI_PILLAGE) +
			countNumAIUnitsByArea(pArea, UNITAI_ATTACK_AIR)
		)
		> 1 + iMilitaryWeight * (bPreparingTotal ? 25 : 20) / 100)
		{
			return AREAAI_OFFENSIVE;
		}
	}

	if (bTargets)
	{
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID())
			&&
				(
					GET_PLAYER((PlayerTypes)iI).AI_isDoStrategy(AI_STRATEGY_DAGGER)
					||
					GET_PLAYER((PlayerTypes)iI).AI_isDoStrategy(AI_STRATEGY_FINAL_WAR)
				)
			&& pArea->getCitiesPerPlayer((PlayerTypes)iI) > 0)
			{
				return AREAAI_MASSING;
			}
		}
		if (bRecentAttack)
		{
			if (countPowerByArea(pArea) > countEnemyPowerByArea(pArea))
			{
				return AREAAI_MASSING;
			}
			return AREAAI_DEFENSIVE;
		}
	}

	if (iAreaCities > 0 && countEnemyDangerByArea(pArea) > iAreaCities)
	{
		return AREAAI_DEFENSIVE;
	}

	if (bChosenTargets)
	{
		return AREAAI_MASSING;
	}

	if (bTargets)
	{
		if (iAreaCities > (getNumMembers() * 3))
		{
			if (GC.getGame().isOption(GAMEOPTION_AI_AGGRESSIVE) || (countPowerByArea(pArea) > ((countEnemyPowerByArea(pArea) * 3) / 2)))
			{
				return AREAAI_MASSING;
			}
		}
		return AREAAI_DEFENSIVE;
	}
	else if (bAssault)
	{
		if (AI_isPrimaryArea(pArea))
		{
			if (bPreparingAssault)
			{
				return AREAAI_ASSAULT_MASSING;
			}
		}
		else if (countNumCitiesByArea(pArea) > 0)
		{
			return AREAAI_ASSAULT_ASSIST;
		}
		return AREAAI_ASSAULT;
	}

	return AREAAI_NEUTRAL;
}

int CvTeamAI::AI_calculateAdjacentLandPlots(const TeamTypes eTeam) const
{
	PROFILE_FUNC();
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	const CvMap& map = GC.getMap();
	int iCount = 0;

	// Toffer - looping backwards like this is more performance friendly,
	//	as map.numPlots() is then only called once in the loop setup stage,
	//	instead of being called before each iteration in the iteration if statement stage.
	for (int iI = map.numPlots()-1; iI > -1; iI--)
	{
		const CvPlot* plot = map.plotByIndex(iI);

		if (!plot->isWater() && plot->getTeam() == eTeam && plot->isAdjacentTeam(getID(), true))
		{
			iCount++;
		}
	}
	return iCount;
}


int CvTeamAI::AI_calculatePlotWarValue(const TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	FAssert(eTeam != getID());

	const CvMap& map = GC.getMap();
	int iValue = 0;

	for (int iI = map.numPlots()-1; iI > -1; iI--)
	{
		const CvPlot* plot = map.plotByIndex(iI);

		if (plot->getTeam() == eTeam)
		{
			if (!plot->isWater() && plot->isAdjacentTeam(getID(), true))
			{
				iValue += 4;
			}

			const BonusTypes eBonus = plot->getBonusType(getID());
			if (NO_BONUS != eBonus)
			{
				iValue += 40 * GC.getBonusInfo(eBonus).getAIObjective();
			}
		}
	}
	return iValue;
}

// How much do we desire the land of another team based on bonuses available there
int CvTeamAI::AI_calculateBonusWarValue(TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	FAssert(eTeam != getID());

	const CvMap& map = GC.getMap();
	int iValue = 0;

	for (int iI = map.numPlots()-1; iI > -1; iI--)
	{
		const CvPlot* plot = map.plotByIndex(iI);

		if (plot->getTeam() == eTeam)
		{
			const BonusTypes eNonObsoleteBonus = plot->getNonObsoleteBonusType(getID());
			if (NO_BONUS != eNonObsoleteBonus)
			{
				int iThisValue = 0;
				for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAliveAndTeam(getID()))
					{
						// 10 seems like a typical value for a health/happiness resource the AI doesn't have
						// Values for strategic resources can be 60 or higher
						iThisValue += GET_PLAYER((PlayerTypes)iJ).AI_bonusVal(eNonObsoleteBonus);
					}
				}
				iThisValue /= getAliveCount();

				if (!plot->isWater())
				{
					if (!plot->isAdjacentTeam(getID(), true))
					{
						CvCity* pWorkingCity = plot->getWorkingCity();
						if (pWorkingCity != NULL)
						{
							for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
							{
								if (GET_PLAYER((PlayerTypes)iJ).isAliveAndTeam(getID())
								&& pWorkingCity->AI_playerCloseness((PlayerTypes)iJ) > 0)
								{
									iThisValue *= 2;
									break;
								}
							}
						}
					}
					else iThisValue *= 3;
				}
				iValue += std::max(0, iThisValue - 4) / 5;
			}
		}
	}
	return iValue;
}

// Higher value means relatively closer proximity, value is relative to third party civs.
// If eOtherTeam is our closest neighbour, then this will return GC.getMap().maxPlotDistance().
int CvTeamAI::AI_calculateCapitalProximity(TeamTypes eOtherTeam) const
{
	PROFILE_EXTRA_FUNC();
	const TeamTypes eTeam = getID();
	FAssertMsg(eTeam != eOtherTeam, "shouldn't call this function on ourselves");
	FAssertMsg(!isNPC(), "NPC has no capital!");

	int iTotalDistance = 0;
	int iCount = 0;

	int iMinDistance = MAX_INT;
	int iMaxDistance = 0;

	for (int iPlayerX = 0; iPlayerX < MAX_PC_PLAYERS; iPlayerX++)
	{
		const CvPlayer& playerX = GET_PLAYER((PlayerTypes)iPlayerX);

		if (playerX.isAliveAndTeam(eTeam))
		{
			const CvCity* pOurCapitalCity = playerX.getCapitalCity();

			if (pOurCapitalCity != NULL)
			{
				for (int iPlayerY = 0; iPlayerY < MAX_PC_PLAYERS; iPlayerY++)
				{
					const CvPlayer& playerY = GET_PLAYER((PlayerTypes)iPlayerY);

					if (playerY.isAliveAndTeam(eTeam, false))
					{
						const CvCity* pTheirCapitalCity = playerY.getCapitalCity();

						if (pTheirCapitalCity != NULL)
						{
							const int iDistance =
							(
								plotDistance(
									pOurCapitalCity->getX(), pOurCapitalCity->getY(),
									pTheirCapitalCity->getX(), pTheirCapitalCity->getY()
								)
								* (pOurCapitalCity->area() != pTheirCapitalCity->area() ? 3 : 2)
							);
							if (playerY.getTeam() == eOtherTeam)
							{
								iTotalDistance += iDistance;
								iCount++;
							}
							iMinDistance = std::min(iDistance, iMinDistance);
							iMaxDistance = std::max(iDistance, iMaxDistance);
						}
					}
				}
			}
		}
	}

	if (iCount > 0)
	{
		FAssert(iMaxDistance > 0);
		return GC.getMap().maxPlotDistance() * (iMaxDistance - (iTotalDistance / iCount - iMinDistance)) / iMaxDistance;
	}
	return 0;
}


bool CvTeamAI::AI_isLandTarget(TeamTypes eTeam, bool bNeighborsOnly) const
{
	PROFILE_EXTRA_FUNC();
	if (!AI_hasCitiesInPrimaryArea(eTeam))
	{
		return false;
	}

	// If shared capital area is largely unclaimed, then we can reach over land
	int iMod = 100;

	if (!bNeighborsOnly)
	{
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			const CvPlayer& player = GET_PLAYER((PlayerTypes)iI);

			if (player.getTeam() == getID() && player.getNumCities() > 0)
			{
				const CvCity* pCapital = player.getCapitalCity();

				if (pCapital != NULL && GET_TEAM(eTeam).AI_isPrimaryArea(pCapital->area()))
				{
					iMod *= pCapital->area()->getNumOwnedTiles();
					iMod /= pCapital->area()->getNumTiles();
				}
			}
		}
	}
	return AI_calculateAdjacentLandPlots(eTeam) >= range(8 * (iMod - 40) / 40, 0, 8);
}

// this determines if eTeam or any of its allies are land targets of us
bool CvTeamAI::AI_isAllyLandTarget(TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	const TeamTypes eMyTeam = getID();

	FAssertMsg(eTeam != eMyTeam, "shouldn't call this function on ourselves");

	const CvTeam& team = GET_TEAM(eTeam);

	for (int iTeamX = 0; iTeamX < MAX_PC_TEAMS; iTeamX++)
	{
		const TeamTypes eTeamX = (TeamTypes)iTeamX;
		if (eTeamX != eMyTeam
		&&
		(
			eTeamX == eTeam
			|| team.isDefensivePact(eTeamX)
			|| team.isVassal(eTeamX)
			|| GET_TEAM(eTeamX).isVassal(eTeam)
		)
		&& AI_isLandTarget(eTeamX))
		{
			return true;
		}
	}
	return false;
}


bool CvTeamAI::AI_shareWar(TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	// No dealing with minor civs
	if (isMinorCiv())
	{
		return false;
	}
	const CvTeam& team = GET_TEAM(eTeam);

	if (team.isMinorCiv())
	{
		return false;
	}

	// Only accumulate if someone actually declared war, not a left over from StartAsMinors
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (iI != getID() && iI != eTeam && isAtWar((TeamTypes)iI) && team.isAtWar((TeamTypes)iI))
		{
			const CvTeam& teamX = GET_TEAM((TeamTypes)iI);

			if (teamX.isAlive() && !teamX.isMinorCiv()
			&& (
				AI_getWarPlan((TeamTypes)iI) != WARPLAN_LIMITED
				|| team.AI_getWarPlan((TeamTypes)iI) != WARPLAN_LIMITED
				|| teamX.AI_getWarPlan(getID()) != WARPLAN_LIMITED
				|| teamX.AI_getWarPlan(eTeam) != WARPLAN_LIMITED))
			{
				return true;
			}
		}
	}
	return false;
}


AttitudeTypes CvTeamAI::AI_getAttitude(TeamTypes eTeam, bool bForced) const
{
	return CvPlayerAI::AI_getAttitudeFromValue(AI_getAttitudeVal(eTeam, bForced));
}


int CvTeamAI::AI_getAttitudeVal(TeamTypes eTeam, bool bForced) const
{
	PROFILE_EXTRA_FUNC();
	const CvTeam& team = GET_TEAM(eTeam);

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(team.isAlive(), "Waste of time to call this function without checking this condition first.");
	FAssertMsg(isAlive(), "Waste of time to call this function without checking this condition first.");
	FAssertMsg(isHasMet(eTeam), "Waste of time to call this function without checking this condition first.");
	FAssertMsg(!team.isNPC(), "Attitude towards NPC is meaningless!");
	FAssertMsg(!isNPC(), "NPC doesn't have attitude!");

	int iAttitude = 0;
	int iCount = 0;

	for (int iA = 0; iA < MAX_PC_PLAYERS; iA++)
	{
		const CvPlayerAI& playerA = GET_PLAYER((PlayerTypes)iA);

		if (playerA.isAliveAndTeam(getID()))
		{
			for (int iB = 0; iB < MAX_PC_PLAYERS; iB++)
			{
				if (GET_PLAYER((PlayerTypes)iB).isAlive() && iA != iB)
				{
					// BBAI TODO: Attitude averaging ... what to do?
					const TeamTypes eTeamB = GET_PLAYER((PlayerTypes)iB).getTeam();

					if (eTeamB == eTeam || GET_TEAM(eTeamB).isVassal(eTeam) || team.isVassal(eTeamB))
					{
						iAttitude += playerA.AI_getAttitudeVal((PlayerTypes)iB, bForced);
						iCount++;
					}
				}
			}
		}
	}
	if (iCount > 0)
	{
		return iAttitude / iCount;
	}
	return 0;
}


int CvTeamAI::AI_getMemoryCount(TeamTypes eTeam, MemoryTypes eMemory) const
{
	PROFILE_FUNC();
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	int iMemoryCount = 0;
	int iCount = 0;

	for (int iA = 0; iA < MAX_PC_PLAYERS; iA++)
	{
		const CvPlayerAI& playerA = GET_PLAYER((PlayerTypes)iA);

		if (playerA.isAliveAndTeam(getID()))
		{
			for (int iB = 0; iB < MAX_PC_PLAYERS; iB++)
			{
				if (GET_PLAYER((PlayerTypes)iB).isAliveAndTeam(eTeam))
				{
					iMemoryCount += playerA.AI_getMemoryCount((PlayerTypes)iB, eMemory);
					iCount++;
				}
			}
		}
	}
	if (iCount > 0)
	{
		return iMemoryCount / iCount;
	}
	return 0;
}


int CvTeamAI::AI_chooseElection(const VoteSelectionData& kVoteSelectionData) const
{
	PROFILE_EXTRA_FUNC();
	const VoteSourceTypes eVoteSource = kVoteSelectionData.eVoteSource;

	FAssert(!isHuman());
	FAssert(GC.getGame().getSecretaryGeneral(eVoteSource) == getID());

	int iBestVote = -1;
	int iBestValue = 0;

	for (int iI = 0; iI < (int)kVoteSelectionData.aVoteOptions.size(); iI++)
	{
		const VoteTypes eVote = kVoteSelectionData.aVoteOptions[iI].eVote;

		FAssert(GC.getVoteInfo(eVote).isVoteSourceType(eVoteSource));
		FAssert(GC.getGame().isChooseElection(eVote));

		bool bValid = true;

		if (!GC.getGame().isTeamVote(eVote))
		{
			for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
			{
				if (GET_PLAYER((PlayerTypes)iJ).isAliveAndTeam(getID()))
				{
					const PlayerVoteTypes eVote = GET_PLAYER((PlayerTypes)iJ).AI_diploVote(kVoteSelectionData.aVoteOptions[iI], eVoteSource, true);

					if (eVote != PLAYER_VOTE_YES || eVote == GC.getGame().getVoteOutcome((VoteTypes)iI))
					{
						bValid = false;
						break;
					}
				}
			}
		}
		if (bValid)
		{
			const int iValue = (1 + GC.getGame().getSorenRandNum(10000, "AI Choose Vote"));

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				iBestVote = iI;
			}
		}
	}
	return iBestVote;
}

/// \brief Relative value of starting a war against eTeam.
///
/// This function computes the value of starting a war against eTeam.
/// The returned value should be compared against other possible targets
/// to pick the best target.
int CvTeamAI::AI_startWarVal(TeamTypes eTeam) const
{
	PROFILE_FUNC();

	int iValue = AI_calculatePlotWarValue(eTeam);

	iValue += AI_calculateCapitalProximity(eTeam) * 3 / (iValue > 0 ? 2 : 3);

	const int iClosenessValue = AI_teamCloseness(eTeam);

	if (iClosenessValue <= 0)
	{
		iValue /= 2;
	}
	iValue += iClosenessValue;

	iValue += AI_calculateBonusWarValue(eTeam);

	const CvTeamAI& team = GET_TEAM(eTeam);

	// Target other teams close to victory
	if (team.AI_isAnyMemberDoVictoryStrategyLevel3())
	{
		iValue += 10;

		const bool bAggressive = GC.getGame().isOption(GAMEOPTION_AI_AGGRESSIVE);
		const bool bConq4 = AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CONQUEST4);

		// Prioritize targets closer to victory
		if (bConq4 || bAggressive)
		{
			iValue *= 3;
			iValue /= 2;
		}

		if (team.AI_isAnyMemberDoVictoryStrategyLevel4())
		{
			if (team.AI_getLowestVictoryCountdown() >= 0)
			{
				iValue += 50;
			}

			iValue *= 2;

			if (bConq4 || bAggressive)
			{
				iValue *= 4;
			}
			else if (AI_isAnyMemberDoVictoryStrategyLevel3())
			{
				iValue *= 2;
			}
		}
	}

	// This adapted legacy code just makes us more willing to enter a war in a trade deal
	// as boost applies to all rivals
	if (AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_DOMINATION3))
	{
		iValue *= (GC.getGame().isOption(GAMEOPTION_AI_AGGRESSIVE) ? 3 : 2);
	}

	// If occupied or conquest inclined and early/not strong, value weak opponents
	if (hasWarPlan(true)
	||  AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CONQUEST2)
	&& !AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CONQUEST3))
	{
		const int iMultiplier = 75 * getPower(false)/std::max(1, team.getDefensivePower(getID())); //k-mod

		iValue *= range(iMultiplier, 50, 400);
		iValue /= 100;
	}

	switch (AI_getAttitude(eTeam))
	{
	case ATTITUDE_FURIOUS:
		iValue *= 16;
		break;
	case ATTITUDE_ANNOYED:
		iValue *= 8;
		break;
	case ATTITUDE_CAUTIOUS:
		iValue *= 4;
		break;
	case ATTITUDE_PLEASED:
		iValue *= 2;
		break;
	case ATTITUDE_FRIENDLY:
		iValue *= 1;
		break;
	default:
		FErrorMsg("error");
		break;
	}

	// Make it harder to bribe player to start a war
	if (AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CULTURE4))
	{
		iValue /= 8;
	}
	else if (AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_SPACE4))
	{
		iValue /= 4;
	}
	else if (AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CULTURE3))
	{
		iValue /= 3;
	}
	return iValue;
}


// XXX this should consider area power...
int CvTeamAI::AI_endWarVal(TeamTypes eTeam) const
{
	PROFILE_FUNC();

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(isAtWar(eTeam), "Current AI Team instance is expected to be at war with eTeam");

	// Just evaluate once per turn and cache for the rest of that turn
	if (m_endWarValueCache[eTeam] != -1)
	{
		return m_endWarValueCache[eTeam];
	}

	int iValue =
	(
		100 + 3 * (getNumCities() + GET_TEAM(eTeam).getNumCities())
		+
		getTotalPopulation() + GET_TEAM(eTeam).getTotalPopulation()
		+
		GET_TEAM(eTeam).AI_getWarSuccess(getID()) * 20
	);

	const int iOurPower = std::max(1, getPower(true));
	const int iTheirPower = std::max(1, GET_TEAM(eTeam).getDefensivePower());

	iValue *= iTheirPower + 10;
	iValue /= std::max(1, iOurPower + iTheirPower + 10);

	const WarPlanTypes eWarPlan = AI_getWarPlan(eTeam);

	// if we are not human, do we want to continue war for strategic reasons?
	// only check if our power is at least 120% of theirs
	if (!isHuman() && iOurPower > ((120 * iTheirPower) / 100))
	{
		bool bDagger = false;
		bool bAnyFinancialTrouble = false;

		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
			{
				if (!bDagger && GET_PLAYER((PlayerTypes)iI).AI_isDoStrategy(AI_STRATEGY_DAGGER))
				{
					bDagger = true;
					if (bAnyFinancialTrouble) break;
				}

				if (!bAnyFinancialTrouble && GET_PLAYER((PlayerTypes)iI).AI_isFinancialTrouble())
				{
					bAnyFinancialTrouble = true;
					if (bDagger) break;
				}
			}
		}

		// if dagger, value peace at 90% power ratio
		if (bDagger)
		{
			iValue *= 9 * iTheirPower;
			iValue /= 10 * iOurPower;
		}

		// for now, we will always do the land mass check for domination
		// if we have more than half the land, then value peace at 90% * land ratio
		const int iLandRatio = getTotalLand(true) * 100 / std::max(1, GET_TEAM(eTeam).getTotalLand(true));
		if (iLandRatio > 120)
		{
			iValue *= 90;
			iValue /= iLandRatio;
		}

		// if in financial trouble, warmongers will continue the fight to make more money
		if (bAnyFinancialTrouble)
		{
			switch (eWarPlan)
			{
				case WARPLAN_TOTAL:
				{
					// if we total warmonger, value peace at 70% * power ratio factor
					if (bDagger || AI_maxWarRand() < 100)
					{
						iValue *= 35 * iTheirPower;
						iValue /= 10 * (iOurPower + 4 * iTheirPower);
					}
					break;
				}
				case WARPLAN_LIMITED:
				{
					// if we limited warmonger, value peace at 70% * power ratio factor
					if (AI_limitedWarRand() < 100)
					{
						iValue *= 35 * iTheirPower;
						iValue /= 10 * (iOurPower + 4 * iTheirPower);
					}
					break;
				}
				case WARPLAN_DOGPILE:
				{
					// if we dogpile warmonger, value peace at 70% * power ratio factor
					if (AI_dogpileWarRand() < 100)
					{
						iValue *= 35 * iTheirPower;
						iValue /= 10 * (iOurPower + 4 * iTheirPower);
					}
					break;
				}
			}
		}
	}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      05/19/10                                jdog5000      */
/*                                                                                              */
/* War strategy AI, Victory Strategy AI                                                         */
/************************************************************************************************/
	if (AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CULTURE4))
	{
		iValue *= 4;
	}
	else if (AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CULTURE3) || AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_SPACE4))
	{
		iValue *= 2;
	}

	if (!isHuman() && eWarPlan == WARPLAN_TOTAL
	|| !GET_TEAM(eTeam).isHuman() && GET_TEAM(eTeam).AI_getWarPlan(getID()) == WARPLAN_TOTAL)
	{
		iValue *= 2;
	}
	else if (!isHuman() && eWarPlan == WARPLAN_DOGPILE && GET_TEAM(eTeam).getAtWarCount(true) > 1
	|| !GET_TEAM(eTeam).isHuman() && GET_TEAM(eTeam).AI_getWarPlan(getID()) == WARPLAN_DOGPILE && getAtWarCount(true) > 1)
	{
		iValue *= 3;
		iValue /= 2;
	}

	// Do we have a big stack en route?
	int iOurAttackers = 0;
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		if (GET_PLAYER((PlayerTypes)iPlayer).getTeam() == getID())
		{
			iOurAttackers += GET_PLAYER((PlayerTypes)iPlayer).AI_enemyTargetMissions(eTeam);
		}
	}
	int iTheirAttackers = 0;
	foreach_(const CvArea* pLoopArea, GC.getMap().areas())
	{
		iTheirAttackers += countEnemyDangerByArea(pLoopArea, eTeam);
	}

	const int iAttackerRatio = 100 * iOurAttackers / std::max(1 + GC.getGame().getCurrentEra(), iTheirAttackers);

	if (GC.getGame().isOption(GAMEOPTION_AI_AGGRESSIVE))
	{
		iValue *= 150;
		iValue /= range(iAttackerRatio, 150, 900);
	}
	else
	{
		iValue *= 200;
		iValue /= range(iAttackerRatio, 200, 600);
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	iValue -= (iValue % GC.getDIPLOMACY_VALUE_REMAINDER());

	if (isHuman())
	{
		iValue = std::max(iValue, GC.getDIPLOMACY_VALUE_REMAINDER());
	}

	m_endWarValueCache[eTeam] = iValue;

	return iValue;
}


bool CvTeamAI::AI_minorPeaceOut(TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();

	if (!AI_hasCitiesInPrimaryArea(eTeam) || AI_teamCloseness(eTeam) <= 0 || AI_getWarSuccess(eTeam) < GC.getWAR_SUCCESS_CITY_CAPTURING())
	{
		return true;
	}

	for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
	{
		if (GET_PLAYER((PlayerTypes)iJ).getTeam() == getID())
		{
			if (GET_PLAYER((PlayerTypes)iJ).AI_isFinancialTrouble())
			{
				return true;
			}
		}
	}
	return false;
}

int CvTeamAI::AI_getBarbarianCivWarVal(TeamTypes eTeam, int iMaxDistance) const
{
	if (AI_getAttitude(eTeam) == ATTITUDE_FRIENDLY || !AI_hasCitiesInPrimaryArea(eTeam))
	{
		return 0;
	}
	const int iClosenessValue = AI_teamCloseness(eTeam, iMaxDistance);
	if (iClosenessValue <= 0)
	{
		return 0;
	}
	int iValue = AI_teamCloseness(eTeam, 16);
	if (iValue > 0)
	{
		iValue += (
			  AI_calculatePlotWarValue(eTeam) / 2
			+ AI_calculateCapitalProximity(eTeam) * 3/2
			+ AI_getWarSuccess(eTeam) * 3
		);
		switch (AI_getAttitude(eTeam))
		{
			case ATTITUDE_FURIOUS: iValue *= 4; break;
			case ATTITUDE_ANNOYED: iValue *= 3; break;
			case ATTITUDE_CAUTIOUS: iValue *= 2;
		}
		iValue *= getPower(true) * AI_maxWarNearbyPowerRatio() / 100;
		iValue /= GET_TEAM(eTeam).getDefensivePower();
	}
	return iValue;
}


int CvTeamAI::AI_techTradeVal(TechTypes eTech, TeamTypes eTeam)
{
	PROFILE_FUNC();
	FAssert(eTeam != getID());
	FAssertMsg(!isHasTech(eTech), "Buying a tech one already own has no value");

	if (m_tradeTechValuesCachedTurn != GC.getGame().getGameTurn())
	{
		m_tradeTechValueCache.clear();
		m_tradeTechValuesCachedTurn = GC.getGame().getGameTurn();
	}
	const int iCacheIndex = eTech * MAX_TEAMS + getID();

	stdext::hash_map<int,int>::const_iterator itr = m_tradeTechValueCache.find(iCacheIndex);
	if (itr != m_tradeTechValueCache.end())
	{
		return itr->second;
	}
	{
		PROFILE("CvTeamAI::AI_techTradeVal.CacheMiss");

		if (gPlayerLogLevel > 3)
		{
			logBBAI(
				"Calculate trade value for tech %S by team %d for team %d",
				GC.getTechInfo(eTech).getDescription(), (int)eTeam, getID()
			);
			logBBAI("Currently have cached values for:");
			for( itr = m_tradeTechValueCache.begin(); itr != m_tradeTechValueCache.end(); ++itr)
			{
				int iTech = itr->first/MAX_TEAMS;
				int iTeam = itr->first%MAX_TEAMS;

				logBBAI("\t%d (%S) for team %d", iTech, GC.getTechInfo((TechTypes)iTech).getDescription(), iTeam);
			}
		}

		CvPlayerAI& teamLeader = GET_PLAYER(getLeaderID());

		const bool bAsync = (teamLeader.isHumanPlayer() || GET_PLAYER(GET_TEAM(eTeam).getLeaderID()).isHumanPlayer());

		const float iOurActualTechValue = (float)teamLeader.AI_TechValueCached(eTech, bAsync, true);
		const float iAverageTechValue = (float)teamLeader.AI_averageCurrentTechValue(eTech, bAsync);

		// Multiply the base cost by a squashing function of relative goodness of the proposed tech and an average one from what we can currently research
		const float boost = (iOurActualTechValue - iAverageTechValue) / (iOurActualTechValue + iAverageTechValue);
		const float sigma = 1.0f / (1.0f + exp(-boost));

		int iCost = std::max(1, getResearchCost(eTech) - getResearchProgress(eTech));
		iCost = (int)(iCost * (sigma * sigma * 3 + 0.25f));

		int iValue = iCost * 3/2;

		int iKnownCount = 0;
		int iPossibleKnownCount = 0;

		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getID() && isHasMet((TeamTypes)iI))
			{
				if (GET_TEAM((TeamTypes)iI).isHasTech(eTech))
				{
					iKnownCount++;
				}
				iPossibleKnownCount++;
			}
		}
		iValue += iCost * (iPossibleKnownCount - iKnownCount) / (2*iPossibleKnownCount);

		iValue = getModifiedIntValue(iValue, GC.getTechInfo(eTech).getAITradeModifier());

		m_tradeTechValueCache[iCacheIndex] = iValue;

		return iValue;
	}
}


DenialTypes CvTeamAI::AI_techTrade(const TechTypes eTech, const TeamTypes eTeam) const
{
	PROFILE_FUNC();

	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(!isNPC(), "NPC doesn't trade away tech!");
	FAssertMsg(!GET_TEAM(eTeam).isNPC(), "Impossible to trade away tech to NPC!");

	if (isHuman() || isVassal(eTeam) || isAtWar(eTeam))
	{
		return NO_DENIAL;
	}

	if (AI_getWorstEnemy() == eTeam)
	{
		return DENIAL_WORST_ENEMY;
	}

	const AttitudeTypes eAttitude = AI_getAttitude(eTeam);

	// If any player in the team wonn't trade tech due to its attitude treshold, then no player on the team will.
	for (int i = 0; i < MAX_PC_PLAYERS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(getID())
		&& eAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)i).getPersonalityType()).getTechRefuseAttitudeThreshold())
		{
			return DENIAL_ATTITUDE;
		}
	}
	const CvGame& game = GC.getGame();
	const bool bTechBrokering = !game.isOption(GAMEOPTION_NO_TECH_BROKERING);

	// Afforess - Don't Sell Military Secrets when gearing for war
	if (hasWarPlan(true)
	&& GC.getTechInfo(eTech).getFlavorValue(GC.getInfoTypeForString("FLAVOR_MILITARY")) > 3
	// Only worry about the receiving team if there is no tech brokering.
	&& (bTechBrokering || AI_getWarPlan(eTeam) != NO_WARPLAN))
	{
		return DENIAL_NO_GAIN;
	}

	if (eAttitude < ATTITUDE_FRIENDLY)
	{
		if (bTechBrokering)
		{
			const int iOtherTeamRank = game.getTeamRank(eTeam);
			if (game.getTeamRank(getID()) < iOtherTeamRank || iOtherTeamRank > game.countCivTeamsAlive()/2)
			{
				int iNoTechTradeThreshold = AI_noTechTradeThreshold();

				iNoTechTradeThreshold *= GC.getGameSpeedInfo(game.getGameSpeedType()).getSpeedPercent();
				iNoTechTradeThreshold /= 100;

				iNoTechTradeThreshold *= std::max(0, 100 + GC.getHandicapInfo(GET_TEAM(eTeam).getHandicapType()).getNoTechTradeModifier());
				iNoTechTradeThreshold /= 100;

				if (AI_getMemoryCount(eTeam, MEMORY_RECEIVED_TECH_FROM_ANY) > iNoTechTradeThreshold)
				{
					return DENIAL_TECH_WHORE;
				}
			}
		}

		int iKnownCount = 0;
		int iPossibleKnownCount = 0;

		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getID() && iI != eTeam && isHasMet((TeamTypes)iI))
			{
				if (GET_TEAM((TeamTypes)iI).isHasTech(eTech))
				{
					iKnownCount++;
				}
				iPossibleKnownCount++;
			}
		}
		int iTechTradeKnownPercent = AI_techTradeKnownPercent();

		iTechTradeKnownPercent *= std::max(0, 100 + GC.getHandicapInfo(GET_TEAM(eTeam).getHandicapType()).getTechTradeKnownModifier());
		iTechTradeKnownPercent /= 100;

		iTechTradeKnownPercent *= AI_getTechMonopolyValue(eTech, eTeam);
		iTechTradeKnownPercent /= 100;

		if ((iPossibleKnownCount > 0) ? iKnownCount * 100 / iPossibleKnownCount < iTechTradeKnownPercent : iTechTradeKnownPercent > 0)
		{
			return DENIAL_TECH_MONOPOLY;
		}
	}

	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		if (isWorldUnit((UnitTypes)iI) && getUnitMaking((UnitTypes)iI) > 0
		&& isTechRequiredForUnit(eTech, (UnitTypes)iI))
		{
			return DENIAL_MYSTERY;
		}
	}

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (isTechRequiredForBuilding(eTech, (BuildingTypes)iI)
		&& isWorldWonder((BuildingTypes)iI) && getBuildingMaking((BuildingTypes)iI) > 0)
		{
			return DENIAL_MYSTERY;
		}
	}

	for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		if (GC.getProjectInfo((ProjectTypes)iI).getTechPrereq() == eTech)
		{
			if (isWorldProject((ProjectTypes)iI) && getProjectMaking((ProjectTypes)iI) > 0)
			{
				return DENIAL_MYSTERY;
			}

			for (int iJ = 0; iJ < GC.getNumVictoryInfos(); iJ++)
			{
				if (game.isVictoryValid((VictoryTypes)iJ) && GC.getProjectInfo((ProjectTypes)iI).getVictoryThreshold((VictoryTypes)iJ))
				{
					return DENIAL_VICTORY;
				}
			}
		}
	}

	return NO_DENIAL;
}


int CvTeamAI::AI_mapTradeVal(TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	uint64_t iValue = 0;

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		const CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot->isRevealed(eTeam, false))
		{
			if (!pLoopPlot->isRevealed(getID(), false))
			{
				iValue += pLoopPlot->isWater() ? 15 : 30;
			}
			else
			{
				if (pLoopPlot->getRevealedOwner(eTeam, false) == pLoopPlot->getOwner()
				&& pLoopPlot->getRevealedOwner(getID(), false) != pLoopPlot->getOwner())
				{
					iValue += 4;
				}
				CvCity* pCity = pLoopPlot->getPlotCity();
				if (pCity != NULL && pCity->isRevealed(eTeam, false) && !pCity->isRevealed(getID(), false))
				{
					iValue += 8;
				}
				else
				{
					if (pLoopPlot->getRevealedImprovementType(eTeam) == pLoopPlot->getImprovementType()
					&& pLoopPlot->getRevealedImprovementType(getID()) != pLoopPlot->getImprovementType())
					{
						iValue += 2;
					}
					if (pLoopPlot->getRevealedRouteType(eTeam, false) == pLoopPlot->getRouteType()
					&& pLoopPlot->getRevealedRouteType(getID(), false) != pLoopPlot->getRouteType())
					{
						iValue += 1;
					}
				}
			}
		}
	}
	if (iValue == 0)
	{
		return 0;
	}
	if (hasWarPlan(true))
	{
		// Up to date maps are invaluable when starting a war.
		iValue *= 3;
		iValue /= 2;
		// We should check to see if their map covers the team(s) we are gearing for war with
		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive() && iI != eTeam && iI != getID()
			&& AI_getWarPlan((TeamTypes)iI) != NO_WARPLAN
			&& GET_TEAM(eTeam).AI_teamCloseness((TeamTypes)iI) > 0)
			{
				iValue *= 2;
			}
		}
	}
	iValue *= 1 + GET_PLAYER(getLeaderID()).getCurrentEra();
	iValue /= 3 * (3 + GC.getMap().getWorldSize()); // Map trading makes much more sense on bigger maps anyhow

	if (GET_TEAM(eTeam).isVassal(getID()))
	{
		iValue *= 2;
		iValue /= 3;
	}
	//OutputDebugString(CvString::format("AI_mapTradeVal: how much is the other's (%d) map worth to me (%d)\n\tValue=%I64u\n", (int)eTeam, getID(), iValue).c_str());
	return static_cast<int>(std::min<uint64_t>(iValue, MAX_INT));
}


DenialTypes CvTeamAI::AI_mapTrade(TeamTypes eTeam) const
{
	PROFILE_FUNC();
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	if (isHuman() || isVassal(eTeam) || isAtWar(eTeam))
	{
		return NO_DENIAL;
	}

	if (AI_getWorstEnemy() == eTeam)
	{
		return DENIAL_WORST_ENEMY;
	}

	// Afforess - Selling Maps right before we go to war is stupid
	if (AI_getWarPlan(eTeam) != NO_WARPLAN)
	{
		return DENIAL_MYSTERY;
	}
	int iMapTradeAttitudeOtherTeam = -1;
	for (int i = 0; i < MAX_PC_PLAYERS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(eTeam))
		{
			const int iThreshold = GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)i).getPersonalityType()).getMapRefuseAttitudeThreshold();
			if (iMapTradeAttitudeOtherTeam < iThreshold)
			{
				iMapTradeAttitudeOtherTeam = iThreshold;
			}
		}
	}
	// We should check to see if their map covers the team(s) we are gearing for war with
	if (hasWarPlan(true))
	{
		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive() && iI != eTeam && iI != getID()
			&& AI_getWarPlan((TeamTypes)iI) != NO_WARPLAN
			&& GET_TEAM(eTeam).AI_getAttitude((TeamTypes)iI) >= iMapTradeAttitudeOtherTeam)
			{
				return DENIAL_NO_GAIN;
			}
		}
	}
	const AttitudeTypes eAttitude = AI_getAttitude(eTeam);

	for (int i = 0; i < MAX_PC_PLAYERS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(getID())
		&& eAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)i).getPersonalityType()).getMapRefuseAttitudeThreshold())
		{
			return DENIAL_ATTITUDE;
		}
	}

	return NO_DENIAL;
}


int CvTeamAI::AI_vassalTradeVal(TeamTypes eTeam) const
{
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	return AI_surrenderTradeVal(eTeam);
}


DenialTypes CvTeamAI::AI_vassalTrade(TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	CvTeamAI& kMasterTeam = GET_TEAM(eTeam);

	for (int iLoopTeam = 0; iLoopTeam < MAX_TEAMS; iLoopTeam++)
	{
		CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iLoopTeam);
		if (kLoopTeam.isAlive() && iLoopTeam != getID() && iLoopTeam != kMasterTeam.getID())
		{
			if (!kLoopTeam.isAtWar(kMasterTeam.getID()) && kLoopTeam.isAtWar(getID()))
			{
				if (kMasterTeam.isForcePeace((TeamTypes)iLoopTeam) || !kMasterTeam.canChangeWarPeace((TeamTypes)iLoopTeam))
				{
					if (!kLoopTeam.isAVassal())
					{
						return DENIAL_WAR_NOT_POSSIBLE_YOU;
					}
				}

				if (!kMasterTeam.isHuman())
				{
					DenialTypes eWarDenial = kMasterTeam.AI_declareWarTrade((TeamTypes)iLoopTeam, getID(), true);
					if (NO_DENIAL != eWarDenial)
					{
						return DENIAL_WAR_NOT_POSSIBLE_YOU;
					}
				}
			}
			else if (kLoopTeam.isAtWar(kMasterTeam.getID()) && !kLoopTeam.isAtWar(getID()))
			{
				if (!kMasterTeam.canChangeWarPeace((TeamTypes)iLoopTeam))
				{
					if (!kLoopTeam.isAVassal())
					{
						return DENIAL_PEACE_NOT_POSSIBLE_YOU;
					}
				}

				if (!kMasterTeam.isHuman())
				{
					DenialTypes ePeaceDenial = kMasterTeam.AI_makePeaceTrade((TeamTypes)iLoopTeam, getID());
					if (NO_DENIAL != ePeaceDenial)
					{
						return DENIAL_PEACE_NOT_POSSIBLE_YOU;
					}
				}
			}
		}
	}

	return AI_surrenderTrade(eTeam);
}


int CvTeamAI::AI_surrenderTradeVal(TeamTypes eTeam) const
{
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	return 0;
}


DenialTypes CvTeamAI::AI_surrenderTrade(TeamTypes eMasterTeam, int iPowerMultiplier) const
{
	PROFILE_FUNC();
	const TeamTypes eTeam = getID();
	FAssertMsg(eMasterTeam != eTeam, "shouldn't call this function on ourselves");

	for (int iTeamX = 0; iTeamX < MAX_TEAMS; iTeamX++)
	{
		const CvTeam& teamX = GET_TEAM((TeamTypes)iTeamX);

		if (teamX.isAlive() && iTeamX != eTeam && !teamX.isAVassal() && iTeamX != eMasterTeam)
		{
			if (isAtWar((TeamTypes)iTeamX))
			{
				if (!teamX.isAtWar(eMasterTeam) && !canChangeWarPeace((TeamTypes)iTeamX))
				{
					return DENIAL_PEACE_NOT_POSSIBLE_US;
				}
			}
			else if (teamX.isAtWar(eMasterTeam)
			&& (isForcePeace((TeamTypes)iTeamX) || !canChangeWarPeace((TeamTypes)iTeamX)))
			{
				return DENIAL_WAR_NOT_POSSIBLE_US;
			}
		}
	}
	const CvTeamAI& kMasterTeam = GET_TEAM(eMasterTeam);

	if (isHuman() && kMasterTeam.isHuman())
	{
		return NO_DENIAL;
	}
	const bool bCapitulation = isAtWar(eMasterTeam);

	int iAttitudeModifier = 0;

	if (!kMasterTeam.isParent(eTeam))
	{
		int iPersonalityModifier = 0;
		int iMembers = 0;
		for (int i = 0; i < MAX_PC_PLAYERS; i++)
		{
			if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(eTeam))
			{
				iPersonalityModifier += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)i).getPersonalityType()).getVassalPowerModifier();
				++iMembers;
			}
		}

		int iTotalPower = 0;
		int iNumNonVassals = 0;
		for (int i = 0; i < MAX_PC_TEAMS; i++)
		{
			const CvTeam& teamX = GET_TEAM((TeamTypes) i);
			if (teamX.isAlive())
			{
				if (teamX.isAVassal() && teamX.isCapitulated())
				{
					// Count capitulated vassals as a fractional add to their master's power
					iTotalPower += (2*teamX.getPower(false))/5;
				}
				else if (!teamX.isSingleCityTeam() && !teamX.isMinorCiv() && !teamX.isRebel())
				{
					iTotalPower += teamX.getPower(false);
					iNumNonVassals++;
				}
			}
		}
		int iAveragePower = iTotalPower / std::max(1, iNumNonVassals);
		int iMasterPower = kMasterTeam.getPower(false);
		const int iMyPower = getPower(true);
		const int iVassalPower = iMyPower * (iPowerMultiplier + iPersonalityModifier / std::max(1, iMembers)) / 100;

		if (bCapitulation)
		{
			const int iTheirSuccess = std::max(10, kMasterTeam.AI_getWarSuccess(eTeam));
			int iOurSuccess = std::max(10, AI_getWarSuccess(eMasterTeam));
			int iOthersBestSuccess = 0;
			for (int i = 0; i < MAX_PC_TEAMS; ++i)
			{
				const TeamTypes eTeamX = (TeamTypes) i;

				if (eTeamX != eMasterTeam && eTeamX != eTeam
				&& GET_TEAM(eTeamX).isAlive() && isAtWar(eTeamX))
				{
					const int iSuccess = GET_TEAM(eTeamX).AI_getWarSuccess(eTeam);
					if (iSuccess > iOthersBestSuccess)
					{
						iOthersBestSuccess = iSuccess;
					}
				}
			}

			// Discourage capitulation to a team that has not done the most damage
			if (iTheirSuccess < iOthersBestSuccess)
			{
				iOurSuccess += iOthersBestSuccess - iTheirSuccess;
			}

			iMasterPower = 2 * iMasterPower * iTheirSuccess / (iTheirSuccess + iOurSuccess);

			if (AI_getWorstEnemy() == eMasterTeam)
			{
				iMasterPower *= 3;
				iMasterPower /= 4;
			}
		}
		else if (!kMasterTeam.AI_isLandTarget(eTeam))
		{
			iMasterPower /= 2;
		}

		for (int iTeamX = 0; iTeamX < MAX_PC_TEAMS; iTeamX++)
		{
			if (iTeamX != eTeam)
			{
				const CvTeamAI& teamX = GET_TEAM((TeamTypes)iTeamX);

				if (teamX.isAlive())
				{
					if (iTeamX != eMasterTeam && teamX.AI_isLandTarget(eTeam))
					{
						const int iPowerX = teamX.getPower(true);
						if (iPowerX > iMyPower)
						{
							if (teamX.isAtWar(eMasterTeam) && !isAtWar((TeamTypes)iTeamX))
							{
								return DENIAL_POWER_YOUR_ENEMIES;
							}

							iAveragePower = (2 * iAveragePower * iPowerX) / std::max(1, iPowerX + iMyPower);

							iAttitudeModifier += (3 * iPowerX) / std::max(1, iMyPower) - 2;
						}

						if (!teamX.isAtWar(eMasterTeam) && isAtWar((TeamTypes)iTeamX))
						{
							iAveragePower = iAveragePower * (iMyPower + iMasterPower) / std::max(1, iMyPower);
						}
					}

					if (!bCapitulation && teamX.isAtWar(eMasterTeam) && !isAtWar((TeamTypes)iTeamX))
					{
						const DenialTypes eDenial = AI_declareWarTrade((TeamTypes)iTeamX, eMasterTeam, false);
						if (eDenial != NO_DENIAL)
						{
							return eDenial;
						}
					}
				}
			}
		}

		if (!isVassal(eMasterTeam) && canVassalRevolt(eMasterTeam)
		|| 5 * iVassalPower > 4 * iAveragePower
		|| 3 * iVassalPower > 2 * iMasterPower)
		{
			return DENIAL_POWER_US;
		}

		for (int i = 0; i < GC.getNumVictoryInfos(); i++)
		{
			const int iAdjPopPrcnt = GC.getGame().getAdjustedPopulationPercent((VictoryTypes)i);
			bool bPopulationThreat = true;
			if (iAdjPopPrcnt > 0)
			{
				bPopulationThreat = false;

				const int iThreshold = GC.getGame().getTotalPopulation() * iAdjPopPrcnt;
				if (400 * getTotalPopulation(!isAVassal()) > 3 * iThreshold)
				{
					return DENIAL_VICTORY;
				}

				if (!bCapitulation
				&& 400 * (getTotalPopulation(isAVassal()) + kMasterTeam.getTotalPopulation()) > 3 * iThreshold)
				{
					bPopulationThreat = true;
				}
			}

			const int iAdjLandPrcnt = GC.getGame().getAdjustedLandPercent((VictoryTypes)i);
			bool bLandThreat = true;
			if (iAdjLandPrcnt > 0)
			{
				bLandThreat = false;

				int iThreshold = GC.getMap().getLandPlots() * iAdjLandPrcnt;
				if (400 * getTotalLand(!isAVassal()) > 3 * iThreshold)
				{
					return DENIAL_VICTORY;
				}

				if (!bCapitulation
				&& 400 * (getTotalLand(isAVassal()) + kMasterTeam.getTotalLand()) > 3 * iThreshold)
				{
					bLandThreat = true;
				}
			}

			if (bLandThreat && bPopulationThreat && (iAdjPopPrcnt > 0 || iAdjLandPrcnt > 0))
			{
				return DENIAL_POWER_YOU;
			}
		}
	}

	if (!bCapitulation)
	{
		if (!kMasterTeam.isParent(eTeam))
		{
			if (AI_getWorstEnemy() == eMasterTeam)
			{
				return DENIAL_WORST_ENEMY;
			}

			if (!AI_hasCitiesInPrimaryArea(eMasterTeam) && AI_calculateAdjacentLandPlots(eMasterTeam) == 0)
			{
				return DENIAL_TOO_FAR;
			}
		}

		const AttitudeTypes eAttitude = AI_getAttitude(eMasterTeam, false);

		if (eAttitude <= ATTITUDE_FURIOUS)
		{
			return DENIAL_ATTITUDE;
		}
		const AttitudeTypes eModifiedAttitude = CvPlayerAI::AI_getAttitudeFromValue(AI_getAttitudeVal(eMasterTeam, false) + iAttitudeModifier);

		for (int i = 0; i < MAX_PC_PLAYERS; i++)
		{
			if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(eTeam)
			&& eModifiedAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)i).getPersonalityType()).getVassalRefuseAttitudeThreshold())
			{
				return DENIAL_ATTITUDE;
			}
		}
	}
	else if (!kMasterTeam.isHuman() && !kMasterTeam.AI_acceptSurrender(eTeam)
	// Scale for small empires, particularly necessary if WAR_SUCCESS_CITY_CAPTURING > 10
	|| AI_getWarSuccess(eMasterTeam) + std::min(getNumCities(), 4) * GC.getWAR_SUCCESS_CITY_CAPTURING() > kMasterTeam.AI_getWarSuccess(eTeam))
	{
		return DENIAL_JOKING;
	}

	return NO_DENIAL;
}


bool CvTeamAI::AI_isAnyMemberDoVictoryStrategy( int iVictoryStrategy ) const
{
	PROFILE_EXTRA_FUNC();
	for (int i = 0; i < MAX_PC_PLAYERS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(getID())
		&&  GET_PLAYER((PlayerTypes)i).AI_isDoVictoryStrategy(iVictoryStrategy))
		{
			return true;
		}
	}

	return false;
}

bool CvTeamAI::AI_isAnyMemberDoVictoryStrategyLevel4() const
{
	PROFILE_EXTRA_FUNC();
	for (int i = 0; i < MAX_PC_PLAYERS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(getID())
		&&  GET_PLAYER((PlayerTypes)i).AI_isDoVictoryStrategyLevel4())
		{
			return true;
		}
	}
	return false;
}

bool CvTeamAI::AI_isAnyMemberDoVictoryStrategyLevel3() const
{
	PROFILE_EXTRA_FUNC();
	for (int i = 0; i < MAX_PC_PLAYERS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(getID())
		&&  GET_PLAYER((PlayerTypes)i).AI_isDoVictoryStrategyLevel3())
		{
			return true;
		}
	}
	return false;
}


/// \brief Compute how close our wars are to capitulation.
///
/// At -99, this means we're losing on net enough to capitulate.  At +99, we're winning enough
/// to perhaps have another player capitulate to us.
int CvTeamAI::AI_getWarSuccessCapitulationRatio() const
{
	PROFILE_EXTRA_FUNC();
	int iSum = 0;
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (iI != getID() && GET_TEAM((TeamTypes)iI).isAlive() && isAtWar((TeamTypes)iI))
		{
			iSum += AI_getWarSuccess((TeamTypes)iI) - GET_TEAM((TeamTypes)iI).AI_getWarSuccess(getID());
		}
	}
	return range(100*iSum / std::max(25, std::min(getNumCities(), 4) * GC.getWAR_SUCCESS_CITY_CAPTURING()), -99, 99);
}

/// \brief Compute power of enemies as percentage of our power.
///
///
int CvTeamAI::AI_getEnemyPowerPercent( bool bConsiderOthers ) const
{
	PROFILE_EXTRA_FUNC();
	int iEnemyPower = 0;
	int iMinors = 0;

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (iI != getID() && GET_TEAM((TeamTypes)iI).isAlive()
		&& isHasMet((TeamTypes)iI) && GET_TEAM((TeamTypes)iI).isMinorCiv())
		{
			iMinors++;
		}
	}

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (iI != getID() && GET_TEAM((TeamTypes)iI).isAlive() && isHasMet((TeamTypes)iI))
		{
			if (isAtWar((TeamTypes)iI))
			{
				int iTempPower = 220 * GET_TEAM((TeamTypes)iI).getPower(false);
				iTempPower /= (AI_hasCitiesInPrimaryArea((TeamTypes)iI) ? 2 : 3);
				iTempPower /= (GET_TEAM((TeamTypes)iI).isMinorCiv() ? iMinors : 1);
				iTempPower /= std::max(1, (bConsiderOthers ? GET_TEAM((TeamTypes)iI).getAtWarCount(true,true) : 1));
				iEnemyPower += iTempPower;
			}
			else if (AI_isChosenWar((TeamTypes)iI))
			{
				// Intend to declare war
				int iTempPower = 240 * GET_TEAM((TeamTypes)iI).getDefensivePower();
				iTempPower /= (AI_hasCitiesInPrimaryArea((TeamTypes)iI) ? 2 : 3);
				iTempPower /= 1 + (bConsiderOthers ? GET_TEAM((TeamTypes)iI).getAtWarCount(true,true) : 0);
				iEnemyPower += iTempPower;
			}
		}
	}

	return (iEnemyPower/std::max(1, (isAVassal() ? getCurrentMasterPower(true) : getPower(true))));
}

/// \brief Sum up air power of enemies plus average of other civs we've met.
///
int CvTeamAI::AI_getRivalAirPower( ) const
{
	PROFILE_EXTRA_FUNC();
	// Count enemy air units, not just those visible to us
	int iRivalAirPower = 0;
	int iEnemyAirPower = 0;
	const int iTeam = getID();

	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		const CvUnitInfo& kUnit = GC.getUnitInfo((UnitTypes)iI);

		if (kUnit.getDomainType() == DOMAIN_AIR && kUnit.getAirCombat() > 0)
		{
			for(int iTeamX = 0; iTeamX < MAX_PC_TEAMS; iTeamX++)
			{
				if (iTeamX != iTeam && GET_TEAM((TeamTypes)iTeamX).isAlive() && isHasMet((TeamTypes)iTeamX))
				{
					int iUnitPower = GET_TEAM((TeamTypes)iTeamX).getUnitCount((UnitTypes)iI);
					if (iUnitPower > 0)
					{
						iUnitPower *= kUnit.getPowerValue();

						if (AI_getWarPlan((TeamTypes)iTeamX) == NO_WARPLAN)
						{
							iRivalAirPower += iUnitPower;
						}
						else
						{
							iEnemyAirPower += iUnitPower;
						}
					}
				}
			}
		}
	}
	return (iEnemyAirPower + (iRivalAirPower / std::max(1,getHasMetCivCount(true))));
}

bool CvTeamAI::AI_acceptSurrender(TeamTypes eSurrenderTeam) const
{
	PROFILE_FUNC();

	if (isHuman() || !isAtWar(eSurrenderTeam))
	{
		return true;
	}

	// Capturing capital or Apollo city will stop space
	if (GET_TEAM(eSurrenderTeam).AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_SPACE3)
	// Capturing top culture cities will stop culture
	||  GET_TEAM(eSurrenderTeam).AI_isAnyMemberDoVictoryStrategy(AI_VICTORY_CULTURE3))
	{
		return false;
	}

	// Check for whether another team has won enough to cause capitulation
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getID()
		&& !GET_TEAM((TeamTypes)iI).isVassal(getID())
		&&  GET_TEAM(eSurrenderTeam).isAtWar((TeamTypes)iI)
		&&  GET_TEAM(eSurrenderTeam).AI_getAtWarCounter((TeamTypes)iI) >= 10
		&&	(
				GET_TEAM(eSurrenderTeam).AI_getWarSuccess((TeamTypes)iI)
				+
				std::min(GET_TEAM(eSurrenderTeam).getNumCities(), 4)
				*
				GC.getWAR_SUCCESS_CITY_CAPTURING()
				<
				GET_TEAM((TeamTypes)iI).AI_getWarSuccess(eSurrenderTeam)
			)
		) return true;
	}

	int iValuableCities = 0;
	int iCitiesThreatenedByUs = 0;
	int iValuableCitiesThreatenedByUs = 0;
	int iCitiesThreatenedByOthers = 0;

	const int iLargePop = 3 * (GET_PLAYER(getLeaderID()).getCurrentEra() + 1);

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(eSurrenderTeam))
		{
			foreach_(CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
			{
				bool bValuable =
				(
						pLoopCity->isHolyCity()
					||	pLoopCity->isHeadquarters()
					||	pLoopCity->hasActiveWorldWonder()
					||	AI_isPrimaryArea(pLoopCity->area())
					&&	GET_TEAM(eSurrenderTeam).countNumCitiesByArea(pLoopCity->area()) < 3
					||	pLoopCity->isCapital()
					&& (
							GET_TEAM(eSurrenderTeam).getNumCities() > GET_TEAM(eSurrenderTeam).getNumMembers()
						||	countNumCitiesByArea(pLoopCity->area()) > 0
					)
				);
				if (!bValuable)
				{
					// Valuable terrain bonuses
					foreach_(const CvPlot* loopPlot, pLoopCity->plots(NUM_CITY_PLOTS))
					{
						const BonusTypes eBonus = loopPlot->getNonObsoleteBonusType(getID());

						if (eBonus != NO_BONUS && GET_PLAYER(getLeaderID()).AI_bonusVal(eBonus) > 15)
						{
							bValuable = true;
							break;
						}
					}
				}
				const int iOwnerPower = GET_PLAYER((PlayerTypes)iI).AI_getOurPlotStrength(pLoopCity->plot(), 2, true, false);
				const int iOurPower = AI_getOurPlotStrength(pLoopCity->plot(), 2, false, false, true);
				const int iOtherPower = GET_PLAYER((PlayerTypes)iI).AI_getEnemyPlotStrength(pLoopCity->plot(), 2, false, false) - iOurPower;

				if (iOtherPower > iOwnerPower)
				{
					iCitiesThreatenedByOthers++;
				}

				if (iOurPower > iOwnerPower)
				{
					iCitiesThreatenedByUs++;
					if (bValuable)
					{
						iValuableCities++;
						iValuableCitiesThreatenedByUs++;
						continue;
					}
				}

				if (bValuable && pLoopCity->getHighestPopulation() >= iLargePop)
				{
					if (!AI_isPrimaryArea(pLoopCity->area()))
					{
						for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
						{
							if (GET_PLAYER((PlayerTypes)iJ).isAliveAndTeam(getID())
							&& pLoopCity->AI_playerCloseness((PlayerTypes)iJ) > 5)
							{
								iValuableCities++;
								break;
							}
						}
					}
					else iValuableCities++;
				}
			}
		}
	}

	if (iValuableCitiesThreatenedByUs > 0)
	{
		return false; // Press for capture of valuable city
	}

	if (iCitiesThreatenedByOthers > 1 + iCitiesThreatenedByUs / 2
	// Keep others from capturing spoils, but let it go if surrender civ is too small to care about
	&& 6*(iValuableCities + GET_TEAM(eSurrenderTeam).getNumCities()) > getNumCities())
	{
		return true;
	}

	// If we're low on the totem poll, accept so enemies don't drag anyone else into war with us
	// Top rank is 0, second is 1, etc.
	const int iTeamRank = GC.getGame().getTeamRank(getID());
	if (iTeamRank > 1 + GC.getGame().countCivTeamsAlive() / 3)
	{
		return true;
	}

	const int iOurWarSuccessRatio = AI_getWarSuccessCapitulationRatio();
	if (iOurWarSuccessRatio < -30)
	{
		// We're doing badly overall, need to be done with this war and gain an ally
		return true;
	}

	int iWarCount = 0;
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && iI != eSurrenderTeam
		&& !GET_TEAM((TeamTypes)iI).isMinorCiv()
		&& !GET_TEAM((TeamTypes)iI).isVassal(eSurrenderTeam)
		&& isAtWar((TeamTypes)iI)
		&& GET_TEAM((TeamTypes)iI).AI_getWarSuccess(getID()) > 5 * GC.getWAR_SUCCESS_ATTACKING())
		{
			iWarCount++;
		}
	}

	if (iWarCount > 0 && iOurWarSuccessRatio < 50)
	{
		return true; // Accept if we have other wars to fight
	}

	// War weariness
	const int iWearinessThreshold =
	(
		(GC.getGame().isOption(GAMEOPTION_AI_AGGRESSIVE) ? 300 : 240)
		+
		10 * iValuableCities + 20 * iCitiesThreatenedByUs
	);
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			const int iWarWearinessPercentAnger =
			(
				GET_PLAYER((PlayerTypes)iI).getModifiedWarWearinessPercentAnger(
					getWarWeariness(eSurrenderTeam)
					*
					std::max(0, 100 + GET_TEAM(eSurrenderTeam).getEnemyWarWearinessModifier())
					/ 10000
				)
			);
			// Significant war weariness from eSurrenderTeam
			if (iWarWearinessPercentAnger > 50
			&& GET_PLAYER((PlayerTypes)iI).getWarWearinessPercentAnger() > iWearinessThreshold)
			{
				return true;
			}
		}
	}

	if (iValuableCities + iCitiesThreatenedByUs >= AI_maxWarRand() / 100)
	{
		// Continue conquest
		return false;
	}

	if (GET_TEAM(eSurrenderTeam).getNumCities() < getNumCities() / 4 - AI_maxWarRand() / 100)
	{
		// Too small to bother leaving alive
		return false;
	}

	return true;
}

void CvTeamAI::AI_getWarRands( int &iMaxWarRand, int &iLimitedWarRand, int &iDogpileWarRand ) const
{
	PROFILE_EXTRA_FUNC();
	iMaxWarRand = AI_maxWarRand();
	iLimitedWarRand = AI_limitedWarRand();
	iDogpileWarRand = AI_dogpileWarRand();

	bool bCult4 = false;
	bool bSpace4 = false;
	bool bCult3 = false;
	bool bFinalWar = false;

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			if (GET_PLAYER((PlayerTypes)iI).AI_isDoStrategy(AI_STRATEGY_FINAL_WAR))
			{
				bFinalWar = true;
			}
			if (GET_PLAYER((PlayerTypes)iI).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE4))
			{
				bCult4 = true;
			}
			if (GET_PLAYER((PlayerTypes)iI).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3))
			{
				bCult3 = true;
			}
			if (GET_PLAYER((PlayerTypes)iI).AI_isDoVictoryStrategy(AI_VICTORY_SPACE4))
			{
				bSpace4 = true;
			}
		}
	}

	if (bCult4)
	{
		iMaxWarRand *= 4;
		iLimitedWarRand *= 3;
		iDogpileWarRand *= 2;
	}
	else if (bSpace4)
	{
		iMaxWarRand *= 3;

		iLimitedWarRand *= 2;

		iDogpileWarRand *= 3;
		iDogpileWarRand /= 2;
	}
	else if (bCult3)
	{
		iMaxWarRand *= 2;

		iLimitedWarRand *= 3;
		iLimitedWarRand /= 2;

		iDogpileWarRand *= 3;
		iDogpileWarRand /= 2;
	}

	const int iVassals = getVassalCount();
	if (iVassals > 0)
	{
		const int iDividend = 2 + getNumMembers();
		const int iDivisor = iDividend + iVassals;

		iMaxWarRand *= iDividend;
		iMaxWarRand /= iDivisor;

		iLimitedWarRand *= iDividend;
		iLimitedWarRand /= iDivisor;

		iDogpileWarRand *= iDividend;
		iDogpileWarRand /= iDivisor;
	}

	if (bFinalWar)
	{
		iMaxWarRand /= 4;
	}
}


void CvTeamAI::AI_getWarThresholds( int &iTotalWarThreshold, int &iLimitedWarThreshold, int &iDogpileWarThreshold ) const
{
	PROFILE_EXTRA_FUNC();
	iLimitedWarThreshold = 0;
	iDogpileWarThreshold = 0;

	int iUnitCostPercentTeamSum = 0;
	bool bAggressive = GC.getGame().isOption(GAMEOPTION_AI_AGGRESSIVE);
	bool bConq2 = false;
	bool bDom3 = false;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		const CvPlayerAI& playerX = GET_PLAYER((PlayerTypes)iI);

		if (playerX.isAliveAndTeam(getID()))
		{
			iUnitCostPercentTeamSum +=
			(
				static_cast<int>(
					100 * playerX.getFinalUnitUpkeep()
					/
					std::max<int64_t>(1, playerX.calculatePreInflatedCosts())
				)
			);
			if (!bAggressive
			&& (playerX.AI_isDoStrategy(AI_STRATEGY_DAGGER)
			||  playerX.AI_isDoVictoryStrategy(AI_VICTORY_CONQUEST4)
			||  playerX.AI_isDoVictoryStrategy(AI_VICTORY_DOMINATION4)))
			{
				bAggressive = true;
			}
			if (!bDom3)
			{
				if (GET_PLAYER((PlayerTypes)iI).AI_isDoVictoryStrategy(AI_VICTORY_DOMINATION3))
				{
					bDom3 = true;
				}
				else if (!bConq2 && GET_PLAYER((PlayerTypes)iI).AI_isDoVictoryStrategy(AI_VICTORY_CONQUEST2))
				{
					bConq2 = true;
				}
			}
		}
	}

	iTotalWarThreshold = iUnitCostPercentTeamSum / std::max(1, getNumMembers());
	if (bAggressive)
	{
		iTotalWarThreshold *= 2;
		iDogpileWarThreshold += 2;
		iLimitedWarThreshold += 2;
	}
	if (bDom3)
	{
		iTotalWarThreshold *= 3;
		iDogpileWarThreshold += 5;
		iLimitedWarThreshold += 2;
	}
	else if (bConq2)
	{
		iTotalWarThreshold *= 2;
		iDogpileWarThreshold += 2;
		iLimitedWarThreshold += 1;
	}
	iTotalWarThreshold /= 3;
}

// Returns odds of player declaring total war times 100
int CvTeamAI::AI_getTotalWarOddsTimes100( ) const
{
	int iTotalWarRand;
	int iLimitedWarRand;
	int iDogpileWarRand;
	AI_getWarRands( iTotalWarRand, iLimitedWarRand, iDogpileWarRand );

	int iTotalWarThreshold;
	int iLimitedWarThreshold;
	int iDogpileWarThreshold;
	AI_getWarThresholds( iTotalWarThreshold, iLimitedWarThreshold, iDogpileWarThreshold );

	return ((100 * 100 * iTotalWarThreshold) / std::max(1, iTotalWarRand));
}


int CvTeamAI::AI_makePeaceTradeVal(TeamTypes ePeaceTeam, TeamTypes eTeam) const
{
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(ePeaceTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(GET_TEAM(ePeaceTeam).isAlive(), "GET_TEAM(ePeaceTeam).isAlive is expected to be true");
	FAssertMsg(atWar(ePeaceTeam, eTeam), "eTeam should be at war with ePeaceTeam");

	int iValue = 50 + GC.getGame().getGameTurn();
	iValue += 8 * (GET_TEAM(eTeam).getNumCities() + GET_TEAM(ePeaceTeam).getNumCities());

	int iModifier = 0;

	switch ((GET_TEAM(eTeam).AI_getAttitude(ePeaceTeam) + GET_TEAM(ePeaceTeam).AI_getAttitude(eTeam)) / 2)
	{
	case ATTITUDE_FURIOUS:
		iModifier += 400;
		break;

	case ATTITUDE_ANNOYED:
		iModifier += 200;
		break;

	case ATTITUDE_CAUTIOUS:
		iModifier += 100;
		break;

	case ATTITUDE_PLEASED:
		iModifier += 50;
		break;

	case ATTITUDE_FRIENDLY:
		break;

	default:
		FErrorMsg("error");
		break;
	}

	iValue *= std::max(0, (iModifier + 100));
	iValue /= 100;

	iValue *= 40;
	iValue /= (GET_TEAM(eTeam).AI_getAtWarCounter(ePeaceTeam) + 10);

	iValue -= (iValue % GC.getDIPLOMACY_VALUE_REMAINDER());

	return isHuman() ? std::max(iValue, GC.getDIPLOMACY_VALUE_REMAINDER()) : iValue;
}


DenialTypes CvTeamAI::AI_makePeaceTrade(TeamTypes ePeaceTeam, TeamTypes eTeam) const
{
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(ePeaceTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(GET_TEAM(ePeaceTeam).isAlive(), "GET_TEAM(ePeaceTeam).isAlive is expected to be true");
	FAssertMsg(isAtWar(ePeaceTeam), "should be at war with ePeaceTeam");

	if (GET_TEAM(ePeaceTeam).isHuman())
	{
		return DENIAL_CONTACT_THEM;
	}

	if (GET_TEAM(ePeaceTeam).isAVassal())
	{
		return DENIAL_VASSAL;
	}

	if (isHuman())
	{
		return NO_DENIAL;
	}

	if (!canChangeWarPeace(ePeaceTeam))
	{
		return DENIAL_VASSAL;
	}

	if (AI_endWarVal(ePeaceTeam) > (GET_TEAM(ePeaceTeam).AI_endWarVal(getID()) * 2))
	{
		return DENIAL_CONTACT_THEM;
	}

	int iLandRatio = ((getTotalLand(true) * 100) / std::max(20, GET_TEAM(eTeam).getTotalLand(true)));
	if (iLandRatio > 250)
	{
		return DENIAL_VICTORY;
	}

	return NO_DENIAL;
}


int CvTeamAI::AI_declareWarTradeVal(TeamTypes eWarTeam, TeamTypes eTeam) const
{
	PROFILE_FUNC();
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(eWarTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(GET_TEAM(eWarTeam).isAlive(), "GET_TEAM(eWarTeam).isAlive is expected to be true");
	FAssertMsg(!atWar(eWarTeam, eTeam), "eTeam should be at peace with eWarTeam");

	int iValue = 0;
	iValue += (GET_TEAM(eWarTeam).getNumCities() * 10);
	iValue += (GET_TEAM(eWarTeam).getTotalPopulation(true) * 2);

	int iModifier = 0;

	switch (GET_TEAM(eTeam).AI_getAttitude(eWarTeam))
	{
	case ATTITUDE_FURIOUS:
		break;

	case ATTITUDE_ANNOYED:
		iModifier += 25;
		break;

	case ATTITUDE_CAUTIOUS:
		iModifier += 50;
		break;

	case ATTITUDE_PLEASED:
		iModifier += 150;
		break;

	case ATTITUDE_FRIENDLY:
		iModifier += 400;
		break;

	default:
		FErrorMsg("error");
		break;
	}

	iValue *= std::max(0, (iModifier + 100));
	iValue /= 100;

	int iTheirPower = GET_TEAM(eTeam).getPower(true);
	int iWarTeamPower = GET_TEAM(eWarTeam).getPower(true);

	iValue *= 50 + ((100 * iWarTeamPower) / (iTheirPower + iWarTeamPower + 1));
	iValue /= 100;

	if (!(GET_TEAM(eTeam).AI_isAllyLandTarget(eWarTeam)))
	{
		iValue *= 2;
	}

	if (!isAtWar(eWarTeam))
	{
		iValue *= 3;
	}
	else
	{
		iValue *= 150;
		iValue /= 100 + ((50 * std::min(100, (100 * AI_getWarSuccess(eWarTeam)) / (8 + getTotalPopulation(false)))) / 100);
	}

	iValue += (GET_TEAM(eTeam).getNumCities() * 20);
	iValue += (GET_TEAM(eTeam).getTotalPopulation(true) * 15);

	if (isAtWar(eWarTeam))
	{
		switch (GET_TEAM(eTeam).AI_getAttitude(getID()))
		{
		case ATTITUDE_FURIOUS:
		case ATTITUDE_ANNOYED:
		case ATTITUDE_CAUTIOUS:
			iValue *= 100;
			break;

		case ATTITUDE_PLEASED:
			iValue *= std::max(75, 100 - getAtWarCount(true) * 10);
			break;

		case ATTITUDE_FRIENDLY:
			iValue *= std::max(50, 100 - getAtWarCount(true) * 20);
			break;

		default:
			FErrorMsg("error");
			break;
		}
		iValue /= 100;
	}

	iValue += GET_TEAM(eWarTeam).getNumNukeUnits() * 250;//Don't want to get nuked
	iValue += GET_TEAM(eTeam).getNumNukeUnits() * 150;//Don't want to use nukes on another's behalf

	if (!GET_TEAM(eWarTeam).isAtWar(true))
	{
		iValue *= 2;

		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getID() && iI != eWarTeam && iI != eTeam
			&& GET_TEAM(eWarTeam).isDefensivePact((TeamTypes)iI))
			{
				iValue += (GET_TEAM((TeamTypes)iI).getNumCities() * 30);
				iValue += (GET_TEAM((TeamTypes)iI).getTotalPopulation(true) * 20);
			}
		}
	}

	iValue *= 60 + (140 * GC.getGame().getGameTurn()) / std::max(1, GC.getGame().getEstimateEndTurn());
	iValue /= 100;

	iValue -= (iValue % GC.getDIPLOMACY_VALUE_REMAINDER());

	if (isHuman())
	{
		return std::max(iValue, GC.getDIPLOMACY_VALUE_REMAINDER());
	}
	else
	{
		return iValue;
	}
}


DenialTypes CvTeamAI::AI_declareWarTrade(TeamTypes eWarTeam, TeamTypes eTeam, bool bConsiderPower) const
{
	PROFILE_FUNC();
//	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	//AI Autoplay calls this
	FAssertMsg(eWarTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(GET_TEAM(eWarTeam).isAlive(), "GET_TEAM(eWarTeam).isAlive is expected to be true");
	FAssertMsg(!isAtWar(eWarTeam), "should be at peace with eWarTeam");

	if (GET_TEAM(eWarTeam).isVassal(eTeam) || GET_TEAM(eWarTeam).isDefensivePact(eTeam))
	{
		return DENIAL_JOKING;
	}

	if (!canDeclareWar(eWarTeam))
	{
		return DENIAL_VASSAL;
	}

	// Afforess - Already decided to go to war with them, would be silly to refuse the offer.
	if (isHuman() || AI_isChosenWar(eWarTeam))
	{
		return NO_DENIAL;
	}

	if (!GC.getGame().isOption(GAMEOPTION_AI_RUTHLESS) && hasWarPlan(true))
	{
		return DENIAL_TOO_MANY_WARS;
	}

	if (bConsiderPower)
	{
		const bool bLandTarget = AI_isAllyLandTarget(eWarTeam);

		if (GET_TEAM(eWarTeam).getDefensivePower() / (bLandTarget ? 2 : 1) >
			getPower(true) + (atWar(eWarTeam, eTeam) ? GET_TEAM(eTeam).getPower(true) : 0))
		{
			return bLandTarget ? DENIAL_POWER_THEM : DENIAL_NO_GAIN;
		}
	}

	// Afforess - Backstab enemies
	if (AI_getMemoryCount(eWarTeam, MEMORY_DECLARED_WAR) > 0)
	{
		return NO_DENIAL;
	}

	if (hasWarPlan(true))
	{
		return DENIAL_TOO_MANY_WARS;
	}

	const AttitudeTypes eAttitude = AI_getAttitude(eTeam);

	// Afforess - Attitude doesn't matter much to Ruthless AI
	if (!GC.getGame().isOption(GAMEOPTION_AI_RUTHLESS) || eAttitude == ATTITUDE_FURIOUS)
	{
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID())
			&& eAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getDeclareWarRefuseAttitudeThreshold())
			{
				return DENIAL_ATTITUDE;
			}
		}
	}

	const AttitudeTypes eAttitudeToVictim = AI_getAttitude(eWarTeam);

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID())
		&& eAttitudeToVictim > GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getDeclareWarThemRefuseAttitudeThreshold())
		{
			return DENIAL_ATTITUDE_THEM;
		}
	}

	// Nuclear deterrent
	if (!atWar(eWarTeam, eTeam) && GET_TEAM(eWarTeam).getNumNukeUnits() > 0)
	{
		return DENIAL_JOKING;
	}
	return NO_DENIAL;
}


int CvTeamAI::AI_openBordersTradeVal(TeamTypes eTeam) const
{
	// Double value if we are planning war, but not against eTeam.
	if (AI_getWarPlan(eTeam) == NO_WARPLAN && hasWarPlan(true))
	{
		return 2 * (getNumCities() + GET_TEAM(eTeam).getNumCities());
	}
	return getNumCities() + GET_TEAM(eTeam).getNumCities();
}


DenialTypes CvTeamAI::AI_openBordersTrade(TeamTypes eTeam) const
{
	PROFILE_FUNC();
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(!isNPC(), "NPC's doesn't trade!");

	if (isHuman() || isVassal(eTeam) || AI_shareWar(eTeam))
	{
		return NO_DENIAL;
	}

	if (AI_getWorstEnemy() == eTeam)
	{
		return DENIAL_WORST_ENEMY;
	}

	if (AI_getWarPlan(eTeam) != NO_WARPLAN)
	{
		return DENIAL_MYSTERY; // We are going to attack eTeam soon
	}

	// Agree to open borders if planning war against their neighbour.
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getID()
		&& AI_getWarPlan((TeamTypes)iI) != NO_WARPLAN
		&& GET_TEAM(eTeam).AI_hasAdjacentLandPlots((TeamTypes)iI))
		{
			return NO_DENIAL;
		}
	}

	if (AI_getMemoryCount(eTeam, MEMORY_CANCELLED_OPEN_BORDERS) > 0)
	{
		return DENIAL_RECENT_CANCEL;
	}
	const AttitudeTypes eAttitude = AI_getAttitude(eTeam);

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID())
		&& eAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getOpenBordersRefuseAttitudeThreshold())
		{
			return DENIAL_ATTITUDE;
		}
	}
	return NO_DENIAL;
}


int CvTeamAI::AI_defensivePactTradeVal(TeamTypes eTeam) const
{
	return ((getNumCities() + GET_TEAM(eTeam).getNumCities()) * 3);
}


DenialTypes CvTeamAI::AI_defensivePactTrade(TeamTypes eTeam) const
{
	PROFILE_FUNC();
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");
	FAssertMsg(!isNPC(), "NPC's doesn't trade!");

	if (isHuman())
	{
		return NO_DENIAL;
	}

	if (GC.getGame().countCivTeamsAlive() == 2)
	{
		return DENIAL_NO_GAIN;
	}

	if (AI_getWorstEnemy() == eTeam)
	{
		return DENIAL_WORST_ENEMY;
	}

	const AttitudeTypes eAttitude = AI_getAttitude(eTeam);

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID())
		&& eAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getDefensivePactRefuseAttitudeThreshold())
		{
			return DENIAL_ATTITUDE;
		}
	}

	return NO_DENIAL;
}


DenialTypes CvTeamAI::AI_permanentAllianceTrade(TeamTypes eTeam) const
{
	PROFILE_FUNC();
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	if (isHuman())
	{
		return NO_DENIAL;
	}

	if (AI_getWorstEnemy() == eTeam)
	{
		return DENIAL_WORST_ENEMY;
	}

	if (getPower(true) + GET_TEAM(eTeam).getPower(true) > GC.getGame().countTotalCivPower() / 2)
	{
		if (getPower(true) > GET_TEAM(eTeam).getPower(true))
		{
			return DENIAL_POWER_US;
		}
		return DENIAL_POWER_YOU;
	}

	if (AI_getDefensivePactCounter(eTeam) + AI_getShareWarCounter(eTeam) < 40)
	{
		return DENIAL_NOT_ALLIED;
	}

	const AttitudeTypes eAttitude = AI_getAttitude(eTeam);

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID())
		&& eAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getPermanentAllianceRefuseAttitudeThreshold())
		{
			return DENIAL_ATTITUDE;
		}
	}

	return NO_DENIAL;
}


TeamTypes CvTeamAI::AI_getWorstEnemy() const
{
	return m_eWorstEnemy;
}

void CvTeamAI::AI_updateWorstEnemy()
{
	if (isNPC())
	{
		return;
	}
	PROFILE_FUNC();

	int iBestValue = MAX_INT;
	TeamTypes eBestTeam = NO_TEAM;
	const bool bRuthless = GC.getGame().isOption(GAMEOPTION_AI_RUTHLESS);

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		const TeamTypes eTeamX = (TeamTypes) iI;
		const CvTeam& teamX = GET_TEAM(eTeamX);

		if (teamX.isAlive() && iI != getID() && !teamX.isVassal(getID()) && isHasMet(eTeamX)
		&& AI_getAttitude(eTeamX) < (bRuthless ? ATTITUDE_FRIENDLY : ATTITUDE_CAUTIOUS))
		{
			const int iValue =
			(
				AI_getAttitudeVal(eTeamX) +
				// Our Worst enemy isn't just the person we hate the most,
				// but the person we hate and is winning! (Ruthless)
				(bRuthless ? GC.getGame().getPlayerRank(teamX.getLeaderID())/2 : 0)
			);
			if (iValue < iBestValue && (int)eTeamX != getID())
			{
				iBestValue = iValue;
				eBestTeam = eTeamX;
			}
		}
	}
	m_eWorstEnemy = eBestTeam;
}


int CvTeamAI::AI_getWarPlanStateCounter(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_aiWarPlanStateCounter[eIndex];
}


void CvTeamAI::AI_setWarPlanStateCounter(TeamTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	m_aiWarPlanStateCounter[eIndex] = iNewValue;
	FASSERT_NOT_NEGATIVE(AI_getWarPlanStateCounter(eIndex));
}


void CvTeamAI::AI_changeWarPlanStateCounter(TeamTypes eIndex, int iChange)
{
	AI_setWarPlanStateCounter(eIndex, (AI_getWarPlanStateCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getAtWarCounter(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_aiAtWarCounter[eIndex];
}


void CvTeamAI::AI_setAtWarCounter(TeamTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	//	Koshling - trying to increment your own atWar (with self) counter should never
	//	happen, but as a fallback we trap it here
	if ( iNewValue == 0 || eIndex != getID() )
	{
		m_aiAtWarCounter[eIndex] = iNewValue;
		FASSERT_NOT_NEGATIVE(AI_getAtWarCounter(eIndex));
	}
	else
	{
		FErrorMsg("Team attempting to incremnt it's own atWar counter (wuith itself)");
	}
}


void CvTeamAI::AI_changeAtWarCounter(TeamTypes eIndex, int iChange)
{
	AI_setAtWarCounter(eIndex, (AI_getAtWarCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getAtPeaceCounter(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_aiAtPeaceCounter[eIndex];
}


void CvTeamAI::AI_setAtPeaceCounter(TeamTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	m_aiAtPeaceCounter[eIndex] = iNewValue;
	FASSERT_NOT_NEGATIVE(AI_getAtPeaceCounter(eIndex));
}


void CvTeamAI::AI_changeAtPeaceCounter(TeamTypes eIndex, int iChange)
{
	AI_setAtPeaceCounter(eIndex, (AI_getAtPeaceCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getHasMetCounter(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_aiHasMetCounter[eIndex];
}


void CvTeamAI::AI_setHasMetCounter(TeamTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	m_aiHasMetCounter[eIndex] = iNewValue;
	FASSERT_NOT_NEGATIVE(AI_getHasMetCounter(eIndex));
}


void CvTeamAI::AI_changeHasMetCounter(TeamTypes eIndex, int iChange)
{
	AI_setHasMetCounter(eIndex, (AI_getHasMetCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getOpenBordersCounter(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_aiOpenBordersCounter[eIndex];
}


void CvTeamAI::AI_setOpenBordersCounter(TeamTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	m_aiOpenBordersCounter[eIndex] = iNewValue;
	FASSERT_NOT_NEGATIVE(AI_getOpenBordersCounter(eIndex));
}


void CvTeamAI::AI_changeOpenBordersCounter(TeamTypes eIndex, int iChange)
{
	AI_setOpenBordersCounter(eIndex, (AI_getOpenBordersCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getDefensivePactCounter(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_aiDefensivePactCounter[eIndex];
}


void CvTeamAI::AI_setDefensivePactCounter(TeamTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	m_aiDefensivePactCounter[eIndex] = iNewValue;
	FASSERT_NOT_NEGATIVE(AI_getDefensivePactCounter(eIndex));
}


void CvTeamAI::AI_changeDefensivePactCounter(TeamTypes eIndex, int iChange)
{
	AI_setDefensivePactCounter(eIndex, (AI_getDefensivePactCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getShareWarCounter(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_aiShareWarCounter[eIndex];
}


void CvTeamAI::AI_setShareWarCounter(TeamTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	m_aiShareWarCounter[eIndex] = iNewValue;
	FASSERT_NOT_NEGATIVE(AI_getShareWarCounter(eIndex));
}


void CvTeamAI::AI_changeShareWarCounter(TeamTypes eIndex, int iChange)
{
	AI_setShareWarCounter(eIndex, (AI_getShareWarCounter(eIndex) + iChange));
}


int CvTeamAI::AI_getWarSuccess(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_aiWarSuccess[eIndex];
}


void CvTeamAI::AI_setWarSuccess(TeamTypes eIndex, int iNewValue)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	if (m_aiWarSuccess[eIndex] != iNewValue)
	{
		for (int iA = 0; iA < MAX_PC_PLAYERS; iA++)
		{
			CvPlayerAI& playerA = GET_PLAYER((PlayerTypes)iA);

			if (playerA.isAliveAndTeam(getID()))
			{
				for (int iB = 0; iB < MAX_PC_PLAYERS; iB++)
				{
					if (GET_PLAYER((PlayerTypes)iB).isAliveAndTeam(eIndex))
					{
						GET_PLAYER((PlayerTypes)iB).AI_invalidateAttitudeCache((PlayerTypes)iA);
						playerA.AI_invalidateAttitudeCache((PlayerTypes)iB);
					}
				}
			}
		}
		m_aiWarSuccess[eIndex] = iNewValue;
		FASSERT_NOT_NEGATIVE(AI_getWarSuccess(eIndex));
	}
}


void CvTeamAI::AI_changeWarSuccess(TeamTypes eIndex, int iChange)
{
	PROFILE_FUNC();

/************************************************************************************************/
/* REVOLUTION_MOD                         05/30/08                                jdog5000      */
/*                                                                                              */
/* Revolution AI                                                                                */
/************************************************************************************************/
	// Rebels celebrate successes more
	if( isRebelAgainst(eIndex) )
	{
		iChange *= 3;
		iChange /= 2;
	}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
	AI_setWarSuccess(eIndex, (AI_getWarSuccess(eIndex) + iChange));
}


int CvTeamAI::AI_getEnemyPeacetimeTradeValue(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_aiEnemyPeacetimeTradeValue[eIndex];
}


void CvTeamAI::AI_setEnemyPeacetimeTradeValue(TeamTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	m_aiEnemyPeacetimeTradeValue[eIndex] = iNewValue;
	FASSERT_NOT_NEGATIVE(AI_getEnemyPeacetimeTradeValue(eIndex));
}


void CvTeamAI::AI_changeEnemyPeacetimeTradeValue(TeamTypes eIndex, int iChange)
{
	AI_setEnemyPeacetimeTradeValue(eIndex, (AI_getEnemyPeacetimeTradeValue(eIndex) + iChange));
}


int CvTeamAI::AI_getEnemyPeacetimeGrantValue(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_aiEnemyPeacetimeGrantValue[eIndex];
}


void CvTeamAI::AI_setEnemyPeacetimeGrantValue(TeamTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	m_aiEnemyPeacetimeGrantValue[eIndex] = iNewValue;
	FASSERT_NOT_NEGATIVE(AI_getEnemyPeacetimeGrantValue(eIndex));
}


void CvTeamAI::AI_changeEnemyPeacetimeGrantValue(TeamTypes eIndex, int iChange)
{
	AI_setEnemyPeacetimeGrantValue(eIndex, (AI_getEnemyPeacetimeGrantValue(eIndex) + iChange));
}


WarPlanTypes CvTeamAI::AI_getWarPlan(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	FAssert(eIndex != getID() || m_aeWarPlan[eIndex] == NO_WARPLAN);
	return m_aeWarPlan[eIndex];
}


bool CvTeamAI::AI_isChosenWar(TeamTypes eIndex) const
{
	switch (AI_getWarPlan(eIndex))
	{
	case WARPLAN_ATTACKED_RECENT:
	case WARPLAN_ATTACKED:
		return false;
		break;
	case WARPLAN_PREPARING_LIMITED:
	case WARPLAN_PREPARING_TOTAL:
	case WARPLAN_LIMITED:
	case WARPLAN_TOTAL:
	case WARPLAN_DOGPILE:
		return true;
		break;
	}

	return false;
}


bool CvTeamAI::AI_isSneakAttackPreparing(TeamTypes eIndex) const
{
	return ((AI_getWarPlan(eIndex) == WARPLAN_PREPARING_LIMITED) || (AI_getWarPlan(eIndex) == WARPLAN_PREPARING_TOTAL));
}


bool CvTeamAI::AI_isSneakAttackReady(TeamTypes eIndex) const
{
	return AI_isChosenWar(eIndex) && !AI_isSneakAttackPreparing(eIndex);
}


void CvTeamAI::AI_setWarPlan(TeamTypes eIndex, WarPlanTypes eNewValue, bool bWar, bool bInFull)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	if (AI_getWarPlan(eIndex) != eNewValue && (bWar || !isAtWar(eIndex)))
	{
		m_aeWarPlan[eIndex] = eNewValue;

		if (bInFull)
		{
			AI_setWarPlanStateCounter(eIndex, 0);

			AI_updateAreaStragies();
		}

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()) && !GET_PLAYER((PlayerTypes)iI).isHumanPlayer())
			{
				GET_PLAYER((PlayerTypes)iI).AI_makeProductionDirty();
			}
		}
	}
}

//if this number is over 0 the teams are "close"
//this may be expensive to run, kinda O(N^2)...
int CvTeamAI::AI_teamCloseness(TeamTypes eIndex, int iMaxDistance) const
{
	PROFILE_FUNC();
	// Toffer - NPC doesn't care what team it is close to, and the feeling is mutual.
	//	NPC's are kinda everywhere and can thus have simplified logic for these kind of things.
	//	Changed MAX_PLAYERS to MAX_PC_PLAYERS in the loops below; NPC didn't call this func at the time of this change.
	FAssert(eIndex < MAX_PC_TEAMS);
	FAssert(!isNPC());
	FAssert(eIndex != getID());

	if (iMaxDistance == -1)
	{
		iMaxDistance = DEFAULT_PLAYER_CLOSENESS;
	}
	int iValue = 0;
	for (int iA = 0; iA < MAX_PC_PLAYERS; iA++)
	{
		const CvPlayerAI& playerA = GET_PLAYER((PlayerTypes)iA);

		if (playerA.isAliveAndTeam(getID()))
		{
			for (int iB = 0; iB < MAX_PC_PLAYERS; iB++)
			{
				if (GET_PLAYER((PlayerTypes)iB).isAliveAndTeam(eIndex))
				{
					iValue += playerA.AI_playerCloseness((PlayerTypes)iB, iMaxDistance);
				}
			}
		}
	}
	return iValue;
}


void CvTeamAI::read(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	CvTeam::read(pStream);

	WRAPPER_READ_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, m_aiWarPlanStateCounter);
	WRAPPER_READ_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, m_aiAtWarCounter);
	WRAPPER_READ_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, m_aiAtPeaceCounter);
	WRAPPER_READ_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, m_aiHasMetCounter);
	WRAPPER_READ_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, m_aiOpenBordersCounter);
	WRAPPER_READ_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, m_aiDefensivePactCounter);
	WRAPPER_READ_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, m_aiShareWarCounter);
	WRAPPER_READ_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, m_aiWarSuccess);
	WRAPPER_READ_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, m_aiEnemyPeacetimeTradeValue);
	WRAPPER_READ_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, m_aiEnemyPeacetimeGrantValue);
	WRAPPER_READ_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, (int*)m_aeWarPlan);

	int iWorstEnemy = -1;
	WRAPPER_READ(wrapper, "CvTeamAI", &iWorstEnemy);
	m_eWorstEnemy = (TeamTypes)iWorstEnemy;

	//	Koshling - defend against apparent corrupt states (that have been observed)
	if ( m_aeWarPlan[getID()] != NO_WARPLAN )
	{
		m_aeWarPlan[getID()] = NO_WARPLAN;

		FErrorMsg("Team apparently planning war with itself! (corrected)");
	}
	if ( m_eWorstEnemy == getID() )
	{
		m_eWorstEnemy = NO_TEAM;

		FErrorMsg("Team apparently its own worst enemy! (corrected)");
	}

	WRAPPER_READ(wrapper, "CvTeamAI", &m_iNoTechTradeThreshold);
	WRAPPER_READ(wrapper, "CvTeamAI", &m_iTechTradeKnownPercent);
	WRAPPER_READ(wrapper, "CvTeamAI", &m_iMaxWarRand);
	WRAPPER_READ(wrapper, "CvTeamAI", &m_iMaxWarNearbyPowerRatio);
	WRAPPER_READ(wrapper, "CvTeamAI", &m_iMaxWarDistantPowerRatio);
	WRAPPER_READ(wrapper, "CvTeamAI", &m_iMaxWarMinAdjacentLandPercent);
	WRAPPER_READ(wrapper, "CvTeamAI", &m_iLimitedWarRand);
	WRAPPER_READ(wrapper, "CvTeamAI", &m_iLimitedWarPowerRatio);
	WRAPPER_READ(wrapper, "CvTeamAI", &m_iDogpileWarRand);
	WRAPPER_READ(wrapper, "CvTeamAI", &m_iMakePeaceRand);
	WRAPPER_READ_OBJECT_END(wrapper);

/* Needed if getMaxCivPlayers return MAX_PC_PLAYERS, now it returns MAX_PLAYERS-1.
	if (getID() == MAX_PC_TEAMS)
	{
		//Read NPC data
		for (int iI = MAX_PC_TEAMS+1; iI < MAX_TEAMS; iI++)
		{
			GET_TEAM((TeamTypes)iI).read(pStream);
		}
	}
*/
}


void CvTeamAI::write(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	CvTeam::write(pStream);

	WRAPPER_WRITE_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, m_aiWarPlanStateCounter);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, m_aiAtWarCounter);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, m_aiAtPeaceCounter);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, m_aiHasMetCounter);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, m_aiOpenBordersCounter);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, m_aiDefensivePactCounter);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, m_aiShareWarCounter);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, m_aiWarSuccess);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, m_aiEnemyPeacetimeTradeValue);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, m_aiEnemyPeacetimeGrantValue);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeamAI", MAX_TEAMS, (int*)m_aeWarPlan);

	if ( m_eWorstEnemy == getID() )
	{
		m_eWorstEnemy = NO_TEAM;

		FErrorMsg("Team apparently its own worst enemy! (corrected)");
	}
	WRAPPER_WRITE(wrapper, "CvTeamAI", (int)m_eWorstEnemy);
	WRAPPER_WRITE(wrapper, "CvTeamAI", m_iNoTechTradeThreshold);
	WRAPPER_WRITE(wrapper, "CvTeamAI", m_iTechTradeKnownPercent);
	WRAPPER_WRITE(wrapper, "CvTeamAI", m_iMaxWarRand);
	WRAPPER_WRITE(wrapper, "CvTeamAI", m_iMaxWarNearbyPowerRatio);
	WRAPPER_WRITE(wrapper, "CvTeamAI", m_iMaxWarDistantPowerRatio);
	WRAPPER_WRITE(wrapper, "CvTeamAI", m_iMaxWarMinAdjacentLandPercent);
	WRAPPER_WRITE(wrapper, "CvTeamAI", m_iLimitedWarRand);
	WRAPPER_WRITE(wrapper, "CvTeamAI", m_iLimitedWarPowerRatio);
	WRAPPER_WRITE(wrapper, "CvTeamAI", m_iDogpileWarRand);
	WRAPPER_WRITE(wrapper, "CvTeamAI", m_iMakePeaceRand);

	WRAPPER_WRITE_OBJECT_END(wrapper);

/* Needed if getMaxCivPlayers return MAX_PC_PLAYERS, now it returns MAX_PLAYERS-1.
	if (getID() == MAX_PC_TEAMS)
	{
		//write NPC data
		for (int iI = MAX_PC_TEAMS+1; iI < MAX_TEAMS; iI++)
		{
			GET_TEAM((TeamTypes)iI).write(pStream);
		}
	}
*/
}

int CvTeamAI::AI_noTechTradeThreshold(bool bRecalculate) const
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(!isNPC(), "No point calling this for NPC");
	if (!bRecalculate)
	{
		return m_iNoTechTradeThreshold;
	}
	int iRand = 0;
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getNoTechTradeThreshold();
			iCount++;
		}
	}
	if (iCount > 1)
	{
		iRand /= iCount;
	}
	// Afforess - Ruthless AI: Trade More Techs
	if (GC.getGame().isOption(GAMEOPTION_AI_RUTHLESS))
	{
		return iRand * 3;
	}
	return iRand;
}


int CvTeamAI::AI_techTradeKnownPercent(bool bRecalculate) const
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(!isNPC(), "No point calling this for NPC");
	if (!bRecalculate)
	{
		return m_iTechTradeKnownPercent;
	}
	int iRand = 0;
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getTechTradeKnownPercent();
			iCount++;
		}
	}
	if (iCount > 1)
	{
		iRand /= iCount;
	}
	// Afforess - Ruthless AI: Trade More Techs, even techs that others haven't discovered
	if (GC.getGame().isOption(GAMEOPTION_AI_RUTHLESS))
	{
		return iRand / 3;
	}
	return iRand;
}


int CvTeamAI::AI_maxWarRand(bool bRecalculate) const
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(!isNPC(), "No point calling this for NPC");
	if (!bRecalculate)
	{
		return m_iMaxWarRand;
	}
	int iRand = 0;
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getMaxWarRand();
			iCount++;
		}
	}
	return iCount > 1 ? iRand/iCount : iRand;
}


int CvTeamAI::AI_maxWarNearbyPowerRatio(bool bRecalculate) const
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(!isNPC(), "No point calling this for NPC");
	if (!bRecalculate)
	{
		return m_iMaxWarNearbyPowerRatio;
	}
	int iRand = 0;
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getMaxWarNearbyPowerRatio();
			iCount++;
		}
	}
	if (iCount > 1)
	{
		iRand /= iCount;
	}
	// Afforess - Ruthless AI attack weaker/closer targets
	if (GC.getGame().isOption(GAMEOPTION_AI_RUTHLESS))
	{
		return iRand / 2;
	}
	return iRand;
}


int CvTeamAI::AI_maxWarDistantPowerRatio(bool bRecalculate) const
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(!isNPC(), "No point calling this for NPC");
	if (!bRecalculate)
	{
		return m_iMaxWarDistantPowerRatio;
	}
	int iRand = 0;
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getMaxWarDistantPowerRatio();
			iCount++;
		}
	}
	if (iCount > 1)
	{
		iRand /= iCount;
	}
	// Afforess - Ruthless AI avoid far away targets
	if (GC.getGame().isOption(GAMEOPTION_AI_RUTHLESS))
	{
		return iRand / 3;
	}
	return iRand;
}


int CvTeamAI::AI_maxWarMinAdjacentLandPercent(bool bRecalculate) const
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(!isNPC(), "No point calling this for NPC");
	if (!bRecalculate)
	{
		return m_iMaxWarMinAdjacentLandPercent;
	}
	int iRand = 0;
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getMaxWarMinAdjacentLandPercent();
			iCount++;
		}
	}
	if (iCount > 1)
	{
		iRand /= iCount;
	}
	// Afforess - Ruthless AI Favors closer targets
	if (GC.getGame().isOption(GAMEOPTION_AI_RUTHLESS))
	{
		return iRand / 4;
	}
	return iRand;
}


int CvTeamAI::AI_limitedWarRand(bool bRecalculate) const
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(!isNPC(), "No point calling this for NPC");
	if (!bRecalculate)
	{
		return m_iLimitedWarRand;
	}
	int iRand = 0;
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getLimitedWarRand();
			iCount++;
		}
	}
	return iCount > 1 ? iRand/iCount : iRand;
}


int CvTeamAI::AI_limitedWarPowerRatio(bool bRecalculate) const
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(!isNPC(), "No point calling this for NPC");
	if (!bRecalculate)
	{
		return m_iLimitedWarPowerRatio;
	}
	int iRand = 0;
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getLimitedWarPowerRatio();
			iCount++;
		}
	}
	return iCount > 1 ? iRand/iCount : iRand;
}


int CvTeamAI::AI_dogpileWarRand(bool bRecalculate) const
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(!isNPC(), "No point calling this for NPC");
	if (!bRecalculate)
	{
		return m_iDogpileWarRand;
	}
	int iRand = 0;
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getDogpileWarRand();
			iCount++;
		}
	}
	return iCount > 1 ? iRand/iCount : iRand;
}


int CvTeamAI::AI_makePeaceRand(bool bRecalculate) const
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(!isNPC(), "No point calling this for NPC");

	if (!bRecalculate)
	{
		return m_iMakePeaceRand;
	}
	int iRand = 0;
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iRand += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getMakePeaceRand();
			iCount++;
		}
	}
	return iCount > 1 ? iRand/iCount : iRand;
}


int CvTeamAI::AI_noWarAttitudeProb(AttitudeTypes eAttitude) const
{
	PROFILE_EXTRA_FUNC();
	int iVictoryStrategyAdjust = 0;
	int iCount = 0;
	int iProb = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iProb += GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getNoWarAttitudeProb(eAttitude);
			iCount++;

			// In final stages of miltaristic victory, AI may turn on its friends!
			if (GET_PLAYER((PlayerTypes)iI).AI_isDoVictoryStrategy(AI_VICTORY_CONQUEST4))
			{
				iVictoryStrategyAdjust += 30;
			}
			else if (GET_PLAYER((PlayerTypes)iI).AI_isDoVictoryStrategy(AI_VICTORY_DOMINATION4))
			{
				iVictoryStrategyAdjust += 20;
			}
		}
	}
	if (iCount > 1)
	{
		iProb /= iCount;
		iVictoryStrategyAdjust /= iCount;
	}
	iProb -= iVictoryStrategyAdjust;

	if (iProb < 1)
	{
		return 0;
	}
	// Afforess - Friends are just enemies we haven't made yet.
	if (GC.getGame().isOption(GAMEOPTION_AI_RUTHLESS))
	{
		return iProb / 3;
	}
	return iProb;
}


void CvTeamAI::AI_doCounter()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (iI != getID())
			{
				AI_changeWarPlanStateCounter(((TeamTypes)iI), 1);

				if (isAtWar((TeamTypes)iI))
				{
					AI_changeAtWarCounter(((TeamTypes)iI), 1);
				}
				else
				{
					AI_changeAtPeaceCounter(((TeamTypes)iI), 1);
				}

				if (isHasMet((TeamTypes)iI))
				{
					AI_changeHasMetCounter(((TeamTypes)iI), 1);
				}

				if (isOpenBorders((TeamTypes)iI))
				{
					AI_changeOpenBordersCounter(((TeamTypes)iI), 1);
				}

				if (isDefensivePact((TeamTypes)iI))
				{
					AI_changeDefensivePactCounter(((TeamTypes)iI), 1);
				}
				else
				{
					if (AI_getDefensivePactCounter((TeamTypes)iI) > 0)
					{
						AI_changeDefensivePactCounter(((TeamTypes)iI), -1);
					}
				}

				if (isHasMet((TeamTypes)iI))
				{
					if (AI_shareWar((TeamTypes)iI))
					{
						AI_changeShareWarCounter(((TeamTypes)iI), 1);
					}
				}
			}
		}
	}
}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/26/10                                jdog5000      */
/*                                                                                              */
/* War Strategy AI                                                                              */
/************************************************************************************************/
// Block AI from declaring war on a distant vassal if it shares an area with the master
bool CvTeamAI::AI_isOkayVassalTarget(const TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	if( GET_TEAM(eTeam).isAVassal() )
	{
		if( !(AI_hasCitiesInPrimaryArea(eTeam)) || AI_calculateAdjacentLandPlots(eTeam) == 0 )
		{
			for( int iI = 0; iI < MAX_PC_TEAMS; iI++ )
			{
				if( GET_TEAM(eTeam).isVassal((TeamTypes)iI) )
				{
					if( AI_hasCitiesInPrimaryArea((TeamTypes)iI) && AI_calculateAdjacentLandPlots((TeamTypes)iI) > 0)
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/


/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      04/25/10                                jdog5000      */
/*                                                                                              */
/* War Strategy, AI logging                                                                     */
/************************************************************************************************/
/// \brief Make war decisions, mainly for starting or switching war plans.
///
///
void CvTeamAI::AI_doWar()
{
	PROFILE_FUNC();
	FAssert(!isHuman());
	FAssert(!isNPC());
	FAssert(!isMinorCiv());

	if (isAVassal())
	{
		return;
	}
	//Base financial stats off team lead player, likely other team members (vassals) are not doing "better" than the leader.
	CvPlayerAI& teamLeader = GET_PLAYER(getLeaderID());

	const int iEnemyPowerPercent = AI_getEnemyPowerPercent();

	int iExtraWarExpenses = GC.getDefineINT("ESTIMATED_EXTRA_WAR_EXPENSE_PERCENT", 12);
	// Revolutions means wars are more risky, in terms of finances, as poor finances can lead to revolutions.
	if (GC.getGame().isOption(GAMEOPTION_UNSUPPORTED_REVOLUTION))
	{
		iExtraWarExpenses *= 125;
		iExtraWarExpenses /= 100;
	}

	int iTimeModifier = 100;
	{
		int iThreshold = (80*AI_maxWarNearbyPowerRatio())/100;

		if( iEnemyPowerPercent < iThreshold )
		{
			iTimeModifier *= iEnemyPowerPercent;
			iTimeModifier /= iThreshold;
		}

		iTimeModifier *= 50 + GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent();
		iTimeModifier /= 150;
		FASSERT_NOT_NEGATIVE(iTimeModifier);
	}

	int iAbandonTimeModifier = 100;
	iAbandonTimeModifier *= 50 + GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent();
	iAbandonTimeModifier /= 150;

	//Afforess - abandon plans more quickly in financial distress
	const short iFundedPercent = teamLeader.AI_fundingHealth(0, iExtraWarExpenses);
	const int iSafePercent = teamLeader.AI_safeFunding();

	if (iSafePercent > iFundedPercent)
	{
		int iPrevAbandonTimeModifier = iAbandonTimeModifier;
		int iFundingGap = range(iSafePercent - iFundedPercent, 0, 100);
		iAbandonTimeModifier *= (100 - iFundingGap);
		iAbandonTimeModifier /= 100;
		//Apply effect twice
		iAbandonTimeModifier *= (100 - iFundingGap);
		iAbandonTimeModifier /= 100;

		if (gTeamLogLevel >= 1)
		{
			logBBAI("      Team %d (%S) WARPLAN has a financial funding gap of %d. iPrevAbandonTimeModifier: %d, iAbandonTimeModifier: %d", getID(), teamLeader.getCivilizationDescription(0), iFundingGap, iPrevAbandonTimeModifier, iAbandonTimeModifier);
		}
	}
	// Afforess - End

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && isHasMet((TeamTypes)iI) && AI_getWarPlan((TeamTypes)iI) != NO_WARPLAN)
		{
			const bool bEnemyVictoryLevel4 = GET_TEAM((TeamTypes)iI).AI_isAnyMemberDoVictoryStrategyLevel4();

			if (AI_getWarPlan((TeamTypes)iI) == WARPLAN_ATTACKED_RECENT)
			{
				FAssert(isAtWar((TeamTypes)iI));

				if (AI_getAtWarCounter((TeamTypes)iI) > (GET_TEAM((TeamTypes)iI).AI_isLandTarget(getID()) ? 9 : 3))
				{
					if (gTeamLogLevel >= 1)
					{
						logBBAI(
							"      Team %d (%S) switching WARPLANS against team %d (%S) from ATTACKED_RECENT to ATTACKED with enemy power percent %d",
							getID(), teamLeader.getCivilizationDescription(0), iI,
							GET_PLAYER(GET_TEAM((TeamTypes)iI).getLeaderID()).getCivilizationDescription(0), iEnemyPowerPercent
						);
					}
					AI_setWarPlan((TeamTypes)iI, WARPLAN_ATTACKED);
				}
			}
			else if (AI_getWarPlan((TeamTypes)iI) == WARPLAN_PREPARING_LIMITED)
			{
				FAssert(canEventuallyDeclareWar((TeamTypes)iI));

				if (AI_getWarPlanStateCounter((TeamTypes)iI) > ((5 * iTimeModifier) / (bEnemyVictoryLevel4 ? 400 : 100)))
				{
					if (gTeamLogLevel >= 1)
					{
						logBBAI(
							"      Team %d (%S) switching WARPLANS against team %d (%S) from PREPARING_LIMITED to LIMITED after %d turns with enemy power percent %d",
							getID(), teamLeader.getCivilizationDescription(0), iI,
							GET_PLAYER(GET_TEAM((TeamTypes)iI).getLeaderID()).getCivilizationDescription(0),
							AI_getWarPlanStateCounter((TeamTypes)iI), iEnemyPowerPercent
						);
					}
					AI_setWarPlan((TeamTypes)iI, WARPLAN_LIMITED);
				}
			}
			else if (AI_getWarPlan((TeamTypes)iI) == WARPLAN_LIMITED || AI_getWarPlan((TeamTypes)iI) == WARPLAN_DOGPILE)
			{
				if (!isAtWar((TeamTypes)iI))
				{
					FAssert(canEventuallyDeclareWar((TeamTypes)iI));

					bool bTarget = false;
					for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++ )
					{
						if (GET_PLAYER((PlayerTypes)iJ).getTeam() == getID() && GET_PLAYER((PlayerTypes)iJ).AI_enemyTargetMissions((TeamTypes)iI) > 0)
						{
							bTarget = true;
							break;
						}
					}

					if (!bTarget && AI_getWarPlanStateCounter((TeamTypes)iI) > iAbandonTimeModifier * 15/100)
					{
						if (gTeamLogLevel >= 1)
						{
							logBBAI(
								"      Team %d (%S) abandoning WARPLAN_LIMITED or WARPLAN_DOGPILE against team %d (%S) after %d turns with enemy power percent %d",
								getID(), teamLeader.getCivilizationDescription(0), iI,
								GET_PLAYER(GET_TEAM((TeamTypes)iI).getLeaderID()).getCivilizationDescription(0),
								AI_getWarPlanStateCounter((TeamTypes)iI), iEnemyPowerPercent
							);
						}
						AI_setWarPlan((TeamTypes)iI, NO_WARPLAN);
					}

					if (AI_getWarPlan((TeamTypes)iI) == WARPLAN_DOGPILE && !GET_TEAM((TeamTypes)iI).isAtWar())
					{
						if (gTeamLogLevel >= 1)
						{
							logBBAI(
								"      Team %d (%S) abandoning WARPLAN_DOGPILE against team %d (%S) after %d turns because enemy has no war with enemy power percent %d",
								getID(), teamLeader.getCivilizationDescription(0), iI,
								GET_PLAYER(GET_TEAM((TeamTypes)iI).getLeaderID()).getCivilizationDescription(0),
								AI_getWarPlanStateCounter((TeamTypes)iI), iEnemyPowerPercent
							);
						}
						AI_setWarPlan((TeamTypes)iI, NO_WARPLAN);
					}
				}
			}
			else if (AI_getWarPlan((TeamTypes)iI) == WARPLAN_PREPARING_TOTAL)
			{
				FAssert(canEventuallyDeclareWar((TeamTypes)iI));

				if (AI_getWarPlanStateCounter((TeamTypes)iI) > 10 * iTimeModifier / (bEnemyVictoryLevel4 ? 400 : 100))
				{
					bool bAllAreasValid = false;
					bool bSomeAreasValid = false;

					foreach_(const CvArea* pLoopArea, GC.getMap().areas())
					{
						if (AI_isPrimaryArea(pLoopArea) && GET_TEAM((TeamTypes)iI).countNumCitiesByArea(pLoopArea) > 0)
						{
							bSomeAreasValid = true;

							const AreaAITypes eAreaAI = AI_calculateAreaAIType(pLoopArea, true);

							if (eAreaAI == AREAAI_DEFENSIVE)
							{
								bAllAreasValid = false;
								break;
							}
							else if (!bAllAreasValid && eAreaAI == AREAAI_OFFENSIVE)
							{
								bAllAreasValid = true;
							}
						}
					}

					if (bAllAreasValid && iEnemyPowerPercent < 140 || !bSomeAreasValid && iEnemyPowerPercent < 110 || GET_TEAM((TeamTypes)iI).AI_getLowestVictoryCountdown() >= 0)
					{
						if (gTeamLogLevel >= 1)
						{
							logBBAI(
								"      Team %d (%S) switching WARPLANS against team %d (%S) from PREPARING_TOTAL to TOTAL after %d turns with enemy power percent %d",
								getID(), teamLeader.getCivilizationDescription(0), iI,
								GET_PLAYER(GET_TEAM((TeamTypes)iI).getLeaderID()).getCivilizationDescription(0),
								AI_getWarPlanStateCounter((TeamTypes)iI), iEnemyPowerPercent
							);
						}
						AI_setWarPlan((TeamTypes)iI, WARPLAN_TOTAL);
					}
					else if (AI_getWarPlanStateCounter((TeamTypes)iI) > iAbandonTimeModifier / 5)
					{
						if (gTeamLogLevel >= 1)
						{
							logBBAI(
								"      Team %d (%S) abandoning WARPLAN_TOTAL_PREPARING against team %d (%S) after %d turns with enemy power percent %d",
								getID(), teamLeader.getCivilizationDescription(0), iI,
								GET_PLAYER(GET_TEAM((TeamTypes)iI).getLeaderID()).getCivilizationDescription(0),
								AI_getWarPlanStateCounter((TeamTypes)iI), iEnemyPowerPercent
							);
						}
						AI_setWarPlan((TeamTypes)iI, NO_WARPLAN);
					}
				}
			}
			else if (AI_getWarPlan((TeamTypes)iI) == WARPLAN_TOTAL)
			{
				if (!isAtWar((TeamTypes)iI))
				{
					FAssert(canEventuallyDeclareWar((TeamTypes)iI));

					bool bActive = false;
					for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
					{
						if (GET_PLAYER((PlayerTypes)iJ).getTeam() == getID()
						&&  GET_PLAYER((PlayerTypes)iJ).AI_enemyTargetMissions((TeamTypes)iI) > 0)
						{
							bActive = true;
							break;
						}
					}

					if (!bActive && AI_getWarPlanStateCounter((TeamTypes)iI) > iAbandonTimeModifier * 25/100)
					{
						if (gTeamLogLevel >= 1)
						{
							logBBAI("      Team %d (%S) abandoning WARPLAN_TOTAL against team %d (%S) after %d turns with enemy power percent %d", getID(), teamLeader.getCivilizationDescription(0), iI, GET_PLAYER(GET_TEAM((TeamTypes)iI).getLeaderID()).getCivilizationDescription(0), AI_getWarPlanStateCounter((TeamTypes)iI), iEnemyPowerPercent );
						}
						AI_setWarPlan(((TeamTypes)iI), NO_WARPLAN);
					}
				}
			}
		}
	}

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			GET_PLAYER((PlayerTypes)iI).AI_doPeace();
		}
	}

	// if at war, check for making peace
	if (isAtWar() && GC.getGame().getSorenRandNum(AI_makePeaceRand(), "AI Make Peace") == 0)
	{
		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getID()
			&& !GET_TEAM((TeamTypes)iI).isHuman() && canContact((TeamTypes)iI))
			{
				FAssert(!GET_TEAM((TeamTypes)iI).isMinorCiv());

				if (isAtWar((TeamTypes)iI) && AI_isChosenWar((TeamTypes)iI)
				&& AI_getAtWarCounter((TeamTypes)iI) > std::max(10, GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() * 14/100))
				{
					// If nothing is happening in war
					if (AI_getWarSuccess((TeamTypes)iI) + GET_TEAM((TeamTypes)iI).AI_getWarSuccess(getID()) < 2*GC.getWAR_SUCCESS_ATTACKING()
					&& GC.getGame().getSorenRandNum(8, "AI Make Peace 1") == 0)
					{
						bool bValid = true;

						for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
						{
							if (GET_PLAYER((PlayerTypes)iJ).getTeam() == getID())
							{
								if (GET_PLAYER((PlayerTypes)iJ).AI_enemyTargetMissions((TeamTypes)iI) > 0)
								{
									bValid = false;
									break;
								}
							}
							else if (GET_PLAYER((PlayerTypes)iJ).getTeam() == iI
							&& GET_PLAYER((PlayerTypes)iJ).AI_enemyTargetMissions(getID()) > 0)
							{
								bValid = false;
								break;
							}
						}

						if (bValid)
						{
							makePeace((TeamTypes)iI);

							if (gTeamLogLevel >= 1)
							{
								logBBAI("  Team %d (%S) making peace due to time and no fighting", getID(), teamLeader.getCivilizationDescription(0));
							}
							break;
						}
					}

					// Fought to a long draw
					if (AI_getAtWarCounter((TeamTypes)iI) > (AI_getWarPlan((TeamTypes)iI) == WARPLAN_TOTAL ? 40 : 30) * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 100)
					{
						int iOurValue = AI_endWarVal((TeamTypes)iI);
						int iTheirValue = GET_TEAM((TeamTypes)iI).AI_endWarVal(getID());
						if (iOurValue > iTheirValue / 2 && iTheirValue > iOurValue / 2)
						{
							if (gTeamLogLevel >= 1)
							{
								logBBAI("  Team %d (%S) making peace due to time and endWarVal %d vs their %d", getID(), teamLeader.getCivilizationDescription(0) , iOurValue, iTheirValue );
							}
							makePeace((TeamTypes)iI);
							break;
						}
					}

					// All alone in what we thought was a dogpile
					if (AI_getWarPlan((TeamTypes)iI) == WARPLAN_DOGPILE
					&& GET_TEAM((TeamTypes)iI).getAtWarCount(true) == 1
					&& GET_TEAM((TeamTypes)iI).AI_endWarVal(getID()) > AI_endWarVal((TeamTypes)iI) / 2)
					{
						if (gTeamLogLevel >= 1)
						{
							logBBAI("  Team %d (%S) making peace due to being only dog-piler left", getID(), teamLeader.getCivilizationDescription(0) );
						}
						makePeace((TeamTypes)iI);
						break;
					}
				}
			}
		}
	}
	const int iNumMembers = getNumMembers();

	// if no war plans, consider starting one!
	if (!hasWarPlan(true) || iEnemyPowerPercent < 45)
	{
		bool bAggressive = GC.getGame().isOption(GAMEOPTION_AI_AGGRESSIVE);
		int iFinancialTroubleCount = 0;
		int iDaggerCount = 0;
		int iGetBetterUnitsCount = 0;
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			CvPlayerAI& playerX = GET_PLAYER((PlayerTypes)iI);
			if (playerX.isAliveAndTeam(getID()))
			{
				if (playerX.AI_isDoStrategy(AI_STRATEGY_DAGGER)
				||  playerX.AI_isDoVictoryStrategy(AI_VICTORY_CONQUEST3)
				||  playerX.AI_isDoVictoryStrategy(AI_VICTORY_DOMINATION4))
				{
					iDaggerCount++;
					bAggressive = true;
				}
				if (playerX.AI_isDoStrategy(AI_STRATEGY_GET_BETTER_UNITS))
				{
					iGetBetterUnitsCount++;
				}
				if (playerX.AI_isFinancialTrouble())
				{
					iFinancialTroubleCount++;
				}
			}
		}
		if (gTeamLogLevel >= 1)
		{
			logBBAI("  Team %d (%S) estimating warplan financial costs, iExtraWarExpenses: %d, iFundedPercent: %d, iSafePercent: %d", getID(), teamLeader.getCivilizationDescription(0), iExtraWarExpenses, iFundedPercent, iSafePercent);
		}

		// We oppose war if half the non-dagger teammates in financial trouble
		bool bFinancesOpposeWar = 2 * (iFinancialTroubleCount - iDaggerCount) >= getNumMembers();

		if (!bFinancesOpposeWar)
		{
			const bool bFinancesProTotalWar = iFundedPercent > iSafePercent;

			// Set some sane default values
			bool bFinancesProLimitedWar = bFinancesProTotalWar;
			bool bFinancesProDogpileWar = bFinancesProTotalWar;

			// Only check limited and dogpile finances if we can not afford a total war. If we can afford a total war, we can afford any war.
			if (!bFinancesProTotalWar)
			{
				// Afforess - It is possible a more limited war could be cheaper
				const short iLimitedWarFundedPercent = teamLeader.AI_fundingHealth(0, iExtraWarExpenses / 2);
				bFinancesProLimitedWar = iLimitedWarFundedPercent > iSafePercent;

				const short iDogPileFundedPercent = teamLeader.AI_fundingHealth(0, iExtraWarExpenses / 3);
				bFinancesProDogpileWar = iDogPileFundedPercent > iSafePercent;

				if (gTeamLogLevel >= 1)
				{
					logBBAI("  Team %d (%S) estimating LIMITED warplan financial costs, iExtraWarExpenses: %d, iLimitedWarFundedPercent: %d, iSafePercent: %d", getID(), teamLeader.getCivilizationDescription(0), iExtraWarExpenses / 2, iLimitedWarFundedPercent, iSafePercent);
					logBBAI("  Team %d (%S) estimating DOGPILE warplan financial costs, iExtraWarExpenses: %d, iDogPileFundedPercent: %d, iSafePercent: %d", getID(), teamLeader.getCivilizationDescription(0), iExtraWarExpenses / 3, iDogPileFundedPercent, iSafePercent);
				}
				// Finances oppose war if we can't afford any of em
				if (!bFinancesProDogpileWar && !bFinancesProLimitedWar)
				{
					bFinancesOpposeWar = true;
				}
			}

			// overall war check (quite frequently true)
			if (!bFinancesOpposeWar && (iGetBetterUnitsCount - iDaggerCount) * 3 < iNumMembers * 2
			// random overall war chance (at noble+ difficulties this is 100%)
			&& GC.getGame().getSorenRandNum(100, "AI Declare War 1") < GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIDeclareWarProb())
			{
				// if random in this range is 0, we go to war of this type (so lower numbers are higher probablity)
				// average of everyone on our team
				int iTotalWarRand;
				int iLimitedWarRand;
				int iDogpileWarRand;
				AI_getWarRands( iTotalWarRand, iLimitedWarRand, iDogpileWarRand );

				int iTotalWarThreshold;
				int iLimitedWarThreshold;
				int iDogpileWarThreshold;
				AI_getWarThresholds( iTotalWarThreshold, iLimitedWarThreshold, iDogpileWarThreshold );

				int iOurPower = getPower(true);

				if (bAggressive && !hasWarPlan(true))
				{
					iOurPower *= 4;
					iOurPower /= 3;
				}

				iOurPower *= (100 - iEnemyPowerPercent);
				iOurPower /= 100;

				TeamTypes eBestTeam = NO_TEAM;
				int iBestValue = 0;
				int iNoWarRoll;

				if (bFinancesProTotalWar && GC.getGame().getSorenRandNum(iTotalWarRand, "AI Maximum War") <= iTotalWarThreshold)
				{
					iNoWarRoll = GC.getGame().getSorenRandNum(100, "AI No War");
					iNoWarRoll = range(iNoWarRoll + (bAggressive ? 10 : 0) + (bFinancesProTotalWar ? 10 : 0) - (20*iGetBetterUnitsCount)/iNumMembers, 0, 99);

					for (int iPass = 0; iPass < 3; iPass++)
					{
						for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
						{
							if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getID() && isHasMet((TeamTypes)iI)
							&& canEventuallyDeclareWar((TeamTypes)iI)
							&& (!GET_TEAM((TeamTypes)iI).isAVassal() || AI_isOkayVassalTarget((TeamTypes)iI))
							&& iNoWarRoll >= AI_noWarAttitudeProb(AI_getAttitude((TeamTypes)iI))
							&& GET_TEAM((TeamTypes)iI).getDefensivePower() * 2/3 < iOurPower * (iPass > 1 ? AI_maxWarDistantPowerRatio() : AI_maxWarNearbyPowerRatio()) / 100
							// XXX make sure they share an area....
							&& (iPass > 1 || AI_isLandTarget((TeamTypes)iI) || AI_isAnyCapitalAreaAlone() || GET_TEAM((TeamTypes)iI).AI_isAnyMemberDoVictoryStrategyLevel4())
							&& (iPass > 0 || AI_calculateAdjacentLandPlots((TeamTypes)iI) >= getTotalLand() * AI_maxWarMinAdjacentLandPercent() / 100 || GET_TEAM((TeamTypes)iI).AI_isAnyMemberDoVictoryStrategyLevel4()))
							{
								const int iValue = AI_startWarVal((TeamTypes)iI);
								if (iValue > 0)
								{
									if (gTeamLogLevel >= 2)
									{
										logBBAI(
											"      Team %d (%S) considering starting TOTAL warplan with team %d with value %d on pass %d with %d adjacent plots",
											getID(), teamLeader.getCivilizationDescription(0), iI, iValue, iPass, AI_calculateAdjacentLandPlots((TeamTypes)iI)
										);
									}
									if (iValue > iBestValue)
									{
										iBestValue = iValue;
										eBestTeam = ((TeamTypes)iI);
									}
								}
							}
						}

						if (eBestTeam != NO_TEAM)
						{
							if (gTeamLogLevel >= 1)
							{
								logBBAI("    Team %d (%S) starting TOTAL warplan preparations against team %d on pass %d", getID(), teamLeader.getCivilizationDescription(0), eBestTeam, iPass );
							}
							AI_setWarPlan(eBestTeam, (iDaggerCount > 0) ? WARPLAN_TOTAL : WARPLAN_PREPARING_TOTAL);
							break;
						}
					}
				}
				else if (bFinancesProLimitedWar && GC.getGame().getSorenRandNum(iLimitedWarRand, "AI Limited War") <= iLimitedWarThreshold)
				{
					iNoWarRoll = GC.getGame().getSorenRandNum(100, "AI No War") - 10;
					iNoWarRoll = range(iNoWarRoll + (bAggressive ? 10 : 0) + (bFinancesProLimitedWar ? 10 : 0), 0, 99);

					for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
					{
						if (GET_TEAM((TeamTypes)iI).isAlive())
						{
							if (iI != getID())
							{
								if (isHasMet((TeamTypes)iI))
								{
									if (canEventuallyDeclareWar((TeamTypes)iI))
									{
										if( GET_TEAM((TeamTypes)iI).isAVassal() && !AI_isOkayVassalTarget((TeamTypes)iI) )
										{
											continue;
										}

										if (iNoWarRoll >= AI_noWarAttitudeProb(AI_getAttitude((TeamTypes)iI)))
										{
											if (AI_isLandTarget((TeamTypes)iI) || (AI_isAnyCapitalAreaAlone() && GET_TEAM((TeamTypes)iI).AI_isAnyCapitalAreaAlone()))
											{
												if (GET_TEAM((TeamTypes)iI).getDefensivePower() < ((iOurPower * AI_limitedWarPowerRatio()) / 100) &&
													!AI_shareWar((TeamTypes)iI) )
												{
													const int iValue = AI_startWarVal((TeamTypes)iI);

													if (iValue > 0 && gTeamLogLevel >= 2)
													{
														logBBAI(
															"      Team %d (%S) considering starting LIMITED warplan with team %d with value %d",
															getID(), teamLeader.getCivilizationDescription(0), iI, iValue
														);
													}

													if (iValue > iBestValue)
													{
														iBestValue = iValue;
														eBestTeam = ((TeamTypes)iI);
													}
												}
											}
										}
									}
								}
							}
						}
					}

					if (eBestTeam != NO_TEAM)
					{
						if( gTeamLogLevel >= 1 )
						{
							logBBAI("    Team %d (%S) starting LIMITED warplan preparations against team %d", getID(), teamLeader.getCivilizationDescription(0), eBestTeam );
						}

						AI_setWarPlan(eBestTeam, (iDaggerCount > 0) ? WARPLAN_LIMITED : WARPLAN_PREPARING_LIMITED);
					}
				}
				else if (bFinancesProDogpileWar && GC.getGame().getSorenRandNum(iDogpileWarRand, "AI Dogpile War") <= iDogpileWarThreshold)
				{
					iNoWarRoll = GC.getGame().getSorenRandNum(100, "AI No War") - 20;
					iNoWarRoll = range(iNoWarRoll + (bAggressive ? 10 : 0) + (bFinancesProDogpileWar ? 10 : 0), 0, 99);

					for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
					{
						if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getID() && isHasMet((TeamTypes)iI) && canDeclareWar((TeamTypes)iI))
						{
							if (GET_TEAM((TeamTypes)iI).isAVassal() && !AI_isOkayVassalTarget((TeamTypes)iI))
							{
								continue;
							}

							if (iNoWarRoll >= AI_noWarAttitudeProb(AI_getAttitude((TeamTypes)iI)) && GET_TEAM((TeamTypes)iI).isAtWar()
							&& (AI_isLandTarget((TeamTypes)iI) || GET_TEAM((TeamTypes)iI).AI_isAnyMemberDoVictoryStrategyLevel4()))
							{
								int iDogpilePower = iOurPower;

								for (int iJ = 0; iJ < MAX_PC_TEAMS; iJ++)
								{
									if (GET_TEAM((TeamTypes)iJ).isAlive() && iJ != iI && atWar((TeamTypes)iJ, (TeamTypes)iI))
									{
										iDogpilePower += GET_TEAM((TeamTypes)iJ).getPower(false);
									}
								}

								FAssert(GET_TEAM((TeamTypes)iI).getPower(true) == GET_TEAM((TeamTypes)iI).getDefensivePower() || GET_TEAM((TeamTypes)iI).isAVassal());

								if (GET_TEAM((TeamTypes)iI).getDefensivePower() * 3 / 2 < iDogpilePower)
								{
									const int iValue = AI_startWarVal((TeamTypes)iI);

									if( iValue > 0 && gTeamLogLevel >= 2 )
									{
										logBBAI("      Team %d (%S) considering starting DOGPILE warplan with team %d with value %d", getID(), teamLeader.getCivilizationDescription(0), iI, iValue );
									}

									if (iValue > iBestValue)
									{
										FAssert(!AI_shareWar((TeamTypes)iI));
										iBestValue = iValue;
										eBestTeam = ((TeamTypes)iI);
									}
								}
							}
						}
					}

					if (eBestTeam != NO_TEAM)
					{
						if( gTeamLogLevel >= 1 )
						{
							logBBAI("  Team %d (%S) starting DOGPILE warplan preparations with team %d", getID(), teamLeader.getCivilizationDescription(0), eBestTeam );
						}
						AI_setWarPlan(eBestTeam, WARPLAN_DOGPILE);
					}
				}
			}
		}
	}
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/


//returns true if war is veto'd by rolls.
bool CvTeamAI::AI_performNoWarRolls(TeamTypes eTeam)
{
	if (GC.getGame().getSorenRandNum(100, "AI Declare War 1") > GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIDeclareWarProb())
	{
		return true;
	}

	if (GC.getGame().getSorenRandNum(100, "AI No War") <= AI_noWarAttitudeProb(AI_getAttitude(eTeam)))
	{
		return true;
	}

	return false;
}

int CvTeamAI::AI_getAttitudeWeight(const TeamTypes eTeam) const
{
	switch (AI_getAttitude(eTeam))
	{
	case ATTITUDE_FURIOUS:
		return -100;
	case ATTITUDE_ANNOYED:
		return -40;
	case ATTITUDE_CAUTIOUS:
		return -5;
	case ATTITUDE_PLEASED:
		return 50;
	case ATTITUDE_FRIENDLY:
		return 100;
	}

	return 0;
}

int CvTeamAI::AI_getLowestVictoryCountdown() const
{
	PROFILE_EXTRA_FUNC();
	int iBestVictoryCountdown = MAX_INT;
	for (int iVictory = 0; iVictory < GC.getNumVictoryInfos(); iVictory++)
	{
		const int iCountdown = getVictoryCountdown((VictoryTypes)iVictory);
		 if (iCountdown > 0)
		 {
			iBestVictoryCountdown = std::min(iBestVictoryCountdown, iCountdown);
		 }
	}
	if (MAX_INT == iBestVictoryCountdown)
	{
		iBestVictoryCountdown = -1;
	}
	return iBestVictoryCountdown;
}

int CvTeamAI::AI_getTechMonopolyValue(TechTypes eTech, TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	int iValue = 0;

	const bool bWarPlan = getAnyWarPlanCount(eTeam) > 0;

	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		const UnitTypes eLoopUnit = (UnitTypes)iI;

		if (isTechRequiredForUnit((eTech), eLoopUnit))
		{
			if (isWorldUnit(eLoopUnit))
			{
				iValue += 50;
			}
			if (GC.getUnitInfo(eLoopUnit).getPrereqAndTech() == eTech)
			{
				int iNavalValue = 0;

				int iCombatRatio = (GC.getUnitInfo(eLoopUnit).getCombat() * 100) / std::max(1, GC.getGame().getBestLandUnitCombat());
				if (iCombatRatio > 50)
				{
					iValue += ((bWarPlan ? 100 : 50) * (iCombatRatio - 40)) / 50;
				}

				switch (GC.getUnitInfo(eLoopUnit).getDefaultUnitAIType())
				{
				case UNITAI_UNKNOWN:
				case UNITAI_ANIMAL:
				case UNITAI_SETTLE:
				case UNITAI_WORKER:
				case UNITAI_SUBDUED_ANIMAL:
				case UNITAI_HUNTER:
				case UNITAI_HUNTER_ESCORT:
				case UNITAI_HEALER:
				case UNITAI_PROPERTY_CONTROL:
				case UNITAI_HEALER_SEA:
				case UNITAI_PROPERTY_CONTROL_SEA:
				case UNITAI_BARB_CRIMINAL:
				case UNITAI_INVESTIGATOR:
				case UNITAI_SEE_INVISIBLE:
				case UNITAI_SEE_INVISIBLE_SEA:
				case UNITAI_ESCORT:
					break;

				case UNITAI_ATTACK:
				case UNITAI_ATTACK_CITY:
				case UNITAI_ATTACK_CITY_LEMMING:
				case UNITAI_COLLATERAL:
					iValue += bWarPlan ? 50 : 20;
					break;

				case UNITAI_PILLAGE:
				case UNITAI_RESERVE:
				case UNITAI_COUNTER:
				case UNITAI_PARADROP:
				case UNITAI_CITY_DEFENSE:
				case UNITAI_CITY_COUNTER:
				case UNITAI_CITY_SPECIAL:
				case UNITAI_PILLAGE_COUNTER:
				case UNITAI_INFILTRATOR:
					iValue += bWarPlan ? 40 : 15;
					break;


				case UNITAI_EXPLORE:
				case UNITAI_MISSIONARY:
					break;

				case UNITAI_PROPHET:
				case UNITAI_ARTIST:
				case UNITAI_SCIENTIST:
				case UNITAI_GENERAL:
				case UNITAI_GREAT_HUNTER:
				case UNITAI_GREAT_ADMIRAL:
				case UNITAI_MERCHANT:
				case UNITAI_ENGINEER:
					break;

				case UNITAI_SPY:
					break;

				case UNITAI_ICBM:
					iValue += bWarPlan ? 80 : 40;
					break;

				case UNITAI_WORKER_SEA:
					break;

				case UNITAI_ATTACK_SEA:
					iNavalValue += 50;
					break;

				case UNITAI_RESERVE_SEA:
				case UNITAI_ESCORT_SEA:
					iNavalValue += 30;
					break;

				case UNITAI_EXPLORE_SEA:
					iValue += GC.getGame().circumnavigationAvailable() ? 100 : 0;
					break;

				case UNITAI_ASSAULT_SEA:
					iNavalValue += 60;
					break;

				case UNITAI_SETTLER_SEA:
				case UNITAI_MISSIONARY_SEA:
				case UNITAI_SPY_SEA:
					break;

				case UNITAI_CARRIER_SEA:
				case UNITAI_MISSILE_CARRIER_SEA:
					iNavalValue += 40;
					break;

				case UNITAI_PIRATE_SEA:
					iNavalValue += 20;
					break;

				case UNITAI_ATTACK_AIR:
				case UNITAI_DEFENSE_AIR:
					iValue += bWarPlan ? 60 : 30;
					break;

				case UNITAI_CARRIER_AIR:
					iNavalValue += 40;
					break;

				case UNITAI_MISSILE_AIR:
					iValue += bWarPlan ? 40 : 20;
					break;

				default:
					FErrorMsg("error"); // This assert was thrown - audit to see what AI may be missing from the above.
					break;
				}

				if (iNavalValue > 0)
				{
					if (AI_isAnyCapitalAreaAlone())
					{
						iValue += iNavalValue / 2;
					}
					if (bWarPlan && !AI_isLandTarget(eTeam))
					{
						iValue += iNavalValue / 2;
					}
				}
			}
		}
	}
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (isTechRequiredForBuilding(eTech, (BuildingTypes)iI))
		{
			if (GC.getBuildingInfo((BuildingTypes)iI).getReligionType() == NO_RELIGION)
			{
				iValue += 30;
			}
			if (isWorldWonder((BuildingTypes)iI) && !GC.getGame().isBuildingMaxedOut((BuildingTypes)iI))
			{
				iValue += 50;
			}
		}
	}
	for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		if (GC.getProjectInfo((ProjectTypes)iI).getTechPrereq() == eTech)
		{
			if (isWorldProject((ProjectTypes)iI))
			{
				if (!(GC.getGame().isProjectMaxedOut((ProjectTypes)iI)))
				{
					iValue += 100;
				}
			}
			else
			{
				iValue += 50;
			}
		}
	}
	return iValue;
}

bool CvTeamAI::AI_isWaterAreaRelevant(const CvArea* pArea) const
{
	PROFILE_FUNC();

	const CvArea* pBiggestArea = GC.getMap().findBiggestArea(true);
	if (pBiggestArea == pArea)
	{
		return true;
	}
	int iTeamCities = 0;
	int iOtherTeamCities = 0;

	// An area is deemed relevant if it has at least 2 cities of our and different teams.
	// Also count lakes which are connected to ocean by a bridge city
	for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; iPlayer++)
	{
		const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);

		if (iTeamCities < 2 && kPlayer.getTeam() == getID() || iOtherTeamCities < 2 && kPlayer.getTeam() != getID())
		{
			foreach_(const CvCity* pLoopCity, kPlayer.cities())
			{
				if (pLoopCity->plot()->isAdjacentToArea(pArea->getID()))
				{
					if (kPlayer.getTeam() == getID())
					{
						iTeamCities++;

						if (pLoopCity->waterArea() == pBiggestArea)
						{
							return true;
						}
					}
					else
					{
						iOtherTeamCities++;
						if (iOtherTeamCities >= 2)
						{
							break;
						}
					}
				}
			}
		}
		if (iTeamCities >= 2 && iOtherTeamCities >= 2)
		{
			return true;
		}
	}
	return false;
}


DenialTypes CvTeamAI::AI_embassyTrade(TeamTypes eTeam) const
{
	PROFILE_FUNC();

	if (isHuman() || isVassal(eTeam) || AI_shareWar(eTeam))
	{
		return NO_DENIAL;
	}

	if (AI_getWorstEnemy() == eTeam)
	{
		return DENIAL_WORST_ENEMY;
	}
	const AttitudeTypes eAttitude = AI_getAttitude(eTeam);

	if (AI_getMemoryCount(eTeam, MEMORY_RECALLED_AMBASSADOR) > 0 && eAttitude < ATTITUDE_PLEASED)
	{
		return DENIAL_RECENT_CANCEL;
	}

	for (int iI = 0; iI < MAX_PC_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID())
		&& eAttitude <= GC.getLeaderHeadInfo(GET_PLAYER((PlayerTypes)iI).getPersonalityType()).getOpenBordersRefuseAttitudeThreshold())
		{
			return DENIAL_ATTITUDE;
		}
	}
	return NO_DENIAL;
}


DenialTypes CvTeamAI::AI_LimitedBordersTrade(TeamTypes eTeam) const
{
	PROFILE_FUNC();

	return AI_openBordersTrade(eTeam);
}

//Returns the type of denial for trading contacts. eContactTeam is the team who we are trading the contact of
//eTeam is the team we are trading the contact to.
DenialTypes CvTeamAI::AI_contactTrade(TeamTypes eContactTeam, TeamTypes eTeam) const
{

	if (isHuman() || isVassal(eTeam))
	{
		return NO_DENIAL;
	}

	if (AI_getWorstEnemy() == eTeam)
	{
		return DENIAL_WORST_ENEMY;
	}

	if (AI_getWorstEnemy() == eContactTeam)
	{
		return DENIAL_MYSTERY;
	}

	if (GC.getGame().isOption(GAMEOPTION_AI_RUTHLESS))
	{
		//Planning war against the team, no need to complicate matters
		if (AI_getWarPlan(eContactTeam) != NO_WARPLAN)
		{
			const int iRand = GC.getGame().getElapsedGameTurns() % 3;
			switch (iRand)
			{
				case 0: return DENIAL_MYSTERY;
				case 1: return DENIAL_JOKING;
				case 2: return DENIAL_NO_GAIN;
			}
		}
	}
	return NO_DENIAL;
}


int CvTeamAI::AI_embassyTradeVal(TeamTypes eTeam) const
{
	return (getNumCities() + GET_TEAM(eTeam).getNumCities()) * 7/5;
}

int CvTeamAI::AI_LimitedBordersTradeVal(TeamTypes eTeam) const
{
	return (getNumCities() + GET_TEAM(eTeam).getNumCities()) * 2/5;
}


int CvTeamAI::AI_contactTradeVal(TeamTypes eContactTeam, TeamTypes eTeamBuyer) const
{
	PROFILE_EXTRA_FUNC();
	const CvTeam& teamBuyer = GET_TEAM(eTeamBuyer);
	int iValue = 5;
	int iAttitude = 0;
	int iMetCount = 0;
	int iPower = 0;
	int iCountTeams = 1; // Pre-counting eContactTeam

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && iI != eTeamBuyer && iI != eContactTeam)
		{
			iCountTeams++;
			if (teamBuyer.isHasMet((TeamTypes)iI))
			{
				iAttitude += GET_TEAM((TeamTypes)iI).AI_getAttitudeVal(eTeamBuyer);
				iPower += GET_TEAM((TeamTypes)iI).getPower(true);
				iMetCount++;
				if (!GET_TEAM((TeamTypes)iI).isHasMet(eContactTeam))
				{
					iValue += 10;
				}
			}
		}
	}

	if (iMetCount > 0)
	{
		switch (CvPlayerAI::AI_getAttitudeFromValue(iAttitude/iMetCount))
		{
		case ATTITUDE_FURIOUS:
			iValue *= 5;
			iValue /= 2;
			break;
		case ATTITUDE_ANNOYED:
			iValue *= 2;
			break;
		case ATTITUDE_CAUTIOUS:
			iValue *= 3;
			iValue /= 2;
			break;
		case ATTITUDE_PLEASED:
			iValue *= 4;
			iValue /= 5;
			break;
		case ATTITUDE_FRIENDLY:
			iValue *= 2;
			iValue /= 3;
			break;
		default:
			FErrorMsg("Missing attitude case in switch.");
			break;
		}

		if (teamBuyer.getPower(true) > iPower/iMetCount)
		{
			iValue *= 4;
			iValue /= 5;
		}
	}

	if (teamBuyer.isTechTrading())
	{
		iValue *= 3;
		iValue /= 2;
	}

	if (teamBuyer.isGoldTrading())
	{
		iValue *= 3;
		iValue /= 2;
	}

	if (teamBuyer.isOpenBordersTrading())
	{
		iValue *= 4;
		iValue /= 3;
	}

	if (teamBuyer.isMapTrading())
	{
		iValue *= 6;
		iValue /= 5;
	}
	return iValue * 100 / (5*iCountTeams);
}

void CvTeamAI::AI_updateDiploCache()
{
	m_iNoTechTradeThreshold = AI_noTechTradeThreshold(true);
	m_iTechTradeKnownPercent = AI_techTradeKnownPercent(true);
	m_iMaxWarRand = AI_maxWarRand(true);
	m_iMaxWarNearbyPowerRatio = AI_maxWarNearbyPowerRatio(true);
	m_iMaxWarDistantPowerRatio = AI_maxWarDistantPowerRatio(true);
	m_iMaxWarMinAdjacentLandPercent = AI_maxWarMinAdjacentLandPercent(true);
	m_iLimitedWarRand = AI_limitedWarRand(true);
	m_iLimitedWarPowerRatio = AI_limitedWarPowerRatio(true);
	m_iDogpileWarRand = AI_dogpileWarRand(true);
	m_iMakePeaceRand = AI_makePeaceRand(true);
}

int CvTeamAI::AI_FreeTradeAgreementVal(TeamTypes eTeam) const
{
	if (isFreeTrade(eTeam))
	{
		return (getNumCities() + GET_TEAM(eTeam).getNumCities()) / 2;
	}
	return getNumCities() + GET_TEAM(eTeam).getNumCities();
}

DenialTypes CvTeamAI::AI_FreeTradeAgreement(TeamTypes eTeam) const
{
	if (isHuman() || isVassal(eTeam))
	{
		return NO_DENIAL;
	}

	if (AI_getWorstEnemy() == eTeam)
	{
		return DENIAL_WORST_ENEMY;
	}

	if (AI_shareWar(eTeam))
	{
		return NO_DENIAL;
	}

	if (AI_getWarPlan(eTeam) != NO_WARPLAN)
	{
		return DENIAL_MYSTERY;
	}

	if (GET_PLAYER(getLeaderID()).getCapitalCity() != NULL
	&&  GET_PLAYER(GET_TEAM(eTeam).getLeaderID()).getCapitalCity() != NULL
	&& !GET_PLAYER(getLeaderID()).getCapitalCity()->isConnectedTo(GET_PLAYER(GET_TEAM(eTeam).getLeaderID()).getCapitalCity()))
	{
		return DENIAL_JOKING;
	}

	if (AI_getAttitude(eTeam) < ATTITUDE_PLEASED)
	{
		return DENIAL_ATTITUDE;
	}
	return NO_DENIAL;
}

bool CvTeamAI::AI_hasAdjacentLandPlots(TeamTypes eTeam) const
{
	PROFILE_FUNC();
	FAssertMsg(eTeam != getID(), "shouldn't call this function on ourselves");

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		const CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (!pLoopPlot->isWater() && pLoopPlot->getTeam() == eTeam && pLoopPlot->isAdjacentTeam(getID(), true))
		{
			return true;
		}
	}
	return false;
}

#ifdef ENABLE_FOGWAR_DECAY
short CvTeamAI::getVisibilityDecay(bool pSeaPlot)
{
	const TeamTypes& team = GC.getGame().getActiveTeam();
	const PlayerTypes myID = GC.getGame().getActivePlayer();
	const bool bIsHuman = GET_TEAM(team).isHuman() || GC.getGame().getAIAutoPlay(myID) > 0 || gDLL->GetAutorun();
	m_iDefaultDecay = GC.getGame().getModderGameOption(MODDERGAMEOPTION_FOGWAR_NBTURNS);

	if ((pSeaPlot && m_bPermanentMapSea) || !bIsHuman || !GC.getGame().getModderGameOption(MODDERGAMEOPTION_FOGWAR_DECAY)) return NO_DECAY;
	if ((!pSeaPlot && m_bPermanentMapLand) || !bIsHuman || !GC.getGame().getModderGameOption(MODDERGAMEOPTION_FOGWAR_DECAY)) return NO_DECAY;

	if (pSeaPlot)
	{
		int iPathfinding = GC.getInfoTypeForString("TECH_NAVIGATION"); 
		if (isHasTech((TechTypes)iPathfinding))
		{
			m_bPermanentMapSea = true;
			return NO_DECAY;
		}
		iPathfinding = GC.getInfoTypeForString("TECH_COMPASS");
		if (isHasTech((TechTypes)iPathfinding))
		{
			return (m_iDefaultDecay + 40);
		}
		iPathfinding = GC.getInfoTypeForString("TECH_CARTOGRAPHY");
		if (isHasTech((TechTypes)iPathfinding))
		{
			return (m_iDefaultDecay + 30);
		}
		iPathfinding = GC.getInfoTypeForString("TECH_PAPER");
		if (isHasTech((TechTypes)iPathfinding))
		{
			return (m_iDefaultDecay + 20);
		}		
		iPathfinding = GC.getInfoTypeForString("TECH_RUDDER");
		if (isHasTech((TechTypes)iPathfinding))
		{
			return (m_iDefaultDecay + 15);
		}
		iPathfinding = GC.getInfoTypeForString("TECH_SEAFARING");
		if (isHasTech((TechTypes)iPathfinding))
		{
			return (m_iDefaultDecay + 12);
		}
		
		iPathfinding = GC.getInfoTypeForString("TECH_WRITING");
		if (isHasTech((TechTypes)iPathfinding))
		{
			return (m_iDefaultDecay + 9);
		}
		iPathfinding = GC.getInfoTypeForString("TECH_STARGAZING");
		if (isHasTech((TechTypes)iPathfinding))
		{
			return (m_iDefaultDecay + 6);
		}
		iPathfinding = GC.getInfoTypeForString("TECH_SAILING");
		if (isHasTech((TechTypes)iPathfinding))
		{
			return (m_iDefaultDecay + 3);
		}
		return (m_iDefaultDecay + 1);
	}
	else
	{
		int iPathfinding = GC.getInfoTypeForString("TECH_CARTOGRAPHY");
		if (isHasTech((TechTypes)iPathfinding))
		{
			m_bPermanentMapLand = true;
			return NO_DECAY;
		}		
		iPathfinding = GC.getInfoTypeForString("TECH_SURVEYING");
		if (isHasTech((TechTypes)iPathfinding))
		{
			return (m_iDefaultDecay + 40);
		}
		iPathfinding = GC.getInfoTypeForString("TECH_WRITING");
		if (isHasTech((TechTypes)iPathfinding))
		{
			return (m_iDefaultDecay + 25);
		}
		iPathfinding = GC.getInfoTypeForString("TECH_EXPLORATION");
		if (isHasTech((TechTypes)iPathfinding))
		{
			return (m_iDefaultDecay + 18);
		}
		iPathfinding = GC.getInfoTypeForString("TECH_IDEOGRAMS");
		if (isHasTech((TechTypes)iPathfinding))
		{
			return (m_iDefaultDecay + 15);
		}
		iPathfinding = GC.getInfoTypeForString("TECH_PICTOGRAPHS");
		if (isHasTech((TechTypes)iPathfinding))
		{
			return (m_iDefaultDecay + 12);
		}
		iPathfinding = GC.getInfoTypeForString("TECH_HUNTING");
		if (isHasTech((TechTypes)iPathfinding))
		{
			return (m_iDefaultDecay + 9);
		}

		iPathfinding = GC.getInfoTypeForString("TECH_TRACKING");
		if (isHasTech((TechTypes)iPathfinding))
		{
			return (m_iDefaultDecay + 6);
		}
		iPathfinding = GC.getInfoTypeForString("TECH_TRAILS");
		if (isHasTech((TechTypes)iPathfinding))
		{
			return (m_iDefaultDecay + 3);
		}
	}
	return (m_iDefaultDecay + 1);
}
#endif