// cityAI.cpp

#include "CvGameCoreDLL.h"

#include "FProfiler.h"
#include "CvArea.h"
#include "CvBonusInfo.h"
#include "CvBuildingInfo.h"
#include "CvImprovementInfo.h"
#include "CvBonusInfo.h"
#include "CvInfos.h"
#include "CvUnitCombatInfo.h"
#include "CvTraitInfo.h"
#include "CvCityAI.h"
#include <string>
#include "CvContractBroker.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvMap.h"
#include "CvPlayerAI.h"
#include "CvPlot.h"
#include "CvPython.h"
#include "CvReachablePlotSet.h"
#include "CvSelectionGroup.h"
#include "CvTeamAI.h"
#include "CvUnit.h"
#include "CvUnitSelectionCriteria.h"
#include "CvDLLFAStarIFaceBase.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLUtilityIFaceBase.h"
#include "OutputRatios.h"
#include "PlotInfo.h"
#include "CvValueService.h"
#include "CvWorkerService.h"


//	KOSHLING MOD - calculate all possible building focuses at once
//	to avoid multiple looping - need to know how many options there
//	are.  Note that I am also ASSUMING only one flag is passed at a
//	time (and runtime checking that)
#define BUILDINGFOUCSINDEX_ANY					0
#define BUILDINGFOCUSINDEX_FOOD					1
#define BUILDINGFOCUSINDEX_PRODUCTION			2
#define BUILDINGFOCUSINDEX_GOLD					3
#define BUILDINGFOCUSINDEX_RESEARCH				4
#define BUILDINGFOCUSINDEX_CULTURE				5
#define BUILDINGFOCUSINDEX_DEFENSE				6
#define BUILDINGFOCUSINDEX_HAPPY				7
#define BUILDINGFOCUSINDEX_HEALTHY				8
#define BUILDINGFOCUSINDEX_EXPERIENCE			9
#define BUILDINGFOCUSINDEX_MAINTENANCE			10
#define BUILDINGFOCUSINDEX_SPECIALIST			11
#define BUILDINGFOCUSINDEX_ESPIONAGE			12
#define BUILDINGFOCUSINDEX_BIGCULTURE			13
#define BUILDINGFOCUSINDEX_WORLDWONDER			14
#define BUILDINGFOCUSINDEX_DOMAINSEA			15
#define BUILDINGFOCUSINDEX_WONDEROK				16
#define BUILDINGFOCUSINDEX_CAPITAL				17

#define NUM_REAL_BUILDINGFOCUS_FLAGS			18
//	Pseudo flags which represent combinations used in the calculation
//	and are not independent so have to be accumulated separately
#define BUILDINGFOCUSINDEX_GOLDANDRESEARCH		18
#define BUILDINGFOCUSINDEX_GOLDANDMAINTENANCE	19

#define NUM_ALL_BUILDINGFOCUS_FLAGS				20


CvCityAI::CvCityAI()
{
	m_aiEmphasizeYieldCount = new int[NUM_YIELD_TYPES];
	m_aiEmphasizeCommerceCount = new int[NUM_COMMERCE_TYPES];
	m_bForceEmphasizeCulture = false;
	m_aiSpecialYieldMultiplier = new int[NUM_YIELD_TYPES];
	m_aiPlayerCloseness = new int[MAX_PLAYERS];

	m_pbEmphasize = NULL;
	m_pbEmphasizeSpecialist = NULL;
	cachedBuildingValues = NULL;

	AI_reset();
}


CvCityAI::~CvCityAI()
{
	AI_uninit();

	SAFE_DELETE_ARRAY(m_aiEmphasizeYieldCount);
	SAFE_DELETE_ARRAY(m_aiEmphasizeCommerceCount);
	SAFE_DELETE_ARRAY(m_aiSpecialYieldMultiplier);
	SAFE_DELETE_ARRAY(m_aiPlayerCloseness);
}


void CvCityAI::AI_init()
{
	AI_reset();

	//--------------------------------
	// Init other game data
	AI_assignWorkingPlots();
	AI_updateBestBuild();
	AI_updateWorkersNeededHere();
}


void CvCityAI::AI_uninit()
{
	AI_FlushBuildingValueCache();

	SAFE_DELETE_ARRAY(m_pbEmphasize);
	SAFE_DELETE_ARRAY(m_pbEmphasizeSpecialist);
}


// FUNCTION: AI_reset()
// Initializes data members that are serialized.
void CvCityAI::AI_reset()
{
	PROFILE_EXTRA_FUNC();
	AI_uninit();

	m_iEmphasizeAvoidGrowthCount = 0;
	m_iEmphasizeGreatPeopleCount = 0;
	m_iUnitSummonedLastEstablishedTurn = 0;
	m_bForceEmphasizeCulture = false;

	m_iEmphasizeAvoidAngryCitizensCount = 0;
	m_iEmphasizeAvoidUnhealthyCitizensCount = 0;
	m_bMilitaryProductionCity = false;
	m_bNavalMilitaryProductionCity = false;

	m_bAssignWorkDirty = false;
	m_bChooseProductionDirty = false;

	m_routeToCity.reset();

	m_bestBuildValuesStale = true;

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiEmphasizeYieldCount[iI] = 0;
	}

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		m_aiEmphasizeCommerceCount[iI] = 0;
	}

	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		m_aiBestBuildValue[iI] = NO_BUILD;
	}

	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		m_aeBestBuild[iI] = NO_BUILD;
	}

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiSpecialYieldMultiplier[iI] = 0;
	}
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aiPlayerCloseness[iI] = 0;
	}
	m_iCachePlayerClosenessTurn = -1;
	m_iCachePlayerClosenessDistance = -1;

	m_iNeededFloatingDefenders = -1;
	m_iNeededFloatingDefendersCacheTurn = -1;
	m_neededDefenders = -1;

	m_iWorkersNeeded = 0;

	m_iBuildPriority = CITY_BUILD_PRIORITY_CEILING;
	m_iRequestedUnit = 0;
	m_iRequestedBuilding = 0;

	FAssertMsg(m_pbEmphasize == NULL, "m_pbEmphasize not NULL!!!");
	FAssertMsg(GC.getNumEmphasizeInfos() > 0, "GC.getNumEmphasizeInfos() is not greater than zero but an array is being allocated in CvCityAI::AI_reset");
	m_pbEmphasize = new bool[GC.getNumEmphasizeInfos()];
	for (int iI = 0; iI < GC.getNumEmphasizeInfos(); iI++)
	{
		m_pbEmphasize[iI] = false;
	}

	FAssertMsg(m_pbEmphasizeSpecialist == NULL, "m_pbEmphasizeSpecialist not NULL!!!");
	FAssertMsg(GC.getNumSpecialistInfos() > 0, "GC.getNumSpecialistInfos() is not greater than zero but an array is being allocated in CvCityAI::AI_reset");
	m_pbEmphasizeSpecialist = new bool[GC.getNumSpecialistInfos()];
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		m_pbEmphasizeSpecialist[iI] = false;
	}

#ifdef YIELD_VALUE_CACHING
	ClearYieldValueCacheImpl();
#endif

	m_iCityThreat = -1;
}


void CvCityAI::SendLog(CvWString function, CvWString message) const
{
	//WIP, wrapper of the new FLB logger, to create correct payload for this class
	CvWString aiType = "CvCityAI";


	logAIJson(aiType, this->getName(), function, message);

}

//	KOSHLING - initialisation called on every city prior to performing unit mission allocation logic
//	This allows caches that will remain valid for the processing of the current turn's units to be cleared
void CvCityAI::AI_preUnitTurn()
{
	PROFILE_FUNC();

	m_neededDefenders = -1;

	m_iCityThreat = -1;

	m_bestUnits.clear();
}

void CvCityAI::AI_trained(UnitTypes eUnitType, UnitAITypes eUnitAIType)
{
}

void CvCityAI::AI_doTurn()
{
	PROFILE_FUNC();

	m_buildValueCache.clear();

	if (!isHuman())
	{
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			setForceSpecialistCount(((SpecialistTypes)iI), 0);
		}
		AI_stealPlots();
	}
	AI_updateBestBuild();
	AI_updateWorkersNeededHere();

	m_routeToCityUpdated = false; // Update on demand

	if (isHuman())
	{
		/* Toffer - Disabled governors ability to rush.
		if (isProductionAutomated())
		{
			AI_doHurry();
		}
		*/
		return;
	}
	AI_doPanic();

	if (!isNPC())
	{
		AI_doDraft();
		AI_doHurry();
		AI_doEmphasize();
		AI_doContractFloatingDefenders();
	}
}


void CvCityAI::AI_doContractFloatingDefenders()
{
	PROFILE_FUNC();

	LOG_CITY_BLOCK(2, {
		int iOurThreat = AI_cityThreat();
		int iLargestThreat = 0;
		int iTotalThreat = std::max(1, GET_PLAYER(getOwner()).AI_getTotalAreaCityThreat(area(), &iLargestThreat));
		logBBAI("City %S has threat level %d (highest %d, total %d)",
				getName().GetCString(), iOurThreat, iLargestThreat, iTotalThreat);
	});

	//	If below optimal garrison size request extra troops
	//	to move to us with a priority based on our threat level
	//	The highest priority we assign to this is FLOATING_DEFENDER_REQUEST_PRIORITY_MAX
	//	which is below the lowest building priority, and will thus soak up existing
	//	troops without local work without causing new production
	if (!AI_isDefended(0, false))
	{
		//	Assess our priority
		int iGarrisonStrength = getGarrisonStrength(false);
		int iMinStrength = AI_minDefenseStrength();
		int iOurPriority;
		int iRequiredStrength;

		//	If we don't even have minimum strength pull in a few defenders at higher priority
		// TB note: !isCapital() added so as to give the AI a way to ignore the need for a while as it develops its first city more efficiently at first.
		if (iGarrisonStrength < iMinStrength && !isCapital())
		{
			iOurPriority = CITY_BUILD_PRIORITY_CEILING;
			iRequiredStrength = iMinStrength - iGarrisonStrength;
		}
		else if (iGarrisonStrength < iMinStrength && isCapital())
		{
			iOurPriority = MINIMUM_CAPITAL_DEFENDER_PRIORITY;
			iRequiredStrength = iMinStrength - iGarrisonStrength;
		}
		else
		{
			int iOurThreat = AI_cityThreat();
			int iLargestThreat = 0;
			GET_PLAYER(getOwner()).AI_getTotalAreaCityThreat(area(), &iLargestThreat);

			iOurPriority = (MINIMUM_CITY_DEFENDER_PRIORITY * iOurThreat) / std::max(1, iLargestThreat);
			iRequiredStrength = AI_neededDefenseStrength() - iGarrisonStrength;
		}

		FAssert(iRequiredStrength > 0);

		LOG_BBAI_CITY(2, ("      City %S requests %d floating defender strength at priority %d", getName().GetCString(), iRequiredStrength, iOurPriority));

		UnitAITypes eFloatingDefenderUnitAI = NO_UNITAI;
		if (iGarrisonStrength < iMinStrength)
		{
			eFloatingDefenderUnitAI = UNITAI_CITY_DEFENSE;
		}
		GET_PLAYER(getOwner()).getContractBroker().advertiseWork(iOurPriority,
			DEFENSIVE_UNITCAPABILITIES,
			getX(),
			getY(),
			NULL,
			eFloatingDefenderUnitAI,
			iRequiredStrength);
	}
	else
	{
		LOG_BBAI_CITY(2, ("      City %S adequately defended", getName().GetCString()));
	}
}


void CvCityAI::AI_assignWorkingPlots()
{
	PROFILE_FUNC();

	if (0 != GC.getAI_SHOULDNT_MANAGE_PLOT_ASSIGNMENT())
	{
		return;
	}

	//update the special yield multiplier to be current
	AI_updateSpecialYieldMultiplier();

	//	If we're in disorder no need to go through all of this, but we must call updateCommerce()
	//	directly to have the disorder take effect on the commerce of this city
	if (isDisorder())
	{
		setCommerceDirty();
	}
	else
	{
		// remove any plots we can no longer work for any reason
		verifyWorkingPlots();

		// make sure at least the forced amount of specialists are assigned
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			const int iForcedSpecialistCount = getForceSpecialistCount((SpecialistTypes)iI);

			if (!isHuman() || isCitizensAutomated() || getSpecialistCount((SpecialistTypes)iI) < iForcedSpecialistCount)
			{
				setSpecialistCount(((SpecialistTypes)iI), iForcedSpecialistCount);
			}
		}

		// if we have more specialists of any type than this city can have, reduce to the max
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			if (!isSpecialistValid((SpecialistTypes)iI))
			{
				const int iMaxSpecialist = getMaxSpecialistCount((SpecialistTypes)iI);
				if (iMaxSpecialist > 0 && getSpecialistCount((SpecialistTypes)iI) > iMaxSpecialist)
				{
					setSpecialistCount(((SpecialistTypes)iI), iMaxSpecialist);
				}
			}
		}

		// always work the home plot (center)
		CvPlot* pHomePlot = getCityIndexPlot(CITY_HOME_PLOT);
		if (pHomePlot != NULL)
		{
			setWorkingPlot(CITY_HOME_PLOT, getPopulation() > 0 && canWork(pHomePlot));
		}

		// keep removing the worst citizen until we are not over the limit
		while (extraPopulation() < 0)
		{
			if (!AI_removeWorstCitizen())
			{
				FErrorMsg("error");
				break;
			}
		}

		// extraSpecialists() is less than extraPopulation()
		FASSERT_NOT_NEGATIVE(extraSpecialists());

		// do we have population unassigned
		while (extraPopulation() > 0)
		{
			// (AI_addBestCitizen now handles forced specialist logic)
			if (!AI_addBestCitizen(/*bWorkers*/ true, /*bSpecialists*/ true))
			{
				break;
			}
		}

		// if we still have population to assign, assign specialists
		while (extraSpecialists() > 0)
		{
			if (!AI_addBestCitizen(/*bWorkers*/ false, /*bSpecialists*/ true))
			{
				break;
			}
		}

		// if automated, look for better choices than the current ones
		if (!isHuman() || isCitizensAutomated())
		{
			AI_juggleCitizens();
		}

		// at this point, we should not be over the limit
		FAssert((getWorkingPopulation() + getSpecialistPopulation()) <= (totalFreeSpecialists() + getPopulation()));
	}

	AI_setAssignWorkDirty(false);

	if (isCitySelected())
	{
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(CitizenButtons_DIRTY_BIT, true);
	}
}


void CvCityAI::AI_updateAssignWork()
{
	if (AI_isAssignWorkDirty())
	{
		//	Don't mess with plot assignments while the user is in the city screen as
		//	current assignments might be transitory
		if (!GC.getGame().isNetworkMultiPlayer() && isHuman() && gDLL->getInterfaceIFace()->isCityScreenUp())
		{
			//	Also assume when they leave the city screen they will leave a state
			//	they are happy with
			AI_setAssignWorkDirty(false);
			return;
		}

		AI_assignWorkingPlots();
	}
}


bool CvCityAI::AI_avoidGrowth()
{
	PROFILE_FUNC();

	if (isFoodProduction())
	{
		return true;
	}

	if (isHuman() && !isProductionAutomated())
	{
		return false;
	}

	if (AI_isEmphasizeAvoidGrowth())
	{
		return true;
	}

	// AI should avoid growth when it has angry citizens, even if emphasizing great people
	if (!isHuman() || !AI_isEmphasizeYield(YIELD_FOOD) && !AI_isEmphasizeGreatPeople())
	{
		int iExtra = (isHuman()) ? 0 : 1;
		int iHappinessLevel = happyLevel() - unhappyLevel(iExtra);

		// ignore military unhappy, since we assume it will be fixed quickly, grow anyway
		if (getMilitaryHappinessUnits() == 0)
		{
			iHappinessLevel += ((GC.getNO_MILITARY_PERCENT_ANGER() * (getPopulation() + 1)) / GC.getPERCENT_ANGER_DIVISOR());
		}

		if (GC.getGame().isOption(GAMEOPTION_UNSUPPORTED_REVOLUTION))
		{
			iHappinessLevel -= std::min(getRevolutionIndex() / 600, 2);
		}
		else if (GET_PLAYER(getOwner()).canPopRush())
		{// if we can pop rush, we want to grow one over the cap
			iHappinessLevel++;
		}

		// if we have angry citizens
		if (iHappinessLevel < 0)
		{
			return true;
		}
	}

	if (AI_isEmphasizeAvoidAngryCitizens())
	{
		if (unhappyLevel() > happyLevel())
		{
			return true;
		}

		if (getFoodTurnsLeft() == 1)
		{
			const int iHappyFacesLeft = happyLevel() - unhappyLevel(getHappinessTimer() > 0 ? GC.getTEMP_HAPPY() : 0);

			if (iHappyFacesLeft < 1)
			{
				return true;
			}
		}
	}

	if (AI_isEmphasizeAvoidUnhealthyCitizens())
	{
		if (badHealth() > goodHealth())
		{
			return true;
		}

		if (getFoodTurnsLeft() == 1)
		{
			const int iHealthyFacesLeft = goodHealth() - badHealth();

			if (iHealthyFacesLeft < 1)
			{
				return true;
			}
		}
	}
	return false;
}


bool CvCityAI::AI_ignoreGrowth()
{
	PROFILE_FUNC();
	return !AI_isEmphasizeYield(YIELD_FOOD) && !AI_isEmphasizeGreatPeople() && !AI_foodAvailable(!isHuman());
}


int CvCityAI::AI_specialistValue(SpecialistTypes eSpecialist, bool bAvoidGrowth, bool bRemove) const
{
	PROFILE_FUNC();

	short aiYields[NUM_YIELD_TYPES];
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		aiYields[iI] = GET_PLAYER(getOwner()).specialistYield(eSpecialist, ((YieldTypes)iI));
	}

	short int aiCommerceYields[NUM_COMMERCE_TYPES];
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		aiCommerceYields[iI] = GET_PLAYER(getOwner()).specialistCommerce(eSpecialist, ((CommerceTypes)iI));
	}

	int iValue = AI_yieldValue(aiYields, aiCommerceYields, bAvoidGrowth, bRemove);

	int iGreatPeopleRate = GC.getSpecialistInfo(eSpecialist).getGreatPeopleRateChange();
	if (iGreatPeopleRate != 0)
	{
		int iEmphasisCount = 0;
		int iGPPValue = 4;
		if (AI_isEmphasizeGreatPeople())
		{
			iGPPValue = isHuman() ? 30 : 20;
		}
		else
		{
			if (AI_isEmphasizeYield(YIELD_COMMERCE))
			{
				iGPPValue = 2;
				iEmphasisCount++;
			}
			if (AI_isEmphasizeYield(YIELD_PRODUCTION))
			{
				iGPPValue = 1;
				iEmphasisCount++;
			}
			if (AI_isEmphasizeYield(YIELD_FOOD))
			{
				iGPPValue = 1;
				iEmphasisCount++;
			}
		}

		// Scale up value for civs/civics with bonuses
		iGreatPeopleRate *= (100 + GET_PLAYER(getOwner()).getGreatPeopleRateModifier());
		iGreatPeopleRate /= 100;

		int iTempValue = (iGreatPeopleRate * iGPPValue);

		if (!isHuman() || AI_isEmphasizeGreatPeople())
		{
			int iProgress = getGreatPeopleProgress();
			if (iProgress > 0)
			{
				int iThreshold = GET_PLAYER(getOwner()).greatPeopleThresholdNonMilitary();
				iTempValue += ((iGreatPeopleRate * (isHuman() ? 1 : 4) * iGPPValue * iProgress * iProgress) / (iThreshold * iThreshold));
			}
		}

		int iCurrentEra = GET_PLAYER(getOwner()).getCurrentEra();
		int iTotalEras = GC.getNumEraInfos();

		if (GET_PLAYER(getOwner()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2))
		{
			const UnitTypes eGreatPeopleUnit = (UnitTypes)GC.getSpecialistInfo(eSpecialist).getGreatPeopleUnitType();
			if (eGreatPeopleUnit != NO_UNIT)
			{
				const CvUnitInfo& kUnitInfo = GC.getUnitInfo(eGreatPeopleUnit);
				if (kUnitInfo.getGreatWorkCulture() > 0)
				{
					iTempValue += kUnitInfo.getGreatWorkCulture() / ((GET_PLAYER(getOwner()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3)) ? 200 : 350);
				}
			}
		}

		if (!isHuman() && (iCurrentEra <= ((iTotalEras * 2) / 3)))
		{
			// try to spawn a prophet for any shrines we have yet to build
			bool bNeedProphet = false;
			int iBestSpreadValue = 0;

			for (int iJ = 0; iJ < GC.getNumReligionInfos(); iJ++)
			{
				const ReligionTypes eReligion = (ReligionTypes)iJ;

				if (isHolyCity(eReligion) && !hasShrine(eReligion)
					&& ((iCurrentEra < iTotalEras / 2) || GC.getGame().countReligionLevels(eReligion) >= 10))
				{
					const UnitTypes eGreatPeopleUnit = (UnitTypes)GC.getSpecialistInfo(eSpecialist).getGreatPeopleUnitType();
					if (eGreatPeopleUnit != NO_UNIT)
					{
						// note, for normal XML, this count will be one (there is only 1 shrine building for each religion)
						foreach_(const BuildingTypes eBuilding, GC.getReligionInfo(eReligion).getShrineBuildings())
						{
							// if this unit builds or forceBuilds this building
							if (GC.getUnitInfo(eGreatPeopleUnit).getHasBuilding(eBuilding))
							{
								bNeedProphet = true;
								iBestSpreadValue = std::max(iBestSpreadValue, GC.getGame().countReligionLevels(eReligion));
							}
						}
					}
				}
			}

			if (bNeedProphet)
			{
				iTempValue += (iGreatPeopleRate * iBestSpreadValue);
			}
		}

		// Scale up value for civs/civics with bonuses
		iTempValue *= getTotalGreatPeopleRateModifier();
		iTempValue /= GET_PLAYER(getOwner()).AI_averageGreatPeopleMultiplier();

		iTempValue /= (1 + iEmphasisCount);
		iValue += iTempValue;
	}
	else
	{
		const SpecialistTypes eGenericCitizen = (SpecialistTypes)GC.getDEFAULT_SPECIALIST();

		// are we the generic specialist?
		if (eSpecialist == eGenericCitizen)
		{
			iValue *= 60;
			iValue /= 100;
		}
	}

	int iExperience = (GC.getSpecialistInfo(eSpecialist).getExperience() * 2);
	for (int iI = 0; iI < GC.getSpecialistInfo(eSpecialist).getNumUnitCombatExperienceTypes(); iI++)
	{
		if (GC.getSpecialistInfo(eSpecialist).getUnitCombatExperienceType(iI).eUnitCombat != NO_UNITCOMBAT)
		{
			iExperience += (GC.getSpecialistInfo(eSpecialist).getUnitCombatExperienceType(iI).iModifier);
			if (isProductionUnit() && isProductionUnitCombat((int)GC.getSpecialistInfo(eSpecialist).getUnitCombatExperienceType(iI).eUnitCombat))
			{
				iExperience += (2 * (GC.getSpecialistInfo(eSpecialist).getUnitCombatExperienceType(iI).iModifier));
			}
		}
	}
	if (0 != iExperience)
	{
		const int iProductionRank = findYieldRateRank(YIELD_PRODUCTION);
		const bool bMetAnyCiv = GET_TEAM(getTeam()).hasMetAnyCiv();

		iValue += iExperience * (bMetAnyCiv ? 4 : 2);
		if (iProductionRank <= GET_PLAYER(getOwner()).getNumCities() / 2 + 1)
		{
			iValue += iExperience * 4;
		}
		iValue += ((getMilitaryProductionModifier() * iExperience * 10) / 100);
	}

	int iSpecialistHealth = GC.getSpecialistInfo(eSpecialist).getHealthPercent() / 100;
	int iSpecialistHappiness = GC.getSpecialistInfo(eSpecialist).getHappinessPercent() / 100;
	int iHappinessLevel = happyLevel() - unhappyLevel(1) + getEspionageHappinessCounter();
	int iHealthLevel = goodHealth() - badHealth(/*bNoAngry*/ false, std::max(0, (iHappinessLevel + 1) / 2)) + getEspionageHealthCounter();

	int iBaseFoodDifference = getYieldRate(YIELD_FOOD) - getFoodConsumedByPopulation() - std::max(0, -iHealthLevel);

	if (iSpecialistHealth != 0)
	{
		iValue += (healthValue(iSpecialistHealth, iHappinessLevel - (getEspionageHappinessCounter() / 2) + std::max(0, iSpecialistHappiness), iHealthLevel, iBaseFoodDifference) * 2);
	}

	if (iSpecialistHappiness != 0)
	{
		iValue += (happynessValue(iSpecialistHappiness, iHappinessLevel, iHealthLevel) * 2);
	}

	//	Koshling - evaluate properties
	int iPropertyValue = 0;

	foreach_(const CvPropertySource * pSource, GC.getSpecialistInfo(eSpecialist).getPropertyManipulators()->getSources())
	{
		//	Sources that deliver to the city or the plot are both considered since the city plot diffuses
		//	to the city for most properties anyway
		if (pSource->getType() == PROPERTYSOURCE_CONSTANT &&
			(pSource->getObjectType() == GAMEOBJECT_CITY || pSource->getObjectType() == GAMEOBJECT_PLOT))
		{
			const PropertyTypes eProperty = pSource->getProperty();
			int iCurrentSourceSize = getTotalBuildingSourcedProperty(eProperty) + getTotalUnitSourcedProperty(eProperty) + getPropertyNonBuildingSource(eProperty);
			int iNewSourceSize = iCurrentSourceSize + static_cast<const CvPropertySourceConstant*>(pSource)->getAmountPerTurn(getGameObject());
			int iDecayPercent = getPropertyDecay(eProperty);

			//	Steady state occurs at a level where the decay removes as much per turn as the sources add
			//	Decay can be 0 if the current level is below the threshold at which decay cuts in, so for the
			//	purposes of calculation just treat this as very slow decay
			int	iCurrentSteadyStateLevel = (100 * iCurrentSourceSize) / std::max(1, iDecayPercent);
			int	iNewSteadyStateLevel = (100 * iNewSourceSize) / std::max(1, iDecayPercent);

			iPropertyValue += getPropertySourceValue(eProperty, iNewSteadyStateLevel - iCurrentSteadyStateLevel);
		}
	}

	iValue += iPropertyValue;

	//TB Insidiousness/Investigation
	int iNumCriminals = plot()->getNumCriminals();
	int iMultiplier = (iNumCriminals * iNumCriminals);
	int iInvestigate = GC.getSpecialistInfo(eSpecialist).getInvestigation();
	int iInsidious = GC.getSpecialistInfo(eSpecialist).getInsidiousness();
	iValue += iInvestigate + (iMultiplier * iInvestigate);
	iValue -= (iInsidious + (iMultiplier * iInsidious));
	//TB DEBUG test
	//Lets see how city assignments go if we half this value - thinking that it may be comparing to the workable plots
	iValue /= 2;

	return AI_isEmphasizeSpecialist(eSpecialist) ? (iValue * 175) : (iValue * 100);
}

void CvCityAI::AI_chooseProduction()
{
	PROFILE_FUNC();
	//#0 START OF AI_CHOOSEPRODUCTION
	m_iRequestedBuilding = 0;
	m_iRequestedUnit = 0;

	const PlayerTypes eOwner = getOwner();
	CvPlayerAI& player = GET_PLAYER(eOwner);

	if (player.isAnarchy())
	{
		return;
	}

	//#0 : Establish all needed Values that will be used for multple prio/conditions
	const bool bDanger = AI_isDanger();
	const int iDangerValue = GET_PLAYER(getOwner()).AI_getPlotDanger(plot(), 2, false);
	const bool bWasFoodProduction = isFoodProduction();
	const bool bFinancialTrouble = player.AI_isFinancialTrouble();
	const bool bCriticalGold = player.AI_hasCriticalGold();
	const int iHammerCostPercent = GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent();

	//# 0 : If their is a alredy a production in City, Conditions to keep them (
	// 1. nearly done buildings
	// 2. Wonders
	// 3. Settlers (if not in financial troubles)
	if (isProduction())
	{
		if (getProductionProgress() > 0)
		{
			// If nearly done, keep building current item (Turns equal/less than: 21=Eternity, 3=Normal, 2=Blitz)
			if (getProductionTurnsLeft() <= (bDanger && getProductionUnit() == NO_UNIT ? 1 + GC.getGame().getGameSpeedType() / 4 : 1 + iHammerCostPercent / 50))
			{
				return;
			}

			if (!bDanger)
			{
				// if we are building a wonder, do not cancel, keep building it
				const BuildingTypes eProductionBuilding = getProductionBuilding();
				if (eProductionBuilding != NO_BUILDING && isLimitedWonder(eProductionBuilding))
				{
					return;
				}

				if ((!bFinancialTrouble || getProductionUnitAI() != UNITAI_SETTLE)
					//if we are killing our growth to train this, then finish it.
					&& bWasFoodProduction
					// If we are on the defensive we may have to reassess our build choice further
					&& area()->getAreaAIType(getTeam()) != AREAAI_DEFENSIVE)
				{
					return;
				}
			}

			// if building a combat unit, and we have nearly no defenders, keep building it
			const UnitTypes eProductionUnit = getProductionUnit();
			if (eProductionUnit != NO_UNIT && plot()->getNumDefenders(eOwner) < 2
				&& GC.getUnitInfo(eProductionUnit).getCombat() > 0)
			{
				return;
			}
		}
		clearOrderQueue();
	}

	m_iBuildPriority = CITY_BUILD_PRIORITY_CEILING;
	m_iTempBuildPriority = CITY_BUILD_PRIORITY_CEILING;

	// only clear the dirty bit if we actually do a check, multiple items might be queued
	AI_setChooseProductionDirty(false);

	bool bInhibitUnits = false;
	if (isHuman() && isProductionAutomated())
	{
		if (!player.isOption(PLAYEROPTION_MODDER_3))
		{
			AI_buildGovernorChooseProduction();
			return;
		}
		bInhibitUnits = AI_isAnyCommerceOrYieldEmphasis();
	}

	const CvArea* pArea = area();
	CvArea* pWaterArea = waterArea(true);
	bool bMaybeWaterArea = false;
	bool bWaterDanger = false;

	if (pWaterArea != NULL)
	{
		bMaybeWaterArea = true;
		if (!GET_TEAM(getTeam()).AI_isWaterAreaRelevant(pWaterArea))
		{
			pWaterArea = NULL;
		}
		bWaterDanger = player.AI_getWaterDanger(plot(), 4) > 0;
	}

	const AreaAITypes eAreaAI = pArea->getAreaAIType(getTeam());

	const bool bMassing = eAreaAI == AREAAI_ASSAULT_MASSING;
	const bool bDefenseWar = eAreaAI == AREAAI_DEFENSIVE;
	const bool bAssaultAssist = eAreaAI == AREAAI_ASSAULT_ASSIST;

	bool bLandWar = bMassing || eAreaAI == AREAAI_OFFENSIVE || eAreaAI == AREAAI_DEFENSIVE;
	bool bAssault = bMassing || bAssaultAssist || eAreaAI == AREAAI_ASSAULT;

	const bool bStrategyTurtle = player.AI_isDoStrategy(AI_STRATEGY_TURTLE);
	int iWarSuccessRatio = GET_TEAM(getTeam()).AI_getWarSuccessCapitulationRatio();
	int iEnemyPowerPerc = GET_TEAM(getTeam()).AI_getEnemyPowerPercent(true);
	int iWarTroubleThreshold = 0;

	if (bLandWar && iWarSuccessRatio < 30)
	{
		iWarTroubleThreshold = std::max(3, (-iWarSuccessRatio / 8));
	}

	if (!bLandWar && !bAssault && GET_TEAM(getTeam()).isAVassal())
	{
		bLandWar = GET_TEAM(getTeam()).isMasterPlanningLandWar(pArea);

		if (!bLandWar)
		{
			bAssault = GET_TEAM(getTeam()).isMasterPlanningSeaWar(pArea);
		}
	}

	bool bGetBetterUnits = player.AI_isDoStrategy(AI_STRATEGY_GET_BETTER_UNITS);
	bool bAggressiveAI = GC.getGame().isOption(GAMEOPTION_AI_AGGRESSIVE);
	bool bIsWarMonger = GC.getLeaderHeadInfo(player.getPersonalityType()).getConquestVictoryWeight() > 35;
	bool bIsPeacefull = GC.getLeaderHeadInfo(player.getPersonalityType()).getConquestVictoryWeight() <= 20;

	int iMaxPropertyUnitsPercent = 20;
	if (bIsPeacefull)
	{
		iMaxPropertyUnitsPercent += 15;
		if (player.isMinorCiv()) //If minor Civ, At war automatically, but not on purpose, so some changes are needed)
		{
			bLandWar = false;
			bAssault = false;

		}
	}
	int iUnitCostPercentage = static_cast<int>(player.getFinalUnitUpkeep() * 100 / std::max<int64_t>(1, player.calculatePreInflatedCosts()));
	int iWaterPercent = AI_calculateWaterWorldPercent();

	int iBuildUnitProb = AI_buildUnitProb();

	int iWorkersInArea = player.AI_totalAreaUnitAIs(pArea, UNITAI_WORKER);
	int iNeededWorkersInArea = player.AI_neededWorkers(pArea);
	// Sea worker need independent of whether water area is militarily relevant
	int iNeededSeaWorkers = (bMaybeWaterArea) ? AI_neededSeaWorkers() : 0;
	const int iWorkersNeeded = AI_getWorkersNeeded() - getNumWorkers();
	int iExistingSeaWorkers = waterArea(true) ? player.AI_totalWaterAreaUnitAIs(waterArea(true), UNITAI_WORKER_SEA) : 0;

	const int iSpreadUnitThreshold = (
			1000 + (bLandWar ? 800 - 10 * iWarSuccessRatio : 0)
			+ 300 * plot()->plotCount(PUF_isUnitAIType, UNITAI_MISSIONARY, -1, NULL, eOwner)
			);

	UnitTypes eBestSpreadUnit = NO_UNIT;
	int iBestSpreadUnitValue = -1;

	int iAreaBestFoundValue;
	int iNumAreaCitySites = player.AI_getNumAreaCitySites(getArea(), iAreaBestFoundValue);

	int iWaterAreaBestFoundValue = 0;
	CvArea* pWaterSettlerArea = pWaterArea;
	if (pWaterSettlerArea == NULL)
	{
		pWaterSettlerArea = GC.getMap().findBiggestArea(true);

		if (pWaterSettlerArea != NULL && player.AI_totalWaterAreaUnitAIs(pWaterSettlerArea, UNITAI_SETTLER_SEA) == 0)
		{
			pWaterSettlerArea = NULL;
		}
	}
	int iNumWaterAreaCitySites = (pWaterSettlerArea == NULL) ? 0 : player.AI_getNumAdjacentAreaCitySites(pWaterSettlerArea->getID(), getArea(), iWaterAreaBestFoundValue);
	int iNumSettlers = player.AI_totalUnitAIs(UNITAI_SETTLE) + player.getContractBroker().numRequestsOutstanding(UNITAI_SETTLE);

	bool bIsCapitalArea = false;
	int iNumCapitalAreaCities = 0;
	if (player.getCapitalCity() != NULL)
	{
		iNumCapitalAreaCities = player.getCapitalCity()->area()->getCitiesPerPlayer(eOwner);
		if (getArea() == player.getCapitalCity()->getArea())
		{
			bIsCapitalArea = true;
		}
	}

	int iMaxSettlers = 0;

	if (!bFinancialTrouble)
	{
		CvUnitSelectionCriteria	criteria;
		criteria.m_bIgnoreGrowth = true;

		if (player.bestBuildableUnitForAIType(NO_DOMAIN, UNITAI_SETTLE, &criteria) != NO_UNIT)
		{
			iMaxSettlers = std::min((player.getNumCities() + 1) / 2, iNumAreaCitySites + iNumWaterAreaCitySites);

			if (bLandWar || bAssault)
			{
				iMaxSettlers = (iMaxSettlers + 2) / 3;
			}

			if (player.getCityLimit() > 0)
			{
				if (player.getCityOverLimitUnhappy() < 1)
				{
					//	Hard limit - don't stockpile more than 2 settlers
					iMaxSettlers = std::min(iMaxSettlers, player.getCityLimit() - player.getNumCities() + 2);
				}
				else if (player.AI_getOverallHappyness() < 0)
				{
					// Soft limit - If we already have unhappy cities don't create settlers that
					//	will increase overall unhappiness as they found new cities.
					iMaxSettlers = std::min(iMaxSettlers, std::max(0, player.getCityLimit() - player.getNumCities()));
				}
			}
		}
	}
	int iEconomyFlags = 0;
	int iEconomyFlagBits = 0;

	if (!isHuman() || AI_isEmphasizeYield(YIELD_PRODUCTION))
	{
		iEconomyFlags |= BUILDINGFOCUS_PRODUCTION;
		iEconomyFlagBits++;
	}
	if (!isHuman() || AI_isEmphasizeYield(YIELD_COMMERCE))
	{
		iEconomyFlags |= BUILDINGFOCUS_GOLD;
		iEconomyFlagBits++;
	}
	if (!isHuman() || AI_isEmphasizeCommerce(COMMERCE_RESEARCH))
	{
		iEconomyFlags |= BUILDINGFOCUS_RESEARCH;
		iEconomyFlagBits++;
	}
	if (!bInhibitUnits)	//	This is actually a proxy for the human owner having set no explicit preferences
	{
		iEconomyFlags |= BUILDINGFOCUS_MAINTENANCE;
		iEconomyFlags |= BUILDINGFOCUS_HAPPY;
		iEconomyFlags |= BUILDINGFOCUS_HEALTHY;

		iEconomyFlagBits += 3;
	}
	if (AI_isEmphasizeGreatPeople())
	{
		iEconomyFlags |= BUILDINGFOCUS_SPECIALIST;
		iEconomyFlagBits++;
	}
	iEconomyFlags |= BUILDINGFOCUS_ESPIONAGE;
	iEconomyFlagBits++;

	//	Normalize threholds using this against the number of bits we are including
	const int iEcononmyFlagsThreasholdWeighting = (100 * iEconomyFlagBits) / 8;

	// Does this city require special attention as a cultural victory city?
	bool bImportantCity = false; //be very careful about setting this.
	const int iNumCitiesInArea = pArea->getCitiesPerPlayer(eOwner);
	const int iCultureRateRank = findCommerceRateRank(COMMERCE_CULTURE);
	const int iCulturalVictoryNumCultureCities = GC.getGame().culturalVictoryNumCultureCities();

	if (iNumCitiesInArea > 2 && player.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2)
		&& iCultureRateRank <= iCulturalVictoryNumCultureCities + 1
		// if we do not have enough cities, then the highest culture city will not get special attention
		&& (iCultureRateRank > 1 || player.getNumCities() > iCulturalVictoryNumCultureCities + 1))
	{
		if (iNumAreaCitySites + iNumWaterAreaCitySites > 0 && player.getNumCities() < 6 && GC.getGame().getSorenRandNum(2, "AI Less Culture More Expand") == 0)
		{
			bImportantCity = false;
		}
		else bImportantCity = true;
	}

	const EraTypes eCurrentEra = player.getCurrentEra();
	const int iCulturePerTurn = getCommerceRate(COMMERCE_CULTURE);
	const bool bPrimaryArea = player.AI_isPrimaryArea(pArea);

	const int iTargetCulturePerTurn = AI_calculateTargetCulturePerTurn();

	// So what's the right detection of defense which works in early game too?
	// Fuyu: The right way is to 1) queue the warriors with UNITAI_CITY_DEFENCE or UNITAI_RESERVE,
	// then 2) to detect defenders with plot()->plotCount(PUF_canDefendGroupHead, -1, -1, eOwner, NO_TEAM, PUF_isCityAIType) - compare AI_isDefended()
	const int iPlotSettlerCount = (iNumSettlers == 0) ? 0 : plot()->plotCount(PUF_isUnitAIType, UNITAI_SETTLE, -1, NULL, eOwner);
	const int iPlotCityDefenderCount = plot()->plotCount(PUF_isUnitAIType, UNITAI_CITY_DEFENSE, -1, NULL, eOwner, NO_TEAM, NULL, -1, -1, 2);
	const int iPlotOtherCityAICount = plot()->plotCount(PUF_canDefend, -1, -1, NULL, eOwner, NO_TEAM, PUF_isCityAIType, -1, -1, 2);

	const int iPlotSettlerEscortCityDefenseCount = plot()->plotCount(PUF_isUnitAIType, UNITAI_CITY_COUNTER, -1, NULL, eOwner) - (AI_minDefenders() + 1);
	const int iPlotSettlerEscortCounterCount = plot()->plotCount(PUF_isUnitAIType, UNITAI_CITY_COUNTER, -1, NULL, eOwner);
	const int iPlotSettlerEscortCount = iPlotSettlerEscortCityDefenseCount + iPlotSettlerEscortCounterCount;

	bool bChooseWorker = false;

	int iPlotCityDefenderStrength = getGarrisonStrength();
	int iPlotOtherCityAIStrength = plot()->plotStrength(UNITVALUE_FLAGS_DEFENSIVE, PUF_canDefend, -1, -1, eOwner, NO_TEAM, PUF_isCityAIType, -1, -1, 2) - iPlotCityDefenderStrength;

	bool bCrushStrategy = player.AI_isDoStrategy(AI_STRATEGY_CRUSH);
	int iNeededFloatingDefenders = (isNPC() || bCrushStrategy) ? 0 : player.AI_getTotalFloatingDefendersNeeded(pArea);
	int iTotalFloatingDefenders = (isNPC() ? 0 : player.AI_getTotalFloatingDefenders(pArea));

	int iNbMinimalAttackers = 3;
	if (bIsPeacefull) iNbMinimalAttackers -= 2;
	if (bIsWarMonger) iNbMinimalAttackers += 5;

	//TB Note: min 1 hunter goes under the priority level of settling initiation because this is exploitable with ambushers (or just plain bad luck for the hunters which is not unlikely).  Destroy all hunters and you cripple growth.
	//Koshling - made having at least 1 hunter a much higher priority
	const int iNeededHunters = player.AI_neededHunters(pArea);
	const int iOwnedHunters = player.AI_totalAreaUnitAIs(pArea, UNITAI_HUNTER);
	const int iHunterDeficitPercent = (iNeededHunters <= iOwnedHunters) ? 0 : (iNeededHunters - iOwnedHunters) * 100 / iNeededHunters;

	const int iLowlimitProductionRank = (player.getNumCities() + 1) * 2 / 3;

	// Free experience for various unit domains
	const int iFreeLandExperience = getSpecialistFreeExperience() + getDomainFreeExperience(DOMAIN_LAND);
	// AIAndy: Not used here at the moment
	//const int iFreeSeaExperience = getSpecialistFreeExperience() + getDomainFreeExperience(DOMAIN_SEA);
	const int iFreeAirExperience = getSpecialistFreeExperience() + getDomainFreeExperience(DOMAIN_AIR);

	const int iOwnedSeeInvisibles = player.AI_totalAreaUnitAIs(pArea, UNITAI_SEE_INVISIBLE);
	const int iMaxSeeInvisibleUnits = 2 + player.getNumCities() + intSqrt(player.getNumCities() * intSqrt(eCurrentEra));

	//	Koshling - protect against broken AI counts, which is known to happen occasionally
	int iNumSpies = std::max(0, player.AI_totalAreaUnitAIs(pArea, UNITAI_SPY));
	int iNeededSpies = iNumCitiesInArea / 3;
	iNeededSpies += isCapital() ? 1 : 0;
	// K-Mod
	if (player.AI_isDoStrategy(AI_STRATEGY_BIG_ESPIONAGE))
	{
		iNeededSpies *= 2;
	}

	int iMaxUnitSpending = AI_evaluateMaxUnitSpending(); 
	
	int iCarriers = player.AI_totalUnitAIs(UNITAI_CARRIER_SEA);

	//Afforess reduced 12 -> 6, since AI rarely reaches this logic, Added exemption for MAD players
	const int NukeProbRNG = !GC.getGame().isOption(GAMEOPTION_AI_RUTHLESS) ? 6 : 3;
	const int iNukesWanted = 1 + 2 * std::min(player.getNumCities(), GC.getGame().getNumCities() - player.getNumCities());

	//Afforess moved AI Aircraft logic up, Aircraft >> Sea transports
	UnitTypeWeightArray airUnitTypes;

	int iAircraftNeed = 0;
	int iAircraftHave = 0;
	UnitTypes eBestAttackAircraft = NO_UNIT;
	UnitTypes eBestMissile = NO_UNIT;


	int iMinFoundValue = player.AI_getMinFoundValue();
	if (iDangerValue > 3)
	{
		iMinFoundValue *= 3;
		iMinFoundValue /= 2;
	}

	int iHealth = goodHealth() - badHealth(true, 0);
	int iFoodDiffBase = foodDifference(false, false, true);

	/// END OF #0 VARIABLES PREPARATION

	clearOrderQueue();

	if (bWasFoodProduction)
	{
		AI_assignWorkingPlots();
	}

	int iProductionRank = findYieldRateRank(YIELD_PRODUCTION);
		
	LOG_CITY_BLOCK(3, {
		logBBAI(
			"\nPlayer %d (%S) - %S (pop %d):\n"
			"	Considering new production: iProdRank %d, iBuildUnitProb %d\n"
			"	Area workers %d (need %d), workers assigned to city %d (need %d more)\n"
			"	iNumSettlers=%d; iMaxSettlers=%d\n",
			eOwner, player.getCivilizationDescription(0), getName().GetCString(), getPopulation(),
			iProductionRank, iBuildUnitProb,
			iWorkersInArea, iNeededWorkersInArea, getNumWorkers(), iWorkersNeeded,
			iNumSettlers, iMaxSettlers
		);
	});

	// --- Situation de base ---
	LOG_BBAI_CITY(3, (
		"City %s base state: Danger=%d (val=%d), FoodProd=%d, FinTrouble=%d, HammerCostPct=%d",
		getName().GetCString(),
		bDanger, iDangerValue, bWasFoodProduction, bFinancialTrouble, iHammerCostPercent
		));

	// --- Production courante ---
	LOG_BBAI_CITY(3, (
		"City %s production: InProd=%d, Prog=%d, TurnsLeft=%d, ProdBuilding=%d, ProdUnit=%d",
		getName().GetCString(),
		isProduction(),
		getProductionProgress(),
		getProductionTurnsLeft(),
		getProductionBuilding(),
		getProductionUnit()
		));

	// --- Militaire / Guerre ---
	LOG_BBAI_CITY(3, (
		"City %s war state: AreaAI=%d (Massing=%d, DefenseWar=%d, AssaultAssist=%d), "
		"LandWar=%d, Assault=%d, StrategyTurtle=%d, WarSuccess=%d, EnemyPowerPerc=%d, "
		"WarTroubleThresh=%d",
		getName().GetCString(),
		eAreaAI, bMassing, bDefenseWar, bAssaultAssist,
		bLandWar, bAssault, bStrategyTurtle,
		iWarSuccessRatio, iEnemyPowerPerc, iWarTroubleThreshold
		));

	// --- Workers / Settlers ---
	LOG_BBAI_CITY(3, (
		"City %s workers/settlers: WorkersInArea=%d, NeededWorkersInArea=%d, "
		"NeededSeaWorkers=%d, WorkersNeeded=%d, ExistingSeaWorkers=%d, "
		"NumAreaCitySites=%d, NumWaterAreaCitySites=%d, NumSettlers=%d, MaxSettlers=%d",
		getName().GetCString(),
		iWorkersInArea, iNeededWorkersInArea,
		iNeededSeaWorkers, iWorkersNeeded, iExistingSeaWorkers,
		iNumAreaCitySites, iNumWaterAreaCitySites, iNumSettlers, iMaxSettlers
		));

	// --- Economie / Culture / Special ---
	LOG_BBAI_CITY(3, (
		"City %s econ/culture: EconFlags=%08lx (bits=%d, threshold=%d), "
		"ImportantCity=%d, CultureRank=%d/%d, CulturePerTurn=%d, TargetCulture=%d, "
		"Era=%d, PrimaryArea=%d",
		getName().GetCString(),
		(unsigned long)iEconomyFlags, iEconomyFlagBits, iEcononmyFlagsThreasholdWeighting,
		bImportantCity, iCultureRateRank, iCulturalVictoryNumCultureCities,
		iCulturePerTurn, iTargetCulturePerTurn,
		eCurrentEra, bPrimaryArea
		));
	// -------------------- BBAI Notes -------------------------
	// #SPEC 1 Barbarian city build priorities
	if (isHominid())
	{
		if (!AI_isDefended(0, true))
		{
			if (AI_chooseDefender("barbarian defenders"))
			{
				return;
			}

			if (AI_chooseUnit("barbarian lack of defense", UNITAI_ATTACK))
			{
				return;
			}
		}

		iBuildUnitProb += iFreeLandExperience;

		bool bRepelColonists = false;
		if (pArea->getNumCities() > pArea->getCitiesPerPlayer(BARBARIAN_PLAYER) + 2)
		{
			if (pArea->getCitiesPerPlayer(BARBARIAN_PLAYER) > pArea->getNumCities() / 3)
			{
				// New world scenario with invading colonists ... fight back!
				bRepelColonists = true;
				iBuildUnitProb += 5 * (pArea->getNumCities() - pArea->getCitiesPerPlayer(BARBARIAN_PLAYER));
			}
		}
		if (iBuildUnitProb > 99) iBuildUnitProb = 99;

		if (!bDanger && GC.getGame().getSorenRandNum(100, "NPC build building") >= iBuildUnitProb)
		{
			int iBarbarianFlags = BUILDINGFOCUS_PRODUCTION | BUILDINGFOCUS_EXPERIENCE;
			if (getPopulation() < 4) iBarbarianFlags |= BUILDINGFOCUS_FOOD;
			if (getPopulation() > 3) iBarbarianFlags |= BUILDINGFOCUS_DEFENSE;

			if (AI_chooseBuilding(iBarbarianFlags, 10))
			{
				LOG_BBAI_CITY(2, ("      City %S uses barb AI_chooseBuilding with flags and iBuildUnitProb = %d", getName().GetCString(), iBuildUnitProb));
				return;
			}

			if (GC.getGame().getSorenRandNum(128, "NPC build building 2") > iBuildUnitProb && AI_chooseBuilding())
			{
				LOG_BBAI_CITY(2, ("      City %S uses barb AI_chooseBuilding without flags and iBuildUnitProb = %d", getName().GetCString(), iBuildUnitProb));
				return;
			}
		}

		if (eAreaAI == AREAAI_OFFENSIVE || eAreaAI == AREAAI_ASSAULT || plot()->plotCount(PUF_isUnitAIType, UNITAI_ASSAULT_SEA, -1, NULL, eOwner) > 0)
		{
			if (AI_chooseUnit("barbarian choose attack city", UNITAI_ATTACK_CITY))
			{
				return;
			}
		}

		if (!bDanger && (pWaterArea != NULL) && (iWaterPercent > 30))
		{
			if (GC.getGame().getSorenRandNum(3, "AI Coast Raiders!") == 0)
			{
				if (player.AI_totalUnitAIs(UNITAI_ASSAULT_SEA) <= player.getNumCities() * 2)
				{
					if (AI_chooseUnit("barbarian transports", UNITAI_ASSAULT_SEA))
					{
						return;
					}
				}
			}
			if (GC.getGame().getSorenRandNum(110, "AI arrrr!") < (iWaterPercent + 10))
			{
				if (player.AI_totalUnitAIs(UNITAI_PIRATE_SEA) <= player.getNumCities() * 2)
				{
					if (AI_chooseUnit("barbarian pirates", UNITAI_PIRATE_SEA))
					{
						return;
					}
				}

				if (player.AI_totalAreaUnitAIs(pWaterArea, UNITAI_ATTACK_SEA) < iNumCitiesInArea * 3)
				{
					if (AI_chooseUnit("barbarian sea attack", UNITAI_ATTACK_SEA))
					{
						return;
					}
				}
			}
		}

		if (!bDanger && iWorkersInArea < iNeededWorkersInArea && iWorkersNeeded > 0)
		{
			if (AI_chooseUnit("barbarian worker for established city", UNITAI_WORKER))
			{
				return;
			}
		}
		if (!bDanger && !bWaterDanger && iNeededSeaWorkers > 0)
		{
			if (AI_chooseUnit("barbarian navy", UNITAI_WORKER_SEA))
			{
				return;
			}
		}

		{
			UnitTypeWeightArray barbarianTypes;
			barbarianTypes.push_back(std::make_pair(UNITAI_ATTACK, 125));
			barbarianTypes.push_back(std::make_pair(UNITAI_ATTACK_CITY, (bRepelColonists ? 200 : 100)));
			barbarianTypes.push_back(std::make_pair(UNITAI_PILLAGE, 60));
			barbarianTypes.push_back(std::make_pair(UNITAI_COUNTER, 40));
			barbarianTypes.push_back(std::make_pair(UNITAI_CITY_DEFENSE, 10));

			if (AI_chooseLeastRepresentedUnit("barbarian units", barbarianTypes))
			{
				return;
			}
		}
		// As last resort choose any unit we can build
		AI_chooseUnit("barbarian last resort", NO_UNITAI);
		return;
	}

	// #SPEC 2 if Player is Rebel (Revolution?, no more needed ?)
	if (player.isRebel())
	{
		UnitTypeWeightArray rebelDefenseTypes;
		rebelDefenseTypes.push_back(std::make_pair(UNITAI_CITY_DEFENSE, 100));
		rebelDefenseTypes.push_back(std::make_pair(UNITAI_COUNTER, 100));
		rebelDefenseTypes.push_back(std::make_pair(UNITAI_COLLATERAL, 100));
		rebelDefenseTypes.push_back(std::make_pair(UNITAI_ATTACK, 100));

		UnitTypeWeightArray rebelOffenseTypes;
		rebelOffenseTypes.push_back(std::make_pair(UNITAI_ATTACK, 125));
		rebelOffenseTypes.push_back(std::make_pair(UNITAI_COUNTER, 100));
		rebelOffenseTypes.push_back(std::make_pair(UNITAI_ATTACK_CITY, 75));

		if (isOccupation())
		{
			// Probably just captured ... may be getting defensive units,
			// but need to build offense to keep rolling
			if (bLandWar || bDanger)
			{
				if (AI_chooseLeastRepresentedUnit("rebel offense", rebelOffenseTypes))
				{
					return;
				}
			}
		}
		else
		{
			// City Defense
			if (plot()->plotCount(PUF_isUnitAIType, UNITAI_CITY_DEFENSE, -1, NULL, eOwner) < (AI_minDefenders()))
			{
				if (AI_chooseUnit("rebel city defense", UNITAI_CITY_DEFENSE))
				{
					return;
				}

				if (AI_chooseDefender("rebel city defense (non-specific)"))
				{
					return;
				}
			}

			// Area defense
			if (
				player.AI_getTotalFloatingDefenders(pArea) <
				(player.AI_getTotalFloatingDefendersNeeded(pArea) + 1) / 2

			&&	AI_chooseLeastRepresentedUnit("rebel defense", rebelDefenseTypes))
			{
				return;
			}

			// Offensive rebel units
			if (bDanger || GC.getGame().getSorenRandNum(100, "AI Build Unit Production") < AI_buildUnitProb())
			{
				if (getYieldRate(YIELD_PRODUCTION) > 5 * getPopulation())
				{
					// Air units
					int iBestDefenseValue = player.AI_bestCityUnitAIValue(UNITAI_DEFENSE_AIR, this);

					if (iBestDefenseValue > 0)
					{
						UnitTypes eBestAttackAircraft = NO_UNIT;
						//int iBestAirValue = player.AI_bestCityUnitAIValue(UNITAI_ATTACK_AIR, this, &eBestAttackAircraft);

						int iAircraftHave = player.AI_getNumAIUnits(UNITAI_ATTACK_AIR) + player.AI_getNumAIUnits(UNITAI_DEFENSE_AIR) + player.AI_getNumAIUnits(UNITAI_MISSILE_AIR);
						int iAircraftNeed = (2 + player.getNumCities() * (3 * GC.getUnitInfo(eBestAttackAircraft).getAirCombat())) / (2 * std::max(1, GC.getGame().getBestLandUnitCombat()));

						UnitTypeWeightArray airUnitTypes;
						airUnitTypes.push_back(std::make_pair(UNITAI_ATTACK_AIR, 60));
						airUnitTypes.push_back(std::make_pair(UNITAI_DEFENSE_AIR, 100));

						if ((iAircraftHave * 2 < iAircraftNeed) && (GC.getGame().getSorenRandNum(2, "AI train escort sea") == 0))
						{
							if (AI_chooseLeastRepresentedUnit("rebel air units", airUnitTypes))
							{
								return;
							}
						}
					}
				}

				if (bLandWar || bDanger)
				{
					if (AI_chooseLeastRepresentedUnit("rebel offense units", rebelOffenseTypes))
					{
						return;
					}
				}

				if (bAssault)
				{
					if (pWaterArea != NULL)
					{
						UnitTypes eBestAssaultUnit = NO_UNIT;
						player.AI_bestCityUnitAIValue(UNITAI_ASSAULT_SEA, this, &eBestAssaultUnit);
						int iBestSeaAssaultCapacity = 0;
						if (eBestAssaultUnit != NO_UNIT)
						{
							iBestSeaAssaultCapacity = GC.getUnitInfo(eBestAssaultUnit).getCargoSpace();
						}

						if (iBestSeaAssaultCapacity > 0)
						{
							int iUnitsToTransport = player.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK_CITY);
							iUnitsToTransport += player.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK);
							iUnitsToTransport += player.AI_totalAreaUnitAIs(pArea, UNITAI_COUNTER);

							int iLocalTransports = player.AI_totalAreaUnitAIs(pArea, UNITAI_ASSAULT_SEA);
							int iTransportsAtSea = player.AI_totalAreaUnitAIs(pWaterArea, UNITAI_ASSAULT_SEA);
							int iTransports = iLocalTransports + iTransportsAtSea;

							int iEscorts = player.AI_totalAreaUnitAIs(pArea, UNITAI_ESCORT_SEA);
							iEscorts += player.AI_totalAreaUnitAIs(pWaterArea, UNITAI_ESCORT_SEA);

							// Escorts
							if ((iEscorts < ((1 + 2 * iTransports) / 3)) && (GC.getGame().getSorenRandNum(2, "AI train escort sea") == 0))
							{
								if (AI_chooseBuilding(BUILDINGFOCUS_DOMAINSEA))
								{
									return;
								}

								if (AI_chooseUnit("rebel sea escorts", UNITAI_ESCORT_SEA))
								{
									return;
								}
							}

							// Transports
							if (iUnitsToTransport > (iTransports * iBestSeaAssaultCapacity))
							{
								if (AI_chooseBuilding(BUILDINGFOCUS_DOMAINSEA))
								{
									return;
								}

								if (AI_chooseUnit("rebel sea assault", UNITAI_ASSAULT_SEA))
								{
									return;
								}
							}

							// Attack troops
							if (iUnitsToTransport < ((iLocalTransports + (bPrimaryArea ? iTransportsAtSea / 2 : 0)) * iBestSeaAssaultCapacity))
							{
								if (AI_chooseLeastRepresentedUnit("rebel offensive units", rebelOffenseTypes))
								{
									return;
								}
							}
						}
					}
				}
			}
		}

		// Buildings important for rebels
		if (iCulturePerTurn == 0 && AI_chooseBuilding(BUILDINGFOCUS_CULTURE, 20))
		{
			return;
		}

		if (getPopulation() < 2 * (eCurrentEra + 1) && AI_chooseBuilding(BUILDINGFOCUS_FOOD))
		{
			return;
		}

		// Happiness?  Health?

		int iRebelFlags = 0;
		iRebelFlags |= BUILDINGFOCUS_CULTURE;
		iRebelFlags |= BUILDINGFOCUS_PRODUCTION;
		iRebelFlags |= BUILDINGFOCUS_EXPERIENCE;
		iRebelFlags |= BUILDINGFOCUS_DEFENSE;

		if (AI_chooseBuilding(iRebelFlags))
		{
			return;
		}

		//essential economic builds
		if (AI_chooseBuilding(iEconomyFlags, 10, iEcononmyFlagsThreasholdWeighting / 4))
		{
			return;
		}

		// If nothing special to build, continue to regular logic
	}

	// #SPEC 3  ?)
	if (isOccupation())
	{
		// try picking forge, etc, any duration
		if (AI_chooseBuilding(BUILDINGFOCUS_PRODUCTION))
		{
			return;
		}

		// pick granary or lighthouse, any duration
		if (AI_chooseBuilding(BUILDINGFOCUS_FOOD))
		{
			return;
		}

		// just pick any building, any duration
		if (AI_chooseBuilding())
		{
			return;
		}
	}

	//#1 - UNITAI_CITY_DEFENSE for Defendless city
	if (!bInhibitUnits && plot()->getNumDefenders(eOwner) == 0) // XXX check for other team's units?
	{
		static CvUnitSelectionCriteria DefaultCriteria;
		if (AI_chooseUnitImmediate("defenseless city", UNITAI_CITY_DEFENSE, &DefaultCriteria))
		{
			return;
		}

		if (player.getNumCities() <= 3)
		{
			CvUnitSelectionCriteria criteria;

			criteria.m_bIgnoreNotUnitAIs = true;
			if (AI_chooseUnitImmediate("defenseless city any unit", UNITAI_CITY_DEFENSE, &criteria))
			{
				return;
			}
		}

		if (AI_chooseUnit("defenseless city counter units", UNITAI_CITY_COUNTER, -1, -1, CITY_BUILD_PRIORITY_CEILING, &DefaultCriteria))
		{
			return;
		}

		if (AI_chooseUnit("defenseless city special units", UNITAI_CITY_SPECIAL, -1, -1, CITY_BUILD_PRIORITY_CEILING, &DefaultCriteria))
		{
			return;
		}

		if (AI_chooseUnit("defenseless city reserve units", UNITAI_RESERVE, -1, -1, CITY_BUILD_PRIORITY_CEILING, &DefaultCriteria))
		{
			return;
		}

		if (player.getNumCities() > 3 && iNumCitiesInArea <= 3)
		{
			CvUnitSelectionCriteria criteria;

			criteria.m_bIgnoreNotUnitAIs = true;
			if (AI_chooseUnit("defenseless city any unit (low city count area)", UNITAI_CITY_DEFENSE, -1, -1, HIGHEST_PRIORITY_ESCORT_PRIORITY, &criteria))
			{
				return;
			}
		}

		if (AI_chooseUnit("defenseless city (accept attack units)", UNITAI_ATTACK, -1, -1, CITY_BUILD_PRIORITY_CEILING, &DefaultCriteria))
		{
			return;
		}
	}

	//#2a Very Critical Income situation
	if (bCriticalGold)
	{
		//Apply Process Income as soon as it's possible
		int iPathfinding = GC.getInfoTypeForString("TECH_COOPERATION");
		if ((GET_TEAM(getTeam()).isHasTech((TechTypes)iPathfinding)))
		{
			bInhibitUnits = true;
			m_iRequestedBuilding = 0;
			if (AI_chooseProcess(COMMERCE_GOLD, NULL, true))
			{
				m_iRequestedBuilding = 10;
				return;
			}
			m_iRequestedUnit = 10;
		}
	}

	//#2b - If Strike, Appropriate Building
	if (player.isStrike())
	{
		// pick granary or lighthouse, any duration
		int iStrikeFlags = 0;
		iStrikeFlags |= BUILDINGFOCUS_GOLD;
		iStrikeFlags |= BUILDINGFOCUS_MAINTENANCE;

		if (AI_chooseBuilding(iStrikeFlags))
		{
			return;
		}

		// try picking forge, etc, any duration
		if (AI_chooseBuilding(BUILDINGFOCUS_PRODUCTION))
		{
			return;
		}

		// just pick any building, any duration
		if (AI_chooseBuilding())
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	//#3 Specific Rules for First Era
	if (eCurrentEra == 0)
	{
		// Warriors are blocked from UNITAI_CITY_DEFENSE, in early game this confuses AI city building
		if (player.AI_totalUnitAIs(UNITAI_CITY_DEFENSE) <= player.getNumCities() + iNumSettlers)
		{
			if (player.AI_bestCityUnitAIValue(UNITAI_CITY_DEFENSE, this) == 0)
			{
				iPlotCityDefenderStrength += iPlotOtherCityAIStrength;
				iPlotOtherCityAIStrength = 0;
				if (iPlotCityDefenderStrength == 0)
				{
					iPlotCityDefenderStrength = plot()->plotStrength(UNITVALUE_FLAGS_DEFENSIVE, PUF_canDefend, -1, -1, eOwner, NO_TEAM, PUF_isDomainType, DOMAIN_LAND, -1, 2);
				}
			}
		}
	}



	m_iTempBuildPriority--;

	//#4 Really easy production trumps everything (Less than 5 turns (instead of only 2)
	int iMaxTurnsUsefull = 2;
	iMaxTurnsUsefull *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent();
	iMaxTurnsUsefull /= 100;

	if (AI_chooseBuilding(BUILDINGFOCUS_PRODUCTION, iMaxTurnsUsefull))
	{
		LOG_BBAI_CITY(2, ("#4 City %S, Finishing Building...", getName().GetCString()));
		m_iRequestedUnit = 10; //Prevent Unit buildings
		return;
	}
	// Emergency happyness
	const int iHappyness = happyLevel() - unhappyLevel(0);

	//#5 If more than 7 unhappy citizens or more than a sixth of the population  => BUILDING for Happyness
	if (iHappyness < -7 || iHappyness < -getPopulation() / 6)
	{
		LOG_BBAI_CITY(2, ("#5 City %S, Dealed with Happiness Prio1", getName().GetCString()));
		if (AI_chooseBuilding(BUILDINGFOCUS_HAPPY, 20, 0, -1, true))
		{
			m_iRequestedUnit = 10; //Prevent Unit buildings
			return;
		}

		// Can we build military happyness units?
		if (player.getHappyPerMilitaryUnit() > 0
			// Only do so up to 4 over what we might normally consider
			&& plot()->plotCount(PUF_canDefend) < (AI_minDefenders() + iPlotSettlerCount + 4)
			&& AI_chooseUnit("emergency military happyness", UNITAI_CITY_DEFENSE))
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	//#6 Emergency property control (bad property > 20% through op range - choose unit to deal if possible)
	if (AI_choosePropertyControlBuildingAndUnit(20, NO_PROPERTY, iMaxPropertyUnitsPercent))
	{
		LOG_BBAI_CITY(2, ("#6 City %S, Dealed with Property Control Prio1 (20%) ", getName().GetCString()));
		return;
	}

	m_iTempBuildPriority--;

	//#7 Emergency Hunters
	if (!bInhibitUnits && iOwnedHunters < 1 && iHunterDeficitPercent > 80)
	{
		if (AI_chooseExperienceBuilding(UNITAI_HUNTER, 2))
		{
			m_iRequestedUnit = 10; //Prevent Unit buildings
			return;
		}
		LOG_BBAI_CITY(2, ("#7 City %S, Will Start to build hunters (iHunterDeficitPercent : %d). For the moment : Owned : %d, Needed : %d", getName().GetCString(), iHunterDeficitPercent, iOwnedHunters, iNeededHunters));
		if (AI_chooseUnit("no hunters at all", UNITAI_HUNTER))
		{
			return;
		}
	}

	//#8 Workers Prio1 - when no workers at all or no danger
	if (!bInhibitUnits && !(bDefenseWar && iWarSuccessRatio < -50))
	{
		if (iWorkersInArea == 0 || (iWorkersInArea <= 3 && iDangerValue < 3)) // Not a single worker on my landmass or low danger.
		{
			if (iNeededWorkersInArea > iWorkersInArea && iProductionRank <= iLowlimitProductionRank)
			{
				LOG_BBAI_CITY(2, ("#8 City %S, Will Start to build workers. For the moment : InArea : %d, Needed : %d", getName().GetCString(), iWorkersInArea, iNeededWorkersInArea));
				if (AI_chooseUnit("no workers", UNITAI_WORKER, -1, -1, CITY_NO_WORKERS_WORKER_PRIORITY))
				{
					return;
				}
				bChooseWorker = true;
			}

			if (!bWaterDanger && iNeededSeaWorkers > iExistingSeaWorkers && getPopulation() < 3)
			{
				if (AI_chooseUnit("no sea workers", UNITAI_WORKER_SEA, -1, -1, CITY_NO_WORKERS_WORKER_PRIORITY))
				{
					return;
				}
			}

			if (!bChooseWorker && iWorkersNeeded > 1 && getPopulation() > 1)
			{
				if (AI_chooseUnit("secondary worker", UNITAI_WORKER))
				{
					return;
				}
				bChooseWorker = true;
			}
		}
	}
	m_iTempBuildPriority--;

	//#9 Non-emergency, but still urgent happyness
	if (!bCriticalGold && iHappyness < 0)
	{
		if (AI_chooseBuilding(BUILDINGFOCUS_HAPPY, 8, 0, -1, true))
		{
			return;
		}

		// Can we build military happyness units?
		if (player.getHappyPerMilitaryUnit() > 0
		// Only do so up to 2 over what we might normally consider
		&& plot()->plotCount(PUF_canDefend) < AI_minDefenders() + iPlotSettlerCount + 2
		&& AI_chooseUnit("military happyness", UNITAI_CITY_DEFENSE))
		{
			return;
		}
	}
	m_iTempBuildPriority--;

	//#10 Minimal escort defense for Settlers 	//TB Note: Moved escorts and settlers to almost the top priority IF it's possible and needed.  This should solve the complaints about settlers not being trained fast enough.	
	if (iMaxSettlers > 0 && !bInhibitUnits && iPlotSettlerCount > 0 && iPlotSettlerEscortCount < (iPlotSettlerCount * 4))
	{
		LOG_BBAI_CITY(2, ("City %S, Will Start to build Min escort defenders. For the moment : InArea : %d, Needed : %d", getName().GetCString(), iPlotSettlerEscortCount, iPlotSettlerCount * 4));
		if (AI_chooseUnit("min defender", UNITAI_CITY_DEFENSE))
		{
			return;
		}

		if (AI_chooseUnit("min defender", UNITAI_CITY_COUNTER))
		{
			return;
		}

		if (AI_chooseUnit("min defender", UNITAI_CITY_DEFENSE))
		{
			return;
		}

		if (AI_chooseUnit("min defender", UNITAI_CITY_COUNTER))
		{
			return;
		}

		if (AI_chooseUnit("min defender", UNITAI_CITY_SPECIAL))
		{
			return;
		}

		if (AI_chooseUnit("min defender", UNITAI_RESERVE))
		{
			return;
		}

		if (AI_chooseUnit("min defender", UNITAI_ATTACK))
		{
			return;
		}

		//Fuyu bIgnoreNotUnitAIs
		if (iNumCitiesInArea <= 3)
		{
			if (AI_chooseUnit("min defender (any unit)", UNITAI_CITY_DEFENSE, -1, 1))
			{
				return;
			}
		}
	}

	m_iTempBuildPriority--;

	//#11 Gold Buildings in case of FinancialTrouble
	if (bFinancialTrouble && AI_chooseBuilding(BUILDINGFOCUS_GOLD, 10))
	{
		LOG_BBAI_CITY(2,("      City %S uses financial difficulty resolution", getName().GetCString()));
		m_iRequestedUnit = 10; //Prevent Unit buildings
		return;
	}


	m_iTempBuildPriority--;

	//#12 Workers and Sea Workers, Specifi for capital city
	if (!bInhibitUnits && iDangerValue < 4 && !bStrategyTurtle && isCapital())
	{
		if (!bWaterDanger && iNeededSeaWorkers > 0 && iExistingSeaWorkers == 0)
		{
			// Build workboat first since it doesn't stop growth
			if (AI_chooseUnit("capital with no sea workers", UNITAI_WORKER_SEA))
			{
				return;
			}
		}

		if (!bChooseWorker && iWorkersInArea <= iNeededWorkersInArea
		&& (iWorkersNeeded > 0 || iNeededWorkersInArea > iWorkersInArea * 4/5)
		// Fuyu: anything bigger than 0 is ok
		&& AI_totalBestBuildValue(pArea) > 0)
		{
			if (AI_chooseUnit("capital with no workers", UNITAI_WORKER))
			{
				return;
			}
			bChooseWorker = true;
		}
		// Sea Workers
		else if (!bWaterDanger && getPopulation() <= 4 && iNeededSeaWorkers > 0
		&& happyLevel() - unhappyLevel(1) > 0 && iExistingSeaWorkers == 0
		&& AI_chooseUnit("capital worker", UNITAI_WORKER_SEA))
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	//#13 Workers if financial troubles, Extra Quick Settler Escort otherwise
	if (iMaxSettlers > 0 && !bInhibitUnits && !(bDefenseWar && iWarSuccessRatio < -50))
	{
		if (iAreaBestFoundValue > iMinFoundValue || iWaterAreaBestFoundValue > iMinFoundValue)
		{
			// BBAI TODO: Needs logic to check for early settler builds, settler builds in small cities, whether settler sea exists for water area sites?
			if (pWaterArea)
			{
				const int iTotalCities = player.getNumCities();
				int iSettlerSeaNeeded = std::min(iNumWaterAreaCitySites, 1 + (iTotalCities + 4) / 8);
				if (player.getCapitalCity())
				{
					int iOverSeasColonies = iTotalCities - player.getCapitalCity()->area()->getCitiesPerPlayer(eOwner);
					int iLoop = 2;
					int iExtras = 0;
					while (iOverSeasColonies >= iLoop)
					{
						iExtras++;
						iLoop += iLoop + 2;
					}
					iSettlerSeaNeeded += std::min(player.AI_totalUnitAIs(UNITAI_WORKER) / 4, iExtras);
				}
				if (bAssault)
				{
					iSettlerSeaNeeded = std::min(1, iSettlerSeaNeeded);
				}

				if (player.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_SETTLER_SEA) < iSettlerSeaNeeded)
				{
					/* financial trouble: 2/3; */
					if (iDangerValue < 5 && bFinancialTrouble && iWorkersInArea < 5 * iNeededWorkersInArea && iWorkersNeeded > 0
					&& (getPopulation() > 1 || GC.getGame().getGameTurn() - getGameTurnAcquired() > 15 * iHammerCostPercent / 100))
					{
						if (!bChooseWorker && AI_chooseUnit("worker needed", UNITAI_WORKER))
						{
							return;
						}
						bChooseWorker = true;
					}

					if (AI_chooseUnit("sea settler needed", UNITAI_SETTLER_SEA))
					{
						return;
					}
				}
			}

			if (iPlotSettlerCount == 0 && iNumSettlers < iMaxSettlers)
			{
				// Workers first if in financial trouble
				if (iDangerValue < 6 && iWorkersInArea < (2 * iNeededWorkersInArea + 2) / 3 && iWorkersNeeded > 0
				&& (getPopulation() > 1 || GC.getGame().getGameTurn() - getGameTurnAcquired() > 15 * iHammerCostPercent / 100))
				{
					if (!bChooseWorker && AI_chooseUnit("worker needed 2", UNITAI_WORKER))
					{
						return;
					}
					bChooseWorker = true;
				}
				// City Defenders - 24.07.2010 - Fuyu
				if (player.getNumMilitaryUnits() <= 1 + player.getNumCities() + iNumSettlers
				//Fuyu: in the beginning, don't count on other cities to build the escorts
				|| player.getNumCities() <= 7 && iNumCitiesInArea <= 3
				&& plot()->plotCount(PUF_canDefendGroupHead, -1, -1, NULL, eOwner, NO_TEAM, PUF_isCityAIType) <= 1)
				{
					LOG_BBAI_CITY(2, ("City %S, Will Start to build Extra quick defenders. For the moment : InArea : %d, Needed : %d", getName().GetCString(), player.getNumMilitaryUnits(), 1 + player.getNumCities() + iNumSettlers));
					if (AI_chooseUnit("extra quick defender", UNITAI_CITY_DEFENSE))
					{
						return;
					}
					//Fuyu bIgnoreNotUnitAIs
					if (iNumCitiesInArea <= 3)
					{
						CvUnitSelectionCriteria criteria;

						criteria.m_bIgnoreNotUnitAIs = true;
						if (AI_chooseUnit("extra quick defender (any unit)", UNITAI_CITY_DEFENSE, -1, -1, -1, &criteria))
						{
							return;
						}
					}

					if (AI_chooseUnit("extra quick defender", UNITAI_CITY_COUNTER))
					{
						return;
					}
					if (AI_chooseUnit("extra quick defender", UNITAI_CITY_SPECIAL))
					{
						return;
					}

					if (AI_chooseUnit("extra quick defender", UNITAI_RESERVE))
					{
						return;
					}
				}
				if (AI_chooseUnit("settler needed", UNITAI_SETTLE))
				{
					return;
				}
			}
		}
	}

	m_iTempBuildPriority--;

	//#14 Check if Investigators need
	if (AI_establishInvestigatorCoverage())
	{
		return;
	}
	//#14b, Check for at least one explorer
	if (player.AI_totalAreaUnitAIs(pArea, UNITAI_EXPLORE) == 0)
	{
		if (AI_chooseUnit("need explorers", UNITAI_EXPLORE))
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	//#15 Check if SeeInvisibleUnits Needed
	if ((m_iRequestedUnit <= MAX_REQUESTEDUNIT_PER_CITY) && !bInhibitUnits && iOwnedSeeInvisibles < iMaxSeeInvisibleUnits)
	{
		LOG_BBAI_CITY(3, ("#15 City %S, Will Start to build See Invisibile Units. For the moment : Owned : %d, Needed : %d", getName().GetCString(), iOwnedSeeInvisibles, iMaxSeeInvisibleUnits));
		if (AI_establishSeeInvisibleCoverage())
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	//#16 Hunters, Prio2
	if (!bInhibitUnits && iOwnedHunters < 8 && iHunterDeficitPercent > 50)
	{
		LOG_BBAI_CITY(3, ("#16 City %S, Will Start to build hunters Prio2 (iHunterDeficitPercent : %d). For the moment : Owned : %d, Needed : %d", getName().GetCString(), iHunterDeficitPercent, iOwnedHunters, iNeededHunters));
		if (AI_chooseExperienceBuilding(UNITAI_HUNTER, 4))
		{
			return;
		}
		if (AI_chooseUnit("#16 not much hunters", UNITAI_HUNTER))
		{
			return;
		}
	}

	//#17 Healthy Business if Needed
	if (iHealth < 0 && -iHealth > iFoodDiffBase / 5)
	{
		LOG_BBAI_CITY(3, ("#17 City %S, Will Start to build Healthy Buildings. For the moment : Health : %d", getName().GetCString(), iHealth));
		//	If there is negative health address it if we can do so quickly (relative to the amount of ill health)
		//	Koshling - don't waste time with this if we have a food excess that is being trimmed by less than 20%
		//	by the ill health (rounded - so at less than 5 net food we always consider this)
		//	Koshling - reduce the time we're prepared to spend on this by the current food excess (if any)
		if (AI_chooseBuilding(BUILDINGFOCUS_HEALTHY, std::max(1, -iHealth - std::max(0, iFoodDiffBase)), 0, -1, true))
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	//#18 - UNITAI_CITY_DEFENSE to reach min defenders in city
	if (!bInhibitUnits && plot()->getNumDefenders(eOwner) < AI_minDefenders()) // XXX check for other team's units?
	{
		LOG_BBAI_CITY(3, ("#18 City %S, Will Start to build defenders to reach mini. For the moment : owned : %d, needer : %d", getName().GetCString(), plot()->getNumDefenders(eOwner), AI_minDefenders()));
		static CvUnitSelectionCriteria DefaultCriteria;
		if (AI_chooseUnit("low defense city", UNITAI_CITY_DEFENSE, -1, -1, CITY_BUILD_PRIORITY_CEILING, &DefaultCriteria))
		{
			return;
		}

		if (player.getNumCities() <= 3)
		{
			CvUnitSelectionCriteria criteria;

			criteria.m_bIgnoreNotUnitAIs = true;
			if (AI_chooseUnit("defenseless city any unit", UNITAI_CITY_DEFENSE, -1, -1, CITY_BUILD_PRIORITY_CEILING, &criteria))
			{
				return;
			}
		}

		if (AI_chooseUnit("defenseless city counter units", UNITAI_CITY_COUNTER, -1, -1, CITY_BUILD_PRIORITY_CEILING, &DefaultCriteria))
		{
			return;
		}

		if (AI_chooseUnit("defenseless city special units", UNITAI_CITY_SPECIAL, -1, -1, CITY_BUILD_PRIORITY_CEILING, &DefaultCriteria))
		{
			return;
		}

		if (AI_chooseUnit("defenseless city reserve units", UNITAI_RESERVE, -1, -1, CITY_BUILD_PRIORITY_CEILING, &DefaultCriteria))
		{
			return;
		}

		if (player.getNumCities() > 3 && iNumCitiesInArea <= 3)
		{
			CvUnitSelectionCriteria criteria;

			criteria.m_bIgnoreNotUnitAIs = true;
			if (AI_chooseUnit("defenseless city any unit (low city count area)", UNITAI_CITY_DEFENSE, -1, -1, HIGHEST_PRIORITY_ESCORT_PRIORITY, &criteria))
			{
				return;
			}
		}

		if (AI_chooseUnit("defenseless city (accept attack units)", UNITAI_ATTACK, -1, -1, CITY_BUILD_PRIORITY_CEILING, &DefaultCriteria))
		{
			return;
		}
	}


	//#19 TB Build Priority Mod Begin
	{
		PROFILE("AI_chooseProduction.TB_Mod");
		LOG_BBAI_CITY(3, ("#19 City %S, Will Start to build One of TB Prio Buildings.", getName().GetCString()));
		int iPriorityCheckFlags = 0;

		if (!isHuman())
		{
			iPriorityCheckFlags |= BUILDINGFOCUS_RESEARCH;
		}

		if (!isHuman() || AI_isEmphasizeYield(YIELD_PRODUCTION))
		{
			iPriorityCheckFlags |= BUILDINGFOCUS_PRODUCTION;
		}

		if (!isHuman() || AI_isEmphasizeYield(YIELD_FOOD))
		{
			iPriorityCheckFlags |= BUILDINGFOCUS_FOOD;
		}

		if (iPriorityCheckFlags != 0 && AI_chooseBuilding(iPriorityCheckFlags, 5))
		{
			return;
		}
	}
	//TB Build Priority Mod End

	m_iTempBuildPriority--;

	//#20 Cultural Buildings
	if (iTargetCulturePerTurn > 0 && !bStrategyTurtle
	&& (!isHuman() || AI_isEmphasizeCommerce(COMMERCE_CULTURE))
	&& iCulturePerTurn < GC.getGame().getSorenRandNum(iTargetCulturePerTurn * 5 / 4, "Culture roll")
	&& AI_chooseBuilding(BUILDINGFOCUS_CULTURE, 15))
	{
		LOG_BBAI_CITY(3, ("#20 City %S, Will Start to build a Cultural Building.", getName().GetCString()));
		return;
	}

	m_iTempBuildPriority--;

	if (m_iTempBuildPriority >= HIGH_PRIORITY_ESCORT_PRIORITY)
	{
		m_iTempBuildPriority = HIGH_PRIORITY_ESCORT_PRIORITY - 1;
	}

	//#21 Health Buildings if Domination as Target
	if (iHealth < 1 && player.AI_isDoVictoryStrategy(AI_VICTORY_DOMINATION3) && AI_chooseBuilding(BUILDINGFOCUS_HEALTHY, 10, 0, player.AI_isDoVictoryStrategy(AI_VICTORY_DOMINATION4) ? 50 : 20))
	{
		return;
	}

	//#22 Health & Cultral Buildings if is a Vassal
	if (!bLandWar && GET_TEAM(getTeam()).isAVassal() && GET_TEAM(getTeam()).isCapitulated())
	{
		if (iHealth < 1 && AI_chooseBuilding(BUILDINGFOCUS_HEALTHY, 10, 0, 3 * getPopulation()))
		{
			return;
		}

		if (iTargetCulturePerTurn > 0
		&& iCulturePerTurn < GC.getGame().getSorenRandNum(iTargetCulturePerTurn * 4 / 5, "Culture roll")
		&& AI_chooseBuilding(BUILDINGFOCUS_CULTURE, 8, 0 + 3 * iWarTroubleThreshold, 3 * getPopulation()))
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	// -------------------- BBAI Notes -------------------------
	//#23 Minimal attack force because of Danger, both land and sea
	if (bDanger && !bInhibitUnits)
	{
		const int iAttackNeeded = iNbMinimalAttackers + std::max(0, AI_neededDefenders() - (iPlotCityDefenderCount + iPlotOtherCityAICount)) + intSqrt(player.getNumCities()); //Try to add some attack units at minimum
		const int iOwnedAttackers = player.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK);
		LOG_BBAI_CITY(2, ("#23 City %S, Will Start to build mini Attack forces, due to Danger. For the moment : Owned : %d, Needed : %d", getName().GetCString(), iOwnedAttackers, iAttackNeeded));
		
		if (iOwnedAttackers < iAttackNeeded
		&& AI_chooseUnitImmediate("minimal attack (danger)", UNITAI_ATTACK))
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	//#24 Minimal Navy
	if (bMaybeWaterArea	&& !bInhibitUnits && !bDanger && !bFinancialTrouble	&& (!bLandWar || iWarSuccessRatio >= -30))
	{
		if (
			player.AI_getNumTrainAIUnits(UNITAI_ATTACK_SEA)
			+ player.AI_getNumTrainAIUnits(UNITAI_PIRATE_SEA)
			+ player.AI_getNumTrainAIUnits(UNITAI_RESERVE_SEA)
			< std::min(3, player.getNumCities())
			&&
			(
				bMaybeWaterArea && bWaterDanger
				||
				pWaterArea && bPrimaryArea
				&& player.AI_countNumAreaHostileUnits(pWaterArea, true, false, false, false, plot(), 15) > 0
				)
			// If there are a few local appropriate AI units already assume they will deal
			&& player.AI_countNumLocalNavy(plot(), 4) < 2
			&&
			(
				AI_chooseUnit("minimal navy", UNITAI_ATTACK_SEA) ||
				AI_chooseUnit("minimal navy", UNITAI_PIRATE_SEA) ||
				AI_chooseUnit("minimal navy", UNITAI_RESERVE_SEA)
				)
			) return;

		if (pWaterArea)
		{
			int iOdds = -1;
			if (iAreaBestFoundValue == 0 || iWaterAreaBestFoundValue > iAreaBestFoundValue)
			{
				iOdds = 100;
			}
			else if (iWaterPercent > 60)
			{
				iOdds = 13;
			}

			if (iOdds > -1)
			{
				if (player.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_EXPLORE_SEA) == 0
					&& AI_chooseUnit("sea explorer", UNITAI_EXPLORE_SEA, iOdds))
				{
					return;
				}

				// BBAI TODO: Really only want to do this if no good area city sites ... 13% chance on water heavy maps
				// of slow start, little benefit
				if (player.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_SETTLER_SEA) == 0
					&& AI_chooseUnit("settler sea", UNITAI_SETTLER_SEA, iOdds))
				{
					return;
				}
			}
		}
	}

	m_iTempBuildPriority--;

	// -------------------- BBAI Notes -------------------------
	// Top normal priorities

	//#25 If Focus Food, Food buildings
	if (!bPrimaryArea && !bLandWar && (!isHuman() || AI_isEmphasizeYield(YIELD_FOOD)) && AI_chooseBuilding(BUILDINGFOCUS_FOOD, 20, 10 + 2 * iWarTroubleThreshold, isHuman() ? -1 : 50))
	{
		LOG_BBAI_CITY(2, ("      City %S uses choose BUILDINGFOCUS_FOOD 1", getName().GetCString()));		
		return;
	}

	m_iTempBuildPriority--;

	//#26 Focus Production buildings
	if (iDangerValue < 5 && (eCurrentEra > GC.getGame().getStartEra() + iProductionRank / 2 || eCurrentEra > GC.getNumEraInfos() / 2))
	{
		if ((!isHuman() || AI_isEmphasizeYield(YIELD_PRODUCTION))
			&& AI_chooseBuilding(BUILDINGFOCUS_PRODUCTION, 20 - iWarTroubleThreshold, 15, (!isHuman() && (bLandWar || bAssault)) ? 25 : -1))
		{
			LOG_BBAI_CITY(2, ("      City %S uses choose BUILDINGFOCUS_PRODUCTION 1", getName().GetCString()));
			return;
		}

		if (!bChooseWorker && !bInhibitUnits && (!bDefenseWar || iWarSuccessRatio >= -30) && iWorkersInArea < iNeededWorkersInArea
		&& (getPopulation() > 3 || iProductionRank < (player.getNumCities() + 1) / 2))
		{
			if (AI_chooseUnit("no danger workers", UNITAI_WORKER))
			{
				return;
			}
			bChooseWorker = true;
		}
	}

	m_iTempBuildPriority--;

	//#27 Floating Defenders
	if (!bInhibitUnits && iTotalFloatingDefenders < ((iNeededFloatingDefenders + 1) / (bGetBetterUnits ? 3 : 2)))
	{
		UnitTypeWeightArray floatingDefenderTypes;
		floatingDefenderTypes.push_back(std::make_pair(UNITAI_CITY_DEFENSE, 125));
		floatingDefenderTypes.push_back(std::make_pair(UNITAI_CITY_COUNTER, 100));
		//floatingDefenderTypes.push_back(std::make_pair(UNITAI_CITY_SPECIAL, 0));
		floatingDefenderTypes.push_back(std::make_pair(UNITAI_RESERVE, 100));
		floatingDefenderTypes.push_back(std::make_pair(UNITAI_COLLATERAL, 100));

		if (/*!bUnitExempt && */AI_chooseLeastRepresentedUnit("floating defender", floatingDefenderTypes))// K-Mod
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	//#28 If losing badly in war, need to build up defenses and counter attack force
	if (!bInhibitUnits && bLandWar && (iWarSuccessRatio < -30 || iEnemyPowerPerc > 150))
	{
		UnitTypeWeightArray defensiveTypes;
		defensiveTypes.push_back(std::make_pair(UNITAI_COUNTER, 100));
		defensiveTypes.push_back(std::make_pair(UNITAI_ATTACK, 100));
		defensiveTypes.push_back(std::make_pair(UNITAI_RESERVE, 60));
		defensiveTypes.push_back(std::make_pair(UNITAI_COLLATERAL, 60));

		if (bDanger || iTotalFloatingDefenders < iNeededFloatingDefenders * 5 / (bGetBetterUnits ? 6 : 4))
		{
			defensiveTypes.push_back(std::make_pair(UNITAI_CITY_DEFENSE, 200));
			defensiveTypes.push_back(std::make_pair(UNITAI_CITY_COUNTER, 50));
		}

		const int iOdds = iBuildUnitProb + iDangerValue - (iWarSuccessRatio < -3 ? iWarSuccessRatio / 3 : 0);
		if (AI_chooseLeastRepresentedUnit("extra defense", defensiveTypes, iOdds))
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	//#29 Extra Workers for large city, if no danger 
	if (!bInhibitUnits && !bChooseWorker && iDangerValue < 7 && iWorkersInArea < iNeededWorkersInArea && (!bDefenseWar || iWarSuccessRatio >= -50) && iProductionRank < (player.getNumCities() + 1) / 2)
	{
		if (AI_chooseUnit("no danger large city extra worker", UNITAI_WORKER))
		{
			return;
		}
		bChooseWorker = true;
	}

	m_iTempBuildPriority--;

	if (m_iTempBuildPriority >= LOW_PRIORITY_ESCORT_PRIORITY)
	{
		m_iTempBuildPriority = LOW_PRIORITY_ESCORT_PRIORITY - 1;
	}

	//#30 Extra Sea Workers, if no danger 
	if ((m_iRequestedUnit <= MAX_REQUESTEDUNIT_PER_CITY) && !bInhibitUnits && !bWaterDanger && (!bDefenseWar || iWarSuccessRatio >= -30) && iNeededSeaWorkers > iExistingSeaWorkers && AI_chooseUnit("no danger extra sea worker", UNITAI_WORKER_SEA))
	{
		return;
	}

	//TB Build Mod end

	m_iTempBuildPriority--;

	//#31 Infiltrator Prio2, when enough cities builded                  // Does rival cities exist?
	if ((m_iRequestedUnit <= MAX_REQUESTEDUNIT_PER_CITY) && !bInhibitUnits && bIsCapitalArea && pArea->getNumCities() > iNumCapitalAreaCities && player.AI_totalAreaUnitAIs(pArea, UNITAI_INFILTRATOR) < 1 && AI_chooseUnit("Infiltrator needed", UNITAI_INFILTRATOR))
	{
		// Harass first; eventually find a way to vary the amount of desired infiltrators by leader personality
		// This is enough of a trigger. Once introduced they will expand themselves if needed and possible.

		return;
	}

	m_iTempBuildPriority--;

	//#32 Main Attack Force Stack
	if (!bInhibitUnits && !bGetBetterUnits && (bIsCapitalArea) && (iAreaBestFoundValue < (iMinFoundValue * 2)) && !bStrategyTurtle)
	{
		// BBAI TODO: Check that this works to produce early rushes on tight maps
		//Building city hunting stack.

		if (getDomainFreeExperience(DOMAIN_LAND) == 0 && getYieldRate(YIELD_PRODUCTION) > 5 * getPopulation()
			&& AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, (eCurrentEra > 1) ? 0 : 7, 33))
		{
			LOG_BBAI_CITY(2, ("      City %S uses special BUILDINGFOCUS_EXPERIENCE 1a", getName().GetCString()));
			return;
		}

		int iStartAttackStackRand = 0;
		if (pArea->getCitiesPerPlayer(BARBARIAN_PLAYER) > 0 || pArea->getCitiesPerPlayer(NEANDERTHAL_PLAYER) > 0)
		{
			iStartAttackStackRand += 15;
		}
		if ((pArea->getNumCities() - iNumCitiesInArea) > 0)
		{
			iStartAttackStackRand += iBuildUnitProb / 2;
		}

		const int iAttackCityCount = player.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK_CITY);
		const int iAttackCount = player.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK);
		int iAttackCityTarget = 3 + iBuildUnitProb / 10; 
		int iAttackTarget = 3 + iBuildUnitProb / 10;

		//Calvitix Boost if Warmonger (Conquest Victory > 35)
		if (bIsWarMonger)
		{
			iStartAttackStackRand += 200;
		}
		else
		{
			iStartAttackStackRand -= (iAttackCityCount + iAttackCount)*2;
			//Calvitix Nerf if Peacefull Leader (Conquest Victory < 20)
			if (bIsPeacefull)
			{
				iStartAttackStackRand -= 100;

			}
			if (!bLandWar && !bDanger)
			{
				iStartAttackStackRand -= 50;
			}

		}


		if (iStartAttackStackRand > 0)///CHECK THIS OUT
		{
			if (bIsWarMonger) {
				iAttackCityTarget = iAttackCityTarget * 450 / 100;
				iAttackTarget = iAttackTarget * 450 / 100;
			}
			if (bIsPeacefull) {
				iAttackCityTarget = iAttackCityTarget * 25 / 100;
				iAttackTarget = iAttackTarget * 25 / 100;
			}
			LOG_BBAI_CITY(2, ("#32 City %S, Will Start to build an Attack Stack, StackRand = %d. For the moment : Attack : %d / %d and Attack_City : %d / %d", getName().GetCString(), iStartAttackStackRand, iAttackCount, iAttackTarget, iAttackCityCount, iAttackCityTarget));
			if (iAttackCount == 0)
			{
				if (!bFinancialTrouble
					&& AI_chooseUnit("build attack force", UNITAI_ATTACK))
				{
					return;
				}
			}
			else if (iAttackCount > 1 && iAttackCityCount == 0)
			{
				if (AI_chooseUnit("start city attack stack", UNITAI_ATTACK_CITY))
				{
					return;
				}
			}
			else if (iAttackCityCount < iAttackCityTarget) 
			{
				if (iAttackCount < iAttackTarget)
				{
					if ((iAttackCount*100 / iAttackTarget) <= (iAttackCityCount*100 / iAttackCityTarget))
					{
						LOG_BBAI_CITY(3, ("#32 City %S, Attack Stack add Attack Unit Order. For the moment : Attack : %d and Attack_City : %d", getName().GetCString(), iAttackCount, iAttackCityCount));
						if (GC.getGame().getSorenRandNum(4, "AI prefer collateral") == 0)
						{
							if (AI_chooseUnit("add Collateral unit to stack", UNITAI_COLLATERAL))
							{
								return;
							}
						}
						if (AI_chooseUnit("add to attack stack", UNITAI_ATTACK))
						{
							return;
						}
					}
				}
				LOG_BBAI_CITY(3, ("#32 City %S, Attack Stack add Attack_City Unit Order. For the moment : Attack : %d and Attack_City : %d", getName().GetCString(), iAttackCount, iAttackCityCount));
				if (GC.getGame().getSorenRandNum(5, "AI prefer collateral") == 0)
				{
					if (AI_chooseUnit("add Collateral unit to stack", UNITAI_COLLATERAL))
					{
						return;
					}
				}
				if (AI_chooseUnit("add to city attack stack", UNITAI_ATTACK_CITY))
				{
					return;
				}
			}
		}
		else
		{
			LOG_BBAI_CITY(2, ("#32 City %S, Attack Stack too low : StackRand = %d. For the moment : Attack : %d / %d and Attack_City : %d / %d", getName().GetCString(), iStartAttackStackRand, iAttackCount, iAttackTarget, iAttackCityCount, iAttackCityTarget));
		}
	}

	m_iTempBuildPriority--;

	//#33 Hunters Prio3
	if (iOwnedHunters < 16 && iHunterDeficitPercent > 25)
	{
		LOG_BBAI_CITY(3, ("City %S, Will Start to build hunters Prio3 (iHunterDeficitPercent : %d). For the moment : Owned : %d, Needed : %d", getName().GetCString(), iHunterDeficitPercent, iOwnedHunters, iNeededHunters));
		if (AI_chooseExperienceBuilding(UNITAI_HUNTER, 6))
		{
			return;
		}
		if (AI_chooseUnit("less than half the required hunters", UNITAI_HUNTER))
		{
			return;
		}
	}

	m_iTempBuildPriority--;
	
	//#34 Opportunistic wonder build (1)
	if (iDangerValue < 6 && !hasActiveWorldWonder() && player.getNumCities() <= 3 && (!bLandWar || iWarSuccessRatio > 30))
	{
		// For small civ at war, don't build wonders unless winning
		if (AI_chooseBuilding(BUILDINGFOCUS_WORLDWONDER, 7))
		{
			LOG_BBAI_CITY(2, ("      City %S uses oppurtunistic wonder build 1", getName().GetCString()));
			return;
		}
	}

	m_iTempBuildPriority--;

	//#35 Buildings for Move Capital (when more Cities int that Area than in Capital'Area
	if (iDangerValue < 6 && !bIsCapitalArea && iNumCitiesInArea > iNumCapitalAreaCities)
	{
		// BBAI TODO:  This check should be done by player, not by city and optimize placement
		// If losing badly in war, don't build big things
		if (!bLandWar
		|| iWarSuccessRatio > -30
		&& (!player.getCapitalCity() || pArea->getPopulationPerPlayer(eOwner) > player.getCapitalCity()->area()->getPopulationPerPlayer(eOwner)))
		{
			if (AI_chooseBuilding(BUILDINGFOCUS_CAPITAL, 15))
			{
				return;
			}
		}
	}

	m_iTempBuildPriority--;

	//#36 Food Buildings Focus 2
	if (!isHuman() || AI_isEmphasizeYield(YIELD_FOOD))
	{
		if (AI_chooseBuilding(BUILDINGFOCUS_FOOD, isCapital() ? 3 : 12, 30))
		{
			LOG_BBAI_CITY(2, ("      City %S uses choose BUILDINGFOCUS_FOOD 2", getName().GetCString()));
			return;
		}
	}

	//#37 Missionaries to spread Religion
	if (!bInhibitUnits && !bDanger && !bStrategyTurtle)
	{
		const int iSpreadUnitRoll = (bLandWar ? 0 : 10) + (100 - iBuildUnitProb) / 3;

		if (AI_bestSpreadUnit(true, true, iSpreadUnitRoll, &eBestSpreadUnit, &iBestSpreadUnitValue)
			&& iBestSpreadUnitValue > iSpreadUnitThreshold
			&& AI_chooseUnit(eBestSpreadUnit, UNITAI_MISSIONARY))
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	//#38 Inquisitors
	if (!bInhibitUnits && AI_trainInquisitor())
	{
		return;
	}

	//#39 Projects
	if (iDangerValue < 3 && !isHuman() && (!bLandWar || iWarSuccessRatio >= 30)	&& iProductionRank <= 1 + player.getNumCities() / 5	&& AI_chooseProject())
	{
		LOG_BBAI_CITY(3, ("#39      City %S uses choose project 1", getName().GetCString()));	
		return;
	}

	m_iTempBuildPriority--;

	//#40 workers (again?) for established city, if financial troubles, or when the workes are not in the good area
	if (!bInhibitUnits && (!bLandWar || iWarSuccessRatio >= 0) && iDangerValue < 4)
	{
		/* financial trouble: ---; will grow above happy cap: 2/3; both: 3/4; else 4/7 */
		if ((iWorkersInArea < ((4 * iNeededWorkersInArea) + 6) / 7)
			/* || (bFinancialTrouble && (iWorkersInArea < (((2*iNeededWorkersInArea) + 1)/3))) */
			|| (((iWorkersInArea < ((2 * iNeededWorkersInArea) + 2) / 3) || (bFinancialTrouble && (iWorkersInArea < (((3 * iNeededWorkersInArea) + 3) / 4))))
				&& (((happyLevel() - unhappyLevel()) <= 0) && (foodDifference(false) > 0 || (foodDifference(false) == 0 && happyLevel() - unhappyLevel() < 0)))))
		{
			if (iWorkersNeeded > 0)
			{
				if (getPopulation() > 2 || GC.getGame().getGameTurn() - getGameTurnAcquired() > 15 * iHammerCostPercent / 100)
				{
					if (!bChooseWorker && AI_chooseUnit("worker for established city", UNITAI_WORKER))
					{
						return;
					}
					bChooseWorker = !bChooseWorker;
				}
			}
		}
	}

	m_iTempBuildPriority--;

	//#41 Cultural Race - Build of Cathedral if Important City
	if (bImportantCity && (!isHuman() || AI_isEmphasizeCommerce(COMMERCE_CULTURE)) && iCultureRateRank <= iCulturalVictoryNumCultureCities)
	{
		//this is needed to build the cathedrals quickly
		//also very good for giving cultural cities first dibs on wonders
		if (iCultureRateRank == iCulturalVictoryNumCultureCities)
		{
			if (AI_chooseBuilding(BUILDINGFOCUS_BIGCULTURE | BUILDINGFOCUS_CULTURE | BUILDINGFOCUS_WONDEROK, 20))
			{
				LOG_BBAI_CITY(3, ("#41      City %S uses cultural victory 1", getName().GetCString()));
				return;
			}
		}
		else if (
			GC.getGame().getSorenRandNum(
				((iCultureRateRank == 1) ? 4 : 1) + 2 * iCulturalVictoryNumCultureCities + (bLandWar ? 5 : 0),
				"AI Build up Culture"
			) < iCultureRateRank
			&&
			AI_chooseBuilding(BUILDINGFOCUS_BIGCULTURE | BUILDINGFOCUS_CULTURE | BUILDINGFOCUS_WONDEROK, (bLandWar ? 8 : 16)))
		{
			LOG_BBAI_CITY(3, ("#41      City %S uses cultural victory 2", getName().GetCString()));
			return;
		}
	}

	m_iTempBuildPriority--;

	//#42 Optional Attack Unit, if no one in that Area
	if (!bInhibitUnits && (!bImportantCity || bLandWar || bAssault))
	{
		// don't build frivolous things if this is an important city unless we at war
		//	Koshling in early game moved optional non-wartime attack unit builds below economy
		if (bPrimaryArea && eCurrentEra != 0
			&& player.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK) == 0
			&& AI_chooseUnit("optional attack", UNITAI_ATTACK))
		{
			return;
		}
	}

	//#42b Explorers
	if ((bMassing || !bLandWar) && iDangerValue < 4 && !bFinancialTrouble)
	{
		const int iNeededExplorers = player.AI_neededExplorers(pArea);
		const int iExplorerDeficitPercent = (iNeededExplorers == 0) ? 0 : (iNeededExplorers - player.AI_totalAreaUnitAIs(pArea, UNITAI_EXPLORE)) * 100 / iNeededExplorers;
		if (iExplorerDeficitPercent >= iHunterDeficitPercent && iExplorerDeficitPercent > 0)
		{
			// If we are just pumping out explorer units and having them die fast go for EXP giving buildings first
			if (AI_chooseExperienceBuilding(UNITAI_EXPLORE, 8))
			{
				return;
			}
			if (AI_chooseUnit("need explorers", UNITAI_EXPLORE))
			{
				return;
			}
		}

		if (iHunterDeficitPercent > 0)
		{
			// If we are just pumping out hunting units and having them die fast go for EXP giving buildings first
			if (AI_chooseExperienceBuilding(UNITAI_HUNTER, 8))
			{
				return;
			}

			if (AI_chooseUnit("need hunters", UNITAI_HUNTER))
			{
				return;
			}
		}
	}

	//#42C Emergency Attackers as Defenders
	if (bDefenseWar || bLandWar && iWarSuccessRatio < -30)
	{
		UnitTypeWeightArray panicDefenderTypes;
		panicDefenderTypes.push_back(std::make_pair(UNITAI_RESERVE, 100));
		panicDefenderTypes.push_back(std::make_pair(UNITAI_COUNTER, 100));
		panicDefenderTypes.push_back(std::make_pair(UNITAI_COLLATERAL, 100));
		panicDefenderTypes.push_back(std::make_pair(UNITAI_ATTACK, 100));

		if (AI_chooseLeastRepresentedUnit("panic defender", panicDefenderTypes, (bGetBetterUnits ? 40 : 60) - iWarSuccessRatio / 3))
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	//#43 Nuke Production // Afforess - Encourage Aggressive AI to stock up on a few nukes
	if (!bInhibitUnits && !bFinancialTrouble && (GC.getGame().isOption(GAMEOPTION_AI_RUTHLESS) || GC.getGame().getSorenRandNum(10, "AI consider Nuke") == 0))
	{
		//keep between 3 to 8 nukes, at least for early considerations
		const int iNukesNeeded = std::max(3, std::min((GC.getGame().getNumCities() - player.getNumCities()) / 5, 8));

		if (player.AI_totalUnitAIs(UNITAI_ICBM) < iNukesNeeded && AI_chooseUnit("ICBM", UNITAI_ICBM))
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	//#44 Food Buildings Focus 3
	if ((!isHuman() || AI_isEmphasizeYield(YIELD_FOOD)) && AI_chooseBuilding(BUILDINGFOCUS_FOOD, 15, 10, (bLandWar ? 30 : -1)))
	{
		LOG_BBAI_CITY(3, ("#44      City %S uses choose BUILDINGFOCUS_FOOD 3", getName().GetCString()));
		return;
	}

	m_iTempBuildPriority--;

	//#45 Opportunistic wonder build Focus 2
	if (!isHuman() && iDangerValue < 6 && (!hasActiveWorldWonder() || (player.getNumCities() > 3)))
	{
		// For civ at war, don't build wonders if losing
		if (!bLandWar || iWarSuccessRatio > -25)
		{
			if (AI_chooseBuilding(BUILDINGFOCUS_WORLDWONDER, 10))
			{
				LOG_BBAI_CITY(3, ("#45      City %S uses opportunistic wonder build 2", getName().GetCString()));
				return;
			}
		}
	}

	m_iTempBuildPriority--;

	//#46 Workers (again), it City doesn't have enough Hammer                                                                  //Calvitix test to reduce workers
	if (!bChooseWorker && !bInhibitUnits && iDangerValue < 8 && (!bLandWar || iWarSuccessRatio >= -30) && 0 < iNeededWorkersInArea && iWorkersNeeded > 1 && (getPopulation() > 1 || GC.getGame().getGameTurn() - getGameTurnAcquired() > 15 * iHammerCostPercent / 100))
	{
		if (AI_chooseUnit("established city needs more workers", UNITAI_WORKER))
		{
			return;
		}
		bChooseWorker = true;
	}

	m_iTempBuildPriority--;

	//#47 semi-urgent property control (bad property > 15% through op range - choose unit to deal if possible)
	if (AI_choosePropertyControlBuildingAndUnit(15, NO_PROPERTY, iMaxPropertyUnitsPercent))
	{
		return;
	}

	m_iTempBuildPriority--;

	//#48 essential economic builds
	if (AI_chooseBuilding(iEconomyFlags, 10, iEcononmyFlagsThreasholdWeighting * (25 + iWarTroubleThreshold) / 100, (bLandWar && !isHuman() ? 40 : -1)))
	{
		LOG_BBAI_CITY(3, ("#48      City %S uses choose iEconomyFlags 1", getName().GetCString()));
		return;
	}

	m_iTempBuildPriority--;

	//#49 GOLD/COMMERCE buildings when good prod.
	if (iDangerValue < 4 && bFinancialTrouble && (isCapital() || getYieldRate(YIELD_PRODUCTION) > std::min(70, std::max(40, iNumCitiesInArea * 6)))	&& AI_chooseProcess(COMMERCE_GOLD))
	{
		LOG_BBAI_CITY(3, ("#49      City %S chooses to build wealth due to financial trouble", getName().GetCString()));
		return;
	}

	m_iTempBuildPriority--;

	//#50 Normal running property control (3) (bad property > 10% through op range - choose unit to deal if possible)
	if (AI_choosePropertyControlBuildingAndUnit(10, NO_PROPERTY, iMaxPropertyUnitsPercent))
	{
		return;
	}

	m_iTempBuildPriority--;

	//#51 Missionaries (Prio2)
	if (!bInhibitUnits && !bDanger && iBestSpreadUnitValue > iSpreadUnitThreshold * (bLandWar ? 80 : 60) / 100 && AI_chooseUnit(eBestSpreadUnit, UNITAI_MISSIONARY))
	{
		return;
	}

	m_iTempBuildPriority--;

	//#52 Buildings special BUILDINGFOCUS_EXPERIENCE
	if (getDomainFreeExperience(DOMAIN_LAND) == 0 && getYieldRate(YIELD_PRODUCTION) > 4	&& AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, (eCurrentEra > 1) ? 0 : 7, 33))
	{
		LOG_BBAI_CITY(3, ("#52      City %S uses special BUILDINGFOCUS_EXPERIENCE 1", getName().GetCString()));
		return;
	}

	m_iTempBuildPriority--;

	//#53 Early game Economic builds & Wonders
	if (AI_chooseBuilding(iEconomyFlags | BUILDINGFOCUS_WONDEROK, 20, 0, eCurrentEra == 0 ? 100 : 50))
	{ ////	Koshling - increased priority for economic builds, especially in early game and allowed
	//	wonders in if they appear best
		LOG_BBAI_CITY(3, ("#53      City %S uses choose iEconomyFlags (Koshling)", getName().GetCString()));
		return;
	}

	m_iTempBuildPriority--;

	//#54 Early game 1x Attack Unit, if war or assault
	if (!bInhibitUnits && bPrimaryArea && (!bImportantCity || bLandWar || bAssault)	&& eCurrentEra == 0 && player.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK) == 0 && AI_chooseUnit("primary area attack", UNITAI_ATTACK))
	{
		// Koshling in early game moved optional non-wartime attack unit builds below economy
		// don't build frivolous things if this is an important city unless we at war
		return;
	}

	m_iTempBuildPriority--;

	//#55 Optional workers & Sea workers (5th), if no Workers in Area, 
	if (!bInhibitUnits && iDangerValue < 3 && iWorkersInArea == 0 && (isCapital() || iNeededWorkersInArea > 0 || (iNeededSeaWorkers > iExistingSeaWorkers)))
	{
		// Koshling - next section moved from quite a bit earlier to avoid not-needed-yet worker builds before we have checked basic economy builds
		// do a check for one tile island type thing?
		// this can be overridden by "wait and grow more"
		if (!bStrategyTurtle && (!bDefenseWar || iWarSuccessRatio >= -30))
		{
			if ((AI_countNumBonuses(NO_BONUS, /*bIncludeOurs*/ true, /*bIncludeNeutral*/ true, -1, /*bLand*/ true, /*bWater*/ false) > 0) ||
				(isCapital() && (getPopulation() > 3) && iNumCitiesInArea > 1))
			{
				if (!bChooseWorker && AI_chooseUnit("optional worker", UNITAI_WORKER))
				{
					return;
				}
				bChooseWorker = true;
			}

			if (iNeededSeaWorkers > iExistingSeaWorkers)
			{
				if (AI_chooseUnit("optional sea worker", UNITAI_WORKER_SEA))
				{
					return;
				}
			}
		}
	}

	m_iTempBuildPriority--;

	//#56 Aircraft Units, to help landwar or assault
	if (!bInhibitUnits && iUnitCostPercentage < (iMaxUnitSpending + 4) && (!bImportantCity || bDefenseWar))
	{
		if (bLandWar || bAssault || (iFreeAirExperience > 0) || (GC.getGame().getSorenRandNum(3, "AI train air") == 0))
		{
			int iBestAirValue = player.AI_bestCityUnitAIValue(UNITAI_ATTACK_AIR, this, &eBestAttackAircraft);
			int iBestMissileValue = player.AI_bestCityUnitAIValue(UNITAI_MISSILE_AIR, this, &eBestMissile);
			if ((iBestAirValue + iBestMissileValue) > 0)
			{
				iAircraftHave = player.AI_totalUnitAIs(UNITAI_ATTACK_AIR) + player.AI_totalUnitAIs(UNITAI_DEFENSE_AIR) + player.AI_totalUnitAIs(UNITAI_MISSILE_AIR);
				if (NO_UNIT != eBestAttackAircraft)
				{
					iAircraftNeed = (2 + player.getNumCities() * (3 * GC.getUnitInfo(eBestAttackAircraft).getAirCombat())) / (2 * std::max(1, GC.getGame().getBestLandUnitCombat()));
					int iBestDefenseValue = player.AI_bestCityUnitAIValue(UNITAI_DEFENSE_AIR, this);
					if ((iBestDefenseValue > 0) && (iBestAirValue > iBestDefenseValue))
					{
						iAircraftNeed *= 3;
						iAircraftNeed /= 2;
					}
				}
				if (iBestMissileValue > 0)
				{
					iAircraftNeed = std::max(iAircraftNeed, 1 + player.getNumCities() / 2);
				}

				bool bAirBlitz = player.AI_isDoStrategy(AI_STRATEGY_AIR_BLITZ);
				bool bLandBlitz = player.AI_isDoStrategy(AI_STRATEGY_LAND_BLITZ);
				if (bAirBlitz)
				{
					iAircraftNeed *= 3;
					iAircraftNeed /= 2;
				}
				else if (bLandBlitz)
				{
					iAircraftNeed /= 2;
					iAircraftNeed += 1;
				}

				airUnitTypes.push_back(std::make_pair(UNITAI_ATTACK_AIR, bAirBlitz ? 125 : 80));
				airUnitTypes.push_back(std::make_pair(UNITAI_DEFENSE_AIR, /*bLandBlitz ? 100 :*/ 100));
				if (iBestMissileValue > 0)
				{
					airUnitTypes.push_back(std::make_pair(UNITAI_MISSILE_AIR, bAssault ? 60 : 40));
				}

				airUnitTypes.push_back(std::make_pair(UNITAI_ICBM, 20));

				if (iAircraftHave * 2 < iAircraftNeed)
				{
					if (AI_chooseLeastRepresentedUnit("need airforce", airUnitTypes))
					{
						return;
					}
				}
				// Additional check for air defenses
				int iFightersHave = player.AI_totalUnitAIs(UNITAI_DEFENSE_AIR);

				if (3 * iFightersHave < iAircraftNeed)
				{
					if (AI_chooseUnit("need air defense", UNITAI_DEFENSE_AIR))
					{
						return;
					}
				}
			}
		}
	}

	m_iTempBuildPriority--;

	//#57 Sea Assault & Sea Transport Units, to help landwar or assault
	if (!bInhibitUnits && iUnitCostPercentage < (iMaxUnitSpending + 10))
	{
		bool bBuildAssault = bAssault;
		CvArea* pAssaultWaterArea = NULL;
		if (NULL != pWaterArea)
		{
			// Coastal city extra logic
			pAssaultWaterArea = pWaterArea;

			// If on offensive and can't reach enemy cities from here, act like using AREAAI_ASSAULT
			if (!bBuildAssault
			&& pAssaultWaterArea != NULL
			&& GET_TEAM(getTeam()).hasWarPlan(true)
			&& eAreaAI != AREAAI_DEFENSIVE
			// BBAI TODO: faster to switch to checking path for some selection group?
			&& !plot()->isHasPathToEnemyCity(getTeam()))
			{
				bBuildAssault = true;
			}
		}

		if (bBuildAssault)
		{
			LOG_BBAI_CITY(3, ("#57      City %S uses build assault", getName().GetCString()));

			UnitTypes eBestAssaultUnit = NO_UNIT;
			if (NULL != pAssaultWaterArea)
			{
				player.AI_bestCityUnitAIValue(UNITAI_ASSAULT_SEA, this, &eBestAssaultUnit);
			}
			else
			{
				player.AI_bestCityUnitAIValue(UNITAI_ASSAULT_SEA, NULL, &eBestAssaultUnit);
			}

			int iBestSeaAssaultCapacity = 0;
			if (eBestAssaultUnit != NO_UNIT)
			{
				iBestSeaAssaultCapacity = GC.getUnitInfo(eBestAssaultUnit).getCargoSpace();
			}

			int iAreaAttackCityUnits = player.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK_CITY);

			int iUnitsToTransport = iAreaAttackCityUnits;
			iUnitsToTransport += player.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK);
			iUnitsToTransport += player.AI_totalAreaUnitAIs(pArea, UNITAI_COUNTER) / 2;

			int iLocalTransports = player.AI_totalAreaUnitAIs(pArea, UNITAI_ASSAULT_SEA);
			int iTransportsAtSea = 0;
			if (NULL != pAssaultWaterArea)
			{
				iTransportsAtSea = player.AI_totalAreaUnitAIs(pAssaultWaterArea, UNITAI_ASSAULT_SEA);
			}
			else
			{
				iTransportsAtSea = player.AI_totalUnitAIs(UNITAI_ASSAULT_SEA) / 2;
			}

			//The way of calculating numbers is a bit fuzzy since the ships
			//can make return trips. When massing for a war it'll train enough
			//ships to move it's entire army. Once the war is underway it'll stop
			//training so many ships on the assumption that those out at sea
			//will return...

			int iTransports = iLocalTransports + (bPrimaryArea ? iTransportsAtSea : iTransportsAtSea / 4);
			int iTransportCapacity = iBestSeaAssaultCapacity * (iTransports);

			if (NULL != pAssaultWaterArea)
			{
				int iEscorts = player.AI_totalAreaUnitAIs(pArea, UNITAI_ESCORT_SEA);
				iEscorts += player.AI_totalAreaUnitAIs(pAssaultWaterArea, UNITAI_ESCORT_SEA);

				int iTransportViability = player.AI_calculateUnitAIViability(UNITAI_ASSAULT_SEA, DOMAIN_SEA);

				int iDesiredEscorts = ((1 + 2 * iTransports) / 3);
				if (iTransportViability > 95)
				{
					// Transports are stronger than escorts (usually Galleons and Caravels)
					iDesiredEscorts /= 3;
				}

				if ((iEscorts < iDesiredEscorts))
				{
					if (AI_chooseBuilding(BUILDINGFOCUS_DOMAINSEA, 12))
					{
						return;
					}

					if (AI_chooseUnit("sea escort", UNITAI_ESCORT_SEA, (iEscorts < iDesiredEscorts / 3) ? -1 : 50))
					{
						return;
					}
				}

				UnitTypes eBestAttackSeaUnit = NO_UNIT;
				player.AI_bestCityUnitAIValue(UNITAI_ATTACK_SEA, this, &eBestAttackSeaUnit);
				if (eBestAttackSeaUnit != NO_UNIT)
				{
					int iDivisor = 2;
					if (GC.getUnitInfo(eBestAttackSeaUnit).getBombardRate() == 0)
					{
						iDivisor = 5;
					}

					int iAttackSea = player.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK_SEA);
					iAttackSea += player.AI_totalAreaUnitAIs(pAssaultWaterArea, UNITAI_ATTACK_SEA);

					if ((iAttackSea < ((1 + 2 * iTransports) / iDivisor)))
					{
						if (AI_chooseBuilding(BUILDINGFOCUS_DOMAINSEA, 12))
						{
							return;
						}

						if (AI_chooseUnit("sea attack", UNITAI_ATTACK_SEA, (iUnitCostPercentage < iMaxUnitSpending) ? 50 : 20))
						{
							return;
						}
					}
				}

				if (iUnitsToTransport > iTransportCapacity)
				{
					if ((iUnitCostPercentage < iMaxUnitSpending) || (iUnitsToTransport > 2 * iTransportCapacity))
					{
						if (AI_chooseBuilding(BUILDINGFOCUS_DOMAINSEA, 12))
						{
							return;
						}

						if (AI_chooseUnit("sea assault", UNITAI_ASSAULT_SEA))
						{
							return;
						}
					}
				}
			}

			if (iUnitCostPercentage < iMaxUnitSpending)
			{
				if (NULL != pAssaultWaterArea)
				{
					if (!bFinancialTrouble && iCarriers < (player.AI_totalUnitAIs(UNITAI_ASSAULT_SEA) / 4))
					{
						// Reduce chances of starting if city has low production
						if (iProductionRank > (player.getNumCities() / 3) && GC.getGame().getSorenRandNum(100, "AI train carrier") < 30)
						{
							if (AI_chooseBuilding(BUILDINGFOCUS_DOMAINSEA, 12))
							{
								return;
							}

							if (AI_chooseUnit("need carrier", UNITAI_CARRIER_SEA))
							{
								return;
							}
						}
					}
				}
			}

			// Consider building more land units to invade with
			int iTrainInvaderChance = iBuildUnitProb + 10;

			iTrainInvaderChance += (bAggressiveAI ? 15 : 0);
			iTrainInvaderChance /= (bAssaultAssist ? 2 : 1);
			iTrainInvaderChance /= (bImportantCity ? 2 : 1);
			iTrainInvaderChance /= (bGetBetterUnits ? 2 : 1);

			iUnitsToTransport *= 9;
			iUnitsToTransport /= 10;

			if ((iUnitsToTransport > iTransportCapacity) && (iUnitsToTransport > (bAssaultAssist ? 2 : 4) * iBestSeaAssaultCapacity))
			{
				// Already have enough
				iTrainInvaderChance /= 2;
			}
			else if (iUnitsToTransport < (iLocalTransports * iBestSeaAssaultCapacity))
			{
				iTrainInvaderChance += 15;
			}

			if (getPopulation() < 4)
			{
				// Let small cities build themselves up first
				iTrainInvaderChance /= (5 - getPopulation());
			}

			UnitTypeWeightArray invaderTypes;
			invaderTypes.push_back(std::make_pair(UNITAI_ATTACK_CITY, 100));
			invaderTypes.push_back(std::make_pair(UNITAI_COUNTER, 50));
			invaderTypes.push_back(std::make_pair(UNITAI_ATTACK, 40));
			if (player.AI_isDoStrategy(AI_STRATEGY_AIR_BLITZ))
			{
				invaderTypes.push_back(std::make_pair(UNITAI_PARADROP, 20));
			}

			if (!bImportantCity && (iUnitsToTransport >= (iLocalTransports * iBestSeaAssaultCapacity)))
			{
				// Have time to build barracks first
				if (AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, 10))
				{
					return;
				}
			}

			if (AI_chooseLeastRepresentedUnit("invader units", invaderTypes, iTrainInvaderChance))
			{
				return;
			}

			if (iUnitCostPercentage < (iMaxUnitSpending))
			{
				int iMissileCarriers = player.AI_totalUnitAIs(UNITAI_MISSILE_CARRIER_SEA);

				if (!bFinancialTrouble && iMissileCarriers > 0 && !bImportantCity)
				{
					if ((iProductionRank <= ((player.getNumCities() / 2) + 1)))
					{
						UnitTypes eBestMissileCarrierUnit = NO_UNIT;
						player.AI_bestCityUnitAIValue(UNITAI_MISSILE_CARRIER_SEA, NULL, &eBestMissileCarrierUnit);
						if (eBestMissileCarrierUnit != NO_UNIT)
						{
							FAssert(GC.getUnitInfo(eBestMissileCarrierUnit).getDomainCargo() == DOMAIN_AIR);

							int iMissileCarrierAirNeeded = iMissileCarriers * GC.getUnitInfo(eBestMissileCarrierUnit).getCargoSpace();

							if ((player.AI_totalUnitAIs(UNITAI_MISSILE_AIR) < iMissileCarrierAirNeeded) ||
								(bPrimaryArea && (player.AI_totalAreaUnitAIs(pArea, UNITAI_MISSILE_CARRIER_SEA) * GC.getUnitInfo(eBestMissileCarrierUnit).getCargoSpace() < player.AI_totalAreaUnitAIs(pArea, UNITAI_MISSILE_AIR))))
							{
								// Don't always build missiles, more likely if really low
								if (AI_chooseUnit("need missiles", UNITAI_MISSILE_AIR, (player.AI_totalUnitAIs(UNITAI_MISSILE_AIR) < iMissileCarrierAirNeeded / 2) ? 50 : 20))
								{
									return;
								}
							}
						}
					}
				}
			}
		}
	}

	m_iTempBuildPriority--;

	//#58 planes & aircraft fill carriers
	if (!bInhibitUnits && (bLandWar || bAssault) && iUnitCostPercentage < (iMaxUnitSpending))
	{
		if (iCarriers > 0 && !bImportantCity)
		{
			UnitTypes eBestCarrierUnit = NO_UNIT;
			player.AI_bestCityUnitAIValue(UNITAI_CARRIER_SEA, NULL, &eBestCarrierUnit);
			if (eBestCarrierUnit != NO_UNIT)
			{
				FAssert(GC.getUnitInfo(eBestCarrierUnit).getDomainCargo() == DOMAIN_AIR);

				const int iCarrierAirNeeded = iCarriers * GC.getUnitInfo(eBestCarrierUnit).getCargoSpace();

				// Reduce chances if city gives no air experience
				if (player.AI_totalUnitAIs(UNITAI_CARRIER_AIR) < iCarrierAirNeeded
					&& AI_chooseUnit("need planes for carriers", UNITAI_CARRIER_AIR, iFreeAirExperience > 0 ? -1 : 35))
				{
					return;
				}
			}
		}
	}
	m_iTempBuildPriority--;

	//#59 Nukes prio2
	if (!bInhibitUnits && !bFinancialTrouble && (player.AI_isDoStrategy(AI_STRATEGY_OWABWNW) || GC.getGame().getSorenRandNum(NukeProbRNG, "AI consider Nuke") == 0))
	{
		if (player.AI_totalUnitAIs(UNITAI_ICBM) < iNukesWanted)
		{
			//Reordered, because nukes are more valuable than carriers
			if (AI_chooseUnit("ICBM (2)", UNITAI_ICBM))
			{
				return;
			}

			if (pWaterArea != NULL && AI_chooseUnit("need missile carrier", UNITAI_MISSILE_CARRIER_SEA, 50))
			{
				return;
			}
		}
	}
	m_iTempBuildPriority--;

	//#60 Special Invasion Combo Units (Attack+Count+Attack_city+Paradrop)
	if (/*!bUnitExempt && */!bInhibitUnits && !bAssault && (!bImportantCity || bDefenseWar) && (iUnitCostPercentage < iMaxUnitSpending))
	{
		if (!bFinancialTrouble && (bLandWar || (player.AI_isDoStrategy(AI_STRATEGY_DAGGER) && !bGetBetterUnits)))
		{
			int iTrainInvaderChance = iBuildUnitProb + 10;

			if (bAggressiveAI)
			{
				iTrainInvaderChance += 15;
			}

			if (bGetBetterUnits)
			{
				iTrainInvaderChance /= 2;
			}
			else if (eAreaAI == AREAAI_MASSING || eAreaAI == AREAAI_ASSAULT_MASSING)
			{
				iTrainInvaderChance = (100 - ((100 - iTrainInvaderChance) / (bCrushStrategy ? 6 : 3)));
			}

			if (AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, 10, 0, bDefenseWar ? 10 : 30))
			{
				return;
			}

			UnitTypeWeightArray invaderTypes;
			invaderTypes.push_back(std::make_pair(UNITAI_ATTACK_CITY, 100));
			invaderTypes.push_back(std::make_pair(UNITAI_COUNTER, 50));
			invaderTypes.push_back(std::make_pair(UNITAI_ATTACK, 40));
			invaderTypes.push_back(std::make_pair(UNITAI_PARADROP, (player.AI_isDoStrategy(AI_STRATEGY_AIR_BLITZ) ? 30 : 20) / (bAssault ? 2 : 1)));

			if (player.AI_totalAreaUnitAIs(pArea, UNITAI_PILLAGE) <= ((iNumCitiesInArea + 1) / 2))
			{
				invaderTypes.push_back(std::make_pair(UNITAI_PILLAGE, 30));
			}

			if (AI_chooseLeastRepresentedUnit("invader units (2)", invaderTypes, iTrainInvaderChance))
			{
				return;
			}
		}
	}

	m_iTempBuildPriority--;

	//#61 Sea settlers escort and defense Assault ships
	if (!bInhibitUnits && (pWaterArea != NULL) && !bDefenseWar && !bAssault)
	{
		if (!bFinancialTrouble)
		{
			// Force civs with foreign colonies to build a few assault transports to defend the colonies
			if (player.AI_totalUnitAIs(UNITAI_ASSAULT_SEA) < (player.getNumCities() - iNumCapitalAreaCities) / 3)
			{
				if (AI_chooseUnit("colony defense assault ships", UNITAI_ASSAULT_SEA))
				{
					return;
				}
			}

			if (player.AI_calculateUnitAIViability(UNITAI_SETTLER_SEA, DOMAIN_SEA) < 61)
			{
				// Force civs to build escorts for settler_sea units
				if (player.AI_totalUnitAIs(UNITAI_SETTLER_SEA) > player.AI_getNumAIUnits(UNITAI_RESERVE_SEA))
				{
					if (AI_chooseUnit("sea settler escorts", UNITAI_RESERVE_SEA))
					{
						return;
					}
				}
			}
		}
	}

	m_iTempBuildPriority--;

	//#62 Sea Piracy
	if (!bInhibitUnits && (pWaterArea != NULL) && !bLandWar && !bAssault && !bFinancialTrouble/* && !bUnitExempt*/) //k-mod
	{
		//Arr.  Don't build pirates in financial trouble, as they'll be disbanded with high probability
		int iPirateCount = player.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_PIRATE_SEA);
		int iNeededPirates = (1 + (pWaterArea->getNumTiles() / std::max(1, 200 - iBuildUnitProb)));
		iNeededPirates *= (20 + iWaterPercent);
		iNeededPirates /= 100;

		if (player.isNoForeignTrade())
		{
			iNeededPirates *= 3;
			iNeededPirates /= 2;
		}
		if (player.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_PIRATE_SEA) < iNeededPirates)
		{
			if (player.AI_calculateUnitAIViability(UNITAI_PIRATE_SEA, DOMAIN_SEA) > 49)
			{
				if (AI_chooseUnit("pirates", UNITAI_PIRATE_SEA, iWaterPercent / (1 + iPirateCount)))
				{
					return;
				}
			}
		}
	}

	m_iTempBuildPriority--;

	//#63 Sea Spy
	if (!bInhibitUnits && !bLandWar && !bFinancialTrouble)
	{
		if ((pWaterArea != NULL) && (iWaterPercent > 40))
		{
			if (player.AI_totalAreaUnitAIs(pArea, UNITAI_SPY) > 0)
			{
				if (player.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_SPY_SEA) == 0)
				{
					if (AI_chooseUnit("sea spy", UNITAI_SPY_SEA))
					{
						return;
					}
				}
			}
		}
	}

	m_iTempBuildPriority--;

	//#64 Missionaries Round 3
	if (!bInhibitUnits && iBestSpreadUnitValue > ((iSpreadUnitThreshold * 40) / 100))
	{
		if (AI_chooseUnit(eBestSpreadUnit, UNITAI_MISSIONARY))
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	//#65 floating Defenders (prio2)
	if (!bInhibitUnits && (iTotalFloatingDefenders < iNeededFloatingDefenders && (!bFinancialTrouble || bLandWar)))
	{
		UnitTypeWeightArray floatingDefenderTypes;
		floatingDefenderTypes.push_back(std::make_pair(UNITAI_CITY_DEFENSE, 125));
		floatingDefenderTypes.push_back(std::make_pair(UNITAI_CITY_COUNTER, 100));
		//floatingDefenderTypes.push_back(std::make_pair(UNITAI_CITY_SPECIAL, 0));
		floatingDefenderTypes.push_back(std::make_pair(UNITAI_RESERVE, 100));
		floatingDefenderTypes.push_back(std::make_pair(UNITAI_COLLATERAL, 100));
		if (AI_chooseLeastRepresentedUnit("floating defenders", floatingDefenderTypes, 50))
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	//#66 Spies
	if (!bInhibitUnits && iNumSpies < iNeededSpies)
	{
		//if (AI_chooseUnit("spy", UNITAI_SPY, 5 + 50 / (1 + iNumSpies)))
		if (AI_chooseUnit("spy", UNITAI_SPY, 30 * iNeededSpies / (3 * iNumSpies + iNeededSpies)))
		{
			return;
		}
	}

	//#67 Optional additionnal settlers
	if (!bInhibitUnits && bLandWar && iDangerValue < 3 && iNumSettlers < iMaxSettlers && !bFinancialTrouble)
	{
		if (iAreaBestFoundValue > iMinFoundValue)
		{
			if (AI_chooseUnit("optional settlers", UNITAI_SETTLE))
			{
				return;
			}
		}
	}

	m_iTempBuildPriority--;

	//#68 Oppurtunistic wonder build 3 (rand)
	if (!isHuman() && iProductionRank <= (player.getNumCities() > 8 ? 3 : 2) && getPopulation() > 3)
	{
		int iWonderRand = 8 + GC.getGame().getSorenRandNum(player.getWonderConstructRand(), "Wonder Construction Rand");

		// increase chance of going for an early wonder
		if (GC.getGame().getElapsedGameTurns() < iHammerCostPercent && iNumCitiesInArea > 1)
		{
			iWonderRand *= 35;
			iWonderRand /= 100;
		}
		else if (iNumCitiesInArea >= 3)
		{
			iWonderRand *= 30;
			iWonderRand /= 100;
		}
		else
		{
			iWonderRand *= 25;
			iWonderRand /= 100;
		}

		if (bAggressiveAI)
		{
			iWonderRand *= 2;
			iWonderRand /= 3;
		}

		int iWonderRoll = GC.getGame().getSorenRandNum(100, "Wonder Build Rand");

		if (iProductionRank == 1)
		{
			iWonderRoll /= 2;
		}

		if (iWonderRoll < iWonderRand)
		{
			int iWonderMaxTurns = 20 + ((iWonderRand - iWonderRoll) * 2);
			if (bLandWar)
			{
				iWonderMaxTurns /= 2;
			}

			if (AI_chooseBuilding(BUILDINGFOCUS_WORLDWONDER, iWonderMaxTurns))
			{
				LOG_BBAI_CITY(3, ("#68      City %S uses oppurtunistic wonder build 3", getName().GetCString()));
				return;
			}
		}
	}

	m_iTempBuildPriority--;

	//#69 air units (LeastRepresented)
	if (!bInhibitUnits && iUnitCostPercentage < iMaxUnitSpending + 4 && !bFinancialTrouble)
	{
		if ((iAircraftHave * 2 >= iAircraftNeed) && (iAircraftHave < iAircraftNeed))
		{
			int iOdds = 33;

			if (iFreeAirExperience > 0 || (iProductionRank <= (1 + player.getNumCities() / 2)))
			{
				iOdds = -1;
			}

			if (AI_chooseLeastRepresentedUnit("air units", airUnitTypes, iOdds))
			{
				return;
			}
		}
	}

	m_iTempBuildPriority--;

	//#70 cultural Buildings for pressure/cultural victory 3
	if (!bLandWar)
	{
		const int iCulturePressure = AI_calculateCulturePressure();
		LOG_BBAI_CITY(3, ("#70      City %S uses cultural pressure %d", getName().GetCString(), iCulturePressure));
		if ((iCulturePressure > 90) || player.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2))
		{
			if (!isHuman() || AI_isEmphasizeCommerce(COMMERCE_CULTURE))
			{
				if (AI_chooseBuilding(BUILDINGFOCUS_CULTURE, 20))
				{
					LOG_BBAI_CITY(3, ("#70      City %S uses cultural pressure/cultural victory 3 OK", getName().GetCString()));
					return;
				}
			}
		}

		if (!bInhibitUnits && pWaterArea != NULL && bFinancialTrouble)
		{
			if (player.AI_totalAreaUnitAIs(pArea, UNITAI_MISSIONARY) > 0)
			{
				if (player.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_MISSIONARY_SEA) == 0)
				{
					if (AI_chooseUnit("sea missionary", UNITAI_MISSIONARY_SEA))
					{
						return;
					}
				}
			}
		}
	}

	m_iTempBuildPriority--;

	//#71 cultural Buildings if Focus on Culture
	if (iTargetCulturePerTurn > 0 && !bStrategyTurtle	&& (!isHuman() || AI_isEmphasizeCommerce(COMMERCE_CULTURE))	&& AI_chooseBuilding(BUILDINGFOCUS_CULTURE, 30))
	{
		LOG_BBAI_CITY(3, ("#71      City %S uses choose Culture Building", getName().GetCString()));
		return;
	}

	//#72 Economic Buildings if Focus on iEconomyFlags
	if (AI_chooseBuilding(iEconomyFlags, 15, iEcononmyFlagsThreasholdWeighting / 5))
	{
		LOG_BBAI_CITY(3, ("#72      City %S uses choose iEconomyFlags 2", getName().GetCString()));
		return;
	}

	m_iTempBuildPriority--;

	//#73 Economic Buildings if Focus on iEconomyFlags
	if (!bLandWar)
	{
		if (AI_chooseBuilding(iEconomyFlags, 30, iEcononmyFlagsThreasholdWeighting * 8 / 100))
		{
			LOG_BBAI_CITY(2, ("#73      City %S uses choose iEconomyFlags 3", getName().GetCString()));
			return;
		}

		const int iCulturePressure = AI_calculateCulturePressure();
		if (!isHuman() || AI_isEmphasizeCommerce(COMMERCE_CULTURE))
		{
			if (iCulturePressure > 50)
			{
				if (AI_chooseBuilding(BUILDINGFOCUS_CULTURE, 30))
				{
					LOG_BBAI_CITY(2, ("#73b      City %S uses choose cultural pressure 2", getName().GetCString()));
					return;
				}
			}
		}

		if (!bInhibitUnits && pWaterArea != NULL)
		{
			if (bPrimaryArea)
			{
				if (player.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_EXPLORE_SEA) < std::min(1, player.AI_neededExplorers(pWaterArea)))
				{
					if (AI_chooseUnit("explore sea", UNITAI_EXPLORE_SEA))
					{
						return;
					}
				}
			}
		}

		if (!isHuman() || AI_isEmphasizeYield(YIELD_PRODUCTION))
		{
			if (getPlotYield(YIELD_PRODUCTION) >= 8)
			{
				if (AI_chooseBuilding(BUILDINGFOCUS_PRODUCTION, 40))
				{
					LOG_BBAI_CITY(2, ("#73c      City %S uses choose BUILDINGFOCUS_PRODUCTION 3", getName().GetCString()));
					return;
				}
			}
		}
	}

	m_iTempBuildPriority--;

	//#74 Counter Unit (at least 1)
	if (!bInhibitUnits && plot()->plotCount(PUF_isUnitAIType, UNITAI_CITY_COUNTER, -1, NULL, eOwner, NO_TEAM, NULL, -1, -1, 2) == 0)
	{   	//	If there are no city counter units in the vicinity best create 1
		LOG_BBAI_CITY(2, ("#74      City %S want to build Counter Unit", getName().GetCString()));
		if (AI_chooseUnit("city counter units needed locally", UNITAI_CITY_COUNTER))
		{
			LOG_BBAI_CITY(4, ("#74      City %S Counter Unit ordered", getName().GetCString()));
			return;
		}
	}

	m_iTempBuildPriority--;

	//#75 Pillage Counter Unit (at least 1)
	if (!bInhibitUnits && plot()->plotCount(PUF_isUnitAIType, UNITAI_PILLAGE_COUNTER, -1, NULL, eOwner, NO_TEAM, NULL, -1, -1, 2) == 0)
	{      	//	If there are no pillage counter units in the vicinity best create 1
		LOG_BBAI_CITY(2, ("#75      City %S want to build Pillage Counter Unit", getName().GetCString()));
		if (AI_chooseUnit("pillage counter units needed locally", UNITAI_PILLAGE_COUNTER))
		{
			LOG_BBAI_CITY(4, ("#75      City %S Pillage Counter Unit ordered", getName().GetCString()));
			return;
		}
	}

	m_iTempBuildPriority--;

	//#76 Gold Buildings if bFinancialTrouble and not landwar
	if (!bLandWar && bFinancialTrouble)
	{  	// we do a similar check lower, in the landwar case
		if (!isHuman() || AI_isEmphasizeCommerce(COMMERCE_GOLD))
		{
			LOG_BBAI_CITY(2, ("#76      City %S uses choose financial trouble gold", getName().GetCString()));
			if (AI_chooseBuilding(BUILDINGFOCUS_GOLD))
			{
				LOG_BBAI_CITY(4, ("#76      City %S Building for financial trouble gold ordered", getName().GetCString()));
				return;
			}
		}
	}

	m_iTempBuildPriority--;

	//#77 	low priority property control (bad property > 5% through op range - choose unit to deal if possible)
	if (AI_choosePropertyControlBuildingAndUnit(5, NO_PROPERTY, iMaxPropertyUnitsPercent))
	{
		return;
	}

	m_iTempBuildPriority--;

	//#78 	Healer Unit
	if (!bInhibitUnits)
	{
		LOG_BBAI_CITY(2, ("#78      City %S Build Healer", getName().GetCString()));
		if (AI_chooseHealerUnit())
		{
			LOG_BBAI_CITY(4, ("#78      City %S Healer ordered", getName().GetCString()));
			return;
		}
	}

	m_iTempBuildPriority--;

	//#79 	Infiltrator Unit (3rd)
	if (!bInhibitUnits)
	{
		//harass first
		int iInfiltratorCount = player.AI_totalAreaUnitAIs(pArea, UNITAI_INFILTRATOR);
		if (iInfiltratorCount < 1)
		{
			LOG_BBAI_CITY(2, ("#79      City %S Build Infiltrator", getName().GetCString()));
			//this is enough of a trigger.  Once introduced they will expand themselves if needed and possible.
			if (AI_chooseUnit("Infiltrator needed anywhere", UNITAI_INFILTRATOR))
			{
				LOG_BBAI_CITY(4, ("#79      City %S Infiltrator ordered", getName().GetCString()));
				return;
			}
		}
	}

	m_iTempBuildPriority--;

	//#80  Project (Prio2) if City big enough
	if (!isHuman() && (iProductionRank <= ((player.getNumCities() > 8) ? 3 : 2)) && (getPopulation() > 3))
	{
		if (AI_chooseProject())
		{
			LOG_BBAI_CITY(2, ("#80      City %S uses choose project 2", getName().GetCString()));
			return;
		}
	}

	m_iTempBuildPriority--;

	//#81 Choose Building, by probability
	if (AI_chooseBuilding())
	{
		LOG_BBAI_CITY(2, ("#81      City %S uses choose building by probability", getName().GetCString()));
		return;
	}

	m_iTempBuildPriority--;

	//#82 Attack and Attack City, if Strategy Final War
	if (!bFinancialTrouble && player.AI_isDoStrategy(AI_STRATEGY_FINAL_WAR))
	{
		LOG_BBAI_CITY(2, ("#82      City %S build attack units for Final War", getName().GetCString()));
		if (AI_chooseUnit("final war units", UNITAI_ATTACK))
		{
			return;
		}

		if (AI_chooseUnit("final war units", UNITAI_ATTACK_CITY))
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	//#83 Hunters (prio4)
	if (!bInhibitUnits && !bFinancialTrouble && iHunterDeficitPercent > 0 && GC.getGame().getSorenRandNum(5, "pump hunter") == 0)
	{
		LOG_BBAI_CITY(2, ("#83      City %S build last hunters", getName().GetCString()));
		// If we are just pumping out hunting units and having them die fast go for EXP giving buildings first
		if (AI_chooseExperienceBuilding(UNITAI_HUNTER, 6))
		{
			return;
		}

		if (AI_chooseUnit("need hunters", UNITAI_HUNTER))
		{
			return;
		}
	}

	m_iTempBuildPriority--;

	//#84 extra Attackers
	if (!bFinancialTrouble && !bInhibitUnits)
	{
		//
		int amountWanted = (iNumCitiesInArea * 2) + 1;
		if (bLandWar)
		{
			amountWanted *= 4;
		}
		if (bIsWarMonger)
		{
			amountWanted *= 5;
		}
		if (bAggressiveAI)
		{
			amountWanted *= 3;
		}
		if (bIsPeacefull)
		{
			amountWanted = std::max(6,amountWanted / 20);
		}

		const int iAttackCityCount = player.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK_CITY);
		const int iAttackCount = player.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK);
		int iAttackTarget = amountWanted * 2 / 3;
		int iAttackCityTarget = amountWanted * 1 / 3;

		if (iAttackCityCount + iAttackCount <= amountWanted)
		{
			LOG_BBAI_CITY(2, ("#84 City %S, Will Start to build an Attack Stack, StackRand = %d. For the moment : Attack : %d / %d and Attack_City : %d / %d", getName().GetCString(), amountWanted, iAttackCount, iAttackTarget, iAttackCityCount, iAttackCityTarget));
			if (iAttackCount == 0)
			{
				if (!bFinancialTrouble
					&& AI_chooseUnit("build attack force", UNITAI_ATTACK))
				{
					LOG_BBAI_CITY(4, ("#84 City %S, Unit UNITAI_ATTACK ordered", getName().GetCString()));
					return;
				}
			}
			else if (iAttackCount > 1 && iAttackCityCount == 0)
			{
				if (!bFinancialTrouble && AI_chooseUnit("start city attack stack", UNITAI_ATTACK_CITY))
				{
					LOG_BBAI_CITY(4, ("#84 City %S, Unit UNITAI_ATTACK_CITY ordered", getName().GetCString()));
					return;
				}
			}
			else if (iAttackCityCount < iAttackCityTarget || iAttackCount < iAttackTarget)
			{
				if (iAttackCount < iAttackTarget)
				{
					if ((iAttackCount * 100 / iAttackTarget) <= (iAttackCityCount * 100 / iAttackCityTarget))
					{
						LOG_BBAI_CITY(3, ("#84 City %S, Attack Stack add Attack Unit Order. For the moment : Attack : %d and Attack_City : %d", getName().GetCString(), iAttackCount, iAttackCityCount));
						if (GC.getGame().getSorenRandNum(4, "AI prefer collateral") == 0)
						{
							if (AI_chooseUnit("add Best unit to stack", NO_UNITAI))
							{
								LOG_BBAI_CITY(4, ("#84 City %S, Unit NO_UNITAI ordered", getName().GetCString()));
								return;
							}
						}
						if (AI_chooseUnit("add to attack stack", UNITAI_ATTACK))
						{
							LOG_BBAI_CITY(4, ("#84 City %S, Unit UNITAI_ATTACK ordered", getName().GetCString()));
							return;
						}
					}
				}
				LOG_BBAI_CITY(3, ("#84 City %S, Attack Stack add Attack_City Unit Order. For the moment : Attack : %d and Attack_City : %d", getName().GetCString(), iAttackCount, iAttackCityCount));
				if (GC.getGame().getSorenRandNum(5, "AI prefer collateral") == 0)
				{
					if (AI_chooseUnit("add Best unit to stack", NO_UNITAI))
					{
						LOG_BBAI_CITY(4, ("#84 City %S, Unit NO_UNITAI ordered", getName().GetCString()));
						return;
					}
				}
				if (AI_chooseUnit("add to city attack stack", UNITAI_ATTACK_CITY))
				{
					LOG_BBAI_CITY(4, ("#84 City %S, Unit UNITAI_ATTACK_CITY ordered", getName().GetCString()));
					return;
				}
			}
		}
		else
		{
			LOG_BBAI_CITY(2, ("#84 City %S, Attack Stack big enough : StackRand = %d. For the moment : Attack : %d / %d and Attack_City : %d / %d", getName().GetCString(), amountWanted, iAttackCount, iAttackTarget, iAttackCityCount, iAttackCityTarget));
		}
	}


	if (bFinancialTrouble)
	{
		AI_chooseProcess(COMMERCE_GOLD);
	}


	// Koshling - AI shouldn't choose gold as often as it does. If we have plenty of gold prefer research most of the time.
	const int iCulturePressure = AI_calculateCulturePressure();
	//	Set up weights 0-100 for each commerce type to weight the choice (gold weigth can actuially go higher than
	//	100, but only if we already have less gold than our target in which case we should already have unconditonally
	//	gone for gold earlier in this choice)
	int64_t commerceWeights[NUM_COMMERCE_TYPES];
	commerceWeights[COMMERCE_GOLD] = std::max<int64_t>(0, 100 - (player.getGold() - player.AI_goldTarget()) * 100 / std::max<int64_t>(1, player.AI_goldTarget()));
	commerceWeights[COMMERCE_RESEARCH] = 150; // Can't think of a good measure for research pressure so just set to a middling value
	commerceWeights[COMMERCE_CULTURE] = 100 + iCulturePressure;
	commerceWeights[COMMERCE_ESPIONAGE] = 110;	//	Is this ever really worthwhile?

	if (isHuman())
	{
		if (AI_isEmphasizeCommerce(COMMERCE_GOLD))
		{
			commerceWeights[COMMERCE_GOLD] = commerceWeights[COMMERCE_GOLD] * 3 / 2;
		}
		if (!AI_isEmphasizeCommerce(COMMERCE_RESEARCH))
		{
			commerceWeights[COMMERCE_RESEARCH] = commerceWeights[COMMERCE_RESEARCH] * 3 / 2;
		}
		if (!AI_isEmphasizeCommerce(COMMERCE_CULTURE))
		{
			commerceWeights[COMMERCE_CULTURE] = commerceWeights[COMMERCE_CULTURE] * 3 / 2;
		}
		if (!AI_isEmphasizeCommerce(COMMERCE_ESPIONAGE))
		{
			commerceWeights[COMMERCE_ESPIONAGE] = commerceWeights[COMMERCE_ESPIONAGE] * 3 / 2;
		}
	}

	LOG_BBAI_CITY(2, ("#85      City %S will build under Commerce pressure", getName().GetCString()));
	if (getHeadOrder() == NULL && !AI_chooseProcess(NO_COMMERCE, commerceWeights))
	{
		LOG_BBAI_CITY(2, ("#86     City %S cannot produce anything", getName().GetCString()));
		FErrorMsg(CvString::format("AI could not choose production for city %S", m_szName.c_str()).c_str());
	}
}

// Toffer - iUnitProductionLossesFactor should be bigger than zero;
// higher values means that the unit losses must be higher relative to the city hammer yield for the city to build exp. buildings.
bool CvCityAI::AI_chooseExperienceBuilding(const UnitAITypes eUnitAI, const int iUnitProductionLossesFactor)
{
	int iUnitValue;
	const UnitTypes eBestUnit = AI_bestUnitAI(eUnitAI, iUnitValue);
	if (eBestUnit != NO_UNIT)
	{
		const int perTurnLossesCost =
			(
				area()->getRecentCombatDeathRate(getOwner(), eUnitAI)
				* GC.getUnitInfo(eBestUnit).getProductionCost()
				* GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent()
				/ 10000
				);

		LOG_BBAI_CITY(2, (
				"      City %S evaluating recent hunter deaths - %d per/turn cost vs %d production",
				getName().GetCString(), perTurnLossesCost,
				GET_PLAYER(getOwner()).calculateTotalYield(YIELD_PRODUCTION)
			));
		if (iUnitProductionLossesFactor * perTurnLossesCost > GET_PLAYER(getOwner()).calculateTotalYield(YIELD_PRODUCTION)
			&& AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, 20))
		{
			return true;
		}
	}
	return false;
}

UnitTypes CvCityAI::AI_bestUnit(int& iBestUnitValue, int iNumSelectableTypes, UnitAITypes* pSelectableTypes, bool bAsync, UnitAITypes* peBestUnitAI, bool bNoRand, bool bNoWeighting, const CvUnitSelectionCriteria* criteria)
{
	PROFILE_EXTRA_FUNC();
	iBestUnitValue = 0;

	if (peBestUnitAI != NULL)
	{
		*peBestUnitAI = NO_UNITAI;
	}

	int aiUnitAIVal[NUM_UNITAI_TYPES];
	int iMilitaryWeight = 0;
	if (!bNoWeighting)
	{
		const CvArea* pWaterArea = waterArea(true);

		const bool bWarPlan = GET_TEAM(getTeam()).hasWarPlan(true);
		bool bDefense = (area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE);
		bool bLandWar = (bDefense || (area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE) || (area()->getAreaAIType(getTeam()) == AREAAI_MASSING));
		bool bAssault = (area()->getAreaAIType(getTeam()) == AREAAI_ASSAULT);
		bool bPrimaryArea = GET_PLAYER(getOwner()).AI_isPrimaryArea(area());
		bool bAreaAlone = GET_PLAYER(getOwner()).AI_isAreaAlone(area());
		bool bFinancialTrouble = GET_PLAYER(getOwner()).AI_isFinancialTrouble();
		bool bDanger = AI_isDanger();

		const bool bMetAnyCiv = GET_TEAM(getTeam()).hasMetAnyCiv();
		iMilitaryWeight = GET_PLAYER(getOwner()).AI_militaryWeight(area());
		int iNumCitiesInArea = area()->getCitiesPerPlayer(getOwner());

		const int iCoastalCities = pWaterArea ? GET_PLAYER(getOwner()).countNumCoastalCitiesByArea(pWaterArea) : 0;

		for (int iI = 0; iI < NUM_UNITAI_TYPES; iI++)
		{
			aiUnitAIVal[iI] = 0;
		}

		if (!bFinancialTrouble && ((bPrimaryArea) ? (GET_PLAYER(getOwner()).findBestFoundValue() > 0) : (area()->getBestFoundValue(getOwner()) > 0)))
		{
			aiUnitAIVal[UNITAI_SETTLE]++;
		}

		aiUnitAIVal[UNITAI_WORKER] += GET_PLAYER(getOwner()).AI_neededWorkers(area());

		aiUnitAIVal[UNITAI_ATTACK] += ((iMilitaryWeight / ((bWarPlan || bLandWar || bAssault) ? 7 : 12)) + (bPrimaryArea ? 2 : 0) + 1);

		aiUnitAIVal[UNITAI_CITY_DEFENSE] += (iNumCitiesInArea + 1);
		aiUnitAIVal[UNITAI_CITY_COUNTER] += ((5 * (iNumCitiesInArea + 1)) / 8);
		aiUnitAIVal[UNITAI_CITY_SPECIAL] += ((iNumCitiesInArea + 1) / 2);

		aiUnitAIVal[UNITAI_ATTACK_CITY] += ((iMilitaryWeight / ((bWarPlan || bLandWar || bAssault) ? 10 : 17)) + ((bPrimaryArea) ? 1 : 0));
		aiUnitAIVal[UNITAI_COUNTER] += ((iMilitaryWeight / ((bWarPlan || bLandWar || bAssault) ? 13 : 22)) + ((bPrimaryArea) ? 1 : 0));
		aiUnitAIVal[UNITAI_PARADROP] += ((iMilitaryWeight / ((bWarPlan || bLandWar || bAssault) ? 5 : 8)) + ((bPrimaryArea) ? 1 : 0));

		aiUnitAIVal[UNITAI_DEFENSE_AIR] += (GET_PLAYER(getOwner()).getNumCities() + 1);
		aiUnitAIVal[UNITAI_CARRIER_AIR] += GET_PLAYER(getOwner()).AI_countCargoSpace(UNITAI_CARRIER_SEA);
		aiUnitAIVal[UNITAI_MISSILE_AIR] += GET_PLAYER(getOwner()).AI_countCargoSpace(UNITAI_MISSILE_CARRIER_SEA);

		if (bPrimaryArea)
		{
			aiUnitAIVal[UNITAI_ICBM] += std::max((GET_PLAYER(getOwner()).getTotalPopulation() / 25), ((GC.getGame().countCivPlayersAlive() + GC.getGame().countTotalNukeUnits()) / (GC.getGame().countCivPlayersAlive() + 1)));
		}

		if (isHominid())
		{
			aiUnitAIVal[UNITAI_ATTACK] *= 2;
		}
		else
		{
			if (!bLandWar)
			{
				aiUnitAIVal[UNITAI_EXPLORE] += GET_PLAYER(getOwner()).AI_neededExplorers(area());
			}

			if (pWaterArea != NULL)
			{
				aiUnitAIVal[UNITAI_WORKER_SEA] += AI_neededSeaWorkers();

				if ((GET_PLAYER(getOwner()).getNumCities() > 3) || (area()->getNumUnownedTiles() < 10))
				{
					if (bPrimaryArea)
					{
						aiUnitAIVal[UNITAI_EXPLORE_SEA] += GET_PLAYER(getOwner()).AI_neededExplorers(pWaterArea);
					}

					if (bPrimaryArea && (GET_PLAYER(getOwner()).findBestFoundValue() > 0) && (pWaterArea->getNumTiles() > 300))
					{
						aiUnitAIVal[UNITAI_SETTLER_SEA]++;
					}

					if (bPrimaryArea && (GET_PLAYER(getOwner()).AI_totalAreaUnitAIs(area(), UNITAI_MISSIONARY) > 0) && (pWaterArea->getNumTiles() > 400))
					{
						aiUnitAIVal[UNITAI_MISSIONARY_SEA]++;
					}

					if (bPrimaryArea && (GET_PLAYER(getOwner()).AI_totalAreaUnitAIs(area(), UNITAI_SPY) > 0) && (pWaterArea->getNumTiles() > 500))
					{
						aiUnitAIVal[UNITAI_SPY_SEA]++;
					}

					aiUnitAIVal[UNITAI_PIRATE_SEA] += pWaterArea->getNumTiles() / 600;
					aiUnitAIVal[UNITAI_ATTACK_SEA] += std::min((pWaterArea->getNumTiles() / 150), ((((iCoastalCities * 2) + (iMilitaryWeight / 9)) / ((bAssault) ? 4 : 6)) + ((bPrimaryArea) ? 1 : 0)));
					aiUnitAIVal[UNITAI_RESERVE_SEA] += std::min((pWaterArea->getNumTiles() / 200), ((((iCoastalCities * 2) + (iMilitaryWeight / 7)) / 5) + ((bPrimaryArea) ? 1 : 0)));
					aiUnitAIVal[UNITAI_ESCORT_SEA] += (GET_PLAYER(getOwner()).AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_ASSAULT_SEA) + (GET_PLAYER(getOwner()).AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_CARRIER_SEA) * 2));
					aiUnitAIVal[UNITAI_ASSAULT_SEA] += std::min((pWaterArea->getNumTiles() / 250), ((((iCoastalCities * 2) + (iMilitaryWeight / 6)) / ((bAssault) ? 5 : 8)) + ((bPrimaryArea) ? 1 : 0)));
					aiUnitAIVal[UNITAI_CARRIER_SEA] += std::min((pWaterArea->getNumTiles() / 350), ((((iCoastalCities * 2) + (iMilitaryWeight / 8)) / 7) + ((bPrimaryArea) ? 1 : 0)));
					aiUnitAIVal[UNITAI_MISSILE_CARRIER_SEA] += std::min((pWaterArea->getNumTiles() / 350), ((((iCoastalCities * 2) + (iMilitaryWeight / 8)) / 7) + ((bPrimaryArea) ? 1 : 0)));
				}
			}

			if (bMetAnyCiv && (bLandWar || bAssault || !bFinancialTrouble || GET_PLAYER(getOwner()).getFinalUnitUpkeep() == 0))
			{
				aiUnitAIVal[UNITAI_ATTACK] += ((iMilitaryWeight / ((bLandWar || bAssault) ? 9 : 16)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));
				aiUnitAIVal[UNITAI_ATTACK_CITY] += ((iMilitaryWeight / ((bLandWar || bAssault) ? 7 : 15)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));
				aiUnitAIVal[UNITAI_COLLATERAL] += ((iMilitaryWeight / ((bDefense) ? 8 : 14)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));
				aiUnitAIVal[UNITAI_PILLAGE] += ((iMilitaryWeight / ((bLandWar || bAssault) ? 10 : 19)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));
				aiUnitAIVal[UNITAI_RESERVE] += ((iMilitaryWeight / ((bLandWar) ? 12 : 17)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));
				aiUnitAIVal[UNITAI_COUNTER] += ((iMilitaryWeight / ((bLandWar || bAssault) ? 9 : 16)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));
				aiUnitAIVal[UNITAI_PARADROP] += ((iMilitaryWeight / ((bLandWar || bAssault) ? 4 : 8)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));

				aiUnitAIVal[UNITAI_ATTACK_AIR] += (GET_PLAYER(getOwner()).getNumCities() + 1);

				if (pWaterArea != NULL)
				{
					if ((GET_PLAYER(getOwner()).getNumCities() > 3) || (area()->getNumUnownedTiles() < 10))
					{
						aiUnitAIVal[UNITAI_ATTACK_SEA] += std::min((pWaterArea->getNumTiles() / 100), ((((iCoastalCities * 2) + (iMilitaryWeight / 10)) / ((bAssault) ? 5 : 7)) + ((bPrimaryArea) ? 1 : 0)));
						aiUnitAIVal[UNITAI_RESERVE_SEA] += std::min((pWaterArea->getNumTiles() / 150), ((((iCoastalCities * 2) + (iMilitaryWeight / 11)) / 8) + ((bPrimaryArea) ? 1 : 0)));
					}
				}
			}
		}

		// XXX this should account for air and heli units too...
		for (int iI = 0; iI < NUM_UNITAI_TYPES; iI++)
		{
			if (GET_PLAYER(getOwner()).AI_unitAIDomainType((UnitAITypes)iI) == DOMAIN_SEA)
			{
				if (pWaterArea != NULL)
				{
					aiUnitAIVal[iI] -= GET_PLAYER(getOwner()).AI_totalWaterAreaUnitAIs(pWaterArea, ((UnitAITypes)iI));
				}
			}
			else if ((GET_PLAYER(getOwner()).AI_unitAIDomainType((UnitAITypes)iI) == DOMAIN_AIR) || (iI == UNITAI_ICBM))
			{
				aiUnitAIVal[iI] -= GET_PLAYER(getOwner()).AI_totalUnitAIs((UnitAITypes)iI);
			}
			else
			{
				aiUnitAIVal[iI] -= GET_PLAYER(getOwner()).AI_totalAreaUnitAIs(area(), ((UnitAITypes)iI));
			}
		}

		aiUnitAIVal[UNITAI_SETTLE] *= ((bDanger) ? 8 : 20);
		aiUnitAIVal[UNITAI_WORKER] *= ((bDanger) ? 2 : 7);
		aiUnitAIVal[UNITAI_ATTACK] *= 3;
		aiUnitAIVal[UNITAI_ATTACK_CITY] *= 4;
		aiUnitAIVal[UNITAI_COLLATERAL] *= 5;
		aiUnitAIVal[UNITAI_PILLAGE] *= 3;
		aiUnitAIVal[UNITAI_RESERVE] *= 3;
		aiUnitAIVal[UNITAI_COUNTER] *= 3;
		aiUnitAIVal[UNITAI_CITY_DEFENSE] *= 2;
		aiUnitAIVal[UNITAI_CITY_COUNTER] *= 2;
		aiUnitAIVal[UNITAI_CITY_SPECIAL] *= 2;
		aiUnitAIVal[UNITAI_EXPLORE] *= ((bDanger) ? 6 : 15);
		aiUnitAIVal[UNITAI_ICBM] *= 18;
		aiUnitAIVal[UNITAI_WORKER_SEA] *= ((bDanger) ? 3 : 10);
		aiUnitAIVal[UNITAI_ATTACK_SEA] *= 5;
		aiUnitAIVal[UNITAI_RESERVE_SEA] *= 4;
		aiUnitAIVal[UNITAI_ESCORT_SEA] *= 20;
		aiUnitAIVal[UNITAI_EXPLORE_SEA] *= 18;
		aiUnitAIVal[UNITAI_ASSAULT_SEA] *= 14;
		aiUnitAIVal[UNITAI_SETTLER_SEA] *= 16;
		aiUnitAIVal[UNITAI_MISSIONARY_SEA] *= 12;
		aiUnitAIVal[UNITAI_SPY_SEA] *= 10;
		aiUnitAIVal[UNITAI_CARRIER_SEA] *= 8;
		aiUnitAIVal[UNITAI_MISSILE_CARRIER_SEA] *= 8;
		aiUnitAIVal[UNITAI_PIRATE_SEA] *= 5;
		aiUnitAIVal[UNITAI_ATTACK_AIR] *= 6;
		aiUnitAIVal[UNITAI_DEFENSE_AIR] *= 3;
		aiUnitAIVal[UNITAI_CARRIER_AIR] *= 15;
		aiUnitAIVal[UNITAI_MISSILE_AIR] *= 15;
		aiUnitAIVal[UNITAI_PROPERTY_CONTROL] *= 4;
		aiUnitAIVal[UNITAI_PROPERTY_CONTROL_SEA] *= 3;
		aiUnitAIVal[UNITAI_HEALER] *= 4;
		aiUnitAIVal[UNITAI_HEALER_SEA] *= 4;

		for (int iI = 0; iI < NUM_UNITAI_TYPES; iI++)
		{
			aiUnitAIVal[iI] *= std::max(0, (GC.getLeaderHeadInfo(getPersonalityType()).getUnitAIWeightModifier(iI) + 100));
			aiUnitAIVal[iI] /= 100;
		}
	}

	int iBestValue = 0;
	UnitTypes eBestUnit = NO_UNIT;
	CvWString strUnitAIType;
	CvWString strUnitAITypeWeight;
	CvWString strUnitType;

	for (int iI = 0; iI < NUM_UNITAI_TYPES; iI++)
	{
		bool bValid = iNumSelectableTypes == -1;

		if (!bValid)
		{
			for (int iJ = 0; iJ < iNumSelectableTypes; iJ++)
			{
				if (pSelectableTypes[iJ] == iI)
				{
					bValid = true;
					break;
				}
			}
		}

		if (bValid && (bNoWeighting || aiUnitAIVal[iI] > 0))
		{
			if (!bNoWeighting)
			{
				if (!bNoRand)
				{
					if (bAsync)
					{
						aiUnitAIVal[iI] += GC.getASyncRand().get(iMilitaryWeight, "AI Best UnitAI ASYNC");
					}
					else
					{
						aiUnitAIVal[iI] += GC.getGame().getSorenRandNum(iMilitaryWeight, "AI Best UnitAI");
					}
				}

				if (aiUnitAIVal[iI] < iBestValue)
				{
					bValid = false;
					LOG_CITY_BLOCK(4, {
						strUnitAITypeWeight = GC.getUnitAIInfo((UnitAITypes)iI).getType();
						logAiEvaluations(2, "City %S, lower prio for UnitAI %S, skipping, UnitAI value %d, best UnitAI value %d", getName().GetCString(), strUnitAIType.c_str(), strUnitType.c_str(), aiUnitAIVal[iI], iBestValue);
					});
				}
				else
				{
					LOG_CITY_BLOCK(4, {
						strUnitAITypeWeight = GC.getUnitAIInfo((UnitAITypes)iI).getType();
						logAiEvaluations(2, "City %S, AiWeight for UnitAI %S is valid, UnitAI value %d, best last UnitAI value %d", getName().GetCString(), strUnitAIType.c_str(), strUnitType.c_str(), aiUnitAIVal[iI], iBestValue);
					});
				}
			}

			if (bValid)
			{
				int iUnitValue;

				const UnitTypes eUnit = AI_bestUnitAI((UnitAITypes)iI, iUnitValue, bAsync, bNoRand, criteria);

				if (eUnit != NO_UNIT && AI_meetsUnitSelectionCriteria(eUnit, criteria) && iUnitValue > iBestUnitValue)
				{
					iBestValue = aiUnitAIVal[iI];
					eBestUnit = eUnit;

					if (peBestUnitAI != NULL)
					{
						*peBestUnitAI = ((UnitAITypes)iI);
					}
					LOG_CITY_BLOCK(3, {
						strUnitType = GC.getUnitInfo(eUnit).getType();
						strUnitAIType = GC.getUnitAIInfo((UnitAITypes)iI).getType();
						logAiEvaluations(2, "City %S, Better AI Unit chosen for %S, type %S, %S, unit final value %d, best value %d", getName().GetCString(), strUnitAIType.c_str(), strUnitType.c_str(), GC.getUnitInfo(eUnit).getDescription(), iUnitValue, iBestValue);
					});
					
					iBestUnitValue = iUnitValue;
				}
			}
		}
	}
	LOG_CITY_BLOCK(3, {
		CvWString strCriteria = "None";
		if (criteria != NULL)
		{
			strCriteria = criteria->getDescription();
		}
		if (eBestUnit == NO_UNIT)
		{

			logAiEvaluations(2, "City %S, At the End, No Unit found that meet criteria (%S) in All UNITAI List.", getName().GetCString(), strCriteria.GetCString());
		}
		else
		{
			logAiEvaluations(2, "City %S, At the End, Unit chosen for %S and criteria (%S), type %S, %S, with value %d", getName().GetCString(), strUnitAIType.c_str(), strCriteria.GetCString(), strUnitType.c_str(), GC.getUnitInfo(eBestUnit).getDescription(), iBestValue);
		}
	});

	return eBestUnit;
}


UnitTypes CvCityAI::AI_bestUnitAI(UnitAITypes eUnitAI, int& iBestValue, bool bAsync, bool bNoRand, const CvUnitSelectionCriteria* criteria)
{
	PROFILE_FUNC();

	FAssert(eUnitAI != NO_UNITAI);

	LOG_CITY_BLOCK(3, {
		const CvWString strUnitType = GC.getUnitAIInfo(eUnitAI).getType();
		CvWString strCriteria = "None";
		bool display = false;
		if (criteria != NULL)
		{
			strCriteria = criteria->getDescription();
			display = true;
			if (criteria->m_bIgnoreGrowth)
			{
				display = false;
				logAiEvaluations(4, "City %S, AI_bestUnitAI searching for %S ... - criteria %S - to evaluate Danger", getName().GetCString(), strUnitType.c_str(), strCriteria.GetCString());
			}
		}
		if (display)
		{
			logAiEvaluations(2, "City %S, AI_bestUnitAI searching for %S ... - criteria %S", getName().GetCString(), strUnitType.c_str(), strCriteria.GetCString());
		}
	});


	iBestValue = 0;
	UnitTypes eBestUnit = NO_UNIT;
	CvUnitSelectionCriteria tempCriteria;

	if (criteria != NULL)
	{
		tempCriteria = *criteria;

		LOG_CITY_BLOCK(4, {
			if (tempCriteria.m_eUnitAI != NO_UNITAI)
		{
			const CvWString strUnitType = GC.getUnitAIInfo(tempCriteria.m_eUnitAI).getType();
			logBBAI(" criteria as param :  %S ...- test %d", strUnitType.c_str(), 0);
		}
		});

	}

	if (tempCriteria.m_eUnitAI == NO_UNITAI)
	{
		tempCriteria.m_eUnitAI = eUnitAI;
	}
	else
	{
		FAssert(tempCriteria.m_eUnitAI == eUnitAI);
	}
	CvPlayerAI& player = GET_PLAYER(getOwner());

	bool bGrowMore = false;
	if (!tempCriteria.m_bIgnoreGrowth && foodDifference(true, true) > 0 && angryPopulation(1) < 1)
	{
		// BBAI NOTE: This is where small city worker and settler production is blocked
		// TB Interpret Note: bGrowMore must be FALSE for cities to queue settlers!
		// This was basically saying that if the city has more population than improved tiles then it won't build settlers!
		bGrowMore = getPopulation() < player.getTotalPopulation() / player.getNumCities() && AI_countGoodTiles(true, false, 100) > getPopulation();
	}

	//	Check the cache
	int cacheKey = tempCriteria.getHash();
	std::map<int, UnitValueInfo>::const_iterator itr = m_bestUnits.find(cacheKey);

	if (itr != m_bestUnits.end())
	{
		// Have to recheck canTrain as we may hit a unit limit mid-turn
		if ((itr->second.eUnit == NO_UNIT || !bGrowMore || !isFoodProduction(itr->second.eUnit))
		&& AI_meetsUnitSelectionCriteria(itr->second.eUnit, &tempCriteria) && canTrain(itr->second.eUnit))
		{
			iBestValue = itr->second.iValue;
			eBestUnit = itr->second.eUnit;

			LOG_CITY_BLOCK(4, {
				const CvWString strUnitType = GC.getUnitInfo(eBestUnit).getType();
				const CvWString strUnitAIType = GC.getUnitAIInfo(eUnitAI).getType();
				logAiEvaluations(2, "City %S, Taking Better AI Unit for %S from cache, type %S, %S, base value %d, final value %d", getName().GetCString(), strUnitAIType.c_str(), strUnitType.c_str(), GC.getUnitInfo(eBestUnit).getDescription(), -9999, iBestValue);
			});

			if (UNITAI_CITY_DEFENSE == eUnitAI && eBestUnit == NO_UNIT && (criteria == NULL || criteria->m_eProperty == NO_PROPERTY))
			{
				OutputDebugString("No buildable defender!!\n");
			}
			return eBestUnit;
		}
	}

	for (int iI = GC.getNumUnitInfos() - 1; iI > -1; iI--)
	{
		const UnitTypes eUnitX = static_cast<UnitTypes>(iI);
		if (bGrowMore && isFoodProduction(eUnitX) || !AI_meetsUnitSelectionCriteria(eUnitX, &tempCriteria))
		{
			continue;
		}
		const CvUnitInfo& unit = GC.getUnitInfo(eUnitX);

		if (unit.getNotUnitAIType(eUnitAI)
		|| tempCriteria.m_eIgnoreAdvisor != NO_ADVISOR
		&& tempCriteria.m_eIgnoreAdvisor == unit.getAdvisorType()
		|| !canTrain(eUnitX))
		{
			continue;
		}
		int iValue = player.AI_unitValue(eUnitX, eUnitAI, area(), &tempCriteria);
		int iBaseValue = iValue;

		if (iValue > 0)
		{
			FAssert(MAX_INT / 100 > iValue);
			if (iValue > MAX_INT / 100)
			{
				iValue = MAX_INT / 100;
			}
			// Allow order of magnitude
			iValue *= 100; // Need it multiplying up so that truncation errors don't render.

			iValue += getProductionExperience(eUnitX);

			/* Toffer - Super slow evaluation, needs smart caching to work.

			// KOSHLING - this need rework to take actual promotion values. *** TODO ***
			// May need some caching to do so at appropriate performance levels.
			const int iCombatType = unit.getUnitCombatType();
			int iPromotionValue = 0;
			for (int iJ = GC.getNumPromotionInfos() - 1; iJ > -1; iJ--)
			{
				// Unit
				if (unit.getFreePromotions(iJ))
				{
					iPromotionValue += 15;
					continue;
				}
				// Buildings
				if (isFreePromotion((PromotionTypes)iJ))
				{
					if (iCombatType != NO_UNITCOMBAT && GC.getPromotionInfo((PromotionTypes)iJ).getUnitCombat(iCombatType))
					{
						iPromotionValue += 15;
						continue;
					}
					bool bFound = false;
					foreach_(const UnitCombatTypes eSubCombat, unit.getSubCombatTypes())
					{
						if (GC.getPromotionInfo((PromotionTypes)iJ).getUnitCombat(eSubCombat))
						{
							iPromotionValue += 15;
							bFound = true;
							break;
						}
					}
					if (bFound) continue;
				}
				// Traits
				if (iCombatType != NO_UNITCOMBAT)
				{
					for (int iK = GC.getNumTraitInfos() - 1; iK > -1; iK--)
					{
						if (hasTrait((TraitTypes)iK) && GC.getTraitInfo((TraitTypes)iK).isFreePromotionUnitCombats(iJ, iCombatType))
						{
							iPromotionValue += 15;
							break;
						}
					}
				}
			}
			iValue *= 100 + iPromotionValue;
			iValue /= 100;
			*/

			if (!bNoRand)
			{
				if (bAsync)
				{
					iValue *= (GC.getASyncRand().get(50, "AI Best Unit ASYNC") + 100);
					iValue /= 100;
				}
				else
				{
					iValue *= (GC.getGame().getSorenRandNum(50, "AI Best Unit") + 100);
					iValue /= 100;
				}
			}

			if (unit.isSuicide())
			{
				iValue /= 3; // much of this is compensated
			}

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestUnit = eUnitX;

				LOG_CITY_BLOCK(4, {
					const CvWString strUnitType = GC.getUnitInfo(eUnitX).getType();
					const CvWString strUnitAIType = GC.getUnitAIInfo(eUnitAI).getType();
					CvWString strCriteria = "None";
					bool display = true;
					if (criteria != NULL)
					{
						strCriteria = criteria->getDescription();
						if (criteria->m_bIgnoreGrowth)
						{
							display = false;
						}
					}
					if (display)
					{
						logAiEvaluations(2, "City %S, Better AI Unit found for %S and criteria %S, type %S, %S, base value %d, final value %d", getName().GetCString(), strUnitAIType.c_str(), strCriteria.GetCString(), strUnitType.c_str(), GC.getUnitInfo(eUnitX).getDescription(), iBaseValue * 100, iValue);
					}
				});


			}
			else
			{
				LOG_CITY_BLOCK(4, {
					const CvWString strUnitType = GC.getUnitInfo(eUnitX).getType();
					const CvWString strUnitAIType = GC.getUnitAIInfo(eUnitAI).getType();
					logAiEvaluations(2, "City %S, AI Unit not chosen (not better) for %S, type %S, %S, base value %d, final value %d", getName().GetCString(), strUnitAIType.c_str(), strUnitType.c_str(), GC.getUnitInfo(eUnitX).getDescription(), iBaseValue*100, iValue);
				});
			}
		}
	}

	if (!bGrowMore && !bAsync)
	{
		UnitValueInfo unitValueInfo;

		unitValueInfo.eUnit = eBestUnit;
		unitValueInfo.iValue = iBestValue;

		m_bestUnits.insert(std::make_pair(cacheKey, unitValueInfo));
	}
#ifdef _DEBUG
	if (eBestUnit == NO_UNIT)
	{
		if (UNITAI_CITY_DEFENSE == eUnitAI && (!criteria || criteria->m_eProperty == NO_PROPERTY))
		{
			OutputDebugString("No buildable defender!!\n");
		}
		OutputDebugString("No Buildable Unit for selected AI!!\n");
	}
#endif
	return eBestUnit;
}

BuildingTypes CvCityAI::AI_bestBuildingThreshold(int iFocusFlags, int iMaxTurns, int iMinThreshold, bool bAsync, AdvisorTypes eIgnoreAdvisor, bool bMaximizeFlaggedValue, PropertyTypes eProperty)
{
	const std::vector<ScoredBuilding> scoredBuildings = AI_bestBuildingsThreshold(iFocusFlags, iMaxTurns, iMinThreshold, bAsync, eIgnoreAdvisor, bMaximizeFlaggedValue, eProperty);
	if (!scoredBuildings.empty())
	{
		return scoredBuildings[0].building;
	}
	return NO_BUILDING;
}

const std::vector<CvCity::ScoredBuilding> CvCityAI::AI_bestBuildingsThreshold(int iFocusFlags, int iMaxTurns, int iMinThreshold, bool bAsync, AdvisorTypes eIgnoreAdvisor, bool bMaximizeFlaggedValue, PropertyTypes eProperty)
{
	PROFILE_EXTRA_FUNC();
	std::vector<ScoredBuilding> scoredBuildings;

	std::vector<BuildingTypes> possibles;

	for (int idx = 0; idx < GC.getNumBuildingInfos(); ++idx)
	{
		if (!GET_PLAYER(getOwner()).isBuildingMaxedOut((BuildingTypes)idx, GC.getBuildingInfo((BuildingTypes)idx).getExtraPlayerInstances()))
		{
			if (canConstruct((BuildingTypes)idx))
			{
				possibles.push_back((BuildingTypes)idx);
			}
		}
	}

	AI_scoreBuildingsFromListThreshold(scoredBuildings, possibles, iFocusFlags, iMaxTurns, iMinThreshold, bAsync, eIgnoreAdvisor, bMaximizeFlaggedValue, eProperty);
	return scoredBuildings;
}

bool AI_buildingInfluencesProperty(const CvCity* city, const CvBuildingInfo& buildingInfo, PropertyTypes eProperty)
{
	PROFILE_EXTRA_FUNC();
	bool bFoundValidation = false;
	foreach_(const CvPropertySource * pSource, buildingInfo.getPropertyManipulators()->getSources())
	{
		//	For now we're only interested in constant sources
		//	TODO - expand this as buildings add other types
		if (pSource->getProperty() == eProperty &&
			pSource->getType() == PROPERTYSOURCE_CONSTANT &&
			GC.getPropertyInfo(eProperty).getAIWeight() * static_cast<const CvPropertySourceConstant*>(pSource)->getAmountPerTurn(city->getGameObject()) > 0)
		{
			bFoundValidation = true;
			break;
		}
	}
	return bFoundValidation;
}

bool CvCityAI::AI_scoreBuildingsFromListThreshold(std::vector<ScoredBuilding>& scoredBuildings, const std::vector<BuildingTypes>& possibleBuildings, int iFocusFlags, int iMaxTurns, int iMinThreshold, bool bAsync, AdvisorTypes eIgnoreAdvisor = NO_ADVISOR, bool bMaximizeFlaggedValue = false, PropertyTypes eProperty = NO_PROPERTY)
{
	PROFILE_FUNC();

	// If we want score for the capital building (palace then we just give the production turns)
	if (iFocusFlags & BUILDINGFOCUS_CAPITAL)
	{
		foreach_(const BuildingTypes eBuilding, possibleBuildings)
		{
			FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eBuilding);

			if (GC.getBuildingInfo(eBuilding).isCapital())
			{
				scoredBuildings.push_back(ScoredBuilding(eBuilding, -getProductionTurnsLeft(eBuilding, 0)));
			}
		}

		// Sort from least turns to most
		std::sort(scoredBuildings.rbegin(), scoredBuildings.rend());
		return !scoredBuildings.empty();
	}

	// A couple of shortcuts to make the code more legible (probably the optimier inlines them all but better safe than sorry)
	CvPlayerAI& player = GET_PLAYER(getOwner());

	const bool bAreaAlone = player.AI_isAreaAlone(area());
	const int iProductionRank = findYieldRateRank(YIELD_PRODUCTION);

	foreach_(const BuildingTypes eBuilding, possibleBuildings)
	{
		FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eBuilding);

		const CvBuildingInfo& buildingInfo = GC.getBuildingInfo(eBuilding);

		if (
			!hasBuilding(eBuilding)
		&&	(
				!isLimitedWonder(eBuilding) // Building is not a wonder
				||
				// Or production isn't automated or we aren't considering a wonder (we don't want automated production producing wonders)
				!isProductionAutomated()
				&&
				(
					iFocusFlags == 0
					|| (iFocusFlags & BUILDINGFOCUS_WONDEROK)
					|| (iFocusFlags & BUILDINGFOCUS_WORLDWONDER)
				)
			)
		// adviser is not one we are ignoring
		&&	(eIgnoreAdvisor == NO_ADVISOR || buildingInfo.getAdvisorType() != eIgnoreAdvisor)
		// We can actually build the building
		&&	canConstruct(eBuilding)
		// Automated production doesn't look at buildings with prerequisites?
		&&	(!isProductionAutomated() || buildingInfo.getPrereqNumOfBuildings().empty()))
		{
			int64_t iValue = 0;

			if (!(iFocusFlags & BUILDINGFOCUS_PROPERTY)
				// If we want to build a building that influences a property (crime/pollution/tourism etc.) then
				// invalidate the building if it doesn't influence the property we are interested in
				|| AI_buildingInfluencesProperty(this, buildingInfo, eProperty))
			{
				iValue = AI_buildingValueThreshold(eBuilding, iFocusFlags, iMinThreshold, bMaximizeFlaggedValue);

				// If the building also gives a free building then factor that in as well
				const BuildingTypes eFreeBuilding = buildingInfo.getFreeBuilding();
				if (eFreeBuilding != NO_BUILDING && eFreeBuilding != NO_BUILDING)
				{
					// Add value of the free building taking into account our focus, and scale it by the number of cities that don't
					// yet have the building.
					iValue += (AI_buildingValue(eFreeBuilding, iFocusFlags) * (player.getNumCities() - player.getBuildingCountPlusMaking(eBuilding)));
				}

				LOG_CITY_BLOCK(3, {
					if (iValue > 0)
				{
					//Calvitix, log only Values > 0
						logAiEvaluations(3, "City %S base value for %S (flags %08lx)=%I64d", getName().GetCString(), buildingInfo.getDescription(), iFocusFlags, iValue);
				}
				});

				// If this new building replaces an old one, subtract the old value.
				if (iValue > 0)
				{
					PROFILE("CvCityAI::AI_bestBuildingThreshold.Replacement");

					for (int iI = 0; iI < buildingInfo.getNumReplacedBuilding(); iI++)
					{
						const BuildingTypes eBuildingX = static_cast<BuildingTypes>(buildingInfo.getReplacedBuilding(iI));

						if (isActiveBuilding(eBuildingX))
						{
							PROFILE("AI_bestBuildingThreshold.Replace");

							iValue -= AI_buildingValueThreshold(eBuildingX, iFocusFlags, iMinThreshold, bMaximizeFlaggedValue, true);

							LOG_BBAI_CITY(2, ("    replaces %S - reduce value to %I64d", GC.getBuildingInfo(eBuildingX).getDescription(), iValue));
						}
					}
				}
			}

			// If this building enables the construction of other buildings then we want to rate it higher!
			if (buildingInfo.EnablesOtherBuildings())
			{
				PROFILE("CvCityAI::AI_bestBuildingThreshold.EnablesOthers");

				const CvGameObjectCity* pObject = getGameObject();
				// add the extra building and its bonuses to the override to see if they influence the construct condition of this building
				std::vector<GOMOverride> queries;
				GOMOverride query = { pObject, GOM_BUILDING, eBuilding, true };
				queries.push_back(query);
				query.GOM = GOM_BONUS;
				foreach_(const BonusModifier& pair, buildingInfo.getFreeBonuses())
				{
					query.id = pair.first;
					queries.push_back(query);
				}

				// TODO OPT: convert the masks to vectors so this look is faster
				for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
				{
					if (hasBuilding((BuildingTypes)iI)) continue;

					// check if this building enables the construct condition of another building
					bool bEnablesCondition = false;
					const BoolExpr* condition = GC.getBuildingInfo((BuildingTypes)iI).getConstructCondition();
					if (condition && condition->evaluateChange(pObject, queries) == BOOLEXPR_CHANGE_BECOMES_TRUE)
					{
						bEnablesCondition = true;
					}

					if ((bEnablesCondition || GC.getBuildingInfo((BuildingTypes)iI).isPrereqInCityBuilding(eBuilding) || GC.getBuildingInfo((BuildingTypes)iI).isPrereqOrBuilding(eBuilding))
					&& canConstructInternal((BuildingTypes)iI, false, false, false, true, eBuilding))
					{
						PROFILE("AI_bestBuildingThreshold.Enablement");

						// We only value the unlocked building at 1/2 rate
						iValue += AI_buildingValueThreshold((BuildingTypes)iI, iFocusFlags, 0, false, true) / 2;

						LOG_EVALAI_CITY(3, (3, "    enables %S - increase value to %I64d", GC.getBuildingInfo((BuildingTypes)iI).getDescription(), iValue));
					}
				}
			}

			// If the buildings provides a free area building as well then adjust the score up
			const BuildingTypes eFreeAreaBuilding = buildingInfo.getFreeAreaBuilding();
			if (eFreeAreaBuilding != NO_BUILDING)
			{
				// Score is weighted by the number of cities that don't have the free building
				int weighting = player.getNumCities() - player.getBuildingCountPlusMaking(eFreeAreaBuilding);
				// Scaled by the ratio of cities in the area / total, giving a guess as to how important this area is in general
				weighting = (100 * weighting * area()->getCitiesPerPlayer(getOwner())) / player.getNumCities();
				// If we have none of the free buildings at all then also increase the score weighting
				if (!hasBuilding(eFreeAreaBuilding))
				{
					weighting++;
				}
				// Finally scale by the actual value of the free building based on our focus
				iValue += (AI_buildingValue(eFreeAreaBuilding, iFocusFlags) * weighting) / 100;
			}

			LOG_CITY_BLOCK(3, {
				if (iValue > 0)
			{
				logAiEvaluations(3,"City %S final value for %S (flags %08lx)=%I64d", getName().GetCString(), buildingInfo.getDescription(), iFocusFlags, iValue);
			}
			});

			// If we got here, and the building value is above zero, then it certainly
			// full-filled some of our criteria and is worth evaluating further
			if (iValue > 0)
			{
				FAssert((MAX_INT / 100) >= iValue);
				// If its a wonder and this city is rated high for production relative to our other cities, then we will bump up the score
				if (isWorldWonder(eBuilding) && iProductionRank <= std::min(3, (player.getNumCities() + 2) / 3))
				{
					int wonderScore = bAsync ?
						GC.getASyncRand().get(player.getWonderConstructRand(), "Wonder Construction Rand ASYNC")
						:
						GC.getGame().getSorenRandNum(player.getWonderConstructRand(), "Wonder Construction Rand");

					// We particularly want to build the wonder if we are alone in an area.
					// Probably because aren't as worried about producing combat units?
					if (bAreaAlone)
					{
						wonderScore *= 2;
					}
					iValue += wonderScore;
				}

				// Add a general random factor between 100% to 125%
				iValue *= bAsync
					?
					(GC.getASyncRand().get(25, "AI Best Building ASYNC") + 100)
					:
					(GC.getGame().getSorenRandNum(25, "AI Best Building") + 100);
				iValue /= 100;

				// Add on how much this building is already constructed (could be partially constructed already)
				iValue += getProgressOnBuilding(eBuilding);

				// Factor in how many turns are left to complete this building
				const int iTurnsLeft = getProductionTurnsLeft(eBuilding, 0);

				// Apply final checks based on how many turns to build
				if (iMaxTurns <= 0 || iTurnsLeft <= iMaxTurns || AI_canRushBuildingConstruction(eBuilding))
				{
					FAssert(MAX_INT / 100 >= iValue);
					// Adjust the score based on the turns to complete the building, more turns means lower score
					// As we got this far we definitely consider this building a candidate so we should give it a score of at least 1
					iValue = std::max<int64_t>(1, iValue * 100 / (iTurnsLeft + 3));

					// Add to our list of potential buildings to return later
					scoredBuildings.push_back(ScoredBuilding(eBuilding, iValue));
				}
			}
		}
	}
	// Sort from highest score to lowest. Need to reverse the order of the sort because default sort is ascending.
	std::sort(scoredBuildings.rbegin(), scoredBuildings.rend());

	return !scoredBuildings.empty();
}

bool CvCityAI::AI_canRushBuildingConstruction(BuildingTypes building) const
{
	PROFILE_EXTRA_FUNC();
	for (int iHurry = 0; iHurry < GC.getNumHurryInfos(); ++iHurry)
	{
		if (canHurryBuilding((HurryTypes)iHurry, building) && AI_getHappyFromHurry((HurryTypes)iHurry, building) > 0)
		{
			return true;
		}
	}
	return false;
}


// KOSHLING Mod - pre-calculate and cache building values for all focuses
//	Simple class to hold the calculated values for one building type and city
#ifdef _DEBUG
//#define VALIDATE_BUILDING_CACHE_CONSISTENCY
#endif

class FocusValueSet
{
public:
	FocusValueSet()
	{
		PROFILE_EXTRA_FUNC();
		for (int i = 0; i < NUM_ALL_BUILDINGFOCUS_FLAGS; i++)
		{
			m_focusValues[i] = 0;
		}
	}

	void AccumulateTo(int iFocusIndex, int value)
	{
		FASSERT_BOUNDS(0, NUM_ALL_BUILDINGFOCUS_FLAGS, iFocusIndex);
		m_focusValues[iFocusIndex] += value;
	}

	void AccumulateToAny(int value)
	{
		AccumulateTo(BUILDINGFOUCSINDEX_ANY, value);
	}

	void Accumulate(int iFocusIndex, int value, bool toAny)
	{
		if (toAny)
		{
			AccumulateToAny(value);
		}
		else
		{
			AccumulateTo(iFocusIndex, value);
		}
	}

	void SetValue(int iFocusIndex, int value)
	{
		m_focusValues[iFocusIndex] = value;
	}

	void SetValueAll(int value)
	{
		PROFILE_EXTRA_FUNC();
		for (int i = 0; i < NUM_ALL_BUILDINGFOCUS_FLAGS; i++)
		{
			m_focusValues[i] = value;
		}
	}

	int	m_focusValues[NUM_ALL_BUILDINGFOCUS_FLAGS];
};

class OneBuildingValueCache
{
public:
	OneBuildingValueCache() { }

	void AccumulateTo(int iFocusIndex, int value, bool isThresholdSet)
	{
		if (isThresholdSet)
			m_thresholdSet.AccumulateTo(iFocusIndex, value);
		else m_noThresholdSet.AccumulateTo(iFocusIndex, value);
	}

	void AccumulateToAny(int value, bool isThresholdSet)
	{
		AccumulateTo(BUILDINGFOUCSINDEX_ANY, value, isThresholdSet);
	}

	void Accumulate(int iFocusIndex, int value)
	{
		AccumulateToAny(value, false);
		AccumulateTo(iFocusIndex, value, true);
	}

	void SetValue(int iFocusIndex, int value, bool isThresholdSet)
	{
		if (isThresholdSet)
			m_thresholdSet.SetValue(iFocusIndex, value);
		else m_noThresholdSet.SetValue(iFocusIndex, value);
	}

	void SetValueAll(int value, bool isThresholdSet)
	{
		PROFILE_EXTRA_FUNC();
		for (int i = 0; i < NUM_ALL_BUILDINGFOCUS_FLAGS; i++)
		{
			SetValue(i, value, isThresholdSet);
		}
	}

	FocusValueSet m_thresholdSet;
	FocusValueSet m_noThresholdSet;
};

class BuildingValueCache
{
public:
	explicit BuildingValueCache(CvCityAI* cachedCity)
	{
		m_iCachedFlags = 0;
		m_bIncomplete = false;
	}

	virtual ~BuildingValueCache()
	{
		PROFILE_EXTRA_FUNC();
		std::map<int, OneBuildingValueCache*>::const_iterator itr;

		for (itr = m_buildingValues.begin(); itr != m_buildingValues.end(); ++itr)
		{
			delete itr->second;
		}
	}

	bool HasValues(BuildingTypes eType)
	{
		return (m_buildingValues.find(eType) != m_buildingValues.end());
	}

	int GetValue(BuildingTypes eType, int iFocusFlags, int threshold, bool bMaximizeFlaggedValue)
	{

		PROFILE_EXTRA_FUNC();
		std::map<int, OneBuildingValueCache*>::const_iterator itr = m_buildingValues.find(eType);
		if (itr == m_buildingValues.end())
		{
			//	No cached value
			return -1;
		}
		int iValueThreshold = 0;
		int iValueNoThreshold = 0;
		OneBuildingValueCache* valueCache = itr->second;

		for (int i = 0; i < NUM_REAL_BUILDINGFOCUS_FLAGS; i++)
		{
			if ((iFocusFlags & 1 << i) != 0)
			{
				iValueThreshold += valueCache->m_thresholdSet.m_focusValues[i];
				iValueNoThreshold += valueCache->m_noThresholdSet.m_focusValues[i];
			}
		}

		//	Add in pseudo-flag contributions
		if ((iFocusFlags & (BUILDINGFOCUS_GOLD | BUILDINGFOCUS_RESEARCH)) != 0)
		{
			iValueThreshold += valueCache->m_thresholdSet.m_focusValues[BUILDINGFOCUSINDEX_GOLDANDRESEARCH];
			iValueNoThreshold += valueCache->m_noThresholdSet.m_focusValues[BUILDINGFOCUSINDEX_GOLDANDRESEARCH];
		}
		if ((iFocusFlags & (BUILDINGFOCUS_GOLD | BUILDINGFOCUS_MAINTENANCE)) != 0)
		{
			iValueThreshold += valueCache->m_thresholdSet.m_focusValues[BUILDINGFOCUSINDEX_GOLDANDMAINTENANCE];
			iValueNoThreshold += valueCache->m_noThresholdSet.m_focusValues[BUILDINGFOCUSINDEX_GOLDANDMAINTENANCE];
		}

		int iPass1Value = iValueThreshold + valueCache->m_thresholdSet.m_focusValues[BUILDINGFOUCSINDEX_ANY];
		if (iValueThreshold < threshold)
		{
			iPass1Value = 0;
		}

		int iPass2Value = 0;
		if ((iFocusFlags & BUILDINGFOCUS_CONSIDER_ANY) != 0)
		{
			iPass2Value = valueCache->m_noThresholdSet.m_focusValues[BUILDINGFOUCSINDEX_ANY];
		}
		else if (iPass1Value > 0)
		{
			iPass2Value = iValueNoThreshold + valueCache->m_noThresholdSet.m_focusValues[BUILDINGFOUCSINDEX_ANY];
		}

		//	If we are really seeking an answer specifically on the values from the specific flags
		//	then the rest is just a tie-breaker so dramatically boost the value of the flag-specific
		//	stuff from pass1
		if (bMaximizeFlaggedValue)
		{
			iPass1Value *= 20;
		}
		else
		{
			//	Even when not maximizing with all the extra effects on buildings pass1 was getting a
			//	bit dominated by pass2 so provide a little extra pas1 weight anyway
			iPass1Value *= 2;
		}
		return std::max(0, iPass1Value + iPass2Value);
	}

	OneBuildingValueCache* GetOneBuildingCache(int eBuilding)
	{
		OneBuildingValueCache* result;

		std::map<int, OneBuildingValueCache*>::const_iterator itr = m_buildingValues.find(eBuilding);
		if (itr == m_buildingValues.end())
		{
			//	New entry
			result = new OneBuildingValueCache();

			m_buildingValues[eBuilding] = result;
			return result;
		}
		return itr->second;
	}

	//	One city is cached at a time
	int			m_iCachedFlags;
	bool		m_bIncomplete;
	std::map<int, OneBuildingValueCache*>	m_buildingValues;
};



int CvCityAI::AI_buildingValue(BuildingTypes eBuilding, int iFocusFlags, bool bForTech, bool bDebug)
{
	if (bDebug)
	{
		return AI_buildingValueThresholdOriginalUncached(eBuilding, 0, -123, false, true);
	}

	if (bForTech)
	{
		PROFILE("AI_buildingValue.ForTech");
		int iValue = AI_buildingValueThresholdOriginal(eBuilding, iFocusFlags, 0, false, false, bForTech);
		if (iValue > 0 && !isHuman())
		{
			const CvPlayerAI& kOwner = GET_PLAYER(getOwner());
			const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
			const int numFlavors = GC.getNumFlavorTypes();
			for (int i = 0; i < numFlavors; ++i)
			{
				iValue += kOwner.AI_getFlavorValue(static_cast<FlavorTypes>(i)) * kBuilding.getFlavorValue(i);
			}
			return std::max(1, iValue);
		}
		return iValue;
	}

	// Toffer - Unsure why we don't cache this as well.
	// Post process value with leader flavour.
	int iValue = AI_buildingValueThreshold(eBuilding, iFocusFlags, 0);
	if (iValue > 0 && !isHuman())
	{
		const CvPlayerAI& kOwner = GET_PLAYER(getOwner());
		const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
		const int numFlavors = GC.getNumFlavorTypes();
		for (int i = 0; i < numFlavors; ++i)
		{
			iValue += kOwner.AI_getFlavorValue(static_cast<FlavorTypes>(i)) * kBuilding.getFlavorValue(i);
		}
		return std::max(1, iValue); // flavour can't make building worthless.
	}
	return iValue;
}

int CvCityAI::AI_buildingValueThreshold(BuildingTypes eBuilding, int iFocusFlags, int iThreshold, bool bMaximizeFlaggedValue, bool bIgnoreCanConstruct)
{
	PROFILE_FUNC();

	// Other requests (with 0 flags) can occur occassionally from other areas (such as civic evaluation) without implying a lot of locality of reference that makes the caching worthwhile.
	if (!bIgnoreCanConstruct && !canConstruct(eBuilding) || cachedBuildingValues == NULL && iFocusFlags == 0)
	{
		PROFILE("AI_buildingValueThreshold.CacheMiss");

		return AI_buildingValueThresholdOriginal(eBuilding, iFocusFlags, iThreshold);
	}
	// We only pre-calculate and cache values for buildings this city can build.
	// Only trigger a change in cache from iFocusFlags != 0 which happens at the start of building search to choose construction.
	int iResult = -1;

	if (cachedBuildingValues == NULL)
	{
		OutputDebugString(CvString::format("Rebuilding building value cache for City %S\n", getName().GetCString()).c_str());
		cachedBuildingValues = new BuildingValueCache(this);
	}

	if (iFocusFlags == 0)
	{
		iFocusFlags = BUILDINGFOCUS_CONSIDER_ANY;
	}

	if ((cachedBuildingValues->m_iCachedFlags & iFocusFlags) != iFocusFlags)
	{
		CalculateAllBuildingValues(~cachedBuildingValues->m_iCachedFlags & iFocusFlags);
	}

retry:
	iResult = cachedBuildingValues->GetValue(eBuilding, iFocusFlags, iThreshold, bMaximizeFlaggedValue);

	if (iResult == -1)
	{
		if (cachedBuildingValues->m_bIncomplete)
		{
			const int iCachedFlags = cachedBuildingValues->m_iCachedFlags;
			cachedBuildingValues->m_iCachedFlags = 0;

			CalculateAllBuildingValues(iCachedFlags);

			cachedBuildingValues->m_bIncomplete = false;
			goto retry;
		}
		if (!bIgnoreCanConstruct)
		{
			return 0;
		}
		return AI_buildingValueThresholdOriginal(eBuilding, iFocusFlags, iThreshold, bMaximizeFlaggedValue);
	}
	// Note: this assert trigger a lot.
#ifdef VALIDATE_BUILDING_CACHE_CONSISTENCY
	FAssertMsg(AI_buildingValueThresholdOriginal(eBuilding, iFocusFlags, iThreshold) == iResult, "Cached building value result mismatch!!\n");
#endif
	return iResult;
}

// XXX should some of these count cities, buildings, etc. based on teams (because wonders are shared...)
// XXX in general, this function needs to be more sensitive to what makes this city unique (more likely to build airports if there already is a harbor...)
//
// KOSHLING - this routine is no longer used for most building evaluations during city production determination.
//	For efficiency reasons this now pre-calculates most of the needed values and then assembles the final result from cached partial results.
//	For this reason if you are making changes to this routine you must also make equivalent changes to CalculateAllBuildingValues() also in this file
//	It mirriors the calculations in a fairly obvious and direct way.
int CvCityAI::AI_buildingValueThresholdOriginal(BuildingTypes eBuilding, int iFocusFlags, int iThreshold, bool bMaximizeFlaggedValue, bool bIgnoreCanBuildReplacement, bool bForTech)
{
	PROFILE_FUNC();

	// We keep a small cache of results for cases not involved in city production (that therefore still use this routine)
	CvChecksum cacheKey;

	cacheKey.add((int)eBuilding);
	cacheKey.add(iFocusFlags);
	cacheKey.add(iThreshold);
	cacheKey.add(bMaximizeFlaggedValue);
	cacheKey.add(bIgnoreCanBuildReplacement);
	cacheKey.add(bForTech);

	std::map<int, int>::const_iterator itr = m_buildValueCache.find(cacheKey.get());
	if (itr != m_buildValueCache.end())
	{
		return itr->second;
	}
	const int iResult = AI_buildingValueThresholdOriginalUncached(eBuilding, iFocusFlags, iThreshold, bMaximizeFlaggedValue, bIgnoreCanBuildReplacement, bForTech);

	m_buildValueCache.insert(std::make_pair(cacheKey.get(), iResult));

	return iResult;
}

int CvCityAI::AI_buildingValueThresholdOriginalUncached(BuildingTypes eBuilding, int iFocusFlags, int iThreshold, bool bMaximizeFlaggedValue, bool bIgnoreCanBuildReplacement, bool bForTech)
{
	PROFILE_FUNC();

	const CvPlayerAI& kOwner = GET_PLAYER(getOwner());
	const CvTeamAI& kTeam = GET_TEAM(getTeam());

	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	int iLimitedWonderLimit = limitedWonderLimit(eBuilding);
	bool bIsLimitedWonder = (iLimitedWonderLimit >= 0);

	const ReligionTypes eStateReligion = kOwner.getStateReligion();

	const CvArea* pArea = area();
	bool bAreaAlone = kOwner.AI_isAreaAlone(pArea);
	const bool bMetAnyCiv = GET_TEAM(getTeam()).hasMetAnyCiv();

	int iFoodDifference = foodDifference(false);

	// Reduce reaction to espionage induced happy/health problems
	int iHappinessLevel = happyLevel() - unhappyLevel(1) + getEspionageHappinessCounter() / 2;
	int iAngryPopulation = range(-iHappinessLevel, 0, (getPopulation() + 1));
	int iHealthLevel = goodHealth() - badHealth(/*bNoAngry*/ false, std::max(0, (iHappinessLevel + 1) / 2)) + getEspionageHealthCounter() / 2;

	int iHappyModifier = (iHappinessLevel <= iHealthLevel && iHappinessLevel <= 6) ? 6 : 3;
	if (iHappinessLevel >= 10)
	{
		iHappyModifier = 1;
	}
	const int iGoldValueAssessmentModifier = kOwner.AI_goldValueAssessmentModifier();

	//Don't consider a building if it causes the city to immediately start shrinking from unhealthiness
	//For that purpose ignore bad health and unhappiness from Espionage.
	int iBuildingActualHappiness = getAdditionalHappinessByBuilding(eBuilding);
	int iBuildingActualHealth = getAdditionalHealthByBuilding(eBuilding);
	int iBaseHappinessLevel = happyLevel() - unhappyLevel() + getEspionageHappinessCounter();
	int iBaseHealthLevel = goodHealth() - badHealth() + getEspionageHealthCounter();

	int iBaseFoodDifference = getYieldRate(YIELD_FOOD) - getFoodConsumedByPopulation() - std::max(0, -iHealthLevel);

	int iBadHealthFromBuilding = std::max(0, -iBuildingActualHealth);
	int iUnhealthyPopulationFromBuilding = std::min(0, -iBaseHealthLevel) + iBadHealthFromBuilding;

	// Allow a bit of shrinking:
	// Population is expendable if angry, working a bad tile, or running a not-so-good specialist
	int iAllowedShrinkRate =
	(
		getFoodConsumedPerPopulation100()
		*
		(
			std::max(0, -iBaseHappinessLevel - getPopulation() * getAngerPercent() / GC.getPERCENT_ANGER_DIVISOR())
			+
			std::min(1, std::max(0, getWorkingPopulation() - AI_countGoodTiles(true, false, 50)))
			+
			std::max(0, visiblePopulation() - AI_countGoodSpecialists(false))
			)
		/ 100
	);
	if (iUnhealthyPopulationFromBuilding > 0 && (iBaseFoodDifference + iAllowedShrinkRate < iUnhealthyPopulationFromBuilding))
	{
		return 0;
	}
	const int iTotalPopulation = kOwner.getTotalPopulation();

	int iNumCities = kOwner.getNumCities();
	int iNumCitiesInArea = pArea->getCitiesPerPlayer(getOwner());

	int aiYieldRank[NUM_YIELD_TYPES];
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		aiYieldRank[iI] = MAX_INT;
	}

	int aiCommerceRank[NUM_COMMERCE_TYPES];
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		aiCommerceRank[iI] = MAX_INT;
	}

	int aiBaseCommerceRate[NUM_COMMERCE_TYPES];
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		aiBaseCommerceRate[iI] = MAX_INT;
	}

	int aiPlayerCommerceRate[NUM_COMMERCE_TYPES];
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		aiPlayerCommerceRate[iI] = kOwner.getTotalCityBaseCommerceRate((CommerceTypes)iI);
	}

	aiYieldRank[YIELD_PRODUCTION] = findBaseYieldRateRank(YIELD_PRODUCTION);
	bool bIsHighProductionCity = (aiYieldRank[YIELD_PRODUCTION] <= std::max(3, (iNumCities / 2)));

	int iCultureRank = findCommerceRateRank(COMMERCE_CULTURE);
	int iCulturalVictoryNumCultureCities = GC.getGame().culturalVictoryNumCultureCities();

	bool bFinancialTrouble = kOwner.AI_isFinancialTrouble();

	bool bCulturalVictory1 = kOwner.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE1);
	bool bCulturalVictory2 = kOwner.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2);
	bool bCulturalVictory3 = kOwner.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3);

	//bool bSpaceVictory1 = kOwner.AI_isDoVictoryStrategy(AI_VICTORY_SPACE1);

	int iValue = 0;

	{
		PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.MainBody");

		int* paiFreeSpecialistCommerce = new int[NUM_COMMERCE_TYPES];
		int* paiFreeSpecialistYield = new int[NUM_YIELD_TYPES];
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			paiFreeSpecialistYield[iI] = 0;
		}
		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			paiFreeSpecialistCommerce[iI] = 0;
		}

		int iSpecialistGreatPeopleRate = 0;
		int iSpecialistExtraHealth = 0;
		int iSpecialistExtraHappy = 0;

		if (kBuilding.getNumPopulationEmployed() > 0)
		{
			removeWorstCitizenActualEffects(kBuilding.getNumPopulationEmployed(), iSpecialistGreatPeopleRate, iSpecialistExtraHappy, iSpecialistExtraHealth, paiFreeSpecialistYield, paiFreeSpecialistCommerce);
		}

		int aiFreeSpecialistYield[NUM_YIELD_TYPES];
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			aiFreeSpecialistYield[iI] = paiFreeSpecialistYield[iI];
		}

		int aiFreeSpecialistCommerce[NUM_COMMERCE_TYPES];
		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			aiFreeSpecialistCommerce[iI] = paiFreeSpecialistCommerce[iI];
		}

		SAFE_DELETE_ARRAY(paiFreeSpecialistCommerce);
		SAFE_DELETE_ARRAY(paiFreeSpecialistYield);

		for (int iI = 1; iI < kBuilding.getFreeSpecialist() + 1; iI++)
		{
			const SpecialistTypes eNewSpecialist = getBestSpecialist(iI);
			if (eNewSpecialist == NO_SPECIALIST) break;
			const CvSpecialistInfo& kSpecialist = GC.getSpecialistInfo(eNewSpecialist);
			for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				aiFreeSpecialistYield[iJ] += kOwner.specialistYield(eNewSpecialist, (YieldTypes)iJ);
			}
			for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				aiFreeSpecialistCommerce[iJ] += kOwner.specialistCommerce(eNewSpecialist, (CommerceTypes)iJ);
			}
			iSpecialistGreatPeopleRate += kSpecialist.getGreatPeopleRateChange();
			iSpecialistExtraHealth += kSpecialist.getHealthPercent();
			iSpecialistExtraHappy += kSpecialist.getHappinessPercent();
		}
		iSpecialistExtraHappy /= 100;
		iSpecialistExtraHealth /= 100;
		iBuildingActualHappiness += iSpecialistExtraHappy;
		iBuildingActualHealth += iSpecialistExtraHealth;

		bool bCanPopRush = kOwner.canPopRush();

		bool bForeignTrade = false;
		int iNumTradeRoutes = getTradeRoutes();
		for (int iI = 0; iI < iNumTradeRoutes; ++iI)
		{
			PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.TradePartners");

			const CvCity* pTradeCity = getTradeCity(iI);
			if (NULL != pTradeCity)
			{
				if (GET_PLAYER(pTradeCity->getOwner()).getTeam() != getTeam() || pTradeCity->area() != pArea)
				{
					bForeignTrade = true;
					break;
				}
			}
		}

		if (kBuilding.isCapital())
		{
			return 0;
		}

		if (!bIgnoreCanBuildReplacement)
		{
			PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Replacements");

			for (int iI = 0; iI < kBuilding.getNumReplacementBuilding(); ++iI)
			{
				if (canConstruct((BuildingTypes)kBuilding.getReplacementBuilding(iI)))
				{
					return 0;
				}
			}
		}

		foreach_(const ReligionModifier & pair, kBuilding.getReligionChanges())
		{
			if (!kTeam.hasHolyCity(pair.first))
			{
				return 0;
			}
		}

		int iPass1Value = 0;

		for (int iPass = 0; iPass < 2; iPass++)
		{
			if (iFocusFlags != 0 && iValue < 1 && iPass > 0)
			{
				continue;
			}

			if (iPass == 1)
			{
				iPass1Value = iValue;
			}
			iValue -= kBuilding.getInsidiousness();
			iValue += kBuilding.getInvestigation();

			if ((iFocusFlags & BUILDINGFOCUS_WORLDWONDER) || (iPass > 0))
			{
				if (isWorldWonder(eBuilding) && aiYieldRank[YIELD_PRODUCTION] <= 3)
				{
					iValue += 10;
				}
			}

			if ((iFocusFlags & BUILDINGFOCUS_DEFENSE) || (iPass > 0))
			{
				PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Defense");

				if (!bAreaAlone)
				{
					if ((GC.getGame().getBestLandUnit() == NO_UNIT) || !(GC.getUnitInfo(GC.getGame().getBestLandUnit()).isIgnoreBuildingDefense()))
					{
						iValue += (std::max(0, std::min(((kBuilding.getDefenseModifier() + getBuildingDefense()) - getNaturalDefense() - 10), kBuilding.getDefenseModifier())) / 4);

						for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
						{
							if (hasBonus((BonusTypes)iI))
							{
								iValue += (kBuilding.getBonusDefenseChanges(iI) / 4);
							}
						}
					}


					if (GC.getGame().isOption(GAMEOPTION_UNSUPPORTED_ZONE_OF_CONTROL))
					{
						iValue += kBuilding.isZoneOfControl() ? 50 : 0;
					}

					iValue += kBuilding.getLineOfSight() * 15;

					foreach_(const UnitCombatModifier2 & modifier, kBuilding.getUnitCombatExtraStrength())
					{
						const int iValidUnitCount = algo::count_if(
							plot()->units(),
							bind(CvUnit::getTeam, _1) == getTeam() &&
							bind(CvUnit::getUnitCombatType, _1) == modifier.first
						);
						iValue += iValidUnitCount * modifier.second / 6;
					}

					bool bDefense = pArea->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE;
					bool bLandWar = bDefense || pArea->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE || pArea->getAreaAIType(getTeam()) == AREAAI_MASSING;
					bool bDanger = AI_isDanger();

					if (bDanger || bLandWar)
					{
						iValue += kBuilding.getAdjacentDamagePercent() * 10;
					}


					iValue += kBuilding.isProtectedCulture() ? 50 : 0;
					iValue += kBuilding.getOccupationTimeModifier() / 20;

					if (kBuilding.getNoEntryDefenseLevel() > 0 && kBuilding.getNoEntryDefenseLevel() < getTotalDefense(false))
					{
						iValue += getTotalDefense(false) - kBuilding.getNoEntryDefenseLevel() / 2;
					}
					if (kBuilding.getNumUnitFullHeal() > 0)
					{
						iValue += kBuilding.getNumUnitFullHeal() * 50;
					}

					iValue += kBuilding.getBombardDefenseModifier() / 4;

					if (GC.getGame().isOption(GAMEOPTION_COMBAT_SURROUND_DESTROY))
					{
						iValue += kBuilding.getLocalDynamicDefense() / 2;
					}
#ifdef STRENGTH_IN_NUMBERS
					if (GC.getGame().isOption(GAMEOPTION_COMBAT_STRENGTH_IN_NUMBERS))
					{
						iValue += kBuilding.getFrontSupportPercentModifier() / 4;
						iValue += kBuilding.getShortRangeSupportPercentModifier() / 4;
						iValue += kBuilding.getMediumRangeSupportPercentModifier() / 4;
						iValue += kBuilding.getLongRangeSupportPercentModifier() / 4;
						iValue += kBuilding.getFlankSupportPercentModifier() / 4;
					}
#endif
					iValue += kBuilding.getLocalCaptureProbabilityModifier() / 6;
					iValue += kBuilding.getLocalCaptureResistanceModifier() / 3;
					iValue -= kBuilding.getRiverDefensePenalty() / 2;
					iValue += kBuilding.getLocalRepel();
					iValue += kBuilding.getMinDefense();
					iValue += kBuilding.getBuildingDefenseRecoverySpeedModifier() / 20;
					iValue += kBuilding.getCityDefenseRecoverySpeedModifier() / 5;
				}

				iValue += kBuilding.getNationalCaptureResistanceModifier();

				//TB Note: Once we have improved methods of evaluating the value of a given combat class and an ability to evaluate the strength
				//of a given combat class as it exists in volume among perceived or real enemies, we need to update the following to something
				//more intricate and accurate.  These are just semi-sufficient patches 'for now'.
				for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
				{
					iValue += kBuilding.getUnitCombatRepelModifier(iI) / 2;
					iValue += kBuilding.getUnitCombatRepelAgainstModifier(iI) / 2;
					iValue += kBuilding.getUnitCombatDefenseAgainstModifier(iI) / 2;
					if (kBuilding.isMayDamageAttackingUnitCombatType(iI))
					{
						iValue += (kBuilding.getDamageAttackerChance() * kBuilding.getDamageToAttacker()) / 4;
					}
				}
				if (kBuilding.isDamageAllAttackers())
				{
					iValue += (kBuilding.getDamageAttackerChance() * kBuilding.getDamageToAttacker());
				}

				iValue += -kBuilding.getAirModifier() / 4;
				iValue += -kBuilding.getNukeModifier() / 4;

				iValue += ((kBuilding.getAllCityDefenseModifier() * iNumCities) / 5);

				iValue += kBuilding.getAirlift() * 25;

				if (kBuilding.isAreaBorderObstacle() && !GC.getGame().isOption(GAMEOPTION_BARBARIAN_NONE))
				{
					int iTempValue = iNumCitiesInArea * 3 / 2;
					//The great wall is much more valuable with more barbarian activity.
					if (GC.getGame().isOption(GAMEOPTION_BARBARIAN_RAGING))
					{
						iTempValue *= 2;
					}
					iValue += iTempValue;
				}
			}

			if ((iFocusFlags & BUILDINGFOCUS_ESPIONAGE) || (iPass > 0))
			{
				iValue += kBuilding.getEspionageDefenseModifier() / 8;
			}


			if (((iFocusFlags & BUILDINGFOCUS_HAPPY) || iPass > 0)
			// If we're evaluating a building we already have (e.g. - for civic enabling/disabling)
			//	and it gives no unhealthy and that's the reason we have it, count it!
			&& (!isNoUnhappiness() || isActiveBuilding(eBuilding) && kBuilding.isNoUnhappiness()))
			{
				PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Happy");

				int iBestHappy = 0;
				for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
				{
					if (canHurryBuilding((HurryTypes)iI, eBuilding))
					{
						const int iHappyFromHurry = AI_getHappyFromHurry((HurryTypes)iI, eBuilding);
						if (iHappyFromHurry > iBestHappy)
						{
							iBestHappy = iHappyFromHurry;
						}
					}
				}
				iValue += iBestHappy * 10;

				//Fuyu ToDo: How to handle Globe Theater national wonder?
				//For now just give massive boost if city is high food yet not one of the main production or commerce cities
				if (kBuilding.isNoUnhappiness() && bIsLimitedWonder)
				{
					iValue += (iAngryPopulation * 10) + getPopulation();
					aiYieldRank[YIELD_FOOD] = findBaseYieldRateRank(YIELD_FOOD);

					if (aiYieldRank[YIELD_FOOD] <= (2 + iLimitedWonderLimit))
					{
						aiYieldRank[YIELD_COMMERCE] = findBaseYieldRateRank(YIELD_COMMERCE);

						if (!isNationalWonder(eBuilding)
							|| aiYieldRank[YIELD_PRODUCTION] > 2 + iLimitedWonderLimit
							&& aiYieldRank[YIELD_COMMERCE] > 2 + iLimitedWonderLimit)
						{
							iValue += getPopulation() * 5;
						}
					}
				}
				else
				{
					iValue += happynessValue(iBuildingActualHappiness, iBaseHappinessLevel, iBaseHealthLevel);
				}

				iValue += (-kBuilding.getHurryAngerModifier() * getHurryPercentAnger()) / 100;

				for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
				{
					iValue += (kBuilding.getCommerceHappiness(iI) * iHappyModifier) / 4;
				}

				int iWarWearinessModifer = kBuilding.getWarWearinessModifier();
				if (iWarWearinessModifer != 0)
				{
					if (!kTeam.isAtWar())
					{
						iWarWearinessModifer /= 2;
					}
					iValue -= iWarWearinessModifer * iHappyModifier / 16;
				}

				iValue += (kBuilding.getAreaHappiness() * (iNumCitiesInArea - 1) * 8);
				iValue += (kBuilding.getGlobalHappiness() * (iNumCities - 1) * 8);

				int iWarWearinessPercentAnger = kOwner.getWarWearinessPercentAnger();
				int iGlobalWarWearinessModifer = kBuilding.getGlobalWarWearinessModifier();
				if (iGlobalWarWearinessModifer != 0)
				{
					iValue += (-(((iGlobalWarWearinessModifer * iWarWearinessPercentAnger / 100) / GC.getPERCENT_ANGER_DIVISOR())) * (iNumCities - 1));
					iValue += (-iGlobalWarWearinessModifer * iHappyModifier) / 16;
				}

				foreach_(const BuildingModifier2 & pair, kBuilding.getBuildingHappinessChanges())
				{
					iValue += (pair.second * (kOwner.getBuildingCount(pair.first) - hasBuilding(pair.first)) * 8);
				}

				if (GC.getGame().isOption(GAMEOPTION_UNSUPPORTED_REVOLUTION))
				{
					if (kBuilding.getRevIdxLocal() != 0)
					{
						int localRevIdx = getLocalRevIndex();

						//	Use the more serious of the before and after values if this building were to be built
						if (kBuilding.getRevIdxLocal() > 0)
						{
							localRevIdx += kBuilding.getRevIdxLocal();
						}
						//	Treat instability seriously as it goes up - not just linear
						int localRevScaling = (localRevIdx < 0 ? 0 : std::min(localRevIdx * localRevIdx / 50 + localRevIdx / 2, 100));

						iValue -= (kBuilding.getRevIdxLocal() * localRevScaling) / 4;
					}
					if (kBuilding.getRevIdxNational() != 0)
					{
						iValue -= (8 * kOwner.getNumCities()) * kBuilding.getRevIdxNational();
					}
					if (kBuilding.getRevIdxDistanceModifier() != 0 && (!isCapital()))
					{
						const CvCity* pCapital = kOwner.getCapitalCity();
						if (pCapital != NULL)
						{
							int iCapitalDistance = ::plotDistance(getX(), getY(), pCapital->getX(), pCapital->getY());
							int iOldCapitalDistance = iCapitalDistance;
							iCapitalDistance *= 100 + kBuilding.getRevIdxDistanceModifier();
							iCapitalDistance /= 100;

							iValue += ((iOldCapitalDistance - iCapitalDistance) * (10 + std::max(0, getLocalRevIndex())));
						}
					}
				}
			}

			if (((iFocusFlags & BUILDINGFOCUS_HEALTHY) || iPass > 0)
			// If we're evaluating a building we already have (e.g. - for civic enabling/disabling)
			//	and it gives no unhealthy and that's the reason we have it, count it!
			&& (!isNoUnhealthyPopulation() || isActiveBuilding(eBuilding) && kBuilding.isNoUnhealthyPopulation()))
			{
				PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Healthy");

				iValue += healthValue(iBuildingActualHealth, iBaseHappinessLevel - (getEspionageHappinessCounter() / 2) + std::max(0, iBuildingActualHappiness), iBaseHealthLevel, iBaseFoodDifference);

				iValue += (kBuilding.getAreaHealth() * (iNumCitiesInArea - 1) * 4);
				iValue += (kBuilding.getGlobalHealth() * (iNumCities - 1) * 4);
			}

			if ((iFocusFlags & BUILDINGFOCUS_EXPERIENCE) || (iPass > 0))
			{
				PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Experience");

				iValue += (kBuilding.getFreeExperience() * (bMetAnyCiv ? 12 : 6));

				foreach_(const UnitCombatModifier2 & modifier, kBuilding.getUnitCombatFreeExperience())
				{
					if (canTrain(modifier.first))
					{
						iValue += modifier.second * (bMetAnyCiv ? 6 : 3);
					}
				}

				for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
				{
					if (kBuilding.isUnitCombatRetrainType((UnitCombatTypes)iI))
					{
						iValue += 20;
					}
				}

				for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
				{
					int iDomainExpValue = 6;

					if (iI == DOMAIN_SEA)
					{
						iDomainExpValue = 7;
					}
					else if (iI == DOMAIN_LAND)
					{
						iDomainExpValue = 12;
					}
					iValue += kBuilding.getDomainFreeExperience(iI) * (bMetAnyCiv ? iDomainExpValue : iDomainExpValue / 2);
				}
				int iPromoValue = 0;
				foreach_(const FreePromoTypes & freePromoType, kBuilding.getFreePromoTypes())
				{
					if (freePromoType.m_pExprFreePromotionCondition)
					{
						iPromoValue += AI_getPromotionValue(freePromoType.ePromotion);
					}
					else
					{
						iPromoValue += AI_getPromotionValue(freePromoType.ePromotion) * 3/2;
					}
				}
				if (kBuilding.isApplyFreePromotionOnMove())
				{
					iPromoValue *= 2;
				}
				iValue += iPromoValue;

				iValue += kBuilding.getNationalCaptureProbabilityModifier() * 2;

				if ((!isDevelopingCity() || isCapital()) && kBuilding.EnablesUnits())
				{
					const bool bWarPlan = kTeam.hasWarPlan(true);
					const CvGameObjectCity* pObject = getGameObject();
					// add the extra building and its bonuses to the override to see if they influence the train condition of a unit
					std::vector<GOMOverride> queries;
					GOMOverride query = { pObject, GOM_BUILDING, eBuilding, true };
					queries.push_back(query);
					query.GOM = GOM_BONUS;
					foreach_(const BonusTypes eFreeBonus, kBuilding.getFreeBonuses() | map_keys)
					{
						query.id = eFreeBonus;
						queries.push_back(query);
					}

					for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
					{
						const CvUnitInfo& kUnit = GC.getUnitInfo((UnitTypes)iI);
						bool bUnitIsEnabler = kUnit.isPrereqAndBuilding((int)eBuilding);
						bool bUnitIsOtherwiseEnabled = false;

						if (kTeam.isHasTech((TechTypes)kUnit.getPrereqAndTech()))
						{
							bUnitIsOtherwiseEnabled = bUnitIsEnabler || kUnit.getNumPrereqAndBuildings() == 0;
							if (!bUnitIsOtherwiseEnabled)
							{
								for (int iI = 0; iI < kUnit.getNumPrereqAndBuildings(); ++iI)
								{
									// Toffer - seems strange to use break here if only one of X "AND" requirements are met...
									if (isActiveBuilding((BuildingTypes)kUnit.getPrereqAndBuilding(iI)))
									{
										bUnitIsOtherwiseEnabled = true;
										break;
									}
								}
							}
						}

						if (bUnitIsOtherwiseEnabled)
						{
							bool bUnitIsBonusEnabled = true;
							if (kUnit.getPrereqAndBonus() != NO_BONUS && !hasBonus((BonusTypes)kUnit.getPrereqAndBonus()))
							{
								if (kBuilding.getFreeBonuses().hasValue((BonusTypes)kUnit.getPrereqAndBonus()))
								{
									bUnitIsEnabler = true;
								}
								else
								{
									bUnitIsBonusEnabled = false;
								}
							}

							bool bHasORBonusAlready = false;
							bool bFreeBonusIsORBonus = false;
							int	iFreeExtraBonusCount = 0;

							foreach_(const BonusTypes eXtraFreeBonus, kUnit.getPrereqOrBonuses())
							{
								iFreeExtraBonusCount++;

								if (hasBonus(eXtraFreeBonus))
								{
									bHasORBonusAlready = true;
								}
								else if (kBuilding.getFreeBonuses().hasValue(eXtraFreeBonus))
								{
									bFreeBonusIsORBonus = true;
								}
							}

							if (iFreeExtraBonusCount > 0 && !bHasORBonusAlready)
							{
								if (bFreeBonusIsORBonus)
								{
									bUnitIsEnabler = true;
								}
								else
								{
									bUnitIsBonusEnabled = false;
								}
							}

							if (bUnitIsEnabler)
							{
								PROFILE("CalculateAllBuildingValues.IsUnitPrereq");

								if (kUnit.getDefaultUnitAIType() != NO_UNITAI && kOwner.AI_totalAreaUnitAIs(pArea, kUnit.getDefaultUnitAIType()) == 0)
								{
									iValue += iNumCitiesInArea;
								}

								int iAllowedUnitsValue = 0;

								// This forces the AI to build necessary buildings for units.
								switch (kUnit.getDefaultUnitAIType())
								{
								case UNITAI_UNKNOWN:
									break;
								case UNITAI_ANIMAL:
									iAllowedUnitsValue += kUnit.getCombat() / 5;
									break;
								case UNITAI_SETTLE:
								case UNITAI_SETTLER_SEA:
									iAllowedUnitsValue += 25;
									break;
								case UNITAI_WORKER:
								case UNITAI_WORKER_SEA:
									iAllowedUnitsValue += std::max(0, AI_getWorkersNeeded()) * 2;// 10; Calvitix test
									break;
								case UNITAI_ATTACK:
								case UNITAI_ATTACK_CITY:
								case UNITAI_COLLATERAL:
								case UNITAI_PILLAGE:
								case UNITAI_RESERVE:
								case UNITAI_COUNTER:
								case UNITAI_CITY_DEFENSE:
								case UNITAI_CITY_COUNTER:
								case UNITAI_CITY_SPECIAL:
								case UNITAI_ATTACK_SEA:
								case UNITAI_RESERVE_SEA:
								case UNITAI_ESCORT_SEA:
								case UNITAI_ASSAULT_SEA:
								case UNITAI_CARRIER_SEA:
								case UNITAI_MISSILE_CARRIER_SEA:
								case UNITAI_PIRATE_SEA:
								case UNITAI_ATTACK_AIR:
								case UNITAI_DEFENSE_AIR:
								case UNITAI_CARRIER_AIR:
								case UNITAI_MISSILE_AIR:
								case UNITAI_PARADROP:
								case UNITAI_ATTACK_CITY_LEMMING:
									iAllowedUnitsValue += std::max(6, (kUnit.getCombat() * kUnit.getCombat())) / 5 + (kUnit.getAirCombat() * kUnit.getAirCombat()) / 5;
									break;
								case UNITAI_INVESTIGATOR:
								case UNITAI_PROPERTY_CONTROL:
								case UNITAI_HEALER:
								case UNITAI_PROPERTY_CONTROL_SEA:
								case UNITAI_HEALER_SEA:
								case UNITAI_EXPLORE:
								case UNITAI_MISSIONARY:
								case UNITAI_PROPHET:
								case UNITAI_ARTIST:
								case UNITAI_SCIENTIST:
								case UNITAI_GENERAL:
								case UNITAI_GREAT_HUNTER:
								case UNITAI_GREAT_ADMIRAL:
								case UNITAI_MERCHANT:
								case UNITAI_ENGINEER:
								case UNITAI_SPY:
								case UNITAI_SPY_SEA:
								case UNITAI_MISSIONARY_SEA:
								case UNITAI_EXPLORE_SEA:
									iAllowedUnitsValue += 10;
									break;
								case UNITAI_ICBM:
									iAllowedUnitsValue += (kUnit.getNukeRange() != -1 ? kUnit.getNukeRange() * 50 : 0);
									break;
								}
								if (bWarPlan)
									iAllowedUnitsValue *= 2;

								if (iAllowedUnitsValue > 0)
								{
									// Just because we can build new units here doesn't add much if we can already build them a ton of other places
									iAllowedUnitsValue = iAllowedUnitsValue * 3 / (3 + std::min(10, kOwner.countNumBuildings(eBuilding)));
								}

								iValue += bUnitIsBonusEnabled ? iAllowedUnitsValue : iAllowedUnitsValue / 5;
							}
						}
					}
				}
			}
			// since this duplicates BUILDINGFOCUS_EXPERIENCE checks, do not repeat on pass 1
			if ((iFocusFlags & BUILDINGFOCUS_DOMAINSEA))
			{
				PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Sea");

				iValue += (kBuilding.getFreeExperience() * (bMetAnyCiv ? 16 : 8));

				for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
				{
					const CvUnitInfo& kUnitInfo = GC.getUnitInfo((UnitTypes)iI);
					UnitCombatTypes eCombatType = (UnitCombatTypes)kUnitInfo.getUnitCombatType();

					if (eCombatType != NO_UNITCOMBAT
						&& kUnitInfo.getDomainType() == DOMAIN_SEA
						&& canTrain((UnitTypes)iI))
					{
						iValue += (kBuilding.getUnitCombatFreeExperience().getValue(eCombatType) * (bMetAnyCiv ? 6 : 3));

						if (kBuilding.isUnitCombatRetrainType(eCombatType))
						{
							iValue += 20;
						}
					}
				}

				iValue += (kBuilding.getDomainFreeExperience(DOMAIN_SEA) * (bMetAnyCiv ? 16 : 8));

				iValue += (kBuilding.getDomainProductionModifier(DOMAIN_SEA) / 4);
			}

			if (((iFocusFlags & BUILDINGFOCUS_MAINTENANCE) || (iFocusFlags & BUILDINGFOCUS_GOLD) || iPass > 0)
			&& kBuilding.getCommerceChange(COMMERCE_GOLD) < 0 && GC.getTREAT_NEGATIVE_GOLD_AS_MAINTENANCE())
			{
				const int iBaseMaintenance = getMaintenanceTimes100();
				const int iMaintenanceMod = getEffectiveMaintenanceModifier();

				const int iCost =
				(
					std::min(
						-1,
						getModifiedIntValue(iBaseMaintenance, iMaintenanceMod)
						-
						getModifiedIntValue(
							iBaseMaintenance - kBuilding.getCommerceChange(COMMERCE_GOLD) * 100,
							iMaintenanceMod + kBuilding.getMaintenanceModifier()
						)
					)
				);
				const int iTempValue = std::min(-1, iCost * iGoldValueAssessmentModifier / 500);

				if (bFinancialTrouble)
				{
					iValue += 4 * iTempValue;
				}
				else iValue += iTempValue;
			}

			if ((iFocusFlags & BUILDINGFOCUS_SPECIALIST) || iPass > 0)
			{
				PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Specialist");

				int iSpecialistsValue = 0;
				int iCurrentSpecialistsRunnable = 0;
				for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
				{
					if (iI != GC.getDEFAULT_SPECIALIST())
					{
						const bool bUnlimited = (kOwner.isSpecialistValid((SpecialistTypes)iI));
						const int iRunnable = (getMaxSpecialistCount((SpecialistTypes)iI) > 0);

						if (bUnlimited || iRunnable > 0)
						{
							if (bUnlimited)
							{
								iCurrentSpecialistsRunnable += 5;
							}
							else
							{
								iCurrentSpecialistsRunnable += iRunnable;
							}
						}

						if (kBuilding.getSpecialistCount(iI) + kTeam.getBuildingSpecialistChange(eBuilding, (SpecialistTypes)iI) > 0)
						{
							if (!bUnlimited && iRunnable < 5)
							{
								int iTempValue = AI_specialistValue(((SpecialistTypes)iI), false, false);

								iTempValue *= 20 + 40 * (kBuilding.getSpecialistCount(iI) + kTeam.getBuildingSpecialistChange(eBuilding, (SpecialistTypes)iI));
								iTempValue /= 100;

								if (iFoodDifference < 2)
								{
									iTempValue /= 4;
								}
								if (iRunnable > 0)
								{
									iTempValue /= 1 + iRunnable;
								}
								iSpecialistsValue += std::max(12, (iTempValue / 100));
							}
						}
					}
				}

				if (iSpecialistsValue > 0)
				{
					iValue += iSpecialistsValue / std::max(2, iCurrentSpecialistsRunnable);
				}
			}

			int iCommerceYieldValue = 0;
			if ((iFocusFlags & (BUILDINGFOCUS_GOLD | BUILDINGFOCUS_RESEARCH | BUILDINGFOCUS_CULTURE | BUILDINGFOCUS_ESPIONAGE)) || iPass > 0)
			{
				iCommerceYieldValue = AI_buildingYieldValue(YIELD_COMMERCE, eBuilding, kBuilding, bForeignTrade, aiFreeSpecialistYield[YIELD_COMMERCE]);
			}

			if ((iFocusFlags & BUILDINGFOCUS_GOLD) && iCommerceYieldValue > 0)
			{
				// trade routes
				iValue += (kOwner.getCommercePercent(COMMERCE_GOLD) * iCommerceYieldValue * (bFinancialTrouble ? 200 : iGoldValueAssessmentModifier)) / 10000;
			}

			if ((iFocusFlags & BUILDINGFOCUS_RESEARCH) && iCommerceYieldValue > 0)
			{
				// trade routes
				iValue += (kOwner.getCommercePercent(COMMERCE_RESEARCH) * iCommerceYieldValue) / 100;
			}

			if ((iFocusFlags & BUILDINGFOCUS_CULTURE) && iCommerceYieldValue > 0)
			{
				// trade routes
				iValue += (kOwner.getCommercePercent(COMMERCE_CULTURE) * iCommerceYieldValue) / 100;
			}

			if ((iFocusFlags & BUILDINGFOCUS_ESPIONAGE) && iCommerceYieldValue > 0)
			{
				// trade routes
				iValue += (kOwner.getCommercePercent(COMMERCE_ESPIONAGE) * iCommerceYieldValue) / 100;
			}

			if (iPass > 0)
			{
				foreach_(const TraitTypes eTrait, kBuilding.getFreeTraitTypes())
				{
					if (GC.getTraitInfo(eTrait).isCivilizationTrait())
					{
						if (!GC.getTraitInfo(eTrait).isNegativeTrait())
						{
							for (int iJ = 0; iJ < GC.getNumFlavorTypes(); iJ++)
							{
								iValue += (GC.getLeaderHeadInfo(kOwner.getLeaderType()).getFlavorValue(iJ) * GC.getTraitInfo(eTrait).getFlavorValue(iJ));
							}
						}
						else
						{
							for (int iJ = 0; iJ < GC.getNumFlavorTypes(); iJ++)
							{
								iValue -= (GC.getLeaderHeadInfo(kOwner.getLeaderType()).getFlavorValue(iJ) * GC.getTraitInfo(eTrait).getFlavorValue(iJ));
							}
						}
					}
				}
				for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
				{
					if (kBuilding.isHurry(iI))
					{
						iValue += 50;
					}
				}
				iValue -= kBuilding.getRiverDefensePenalty() / 4;

				if (kBuilding.getDomesticGreatGeneralRateModifier() != 0)
				{
					iValue += (kBuilding.getDomesticGreatGeneralRateModifier() / 10);
				}

				if (kBuilding.isAreaBorderObstacle() && !pArea->isBorderObstacle(getTeam())
				&& !GC.getGame().isOption(GAMEOPTION_BARBARIAN_NONE))
				{
					iValue += iNumCitiesInArea;

					if (GC.getGame().isOption(GAMEOPTION_BARBARIAN_RAGING))
					{
						iValue += iNumCitiesInArea;
					}
				}

				if (kBuilding.isGovernmentCenter())
				{
					FAssert(!kBuilding.isCapital());
					iValue += ((calculateDistanceMaintenance() - 3) * iNumCitiesInArea);
				}

				if (kBuilding.isMapCentering())
				{
					iValue++;
				}

				foreach_(const BonusModifier& pair, kBuilding.getFreeBonuses())
				{
					//	If we have spares adding another doesn't do anything that scales by city count - only
					//	the first one does that.  Furthermore as spares rack up even their trade value decreases
					iValue += (
						100 * kOwner.AI_bonusVal(pair.first, 1) * pair.second
						* (kOwner.getNumTradeableBonuses(pair.first) == 0 ? iNumCities : 1)
						)
						/ (100 * std::max(1, kOwner.getNumTradeableBonuses(pair.first)));
				}

				int iCivicOption = kBuilding.getCivicOption();
				if (iCivicOption != NO_CIVICOPTION)
				{
					for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
					{
						if (GC.getCivicInfo((CivicTypes)iI).getCivicOptionType() == iCivicOption && !kOwner.canDoCivics((CivicTypes)iI))
						{
							iValue += kOwner.AI_civicValue((CivicTypes)iI) / 10;
						}
					}
				}

				int iGreatPeopleRateModifier = kBuilding.getGreatPeopleRateModifier();
				if (iGreatPeopleRateModifier > 0)
				{
					const int iGreatPeopleRate = getBaseGreatPeopleRate();
					const int kTargetGPRate = 10;

					// either not a wonder, or a wonder and our GP rate is at least the target rate
					if (!bIsLimitedWonder || iGreatPeopleRate >= kTargetGPRate)
					{
						iValue += ((iGreatPeopleRateModifier * iGreatPeopleRate) / 16);
					}
					// otherwise, this is a limited wonder (aka National Epic), we _really_ do not want to build this here
					// subtract from the value (if this wonder has a lot of other stuff, we still might build it)
					else
					{
						iValue -= ((iGreatPeopleRateModifier * (kTargetGPRate - iGreatPeopleRate)) / 12);
					}
				}

				iValue += kBuilding.getGlobalGreatPeopleRateModifier() * iNumCities / 8;
				iValue += kBuilding.getAnarchyModifier() / (-4);
				iValue += kBuilding.getGlobalHurryModifier() * (-2);
				iValue += kBuilding.getGlobalFreeExperience() * iNumCities * (bMetAnyCiv ? 6 : 3);

				if (bCanPopRush)
				{
					iValue += kBuilding.getFoodKept() / 2;
				}

				iValue += kBuilding.getAirlift() * (getPopulation() * 3 + 10);

				int iAirDefense = -kBuilding.getAirModifier();
				if (iAirDefense > 0
					&& (kOwner.AI_totalUnitAIs(UNITAI_DEFENSE_AIR) > 0 && kOwner.AI_totalUnitAIs(UNITAI_ATTACK_AIR) > 0 || kOwner.AI_totalUnitAIs(UNITAI_MISSILE_AIR) > 0))
				{
					iValue += iAirDefense / (bMetAnyCiv ? 2 : 4);
				}

				iValue += kBuilding.getAirUnitCapacity() * (getPopulation() * 2 + 10);
				iValue += -kBuilding.getNukeModifier() / (bMetAnyCiv ? 10 : 20);
				iValue += kBuilding.getFreeSpecialist() * 16;
				iValue += kBuilding.getAreaFreeSpecialist() * iNumCitiesInArea * 12;
				iValue += kBuilding.getGlobalFreeSpecialist() * iNumCities * 12;
				iValue += kBuilding.getWorkerSpeedModifier() * kOwner.AI_getNumAIUnits(UNITAI_WORKER) / 10;

				int iMilitaryProductionModifier = kBuilding.getMilitaryProductionModifier();

				for (int iJ = 0; iJ < kBuilding.getNumUnitCombatProdModifiers(); iJ++)
				{
					iMilitaryProductionModifier += kBuilding.getUnitCombatProdModifier(iJ) / 4;
				}
				if (bMetAnyCiv && iMilitaryProductionModifier > 0)
				{
					// either not a wonder, or a wonder and we are a high production city
					if (!bIsLimitedWonder || bIsHighProductionCity)
					{
						iValue += (iMilitaryProductionModifier / 4);

						// if a wonder, then pick one of the best cities
						if (bIsLimitedWonder)
						{
							// if one of the top 3 production cities, give a big boost
							if (aiYieldRank[YIELD_PRODUCTION] <= (2 + iLimitedWonderLimit))
							{
								iValue += 2 * iMilitaryProductionModifier / (2 + aiYieldRank[YIELD_PRODUCTION]);
							}
						}
						// otherwise, any of the top half of cities will do
						else if (bIsHighProductionCity)
						{
							iValue += iMilitaryProductionModifier / 4;
						}
						iValue += iMilitaryProductionModifier * (getFreeExperience() + getSpecialistFreeExperience()) / 10;
					}
					// otherwise, this is a limited wonder (aka Heroic Epic), we _really_ do not want to build this here
					// subtract from the value (if this wonder has a lot of other stuff, we still might build it)
					else
					{
						iValue -= (iMilitaryProductionModifier * aiYieldRank[YIELD_PRODUCTION]) / 5;
					}
				}
				iValue += kBuilding.getSpaceProductionModifier() / 5;
				iValue += kBuilding.getGlobalSpaceProductionModifier() * iNumCities / 20;

				if (kBuilding.getGreatPeopleUnitType() != NO_UNIT)
				{
					iValue++; // XXX improve this for diversity...
				}

				// prefer to build great people buildings in places that already have some GP points

				iValue += (kBuilding.getGreatPeopleRateChange() + iSpecialistGreatPeopleRate) * 10 * (1 + getBaseGreatPeopleRate() / 2);

				if (!bAreaAlone)
				{
					for (int iI = 0; iI < kBuilding.getNumHealUnitCombatTypes(); iI++)
					{
						iValue += kBuilding.getHealUnitCombatType(iI).iHeal / 4;
					}
					iValue += kBuilding.getHealRateChange() / 2;
					for (int iI = 0; iI < kBuilding.getNumAidRateChanges(); iI++)
					{
						iValue += kBuilding.getAidRateChange(iI).iChange / 3;//Update
					}
					for (int iI = 0; iI < kBuilding.getNumBonusAidModifiers(); iI++)
					{
						if (hasBonus(kBuilding.getBonusAidModifier(iI).eBonusType))
						{
							iValue += kBuilding.getBonusAidModifier(iI).iModifier / 3;
						}
					}
				}

				if (kBuilding.getPopulationChange() != 0)
				{
					if (kBuilding.getPopulationChange() > 0)
					{
						int iPop = getPopulation();
						for (int iI = 0; iI < kBuilding.getPopulationChange(); iI++)
						{
							iValue += 20 + 5 * (iPop + iI);
						}
					}
					else
					{
						int iPop = getPopulation();
						// Toffer: iPop decrement before calc is intentional
						//	as a pop 1 city should see the same value in gaining one pop as a pop 2 city negatively sees in losing one pop.
						for (int iI = 0; iI < -kBuilding.getPopulationChange() && iPop-- > 1; iI++)
						{
							iValue -= 20 + 5 * iPop;
						}
					}
				}
				if (kBuilding.getGlobalPopulationChange() > 0)
				{
					const int iTotalPopulation = kOwner.getTotalPopulation();
					for (int iI = 0; iI < kBuilding.getGlobalPopulationChange(); iI++)
					{
						iValue += 20 + 5 * ((iTotalPopulation + iNumCities * iI) / iNumCities);
					}
				}

				iValue += kBuilding.getFreeTechs() * 80;
				iValue += kBuilding.getEnemyWarWearinessModifier() / 2;

				for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
				{
					if (kBuilding.getFreeSpecialistCount(iI) > 0)
					{
						iValue += AI_specialistValue(((SpecialistTypes)iI), false, false) * kBuilding.getFreeSpecialistCount(iI) / 50;
					}
				}

				foreach_(const ImprovementModifier & pair, kBuilding.getImprovementFreeSpecialists())
				{
					iValue += pair.second * countNumImprovedPlots(pair.first, true) * 50;
				}

				for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
				{
					iValue += kBuilding.getDomainProductionModifier(iI) / 5;

					if (bIsHighProductionCity)
					{
						iValue += kBuilding.getDomainProductionModifier(iI) / 5;
					}
				}
				int forcedTradeRoutesValue = 0;
				if (kBuilding.isForceAllTradeRoutes())
				{
					for (int iI = 0; iI < MAX_TEAMS; iI++)
					{
						if (GET_TEAM((TeamTypes)iI).isAlive() && !kTeam.isVassal((TeamTypes)iI) && !kTeam.isOpenBorders((TeamTypes)iI))
						{
							forcedTradeRoutesValue += GET_TEAM((TeamTypes)iI).getNumCities() * 2;
						}
					}
				}
				iValue += forcedTradeRoutesValue;
				foreach_(const UnitModifier2 & modifier, kBuilding.getUnitProductionModifiers())
				{
					int unitProductionModifierValue = 0;
					const UnitTypes eLoopUnit = modifier.first;

					if (canTrain(eLoopUnit))
					{
						const int iModifier = modifier.second;
						const UnitAITypes eUnitAI = GC.getUnitInfo(eLoopUnit).getDefaultUnitAIType();
						const UnitTypes eBestUnit = kOwner.bestBuildableUnitForAIType(GC.getUnitInfo(eLoopUnit).getDomainType(), eUnitAI);

						int iBuildCost = 0;
						if (eBestUnit == NO_UNIT)
						{
							iBuildCost = GC.getUnitInfo(eLoopUnit).getProductionCost();
						}
						else
						{
							int	iBestUnitAIValue = kOwner.AI_unitValue(eBestUnit, eUnitAI, pArea);
							int	iThisUnitAIValue = kOwner.AI_unitValue(eLoopUnit, eUnitAI, pArea);

							//	Value as cost of production of the unit we can build scaled by their relative AI value (non-linear - we're squaring the ratio)
							int	iComparisonToBestFactor = (10 * iThisUnitAIValue) / std::max(1, iBestUnitAIValue);

							iBuildCost = (iComparisonToBestFactor * iComparisonToBestFactor * GC.getUnitInfo(eBestUnit).getProductionCost()) / 100;
						}
						int iTempValue = (iBuildCost * 2 * iModifier) / 100;
						if (bIsHighProductionCity)
						{
							iTempValue *= 3; // adding 10% and 20% is same as adding 30% or 3 * 10%
						}
						unitProductionModifierValue += iTempValue;
					}

					iValue += unitProductionModifierValue;
				}
				if (kBuilding.getPopulationgrowthratepercentage() != 0)
				{
					int iFoodPerTurn = foodDifference();

					if (iFoodPerTurn > 0)
					{
						int iCityHappy = happyLevel() - unhappyLevel();
						if (iCityHappy >= 0)
						{
							int iCurrentFoodToGrow = growthThreshold();
							int growthValue = (std::min(3, iCityHappy + 1) * iCurrentFoodToGrow) / iFoodPerTurn;
							iValue -= (kBuilding.getPopulationgrowthratepercentage() * growthValue) / 100;
						}
					}

				}
				if (kBuilding.getGlobalPopulationgrowthratepercentage() != 0)
				{
					int iCityCount = 0;
					int globalGrowthValue = 0;
					foreach_(const CvCity * pLoopCity, kOwner.cities())
					{
						const int iFoodPerTurn = pLoopCity->foodDifference();

						iCityCount++;

						if (iFoodPerTurn > 0)
						{
							const int iCityHappy = pLoopCity->happyLevel() - pLoopCity->unhappyLevel();
							if (iCityHappy >= 0)
							{
								const int iCurrentFoodToGrow = pLoopCity->growthThreshold();
								globalGrowthValue -= (std::min(3, iCityHappy + 1) * iCurrentFoodToGrow) / iFoodPerTurn;
							}
						}
					}

					if (iCityCount > 0)
					{
						iValue += (kBuilding.getGlobalPopulationgrowthratepercentage() * globalGrowthValue) / (iCityCount * 100);
					}
				}

				foreach_(const BuildingModifier2 & modifier, kBuilding.getBuildingProductionModifiers())
				{
					const BuildingTypes eLoopBuilding = modifier.first;
					if (canConstruct(eLoopBuilding))
					{
						const int iModifier = modifier.second;
						if (iModifier > -100)
						{
							const int iOriginalCost = getHurryCost(eLoopBuilding);
							const int iNewCost = (iOriginalCost * (100 / (100 + iModifier)));
							iValue += (iOriginalCost - iNewCost) / 10;
						}
						else
						{//If the modifier is less than -100, avoid it like the plague
							iValue -= 100;
						}
					}
				}

				foreach_(const BuildingModifier2 & modifier, kBuilding.getGlobalBuildingProductionModifiers())
				{
					const BuildingTypes eLoopBuilding = modifier.first;
					if (kOwner.canConstruct(eLoopBuilding))
					{
						const int iModifier = modifier.second;

						if (iModifier > -100)
						{
							const int iCount = count_if(kOwner.cities(), !CvCity::fn::hasBuilding(eLoopBuilding));
							const int iOriginalCost = getHurryCost(eLoopBuilding);
							const int iNewCost = (iOriginalCost * (100 / (100 + iModifier)));
							iValue += ((iOriginalCost - iNewCost) * iCount) / 10;
						}
						else
						{//If the modifier is less than -100, avoid it like the plague
							iValue -= 100;
						}
					}
				}

				foreach_(const BuildingModifier2 & modifier, kBuilding.getGlobalBuildingCostModifiers())
				{
					const BuildingTypes eLoopBuilding = modifier.first;
					if (kOwner.canConstruct(eLoopBuilding))
					{
						const int iOriginalCost = kOwner.getProductionNeeded(eLoopBuilding);
						int iPlayerMod = kOwner.getBuildingCostModifier(eLoopBuilding);

						int iNewCost = 0;
						// Reverse old modifier
						if (iPlayerMod < 0)
						{
							iNewCost = iOriginalCost * (-1 * iPlayerMod + 100) / 100;
						}
						else if (iPlayerMod > 0)
						{
							iNewCost = iOriginalCost * 100 / (100 + iPlayerMod);
						}
						iPlayerMod += modifier.second;
						// Apply new modifier
						if (iPlayerMod < 0)
						{
							iNewCost = iOriginalCost * 100 / (-1 * iPlayerMod + 100);
						}
						else if (iPlayerMod > 0)
						{
							iNewCost = iOriginalCost * (100 + iPlayerMod) / 100;
						}

						const int iCount = count_if(kOwner.cities(), !CvCity::fn::hasBuilding(eLoopBuilding));

						iValue += (iOriginalCost - iNewCost) * iCount / 10;
					}
				}

				if (kBuilding.isProvidesFreshWater() && !plot()->isFreshWater())
				{
					int freshWaterValue = 0;
					foreach_(const CvPlot * pLoopPlot, plots(NUM_CITY_PLOTS_1, true))
					{
						if (!pLoopPlot->isWater() &&
							!pLoopPlot->isFreshWater() &&
							!pLoopPlot->isHills() &&
							!pLoopPlot->isImpassable())
						{
							if (pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
							{//if there are no improvements, farms become much more likely
								freshWaterValue += 20;
							}
							else
							{//we are not likely to tear down existing improvements for a farm
								freshWaterValue += 5;
							}
						}
					}
					iValue += freshWaterValue;
				}

				int religiousBuildingValue = 0;
				for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
				{
					PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Units");

					if (GC.getUnitInfo((UnitTypes)iI).isPrereqAndBuilding((int)eBuilding))
					{
						// BBAI TODO: Smarter monastary construction, better support for mods

						if (kOwner.AI_totalAreaUnitAIs(pArea, GC.getUnitInfo((UnitTypes)iI).getDefaultUnitAIType()) == 0)
						{
							religiousBuildingValue += iNumCitiesInArea;
						}

						religiousBuildingValue++;

						ReligionTypes eReligion = (ReligionTypes)(GC.getUnitInfo((UnitTypes)iI).getPrereqReligion());
						if (eReligion != NO_RELIGION)
						{
							//encouragement to get some minimal ability to train special units
							if (bCulturalVictory1 || isHolyCity(eReligion) || isCapital())
							{
								religiousBuildingValue += (2 + iNumCitiesInArea);
							}

							if (bCulturalVictory2 && GC.getUnitInfo((UnitTypes)iI).getReligionSpreads(eReligion))
							{
								//this gives a very large extra value if the religion is (nearly) unique
								//to no extra value for a fully spread religion.
								//I'm torn between huge boost and enough to bias towards the best monastery type.
								int iReligionCount = kOwner.getHasReligionCount(eReligion);
								religiousBuildingValue += (100 * (iNumCities - iReligionCount)) / (iNumCities * (iReligionCount + 1));
							}
						}
					}
				}

				if (religiousBuildingValue > 0)
				{
					// Just because we can build new units here doesn't add much if we can already build them a ton of other places
					religiousBuildingValue = religiousBuildingValue * 3 / (3 + std::min(10, kOwner.countNumBuildings(eBuilding)));
				}

				iValue += religiousBuildingValue;

				// is this building needed to build other buildings?
				for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
				{
					PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Buildings");

					const BuildingTypes eLoopBuilding = static_cast<BuildingTypes>(iI);
					const int iPrereqBuildings = kOwner.getBuildingPrereqBuilding(eLoopBuilding, eBuilding);

					// if we need some of us to build iI building, and we dont need more than we have cities
					if (iPrereqBuildings > 0 && iPrereqBuildings <= iNumCities)
					{
						// do we need more than what we are currently building?
						if (iPrereqBuildings > kOwner.getBuildingCountPlusMaking(eBuilding))
						{
							iValue += (iNumCities * 3);

							if (bCulturalVictory1)
							{
								const int iLoopBuildingCultureModifier = GC.getBuildingInfo(eLoopBuilding).getCommerceModifier(COMMERCE_CULTURE);
								if (iLoopBuildingCultureModifier > 0)
								{
									const int iLoopBuildingsBuilt = kOwner.getBuildingCount(eLoopBuilding);

									// If we have less than the number needed in culture cities
									// OR we are one of the top cities AND we do not have the building
									if (iLoopBuildingsBuilt < iCulturalVictoryNumCultureCities
									|| iCultureRank <= iCulturalVictoryNumCultureCities
									&& !hasBuilding(eLoopBuilding))
									{
										iValue += iLoopBuildingCultureModifier;

										if (bCulturalVictory3)
										{
											iValue += iLoopBuildingCultureModifier * 2;
										}
									}
								}
							}
						}
					}
				}

				for (int iI = 0; iI < GC.getNumVoteSourceInfos(); ++iI)
				{
					PROFILE("CvCityAI::AI_buildingValueThresholdOriginal.Votes");

					if (kBuilding.getVoteSourceType() == iI)
					{
						int votingSourceValue = 0;
						if (kBuilding.needStateReligionInCity())
						{
							int iShareReligionCount = 0;
							int iPlayerCount = 0;
							for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
							{
								if (iPlayer != getOwner() && GET_PLAYER((PlayerTypes)iPlayer).isAlive())
								{
									iPlayerCount++;
									if (kOwner.getStateReligion() == GET_PLAYER((PlayerTypes)iPlayer).getStateReligion())
									{
										iShareReligionCount++;
									}
								}
							}
							votingSourceValue += (200 * (1 + iShareReligionCount)) / (1 + iPlayerCount);
						}
						else votingSourceValue += 100;

						iValue += (votingSourceValue * (kOwner.AI_isDoVictoryStrategy(AI_VICTORY_DIPLOMACY1) ? 5 : 1));
					}
				}

				iValue += (
					AI_getBuildingYieldValue(
						eBuilding, kBuilding, bIsLimitedWonder, bForeignTrade, bFinancialTrouble,
						aiFreeSpecialistYield, aiYieldRank, iLimitedWonderLimit, pArea, iTotalPopulation, iFoodDifference
					)
				);
			}
			else
			{
				if (iFocusFlags & BUILDINGFOCUS_FOOD)
				{
					iValue += AI_buildingYieldValue(YIELD_FOOD, eBuilding, kBuilding, bForeignTrade, aiFreeSpecialistYield[YIELD_FOOD]);

					iValue += kBuilding.getFoodKept();
				}

				if (iFocusFlags & BUILDINGFOCUS_PRODUCTION)
				{
					iValue += (
						AI_getBuildingProductionValue(
							eBuilding, kBuilding, bIsLimitedWonder, bForeignTrade,
							aiFreeSpecialistYield[YIELD_PRODUCTION],
							aiYieldRank[YIELD_PRODUCTION], iLimitedWonderLimit
						)
					);
				}
			}

			// Deal with properties
			if (iFocusFlags & BUILDINGFOCUS_PROPERTY)
			{
				iValue += buildingPropertiesValue(kBuilding) * 10;
			}
			else iValue += buildingPropertiesValue(kBuilding);


			if (iFocusFlags & BUILDINGFOCUS_INVESTIGATION)
			{
				iValue += kBuilding.getInvestigation() * 10;
			}
			else iValue += kBuilding.getInvestigation() * 5;


			if (iPass > 0)
			{
				iValue += (
					AI_getBuildingCommerceValue(
						eBuilding, kBuilding, bIsLimitedWonder, bFinancialTrouble, bCulturalVictory1,
						aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate, aiCommerceRank,
						iLimitedWonderLimit, iTotalPopulation
					)
				);

				foreach_(const ReligionModifier & pair, kBuilding.getReligionChanges())
				{
					if (kTeam.hasHolyCity(pair.first))
					{
						iValue += (pair.second * (eStateReligion == pair.first ? 10 : 1));
					}
				}

				if (NO_VOTESOURCE != kBuilding.getVoteSourceType())
				{
					iValue += 100;
				}
			}
			else
			{
				if (iFocusFlags & BUILDINGFOCUS_GOLD)
				{
					iValue += (getBuildingCommerceValue(eBuilding, COMMERCE_GOLD, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate) * iGoldValueAssessmentModifier) / 100;
				}

				if (iFocusFlags & BUILDINGFOCUS_RESEARCH)
				{
					iValue += getBuildingCommerceValue(eBuilding, COMMERCE_RESEARCH, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate);
				}

				if (iFocusFlags & BUILDINGFOCUS_CULTURE)
				{
					iValue += getBuildingCommerceValue(eBuilding, COMMERCE_CULTURE, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate);
				}

				if (iFocusFlags & BUILDINGFOCUS_BIGCULTURE)
				{
					iValue += getBuildingCommerceValue(eBuilding, COMMERCE_CULTURE, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate) / 5;
				}

				if (iFocusFlags & BUILDINGFOCUS_ESPIONAGE)
				{
					iValue += getBuildingCommerceValue(eBuilding, COMMERCE_ESPIONAGE, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate);
				}
			}

			if (iThreshold > 0 && iPass == 0 && iValue < iThreshold)
			{
				iValue = 0;
			}
		}

		//	If we are really seeking an answer specifically on the values from the specific flags
		//	then the rest is just a tie-breaker so dramatically boost the value of the flag-specific
		//	stuff from pass1
		if (bMaximizeFlaggedValue)
		{
			iValue += iPass1Value * 19;
		}
		else
		{
			//	Even when not maximizing with all the extra effects on buildings pass1 was getting a
			//	bit dominated by pass2 so provide a little extra pass1 weight anyway
			iValue += iPass1Value;
		}

		// If wonder is being constructed in some special way, reduce the value for small cities.
		if (!bForTech && bIsLimitedWonder && iValue > 0 && getPopulation() < 7 && !canConstruct(eBuilding))
		{
			iValue = std::max(1, iValue / (8 - getPopulation()));
		}
	}
	if (iThreshold == -123) // Debug UI - magic number.
	{
		return iValue;
	}
	return std::max(0, iValue);
}

int CvCityAI::AI_buildingYieldValue(YieldTypes eYield, BuildingTypes eBuilding, const CvBuildingInfo& kBuilding, bool bForeignTrade, int iFreeSpecialistYield) const
{
	PROFILE_EXTRA_FUNC();
	int iValue = tradeRouteValue(kBuilding, eYield, bForeignTrade);

	int iBaseRate = getPlotYield(eYield);
	{
		int iPlotChange = 0;
		foreach_(const PlotArray & pair, kBuilding.getPlotYieldChanges())
		{
			if (pair.second[eYield] != 0)
			{
				foreach_(const CvPlot * plotX, plots(NUM_CITY_PLOTS))
				{
					if (plotX->getPlotType() == pair.first && canWork(plotX))
					{
						iPlotChange += pair.second[eYield];
					}
				}
			}
		}
		foreach_(const TerrainArray & pair, kBuilding.getTerrainYieldChanges())
		{
			if (pair.second[eYield] != 0)
			{
				foreach_(const CvPlot * plotX, plots(NUM_CITY_PLOTS))
				{
					if (plotX->getTerrainType() == pair.first && canWork(plotX))
					{
						iPlotChange += pair.second[eYield];
					}
				}
			}
		}
		if (kBuilding.getRiverPlotYieldChange(eYield) > 0)
		{
			iPlotChange += kBuilding.getRiverPlotYieldChange(eYield) * countNumRiverPlots();
		}
		if (iPlotChange != 0)
		{
			iValue += std::min(getPopulation(), 10) * iPlotChange;
			iBaseRate += iPlotChange * 3 / 4;
		}
	}

	{
		const int iYield = 8 * (100 * iFreeSpecialistYield + getBaseYieldRateFromBuilding100(eYield, eBuilding));
		if (iYield > 0)
		{
			iValue += std::max(1, iYield / 100);
		}
		else if (iYield < 0)
		{
			iValue += std::min(-1, iYield / 100);
		}
	}

	{
		int iMod = kBuilding.getYieldModifier(eYield) + GET_TEAM(getTeam()).getBuildingYieldTechModifier(eYield, eBuilding);
		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			if (hasBonus((BonusTypes)iI))
			{
				iMod += kBuilding.getBonusYieldModifier(iI, eYield);
				if (kBuilding.getVicinityBonusYieldChanges(iI, eYield) != 0 && (hasVicinityBonus((BonusTypes)iI) || hasRawVicinityBonus((BonusTypes)iI)))
				{
					iValue += kBuilding.getVicinityBonusYieldChanges(iI, eYield) * 8;
				}
			}
		}
		if (!isPower() && (kBuilding.isPower() || kBuilding.getPowerBonus() != NO_BONUS && hasBonus((BonusTypes)kBuilding.getPowerBonus())))
		{
			iMod += getPowerYieldRateModifier(eYield);
		}
		iValue += iBaseRate * iMod / 12;
	}
	return iValue;
}


ProjectTypes CvCityAI::AI_bestProject() const
{
	PROFILE_EXTRA_FUNC();
	int iProductionRank = findYieldRateRank(YIELD_PRODUCTION);

	int iBestValue = 0;
	ProjectTypes eBestProject = NO_PROJECT;

	for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		if (canCreate((ProjectTypes)iI))
		{
			int iValue = AI_projectValue((ProjectTypes)iI);

			if ((GC.getProjectInfo((ProjectTypes)iI).getEveryoneSpecialUnit() != NO_SPECIALUNIT) ||
				(GC.getProjectInfo((ProjectTypes)iI).getEveryoneSpecialBuilding() != NO_SPECIALBUILDING) ||
				GC.getProjectInfo((ProjectTypes)iI).isAllowsNukes())
			{
				if (GC.getGame().getSorenRandNum(100, "Project Everyone") == 0)
				{
					iValue++;
				}
			}

			if (iValue > 0)
			{
				iValue += getProjectProduction((ProjectTypes)iI);

				const int iTurnsLeft = getProductionTurnsLeft(((ProjectTypes)iI), 0);

				if (!GET_TEAM(getTeam()).isHuman() || iTurnsLeft <= std::max(1, 10 * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent() / 100))
				{
					if (iTurnsLeft <= std::max(1, 20 * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent() / 100)
					|| iProductionRank <= std::max(3, GET_PLAYER(getOwner()).getNumCities() / 2))
					{
						if (iProductionRank == 1)
						{
							iValue += iTurnsLeft;
						}
						else
						{
							FAssert((MAX_INT / 1000) > iValue);
							iValue *= 1000;
							iValue /= std::max(1, (iTurnsLeft + 10));
						}

						iValue = std::max(1, iValue);

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							eBestProject = ((ProjectTypes)iI);
						}
					}
				}
			}
		}
	}

	return eBestProject;
}


int CvCityAI::AI_projectValue(ProjectTypes eProject) const
{
	PROFILE_EXTRA_FUNC();
	int iValue = 0;
	const CvProjectInfo& project = GC.getProjectInfo(eProject);

	if (project.getNukeInterception() > 0 && GC.getGame().canTrainNukes())
	{
		iValue += project.getNukeInterception() / 10;
	}

	if (project.getTechShare() > 0 && project.getTechShare() < GET_TEAM(getTeam()).getHasMetCivCount(true))
	{
		iValue += 20 / project.getTechShare();
	}

	for (int iI = 0; iI < GC.getNumVictoryInfos(); iI++)
	{
		if (GC.getGame().isVictoryValid((VictoryTypes)iI))
		{
			iValue += 20 * std::max(0, project.getVictoryThreshold(iI) - GET_TEAM(getTeam()).getProjectCount(eProject));
		}
	}

	for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		iValue += 10 * std::max(0, GC.getProjectInfo((ProjectTypes)iI).getProjectsNeeded(eProject) - GET_TEAM(getTeam()).getProjectCount(eProject));
	}

	const int iOurNumCities = GET_PLAYER(getOwner()).getNumCities();

	iValue += 4 * iOurNumCities * project.getGlobalHappiness();
	iValue += 4 * iOurNumCities * project.getGlobalHealth();

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		iValue += project.getCommerceModifier(iI) * iOurNumCities / 4;
	}
	const int iWorldHappy = project.getWorldHappiness();
	const int iWorldHealth = project.getWorldHealth();
	const int iWorldTradeRoutes = project.getWorldTradeRoutes();

	if (iWorldHappy != 0 || iWorldHealth != 0 || iWorldTradeRoutes != 0)
	{
		iValue += iWorldTradeRoutes * 8 * (1 + getPopulation() / 5);

		iValue += 4 * (iOurNumCities * iWorldHappy + iOurNumCities * iWorldHealth);

		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive() && !GET_PLAYER((PlayerTypes)iI).isMinorCiv() && iI != getOwner())
			{
				const int iNumCities = GET_PLAYER((PlayerTypes)iI).getNumCities();

				switch (GET_PLAYER(getOwner()).AI_getAttitude((PlayerTypes)iI))
				{
				case ATTITUDE_FURIOUS:
				{
					iValue -= 2 * (iNumCities * iWorldHealth + iNumCities * iWorldHappy);
					iValue -= iWorldTradeRoutes * 8;
					break;
				}
				case ATTITUDE_ANNOYED:
				{
					iValue -= iNumCities * iWorldHealth + iNumCities * iWorldHappy;
					iValue -= iWorldTradeRoutes * 4;
					break;
				}
				case ATTITUDE_CAUTIOUS:
				{
					iValue -= (iNumCities * iWorldHealth + iNumCities * iWorldHappy) / 3;
					iValue -= iWorldTradeRoutes;
					break;
				}
				case ATTITUDE_PLEASED:
				{
					iValue += iNumCities * iWorldHealth + iNumCities * iWorldHappy;
					iValue += iWorldTradeRoutes * 4;
					break;
				}
				case ATTITUDE_FRIENDLY:
				{
					iValue += 2 * (iNumCities * iWorldHealth + iNumCities * iWorldHappy);
					iValue += iWorldTradeRoutes * 8;
					break;
				}
				default:
				{
					FErrorMsg("error");
					break;
				}
				}
			}
		}
	}
	return iValue;
}

ProcessTypes CvCityAI::AI_bestProcess(CommerceTypes eCommerceType, int64_t* commerceWeights) const
{
	PROFILE_EXTRA_FUNC();
	int64_t iBestValue = 0;
	ProcessTypes eBestProcess = NO_PROCESS;

	for (int iI = 0; iI < GC.getNumProcessInfos(); iI++)
	{
		const ProcessTypes eProcess = static_cast<ProcessTypes>(iI);
		if (canMaintain(eProcess))
		{
			const int64_t iValue = AI_processValue(eProcess, eCommerceType, commerceWeights);

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestProcess = eProcess;
			}
		}
	}
	return eBestProcess;
}

int64_t CvCityAI::AI_processValue(ProcessTypes eProcess, CommerceTypes eCommerceType, int64_t* commerceWeights) const
{
	PROFILE_EXTRA_FUNC();
	int64_t iValue = 0;
	const bool bSpecific = eCommerceType != NO_COMMERCE;

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		const CommerceTypes eCommerce = static_cast<CommerceTypes>(iI);
		if (bSpecific && eCommerce != eCommerceType)
		{
			continue;
		}
		int64_t iTempValue = GC.getProcessInfo(eProcess).getProductionToCommerceModifier(eCommerce);

		iTempValue *= GET_PLAYER(getOwner()).AI_commerceWeight(eCommerce, this); // scaled by 100 at this point
		if (iTempValue == 0) continue; // weight may very well be 0 if commerce is worthless.

		iTempValue *= GET_PLAYER(getOwner()).AI_averageCommerceExchange(eCommerce);
		iTempValue = std::max<int64_t>(1, iTempValue / 6000); // 60 * 100

		// Koshling - take into account external factors determining which commerce types we WANT most right now
		if (commerceWeights != NULL)
		{
			iTempValue = std::max<int64_t>(1, iTempValue * (100 + commerceWeights[iI]) / 100);
		}
		iValue += iTempValue;
	}
	// Minimum 1 value if commerce type was not specified, minimum zero value if it was.
	return std::max<int64_t>(!bSpecific, iValue);
}


int CvCityAI::AI_neededSeaWorkers() const
{
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                      01/01/09                                jdog5000      */
	/*                                                                                              */
	/* Worker AI                                                                                    */
	/************************************************************************************************/
	CvArea* pWaterArea = waterArea(true);

	if (pWaterArea == NULL)
	{
		return 0;
	}
	#define	NB_MAX_SEA_WORKERS 5.0
	int iNeededSeaWorkers = GET_PLAYER(getOwner()).countUnimprovedBonuses(pWaterArea);

	// Check if second water area city can reach was any unimproved bonuses
	pWaterArea = secondWaterArea();
	if (pWaterArea != NULL)
	{
		iNeededSeaWorkers += GET_PLAYER(getOwner()).countUnimprovedBonuses(pWaterArea);
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                       END                                                  */
	/************************************************************************************************/
	//Calvitix, limit the amount of Sea Workers
	WorldSizeTypes eWorldSize = GC.getMap().getWorldSize();
	int iWorldSize = (int)eWorldSize;
	int iMaxSeaWorkers = 4 + int(NB_MAX_SEA_WORKERS * pow((iWorldSize + 1) / 6.0, 0.8));
	return std::min(iNeededSeaWorkers, iMaxSeaWorkers);
}


bool CvCityAI::AI_isDefended(int iExtra, bool bAllowAnyDefenders)
{
	PROFILE_FUNC();

	return getGarrisonStrength(bAllowAnyDefenders) + iExtra >= AI_neededDefenseStrength();
}

bool CvCityAI::AI_isAdequateHappinessMilitary(int iExtra) const
{
	PROFILE_FUNC();

	return ((plot()->plotCount(PUF_isMilitaryHappiness, -1, -1, NULL, getOwner(), NO_TEAM, PUF_isCityAIType) + iExtra) >= AI_neededHappinessDefenders()); // XXX check for other team's units?
}

/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						10/17/08		jdog5000		*/
/* 																			*/
/* 	Air AI																	*/
/********************************************************************************/
/* original BTS code
bool CvCityAI::AI_isAirDefended(int iExtra)
{
	PROFILE_FUNC();

	return ((plot()->plotCount(PUF_canAirDefend, -1, -1, getOwner(), NO_TEAM, PUF_isDomainType, DOMAIN_AIR) + iExtra) >= AI_neededAirDefenders()); // XXX check for other team's units?
}
*/
// Function now answers question of whether city has enough ready air defense, no longer just counts fighters
bool CvCityAI::AI_isAirDefended(bool bCountLand, int iExtra)
{
	PROFILE_FUNC();

	int iAirIntercept = 0;
	int iLandIntercept = 0;

	foreach_(const CvUnit * pLoopUnit, plot()->units())
	{
		if (pLoopUnit->getOwner() == getOwner() &&
			pLoopUnit->canAirDefend())
		{
			if (pLoopUnit->getDomainType() == DOMAIN_AIR)
			{
				// can find units which are already air patrolling using group activity
				if (pLoopUnit->getGroup()->getActivityType() == ACTIVITY_INTERCEPT)
				{
					iAirIntercept += pLoopUnit->currInterceptionProbability();
				}
				else
				{
					// Count air units which can air patrol
					if (pLoopUnit->getDamage() == 0 && !pLoopUnit->hasMoved())
					{
						if (pLoopUnit->AI_getUnitAIType() == UNITAI_DEFENSE_AIR)
						{
							iAirIntercept += pLoopUnit->currInterceptionProbability();
						}
						else
						{
							iAirIntercept += pLoopUnit->currInterceptionProbability() / 3;
						}
					}

				}
			}
			else if (pLoopUnit->getDomainType() == DOMAIN_LAND)
			{
				iLandIntercept += pLoopUnit->currInterceptionProbability();
			}
		}
	}

	int iAirDefenders = iExtra;
	iAirDefenders += (iAirIntercept / 100);

	if (bCountLand)
	{
		iAirDefenders += (iLandIntercept / 100);
	}

	int iNeededAirDefenders = AI_neededAirDefenders();
	bool bHaveEnough = (iAirDefenders >= iNeededAirDefenders);

	return bHaveEnough;
}
/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						END								*/
/********************************************************************************/


int CvCityAI::AI_neededDefenders()
{
	PROFILE_FUNC();

	if (m_neededDefenders != -1)
	{
		return m_neededDefenders;
	}

	if (isHominid())
	{
		return std::max
		(
			1,
			GC.getHandicapInfo(GC.getGame().getHandicapType()).getBarbarianInitialDefenders()
			+ (getPopulation() + 2) / 7
		);
	}

	const int iTurn = GC.getGame().getGameTurn();
	const int iTurnsThreshold = 8 * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 100;

	const CvPlayerAI& player = GET_PLAYER(getOwner());
	int iDefenders = 1;
	const bool bCapital = isCapital();

	if (iTurn > iTurnsThreshold && (bCapital || hasActiveWorldWonder() || isHolyCity()))
	{
		iDefenders++;

		if (player.AI_isDoStrategy(AI_STRATEGY_ALERT1))
		{
			iDefenders++;
		}
		if (player.AI_isDoStrategy(AI_STRATEGY_TURTLE))
		{
			iDefenders += 2;
		}
	}

	if (!player.AI_isDoStrategy(AI_STRATEGY_CRUSH))
	{
		iDefenders += AI_neededFloatingDefenders();
	}
	else iDefenders += (AI_neededFloatingDefenders() + 2) / 4;

	const bool bDefenseWar = area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE;

	if ((bDefenseWar || player.AI_isDoStrategy(AI_STRATEGY_ALERT2)) && !plot()->isHills())
	{
		iDefenders++;
	}

	// If city recently aquired, 80 turns on eternity, 8 turns on Normal, 4 turns on Blitz.
	if (iTurn > 2 * iTurnsThreshold && iTurn - getGameTurnAcquired() <= iTurnsThreshold)
	{
		iDefenders = std::max(2, iDefenders);

		// When on the offensive, halve defenders.
		if ((area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE || area()->getAreaAIType(getTeam()) == AREAAI_MASSING)
			&& !hasActiveWorldWonder() && !isHolyCity())
		{
			iDefenders /= 2;
		}

		if (AI_isDanger())
		{
			iDefenders += 3;
		}

		if (bDefenseWar)
		{
			iDefenders++;
		}
	}

	if (player.AI_isDoStrategy(AI_STRATEGY_LAST_STAND))
	{
		iDefenders += 20;
	}

	if (player.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3)
		&& findCommerceRateRank(COMMERCE_CULTURE) <= GC.getGame().culturalVictoryNumCultureCities())
	{
		iDefenders += 4;

		if (bDefenseWar)
		{
			iDefenders += 2;
		}
	}

	if (player.AI_isDoVictoryStrategy(AI_VICTORY_SPACE3))
	{
		if (bCapital || isProductionProject())
		{
			iDefenders += 4;

			if (bDefenseWar)
			{
				iDefenders += 3;
			}
		}

		if (bCapital && player.AI_isDoVictoryStrategy(AI_VICTORY_SPACE4))
		{
			iDefenders += 6;
		}
	}

	if (GC.getGame().isOption(GAMEOPTION_UNSUPPORTED_REVOLUTION))
	{
		const int iExtraRevDefenders = intSqrt(getRevolutionIndex()) - 10;
		if (iExtraRevDefenders > 0)
		{
			iDefenders += iExtraRevDefenders;
		}
		logBBAI("      City %S extra revolution defenders needed: %d\n\tP.S. negative value is ignored", getName().GetCString(), iExtraRevDefenders);
	}

	const int iHappyPerUnit = player.getHappyPerMilitaryUnit();
	if (iHappyPerUnit != 0)
	{
		const int iHappiness = happyLevel() - unhappyLevel(0);
		if (iHappiness < 0)
		{
			iDefenders -= iHappyPerUnit * iHappiness / (1 + GET_TEAM(getTeam()).getAtWarCount(true));
		}
	}

	iDefenders = std::max(iDefenders, AI_minDefenders());

	m_neededDefenders = iDefenders;

	return iDefenders;
}


int CvCityAI::AI_neededHappinessDefenders() const
{
	int iResult = 0;
	if (GET_PLAYER(getOwner()).getHappyPerMilitaryUnit() > 0)
	{
		int iCurrent = plot()->plotCount(PUF_isMilitaryHappiness, -1, -1, NULL, getOwner(), NO_TEAM);
		int iHappiness = happyLevel() - unhappyLevel(0);
		int iBaseHappiness = iHappiness - GET_PLAYER(getOwner()).getHappyPerMilitaryUnit() * iCurrent;

		if (iBaseHappiness < 0)
		{
			iResult = -iBaseHappiness / GET_PLAYER(getOwner()).getHappyPerMilitaryUnit();
		}
	}

	return iResult;
}

int CvCityAI::AI_minDefenders() const
{
	return (3 + (GET_PLAYER(getOwner()).getCurrentEra() + 2)) / 2;
}

int CvCityAI::getGarrisonStrength(bool bAllowAnyDefenders) const
{
	PROFILE_FUNC();

	//	Note - the range check is slightly flawed since its possible for units 2 away to not actually
	//	be able to get to the city (e.g. - over water) so this probably needs tweaking slightly
	return plot()->plotStrength(UNITVALUE_FLAGS_DEFENSIVE, PUF_canDefend, -1, -1, getOwner(), NO_TEAM, PUF_isCityGarrison, getID(), (int)bAllowAnyDefenders, 2);
}

int CvCityAI::AI_neededDefenseStrength()
{
	//	For now we just use the old calculation for needed defenders (a count)
	//	and multiply it by the base strength of the best defender we can currently build
	return AI_neededDefenders() * GET_PLAYER(getOwner()).strengthOfBestUnitAI(DOMAIN_LAND, UNITAI_CITY_DEFENSE);
}

int CvCityAI::AI_minDefenseStrength() const
{
	//	For now we just use the old calculation for needed defenders (a count)
	//	and multiply it by the base strength of the best defender we can currently build
	return AI_minDefenders() * GET_PLAYER(getOwner()).strengthOfBestUnitAI(DOMAIN_LAND, UNITAI_CITY_DEFENSE);
}

int CvCityAI::AI_neededFloatingDefenders()
{
	if (m_iNeededFloatingDefendersCacheTurn != GC.getGame().getGameTurn())
	{
		AI_updateNeededFloatingDefenders();
	}
	return m_iNeededFloatingDefenders;
}

void CvCityAI::AI_updateNeededFloatingDefenders()
{
	int iFloatingDefenders = GET_PLAYER(getOwner()).AI_getTotalFloatingDefendersNeeded(area());

	int iTotalThreat = std::max(1, GET_PLAYER(getOwner()).AI_getTotalAreaCityThreat(area()));

	int iThreatModifier = 0;

	//	Use the threat modifier (which is based on danger from higher-difficulty setting players
	//	which usually means humans!) to increase local floating defender needs
	iFloatingDefenders *= AI_cityThreat(NO_TEAM, &iThreatModifier);
	iFloatingDefenders = (iFloatingDefenders * (100 + iThreatModifier)) / 100;
	iFloatingDefenders += (iTotalThreat / 2);
	iFloatingDefenders /= iTotalThreat;

	m_iNeededFloatingDefenders = iFloatingDefenders;
	m_iNeededFloatingDefendersCacheTurn = GC.getGame().getGameTurn();
}

int CvCityAI::AI_neededAirDefenders() const
{
	PROFILE_EXTRA_FUNC();
	int iOtherTeam = 0;
	int iEnemyTeam = 0;
	foreach_(const CvPlot * pLoopPlot, plot()->rect(5, 5))
	{
		if (pLoopPlot->isOwned() && pLoopPlot->getTeam() != getTeam())
		{
			iOtherTeam++;
			if (GET_TEAM(getTeam()).AI_getWarPlan(pLoopPlot->getTeam()) != NO_WARPLAN)
			{
				// If enemy has no bombers, don't need to defend as much
				if (GET_PLAYER(pLoopPlot->getOwner()).AI_totalUnitAIs(UNITAI_ATTACK_AIR) == 0)
				{
					continue;
				}
				iEnemyTeam += 2;
				if (pLoopPlot->isCity())
				{
					iEnemyTeam += 6;
				}
			}
		}
	}

	const int iDefenders = std::max((iEnemyTeam > 0) ? 2 : 0, (iOtherTeam + iEnemyTeam + 2) / 8);

	if (iDefenders == 0 && isCoastal(GC.getWorldInfo(GC.getMap().getWorldSize()).getOceanMinAreaSize()))
	{
		return 1;
	}
	return iDefenders;
}

#define	MAX_DANGER_EVALUATION_RANGE		6
#define	DISTANCE_ATTENUATION_FACTOR(x)	((2*(x))/5)
#define	NEUTRAL_BORDER_PLOT_DANGER		5
#define	OWNED_PLOT_NON_WAR_BASE_DANGER	10
#define	OWNED_PLOT_WAR_DANGER			30

//	Koshling - provide a numerical evaluation of city danger from 0 (very safe) to 100 (about to be lost)
int CvCityAI::evaluateDanger()
{
	PROFILE_FUNC();

	CvUnitSelectionCriteria	noGrowthCriteria;

	noGrowthCriteria.m_bIgnoreGrowth = true;

	UnitTypes eDummyUnit = GET_PLAYER(getOwner()).bestBuildableUnitForAIType(DOMAIN_LAND, UNITAI_ATTACK, &noGrowthCriteria);

	if (eDummyUnit == NO_UNIT)
	{
		eDummyUnit = GET_PLAYER(getOwner()).bestBuildableUnitForAIType(DOMAIN_LAND, UNITAI_CITY_DEFENSE, &noGrowthCriteria);
	}
	if (eDummyUnit == NO_UNIT)
	{
		// Should never happen but empirically it does (very rarely) - needs future investigation
		FErrorMsg(CvString::format("Cannot find any buildable units to use for strength test asking city is %S, owned by player %d", m_szName.c_str(), getOwner()).c_str());
		return 100;
	}

	CvUnit* pTempUnit = GET_PLAYER(getOwner()).getTempUnit(eDummyUnit, getX(), getY());

	CvReachablePlotSet plotSet(pTempUnit->getGroup(), 0, MAX_DANGER_EVALUATION_RANGE, false, 1);

	int iBorderDanger = 0;
	int iDistanceModifier[MAX_DANGER_EVALUATION_RANGE];

	iDistanceModifier[0] = 1000;
	for (int iI = 1; iI < MAX_DANGER_EVALUATION_RANGE; iI++)
	{
		iDistanceModifier[iI] = DISTANCE_ATTENUATION_FACTOR(iDistanceModifier[iI - 1]);
	}

	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		CvPlot* pLoopPlot = itr.plot();
		TeamTypes ePlotTeam = pLoopPlot->getTeam();

		if (ePlotTeam != getTeam())
		{
			if (ePlotTeam == NO_TEAM)
			{
				// Neutral territory border
				iBorderDanger += NEUTRAL_BORDER_PLOT_DANGER * iDistanceModifier[itr.stepDistance() - 1];
			}
			else if (!GET_TEAM(getTeam()).isAtWar(ePlotTeam))
			{
				// Border with a player we are not at war with - weight by attitude
				int iModifier;
				int iTempBorderDanger =
					(
						OWNED_PLOT_NON_WAR_BASE_DANGER
						*
						getPlayerDangerPercentage(pLoopPlot->getOwner(), iModifier)
						*
						iDistanceModifier[itr.stepDistance() - 1]
						/
						100
						);
				iBorderDanger += iTempBorderDanger * (100 + iModifier) / 100;
			}
			// Border with someone we are at war with
			else iBorderDanger += OWNED_PLOT_WAR_DANGER * iDistanceModifier[itr.stepDistance() - 1];
		}
	}
	GET_PLAYER(getOwner()).releaseTempUnit();

	return iBorderDanger / 100;
}

bool CvCityAI::AI_isDanger() const
{
	return GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot(), 2, false);
}


int CvCityAI::AI_getEmphasizeAvoidGrowthCount() const
{
	return m_iEmphasizeAvoidGrowthCount;
}


bool CvCityAI::AI_isEmphasizeAvoidGrowth() const
{
	return (AI_getEmphasizeAvoidGrowthCount() > 0);
}


int CvCityAI::AI_getEmphasizeGreatPeopleCount() const
{
	return m_iEmphasizeGreatPeopleCount;
}


bool CvCityAI::AI_isEmphasizeGreatPeople() const
{
	return (AI_getEmphasizeGreatPeopleCount() > 0);
}


bool CvCityAI::AI_isAssignWorkDirty() const
{
	return m_bAssignWorkDirty;
}


void CvCityAI::AI_setAssignWorkDirty(bool bNewValue)
{
	m_bAssignWorkDirty = bNewValue;
}


bool CvCityAI::AI_isChooseProductionDirty() const
{
	return m_bChooseProductionDirty;
}


void CvCityAI::AI_setChooseProductionDirty(bool bNewValue)
{
	m_bChooseProductionDirty = bNewValue;
}


CvCity* CvCityAI::AI_getRouteToCity() const
{
	if (!m_routeToCityUpdated)
	{
		AI_updateRouteToCity();

		m_routeToCityUpdated = true;
	}
	return getCity(m_routeToCity);
}


void CvCityAI::AI_updateRouteToCity() const
{
	PROFILE_FUNC();

	static PlayerTypes eCachedPlayer = NO_PLAYER;
	static stdext::hash_map<int, bool>* allAreaCitiesConnected = NULL;

	if (allAreaCitiesConnected == NULL)
	{
		allAreaCitiesConnected = new stdext::hash_map<int, bool>();
	}

	if (eCachedPlayer != getOwner())
	{
		eCachedPlayer = getOwner();
		allAreaCitiesConnected->clear();
	}

	const CvCity* pBestCity = NULL;
	stdext::hash_map<int, bool>::const_iterator itr;
	if ((itr = allAreaCitiesConnected->find(area()->getID())) == allAreaCitiesConnected->end() ||
		!(itr->second))
	{
		gDLL->getFAStarIFace()->ForceReset(&GC.getRouteFinder());

		int iBestValue = MAX_INT;

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
			{
				foreach_(CvCity * pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
				{
					if (pLoopCity != this)
					{
						if (pLoopCity->area() == area())
						{
							if (!gDLL->getFAStarIFace()->GeneratePath(&GC.getRouteFinder(), getX(), getY(), pLoopCity->getX(), pLoopCity->getY(), false, getOwner(), true))
							{
								const int iValue = plotDistance(getX(), getY(), pLoopCity->getX(), pLoopCity->getY());

								if (iValue < iBestValue)
								{
									iBestValue = iValue;
									pBestCity = pLoopCity;
								}
							}
						}
					}
				}
			}
		}

		if (itr == allAreaCitiesConnected->end())
		{
			(*allAreaCitiesConnected)[area()->getID()] = (pBestCity == NULL);
		}
	}

	if (pBestCity != NULL)
	{
		m_routeToCity = pBestCity->getIDInfo();
	}
	else
	{
		m_routeToCity.reset();
	}
}


bool CvCityAI::AI_isEmphasizeYield(const YieldTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	return m_aiEmphasizeYieldCount[eIndex] > 0;
}

bool CvCityAI::AI_isEmphasizeCommerce(const CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_aiEmphasizeCommerceCount[eIndex] > 0;
}

bool CvCityAI::AI_isAnyCommerceOrYieldEmphasis() const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		if (m_aiEmphasizeCommerceCount[iI] > 0)
		{
			return true;
		}
	}
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (m_aiEmphasizeYieldCount[iI] > 0)
		{
			return true;
		}
	}
	return false;
}


bool CvCityAI::AI_isEmphasize(EmphasizeTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumEmphasizeInfos(), eIndex);
	FAssertMsg(m_pbEmphasize != NULL, "m_pbEmphasize is not expected to be equal with NULL");
	return m_pbEmphasize[eIndex];
}

bool CvCityAI::AI_isEmphasizeSpecialist(SpecialistTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex);
	FAssertMsg(m_pbEmphasizeSpecialist != NULL, "m_pbEmphasize is not expected to be equal with NULL");
	return m_pbEmphasizeSpecialist[eIndex];
}


void CvCityAI::AI_setEmphasize(EmphasizeTypes eIndex, bool bNewValue)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumEmphasizeInfos(), eIndex);

	if (AI_isEmphasize(eIndex) != bNewValue)
	{
		m_pbEmphasize[eIndex] = bNewValue;

		if (GC.getEmphasizeInfo(eIndex).isAvoidGrowth())
		{
			m_iEmphasizeAvoidGrowthCount += AI_isEmphasize(eIndex) ? 1 : -1;
			FASSERT_NOT_NEGATIVE(AI_getEmphasizeAvoidGrowthCount());
		}

		if (GC.getEmphasizeInfo(eIndex).isGreatPeople())
		{
			m_iEmphasizeGreatPeopleCount += AI_isEmphasize(eIndex) ? 1 : -1;
			FASSERT_NOT_NEGATIVE(AI_getEmphasizeGreatPeopleCount());
		}

		if (GC.getEmphasizeInfo(eIndex).isAvoidAngryCitizens())
		{
			m_iEmphasizeAvoidAngryCitizensCount += AI_isEmphasize(eIndex) ? 1 : -1;
			FASSERT_NOT_NEGATIVE(AI_getEmphasizeAvoidAngryCitizensCount());
		}

		if (GC.getEmphasizeInfo(eIndex).isAvoidUnhealthyCitizens())
		{
			m_iEmphasizeAvoidUnhealthyCitizensCount += AI_isEmphasize(eIndex) ? 1 : -1;
			FASSERT_NOT_NEGATIVE(AI_getEmphasizeAvoidUnhealthyCitizensCount());
		}

		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			if (GC.getEmphasizeInfo(eIndex).getYieldChange(iI))
			{
				m_aiEmphasizeYieldCount[iI] += AI_isEmphasize(eIndex) ? 1 : -1;
				FASSERT_NOT_NEGATIVE(m_aiEmphasizeYieldCount[iI]);
			}
		}

		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			if (GC.getEmphasizeInfo(eIndex).getCommerceChange(iI))
			{
				m_aiEmphasizeCommerceCount[iI] += AI_isEmphasize(eIndex) ? 1 : -1;
				FASSERT_NOT_NEGATIVE(m_aiEmphasizeCommerceCount[iI]);
			}
		}

		AI_assignWorkingPlots();

		//	If we're using AI govenors and not part way through a build reflect
		//	the changes in a new production choice immediately
		if (isHuman() && (!isProduction() || getProductionProgress() == 0) && isProductionAutomated() && GET_PLAYER(getOwner()).isOption(PLAYEROPTION_MODDER_3))
		{
			AI_chooseProduction();
		}
	}
}

void CvCityAI::AI_setEmphasizeSpecialist(SpecialistTypes eIndex, bool bNewValue)
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex);

	if (AI_isEmphasizeSpecialist(eIndex) != bNewValue)
	{
		m_pbEmphasizeSpecialist[eIndex] = bNewValue;

		AI_assignWorkingPlots();
	}
}

void CvCityAI::AI_forceEmphasizeCulture(bool bNewValue)
{
	if (m_bForceEmphasizeCulture != bNewValue)
	{
		m_bForceEmphasizeCulture = bNewValue;

		m_aiEmphasizeCommerceCount[COMMERCE_CULTURE] += (bNewValue ? 1 : -1);
		FASSERT_NOT_NEGATIVE(m_aiEmphasizeCommerceCount[COMMERCE_CULTURE]);
	}
}


int CvCityAI::AI_getBestBuildValue(int iIndex) const
{
	FASSERT_BOUNDS(0, NUM_CITY_PLOTS, iIndex);
	return m_aiBestBuildValue[iIndex];
}


int CvCityAI::AI_totalBestBuildValue(const CvArea* pArea) const
{
	PROFILE_EXTRA_FUNC();
	int iTotalValue = 0;

	for (int iI = SKIP_CITY_HOME_PLOT; iI < getNumCityPlots(); iI++)
	{
		const CvPlot* pLoopPlot = plotCity(getX(), getY(), iI);

		if (pLoopPlot != NULL && pLoopPlot->area() == pArea
			&& (
				pLoopPlot->getImprovementType() == NO_IMPROVEMENT
				||
				!GET_PLAYER(getOwner()).isOption(PLAYEROPTION_SAFE_AUTOMATION)
				||
				pLoopPlot->getImprovementType() == GC.getIMPROVEMENT_CITY_RUINS()
				)
			) iTotalValue += AI_getBestBuildValue(iI);
	}
	return iTotalValue;
}

int CvCityAI::AI_clearFeatureValue(int iIndex) const
{
	const CvPlot* pPlot = plotCity(getX(), getY(), iIndex);
	FAssert(pPlot != NULL);

	const FeatureTypes eFeature = pPlot->getFeatureType();

	if (eFeature == NO_FEATURE)
		return 0;

	FAssert(eFeature != NO_FEATURE);

	const CvFeatureInfo& kFeatureInfo = GC.getFeatureInfo(eFeature);

	int iValue = 0;
	iValue += kFeatureInfo.getYieldChange(YIELD_FOOD) * 100;
	iValue += kFeatureInfo.getYieldChange(YIELD_PRODUCTION) * 60;
	iValue += kFeatureInfo.getYieldChange(YIELD_COMMERCE) * 40;

	if (iValue > 0 && pPlot->isBeingWorked())
	{
		iValue *= 3;
		iValue /= 2;
	}
	if (iValue != 0)
	{
		const BonusTypes eBonus = pPlot->getBonusType(getTeam());
		if (eBonus != NO_BONUS)
		{
			iValue *= 3;
			if (pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				if (GC.getImprovementInfo(pPlot->getImprovementType()).isImprovementBonusTrade(eBonus))
				{
					iValue *= 4;
				}
			}
		}
	}

	int iHealthValue = 0;
	if (kFeatureInfo.getHealthPercent() != 0)
	{
		const int iHealth = goodHealth() - badHealth();

		//speed up Jungle Clearing
		const int iMultiplier = kFeatureInfo.getHealthPercent() > 0 ? 6 : 10;
		iHealthValue += (iMultiplier * kFeatureInfo.getHealthPercent()) / std::max(3, 1 + iHealth);

		if (iHealthValue > 0 && !pPlot->isBeingWorked())
		{
			iHealthValue *= 3;
			iHealthValue /= 2;
		}
	}
	iValue += iHealthValue;

	if (iValue > 0)
	{
		if (pPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			if (GC.getImprovementInfo(pPlot->getImprovementType()).isRequiresFeature())
			{
				iValue += 500;
			}
		}

		if (GET_PLAYER(getOwner()).getAdvancedStartPoints() >= 0)
		{
			iValue += 400;
		}
	}

	return -iValue;
}

namespace {
	// Score a BuildType on a plot for how many turns it takes to build it
	int score_build_type(const BuildTypes buildType, const CvPlot* plot)
	{
		return plot->getBuildTurnsLeft(buildType, 0, 0);
	}
};

int CvCityAI::AI_getGoodTileCount() const
{
	PROFILE_EXTRA_FUNC();
	int aiFinalYields[NUM_YIELD_TYPES];

	const CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	int iGoodTileCount = 0;
	for (int iI = SKIP_CITY_HOME_PLOT; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(iI);

		if (NULL != pLoopPlot && pLoopPlot->getWorkingCity() == this)
		{
			const int iCount = kPlayer.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD);

			bool bUseBaseValue = true;
			//If the tile has a BestBuild and is being improved, then use the BestBuild
			//determine if the tile is being improved.

			if (iCount > 0)
			{
				BuildTypes eBuild = NO_BUILD;
				if (m_aeBestBuild[iI] != NO_BUILD && m_aiBestBuildValue[iI] > 0)
				{
					eBuild = m_aeBestBuild[iI];
				}
				else
				{
					// This check is necessary to stop oscillation which can result
					// when best build changes food situation for city, changing the best build.
					// this signature is erroring out in vscode, but it does compile and work as intended, so we ignore the error
					eBuild = scoring::min_score(
						pLoopPlot->units() | filtered(CvUnit::fn::getBuildType() != NO_BUILD)
						| transformed(CvUnit::fn::getBuildType()),
						bind(score_build_type, _1, pLoopPlot)
					).get_value_or(NO_BUILD);
				}

				if (eBuild != NO_BUILD)
				{
					const ImprovementTypes eImprovement = GC.getBuildInfo(eBuild).getImprovement();
					if (eImprovement != NO_IMPROVEMENT)
					{
						bool bIgnoreFeature = false;
						if (pLoopPlot->getFeatureType() != NO_FEATURE)
						{
							if (GC.getBuildInfo(eBuild).isFeatureRemove(pLoopPlot->getFeatureType()))
							{
								bIgnoreFeature = true;
							}
						}

						bUseBaseValue = false;
						for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
						{
							aiFinalYields[iJ] = (pLoopPlot->calculateNatureYield(((YieldTypes)iJ), getTeam(), bIgnoreFeature) + pLoopPlot->calculateImprovementYieldChange(eImprovement, ((YieldTypes)iJ), getOwner(), false));
						}
					}
				}
			}

			//Otherwise use the base value.
			if (bUseBaseValue)
			{
				for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					//by default we'll use the current value
					aiFinalYields[iJ] = pLoopPlot->getYield((YieldTypes)iJ);
					if (pLoopPlot->getFeatureType() != NO_FEATURE)
					{
						aiFinalYields[iJ] += std::max(0, -GC.getFeatureInfo(pLoopPlot->getFeatureType()).getYieldChange((YieldTypes)iJ));
					}
				}
			}

			if (((aiFinalYields[YIELD_FOOD] * 10) + (aiFinalYields[YIELD_PRODUCTION] * 6) + (aiFinalYields[YIELD_COMMERCE] * 4)) > 21)
			{
				iGoodTileCount++;
			}
		}
	}

	return iGoodTileCount;
}

int CvCityAI::AI_countWorkedPoorTiles() const
{
	PROFILE_EXTRA_FUNC();
	int aiFinalYields[NUM_YIELD_TYPES];

	const CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	int iWorkedPoorTileCount = 0;
	for (int iI = SKIP_CITY_HOME_PLOT; iI < NUM_CITY_PLOTS; iI++)
	{
		const CvPlot* pLoopPlot = getCityIndexPlot(iI);

		if (NULL != pLoopPlot && pLoopPlot->getWorkingCity() == this && pLoopPlot->isBeingWorked())
		{
			const int iCount = kPlayer.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD);

			bool bUseBaseValue = true;
			//If the tile has a BestBuild and is being improved, then use the BestBuild
			//determine if the tile is being improved.

			if (iCount > 0)
			{
				BuildTypes eBuild = NO_BUILD;
				if (m_aeBestBuild[iI] != NO_BUILD && m_aiBestBuildValue[iI] > 0)
				{
					eBuild = m_aeBestBuild[iI];
				}
				else
				{
					// This check is necessary to stop oscillation which can result
					// when best build changes food situation for city, changing the best build.
					foreach_(const CvUnit * unit, pLoopPlot->units())
					{
						if (unit->getBuildType() != NO_BUILD)
						{
							if (eBuild == NO_BUILD || pLoopPlot->getBuildTurnsLeft(eBuild, 0, 0) > pLoopPlot->getBuildTurnsLeft(unit->getBuildType(), 0, 0))
							{
								eBuild = unit->getBuildType();
							}
						}
					}
				}

				if (eBuild != NO_BUILD)
				{
					const ImprovementTypes eImprovement = GC.getBuildInfo(eBuild).getImprovement();
					if (eImprovement != NO_IMPROVEMENT)
					{
						bool bIgnoreFeature = false;
						if (pLoopPlot->getFeatureType() != NO_FEATURE)
						{
							if (GC.getBuildInfo(eBuild).isFeatureRemove(pLoopPlot->getFeatureType()))
							{
								bIgnoreFeature = true;
							}
						}

						bUseBaseValue = false;
						for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
						{
							aiFinalYields[iJ] = (pLoopPlot->calculateNatureYield(((YieldTypes)iJ), getTeam(), bIgnoreFeature) + pLoopPlot->calculateImprovementYieldChange(eImprovement, ((YieldTypes)iJ), getOwner(), false));
						}
					}
				}
			}

			//Otherwise use the base value.
			if (bUseBaseValue)
			{
				for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					//by default we'll use the current value
					aiFinalYields[iJ] = pLoopPlot->getYield((YieldTypes)iJ);
					if (pLoopPlot->getFeatureType() != NO_FEATURE)
					{
						aiFinalYields[iJ] += std::max(0, -GC.getFeatureInfo(pLoopPlot->getFeatureType()).getYieldChange((YieldTypes)iJ));
					}
				}
			}

			if (((aiFinalYields[YIELD_FOOD] * 10) + (aiFinalYields[YIELD_PRODUCTION] * 6) + (aiFinalYields[YIELD_COMMERCE] * 4)) <= 21)
			{
				iWorkedPoorTileCount++;
			}
		}
	}

	return iWorkedPoorTileCount;
}

int CvCityAI::AI_getTargetSize() const
{
	PROFILE_FUNC();

	const CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	int iTargetSize = AI_getGoodTileCount();

	int iHappyAdjust = 0;
	int iHealthAdjust = 0;
	if (getProductionBuilding() != NO_BUILDING)
	{
		iHappyAdjust += getAdditionalHappinessByBuilding(getProductionBuilding());
		iHealthAdjust += getAdditionalHealthByBuilding(getProductionBuilding());
	}

	iTargetSize -= std::max(0, (iTargetSize - (1 + getPopulation() + goodHealth() - badHealth() + getEspionageHealthCounter() + std::max(0, iHealthAdjust))) / 2);

	if (iTargetSize < getPopulation())
	{
		iTargetSize = std::max(iTargetSize, getPopulation() - (AI_countWorkedPoorTiles() / 2));
	}

	// Target city size should not be perturbed by espionage, other short term effects
	iTargetSize = std::min(iTargetSize, getPopulation() + (happyLevel() - unhappyLevel() + getEspionageHappinessCounter() + std::max(0, iHappyAdjust)));

	if (kPlayer.getAdvancedStartPoints() >= 0)
	{
		iTargetSize += 2 + kPlayer.getCurrentEra();
	}

	return iTargetSize;
}


void CvCityAI::AI_getYieldMultipliers(int& iFoodMultiplier, int& iProductionMultiplier, int& iCommerceMultiplier, int& iDesiredFoodChange) const
{
	PROFILE_EXTRA_FUNC();
	iFoodMultiplier = 100;
	iCommerceMultiplier = 100;
	iProductionMultiplier = 100;
	iDesiredFoodChange = 0;

	const CvPlayerAI& kPlayer = GET_PLAYER(getOwner());

	int aiFinalYields[NUM_YIELD_TYPES];

	int iBonusFoodSurplus = 0;
	int iBonusFoodDeficit = 0;
	int iFeatureFoodSurplus = 0;
	int iHillFoodDeficit = 0;
	int iFoodTotal = GC.getYieldInfo(YIELD_FOOD).getMinCity();
	int iProductionTotal = GC.getYieldInfo(YIELD_PRODUCTION).getMinCity();

	for (int iI = SKIP_CITY_HOME_PLOT; iI < getNumCityPlots(); iI++)
	{
		const CvPlot* pLoopPlot = getCityIndexPlot(iI);

		if (NULL != pLoopPlot && pLoopPlot->getWorkingCity() == this)
		{
			bool bUseBaseValue = true;
			// If the tile has a BestBuild or is being improved,
			// then use the BestBuild determine if the tile is being improved.
			if (kPlayer.AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD) > 0)
			{
				BuildTypes eBuild = NO_BUILD;
				if (m_aeBestBuild[iI] != NO_BUILD && m_aiBestBuildValue[iI] > 0)
				{
					eBuild = m_aeBestBuild[iI];
				}
				else
				{
					// This check is necessary to stop oscillation which can result
					// when best build changes food situation for city.
					eBuild = scoring::min_score(pLoopPlot->units()
						| filtered(CvUnit::fn::getBuildType() != NO_BUILD)
						| transformed(CvUnit::fn::getBuildType()),
						bind(score_build_type, _1, pLoopPlot)
					).get_value_or(NO_BUILD);
				}

				if (eBuild != NO_BUILD)
				{
					const ImprovementTypes eImprovement = GC.getBuildInfo(eBuild).getImprovement();
					if (eImprovement != NO_IMPROVEMENT)
					{
						const bool bIgnoreFeature = pLoopPlot->getFeatureType() != NO_FEATURE && GC.getBuildInfo(eBuild).isFeatureRemove(pLoopPlot->getFeatureType());
						bUseBaseValue = false;
						for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
						{
							aiFinalYields[iJ] = pLoopPlot->calculateNatureYield((YieldTypes)iJ, getTeam(), bIgnoreFeature) + pLoopPlot->calculateImprovementYieldChange(eImprovement, (YieldTypes)iJ, getOwner(), false);
						}
					}
				}
			}
			//Otherwise use the base value.
			if (bUseBaseValue)
			{
				for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					//by default we'll use the current value
					aiFinalYields[iJ] = pLoopPlot->getYield((YieldTypes)iJ);
					if (pLoopPlot->getFeatureType() != NO_FEATURE)
					{
						aiFinalYields[iJ] += std::max(0, -GC.getFeatureInfo(pLoopPlot->getFeatureType()).getYieldChange((YieldTypes)iJ));
					}
				}
			}

			if (pLoopPlot->isBeingWorked() || 21 < aiFinalYields[YIELD_FOOD] * 10 + aiFinalYields[YIELD_PRODUCTION] * 6 + aiFinalYields[YIELD_COMMERCE] * 4)
			{
				if (pLoopPlot->isBeingWorked())
				{
					iFoodTotal += aiFinalYields[YIELD_FOOD];
				}
				else
				{
					iFoodTotal += aiFinalYields[YIELD_FOOD] / 2;
				}
				if (aiFinalYields[YIELD_PRODUCTION] > 1)
				{
					iProductionTotal += aiFinalYields[YIELD_PRODUCTION];
				}
			}

			if (pLoopPlot->getBonusType(getTeam()) != NO_BONUS)
			{
				const int iNetFood = aiFinalYields[YIELD_FOOD] - GC.getFOOD_CONSUMPTION_PER_POPULATION();
				if (0 < iNetFood)
				{
					iBonusFoodSurplus += iNetFood;
				}
				else if (iNetFood < 0)
				{
					iBonusFoodDeficit -= iNetFood;
				}
			}

			if (pLoopPlot->getFeatureType() != NO_FEATURE)
			{
				iFeatureFoodSurplus += std::max(0, pLoopPlot->calculateNatureYield(YIELD_FOOD, getTeam()) - GC.getFOOD_CONSUMPTION_PER_POPULATION());
			}

			if ((pLoopPlot->isHills()))
			{
				iHillFoodDeficit += std::max(0, GC.getFOOD_CONSUMPTION_PER_POPULATION() - pLoopPlot->calculateNatureYield(YIELD_FOOD, getTeam()));
			}
		}
	}


	const int iBonusFoodDiff = iBonusFoodSurplus + iFeatureFoodSurplus - iBonusFoodDeficit - iHillFoodDeficit / 2;
	if (iBonusFoodDiff < 2)
	{
		iFoodMultiplier += 10 * (2 - iBonusFoodDiff);
	}
	const int iTargetSize = AI_getTargetSize();

	int iExtraFoodForGrowth = std::max(0, iTargetSize - getPopulation() + 3) / 4;
	if (getPopulation() < iTargetSize)
	{
		iExtraFoodForGrowth++;
	}

	const int iFoodDifference = iFoodTotal - iExtraFoodForGrowth - iTargetSize * getFoodConsumedPerPopulation100() / 100;

	iDesiredFoodChange = -iFoodDifference + std::max(0, badHealth() - goodHealth());
	if (iDesiredFoodChange > 3 && iTargetSize > getPopulation())
	{
		iDesiredFoodChange = (iDesiredFoodChange + 3) / 2;
	}

	if (iFoodDifference > 4)
	{
		iFoodMultiplier -= 8 + 4 * iFoodDifference;
	}
	else if (iFoodDifference < 0)
	{
		iFoodMultiplier += -iFoodDifference * 4;
	}

	if (iProductionTotal < 10)
	{
		iProductionMultiplier += (80 - 8 * iProductionTotal);
	}
	const int iProductionTarget = 1 + std::min(getPopulation(), iTargetSize * 3 / 5);

	if (iProductionTotal < iProductionTarget)
	{
		iProductionMultiplier += 8 * (iProductionTarget - iProductionTotal);
	}

	if (iBonusFoodSurplus + iFeatureFoodSurplus > 5 && iBonusFoodDeficit + iHillFoodDeficit > 8)
	{
		//probably a good candidate for a wonder pump
		iProductionMultiplier += 40;

		if (kPlayer.AI_isFinancialTrouble())
		{
			iCommerceMultiplier -= 40;
		}
	}

	const short iProfitMargin = kPlayer.getProfitMargin();

	if (iProfitMargin < 50)
	{
		iCommerceMultiplier += (50 - iProfitMargin) * 33 / 50;
	}
	// AI no longer uses emphasis except for short term boosts.
	if (isHuman())
	{
		if (AI_isEmphasizeYield(YIELD_FOOD))
		{
			iFoodMultiplier *= 130;
			iFoodMultiplier /= 100;
		}
		if (AI_isEmphasizeYield(YIELD_PRODUCTION))
		{
			iProductionMultiplier *= 140;
			iProductionMultiplier /= 100;
		}
		if (AI_isEmphasizeYield(YIELD_COMMERCE))
		{
			iCommerceMultiplier *= 140;
			iCommerceMultiplier /= 100;
		}
	}
	// Now we normalize the effect by # of cities
	const int iNumCities = kPlayer.getNumCities();
	// In short, the relative multipliers should *never* make a difference in an OCC
	// so this indeed equals "100" for the iNumCities == 1 case.
	if (iNumCities > 1)
	{
		int iProductionAdvantage = 100 * AI_yieldMultiplier(YIELD_PRODUCTION) / kPlayer.AI_averageYieldMultiplier(YIELD_PRODUCTION);

		iProductionAdvantage *= kPlayer.AI_averageYieldMultiplier(YIELD_COMMERCE);
		iProductionAdvantage /= AI_yieldMultiplier(YIELD_COMMERCE);

		iProductionAdvantage = (iProductionAdvantage * (iNumCities - 1) + 200) / (iNumCities + 1);

		iProductionMultiplier *= iProductionAdvantage;
		iProductionMultiplier /= 100;

		iCommerceMultiplier *= 100;
		iCommerceMultiplier /= iProductionAdvantage;

		int iGreatPeopleAdvantage = 100 * getTotalGreatPeopleRateModifier() / kPlayer.AI_averageGreatPeopleMultiplier();

		iGreatPeopleAdvantage = (iGreatPeopleAdvantage * (iNumCities - 1) + 200) / (iNumCities + 1);
		iGreatPeopleAdvantage += 200; //gpp multipliers are larger than others so lets not go overboard
		iGreatPeopleAdvantage /= 3;

		//With great people we want to slightly increase food priority at the expense of commerce
		//this gracefully handles both wonder and specialist based GPP...
		iCommerceMultiplier *= 100;
		iCommerceMultiplier /= iGreatPeopleAdvantage;
		iFoodMultiplier *= iGreatPeopleAdvantage;
		iFoodMultiplier /= 100;
	}

	// if leader flavor likes production, increase production, reduce commerce
	if (kPlayer.AI_isDoStrategy(AI_STRATEGY_PRODUCTION))
	{
		iProductionMultiplier += 10;
		iCommerceMultiplier -= 10;
	}

	if (iFoodMultiplier < 100)
	{
		iFoodMultiplier = 10000 / (200 - iFoodMultiplier);
	}
	if (iProductionMultiplier < 100)
	{
		iProductionMultiplier = 10000 / (200 - iProductionMultiplier);
	}
	if (iCommerceMultiplier < 100)
	{
		iCommerceMultiplier = 10000 / (200 - iCommerceMultiplier);
	}
	if (angryPopulation(1) > 0)
	{
		iFoodMultiplier /= 2;
	}
}


namespace {
	const struct BuildsImprovement
	{
		BuildsImprovement(ImprovementTypes improvement) : improvement(improvement) {}
		bool operator()(const BuildTypes buildType) const
		{
			if (buildType == NO_BUILD) return false;
			return GC.getBuildInfo(buildType).getImprovement() == improvement;
		}
		const ImprovementTypes improvement;
	};

	const struct BuildsAnyImprovement
	{
		bool operator()(const BuildTypes buildType) const
		{
			if (buildType == NO_BUILD) return false;
			return GC.getBuildInfo(buildType).getImprovement() != NO_IMPROVEMENT;
		}
	};
};


// Only used for improvement upgrading.
int CvCityAI::AI_getImprovementValue(const CvPlot* pPlot, ImprovementTypes eImprovement, int iFoodPriority, int iProductionPriority, int iCommercePriority, int iFoodChange) const
{
	PROFILE_EXTRA_FUNC();
	FAssert(eImprovement != NO_IMPROVEMENT);

	const CvImprovementInfo& improvement = GC.getImprovementInfo(eImprovement);
	const BonusTypes eBonus = pPlot->getBonusType(getTeam());
	int iValue = 100;

	if (eBonus == NO_BONUS)
	{
		/*
		for (int iI = GC.getNumMapBonuses() - 1; iI > -1; iI--)
		{
			if (improvement.getImprovementBonusDiscoverRand(GC.getMapBonus(iI)) > 0)
			{
				iValue += 0;
			}
		}
		*/
	}
	else if (!GET_TEAM(getTeam()).isBonusObsolete(eBonus))
	{
		if (improvement.isImprovementBonusTrade(eBonus))
		{
			iValue *= 50;
		}
		else if (pPlot->getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(pPlot->getImprovementType()).isImprovementBonusTrade(eBonus))
		{
			return 0; //Always prefer improvements which connect bonuses.
		}
	}
	if (improvement.isCarriesIrrigation()) iValue += 200;

	const int iCultureRange = improvement.getCultureRange() + 1;
	// Blaze: Comment out culture&range to not overvalue cottage line.
	// iValue += 4 * improvement.getCulture() * iCultureRange * iCultureRange;

	// We should only be caring about military stats on military improvements; others are incidential
	if (improvement.isMilitaryStructure())
	{
		int iMilitaryValue = 3 * improvement.getAirBombDefense();

		if (improvement.isZOCSource()) iMilitaryValue += 200;

		iMilitaryValue += 2 * (5 * improvement.getVisibilityChange() + 2 * improvement.getSeeFrom());

		const int iDefense = improvement.getDefenseModifier();
		if (iDefense < 0)
		{
			iMilitaryValue -= iDefense * iDefense;
		}
		else if (iDefense > 0)
		{
			iMilitaryValue += iDefense * iDefense;
		}

		iMilitaryValue *= (1 + iCultureRange);
		iMilitaryValue *= 5;
		iMilitaryValue /= 3;
		iValue += iMilitaryValue;
	}

	const ImprovementTypes eFinalUpgrade = finalImprovementUpgrade(eImprovement);
	FAssert(eFinalUpgrade != NO_IMPROVEMENT);

	int yields[NUM_YIELD_TYPES];
	iValue *= 2;
	for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
	{
		yields[iJ] =
			(pPlot->calculateImprovementYieldChange(eFinalUpgrade, (YieldTypes)iJ, getOwner(), false, true) * 1
				+ pPlot->calculateImprovementYieldChange(eImprovement, (YieldTypes)iJ, getOwner(), false, true) * 5
				) / 2;
	}
	iValue += yields[YIELD_FOOD] * iFoodPriority;
	iValue += yields[YIELD_PRODUCTION] * iProductionPriority;
	iValue += yields[YIELD_COMMERCE] * iCommercePriority;
	iValue /= 10;

	if (yields[YIELD_FOOD] * 3 / 5 > (yields[YIELD_PRODUCTION] + yields[YIELD_COMMERCE]) / 2)
	{
		//this is a food yielding tile
		if (iFoodPriority > 100)
		{
			iValue *= 100 + iFoodPriority;
			iValue /= 200;
		}
		if (iFoodChange > 0)
		{
			iValue += 5 * yields[YIELD_FOOD] * iFoodChange * iFoodPriority / 100;
		}
	}

	if (iFoodPriority < 100 && iProductionPriority > 100)
	{
		iValue *= 200 + (iProductionPriority - 100) * yields[YIELD_PRODUCTION];
		iValue /= 200;
	}

	iValue += 100 * getImprovementFreeSpecialists(eImprovement);

	if (improvement.getHappiness() != 0)
	{
		int iHappyLevel = happyLevel() - unhappyLevel(0);
		if (iHappyLevel - iHappyLevel / 5 < 0)
		{
			iValue += 100 * improvement.getHappiness();
		}
		else iValue += 20 * improvement.getHappiness();
	}

	iValue += improvement.getHealthPercent();

	if (!isHuman())
	{
		iValue *= std::max(0, GC.getLeaderHeadInfo(getPersonalityType()).getImprovementWeightModifier(eFinalUpgrade) + 200);
		iValue /= 200;
	}
	return iValue;
}


BuildTypes CvCityAI::AI_getBestBuild(int iIndex) const
{
	FASSERT_BOUNDS(0, NUM_CITY_PLOTS, iIndex);
	FASSERT_BOUNDS(-1, GC.getNumBuildInfos(), m_aeBestBuild[iIndex]);
	return m_aeBestBuild[iIndex];
}


int CvCityAI::AI_countBestBuilds(const CvArea* pArea) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = SKIP_CITY_HOME_PLOT; iI < getNumCityPlots(); iI++)
	{
		const CvPlot* pLoopPlot = plotCity(getX(), getY(), iI);

		if (pLoopPlot != NULL && pLoopPlot->area() == pArea && AI_getBestBuild(iI) != NO_BUILD)
		{
			iCount++;
		}
	}

	return iCount;
}
BuildTypes CvCityAI::GetShortestBuildTimeOnPlot(const CvPlot* plot) const
{
	return scoring::min_score(plot->units()
		| filtered(CvUnit::fn::getBuildType() != NO_BUILD)
		| transformed(CvUnit::fn::getBuildType()),
		bind(score_build_type, _1, plot)
	).get_value_or(NO_BUILD);
}

void CvCityAI::AI_markBestBuildValuesStale()
{
	m_bestBuildValuesStale = true;
}

void CvCityAI::AI_updateBestBuild()
{
	PROFILE_FUNC();
	if (!m_bestBuildValuesStale)
	{
		OutputDebugString(CvString::format("City %S skips re-evaluation of build values\n", getName().GetCString()).c_str());
		return;
	}
	m_bestBuildValuesStale = false;

	OutputRatios ratios = OutputRatios(this->getBaseYieldRate(YIELD_FOOD), this->getBaseYieldRate(YIELD_PRODUCTION), this->getBaseYieldRate(YIELD_COMMERCE));

	// these are the current default weights to make AI actually care about food at their plots function is built in such a way
	// that you can call it several times to adjust the ratio
	// (i.e a city has food preference, call it 2nd time around with 2,1,1, and the value of food will double)

	std::vector<plotInfo> optimalYieldList = std::vector<plotInfo>(NUM_CITY_PLOTS);

	for (int iPlotCounter = 1; iPlotCounter < getNumCityPlots(); iPlotCounter++) // start at 1, 0 is the plot of the city
	{
		optimalYieldList[iPlotCounter].index = iPlotCounter;
		CvPlot* loopedPlot = getCityIndexPlot(iPlotCounter);

		if (NULL == loopedPlot || !(loopedPlot->getOwner() == getOwner()) || !(loopedPlot->getWorkingCity() == this)) {
			m_aeBestBuild[iPlotCounter] = NO_BUILD;
			m_aiBestBuildValue[iPlotCounter] = 0;
			continue;
		}

		optimalYieldList[iPlotCounter].value = m_aiBestBuildValue[iPlotCounter];
		optimalYieldList[iPlotCounter].currentBuild = m_aeBestBuild[iPlotCounter];

		AI_findBestImprovementForPlot(loopedPlot, &optimalYieldList[iPlotCounter], ratios);

		m_aeBestBuild[iPlotCounter] = optimalYieldList[iPlotCounter].newBuild;
		m_aiBestBuildValue[iPlotCounter] = optimalYieldList[iPlotCounter].newValue;
	}
}

// Better drafting strategy by Blake - thank you!
void CvCityAI::AI_doDraft()
{
	PROFILE_FUNC();
	FAssert(!isHuman());

	if (!canConscript())
	{
		return;
	}

	if (GC.getGame().AI_combatValue(getConscriptUnit()) > 33)
	{
		const bool bLandWar = area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE || area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE || area()->getAreaAIType(getTeam()) == AREAAI_MASSING;
		const bool bDanger = !AI_isDefended() && AI_isDanger();

		if (!bDanger && !bLandWar)
		{
			return;
		}
		// Don't go broke from drafting
		if (!bDanger && GET_PLAYER(getOwner()).AI_isFinancialTrouble())
		{
			return;
		}
		const int iConscriptPop = getConscriptPopulation();
		// Don't shrink cities too much
		if (!bDanger && 3 * (getPopulation() - iConscriptPop) < 2 * getHighestPopulation())
		{
			return;
		}
		// Large cities want a little spare happiness
		const int iHappyDiff = GC.getCONSCRIPT_POP_ANGER() + (getPopulation() - iConscriptPop) / 6;

		if (bLandWar && 0 == angryPopulation(iHappyDiff))
		{
			bool bWait = true;

			if (GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_TURTLE))
			{
				// Full out defensive
				if (bDanger || AI_countWorkedPoorTiles() + std::max(0, visiblePopulation() - AI_countGoodSpecialists(true)) >= 1)
				{
					bWait = false;
				}
			}

			if (bWait && bDanger)
			{
				// If city might be captured, don't hold back
				const int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(), 0, true, false, true);

				if (iOurDefense == 0 || 3 * GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false) > 2 * iOurDefense)
				{
					bWait = false;
				}
			}

			if (!bWait)
			{
				LOG_CITY_BLOCK(2, {
					logBBAI("      City %S (size %d, highest %d) chooses to conscript with danger: %d, land war: %d, poor tiles: %d, bad specialists: %d",
						getName().GetCString(), getPopulation(), getHighestPopulation(), bDanger, bLandWar,
						AI_countWorkedPoorTiles(), std::max(0, visiblePopulation() - AI_countGoodSpecialists(true))
					);
				});
				conscript();
			}
		}
	}
}

// Better pop-rushing strategy by Blake - thank you!
void CvCityAI::AI_doHurry(bool bForce)
{
	PROFILE_FUNC();

	FAssert(!isHuman() /*|| isProductionAutomated()*/); // Toffer - Disabled governors ability to rush.

	if (isNPC() || getProductionTurnsLeft() < 2 || getProductionProgress() == 0 && !bForce)
	{
		return;
	}
	CvArea* pWaterArea = waterArea();
	const CvPlayerAI& player = GET_PLAYER(getOwner());

	const UnitTypes eProductionUnit = getProductionUnit();
	const UnitAITypes eProductionUnitAI = getProductionUnitAI();
	const BuildingTypes eProductionBuilding = getProductionBuilding();

	const bool bDanger = AI_isDanger();

	for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
	{
		if (!canHurry((HurryTypes)iI))
		{
			continue;
		}
		const int iHurryPopulation = hurryPopulation((HurryTypes)iI);
		const int iHurryAngerLength = hurryAngerLength((HurryTypes)iI);

		// Gold hurry
		if (iHurryPopulation < 1)
		{
			if (bForce)
			{
				hurry((HurryTypes)iI);
				return;
			}
			// Rush defenses when in big trouble
			if (eProductionUnit != NO_UNIT
				&& area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE && GET_TEAM(getTeam()).AI_getEnemyPowerPercent(true) > 150
				&& !player.AI_isFinancialTrouble() && iHurryAngerLength == 0)
			{
				int64_t iHurryGold = getHurryGold((HurryTypes)iI);
				bool bWait = true;

				if (player.AI_isDoStrategy(AI_STRATEGY_TURTLE))
				{
					if ((bDanger ? 5 : 8) * iHurryGold < player.getGold())
					{
						bWait = false;
					}
				}
				else if ((bDanger ? 8 : 12) * iHurryGold < player.getGold())
				{
					bWait = false;
				}

				if (!bWait)
				{
					LOG_BBAI_CITY(2, ("      City %S hurry gold at %I64d to rush defenses for recent attack", getName().GetCString(), iHurryGold));
					hurry((HurryTypes)iI);
					return;
				}
			}
			if (eProductionBuilding != NO_BUILDING)
			{
				/* Toffer - Disabled governors ability to rush.
				// City governor hard limitation.
				if (player.isHumanPlayer() && player.getHurriedCount() > 32)
				{
					continue;
				}
				*/
				// If it doesn't reduce time to completion, it isn't worth the inflation.
				// (ToDo) Inflation may in the future be based on gold amount spent
				// rather than e.g. hurry count, making this check obsolete.
				const int iTurns = getProductionTurnsLeft(eProductionBuilding, 1);
				if (iTurns < 2)
				{
					continue;
				}
				const int64_t iGold = player.getGold();
				const int64_t iCost = getHurryGold((HurryTypes)iI);

				// Toffer - Sanity control - Limit overspending and inflation.
				// If risky investment, financial trouble or at 100% or more inflation, do not hurry production.
				if (iCost > iGold / 10
					|| player.AI_isFinancialTrouble()
					|| player.getInflationCost() > player.calculatePreInflatedCosts())
				{
					continue;
				}
				const int iValue = AI_buildingValueThreshold(eProductionBuilding, BUILDINGFOCUS_GOLD | BUILDINGFOCUS_MAINTENANCE | BUILDINGFOCUS_PRODUCTION) / 3;

				if (iValue > 0 && iCost / iValue < iTurns)
				{
					LOG_CITY_BLOCK(3, {
						// %I64d format bug requires more than one log entry as all % after it will get the wrong value.
						logBBAI("\tCity %S hurry production: cost %I64d, ...", getName().GetCString(), iCost);
						logBBAI("\t... Building-Value %d, turns left %d, treasury %I64d", iValue, iTurns, iGold);
					});
					hurry((HurryTypes)iI);
					return;
				}
			}
			continue;
		}
		// Whipping
		int iMinTurns = MAX_INT;
		bool bEssential = false;

		if (eProductionBuilding != NO_BUILDING)
		{
			if (bDanger)
			{
				if (GC.getBuildingInfo(eProductionBuilding).getDefenseModifier() > 0)
				{
					bEssential = true;
				}
			}
			else
			{
				if (isWorldWonder(eProductionBuilding))
				{
					iMinTurns = 2;
					bEssential = true;
				}
				else
				{
					const int iAdditionalProduction = getAdditionalYieldByBuilding(YIELD_PRODUCTION, eProductionBuilding, true);

					if (iAdditionalProduction > 0 && 3 * iAdditionalProduction > 2 * getBaseYieldRate(YIELD_PRODUCTION))
					{
						iMinTurns = 3;
					}
				}
			}

		}
		else if (eProductionUnit != NO_UNIT)
		{
			if (bDanger)
			{
				if (
					GC.getUnitInfo(eProductionUnit).getDomainType() == DOMAIN_LAND
				&&	GC.getUnitInfo(eProductionUnit).getCombat() > 0)
				{
					bEssential = true;
				}
			}
			else if (eProductionUnitAI != NO_UNITAI)
			{
				if (eProductionUnitAI == UNITAI_CITY_DEFENSE
				&& plot()->plotCheck(PUF_isUnitAIType, UNITAI_SETTLE, -1, NULL, getOwner())
				&& !AI_isDefended(-2 * player.strengthOfBestUnitAI(DOMAIN_LAND, UNITAI_CITY_DEFENSE))) // XXX check for other team's units?
				{
					iMinTurns = 3;
				}
				else if (eProductionUnitAI == UNITAI_SETTLE
				&& !player.AI_isFinancialTrouble()
				&& area()->getNumAIUnits(getOwner(), UNITAI_SETTLE) == 0
				&& area()->getBestFoundValue(getOwner()) > 0)
				{
					iMinTurns = 1;
					bEssential = true;
				}
			}
		}

		if (!bDanger || !bEssential)
		{
			if (iMinTurns == MAX_INT)
			{
				return;
			}
			// adjust for game speed
			iMinTurns *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent();
			iMinTurns /= 100;
		}

		if (bForce || AI_countGoodTiles(healthRate(0) == 0, false, bEssential ? 100 : 50) <= getPopulation() - iHurryPopulation + player.getCurrentEra())
		{
			if (bForce || bDanger && bEssential || getProductionTurnsLeft() > iMinTurns)
			{
				LOG_BBAI_CITY(2, (
						"      City %S hurry pop at %d with bad tiles and no reason to wait with pop %d",
						getName().GetCString(), iHurryPopulation, getPopulation()
					));
				hurry((HurryTypes)iI);
				return;
			}
		}
	}
}


// Improved use of emphasize by Blake, to go with his whipping strategy - thank you!
void CvCityAI::AI_doEmphasize()
{
	//Note from Blake:
	//Emphasis proved to be too clumsy to manage AI economies,
	//as such it's been nearly completely phased out by
	//the AI_specialYieldMultiplier system which allows arbitary
	//value-boosts and works very well.
	//Ideally the AI should never use emphasis.

	PROFILE_FUNC();
	FAssert(!isHuman());

	const bool bFirstTech =
		(
			GET_PLAYER(getOwner()).getCurrentResearch() != NO_TECH
			?
			GET_PLAYER(getOwner()).AI_isFirstTech(GET_PLAYER(getOwner()).getCurrentResearch())
			:
			false
			);
	const bool bCultureVictory = GET_PLAYER(getOwner()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2);
	const int iPopulationRank = findPopulationRank();

	for (int iI = 0; iI < GC.getNumEmphasizeInfos(); iI++)
	{
		bool bEmphasize = false;

		if (AI_specialYieldMultiplier(YIELD_PRODUCTION) < 50)
		{
			if (bFirstTech && GC.getEmphasizeInfo((EmphasizeTypes)iI).getYieldChange(YIELD_COMMERCE) > 0)
			{
				bEmphasize = true;
			}

			if (!bEmphasize && bFirstTech && !bCultureVictory
				&& GC.getEmphasizeInfo((EmphasizeTypes)iI).getCommerceChange(COMMERCE_RESEARCH) > 0
				&& iPopulationRank < 1 + GET_PLAYER(getOwner()).getNumCities() / 4)
			{
				bEmphasize = true;
			}

			if (!bEmphasize && GC.getEmphasizeInfo((EmphasizeTypes)iI).isGreatPeople())
			{
				int iHighFoodTotal = 0;
				int iHighFoodPlotCount = 0;
				int iHighHammerPlotCount = 0;
				int iHighHammerTotal = 0;
				int iGoodFoodSink = 0;
				const int iFoodPerPop = GC.getFOOD_CONSUMPTION_PER_POPULATION();

				foreach_(const CvPlot * pLoopPlot, plots())
				{
					if (pLoopPlot->getWorkingCity() == this)
					{
						const int iFood = pLoopPlot->getYield(YIELD_FOOD);
						if (iFood > iFoodPerPop)
						{
							iHighFoodTotal += iFood;
							iHighFoodPlotCount++;
						}
						const int iHammers = pLoopPlot->getYield(YIELD_PRODUCTION);
						if (iHammers >= 3 && iHammers + iFood >= 4)
						{
							iHighHammerPlotCount++;
							iHighHammerTotal += iHammers;
						}
						if (pLoopPlot->getYield(YIELD_COMMERCE) * 2 + iHammers * 3 > 9)
						{
							iGoodFoodSink += std::max(0, iFoodPerPop - iFood);
						}
					}
				}

				if (iHighFoodTotal + iHighFoodPlotCount - iGoodFoodSink >= foodConsumption(true)
					&& iHighHammerPlotCount < 2 && iHighHammerTotal < getPopulation()
					&& AI_countGoodTiles(true, false, 100, true) < getPopulation())
				{
					bEmphasize = true;
				}
			}
		}
		AI_setEmphasize((EmphasizeTypes)iI, bEmphasize);
	}
}


//Fuyu bIgnoreNotUnitAIs
bool CvCityAI::AI_chooseUnit(const char* reason, UnitAITypes eUnitAI, int iOdds, int iUnitStrength, int iPriorityOverride, const CvUnitSelectionCriteria* criteria)
{//Adding a unit type direct selection here...
#ifdef USE_UNIT_TENDERING
	//	Have we already contracted for a unit?
	if (m_iRequestedUnit > MAX_REQUESTEDUNIT_PER_CITY)
	{

		LOG_CITY_BLOCK(4, {
			CvString unitAIType;
			CvString reasonStr = reason;
			if (eUnitAI != NO_UNITAI)
			{
				CvInfoBase& AIType = GC.getUnitAIInfo(eUnitAI);
				unitAIType = AIType.getType();
			}
			else
			{
				unitAIType = "NO_UNITAI";
			}
			logBBAI("      City %S already have unit in tender queue, m_bRequestedUnit is true, ChooseUnit for reason %S of Type %S aborted", getName().GetCString(), reasonStr.GetCString(), unitAIType.GetCString());
		});
		return false;
	}
#endif

	if (iOdds < 0 || GC.getGame().getSorenRandNum(100, "City AI choose unit") < iOdds)
	{
		if (iPriorityOverride == -1)
		{
			iPriorityOverride = m_iTempBuildPriority;
		}

		CvString unitAIType;
		if (eUnitAI != NO_UNITAI)
		{
			CvInfoBase& AIType = GC.getUnitAIInfo(eUnitAI);
			unitAIType = AIType.getType();
		}
		else
		{
			unitAIType = "NO_UNITAI";
		}

#ifdef USE_UNIT_TENDERING
		if (!isNPC())
		{
			//	Check someone can build a suitable unit before putting out a tender for one.
			if (eUnitAI == NO_UNITAI || GET_PLAYER(getOwner()).bestBuildableUnitForAIType(NO_DOMAIN, eUnitAI, criteria) != NO_UNIT)
			{
				LOG_BBAI_CITY(2, ("      City %S pop %d puts out tenders for %d unit strength of AIType: %s at priority %d (reason: %s)", getName().GetCString(), getPopulation(), iUnitStrength, unitAIType.c_str(), iPriorityOverride, reason));

				//	Put out a tender for this unit
				GET_PLAYER(getOwner()).getContractBroker().advertiseWork(iPriorityOverride,
					NO_UNITCAPABILITIES,
					getX(),
					getY(),
					NULL,
					eUnitAI,
					iUnitStrength,
					criteria);

				m_iRequestedUnit ++;

				return (m_iRequestedBuilding > MAX_REQUESTEDBUILDING_PER_CITY);
			}
			//	Looks like we probably can't build it, but fallback check if we can ourselves, and if so just
			//	queue it here (handles edge cases where building requirements mean it can be built locally but
			//	not in capital or best coastal).  Do this by falling through to local handling
		}
#endif

		return AI_chooseUnitImmediate(reason, eUnitAI, criteria, NO_UNIT);
	}

	return false;
}

bool CvCityAI::AI_chooseUnitImmediate(const char* reason, UnitAITypes eUnitAI, const CvUnitSelectionCriteria* criteria, UnitTypes eUnitType)
{
	UnitTypes eBestUnit = NO_UNIT;

	if (eUnitType == NO_UNIT)
	{
		if (eUnitAI != NO_UNITAI)
		{
			int iDummyValue = 0;
			eBestUnit = AI_bestUnitAI(eUnitAI, iDummyValue, false, false, criteria);
		}
		else
		{
			int iDummyValue = 0;
			eBestUnit = AI_bestUnit(iDummyValue, -1, NULL, false, &eUnitAI, false, false, criteria);
		}
	}
	else
	{
		eBestUnit = eUnitType;
	}

	if (eBestUnit != NO_UNIT)
	{
		LOG_CITY_BLOCK(2, {
			const CvString unitAIType(eUnitAI != NO_UNITAI ? GC.getUnitAIInfo(eUnitAI).getType() : "NO_UNITAI");
			logBBAI("      City %S pop %d builds unit of AIType: %s at priority %d (reason: %s)", getName().GetCString(), getPopulation(), unitAIType.c_str(), m_iTempBuildPriority, reason);
		});

		pushOrder(ORDER_TRAIN, eBestUnit, eUnitAI, false, false, false);
		return true;
	}

	return false;
}

bool CvCityAI::AI_chooseUnit(UnitTypes eUnit, UnitAITypes eUnitAI)
{
	if (eUnit != NO_UNIT)
	{
		pushOrder(ORDER_TRAIN, eUnit, eUnitAI, false, false, true);
		return true;
	}
	return false;
}

bool CvCityAI::AI_chooseDefender(const char* reason)
{
	if (plot()->plotCheck(PUF_isUnitAIType, UNITAI_CITY_SPECIAL, -1, NULL, getOwner()) == NULL)
	{
		if (AI_chooseUnit(reason, UNITAI_CITY_SPECIAL))
		{
			return true;
		}
	}

	if (plot()->plotCheck(PUF_isUnitAIType, UNITAI_CITY_COUNTER, -1, NULL, getOwner()) == NULL)
	{
		if (AI_chooseUnit(reason, UNITAI_CITY_COUNTER))
		{
			return true;
		}
	}

	if (AI_chooseUnit(reason, UNITAI_CITY_DEFENSE))
	{
		return true;
	}

	return false;
}

bool CvCityAI::AI_chooseLeastRepresentedUnit(const char* reason, UnitTypeWeightArray& allowedTypes, int iOdds)
{
	PROFILE_EXTRA_FUNC();
	if (iOdds < 0 || iOdds > GC.getGame().getSorenRandNum(100, "AI choose least represented unit overall odds"))
	{
		std::multimap<int, UnitAITypes, std::greater<int> > bestTypes;
		int iTotalWeight = 0;

		for (UnitTypeWeightArray::iterator it = allowedTypes.begin(); it != allowedTypes.end(); ++it)
		{
			int iValue = it->second;
			iValue *= 750 + GC.getGame().getSorenRandNum(250, "AI choose least represented unit");
			iValue /= std::max(1, GET_PLAYER(getOwner()).AI_totalAreaUnitAIs(area(), it->first));
			bestTypes.insert(std::make_pair(iValue, it->first));

			iTotalWeight += iValue / 100;
		}

		int iChoiceWeight = GC.getGame().getSorenRandNum(iTotalWeight, "AI choose least represented unit");

		for (std::multimap<int, UnitAITypes, std::greater<int> >::iterator best_it = bestTypes.begin(); best_it != bestTypes.end(); ++best_it)
		{
			iChoiceWeight -= best_it->first / 100;

			CvString reasonString;

			reasonString.Format("least represented (%s)", reason);

			if (iChoiceWeight <= 0 && AI_chooseUnit(reasonString.c_str(), best_it->second))
			{
				return true;
			}
		}
	}

	return false;
}

bool CvCityAI::AI_bestSpreadUnit(bool bMissionary, bool bExecutive, int iBaseChance, UnitTypes* eBestSpreadUnit, int* iBestSpreadUnitValue) const
{
	PROFILE_EXTRA_FUNC();
	const CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	const CvTeamAI& kTeam = GET_TEAM(getTeam());
	CvGame& kGame = GC.getGame();

	FAssert(eBestSpreadUnit != NULL && iBestSpreadUnitValue != NULL);

	int iBestValue = 0;

	if (bMissionary)
	{
		for (int iReligion = 0; iReligion < GC.getNumReligionInfos(); iReligion++)
		{
			const ReligionTypes eReligion = (ReligionTypes)iReligion;
			const int iNeededMissionaries = kPlayer.AI_neededMissionaries(area(), eReligion);

			if (isHasReligion(eReligion) && iNeededMissionaries > 0)
			{
				const int iHasCount = kPlayer.getHasReligionCount(eReligion);
				FAssert(iHasCount > 0);
				int iRoll = (iHasCount > 4) ? iBaseChance : (((100 - iBaseChance) / iHasCount) + iBaseChance);
				if (kPlayer.AI_isDoStrategy(AI_STRATEGY_MISSIONARY))
				{
					iRoll *= (kPlayer.getStateReligion() == eReligion) ? 170 : 65;
					iRoll /= 100;
				}
				if (kPlayer.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2))
				{
					iRoll += 25;
				}
				else if (!kTeam.hasHolyCity(eReligion) && !(kPlayer.getStateReligion() == eReligion))
				{
					iRoll /= 2;
					if (kPlayer.isNoNonStateReligionSpread())
					{
						iRoll /= 2;
					}
				}

				if (kPlayer.isPushReligiousVictory() && (kPlayer.getStateReligion() == eReligion))
				{
					iRoll += 25;
				}
				else if (kPlayer.isConsiderReligiousVictory() && (kPlayer.getStateReligion() == eReligion))
				{
					iRoll += 7;
				}
				else if ((kPlayer.isPushReligiousVictory() || kPlayer.isConsiderReligiousVictory())
					&& (kPlayer.getStateReligion() != eReligion))
				{
					iRoll = 0;
				}

				if (iRoll > kGame.getSorenRandNum(100, "AI choose missionary"))
				{
					const int iReligionValue = kPlayer.AI_missionaryValue(area(), eReligion);
					if (iReligionValue > 0)
					{
						for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
						{
							if (GC.getUnitInfo((UnitTypes)iI).getReligionSpreads(eReligion) > 0
								&& GC.getUnitInfo((UnitTypes)iI).getProductionCost() > 0)
							{
								const int iValue = iReligionValue / GC.getUnitInfo((UnitTypes)iI).getProductionCost();

								if (iValue > iBestValue && canTrain((UnitTypes)iI))
								{
									iBestValue = iValue;
									*eBestSpreadUnit = (UnitTypes)iI;
									*iBestSpreadUnitValue = iReligionValue;
								}
							}
						}
					}
				}
			}
		}
	}

	if (bExecutive)
	{
		for (int iCorporation = 0; iCorporation < GC.getNumCorporationInfos(); iCorporation++)
		{
			const CorporationTypes eCorporation = (CorporationTypes)iCorporation;
			if (isActiveCorporation(eCorporation) && kPlayer.getHasCorporationCount(eCorporation) > 0)
			{
				const int iHasCount = kPlayer.getHasCorporationCount(eCorporation);
				FAssert(iHasCount > 0);
				int iRoll = (iHasCount > 4) ? iBaseChance : (((100 - iBaseChance) / iHasCount) + iBaseChance);
				if (!kTeam.hasHeadquarters(eCorporation))
				{
					iRoll /= 8;
				}

				if (iRoll > kGame.getSorenRandNum(100, "AI choose executive"))
				{
					int iCorporationValue = kPlayer.AI_executiveValue(area(), eCorporation);
					if (iCorporationValue > 0)
					{
						for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
						{
							const CvUnitInfo& kUnitInfo = GC.getUnitInfo((UnitTypes)iI);
							if (kUnitInfo.getCorporationSpreads(eCorporation) > 0 && canTrain((UnitTypes)iI))
							{
								int iValue = iCorporationValue / kUnitInfo.getProductionCost();
								int iTotalCount = 0;
								int iPlotCount = 0;
								foreach_(const CvUnit * pLoopUnit, kPlayer.units())
								{
									if ((pLoopUnit->AI_getUnitAIType() == UNITAI_MISSIONARY) && (pLoopUnit->getUnitInfo().getCorporationSpreads(eCorporation) > 0))
									{
										iTotalCount++;
										if (pLoopUnit->plot() == plot())
										{
											iPlotCount++;
										}
									}
								}
								iCorporationValue /= std::max(1, (iTotalCount / 4) + iPlotCount);

								int iCost = std::max(0, GC.getCorporationInfo(eCorporation).getSpreadCost());

								if (kPlayer.getGold() >= iCost)
								{
									iCost *= GC.getCORPORATION_FOREIGN_SPREAD_COST_PERCENT();
									iCost /= 100;
									if (kPlayer.getGold() < iCost && iTotalCount > 1)
									{
										iCorporationValue /= 2;
									}
								}
								else if (iTotalCount > 1)
								{
									iCorporationValue /= 5;
								}
								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									*eBestSpreadUnit = (UnitTypes)iI;
									*iBestSpreadUnitValue = iCorporationValue;
								}
							}
						}
					}
				}
			}
		}
	}
	return (*eBestSpreadUnit != NULL);
}

bool CvCityAI::AI_chooseBuilding(int iFocusFlags, int iMaxTurns, int iMinThreshold, int iOdds, bool bMaximizeFlaggedValue, PropertyTypes eProperty)
{
	PROFILE_EXTRA_FUNC();
#ifdef USE_UNIT_TENDERING
	//	Have we already selected a building?
	if (m_iRequestedBuilding > MAX_REQUESTEDBUILDING_PER_CITY)
	{
		return false;
	}
	m_iBuildPriority = m_iTempBuildPriority;
#endif

	if (iMaxTurns != MAX_INT)
	{
		iMaxTurns = std::max(1, iMaxTurns * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent() / 100);
	}
	const std::vector<ScoredBuilding> bestBuildings = AI_bestBuildingsThreshold(iFocusFlags, iMaxTurns, iMinThreshold, false, NO_ADVISOR, bMaximizeFlaggedValue, eProperty);
	const int stdDesiredQueueTurns = GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent() / 20;
	const int desiredQueueTurns = std::max(1, std::min(stdDesiredQueueTurns, iMaxTurns));
	bool enqueuedBuilding = false;
	int nbBuildings = 0;
	for (size_t i = 0; i < bestBuildings.size() && getTotalProductionQueueTurnsLeft() <= desiredQueueTurns; ++i)
	{
		const BuildingTypes eBestBuilding = bestBuildings[i].building;
		if (iOdds < 0
		|| getProgressOnBuilding(eBestBuilding) > 0
		|| GC.getGame().getSorenRandNum(100, "City AI choose building") < iOdds)
		{
			pushOrder(ORDER_CONSTRUCT, eBestBuilding, -1, false, false, true); //not insert, append to queue
			nbBuildings += 1;
			enqueuedBuilding = true;
		}
		// If we failed a roll then abort now, we don't want to choose worse buildings
		else break;

		CvPlayer& player = GET_PLAYER(getOwner());
		if (nbBuildings > (NB_MAX_BUILDINGS + player.getCurrentEra() / 2))
		{
			LOG_BBAI_CITY(3, ("Player %d, city: %S, too many building already ordered (nb buildings = %d, nb desired turns = %d, Std = %d)", getOwner(), getName().GetCString(), nbBuildings, desiredQueueTurns, stdDesiredQueueTurns));
		}
		break;

	}
#ifdef USE_UNIT_TENDERING
	if (enqueuedBuilding)
	{
		m_iRequestedBuilding ++;

		return (isNPC() || m_iRequestedUnit > MAX_REQUESTEDUNIT_PER_CITY);
	}
	return false;
#else
	return enqueuedBuilding;
#endif
}


bool CvCityAI::AI_chooseProject()
{
#ifdef USE_UNIT_TENDERING
	if (m_iRequestedBuilding > MAX_REQUESTEDBUILDING_PER_CITY)
	{
		return false;
	}

	m_iBuildPriority = m_iTempBuildPriority;
#endif

	ProjectTypes eBestProject = AI_bestProject();

	if (eBestProject != NO_PROJECT)
	{
		pushOrder(ORDER_CREATE, eBestProject, -1, false, false, true);
		
		return true;
	}

	return false;
}


bool CvCityAI::AI_chooseProcess(CommerceTypes eCommerceType, int64_t* commerceWeights,bool bforce)
{
#ifdef USE_UNIT_TENDERING
	if (eCommerceType != NO_COMMERCE && (m_iRequestedBuilding > MAX_REQUESTEDBUILDING_PER_CITY && m_iRequestedUnit > MAX_REQUESTEDUNIT_PER_CITY))
	{
		return false;
	}
	m_iBuildPriority = m_iTempBuildPriority;
#endif

	const ProcessTypes eBestProcess = AI_bestProcess(eCommerceType, commerceWeights);

	if (eBestProcess != NO_PROCESS)
	{
		pushOrder(ORDER_MAINTAIN, eBestProcess, -1, false, false, !bforce);

		return true;
	}

	return false;
}


// Returns true if a worker was added to a plot...
bool CvCityAI::AI_addBestCitizen(bool bWorkers, bool bSpecialists, int* piBestPlot, SpecialistTypes* peBestSpecialist)
{
	PROFILE_FUNC();

	const bool bAvoidGrowth = AI_avoidGrowth();
	const bool bIgnoreGrowth = AI_ignoreGrowth();

	int iBestSpecialistValue = 0;
	SpecialistTypes eBestSpecialist = NO_SPECIALIST;

	// --- Specialist selection ---
	if (bSpecialists)
	{
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); ++iI)
		{
			const SpecialistTypes eSpec = static_cast<SpecialistTypes>(iI);
			const int iForced = getForceSpecialistCount(eSpec);
			if (iForced > 0 && isSpecialistValid(eSpec, 1))
			{
				// Forced specialist: assign immediately
				changeSpecialistCount(eSpec, 1);
				if (piBestPlot && peBestSpecialist)
				{
					*peBestSpecialist = eSpec;
					*piBestPlot = -1;
				}
				return true;
			}
			if (isSpecialistValid(eSpec, 1))
			{
				const int iValue = AI_specialistValue(eSpec, bAvoidGrowth, false);
				if (iValue > iBestSpecialistValue)
				{
					iBestSpecialistValue = iValue;
					eBestSpecialist = eSpec;
				}
			}
		}
	}

	// --- Worker plot selection ---
	int iBestPlotValue = 0;
	int iBestPlot = -1;
	if (bWorkers)
	{
		for (int iI = SKIP_CITY_HOME_PLOT; iI < NUM_CITY_PLOTS; ++iI)
		{
			if (!isWorkingPlot(iI))
			{
				const CvPlot* pLoopPlot = getCityIndexPlot(iI);
				if (pLoopPlot && canWork(pLoopPlot))
				{
					const int iValue = AI_plotValue(pLoopPlot, bAvoidGrowth, /*bRemove*/ false, /*bIgnoreFood*/ false, bIgnoreGrowth);
					if (iValue > iBestPlotValue)
					{
						iBestPlotValue = iValue;
						iBestPlot = iI;
					}
				}
			}
		}
	}

	// --- Decision: assign best option ---
	if (iBestPlot != -1 && iBestPlotValue > iBestSpecialistValue)
	{
		eBestSpecialist = NO_SPECIALIST;
	}

	if (eBestSpecialist != NO_SPECIALIST)
	{
		changeSpecialistCount(eBestSpecialist, 1);
		if (piBestPlot && peBestSpecialist)
		{
			*peBestSpecialist = eBestSpecialist;
			*piBestPlot = -1;
		}
		return true;
	}
	if (iBestPlot != -1)
	{
		setWorkingPlot(iBestPlot, true);
		if (piBestPlot && peBestSpecialist)
		{
			*peBestSpecialist = NO_SPECIALIST;
			*piBestPlot = iBestPlot;
		}
		return true;
	}
	return false;
}


// Returns true if a worker was removed from a plot...
bool CvCityAI::AI_removeWorstCitizen(SpecialistTypes eIgnoreSpecialist)
{
	PROFILE_EXTRA_FUNC();
	// if we are using more specialists than the free ones we get
	if (extraFreeSpecialists() < 0)
	{
		// does generic 'citizen' specialist exist?
		if (GC.getDEFAULT_SPECIALIST() != NO_SPECIALIST)
		{
			// is ignore something other than generic citizen?
			if (eIgnoreSpecialist != GC.getDEFAULT_SPECIALIST())
			{
				// do we have at least one more generic citizen than we are forcing?
				if (getSpecialistCount((SpecialistTypes)GC.getDEFAULT_SPECIALIST()) > getForceSpecialistCount((SpecialistTypes)GC.getDEFAULT_SPECIALIST()))
				{
					// remove the extra generic citzen
					changeSpecialistCount((SpecialistTypes)GC.getDEFAULT_SPECIALIST(), -1);
					return true;
				}
			}
		}
	}

	bool bAvoidGrowth = AI_avoidGrowth();
	bool bIgnoreGrowth = AI_ignoreGrowth();

	int iWorstValue = MAX_INT;
	SpecialistTypes eWorstSpecialist = NO_SPECIALIST;
	int iWorstPlot = -1;

	// if we are using more specialists than the free ones we get
	if (extraFreeSpecialists() < 0)
	{
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			if (eIgnoreSpecialist != iI)
			{
				if (getSpecialistCount((SpecialistTypes)iI) > getForceSpecialistCount((SpecialistTypes)iI))
				{
					int iValue = AI_specialistValue(((SpecialistTypes)iI), bAvoidGrowth, /*bRemove*/ true);

					if (iValue < iWorstValue)
					{
						iWorstValue = iValue;
						eWorstSpecialist = ((SpecialistTypes)iI);
						iWorstPlot = -1;
					}
				}
			}
		}
	}

	// check all the plots we working
	for (int iI = SKIP_CITY_HOME_PLOT; iI < NUM_CITY_PLOTS; iI++)
	{
		if (isWorkingPlot(iI))
		{
			const CvPlot* pLoopPlot = getCityIndexPlot(iI);

			if (pLoopPlot != NULL)
			{
				const int iValue = AI_plotValue(pLoopPlot, bAvoidGrowth, /*bRemove*/ true, /*bIgnoreFood*/ false, bIgnoreGrowth);

				if (iValue < iWorstValue)
				{
					iWorstValue = iValue;
					eWorstSpecialist = NO_SPECIALIST;
					iWorstPlot = iI;
				}
			}
		}
	}

	if (eWorstSpecialist != NO_SPECIALIST)
	{
		changeSpecialistCount(eWorstSpecialist, -1);
		return true;
	}
	else if (iWorstPlot != -1)
	{
		setWorkingPlot(iWorstPlot, false);
		return true;
	}

	// if we still have not removed one, then try again, but do not ignore the one we were told to ignore
	if (extraFreeSpecialists() < 0)
	{
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			if (getSpecialistCount((SpecialistTypes)iI) > 0)
			{
				int iValue = AI_specialistValue(((SpecialistTypes)iI), bAvoidGrowth, /*bRemove*/ true);

				if (iValue < iWorstValue)
				{
					iWorstValue = iValue;
					eWorstSpecialist = ((SpecialistTypes)iI);
				}
			}
		}
	}

	if (eWorstSpecialist != NO_SPECIALIST)
	{
		changeSpecialistCount(eWorstSpecialist, -1);
		return true;
	}

	return false;
}


void CvCityAI::AI_juggleCitizens()
{
	PROFILE_EXTRA_FUNC();
	bool bAvoidGrowth = AI_avoidGrowth();
	bool bIgnoreGrowth = AI_ignoreGrowth();

	// one at a time, remove the worst citizen, then add the best citizen
	// until we add back the same one we removed
	for (int iPass = 0; iPass < 2; iPass++)
	{
		bool bCompletedChecks = false;
		int iCount = 0;

		//std::vector<int> aWorstPlots;
		bool worstPlots[NUM_CITY_PLOTS];
		memset(worstPlots, 0, sizeof(worstPlots));

		while (!bCompletedChecks)
		{
			int iLowestValue = MAX_INT;
			int iWorstPlot = -1;
			int iValue;

			for (int iI = SKIP_CITY_HOME_PLOT; iI < NUM_CITY_PLOTS; iI++)
			{
				if (isWorkingPlot(iI))
				{
					const CvPlot* pLoopPlot = getCityIndexPlot(iI);

					if (pLoopPlot != NULL)
					{
						iValue = AI_plotValue(pLoopPlot, bAvoidGrowth, /*bRemove*/ true, /*bIgnoreFood*/ false, bIgnoreGrowth, (iPass == 0));

						// use <= so that we pick the last one that is lowest, to avoid infinite loop with AI_addBestCitizen
						if (iValue <= iLowestValue)
						{
							iLowestValue = iValue;
							iWorstPlot = iI;
						}
					}
				}
			}

			// if no worst plot, or we looped back around and are trying to remove the first plot we removed, stop
			//if (iWorstPlot == -1 || std::find(aWorstPlots.begin(), aWorstPlots.end(), iWorstPlot) != aWorstPlots.end())
			if (iWorstPlot == -1 || worstPlots[iWorstPlot])
			{
				bCompletedChecks = true;
			}
			else
			{
				// if this the first worst plot, remember it
				//aWorstPlots.push_back(iWorstPlot);
				worstPlots[iWorstPlot] = true;

				setWorkingPlot(iWorstPlot, false);

				if (AI_addBestCitizen(true, true))
				{
					if (isWorkingPlot(iWorstPlot))
					{
						bCompletedChecks = true;
					}
				}
			}

			iCount++;
			if (iCount > (NUM_CITY_PLOTS + 1))
			{
				FErrorMsg("infinite loop");
				break; // XXX
			}
		}

		if ((iPass == 0) && (foodDifference(false) >= 0))
		{
			//good enough, the starvation code
			break;
		}
	}
}


bool CvCityAI::AI_potentialPlot(short* piYields) const
{
	const int iNetFood = piYields[YIELD_FOOD] - GC.getFOOD_CONSUMPTION_PER_POPULATION();

	if (iNetFood < 0)
	{
		if (piYields[YIELD_FOOD] == 0)
		{
			if (piYields[YIELD_PRODUCTION] + piYields[YIELD_COMMERCE] < 2)
			{
				return false;
			}
		}
		else
		{
			if (piYields[YIELD_PRODUCTION] + piYields[YIELD_COMMERCE] == 0)
			{
				return false;
			}
		}
	}

	return true;
}


bool CvCityAI::AI_foodAvailable(int iExtra) const
{
	PROFILE_FUNC();

	bool abPlotAvailable[NUM_CITY_PLOTS];
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		abPlotAvailable[iI] = false;
	}

	int iFoodCount = 0;
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		const CvPlot* pLoopPlot = getCityIndexPlot(iI);

		if (pLoopPlot != NULL)
		{
			if (iI == CITY_HOME_PLOT)
			{
				iFoodCount += pLoopPlot->getYield(YIELD_FOOD);
			}
			else if ((pLoopPlot->getWorkingCity() == this) && AI_potentialPlot(pLoopPlot->getYield()))
			{
				abPlotAvailable[iI] = true;
			}
		}
	}

	int iPopulation = (getPopulation() + iExtra);

	while (iPopulation > 0)
	{
		int iBestValue = 0;
		int iBestPlot = CITY_HOME_PLOT;

		for (int iI = 0; iI < getNumCityPlots(); iI++)
		{
			if (abPlotAvailable[iI])
			{
				const CvPlot* pPlot = getCityIndexPlot(iI);
				if (pPlot)
				{
					const int iValue = pPlot->getYield(YIELD_FOOD);

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						iBestPlot = iI;
					}
				}
			}
		}

		if (iBestPlot != CITY_HOME_PLOT)
		{
			iFoodCount += iBestValue;
			abPlotAvailable[iBestPlot] = false;
		}
		else
		{
			break;
		}

		iPopulation--;
	}

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		iFoodCount += (GC.getSpecialistInfo((SpecialistTypes)iI).getYieldChange(YIELD_FOOD) * getFreeSpecialistCount((SpecialistTypes)iI));
	}

	if (iFoodCount < foodConsumption(false, iExtra))
	{
		return false;
	}

	return true;
}

#ifdef YIELD_VALUE_CACHING
void CvCityAI::AI_NoteSpecialistChange()
{
	ClearYieldValueCache();
}

void CvCityAI::AI_NoteWorkerChange()
{
	ClearYieldValueCache();
}

void CvCityAI::ClearYieldValueCache()
{
	ClearYieldValueCacheImpl();
}

void CvCityAI::ClearYieldValueCacheImpl()
{
	PROFILE_EXTRA_FUNC();
	if (yieldValueCache.currentUseCounter > 0)
	{
		yieldValueCache.currentUseCounter = 0;

		for (int i = 0; i < YIELD_VALUE_CACHE_SIZE; i++)
		{
			yieldValueCache.entries[i].iLastUseCount = 0;
		}
	}
}

void CvCityAI::CheckYieldValueCache(char* label)
{
	PROFILE_EXTRA_FUNC();
	if (yieldValueCache.currentUseCounter > 0)
	{
		for (int i = 0; i < YIELD_VALUE_CACHE_SIZE; i++)
		{
			struct yieldValueCacheEntry* entry = &yieldValueCache.entries[i];

			if (entry->iLastUseCount != 0)
			{
				int realValue = AI_yieldValueInternal(entry->aiYields, entry->aiCommerceYields, entry->bAvoidGrowth, entry->bRemove, entry->bIgnoreFood, entry->bIgnoreGrowth, entry->bIgnoreStarvation, entry->bWorkerOptimization);

				FAssertMsg(realValue == entry->iResult, CvString::format("CheckYieldValueCache %s failed\n", label).c_str());
			}
		}
	}
}

int CvCityAI::yieldValueCacheHits = 0;
int CvCityAI::yieldValueCacheReads = 0;
#endif

int CvCityAI::AI_yieldValue(short* piYields, short* piCommerceYields, bool bAvoidGrowth, bool bRemove, bool bIgnoreFood, bool bIgnoreGrowth, bool bIgnoreStarvation, bool bWorkerOptimization) const
{
PROFILE_EXTRA_FUNC();
#ifdef YIELD_VALUE_CACHING
#ifdef _DEBUG
	//	Uncomment this to perform functional verification
	//#define VERIFY_YIELD_CACHE_RESULTS
#endif

	//	Check cache first
	int worstLRU = 0x7FFFFFFF;

	struct yieldValueCacheEntry* worstLRUEntry = NULL;
	yieldValueCacheReads++;

	//OutputDebugString(CvString::format("AI_yieldValue (%d,%d,%d) at seq %d\n", piYields[0], piYields[1], piYields[2], yieldValueCacheReads).c_str());
	//PROFILE_STACK_DUMP

	for (int i = 0; i < YIELD_VALUE_CACHE_SIZE; i++)
	{
		struct yieldValueCacheEntry* entry = &yieldValueCache.entries[i];
		if (entry->iLastUseCount == 0)
		{
			worstLRUEntry = entry;
			break;
		}

		bool bMatch = true;

		for (int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if (piYields[j] != entry->aiYields[j])
			{
				bMatch = false;
				break;
			}
		}
		if (bMatch)
		{
			if (piCommerceYields != NULL)
			{
				for (int j = 0; j < NUM_COMMERCE_TYPES; j++)
				{
					if (piCommerceYields[j] != entry->aiCommerceYields[j])
					{
						bMatch = false;
						break;
					}
				}
			}
			else
			{
				for (int j = 0; j < NUM_COMMERCE_TYPES; j++)
				{
					if (entry->aiCommerceYields[j] != 0)
					{
						bMatch = false;
						break;
					}
				}
			}
		}

		if (bMatch &&
			entry->bAvoidGrowth == bAvoidGrowth &&
			entry->bRemove == bRemove &&
			entry->bIgnoreFood == bIgnoreFood &&
			entry->bIgnoreGrowth == bIgnoreGrowth &&
			entry->bIgnoreStarvation == bIgnoreStarvation)
		{
			entry->iLastUseCount = ++yieldValueCache.currentUseCounter;
			yieldValueCacheHits++;
#ifdef VERIFY_YIELD_CACHE_RESULTS
			int realValue = AI_yieldValueInternal(piYields, piCommerceYields, bAvoidGrowth, bRemove, bIgnoreFood, bIgnoreGrowth, bIgnoreStarvation, bWorkerOptimization);

			if (realValue != entry->iResult)
			{
				OutputDebugString(CvString::format("Cache entry %08lx verification failed, turn is %d\n", entry, GC.getGame().getGameTurn()).c_str());
				FErrorMsg("Yield value cache verification failure");
				CHECK_YIELD_VALUE_CACHE("Validation");
			}
#endif
			return entry->iResult;
		}
		else if (entry->iLastUseCount < worstLRU)
		{
			worstLRU = entry->iLastUseCount;
			worstLRUEntry = entry;
		}
	}

	int iResult = AI_yieldValueInternal(piYields, piCommerceYields, bAvoidGrowth, bRemove, bIgnoreFood, bIgnoreGrowth, bIgnoreStarvation, bWorkerOptimization);

	FAssertMsg(worstLRUEntry != NULL, "No plot value cache entry found to replace");
	if (worstLRUEntry != NULL)
	{
		for (int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			worstLRUEntry->aiYields[j] = piYields[j];
		}

		if (piCommerceYields != NULL)
		{
			for (int j = 0; j < NUM_COMMERCE_TYPES; j++)
			{
				worstLRUEntry->aiCommerceYields[j] = piCommerceYields[j];
			}
		}
		else
		{
			for (int j = 0; j < NUM_COMMERCE_TYPES; j++)
			{
				worstLRUEntry->aiCommerceYields[j] = 0;
			}
		}

		worstLRUEntry->bAvoidGrowth = bAvoidGrowth;
		worstLRUEntry->bRemove = bRemove;
		worstLRUEntry->bIgnoreFood = bIgnoreFood;
		worstLRUEntry->bIgnoreGrowth = bIgnoreGrowth;
		worstLRUEntry->bIgnoreStarvation = bIgnoreStarvation;
		worstLRUEntry->bWorkerOptimization = bWorkerOptimization;
		worstLRUEntry->iResult = iResult;
		worstLRUEntry->iLastUseCount = ++yieldValueCache.currentUseCounter;
	}

	return iResult;
#else
	return AI_yieldValueInternal(piYields, piCommerceYields, bAvoidGrowth, bRemove, bIgnoreFood, bIgnoreGrowth, bIgnoreStarvation, bWorkerOptimization);
#endif
}


int CvCityAI::AI_yieldValueInternal(short* piYields, short* piCommerceYields, bool bAvoidGrowth, bool bRemove, bool bIgnoreFood, bool bIgnoreGrowth, bool bIgnoreStarvation, bool bWorkerOptimization) const
{
	PROFILE_FUNC();

	const int iBaseProductionValue = 15;
	const int iBaseCommerceValue[NUM_COMMERCE_TYPES] = { 7,10,7,7 };	//	Koshling - boost science a bit

	const int iMaxFoodValue = (3 * iBaseProductionValue) - 1;

	int aiYields[NUM_YIELD_TYPES];
	int aiCommerceYieldsTimes100[NUM_COMMERCE_TYPES];

	bool bEmphasizeFood = AI_isEmphasizeYield(YIELD_FOOD);
	bool bFoodIsProduction = isFoodProduction();
	bool bCanPopRush = GET_PLAYER(getOwner()).canPopRush();

	for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
	{
		aiCommerceYieldsTimes100[iJ] = 0;
	}

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (piYields[iI] == 0)
		{
			aiYields[iI] = 0;
		}
		else
		{
			// Get yield for city after adding/removing the citizen in question
			//TB Traits begin
			int iOldCityYield = getBaseYieldRate((YieldTypes)iI);
			//TB Traits end
			int iNewCityYield = (bRemove ? (iOldCityYield - piYields[iI]) : (iOldCityYield + piYields[iI]));
			const int iModifier = getBaseYieldRateModifier((YieldTypes)iI);

			iNewCityYield = iNewCityYield * iModifier / 100;
			iOldCityYield = iOldCityYield * iModifier / 100;

			// The yield of the citizen in question is the difference of total yields
			// to account for rounding of modifiers
			aiYields[iI] = (bRemove ? (iOldCityYield - iNewCityYield) : (iNewCityYield - iOldCityYield));
		}
	}

	for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
	{
		int iModifier = getTotalCommerceRateModifier((CommerceTypes)iJ);

		int iCommerceTimes100 = aiYields[YIELD_COMMERCE] * GET_PLAYER(getOwner()).getCommercePercent((CommerceTypes)iJ);
		if (piCommerceYields != NULL)
		{
			iCommerceTimes100 += piCommerceYields[iJ] * 100;
		}
		aiCommerceYieldsTimes100[iJ] += (iCommerceTimes100 * iModifier) / 100;
	}

	// should not really use this much, but making it accurate
	aiYields[YIELD_COMMERCE] = 0;
	for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
	{
		aiYields[YIELD_COMMERCE] += aiCommerceYieldsTimes100[iJ] / 100;
	}

	int iValue = 0;
	int iSlaveryValue = 0;

	int iFoodGrowthValue = 0;
	int iFoodGPPValue = 0;

	if (!bIgnoreFood && aiYields[YIELD_FOOD] != 0)
	{
		// tiny food factor, to ensure that even when we don't want to grow,
		// we still prefer more food if everything else is equal
		iValue += (aiYields[YIELD_FOOD] * 1);

		int iFoodPerTurn = (foodDifference(false) - ((bRemove) ? aiYields[YIELD_FOOD] : 0));
		int iFoodLevel = getFood();
		int iFoodToGrow = growthThreshold();
		int iHealthLevel = goodHealth() - badHealth(/*bNoAngry*/ false, 0);
		int iHappinessLevel = (isNoUnhappiness() ? std::max(3, iHealthLevel + 5) : happyLevel() - unhappyLevel(0));
		int iPopulation = getPopulation();
		int	iExtraPopulationThatCanWork = std::min(iPopulation - range(-iHappinessLevel, 0, iPopulation) + std::min(0, extraFreeSpecialists()), NUM_CITY_PLOTS) - getWorkingPopulation() + ((bRemove) ? 1 : 0);

		int iConsumptionByPopulation = getFoodConsumedByPopulation();

		int iAdjustedFoodDifference = (getYieldRate(YIELD_FOOD) + std::min(0, iHealthLevel)) - iConsumptionByPopulation;

		// if we not human, allow us to starve to half full if avoiding growth
		if (!bIgnoreStarvation)
		{
			int iStarvingAllowance = 0;
			if (bAvoidGrowth && !isHuman())
			{
				iStarvingAllowance = std::max(0, (iFoodLevel - std::max(1, ((9 * iFoodToGrow) / 10))));
			}

			if ((iStarvingAllowance < 1) && (iFoodLevel > ((iFoodToGrow * 75) / 100)))
			{
				iStarvingAllowance = 1;
			}

			// if still starving
			if ((iFoodPerTurn + iStarvingAllowance) < 0)
			{
				// if working plots all like this one will save us from starving
				if (std::max(0, iExtraPopulationThatCanWork * aiYields[YIELD_FOOD]) >= -iFoodPerTurn)
				{
					// if this is high food, then we want to pick it first, this will allow us to pick some great non-food later
					int iHighFoodThreshold = std::min(getBestYieldAvailable(YIELD_FOOD), iConsumptionByPopulation + 1);
					if (iFoodPerTurn <= (AI_isEmphasizeGreatPeople() ? 0 : -iHighFoodThreshold) && aiYields[YIELD_FOOD] >= iHighFoodThreshold)
					{
						// value all the food that will contribute to not starving
						iValue += 2048 * std::min(aiYields[YIELD_FOOD], -iFoodPerTurn);
					}
					else
					{
						// give a huge boost to this plot, but not based on how much food it has
						// ie, if working a bunch of 1f 7h plots will stop us from starving, then do not force working unimproved 2f plot
						iValue += 2048;
					}
				}
				else
				{
					// value food high(32), but not forced
					iValue += 32 * std::min(aiYields[YIELD_FOOD], -iFoodPerTurn);
				}
			}
		}

		// if food isn't production, then adjust for growth
		if (bWorkerOptimization || !bFoodIsProduction)
		{
			if (!bAvoidGrowth)
			{
				int iPopToGrow = 0;
				// only do relative checks on food if we want to grow AND we not emph food
				// the emp food case will just give a big boost to all food under all circumstances
				if (bWorkerOptimization || (!bIgnoreGrowth))// && !bEmphasizeFood))
				{
					// also avail: iFoodLevel, iFoodToGrow

					// adjust iFoodPerTurn assuming that we work plots all equal to iConsumtionPerPop
					// that way it is our guesstimate of how much excess food we will have
					iFoodPerTurn += (iExtraPopulationThatCanWork * iConsumptionByPopulation);

					// we have less than 10 extra happy, do some checks to see if we can increase it
					if (iHappinessLevel < 10)
					{
						// if we have anger becase no military, do not count it, on the assumption that it will
						// be remedied soon, and that we still want to grow
						if (getMilitaryHappinessUnits() == 0)
						{
							if (GET_PLAYER(getOwner()).getNumCities() > 2)
							{
								iHappinessLevel += ((GC.getNO_MILITARY_PERCENT_ANGER() * (iPopulation + 1)) / GC.getPERCENT_ANGER_DIVISOR());
							}
						}

						// currently we can at most increase happy by 2 in the following checks
						const int kMaxHappyIncrease = 2;

						// if happy is large enough so that it will be over zero after we do the checks
						int iNewFoodPerTurn = iFoodPerTurn + aiYields[YIELD_FOOD] - iConsumptionByPopulation;
						if ((iHappinessLevel + kMaxHappyIncrease) > 0 && iNewFoodPerTurn > 0)
						{
							int iApproxTurnsToGrow = (iNewFoodPerTurn > 0) ? ((iFoodToGrow - iFoodLevel) / iNewFoodPerTurn) : MAX_INT;

							// do we have hurry anger?
							int iHurryAngerTimer = getHurryAngerTimer();
							if (iHurryAngerTimer > 0)
							{
								int iTurnsUntilAngerIsReduced = iHurryAngerTimer % flatHurryAngerLength();

								// angry population is bad but if we'll recover by the time we grow...
								if (iTurnsUntilAngerIsReduced <= iApproxTurnsToGrow)
								{
									iHappinessLevel++;
								}
							}

							// do we have conscript anger?
							int iConscriptAngerTimer = getConscriptAngerTimer();
							if (iConscriptAngerTimer > 0)
							{
								int iTurnsUntilAngerIsReduced = iConscriptAngerTimer % flatConscriptAngerLength();

								// angry population is bad but if we'll recover by the time we grow...
								if (iTurnsUntilAngerIsReduced <= iApproxTurnsToGrow)
								{
									iHappinessLevel++;
								}
							}

							// do we have defy resolution anger?
							int iDefyResolutionAngerTimer = getDefyResolutionAngerTimer();
							if (iDefyResolutionAngerTimer > 0)
							{
								int iTurnsUntilAngerIsReduced = iDefyResolutionAngerTimer % flatDefyResolutionAngerLength();

								// angry population is bad but if we'll recover by the time we grow...
								if (iTurnsUntilAngerIsReduced <= iApproxTurnsToGrow)
								{
									iHappinessLevel++;
								}
							}
						}
					}

					if (bEmphasizeFood)
					{
						//If we are emphasize food, pay less heed to caps.
						iHealthLevel += 5;
						iHappinessLevel += 2;
					}

					bool bBarFull = (iFoodLevel + iFoodPerTurn /*+ aiYields[YIELD_FOOD]*/ > ((90 * iFoodToGrow) / 100));

					iPopToGrow = std::max(0, iHappinessLevel);
					int iGoodTiles = AI_countGoodTiles(iHealthLevel > 0, true, 50, true);
					iGoodTiles += AI_countGoodSpecialists(iHealthLevel > 0);
					iGoodTiles += bBarFull ? 0 : 1;

					if (!bEmphasizeFood)
					{
						iPopToGrow = std::min(iPopToGrow, iGoodTiles + ((bRemove) ? 1 : 0));
					}

					// if we have growth pontential, fill food bar to 85%
					bool bFillingBar = false;
					if (iPopToGrow == 0 && iHappinessLevel >= 0 && iGoodTiles >= 0 && iHealthLevel >= 0)
					{
						if (!bBarFull)
						{
							if (AI_specialYieldMultiplier(YIELD_PRODUCTION) < 50)
							{
								bFillingBar = true;
							}
						}
					}

					if (getPopulation() < 3)
					{
						iPopToGrow = std::max(iPopToGrow, 3 - getPopulation());
						iPopToGrow += 2;
					}

					// if we want to grow
					if (iPopToGrow > 0 || bFillingBar)
					{

						// will multiply this by factors
						iFoodGrowthValue = aiYields[YIELD_FOOD];
						if (iHealthLevel < (bFillingBar ? 0 : 1))
						{
							iFoodGrowthValue--;
						}

						// (range 1-25) - we want to grow more if we have a lot of growth to do
						// factor goes up like this: 0:1, 1:8, 2:9, 3:10, 4:11, 5:13, 6:14, 7:15, 8:16, 9:17, ... 17:25
						int iFactorPopToGrow;

						if (iPopToGrow < 1 || bFillingBar)
							iFactorPopToGrow = 20 - (10 * (iFoodLevel + iFoodPerTurn + aiYields[YIELD_FOOD])) / iFoodToGrow;
						else if (iPopToGrow < 7)
							iFactorPopToGrow = 17 + 3 * iPopToGrow;
						else
							iFactorPopToGrow = 41;

						iFoodGrowthValue *= iFactorPopToGrow;

						//If we already grow somewhat fast, devalue further food
						//Remember growth acceleration is not dependent on food eaten per
						//pop, 4f twice as fast as 2f twice as fast as 1f...
						int iHighGrowthThreshold = 2 + std::max(std::max(0, 5 - getPopulation()), (iPopToGrow + 1) / 2);
						if (bEmphasizeFood)
						{
							iHighGrowthThreshold *= 2;
						}

						if (iFoodPerTurn > iHighGrowthThreshold)
						{
							iFoodGrowthValue *= 25 + ((75 * iHighGrowthThreshold) / iFoodPerTurn);
							iFoodGrowthValue /= 100;
						}
					}
				}

				//very high food override
				if ((isHuman()) && ((iPopToGrow > 0) || bCanPopRush))
				{
					//very high food override
					int iTempValue = std::max(0, 30 * aiYields[YIELD_FOOD] - 15 * iConsumptionByPopulation);
					iTempValue *= std::max(0, 3 * iConsumptionByPopulation - iAdjustedFoodDifference);
					iTempValue /= 3 * std::max(1, iConsumptionByPopulation);
					if (iHappinessLevel < 0)
					{
						iTempValue *= 2;
						iTempValue /= 1 + 2 * -iHappinessLevel;
					}
					iFoodGrowthValue += iTempValue;
				}
				//Slavery Override
				if (bCanPopRush && (iHappinessLevel > 0))
				{
					iSlaveryValue = 30 * 14 * std::max(0, aiYields[YIELD_FOOD] - ((iHealthLevel < 0) ? 1 : 0));
					iSlaveryValue /= std::max(10, growthThreshold() * (100 - getFoodKeptPercent()));

					iSlaveryValue *= 100;
					iSlaveryValue /= getHurryCostModifier();

					iSlaveryValue *= iConsumptionByPopulation * 2;
					iSlaveryValue /= iConsumptionByPopulation * 2 + std::max(1, iAdjustedFoodDifference);
				}

				//Great People Override
				if ((iExtraPopulationThatCanWork > 1) && AI_isEmphasizeGreatPeople())
				{
					int iAdjust = iConsumptionByPopulation;
					if (iFoodPerTurn == 0)
					{
						iAdjust -= 1;
					}
					iFoodGPPValue += std::max(0, aiYields[YIELD_FOOD] - iAdjust) * std::max(0, (12 + 5 * std::min(0, iHappinessLevel)));
				}
			}
		}
	}


	int iProductionValue = 0;
	int iCommerceValue = 0;
	int iFoodValue = std::min(iFoodGrowthValue, iMaxFoodValue * aiYields[YIELD_FOOD]);
	// if food is production, the count it
	int adjustedYIELD_PRODUCTION = (((bFoodIsProduction) ? aiYields[YIELD_FOOD] : 0) + aiYields[YIELD_PRODUCTION]);

	// value production medium(15)
	iProductionValue += (adjustedYIELD_PRODUCTION * iBaseProductionValue);
	if (!isProduction() && !isHuman())
	{
		iProductionValue /= 2;
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                      05/18/09                                jdog5000      */
	/*                                                                                              */
	/* City AI				                                                                         */
	/************************************************************************************************/
		// If city has more than enough food, but very little production, add large value to production
		// Particularly helps coastal cities with plains forests
	if (aiYields[YIELD_PRODUCTION] > 0)
	{
		if (!bFoodIsProduction && isProduction())
		{
			if (foodDifference(false) >= GC.getFOOD_CONSUMPTION_PER_POPULATION())
			{
				if (getYieldRate(YIELD_PRODUCTION) < (1 + getPopulation() / 3))
				{
					iValue += 128 + 8 * aiYields[YIELD_PRODUCTION];
				}
			}
		}
	}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                       END                                                  */
	/************************************************************************************************/

		// value commerce low(6)

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		if (aiCommerceYieldsTimes100[iI] != 0)
		{
			int iCommerceWeight = GET_PLAYER(getOwner()).AI_commerceWeight((CommerceTypes)iI);
			if (AI_isEmphasizeCommerce((CommerceTypes)iI))
			{
				iCommerceWeight *= 200;
				iCommerceWeight /= 100;
			}
			if (iI == COMMERCE_CULTURE)
			{
				if (getCultureLevel() <= (CultureLevelTypes)1)
				{
					iCommerceValue += (15 * aiCommerceYieldsTimes100[iI]) / 100;
				}
			}
			//	Koshling - Removing the 'normalization' by AI_averageCommerceExchange() from this calculation.  Basically what this did was
			//	to say that commerce types we have a civilization-wide good multiplier for should be devalued relative to those we don't.
			//	However, within the context of city yield evaluation it's not being compared just with other commerce types, but also with other
			//	yield types which have no similar normalization.  The result was that as average science multipliers increased (via buildings and
			//	so on) production was valued relatively more and more over science
			//iCommerceValue += (iCommerceWeight * (aiCommerceYieldsTimes100[iI] * iBaseCommerceValue) * GET_PLAYER(getOwner()).AI_averageCommerceExchange((CommerceTypes)iI)) / 1000000;
			iCommerceValue += (iCommerceWeight * (aiCommerceYieldsTimes100[iI] * iBaseCommerceValue[iI])) / 10000;
		}
	}
	/*
		if (!bWorkerOptimization && bEmphasizeFood)
		{
			if (!bFoodIsProduction)
			{
				// value food extremely high(180)
				iFoodValue *= 125;
				iFoodValue /= 100;
			}
		}

		if (!bWorkerOptimization && AI_isEmphasizeYield(YIELD_PRODUCTION))
		{
			// value production high(80)
			iProductionValue += (adjustedYIELD_PRODUCTION * 80);
		}
	*/
	//Slavery translation
	if ((iSlaveryValue > 0) && (iSlaveryValue > iFoodValue))
	{
		//treat the food component as production
		iFoodValue = 0;
	}
	else
	{
		//treat it as just food
		iSlaveryValue = 0;
	}

	iFoodValue += iFoodGPPValue;
	/*
		if (!bWorkerOptimization && AI_isEmphasizeYield(YIELD_COMMERCE))
		{
			for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
			{
				iCommerceValue += ((iCommerceYields[iI] * 40) * GET_PLAYER(getOwner()).AI_averageCommerceExchange((CommerceTypes)iI)) / 100;
			}
		}

		for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			if (!bWorkerOptimization && AI_isEmphasizeCommerce((CommerceTypes) iJ))
			{
				// value the part of our commerce that goes to our emphasis medium (40)
				iCommerceValue += (iCommerceYields[iJ] * 40);
			}
		}
	*/
	//Lets have some fun with the multipliers, this basically bluntens the impact of
	//massive bonuses.....

	//normalize the production... this allows the system to account for rounding
	//and such while preventing an "out to lunch smoking weed" scenario with
	//unusually high transient production modifiers.
	//Other yields don't have transient bonuses in quite the same way.

/************************************************************************************************/
/* UNOFFICIAL_PATCH                       05/16/10                                jdog5000      */
/*                                                                                              */
/* City AI                                                                                      */
/************************************************************************************************/
	// Rounding can be a problem, particularly for small commerce amounts.  Added safe guards to make
	// sure commerce is counted, even if just a tiny amount.
	if (AI_isEmphasizeYield(YIELD_PRODUCTION))
	{
		iProductionValue *= 130;
		iProductionValue /= 100;

		if (isFoodProduction())
		{
			iFoodValue *= 130;
			iFoodValue /= 100;
		}

		if (!AI_isEmphasizeYield(YIELD_COMMERCE) && iCommerceValue > 0)
		{
			iCommerceValue *= 60;
			iCommerceValue /= 100;
			iCommerceValue = std::max(1, iCommerceValue);
		}
		if (!AI_isEmphasizeYield(YIELD_FOOD) && iFoodValue > 0)
		{
			iFoodValue *= 75;
			iFoodValue /= 100;
			iFoodValue = std::max(1, iFoodValue);
		}
	}
	if (AI_isEmphasizeYield(YIELD_FOOD))
	{
		if (!isFoodProduction())
		{
			iFoodValue *= 130;
			iFoodValue /= 100;
			iSlaveryValue *= 130;
			iSlaveryValue /= 100;
		}
	}
	if (AI_isEmphasizeYield(YIELD_COMMERCE))
	{
		iCommerceValue *= 130;
		iCommerceValue /= 100;
		if (!AI_isEmphasizeYield(YIELD_PRODUCTION) && iProductionValue > 0)
		{
			iProductionValue *= 75;
			iProductionValue /= 100;
			iProductionValue = std::max(1, iProductionValue);
		}
		if (!AI_isEmphasizeYield(YIELD_FOOD) && iFoodValue > 0)
		{
			//Don't supress twice.
			if (!AI_isEmphasizeYield(YIELD_PRODUCTION))
			{
				iFoodValue *= 80;
				iFoodValue /= 100;
				iFoodValue = std::max(1, iFoodValue);
			}
		}
	}

	if (iProductionValue > 0)
	{
		if (isFoodProduction())
		{
			iProductionValue *= 100 + (bWorkerOptimization ? 0 : AI_specialYieldMultiplier(YIELD_PRODUCTION));
			iProductionValue /= 100;
		}
		else
		{
			iProductionValue += iSlaveryValue;
			iProductionValue *= (100 + (bWorkerOptimization ? 0 : AI_specialYieldMultiplier(YIELD_PRODUCTION)));

			iProductionValue /= GET_PLAYER(getOwner()).AI_averageYieldMultiplier(YIELD_PRODUCTION);
		}

		iValue += std::max(1, iProductionValue);
	}

	if (iCommerceValue > 0)
	{
		iCommerceValue *= (100 + (bWorkerOptimization ? 0 : AI_specialYieldMultiplier(YIELD_COMMERCE)));
		iCommerceValue /= GET_PLAYER(getOwner()).AI_averageYieldMultiplier(YIELD_COMMERCE);
		iValue += std::max(1, iCommerceValue);
	}
	//
	if (iFoodValue > 0)
	{
		iFoodValue *= 100;
		iFoodValue /= GET_PLAYER(getOwner()).AI_averageYieldMultiplier(YIELD_FOOD);
		iValue += std::max(1, iFoodValue);
	}
	/************************************************************************************************/
	/* UNOFFICIAL_PATCH                        END                                                  */
	/************************************************************************************************/

	return iValue;
}


int CvCityAI::AI_plotValue(const CvPlot* pPlot, bool bAvoidGrowth, bool bRemove, bool bIgnoreFood, bool bIgnoreGrowth, bool bIgnoreStarvation) const
{
	PROFILE_FUNC();

	int iTotalPotential = getYieldRate(YIELD_FOOD) + pPlot->getYield(YIELD_FOOD);

	short aiYields[NUM_YIELD_TYPES];
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		//TB first plot logic
		if ((YieldTypes)iI == YIELD_FOOD && getPopulation() <= 1 && (iTotalPotential - foodConsumption() <= 0))
		{
			bIgnoreFood = true;
			bIgnoreGrowth = true;
			bIgnoreStarvation = true;
			aiYields[iI] = 0;
		}
		else
		{
			aiYields[iI] = pPlot->getYield((YieldTypes)iI);
		}
	}
	int iValue = 100 * AI_yieldValue(aiYields, NULL, bAvoidGrowth, bRemove, bIgnoreFood, bIgnoreGrowth, bIgnoreStarvation);

	const ImprovementTypes eCurrentImprovement = pPlot->getImprovementType();

	if (eCurrentImprovement != NO_IMPROVEMENT)
	{
		const ImprovementTypes eFinalImprovement = GET_TEAM(getTeam()).finalImprovementUpgrade(eCurrentImprovement);

		if (eFinalImprovement != NO_IMPROVEMENT)
		{
			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				int iYieldDiff = (pPlot->calculateImprovementYieldChange(eFinalImprovement, ((YieldTypes)iI), getOwner()) - pPlot->calculateImprovementYieldChange(eCurrentImprovement, ((YieldTypes)iI), getOwner()));
				aiYields[iI] += iYieldDiff;
			}
			const int iFinalYieldValue = (AI_yieldValue(aiYields, NULL, bAvoidGrowth, bRemove, bIgnoreFood, bIgnoreGrowth, bIgnoreStarvation) * 100);

			if (iFinalYieldValue > iValue)
			{
				iValue = (40 * iValue + 60 * iFinalYieldValue) / 100;
			}
			else
			{
				iValue = (60 * iValue + 40 * iFinalYieldValue) / 100;
			}
		}
	}

	// If we are not emphasizing food or use food for production)
	if ((!AI_isEmphasizeYield(YIELD_FOOD) || isFoodProduction())
	// and this plot is super bad (less than 2 food and less than combined 2 prod/commerce
	&& !AI_potentialPlot(aiYields))
	{
		// undervalue it even more!
		iValue /= 16;
	}

	if (eCurrentImprovement != NO_IMPROVEMENT)
	{
		if (pPlot->getBonusType(getTeam()) == NO_BONUS)
		{
			const CvImprovementInfo& currentImprovement = GC.getImprovementInfo(eCurrentImprovement);
			const CvTeam& team = GET_TEAM(getTeam());

			for (int iI = GC.getNumMapBonuses() - 1; iI > -1; iI--)
			{
				if (currentImprovement.getImprovementBonusDiscoverRand(GC.getMapBonus(iI)) > 0
				&& team.isHasTech((TechTypes)GC.getBonusInfo(GC.getMapBonus(iI)).getTechReveal()))
				{
					iValue += 35;
				}
			}
		}
		if (GC.getImprovementInfo(pPlot->getImprovementType()).getImprovementUpgrade() != NO_IMPROVEMENT)
		{
			iValue += 200;
			iValue -= pPlot->getUpgradeTimeLeft(eCurrentImprovement, NO_PLAYER);
		}
	}
	return iValue;
}


int CvCityAI::AI_experienceWeight() const
{
	return ((getProductionExperience() + getDomainFreeExperience(DOMAIN_SEA)) * 2);
}


int CvCityAI::AI_buildUnitProb() const
{
	int iProb = GC.getLeaderHeadInfo(getPersonalityType()).getBuildUnitProb() + AI_experienceWeight();

	if (GET_PLAYER(getOwner()).AI_isFinancialTrouble())
	{
		iProb /= 2;
	}
	else if (GET_TEAM(getTeam()).getHasMetCivCount(false) == 0)
	{
		iProb /= 2;
	}
	else // more units from cities with military production bonuses
	{
		iProb += std::min(15, getMilitaryProductionModifier() / 4);
	}

	if (GET_PLAYER(getOwner()).isRebel())
	{
		iProb *= 4;
		iProb /= 3;
	}

	if (AI_isMilitaryProductionCity())
	{
		iProb *= 4;
		iProb /= 3;
	}
	else if (GET_PLAYER(getOwner()).AI_getMilitaryProductionCityCount() > 0)
	{
		iProb *= 3;
		iProb /= 4;
	}

	if (AI_isNavalMilitaryProductionCity())
	{
		iProb *= 4;
		iProb /= 3;
	}
	else if (GET_PLAYER(getOwner()).AI_getNavalMilitaryProductionCityCount() > 0)
	{
		iProb *= 3;
		iProb /= 4;
	}
	return iProb;
}

bool CvCityAI::AI_checkIrrigationSpread(const CvPlot* pPlot) const
{
	PROFILE_EXTRA_FUNC();
	bool bEmphasizeIrrigation = false;
	if (pPlot->isIrrigated() || pPlot->isFreshWater() && pPlot->canHavePotentialIrrigation())
	{
		foreach_(const CvPlot * pAdjacentPlot, pPlot->adjacent())
		{
			if (pAdjacentPlot->getOwner() == getOwner()
				&& pAdjacentPlot->isCityRadius()
				&& !pAdjacentPlot->isFreshWater())
			{
				//check for a city? cities can conduct irrigation and that effect is quite
				//useful... so I think irrigate cities.
				if (pAdjacentPlot->isPotentialIrrigation())
				{
					CvPlot* eBestIrrigationPlot = NULL;

					foreach_(CvPlot * pAdjacentPlot2, pAdjacentPlot->adjacent())
					{
						if (pAdjacentPlot2->getOwner() == getOwner())
						{
							if (pAdjacentPlot->isIrrigated())
							{
								//the irrigation has to be coming from somewhere
								if (pAdjacentPlot2->isIrrigated())
								{
									//if we find a tile which is already carrying irrigation
									//then lets not replace that one...
									eBestIrrigationPlot = pAdjacentPlot2;

									if (
										(
											pAdjacentPlot2->isCity()
											|| pAdjacentPlot2->getNonObsoleteBonusType(getTeam()) != NO_BONUS
											|| !pAdjacentPlot2->isCityRadius()
											)
										&& pAdjacentPlot2->isFreshWater())
									{
										//these are all ideal for irrigation chains so stop looking.
										break;
									}
								}
							}
							else if (pAdjacentPlot2->getNonObsoleteBonusType(getTeam()) == NO_BONUS
								&& pAdjacentPlot2->canHavePotentialIrrigation()
								&& pAdjacentPlot2->isIrrigationAvailable())
							{
								//could use more sophisticated logic
								//however this would rely on things like smart irrigation chaining
								//of out-of-city plots
								eBestIrrigationPlot = pAdjacentPlot2;
								break;
							}
						}
					}

					if (pPlot == eBestIrrigationPlot)
					{
						bEmphasizeIrrigation = true;
						break;
					}
				}
			}
		}
	}
	return bEmphasizeIrrigation;
}

void CvCityAI::AI_findBestImprovementForPlot(const CvPlot* pPlot, plotInfo* plotInfo, OutputRatios& ratios) const
{
	PROFILE_EXTRA_FUNC();
	if (plotInfo == NULL) return;
	if (pPlot == NULL) return;
	if (pPlot->getOwner() != getOwner()) return;

	plotInfo->newBuild = NO_BUILD;
	plotInfo->currentImprovement = pPlot->getImprovementType();
	plotInfo->newImprovement = NO_IMPROVEMENT;


	for (int yieldCounter = 0; yieldCounter < NUM_YIELD_TYPES; yieldCounter++)
	{
		plotInfo->yields[yieldCounter] = 0;
		plotInfo->yields[yieldCounter] = pPlot->getYield((YieldTypes)yieldCounter);
	}

	const FeatureTypes eFeature = pPlot->getFeatureType();
	const CvFeatureInfo* currentFeature = eFeature != NO_FEATURE ? &GC.getFeatureInfo(eFeature) : NULL;
	const CvImprovementInfo* currentImprovement = plotInfo->currentImprovement != NO_IMPROVEMENT ? &GC.getImprovementInfo(plotInfo->currentImprovement) : NULL;

	FAssertMsg(pPlot->getOwner() == getOwner(), "pPlot must be owned by this city's owner");

	BonusTypes eNonObsoleteBonus = NO_BONUS;
	bool bonusIsImproved = false;
	bool plotIsWorked = false;
	pPlot->getVisibleBonusState(eNonObsoleteBonus, bonusIsImproved, plotIsWorked);

	if (bonusIsImproved)
	{
		plotInfo->bonus = eNonObsoleteBonus;
		plotInfo->worked = plotIsWorked;
	}

	const bool plotHasBonus = eNonObsoleteBonus != NO_BONUS;
	const int currentPlotValue = plotInfo->currentImprovement != NO_IMPROVEMENT ? CvValueService::CalculateCityPlotValue(ratios, plotInfo->yields, plotHasBonus, bonusIsImproved) : 0;

	//If a worker is already building a build, force that Build.
	BuildTypes eForcedBuild = algo::find_if(
		pPlot->units() | transformed(CvUnit::fn::getBuildType()),
		BuildsAnyImprovement()
	).get_value_or(NO_BUILD);

	// this entire section could be done smarter?
	if (eForcedBuild != NO_BUILD)
	{
		plotInfo->newBuild = eForcedBuild;
		plotInfo->newValue = plotInfo->value;
		return;
	}
	const CvPlayerAI& player = GET_PLAYER(getOwner());

	const bool gameOptionLeaveForestOn = player.isOption(PLAYEROPTION_LEAVE_FORESTS);


	//bool bEmphasizeIrrigation = !bonusIsImproved && AI_checkIrrigationSpread(pPlot);

	//AI_clearfeaturevalue needs to be rewritten to work with new priorities
	// int iClearFeatureValue = currentFeature ? AI_clearFeatureValue(getCityPlotIndex(pPlot)) : 0;

	for (int iI = GC.getNumImprovementInfos() - 1; iI > -1; iI--)
	{
		const ImprovementTypes ePotentialImprovement = static_cast<ImprovementTypes>(iI);
		const CvImprovementInfo& potentialImprovementInfo = GC.getImprovementInfo(ePotentialImprovement);

		// check if improvement is a fort or watchtower, then its a no.
		if (potentialImprovementInfo.isMilitaryStructure()) continue;

		// check if improvement can be built by team
		if (!pPlot->canBuildImprovement(ePotentialImprovement, getTeam())) continue;

		BuildTypes eBestBuild = CvWorkerService::GetFastestBuildForImprovementType(player, ePotentialImprovement, pPlot);

		// if we cannot build any of the valid builds for the improvement, skip to next improvement
		if (eBestBuild == NO_BUILD) continue;

		bool bIgnoreFeature = false;
		bool bValid = true;
		if (eFeature != NO_FEATURE && GC.getBuildInfo(eBestBuild).isFeatureRemove(eFeature))
		{
			bIgnoreFeature = true;

			// check if feature is forest
			if (!plotHasBonus && currentFeature->getYieldChange(YIELD_PRODUCTION) > 0)
			{
				if (gameOptionLeaveForestOn)
				{
					bValid = false;
				}
				else if (healthRate() < 0 && currentFeature->getHealthPercent() > 0)
				{
					bValid = false;
				}
				else if (player.getFeatureHappiness(eFeature) > 0)
				{
					bValid = false;
				}
			}
		}

		if (!bValid) continue;

		for (int yieldCounter = 0; yieldCounter < NUM_YIELD_TYPES; yieldCounter++)
		{
			plotInfo->newYields[yieldCounter] = 0;
			plotInfo->newYields[yieldCounter] = pPlot->calculateNatureYield((YieldTypes)yieldCounter, getTeam(), bIgnoreFeature);
			plotInfo->newYields[yieldCounter] += pPlot->calculateImprovementYieldChange(ePotentialImprovement, (YieldTypes)yieldCounter, getOwner(), false, true);
		}

		// subtract existing plot value
		int plotValue = std::max(0, CvValueService::CalculateCityPlotValue(ratios, plotInfo->newYields, plotHasBonus, plotHasBonus && potentialImprovementInfo.isImprovementBonusTrade(eNonObsoleteBonus)) - currentPlotValue);

		if (plotValue >= plotInfo->newValue)
		{
			plotInfo->newValue = plotValue;
			plotInfo->newBuild = eBestBuild;
			plotInfo->newImprovement = ePotentialImprovement;
		}

	}
	if (plotInfo->newImprovement == plotInfo->currentImprovement) {
		plotInfo->newValue = 0;
		plotInfo->newBuild = NO_BUILD;
		plotInfo->newImprovement = NO_IMPROVEMENT;
	}
}

int CvCityAI::AI_getHappyFromHurry(HurryTypes eHurry) const
{
	return AI_getHappyFromHurry(hurryPopulation(eHurry));
}

int CvCityAI::AI_getHappyFromHurry(int iHurryPopulation) const
{
	const int iHappyDiff = iHurryPopulation - GC.getHURRY_POP_ANGER();

	if (iHappyDiff > 0 && getHurryAngerTimer() <= 1 && 2 * angryPopulation(1) - healthRate(false, 1) > 1)
	{
		return iHappyDiff;
	}
	return 0;
}

int CvCityAI::AI_getHappyFromHurry(HurryTypes eHurry, UnitTypes eUnit) const
{
	return AI_getHappyFromHurry(getHurryPopulation(eHurry, getHurryCost(eUnit)));
}

int CvCityAI::AI_getHappyFromHurry(HurryTypes eHurry, BuildingTypes eBuilding) const
{
	return AI_getHappyFromHurry(getHurryPopulation(eHurry, getHurryCost(eBuilding)));
}


int CvCityAI::AI_cityValue() const
{
	AreaAITypes eAreaAI = area()->getAreaAIType(getTeam());
	if (eAreaAI == AREAAI_OFFENSIVE || eAreaAI == AREAAI_MASSING || eAreaAI == AREAAI_DEFENSIVE)
	{
		return 0;
	}
	return (
		getCommerceRateTimes100(COMMERCE_GOLD)
		+
		getCommerceRateTimes100(COMMERCE_RESEARCH)
		+
		getYieldRate100(YIELD_PRODUCTION)
		-
		3 * calculateColonyMaintenanceTimes100()
		);
}

bool CvCityAI::AI_doPanic()
{
	if (
		area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE
	||	area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE
	||	area()->getAreaAIType(getTeam()) == AREAAI_MASSING)
	{
		const int iRatio =
		(
			100 * GET_PLAYER(getOwner()).AI_getEnemyPlotStrength(plot(), 2, false, false)
			/
			std::max(1, GET_PLAYER(getOwner()).AI_getOurPlotStrength(plot(), 0, true, false))
		);

		if (iRatio > 200)
		{
			if (getProductionUnit() != NO_UNIT)
			{
				if (getProductionProgress() > 0 && GC.getUnitInfo(getProductionUnit()).getCombat() > 0)
				{
					AI_doHurry(true);
					return true;
				}
				return false;
			}
			if (
				GC.getGame().getSorenRandNum(2, "AI choose panic unit") == 0
			&&	(
						AI_chooseUnitImmediate("panic defense", UNITAI_CITY_COUNTER)
					||	AI_chooseUnitImmediate("panic defense", UNITAI_CITY_DEFENSE)
					||	AI_chooseUnitImmediate("panic defense", UNITAI_ATTACK)
				)
			) AI_doHurry(iRatio > 250);
		}
	}
	return false;
}

int CvCityAI::AI_calculateCulturePressure(bool bGreatWork) const
{
	PROFILE_EXTRA_FUNC();
	int iValue = 0;
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		const CvPlot* pLoopPlot = plotCity(getX(), getY(), iI);
		if (pLoopPlot == NULL)
		{
			continue;
		}
		if (pLoopPlot->getOwner() != NO_PLAYER)
		{
			int iTempValue = pLoopPlot->calculateCulturePercent(getOwner());
			if (iTempValue == 100)
			{
				continue; // No culture pressure whatsoever
			}
			if (iTempValue > 0 && iTempValue <= 75)
			{
				// Major foreign culture intrusion
				iTempValue = 100 - iTempValue;

				if (iI != CITY_HOME_PLOT)
				{
					iTempValue *= 4;
					iTempValue /= NUM_CITY_PLOTS;
				}
				const BonusTypes eNonObsoleteBonus = pLoopPlot->getNonObsoleteBonusType(getTeam());

				if (eNonObsoleteBonus != NO_BONUS)
				{
					if (GET_PLAYER(getOwner()).getNumTradeableBonuses(eNonObsoleteBonus) == 0)
					{
						iTempValue += 4 * GET_PLAYER(getOwner()).AI_bonusVal(eNonObsoleteBonus);
					}
					else iTempValue += 2 * GET_PLAYER(getOwner()).AI_bonusVal(eNonObsoleteBonus);
				}

				if (iTempValue > 80 && pLoopPlot->getOwner() == getOwner())
				{
					// Captured territory special case
					iTempValue *= 100 - iTempValue;
					iTempValue /= 100;
				}

				if (pLoopPlot->getTeam() != getTeam())
				{
					iTempValue *= 2;
					if (bGreatWork && GET_PLAYER(getOwner()).AI_getAttitude(pLoopPlot->getOwner()) == ATTITUDE_FRIENDLY)
					{
						iValue /= 10;
					}
				}
				else if (bGreatWork)
				{
					iTempValue /= 10;
				}
				else iTempValue /= 2;

				iValue += iTempValue;
			}
			else iValue++; // Minor foreign culture intrusion
		}
		else iValue++; // Neutral land
	}
	return iValue;
}


void CvCityAI::AI_buildGovernorChooseProduction()
{
	PROFILE_FUNC();

	// only clear the dirty bit if we actually do a check, multiple items might be queued
	AI_setChooseProductionDirty(false);

	CvArea* pWaterArea = waterArea();

	const bool bWasFoodProduction = isFoodProduction();
	clearOrderQueue();

	if (bWasFoodProduction)
	{
		AI_assignWorkingPlots();
	}

#ifdef _MOD_GOVWORKERS
	if (!isHuman() || GET_PLAYER(getOwner()).isOption(PLAYEROPTION_MODDER_1))
#endif
	{
		//workboat
		if (pWaterArea != NULL
			&& GET_PLAYER(getOwner()).AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_WORKER_SEA) == 0
			&& AI_neededSeaWorkers() > 0
			&& AI_chooseUnit("no sea workers for area", UNITAI_WORKER_SEA))
		{
			return;
		}
	}

	// Hammers first, always.
	if (AI_chooseBuilding(BUILDINGFOCUS_PRODUCTION))
	{
		return;
	}
	const int iPop = getPopulation();

	if (AI_countNumBonuses(NO_BONUS, false, true, 10, true, true) > 0
	&& iPop > AI_countNumBonuses(NO_BONUS, true, false, -1, true, true)
	&& getCommerceRate(COMMERCE_CULTURE) == 0
	&& AI_chooseBuilding(BUILDINGFOCUS_CULTURE))
	{
		return;
	}

	// pick granary or lighthouse, any duration
	if (AI_chooseBuilding(BUILDINGFOCUS_FOOD))
	{
		return;
	}

	if (angryPopulation(1) > 1 && AI_chooseBuilding(BUILDINGFOCUS_HAPPY, 10))
	{
		return;
	}

	if (AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, 8, 33))
	{
		return;
	}
	const int iCulturePressure = AI_calculateCulturePressure();

	if (iCulturePressure > 100
		|| iCulturePressure != 0 && getCommerceRateTimes100(COMMERCE_CULTURE) == 0)
	{
		if (AI_chooseBuilding(BUILDINGFOCUS_CULTURE, 15))
		{
			return;
		}
	}
	int iEconomyFlags = 0;
	iEconomyFlags |= BUILDINGFOCUS_GOLD;
	iEconomyFlags |= BUILDINGFOCUS_RESEARCH;
	iEconomyFlags |= BUILDINGFOCUS_MAINTENANCE;
	iEconomyFlags |= BUILDINGFOCUS_HAPPY;
	iEconomyFlags |= BUILDINGFOCUS_HEALTHY;
	iEconomyFlags |= BUILDINGFOCUS_SPECIALIST;
	iEconomyFlags |= BUILDINGFOCUS_ESPIONAGE;

	const int iMinValueDivisor = iPop < 3 ? 3 : (iPop < 7 ? 2 : 1);

	//20 means 5g or ~2 happiness...
	if (AI_chooseBuilding(iEconomyFlags, 12, 20 / iMinValueDivisor))
	{
		return;
	}

#ifdef _MOD_GOVWORKERS
	if (!isHuman() || GET_PLAYER(getOwner()).isOption(PLAYEROPTION_MODDER_1))
#endif
	{
		if (!AI_isDanger()
		&& (
			GET_PLAYER(getOwner()).AI_totalAreaUnitAIs(area(), UNITAI_WORKER)
			<
			(GET_PLAYER(getOwner()).AI_neededWorkers(area()) + 1) / 2
		)
		&& AI_chooseUnit("no danger optional worker", UNITAI_WORKER))
		{
			return;
		}
	}

	if (GC.getDEFAULT_SPECIALIST() != NO_SPECIALIST
	&& getSpecialistCount((SpecialistTypes)GC.getDEFAULT_SPECIALIST()) > 0
	&& AI_chooseBuilding(BUILDINGFOCUS_SPECIALIST, 20)
	|| AI_chooseBuilding(iEconomyFlags, 10, 15 / iMinValueDivisor)
	|| AI_chooseBuilding(iEconomyFlags | BUILDINGFOCUS_CULTURE, 10, 10 / iMinValueDivisor))
	{
		return;
	}
	if (AI_chooseBuilding())
	{
		return;
	}
	// As last resort select a process
	if (!getHeadOrder() && !AI_chooseProcess(NO_COMMERCE))
	{
		FErrorMsg(CvString::format("Governor could not choose production for city %S", m_szName.c_str()).c_str());
	}
}

int CvCityAI::AI_calculateWaterWorldPercent() const
{
	PROFILE_EXTRA_FUNC();
	int iTeamCityCount = 0;
	int iOtherCityCount = 0;
	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (iI == getTeam() || GET_TEAM((TeamTypes)iI).isVassal(getTeam()) || GET_TEAM(getTeam()).isVassal((TeamTypes)iI))
			{
				iTeamCityCount += GET_TEAM((TeamTypes)iI).countNumCitiesByArea(area());
			}
			else iOtherCityCount += GET_TEAM((TeamTypes)iI).countNumCitiesByArea(area());
		}
	}

	int iWaterPercent = 100;
	if (iOtherCityCount != 0)
	{
		iWaterPercent -= (iTeamCityCount + iOtherCityCount) * 100 / GC.getGame().getNumCities();
	}
	iWaterPercent /= 2;

	iWaterPercent += 50 * (2 + iTeamCityCount) / (2 + iTeamCityCount + iOtherCityCount);

	return std::max(1, iWaterPercent);
}


// The magic value is basically "Look at this plot, is it worth working"
//	Less than -50 means the plot is worthless in a "workers kill yourself" kind of way.
//	-50 to -1 means the plot isn't worth growing to work - might be okay with emphasize though.
//	0 to 50 means it is marginal.
//	50 to 100 means it's okay.
//	Above 100 means it's definitely decent - seriously question ever not working it.
//	This function deliberately doesn't use emphasize settings.
int CvCityAI::AI_getPlotMagicValue(const CvPlot* pPlot, bool bHealthy, bool bWorkerOptimization) const
{
	PROFILE_EXTRA_FUNC();
	FAssert(pPlot);

	int yields100[NUM_YIELD_TYPES];

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (bWorkerOptimization && pPlot->getWorkingCity() == this && AI_getBestBuild(getCityPlotIndex(pPlot)) != NO_BUILD)
		{
			yields100[iI] = pPlot->getYieldWithBuild(AI_getBestBuild(getCityPlotIndex(pPlot)), (YieldTypes)iI, true) * 100;
		}
		else
		{
			yields100[iI] = pPlot->getYield((YieldTypes)iI) * 100;
		}
	}
	const ImprovementTypes eCurrentImprovement = pPlot->getImprovementType();

	if (eCurrentImprovement != NO_IMPROVEMENT)
	{
		const ImprovementTypes eFinalImprovement = finalImprovementUpgrade(eCurrentImprovement);

		if (eFinalImprovement != NO_IMPROVEMENT && eFinalImprovement != eCurrentImprovement)
		{
			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				yields100[iI] += (
					(
						100 * pPlot->calculateImprovementYieldChange(eFinalImprovement, (YieldTypes)iI, getOwner())
						-
						100 * pPlot->calculateImprovementYieldChange(eCurrentImprovement, (YieldTypes)iI, getOwner())
					)
					/ 2
				);
			}
		}
	}
	const int iReductant = (
		100 *
		std::min(
			getFoodConsumedPerPopulation100(!bHealthy),
			2 *
			std::max(
				std::max(
					yields100[YIELD_FOOD],
					yields100[YIELD_PRODUCTION]
				),
				yields100[YIELD_COMMERCE]
			)
		)
	);
	return (
		(
			yields100[YIELD_FOOD] * 125
			+
			yields100[YIELD_PRODUCTION] * 75
			+
			yields100[YIELD_COMMERCE] * 50
			-
			iReductant
		)
		/ 1000
	);
}

// Useful for deciding whether or not to grow... or whether the city needs terrain improvement.
// If healthy is false it assumes bad health conditions.
int CvCityAI::AI_countGoodTiles(bool bHealthy, bool bUnworkedOnly, int iThreshold, bool bWorkerOptimization) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	foreach_(const CvPlot * plotX, plots(true))
	{
		if (plotX->getWorkingCity() == this
		&& (!bUnworkedOnly || !plotX->isBeingWorked())
		&& AI_getPlotMagicValue(plotX, bHealthy) > iThreshold)
		{
			iCount++;
		}
	}
	return iCount;
}

int CvCityAI::AI_calculateTargetCulturePerTurn() const
{
	PROFILE_EXTRA_FUNC();
	bool bOwnAllWorkablePlots = true;
	bool bCompetition = false;
	bool bBonus = false;

	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		const CvPlot* plotX = plotCity(getX(),getY(),iI);

		if (plotX != NULL && plotX->getOwner() != getOwner())
		{
			if (plotX->isOwned())
			{
				bCompetition = true;
			}
			if (plotX->getBonusType(getTeam()) != NO_BONUS)
			{
				bBonus = true;
			}
			bOwnAllWorkablePlots = false;
		}
	}
	if (bOwnAllWorkablePlots)
	{
		return 0;
	}
	int iTarget = getCommerceRate(COMMERCE_CULTURE) + 1;

	if (bCompetition)
	{
		if (bBonus)
		{
			iTarget += 20;
			iTarget *= 5;
			iTarget /= 4;
		}
		iTarget *= 5;
		iTarget /= 4;
	}
	return iTarget;
}

int CvCityAI::AI_countGoodSpecialists(bool bHealthy) const
{
	PROFILE_EXTRA_FUNC();
	const CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	int iCount = 0;
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		const SpecialistTypes eSpecialist = (SpecialistTypes)iI;

		int iValue = 0;

		iValue += 100 * kPlayer.specialistYield(eSpecialist, YIELD_FOOD);
		iValue += 65 * kPlayer.specialistYield(eSpecialist, YIELD_PRODUCTION);
		iValue += 40 * kPlayer.specialistYield(eSpecialist, YIELD_COMMERCE);

		iValue += 40 * kPlayer.specialistCommerce(eSpecialist, COMMERCE_RESEARCH);
		iValue += 40 * kPlayer.specialistCommerce(eSpecialist, COMMERCE_GOLD);
		iValue += 20 * kPlayer.specialistCommerce(eSpecialist, COMMERCE_ESPIONAGE);
		iValue += 15 * kPlayer.specialistCommerce(eSpecialist, COMMERCE_CULTURE);
		iValue += 25 * GC.getSpecialistInfo(eSpecialist).getGreatPeopleRateChange();

		iValue += GC.getSpecialistInfo(eSpecialist).getHealthPercent();
		iValue += GC.getSpecialistInfo(eSpecialist).getHappinessPercent();
		iValue += GC.getSpecialistInfo(eSpecialist).getInvestigation();
		iValue -= GC.getSpecialistInfo(eSpecialist).getInsidiousness();

		if (iValue >= (bHealthy ? 200 : 300))
		{
			iCount += getMaxSpecialistCount(eSpecialist);
		}
	}
	iCount -= getFreeSpecialist();

	return iCount;
}
//0 is normal
//higher than zero means special.
int CvCityAI::AI_getCityImportance(bool bEconomy, bool bMilitary) const
{
	int iValue = 0;

	if (GET_PLAYER(getOwner()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2))
	{
		int iCultureRateRank = findCommerceRateRank(COMMERCE_CULTURE);
		int iCulturalVictoryNumCultureCities = GC.getGame().culturalVictoryNumCultureCities();

		if (iCultureRateRank <= iCulturalVictoryNumCultureCities)
		{
			iValue += 100;

			if (getCultureLevel() < GC.getNumCultureLevelInfos() - 1)
			{
				iValue += !bMilitary ? 100 : 0;
			}
			else iValue += bMilitary ? 100 : 0;
		}
	}
	return iValue;
}

void CvCityAI::AI_stealPlots()
{
	PROFILE_FUNC();

	const int iImportance = AI_getCityImportance(true, false);

	foreach_(CvPlot * pLoopPlot, plots(NUM_CITY_PLOTS))
	{
		if (iImportance > 0 && pLoopPlot->getOwner() == getOwner())
		{
			CvCityAI* pWorkingCity = static_cast<CvCityAI*>(pLoopPlot->getWorkingCity());
			if ((pWorkingCity != this) && (pWorkingCity != NULL))
			{
				FAssert(pWorkingCity->getOwner() == getOwner());
				const int iOtherImportance = pWorkingCity->AI_getCityImportance(true, false);
				if (iImportance > iOtherImportance)
				{
					pLoopPlot->setWorkingCityOverride(this);
				}
			}
		}

		if (pLoopPlot->getWorkingCityOverride() == this)
		{
			if (pLoopPlot->getOwner() != getOwner())
			{
				pLoopPlot->setWorkingCityOverride(NULL);
			}
		}
	}
}


int CvCityAI::AI_yieldMultiplier(YieldTypes eYield) const
{
	PROFILE_FUNC();

	int iMultiplier = getBaseYieldRateModifier(eYield);

	if (eYield == YIELD_PRODUCTION)
	{
		iMultiplier += (getMilitaryProductionModifier() / 2);
		for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
		{
			iMultiplier += getUnitCombatProductionModifier((UnitCombatTypes)iI) / 4;
		}
	}

	if (eYield == YIELD_COMMERCE)
	{
		iMultiplier += (getTotalCommerceRateModifier(COMMERCE_RESEARCH) * 60) / 100;
		iMultiplier += (getTotalCommerceRateModifier(COMMERCE_GOLD) * 35) / 100;
		iMultiplier += (getTotalCommerceRateModifier(COMMERCE_CULTURE) * 15) / 100;
	}

	return iMultiplier;
}
//this should be called before doing governor stuff.
//This is the function which replaces emphasis
//Could stand for a Commerce Variety to be added
//especially now that there is Espionage
void CvCityAI::AI_updateSpecialYieldMultiplier()
{
	PROFILE_FUNC();

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiSpecialYieldMultiplier[iI] = 0;
	}

	const UnitTypes eProductionUnit = getProductionUnit();
	if (eProductionUnit != NO_UNIT)
	{
		if (GC.getUnitInfo(eProductionUnit).getDefaultUnitAIType() == UNITAI_WORKER_SEA)
		{
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 50;
			m_aiSpecialYieldMultiplier[YIELD_COMMERCE] -= 50;
		}
		if ((GC.getUnitInfo(eProductionUnit).getDefaultUnitAIType() == UNITAI_WORKER) ||
			(GC.getUnitInfo(eProductionUnit).getDefaultUnitAIType() == UNITAI_SETTLE))

		{
			m_aiSpecialYieldMultiplier[YIELD_COMMERCE] -= 50;
		}
	}

	const BuildingTypes eProductionBuilding = getProductionBuilding();
	if (eProductionBuilding != NO_BUILDING)
	{
		if (isWorldWonder(eProductionBuilding) || isProductionProject())
		{
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 50;
			m_aiSpecialYieldMultiplier[YIELD_COMMERCE] -= 25;
		}
		m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += std::max(-25, GC.getBuildingInfo(eProductionBuilding).getFoodKept());

		if (GC.getBuildingInfo(eProductionBuilding).getCommerceChange(COMMERCE_CULTURE) > 0
			|| GC.getBuildingInfo(eProductionBuilding).getCommercePerPopChange(COMMERCE_CULTURE) > 0)
		{
			const int iTargetCultureRate = AI_calculateTargetCulturePerTurn();
			if (iTargetCultureRate > 0)
			{
				if (iTargetCultureRate == 1)
				{
					m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 50;
				}
				else
				{
					m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 20;
				}
			}
		}
	}

	// non-human production value increase
	if (!isHuman())
	{
		const CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
		const AreaAITypes eAreaAIType = area()->getAreaAIType(getTeam());

		if ((kPlayer.AI_isDoStrategy(AI_STRATEGY_DAGGER) && getPopulation() >= 4)
			|| (eAreaAIType == AREAAI_OFFENSIVE) || (eAreaAIType == AREAAI_DEFENSIVE)
			|| (eAreaAIType == AREAAI_MASSING) || (eAreaAIType == AREAAI_ASSAULT))
		{
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 10;
			if (!kPlayer.AI_isFinancialTrouble())
			{
				m_aiSpecialYieldMultiplier[YIELD_COMMERCE] -= 40;
			}
		}


		const short iProfitMargin = kPlayer.getProfitMargin();

		if (iProfitMargin <= 40)
		{
			//Greatly decrease production weight
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] *= (40 + iProfitMargin * 5) / 4; // range from 10 to 60
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] /= 80; // 0.125 - 0.75
		}
		else if (iProfitMargin <= 60)
		{
			//Slightly decrease production weight.
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] *= 110 + iProfitMargin * 4; // range from 274 to 350
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] /= 360; // 0.761 - 0.972
		}
	}
}

int CvCityAI::AI_specialYieldMultiplier(YieldTypes eYield) const
{
	return m_aiSpecialYieldMultiplier[eYield];
}


int CvCityAI::AI_countNumBonuses(BonusTypes eBonus, bool bIncludeOurs, bool bIncludeNeutral, int iOtherCultureThreshold, bool bLand, bool bWater) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	foreach_(const CvPlot * pLoopPlot, plots())
	{
		if (pLoopPlot->area() == area() || (bWater && pLoopPlot->isWater()))
		{
			const BonusTypes eLoopBonus = pLoopPlot->getBonusType(getTeam());
			if (eLoopBonus != NO_BONUS)
			{
				if ((eBonus == NO_BONUS) || (eBonus == eLoopBonus))
				{
					if (bIncludeOurs && (pLoopPlot->getOwner() == getOwner()) && (pLoopPlot->getWorkingCity() == this))
					{
						iCount++;
					}
					else if (bIncludeNeutral && (!pLoopPlot->isOwned()))
					{
						iCount++;
					}
					else if ((iOtherCultureThreshold > 0) && (pLoopPlot->isOwned() && (pLoopPlot->getOwner() != getOwner())))
					{
						if ((pLoopPlot->getCulture(pLoopPlot->getOwner()) - pLoopPlot->getCulture(getOwner())) < iOtherCultureThreshold)
						{
							iCount++;
						}
					}
				}
			}
		}
	}

	return iCount;
}


int CvCityAI::AI_countNumImprovableBonuses(bool bIncludeNeutral, TechTypes eExtraTech, bool bLand, bool bWater) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	foreach_(const CvPlot * pLoopPlot, plots(NUM_CITY_PLOTS))
	{
		if (bLand && pLoopPlot->area() == area() || bWater && pLoopPlot->isWater())
		{
			const BonusTypes eLoopBonus = pLoopPlot->getBonusType(getTeam());
			if (eLoopBonus != NO_BONUS
			&& (pLoopPlot->getOwner() == getOwner() && pLoopPlot->getWorkingCity() == this || bIncludeNeutral && !pLoopPlot->isOwned()))
			{
				const std::vector<std::pair<ImprovementTypes, BuildTypes> >* improvements = GC.getBonusInfo(eLoopBonus).getTradeProvidingImprovements();

				for (std::vector<std::pair<ImprovementTypes, BuildTypes> >::const_iterator itr = improvements->begin(); itr != improvements->end(); ++itr)
				{
					if (GET_PLAYER(getOwner()).canBuild(pLoopPlot, itr->second))
					{
						iCount++;
						break;
					}
					else if ((eExtraTech != NO_TECH))
					{
						const CvBuildInfo& kBuild = GC.getBuildInfo(itr->second);

						//	Koshling - not checking if eExtraTech obsoletes the build since we ARE checking is ENABLES it, and it
						//	makes no sense for the same tech to boh enable and obsolete a build.  However, we DO need to check that
						//	we do not ALREADY have an obsoleting tech
						if (kBuild.getTechPrereq() == eExtraTech
						&& (kBuild.getObsoleteTech() == NO_TECH || !GET_TEAM(getTeam()).isHasTech(kBuild.getObsoleteTech())))
						{
							iCount++;
							break;
						}
					}
				}
			}
		}
	}

	return iCount;
}


int CvCityAI::AI_playerCloseness(PlayerTypes eIndex, int iMaxDistance)
{
	FAssert(GET_PLAYER(eIndex).isAlive());

	if (m_iCachePlayerClosenessTurn != GC.getGame().getGameTurn()
		|| m_iCachePlayerClosenessDistance != iMaxDistance)
	{
		AI_cachePlayerCloseness(iMaxDistance);
	}
	return m_aiPlayerCloseness[eIndex];
}

void CvCityAI::AI_cachePlayerCloseness(int iMaxDistance)
{
	PROFILE_FUNC();

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_TEAM(getTeam()).isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
		{
			int iValue = 0;
			int iBestValue = 0;
			foreach_(const CvCity * pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
			{
				if (pLoopCity == this)
				{
					continue;
				}
				int iDistance = stepDistance(getX(), getY(), pLoopCity->getX(), pLoopCity->getY());

				if (area() != pLoopCity->area())
				{
					iDistance += 1;
					iDistance /= 2;
				}
				if (iDistance <= iMaxDistance)
				{
					if (getArea() == pLoopCity->getArea())
					{
						const int iPathDistance = GC.getMap().calculatePathDistance(plot(), pLoopCity->plot());
						if (iPathDistance > 0)
						{
							iDistance = iPathDistance;
						}
					}
					if (iDistance <= iMaxDistance)
					{
						// Weight by population of both cities, not just pop of other city
						int iTempValue = 20 + pLoopCity->getPopulation() + getPopulation();

						iTempValue *= 1 + iMaxDistance - iDistance;
						iTempValue /= 1 + iMaxDistance;

						//reduce for small islands.
						const int iAreaCityCount = pLoopCity->area()->getNumCities();
						iTempValue *= std::min(iAreaCityCount, 5);
						iTempValue /= 5;
						if (iAreaCityCount < 3)
						{
							iTempValue /= 2;
						}

						if (pLoopCity->isHominid())
						{
							iTempValue /= 4;
						}

						iValue += iTempValue;
						iBestValue = std::max(iBestValue, iTempValue);
					}
				}
			}
			m_aiPlayerCloseness[iI] = (iBestValue + iValue / 4);
		}
	}
	m_iCachePlayerClosenessTurn = GC.getGame().getGameTurn();
	m_iCachePlayerClosenessDistance = iMaxDistance;
}

int	CvCityAI::getPlayerDangerPercentage(PlayerTypes ePlayer, int& iModifier) const
{
	iModifier = 0;

	const CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	bool bCrushStrategy = GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_CRUSH);

	int	iResult = 0;
	if ((bCrushStrategy) && (GET_TEAM(getTeam()).AI_getWarPlan(kPlayer.getTeam()) != NO_WARPLAN))
	{
		iResult = 400;
	}
	else if (atWar(getTeam(), kPlayer.getTeam()))
	{
		iResult = 300;
	}
	// Beef up border security before starting war, but not too much
	else if (GET_TEAM(getTeam()).AI_getWarPlan(kPlayer.getTeam()) != NO_WARPLAN)
	{
		iResult = 180;
	}
	// Extra trust of/for Vassals, regardless of relations
	else if (GET_TEAM(kPlayer.getTeam()).isVassal(getTeam()) ||
		GET_TEAM(getTeam()).isVassal(kPlayer.getTeam()))
	{
		iResult = 30;
	}
	else
	{
		switch (GET_PLAYER(getOwner()).AI_getAttitude(ePlayer))
		{
		case ATTITUDE_FURIOUS:
			iResult = 180;
			break;

		case ATTITUDE_ANNOYED:
			iResult = 130;
			break;

		case ATTITUDE_CAUTIOUS:
			iResult = 100;
			break;

		case ATTITUDE_PLEASED:
			iResult = 50;
			break;

		case ATTITUDE_FRIENDLY:
			iResult = 20;
			break;

		default:
			FErrorMsg("error");
			break;
		}

		// Beef up border security next to powerful rival, (Fuyu) just not too much if our units are weaker on average
		if (kPlayer.getPower() > GET_PLAYER(getOwner()).getPower())
		{
			int iTempMultiplier = std::min(400, (100 * kPlayer.getPower()) / std::max(1, GET_PLAYER(getOwner()).getPower()));
			iTempMultiplier += range(((100 * kPlayer.getNumMilitaryUnits()) / std::max(1, GET_PLAYER(getOwner()).getNumMilitaryUnits())), 100, iTempMultiplier);
			iTempMultiplier /= 2;
			iResult *= iTempMultiplier;
			iResult /= 100;
		}

		if (bCrushStrategy)
		{
			iResult /= 2;
		}
	}

	//	Koshling - modify by the difference in difficulty levels between us and the player
	//	concerned (upwards only). This is to encourage the AI to be more prepared against humnas
	//	playing on higher difficulties
	int	iDifficultyDifference = (GET_PLAYER(getOwner()).getHandicapType() - GET_PLAYER(ePlayer).getHandicapType());
	if (iDifficultyDifference < 0)
	{
		iModifier = -10 * iDifficultyDifference;
		iResult = (iResult * (100 + iModifier)) / 100;
	}

	return iResult;
}

int CvCityAI::AI_cityThreat(TeamTypes eTargetTeam, int* piThreatModifier)
{
	PROFILE_FUNC();

	if (m_iCityThreat != -1 && eTargetTeam == NO_TEAM)
	{
		if (piThreatModifier != NULL)
		{
			*piThreatModifier = m_iCityThreatModifier;
		}
		return m_iCityThreat;
	}

	int iValue = (eTargetTeam == NO_TEAM ? evaluateDanger() : 0);
	int iModifier = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(eTargetTeam) && iI != getOwner())
		{
			int iTempValue = AI_playerCloseness((PlayerTypes)iI, DEFAULT_PLAYER_CLOSENESS);
			if (iTempValue > 0)
			{
				int iTempModifier;

				iTempValue *= getPlayerDangerPercentage((PlayerTypes)iI, iTempModifier);
				iTempValue /= 100;
				iValue += iTempValue;

				iModifier += iTempModifier;
			}
		}
	}

	if (isCoastal(GC.getWorldInfo(GC.getMap().getWorldSize()).getOceanMinAreaSize()))
	{
		int iCurrentEra = GET_PLAYER(getOwner()).getCurrentEra();
		iValue += std::max(0, ((10 * iCurrentEra) / 3) - 6); //there are better ways to do this
	}

	iValue += getNumActiveWorldWonders() * 5;

	if (GET_PLAYER(getOwner()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3))
	{
		iValue += 5;
		iValue += getTotalCommerceRateModifier(COMMERCE_CULTURE) / 20;
		if (getCultureLevel() >= (GC.getNumCultureLevelInfos() - 2))
		{
			iValue += 20;
			if (getCultureLevel() >= (GC.getNumCultureLevelInfos() - 1))
			{
				iValue += 30;
			}
		}
	}

	iValue += 2 * GET_PLAYER(getOwner()).AI_getPlotDanger(plot(), 3, false);

	if (getRevolutionIndex() > 750)
	{
		iValue += getRevolutionIndex() / 25;
	}
	else
	{
		iValue += getRevolutionIndex() / 100;
	}

	m_iCityThreat = iValue;
	m_iCityThreatModifier = iModifier;
	if (piThreatModifier != NULL)
	{
		*piThreatModifier = m_iCityThreatModifier;
	}

	return iValue;
}

//This needs to be serialized for human workers.
void CvCityAI::AI_updateWorkersNeededHere()
{
	PROFILE_FUNC();

	short aiYields[NUM_YIELD_TYPES];

	int iUnimprovedWorkedPlotCount = 0;
	int iUnimprovedUnworkedPlotCount = 0;
	int iWorkedUnimprovableCount = 0;
	int iImprovedUnworkedPlotCount = 0;

	int iSpecialCount = 0;

	int iWorstWorkedPlotValue = MAX_INT;
	int iBestUnworkedPlotValue = 0;

	for (int iI = SKIP_CITY_HOME_PLOT; iI < NUM_CITY_PLOTS; iI++)
	{
		const CvPlot* plotX = getCityIndexPlot(iI);
		if (plotX && plotX->getWorkingCity() == this && plotX->getArea() == getArea())
		{
			if (plotX->getImprovementType() == NO_IMPROVEMENT)
			{
				if (plotX->isBeingWorked())
				{
					(
						AI_getBestBuild(iI) != NO_BUILD
						?
						iUnimprovedWorkedPlotCount++
						:
						iWorkedUnimprovableCount++
					);
				}
				else if (AI_getBestBuild(iI) != NO_BUILD)
				{
					iUnimprovedUnworkedPlotCount++;
				}
			}
			else if (!plotX->isBeingWorked())
			{
				iImprovedUnworkedPlotCount++;
			}

			for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				aiYields[iJ] = plotX->getYield((YieldTypes)iJ);
			}

			if (plotX->isBeingWorked())
			{
				iWorstWorkedPlotValue = std::min(iWorstWorkedPlotValue, AI_yieldValue(aiYields, NULL, false, false, false, false, true, true));
			}
			else iBestUnworkedPlotValue = std::max(iBestUnworkedPlotValue, AI_yieldValue(aiYields, NULL, false, false, false, false, true, true));
		}
	}
	//specialists?

	iUnimprovedWorkedPlotCount += std::min(iUnimprovedUnworkedPlotCount, iWorkedUnimprovableCount) / 2;

	int iWorkersNeeded = 2 * iUnimprovedWorkedPlotCount;

	int iBestPotentialPlotValue = -1;
	if (iWorstWorkedPlotValue != MAX_INT)
	{
		//Add an additional citizen to account for future growth.
		int iBestPlot = -1;
		SpecialistTypes eBestSpecialist = NO_SPECIALIST;

		if (angryPopulation() == 0)
		{
			AI_addBestCitizen(true, true, &iBestPlot, &eBestSpecialist);
		}

		for (int iI = SKIP_CITY_HOME_PLOT; iI < NUM_CITY_PLOTS; iI++)
		{
			const CvPlot* plotX = getCityIndexPlot(iI);

			if (plotX
			&& plotX->getWorkingCity() == this
			&& plotX->getArea() == getArea()
			&& AI_getBestBuild(iI) != NO_BUILD)
			{
				for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					aiYields[iJ] = plotX->getYieldWithBuild(m_aeBestBuild[iI], (YieldTypes)iJ, true);
				}

				const int iPlotValue = AI_yieldValue(aiYields, NULL, false, false, false, false, true, true);
				const ImprovementTypes eBestImp = GC.getBuildInfo(AI_getBestBuild(iI)).getImprovement();
				if (eBestImp != NO_IMPROVEMENT
					&& (getImprovementFreeSpecialists(eBestImp) > 0 || GC.getImprovementInfo(eBestImp).getHappiness() > 0))
				{
					iSpecialCount++;
				}
				iBestPotentialPlotValue = std::max(iBestPotentialPlotValue, iPlotValue);
			}
		}

		if (iBestPlot != -1)
		{
			setWorkingPlot(iBestPlot, false);
		}
		if (eBestSpecialist != NO_SPECIALIST)
		{
			changeSpecialistCount(eBestSpecialist, -1);
		}

		if (iBestPotentialPlotValue > iWorstWorkedPlotValue)
		{
			iWorkersNeeded += 2;
		}
	}

	iWorkersNeeded += (std::max(0, iUnimprovedWorkedPlotCount - 1) * (GET_PLAYER(getOwner()).getCurrentEra())) / 3;

	if (GET_PLAYER(getOwner()).AI_isFinancialTrouble())
	{
		iWorkersNeeded *= 3;
		iWorkersNeeded /= 2;
	}

	if (iWorkersNeeded > 0)
	{
		iWorkersNeeded++;
		iWorkersNeeded = std::max(1, iWorkersNeeded / 3);
	}

	const int iSpecialistExtra =
		(
			std::min(
				getSpecialistPopulation() - totalFreeSpecialists(),
				iUnimprovedUnworkedPlotCount
			)
			- iImprovedUnworkedPlotCount
			);
	iWorkersNeeded += std::max(0, 1 + iSpecialistExtra) / 2;

	if (iWorstWorkedPlotValue <= iBestUnworkedPlotValue && iBestUnworkedPlotValue >= iBestPotentialPlotValue)
	{
		iWorkersNeeded /= 2;
	}
	if (angryPopulation(1) > 0)
	{
		iWorkersNeeded++;
		iWorkersNeeded /= 2;
	}

	iWorkersNeeded += (iSpecialCount + 1) / 2;

	iWorkersNeeded = std::max((iUnimprovedWorkedPlotCount + 1) / 2, iWorkersNeeded);

	m_iWorkersNeeded = std::max(0, iWorkersNeeded - area()->getNumTrainAIUnits(getOwner(), UNITAI_WORKER));

	FASSERT_NOT_NEGATIVE(m_iWorkersNeeded);
	LOG_BBAI_CITY(3, ("Player %d, city: %S, workers have: %d, workers needed: %d", getOwner(), getName().GetCString(), getNumWorkers(), m_iWorkersNeeded));	
}


BuildingTypes CvCityAI::AI_bestAdvancedStartBuilding(int iPass)
{
	int iFocusFlags = 0;

	if (iPass >= 0)
	{
		iFocusFlags |= BUILDINGFOCUS_CULTURE;
	}
	if (iPass >= 1)
	{
		iFocusFlags |= BUILDINGFOCUS_FOOD;
	}
	if (iPass >= 2)
	{
		iFocusFlags |= BUILDINGFOCUS_PRODUCTION;
	}
	if (iPass >= 3)
	{
		iFocusFlags |= BUILDINGFOCUS_EXPERIENCE;
	}
	if (iPass >= 4)
	{
		iFocusFlags |= (BUILDINGFOCUS_HAPPY | BUILDINGFOCUS_HEALTHY);
	}
	if (iPass >= 5)
	{
		iFocusFlags |= (BUILDINGFOCUS_GOLD | BUILDINGFOCUS_RESEARCH | BUILDINGFOCUS_MAINTENANCE | BUILDINGFOCUS_ESPIONAGE);
	}
	return AI_bestBuildingThreshold(iFocusFlags, 0, std::max(0, 25 - iPass * 5));
}


void CvCityAI::read(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	CvCity::read(pStream);

	WRAPPER_READ(wrapper, "CvCityAI", &m_iEmphasizeAvoidGrowthCount);
	WRAPPER_READ(wrapper, "CvCityAI", &m_iEmphasizeGreatPeopleCount);
	WRAPPER_READ(wrapper, "CvCityAI", &m_bAssignWorkDirty);
	WRAPPER_READ(wrapper, "CvCityAI", &m_bChooseProductionDirty);

	WRAPPER_READ(wrapper, "CvCityAI", &m_iEmphasizeAvoidAngryCitizensCount);
	WRAPPER_READ(wrapper, "CvCityAI", &m_iEmphasizeAvoidUnhealthyCitizensCount);
	WRAPPER_READ(wrapper, "CvCityAI", &m_bMilitaryProductionCity);

	WRAPPER_READ(wrapper, "CvCityAI", (int*)&m_routeToCity.eOwner);
	WRAPPER_READ(wrapper, "CvCityAI", &m_routeToCity.iID);

	WRAPPER_READ_ARRAY(wrapper, "CvCityAI", NUM_YIELD_TYPES, m_aiEmphasizeYieldCount);
	WRAPPER_READ_ARRAY(wrapper, "CvCityAI", NUM_COMMERCE_TYPES, m_aiEmphasizeCommerceCount);
	WRAPPER_READ(wrapper, "CvCityAI", &m_bForceEmphasizeCulture);
	WRAPPER_READ_ARRAY(wrapper, "CvCityAI", NUM_CITY_PLOTS, m_aiBestBuildValue);
	WRAPPER_READ_CLASS_ENUM_ARRAY(wrapper, "CvCityAI", REMAPPED_CLASS_TYPE_BUILDS, NUM_CITY_PLOTS, (int*)m_aeBestBuild);
	WRAPPER_READ_ARRAY(wrapper, "CvCityAI", GC.getNumEmphasizeInfos(), m_pbEmphasize);

	WRAPPER_READ_CLASS_ARRAY_DECORATED(wrapper, "CvCityAI", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_pbEmphasizeSpecialist, "EmphasizeSpecialist");

	WRAPPER_READ_ARRAY(wrapper, "CvCityAI", NUM_YIELD_TYPES, m_aiSpecialYieldMultiplier);
	WRAPPER_READ(wrapper, "CvCityAI", &m_iCachePlayerClosenessTurn);
	WRAPPER_READ(wrapper, "CvCityAI", &m_iCachePlayerClosenessDistance);
	WRAPPER_READ_ARRAY(wrapper, "CvCityAI", MAX_PLAYERS, m_aiPlayerCloseness);
	WRAPPER_READ(wrapper, "CvCityAI", &m_iNeededFloatingDefenders);
	WRAPPER_READ(wrapper, "CvCityAI", &m_iNeededFloatingDefendersCacheTurn);
	WRAPPER_READ(wrapper, "CvCityAI", &m_iWorkersNeeded);

	WRAPPER_READ(wrapper, "CvCityAI", &m_iTempBuildPriority);
	WRAPPER_READ(wrapper, "CvCityAI", &m_iBuildPriority);
	WRAPPER_READ(wrapper, "CvCityAI", &m_bNavalMilitaryProductionCity);
	WRAPPER_READ(wrapper, "CvCityAI", &m_iUnitSummonedLastEstablishedTurn);

	WRAPPER_READ_OBJECT_END(wrapper);
}


void CvCityAI::write(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	CvCity::write(pStream);

	WRAPPER_WRITE(wrapper, "CvCityAI", m_iEmphasizeAvoidGrowthCount);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iEmphasizeGreatPeopleCount);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_bAssignWorkDirty);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_bChooseProductionDirty);

	WRAPPER_WRITE(wrapper, "CvCityAI", m_iEmphasizeAvoidAngryCitizensCount);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iEmphasizeAvoidUnhealthyCitizensCount);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_bMilitaryProductionCity);

	WRAPPER_WRITE(wrapper, "CvCityAI", m_routeToCity.eOwner);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_routeToCity.iID);

	WRAPPER_WRITE_ARRAY(wrapper, "CvCityAI", NUM_YIELD_TYPES, m_aiEmphasizeYieldCount);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCityAI", NUM_COMMERCE_TYPES, m_aiEmphasizeCommerceCount);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_bForceEmphasizeCulture);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCityAI", NUM_CITY_PLOTS, m_aiBestBuildValue);
	WRAPPER_WRITE_CLASS_ENUM_ARRAY(wrapper, "CvCityAI", REMAPPED_CLASS_TYPE_BUILDS, NUM_CITY_PLOTS, (int*)m_aeBestBuild);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCityAI", GC.getNumEmphasizeInfos(), m_pbEmphasize);
	WRAPPER_WRITE_CLASS_ARRAY_DECORATED(wrapper, "CvCityAI", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_pbEmphasizeSpecialist, "EmphasizeSpecialist");
	WRAPPER_WRITE_ARRAY(wrapper, "CvCityAI", NUM_YIELD_TYPES, m_aiSpecialYieldMultiplier);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iCachePlayerClosenessTurn);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iCachePlayerClosenessDistance);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCityAI", MAX_PLAYERS, m_aiPlayerCloseness);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iNeededFloatingDefenders);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iNeededFloatingDefendersCacheTurn);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iWorkersNeeded);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iTempBuildPriority);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iBuildPriority);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_bNavalMilitaryProductionCity);
	WRAPPER_WRITE(wrapper, "CvCityAI", m_iUnitSummonedLastEstablishedTurn);

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

/************************************************************************************************/
/* RevDCM                  Start		 5/2/09                                                 */
/*                                                                                              */
/* Inquisitions                                                                                 */
/************************************************************************************************/
bool CvCityAI::AI_trainInquisitor()
{
	PROFILE_EXTRA_FUNC();
	const CvPlayerAI& kPlayerAI = GET_PLAYER(getOwner());

	if (!GC.getGame().isOption(GAMEOPTION_RELIGION_INQUISITIONS)
		|| !kPlayerAI.isInquisitionConditions())
	{
		return false;
	}
	if (!kPlayerAI.hasInquisitionTarget())
	{
		return false;
	}
	if (getPopulation() < GC.getMIN_POP_CONSIDER_INQUISITORS())
	{
		return false;
	}

	int iUnitCost = MAX_INT;
	UnitTypes eBestUnit = NO_UNIT;

	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		if (GC.getUnitInfo((UnitTypes)iI).isInquisitor()
			&& GC.getUnitInfo((UnitTypes)iI).getProductionCost() < iUnitCost
			&& canTrain((UnitTypes)iI, false, false))
		{
			eBestUnit = (UnitTypes)iI;
			iUnitCost = GC.getUnitInfo(eBestUnit).getProductionCost();
		}
	}
	if (eBestUnit == NO_UNIT)
	{
		return false;
	}

	int iBuildOdds = 0;
	if (kPlayerAI.isPushReligiousVictory())
	{
		iBuildOdds += 30;
	}
	else if (kPlayerAI.isConsiderReligiousVictory())
	{
		iBuildOdds += 7;
	}
	if (kPlayerAI.countCityReligionRevolts() > 3)
	{
		iBuildOdds += kPlayerAI.countCityReligionRevolts() * 20;
	}
	else
	{
		iBuildOdds += kPlayerAI.countCityReligionRevolts() * 10;
	}
	if (kPlayerAI.isNonStateReligionCommerce())
	{
		iBuildOdds /= 2;
	}

	iBuildOdds -= 3;
	if (iBuildOdds > 0)
	{
		if (GC.getGame().getSorenRandNum(100, "AI choose Inquisitor") < iBuildOdds)
		{
			pushOrder(ORDER_TRAIN, eBestUnit, -1, false, false, true);
			return true;
		}
	}
	return false;
}
/************************************************************************************************/
/* Inquisitions	                     END                                                        */
/************************************************************************************************/

int CvCityAI::AI_getEmphasizeAvoidAngryCitizensCount() const
{
	return m_iEmphasizeAvoidAngryCitizensCount;
}

bool CvCityAI::AI_isEmphasizeAvoidAngryCitizens() const
{
	return (AI_getEmphasizeAvoidAngryCitizensCount() > 0);
}

int CvCityAI::AI_getEmphasizeAvoidUnhealthyCitizensCount() const
{
	return m_iEmphasizeAvoidUnhealthyCitizensCount;
}

bool CvCityAI::AI_isEmphasizeAvoidUnhealthyCitizens() const
{
	return (AI_getEmphasizeAvoidUnhealthyCitizensCount() > 0);
}

//	Koshling  - this method is unused!!
bool CvCityAI::AI_buildCaravan()
{
	PROFILE_EXTRA_FUNC();
	FErrorMsg("error");

	int iAveProduction = 0;
	foreach_(const CvCity * pLoopCity, GET_PLAYER(getOwner()).cities())
	{
		iAveProduction += pLoopCity->getYieldRate(YIELD_PRODUCTION);
	}
	int iNumCities = GET_PLAYER(getOwner()).getNumCities();
	iAveProduction /= iNumCities;

	UnitTypes eBestUnit = NO_UNIT;
	if (((2 * getYieldRate(YIELD_PRODUCTION)) / 3) > iAveProduction)
	{
		int iBestHurry = 0;
		for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			if (GC.getUnitInfo((UnitTypes)iI).getBaseHurry() > iBestHurry)
			{
				if (canTrain((UnitTypes)iI, false, true, false))
				{
					iBestHurry = GC.getUnitInfo((UnitTypes)iI).getBaseHurry();
					eBestUnit = (UnitTypes)iI;
				}
			}
		}
		if (eBestUnit != NO_UNIT)
		{
			int iOdds = 100;
			if (GC.getGame().isOption(GAMEOPTION_AI_AGGRESSIVE))
				iOdds *= 2;
			iOdds /= iNumCities;
			iOdds *= iAveProduction;
			iOdds /= std::max(1, getYieldRate(YIELD_PRODUCTION));
			iOdds = std::max(1, iOdds);
			if (GC.getGame().getSorenRandNum(iOdds, "Caravan Production") == 0)
			{
				pushOrder(ORDER_TRAIN, eBestUnit, -1, false, false, true);
				//logging::logMsg("C2C.log", "City %S built a caravan", getName().GetCString());
				return true;
			}
		}
	}
	return false;
}

int CvCityAI::AI_getPromotionValue(PromotionTypes ePromotion) const
{
	PROFILE_EXTRA_FUNC();
	int iCanTrainCount = 0;
	int iValue = 0;
	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		const CvUnitInfo& kUnit = GC.getUnitInfo((UnitTypes)iI);

		if (kUnit.getUnitCombatType() != NO_UNITCOMBAT)
		{
			bool bUnitCanGetPromotion = false;

			if (GC.getPromotionInfo(ePromotion).getUnitCombat(kUnit.getUnitCombatType()))
			{
				bUnitCanGetPromotion = true;
			}
			else
			{
				//TB SubCombat Mod Begin
				foreach_(const UnitCombatTypes eSubCombat, kUnit.getSubCombatTypes())
				{
					if (GC.getPromotionInfo(ePromotion).getUnitCombat(eSubCombat))
					{
						bUnitCanGetPromotion = true;
						break;
					}
				}
			}
			//TB SubCombat Mod End

			if (canTrain((UnitTypes)iI))
			{
				iCanTrainCount++;
				if (bUnitCanGetPromotion)
				{
					iValue += GET_PLAYER(getOwner()).AI_promotionValue(ePromotion, (UnitTypes)iI);
				}
			}
		}
	}
	if (iValue == 0)
	{
		return 0; //Avoid division by 0 when the city can't train units or the promotion never applies.
	}
	if (iValue > 0)
	{
		iValue = iValue / iCanTrainCount + 1;
	}
	else
	{
		iValue = iValue / iCanTrainCount - 1;
	}

	if (iValue > 0 && GET_TEAM(getTeam()).hasWarPlan(true))
	{
		iValue *= 2;
	}
	return iValue;
}

SpecialistTypes CvCity::getBestSpecialist(int iExtra) const
{
	PROFILE_EXTRA_FUNC();
	int iBestValue = 0;
	SpecialistTypes eBestSpecialist = NO_SPECIALIST;
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		if (isSpecialistValid((SpecialistTypes)iI, iExtra))
		{
			const int iValue = AI_specialistValue((SpecialistTypes)iI, false, false);
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestSpecialist = (SpecialistTypes)iI;
			}
		}
	}

	return eBestSpecialist;
}

int CvCityAI::AI_calculateActualImprovementHealth(ImprovementTypes eImprovement) const
{
	PROFILE_EXTRA_FUNC();
	if (eImprovement == NO_IMPROVEMENT)
	{
		return 0;
	}
	const CvPlayer& player = GET_PLAYER(getOwner());

	int iHealthPercent = GC.getImprovementInfo(eImprovement).getHealthPercent();

	for (int i = 0; i < GC.getNumCivicOptionInfos(); i++)
	{
		if (player.getCivics((CivicOptionTypes)i) != NO_CIVIC)
		{
			iHealthPercent += GC.getCivicInfo(player.getCivics((CivicOptionTypes)i)).getImprovementHealthPercentChanges(eImprovement);
		}
	}
	// Toffer - Double rounding error is on purpose here, don't merge the two divisions into one.
	// e.g. this would be wrong: (iBadHealthPercent + iHealthPercent - iBadHealthPercent) / 100
	if (iHealthPercent < 0)
	{
		const int iBadHealthPercent = getImprovementBadHealth();
		return (iBadHealthPercent + iHealthPercent) / 100 - iBadHealthPercent / 100;
	}
	else if (iHealthPercent > 0)
	{
		const int iGoodHealthPercent = getImprovementGoodHealth();
		return (iGoodHealthPercent + iHealthPercent) / 100 - iGoodHealthPercent / 100;
	}
	return 0;
}

bool CvCityAI::AI_isMilitaryProductionCity() const
{
	return m_bMilitaryProductionCity;
}

void CvCityAI::AI_setMilitaryProductionCity(bool bNewVal)
{
	if (bNewVal != AI_isMilitaryProductionCity())
	{
		m_bMilitaryProductionCity = bNewVal;
		AI_setChooseProductionDirty(true);
	}
}

int CvCityAI::AI_getMilitaryProductionRateRank() const
{
	PROFILE_FUNC();

	int iRate = getPopulation() + getYieldRate(YIELD_PRODUCTION) - getYieldRate(YIELD_COMMERCE);

	for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		if ((DomainTypes)iI != DOMAIN_SEA)
		{
			iRate += getDomainProductionModifier((DomainTypes)iI) / 10;
			iRate += getDomainFreeExperience((DomainTypes)iI);
		}
	}
	iRate += getProductionModifier() / 10;
	iRate += getFreeExperience();

	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		if (GC.getUnitCombatInfo((UnitCombatTypes)iI).isForMilitary())
		{
			iRate += getUnitCombatProductionModifier((UnitCombatTypes)iI) / 10;
			iRate += getUnitCombatFreeExperience((UnitCombatTypes)iI);
		}
	}
	int iRank = 1;

	foreach_(const CvCity * pLoopCity, GET_PLAYER(getOwner()).cities())
	{
		const int iLoopRate = pLoopCity->getPopulation() + pLoopCity->getYieldRate(YIELD_PRODUCTION) - pLoopCity->getYieldRate(YIELD_COMMERCE);
		if (iLoopRate > iRate || (iLoopRate == iRate && pLoopCity->getID() < getID()))
		{
			iRank++;
		}
	}
	return iRank;
}

bool CvCityAI::AI_isNavalMilitaryProductionCity() const
{
	return m_bNavalMilitaryProductionCity;
}

void CvCityAI::AI_setNavalMilitaryProductionCity(bool bNewVal)
{
	if (bNewVal != AI_isNavalMilitaryProductionCity())
	{
		m_bNavalMilitaryProductionCity = bNewVal;
		AI_setChooseProductionDirty(true);
	}
}

int CvCityAI::AI_getNavalMilitaryProductionRateRank() const
{
	PROFILE_FUNC();

	if (!isCoastal(8))
	{
		return 0;
	}
	int iRate = getPopulation() + getYieldRate(YIELD_PRODUCTION) - getYieldRate(YIELD_COMMERCE);

	iRate += getDomainProductionModifier(DOMAIN_SEA) / 10;
	iRate += getDomainFreeExperience(DOMAIN_SEA);

	iRate += getProductionModifier() / 10;
	iRate += getFreeExperience();

	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		if (GC.getUnitCombatInfo((UnitCombatTypes)iI).isForNavalMilitary())
		{
			iRate += getUnitCombatProductionModifier((UnitCombatTypes)iI) / 10;
			iRate += getUnitCombatFreeExperience((UnitCombatTypes)iI);
		}
	}
	int iRank = 1;

	foreach_(const CvCity * pLoopCity, GET_PLAYER(getOwner()).cities())
	{
		const int iLoopRate = pLoopCity->getPopulation() + pLoopCity->getYieldRate(YIELD_PRODUCTION) - pLoopCity->getYieldRate(YIELD_COMMERCE);
		if (iLoopRate > iRate || (iLoopRate == iRate && pLoopCity->getID() < getID()))
		{
			iRank++;
		}
	}
	return iRank;
}


static bool buildingHasTradeRouteValue(BuildingTypes eBuilding)
{
	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	return (kBuilding.getTradeRoutes() > 0 ||
		kBuilding.getCoastalTradeRoutes() > 0 ||
		kBuilding.getGlobalTradeRoutes() > 0 ||
		kBuilding.getTradeRouteModifier() > 0 ||
		kBuilding.getForeignTradeRouteModifier() > 0);
}

bool CvCityAI::buildingMayHaveAnyValue(BuildingTypes eBuilding, int iFocusFlags) const
{
	PROFILE_FUNC();

	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	if (kBuilding.isAutoBuild() || kBuilding.getProductionCost() <= 0)
	{
		return false;
	}

	if ((iFocusFlags & BUILDINGFOCUS_CONSIDER_ANY) != 0)
	{
		return true;
	}

	if ((iFocusFlags & ~BUILDINGFOCUS_WONDEROK) == BUILDINGFOCUS_WORLDWONDER)
	{
		return isLimitedWonder(eBuilding);
	}

	if ((iFocusFlags & BUILDINGFOCUS_WONDEROK) != 0 && isLimitedWonder(eBuilding))
	{
		return true;
	}

	if ((iFocusFlags & BUILDINGFOCUS_BIGCULTURE) != 0)
	{
		iFocusFlags |= BUILDINGFOCUS_CULTURE;
	}

	//	Some things cause us to consider the building in any role
	if (kBuilding.getFreeSpecialist() > 0 ||
		kBuilding.getAreaFreeSpecialist() > 0 ||
		kBuilding.getGlobalFreeSpecialist() > 0 ||
		(kBuilding.getPropertyManipulators() != NULL && kBuilding.getPropertyManipulators()->getNumSources() > 0) ||
		kBuilding.getProperties()->getNumProperties() > 0)
	{
		return true;
	}
	if (kBuilding.isPower() || (kBuilding.getPowerBonus() != NO_BONUS && hasBonus((BonusTypes)(kBuilding.getPowerBonus()))))
	{
		return true;
	}
	if (kBuilding.getFoundsCorporation() != NO_CORPORATION || kBuilding.getGlobalReligionCommerce() > 0 || kBuilding.getGlobalCorporationCommerce() > 0)
	{
		return true;
	}

	const bool buildingModifiesGenericYields =
		(
			kBuilding.getBonusYieldModifier(NO_BONUS, NO_COMMERCE) > 0 ||
			kBuilding.getBonusYieldChanges(NO_BONUS, NO_COMMERCE) > 0 ||
			kBuilding.getVicinityBonusYieldChanges(NO_BONUS, NO_COMMERCE) > 0
			);

	const bool buildingModifiesCommerceYields =
		(
			buildingModifiesGenericYields
			|| kBuilding.getYieldModifier(YIELD_COMMERCE) > 0
			|| GET_TEAM(getTeam()).getBuildingYieldTechModifier(YIELD_COMMERCE, eBuilding) > 0
			|| kBuilding.getPowerYieldModifier(YIELD_COMMERCE) > 0
			|| kBuilding.getRiverPlotYieldChange(YIELD_COMMERCE) > 0
			|| getBaseYieldRateFromBuilding100(YIELD_COMMERCE, eBuilding) > 0
			);

	const bool bHasTradeRouteValue = buildingHasTradeRouteValue(eBuilding);

	if ((iFocusFlags & BUILDINGFOCUS_FOOD) != 0)
	{
		if (buildingModifiesGenericYields
			|| bHasTradeRouteValue
			|| kBuilding.getFoodKept() > 0
			|| GET_TEAM(getTeam()).getBuildingYieldTechModifier(YIELD_FOOD, eBuilding) > 0
			|| kBuilding.getYieldModifier(YIELD_FOOD) > 0
			|| kBuilding.getRiverPlotYieldChange(YIELD_FOOD) > 0
			|| getBaseYieldRateFromBuilding100(YIELD_FOOD, eBuilding) > 0)
		{
			return true;
		}
	}
	if ((iFocusFlags & BUILDINGFOCUS_PRODUCTION) != 0)
	{
		if (buildingModifiesGenericYields
			|| bHasTradeRouteValue
			|| kBuilding.getYieldModifier(YIELD_PRODUCTION) > 0
			|| GET_TEAM(getTeam()).getBuildingYieldTechModifier(YIELD_PRODUCTION, eBuilding) > 0
			|| kBuilding.getPowerYieldModifier(YIELD_PRODUCTION) > 0
			|| kBuilding.getRiverPlotYieldChange(YIELD_PRODUCTION) > 0
			|| kBuilding.getHurryCostModifier() < 0
			|| kBuilding.getMilitaryProductionModifier() > 0
			|| kBuilding.getSpaceProductionModifier() > 0
			|| kBuilding.getGlobalSpaceProductionModifier() > 0
			|| kBuilding.getDomainProductionModifier(NO_DOMAIN) > 0
			|| kBuilding.getNumUnitCombatProdModifiers() > 0
			|| getBaseYieldRateFromBuilding100(YIELD_PRODUCTION, eBuilding) > 0)
		{
			return true;
		}
	}
	if ((iFocusFlags & BUILDINGFOCUS_GOLD) != 0)
	{
		if (buildingModifiesCommerceYields ||
			bHasTradeRouteValue ||
			kBuilding.getCommerceChange(COMMERCE_GOLD) > 0 ||
			kBuilding.getCommercePerPopChange(COMMERCE_GOLD) > 0 ||
			kBuilding.getCommerceModifier(COMMERCE_GOLD) > 0 ||
			kBuilding.getGlobalCommerceModifier(COMMERCE_GOLD) > 0 ||
			kBuilding.getSpecialistExtraCommerce(COMMERCE_GOLD) > 0 ||
			kBuilding.getStateReligionCommerce(COMMERCE_GOLD) > 0 ||
			GET_TEAM(getTeam()).getBuildingCommerceTechModifier(COMMERCE_GOLD, eBuilding) > 0)
		{
			return true;
		}
		iFocusFlags |= BUILDINGFOCUS_MAINTENANCE;
	}
	if ((iFocusFlags & BUILDINGFOCUS_RESEARCH) != 0)
	{
		if (buildingModifiesCommerceYields ||
			bHasTradeRouteValue ||
			kBuilding.getCommerceChange(COMMERCE_RESEARCH) > 0 ||
			kBuilding.getCommercePerPopChange(COMMERCE_RESEARCH) > 0 ||
			kBuilding.getCommerceModifier(COMMERCE_RESEARCH) > 0 ||
			kBuilding.getGlobalCommerceModifier(COMMERCE_RESEARCH) > 0 ||
			kBuilding.getSpecialistExtraCommerce(COMMERCE_RESEARCH) > 0 ||
			kBuilding.getStateReligionCommerce(COMMERCE_RESEARCH) > 0 ||
			GET_TEAM(getTeam()).getBuildingCommerceTechModifier(COMMERCE_RESEARCH, eBuilding) > 0)
		{
			return true;
		}
	}
	if ((iFocusFlags & BUILDINGFOCUS_CULTURE) != 0)
	{
		if (buildingModifiesCommerceYields ||
			bHasTradeRouteValue ||
			kBuilding.getCommerceChange(COMMERCE_CULTURE) > 0 ||
			kBuilding.getCommercePerPopChange(COMMERCE_CULTURE) > 0 ||
			kBuilding.getCommerceModifier(COMMERCE_CULTURE) > 0 ||
			kBuilding.getGlobalCommerceModifier(COMMERCE_CULTURE) > 0 ||
			kBuilding.getSpecialistExtraCommerce(COMMERCE_CULTURE) > 0 ||
			kBuilding.getStateReligionCommerce(COMMERCE_CULTURE) > 0 ||
			GET_TEAM(getTeam()).getBuildingCommerceTechModifier(COMMERCE_CULTURE, eBuilding) > 0)
		{
			return true;
		}
	}
	if ((iFocusFlags & BUILDINGFOCUS_DEFENSE) != 0)
	{
#ifdef STRENGTH_IN_NUMBERS
		if (GC.getGame().isOption(GAMEOPTION_COMBAT_STRENGTH_IN_NUMBERS))
		{
			if (kBuilding.getFrontSupportPercentModifier() > 0
				|| kBuilding.getShortRangeSupportPercentModifier() > 0
				|| kBuilding.getMediumRangeSupportPercentModifier() > 0
				|| kBuilding.getLongRangeSupportPercentModifier() > 0
				|| kBuilding.getFlankSupportPercentModifier() > 0)
			{
				return true;
			}
		}
#endif
		if (kBuilding.getDefenseModifier() > 0 ||
			kBuilding.getBombardDefenseModifier() > 0 ||
			kBuilding.getAllCityDefenseModifier() > 0 ||
			kBuilding.isNeverCapture() ||
			kBuilding.isNukeImmune() ||
			GC.getGame().isOption(GAMEOPTION_UNSUPPORTED_ZONE_OF_CONTROL) && kBuilding.isZoneOfControl() ||
			kBuilding.getLineOfSight() > 0 ||
			!kBuilding.getUnitCombatExtraStrength().empty() ||
			kBuilding.getAdjacentDamagePercent() > 0 ||
			kBuilding.isProtectedCulture() ||
			kBuilding.getOccupationTimeModifier() > 0 ||
			kBuilding.getNoEntryDefenseLevel() > 0 ||
			kBuilding.getNumUnitFullHeal() > 0 ||
			kBuilding.isAreaBorderObstacle() ||
			GC.getGame().isOption(GAMEOPTION_COMBAT_SURROUND_DESTROY) && kBuilding.getLocalDynamicDefense() > 0 ||
			kBuilding.getLocalCaptureProbabilityModifier() > 0 ||
			kBuilding.getLocalCaptureResistanceModifier() > 0 ||
			kBuilding.getNationalCaptureResistanceModifier() > 0 ||
			kBuilding.getNumUnitCombatRepelModifiers() > 0 ||
			kBuilding.getRiverDefensePenalty() < 0 ||
			kBuilding.getLocalRepel() > 0 ||
			kBuilding.getMinDefense() > 0 ||
			kBuilding.getBuildingDefenseRecoverySpeedModifier() > 0 ||
			kBuilding.getCityDefenseRecoverySpeedModifier() > 0 ||
			kBuilding.getNumUnitCombatRepelAgainstModifiers() > 0 ||
			kBuilding.getNumUnitCombatDefenseAgainstModifiers() > 0 ||
			(kBuilding.getDamageAttackerChance() > 0 && kBuilding.getDamageToAttacker() > 0))
		{
			return true;
		}
	}
	if ((iFocusFlags & BUILDINGFOCUS_HAPPY) != 0)
	{
		foreach_(const TechModifier & modifier, kBuilding.getTechHappinessChanges())
		{
			if (GET_TEAM(getTeam()).isHasTech(modifier.first) && modifier.second > 0)
			{
				return true;
			}
		}
		if (kBuilding.getHappiness() > 0
			|| kBuilding.getAreaHappiness() > 0
			|| kBuilding.getGlobalHappiness() > 0
			|| kBuilding.getStateReligionHappiness() > 0
			|| kBuilding.isNoUnhappiness()
			|| kBuilding.getWarWearinessModifier() < 0
			|| kBuilding.getGlobalWarWearinessModifier() < 0
			|| kBuilding.getCommerceHappiness(NO_COMMERCE) > 0
			|| !kBuilding.getBonusHappinessChanges().empty()
			|| !kBuilding.getBuildingHappinessChanges().empty()
			|| GET_PLAYER(getOwner()).getExtraBuildingHappiness(eBuilding) > 0)
		{
			return true;
		}
	}
	if ((iFocusFlags & BUILDINGFOCUS_HEALTHY) != 0)
	{
		foreach_(const TechModifier & modifier, kBuilding.getTechHealthChanges())
		{
			if (GET_TEAM(getTeam()).isHasTech(modifier.first) && modifier.second > 0)
			{
				return true;
			}
		}
		if (kBuilding.getHealth() > 0
			|| kBuilding.getAreaHealth() > 0
			|| kBuilding.getGlobalHealth() > 0
			|| kBuilding.isNoUnhealthyPopulation()
			|| kBuilding.isBuildingOnlyHealthy()
			|| !kBuilding.getBonusHealthChanges().empty()
			|| kBuilding.getHealthPercentPerPopulation() > 0
			|| GET_PLAYER(getOwner()).getExtraBuildingHealth(eBuilding) > 0)
		{
			return true;
		}
	}
	if ((iFocusFlags & BUILDINGFOCUS_DOMAINSEA) != 0)
	{
		if (kBuilding.getDomainProductionModifier(DOMAIN_SEA) > 0 || kBuilding.getDomainFreeExperience(DOMAIN_SEA) > 0)
		{
			return true;
		}
		iFocusFlags |= BUILDINGFOCUS_EXPERIENCE;
	}
	if ((iFocusFlags & BUILDINGFOCUS_EXPERIENCE) != 0)
	{
		if (kBuilding.getFreeExperience() > 0 ||
			kBuilding.getGlobalFreeExperience() > 0 ||
			!kBuilding.getUnitCombatFreeExperience().empty() ||
			kBuilding.getDomainFreeExperience(NO_DOMAIN) > 0 ||
			kBuilding.isApplyFreePromotionOnMove() ||
			kBuilding.EnablesUnits() ||
			!kBuilding.getFreePromoTypes().empty() ||
			kBuilding.getNumUnitCombatRetrainTypes() > 0 ||
			kBuilding.getNationalCaptureProbabilityModifier() > 0 ||
			!kBuilding.getUnitCombatFreeExperience().empty() ||
			kBuilding.isAnyDomainFreeExperience())
		{
			return true;
		}
	}
	if ((iFocusFlags & BUILDINGFOCUS_MAINTENANCE) != 0)
	{
		if (kBuilding.getMaintenanceModifier() < 0 ||
			kBuilding.getGlobalMaintenanceModifier() < 0 ||
			kBuilding.getAreaMaintenanceModifier() < 0 ||
			kBuilding.getOtherAreaMaintenanceModifier() < 0 ||
			kBuilding.getDistanceMaintenanceModifier() < 0 ||
			kBuilding.getNumCitiesMaintenanceModifier() < 0 ||
			kBuilding.getCoastalDistanceMaintenanceModifier() < 0 ||
			kBuilding.getConnectedCityMaintenanceModifier())
		{
			return true;
		}
	}
	if ((iFocusFlags & BUILDINGFOCUS_SPECIALIST) != 0)
	{
		if (kBuilding.getSpecialistCount(NO_SPECIALIST) > 0 ||
			kBuilding.getFreeSpecialistCount(NO_SPECIALIST) > 0 ||
			!kBuilding.getImprovementFreeSpecialists().empty() ||
			kBuilding.getTechSpecialistChange(NO_TECH, NO_SPECIALIST) > 0)
		{
			return true;
		}
	}
	if ((iFocusFlags & BUILDINGFOCUS_ESPIONAGE) != 0)
	{
		if (buildingModifiesCommerceYields ||
			bHasTradeRouteValue ||
			kBuilding.getEspionageDefenseModifier() > 0 ||
			kBuilding.getCommerceChange(COMMERCE_ESPIONAGE) > 0 ||
			kBuilding.getCommercePerPopChange(COMMERCE_ESPIONAGE) > 0 ||
			kBuilding.getCommerceModifier(COMMERCE_ESPIONAGE) > 0 ||
			kBuilding.getGlobalCommerceModifier(COMMERCE_ESPIONAGE) > 0 ||
			kBuilding.getSpecialistExtraCommerce(COMMERCE_ESPIONAGE) > 0 ||
			kBuilding.getStateReligionCommerce(COMMERCE_ESPIONAGE) > 0 ||
			GET_TEAM(getTeam()).getBuildingCommerceTechModifier(COMMERCE_ESPIONAGE, eBuilding) > 0)
		{
			return true;
		}
	}
	if ((iFocusFlags & BUILDINGFOCUS_CAPITAL) != 0 && kBuilding.isCapital())
	{
		return true;
	}
	return false;
}


void CvCityAI::AI_FlushBuildingValueCache(bool bRetainValues)
{
	PROFILE_FUNC();

	if (!bRetainValues)
	{
#ifdef _DEBUG
		OutputDebugString("Flush building cache\n");
#endif
		SAFE_DELETE(cachedBuildingValues);
	}
	else if (cachedBuildingValues != NULL)
	{
		cachedBuildingValues->m_bIncomplete = true;
	}
}

void CvCityAI::CalculateAllBuildingValues(int iFocusFlags)
{
	PROFILE_FUNC();

	// KOSHLING optimisation - moved what we could outside of the building loop
	const PlayerTypes ePlayer = getOwner();
	const TeamTypes eTeam = getTeam();

	const CvPlayerAI& kOwner = GET_PLAYER(ePlayer);
	const CvTeam& team = GET_TEAM(eTeam);
	const CvArea* pArea = area();

	const ReligionTypes eStateReligion = kOwner.getStateReligion();

	const bool bMetAnyCiv = team.hasMetAnyCiv();
	const bool bAtWar = team.isAtWar();
	const bool bWarPlan = team.hasWarPlan(true);
	//const bool bCleanPower = pArea->isCleanPower(eTeam);
	const bool bDevelopingCity = isDevelopingCity();
	const bool bCapital = isCapital();
	const bool bCanPopRush = kOwner.canPopRush();
	const bool bAreaAlone = kOwner.AI_isAreaAlone(pArea);
	const bool bZOC = bAreaAlone ? false : GC.getGame().isOption(GAMEOPTION_UNSUPPORTED_ZONE_OF_CONTROL);
	const bool bSAD = bAreaAlone ? false : GC.getGame().isOption(GAMEOPTION_COMBAT_SURROUND_DESTROY);
#ifdef STRENGTH_IN_NUMBERS
	const bool bSIN = bAreaAlone ? false : GC.getGame().isOption(GAMEOPTION_COMBAT_STRENGTH_IN_NUMBERS);
#endif
	const UnitTypes eBestLandUnit = bAreaAlone ? NO_UNIT : GC.getGame().getBestLandUnit();

	const int iGoldValueAssessmentModifier = kOwner.AI_goldValueAssessmentModifier();
	const int iTotalPopulation = kOwner.getTotalPopulation();
	const int iNumCities = kOwner.getNumCities();
	const int iNumCitiesInArea = pArea->getCitiesPerPlayer(ePlayer);
	const int iHurryPercentAnger = getHurryPercentAnger();
	const int iPopulation = getPopulation();
	const int iHappyLevel = happyLevel();
	const int iCityHappy = iHappyLevel - unhappyLevel();
	const int iGoodHealth = goodHealth();
	const int iBadHealth = badHealth();
	const int iEspionageHappyCounter = getEspionageHappinessCounter();
	const int iEspionageHealthCounter = getEspionageHealthCounter();
	const int iFoodDifference = foodDifference(false);
	const int iNumTradeRoutes = getTradeRoutes();
	const int iBaseGreatPeopleRate = getBaseGreatPeopleRate();

	// Reduce reaction to espionage induced happy/health problems
	const int iHappinessLevel = iHappyLevel - unhappyLevel(1) + iEspionageHappyCounter / 2;
	const int iHealthLevel = iGoodHealth - badHealth(/*bNoAngry*/ false, std::max(0, (iHappinessLevel + 1) / 2)) + iEspionageHealthCounter / 2;
	const int iHappyModifier =
		(
			iHappinessLevel >= 10 ? 1
			:
			(iHappinessLevel <= iHealthLevel && iHappinessLevel <= 6) ? 6 : 3
			);
	const int iBaseHappinessLevel = iCityHappy + iEspionageHappyCounter;
	const int iBaseHealthLevel = iGoodHealth - iBadHealth + iEspionageHealthCounter;
	const int iAngryPopulation = range(-iHappinessLevel, 0, iPopulation + 1);
	const int iBaseFoodDifference = getYieldRate(YIELD_FOOD) - getFoodConsumedByPopulation() - std::max(0, -iHealthLevel);

	int aiYieldRank[NUM_YIELD_TYPES];
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		aiYieldRank[iI] = MAX_INT;
	}
	aiYieldRank[YIELD_PRODUCTION] = findBaseYieldRateRank(YIELD_PRODUCTION);
	const bool bIsHighProductionCity = aiYieldRank[YIELD_PRODUCTION] <= std::max(3, iNumCities / 2);

	int aiCommerceRank[NUM_COMMERCE_TYPES];
	int aiBaseCommerceRate[NUM_COMMERCE_TYPES];
	int aiPlayerCommerceRate[NUM_COMMERCE_TYPES];

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		aiCommerceRank[iI] = MAX_INT;
		aiBaseCommerceRate[iI] = MAX_INT;
		aiPlayerCommerceRate[iI] = kOwner.getTotalCityBaseCommerceRate((CommerceTypes)iI);
	}
	const int iCultureRank = findCommerceRateRank(COMMERCE_CULTURE);
	aiCommerceRank[COMMERCE_CULTURE] = iCultureRank;

	const int iCulturalVictoryNumCultureCities = GC.getGame().culturalVictoryNumCultureCities();
	const int iAllowedShrinkRate =
		(
			getFoodConsumedPerPopulation100() *
			(
				std::max(0, -iBaseHappinessLevel - getAngerPercent() * iPopulation / GC.getPERCENT_ANGER_DIVISOR())
				+
				std::min(1, std::max(0, (getWorkingPopulation() - AI_countGoodTiles(true, false, 50))))
				+
				std::max(0, (visiblePopulation() - AI_countGoodSpecialists(false)))
				)
			/ 100
			);
	const bool bFinancialTrouble = kOwner.AI_isFinancialTrouble();
	const bool bCulturalVictory1 = kOwner.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE1);
	const bool bCulturalVictory2 = kOwner.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2);
	const bool bCulturalVictory3 = kOwner.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3);
	//const bool bSpaceVictory1 = kOwner.AI_isDoVictoryStrategy(AI_VICTORY_SPACE1);
	const int iVoteValueFactor = kOwner.AI_isDoVictoryStrategy(AI_VICTORY_DIPLOMACY1) ? 5 : 1;

	const bool bDefense = pArea->getAreaAIType(eTeam) == AREAAI_DEFENSIVE;
	const bool bLandWar = bDefense || pArea->getAreaAIType(eTeam) == AREAAI_OFFENSIVE || pArea->getAreaAIType(eTeam) == AREAAI_MASSING;
	const bool bDanger = AI_isDanger();

	LOG_EVALAI_CITY(3, (1, 
			"      City %S CalculateAllBuildingValues for flags %08lx (already has %08lx)",
			getName().GetCString(), iFocusFlags, cachedBuildingValues->m_iCachedFlags
		));
	// Either wonder flag forces all wonders to be calculated, so we can mark them both off as done.
	if ((iFocusFlags & (BUILDINGFOCUS_WORLDWONDER | BUILDINGFOCUS_WONDEROK)) != 0)
	{
		iFocusFlags |= (BUILDINGFOCUS_WORLDWONDER | BUILDINGFOCUS_WONDEROK);
	}

	cachedBuildingValues->m_iCachedFlags |= iFocusFlags;

	std::set<BuildingTypes> buildingsToCalculate;
	const int iNumBuildings = GC.getNumBuildingInfos();
	{
		PROFILE("CvCityAI::CalculateAllBuildingValues.PreLoop");
		for (int iBuilding = 0; iBuilding < iNumBuildings; iBuilding++)
		{
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(iBuilding);
			if (!canConstruct(eBuilding))
			{
				continue;
			}
			buildingsToCalculate.insert(eBuilding);
			const CvBuildingInfo& building = GC.getBuildingInfo(eBuilding);

			if (building.EnablesOtherBuildings())
			{
				const CvGameObjectCity* pObject = getGameObject();

				// add the extra building and its bonuses to the override to see if they influence the construct condition of this building
				std::vector<GOMOverride> queries;
				GOMOverride query = { pObject, GOM_BUILDING, eBuilding, true };
				queries.push_back(query);
				query.GOM = GOM_BONUS;
				foreach_(const BonusModifier& pair, GC.getBuildingInfo(eBuilding).getFreeBonuses())
				{
					query.id = pair.first;
					queries.push_back(query);
				}

				for (int iJ = 0; iJ < iNumBuildings; iJ++)
				{
					const BuildingTypes eType = static_cast<BuildingTypes>(iJ);
					if (algo::none_of_equal(buildingsToCalculate, eType) && !hasBuilding(eType))
					{
						// check if this building enables the construct condition of another building
						bool bEnablesCondition = GC.getBuildingInfo(eType).isPrereqInCityBuilding(iBuilding) || GC.getBuildingInfo(eType).isPrereqOrBuilding(iBuilding);
						if (!bEnablesCondition)
						{
							const BoolExpr* condition = GC.getBuildingInfo(eType).getConstructCondition();
							bEnablesCondition = condition != NULL && condition->evaluateChange(pObject, queries) == BOOLEXPR_CHANGE_BECOMES_TRUE;
						}
						if (bEnablesCondition && canConstructInternal(eType, false, false, false, true, eBuilding))
						{
							buildingsToCalculate.insert(eType);
						}
					}
				}
			}
		}
	}
	{
		PROFILE("CvCityAI::CalculateAllBuildingValues.Loop");
		// Calculate all possible (by focus) values for each building type
		foreach_(const BuildingTypes & eBuilding, buildingsToCalculate)
		{
			if (NO_BUILDING == eBuilding || cachedBuildingValues->HasValues(eBuilding) || !buildingMayHaveAnyValue(eBuilding, iFocusFlags))
			{
				continue;
			}
			OneBuildingValueCache* valuesCache = cachedBuildingValues->GetOneBuildingCache(eBuilding);

			// What follows is essentially a copy of the code in AI_buildingValueThreshold with the
			// modification that it accumulates all focus values to the cache's focus value array

			PROFILE("CvCityAI::CalculateAllBuildingValues.building");

			const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

			LOG_BBAI_CITY(4, ("          Calc value for %S", kBuilding.getDescription()));
			if (kBuilding.isCapital()) continue; // Perhaps the palace should have value...

			// Don't consider a building if it causes the city to immediately start shrinking from unhealthiness
			// For that purpose ignore bad health and unhappiness from Espionage.
			int iBuildingActualHealth = getAdditionalHealthByBuilding(eBuilding);

			const int iUnhealthyPopulationFromBuilding = std::min(0, -iBaseHealthLevel) + std::max(0, -iBuildingActualHealth);

			// Allow a bit of shrinking: Population is expendable if angry, working a bad tile, or running a not-so-good specialist
			if (iUnhealthyPopulationFromBuilding > 0 && iBaseFoodDifference + iAllowedShrinkRate < iUnhealthyPopulationFromBuilding)
			{
				continue;
			}

			bool bSkipBuilding = false;

			for (int iI = 0; iI < kBuilding.getNumReplacementBuilding(); ++iI)
			{
				if (canConstruct((BuildingTypes)kBuilding.getReplacementBuilding(iI)))
				{
					bSkipBuilding = true;
					break;
				}
			}
			if (bSkipBuilding) continue;

			foreach_(const ReligionModifier & pair, kBuilding.getReligionChanges())
			{
				if (!team.hasHolyCity(pair.first))
				{
					bSkipBuilding = true;
					break;
				}
			}
			if (bSkipBuilding) continue;

			int* paiFreeSpecialistCommerce = new int[NUM_COMMERCE_TYPES];
			int* paiFreeSpecialistYield = new int[NUM_YIELD_TYPES];

			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				paiFreeSpecialistYield[iI] = 0;
			}
			for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
			{
				paiFreeSpecialistCommerce[iI] = 0;
			}
			int iSpecialistGreatPeopleRate = 0;
			int iSpecialistExtraHealth = 0;
			int iSpecialistExtraHappy = 0;

			if (kBuilding.getNumPopulationEmployed() > 0)
			{
				removeWorstCitizenActualEffects(kBuilding.getNumPopulationEmployed(), iSpecialistGreatPeopleRate, iSpecialistExtraHappy, iSpecialistExtraHealth, paiFreeSpecialistYield, paiFreeSpecialistCommerce);
			}
			int aiFreeSpecialistYield[NUM_YIELD_TYPES];
			int aiFreeSpecialistCommerce[NUM_COMMERCE_TYPES];

			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				aiFreeSpecialistYield[iI] = paiFreeSpecialistYield[iI];
			}
			for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
			{
				aiFreeSpecialistCommerce[iI] = paiFreeSpecialistCommerce[iI];
			}
			SAFE_DELETE_ARRAY(paiFreeSpecialistCommerce);
			SAFE_DELETE_ARRAY(paiFreeSpecialistYield);

			for (int iI = 1; iI < kBuilding.getFreeSpecialist() + 1; iI++)
			{
				const SpecialistTypes eNewSpecialist = getBestSpecialist(iI);
				if (eNewSpecialist == NO_SPECIALIST) break;

				for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					aiFreeSpecialistYield[iJ] += kOwner.specialistYield(eNewSpecialist, (YieldTypes)iJ);
				}
				for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
				{
					aiFreeSpecialistCommerce[iJ] += kOwner.specialistCommerce(eNewSpecialist, (CommerceTypes)iJ);
				}
				const CvSpecialistInfo& kSpecialist = GC.getSpecialistInfo(eNewSpecialist);
				iSpecialistGreatPeopleRate += kSpecialist.getGreatPeopleRateChange();
				iSpecialistExtraHealth += kSpecialist.getHealthPercent();
				iSpecialistExtraHappy += kSpecialist.getHappinessPercent();
			}
			iBuildingActualHealth += iSpecialistExtraHealth / 100;
			const int iBuildingActualHappiness = getAdditionalHappinessByBuilding(eBuilding) + iSpecialistExtraHappy / 100;
			const int iLimitedWonderLimit = limitedWonderLimit(eBuilding);
			const bool bIsLimitedWonder = iLimitedWonderLimit >= 0;

			if (isWorldWonder(eBuilding) && aiYieldRank[YIELD_PRODUCTION] <= 3)
			{
				valuesCache->Accumulate(BUILDINGFOCUSINDEX_WORLDWONDER, 10);
			}
			{
				PROFILE("CalculateAllBuildingValues.Defense");
				int iValue = 0;

				if (!bAreaAlone)
				{
					if (eBestLandUnit == NO_UNIT || !GC.getUnitInfo(eBestLandUnit).isIgnoreBuildingDefense())
					{
						iValue += std::max(0, std::min(kBuilding.getDefenseModifier() + getBuildingDefense() - getNaturalDefense() - 10, kBuilding.getDefenseModifier())) / 4;

						for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
						{
							if (hasBonus((BonusTypes)iI))
							{
								iValue += kBuilding.getBonusDefenseChanges(iI) / 4;
							}
						}
					}
					if (bZOC)
					{
						iValue += kBuilding.isZoneOfControl() ? 50 : 0;
					}
					iValue += kBuilding.getLineOfSight() * 15;

					foreach_(const UnitCombatModifier2 & modifier, kBuilding.getUnitCombatExtraStrength())
					{
						foreach_(const CvUnit * pLoopUnit, plot()->units())
						{
							if (pLoopUnit->getTeam() == eTeam)
							{
								if (pLoopUnit->getUnitCombatType() == modifier.first
									//TB - May cause some unexpected imbalance though it could also imbalance to bypass... a place to watch
									|| algo::any_of_equal(pLoopUnit->getUnitInfo().getSubCombatTypes(), modifier.first))
								{
									iValue += modifier.second / 6;
								}
							}
						}
					}

					if (bDanger || bLandWar)
					{
						iValue += kBuilding.getAdjacentDamagePercent() * 10;
					}

					iValue += kBuilding.isProtectedCulture() ? 50 : 0;
					iValue += kBuilding.getOccupationTimeModifier() / 20;

					if (kBuilding.getNoEntryDefenseLevel() > 0 && kBuilding.getNoEntryDefenseLevel() < getTotalDefense(false))
					{
						iValue += getTotalDefense(false) - kBuilding.getNoEntryDefenseLevel() / 2;
					}
					if (kBuilding.getNumUnitFullHeal() > 0)
					{
						iValue += kBuilding.getNumUnitFullHeal() * 50;
					}

					iValue += kBuilding.getBombardDefenseModifier() / 4;

					if (bSAD)
					{
						iValue += kBuilding.getLocalDynamicDefense() / 2;
					}
#ifdef STRENGTH_IN_NUMBERS
					if (bSIN)
					{
						iValue += kBuilding.getFrontSupportPercentModifier() / 4;
						iValue += kBuilding.getShortRangeSupportPercentModifier() / 4;
						iValue += kBuilding.getMediumRangeSupportPercentModifier() / 4;
						iValue += kBuilding.getLongRangeSupportPercentModifier() / 4;
						iValue += kBuilding.getFlankSupportPercentModifier() / 4;
					}
#endif
					iValue += kBuilding.getLocalCaptureProbabilityModifier() / 6;
					iValue += kBuilding.getLocalCaptureResistanceModifier() / 3;
					iValue -= kBuilding.getRiverDefensePenalty() / 2;
					iValue += kBuilding.getLocalRepel();
					iValue += kBuilding.getMinDefense();
					iValue += kBuilding.getBuildingDefenseRecoverySpeedModifier() / 20;
					iValue += kBuilding.getCityDefenseRecoverySpeedModifier() / 5;
				}
				iValue += kBuilding.getNationalCaptureResistanceModifier();

				//TB Note: Once we have improved methods of evaluating the value of a given combat class and an ability to evaluate the strength
				//of a given combat class as it exists in volume among perceived or real enemies, we need to update the following to something
				//more intricate and accurate.  These are just semi-sufficient patches 'for now'.
				for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
				{
					iValue += kBuilding.getUnitCombatRepelModifier(iI) / 2;
					iValue += kBuilding.getUnitCombatRepelAgainstModifier(iI) / 2;
					iValue += kBuilding.getUnitCombatDefenseAgainstModifier(iI) / 2;
					if (kBuilding.isMayDamageAttackingUnitCombatType(iI))
					{
						iValue += (kBuilding.getDamageAttackerChance() * kBuilding.getDamageToAttacker()) / 4;
					}
				}
				if (kBuilding.isDamageAllAttackers())
				{
					iValue += (kBuilding.getDamageAttackerChance() * kBuilding.getDamageToAttacker());
				}
				iValue -= kBuilding.getRiverDefensePenalty() / 2;
				iValue -= kBuilding.getAirModifier() / 4;
				iValue -= kBuilding.getNukeModifier() / 4;

				iValue += kBuilding.getAllCityDefenseModifier() * iNumCities / 5;
				iValue += kBuilding.getAirlift() * 25;

				if (kBuilding.isAreaBorderObstacle() && !GC.getGame().isOption(GAMEOPTION_BARBARIAN_NONE))
				{
					int areaBorderObstacleValue = iNumCitiesInArea * 3 / 2;
					//The great wall is much more valuable with more barbarian activity.
					if (GC.getGame().isOption(GAMEOPTION_BARBARIAN_RAGING))
						areaBorderObstacleValue *= 2;
					iValue += areaBorderObstacleValue;
				}
				valuesCache->Accumulate(BUILDINGFOCUSINDEX_DEFENSE, iValue);
			}
			valuesCache->Accumulate(BUILDINGFOCUSINDEX_ESPIONAGE, kBuilding.getEspionageDefenseModifier() / 8);

			if (!isNoUnhappiness())
			{
				PROFILE("CalculateAllBuildingValues.Happy");
				int iBestHappy = 0;

				for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
				{
					if (canHurryBuilding((HurryTypes)iI, eBuilding))
					{
						const int iHappyFromHurry = AI_getHappyFromHurry((HurryTypes)iI, eBuilding);
						if (iHappyFromHurry > iBestHappy)
						{
							iBestHappy = iHappyFromHurry;
						}
					}
				}
				int iValue = iBestHappy * 10;

				//Fuyu ToDo: How to handle Globe Theater national wonder?
				//For now just give massive boost if city is high food yet not one of the main production or commerce cities
				if (kBuilding.isNoUnhappiness() && bIsLimitedWonder)
				{
					iValue += iAngryPopulation * 10 + iPopulation;

					aiYieldRank[YIELD_FOOD] = findBaseYieldRateRank(YIELD_FOOD);
					aiYieldRank[YIELD_COMMERCE] = findBaseYieldRateRank(YIELD_COMMERCE);
					if (
						aiYieldRank[YIELD_FOOD] <= 2 + iLimitedWonderLimit
						&&
						(
							!isNationalWonder(eBuilding)
							|| aiYieldRank[YIELD_PRODUCTION] > 2 + iLimitedWonderLimit
							&& aiYieldRank[YIELD_COMMERCE] > 2 + iLimitedWonderLimit
							)
						) iValue += iPopulation * 5;
				}
				else iValue += happynessValue(iBuildingActualHappiness, iBaseHappinessLevel, iBaseHealthLevel);

				iValue -= kBuilding.getHurryAngerModifier() * iHurryPercentAnger / 100;

				for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
				{
					iValue += kBuilding.getCommerceHappiness(iI) * iHappyModifier / 4;
				}

				if (kBuilding.getWarWearinessModifier() != 0)
				{
					iValue -=
						(
							bAtWar
							?
							kBuilding.getWarWearinessModifier() * iHappyModifier / 16
							:
							kBuilding.getWarWearinessModifier() * iHappyModifier / 32
							);
				}

				iValue += 8 * kBuilding.getAreaHappiness() * (iNumCitiesInArea - 1);
				iValue += 8 * kBuilding.getGlobalHappiness() * (iNumCities - 1);

				int iWarWearinessPercentAnger = kOwner.getWarWearinessPercentAnger();
				int iGlobalWarWearinessModifer = kBuilding.getGlobalWarWearinessModifier();
				if (iGlobalWarWearinessModifer != 0)
				{
					iValue += (-(((iGlobalWarWearinessModifer * iWarWearinessPercentAnger / 100) / GC.getPERCENT_ANGER_DIVISOR())) * (iNumCities - 1));
					iValue -= iGlobalWarWearinessModifer * iHappyModifier / 16;
				}

				foreach_(const BuildingModifier2 & pair, kBuilding.getBuildingHappinessChanges())
				{
					iValue += (pair.second * (kOwner.getBuildingCount(pair.first) - hasBuilding(pair.first)) * 8);
				}

				if (GC.getGame().isOption(GAMEOPTION_UNSUPPORTED_REVOLUTION))
				{
					if (kBuilding.getRevIdxLocal() != 0)
					{
						int localRevIdx = getLocalRevIndex();

						//	Use the more serious of the before and after values if this building were to be built
						if (kBuilding.getRevIdxLocal() > 0)
						{
							localRevIdx += kBuilding.getRevIdxLocal();
						}
						//	Treat instability seriously as it goes up - not just linear
						const int localRevScaling = (localRevIdx < 0 ? 0 : std::min(localRevIdx * localRevIdx / 50 + localRevIdx / 2, 100));

						iValue -= (kBuilding.getRevIdxLocal() * localRevScaling) / 4;
					}
					if (kBuilding.getRevIdxNational() != 0)
					{
						iValue -= 8 * iNumCities * kBuilding.getRevIdxNational();
					}
					if (kBuilding.getRevIdxDistanceModifier() != 0 && !bCapital)
					{
						const CvCity* pCapital = kOwner.getCapitalCity();
						if (pCapital != NULL)
						{
							int iCapitalDistance = ::plotDistance(getX(), getY(), pCapital->getX(), pCapital->getY());
							const int iOldCapitalDistance = iCapitalDistance;
							iCapitalDistance *= 100 + kBuilding.getRevIdxDistanceModifier();
							iCapitalDistance /= 100;

							iValue += ((iOldCapitalDistance - iCapitalDistance) * (10 + std::max(0, getLocalRevIndex())));
						}
					}
				}
				valuesCache->Accumulate(BUILDINGFOCUSINDEX_HAPPY, iValue);
			}

			if (!isNoUnhealthyPopulation())
			{
				PROFILE("CalculateAllBuildingValues.Health");
				int iValue = 0;

				iValue += healthValue(iBuildingActualHealth, iBaseHappinessLevel - iEspionageHappyCounter / 2 + std::max(0, iBuildingActualHappiness), iBaseHealthLevel, iBaseFoodDifference);

				iValue += 4 * kBuilding.getAreaHealth() * (iNumCitiesInArea - 1);
				iValue += 4 * kBuilding.getGlobalHealth() * (iNumCities - 1);

				valuesCache->Accumulate(BUILDINGFOCUSINDEX_HEALTHY, iValue);
			}
			{ // valuesCache->Accumulate(BUILDINGFOCUSINDEX_EXPERIENCE, iValue)
				PROFILE("CalculateAllBuildingValues.Experience");
				int iValue = kBuilding.getFreeExperience() * (bMetAnyCiv ? 12 : 6);

				foreach_(const UnitCombatModifier2 & modifier, kBuilding.getUnitCombatFreeExperience())
				{
					if (canTrain(modifier.first))
					{
						iValue += modifier.second * (bMetAnyCiv ? 6 : 3);
					}
				}

				for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
				{
					if (kBuilding.isUnitCombatRetrainType((UnitCombatTypes)iI))
					{
						iValue += 20;
					}
				}

				for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
				{
					int iDomainExpValue = 6;

					if (iI == DOMAIN_SEA)
					{
						iDomainExpValue = 7;
					}
					else if (iI == DOMAIN_LAND)
					{
						iDomainExpValue = 12;
					}
					iValue += kBuilding.getDomainFreeExperience(iI) * (bMetAnyCiv ? iDomainExpValue : iDomainExpValue / 2);
				}

				int iPromoValue = 0;
				foreach_(const FreePromoTypes & freePromoType, kBuilding.getFreePromoTypes())
				{
					if (freePromoType.m_pExprFreePromotionCondition)
					{
						iPromoValue += AI_getPromotionValue(freePromoType.ePromotion);
					}
					else
					{
						iPromoValue += AI_getPromotionValue(freePromoType.ePromotion) * 3/2;
					}
				}

				if (kBuilding.isApplyFreePromotionOnMove())
				{
					iPromoValue *= 2;
				}

				if (iFocusFlags & BUILDINGFOCUS_EXPERIENCE)
				{
					iPromoValue *= 2;
				}
				iValue += iPromoValue;

				iValue += kBuilding.getNationalCaptureProbabilityModifier() * 2;

				if (iFocusFlags & BUILDINGFOCUS_INVESTIGATION)
				{
					iValue += kBuilding.getInvestigation() * 5;
				}
				else iValue += kBuilding.getInvestigation();

				if (iFocusFlags & BUILDINGFOCUS_PROPERTY)
				{
					iValue += buildingPropertiesValue(kBuilding) * 10;
				}
				else
				{
					iValue += buildingPropertiesValue(kBuilding);
				}

				if ((!bDevelopingCity || bCapital) && kBuilding.EnablesUnits())
				{
					PROFILE("CalculateAllBuildingValues.NotDeveloping");

					const CvGameObjectCity* pObject = getGameObject();
					// add the extra building and its bonuses to the override to see if they influence the train condition of a unit
					std::vector<GOMOverride> queries;
					GOMOverride query = { pObject, GOM_BUILDING, eBuilding, true };
					queries.push_back(query);
					query.GOM = GOM_BONUS;
					foreach_(const BonusModifier& pair, kBuilding.getFreeBonuses())
					{
						query.id = pair.first;
						queries.push_back(query);
					}

					for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
					{
						const CvUnitInfo& kUnit = GC.getUnitInfo((UnitTypes)iI);
						bool bUnitIsEnabler = kUnit.isPrereqAndBuilding((int)eBuilding);
						bool bUnitIsOtherwiseEnabled = false;

						if (team.isHasTech((TechTypes)kUnit.getPrereqAndTech()))
						{
							bUnitIsOtherwiseEnabled = bUnitIsEnabler || kUnit.getNumPrereqAndBuildings() == 0;
							if (!bUnitIsOtherwiseEnabled)
							{
								for (int iI = 0; iI < kUnit.getNumPrereqAndBuildings(); ++iI)
								{
									// Toffer - seems strange to use break here if only one of X "AND" requirements are met...
									if (isActiveBuilding((BuildingTypes)kUnit.getPrereqAndBuilding(iI)))
									{
										bUnitIsOtherwiseEnabled = true;
										break;
									}
								}
							}
						}

						if (bUnitIsOtherwiseEnabled)
						{
							// check if this building enables the train condition of this unit
							const BoolExpr* condition = kUnit.getTrainCondition();
							if (condition != NULL)
							{
								if (condition->evaluateChange(pObject, queries) == BOOLEXPR_CHANGE_BECOMES_TRUE)
								{
									bUnitIsEnabler = true;
								}
							}

							bool bUnitIsBonusEnabled = true;
							if (kUnit.getPrereqAndBonus() != NO_BONUS && !hasBonus((BonusTypes)kUnit.getPrereqAndBonus()))
							{
								if (kBuilding.getFreeBonuses().hasValue((BonusTypes)kUnit.getPrereqAndBonus()))
								{
									bUnitIsEnabler = true;
								}
								else bUnitIsBonusEnabled = false;
							}

							bool bHasORBonusAlready = false;
							bool bFreeBonusIsORBonus = false;
							int iFreeExtraBonusCount = 0;

							foreach_(const BonusTypes eXtraFreeBonus, kUnit.getPrereqOrBonuses())
							{
								iFreeExtraBonusCount++;

								if (hasBonus(eXtraFreeBonus))
								{
									bHasORBonusAlready = true;
								}
								else if (kBuilding.getFreeBonuses().hasValue(eXtraFreeBonus))
								{
									bFreeBonusIsORBonus = true;
								}
							}

							if (iFreeExtraBonusCount > 0 && !bHasORBonusAlready)
							{
								if (bFreeBonusIsORBonus)
								{
									bUnitIsEnabler = true;
								}
								else bUnitIsBonusEnabled = false;
							}


							if (bUnitIsEnabler)
							{
								PROFILE("CalculateAllBuildingValues.IsUnitPrereq");

								if (kUnit.getDefaultUnitAIType() != NO_UNITAI && kOwner.AI_totalAreaUnitAIs(pArea, kUnit.getDefaultUnitAIType()) == 0)
								{
									iValue += iNumCitiesInArea;
								}

								int unitsEnabledValue = 0;

								//This forces the AI to build necessary buildings for units.
								switch (kUnit.getDefaultUnitAIType())
								{
								case UNITAI_UNKNOWN:
									break;
								case UNITAI_ANIMAL:
									unitsEnabledValue += kUnit.getCombat() / 5;
									break;
								case UNITAI_SETTLE:
								case UNITAI_SETTLER_SEA:
									unitsEnabledValue += 25;
									break;
								case UNITAI_WORKER:
								case UNITAI_WORKER_SEA:
									unitsEnabledValue += std::max(0, AI_getWorkersNeeded()) * 10;
									break;
								case UNITAI_ATTACK:
								case UNITAI_ATTACK_CITY:
								case UNITAI_COLLATERAL:
								case UNITAI_PILLAGE:
								case UNITAI_RESERVE:
								case UNITAI_COUNTER:
								case UNITAI_CITY_DEFENSE:
								case UNITAI_CITY_COUNTER:
								case UNITAI_CITY_SPECIAL:
								case UNITAI_ATTACK_SEA:
								case UNITAI_RESERVE_SEA:
								case UNITAI_ESCORT_SEA:
								case UNITAI_ASSAULT_SEA:
								case UNITAI_CARRIER_SEA:
								case UNITAI_MISSILE_CARRIER_SEA:
								case UNITAI_PIRATE_SEA:
								case UNITAI_ATTACK_AIR:
								case UNITAI_DEFENSE_AIR:
								case UNITAI_CARRIER_AIR:
								case UNITAI_MISSILE_AIR:
								case UNITAI_PARADROP:
								case UNITAI_ATTACK_CITY_LEMMING:
									unitsEnabledValue += std::max(6, kUnit.getCombat() * kUnit.getCombat()) / 6;
									break;
								case UNITAI_INFILTRATOR:
								case UNITAI_INVESTIGATOR:
								case UNITAI_PROPERTY_CONTROL:
								case UNITAI_HEALER:
								case UNITAI_PROPERTY_CONTROL_SEA:
								case UNITAI_HEALER_SEA:
								case UNITAI_EXPLORE:
								case UNITAI_MISSIONARY:
								case UNITAI_PROPHET:
								case UNITAI_ARTIST:
								case UNITAI_SCIENTIST:
								case UNITAI_GENERAL:
								case UNITAI_GREAT_HUNTER:
								case UNITAI_GREAT_ADMIRAL:
								case UNITAI_MERCHANT:
								case UNITAI_ENGINEER:
								case UNITAI_SPY:
								case UNITAI_SPY_SEA:
								case UNITAI_MISSIONARY_SEA:
								case UNITAI_EXPLORE_SEA:
									unitsEnabledValue += 20;
									break;
								case UNITAI_ICBM:
									unitsEnabledValue += kUnit.getNukeRange() != -1 ? kUnit.getNukeRange() * 50 : 0;
									break;
								}
								if (unitsEnabledValue > 0)
								{
									if (bWarPlan) unitsEnabledValue *= 2;
									// Just because we can build new units here doesn't add much if we can already build them a ton of other places
									unitsEnabledValue = unitsEnabledValue * 3 / (3 + std::min(10, kOwner.countNumBuildings(eBuilding)));
									iValue += bUnitIsBonusEnabled ? unitsEnabledValue : unitsEnabledValue / 5;
								}
							}
						}
					}
				}
				valuesCache->Accumulate(BUILDINGFOCUSINDEX_EXPERIENCE, iValue);
			}
			{
				PROFILE("CalculateAllBuildingValues.Sea");

				int iValue = kBuilding.getFreeExperience() * (bMetAnyCiv ? 16 : 8);

				for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
				{
					const CvUnitInfo& kUnitInfo = GC.getUnitInfo((UnitTypes)iI);
					UnitCombatTypes eCombatType = (UnitCombatTypes)kUnitInfo.getUnitCombatType();

					if (eCombatType != NO_UNITCOMBAT && kUnitInfo.getDomainType() == DOMAIN_SEA && canTrain((UnitTypes)iI))
					{
						iValue += kBuilding.getUnitCombatFreeExperience().getValue(eCombatType) * (bMetAnyCiv ? 6 : 3);

						if (kBuilding.isUnitCombatRetrainType(eCombatType))
						{
							iValue += 20;
						}
					}
				}

				iValue += kBuilding.getDomainFreeExperience(DOMAIN_SEA) * (bMetAnyCiv ? 16 : 8);

				iValue += kBuilding.getDomainProductionModifier(DOMAIN_SEA) / 4;
				valuesCache->AccumulateTo(BUILDINGFOCUSINDEX_DOMAINSEA, iValue, false);
				valuesCache->AccumulateTo(BUILDINGFOCUSINDEX_DOMAINSEA, iValue, true);
			}
			if (kBuilding.getCommerceChange(COMMERCE_GOLD) < 0 && GC.getTREAT_NEGATIVE_GOLD_AS_MAINTENANCE())
			{
				PROFILE("CalculateAllBuildingValues.Maintenance");
				const int iBaseMaintenance = getMaintenanceTimes100();
				const int iMaintenanceMod = getEffectiveMaintenanceModifier();

				const int iCost =
				(
					std::min(
						-1,
						getModifiedIntValue(iBaseMaintenance, iMaintenanceMod)
						-
						getModifiedIntValue(
							iBaseMaintenance - kBuilding.getCommerceChange(COMMERCE_GOLD) * 100,
							iMaintenanceMod + kBuilding.getMaintenanceModifier()
						)
					)
				);
				int iTempValue = std::min(-1, iCost * iGoldValueAssessmentModifier / 500);

				if (bFinancialTrouble)
				{
					iTempValue *= 4;
				}
				valuesCache->Accumulate(BUILDINGFOCUSINDEX_GOLDANDMAINTENANCE, iTempValue);
			}
			{
				PROFILE("CalculateAllBuildingValues.Specialist");
				int iValue = 0;
				int iSpecialistsValue = 0;
				int iCurrentSpecialistsRunnable = 0;
				for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
				{
					if (iI != GC.getDEFAULT_SPECIALIST())
					{
						const bool bUnlimited = kOwner.isSpecialistValid((SpecialistTypes)iI);
						const int iRunnable = (getMaxSpecialistCount((SpecialistTypes)iI) > 0);

						if (bUnlimited || (iRunnable > 0))
						{
							if (bUnlimited)
							{
								iCurrentSpecialistsRunnable += 5;
							}
							else
							{
								iCurrentSpecialistsRunnable += iRunnable;
							}
						}

						if ((kBuilding.getSpecialistCount(iI) + team.getBuildingSpecialistChange(eBuilding, (SpecialistTypes)iI)) > 0)
						{
							if ((!bUnlimited) && (iRunnable < 5))
							{
								int specialistValue = AI_specialistValue(((SpecialistTypes)iI), false, false);

								specialistValue *= (20 + (40 * (kBuilding.getSpecialistCount(iI) + team.getBuildingSpecialistChange(eBuilding, (SpecialistTypes)iI))));
								specialistValue /= 100;

								if (iFoodDifference < 2)
								{
									specialistValue /= 4;
								}
								if (iRunnable > 0)
								{
									specialistValue /= 1 + iRunnable;
								}
								iSpecialistsValue += std::max(12, (specialistValue / 100));
							}
						}
					}
				}

				if (iSpecialistsValue > 0)
				{
					iValue += iSpecialistsValue / std::max(2, iCurrentSpecialistsRunnable);
				}
				valuesCache->Accumulate(BUILDINGFOCUSINDEX_SPECIALIST, iValue);
			}

			bool bForeignTrade = false;
			for (int iI = 0; iI < iNumTradeRoutes; ++iI)
			{
				CvCity* pTradeCity = getTradeCity(iI);
				if (NULL != pTradeCity
					&& (GET_PLAYER(pTradeCity->getOwner()).getTeam() != eTeam || pTradeCity->area() != pArea))
				{
					bForeignTrade = true;
					break;
				}
			}
			{
				PROFILE("CalculateAllBuildingValues.CommerceYields");

				// commerce yield
				const int iCommerceYieldValue = (
					AI_buildingYieldValue(YIELD_COMMERCE, eBuilding, kBuilding, bForeignTrade, aiFreeSpecialistYield[YIELD_COMMERCE])
					);

				valuesCache->Accumulate(
					BUILDINGFOCUSINDEX_GOLD,
					kOwner.getCommercePercent(COMMERCE_GOLD) * iCommerceYieldValue
					* (bFinancialTrouble ? 200 : iGoldValueAssessmentModifier)
					/ 10000
				);
				valuesCache->Accumulate(
					BUILDINGFOCUSINDEX_RESEARCH,
					kOwner.getCommercePercent(COMMERCE_RESEARCH) * iCommerceYieldValue / 100
				);
				valuesCache->Accumulate(
					BUILDINGFOCUSINDEX_CULTURE,
					kOwner.getCommercePercent(COMMERCE_CULTURE) * iCommerceYieldValue / 100
				);
				valuesCache->Accumulate(
					BUILDINGFOCUSINDEX_ESPIONAGE,
					kOwner.getCommercePercent(COMMERCE_ESPIONAGE) * iCommerceYieldValue / 100
				);
			}
			{ // valuesCache->AccumulateToAny(iValue, false);
				int iValue = 0;

				foreach_(const TraitTypes eTrait, kBuilding.getFreeTraitTypes())
				{
					if (GC.getTraitInfo(eTrait).isCivilizationTrait())
					{
						if (!GC.getTraitInfo(eTrait).isNegativeTrait())
						{
							for (int iJ = 0; iJ < GC.getNumFlavorTypes(); iJ++)
							{
								iValue += GC.getLeaderHeadInfo(kOwner.getLeaderType()).getFlavorValue(iJ) * GC.getTraitInfo(eTrait).getFlavorValue(iJ);
							}
						}
						else
						{
							for (int iJ = 0; iJ < GC.getNumFlavorTypes(); iJ++)
							{
								iValue -= GC.getLeaderHeadInfo(kOwner.getLeaderType()).getFlavorValue(iJ) * GC.getTraitInfo(eTrait).getFlavorValue(iJ);
							}
						}
					}
				}
				for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
				{
					if (kBuilding.isHurry(iI))
					{
						iValue += 50;
					}
				}
				iValue -= kBuilding.getRiverDefensePenalty() / 4;

				if (kBuilding.getDomesticGreatGeneralRateModifier() != 0)
				{
					iValue += (kBuilding.getDomesticGreatGeneralRateModifier() / 10);
				}

				// Very few buildings have this (great wall), so no need to cache these values before the loop
				if (kBuilding.isAreaBorderObstacle() && !pArea->isBorderObstacle(eTeam))
				{
					if (!GC.getGame().isOption(GAMEOPTION_BARBARIAN_NONE))
					{
						iValue += iNumCitiesInArea;

						if (GC.getGame().isOption(GAMEOPTION_BARBARIAN_RAGING))
						{
							iValue += iNumCitiesInArea;
						}
					}
				}

				if (kBuilding.isGovernmentCenter())
				{
					iValue += (calculateDistanceMaintenance() - 3) * iNumCitiesInArea;
				}
				if (kBuilding.isMapCentering())
				{
					iValue++;
				}

				foreach_(const BonusModifier& pair, kBuilding.getFreeBonuses())
				{
					// Spares doesn't do anything that scales by city count, only the first one does that.
					// Furthermore as spares rack up even their trade value decreases.
					iValue += (
						pair.second
						*
						kOwner.AI_bonusVal(pair.first, 1)
						*
						(kOwner.getNumTradeableBonuses(pair.first) == 0 ? iNumCities : 1)
						/
						std::max(1, kOwner.getNumTradeableBonuses(pair.first))
					);
				}

				if (kBuilding.getCivicOption() != NO_CIVICOPTION)
				{
					for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
					{
						if (GC.getCivicInfo((CivicTypes)iI).getCivicOptionType() == kBuilding.getCivicOption()
							&& !kOwner.canDoCivics((CivicTypes)iI))
						{
							iValue += (kOwner.AI_civicValue((CivicTypes)iI) / 10);
						}
					}
				}

				if (kBuilding.getGreatPeopleRateModifier() > 0)
				{
					// Either not a wonder, or a wonder and our GP rate is at least the target rate (10).
					if (!bIsLimitedWonder || iBaseGreatPeopleRate >= 10)
					{
						iValue += kBuilding.getGreatPeopleRateModifier() * iBaseGreatPeopleRate / 16;
					}
					// otherwise, this is a limited wonder (aka National Epic), we really do not want to build this here
					// subtract from the value (if this wonder has a lot of other stuff, we still might build it)
					else
					{
						iValue -= kBuilding.getGreatPeopleRateModifier() * (10 - iBaseGreatPeopleRate) / 12;
					}
				}

				iValue += kBuilding.getGlobalGreatPeopleRateModifier() * iNumCities / 8;
				iValue -= kBuilding.getAnarchyModifier() / 4;
				iValue -= kBuilding.getGlobalHurryModifier() * 2;
				iValue += kBuilding.getGlobalFreeExperience() * iNumCities * (bMetAnyCiv ? 6 : 3);

				if (bCanPopRush)
				{
					iValue += kBuilding.getFoodKept() / 2;
				}
				iValue += kBuilding.getAirlift() * (iPopulation * 3 + 10);

				if (kBuilding.getAirModifier() < 0
					&& (
						kOwner.AI_totalUnitAIs(UNITAI_MISSILE_AIR) > 0
						|| kOwner.AI_totalUnitAIs(UNITAI_DEFENSE_AIR) > 0
						&& kOwner.AI_totalUnitAIs(UNITAI_ATTACK_AIR) > 0
						)
					) iValue -= kBuilding.getAirModifier() / (bMetAnyCiv ? 2 : 4);

				iValue += kBuilding.getAirUnitCapacity() * (iPopulation * 2 + 10);
				iValue -= kBuilding.getNukeModifier() / (bMetAnyCiv ? 10 : 20);
				iValue += kBuilding.getAreaFreeSpecialist() * iNumCitiesInArea * 12;
				iValue += kBuilding.getGlobalFreeSpecialist() * iNumCities * 12;
				iValue += kBuilding.getWorkerSpeedModifier() * kOwner.AI_getNumAIUnits(UNITAI_WORKER) / 10;

				int iMilitaryProductionModifier = kBuilding.getMilitaryProductionModifier();

				for (int iJ = 0; iJ < kBuilding.getNumUnitCombatProdModifiers(); iJ++)
				{
					iMilitaryProductionModifier += kBuilding.getUnitCombatProdModifier(iJ) / 4;
				}

				if (bMetAnyCiv && iMilitaryProductionModifier > 0)
				{
					// either not a wonder, or a wonder and we are a high production city
					if (!bIsLimitedWonder || bIsHighProductionCity)
					{
						iValue += (iMilitaryProductionModifier / 4);

						// if a wonder, then pick one of the best cities
						if (bIsLimitedWonder)
						{
							// if one of the top 3 production cities, give a big boost
							if (aiYieldRank[YIELD_PRODUCTION] <= (2 + iLimitedWonderLimit))
							{
								iValue += (2 * iMilitaryProductionModifier) / (2 + aiYieldRank[YIELD_PRODUCTION]);
							}
						}
						// otherwise, any of the top half of cities will do
						else if (bIsHighProductionCity)
						{
							iValue += iMilitaryProductionModifier / 4;
						}
						iValue += iMilitaryProductionModifier * (getFreeExperience() + getSpecialistFreeExperience()) / 10;
					}
					// otherwise, this is a limited wonder (aka Heroic Epic), we really do not want to build this here
					// subtract from the value (if this wonder has a lot of other stuff, we still might build it)
					else iValue -= iMilitaryProductionModifier * aiYieldRank[YIELD_PRODUCTION] / 5;
				}
				iValue += (kBuilding.getSpaceProductionModifier() / 5);
				iValue += ((kBuilding.getGlobalSpaceProductionModifier() * iNumCities) / 20);


				if (kBuilding.getGreatPeopleUnitType() != NO_UNIT)
				{
					iValue++; // XXX improve this for diversity...
				}
				// prefer to build great people buildings in places that already have some GP points
				iValue += (kBuilding.getGreatPeopleRateChange() + iSpecialistGreatPeopleRate) * 10 * (1 + getBaseGreatPeopleRate() / 2);

				if (!bAreaAlone)
				{
					for (int iI = 0; iI < kBuilding.getNumHealUnitCombatTypes(); iI++)
					{
						iValue += (kBuilding.getHealUnitCombatType(iI).iHeal / 4);
					}
					iValue += (kBuilding.getHealRateChange() / 2);
					for (int iI = 0; iI < kBuilding.getNumAidRateChanges(); iI++)
					{
						iValue += (kBuilding.getAidRateChange(iI).iChange / 3);//Update
					}
					for (int iI = 0; iI < kBuilding.getNumBonusAidModifiers(); iI++)
					{
						if (hasBonus(kBuilding.getBonusAidModifier(iI).eBonusType))
						{
							iValue += kBuilding.getBonusAidModifier(iI).iModifier / 3;
						}
					}
				}

				iValue += kBuilding.getGlobalPopulationChange() * iNumCities * 4;
				iValue += kBuilding.getFreeTechs() * 80;
				iValue += kBuilding.getEnemyWarWearinessModifier() / 2;

				for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
				{
					if (kBuilding.getFreeSpecialistCount(iI) > 0)
					{
						iValue += AI_specialistValue((SpecialistTypes)iI, false, false) * kBuilding.getFreeSpecialistCount(iI) / 50;
					}
				}

				foreach_(const ImprovementModifier & pair, kBuilding.getImprovementFreeSpecialists())
				{
					iValue += pair.second * countNumImprovedPlots(pair.first, true) * 50;
				}

				for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
				{
					iValue += (kBuilding.getDomainProductionModifier(iI) / 5);

					if (bIsHighProductionCity)
					{
						iValue += (kBuilding.getDomainProductionModifier(iI) / 5);
					}
				}

				if (kBuilding.isForceAllTradeRoutes())
				{
					for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
					{
						if (GET_TEAM((TeamTypes)iI).isAlive()
							&& !team.isVassal((TeamTypes)iI)
							&& !team.isOpenBorders((TeamTypes)iI))
						{
							iValue += GET_TEAM((TeamTypes)iI).getNumCities() * 2;
						}
					}
				}
				foreach_(const UnitModifier2 & modifier, kBuilding.getUnitProductionModifiers())
				{
					int unitProductionModifierValue = 0;
					const UnitTypes eLoopUnit = modifier.first;
					if (canTrain(eLoopUnit))
					{
						const int iModifier = modifier.second;
						const UnitAITypes eUnitAI = GC.getUnitInfo(eLoopUnit).getDefaultUnitAIType();
						const UnitTypes eBestUnit = kOwner.bestBuildableUnitForAIType(GC.getUnitInfo(eLoopUnit).getDomainType(), eUnitAI);

						int iBuildCost = GC.getUnitInfo(eLoopUnit).getProductionCost();

						if (eBestUnit != NO_UNIT)
						{
							// Value as cost of production of the unit we can build scaled by their relative AI value (non-linear - we're squaring the ratio)
							const int iComparisonToBestFactor =
								(
									10 * kOwner.AI_unitValue(eLoopUnit, eUnitAI, pArea)
									/
									std::max(1, kOwner.AI_unitValue(eBestUnit, eUnitAI, pArea))
									);
							iBuildCost = iComparisonToBestFactor * iComparisonToBestFactor * iBuildCost / 100;
						}
						int iTempValue = iBuildCost * iModifier / 50;
						if (bIsHighProductionCity)
						{
							iTempValue *= 3;
						}
						unitProductionModifierValue += iTempValue;
					}

					iValue += unitProductionModifierValue;
				}
				if (kBuilding.getPopulationgrowthratepercentage() != 0 && iCityHappy >= 0 && iFoodDifference > 0)
				{
					iValue -=
						(
							kBuilding.getPopulationgrowthratepercentage()
							* std::min(3, iCityHappy + 1) * growthThreshold()
							/ (100 * iFoodDifference)
							);
				}
				if (kBuilding.getGlobalPopulationgrowthratepercentage() != 0)
				{
					int iCityCount = 0;
					int popGrowthRateGlobalValue = 0;
					foreach_(const CvCity * pLoopCity, kOwner.cities())
					{
						const int iFoodPerTurn = pLoopCity->foodDifference(false);

						iCityCount++;

						if (iFoodPerTurn > 0)
						{
							const int iCityHappyX = pLoopCity->happyLevel() - pLoopCity->unhappyLevel();
							if (iCityHappyX >= 0)
							{
								popGrowthRateGlobalValue -=
									(
										std::min(3, iCityHappyX + 1) * pLoopCity->growthThreshold() / iFoodPerTurn
										);
							}
						}
					}

					if (iCityCount > 0)
					{
						iValue += kBuilding.getGlobalPopulationgrowthratepercentage() * popGrowthRateGlobalValue / (iCityCount * 100);
					}
				}

				foreach_(const BuildingModifier2 & modifier, kBuilding.getBuildingProductionModifiers())
				{
					const BuildingTypes eLoopBuilding = modifier.first;
					if (canConstruct(eLoopBuilding))
					{
						const int iModifier = modifier.second;
						if (iModifier > -100)
						{
							const int iOriginalCost = getHurryCost(eLoopBuilding);
							const int iNewCost = (iOriginalCost * (100 / (100 + iModifier)));
							iValue += (iOriginalCost - iNewCost) / 10;
						}
						else
						{//If the modifier is less than -100, avoid it like the plague
							iValue -= 100;
						}
					}
				}

				foreach_(const BuildingModifier2 & modifier, kBuilding.getGlobalBuildingProductionModifiers())
				{
					const BuildingTypes eLoopBuilding = modifier.first;
					if (canConstruct(eLoopBuilding))
					{
						const int iModifier = modifier.second;
						const int iOriginalCost = getHurryCost(eLoopBuilding);

						if (iModifier > -100)
						{
							const int iCount = count_if(kOwner.cities(), !CvCity::fn::hasBuilding(eLoopBuilding));
							const int iNewCost = (iOriginalCost * (100 / (100 + iModifier)));
							iValue += ((iOriginalCost - iNewCost) * iCount) / 10;
						}
						else
						{//If the modifier is less than -100, avoid it like the plague
							iValue -= 100;
						}
					}
				}

				foreach_(const BuildingModifier2 & modifier, kBuilding.getGlobalBuildingCostModifiers())
				{
					const BuildingTypes eLoopBuilding = modifier.first;
					if (kOwner.canConstruct(eLoopBuilding))
					{
						const int iOriginalCost = kOwner.getProductionNeeded(eLoopBuilding);
						int iPlayerMod = kOwner.getBuildingCostModifier(eLoopBuilding);

						int iNewCost = 0;
						// Reverse old modifier
						if (iPlayerMod < 0)
						{
							iNewCost = iOriginalCost * (-1 * iPlayerMod + 100) / 100;
						}
						else if (iPlayerMod > 0)
						{
							iNewCost = iOriginalCost * 100 / (100 + iPlayerMod);
						}
						iPlayerMod += modifier.second;
						// Apply new modifier
						if (iPlayerMod < 0)
						{
							iNewCost = iOriginalCost * 100 / (-1 * iPlayerMod + 100);
						}
						else if (iPlayerMod > 0)
						{
							iNewCost = iOriginalCost * (100 + iPlayerMod) / 100;
						}

						const int iCount = count_if(kOwner.cities(), !CvCity::fn::hasBuilding(eLoopBuilding));
						iValue += (iOriginalCost - iNewCost) * iCount / 10;
					}
				}

				if (kBuilding.isProvidesFreshWater() && !plot()->isFreshWater())
				{
					int freshWaterModifier = 0;
					foreach_(const CvPlot * pLoopPlot, plots(NUM_CITY_PLOTS_1, true))
					{
						if (!pLoopPlot->isWater() &&
							!pLoopPlot->isFreshWater() &&
							!pLoopPlot->isHills() &&
							!pLoopPlot->isImpassable())
						{
							if (pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
							{//if there are no improvements, farms become much more likely
								freshWaterModifier += 20;
							}
							else
							{//we are not likely to tear down existing improvements for a farm
								freshWaterModifier += 5;
							}
						}
					}
					iValue += freshWaterModifier;
				}

				int religiousBuildingValue = 0;
				for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
				{
					if (GC.getUnitInfo((UnitTypes)iI).isPrereqAndBuilding((int)eBuilding))
					{
						// BBAI TODO: Smarter monastary construction, better support for mods

						if (kOwner.AI_totalAreaUnitAIs(pArea, GC.getUnitInfo((UnitTypes)iI).getDefaultUnitAIType()) == 0)
						{
							religiousBuildingValue += iNumCitiesInArea;
						}

						religiousBuildingValue++;

						const ReligionTypes eReligion = (ReligionTypes)(GC.getUnitInfo((UnitTypes)iI).getPrereqReligion());
						if (eReligion != NO_RELIGION)
						{
							//encouragement to get some minimal ability to train special units
							if (bCulturalVictory1 || bCapital || isHolyCity(eReligion))
							{
								religiousBuildingValue += (2 + iNumCitiesInArea);
							}

							if (bCulturalVictory2 && GC.getUnitInfo((UnitTypes)iI).getReligionSpreads(eReligion))
							{
								//this gives a very large extra value if the religion is (nearly) unique
								//to no extra value for a fully spread religion.
								//I'm torn between huge boost and enough to bias towards the best monastery type.
								int iReligionCount = kOwner.getHasReligionCount(eReligion);
								religiousBuildingValue += (100 * (iNumCities - iReligionCount)) / (iNumCities * (iReligionCount + 1));
							}
						}
					}
				}

				if (religiousBuildingValue > 0)
				{
					// Just because we can build new units here doesn't add much if we can already build them a ton of other places
					religiousBuildingValue = religiousBuildingValue * 3 / (3 + std::min(10, kOwner.countNumBuildings(eBuilding)));
				}

				iValue += religiousBuildingValue;

				// is this building needed to build other buildings?
				for (int iI = 0; iI < iNumBuildings; iI++)
				{
					const BuildingTypes eType = static_cast<BuildingTypes>(iI);
					const int iPrereqBuildings = kOwner.getBuildingPrereqBuilding(eType, eBuilding);

					// if we need some of us to build iI building, and we dont need more than we have cities
					if (iPrereqBuildings > 0 && iPrereqBuildings <= iNumCities
					// do we need more than what we are currently building?
					&& iPrereqBuildings > kOwner.getBuildingCountPlusMaking(eBuilding))
					{
						iValue += 3 * iNumCities;

						if (bCulturalVictory1)
						{
							const int iCultureModifier = GC.getBuildingInfo(eType).getCommerceModifier(COMMERCE_CULTURE);
							if (
								iCultureModifier > 0
							&&	(
									// If we have less than the number needed in culture cities
									kOwner.getBuildingCount(eType) < iCulturalVictoryNumCultureCities
									// OR we are one of the top cities AND we do not have the building
									|| iCultureRank <= iCulturalVictoryNumCultureCities
									&& !hasBuilding(eType)
								)
							) iValue += bCulturalVictory3 ? iCultureModifier * 3 : iCultureModifier;
						}
					}
				}

				if (kBuilding.getVoteSourceType() != NO_VOTESOURCE)
				{
					for (int iI = 0; iI < GC.getNumVoteSourceInfos(); ++iI)
					{
						if (kBuilding.getVoteSourceType() == iI)
						{
							if (kBuilding.needStateReligionInCity())
							{
								int iShareReligionCount = 0;
								int iPlayerCount = 0;
								for (int iK = 0; iK < MAX_PC_PLAYERS; iK++)
								{
									if (iK != ePlayer && GET_PLAYER((PlayerTypes)iK).isAlive())
									{
										iPlayerCount++;
										if (eStateReligion == GET_PLAYER((PlayerTypes)iK).getStateReligion())
										{
											iShareReligionCount++;
										}
									}
								}
								iValue += iVoteValueFactor * 200 * (1 + iShareReligionCount) / (1 + iPlayerCount);
							}
							else iValue += iVoteValueFactor * 100;
						}
					}
				}
				iValue += AI_getBuildingYieldValue(
					eBuilding, kBuilding, bIsLimitedWonder, bForeignTrade, bFinancialTrouble,
					aiFreeSpecialistYield, aiYieldRank, iLimitedWonderLimit, pArea, iTotalPopulation, iFoodDifference
				);
				valuesCache->AccumulateToAny(iValue, false);
			}

			{
				PROFILE("CalculateAllBuildingValues.Food");

				valuesCache->AccumulateTo(
					BUILDINGFOCUSINDEX_FOOD,
					AI_buildingYieldValue(YIELD_FOOD, eBuilding, kBuilding, bForeignTrade, aiFreeSpecialistYield[YIELD_FOOD]) + kBuilding.getFoodKept(),
					true
				);
			}
			valuesCache->AccumulateTo(
				BUILDINGFOCUSINDEX_PRODUCTION,
				AI_getBuildingProductionValue(
					eBuilding, kBuilding, bIsLimitedWonder, bForeignTrade,
					aiFreeSpecialistYield[YIELD_PRODUCTION],
					aiYieldRank[YIELD_PRODUCTION], iLimitedWonderLimit
				)
				, true
			);
			{
				int iValue = (
					AI_getBuildingCommerceValue(
						eBuilding, kBuilding, bIsLimitedWonder, bFinancialTrouble, bCulturalVictory1,
						aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate, aiCommerceRank,
						iLimitedWonderLimit, iTotalPopulation
					)
				);

				foreach_(const ReligionModifier & pair, kBuilding.getReligionChanges())
				{
					if (team.hasHolyCity(pair.first))
					{
						iValue += pair.second * (eStateReligion == pair.first ? 10 : 1);
					}
				}

				if (NO_VOTESOURCE != kBuilding.getVoteSourceType())
				{
					iValue += 100;
				}
				valuesCache->AccumulateToAny(iValue, false);
			}
			valuesCache->AccumulateTo(BUILDINGFOCUSINDEX_GOLD, getBuildingCommerceValue(eBuilding, COMMERCE_GOLD, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate), true);
			valuesCache->AccumulateTo(BUILDINGFOCUSINDEX_RESEARCH, getBuildingCommerceValue(eBuilding, COMMERCE_RESEARCH, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate), true);
			valuesCache->AccumulateTo(BUILDINGFOCUSINDEX_CULTURE, getBuildingCommerceValue(eBuilding, COMMERCE_CULTURE, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate), true);
			valuesCache->AccumulateTo(BUILDINGFOCUSINDEX_BIGCULTURE, getBuildingCommerceValue(eBuilding, COMMERCE_CULTURE, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate) / 5, true);
			valuesCache->AccumulateTo(BUILDINGFOCUSINDEX_ESPIONAGE, getBuildingCommerceValue(eBuilding, COMMERCE_ESPIONAGE, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate), true);

			if (!isHuman())
			{
				valuesCache->AccumulateToAny(kBuilding.getAIWeight(), false);
				// Flavor calculation is non-linear and cannot be calculated in the caching, it is calculated post-cache retrieval.
			}
		}
	}
}

int CvCityAI::getBuildingCommerceValue(BuildingTypes eBuilding, int iI, int* aiFreeSpecialistYield, int* aiFreeSpecialistCommerce, int* aiBaseCommerceRate, int* aiPlayerCommerceRate) const
{
	PROFILE_EXTRA_FUNC();
	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	const CvPlayerAI& kOwner = GET_PLAYER(getOwner());
	int iLimitedWonderLimit = limitedWonderLimit(eBuilding);
	bool bCulturalVictory1 = kOwner.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE1);
	bool bCulturalVictory3 = kOwner.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3);

	int iBaseCommerceChange = getBaseCommerceRateFromBuilding100((CommerceTypes)iI, eBuilding);

	int iResult = 4 * (iBaseCommerceChange + 100 * aiFreeSpecialistCommerce[iI]); // iResult is scaled by 100 at this point

	if (iResult > 0)
	{
		if (iI == COMMERCE_CULTURE)
		{
			// K-Mod. + 2 * foreign culture percent.
			iResult *= 300 - 2 * calculateCulturePercent(getOwner()); // iResult * range(100, 300)
			iResult /= 100;
			// K-Mod end
			if (bCulturalVictory1)
			{
				iResult *= 2;
			}
		}
		if (kBuilding.getCommerceChangeDoubleTime(iI) > 0)
		{
			iResult += iResult * 500 / (500 + kBuilding.getCommerceChangeDoubleTime(iI));
		}
		// Multiply up by the city's current commerce rate modifier
		iResult = std::max(1, iResult * getTotalCommerceRateModifier((CommerceTypes)iI) / 100);
	}

	// Toffer - Hmm, the following code is odd, why not use the AI_buildingYieldValue function?

	//	Factor in yield changes
	const int iSemiModifiedBase = (
		getPlotYield(YIELD_COMMERCE) * (
			kBuilding.getYieldModifier(YIELD_COMMERCE)
			+ GET_TEAM(getTeam()).getBuildingYieldTechModifier(YIELD_COMMERCE, eBuilding)
		)
	);
	int iTempValue = iSemiModifiedBase * kOwner.getCommercePercent((CommerceTypes)iI) / 3000;

	// if this is a limited wonder, and we are not one of the top 4 in this category, subtract the value
	// we do _not_ want to build this here (unless the value was small anyway)
	if (iLimitedWonderLimit >= 0 && (findCommerceRateRank((CommerceTypes)iI) > (3 + iLimitedWonderLimit)))
	{
		iTempValue *= -1;
	}

	iResult += iTempValue;

	iTempValue = (
		(iSemiModifiedBase + aiFreeSpecialistYield[YIELD_COMMERCE]) / 8
		+
		getBaseYieldRateFromBuilding100(YIELD_COMMERCE, eBuilding) * 8 / 100
	);

	for (int iJ = 0; iJ < GC.getNumBonusInfos(); iJ++)
	{
		if (hasBonus((BonusTypes)iJ))
		{
			//TB Traits begin
			iTempValue += (kBuilding.getBonusYieldModifier(iJ, YIELD_COMMERCE) * getBaseYieldRate(YIELD_COMMERCE) / 12);
			//TB Traits end
			iTempValue += (kBuilding.getBonusYieldChanges(iJ, YIELD_COMMERCE) * 8);
			if (kBuilding.getVicinityBonusYieldChanges(iJ, YIELD_COMMERCE) != 0 && hasVicinityBonus((BonusTypes)iJ))
			{
				iTempValue += (kBuilding.getVicinityBonusYieldChanges(iJ, YIELD_COMMERCE) * 8);
			}
		}
	}
	iTempValue *= GET_PLAYER(getOwner()).getCommercePercent((CommerceTypes)iI);
	iTempValue /= 100;
	iResult += iTempValue;

	// add value for a commerce modifier
	int iCommerceModifier = kBuilding.getCommerceModifier(iI) + GET_TEAM(getTeam()).getBuildingCommerceTechModifier((CommerceTypes)iI, eBuilding);

	for (int iJ = 0; iJ < GC.getNumBonusInfos(); iJ++)
	{
		iCommerceModifier += kBuilding.getBonusCommerceModifier(iJ, iI);
	}

	if (aiBaseCommerceRate[iI] == MAX_INT)
	{
		aiBaseCommerceRate[iI] = getBaseCommerceRate((CommerceTypes)iI);
	}
	int iBaseCommerceRate = aiBaseCommerceRate[iI];
	int iCommerceMultiplierValue = iCommerceModifier * iBaseCommerceRate;

	//	Koshling - % modifiers are undervalued by the AI for two reasons:
	//	1)	It only calculates its efect at the time of construction, but that discounts future increases as
	//		base reserach goes up
	//	2)	In the early game because this calculation only takes account of the absolute increase, not its proportion of the
	//		civilization whole, which will be substantial early on (indeed 100% with 1 city).
	//	We therefore apply a small boost to reflect futures, and compare to the whole
	//	and make an upward adjustment proportional to the ratio with the civilization whole.
	//
	//	We don't apply this for culture, since it isn't really a civ-wide quantity for most purposes
	if (iCommerceMultiplierValue > 0 && iI != COMMERCE_CULTURE)
	{
		iCommerceMultiplierValue += 120; // Discounted futures
		iCommerceMultiplierValue += 2 * iCommerceMultiplierValue * std::max(1, aiBaseCommerceRate[iI])
			/ std::max(1, aiPlayerCommerceRate[iI]); // Proportion of whole
	}

	if (iI == COMMERCE_CULTURE && iCommerceModifier != 0)
	{
		int iCultureRateRank = findCommerceRateRank(COMMERCE_CULTURE);
		int iCulturalVictoryNumCultureCities = GC.getGame().culturalVictoryNumCultureCities();

		if (bCulturalVictory1)
		{
			// if this is one of our top culture cities, then we want to build this here first!
			if (iCultureRateRank <= iCulturalVictoryNumCultureCities)
			{
				iCommerceMultiplierValue /= 8;

				// if we at culture level 3, then these need to get built asap
				if (bCulturalVictory3)
				{
					// its most important to build in the lowest rate city, but important everywhere
					iCommerceMultiplierValue += std::max(100, 500 - iBaseCommerceRate) * iCommerceModifier;
				}
			}
			else
			{
				int iCountBuilt = kOwner.getBuildingCountPlusMaking(eBuilding);

				// do we have enough buildings to build extras?
				bool bHaveEnough = true;

				// if its limited and the limit is less than the number we need in culture cities, do not build here
				if (iLimitedWonderLimit >= 0 && (iLimitedWonderLimit <= iCulturalVictoryNumCultureCities))
				{
					bHaveEnough = false;
				}

				for (int iJ = 0; bHaveEnough && iJ < GC.getNumBuildingInfos(); iJ++)
				{
					// count excess the number of prereq buildings which do not have this building built for yet
					const int iPrereqBuildings = kOwner.getBuildingPrereqBuilding(eBuilding, (BuildingTypes)iJ, -iCountBuilt);

					// do we not have enough built (do not count ones in progress)
					if (iPrereqBuildings > 0 && kOwner.getBuildingCount((BuildingTypes)iJ) < iPrereqBuildings)
					{
						bHaveEnough = false;
					}
				}

				// if we have enough and our rank is close to the top, then possibly build here too
				if (bHaveEnough && (iCultureRateRank - iCulturalVictoryNumCultureCities) <= 3)
				{
					iCommerceMultiplierValue /= 12;
				}
				// otherwise, we really do not want to build this here
				else
				{
					iCommerceMultiplierValue /= 30;
				}
			}
		}
		else
		{
			iCommerceMultiplierValue /= 15;

			// increase priority if we need culture oppressed city
			iCommerceMultiplierValue *= (100 - calculateCulturePercent(getOwner()));
		}
	}
	else
	{
		iCommerceMultiplierValue /= 15;
	}
	iResult += iCommerceMultiplierValue;

	iResult += kBuilding.getGlobalCommerceModifier(iI) * GET_PLAYER(getOwner()).getCommerceRate((CommerceTypes)iI) / 8;
	iResult += kBuilding.getSpecialistExtraCommerce(iI) * kOwner.getTotalPopulation() / 3;
	{
		const ReligionTypes eStateReligion = kOwner.getStateReligion();

		if (eStateReligion != NO_RELIGION)
		{
			iResult += kBuilding.getStateReligionCommerce(iI) * kOwner.getHasReligionCount(eStateReligion) * 3;
		}
		const ReligionTypes eReligionGlobalCommerce = (ReligionTypes)kBuilding.getGlobalReligionCommerce();

		if (eReligionGlobalCommerce != NO_RELIGION)
		{
			iResult += GC.getReligionInfo(eReligionGlobalCommerce).getGlobalReligionCommerce(iI) * GC.getGame().countReligionLevels(eReligionGlobalCommerce) * 2;

			if (eStateReligion == eReligionGlobalCommerce)
			{
				iResult += 10;
			}
		}
	}

	CorporationTypes eCorporation = (CorporationTypes)kBuilding.getFoundsCorporation();
	int iCorpValue = 0;
	if (NO_CORPORATION != eCorporation)
	{
		iCorpValue = kOwner.AI_corporationValue(eCorporation, this);

		for (int iCorp = 0; iCorp < GC.getNumCorporationInfos(); iCorp++)
		{
			if (iCorp != eCorporation && kOwner.hasHeadquarters((CorporationTypes)iCorp)
				&& GC.getGame().isCompetingCorporation(eCorporation, (CorporationTypes)iCorp))
			{
				if (kOwner.AI_corporationValue((CorporationTypes)iCorp, this) > iCorpValue)
				{
					iCorpValue = -1;
					break;
				}
				else if (!isHasCorporation((CorporationTypes)iCorp))
				{
					iCorpValue = -1;
				}
			}
		}
		iResult += iCorpValue / 25;
	}

	if (iCorpValue >= 0) // Don't build if it'll hurt us.
	{
		if (kBuilding.getGlobalCorporationCommerce() != NO_CORPORATION)
		{
			int iGoldValue = (GC.getCorporationInfo((CorporationTypes)(kBuilding.getGlobalCorporationCommerce())).getHeadquarterCommerce(iI) * GC.getGame().countCorporationLevels((CorporationTypes)(kBuilding.getGlobalCorporationCommerce())) * 2);

			iGoldValue += GC.getCorporationInfo((CorporationTypes)(kBuilding.getGlobalCorporationCommerce())).getHeadquarterCommerce(iI);
			if (iGoldValue > 0)
			{
				iGoldValue += 2 + (kOwner.getNumCities() / 4);
				iGoldValue += std::min(iGoldValue, getBuildingCommerce((CommerceTypes)iI) / 2) / 2;
			}
			iGoldValue *= 2;
			iGoldValue *= getTotalCommerceRateModifier((CommerceTypes)iI);
			iGoldValue *= std::max(50, getTotalCommerceRateModifier((CommerceTypes)iI) - 150);
			iGoldValue /= 5000;
			iCorpValue += iGoldValue;
		}
	}

	if (iCorpValue > 0)
	{
		if (kOwner.isNoCorporations())
		{
			iCorpValue /= 2;
		}
		iResult += iCorpValue;
	}

	if (kBuilding.isCommerceFlexible(iI) && !kOwner.isCommerceFlexible((CommerceTypes)iI))
	{
		iResult += 40;
	}

	if (iI == COMMERCE_GOLD && kOwner.AI_isFinancialTrouble())
	{
		iResult *= 2;
	}

	if (iI == COMMERCE_RESEARCH && kOwner.AI_isDoVictoryStrategy(AI_VICTORY_SPACE1))
	{
		iResult *= 3;
		iResult /= 2;
	}

	//	If we are desperate for SOME culture boost non-trivial producers
	if (iI == COMMERCE_CULTURE && iResult >= 3 && AI_calculateTargetCulturePerTurn() > 0)
	{
		iResult += 7;
	}

	//	For research if this is a significant boost to our total (civ wide) then give it significant
	//	extra weight
	if (iI == COMMERCE_RESEARCH)
	{
		int iPlayerTotal = GET_PLAYER(getOwner()).getCommerceRate(COMMERCE_RESEARCH);

		if (iPlayerTotal < 1)
		{
			iPlayerTotal = 1;
		}
		// iResult should be approx 8*<research gain> so
		// we are adding a multiplier based on the proportion of the total
		// doubling our total will multiply by 9 (+= 8*)
		iResult += iResult * iResult / iPlayerTotal;
	}
	if (iResult > 0)
	{
		return std::max(1, iResult / 100);
	}
	if (iResult < 0)
	{
		return std::min(-1, iResult / 100);
	}
	return 0;
}


int CvCityAI::tradeRouteValue(const CvBuildingInfo& kBuilding, YieldTypes eYield, bool bForeignTrade) const
{
	PROFILE_FUNC();

	const int iExtraTradeRoutes = kBuilding.getTradeRoutes();
	const int iCurrentTradeRoutes = getTradeRoutes();
	const CvPlayerAI& kOwner = GET_PLAYER(getOwner());

	int	iTradeRouteValue = 0;
	if (iCurrentTradeRoutes <= -iExtraTradeRoutes)
	{
		iTradeRouteValue = 0;
	}
	else if (iCurrentTradeRoutes > 0)
	{
		iTradeRouteValue = getTradeYield(eYield) / iCurrentTradeRoutes;
	}
	else
	{
		CvCity* pCity = kOwner.getCapitalCity();
		if (!pCity)
		{
			int iLoop;
			pCity = kOwner.firstCity(&iLoop);
		}

		if (pCity)
		{
			iTradeRouteValue = calculateTradeYield(eYield, calculateTradeProfit(pCity));

			if (kOwner.isNoForeignTrade())
			{
				iTradeRouteValue /= 3;
			}
		}
	}

	int iValue = std::min(iExtraTradeRoutes, iCurrentTradeRoutes + iExtraTradeRoutes) * 4 * iTradeRouteValue;
	iValue += (kBuilding.getCoastalTradeRoutes() * kOwner.countNumCoastalCities() * 4 * iTradeRouteValue);
	iValue += (kBuilding.getGlobalTradeRoutes() * kOwner.getNumCities() * 4 * iTradeRouteValue);

	int iUnmodifiedCurrentValue = (100 * getTradeYield(eYield)) / (100 + getTradeRouteModifier());

	iValue += ((kBuilding.getTradeRouteModifier() * iUnmodifiedCurrentValue) / (bForeignTrade ? 12 : 25));
	if (bForeignTrade)
	{
		iValue += ((kBuilding.getForeignTradeRouteModifier() * iUnmodifiedCurrentValue) / 12);
	}

	return iValue;
#ifdef OLD_TRADE_ROUTE_CALCULATION
	iTempValue = ((kBuilding.getTradeRoutes() * ((8 * std::max(0, (totalTradeModifier() + 100))) / 100))
		* (getPopulation() / 5 + 1));
	int iGlobalTradeValue = (((6 * iTotalPopulation) / 5) / iNumCities);
	iTempValue += (kBuilding.getCoastalTradeRoutes() * kOwner.countNumCoastalCities() * iGlobalTradeValue);
	iTempValue += (kBuilding.getGlobalTradeRoutes() * iNumCities * iGlobalTradeValue);

	iTempValue += ((kBuilding.getTradeRouteModifier() * getTradeYield(YIELD_COMMERCE)) / (bForeignTrade ? 12 : 25));
	if (bForeignTrade)
	{
		iTempValue += ((kBuilding.getForeignTradeRouteModifier() * getTradeYield(YIELD_COMMERCE)) / 12);
	}

	if (bFinancialTrouble)
	{
		iTempValue *= 2;
	}

	if (kOwner.isNoForeignTrade())
	{
		iTempValue /= 3;
	}
#endif
}

// Evaluate a building we are considering building here in terms of its effect on properties
int CvCityAI::buildingPropertiesValue(const CvBuildingInfo& kBuilding) const
{
	PROFILE_EXTRA_FUNC();
	//	Evaluate building properties
	std::map<int, int> effectivePropertyChanges;

	foreach_(const CvPropertySource * pSource, kBuilding.getPropertyManipulators()->getSources())
	{
		//	For now we're only interested in constant sources
		//	TODO - expand this as buildings add other types
		if (pSource->getType() == PROPERTYSOURCE_CONSTANT)
		{
			// Convert to an effective absolute amount by looking at the steady state value given current
			const PropertyTypes eProperty = pSource->getProperty();
			//	Only count half the unit source as we want to encourage building sources over unit ones
			int iCurrentSourceSize = getTotalBuildingSourcedProperty(eProperty) + getTotalUnitSourcedProperty(eProperty) / 2 + getPropertyNonBuildingSource(eProperty);
			int iNewSourceSize = iCurrentSourceSize + static_cast<const CvPropertySourceConstant*>(pSource)->getAmountPerTurn(getGameObject());
			int iDecayPercent = getPropertyDecay(eProperty);

			//	Steady state occurs at a level where the decay removes as much per turn as the sources add
			//	Decay can be 0 if the current level is below the threshold at which decay cuts in, so for the
			//	purposes of calculation just treat this as very slow decay
			int	iCurrentSteadyStateLevel = (100 * iCurrentSourceSize) / std::max(1, iDecayPercent);
			int	iNewSteadyStateLevel = (100 * iNewSourceSize) / std::max(1, iDecayPercent);

			std::map<int, int>::iterator itr = effectivePropertyChanges.find(eProperty);
			if (itr == effectivePropertyChanges.end())
			{
				effectivePropertyChanges[eProperty] = (iNewSteadyStateLevel - iCurrentSteadyStateLevel);
			}
			else
			{
				itr->second += (iNewSteadyStateLevel - iCurrentSteadyStateLevel);
			}
		}
	}

	const CvProperties* buildingProperties = kBuilding.getProperties();
	int num = buildingProperties->getNumProperties();
	for (int iI = 0; iI < num; iI++)
	{
		const PropertyTypes eProperty = (PropertyTypes)buildingProperties->getProperty(iI);
		const int iBuildingValue = buildingProperties->getValue(iI);

		std::map<int, int>::iterator itr = effectivePropertyChanges.find(eProperty);
		if (itr == effectivePropertyChanges.end())
		{
			effectivePropertyChanges[eProperty] = iBuildingValue;
		}
		else
		{
			itr->second += iBuildingValue;
		}
	}

	int iValue = 0;
	for (std::map<int, int>::const_iterator itr = effectivePropertyChanges.begin(); itr != effectivePropertyChanges.end(); ++itr)
	{
		iValue += getPropertySourceValue((PropertyTypes)itr->first, itr->second);
	}

	return iValue;
}

int CvCityAI::getPropertySourceValue(PropertyTypes eProperty, int iSourceValue) const
{
	if (iSourceValue == 0) return 0;

	const CvPropertyInfo& kProperty = GC.getPropertyInfo(eProperty);
	const int iOperationalLow = kProperty.getOperationalRangeMin();
	const int iOperationalHigh = kProperty.getOperationalRangeMax();
	const int iOperationalRange = (iOperationalHigh - iOperationalLow);
	if (iOperationalRange == 0) return 0;

	const CvProperties* cityProperties = getPropertiesConst();
	const int iCurrentValue = cityProperties->getValueByProperty(eProperty);
	const int iWouldBeValue = iCurrentValue + iSourceValue;

	// Normalize it to between zero and operational range
	int iCurrentNormalizedValue = (range(iCurrentValue, iOperationalLow, iOperationalHigh) - iOperationalLow) * 10000 / iOperationalRange;
	int iWouldBeNormalizedValue = (range(iWouldBeValue, iOperationalLow, iOperationalHigh) - iOperationalLow) * 10000 / iOperationalRange;

	int iValue = kProperty.getAIWeight() * (iWouldBeNormalizedValue - iCurrentNormalizedValue); // value is scaled up by 10 000 at this point.

	// Increase value as we move towards the nasty end of the range (cautious approach)
	int nastiness; // Toffer - Range from zero to iOperationalRange

	if (kProperty.getAIWeight() > 0)
	{
		nastiness = iOperationalRange - std::min(iCurrentNormalizedValue, iWouldBeNormalizedValue);
	}
	else
	{
		nastiness = std::max(iCurrentNormalizedValue, iWouldBeNormalizedValue);
	}

	// Toffer, if 0 nastiness, halve value, if max nastiness double value
	iValue = (iValue * (iOperationalRange/2 + nastiness) / iOperationalRange) / 100; // value is scaled up by 100 at this point.

	switch (kProperty.getAIScaleType())
	{
	case AISCALE_CITY:
		break;
	case AISCALE_AREA:
		iValue *= area()->getCitiesPerPlayer(getOwner());
		break;
	case AISCALE_PLAYER:
		iValue *= GET_PLAYER(getOwner()).getNumCities();
		break;
	case AISCALE_TEAM:
		iValue *= GET_TEAM(getTeam()).getNumCities();
		break;
	}

	// Don't trust the math above, so I don't assume iValue will always be above zero even though the property change is a good thing
	if (iSourceValue * kProperty.getAIWeight() > 0)
	{
		return std::max(1, iValue / 100);
	}
	// Bad property change
	return std::min(-1, iValue / 100);
}

int CvCityAI::getPropertyDecay(PropertyTypes eProperty) const
{
	PROFILE_EXTRA_FUNC();
	int	iTotalDecay = 0;
	const int current = getPropertiesConst()->getValueByProperty(eProperty);
	int iLowestThresholdDecay = 0;

	foreach_(const CvPropertySource * pSource, GC.getPropertyInfo(eProperty).getPropertyManipulators()->getSources())
	{
		if (pSource->getType() == PROPERTYSOURCE_DECAY)
		{
			const CvPropertySourceDecay* pDecaySource = static_cast<const CvPropertySourceDecay*>(pSource);

			//	For AI evaluation purposes the no-decay threshold is largely
			//	irrelevant since we are only interested in EVENTUAL equilibrium
			//	points.  If there are multiple decay sources we should count
			//	only those in current force OR the single lowest is none are
			int iNoDecayThreshold = pDecaySource->getNoDecayAmount();
			if (current > iNoDecayThreshold)
			{
				iTotalDecay += pDecaySource->getPercent();
			}
			else if (iTotalDecay == 0)
			{
				iLowestThresholdDecay = pDecaySource->getPercent();
			}
		}
	}

	if (iTotalDecay == 0)
	{
		iTotalDecay = iLowestThresholdDecay;
	}

	return iTotalDecay;
}

int CvCityAI::getPropertyNonBuildingSource(PropertyTypes eProperty) const
{
	PROFILE_EXTRA_FUNC();
	int	iTotal = 0;
	const int current = getPropertiesConst()->getValueByProperty(eProperty);

	foreach_(const CvPropertySource * pSource, GC.getPropertyInfo(eProperty).getPropertyManipulators()->getSources())
	{
		if (pSource->getType() == PROPERTYSOURCE_ATTRIBUTE_CONSTANT &&
			pSource->getObjectType() == GAMEOBJECT_CITY &&
			pSource->getProperty() == eProperty)
		{
			const CvPropertySourceAttributeConstant* pCitySource = static_cast<const CvPropertySourceAttributeConstant*>(pSource);

			iTotal += pCitySource->getSourcePredict(getGameObject(), current);
		}
	}

	//	Add in contribution from specialists
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		const int iNum = getSpecialistCount((SpecialistTypes)iI) + getFreeSpecialistCount((SpecialistTypes)iI);

		if (iNum > 0)
		{
			int iContribution = 0;

			foreach_(const CvPropertySource * pSource, GC.getSpecialistInfo((SpecialistTypes)iI).getPropertyManipulators()->getSources())
			{
				//	Sources that deliver to the city or the plot are both considered since the city plot diffuses
				//	to the city for most properties anyway
				if (pSource->getType() == PROPERTYSOURCE_CONSTANT &&
					(pSource->getObjectType() == GAMEOBJECT_CITY || pSource->getObjectType() == GAMEOBJECT_PLOT) &&
					pSource->getProperty() == eProperty)
				{
					iContribution += static_cast<const CvPropertySourceConstant*>(pSource)->getAmountPerTurn(getGameObject());
				}
			}

			iTotal += iNum * iContribution;
		}
	}

	//	Add oplayer-based/global sources (population, traits, handicap, ...)
	iTotal += getGlobalSourcedProperty(eProperty);

	return iTotal;
}

int CvCityAI::AI_getBuildPriority() const
{
	return m_iBuildPriority;
}

bool CvCityAI::AI_choosePropertyControlBuildingAndUnit(int iTriggerPercentOfPropertyOpRange, PropertyTypes pProperty, int eMaxPropertyUnitsPercent)
{
	PROFILE_EXTRA_FUNC();
	if (AI_isPropertyControlBuildingQueued())
	{
		return false;
	}

	bool bSuccessful = false;
	bool bAnySuccessful = false;
	CvPlot* pPlot = plot();
	const PlayerTypes eOwner = getOwner();
	CvPlayerAI& player = GET_PLAYER(eOwner);
	const CvArea* pArea = area();


	int iGameTurn = GC.getGame().getGameTurn();

	for (int iI = 0; iI < GC.getNumPropertyInfos(); iI++)
	{
		PropertyTypes eProperty = (PropertyTypes)iI;

		if (pProperty == NO_PROPERTY || pProperty == eProperty)
		{
			if (GC.getPropertyInfo(eProperty).getAIWeight() != 0 && GC.getPropertyInfo(eProperty).isSourceDrain())
			{
				int iCurrentValue = getPropertiesConst()->getValueByProperty(eProperty);
				int iCurrentChange = getPropertiesConst()->getChangeByProperty(eProperty);
				int iOperationRangeMin = GC.getPropertyInfo(eProperty).getOperationalRangeMin();
				int iOperationRangeMax = GC.getPropertyInfo(eProperty).getOperationalRangeMax();
				int iCurrentPercent = ((iCurrentValue - iOperationRangeMin) * 100) / (iOperationRangeMax - iOperationRangeMin);

				int iEval = getPropertyNeed(eProperty);
				int iTrainReluctance = GC.getPropertyInfo(eProperty).getTrainReluctance();
				int iCheck = iTriggerPercentOfPropertyOpRange;
				iEval /= iCheck;
				iCheck *= iTrainReluctance;
				//TBNote: May still need to take a count of units ordered... there's only so many units the AI should be willing to train at once for some tasks.  I'm thinking of polution control here.
				//	Don't bother trying to build units for hopelessly out-of-control properties
				//	or else we'll spam units endlessly in cases we cannot really control
				int iLimit = 250 * ((1 + (int)player.getCurrentEra())/5);
				int iAcceptanceLimit = 100 * ((1 + (int)player.getCurrentEra())/2); //Acceptable Value 
				int iAcceptanceRate = iCurrentValue/100 * 5; //Accept a 5% rate increase
				if (iTrainReluctance > 0)
				{
					iLimit /= iTrainReluctance;
				}

				//Calvitix : Evaluate the future evolution of the property, with actuel change, and estimate the number of turn to achieve goal 
				#define MIN_PROPERTY_TOTAL_TO_DEAL_WITH  -1000
				bool isGettingBetter = false;
				bool isGoodEnough = false;
				bool ismaxPropUnitsReached = false;
				//iCurrentChange /= iTrainReluctance;
				CvWString szProperty = GC.getPropertyInfo(eProperty).getType();
				CvWString szPropertyEduRef = "PROPERTY_EDUCATION";
				CvWString szPropertyTourismRef = "PROPERTY_TOURISM"; //Calvitix TODO : replace with AIWeight() / 50 - to get the sign of property

				if ((iCurrentChange < 0 && szProperty != szPropertyEduRef && szProperty != szPropertyTourismRef)
					|| (iCurrentChange > 0 && (szProperty == szPropertyEduRef || szProperty == szPropertyTourismRef)))
				{	//First Test : If getting better (Change < 0).
					//For Education, its the opposite
					isGettingBetter = true;

					if ((iCurrentValue > 0 && szProperty != szPropertyEduRef && szProperty != szPropertyTourismRef) || (iCurrentValue < 0 && (szProperty == szPropertyEduRef || szProperty == szPropertyTourismRef)))
					{
						logAiEvaluations(3, "City %S, step %d %%, worried about property %S. But things are going better value(%d) change(%d)", getName().GetCString(), iTriggerPercentOfPropertyOpRange, szProperty.GetCString(), iCurrentValue, iCurrentChange);
					}

				}

				// if its lowering, but on a sufficient level (will be deal later)
				if ((iCurrentChange > 0 && iCurrentChange < iAcceptanceRate && iCurrentValue < (iAcceptanceLimit * -1) && szProperty != szPropertyEduRef && szProperty != szPropertyTourismRef)
					|| ((iCurrentChange < 0 && iCurrentChange > (iAcceptanceRate * -1) && iCurrentValue > iAcceptanceLimit) && (szProperty == szPropertyEduRef || szProperty == szPropertyTourismRef)))
				{

					logAiEvaluations(3, "City %S, step %d %%, property %S can be a problem in future, but not now. value(%d) change(%d)", getName().GetCString(), iTriggerPercentOfPropertyOpRange, szProperty.GetCString(), iCurrentValue, iCurrentChange);

				}
				
				//Count the percent of Prop Control units in the total army. Maximal allowed : 20% (TODO set param depend on leader)
				int iPropControlInArea = player.AI_totalAreaUnitAIs(pArea, UNITAI_PROPERTY_CONTROL);
				int iUnitsInArea = player.getNumUnits();
				if ((iPropControlInArea * 100 / (iUnitsInArea+1)) > eMaxPropertyUnitsPercent)
				{
					ismaxPropUnitsReached = true;
					logAiEvaluations(3, "City %S, step %d %%, worried about property %S. But Maximal Prop Control reached :  value(%d) / total units (%d)", getName().GetCString(), iTriggerPercentOfPropertyOpRange, szProperty.GetCString(), iPropControlInArea, iUnitsInArea);
				}

				if (iEval > iCheck && iCurrentPercent < iLimit && !isGettingBetter && !isGoodEnough && !ismaxPropUnitsReached)
				{
					//	Order a suitable unit if possible
					CvUnitSelectionCriteria criteria;
					CvString	str;

					criteria.m_eProperty = eProperty;
					criteria.m_bPropertyBeneficial = true;

					int iTotalProperty = player.AI_getTotalProperty(pProperty);
					if (iTotalProperty < MIN_PROPERTY_TOTAL_TO_DEAL_WITH)
					logAiEvaluations(3, "City %S, step %d %%, worried about property %S : value(%d) change(%d). But Global Property is so low (%d), it's not a priority.", getName().GetCString(), iTriggerPercentOfPropertyOpRange, szProperty.GetCString(), iCurrentValue, iCurrentChange, iTotalProperty);


					CvWString szProperty = GC.getPropertyInfo(eProperty).getType();
					logAiEvaluations(3, "City %S, step %d %%, worried about property %S : value(%d) change(%d). Trying to Order something", getName().GetCString(), iTriggerPercentOfPropertyOpRange, szProperty.GetCString(), iCurrentValue, iCurrentChange);


					//eventually let's get coastal cities ordering some coast guard contracts.
					if (AI_isNavalMilitaryProductionCity())
					{
						if (m_iUnitSummonedLastEstablishedTurn != iGameTurn)
						{
							int iResponders = GET_PLAYER(getOwner()).getContractBroker().numRequestsOutstanding(UNITAI_PROPERTY_CONTROL_SEA, false, pPlot);
							if (iResponders < 1)
							{
								str.Format("Property control for %S", GC.getPropertyInfo(eProperty).getDescription());
								bSuccessful = AI_chooseUnitImmediate(str.c_str(), UNITAI_PROPERTY_CONTROL_SEA, &criteria);
								if (bSuccessful)
								{
									bAnySuccessful = true;
									m_iUnitSummonedLastEstablishedTurn = iGameTurn;
								}
							}
						}
					}
					if (!bSuccessful)
					{
						//First try for a building that's quick to build and has a
						if (AI_chooseBuilding(BUILDINGFOCUS_PROPERTY, 4, 0, -1, true, eProperty))
						{
							LOG_BBAI_CITY(2, ("      City %S selects a property control building", getName().GetCString()));
							AI_setPropertyControlBuildingQueued(true);
							return true;
						}

						if (m_iUnitSummonedLastEstablishedTurn != iGameTurn)
						{
							int iResponders = GET_PLAYER(getOwner()).getContractBroker().numRequestsOutstanding(UNITAI_PROPERTY_CONTROL, false, pPlot);
							if (iResponders < 2)//if there aren't already a couple or more property control units being trained and/or on the way
							{
								//then lets order the best type we can get from any city we have:
								str.Format("Contracts Property control for %S", GC.getPropertyInfo(eProperty).getDescription());
								bSuccessful = AI_chooseUnit(str.c_str(), UNITAI_PROPERTY_CONTROL, -1, -1, HIGH_PRIORITY_ESCORT_PRIORITY, &criteria);
							}
							else
							{
								//otherwise, let's see what we can build here in this city and let's train that.
								//if this causes us to overtrain a little then it won't hurt too much and should only happen because the city is super effective.
								int iUnitValue = 0;
								UnitTypes eBestUnit = AI_bestUnitAI(UNITAI_PROPERTY_CONTROL, iUnitValue, false, false, &criteria);
								if (eBestUnit != NO_UNIT)
								{
									str.Format("Local Property control for %S", GC.getPropertyInfo(eProperty).getDescription());
									bSuccessful = AI_chooseUnitImmediate(str.c_str(), UNITAI_PROPERTY_CONTROL, &criteria, eBestUnit);
								}
							}
							if (bSuccessful)
							{
								bAnySuccessful = true;
								m_iUnitSummonedLastEstablishedTurn = iGameTurn;
							}
						}
					}
				}
			}
		}
	}

	return bAnySuccessful;
}

bool CvCityAI::AI_chooseHealerUnit(int iMinNeeded)
{
	PROFILE_EXTRA_FUNC();
	int iBestUnitCombatValue = 0;
	UnitCombatTypes eBestUnitCombat = NO_UNITCOMBAT;
	int iSecondBestUnitCombatValue = 0;
	UnitCombatTypes eSecondBestUnitCombat = NO_UNITCOMBAT;
	DomainTypes eBestDomain = NO_DOMAIN;
	DomainTypes eSecondBestDomain = NO_DOMAIN;

	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		if (GC.getUnitCombatInfo((UnitCombatTypes)iI).isHealsAs())
		{
			UnitCombatTypes eUnitCombat = (UnitCombatTypes)iI;
			PlayerTypes ePlayer = getOwner();
			int iOverloadCount = 0;
			int iOverloadCountLand = 0;
			int iOverloadCountSea = 0;
			int iCount = 0;
			int iCountLand = 0;
			int iCountSea = 0;
			CvPlot* pLoopPlot;
			for (int iJ = 0; iJ < NUM_CITY_PLOTS_1; iJ++)
			{
				pLoopPlot = plotCity(getX(), getY(), iJ);
				if (pLoopPlot != NULL)
				{
					for (int iK = 0; iK < NUM_DOMAIN_TYPES; iK++)
					{
						DomainTypes eDomain = (DomainTypes)iK;
						int iOverload = pLoopPlot->getOverloadedUnitCombatsSupportedByHealer(ePlayer, eUnitCombat, eDomain);
						iOverloadCount += iOverload;
						if (eDomain == DOMAIN_LAND)
						{
							iOverloadCountLand += iOverload;
						}
						else if (eDomain == DOMAIN_SEA)
						{
							iOverloadCountSea += iOverload;
						}
						else if (eDomain == DOMAIN_AIR)
						{
							if (pLoopPlot->isWater())
							{
								iOverloadCountSea += iOverload;
							}
							else
							{
								iOverloadCountLand += iOverload;
							}
						}

						int iResult = pLoopPlot->getUnitCombatsUnsupportedByHealer(ePlayer, eUnitCombat, eDomain);
						iCount += iResult;
						if (eDomain == DOMAIN_LAND)
						{
							iCountLand += iResult;
						}
						else if (eDomain == DOMAIN_SEA)
						{
							iCountSea += iResult;
						}
						else if (eDomain == DOMAIN_AIR)
						{
							if (pLoopPlot->isWater())
							{
								iCountSea += iResult;
							}
							else
							{
								iCountLand += iResult;
							}
						}
					}
				}
			}
			iCount -= iOverloadCount;
			iCountSea -= iOverloadCountSea;
			iCountLand -= iOverloadCountLand;
			if (iCount >= iMinNeeded)
			{
				if (iCount > iBestUnitCombatValue)
				{
					iBestUnitCombatValue = iCount;
					eBestUnitCombat = eUnitCombat;
					if (iCountLand >= iCountSea)
					{
						eBestDomain = DOMAIN_LAND;
					}
					else
					{
						eBestDomain = DOMAIN_SEA;
					}
				}
				if ((iCount > iSecondBestUnitCombatValue) && iCount < iBestUnitCombatValue)
				{
					iSecondBestUnitCombatValue = iCount;
					eSecondBestUnitCombat = eUnitCombat;
					if (iCountLand >= iCountSea)
					{
						eSecondBestDomain = DOMAIN_LAND;
					}
					else
					{
						eSecondBestDomain = DOMAIN_SEA;
					}
				}
			}
		}
	}
	bool bSuccessful = false;
	if (eBestUnitCombat != NO_UNITCOMBAT)
	{
		//	Order a suitable unit if possible
		CvUnitSelectionCriteria criteria;
		CvString	str;

		criteria.m_eHealUnitCombat = eBestUnitCombat;
		criteria.m_bIsHealer = true;

		if (eBestDomain == DOMAIN_SEA && AI_isNavalMilitaryProductionCity())
		{
			str.Format("Healer for best selection: %S", GC.getUnitCombatInfo(eBestUnitCombat).getDescription());
			bSuccessful = AI_chooseUnit(str.c_str(), UNITAI_HEALER_SEA, -1, -1, -1, &criteria);
		}
		if (!bSuccessful && eBestDomain == DOMAIN_LAND)
		{
			str.Format("Healer for best selection: %S", GC.getUnitCombatInfo(eBestUnitCombat).getDescription());
			bSuccessful = AI_chooseUnit(str.c_str(), UNITAI_HEALER, -1, -1, -1, &criteria);
		}
	}
	if (!bSuccessful && eSecondBestUnitCombat != NO_UNITCOMBAT)
	{
		//	Order a suitable unit if possible
		CvUnitSelectionCriteria criteria;
		CvString	str;

		criteria.m_eHealUnitCombat = eSecondBestUnitCombat;
		criteria.m_bIsHealer = true;

		if (eSecondBestDomain == DOMAIN_SEA && AI_isNavalMilitaryProductionCity())
		{
			str.Format("Healer for best selection: %S", GC.getUnitCombatInfo(eSecondBestUnitCombat).getDescription());
			bSuccessful = AI_chooseUnit(str.c_str(), UNITAI_HEALER_SEA, -1, -1, -1, &criteria);
		}
		if (!bSuccessful && eSecondBestDomain == DOMAIN_LAND)
		{
			str.Format("Healer for best selection: %S", GC.getUnitCombatInfo(eSecondBestUnitCombat).getDescription());
			bSuccessful = AI_chooseUnit(str.c_str(), UNITAI_HEALER, -1, -1, -1, &criteria);
		}
	}
	return bSuccessful;
}

int CvCityAI::happynessValue(int iAddedHappyness, int iBaseHappinessLevel, int iBaseHealthLevel) const
{
	PROFILE_EXTRA_FUNC();
	bool bIsNegative = false;

	if (iAddedHappyness < 0)
	{
		bIsNegative = true;
		iAddedHappyness = -iAddedHappyness;
	}

	int iValue = 0;

	if (iAddedHappyness != 0)
	{
		int iTempHealthLevel = iBaseHealthLevel - (getEspionageHealthCounter() / 2) + std::max(0, iAddedHappyness);
		int iTempHappinessLevel = iBaseHappinessLevel - (getEspionageHappinessCounter() / 2);
		int iTempCurrentHappinessLevel = iBaseHappinessLevel;
		int iTempHappyChangeValue = 0;
		for (int iI = 0; iI < iAddedHappyness; iI++)
		{
			if (bIsNegative) { iTempHappinessLevel--; iTempCurrentHappinessLevel--; iTempHappyChangeValue += 1; }

			//The Value of Happiness
			if (iTempHappinessLevel < 0)
			{
				iTempHappyChangeValue += 10;
				if (bIsNegative) {
					iTempHappyChangeValue += 9;
					if (iTempCurrentHappinessLevel < 0)
					{
						iTempHappyChangeValue += 10;
					}
				}
			}
			else if (iTempHappinessLevel <= iTempHealthLevel && iTempHappinessLevel <= 6) {
				iTempHappyChangeValue += 6;
				if (bIsNegative) { iTempHappyChangeValue += 3; }
			}
			else if (iTempHappinessLevel < 10) { iTempHappyChangeValue += 3; }
			else { iTempHappyChangeValue += 1; }

			if (!bIsNegative) { iTempHappinessLevel++; iTempCurrentHappinessLevel++; }
		}

		if (bIsNegative)
		{
			iValue -= iTempHappyChangeValue;

			//	If we're already unhappy then exach extar unhappy will force
			//	use to remove a worked plot as well
			if (iBaseHappinessLevel <= 0)
			{
				iValue -= (iAddedHappyness * worstWorkedPlotValue()) / 100;
			}
		}
		else
		{
			iValue += iTempHappyChangeValue;
		}
	}

	return iValue;
}

int CvCityAI::healthValue(int iAddedHealth, int iUseHappinessLevel, int iBaseHealthLevel, int iBaseFoodDifference) const
{
	PROFILE_EXTRA_FUNC();
	bool bIsNegative = false;
	if (iAddedHealth < 0)
	{
		bIsNegative = true;
		iAddedHealth = -iAddedHealth;
	}
	int iValue = 0;
	if (iAddedHealth != 0)
	{
		int iTempHappinessLevel = iUseHappinessLevel;
		int iTempHealthLevel = (goodHealth() - badHealth(/*bNoAngry*/ false, std::max(0, (iTempHappinessLevel + 1) / 2))) + ((getEspionageHealthCounter() + 1) / 2);
		int iTempCurrentHealthLevel = iBaseHealthLevel;
		int iTempHealthChangeValue = 0;
		for (int iI = 0; iI < iAddedHealth; iI++)
		{
			if (bIsNegative) { iTempHealthLevel--; iTempCurrentHealthLevel--; iTempHealthChangeValue += 1; }

			//Health Values
			if (iTempCurrentHealthLevel < 0) {
				iTempHealthChangeValue += 10;
				if (bIsNegative) {
					iTempHealthChangeValue += 9;
					if (std::max(0, iBaseFoodDifference - 1) < -iTempCurrentHealthLevel)
					{
						iTempHealthChangeValue += 10;
					}
				}
			}
			else if (iTempHealthLevel < 0) { iTempHealthChangeValue += 8; }
			else if (iTempHealthLevel < iTempHappinessLevel && iTempHealthLevel <= 4) {
				iTempHealthChangeValue += 4;
				if (bIsNegative) { iTempHealthChangeValue += 1; }
			}
			else if (iTempHealthLevel < 8) { iTempHealthChangeValue += 2; }
			else if (!bIsNegative) { iTempHealthChangeValue += 1; }

			if (!bIsNegative) { iTempHealthLevel++; iTempCurrentHealthLevel++; }
		}
		if (bIsNegative)
		{
			iValue -= iTempHealthChangeValue;
		}
		else
		{
			iValue += iTempHealthChangeValue;
		}
	}

	return iValue;
}

int CvCityAI::worstWorkedPlotValue() const
{
	PROFILE_EXTRA_FUNC();
	int iWorstPlotValue = MAX_INT;

	for (int iI = SKIP_CITY_HOME_PLOT; iI < NUM_CITY_PLOTS; iI++)
	{
		if (isWorkingPlot(iI))
		{
			const CvPlot* pLoopPlot = getCityIndexPlot(iI);

			if (pLoopPlot != NULL)
			{
				const int iValue = AI_plotValue(pLoopPlot, false, /*bRemove*/ false, /*bIgnoreFood*/ false, false);

				if (iValue < iWorstPlotValue)
				{
					iWorstPlotValue = iValue;
				}
			}
		}
	}

	return (iWorstPlotValue == MAX_INT ? 0 : iWorstPlotValue);
}

bool CvCityAI::AI_establishSeeInvisibleCoverage()
{
	PROFILE_EXTRA_FUNC();
	const CvPlot* pPlot = plot();

	if (pPlot != NULL)
	{
		for (int iI = 0; iI < GC.getNumInvisibleInfos(); iI++)
		{
			const InvisibleTypes eVisible = (InvisibleTypes)iI;
			CvUnitSelectionCriteria criteria;
			criteria.m_eVisibility = eVisible;
			criteria.m_bNoNegativeProperties = true;

			//continue to use this best unit method so as to establish a rare coastal response.
			//if (plot()->isCoastalLand())
			//{
			//	if (!isCachedBestSeeInvisibleUnit(eVisible, true))
			//	{
			//		eBestUnit = AI_bestUnitAI(UNITAI_SEE_INVISIBLE_SEA, iUnitValue, false, false, &criteria);
			//		setBestSeeInvisibleUnitCache(eVisible, eBestUnit, true);
			//	}
			//	else
			//	{
			//		eBestUnit = getCachedBestSeeInvisibleUnit(eVisible, true);
			//	}
			//
			//	if (eBestUnit != NULL)
			//	{
			//		iResponders = GET_PLAYER(getOwner()).getContractBroker().numRequestsOutstanding(UNITAI_SEE_INVISIBLE_SEA, false, eBestUnit, pPlot);
			//		iResponders += pPlot->countSeeInvisibleActive(getOwner(), eVisible);
			//		if (iResponders < 1)
			//		{
			//			//end result
			//			if (AI_chooseUnit("See Invisible Sea needed", UNITAI_SEE_INVISIBLE_SEA, -1, -1, -1, &criteria, eBestUnit) )
			//			{
			//				return true;
			//			}
			//		}
			//	}
			//}
			//if (!isCachedBestSeeInvisibleUnit(eVisible))
			//{
			//	eBestUnit = AI_bestUnitAI(UNITAI_SEE_INVISIBLE, iUnitValue, false, false, &criteria);
			//	setBestSeeInvisibleUnitCache(eVisible, eBestUnit);
			//}
			//else
			//{
			//	eBestUnit = getCachedBestSeeInvisibleUnit(eVisible);
			//}
			//
			//if (eBestUnit != NO_UNIT)
			//{
			int iResponders = GET_PLAYER(getOwner()).getContractBroker().numRequestsOutstanding(UNITAI_SEE_INVISIBLE, false, pPlot);//This will tend to space out the AI's sense of immediate need to prepare for each type.
			int iActiveResponders = pPlot->countSeeInvisibleActive(getOwner(), eVisible);
			if ((iResponders+ iActiveResponders) < 1)
			{
				LOG_CITY_BLOCK(3, {
					CvWString szDesc = GC.getInvisibleInfo(eVisible).getDescription();
					logAiEvaluations(2, "City %S, Not enough 'UNITAI_SEE_INVISIBLE' (for visibility : %S) here : responders %d, active %d", getName().GetCString(), szDesc.GetCString(), iResponders, iActiveResponders);
				});
				//end result
				//const int iMaxSeeInvisibleUnits = 2 + player.getNumCities() + intSqrt(player.getNumCities() * intSqrt(eCurrentEra));



				//Calvitix Test. demand an See Invisible only in 20% of cases
				int irandomaction = GC.getGame().getSorenRandNum(10, "AI See Invisible Invoqued");

				if (irandomaction > 7 && AI_chooseUnit("See Invisible needed", UNITAI_SEE_INVISIBLE, -1, -1, -1, &criteria))
				{
					LOG_CITY_BLOCK(3, {
						CvWString szDesc = GC.getInvisibleInfo(eVisible).getDescription();
						CvWString szDesccriteria = criteria.getDescription();
						logAiEvaluations(2, "City %S, Unit has been chosen for 'UNITAI_SEE_INVISIBLE' (for visibility : %S), criteria : %S", getName().GetCString(), szDesc.GetCString(), szDesccriteria.GetCString());
					});
					return true;
				}
				else if (irandomaction <= 7)
				{
					LOG_CITY_BLOCK(3, {
						CvWString szDesc = GC.getInvisibleInfo(eVisible).getDescription();
						CvWString szDesccriteria = criteria.getDescription();
						logAiEvaluations(2, "City %S, Unit for 'UNITAI_SEE_INVISIBLE' (for visibility : %S) , criteria : %S, needed, but random fail.", getName().GetCString(), szDesc.GetCString(), szDesccriteria.GetCString());
					});
				}

			}
			else
			{
				LOG_CITY_BLOCK(3, {
					CvWString szDesc = GC.getInvisibleInfo(eVisible).getDescription();
					logAiEvaluations(2, "City %S, There is enough 'UNITAI_SEE_INVISIBLE' (for visibility : %S) here : responders %d, active %d", getName().GetCString(), szDesc.GetCString(), iResponders, iActiveResponders);
				});
			}
		}
	}
	return false;
}

bool CvCityAI::AI_establishInvestigatorCoverage()
{
	//Fox in the henhouse protocol
	CvPlot* pPlot = plot();

	const int iNumLocalCriminals = pPlot->getNumCriminals();

	if (iNumLocalCriminals > 0) // Yes, the first round the city has trained its first criminal it will run into this, even if no spawns ever occur. Can never be too ready, right?
	{
		//First try for a building that's quick to build and has investigation
		if (AI_chooseBuilding(BUILDINGFOCUS_INVESTIGATION, iNumLocalCriminals * 2, 0, -1, true))
		{
			LOG_BBAI_CITY(2, ("      City %S selects an investigation enhancing building", getName().GetCString()));
			return true;
		}
		CvUnitSelectionCriteria criteria;
		criteria.m_bNoNegativeProperties = true;

		// MISSIONAI_INVESTIGATOR_MAINTAIN is probably not working as designed due to the way the contract broker works.
		//	Once answered, the unit can really only be tracked by its AI type as mission type has been generically set.
		const int iLocalInvestigators = (
			GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(pPlot, MISSIONAI_INVESTIGATOR_MAINTAIN, NULL, 0)
			+
			pPlot->getNumPlayerUnitAI(UNITAI_INVESTIGATOR, getOwner())
			+
			GET_PLAYER(getOwner()).getContractBroker().numRequestsOutstanding(UNITAI_INVESTIGATOR, false, pPlot)
			);

		if (iLocalInvestigators < iNumLocalCriminals && iLocalInvestigators < 1
			&& AI_chooseUnit("Investigator needed", UNITAI_INVESTIGATOR, -1, -1, -1, &criteria))
		{
			return true;
		}
	}
	return false;
}

bool CvCityAI::AI_isNegativePropertyUnit(UnitTypes eUnit) const
{
	PROFILE_EXTRA_FUNC();
	if (eUnit == NO_UNIT)
	{
		return false;
	}

	const CvPropertyManipulators* propertyManipulators = GC.getUnitInfo(eUnit).getPropertyManipulators();
	if (propertyManipulators != NULL)
	{
		foreach_(const CvPropertySource * pSource, propertyManipulators->getSources())
		{
			//	We have a source for a property - value is crudely just the AIweight of that property times the source size (which is expected to only depend on the player)
			if (pSource->getType() == PROPERTYSOURCE_CONSTANT)
			{
				if (GC.getPropertyInfo(pSource->getProperty()).getAIWeight() < 0)
				{
					if (static_cast<const CvPropertySourceConstant*>(pSource)->getAmountPerTurn(getGameObject()) > 0)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool CvCityAI::AI_meetsUnitSelectionCriteria(UnitTypes eUnit, const CvUnitSelectionCriteria* criteria) const
{
	PROFILE_EXTRA_FUNC();
	//Add more here as needs demand - Some cleanup could be nice too.  Consolidate some of the other checks and possible redundancies into this location?
	if (eUnit != NO_UNIT && criteria)
	{
		//No Negative Properties
		if (criteria->m_bNoNegativeProperties && AI_isNegativePropertyUnit(eUnit))
		{
			return false;
		}
		//See Invisible type match
		if (criteria->m_eUnitAI == UNITAI_SEE_INVISIBLE || criteria->m_eUnitAI == UNITAI_SEE_INVISIBLE_SEA)
		{
			const CvUnitInfo& kUnitInfo = GC.getUnitInfo(eUnit);

			if (kUnitInfo.isFound())
			{
				return false;
			}

			if (!GC.getGame().isOption(GAMEOPTION_COMBAT_HIDE_SEEK))
			{
				int iVisibilityRequested = (int)criteria->m_eVisibility;
				bool bFound = false;
				for (int iI = 0; iI < kUnitInfo.getNumSeeInvisibleTypes(); ++iI)
				{
					if (kUnitInfo.getSeeInvisibleType(iI) == iVisibilityRequested)
					{
						bFound = true;
						break;
					}
				}
				if (!bFound)
				{
					return false;
				}
			}
			else if (kUnitInfo.getVisibilityIntensityType(criteria->m_eVisibility) <= 0)
			{
				return false;
			}
		}
	}
	return true;
}


int CvCityAI::AI_getBuildingYieldValue
(
	BuildingTypes eBuilding, const CvBuildingInfo& kBuilding, bool bIsLimitedWonder, bool bForeignTrade, bool bFinancialTrouble,
	int* aiFreeSpecialistYield, int* aiYieldRank, int iLimitedWonderLimit, const CvArea* pArea, int iTotalPopulation, int iFoodDifference
)
const {
	PROFILE_FUNC();

	const CvPlayerAI& kOwner = GET_PLAYER(getOwner());
	int iValue = 0;
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		int iYieldValue = 0;

		if (iFoodDifference > 0)
		{
			iValue += kBuilding.getFoodKept() / 2;
		}
		iValue += AI_buildingYieldValue((YieldTypes)iI, eBuilding, kBuilding, bForeignTrade, aiFreeSpecialistYield[iI]);
		{
			int iGlobalYieldModValue = 0;

			if (kBuilding.getGlobalYieldModifier(iI) > 0 || kBuilding.getAreaYieldModifier(iI) > 0)
			{
				foreach_(const CvCity * pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
				{
					iGlobalYieldModValue +=
					(
						pLoopCity->getPlotYield((YieldTypes)iI)
						*
						(
							kBuilding.getGlobalYieldModifier(iI)
							+
							(pLoopCity->area() == pArea ? kBuilding.getAreaYieldModifier(iI) : 0)
						)
					);
				}
			}
			if (iGlobalYieldModValue > 0)
			{
				iYieldValue += std::max(1, iGlobalYieldModValue / 12);
			}
			else if (iGlobalYieldModValue < 0)
			{
				iYieldValue += std::min(-1, iGlobalYieldModValue / 12);
			}
		}
		for (int iJ = GC.getNumSpecialistInfos() - 1; iJ > -1; iJ--)
		{
			iYieldValue += kBuilding.getSpecialistYieldChange(iJ, iI) * iTotalPopulation / 5;
		}

		if (iYieldValue != 0)
		{
			if (bFinancialTrouble && iI == YIELD_COMMERCE)
			{
				iYieldValue *= 2;
			}

			iYieldValue *= kOwner.AI_yieldWeight((YieldTypes)iI);
			iYieldValue /= 100;

			if (aiYieldRank[iI] == MAX_INT)
			{
				aiYieldRank[iI] = findBaseYieldRateRank((YieldTypes)iI);
			}

			// if this is a limited wonder, and we are not one of the top 4 in this category, subtract the value
			// we do _not_ want to build this here (unless the value was small anyway)
			// Exempt unit build from this test however
			if (bIsLimitedWonder && canConstruct(eBuilding) && (aiYieldRank[iI] > (3 + iLimitedWonderLimit)))
			{
				iYieldValue *= -1;
			}
			iValue += iYieldValue;
		}
	}
	return iValue;
}

int CvCityAI::AI_getBuildingProductionValue
(
	BuildingTypes eBuilding, const CvBuildingInfo& kBuilding, bool bIsLimitedWonder,
	bool bForeignTrade, int iFreeSpecialistYield, int iYieldRank, int iLimitedWonderLimit
)
const {
	PROFILE_FUNC();

	int directYieldValue = AI_buildingYieldValue(YIELD_PRODUCTION, eBuilding, kBuilding, bForeignTrade, iFreeSpecialistYield);

	// If this is a limited wonder, and we are not one of the top 4 in this category, subtract the value
	// we do _not_ want to build this here (unless the value was small anyway)
	if (bIsLimitedWonder && iYieldRank > 3 + iLimitedWonderLimit)
	{
		directYieldValue *= -1;
	}
	int iValue = directYieldValue;

	// Koshling - modify direct production value if the base yield rate for this city (for production)
	//	is low in relation to the amount this would add
	//	i.e. - boost the value of extra production especially when we don't already have much
	if (directYieldValue > getYieldRate(YIELD_PRODUCTION) / 3)
	{
		// directYieldValue is roughly 8*<direct production gain>, so if that implies a net gain of 4% (ish) or more boost the value
		int iBoost = directYieldValue * 12 - getYieldRate(YIELD_PRODUCTION) * 4;

		// Examples:
		//	1) City production is 1, building adds 1 (directYieldValue 8), so iBoost is 92 - value almost trebbled
		//	2) Same building when production is already 10, iBoost is 56 - value roughly doubled
		//	3) Same building when production >= 24 - no boost
		directYieldValue += (directYieldValue * iBoost) / 50;
	}
	return iValue + directYieldValue;
}


int CvCityAI::AI_getBuildingCommerceValue
(
	BuildingTypes eBuilding, const CvBuildingInfo& kBuilding, bool bIsLimitedWonder, bool bFinancialTrouble, bool bCulturalVictory1,
	int* aiFreeSpecialistYield, int* aiFreeSpecialistCommerce, int* aiBaseCommerceRate, int* aiPlayerCommerceRate, int* aiCommerceRank,
	int iLimitedWonderLimit, int iTotalPopulation
)
const {
	PROFILE_FUNC();

	const CvPlayerAI& kOwner = GET_PLAYER(getOwner());
	int iValue = 0;
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		int directCommerceValue = getBuildingCommerceValue(eBuilding, iI, aiFreeSpecialistYield, aiFreeSpecialistCommerce, aiBaseCommerceRate, aiPlayerCommerceRate);

		for (int iJ = GC.getNumSpecialistInfos() - 1; iJ > -1; iJ--)
		{
			directCommerceValue += kBuilding.getSpecialistCommerceChange(iJ, iI) * iTotalPopulation / 6;
		}
		if (directCommerceValue != 0)
		{
			// Make sure we don't reduce 1 to 0! TEST TEST
			if (directCommerceValue > 1) directCommerceValue /= 2;

			if (bFinancialTrouble && iI == COMMERCE_GOLD)
			{
				directCommerceValue *= 2;
			}

			directCommerceValue *= kOwner.AI_commerceWeight(static_cast<CommerceTypes>(iI), this);
			directCommerceValue = (directCommerceValue + 99) / 100; // Round up

			// if this is a limited wonder, and we are not one of the top 4 in this category, subtract the value
			// we do _not_ want to build this here (unless the value was small anyway)
			if (bIsLimitedWonder)
			{
				if (MAX_INT == aiCommerceRank[iI])
				{
					aiCommerceRank[iI] = findCommerceRateRank(static_cast<CommerceTypes>(iI));
				}
				if (aiCommerceRank[iI] > 3 + iLimitedWonderLimit
					|| bCulturalVictory1 && iI == COMMERCE_CULTURE && aiCommerceRank[iI] == 1)
				{
					// for culture, just set it to zero, not negative, just about every wonder gives culture
					if (iI == COMMERCE_CULTURE)
					{
						directCommerceValue = 0;
					}
					else directCommerceValue *= -1;
				}
			}
			iValue += directCommerceValue;
		}
	}
	return iValue;
}

int CvCityAI::AI_evaluateMaxUnitSpending() const
{
	bool bAggressiveAI = GC.getGame().isOption(GAMEOPTION_AI_AGGRESSIVE);
	int iBuildUnitProb = AI_buildUnitProb();
	const PlayerTypes eOwner = getOwner();
	CvPlayerAI& player = GET_PLAYER(eOwner);
	const CvArea* pArea = area();
	const AreaAITypes eAreaAI = pArea->getAreaAIType(getTeam());
	const bool bDefenseWar = eAreaAI == AREAAI_DEFENSIVE;

	int iMaxUnitSpending = (bAggressiveAI ? 6 : 3) + iBuildUnitProb / 3;
	if (player.AI_isDoVictoryStrategy(AI_VICTORY_CONQUEST4))
	{
		iMaxUnitSpending += 7;
	}
	else if (player.AI_isDoVictoryStrategy(AI_VICTORY_CONQUEST3) || player.AI_isDoVictoryStrategy(AI_VICTORY_DOMINATION3))
	{
		iMaxUnitSpending += 3;
	}
	else if (player.AI_isDoVictoryStrategy(AI_VICTORY_CONQUEST1))
	{
		iMaxUnitSpending += 1;
	}

	if (player.AI_isDoStrategy(AI_STRATEGY_FINAL_WAR))
	{
		iMaxUnitSpending = 5 + iMaxUnitSpending + (100 - iMaxUnitSpending) / 2;
	}
	else
	{
		iMaxUnitSpending += bDefenseWar ? 4 : 0;
		switch (eAreaAI)
		{
		case AREAAI_OFFENSIVE:
			iMaxUnitSpending += 5;
			break;

		case AREAAI_DEFENSIVE:
			iMaxUnitSpending += 10;
			break;

		case AREAAI_MASSING:
			iMaxUnitSpending += 25;
			break;

		case AREAAI_ASSAULT:
			iMaxUnitSpending += 8;
			break;

		case AREAAI_ASSAULT_MASSING:
			iMaxUnitSpending += 16;
			break;

		case AREAAI_ASSAULT_ASSIST:
			iMaxUnitSpending += 6;
			break;

		case AREAAI_NEUTRAL:
			break;
		default:
			FErrorMsg("error");
		}
	}
	return iMaxUnitSpending;
}

