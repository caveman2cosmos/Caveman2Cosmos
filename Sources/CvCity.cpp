
#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvArea.h"
#include "CvArtFileMgr.h"
#include "CvBuildingInfo.h"
#include "CvCity.h"
#include "CvContractBroker.h"
#include "CvDLLEntity.h"
#include "CvEventReporter.h"
#include "CvGameAI.h"
#include "CvGameTextMgr.h"
#include "CvGlobals.h"
#include "CvImprovementInfo.h"
#include "CvBonusInfo.h"
#include "CvInfos.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvPlayerAI.h"
#include "CvPopupInfo.h"
#include "CvPython.h"
#include "CvReachablePlotSet.h"
#include "CvSelectionGroup.h"
#include "CvTeamAI.h"
#include "CvUnit.h"
#include "CvUnitSelectionCriteria.h"
#include "CvViewport.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLUtilityIFaceBase.h"
#include "CvTraitInfo.h"
#ifdef THE_GREAT_WALL
#include "CvDLLEngineIFaceBase.h"
#endif
#include "CityOutputHistory.h"

//Disable this passed in initialization list warning, as it is only stored in the constructor of CvBuildingList and not used
#pragma warning( disable : 4355 )


CvCity::CvCity()
	: m_GameObject(this),
	m_BuildingList(NULL, this),
	m_UnitList(NULL, this),
	m_Properties(this),
	m_outputHistory()
{
	m_aiRiverPlotYield = new int[NUM_YIELD_TYPES];
	m_aiBaseYieldRate = new int[NUM_YIELD_TYPES];
	m_aiExtraYield = new int[NUM_YIELD_TYPES];
	m_buildingExtraYield100 = new int[NUM_YIELD_TYPES];
	m_buildingYieldMod = new int[NUM_YIELD_TYPES];
	m_aiBaseYieldPerPopRate = new int[NUM_YIELD_TYPES];
	m_aiYieldRateModifier = new int[NUM_YIELD_TYPES];
	m_aiPowerYieldRateModifier = new int[NUM_YIELD_TYPES];
	m_aiBonusYieldRateModifier = new int[NUM_YIELD_TYPES];
	m_aiTradeYield = new int[NUM_YIELD_TYPES];
	m_aiCorporationYield = new int[NUM_YIELD_TYPES];
	m_aiExtraSpecialistYield = new int[NUM_YIELD_TYPES];
	m_aiExtraSpecialistCommerce = new int[NUM_COMMERCE_TYPES];
	m_aiCommerceRate = new int[NUM_COMMERCE_TYPES];
	m_abCommerceRateDirty = new bool[NUM_COMMERCE_TYPES];
	m_aiProductionToCommerceModifier = new int[NUM_COMMERCE_TYPES];
	m_aiBuildingCommerce = new int[NUM_COMMERCE_TYPES];
	m_aiSpecialistCommerce100 = new int[NUM_COMMERCE_TYPES];
	m_aiReligionCommerce = new int[NUM_COMMERCE_TYPES];
	m_aiCorporationCommerce = new int[NUM_COMMERCE_TYPES];
	m_aiCommerceRateModifier = new int[NUM_COMMERCE_TYPES];
	m_aiCommerceHappinessPer = new int[NUM_COMMERCE_TYPES];
	m_commercePerPopFromBuildings = new int[NUM_COMMERCE_TYPES];
	m_buildingCommerceMod = new int[NUM_COMMERCE_TYPES];
	m_aiDomainFreeExperience = new int[NUM_DOMAIN_TYPES];
	m_aiDomainProductionModifier = new int[NUM_DOMAIN_TYPES];

	m_aiCulture = new int[MAX_PLAYERS];
	m_aiNumRevolts = new int[MAX_PLAYERS];
	m_abEverOwned = new bool[MAX_PLAYERS];
	m_abTradeRoute = new bool[MAX_PLAYERS];
	m_abRevealed = new bool[MAX_TEAMS];
	m_abEspionageVisibility = new bool[MAX_TEAMS];

	const int iNumBuildings = GC.getNumBuildingInfos();
	m_bHasBuildings = new bool[iNumBuildings];
	m_pabReligiouslyDisabledBuilding = new bool[iNumBuildings];

	m_paiFreeBonus = NULL;
	m_paiNumBonuses = NULL;
	m_paiProjectProduction = NULL;
	m_paiUnitProduction = NULL;
	m_paiGreatPeopleUnitRate = NULL;
	m_paiGreatPeopleUnitProgress = NULL;
	m_paiSpecialistCount = NULL;
	m_paiMaxSpecialistCount = NULL;
	m_paiForceSpecialistCount = NULL;
	m_paiFreeSpecialistCount = NULL;
	m_paiFreeSpecialistCountUnattributed = NULL;
	m_paiImprovementFreeSpecialists = NULL;
	m_paiReligionInfluence = NULL;
	m_paiStateReligionHappiness = NULL;
	m_paiUnitCombatFreeExperience = NULL;
	m_bPropertyControlBuildingQueued = false;

	m_pabWorkingPlot = NULL;
	m_pabHasReligion = NULL;
	m_pabHasCorporation = NULL;

	m_aiBonusCommerceRateModifier = new int[NUM_COMMERCE_TYPES];
	m_aiBonusCommercePercentChanges = new int[NUM_COMMERCE_TYPES];
	m_aiBuildingCommerceTechChange = new int[NUM_COMMERCE_TYPES];

	m_cachedPropertyNeeds = NULL;
	m_pabHadVicinityBonus = NULL;
	m_pabHasVicinityBonus = NULL;
	m_pabHadRawVicinityBonus = NULL;
	m_pabHasRawVicinityBonus = NULL;
	m_pabHasVicinityBonusCached = NULL;
	m_pabHasRawVicinityBonusCached = NULL;
	m_paiUnitCombatExtraStrength = NULL;
	m_pabAutomatedCanBuild = NULL;
	m_paiNewAfflictionTypeCount = NULL;
	m_paiAidRate = NULL;
	m_ppaaiExtraBonusAidModifier = NULL;
	m_paiNewExtraAfflictionOutbreakLevelChange = NULL;
	m_paiNewAfflictionToleranceChange = NULL;
	m_paiNewCurrentOvercomeChange = NULL;
	m_paiUnitCombatProductionModifier = NULL;
	m_paiUnitCombatRepelModifier = NULL;
	m_paiUnitCombatRepelAgainstModifier = NULL;
	m_paiUnitCombatDefenseAgainstModifier = NULL;
	m_paiPromotionLineAfflictionAttackCommunicability = NULL;
	m_ppaaiLocalSpecialistExtraYield = NULL;
	m_ppaaiLocalSpecialistExtraCommerce = NULL;

	m_paiSpecialistBannedCount = NULL;
	m_paiDamageAttackingUnitCombatCount = NULL;
	m_paiHealUnitCombatTypeVolume = NULL;

#ifdef CAN_TRAIN_CACHING
	//	KOSHLING - clear canTrain cache validity
	m_canTrainCachePopulated = false;
#endif
	m_bVisibilitySetup = false;
	m_bCanConstruct = NULL;

	//CvDLLEntity::createCityEntity(this);		// create and attach entity to city

	m_aiBaseYieldRank = new int[NUM_YIELD_TYPES];
	m_abBaseYieldRankValid = new bool[NUM_YIELD_TYPES];
	m_aiYieldRank = new int[NUM_YIELD_TYPES];
	m_abYieldRankValid = new bool[NUM_YIELD_TYPES];
	m_aiCommerceRank = new int[NUM_COMMERCE_TYPES];
	m_abCommerceRankValid = new bool[NUM_COMMERCE_TYPES];

	m_deferringBonusProcessingCount = 0;
	m_paiStartDeferredSectionNumBonuses = NULL;
	m_bMarkedForDestruction = false;

	reset(0, NO_PLAYER, 0, 0, true);
}

CvCity::~CvCity()
{
	if (getEntity() != NULL)
	{
		CvDLLEntity::removeEntity();			// remove entity from engine
		CvDLLEntity::destroyEntity();			// delete CvCityEntity and detach from us
	}

	uninit();

	SAFE_DELETE_ARRAY(m_aiBaseYieldRank);
	SAFE_DELETE_ARRAY(m_abBaseYieldRankValid);
	SAFE_DELETE_ARRAY(m_aiYieldRank);
	SAFE_DELETE_ARRAY(m_abYieldRankValid);
	SAFE_DELETE_ARRAY(m_aiCommerceRank);
	SAFE_DELETE_ARRAY(m_abCommerceRankValid);

	SAFE_DELETE_ARRAY(m_aiRiverPlotYield);
	SAFE_DELETE_ARRAY(m_aiBaseYieldRate);
	SAFE_DELETE_ARRAY(m_aiExtraYield);
	SAFE_DELETE_ARRAY(m_buildingExtraYield100);
	SAFE_DELETE_ARRAY(m_buildingYieldMod);
	SAFE_DELETE_ARRAY(m_buildingCommerceMod);
	SAFE_DELETE_ARRAY(m_aiBaseYieldPerPopRate);
	SAFE_DELETE_ARRAY(m_aiYieldRateModifier);
	SAFE_DELETE_ARRAY(m_aiPowerYieldRateModifier);
	SAFE_DELETE_ARRAY(m_aiBonusYieldRateModifier);
	SAFE_DELETE_ARRAY(m_aiTradeYield);
	SAFE_DELETE_ARRAY(m_aiCorporationYield);
	SAFE_DELETE_ARRAY(m_aiExtraSpecialistYield);

	SAFE_DELETE_ARRAY(m_aiExtraSpecialistCommerce);
	SAFE_DELETE_ARRAY(m_aiCommerceRate);
	SAFE_DELETE_ARRAY(m_abCommerceRateDirty);
	SAFE_DELETE_ARRAY(m_aiProductionToCommerceModifier);
	SAFE_DELETE_ARRAY(m_aiBuildingCommerce);
	SAFE_DELETE_ARRAY(m_aiSpecialistCommerce100);
	SAFE_DELETE_ARRAY(m_aiReligionCommerce);
	SAFE_DELETE_ARRAY(m_aiCorporationCommerce);
	SAFE_DELETE_ARRAY(m_aiCommerceRateModifier);
	SAFE_DELETE_ARRAY(m_aiCommerceHappinessPer);
	SAFE_DELETE_ARRAY(m_commercePerPopFromBuildings);
	SAFE_DELETE_ARRAY(m_aiDomainFreeExperience);
	SAFE_DELETE_ARRAY(m_aiDomainProductionModifier);
	SAFE_DELETE_ARRAY(m_aiCulture);
	SAFE_DELETE_ARRAY(m_aiNumRevolts);
	SAFE_DELETE_ARRAY(m_abEverOwned);
	SAFE_DELETE_ARRAY(m_abTradeRoute);
	SAFE_DELETE_ARRAY(m_abRevealed);
	SAFE_DELETE_ARRAY(m_abEspionageVisibility);
	SAFE_DELETE_ARRAY(m_aiBonusCommerceRateModifier);
	SAFE_DELETE_ARRAY(m_aiBonusCommercePercentChanges);
	SAFE_DELETE_ARRAY(m_aiBuildingCommerceTechChange);

	SAFE_DELETE_ARRAY(m_bHasBuildings);
	SAFE_DELETE_ARRAY(m_pabReligiouslyDisabledBuilding);
}


void CvCity::init(int iID, PlayerTypes eOwner, int iX, int iY, bool bBumpUnits, bool bUpdatePlotGroups)
{
	PROFILE_FUNC();

	//--------------------------------
	// Log this event
	if (GC.getLogging())
	{
		char szOut[1024];
		sprintf(szOut, "Player %d City %d built at %d:%d\n", eOwner, iID, iX, iY);
		gDLL->messageControlLog(szOut);
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	//--------------------------------
	// Init saved data
	reset(iID, eOwner, pPlot->getX(), pPlot->getY());

	CvPlayer& player = GET_PLAYER(eOwner);
	if (player.isHumanPlayer(true))
	{
		player.setIdleCity(getID(), true);
	}
	//--------------------------------
	// Init non-saved data
	setupGraphical();

	//--------------------------------
	// Init other game data
	bool bFound = false;
	if (GC.getGame().isOption(GAMEOPTION_MAP_PERSONALIZED) && player.isModderOption(MODDEROPTION_USE_LANDMARK_NAMES))
	{
		for (int iI = 0; iI < NUM_CITY_PLOTS_2; iI++)
		{
			CvPlot* pLoopPlot = getCityIndexPlot(iI);
			FAssertMsg(pLoopPlot != NULL, CvString::format("pLoopPlot was null for iIndex %d", iI).c_str());
			if (!pLoopPlot->getLandmarkName().empty() && pLoopPlot->getLandmarkType() != NO_LANDMARK)
			{
				setName(pLoopPlot->getLandmarkName());
				if (!getName().empty())
				{
					bFound = true;
					break;
				}
			}
		}
	}
	if (!bFound) setName(player.getNewCityName());

	setEverOwned(getOwner(), true);

	pPlot->setImprovementType(NO_IMPROVEMENT);
	pPlot->setOwner(getOwner(), bBumpUnits, false);
	pPlot->setPlotCity(this);

	updateCultureLevel(false);

	pPlot->changeCulture(getOwner(), GC.getFREE_CITY_CULTURE(), bBumpUnits);

	// Immediately put some tiles on adjacent tiles if not 1TF option. Which tiles depends on game options.
	if (!GC.getGame().isOption(GAMEOPTION_CULTURE_1_CITY_TILE_FOUNDING))
	{
		const int iAdjCulture = GC.getFREE_CITY_ADJACENT_CULTURE();
		// Special case: RCS with no MCB gains only tiles a lvl 1 city would gain as defined by RCS
		if (GC.getGame().isOption(GAMEOPTION_CULTURE_REALISTIC_SPREAD) && !GC.getGame().isOption(GAMEOPTION_CULTURE_MIN_CITY_BORDER))
		{
			foreach_(CvPlot* pAdjacentPlot, plot()->cardinalDirectionAdjacent())
			{
				if (cultureDistance(*pAdjacentPlot) == 1)
				{
					pAdjacentPlot->changeCulture(getOwner(), iAdjCulture, bBumpUnits);
					pAdjacentPlot->updateCulture(bBumpUnits, false);
				}
			}
		}
		// All 8 tiles gained. MCB enables this even if RCS also on; 'soft' RCS
		else
		{
			foreach_(CvPlot* pAdjacentPlot, plot()->adjacent())
			{
				pAdjacentPlot->changeCulture(getOwner(), iAdjCulture, bBumpUnits);
				pAdjacentPlot->updateCulture(bBumpUnits, false);
			}
		}
	}

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM(getTeam()).isVassal((TeamTypes)iI))
		{
			pPlot->changeAdjacentSight((TeamTypes)iI, 1, true, NULL, false);
		}
	}

	if (pPlot->getFeatureType() != NO_FEATURE && pPlot->getFeatureType() != GC.getFEATURE_FLOOD_PLAINS()
	&& (GC.getFeatureInfo(pPlot->getFeatureType()).getPopDestroys() == 0 || GC.getFeatureInfo(pPlot->getFeatureType()).getPopDestroys() == 1))
	{
		pPlot->setFeatureType(NO_FEATURE);
	}

	pPlot->updateCityRoute(false);

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && pPlot->isVisible((TeamTypes)iI, false))
		{
			setRevealed((TeamTypes)iI, true);
		}
	}

	{
		// don't use pPlot->plotCount(PUF_isMilitaryHappiness), it doesn't count dead units
		//	and will thus for the AI not recognize units that have just merged on the plot the same turn before it founded the city.
		// hmm, maybe plotCount should always count dead units, need to investigate, could add a new paramater to make it count dead units too.
		int iCount = 0;
		foreach_(const CvUnit* unitX, pPlot->units())
		{
			if (unitX->isMilitaryHappiness())
			{
				iCount++;
			}
		}
		changeMilitaryHappinessUnits(iCount);
	}

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		changeCommerceHappinessPer(((CommerceTypes)iI), GC.getCommerceInfo((CommerceTypes)iI).getInitialHappiness());
	}

	CvArea* pArea = area();
	//TBFREEBUILD
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		const uint16_t iFreeAreaBuildingCount = player.getFreeAreaBuildingCount((BuildingTypes)iI, pArea);
		if (iFreeAreaBuildingCount > 0)
		{
			changeFreeAreaBuildingCount((BuildingTypes)iI, iFreeAreaBuildingCount);
		}
		else if (player.getFreeBuildingCount((BuildingTypes)iI) > 0)
		{
			setFreeBuilding(((BuildingTypes)iI), true);
		}
	}

	pArea->changeCitiesPerPlayer(getOwner(), 1);

	GET_TEAM(getTeam()).changeNumCities(1);

	GC.getGame().changeNumCities(1);

	bool bHistoricalCalendar = GC.getGame().isModderGameOption(MODDERGAMEOPTION_USE_HISTORICAL_ACCURATE_CALENDAR);

	setGameTurnFounded(GC.getGame().getGameTurn(), bHistoricalCalendar);
	setGameTurnAcquired(GC.getGame().getGameTurn(), bHistoricalCalendar);

	setPopulation(GC.getINITIAL_CITY_POPULATION() + GC.getEraInfo(GC.getGame().getStartEra()).getFreePopulation(), false);

	changeAirUnitCapacity(GC.getCITY_AIR_UNIT_CAPACITY());

	updateFreshWaterHealth();
	updateFeatureHealth();
	updateImprovementHealth();
	updateFeatureHappiness();

	player.setMaintenanceDirty(true);

	GC.getMap().updateWorkingCity();

	GC.getGame().AI_makeAssignWorkDirty();

	player.setFoundedFirstCity(true);

	if (isNPC() || player.getNumCities() == 1)
	{
		for (int iI = 0; iI < GC.getCivilizationInfo(getCivilizationType()).getNumCivilizationBuildings(); iI++)
		{
			changeHasBuilding((BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuilding(iI), true);
		}
	}

	updateEspionageVisibility(false);

	if (bUpdatePlotGroups)
	{
		GC.getGame().updatePlotGroups();
	}
	setCivilizationType(player.getCivilizationType());

	m_UnitList.init();

	AI_init();
}


void CvCity::uninit()
{
	SAFE_DELETE_ARRAY(m_paiFreeBonus);
	SAFE_DELETE_ARRAY(m_paiNumBonuses);
	SAFE_DELETE_ARRAY(m_paiProjectProduction);
	SAFE_DELETE_ARRAY(m_paiUnitProduction);
	SAFE_DELETE_ARRAY(m_paiGreatPeopleUnitRate);
	SAFE_DELETE_ARRAY(m_paiGreatPeopleUnitProgress);
	SAFE_DELETE_ARRAY(m_paiSpecialistCount);
	SAFE_DELETE_ARRAY(m_paiMaxSpecialistCount);
	SAFE_DELETE_ARRAY(m_paiForceSpecialistCount);
	SAFE_DELETE_ARRAY(m_paiFreeSpecialistCount);
	SAFE_DELETE_ARRAY(m_paiFreeSpecialistCountUnattributed);
	SAFE_DELETE_ARRAY(m_paiImprovementFreeSpecialists);
	SAFE_DELETE_ARRAY(m_paiReligionInfluence);
	SAFE_DELETE_ARRAY(m_paiStateReligionHappiness);
	SAFE_DELETE_ARRAY(m_paiUnitCombatFreeExperience);
	SAFE_DELETE_ARRAY(m_cachedPropertyNeeds);
	SAFE_DELETE_ARRAY(m_pabHadVicinityBonus);
	SAFE_DELETE_ARRAY(m_pabHasVicinityBonus);
	SAFE_DELETE_ARRAY(m_pabHadRawVicinityBonus);
	SAFE_DELETE_ARRAY(m_pabHasRawVicinityBonus);
	SAFE_DELETE_ARRAY(m_pabHasVicinityBonusCached);
	SAFE_DELETE_ARRAY(m_pabHasRawVicinityBonusCached);
	SAFE_DELETE_ARRAY(m_paiUnitCombatExtraStrength);
	SAFE_DELETE_ARRAY(m_pabAutomatedCanBuild);
	SAFE_DELETE_ARRAY(m_pabWorkingPlot);
	SAFE_DELETE_ARRAY(m_pabHasReligion);
	SAFE_DELETE_ARRAY(m_pabHasCorporation);

	SAFE_DELETE_ARRAY(m_paiNewAfflictionTypeCount);
	SAFE_DELETE_ARRAY(m_paiAidRate);
	SAFE_DELETE_ARRAY2(m_ppaaiExtraBonusAidModifier, GC.getNumBonusInfos());
	SAFE_DELETE_ARRAY(m_paiNewExtraAfflictionOutbreakLevelChange);
	SAFE_DELETE_ARRAY(m_paiNewAfflictionToleranceChange);
	SAFE_DELETE_ARRAY(m_paiNewCurrentOvercomeChange);
	SAFE_DELETE_ARRAY(m_paiUnitCombatProductionModifier);
	SAFE_DELETE_ARRAY(m_paiUnitCombatRepelModifier);
	SAFE_DELETE_ARRAY(m_paiUnitCombatRepelAgainstModifier);
	SAFE_DELETE_ARRAY(m_paiUnitCombatDefenseAgainstModifier);
	SAFE_DELETE_ARRAY(m_paiPromotionLineAfflictionAttackCommunicability);
	SAFE_DELETE_ARRAY(m_paiStartDeferredSectionNumBonuses);
	SAFE_DELETE_ARRAY(m_paiSpecialistBannedCount);
	SAFE_DELETE_ARRAY(m_paiDamageAttackingUnitCombatCount);
	SAFE_DELETE_ARRAY(m_paiHealUnitCombatTypeVolume);
	SAFE_DELETE_ARRAY2(m_ppaaiLocalSpecialistExtraYield, GC.getNumSpecialistInfos());
	SAFE_DELETE_ARRAY2(m_ppaaiLocalSpecialistExtraCommerce, GC.getNumSpecialistInfos());
}

// FUNCTION: reset()
// Initializes data members that are serialized.
void CvCity::reset(int iID, PlayerTypes eOwner, int iX, int iY, bool bConstructorCall)
{
	PROFILE_EXTRA_FUNC();
	//--------------------------------
	// Uninit class
	uninit();

	if (!bConstructorCall)
	{
		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			setCommerceModifierDirty((CommerceTypes)iI);
		}
	}
	m_iID = iID;
	m_iX = iX;
	m_iY = iY;
	m_iRallyX = INVALID_PLOT_COORD;
	m_iRallyY = INVALID_PLOT_COORD;
	m_iGameTurnFounded = 0;
	m_iGameTurnAcquired = 0;
	m_iPopulation = 0;
	m_iHighestPopulation = 0;
	m_iWorkingPopulation = 0;
	m_iSpecialistPopulation = 0;
	m_iNumGreatPeople = 0;
	m_iBaseGreatPeopleRate = 0;
	m_iGreatPeopleRateModifier = 0;
	m_iGreatPeopleProgress = 0;
	m_iNumWorldWonders = 0;
	m_iNumTeamWonders = 0;
	m_iNumNationalWonders = 0;
	m_iNumBuildings = 0;
	m_iGovernmentCenterCount = 0;
	m_iMaintenance = 0;
	m_iMaintenanceModifier = 0;
	m_iWarWearinessModifier = 0;
	m_iHurryAngerModifier = 0;
	m_iHealRate = 0;
	m_iEspionageHealthCounter = 0;
	m_iEspionageHappinessCounter = 0;
	m_iFreshWaterGoodHealth = 0;
	m_iFeatureGoodHealth = 0;
	m_iFeatureBadHealth = 0;
	m_iBuildingGoodHealth = 0;
	m_iBuildingBadHealth = 0;
	m_iBonusGoodHealth = 0;
	m_iBonusBadHealth = 0;
	m_iHurryAngerTimer = 0;
	m_iRevRequestAngerTimer = 0;
	m_iRevSuccessTimer = 0;
	m_iConscriptAngerTimer = 0;
	m_iDefyResolutionAngerTimer = 0;
	m_iHappinessTimer = 0;
	m_iMilitaryHappinessUnits = 0;
	m_iBuildingGoodHappiness = 0;
	m_iBuildingBadHappiness = 0;
	m_iExtraBuildingGoodHappiness = 0;
	m_iExtraBuildingBadHappiness = 0;
	m_iExtraBuildingGoodHealth = 0;
	m_iExtraBuildingBadHealth = 0;
	m_iFeatureGoodHappiness = 0;
	m_iFeatureBadHappiness = 0;
	m_iBonusGoodHappiness = 0;
	m_iBonusBadHappiness = 0;
	m_iReligionGoodHappiness = 0;
	m_iReligionBadHappiness = 0;
	m_iExtraHappiness = 0;
	m_iExtraHealth = 0;
	m_iNoUnhappinessCount = 0;
	m_iNoUnhealthyPopulationCount = 0;
	m_iBuildingOnlyHealthyCount = 0;
	m_iFood = 0;
	m_iFoodKept = 0;
	m_iFoodKeptPercent = 0;
	m_iOverflowProduction = 0;
	m_iFeatureProduction = 0;
	m_iMilitaryProductionModifier = 0;
	m_iSpaceProductionModifier = 0;
	m_iExtraTradeRoutes = 0;
	m_iTradeRouteModifier = 0;
	m_iForeignTradeRouteModifier = 0;
	m_iBuildingDefense = 0;
	m_iBuildingBombardDefense = 0;
	m_iFreeExperience = 0;
	m_iCurrAirlift = 0;
	m_iMaxAirlift = 0;
	m_iAirModifier = 0;
	m_iAirUnitCapacity = 0;
	m_iNukeModifier = 0;
	m_iFreeSpecialist = 0;
	m_iPowerCount = 0;
	m_iDefenseDamage = 0;
	m_iLastDefenseDamage = 0;
	m_iOccupationTimer = 0;
	m_iCultureUpdateTimer = 0;
	m_iCitySizeBoost = 0;
	m_iSpecialistFreeExperience = 0;
	m_iEspionageDefenseModifier = 0;
	m_iSpecialistInsidiousness = 0;
	m_iSpecialistInvestigation = 0;
	m_icachedPropertyNeedsTurn = 0;
	m_fPopulationgrowthratepercentageLog = 0.0;
	m_iImprovementGoodHealth = 0;
	m_iImprovementBadHealth = 0;
	m_iSpecialistGoodHealth = 0;
	m_iSpecialistBadHealth = 0;
	m_iSpecialistHappiness = 0;
	m_iSpecialistUnhappiness = 0;
	m_iCiv = NO_CIVILIZATION;
	m_iLineOfSight = 0;
	m_iLandmarkAngerTimer = 0;
	m_iFreshWater = 0;
	m_iAdjacentDamagePercent = 0;
	m_iLostProduction = 0;
	m_iWorkableRadiusOverride = 0;
	m_iProtectedCultureCount = 0;
	m_iNumUnitFullHeal = 0;
	m_iDisabledPowerTimer = 0;
	m_iWarWearinessTimer = 0;
	m_iEventAnger = 0;
	m_iMinimumDefenseLevel = 0;
	m_iNumPopulationEmployed = 0;
	m_iHealthPercentPerPopulation = 0;
	m_iHappinessPercentPerPopulation = 0;
	m_iQuarantinedCount = 0;
	m_bNeverLost = true;
	m_bPropertyControlBuildingQueued = false;
	m_bVisibilitySetup = false;
	m_bBombarded = false;
	m_bDrafted = false;
	m_bAirliftTargeted = false;
	m_bWeLoveTheKingDay = false;
	m_bCitizensAutomated = true;
	m_bProductionAutomated = false;
	m_bWallOverride = false;
	m_bInfoDirty = true;
	m_bLayoutDirty = false;
	m_bMaintenanceDirty = false;
	m_bPlundered = false;
	m_bPlotWorkingMasked = false;
	m_bPopProductionProcess = false;

	m_eOwner = eOwner;
	m_ePreviousOwner = NO_PLAYER;
	m_eOriginalOwner = eOwner;
	m_eCultureLevel = NO_CULTURELEVEL;

	m_iRevolutionIndex = 0;
	m_iLocalRevIndex = -1;
	m_iRevIndexDistanceMod = 0;
	m_iRevIndexAverage = 0;
	m_iRevolutionCounter = 0;
	m_iReinforcementCounter = 0;

	//TB Combat Mod (Buildings) begin
#ifdef STRENGTH_IN_NUMBERS
	m_iTotalFrontSupportPercentModifier = 0;
	m_iTotalShortRangeSupportPercentModifier = 0;
	m_iTotalMediumRangeSupportPercentModifier = 0;
	m_iTotalLongRangeSupportPercentModifier = 0;
	m_iTotalFlankSupportPercentModifier = 0;
#endif
	m_iExtraLocalCaptureProbabilityModifier = 0;
	m_iExtraLocalCaptureResistanceModifier = 0;
	m_iExtraLocalDynamicDefense = 0;
	m_iExtraRiverDefensePenalty = 0;
	m_iExtraLocalRepel = 0;
	m_iExtraMinDefense = 0;
	m_iExtraBuildingDefenseRecoverySpeedModifier = 0;
	m_iModifiedBuildingDefenseRecoverySpeedCap = 0;
	m_iExtraCityDefenseRecoverySpeedModifier = 0;
	m_iExtraTechSpecialistHappiness = 0;
	m_iExtraBuildingHappinessFromTech = 0;
	m_iExtraTechSpecialistHealth = 0;
	m_iExtraBuildingHealthFromTech = 0;
	m_iPrioritySpecialist = NO_SPECIALIST;
	m_iExtraInsidiousness = 0;
	m_iExtraInvestigation = 0;
	m_iSpecialistInsidiousness = 0;
	m_iSpecialistInvestigation = 0;
	m_icachedPropertyNeedsTurn = 0;
	//TB Combat Mod (Buildings) end

	m_iZoCCount = 0;

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiRiverPlotYield[iI] = 0;
		m_aiBaseYieldRate[iI] = 0;
		m_aiExtraYield[iI] = 0;
		m_buildingExtraYield100[iI] = 0;
		m_buildingYieldMod[iI] = 0;
		m_aiBaseYieldPerPopRate[iI] = 0;
		m_aiYieldRateModifier[iI] = 0;
		m_aiPowerYieldRateModifier[iI] = 0;
		m_aiBonusYieldRateModifier[iI] = 0;
		m_aiTradeYield[iI] = 0;
		m_aiCorporationYield[iI] = 0;
		m_aiExtraSpecialistYield[iI] = 0;
		m_abBaseYieldRankValid[iI] = false;
		m_abYieldRankValid[iI] = false;
		m_aiBaseYieldRank[iI] = -1;
		m_aiYieldRank[iI] = -1;
	}

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		m_aiCommerceRate[iI] = 0;
		m_abCommerceRateDirty[iI] = false;
		m_aiProductionToCommerceModifier[iI] = 0;
		m_aiBuildingCommerce[iI] = 0;
		m_aiSpecialistCommerce100[iI] = 0;
		m_aiReligionCommerce[iI] = 0;
		m_aiCorporationCommerce[iI] = 0;
		m_aiCommerceRateModifier[iI] = 0;
		m_aiCommerceHappinessPer[iI] = 0;
		m_commercePerPopFromBuildings[iI] = 0;
		m_aiBonusCommerceRateModifier[iI] = 0;
		m_aiBonusCommercePercentChanges[iI] = 0;
		m_aiBuildingCommerceTechChange[iI] = 0;
		m_aiExtraSpecialistCommerce[iI] = 0;
		m_buildingCommerceMod[iI] = 0;
		m_abCommerceRankValid[iI] = false;
		m_aiCommerceRank[iI] = -1;
	}

	for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		m_aiDomainFreeExperience[iI] = 0;
		m_aiDomainProductionModifier[iI] = 0;
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aiCulture[iI] = 0;
		m_aiNumRevolts[iI] = 0;
		m_abEverOwned[iI] = false;
		m_abTradeRoute[iI] = false;
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_abRevealed[iI] = false;
		m_abEspionageVisibility[iI] = false;
	}

	m_hasBuildings.clear();
	m_vDisabledBuildings.clear();
	m_vFreeBuildings.clear();
	m_freeAreaBuildingCount.clear();
	m_paTradeCities.clear();
	m_orderQueue.clear();
	m_aEventsOccured.clear();
	m_aBuildingYieldChange.clear();
	m_aBuildingCommerceChange.clear();
	m_aBuildingHappyChange.clear();
	m_aBuildingHealthChange.clear();
	m_buildingProductionMod.clear();
	m_unitProductionMod.clear();
	m_bonusDefenseChanges.clear();
	m_terrainYieldChanges.clear();
	m_plotYieldChanges.clear();
	m_Properties.clear();
	m_aPropertySpawns.clear();
	m_buildingHappinessFromTech.clear();
	m_buildingHealthFromTech.clear();
	m_progressOnBuilding.clear();
	m_delayOnBuilding.clear();
	m_progressOnUnit.clear();
	m_delayOnUnit.clear();
	m_corpBonusProduction.clear();
	m_buildingLedger.clear();

	m_szName.clear();
	m_szScriptData = "";

	m_bPopulationRankValid = false;
	m_iPopulationRank = -1;

	if (!bConstructorCall)
	{
		FAssertMsg(0 < GC.getNumSpecialistInfos(), "GC.getNumSpecialistInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		FAssertMsg(m_ppaaiLocalSpecialistExtraYield == NULL, "about to leak memory, CvCity::m_ppaaiLocalSpecialistExtraYield");
		m_ppaaiLocalSpecialistExtraYield = new int* [GC.getNumSpecialistInfos()];
		FAssertMsg(m_ppaaiLocalSpecialistExtraCommerce == NULL, "about to leak memory, CvCity::m_ppaaiLocalSpecialistExtraCommerce");
		m_ppaaiLocalSpecialistExtraCommerce = new int* [GC.getNumSpecialistInfos()];
		m_paiSpecialistBannedCount = new int[GC.getNumSpecialistInfos()];
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			m_ppaaiLocalSpecialistExtraYield[iI] = new int[NUM_YIELD_TYPES];
			for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				m_ppaaiLocalSpecialistExtraYield[iI][iJ] = 0;
			}
			m_paiSpecialistBannedCount[iI] = 0;
			m_ppaaiLocalSpecialistExtraCommerce[iI] = new int[NUM_COMMERCE_TYPES];
			for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				m_ppaaiLocalSpecialistExtraCommerce[iI][iJ] = 0;
			}
		}

		FAssertMsg(0 < GC.getNumBonusInfos(), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiFreeBonus = new int[GC.getNumBonusInfos()];
		m_paiNumBonuses = new int[GC.getNumBonusInfos()];
		m_ppaaiExtraBonusAidModifier = new int* [GC.getNumBonusInfos()];

		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			m_ppaaiExtraBonusAidModifier[iI] = new int[GC.getNumPropertyInfos()];
			for (int iJ = 0; iJ < GC.getNumPropertyInfos(); iJ++)
			{
				m_ppaaiExtraBonusAidModifier[iI][iJ] = 0;
			}
			m_paiFreeBonus[iI] = 0;
			m_paiNumBonuses[iI] = 0;
		}

		m_paiProjectProduction = new int[GC.getNumProjectInfos()];
		for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
		{
			m_paiProjectProduction[iI] = 0;
		}

		m_paiPromotionLineAfflictionAttackCommunicability = new int[GC.getNumPromotionLineInfos()];
		for (int iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
		{
			m_paiPromotionLineAfflictionAttackCommunicability[iI] = 0;
		}

		for (int iI = GC.getNumBuildingInfos() - 1; iI > -1; iI--)
		{
			m_bHasBuildings[iI] = false;
			m_pabReligiouslyDisabledBuilding[iI] = false;
		}

		m_paiUnitProduction = new int[GC.getNumUnitInfos()];
		m_paiGreatPeopleUnitRate = new int[GC.getNumUnitInfos()];
		m_paiGreatPeopleUnitProgress = new int[GC.getNumUnitInfos()];
		for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			m_paiUnitProduction[iI] = 0;
			m_paiGreatPeopleUnitRate[iI] = 0;
			m_paiGreatPeopleUnitProgress[iI] = 0;
		}

		FAssertMsg((0 < GC.getNumSpecialistInfos()), "GC.getNumSpecialistInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiSpecialistCount = new int[GC.getNumSpecialistInfos()];
		m_paiMaxSpecialistCount = new int[GC.getNumSpecialistInfos()];
		m_paiForceSpecialistCount = new int[GC.getNumSpecialistInfos()];
		m_paiFreeSpecialistCount = new int[GC.getNumSpecialistInfos()];
		m_paiFreeSpecialistCountUnattributed = new int[GC.getNumSpecialistInfos()];
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			m_paiSpecialistCount[iI] = 0;
			m_paiMaxSpecialistCount[iI] = 0;
			m_paiForceSpecialistCount[iI] = 0;
			m_paiFreeSpecialistCount[iI] = 0;
			m_paiFreeSpecialistCountUnattributed[iI] = 0;
		}

		FAssertMsg((0 < GC.getNumImprovementInfos()), "GC.getNumImprovementInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiImprovementFreeSpecialists = new int[GC.getNumImprovementInfos()];
		for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			m_paiImprovementFreeSpecialists[iI] = 0;
		}

		m_paiReligionInfluence = new int[GC.getNumReligionInfos()];
		m_paiStateReligionHappiness = new int[GC.getNumReligionInfos()];
		m_pabHasReligion = new bool[GC.getNumReligionInfos()];
		for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			m_paiReligionInfluence[iI] = 0;
			m_paiStateReligionHappiness[iI] = 0;
			m_pabHasReligion[iI] = false;
		}

		m_pabHasCorporation = new bool[GC.getNumCorporationInfos()];
		for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
		{
			m_pabHasCorporation[iI] = false;
		}

		FAssertMsg((0 < GC.getNumUnitCombatInfos()), "GC.getNumUnitCombatInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiUnitCombatFreeExperience = new int[GC.getNumUnitCombatInfos()];
		m_paiUnitCombatExtraStrength = new int[GC.getNumUnitCombatInfos()];
		m_paiUnitCombatProductionModifier = new int[GC.getNumUnitCombatInfos()];
		m_paiUnitCombatRepelModifier = new int[GC.getNumUnitCombatInfos()];
		m_paiUnitCombatRepelAgainstModifier = new int[GC.getNumUnitCombatInfos()];
		m_paiUnitCombatDefenseAgainstModifier = new int[GC.getNumUnitCombatInfos()];
		m_paiDamageAttackingUnitCombatCount = new int[GC.getNumUnitCombatInfos()];
		m_paiHealUnitCombatTypeVolume = new int[GC.getNumUnitCombatInfos()];

		for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
		{
			m_paiUnitCombatFreeExperience[iI] = 0;
			m_paiUnitCombatExtraStrength[iI] = 0;
			m_paiUnitCombatProductionModifier[iI] = 0;
			m_paiUnitCombatRepelModifier[iI] = 0;
			m_paiUnitCombatRepelAgainstModifier[iI] = 0;
			m_paiUnitCombatDefenseAgainstModifier[iI] = 0;
			m_paiDamageAttackingUnitCombatCount[iI] = 0;
			m_paiHealUnitCombatTypeVolume[iI] = 0;
		}

		FAssertMsg((0 < GC.getNumPromotionLineInfos()), "GC.getNumPromotionLineInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiNewAfflictionTypeCount = new int[GC.getNumPromotionLineInfos()];
		m_paiNewExtraAfflictionOutbreakLevelChange = new int[GC.getNumPromotionLineInfos()];
		m_paiNewAfflictionToleranceChange = new int[GC.getNumPromotionLineInfos()];
		m_paiNewCurrentOvercomeChange = new int[GC.getNumPromotionLineInfos()];
		for (int iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
		{
			m_paiNewAfflictionTypeCount[iI] = 0;
			m_paiNewExtraAfflictionOutbreakLevelChange[iI] = 0;
			m_paiNewAfflictionToleranceChange[iI] = 0;
			m_paiNewCurrentOvercomeChange[iI] = 0;
		}

		FAssertMsg((0 < GC.getNumPropertyInfos()), "GC.getNumPropertyInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiAidRate = new int[GC.getNumPropertyInfos()];
		for (int iI = 0; iI < GC.getNumPropertyInfos(); iI++)
		{
			m_paiAidRate[iI] = 0;
		}

		FAssertMsg((0 < GC.getNumInvisibleInfos()), "GC.getNumInvisibleInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_aiCachedBestSeeInvisibleUnit = new int[GC.getNumInvisibleInfos()];
		m_abCachedBestSeeInvisibleUnit = new bool[GC.getNumInvisibleInfos()];
		m_aiCachedBestSeeInvisibleUnitSea = new int[GC.getNumInvisibleInfos()];
		m_abCachedBestSeeInvisibleUnitSea = new bool[GC.getNumInvisibleInfos()];
		for (int iI = 0; iI < GC.getNumInvisibleInfos(); iI++)
		{
			m_aiCachedBestSeeInvisibleUnit[iI] = -1;
			m_abCachedBestSeeInvisibleUnit[iI] = false;
			m_aiCachedBestSeeInvisibleUnitSea[iI] = -1;
			m_abCachedBestSeeInvisibleUnitSea[iI] = false;
		}

		m_pabWorkingPlot = new bool[NUM_CITY_PLOTS];
		for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			m_pabWorkingPlot[iI] = false;
		}
		const int iMaxTradeRoutes = getMaxTradeRoutes();

		FAssertMsg((0 < iMaxTradeRoutes), "Max Trade Routes is not greater than zero but an array is being allocated in CvCity::reset");
		m_paTradeCities = std::vector<IDInfo>(iMaxTradeRoutes);

		m_pabHadVicinityBonus = new bool[GC.getNumBonusInfos()];
		m_pabHadRawVicinityBonus = new bool[GC.getNumBonusInfos()];
		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			m_pabHadVicinityBonus[iI] = false;
			m_pabHadRawVicinityBonus[iI] = false;
		}

		m_cachedPropertyNeeds = new int[GC.getNumPropertyInfos()];
		for (int iI = 0; iI < GC.getNumPropertyInfos(); iI++)
		{
			m_cachedPropertyNeeds[iI] = 0;
		}

		FAssertMsg(m_pabAutomatedCanBuild == NULL, "about to leak memory, CvCity::m_pabAutomatedCanBuild");
		m_pabAutomatedCanBuild = new bool[GC.getNumBuildInfos()];
		for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
		{
			m_pabAutomatedCanBuild[iI] = true;
		}
	}

	if (eOwner != NO_PLAYER)
	{
		m_BuildingList.setPlayerToOwner();
		m_UnitList.setPlayerToOwner();
		m_UnitList.init();
	}

	if (!bConstructorCall)
	{
		m_BuildingList.init();
		AI_reset();
	}

	m_bIsGreatWallSeed = false;
	m_deferringBonusProcessingCount = 0;

	m_outputHistory.reset();
}


//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvCity::setupGraphical()
{
	PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	if (!isInViewport())
	{
		return;
	}

	if (getEntity() == NULL)
	{
		createCityEntity(this);
	}

	CvDLLEntity::setup();

	setInfoDirty(true);
	setLayoutDirty(true);
}


int CvCity::getRevolutionIndex() const
{
	return m_iRevolutionIndex;
}

void CvCity::setRevolutionIndex(int iNewValue)
{
	if (iNewValue < 0)
		iNewValue = 0;

	m_iRevolutionIndex = iNewValue;
}

void CvCity::changeRevolutionIndex(int iChange)
{
	setRevolutionIndex(getRevolutionIndex() + iChange);
}

int CvCity::getLocalRevIndex() const
{
	return m_iLocalRevIndex;
}

void CvCity::setLocalRevIndex(int iNewValue)
{
	m_iLocalRevIndex = iNewValue;
}

void CvCity::changeLocalRevIndex(int iChange)
{
	setLocalRevIndex(getLocalRevIndex() + iChange);
}

int CvCity::getRevIndexAverage() const
{
	return m_iRevIndexAverage;
}

void CvCity::setRevIndexAverage(int iNewValue)
{
	m_iRevIndexAverage = range(iNewValue, 0, 3400);
}

void CvCity::updateRevIndexAverage()
{
	setRevIndexAverage((2 * getRevIndexAverage() + getRevolutionIndex()) / 3);
}

void CvCity::changeRevIndexDistanceMod(const int iChange)
{
	m_iRevIndexDistanceMod += iChange;
}

int CvCity::getRevolutionCounter() const
{
	return m_iRevolutionCounter;
}

void CvCity::setRevolutionCounter(int iNewValue)
{
	if (iNewValue < 0)
		iNewValue = 0;

	m_iRevolutionCounter = iNewValue;
}

void CvCity::changeRevolutionCounter(int iChange)
{
	setRevolutionCounter(getRevolutionCounter() + iChange);
}

int CvCity::getReinforcementCounter() const
{
	return m_iReinforcementCounter;
}

void CvCity::setReinforcementCounter(int iNewValue)
{
	if (iNewValue < 0)
		iNewValue = 0;

	m_iReinforcementCounter = iNewValue;
}

void CvCity::changeReinforcementCounter(int iChange)
{
	setReinforcementCounter(getReinforcementCounter() + iChange);
}

bool CvCity::isRecentlyAcquired() const
{
	return
	(
		(GC.getGame().getGameTurn() - getGameTurnAcquired())
		<
		12 * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 100
	);
}


void CvCity::kill(bool bUpdatePlotGroups, bool bUpdateCulture)
{
	PROFILE_FUNC();

	if (isCitySelected())
	{
		gDLL->getInterfaceIFace()->clearSelectedCities();
	}
	while (m_workers.size() > 0)
	{
		setWorkerHave(m_workers[0], false);
	}
	const PlayerTypes eOwner = getOwner();
	CvPlayer& kOwner = GET_PLAYER(eOwner);

	CvPlot* pPlot = plot();

	// Take this plot out of zobrist hashes for local plot groups
	pPlot->ToggleInPlotGroupsZobristContributors();

	CvPlotGroup* originalTradeNetworkConnectivity[MAX_PLAYERS];
	// Whose trade networks was this city relevant to prior to razing
	if (bUpdatePlotGroups)
	{
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			originalTradeNetworkConnectivity[iI] = GET_PLAYER((PlayerTypes)iI).isAlive() ? pPlot->getPlotGroup((PlayerTypes)iI) : NULL;
		}
	}

	algo::for_each(
		plots(NUM_CITY_PLOTS) | filtered(CvPlot::fn::getWorkingCityOverride() == this),
		CvPlot::fn::setWorkingCityOverride(NULL)
	);
	setCultureLevel(NO_CULTURELEVEL, false);
	clearCultureDistanceCache();

	for (int iI = 0, iNum = GC.getNumBuildingInfos(); iI < iNum; iI++)
	{
		changeHasBuilding((BuildingTypes)iI, false);
	}

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		setHasReligion((ReligionTypes)iI, false, false, true);

		if (isHolyCity((ReligionTypes)iI))
		{
			GC.getGame().setHolyCity((ReligionTypes)iI, NULL, false);
		}
	}

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		setHasCorporation((CorporationTypes)iI, false, false);

		if (isHeadquarters((CorporationTypes)iI))
		{
			GC.getGame().setHeadquarters((CorporationTypes)iI, NULL, false);
		}
	}

	setPopulation(0);
	//AI_assignWorkingPlots();
	clearOrderQueue();

	if (m_orderQueue.empty() && kOwner.isHumanPlayer(true))
	{
		kOwner.setIdleCity(getID(), false);
	}
	else
	{
		FAssertMsg(!kOwner.isIdleCity(getID()), "City with production is cached as idle!");
	}

	// remember the visibility before we take away the city from the plot below
	std::vector<bool> abEspionageVisibility;
	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		abEspionageVisibility.push_back(getEspionageVisibility((TeamTypes)iI));
	}

	// Need to clear trade routes of dead city, else they'll be claimed for the owner forever
	clearTradeRoutes();

	const bool bCapital = isCapital();

	area()->changeCitiesPerPlayer(eOwner, -1);
	GET_TEAM(getTeam()).changeNumCities(-1);
	GC.getGame().changeNumCities(-1);

	pPlot->setPlotCity(NULL);

	pPlot->setImprovementType(GC.getIMPROVEMENT_CITY_RUINS());

	kOwner.setCommerceDirty(NO_COMMERCE);
	kOwner.updateCommerce(NO_COMMERCE);

	CvEventReporter::getInstance().cityLost(this);

	kOwner.deleteCity(getID());

	if (bUpdateCulture)
	{
		pPlot->updateCulture(true, false);

		algo::for_each(pPlot->adjacent(), bind(CvPlot::updateCulture, _1, true, false));
	}

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM(kOwner.getTeam()).isVassal((TeamTypes)iI))
		{
			pPlot->changeAdjacentSight((TeamTypes)iI, 1, false, NULL, false);
		}
	}

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (abEspionageVisibility[iI])
		{
			pPlot->changeAdjacentSight((TeamTypes)iI, 1, false, NULL, false);
		}
	}

	kOwner.setMaintenanceDirty(true);

	GC.getMap().updateWorkingCity();

	GC.getGame().AI_makeAssignWorkDirty();

	if (bCapital)
	{
		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (GET_TEAM(kOwner.getTeam()).isHasEmbassy((TeamTypes)iI))
			{
				pPlot->changeAdjacentSight((TeamTypes)iI, 1, false, NULL, false);
			}
		}
		kOwner.findNewCapital();

		GET_TEAM(kOwner.getTeam()).resetVictoryProgress();
	}
	if (bUpdatePlotGroups)
	{
		PROFILE("CvCity::kill.UpdatePlotGroups");
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (originalTradeNetworkConnectivity[iI] != NULL)
				{
					originalTradeNetworkConnectivity[iI]->recalculatePlots();
				}
				else if (pPlot->isTradeNetwork(GET_PLAYER((PlayerTypes)iI).getTeam()))
				{
					GET_PLAYER((PlayerTypes)iI).updatePlotGroups(pPlot->area());
				}
			}
		}
	}

	if (eOwner == GC.getGame().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
	}
}

void CvCity::killTestCheap()
{
	if (isCitySelected())
	{
		gDLL->getInterfaceIFace()->clearSelectedCities();
	}
	GET_PLAYER(getOwner()).deleteCity(getID());
}

void CvCity::doTurn()
{
	PROFILE("CvCity::doTurn()");

	FAssert(m_deferringBonusProcessingCount == 0);


	flushBestSeeInvisibleUnitCache();
	//	Fail safe
	m_deferringBonusProcessingCount = 0;

	//	New turn.  Generally won't really need a cache flush because
	//	last city of previous turn won't match, but for the sake of edge cases
	//	flush anyway to be safe
	AI_FlushBuildingValueCache();
	FlushCanConstructCache();
	setBuildingListInvalid();
	setUnitListInvalid();
#ifdef CAN_TRAIN_CACHING
	populateCanTrainCache(false);
#endif

	m_unitSourcedPropertyCache.clear();

	//	Slight hack for now - will need to change as and when we go multi-threaded
	CvPlot::setDeferredPlotGroupRecalculationMode(true);

	if (!isBombarded())
	{
		changeDefenseDamage(-cityDefenseRecoveryRate());
	}

	setLastDefenseDamage(getDefenseDamage());
	setBombarded(false);
	setPlundered(false);
	setDrafted(false);
	setAirliftTargeted(false);
	setBuiltFoodProducedUnit(false);
	//Promotes Units if there is a building that allows it
	doPromotion();
	//Does vicinity bonus checks
	doVicinityBonus();
	//Checks conditions of buildings, may disable or enable some
	checkBuildings();
	checkFreeBuildings();

	//Damages enemy units around the city, if applicable
	doAttack();
	//Heals friendly units in the city extra, if applicable
	doHeal();
	//Spreads corporations
	doCorporation();
	//Counts down the disable power timer
	doDisabledPower();

	recalculatePopulationgrowthratepercentage();

	doWarWeariness();

	AI_doTurn();

	bool bAllowNoProduction = !doCheckProduction();

	changeFood(foodDifference(), true);

	doCulture();

	// updating after plot culture ensures player always sees correct ownership on plot,
	// but plot could technically wiggle back and forth during AI turns.
	doPlotCulture(getOwner(), getCommerceRate(COMMERCE_CULTURE));

	//	Force deferred plot group recalculation to happen now before we assess production
	CvPlot::setDeferredPlotGroupRecalculationMode(false);

	//	Auto-build any auto-build buildings we can
	doAutobuild();

	doProduction(bAllowNoProduction);

	GET_PLAYER(getOwner()).getContractBroker().advertiseTender(this, AI_getBuildPriority());

	doDecay();

	doReligion();

	doGreatPeople();

	doMeltdown();

	updateEspionageVisibility(true);

	setCurrAirlift(0);

	doPropertyUnitSpawn();

#ifdef OUTBREAKS_AND_AFFLICTIONS
	//TB Combat Mod (Buildings) begin
	if (GC.getGame().isOption(GAMEOPTION_COMBAT_OUTBREAKS_AND_AFFLICTIONS))
	{
		for (int iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
		{
			PromotionLineTypes eAfflictionLine = ((PromotionLineTypes)iI);

			if (GC.getPromotionLineInfo(eAfflictionLine).isAffliction())
			{
				doOutbreakCheck(eAfflictionLine);

				if (hasAfflictionType(eAfflictionLine))
				{
					changeCurrentOvercomeChange(eAfflictionLine, GC.getPromotionLineInfo(eAfflictionLine).getOvercomeAdjperTurn());
					doOvercomeCheck(eAfflictionLine);
				}

				setPromotionLineAfflictionAttackCommunicability(eAfflictionLine, 0);
				if (getAfflictionToleranceChange(eAfflictionLine) > 0)
				{
					int iDecay = GC.getPromotionLineInfo(eAfflictionLine).getToleranceDecay();
					if (iDecay != 0)
					{
						changeAfflictionToleranceChange(eAfflictionLine, -iDecay);
					}
				}
			}
		}
	}
#endif // OUTBREAKS_AND_AFFLICTIONS

	//TB Combat Mod (Buildings) end

	if (getCultureUpdateTimer() > 0)
	{
		changeCultureUpdateTimer(-1);
	}

	if (getOccupationTimer() > 0)
	{
		changeOccupationTimer(-1);
	}

	if (getHurryAngerTimer() > 0)
	{
		changeHurryAngerTimer(-1);
	}

	if (getRevRequestAngerTimer() > 0)
	{
		changeRevRequestAngerTimer(-1);
	}

	if (getRevSuccessTimer() > 0)
	{
		changeRevSuccessTimer(-1);
	}

	if (getConscriptAngerTimer() > 0)
	{
		changeConscriptAngerTimer(-1);
	}

	if (getDefyResolutionAngerTimer() > 0)
	{
		changeDefyResolutionAngerTimer(-1);
	}

	if (getHappinessTimer() > 0)
	{
		changeHappinessTimer(-1);
	}

	if (getLandmarkAngerTimer() > 0)
	{
		changeLandmarkAngerTimer(-1);
	}

	if (getEspionageHealthCounter() > 0)
	{
		changeEspionageHealthCounter(-1);
	}

	if (getEspionageHappinessCounter() > 0)
	{
		changeEspionageHappinessCounter(-1);
	}

	if (isOccupation() || (angryPopulation() > 0) || (healthRate() < 0))
	{
		setWeLoveTheKingDay(false);
	}
	else if ((getPopulation() >= GC.getWE_LOVE_THE_KING_POPULATION_MIN_POPULATION()) && (GC.getGame().getSorenRandNum(GC.getWE_LOVE_THE_KING_RAND(), "Do We Love The King?") < getPopulation()))
	{
		setWeLoveTheKingDay(true);
	}
	else
	{
		setWeLoveTheKingDay(false);
	}

	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		m_pabHadVicinityBonus[iI] = hasVicinityBonus((BonusTypes)iI);
		m_pabHadRawVicinityBonus[iI] = hasRawVicinityBonus((BonusTypes)iI);
	}

#ifdef CAN_TRAIN_CACHING
	//	Outside the scope of the city's turn where we expect to be using cached values turn
	//	the cache off (we may choose to widden the scope of usability later but for now this is safer)
	clearCanTrainCache();
#endif

	// ONEVENT - Do turn
	CvEventReporter::getInstance().cityDoTurn(this, getOwner());
}

void CvCity::doAutobuild()
{
	PROFILE_EXTRA_FUNC();
	// Toffer - Property buildings should be checked each turn.
	if (!GET_PLAYER(getOwner()).isNPC())
	{
		checkPropertyBuildings();
	}
	// Auto-build any auto-build buildings we can
	for (int iI = GC.getNumBuildingInfos() - 1; iI > -1; iI--)
	{
		const CvBuildingInfo& kBuilding = GC.getBuildingInfo((BuildingTypes)iI);
		if (kBuilding.isAutoBuild())
		{
			if (!hasBuilding((BuildingTypes)iI))
			{
				if (canConstruct((BuildingTypes)iI, false, false, true))
				{
					changeHasBuilding((BuildingTypes)iI, true);
					const CvWString szBuffer = gDLL->getText("TXT_KEY_COMPLETED_AUTO_BUILD", kBuilding.getTextKeyWide(), getName().GetCString());
					AddDLLMessage(getOwner(), true, 10, szBuffer, NULL, MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN());
				}
			}
			// Toffer - World wonder autobuilds should never be auto-removed.
			else if (kBuilding.getMaxGlobalInstances() == -1)
			{
				// Special rule meant for adopted cultures, hopefully it won't affect other autobuilds in an irrational way.
				foreach_(const BuildingModifier2& modifier, kBuilding.getPrereqNumOfBuildings())
				{
					if (GET_PLAYER(getOwner()).getBuildingCount(modifier.first) < GET_PLAYER(getOwner()).getBuildingPrereqBuilding((BuildingTypes)iI, modifier.first, 0))
					{
						changeHasBuilding((BuildingTypes)iI, false);
						const CvWString szBuffer = gDLL->getText("TXT_KEY_COMPLETED_AUTO_BUILD_NOT", kBuilding.getTextKeyWide(), getName().GetCString());
						AddDLLMessage(getOwner(), true, 10, szBuffer, NULL, MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_RED());
						break;
					}
				}
			}
		}
	}
}

void CvCity::checkPropertyBuildings()
{
	PROFILE_EXTRA_FUNC();
#ifdef OUTBREAKS_AND_AFFLICTIONS
	const bool bOaA = GC.getGame().isOption(GAMEOPTION_COMBAT_OUTBREAKS_AND_AFFLICTIONS);
#endif

	for (int iI = GC.getNumPropertyInfos() - 1; iI > -1; iI--)
	{
		const PropertyTypes eProperty = static_cast<PropertyTypes>(iI);
		const int iValue = getProperties()->getValueByProperty(eProperty);

#ifdef OUTBREAKS_AND_AFFLICTIONS
		if (!bOaA || !GC.getPropertyInfo(eProperty).isOAType())
#endif
		{
			foreach_(const PropertyBuilding& kBuilding, GC.getPropertyInfo(eProperty).getPropertyBuildings())
			{
				const bool bInRange = (iValue >= kBuilding.iMinValue) && (iValue <= kBuilding.iMaxValue);

				if (isActiveBuilding(kBuilding.eBuilding))
				{
					if (!bInRange || !canConstruct(kBuilding.eBuilding, false, false, true, true))
					{
						changeHasBuilding(kBuilding.eBuilding, false);
					}
				}
				else if (bInRange && canConstruct(kBuilding.eBuilding, false, false, true, true))
				{
					changeHasBuilding(kBuilding.eBuilding, true);
				}
			}
		}
	}
}

bool CvCity::isCitySelected() const
{
	return gDLL->getInterfaceIFace()->isCitySelected(const_cast<CvCity*>(this));
}


bool CvCity::canBeSelected() const
{
	PROFILE_EXTRA_FUNC();
	if ((getTeam() == GC.getGame().getActiveTeam()) || GC.getGame().isDebugMode())
	{
		return true;
	}

	if (GC.getGame().getActiveTeam() != NO_TEAM)
	{
		if (plot()->isInvestigate(GC.getGame().getActiveTeam()))
		{
			return true;
		}
	}

	// EspionageEffect
	for (int iLoop = 0; iLoop < GC.getNumEspionageMissionInfos(); iLoop++)
	{
		// Check the XML
		if (GC.getEspionageMissionInfo((EspionageMissionTypes)iLoop).isPassive() && GC.getEspionageMissionInfo((EspionageMissionTypes)iLoop).isInvestigateCity())
		{
			// Is Mission good?
			if (GET_PLAYER(GC.getGame().getActivePlayer()).canDoEspionageMission((EspionageMissionTypes)iLoop, getOwner(), plot(), -1, NULL))
			{
				return true;
			}
		}
	}

	return false;
}


/*DllExport*/ void CvCity::updateSelectedCity(bool bTestProduction)
{
	OutputDebugString(CvString::format("Exe updating selected city (bTestProduction=%d)\n", (int)bTestProduction).c_str());
	algo::for_each(plots(), bind(CvPlot::updateShowCitySymbols, _1));
}


void CvCity::updateYield()
{
	algo::for_each(plots(), bind(CvPlot::updateYield, _1));
}


// XXX kill this?
void CvCity::updateVisibility()
{
	PROFILE_FUNC();

	if (!GC.IsGraphicsInitialized() || !isInViewport())
	{
		return;
	}

	FAssert(GC.getGame().getActiveTeam() != NO_TEAM);

	if (isVisibilitySetup())
	{
		CvDLLEntity::setVisible(isInViewport() && isRevealed(GC.getGame().getActiveTeam(), true));
	}
	else
	{
		setupGraphical();
		m_bVisibilitySetup = true;
	}

}

bool CvCity::isVisibilitySetup() const
{
	return m_bVisibilitySetup;
}


void CvCity::createGreatPeople(UnitTypes eGreatPersonUnit, bool bIncrementThreshold, bool bIncrementExperience)
{
	GET_PLAYER(getOwner()).createGreatPeople(eGreatPersonUnit, bIncrementThreshold, bIncrementExperience, getX(), getY());
}


void CvCity::doTask(TaskTypes eTask, int iData1, int iData2, bool bOption, bool bAlt, bool bShift, bool bCtrl)
{
	switch (eTask)
	{
	case TASK_RAZE:
		GET_PLAYER(getOwner()).raze(this);
		break;

	case TASK_DISBAND:
		GET_PLAYER(getOwner()).disband(this);
		break;

	case TASK_GIFT:
	{
		if (getLiberationPlayer(false) != iData1)
		{
			GET_PLAYER((PlayerTypes)iData1).acquireCity(this, false, true, true);
		}
		else liberate(false);
		break;
	}
	case TASK_KEEP:
		// In this context: bOption = bConquest; bAlt = bTrade.
		CvEventReporter::getInstance().cityAcquiredAndKept((PlayerTypes)iData1, getOwner(), this, bOption, bAlt);
		break;

	case TASK_LIBERATE:
		liberate(iData1 != 0);
		break;

	case TASK_SET_AUTOMATED_CITIZENS:
		setCitizensAutomated(bOption);
		break;

	case TASK_SET_AUTOMATED_PRODUCTION:
		setProductionAutomated(bOption);
		break;

	case TASK_SET_EMPHASIZE:
		AI_setEmphasize(((EmphasizeTypes)iData1), bOption);
		break;

	case TASK_EMPHASIZE_SPECIALIST:
		AI_setEmphasizeSpecialist((SpecialistTypes)iData1, bOption);
		break;

	case TASK_CHANGE_SPECIALIST:
		alterSpecialistCount(((SpecialistTypes)iData1), iData2);
		break;

	case TASK_CHANGE_WORKING_PLOT:
		alterWorkingPlot(iData1);
		break;

	case TASK_CLEAR_WORKING_OVERRIDE:
		clearWorkingOverride(iData1);
		break;

	case TASK_HURRY:
		hurry((HurryTypes)iData1);
		break;

	case TASK_CONSCRIPT:
		conscript();
		break;

	case TASK_CLEAR_ORDERS:
		clearOrderQueue();
		break;

	case TASK_RALLY_PLOT:
		setRallyPlot(GC.getMap().plot(iData1, iData2));
		break;

	case TASK_CLEAR_RALLY_PLOT:
		setRallyPlot(NULL);
		break;
	default:
		FErrorMsg("eTask failed to match a valid option");
		break;
	}
}


int CvCity::getCityPlotIndex(const CvPlot* pPlot) const
{
	return plotCityXY(this, pPlot);
}


CvPlot* CvCity::getCityIndexPlot(int iIndex) const
{
	return plotCity(getX(), getY(), iIndex);
}


bool CvCity::canWork(const CvPlot* pPlot) const
{
	if (pPlot->getWorkingCity() != this)
	{
		return false;
	}

	FAssertMsg(getCityPlotIndex(pPlot) != -1, "getCityPlotIndex(pPlot) is expected to be assigned (not -1)");

	if (getCityPlotIndex(pPlot) >= getNumCityPlots()) return false; // Just in case FAssertMsg doesn't end the function.

	//in the rare case that a city ends up with an invisible animal inside the city or something, the city plot should be made immune to the following effect.
	if (pPlot != plot())
	{
		if (pPlot->plotCheck(PUF_canSiege, getOwner()) != NULL)
		{
			return false;
		}
	}

	if (pPlot->isWater())
	{
		if (!(GET_TEAM(getTeam()).isWaterWork()))
		{
			return false;
		}

		if (pPlot->getBlockadedCount(getTeam()) > 0)
		{
			return false;
		}

		/* Replaced by blockade mission, above
		if (!(pPlot->plotCheck(PUF_canDefend, -1, -1, NO_PLAYER, getTeam())))
		{
			foreach_(const CvPlot* pLoopPlot, pPlot->adjacent())
			{
				if (pLoopPlot->isWater())
				{
					if (pLoopPlot->plotCheck(PUF_canSiege, getOwner()) != NULL)
					{
						return false;
					}
				}
			}
		}
		*/
	}

	if (!(pPlot->hasYield()))
	{
		return false;
	}

	return true;
}


void CvCity::verifyWorkingPlot(int iIndex)
{
	FASSERT_BOUNDS(0, NUM_CITY_PLOTS, iIndex);

	if (isWorkingPlot(iIndex))
	{
		const CvPlot* pPlot = getCityIndexPlot(iIndex);

		if (pPlot != NULL && !canWork(pPlot))
		{
			setWorkingPlot(iIndex, false);

			AI_setAssignWorkDirty(true);
		}
	}
}


void CvCity::verifyWorkingPlots()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < getNumCityPlots(); iI++)
	{
		verifyWorkingPlot(iI);
	}
}


void CvCity::clearWorkingOverride(int iIndex)
{
	CvPlot* pPlot = getCityIndexPlot(iIndex);

	if (pPlot != NULL)
	{
		pPlot->setWorkingCityOverride(NULL);
	}
}


int CvCity::countNumImprovedPlots(ImprovementTypes eImprovement, bool bPotential) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	foreach_(const CvPlot* pLoopPlot, plots())
	{
		if (pLoopPlot->getWorkingCity() == this)
		{
			if (eImprovement != NO_IMPROVEMENT)
			{
				if (pLoopPlot->getImprovementType() == eImprovement ||
					(bPotential && pLoopPlot->canHaveImprovement(eImprovement, getTeam())))
				{
					++iCount;
				}
			}
			else if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				iCount++;
			}
		}
	}

	return iCount;
}


int CvCity::countNumWaterPlots() const
{
	return algo::count_if(plots(),
		bind(CvPlot::getWorkingCity, _1) == this && bind(CvPlot::isWater, _1));
}

int CvCity::countNumRiverPlots() const
{
	return algo::count_if(plots(),
		bind(CvPlot::getWorkingCity, _1) == this && bind(CvPlot::isRiver, _1));
}


int CvCity::findPopulationRank() const
{
	if (!m_bPopulationRankValid)
	{
		const int iRank = 1 + algo::count_if(GET_PLAYER(getOwner()).cities(),
			CvCity::fn::getPopulation() > getPopulation()
			|| (CvCity::fn::getPopulation() == getPopulation() && CvCity::fn::getID() < getID())
		);

		// shenanigans are to get around the const check
		m_bPopulationRankValid = true;
		m_iPopulationRank = iRank;
	}

	return m_iPopulationRank;
}


int CvCity::findBaseYieldRateRank(YieldTypes eYield) const
{
	if (!m_abBaseYieldRankValid[eYield])
	{
		//TB Traits begin
		const int iRate = getBaseYieldRate(eYield);
		//TB Traits end

		const int iRank = 1 + algo::count_if(GET_PLAYER(getOwner()).cities(),
			CvCity::fn::getBaseYieldRate(eYield) > iRate
			|| (CvCity::fn::getBaseYieldRate(eYield) == iRate && CvCity::fn::getID() < getID())
		);
		m_abBaseYieldRankValid[eYield] = true;
		m_aiBaseYieldRank[eYield] = iRank;
	}
	return m_aiBaseYieldRank[eYield];
}


int CvCity::findYieldRateRank(YieldTypes eYield) const
{
	PROFILE_FUNC();

	if (!m_abYieldRankValid[eYield])
	{
		const int iRate = getYieldRate100(eYield);

		const int iRank = 1 + algo::count_if(GET_PLAYER(getOwner()).cities(),
			CvCity::fn::getYieldRate100(eYield) > iRate
			|| (CvCity::fn::getYieldRate100(eYield) == iRate && CvCity::fn::getID() < getID())
		);
		m_abYieldRankValid[eYield] = true;
		m_aiYieldRank[eYield] = iRank;
	}
	return m_aiYieldRank[eYield];
}


int CvCity::findCommerceRateRank(CommerceTypes eCommerce) const
{
	if (!m_abCommerceRankValid[eCommerce])
	{
		int iRate = getCommerceRateTimes100(eCommerce);

		const int iRank = 1 + algo::count_if(GET_PLAYER(getOwner()).cities(),
			CvCity::fn::getCommerceRateTimes100(eCommerce) > iRate
			|| (CvCity::fn::getCommerceRateTimes100(eCommerce) == iRate && CvCity::fn::getID() < getID())
		);
		m_abCommerceRankValid[eCommerce] = true;
		m_aiCommerceRank[eCommerce] = iRank;
	}
	return m_aiCommerceRank[eCommerce];
}


bool CvCity::isPlotTrainable(UnitTypes eUnit, bool bTestVisible) const
{
	PROFILE_FUNC();

	if (!bTestVisible)
	{
		const CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);
		const CvPlayer& pPlayer = GET_PLAYER(getOwner());

		if (kUnit.isStateReligion() && pPlayer.getStateReligion() != NO_RELIGION && !isHasReligion(pPlayer.getStateReligion()))
		{
			return false;
		}

		for (int iI = 0; iI < kUnit.getNumPrereqAndBuildings(); ++iI)
		{
			const BuildingTypes eBuildingX = (BuildingTypes)kUnit.getPrereqAndBuilding(iI);
			if (!GET_TEAM(getTeam()).isObsoleteBuilding(eBuildingX) && !isActiveBuilding(eBuildingX))
			{
				const SpecialBuildingTypes eSpecialBuilding = GC.getBuildingInfo(eBuildingX).getSpecialBuilding();

				if (eSpecialBuilding == NO_SPECIALBUILDING || !pPlayer.isSpecialBuildingNotRequired(eSpecialBuilding))
				{
					return false;
				}
			}
		}

		{
			bool bFound = true;
			for (int iI = 0; iI < kUnit.getPrereqOrBuildingsNum(); iI++)
			{
				if (!GET_TEAM(getTeam()).isObsoleteBuilding(kUnit.getPrereqOrBuilding(iI)))
				{
					bFound = false;
					if (isActiveBuilding(kUnit.getPrereqOrBuilding(iI)))
					{
						bFound = true;
						break;
					}
				}
			}
			if (!bFound) return false;
		}

		if (kUnit.getTrainCondition())
		{
			if (!kUnit.getTrainCondition()->evaluate(const_cast<CvGameObjectCity*>(getGameObject())))
			{
				return false;
			}
		}
	}
	return true;
}

// Returns true if the city can train a unit that force obsoletes eUnit
bool CvCity::isSupersedingUnitAvailable(UnitTypes eUnit) const
{
	PROFILE_FUNC();
	FAssertMsg(eUnit != NO_UNIT, "eUnit is expected to be assigned (not NO_UNIT)");

	for (int iI = 0; iI < GC.getUnitInfo(eUnit).getNumSupersedingUnits(); ++iI)
	{
		if (canTrain((UnitTypes) GC.getUnitInfo(eUnit).getSupersedingUnit(iI), false, false, false, true, true))
		{
			return true;
		}
	}
	return false;
}


// Returns one of the upgrades...
UnitTypes CvCity::allUpgradesAvailable(UnitTypes eUnit, int iUpgradeCount) const
{
	PROFILE_FUNC();

	UnitTypes eResult = NO_UNIT;
	bool bHasCachedResult = false;

	//OutputDebugString(CvString::format("allUpgradesAvailable for %d (recursion depth %d)\n", eUnit, iUpgradeCount).c_str());
	if (iUpgradeCount == 0)
	{
		stdext::hash_map<UnitTypes, UnitTypes>::const_iterator itr = m_eCachedAllUpgradesResultsRoot.find(eUnit);
		if (itr != m_eCachedAllUpgradesResultsRoot.end())
		{
			//OutputDebugString(CvString::format("\t...cached result %d\n", itr->second).c_str());
			eResult = itr->second;
			bHasCachedResult = true;
		}
	}
	else
	{
		stdext::hash_map<UnitTypes, UnitTypes>::const_iterator itr = m_eCachedAllUpgradesResults.find(eUnit);
		if (itr != m_eCachedAllUpgradesResults.end())
		{
			//OutputDebugString(CvString::format("\t...cached result %d\n", itr->second).c_str());
			eResult = itr->second;
			bHasCachedResult = true;
		}
	}

	if (!bHasCachedResult)
	{
		/************************************************************************************************/
		/* REVDCM                                 04/16/10                                phungus420    */
		/*                                                                                              */
		/* CanTrain Performance                                                                         */
		/************************************************************************************************/
		const int iNumUnitInfos = GC.getNumUnitInfos();

		FAssertMsg(eUnit != NO_UNIT, "eUnit is expected to be assigned (not NO_UNIT)");

		if (iUpgradeCount <= iNumUnitInfos)
		{
			const CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);

			UnitTypes eUpgradeUnit = NO_UNIT;

			bool bUpgradeFound = false;
			bool bUpgradeAvailable = false;
			bool bUpgradeUnavailable = false;

			for (int iI = 0; iI < kUnit.getNumUnitUpgrades(); iI++)
			{
				bUpgradeFound = true;

				UnitTypes eTempUnit = (UnitTypes) kUnit.getUnitUpgrade(iI);
				if (kUnit.isSupersedingUnit(eTempUnit))
				{
					// if this is avaliable then the unit won't be buildable anyhow, so it makes little sense to consider it here.
					continue;
				}

				eTempUnit = allUpgradesAvailable(eTempUnit, (iUpgradeCount + 1));

				if (eTempUnit != NO_UNIT)
				{
					eUpgradeUnit = eTempUnit;
					bUpgradeAvailable = true;
				}
				else
				{
					bUpgradeUnavailable = true;
				}
			}

			if (iUpgradeCount > 0)
			{
				if (bUpgradeFound && bUpgradeAvailable)
				{
					FAssertMsg(eUpgradeUnit != NO_UNIT, "eUpgradeUnit is expected to be assigned (not NO_UNIT)");
					eResult = eUpgradeUnit;
				}
				else if (canTrain(eUnit, false, false, false, true))
				{
					eResult = eUnit;
				}

				//	Cache the result so that we don't have to recalculate it multiple times
				//OutputDebugString(CvString::format("\t...caching result %d\n", eResult).c_str());
				m_eCachedAllUpgradesResults[eUnit] = eResult;
			}
			else
			{
				if (bUpgradeFound && !bUpgradeUnavailable)
				{
					eResult = eUpgradeUnit;
				}

				//	Cache the result so that we don't have to recalculate it multiple times
				//OutputDebugString(CvString::format("\t...caching result %d\n", eResult).c_str());
				m_eCachedAllUpgradesResultsRoot[eUnit] = eResult;
			}
		}
	}

	return eResult;
}


int CvCity::getMaxNumWorldWonders() const
{
	return GC.getCultureLevelInfo(getCultureLevel()).getMaxWorldWonders();
}

bool CvCity::isWorldWondersMaxed() const
{
	if (GC.getGame().isOption(GAMEOPTION_CHALLENGE_ONE_CITY))
	{
		return false;
	}
	if (GC.getGame().isOption(GAMEOPTION_NO_WONDER_LIMIT))
	{
		return false;
	}
	if (getNumWorldWonders() >= getMaxNumWorldWonders())
	{
		return true;
	}
	return false;
}


int CvCity::getMaxNumTeamWonders() const
{
	return GC.getCultureLevelInfo(getCultureLevel()).getMaxTeamWonders();
}

bool CvCity::isTeamWondersMaxed() const
{
	if (GC.getGame().isOption(GAMEOPTION_CHALLENGE_ONE_CITY))
	{
		return false;
	}
	if (GC.getGame().isOption(GAMEOPTION_NO_WONDER_LIMIT))
	{
		return false;
	}
	if (getNumTeamWonders() >= getMaxNumTeamWonders())
	{
		return true;
	}
	return false;
}


int CvCity::getMaxNumNationalWonders() const
{
	return GC.getGame().isOption(GAMEOPTION_CHALLENGE_ONE_CITY) ? GC.getCultureLevelInfo(getCultureLevel()).getMaxNationalWondersOCC() : GC.getCultureLevelInfo(getCultureLevel()).getMaxNationalWonders();
}

bool CvCity::isNationalWondersMaxed() const
{
	if (GC.getGame().isOption(GAMEOPTION_NO_WONDER_LIMIT))
	{
		return false;
	}
	if (getMaxNumNationalWonders() != -1 && getNumNationalWonders() >= getMaxNumNationalWonders())
	{
		return true;
	}
	return false;
}

bool CvCity::canTrainInternal(UnitTypes eUnit, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreUpgrades) const
{
	PROFILE("CvCity::canTrainInternal (units)");

	if (eUnit == NO_UNIT)
	{
		return false;
	}
	const CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);

	if (kUnit.getPrereqReligion() != NO_RELIGION && !isHasReligion((ReligionTypes)kUnit.getPrereqReligion()))
	{
		return false;
	}
	if (kUnit.getPrereqCorporation() != NO_CORPORATION && !isActiveCorporation((CorporationTypes)kUnit.getPrereqCorporation()))
	{
		return false;
	}
	if (kUnit.getHolyCity() != NO_RELIGION && !isHolyCity((ReligionTypes)kUnit.getHolyCity()))
	{
		return false;
	}

	if (!plot()->canTrain(eUnit, bTestVisible))
	{
		return false;
	}

	if (!bIgnoreUpgrades && isSupersedingUnitAvailable(eUnit))
	{
		return false;
	}

	if (!isPlotTrainable(eUnit, bTestVisible))
	{
		return false;
	}

	if (kUnit.isForceUpgrade() && canUpgradeUnit(eUnit))
	{
		return false;
	}

	if (isNPC() && GC.getCivilizationInfo(getCivilizationType()).isStronglyRestricted())
	{
		bool bValid = false;
		for (int iI = 0; iI < kUnit.getNumEnabledCivilizationTypes(); iI++)
		{
			if (getCivilizationType() == kUnit.getEnabledCivilizationType(iI).eCivilization)
			{
				bValid = true;
				break;
			}
		}
		if (!bValid)
		{
			return false;
		}
	}

	if (!bTestVisible && kUnit.isRequiresStateReligionInCity())
	{
		const ReligionTypes eStateReligion = GET_PLAYER(getOwner()).getStateReligion();
		if (NO_RELIGION == eStateReligion || !isHasReligion(eStateReligion))
		{
			return false;
		}
	}

	if (kUnit.getPrereqVicinityBonus() != NO_BONUS && !hasVicinityBonus((BonusTypes)kUnit.getPrereqVicinityBonus()))
	{
		return false;
	}

	if (!bTestVisible)
	{
		bool bHasAnyVicinityBonus = false;
		bool bRequiresAnyVicinityBonus = false;
		foreach_(const BonusTypes ePreReqBonus, kUnit.getPrereqOrVicinityBonuses())
		{
			bRequiresAnyVicinityBonus = true;
			if (hasVicinityBonus(ePreReqBonus))
			{
				bHasAnyVicinityBonus = true;
				break;
			}
		}
		if (bRequiresAnyVicinityBonus && !bHasAnyVicinityBonus)
		{
			return false;
		}
	}
	if (GC.getGame().isOption(GAMEOPTION_ADVANCED_REALISTIC_CORPORATIONS) && GET_PLAYER(getOwner()).isNoForeignCorporations())
	{
		for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
		{
			if (kUnit.getCorporationSpreads(iI) > 0)
			{
				return false;
			}
		}
	}

	if (!bIgnoreUpgrades && allUpgradesAvailable(eUnit) != NO_UNIT)
	{
		return false;
	}
	return true;
}

#ifdef CAN_TRAIN_CACHING
#ifdef _DEBUG
//	Uncomment this for consistency testing of the canTrain cache
//#define VALIDATE_CAN_TRAIN_CACHE_CONSISTENCY
#endif

void CvCity::populateCanTrainCache(bool bUnconditional) const
{
	PROFILE_FUNC();

	if (bUnconditional || !m_canTrainCachePopulated)
	{
		clearCanTrainCache();

		int iCount = 0;
		const int numUnitInfos = GC.getNumUnitInfos();
		for (int iI = 0; iI < numUnitInfos; iI++)
		{
			if (canTrain((UnitTypes)iI))
			{
				m_canTrainCacheUnits[(UnitTypes)iI] = true;
				iCount++;
			}
		}

		if (iCount == 0)
		{
			OutputDebugString("Nothing trainable!\n");
		}

		m_canTrainCachePopulated = true;
		m_canTrainCacheDirty = false;

	}
}

void CvCity::clearCanTrainCache() const
{
	m_canTrainCachePopulated = false;
	m_canTrainCacheUnits.clear();

	clearUpgradeCache(NO_UNIT);
}
#endif

void CvCity::clearUpgradeCache(UnitTypes eUnit) const
{
	if (eUnit == NO_UNIT)
	{
		m_eCachedAllUpgradesResults.clear();
		m_eCachedAllUpgradesResultsRoot.clear();
	}
	else
	{
		m_eCachedAllUpgradesResults.erase(eUnit);
		m_eCachedAllUpgradesResultsRoot.erase(eUnit);
	}
}

bool CvCity::canTrain(UnitTypes eUnit, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreUpgrades, bool bPropertySpawn) const
{
	if (!GET_PLAYER(getOwner()).canTrain(eUnit, bContinue, bTestVisible, bIgnoreCost, bPropertySpawn))
	{
		return false;
	}
#ifdef CAN_TRAIN_CACHING
	if (!bContinue && !bTestVisible && !bIgnoreCost && !bIgnoreUpgrades)
	{
		if (m_canTrainCachePopulated)
		{
			bool bHaveCachedResult;
			bool bResult;

			if (m_canTrainCacheDirty)
			{
				PROFILE("CvCity::canTrain.ProcessDirtyCache");

				//	Needs repopulating
				populateCanTrainCache();
			}

			stdext::hash_map<UnitTypes, bool>::iterator itr = m_canTrainCacheUnits.find(eUnit);
			if (itr == m_canTrainCacheUnits.end())
			{
#ifdef VALIDATE_CAN_TRAIN_CACHE_CONSISTENCY
				if (canTrainInternal(eUnit))
				{
					FErrorMsg("Consistency check failure in canTrain cache - false negative\n");
				}
#endif
				bResult = false;
				bHaveCachedResult = true;
			}
			else if (itr->second)
			{
#ifdef VALIDATE_CAN_TRAIN_CACHE_CONSISTENCY
				if (!canTrainInternal(eUnit))
				{
					FErrorMsg("Consistency check failure in canTrain cache - false positive\n");
				}
#endif
				bResult = true;
				bHaveCachedResult = true;
			}
			else	//	In map but with false => recalculate
			{
				bHaveCachedResult = false;
			}

			if (!bHaveCachedResult)
			{
				bResult = canTrainInternal(eUnit);

				if (bResult)
				{
					m_canTrainCacheUnits[eUnit] = true;
				}
				else
				{
					m_canTrainCacheUnits.erase(eUnit);
				}

			}

			return bResult;
		}
	}
#endif

	PROFILE("canTrain.NonStandard");
	return canTrainInternal(eUnit, bContinue, bTestVisible, bIgnoreCost, bIgnoreUpgrades);
}

void CvCity::invalidateCachedCanTrainForUnit(UnitTypes eUnit) const
{
	PROFILE_FUNC();

	if (m_canTrainCachePopulated)
	{
		PROFILE("CvCity::invalidateCachedCanTrainForUnit");

		if (eUnit == NO_UNIT)
		{
			m_canTrainCacheDirty = true;	//	Entire map dirty
		}
		else
		{
			m_canTrainCacheUnits[eUnit] = false;
		}
	}

	clearUpgradeCache(eUnit);

}

bool CvCity::canTrain(UnitCombatTypes eUnitCombat) const
{
	PROFILE_FUNC();
	const int num = GC.getNumUnitInfos();

	for (int i = 0; i < num; i++)
	{
		if (GC.getUnitInfo((UnitTypes) i).hasUnitCombat(eUnitCombat) && canTrain((UnitTypes) i))
		{
			return true;
		}
	}
	return false;
}

//	KOSHLING - cache can construct values
#ifdef _DEBUG
//	Uncomment to add runtime results checking
//#define VALIDATE_CAN_CONSTRUCT_CACHE
#endif

void CvCity::FlushCanConstructCache(BuildingTypes eBuilding)
{
	//OutputDebugString(CvString::format("[%d] FlushCanConstructCache (%d), workitem priority = %08lx\n", GetCurrentThreadId(), eBuilding, (m_workItem == NULL ? -1 : m_workItem->GetPriority())).c_str());

	if (eBuilding == NO_BUILDING)
	{
		SAFE_DELETE(m_bCanConstruct);
	}
	else if (m_bCanConstruct != NULL)
	{
		(*m_bCanConstruct).erase(eBuilding);
	}
}

void CvCity::NoteBuildingNoLongerConstructable(BuildingTypes eBuilding) const
{
	if (m_bCanConstruct != NULL)
	{
		(*m_bCanConstruct)[eBuilding] = false;
	}
}

bool CvCity::canConstruct(BuildingTypes eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreAmount, bool bIgnoreBuildings, TechTypes eIgnoreTechReq, int* probabilityEverConstructable, bool bAffliction, bool bExposed) const
{
	PROFILE_FUNC();

	if (eBuilding == NO_BUILDING)
	{
		return false;
	}
	if (!bContinue && !bTestVisible && !bIgnoreCost && !bIgnoreAmount && !bIgnoreBuildings && eIgnoreTechReq == NO_TECH && probabilityEverConstructable == NULL && !bAffliction && !bExposed)
	{
		bool bResult;
		bool bHaveCachedResult;

		if (m_bCanConstruct == NULL)
		{

			m_bCanConstruct = new std::map<int, bool>();
			bHaveCachedResult = false;
		}
		else
		{
			std::map<int, bool>::const_iterator itr = m_bCanConstruct->find(eBuilding);
			if (itr == m_bCanConstruct->end())
			{
				bHaveCachedResult = false;
			}
			else
			{
				bResult = itr->second;
				bHaveCachedResult = true;
#ifdef VALIDATE_CAN_CONSTRUCT_CACHE
				//	Verify if required
				if (bResult != canConstructInternal(eBuilding, bContinue, bTestVisible, bIgnoreCost, bIgnoreAmount, NO_BUILDING, bIgnoreBuildings, eIgnoreTechReq, NULL, bAffliction, bExposed))
				{
					MessageBox(NULL, "canConstruct cached result mismatch", "cvGameCore", MB_OK);
					FErrorMsg("canConstruct cached result mismatch");
				}
#endif
			}
		}

		if (!bHaveCachedResult)
		{
			bResult = canConstructInternal(eBuilding, bContinue, bTestVisible, bIgnoreCost, bIgnoreAmount, NO_BUILDING, bIgnoreBuildings, eIgnoreTechReq, NULL, bAffliction, bExposed);
			{

				if (m_bCanConstruct == NULL)
				{

					m_bCanConstruct = new std::map<int, bool>();
				}
				(*m_bCanConstruct)[eBuilding] = bResult;
			}
		}

		return bResult;
	}
	return canConstructInternal(eBuilding, bContinue, bTestVisible, bIgnoreCost, bIgnoreAmount, NO_BUILDING, bIgnoreBuildings, eIgnoreTechReq, probabilityEverConstructable, bAffliction, bExposed);
}
//	KOSHLING - Can construct cache end

bool CvCity::canConstructInternal(BuildingTypes eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreAmount, BuildingTypes withExtraBuilding, bool bIgnoreBuildings, TechTypes eIgnoreTechReq, int* probabilityEverConstructable, bool bAffliction, bool bExposed) const
{
	PROFILE_FUNC();

	if (probabilityEverConstructable != NULL)
	{
		*probabilityEverConstructable = 0;
	}

	if (eBuilding == NO_BUILDING)
	{
		return false;
	}

	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	if (GC.getCivilizationInfo(getCivilizationType()).isStronglyRestricted() && isNPC())
	{
		bool bQual = false;
		for (int iI = 0; iI < kBuilding.getNumEnabledCivilizationTypes(); iI++)
		{
			if (getCivilizationType() == kBuilding.getEnabledCivilizationType(iI).eCivilization)
			{
				bQual = true;
				break;
			}
		}
		if (!bQual)
		{
			return false;
		}
	}

	//ls612: No Holy City Tag
	if (!bExposed && kBuilding.isNoHolyCity() && isHolyCity())
	{
		return false;
	}

	// Toffer - An extension can't exist without that which it extends.
	if (kBuilding.getExtendsBuilding() > NO_BUILDING && !hasBuilding(kBuilding.getExtendsBuilding()))
	{
		return false;
	}

	if (
		!GET_PLAYER(getOwner()).canConstruct(
			eBuilding, bContinue, bTestVisible, bIgnoreCost, eIgnoreTechReq,
			probabilityEverConstructable, bAffliction, bExposed
		)
	) return false;

	if (!bIgnoreAmount && hasBuilding(eBuilding))
	{
		return false;
	}
	if (isDisabledBuilding(eBuilding))
	{
		return false;
	}

	if (!bExposed)
	{
		if (kBuilding.needStateReligionInCity())
		{
			const ReligionTypes eStateReligion = GET_PLAYER(getOwner()).getStateReligion();

			if (NO_RELIGION == eStateReligion || !isHasReligion(eStateReligion))
			{
				if (probabilityEverConstructable != NULL)
				{
					*probabilityEverConstructable = 80;
				}
				return false;
			}
		}

		const ReligionTypes eReligion = (ReligionTypes)kBuilding.getPrereqReligion();

		if (eReligion != NO_RELIGION && !isHasReligion(eReligion))
		{
			if (probabilityEverConstructable != NULL)
			{
				*probabilityEverConstructable = 50;
			}
			return false;
		}

		const CorporationTypes prereqCorp = (CorporationTypes)kBuilding.getPrereqCorporation();

		if (prereqCorp != NO_CORPORATION && !isHasCorporation(prereqCorp))
		{
			if (probabilityEverConstructable != NULL)
			{
				*probabilityEverConstructable = 30;
			}
			return false;
		}

		const CorporationTypes foundsCorp = (CorporationTypes)kBuilding.getFoundsCorporation();

		if (foundsCorp != NO_CORPORATION)
		{
			if (GC.getGame().isCorporationFounded(foundsCorp))
			{
				return false;
			}

			for (int iCorporation = 0; iCorporation < GC.getNumCorporationInfos(); ++iCorporation)
			{
				if (isHeadquarters((CorporationTypes)iCorporation)
				&& GC.getGame().isCompetingCorporation((CorporationTypes)iCorporation, foundsCorp))
				{
					return false;
				}
			}
		}

		if (!isValidBuildingLocation(eBuilding))
		{
			return false;
		}

		if (kBuilding.isGovernmentCenter() && isGovernmentCenter())
		{
			if (probabilityEverConstructable != NULL)
			{
				*probabilityEverConstructable = 10;
			}
			return false;
		}
	}

	if (!bTestVisible && !bExposed)
	{
		if (!bAffliction)
		{
			if (!bContinue && getFirstBuildingOrder(eBuilding) != -1)
			{
				return false;
			}

			if (isLimitedWonder(eBuilding))
			{
				if (isNPC())
				{
					return false;
				}
				if (!kBuilding.isNoLimit())
				{
					if (isWorldWonder(eBuilding))
					{
						if (isWorldWondersMaxed())
						{
							return false;
						}
					}
					else if (isTeamWonder(eBuilding))
					{
						if (isTeamWondersMaxed())
						{
							return false;
						}
					}
					else if (isNationalWonder(eBuilding) && isNationalWondersMaxed())
					{
						return false;
					}
				}
			}
		}

		if (kBuilding.getHolyCity() != NO_RELIGION
		&& !isHolyCity((ReligionTypes)kBuilding.getHolyCity()))
		{
			return false;
		}

		if (kBuilding.getPrereqAndBonus() != NO_BONUS
		&& !hasBonus((BonusTypes)kBuilding.getPrereqAndBonus()))
		{
			if (probabilityEverConstructable != NULL)
			{
				*probabilityEverConstructable = 50;
			}
			return false;
		}

		const CorporationTypes foundsCorp = (CorporationTypes)kBuilding.getFoundsCorporation();

		if (foundsCorp != NO_CORPORATION && !bAffliction)
		{
			if (GC.getGame().isCorporationFounded(foundsCorp)
			|| GET_PLAYER(getOwner()).isNoCorporations())
			{
				return false;
			}
			bool bHasBonus = false;
			bool bRequiresBonus = false;
			foreach_(const BonusTypes eBonus, GC.getCorporationInfo(foundsCorp).getPrereqBonuses())
			{
				bRequiresBonus = true;
				if (hasBonus(eBonus))
				{
					bHasBonus = true;
					break;
				}
			}

			if (bRequiresBonus && !bHasBonus)
			{
				if (probabilityEverConstructable != NULL)
				{
					*probabilityEverConstructable = 30;
				}
				return false;
			}
		}

		if (!bAffliction) //bAffliction ONLY applies during the Outbreaks and Afflictions option being on!
		{
			if (!(*getPropertiesConst() <= *(kBuilding.getPrereqMaxProperties()))
			|| !(*getPropertiesConst() >= *(kBuilding.getPrereqMinProperties())))
			{
				return false;
			}
		}

		if (plot()->getLatitude() > kBuilding.getMaxLatitude()
		||  plot()->getLatitude() < kBuilding.getMinLatitude())
		{
			return false;
		}

		const int iPrereqPopulation = (
			std::max(
				kBuilding.getPrereqPopulation(),
				1 + getNumPopulationEmployed() + kBuilding.getNumPopulationEmployed()
			)
		);
		if (iPrereqPopulation > 1 && getPopulation() < iPrereqPopulation)
		{
			if (probabilityEverConstructable != NULL)
			{
				*probabilityEverConstructable = 50;
			}
			return false;
		}

		if (kBuilding.getPrereqCultureLevel() != NO_CULTURELEVEL
		&& getCultureLevel() < kBuilding.getPrereqCultureLevel())
		{
			if (probabilityEverConstructable != NULL)
			{
				*probabilityEverConstructable = 50;
			}
			return false;
		}

		const BuildingTypes ePrereqBuilding = kBuilding.getPrereqAnyoneBuilding();

		if (ePrereqBuilding != NO_BUILDING && GC.getGame().getBuildingCreatedCount(ePrereqBuilding) == 0)
		{
			return false;
		}

		bool bRequiresBonus = false;
		bool bNeedsBonus = true;
		foreach_(const BonusTypes ePrereqBonus, kBuilding.getPrereqOrBonuses())
		{
			bRequiresBonus = true;

			if (hasBonus(ePrereqBonus))
			{
				bNeedsBonus = false;
				break;
			}
		}
		if (bRequiresBonus && bNeedsBonus)
		{
			if (probabilityEverConstructable != NULL)
			{
				*probabilityEverConstructable = 50;
			}
			return false;
		}
	}

	/************************************************************************************************/
	/* Koshling - moved this so non-met vicinity required buildings get hidden in city screen even	*/
	/* when not ordinarily hiding uncosntructable (since you cannot influence your vicinity bonuses	*/
	/* so its not useful information for the most part												*/
	/************************************************************************************************/
	// Blaze - Might be useful in some situations - is in radius, tile not owned - but that's a TODO
	if (!bExposed)
	{
		if (kBuilding.getPrereqVicinityBonus() != NO_BONUS
		&& !hasVicinityBonus((BonusTypes)kBuilding.getPrereqVicinityBonus()))
		{
			if (probabilityEverConstructable != NULL)
			{
				*probabilityEverConstructable = 5;
			}
			return false;
		}
		if (kBuilding.getPrereqRawVicinityBonus() != NO_BONUS
		&& !hasRawVicinityBonus((BonusTypes)kBuilding.getPrereqRawVicinityBonus()))
		{
			if (probabilityEverConstructable != NULL)
			{
				*probabilityEverConstructable = 5;
			}
			return false;
		}
		//Hide Buildings that shouldn't appear in the early game and require other buildings
		const bool bTest = (
			(
				kBuilding.getPrereqAndTech() == NO_TECH &&
				kBuilding.getObsoleteTech() == NO_TECH
			)
			? true : !bTestVisible
		);
		if (bTest && !bIgnoreBuildings)
		{
			for (int iI = 0; iI < kBuilding.getNumPrereqInCityBuildings(); iI++)
			{
				const BuildingTypes ePrereqBuilding = static_cast<BuildingTypes>(kBuilding.getPrereqInCityBuilding(iI));

				if (ePrereqBuilding != withExtraBuilding 
				&& !GET_TEAM(getTeam()).isObsoleteBuilding(ePrereqBuilding)
				&& !isActiveBuilding(ePrereqBuilding))
				{
					if (probabilityEverConstructable)
					{
						*probabilityEverConstructable = 25;
					}
					return false;
				}
			}
		}
	}

	if (!bTestVisible && !bIgnoreBuildings || bAffliction)
	{
		for (int iI = 0; iI < kBuilding.getNumPrereqNotInCityBuildings(); ++iI)
		{
			if (hasBuilding(static_cast<BuildingTypes>(kBuilding.getPrereqNotInCityBuilding(iI))))
			{
				return false;
			}
		}

		bool bValid = false;
		bool bRequires = false;
		for (int iI = 0; iI < kBuilding.getNumPrereqOrBuilding(); ++iI)
		{
			const BuildingTypes ePrereqBuilding = static_cast<BuildingTypes>(kBuilding.getPrereqOrBuilding(iI));
			if (!GET_TEAM(getTeam()).isObsoleteBuilding(ePrereqBuilding))
			{
				bRequires = true;
				if (withExtraBuilding == ePrereqBuilding || isActiveBuilding(ePrereqBuilding))
				{
					bValid = true;
					break;
				}
			}
		}

		if (bRequires && !bValid)
		{
			if (probabilityEverConstructable != NULL)
			{
				*probabilityEverConstructable = 25;
			}
			return false;
		}
		if (kBuilding.isPrereqPower() && !isPower())
		{
			if (probabilityEverConstructable != NULL)
			{
				*probabilityEverConstructable = 25;
			}
			return false;
		}
	}

	if (!bAffliction)
	{
		//Can not construct replaced buildings.
		for (int iI = 0; iI < kBuilding.getNumReplacementBuilding(); ++iI)
		{
			const BuildingTypes eReplacement = (BuildingTypes)kBuilding.getReplacementBuilding(iI);

			if (isActiveBuilding(eReplacement)
			// Toffer - This is not the right place to do HIDE_REPLACED_BUILDINGS...
			//	Should be an interface only thing.
			|| GET_PLAYER(getOwner()).isModderOption(MODDEROPTION_HIDE_REPLACED_BUILDINGS)
			&& canConstruct(eReplacement, true, false, false, true))
			{
				return false;
			}
		}
	}

	// Koshling - Always hide unbuildable due to vicinity bonuses, it's not really useful to see them
	// Blaze - Might be useful in some situations - is in radius, tile not owned - but that's a TODO
	if (!bExposed)
	{
		bool bHasAnyVicinityBonus = false;
		bool bRequiresAnyVicinityBonus = false;
		foreach_(const BonusTypes ePrereqBonus, kBuilding.getPrereqOrVicinityBonuses())
		{
			bRequiresAnyVicinityBonus = true;
			if (hasVicinityBonus(ePrereqBonus))
			{
				bHasAnyVicinityBonus = true;
				break;
			}
		}
		if (bRequiresAnyVicinityBonus && !bHasAnyVicinityBonus)
		{
			if (probabilityEverConstructable != NULL)
			{
				*probabilityEverConstructable = 5;
			}
			return false;
		}
		bool bHasAnyRawVicinityBonus = false;
		bool bRequiresAnyRawVicinityBonus = false;
		foreach_(const BonusTypes bonus, kBuilding.getPrereqOrRawVicinityBonuses())
		{
			bRequiresAnyRawVicinityBonus = true;
			if (hasRawVicinityBonus(bonus))
			{
				bHasAnyRawVicinityBonus = true;
				break;
			}
		}
		if (bRequiresAnyRawVicinityBonus && !bHasAnyRawVicinityBonus)
		{
			if (probabilityEverConstructable != NULL)
			{
				*probabilityEverConstructable = 5;
			}
			return false;
		}
	}

	if (!bTestVisible && kBuilding.getConstructCondition() && !bExposed)
	{
		const CvGameObjectCity* pObject = getGameObject();
		if (withExtraBuilding != NO_BUILDING)
		{
			// add the extra building and its bonuses to the override to see if they influence the construct condition of this building
			std::vector<GOMOverride> queries;
			GOMOverride query = { pObject, GOM_BUILDING, withExtraBuilding, true };
			queries.push_back(query);

			query.GOM = GOM_BONUS;
			foreach_(const BonusModifier& pair, GC.getBuildingInfo(withExtraBuilding).getFreeBonuses())
			{
				query.id = pair.first;
				queries.push_back(query);
			}

			const BoolExprChange result = kBuilding.getConstructCondition()->evaluateChange(pObject, queries);
			if ((result == BOOLEXPR_CHANGE_REMAINS_FALSE) || (result == BOOLEXPR_CHANGE_BECOMES_FALSE))
			{
				return false;
			}
		}
		else if (!kBuilding.getConstructCondition()->evaluate(pObject))
		{
			return false;
		}
	}
	return true;
}


bool CvCity::canCreate(ProjectTypes eProject, bool bContinue, bool bTestVisible) const
{
	if (!GET_PLAYER(getOwner()).canCreate(eProject, bContinue, bTestVisible))
	{
		return false;
	}

	if (!isMapCategory(*plot(), GC.getProjectInfo(eProject)))
	{
		return false;
	}

	return true;
}


bool CvCity::canMaintain(ProcessTypes eProcess) const
{
	if (!GET_PLAYER(getOwner()).canMaintain(eProcess)
	|| Cy::call<bool>(PYGameModule, "cannotMaintain", Cy::Args() << const_cast<CvCity*>(this) << eProcess))
	{
		return false;
	}
	return true;
}


int CvCity::getFoodTurnsLeft() const
{
	const int iFoodDifference = foodDifference();

	if (iFoodDifference <= 0)
	{
		return growthThreshold() - getFood();
	}
	const int iFoodLeft = growthThreshold() - getFood();

	int iTurnsLeft = iFoodLeft / iFoodDifference;

	if (iTurnsLeft * iFoodDifference < iFoodLeft)
	{
		iTurnsLeft++;
	}

	return std::max(1, iTurnsLeft);
}


bool CvCity::isProduction() const
{
	return getHeadOrder();
}


bool CvCity::isProductionLimited() const
{
	bst::optional<OrderData> order = getHeadOrder();

	if (order != NULL)
	{
		switch (order->eOrderType)
		{
		case ORDER_TRAIN:
			return isLimitedUnit((UnitTypes)EXTERNAL_ORDER_IDATA(order->iData1));
			break;

		case ORDER_CONSTRUCT:
			return isLimitedWonder(static_cast<BuildingTypes>(order->iData1));
			break;

		case ORDER_CREATE:
			return isLimitedProject((ProjectTypes)(order->iData1));
			break;

		case ORDER_MAINTAIN:
		case ORDER_LIST:
			break;

		default:
			FErrorMsg("order->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return false;
}

bool CvCity::isProductionUnitCombat(int iIndex) const
{
	bst::optional<OrderData> order = getHeadOrder();

	if (order)
	{
		switch (order->eOrderType)
		{
		case ORDER_TRAIN:
			return GC.getUnitInfo(order->getUnitType()).hasUnitCombat((UnitCombatTypes)iIndex);
		case ORDER_CONSTRUCT:
			return false;
		case ORDER_CREATE:
			return false;
		case ORDER_MAINTAIN:
			return false;
		case ORDER_LIST:
			return false;
		default:
			FErrorMsg("order->eOrderType failed to match a valid option");
			break;
		}
	}
	return false;
}


bool CvCity::isProductionUnit() const
{
	bst::optional<OrderData> order = getHeadOrder();
	return order && order->eOrderType == ORDER_TRAIN;
}


bool CvCity::isProductionBuilding() const
{
	bst::optional<OrderData> order = getHeadOrder();
	return order && order->eOrderType == ORDER_CONSTRUCT;
}


bool CvCity::isProductionProject() const
{
	bst::optional<OrderData> order = getHeadOrder();
	return order && order->eOrderType == ORDER_CREATE;
}


bool CvCity::isProductionProcess() const
{
	bst::optional<OrderData> order = getHeadOrder();
	return order && order->eOrderType == ORDER_MAINTAIN;
}


bool CvCity::canContinueProduction(const OrderData& order) const
{
	switch (order.eOrderType)
	{
	case ORDER_TRAIN:
		return canTrain(order.getUnitType(), true);
	case ORDER_CONSTRUCT:
		return canConstruct(order.getBuildingType(), true);
	case ORDER_CREATE:
		return canCreate(order.getProjectType(), true);
	case ORDER_MAINTAIN:
		return canMaintain(order.getProcessType());
	case ORDER_LIST:
		return true;
	default:
		FErrorMsg("order.eOrderType failed to match a valid option");
		break;
	}
	return false;
}


int CvCity::getProductionExperience(UnitTypes eUnit) const
{
	PROFILE_EXTRA_FUNC();
	const CvPlayer& kPlayer = GET_PLAYER(getOwner());

	int iExperience = getFreeExperience() + kPlayer.getFreeExperience();

	if (eUnit != NO_UNIT)
	{
		const CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);

		if (kUnit.isSpy() && !GC.isSS_ENABLED())
		{
			return 0;
		}

		if (kUnit.canAcquireExperience())
		{
			iExperience += getSpecialistFreeExperience();

			if (kUnit.getUnitCombatType() != NO_UNITCOMBAT)
			{
				const UnitCombatTypes combatType = (UnitCombatTypes)kUnit.getUnitCombatType();
				iExperience += getUnitCombatFreeExperience(combatType);
				iExperience += kPlayer.getUnitCombatFreeExperience(combatType);
			}

			foreach_(const UnitCombatTypes eSubCombat, kUnit.getSubCombatTypes())
			{
				iExperience += getUnitCombatFreeExperience(eSubCombat);
				iExperience += kPlayer.getUnitCombatFreeExperience(eSubCombat);
			}

			iExperience += getDomainFreeExperience(kUnit.getDomainType());
		}
	}

	if (kPlayer.getStateReligion() != NO_RELIGION && isHasReligion(kPlayer.getStateReligion()))
	{
		iExperience += kPlayer.getStateReligionFreeExperience();
	}

	int iExperienceModifier = 0;
	if (kPlayer.getCapitalXPModifier() != 0 && (isCapital() || isGovernmentCenter()))
	{
		iExperienceModifier += kPlayer.getCapitalXPModifier();
	}

	if (kPlayer.getStateReligionHolyCityXPModifier() != 0 && isHolyCity(kPlayer.getStateReligion()))
	{
		iExperienceModifier += kPlayer.getStateReligionHolyCityXPModifier();
	}

	if (kPlayer.getNonStateReligionHolyCityXPModifier() != 0 && isHolyCity() && !isHolyCity(kPlayer.getStateReligion()))
	{
		iExperienceModifier += kPlayer.getNonStateReligionHolyCityXPModifier();
	}

	if (iExperienceModifier != 0)
	{
		iExperience *= 100 + iExperienceModifier;
		iExperience /= 100;
	}

	return std::max(0, iExperience);
}

void CvCity::addProductionExperience(CvUnit* pUnit, bool bConscript)
{
	PROFILE_FUNC();

	if (pUnit->canAcquirePromotionAny())
	{
		pUnit->changeExperience(getProductionExperience(pUnit->getUnitType()) / ((bConscript) ? 2 : 1));
	}

	foreach_(const BuildingTypes eType, getHasBuildings())
	{
		if (!isDisabledBuilding(eType))
		{
			assignPromotionsFromBuildingChecked(GC.getBuildingInfo(eType), pUnit);
		}
	}
}

UnitTypes CvCity::getProductionUnit() const
{
	bst::optional<OrderData> order = getHeadOrder();

	if (order)
	{
		switch (order->eOrderType)
		{
		case ORDER_TRAIN:
			return order->getUnitType();
		case ORDER_CONSTRUCT:
		case ORDER_CREATE:
		case ORDER_MAINTAIN:
		case ORDER_LIST:
			break;

		default:
			FErrorMsg("order->eOrderType failed to match a valid option");
			break;
		}
	}

	return NO_UNIT;
}


UnitAITypes CvCity::getProductionUnitAI() const
{
	bst::optional<OrderData> order = getHeadOrder();

	if (order != NULL)
	{
		switch (order->eOrderType)
		{
		case ORDER_TRAIN:
			return order->getUnitAIType();
		case ORDER_CONSTRUCT:
		case ORDER_CREATE:
		case ORDER_MAINTAIN:
		case ORDER_LIST:
			break;

		default:
			FErrorMsg("order->eOrderType failed to match a valid option");
			break;
		}
	}

	return NO_UNITAI;
}


BuildingTypes CvCity::getProductionBuilding() const
{
	bst::optional<OrderData> order = getHeadOrder();

	if (order != NULL)
	{
		switch (order->eOrderType)
		{
		case ORDER_TRAIN:
			break;
		case ORDER_CONSTRUCT:
			return order->getBuildingType();
		case ORDER_CREATE:
		case ORDER_MAINTAIN:
		case ORDER_LIST:
			break;

		default:
			FErrorMsg("order->eOrderType failed to match a valid option");
			break;
		}
	}

	return NO_BUILDING;
}


ProjectTypes CvCity::getProductionProject() const
{
	bst::optional<OrderData> order = getHeadOrder();

	if (order)
	{
		switch (order->eOrderType)
		{
		case ORDER_TRAIN:
		case ORDER_CONSTRUCT:
			break;
		case ORDER_CREATE:
			return order->getProjectType();
		case ORDER_MAINTAIN:
		case ORDER_LIST:
			break;

		default:
			FErrorMsg("order->eOrderType failed to match a valid option");
			break;
		}
	}

	return NO_PROJECT;
}


ProcessTypes CvCity::getProductionProcess() const
{
	bst::optional<OrderData> order = getHeadOrder();

	if (order)
	{
		switch (order->eOrderType)
		{
		case ORDER_TRAIN:
		case ORDER_CONSTRUCT:
		case ORDER_CREATE:
		case ORDER_LIST:
			break;
		case ORDER_MAINTAIN:
			return order->getProcessType();

		default:
			FErrorMsg("order->eOrderType failed to match a valid option");
			break;
		}
	}

	return NO_PROCESS;
}


const wchar_t* CvCity::getProductionName() const
{
	bst::optional<OrderData> order = getHeadOrder();

	if (order)
	{
		switch (order->eOrderType)
		{
		case ORDER_TRAIN:
			return GC.getUnitInfo(order->getUnitType()).getDescription(getCivilizationType());
		case ORDER_CONSTRUCT:
			return GC.getBuildingInfo(order->getBuildingType()).getDescription();
		case ORDER_CREATE:
			return GC.getProjectInfo(order->getProjectType()).getDescription();
		case ORDER_MAINTAIN:
			return GC.getProcessInfo(order->getProcessType()).getDescription();
		case ORDER_LIST:
			break; // It is never at the head of the list

		default:
			FErrorMsg("order->eOrderType failed to match a valid option");
			break;
		}
	}

	return L"";
}


int CvCity::getGeneralProductionTurnsLeft() const
{
	bst::optional<OrderData> order = getHeadOrder();

	if (order)
	{
		switch (order->eOrderType)
		{
		case ORDER_TRAIN:
			return getProductionTurnsLeft(order->getUnitType(), 0);
		case ORDER_CONSTRUCT:
			return getProductionTurnsLeft(order->getBuildingType(), 0);
		case ORDER_CREATE:
			return getProductionTurnsLeft(order->getProjectType(), 0);
		case ORDER_MAINTAIN:
		case ORDER_LIST:
			return 0;

		default:
			FErrorMsg("order->eOrderType failed to match a valid option");
			break;
		}
	}

	return 0;
}


const wchar_t* CvCity::getProductionNameKey() const
{
	bst::optional<OrderData> order = getHeadOrder();

	if (order)
	{
		switch (order->eOrderType)
		{
		case ORDER_TRAIN:
			return GC.getUnitInfo(order->getUnitType()).getTextKeyWide();
		case ORDER_CONSTRUCT:
			return GC.getBuildingInfo(order->getBuildingType()).getTextKeyWide();
		case ORDER_CREATE:
			return GC.getProjectInfo(order->getProjectType()).getTextKeyWide();
		case ORDER_MAINTAIN:
			return GC.getProcessInfo(order->getProcessType()).getTextKeyWide();
		case ORDER_LIST:
			break; // It is never at the head of the list

		default:
			FErrorMsg("pOrderNode->m_data.eOrderType failed to match a valid option");
			break;
		}
	}

	return L"";
}

bool CvCity::isFoodProduction(const OrderData& order) const
{
	return order.getOrderType() == ORDER_TRAIN && isFoodProduction(order.getUnitType());
}

bool CvCity::isFoodProduction() const
{
	bst::optional<OrderData> order = getHeadOrder();

	return order && isFoodProduction(*order);
}


bool CvCity::isFoodProduction(UnitTypes eUnit) const
{
	return GC.getUnitInfo(eUnit).isFoodProduction()
		|| GET_PLAYER(getOwner()).isMilitaryFoodProduction()
		&& GC.getUnitInfo(eUnit).isMilitaryProduction();
}

namespace {
	bool matchUnitOrder(const OrderData& order, const UnitTypes unitType)
	{
		return order.eOrderType == ORDER_TRAIN && order.getUnitType() == unitType;
	}
	bool matchBuildingOrder(const OrderData& order, const BuildingTypes buildingType)
	{
		return order.eOrderType == ORDER_CONSTRUCT && order.getBuildingType() == buildingType;
	}
	bool matchProjectOrder(const OrderData& order, const ProjectTypes projectType)
	{
		return order.eOrderType == ORDER_CREATE && order.getProjectType() == projectType;
	}
	bool matchUnitAITypeOrder(const OrderData& order, const UnitAITypes unitAIType)
	{
		return order.eOrderType == ORDER_TRAIN && order.getUnitAIType() == unitAIType;
	}
};

int CvCity::getFirstUnitOrder(UnitTypes eUnit) const
{
	OrderQueue::const_iterator order = bst::find_if(m_orderQueue, bind(matchUnitOrder, _1, eUnit));
	if (order == m_orderQueue.end())
	{
		return -1;
	}
	return std::distance(m_orderQueue.begin(), order);
}

int CvCity::getFirstBuildingOrder(BuildingTypes eBuilding) const
{
	OrderQueue::const_iterator order = bst::find_if(m_orderQueue, bind(matchBuildingOrder, _1, eBuilding));
	if (order == m_orderQueue.end())
	{
		return -1;
	}
	return std::distance(m_orderQueue.begin(), order);
}

int CvCity::getFirstProjectOrder(ProjectTypes eProject) const
{
	OrderQueue::const_iterator order = bst::find_if(m_orderQueue, bind(matchProjectOrder, _1, eProject));
	if (order == m_orderQueue.end())
	{
		return -1;
	}
	return std::distance(m_orderQueue.begin(), order);
}

int CvCity::getNumTrainUnitAI(UnitAITypes eUnitAI) const
{
	return algo::count_if(m_orderQueue, bind(matchUnitAITypeOrder, _1, eUnitAI));
}


int CvCity::getProductionProgress() const
{
	bst::optional<OrderData> order = getHeadOrder();

	if (order)
	{
		switch (order->eOrderType)
		{
			case ORDER_TRAIN:
			{
				return getProgressOnUnit(order->getUnitType());
			}
			case ORDER_CONSTRUCT:
			{
				return getProgressOnBuilding(order->getBuildingType());
			}
			case ORDER_CREATE:
			{
				return getProjectProduction(order->getProjectType());
			}
			case ORDER_MAINTAIN:
			case ORDER_LIST:
			{
				break;
			}
			default: FErrorMsg("order->eOrderType failed to match a valid option");
		}
	}
	return 0;
}


int CvCity::getProductionNeeded() const
{
	bst::optional<OrderData> order = getHeadOrder();
	return order ? getProductionNeeded(*order) : MAX_INT;
}

int CvCity::getProductionNeeded(const OrderData& order) const
{
	switch (order.eOrderType)
	{
	case ORDER_TRAIN:
		return getProductionNeeded(order.getUnitType());
	case ORDER_CONSTRUCT:
		return getProductionNeeded(order.getBuildingType());
	case ORDER_CREATE:
		return getProductionNeeded(order.getProjectType());
	case ORDER_MAINTAIN:
	case ORDER_LIST:
		break;

	default:
		FErrorMsg("OrderType failed to match a valid option");
		break;
	}

	return MAX_INT;
}

int CvCity::getProductionNeeded(UnitTypes eUnit) const
{
	return std::max(1, getModifiedIntValue(GET_PLAYER(getOwner()).getProductionNeeded(eUnit), -getProductionModifier(eUnit)));
}

int CvCity::getProductionNeeded(BuildingTypes eBuilding) const
{
	return std::max(1, getModifiedIntValue(GET_PLAYER(getOwner()).getProductionNeeded(eBuilding), -getProductionModifier(eBuilding)));
}

int CvCity::getProductionNeeded(ProjectTypes eProject) const
{
	return std::max(1, getModifiedIntValue(GET_PLAYER(getOwner()).getProductionNeeded(eProject), -getProductionModifier(eProject)));
}

int CvCity::getProductionTurnsLeft() const
{
	bst::optional<OrderData> order = getHeadOrder();
	return order ? getOrderProductionTurnsLeft(*order) : 0;
}

int CvCity::getOrderProductionTurnsLeft(const OrderData& order, int iIndex) const
{
	switch (order.eOrderType)
	{
	case ORDER_TRAIN:
		return getProductionTurnsLeft(order.getUnitType(), iIndex);
	case ORDER_CONSTRUCT:
		return getProductionTurnsLeft(order.getBuildingType(), iIndex);
	case ORDER_CREATE:
		return getProductionTurnsLeft(order.getProjectType(), iIndex);
	case ORDER_MAINTAIN:
		break;
	case ORDER_LIST:
		return 0;
	default:
		FErrorMsg("order.eOrderType failed to match a valid option");
		break;
	}
	return MAX_INT;
}


int CvCity::getTotalProductionQueueTurnsLeft() const
{
	PROFILE_EXTRA_FUNC();
	if (m_orderQueue.empty())
	{
		return 0;
	}

	int currProd = getCurrentProductionDifference(ProductionCalc::FoodProduction | ProductionCalc::Overflow);
	if (currProd == 0)
	{
		return MAX_INT;
	}

	int turns = 1;
	foreach_ (const OrderData & order, m_orderQueue)
	{
		int productionNeeded = getProductionNeeded(order);
		if (productionNeeded > 999)
		{
			return 999;
		}

		while (productionNeeded > 0)
		{
			if (currProd > productionNeeded)
			{
				// Can build this turn
				currProd -= productionNeeded;
				productionNeeded = 0;
			}
			else
			{
				// Next turn
				productionNeeded -= currProd;
				currProd = getProductionDifference(order, ProductionCalc::FoodProduction);
				if (currProd <= 0)
				{
					return MAX_INT;
				}
				++turns;
			}
		}
	}

	return turns;
}

namespace {
	bool matchUnitAtPlot(const OrderData& order, const UnitAITypes contractedAIType, const CvPlot* pDestPlot)
	{
		return order.eOrderType == ORDER_TRAIN
			&& order.unit.contractedAIType == contractedAIType
			&& (pDestPlot == NULL || order.unit.plotIndex == GC.getMap().plotNum(pDestPlot->getX(), pDestPlot->getY()));
	}
};

int CvCity::numQueuedUnits(UnitAITypes contractedAIType, const CvPlot* contractedPlot) const
{
	return algo::count_if(m_orderQueue, bind(matchUnitAtPlot, _1, contractedAIType, contractedPlot));
}

int CvCity::getProductionTurnsLeft(UnitTypes eUnit, int orderIndex) const
{
	const int firstOrderIndex = getFirstUnitOrder(eUnit);
	// We can count production already put towards this if we are looking
	// at the first one enqueued, or it isn't enqueued at all (and therefore would
	// be the first one were it to be)
	const int alreadyDone = (firstOrderIndex == -1 || firstOrderIndex == orderIndex) ? getProgressOnUnit(eUnit) : 0;
	const ProductionCalc::flags foodProd = isFoodProduction(eUnit) ? ProductionCalc::FoodProduction : ProductionCalc::None;
	const int perTurnProduction = getProductionPerTurn(foodProd | ProductionCalc::Yield);
	// If we are looking at the first order then overflow would be applied
	const int nextTurnProduction = (orderIndex == 0) ? getProductionPerTurn(foodProd | ProductionCalc::Overflow | ProductionCalc::Yield) : perTurnProduction;
	return getProductionTurnsLeft(getProductionNeeded(eUnit), alreadyDone, nextTurnProduction, perTurnProduction);
}

int CvCity::getProductionTurnsLeft(BuildingTypes eBuilding, int orderIndex) const
{
	const int firstOrderIndex = getFirstBuildingOrder(eBuilding);
	// We can count production already put towards this if we are looking
	// at the first one enqueued, or it isn't enqueued at all (and therefore would
	// be the first one were it to be)
	const int alreadyDone = (firstOrderIndex == -1 || firstOrderIndex == orderIndex) ? getProgressOnBuilding(eBuilding) : 0;
	const int perTurnProduction = getProductionPerTurn(ProductionCalc::Yield);
	// If we are looking at the first order then overflow would be applied
	const int nextTurnProduction = (orderIndex == 0) ? getProductionPerTurn(ProductionCalc::Overflow | ProductionCalc::Yield) : perTurnProduction;
	return getProductionTurnsLeft(getProductionNeeded(eBuilding), alreadyDone, nextTurnProduction, perTurnProduction);
}


int CvCity::getProductionTurnsLeft(ProjectTypes eProject, int orderIndex) const
{
	const int firstOrderIndex = getFirstProjectOrder(eProject);
	// We can count production already put towards this if we are looking
	// at the first one enqueued, or it isn't enqueued at all (and therefore would
	// be the first one were it to be)
	const int alreadyDone = (firstOrderIndex == -1 || firstOrderIndex == orderIndex) ? getProjectProduction(eProject) : 0;
	const int perTurnProduction = getProductionPerTurn(ProductionCalc::Yield);
	// If we are looking at the first order then overflow would be applied
	const int nextTurnProduction = (orderIndex == 0) ? getProductionPerTurn(ProductionCalc::Overflow | ProductionCalc::Yield) : perTurnProduction;
	return getProductionTurnsLeft(getProductionNeeded(eProject), alreadyDone, nextTurnProduction, perTurnProduction);
}

int CvCity::getProductionTurnsLeft(int totalRequiredProduction, int currentProduction, int nextTurnProduction, int perTurnProduction) const
{
	int remainingProduction = std::max(0, totalRequiredProduction - currentProduction - nextTurnProduction);
	// This doesn't look right...
	if (perTurnProduction == 0)
	{
		return remainingProduction + 1;
	}

	int turnsLeft = remainingProduction / perTurnProduction;
	if (turnsLeft * perTurnProduction < remainingProduction)
	{
		turnsLeft++;
	}

	turnsLeft++;

	return std::max(1, turnsLeft);
}


void CvCity::setProductionProgress(int iNewValue)
{
	if (isProductionUnit())
	{
		setProgressOnUnit(getProductionUnit(), iNewValue);
	}
	else if (isProductionBuilding())
	{
		setProgressOnBuilding(getProductionBuilding(), iNewValue);
	}
	else if (isProductionProject())
	{
		setProjectProduction(getProductionProject(), std::max(0, iNewValue));
	}
}


void CvCity::changeProduction(int iChange)
{
	if (isProductionUnit())
	{
		changeProgressOnUnit(getProductionUnit(), iChange);
	}
	else if (isProductionBuilding())
	{
		changeProgressOnBuilding(getProductionBuilding(), iChange);
	}
	else if (isProductionProject())
	{
		changeProjectProduction(getProductionProject(), iChange);
	}
	else if (isProductionProcess())
	{
		const CvProcessInfo& kProcess = GC.getProcessInfo(getProductionProcess());

		//	Add gold and espionage directly to player totals
		GET_PLAYER(getOwner()).changeGold((kProcess.getProductionToCommerceModifier(COMMERCE_GOLD) * iChange) / 100);
		GET_PLAYER(getOwner()).doEspionageOneOffPoints((kProcess.getProductionToCommerceModifier(COMMERCE_ESPIONAGE) * iChange) / 100);

		//	Research accrues to the team
		const TechTypes eCurrentTech = GET_PLAYER(getOwner()).getCurrentResearch();
		if (eCurrentTech != NO_TECH)
		{
			GET_TEAM(getTeam()).changeResearchProgress(eCurrentTech,
				(kProcess.getProductionToCommerceModifier(COMMERCE_RESEARCH) * iChange) / 100,
				getOwner());
		}

		//	Culture to the city itself
		changeCulture(getOwner(), (kProcess.getProductionToCommerceModifier(COMMERCE_CULTURE) * iChange) / 100, false, false);
	}
}

int CvCity::getProductionModifier(const OrderData& order) const
{
	switch (order.getOrderType())
	{
	case ORDER_TRAIN:
		return getProductionModifier(order.getUnitType());
	case ORDER_CONSTRUCT:
		return getProductionModifier(order.getBuildingType());
	case ORDER_CREATE:
		return getProductionModifier(order.getProjectType());
	case ORDER_MAINTAIN:
	case ORDER_LIST:
		break;

	default:
		FErrorMsg("OrderType failed to match a valid option");
		break;
	}

	return 0;
}

int CvCity::getProductionModifier() const
{
	bst::optional<OrderData> order = getHeadOrder();
	return order ? getProductionModifier(*order) : 0;
}

int CvCity::getProductionModifier(UnitTypes eUnit) const
{
	PROFILE_EXTRA_FUNC();
	const CvUnitInfo& unit = GC.getUnitInfo(eUnit);
	int iMultiplier = GET_PLAYER(getOwner()).getProductionModifier(eUnit);

	iMultiplier += getUnitProductionModifier(eUnit);
	iMultiplier += GET_PLAYER(getOwner()).getUnitProductionModifier(eUnit);
	if (!unit.isNoNonTypeProdMods())
	{
		iMultiplier += getDomainProductionModifier(unit.getDomainType());
		if (unit.getUnitCombatType() != NO_UNITCOMBAT)
		{
			iMultiplier += GET_PLAYER(getOwner()).getUnitCombatProductionModifier((UnitCombatTypes)(unit.getUnitCombatType()));
			iMultiplier += getUnitCombatProductionModifier((UnitCombatTypes)(unit.getUnitCombatType()));

			foreach_(const UnitCombatTypes eSubCombat, unit.getSubCombatTypes())
			{
				iMultiplier += GET_PLAYER(getOwner()).getUnitCombatProductionModifier(eSubCombat);
				iMultiplier += getUnitCombatProductionModifier(eSubCombat);
			}
		}

		if (unit.isMilitaryProduction())
		{
			iMultiplier += getMilitaryProductionModifier();
		}

		if (GET_PLAYER(getOwner()).getStateReligion() != NO_RELIGION && isHasReligion(GET_PLAYER(getOwner()).getStateReligion()))
		{
			iMultiplier += GET_PLAYER(getOwner()).getStateReligionUnitProductionModifier();
		}
	}
	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		if (hasBonus((BonusTypes)iI))
		{
			iMultiplier += unit.getBonusProductionModifier(iI);
		}
	}
	return iMultiplier;
}


int CvCity::getProductionModifier(BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	int iMultiplier = GET_PLAYER(getOwner()).getProductionModifier(eBuilding);

	iMultiplier += getBuildingProductionModifier(eBuilding);

	iMultiplier += GET_PLAYER(getOwner()).getBuildingProductionModifier(eBuilding);

	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		if (hasBonus((BonusTypes)iI))
		{
			iMultiplier += GC.getBuildingInfo(eBuilding).getBonusProductionModifier(iI);
		}
	}

	if (GET_PLAYER(getOwner()).getStateReligion() != NO_RELIGION)
	{
		if (isHasReligion(GET_PLAYER(getOwner()).getStateReligion()))
		{
			iMultiplier += GET_PLAYER(getOwner()).getStateReligionBuildingProductionModifier();
		}
	}

	return iMultiplier;
}


int CvCity::getProductionModifier(ProjectTypes eProject) const
{
	PROFILE_EXTRA_FUNC();
	int iMultiplier = GET_PLAYER(getOwner()).getProductionModifier(eProject);

	if (GC.getProjectInfo(eProject).isSpaceship())
	{
		iMultiplier += getSpaceProductionModifier();
	}

	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		if (hasBonus((BonusTypes)iI))
		{
			iMultiplier += GC.getProjectInfo(eProject).getBonusProductionModifier(iI);
		}
	}
	return iMultiplier;
}


int CvCity::getProductionPerTurn(ProductionCalc::flags flags = ProductionCalc::Yield) const
{
	if (isDisorder())
	{
		return 0;
	}
	const int iFoodProduction = (flags & ProductionCalc::FoodProduction) ? std::max(0, getYieldRate(YIELD_FOOD) - foodConsumption(true)) : 0;
	const int iOverflow = (flags & ProductionCalc::Overflow) ? getOverflowProduction() + getFeatureProduction() : 0;
	const int iYield = (flags & ProductionCalc::Yield) ? getBaseYieldRate(YIELD_PRODUCTION) : 0;

	return std::max(1, getExtraYield(YIELD_PRODUCTION) + iOverflow + iFoodProduction + iYield * getBaseYieldRateModifier(YIELD_PRODUCTION) / 100);
}

int CvCity::getProductionDifference(const OrderData& orderData, ProductionCalc::flags flags) const
{
	const ProductionCalc::flags foodFlag = ((flags & ProductionCalc::FoodProduction) && isFoodProduction(orderData)) ? ProductionCalc::FoodProduction : ProductionCalc::None;
	const ProductionCalc::flags overflowProd = (flags & ProductionCalc::Overflow) ? ProductionCalc::Overflow : ProductionCalc::None;

	return range(getProductionPerTurn(foodFlag | overflowProd | ProductionCalc::Yield), 0, MAX_INT);
}

int CvCity::getCurrentProductionDifference(ProductionCalc::flags flags) const
{
	bst::optional<OrderData> order = getHeadOrder();
	return order ? getProductionDifference(*order, flags) : 0;
}


bool CvCity::canHurryInternal(const HurryTypes eHurry) const
{
	if (!GET_PLAYER(getOwner()).canHurry(eHurry))
	{
		return false;
	}

	if (isDisorder())
	{
		return false;
	}

	if (getPopulation() <= hurryPopulation(eHurry))
	{
		return false;
	}
	return true;
}

bool CvCity::canHurry(const HurryTypes eHurry, const bool bTestVisible) const
{
	if (!canHurryInternal(eHurry))
	{
		return false;
	}

	if (getProductionProgress() >= getProductionNeeded())
	{
		return false;
	}

	if (!bTestVisible)
	{
		if (!isProductionUnit() && !isProductionBuilding())
		{
			return false;
		}

		if (GET_PLAYER(getOwner()).getGold() < getHurryGold(eHurry))
		{
			return false;
		}

		if (maxHurryPopulation() < hurryPopulation(eHurry))
		{
			return false;
		}
	}
	return true;
}

bool CvCity::canHurryUnit(HurryTypes eHurry, UnitTypes eUnit) const
{
	if (!canHurryInternal(eHurry))
	{
		return false;
	}

	if (getProgressOnUnit(eUnit) >= getProductionNeeded(eUnit))
	{
		return false;
	}

	if (GET_PLAYER(getOwner()).getGold() < getHurryGold(eHurry, getHurryCost(eUnit)))
	{
		return false;
	}

	if (maxHurryPopulation() < getHurryPopulation(eHurry, getHurryCost(eUnit)))
	{
		return false;
	}
	return true;
}

bool CvCity::canHurryBuilding(HurryTypes eHurry, BuildingTypes eBuilding) const
{
	if (!canHurryInternal(eHurry))
	{
		return false;
	}

	if (getProgressOnBuilding(eBuilding) >= getProductionNeeded(eBuilding))
	{
		return false;
	}

	if (GET_PLAYER(getOwner()).getGold() < getHurryGold(eHurry, getHurryCost(eBuilding)))
	{
		return false;
	}

	if (maxHurryPopulation() < getHurryPopulation(eHurry, getHurryCost(eBuilding)))
	{
		return false;
	}
	return true;
}


void CvCity::hurry(HurryTypes eHurry)
{
	int64_t iHurryGold = 0;
	int iHurryPopulation = 0;
	int iHurryAngerLength = 0;
	if (!canHurry(eHurry))
	{
		return;
	}

	bool bWhip = (GC.getHurryInfo(eHurry).getProductionPerPopulation() > 0);
	bool bBuy = (GC.getHurryInfo(eHurry).getGoldPerProduction() > 0);

	if (bBuy)
	{
		iHurryGold = getHurryGold(eHurry);
		GET_PLAYER(getOwner()).changeHurriedCount(1);
		GET_PLAYER(getOwner()).changeGold(-iHurryGold);
	}

	if (bWhip)
	{
		iHurryPopulation = hurryPopulation(eHurry);
		iHurryAngerLength = hurryAngerLength(eHurry);
		changePopulation(-(iHurryPopulation));
		changeHurryAngerTimer(iHurryAngerLength);
	}

	changeProduction(hurryProduction(eHurry));

	if (gCityLogLevel >= 2)
	{
		CvWStringBuffer szBuffer;
		CvWString szString;
		if (isProductionUnit())
		{
			szString = GC.getUnitInfo(getProductionUnit()).getDescription(getCivilizationType());
		}
		else if (isProductionBuilding())
		{
			szString = GC.getBuildingInfo(getProductionBuilding()).getDescription();
		}
		else if (isProductionProject())
		{
			szString = GC.getProjectInfo(getProductionProject()).getDescription();
		}

		logBBAI("    City %S hurrying production of %S at cost of %d pop, %I64d gold, %d anger length", getName().GetCString(), szString.GetCString(), iHurryPopulation, iHurryGold, iHurryAngerLength);
	}

	if (isCitySelected())
	{
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	}

	// Python Event
	CvEventReporter::getInstance().cityHurry(this, eHurry);
}

// BUG - Hurry Assist - start
bool CvCity::hurryOverflow(HurryTypes eHurry, int* iProduction, int* iGold, bool bCountThisTurn) const
{
	if (!canHurry(eHurry))
	{
		return false;
	}

	if (GC.getHurryInfo(eHurry).getProductionPerPopulation() == 0)
	{
		*iProduction = 0;
		*iGold = 0;
		return true;
	}
	int iTotal, iCurrent, iGoldPercent;

	if (isProductionUnit())
	{
		const UnitTypes eUnit = getProductionUnit();
		FAssertMsg(eUnit != NO_UNIT, "eUnit is expected to be assigned a valid unit type");
		iTotal = getProductionNeeded(eUnit);
		iCurrent = getProgressOnUnit(eUnit);
		iGoldPercent = GC.getMAXED_UNIT_GOLD_PERCENT();
	}
	else if (isProductionBuilding())
	{
		const BuildingTypes eBuilding = getProductionBuilding();
		FAssertMsg(eBuilding != NO_BUILDING, "eBuilding is expected to be assigned a valid building type");
		iTotal = getProductionNeeded(eBuilding);
		iCurrent = getProgressOnBuilding(eBuilding);
		iGoldPercent = GC.getMAXED_BUILDING_GOLD_PERCENT();
	}
	else if (isProductionProject())
	{
		const ProjectTypes eProject = getProductionProject();
		FAssertMsg(eProject != NO_PROJECT, "eProject is expected to be assigned a valid project type");
		iTotal = getProductionNeeded(eProject);
		iCurrent = getProjectProduction(eProject);
		iGoldPercent = GC.getMAXED_PROJECT_GOLD_PERCENT();
	}
	else return false;

	int iOverflow = iCurrent + hurryProduction(eHurry) - iTotal;
	if (bCountThisTurn)
	{
		// include chops and previous overflow here
		iOverflow += getCurrentProductionDifference(ProductionCalc::FoodProduction | ProductionCalc::Overflow);
	}
	const int iMaxOverflow = getMaxProductionOverflow();

	*iProduction = std::min(iOverflow, iMaxOverflow);
	*iGold = std::max(0, iOverflow - iMaxOverflow) * iGoldPercent / 100;

	return true;
}
// BUG - Hurry Assist - end


UnitTypes CvCity::getConscriptUnit() const
{
	PROFILE_EXTRA_FUNC();
	UnitTypes eBestUnit = NO_UNIT;

	int iBestValue = 0;
	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		if (canTrain((UnitTypes) iI))
		{
			int iValue = GC.getUnitInfo((UnitTypes) iI).getConscriptionValue();
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestUnit = (UnitTypes) iI;
			}
		}
	}
	return eBestUnit;
}


int CvCity::getConscriptPopulation() const
{
	return std::max(0, GC.getCONSCRIPT_POPULATION());
}


int CvCity::flatConscriptAngerLength() const
{
	return std::max(1, GC.getCONSCRIPT_ANGER_DIVISOR() * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 100);
}


bool CvCity::canConscript(bool bOnCapture) const
{
	if (!bOnCapture)
	{
		if (isDisorder() || isDrafted())
		{
			return false;
		}
	}

	if (getPopulation() <= getConscriptPopulation())
	{
		return false;
	}

	if (GET_PLAYER(getOwner()).getConscriptCount() >= GET_PLAYER(getOwner()).getMaxConscript())
	{
		return false;
	}

	if (!bOnCapture && plot()->calculateTeamCulturePercent(getTeam()) < GC.getCONSCRIPT_MIN_CULTURE_PERCENT())
	{
		return false;
	}

	if (getConscriptUnit() == NO_UNIT)
	{
		return false;
	}

	return true;
}

CvUnit* CvCity::initConscriptedUnit()
{
	const UnitTypes eConscriptUnit = getConscriptUnit();
	if (NO_UNIT == eConscriptUnit)
	{
		return NULL;
	}

	UnitAITypes eCityAI = NO_UNITAI;
	if (GET_PLAYER(getOwner()).AI_unitValue(eConscriptUnit, UNITAI_ATTACK, area()) > 0)
	{
		eCityAI = UNITAI_ATTACK;
	}
	else if (GET_PLAYER(getOwner()).AI_unitValue(eConscriptUnit, UNITAI_CITY_DEFENSE, area()) > 0)
	{
		eCityAI = UNITAI_CITY_DEFENSE;
	}
	else if (GET_PLAYER(getOwner()).AI_unitValue(eConscriptUnit, UNITAI_CITY_COUNTER, area()) > 0)
	{
		eCityAI = UNITAI_CITY_COUNTER;
	}
	else if (GET_PLAYER(getOwner()).AI_unitValue(eConscriptUnit, UNITAI_CITY_SPECIAL, area()) > 0)
	{
		eCityAI = UNITAI_CITY_SPECIAL;
	}
	else
	{
		eCityAI = NO_UNITAI;
	}

	CvUnit* pUnit = GET_PLAYER(getOwner()).initUnit(eConscriptUnit, getX(), getY(), eCityAI, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
	FAssertMsg(pUnit != NULL, "pUnit expected to be assigned (not NULL)");

	if (NULL != pUnit)
	{
		addProductionExperience(pUnit, true);

		pUnit->setMoves(0);
	}

	return pUnit;
}


void CvCity::conscript(bool bOnCapture)
{
	PROFILE_EXTRA_FUNC();
	if (!canConscript(bOnCapture))
	{
		return;
	}
	const int iNumConscripts = -getConscriptPopulation();
	const int iAngerLength = flatConscriptAngerLength();
	changePopulation(-1);
	changeConscriptAngerTimer(iAngerLength);

	setDrafted(true);

	if (!bOnCapture)
	{
		GET_PLAYER(getOwner()).changeConscriptCount(1);
	}

	for (int i = 0; i < iNumConscripts; i++)
	{
		CvUnit* pUnit = initConscriptedUnit();
		FAssertMsg(pUnit != NULL, "pUnit expected to be assigned (not NULL)");

		if (NULL != pUnit && gCityLogLevel >= 2)
		{
			logBBAI("      City %S does conscript of %d %S at cost of %d anger", getName().GetCString(), iNumConscripts, pUnit->getName().GetCString(), iAngerLength);
		}
	}
}


int CvCity::getBonusHealth(BonusTypes eBonus) const
{
	PROFILE_EXTRA_FUNC();
	int iHealth = GC.getBonusInfo(eBonus).getHealth();

	foreach_(const BuildingTypes eTypeX, getHasBuildings())
	{
		if (!isReligiouslyLimitedBuilding(eTypeX) && !isDisabledBuilding(eTypeX))
		{
			iHealth += GC.getBuildingInfo(eTypeX).getBonusHealthChanges().getValue(eBonus);
		}
	}
	return iHealth;
}


int CvCity::getBonusHappiness(BonusTypes eBonus) const
{
	PROFILE_EXTRA_FUNC();
	int iHappiness = GC.getBonusInfo(eBonus).getHappiness();

	foreach_(const BuildingTypes eTypeX, getHasBuildings())
	{
		if (!isReligiouslyLimitedBuilding(eTypeX) && !isDisabledBuilding(eTypeX))
		{
			iHappiness += GC.getBuildingInfo(eTypeX).getBonusHappinessChanges().getValue(eBonus);
		}
	}
	if (hasBonus(eBonus))
	{
		for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
			if (GET_PLAYER(getOwner()).hasTrait((TraitTypes)iI))
			{
				foreach_(const BonusModifier& pair, GC.getTraitInfo((TraitTypes)iI).getBonusHappinessChanges())
				{
					if (pair.first == eBonus)
					{
						iHappiness += pair.second;
					}
				}
			}
		}
	}
	return iHappiness;
}


int CvCity::getBonusPower(BonusTypes eBonus) const
{
	PROFILE_EXTRA_FUNC();

	int iCount = 0;
	foreach_(const BuildingTypes eTypeX, getHasBuildings())
	{
		if (!isReligiouslyLimitedBuilding(eTypeX)
		&& GC.getBuildingInfo(eTypeX).getPowerBonus() == eBonus
		&& !isDisabledBuilding(eTypeX))
		{
			iCount++;
		}
	}
	return iCount;
}


int CvCity::getBonusYieldRateModifier(YieldTypes eIndex, BonusTypes eBonus) const
{
	PROFILE_EXTRA_FUNC();

	int iModifier = 0;
	foreach_(const BuildingTypes eTypeX, getHasBuildings())
	{
		if (!isReligiouslyLimitedBuilding(eTypeX) && !isDisabledBuilding(eTypeX))
		{
			iModifier += GC.getBuildingInfo(eTypeX).getBonusYieldModifier(eBonus, eIndex);
		}
	}
	return iModifier;
}

void CvCity::processBonus(BonusTypes eBonus, int iChange)
{
	PROFILE_FUNC();
	{
		const int iBaseValue = GC.getBonusInfo(eBonus).getHealth();
		int iGoodValue = std::max(0, iBaseValue);
		int iBadValue = std::min(0, iBaseValue);

		foreach_(const BuildingTypes eTypeX, getHasBuildings())
		{
			if (!isReligiouslyLimitedBuilding(eTypeX) && !isDisabledBuilding(eTypeX))
			{
				const int iValue = GC.getBuildingInfo(eTypeX).getBonusHealthChanges().getValue(eBonus);

				if (iValue >= 0)
				{
					iGoodValue += iValue;
				}
				else iBadValue += iValue;
			}
		}

		changeBonusGoodHealth(iGoodValue * iChange);
		changeBonusBadHealth(iBadValue * iChange);
	}

	{
		const int iBaseValue = GC.getBonusInfo(eBonus).getHappiness();
		int iGoodValue = std::max(0, iBaseValue);
		int iBadValue = std::min(0, iBaseValue);

		foreach_(const BuildingTypes eTypeX, getHasBuildings())
		{
			if (!isReligiouslyLimitedBuilding(eTypeX) && !isDisabledBuilding(eTypeX))
			{
				const int iValue = GC.getBuildingInfo(eTypeX).getBonusHappinessChanges().getValue(eBonus);

				if (iValue >= 0)
				{
					iGoodValue += iValue;
				}
				else iBadValue += iValue;
			}
		}
		for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
			const TraitTypes eTrait = (TraitTypes)iI;
			if (GET_PLAYER(getOwner()).hasTrait(eTrait))
			{
				int iValue = 0;
				foreach_(const BonusModifier& pair, GC.getTraitInfo((TraitTypes)iI).getBonusHappinessChanges())
				{
					if (pair.first == eBonus)
					{
						iValue = pair.second;
						break;
					}
				}
				if (iValue >= 0)
				{
					iGoodValue += iValue;
				}
				else
				{
					iBadValue += iValue;
				}
			}
		}
		changeBonusGoodHappiness(iGoodValue * iChange);
		changeBonusBadHappiness(iBadValue * iChange);
	}

	changePowerCount(getBonusPower(eBonus) * iChange);

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		changeBonusYieldRateModifier(((YieldTypes)iI), (getBonusYieldRateModifier(((YieldTypes)iI), eBonus) * iChange));
	}

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		changeBonusCommerceRateModifier(((CommerceTypes)iI), (getBonusCommerceRateModifier(((CommerceTypes)iI), eBonus) * iChange));
		changeBonusCommercePercentChanges(((CommerceTypes)iI), (getBonusCommercePercentChanges(((CommerceTypes)iI), eBonus) * iChange));
	}
	foreach_(const BuildingTypes eTypeX, getHasBuildings())
	{
		if (!isReligiouslyLimitedBuilding(eTypeX)
		&& GC.getBuildingInfo(eTypeX).getVicinityBonusYieldChanges(NO_BONUS, NO_YIELD) != 0
		&& !isDisabledBuilding(eTypeX))
		{
			for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				updateYieldRate(eTypeX, (YieldTypes)iJ, getBuildingYieldChange(eTypeX, (YieldTypes)iJ) + GC.getBuildingInfo(eTypeX).getBonusYieldChanges(eBonus, iJ) * iChange);
			}
		}
	}
}



// Toffer - AlphaOmega (beginning\end) is only true when this is called from setHasBuilding(...).
//	Added the extra input because I'm not sure if I can get away with changing the m_buildingLedger map 
//	after processsBuilding(..) is called from it rather than the current order which is before the call.
//	It is currently only used in the initial "sanity control" safety net.
void CvCity::processBuilding(const BuildingTypes eBuilding, const int iChange, const bool bReligiously, const bool bAlphaOmega)
{
	PROFILE_FUNC();
	FAssert(iChange == 1 || iChange == -1);

	// Toffer - Sanity control
	if (iChange == -1)
	{
		if (isDisabledBuilding(eBuilding) || !bAlphaOmega && !hasBuilding(eBuilding))
		{
			FErrorMsg("Trying to process out a building that haven't been processed in! Code copes, but it shouldn't have to!");
			return;
		}
		if (isReligiouslyLimitedBuilding(eBuilding))
		{
			if (bReligiously)
			{
				FErrorMsg("Trying to religiously process out a building that is already religiously processed out! Code copes, but it shouldn't have to!");
				return;
			}
			FErrorMsg("Trying to process out a building that is already religiously processed out! Code copes, but it shouldn't have to!");
			setReligiouslyLimitedBuilding(eBuilding, false);
		}
	}
	else if (bReligiously)
	{
		if (!isReligiouslyLimitedBuilding(eBuilding))
		{
			FErrorMsg("Trying to religiously process in a building that was never religiously processed out! Code copes, but it shouldn't have to!");
			return;
		}
		if (isDisabledBuilding(eBuilding))
		{
			FErrorMsg("Trying to religiously process in a building that is disabled! Code copes, but it shouldn't have to!");
			return;
		}
	}
	else if (!bAlphaOmega && isActiveBuilding(eBuilding))
	{
		FErrorMsg("Trying to process in a building that is already processed in! Code copes, but it shouldn't have to!");
		return;
	}
	CvPlayer& owner = GET_PLAYER(getOwner());

	// Process the building
	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	if (!bReligiously && kBuilding.isOrbitalInfrastructure())
	{
		owner.noteOrbitalInfrastructureCountDirty();
	}

	{
		PROFILE("CvCity::processBuilding.properties");

		const CvProperties* pProp = kBuilding.getProperties();
		const CvProperties* pPropAllCities = kBuilding.getPropertiesAllCities();
		if (iChange > 0)
		{
			// Property manipulators must be serialized as the propagation can mean we're updating
			//	properties on many entities (granular locking produces too much overhead).
			if (!pProp->isEmpty())
			{
				getProperties()->addProperties(pProp);
			}
			if (!pPropAllCities->isEmpty())
			{
				GET_TEAM(getTeam()).addPropertiesAllCities(pPropAllCities);
			}
		}
		else
		{
			// Property manipulators must be serialized as the propagation can mean we're updating
			//	properties on many entities (granular locking produces too much overhead).
			if (!pProp->isEmpty())
			{
				getProperties()->subtractProperties(pProp);
			}
			if (!pPropAllCities->isEmpty())
			{
				GET_TEAM(getTeam()).subtractPropertiesAllCities(pPropAllCities);
			}
		}
	}

	{
		PROFILE("CvCity::processBuilding.part2");

		const bool bChange = (iChange == 1);
		foreach_(const TraitTypes eTrait, kBuilding.getFreeTraitTypes())
		{
			if (GC.getTraitInfo(eTrait).isCivilizationTrait())
			{
				owner.setHasTrait(eTrait, bChange);
			}
		}

		foreach_(const BonusModifier& pair, kBuilding.getFreeBonuses())
		{
			changeFreeBonus(pair.first, pair.second * iChange);
			clearVicinityBonusCache(pair.first);
			clearRawVicinityBonusCache(pair.first);
		}

		if (kBuilding.getPropertySpawnProperty() != NO_PROPERTY && kBuilding.getPropertySpawnUnit() != NO_UNIT)
		{
			FAssertMsg(GC.getUnitInfo(kBuilding.getPropertySpawnUnit()).isBlendIntoCity(),
				CvString::format("Building %s wants to add property spawner with unit class %s, but this unit doesn't have bBlendIntoCity enabled, which is a requirement",
					kBuilding.getType(), GC.getUnitInfo(kBuilding.getPropertySpawnUnit()).getType()).c_str());

			changePropertySpawn(iChange, kBuilding.getPropertySpawnProperty(), kBuilding.getPropertySpawnUnit());
		}

		changeEspionageDefenseModifier(kBuilding.getEspionageDefenseModifier() * iChange);

		changePopulationgrowthratepercentage(kBuilding.getPopulationgrowthratepercentage(), iChange == 1);

		if (!bReligiously)
		{
			changeGreatPeopleRateModifier(kBuilding.getGreatPeopleRateModifier() * iChange);
		}
		changeFreeExperience(kBuilding.getFreeExperience() * iChange);
		changeFoodKeptPercent(kBuilding.getFoodKept() * iChange);
		changeMaxAirlift(kBuilding.getAirlift() * iChange);
		changeAirModifier(kBuilding.getAirModifier() * iChange);
		changeAirUnitCapacity(kBuilding.getAirUnitCapacity() * iChange);
		changeNukeModifier(kBuilding.getNukeModifier() * iChange);
		changeFreeSpecialist(kBuilding.getFreeSpecialist() * iChange);
		changeMaintenanceModifier(kBuilding.getMaintenanceModifier() * iChange);
		changeWarWearinessModifier(kBuilding.getWarWearinessModifier() * iChange);
		changeHurryAngerModifier(kBuilding.getHurryAngerModifier() * iChange);
		changeHealRate(kBuilding.getHealRateChange() * iChange);
		changeQuarantinedCount(kBuilding.isQuarantine() ? iChange : 0);

		if (kBuilding.getHealth() > 0)
		{
			changeBuildingGoodHealth(kBuilding.getHealth() * iChange);
		}
		else changeBuildingBadHealth(kBuilding.getHealth() * iChange);


		if (kBuilding.getHappiness() > 0)
		{
			changeBuildingGoodHappiness(kBuilding.getHappiness() * iChange);
		}
		else changeBuildingBadHappiness(kBuilding.getHappiness() * iChange);


		if (kBuilding.getReligionType() != NO_RELIGION)
		{
			changeStateReligionHappiness((ReligionTypes)kBuilding.getReligionType(), kBuilding.getStateReligionHappiness() * iChange);
		}
		changeMilitaryProductionModifier(kBuilding.getMilitaryProductionModifier() * iChange);
		changeSpaceProductionModifier(kBuilding.getSpaceProductionModifier() * iChange);
		changeExtraTradeRoutes(kBuilding.getTradeRoutes() * iChange);
		changeTradeRouteModifier(kBuilding.getTradeRouteModifier() * iChange);
		changeForeignTradeRouteModifier(kBuilding.getForeignTradeRouteModifier() * iChange);
		changePowerCount(kBuilding.isPower() ? iChange : 0);
		changeGovernmentCenterCount(kBuilding.isGovernmentCenter() ? iChange : 0);
		changeNoUnhappinessCount(kBuilding.isNoUnhappiness() ? iChange : 0);
		changeNoUnhealthyPopulationCount(kBuilding.isNoUnhealthyPopulation() ? iChange : 0);
		changeBuildingOnlyHealthyCount(kBuilding.isBuildingOnlyHealthy() ? iChange : 0);
		changeRevIndexDistanceMod(kBuilding.getRevIdxDistanceModifier() * iChange);
	}
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		PROFILE("CvCity::processBuilding.Yields");
		const YieldTypes eYieldX = static_cast<YieldTypes>(iI);
		changeBuildingExtraYield100(eYieldX, 100 * iChange * (kBuilding.getYieldChange(iI) + getBuildingYieldChange(eBuilding, eYieldX)));
		changeRiverPlotYield(eYieldX, kBuilding.getRiverPlotYieldChange(iI) * iChange);
		changeBaseYieldPerPopRate(eYieldX, kBuilding.getYieldPerPopChange(iI) * iChange);

		changeBuildingYieldModifier(eYieldX, iChange * kBuilding.getYieldModifier(iI));

		changePowerYieldRateModifier(eYieldX, kBuilding.getPowerYieldModifier(iI) * iChange);
	}

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		PROFILE("CvCity::processBuilding.Commerces");
		const CommerceTypes eCommerceX = static_cast<CommerceTypes>(iI);

		changeCommercePerPopFromBuildings(eCommerceX, iChange * kBuilding.getCommercePerPopChange(iI));

		changeBuildingCommerceChange(eBuilding, eCommerceX, iChange * owner.getBuildingCommerceChange(eBuilding, eCommerceX));

		changeBuildingCommerceModifier(eCommerceX, iChange * (kBuilding.getCommerceModifier(iI) + owner.getBuildingCommerceModifier(eBuilding, eCommerceX)));

		changeCommerceHappinessPer(eCommerceX, kBuilding.getCommerceHappiness(iI) * iChange);
	}

	if (!bReligiously)
	{
		foreach_(const ReligionModifier& pair, kBuilding.getReligionChanges())
		{
			PROFILE("CvCity::processBuilding.Religions");
			changeReligionInfluence(pair.first, pair.second * iChange);
		}
	}

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		PROFILE("CvCity::processBuilding.Specialists");

		changeMaxSpecialistCount((SpecialistTypes)iI, (kBuilding.getSpecialistCount(iI) + GET_TEAM(getTeam()).getBuildingSpecialistChange(eBuilding, (SpecialistTypes)iI)) * iChange);
		changeFreeSpecialistCount((SpecialistTypes)iI, kBuilding.getFreeSpecialistCount(iI) * iChange);
	}

	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		if (kBuilding.isMayDamageAttackingUnitCombatType(iI))
		{
			changeDamageAttackingUnitCombatCount((UnitCombatTypes)iI, iChange);
		}
	}

	if (kBuilding.getNumHealUnitCombatTypes() > 0)
	{
		for (int iI = 0; iI < kBuilding.getNumHealUnitCombatTypes(); iI++)
		{
			changeHealUnitCombatTypeVolume((UnitCombatTypes)kBuilding.getHealUnitCombatType(iI).eUnitCombat, kBuilding.getHealUnitCombatType(iI).iHeal * iChange);
		}
	}


	{
		PROFILE("CvCity::processBuilding.Part3");

		foreach_(const UnitCombatModifier2& modifier, kBuilding.getUnitCombatExtraStrength())
		{
			changeUnitCombatExtraStrength(modifier.first, modifier.second * iChange);
		}

		if (!bReligiously)
		{
			changeLineOfSight(kBuilding.getLineOfSight() * iChange);
		}
		changeAdjacentDamagePercent(kBuilding.getAdjacentDamagePercent() * iChange);
		changeNumUnitFullHeal(kBuilding.getNumUnitFullHeal() * iChange);
		changeNumPopulationEmployed(kBuilding.getNumPopulationEmployed() * iChange);
		changeHealthPercentPerPopulation(kBuilding.getHealthPercentPerPopulation() * iChange);
		changeHappinessPercentPerPopulation(kBuilding.getHappinessPercentPerPopulation() * iChange);

#ifdef OUTBREAKS_AND_AFFLICTIONS
		//TB Combat Mods (Buildings) begin
		for (int iI = 0; iI < kBuilding.getNumAidRateChanges(); iI++)
		{
			PropertyTypes eProperty = kBuilding.getAidRateChange(iI).ePropertyType;
			changeAidRate(eProperty, kBuilding.getAidRateChange(iI).iChange * iChange);
		}
#endif // OUTBREAKS_AND_AFFLICTIONS

#ifdef STRENGTH_IN_NUMBERS
		changeTotalFrontSupportPercentModifier(kBuilding.getFrontSupportPercentModifier() * iChange);
		changeTotalShortRangeSupportPercentModifier(kBuilding.getShortRangeSupportPercentModifier() * iChange);
		changeTotalMediumRangeSupportPercentModifier(kBuilding.getMediumRangeSupportPercentModifier() * iChange);
		changeTotalLongRangeSupportPercentModifier(kBuilding.getLongRangeSupportPercentModifier() * iChange);
		changeTotalFlankSupportPercentModifier(kBuilding.getFlankSupportPercentModifier() * iChange);
#endif // STRENGTH_IN_NUMBERS
	}
#ifdef OUTBREAKS_AND_AFFLICTIONS
	if (kBuilding.getPromotionLineType() != NO_PROMOTIONLINE && GC.getPromotionLineInfo(kBuilding.getPromotionLineType()).isAffliction())
	{
		changeAfflictionTypeCount(kBuilding.getPromotionLineType(), iChange);
	}


	for (int iI = 0; iI < kBuilding.getNumBonusAidModifiers(); iI++)
	{
		BonusTypes eBonus = kBuilding.getBonusAidModifier(iI).eBonusType;
		PropertyTypes ePropertyType = kBuilding.getBonusAidModifier(iI).ePropertyType;

		changeExtraBonusAidModifier(eBonus, ePropertyType, kBuilding.getBonusAidModifier(iI).iModifier);
	}


	for (int iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
	{
		PROFILE("CvCity::processBuilding.PromotionLines");
		const PromotionLineTypes eAfflictionLine = static_cast<PromotionLineTypes>(iI);
		changeExtraAfflictionOutbreakLevelChange(eAfflictionLine, kBuilding.getAfflictionOutbreakLevelChange(iI) * iChange);

		if (iChange == -1 && kBuilding.getPromotionLineType() == eAfflictionLine)
		{
			changeAfflictionToleranceChange(eAfflictionLine, GC.getPromotionLineInfo(eAfflictionLine).getToleranceBuildup());
			if (!hasAfflictionType(eAfflictionLine))
			{
				setCurrentOvercomeChange(eAfflictionLine, 0);
			}
		}
	}
#endif // OUTBREAKS_AND_AFFLICTIONS

	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		PROFILE("CvCity::processBuilding.CombatInfos");

		const UnitCombatTypes eCombatX = static_cast<UnitCombatTypes>(iI);

		changeUnitCombatProductionModifier(eCombatX, kBuilding.getUnitCombatProdModifier(iI) * iChange);

		if (!bReligiously)
		{
			changeUnitCombatRepelModifierTotal(eCombatX, kBuilding.getUnitCombatRepelModifier(iI) * iChange);
			//TB Defense Mod
			changeUnitCombatRepelAgainstModifierTotal(eCombatX, kBuilding.getUnitCombatRepelAgainstModifier(iI) * iChange);
			changeUnitCombatDefenseAgainstModifierTotal(eCombatX, kBuilding.getUnitCombatDefenseAgainstModifier(iI) * iChange);
		}
	}
	//TB Combat Mods (Buildings) end

	//TB Building Tags
	changeExtraLocalCaptureProbabilityModifier(kBuilding.getLocalCaptureProbabilityModifier() * iChange);
	changeExtraLocalCaptureResistanceModifier(kBuilding.getLocalCaptureResistanceModifier() * iChange);
	changeExtraInsidiousness(kBuilding.getInsidiousness() * iChange);
	changeExtraInvestigation(kBuilding.getInvestigation() * iChange);

	//TB Defense Mod
	if (!bReligiously)
	{
		changeExtraLocalDynamicDefense(kBuilding.getLocalDynamicDefense() * iChange);
		changeExtraRiverDefensePenalty(kBuilding.getRiverDefensePenalty() * iChange);
		changeExtraLocalRepel(kBuilding.getLocalRepel() * iChange);
		changeExtraMinDefense(kBuilding.getMinDefense() * iChange);
		if (kBuilding.getBuildingDefenseRecoverySpeedModifier() > 0 && kBuilding.getDefenseModifier() > 0)
		{
			changeExtraBuildingDefenseRecoverySpeedModifier(kBuilding.getBuildingDefenseRecoverySpeedModifier() * iChange);
			changeModifiedBuildingDefenseRecoverySpeedCap(kBuilding.getDefenseModifier() * iChange);
		}
		changeExtraCityDefenseRecoverySpeedModifier(kBuilding.getCityDefenseRecoverySpeedModifier() * iChange);
	}

	foreach_(const TechModifier& modifier, kBuilding.getTechHappinessChanges())
	{
		changeBuildingHappinessFromTech(modifier.first, modifier.second * iChange);
	}

	foreach_(const TechModifier& modifier, kBuilding.getTechHealthChanges())
	{
		changeBuildingHealthFromTech(modifier.first, modifier.second * iChange);
	}

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		const SpecialistTypes eSpecialistX = static_cast<SpecialistTypes>(iI);
		for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			if (kBuilding.getLocalSpecialistYieldChange(eSpecialistX, (YieldTypes) iJ) != 0)
			{
				changeLocalSpecialistExtraYield(eSpecialistX, (YieldTypes)iJ, kBuilding.getLocalSpecialistYieldChange(eSpecialistX, (YieldTypes)iJ) * iChange);
			}
		}
		for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			if (kBuilding.getLocalSpecialistCommerceChange(eSpecialistX, (CommerceTypes) iJ) != 0)
			{
				changeLocalSpecialistExtraCommerce(eSpecialistX, (CommerceTypes)iJ, kBuilding.getLocalSpecialistCommerceChange((SpecialistTypes)iI, (CommerceTypes)iJ) * iChange);
			}
		}
	}

	{
		PROFILE("CvCity::processBuilding.Part4");

		int iMinBuildingDefenseLevel = kBuilding.getNoEntryDefenseLevel();

		if (!GC.getGame().isOption(GAMEOPTION_COMBAT_REALISTIC_SIEGE))
		{
			iMinBuildingDefenseLevel = 0;
		}

		if (iMinBuildingDefenseLevel > 0)
		{
			const int iCurrentMinDefenseLevel = getMinimumDefenseLevel();

			if (iChange > 0)
			{
				if (iMinBuildingDefenseLevel > iCurrentMinDefenseLevel)
				{
					setMinimumDefenseLevel(iMinBuildingDefenseLevel);
				}
			}
			else if (iMinBuildingDefenseLevel == iCurrentMinDefenseLevel)
			{
				int iNewMinDefenseLevel = 0;

				foreach_(const BuildingTypes eTypeX, getHasBuildings())
				{
					if (!isDisabledBuilding(eTypeX))
					{
						const int iLevel = GC.getBuildingInfo(eTypeX).getNoEntryDefenseLevel();

						if (iLevel > iNewMinDefenseLevel)
						{
							iNewMinDefenseLevel = iLevel;
						}
					}
				}
				setMinimumDefenseLevel(iNewMinDefenseLevel);
			}
		}

		if (kBuilding.isZoneOfControl())
		{
			changeZoCCount(iChange);
		}

		if (!bReligiously)
		{
			if (kBuilding.isProtectedCulture())
			{
				changeProtectedCultureCount(iChange > 0 ? 1 : -1);
			}
			if (kBuilding.getWorkableRadius() > 0)
			{
				setWorkableRadiusOverride(iChange > 0 ? kBuilding.getWorkableRadius() : 0);
			}
			if (kBuilding.isProvidesFreshWater())
			{
				changeFreshWater(iChange);
			}
		}
	}
	foreach_(const ImprovementModifier& pair, kBuilding.getImprovementFreeSpecialists())
	{
		changeImprovementFreeSpecialists(pair.first, pair.second * iChange);
	}

	foreach_(const BonusModifier& modifier, kBuilding.getBonusHealthChanges())
	{
		if (hasBonus(modifier.first))
		{
			if (modifier.second > 0)
			{
				changeBonusGoodHealth(modifier.second * iChange);
			}
			else
			{
				changeBonusBadHealth(modifier.second * iChange);
			}
		}
	}

	foreach_(const BonusModifier& modifier, kBuilding.getBonusHappinessChanges())
	{
		if (hasBonus(modifier.first))
		{
			if (modifier.second > 0)
			{
				changeBonusGoodHappiness(modifier.second * iChange);
			}
			else
			{
				changeBonusBadHappiness(modifier.second * iChange);
			}
		}
	}

	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		PROFILE("CvCity::processBuilding.Bonuses2");
		if (hasBonus((BonusTypes)iI))
		{
			if (kBuilding.getPowerBonus() == iI)
			{
				changePowerCount(iChange);
			}

			for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				changeBonusYieldRateModifier((YieldTypes)iJ, kBuilding.getBonusYieldModifier(iI, iJ) * iChange);

				int iBonusChange = kBuilding.getBonusYieldChanges(iI, iJ);
				if (hasVicinityBonus((BonusTypes)iI))
				{
					iBonusChange += kBuilding.getVicinityBonusYieldChanges(iI, iJ);
				}
				updateYieldRate(eBuilding, (YieldTypes)iJ, (getBuildingYieldChange(eBuilding, (YieldTypes)iJ) + iBonusChange) * iChange);
			}
			for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				changeBonusCommerceRateModifier((CommerceTypes)iJ, kBuilding.getBonusCommerceModifier(iI, iJ) * iChange);
				changeBonusCommercePercentChanges((CommerceTypes)iJ, kBuilding.getBonusCommercePercentChanges(iI, iJ) * iChange);
			}
		}
	}
	foreach_(const TechArray& pair, kBuilding.getTechYieldChanges100())
	{
		if (GET_TEAM(getTeam()).isHasTech(pair.first))
		{
			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				changeBuildingExtraYield100((YieldTypes)iI, iChange * pair.second[(YieldTypes)iI]);
			}
		}
	}
	foreach_(const TechArray& pair, kBuilding.getTechYieldModifiers())
	{
		if (GET_TEAM(getTeam()).isHasTech(pair.first))
		{
			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				changeBuildingYieldModifier((YieldTypes)iI, iChange * pair.second[(YieldTypes)iI]);
			}
		}
	}
	foreach_(const TechCommerceArray& pair, kBuilding.getTechCommerceChanges100())
	{
		if (GET_TEAM(getTeam()).isHasTech(pair.first))
		{
			for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
			{
				changeBuildingCommerceTechChange((CommerceTypes)iI, iChange * pair.second[(CommerceTypes)iI]);
			}
		}
	}
	foreach_(const TechCommerceArray& pair, kBuilding.getTechCommerceModifiers())
	{
		if (GET_TEAM(getTeam()).isHasTech(pair.first))
		{
			for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
			{
				changeBuildingCommerceModifier((CommerceTypes)iI, iChange * pair.second[(CommerceTypes)iI]);
			}
		}
	}
	foreach_(const TerrainArray& pair, kBuilding.getTerrainYieldChanges())
	{
		YieldArray yields;

		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			yields[iI] = iChange * pair.second[iI];
		}
		changeTerrainYieldChanges(pair.first, yields);
	}
	foreach_(const PlotArray& pair, kBuilding.getPlotYieldChanges())
	{
		YieldArray yields;

		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			yields[iI] = iChange * pair.second[iI];
		}
		changePlotYieldChanges(pair.first, yields);
	}

	foreach_(const UnitCombatModifier2& modifier, kBuilding.getUnitCombatFreeExperience())
	{
		changeUnitCombatFreeExperience(modifier.first, modifier.second * iChange);
	}

	for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		changeDomainFreeExperience((DomainTypes)iI, kBuilding.getDomainFreeExperience(iI) * iChange);
		changeDomainProductionModifier((DomainTypes)iI, kBuilding.getDomainProductionModifier(iI) * iChange);
	}

	foreach_(const UnitModifier2& modifier, kBuilding.getUnitProductionModifiers())
	{
		changeUnitProductionModifier(modifier.first, modifier.second * iChange);
	}
	foreach_(const BuildingModifier2& modifier, kBuilding.getBuildingProductionModifiers())
	{
		changeBuildingProductionModifier(modifier.first, modifier.second * iChange);
	}
	foreach_(const BuildingCommerce& kChange, kBuilding.getGlobalBuildingCommerceChanges())
	{
		for (int i = 0; i < NUM_COMMERCE_TYPES; i++)
		{
			owner.changeBuildingCommerceChange(kChange.first, (CommerceTypes)i, kChange.second[i] * iChange);
		}
	}
	/*
	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			changeImprovementYieldChange(((ImprovementTypes)iI), ((YieldTypes)iJ), (kBuilding.getImprovementYieldChanges(iI, iJ) * iChange));
		}
	}
	*/
	{
		PROFILE("CvCity::processBuilding.Part5");
		updateExtraBuildingHappiness();
		updateExtraBuildingHealth();

		owner.changeAssets(kBuilding.getAssetValue() * iChange);

		area()->changePower(getOwner(), kBuilding.getPowerValue() * iChange);
		owner.changePower(kBuilding.getPowerValue() * iChange);

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getTeam()) && (iI == getOwner() || kBuilding.isTeamShare()))
			{
				GET_PLAYER((PlayerTypes)iI).processBuilding(eBuilding, iChange, area(), bReligiously);
			}
		}
	}

	GET_TEAM(getTeam()).processBuilding(eBuilding, iChange, bReligiously);

	if (!bReligiously)
	{
		PROFILE("CvCity::processBuilding.NotReligiouslyDisabling");

		GC.getGame().processBuilding(eBuilding, iChange);

		// Note: this whole section was pretty well qualified to be ignored on religious disabling.

		//TB DEFENSEBUG:  The following building defense line is allowing buildings that are replaced to continue to function!
		//We can only assume this entire section therefore gets around replaced buildings, particularly after a recalc.

		changeBuildingDefense(kBuilding.getDefenseModifier() * iChange);
		changeBuildingBombardDefense(kBuilding.getBombardDefenseModifier() * iChange);

		changeBaseGreatPeopleRate(kBuilding.getGreatPeopleRateChange() * iChange);

		UnitTypes eGreatPeopleUnit = (UnitTypes)kBuilding.getGreatPeopleUnitType();

		if (eGreatPeopleUnit != NO_UNIT)
		{
			changeGreatPeopleUnitRate(eGreatPeopleUnit, kBuilding.getGreatPeopleRateChange() * iChange);
		}

		const SpecialBuildingTypes eSpecialBuilding = kBuilding.getSpecialBuilding();
		if (eSpecialBuilding != NO_SPECIALBUILDING)
		{
			owner.changeBuildingGroupCount(eSpecialBuilding, iChange);
		}

		owner.changeWondersScore(getWonderScore(eBuilding) * iChange);

		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			if (kBuilding.getBonusDefenseChanges(iI) != 0)
			{
				changeBonusDefenseChanges((BonusTypes)iI, kBuilding.getBonusDefenseChanges(iI) * iChange);
			}
		}
	}
	updateBuildingCommerce();

	m_buildingSourcedPropertyCache.clear();

	// New or removed buildings can affect the assessment of the best plot builds
	AI_markBestBuildValuesStale();

	if (!bReligiously && GC.getGame().isOption(GAMEOPTION_RELIGION_DISABLING))
	{
		checkReligiousDisabling(eBuilding, owner);
		updateReligionHappiness();
		updateReligionCommerce();
	}
	setMaintenanceDirty(true);
	setLayoutDirty(true);
}


void CvCity::processProcess(ProcessTypes eProcess, int iChange)
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		changeProductionToCommerceModifier(((CommerceTypes)iI), (GC.getProcessInfo(eProcess).getProductionToCommerceModifier(iI) * iChange));
	}
}


void CvCity::processSpecialist(SpecialistTypes eSpecialist, int iChange)
{
	PROFILE_EXTRA_FUNC();
	UnitTypes eGreatPeopleUnit = (UnitTypes)GC.getSpecialistInfo(eSpecialist).getGreatPeopleUnitType();

	if (eGreatPeopleUnit != NO_UNIT)
	{
		changeGreatPeopleUnitRate(eGreatPeopleUnit, GC.getSpecialistInfo(eSpecialist).getGreatPeopleRateChange() * iChange);
	}

	changeBaseGreatPeopleRate(GC.getSpecialistInfo(eSpecialist).getGreatPeopleRateChange() * iChange);

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		changeExtraYield((YieldTypes)iI, (GC.getSpecialistInfo(eSpecialist).getYieldChange(iI) + GET_PLAYER(getOwner()).getSpecialistYieldPercentChanges(eSpecialist, (YieldTypes)iI) / 100) * iChange);
	}

	int iCommerceChangeTotal = 0;
	int iCommerceChangeModifierTotal = 0;
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		//changeSpecialistCommerce(((CommerceTypes)iI), (GC.getSpecialistInfo(eSpecialist).getCommerceChange(iI) * iChange));
		//TB Adjustment note: this makes the math a bit more accurate to the intention.  However, in this and the previous version
		//there is a potential for small mathematical error that is easily righted via a recalc.  IF a Player's SpecialistCommercePercentChanges adjusts since
		//the specialist was initially processed in and the specialist is then processed out, you will get an incorrect removal because the
		//commerce the specialist added was not updated when the Player's percent modifier to that value changed.  Again, small error and easily righted via a recalc.
		//If this proves to become a bigger issue than I think it will, we'll need to call a mini-recalculation on this value in each city for the player
		//every time an adjustment to SpecialistCommercePercentChanges takes place.  That would be the patient and correct way to address this.
		if (GC.getSpecialistInfo(eSpecialist).getCommerceChange(iI) != 0)
		{
			iCommerceChangeTotal += (100 * GC.getSpecialistInfo(eSpecialist).getCommerceChange(iI));
			iCommerceChangeModifierTotal += (iCommerceChangeTotal * GET_PLAYER(getOwner()).getSpecialistCommercePercentChanges(eSpecialist, (CommerceTypes)iI)) / 100;
			iCommerceChangeTotal += iCommerceChangeModifierTotal;
			changeSpecialistCommerceTimes100(((CommerceTypes)iI), iCommerceChangeTotal * iChange);
		}
		//Without reanalyzing, it looks like the above will omit Specialist Extra Commerce values from % modifiers to commerce percent changes?  hmm...  I think something may be fishy here.
	}

	updateExtraSpecialistYield();
	updateExtraSpecialistCommerce();
	updateSpecialistHappinessHealthFromTech();

	if (GC.getSpecialistInfo(eSpecialist).getHealthPercent() > 0)
	{
		changeSpecialistGoodHealth(GC.getSpecialistInfo(eSpecialist).getHealthPercent() * iChange);
	}
	else
	{
		changeSpecialistBadHealth(GC.getSpecialistInfo(eSpecialist).getHealthPercent() * iChange);
	}
	if (GC.getSpecialistInfo(eSpecialist).getHappinessPercent() > 0)
	{
		changeSpecialistHappiness(GC.getSpecialistInfo(eSpecialist).getHappinessPercent() * iChange);
	}
	else
	{
		changeSpecialistUnhappiness(GC.getSpecialistInfo(eSpecialist).getHappinessPercent() * iChange);
	}

	changeSpecialistFreeExperience(GC.getSpecialistInfo(eSpecialist).getExperience() * iChange);

	changeSpecialistInsidiousness(GC.getSpecialistInfo(eSpecialist).getInsidiousness() * iChange);
	changeSpecialistInvestigation(GC.getSpecialistInfo(eSpecialist).getInvestigation() * iChange);

	for (int iI = 0; iI < GC.getSpecialistInfo(eSpecialist).getNumUnitCombatExperienceTypes(); iI++)
	{
		if (GC.getSpecialistInfo(eSpecialist).getUnitCombatExperienceType(iI).eUnitCombat != NO_UNITCOMBAT)
		{
			changeUnitCombatFreeExperience(GC.getSpecialistInfo(eSpecialist).getUnitCombatExperienceType(iI).eUnitCombat, GC.getSpecialistInfo(eSpecialist).getUnitCombatExperienceType(iI).iModifier * iChange);
		}
	}
}


HandicapTypes CvCity::getHandicapType() const
{
	return GET_PLAYER(getOwner()).getHandicapType();
}


CivilizationTypes CvCity::getCivilizationType() const
{
	if (m_iCiv == NO_CIVILIZATION)
	{
		return GET_PLAYER(getOwner()).getCivilizationType();
	}
	return (CivilizationTypes)m_iCiv;
}


LeaderHeadTypes CvCity::getPersonalityType() const
{
	return GET_PLAYER(getOwner()).getPersonalityType();
}


ArtStyleTypes CvCity::getArtStyleType() const
{
	if (getOriginalOwner() != NO_PLAYER)
	{
		return GET_PLAYER(getOriginalOwner()).getArtStyleType();
	}
	return GET_PLAYER(getOwner()).getArtStyleType();
}

CitySizeTypes CvCity::getCitySizeType() const
{
	return ((CitySizeTypes)(range((getPopulation() / 7), 0, (NUM_CITYSIZE_TYPES - 1))));
}

const CvArtInfoBuilding* CvCity::getBuildingArtInfo(BuildingTypes eBuilding) const
{
	return GC.getBuildingInfo(eBuilding).getArtInfo();
}

float CvCity::getBuildingVisibilityPriority(BuildingTypes eBuilding) const
{
	return GC.getBuildingInfo(eBuilding).getVisibilityPriority();
}

bool CvCity::hasTrait(TraitTypes eTrait) const
{
	return GET_PLAYER(getOwner()).hasTrait(eTrait);
}

bool CvCity::isNPC() const
{
	return GET_PLAYER(getOwner()).isNPC();
}

bool CvCity::isHominid() const
{
	return GET_PLAYER(getOwner()).isHominid();
}

bool CvCity::isHuman() const
{
	return GET_PLAYER(getOwner()).isHumanPlayer();
}

bool CvCity::isVisible(TeamTypes eTeam, bool bDebug) const
{
	return plot()->isVisible(eTeam, bDebug);
}

bool CvCity::isCapital() const
{
	return (GET_PLAYER(getOwner()).getCapitalCity() == this);
}

bool CvCity::isCoastal(int iMinWaterSize) const
{
	return plot()->isCoastalLand(iMinWaterSize);
}

bool CvCity::isDisorder() const
{
	return (isOccupation() || GET_PLAYER(getOwner()).isAnarchy());
}

bool CvCity::isHolyCity(ReligionTypes eIndex) const
{
	return eIndex != NO_RELIGION && GC.getGame().getHolyCity(eIndex) == this;
}

bool CvCity::isHolyCity() const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if (isHolyCity((ReligionTypes)iI))
		{
			return true;
		}
	}
	return false;
}


bool CvCity::isHeadquarters(CorporationTypes eIndex) const
{
	return (GC.getGame().getHeadquarters(eIndex) == this);
}

void CvCity::setHeadquarters(CorporationTypes eIndex)
{
	if (!isHeadquarters(eIndex))
	{
		GC.getGame().setHeadquarters(eIndex, this, true);

		const UnitTypes eFreeUnit = (UnitTypes)GC.getCorporationInfo(eIndex).getFreeUnit();

		if (eFreeUnit != NO_UNIT)
		{
			GET_PLAYER(getOwner()).initUnit(eFreeUnit, getX(), getY(), NO_UNITAI, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
		}
	}
}

bool CvCity::isHeadquarters() const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (isHeadquarters((CorporationTypes)iI))
		{
			return true;
		}
	}

	return false;
}


int CvCity::getOvercrowdingPercentAnger(int iExtra) const
{
	if (getPopulation() + iExtra > 0)
	{
		return GC.getPERCENT_ANGER_DIVISOR();
	}
	return 0;
}


int CvCity::getNoMilitaryPercentAnger() const
{
	if (getMilitaryHappinessUnits() == 0)
	{
		return GC.getNO_MILITARY_PERCENT_ANGER();
	}
	return 0;
}


int CvCity::getCulturePercentAnger() const
{
	PROFILE_EXTRA_FUNC();
	const int iTotalCulture = plot()->countTotalCulture();

	if (iTotalCulture == 0)
	{
		return 0;
	}
	int iAngryCulture = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getTeam(), false))
		{
			int iCulture = plot()->getCulture((PlayerTypes)iI);

			if (iCulture > 0)
			{
				if (atWar(GET_PLAYER((PlayerTypes)iI).getTeam(), getTeam()))
				{
					if (iCulture > MAX_INT / 1000)
					{
						iCulture /= 100;
						iCulture *= std::max(0, (GC.getAT_WAR_CULTURE_ANGER_MODIFIER() + 100));
						iCulture *= 100;
					}
					else
					{
						iCulture *= std::max(0, (GC.getAT_WAR_CULTURE_ANGER_MODIFIER() + 100));
						iCulture /= 100;
					}
				}
				iAngryCulture += iCulture;
			}
		}
	}
	return GC.getCULTURE_PERCENT_ANGER() * iAngryCulture / iTotalCulture;
}


int CvCity::getReligionPercentAnger() const
{
	PROFILE_EXTRA_FUNC();
	if (GC.getGame().getNumCities() == 0)
	{
		return 0;
	}

	const int religionCount = getReligionCount();
	if (religionCount == 0)
	{
		return 0;
	}

	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		const CvPlayer& playerX = GET_PLAYER((PlayerTypes)iI);

		if (playerX.isAlive() && atWar(playerX.getTeam(), getTeam()))
		{
			FAssertMsg(playerX.getTeam() != getTeam(), "Player is at war with himself! :O");

			if (playerX.getStateReligion() != NO_RELIGION
			&& isHasReligion(playerX.getStateReligion()))
			{
				iCount += playerX.getHasReligionCount(playerX.getStateReligion());
			}
		}
	}
	const int iAnger = GC.getRELIGION_PERCENT_ANGER() * iCount / GC.getGame().getNumCities();

	return iAnger / religionCount;
}


int CvCity::getHurryPercentAnger(int iExtra) const
{
	if (getHurryAngerTimer() == 0)
	{
		return 0;
	}
	return 1 + (1 + (getHurryAngerTimer() - 1) / flatHurryAngerLength()) * GC.getHURRY_POP_ANGER() * GC.getPERCENT_ANGER_DIVISOR() / std::max(1, getPopulation() + iExtra);
}


int CvCity::getConscriptPercentAnger(int iExtra) const
{
	if (getConscriptAngerTimer() == 0)
	{
		return 0;
	}
	return 1 + (1 + (getConscriptAngerTimer() - 1) / flatConscriptAngerLength()) * GC.getCONSCRIPT_POP_ANGER() * GC.getPERCENT_ANGER_DIVISOR() / std::max(1, getPopulation() + iExtra);
}

int CvCity::getDefyResolutionPercentAnger(int iExtra) const
{
	if (getDefyResolutionAngerTimer() == 0)
	{
		return 0;
	}
	return 1 + (1 + (getDefyResolutionAngerTimer() - 1) / flatDefyResolutionAngerLength()) * GC.getDEFY_RESOLUTION_POP_ANGER() * GC.getPERCENT_ANGER_DIVISOR() / std::max(1, getPopulation() + iExtra);
}


int CvCity::getWarWearinessPercentAnger() const
{
	int iAnger = GET_PLAYER(getOwner()).getWarWearinessPercentAnger();

	iAnger *= std::max(0, (getWarWearinessModifier() + GET_PLAYER(getOwner()).getWarWearinessModifier() + 100));
	iAnger /= 100;

	iAnger *= std::max(0, (getWarWearinessTimer() + 100));
	iAnger /= 100;

	return iAnger;
}


int CvCity::getRevRequestPercentAnger(int iExtra) const
{
	if (getRevRequestAngerTimer() == 0)
	{
		return 0;
	}

	int iAnger = GC.getHURRY_ANGER_DIVISOR();
	iAnger *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
	iAnger /= 100;

	iAnger = std::max(1, iAnger);

	int iAngerPercent = 2 * (1 + (1 + (getRevRequestAngerTimer() - 1) / iAnger) * GC.getHURRY_POP_ANGER() * GC.getPERCENT_ANGER_DIVISOR() / std::max(1, getPopulation() + iExtra));

	return iAngerPercent;
}

int CvCity::getRevIndexPercentAnger() const
{
	const int iLocalAdjust = std::min(getLocalRevIndex() * 5, 100);

	int iAnger = (125 + iLocalAdjust) * (getRevolutionIndex() - 325) / 7500;
	if (iAnger < 1) return 0;

	iAnger = std::min(iAnger, 40);

	return iAnger * GC.getPERCENT_ANGER_DIVISOR() / 100;
}

int CvCity::getAngerPercent(const int iExtra) const
{
	return
	(
		getHurryPercentAnger(iExtra) +
		getConscriptPercentAnger(iExtra) +
		getDefyResolutionPercentAnger(iExtra) +
		getWarWearinessPercentAnger()
	);
}

int CvCity::getRevSuccessHappiness() const
{
	if (getRevSuccessTimer() == 0)
	{
		return 0;
	}

	int iHappy = GC.getHURRY_ANGER_DIVISOR();

	iHappy *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
	iHappy /= 100;
	iHappy = std::max(1, iHappy);

	int iHappyPercent = 2 * (1 + (1 + (getRevSuccessTimer() - 1) / iHappy) * GC.getHURRY_POP_ANGER() * GC.getPERCENT_ANGER_DIVISOR() / std::max(1, getPopulation()));

	return iHappyPercent * getPopulation() / GC.getPERCENT_ANGER_DIVISOR();
}


int CvCity::getLargestCityHappiness() const
{
	if (findPopulationRank() <= GC.getWorldInfo(GC.getMap().getWorldSize()).getTargetNumCities())
	{
		return GET_PLAYER(getOwner()).getLargestCityHappiness();
	}
	return 0;
}

int CvCity::getVassalHappiness() const
{
	PROFILE_EXTRA_FUNC();
	int iHappy = 0;

	for (int i = 0; i < MAX_TEAMS; i++)
	{
		if (getTeam() != i && GET_TEAM((TeamTypes)i).isVassal(getTeam()))
		{
			iHappy += GC.getVASSAL_HAPPINESS();
		}
	}
	return iHappy;
}

int CvCity::getVassalUnhappiness() const
{
	PROFILE_EXTRA_FUNC();
	int iUnhappy = 0;

	for (int i = 0; i < MAX_TEAMS; i++)
	{
		if (getTeam() != i && GET_TEAM(getTeam()).isVassal((TeamTypes)i))
		{
			iUnhappy += GC.getVASSAL_HAPPINESS();
		}
	}
	return iUnhappy;
}


// TheLadiesOgre - 20.10.2009
namespace
{
	int getCelebrityHappyClamped(const CvUnit* unit)
	{
		return std::max(0, unit->getCelebrityHappy());
	}
}
int CvCity::getCelebrityHappiness() const
{
	return algo::accumulate(plot()->units() | transformed(getCelebrityHappyClamped), 0);
}
// ! TheLadiesOgre


int CvCity::unhappyLevel(int iExtra) const
{
	PROFILE_FUNC();

	if (isNoUnhappiness() || isCapital() && GET_PLAYER(getOwner()).isNoCapitalUnhappiness())
	{
		return 0;
	}
	int iUnhappiness = 0;
	int iAngerPercent = 0;

	iAngerPercent += getOvercrowdingPercentAnger(iExtra);
	iAngerPercent += getNoMilitaryPercentAnger();
	iAngerPercent += getCulturePercentAnger();
	iAngerPercent += getReligionPercentAnger();
	iAngerPercent += getHurryPercentAnger(iExtra);
	iAngerPercent += getConscriptPercentAnger(iExtra);
	iAngerPercent += getDefyResolutionPercentAnger(iExtra);
	iAngerPercent += getWarWearinessPercentAnger();
	iAngerPercent += getRevRequestPercentAnger(iExtra);
	iAngerPercent += getRevIndexPercentAnger();

	for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		iAngerPercent += GET_PLAYER(getOwner()).getCivicPercentAnger((CivicTypes)iI);
	}

	iUnhappiness = ((iAngerPercent * (getPopulation() + iExtra)) / GC.getPERCENT_ANGER_DIVISOR());

	iUnhappiness -= std::min(0, getLargestCityHappiness());
	iUnhappiness -= std::min(0, getMilitaryHappiness());
	iUnhappiness -= std::min(0, getCurrentStateReligionHappiness());
	iUnhappiness -= std::min(0, getBuildingBadHappiness());
	iUnhappiness -= std::min(0, getExtraBuildingBadHappiness());
	iUnhappiness -= std::min(0, getFeatureBadHappiness());
	iUnhappiness -= std::min(0, getBonusBadHappiness());
	iUnhappiness -= std::min(0, getReligionBadHappiness());
	iUnhappiness -= std::min(0, getCommerceHappiness());
	iUnhappiness -= std::min(0, area()->getBuildingHappiness(getOwner()));
	iUnhappiness -= std::min(0, GET_PLAYER(getOwner()).getBuildingHappiness());
	iUnhappiness -= std::min(0, (getExtraHappiness() + GET_PLAYER(getOwner()).getExtraHappiness()));
	iUnhappiness -= std::min(0, GC.getHandicapInfo(getHandicapType()).getHappyBonus());
	iUnhappiness += std::max(0, getVassalUnhappiness());
	iUnhappiness += std::max(0, getEspionageHappinessCounter());
	iUnhappiness -= std::min(0, getCivicHappiness());
	iUnhappiness -= std::min(0, getSpecialistUnhappiness() / 100);
	iUnhappiness -= std::min(0, (GET_PLAYER(getOwner()).getWorldHappiness()));
	iUnhappiness -= std::min(0, (GET_PLAYER(getOwner()).getProjectHappiness()));
	iUnhappiness += std::max(0, GET_PLAYER(getOwner()).calculateTaxRateUnhappiness());
	iUnhappiness -= std::min(0, calculateCorporationHappiness());
	iUnhappiness += std::max(0, getEventAnger());
	iUnhappiness -= std::min(0, getExtraTechHappinessTotal());

	int iForeignAnger = GET_PLAYER(getOwner()).getForeignUnhappyPercent();
	if (iForeignAnger != 0) {
		iForeignAnger = 100 / iForeignAnger;
		iForeignAnger = ((100 - plot()->calculateCulturePercent(getOwner())) * iForeignAnger) / 100;
		iUnhappiness += std::max(0, iForeignAnger);
	}
	if (GC.getGame().isOption(GAMEOPTION_MAP_PERSONALIZED))
	{
		if (!GET_PLAYER(getOwner()).isNoLandmarkAnger())
		{
			iUnhappiness += std::max(0, getLandmarkAnger());
		}
		iUnhappiness -= std::min(0, GET_PLAYER(getOwner()).getLandmarkHappiness());
	}

	if (GET_PLAYER(getOwner()).getCityLimit() != 0 &&
		GET_PLAYER(getOwner()).getCityOverLimitUnhappy() != 0)
	{
		int overLimitCities = GET_PLAYER(getOwner()).getNumCities() - GET_PLAYER(getOwner()).getCityLimit();

		if (overLimitCities > 0)
		{
			iUnhappiness += GET_PLAYER(getOwner()).getCityOverLimitUnhappy() * overLimitCities;
		}
	}

	return std::max(0, iUnhappiness);
}


int CvCity::happyLevel() const
{
	PROFILE_FUNC();

	int iHappiness = 0;

	iHappiness += std::max(0, getRevSuccessHappiness());
	iHappiness += std::max(0, getLargestCityHappiness());
	iHappiness += std::max(0, getMilitaryHappiness());
	iHappiness += std::max(0, getCurrentStateReligionHappiness());
	iHappiness += std::max(0, getBuildingGoodHappiness());
	iHappiness += std::max(0, getExtraBuildingGoodHappiness());
	iHappiness += std::max(0, getFeatureGoodHappiness());
	iHappiness += std::max(0, getBonusGoodHappiness());
	iHappiness += std::max(0, getReligionGoodHappiness());
	iHappiness += std::max(0, getCommerceHappiness());
	iHappiness += std::max(0, area()->getBuildingHappiness(getOwner()));
	iHappiness += std::max(0, GET_PLAYER(getOwner()).getBuildingHappiness());
	iHappiness += std::max(0, (getExtraHappiness() + GET_PLAYER(getOwner()).getExtraHappiness()));
	iHappiness += std::max(0, GC.getHandicapInfo(getHandicapType()).getHappyBonus());
	iHappiness += std::max(0, getVassalHappiness());
	iHappiness += std::max(0, getCivicHappiness());
	iHappiness += std::max(0, getSpecialistHappiness() / 100);
	iHappiness += std::max(0, (GET_PLAYER(getOwner()).getWorldHappiness()));
	iHappiness += std::max(0, (GET_PLAYER(getOwner()).getProjectHappiness()));
	iHappiness += std::max(0, calculateCorporationHappiness());
	iHappiness += std::max(0, getCelebrityHappiness());
	iHappiness += std::max(0, getExtraTechHappinessTotal());

	if (GC.getGame().isOption(GAMEOPTION_MAP_PERSONALIZED))
	{
		iHappiness += std::max(0, GET_PLAYER(getOwner()).getLandmarkHappiness());
	}
	if (getHappinessTimer() > 0)
	{
		iHappiness += GC.getTEMP_HAPPY();
	}
	return std::max(0, iHappiness);
}


int CvCity::angryPopulation(int iExtra) const
{
	PROFILE("CvCityAI::angryPopulation");

	return range(unhappyLevel(iExtra) - happyLevel(), 0, getPopulation() + iExtra);
}


int CvCity::visiblePopulation() const
{
	return getPopulation() - angryPopulation() - getWorkingPopulation() - getNumPopulationEmployed();
}


int CvCity::totalFreeSpecialists() const
{
	PROFILE_EXTRA_FUNC();
	if (getPopulation() < 1)
	{
		return 0;
	}
	int iCount = getFreeSpecialist() + area()->getFreeSpecialist(getOwner()) + GET_PLAYER(getOwner()).getFreeSpecialist();

	for (int iI = 0; iI < GC.getNumImprovementInfos(); ++iI)
	{
		const int iNumSpecialistsPerImprovement = getImprovementFreeSpecialists((ImprovementTypes)iI);
		if (iNumSpecialistsPerImprovement != 0)
		{
			iCount += iNumSpecialistsPerImprovement * countNumImprovedPlots((ImprovementTypes)iI);
		}
	}
	if (GET_PLAYER(getOwner()).hasFreeSpecialistperWorldWonder())
	{
		iCount += getNumWorldWonders();
	}
	if (GET_PLAYER(getOwner()).hasFreeSpecialistperNationalWonder())
	{
		iCount += getNumNationalWonders();
	}
	if (GET_PLAYER(getOwner()).hasFreeSpecialistperTeamProject())
	{
		iCount += getNumTeamWonders();
	}
	// Toffer - Negative free specialist effectively reduce pop of city...
	//	That's not an intended effect of the free specialist feature.
	return std::max(0, iCount);
}


int CvCity::extraPopulation() const
{
	return visiblePopulation() + std::min(0, extraFreeSpecialists());
}


int CvCity::extraSpecialists() const
{
	return visiblePopulation() + extraFreeSpecialists();
}


int CvCity::extraFreeSpecialists() const
{
	return totalFreeSpecialists() - getSpecialistPopulation();
}


int CvCity::unhealthyPopulation(bool bNoAngry, int iExtra) const
{
	if (isNoUnhealthyPopulation())
	{
		return 0;
	}
	return std::max(0, getPopulation() + iExtra - (bNoAngry ? angryPopulation(iExtra) : 0));
}


int CvCity::totalGoodBuildingHealth() const
{
	return getBuildingGoodHealth()
		+ area()->getBuildingGoodHealth(getOwner())
		+ GET_PLAYER(getOwner()).getBuildingGoodHealth()
		+ getExtraBuildingGoodHealth()
		+ std::max(0, calculatePopulationHealth());
}


int CvCity::totalBadBuildingHealth() const
{
	if (isBuildingOnlyHealthy())
	{
		return 0;
	}
	return getBuildingBadHealth()
		+ area()->getBuildingBadHealth(getOwner())
		+ GET_PLAYER(getOwner()).getBuildingBadHealth()
		+ getExtraBuildingBadHealth()
		+ std::min(0, calculatePopulationHealth());
}


int CvCity::goodHealth() const
{
	PROFILE_FUNC();
	const CvPlayer& owner = GET_PLAYER(getOwner());

	int iTotalHealth = 0;

	iTotalHealth += std::max<int>(0, getFreshWaterGoodHealth());
	iTotalHealth += std::max<int>(0, getFeatureGoodHealth());
	iTotalHealth += std::max<int>(0, getBonusGoodHealth());
	iTotalHealth += std::max<int>(0, totalGoodBuildingHealth());
	iTotalHealth += std::max<int>(0, getExtraHealth());
	iTotalHealth += std::max<int>(0, GC.getHandicapInfo(getHandicapType()).getHealthBonus());
	iTotalHealth += std::max<int>(0, getImprovementGoodHealth() / 100);
	iTotalHealth += std::max<int>(0, getSpecialistGoodHealth() / 100);
	iTotalHealth += std::max<int>(0, calculateCorporationHealth());
	iTotalHealth += std::max<int>(0, getExtraTechHealthTotal());
	iTotalHealth += std::max<int>(0, owner.getExtraHealth());
	iTotalHealth += std::max<int>(0, owner.getCivicHealth());
	iTotalHealth += std::max<int>(0, owner.getCivilizationHealth());
	iTotalHealth += std::max<int>(0, owner.getWorldHealth());
	iTotalHealth += std::max<int>(0, owner.getProjectHealth());

	return iTotalHealth;
}


int CvCity::badHealth(bool bNoAngry, int iExtra) const
{
	PROFILE_FUNC();
	const CvPlayer& owner = GET_PLAYER(getOwner());

	int iTotalHealth = 0;

	iTotalHealth -= std::max<int>(0, getEspionageHealthCounter());
	iTotalHealth += std::min<int>(0, getFeatureBadHealth());
	iTotalHealth += std::min<int>(0, getBonusBadHealth());
	iTotalHealth += std::min<int>(0, totalBadBuildingHealth());
	iTotalHealth += std::min<int>(0, getExtraHealth());
	iTotalHealth += std::min<int>(0, GC.getHandicapInfo(getHandicapType()).getHealthBonus());
	iTotalHealth += std::min<int>(0, getExtraBuildingBadHealth());
	iTotalHealth += std::min<int>(0, getImprovementBadHealth() / 100);
	iTotalHealth += std::min<int>(0, getSpecialistBadHealth() / 100);
	iTotalHealth += std::min<int>(0, calculateCorporationHealth());
	iTotalHealth += std::min<int>(0, getExtraTechHealthTotal());
	iTotalHealth += std::min<int>(0, owner.getExtraHealth());
	iTotalHealth += std::min<int>(0, owner.getCivicHealth());
	iTotalHealth += std::min<int>(0, owner.getCivilizationHealth());
	iTotalHealth += std::min<int>(0, owner.getWorldHealth());
	iTotalHealth += std::min<int>(0, owner.getProjectHealth());

	return unhealthyPopulation(bNoAngry, iExtra) - iTotalHealth;
}


int CvCity::healthRate(bool bNoAngry, int iExtra) const
{
	return std::min(0, (goodHealth() - badHealth(bNoAngry, iExtra)));
}


// Toffer - Gradual food consumption change
//	Food consumption should gradually increment, or decrement, during the growth process so that
// 	a pop growth, or decline, doesn't entail an abrumpt jump in food consumption from one turn to the other.
int CvCity::getPopulationPlusProgress100(const int iExtra) const
{
	if (iExtra == 0)
	{
		return 100 * getPopulation() + 100 * getFood() / growthThreshold();
	}
	return 100 * (getPopulation() + iExtra) + getFoodKeptPercent();
}

int CvCity::getFoodConsumedPerPopulation100(const int iExtra) const
{
	// Treat pop 1.00 or less as zero in this regard.
	const int iPop100 = getPopulationPlusProgress100(iExtra) - 100;
	if (iPop100 <= 0)
	{
		return 100 * GC.getFOOD_CONSUMPTION_PER_POPULATION();
	}
	// The above is only strictly needed in case iPop100 becomes a negative value due to negative iExtra value input.
	//	Negative iExtra value is currently never passed to this function, so it's a future proof thing.
	return 100 * GC.getFOOD_CONSUMPTION_PER_POPULATION() + (iPop100 - 100) * GC.getFOOD_CONSUMPTION_PER_POPULATION_PERCENT() / 100;
}

int CvCity::getFoodConsumedByPopulation(const int iExtra) const
{
	return getPopulationPlusProgress100(iExtra) * getFoodConsumedPerPopulation100() / 10000;
}

int CvCity::foodConsumption(const bool bNoAngry, const int iExtra, const bool bIncludeWastage) const
{
	return getFoodConsumedByPopulation(iExtra)
		- (bNoAngry ? angryPopulation(iExtra) : 0) // Doesn't belong here, should be extracted out to wherever it is needed
		- healthRate(bNoAngry, iExtra)
		+ (bIncludeWastage ? (int)foodWastage() : 0);
}
// ! Toffer - Gradual food consumption change


// Included by Thunderbrd 6/8/2019, code contributed by Sorcdk
float CvCity::foodWastage(int surplass) const
{
#define MAX_SURPLASS    200
	static float calculatedWaste[MAX_SURPLASS];
	static int calculatedTo = -1;
	const int startWasteAtConsumptionPercent = GC.getWASTAGE_START_CONSUMPTION_PERCENT();
	float wastageGrowthFactor = GC.getWASTAGE_GROWTH_FACTOR();

	if (wastageGrowthFactor == 0)
	{
		wastageGrowthFactor = (float)0.05; // default
	}

	if (startWasteAtConsumptionPercent >= 0)
	{
		if (surplass == -1)
		{
			surplass = foodDifference(true, false) - getFoodConsumedByPopulation() * startWasteAtConsumptionPercent / 100;
		}
	}
	else surplass = -1;

	// Nothing wasted if there is no surplass
	if (surplass <= 0)
	{
		return 0;
	}
	// Cache what we can as it's not a trivially cheap computation
	else if (surplass <= calculatedTo)
	{
		return calculatedWaste[surplass];
	}
	else if (surplass >= MAX_SURPLASS)
	{
		// After the max we shift to from assymtotic behavior toward the limit to the limit of efficiency
		return foodWastage(MAX_SURPLASS - 1) + (foodWastage(MAX_SURPLASS - 1) - foodWastage(MAX_SURPLASS - 2)) * (surplass - MAX_SURPLASS + 1);
	}
	else
	{
		calculatedWaste[surplass] = foodWastage(surplass - 1) + (float)1.0 - (wastageGrowthFactor + ((float)1.0 - wastageGrowthFactor) / ((float)1.0 + (float)0.05 * (float)surplass));
		calculatedTo = surplass;

		return calculatedWaste[surplass];
	}
}


int CvCity::foodDifference(const bool bBottom, const bool bIncludeWastage, const bool bIgnoreFoodBuildOrRev) const
{
	if (!bIgnoreFoodBuildOrRev && isDisorder())
	{
		return 0;
	}
	int iDifference;

	if (!bIgnoreFoodBuildOrRev && isFoodProduction())
	{
		iDifference = std::min(0, getYieldRate(YIELD_FOOD) - foodConsumption(false, 0, bIncludeWastage));
	}
	else iDifference = getYieldRate(YIELD_FOOD) - foodConsumption(false, 0, bIncludeWastage);

	if (bBottom && getPopulation() == 1 && getFood() == 0)
	{
		iDifference = std::max(0, iDifference);
	}

	return iDifference;
}


int CvCity::growthThreshold(const int iPopChange) const
{
	const int iThreshold = (
		getModifiedIntValue(
			GET_PLAYER(getOwner()).getGrowthThreshold(getPopulation() + iPopChange),
			getPopulationgrowthratepercentage() + GET_PLAYER(getOwner()).getPopulationgrowthratepercentage()
		)
	);
	if (isHominid())
	{
		return std::max(1, iThreshold / 2); // Those barbarians are just so damned fecund!
	}
	return std::max(1, iThreshold);
}


int CvCity::productionLeft() const
{
	return getProductionNeeded() - getProductionProgress();
}

int CvCity::getHurryCostModifier() const
{
	bst::optional<OrderData> order = getHeadOrder();

	if (order)
	{
		switch (order->eOrderType)
		{
		case ORDER_TRAIN:
			return getHurryCostModifier(order->getUnitType());
		case ORDER_CONSTRUCT:
			return getHurryCostModifier(order->getBuildingType());
		case ORDER_CREATE:
		case ORDER_MAINTAIN:
		case ORDER_LIST:
			break;

		default:
			FErrorMsg("order->eOrderType did not match a valid option");
			break;
		}
	}
	return 100;
}

int CvCity::getHurryCostModifier(UnitTypes eUnit) const
{
	return getHurryCostModifier(GC.getUnitInfo(eUnit).getHurryCostModifier(), getProductionModifier(eUnit));
}

int CvCity::getHurryCostModifier(BuildingTypes eBuilding) const
{
	return getHurryCostModifier(GC.getBuildingInfo(eBuilding).getHurryCostModifier(), getProductionModifier(eBuilding));
}

int CvCity::getHurryCostModifier(int iBaseModifier, int iExtraMod) const
{
	int iModifier = 100 + iBaseModifier;

	if (iExtraMod != 0)
	{
		iModifier = getModifiedIntValue(iModifier, iExtraMod);
	}
	iModifier = getModifiedIntValue(iModifier, GET_PLAYER(getOwner()).getHurryModifier());
	iModifier = getModifiedIntValue(iModifier, GET_PLAYER(getOwner()).getHurryCostModifier());

	return std::max(1, iModifier); // Avoid potential divide by 0s
}


int CvCity::hurryCost() const
{
	return getHurryCost(productionLeft(), getHurryCostModifier());
}

int CvCity::getHurryCost(UnitTypes eUnit) const
{
	return getHurryCost(getProductionNeeded(eUnit) - getProgressOnUnit(eUnit), getHurryCostModifier(eUnit));
}

int CvCity::getHurryCost(BuildingTypes eBuilding) const
{
	return getHurryCost(getProductionNeeded(eBuilding) - getProgressOnBuilding(eBuilding), getHurryCostModifier(eBuilding));
}

int CvCity::getHurryCost(int iProductionLeft, int iHurryModifier) const
{
	return std::max(0, (iProductionLeft * iHurryModifier + 99) / 100); // round up
}

int64_t CvCity::getHurryGold(const HurryTypes eHurry, int iHurryCost) const
{
	if (GC.getHurryInfo(eHurry).getGoldPerProduction() == 0)
	{
		return 0;
	}
	if (iHurryCost < 0)
	{
		iHurryCost = hurryCost();
	}
	int64_t iGold = iHurryCost * GC.getHurryInfo(eHurry).getGoldPerProduction();

	FAssert(iGold <= 2000000000); // We'll need to take measures if this comes up

	return std::max<int64_t>(1, iGold);
}


int CvCity::hurryPopulation(HurryTypes eHurry) const
{
	return getHurryPopulation(eHurry, hurryCost());
}

int CvCity::getHurryPopulation(HurryTypes eHurry, int iHurryCost) const
{
	const int prodPerPop = GC.getGame().getProductionPerPopulation(eHurry);
	if (prodPerPop < 1)
	{
		return 0;
	}
	return std::max(1, 1 + (iHurryCost - 1) / prodPerPop);
}

int CvCity::hurryProduction(HurryTypes eHurry) const
{
	const int prodPerPop = GC.getGame().getProductionPerPopulation(eHurry);
	if (prodPerPop > 0)
	{
		const int iProduction = 100 * hurryPopulation(eHurry) * prodPerPop / std::max(1, getHurryCostModifier());
		FAssert(iProduction >= productionLeft());
		return iProduction;
	}
	return productionLeft();
}


int CvCity::flatHurryAngerLength() const
{
	int iAnger = GC.getHURRY_ANGER_DIVISOR();
	iAnger *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
	iAnger /= 100;
	iAnger *= std::max(0, 100 + getHurryAngerModifier());
	iAnger /= 100;

	return std::max(1, iAnger);
}


int CvCity::hurryAngerLength(HurryTypes eHurry) const
{
	return GC.getHurryInfo(eHurry).isAnger() ? flatHurryAngerLength() : 0;
}


int CvCity::maxHurryPopulation() const
{
	return (getPopulation() / 2);
}


// We enter realistic culture here
int CvCity::cultureDistance(const CvPlot& plot) const
{
	// Entry point for realistic culture. Tries to use cached value if possible,
	// if not, must recalc everything in range. Not sure if cache is stored separately
	// for each city or just locally for the function whenever called?
	// In either case need to recompute cache each turn because many things can change distance.
	PROFILE_FUNC();

	if (GC.getGame().isOption(GAMEOPTION_CULTURE_REALISTIC_SPREAD))
	{
		std::map<const CvPlot*, int>::const_iterator itr = m_aCultureDistances.find(&plot);

		if (itr == m_aCultureDistances.end())
		{
			// Need culture level, not coordinate, because maybe shortest route
			// is in a loop of distance greater than coord offset
			recalculateCultureDistances(std::max(0, (int)getCultureLevel()));

			return m_aCultureDistances[&plot];
		}
		else
		{
			return itr->second;
		}
	}
	return plotDistance(getX(), getY(), plot.getX(), plot.getY());
}

void CvCity::recalculateCultureDistances(int iMaxDistance) const
{
	// This function does some special checks for tiles adjacent to city,
	// then (inefficiently, atm) calls calculateCultureDistance to compute specific tiles
	PROFILE_FUNC();

	// MCB w/o 1TF makes 8 adjacent tiles dist 1 ('soft' RCS)
	if (GC.getGame().isOption(GAMEOPTION_CULTURE_MIN_CITY_BORDER) && !GC.getGame().isOption(GAMEOPTION_CULTURE_1_CITY_TILE_FOUNDING))
	{
		foreach_(const CvPlot* plotX, plot()->adjacent())
		{
			m_aCultureDistances[plotX] = 1;
		}
	}
	// City tile itself should always have dist 0
	m_aCultureDistances[plot()] = 0;

	// Blaze: Spiraling outward from center (style of getCityIndexPlot) is more efficient if perf issues exist;
	// 	this implementation is rather brute-force and inefficient. Will need to make edits to existing func tho,
	// 	because getCityIndexPlot(0) == getCityIndexPlot(37), and we'll need a few more plots than that...
	// 	Maybe pass an optional bool arg telling it not to take the modulus or however it loops? Also plots() func, need look into.

	// Currently: Calculate distance values of all tiles in iMaxDistance radial size grid,
	//   recalculating entire grid until no values have changed. This happens ~iMaxDistance times per city per turn.
	bool bHasChanged = iMaxDistance > 0;

	int numLoops = 0;
	// as long as there are changes during the last iteration
	while (bHasChanged)
	{
		// reset the has changed variable to note a new loop cycle has begun
		bHasChanged = false;

		foreach_(const CvPlot* plotX, plot()->rect(iMaxDistance, iMaxDistance))
		{
			// This is a slightly cursed function.
			// Center should never be calculated:
			if (plotX == plot()) continue;
			// MCB w/o 1TF makes adjacent 8 always distance 1; should not be custom calculated
			if (GC.getGame().isOption(GAMEOPTION_CULTURE_MIN_CITY_BORDER) && !GC.getGame().isOption(GAMEOPTION_CULTURE_1_CITY_TILE_FOUNDING))
			{
				bool bCityAdjacent = false;
				foreach_(const CvPlot* plotAdjacent, plotX->adjacent())
				{
					if (plotAdjacent == plot())
					{
						bCityAdjacent = true;
						break;
					}
				}
				if (bCityAdjacent) continue;
			}

			// recalculate the value to determine if it has changed
			const int iNewValue = calculateCultureDistance(plotX, iMaxDistance);

			// if it has changed, save the value and mark that
			//   ALL values (!) should be recomputed since they
			//   may depend on this value
			if (m_aCultureDistances[plotX] != iNewValue)
			{
				m_aCultureDistances[plotX] = iNewValue;
				bHasChanged = true;
			}
		}
		numLoops++;
		if (numLoops > 100)
		{
			bHasChanged = false;
			FAssertMsg(numLoops < 100, "Realistic Culture hit infinte loop trying to update distances! Bypassing...");
		}
	}
}

int CvCity::calculateCultureDistance(const CvPlot* mainPlot, int iMaxDistance) const
{
	// This function is the meat of Realistic Culture, determines extra distance/cost for tiles.
	// Each point of distance corresponds 1:1 with level of city culture to reach it.
	PROFILE_FUNC();

	// if the plot distance is greater than the maximum desired plot distance
	// or if the plot does not exist, then the plot distance is maximal
	if (!mainPlot || plotDistance(getX(), getY(), mainPlot->getX(), mainPlot->getY()) > iMaxDistance)
		return MAX_INT;

	// Potentially don't need to calculate anything if plot has no already calculated neighbors
	const CvTeam& kTeam = GET_TEAM(getTeam());
	const bool bIsWater = mainPlot->isWater();
	const bool bIsCity = mainPlot->isCity();
	const bool bIsBonus = mainPlot->getBonusType(getTeam()) != NO_BONUS;
	const int terrainType = mainPlot->getTerrainType();
	const int featureType = mainPlot->getFeatureType();
	const int improvementType = mainPlot->getImprovementType();
	const int routeType = mainPlot->getRouteType();
	const int owner = mainPlot->getOwner();

// C++03 compatible iteration
bool bHasCalculatedNeighbors = false;
std::vector<std::pair<const CvPlot*, int> > validNeighbors; // Note the space between > >

CvPlot::adjacent_range adjacentPlots = mainPlot->cardinalDirectionAdjacent();
for (CvPlot::adjacent_range::const_iterator it = adjacentPlots.begin(); it != adjacentPlots.end(); ++it)
{
	const CvPlot* adjacentPlot = *it;
	int neighborDist = m_aCultureDistances[adjacentPlot];
	if (neighborDist != MAX_INT && (neighborDist != 0 || adjacentPlot == plot()))
	{
		bHasCalculatedNeighbors = true;
		validNeighbors.push_back(std::make_pair(adjacentPlot, neighborDist));
	}
}

if (!bHasCalculatedNeighbors) return MAX_INT;

	// Terrain/feature/route calculations
	int terrainDistance = 0;
	// Distance from ground tile type (peak or specific terrain)
	if (mainPlot->isAsPeak())
	{
		terrainDistance += GC.getTerrainInfo(GC.getTERRAIN_PEAK()).getCultureDistance();
		terrainDistance += !kTeam.isMoveFastPeaks()
			+ !kTeam.isCanPassPeaks()
			+ !kTeam.isCanFoundOnPeaks();
	}
	else
	{
		terrainDistance += GC.getTerrainInfo((TerrainTypes)terrainType).getCultureDistance();
		// Terrain distance increased if can't trade on water terrain, can't see far (optics)
		if (bIsWater)
		{
			if (!kTeam.isTerrainTrade((TerrainTypes)terrainType)) terrainDistance += 2;
			if (!mainPlot->isAdjacentToLand() && !kTeam.isExtraWaterSeeFrom()) terrainDistance += 1;
		}
		else
		{
			// Freshwater penalty acts as an inhibitor; effectively reduced as farms spread irrigation,
			// removed at bAllowsDesertFarming (refrigeration)
			if (!mainPlot->isFreshWater() && !kTeam.isCanFarmDesert()) terrainDistance += 1;
			if (mainPlot->isHills())
			{
				terrainDistance += GC.getTerrainInfo(GC.getTERRAIN_HILL()).getCultureDistance()
					+ !kTeam.isCanFoundOnPeaks()
					+ !kTeam.isBridgeBuilding();
			}
		}
	}

	// Distance from features
	if (featureType != NO_FEATURE)
	{
		// some features cause underlying terrain cost to be ignored; oasis, floodplain, natural wonders
		const CvFeatureInfo& featureInfo = GC.getFeatureInfo((FeatureTypes)featureType);
		if (featureInfo.isIgnoreTerrainCulture())
		{
			terrainDistance = 0;
		}
		// penalty for unimproved features
		else if (improvementType == NO_IMPROVEMENT)
		{
			terrainDistance += 1;
		}
		// penalty for improved features if outside city affected territory
		else if (!mainPlot->isInCultureRangeOfCityByPlayer((PlayerTypes)owner)
	 || !mainPlot->isInCultureRangeOfCityByPlayer(getOwner()))
		{
			terrainDistance += 1;
		}
		// Always add base feature cost
		terrainDistance += featureInfo.getCultureDistance();
	}

	// Route tier modifier
	// Using route value/tier as softer land tile inhibitor by era
	// Base distance (x): 0, 1, 2, 3, 4 ... translates to:
	// Tier 0 (noroute):  0, 1, 3, 5, 7 ... // 2x-1: Always applies to tiles not yet influenced
	// Tier 1 (trail):	  0, 1, 3, 4, 6 ... // 3x/2
	// Tier 2 (path):	  0, 1, 2, 4, 5 ... // 4x/3
	// Tier 3 (road):	  0, 1, 2, 3, 5 ... // 5x/4
	if (!bIsWater && !bIsBonus)
	{
		int routeTierMod = 0;
		if (routeType != NO_ROUTE &&
			(mainPlot->isInCultureRangeOfCityByPlayer((PlayerTypes)owner) ||
				mainPlot->isInCultureRangeOfCityByPlayer(getOwner())))
		{
			routeTierMod = std::max(routeTierMod, GC.getRouteInfo((RouteTypes)routeType).getValue());
		}
		// Penalties applied for low tier routes (pre-paved road)
		if (routeTierMod == 0)
		{
			terrainDistance = std::max(0, 1 + 2 * (terrainDistance - 1));
		}
		else if (routeTierMod < 4)
		{
			terrainDistance = terrainDistance * (routeTierMod + 2) / (routeTierMod + 1);
		}
	}

	// Halve terrain distance if bonus is present
	if (bIsBonus) terrainDistance /= 2;

	// Final neighbor loop
	/* Determine the final cultural distance of given plot:
		1: All directions from given plot are checked (could come from weird direction)
		2: Neighbors with distance of MAX_INT are ignored because they
			don't exist or haven't been calculated yet
		3: Smallest total possible distance is used from all neighbors
		4: Greater river penalty for lacking river trade, bridge building
		5: City tiles are easier to influence, even across river */
	int distance = MAX_INT;
	const int extraRiverPenalty = !kTeam.isBridgeBuilding() + !kTeam.isRiverTrade();

	for (std::vector<std::pair<const CvPlot*, int> >::const_iterator it = validNeighbors.begin(); it != validNeighbors.end(); ++it)
	{
		// An uncalculated tile will either have MAX_INT (no neighbors on prev calc) or 0 (1st iteration).
		// 0 may be used if it's the city tile itself, though.
		const CvPlot* adjacentPlot = it->first;
		int neighborDist = it->second;
		int netDistanceModifier = terrainDistance;
		// If we are adjacent to our own city center, different rules. Cheaper, but not straight 0 cost.

		if (neighborDist == 0)
		{
			netDistanceModifier += mainPlot->isRiverCrossing(directionXY(mainPlot, adjacentPlot)) * (extraRiverPenalty);
			netDistanceModifier /= 3;
		}
		else
		{
			netDistanceModifier += mainPlot->isRiverCrossing(directionXY(mainPlot, adjacentPlot)) * (1 + extraRiverPenalty) * 2;
		}

		// Other city tiles are easier to influence even if across river (city itself crosses river)
		netDistanceModifier /= (1 + bIsCity);

		int totalDist = neighborDist + 1 + std::max(0, netDistanceModifier);
		if (totalDist < distance) distance = totalDist;
	}
	return distance;
}



void CvCity::clearCultureDistanceCache()
{
	m_aCultureDistances.clear();
}
// End realistic culture


int CvCity::netRevoltRisk100(PlayerTypes cultureAttacker) const
{
	// Returns 100x % chance of revolt to eCultureAttacker when modified by defending units
	// 108 = 1.08%, 9,876 = 98.76%
	return std::min(10000, std::max(0, baseRevoltRisk100(cultureAttacker) * (unitRevoltRiskModifier(cultureAttacker))) / 100);
}


int CvCity::baseRevoltRisk100(PlayerTypes eCultureAttacker) const
{
	PROFILE_EXTRA_FUNC();
	// Returns 100x% chance of revolt to eCultureAttacker unmodified by defending units
	// Should probably be less dependent on era or pop.
	int iRisk = (getHighestPopulation() * 2);

	// Presence of 3rd party culture lowers max bonus
	const int iAttackerPercent100 = plot()->calculateCulturePercent(eCultureAttacker, 2);
	int iDefenderPercent100 = plot()->calculateCulturePercent(getOwner(), 2);
	// Adjust defender percent by possible fixed border modifier
	// (otherwise inflated risk when FB city is threatened)
	if (GET_PLAYER(getOwner()).hasFixedBorders())
	{
		iDefenderPercent100 *= (GC.getDefineINT("FIXED_BORDERS_CULTURE_RATIO_PERCENT"));
		iDefenderPercent100 /= 100;
	}

	// If adjacent tiles can be acquired, factor in, else there's an additional min risk
	if (!GC.getGame().isOption(GAMEOPTION_CULTURE_MIN_CITY_BORDER))
	{
		foreach_(const CvPlot* pLoopPlot, plot()->adjacent()
		| filtered(CvPlot::fn::getOwner() == eCultureAttacker))
		{
			iRisk += (GC.getGame().getCurrentEra() + 1);
		}
	}
	else iRisk += (GC.getGame().getCurrentEra() + 1);
	// iRisk is currently something like 10 aka 10%

	// Ranges from 10000 to 1,000,000 as attacker:defender culture % ratio goes from 1:1 to 1:0.01
	// Nonlinear!
	const int iCultureRatioModifier = 10000 * iAttackerPercent100 / std::max(1, iDefenderPercent100);
	// XML to make this even stronker (default 200 doubles *only mod part* of above modifier)
	iRisk *= ((iCultureRatioModifier - 10000) * (GC.getREVOLT_TOTAL_CULTURE_MODIFIER()) / 100 + 10000);
	iRisk /= 100;
	// iRisk is now measured in x100 here aka 505 or 2345 meaning 5.05% or 23.45%.

	// By default (100), attacker having a state religion doubles attacking power
	if (GET_PLAYER(eCultureAttacker).getStateReligion() != NO_RELIGION)
	{
		if (isHasReligion(GET_PLAYER(eCultureAttacker).getStateReligion()))
		{
			iRisk *= std::max(0, (GC.getREVOLT_OFFENSE_STATE_RELIGION_MODIFIER() + 100));
			iRisk /= 100;
		}
	}

	// By default (-50), defender having state religion halves attacker's power
	if (GET_PLAYER(getOwner()).getStateReligion() != NO_RELIGION)
	{
		if (isHasReligion(GET_PLAYER(getOwner()).getStateReligion()))
		{
			iRisk *= std::max(0, (GC.getREVOLT_DEFENSE_STATE_RELIGION_MODIFIER() + 100));
			iRisk /= 100;
		}
	}
	return iRisk;
}


int CvCity::unitRevoltRiskModifier(PlayerTypes eCultureAttacker) const
{
	PROFILE_EXTRA_FUNC();
	// constructed from icultureGarrison of stationed units
	// returns percent modifier on revolt risk due to units
	int iGarrison = 0;

	foreach_ (const CvUnit * unit, plot()->units())
		iGarrison += unit->revoltProtectionTotal();

	// Blaze: This also doubles negative impact of criminal units while at war. Intended? Fix?
	if (atWar(GET_PLAYER(eCultureAttacker).getTeam(), getTeam()))
		iGarrison *= 2;

	// Negative revolt protection increases multiplier (-5% revolt protection -> 105% multiplier)
	if (iGarrison < 0) return 100 - iGarrison;
	// Positive revolt protection has diminishing returns
	// 100% protection -> 50% multiplier, 200% protection -> 33% multiplier
	return (10000 / (100 + iGarrison));
}


bool CvCity::hasActiveWorldWonder() const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const BuildingTypes eType, getHasBuildings())
	{
		if (isWorldWonder(eType) && !isDisabledBuilding(eType))
		{
			return true;
		}
	}
	return false;
}


int CvCity::getNumActiveWorldWonders() const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	foreach_(const BuildingTypes eType, getHasBuildings())
	{
		if (isWorldWonder(eType) && !isDisabledBuilding(eType))
		{
			iCount++;
		}
	}
	return iCount;
}


int CvCity::getReligionCount() const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if (isHasReligion((ReligionTypes)iI))
		{
			iCount++;
		}
	}
	return iCount;
}

int CvCity::getCorporationCount() const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (isHasCorporation((CorporationTypes)iI))
		{
			iCount++;
		}
	}
	return iCount;
}


int CvCity::getID() const
{
	return m_iID;
}


int CvCity::getIndex() const
{
	return (getID() & FLTA_INDEX_MASK);
}


IDInfo CvCity::getIDInfo() const
{
	IDInfo city(getOwner(), getID());
	return city;
}


void CvCity::setID(int iID)
{
	m_iID = iID;
}


int CvCity::getViewportX() const
{
	const CvViewport* pCurrentViewPort = GC.getCurrentViewport();
	FAssert(pCurrentViewPort != NULL);
	FAssert(isInViewport());

	return pCurrentViewPort->getViewportXFromMapX(m_iX);
}


int CvCity::getViewportY() const
{
	const CvViewport* pCurrentViewPort = GC.getCurrentViewport();
	FAssert(pCurrentViewPort != NULL);
	FAssert(isInViewport());

	return pCurrentViewPort->getViewportYFromMapY(m_iY);
}

bool CvCity::isInViewport() const
{
	return GC.getCurrentViewport()->isInViewport(m_iX, m_iY);
}


bool CvCity::at(int iX, int iY) const
{
	return ((getX() == iX) && (getY() == iY));
}


bool CvCity::at(const CvPlot* pPlot) const
{
	return (plot() == pPlot);
}


CvPlot* CvCity::plot() const
{
	return GC.getMap().plotSorenINLINE(getX(), getY());
}


/*DllExport*/ CvPlot* CvCity::plotExternal() const
{
#ifdef _DEBUG
	OutputDebugString("exe is asking for the plot of this city\n");
#endif
	FAssert(isInViewport());
	return GC.getMap().plotSorenINLINE(getX(), getY());
}


CvPlotGroup* CvCity::plotGroup(const PlayerTypes ePlayer) const
{
	return plot()->getPlotGroup(ePlayer);
}


bool CvCity::isConnectedTo(const CvCity* pCity) const
{
	return plot()->isConnectedTo(pCity);
}


bool CvCity::isConnectedToCapital(const PlayerTypes ePlayer) const
{
	return plot()->isConnectedToCapital(ePlayer);
}


int CvCity::getArea() const
{
	return plot()->getArea();
}

CvArea* CvCity::area() const
{
	return plot()->area();
}


CvArea* CvCity::waterArea(const bool bNoImpassable) const
{
	return plot()->waterArea(bNoImpassable);
}

// Expose plot function through city
CvArea* CvCity::secondWaterArea() const
{
	return plot()->secondWaterArea();
}

// Find the largest water area shared by this city and other city, if any
CvArea* CvCity::sharedWaterArea(const CvCity* pOtherCity) const
{
	CvArea* pWaterArea = waterArea(true);
	if (pWaterArea)
	{
		CvArea* pOtherWaterArea = pOtherCity->waterArea(true);
		if (pOtherWaterArea)
		{
			if (pWaterArea == pOtherWaterArea)
			{
				return pWaterArea;
			}
			CvArea* pSecondWaterArea = secondWaterArea();

			if (pSecondWaterArea && pSecondWaterArea == pOtherWaterArea)
			{
				return pSecondWaterArea;
			}
			CvArea* pOtherSecondWaterArea = pOtherCity->secondWaterArea();

			if (pOtherSecondWaterArea && pWaterArea == pOtherSecondWaterArea)
			{
				return pWaterArea;
			}

			if (pSecondWaterArea && pOtherSecondWaterArea && pSecondWaterArea == pOtherSecondWaterArea)
			{
				return pSecondWaterArea;
			}
		}
	}
	return NULL;
}

bool CvCity::isBlockaded() const
{
	return algo::any_of(plot()->adjacent(), CvPlot::fn::getBlockadedCount(getTeam()) > 0);
}


CvPlot* CvCity::getRallyPlot() const
{
	return GC.getMap().plotSorenINLINE(m_iRallyX, m_iRallyY);
}


void CvCity::setRallyPlot(const CvPlot* pPlot)
{
	if (getRallyPlot() != pPlot)
	{
		if (pPlot)
		{
			m_iRallyX = pPlot->getX();
			m_iRallyY = pPlot->getY();
		}
		else
		{
			m_iRallyX = INVALID_PLOT_COORD;
			m_iRallyY = INVALID_PLOT_COORD;
		}

		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
		}
	}
}


int CvCity::getGameTurnFounded(const bool bACalendar) const
{
	if (bACalendar) // Accurate Calendar
	{
		return decodeACTurn(m_iGameTurnFounded);
	}
	return m_iGameTurnFounded;
}


void CvCity::setGameTurnFounded(const int iNewValue, const bool bHistoricalCalendar)
{
	if (bHistoricalCalendar)
	{
		CvDate& turnDate = GC.getGame().getCurrentDate();
		int encodeddate = encodeACDateturn(turnDate.getYear(), iNewValue);
		m_iGameTurnFounded = encodeddate;
		//Calvitix (store the year directly, as the turn=>Date is dynamic with Accurate Calendar)

		GC.getMap().updateWorkingCity();
	}
	else
	{
		if (getGameTurnFounded() != iNewValue)
		{
			m_iGameTurnFounded = iNewValue;
			FASSERT_NOT_NEGATIVE(getGameTurnFounded());

			GC.getMap().updateWorkingCity();
		}
	}
}

int CvCity::getGameDateFounded(const bool bACalendar) const
{
	if (bACalendar) // Accurate Calendar
	{
		return decodeACDate(m_iGameTurnFounded);
	}
	return m_iGameTurnFounded;
}


void CvCity::setGameDateFounded(const int iNewValue, const bool bHistoricalCalendar)
{
	if (bHistoricalCalendar)
	{
		CvDate& turnDate = GC.getGame().getCurrentDate();
		//Calvitix (store the year directly, as the turn=>Date is dynamic with Accurate Calendar)
		m_iGameTurnFounded = encodeACDateturn(turnDate.getYear(), iNewValue);

		GC.getMap().updateWorkingCity();
	}
	else
	{
		if (getGameTurnFounded() != iNewValue)
		{
			m_iGameTurnFounded = iNewValue;
			FASSERT_NOT_NEGATIVE(getGameTurnFounded());

			GC.getMap().updateWorkingCity();
		}
	}
}

int CvCity::getGameTurnAcquired(const bool bHistoricalCalendar) const
{
	if (bHistoricalCalendar) // Accurate Calendar
	{
		return decodeACTurn(m_iGameTurnAcquired);
	}
	return m_iGameTurnAcquired;
}

int CvCity::getGameDateAcquired(const bool bHistoricalCalendar) const
{
	if (bHistoricalCalendar) // Accurate Calendar
	{
		return decodeACDate(m_iGameTurnAcquired);
	}
	return m_iGameTurnAcquired;
}


void CvCity::setGameTurnAcquired(const int iNewValue, const bool bHistoricalCalendar)
{
	if (bHistoricalCalendar)
	{
		CvDate& turnDate = GC.getGame().getCurrentDate();
		m_iGameTurnAcquired = iNewValue;
		//Calvitix (store the year directly, and encode the Date wit hwiseBit)
		m_iGameTurnAcquired = encodeACDateturn(turnDate.getYear(), iNewValue);

	}
	else
	{
		m_iGameTurnAcquired = iNewValue;
		FASSERT_NOT_NEGATIVE(getGameTurnAcquired());
	}
}


int CvCity::getPopulation() const
{
	return m_iPopulation;
}


void CvCity::setPopulation(int iNewValue, bool bNormal)
{
	const int iOldPopulation = getPopulation();

	if (iOldPopulation == iNewValue)
	{
		return;
	}
	m_iPopulation = iNewValue;

	FASSERT_NOT_NEGATIVE(iNewValue);

	CvPlayer& owner = GET_PLAYER(getOwner());
	owner.invalidatePopulationRankCache();

	if (iNewValue > getHighestPopulation())
	{
		setHighestPopulation(iNewValue);
	}

	area()->changePopulationPerPlayer(getOwner(), (iNewValue - iOldPopulation));
	owner.changeTotalPopulation(iNewValue - iOldPopulation);
	GET_TEAM(getTeam()).changeTotalPopulation(iNewValue - iOldPopulation);
	GC.getGame().changeTotalPopulation(iNewValue - iOldPopulation);

	if (iNewValue > 0)
	{
		if (plot()->getFeatureType() != NO_FEATURE)
		{
			const int iPopDestroys = GC.getFeatureInfo(plot()->getFeatureType()).getPopDestroys();
			if (iPopDestroys > -1 && iNewValue >= iPopDestroys)
			{
				plot()->setFeatureType(NO_FEATURE);
			}
		}
		updateFeatureHealth();
		if (bNormal)
		{
			checkBuildings();
		}

		if (
			!isHuman()
		&&
			(
				iOldPopulation == 1 && iNewValue > 1
				||
				iNewValue == 1 && iOldPopulation > 1
				||
				iNewValue > iOldPopulation && owner.getNumCities() <= 2
			)
		) AI_setChooseProductionDirty(true);
	}
	plot()->updateYield();
	owner.setMaintenanceDirty(true);

	owner.AI_makeAssignWorkDirty();

	setInfoDirty(true);
	setLayoutDirty(true);

	plot()->plotAction(PUF_makeInfoBarDirty);

	if (isCitySelected())
	{
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	}
}


void CvCity::changePopulation(int iChange)
{
	setPopulation(getPopulation() + iChange);
}


int64_t CvCity::getRealPopulation() const
{
	//return (((long)(pow((float)getPopulation(), 2.8f))) * 1000);

	//	Koshling - using table provided by Praetyre to give more realistic results
	static int realPopulationTable[] =
	{
		0,
		50,
		100,
		250,
		500,
		750,
		1000,
		1500,
		2000,
		2500,
		3000,
		4000,
		5000,
		7500,
		10000,
		15000,
		20000,
		30000,
		40000,
		50000,
		60000,
		70000,
		80000,
		90000,
		100000,
		125000,
		150000,
		175000,
		200000,
		225000,
		250000,
		300000,
		350000,
		400000,
		450000,
		500000,
		550000,
		600000,
		650000,
		700000,
		750000,
		800000,
		850000,
		900000,
		950000,
		1000000,
		1100000,
		1200000,
		1300000,
		1400000,
		1500000,
		1600000,
		1700000,
		1800000,
		1900000,
		2000000,
		2250000,
		2500000,
		2750000,
		3000000,
		3500000,
		4000000,
		5000000,
		6000000,
		7000000,
		8000000,
		9000000,
		10000000,
		12000000,
		14000000,
		16000000,
		18000000,
		20000000,
		22250000,
		25000000,
		27500000,
		30000000,
		35000000,
		40000000,
		45000000,
		50000000,
		55000000,
		60000000,
		65000000,
		70000000,
		75000000,
		80000000,
		85000000,
		90000000,
		95000000,
		100000000,
		110000000,
		120000000,
		130000000,
		140000000,
		150000000,
		160000000,
		170000000,
		180000000,
		190000000,
		200000000
	};
#define NUM_POP_TABLE_ENTRIES (sizeof(realPopulationTable)/sizeof(int))

	if (getPopulation() < NUM_POP_TABLE_ENTRIES)
	{
		return realPopulationTable[getPopulation()];
	}
	return realPopulationTable[NUM_POP_TABLE_ENTRIES - 1] + ((realPopulationTable[NUM_POP_TABLE_ENTRIES - 1] - realPopulationTable[NUM_POP_TABLE_ENTRIES - 2]) * (getPopulation() - NUM_POP_TABLE_ENTRIES));
}

int CvCity::getHighestPopulation() const
{
	return m_iHighestPopulation;
}


void CvCity::setHighestPopulation(int iNewValue)
{
	m_iHighestPopulation = iNewValue;
	FASSERT_NOT_NEGATIVE(getHighestPopulation());
}


int CvCity::getWorkingPopulation() const
{
	return m_iWorkingPopulation;
}


void CvCity::changeWorkingPopulation(int iChange)
{
	m_iWorkingPopulation += iChange;
	FASSERT_NOT_NEGATIVE(m_iWorkingPopulation);
}


int CvCity::getSpecialistPopulation() const
{
	return m_iSpecialistPopulation;
}


void CvCity::changeSpecialistPopulation(int iChange)
{
	if (iChange != 0)
	{
		m_iSpecialistPopulation += iChange;
		FASSERT_NOT_NEGATIVE(m_iSpecialistPopulation);

		GET_PLAYER(getOwner()).invalidateYieldRankCache();

		setCommerceDirty(NO_COMMERCE);
	}
}


int CvCity::getNumGreatPeople() const
{
	return m_iNumGreatPeople;
}


void CvCity::changeNumGreatPeople(int iChange)
{
	if (iChange != 0)
	{
		m_iNumGreatPeople += iChange;
		FASSERT_NOT_NEGATIVE(m_iNumGreatPeople);

		setCommerceDirty(NO_COMMERCE);
	}
}


int CvCity::getBaseGreatPeopleRate() const
{
	return std::max(0, m_iBaseGreatPeopleRate) + GET_PLAYER(getOwner()).getNationalGreatPeopleRate();
}


int CvCity::getGreatPeopleRate() const
{
	if (isDisorder())
	{
		return 0;
	}
	return getBaseGreatPeopleRate() * getTotalGreatPeopleRateModifier() / 100;
}


int CvCity::getTotalGreatPeopleRateModifier() const
{
	const CvPlayer& owner = GET_PLAYER(getOwner());

	int iModifier = 100 + getGreatPeopleRateModifier() + owner.getGreatPeopleRateModifier();

	if (owner.getStateReligion() != NO_RELIGION && isHasReligion(owner.getStateReligion()))
	{
		iModifier += owner.getStateReligionGreatPeopleRateModifier();
	}

	if (owner.isGoldenAge())
	{
		iModifier += GC.getGOLDEN_AGE_GREAT_PEOPLE_MODIFIER();
	}

	return std::max(0, iModifier);
}


void CvCity::changeBaseGreatPeopleRate(int iChange)
{
	m_iBaseGreatPeopleRate += iChange;
}


int CvCity::getGreatPeopleRateModifier() const
{
	return m_iGreatPeopleRateModifier;
}


void CvCity::changeGreatPeopleRateModifier(int iChange)
{
	m_iGreatPeopleRateModifier += iChange;
}


int CvCity::getGreatPeopleProgress() const
{
	return m_iGreatPeopleProgress;
}


void CvCity::changeGreatPeopleProgress(int iChange)
{
	m_iGreatPeopleProgress += iChange;
	FAssertMsg(getGreatPeopleProgress() >= 0, CvString::format("City %S m_iGreatPeopleProgress is %d", m_szName.c_str(), m_iGreatPeopleProgress).c_str());
}

CvProperties* CvCity::getProperties()
{
	return &m_Properties;
}

const CvProperties* CvCity::getPropertiesConst() const
{
	return &m_Properties;
}


// BUG - Building Additional Great People - start
/*
 * Returns the total additional great people rate that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalGreatPeopleRateByBuilding(BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eBuilding);

	const int iRate = getBaseGreatPeopleRate();
	const int iModifier = getTotalGreatPeopleRateModifier();
	int iExtra =
	(
		(iRate + getAdditionalBaseGreatPeopleRateByBuilding(eBuilding))
		*
		(iModifier + getAdditionalGreatPeopleRateModifierByBuilding(eBuilding))
		/
		100
		-
		iRate * iModifier / 100
	);
	const CvBuildingInfo& building = GC.getBuildingInfo(eBuilding);

	for (int iI = 0; iI < building.getNumReplacedBuilding(); iI++)
	{
		const BuildingTypes eBuildingX = static_cast<BuildingTypes>(building.getReplacedBuilding(iI));

		if (hasFullyActiveBuilding(eBuildingX))
		{
			iExtra -= getAdditionalGreatPeopleRateByBuilding(eBuildingX);
		}
	}
	return iExtra;
}

/*
 * Returns the additional great people rate that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalBaseGreatPeopleRateByBuilding(BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eBuilding);

	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	int iExtraRate = kBuilding.getGreatPeopleRateChange();

	// Specialists
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); ++iI)
	{
		if (kBuilding.getFreeSpecialistCount((SpecialistTypes)iI) != 0)
		{
			iExtraRate += getAdditionalBaseGreatPeopleRateBySpecialist((SpecialistTypes)iI, kBuilding.getFreeSpecialistCount((SpecialistTypes)iI));
		}
	}

	for (int iI = 1; iI < kBuilding.getFreeSpecialist() + 1; iI++)
	{
		const SpecialistTypes eNewSpecialist = getBestSpecialist(iI);
		if (eNewSpecialist == NO_SPECIALIST) break;

		iExtraRate += GC.getSpecialistInfo(eNewSpecialist).getGreatPeopleRateChange();

	}

	if (kBuilding.getNumPopulationEmployed() > 0)
	{
		int* paiCommerce = new int[NUM_COMMERCE_TYPES];
		int* paiYield = new int[NUM_YIELD_TYPES];
		int iGreatPeopleRate;
		int iHappiness;
		int iHealthiness;
		removeWorstCitizenActualEffects(kBuilding.getNumPopulationEmployed(), iGreatPeopleRate, iHappiness, iHealthiness, paiYield, paiCommerce);
		SAFE_DELETE_ARRAY(paiCommerce);
		SAFE_DELETE_ARRAY(paiYield);
		iExtraRate += iGreatPeopleRate;
	}

	return iExtraRate;
}

/*
 * Returns the additional great people rate modifier that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalGreatPeopleRateModifierByBuilding(BuildingTypes eBuilding) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eBuilding);

	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	return kBuilding.getGreatPeopleRateModifier() + kBuilding.getGlobalGreatPeopleRateModifier();
}


// BUG - Specialist Additional Great People - start
/*
 * Returns the total additional great people rate that changing the number of the given specialist will provide/remove.
 */
int CvCity::getAdditionalGreatPeopleRateBySpecialist(SpecialistTypes eSpecialist, int iChange) const
{
	const int iRate = getBaseGreatPeopleRate();
	const int iModifier = getTotalGreatPeopleRateModifier();
	const int iExtraRate = getAdditionalBaseGreatPeopleRateBySpecialist(eSpecialist, iChange);

	const int iExtra = ((iRate + iExtraRate) * iModifier / 100) - (iRate * iModifier / 100);

	return iExtra;
}

/*
 * Returns the additional great people rate that changing the number of the given specialist will provide/remove.
 */
int CvCity::getAdditionalBaseGreatPeopleRateBySpecialist(SpecialistTypes eSpecialist, int iChange) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eSpecialist);

	return iChange * GC.getSpecialistInfo(eSpecialist).getGreatPeopleRateChange();
}
// BUG - Specialist Additional Great People - end


int CvCity::getNumWorldWonders() const
{
	return m_iNumWorldWonders;
}


void CvCity::changeNumWorldWonders(int iChange)
{
	m_iNumWorldWonders += iChange;
	FASSERT_NOT_NEGATIVE(getNumWorldWonders());
}


int CvCity::getNumTeamWonders() const
{
	return m_iNumTeamWonders;
}


void CvCity::changeNumTeamWonders(int iChange)
{
	m_iNumTeamWonders += iChange;
	FASSERT_NOT_NEGATIVE(getNumTeamWonders());
}


int CvCity::getNumNationalWonders() const
{
	return m_iNumNationalWonders;
}


void CvCity::changeNumNationalWonders(int iChange)
{
	m_iNumNationalWonders += iChange;
	FASSERT_NOT_NEGATIVE(getNumNationalWonders());
}


int CvCity::getNumBuildings() const
{
	return m_iNumBuildings;
}


void CvCity::changeNumBuildings(int iChange)
{
	m_iNumBuildings += iChange;
	FASSERT_NOT_NEGATIVE(getNumBuildings());
}


bool CvCity::isGovernmentCenter() const
{
	return m_iGovernmentCenterCount > 0;
}

void CvCity::changeGovernmentCenterCount(int iChange)
{
	if (iChange != 0)
	{
		const bool bGovernmentCenter = m_iGovernmentCenterCount > 0;
		m_iGovernmentCenterCount += iChange;
		FASSERT_NOT_NEGATIVE(m_iGovernmentCenterCount);

		if (bGovernmentCenter != isGovernmentCenter())
		{
			GET_PLAYER(getOwner()).setMaintenanceDirty(true);
		}
	}
}


// BUG - Building Saved Maintenance - start
/*
 * Returns the rounded total additional gold from saved maintenance that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getSavedMaintenanceByBuilding(BuildingTypes eBuilding) const
{
	return getSavedMaintenanceTimes100ByBuilding(eBuilding) / 100;
}

/*
 * Returns the total additional gold from saved maintenance times 100 that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getSavedMaintenanceTimes100ByBuilding(BuildingTypes eBuilding) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eBuilding);

	if (isDisorder() || isWeLoveTheKingDay())
	{
		return 0;
	}
	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	const int iModifier =
	(
		kBuilding.getMaintenanceModifier() +
		kBuilding.getGlobalMaintenanceModifier() +
		kBuilding.getAreaMaintenanceModifier() +
		(
			(isConnectedToCapital() && !isCapital())
			?
			kBuilding.getConnectedCityMaintenanceModifier() : 0
		)
	);
	const int iDirectMaintenance =
	(
		(kBuilding.getCommerceChange(COMMERCE_GOLD) < 0 && GC.getTREAT_NEGATIVE_GOLD_AS_MAINTENANCE())
		?
		-kBuilding.getCommerceChange(COMMERCE_GOLD) * 100 : 0
	);
	if (iModifier == 0 && iDirectMaintenance == 0)
	{
		return 0;
	}
	const int iModOld = getEffectiveMaintenanceModifier();
	const int iBaseOld = calculateBaseMaintenanceTimes100();

	return getModifiedIntValue(iBaseOld, iModOld) - getModifiedIntValue(iBaseOld + iDirectMaintenance, iModOld + iModifier);
}
// BUG - Building Saved Maintenance - end

int CvCity::getDistanceMaintenanceSavedTimes100ByCivic(CivicTypes eCivic) const
{
	FASSERT_BOUNDS(0, GC.getNumCivicInfos(), eCivic);

	if (isDisorder() || isWeLoveTheKingDay())
	{
		return 0;
	}
	const int iMod = GC.getCivicInfo(eCivic).getDistanceMaintenanceModifier();

	if (iMod == 0)
	{
		return 0;
	}
	const int iFinalMod = getEffectiveMaintenanceModifier();

	return (
		getModifiedIntValue(calculateDistanceMaintenanceTimes100(), iFinalMod)
		-
		getModifiedIntValue(calculateDistanceMaintenanceTimes100(iMod), iFinalMod)
	);
}

int CvCity::getNumCitiesMaintenanceSavedTimes100ByCivic(CivicTypes eCivic) const
{
	FASSERT_BOUNDS(0, GC.getNumCivicInfos(), eCivic);
	if (isDisorder() || isWeLoveTheKingDay())
	{
		return 0;
	}
	const int iMod = GC.getCivicInfo(eCivic).getNumCitiesMaintenanceModifier();

	if (iMod == 0)
	{
		return 0;
	}
	const int iFinalMod = getEffectiveMaintenanceModifier();

	return (
		getModifiedIntValue(calculateNumCitiesMaintenanceTimes100(), iFinalMod)
		-
		getModifiedIntValue(calculateNumCitiesMaintenanceTimes100(iMod), iFinalMod)
	);
}

int CvCity::getHomeAreaMaintenanceSavedTimes100ByCivic(CivicTypes eCivic) const
{
	FASSERT_BOUNDS(0, GC.getNumCivicInfos(), eCivic);

	if (isDisorder() || isWeLoveTheKingDay() || !area()->isHomeArea(getOwner()))
	{
		return 0;
	}
	const int iMod = GC.getCivicInfo(eCivic).getHomeAreaMaintenanceModifier();

	if (iMod == 0)
	{
		return 0;
	}
	const int iBaseMaintenance = calculateBaseMaintenanceTimes100();
	const int iEffectiveMaintenanceMod = getEffectiveMaintenanceModifier();
	return // The difference
	(
		getModifiedIntValue(iBaseMaintenance, iEffectiveMaintenanceMod)
		-
		getModifiedIntValue(iBaseMaintenance, iEffectiveMaintenanceMod + iMod)
	);
}

int CvCity::getOtherAreaMaintenanceSavedTimes100ByCivic(CivicTypes eCivic) const
{
	FASSERT_BOUNDS(0, GC.getNumCivicInfos(), eCivic);

	if (isDisorder() || isWeLoveTheKingDay() || area()->isHomeArea(getOwner()))
	{
		return 0;
	}
	const int iMod = GC.getCivicInfo(eCivic).getOtherAreaMaintenanceModifier();

	if (iMod == 0)
	{
		return 0;
	}
	const int iBaseMaintenance = calculateBaseMaintenanceTimes100();
	const int iEffectiveMaintenanceMod = getEffectiveMaintenanceModifier();
	return // The difference
	(
		getModifiedIntValue(iBaseMaintenance, iEffectiveMaintenanceMod)
		-
		getModifiedIntValue(iBaseMaintenance, iEffectiveMaintenanceMod + iMod)
	);
}

int CvCity::getMaintenance() const
{
	if (m_bMaintenanceDirty)
	{
		updateMaintenance();
	}
	return m_iMaintenance / 100;
}

int CvCity::getMaintenanceTimes100() const
{
	if (m_bMaintenanceDirty)
	{
		updateMaintenance();
	}
	return m_iMaintenance;
}

int CvCity::getEffectiveMaintenanceModifier() const
{
	int iModifier = getMaintenanceModifier() + GET_PLAYER(getOwner()).getMaintenanceModifier() + area()->getTotalAreaMaintenanceModifier(getOwner());

	if (isConnectedToCapital() && !isCapital())
	{
		iModifier += GET_PLAYER(getOwner()).getConnectedCityMaintenanceModifier();
	}
	return iModifier;
}

void CvCity::setMaintenanceDirty(const bool bDirty, const bool bPlayer) const
{
	m_bMaintenanceDirty = bDirty;

	if (bPlayer)
	{
		GET_PLAYER(getOwner()).setMaintenanceDirty(bDirty, false);
	}
}

void CvCity::updateMaintenance() const
{
	m_bMaintenanceDirty = false;

	int iNewMaintenance = GC.getEraInfo(GET_PLAYER(getOwner()).getCurrentEra()).getInitialCityMaintenancePercent();
	if (!isDisorder() && !isWeLoveTheKingDay() && getPopulation() > 0)
	{
		iNewMaintenance += getModifiedIntValue(calculateBaseMaintenanceTimes100(), getEffectiveMaintenanceModifier());
	}

	if (m_iMaintenance != iNewMaintenance)
	{
		FASSERT_NOT_NEGATIVE(iNewMaintenance);

		m_iMaintenance = iNewMaintenance;
	}
}

int CvCity::calculateDistanceMaintenance() const
{
	return calculateDistanceMaintenanceTimes100() / 100;
}

int CvCity::calculateDistanceMaintenanceTimes100(int iExtraDistanceModifier, int iExtraCoastalDistanceModifier) const
{
	PROFILE_EXTRA_FUNC();
	if (isGovernmentCenter())
	{
		return 0;
	}
	const CvPlayer& kOwner = GET_PLAYER(getOwner());

	int iDistMaint = MAX_INT;
	bool bFound = false;
	foreach_(const CvCity* pLoopCity, kOwner.cities())
	{
		if (pLoopCity != this && pLoopCity->isGovernmentCenter())
		{
			const int iDistance = plotDistance(getX(), getY(), pLoopCity->getX(), pLoopCity->getY());

			int iValue = GC.getBASE_DISTANCE_MAINTENANCE_PER_100_PLOTS() * (iDistance + iDistance / 3);

			iValue *= getPopulation() + 5;
			iValue /= 10;

			if (isCoastal(GC.getWorldInfo(GC.getMap().getWorldSize()).getOceanMinAreaSize()))
			{
				iValue = getModifiedIntValue(
					iValue,
					kOwner.getDistanceMaintenanceModifier() + iExtraDistanceModifier
					+
					kOwner.getCoastalDistanceMaintenanceModifier() + iExtraCoastalDistanceModifier
				);
			}
			else
			{
				iValue = getModifiedIntValue(iValue, kOwner.getDistanceMaintenanceModifier() + iExtraDistanceModifier);
			}

			// Toffer: Is this scaling rational?
			// It may be more probable that players would settle cities further away from capital on bigger
			// maps than they normally would on a smaller one even if closer alternatives are possible.
			// So maybe a small discount on bigger maps makes sense, if just to give the players a lttle more
			// freedom in where to settle as there's more possible locations to consider.
			iValue *= GC.getWorldInfo(GC.getMap().getWorldSize()).getDistanceMaintenancePercent();
			iValue /= 100;
			// !Toffer

			iValue *= GC.getHandicapInfo(getHandicapType()).getDistanceMaintenancePercent();
			iValue /= 100;

			iValue = std::max(0, iValue);

			iDistMaint = std::min(iDistMaint, iValue);
			bFound = true;
		}
	}
	// Rebels pay less maintenance
	return bFound ? iDistMaint / (1 + kOwner.isRebel()) : 0;
}

int CvCity::calculateNumCitiesMaintenance() const
{
	return calculateNumCitiesMaintenanceTimes100() / 100;
}

int CvCity::calculateNumCitiesMaintenanceTimes100(int iExtraModifier) const
{
	PROFILE_EXTRA_FUNC();
	// Toffer: Single city civs gets a free pass, and allows early second city founding without economical collapse.
	const CvPlayer& owner = GET_PLAYER(getOwner());
	const int iCities = owner.getNumCities() - 1;
	if (iCities < 1)
	{
		return 0;
	}

	// Toffer - ToDo: Add global define called BASE_NUM_CITY_MAINTENANCE_PERCENT
	int iNumCitiesPercent = 72;

	iNumCitiesPercent *= 13 + getPopulation();
	iNumCitiesPercent /= 13;

	/* Toffer - Skews early game balance too much between map sizes.
	Large maps have a discount on distance maintenance, that is adequate, this doesn't skew early game very much as you settle close to capital anyway.
	iNumCitiesPercent *= GC.getWorldInfo(GC.getMap().getWorldSize()).getNumCitiesMaintenancePercent();
	iNumCitiesPercent /= 100;
	*/
	int iNumCitiesMaint = iCities * iNumCitiesPercent;

	int iNumVassalCities = 0;
	int iVassals = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getTeam(), false)
		&& GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isVassal(getTeam()))
		{
			iNumVassalCities += GET_PLAYER((PlayerTypes)iI).getNumCities();
			iVassals++;
		}
	}
	if (iNumVassalCities > 0)
	{
		iNumCitiesMaint += iNumVassalCities * iNumCitiesPercent / (3 + iVassals);
	}

	iNumCitiesMaint = getModifiedIntValue(iNumCitiesMaint, owner.getNumCitiesMaintenanceModifier() + iExtraModifier);

	iNumCitiesMaint *= GC.getHandicapInfo(getHandicapType()).getNumCitiesMaintenancePercent();
	iNumCitiesMaint /= 100;

	// Rebels pay less maintenance
	if (owner.isRebel())
	{
		iNumCitiesMaint /= 2;
	}
	// Toffer - ToDo: Max value should perhaps be a global define.
	iNumCitiesMaint = std::min(iNumCitiesMaint, 2000000);
	FASSERT_NOT_NEGATIVE(iNumCitiesMaint);

	return iNumCitiesMaint;
}


int CvCity::calculateColonyMaintenance() const
{
	return (calculateColonyMaintenanceTimes100() / 100);
}

int CvCity::calculateColonyMaintenanceTimes100() const
{
	if (GC.getGame().isOption(GAMEOPTION_NO_VASSAL_STATES))
	{
		return 0;
	}
	const CvPlayer& owner = GET_PLAYER(getOwner());
	const CvCity* pCapital = owner.getCapitalCity();

	if (pCapital && pCapital->area() == area())
	{
		return 0;
	}

	int iNumCitiesPercent = 100;

	iNumCitiesPercent *= 17 + getPopulation();
	iNumCitiesPercent /= 18;

	iNumCitiesPercent *= GC.getWorldInfo(GC.getMap().getWorldSize()).getColonyMaintenancePercent();
	iNumCitiesPercent /= 100;

	iNumCitiesPercent *= GC.getHandicapInfo(getHandicapType()).getColonyMaintenancePercent();
	iNumCitiesPercent /= 100;

	int iNumCities = (area()->getCitiesPerPlayer(getOwner()) - 1) * iNumCitiesPercent;

	int iMaintenance = iNumCities * iNumCities / 100;

	iMaintenance = std::min(iMaintenance, GC.getHandicapInfo(getHandicapType()).getMaxColonyMaintenance() * calculateDistanceMaintenanceTimes100() / 100);

	FASSERT_NOT_NEGATIVE(iMaintenance);

	// Rebels pay less maintenance
	return owner.isRebel() ? iMaintenance / 2 : iMaintenance;
}


int CvCity::calculateCorporationMaintenance() const
{
	return (calculateCorporationMaintenanceTimes100() / 100);
}

int CvCity::calculateCorporationMaintenanceTimes100() const
{
	PROFILE_EXTRA_FUNC();
	int iMaintenance = 0;

	for (int iCorporation = 0; iCorporation < GC.getNumCorporationInfos(); ++iCorporation)
	{
		if (isActiveCorporation((CorporationTypes)iCorporation))
		{
			iMaintenance += calculateCorporationMaintenanceTimes100((CorporationTypes)iCorporation);
		}
	}
	// Rebels pay less maintenance
	return GET_PLAYER(getOwner()).isRebel() ? iMaintenance / 2 : iMaintenance;
}

int CvCity::calculateCorporationMaintenanceTimes100(CorporationTypes eCorporation) const
{
	PROFILE_EXTRA_FUNC();
	int iMaintenance = 0;

	for (int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; ++iCommerce)
	{
		iMaintenance += 100 * GC.getCorporationInfo(eCorporation).getHeadquarterCommerce(iCommerce);
	}

	// Bonus Maintenance
	int iNumBonuses = 0;
	foreach_(const BonusTypes eBonus, GC.getCorporationInfo(eCorporation).getPrereqBonuses())
	{
		iNumBonuses += getNumBonuses(eBonus);
	}
	iMaintenance +=
	(
		GC.getCorporationInfo(eCorporation).getMaintenance() * iNumBonuses
		* GC.getWorldInfo(GC.getMap().getWorldSize()).getCorporationMaintenancePercent()
		/ 100
	);

	// Population factor
	iMaintenance *= 17 + getPopulation();
	iMaintenance /= 18;

	// Handicap
	if (GC.getGame().isOption(GAMEOPTION_ADVANCED_REALISTIC_CORPORATIONS))
	{
		iMaintenance = (
			iMaintenance
			* GC.getHandicapInfo(getHandicapType()).getCorporationMaintenancePercent()
			* GC.getHandicapInfo(getHandicapType()).getCorporationMaintenancePercent()
			/ 8000
		);
	}
	else
	{
		iMaintenance *= GC.getHandicapInfo(getHandicapType()).getCorporationMaintenancePercent();
		iMaintenance /= 100;
	}

	const CvPlayer& owner = GET_PLAYER(getOwner());
	// National Modifier
	iMaintenance = getModifiedIntValue(iMaintenance, owner.getCorporationMaintenanceModifier() + GET_TEAM(getTeam()).getCorporationMaintenanceModifier());

	FASSERT_NOT_NEGATIVE(iMaintenance);

	// Rebels pay less maintenance
	return owner.isRebel() ? iMaintenance / 2 : iMaintenance;
}

int CvCity::calculateBuildingMaintenanceTimes100() const
{
	PROFILE_EXTRA_FUNC();
	if (GC.getTREAT_NEGATIVE_GOLD_AS_MAINTENANCE())
	{
		int iResult = 0;

		foreach_(const BuildingTypes eTypeX, getHasBuildings())
		{
			if (GC.getBuildingInfo(eTypeX).getCommerceChange(COMMERCE_GOLD) < 0
			&& !isReligiouslyLimitedBuilding(eTypeX)
			&& !isDisabledBuilding(eTypeX))
			{
				iResult -= GC.getBuildingInfo(eTypeX).getCommerceChange(COMMERCE_GOLD);
			}
		}
		// Rebels pay less maintenance
		return GET_PLAYER(getOwner()).isRebel() ? 50 * iResult : 100 * iResult;
	}
	return 0;
}

int CvCity::calculateBaseMaintenanceTimes100() const
{
	return (
		calculateBuildingMaintenanceTimes100()
		+
		calculateDistanceMaintenanceTimes100()
		+
		calculateNumCitiesMaintenanceTimes100()
		+
		calculateColonyMaintenanceTimes100()
		+
		calculateCorporationMaintenanceTimes100()
	);
}


int CvCity::getMaintenanceModifier() const
{
	return m_iMaintenanceModifier;
}


void CvCity::changeMaintenanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iMaintenanceModifier += iChange;
		setMaintenanceDirty(true);
	}
}


int CvCity::getWarWearinessModifier() const
{
	return m_iWarWearinessModifier;
}


void CvCity::changeWarWearinessModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iWarWearinessModifier += iChange;

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getHurryAngerModifier() const
{
	int iTotalHurryAngerModifier = m_iHurryAngerModifier;
	iTotalHurryAngerModifier += GET_PLAYER(getOwner()).getNationalHurryAngerModifier();
	// AIAndy: This used to return m_iHurryAngerModifier instead of the calculated iTotalHurryAngerModifier which I don't assume is correct
	return iTotalHurryAngerModifier;
}


void CvCity::changeHurryAngerModifier(int iChange)
{
	if (0 != iChange)
	{
		int iRatio = 0;

		//	Foregiveness for crazily long hurry anger times because its was bugged in older
		//	versions!
		if (m_iHurryAngerTimer > 1000)
		{
			m_iHurryAngerTimer = 0;
		}

		if (m_iHurryAngerTimer > 0 && !GC.getGame().isRecalculatingModifiers())
		{
			iRatio = (100 * (m_iHurryAngerTimer - 1)) / std::max(1, 100 + getHurryAngerModifier());
		}

		m_iHurryAngerModifier += iChange;

		if (m_iHurryAngerTimer > 0 && !GC.getGame().isRecalculatingModifiers())
		{
			m_iHurryAngerTimer = (iRatio * std::max(1, 100 + getHurryAngerModifier())) / 100 + 1;
		}
	}
}


int CvCity::getHealRate() const
{
	return m_iHealRate;
}


void CvCity::changeHealRate(int iChange)
{
	m_iHealRate += iChange;
	FASSERT_NOT_NEGATIVE(getHealRate());
}

int CvCity::getHealUnitCombatTypeTotal(UnitCombatTypes eUnitCombat) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eUnitCombat);

	return m_paiHealUnitCombatTypeVolume[eUnitCombat];
}

void CvCity::changeHealUnitCombatTypeVolume(UnitCombatTypes eUnitCombat, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eUnitCombat);

	m_paiHealUnitCombatTypeVolume[eUnitCombat] += iChange;
}

int CvCity::getEspionageHealthCounter() const
{
	return std::min(8, m_iEspionageHealthCounter);
}

void CvCity::changeEspionageHealthCounter(int iChange)
{
	if (iChange != 0)
	{
		m_iEspionageHealthCounter += iChange;
	}
}

int CvCity::getEspionageHappinessCounter() const
{
	return std::min(8, m_iEspionageHappinessCounter);
}


void CvCity::changeEspionageHappinessCounter(int iChange)
{
	if (iChange != 0)
	{
		m_iEspionageHappinessCounter += iChange;
	}
}


int CvCity::getFreshWaterGoodHealth() const
{
	return m_iFreshWaterGoodHealth;
}


void CvCity::updateFreshWaterHealth()
{
	const int iNewGoodHealth = plot()->isFreshWater() ? GC.getFRESH_WATER_HEALTH_CHANGE() : 0;

	if (getFreshWaterGoodHealth() != iNewGoodHealth)
	{
		m_iFreshWaterGoodHealth = iNewGoodHealth;
		FASSERT_NOT_NEGATIVE(getFreshWaterGoodHealth());

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}


int CvCity::getFeatureGoodHealth() const
{
	return m_iFeatureGoodHealth;
}


int CvCity::getFeatureBadHealth() const
{
	return m_iFeatureBadHealth;
}


// BUG - Feature Health - start
/*
 * Recalculates the total percentage health effects from existing features
 * and updates the values if they have changed.
 *
 * Bad health is stored as a negative value.
 */
void CvCity::updateFeatureHealth()
{
	int iNewGoodHealth = 0;
	int iNewBadHealth = 0;

	calculateFeatureHealthPercent(iNewGoodHealth, iNewBadHealth);
	iNewBadHealth = -iNewBadHealth;  // convert to "negative is bad"

	iNewGoodHealth /= 100;

	// AIAndy: Feature unhealthiness reduced for small cities
	iNewBadHealth *= std::max(std::min(getPopulation() - 2, 5), 0);
	iNewBadHealth /= 500;

	if ((getFeatureGoodHealth() != iNewGoodHealth) || (getFeatureBadHealth() != iNewBadHealth))
	{
		m_iFeatureGoodHealth = iNewGoodHealth;
		m_iFeatureBadHealth = iNewBadHealth;
		FAssert(getFeatureGoodHealth() >= 0);
		FAssert(getFeatureBadHealth() <= 0);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

/*
 * Adds the total percentage health effects from existing features to iGood and iBad.
 *
 * Positive values for iBad mean an increase in unhealthiness.
 */
void CvCity::calculateFeatureHealthPercent(int& iGood, int& iBad) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const CvPlot* loopPlot, plots(NUM_CITY_PLOTS))
	{
		const FeatureTypes eFeature = loopPlot->getFeatureType();

		if (eFeature != NO_FEATURE)
		{
			const int iHealth = GC.getFeatureInfo(eFeature).getHealthPercent();

			if (iHealth > 0)
			{
				iGood += iHealth;
			}
			else
			{
				iBad -= iHealth;
			}
		}
	}
}

namespace {
	bool isBuildingFeatureRemove(const CvUnit* unit, const FeatureTypes eFeature) {
		const BuildTypes eBuild = unit->getBuildType();
		return eBuild != NO_BUILD && GC.getBuildInfo(eBuild).isFeatureRemove(eFeature);
	}

}
/*
 * Subtracts the total percentage health effects of features currently being removed to iGood and iBad.
 * If pIgnorePlot is not NULL, it is not checked for feature removal.
 * Checks only plots visible to this city's owner.
 *
 * Positive values for iBad mean an increase in unhealthiness.
 */
void CvCity::calculateFeatureHealthPercentChange(int& iGood, int& iBad, CvPlot* pIgnorePlot) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const CvPlot* loopPlot, plots(NUM_CITY_PLOTS))
	{
		if (loopPlot == pIgnorePlot || !loopPlot->isVisible(getTeam(), true))
			continue;

		const FeatureTypes eFeature = loopPlot->getFeatureType();

		if (eFeature != NO_FEATURE)
		{
			const int iHealth = GC.getFeatureInfo(eFeature).getHealthPercent();

			if (iHealth != 0)
			{
				if (algo::any_of(loopPlot->units(), bind(isBuildingFeatureRemove, _1, eFeature)))
				{
					if (iHealth > 0)
					{
						iGood += iHealth;
					}
					else
					{
						iBad -= iHealth;
					}
				}
			}
		}
	}
}

/*
 * Returns the total additional health that adding or removing iChange eFeatures will provide.
 */
int CvCity::getAdditionalHealthByFeature(FeatureTypes eFeature, int iChange) const
{
	int iGood = 0, iBad = 0;
	return getAdditionalHealthByFeature(eFeature, iChange, iGood, iBad);
}

/*
 * Returns the total additional health that adding or removing iChange eFeatures will provide
 * and sets the good and bad levels individually.
 *
 * Doesn't reset iGood or iBad to zero.
 * Positive values for iBad mean an increase in unhealthiness.
 */
int CvCity::getAdditionalHealthByFeature(FeatureTypes eFeature, int iChange, int& iGood, int& iBad) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), eFeature);

	const int iHealth = GC.getFeatureInfo(eFeature).getHealthPercent();

	if (iHealth > 0)
	{
		return getAdditionalHealth(iChange * iHealth, 0, iGood, iBad);
	}
	else
	{
		return getAdditionalHealth(0, -iChange * iHealth, iGood, iBad);
	}
}

/*
 * Returns the total additional health that adding or removing a good or bad health percent will provide
 * and sets the good and bad levels individually.
 *
 * Doesn't reset iGood or iBad to zero.
 * Positive values for iBad and iBadPercent mean an increase in unhealthiness.
 */
int CvCity::getAdditionalHealth(int iGoodPercent, int iBadPercent, int& iGood, int& iBad) const
{
	const int iStarting = iGood - iBad;

	// Add current
	calculateFeatureHealthPercent(iGoodPercent, iBadPercent);

	// Delta
	iGood += (iGoodPercent / 100) + getFeatureGoodHealth();
	iBad += (iBadPercent / 100) - getFeatureBadHealth();

	return iGood - iBad - iStarting;
}
// BUG - Feature Health - end

// BUG - Actual Effects - start
/*
 * Returns the additional angry population caused by the given happiness changes.
 *
 * Positive values for iBad mean an increase in unhappiness.
 */
int CvCity::getAdditionalAngryPopuplation(int iGood, int iBad) const
{
	int iHappy = happyLevel();
	int iUnhappy = unhappyLevel();
	int iPop = getPopulation();

	return range((iUnhappy + iBad) - (iHappy + iGood), 0, iPop) - range(iUnhappy - iHappy, 0, iPop);
}

/*
 * Returns the additional spoiled food caused by the given health changes.
 *
 * Positive values for iBad mean an increase in unhealthiness.
 */
int CvCity::getAdditionalSpoiledFood(int iGood, int iBad, int iHealthAdjust) const
{
	int iHealthy = goodHealth();
	int iUnhealthy = badHealth();
	int iRate = iHealthy - iUnhealthy + iHealthAdjust;

	return std::min(0, iRate) - std::min(0, iRate + iGood - iBad);
}

/*
 * Returns the additional starvation caused by the given spoiled food.
 */
int CvCity::getAdditionalStarvation(int iSpoiledFood, int iFoodAdjust) const
{
	int iFood = getYieldRate(YIELD_FOOD) - foodConsumption() + iFoodAdjust;

	if (iSpoiledFood > 0)
	{
		if (iFood <= 0)
		{
			return iSpoiledFood;
		}
		else if (iSpoiledFood > iFood)
		{
			return iSpoiledFood - iFood;
		}
	}
	else if (iSpoiledFood < 0)
	{
		if (iFood < 0)
		{
			return std::max(iFood, iSpoiledFood);
		}
	}

	return 0;
}
// BUG - Actual Effects - start


int CvCity::getBuildingGoodHealth() const
{
	return m_iBuildingGoodHealth;
}

int CvCity::getBuildingBadHealth() const
{
	return m_iBuildingBadHealth;
}

int CvCity::getBuildingHealth(BuildingTypes eBuilding) const
{
	int iHealth = getBuildingGoodHealth(eBuilding);

	if (!isBuildingOnlyHealthy())
	{
		iHealth += getBuildingBadHealth(eBuilding);
	}

	return iHealth;
}

int CvCity::getBuildingGoodHealth(BuildingTypes eBuilding) const
{
	int iHealth = std::max(0, GC.getBuildingInfo(eBuilding).getHealth());
	iHealth += std::max(0, getBuildingHealthChange(eBuilding));
	iHealth += std::max(0, GET_PLAYER(getOwner()).getExtraBuildingHealth(eBuilding));
	iHealth += std::max(0, (GC.getBuildingInfo(eBuilding).getHealthPercentPerPopulation() * getPopulation()) / 100);

	return iHealth;
}

int CvCity::getBuildingBadHealth(BuildingTypes eBuilding) const
{
	if (isBuildingOnlyHealthy())
	{
		return 0;
	}

	int iHealth = std::min(0, GC.getBuildingInfo(eBuilding).getHealth());
	iHealth += std::min(0, getBuildingHealthChange(eBuilding));
	iHealth += std::min(0, GET_PLAYER(getOwner()).getExtraBuildingHealth(eBuilding));
	iHealth += std::min(0, (GC.getBuildingInfo(eBuilding).getHealthPercentPerPopulation() * getPopulation()) / 100);

	return iHealth;
}

void CvCity::changeBuildingGoodHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingGoodHealth += iChange;
		FASSERT_NOT_NEGATIVE(getBuildingGoodHealth());

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

void CvCity::changeBuildingBadHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingBadHealth += iChange;
		FAssert(getBuildingBadHealth() <= 0);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}


int CvCity::getBonusGoodHealth() const
{
	return m_iBonusGoodHealth;
}

int CvCity::getBonusBadHealth() const
{
	return m_iBonusBadHealth;
}

void CvCity::changeBonusGoodHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iBonusGoodHealth += iChange;
		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

void CvCity::changeBonusBadHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iBonusBadHealth += iChange;
		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}


int CvCity::getMilitaryHappinessUnits() const
{
	return m_iMilitaryHappinessUnits;
}


int CvCity::getMilitaryHappiness() const
{
	return (getMilitaryHappinessUnits() * GET_PLAYER(getOwner()).getHappyPerMilitaryUnit());
}


void CvCity::changeMilitaryHappinessUnits(int iChange)
{
	if (iChange != 0)
	{
		m_iMilitaryHappinessUnits += iChange;
		FASSERT_NOT_NEGATIVE(getMilitaryHappinessUnits());

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getBuildingGoodHappiness() const
{
	return m_iBuildingGoodHappiness + std::max(0, calculatePopulationHappiness());
}


int CvCity::getBuildingBadHappiness() const
{
	return m_iBuildingBadHappiness + std::min(0, calculatePopulationHappiness());
}


int CvCity::getBuildingHappiness(BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	const CvBuildingInfo& info = GC.getBuildingInfo(eBuilding);
	int iHappiness =
	(
		info.getHappiness()
		+
		info.getHappinessPercentPerPopulation() * getPopulation() / 100
		+
		getBuildingHappyChange(eBuilding)
		+
		GET_PLAYER(getOwner()).getExtraBuildingHappiness(eBuilding)
	);

	if (info.getReligionType() != NO_RELIGION && info.getReligionType() == GET_PLAYER(getOwner()).getStateReligion())
	{
		iHappiness += info.getStateReligionHappiness();
	}

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		iHappiness += info.getCommerceHappiness(iI) * GET_PLAYER(getOwner()).getCommercePercent((CommerceTypes)iI) / 100;
	}

	return iHappiness;
}


void CvCity::changeBuildingGoodHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingGoodHappiness += iChange;
		FASSERT_NOT_NEGATIVE(getBuildingGoodHappiness());

		AI_setAssignWorkDirty(true);
	}
}


void CvCity::changeBuildingBadHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingBadHappiness += iChange;
		FAssert(getBuildingBadHappiness() <= 0);

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getExtraBuildingGoodHappiness() const
{
	return m_iExtraBuildingGoodHappiness;
}


int CvCity::getExtraBuildingBadHappiness() const
{
	return m_iExtraBuildingBadHappiness;
}


/********************************************************************************/
/* 	New Civic AI						19.08.2010				Fuyu			*/
/********************************************************************************/
//Fuyu bLimited
void CvCity::updateExtraBuildingHappiness(bool bLimited)
{
	PROFILE_EXTRA_FUNC();
	int iNewExtraBuildingGoodHappiness = 0;
	int iNewExtraBuildingBadHappiness = 0;

	foreach_(const BuildingTypes eTypeX, getHasBuildings())
	{
		if (!isReligiouslyLimitedBuilding(eTypeX) && !isDisabledBuilding(eTypeX))
		{
			int iChange = GET_PLAYER(getOwner()).getExtraBuildingHappiness(eTypeX);

			if (iChange > 0)
			{
				iNewExtraBuildingGoodHappiness += iChange;
			}
			else
			{
				iNewExtraBuildingBadHappiness += iChange;
			}
		}
	}

	if (getExtraBuildingGoodHappiness() != iNewExtraBuildingGoodHappiness)
	{
		m_iExtraBuildingGoodHappiness = iNewExtraBuildingGoodHappiness;
		FASSERT_NOT_NEGATIVE(getExtraBuildingGoodHappiness());

		if (!bLimited)
		{
			AI_setAssignWorkDirty(true);
		}
	}

	if (getExtraBuildingBadHappiness() != iNewExtraBuildingBadHappiness)
	{
		m_iExtraBuildingBadHappiness = iNewExtraBuildingBadHappiness;
		FAssert(getExtraBuildingBadHappiness() <= 0);

		if (!bLimited)
		{
			AI_setAssignWorkDirty(true);
		}
	}
}
int CvCity::getAdditionalHappinessByCivic(CivicTypes eCivic, bool bDifferenceToCurrent, bool bCivicOptionVacuum, ReligionTypes eStateReligion, int iExtraPop, int iMilitaryHappinessUnits) const
{
	PROFILE_EXTRA_FUNC();
	if (bDifferenceToCurrent)
	{
		return getAdditionalHappinessByCivic(eCivic, false, bCivicOptionVacuum, eStateReligion, iExtraPop, iMilitaryHappinessUnits) - getAdditionalHappinessByCivic(GET_PLAYER(getOwner()).getCivics((CivicOptionTypes)(GC.getCivicInfo(eCivic).getCivicOptionType())), false, bCivicOptionVacuum, eStateReligion, iExtraPop, iMilitaryHappinessUnits);
	}

	if (eCivic == NO_CIVIC)
	{
		return 0;
	}

	const CvCivicInfo& kCivic = GC.getCivicInfo(eCivic);
	const CvPlayer& kOwner = GET_PLAYER(getOwner());
	if (eStateReligion == NO_RELIGION)
	{
		eStateReligion = kOwner.getStateReligion();
	}
	if (!bCivicOptionVacuum && !kCivic.isStateReligion() && kOwner.getStateReligionCount() == 1)
	{
		if (GC.getCivicInfo(kOwner.getCivics((CivicOptionTypes)(kCivic.getCivicOptionType()))).isStateReligion())
		{
			eStateReligion = NO_RELIGION;
		}
	}

	int iHappy = 0;

	//#1.a: Military Happiness
	if (kCivic.getHappyPerMilitaryUnit() != 0)
	{
		if (iMilitaryHappinessUnits < 0) //default -1
		{
			iMilitaryHappinessUnits = getMilitaryHappinessUnits();
		}
		iHappy += iMilitaryHappinessUnits * kCivic.getHappyPerMilitaryUnit();
	}

	//#1.b: CivicPercentAnger and WarWearinessModifier
	if (kCivic.getCivicPercentAnger() != 0 || kCivic.getWarWearinessModifier() != 0 && kOwner.getWarWearinessPercentAnger() != 0)
	{
		//int CvCity::unhappyLevel(int iExtra) const
		int iAngerPercent = 0;

		iAngerPercent += getOvercrowdingPercentAnger(iExtraPop);
		iAngerPercent += getNoMilitaryPercentAnger();
		iAngerPercent += getCulturePercentAnger();
		iAngerPercent += getReligionPercentAnger();
		iAngerPercent += getHurryPercentAnger(iExtraPop);
		iAngerPercent += getConscriptPercentAnger(iExtraPop);
		iAngerPercent += getDefyResolutionPercentAnger(iExtraPop);
		int iOldWarWearinessAngerPercent;
		if (!bCivicOptionVacuum && kOwner.getWarWearinessPercentAnger() != 0
			&& GC.getCivicInfo(kOwner.getCivics((CivicOptionTypes)(kCivic.getCivicOptionType()))).getWarWearinessModifier() != 0)
		{
			//int CvCity::getWarWearinessPercentAnger() const
			iOldWarWearinessAngerPercent = kOwner.getWarWearinessPercentAnger();

			iOldWarWearinessAngerPercent *= std::max(0, (getWarWearinessModifier() + kOwner.getWarWearinessModifier() - GC.getCivicInfo(kOwner.getCivics((CivicOptionTypes)(kCivic.getCivicOptionType()))).getWarWearinessModifier() + 100));
			iOldWarWearinessAngerPercent /= 100;
		}
		else
		{
			iOldWarWearinessAngerPercent = getWarWearinessPercentAnger();
		}
		iAngerPercent += iOldWarWearinessAngerPercent;

		for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
		{
			iAngerPercent += kOwner.getCivicPercentAnger((CivicTypes)iI);
		}

		int iUnhappinessNow = ((iAngerPercent * (getPopulation() + iExtraPop)) / GC.getPERCENT_ANGER_DIVISOR());
		iAngerPercent -= kOwner.getCivicPercentAnger(eCivic);

		if (kOwner.getWarWearinessPercentAnger() != 0 && kCivic.getWarWearinessModifier() != 0)
		{
			//int CvCity::getWarWearinessPercentAnger() const
			int iNewWarWearinessAngerPercent = kOwner.getWarWearinessPercentAnger();

			iNewWarWearinessAngerPercent *= std::max(0, (getWarWearinessModifier() + kOwner.getWarWearinessModifier() + kCivic.getWarWearinessModifier() - ((bCivicOptionVacuum) ? 0 : GC.getCivicInfo(kOwner.getCivics((CivicOptionTypes)(kCivic.getCivicOptionType()))).getWarWearinessModifier()) + 100));
			iNewWarWearinessAngerPercent /= 100;
			iAngerPercent -= iOldWarWearinessAngerPercent;
			iAngerPercent += iNewWarWearinessAngerPercent;
		}
		int iUnhappinessThen = ((iAngerPercent * (getPopulation() + iExtraPop)) / GC.getPERCENT_ANGER_DIVISOR());
		iHappy += iUnhappinessNow - iUnhappinessThen;
	}

	//#1.c: LargestCityHappiness
	if (kCivic.getLargestCityHappiness() != 0)
	{
		//int CvCity::getLargestCityHappiness() const
		if (findPopulationRank() <= GC.getWorldInfo(GC.getMap().getWorldSize()).getTargetNumCities())
		{
			iHappy += kCivic.getLargestCityHappiness();
		}
	}

	if (kCivic.getCivicHappiness() != 0)
	{
		iHappy += kCivic.getCivicHappiness();
	}

	if (kCivic.getCityLimit(getOwner()) > 0 && kCivic.getCityOverLimitUnhappy() > 0 && kOwner.getNumCities() > kCivic.getCityLimit(getOwner()))
	{
		iHappy -= kCivic.getCityOverLimitUnhappy() * (kOwner.getNumCities() - kCivic.getCityLimit(getOwner()));
	}

	if (kCivic.getForeignerUnhappyPercent() > 0)
	{
		iHappy -= ((100 / kCivic.getForeignerUnhappyPercent()) * (100 - plot()->calculateCulturePercent(getOwner()))) / 100;
	}

	//#1.d: BuildingHappinessChanges
	if (kCivic.isAnyBuildingHappinessChange())
	{
		foreach_(const BuildingTypes eTypeX, getHasBuildings())
		{
			if (!isDisabledBuilding(eTypeX))
			{
				iHappy += kCivic.getBuildingHappinessChanges(eTypeX);
			}
		}
	}

	//#1.e: FeatureHappinessChanges
	if (kCivic.isAnyFeatureHappinessChange())
	{
		for (int iI = 0; iI < GC.getNumFeatureInfos(); iI++)
		{
			const int iTempHappy = kCivic.getFeatureHappinessChanges(iI);

			if (iTempHappy != 0)
			{
				iHappy += iTempHappy * algo::count_if(plots(NUM_CITY_PLOTS), CvPlot::fn::getFeatureType() == (FeatureTypes)iI);
			}
		}
	}

	//#1.f: Religious Happiness
	if (kCivic.getStateReligionHappiness() != 0 || kCivic.getNonStateReligionHappiness() != 0)
	{
		for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			if (isHasReligion((ReligionTypes)iI))
			{
				if ((ReligionTypes)iI == eStateReligion)
				{
					iHappy += kCivic.getStateReligionHappiness();
				}
				else
				{
					iHappy += kCivic.getNonStateReligionHappiness();
				}
			}
		}
	}

	if (isCapital() && kCivic.isNoCapitalUnhappiness())
	{
		iHappy = std::max(unhappyLevel(0), iHappy);
	}

	return iHappy;
}


int CvCity::getAdditionalHealthByCivic(CivicTypes eCivic, bool bDifferenceToCurrent) const
{
	int iGood = 0, iBad = 0, iBadBuilding = 0;
	return getAdditionalHealthByCivic(eCivic, iGood, iBad, iBadBuilding, bDifferenceToCurrent);
}

int CvCity::getAdditionalHealthByCivic(CivicTypes eCivic, int& iGood, int& iBad, int& iBadBuilding, bool bDifferenceToCurrent, int iExtraPop, bool bCivicOptionVacuum, int iIgnoreNoUnhealthyPopulationCount, int iIgnoreBuildingOnlyHealthyCount) const
{
	PROFILE_EXTRA_FUNC();
	if (bDifferenceToCurrent)
	{
		int iHealthNew = getAdditionalHealthByCivic(eCivic, iGood, iBad, iBadBuilding, false, iExtraPop, bCivicOptionVacuum, iIgnoreNoUnhealthyPopulationCount, iIgnoreBuildingOnlyHealthyCount);

		int iTempGood = 0; int iTempBad = 0; int iTempBadBuilding = 0;
		int iHealthOld = getAdditionalHealthByCivic(GET_PLAYER(getOwner()).getCivics((CivicOptionTypes)(GC.getCivicInfo(eCivic).getCivicOptionType())), iTempGood, iTempBad, iTempBadBuilding, false, iExtraPop, bCivicOptionVacuum, iIgnoreNoUnhealthyPopulationCount, iIgnoreBuildingOnlyHealthyCount);
		iGood += iTempBad;
		iBad += iTempGood;
		iBadBuilding -= iTempBadBuilding; //can become negative

		return iHealthNew - iHealthOld;
	}


	if (eCivic == NO_CIVIC)
	{
		return 0;
	}

	const CvCivicInfo& kCivic = GC.getCivicInfo(eCivic);
	CvPlayer& kOwner = GET_PLAYER(getOwner());

	if (!bCivicOptionVacuum)
	{
		if (kOwner.isCivic(eCivic) || GC.getCivicInfo(kOwner.getCivics((CivicOptionTypes)kCivic.getCivicOptionType())).isNoUnhealthyPopulation())
		{
			iIgnoreNoUnhealthyPopulationCount++;
		}
		if (kOwner.isCivic(eCivic) || GC.getCivicInfo(kOwner.getCivics((CivicOptionTypes)kCivic.getCivicOptionType())).isBuildingOnlyHealthy())
		{
			iIgnoreBuildingOnlyHealthyCount++;
		}
	}

	//#2.a: ExtraHealth
	addGoodOrBad(kCivic.getExtraHealth(), iGood, iBad);

	//#2.b: NoUnhealthyPopulation
	if (kCivic.isNoUnhealthyPopulation())
	{
		if ((iIgnoreNoUnhealthyPopulationCount != 0))
		{
			if (kOwner.getNoUnhealthyPopulationCount() <= iIgnoreNoUnhealthyPopulationCount && getNoUnhealthyPopulationCount() <= 0)
			{
				//std::max(0, ((getPopulation() + iExtra - ((bNoAngry) ? angryPopulation(iExtra) : 0))))
				iGood += std::max(0, ((getPopulation() + iExtraPop)));
			}
		}
		else
		{
			iGood += unhealthyPopulation(false, iExtraPop);
		}
	}

	//#2.c: BuildingOnlyHealthy
	if (kCivic.isBuildingOnlyHealthy())
	{
		if (iIgnoreBuildingOnlyHealthyCount != 0)
		{
			int iOwnerBuildingOnlyHealthyCount = kOwner.getBuildingOnlyHealthyCount();
			if (iOwnerBuildingOnlyHealthyCount <= iIgnoreBuildingOnlyHealthyCount && getBuildingOnlyHealthyCount() <= 0)
			{
				kOwner.changeBuildingOnlyHealthyCount(-iOwnerBuildingOnlyHealthyCount, true);
				iGood -= totalBadBuildingHealth();
				kOwner.changeBuildingOnlyHealthyCount(iOwnerBuildingOnlyHealthyCount, true);
			}
		}
		else
		{
			iGood -= totalBadBuildingHealth();
		}
	}

	//#2.d: BuildingHealthChanges
	if (kCivic.isAnyBuildingHealthChange())
	{
		foreach_(const BuildingTypes eTypeX, getHasBuildings())
		{
			if (!isDisabledBuilding(eTypeX))
			{
				int iTempHealth = kCivic.getBuildingHealthChanges(eTypeX);

				if (iTempHealth > 0)
				{
					iGood += iTempHealth;
				}
				else if (iTempHealth < 0 && (getBuildingOnlyHealthyCount() <= 0 || kOwner.getBuildingOnlyHealthyCount() <= iIgnoreBuildingOnlyHealthyCount))
				{
					iBad -= iTempHealth;
					iBadBuilding -= iTempHealth;
					if (kCivic.isBuildingOnlyHealthy())
					{
						iGood += iTempHealth;
					}
				}
			}
		}
	}
	return iGood - iBad;
}

int CvCity::getAdditionalHealthByPlayerNoUnhealthyPopulation(int iExtraPop, int iIgnoreNoUnhealthyPopulationCount) const
{
	int iHealth = 0;
	if (iIgnoreNoUnhealthyPopulationCount != 0)
	{
		if (GET_PLAYER(getOwner()).getNoUnhealthyPopulationCount() <= iIgnoreNoUnhealthyPopulationCount && getNoUnhealthyPopulationCount() <= 0)
		{
			//std::max(0, ((getPopulation() + iExtra - ((bNoAngry) ? angryPopulation(iExtra) : 0))))
			iHealth += std::max(0, ((getPopulation() + iExtraPop)));
		}
	}
	else
	{
		iHealth += unhealthyPopulation(false, iExtraPop);
	}
	return iHealth;
}

int CvCity::getAdditionalHealthByPlayerBuildingOnlyHealthy(int iIgnoreBuildingOnlyHealthyCount) const
{
	int iHealth = 0;
	if (iIgnoreBuildingOnlyHealthyCount != 0)
	{
		CvPlayer& kOwner = GET_PLAYER(getOwner());
		int iOwnerBuildingOnlyHealthyCount = kOwner.getBuildingOnlyHealthyCount();
		if (iOwnerBuildingOnlyHealthyCount <= iIgnoreBuildingOnlyHealthyCount && getBuildingOnlyHealthyCount() <= 0)
		{
			kOwner.changeBuildingOnlyHealthyCount(-iOwnerBuildingOnlyHealthyCount, true);
			iHealth -= totalBadBuildingHealth();
			kOwner.changeBuildingOnlyHealthyCount(iOwnerBuildingOnlyHealthyCount, true);
		}
	}
	else
	{
		iHealth -= totalBadBuildingHealth();
	}
	return iHealth;
}

/********************************************************************************/
/* 	New Civic AI												END 			*/
/********************************************************************************/

// BUG - Building Additional Happiness - start
/*
 * Returns the total additional happiness that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalHappinessByBuilding(BuildingTypes eBuilding) const
{
	int iGood = 0, iBad = 0, iAngryPop = 0;
	return getAdditionalHappinessByBuilding(eBuilding, iGood, iBad, iAngryPop);
}

/*
 * Returns the total additional happiness that adding one of the given buildings will provide
 * and sets the good and bad levels individually and any resulting additional angry population.
 *
 * Doesn't reset iGood or iBad to zero.
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalHappinessByBuilding(BuildingTypes eBuilding, int& iGood, int& iBad, int& iAngryPop) const
{
	PROFILE_FUNC();

	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eBuilding);

	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	int iStarting = iGood - iBad;
	int iStartingBad = iBad;

	// Basic
	addGoodOrBad(kBuilding.getHappiness(), iGood, iBad);

	// Building
	addGoodOrBad(getBuildingHappyChange(eBuilding), iGood, iBad);

	// Other Building
	foreach_(const BuildingModifier2& pair, kBuilding.getBuildingHappinessChanges())
	{
		addGoodOrBad(pair.second * (isActiveBuilding(pair.first) + (eBuilding == pair.first ? 1 : 0)), iGood, iBad);
	}

	// Player Building
	addGoodOrBad(GET_PLAYER(getOwner()).getExtraBuildingHappiness(eBuilding), iGood, iBad);

	// Area
	addGoodOrBad(kBuilding.getAreaHappiness(), iGood, iBad);

	// Global
	addGoodOrBad(kBuilding.getGlobalHappiness(), iGood, iBad);

	// Religion
	if (kBuilding.getReligionType() != NO_RELIGION && kBuilding.getReligionType() == GET_PLAYER(getOwner()).getStateReligion())
	{
		iGood += kBuilding.getStateReligionHappiness();
	}

	// Bonus
	foreach_(const BonusModifier& modifier, kBuilding.getBonusHappinessChanges())
	{
		if ((hasBonus(modifier.first) || kBuilding.getFreeBonuses().hasValue(modifier.first)))
		{
			addGoodOrBad(modifier.second, iGood, iBad);
		}
	}

	// Commerce
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		int iCommerceHappiness = kBuilding.getCommerceHappiness(iI);

		if (iCommerceHappiness != 0)
		{
			addGoodOrBad(iCommerceHappiness * GET_PLAYER(getOwner()).getCommercePercent((CommerceTypes)iI) / 100, iGood, iBad);
		}
	}

	// War Weariness Modifier
	int iWarWearinessModifier = kBuilding.getWarWearinessModifier() + kBuilding.getGlobalWarWearinessModifier();
	if (iWarWearinessModifier != 0)
	{
		int iBaseAngerPercent = 0;

		iBaseAngerPercent += getOvercrowdingPercentAnger();
		iBaseAngerPercent += getNoMilitaryPercentAnger();
		iBaseAngerPercent += getCulturePercentAnger();
		iBaseAngerPercent += getReligionPercentAnger();
		iBaseAngerPercent += getHurryPercentAnger();
		iBaseAngerPercent += getConscriptPercentAnger();
		iBaseAngerPercent += getDefyResolutionPercentAnger();
		for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
		{
			iBaseAngerPercent += GET_PLAYER(getOwner()).getCivicPercentAnger((CivicTypes)iI);
		}

		int iCurrentAngerPercent = iBaseAngerPercent + getWarWearinessPercentAnger();
		int iCurrentUnhappiness = iCurrentAngerPercent * getPopulation() / GC.getPERCENT_ANGER_DIVISOR();

		int iNewWarAngerPercent = GET_PLAYER(getOwner()).getWarWearinessPercentAnger();
		iNewWarAngerPercent *= std::max(0, (iWarWearinessModifier + getWarWearinessModifier() + GET_PLAYER(getOwner()).getWarWearinessModifier() + 100));
		iNewWarAngerPercent /= 100;
		int iNewAngerPercent = iBaseAngerPercent + iNewWarAngerPercent;
		int iNewUnhappiness = iNewAngerPercent * getPopulation() / GC.getPERCENT_ANGER_DIVISOR();

		iBad += iNewUnhappiness - iCurrentUnhappiness;
	}

	//	KOSHLING - port from K-mod - no unhappiness already => we don't care what this building does
	if (isNoUnhappiness())
	{
		iBad = iStartingBad;
	}
	// No Unhappiness
	else if (kBuilding.isNoUnhappiness())
	{
		// override extra unhappiness and completely negate all existing unhappiness
		iBad = iStartingBad - unhappyLevel();
	}
	// Effect on Angry Population
	int iHappy = happyLevel();
	int iUnhappy = unhappyLevel();
	int iPop = getPopulation();
	iAngryPop += range((iUnhappy + iBad) - (iHappy + iGood), 0, iPop) - range(iUnhappy - iHappy, 0, iPop);

	foreach_(const TechModifier& modifier, kBuilding.getTechHappinessChanges())
	{
		if (GET_TEAM(getTeam()).isHasTech(modifier.first))
		{
			addGoodOrBad(modifier.second, iGood, iBad);
		}
	}

	for (int iI = 0; iI < kBuilding.getNumReplacedBuilding(); iI++)
	{
		const BuildingTypes eBuildingX = static_cast<BuildingTypes>(kBuilding.getReplacedBuilding(iI));

		if (isActiveBuilding(eBuildingX))
		{
			addGoodOrBad(-getBuildingHappiness(eBuildingX), iGood, iBad);
		}
	}

	if (kBuilding.getHappinessPercentPerPopulation() > 0)
	{
		addGoodOrBad(kBuilding.getHappinessPercentPerPopulation() * getPopulation() / 100, iGood, iBad);
	}

	int iSpecialistExtraHappy = 0;

	for (int iI = 1; iI < kBuilding.getFreeSpecialist() + 1; iI++)
	{
		const SpecialistTypes eNewSpecialist = getBestSpecialist(iI);
		if (eNewSpecialist == NO_SPECIALIST) break;

		iSpecialistExtraHappy += GC.getSpecialistInfo(eNewSpecialist).getHappinessPercent();
	}
	iSpecialistExtraHappy /= 100;
	addGoodOrBad(iSpecialistExtraHappy, iGood, iBad);

	if (kBuilding.getNumPopulationEmployed() > 0)
	{
		int* paiCommerce = new int[NUM_COMMERCE_TYPES];
		int* paiYield = new int[NUM_YIELD_TYPES];
		int iGreatPeopleRate;
		int iHappiness;
		int iHealthiness;
		removeWorstCitizenActualEffects(kBuilding.getNumPopulationEmployed(), iGreatPeopleRate, iHappiness, iHealthiness, paiYield, paiCommerce);
		SAFE_DELETE_ARRAY(paiCommerce);
		SAFE_DELETE_ARRAY(paiYield);
		addGoodOrBad(iHappiness, iGood, iBad);
	}

	return iGood - iBad - iStarting;
}


/*
 * Returns the total additional health that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalHealthByBuilding(BuildingTypes eBuilding) const
{
	int iGood = 0, iBad = 0, iSpoiledFood = 0, iStarvation = 0;
	return getAdditionalHealthByBuilding(eBuilding, iGood, iBad, iSpoiledFood, iStarvation);
}

/*
 * Returns the total additional health that adding one of the given buildings will provide
 * and sets the good and bad levels individually and any resulting additional spoiled food.
 *
 * Doesn't reset iGood, iBad, iSpoiledFood, iStarvation to zero.
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalHealthByBuilding(BuildingTypes eBuilding, int& iGood, int& iBad, int& iSpoiledFood, int& iStarvation) const
{
	PROFILE_FUNC();

	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eBuilding);

	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	const int iStarting = iGood - iBad;
	const int iStartingBad = iBad;

	// Basic
	addGoodOrBad(kBuilding.getHealth(), iGood, iBad);

	// Building
	addGoodOrBad(getBuildingHealthChange(eBuilding), iGood, iBad);

	// Player Building
	addGoodOrBad(GET_PLAYER(getOwner()).getExtraBuildingHealth(eBuilding), iGood, iBad);

	// Area
	addGoodOrBad(kBuilding.getAreaHealth(), iGood, iBad);

	// Global
	addGoodOrBad(kBuilding.getGlobalHealth(), iGood, iBad);

	// No Unhealthiness from Buildings
	if (isBuildingOnlyHealthy())
	{
		// undo bad from this building
		iBad = iStartingBad;
	}
	if (kBuilding.isBuildingOnlyHealthy())
	{
		// undo bad from this and all existing buildings
		iBad = iStartingBad + totalBadBuildingHealth();
	}

	// Bonus
	foreach_(const BonusModifier& modifier, kBuilding.getBonusHealthChanges())
	{
		if ((hasBonus(modifier.first) || kBuilding.getFreeBonuses().hasValue(modifier.first)))
		{
			addGoodOrBad(modifier.second, iGood, iBad);
		}
	}

	// No Unhealthiness from Population
	if (kBuilding.isNoUnhealthyPopulation())
	{
		iBad -= getPopulation();
	}

	// Effect on Spoiled Food
	int iHealthy = goodHealth();
	int iUnhealthy = badHealth();
	int iFood = getYieldRate(YIELD_FOOD) - foodConsumption();
	iSpoiledFood -= std::min(0, (iHealthy + iGood) - (iUnhealthy + iBad)) - std::min(0, iHealthy - iUnhealthy);
	if (iSpoiledFood > 0)
	{
		if (iFood <= 0)
		{
			iStarvation += iSpoiledFood;
		}
		else if (iSpoiledFood > iFood)
		{
			iStarvation += iSpoiledFood - iFood;
		}
	}
	else if (iSpoiledFood < 0)
	{
		if (iFood < 0)
		{
			iStarvation += std::max(iFood, iSpoiledFood);
		}
	}

	foreach_(const TechModifier& modifier, kBuilding.getTechHealthChanges())
	{
		if (GET_TEAM(getTeam()).isHasTech(modifier.first))
		{
			addGoodOrBad(modifier.second, iGood, iBad);
		}
	}

	for (int iI = 0; iI < kBuilding.getNumReplacedBuilding(); iI++)
	{
		const BuildingTypes eBuildingX = static_cast<BuildingTypes>(kBuilding.getReplacedBuilding(iI));

		if (hasFullyActiveBuilding(eBuildingX))
		{
			addGoodOrBad(-getBuildingHealth(eBuildingX), iGood, iBad);
		}
	}

	if (kBuilding.getHealthPercentPerPopulation() > 0)
	{
		addGoodOrBad(kBuilding.getHealthPercentPerPopulation() * getPopulation() / 100, iGood, iBad);
	}

	int iSpecialistExtraHealth = 0;

	for (int iI = 1; iI < kBuilding.getFreeSpecialist() + 1; iI++)
	{
		const SpecialistTypes eNewSpecialist = getBestSpecialist(iI);
		if (eNewSpecialist == NO_SPECIALIST) break;

		iSpecialistExtraHealth += GC.getSpecialistInfo(eNewSpecialist).getHealthPercent();
	}
	iSpecialistExtraHealth /= 100;
	addGoodOrBad(iSpecialistExtraHealth, iGood, iBad);

	if (kBuilding.getNumPopulationEmployed() > 0)
	{
		int* paiCommerce = new int[NUM_COMMERCE_TYPES];
		int* paiYield = new int[NUM_YIELD_TYPES];
		int iGreatPeopleRate;
		int iHappiness;
		int iHealthiness;
		removeWorstCitizenActualEffects(kBuilding.getNumPopulationEmployed(), iGreatPeopleRate, iHappiness, iHealthiness, paiYield, paiCommerce);
		SAFE_DELETE_ARRAY(paiCommerce);
		SAFE_DELETE_ARRAY(paiYield);
		addGoodOrBad(iHealthiness, iGood, iBad);
	}
	return iGood - iBad - iStarting;
}

/*
 * Adds iValue to iGood if it is positive or its negative to iBad if it is negative.
 */
void addGoodOrBad(int iValue, int& iGood, int& iBad)
{
	if (iValue > 0)
	{
		iGood += iValue;
	}
	else if (iValue < 0)
	{
		iBad -= iValue;
	}
}

/*
 * Subtracts iValue from iGood if it is positive or its negative from iBad if it is negative.
 */
void subtractGoodOrBad(int iValue, int& iGood, int& iBad)
{
	if (iValue > 0)
	{
		iGood -= iValue;
	}
	else if (iValue < 0)
	{
		iBad += iValue;
	}
}
// BUG - Building Additional Happiness - end


int CvCity::getExtraBuildingGoodHealth() const
{
	return m_iExtraBuildingGoodHealth;
}


int CvCity::getExtraBuildingBadHealth() const
{
	return m_iExtraBuildingBadHealth;
}


void CvCity::updateExtraBuildingHealth(bool bLimited)
{
	PROFILE_EXTRA_FUNC();
	int iNewExtraBuildingGoodHealth = 0;
	int iNewExtraBuildingBadHealth = 0;

	foreach_(const BuildingTypes eTypeX, getHasBuildings())
	{
		if (!isReligiouslyLimitedBuilding(eTypeX) && !isDisabledBuilding(eTypeX))
		{
			int iChange = GET_PLAYER(getOwner()).getExtraBuildingHealth(eTypeX);

			if (iChange > 0)
			{
				iNewExtraBuildingGoodHealth += iChange;
			}
			else iNewExtraBuildingBadHealth += iChange;
		}
	}

	if (getExtraBuildingGoodHealth() != iNewExtraBuildingGoodHealth)
	{
		m_iExtraBuildingGoodHealth = iNewExtraBuildingGoodHealth;
		FASSERT_NOT_NEGATIVE(getExtraBuildingGoodHealth());

		if (!bLimited)
		{
			AI_setAssignWorkDirty(true);
		}
	}

	if (getExtraBuildingBadHealth() != iNewExtraBuildingBadHealth)
	{
		m_iExtraBuildingBadHealth = iNewExtraBuildingBadHealth;
		FAssert(getExtraBuildingBadHealth() <= 0);

		if (!bLimited)
		{
			AI_setAssignWorkDirty(true);
		}
	}
}


int CvCity::getFeatureGoodHappiness() const
{
	return m_iFeatureGoodHappiness;
}


int CvCity::getFeatureBadHappiness() const
{
	return m_iFeatureBadHappiness;
}


void CvCity::updateFeatureHappiness(bool bLimited)
{
	PROFILE_EXTRA_FUNC();
	int iNewFeatureGoodHappiness = 0;
	int iNewFeatureBadHappiness = 0;

	foreach_(const CvPlot* pLoopPlot, plots())
	{
		const FeatureTypes eFeature = pLoopPlot->getFeatureType();

		if (eFeature != NO_FEATURE)
		{
			int iHappy = GET_PLAYER(getOwner()).getFeatureHappiness(eFeature);
			if (iHappy > 0)
			{
				iNewFeatureGoodHappiness += iHappy;
			}
			else
			{
				iNewFeatureBadHappiness += iHappy;
			}
		}

		const ImprovementTypes eImprovement = pLoopPlot->getImprovementType();

		if (NO_IMPROVEMENT != eImprovement)
		{
			int iHappy = GC.getImprovementInfo(eImprovement).getHappiness();

			const CvPlayer& kPlayer = GET_PLAYER(getOwner());
			for (int iJ = 0; iJ < GC.getNumCivicOptionInfos(); iJ++)
			{
				if (kPlayer.getCivics((CivicOptionTypes)iJ) != NO_CIVIC)
				{
					iHappy += GC.getCivicInfo(kPlayer.getCivics((CivicOptionTypes)iJ)).getImprovementHappinessChanges(eImprovement);
				}
			}

			if (iHappy > 0)
			{
				iNewFeatureGoodHappiness += iHappy;
			}
			else
			{
				iNewFeatureBadHappiness += iHappy;
			}
		}
	}

	if (getFeatureGoodHappiness() != iNewFeatureGoodHappiness)
	{
		m_iFeatureGoodHappiness = iNewFeatureGoodHappiness;
		FASSERT_NOT_NEGATIVE(getFeatureGoodHappiness());

		if (!bLimited)
		{
			AI_setAssignWorkDirty(true);
		}
	}

	if (getFeatureBadHappiness() != iNewFeatureBadHappiness)
	{
		m_iFeatureBadHappiness = iNewFeatureBadHappiness;
		FAssert(getFeatureBadHappiness() <= 0);

		if (!bLimited)
		{
			AI_setAssignWorkDirty(true);
		}
	}
}


int CvCity::getBonusGoodHappiness() const
{
	return m_iBonusGoodHappiness;
}


int CvCity::getBonusBadHappiness() const
{
	return m_iBonusBadHappiness;
}


void CvCity::changeBonusGoodHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iBonusGoodHappiness += iChange;
		AI_setAssignWorkDirty(true);
	}
}


void CvCity::changeBonusBadHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iBonusBadHappiness += iChange;
		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getReligionGoodHappiness() const
{
	return m_iReligionGoodHappiness;
}


int CvCity::getReligionBadHappiness() const
{
	return m_iReligionBadHappiness;
}


int CvCity::getReligionHappiness(ReligionTypes eReligion) const
{
	if (isHasReligion(eReligion))
	{
		if (eReligion == GET_PLAYER(getOwner()).getStateReligion())
		{
			return GET_PLAYER(getOwner()).getStateReligionHappiness();
		}
		return GET_PLAYER(getOwner()).getNonStateReligionHappiness();
	}
	return 0;
}


void CvCity::updateReligionHappiness(bool bLimited)
{
	PROFILE_EXTRA_FUNC();
	int iNewReligionGoodHappiness = 0;
	int iNewReligionBadHappiness = 0;

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		int iChange = getReligionHappiness((ReligionTypes)iI);

		if (iChange > 0)
		{
			iNewReligionGoodHappiness += iChange;
		}
		else
		{
			iNewReligionBadHappiness += iChange;
		}
	}

	if (getReligionGoodHappiness() != iNewReligionGoodHappiness)
	{
		m_iReligionGoodHappiness = iNewReligionGoodHappiness;
		FASSERT_NOT_NEGATIVE(getReligionGoodHappiness());

		if (!bLimited)
		{
			AI_setAssignWorkDirty(true);
		}
	}

	if (getReligionBadHappiness() != iNewReligionBadHappiness)
	{
		m_iReligionBadHappiness = iNewReligionBadHappiness;
		FAssert(getReligionBadHappiness() <= 0);

		if (!bLimited)
		{
			AI_setAssignWorkDirty(true);
		}
	}
}


int CvCity::getExtraHappiness() const
{
	return m_iExtraHappiness;
}


void CvCity::changeExtraHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraHappiness += iChange;

		AI_setAssignWorkDirty(true);
	}
}

int CvCity::getExtraHealth() const
{
	return m_iExtraHealth;
}


void CvCity::changeExtraHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraHealth += iChange;

		AI_setAssignWorkDirty(true);
	}
}



int CvCity::getHurryAngerTimer() const
{
	return m_iHurryAngerTimer;
}


void CvCity::changeHurryAngerTimer(int iChange)
{
	if (iChange != 0)
	{
		m_iHurryAngerTimer += iChange;
		FASSERT_NOT_NEGATIVE(getHurryAngerTimer());

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getRevRequestAngerTimer() const
{
	return m_iRevRequestAngerTimer;
}

void CvCity::changeRevRequestAngerTimer(int iChange)
{
	if (iChange != 0)
	{
		m_iRevRequestAngerTimer += iChange;
		FASSERT_NOT_NEGATIVE(getRevRequestAngerTimer());

		AI_setAssignWorkDirty(true);
	}
}

int CvCity::getRevSuccessTimer() const
{
	return m_iRevSuccessTimer;
}

void CvCity::changeRevSuccessTimer(int iChange)
{
	if (iChange != 0)
	{
		m_iRevSuccessTimer += iChange;
		FASSERT_NOT_NEGATIVE(getRevSuccessTimer());

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getConscriptAngerTimer() const
{
	return m_iConscriptAngerTimer;
}


void CvCity::changeConscriptAngerTimer(int iChange)
{
	if (iChange != 0)
	{
		m_iConscriptAngerTimer += iChange;
		FASSERT_NOT_NEGATIVE(getConscriptAngerTimer());

		AI_setAssignWorkDirty(true);
	}
}

int CvCity::getDefyResolutionAngerTimer() const
{
	return m_iDefyResolutionAngerTimer;
}


void CvCity::changeDefyResolutionAngerTimer(int iChange)
{
	if (iChange != 0)
	{
		m_iDefyResolutionAngerTimer += iChange;
		FASSERT_NOT_NEGATIVE(getDefyResolutionAngerTimer());

		AI_setAssignWorkDirty(true);
	}
}

int CvCity::flatDefyResolutionAngerLength() const
{
	int iAnger = GC.getDEFY_RESOLUTION_ANGER_DIVISOR();

	iAnger *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
	iAnger /= 100;

	return std::max(1, iAnger);
}


int CvCity::getHappinessTimer() const
{
	return m_iHappinessTimer;
}


void CvCity::changeHappinessTimer(int iChange)
{
	if (iChange != 0)
	{
		m_iHappinessTimer += iChange;
		FASSERT_NOT_NEGATIVE(getHappinessTimer());

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getNoUnhappinessCount() const
{
	return m_iNoUnhappinessCount;
}


bool CvCity::isNoUnhappiness() const
{
	return (getNoUnhappinessCount() > 0);
}


void CvCity::changeNoUnhappinessCount(int iChange)
{
	if (iChange != 0)
	{
		m_iNoUnhappinessCount += iChange;
		FASSERT_NOT_NEGATIVE(getNoUnhappinessCount());

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getNoUnhealthyPopulationCount()	const
{
	return m_iNoUnhealthyPopulationCount;
}


bool CvCity::isNoUnhealthyPopulation() const
{
	if (GET_PLAYER(getOwner()).isNoUnhealthyPopulation())
	{
		return true;
	}

	return (getNoUnhealthyPopulationCount() > 0);
}


void CvCity::changeNoUnhealthyPopulationCount(int iChange)
{
	if (iChange != 0)
	{
		m_iNoUnhealthyPopulationCount += iChange;
		FASSERT_NOT_NEGATIVE(getNoUnhealthyPopulationCount());

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getBuildingOnlyHealthyCount() const
{
	return m_iBuildingOnlyHealthyCount;
}


bool CvCity::isBuildingOnlyHealthy() const
{
	if (GET_PLAYER(getOwner()).isBuildingOnlyHealthy())
	{
		return true;
	}

	return (getBuildingOnlyHealthyCount() > 0);
}


void CvCity::changeBuildingOnlyHealthyCount(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingOnlyHealthyCount += iChange;
		FASSERT_NOT_NEGATIVE(getBuildingOnlyHealthyCount());

		AI_setAssignWorkDirty(true);
	}
}

int CvCity::getFood() const
{
	return m_iFood;
}


void CvCity::setFood(int iNewValue)
{
	if (m_iFood != iNewValue)
	{
		m_iFood = iNewValue;

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}


void CvCity::changeFood(int iChange, const bool bHandleGrowth)
{
	if (iChange != 0)
	{
		m_iFood += iChange;

		if (bHandleGrowth)
		{
			if (iChange > 0)
			{
				changeFoodKept(std::max(1, iChange * getFoodKeptPercent() / 100));
			}
			else // decay stored food, hardcoded rate for now.
			{
				changeFoodKept(std::min(-1, iChange / 2));
			}

			if (m_iFood < 0)
			{
				while (m_iFood < 0)
				{
					if (getPopulation() > 1)
					{
						changePopulation(-1);
						m_iFood = m_iFood + growthThreshold();
					}
					else m_iFood = 0;
				}
			}
			else
			{
				int iGrowthThreshold = growthThreshold();
				if (m_iFood >= iGrowthThreshold)
				{
					if (isHuman() && AI_avoidGrowth() || AI_isEmphasizeAvoidGrowth())
					{
						m_iFood = iGrowthThreshold;
					}
					else
					{
						while (m_iFood >= iGrowthThreshold)
						{
							m_iFood -= iGrowthThreshold;

							if (m_iFood < getFoodKept())
							{
								const int iDiff = getFoodKept() - m_iFood;
								m_iFood += iDiff;
								changeFoodKept(-iDiff);
							}
							changePopulation(1);
							iGrowthThreshold = growthThreshold();
						}
					}
				}
			}
		}
#ifdef YIELD_VALUE_CACHING
		//	Yield calculation depends on city food stores so invalidate cache
		ClearYieldValueCache();
#endif
	}
}


void CvCity::changeFoodKept(int iChange)
{
	if (iChange != 0)
	{
		m_iFoodKept = range(m_iFoodKept + iChange, 0, growthThreshold() * getFoodKeptPercent() / 100);
	}
}


int CvCity::getFoodKeptPercent() const
{
	return range(m_iFoodKeptPercent, 0, 99);
}


void CvCity::changeFoodKeptPercent(int iChange)
{
	m_iFoodKeptPercent += iChange;
}

int CvCity::getMaxProductionOverflow() const
{
	return getYieldRate(YIELD_PRODUCTION) * 2;
}


int CvCity::getOverflowProduction() const
{
	return m_iOverflowProduction;
}


void CvCity::setOverflowProduction(int iNewValue)
{
	m_iOverflowProduction = iNewValue;
	FASSERT_NOT_NEGATIVE(m_iOverflowProduction);
}


void CvCity::changeOverflowProduction(int iChange)
{
	setOverflowProduction(m_iOverflowProduction + iChange);
}


int CvCity::getFeatureProduction() const
{
	return m_iFeatureProduction;
}


void CvCity::setFeatureProduction(int iNewValue)
{
	m_iFeatureProduction = iNewValue;
	FASSERT_NOT_NEGATIVE(getFeatureProduction());
}


void CvCity::changeFeatureProduction(int iChange)
{
	setFeatureProduction(getFeatureProduction() + iChange);
}


int CvCity::getMilitaryProductionModifier()	const
{
	return m_iMilitaryProductionModifier;
}


void CvCity::changeMilitaryProductionModifier(int iChange)
{
	m_iMilitaryProductionModifier = (m_iMilitaryProductionModifier + iChange);
}


int CvCity::getSpaceProductionModifier() const
{
	return m_iSpaceProductionModifier;
}


void CvCity::changeSpaceProductionModifier(int iChange)
{
	m_iSpaceProductionModifier = (m_iSpaceProductionModifier + iChange);
}


int CvCity::getExtraTradeRoutes() const
{
	return m_iExtraTradeRoutes;
}


void CvCity::changeExtraTradeRoutes(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraTradeRoutes = (m_iExtraTradeRoutes + iChange);
		//TB Note: There's a minimum on Trade routes, ultimately, of 0.  But it should be allowed to have a negative modifier.
		/*FAssert(getExtraTradeRoutes() >= 0);*/

		updateTradeRoutes();
	}
}

int CvCity::getMaxTradeRoutes() const
{
	return getOwner() == NO_PLAYER ? GC.getMAX_TRADE_ROUTES() : GC.getMAX_TRADE_ROUTES() + GET_PLAYER(getOwner()).getMaxTradeRoutesAdjustment();
}



int CvCity::getTradeRouteModifier() const
{
	return m_iTradeRouteModifier;
}

void CvCity::changeTradeRouteModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iTradeRouteModifier += iChange;

		updateTradeRoutes();
	}
}

int CvCity::getForeignTradeRouteModifier() const
{
	return m_iForeignTradeRouteModifier;
}

void CvCity::changeForeignTradeRouteModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iForeignTradeRouteModifier += iChange;

		updateTradeRoutes();
	}
}


int CvCity::getBuildingDefense() const
{
	return m_iBuildingDefense;
}


void CvCity::changeBuildingDefense(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingDefense += iChange;

		setInfoDirty(true);

		plot()->plotAction(PUF_makeInfoBarDirty);
	}
}


int CvCity::getBuildingBombardDefense() const
{
	int iBombardDefenseTotal = m_iBuildingBombardDefense;
	iBombardDefenseTotal += GET_PLAYER(getOwner()).getNationalBombardDefenseModifier();
	return std::min(GC.getGame().getModderGameOption(MODDERGAMEOPTION_MAX_BOMBARD_DEFENSE), iBombardDefenseTotal);
}


void CvCity::changeBuildingBombardDefense(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingBombardDefense += iChange;
		FASSERT_NOT_NEGATIVE(getBuildingBombardDefense());
	}
}

// BUG - Building Additional Bombard Defense - start
int CvCity::getAdditionalBombardDefenseByBuilding(BuildingTypes eBuilding) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eBuilding);

	const int iBaseDefense = getBuildingBombardDefense();

	// cap total bombard defense at 100
	return std::min(GC.getBuildingInfo(eBuilding).getBombardDefenseModifier() + iBaseDefense, 100) - iBaseDefense;
}
// BUG - Building Additional Bombard Defense - end


int CvCity::getFreeExperience() const
{
	return m_iFreeExperience;
}


void CvCity::changeFreeExperience(int iChange)
{
	m_iFreeExperience += iChange;
	FASSERT_NOT_NEGATIVE(getFreeExperience());
}


int CvCity::getCurrAirlift() const
{
	return m_iCurrAirlift;
}


void CvCity::setCurrAirlift(int iNewValue)
{
	m_iCurrAirlift = iNewValue;
	FASSERT_NOT_NEGATIVE(getCurrAirlift());
}


void CvCity::changeCurrAirlift(int iChange)
{
	setCurrAirlift(getCurrAirlift() + iChange);
}


int CvCity::getMaxAirlift() const
{
	return m_iMaxAirlift;
}


void CvCity::changeMaxAirlift(int iChange)
{
	m_iMaxAirlift += iChange;
	FASSERT_NOT_NEGATIVE(getMaxAirlift());
}

int CvCity::getAirModifier() const
{
	return m_iAirModifier;
}

void CvCity::changeAirModifier(int iChange)
{
	m_iAirModifier += iChange;
}

int CvCity::getSMAirUnitCapacity(TeamTypes eTeam) const
{
	int iCapacity = getAirUnitCapacity(eTeam);
	iCapacity *= GC.getGame().getBaseAirUnitIncrementsbyCargoVolume();
	return iCapacity;
}

int CvCity::getAirUnitCapacity(TeamTypes eTeam) const
{
	int iCapacity = (getTeam() == eTeam ? m_iAirUnitCapacity : GC.getCITY_AIR_UNIT_CAPACITY());
	iCapacity += GET_PLAYER(getOwner()).getNationalAirUnitCapacity();
	return iCapacity;
}

void CvCity::changeAirUnitCapacity(int iChange)
{
	m_iAirUnitCapacity += iChange;
	FASSERT_NOT_NEGATIVE(getAirUnitCapacity(getTeam()));
}

int CvCity::getNukeModifier() const
{
	return m_iNukeModifier;
}


void CvCity::changeNukeModifier(int iChange)
{
	m_iNukeModifier += iChange;
}


int CvCity::getFreeSpecialist() const
{
	return std::max(0, m_iFreeSpecialist);
}


void CvCity::changeFreeSpecialist(int iChange)
{
	if (iChange != 0)
	{
		m_iFreeSpecialist += iChange;

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getPowerCount() const
{
	return m_iPowerCount;
}


bool CvCity::isPower() const
{
	return getDisabledPowerTimer() < 1 && (getPowerCount() > 0 || isAreaCleanPower());
}


bool CvCity::isAreaCleanPower() const
{
	return area() != NULL && area()->isCleanPower(getTeam());
}

void CvCity::changePowerCount(int iChange)
{
	if (iChange != 0)
	{
		const bool wasPower = isPower();

		m_iPowerCount += iChange;
		FASSERT_NOT_NEGATIVE(getPowerCount());
		// cppcheck-suppress knownConditionTrueFalse
		if (wasPower != isPower())
		{
			GET_PLAYER(getOwner()).invalidateYieldRankCache();

			setCommerceDirty();

			if (getTeam() == GC.getGame().getActiveTeam())
			{
				setInfoDirty(true);
			}
		}
	}
}


int CvCity::getDefenseDamage() const
{
	return m_iDefenseDamage;
}


void CvCity::changeDefenseDamage(int iChange)
{
	if (iChange != 0)
	{
		m_iDefenseDamage = range((m_iDefenseDamage + iChange), 0, GC.getMAX_CITY_DEFENSE_DAMAGE());

		if (iChange > 0)
		{
			setBombarded(true);
		}

		setInfoDirty(true);

		plot()->plotAction(PUF_makeInfoBarDirty);
	}
}

void CvCity::changeDefenseModifier(int iChange)
{
	if (iChange != 0)
	{
		int iTotalDefense = getTotalDefense(false);

		if (iTotalDefense > 0)
		{
			changeDefenseDamage(-(GC.getMAX_CITY_DEFENSE_DAMAGE() * iChange + (iChange > 0 ? iTotalDefense : -iTotalDefense) / 2) / iTotalDefense);
		}
	}
}


int CvCity::getLastDefenseDamage() const
{
	return m_iLastDefenseDamage;
}


void CvCity::setLastDefenseDamage(int iNewValue)
{
	m_iLastDefenseDamage = iNewValue;
}


bool CvCity::isBombardable(const CvUnit* pUnit) const
{
	if (NULL != pUnit && !pUnit->isEnemy(getTeam()))
	{
		return false;
	}

	return (getDefenseModifier(false) > getExtraMinDefense());
}


int CvCity::getNaturalDefense() const
{
	if (getCultureLevel() == NO_CULTURELEVEL)
	{
		return 0;
	}
	return GC.getCultureLevelInfo(getCultureLevel()).getCityDefenseModifier();
}


int CvCity::getTotalDefense(bool bIgnoreBuilding) const
{
	return std::max(bIgnoreBuilding ? 0 : getBuildingDefense(), getNaturalDefense()) + GET_PLAYER(getOwner()).getCityDefenseModifier() + calculateBonusDefense();
}


int CvCity::getDefenseModifier(bool bIgnoreBuilding) const
{
	if (isOccupation())
	{
		return 0;
	}
	return (std::max(getExtraMinDefense(), ((getTotalDefense(bIgnoreBuilding) * (GC.getMAX_CITY_DEFENSE_DAMAGE() - getDefenseDamage())) / GC.getMAX_CITY_DEFENSE_DAMAGE())));
}


int CvCity::getOccupationTimer() const
{
	return m_iOccupationTimer;
}


bool CvCity::isOccupation() const
{
	return m_iOccupationTimer > 0;
}


void CvCity::setOccupationTimer(int iNewValue)
{
	FASSERT_NOT_NEGATIVE(iNewValue);

	if (m_iOccupationTimer != iNewValue)
	{
		const bool wasOccupation = m_iOccupationTimer > 0;

		m_iOccupationTimer = iNewValue;

		if (wasOccupation != isOccupation())
		{
			updateCorporation();
			updateTradeRoutes();

			updateCultureLevel(true);

			AI_setAssignWorkDirty(true);
		}
		setInfoDirty(true);
	}
}


void CvCity::changeOccupationTimer(int iChange)
{
	setOccupationTimer(m_iOccupationTimer + iChange);
}


int CvCity::getCultureUpdateTimer() const
{
	return m_iCultureUpdateTimer;
}


void CvCity::setCultureUpdateTimer(int iNewValue)
{
	m_iCultureUpdateTimer = iNewValue;
	FASSERT_NOT_NEGATIVE(getOccupationTimer());
}


void CvCity::changeCultureUpdateTimer(int iChange)
{
	setCultureUpdateTimer(getCultureUpdateTimer() + iChange);
}


int CvCity::getCitySizeBoost() const
{
	return m_iCitySizeBoost;
}


void CvCity::setCitySizeBoost(int iBoost)
{
	if (getCitySizeBoost() != iBoost)
	{
		m_iCitySizeBoost = iBoost;

		setLayoutDirty(true);
	}
}

bool CvCity::isNeverLost() const
{
	return m_bNeverLost;
}


void CvCity::setNeverLost(bool bNewValue)
{
	m_bNeverLost = bNewValue;
}


bool CvCity::isBombarded() const
{
	return m_bBombarded;
}


void CvCity::setBombarded(bool bNewValue)
{
	m_bBombarded = bNewValue;
}


bool CvCity::isDrafted() const
{
	return m_bDrafted;
}


void CvCity::setDrafted(bool bNewValue)
{
	m_bDrafted = bNewValue;
}


bool CvCity::isAirliftTargeted() const
{
	return m_bAirliftTargeted;
}


void CvCity::setAirliftTargeted(bool bNewValue)
{
	m_bAirliftTargeted = bNewValue;
}


bool CvCity::isPlundered() const
{
	return m_bPlundered;
}


void CvCity::setPlundered(bool bNewValue)
{
	if (bNewValue != isPlundered())
	{
		m_bPlundered = bNewValue;

		updateTradeRoutes();
	}
}


bool CvCity::isWeLoveTheKingDay() const
{
	return m_bWeLoveTheKingDay;
}


void CvCity::setWeLoveTheKingDay(bool bNewValue)
{
	PROFILE_EXTRA_FUNC();
	if (isWeLoveTheKingDay() != bNewValue)
	{
		m_bWeLoveTheKingDay = bNewValue;

		CvPlayer& owner = GET_PLAYER(getOwner());
		setMaintenanceDirty(true);

		CivicTypes eCivic = NO_CIVIC;

		for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
		{
			if (owner.isCivic((CivicTypes)iI))
			{
				if (!CvWString(GC.getCivicInfo((CivicTypes)iI).getWeLoveTheKing()).empty())
				{
					eCivic = ((CivicTypes)iI);
					break;
				}
			}
		}

		if (eCivic != NO_CIVIC)
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_CITY_CELEBRATE", getNameKey(), GC.getCivicInfo(eCivic).getWeLoveTheKing());
			AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_WELOVEKING", MESSAGE_TYPE_MINOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_HAPPY_PERSON")->getPath(), GC.getCOLOR_WHITE(), getX(), getY(), true, true);
		}
	}
}


bool CvCity::isCitizensAutomated() const
{
	return m_bCitizensAutomated;
}


void CvCity::setCitizensAutomated(bool bNewValue)
{
	PROFILE_EXTRA_FUNC();
	if (isCitizensAutomated() != bNewValue)
	{
		m_bCitizensAutomated = bNewValue;

		if (isCitizensAutomated())
		{
			AI_assignWorkingPlots();
		}
		else
		{
			for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
			{
				setForceSpecialistCount(((SpecialistTypes)iI), 0);
			}
		}

		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(CitizenButtons_DIRTY_BIT, true);
		}
	}
}


bool CvCity::isProductionAutomated() const
{
	return m_bProductionAutomated;
}

void CvCity::setProductionAutomated(bool bNewValue)
{
	if (m_bProductionAutomated != bNewValue)
	{
		m_bProductionAutomated = bNewValue;

		if (bNewValue)
		{
			clearOrderQueue();
			AI_chooseProduction();
			gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		}
	}
}


bool CvCity::isWallOverride() const
{
	return m_bWallOverride;
}


void CvCity::setWallOverride(bool bOverride)
{
	if (isWallOverride() != bOverride)
	{
		m_bWallOverride = bOverride;

		setLayoutDirty(true);
	}
}


bool CvCity::isInfoDirty() const
{
	return m_bInfoDirty;
}


void CvCity::setInfoDirty(bool bNewValue)
{
	m_bInfoDirty = bNewValue;
}


bool CvCity::isLayoutDirty() const
{
	return m_bLayoutDirty;
}


void CvCity::setLayoutDirty(bool bNewValue)
{
	m_bLayoutDirty = bNewValue;
}


PlayerTypes CvCity::getPreviousOwner() const
{
	return m_ePreviousOwner;
}


void CvCity::setPreviousOwner(PlayerTypes eNewValue)
{
	m_ePreviousOwner = eNewValue;
}


PlayerTypes CvCity::getOriginalOwner() const
{
	return m_eOriginalOwner;
}


void CvCity::setOriginalOwner(PlayerTypes eNewValue)
{
	m_eOriginalOwner = eNewValue;
}


TeamTypes CvCity::getTeam() const
{
	return GET_PLAYER(getOwner()).getTeam();
}


CultureLevelTypes CvCity::getCultureLevel() const
{
	return m_eCultureLevel;
}

int CvCity::getCultureThreshold() const
{
	PROFILE_EXTRA_FUNC();
	const GameSpeedTypes eSpeed = GC.getGame().getGameSpeedType();
	const int iCulture = getCultureTimes100(getOwner()) / 100;
	const int iNumCultureLevels = GC.getNumCultureLevelInfos();

	for (int i = 0; i < iNumCultureLevels; i++)
	{
		const CvCultureLevelInfo& info = GC.getCultureLevelInfo(static_cast<CultureLevelTypes>(i));

		if (info.getLevel() > -1 && iCulture < info.getSpeedThreshold(eSpeed))
		{
			return info.getSpeedThreshold(eSpeed);
		}
	}
	return -1;
}


void CvCity::setCultureLevel(CultureLevelTypes eNewValue, bool bUpdatePlotGroups)
{
	PROFILE_FUNC();

	if (m_eCultureLevel == eNewValue)
	{
		return;
	}
	const CultureLevelTypes eOldValue = m_eCultureLevel;

	m_eCultureLevel = eNewValue;

	// Culture level change can change our radius requiring recalculation of best builds
	AI_markBestBuildValuesStale();

	// Border expansion alert
	if (GC.getGame().isFinalInitialized() && eNewValue > eOldValue && eNewValue > 1)
	{
		{
			AddDLLMessage(
				getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
				gDLL->getText("TXT_KEY_MISC_BORDERS_EXPANDED", getNameKey(), GC.getCultureLevelInfo(eNewValue).getTextKeyWide()),
				"AS2D_CULTUREEXPANDS", MESSAGE_TYPE_MINOR_EVENT,
				GC.getCommerceInfo(COMMERCE_CULTURE).getButton(),
				GC.getCOLOR_WHITE(), getX(), getY(), true, true
			);
		}
		// Afforess - Update Health and Happiness when culture expands
		updateFeatureHappiness();
		updateFeatureHealth();
		updateImprovementHealth();

		// Alert people if max culture level acquired in a known city
		if (getCultureThreshold() == -1)
		{
			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHumanPlayer())
				{
					if (isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
					{
						AddDLLMessage(
							(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(),
							gDLL->getText(
								"TXT_KEY_MISC_CULTURE_LEVEL",
								getNameKey(),
								GC.getCultureLevelInfo(eNewValue).getTextKeyWide()
							),
							"AS2D_CULTURELEVEL", MESSAGE_TYPE_MAJOR_EVENT,
							GC.getCommerceInfo(COMMERCE_CULTURE).getButton(),
							GC.getCOLOR_HIGHLIGHT_TEXT(), getX(), getY(), true, true
						);
					}
					else
					{
						AddDLLMessage(
							(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(),
							gDLL->getText(
								"TXT_KEY_MISC_CULTURE_LEVEL_UNKNOWN",
								GC.getCultureLevelInfo(eNewValue).getTextKeyWide()
							),
							"AS2D_CULTURELEVEL", MESSAGE_TYPE_MAJOR_EVENT,
							GC.getCommerceInfo(COMMERCE_CULTURE).getButton(), GC.getCOLOR_HIGHLIGHT_TEXT()
						);
					}
				}
			}
		}

		// ONEVENT - Culture growth
		CvEventReporter::getInstance().cultureExpansion(this, getOwner());

		//Stop Build Culture
		if (isHuman() && !isProductionAutomated() && isProductionProcess()
		&& GC.getProcessInfo(getProductionProcess()).getProductionToCommerceModifier(COMMERCE_CULTURE) > 0)
		{
			m_bPopProductionProcess = true;
		}
	}
	AI_updateBestBuild();
}

// Stores level as indexed by values in xml.
void CvCity::updateCultureLevel(bool bUpdatePlotGroups)
{
	PROFILE_EXTRA_FUNC();
	if (getCultureUpdateTimer() > 0)
	{
		return;
	}
	CvGameAI& GAME = GC.getGame();

	if (!isOccupation())
	{
		const GameSpeedTypes eSpeed = GAME.getGameSpeedType();
		const int iCulture = getCultureTimes100(getOwner()) / 100;

		// Will set culture level to that indexed by xml, but only if matches option of current game
		for (int iI = GC.getNumCultureLevelInfos() - 1; iI > 0; iI--)
		{
			const CvCultureLevelInfo& info = GC.getCultureLevelInfo(static_cast<CultureLevelTypes>(iI));

			if (info.getLevel() > -1 && iCulture >= info.getSpeedThreshold(eSpeed))
			{
				setCultureLevel(static_cast<CultureLevelTypes>(iI), bUpdatePlotGroups);
				return;
			}
		}
	}
	setCultureLevel((CultureLevelTypes)0, bUpdatePlotGroups);
}


int CvCity::getRiverPlotYield(YieldTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	return m_aiRiverPlotYield[eIndex];
}

void CvCity::changeRiverPlotYield(YieldTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiRiverPlotYield[eIndex] += iChange;

		updateYield();
	}
}


void CvCity::changeTerrainYieldChanges(const TerrainTypes eTerrain, const YieldArray& yields)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), eTerrain);
	{
		bool bNoChange = true;
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			if (yields[iI] != 0)
			{
				bNoChange = false;
				break;
			}
		}
		if (bNoChange)
		{
			FErrorMsg("Redundant function call");
			return;
		}
	}
	std::map<short, YieldArray>::const_iterator itr = m_terrainYieldChanges.find((short)eTerrain);

	if (itr == m_terrainYieldChanges.end())
	{
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			if (yields[iI] != 0)
			{
				m_terrainYieldChanges.insert(std::make_pair((short)eTerrain, yields));
				break;
			}
		}
	}
	else
	{
		bool bEmpty = true;

		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			m_terrainYieldChanges[itr->first][iI] += yields[iI];

			if (bEmpty && m_terrainYieldChanges[itr->first][iI] != 0)
			{
				bEmpty = false;
			}
		}
		if (bEmpty)
		{
			m_terrainYieldChanges.erase(itr->first);
		}
	}
	updateYield();
}

int CvCity::getTerrainYieldChange(const TerrainTypes eTerrain, const YieldTypes eYield) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), eTerrain);
	std::map<short, YieldArray>::const_iterator itr = m_terrainYieldChanges.find((short)eTerrain);
	return itr != m_terrainYieldChanges.end() ? itr->second[eYield] : 0;
}


void CvCity::changePlotYieldChanges(const PlotTypes ePlot, const YieldArray& yields)
{
	PROFILE_EXTRA_FUNC();
	{
		bool bNoChange = true;
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			if (yields[iI] != 0)
			{
				bNoChange = false;
				break;
			}
		}
		if (bNoChange)
		{
			FErrorMsg("Redundant function call");
			return;
		}
	}
	std::map<short, YieldArray>::const_iterator itr = m_plotYieldChanges.find((short)ePlot);

	if (itr == m_plotYieldChanges.end())
	{
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			if (yields[iI] != 0)
			{
				m_plotYieldChanges.insert(std::make_pair((short)ePlot, yields));
				break;
			}
		}
	}
	else
	{
		bool bEmpty = true;

		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			m_plotYieldChanges[itr->first][iI] += yields[iI];

			if (bEmpty && m_plotYieldChanges[itr->first][iI] != 0)
			{
				bEmpty = false;
			}
		}
		if (bEmpty)
		{
			m_plotYieldChanges.erase(itr->first);
		}
	}
	updateYield();
}

int CvCity::getPlotYieldChange(const PlotTypes ePlot, const YieldTypes eYield) const
{
	std::map<short, YieldArray>::const_iterator itr = m_plotYieldChanges.find((short)ePlot);
	return itr != m_plotYieldChanges.end() ? itr->second[eYield] : 0;
}


int CvCity::getYieldChangeAt(const CvPlot* pPlot, const YieldTypes eYield) const
{
	int iYield = (
		getPlotYieldChange(pPlot->getPlotType(), eYield)
		+
		getTerrainYieldChange(pPlot->getTerrainType(), eYield)
	);
	if (pPlot->isRiver())
	{
		iYield += getRiverPlotYield(eYield);
	}
	return iYield;
}


// Toffer - Base yield directly produced by building, not indirectly through trade, plots, specialists, modifiers, etc.
int CvCity::getBaseYieldRateFromBuilding100(const YieldTypes eYield, const BuildingTypes eBuilding) const
{
	const CvBuildingInfo& building = GC.getBuildingInfo(eBuilding);
	return (
		building.getYieldChange(eYield) * 100
		+
		building.getYieldPerPopChange(eYield) * getPopulation()
		+
		GET_TEAM(getTeam()).getBuildingYieldTechChange(eYield, eBuilding)
		+
		getBuildingYieldChange(eBuilding, eYield)
	);
}

/*
 * Returns the total additional yield that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalYieldByBuilding(YieldTypes eIndex, BuildingTypes eBuilding, bool bFilter) const
{
	PROFILE_EXTRA_FUNC();
	const CvBuildingInfo& building = GC.getBuildingInfo(eBuilding);

	const int iBase = getBaseYieldRate(eIndex);
	const int iMod = getBaseYieldRateModifier(eIndex);

	int iAddBase = getAdditionalBaseYieldByBuilding(eIndex, eBuilding);
	int iAddExtra = getAdditionalExtraYieldByBuilding(eIndex, eBuilding);
	int iAddBaseMod = getAdditionalBaseYieldModifierByBuilding(eIndex, eBuilding, bFilter);

	for (int iI = 0; iI < building.getNumReplacedBuilding(); iI++)
	{
		const BuildingTypes eBuildingX = static_cast<BuildingTypes>(building.getReplacedBuilding(iI));

		if (hasFullyActiveBuilding(eBuildingX))
		{
			iAddBase -= getAdditionalBaseYieldByBuilding(eIndex, eBuildingX);
			iAddExtra -= getAdditionalExtraYieldByBuilding(eIndex, eBuildingX);
			iAddBaseMod -= getAdditionalBaseYieldModifierByBuilding(eIndex, eBuildingX, bFilter);
		}
	}
	return
	(
		(iBase + iAddBase) * (iMod + iAddBaseMod) / 100
		-
		iBase * iMod / 100
		+
		iAddExtra
	);
}

/*
 * Returns the additional yield rate that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalBaseYieldByBuilding(YieldTypes eIndex, BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eBuilding);

	int iBaseYield = 0;

	const CvBuildingInfo& building = GC.getBuildingInfo(eBuilding);

	if (building.getRiverPlotYieldChange(eIndex) != 0)
	{
		const int iChange = building.getRiverPlotYieldChange(eIndex);
		for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
		{
			if (isWorkingPlot(iI) && getCityIndexPlot(iI)->isRiver())
			{
				iBaseYield += iChange;
			}
		}
	}
	return iBaseYield;
}

int CvCity::getAdditionalExtraYieldByBuilding(YieldTypes eIndex, BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	const CvBuildingInfo& building = GC.getBuildingInfo(eBuilding);

	int iExtraYield = getBaseYieldRateFromBuilding100(eIndex, eBuilding) / 100;

	// Trade
	const int iPlayerTradeYieldModifier = GET_PLAYER(getOwner()).getTradeYieldModifier(eIndex);
	if (iPlayerTradeYieldModifier > 0 && (building.getTradeRouteModifier() != 0 || building.getForeignTradeRouteModifier() != 0))
	{
		int iTotalTradeYield = 0;
		int iNewTotalTradeYield = 0;
		// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
		int iTradeProfitDivisor = 100;
#else
		int iTradeProfitDivisor = 10000;
#endif
		// BUG - Fractional Trade Routes - end

		for (int iI = 0; iI < getTradeRoutes(); ++iI)
		{
			const CvCity* pCity = getTradeCity(iI);
			if (pCity)
			{
				const int iTradeProfit = getBaseTradeProfit(pCity);
				int iTradeModifier = totalTradeModifier(pCity);

				iTotalTradeYield += iTradeProfit * iTradeModifier / iTradeProfitDivisor * iPlayerTradeYieldModifier / 100;

				iTradeModifier += building.getTradeRouteModifier();
				if (pCity->getOwner() != getOwner())
				{
					iTradeModifier += building.getForeignTradeRouteModifier();
				}
				iNewTotalTradeYield += iTradeProfit * iTradeModifier / iTradeProfitDivisor * iPlayerTradeYieldModifier / 100;
			}
		}
#ifdef _MOD_FRACTRADE
		iTotalTradeYield /= 100;
		iNewTotalTradeYield /= 100;
#endif
		iExtraYield += iNewTotalTradeYield - iTotalTradeYield;
	}

	// Specialists
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); ++iI)
	{
		if (building.getFreeSpecialistCount((SpecialistTypes)iI) != 0)
		{
			iExtraYield += building.getFreeSpecialistCount((SpecialistTypes)iI) * getYieldBySpecialist(eIndex, (SpecialistTypes)iI);
		}
	}

//	// Bonuses
//	for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
//	{
//		if (hasBonus((BonusTypes)iI))
//		{
//			iExtraYield += building.getBonusYieldChanges(iI, eIndex);
//		}
//		if (building.getVicinityBonusYieldChanges(iI, eIndex) != 0 && hasVicinityBonus((BonusTypes)iI))
//		{
//			iExtraYield += building.getVicinityBonusYieldChanges(iI, eIndex);
//		}
//	}

	for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
    {
       if (eBuilding == (BuildingTypes)GC.getInfoTypeForString("BUILDING_STORAGE"))
       {
           int iBonusCount = GET_PLAYER(getOwner()).getNumAvailableBonuses((BonusTypes)iI);

           if (iBonusCount > 0)
           {
               // Add empire-wide bonus yields
               iExtraYield += iBonusCount * building.getBonusYieldChanges(iI, eIndex);
           }
       }
        else
        {
            if (hasBonus((BonusTypes)iI))
            {
                iExtraYield += building.getBonusYieldChanges(iI, eIndex);
            }

            if (building.getVicinityBonusYieldChanges(iI, eIndex) != 0 && hasVicinityBonus((BonusTypes)iI))
            {
                iExtraYield += building.getVicinityBonusYieldChanges(iI, eIndex);
            }
        }
    }

	const int iTradeRoutes = building.getGlobalTradeRoutes() + building.getCoastalTradeRoutes() + building.getTradeRoutes();
	if (iTradeRoutes != 0)
	{
		int* paiTradeYields = new int[NUM_YIELD_TYPES];
		calculateExtraTradeRouteProfit(iTradeRoutes, paiTradeYields);
		iExtraYield += paiTradeYields[eIndex];
		SAFE_DELETE_ARRAY(paiTradeYields);
	}

	for (int iI = 1; iI < building.getFreeSpecialist() + 1; iI++)
	{
		const SpecialistTypes eNewSpecialist = getBestSpecialist(iI);
		if (eNewSpecialist == NO_SPECIALIST) break;
		iExtraYield += specialistYield(eNewSpecialist, eIndex);
	}

	// This could potentially affect base yield, but let's always pretend there's a specialist to remove.
	if (building.getNumPopulationEmployed() > 0)
	{
		int* paiCommerce = new int[NUM_COMMERCE_TYPES];
		int* paiYield = new int[NUM_YIELD_TYPES];
		int iGreatPeopleRate;
		int iHappiness;
		int iHealthiness;
		removeWorstCitizenActualEffects(building.getNumPopulationEmployed(), iGreatPeopleRate, iHappiness, iHealthiness, paiYield, paiCommerce);
		iExtraYield += paiYield[eIndex];
		SAFE_DELETE_ARRAY(paiCommerce);
		SAFE_DELETE_ARRAY(paiYield);
	}

	return iExtraYield;
}

/*
 * Returns the additional yield rate modifier that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalBaseYieldModifierByBuilding(YieldTypes eYield, BuildingTypes eBuilding, bool bFilter) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eYield);
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eBuilding);

	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	int iYieldMod = kBuilding.getYieldModifier(eYield);

	if (isPower())
	{
		iYieldMod += kBuilding.getPowerYieldModifier(eYield);
	}
	else if (kBuilding.isPower() || (kBuilding.getPowerBonus() != NO_BONUS && hasBonus((BonusTypes)kBuilding.getPowerBonus())))
	{
		foreach_(const BuildingTypes eTypeX, getHasBuildings())
		{
			if (!isDisabledBuilding(eTypeX))
			{
				iYieldMod += GC.getBuildingInfo(eTypeX).getPowerYieldModifier(eYield);
			}
		}
	}

	if (eYield == YIELD_PRODUCTION && !bFilter)
	{
		iYieldMod += kBuilding.getMilitaryProductionModifier() / 2; // AIAndy: It does not make sense to count the production increases for specific domains fully
		iYieldMod += kBuilding.getSpaceProductionModifier() / 4;
		iYieldMod += kBuilding.getGlobalSpaceProductionModifier() / 2;

		int iMaxModifier = 0;
		for (int i = 0; i < NUM_DOMAIN_TYPES; i++)
		{
			iMaxModifier = std::max(iMaxModifier, kBuilding.getDomainProductionModifier((DomainTypes)i));
		}
		iYieldMod += iMaxModifier / 4;
	}
	for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
	{
		if (hasBonus((BonusTypes)iI))
		{
			iYieldMod += kBuilding.getBonusYieldModifier(iI, eYield);
		}
	}
	foreach_(const TechArray& pair, kBuilding.getTechYieldModifiers())
	{
		if (GET_TEAM(getTeam()).isHasTech(pair.first))
		{
			iYieldMod += pair.second[eYield];
		}
	}
	return iYieldMod;
}


int CvCity::getYieldBySpecialist(YieldTypes eIndex, SpecialistTypes eSpecialist) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eSpecialist);
	return (
		GC.getSpecialistInfo(eSpecialist).getYieldChange(eIndex)
		+ GET_PLAYER(getOwner()).getExtraSpecialistYield(eSpecialist, eIndex)
		+ GET_PLAYER(getOwner()).getSpecialistYieldPercentChanges(eSpecialist, eIndex) / 100
	);
}

// note: player->invalidateYieldRankCache() must be called for anything that is checked here
// so if any extra checked things are added here, the cache needs to be invalidated
int CvCity::getBaseYieldRateModifier(YieldTypes eIndex, int iExtra) const
{
	int iModifier = (
		100 + iExtra
		+ getBonusYieldRateModifier(eIndex)
		+ getBuildingYieldModifier(eIndex)
		// Event modifiers
		+ getYieldRateModifier(eIndex) + GET_PLAYER(getOwner()).getYieldRateModifier(eIndex)
	);
	if (isPower())
	{
		iModifier += getPowerYieldRateModifier(eIndex);
	}
	if (area() != NULL)
	{
		iModifier += area()->getYieldRateModifier(getOwner(), eIndex);
	}
	if (isCapital())
	{
		iModifier += GET_PLAYER(getOwner()).getCapitalYieldRateModifier(eIndex);
	}
	return std::max(0, iModifier);
}

int CvCity::getYieldRate(const YieldTypes eYield) const
{
	return getYieldRate100(eYield) / 100;
}

int CvCity::getYieldRate100(const YieldTypes eYield) const
{
	PROFILE_FUNC();
	return std::min(CITY_MAX_YIELD_RATE,std::max(100, getBaseYieldRate(eYield) * getBaseYieldRateModifier(eYield) + 100 * getExtraYield(eYield)));
}

int CvCity::getPlotYield(YieldTypes eIndex)	const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	return m_aiBaseYieldRate[eIndex];
}

void CvCity::changePlotYield(YieldTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiBaseYieldRate[eIndex] += iChange;
		onYieldChange();
	}
}


void CvCity::changeBuildingYieldModifier(YieldTypes eYield, int iChange)
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eYield);

	if (iChange != 0)
	{
		m_buildingYieldMod[eYield] += iChange;

		GET_PLAYER(getOwner()).invalidateYieldRankCache(eYield);

		if (eYield == YIELD_COMMERCE)
		{
			setCommerceDirty();
		}
		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

int CvCity::getBuildingYieldModifier(YieldTypes eYield) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eYield);
	return m_buildingYieldMod[eYield];
}


void CvCity::changeBuildingExtraYield100(YieldTypes eYield, int iChange)
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eYield);

	if (iChange != 0)
	{
		m_buildingExtraYield100[eYield] += iChange;
		onYieldChange();
	}
}

int CvCity::getBuildingExtraYield100(YieldTypes eYield) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eYield);
	return m_buildingExtraYield100[eYield];
}


int CvCity::getExtraYield100(YieldTypes eYield) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eYield);
	return (
		m_aiExtraYield[eYield] * 100
		+
		getBuildingExtraYield100(eYield)
		+
		getBaseYieldPerPopRate(eYield) * getPopulation()
	);
}

void CvCity::changeExtraYield(YieldTypes eYield, int iChange)
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eYield);

	if (iChange != 0)
	{
		m_aiExtraYield[eYield] += iChange;
		onYieldChange();
	}
}

int CvCity::getExtraYield(YieldTypes eYield) const
{
	return getExtraYield100(eYield) / 100;
}


void CvCity::onYieldChange()
{
#ifdef YIELD_VALUE_CACHING
	ClearYieldValueCache();
#endif
	setCommerceDirty();

	if (getTeam() == GC.getGame().getActiveTeam())
	{
		setInfoDirty(true);
	}
	if (isCitySelected())
	{
		gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	}
}

int CvCity::getBaseYieldPerPopRate(YieldTypes eIndex)	const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);

	return m_aiBaseYieldPerPopRate[eIndex];
}

void CvCity::setBaseYieldPerPopRate(YieldTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);

	if (m_aiBaseYieldPerPopRate[eIndex] != iNewValue)
	{
		m_aiBaseYieldPerPopRate[eIndex] = iNewValue;
		onYieldChange();
	}
}

void CvCity::changeBaseYieldPerPopRate(YieldTypes eIndex, int iChange)
{
	setBaseYieldPerPopRate(eIndex, (getBaseYieldPerPopRate(eIndex) + iChange));
}

int CvCity::getYieldRateModifier(YieldTypes eIndex)	const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	return m_aiYieldRateModifier[eIndex];
}


void CvCity::changeYieldRateModifier(YieldTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiYieldRateModifier[eIndex] += iChange;

		GET_PLAYER(getOwner()).invalidateYieldRankCache(eIndex);

		if (eIndex == YIELD_COMMERCE)
		{
			setCommerceDirty();
		}

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}


int CvCity::getPowerYieldRateModifier(YieldTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	return m_aiPowerYieldRateModifier[eIndex];
}


void CvCity::changePowerYieldRateModifier(YieldTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiPowerYieldRateModifier[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(getYieldRate100(eIndex));

		GET_PLAYER(getOwner()).invalidateYieldRankCache(eIndex);

		if (eIndex == YIELD_COMMERCE)
		{
			setCommerceDirty();
		}

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}


int CvCity::getBonusYieldRateModifier(YieldTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	return m_aiBonusYieldRateModifier[eIndex];
}


void CvCity::changeBonusYieldRateModifier(YieldTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiBonusYieldRateModifier[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(getYieldRate100(eIndex));

		GET_PLAYER(getOwner()).invalidateYieldRankCache(eIndex);

		if (eIndex == YIELD_COMMERCE)
		{
			setCommerceDirty();
		}

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

void CvCity::setTradeYield(YieldTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);

	const int iOldValue = m_aiTradeYield[eIndex];

	if (iOldValue != iNewValue)
	{
		m_aiTradeYield[eIndex] = iNewValue;
		FASSERT_NOT_NEGATIVE(m_aiTradeYield[eIndex]);
	}
}

int CvCity::getTradeYield(YieldTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	return m_aiTradeYield[eIndex];
}


int CvCity::totalTradeModifier(const CvCity* pOtherCity) const
{
	PROFILE_EXTRA_FUNC();
	int iModifier = 100;

	iModifier += getTradeRouteModifier();
	iModifier += getPopulationTradeModifier();
	iModifier += GET_TEAM(getTeam()).getTradeModifier();

	if (isConnectedToCapital())
	{
		iModifier += GC.getCAPITAL_TRADE_MODIFIER();
	}

	if (NULL != pOtherCity)
	{
		if (area() != pOtherCity->area())
		{
			iModifier += GC.getOVERSEAS_TRADE_MODIFIER();
		}

		if (getTeam() != pOtherCity->getTeam())
		{
			iModifier += getForeignTradeRouteModifier();
			iModifier += GET_PLAYER(getOwner()).getForeignTradeRouteModifier();
			iModifier += GET_TEAM(getTeam()).getForeignTradeModifier();

			const CvPlayer& kOtherPlayer = GET_PLAYER(pOtherCity->getOwner());
			const CvPlayer& kPlayer = GET_PLAYER(getOwner());
			for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
			{
				if (kPlayer.getCivics((CivicOptionTypes)iI) == kOtherPlayer.getCivics((CivicOptionTypes)iI))
				{
					iModifier += GC.getCivicInfo(kPlayer.getCivics((CivicOptionTypes)iI)).getSharedCivicTradeRouteModifier();
				}
			}
			iModifier += getPeaceTradeModifier(pOtherCity->getTeam());
		}
	}

	return iModifier;
}

int CvCity::getPopulationTradeModifier() const
{
	return std::max(0, (getPopulation() + GC.getOUR_POPULATION_TRADE_MODIFIER_OFFSET()) * GC.getOUR_POPULATION_TRADE_MODIFIER());
}

int CvCity::getPeaceTradeModifier(TeamTypes eTeam) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);
	FAssert(eTeam != getTeam());

	if (atWar(eTeam, getTeam()))
	{
		return 0;
	}

	const int iPeaceTurns = std::min(GC.getFOREIGN_TRADE_FULL_CREDIT_PEACE_TURNS(), GET_TEAM(getTeam()).AI_getAtPeaceCounter(eTeam));

	if (GC.getGame().getElapsedGameTurns() <= iPeaceTurns)
	{
		return GC.getFOREIGN_TRADE_MODIFIER();
	}

	return ((GC.getFOREIGN_TRADE_MODIFIER() * iPeaceTurns) / std::max(1, GC.getFOREIGN_TRADE_FULL_CREDIT_PEACE_TURNS()));
}

int CvCity::getBaseTradeProfit(const CvCity* pCity) const
{
	int iProfit = std::min(pCity->getPopulation() * GC.getTHEIR_POPULATION_TRADE_PERCENT(), plotDistance(getX(), getY(), pCity->getX(), pCity->getY()) * GC.getWorldInfo(GC.getMap().getWorldSize()).getTradeProfitPercent());

	iProfit *= GC.getTRADE_PROFIT_PERCENT();
	iProfit /= 100;

	iProfit = std::max(100, iProfit);

	return iProfit;
}

// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE

// Note: getBaseTradeProfit() already returns a times-100 value.

/*
 * Returns the fractional (times 100) trade profit for the route to the given city.
 */
int CvCity::calculateTradeProfitTimes100(const CvCity* pCity) const
{
	int iProfit = getBaseTradeProfit(pCity);

	iProfit *= totalTradeModifier(pCity);
	iProfit /= 100;

	return iProfit;
}

/*
 * Returns the truncated trade profit for the route to the given city.
 *
 * This function is kept only for old Python code.
 */
int CvCity::calculateTradeProfit(const CvCity* pCity) const
{
	return calculateTradeProfitTimes100(pCity) / 100;
}

#else

// unchanged

int CvCity::calculateTradeProfit(const CvCity* pCity) const
{
	int iProfit = getBaseTradeProfit(pCity);

	iProfit *= totalTradeModifier(pCity);
	iProfit /= 10000;

	return iProfit;
}

#endif
// BUG - Fractional Trade Routes - end

int CvCity::calculateTradeYield(YieldTypes eIndex, int iTradeProfit) const
{
	if (iTradeProfit != 0 && GET_PLAYER(getOwner()).getTradeYieldModifier(eIndex) > 0)
	{
		return iTradeProfit * GET_PLAYER(getOwner()).getTradeYieldModifier(eIndex) / 100;
	}
	return 0;
}

// BUG - Trade Totals - start
/*
 * Adds the yield and count for each trade route with eWithPlayer.
 *
 * The yield and counts are not reset to zero.
 * If Fractional Trade Routes is enabled and bRound is false, or if bBase if true, the yield values are left times 100.
 */
void CvCity::calculateTradeTotals(YieldTypes eIndex, int& iDomesticYield, int& iDomesticRoutes, int& iForeignYield, int& iForeignRoutes, PlayerTypes eWithPlayer, bool bRound, bool bBase) const
{
	PROFILE_EXTRA_FUNC();
	if (!isDisorder())
	{
		int iCityDomesticYield = 0;
		int iCityDomesticRoutes = 0;
		int iCityForeignYield = 0;
		int iCityForeignRoutes = 0;
		const int iNumTradeRoutes = getTradeRoutes();
		const PlayerTypes ePlayer = getOwner();

		for (int iI = 0; iI < iNumTradeRoutes; ++iI)
		{
			const CvCity* pTradeCity = getTradeCity(iI);
			if (pTradeCity && pTradeCity->getOwner() >= 0 && (NO_PLAYER == eWithPlayer || pTradeCity->getOwner() == eWithPlayer))
			{
				int iTradeYield;

				if (bBase)
				{
					iTradeYield = getBaseTradeProfit(pTradeCity);
				}
				else
				{
					// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
					const int iTradeProfit = calculateTradeProfitTimes100(pTradeCity);
#else
					const int iTradeProfit = calculateTradeProfit(pTradeCity);
#endif
					// BUG - Fractional Trade Routes - end
					iTradeYield = calculateTradeYield(YIELD_COMMERCE, iTradeProfit);
				}

				if (pTradeCity->getOwner() == ePlayer)
				{
					iCityDomesticYield += iTradeYield;
					iCityDomesticRoutes++;
				}
				else
				{
					iCityForeignYield += iTradeYield;
					iCityForeignRoutes++;
				}
			}
		}

		// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
		if (bRound)
		{
			iDomesticYield += iCityDomesticYield / 100;
			iDomesticRoutes += iCityDomesticRoutes / 100;
			iForeignYield += iCityForeignYield / 100;
			iForeignRoutes += iCityForeignRoutes / 100;
		}
		else
#endif
			// BUG - Fractional Trade Routes - end
		{
			iDomesticYield += iCityDomesticYield;
			iDomesticRoutes += iCityDomesticRoutes;
			iForeignYield += iCityForeignYield;
			iForeignRoutes += iCityForeignRoutes;
		}
	}
}

/*
 * Returns the total trade yield.
 *
 * If Fractional Trade Routes is enabled or bBase is true, the yield value is left times 100.
 * UNUSED
 */
int CvCity::calculateTotalTradeYield(YieldTypes eIndex, PlayerTypes eWithPlayer, bool bRound, bool bBase) const
{
	int iDomesticYield = 0;
	int iDomesticRoutes = 0;
	int iForeignYield = 0;
	int iForeignRoutes = 0;

	calculateTradeTotals(eIndex, iDomesticYield, iDomesticRoutes, iForeignYield, iForeignRoutes, eWithPlayer, bRound, bBase);
	return iDomesticYield + iForeignRoutes;
}
// BUG - Trade Totals - end


int CvCity::getExtraSpecialistYield(YieldTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	return m_aiExtraSpecialistYield[eIndex];
}


int CvCity::getExtraSpecialistYield(YieldTypes eIndex, SpecialistTypes eSpecialist) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eSpecialist);
	return
	(
		specialistCount(eSpecialist) * (
			getLocalSpecialistExtraYield(eSpecialist, eIndex)
			+ GET_PLAYER(getOwner()).getExtraSpecialistYield(eSpecialist, eIndex)
			+ GET_PLAYER(getOwner()).getSpecialistExtraYield(eIndex)
		)
	);
}


void CvCity::updateExtraSpecialistYield(YieldTypes eYield)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eYield);

	const int iOldYield = getExtraSpecialistYield(eYield);
	int iNewYield = 0;

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		iNewYield += getExtraSpecialistYield(eYield, (SpecialistTypes)iI);
	}

	if (iOldYield != iNewYield)
	{
		m_aiExtraSpecialistYield[eYield] = iNewYield;

		changeExtraYield(eYield, iNewYield - iOldYield);
	}
}


void CvCity::updateExtraSpecialistYield()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		updateExtraSpecialistYield((YieldTypes)iI);
	}
}

int CvCity::getExtraSpecialistCommerceTotal(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_aiExtraSpecialistCommerce[eIndex];
}

int CvCity::getExtraSpecialistCommerce(CommerceTypes eIndex, SpecialistTypes eSpecialist) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eSpecialist);
	return (specialistCount(eSpecialist) * (getLocalSpecialistExtraCommerce(eSpecialist, eIndex) + GET_PLAYER(getOwner()).getExtraSpecialistCommerce(eSpecialist, eIndex) + GET_PLAYER(getOwner()).getSpecialistExtraCommerce(eIndex)));
}

void CvCity::updateExtraSpecialistCommerce(CommerceTypes eCommerce)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eCommerce);

	int iOldCommerce = getExtraSpecialistCommerceTotal(eCommerce);

	int iNewCommerce = 0;

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		iNewCommerce += getExtraSpecialistCommerce(eCommerce, ((SpecialistTypes)iI));
	}

	if (iOldCommerce != iNewCommerce)
	{
		m_aiExtraSpecialistCommerce[eCommerce] = iNewCommerce;
	}
}

void CvCity::updateExtraSpecialistCommerce()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		updateExtraSpecialistCommerce((CommerceTypes)iI);
	}

	setCommerceDirty();

	if (getTeam() == GC.getGame().getActiveTeam())
	{
		setInfoDirty(true);
	}
	if (isCitySelected())
	{
		gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	}
}


int CvCity::getCommerceRate(CommerceTypes eIndex) const
{
	return getCommerceRateTimes100(eIndex) / 100;
}

int CvCity::getCommerceRateTimes100(CommerceTypes eIndex) const
{
	PROFILE_FUNC();
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (m_abCommerceRateDirty[eIndex])
	{
		updateCommerce(eIndex);
	}
	return m_aiCommerceRate[eIndex];
}


int CvCity::getCommerceFromPercent(CommerceTypes eIndex) const
{
	const int iYieldRate = getYieldRate100(YIELD_COMMERCE);

	if (eIndex == COMMERCE_GOLD)
	{
		return std::max(
			0,
			(
				iYieldRate * 100
				- iYieldRate * GET_PLAYER(getOwner()).getCommercePercent(COMMERCE_RESEARCH)
				- iYieldRate * GET_PLAYER(getOwner()).getCommercePercent(COMMERCE_CULTURE)
				- iYieldRate * GET_PLAYER(getOwner()).getCommercePercent(COMMERCE_ESPIONAGE)
			)
			/ 100
		);
	}
	return iYieldRate * GET_PLAYER(getOwner()).getCommercePercent(eIndex) / 100;
}


int CvCity::getBaseCommerceRate(CommerceTypes eIndex) const
{
	return getBaseCommerceRateTimes100(eIndex) / 100;
}

int CvCity::getBaseCommerceRateTimes100(CommerceTypes eIndex) const
{
	PROFILE_FUNC();

	if (m_abCommerceRateDirty[eIndex])
	{
		updateCommerce(eIndex);
	}

	//STEP 1 : Slider + remaining steps.
	return getCommerceFromPercent(eIndex) + getBaseCommerceRateExtra(eIndex);
}

int CvCity::getBaseCommerceRateExtra(CommerceTypes eIndex) const
{
	//STEP 1 : Slider
	//STEP 2 : Commerce Changes from specialists
	int iBaseExtraRate = 100 * (getSpecialistCommerce(eIndex) + getExtraSpecialistCommerceTotal(eIndex));

	//STEP 3 : Religion Commerce
	iBaseExtraRate += 100 * getReligionCommerce(eIndex);

	//STEP 4 : Corporation Commerce
	iBaseExtraRate += 100 * getCorporationCommerce(eIndex);

	//STEP 5 : Building Commerce
	iBaseExtraRate += getBuildingCommerce100(eIndex);

	//STEP 6 : Free City Commerce (player tallied from civics/traits a change value to all cities commerce output)
	iBaseExtraRate += GET_PLAYER(getOwner()).getExtraCommerce100(eIndex);

	//STEP 7 : Minted Commerce
	if (eIndex == COMMERCE_GOLD)
	{
		iBaseExtraRate += getMintedCommerceTimes100();
	}

	//STEP 8 : Golden Age Base Commerce Changes (usually trait driven though it might be interesting to enable this on civics.)
	if (GET_PLAYER(getOwner()).isGoldenAge())
	{
		iBaseExtraRate += 100 * GET_PLAYER(getOwner()).getGoldenAgeCommerce(eIndex);
	}
	return iBaseExtraRate;
}

int CvCity::getCommerceRateAtSliderPercent(CommerceTypes eIndex, int iSliderPercent) const
{
	PROFILE_FUNC();

	if (isDisorder())
	{
		return 0;
	}
	if (m_abCommerceRateDirty[eIndex])
	{
		updateCommerce(eIndex);
	}
	int iRate = std::min<int>(CITY_MAX_YIELD_RATE100, getYieldRate100(YIELD_COMMERCE));
	int iExtraRate = std::min<int>(CITY_MAX_YIELD_RATE100, getBaseCommerceRateExtra(eIndex));

	iRate = iRate * iSliderPercent / 100 + iExtraRate;

	// Don't apply rate modifiers to negative commerce or you get counter-intuitive results
	//	like intelligence agencies making your negative espionage worse!
	if (iRate < CITY_MAX_YIELD_RATE)
	{
		if (iRate > 0)
		{
			iRate = iRate * getTotalCommerceRateModifier(eIndex) / 100;
		}
		else
		{
			iRate = iRate * 100 / getTotalCommerceRateModifier(eIndex);
		}
		iRate += getYieldRate(YIELD_PRODUCTION) * getProductionToCommerceModifier(eIndex);
	}

	// Culture and science cannot be negative
	if (iRate < 0 && (eIndex == COMMERCE_CULTURE || eIndex == COMMERCE_RESEARCH))
	{
		return 0;
	}
	if (iRate < MIN_TOL_FALSE_ACCUMULATE)
		return CITY_MAX_YIELD_RATE;
	return  std::min(CITY_MAX_YIELD_RATE,iRate);
}

int CvCity::getTotalCommerceRateModifier(CommerceTypes eIndex) const
{
	PROFILE_FUNC();

	if (m_totalCommerceRateModifier[eIndex] == MIN_INT)
	{
		int iTotal = 100;
		//STEP 1 : Bonus Commerce Rate Modifier
		iTotal += calculateBonusCommerceRateModifier(eIndex);
		//STEP 2 : Buildings
		iTotal += getBuildingCommerceModifier(eIndex);

		//STEP 2.5 : Player level building effects
		iTotal += GET_PLAYER(getOwner()).getCommerceRateModifierfromBuildings(eIndex);

		//STEP 3 : Events
		iTotal += getCommerceRateModifier(eIndex) + GET_PLAYER(getOwner()).getCommerceRateModifierfromEvents(eIndex);

		//STEP 4 : Player level
		//(Noting subtractions so it's easier to follow in comparison to the Text Manager.)
		//Aside from those noted, Player level modifiers also include Civics, Traits, and Techs
		//Events and buildings were isolated out so they could be expressed in the above categories
		iTotal += (
			GET_PLAYER(getOwner()).getCommerceRateModifier(eIndex)
			- GET_PLAYER(getOwner()).getCommerceRateModifierfromEvents(eIndex)
			- GET_PLAYER(getOwner()).getCommerceRateModifierfromBuildings(eIndex)
		);
		//STEP 5 : Capital Modifier
		if (isCapital())
		{
			iTotal += GET_PLAYER(getOwner()).getCapitalCommerceRateModifier(eIndex);
		}
		m_totalCommerceRateModifier[eIndex] = std::max(1, iTotal);
	}

	return m_totalCommerceRateModifier[eIndex];
}


void CvCity::setCommerceModifierDirty(CommerceTypes eCommerce)
{
	m_totalCommerceRateModifier[eCommerce] = MIN_INT;

	setCommerceDirty(eCommerce);
}

void CvCity::setCommerceDirty(CommerceTypes eCommerce)
{
	PROFILE_EXTRA_FUNC();
	if (eCommerce == NO_COMMERCE)
	{
		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			setCommerceDirty((CommerceTypes)iI);
		}
	}
	else
	{
		m_abCommerceRateDirty[eCommerce] = true;
		if (getOwner() != NO_PLAYER)
		{
			GET_PLAYER(getOwner()).setCommerceDirty(eCommerce, true);
		}
	}
}

void CvCity::updateCommerce(CommerceTypes eIndex, bool bForce) const
{
	PROFILE_EXTRA_FUNC();
	if (!GC.getGame().isRecalculatingModifiers())
	{
		if (eIndex == NO_COMMERCE)
		{
			GET_PLAYER(getOwner()).invalidateYieldRankCache();

			for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
			{
				updateCommerce((CommerceTypes)iI, bForce);
			}
		}
		else
		{
			FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

			if (bForce || m_abCommerceRateDirty[eIndex])
			{
				m_abCommerceRateDirty[eIndex] = false;
				const int iOldCommerce = m_aiCommerceRate[eIndex];
				const int iNewCommerce = getCommerceRateAtSliderPercent(eIndex, GET_PLAYER(getOwner()).getCommercePercent(eIndex));

				if (iOldCommerce != iNewCommerce)
				{
					m_aiCommerceRate[eIndex] = iNewCommerce;

					GET_PLAYER(getOwner()).invalidateCommerceRankCache(eIndex);

					GET_PLAYER(getOwner()).changeCommerceRate(eIndex, iNewCommerce - iOldCommerce);

					if (isCitySelected())
					{
						gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
						gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
					}
				}
			}
		}
	}
}

int CvCity::getProductionToCommerceModifier(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_aiProductionToCommerceModifier[eIndex];
}


void CvCity::changeProductionToCommerceModifier(CommerceTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiProductionToCommerceModifier[eIndex] += iChange;

		setCommerceDirty(eIndex);
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
	}
}


int CvCity::getBuildingCommerce(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_aiBuildingCommerce[eIndex];
}

int CvCity::getBuildingCommerce100(CommerceTypes eIndex) const
{
	return (
		100 * getBuildingCommerce(eIndex)
		+ getBonusCommercePercentChanges(eIndex)
		+ getBuildingCommerceTechChange(eIndex)
		+ getCommercePerPopFromBuildings(eIndex) * getPopulation()
	);
}

int CvCity::getBuildingCommerceByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding, const bool bFull, const bool bTestVisible) const
{
	PROFILE_FUNC();

	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eBuilding);

	if (!isActiveBuilding(eBuilding) && !bTestVisible)
		return 0;

	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	const CvPlayer& kOwner = GET_PLAYER(getOwner());
	const CvTeam& kTeam = GET_TEAM(getTeam());

	int iCommerce = 0;

	if (!isReligiouslyLimitedBuilding(eBuilding))
	{
		int iBaseCommerceChange = kBuilding.getCommerceChange(eIndex);

		if (eIndex == COMMERCE_GOLD && iBaseCommerceChange < 0 && GC.getTREAT_NEGATIVE_GOLD_AS_MAINTENANCE())
			iBaseCommerceChange = 0;

		if (iBaseCommerceChange != 0)
		{
			if (kBuilding.isOrbital())
			{
				const int iOrbitalCities = kOwner.countNumCitiesWithOrbitalInfrastructure();
				const int iVal = std::min(iBaseCommerceChange * iOrbitalCities, getPopulation());
				iCommerce += hasOrbitalInfrastructure() ? iVal : (iVal / 2);
			}
			else
			{
				iCommerce += iBaseCommerceChange;
			}
		}

		iCommerce += getBuildingCommerceChange(eBuilding, eIndex);

		if (bFull)
		{
			// Toffer - These are cached separately, so should not be counted when caching m_aiBuildingCommerce through this function.			
			iCommerce += (kBuilding.getCommercePerPopChange(eIndex)
				+ getBonusCommercePercentChanges(eIndex, eBuilding)
				+ kTeam.getBuildingCommerceTechChange(eIndex, eBuilding)) / 100;
		}

		const ReligionTypes eRel = (ReligionTypes)kBuilding.getReligionType();
		if (eRel != NO_RELIGION && eRel == kOwner.getStateReligion())
			iCommerce += kOwner.getStateReligionBuildingCommerce(eIndex);

		const ReligionTypes eGlobalRel = (ReligionTypes)kBuilding.getGlobalReligionCommerce();
		if (eGlobalRel != NO_RELIGION)
		{
			iCommerce += GC.getReligionInfo(eGlobalRel).getGlobalReligionCommerce(eIndex)
				* GC.getGame().countReligionLevels(eGlobalRel);
		}
	}

	const CorporationTypes eGlobalCorp = (CorporationTypes)kBuilding.getGlobalCorporationCommerce();
	if (eGlobalCorp != NO_CORPORATION)
	{
		iCommerce += GC.getCorporationInfo(eGlobalCorp).getHeadquarterCommerce(eIndex)
			* GC.getGame().countCorporationLevels(eGlobalCorp);
	}

	const int iDoubleTime = kBuilding.getCommerceChangeDoubleTime(eIndex);
	if (iDoubleTime != 0)
	{
		const int iTimeBuilt = getBuildingData(eBuilding).iTimeBuilt;
		if (iTimeBuilt != MIN_INT && GC.getGame().getGameTurnYear() - iTimeBuilt >= iDoubleTime)
			iCommerce *= 2;
	}

	return iCommerce;
}


/*
 * Returns the total additional commerce times 100 that adding one of the given buildings will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalCommerceTimes100ByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	const int iExtraRateTimes100 = getBaseCommerceRateFromBuilding100(eIndex, eBuilding);
	const int iExtraModifier = getAdditionalCommerceRateModifierByBuilding(eIndex, eBuilding);

	if (iExtraRateTimes100 == 0 && iExtraModifier == 0)
	{
		return 0;
	}
	const int iRateTimes100 = getBaseCommerceRateTimes100(eIndex);
	const int iModifier = getTotalCommerceRateModifier(eIndex);

	const int iCommerceWithoutBuilding =
	(
		iRateTimes100 > 0
		?
		iRateTimes100 * iModifier / 100
		:
		iRateTimes100 * 100 / iModifier
	);
	const int iCommerceWithBuilding
	(
		iRateTimes100 + iExtraRateTimes100 > 0
		?
		(iRateTimes100 + iExtraRateTimes100) * (iModifier + iExtraModifier) / 100
		:
		(iRateTimes100 + iExtraRateTimes100) * 100 / (iModifier + iExtraModifier)
	);
	int iExtraTimes100 = iCommerceWithBuilding - iCommerceWithoutBuilding;

	const CvBuildingInfo& building = GC.getBuildingInfo(eBuilding);

	for (int iI = 0; iI < building.getNumReplacedBuilding(); iI++)
	{
		const BuildingTypes eBuildingX = static_cast<BuildingTypes>(building.getReplacedBuilding(iI));

		if (hasFullyActiveBuilding(eBuildingX))
		{
			iExtraTimes100 -= getAdditionalCommerceTimes100ByBuilding(eIndex, eBuildingX);
		}
	}
	return iExtraTimes100;
}

/*
 * Returns the additional base commerce rate constructing the given building will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getBaseCommerceRateFromBuilding100(CommerceTypes eIndex, BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eBuilding);

	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	int iExtraRate100 = 100 * kBuilding.getCommerceChange(eIndex);
	if (iExtraRate100 < 0 && eIndex == COMMERCE_GOLD && GC.getTREAT_NEGATIVE_GOLD_AS_MAINTENANCE())
	{
		iExtraRate100 = 0;
	}
	iExtraRate100 += 100 * getBuildingCommerceChange(eBuilding, eIndex);

	if (kBuilding.getReligionType() != NO_RELIGION
	&& kBuilding.getReligionType() == GET_PLAYER(getOwner()).getStateReligion())
	{
		iExtraRate100 += 100 * GET_PLAYER(getOwner()).getStateReligionBuildingCommerce(eIndex);
	}

	if (kBuilding.getGlobalReligionCommerce() != NO_RELIGION)
	{
		iExtraRate100 += 100 * (
			GC.getReligionInfo((ReligionTypes)kBuilding.getGlobalReligionCommerce()).getGlobalReligionCommerce(eIndex)
			*
			GC.getGame().countReligionLevels((ReligionTypes)kBuilding.getGlobalReligionCommerce())
		);
	}
	if (kBuilding.getGlobalCorporationCommerce() != NO_CORPORATION)
	{
		iExtraRate100 += 100 * (
			GC.getCorporationInfo((CorporationTypes)kBuilding.getGlobalCorporationCommerce()).getHeadquarterCommerce(eIndex)
			*
			GC.getGame().countCorporationLevels((CorporationTypes)kBuilding.getGlobalCorporationCommerce())
		);
	}

	// Specialists
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); ++iI)
	{
		if (kBuilding.getFreeSpecialistCount((SpecialistTypes)iI) != 0)
		{
			iExtraRate100 += 100 * getAdditionalBaseCommerceRateBySpecialist(eIndex, (SpecialistTypes)iI, kBuilding.getFreeSpecialistCount((SpecialistTypes)iI));
		}
	}

	for (int iI = 1; iI < kBuilding.getFreeSpecialist() + 1; iI++)
	{
		const SpecialistTypes eNewSpecialist = getBestSpecialist(iI);
		if (eNewSpecialist == NO_SPECIALIST) break;

		iExtraRate100 += 100 * GET_PLAYER(getOwner()).specialistCommerce(eNewSpecialist, eIndex);
	}

	if (kBuilding.getNumPopulationEmployed() > 0)
	{
		int* paiCommerce = new int[NUM_COMMERCE_TYPES];
		int* paiYield = new int[NUM_YIELD_TYPES];
		int iGreatPeopleRate;
		int iHappiness;
		int iHealthiness;
		removeWorstCitizenActualEffects(kBuilding.getNumPopulationEmployed(), iGreatPeopleRate, iHappiness, iHealthiness, paiYield, paiCommerce);
		iExtraRate100 += 100 * paiCommerce[eIndex];
		SAFE_DELETE_ARRAY(paiCommerce);
		SAFE_DELETE_ARRAY(paiYield);
	}

	// Toffer - The rest are already scaled up two orders of magnitude.
	if (kBuilding.isForceAllTradeRoutes())
	{
		const int iCurrentTradeRevenue = GET_PLAYER(getOwner()).calculateTotalExports(YIELD_COMMERCE);

		GET_PLAYER(getOwner()).changeForceAllTradeRoutes(1);

		const int iFutureTradeRevenue = GET_PLAYER(getOwner()).calculateTotalExports(YIELD_COMMERCE);

		GET_PLAYER(getOwner()).changeForceAllTradeRoutes(-1);

		iExtraRate100 += (iFutureTradeRevenue - iCurrentTradeRevenue) * GET_PLAYER(getOwner()).getCommercePercent(eIndex);
	}
	iExtraRate100 += getPopulation() * kBuilding.getCommercePerPopChange(eIndex);

	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		if (hasBonus((BonusTypes)iI))
		{
			iExtraRate100 += kBuilding.getBonusCommercePercentChanges(iI, eIndex);
		}
	}
	foreach_(const TechCommerceArray& pair, kBuilding.getTechCommerceChanges100())
	{
		if (GET_TEAM(getTeam()).isHasTech(pair.first))
		{
			iExtraRate100 += pair.second[eIndex];
		}
	}
	return iExtraRate100;
}



/*
 * Returns the additional commerce rate modifier constructing the given building will provide.
 *
 * Doesn't check if the building can be constructed in this city.
 */
int CvCity::getAdditionalCommerceRateModifierByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eBuilding);

	if (isReligiouslyLimitedBuilding(eBuilding))
	{
		return 0;
	}
	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	int iMod = kBuilding.getCommerceModifier(eIndex) + kBuilding.getGlobalCommerceModifier(eIndex);

	foreach_(const TechCommerceArray& pair, kBuilding.getTechCommerceModifiers())
	{
		if (GET_TEAM(getTeam()).isHasTech(pair.first))
		{
			iMod += pair.second[eIndex];
		}
	}
	for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
	{
		if (hasBonus((BonusTypes)iI))
		{
			iMod += kBuilding.getBonusCommerceModifier(iI, eIndex);
		}
	}
	return iMod;
}
// BUG - Building Additional Commerce - end


void CvCity::updateBuildingCommerce()
{
	PROFILE_FUNC();

	// Disabled during modifier recalc (and called explicitly there after re-enabling)
	if (!GC.getGame().isRecalculatingModifiers())
	{
		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			const CommerceTypes eType = static_cast<CommerceTypes>(iI);
			int iNewBuildingCommerce = 0;

			for (int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
			{
				iNewBuildingCommerce += getBuildingCommerceByBuilding(eType, (BuildingTypes)iJ);
			}

			if (getBuildingCommerce(eType) != iNewBuildingCommerce)
			{
				m_aiBuildingCommerce[iI] = iNewBuildingCommerce;

				setCommerceDirty(eType);
			}
		}
	}
}


int CvCity::getSpecialistCommerce(CommerceTypes eIndex)	const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_aiSpecialistCommerce100[eIndex] / 100;
}


void CvCity::changeSpecialistCommerceTimes100(CommerceTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiSpecialistCommerce100[eIndex] += iChange;
		setCommerceDirty(eIndex);
	}
}




// Returns the total additional commerce that changing the number of given specialists will provide/remove.
int CvCity::getAdditionalCommerceTimes100BySpecialist(CommerceTypes eIndex, SpecialistTypes eSpecialist, int iChange) const
{
	int iExtraRate = getAdditionalBaseCommerceRateBySpecialist(eIndex, eSpecialist, iChange);
	if (iExtraRate == 0)
	{
		return 0;
	}
	iExtraRate += GET_PLAYER(getOwner()).getSpecialistCommercePercentChanges(eSpecialist, eIndex);

	return iExtraRate * getTotalCommerceRateModifier(eIndex);
}

// Returns the additional base commerce rate that changing the number of given specialists will provide/remove.
int CvCity::getAdditionalBaseCommerceRateBySpecialist(CommerceTypes eIndex, SpecialistTypes eSpecialist, int iChange) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eSpecialist);
	return (
		iChange * (
			GC.getSpecialistInfo(eSpecialist).getCommerceChange(eIndex)
			+ GET_PLAYER(getOwner()).getExtraSpecialistCommerce(eSpecialist, eIndex)
			+ getLocalSpecialistExtraCommerce(eSpecialist, eIndex)
			+ GET_PLAYER(getOwner()).getSpecialistExtraCommerce(eIndex)
		)
	);
}



int CvCity::getReligionCommerce(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex)
	return m_aiReligionCommerce[eIndex];
}


int CvCity::getReligionCommerceByReligion(CommerceTypes eIndex, ReligionTypes eReligion) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	FASSERT_BOUNDS(0, GC.getNumReligionInfos(), eReligion);

	int iCommerce = 0;

	if(GET_PLAYER(getOwner()).getStateReligion() == eReligion
	|| GET_PLAYER(getOwner()).getStateReligion() == NO_RELIGION
	|| GET_PLAYER(getOwner()).isNonStateReligionCommerce())
	{
		if (isHasReligion(eReligion))
		{
			iCommerce += GC.getReligionInfo(eReligion).getStateReligionCommerce(eIndex);

			if (isHolyCity(eReligion))
			{
				iCommerce += GC.getReligionInfo(eReligion).getHolyCityCommerce(eIndex);
			}
		}
	}
	return iCommerce;
}


// XXX can this be simplified???
void CvCity::updateReligionCommerce(CommerceTypes eIndex)
{
	PROFILE_EXTRA_FUNC();
	int iNewReligionCommerce = 0;

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		iNewReligionCommerce += getReligionCommerceByReligion(eIndex, ((ReligionTypes)iI));
	}

	if (getReligionCommerce(eIndex) != iNewReligionCommerce)
	{
		m_aiReligionCommerce[eIndex] = iNewReligionCommerce;
		FASSERT_NOT_NEGATIVE(getReligionCommerce(eIndex));

		setCommerceDirty(eIndex);
	}
}


void CvCity::updateReligionCommerce()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		updateReligionCommerce((CommerceTypes)iI);
	}
}


int CvCity::getCorporationYield(YieldTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex)
	return m_aiCorporationYield[eIndex];
}

void CvCity::setCorporationYield(YieldTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex)

	const int iOldValue = getCorporationYield(eIndex);

	if (iOldValue != iNewValue)
	{
		m_aiCorporationYield[eIndex] = iNewValue;
		FASSERT_NOT_NEGATIVE(getCorporationYield(eIndex));

		changeExtraYield(eIndex, iNewValue - iOldValue);
	}
}

int CvCity::getCorporationCommerce(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_aiCorporationCommerce[eIndex];
}


int CvCity::getCorporationYieldByCorporation(YieldTypes eIndex, CorporationTypes eCorporation) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	FASSERT_BOUNDS(0, GC.getNumCorporationInfos(), eCorporation);

	if (!isActiveCorporation(eCorporation) || isDisorder())
	{
		return 0;
	}
	int iYield = GC.getCorporationInfo(eCorporation).getYieldChange(eIndex) * 100;

	foreach_(const BonusTypes eBonus, GC.getCorporationInfo(eCorporation).getPrereqBonuses())
	{
		if (getNumBonuses(eBonus) > 0)
		{
			iYield += (GC.getCorporationInfo(eCorporation).getYieldProduced(eIndex) * getNumBonuses(eBonus) * GC.getWorldInfo(GC.getMap().getWorldSize()).getCorporationMaintenancePercent()) / 100;
		}
	}
	return (iYield + 99) / 100;
}

int CvCity::getCorporationCommerceByCorporation(CommerceTypes eIndex, CorporationTypes eCorporation) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	FASSERT_BOUNDS(0, GC.getNumCorporationInfos(), eCorporation);

	if (!isActiveCorporation(eCorporation) || isDisorder())
	{
		return 0;
	}
	int iCommerce = GC.getCorporationInfo(eCorporation).getCommerceChange(eIndex) * 100;

	foreach_(const BonusTypes eBonus, GC.getCorporationInfo(eCorporation).getPrereqBonuses())
	{
		if (getNumBonuses(eBonus) > 0)
		{
			iCommerce += (
				GC.getCorporationInfo(eCorporation).getCommerceProduced(eIndex)
				* getNumBonuses(eBonus)
				* GC.getWorldInfo(GC.getMap().getWorldSize()).getCorporationMaintenancePercent()
				/ 100
			);
		}
	}
	return (getModifiedIntValue(iCommerce, GET_TEAM(getTeam()).getCorporationRevenueModifier()) + 99) / 100;
}

void CvCity::updateCorporationCommerce(CommerceTypes eIndex)
{
	PROFILE_EXTRA_FUNC();
	int iNewCommerce = 0;

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		iNewCommerce += getCorporationCommerceByCorporation(eIndex, (CorporationTypes)iI);
	}

	if (getCorporationCommerce(eIndex) != iNewCommerce)
	{
		m_aiCorporationCommerce[eIndex] = iNewCommerce;
		FASSERT_NOT_NEGATIVE(getCorporationCommerce(eIndex));

		setCommerceDirty(eIndex);
	}
}

void CvCity::updateCorporationYield(YieldTypes eIndex)
{
	PROFILE_EXTRA_FUNC();
	const int iOldYield = getCorporationYield(eIndex);
	int iNewYield = 0;

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		iNewYield += getCorporationYieldByCorporation(eIndex, (CorporationTypes)iI);
	}

	if (iOldYield != iNewYield)
	{
		m_aiCorporationYield[eIndex] = iNewYield;
		FASSERT_NOT_NEGATIVE(getCorporationYield(eIndex));

		changeExtraYield(eIndex, iNewYield - iOldYield);
	}
}


void CvCity::updateCorporation()
{
	PROFILE_EXTRA_FUNC();
	updateCorporationBonus();
	updateBuildingCommerce();

	for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		updateCorporationYield((YieldTypes)iI);
	}
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; ++iI)
	{
		updateCorporationCommerce((CommerceTypes)iI);
	}
	setMaintenanceDirty(true);
}


void CvCity::updateCorporationBonus()
{
	PROFILE_EXTRA_FUNC();

	m_corpBonusProduction.clear();
	const int iNumBonuses = GC.getNumBonusInfos();
	bool* abHadBonus = new bool[iNumBonuses];
	int* aiLastCorpProducedBonus = new int[iNumBonuses];
	int* aiExtraCorpProducedBonus = new int[iNumBonuses];

	for (int iI = 0; iI < iNumBonuses; ++iI)
	{
		abHadBonus[iI] = hasBonus((BonusTypes)iI);
		aiLastCorpProducedBonus[iI] = getNumBonuses((BonusTypes)iI);
		aiExtraCorpProducedBonus[iI] = 0;
	}

	for (int iIter = 0; iIter < GC.getNumCorporationInfos(); ++iIter)
	{
		for (int iCorp = 0; iCorp < GC.getNumCorporationInfos(); ++iCorp)
		{
			const int iBonusProduced = GC.getCorporationInfo((CorporationTypes)iCorp).getBonusProduced();

			if (-1 < iBonusProduced
			&& !GET_TEAM(getTeam()).isBonusObsolete((BonusTypes)iBonusProduced)
			&& GET_TEAM(getTeam()).isHasTech((TechTypes)GC.getBonusInfo((BonusTypes)iBonusProduced).getTechCityTrade())
			&& isHasCorporation((CorporationTypes)iCorp) && GET_PLAYER(getOwner()).isActiveCorporation((CorporationTypes)iCorp))
			{
				bool bConsumes = false;

				foreach_(const BonusTypes eBonusConsumed, GC.getCorporationInfo((CorporationTypes)iCorp).getPrereqBonuses())
				{
					if (eBonusConsumed != iBonusProduced) // ignore circular xml definiton error.
					{
						bConsumes = true;
						aiExtraCorpProducedBonus[iBonusProduced] += aiLastCorpProducedBonus[eBonusConsumed];
					}
				}
				if (iIter == 0 && !bConsumes) // Only handle this conditionless production once.
				{
					aiExtraCorpProducedBonus[iBonusProduced] += 1;
				}
			}
		}

		bool bChanged = false;
		for (int iI = 0; iI < iNumBonuses; ++iI)
		{
			if (aiExtraCorpProducedBonus[iI] != 0)
			{
				bool bFirst = true;
				for (std::vector< std::pair<BonusTypes, int> >::iterator it = m_corpBonusProduction.begin(); it != m_corpBonusProduction.end(); ++it)
				{
					if ((*it).first == iI)
					{
						(*it).second += aiExtraCorpProducedBonus[iI];
						bFirst = false;
					}
				}
				if (bFirst) m_corpBonusProduction.push_back(std::make_pair(static_cast<BonusTypes>(iI), aiExtraCorpProducedBonus[iI]));

				bChanged = true; // The produced bonus might be consumed by another corp to produce another bonus,
				//	which means we need to loop iIter to check for and handle that case.
			}
			aiLastCorpProducedBonus[iI] = aiExtraCorpProducedBonus[iI];
			aiExtraCorpProducedBonus[iI] = 0;
		}
		if (!bChanged)
		{
			break;
		}
	}

	for (int iI = 0; iI < iNumBonuses; ++iI)
	{
		if (abHadBonus[iI] != hasBonus((BonusTypes)iI))
		{
			if (abHadBonus[iI])
			{
				processBonus((BonusTypes)iI, -1);
			}
			else processBonus((BonusTypes)iI, 1);
		}
	}
	SAFE_DELETE_ARRAY(abHadBonus);
	SAFE_DELETE_ARRAY(aiLastCorpProducedBonus);
	SAFE_DELETE_ARRAY(aiExtraCorpProducedBonus);
}

//TB NOTE: getCommerceRateModifier and changeCommerceRateModifier now only apply to events.  I'd rename them but it might get more confusing that way.
int CvCity::getCommerceRateModifier(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_aiCommerceRateModifier[eIndex];
}


void CvCity::changeCommerceRateModifier(CommerceTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiCommerceRateModifier[eIndex] += iChange;

		setCommerceModifierDirty(eIndex);

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getCommerceHappinessPer(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_aiCommerceHappinessPer[eIndex];
}


int CvCity::getCommerceHappinessByType(CommerceTypes eCommerce) const
{
	return ((getCommerceHappinessPer(eCommerce) * GET_PLAYER(getOwner()).getCommercePercent(eCommerce)) / 100);
}


int CvCity::getCommerceHappiness() const
{
	PROFILE_EXTRA_FUNC();
	int iHappiness = 0;

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		iHappiness += getCommerceHappinessByType((CommerceTypes)iI);
	}

	return iHappiness;
}


void CvCity::changeCommerceHappinessPer(CommerceTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiCommerceHappinessPer[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(getCommerceHappinessPer(eIndex));

		AI_setAssignWorkDirty(true);
	}
}


void CvCity::changeCommercePerPopFromBuildings(const CommerceTypes eIndex, const int iChange)
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	if (iChange != 0)
	{
		m_commercePerPopFromBuildings[eIndex] += iChange;
		setCommerceDirty(eIndex);
	}
}

int CvCity::getCommercePerPopFromBuildings(const CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_commercePerPopFromBuildings[eIndex];
}

void CvCity::changeBuildingCommerceModifier(CommerceTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (iChange != 0)
	{
		m_buildingCommerceMod[eIndex] += iChange;
		setCommerceModifierDirty(eIndex);
		GET_PLAYER(getOwner()).invalidateCommerceRankCache();
	}
}

int CvCity::getBuildingCommerceModifier(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_buildingCommerceMod[eIndex];
}


int CvCity::getDomainFreeExperience(DomainTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_DOMAIN_TYPES, eIndex);
	int iTotalDomainFreeExperience = m_aiDomainFreeExperience[eIndex];
	iTotalDomainFreeExperience += GET_PLAYER(getOwner()).getNationalDomainFreeExperience(eIndex);

	return std::max(0, iTotalDomainFreeExperience);
}


void CvCity::changeDomainFreeExperience(DomainTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_DOMAIN_TYPES, eIndex);
	m_aiDomainFreeExperience[eIndex] += iChange;
	FASSERT_NOT_NEGATIVE(getDomainFreeExperience(eIndex));
}


int CvCity::getDomainProductionModifier(DomainTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_DOMAIN_TYPES, eIndex);
	int iTotalDomainProductionModifier = m_aiDomainProductionModifier[eIndex];
	iTotalDomainProductionModifier += GET_PLAYER(getOwner()).getNationalDomainProductionModifier(eIndex);

	return std::max(0, iTotalDomainProductionModifier);
}


void CvCity::changeDomainProductionModifier(DomainTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_DOMAIN_TYPES, eIndex);
	m_aiDomainProductionModifier[eIndex] += iChange;
}


int CvCity::getCulture(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	return (m_aiCulture[eIndex] < 0 ? MAX_INT / 100 : m_aiCulture[eIndex] / 100);
}

int CvCity::getCultureTimes100(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	return (m_aiCulture[eIndex] < 0 ? MAX_INT : m_aiCulture[eIndex]);
}


int CvCity::countTotalCultureTimes100() const
{
	PROFILE_EXTRA_FUNC();
	int iTotalCulture = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iTotalCulture += getCultureTimes100((PlayerTypes)iI);
		}
	}
	return iTotalCulture;
}


PlayerTypes CvCity::findHighestCulture() const
{
	PROFILE_EXTRA_FUNC();
	int iBestValue = 0;
	PlayerTypes eBestPlayer = NO_PLAYER;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			const int iValue = getCultureTimes100((PlayerTypes)iI);

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestPlayer = ((PlayerTypes)iI);
			}
		}
	}
	return eBestPlayer;
}


int CvCity::calculateCulturePercent(PlayerTypes eIndex) const
{
	const int iTotalCulture = countTotalCultureTimes100();

	if (iTotalCulture > 0)
	{
		return getCultureTimes100(eIndex) * 100 / iTotalCulture;
	}
	return 0;
}


int CvCity::calculateTeamCulturePercent(TeamTypes eIndex) const
{
	PROFILE_EXTRA_FUNC();
	int iTeamCulturePercent = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(eIndex))
		{
			iTeamCulturePercent += calculateCulturePercent((PlayerTypes)iI);
		}
	}
	return iTeamCulturePercent;
}


void CvCity::setCulture(PlayerTypes eIndex, int iNewValue, bool bPlots, bool bUpdatePlotGroups, bool bNationalSet)
{
	setCultureTimes100(eIndex, (iNewValue > MAX_INT / 100) ? MAX_INT : 100 * iNewValue, bPlots, bUpdatePlotGroups, bNationalSet);
}

void CvCity::setCultureTimes100(PlayerTypes eIndex, int iNewValue, bool bPlots, bool bUpdatePlotGroups, bool bNationalSet)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);

	const int iOldCulture = getCultureTimes100(eIndex);

	if (iOldCulture != iNewValue)
	{
		FASSERT_NOT_NEGATIVE(iNewValue);

		if (iNewValue < iOldCulture || getCultureThreshold() > -1)
		{
			m_aiCulture[eIndex] = iNewValue;

			updateCultureLevel(bUpdatePlotGroups);

			if (getCultureThreshold() == -1)
			{
				// Toffer - Cap value at max culture threshold
				m_aiCulture[eIndex] = 100 * GC.getGame().getCultureThreshold(getCultureLevel());
			}
		}
		if (bPlots && iNewValue > iOldCulture)
		{
			doPlotCulture(eIndex, (iNewValue - iOldCulture) / 100);
		}
	}

	if (!bNationalSet && iOldCulture < iNewValue)
	{
		GET_PLAYER(getOwner()).changeCulture((iNewValue - iOldCulture) / 100);
	}
}


void CvCity::changeCulture(PlayerTypes eIndex, int iChange, bool bPlots, bool bUpdatePlotGroups)
{
	changeCultureTimes100(eIndex, 100 * iChange, bPlots, bUpdatePlotGroups);
}

void CvCity::changeCultureTimes100(PlayerTypes eIndex, int iChange, bool bPlots, bool bUpdatePlotGroups)
{
	if (iChange == 0) return;

	const int iOld = getCultureTimes100(eIndex);

	if (iChange > 99)
	{
		GET_PLAYER(getOwner()).changeCulture(iChange / 100);
	}

	int iNew;
	if (iChange < 0)
	{
		iNew = std::max(0, iOld + iChange);
	}
	else if (MAX_INT - iChange > iOld)
	{
		iNew = iOld + iChange;
	}
	else iNew = MAX_INT;

	setCultureTimes100(eIndex, iNew, bPlots, bUpdatePlotGroups, true);
}


int CvCity::getNumRevolts(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	return m_aiNumRevolts[eIndex];
}


void CvCity::changeNumRevolts(PlayerTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	m_aiNumRevolts[eIndex] += iChange;
	FASSERT_NOT_NEGATIVE(m_aiNumRevolts[eIndex]);
}

bool CvCity::isEverOwned(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	return m_abEverOwned[eIndex];
}


void CvCity::setEverOwned(PlayerTypes eIndex, bool bNewValue)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	m_abEverOwned[eIndex] = bNewValue;
}


bool CvCity::isTradeRoute(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	return m_abTradeRoute[eIndex];
}


void CvCity::setTradeRoute(PlayerTypes eIndex, bool bNewValue)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	m_abTradeRoute[eIndex] = bNewValue;
}


bool CvCity::isRevealed(TeamTypes eIndex, bool bDebug) const
{
	if (bDebug && GC.getGame().isDebugMode())
	{
		return true;
	}
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_abRevealed[eIndex];
}


void CvCity::setRevealed(TeamTypes eIndex, bool bNewValue)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	setupGraphical();

	if (isRevealed(eIndex, false) != bNewValue)
	{
		m_abRevealed[eIndex] = bNewValue;

		updateVisibility();

		if (eIndex == GC.getGame().getActiveTeam())
		{
			algo::for_each(plots(), bind(CvPlot::updateSymbols, _1));
		}
	}
}


bool CvCity::getEspionageVisibility(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_abEspionageVisibility[eIndex];
}


void CvCity::setEspionageVisibility(TeamTypes eIndex, bool bNewValue, bool bUpdatePlotGroups)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	if (getEspionageVisibility(eIndex) != bNewValue)
	{
		plot()->updateSight(false, bUpdatePlotGroups);

		m_abEspionageVisibility[eIndex] = bNewValue;

		plot()->updateSight(true, bUpdatePlotGroups);
	}
}

void CvCity::updateEspionageVisibility(bool bUpdatePlotGroups)
{
	PROFILE_EXTRA_FUNC();
	std::vector<EspionageMissionTypes> aMission;

	for (int iI = 0; iI < GC.getNumEspionageMissionInfos(); ++iI)
	{
		if (GC.getEspionageMissionInfo((EspionageMissionTypes)iI).isPassive() && GC.getEspionageMissionInfo((EspionageMissionTypes)iI).getVisibilityLevel() > 0)
		{
			aMission.push_back((EspionageMissionTypes)iI);
		}
	}
	const TeamTypes eTeam = getTeam();

	for (int iI = 0; iI < MAX_PC_TEAMS; ++iI)
	{
		const TeamTypes eTeamX = static_cast<TeamTypes>(iI);
		bool bVisibility = false;

		if (eTeamX != eTeam && isRevealed(eTeamX, false))
		{
			for (int iJ = 0; iJ < MAX_PC_PLAYERS; ++iJ)
			{
				const CvPlayer& playerY = GET_PLAYER((PlayerTypes)iJ);

				if (playerY.isAliveAndTeam(eTeamX))
				{
					foreach_(const EspionageMissionTypes& it, aMission)
					{
						if (playerY.canDoEspionageMission(it, getOwner(), plot(), -1, NULL))
						{
							bVisibility = true;
							break;
						}
					}
					if (bVisibility) break;
				}
			}
		}
		setEspionageVisibility(eTeamX, bVisibility, bUpdatePlotGroups);
	}
}

const wchar_t* CvCity::getNameKey() const
{
	return m_szName;
}


const CvWString CvCity::getName(uint uiForm) const
{
	return gDLL->getObjectText(m_szName, uiForm, true);
}


void CvCity::setName(const wchar_t* szNewValue, bool bFound)
{
	CvWString szName(szNewValue);
	gDLL->stripSpecialCharacters(szName);

	if (!szName.empty())
	{
		if (GET_PLAYER(getOwner()).isCityNameValid(szName, false))
		{
			m_szName = szName;

			setInfoDirty(true);

			if (isCitySelected())
			{
				gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
			}
		}
		if (bFound)
		{
			doFoundMessage();
		}
	}
}


void CvCity::doFoundMessage()
{
	CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_HAS_BEEN_FOUNDED", getNameKey());
	AddDLLMessage(getOwner(), false, -1, szBuffer, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), MESSAGE_TYPE_MAJOR_EVENT, NULL, NO_COLOR, getX(), getY());

	szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_IS_FOUNDED", getNameKey());
	GC.getGame().addReplayMessage(REPLAY_MESSAGE_CITY_FOUNDED, getOwner(), szBuffer, getX(), getY(), GC.getCOLOR_ALT_HIGHLIGHT_TEXT());
}


std::string CvCity::getScriptData() const
{
	return m_szScriptData;
}

// cppcheck-suppress passedByValue
void CvCity::setScriptData(std::string szNewValue)
{
	m_szScriptData = szNewValue;
}


int CvCity::getFreeBonus(BonusTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eIndex);
	return m_paiFreeBonus[eIndex];
}


void CvCity::changeFreeBonus(BonusTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eIndex);

	if (iChange != 0)
	{
		GET_PLAYER(getOwner()).startDeferredPlotGroupBonusCalculation();

		plot()->updatePlotGroupBonus(false);
		m_paiFreeBonus[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(getFreeBonus(eIndex));
		plot()->updatePlotGroupBonus(true);

		GET_PLAYER(getOwner()).endDeferredPlotGroupBonusCalculation();
	}
}

int CvCity::getNumBonusesFromBase(BonusTypes eIndex, int iBaseNum) const
{
	if (GET_PLAYER(getOwner()).getBonusMintedPercent(eIndex) > 0)
	{
		return 0;
	}
	return iBaseNum;
}

int CvCity::getNumBonuses(BonusTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eIndex);

	if (!GET_TEAM(getTeam()).isHasTech((TechTypes)GC.getBonusInfo(eIndex).getTechCityTrade()))
	{
		return 0;
	}
	return getNumBonusesFromBase(eIndex, m_paiNumBonuses[eIndex]) + getCorpBonusProduction(eIndex);
}


bool CvCity::hasBonus(BonusTypes eIndex) const
{
	return getNumBonuses(eIndex) > 0;
}

void CvCity::startDeferredBonusProcessing()
{
	PROFILE_EXTRA_FUNC();
	if (0 == m_deferringBonusProcessingCount++)
	{
		SAFE_DELETE_ARRAY(m_paiStartDeferredSectionNumBonuses);

		m_paiStartDeferredSectionNumBonuses = new int[GC.getNumBonusInfos()];

		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			m_paiStartDeferredSectionNumBonuses[iI] = m_paiNumBonuses[iI];
		}
	}
}

void CvCity::endDeferredBonusProcessing()
{
	PROFILE_EXTRA_FUNC();
	if (0 == --m_deferringBonusProcessingCount)
	{
		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			processNumBonusChange((BonusTypes)iI, m_paiStartDeferredSectionNumBonuses[iI], m_paiNumBonuses[iI]);
		}

		SAFE_DELETE_ARRAY(m_paiStartDeferredSectionNumBonuses);
	}
}

void CvCity::processNumBonusChange(BonusTypes eIndex, int iOldValue, int iNewValue)
{
	if (iOldValue != iNewValue)
	{
		const bool bOldHasBonus = (getNumBonusesFromBase(eIndex, iOldValue) != 0);
		const bool bNewHasBonus = (getNumBonusesFromBase(eIndex, iNewValue) != 0);

		if (bOldHasBonus != bNewHasBonus)
		{
			if (bNewHasBonus)
			{
				processBonus(eIndex, 1);
			}
			else
			{
				processBonus(eIndex, -1);
			}
		}

		if (isCorporationBonus(eIndex))
		{
			updateCorporation();
		}

		//	Linking bonuses may change what is buildable
		FlushCanConstructCache();
	}
}

void CvCity::changeNumBonuses(BonusTypes eIndex, int iChange)
{
	PROFILE_FUNC();

	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eIndex);

	if (iChange != 0)
	{
		//bool bOldHasBonus = hasBonus(eIndex);

		m_paiNumBonuses[eIndex] += iChange;

		if (m_deferringBonusProcessingCount == 0)
		{
			processNumBonusChange(eIndex, m_paiNumBonuses[eIndex] - iChange, m_paiNumBonuses[eIndex]);
		}
	}
}


int CvCity::getCorpBonusProduction(const BonusTypes eBonus) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eBonus)

	for (std::vector< std::pair<BonusTypes, int> >::const_iterator it = m_corpBonusProduction.begin(); it != m_corpBonusProduction.end(); ++it)
	{
		if ((*it).first == eBonus)
		{
			return (*it).second;
		}
	}
	return 0;
}

void CvCity::changeCorpBonusProduction(const BonusTypes eBonus, const int iChange)
{
}


bool CvCity::isCorporationBonus(BonusTypes eBonus) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eBonus);

	for (int iCorp = 0; iCorp < GC.getNumCorporationInfos(); ++iCorp)
	{
		if (GET_PLAYER(getOwner()).isActiveCorporation((CorporationTypes)iCorp)
		&& isHasCorporation((CorporationTypes)iCorp)
		&& algo::any_of_equal(GC.getCorporationInfo((CorporationTypes)iCorp).getPrereqBonuses(), eBonus))
		{
			return true;
		}
	}

	return false;
}

bool CvCity::isActiveCorporation(CorporationTypes eCorporation) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumCorporationInfos(), eCorporation);

	if (!isHasCorporation(eCorporation))
	{
		return false;
	}

	if (!GET_PLAYER(getOwner()).isActiveCorporation(eCorporation))
	{
		return false;
	}

	if (GC.getCorporationInfo(eCorporation).getObsoleteTech() != NO_TECH)
	{
		if (GET_TEAM(getTeam()).isHasTech(GC.getCorporationInfo(eCorporation).getObsoleteTech()))
		{
			return false;
		}
	}
	bool bRequiresBonus = false;
	bool bHasRequiredBonus = false;

	foreach_(const BonusTypes eBonus, GC.getCorporationInfo(eCorporation).getPrereqBonuses())
	{
		bRequiresBonus = true;
		if (getNumBonuses(eBonus) > 0)
		{
			bHasRequiredBonus = true;
			break;
		}
	}
	if (bRequiresBonus && bHasRequiredBonus)
	{
		return true;
	}
	if (!bRequiresBonus)
	{
		return true;
	}

	return false;
}

int CvCity::getProgressOnBuilding(const BuildingTypes eType)	const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eType);

	for (std::vector< std::pair<BuildingTypes, int> >::const_iterator it = m_progressOnBuilding.begin(); it != m_progressOnBuilding.end(); ++it)
	{
		if ((*it).first == eType)
		{
			return (*it).second;
		}
	}
	return 0;
}

void CvCity::setProgressOnBuilding(const BuildingTypes eType, int iNewValue)
{
	FASSERT_NOT_NEGATIVE(iNewValue);
	if (iNewValue < 0) iNewValue = 0;

	const int iOldValue = getProgressOnBuilding(eType);
	if (iOldValue != iNewValue)
	{
		changeProgressOnBuilding(eType, iNewValue - iOldValue);
	}
}

void CvCity::changeProgressOnBuilding(const BuildingTypes eType, const int iChange)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eType);
	if (iChange == 0)
	{
		return;
	}
	if (getTeam() == GC.getGame().getActiveTeam())
	{
		setInfoDirty(true);

		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
		}
	}
	for (std::vector< std::pair<BuildingTypes, int> >::iterator it = m_progressOnBuilding.begin(); it != m_progressOnBuilding.end(); ++it)
	{
		if ((*it).first == eType)
		{
			if ((*it).second <= -iChange)
			{
				m_progressOnBuilding.erase(it);
				endDelayOnBuilding(eType);
			}
			else
			{
				(*it).second += iChange;
			}
			return;
		}
	}
	m_progressOnBuilding.push_back(std::make_pair(eType, iChange));
}


int CvCity::getDelayOnBuilding(const BuildingTypes eType)	const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eType);

	for (std::vector< std::pair<BuildingTypes, int> >::const_iterator it = m_delayOnBuilding.begin(); it != m_delayOnBuilding.end(); ++it)
	{
		if ((*it).first == eType)
		{
			return (*it).second;
		}
	}
	return 0;
}

void CvCity::endDelayOnBuilding(const BuildingTypes eType)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eType);

	for (std::vector< std::pair<BuildingTypes, int> >::iterator it = m_delayOnBuilding.begin(); it != m_delayOnBuilding.end(); ++it)
	{
		if ((*it).first == eType)
		{
			m_delayOnBuilding.erase(it);
			return;
		}
	}
}

void CvCity::tickDelayOnBuilding(const BuildingTypes eType, const bool bIncrement)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eType);

	for (std::vector< std::pair<BuildingTypes, int> >::iterator it = m_delayOnBuilding.begin(); it != m_delayOnBuilding.end(); ++it)
	{
		if ((*it).first == eType)
		{
			if (bIncrement)
			{
				(*it).second += 1;
			}
			else if ((*it).second == 1)
			{
				m_delayOnBuilding.erase(it);
			}
			else (*it).second -= 1;

			return;
		}
	}
	if (!bIncrement)
	{
		FErrorMsg("Trying to decrement past zero for a value where negatives makes no sense!");
		return;
	}
	m_delayOnBuilding.push_back(std::make_pair(eType, 1));
}


// Returns true if the given building will decay this turn.
bool CvCity::isBuildingProductionDecay(BuildingTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	return (
		isHuman()
		&& getProductionBuilding() != eIndex
		&& getProgressOnBuilding(eIndex) > 0
		&& (
			100 * getDelayOnBuilding(eIndex)
			>=
			GC.getBUILDING_PRODUCTION_DECAY_TIME() * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent()
		)
	);
}

// Returns the amount by which the given building will decay once it reaches the limit.
// Ignores whether or not the building will actually decay this turn.
int CvCity::getBuildingProductionDecay(BuildingTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	const int iProduction = getProgressOnBuilding(eIndex);
	return iProduction - iProduction * GC.getBUILDING_PRODUCTION_DECAY_PERCENT() / 100;
}

// Returns the number of turns left before the given building will decay.
int CvCity::getBuildingProductionDecayTurns(BuildingTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	return std::max(1, 1 + (GC.getBUILDING_PRODUCTION_DECAY_TIME() * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() + 99) / 100 - getDelayOnBuilding(eIndex));
}


int CvCity::getProjectProduction(ProjectTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumProjectInfos(), eIndex);
	return m_paiProjectProduction[eIndex];
}

void CvCity::setProjectProduction(ProjectTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, GC.getNumProjectInfos(), eIndex);

	if (getProjectProduction(eIndex) != iNewValue)
	{
		m_paiProjectProduction[eIndex] = iNewValue;
		FASSERT_NOT_NEGATIVE(getProjectProduction(eIndex));

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}

		if ((getOwner() == GC.getGame().getActivePlayer()) && isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
		}
	}
}

void CvCity::changeProjectProduction(ProjectTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumProjectInfos(), eIndex);
	setProjectProduction(eIndex, (getProjectProduction(eIndex) + iChange));
}


int CvCity::getProgressOnUnit(const UnitTypes eUnit) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit)

	for (std::vector< std::pair<UnitTypes, int> >::const_iterator it = m_progressOnUnit.begin(); it != m_progressOnUnit.end(); ++it)
	{
		if ((*it).first == eUnit)
		{
			return (*it).second;
		}
	}
	return 0;
}


void CvCity::setProgressOnUnit(const UnitTypes eUnit, int iNewValue)
{
	FASSERT_NOT_NEGATIVE(iNewValue);
	if (iNewValue < 0) iNewValue = 0;

	const int iOldValue = getProgressOnUnit(eUnit);
	if (iOldValue != iNewValue)
	{
		changeProgressOnUnit(eUnit, iNewValue - iOldValue);
	}
}


void CvCity::changeProgressOnUnit(const UnitTypes eUnit, const int iChange)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);
	if (iChange == 0)
	{
		return;
	}
	if (getTeam() == GC.getGame().getActiveTeam())
	{
		setInfoDirty(true);

		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
		}
	}
	for (std::vector< std::pair<UnitTypes, int> >::iterator it = m_progressOnUnit.begin(); it != m_progressOnUnit.end(); ++it)
	{
		if ((*it).first == eUnit)
		{
			if ((*it).second <= -iChange)
			{
				m_progressOnUnit.erase(it);
				endDelayOnUnit(eUnit);
			}
			else
			{
				(*it).second += iChange;
			}
			return;
		}
	}
	m_progressOnUnit.push_back(std::make_pair(eUnit, iChange));
}


int CvCity::getDelayOnUnit(const UnitTypes eUnit) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);

	for (std::vector< std::pair<UnitTypes, int> >::const_iterator it = m_delayOnUnit.begin(); it != m_delayOnUnit.end(); ++it)
	{
		if ((*it).first == eUnit)
		{
			return (*it).second;
		}
	}
	return 0;
}

void CvCity::endDelayOnUnit(const UnitTypes eUnit)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);

	for (std::vector< std::pair<UnitTypes, int> >::iterator it = m_delayOnUnit.begin(); it != m_delayOnUnit.end(); ++it)
	{
		if ((*it).first == eUnit)
		{
			m_delayOnUnit.erase(it);
			return;
		}
	}
}

void CvCity::tickDelayOnUnit(const UnitTypes eUnit, const bool bIncrement)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);

	for (std::vector< std::pair<UnitTypes, int> >::iterator it = m_delayOnUnit.begin(); it != m_delayOnUnit.end(); ++it)
	{
		if ((*it).first == eUnit)
		{
			if (bIncrement)
			{
				(*it).second += 1;
			}
			else if ((*it).second == 1)
			{
				m_delayOnUnit.erase(it);
			}
			else (*it).second -= 1;

			return;
		}
	}
	if (!bIncrement)
	{
		FErrorMsg("Trying to decrement past zero for a value where negatives makes no sense!");
		return;
	}
	m_delayOnUnit.push_back(std::make_pair(eUnit, 1));
}


// Returns true if the given unit will decay this turn.
bool CvCity::isUnitProductionDecay(UnitTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eIndex);
	return (
		isHuman()
		&& getProductionUnit() != eIndex
		&& getProgressOnUnit(eIndex) > 0
		&& (
			100 * getDelayOnUnit(eIndex)
			>=
			GC.getUNIT_PRODUCTION_DECAY_TIME() * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent()
		)
	);
}

// Returns the amount by which the given unit will decay once it reaches the limit.
// Ignores whether or not the unit will actually decay this turn.
int CvCity::getUnitProductionDecay(UnitTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eIndex);
	const int iProduction = getProgressOnUnit(eIndex);
	return iProduction - iProduction * GC.getUNIT_PRODUCTION_DECAY_PERCENT() / 100;
}

// Returns the number of turns left before the given unit will decay.
int CvCity::getUnitProductionDecayTurns(UnitTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eIndex);
	return std::max(1, 1 + (GC.getUNIT_PRODUCTION_DECAY_TIME() * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() + 99) / 100 - getDelayOnUnit(eIndex));
}


int CvCity::getGreatPeopleUnitRate(UnitTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eIndex);
	int iTotalGreatPeopleUnitRate = 0;
	iTotalGreatPeopleUnitRate += m_paiGreatPeopleUnitRate[eIndex];
	iTotalGreatPeopleUnitRate += GET_PLAYER(getOwner()).getNationalGreatPeopleUnitRate(eIndex);
	return std::max(0, iTotalGreatPeopleUnitRate);
}

void CvCity::setGreatPeopleUnitRate(UnitTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eIndex);
	m_paiGreatPeopleUnitRate[eIndex] = iNewValue;
}


void CvCity::changeGreatPeopleUnitRate(UnitTypes eIndex, int iChange)
{
	setGreatPeopleUnitRate(eIndex, (m_paiGreatPeopleUnitRate[eIndex] + iChange));
}


int CvCity::getGreatPeopleUnitProgress(UnitTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eIndex);
	return m_paiGreatPeopleUnitProgress[eIndex];
}


void CvCity::setGreatPeopleUnitProgress(UnitTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eIndex);
	m_paiGreatPeopleUnitProgress[eIndex] = iNewValue;
	FASSERT_NOT_NEGATIVE(getGreatPeopleUnitProgress(eIndex));
}


void CvCity::changeGreatPeopleUnitProgress(UnitTypes eIndex, int iChange)
{
	setGreatPeopleUnitProgress(eIndex, (getGreatPeopleUnitProgress(eIndex) + iChange));
}


int CvCity::getSpecialistCount(SpecialistTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex);
	return m_paiSpecialistCount[eIndex];
}


void CvCity::setSpecialistCount(SpecialistTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex);

	const int iOldValue = getSpecialistCount(eIndex);

	if (iOldValue != iNewValue)
	{
		m_paiSpecialistCount[eIndex] = iNewValue;
		FASSERT_NOT_NEGATIVE(getSpecialistCount(eIndex));

		changeSpecialistPopulation(iNewValue - iOldValue);
		processSpecialist(eIndex, (iNewValue - iOldValue));

		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(CitizenButtons_DIRTY_BIT, true);
		}

#ifdef YIELD_VALUE_CACHING
		AI_NoteSpecialistChange();
#endif
	}
}


void CvCity::changeSpecialistCount(SpecialistTypes eIndex, int iChange)
{
	setSpecialistCount(eIndex, (getSpecialistCount(eIndex) + iChange));
}


void CvCity::alterSpecialistCount(SpecialistTypes eIndex, int iChange)
{
	PROFILE_EXTRA_FUNC();
	if (iChange == 0)
	{
		return;
	}
	if (isCitizensAutomated())
	{
		int iForcedSpecialists = getForceSpecialistCount(eIndex);
		if (getForceSpecialistCount(eIndex) + iChange < 0)
		{
			FErrorMsg("This shouldn't happen");
			setCitizensAutomated(false);
		}
		else
		{
			bool bAutomated = true;
			if (iChange > 0)
			{
				int iForced = 0;
				for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
				{
					iForced += getForceSpecialistCount((SpecialistTypes)iI);
				}
				if (iForced + iChange > getMaxSpecialistCount())
				{
					setCitizensAutomated(false);
					bAutomated = false;
				}
			}
			else
			{
				changeSpecialistCount(eIndex, iChange);
			}

			if (bAutomated)
			{
				setForceSpecialistCount(eIndex, getForceSpecialistCount(eIndex) + iChange);
				AI_assignWorkingPlots();
				return;
			}
		}
	}

	if (iChange > 0)
	{
		for (int iI = 0; iI < iChange; iI++)
		{
			if ((extraPopulation() > 0 || AI_removeWorstCitizen(eIndex)) && isSpecialistValid(eIndex, 1))
			{
				changeSpecialistCount(eIndex, 1);
			}
		}
	}
	else
	{
		for (int iI = 0; iI < -(iChange); iI++)
		{
			if (getSpecialistCount(eIndex) > 0)
			{
				changeSpecialistCount(eIndex, -1);

				if (eIndex != GC.getDEFAULT_SPECIALIST() && GC.getDEFAULT_SPECIALIST() != NO_SPECIALIST)
				{
					changeSpecialistCount((SpecialistTypes)GC.getDEFAULT_SPECIALIST(), 1);
				}
				else if (extraFreeSpecialists() > 0)
				{
					AI_addBestCitizen(false, true);
				}
				else
				{
					bool bCanWorkPlot = false;

					for (int iJ = SKIP_CITY_HOME_PLOT; iJ < getNumCityPlots(); iJ++)
					{
						if (!isWorkingPlot(iJ))
						{
							const CvPlot* pLoopPlot = getCityIndexPlot(iJ);

							if (pLoopPlot != NULL && canWork(pLoopPlot))
							{
								bCanWorkPlot = true;
								break;
							}
						}
					}

					if (bCanWorkPlot)
					{
						AI_addBestCitizen(true, false);
					}
					else
					{
						AI_addBestCitizen(false, true);
					}
				}
			}
		}
	}
}


int CvCity::getMaxSpecialistCount() const
{
	return totalFreeSpecialists() + getPopulation() - angryPopulation() - getNumPopulationEmployed();
}

int CvCity::getMaxSpecialistCount(SpecialistTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex);
	return m_paiMaxSpecialistCount[eIndex];
}

bool CvCity::isSpecialistValid(SpecialistTypes eIndex, int iExtra) const
{
	return
	(
		(
			GET_PLAYER(getOwner()).isSpecialistValid(eIndex)
			||
			getMaxSpecialistCount(eIndex) > 0 && getSpecialistCount(eIndex) + iExtra <= getMaxSpecialistCount(eIndex)
		)
		&&
		getSpecialistCount(eIndex) + iExtra <= getMaxSpecialistCount()
	);
}


void CvCity::changeMaxSpecialistCount(SpecialistTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex);

	if (iChange != 0)
	{
		m_paiMaxSpecialistCount[eIndex] = std::max(0, (m_paiMaxSpecialistCount[eIndex] + iChange));

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getForceSpecialistCount(SpecialistTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex);
	return m_paiForceSpecialistCount[eIndex];
}


bool CvCity::isSpecialistForced() const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		if (getForceSpecialistCount((SpecialistTypes)iI) > 0)
		{
			return true;
		}
	}
	return false;
}


void CvCity::setForceSpecialistCount(SpecialistTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex);

	if (getForceSpecialistCount(eIndex) != iNewValue)
	{
		m_paiForceSpecialistCount[eIndex] = std::max(0, iNewValue);

		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(Help_DIRTY_BIT, true);
		}

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getFreeSpecialistCount(SpecialistTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex);
	return m_paiFreeSpecialistCount[eIndex];
}

int CvCity::getAddedFreeSpecialistCount(SpecialistTypes eIndex) const
{
	return m_paiFreeSpecialistCountUnattributed[eIndex];
}

void CvCity::setFreeSpecialistCount(SpecialistTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex);

	const int iOldValue = getFreeSpecialistCount(eIndex);

	iNewValue += GET_PLAYER(getOwner()).getFreeSpecialistCount(eIndex);
	iNewValue += GET_TEAM(getTeam()).getFreeSpecialistCount(eIndex);

	iNewValue = std::max(0, iNewValue);
	if (iOldValue != iNewValue)
	{
		m_paiFreeSpecialistCount[eIndex] = iNewValue;
		FASSERT_NOT_NEGATIVE(m_paiFreeSpecialistCount[eIndex]);

		changeNumGreatPeople(iNewValue - iOldValue);
		processSpecialist(eIndex, (iNewValue - iOldValue));

		if (isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(CitizenButtons_DIRTY_BIT, true);
		}
	}
}

void CvCity::changeFreeSpecialistCount(SpecialistTypes eIndex, int iChange, bool bUnattributed)
{
	setFreeSpecialistCount(eIndex, (getFreeSpecialistCount(eIndex) + iChange - GET_PLAYER(getOwner()).getFreeSpecialistCount(eIndex)) - GET_TEAM(getTeam()).getFreeSpecialistCount(eIndex));

	if (bUnattributed)
	{
		m_paiFreeSpecialistCountUnattributed[eIndex] += iChange;
	}
}

int CvCity::getImprovementFreeSpecialists(ImprovementTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), eIndex);
	return m_paiImprovementFreeSpecialists[eIndex];
}

void CvCity::changeImprovementFreeSpecialists(ImprovementTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), eIndex);

	m_paiImprovementFreeSpecialists[eIndex] += iChange;
}

uint32_t CvCity::getReligionInfluence(ReligionTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumReligionInfos(), eIndex);
	// Less than zero is meaningless for this value.
	return std::max(0, m_paiReligionInfluence[eIndex]);
}


void CvCity::changeReligionInfluence(ReligionTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumReligionInfos(), eIndex);
	m_paiReligionInfluence[eIndex] += iChange;
}


int CvCity::getCurrentStateReligionHappiness() const
{
	if (GET_PLAYER(getOwner()).getStateReligion() != NO_RELIGION)
	{
		return getStateReligionHappiness(GET_PLAYER(getOwner()).getStateReligion());
	}
	return 0;
}


int CvCity::getStateReligionHappiness(ReligionTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumReligionInfos(), eIndex);
	return m_paiStateReligionHappiness[eIndex];
}


void CvCity::changeStateReligionHappiness(ReligionTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumReligionInfos(), eIndex);

	if (iChange != 0)
	{
		m_paiStateReligionHappiness[eIndex] += iChange;

		AI_setAssignWorkDirty(true);
	}
}


int CvCity::getUnitCombatFreeExperience(UnitCombatTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eIndex);
	if (eIndex == -1) return 0;
	return m_paiUnitCombatFreeExperience[eIndex];
}


void CvCity::changeUnitCombatFreeExperience(UnitCombatTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eIndex);
	m_paiUnitCombatFreeExperience[eIndex] += iChange;
}

bool CvCity::isFreePromotion(PromotionTypes ePromo) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const BuildingTypes eTypeX, getHasBuildings())
	{
		if (isDisabledBuilding(eTypeX))
		{
			continue;
		}

		foreach_(const FreePromoTypes& freePromoType, GC.getBuildingInfo(eTypeX).getFreePromoTypes())
		{
			if (ePromo == freePromoType.ePromotion)
			{
				// Toffer - This should be expanded to evaluate specific units and unitinfos
				if (!freePromoType.m_pExprFreePromotionCondition)
				{
					return true;
				}
				break;
			}
		}
	}
	return false;
}

int CvCity::getSpecialistFreeExperience() const
{
	return m_iSpecialistFreeExperience;
}


void CvCity::changeSpecialistFreeExperience(int iChange)
{
	m_iSpecialistFreeExperience += iChange;
	FASSERT_NOT_NEGATIVE(m_iSpecialistFreeExperience);
}


int CvCity::getEspionageDefenseModifier() const
{
	return m_iEspionageDefenseModifier + GET_PLAYER(getOwner()).getNationalEspionageDefense();
}


void CvCity::changeEspionageDefenseModifier(int iChange)
{
	m_iEspionageDefenseModifier += iChange;
}

bool CvCity::isWorkingPlot(int iIndex) const
{
	FASSERT_BOUNDS(0, NUM_CITY_PLOTS, iIndex);

	return (m_bPlotWorkingMasked ? 0 : m_pabWorkingPlot[iIndex]);
}


bool CvCity::isWorkingPlot(const CvPlot* pPlot) const
{
	const int iIndex = getCityPlotIndex(pPlot);

	return iIndex != -1 && isWorkingPlot(iIndex);
}

void CvCity::processWorkingPlot(int iPlot, int iChange, bool yieldsOnly)
{
	PROFILE_EXTRA_FUNC();
	CvPlot* pPlot = getCityIndexPlot(iPlot);

	if (pPlot)
	{
		FAssertMsg(pPlot->getWorkingCity() == this, "WorkingCity is expected to be this");

		if (!yieldsOnly)
		{
			if (iPlot != CITY_HOME_PLOT)
			{
				changeWorkingPopulation(iChange);
			}

			// update plot builder special case where a plot is being worked but is (a) unimproved  or (b) un-bonus'ed
			pPlot->updatePlotBuilder();

			if (getTeam() == GC.getGame().getActiveTeam() || GC.getGame().isDebugMode())
			{
				pPlot->updateSymbolDisplay();
			}
		}

		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			changePlotYield((YieldTypes)iI, iChange * pPlot->getYield((YieldTypes)iI));
		}
	}

	if (isCitySelected())
	{
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
	}

#ifdef YIELD_VALUE_CACHING
	AI_NoteWorkerChange();
#endif
}

void CvCity::setWorkingPlot(int iIndex, bool bNewValue)
{
	FASSERT_BOUNDS(0, NUM_CITY_PLOTS, iIndex);

	if (isWorkingPlot(iIndex) != bNewValue)
	{
		m_pabWorkingPlot[iIndex] = bNewValue;

		processWorkingPlot(iIndex, bNewValue ? 1 : -1);
		if (bNewValue)
		{
			CvPlot* pPlot = getCityIndexPlot(iIndex);
			FAssertMsg(pPlot != NULL, CvString::format("pPlot was null for iIndex %d", iIndex).c_str());
		}
	}
}


void CvCity::alterWorkingPlot(int iIndex)
{
	FASSERT_BOUNDS(0, NUM_CITY_PLOTS, iIndex);

	if (iIndex != CITY_HOME_PLOT)
	{
		CvPlot* pPlot = getCityIndexPlot(iIndex);
		FAssertMsg(pPlot != NULL, CvString::format("pPlot was null for iIndex %d", iIndex).c_str());
		if (pPlot != NULL)
		{
			if (canWork(pPlot))
			{
				setCitizensAutomated(false);

				if (isWorkingPlot(iIndex))
				{
					setWorkingPlot(iIndex, false);

					if (GC.getDEFAULT_SPECIALIST() != NO_SPECIALIST)
					{
						changeSpecialistCount((SpecialistTypes)GC.getDEFAULT_SPECIALIST(), 1);
					}
					else AI_addBestCitizen(false, true);
				}
				else if (extraPopulation() > 0 || AI_removeWorstCitizen())
				{
					setWorkingPlot(iIndex, true);
				}
			}
			else if (pPlot->getOwner() == getOwner())
			{
				pPlot->setWorkingCityOverride(this);
			}
		}
	}
	else setCitizensAutomated(true);
}


bool CvCity::isActiveBuilding(BuildingTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	return !isDisabledBuilding(eIndex) && hasBuilding(eIndex);
}

bool CvCity::hasFullyActiveBuilding(const BuildingTypes eIndex) const
{
	return !isReligiouslyLimitedBuilding(eIndex) && isActiveBuilding(eIndex);
}


void CvCity::changeHasBuilding(const BuildingTypes eIndex, const bool bNewValue)
{
	if (bNewValue)
	{
		setHasBuilding(eIndex, true, getOwner(), GC.getGame().getGameTurnYear());
	}
	else setHasBuilding(eIndex, false, NO_PLAYER, MIN_INT);
}


void CvCity::setHasBuilding(const BuildingTypes eType, const bool bNewValue, const PlayerTypes eOriginalOwner, const int iOriginalTime, const bool bFirst)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eType);

	if (bNewValue != hasBuilding(eType))
	{
		const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eType);

		// Changing the buildings in a city invaldiates lots of cached data so flush the caches
		if (kBuilding.EnablesOtherBuildings())
		{
			AI_FlushBuildingValueCache(true);
			FlushCanConstructCache();
		}
#ifdef YIELD_VALUE_CACHING
		ClearYieldValueCache(); // A new building can change yield rates
#endif
#ifdef CAN_TRAIN_CACHING
		// Mark all unit canTrain values cached as dirty
		invalidateCachedCanTrainForUnit(NO_UNIT);
#endif

		alterBuildingLedger(eType, bNewValue, eOriginalOwner, iOriginalTime);

		setupBuilding(kBuilding, eType, bNewValue, bFirst);

		if (bNewValue) // Building addition
		{
			processBuilding(eType, 1, false, true);
		}
		else // Building removal
		{
			if (isDisabledBuilding(eType))
			{
				setDisabledBuilding(eType, false, false);
			}
			else
			{
				if (isReligiouslyLimitedBuilding(eType))
				{
					setReligiouslyLimitedBuilding(eType, false);
				}
				processBuilding(eType, -1, false, true);
			}

			//Remove any extensions of this building
			std::vector<BuildingTypes> temp;

			foreach_(const BuildingTypes eTypeX, getHasBuildings())
			{
				if (GC.getBuildingInfo(eTypeX).getExtendsBuilding() == eType)
				{
					temp.push_back(eTypeX);
				}
			}
			foreach_(const BuildingTypes eType, temp)
			{
				changeHasBuilding(eType, false);
			}
		}

		// Disable\Enable buildings replaced by this one.
		for (int iI = 0; iI < kBuilding.getNumReplacedBuilding(); iI++)
		{
			const BuildingTypes eReplaced = (BuildingTypes)kBuilding.getReplacedBuilding(iI);

			if (hasBuilding(eReplaced))
			{
				if (bNewValue)
				{
					setDisabledBuilding(eReplaced, true);
				}
				else
				{
					bool bEnableBuilding = true;
					const CvBuildingInfo& replaced = GC.getBuildingInfo(eReplaced);
					for (int iJ = 0; iJ < replaced.getNumReplacementBuilding(); ++iJ)
					{
						if (hasBuilding((BuildingTypes)replaced.getReplacementBuilding(iJ)))
						{
							bEnableBuilding = false;
							break;
						}
					}
					if (bEnableBuilding)
					{
						setDisabledBuilding(eReplaced, false);
					}
				}
			}
		}
	}
}


// Toffer - Called by setupBuilding() and recalculateModifiers(), game-count left out as it isn't recalculated and doesn't need to be either.
void CvCity::handleBuildingCounts(const BuildingTypes eBuilding, const int iChange, const bool bWonder)
{
	GET_PLAYER(getOwner()).changeBuildingCount(eBuilding, iChange);
	GET_TEAM(getTeam()).changeBuildingCount(eBuilding, iChange);
	changeNumBuildings(iChange);

	if (bWonder)
	{
		if (isWorldWonder(eBuilding))
		{
			changeNumWorldWonders(iChange);
		}
		else if (isTeamWonder(eBuilding))
		{
			changeNumTeamWonders(iChange);
		}
		else if (isNationalWonder(eBuilding))
		{
			changeNumNationalWonders(iChange);
		}
	}
}


// Toffer - Function added only for readability reasons.
void CvCity::setupBuilding(const CvBuildingInfo& kBuilding, const BuildingTypes eType, const bool bNewValue, const bool bFirst)
{
	PROFILE_EXTRA_FUNC();
	const int iChange = bNewValue ? 1 : -1;

	handleBuildingCounts(eType, iChange, isLimitedWonder(eType) && !kBuilding.isNoLimit());

	if (!bNewValue) // Building removal
	{
		if (!isWorldWonder(eType)) // World wonders can only be built once, so the count is essential to keep track of.
		{
			GC.getGame().changeNumBuildings(eType, iChange);
		}
	}
	else // Building addition
	{
		GC.getGame().changeNumBuildings(eType, iChange);

		if (kBuilding.needStateReligionInCity() && kBuilding.getVoteSourceType() > -1)
		{
			const VoteSourceTypes eVoteSource = (VoteSourceTypes) kBuilding.getVoteSourceType();
			if (eVoteSource > NO_VOTESOURCE && GC.getGame().getVoteSourceReligion(eVoteSource) == NO_RELIGION)
			{
				GC.getGame().setVoteSourceReligion(eVoteSource, GET_PLAYER(getOwner()).getStateReligion(), true);
			}
		}

		if (bFirst)
		{
		}
		if (kBuilding.isAllowsNukes())
		{
			GET_PLAYER(getOwner()).makeNukesValid(true);
		}

		// Toffer: Certain things should only apply when the building is built the very first time.
		if (bFirst) // Not city copy on owner change, actually built.
		{
			{
				const int iPopChange = kBuilding.getPopulationChange();
				if (iPopChange != 0)
				{
					if (iPopChange > 0)
					{
						for (int iI = 0; iI < iPopChange; iI++)
						{
							changeFood(growthThreshold(), true);
						}
					}
					else
					{
						for (int iI = 0; iI < -iPopChange; iI++)
						{
							changeFood(-std::max(growthThreshold(-1), getFood() + 1), true);
						}
					}
					// Don't starve with the extra citizen working nothing
					AI_updateAssignWork();
				}
			}

			if (kBuilding.isCapital())
			{
				GET_PLAYER(getOwner()).setCapitalCity(this);
			}

			if (NO_CORPORATION != (CorporationTypes)kBuilding.getFoundsCorporation()
			&& !GC.getGame().isCorporationFounded((CorporationTypes)kBuilding.getFoundsCorporation()))
			{
				setHeadquarters((CorporationTypes)kBuilding.getFoundsCorporation());
			}

			if (kBuilding.getFreeSpecialTech() != NO_TECH && !GET_TEAM(getTeam()).isHasTech(kBuilding.getFreeSpecialTech()))
			{
				GET_TEAM(getTeam()).setHasTech(kBuilding.getFreeSpecialTech(), true, getOwner(), true, true);
			}

			if (GC.getGame().isFinalInitialized() && !gDLL->GetWorldBuilderMode())
			{
				if (kBuilding.isGoldenAge())
				{
					GET_PLAYER(getOwner()).changeGoldenAgeTurns(1 + GET_PLAYER(getOwner()).getGoldenAgeLength());
				}

				if (kBuilding.getGlobalPopulationChange() > 0)
				{
					if (kBuilding.isTeamShare())
					{
						for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
						{
							if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getTeam()))
							{
								foreach_(CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
								{
									for (int iI = kBuilding.getGlobalPopulationChange() - 1; iI > -1; iI--)
									{
										pLoopCity->changeFood(pLoopCity->growthThreshold());
									}
									// so subsequent cities don't starve with the extra citizen working nothing
									pLoopCity->AI_updateAssignWork();
								}
							}
						}
					}
					else
					{
						foreach_(CvCity* pLoopCity, GET_PLAYER(getOwner()).cities())
						{
							for (int iI = kBuilding.getGlobalPopulationChange() - 1; iI > -1; iI--)
							{
								pLoopCity->changeFood(pLoopCity->growthThreshold());
							}
							// so subsequent cities don't starve with the extra citizen working nothing
							pLoopCity->AI_updateAssignWork();
						}
					}
				}

				if (kBuilding.getFreeTechs() > 0)
				{
					if (isHuman())
					{
						GET_PLAYER(getOwner()).chooseTech(
							kBuilding.getFreeTechs(),
							gDLL->getText("TXT_KEY_MISC_COMPLETED_WONDER_CHOOSE_TECH", kBuilding.getTextKeyWide())
						);
					}
					else
					{
						for (int iI = 0; iI < kBuilding.getFreeTechs(); iI++)
						{
							GET_PLAYER(getOwner()).AI_chooseFreeTech();
						}
					}
				}

				if (isWorldWonder(eType))
				{
					GC.getGame().addReplayMessage(
						REPLAY_MESSAGE_MAJOR_EVENT, getOwner(),
						gDLL->getText("TXT_KEY_MISC_COMPLETES_WONDER", GET_PLAYER(getOwner()).getNameKey(), kBuilding.getTextKeyWide()),
						getX(), getY(), GC.getCOLOR_BUILDING_TEXT()
					);
					for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHumanPlayer())
						{
							if (isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
							{
								AddDLLMessage(
									(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(),
									gDLL->getText("TXT_KEY_MISC_WONDER_COMPLETED", GET_PLAYER(getOwner()).getNameKey(), kBuilding.getTextKeyWide()),
									"AS2D_WONDER_BUILDING_BUILD", MESSAGE_TYPE_MAJOR_EVENT,
									kBuilding.getArtInfo()->getButton(), GC.getCOLOR_BUILDING_TEXT(), getX(), getY(), true, true
								);
							}
							else
							{
								AddDLLMessage(
									(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(),
									gDLL->getText("TXT_KEY_MISC_WONDER_COMPLETED_UNKNOWN", kBuilding.getTextKeyWide()),
									"AS2D_WONDER_BUILDING_BUILD", MESSAGE_TYPE_MAJOR_EVENT,
									kBuilding.getArtInfo()->getButton(), GC.getCOLOR_BUILDING_TEXT()
								);
							}
						}
					}
				}
			}
		}
	}
#ifdef THE_GREAT_WALL
	//great wall
	if (bFirst) // Not city copy on owner change, actually built or destroyed.
	{
		if (kBuilding.isAreaBorderObstacle())
		{
			bool bHas = false;
			foreach_(const BuildingTypes eTypeX, getHasBuildings())
			{
				if (eType != eTypeX && GC.getBuildingInfo(eTypeX).isAreaBorderObstacle() && !isDisabledBuilding(eTypeX))
				{
					bHas = true;
					break;
				}
			}
			if (bNewValue)
			{
				if (!bHas)
				{
					processGreatWall(true, true);
				}
			}
			else if (bHas)
			{
				processGreatWall(true, true);
			}
		}
	}
#endif // THE_GREAT_WALL
}

bool CvCity::processGreatWall(bool bIn, bool bForce, bool bSeeded)
{
	PROFILE_EXTRA_FUNC();
	/*
	> TBNote: I've found both a crash scenario in PBEM and an infinite hang scenario in single player.
	> A player complained about exceedingly strange graphic artifice when they encircle the globe with a singular culture that possesses the GW and the hang looked to have a similar basis.
	> Unfortunately, I'm no front end specialist and I don't have a theory on how to isolate the cause except to say that it's the new way that the GW is being processed that's causing the crashes and hangs.
	> So for now, this is disabled.  I may eventually create a manual option to turn it off or on and that would give us the ability to turn it off only if it's creating problems.
	> Better would be to FIX it but I've got no clue on that.
	See https://github.com/caveman2cosmos/Caveman2Cosmos/issues/44
	*/

#ifdef THE_GREAT_WALL
	if (!bForce && !GC.getENABLE_VIEWPORTS() && !GC.getDefineBOOL("DYNAMIC_GREAT_WALL"))
	{
		return true;
	}

	bool bHasGreatWall = false;
	if (bIn || !bSeeded)
	{
		foreach_(const BuildingTypes eTypeX, getHasBuildings())
		{
			if (GC.getBuildingInfo(eTypeX).isAreaBorderObstacle() && !isDisabledBuilding(eTypeX))
			{
				bHasGreatWall = true;
				break;
			}
		}
	}
	else bHasGreatWall = m_bIsGreatWallSeed;


	if (bHasGreatWall)
	{
		CvCity* pUseCity = NULL;

		if (isInViewport())
		{
			pUseCity = this;
		}
		else
		{
			//	Need to find a culturally connected city that IS in the current viewport
			int iDummyVal;
			CvUnitSelectionCriteria	noGrowthCriteria;

			noGrowthCriteria.m_bIgnoreGrowth = true;

			UnitTypes eDummyUnit = AI_bestUnitAI(UNITAI_ATTACK, iDummyVal, true, true, &noGrowthCriteria);

			if (eDummyUnit == NO_UNIT)
			{
				eDummyUnit = AI_bestUnitAI(UNITAI_CITY_DEFENSE, iDummyVal, true, true, &noGrowthCriteria);

				FAssert(eDummyUnit != NO_UNIT);
			}
			if (eDummyUnit != NO_UNIT)
			{
				CvUnit* pTempUnit = GET_PLAYER(getOwner()).getTempUnit(eDummyUnit, getX(), getY());
				CvReachablePlotSet	plotSet(pTempUnit->getGroup(), MOVE_OUR_TERRITORY, MAX_INT);

				for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
				{
					const CvCity* pCity = itr.plot()->getPlotCity();

					if (pCity != NULL && pCity->isInViewport())
					{
						pUseCity = pCity;
						break;
					}
				}
				GET_PLAYER(getOwner()).releaseTempUnit();
			}
		}

		//	If no suitable city is within the viewport we'll have to move the viewport
		bool bViewportMoved = false;
		int iOldViewportXOffset = 0;
		int iOldViewportYOffset = 0;

		if (pUseCity == NULL && !bSeeded)
		{
			pUseCity = this;
			bViewportMoved = true;

			GC.getCurrentViewport()->getMapOffset(iOldViewportXOffset, iOldViewportYOffset);
			GC.getCurrentViewport()->setOffsetToShow(getX(), getY());
		}
		//	remove or re-add
		if (pUseCity != NULL)
		{
			if (bIn)
			{
				pUseCity->m_bIsGreatWallSeed = true;
				gDLL->getEngineIFace()->AddGreatWall(pUseCity);
			}
			else
			{
				pUseCity->m_bIsGreatWallSeed = false;
				gDLL->getEngineIFace()->RemoveGreatWall(pUseCity);
			}
		}

		if (bViewportMoved)
		{
			GC.getCurrentViewport()->setMapOffset(iOldViewportXOffset, iOldViewportYOffset);
		}

		return true;
	}
#endif // THE_GREAT_WALL
	return false;
}


// Toffer - ToDo - Would make more sense to store this info in the CvArea object, mapped to player, rather than duplicating the info across all cities in the area.
void CvCity::changeFreeAreaBuildingCount(const BuildingTypes eIndex, const int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	FAssertMsg(iChange != 0, "This is not a change!");

	std::map<short, uint16_t>::const_iterator itr = m_freeAreaBuildingCount.find((short)eIndex);

	if (itr == m_freeAreaBuildingCount.end())
	{
		if (iChange > 0)
		{
			m_freeAreaBuildingCount.insert(std::make_pair((short)eIndex, iChange));
			setFreeBuilding(eIndex, true);
		}
		else FErrorMsg("Expected positive iChange for first building of a kind");
	}
	else if (iChange < 0 && (int)(itr->second) <= -iChange)
	{
		FAssertMsg((int)(itr->second) >= -iChange, "This change would bring the count to a negative value! Code copes with it though")
		m_freeAreaBuildingCount.erase(itr->first);
		setFreeBuilding(eIndex, false);
	}
	else // change building count
	{
		m_freeAreaBuildingCount[itr->first] += iChange;
	}
}

uint16_t CvCity::getFreeAreaBuildingCount(const short iIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), iIndex);
	std::map<short, uint16_t>::const_iterator itr = m_freeAreaBuildingCount.find(iIndex);
	return itr != m_freeAreaBuildingCount.end() ? itr->second : 0;
}

void CvCity::setFreeBuilding(const BuildingTypes eIndex, const bool bNewValue)
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);

	std::vector<short>::iterator itr = find(m_vFreeBuildings.begin(), m_vFreeBuildings.end(), (short)eIndex);

	if (bNewValue)
	{
		if (itr == m_vFreeBuildings.end()
		// Buildings built manually shouldn't disappear when the free building effect ends,
		&& !hasBuilding(eIndex) // therefore we won't count it as a free building just yet if we have the building.
		&& !GET_TEAM(getTeam()).isObsoleteBuilding(eIndex)
		&& isValidBuildingLocation(eIndex))
		{
			m_vFreeBuildings.push_back(eIndex);
			changeHasBuilding(eIndex, true);
		}
	}
	else if (itr != m_vFreeBuildings.end()
	&& getFreeAreaBuildingCount(eIndex) == 0
	&& GET_PLAYER(getOwner()).getFreeBuildingCount(eIndex) == 0)
	{
		m_vFreeBuildings.erase(itr);
		if (hasBuilding(eIndex))
		{
			changeHasBuilding(eIndex, false);
		}
		else FErrorMsg("This shouldn't really happen...");
	}
}

bool CvCity::isFreeBuilding(const short iIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), iIndex);
	return algo::any_of_equal(m_vFreeBuildings, iIndex);
}

void CvCity::checkFreeBuildings()
{
	PROFILE_EXTRA_FUNC();
	GET_PLAYER(getOwner()).checkFreeBuildings(this);

	for (std::map<short, uint16_t>::const_iterator itr = m_freeAreaBuildingCount.begin(); itr != m_freeAreaBuildingCount.end(); ++itr)
	{
		if (!isFreeBuilding((BuildingTypes)itr->first))
		{
			// This one will not register the free building if it is still not valid or if it was manually built.
			setFreeBuilding((BuildingTypes)itr->first, true);
		}
	}
}


bool CvCity::isHasReligion(ReligionTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumReligionInfos(), eIndex);
	return m_pabHasReligion[eIndex];
}

void CvCity::checkReligiousDisablingAllBuildings()
{
	PROFILE_EXTRA_FUNC();
	if (!GC.getGame().isOption(GAMEOPTION_RELIGION_DISABLING) || getReligionCount() == 0)
	{
		return;
	}
	const CvPlayer& player = GET_PLAYER(getOwner());

	for (int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
	{
		checkReligiousDisabling((BuildingTypes)iJ, player);
	}
	setMaintenanceDirty(true);
	updateReligionHappiness();
	updateReligionCommerce();
}

void CvCity::checkReligiousDisabling(const BuildingTypes eBuilding, const CvPlayer& player)
{
	PROFILE_EXTRA_FUNC();
	if (isDisabledBuilding(eBuilding))
	{
		return;
	}
	const CvBuildingInfo& building = GC.getBuildingInfo(eBuilding);

	const ReligionTypes eReligion = (ReligionTypes)building.getReligionType();
	const ReligionTypes eReligionReq = (ReligionTypes)building.getPrereqReligion();

	// If building is not of a religious nature
	if (eReligion == NO_RELIGION && eReligionReq == NO_RELIGION
	// or if city doesn't have the building's religion(s)
	|| (eReligion == NO_RELIGION || !isHasReligion(eReligion))
	&& (eReligionReq == NO_RELIGION || !isHasReligion(eReligionReq))
	// or the city doesn't have the building
	|| !hasBuilding(eBuilding))
	{
		return; // Nothing needs to be done
	}

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		const ReligionTypes eReligionX = (ReligionTypes)iI;

		if (eReligion == eReligionX || eReligionReq == eReligionX)
		{
			if (isReligiouslyLimitedBuilding(eBuilding))
			{
				if (player.hasAllReligionsActive() || player.getStateReligion() == eReligionX)
				{
					setReligiouslyLimitedBuilding(eBuilding, false);
				}
			}
			else if (!player.hasAllReligionsActive() && player.getStateReligion() != eReligionX)
			{
				setReligiouslyLimitedBuilding(eBuilding, true);
			}
		}
	}
}

void CvCity::applyReligionModifiers(const ReligionTypes eIndex, const bool bValue)
{
	PROFILE_EXTRA_FUNC();
	foreach_(const BuildingTypes eTypeX, getHasBuildings())
	{
		if (GC.getBuildingInfo(eTypeX).getPrereqReligion() == eIndex)
		{
			if (bValue)
			{
				if (isDisabledBuilding(eTypeX))
				{
					setDisabledBuilding(eTypeX, false);
				}
			}
			else if (!isDisabledBuilding(eTypeX))
			{
				setDisabledBuilding(eTypeX, true);
			}
		}
	}
	setMaintenanceDirty(true);
	updateReligionHappiness();
	updateReligionCommerce();
}

void CvCity::setHasReligion(ReligionTypes eIndex, bool bNewValue, bool bAnnounce, bool bArrows)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumReligionInfos(), eIndex);

	if (isHasReligion(eIndex) != bNewValue)
	{
		for (int iVoteSource = 0; iVoteSource < GC.getNumVoteSourceInfos(); ++iVoteSource)
		{
			processVoteSourceBonus((VoteSourceTypes)iVoteSource, false);
		}

		m_pabHasReligion[eIndex] = bNewValue;

		for (int iVoteSource = 0; iVoteSource < GC.getNumVoteSourceInfos(); ++iVoteSource)
		{
			processVoteSourceBonus((VoteSourceTypes)iVoteSource, true);
		}

		GET_PLAYER(getOwner()).changeHasReligionCount(eIndex, ((isHasReligion(eIndex)) ? 1 : -1));

		// Religion changes may change what is buildable
		FlushCanConstructCache();

		AI_setAssignWorkDirty(true);

		setInfoDirty(true);

		if (isHasReligion(eIndex))
		{
			GC.getGame().makeReligionFounded(eIndex, getOwner());

			if (bAnnounce)
			{
				if (GC.getGame().getHolyCity(eIndex) != this)
				{
					for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).isAlive()
						&&  GET_PLAYER((PlayerTypes)iI).isHumanPlayer()
						&& isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false)
						&& (getOwner() == iI || GET_PLAYER((PlayerTypes)iI).getStateReligion() == eIndex || GET_PLAYER((PlayerTypes)iI).hasHolyCity(eIndex)))
						{
							AddDLLMessage(
								(PlayerTypes) iI, false, GC.getEVENT_MESSAGE_TIME_LONG(),
								gDLL->getText("TXT_KEY_MISC_RELIGION_SPREAD", GC.getReligionInfo(eIndex).getTextKeyWide(), getNameKey()),
								GC.getReligionInfo(eIndex).getSound(), MESSAGE_TYPE_MAJOR_EVENT, GC.getReligionInfo(eIndex).getButton(),
								GC.getCOLOR_WHITE(), getX(), getY(), bArrows, bArrows
							);
						}
					}
				}

				if (isHuman()
				&& GET_PLAYER(getOwner()).getHasReligionCount(eIndex) == 1
				&& GET_PLAYER(getOwner()).canConvert(eIndex)
				&& GET_PLAYER(getOwner()).getStateReligion() == NO_RELIGION)
				{
					CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHANGERELIGION);
					if (NULL != pInfo)
					{
						pInfo->setData1(eIndex);
						gDLL->getInterfaceIFace()->addPopup(pInfo, getOwner());
					}
				}
			}
		}
		else if (bAnnounce)
		{
			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive()
				&&  GET_PLAYER((PlayerTypes)iI).isHumanPlayer()
				&& isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false)
				&& (getOwner() == iI || GET_PLAYER((PlayerTypes)iI).getStateReligion() == eIndex || GET_PLAYER((PlayerTypes)iI).hasHolyCity(eIndex)))
				{
					AddDLLMessage(
						(PlayerTypes) iI, false, GC.getEVENT_MESSAGE_TIME_LONG(),
						gDLL->getText("TXT_KEY_MISC_RELIGION_DECAY", getNameKey(), GC.getReligionInfo(eIndex).getTextKeyWide()),
						GC.getReligionInfo(eIndex).getSound(), MESSAGE_TYPE_MAJOR_EVENT, GC.getReligionInfo(eIndex).getButton(),
						GC.getCOLOR_RED(), getX(), getY(), bArrows, bArrows
					);
				}
			}
		}

		// Python Event
		if (bNewValue)
		{
			CvEventReporter::getInstance().religionSpread(eIndex, getOwner(), this);
		}
		else CvEventReporter::getInstance().religionRemove(eIndex, getOwner(), this);

		applyReligionModifiers(eIndex, bNewValue);

		checkReligiousDisablingAllBuildings();
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (!GET_PLAYER((PlayerTypes) iI).isHumanPlayer()
		&& (getTeam() == GET_PLAYER((PlayerTypes)iI).getTeam() || GET_TEAM(getTeam()).isVassal(GET_PLAYER((PlayerTypes)iI).getTeam())))
		{
			GET_PLAYER((PlayerTypes)iI).AI_setHasInquisitionTarget();
		}
	}
}


void CvCity::processVoteSourceBonus(VoteSourceTypes eVoteSource, bool bActive)
{
	PROFILE_EXTRA_FUNC();
	if (!GET_PLAYER(getOwner()).isLoyalMember(eVoteSource))
	{
		return;
	}

	if (GC.getGame().isDiploVote(eVoteSource))
	{
		ReligionTypes eReligion = GC.getGame().getVoteSourceReligion(eVoteSource);

		SpecialistTypes eSpecialist = (SpecialistTypes)GC.getVoteSourceInfo(eVoteSource).getFreeSpecialist();

		if (NO_SPECIALIST != eSpecialist && (NO_RELIGION == eReligion || isHasReligion(eReligion)))
		{
			changeFreeSpecialistCount(eSpecialist, bActive ? 1 : -1);
		}

		if (NO_RELIGION != eReligion && isHasReligion(eReligion))
		{
			for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
			{
				int iChange = GC.getVoteSourceInfo(eVoteSource).getReligionYield(iYield);
				if (!bActive)
				{
					iChange = -iChange;
				}

				if (0 != iChange)
				{
					for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); ++iBuilding)
					{
						if (GC.getBuildingInfo((BuildingTypes)iBuilding).getReligionType() == eReligion)
						{
							changeBuildingYieldChange((BuildingTypes)iBuilding, (YieldTypes)iYield, iChange);
						}
					}
				}
			}

			for (int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; ++iCommerce)
			{
				int iChange = GC.getVoteSourceInfo(eVoteSource).getReligionCommerce(iCommerce);
				if (!bActive)
				{
					iChange = -iChange;
				}

				if (0 != iChange)
				{
					for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); ++iBuilding)
					{
						if (GC.getBuildingInfo((BuildingTypes)iBuilding).getReligionType() == eReligion)
						{
							changeBuildingCommerceChange((BuildingTypes)iBuilding, (CommerceTypes)iCommerce, iChange);
						}
					}
				}
			}
		}
	}
}


bool CvCity::isHasCorporation(CorporationTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumCorporationInfos(), eIndex);
	return m_pabHasCorporation[eIndex];
}

void CvCity::applyCorporationModifiers(CorporationTypes eIndex, bool bValue)
{
	PROFILE_EXTRA_FUNC();
	foreach_(const BuildingTypes eTypeX, getHasBuildings())
	{
		if (GC.getBuildingInfo(eTypeX).getPrereqCorporation() == eIndex)
		{
			if (bValue)
			{
				if (isDisabledBuilding(eTypeX))
				{
					setDisabledBuilding(eTypeX, false);
				}
			}
			else if (!isDisabledBuilding(eTypeX))
			{
				setDisabledBuilding(eTypeX, true);
			}
		}
	}
	changeMilitaryProductionModifier(GC.getCorporationInfo(eIndex).getMilitaryProductionModifier() * (bValue ? 1 : -1));
	changeFreeExperience(GC.getCorporationInfo(eIndex).getFreeXP() * (bValue ? 1 : -1));

	CvCity* pHeadquarters = GC.getGame().getHeadquarters(eIndex);

	if (NULL != pHeadquarters)
	{
		pHeadquarters->updateCorporation();
	}

	updateCorporation();
}

void CvCity::setHasCorporation(CorporationTypes eIndex, bool bNewValue, bool bAnnounce, bool bArrows)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumCorporationInfos(), eIndex);

	if (isHasCorporation(eIndex) != bNewValue)
	{
		if (bNewValue)
		{
			bool bReplacedHeadquarters = false;
			for (int iCorp = 0; iCorp < GC.getNumCorporationInfos(); ++iCorp)
			{
				if (iCorp != eIndex && isHasCorporation((CorporationTypes)iCorp)
				&& GC.getGame().isCompetingCorporation((CorporationTypes)iCorp, eIndex))
				{
					if (GC.getGame().getHeadquarters((CorporationTypes)iCorp) == this)
					{
						GC.getGame().replaceCorporation((CorporationTypes)iCorp, eIndex);
						bReplacedHeadquarters = true;
					}
					else setHasCorporation((CorporationTypes)iCorp, false, false);
				}
			}
			if (bReplacedHeadquarters)
			{
				return; // already set the corporation in this city
			}
		}

		m_pabHasCorporation[eIndex] = bNewValue;

		GET_PLAYER(getOwner()).changeHasCorporationCount(eIndex, ((isHasCorporation(eIndex)) ? 1 : -1));

		applyCorporationModifiers(eIndex, bNewValue);

		AI_setAssignWorkDirty(true);

		setInfoDirty(true);

		if (isHasCorporation(eIndex))
		{
			GC.getGame().makeCorporationFounded(eIndex, getOwner());
		}

		if (bAnnounce)
		{
			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHumanPlayer()
				&& (getOwner() == iI || GET_PLAYER((PlayerTypes)iI).hasHeadquarters(eIndex)))
				{
					if (bNewValue)
					{
						{
							AddDLLMessage(
								(PlayerTypes) iI, false, GC.getEVENT_MESSAGE_TIME(),
								gDLL->getText(
									"TXT_KEY_MISC_CORPORATION_SPREAD",
									GC.getCorporationInfo(eIndex).getTextKeyWide(), getNameKey()
								),
								GC.getCorporationInfo(eIndex).getSound(), MESSAGE_TYPE_MAJOR_EVENT,
								GC.getCorporationInfo(eIndex).getButton(),
								GC.getCOLOR_WHITE(), getX(), getY(), bArrows, bArrows
							);
						}

						if (getOwner() == iI)
						{
							CvWStringBuffer szBonusString;
							GAMETEXT.setCorporationHelpCity(szBonusString, eIndex, this);

							CvWString szBonusList;
							bool bFirst = true;
							foreach_(const BonusTypes eBonus, GC.getCorporationInfo(eIndex).getPrereqBonuses())
							{
								CvWString szTemp;
								szTemp.Format(L"%s", GC.getBonusInfo(eBonus).getDescription());
								setListHelp(szBonusList, L"", szTemp, L", ", bFirst);
								bFirst = false;
							}
							if (!bFirst)
							{
								AddDLLMessage(
									(PlayerTypes) iI, false, GC.getEVENT_MESSAGE_TIME(),
									gDLL->getText("TXT_KEY_MISC_CORPORATION_SPREAD_BONUS", GC.getCorporationInfo(eIndex).getTextKeyWide(), szBonusString.getCString(), getNameKey(), szBonusList.GetCString()),
									GC.getCorporationInfo(eIndex).getSound(), MESSAGE_TYPE_MINOR_EVENT, GC.getCorporationInfo(eIndex).getButton(), GC.getCOLOR_WHITE(), getX(), getY(), bArrows, bArrows
								);
							}

						}
					}
					else
					{
						AddDLLMessage(
							(PlayerTypes) iI, false, GC.getEVENT_MESSAGE_TIME(),
							gDLL->getText("TXT_KEY_MISC_CORPORATION_DECAY", GC.getCorporationInfo(eIndex).getTextKeyWide(), getNameKey()),
							GC.getCorporationInfo(eIndex).getSound(), MESSAGE_TYPE_MAJOR_EVENT, GC.getCorporationInfo(eIndex).getButton(),
							GC.getCOLOR_RED(), getX(), getY(), bArrows, bArrows
						);
					}
				}
			}
		}

		// Python Event
		if (bNewValue)
		{
			CvEventReporter::getInstance().corporationSpread(eIndex, getOwner(), this);
		}
		else CvEventReporter::getInstance().corporationRemove(eIndex, getOwner(), this);
	}
}


CvCity* CvCity::getTradeCity(int iIndex) const
{
	FASSERT_BOUNDS(0, (int)m_paTradeCities.size(), iIndex);
	return getCity(m_paTradeCities[iIndex]);
}


int CvCity::getTradeRoutes() const
{
	int iTradeRoutes = GC.getGame().getTradeRoutes();
	iTradeRoutes += GET_PLAYER(getOwner()).getTradeRoutes();

	if (isCoastal(GC.getWorldInfo(GC.getMap().getWorldSize()).getOceanMinAreaSize()))
	{
		iTradeRoutes += GET_PLAYER(getOwner()).getCoastalTradeRoutes();
	}
	iTradeRoutes += getExtraTradeRoutes();

	return std::max(0, std::min(iTradeRoutes, getMaxTradeRoutes()));
}


void CvCity::clearTradeRoutes()
{
	PROFILE_EXTRA_FUNC();
	for (int cityIdx = 0; cityIdx < static_cast<int>(m_paTradeCities.size()); cityIdx++)
	{
		CvCity* pLoopCity = getTradeCity(cityIdx);

		if (pLoopCity != NULL)
		{
			pLoopCity->setTradeRoute(getOwner(), false);
		}
	}
	m_paTradeCities = std::vector<IDInfo>(getMaxTradeRoutes());
}


// XXX eventually, this needs to be done when roads are built/destroyed...
void CvCity::updateTradeRoutes()
{
	PROFILE_FUNC();
	const int iMaxTradeRoutes = getMaxTradeRoutes();

	std::vector<int> paiBestValue(iMaxTradeRoutes, 0);

	clearTradeRoutes();

	if (!isDisorder() && !isPlundered() && !isQuarantined())
	{
		const int iTradeRoutes = getTradeRoutes();

		FAssert(iTradeRoutes <= iMaxTradeRoutes);

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER(getOwner()).canHaveTradeRoutesWith((PlayerTypes)iI))
			{
				foreach_(CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
				{
					if (pLoopCity != this)
					{
						if (!(pLoopCity->isTradeRoute(getOwner())) || (getTeam() == GET_PLAYER((PlayerTypes)iI).getTeam()))
						{
							if (pLoopCity->plotGroup(getOwner()) == plotGroup(getOwner()) || GC.getIGNORE_PLOT_GROUP_FOR_TRADE_ROUTES())
							{
								// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
								const int iValue = calculateTradeProfitTimes100(pLoopCity);
#else
								const int iValue = calculateTradeProfit(pLoopCity);
#endif
								// BUG - Fractional Trade Routes - end

								for (int iJ = 0; iJ < iTradeRoutes; iJ++)
								{
									if (iValue > paiBestValue[iJ])
									{
										for (int iK = (iTradeRoutes - 1); iK > iJ; iK--)
										{
											paiBestValue[iK] = paiBestValue[(iK - 1)];
											m_paTradeCities[iK] = m_paTradeCities[(iK - 1)];
										}

										paiBestValue[iJ] = iValue;
										m_paTradeCities[iJ] = pLoopCity->getIDInfo();

										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	int iTradeProfit = 0;

	for (int iI = 0; iI < iMaxTradeRoutes; iI++)
	{
		CvCity* pLoopCity = getTradeCity(iI);

		if (pLoopCity != NULL)
		{
			pLoopCity->setTradeRoute(getOwner(), true);

			// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
			iTradeProfit += calculateTradeProfitTimes100(pLoopCity);
#else
			iTradeProfit += calculateTradeProfit(pLoopCity);
#endif
			// BUG - Fractional Trade Routes - end
		}
	}

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
		setTradeYield(((YieldTypes)iI), calculateTradeYield(((YieldTypes)iI), iTradeProfit) / 100); // XXX could take this out if handled when CvPlotGroup changes...
#else
		setTradeYield(((YieldTypes)iI), calculateTradeYield(((YieldTypes)iI), iTradeProfit)); // XXX could take this out if handled when CvPlotGroup changes...
#endif
// BUG - Fractional Trade Routes - end
	}
}


void CvCity::clearOrderQueue()
{
	PROFILE_EXTRA_FUNC();
	while (getOrderQueueLength() > 0)
	{
		popOrder(0, false, false, false);
	}

	if (getTeam() == GC.getGame().getActiveTeam() || GC.getGame().isDebugMode())
	{
		setInfoDirty(true);
	}
}


bool CvCity::pushFirstValidBuildListOrder(int iListID)
{
	PROFILE_EXTRA_FUNC();
	const CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	int index = kPlayer.m_pBuildLists->getIndexByID(iListID);
	if (index < 0)
		return false;
	const int iNum = kPlayer.m_pBuildLists->getListLength(index);
	for (int i = 0; i < iNum; i++)
	{
		const OrderData* pOrder = kPlayer.m_pBuildLists->getOrder(index, i);

		if (canContinueProduction(*pOrder))
		{
			pushOrder(pOrder->eOrderType, pOrder->iData1, pOrder->iData2, pOrder->bSave, false, true); //Calvitix, append buildings, not insert
			return true;
		}
	}
	return false;
}

void CvCity::pushOrder(OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bForce, CvPlot* deliveryDestination, UnitAITypes contractedAIType, uint8_t contractFlags)
{
	//bool bBuildingUnit = false;
	//bool bBuildingBuilding = false;

	if (bPop)
	{
		popOrder(0, false, false, false);
	}

	bool bValid = false;
	bool bJustAddedProcess = false;
	CvPlayerAI& owner = GET_PLAYER(getOwner());
	bool bIsHuman = owner.isHumanPlayer(true);

	OrderData order;

	switch (eOrder)
	{
		case ORDER_TRAIN:
		{
			const UnitTypes unitType = static_cast<UnitTypes>(iData1);
			if (canTrain(unitType) || bForce)
			{
				const uint16_t iAIType = EXTERNAL_ORDER_IDATA(iData2);
				const UnitAITypes AIType = (iAIType == 0xFFFF) ?
					GC.getUnitInfo(unitType).getDefaultUnitAIType()
					:
					static_cast<UnitAITypes>(iAIType)
					;

				contractedAIType = (iAIType == 0xFFFF)?
					static_cast<UnitAITypes>(0xFF)
					:
					contractedAIType
					;

				const short plotIndex = (deliveryDestination != NULL) ?
					order.unit.plotIndex = GC.getMap().plotNum(deliveryDestination->getX(), deliveryDestination->getY())
					:
					order.unit.plotIndex = 0xFFFF;



				order = OrderData::createUnitOrder(unitType, AIType, plotIndex, contractFlags, contractedAIType, bSave);

				int alreadyQueued = 0;
				for (std::vector<OrderData>::const_iterator it = m_orderQueue.begin(); it != m_orderQueue.end(); ++it) {
					if (it->eOrderType == ORDER_TRAIN && it->getUnitType() == order.getUnitType()) {
						alreadyQueued+= 1;
					}
				}

				//don't add the same unit if already 2 of them (if prod take more than 2 turns), and the first item in queue is not already finished, and the cost of the order is more than 3 turns
				if (!bIsHuman && (bAppend && !bForce) && ((alreadyQueued > 1 && getProductionTurnsLeft(unitType, 2) > 2) || alreadyQueued > 4)) {
					if (gCityLogLevel >= 3)
					{
						const CvWString szStringUnitAi = GC.getUnitAIInfo(order.getUnitAIType()).getType();
						logBBAI("    City %S unit %S for type %S is already in queue", getName().GetCString(), GC.getUnitInfo(unitType).getDescription(getCivilizationType()), szStringUnitAi.GetCString());
					}
					return;
				}

				owner.changeUnitMaking(unitType, 1);

				area()->changeNumTrainAIUnits(getOwner(), order.getUnitAIType(), 1);
				owner.AI_changeNumTrainAIUnits(order.getUnitAIType(), 1);

				CvEventReporter::getInstance().cityBuildingUnit(this, unitType);
				setUnitListInvalid();
				if (gCityLogLevel >= 1)
				{
					CvWString verb = "pushes";
					if (!bAppend) verb = "inserts";
					const CvWString szStringUnitAi = GC.getUnitAIInfo(order.getUnitAIType()).getType();
					logBBAI("    City %S %S production of unit %S for type %S", getName().GetCString(), verb.GetCString(), GC.getUnitInfo(unitType).getDescription(getCivilizationType()), szStringUnitAi.GetCString());
				}
				bValid = true;
			}
			break;
		}
		case ORDER_CONSTRUCT:
		{
			const BuildingTypes buildingType = static_cast<BuildingTypes>(iData1);
			if (canConstruct(buildingType) || bForce)
			{
				order = OrderData::createBuildingOrder(buildingType, bSave);

				int alreadyQueued = 0;
				for (std::vector<OrderData>::const_iterator it = m_orderQueue.begin(); it != m_orderQueue.end(); ++it) {
					if (it->eOrderType == ORDER_CONSTRUCT && it->getBuildingType() == order.getBuildingType()) {
						alreadyQueued += 1;
					}
				}

				//don't add the same building after
				if (!bIsHuman && (bAppend && !bForce) && alreadyQueued > 0) {
					if (gCityLogLevel >= 3)
					{
						logBBAI("    City %S building %S already in queue", getName().GetCString(), GC.getBuildingInfo(buildingType).getDescription());
					}
					return;
				}

				NoteBuildingNoLongerConstructable(buildingType);

				owner.changeBuildingMaking(buildingType, 1);

				const SpecialBuildingTypes eSpecialBuilding = GC.getBuildingInfo(buildingType).getSpecialBuilding();
				if (eSpecialBuilding != NO_SPECIALBUILDING)
				{
					owner.changeBuildingGroupMaking(eSpecialBuilding, 1);
				}

				CvEventReporter::getInstance().cityBuildingBuilding(this, buildingType);
				setBuildingListInvalid();
				if (gCityLogLevel >= 1)
				{
					CvWString verb = "pushes";
					if (!bAppend) verb = "inserts";
					logBBAI("    City %S %S production of building %S", getName().GetCString(), verb.GetCString(), GC.getBuildingInfo(buildingType).getDescription());
				}
				bValid = true;
			}
			break;
		}
		case ORDER_CREATE:
		{
			const ProjectTypes projectType = static_cast<ProjectTypes>(iData1);
			if (canCreate(projectType) || bForce)
			{
				order = OrderData::createProjectOrder(projectType, bSave);
				GET_TEAM(getTeam()).changeProjectMaking(projectType, 1);
				CvEventReporter::getInstance().cityBuildingProject(this, projectType);
				if (gCityLogLevel >= 1)
				{
					logBBAI("    City %S pushes production of project %S", getName().GetCString(), GC.getProjectInfo(projectType).getDescription());
				}
				bValid = true;
			}
			break;
		}
		case ORDER_MAINTAIN:
		{
			const ProcessTypes processType = static_cast<ProcessTypes>(iData1);
			if (canMaintain(processType) || bForce)
			{
				order = OrderData::createProcessOrder(processType, bSave);
				CvEventReporter::getInstance().cityBuildingProcess(this, processType);
				if (gCityLogLevel >= 1)
				{
					CvWString verb = "pushes";
					if (!bAppend) verb = "inserts";
					logBBAI("    City %S %S production of process %S", getName().GetCString(), verb.GetCString(), GC.getProcessInfo(processType).getDescription());
				}
				bValid = true;
				//if (!bForce) bAppend = true;
				bJustAddedProcess = true;
			}
			break;
		}
		case ORDER_LIST:
		{
			bValid = true;
			break;
		}
		default: FErrorMsg("iOrder did not match a valid option");
	}

	if (!bValid)
	{
		return;
	}



	if (m_orderQueue.empty() && bIsHuman)
	{
		owner.setIdleCity(getID(), false);
	}


	if (bAppend && !m_orderQueue.empty() && !(m_orderQueue.begin()->eOrderType == ORDER_MAINTAIN))
	{
		m_orderQueue.push_back(order);
	}
	else
	{
		stopHeadOrder();
		m_orderQueue.insert(m_orderQueue.begin(), order);
	}

	if (!bAppend || getOrderQueueLength() == 1)
	{
		// If the head order is a build list, resolve it
		if (eOrder != ORDER_LIST)
		{
			startHeadOrder();
		}
		else if (!pushFirstValidBuildListOrder(iData1))
		{
			// pop the list if there is nothing to construct on it any more
			popOrder(0);
		}
		else if (!bSave)
		{
			popOrder(1);
		}
	}

	//Check the Queue, and remove process if there's something else to do
	if (!bJustAddedProcess && getOrderQueueLength() > 1 && m_orderQueue[0].eOrderType == ORDER_MAINTAIN)
	{
		CvString str = GC.getProcessInfo(m_orderQueue[0].getProcessType()).getType();
		const PlayerTypes eOwner = getOwner();
		CvPlayerAI& player = GET_PLAYER(eOwner);
		const bool bFinancialTrouble = player.AI_isFinancialTrouble();
		if (!bFinancialTrouble)
		{ // IF financial Trouble, keep the process if needed
			popOrder(0);
		}
		LOG_BBAI_CITY(3, ("    City %S has process on head, and queue added. Removing  %S", getName().GetCString(), str.GetCString()));
	}


	// Why does this cause a crash???

/*	if (bBuildingUnit)
	{
		CvEventReporter::getInstance().cityBuildingUnit(this, (UnitTypes)iData1);
	}
	else if (bBuildingBuilding)
	{
		CvEventReporter::getInstance().cityBuildingBuilding(this, (BuildingTypes)iData1);
	}*/

	if ((getTeam() == GC.getGame().getActiveTeam()) || GC.getGame().isDebugMode())
	{
		setInfoDirty(true);
	}
	if (isCitySelected())
	{
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
	}
}

void CvCity::popOrder(int orderIndex, bool bFinish, bool bChoose, bool bResolveList)
{
	PROFILE_FUNC();

	if (m_orderQueue.empty() || m_orderQueue.size() <= static_cast<size_t>(orderIndex))
	{
		return;
	}

	if (orderIndex == -1) orderIndex = m_orderQueue.size() - 1;

	FAssertMsg(orderIndex >= 0 && orderIndex < static_cast<int>(m_orderQueue.size()), "Order index out of bounds");

	const OrderData order = m_orderQueue[orderIndex];
	const OrderData externalOrder = order.to_external();

	const bool bWasFoodProduction = isFoodProduction();

	if (bFinish && order.bSave)
	{
		pushOrder(externalOrder.eOrderType, externalOrder.iData1, externalOrder.iData2, true, false, true);
	}
	CvPlayerAI& owner = GET_PLAYER(getOwner());

	UnitTypes eTrainUnit = NO_UNIT;
	BuildingTypes eConstructBuilding = NO_BUILDING;
	ProjectTypes eCreateProject = NO_PROJECT;

	switch (order.eOrderType)
	{
		case ORDER_TRAIN:
		{
			FAssertMsg(order.getUnitType() > -1 && order.getUnitType() < GC.getNumUnitInfos() && order.getUnitAIType() > -1 && order.getUnitAIType() < NUM_UNITAI_TYPES, "Train unit order is invalid");
			eTrainUnit = order.getUnitType();
			const UnitAITypes eTrainAIUnit = order.getUnitAIType();
			FAssertMsg(eTrainUnit != NO_UNIT, "eTrainUnit is expected to be assigned a valid unit type");
			FAssertMsg(eTrainAIUnit != NO_UNITAI, "eTrainAIUnit is expected to be assigned a valid unit AI type");

			owner.changeUnitMaking(eTrainUnit, -1);

			area()->changeNumTrainAIUnits(getOwner(), eTrainAIUnit, -1);
			owner.AI_changeNumTrainAIUnits(eTrainAIUnit, -1);

			setUnitListInvalid();

			if (bFinish)
			{
				AI_trained(eTrainUnit, eTrainAIUnit);

				
				LOG_CITY_BLOCK(2, {
					const CvWString szStringUnitAi = GC.getUnitAIInfo(eTrainAIUnit).getType();
					logBBAI("      City %S builds unit %S for Type %S", getName().GetCString(), GC.getUnitInfo(eTrainUnit).getDescription(), szStringUnitAi.GetCString());
				});
				const int iProgress = getProgressOnUnit(eTrainUnit);
				const int iRawOverflow = iProgress - getProductionNeeded(eTrainUnit);
				const int iMaxOverflow = getMaxProductionOverflow();
				const int iOverflow = std::min(iMaxOverflow, iRawOverflow);
				if (iOverflow > 0)
				{
					changeOverflowProduction(iOverflow);
				}
				changeProgressOnUnit(eTrainUnit, -iProgress);

				m_iLostProductionModified = std::max(0, iRawOverflow - iMaxOverflow);
				m_iGoldFromLostProduction = m_iLostProductionModified * GC.getMAXED_UNIT_GOLD_PERCENT() / 100;

				CvUnit* pUnit = owner.initUnit(eTrainUnit, getX(), getY(), eTrainAIUnit, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
				if (!pUnit)
				{
					FErrorMsg("pUnit is expected to be assigned a valid unit object");
					return;
				}
				if (GC.getGame().isModderGameOption(MODDERGAMEOPTION_MAX_UNITS_PER_TILES)
				&& !pUnit->canEnterPlot(plot(), MoveCheck::IgnoreLocation))
				{
					pUnit->jumpToNearestValidPlot(false);
				}
				pUnit->finishMoves();

				addProductionExperience(pUnit);

				const short iPlotIndex = order.unit.plotIndex;
				int iFlags;

				CvPlot* pRallyPlot = NULL;
				if (iPlotIndex != (const short)0xFFFF)
				{
					iFlags = MOVE_NO_ENEMY_TERRITORY;
					pRallyPlot = GC.getMap().plotByIndex(iPlotIndex);
					if (pRallyPlot != NULL && gUnitLogLevel >= 3)
					{
						logBBAI("    New unit %S at (%d,%d) headed to contractual delivery plot (%d,%d)",
							pUnit->getUnitInfo().getDescription(),
							getX(), getY(),
							pRallyPlot->getX(), pRallyPlot->getY());
					}
				}
				else
				{
					iFlags = 0;
					pRallyPlot = getRallyPlot();
				}

				if (pRallyPlot)
				{
					const bool bIsUnitMission = (order.unit.contractFlags & AUX_CONTRACT_FLAG_IS_UNIT_CONTRACT) != 0;
					if (pRallyPlot != plot())
					{
						pUnit->getGroup()->pushMission(MISSION_MOVE_TO,
							pRallyPlot->getX(),
							pRallyPlot->getY(),
							iFlags,
							false,
							false,
							bIsUnitMission ? MISSIONAI_CONTRACT_UNIT : MISSIONAI_CONTRACT,
							pRallyPlot);
					}
					else
					{
						pUnit->getGroup()->AI_setMissionAI(bIsUnitMission ? MISSIONAI_CONTRACT_UNIT : MISSIONAI_CONTRACT, plot(), NULL);
					}
				}
				/*  There seems to be an issue with AI missionaries not working correctly - forcing automation  */
				/* is a kludgy way to fix this                                                                  */
				if (!isHuman())
				{
					pUnit->automate(AUTOMATE_RELIGION);
				}
				if (isHuman())
				{
					if (owner.isOption(PLAYEROPTION_START_AUTOMATED))
					{
						pUnit->automate(AUTOMATE_BUILD);
					}

					if (owner.isOption(PLAYEROPTION_MISSIONARIES_AUTOMATED))
					{
						pUnit->automate(AUTOMATE_RELIGION);
					}
					if (owner.isOption(PLAYEROPTION_MODDER_2))
					{
						CvPlot* pPlot = plot();
						if (pPlot != NULL)
						{
							if (pUnit->canSleep() || pUnit->canFortify())
							{
								pUnit->getGroup()->setActivityType(ACTIVITY_SLEEP);
							}
						}
					}
				}

#ifdef CAN_TRAIN_CACHING
				//	Training a unit can mean we might not be abel to build any more of them
				//	so clear its entry in the canTrain cache to force recalculation
				invalidateCachedCanTrainForUnit(eTrainUnit);
#endif

				//	KOSHLING - must not hold onto the pointer after the Python call or
				//	a crash occurs if that Python decides to destroy the just-built unit
				int iUnitId = pUnit->getID();

				CvEventReporter::getInstance().unitBuilt(this, pUnit);

				//	Python may have destroyed the unit we just built so refind by id
				pUnit = owner.getUnit(iUnitId);
				if (pUnit != NULL)
				{
					if (gCityLogLevel >= 1)
					{
						const CvWString szStringUnitAi = GC.getUnitAIInfo(pUnit->AI_getUnitAIType()).getType();
						logBBAI("    City %S finishes production of unit %S for Type %S", getName().GetCString(), pUnit->getName(0).GetCString(), szStringUnitAi.GetCString());
					}

					if (GC.getUnitInfo(eTrainUnit).getDomainType() == DOMAIN_AIR)
					{
						if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
						{
							if (plot()->countNumAirUnitCargoVolume(getTeam()) > getSMAirUnitCapacity(getTeam()))
							{
								//probably need to look into this too.
								pUnit->jumpToNearestValidPlot();
							}
						}
						else if (plot()->countNumAirUnits(getTeam()) > getAirUnitCapacity(getTeam()))
						{
							pUnit->jumpToNearestValidPlot();  // can destroy unit
						}
					}

				}
			}
			break;
		}
		case ORDER_CONSTRUCT:
		{
			eConstructBuilding = order.getBuildingType();

			owner.changeBuildingMaking(eConstructBuilding, -1);

			const SpecialBuildingTypes eSpecialBuilding = GC.getBuildingInfo(eConstructBuilding).getSpecialBuilding();
			if (eSpecialBuilding != NO_SPECIALBUILDING)
			{
				owner.changeBuildingGroupMaking(eSpecialBuilding, -1);
			}

			if (bFinish)
			{
				if (owner.isBuildingMaxedOut(eConstructBuilding))
				{
					owner.removeBuilding(eConstructBuilding);
				}

				changeHasBuilding(eConstructBuilding, true);

				AI_setPropertyControlBuildingQueued(false);

				if (gCityLogLevel >= 2)
				{
					logBBAI("      City %S builds building %S",
						getName().GetCString(),
						GC.getBuildingInfo(eConstructBuilding).getDescription());
				}
				const int iProgress = getProgressOnBuilding(eConstructBuilding);
				const int iRawOverflow = iProgress - getProductionNeeded(eConstructBuilding);
				const int iMaxOverflow = getMaxProductionOverflow();
				const int iOverflow = std::min(iMaxOverflow, iRawOverflow);
				if (iOverflow > 0)
				{
					changeOverflowProduction(iOverflow);
				}
				changeProgressOnBuilding(eConstructBuilding, -iProgress);

				m_iLostProductionModified = std::max(0, iRawOverflow - iMaxOverflow);
				m_iGoldFromLostProduction = m_iLostProductionModified * GC.getMAXED_BUILDING_GOLD_PERCENT() / 100;

				CvEventReporter::getInstance().buildingBuilt(this, eConstructBuilding);
			}
			else if (!canConstruct(eConstructBuilding))
			{
				const BuildingTypes eBuilding = GC.getBuildingInfo(eConstructBuilding).getProductionContinueBuilding();
				if (eBuilding != NO_BUILDING && canConstruct(eBuilding, true, false, false, false))
				{
					if (m_iLostProduction == 0)
					{
						m_iLostProduction = getProgressOnBuilding(eConstructBuilding);
						changeProgressOnBuilding(eConstructBuilding, -m_iLostProduction);
					}
					if (m_iLostProduction > 0)
					{
						changeProgressOnBuilding(eBuilding, m_iLostProduction);
						const CvWString szMessage = gDLL->getText("TXT_KEY_MISC_PROD_CONVERTED", m_iLostProduction, GC.getBuildingInfo(eConstructBuilding).getTextKeyWide(), GC.getBuildingInfo(eBuilding).getTextKeyWide());
						AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_WONDERGOLD", MESSAGE_TYPE_MINOR_EVENT, GC.getYieldInfo(YIELD_PRODUCTION).getButton(), GC.getCOLOR_GREEN(), getX(), getY(), true, true);

						m_iLostProduction = 0;
					}
				}
			}

			FlushCanConstructCache(eConstructBuilding);	//	Flush value for this building
			setBuildingListInvalid();
			break;
		}
		case ORDER_CREATE:
		{
			eCreateProject = order.getProjectType();

			GET_TEAM(getTeam()).changeProjectMaking(eCreateProject, -1);

			if (bFinish)
			{
				OutputDebugString(CvString::format("Project %d (%S) built\n", eCreateProject, GC.getProjectInfo(eCreateProject).getDescription()).c_str());

				// Event reported to Python before the project is built, so that we can show the movie before awarding free techs, for example
				CvEventReporter::getInstance().projectBuilt(this, eCreateProject);

				GET_TEAM(getTeam()).changeProjectCount(eCreateProject, 1);

				if (GC.getProjectInfo(eCreateProject).isSpaceship())
				{
					bool needsArtType = true;
					VictoryTypes eVictory = (VictoryTypes)GC.getProjectInfo(eCreateProject).getVictoryPrereq();

					if (NO_VICTORY != eVictory && GET_TEAM(getTeam()).canLaunch(eVictory))
					{
						if (isHuman())
						{
							CvPopupInfo* pInfo = NULL;

							if (GC.getGame().isNetworkMultiPlayer())
							{
								pInfo = new CvPopupInfo(BUTTONPOPUP_LAUNCH, GC.getProjectInfo(eCreateProject).getVictoryPrereq());
							}
							else
							{
								pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN, eCreateProject);
								pInfo->setText(L"showSpaceShip");
								needsArtType = false;
							}

							gDLL->getInterfaceIFace()->addPopup(pInfo, getOwner());
						}
						else
						{
							owner.AI_launch(eVictory);
						}
					}
					else
					{
						//show the spaceship progress
						if (isHuman())
						{
							if (!GC.getGame().isNetworkMultiPlayer())
							{
								CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN, eCreateProject);
								pInfo->setText(L"showSpaceShip");
								gDLL->getInterfaceIFace()->addPopup(pInfo, getOwner());
								needsArtType = false;
							}
						}
					}

					if (needsArtType)
					{
						int defaultArtType = GET_TEAM(getTeam()).getProjectDefaultArtType(eCreateProject);
						int projectCount = GET_TEAM(getTeam()).getProjectCount(eCreateProject);
						GET_TEAM(getTeam()).setProjectArtType(eCreateProject, projectCount - 1, defaultArtType);
					}
				}
				const int iRawOverflow = getProjectProduction(eCreateProject) - getProductionNeeded(eCreateProject);
				const int iMaxOverflow = getMaxProductionOverflow();
				const int iOverflow = std::min(iMaxOverflow, iRawOverflow);
				if (iOverflow > 0)
				{
					changeOverflowProduction(iOverflow);
				}
				setProjectProduction(eCreateProject, 0);

				m_iLostProductionModified = std::max(0, iRawOverflow - iMaxOverflow);
				m_iGoldFromLostProduction = m_iLostProductionModified * GC.getMAXED_PROJECT_GOLD_PERCENT() / 100;
			}
			break;
		}
		case ORDER_MAINTAIN:
		case ORDER_LIST:
		{
			break;
		}
		default: FErrorMsg("order.eOrderType is not a valid option");

	}
	const bool bStart = orderIndex == 0;
	if (bStart)
	{
		stopHeadOrder();
	}

	m_orderQueue.erase(m_orderQueue.begin() + orderIndex);

	if (bStart)
	{
		startHeadOrder();
	}

	if (bResolveList)
	{
		// Check if head of queue is a build list and resolve it in that case
		bst::optional<OrderData> nextOrder = getHeadOrder();
		if (nextOrder && nextOrder->eOrderType == ORDER_LIST)
		{
			if (!pushFirstValidBuildListOrder(nextOrder->orderlist.id))
			{
				// pop the list if there is nothing to construct on it any more
				popOrder(0);
			}
			else if (!nextOrder->bSave)
			{
				popOrder(1);
			}
		}
	}

	if (m_orderQueue.empty())
	{
		if (owner.isHumanPlayer(true))
		{
			owner.setIdleCity(getID(), true);
		}

		if (bChoose)
		{
			if (isHuman() && !isProductionAutomated())
			{
				if (bWasFoodProduction)
				{
					AI_assignWorkingPlots();
				}
			}
			else AI_chooseProduction();
		}
	}

	if (bFinish)
	{
		switch (order.eOrderType)
		{
			case ORDER_TRAIN:
			{
				m_outputHistory.addToHistory(ORDER_TRAIN, (uint16_t)order.getUnitType());
				break;
			}
			case ORDER_CONSTRUCT:
			{
				m_outputHistory.addToHistory(ORDER_CONSTRUCT, (uint16_t)order.getBuildingType());
				break;
			}
			case ORDER_CREATE:
			{
				m_outputHistory.addToHistory(ORDER_CREATE, (uint16_t)order.getProjectType());
				break;
			}
			default: FErrorMsg("Can Occur?");
		}
		const char* szIcon = NULL;
		wchar_t szBuffer[1024];
		char szSound[1024];
		bool bCompletionMessage = false;

		if (eTrainUnit != NO_UNIT)
		{
			if (getBugOptionBOOL("Civ4lerts__CompleteUnit", false) || isLimitedUnit(eTrainUnit) && getBugOptionBOOL("Civ4lerts__CompleteSpecial", true))
			{
				swprintf(szBuffer, gDLL->getText("TXT_KEY_MISC_TRAINED_UNIT_IN", GC.getUnitInfo(eTrainUnit).getTextKeyWide(), getNameKey()).GetCString());
				strcpy(szSound, GC.getUnitInfo(eTrainUnit).getArtInfo(0, owner.getCurrentEra(), NO_UNIT_ARTSTYLE)->getTrainSound());
				szIcon = owner.getUnitButton(eTrainUnit);
				bCompletionMessage = true;
			}
		}
		else if (eConstructBuilding != NO_BUILDING)
		{
			if (getBugOptionBOOL("Civ4lerts__CompleteBuilding", false) || isLimitedWonder(eConstructBuilding) && getBugOptionBOOL("Civ4lerts__CompleteSpecial", true))
			{
				swprintf(szBuffer, gDLL->getText("TXT_KEY_MISC_CONSTRUCTED_BUILD_IN", GC.getBuildingInfo(eConstructBuilding).getTextKeyWide(), getNameKey()).GetCString());
				strcpy(szSound, GC.getBuildingInfo(eConstructBuilding).getConstructSound());
				szIcon = GC.getBuildingInfo(eConstructBuilding).getButton();
				bCompletionMessage = true;
			}
		}
		else if (eCreateProject != NO_PROJECT)
		{
			if (getBugOptionBOOL("Civ4lerts__CompleteProject", true) || isLimitedProject(eCreateProject) && getBugOptionBOOL("Civ4lerts__CompleteSpecial", true))
			{
				swprintf(szBuffer, gDLL->getText("TXT_KEY_MISC_CREATED_PROJECT_IN", GC.getProjectInfo(eCreateProject).getTextKeyWide(), getNameKey()).GetCString());
				strcpy(szSound, GC.getProjectInfo(eCreateProject).getCreateSound());
				szIcon = GC.getProjectInfo(eCreateProject).getButton();
				bCompletionMessage = true;
			}
		}
		if (bCompletionMessage)
		{
			if (isProduction() && getBugOptionBOOL("Civ4lerts__CompleteBegunOn", true))
			{
				wchar_t szTempBuffer[1024];
				swprintf(szTempBuffer, gDLL->getText("TXT_KEY_MISC_WORK_HAS_BEGUN", getProductionNameKey()).GetCString());
				wcscat(szBuffer, szTempBuffer);
			}
			AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, szSound, MESSAGE_TYPE_MINOR_EVENT, szIcon, GC.getCOLOR_WHITE(), getX(), getY(), true, true);
		}
	}

	if ((getTeam() == GC.getGame().getActiveTeam()) || GC.getGame().isDebugMode())
	{
		setInfoDirty(true);
	}
	if (isCitySelected())
	{
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
	}
}


void CvCity::startHeadOrder()
{
	bst::optional<OrderData> order = getHeadOrder();

	if (order && order->eOrderType == ORDER_MAINTAIN)
	{
		processProcess(((ProcessTypes)(order->iData1)), 1);
		AI_setAssignWorkDirty(true);
	}
}


void CvCity::stopHeadOrder()
{
	bst::optional<OrderData> order = getHeadOrder();

	if (order && order->eOrderType == ORDER_MAINTAIN)
	{
		processProcess(((ProcessTypes)(order->iData1)), -1);
	}
}


int CvCity::getOrderQueueLength() const
{
	return m_orderQueue.size();
}

//CLLNode<OrderData>* CvCity::nextOrderQueueNode(CLLNode<OrderData>* pNode) const
//{
//	return m_orderQueue.next(pNode);
//}
//
//
//CLLNode<OrderData>* CvCity::headOrderQueueNode() const
//{
//	return m_orderQueue.head();
//}
//
//int CvCity::getNumOrdersQueued() const
//{
//	return m_orderQueue.getLength();
//}

OrderData CvCity::getOrderData(int iIndex) const
{
	if (m_orderQueue.size() <= static_cast<size_t>(iIndex))
	{
		return OrderData::InvalidOrder;
	}
	return m_orderQueue[iIndex].to_external();
}

void CvCity::setWallOverridePoints(const std::vector< std::pair<float, float> >& kPoints)
{
	m_kWallOverridePoints = kPoints;
	setLayoutDirty(true);
}

const std::vector< std::pair<float, float> >& CvCity::getWallOverridePoints() const
{
	return m_kWallOverridePoints;
}

// Protected Functions...


void CvCity::doCulture()
{
	PROFILE_FUNC();
	changeCultureTimes100(getOwner(), getCommerceRateTimes100(COMMERCE_CULTURE), false, true);
}


void CvCity::doPlotCulture(PlayerTypes ePlayer, int iCultureRate)
{
	PROFILE_FUNC();
	FASSERT_BOUNDS(0, MAX_PLAYERS, ePlayer);

	int iCultureLevel = GC.getCultureLevelInfo(getCultureLevel()).getLevel();

	clearCultureDistanceCache();

	foreach_(CvPlot* plotX, plot()->rect(iCultureLevel, iCultureLevel))
	{
		const int iCultureDistance = cultureDistance(*plotX);

		if (iCultureDistance <= iCultureLevel && plotX->isPotentialCityWorkForArea(area()))
		{
			// changeCulture includes a check to bump culture value upward
			// to ensure plot cannot be lost thru decay even if culture gain is too small
			plotX->changeCulture(
				ePlayer,
				cultureDistanceDropoff(iCultureRate, iCultureLevel, iCultureDistance),
				// Toffer - Only update plot ownership when the culture of non-owners increase.
				plotX->getOwner() != ePlayer
			);
			plotX->setInCultureRangeOfCityByPlayer(ePlayer);
		}
	}
}


/* This function could probably be improved some.
	Currently is linear dropoff or flat, but nonlinearity might be necessary if culture too strong.
	I want to do something like ((range-distance)/range)^x as a modifier on base gain for
	fractional x around 1 where xml controls x, but... obvious issues with floating point. */
int CvCity::cultureDistanceDropoff(int baseCultureGain, int rangeOfSource, int distanceFromSource)
{
	FASSERT_NOT_NEGATIVE(baseCultureGain);
	FAssertMsg(distanceFromSource <= rangeOfSource, "Calculating culture gain for distance greater than max range.");

	if (baseCultureGain < 1) return 1;

	// Some fraction 0-100 should be distance-modified.
	// At default 75, city flipping may begin at max radius overlap if
	// larger city produces 4x culture of lesser city (25% of larger output equal to 100% of lesser).
	const int iDensityFactor = GC.getCITY_CULTURE_DENSITY_FACTOR();

	// 1->0 multiplier on base rate as distance from source goes 0->max
	const int modifiedCultureGain = (
		baseCultureGain * (rangeOfSource - distanceFromSource) * iDensityFactor
		/
		std::max(100, 100*rangeOfSource)
		+ // The rest is flat base culture rate.
		baseCultureGain * (100 - iDensityFactor) / 100
	);
	return std::max(modifiedCultureGain, 1);
}


bool CvCity::doCheckProduction()
{
	PROFILE_EXTRA_FUNC();
	CvPlayerAI& player = GET_PLAYER(getOwner());
	{
		std::vector< std::pair<UnitTypes, int> > decayUnit;
		for (std::vector< std::pair<UnitTypes, int> >::const_iterator it = m_progressOnUnit.begin(); it != m_progressOnUnit.end(); ++it)
		{
			const UnitTypes eUnitX = (*it).first;

			if (player.isProductionMaxedUnit(eUnitX))
			{
				const int iProgress = (*it).second;
				const int iProductionGold = iProgress * GC.getMAXED_UNIT_GOLD_PERCENT() / 100;

				if (iProductionGold > 0)
				{
					player.changeGold(iProductionGold);
					AddDLLMessage(
						getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText(
							"TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED",
							getNameKey(), GC.getUnitInfo(eUnitX).getTextKeyWide(), iProductionGold
						),
						"AS2D_WONDERGOLD", MESSAGE_TYPE_MINOR_EVENT, GC.getCommerceInfo(COMMERCE_GOLD).getButton(),
						GC.getCOLOR_RED(), getX(), getY(), true, true
					);
				}
				decayUnit.push_back(std::make_pair(eUnitX, -iProgress));
			}
		}
		for (std::vector< std::pair<UnitTypes, int> >::iterator it = decayUnit.begin(); it != decayUnit.end(); ++it)
		{
			changeProgressOnUnit((*it).first, (*it).second);
		}
	}
	{
		std::vector< std::pair<BuildingTypes, int> > decayBuilding;
		for (std::vector< std::pair<BuildingTypes, int> >::iterator it = m_progressOnBuilding.begin(); it != m_progressOnBuilding.end(); ++it)
		{
			const BuildingTypes eTypeX = (*it).first;

			if (player.isProductionMaxedBuilding(eTypeX))
			{
				const int iProgress = (*it).second;

				if (GC.getBuildingInfo(eTypeX).getProductionContinueBuilding() != NO_BUILDING && canConstruct(eTypeX))
				{
					m_iLostProduction = iProgress;
				}
				else
				{
					const int iProductionGold = iProgress * GC.getMAXED_BUILDING_GOLD_PERCENT() / 100;

					if (iProductionGold > 0)
					{
						player.changeGold(iProductionGold);
						AddDLLMessage(
							getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
							gDLL->getText(
								"TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED",
								getNameKey(), GC.getBuildingInfo(eTypeX).getTextKeyWide(), iProductionGold
							),
							"AS2D_WONDERGOLD", MESSAGE_TYPE_MINOR_EVENT, GC.getCommerceInfo(COMMERCE_GOLD).getButton(),
							GC.getCOLOR_RED(), getX(), getY(), true, true
						);
					}
				}
				decayBuilding.push_back(std::make_pair(eTypeX, -iProgress));
			}
		}
		for (std::vector< std::pair<BuildingTypes, int> >::iterator it = decayBuilding.begin(); it != decayBuilding.end(); ++it)
		{
			changeProgressOnBuilding((*it).first, (*it).second);
		}
	}

	for (int iI = GC.getNumProjectInfos() - 1; iI > -1; iI--)
	{
		const ProjectTypes eTypeX = static_cast<ProjectTypes>(iI);
		if (getProjectProduction(eTypeX) > 0 && player.isProductionMaxedProject(eTypeX))
		{
			const int iProductionGold = ((getProjectProduction(eTypeX) * GC.getMAXED_PROJECT_GOLD_PERCENT()) / 100);

			if (iProductionGold > 0)
			{
				player.changeGold(iProductionGold);
				AddDLLMessage(
					getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText(
						"TXT_KEY_MISC_LOST_WONDER_PROD_CONVERTED",
						getNameKey(), GC.getProjectInfo(eTypeX).getTextKeyWide(), iProductionGold
					),
					"AS2D_WONDERGOLD", MESSAGE_TYPE_MINOR_EVENT, GC.getCommerceInfo(COMMERCE_GOLD).getButton(),
					GC.getCOLOR_RED(), getX(), getY(), true, true
				);
			}
			setProjectProduction(eTypeX, 0);
		}
	}

	if (!isProduction() && !isDisorder() && isHuman() && !isProductionAutomated())
	{
		return true;
	}

	for (int iI = GC.getNumUnitInfos() - 1; iI > -1; iI--)
	{
		const UnitTypes unitType = static_cast<UnitTypes>(iI);
		if (getFirstUnitOrder(unitType) != -1)
		{
			const UnitTypes eUpgradeUnit = allUpgradesAvailable(unitType);

			if (eUpgradeUnit != NO_UNIT)
			{
				FAssertMsg(eUpgradeUnit != unitType, "eUpgradeUnit is expected to be different from iI");

				// Move already committed production from old unit type to new unit type
				const int iProgress = getProgressOnUnit(unitType);
				changeProgressOnUnit(unitType, -iProgress);
				changeProgressOnUnit(eUpgradeUnit, iProgress);

				// Change the unit types in the queue
				foreach_(OrderData& order, m_orderQueue | filtered(bind(matchUnitOrder, _1, unitType)))
				{
					player.changeUnitMaking(order.getUnitType(), -1);
					order.setUnitType(eUpgradeUnit);
					if (player.AI_unitValue(eUpgradeUnit, order.getUnitAIType(), area()) == 0)
					{
						area()->changeNumTrainAIUnits(getOwner(), order.getUnitAIType(), -1);
						player.AI_changeNumTrainAIUnits(order.getUnitAIType(), -1);
						order.setUnitAIType(GC.getUnitInfo(eUpgradeUnit).getDefaultUnitAIType());
						area()->changeNumTrainAIUnits(getOwner(), order.getUnitAIType(), 1);
						player.AI_changeNumTrainAIUnits(order.getUnitAIType(), 1);
					}
					player.changeUnitMaking(order.getUnitType(), 1);
				}
			}
		}
	}
	bool bOK = true;

	for (int iI = getOrderQueueLength() - 1; iI >= 0; iI--)
	{
		if (!canContinueProduction(getOrderAt(iI)))
		{
			popOrder(iI, false, true);
			bOK = false;
		}
	}
	return bOK;
}


void CvCity::doProduction(bool bAllowNoProduction)
{
	PROFILE_EXTRA_FUNC();
	if (!isHuman() || isProductionAutomated())
	{
		// Koshling - with the unit contracting system we only build units to contractual orders
		//	(apart from a few emergency cases) and we should not change from building them due to new techs, etc.
		if (!isProduction() || isProductionProcess() || AI_isChooseProductionDirty() && !isProductionUnit())
		{
			AI_chooseProduction();
		}
	}

	if (!bAllowNoProduction && !isProduction())
	{
		return;
	}

	if (isProductionProcess())
	{
		if (m_bPopProductionProcess)
		{
			popOrder(0, false, true);
			m_bPopProductionProcess = false;
		}
		return;
	}

	if (isDisorder())
	{
		return;
	}

	if (isProduction())
	{
		changeProduction(getCurrentProductionDifference(ProductionCalc::FoodProduction | ProductionCalc::Overflow));
		setOverflowProduction(0);
		setFeatureProduction(0);

		setBuiltFoodProducedUnit(isFoodProduction());
		clearLostProduction();

		while (productionLeft() <= 0)
		{
			popOrder(0, true, true);

			if (!isProduction())
			{
				if (isHuman())
				{
					break;
				}
				else AI_chooseProduction();

				FAssertMsg(isProduction(), "AI set city to produce nothing at all!")
			}

			/* Toffer - Don't think the wonder limit can be breached here just like that.
			// Prevents breaching the wonder limit.
			// Eliminates pre-build exploits for all Wonders and all Projects
			if (isProductionWonder() || isProductionProject())
			{
				break;
			}
			*/

			// Eliminate pre-build exploits for Settlers and Workers
			if (isFoodProduction() && !isBuiltFoodProducedUnit())
			{
				break;
			}
			changeProduction(getOverflowProduction());
			setOverflowProduction(0);
		}

		if (m_iGoldFromLostProduction > 0)
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_LOST_PROD_CONVERTED", getNameKey(), m_iLostProductionModified, m_iGoldFromLostProduction);
			AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_WONDERGOLD", MESSAGE_TYPE_MINOR_EVENT, GC.getCommerceInfo(COMMERCE_GOLD).getButton(), GC.getCOLOR_RED(), getX(), getY(), true, true);

			GET_PLAYER(getOwner()).changeGold(m_iGoldFromLostProduction);
			clearLostProduction();
		}
	}
	else
	{
		changeOverflowProduction(getCurrentProductionDifference(ProductionCalc::FoodProduction));
	}
}


void CvCity::doDecay()
{
	PROFILE_EXTRA_FUNC();
	if (!isHuman())
	{
		return;
	}
	{
		std::vector< std::pair<BuildingTypes, int> > decayBuilding;
		for (std::vector< std::pair<BuildingTypes, int> >::iterator it = m_progressOnBuilding.begin(); it != m_progressOnBuilding.end(); ++it)
		{
			const BuildingTypes eTypeX = (*it).first;

			if (getProductionBuilding() != eTypeX)
			{
				tickDelayOnBuilding(eTypeX);

				const int iGameSpeedPercent = GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
				if (100 * getDelayOnBuilding(eTypeX) > GC.getBUILDING_PRODUCTION_DECAY_TIME()* iGameSpeedPercent)
				{
					decayBuilding.push_back(
						std::make_pair(
							eTypeX,
							-((*it).second * (100 - GC.getBUILDING_PRODUCTION_DECAY_PERCENT()) + iGameSpeedPercent - 1) / iGameSpeedPercent
						)
					);
				}
			}
			else if (getDelayOnBuilding(eTypeX) > 0)
			{
				tickDelayOnBuilding(eTypeX, false);
			}
		}
		for (std::vector< std::pair<BuildingTypes, int> >::iterator it = decayBuilding.begin(); it != decayBuilding.end(); ++it)
		{
			changeProgressOnBuilding((*it).first, (*it).second);
		}
	}
	{
		std::vector< std::pair<UnitTypes, int> > decayUnit;
		for (std::vector< std::pair<UnitTypes, int> >::const_iterator it = m_progressOnUnit.begin(); it != m_progressOnUnit.end(); ++it)
		{
			const UnitTypes eUnitX = (*it).first;

			if (getProductionUnit() != eUnitX)
			{
				tickDelayOnUnit(eUnitX);

				const int iGameSpeedPercent = GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
				if (100 * getDelayOnUnit(eUnitX) > GC.getUNIT_PRODUCTION_DECAY_TIME() * iGameSpeedPercent)
				{
					decayUnit.push_back(
						std::make_pair(
							eUnitX,
							-((*it).second * (100 - GC.getUNIT_PRODUCTION_DECAY_PERCENT()) + iGameSpeedPercent - 1) / iGameSpeedPercent
						)
					);
				}
			}
			else if (getDelayOnUnit(eUnitX) > 0)
			{
				tickDelayOnUnit(eUnitX, false);
			}
		}
		for (std::vector< std::pair<UnitTypes, int> >::iterator it = decayUnit.begin(); it != decayUnit.end(); ++it)
		{
			changeProgressOnUnit((*it).first, (*it).second);
		}
	}
}


void CvCity::doReligion()
{
	PROFILE_EXTRA_FUNC();
	CvGame& GAME = GC.getGame();
	const bool bReligionDecay = GAME.isModderGameOption(MODDERGAMEOPTION_RELIGION_DECAY);
	const bool bMultRelSpread = GAME.isModderGameOption(MODDERGAMEOPTION_MULTIPLE_RELIGION_SPREAD);

	const ReligionTypes eStateReligion = GET_PLAYER(getOwner()).getStateReligion();
	const int iReligionCount = getReligionCount();

	const int iNumReligions = GC.getNumReligionInfos();
	int iReligionX = GAME.getSorenRandNum(iNumReligions, "Random start index");
	int iCount = 0;
	while (iCount++ < iNumReligions)
	{
		const ReligionTypes eReligionX = static_cast<ReligionTypes>(iReligionX);

		if (GAME.isReligionFounded(eReligionX))
		{
			if (isHasReligion(eReligionX))
			{
				if (bReligionDecay)
				{
					const CvCity* pHolyCity = GAME.getHolyCity(eReligionX);

					if (eReligionX != eStateReligion && pHolyCity != this && iReligionCount > 1)
					{
						const int iExp = iReligionCount - 2;
						int iDecay = GC.getReligionInfo(eReligionX).getSpreadFactor() + iExp * iExp;

						foreach_(const CvCity* cityX, GET_PLAYER(getOwner()).cities())
						{
							if (cityX->isConnectedTo(this) && cityX->isHasReligion(eReligionX))
							{
								iDecay *= 9;
								iDecay /= 10 + cityX->getReligionInfluence(eReligionX);
							}
						}

						iDecay /= 1 + getReligionInfluence(eReligionX);
						if (pHolyCity != NULL)
						{
							if (pHolyCity->getOwner() == getOwner())
							{
								iDecay /= 2;
							}
							else if (GET_TEAM(getTeam()).isAtWar(pHolyCity->getTeam()))
							{
								iDecay *= 4;
								iDecay /= 3;
							}
						}
						if (iDecay > 0)
						{
							const int iSpreadRand =
							(
								GC.getRELIGION_SPREAD_RAND()
								*
								GC.getGameSpeedInfo(GAME.getGameSpeedType()).getSpeedPercent()
								/
								100
							);
							if (GAME.getSorenRandNum(iSpreadRand, "Religion Decay") < iDecay)
							{
								setHasReligion(eReligionX, false, true, false);

								std::vector<BuildingTypes> temp;
								foreach_(const BuildingTypes eTypeX, getHasBuildings())
								{
									if (GC.getBuildingInfo(eTypeX).getPrereqReligion() == iReligionX)
									{
										temp.push_back(eTypeX);
									}
								}
								foreach_(const BuildingTypes eTypeX, temp)
								{
									changeHasBuilding(eTypeX, false);
								}
								break;
							}
						}
					}
				}
			}
			else if (bMultRelSpread || iReligionCount == 0)
			{
				if (eReligionX == eStateReligion || !GET_PLAYER(getOwner()).isNoNonStateReligionSpread())
				{
					const int iSpreadFactor = std::max(1, GC.getReligionInfo(eReligionX).getSpreadFactor());
					int iRandThreshold = 0;

					for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
					{
						if (GET_PLAYER((PlayerTypes)iJ).isAlive())
						{
							foreach_(const CvCity* cityX, GET_PLAYER((PlayerTypes)iJ).cities())
							{
								if (cityX == this || cityX->isConnectedTo(this))
								{
									int iSpread = cityX->getReligionInfluence(eReligionX);
									if (iSpread > 0)
									{
										iSpread *= iSpreadFactor;
										if (cityX == this)
										{
											iSpread = 2 * iSpread / (iReligionCount + 1);
										}
										else
										{
											iSpread /=
											(
												(iReligionCount + 1)
												*
												std::max(
													1,
													GC.getRELIGION_SPREAD_DISTANCE_DIVISOR()
													* plotDistance(getX(), getY(), cityX->getX(), cityX->getY())
													/
													GC.getMap().maxPlotDistance() - 5
												)
											);
										}
										iRandThreshold = std::max(iRandThreshold, iSpread);
									}
								}
							}
						}
					}
					if (iRandThreshold > 0)
					{
						iRandThreshold *= std::max(1, getModifiedIntValue(100, GET_PLAYER(getOwner()).getReligionSpreadRate()));
						iRandThreshold /= 100;

						const int iSpreadRand =
						(
							GC.getRELIGION_SPREAD_RAND()
							*
							GC.getGameSpeedInfo(GAME.getGameSpeedType()).getSpeedPercent()
							/
							100
						);

						if (GAME.getSorenRandNum(iSpreadRand, "Religion Spread") < iRandThreshold)
						{
							setHasReligion((eReligionX), true, true, true);
							break;
						}
					}
				}
			}
		}
		if (++iReligionX == iNumReligions)
		{
			iReligionX = 0;
		}
	}
}


void CvCity::doGreatPeople()
{
	PROFILE_EXTRA_FUNC();
	if (isDisorder())
	{
		return;
	}
	changeGreatPeopleProgress(getGreatPeopleRate());

	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		changeGreatPeopleUnitProgress(((UnitTypes)iI), getGreatPeopleUnitRate((UnitTypes)iI));
	}

	if (getGreatPeopleProgress() >= GET_PLAYER(getOwner()).greatPeopleThresholdNonMilitary())
	{
		int iTotalGreatPeopleUnitProgress = 0;
		for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			iTotalGreatPeopleUnitProgress += getGreatPeopleUnitProgress((UnitTypes)iI);
		}
		int iGreatPeopleUnitRand = GC.getGame().getSorenRandNum(iTotalGreatPeopleUnitProgress, "Great Person");

		UnitTypes eGreatPeopleUnit = NO_UNIT;
		for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			if (iGreatPeopleUnitRand < getGreatPeopleUnitProgress((UnitTypes)iI))
			{
				eGreatPeopleUnit = ((UnitTypes)iI);
				break;
			}
			else
			{
				iGreatPeopleUnitRand -= getGreatPeopleUnitProgress((UnitTypes)iI);
			}
		}

		if (eGreatPeopleUnit != NO_UNIT)
		{
			changeGreatPeopleProgress(-(GET_PLAYER(getOwner()).greatPeopleThresholdNonMilitary()));

			for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
			{
				setGreatPeopleUnitProgress(((UnitTypes)iI), 0);
			}
			createGreatPeople(eGreatPeopleUnit, true, false);
		}
	}
}


void CvCity::doMeltdown()
{
	PROFILE_EXTRA_FUNC();

	std::vector<BuildingTypes> temp;

	foreach_(const BuildingTypes eType, getHasBuildings())
	{
		if (!isDisabledBuilding(eType))
		{
			const int iOdds = GC.getBuildingInfo(eType).getNukeExplosionRand();

			if (iOdds > 0 && GC.getGame().getSorenRandNum(10000, "Meltdown!!!") < iOdds)
			{
				temp.push_back(eType);

				plot()->nukeExplosion(1);
				{
					CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_MELTDOWN_CITY", getNameKey());
					AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_MELTDOWN", MESSAGE_TYPE_MINOR_EVENT,
						ARTFILEMGR.getInterfaceArtInfo("INTERFACE_UNHEALTHY_PERSON")->getPath(), GC.getCOLOR_RED(), getX(), getY(), true, true);
				}
				break;
			}
		}
	}
	foreach_(const BuildingTypes eType, temp)
	{
		changeHasBuilding(eType, false);
	}
}

// Private Functions...

void CvCity::read(FDataStreamBase* pStream)
{

	PROFILE_EXTRA_FUNC();
	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	// Init data before load
	reset();

	WRAPPER_READ(wrapper, "CvCity", &m_iID);
	WRAPPER_READ(wrapper, "CvCity", &m_iX);
	WRAPPER_READ(wrapper, "CvCity", &m_iY);
	WRAPPER_READ(wrapper, "CvCity", &m_iRallyX);
	WRAPPER_READ(wrapper, "CvCity", &m_iRallyY);
	WRAPPER_READ(wrapper, "CvCity", &m_iGameTurnFounded);
	WRAPPER_READ(wrapper, "CvCity", &m_iGameTurnAcquired);
	WRAPPER_READ(wrapper, "CvCity", &m_iPopulation);
	WRAPPER_READ(wrapper, "CvCity", &m_iHighestPopulation);
	WRAPPER_READ(wrapper, "CvCity", &m_iWorkingPopulation);
	WRAPPER_READ(wrapper, "CvCity", &m_iSpecialistPopulation);
	WRAPPER_READ(wrapper, "CvCity", &m_iNumGreatPeople);
	WRAPPER_READ(wrapper, "CvCity", &m_iBaseGreatPeopleRate);
	WRAPPER_READ(wrapper, "CvCity", &m_iGreatPeopleRateModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iGreatPeopleProgress);
	WRAPPER_READ(wrapper, "CvCity", &m_iNumWorldWonders);
	WRAPPER_READ(wrapper, "CvCity", &m_iNumTeamWonders);
	WRAPPER_READ(wrapper, "CvCity", &m_iNumNationalWonders);
	WRAPPER_READ(wrapper, "CvCity", &m_iNumBuildings);
	WRAPPER_READ(wrapper, "CvCity", &m_iGovernmentCenterCount);
	WRAPPER_READ(wrapper, "CvCity", &m_iMaintenance);
	WRAPPER_READ(wrapper, "CvCity", &m_iMaintenanceModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iWarWearinessModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iHurryAngerModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iHealRate);
	WRAPPER_READ(wrapper, "CvCity", &m_iEspionageHealthCounter);
	WRAPPER_READ(wrapper, "CvCity", &m_iEspionageHappinessCounter);
	WRAPPER_READ(wrapper, "CvCity", &m_iFreshWaterGoodHealth);

	WRAPPER_READ(wrapper, "CvCity", &m_iFeatureGoodHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iFeatureBadHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iBuildingGoodHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iBuildingBadHealth);

	WRAPPER_READ(wrapper, "CvCity", &m_iBonusGoodHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iBonusBadHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iHurryAngerTimer);
	WRAPPER_READ(wrapper, "CvCity", &m_iRevRequestAngerTimer);
	WRAPPER_READ(wrapper, "CvCity", &m_iRevSuccessTimer);
	WRAPPER_READ(wrapper, "CvCity", &m_iConscriptAngerTimer);
	WRAPPER_READ(wrapper, "CvCity", &m_iDefyResolutionAngerTimer);
	WRAPPER_READ(wrapper, "CvCity", &m_iHappinessTimer);
	WRAPPER_READ(wrapper, "CvCity", &m_iMilitaryHappinessUnits);
	WRAPPER_READ(wrapper, "CvCity", &m_iBuildingGoodHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iBuildingBadHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraBuildingGoodHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraBuildingBadHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraBuildingGoodHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraBuildingBadHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iFeatureGoodHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iFeatureBadHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iBonusGoodHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iBonusBadHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iReligionGoodHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iReligionBadHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iNoUnhappinessCount);
	WRAPPER_READ(wrapper, "CvCity", &m_iNoUnhealthyPopulationCount);
	WRAPPER_READ(wrapper, "CvCity", &m_iBuildingOnlyHealthyCount);
	WRAPPER_READ(wrapper, "CvCity", &m_iFood);
	WRAPPER_READ(wrapper, "CvCity", &m_iFoodKept);
	WRAPPER_READ(wrapper, "CvCity", &m_iFoodKeptPercent);
	WRAPPER_READ(wrapper, "CvCity", &m_iOverflowProduction);
	WRAPPER_READ(wrapper, "CvCity", &m_iFeatureProduction);
	WRAPPER_READ(wrapper, "CvCity", &m_iMilitaryProductionModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iSpaceProductionModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraTradeRoutes);
	WRAPPER_READ(wrapper, "CvCity", &m_iTradeRouteModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iForeignTradeRouteModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iBuildingDefense);
	WRAPPER_READ(wrapper, "CvCity", &m_iBuildingBombardDefense);
	WRAPPER_READ(wrapper, "CvCity", &m_iFreeExperience);
	WRAPPER_READ(wrapper, "CvCity", &m_iCurrAirlift);
	WRAPPER_READ(wrapper, "CvCity", &m_iMaxAirlift);
	WRAPPER_READ(wrapper, "CvCity", &m_iAirModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iAirUnitCapacity);
	WRAPPER_READ(wrapper, "CvCity", &m_iNukeModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iFreeSpecialist);
	WRAPPER_READ(wrapper, "CvCity", &m_iPowerCount);

	WRAPPER_READ(wrapper, "CvCity", &m_iDefenseDamage);
	WRAPPER_READ(wrapper, "CvCity", &m_iLastDefenseDamage);
	WRAPPER_READ(wrapper, "CvCity", &m_iOccupationTimer);
	WRAPPER_READ(wrapper, "CvCity", &m_iCultureUpdateTimer);
	WRAPPER_READ(wrapper, "CvCity", &m_iCitySizeBoost);
	WRAPPER_READ(wrapper, "CvCity", &m_iSpecialistFreeExperience);
	WRAPPER_READ(wrapper, "CvCity", &m_iEspionageDefenseModifier);

	WRAPPER_READ(wrapper, "CvCity", &m_bNeverLost);
	WRAPPER_READ(wrapper, "CvCity", &m_bBombarded);
	WRAPPER_READ(wrapper, "CvCity", &m_bDrafted);
	WRAPPER_READ(wrapper, "CvCity", &m_bAirliftTargeted);
	WRAPPER_READ(wrapper, "CvCity", &m_bWeLoveTheKingDay);
	WRAPPER_READ(wrapper, "CvCity", &m_bCitizensAutomated);
	WRAPPER_READ(wrapper, "CvCity", &m_bProductionAutomated);
	WRAPPER_READ(wrapper, "CvCity", &m_bWallOverride);
	// m_bInfoDirty not saved...
	// m_bLayoutDirty not saved...
	WRAPPER_READ(wrapper, "CvCity", &m_bPlundered);

	WRAPPER_READ(wrapper, "CvCity", (int*)&m_eOwner);
	WRAPPER_READ(wrapper, "CvCity", (int*)&m_ePreviousOwner);
	WRAPPER_READ(wrapper, "CvCity", (int*)&m_eOriginalOwner);
	WRAPPER_READ(wrapper, "CvCity", (int*)&m_eCultureLevel);
	if (m_eCultureLevel >= GC.getNumCultureLevelInfos())
	{
		m_eCultureLevel = (CultureLevelTypes)(GC.getNumCultureLevelInfos() - 1);
	}

	WRAPPER_READ(wrapper, "CvCity", &m_iRevolutionIndex);
	WRAPPER_READ(wrapper, "CvCity", &m_iLocalRevIndex);
	WRAPPER_READ(wrapper, "CvCity", &m_iRevIndexAverage);
	WRAPPER_READ(wrapper, "CvCity", &m_iRevolutionCounter);
	WRAPPER_READ(wrapper, "CvCity", &m_iReinforcementCounter);

	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiRiverPlotYield);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiBaseYieldRate);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiYieldRateModifier);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiPowerYieldRateModifier);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiBonusYieldRateModifier);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiTradeYield);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiCorporationYield);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiExtraSpecialistYield);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCommerceRate);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiProductionToCommerceModifier);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiBuildingCommerce);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiSpecialistCommerce100);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiReligionCommerce);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCorporationCommerce);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCommerceRateModifier);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCommerceHappinessPer);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_DOMAIN_TYPES, m_aiDomainFreeExperience);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_DOMAIN_TYPES, m_aiDomainProductionModifier);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", MAX_PLAYERS, m_aiCulture);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", MAX_PLAYERS, m_aiNumRevolts);

	WRAPPER_READ_ARRAY(wrapper, "CvCity", MAX_PLAYERS, m_abEverOwned);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", MAX_PLAYERS, m_abTradeRoute);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", MAX_TEAMS, m_abRevealed);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", MAX_TEAMS, m_abEspionageVisibility);

	WRAPPER_READ_STRING(wrapper, "CvCity", m_szName);
	WRAPPER_READ_STRING(wrapper, "CvCity", m_szScriptData);

	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiFreeBonus);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNumBonuses);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROJECTS, GC.getNumProjectInfos(), m_paiProjectProduction);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_paiUnitProduction);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_paiGreatPeopleUnitRate);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_paiGreatPeopleUnitProgress);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiSpecialistCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiMaxSpecialistCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiForceSpecialistCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiFreeSpecialistCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiFreeSpecialistCountUnattributed);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_IMPROVEMENTS, GC.getNumImprovementInfos(), m_paiImprovementFreeSpecialists);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_RELIGIONS, GC.getNumReligionInfos(), m_paiReligionInfluence);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_RELIGIONS, GC.getNumReligionInfos(), m_paiStateReligionHappiness);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatFreeExperience);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_CITY_PLOTS, m_pabWorkingPlot);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_RELIGIONS, GC.getNumReligionInfos(), m_pabHasReligion);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_CORPORATIONS, GC.getNumCorporationInfos(), m_pabHasCorporation);

	WRAPPER_READ(wrapper, "CvCity", &m_iImprovementGoodHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iImprovementBadHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iSpecialistGoodHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iSpecialistBadHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iSpecialistHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iSpecialistUnhappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iEventAnger);

	WRAPPER_READ(wrapper, "CvCity", &m_fPopulationgrowthratepercentageLog);

	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiBonusCommerceRateModifier);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiBonusCommercePercentChanges);

	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_pabHadVicinityBonus);
	WRAPPER_READ_CLASS_ENUM(wrapper, "CvCity", REMAPPED_CLASS_TYPE_CIVILIZATIONS, &m_iCiv);

	WRAPPER_READ(wrapper, "CvCity", &m_iLineOfSight);
	WRAPPER_READ(wrapper, "CvCity", &m_iLandmarkAngerTimer);

	WRAPPER_READ(wrapper, "CvCity", &m_iFreshWater);
	WRAPPER_READ(wrapper, "CvCity", &m_iAdjacentDamagePercent);
	WRAPPER_READ(wrapper, "CvCity", &m_iWorkableRadiusOverride);
	WRAPPER_READ(wrapper, "CvCity", &m_iProtectedCultureCount);
	WRAPPER_READ(wrapper, "CvCity", &m_iNumUnitFullHeal);
	WRAPPER_READ(wrapper, "CvCity", &m_iDisabledPowerTimer);
	WRAPPER_READ(wrapper, "CvCity", &m_iWarWearinessTimer);

	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatExtraStrength);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDS, GC.getNumBuildInfos(), m_pabAutomatedCanBuild);

	WRAPPER_READ(wrapper, "CvCity", &m_iMinimumDefenseLevel);
	WRAPPER_READ(wrapper, "CvCity", &m_iNumPopulationEmployed);
	WRAPPER_READ(wrapper, "CvCity", &m_iHappinessPercentPerPopulation);
	WRAPPER_READ(wrapper, "CvCity", &m_iHealthPercentPerPopulation);

	// Read all saved trade routes
	int iNumTradeRoutes = 0;
	WRAPPER_READ(wrapper, "CvCity", &iNumTradeRoutes);
	m_paTradeCities = std::vector<IDInfo>(iNumTradeRoutes);
	for (int iI = 0; iI < iNumTradeRoutes; ++iI)
	{
		WRAPPER_READ(wrapper, "CvCity", (int*)&m_paTradeCities[iI].eOwner);
		WRAPPER_READ(wrapper, "CvCity", &m_paTradeCities[iI].iID);
	}
	// Discard saved trade routes above the max count we allow
	m_paTradeCities.resize(getMaxTradeRoutes());

	int orderQueueSize = 0;
	WRAPPER_READ(wrapper, "CvCity", &orderQueueSize);
	m_orderQueue.clear();
	for (int orderQueueIndex = 0; orderQueueIndex < orderQueueSize; ++orderQueueIndex)
	{
		OrderData orderData;
		WRAPPER_READ_ARRAY_DECORATED(wrapper, "CvCity", sizeof(OrderData), (uint8_t*)&orderData, "OrderData");
		bool bDeleteNode = false;
		switch (orderData.eOrderType)
		{
		case ORDER_TRAIN: {
			orderData.setUnitType(static_cast<UnitTypes>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_UNITS, orderData.getUnitType(), true)));
			bDeleteNode = (orderData.getUnitType() == NO_UNIT);
			if (!bDeleteNode && orderData.getUnitAIType() == static_cast<UnitAITypes>(0xFF))
			{
				orderData.unit.AIType = GC.getUnitInfo(orderData.getUnitType()).getDefaultUnitAIType();
			}
			break;
		};
		case ORDER_CONSTRUCT: {
			orderData.setBuildingType(static_cast<BuildingTypes>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDINGS, orderData.getBuildingType(), true)));
			bDeleteNode = (orderData.getBuildingType() == NO_BUILDING);
			break;
		};
		case ORDER_CREATE: {
			orderData.setProjectType(static_cast<ProjectTypes>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_PROJECTS, orderData.getProjectType(), true)));
			bDeleteNode = (orderData.getProjectType() == NO_PROJECT);
			break;
		};
		case ORDER_MAINTAIN:
		case ORDER_LIST:
		default:
			break;
		};
		if (!bDeleteNode)
		{
			m_orderQueue.push_back(orderData);
		}
	}

	WRAPPER_READ(wrapper, "CvCity", &m_iPopulationRank);
	WRAPPER_READ(wrapper, "CvCity", &m_bPopulationRankValid);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiBaseYieldRank);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_abBaseYieldRankValid);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiYieldRank);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_abYieldRankValid);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCommerceRank);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_abCommerceRankValid);

	m_Properties.readWrapper(pStream);

	unsigned int iNumElts = 0;
	WRAPPER_READ(wrapper, "CvCity", &iNumElts);
	m_aEventsOccured.clear();
	for (unsigned int i = 0; i < iNumElts; ++i)
	{
		EventTypes eEvent = NO_EVENT;
		WRAPPER_READ_CLASS_ENUM_ALLOW_MISSING(wrapper, "CvCity", REMAPPED_CLASS_TYPE_EVENTS, (int*)&eEvent);

		if (eEvent != NO_EVENT)
		{
			m_aEventsOccured.push_back(eEvent);
		}
	}

	WRAPPER_READ(wrapper, "CvCity", &iNumElts);
	m_aBuildingYieldChange.clear();
	for (unsigned int i = 0; i < iNumElts; ++i)
	{
		BuildingYieldChange kChange;
		kChange.read(pStream);
		m_aBuildingYieldChange.push_back(kChange);
	}

	WRAPPER_READ(wrapper, "CvCity", &iNumElts);
	m_aBuildingCommerceChange.clear();
	for (unsigned int i = 0; i < iNumElts; ++i)
	{
		BuildingCommerceChange kChange;
		kChange.read(pStream);
		m_aBuildingCommerceChange.push_back(kChange);
	}

	WRAPPER_READ(wrapper, "CvCity", &iNumElts);
	m_aBuildingHappyChange.clear();
	for (unsigned int i = 0; i < iNumElts; ++i)
	{
		int iBuilding;
		WRAPPER_READ_CLASS_ENUM(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, &iBuilding);
		int iChange;
		WRAPPER_READ(wrapper, "CvCity", &iChange);
		m_aBuildingHappyChange.push_back(std::make_pair((BuildingTypes)iBuilding, iChange));
	}

	WRAPPER_READ(wrapper, "CvCity", &iNumElts);
	m_aBuildingHealthChange.clear();
	for (unsigned int i = 0; i < iNumElts; ++i)
	{
		int iBuilding;
		WRAPPER_READ_CLASS_ENUM(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, &iBuilding);
		int iChange;
		WRAPPER_READ(wrapper, "CvCity", &iChange);
		m_aBuildingHealthChange.push_back(std::make_pair((BuildingTypes)iBuilding, iChange));
	}

	//	Now the owner has been restored from the save set the info on the building list
	m_BuildingList.setPlayerToOwner();
	m_UnitList.setPlayerToOwner();

	// phunny_pharmer:
	// clear the culture distance cache (note that it is not saved in the .sav file)
	clearCultureDistanceCache();

	//TB Combat Mod (Buildings) begin
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatProductionModifier);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatRepelModifier);
#ifdef STRENGTH_IN_NUMBERS
	WRAPPER_READ(wrapper, "CvCity", &m_iTotalFrontSupportPercentModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iTotalShortRangeSupportPercentModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iTotalMediumRangeSupportPercentModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iTotalLongRangeSupportPercentModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iTotalFlankSupportPercentModifier);
#else
	WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", &m_iTotalFrontSupportPercentModifier, SAVE_VALUE_TYPE_INT);
	WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", &m_iTotalShortRangeSupportPercentModifier, SAVE_VALUE_TYPE_INT);
	WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", &m_iTotalMediumRangeSupportPercentModifier, SAVE_VALUE_TYPE_INT);
	WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", &m_iTotalLongRangeSupportPercentModifier, SAVE_VALUE_TYPE_INT);
	WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", &m_iTotalFlankSupportPercentModifier, SAVE_VALUE_TYPE_INT);
#endif

	WRAPPER_READ(wrapper, "CvCity", &m_iExtraTechSpecialistHappiness);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraBuildingHappinessFromTech);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraTechSpecialistHealth);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraBuildingHealthFromTech);
	for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_SPECIALISTS); ++i)
	{
		int	iI = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_SPECIALISTS, i, true);

		if (iI != -1)
		{
			WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_ppaaiLocalSpecialistExtraYield[iI]);
		}
		else
		{
			//	Consume the values
			WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", m_ppaaiLocalSpecialistExtraYield[iI], SAVE_VALUE_TYPE_INT_ARRAY);
		}
	}

	WRAPPER_READ(wrapper, "CvCity", &m_iExtraLocalCaptureProbabilityModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraLocalCaptureResistanceModifier);

	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_pabReligiouslyDisabledBuilding);
	WRAPPER_READ(wrapper, "CvCity", &m_iPrioritySpecialist);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiSpecialistBannedCount);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraLocalDynamicDefense);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraRiverDefensePenalty);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraLocalRepel);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraMinDefense);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraBuildingDefenseRecoverySpeedModifier);
	WRAPPER_READ(wrapper, "CvCity", &m_iModifiedBuildingDefenseRecoverySpeedCap);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraCityDefenseRecoverySpeedModifier);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatRepelAgainstModifier);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatDefenseAgainstModifier);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiDamageAttackingUnitCombatCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiHealUnitCombatTypeVolume);
	//TB Combat Mod (Buildings) end

	WRAPPER_READ(wrapper, "CvCity", &m_iZoCCount);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraInsidiousness);
	WRAPPER_READ(wrapper, "CvCity", &m_iExtraInvestigation);
	WRAPPER_READ(wrapper, "CvCity", &m_iSpecialistInsidiousness);
	WRAPPER_READ(wrapper, "CvCity", &m_iSpecialistInvestigation);
	WRAPPER_READ(wrapper, "CvCity", &iNumElts);
	m_aPropertySpawns.clear();
	for (unsigned int i = 0; i < iNumElts; ++i)
	{
		PropertySpawns kChange;
		kChange.read(pStream);
		m_aPropertySpawns.push_back(kChange);
	}
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiExtraSpecialistCommerce);
	for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_SPECIALISTS); ++i)
	{
		int	iI = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_SPECIALISTS, i, true);

		if (iI != -1)
		{
			WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_ppaaiLocalSpecialistExtraCommerce[iI]);
		}
		else
		{
			//	Consume the values
			WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", m_ppaaiLocalSpecialistExtraCommerce[iI], SAVE_VALUE_TYPE_INT_ARRAY);
		}
	}
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiBaseYieldPerPopRate);
	WRAPPER_READ(wrapper, "CvCity", &m_bVisibilitySetup);
	m_bVisibilitySetup = false;

	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiNewAfflictionTypeCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiNewExtraAfflictionOutbreakLevelChange);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiNewAfflictionToleranceChange);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiNewCurrentOvercomeChange);

	for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_BONUSES); ++i)
	{
		int	iI = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BONUSES, i, true);

		if (iI != -1)
		{
			WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROPERTIES, GC.getNumPropertyInfos(), m_ppaaiExtraBonusAidModifier[iI]);
		}
		else
		{
			WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", m_ppaaiExtraBonusAidModifier[iI], SAVE_VALUE_TYPE_INT_ARRAY);
		}
	}
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROPERTIES, GC.getNumPropertyInfos(), m_paiAidRate);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiPromotionLineAfflictionAttackCommunicability);
	WRAPPER_READ(wrapper, "CvCity", &m_iQuarantinedCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_pabHadRawVicinityBonus);
	WRAPPER_READ(wrapper, "CvCity", &m_bPropertyControlBuildingQueued);

	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiExtraYield);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiBuildingCommerceTechChange);

	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_commercePerPopFromBuildings);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_buildingExtraYield100);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_buildingYieldMod);
	WRAPPER_READ_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_buildingCommerceMod);
	{
		short iSize = 0;
		short iType;
		YieldArray yields;
		WRAPPER_READ_DECORATED(wrapper, "CvCity", &iSize, "PlotYieldChangesSize");
		while (iSize-- > 0)
		{
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iType, "PlotYieldChangesType");
			WRAPPER_READ_ARRAY_DECORATED(wrapper, "CvCity", NUM_YIELD_TYPES, yields.elems, "PlotYieldChanges");

			if (iType > -1)
			{
				m_plotYieldChanges.insert(std::make_pair(iType, yields));
			}
		}
	}
	WRAPPER_READ(wrapper, "CvCity", &m_iRevIndexDistanceMod);

	// Toffer - Read vectors
	{
		short iType;
		{
			uint16_t iCityOutputHistorySize;
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iCityOutputHistorySize, "CityOutputHistorySize");

			for (uint16_t iI = 0; iI < iCityOutputHistorySize; iI++)
			{
				uint32_t iTurn;
				WRAPPER_READ_DECORATED(wrapper, "CvCity", &iTurn, "RecentOutputTurn");
				m_outputHistory.setRecentOutputTurn(iI, iTurn);

				uint16_t iCityOutputHistoryNumEntries;
				WRAPPER_READ_DECORATED(wrapper, "CvCity", &iCityOutputHistoryNumEntries, "CityOutputHistoryNumEntries");

				for (uint16_t iJ = 0; iJ < iCityOutputHistoryNumEntries; iJ++)
				{
					uint16_t iOrderType;
					uint16_t iType;
					WRAPPER_READ_DECORATED(wrapper, "CvCity", &iOrderType, "OrderType");
					WRAPPER_READ_DECORATED(wrapper, "CvCity", &iType, "Type");

					m_outputHistory.addToHistory(static_cast<OrderTypes>(iOrderType), iType, static_cast<short>(iI));
				}
			}
		}
		short iSize;

		// Techs
		WRAPPER_READ_DECORATED(wrapper, "CvCity", &iSize, "BuildingHappinessFromTechSize");
		for (short i = 0; i < iSize; ++i)
		{
			int iValue = 0;
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iType, "BuildingHappinessFromTechType");
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iValue, "BuildingHappinessFromTechValue");
			const TechTypes eTech = static_cast<TechTypes>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_TECHS, iType, true));

			if (eTech != NO_TECH)
			{
				m_buildingHappinessFromTech.push_back(std::make_pair(eTech, iValue));
			}
		}

		WRAPPER_READ_DECORATED(wrapper, "CvCity", &iSize, "BuildingHealthFromTechSize");
		for (short i = 0; i < iSize; ++i)
		{
			int iValue = 0;
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iType, "BuildingHealthFromTechType");
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iValue, "BuildingHealthFromTechValue");
			const TechTypes eTech = static_cast<TechTypes>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_TECHS, iType, true));

			if (eTech != NO_TECH)
			{
				m_buildingHealthFromTech.push_back(std::make_pair(eTech, iValue));
			}
		}

		// Buildings
		WRAPPER_READ_DECORATED(wrapper, "CvCity", &iSize, "FreeBuildingsSize");
		for (short i = 0; i < iSize; ++i)
		{
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iType, "FreeBuildingsIndex");
			iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDINGS, iType, true));

			if (iType > -1)
			{
				m_vFreeBuildings.push_back(iType);
			}
		}

		WRAPPER_READ_DECORATED(wrapper, "CvCity", &iSize, "DisabledBuildingsSize");
		for (short i = 0; i < iSize; ++i)
		{
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iType, "DisabledBuildingsIndex");
			iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDINGS, iType, true));

			if (iType > -1)
			{
				m_vDisabledBuildings.push_back(iType);
			}
		}

		WRAPPER_READ_DECORATED(wrapper, "CvCity", &iSize, "BuildingProgressSize");
		for (short i = 0; i < iSize; ++i)
		{
			int iValue = 0;
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iType, "BuildingProgressType");
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iValue, "BuildingProgressValue");
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDINGS, iType, true));

			if (eBuilding != NO_BUILDING)
			{
				m_progressOnBuilding.push_back(std::make_pair(eBuilding, iValue));
			}
		}

		WRAPPER_READ_DECORATED(wrapper, "CvCity", &iSize, "DelayOnBuildingSize");
		for (short i = 0; i < iSize; ++i)
		{
			int iValue = 0;
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iType, "DelayOnBuildingType");
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iValue, "DelayOnBuildingValue");
			const BuildingTypes eBuilding = static_cast<BuildingTypes>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDINGS, iType, true));

			if (eBuilding != NO_BUILDING)
			{
				m_delayOnBuilding.push_back(std::make_pair(eBuilding, iValue));
			}
		}

		// Units
		WRAPPER_READ_DECORATED(wrapper, "CvCity", &iSize, "UnitProgressSize");
		for (short i = 0; i < iSize; ++i)
		{
			int iValue = 0;
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iType, "UnitProgressType");
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iValue, "UnitProgressValue");
			const UnitTypes eUnit = static_cast<UnitTypes>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_UNITS, iType, true));

			if (eUnit != NO_UNIT)
			{
				m_progressOnUnit.push_back(std::make_pair(eUnit, iValue));
			}
		}

		WRAPPER_READ_DECORATED(wrapper, "CvCity", &iSize, "WorkersSize");
		for (short i = 0; i < iSize; ++i)
		{
			int iUnitID;
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iUnitID, "WorkerUnitID");
			m_workers.push_back(iUnitID);
		}

		WRAPPER_READ_DECORATED(wrapper, "CvCity", &iSize, "DelayOnUnitSize");
		for (short i = 0; i < iSize; ++i)
		{
			int iValue = 0;
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iType, "DelayOnUnitType");
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iValue, "DelayOnUnitValue");
			const UnitTypes eUnit = static_cast<UnitTypes>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_UNITS, iType, true));

			if (eUnit != NO_UNIT)
			{
				m_delayOnUnit.push_back(std::make_pair(eUnit, iValue));
			}
		}

		// Bonuses
		WRAPPER_READ_DECORATED(wrapper, "CvCity", &iSize, "CorpBonusProductionSize");
		for (short i = 0; i < iSize; ++i)
		{
			int iValue = 0;
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iType, "CorpBonusProductionType");
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iValue, "CorpBonusProductionValue");
			const BonusTypes eBonus = static_cast<BonusTypes>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BONUSES, iType, true));

			if (eBonus != NO_BONUS)
			{
				m_corpBonusProduction.push_back(std::make_pair(eBonus, iValue));
			}
		}
	}
	// Toffer - Read maps
	{
		short iSize;
		short iType;
		int iCount;
		uint16_t sCountU;

		// Bonus
		WRAPPER_READ_DECORATED(wrapper, "CvCity", &iSize, "BonusDefenseChangesSize");
		while (iSize-- > 0)
		{
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iType, "BonusDefenseChangesType");
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iCount, "BonusDefenseChanges");
			iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BONUSES, iType, true));

			if (iType > -1)
			{
				m_bonusDefenseChanges.insert(std::make_pair(iType, iCount));
			}
		}

		// Building
		WRAPPER_READ_DECORATED(wrapper, "CvCity", &iSize, "BuildingProductionModSize");
		while (iSize-- > 0)
		{
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iType, "BuildingProductionModType");
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iCount, "BuildingProductionMod");
			iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDINGS, iType, true));

			if (iType > -1)
			{
				m_buildingProductionMod.insert(std::make_pair(iType, iCount));
			}
		}

		WRAPPER_READ_DECORATED(wrapper, "CvCity", &iSize, "FreeAreaBuildingCountSize");
		while (iSize-- > 0)
		{
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iType, "FreeAreaBuildingCountType");
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &sCountU, "FreeAreaBuildingCount");
			iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDINGS, iType, true));

			if (iType > -1)
			{
				m_freeAreaBuildingCount.insert(std::make_pair(iType, sCountU));
			}
		}

		WRAPPER_READ_DECORATED(wrapper, "CvCity", &iSize, "BuildingLedgerSize");
		while (iSize-- > 0)
		{
			int iPlayer;
			int iTime;
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iType, "BuildingLedgerType");
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iPlayer, "BuildingLedgerBuiltBy");
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iTime, "BuildingLedgerTimeBuilt");
			const BuildingTypes eType = static_cast<BuildingTypes>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDINGS, iType, true));

			if (eType != NO_BUILDING)
			{
				m_bHasBuildings[eType] = true; // quick lookup
				m_hasBuildings.push_back(eType); // quick iteration
				// compressed data all buildings have
				BuiltBuildingData data;
				data.eBuiltBy = (PlayerTypes)iPlayer;
				data.iTimeBuilt = iTime;
				m_buildingLedger.insert(std::make_pair(eType, data));
			}
		}

		// Unit
		WRAPPER_READ_DECORATED(wrapper, "CvCity", &iSize, "UnitProductionModSize");
		while (iSize-- > 0)
		{
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iType, "UnitProductionModType");
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iCount, "UnitProductionMod");
			iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_UNITS, iType, true));

			if (iType > -1)
			{
				m_unitProductionMod.insert(std::make_pair(iType, iCount));
			}
		}
		YieldArray yields;

		WRAPPER_READ_DECORATED(wrapper, "CvCity", &iSize, "TerrainYieldChangesSize");
		while (iSize-- > 0)
		{
			WRAPPER_READ_DECORATED(wrapper, "CvCity", &iType, "TerrainYieldChangesType");
			WRAPPER_READ_ARRAY_DECORATED(wrapper, "CvCity", NUM_YIELD_TYPES, yields.elems, "TerrainYieldChanges");
			iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_TERRAINS, iType, true));

			if (iType > -1)
			{
				m_terrainYieldChanges.insert(std::make_pair(iType, yields));
			}
		}
	}
	WRAPPER_READ_OBJECT_END(wrapper);
	//Example of how to skip an unneeded element
	//WRAPPER_SKIP_ELEMENT(wrapper, "CvCity", m_iMaxFoodKeptPercent, SAVE_VALUE_ANY);	// was present in old formats
}

void CvCity::write(FDataStreamBase* pStream)
{
	PROFILE_EXTRA_FUNC();
	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE(wrapper, "CvCity", m_iID);
	WRAPPER_WRITE(wrapper, "CvCity", m_iX);
	WRAPPER_WRITE(wrapper, "CvCity", m_iY);
	WRAPPER_WRITE(wrapper, "CvCity", m_iRallyX);
	WRAPPER_WRITE(wrapper, "CvCity", m_iRallyY);
	WRAPPER_WRITE(wrapper, "CvCity", m_iGameTurnFounded);
	WRAPPER_WRITE(wrapper, "CvCity", m_iGameTurnAcquired);
	WRAPPER_WRITE(wrapper, "CvCity", m_iPopulation);
	WRAPPER_WRITE(wrapper, "CvCity", m_iHighestPopulation);
	WRAPPER_WRITE(wrapper, "CvCity", m_iWorkingPopulation);
	WRAPPER_WRITE(wrapper, "CvCity", m_iSpecialistPopulation);
	WRAPPER_WRITE(wrapper, "CvCity", m_iNumGreatPeople);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBaseGreatPeopleRate);
	WRAPPER_WRITE(wrapper, "CvCity", m_iGreatPeopleRateModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iGreatPeopleProgress);
	WRAPPER_WRITE(wrapper, "CvCity", m_iNumWorldWonders);
	WRAPPER_WRITE(wrapper, "CvCity", m_iNumTeamWonders);
	WRAPPER_WRITE(wrapper, "CvCity", m_iNumNationalWonders);
	WRAPPER_WRITE(wrapper, "CvCity", m_iNumBuildings);
	WRAPPER_WRITE(wrapper, "CvCity", m_iGovernmentCenterCount);
	WRAPPER_WRITE(wrapper, "CvCity", m_iMaintenance);
	WRAPPER_WRITE(wrapper, "CvCity", m_iMaintenanceModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iWarWearinessModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iHurryAngerModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iHealRate);
	WRAPPER_WRITE(wrapper, "CvCity", m_iEspionageHealthCounter);
	WRAPPER_WRITE(wrapper, "CvCity", m_iEspionageHappinessCounter);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFreshWaterGoodHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFeatureGoodHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFeatureBadHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBuildingGoodHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBuildingBadHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBonusGoodHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBonusBadHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iHurryAngerTimer);
	WRAPPER_WRITE(wrapper, "CvCity", m_iRevRequestAngerTimer);
	WRAPPER_WRITE(wrapper, "CvCity", m_iRevSuccessTimer);
	WRAPPER_WRITE(wrapper, "CvCity", m_iConscriptAngerTimer);
	WRAPPER_WRITE(wrapper, "CvCity", m_iDefyResolutionAngerTimer);
	WRAPPER_WRITE(wrapper, "CvCity", m_iHappinessTimer);
	WRAPPER_WRITE(wrapper, "CvCity", m_iMilitaryHappinessUnits);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBuildingGoodHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBuildingBadHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraBuildingGoodHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraBuildingBadHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraBuildingGoodHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraBuildingBadHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFeatureGoodHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFeatureBadHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBonusGoodHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBonusBadHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iReligionGoodHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iReligionBadHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iNoUnhappinessCount);
	WRAPPER_WRITE(wrapper, "CvCity", m_iNoUnhealthyPopulationCount);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBuildingOnlyHealthyCount);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFood);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFoodKept);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFoodKeptPercent);
	WRAPPER_WRITE(wrapper, "CvCity", m_iOverflowProduction);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFeatureProduction);
	WRAPPER_WRITE(wrapper, "CvCity", m_iMilitaryProductionModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iSpaceProductionModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraTradeRoutes);
	WRAPPER_WRITE(wrapper, "CvCity", m_iTradeRouteModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iForeignTradeRouteModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBuildingDefense);
	WRAPPER_WRITE(wrapper, "CvCity", m_iBuildingBombardDefense);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFreeExperience);
	WRAPPER_WRITE(wrapper, "CvCity", m_iCurrAirlift);
	WRAPPER_WRITE(wrapper, "CvCity", m_iMaxAirlift);
	WRAPPER_WRITE(wrapper, "CvCity", m_iAirModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iAirUnitCapacity);
	WRAPPER_WRITE(wrapper, "CvCity", m_iNukeModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iFreeSpecialist);
	WRAPPER_WRITE(wrapper, "CvCity", m_iPowerCount);
	WRAPPER_WRITE(wrapper, "CvCity", m_iDefenseDamage);
	WRAPPER_WRITE(wrapper, "CvCity", m_iLastDefenseDamage);
	WRAPPER_WRITE(wrapper, "CvCity", m_iOccupationTimer);
	WRAPPER_WRITE(wrapper, "CvCity", m_iCultureUpdateTimer);
	WRAPPER_WRITE(wrapper, "CvCity", m_iCitySizeBoost);
	WRAPPER_WRITE(wrapper, "CvCity", m_iSpecialistFreeExperience);
	WRAPPER_WRITE(wrapper, "CvCity", m_iEspionageDefenseModifier);

	WRAPPER_WRITE(wrapper, "CvCity", m_bNeverLost);
	WRAPPER_WRITE(wrapper, "CvCity", m_bBombarded);
	WRAPPER_WRITE(wrapper, "CvCity", m_bDrafted);
	WRAPPER_WRITE(wrapper, "CvCity", m_bAirliftTargeted);
	WRAPPER_WRITE(wrapper, "CvCity", m_bWeLoveTheKingDay);
	WRAPPER_WRITE(wrapper, "CvCity", m_bCitizensAutomated);
	WRAPPER_WRITE(wrapper, "CvCity", m_bProductionAutomated);
	WRAPPER_WRITE(wrapper, "CvCity", m_bWallOverride);
	// m_bInfoDirty not saved...
	// m_bLayoutDirty not saved...
	WRAPPER_WRITE(wrapper, "CvCity", m_bPlundered);
	WRAPPER_WRITE(wrapper, "CvCity", m_eOwner);
	WRAPPER_WRITE(wrapper, "CvCity", m_ePreviousOwner);
	WRAPPER_WRITE(wrapper, "CvCity", m_eOriginalOwner);
	WRAPPER_WRITE(wrapper, "CvCity", m_eCultureLevel);
	WRAPPER_WRITE(wrapper, "CvCity", m_iRevolutionIndex);
	WRAPPER_WRITE(wrapper, "CvCity", m_iLocalRevIndex);
	WRAPPER_WRITE(wrapper, "CvCity", m_iRevIndexAverage);
	WRAPPER_WRITE(wrapper, "CvCity", m_iRevolutionCounter);
	WRAPPER_WRITE(wrapper, "CvCity", m_iReinforcementCounter);

	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiRiverPlotYield);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiBaseYieldRate);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiYieldRateModifier);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiPowerYieldRateModifier);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiBonusYieldRateModifier);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiTradeYield);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiCorporationYield);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiExtraSpecialistYield);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCommerceRate);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiProductionToCommerceModifier);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiBuildingCommerce);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiSpecialistCommerce100);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiReligionCommerce);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCorporationCommerce);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCommerceRateModifier);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCommerceHappinessPer);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_DOMAIN_TYPES, m_aiDomainFreeExperience);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_DOMAIN_TYPES, m_aiDomainProductionModifier);

	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", MAX_PLAYERS, m_aiCulture);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", MAX_PLAYERS, m_aiNumRevolts);

	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", MAX_PLAYERS, m_abEverOwned);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", MAX_PLAYERS, m_abTradeRoute);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", MAX_TEAMS, m_abRevealed);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", MAX_TEAMS, m_abEspionageVisibility);

	WRAPPER_WRITE_STRING(wrapper, "CvCity", m_szName);
	WRAPPER_WRITE_STRING(wrapper, "CvCity", m_szScriptData);

	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiFreeBonus);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNumBonuses);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROJECTS, GC.getNumProjectInfos(), m_paiProjectProduction);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_paiUnitProduction);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_paiGreatPeopleUnitRate);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_paiGreatPeopleUnitProgress);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiSpecialistCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiMaxSpecialistCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiForceSpecialistCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiFreeSpecialistCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiFreeSpecialistCountUnattributed);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_IMPROVEMENTS, GC.getNumImprovementInfos(), m_paiImprovementFreeSpecialists);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_RELIGIONS, GC.getNumReligionInfos(), m_paiReligionInfluence);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_RELIGIONS, GC.getNumReligionInfos(), m_paiStateReligionHappiness);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatFreeExperience);

	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_CITY_PLOTS, m_pabWorkingPlot);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_RELIGIONS, GC.getNumReligionInfos(), m_pabHasReligion);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_CORPORATIONS, GC.getNumCorporationInfos(), m_pabHasCorporation);

	WRAPPER_WRITE(wrapper, "CvCity", m_iImprovementGoodHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iImprovementBadHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iSpecialistGoodHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iSpecialistBadHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iSpecialistHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iSpecialistUnhappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iEventAnger);
	WRAPPER_WRITE(wrapper, "CvCity", m_fPopulationgrowthratepercentageLog);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiBonusCommerceRateModifier);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiBonusCommercePercentChanges);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_pabHadVicinityBonus);
	WRAPPER_WRITE_CLASS_ENUM(wrapper, "CvCity", REMAPPED_CLASS_TYPE_CIVILIZATIONS, m_iCiv);

	WRAPPER_WRITE(wrapper, "CvCity", m_iLineOfSight);
	WRAPPER_WRITE(wrapper, "CvCity", m_iLandmarkAngerTimer);

	WRAPPER_WRITE(wrapper, "CvCity", m_iFreshWater);
	WRAPPER_WRITE(wrapper, "CvCity", m_iAdjacentDamagePercent);
	WRAPPER_WRITE(wrapper, "CvCity", m_iWorkableRadiusOverride);
	WRAPPER_WRITE(wrapper, "CvCity", m_iProtectedCultureCount);
	WRAPPER_WRITE(wrapper, "CvCity", m_iNumUnitFullHeal);
	WRAPPER_WRITE(wrapper, "CvCity", m_iDisabledPowerTimer);
	WRAPPER_WRITE(wrapper, "CvCity", m_iWarWearinessTimer);

	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatExtraStrength);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDS, GC.getNumBuildInfos(), m_pabAutomatedCanBuild);

	WRAPPER_WRITE(wrapper, "CvCity", m_iMinimumDefenseLevel);
	WRAPPER_WRITE(wrapper, "CvCity", m_iNumPopulationEmployed);
	WRAPPER_WRITE(wrapper, "CvCity", m_iHappinessPercentPerPopulation);
	WRAPPER_WRITE(wrapper, "CvCity", m_iHealthPercentPerPopulation);

	const int iNumTradeRoutes = m_paTradeCities.size();
	WRAPPER_WRITE(wrapper, "CvCity", iNumTradeRoutes);
	for (int iI = 0; iI < iNumTradeRoutes; iI++)
	{
		WRAPPER_WRITE(wrapper, "CvCity", m_paTradeCities[iI].eOwner);
		WRAPPER_WRITE(wrapper, "CvCity", m_paTradeCities[iI].iID);
	}

	const int orderQueueSize = m_orderQueue.size();
	WRAPPER_WRITE(wrapper, "CvCity", orderQueueSize);
	foreach_(const OrderData& orderData, m_orderQueue)
	{
		WRAPPER_WRITE_ARRAY_DECORATED(wrapper, "CvCity", sizeof(OrderData), (const uint8_t*)&orderData, "OrderData");
	}

	WRAPPER_WRITE(wrapper, "CvCity", m_iPopulationRank);
	WRAPPER_WRITE(wrapper, "CvCity", m_bPopulationRankValid);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiBaseYieldRank);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_abBaseYieldRankValid);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiYieldRank);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_abYieldRankValid);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiCommerceRank);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_abCommerceRankValid);

	m_Properties.writeWrapper(pStream);

	WRAPPER_WRITE_DECORATED(wrapper, "CvCity", m_aEventsOccured.size(), "iNumElts");
	foreach_(const EventTypes eEvent, m_aEventsOccured)
	{
		WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "CvCity", REMAPPED_CLASS_TYPE_EVENTS, eEvent, "eEvent");
	}

	WRAPPER_WRITE_DECORATED(wrapper, "CvCity", m_aBuildingYieldChange.size(), "iNumElts");
	foreach_(BuildingYieldChange& pChange, m_aBuildingYieldChange)
	{
		pChange.write(pStream);
	}

	WRAPPER_WRITE_DECORATED(wrapper, "CvCity", m_aBuildingCommerceChange.size(), "iNumElts");
	foreach_(BuildingCommerceChange& pChange, m_aBuildingCommerceChange)
	{
		pChange.write(pStream);
	}

	WRAPPER_WRITE_DECORATED(wrapper, "CvCity", m_aBuildingHappyChange.size(), "iNumElts");
	for (BuildingChangeArray::iterator it = m_aBuildingHappyChange.begin(); it != m_aBuildingHappyChange.end(); ++it)
	{
		WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, (*it).first, "iBuilding");
		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (*it).second, "iChange");
	}

	WRAPPER_WRITE_DECORATED(wrapper, "CvCity", m_aBuildingHealthChange.size(), "iNumElts");
	for (BuildingChangeArray::iterator it = m_aBuildingHealthChange.begin(); it != m_aBuildingHealthChange.end(); ++it)
	{
		WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, (*it).first, "iBuilding");
		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (*it).second, "iChange");
	}

	//TB Combat Mod (Buildings) begin
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatProductionModifier);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatRepelModifier);
#ifdef STRENGTH_IN_NUMBERS
	WRAPPER_WRITE(wrapper, "CvCity", m_iTotalFrontSupportPercentModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iTotalShortRangeSupportPercentModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iTotalMediumRangeSupportPercentModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iTotalLongRangeSupportPercentModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iTotalFlankSupportPercentModifier);
#endif

	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraTechSpecialistHappiness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraBuildingHappinessFromTech);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraTechSpecialistHealth);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraBuildingHealthFromTech);
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_ppaaiLocalSpecialistExtraYield[iI]);
	}
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraLocalCaptureProbabilityModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraLocalCaptureResistanceModifier);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_pabReligiouslyDisabledBuilding);
	WRAPPER_WRITE(wrapper, "CvCity", m_iPrioritySpecialist);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiSpecialistBannedCount);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraLocalDynamicDefense);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraRiverDefensePenalty);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraLocalRepel);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraMinDefense);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraBuildingDefenseRecoverySpeedModifier);
	WRAPPER_WRITE(wrapper, "CvCity", m_iModifiedBuildingDefenseRecoverySpeedCap);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraCityDefenseRecoverySpeedModifier);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatRepelAgainstModifier);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiUnitCombatDefenseAgainstModifier);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiDamageAttackingUnitCombatCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_COMBATINFOS, GC.getNumUnitCombatInfos(), m_paiHealUnitCombatTypeVolume);
	//TB Combat Mod (Buildings) end

	WRAPPER_WRITE(wrapper, "CvCity", m_iZoCCount);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraInsidiousness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iExtraInvestigation);
	WRAPPER_WRITE(wrapper, "CvCity", m_iSpecialistInsidiousness);
	WRAPPER_WRITE(wrapper, "CvCity", m_iSpecialistInvestigation);

	WRAPPER_WRITE_DECORATED(wrapper, "CvCity", m_aPropertySpawns.size(), "iNumElts");
	foreach_(PropertySpawns& kPropertySpawn, m_aPropertySpawns)
	{
		kPropertySpawn.write(pStream);
	}
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiExtraSpecialistCommerce);
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_ppaaiLocalSpecialistExtraCommerce[iI]);
	}
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiBaseYieldPerPopRate);
	WRAPPER_WRITE(wrapper, "CvCity", m_bVisibilitySetup);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiNewAfflictionTypeCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiNewExtraAfflictionOutbreakLevelChange);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiNewAfflictionToleranceChange);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiNewCurrentOvercomeChange);

	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROPERTIES, GC.getNumPropertyInfos(), m_ppaaiExtraBonusAidModifier[iI]);
	}

	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROPERTIES, GC.getNumPropertyInfos(), m_paiAidRate);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiPromotionLineAfflictionAttackCommunicability);
	WRAPPER_WRITE(wrapper, "CvCity", m_iQuarantinedCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvCity", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_pabHadRawVicinityBonus);
	WRAPPER_WRITE(wrapper, "CvCity", m_bPropertyControlBuildingQueued);

	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_aiExtraYield);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_aiBuildingCommerceTechChange);

	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_commercePerPopFromBuildings);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_buildingExtraYield100);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_YIELD_TYPES, m_buildingYieldMod);
	WRAPPER_WRITE_ARRAY(wrapper, "CvCity", NUM_COMMERCE_TYPES, m_buildingCommerceMod);
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (short)m_plotYieldChanges.size(), "PlotYieldChangesSize");
		//for (std::map<short, YieldArray>::const_iterator it = m_plotYieldChanges.begin(), itEnd = m_plotYieldChanges.end(); it != itEnd; ++it)
		typedef std::pair<short, YieldArray> PlotYieldChange;
		foreach_(const PlotYieldChange& plotYieldChange, m_plotYieldChanges)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", plotYieldChange.first, "PlotYieldChangesType");
			WRAPPER_WRITE_ARRAY_DECORATED(wrapper, "CvCity", NUM_YIELD_TYPES, plotYieldChange.second.elems, "PlotYieldChanges");
		}
	}
	WRAPPER_WRITE(wrapper, "CvCity", m_iRevIndexDistanceMod);

	// Toffer - Write vectors
	{
		{
			uint16_t iCityOutputHistorySize = CityOutputHistory::getCityOutputHistorySize();
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", iCityOutputHistorySize, "CityOutputHistorySize");
			for (uint16_t iI = 0; iI < iCityOutputHistorySize; iI++)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvCity", m_outputHistory.getRecentOutputTurn(iI), "RecentOutputTurn");
				uint16_t iCityOutputHistoryNumEntries = m_outputHistory.getCityOutputHistoryNumEntries(iI);
				WRAPPER_WRITE_DECORATED(wrapper, "CvCity", iCityOutputHistoryNumEntries, "CityOutputHistoryNumEntries");
				for (uint16_t iJ = 0; iJ < iCityOutputHistoryNumEntries; iJ++)
				{
					WRAPPER_WRITE_DECORATED(wrapper, "CvCity", m_outputHistory.getCityOutputHistoryEntry(iI, iJ, true), "OrderType");
					WRAPPER_WRITE_DECORATED(wrapper, "CvCity", m_outputHistory.getCityOutputHistoryEntry(iI, iJ, false), "Type");
				}
			}
		}

		// Techs
		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (short)m_buildingHappinessFromTech.size(), "BuildingHappinessFromTechSize");
		for (std::vector< std::pair<TechTypes, int> >::iterator it = m_buildingHappinessFromTech.begin(); it != m_buildingHappinessFromTech.end(); ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", static_cast<short>((*it).first), "BuildingHappinessFromTechType");
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (*it).second, "BuildingHappinessFromTechValue");
		}

		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (short)m_buildingHealthFromTech.size(), "BuildingHealthFromTechSize");
		for (std::vector< std::pair<TechTypes, int> >::iterator it = m_buildingHealthFromTech.begin(); it != m_buildingHealthFromTech.end(); ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", static_cast<short>((*it).first), "BuildingHealthFromTechType");
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (*it).second, "BuildingHealthFromTechValue");
		}

		// Buildings
		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (short)m_vFreeBuildings.size(), "FreeBuildingsSize");
		foreach_(const short building, m_vFreeBuildings)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", building, "FreeBuildingsIndex");
		}

		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (short)m_vDisabledBuildings.size(), "DisabledBuildingsSize");
		foreach_(const short building, m_vDisabledBuildings)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", building, "DisabledBuildingsIndex");
		}

		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (short)m_progressOnBuilding.size(), "BuildingProgressSize");
		for (std::vector< std::pair<BuildingTypes, int> >::iterator it = m_progressOnBuilding.begin(); it != m_progressOnBuilding.end(); ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", static_cast<short>((*it).first), "BuildingProgressType");
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (*it).second, "BuildingProgressValue");
		}

		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (short)m_delayOnBuilding.size(), "DelayOnBuildingSize");
		for (std::vector< std::pair<BuildingTypes, int> >::iterator it = m_delayOnBuilding.begin(); it != m_delayOnBuilding.end(); ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", static_cast<short>((*it).first), "DelayOnBuildingType");
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (*it).second, "DelayOnBuildingValue");
		}

		// Units
		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (short)m_progressOnUnit.size(), "UnitProgressSize");
		for (std::vector< std::pair<UnitTypes, int> >::iterator it = m_progressOnUnit.begin(); it != m_progressOnUnit.end(); ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", static_cast<short>((*it).first), "UnitProgressType");
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (*it).second, "UnitProgressValue");
		}

		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (short)m_workers.size(), "WorkersSize");
		foreach_(const int iUnitID, m_workers)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", iUnitID, "WorkerUnitID");
		}

		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (short)m_delayOnUnit.size(), "DelayOnUnitSize");
		for (std::vector< std::pair<UnitTypes, int> >::iterator it = m_delayOnUnit.begin(); it != m_delayOnUnit.end(); ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", static_cast<short>((*it).first), "DelayOnUnitType");
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (*it).second, "DelayOnUnitValue");
		}

		// Bonuses
		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (short)m_corpBonusProduction.size(), "CorpBonusProductionSize");
		for (std::vector< std::pair<BonusTypes, int> >::iterator it = m_corpBonusProduction.begin(); it != m_corpBonusProduction.end(); ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", static_cast<short>((*it).first), "CorpBonusProductionType");
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (*it).second, "CorpBonusProductionValue");
		}
	}
	// Toffer - Write Maps
	{
		// Bonus
		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (short)m_bonusDefenseChanges.size(), "BonusDefenseChangesSize");
		for (std::map<short, int>::const_iterator it = m_bonusDefenseChanges.begin(), itEnd = m_bonusDefenseChanges.end(); it != itEnd; ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", it->first, "BonusDefenseChangesType");
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", it->second, "BonusDefenseChanges");
		}

		// Building
		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (short)m_buildingProductionMod.size(), "BuildingProductionModSize");
		for (std::map<short, int>::const_iterator it = m_buildingProductionMod.begin(), itEnd = m_buildingProductionMod.end(); it != itEnd; ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", it->first, "BuildingProductionModType");
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", it->second, "BuildingProductionMod");
		}

		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (short)m_freeAreaBuildingCount.size(), "FreeAreaBuildingCountSize");
		for (std::map<short, uint16_t>::const_iterator it = m_freeAreaBuildingCount.begin(), itEnd = m_freeAreaBuildingCount.end(); it != itEnd; ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", it->first, "FreeAreaBuildingCountType");
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", it->second, "FreeAreaBuildingCount");
		}

		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (short)m_buildingLedger.size(), "BuildingLedgerSize");
		for (std::map<BuildingTypes, BuiltBuildingData>::const_iterator it = m_buildingLedger.begin(), itEnd = m_buildingLedger.end(); it != itEnd; ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", static_cast<short>(it->first), "BuildingLedgerType");
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", it->second.eBuiltBy, "BuildingLedgerBuiltBy");
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", it->second.iTimeBuilt, "BuildingLedgerTimeBuilt");
		}

		// Unit
		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (short)m_unitProductionMod.size(), "UnitProductionModSize");
		for (std::map<short, int>::const_iterator it = m_unitProductionMod.begin(), itEnd = m_unitProductionMod.end(); it != itEnd; ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", it->first, "UnitProductionModType");
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", it->second, "UnitProductionMod");
		}

		// Terrains
		WRAPPER_WRITE_DECORATED(wrapper, "CvCity", (short)m_terrainYieldChanges.size(), "TerrainYieldChangesSize");
		for (std::map<short, YieldArray>::const_iterator it = m_terrainYieldChanges.begin(), itEnd = m_terrainYieldChanges.end(); it != itEnd; ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvCity", it->first, "TerrainYieldChangesType");
			WRAPPER_WRITE_ARRAY_DECORATED(wrapper, "CvCity", NUM_YIELD_TYPES, it->second.elems, "TerrainYieldChanges");
		}
	}
	WRAPPER_WRITE_OBJECT_END(wrapper);
}


//------------------------------------------------------------------------------------------------
class VisibleBuildingComparator
{
public:
	bool operator() (BuildingTypes e1, BuildingTypes e2)
	{
		if (GC.getBuildingInfo(e1).getVisibilityPriority() > GC.getBuildingInfo(e2).getVisibilityPriority())
			return true;
		else if (GC.getBuildingInfo(e1).getVisibilityPriority() == GC.getBuildingInfo(e2).getVisibilityPriority())
		{
			//break ties by building type higher building type
			if (e1 > e2)
				return true;
		}

		return false;
	}
};

//	Flags to determine which building types are displayed
#define	SHOW_BUILDINGS_WONDERS	1
#define	SHOW_BUILDINGS_DEFENSES	2
#define	SHOW_BUILDINGS_OTHER	128

void CvCity::getVisibleBuildings(std::list<BuildingTypes>& kChosenVisible, int& iChosenNumGenerics)
{
	PROFILE_EXTRA_FUNC();
	if (!plot()->isGraphicsVisible(ECvPlotGraphics::CITY))
	{
		iChosenNumGenerics = 0;
		return;
	}

	const int iShowFlags = GC.getSHOW_BUILDINGS_LEVEL();
	std::vector<BuildingTypes> kVisible;

	foreach_(const BuildingTypes eType, getHasBuildings())
	{
		bool bValid = false;
		const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eType);

		if (kBuilding.getNotShowInCity()) continue;

		const bool bIsWonder = isLimitedWonder(eType);
		const bool bIsDefense = (kBuilding.getDefenseModifier() > 0);

		if ((iShowFlags & SHOW_BUILDINGS_WONDERS) != 0)
		{
			//	Wonders
			bValid |= bIsWonder;
		}
		if ((iShowFlags & SHOW_BUILDINGS_DEFENSES) != 0)
		{
			//	Wonders
			bValid |= bIsDefense;
		}
		if ((iShowFlags & SHOW_BUILDINGS_OTHER) != 0)
		{
			//	Wonders
			bValid |= (!bIsWonder && !bIsDefense);
		}

		if (bValid)
		{
			kVisible.push_back(eType);
		}
	}

	// sort the visible ones by decreasing priority
	algo::sort(kVisible, VisibleBuildingComparator());

	// how big is this city, in terms of buildings?
	// general rule: no more than fPercentUnique percent of a city can be uniques
	int iTotalVisibleBuildings = 0;
	if (stricmp(GC.getDefineSTRING("GAME_CITY_SIZE_METHOD"), "METHOD_EXPONENTIAL") == 0)
	{
		int iCityScaleMod = ((int)(pow((float)getPopulation(), GC.getDefineFLOAT("GAME_CITY_SIZE_EXP_MODIFIER")))) * 2;
		iTotalVisibleBuildings = (10 + iCityScaleMod);
	}
	else
	{
		float fLo = GC.getDefineFLOAT("GAME_CITY_SIZE_LINMAP_AT_0");
		float fHi = GC.getDefineFLOAT("GAME_CITY_SIZE_LINMAP_AT_50");
		float fCurSize = (float)getPopulation();
		iTotalVisibleBuildings = int(((fHi - fLo) / 50.0f) * fCurSize + fLo);
	}
	float fMaxUniquePercent = GC.getDefineFLOAT("GAME_CITY_SIZE_MAX_PERCENT_UNIQUE");
	int iMaxNumUniques = (int)(fMaxUniquePercent * iTotalVisibleBuildings);

	// compute how many buildings are generics vs. unique Civ buildings?
	int iNumGenerics;
	int iNumUniques;
	if ((int)kVisible.size() > iMaxNumUniques)
	{
		iNumUniques = iMaxNumUniques;
	}
	else
	{
		iNumUniques = kVisible.size();
	}
	iNumGenerics = iTotalVisibleBuildings - iNumUniques + getCitySizeBoost();

	// return
	iChosenNumGenerics = iNumGenerics;
	for (int i = 0; i < iNumUniques; i++)
	{
		kChosenVisible.push_back(kVisible[i]);
	}
}

static int natGetDeterministicRandom(int iMin, int iMax, int iSeedX, int iSeedY)
{
	srand(7297 * iSeedX + 2909 * iSeedY);
	return (rand() % (iMax - iMin)) + iMin;
}

void CvCity::getVisibleEffects(ZoomLevelTypes eCurZoom, std::vector<const char*>& kEffectNames)
{
	PROFILE_EXTRA_FUNC();
	if (isOccupation() && isVisible(getTeam(), false) == true)
	{
		if (eCurZoom == ZOOM_DETAIL)
		{
			kEffectNames.push_back("EFFECT_CITY_BIG_BURNING_SMOKE");
			kEffectNames.push_back("EFFECT_CITY_FIRE");
		}
		else
		{
			kEffectNames.push_back("EFFECT_CITY_BIG_BURNING_SMOKE");
		}
		return;
	}

	if ((getTeam() == GC.getGame().getActiveTeam()) || GC.getGame().isDebugMode())
	{

		if (angryPopulation() > 0)
		{
			kEffectNames.push_back("EFFECT_CITY_BURNING_SMOKE");
		}

		if (healthRate() < 0)
		{
			kEffectNames.push_back("EFFECT_CITY_DISEASED");
		}


		if (isWeLoveTheKingDay())
		{
			int iSeed = natGetDeterministicRandom(0, 32767, getX(), getY());
			CvRandom kRand;
			kRand.init(iSeed);

			// fireworks
			const char* szFireworkEffects[] =
			{ "FIREWORKS_RED_LARGE_SLOW",
				"FIREWORKS_RED_SMALL_FAST",
				"FIREWORKS_GREEN_LARGE_SLOW",
				"FIREWORKS_GREEN_SMALL_FAST",
				"FIREWORKS_PURPLE_LARGE_SLOW",
				"FIREWORKS_PURPLE_SMALL_FAST",
				"FIREWORKS_YELLOW_LARGE_SLOW",
				"FIREWORKS_YELLOW_SMALL_FAST",
				"FIREWORKS_BLUE_LARGE_SLOW",
				"FIREWORKS_BLUE_SMALL_FAST" };

			int iNumEffects = sizeof(szFireworkEffects) / sizeof(char*);
			for (int i = 0; i < (iNumEffects < 3 ? iNumEffects : 3); i++)
			{
				kEffectNames.push_back(szFireworkEffects[kRand.get(iNumEffects)]);
			}
		}
	}
}

void CvCity::getCityBillboardSizeIconColors(NiColorA& kDotColor, NiColorA& kTextColor) const
{
	const NiColorA kPlayerColor = GC.getColorInfo((ColorTypes)GC.getPlayerColorInfo(GET_PLAYER(getOwner()).getPlayerColor()).getColorTypePrimary()).getColor();
	static const NiColorA kGrowing(0.73f, 1, 0.73f, 1);
	static const NiColorA kShrinking(1, 0.73f, 0.73f, 1);
	static const NiColorA kStagnant(0.83f, 0.83f, 0.83f, 1);
	static const NiColorA kUnknown(.5f, .5f, .5f, 1);
	static const NiColorA kWhite(1, 1, 1, 1);
	static const NiColorA kBlack(0, 0, 0, 1);

	if ((getTeam() == GC.getGame().getActiveTeam()))
	{
		if (foodDifference() < 0)
		{
			if ((foodDifference() == -1) && (getFood() >= ((75 * growthThreshold()) / 100)))
			{
				kDotColor = kStagnant;
				kTextColor = kBlack;
			}
			else
			{
				kDotColor = kShrinking;
				kTextColor = kBlack;
			}
		}
		else if (foodDifference() > 0)
		{
			kDotColor = kGrowing;
			kTextColor = kBlack;
		}
		else if (foodDifference() == 0)
		{
			kDotColor = kStagnant;
			kTextColor = kBlack;
		}
	}
	else
	{
		kDotColor = kPlayerColor;
		const NiColorA kPlayerSecondaryColor = GC.getColorInfo((ColorTypes)GC.getPlayerColorInfo(GET_PLAYER(getOwner()).getPlayerColor()).getColorTypeSecondary()).getColor();
		kTextColor = kPlayerSecondaryColor;
	}
}

const char* CvCity::getCityBillboardProductionIcon() const
{
	if (canBeSelected() && isProduction())
	{
		bst::optional<OrderData> nextOrder = getHeadOrder();
		FAssert(nextOrder);

		const char* szIcon = NULL;
		switch (nextOrder->eOrderType)
		{
		case ORDER_TRAIN:
		{
			UnitTypes eType = getProductionUnit();
			FAssert(eType != NO_UNIT);
			szIcon = GET_PLAYER(getOwner()).getUnitButton(eType);
			break;
		}
		case ORDER_CONSTRUCT:
		{
			BuildingTypes eType = getProductionBuilding();
			FAssert(eType != NO_BUILDING);
			szIcon = GC.getBuildingInfo(eType).getButton();
			break;
		}
		case ORDER_CREATE:
		{
			ProjectTypes eType = getProductionProject();
			FAssert(eType != NO_PROJECT);
			szIcon = GC.getProjectInfo(eType).getButton();
			break;
		}
		case ORDER_MAINTAIN:
		{
			ProcessTypes eType = getProductionProcess();
			FAssert(eType != NO_PROCESS);
			szIcon = GC.getProcessInfo(eType).getButton();
			break;
		}
		case ORDER_LIST:
		{
			// Should not happen
			break;
		}
		default:
		{
			FErrorMsg("error");
		}
		}
		return szIcon;
	}
	return ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_NOPRODUCTION")->getPath();
}

bool CvCity::getFoodBarPercentages(std::vector<float>& afPercentages) const
{
	if (!canBeSelected())
	{
		return false;
	}
	afPercentages.resize(NUM_INFOBAR_TYPES, 0.0f);
	if (foodDifference() < 0)
	{
		afPercentages[INFOBAR_STORED] = std::max(0, (getFood() + foodDifference())) / (float)growthThreshold();
		afPercentages[INFOBAR_RATE_EXTRA] = std::min(-foodDifference(), getFood()) / (float)growthThreshold();
	}
	else
	{
		afPercentages[INFOBAR_STORED] = getFood() / (float)growthThreshold();
		afPercentages[INFOBAR_RATE] = foodDifference() / (float)growthThreshold();
	}
	return true;
}

bool CvCity::getProductionBarPercentages(std::vector<float>& afPercentages) const
{
	if (!canBeSelected())
	{
		return false;
	}

	if (!isProductionProcess())
	{
		afPercentages.resize(NUM_INFOBAR_TYPES, 0.0f);
		const int iProductionDiffNoFood = getCurrentProductionDifference(ProductionCalc::Overflow);
		const int iProductionDiffJustFood = getCurrentProductionDifference(ProductionCalc::FoodProduction | ProductionCalc::Overflow) - iProductionDiffNoFood;
		afPercentages[INFOBAR_STORED] = getProductionProgress() / (float)getProductionNeeded();
		afPercentages[INFOBAR_RATE] = iProductionDiffNoFood / (float)getProductionNeeded();
		afPercentages[INFOBAR_RATE_EXTRA] = iProductionDiffJustFood / (float)getProductionNeeded();
	}

	return true;
}

NiColorA CvCity::getBarBackgroundColor() const
{
	if (atWar(getTeam(), GC.getGame().getActiveTeam()))
	{
		return NiColorA(0.5f, 0, 0, 0.5f); // red
	}
	return NiColorA(0, 0, 0, 0.5f);
}

bool CvCity::isStarCity() const
{
	return isCapital();
}

bool CvCity::isValidBuildingLocation(BuildingTypes eBuilding) const
{
	PROFILE_FUNC();

	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	// if both the river and water flags are set, we require one of the two conditions, not both
	if (kBuilding.isWater())
	{
		if ((!kBuilding.isRiver() || !plot()->isRiver()) && !isCoastal(kBuilding.getMinAreaSize()))
		{
			return false;
		}
	}
	else if (area()->getNumTiles() < kBuilding.getMinAreaSize() || kBuilding.isRiver() && !plot()->isRiver())
	{
		return false;
	}

	if (!isValidTerrainForBuildings(eBuilding))
	{
		return false;
	}
	if (kBuilding.isFreshWater() && !plot()->isFreshWater())
	{
		return false;
	}
	if (!isMapCategory(*plot(), kBuilding))
	{
		return false;
	}
	return true;
}

bool CvCity::isEventTriggerPossible(EventTriggerTypes eTrigger) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumEventTriggerInfos(), eTrigger);

	const CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eTrigger);

	if (!CvString(kTrigger.getPythonCanDoCity()).empty())
	{
		if (!Cy::call<bool>(PYRandomEventModule, kTrigger.getPythonCanDoCity(), Cy::Args()
			<< eTrigger
			<< getOwner()
			<< getID()
			))
		{
			return false;
		}
	}

	if (kTrigger.getNumBuildings() > 0 && kTrigger.getNumBuildingsRequired() > 0)
	{
		bool bFoundValid = false;

		for (int i = 0; i < kTrigger.getNumBuildingsRequired(); ++i)
		{
			if (kTrigger.getBuildingRequired(i) != NO_BUILDING && isActiveBuilding((BuildingTypes)kTrigger.getBuildingRequired(i)))
			{
				bFoundValid = true;
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}


	if (getReligionCount() < kTrigger.getNumReligions())
	{
		return false;
	}

	if (kTrigger.getNumReligions() > 0 && kTrigger.getNumReligionsRequired() > 0)
	{
		bool bFoundValid = false;

		for (int i = 0; i < kTrigger.getNumReligionsRequired(); ++i)
		{
			if (!kTrigger.isStateReligion() || kTrigger.getReligionRequired(i) == GET_PLAYER(getOwner()).getStateReligion())
			{
				if (isHasReligion((ReligionTypes)kTrigger.getReligionRequired(i)))
				{
					if (!kTrigger.isHolyCity() || isHolyCity((ReligionTypes)kTrigger.getReligionRequired(i)))
					{
						bFoundValid = true;
					}
				}
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (getCorporationCount() < kTrigger.getNumCorporations())
	{
		return false;
	}

	if (kTrigger.getNumCorporations() > 0 && kTrigger.getNumCorporationsRequired() > 0)
	{
		bool bFoundValid = false;

		for (int i = 0; i < kTrigger.getNumCorporationsRequired(); ++i)
		{
			if (isHasCorporation((CorporationTypes)kTrigger.getCorporationRequired(i)))
			{
				if (!kTrigger.isHeadquarters() || isHeadquarters((CorporationTypes)kTrigger.getCorporationRequired(i)))
				{
					bFoundValid = true;
				}
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (kTrigger.getMinPopulation() > 0)
	{
		if (getPopulation() < kTrigger.getMinPopulation())
		{
			return false;
		}
	}


	if (kTrigger.getMaxPopulation() > 0)
	{
		if (getPopulation() > kTrigger.getMaxPopulation())
		{
			return false;
		}
	}

	if (kTrigger.getAngry() > 0)
	{
		if (unhappyLevel(0) - happyLevel() < kTrigger.getAngry())
		{
			return false;
		}
	}
	else if (kTrigger.getAngry() < 0)
	{
		if (happyLevel() - unhappyLevel(0) < -kTrigger.getAngry())
		{
			return false;
		}
	}

	if (kTrigger.getUnhealthy() > 0)
	{
		if (badHealth(false, 0) - goodHealth() < kTrigger.getUnhealthy())
		{
			return false;
		}
	}
	else if (kTrigger.getUnhealthy() < 0)
	{
		if (goodHealth() - badHealth(false, 0) < -kTrigger.getUnhealthy())
		{
			return false;
		}
	}

	if (kTrigger.isPrereqEventCity() && kTrigger.getNumPrereqEvents() > 0)
	{
		bool bFoundValid = true;

		for (int iI = 0; iI < kTrigger.getNumPrereqEvents(); ++iI)
		{
			if (!isEventOccured((EventTypes)kTrigger.getPrereqEvent(iI)))
			{
				bFoundValid = false;
				break;
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (!((*getPropertiesConst()) >= *kTrigger.getPrereqMinProperties()))
		return false;

	if (!((*getPropertiesConst()) <= *kTrigger.getPrereqMaxProperties()))
		return false;

	if (0 == getFood() && kTrigger.getCityFoodWeight() > 0)
	{
		return false;
	}
	return true;
}

int CvCity::getTriggerValue(EventTriggerTypes eTrigger) const
{
	FASSERT_BOUNDS(0, GC.getNumEventTriggerInfos(), eTrigger);

	if (!isEventTriggerPossible(eTrigger))
	{
		return MIN_INT;
	}

	return getFood() * GC.getEventTriggerInfo(eTrigger).getCityFoodWeight();
}

bool CvCity::canApplyEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const
{
	PROFILE_EXTRA_FUNC();
	const CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (!kEvent.isCityEffect() && !kEvent.isOtherPlayerCityEffect())
	{
		return true;
	}

	if (-1 == kTriggeredData.m_iCityId && kEvent.isCityEffect())
	{
		return false;
	}

	if (-1 == kTriggeredData.m_iOtherPlayerCityId && kEvent.isOtherPlayerCityEffect())
	{
		return false;
	}

	if (kEvent.getFood() + ((100 + kEvent.getFoodPercent()) * getFood()) / 100 < 0)
	{
		return false;
	}

	if (kEvent.getPopulationChange() + getPopulation() <= 0)
	{
		return false;
	}

	if (100 * kEvent.getCulture() + getCultureTimes100(getOwner()) < 0)
	{
		return false;
	}

	if (kEvent.getBuilding() != NO_BUILDING && kEvent.getBuildingChange() != 0)
	{
		if (kEvent.getBuildingChange() > 0)
		{
			if (hasBuilding((BuildingTypes)kEvent.getBuilding()))
			{
				return false;
			}
		}
		else if (!hasBuilding((BuildingTypes)kEvent.getBuilding()))
		{
			return false;
		}
	}

	if (-1 != kEvent.getMaxNumReligions() && getReligionCount() > kEvent.getMaxNumReligions())
	{
		return false;
	}

	if (kEvent.getMinPillage() > 0)
	{
		int iNumImprovements = 0;

		foreach_(const CvPlot* pPlot, plots(true))
		{
			if (pPlot->getOwner() == getOwner() && pPlot->isImprovementDestructible())
			{
				++iNumImprovements;
			}
		}
		if (iNumImprovements < kEvent.getMinPillage())
		{
			return false;
		}
	}
	return true;
}

void CvCity::applyEvent(EventTypes eEvent, const EventTriggeredData* pTriggeredData)
{
	PROFILE_EXTRA_FUNC();
	//	NULL pTriggeredData implies a replay after a reset of modifiers and only modifier effects
	//	should be applied
	bool	adjustModifiersOnly = (pTriggeredData == NULL);
	const EventTriggeredData& kTriggeredData = *pTriggeredData;

	if (!adjustModifiersOnly)
	{
		if (!canApplyEvent(eEvent, kTriggeredData))
		{
			return;
		}

		setEventOccured(eEvent, true);
	}

	const CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (kEvent.isCityEffect() || kEvent.isOtherPlayerCityEffect())
	{
		if (kEvent.getHappy() != 0)
		{
			changeExtraHappiness(kEvent.getHappy());
		}

		if (kEvent.getHealth() != 0)
		{
			changeExtraHealth(kEvent.getHealth());
		}

		if (kEvent.getHurryAnger() != 0 && !adjustModifiersOnly)
		{
			changeHurryAngerTimer(kEvent.getHurryAnger() * flatHurryAngerLength());
		}

		if (kEvent.getHappyTurns() != 0 && !adjustModifiersOnly)
		{
			changeHappinessTimer(kEvent.getHappyTurns());
		}

		if (kEvent.getFood() != 0 || kEvent.getFoodPercent() != 0)
		{
			changeFood(kEvent.getFood() + (kEvent.getFoodPercent() * getFood()) / 100);
		}

		if (kEvent.getPopulationChange() != 0 && !adjustModifiersOnly)
		{
			changePopulation(kEvent.getPopulationChange());
		}

		if (kEvent.getRevoltTurns() > 0 && !adjustModifiersOnly)
		{
			changeCultureUpdateTimer(kEvent.getRevoltTurns());
			changeOccupationTimer(kEvent.getRevoltTurns());
		}

		if (0 != kEvent.getSpaceProductionModifier())
		{
			changeSpaceProductionModifier(kEvent.getSpaceProductionModifier());
		}

		if (kEvent.getMaxPillage() > 0 && !adjustModifiersOnly)
		{
			FAssert(kEvent.getMaxPillage() >= kEvent.getMinPillage());
			int iNumPillage = kEvent.getMinPillage() + GC.getGame().getSorenRandNum(kEvent.getMaxPillage() - kEvent.getMinPillage(), "Pick number of event pillaged plots");

			int iNumPillaged = 0;
			for (int i = 0; i < iNumPillage; ++i)
			{
				int iRandOffset = GC.getGame().getSorenRandNum(NUM_CITY_PLOTS, "Pick event pillage plot");
				for (int j = 0; j < NUM_CITY_PLOTS; ++j)
				{
					int iPlot = (j + iRandOffset) % NUM_CITY_PLOTS;
					if (CITY_HOME_PLOT != iPlot)
					{
						CvPlot* pPlot = getCityIndexPlot(iPlot);

						if (NULL != pPlot && pPlot->getOwner() == getOwner() && pPlot->isImprovementDestructible())
						{
							AddDLLMessage(
								getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
								gDLL->getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", GC.getImprovementInfo(pPlot->getImprovementType()).getTextKeyWide()),
								"AS2D_PILLAGED", MESSAGE_TYPE_INFO, GC.getImprovementInfo(pPlot->getImprovementType()).getButton(), GC.getCOLOR_RED(), pPlot->getX(), pPlot->getY(), true, true
							);
							pPlot->setImprovementType(NO_IMPROVEMENT);
							++iNumPillaged;
							break;
						}
					}
				}
			}

			PlayerTypes eOtherPlayer = kTriggeredData.m_eOtherPlayer;
			if (!kEvent.isCityEffect() && kEvent.isOtherPlayerCityEffect())
			{
				eOtherPlayer = kTriggeredData.m_ePlayer;
			}

			if (NO_PLAYER != eOtherPlayer)
			{

				CvWString szBuffer = gDLL->getText("TXT_KEY_EVENT_NUM_CITY_IMPROVEMENTS_DESTROYED", iNumPillaged, GET_PLAYER(getOwner()).getCivilizationAdjectiveKey());
				AddDLLMessage(eOtherPlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGED", MESSAGE_TYPE_INFO);
			}
		}

		for (int i = 0; i < NUM_COMMERCE_TYPES; ++i)
		{
			if (kEvent.getCommerceModifier(i) != 0)
			{
				changeCommerceRateModifier((CommerceTypes)i, kEvent.getCommerceModifier(i));
			}
		}

		for (int i = 0; i < NUM_YIELD_TYPES; ++i)
		{
			if (kEvent.getYieldModifier(i) != 0)
			{
				changeYieldRateModifier((YieldTypes)i, kEvent.getYieldModifier(i));
			}
		}

		for (int i = 0; i < GC.getNumSpecialistInfos(); ++i)
		{
			if (kEvent.getFreeSpecialistCount(i) > 0)
			{
				changeFreeSpecialistCount((SpecialistTypes)i, kEvent.getFreeSpecialistCount(i));
			}
		}

		if (kEvent.getCulture() != 0 && !adjustModifiersOnly)
		{
			changeCulture(getOwner(), kEvent.getCulture(), true, true);
		}

		if (kEvent.getRevolutionIndexChange() > 0 && !adjustModifiersOnly)
		{
			changeLocalRevIndex(kEvent.getRevolutionIndexChange());
		}
		else if (kEvent.getRevolutionIndexChange() < 0 && !adjustModifiersOnly)
		{
			changeLocalRevIndex(std::max(-getLocalRevIndex(), kEvent.getRevolutionIndexChange()));
		}
	}


	if (kEvent.getFreeUnit() != NO_UNIT && !adjustModifiersOnly)
	{
		UnitTypes eUnit = (UnitTypes) kEvent.getFreeUnit();
		for (int i = 0; i < kEvent.getNumUnits(); ++i)
		{
			GET_PLAYER(getOwner()).initUnit(eUnit, getX(), getY(), NO_UNITAI, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
		}
	}

	const BuildingTypes eventBuilding = static_cast<BuildingTypes>(kEvent.getBuilding());
	if (eventBuilding != NO_BUILDING && !adjustModifiersOnly)
	{
		if (0 != kEvent.getBuildingChange())
		{
			changeHasBuilding(eventBuilding, kEvent.getBuildingChange() > 0);
		}
	}

	if (kEvent.getNumBuildingYieldChanges() > 0)
	{
		for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); ++iBuilding)
		{
			for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
			{
				setBuildingYieldChange((BuildingTypes)iBuilding, (YieldTypes)iYield, getBuildingYieldChange((BuildingTypes)iBuilding, (YieldTypes)iYield) + kEvent.getBuildingYieldChange(iBuilding, iYield));
			}
		}
	}

	if (kEvent.getNumBuildingCommerceChanges() > 0)
	{
		for (int iI = GC.getNumBuildingInfos() - 1; iI > -1; iI--)
		{
			for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; ++iJ)
			{
				const int iChange = kEvent.getBuildingCommerceChange(iI, iJ);
				if (iChange != 0)
				{
					changeBuildingCommerceChange(static_cast<BuildingTypes>(iI), static_cast<CommerceTypes>(iJ), iChange);
				}
			}
		}
	}

	if (kEvent.getNumBuildingHappyChanges() > 0)
	{
		for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); ++iBuilding)
		{
			setBuildingHappyChange((BuildingTypes)iBuilding, kEvent.getBuildingHappyChange(iBuilding));
		}
	}

	if (kEvent.getNumBuildingHealthChanges() > 0)
	{
		for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); ++iBuilding)
		{
			setBuildingHealthChange((BuildingTypes)iBuilding, kEvent.getBuildingHealthChange(iBuilding));
		}
	}

	getProperties()->addProperties(kEvent.getProperties());
	//GET_PLAYER(getOwner()).getProperties()->addProperties(kEvent.getProperties());
}

bool CvCity::isEventOccured(EventTypes eEvent) const
{
	return algo::any_of_equal(m_aEventsOccured, eEvent);
}

void CvCity::setEventOccured(EventTypes eEvent, bool bOccured)
{
	std::vector<EventTypes>::iterator itr = find(m_aEventsOccured.begin(), m_aEventsOccured.end(), eEvent);

	if (itr == m_aEventsOccured.end())
	{
		if (bOccured)
		{
			m_aEventsOccured.push_back(eEvent);
		}
	}
	else if (!bOccured)
	{
		m_aEventsOccured.erase(itr);
	}
}

// CACHE: cache frequently used values
///////////////////////////////////////
bool CvCity::hasShrine(ReligionTypes eReligion) const
{
	PROFILE_EXTRA_FUNC();
	// note, for normal XML, this count will be one, there is only one shrine of each religion
	foreach_(const BuildingTypes eBuilding, GC.getReligionInfo(eReligion).getShrineBuildings())
	{
		if (isActiveBuilding(eBuilding))
		{
			return true;
		}
	}
	return false;
}

bool CvCity::hasOrbitalInfrastructure() const
{
	PROFILE_EXTRA_FUNC();
	//ls612: To check if a city gets full benefits from Orbital Buildings
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (GC.getBuildingInfo((BuildingTypes)iI).isOrbitalInfrastructure() && isActiveBuilding((BuildingTypes)iI))
		{
			return true;
		}
	}
	return false;
}

void CvCity::invalidatePopulationRankCache()
{
	m_bPopulationRankValid = false;
}

void CvCity::invalidateYieldRankCache(YieldTypes eYield)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(NO_YIELD, NUM_YIELD_TYPES, eYield);

	if (eYield == NO_YIELD)
	{
		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			m_abBaseYieldRankValid[iI] = false;
			m_abYieldRankValid[iI] = false;
		}
	}
	else
	{
		m_abBaseYieldRankValid[eYield] = false;
		m_abYieldRankValid[eYield] = false;
	}
}

void CvCity::invalidateCommerceRankCache(CommerceTypes eCommerce)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(NO_COMMERCE, NUM_COMMERCE_TYPES, eCommerce);

	if (eCommerce == NO_COMMERCE)
	{
		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			m_abCommerceRankValid[iI] = false;
		}
	}
	else
	{
		m_abCommerceRankValid[eCommerce] = false;
	}
}

int CvCity::getBuildingYieldChange(BuildingTypes eBuilding, YieldTypes eYield) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const BuildingYieldChange& it, m_aBuildingYieldChange)
	{
		if (it.eBuilding == eBuilding && it.eYield == eYield)
		{
			return it.iChange;
		}
	}
	return 0;
}

void CvCity::setBuildingYieldChange(BuildingTypes eBuilding, YieldTypes eYield, int iChange)
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	bool bFound = false;
	const bool bErase = iChange == 0;
	foreach_(BuildingYieldChange& yieldChange, m_aBuildingYieldChange)
	{
		if (yieldChange.eBuilding == eBuilding && yieldChange.eYield == eYield)
		{
			bFound = true;
			const int iOldChange = yieldChange.iChange;
			if (iOldChange != iChange)
			{
				if (!bErase)
				{
					yieldChange.iChange = iChange;
				}

				if (hasFullyActiveBuilding(eBuilding))
				{
					changeExtraYield(eYield, iChange - iOldChange);
				}
			}
			break;
		}
		iCount++;
	}

	if (bFound)
	{
		if (bErase)
		{
			m_aBuildingYieldChange.erase(m_aBuildingYieldChange.begin()+iCount);
		}
		return;
	}
	else if (bErase) return;

	// Cache new vector entry.
	BuildingYieldChange kChange;
	kChange.eBuilding = eBuilding;
	kChange.eYield = eYield;
	kChange.iChange = iChange;
	m_aBuildingYieldChange.push_back(kChange);

	if (hasFullyActiveBuilding(eBuilding))
	{
		changeExtraYield(eYield, iChange);
	}
}

void CvCity::changeBuildingYieldChange(BuildingTypes eBuilding, YieldTypes eYield, int iChange)
{
	setBuildingYieldChange(eBuilding, eYield, getBuildingYieldChange(eBuilding, eYield) + iChange);
}

int CvCity::getBuildingCommerceChange(BuildingTypes eBuilding, CommerceTypes eCommerce) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const BuildingCommerceChange& it, m_aBuildingCommerceChange)
	{
		if (it.eBuilding == eBuilding && it.eCommerce == eCommerce)
		{
			return it.iChange;
		}
	}
	return 0;
}

void CvCity::setBuildingCommerceChange(BuildingTypes eBuilding, CommerceTypes eCommerce, int iChange)
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	bool bFound = false;
	const bool bErase = iChange == 0;
	foreach_(BuildingCommerceChange& commerceChange, m_aBuildingCommerceChange)
	{
		if (commerceChange.eBuilding == eBuilding && commerceChange.eCommerce == eCommerce)
		{
			bFound = true;
			if (commerceChange.iChange != iChange)
			{
				if (!bErase)
				{
					commerceChange.iChange = iChange;
				}
				updateBuildingCommerce();
			}
			break;
		}
		iCount++;
	}

	if (bFound)
	{
		if (bErase)
		{
			m_aBuildingCommerceChange.erase(m_aBuildingCommerceChange.begin()+iCount);
		}
		return;
	}
	else if (bErase) return;

	// Cache new vector entry.
	BuildingCommerceChange kChange;
	kChange.eBuilding = eBuilding;
	kChange.eCommerce = eCommerce;
	kChange.iChange = iChange;
	m_aBuildingCommerceChange.push_back(kChange);

	updateBuildingCommerce();
}

void CvCity::changeBuildingCommerceChange(BuildingTypes eBuilding, CommerceTypes eCommerce, int iChange)
{
	setBuildingCommerceChange(eBuilding, eCommerce, getBuildingCommerceChange(eBuilding, eCommerce) + iChange);
}


void CvCity::setBuildingHappyChange(BuildingTypes eBuilding, int iChange)
{
	PROFILE_EXTRA_FUNC();
	for (BuildingChangeArray::iterator it = m_aBuildingHappyChange.begin(); it != m_aBuildingHappyChange.end(); ++it)
	{
		if ((*it).first == eBuilding)
		{
			if ((*it).second != iChange)
			{
				const int iOldChange = (*it).second;

				m_aBuildingHappyChange.erase(it);

				if (hasFullyActiveBuilding(eBuilding))
				{
					if (iOldChange > 0)
					{
						changeBuildingGoodHappiness(-iOldChange);
					}
					else if (iOldChange < 0)
					{
						changeBuildingBadHappiness(-iOldChange);
					}

					if (iChange != 0)
					{
						m_aBuildingHappyChange.push_back(std::make_pair(eBuilding, iChange));
						if (iChange > 0)
						{
							changeBuildingGoodHappiness(iChange);
						}
						else if (iChange < 0)
						{
							changeBuildingBadHappiness(iChange);
						}
					}
				}
			}
			return;
		}
	}

	if (0 != iChange && hasFullyActiveBuilding(eBuilding))
	{
		m_aBuildingHappyChange.push_back(std::make_pair(eBuilding, iChange));

		if (iChange > 0)
		{
			changeBuildingGoodHappiness(iChange);
		}
		else if (iChange < 0)
		{
			changeBuildingBadHappiness(iChange);
		}
	}
}


int CvCity::getBuildingHappyChange(BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	for (BuildingChangeArray::const_iterator it = m_aBuildingHappyChange.begin(); it != m_aBuildingHappyChange.end(); ++it)
	{
		if ((*it).first == eBuilding)
		{
			return (*it).second;
		}
	}
	return 0;
}


void CvCity::setBuildingHealthChange(BuildingTypes eBuilding, int iChange)
{
	PROFILE_EXTRA_FUNC();
	for (BuildingChangeArray::iterator it = m_aBuildingHealthChange.begin(); it != m_aBuildingHealthChange.end(); ++it)
	{
		if ((*it).first == eBuilding)
		{
			if ((*it).second != iChange)
			{
				const int iOldChange = (*it).second;

				m_aBuildingHealthChange.erase(it);

				if (hasFullyActiveBuilding(eBuilding))
				{
					if (iOldChange > 0)
					{
						changeBuildingGoodHealth(-iOldChange);
					}
					else if (iOldChange < 0)
					{
						changeBuildingBadHealth(-iOldChange);
					}

					if (iChange != 0)
					{
						m_aBuildingHealthChange.push_back(std::make_pair(eBuilding, iChange));
						if (iChange > 0)
						{
							changeBuildingGoodHealth(iChange);
						}
						else if (iChange < 0)
						{
							changeBuildingBadHealth(iChange);
						}
					}
				}
			}
			return;
		}
	}

	if (0 != iChange && hasFullyActiveBuilding(eBuilding))
	{
		m_aBuildingHealthChange.push_back(std::make_pair(eBuilding, iChange));

		if (iChange > 0)
		{
			changeBuildingGoodHealth(iChange);
		}
		else if (iChange < 0)
		{
			changeBuildingBadHealth(iChange);
		}
	}
}


int CvCity::getBuildingHealthChange(BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	for (BuildingChangeArray::const_iterator it = m_aBuildingHealthChange.begin(); it != m_aBuildingHealthChange.end(); ++it)
	{
		if ((*it).first == eBuilding)
		{
			return (*it).second;
		}
	}

	return 0;
}

void CvCity::liberate(bool bConquest)
{
	PROFILE_EXTRA_FUNC();
	const PlayerTypes ePlayer = getLiberationPlayer(bConquest);

	if (NO_PLAYER == ePlayer) return;

	const PlayerTypes eOwner = getOwner();

	int iOldMasterLand = 0;
	int iOldVassalLand = 0;
	if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isVassal(GET_PLAYER(eOwner).getTeam()))
	{
		iOldMasterLand = GET_TEAM(GET_PLAYER(eOwner).getTeam()).getTotalLand();
		iOldVassalLand = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getTotalLand(false);
	}

	const CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_LIBERATED", getNameKey(), GET_PLAYER(eOwner).getNameKey(), GET_PLAYER(ePlayer).getCivilizationAdjectiveKey());
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHumanPlayer() && isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
		{
			AddDLLMessage(
				(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(),
				szBuffer, "AS2D_REVOLTEND", MESSAGE_TYPE_MAJOR_EVENT,
				ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(),
				GC.getCOLOR_HIGHLIGHT_TEXT(), getX(), getY(), true, true
			);
		}
	}
	GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, eOwner, szBuffer, getX(), getY(), GC.getCOLOR_HIGHLIGHT_TEXT());

	const int iCulture = getCultureTimes100(eOwner);
	const CvPlot* cityPlot = plot();
	GET_PLAYER(ePlayer).acquireCity(this, false, true, true); // Invalidates this city object. this::kill() is called.
	GET_PLAYER(ePlayer).AI_changeMemoryCount(eOwner, MEMORY_LIBERATED_CITIES, 1);

	if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isVassal(GET_PLAYER(eOwner).getTeam()))
	{
		int iNewMasterLand = GET_TEAM(GET_PLAYER(eOwner).getTeam()).getTotalLand();
		int iNewVassalLand = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getTotalLand(false);

		GET_TEAM(GET_PLAYER(ePlayer).getTeam()).setMasterPower(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getMasterPower() + iNewMasterLand - iOldMasterLand);
		GET_TEAM(GET_PLAYER(ePlayer).getTeam()).setVassalPower(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getVassalPower() + iNewVassalLand - iOldVassalLand);
	}
	CvCity* pCity = cityPlot->getPlotCity();

	FAssertMsg(NULL != pCity, "Not expected!")

	pCity->setCultureTimes100(ePlayer, pCity->getCultureTimes100(ePlayer) + iCulture / 2, true, true);

	if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAVassal())
	{
		for (int i = 0; i < GC.getCOLONY_NUM_FREE_DEFENDERS(); ++i)
		{
			pCity->initConscriptedUnit();
		}
	}
}

PlayerTypes CvCity::getLiberationPlayer(bool bConquest) const
{
	PROFILE_EXTRA_FUNC();
	if (isCapital())
	{
		return NO_PLAYER;
	}

	for (int iI = 0; iI < MAX_PC_PLAYERS; ++iI)
	{
		const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.isAlive() && kLoopPlayer.getParent() == getOwner())
		{
			const CvCity* pLoopCapital = kLoopPlayer.getCapitalCity();
			if (NULL != pLoopCapital && pLoopCapital->area() == area())
			{
				return (PlayerTypes)iI;
			}
		}
	}

	const CvPlayer& kOwner = GET_PLAYER(getOwner());
	if (kOwner.canSplitEmpire() && kOwner.canSplitArea(area()->getID()))
	{
		const PlayerTypes ePlayer = kOwner.getSplitEmpirePlayer(area()->getID());

		if (NO_PLAYER != ePlayer && GET_PLAYER(ePlayer).isAlive())
		{
			return ePlayer;
		}
	}

	const int iTotalCultureTimes100 = countTotalCultureTimes100();
	PlayerTypes eBestPlayer = NO_PLAYER;
	int iBestValue = 0;

	for (int iI = 0; iI < MAX_PC_PLAYERS; ++iI)
	{
		const CvPlayer& playerX = GET_PLAYER((PlayerTypes)iI);

		if (playerX.isAlive())
		{
			const CvCity* pCapital = playerX.getCapitalCity();
			if (NULL != pCapital)
			{
				int iCapitalDistance = ::plotDistance(getX(), getY(), pCapital->getX(), pCapital->getY());
				if (area() != pCapital->area())
				{
					iCapitalDistance *= 2;
				}

				int iCultureTimes100 = getCultureTimes100((PlayerTypes)iI);

				if (bConquest && iI == getOriginalOwner())
				{
					iCultureTimes100 *= 3;
					iCultureTimes100 /= 2;
				}

				if (playerX.getTeam() == getTeam()
				|| GET_TEAM(playerX.getTeam()).isVassal(getTeam())
				|| GET_TEAM(getTeam()).isVassal(playerX.getTeam()))
				{
					iCultureTimes100 *= 2;
					iCultureTimes100 = (iCultureTimes100 + iTotalCultureTimes100) / 2;
				}

				const int iValue = std::max(100, iCultureTimes100) / std::max(1, iCapitalDistance);

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestPlayer = (PlayerTypes)iI;
				}
			}
		}
	}

	if (NO_PLAYER != eBestPlayer)
	{
		if (getOwner() == eBestPlayer)
		{
			return NO_PLAYER;
		}
		for (int iPlot = 0; iPlot < getNumCityPlots(); ++iPlot)
		{
			CvPlot* pLoopPlot = ::plotCity(getX(), getY(), iPlot);

			if (NULL != pLoopPlot && pLoopPlot->isVisibleEnemyUnit(eBestPlayer))
			{
				return NO_PLAYER;
			}
		}
	}
	return eBestPlayer;
}

int CvCity::getBestYieldAvailable(YieldTypes eYield) const
{
	PROFILE_EXTRA_FUNC();
	int iBestYieldAvailable = 0;

	for (int iJ = SKIP_CITY_HOME_PLOT; iJ < NUM_CITY_PLOTS; ++iJ)
	{
		if (!isWorkingPlot(iJ))
		{
			const CvPlot* pPlot = getCityIndexPlot(iJ);

			if (NULL != pPlot && canWork(pPlot) && pPlot->getYield(eYield) > iBestYieldAvailable)
			{
				iBestYieldAvailable = pPlot->getYield(eYield);
			}
		}
	}

	for (int iJ = 0; iJ < GC.getNumSpecialistInfos(); ++iJ)
	{
		if (isSpecialistValid((SpecialistTypes)iJ, 1))
		{
			int iYield = GC.getSpecialistInfo((SpecialistTypes)iJ).getYieldChange(eYield);
			if (iYield > iBestYieldAvailable)
			{
				iBestYieldAvailable = iYield;
			}
		}
	}

	return iBestYieldAvailable;
}

int CvCity::getMusicScriptId() const
{
	bool bIsHappy = true;
	if (GC.getGame().getActiveTeam() == getTeam())
	{
		if (angryPopulation() > 0)
		{
			bIsHappy = false;
		}
	}
	else
	{
		if (GET_TEAM(GC.getGame().getActiveTeam()).isAtWar(getTeam()))
		{
			bIsHappy = false;
		}
	}

	const CvLeaderHeadInfo& kLeaderInfo = GC.getLeaderHeadInfo(GET_PLAYER(getOwner()).getLeaderType());
	const EraTypes eCurEra = GET_PLAYER(getOwner()).getCurrentEra();
	if (bIsHappy)
	{
		return (kLeaderInfo.getDiploPeaceMusicScriptIds(eCurEra));
	}
	else
	{
		return (kLeaderInfo.getDiploWarMusicScriptIds(eCurEra));
	}
}

int CvCity::getSoundscapeScriptId() const
{
	return GC.getEraInfo(GET_PLAYER(getOwner()).getCurrentEra()).getCitySoundscapeSciptId(getCitySizeType());
}

void CvCity::cheat(bool bCtrl, bool bAlt, bool bShift)
{
	if (gDLL->getChtLvl() > 0 || GC.getGame().isDebugMode())
	{
		if (bCtrl)
		{
			changeCulture(getOwner(), GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent(), true, true);
		}
		else if (bShift)
		{
			changePopulation(1);
		}
		else
		{
			popOrder(0, true);
		}
	}
}

void CvCity::getBuildQueue(std::vector<std::string>& astrQueue) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const OrderData& order, m_orderQueue)
	{
		switch (order.eOrderType)
		{
		case ORDER_TRAIN:
			astrQueue.push_back(GC.getUnitInfo(order.getUnitType()).getType());
			break;

		case ORDER_CONSTRUCT:
			astrQueue.push_back(GC.getBuildingInfo(order.getBuildingType()).getType());
			break;

		case ORDER_CREATE:
			astrQueue.push_back(GC.getProjectInfo(order.getProjectType()).getType());
			break;

		case ORDER_MAINTAIN:
			astrQueue.push_back(GC.getProcessInfo(order.getProcessType()).getType());
			break;

		case ORDER_LIST:
			astrQueue.push_back("List");
			break;

		default:
			FErrorMsg(CvString::format("Unexpected eOrderType %d", order.eOrderType).c_str());
			break;
		}
	}
}

/************************************************************************************************/
/* INFLUENCE_DRIVEN_WAR                   04/16/09                                johnysmith    */
/*                                                                                              */
/* Original Author Moctezuma              Start                                                 */
/************************************************************************************************/
// ------ BEGIN InfluenceDrivenWar -------------------------------
void CvCity::emergencyConscript()
{
	if (getConscriptUnit() == NO_UNIT)
	{
		return;
	}
	if (getConscriptAngerTimer() > 3 * flatConscriptAngerLength() * GC.getIDW_EMERGENCY_DRAFT_ANGER_MULTIPLIER() / 100)
	{
		return;
	}
	changeConscriptAngerTimer(flatConscriptAngerLength() * GC.getIDW_EMERGENCY_DRAFT_ANGER_MULTIPLIER() / 100);
	changePopulation(-1);

	const UnitTypes eConscriptUnit = getConscriptUnit();
	UnitAITypes eCityAI;

	if (GET_PLAYER(getOwner()).AI_unitValue(eConscriptUnit, UNITAI_CITY_DEFENSE, area()) > 0)
	{
		eCityAI = UNITAI_CITY_DEFENSE;
	}
	else if (GET_PLAYER(getOwner()).AI_unitValue(eConscriptUnit, UNITAI_CITY_COUNTER, area()) > 0)
	{
		eCityAI = UNITAI_CITY_COUNTER;
	}
	else if (GET_PLAYER(getOwner()).AI_unitValue(eConscriptUnit, UNITAI_CITY_SPECIAL, area()) > 0)
	{
		eCityAI = UNITAI_CITY_SPECIAL;
	}
	else
	{
		eCityAI = NO_UNITAI;
	}

	CvUnit* pUnit = GET_PLAYER(getOwner()).initUnit(eConscriptUnit, getX(), getY(), eCityAI, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
	if (pUnit == NULL)
	{
		FErrorMsg("pUnit is expected to be assigned a valid unit object");
		return;
	}
	addProductionExperience(pUnit, true);
	pUnit->setMoves(0);
	pUnit->setDamage((100 - GC.getIDW_EMERGENCY_DRAFT_STRENGTH()) * pUnit->getMaxHP() / 100, getOwner());
}
// ------ END InfluenceDrivenWar ---------------------------------


int CvCity::getRevTrend() const
{
	if (!GC.getGame().isOption(GAMEOPTION_UNSUPPORTED_REVOLUTION))
		return 0;

	//This is the value from python
	int iRevInsigatorThreshold = 1000;
	int iRevIndex = std::min(getRevolutionIndex(), iRevInsigatorThreshold);
	int iDeltaTrend = iRevIndex - getRevIndexAverage();
	if (iDeltaTrend != 0)
		iDeltaTrend *= std::max(abs(iDeltaTrend), 1 + iRevInsigatorThreshold / 100);
	return iDeltaTrend;
}

bool CvCity::isInquisitionConditions() const
{
	PROFILE_EXTRA_FUNC();
	const ReligionTypes eStateReligion = GET_PLAYER(getOwner()).getStateReligion();

	if (eStateReligion == NO_RELIGION)
	{
		return false;
	}
	if (isHasReligion(eStateReligion))
	{
		for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			if (isHasReligion(ReligionTypes(iI)) && ReligionTypes(iI) != eStateReligion
			&& (!isHolyCity(ReligionTypes(iI)) || isHolyCity(ReligionTypes(iI)) && GC.isOC_RESPAWN_HOLY_CITIES()))
			{
				return true;
			}
		}
	}
	return false;
}

/*
Checks the cities culture level and if it meets the criteria specified in the CultureLevelsInfo.xml, the city radius expands.
NUM_CITY_PLOTS is the largest city size, a radius of 3, NUM_CITY_PLOTS_2 is the standard BTS city size ( a radius of 2), and NUM_CITY_PLOTS_3
is a city size of 1.
*/
int CvCity::getNumCityPlots() const
{
	if (getWorkableRadiusOverride() == 0 && !GC.getGame().isOption(GAMEOPTION_EXP_LARGER_CITIES))
	{
		return NUM_CITY_PLOTS_2;
	}
	if (getCultureLevel() == -1)
	{
		return NUM_CITY_PLOTS_1;
	}
	const int iRadius =
	(
		getWorkableRadiusOverride() > 0
		?
		getWorkableRadiusOverride()
		:
		GC.getCultureLevelInfo(getCultureLevel()).getCityRadius()
	);
	switch (iRadius)
	{
		case 3:
		{
			return NUM_CITY_PLOTS;
		}
		case 2:
		{
			return NUM_CITY_PLOTS_2;
		}
		case 1:
		{
			return NUM_CITY_PLOTS_1;
		}
		default:
		{
			return NUM_CITY_PLOTS_2;
		}
	}
}

/*
 updateYieldRate(...) checks to see if the building given is in the city, and if it is, updates the yield by the iChange amount.
*/
void CvCity::updateYieldRate(BuildingTypes eBuilding, YieldTypes eYield, int iChange)
{
	setBuildingYieldChange(eBuilding, eYield, iChange);
}

/*
Given a building type, specialist type and the amount of specialists to change, updateMaxSpecialistCount changes the specialist count by the iChange value.
*/
void CvCity::updateMaxSpecialistCount(BuildingTypes eBuilding, SpecialistTypes eSpecialist, int iChange)
{
	if (hasFullyActiveBuilding(eBuilding))
	{
		changeMaxSpecialistCount(eSpecialist, iChange);
	}
}


/*
	Checks to see if the city is producing a wonder
*/
bool CvCity::isProductionWonder() const
{
	bst::optional<OrderData> headOrder = getHeadOrder();

	if (headOrder && headOrder->eOrderType == ORDER_CONSTRUCT)
	{
		return isLimitedWonder(headOrder->getBuildingType());
	}
	return false;
}

void CvCity::clearLostProduction()
{
	m_iLostProductionModified = 0;
	m_iGoldFromLostProduction = 0;
}

int CvCity::getSpecialistGoodHealth() const
{
	return m_iSpecialistGoodHealth;
}

int CvCity::getSpecialistBadHealth() const
{
	return m_iSpecialistBadHealth;
}

int CvCity::getSpecialistHappiness() const
{
	return m_iSpecialistHappiness;
}

int CvCity::getSpecialistUnhappiness() const
{
	return m_iSpecialistUnhappiness;
}

void CvCity::changeSpecialistGoodHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iSpecialistGoodHealth += iChange;

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

void CvCity::changeSpecialistBadHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iSpecialistBadHealth += iChange;

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

void CvCity::changeSpecialistHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iSpecialistHappiness += iChange;

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}


void CvCity::changeSpecialistUnhappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iSpecialistUnhappiness += iChange;

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

int CvCity::getImprovementGoodHealth() const
{
	return m_iImprovementGoodHealth;
}


int CvCity::getImprovementBadHealth() const
{
	return m_iImprovementBadHealth;
}

void CvCity::updateImprovementHealth()
{
	PROFILE_EXTRA_FUNC();
	const CvPlayer& player = GET_PLAYER(getOwner());

	int iNewGoodHealthPercent = 0;
	int iNewBadHealthPercent = 0;
	foreach_(CvPlot* pLoopPlot, plots())
	{
		if (pLoopPlot->getOwner() != NO_PLAYER && pLoopPlot->getOwner() == getOwner())
		{
			const ImprovementTypes eImprovement = pLoopPlot->getImprovementType();

			if (eImprovement != NO_IMPROVEMENT)
			{
				int iHealthPercent = GC.getImprovementInfo(eImprovement).getHealthPercent();

				for (int iJ = 0; iJ < GC.getNumCivicOptionInfos(); iJ++)
				{
					if (player.getCivics((CivicOptionTypes)iJ) != NO_CIVIC)
					{
						iHealthPercent += GC.getCivicInfo(player.getCivics((CivicOptionTypes)iJ)).getImprovementHealthPercentChanges(eImprovement);
					}
				}

				if (iHealthPercent > 0)
				{
					iNewGoodHealthPercent += iHealthPercent;
				}
				else if (iHealthPercent < 0)
				{
					iNewBadHealthPercent += iHealthPercent;
				}
			}
		}
	}

	if (m_iImprovementGoodHealth != iNewGoodHealthPercent || m_iImprovementBadHealth != iNewBadHealthPercent)
	{
		m_iImprovementGoodHealth = iNewGoodHealthPercent;
		m_iImprovementBadHealth = iNewBadHealthPercent;
		FAssert(getImprovementGoodHealth() >= 0);
		FAssert(getImprovementBadHealth() <= 0);

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}


int CvCity::getLandmarkAngerTimer() const
{
	return m_iLandmarkAngerTimer;
}

void CvCity::changeLandmarkAngerTimer(int iChange)
{
	if (iChange != 0)
	{
		m_iLandmarkAngerTimer += iChange;
		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

int CvCity::getLandmarkAnger() const
{
	int iAnger = 0;
	if (getLandmarkAngerTimer() > 0)
	{
		iAnger++;
	}
	int iDivisor = std::max(1, GC.getLANDMARK_ANGER_DIVISOR());
	iDivisor *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
	iDivisor /= 100;

	iAnger += getLandmarkAngerTimer() / std::max(1, iDivisor);

	return iAnger;
}

int CvCity::getCivicHappiness() const
{
	return GET_PLAYER(getOwner()).getCivicHappiness();
}

bool CvCity::isBuiltFoodProducedUnit() const
{
	return m_bBuiltFoodProducedUnit;
}

void CvCity::setBuiltFoodProducedUnit(bool bNewValue)
{
	m_bBuiltFoodProducedUnit = bNewValue;
}

int CvCity::getBonusCommerceRateModifier(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_aiBonusCommerceRateModifier[eIndex];
}

void CvCity::changeBonusCommerceRateModifier(CommerceTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiBonusCommerceRateModifier[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(getCommerceRate(eIndex));

		GET_PLAYER(getOwner()).invalidateCommerceRankCache(eIndex);

		setCommerceModifierDirty(eIndex);
		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

int CvCity::getBonusCommerceRateModifier(CommerceTypes eIndex, BonusTypes eBonus) const
{
	PROFILE_EXTRA_FUNC();
	int iModifier = 0;
	foreach_(const BuildingTypes eTypeX, getHasBuildings())
	{
		if (!isReligiouslyLimitedBuilding(eTypeX) && !isDisabledBuilding(eTypeX))
		{
			iModifier += GC.getBuildingInfo(eTypeX).getBonusCommerceModifier(eBonus, eIndex);
		}
	}
	return iModifier;
}


void CvCity::changeBonusCommercePercentChanges(CommerceTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiBonusCommercePercentChanges[eIndex] += iChange;
		setCommerceDirty(eIndex);
	}
}

int CvCity::getBonusCommercePercentChanges(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_aiBonusCommercePercentChanges[eIndex];
}

int CvCity::getBonusCommercePercentChanges(CommerceTypes eIndex, BonusTypes eBonus) const
{
	PROFILE_EXTRA_FUNC();
	int iPercentCommerce = 0;
	foreach_(const BuildingTypes eTypeX, getHasBuildings())
	{
		if (!isReligiouslyLimitedBuilding(eTypeX) && !isDisabledBuilding(eTypeX))
		{
			iPercentCommerce += GC.getBuildingInfo(eTypeX).getBonusCommercePercentChanges(eBonus, eIndex);
		}
	}
	return iPercentCommerce;
}

int CvCity::getBonusCommercePercentChanges(CommerceTypes eIndex, BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	if (!hasFullyActiveBuilding(eBuilding))
	{
		return 0;
	}
	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	int iPercentCommerce = 0;
	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		if (hasBonus((BonusTypes)iI))
		{
			iPercentCommerce += kBuilding.getBonusCommercePercentChanges((BonusTypes)iI, eIndex);
		}
	}
	return iPercentCommerce;
}


void CvCity::changeBuildingCommerceTechChange(CommerceTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiBuildingCommerceTechChange[eIndex] += iChange;
		setCommerceDirty(eIndex);
	}
}


int CvCity::getBuildingCommerceTechChange(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_aiBuildingCommerceTechChange[eIndex];
}

int CvCity::getBuildingCommerceTechChange(CommerceTypes eIndex, TechTypes eTech) const
{
	PROFILE_EXTRA_FUNC();
	int iCommerce100 = 0;
	foreach_(const BuildingTypes eTypeX, getHasBuildings())
	{
		if (!isReligiouslyLimitedBuilding(eTypeX) && !isDisabledBuilding(eTypeX))
		{
			foreach_(const TechCommerceArray& pair, GC.getBuildingInfo(eTypeX).getTechCommerceChanges100())
			{
				if (eTech == pair.first)
				{
					iCommerce100 += pair.second[eIndex];
				}
			}
		}
	}
	return iCommerce100;
}


int CvCity::getBuildingCommerceTechModifier(CommerceTypes eYield, TechTypes eTech) const
{
	PROFILE_EXTRA_FUNC();
	int iMod = 0;
	foreach_(const BuildingTypes eTypeX, getHasBuildings())
	{
		if (!isReligiouslyLimitedBuilding(eTypeX) && !isDisabledBuilding(eTypeX))
		{
			foreach_(const TechCommerceArray& pair, GC.getBuildingInfo(eTypeX).getTechCommerceModifiers())
			{
				if (eTech == pair.first)
				{
					iMod += pair.second[eYield];
				}
			}
		}
	}
	return iMod;
}


int CvCity::getBuildingYieldTechChange(YieldTypes eYield, TechTypes eTech) const
{
	PROFILE_EXTRA_FUNC();
	int iYield100 = 0;
	foreach_(const BuildingTypes eTypeX, getHasBuildings())
	{
		if (!isReligiouslyLimitedBuilding(eTypeX) && !isDisabledBuilding(eTypeX))
		{
			foreach_(const TechArray& pair, GC.getBuildingInfo(eTypeX).getTechYieldChanges100())
			{
				if (eTech == pair.first)
				{
					iYield100 += pair.second[eYield];
				}
			}
		}
	}
	return iYield100;
}


int CvCity::getBuildingYieldTechModifier(YieldTypes eYield, TechTypes eTech) const
{
	PROFILE_EXTRA_FUNC();
	int iMod = 0;
	foreach_(const BuildingTypes eTypeX, getHasBuildings())
	{
		if (!isReligiouslyLimitedBuilding(eTypeX) && !isDisabledBuilding(eTypeX))
		{
			foreach_(const TechArray& pair, GC.getBuildingInfo(eTypeX).getTechYieldModifiers())
			{
				if (eTech == pair.first)
				{
					iMod += pair.second[eYield];
				}
			}
		}
	}
	return iMod;
}


void CvCity::changeUnitProductionModifier(const UnitTypes eUnit, const int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);
	if (iChange == 0)
	{
		return;
	}
	std::map<short, int>::const_iterator itr = m_unitProductionMod.find((short)eUnit);

	if (itr == m_unitProductionMod.end())
	{
		m_unitProductionMod.insert(std::make_pair((short)eUnit, iChange));
	}
	else if (itr->second == -iChange)
	{
		m_unitProductionMod.erase(itr->first);
	}
	else // change unit mod
	{
		m_unitProductionMod[itr->first] += iChange;
	}
}

int CvCity::getUnitProductionModifier(const UnitTypes eUnit) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);
	std::map<short, int>::const_iterator itr = m_unitProductionMod.find((short)eUnit);
	return itr != m_unitProductionMod.end() ? itr->second : 0;
}


void CvCity::changeBuildingProductionModifier(const BuildingTypes eIndex, const int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	if (iChange == 0)
	{
		return;
	}
	std::map<short, int>::const_iterator itr = m_buildingProductionMod.find((short)eIndex);

	if (itr == m_buildingProductionMod.end())
	{
		m_buildingProductionMod.insert(std::make_pair((short)eIndex, iChange));
	}
	else if (itr->second == -iChange)
	{
		m_buildingProductionMod.erase(itr->first);
	}
	else // change building mod
	{
		m_buildingProductionMod[itr->first] += iChange;
	}
}

int CvCity::getBuildingProductionModifier(const BuildingTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	std::map<short, int>::const_iterator itr = m_buildingProductionMod.find((short)eIndex);
	return itr != m_buildingProductionMod.end() ? itr->second : 0;
}


void CvCity::changeBonusDefenseChanges(const BonusTypes eBonus, const int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eBonus);

	if (iChange == 0)
	{
		FErrorMsg("This is not a change!");
		return;
	}
	std::map<short, int>::const_iterator itr = m_bonusDefenseChanges.find((short)eBonus);

	if (itr == m_bonusDefenseChanges.end())
	{
		m_bonusDefenseChanges.insert(std::make_pair((short)eBonus, iChange));
	}
	else if (itr->second == -iChange)
	{
		m_bonusDefenseChanges.erase(itr->first);
	}
	else // change defense
	{
		m_bonusDefenseChanges[itr->first] += iChange;
	}
}

int CvCity::getBonusDefenseChanges(const BonusTypes eBonus) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eBonus);
	std::map<short, int>::const_iterator itr = m_bonusDefenseChanges.find((short)eBonus);
	return itr != m_bonusDefenseChanges.end() ? itr->second : 0;
}

int CvCity::calculateBonusDefense() const
{
	PROFILE_EXTRA_FUNC();
	int iBonusDefense = 0;

	for (std::map<short, int>::const_iterator itr = m_bonusDefenseChanges.begin(); itr != m_bonusDefenseChanges.end(); ++itr)
	{
		if (hasBonus((BonusTypes)itr->first))
		{
			iBonusDefense += itr->second;
		}
	}
	return iBonusDefense;
}


bool CvCity::hadVicinityBonus(BonusTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eIndex);
	return m_pabHadVicinityBonus[eIndex];
}

bool CvCity::hadRawVicinityBonus(BonusTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eIndex);
	return m_pabHadRawVicinityBonus[eIndex];
}


void CvCity::recalculatePopulationgrowthratepercentage()
{
	PROFILE_FUNC();

	m_fPopulationgrowthratepercentageLog = 0;

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		const BuildingTypes eLoopBuilding = static_cast<BuildingTypes>(iI);
		if (isActiveBuilding(eLoopBuilding) && GC.getBuildingInfo(eLoopBuilding).getPopulationgrowthratepercentage() != 0)
		{
			changePopulationgrowthratepercentage(GC.getBuildingInfo(eLoopBuilding).getPopulationgrowthratepercentage(), true);
		}
	}
}

int CvCity::getPopulationgrowthratepercentage() const
{
	return (int)(exp(m_fPopulationgrowthratepercentageLog) * 100 - 100);
}

void CvCity::changePopulationgrowthratepercentage(int iChange, bool bAdd)
{
	m_fPopulationgrowthratepercentageLog += (bAdd ? 1 : -1) * log((100 + (float)iChange) / 100);
}

void CvCity::doPromotion()
{
	PROFILE_FUNC();

	if (isDisorder())
	{
		return;
	}

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iI);
		if (!isActiveBuilding(eBuilding))
		{
			continue;
		}
		const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

		if (kBuilding.isApplyFreePromotionOnMove() && !kBuilding.getFreePromoTypes().empty())
		{
			foreach_(CvUnit* pLoopUnit, plot()->units())
			{
				if (pLoopUnit->getTeam() == GET_PLAYER(getOwner()).getTeam())
				{
					assignPromotionsFromBuildingChecked(kBuilding, pLoopUnit);
				}
			}
		}
	}
}


bool CvCity::isValidTerrainForBuildings(BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	//This had to be hardcoded, since there is a terrain peak, but it is really a plot type, not a terrain.
	const int iTerrainPeak = (int)GC.getTERRAIN_PEAK();
	const int iTerrainHill = (int)GC.getTERRAIN_HILL();

	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	// Terrain checks requires owned tile
	bool bRequiresTerrain = false;
	bool bValidTerrain = false;
	for (int iI = 0; iI < GC.getNumTerrainInfos(); iI++)
	{
		if (kBuilding.isPrereqOrTerrain(iI) && !bValidTerrain)
		{
			bRequiresTerrain = true;
			const bool bPeak = iI == iTerrainPeak;
			const bool bHill = iI == iTerrainHill;

			foreach_(const CvPlot* plotX, plots())
			{
				if (plotX->getTeam() != getTeam()) continue;

				if (bPeak)
				{
					if (plotX->isAsPeak())
					{
						bValidTerrain = true;
						break;
					}
				}
				else if (bHill)
				{
					if (plotX->isHills())
					{
						bValidTerrain = true;
						break;
					}
				}
				else if (plotX->getTerrainType() == iI)
				{
					bValidTerrain = true;
					break;
				}
			}
		}

		if (kBuilding.isPrereqAndTerrain(iI))
		{
			const bool bPeak = iI == iTerrainPeak;
			const bool bHill = iI == iTerrainHill;

			bool bHasAndTerrain = false;
			foreach_(const CvPlot* plotX, plots())
			{
				if ( plotX->getTeam() != getTeam()) continue;

				if (bPeak)
				{
					if (plotX->isAsPeak())
					{
						bHasAndTerrain = true;
						break;
					}
				}
				else if (bHill)
				{
					if (plotX->isHills())
					{
						bHasAndTerrain = true;
						break;
					}
				}
				else if (plotX->getTerrainType() == iI)
				{
					bHasAndTerrain = true;
					break;
				}
			}
			if (!bHasAndTerrain)
			{
				return false;
			}
		}
	}
	if (!bValidTerrain && bRequiresTerrain)
	{
		return false;
	}

	// Improvement checks require owned tile
	if (!kBuilding.getPrereqOrImprovements().empty())
	{
		bool bHasValidImprovement = false;
		foreach_(const ImprovementTypes prereqOrImprovement, kBuilding.getPrereqOrImprovements())
		{
			if (algo::any_of(plots(), bind(CvPlot::getImprovementType, _1) == prereqOrImprovement && bind(CvPlot::getTeam, _1) == getTeam()))
			{
				bHasValidImprovement = true;
				break;
			}
		}
		if (!bHasValidImprovement)
		{
			return false;
		}
	}

	bool bRequiresOrFeature = false;
	bool bHasValidFeature = false;
	// Feature checks require owned tile unless strict vicinity off; then, neutral OK too
	for (int iI = 0; iI < GC.getNumFeatureInfos(); iI++)
	{
		if (kBuilding.isPrereqOrFeature(iI))
		{
			bRequiresOrFeature = true;
			foreach_(const CvPlot* plotX, plots())
			{
				if (plotX->getFeatureType() == iI)
				{
					if (plotX->getTeam() == getTeam())
					{
						bHasValidFeature = true;
						break;
					}
					// Features are allowed from neutral territory with Strict Vicinity OFF
					else if (!GC.getGame().isOption(GAMEOPTION_EXP_STRICT_VICINITY) && plotX->getTeam() == NO_TEAM)
					{
						bHasValidFeature = true;
						break;
					}
				}
			}
		}
	}
	if (!bHasValidFeature && bRequiresOrFeature)
	{
		return false;
	}

	return true;
}

void CvCity::changeFreshWater(int iChange)
{
	if (iChange != 0)
	{
		const bool bDidHaveFreshWater = m_iFreshWater > 0;
		m_iFreshWater += iChange;

		if (bDidHaveFreshWater != hasFreshWater())
		{
			algo::for_each(plots(), bind(CvPlot::updateIrrigated, _1));

			updateFreshWaterHealth();
		}
	}
}

bool CvCity::hasFreshWater() const
{
	return m_iFreshWater > 0;
}


bool CvCity::canUpgradeUnit(UnitTypes eUnit) const
{
	PROFILE_FUNC();

	for (int iI = 0; iI < GC.getUnitInfo(eUnit).getNumUnitUpgrades(); iI++)
	{
		const UnitTypes eUpgradeUnit = (UnitTypes)GC.getUnitInfo(eUnit).getUnitUpgrade(iI);

		if (GC.getGame().isUnitMaxedOut(eUpgradeUnit) || GET_PLAYER(getOwner()).isUnitMaxedOut(eUpgradeUnit))
		{ // if the upgrade unit is maxed out, I assume you can construct them, and already have constructed the max
			return true;
		}
	}
	return false;
}

void CvCity::setCivilizationType(int iCiv)
{
	m_iCiv = iCiv;
}

int CvCity::getAdditionalDefenseByBuilding(BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eBuilding);

	int iExtraRate = 0;
	int iExtraBuildingRate = 0;

	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	if (kBuilding.getDefenseModifier() != 0)
	{
		//int iCultureDefense = getNaturalDefense() - getBuildingDefense();
		//iExtraRate += std::max(0, kBuilding.getDefenseModifier() - std::max(0, iCultureDefense));
		iExtraBuildingRate += kBuilding.getDefenseModifier();
	}
	for (int iJ = 0; iJ < GC.getNumBonusInfos(); iJ++)
	{
		if (hasBonus((BonusTypes)iJ))
		{
			iExtraRate += kBuilding.getBonusDefenseChanges(iJ);
		}
	}
	if (kBuilding.getAllCityDefenseModifier() != 0)
	{
		iExtraRate += kBuilding.getAllCityDefenseModifier();
	}

	// If this new building replaces an old one, subtract the old defense rate from the new one.
	for (int iI = 0; iI < kBuilding.getNumReplacedBuilding(); iI++)
	{
		const BuildingTypes eBuildingX = static_cast<BuildingTypes>(kBuilding.getReplacedBuilding(iI));

		if (isActiveBuilding(eBuildingX))
		{
			const CvBuildingInfo& info = GC.getBuildingInfo(eBuildingX);

			iExtraBuildingRate -= info.getDefenseModifier();
			for (int iJ = 0; iJ < GC.getNumBonusInfos(); iJ++)
			{
				if (hasBonus((BonusTypes)iJ))
				{
					iExtraRate -= info.getBonusDefenseChanges(iJ);
				}
			}
			iExtraRate -= info.getAllCityDefenseModifier();
		}
	}

	const int iOldEffectiveBuildingRate = std::max(getBuildingDefense(), getNaturalDefense());
	const int iNewEffectiveBuildingRate = std::max(getBuildingDefense() + iExtraBuildingRate, getNaturalDefense());

	return iExtraRate + iNewEffectiveBuildingRate - iOldEffectiveBuildingRate;
}


void CvCity::checkBuildings(bool bAlertOwner)
{
	PROFILE_FUNC();
	const CvPlayer& player = GET_PLAYER(getOwner());
	bAlertOwner = bAlertOwner && !player.isModderOption(MODDEROPTION_IGNORE_DISABLED_ALERTS);

	const bool bBannedNonStateReligions = player.hasBannedNonStateReligions() && getReligionCount() > 0;
	ReligionTypes eReligion = NO_RELIGION;
	ReligionTypes eStateReligion = NO_RELIGION;
	if (bBannedNonStateReligions)
	{
		eStateReligion = player.getStateReligion();
	}

	foreach_(const BuildingTypes eType, getHasBuildings())
	{
		bool bIsReplaced = false;
		bool bDisableBuilding = false;

		bool bMissingBonus = false;
		bool bMissingFreshWater = false;
		bool bRequiresCivics = false;
		bool bRequiresWar = false;
		bool bRequiresPower = false;
		bool bRequiresPopulation = false;
		bool bPopulationTooHigh = false;
		bool bReligionBanned = false;

		const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eType);

		while (true) // This loop will never actually loop.
		{
			/* Check if disabled through replacement */
			for (int iJ = 0; iJ < kBuilding.getNumReplacementBuilding(); ++iJ)
			{
				// Toffer, we get into some ugly causality territory if we restrict this to isActiveBuilding.
				//	Better to say that if the replacement gets disabled, then that means all it replaced also went disabled (which is what they are anyway).
				//	If we try to enable the replaced when the replacement goes disabled,
				//	then the order we check the replaced in is important as many of the replaced are replacements for the other replaced.
				if (hasBuilding((BuildingTypes)kBuilding.getReplacementBuilding(iJ)))
				{
					bIsReplaced = true;
					break;
				}
			}
			if (bIsReplaced) break;

			/* Check for Appropriate Resources */
			{
				bool bNeedsBonus = false;
				bool bHasBonus = false;
				bool bHasRawVicinityBonus = false;
				bool bNeedsRawVicinityBonus = false;
				bool bNeedsVicinityBonus = false;
				bool bHasVicinityBonus = false;
				if (kBuilding.getPrereqVicinityBonus() != NO_BONUS)
				{
					bNeedsVicinityBonus = true;
					bHasVicinityBonus = hasVicinityBonus((BonusTypes)kBuilding.getPrereqVicinityBonus());
				}
				if (kBuilding.getPrereqRawVicinityBonus() != NO_BONUS)
				{
					bNeedsRawVicinityBonus = true;
					bHasRawVicinityBonus = hasRawVicinityBonus((BonusTypes)kBuilding.getPrereqRawVicinityBonus());
				}

				if (!bNeedsVicinityBonus || bHasVicinityBonus)
				{
					bool bHasORVicinityBonus = false;
					bool bNeedsORVicinityBonus = false;

					foreach_(const BonusTypes ePrereqBonus, kBuilding.getPrereqOrVicinityBonuses())
					{
						bNeedsORVicinityBonus = true;
						if (hasVicinityBonus(ePrereqBonus))
						{
							bHasORVicinityBonus = true;
							break;
						}
					}

					bNeedsVicinityBonus |= bNeedsORVicinityBonus;
					if (bNeedsORVicinityBonus)
					{
						bHasVicinityBonus = bHasORVicinityBonus;
					}
				}

				if (!bNeedsRawVicinityBonus || bHasRawVicinityBonus)
				{
					bool bHasORRawVicinityBonus = false;
					bool bNeedsORRawVicinityBonus = false;

					foreach_(BonusTypes bonus, kBuilding.getPrereqOrRawVicinityBonuses())
					{
						bNeedsORRawVicinityBonus = true;
						if (hasRawVicinityBonus(bonus))
						{
							bHasORRawVicinityBonus = true;
							break;
						}
					}

					bNeedsRawVicinityBonus |= bNeedsORRawVicinityBonus;
					if (bNeedsORRawVicinityBonus)
					{
						bHasRawVicinityBonus = bHasORRawVicinityBonus;
					}
				}

				// We lack the nessecary resource, turn off the building
				if (bNeedsRawVicinityBonus && !bHasRawVicinityBonus
				|| bNeedsVicinityBonus && !bHasVicinityBonus)
				{
					bDisableBuilding = true;
					bMissingBonus = true;
					break;
				}

				// Check trade-available resource requirements
				if (kBuilding.getPrereqAndBonus() != NO_BONUS)
				{
					bNeedsBonus = true;
					bHasBonus = hasBonus((BonusTypes)kBuilding.getPrereqAndBonus());
				}
				if (!bNeedsBonus || bHasBonus)
				{
					bool bHasORBonus = false;
					bool bNeedsORBonus = false;

					foreach_(const BonusTypes ePrereqBonus, kBuilding.getPrereqOrBonuses())
					{
						bNeedsORBonus = true;
						if (hasBonus(ePrereqBonus))
						{
							bHasORBonus = true;
							break;
						}
					}

					bNeedsBonus |= bNeedsORBonus;
					if (bNeedsORBonus)
					{
						bHasBonus = bHasORBonus;
					}
				}

				//we lack the nessecary resource, turn off the building
				if (bNeedsBonus && !bHasBonus)
				{
					bDisableBuilding = true;
					bMissingBonus = true;
					break;
				}
			}

			/* Check War Conditions */
			if (kBuilding.isPrereqWar() && !GET_TEAM(getTeam()).isAtWar())
			{
				bDisableBuilding = true;
				bRequiresWar = true;
				break;
			}

			/* Check Civic Requirements */
			if (kBuilding.isRequiresActiveCivics() && !player.hasValidCivics(eType))
			{
				bDisableBuilding = true;
				bRequiresCivics = true;
				break;
			}

			/*Check Elecricity Requirements */
			if (kBuilding.isPrereqPower() && !isPower())
			{
				bDisableBuilding = true;
				bRequiresPower = true;
				break;
			}

			/* Check fresh water */
			if (kBuilding.isFreshWater() && !plot()->isFreshWater() && !hasFreshWater())
			{
				bDisableBuilding = true;
				bMissingFreshWater = true;
				break;
			}

			{
				/* Check The Employed Population */
				const int iPrereqPopulation = (
					std::max(
						kBuilding.getPrereqPopulation(),
						1 + getNumPopulationEmployed() + kBuilding.getNumPopulationEmployed()
					)
				);
				if (iPrereqPopulation > 1 && getPopulation() < iPrereqPopulation)
				{
					bDisableBuilding = true;
					bRequiresPopulation = true;
					break;
				}
			}

			/* Check max population requirement */
			if (kBuilding.getMaxPopAllowed() > 0 && kBuilding.getMaxPopAllowed() < getPopulation())
			{
				bDisableBuilding = true;
				bPopulationTooHigh = true;
				break;
			}

			/* Check banned non-state religion */
			if (bBannedNonStateReligions)
			{
				eReligion = (ReligionTypes)kBuilding.getReligionType();
				if (eReligion == NO_RELIGION)
				{
					eReligion = (ReligionTypes)kBuilding.getPrereqReligion();
				}
				if (eReligion != NO_RELIGION && eStateReligion != eReligion)
				{
					bDisableBuilding = true;
					bReligionBanned = true;
					break;
				}
			}
			break; // The while loop is not supposed to loop.
		}
		if (bIsReplaced) continue; // Replacement disabling/enabling is handled in changeHasBuilding(...).

		if (isDisabledBuilding(eType))
		{
			if (!bDisableBuilding)
			{
				setDisabledBuilding(eType, false);

				if (bAlertOwner)
				{
					AddDLLMessage(
						getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_CITY_RESTORED_BUILDINGS", getNameKey(), kBuilding.getDescription()),
						NULL, MESSAGE_TYPE_MINOR_EVENT, kBuilding.getButton(), GC.getCOLOR_WHITE(), getX(), getY(), true, true
					);
				}
			}
		}
		else if (bDisableBuilding)
		{
			setDisabledBuilding(eType, true);

			if (bAlertOwner)
			{
				// Toffer - Should combine the text messages if there's more than one reason.
				CvWString szBuffer;

				if (bMissingBonus)
				{
					szBuffer = gDLL->getText("TXT_KEY_CITY_REMOVED_BUILDINGS_RESOURCES", kBuilding.getDescription(), getNameKey(), kBuilding.getDescription());
				}
				else if (bRequiresWar)
					szBuffer = gDLL->getText("TXT_KEY_REMOVED_BUILDINGS_WARTIME", kBuilding.getDescription(), getNameKey());
				else if (bRequiresCivics)
					szBuffer = gDLL->getText("TXT_KEY_CITY_REMOVED_BUILDINGS_CIVICS", kBuilding.getDescription(), getNameKey(), kBuilding.getDescription());
				else if (bRequiresPower)
					szBuffer = gDLL->getText("TXT_KEY_CITY_REMOVED_BUILDINGS_POWER", kBuilding.getDescription(), getNameKey());
				else if (bMissingFreshWater)
					szBuffer = gDLL->getText("TXT_KEY_CITY_REMOVED_FRESH_WATER", kBuilding.getDescription(), getNameKey());
				else if (bRequiresPopulation)
					szBuffer = gDLL->getText("TXT_KEY_CITY_REMOVED_BUILDINGS_POPULATION", kBuilding.getDescription(), getNameKey());
				else if (bPopulationTooHigh)
					szBuffer = gDLL->getText("TXT_KEY_CITY_REMOVED_MAX_POPULATION", kBuilding.getDescription(), getNameKey());
				else if (bReligionBanned)
					szBuffer = gDLL->getText("TXT_KEY_CITY_RELIGIOUSLY_DISABLED_COMPLETELY_BUILDINGS", getNameKey(), GC.getReligionInfo(eReligion).getDescription(), kBuilding.getDescription());
				else
				{
					FErrorMsg("error");
					szBuffer = gDLL->getText("TXT_KEY_CITY_REMOVED_BUILDINGS_RESOURCES", kBuilding.getDescription(), getNameKey(), kBuilding.getDescription());
				}
				AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, kBuilding.getButton(), GC.getCOLOR_WARNING_TEXT(), getX(), getY(), true, true);
			}
		}
	}
}



int CvCity::calculateCorporationHealth() const
{
	PROFILE_EXTRA_FUNC();
	int iHealth = 0;

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (isActiveCorporation((CorporationTypes)iI))
		{
			iHealth += GC.getCorporationInfo((CorporationTypes)iI).getHealth();
		}
	}
	return iHealth;
}

int CvCity::calculateCorporationHappiness() const
{
	PROFILE_EXTRA_FUNC();
	int iHappiness = 0;

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (isActiveCorporation((CorporationTypes)iI))
		{
			iHappiness += GC.getCorporationInfo((CorporationTypes)iI).getHappiness();
		}
	}
	return iHappiness;
}


int CvCity::getLineOfSight() const
{
	return m_iLineOfSight;
}

void CvCity::changeLineOfSight(int iChange)
{
	m_iLineOfSight += iChange;
}


BuildTypes CvCity::findChopBuild(FeatureTypes eFeature) const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		const CvBuildInfo& kBuild = GC.getBuildInfo((BuildTypes)iI);
		if (kBuild.getImprovement() == NO_IMPROVEMENT)
		{
			if (kBuild.isFeatureRemove(eFeature) &&
				kBuild.getFeatureProduction(eFeature) != 0 &&
				GET_TEAM(getTeam()).isHasTech(kBuild.getTechPrereq()) &&
				(kBuild.getObsoleteTech() == NO_TECH || !GET_TEAM(getTeam()).isHasTech(kBuild.getObsoleteTech())))
			{
				return (BuildTypes)iI;
			}
		}
	}

	return NO_BUILD;
}


int CvCity::calculateBonusCommerceRateModifier(CommerceTypes eIndex) const
{
	PROFILE_EXTRA_FUNC();
	int iMod = getBonusCommerceRateModifier(eIndex);

	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		if (hasBonus((BonusTypes)iI))
		{
			iMod += GET_PLAYER(getOwner()).getBonusCommerceModifier((BonusTypes)iI, eIndex);//from Player level
		}
	}
	return iMod;
}

static bool bonusAvailableFromBuildings(BonusTypes eBonus)
{
	PROFILE_EXTRA_FUNC();
	static bool* bBonusAvailability = NULL;

	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eBonus);

	if (bBonusAvailability == NULL)
	{
		CvXMLLoadUtility::InitList<bool>(&bBonusAvailability, GC.getNumBonusInfos(), false);

		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			foreach_(const CvBuildingInfo* pBuilding, GC.getBuildingInfos())
			{
				if (pBuilding->getFreeBonuses().hasValue((BonusTypes)iI))
				{
					bBonusAvailability[iI] = true;
					break;
				}
			}
		}
	}

	return bBonusAvailability[eBonus];
}

void CvCity::clearVicinityBonusCache(BonusTypes eBonus)
{
	if (m_pabHasVicinityBonusCached != NULL)
	{
		m_pabHasVicinityBonusCached[eBonus] = false;
	}
}

bool CvCity::hasVicinityBonus(BonusTypes eBonus) const
{
	PROFILE_FUNC();
	if (!GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
	{
		if (m_pabHasVicinityBonusCached == NULL)
		{
			m_pabHasVicinityBonusCached = new bool[GC.getNumBonusInfos()];
			SAFE_DELETE_ARRAY(m_pabHasVicinityBonus);
			m_pabHasVicinityBonus = new bool[GC.getNumBonusInfos()];

			for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
			{
				m_pabHasVicinityBonusCached[iI] = false;
			}
		}

		if (m_pabHasVicinityBonusCached[eBonus])
		{
			return m_pabHasVicinityBonus[eBonus];
		}
	}

	bool bResult = false;

	//No sense in checking...
	if (hasBonus(eBonus))
	{
		if (plot()->getBonusType() == eBonus)
		{
			bResult = true;
		}

		if (!bResult)
		{
			for (int iI = 0; iI < getNumCityPlots(); iI++)
			{
				CvPlot* pLoopPlot = plotCity(getX(), getY(), iI);
				if (pLoopPlot != NULL
				&& pLoopPlot->getBonusType() == eBonus
				&& pLoopPlot->getOwner() == getOwner()
				&& pLoopPlot->isHasValidBonus()
				&& pLoopPlot->isConnectedTo(this))
				{
					bResult = true;
					break;
				}
			}
		}

		if (!bResult && bonusAvailableFromBuildings(eBonus))
		{
			for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
			{
				if (GC.getBuildingInfo((BuildingTypes)iI).getFreeBonuses().hasValue(eBonus) && isActiveBuilding((BuildingTypes)iI))
				{
					bResult = true;
					break;
				}
			}
		}
	}

	if (!GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
	{
		m_pabHasVicinityBonus[eBonus] = bResult;
		m_pabHasVicinityBonusCached[eBonus] = true;
	}

	return bResult;
}

void CvCity::clearRawVicinityBonusCache(BonusTypes eBonus)
{
	if (m_pabHasRawVicinityBonusCached != NULL)
	{
		m_pabHasRawVicinityBonusCached[eBonus] = false;
	}
}

bool CvCity::hasRawVicinityBonus(BonusTypes eBonus) const
{
	PROFILE_FUNC();

	const int numBonusInfos = GC.getNumBonusInfos();
	const int numBuildingInfos = GC.getNumBuildingInfos();

	// --- Cache check (single-threaded only) ---
	if (!GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
	{
		if (m_pabHasRawVicinityBonusCached == NULL)
		{
			m_pabHasRawVicinityBonusCached = new bool[numBonusInfos]();
			SAFE_DELETE_ARRAY(m_pabHasRawVicinityBonus);
			m_pabHasRawVicinityBonus = new bool[numBonusInfos]();
		}

		if (m_pabHasRawVicinityBonusCached[eBonus])
		{
			return m_pabHasRawVicinityBonus[eBonus];
		}
	}

	// --- Direct city plot check ---
	if (plot()->getBonusType() == eBonus)
	{
		if (!GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
		{
			m_pabHasRawVicinityBonus[eBonus] = true;
			m_pabHasRawVicinityBonusCached[eBonus] = true;
		}
		return true;
	}

	// --- City plots check ---
	for (int iI = 0, n = getNumCityPlots(); iI < n; ++iI)
	{
		CvPlot* pLoopPlot = plotCity(getX(), getY(), iI);
		if (pLoopPlot != NULL
			&& pLoopPlot->getBonusType() == eBonus
			&& pLoopPlot->getOwner() == getOwner())
		{
			if (!GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
			{
				m_pabHasRawVicinityBonus[eBonus] = true;
				m_pabHasRawVicinityBonusCached[eBonus] = true;
			}
			return true;
		}
	}

	// --- Building-provided bonus check ---
	if (bonusAvailableFromBuildings(eBonus))
	{
		for (int iI = 0; iI < numBuildingInfos; ++iI)
		{
			const CvBuildingInfo& kBuilding = GC.getBuildingInfo((BuildingTypes)iI);
			if (kBuilding.getFreeBonuses().hasValue(eBonus) && isActiveBuilding((BuildingTypes)iI))
			{
				if (!GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
				{
					m_pabHasRawVicinityBonus[eBonus] = true;
					m_pabHasRawVicinityBonusCached[eBonus] = true;
				}
				return true;
			}
		}
	}

	// --- Cache result if applicable ---
	if (!GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
	{
		m_pabHasRawVicinityBonus[eBonus] = false;
		m_pabHasRawVicinityBonusCached[eBonus] = true;
	}

	return false;
}

bool CvCity::isDevelopingCity() const
{
	return
	(
		getPopulation() < 3 && !isCapital()
		|| // Pop is less than half your average city pop value.
		getPopulation() < GET_PLAYER(getOwner()).getTotalPopulation() / (2*GET_PLAYER(getOwner()).getNumCities())
	);
}

void CvCity::doVicinityBonus()
{
	PROFILE_FUNC();

	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		//	Clear the cache each turn before performing this calculation
		clearVicinityBonusCache((BonusTypes)iI);
		clearRawVicinityBonusCache((BonusTypes)iI);

		int iChange = 0;
		const bool bHadVicinityBonus = hadVicinityBonus((BonusTypes)iI);
		const bool bHasVicinityBonus = hasVicinityBonus((BonusTypes)iI);
		if (bHadVicinityBonus && !bHasVicinityBonus)
		{
			iChange = -1;
		}
		else if (bHasVicinityBonus && !bHadVicinityBonus)
		{
			iChange = 1;
		}
		if (iChange != 0)
		{
			for (int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
			{
				const CvBuildingInfo& kBuilding = GC.getBuildingInfo((BuildingTypes)iJ);

				if (kBuilding.getVicinityBonusYieldChanges(NO_BONUS, NO_YIELD) != 0 && isActiveBuilding((BuildingTypes)iJ))
				{
					for (int iK = 0; iK < NUM_YIELD_TYPES; iK++)
					{
						const int iYieldChange = kBuilding.getVicinityBonusYieldChanges((BonusTypes)iI, iK);

						if (iYieldChange != 0)
						{
							updateYieldRate((BuildingTypes)iJ, (YieldTypes)iK, (getBuildingYieldChange((BuildingTypes)iJ, (YieldTypes)iK) + (iYieldChange * iChange)));
						}
					}
				}
			}
		}
	}
}

void CvCity::setDisabledBuilding(const BuildingTypes eIndex, const bool bNewValue, const bool bProcess)
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);

	std::vector<short>::iterator itr = find(m_vDisabledBuildings.begin(), m_vDisabledBuildings.end(), (short)eIndex);

	if (bNewValue)
	{
		if (itr == m_vDisabledBuildings.end())
		{
			if (bProcess)
			{
				if (isReligiouslyLimitedBuilding(eIndex))
				{
					// Needs to be fully processed in before we disable it.
					setReligiouslyLimitedBuilding(eIndex, false);
				}
				processBuilding(eIndex, -1);
			}
			m_vDisabledBuildings.push_back(eIndex);
		}
	}
	else if (itr != m_vDisabledBuildings.end())
	{
		if (bProcess)
		{
			processBuilding(eIndex, 1);
		}
		m_vDisabledBuildings.erase(itr);
	}
}

bool CvCity::isDisabledBuilding(const short iIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), iIndex);
	return algo::any_of_equal(m_vDisabledBuildings, iIndex);
}


// @SAVEBREAK - Toffer - Change m_pabReligiouslyDisabledBuilding to vector containing eIndex elements.
bool CvCity::isReligiouslyLimitedBuilding(BuildingTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	return m_pabReligiouslyDisabledBuilding[eIndex];
}

void CvCity::setReligiouslyLimitedBuilding(BuildingTypes eIndex, bool bNewValue)
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);

	const bool bOldValue = m_pabReligiouslyDisabledBuilding[eIndex];

	ReligionTypes eReligion = (ReligionTypes)GC.getBuildingInfo(eIndex).getReligionType();
	if (eReligion == NO_RELIGION)
	{
		eReligion = (ReligionTypes)GC.getBuildingInfo(eIndex).getPrereqReligion();
	}
	FASSERT_BOUNDS(0, GC.getNumReligionInfos(), eReligion);

	if (bOldValue != bNewValue)
	{
		processBuilding(eIndex, bNewValue ? -1 : 1, true);
		m_pabReligiouslyDisabledBuilding[eIndex] = bNewValue;

		if (!bNewValue)
		{
			if (/*!GET_PLAYER(getOwner()).isModderOption(MODDEROPTION_IGNORE_DISABLED_ALERTS) &&*/ GET_PLAYER(getOwner()).isHumanPlayer())
			{

				const CvWString szBuffer = gDLL->getText("TXT_KEY_CITY_RELIGIOUSLY_RESTORED_BUILDINGS", getNameKey(), GC.getReligionInfo(eReligion).getDescription(), GC.getBuildingInfo(eIndex).getDescription());
				AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, GC.getBuildingInfo(eIndex).getButton(), GC.getCOLOR_WHITE(), getX(), getY(), true, true);
			}
		}
		else if (/*!GET_PLAYER(getOwner()).isModderOption(MODDEROPTION_IGNORE_DISABLED_ALERTS) && */GET_PLAYER(getOwner()).isHumanPlayer())
		{

			const CvWString szBuffer = gDLL->getText("TXT_KEY_CITY_RELIGIOUSLY_DISABLED_BUILDINGS", getNameKey(), GC.getReligionInfo(eReligion).getDescription(), GC.getBuildingInfo(eIndex).getDescription());
			AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, GC.getBuildingInfo(eIndex).getButton(), GC.getCOLOR_WARNING_TEXT(), getX(), getY(), true, true);
		}
	}
}

int CvCity::getUnitCombatExtraStrength(UnitCombatTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eIndex);
	return m_paiUnitCombatExtraStrength[eIndex];
}


void CvCity::changeUnitCombatExtraStrength(UnitCombatTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eIndex);
	m_paiUnitCombatExtraStrength[eIndex] += iChange;
}

bool CvCity::isZoneOfControl() const
{
	return (m_iZoCCount > 0);
}

void CvCity::changeZoCCount(short iChange)
{
	if (iChange != 0)
	{
		m_iZoCCount = std::max(0, (m_iZoCCount + iChange));
	}
}

int CvCity::getAdjacentDamagePercent() const
{
	return m_iAdjacentDamagePercent;
}

void CvCity::changeAdjacentDamagePercent(int iChange)
{
	m_iAdjacentDamagePercent += iChange;
}

int CvCity::getWorkableRadiusOverride() const
{
	return m_iWorkableRadiusOverride;
}

void CvCity::setWorkableRadiusOverride(int iNewVal)
{
	m_iWorkableRadiusOverride = iNewVal;
}

bool CvCity::isProtectedCulture() const
{
	return getProtectedCultureCount() > 0;
}

int CvCity::getProtectedCultureCount() const
{
	return m_iProtectedCultureCount;
}

void CvCity::changeProtectedCultureCount(int iChange)
{
	m_iProtectedCultureCount += iChange;
}

int CvCity::getNumUnitFullHeal() const
{
	return m_iNumUnitFullHeal;
}

void CvCity::changeNumUnitFullHeal(int iChange)
{
	m_iNumUnitFullHeal += iChange;
}


void CvCity::doAttack()
{
	PROFILE_FUNC();

	if (getAdjacentDamagePercent() > 0)
	{
		if (GET_TEAM(getTeam()).isAtWar(true))
		{
			bool abInformPlayer[MAX_PLAYERS];
			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				abInformPlayer[iI] = false;
			}
			foreach_(const CvPlot* pAdjacentPlot, plot()->adjacent())
			{
				foreach_(CvUnit* pLoopUnit, pAdjacentPlot->units() | filtered(CvUnit::fn::getTeam() != getTeam()))
				{
					if (GET_TEAM(getTeam()).isAtWar(pLoopUnit->getTeam()))
					{
						//	Koshling - changed city defenses to have a 1-in-4 chance of damaging each unit each turn
						if (pLoopUnit->baseCombatStr() && GC.getGame().getSorenRandNum(4, "City adjacent damage") == 0)
						{
							int iDamage = pLoopUnit->getHP();
							iDamage *= getAdjacentDamagePercent();
							iDamage /= 100;

							pLoopUnit->changeDamage(iDamage, getOwner());
							if (!abInformPlayer[pLoopUnit->getOwner()])
							{
								abInformPlayer[pLoopUnit->getOwner()] = true;
								CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_UNITS_DAMAGED", getNameKey());
								AddDLLMessage(pLoopUnit->getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_INFO, pLoopUnit->getButton(), GC.getCOLOR_RED(), pLoopUnit->getX(), pLoopUnit->getY(), true, true);
							}
						}
					}
				}
			}
		}
	}
}

void CvCity::doHeal()
{
	PROFILE_FUNC();

	if (getNumUnitFullHeal() > 0)
	{
		UnitVector damagedUnits;
		// Get the damaged units on our team
		algo::push_back(
			damagedUnits,
			plot()->units() | filtered(CvUnit::fn::getTeam() == getTeam() && CvUnit::fn::getDamage() > 0)
		);
		// Randomize them
		algo::random_shuffle(damagedUnits, CvGame::SorenRand("Unit Full Heals"));
		// Heal as many as we are able
		const int maxHeals = std::min<int>(getNumUnitFullHeal(), damagedUnits.size());
		foreach_(CvUnit * unit, damagedUnits | sliced(0, maxHeals))
		{
			unit->setDamage(0, getOwner(), false);
		}
	}
}

void CvCity::doCorporation()
{
	PROFILE_FUNC();

	if (!GC.getGame().isOption(GAMEOPTION_ADVANCED_REALISTIC_CORPORATIONS))
	{
		return;
	}
	const PlayerTypes ePlayer = getOwner();
	CvPlayer& kOwner = GET_PLAYER(ePlayer);

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (GC.getGame().getHeadquarters((CorporationTypes)iI) == NULL)
		{
			continue;
		}
		if (!isHasCorporation((CorporationTypes)iI) && GC.getGame().canEverSpread((CorporationTypes)iI))
		{
			if (kOwner.isNoCorporations() || kOwner.isNoForeignCorporations() && GC.getGame().getHeadquarters((CorporationTypes)iI)->getOwner() != ePlayer)
			{
				continue;
			}
			int iRandThreshold = 0;
			for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
			{
				const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iJ);
				if (kPlayer.isAlive())
				{
					foreach_(const CvCity* pLoopCity, kPlayer.cities())
					{
						if (pLoopCity->isConnectedTo(this))
						{
							int iSpread = pLoopCity->getCorporationInfluence((CorporationTypes)iI);

							iSpread *= GC.getCorporationInfo((CorporationTypes)iI).getSpread();

							iSpread /= 100;

							if (iJ != ePlayer && GET_TEAM(kPlayer.getTeam()).isFreeTradeAgreement(kOwner.getTeam()))
							{
								iSpread *= (100 + GC.getFREE_TRADE_CORPORATION_SPREAD_MOD());
								iSpread /= 100;
							}

							if (iSpread > 0)
							{
								iSpread /= std::max(1, (((GC.getCORPORATION_SPREAD_DISTANCE_DIVISOR() * plotDistance(getX(), getY(), pLoopCity->getX(), pLoopCity->getY())) / GC.getMap().maxPlotDistance()) - 5));

								iRandThreshold = std::max(iRandThreshold, iSpread);
							}
						}
					}
				}
			}
			iRandThreshold *= kOwner.getCorporationSpreadModifier() + 100;
			iRandThreshold /= 100;
			iRandThreshold *= kOwner.getCorporationInfluence((CorporationTypes)iI);
			iRandThreshold /= 100;
			iRandThreshold /= 1 + getCorporationCount() / 2;

			logBBAI("  City (%S) Has Rand Threshold of %d for Corporation %S", getName().GetCString(), iRandThreshold, GC.getCorporationInfo((CorporationTypes)iI).getDescription());
			int iRand = GC.getCORPORATION_SPREAD_RAND();
			iRand *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
			iRand /= 100;
			iRand = GC.getGame().getSorenRandNum(iRand, "Corporation Spread");
			if (iRand < iRandThreshold)
			{
				//Remove Hostile Corporations
				for (int iJ = 0; iJ < GC.getNumCorporationInfos(); iJ++)
				{
					if (iI != iJ
					&& GC.getGame().isCompetingCorporation((CorporationTypes)iJ, (CorporationTypes)iI)
					&& isActiveCorporation((CorporationTypes)iJ))
					{
						setHasCorporation((CorporationTypes)iJ, false, false, false);

						AddDLLMessage(
							ePlayer, false, GC.getEVENT_MESSAGE_TIME(),
							gDLL->getText(
								"TXT_KEY_MISC_CORPORATION_HOSTILE_TAKEOVER",
								GC.getCorporationInfo((CorporationTypes)iJ).getTextKeyWide(),
								GC.getCorporationInfo((CorporationTypes)iI).getTextKeyWide(),
								getNameKey()
							),
							GC.getCorporationInfo((CorporationTypes)iJ).getSound(),
							MESSAGE_TYPE_MAJOR_EVENT,
							GC.getCorporationInfo((CorporationTypes)iJ).getButton(),
							GC.getCOLOR_WHITE(), getX(), getY(), false, false
						);
					}
				}
				setHasCorporation((CorporationTypes)iI, true, true, false);
				break;
			}
		}
		// Decay
		else if (this != GC.getGame().getHeadquarters((CorporationTypes)iI))
		{
			// TODO: Should HQ ever relocate?
			const int iDiff =
			(
				GC.getGame().getAverageCorporationInfluence(this, (CorporationTypes)iI)
				-
				GC.getCorporationInfo((CorporationTypes)iI).getSpread()
				* getCorporationInfluence((CorporationTypes)iI)
				* kOwner.getCorporationInfluence((CorporationTypes)iI)
				/
				10000
			);
			// Our influence is lower than average
			if (iDiff > 0)
			{
				const int iRand =
				(
					GC.getCORPORATION_SPREAD_RAND()
					* GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent()
					/ 100
				);
				if (GC.getGame().getSorenRandNum(iRand, "Corporation Decay") < iDiff)
				{
					setHasCorporation((CorporationTypes)iI, false, true, false);
					break;
				}
			}
		}
	}
}

// TODO: cache this variable
int CvCity::getCorporationInfluence(CorporationTypes eCorporation) const
{
	PROFILE_EXTRA_FUNC();
	int iInfluence = 100;

	int iBonusesConsumed = 0;
	int iNumAvailBonuses = 0;
	//Influence scales based on the number of resources a corporation consumes
	foreach_(const BonusTypes eBonus, GC.getCorporationInfo(eCorporation).getPrereqBonuses())
	{
		iBonusesConsumed++;
		iNumAvailBonuses += getNumBonuses(eBonus);
	}

	if (iNumAvailBonuses > 0)
	{
		iInfluence += iNumAvailBonuses;
	}
	else
	{
		if (iNumAvailBonuses == 0 && iBonusesConsumed > 0)
		{
			return 0;
		}
	}

	if (iBonusesConsumed > 0)
	{
		foreach_(const BonusTypes eBonus, GC.getCorporationInfo(eCorporation).getPrereqBonuses())
		{
			if (hasBonus(eBonus))
			{
				iInfluence += (GC.getCORPORATION_RESOURCE_BASE_INFLUENCE() / iBonusesConsumed);
			}
		}
	}

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (iI != eCorporation)
		{
			if (GC.getGame().isCompetingCorporation(eCorporation, (CorporationTypes)iI))
			{
				if (isActiveCorporation((CorporationTypes)iI))
				{
					iInfluence /= 10;
				}
			}
		}
	}
	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (iI != eCorporation)
		{
			if (GC.getGame().isCompetingCorporation(eCorporation, (CorporationTypes)iI))
			{
				if (isActiveCorporation((CorporationTypes)iI))
				{
					if (GC.getGame().getHeadquarters((CorporationTypes)iI) == this)
					{
						return 0;
					}
				}
			}
		}
	}
	int iAveragePopulation = GC.getGame().getTotalPopulation();
	iAveragePopulation /= std::max(1, GC.getGame().getNumCivCities());
	if (iAveragePopulation > 0)
	{
		iInfluence *= getPopulation();
		iInfluence /= iAveragePopulation;
	}
	return iInfluence;
}

int64_t CvCity::calcCorporateMaintenance() const
{
	PROFILE_FUNC();

	int64_t iTaxes = 0;

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (isActiveCorporation((CorporationTypes)iI) && GET_PLAYER(getOwner()).isActiveCorporation((CorporationTypes)iI))
		{
			const CorporationTypes eCorporation = (CorporationTypes)iI;

			for (int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; ++iCommerce)
			{
				iTaxes += 100 * GC.getCorporationInfo(eCorporation).getHeadquarterCommerce(iCommerce);
			}

			int iNumBonuses = 0;
			foreach_(const BonusTypes eBonus, GC.getCorporationInfo(eCorporation).getPrereqBonuses())
			{
				iNumBonuses += getNumBonuses(eBonus);
			}

			iTaxes +=
			(
				GC.getCorporationInfo(eCorporation).getMaintenance() * iNumBonuses *
				GC.getWorldInfo(GC.getMap().getWorldSize()).getCorporationMaintenancePercent()
				/ 100
			);
			const int iAveragePopulation = GC.getGame().getTotalPopulation() / std::max(1, GC.getGame().getNumCivCities());

			if (iAveragePopulation > 0)
			{
				iTaxes *= getPopulation();
				iTaxes /= iAveragePopulation;
			}
			iTaxes = getModifiedIntValue64(iTaxes, GET_PLAYER(getOwner()).getCorporationMaintenanceModifier() + GET_TEAM(getTeam()).getCorporationMaintenanceModifier());

			iTaxes *= GC.getHandicapInfo(getHandicapType()).getCorporationMaintenancePercent();
			iTaxes /= 100;
		}
	}
	FASSERT_NOT_NEGATIVE(iTaxes);

	return iTaxes / 100;
}

int CvCity::getDisabledPowerTimer() const
{
	return m_iDisabledPowerTimer;
}

void CvCity::changeDisabledPowerTimer(int iChange)
{
	m_iDisabledPowerTimer += iChange;
}

int CvCity::getWarWearinessTimer() const
{
	return m_iWarWearinessTimer;
}

void CvCity::changeWarWearinessTimer(int iChange)
{
	m_iWarWearinessTimer += iChange;
}

void CvCity::doDisabledPower()
{
	if (getDisabledPowerTimer() > 0)
	{
		changeDisabledPowerTimer(-1);
		if (getDisabledPowerTimer() == 0)
		{

			CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_POWER_RESTORED", getNameKey());
			AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_MINOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), GC.getCOLOR_GREEN(), getX(), getY(), false, false);
		}
	}
}

void CvCity::doWarWeariness()
{
	if (getWarWearinessTimer() > 0)
	{
		changeWarWearinessTimer(-20);
	}
	if (getEventAnger() > 0)
	{
		int iTurnCheck = 10;
		iTurnCheck *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
		iTurnCheck /= 100;
		if (GC.getGame().getElapsedGameTurns() % iTurnCheck == 0)
		{
			changeEventAnger(-1);
		}
	}
}

int CvCity::getEventAnger() const
{
	return m_iEventAnger;
}

void CvCity::changeEventAnger(int iChange)
{
	if (iChange != 0)
	{
		m_iEventAnger += iChange;
		FASSERT_NOT_NEGATIVE(getEventAnger());

		AI_setAssignWorkDirty(true);

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			setInfoDirty(true);
		}
	}
}

int CvCity::getNonHolyReligionCount() const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if (isHasReligion((ReligionTypes)iI) && !isHolyCity((ReligionTypes)iI))
		{
			iCount++;
		}
	}
	return iCount;
}

void CvCity::calculateExtraTradeRouteProfit(int iExtra, int*& aiTradeYields) const
{
	PROFILE_FUNC();

	const int iMaxTradeRoutes = getMaxTradeRoutes();

	std::vector<int> paiBestValue(iMaxTradeRoutes, 0);
	std::vector<IDInfo> paTradeCities(iMaxTradeRoutes, IDInfo());

	if (!isDisorder() && !isPlundered())
	{
		const int iTradeRoutes = std::min(getTradeRoutes() + std::max(0, iExtra), iMaxTradeRoutes);

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER(getOwner()).canHaveTradeRoutesWith((PlayerTypes)iI))
			{
				foreach_(CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
				{
					if (pLoopCity != this
					&& (!pLoopCity->isTradeRoute(getOwner()) || getTeam() == GET_PLAYER((PlayerTypes)iI).getTeam())
					&& (pLoopCity->plotGroup(getOwner()) == plotGroup(getOwner()) || GC.getIGNORE_PLOT_GROUP_FOR_TRADE_ROUTES()))
					{
						const int iValue = calculateTradeProfitTimes100(pLoopCity);

						for (int iJ = 0; iJ < iTradeRoutes; iJ++)
						{
							if (iValue > paiBestValue[iJ])
							{
								for (int iK = (iTradeRoutes - 1); iK > iJ; iK--)
								{
									paiBestValue[iK] = paiBestValue[(iK - 1)];
									paTradeCities[iK] = paTradeCities[(iK - 1)];
								}

								paiBestValue[iJ] = iValue;
								paTradeCities[iJ] = pLoopCity->getIDInfo();

								break;
							}
						}
					}
				}
			}
		}
	}

	int iTradeProfit = 0;

	for (int iI = 0; iI < std::min(getTradeRoutes(), iMaxTradeRoutes); iI++)
	{
		CvCity* pLoopCity = getCity(paTradeCities[iI]);

		if (pLoopCity != NULL)
		{
			iTradeProfit -= calculateTradeProfitTimes100(pLoopCity);
		}
	}

	for (int iI = 0; iI < getTradeRoutes() + iExtra; iI++)
	{
		CvCity* pLoopCity = getCity(paTradeCities[iI]);

		if (pLoopCity != NULL)
		{
			iTradeProfit += calculateTradeProfitTimes100(pLoopCity);
		}
	}

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		aiTradeYields[iI] = calculateTradeYield(((YieldTypes)iI), iTradeProfit) / 100;
	}
}

int CvCity::getMinimumDefenseLevel() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_REALISTIC_SIEGE))
	{
		return 0;
	}

	if (m_iMinimumDefenseLevel == 0)
	{
		return m_iMinimumDefenseLevel;
	}

	if (getExtraMinDefense() > m_iMinimumDefenseLevel)
	{
		return getExtraMinDefense();
	}

	return m_iMinimumDefenseLevel;
}

void CvCity::setMinimumDefenseLevel(int iNewValue)
{
	m_iMinimumDefenseLevel = iNewValue;
}

int CvCity::getNumPopulationEmployed() const
{
	return m_iNumPopulationEmployed;
}

void CvCity::setNumPopulationEmployed(int iNewValue)
{
	m_iNumPopulationEmployed = iNewValue;
}

void CvCity::changeNumPopulationEmployed(int iChange)
{
	setNumPopulationEmployed(iChange + getNumPopulationEmployed());
	FASSERT_NOT_NEGATIVE(getNumPopulationEmployed());
}

void CvCity::removeWorstCitizenActualEffects(int iNumCitizens, int& iGreatPeopleRate, int& iHappiness, int& iHealthiness, int*& aiYields, int*& aiCommerces) const
{
	PROFILE_FUNC();

	std::vector<SpecialistTypes> paeRemovedSpecailists(iNumCitizens, NO_SPECIALIST);
	std::vector<bool> abRemovedPlots(NUM_CITY_PLOTS, false);

	iGreatPeopleRate = 0;
	iHappiness = 0;
	iHealthiness = 0;
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		aiYields[iI] = 0;
	}
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		aiCommerces[iI] = 0;
	}
	int iGenericSpecialist = GC.getDEFAULT_SPECIALIST();
	int iNumRemoved = 0;
	int iNumSpecialistsRemoved = 0;

	// if we are using more specialists than the free ones we get
	while (getAssignedSpecialistCount() < iNumRemoved && iNumRemoved < iNumCitizens)
	{
		// Does generic 'citizen' specialist exist?
		if (iGenericSpecialist != NO_SPECIALIST
		// Do we have at least one more generic citizen than we are forcing?
		&& getSpecialistCount((SpecialistTypes)iGenericSpecialist) > getForceSpecialistCount((SpecialistTypes)iGenericSpecialist))
		{
			paeRemovedSpecailists[iNumRemoved] = (SpecialistTypes)(iGenericSpecialist);
			iNumRemoved++;
			iNumSpecialistsRemoved++;
		}
	}
	bool bAvoidGrowth = false;
	bool bIgnoreGrowth = false;

	while (iNumRemoved < iNumCitizens)
	{
		int iWorstValue = MAX_INT;
		SpecialistTypes eWorstSpecialist = NO_SPECIALIST;

		// if we are using more specialists than the free ones we get
		if (getAssignedSpecialistCount() < iNumSpecialistsRemoved)
		{
			for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
			{
				if (getSpecialistCount((SpecialistTypes)iI) > getForceSpecialistCount((SpecialistTypes)iI))
				{
					const int iValue = AI_specialistValue(((SpecialistTypes)iI), bAvoidGrowth, /*bRemove*/ true);

					if (iValue < iWorstValue)
					{
						iWorstValue = iValue;
						eWorstSpecialist = (SpecialistTypes)iI;
					}
				}
			}
		}

		// check all the plots we working
		int iWorstPlot = -1;
		for (int plotIdx = SKIP_CITY_HOME_PLOT; plotIdx < NUM_CITY_PLOTS; plotIdx++)
		{
			if (isWorkingPlot(plotIdx) && !abRemovedPlots[plotIdx])
			{
				const CvPlot* pLoopPlot = getCityIndexPlot(plotIdx);

				if (pLoopPlot != NULL)
				{
					const int iValue = AI_plotValue(pLoopPlot, bAvoidGrowth, /*bRemove*/ true, /*bIgnoreFood*/ false, bIgnoreGrowth);

					if (iValue < iWorstValue)
					{
						iWorstValue = iValue;
						eWorstSpecialist = NO_SPECIALIST;
						iWorstPlot = plotIdx;
					}
				}
			}
		}

		if (eWorstSpecialist != NO_SPECIALIST)
		{
			paeRemovedSpecailists[iNumRemoved] = eWorstSpecialist;
			iNumRemoved++;
			iNumSpecialistsRemoved++;
		}
		else if (iWorstPlot != -1)
		{
			abRemovedPlots[iWorstPlot] = true;
			iNumRemoved++;
		}
		else break;
	}

	for (int iI = 0; iI < iNumCitizens; iI++)
	{
		if (paeRemovedSpecailists[iI] != NO_SPECIALIST)
		{
			const CvSpecialistInfo& kSpecialist = GC.getSpecialistInfo(paeRemovedSpecailists[iI]);
			iHappiness -= kSpecialist.getHappinessPercent();
			iHealthiness -= kSpecialist.getHealthPercent();
			iGreatPeopleRate -= kSpecialist.getGreatPeopleRateChange();
			for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				//Team Project (1)
				aiYields[iJ] -= specialistYield(paeRemovedSpecailists[iI], (YieldTypes)iJ);
			}
			for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				aiCommerces[iJ] -= GET_PLAYER(getOwner()).specialistCommerce(paeRemovedSpecailists[iI], (CommerceTypes)iJ);
			}
		}
	}
	iHealthiness /= 100;
	iHappiness /= 100;
	for (int plotIdx = 0; plotIdx < NUM_CITY_PLOTS; plotIdx++)
	{
		if (abRemovedPlots[plotIdx])
		{
			const CvPlot* pLoopPlot = getCityIndexPlot(plotIdx);
			FAssertMsg(pLoopPlot != NULL, CvString::format("pLoopPlot was null for iIndex %d", plotIdx).c_str());
			if (pLoopPlot != NULL)
			{
				for (int yieldIdx = 0; yieldIdx < NUM_YIELD_TYPES; yieldIdx++)
				{
					aiYields[yieldIdx] -= pLoopPlot->getYield((YieldTypes)yieldIdx);
				}
			}
		}
	}
}

int CvCity::calculatePopulationHappiness() const
{
	return m_iHappinessPercentPerPopulation * getPopulation() / 100;
}

void CvCity::changeHappinessPercentPerPopulation(int iChange)
{
	if (iChange != 0)
	{
		m_iHappinessPercentPerPopulation += iChange;
		AI_setAssignWorkDirty(true);
	}
}

int CvCity::calculatePopulationHealth() const
{
	return m_iHealthPercentPerPopulation * getPopulation() / 100;
}

void CvCity::changeHealthPercentPerPopulation(int iChange)
{
	if (iChange != 0)
	{
		m_iHealthPercentPerPopulation += iChange;
		AI_setAssignWorkDirty(true);
	}
}

int CvCity::getAssignedSpecialistCount() const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		iCount += getSpecialistCount((SpecialistTypes)iI);
	}
	return iCount;
}

bool CvCity::isAutomatedCanBuild(BuildTypes eBuild) const
{
	return m_pabAutomatedCanBuild[eBuild];
}

void CvCity::setAutomatedCanBuild(BuildTypes eBuild, bool bNewValue)
{
	m_pabAutomatedCanBuild[eBuild] = bNewValue;
}

int CvCity::getMintedCommerceTimes100() const
{
	PROFILE_EXTRA_FUNC();
	int iCommerceTimes100 = 0;
	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		int iBonusCount = getNumBonuses((BonusTypes)iI);
		if (iBonusCount != 0)
		{
			iCommerceTimes100 += iBonusCount * GET_PLAYER(getOwner()).getBonusMintedPercent((BonusTypes)iI);
		}
	}
	return iCommerceTimes100;
}

void CvCity::clearModifierTotals()
{
	PROFILE_EXTRA_FUNC();
	// If the city is running a process turn it off until the recalc completes
	bst::optional<OrderData> headOrder = getHeadOrder();
	if (headOrder)
	{
		if (headOrder->eOrderType == ORDER_MAINTAIN)
		{
			processProcess(headOrder->getProcessType(), -1);
		}
	}
	// Remove this city from its plot group (without bothering to do any recalculation iteratively)
	CvPlotGroup* ownerPlotGroup = plotGroup(getOwner());
	if (ownerPlotGroup)
	{
		ownerPlotGroup->removePlot(plot(), false);
	}
	m_iNumNationalWonders = 0;
	m_iNumWorldWonders = 0;
	m_iNumTeamWonders = 0;
	m_iNumBuildings = 0;
	m_iGreatPeopleRateModifier = 0;
	m_iGovernmentCenterCount = 0;
	m_iMaintenanceModifier = 0;
	m_iWarWearinessModifier = 0;
	m_iHurryAngerModifier = 0;
	m_iHealRate = 0;
	m_iBuildingGoodHealth = 0;
	m_iBuildingBadHealth = 0;
	m_iBonusGoodHealth = 0;
	m_iBonusBadHealth = 0;
	m_iBuildingGoodHappiness = 0;
	m_iBuildingBadHappiness = 0;
	m_iExtraBuildingGoodHappiness = 0;
	m_iExtraBuildingBadHappiness = 0;
	m_iExtraBuildingGoodHealth = 0;
	m_iExtraBuildingBadHealth = 0;
	m_iBonusGoodHappiness = 0;
	m_iBonusBadHappiness = 0;
	m_iReligionGoodHappiness = 0;
	m_iReligionBadHappiness = 0;
	m_iExtraHappiness = 0;
	m_iExtraHealth = 0;
	m_iNoUnhappinessCount = 0;
	m_iNoUnhealthyPopulationCount = 0;
	m_iBuildingOnlyHealthyCount = 0;
	m_iFoodKeptPercent = 0;
	m_iMilitaryProductionModifier = 0;
	m_iSpaceProductionModifier = 0;
	m_iExtraTradeRoutes = 0;
	m_iTradeRouteModifier = 0;
	m_iForeignTradeRouteModifier = 0;
	m_iBuildingDefense = 0;
	m_iBuildingBombardDefense = 0;
	m_iFreeExperience = 0;
	m_iMaxAirlift = 0;
	m_iAirModifier = 0;
	m_iAirUnitCapacity = 0;
	m_iNukeModifier = 0;
	m_iFreeSpecialist = 0;
	m_iPowerCount = 0;
	m_iSpecialistFreeExperience = 0;
	m_iEspionageDefenseModifier = 0;
	m_fPopulationgrowthratepercentageLog = 0.0;
	m_iImprovementGoodHealth = 0;
	m_iImprovementBadHealth = 0;
	m_iSpecialistGoodHealth = 0;
	m_iSpecialistBadHealth = 0;
	m_iSpecialistHappiness = 0;
	m_iSpecialistUnhappiness = 0;
	m_iLineOfSight = 0;
	m_iAdjacentDamagePercent = 0;
	m_iWorkableRadiusOverride = 0;
	m_iProtectedCultureCount = 0;
	m_iNumUnitFullHeal = 0;
	m_iMinimumDefenseLevel = 0;
	m_iHealthPercentPerPopulation = 0;
	m_iHappinessPercentPerPopulation = 0;
	m_iBaseGreatPeopleRate = 0;
	m_iNumPopulationEmployed = 0;

	for (int iI = GC.getNumBonusInfos() - 1; iI > -1; iI--)
	{
		for (int iJ = 0; iJ < GC.getNumPropertyInfos(); iJ++)
		{
			m_ppaaiExtraBonusAidModifier[iI][iJ] = 0;
		}
		m_paiFreeBonus[iI] = 0;
		m_paiNumBonuses[iI] = 0;
	}

	for (int iI = GC.getNumBuildingInfos() - 1; iI > -1; iI--)
	{
		m_pabReligiouslyDisabledBuilding[iI] = false;
	}

	for (int iI = GC.getNumUnitInfos() - 1; iI > -1; iI--)
	{
		m_paiGreatPeopleUnitRate[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		m_paiMaxSpecialistCount[iI] = 0;
		m_paiForceSpecialistCount[iI] = 0;
		m_paiFreeSpecialistCount[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		m_paiImprovementFreeSpecialists[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		m_paiStateReligionHappiness[iI] = 0;
	}

	for (int iI = GC.getNumUnitCombatInfos() - 1; iI > -1; iI--)
	{
		m_paiUnitCombatFreeExperience[iI] = 0;
		m_paiUnitCombatExtraStrength[iI] = 0;
		m_paiUnitCombatProductionModifier[iI] = 0;
		m_paiUnitCombatRepelModifier[iI] = 0;
		m_paiUnitCombatRepelAgainstModifier[iI] = 0;
		m_paiUnitCombatDefenseAgainstModifier[iI] = 0;
		m_paiDamageAttackingUnitCombatCount[iI] = 0;
		m_paiHealUnitCombatTypeVolume[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
	{
		m_paiNewAfflictionTypeCount[iI] = 0;
		m_paiNewExtraAfflictionOutbreakLevelChange[iI] = 0;
		m_paiNewAfflictionToleranceChange[iI] = 0;
		m_paiNewCurrentOvercomeChange[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumPropertyInfos(); iI++)
	{
		m_paiAidRate[iI] = 0;
	}

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiRiverPlotYield[iI] = 0;
		m_aiBaseYieldRate[iI] = 0;
		m_aiBaseYieldPerPopRate[iI] = 0;
		m_aiYieldRateModifier[iI] = 0;
		m_aiPowerYieldRateModifier[iI] = 0;
		m_aiBonusYieldRateModifier[iI] = 0;
		m_aiTradeYield[iI] = 0;
		m_aiCorporationYield[iI] = 0;
		m_aiExtraSpecialistYield[iI] = 0;
		m_aiExtraYield[iI] = 0;
		m_buildingExtraYield100[iI] = 0;
		m_buildingYieldMod[iI] = 0;
	}

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		m_aiCommerceRate[iI] = 0;
		m_abCommerceRateDirty[iI] = false;
		m_aiProductionToCommerceModifier[iI] = 0;
		m_aiBuildingCommerce[iI] = 0;
		m_aiSpecialistCommerce100[iI] = 0;
		m_aiReligionCommerce[iI] = 0;
		m_aiCorporationCommerce[iI] = 0;
		m_aiCommerceRateModifier[iI] = 0;
		m_aiCommerceHappinessPer[iI] = 0;
		m_commercePerPopFromBuildings[iI] = 0;
		m_aiBonusCommerceRateModifier[iI] = 0;
		m_aiBonusCommercePercentChanges[iI] = 0;
		m_aiBuildingCommerceTechChange[iI] = 0;
		m_aiExtraSpecialistCommerce[iI] = 0;
		m_buildingCommerceMod[iI] = 0;
	}

	for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		m_aiDomainFreeExperience[iI] = 0;
		m_aiDomainProductionModifier[iI] = 0;
	}

	m_vDisabledBuildings.clear();
	m_vFreeBuildings.clear();
	m_freeAreaBuildingCount.clear();
	m_aBuildingYieldChange.clear();
	m_aBuildingCommerceChange.clear();
	m_aBuildingHappyChange.clear();
	m_aBuildingHealthChange.clear();
	m_buildingProductionMod.clear();
	m_unitProductionMod.clear();
	m_bonusDefenseChanges.clear();
	m_buildingHappinessFromTech.clear();
	m_buildingHealthFromTech.clear();
	m_corpBonusProduction.clear();

	m_terrainYieldChanges.clear();
	m_plotYieldChanges.clear();

	//m_Properties.clear();
	m_aPropertySpawns.clear();

	//	Force isWorkingPlot() to return false for now because we don't
	//	want chnages to other thuings like traits, adjusting trhe not-yte-set
	//	city yields based on plots being worked until we explicitly add them back in
	m_bPlotWorkingMasked = true;

#ifdef STRENGTH_IN_NUMBERS
	m_iTotalFrontSupportPercentModifier = 0;
	m_iTotalShortRangeSupportPercentModifier = 0;
	m_iTotalMediumRangeSupportPercentModifier = 0;
	m_iTotalLongRangeSupportPercentModifier = 0;
	m_iTotalFlankSupportPercentModifier = 0;
#endif
	m_iExtraLocalCaptureProbabilityModifier = 0;
	m_iExtraLocalCaptureResistanceModifier = 0;
	m_iExtraLocalDynamicDefense = 0;
	m_iExtraRiverDefensePenalty = 0;
	m_iExtraLocalRepel = 0;
	m_iExtraMinDefense = 0;
	m_iExtraBuildingDefenseRecoverySpeedModifier = 0;
	m_iModifiedBuildingDefenseRecoverySpeedCap = 0;
	m_iExtraCityDefenseRecoverySpeedModifier = 0;
	m_iExtraTechSpecialistHappiness = 0;
	m_iExtraBuildingHappinessFromTech = 0;
	m_iExtraTechSpecialistHealth = 0;
	m_iExtraBuildingHealthFromTech = 0;
	m_iPrioritySpecialist = NO_SPECIALIST;
	m_iExtraInsidiousness = 0;
	m_iExtraInvestigation = 0;

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			m_ppaaiLocalSpecialistExtraYield[iI][iJ] = 0;
		}
		m_paiSpecialistBannedCount[iI] = 0;
		for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			m_ppaaiLocalSpecialistExtraCommerce[iI][iJ] = 0;
		}
	}
}

void CvCity::recalculateModifiers()
{
	PROFILE_EXTRA_FUNC();
	area()->changePower(getOwner(), getPopulation());

	m_bPlotWorkingMasked = false;

	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		if (isWorkingPlot(iI))
		{
			processWorkingPlot(iI, 1, true);
		}
	}

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		const int iSpecialistCount = getSpecialistCount((SpecialistTypes)iI);

		if (iSpecialistCount > 0)
		{
			processSpecialist((SpecialistTypes)iI, iSpecialistCount);
		}

		//	Add back the unattributed specialists (those direct from Python or
		//	from GPs that joined the city)
		if (m_paiFreeSpecialistCountUnattributed[iI] != 0)
		{
			m_paiFreeSpecialistCount[iI] += m_paiFreeSpecialistCountUnattributed[iI];
			processSpecialist((SpecialistTypes)iI, m_paiFreeSpecialistCountUnattributed[iI]);
		}
	}
	{
		std::vector<BuildingTypes> newBuildings;

		foreach_(const BuildingTypes eTypeX, getHasBuildings())
		{
			const CvBuildingInfo& info = GC.getBuildingInfo(eTypeX);

			// Toffer - Xml changes may have invalidated a building the city have; hence bValid.
			const bool bObsolete = info.getObsoleteTech() < 0 ? false : GET_TEAM(getTeam()).isHasTech(info.getObsoleteTech());
			bool bValid =
			(
				!bObsolete
				&& // Do we have the building that this may be an extention of?
				(info.getExtendsBuilding() == -1 || hasBuilding(info.getExtendsBuilding()))
			);
			if (!bValid) // Forget it.
			{
				alterBuildingLedger(eTypeX, false);

				if (bObsolete)
				{
					BuildingTypes eObsoletesToBuilding = info.getObsoletesToBuilding();
					while (eObsoletesToBuilding > NO_BUILDING)
					{
						if (hasBuilding(eObsoletesToBuilding))
						{
							break;
						}
						const CvBuildingInfo& newBuilding = GC.getBuildingInfo(eObsoletesToBuilding);
						if (newBuilding.getObsoleteTech() < 0 || !GET_TEAM(getTeam()).isHasTech(newBuilding.getObsoleteTech()))
						{
							newBuildings.push_back(eObsoletesToBuilding);
							break;
						}
						eObsoletesToBuilding = newBuilding.getObsoletesToBuilding();
					}
				}
			}
			else
			{
				// Is it replaced by another building.
				for (int iJ = 0; iJ < info.getNumReplacementBuilding(); ++iJ)
				{
					if (hasBuilding((BuildingTypes)info.getReplacementBuilding(iJ)))
					{
						setDisabledBuilding(eTypeX, true, false);
						bValid = false;
						break;
					}
				}
				handleBuildingCounts(eTypeX, 1, isLimitedWonder(eTypeX) && !info.isNoLimit());
				if (bValid)
				{
					processBuilding(eTypeX, 1, false, true);
				}
			}
		}
		foreach_(const BuildingTypes eTypeX, newBuildings)
		{
			changeHasBuilding(eTypeX, true);
		}
	}

	//	Put corporations back
	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (isHasCorporation((CorporationTypes)iI))
		{
			applyCorporationModifiers((CorporationTypes)iI, true);
		}
	}

	updateCorporation();

	//	Put religions back
	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if (isHasReligion((ReligionTypes)iI))
		{
			applyReligionModifiers((ReligionTypes)iI, true);
		}
	}

	//checkBuildings(false);
	checkReligiousDisablingAllBuildings();

	//	Replay events in so far as they effect modifiers
	for (int iI = 0; iI < (int)m_aEventsOccured.size(); iI++)
	{
		if (m_aEventsOccured[iI] != NO_EVENT)
		{
			applyEvent(m_aEventsOccured[iI], NULL);
		}
	}

	//	If the city is running a process account for it again
	bst::optional<OrderData> headOrder = getHeadOrder();
	if (headOrder)
	{
		switch (headOrder->eOrderType)
		{
		case ORDER_MAINTAIN:
			processProcess(headOrder->getProcessType(), 1);
			break;
		default:
			break;
		}
	}

	updateFreshWaterHealth();
	updateFeatureHealth();
	updateImprovementHealth();
	updateFeatureHappiness();

	//ls612: Make Sure to keep the Air Unit capacity
	changeAirUnitCapacity(GC.getCITY_AIR_UNIT_CAPACITY());
}

void CvCity::setBuildingListInvalid()
{
	m_BuildingList.setInvalid();
}

bool CvCity::getBuildingListFilterActive(BuildingFilterTypes eFilter) const
{
	return m_BuildingList.getFilterActive(eFilter);
}

void CvCity::setBuildingListFilterActive(BuildingFilterTypes eFilter, bool bActive)
{
	m_BuildingList.setFilterActive(eFilter, bActive);
}

BuildingGroupingTypes CvCity::getBuildingListGrouping()
{
	return m_BuildingList.getGroupingActive();
}

void CvCity::setBuildingListGrouping(BuildingGroupingTypes eGrouping)
{
	m_BuildingList.setGroupingActive(eGrouping);
}

BuildingSortTypes CvCity::getBuildingListSorting()
{
	return m_BuildingList.getSortingActive();
}

void CvCity::setBuildingListSorting(BuildingSortTypes eSorting)
{
	m_BuildingList.setSortingActive(eSorting);
}

int CvCity::getBuildingListGroupNum()
{
	return m_BuildingList.getGroupNum();
}

int CvCity::getBuildingListNumInGroup(int iGroup)
{
	return m_BuildingList.getNumInGroup(iGroup);
}

BuildingTypes CvCity::getBuildingListType(int iGroup, int iPos)
{
	return m_BuildingList.getBuildingType(iGroup, iPos);
}

int CvCity::getBuildingListSelectedBuildingRow()
{
	return m_BuildingList.getBuildingSelectionRow();
}

int CvCity::getBuildingListSelectedWonderRow()
{
	return m_BuildingList.getWonderSelectionRow();
}

BuildingTypes CvCity::getBuildingListSelectedBuilding()
{
	return m_BuildingList.getSelectedBuilding();
}

BuildingTypes CvCity::getBuildingListSelectedWonder()
{
	return m_BuildingList.getSelectedWonder();
}

void CvCity::setBuildingListSelectedBuilding(BuildingTypes eBuilding)
{
	m_BuildingList.setSelectedBuilding(eBuilding);
}

void CvCity::setBuildingListSelectedWonder(BuildingTypes eWonder)
{
	m_BuildingList.setSelectedWonder(eWonder);
}

void CvCity::setUnitListInvalid()
{
	m_UnitList.setInvalid();
}

bool CvCity::getUnitListFilterActive(UnitFilterTypes eFilter) const
{
	return m_UnitList.getFilterActive(eFilter);
}

void CvCity::setUnitListFilterActive(UnitFilterTypes eFilter, bool bActive)
{
	m_UnitList.setFilterActive(eFilter, bActive);
}

UnitGroupingTypes CvCity::getUnitListGrouping()
{
	return m_UnitList.getGroupingActive();
}

void CvCity::setUnitListGrouping(UnitGroupingTypes eGrouping)
{
	m_UnitList.setGroupingActive(eGrouping);
}

UnitSortTypes CvCity::getUnitListSorting()
{
	return m_UnitList.getSortingActive();
}

void CvCity::setUnitListSorting(UnitSortTypes eSorting)
{
	m_UnitList.setSortingActive(eSorting);
}

int CvCity::getUnitListGroupNum()
{
	return m_UnitList.getGroupNum();
}

int CvCity::getUnitListNumInGroup(int iGroup)
{
	return m_UnitList.getNumInGroup(iGroup);
}

UnitTypes CvCity::getUnitListType(int iGroup, int iPos)
{
	return m_UnitList.getUnitListType(iGroup, iPos);
}

int CvCity::getUnitListSelectedRow()
{
	return m_UnitList.getSelectionRow();
}

UnitTypes CvCity::getUnitListSelected()
{
	return m_UnitList.getSelectedUnit();
}

void CvCity::setUnitListSelected(UnitTypes eUnit)
{
	m_UnitList.setSelectedUnit(eUnit);
}

int CvCity::getTotalBuildingSourcedProperty(PropertyTypes eProperty) const
{
	PROFILE_EXTRA_FUNC();
	std::map<int, int>::const_iterator itr = m_buildingSourcedPropertyCache.find(eProperty);

	if (itr != m_buildingSourcedPropertyCache.end())
	{
		return itr->second;
	}
	int	iValue = 0;

	foreach_(const BuildingTypes eTypeX, getHasBuildings())
	{
		if (!isReligiouslyLimitedBuilding(eTypeX) && !isDisabledBuilding(eTypeX))
		{
			foreach_(const CvPropertySource* pSource, GC.getBuildingInfo(eTypeX).getPropertyManipulators()->getSources())
			{
				//	For now we're only interested in constant sources
				//	TODO - expand this as buildings add other types
				if (pSource->getType() == PROPERTYSOURCE_CONSTANT && pSource->getProperty() == eProperty)
				{
					iValue += static_cast<const CvPropertySourceConstant*>(pSource)->getAmountPerTurn(getGameObject());
				}
			}
		}
	}
	m_buildingSourcedPropertyCache[(int)eProperty] = iValue;

	return iValue;
}

void unitSources(const CvPropertyManipulators* pMani, PropertyTypes eProperty, const CvCity* pCity, int* iValue)
{
	PROFILE_EXTRA_FUNC();
	foreach_(const CvPropertySource* pSource, pMani->getSources())
	{
		//	Sources that deliver to the city or the plot are both considered since the city plot diffuses
		//	to the city for most properties anyway
		if (pSource->getProperty() == eProperty &&
			(pSource->getObjectType() == GAMEOBJECT_CITY || pSource->getObjectType() == GAMEOBJECT_PLOT) &&
			pSource->getType() == PROPERTYSOURCE_CONSTANT)
		{
			*iValue += static_cast<const CvPropertySourceConstant*>(pSource)->getAmountPerTurn(pCity->getGameObject());
		}
	}
}

int CvCity::getTotalUnitSourcedProperty(PropertyTypes eProperty) const
{
	PROFILE_EXTRA_FUNC();
	std::map<int, int>::const_iterator itr = m_unitSourcedPropertyCache.find(eProperty);

	if (itr != m_unitSourcedPropertyCache.end())
	{
		return itr->second;
	}
	int	iValue = 0;

	foreach_ (const CvUnit* unit, plot()->units())
	{
		unit->getGameObject()->foreachManipulator(bind(unitSources, _1, eProperty, this, &iValue));
	}
	m_unitSourcedPropertyCache[(int)eProperty] = iValue;

	return iValue;
}

void unitHasSources(const CvPropertyManipulators* pMani, bool* bHasSources)
{
	PROFILE_EXTRA_FUNC();
	foreach_(const CvPropertySource* pSource, pMani->getSources())
	{
		//	Sources that deliver to the city or the plot are both considered since the city plot diffuses
		//	to the city for most properties anyway
		if ((pSource->getObjectType() == GAMEOBJECT_CITY || pSource->getObjectType() == GAMEOBJECT_PLOT) &&
			pSource->getType() == PROPERTYSOURCE_CONSTANT)
		{
			*bHasSources = true;
			break;
		}
	}
}

//	Helper function to determine if a unit has any city/plot property sources
static bool unitHasCityOrPlotPropertySources(const CvUnit* pUnit)
{
	PROFILE_EXTRA_FUNC();
	bool bHasSources;

	pUnit->getGameObject()->foreachManipulator(bind(unitHasSources, _1, &bHasSources));

	return bHasSources;
}

void CvCity::noteUnitMoved(const CvUnit* pUnit) const
{
	if (unitHasCityOrPlotPropertySources(pUnit))
	{
		m_unitSourcedPropertyCache.clear();
	}
}

void sumCitySources(const CvPropertyManipulators* pMani, const CvCity* pCity, int* iSum, PropertyTypes eProperty)
{
	PROFILE_EXTRA_FUNC();
	foreach_(const CvPropertySource* pSource, pMani->getSources())
	{
		if (pSource->getProperty() == eProperty)
		{
			if (pSource->isActive(const_cast<CvGameObjectCity*>(pCity->getGameObject())))
			{
				*iSum += pSource->getSourcePredict(pCity->getGameObject(), pCity->getPropertiesConst()->getValueByProperty(eProperty));
			}
		}
	}
}

int CvCity::getGlobalSourcedProperty(PropertyTypes eProperty) const
{
	PROFILE_EXTRA_FUNC();
	int iSum = 0;
	foreach_(const CvPropertySource* pSource, GC.getPropertyInfo(eProperty).getPropertyManipulators()->getSources())
	{
		if (pSource->isActive(getGameObject()))
		{
			iSum += pSource->getSourcePredict(getGameObject(), getPropertiesConst()->getValueByProperty(eProperty));
		}
	}
	// Add sources from the player object that have an effect on cities
	GET_PLAYER(getOwner()).getGameObject()->foreachManipulator(bind(sumCitySources, _1, this, &iSum, eProperty));

	return iSum;
}

#ifdef OUTBREAKS_AND_AFFLICTIONS
int CvCity::getPropertyValue(PropertyTypes eProperty) const
{
	return getPropertiesConst()->getValueByProperty(eProperty);
}

//TB Combat Mods (Buildings) begin
int CvCity::getUnitAidPresent(PropertyTypes eProperty) const
{
	PROFILE_FUNC();

	return algo::max_element(
		plot()->units() | filtered(CvUnit::fn::getTeam() == getTeam())
						| transformed(CvUnit::fn::aidTotal(eProperty))
	).get_value_or(0);
}

int CvCity::getCityAidTotal(PromotionLineTypes ePromotionLineType) const
{
	PROFILE_EXTRA_FUNC();
	if (ePromotionLineType != NO_PROMOTIONLINE)
	{
		const PropertyTypes ePropertyType = GC.getPromotionLineInfo(ePromotionLineType).getPropertyType();
		if (ePropertyType != NO_PROPERTY)
		{
			int iBonusTotal = 0;
			for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
			{
				iBonusTotal += getExtraBonusAidModifier((BonusTypes)iI, ePropertyType);
			}
			return (getAidRate(ePropertyType) + getUnitAidPresent(ePropertyType) + iBonusTotal);
		}
	}
	return 0;
}

int CvCity::getAidRate(PropertyTypes ePropertyType) const
{
	FASSERT_BOUNDS(0, GC.getNumPropertyInfos(), ePropertyType);
	return std::max(0, m_paiAidRate[ePropertyType]);
}

void CvCity::changeAidRate(PropertyTypes ePropertyType, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumPropertyInfos(), ePropertyType);
	m_paiAidRate[ePropertyType] += iChange;
}

void CvCity::setAidRate(PropertyTypes ePropertyType, int iChange)//may be unnecessary
{
	FASSERT_BOUNDS(0, GC.getNumPropertyInfos(), ePropertyType);
	m_paiAidRate[ePropertyType] = iChange;
}

bool CvCity::hasAfflictionType(PromotionLineTypes ePromotionLineType) const
{
	FASSERT_BOUNDS(0, GC.getNumPromotionLineInfos(), ePromotionLineType);
	return m_paiNewAfflictionTypeCount[ePromotionLineType] > 0;
}

void CvCity::changeAfflictionTypeCount(PromotionLineTypes ePromotionLineType, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumPromotionLineInfos(), ePromotionLineType);
	m_paiNewAfflictionTypeCount[ePromotionLineType] += iChange;
}

void CvCity::setAfflictionTypeCount(PromotionLineTypes ePromotionLineType, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumPromotionLineInfos(), ePromotionLineType);
	m_paiNewAfflictionTypeCount[ePromotionLineType] = iChange;
}

int CvCity::getExtraBonusAidModifier(BonusTypes eBonusType, PropertyTypes ePropertyType) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eBonusType);
	return m_ppaaiExtraBonusAidModifier[eBonusType][ePropertyType];
}

void CvCity::changeExtraBonusAidModifier(BonusTypes eBonusType, PropertyTypes ePropertyType, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eBonusType);
	m_ppaaiExtraBonusAidModifier[eBonusType][ePropertyType] += iChange;
}

int CvCity::getExtraAfflictionOutbreakLevelChange(PromotionLineTypes ePromotionLine) const
{
	FASSERT_BOUNDS(0, GC.getNumPromotionLineInfos(), ePromotionLine);
	return m_paiNewExtraAfflictionOutbreakLevelChange[ePromotionLine];
}

void CvCity::changeExtraAfflictionOutbreakLevelChange(PromotionLineTypes ePromotionLine, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumPromotionLineInfos(), ePromotionLine);
	m_paiNewExtraAfflictionOutbreakLevelChange[ePromotionLine] += iChange;
}

void CvCity::setExtraAfflictionOutbreakLevelChange(PromotionLineTypes ePromotionLine, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumPromotionLineInfos(), ePromotionLine);
	m_paiNewExtraAfflictionOutbreakLevelChange[ePromotionLine] = iChange;
}

int CvCity::getAfflictionToleranceChange(PromotionLineTypes ePromotionLine) const
{
	FASSERT_BOUNDS(0, GC.getNumPromotionLineInfos(), ePromotionLine);
	return m_paiNewAfflictionToleranceChange[ePromotionLine];
}

void CvCity::changeAfflictionToleranceChange(PromotionLineTypes ePromotionLine, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumPromotionLineInfos(), ePromotionLine);
	m_paiNewAfflictionToleranceChange[ePromotionLine] = std::max(0, (m_paiNewAfflictionToleranceChange[ePromotionLine] + iChange));
}

void CvCity::setAfflictionToleranceChange(PromotionLineTypes ePromotionLine, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumPromotionLineInfos(), ePromotionLine);
	m_paiNewAfflictionToleranceChange[ePromotionLine] = iChange;
}

int CvCity::getCurrentOvercomeChange(PromotionLineTypes ePromotionLine) const
{
	FASSERT_BOUNDS(0, GC.getNumPromotionLineInfos(), ePromotionLine);
	return m_paiNewCurrentOvercomeChange[ePromotionLine];
}

void CvCity::changeCurrentOvercomeChange(PromotionLineTypes ePromotionLine, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumPromotionLineInfos(), ePromotionLine);
	m_paiNewCurrentOvercomeChange[ePromotionLine] += iChange;
}

void CvCity::setCurrentOvercomeChange(PromotionLineTypes ePromotionLine, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumPromotionLineInfos(), ePromotionLine);
	m_paiNewCurrentOvercomeChange[ePromotionLine] = iChange;
}

int CvCity::getTotalTechOutbreakLevelChange(BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	int iTotalTechOutbreakLevelChange = 0;
	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		const TechTypes eTech = ((TechTypes)iI);
		if (GET_TEAM(getTeam()).isHasTech(eTech))
		{
			iTotalTechOutbreakLevelChange += GC.getBuildingInfo(eBuilding).getTechOutbreakLevelChange(iI);
		}
	}
	return iTotalTechOutbreakLevelChange;
}

int CvCity::getOutbreakChangesTotal(BuildingTypes eAfflictionBuilding, PromotionLineTypes eAfflictionLine) const
{
	int iTotal = 0;
	//iTotal -= getTradeCommunicabilityTotal(eAfflictionBuilding, eAfflictionLine);
	//iTotal -= getUnitCommunicability(eAfflictionLine);
	//iTotal += getCityAidTotal();
	//TB Note: Communicability is tallied in the Outbreak Check function earlier than this function is called, then subtracted there
	iTotal += getExtraAfflictionOutbreakLevelChange(eAfflictionLine);
	iTotal += getTotalTechOutbreakLevelChange(eAfflictionBuilding);
	iTotal += getAfflictionToleranceChange(eAfflictionLine);

	return iTotal;
}

int CvCity::getOvercomeChangesTotal(BuildingTypes eAfflictionBuilding, PromotionLineTypes eAfflictionLine) const
{
	int iTotal = 0;
	iTotal -= getTotalCommunicableExposure(eAfflictionLine);
	iTotal += getCityAidTotal(eAfflictionLine);
	iTotal += getAfflictionToleranceChange(eAfflictionLine);
	iTotal -= getCurrentOvercomeChange(eAfflictionLine);
	/*iTotal += getExtraAfflictionOutbreakLevelChange(eAfflictionLine);*/
	/*iTotal += getTotalTechOutbreakLevelChange(eAfflictionBuilding);*/
	/*iTotal -= getUnitCommunicability(eAfflictionLine);*///Now included in Total Communicable Exposure

	return iTotal;
}

int CvCity::getUnitCommunicability(PromotionLineTypes eAfflictionLine) const
{
	PROFILE_FUNC();


	if (GC.getPromotionLineInfo(eAfflictionLine).isNoSpreadUnittoCity())
		return 0;

	// Find unit with the highest probability to afflict
	bst::optional<int> worstAffliction = algo::max_element(
		plot()->units() | filtered(CvUnit::fn::hasAfflictionLine(eAfflictionLine))
						| transformed(CvUnit::fn::worsenedProbabilitytoAfflict(eAfflictionLine))
	);

	if (worstAffliction)
	{
		return *worstAffliction + GC.getPromotionLineInfo(eAfflictionLine).getCommunicability();
	}
	return 0;
}

int CvCity::getTradeCommunicabilityTotal(BuildingTypes eAfflictionBuilding, PromotionLineTypes eAfflictionLine) const
{
	PROFILE_EXTRA_FUNC();
	int iTradeCommunicabilityTotal = 0;
	for (int iI = 0; iI < getTradeRoutes(); ++iI)
	{
		const CvCity* pCity = getTradeCity(iI);
		if (pCity->hasAfflictionType(eAfflictionLine) && pCity->isActiveBuilding(eAfflictionBuilding))
		{
			iTradeCommunicabilityTotal += GC.getBuildingInfo(eAfflictionBuilding).getTradeCommunicability();
		}
	}
	return iTradeCommunicabilityTotal;
}

bool CvCity::canAcquireAffliction(BuildingTypes eDisease, PromotionLineTypes eAfflictionLine) const
{
	if (eDisease == NO_BUILDING || hasBuilding(eDisease))
	{
		return false;
	}
	return true;
}

int CvCity::getTotalCommunicableExposure(PromotionLineTypes eAfflictionLine) const
{
	PROFILE_EXTRA_FUNC();
	const CvPromotionLineInfo& kAffliction = GC.getPromotionLineInfo(eAfflictionLine);

	int iTradeCommunicability = 0;
	for (int iI = 0; iI < kAffliction.getNumBuildings(); iI++)
	{
		iTradeCommunicability += getTradeCommunicabilityTotal((BuildingTypes)kAffliction.getBuilding(iI), eAfflictionLine);
	}
	int iUnitCommunicability = getUnitCommunicability(eAfflictionLine);
	int iWorkedTileCommunicability = 0;
	int iVicinityTileCommunicability = 0;

	for (int iI = 0; iI < getNumCityPlots(); iI++)
	{
		const CvPlot* pLoopPlot = ::plotCity(getX(), getY(), iI);
		if (isWorkingPlot(iI))
		{
			iWorkedTileCommunicability += pLoopPlot->getCommunicability(eAfflictionLine, true, false, false);
		}
		iVicinityTileCommunicability += pLoopPlot->getCommunicability(eAfflictionLine, false, true, false);
	}

	int iAccessVolumeBonusCommunicability = 0;

	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		const BonusTypes eBonus = (BonusTypes)iI;
		iAccessVolumeBonusCommunicability += (getNumBonuses(eBonus) * GC.getBonusInfo(eBonus).getAfflictionCommunicabilityType(eAfflictionLine, false, false, true).iModifier);
	}

	int iTotalCommunicableExposure = iTradeCommunicability + iUnitCommunicability + iWorkedTileCommunicability + iVicinityTileCommunicability + iAccessVolumeBonusCommunicability;
	return iTotalCommunicableExposure;
}

void CvCity::doOutbreakCheck(PromotionLineTypes eAfflictionLine)
{
	PROFILE_EXTRA_FUNC();
	const PropertyTypes ePropertyType = GC.getPromotionLineInfo(eAfflictionLine).getPropertyType();
	const CvPromotionLineInfo& kAffliction = GC.getPromotionLineInfo(eAfflictionLine);
	BuildingTypes eBuilding;
	int iLowestLinePriority = MAX_INT;

	if (kAffliction.getObsoleteTech() != NO_TECH)
	{
		if (GET_TEAM(getTeam()).isHasTech(kAffliction.getObsoleteTech()))
		{
			return;
		}
	}

	if (kAffliction.getPrereqTech() != NO_TECH)
	{
		if (!GET_TEAM(getTeam()).isHasTech(kAffliction.getPrereqTech()))
		{
			return;
		}
	}

	for (int iI = 0; iI < kAffliction.getNumBuildings(); iI++)
	{
		if (canAcquireAffliction((BuildingTypes)kAffliction.getBuilding(iI), eAfflictionLine))
		{
			const int iLinePriority = GC.getBuildingInfo((BuildingTypes)kAffliction.getBuilding(iI)).getLinePriority();
			if (iLinePriority < iLowestLinePriority)
			{
				iLowestLinePriority = iLinePriority;
				eBuilding = (BuildingTypes)kAffliction.getBuilding(iI);
			}
		}
	}

	if (iLowestLinePriority == MAX_INT)
	{
		//can't acquire
		return;
	}
	//set bonuses with promotionlines like buildings with promotionlines on promotionlineinfos so we can loop here
	int iTotalCommunicableExposure = getTotalCommunicableExposure(eAfflictionLine);

	bool bExposed = (iTotalCommunicableExposure > 0);
	bool bDoCheck = false;

	if (iLowestLinePriority <= 1)
	{
		//we do not have an outbreak at all and must check to see if we're exposed.
		//If not exposed, we check canconstruct with a fair amount of prerequisite details.  The base likelihood is used for the check.
		//Otherwise, many canconstruct factors won't matter.  And the degree to which we are exposed will make a difference in the likelihood.
		bDoCheck = canConstruct(eBuilding, false, false, true/*bIgnoreCost - must be true for affliction checks*/, false, false, NO_TECH, 0, true, bExposed);
		//<-Add bools for bAffliction and bExposed - bAffliction would be true and bExposed would be bExposed
	}
	else
	{
		//We already are in an outbreak.
		//Exposed has nothing to do with it.  The likelihood for getting worse is entirely a matter of the disease threshhold for this lowest building.
		bDoCheck = true;
	}

	if (!bDoCheck)
	{
		return;
	}

	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	//Design the check portion.
	int iOutbreakBase = kBuilding.getOutbreakBase();
	int iOutbreakTotal = iOutbreakBase - iTotalCommunicableExposure + getOutbreakChangesTotal(eBuilding, eAfflictionLine);

	int iChancetoOutbreak = getPropertyValue(ePropertyType) - iOutbreakTotal;
	int iChanceModifier = kAffliction.getOutbreakModifier();
	iChancetoOutbreak *= iChanceModifier;//spreads the chance of an outbreak across a wider range of property totals if low and condenses it into a smaller range if high.
	iChancetoOutbreak /= 100; //in short, this is a % modifier to the value of 1 of the property value.  Thus, if you would normally have a 100% chance to have an outbreak at 100 property value (0 base threshold with no modifiers) and a 50% chance at 50 property value, a 50% modifier here (half) would make it so you have a 100% chance at 200, a 50% chance at 100 and a 25% chance at 50.  This halving increased the RANGE by twice as much and diluted the value of a given point of the modifier.

	int iOutbreakRollResult = GC.getGame().getSorenRandNum(100, "Outbreak Check");

	if (iOutbreakRollResult < iChancetoOutbreak)
	{
		const CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_OUTBREAK", getNameKey(), kBuilding.getDescription());
		AddDLLMessage(getOwner(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_RED(), getX(), getY());
		changeHasBuilding(eBuilding, true);
	}

	return;
}

void CvCity::doOvercomeCheck(PromotionLineTypes eAfflictionLine)
{
	PROFILE_EXTRA_FUNC();
	const PropertyTypes ePropertyType = GC.getPromotionLineInfo(eAfflictionLine).getPropertyType();
	const CvPromotionLineInfo& kAffliction = GC.getPromotionLineInfo(eAfflictionLine);
	int iHighestLinePriority = 0;
	BuildingTypes eBuilding;

	for (int iI = 0; iI < kAffliction.getNumBuildings(); iI++)
	{
		if (isActiveBuilding((BuildingTypes)kAffliction.getBuilding(iI)))
		{
			const int iLinePriority = GC.getBuildingInfo((BuildingTypes)kAffliction.getBuilding(iI)).getLinePriority();
			if (iLinePriority > iHighestLinePriority)
			{
				iHighestLinePriority = iLinePriority;
				eBuilding = (BuildingTypes)kAffliction.getBuilding(iI);
			}
		}
	}

	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	int iOvercomeBase = kBuilding.getOvercomeBase();
	int iOvercomeTotal = iOvercomeBase + getOvercomeChangesTotal(eBuilding, eAfflictionLine);
	int iChancetoOvercome = iOvercomeTotal - getPropertyValue(ePropertyType);
	int iChanceModifier = kAffliction.getOvercomeModifier();
	iChancetoOvercome *= iChanceModifier;
	iChancetoOvercome /= 100;

	int iOvercomeRollResult = GC.getGame().getSorenRandNum(100, "Overcome Check");

	if (iOvercomeRollResult < iChancetoOvercome)
	{
		const CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_OVERCOME_CITY", getNameKey(), GC.getBuildingInfo(eBuilding).getDescription());
		AddDLLMessage(getOwner(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), getX(), getY());
		changeHasBuilding(eBuilding, false);
	}
	//PropertyTypes eDiseaseType = GC.getPROPERTY_DISEASE();
	//PropertyTypes eProperty;
	//PromotionLineTypes eCityDisease;
	//CvPropertyInfo& kInfo = GC.getPropertyInfo(eDiseaseType);
	//CvWString szBuffer;
	//int iNum = kInfo.getNumPropertyBuildings();
	//int iI;
	//int iJ;
	//int iOutbreakBase;
	//int iOvercomeTotal;
	//int iChancetoOvercome;
	//int iOvercomeRollResult;

	//for (iI=0; iI<iNum; iI++)
	//{
	//	eProperty = ((PropertyTypes)iI);
	//	if (eProperty == eDiseaseType)
	//	{
	//		PropertyBuilding& kBuilding = kInfo.getPropertyBuilding(iI);

	//		for (int iJ = 0; iJ < GC.getBuildingInfo(kBuilding.eBuilding).getNumDiseaseTypes(); iJ++)
	//		{
	//			eCityDisease = (PromotionLineTypes)GC.getBuildingInfo(kBuilding.eBuilding).getDiseaseType(iJ);
	//			if (eCityDisease == eAfflictionLine)
	//			{
	//				iOutbreakBase = kBuilding.iMinValue;
	//				iOvercomeTotal = iOutbreakBase + getOvercomeChangesTotal(kBuilding.eBuilding, eAfflictionLine);
	//				iChancetoOvercome = iOvercomeTotal - getDiseasePropertyValue();

	//				if (getDiseasePropertyValue() < iOvercomeTotal)
	//				{
	//					if (isActiveBuilding(kBuilding.eBuilding))
	//					{
	//						iOvercomeRollResult = GC.getGame().getSorenRandNum(100, "Overcome");
	//						if (iOvercomeRollResult < iChancetoOvercome)
	//						{

	//							const CvWString szBuffer = DLL_SERIALIZE(gDLL->getText("TXT_KEY_MISC_OVERCOME_CITY", getNameKey(), GC.getBuildingInfo(kBuilding.eBuilding).getDescription()));
	//							AddDLLMessage(getOwner(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_POSITIVE_DINK", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_GREEN(), getX(), getY());
	//							changeHasBuilding(kBuilding.eBuilding, false);
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
}

int CvCity::getPromotionLineAfflictionAttackCommunicability(PromotionLineTypes eAffliction) const
{
	FASSERT_BOUNDS(0, GC.getNumPromotionLineInfos(), eAffliction);
	return m_paiPromotionLineAfflictionAttackCommunicability[eAffliction];
}

void CvCity::changePromotionLineAfflictionAttackCommunicability(PromotionLineTypes eAffliction, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumPromotionLineInfos(), eAffliction);
	m_paiPromotionLineAfflictionAttackCommunicability[eAffliction] += iChange;
}

void CvCity::setPromotionLineAfflictionAttackCommunicability(PromotionLineTypes eAffliction, int iValue)
{
	FASSERT_BOUNDS(0, GC.getNumPromotionLineInfos(), eAffliction);
	m_paiPromotionLineAfflictionAttackCommunicability[eAffliction] = iValue;
}
#endif // OUTBREAKS_AND_AFFLICTIONS

int CvCity::getUnitCombatProductionModifier(UnitCombatTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eIndex);
	return m_paiUnitCombatProductionModifier[eIndex];
}

void CvCity::changeUnitCombatProductionModifier(UnitCombatTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eIndex);
	m_paiUnitCombatProductionModifier[eIndex] += iChange;
}

int CvCity::getUnitCombatRepelModifierTotal(UnitCombatTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eIndex);
	return m_paiUnitCombatRepelModifier[eIndex];
}

void CvCity::changeUnitCombatRepelModifierTotal(UnitCombatTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eIndex);
	m_paiUnitCombatRepelModifier[eIndex] += iChange;
}

int CvCity::getUnitCombatRepelAgainstModifierTotal(UnitCombatTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eIndex);
	return m_paiUnitCombatRepelAgainstModifier[eIndex];
}

void CvCity::changeUnitCombatRepelAgainstModifierTotal(UnitCombatTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eIndex);
	m_paiUnitCombatRepelAgainstModifier[eIndex] += iChange;
}

int CvCity::getUnitCombatDefenseAgainstModifierTotal(UnitCombatTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eIndex);
	return m_paiUnitCombatDefenseAgainstModifier[eIndex];
}

void CvCity::changeUnitCombatDefenseAgainstModifierTotal(UnitCombatTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eIndex);
	m_paiUnitCombatDefenseAgainstModifier[eIndex] += iChange;
}

#ifdef STRENGTH_IN_NUMBERS
int CvCity::getTotalFrontSupportPercentModifier() const
{
	return m_iTotalFrontSupportPercentModifier;
}

void CvCity::changeTotalFrontSupportPercentModifier(int iChange)
{
	m_iTotalFrontSupportPercentModifier += iChange;
	FASSERT_NOT_NEGATIVE(getTotalFrontSupportPercentModifier());
}

int CvCity::getTotalShortRangeSupportPercentModifier() const
{
	return m_iTotalShortRangeSupportPercentModifier;
}

void CvCity::changeTotalShortRangeSupportPercentModifier(int iChange)
{
	m_iTotalShortRangeSupportPercentModifier += iChange;
	FASSERT_NOT_NEGATIVE(getTotalShortRangeSupportPercentModifier());
}

int CvCity::getTotalMediumRangeSupportPercentModifier() const
{
	return m_iTotalMediumRangeSupportPercentModifier;
}

void CvCity::changeTotalMediumRangeSupportPercentModifier(int iChange)
{
	m_iTotalMediumRangeSupportPercentModifier += iChange;
	FASSERT_NOT_NEGATIVE(getTotalMediumRangeSupportPercentModifier());
}

int CvCity::getTotalLongRangeSupportPercentModifier() const
{
	return m_iTotalLongRangeSupportPercentModifier;
}

void CvCity::changeTotalLongRangeSupportPercentModifier(int iChange)
{
	m_iTotalLongRangeSupportPercentModifier += iChange;
	FASSERT_NOT_NEGATIVE(getTotalLongRangeSupportPercentModifier());
}

int CvCity::getTotalFlankSupportPercentModifier() const
{
	return m_iTotalFlankSupportPercentModifier;
}

void CvCity::changeTotalFlankSupportPercentModifier(int iChange)
{
	m_iTotalFlankSupportPercentModifier += iChange;
	FASSERT_NOT_NEGATIVE(getTotalFlankSupportPercentModifier());
}
#endif

void CvCity::assignPromotionsFromBuildingChecked(const CvBuildingInfo& building, CvUnit* unit) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const FreePromoTypes& freePromoType, building.getFreePromoTypes())
	{
		if (unit->canAcquirePromotion(freePromoType.ePromotion, PromotionRequirements::Promote | PromotionRequirements::ForFree)
#ifdef COMBAT_MOD_EQUIPTMENT
		|| GC.getPromotionInfo(freePromoType.ePromotion).isEquipment() && canEquip(unit, freePromoType.ePromotion)
#endif COMBAT_MOD_EQUIPTMENT
		) {
			if (!freePromoType.m_pExprFreePromotionCondition ||
				freePromoType.m_pExprFreePromotionCondition->evaluate(unit->getGameObject()))
			{
				unit->setHasPromotion(freePromoType.ePromotion, true);
			}
		}
	}
}

bool CvCity::canEquip(const CvUnit* pUnit, PromotionTypes eEquipment) const
{
	PROFILE_EXTRA_FUNC();
	//Some of this could be a bit misleading if its not understood that the result should be true if its NOT an equipment at all.
	if (GC.getPromotionInfo(eEquipment).isEquipment())
	{
		for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			const PromotionTypes eMayDeny = ((PromotionTypes)iI);
			if (pUnit->isHasPromotion(eMayDeny) && GC.getPromotionInfo(eMayDeny).isEquipment())
			{
				if (GC.getPromotionInfo(eEquipment).getPromotionLine() != NO_PROMOTIONLINE && GC.getPromotionInfo(eMayDeny).getPromotionLine() != NO_PROMOTIONLINE)
				{
					if (GC.getPromotionInfo(eEquipment).getPromotionLine() == GC.getPromotionInfo(eMayDeny).getPromotionLine())
					{
						if (GC.getPromotionInfo(eEquipment).getLinePriority() <= GC.getPromotionInfo(eMayDeny).getLinePriority())
						{
							return false;
						}
					}
				}
			}
		}

		for (int iJ = 0; iJ < GC.getPromotionInfo(eEquipment).getNumNoAutoEquiptoCombatClassTypes(); iJ++)
		{
			const UnitCombatTypes eNoAuto = (UnitCombatTypes)GC.getPromotionInfo(eEquipment).getNoAutoEquiptoCombatClassType(iJ);
			if (pUnit->isHasUnitCombat(eNoAuto))
			{
				return false;
			}
		}
	}
	return true;
}


int CvCity::getBaseYieldRate(YieldTypes eIndex) const
{
	int iBaseYield = getPlotYield(eIndex) + getTradeYield(eIndex) + GET_PLAYER(getOwner()).getFreeCityYield(eIndex);

	if (GET_PLAYER(getOwner()).isGoldenAge() && GET_PLAYER(getOwner()).getGoldenAgeYield(eIndex) > 0)
	{
		iBaseYield += GET_PLAYER(getOwner()).getGoldenAgeYield(eIndex);
	}
	return iBaseYield;
}


int CvCity::getExtraLocalCaptureProbabilityModifier() const
{
	return m_iExtraLocalCaptureProbabilityModifier;
}

void CvCity::setExtraLocalCaptureProbabilityModifier(int iValue)
{
	m_iExtraLocalCaptureProbabilityModifier = iValue;
}

void CvCity::changeExtraLocalCaptureProbabilityModifier(int iChange)
{
	m_iExtraLocalCaptureProbabilityModifier += iChange;
}

int CvCity::getExtraLocalCaptureResistanceModifier() const
{
	return m_iExtraLocalCaptureResistanceModifier;
}
void CvCity::setExtraLocalCaptureResistanceModifier(int iValue)
{
	m_iExtraLocalCaptureResistanceModifier = iValue;
}
void CvCity::changeExtraLocalCaptureResistanceModifier(int iChange)
{
	m_iExtraLocalCaptureResistanceModifier += iChange;
}

int CvCity::localCitizenCaptureResistance() const
{
	int iTotal = 0;
	iTotal += getExtraLocalCaptureResistanceModifier();
	iTotal += GET_PLAYER(getOwner()).getExtraNationalCaptureResistanceModifier();
	return iTotal;
}

int CvCity::getExtraLocalDynamicDefense() const
{
	return m_iExtraLocalDynamicDefense;
}
void CvCity::setExtraLocalDynamicDefense(int iValue)
{
	m_iExtraLocalDynamicDefense = iValue;
}
void CvCity::changeExtraLocalDynamicDefense(int iChange)
{
	m_iExtraLocalDynamicDefense += iChange;
}

int CvCity::getExtraRiverDefensePenalty() const
{
	return m_iExtraRiverDefensePenalty;
}
void CvCity::setExtraRiverDefensePenalty(int iValue)
{
	m_iExtraRiverDefensePenalty = iValue;
}
void CvCity::changeExtraRiverDefensePenalty(int iChange)
{
	m_iExtraRiverDefensePenalty += iChange;
}

int CvCity::getExtraLocalRepel() const
{
	return m_iExtraLocalRepel;
}
void CvCity::setExtraLocalRepel(int iValue)
{
	m_iExtraLocalRepel = iValue;
}
void CvCity::changeExtraLocalRepel(int iChange)
{
	m_iExtraLocalRepel += iChange;
}

int CvCity::getExtraMinDefense() const
{
	return m_iExtraMinDefense;
}
void CvCity::setExtraMinDefense(int iValue)
{
	m_iExtraMinDefense = iValue;
}
void CvCity::changeExtraMinDefense(int iChange)
{
	m_iExtraMinDefense += iChange;
}

int CvCity::getExtraBuildingDefenseRecoverySpeedModifier() const
{
	return m_iExtraBuildingDefenseRecoverySpeedModifier;
}
void CvCity::setExtraBuildingDefenseRecoverySpeedModifier(int iValue)
{
	m_iExtraBuildingDefenseRecoverySpeedModifier = iValue;
}
void CvCity::changeExtraBuildingDefenseRecoverySpeedModifier(int iChange)
{
	m_iExtraBuildingDefenseRecoverySpeedModifier += iChange;
}

int CvCity::getModifiedBuildingDefenseRecoverySpeedCap() const
{
	return m_iModifiedBuildingDefenseRecoverySpeedCap;
}
void CvCity::setModifiedBuildingDefenseRecoverySpeedCap(int iValue)
{
	m_iModifiedBuildingDefenseRecoverySpeedCap = iValue;
}
void CvCity::changeModifiedBuildingDefenseRecoverySpeedCap(int iChange)
{
	m_iModifiedBuildingDefenseRecoverySpeedCap += iChange;
}

int CvCity::getExtraCityDefenseRecoverySpeedModifier() const
{
	return m_iExtraCityDefenseRecoverySpeedModifier;
}
void CvCity::setExtraCityDefenseRecoverySpeedModifier(int iValue)
{
	m_iExtraCityDefenseRecoverySpeedModifier = iValue;
}
void CvCity::changeExtraCityDefenseRecoverySpeedModifier(int iChange)
{
	m_iExtraCityDefenseRecoverySpeedModifier += iChange;
}

int CvCity::cityDefenseRecoveryRate() const
{
	int iValue = GC.getCITY_DEFENSE_DAMAGE_HEAL_RATE();

	int iRecoveryModifier = getExtraCityDefenseRecoverySpeedModifier();

	if (getDefenseModifier(false) < getModifiedBuildingDefenseRecoverySpeedCap())
	{
		iRecoveryModifier += getExtraBuildingDefenseRecoverySpeedModifier();
	}

	iValue *= (100 + iRecoveryModifier);
	iValue /= 100;

	return iValue;
}

bool CvCity::canDamageAttackingUnitCombat(UnitCombatTypes eUnitCombat) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eUnitCombat);

	return m_paiDamageAttackingUnitCombatCount[eUnitCombat] > 0;
}

int CvCity::getDamageAttackingUnitCombatCount(UnitCombatTypes eUnitCombat) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eUnitCombat);

	return m_paiDamageAttackingUnitCombatCount[eUnitCombat];
}

void CvCity::setDamageAttackingUnitCombatCount(UnitCombatTypes eUnitCombat, int iValue)
{
	m_paiDamageAttackingUnitCombatCount[eUnitCombat] = iValue;
}

void CvCity::changeDamageAttackingUnitCombatCount(UnitCombatTypes eUnitCombat, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eUnitCombat);

	m_paiDamageAttackingUnitCombatCount[eUnitCombat] += iChange;
}


void CvCity::updateExtraTechSpecialistHappiness()
{
	PROFILE_EXTRA_FUNC();
	int iRunningTotal = 0;

	for (int iI = GC.getNumSpecialistInfos() - 1; iI > -1; iI--)
	{
		const SpecialistTypes eSpecialist = static_cast<SpecialistTypes>(iI);;
		const int iSpecificSpecialistCount = specialistCount(eSpecialist);

		for (int iJ = GC.getNumTechInfos() - 1; iJ > -1; iJ--)
		{
			if (GET_TEAM(getTeam()).isHasTech(static_cast<TechTypes>(iJ)))
			{
				iRunningTotal += iSpecificSpecialistCount * GC.getSpecialistInfo(eSpecialist).getTechHappiness(static_cast<TechTypes>(iJ));
			}
		}
	}
	m_iExtraTechSpecialistHappiness = iRunningTotal;
}

int CvCity::getBuildingHappinessFromTech(const TechTypes eTech) const
{
	PROFILE_EXTRA_FUNC();
	for (std::vector< std::pair<TechTypes, int> >::const_iterator it = m_buildingHappinessFromTech.begin(); it != m_buildingHappinessFromTech.end(); ++it)
	{
		if ((*it).first == eTech)
		{
			return ((*it).second);
		}
	}
	return 0;
}

void CvCity::changeBuildingHappinessFromTech(const TechTypes eTech, const int iChange)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eTech);

	if (iChange == 0)
	{
		return;
	}
	if (GET_TEAM(getTeam()).isHasTech(eTech))
	{
		m_iExtraBuildingHappinessFromTech += iChange;
	}
	bool bFirst = true;
	for (std::vector< std::pair<TechTypes, int> >::iterator it = m_buildingHappinessFromTech.begin(); it != m_buildingHappinessFromTech.end(); ++it)
	{
		if ((*it).first == eTech)
		{
			if ((*it).second == -iChange)
			{
				m_buildingHappinessFromTech.erase(it);
			}
			else
			{
				(*it).second += iChange;
			}
			bFirst = false;
			break;
		}
	}
	if (bFirst) m_buildingHappinessFromTech.push_back(std::make_pair(eTech, iChange));
}

int CvCity::getExtraTechHappinessTotal() const
{
	return m_iExtraBuildingHappinessFromTech + m_iExtraTechSpecialistHappiness;
}

void CvCity::updateSpecialistHappinessHealthFromTech()
{
	updateExtraTechSpecialistHappiness();
	updateExtraTechSpecialistHealth();

	AI_setAssignWorkDirty(true);

	if (getTeam() == GC.getGame().getActiveTeam())
	{
		setInfoDirty(true);
	}
}


void CvCity::updateExtraTechSpecialistHealth()
{
	PROFILE_EXTRA_FUNC();
	int iRunningTotal = 0;

	for (int iI = GC.getNumSpecialistInfos() - 1; iI > -1; iI--)
	{
		const SpecialistTypes eSpecialist = static_cast<SpecialistTypes>(iI);
		const int iSpecificSpecialistCount = specialistCount(eSpecialist);

		for (int iJ = GC.getNumTechInfos() - 1; iJ > -1; iJ--)
		{
			if (GET_TEAM(getTeam()).isHasTech(static_cast<TechTypes>(iJ)))
			{
				iRunningTotal += iSpecificSpecialistCount * GC.getSpecialistInfo(eSpecialist).getTechHealth(static_cast<TechTypes>(iJ));
			}
		}
	}
	m_iExtraTechSpecialistHealth = iRunningTotal;
}

int CvCity::getExtraTechSpecialistHealth() const
{
	return m_iExtraTechSpecialistHealth;
}

int CvCity::getBuildingHealthFromTech(const TechTypes eTech) const
{
	PROFILE_EXTRA_FUNC();
	for (std::vector< std::pair<TechTypes, int> >::const_iterator it = m_buildingHealthFromTech.begin(); it != m_buildingHealthFromTech.end(); ++it)
	{
		if ((*it).first == eTech)
		{
			return ((*it).second);
		}
	}
	return 0;
}

void CvCity::changeBuildingHealthFromTech(const TechTypes eTech, const int iChange)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eTech);

	if (iChange == 0)
	{
		return;
	}
	if (GET_TEAM(getTeam()).isHasTech(eTech))
	{
		m_iExtraBuildingHealthFromTech += iChange;
	}
	bool bFirst = true;
	for (std::vector< std::pair<TechTypes, int> >::iterator it = m_buildingHealthFromTech.begin(); it != m_buildingHealthFromTech.end(); ++it)
	{
		if ((*it).first == eTech)
		{
			if ((*it).second == -iChange)
			{
				m_buildingHealthFromTech.erase(it);
			}
			else
			{
				(*it).second += iChange;
			}
			bFirst = false;
			break;
		}
	}
	if (bFirst) m_buildingHealthFromTech.push_back(std::make_pair(eTech, iChange));
}

int CvCity::getExtraTechHealthTotal() const
{
	return m_iExtraBuildingHealthFromTech + m_iExtraTechSpecialistHealth;
}

int CvCity::getLocalSpecialistExtraYield(SpecialistTypes eSpecialist, YieldTypes eYield) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eSpecialist);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eYield);

	return m_ppaaiLocalSpecialistExtraYield[eSpecialist][eYield];
}

void CvCity::changeLocalSpecialistExtraYield(SpecialistTypes eSpecialist, YieldTypes eYield, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eSpecialist);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eYield);

	if (iChange != 0)
	{
		m_ppaaiLocalSpecialistExtraYield[eSpecialist][eYield] += iChange;
	}
	updateExtraSpecialistYield();
	AI_setAssignWorkDirty(true);
}

int CvCity::getLocalSpecialistExtraCommerce(SpecialistTypes eSpecialist, CommerceTypes eCommerce) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eSpecialist);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eCommerce);

	return m_ppaaiLocalSpecialistExtraCommerce[eSpecialist][eCommerce];
}

void CvCity::changeLocalSpecialistExtraCommerce(SpecialistTypes eSpecialist, CommerceTypes eCommerce, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eSpecialist);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eCommerce);

	if (iChange != 0)
	{
		m_ppaaiLocalSpecialistExtraCommerce[eSpecialist][eCommerce] = (m_ppaaiLocalSpecialistExtraCommerce[eSpecialist][eCommerce] + iChange);
	}

	updateExtraSpecialistCommerce();

	AI_setAssignWorkDirty(true);
}

int CvCity::specialistCount(SpecialistTypes eSpecialist) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eSpecialist);

	return getSpecialistCount(eSpecialist) + getFreeSpecialistCount(eSpecialist);
}

int CvCity::specialistYield(SpecialistTypes eSpecialist, YieldTypes eYield) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eSpecialist);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eYield);

	return GET_PLAYER(getOwner()).specialistYield(eSpecialist, eYield) + getLocalSpecialistExtraYield(eSpecialist, eYield);
}

int CvCity::specialistCommerce(SpecialistTypes eSpecialist, CommerceTypes eCommerce) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eSpecialist);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eCommerce);

	int iTotal = GET_PLAYER(getOwner()).specialistCommerce(eSpecialist, eCommerce);
	iTotal += getLocalSpecialistExtraCommerce(eSpecialist, eCommerce);

	return iTotal;
}

int CvCity::specialistYieldTotal(SpecialistTypes eSpecialist, YieldTypes eYield) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eSpecialist);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eYield);

	return (specialistCount(eSpecialist) * specialistYield(eSpecialist, eYield));
}

int CvCity::getPrioritorizedSpecialist() const
{
	return m_iPrioritySpecialist;
}

void CvCity::setPrioritorizedSpecialist(SpecialistTypes eSpecialist)
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eSpecialist);

	m_iPrioritySpecialist = (int)eSpecialist;
	if (isSpecialistBanned(eSpecialist))
	{
		removeSpecialistBan(eSpecialist);
	}
}

bool CvCity::isSpecialistBanned(SpecialistTypes eSpecialist) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eSpecialist);

	return m_paiSpecialistBannedCount[eSpecialist] > 0;
}

void CvCity::banSpecialist(SpecialistTypes eSpecialist)
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eSpecialist);

	m_paiSpecialistBannedCount[eSpecialist] = 1;
	if (m_iPrioritySpecialist == (int)eSpecialist)
	{
		m_iPrioritySpecialist = NO_SPECIALIST;
	}
}

void CvCity::removeSpecialistBan(SpecialistTypes eSpecialist)
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eSpecialist);

	m_paiSpecialistBannedCount[eSpecialist] = 0;
}

bool CvCity::isDirectAttackable() const
{
	int iMinimumDefenseLevel = getMinimumDefenseLevel();
	if (iMinimumDefenseLevel == 0)
	{
		iMinimumDefenseLevel = MAX_INT;
	}

	//ls612 Quickfix: Cities can be attacked if they have no defenders, regardless of if there is a minimun defense level
	if (getDefenseModifier(false) > iMinimumDefenseLevel&& plot()->getNumDefenders(getOwner()) > 0)
	{
		return false;
	}

	return true;
}

int CvCity::getInvestigationTotal(bool bActual) const
{
	PROFILE_EXTRA_FUNC();
	int iBestUnitInvestigation = 0;
	int iAssistance = 0;
	int iFivePercentAssistance = 0;

	const CvPlot* pPlot = plot();
	if (pPlot != NULL)
	{
		CvUnit* pBestUnit = NULL;
		foreach_(CvUnit* pLoopUnit, pPlot->units())
		{
			if (pLoopUnit->getOwner() == getOwner())
			{
				const int iUnitInvestigation = pLoopUnit->getInvestigationTotal();

				if (iUnitInvestigation > iBestUnitInvestigation)
				{
					iBestUnitInvestigation = iUnitInvestigation;
					pBestUnit = pLoopUnit;
				}
				if (iUnitInvestigation > 0)
				{
					iAssistance++;
					iFivePercentAssistance += iUnitInvestigation;
				}
			}
		}
		if (bActual && pBestUnit != NULL)
		{
			iAssistance--;//To remove the bonus one would give itself.
			iFivePercentAssistance -= iBestUnitInvestigation;
			pBestUnit->changeExperience100(5);
		}
	}
	iFivePercentAssistance /= 20;
	int iTotal = iAssistance;
	iTotal += iFivePercentAssistance;
	iTotal += iBestUnitInvestigation;
	iTotal += getExtraInvestigation();
	iTotal += getSpecialistInvestigation();

	return iTotal;
}

int CvCity::getExtraInsidiousness() const
{
	return m_iExtraInsidiousness;
}
void CvCity::setExtraInsidiousness(int iValue)
{
	m_iExtraInsidiousness = iValue;
}
void CvCity::changeExtraInsidiousness(int iChange)
{
	m_iExtraInsidiousness += iChange;
}

int CvCity::getExtraInvestigation() const
{
	return m_iExtraInvestigation;
}
void CvCity::setExtraInvestigation(int iValue)
{
	m_iExtraInvestigation = iValue;
}
void CvCity::changeExtraInvestigation(int iChange)
{
	m_iExtraInvestigation += iChange;
}

int CvCity::getSpecialistInsidiousness() const
{
	return m_iSpecialistInsidiousness;
}
void CvCity::changeSpecialistInsidiousness(int iChange)
{
	m_iSpecialistInsidiousness += iChange;
	FASSERT_NOT_NEGATIVE(m_iSpecialistInsidiousness);
}

int CvCity::getSpecialistInvestigation() const
{
	return m_iSpecialistInvestigation;
}
void CvCity::changeSpecialistInvestigation(int iChange)
{
	m_iSpecialistInvestigation += iChange;
	FASSERT_NOT_NEGATIVE(m_iSpecialistInvestigation);
}

int CvCity::getPropertyNeed(PropertyTypes eProperty) const
{
	PROFILE_EXTRA_FUNC();
	if (m_icachedPropertyNeedsTurn != GC.getGame().getGameTurn() || NULL == m_cachedPropertyNeeds)
	{
		const EraTypes eEra = GET_PLAYER(getOwner()).getCurrentEra();

		if (NULL == m_cachedPropertyNeeds)
		{
			m_cachedPropertyNeeds = new int[GC.getNumPropertyInfos()];
			for (int iI = 0; iI < GC.getNumPropertyInfos(); iI++)
			{
				m_cachedPropertyNeeds[iI] = 0;
			}
		}

		for (int iI = 0; iI < GC.getNumPropertyInfos(); iI++)
		{
			const PropertyTypes pProperty = (PropertyTypes)iI;
			if (GC.getPropertyInfo(pProperty).getAIWeight() != 0)
			{
				int iCurrentValue = getPropertiesConst()->getValueByProperty(pProperty);
				int iCurrentChange = getPropertiesConst()->getChangeByProperty(pProperty);
				//TB attempt to allow some modification to need based on existing drift value
				int iCurrentSourceSize = iCurrentChange;//Calvitix remove getTotalBuildingSourcedProperty(eProperty) + getTotalUnitSourcedProperty(eProperty);
				iCurrentSourceSize *= 10; //Evolution for the next 10 turns
				iCurrentValue += iCurrentSourceSize;
				//
				int iTarget = 0;
				if (GC.getPropertyInfo(pProperty).isTargetLevelbyEraType((int)eEra))
				{
					iTarget = GC.getPropertyInfo(pProperty).getTargetLevelbyEraType((int)eEra);
				}
				else
				{
					iTarget = GC.getPropertyInfo(pProperty).getTargetLevel();
				}
				int iNeedori = iTarget - iCurrentValue; 
				int iNeed = iTarget - iCurrentValue;
				CvWString szProperty = GC.getPropertyInfo(pProperty).getType();
				int iAIPropertyWeight = GC.getPropertyInfo(pProperty).getAIWeight() / 50;
				if (iAIPropertyWeight > 0) //Properties that are positive (EDU,TOURISM)
				{ 
					if (iTarget > iCurrentValue) //Still need to be better
					{
						iNeed = (iTarget - iCurrentValue) * iAIPropertyWeight;
					}
					else
					{
						iNeed = -1;
					}		
				}
				else
				{
					if (iTarget < iCurrentValue) //Still need to be better
					{
						iNeed = (iTarget - iCurrentValue) * iAIPropertyWeight;
					}
					else
					{
						iNeed = -1;
					}
				}
				m_cachedPropertyNeeds[iI] = iNeed;//(iNeed * (GC.getPropertyInfo(pProperty).getAIWeight() / 50));
				if (gCityLogLevel > 3)
				{
					logAiEvaluations(3, "	City %S need for Property %S : (Base value : %d, Final with AIWeight : %d)", getName().GetCString(), szProperty.GetCString(), iNeedori, m_cachedPropertyNeeds[iI]);
				}



			}
		}
		m_icachedPropertyNeedsTurn = GC.getGame().getGameTurn();
	}
	int iIndex = (int)eProperty;

	return (m_cachedPropertyNeeds[iIndex]);
}

int CvCity::getNumPropertySpawns() const
{
	return (int)m_aPropertySpawns.size();
}

PropertySpawns& CvCity::getPropertySpawn(int iIndex)
{
	FASSERT_BOUNDS(0, getNumPropertySpawns(), iIndex);
	return m_aPropertySpawns[iIndex];
}

void CvCity::changePropertySpawn(int iChange, PropertyTypes eProperty, UnitTypes eUnit)
{
	PROFILE_EXTRA_FUNC();
	const bool bAdding = (iChange > 0);
	if (bAdding)
	{
		if (NO_PROPERTY != eProperty && NO_UNIT != eUnit)
		{
			PropertySpawns kChange;
			kChange.eProperty = eProperty;
			kChange.eUnit = eUnit;
			m_aPropertySpawns.push_back(kChange);
		}
	}
	else
	{
		std::vector<PropertySpawns> m_aTempPropertySpawns;
		foreach_(const PropertySpawns& it, m_aPropertySpawns)
		{
			if (it.eProperty != eProperty || it.eUnit != eUnit)
			{
				PropertySpawns kChange;
				kChange.eProperty = it.eProperty;
				kChange.eUnit = it.eUnit;
				m_aTempPropertySpawns.push_back(kChange);
			}
		}
		m_aPropertySpawns.clear();
		foreach_(const PropertySpawns& it, m_aTempPropertySpawns)
		{
			PropertySpawns kChange;
			kChange.eProperty = it.eProperty;
			kChange.eUnit = it.eUnit;
			m_aPropertySpawns.push_back(kChange);
		}
		m_aTempPropertySpawns.clear();
	}
}

void CvCity::doPropertyUnitSpawn()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getNumPropertyInfos(); iI++)
	{
		const PropertyTypes eProperty = (PropertyTypes)iI;
		int iCurrentValue = std::max(0, getPropertiesConst()->getValueByProperty(eProperty));
		if (eProperty == 0)
		{
			// TB - SHOULD be crime, but this is subject to flaw if the first property type ever changes.
			//	There's a faster way than getvisual but it takes some setup.
			//	If it becomes necessary to move off of hard coding, use the examples for peaks and hills.
			const int iNumCriminals = plot()->getNumCriminals();
			if (iNumCriminals >= getPopulation() / 2)
			{
				continue;
			}
			iCurrentValue -= iNumCriminals * iCurrentValue / 10;
		}
		const bool bPositiveProperty = GC.getPropertyInfo(eProperty).getAIWeight() >= 0;

		iCurrentValue = std::max(0, iCurrentValue);
		iCurrentValue /= 2;
		const PlayerTypes eSpawnOwner = bPositiveProperty ? getOwner() : (PlayerTypes)BARBARIAN_PLAYER;

		foreach_(const PropertySpawns& it, m_aPropertySpawns)
		{
			if (it.eProperty == eProperty
			&& GC.getGame().getSorenRandNum(10000, "Property Unit Spawn Check") < iCurrentValue)
			{
				const UnitTypes eUnit = it.eUnit;
				if (!GET_PLAYER(getOwner()).canTrain(eUnit, false, false, true, true))
				{
					continue;
				}
				//TBNote:Saving this brilliant example...
				//std::vector<int> aiUnitAIIndex;
				//for (int iJ = 0; iJ < NUM_UNITAI_TYPES; iJ++)
				//{
				//	if (GC.getUnitInfo(eUnit).getUnitAIType(iJ))
				//	{
				//		aiUnitAIIndex.push_back(iJ);
				//	}
				//}
				//int iAIRoll = GC.getGame().getSorenRandNum(aiUnitAIIndex.size(), "Property Unit Spawn AI Check");
				//UnitAITypes eUnitAI = (UnitAITypes)aiUnitAIIndex[iAIRoll];

				FAssertMsg(bPositiveProperty || GC.getUnitInfo(eUnit).isBlendIntoCity(), CvString::format("Trying to spawn %s from property spawn, but it doesn't have bBlendIntoCity enabled, which is a requirement", GC.getUnitInfo(eUnit).getType()).c_str());

				CvUnit* pUnit = GET_PLAYER(eSpawnOwner).initUnit(eUnit, getX(), getY(), UNITAI_BARB_CRIMINAL, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));

				if (pUnit != NULL)
				{
					FAssertMsg(pUnit != NULL, "pUnit is expected to be assigned a valid unit object");
					if (pUnit->isExcile())
					{
						pUnit->jumpToNearestValidPlot(false);
					}
					pUnit->finishMoves();

					addProductionExperience(pUnit);

					if (!GET_PLAYER(getOwner()).isModderOption(MODDEROPTION_IGNORE_DISABLED_ALERTS))
					{
						if (bPositiveProperty)
						{
							AddDLLMessage(
								getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
								gDLL->getText("TXT_KEY_CITY_PROPERTY_SPAWN_FRIENDLY", GC.getUnitInfo(eUnit).getDescription(), getNameKey()),
								NULL, MESSAGE_TYPE_MINOR_EVENT, GC.getUnitInfo(eUnit).getButton(), GC.getCOLOR_HIGHLIGHT_TEXT(), getX(), getY(), true, true
							);
						}
						else
						{
							AddDLLMessage(
								getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
								gDLL->getText("TXT_KEY_CITY_PROPERTY_SPAWN_BARB", GC.getUnitInfo(eUnit).getDescription(), getNameKey()),
								NULL, MESSAGE_TYPE_MINOR_EVENT, GC.getUnitInfo(eUnit).getButton(), GC.getCOLOR_WARNING_TEXT(), getX(), getY(), true, true
							);
						}
					}
				}
			}
		}
	}
}

bool CvCity::isQuarantined() const
{
	return (getQuarantinedCount() > 0);
}

int CvCity::getQuarantinedCount() const
{
	return m_iQuarantinedCount;
}

void CvCity::changeQuarantinedCount(int iChange)
{
	m_iQuarantinedCount += iChange;
}

void CvCity::resetQuarantinedCount()
{
	m_iQuarantinedCount = 0;
}

void CvCity::flushBestSeeInvisibleUnitCache()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getNumInvisibleInfos(); iI++)
	{
		m_aiCachedBestSeeInvisibleUnit[iI] = -1;
		m_abCachedBestSeeInvisibleUnit[iI] = false;
		m_aiCachedBestSeeInvisibleUnitSea[iI] = -1;
		m_abCachedBestSeeInvisibleUnitSea[iI] = false;
	}
}

void CvCity::setBestSeeInvisibleUnitCache(InvisibleTypes eInvisible, UnitTypes eUnitType, bool bSea)
{
	int iI = (int)eInvisible;
	if (bSea)
	{
		m_aiCachedBestSeeInvisibleUnitSea[iI] = (int)eUnitType;
		m_abCachedBestSeeInvisibleUnitSea[iI] = true;
		return;
	}
	m_aiCachedBestSeeInvisibleUnit[iI] = (int)eUnitType;
	m_abCachedBestSeeInvisibleUnit[iI] = true;
}

UnitTypes CvCity::getCachedBestSeeInvisibleUnit(InvisibleTypes eInvisible, bool bSea) const
{
	int iI = (int)eInvisible;
	if (bSea)
	{
		return (UnitTypes)m_aiCachedBestSeeInvisibleUnitSea[iI];
	}
	return (UnitTypes)m_aiCachedBestSeeInvisibleUnit[iI];
}

bool CvCity::isCachedBestSeeInvisibleUnit(InvisibleTypes eInvisible, bool bSea) const
{
	int iI = (int)eInvisible;
	if (bSea)
	{
		return m_abCachedBestSeeInvisibleUnitSea[iI];
	}
	return m_abCachedBestSeeInvisibleUnit[iI];
}

void CvCity::AI_setPropertyControlBuildingQueued(bool bSet)
{
	m_bPropertyControlBuildingQueued = bSet;
}

bool CvCity::AI_isPropertyControlBuildingQueued() const
{
	return m_bPropertyControlBuildingQueued;
}


void CvCity::setWorkerHave(const int iUnitID, const bool bNewValue)
{
	std::vector<int>::iterator itr = find(m_workers.begin(), m_workers.end(), iUnitID);

	if (bNewValue)
	{
		UnitCompWorker* workerComp = GET_PLAYER(getOwner()).getUnit(iUnitID)->getWorkerComponent();
		if (workerComp)
		{
			if (itr == m_workers.end())
			{
				m_workers.push_back(iUnitID);
			}
			else FErrorMsg("Tried to add a duplicate vector element!");

			workerComp->setCityAssignment(getID());
		}
		else FErrorMsg("UnitCompWorker unexpectedly not initialized");
	}
	else if (itr != m_workers.end())
	{
		CvUnit* unitX = GET_PLAYER(getOwner()).getUnit(iUnitID);
		if (unitX)
		{
			UnitCompWorker* workerComp = unitX->getWorkerComponent();
			if (workerComp)
			{
				workerComp->setCityAssignment(-1);
			}
			else FErrorMsg("UnitCompWorker unexpectedly not initialized!");
		}
		else FErrorMsg("m_workers contained an invalid unitID!");

		m_workers.erase(itr);
	}
	else
	{
		FErrorMsg("Vector element to remove was missing!");
		UnitCompWorker* workerComp = GET_PLAYER(getOwner()).getUnit(iUnitID)->getWorkerComponent();
		if (workerComp)
		{
			workerComp->setCityAssignment(-1);
		}
	}
}

void CvCity::processTech(const TechTypes eTech, const int iChange)
{
	if (!GC.getGame().isRecalculatingModifiers())
	{
		updateSpecialistHappinessHealthFromTech();
		{
			const int iBuildingHappinessFromTech = getBuildingHappinessFromTech(eTech);

			if (iBuildingHappinessFromTech != 0)
			{
				m_iExtraBuildingHappinessFromTech += iBuildingHappinessFromTech * iChange;
			}
		}
		m_iExtraBuildingHealthFromTech += getBuildingHealthFromTech(eTech) * iChange;
	}
}


const CityOutputHistory* CvCity::getCityOutputHistory() const
{
	return &m_outputHistory;
}


void CvCity::alterBuildingLedger(const BuildingTypes eType, const bool bAdd, const PlayerTypes eOwner, const int iTime)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eType);

	std::map<BuildingTypes, BuiltBuildingData>::const_iterator itr = m_buildingLedger.find(eType);

	if (itr == m_buildingLedger.end())
	{
		if (bAdd)
		{
			m_bHasBuildings[eType] = true;
			m_hasBuildings.push_back(eType);
			BuiltBuildingData data; 
			data.eBuiltBy = eOwner;
			data.iTimeBuilt = iTime;
			m_buildingLedger.insert(std::make_pair(eType, data));
		}
		else
		{
			FErrorMsg("Trying to remove entry that does not exist!");
		}
	}
	else if (bAdd)
	{
		FErrorMsg("Trying to add a duplicate entry!");
	}
	else 
	{
		m_bHasBuildings[eType] = false;
		m_hasBuildings.erase(find(m_hasBuildings.begin(), m_hasBuildings.end(), eType));
		m_buildingLedger.erase(itr->first);
	}
}

bool CvCity::hasBuilding(const BuildingTypes eType) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eType);
	return m_bHasBuildings[eType];
}

BuiltBuildingData CvCity::getBuildingData(const BuildingTypes eType) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eType);
	FAssert(m_bHasBuildings[eType]);
	if (m_bHasBuildings[eType])
	{
		return m_buildingLedger.find(eType)->second;
	}
	BuiltBuildingData data; 
	data.eBuiltBy = NO_PLAYER;
	data.iTimeBuilt = MIN_INT;
	return data;
}

