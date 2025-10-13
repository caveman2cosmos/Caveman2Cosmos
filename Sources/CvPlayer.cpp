// player.cpp


#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvArea.h"
#include "CvArtFileMgr.h"
#include "CvBuildingInfo.h"
#include "CvBonusInfo.h"
#include "CvCity.h"
#include "CvCityAI.h"
#include "CvContractBroker.h"
#include "CvDeal.h"
#include "CvDiploParameters.h"
#include "CvEventReporter.h"
#include "CvGameAI.h"
#include "CvGameTextMgr.h"
#include "CvGlobals.h"
#include "CvImprovementInfo.h"
#include "CvHeritageInfo.h"
#include "CvInfos.h"
#include "CvInitCore.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvPathGenerator.h"
#include "CvPlayerAI.h"
#include "CvPopupInfo.h"
#include "CvPython.h"
#include "CvSelectionGroupAI.h"
#include "CvTalkingHeadMessage.h"
#include "CvTeamAI.h"
#include "CvUnit.h"
#include "CvViewport.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvDLLEntityIFaceBase.h"
#include "CvDLLFAStarIFaceBase.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLUtilityIFaceBase.h"
#include "CvTraitInfo.h"
#include <boost/scoped_ptr.hpp>

//	Koshling - save flag indicating this player has no data in the save as they have never been alive
#define	PLAYER_UI_FLAG_OMITTED 2

//#define VALIDATION_FOR_PLOT_GROUPS

//	Helper class used to efficiently cache unit upgrade paths for this player
class CvUpgradeCache
{
	struct UpgradePair
	{
		UnitTypes eFrom;
		UnitTypes eTo;

		UpgradePair(UnitTypes eFrom = NO_UNIT, UnitTypes eTo = NO_UNIT) : eFrom(eFrom), eTo(eTo) {}

		// hash function
		operator size_t() const
		{
			return stdext::hash_value(eFrom) ^ stdext::hash_value(eTo);
		}

		bool operator<(const UpgradePair& other) const
		{
			if (eFrom == other.eFrom) return eTo < other.eTo;
			return eFrom < other.eFrom;
		}
	};

public:
	CvUpgradeCache() : m_init(false) {}

	bool upgradeAvailable(UnitTypes eFromUnit, UnitTypes eToUnit)
	{
		PROFILE_FUNC();

		if (!m_init)
		{
			init();
		}
		return m_validUpgrades.find(UpgradePair(eFromUnit, eToUnit)) != m_validUpgrades.end();
	}

private:
	void init()
	{
		PROFILE_FUNC();

		std::vector<UpgradePair> directUpgrades;

		// First populate the direct upgrades
		for(int iUnit = 0; iUnit < GC.getNumUnitInfos(); iUnit++)
		{
			const UnitTypes unitType = static_cast<UnitTypes>(iUnit);
			const CvUnitInfo& unitInfo = GC.getUnitInfo(unitType);
			for(int i = 0; i < unitInfo.getNumUnitUpgrades(); i++)
			{
				directUpgrades.push_back(UpgradePair(unitType, static_cast<UnitTypes>(unitInfo.getUnitUpgrade(i))));
			}
		}

		for(int iUnit = 0; iUnit < GC.getNumUnitInfos(); iUnit++)
		{
			const UnitTypes unitType = static_cast<UnitTypes>(iUnit);
			populateUpgradeChain(unitType, unitType, directUpgrades, m_validUpgrades);
		}
		m_init = true;
	}

	template < class DirectUpgradeCont_, class ValidUpgradeCont_ >
	void populateUpgradeChain(UnitTypes eBaseType, UnitTypes eType, const DirectUpgradeCont_& directUpgrades, ValidUpgradeCont_& validUpgrades)
	{
		PROFILE_EXTRA_FUNC();
		for (DirectUpgradeCont_::const_iterator itr = directUpgrades.begin(); itr != directUpgrades.end(); ++itr)
		{
			if (itr->eFrom == eType)
			{
				validUpgrades.insert(UpgradePair(eBaseType, itr->eTo));
				populateUpgradeChain(eBaseType, itr->eTo, directUpgrades, validUpgrades);
			}
		}
	}

private:
	bool m_init;
	stdext::hash_set<UpgradePair> m_validUpgrades;
};

// Public Functions...

// Public Functions...

//Disable this passed in initialization list warning, as it is only stored in the constructor of CvBuildingList and not used
#pragma warning( disable : 4355 )

CvPlayer::CvPlayer()
: m_GameObject(this),
m_BuildingList(this, NULL),
m_UnitList(this, NULL),
m_Properties(this),
m_pBuildLists(NULL),
m_cachedBonusCount(NULL)
{
	PROFILE_EXTRA_FUNC();
	m_aiSeaPlotYield = new int[NUM_YIELD_TYPES];
	m_aiYieldRateModifier = new int[NUM_YIELD_TYPES];
	m_aiCapitalYieldRateModifier = new int[NUM_YIELD_TYPES];
	m_aiExtraYieldThreshold = new int[NUM_YIELD_TYPES];
	m_aiTradeYieldModifier = new int[NUM_YIELD_TYPES];
	m_aiFreeCityCommerce = new int[NUM_COMMERCE_TYPES];
	m_extraCommerce = new int[NUM_COMMERCE_TYPES];
	m_aiCommercePercent = new int[NUM_COMMERCE_TYPES];
	m_aiCommerceRate = new int[NUM_COMMERCE_TYPES];
	m_abCommerceDirty = new bool[NUM_COMMERCE_TYPES];
	m_aiCommerceRateModifier = new int[NUM_COMMERCE_TYPES];
	m_aiCommerceRateModifierfromEvents = new int[NUM_COMMERCE_TYPES];
	m_aiCommerceRateModifierfromBuildings = new int[NUM_COMMERCE_TYPES];
	m_aiCapitalCommerceRateModifier = new int[NUM_COMMERCE_TYPES];
	m_aiStateReligionBuildingCommerce = new int[NUM_COMMERCE_TYPES];
	m_aiSpecialistExtraCommerce = new int[NUM_COMMERCE_TYPES];
	m_aiSpecialistExtraYield = new int[NUM_YIELD_TYPES];
	m_aiCommerceFlexibleCount = new int[NUM_COMMERCE_TYPES];
	m_aiGoldPerTurnByPlayer = new int[MAX_PLAYERS];
	m_aiEspionageSpendingWeightAgainstTeam = new int[MAX_TEAMS];

	m_abFeatAccomplished = new bool[NUM_FEAT_TYPES];
	m_abOptions = new bool[NUM_PLAYEROPTION_TYPES];

	m_paiImprovementCount = NULL;
	m_paiFeatureHappiness = NULL;
	m_paiBuildingCount = NULL;
	m_paiBuildingGroupCount = NULL;
	m_paiBuildingGroupMaking = NULL;
	m_paiHurryCount = NULL;
	m_paiSpecialBuildingNotRequiredCount = NULL;
	m_paiHasCivicOptionCount = NULL;
	m_paiNoCivicUpkeepCount = NULL;
	m_paiHasReligionCount = NULL;
	m_paiHasCorporationCount = NULL;
	m_paiUpkeepCount = NULL;
	m_paiSpecialistValidCount = NULL;
	m_aiPathLengthCache = NULL;
	m_aiCostPathLengthCache = NULL;
	m_bCanConstruct = NULL;
	m_bCanConstructCached = NULL;
	m_bCanConstructDefaultParam = NULL;
	m_bCanConstructCachedDefaultParam = NULL;
	m_pabResearchingTech = NULL;
	m_pabLoyalMember = NULL;
	m_paeCivics = NULL;
	m_ppaaiSpecialistExtraYield = NULL;
	m_ppaaiImprovementYieldChange = NULL;
	m_pabAutomatedCanBuild = NULL;
	m_ppaaiTerrainYieldChange = NULL;
	m_paiResourceConsumption = NULL;
	m_paiFreeSpecialistCount = NULL;
	m_ppiBuildingCommerceModifier = NULL;
	m_ppiBuildingCommerceChange = NULL;
	m_ppiSpecialistCommercePercentChanges = NULL;
	m_ppiSpecialistYieldPercentChanges = NULL;
	m_ppiBonusCommerceModifier = NULL;
	m_aiLandmarkYield = new int[NUM_YIELD_TYPES];
	m_aiModderOptions = new int[NUM_MODDEROPTION_TYPES];
	m_paiPlayerWideAfflictionCount = NULL;

	m_bHasLanguage = false;

	//TB Traits begin
	m_paiImprovementUpgradeRateModifierSpecific = NULL;
	m_paiBuildWorkerSpeedModifierSpecific = NULL;
	m_ppaaiSpecialistExtraCommerce = NULL;
	m_aiFreeCityYield = new int[NUM_YIELD_TYPES];
	m_pabHasTrait = NULL;
	m_aiLessYieldThreshold = new int[NUM_YIELD_TYPES];

	m_paiNationalDomainFreeExperience = NULL;
	m_paiNationalDomainProductionModifier = NULL;
	m_paiNationalTechResearchModifier = NULL;

	m_paiEraAdvanceFreeSpecialistCount = NULL;

	m_aiGoldenAgeYield = new int[NUM_YIELD_TYPES];
	m_aiGoldenAgeCommerce = new int[NUM_COMMERCE_TYPES];
	//TB Traits end

	m_bDisableHuman = false;
	m_bUnitUpkeepDirty = true;

	m_iStabilityIndex = 500;
	m_iStabilityIndexAverage = 500;

	m_bRebel = false;
	m_iMotherPlayer = -1;
	m_iNationalGreatPeopleRate = 0;

	// Used for DynamicCivNames
	CvWString m_szName;
	CvWString m_szCivDesc;
	CvWString m_szCivShort;
	CvWString m_szCivAdj;

	m_bDoNotBotherStatus = NO_PLAYER;


	// Free Tech Popup Fix
	m_iChoosingFreeTech = 0;
	m_bChoosingReligion = false;

	m_zobristValue = GC.getGame().getSorenRand().getInt();

	m_iNumAnimalsSubdued = 0;
	m_iNumAnarchyTurns = 0;
	m_iNumCivicSwitches = 0;
	m_iNumCivicsSwitched = 0;
	m_iFocusPlotX = -1;
	m_iFocusPlotY = -1;

	m_iMinTaxIncome = 0;
	m_iMaxTaxIncome = 0;

	for (int i = 0; i < NUM_COMMERCE_TYPES; ++i)
	{
		m_cachedTotalCityBaseCommerceRate[i] = MAX_INT;
	}

	for (int i = 0; i < NUM_MAPS; i++)
	{
		m_groupCycles.push_back(new CLinkList<int>);
		m_plotGroups.push_back(new FFreeListTrashArray<CvPlotGroup>);
		m_cities.push_back(new FFreeListTrashArray<CvCityAI>);
		m_units.push_back(new FFreeListTrashArray<CvUnitAI>);
		m_selectionGroups.push_back(new FFreeListTrashArray<CvSelectionGroupAI>);
	}

	reset(NO_PLAYER, true);
}


CvPlayer::~CvPlayer()
{
	PROFILE_EXTRA_FUNC();
	uninit();

	SAFE_DELETE_ARRAY(m_aiSeaPlotYield);
	SAFE_DELETE_ARRAY(m_aiYieldRateModifier);
	SAFE_DELETE_ARRAY(m_aiCapitalYieldRateModifier);
	SAFE_DELETE_ARRAY(m_aiExtraYieldThreshold);
	SAFE_DELETE_ARRAY(m_aiTradeYieldModifier);
	SAFE_DELETE_ARRAY(m_aiFreeCityCommerce);
	SAFE_DELETE_ARRAY(m_extraCommerce);
	SAFE_DELETE_ARRAY(m_aiCommercePercent);
	SAFE_DELETE_ARRAY(m_aiCommerceRate);
	SAFE_DELETE_ARRAY(m_abCommerceDirty);
	SAFE_DELETE_ARRAY(m_aiCommerceRateModifier);
	SAFE_DELETE_ARRAY(m_aiCommerceRateModifierfromEvents);
	SAFE_DELETE_ARRAY(m_aiCommerceRateModifierfromBuildings);
	SAFE_DELETE_ARRAY(m_aiCapitalCommerceRateModifier);
	SAFE_DELETE_ARRAY(m_aiStateReligionBuildingCommerce);
	SAFE_DELETE_ARRAY(m_aiSpecialistExtraCommerce);
	SAFE_DELETE_ARRAY(m_aiSpecialistExtraYield);
	SAFE_DELETE_ARRAY(m_aiCommerceFlexibleCount);
	SAFE_DELETE_ARRAY(m_aiGoldPerTurnByPlayer);
	SAFE_DELETE_ARRAY(m_aiEspionageSpendingWeightAgainstTeam);
	SAFE_DELETE_ARRAY(m_abFeatAccomplished);
	SAFE_DELETE_ARRAY(m_abOptions);

	SAFE_DELETE_ARRAY(m_aiLandmarkYield);
	SAFE_DELETE_ARRAY(m_aiModderOptions);

	//TB Traits begin
	SAFE_DELETE_ARRAY(m_aiFreeCityYield);
	SAFE_DELETE_ARRAY(m_aiLessYieldThreshold);
		//Team Project (6)
	SAFE_DELETE_ARRAY(m_paiEraAdvanceFreeSpecialistCount);
	//Team Project (7)
	SAFE_DELETE_ARRAY(m_aiGoldenAgeYield);
	SAFE_DELETE_ARRAY(m_aiGoldenAgeCommerce);
	//TB Traits end

	SAFE_DELETE_ARRAY(m_cachedBonusCount);

	for (int i = 0; i < NUM_MAPS; i++)
	{
		SAFE_DELETE(m_groupCycles[i]);
		SAFE_DELETE(m_plotGroups[i]);
		SAFE_DELETE(m_cities[i]);
		SAFE_DELETE(m_units[i]);
		SAFE_DELETE(m_selectionGroups[i]);
	}
	//SAFE_DELETE(m_armies);
}


void CvPlayer::baseInit(PlayerTypes eID)
{
	PROFILE_EXTRA_FUNC();
	//--------------------------------
	// Init saved data
	reset(eID);
	//--------------------------------
	// Init containers
	for (int i = 0; i < NUM_MAPS; i++)
	{
		m_plotGroups[i]->init();
		m_cities[i]->init();
		m_units[i]->init();
		m_selectionGroups[i]->init();

	}
#ifdef CVARMY_BREAKSAVE
	m_armies.init(1);
	m_armies.setCurrentID(0);
#endif // CVARMY_BREAKSAVE

	m_eventsTriggered.init();
	//--------------------------------
	// Init non-saved data
	setupGraphical();

	m_UnitList.init();

	FAssert(getTeam() != NO_TEAM);
}

void CvPlayer::initMore(PlayerTypes eID, LeaderHeadTypes ePersonality, bool bSetAlive)
{
	PROFILE_EXTRA_FUNC();
	if (bSetAlive)
	{
		setAlive(true);
	}
	changeBaseFreeUnitUpkeepCivilian(GC.getBASE_FREE_UNITS_UPKEEP_CIVILIAN());
	changeBaseFreeUnitUpkeepMilitary(GC.getBASE_FREE_UNITS_UPKEEP_MILITARY());
	changeFreeUnitUpkeepCivilianPopPercent(GC.getBASE_FREE_UNITS_UPKEEP_CIVILIAN_PER_100_POP());
	changeFreeUnitUpkeepMilitaryPopPercent(GC.getBASE_FREE_UNITS_UPKEEP_MILITARY_PER_100_POP());
	changeTradeRoutes(GC.getINITIAL_TRADE_ROUTES());
	changeStateReligionHappiness(GC.getINITIAL_STATE_RELIGION_HAPPINESS());
	changeNonStateReligionHappiness(GC.getINITIAL_NON_STATE_RELIGION_HAPPINESS());

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		changeTradeYieldModifier(((YieldTypes)iI), GC.getYieldInfo((YieldTypes)iI).getTradeModifier());
	}
	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		setCommercePercent(((CommerceTypes)iI), GC.getCommerceInfo((CommerceTypes)iI).getInitialPercent());
	}

	if (GC.getGame().isOption(GAMEOPTION_AI_RANDOM_PERSONALITIES) && !isNPC() && !isMinorCiv())
	{
		ePersonality = NO_LEADER;
		int iBestValue = 0;

		for (int iI = 0; iI < GC.getNumLeaderHeadInfos(); iI++)
		{
			if (!GC.getLeaderHeadInfo((LeaderHeadTypes)iI).isNPC())
			{
				int iValue = (1 + GC.getGame().getSorenRandNum(10000, "Choosing Personality"));

				for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive()
					&& GET_PLAYER((PlayerTypes)iJ).getPersonalityType() == (LeaderHeadTypes) iI)
					{
						iValue /= 2;
					}
				}
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					ePersonality = (LeaderHeadTypes)iI;
				}
			}
		}
		if (ePersonality != NO_LEADER)
		{
			setPersonalityType(ePersonality);
		}
	}
	FAssertMsg(GC.getNumTraitInfos() > 0, "GC.getNumTraitInfos() is less than or equal to zero but is expected to be larger than zero in CvPlayer::init");
	{
		const int iNumCoreDefaultTraits = GC.getLeaderHeadInfo(ePersonality).getNumDefaultTraits();
		const int iNumDefaultComplexTraits = GC.getLeaderHeadInfo(ePersonality).getNumDefaultComplexTraits();

		if (GC.getGame().isOption(GAMEOPTION_LEADER_COMPLEX_TRAITS) && iNumDefaultComplexTraits > 0)
		{
			for (int iI = 0; iI < iNumDefaultComplexTraits; ++iI)
			{
				const TraitTypes eTrait = TraitTypes(GC.getLeaderHeadInfo(ePersonality).getDefaultComplexTrait(iI));
				if (GC.getTraitInfo(eTrait).isValidTrait(true))
				{
					m_pabHasTrait[eTrait] = true;
					processTrait(eTrait, 1);
				}
			}
		}
		else if (iNumCoreDefaultTraits > 0)
		{
			for (int iI = 0; iI < iNumCoreDefaultTraits; ++iI)
			{
				const TraitTypes eTrait = TraitTypes(GC.getLeaderHeadInfo(ePersonality).getDefaultTrait(iI));
				if (GC.getTraitInfo(eTrait).isValidTrait(true))
				{
					m_pabHasTrait[eTrait] = true;
					processTrait(eTrait, 1);
				}
			}
		}
		else
		{
			for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
			{
				if (GC.getLeaderHeadInfo(ePersonality).hasTrait((TraitTypes)iI) && GC.getTraitInfo((TraitTypes)iI).isValidTrait(true))
				{
					m_pabHasTrait[(TraitTypes)iI] = true;
					processTrait((TraitTypes)iI, 1);
				}
			}
		}
	}
	// Toffer: Someone should look into if it's possible to not set any civics at all for NPC teams.
	// I imagine there's a lot of code that doesn't expect NO_CIVIC to be the set civic though.
	for (int iI = GC.getNumCivicOptionInfos() - 1; iI > -1; iI--)
	{
		setCivics((CivicOptionTypes)iI, (CivicTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationInitialCivics(iI));
	}
}


void CvPlayer::init(PlayerTypes eID)
{
	PROFILE_EXTRA_FUNC();
	baseInit(eID);
	//--------------------------------
	// Init other game data
	GET_TEAM(getTeam()).changeNumMembers(1);

	if (GC.getInitCore().getSlotStatus(getID()) == SS_TAKEN || GC.getInitCore().getSlotStatus(getID()) == SS_COMPUTER)
	{
		initMore(eID, getLeaderType());

		updateMaxAnarchyTurns();
		updateMinAnarchyTurns();

		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			updateExtraYieldThreshold((YieldTypes)iI);
			updateLessYieldThreshold((YieldTypes)iI);
		}

		for (int iI = 0; iI < GC.getNumEventInfos(); iI++)
		{
			resetEventOccured((EventTypes)iI, false);
		}

		for (int iI = 0; iI < GC.getNumEventTriggerInfos(); iI++)
		{
			resetTriggerFired((EventTriggerTypes)iI);
		}
	}

	if (GC.getGame().isOption(GAMEOPTION_LEADER_DEVELOPING))
	{
		setLeaderHeadLevel(0);
	}
	m_contractBroker.init(eID);
	AI_init();
}

//
// Copy of CvPlayer::init but with modifications for use in the middle of a game
//
void CvPlayer::initInGame(PlayerTypes eID, bool bSetAlive)
{
	PROFILE_EXTRA_FUNC();
	baseInit(eID);

	//--------------------------------
	// Init other game data

	// Some effects on team necessary if this is the only member of the team
	int iOtherTeamMembers = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (iI != getID() && GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
		{
			iOtherTeamMembers++;
		}
	}

	bool bTeamInit = false;
	if (iOtherTeamMembers == 0 || GET_TEAM(getTeam()).getNumMembers() == 0)
	{
		bTeamInit = true;
		GET_TEAM(getTeam()).init(getTeam());
		GET_TEAM(getTeam()).resetPlotAndCityData();
	}

	if (bTeamInit || GET_TEAM(getTeam()).getNumMembers() == iOtherTeamMembers)
	{
		GET_TEAM(getTeam()).changeNumMembers(1);
	}

	if ((GC.getInitCore().getSlotStatus(getID()) == SS_TAKEN) || (GC.getInitCore().getSlotStatus(getID()) == SS_COMPUTER))
	{
		initMore(eID, getPersonalityType(), bSetAlive);

		if (!isNPC() && canLeaderPromote())
		{
			do
			{
				doPromoteLeader();
			}
			while (canLeaderPromote());
		}
		updateMaxAnarchyTurns();
		updateMinAnarchyTurns();

		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			updateExtraYieldThreshold((YieldTypes)iI);
			updateLessYieldThreshold((YieldTypes)iI);
		}

		// Reset all triggers at first, set those whose events have fired in next block
		for (int iI = 0; iI < GC.getNumEventTriggerInfos(); iI++)
		{
			resetTriggerFired((EventTriggerTypes)iI);
		}

		for (int iI = 0; iI < GC.getNumEventInfos(); iI++)
		{
			// Has global trigger fired already?
			const EventTriggeredData* pEvent = NULL;
			for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
			{
				if (iJ != getID())
				{
					pEvent = GET_PLAYER((PlayerTypes)iJ).getEventOccured((EventTypes)iI, true);
					if (pEvent && pEvent->m_eTrigger > NO_EVENTTRIGGER)
					{
						const CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(pEvent->m_eTrigger);
						if (kTrigger.isGlobal())
						{
							setTriggerFired( *pEvent, false, false );
							break;
						}
						else if (kTrigger.isTeam() && GET_PLAYER((PlayerTypes)iJ).getTeam() == getTeam())
						{
							setTriggerFired( *pEvent, false, false );
							break;
						}
					}
				}
			}
			resetEventOccured((EventTypes)iI, false);
		}
	}
	resetPlotAndCityData();
	m_contractBroker.init(eID);
	AI_init();
}

//
// Reset all data for this player stored in plot and city objects
//
void CvPlayer::resetPlotAndCityData()
{
	PROFILE_EXTRA_FUNC();
	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); ++iPlot)
	{
		CvPlot* plotX = GC.getMap().plotByIndex(iPlot);

		plotX->setCulture(getID(), 0, false, false);
		plotX->setFoundValue(getID(), 0);

		CvCity* city = plotX->getPlotCity();
		if (city)
		{
			city->setCulture(getID(), 0, false, false);
			city->changeNumRevolts(getID(), -city->getNumRevolts(getID()));
			city->setEverOwned(getID(), false);
			city->setTradeRoute(getID(), false);
		}
	}
}


void CvPlayer::uninit()
{
	PROFILE_EXTRA_FUNC();
	SAFE_DELETE_ARRAY(m_paiImprovementCount);
	SAFE_DELETE_ARRAY(m_paiFeatureHappiness);
	SAFE_DELETE_ARRAY(m_paiBuildingCount);
	SAFE_DELETE_ARRAY(m_paiBuildingGroupCount);
	SAFE_DELETE_ARRAY(m_paiBuildingGroupMaking);
	SAFE_DELETE_ARRAY(m_paiHurryCount);
	SAFE_DELETE_ARRAY(m_paiSpecialBuildingNotRequiredCount);
	SAFE_DELETE_ARRAY(m_paiHasCivicOptionCount);
	SAFE_DELETE_ARRAY(m_paiNoCivicUpkeepCount);
	SAFE_DELETE_ARRAY(m_paiHasReligionCount);
	SAFE_DELETE_ARRAY(m_paiHasCorporationCount);
	SAFE_DELETE_ARRAY(m_paiUpkeepCount);
	SAFE_DELETE_ARRAY(m_paiSpecialistValidCount);

	SAFE_DELETE_ARRAY(m_pabResearchingTech);
	SAFE_DELETE_ARRAY(m_pabLoyalMember);

	SAFE_DELETE_ARRAY(m_paeCivics);
	SAFE_DELETE_ARRAY(m_aiPathLengthCache);
	SAFE_DELETE_ARRAY(m_aiCostPathLengthCache);

	SAFE_DELETE_ARRAY(m_bCanConstruct);
	SAFE_DELETE_ARRAY(m_bCanConstructCached);
	SAFE_DELETE_ARRAY(m_bCanConstructDefaultParam);
	SAFE_DELETE_ARRAY(m_bCanConstructCachedDefaultParam);
	SAFE_DELETE_ARRAY(m_paiPlayerWideAfflictionCount);

	m_upgradeCache.reset();

	SAFE_DELETE(m_pBuildLists);

	SAFE_DELETE_ARRAY2(m_ppaaiSpecialistExtraYield, GC.getNumSpecialistInfos());
	SAFE_DELETE_ARRAY2(m_ppaaiImprovementYieldChange, GC.getNumImprovementInfos());
	SAFE_DELETE_ARRAY(m_pabAutomatedCanBuild);
	SAFE_DELETE_ARRAY(m_paiResourceConsumption);
	SAFE_DELETE_ARRAY(m_paiFreeSpecialistCount);
	SAFE_DELETE_ARRAY(m_paiImprovementUpgradeRateModifierSpecific);
	SAFE_DELETE_ARRAY(m_paiBuildWorkerSpeedModifierSpecific);
	SAFE_DELETE_ARRAY(m_pabHasTrait);
	SAFE_DELETE_ARRAY(m_paiNationalDomainFreeExperience);
	SAFE_DELETE_ARRAY(m_paiNationalDomainProductionModifier);
	SAFE_DELETE_ARRAY(m_paiNationalTechResearchModifier);
	SAFE_DELETE_ARRAY2(m_ppaaiSpecialistExtraCommerce, GC.getNumSpecialistInfos());
	SAFE_DELETE_ARRAY2(m_ppaaiTerrainYieldChange, GC.getNumTerrainInfos());
	SAFE_DELETE_ARRAY2(m_ppiBuildingCommerceModifier, GC.getNumBuildingInfos());
	SAFE_DELETE_ARRAY2(m_ppiBuildingCommerceChange, GC.getNumBuildingInfos());
	SAFE_DELETE_ARRAY2(m_ppiBonusCommerceModifier, GC.getNumBonusInfos());
	SAFE_DELETE_ARRAY2(m_ppiSpecialistCommercePercentChanges, GC.getNumSpecialistInfos());
	SAFE_DELETE_ARRAY2(m_ppiSpecialistYieldPercentChanges, GC.getNumSpecialistInfos());


	m_triggersFired.clear();
	m_mapScoreHistory.clear();
	m_bonusExport.clear();
	m_bonusImport.clear();
	m_bonusMintedPercent.clear();
	m_unitCount.clear();
	m_unitCountSM.clear();
	m_unitMaking.clear();
	m_greatGeneralPointsType.clear();
	m_goldenAgeOnBirthOfGreatPersonCount.clear();
	m_greatPeopleRateforUnit.clear();
	m_buildingMaking.clear();
	m_freeBuildingCount.clear();
	m_extraBuildingHappiness.clear();
	m_extraBuildingHealth.clear();
	m_buildingProductionMod.clear();
	m_buildingCostMod.clear();
	m_unitProductionMod.clear();
	m_unitCombatProductionMod.clear();
	m_unitCombatFreeXP.clear();
	m_researchQueue.clear();
	m_cityNames.clear();
	m_myHeritage.clear();

	m_contractBroker.reset();

	for (int i = 0; i < NUM_MAPS; i++)
	{
		m_groupCycles[i]->clear();
		m_plotGroups[i]->uninit();
		m_cities[i]->uninit();
		m_units[i]->uninit();
		m_selectionGroups[i]->uninit();
	}
	m_eventsTriggered.uninit();

	m_armies.uninit();


	clearMessages();

	clearPopups();

	clearDiplomacy();

	if (GC.getGame().isOption(GAMEOPTION_LEADER_DEVELOPING))
	{
		setLeaderHeadLevel(0);
	}
}


// FUNCTION: reset()
// Initializes data members that are serialized.
void CvPlayer::reset(PlayerTypes eID, bool bConstructorCall)
{
	PROFILE_EXTRA_FUNC();
	int iI, iJ;

	//--------------------------------
	// Uninit class
	uninit();

	algo::fill(m_startingCoords, XYCoords());
	m_iTotalPopulation = 0;
	m_iTotalLand = 0;
	m_iTotalLandScored = 0;
	m_iGold = 0;
	m_iGoldPerTurn = 0;
	m_iAdvancedStartPoints = -1;
	m_iGoldenAgeTurns = 0;
	m_iNumUnitGoldenAges = 0;
	m_iStrikeTurns = 0;
	m_iAnarchyTurns = 0;
	m_iMaxAnarchyTurns = 0;
	m_iMinAnarchyTurns = 0;
	m_iAnarchyModifier = 0;
	m_iGoldenAgeModifier = 0;
	m_iGlobalHurryModifier = 0;
	m_iGreatPeopleCreated = 0;
	m_iGreatGeneralsCreated = 0;
	m_iGreatPeopleThresholdModifier = 0;
	m_iGreatGeneralsThresholdModifier = 0;
	m_iGreatPeopleRateModifier = 0;
	m_iGreatGeneralRateModifier = 0;
	m_iDomesticGreatGeneralRateModifier = 0;
	m_iStateReligionGreatPeopleRateModifier = 0;
	m_iMaxGlobalBuildingProductionModifier = 0;
	m_iMaxTeamBuildingProductionModifier = 0;
	m_iMaxPlayerBuildingProductionModifier = 0;
	m_iFreeExperience = 0;
	m_iFeatureProductionModifier = 0;
	m_iWorkerSpeedModifier = 0;
	m_iImprovementUpgradeRateModifier = 0;
	m_iMilitaryProductionModifier = 0;
	m_iSpaceProductionModifier = 0;
	m_iCityDefenseModifier = 0;

	m_iNonStateReligionCommerceCount = 0;
	m_iUpgradeAnywhereCount = 0;
	m_iRevIdxLocal = 0;
	m_iRevIdxNational = 0;
	m_iRevIdxDistanceModifier = 0;
	m_iRevIdxHolyCityGood = 0;
	m_iRevIdxHolyCityBad = 0;
	m_fRevIdxNationalityMod = 0;
	m_fRevIdxBadReligionMod = 0;
	m_fRevIdxGoodReligionMod = 0;
	m_bInquisitionConditions = false;
	m_iUnitUpgradePriceModifier = 0;
	m_iCityLimit = 0;
	m_iCityOverLimitUnhappy = 0;
	m_iForeignUnhappyPercent = 0;

	m_iNumNukeUnits = 0;
	m_iNumOutsideUnits = 0;
	m_iBaseFreeUnitUpkeepCivilian = 0;
	m_iBaseFreeUnitUpkeepMilitary = 0;
	m_iFreeUnitUpkeepCivilianPopPercent = 0;
	m_iFreeUnitUpkeepMilitaryPopPercent = 0;

	m_iCivilianUnitUpkeepMod = 0;
	m_iMilitaryUnitUpkeepMod = 0;
	m_iUnitUpkeepMilitary100 = 0;
	m_iUnitUpkeepCivilian100 = 0;
	m_iFinalUnitUpkeep = 0;

	m_iNumMilitaryUnits = 0;
	m_iHappyPerMilitaryUnit = 0;
	m_iMilitaryFoodProductionCount = 0;
	m_iConscriptCount = 0;
	m_iMaxConscript = 0;
	m_iHighestUnitLevel = 1;
	m_iOverflowResearch = 0;
	m_iNoUnhealthyPopulationCount = 0;
	m_iExpInBorderModifier = 0;
	m_iBuildingOnlyHealthyCount = 0;

	m_iMaintenanceModifier = 0;
	m_iCoastalDistanceMaintenanceModifier = 0;
	m_iConnectedCityMaintenanceModifier = 0;
	m_iDistanceMaintenanceModifier = 0;
	m_iNumCitiesMaintenanceModifier = 0;
	m_iCorporationMaintenanceModifier = 0;
	m_iHomeAreaMaintenanceModifier = 0;
	m_iOtherAreaMaintenanceModifier = 0;
	m_iTotalMaintenance = 0;

	m_iUpkeepModifier = 0;
	m_iLevelExperienceModifier = 0;
	m_iExtraHealth = 0;
	m_iCivicHealth = 0;
	m_iBuildingGoodHealth = 0;
	m_iBuildingBadHealth = 0;
	m_iExtraHappiness = 0;
	m_iExtraHappinessUnattributed = 0;
	m_iBuildingHappiness = 0;
	m_iLargestCityHappiness = 0;
	m_iWarWearinessPercentAnger = 0;
	m_iWarWearinessModifier = 0;
	m_iFreeSpecialist = 0;
	m_iNoForeignTradeCount = 0;
	m_iNoCorporationsCount = 0;
	m_iNoForeignCorporationsCount = 0;
	m_iCoastalTradeRoutes = 0;
	m_iTradeRoutes = 0;
	m_iRevolutionTimer = 0;
	m_iConversionTimer = 0;
	m_iStateReligionCount = 0;
	m_iNoNonStateReligionSpreadCount = 0;
	m_iStateReligionHappiness = 0;
	m_iNonStateReligionHappiness = 0;
	m_iStateReligionUnitProductionModifier = 0;
	m_iStateReligionBuildingProductionModifier = 0;
	m_iStateReligionFreeExperience = 0;
	m_iCapitalCityID = FFreeList::INVALID_INDEX;
	m_iCitiesLost = 0;
	m_iWinsVsBarbs = 0;
	m_iAssets = 0;
	m_iPower = 0;
	m_iTechPower = 0;
	m_iUnitPower = 0;
	m_iPopulationScore = 0;
	m_iLandScore = 0;
	m_iTechScore = 0;
	m_iWondersScore = 0;
	m_iCombatExperience = 0;
	m_iPopRushHurryCount = 0;
	m_iInflationModifier = 0;
	m_uiStartTime = 0;

	m_bAlive = false;
	m_bEverAlive = false;
	m_bTurnActive = false;
	m_bAutoMoves = false;
	m_bEndTurn = false;
	m_bForcedCityCycle = false;
	m_bPbemNewTurn = false;
	m_bExtendedGame = false;
	m_bFoundedFirstCity = false;
	m_bStrike = false;
	//TB Nukefix
	m_bNukesValid = false;
	m_bHuman = false;
	//TB Traits begin
	m_iCivicAnarchyModifier = 0;
	m_iReligiousAnarchyModifier = 0;
	m_iAIAttitudeModifier = 0;
	m_iTraitExtraCityDefense = 0;
	m_iLeaderHeadLevel = 0;
	m_iNationalEspionageDefense = 0;
	m_iInquisitionCount = 0;
	m_iMaxTradeRoutesAdjustment = 0;
	m_iNationalHurryAngerModifier = 0;
	m_iNationalEnemyWarWearinessModifier = 0;
	m_iNationalBombardDefenseModifier = 0;
	m_iFixedBordersCount = 0;
	m_iFreedomFighterCount = 0;
	m_iExtraFreedomFighters = 0;

	m_iNationalCityStartCulture = 0;
	m_iNationalAirUnitCapacity = 0;
	m_iCapitalXPModifier = 0;
	m_iStateReligionHolyCityXPModifier = 0;
	m_iNonStateReligionHolyCityXPModifier = 0;
	m_iNationalCityStartBonusPopulation = 0;
	m_iNationalMissileRangeChange = 0;
	m_iNationalFlightOperationRangeChange = 0;
	m_iNationalNavalCargoSpaceChange = 0;
	m_iNationalMissileCargoSpaceChange = 0;
	m_iCitiesStartwithStateReligionCount = 0;
	m_iDraftsOnCityCaptureCount = 0;
	m_iFreeSpecialistperWorldWonderCount = 0;
	m_iFreeSpecialistperNationalWonderCount = 0;
	m_iFreeSpecialistperTeamProjectCount = 0;
	m_iExtraGoodyCount = 0;

	m_iAllReligionsActiveCount = 0;

	m_iExtraNationalCaptureProbabilityModifier = 0;
	m_iExtraNationalCaptureResistanceModifier = 0;

	m_iExtraStateReligionSpreadModifier = 0;
	m_iExtraNonStateReligionSpreadModifier = 0;
	m_iNationalGreatPeopleRate = 0;
	//TB Traits end
	m_iBaseMergeSelection = FFreeList::INVALID_INDEX;
	m_iFirstMergeSelection = FFreeList::INVALID_INDEX;
	m_iSecondMergeSelection = FFreeList::INVALID_INDEX;
	m_iSplittingUnit = FFreeList::INVALID_INDEX;
	m_iAmbushingUnit = FFreeList::INVALID_INDEX;
	m_bAssassinate = false;

	m_contractBroker.reset();
	m_Properties.clear();
	m_canHaveBuilder.clear();
	m_civicSwitchHistory.clear();

	setTurnHadUIInteraction(false);

	// Free Tech Popup Fix
	m_iChoosingFreeTech = 0;

	m_bDisableHuman = false;
	m_bUnitUpkeepDirty = true;

	m_iStabilityIndex = 500;
	m_iStabilityIndexAverage = 500;

	m_bRebel = false;
	m_iMotherPlayer = -1;

	// Used for DynamicCivNames
	m_szName.clear();
	m_szCivDesc.clear();
	m_szCivShort.clear();
	m_szCivAdj.clear();

	m_eID = eID;
	updateTeamType();
	updateHuman();

	if (m_eID != NO_PLAYER)
	{
		m_ePersonalityType = GC.getInitCore().getLeader(m_eID); //??? Is this repeated data???
	}
	else
	{
		m_ePersonalityType = NO_LEADER;
	}
	m_eCurrentEra = ((EraTypes)0);  //??? Is this repeated data???
	m_eLastStateReligion = NO_RELIGION;
	m_eParent = NO_PLAYER;

	m_ePledgedVote = NO_PLAYER_VOTE;
	m_eSecretaryGeneralVote = NO_TEAM;
	m_eGreatGeneralTypetoAssign = NO_UNIT;
	m_iEnslavementChance = 0;
	m_iForeignTradeRouteModifier = 0;
	m_iTaxRateUnhappiness = 0;
	m_iCivicHappiness = 0;
	m_iUnitUpgradePriceModifier = 0;
	m_iNationalGreatPeopleRate = 0;

	// Afforess Food Threshold Modifier 08/16/09
	m_fPopulationgrowthratepercentageLog = 0;

	m_iExtraCityDefense = 0;
	m_iDistantUnitSupportCostModifier = 0;
	m_iReligionSpreadRate = 0;

	m_iWorldHappiness = 0;
	m_iProjectHappiness = 0;
	m_iWorldHealth = 0;
	m_iProjectHealth = 0;
	m_iForceAllTradeRoutes = 0;
	m_iNoCapitalUnhappiness = 0;
	m_iCivilizationHealth = 0;

	m_bShowLandmarks = true;

	m_iBuildingInflation = 0;
	m_iCivicInflation = 0;
	m_iProjectInflation = 0;
	m_iTechInflation = 0;

	m_iHurryCostModifier = 0;
	m_iHurryInflationModifier = 0;
	m_iHurryCount = 0;

	m_eBestRoute = NO_ROUTE;

	m_iNoLandmarkAngerCount = 0;
	m_iLandmarkHappiness = 0;
	m_eDemandWarAgainstTeam = NO_TEAM;

	m_iCorporationSpreadModifier = 0;
	// @SAVEBREAK - delete
	m_iCorporateTaxIncome = 0;
	// !SAVEBREAK
	m_iCorporateMaintenance = 0;

	m_iCulture = 0;

	m_iUpgradeRoundCount = 0;
	m_iSelectionRegroup = 0;

	m_iFractionalCombatExperience = 0;
	m_iNationalGreatPeopleRate = 0;

	m_bInhibitPlotGroupRecalc = false;

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiSeaPlotYield[iI] = 0;
		m_aiYieldRateModifier[iI] = 0;
		m_aiCapitalYieldRateModifier[iI] = 0;
		m_aiExtraYieldThreshold[iI] = 0;
		m_aiTradeYieldModifier[iI] = 0;
		m_aiLandmarkYield[iI] = 0;

		//TB Traits begin
		m_aiFreeCityYield[iI] = 0;
		m_aiSpecialistExtraYield[iI] = 0;
		m_aiLessYieldThreshold[iI] = 0;
		m_aiGoldenAgeYield[iI] = 0;
		//TB Traits end
	}

	for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		m_aiCommercePercent[iI] = 0;
		m_aiFreeCityCommerce[iI] = 0;
		m_extraCommerce[iI] = 0;
		m_aiCommerceRate[iI] = 0;
		m_abCommerceDirty[iI] = false;
		m_aiCommerceRateModifier[iI] = 0;
		m_aiCommerceRateModifierfromEvents[iI] = 0;
		m_aiCommerceRateModifierfromBuildings[iI] = 0;
		m_aiCapitalCommerceRateModifier[iI] = 0;
		m_aiStateReligionBuildingCommerce[iI] = 0;
		m_aiSpecialistExtraCommerce[iI] = 0;
		m_aiCommerceFlexibleCount[iI] = 0;
		m_aiGoldenAgeCommerce[iI] = 0;
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aiGoldPerTurnByPlayer[iI] = 0;
		if (!bConstructorCall && getID() != NO_PLAYER)
		{
			GET_PLAYER((PlayerTypes) iI).m_aiGoldPerTurnByPlayer[getID()] = 0;
		}
	}

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiEspionageSpendingWeightAgainstTeam[iI] = 0;

		if (!bConstructorCall && getTeam() != NO_TEAM)
		{
			for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
			{
				if (GET_PLAYER((PlayerTypes) iJ).getTeam() == iI)
				{
					GET_PLAYER((PlayerTypes) iJ).setEspionageSpendingWeightAgainstTeam(getTeam(), 0);
				}
			}
		}
	}

	for (iI = 0; iI < NUM_FEAT_TYPES; iI++)
	{
		m_abFeatAccomplished[iI] = false;
	}

	for (iI = 0; iI < NUM_PLAYEROPTION_TYPES; iI++)
	{
		m_abOptions[iI] = false;
	}

	m_szScriptData = "";

	SAFE_DELETE_ARRAY(m_cachedBonusCount);
	m_cachedBonusCountGameTurn = -1;

	if (!bConstructorCall)
	{
		FAssertMsg(0 < GC.getNumImprovementInfos(), "GC.getNumImprovementInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		FAssertMsg(m_paiImprovementCount==NULL, "about to leak memory, CvPlayer::m_paiImprovementCount");
		m_paiImprovementCount = new int [GC.getNumImprovementInfos()];
		for (iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			m_paiImprovementCount[iI] = 0;
		}

		FAssertMsg(m_paiFeatureHappiness==NULL, "about to leak memory, CvPlayer::m_paiFeatureHappiness");
		m_paiFeatureHappiness = new int [GC.getNumFeatureInfos()];
		for (iI = 0; iI < GC.getNumFeatureInfos(); iI++)
		{
			m_paiFeatureHappiness[iI] = 0;
		}

		FAssertMsg(0 < GC.getNumBuildingInfos(), "GC.getNumBuildingInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		FAssertMsg(m_paiBuildingCount==NULL, "about to leak memory, CvPlayer::m_paiBuildingCount");
		m_paiBuildingCount = new int [GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_paiBuildingCount[iI] = 0;
		}

		FAssertMsg(m_paiBuildingGroupCount==NULL, "about to leak memory, CvPlayer::m_paiBuildingGroupCount");
		m_paiBuildingGroupCount = new int [GC.getNumSpecialBuildingInfos()];
		FAssertMsg(m_paiBuildingGroupMaking==NULL, "about to leak memory, CvPlayer::m_paiBuildingGroupMaking");
		m_paiBuildingGroupMaking = new int [GC.getNumSpecialBuildingInfos()];
		for (iI = 0; iI < GC.getNumSpecialBuildingInfos(); iI++)
		{
			m_paiBuildingGroupCount[iI] = 0;
			m_paiBuildingGroupMaking[iI] = 0;
		}

		FAssertMsg(m_paiHurryCount==NULL, "about to leak memory, CvPlayer::m_paiHurryCount");
		m_paiHurryCount = new int [GC.getNumHurryInfos()];
		for (iI = 0; iI < GC.getNumHurryInfos(); iI++)
		{
			m_paiHurryCount[iI] = 0;
		}

		FAssertMsg(m_paiSpecialBuildingNotRequiredCount==NULL, "about to leak memory, CvPlayer::m_paiSpecialBuildingNotRequiredCount");
		m_paiSpecialBuildingNotRequiredCount = new int [GC.getNumSpecialBuildingInfos()];
		for (iI = 0; iI < GC.getNumSpecialBuildingInfos(); iI++)
		{
			m_paiSpecialBuildingNotRequiredCount[iI] = 0;
		}

		FAssertMsg(m_paiHasCivicOptionCount==NULL, "about to leak memory, CvPlayer::m_paiHasCivicOptionCount");
		m_paiHasCivicOptionCount = new int[GC.getNumCivicOptionInfos()];
		FAssertMsg(m_paiNoCivicUpkeepCount==NULL, "about to leak memory, CvPlayer::m_paiNoCivicUpkeepCount");
		m_paiNoCivicUpkeepCount = new int[GC.getNumCivicOptionInfos()];
		FAssertMsg(m_paeCivics==NULL, "about to leak memory, CvPlayer::m_paeCivics");
		m_paeCivics = new CivicTypes [GC.getNumCivicOptionInfos()];
		for (iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
		{
			m_paiHasCivicOptionCount[iI] = 0;
			m_paiNoCivicUpkeepCount[iI] = 0;
			m_paeCivics[iI] = NO_CIVIC;
		}


		FAssertMsg(m_paiPlayerWideAfflictionCount==NULL, "about to leak memory, CvPlayer::m_paiPlayerWideAfflictionCount");
		m_paiPlayerWideAfflictionCount = new int[GC.getNumPromotionLineInfos()];

		for (iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
		{
			m_paiPlayerWideAfflictionCount[iI] = 0;
		}

		FAssertMsg(m_bCanConstruct==NULL, "about to leak memory, CvPlayer::m_bCanConstruct");
		m_bCanConstruct = new bool[GC.getNumBuildingInfos()];

		FAssertMsg(m_bCanConstructCached==NULL, "about to leak memory, CvPlayer::m_bCanConstructCached");
		m_bCanConstructCached = new bool[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_bCanConstructCached[iI] = false;
		}

		FAssertMsg(m_bCanConstructDefaultParam==NULL, "about to leak memory, CvPlayer::m_bCanConstructDefaultParam");
		m_bCanConstructDefaultParam = new bool[GC.getNumBuildingInfos()];

		FAssertMsg(m_bCanConstructCachedDefaultParam==NULL, "about to leak memory, CvPlayer::m_bCanConstructCachedDefaultParam");
		m_bCanConstructCachedDefaultParam = new bool[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_bCanConstructCachedDefaultParam[iI] = false;
		}

		FAssertMsg(m_aiPathLengthCache==NULL, "about to leak memory, CvPlayer::m_aiPathLengthCache");
		m_aiPathLengthCache = new int[GC.getNumTechInfos()];
		for (iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			m_aiPathLengthCache[iI] = -1;
		}

		FAssertMsg(m_aiCostPathLengthCache==NULL, "about to leak memory, CvPlayer::m_aiCostPathLengthCache");
		m_aiCostPathLengthCache = new int[GC.getNumTechInfos()];
		for (iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			m_aiCostPathLengthCache[iI] = -1;
		}

		FAssertMsg(m_paiHasReligionCount==NULL, "about to leak memory, CvPlayer::m_paiHasReligionCount");
		m_paiHasReligionCount = new int[GC.getNumReligionInfos()];
		for (iI = 0;iI < GC.getNumReligionInfos();iI++)
		{
			m_paiHasReligionCount[iI] = 0;
		}

		FAssertMsg(m_paiHasCorporationCount==NULL, "about to leak memory, CvPlayer::m_paiHasCorporationCount");
		m_paiHasCorporationCount = new int[GC.getNumCorporationInfos()];
		for (iI = 0;iI < GC.getNumCorporationInfos();iI++)
		{
			m_paiHasCorporationCount[iI] = 0;
		}

		FAssertMsg(m_pabResearchingTech==NULL, "about to leak memory, CvPlayer::m_pabResearchingTech");
		m_pabResearchingTech = new bool[GC.getNumTechInfos()];
		for (iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			m_pabResearchingTech[iI] = false;
		}

		FAssertMsg(m_pabLoyalMember==NULL, "about to leak memory, CvPlayer::m_pabLoyalMember");
		m_pabLoyalMember = new bool[GC.getNumVoteSourceInfos()];
		for (iI = 0; iI < GC.getNumVoteSourceInfos(); iI++)
		{
			m_pabLoyalMember[iI] = true;
		}

		FAssertMsg(0 < GC.getNumUpkeepInfos(), "GC.getNumUpkeepInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		FAssertMsg(m_paiUpkeepCount==NULL, "about to leak memory, CvPlayer::m_paiUpkeepCount");
		m_paiUpkeepCount = new int[GC.getNumUpkeepInfos()];
		for (iI = 0; iI < GC.getNumUpkeepInfos(); iI++)
		{
			m_paiUpkeepCount[iI] = 0;
		}

		FAssertMsg(0 < GC.getNumSpecialistInfos(), "GC.getNumSpecialistInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		FAssertMsg(m_paiSpecialistValidCount==NULL, "about to leak memory, CvPlayer::m_paiSpecialistValidCount");
		m_paiSpecialistValidCount = new int[GC.getNumSpecialistInfos()];
		for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			m_paiSpecialistValidCount[iI] = 0;
		}

		FAssertMsg(0 < GC.getNumSpecialistInfos(), "GC.getNumSpecialistInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		FAssertMsg(m_ppaaiSpecialistExtraYield==NULL, "about to leak memory, CvPlayer::m_ppaaiSpecialistExtraYield");
		m_ppaaiSpecialistExtraYield = new int*[GC.getNumSpecialistInfos()];
		m_paiFreeSpecialistCount = new int[GC.getNumSpecialistInfos()];

		for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			m_ppaaiSpecialistExtraYield[iI] = new int[NUM_YIELD_TYPES];
			for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				m_ppaaiSpecialistExtraYield[iI][iJ] = 0;
			}
			m_paiFreeSpecialistCount[iI] = 0;
		}
		//TB Traits begin
		FAssertMsg(0 < GC.getNumSpecialistInfos(), "GC.getNumSpecialistInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		FAssertMsg(m_ppaaiSpecialistExtraCommerce==NULL, "about to leak memory, CvPlayer::m_ppaaiSpecialistExtraCommerce");
		m_ppaaiSpecialistExtraCommerce = new int*[GC.getNumSpecialistInfos()];
		for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			m_ppaaiSpecialistExtraCommerce[iI] = new int[NUM_COMMERCE_TYPES];
			for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				m_ppaaiSpecialistExtraCommerce[iI][iJ] = 0;
			}
		}

		FAssertMsg(m_pabHasTrait==NULL, "about to leak memory, CvPlayer::m_pabHasTrait");
		m_pabHasTrait = new bool[GC.getNumTraitInfos()];
		for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
			m_pabHasTrait[iI] = false;
		}

		//TB Traits end
		FAssertMsg(m_ppaaiImprovementYieldChange==NULL, "about to leak memory, CvPlayer::m_ppaaiImprovementYieldChange");
		m_ppaaiImprovementYieldChange = new int*[GC.getNumImprovementInfos()];
		//TB Traits begin
		FAssertMsg(m_paiImprovementUpgradeRateModifierSpecific==NULL, "about to leak memory, CvPlayer::m_paiImprovementUpgradeRateModifierSpecific");
		m_paiImprovementUpgradeRateModifierSpecific = new int [GC.getNumImprovementInfos()];
		//TB Traits end
		for (iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			m_ppaaiImprovementYieldChange[iI] = new int[NUM_YIELD_TYPES];
			for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				m_ppaaiImprovementYieldChange[iI][iJ] = 0;
			}
			//TB Traits begin
			m_paiImprovementUpgradeRateModifierSpecific[iI] = 0;
			//TB Traits end
		}

		STATIC_ASSERT(NUM_DOMAIN_TYPES > 0, value_should_be_greater_than_zero);
		m_paiNationalDomainFreeExperience = new int[NUM_DOMAIN_TYPES];
		m_paiNationalDomainProductionModifier = new int[NUM_DOMAIN_TYPES];
		for (iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
		{
			m_paiNationalDomainFreeExperience[iI] = 0;
			m_paiNationalDomainProductionModifier[iI] = 0;
		}

		FAssertMsg((0 < GC.getNumTechInfos()),  "GC.getNumTechInfos() is not greater than zero but an array is being allocated in CvCity::reset");
		m_paiNationalTechResearchModifier = new int[GC.getNumTechInfos()];
		for (iI = 0;iI < GC.getNumTechInfos();iI++)
		{
			m_paiNationalTechResearchModifier[iI] = 0;
		}

		FAssertMsg(m_pabAutomatedCanBuild==NULL, "about to leak memory, CvPlayer::m_pabAutomatedCanBuild");
		m_pabAutomatedCanBuild = new bool [GC.getNumBuildInfos()];
		//TB Traits begin
		FAssertMsg(m_paiBuildWorkerSpeedModifierSpecific==NULL, "about to leak memory, CvPlayer::m_paiBuildWorkerSpeedModifierSpecific");
		m_paiBuildWorkerSpeedModifierSpecific = new int [GC.getNumBuildInfos()];
		//TB Traits end
		for (iI = 0; iI < GC.getNumBuildInfos(); iI++)
		{
			m_pabAutomatedCanBuild[iI] = true;
			//TB Traits begin
			m_paiBuildWorkerSpeedModifierSpecific[iI] = 0;
			//TB Traits end
		}

		FAssertMsg(0 < GC.getNumBonusInfos(), "GC.getNumBonusInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		FAssertMsg(m_paiResourceConsumption==NULL, "about to leak memory, CvPlayer::m_paiResourceConsumption");
		m_paiResourceConsumption = new int [GC.getNumBonusInfos()];
		for (iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			m_paiResourceConsumption[iI] = 0;
		}

		FAssertMsg(m_ppaaiTerrainYieldChange==NULL, "about to leak memory, CvPlayer::m_ppaaiTerrainYieldChange");
		m_ppaaiTerrainYieldChange = new int*[GC.getNumTerrainInfos()];
		for (iI = 0; iI < GC.getNumTerrainInfos(); iI++)
		{
			m_ppaaiTerrainYieldChange[iI] = new int[NUM_YIELD_TYPES];
			for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				m_ppaaiTerrainYieldChange[iI][iJ] = 0;
			}
		}
		FAssertMsg(m_ppiBuildingCommerceModifier==NULL, "about to leak memory, CvPlayer::m_ppiBuildingCommerceModifier");
		m_ppiBuildingCommerceModifier = new int*[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_ppiBuildingCommerceModifier[iI] = new int[NUM_COMMERCE_TYPES];
			for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				m_ppiBuildingCommerceModifier[iI][iJ] = 0;
			}
		}
		FAssertMsg(m_ppiBuildingCommerceChange==NULL, "about to leak memory, CvPlayer::m_ppiBuildingCommerceChange");
		m_ppiBuildingCommerceChange = new int*[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_ppiBuildingCommerceChange[iI] = new int[NUM_COMMERCE_TYPES];
			for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				m_ppiBuildingCommerceChange[iI][iJ] = 0;
			}
		}

		FAssertMsg(m_ppiBonusCommerceModifier==NULL, "about to leak memory, CvPlayer::m_ppiBonusCommerceModifier");
		m_ppiBonusCommerceModifier = new int*[GC.getNumBonusInfos()];
		for (iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			m_ppiBonusCommerceModifier[iI] = new int[NUM_COMMERCE_TYPES];
			for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				m_ppiBonusCommerceModifier[iI][iJ] = 0;
			}
		}

		FAssertMsg(m_ppiSpecialistCommercePercentChanges==NULL, "about to leak memory, CvPlayer::m_ppiSpecialistCommercePercentChanges");
		FAssertMsg(m_ppiSpecialistYieldPercentChanges==NULL, "about to leak memory, CvPlayer::m_ppiSpecialistYieldPercentChanges");
		m_ppiSpecialistCommercePercentChanges = new int*[GC.getNumSpecialistInfos()];
		m_ppiSpecialistYieldPercentChanges = new int*[GC.getNumSpecialistInfos()];
		m_paiEraAdvanceFreeSpecialistCount = new int[GC.getNumSpecialistInfos()];

		for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			m_ppiSpecialistCommercePercentChanges[iI] = new int[NUM_COMMERCE_TYPES];
			for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				m_ppiSpecialistCommercePercentChanges[iI][iJ] = 0;
			}
			m_ppiSpecialistYieldPercentChanges[iI] = new int[NUM_YIELD_TYPES];
			for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				m_ppiSpecialistYieldPercentChanges[iI][iJ] = 0;
			}
			m_paiEraAdvanceFreeSpecialistCount[iI] = 0;
		}

		for (iI = 0; iI < NUM_MODDEROPTION_TYPES; iI++)
		{
			m_aiModderOptions[iI] = 0;
		}
		m_mapEventsOccured.clear();
		m_mapEventCountdown.clear();
		m_aFreeUnitCombatPromotions.clear();
		m_aFreeUnitPromotions.clear();
		m_aVote.clear();
		m_aUnitExtraCosts.clear();
		m_triggersFired.clear();
	}
	for (int i = 0; i < NUM_MAPS; i++)
	{
		m_plotGroups[i]->removeAll();
		m_cities[i]->removeAll();
		m_units[i]->removeAll();
		m_selectionGroups[i]->removeAll();
	}
	m_pTempUnit = NULL;

	m_eventsTriggered.removeAll();

	if (!bConstructorCall)
	{
		m_BuildingList.init();
		AI_reset(false);
	}

	m_bDoNotBotherStatus = NO_PLAYER;

	m_UnitList.init();

	m_iNumAnimalsSubdued = 0;
	m_iNumAnarchyTurns = 0;
	m_iNumCivicSwitches = 0;
	m_iNumCivicsSwitched = 0;
	m_unitConstructionCounts.clear(); // Toffer - Remove @SAVEBREAK as it is only used for logging purposes, not particularly interesting logging.

	m_iMinTaxIncome = 0;
	m_iMaxTaxIncome = 0;

	m_bMaintenanceDirty = false;
	m_orbitalInfrastructureCountDirty = true;
	m_iFocusPlotX = -1;
	m_iFocusPlotY = -1;

	m_commanders.clear();
	m_commandFieldPlots.clear();
	m_commodores.clear();
	m_commodoreFieldPlots.clear();
	m_idleCities.clear();
	m_myHeritage.clear();
}


void CvPlayer::changePersonalityType()
{
	PROFILE_EXTRA_FUNC();
	if (GC.getGame().isOption(GAMEOPTION_AI_RANDOM_PERSONALITIES))
	{
		if (!isNPC())
		{
			LeaderHeadTypes eBestPersonality = NO_LEADER;
			int iBestValue = 0;

			for (int iI = 0; iI < GC.getNumLeaderHeadInfos(); iI++)
			{
				if (!GC.getLeaderHeadInfo((LeaderHeadTypes)iI).isNPC())
				{
					int iValue = 1 + GC.getGame().getSorenRandNum(10000, "Choosing Personality");

					for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
					{
						if (GET_PLAYER((PlayerTypes)iJ).isAlive()
						&& GET_PLAYER((PlayerTypes)iJ).getPersonalityType() == (LeaderHeadTypes)iI)
						{
							iValue /= 2;
						}
					}
					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						eBestPersonality = (LeaderHeadTypes)iI;
					}
				}
			}
			if (eBestPersonality != NO_LEADER)
			{
				setPersonalityType(eBestPersonality);
			}
		}
	}
	else
	{
		setPersonalityType(getLeaderType());
	}
}

//
// reset state of event logic, unit prices
//
void CvPlayer::resetCivTypeEffects()
{
	PROFILE_EXTRA_FUNC();
	if (!isAlive())
	{
		for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
		{
			setCivics(((CivicOptionTypes)iI), ((CivicTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationInitialCivics(iI))));
		}

		for (int iI = 0; iI < GC.getNumEventInfos(); iI++)
		{
			resetEventOccured((EventTypes)iI, false);
		}

		for (int iI = 0; iI < GC.getNumEventTriggerInfos(); iI++)
		{
			if( (!GC.getEventTriggerInfo((EventTriggerTypes)iI).isGlobal()) && (!GC.getEventTriggerInfo((EventTriggerTypes)iI).isTeam() || GET_TEAM(getTeam()).getNumMembers() == 1) )
			{
				resetTriggerFired((EventTriggerTypes)iI);
			}
		}
	}

	for (int iI = 0; iI < GC.getNumUnitInfos(); ++iI)
	{
		if (GC.getUnitInfo((UnitTypes)iI).isFound())
		{
			setUnitExtraCost((UnitTypes)iI, getNewCityProductionValue());
		}
	}
}


void CvPlayer::changeLeader(LeaderHeadTypes eNewLeader)
{
	PROFILE_EXTRA_FUNC();
	if (getLeaderType() == eNewLeader) return;

	// Set new personality
	changePersonalityType();

	// Clear old traits
	clearLeaderTraits();

	GC.getInitCore().setLeader(getID(), eNewLeader);

	// Add new traits
	FAssertMsg((GC.getNumTraitInfos() > 0), "GC.getNumTraitInfos() is less than or equal to zero but is expected to be larger than zero in CvPlayer::init");

	const LeaderHeadTypes eLeader = (LeaderHeadTypes)(int)getLeaderType();

	int iNumCoreDefaultTraits = GC.getLeaderHeadInfo(eLeader).getNumDefaultTraits();
	int iNumDefaultComplexTraits = GC.getLeaderHeadInfo(eLeader).getNumDefaultComplexTraits();
	TraitTypes eTrait = NO_TRAIT;

	if (GC.getGame().isOption(GAMEOPTION_LEADER_COMPLEX_TRAITS) && iNumDefaultComplexTraits > 0)
	{
		for (int iI = 0; iI < iNumDefaultComplexTraits; ++iI)
		{
			if (GC.getLeaderHeadInfo(eLeader).isDefaultComplexTrait(iI))
			{
				eTrait = TraitTypes(GC.getLeaderHeadInfo(eLeader).getDefaultComplexTrait(iI));
				if (GC.getTraitInfo(eTrait).isValidTrait(true))
				{
					m_pabHasTrait[eTrait] = true;
					processTrait(eTrait, 1);
				}
			}
		}
	}
	else if (iNumCoreDefaultTraits > 0)
	{
		for (int iI = 0; iI < iNumCoreDefaultTraits; ++iI)
		{
			if (GC.getLeaderHeadInfo(eLeader).isDefaultTrait(iI))
			{
				eTrait = TraitTypes(GC.getLeaderHeadInfo(eLeader).getDefaultTrait(iI));
				if (GC.getTraitInfo(eTrait).isValidTrait(true))
				{
					m_pabHasTrait[eTrait] = true;
					processTrait(eTrait, 1);
				}
			}
		}
	}
	else
	{
		for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
			if (GC.getLeaderHeadInfo(eLeader).hasTrait((TraitTypes)iI) && GC.getTraitInfo((TraitTypes)iI).isValidTrait(true))
			{
				m_pabHasTrait[(TraitTypes)iI] = true;
				processTrait((TraitTypes)iI, 1);
			}
		}
	}

	if (!isNPC() && canLeaderPromote())
	{
		do
		{
			doPromoteLeader();
		}
		while (canLeaderPromote());
	}

	if (isAlive() || isEverAlive())
	{
		gDLL->getInterfaceIFace()->setDirty(HighlightPlot_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(UnitInfo_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Flag_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(MinimapSection_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Foreign_Screen_DIRTY_BIT, true);
	}
	AI_init();
}


void CvPlayer::changeCiv(CivilizationTypes eNewCiv)
{
	PROFILE_EXTRA_FUNC();
	CivilizationTypes eOldCiv = getCivilizationType();
	PlayerColorTypes eColor = (PlayerColorTypes)GC.getCivilizationInfo(eNewCiv).getDefaultPlayerColor();

	if (eOldCiv == eNewCiv) return;

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (eColor == NO_PLAYERCOLOR || (GET_PLAYER((PlayerTypes)iI).getPlayerColor() == eColor && iI != getID()) )
		{
			for (int iK = 0; iK < GC.getNumPlayerColorInfos(); iK++)
			{
				if (iK != GC.getCivilizationInfo((CivilizationTypes)GC.getDefineINT("BARBARIAN_CIVILIZATION")).getDefaultPlayerColor())
				{
					bool bValid = true;

					for (int iL = 0; iL < MAX_PC_PLAYERS; iL++)
					{
						if (GET_PLAYER((PlayerTypes)iL).getPlayerColor() == iK)
						{
							bValid = false;
							break;
						}
					}

					if (bValid)
					{
						eColor = (PlayerColorTypes)iK;
						iI = MAX_PC_PLAYERS;
						break;
					}
				}
			}
		}
	}

	GC.getInitCore().setCiv( getID(), eNewCiv );
	GC.getInitCore().setColor( getID(), eColor );

	resetCivTypeEffects();

	if( isAlive() )
	{
		// if the player is alive and showing on scoreboard, etc
		// change colors, graphics, flags, units
		GC.getInitCore().setFlagDecal( getID(), (CvWString)GC.getCivilizationInfo(eNewCiv).getFlagTexture() );
		GC.getInitCore().setArtStyle( getID(), (ArtStyleTypes)GC.getCivilizationInfo(eNewCiv).getArtStyleType() );

		// Forces update of units flags
		EraTypes eEra = getCurrentEra();
		bool bAuto = m_bDisableHuman;
		m_bDisableHuman = true;
		//setCurrentEra((EraTypes)((eEra + 1)%GC.getNumEraInfos()));
		setCurrentEra((EraTypes)0);
		setCurrentEra((EraTypes)(GC.getNumEraInfos() - 1));

		setCurrentEra(eEra);
		m_bDisableHuman = bAuto;
		gDLL->getInterfaceIFace()->makeInterfaceDirty();

		// dirty all of this player's cities...
		algo::for_each(cities(), CvCity::fn::setLayoutDirty(true));

		//update unit eras
		foreach_(CvUnit* pLoopUnit, units())
		{
			pLoopUnit->reloadEntity();
/*
			CvPlot* pLoopPlot = pLoopUnit->plot();
			if( pLoopPlot )
			{

				CvFlagEntity* pFlag = pLoopPlot->getFlagSymbol();

				if( pFlag )
				{

					if( gDLL->getFlagEntityIFace()->getPlayer(pFlag) == getID() )
					{
						gDLL->getFlagEntityIFace()->destroy(pFlag);
						CvFlagEntity* pNewFlag = gDLL->getFlagEntityIFace()->create(getID());
						if (pFlag)
						{
							gDLL->getFlagEntityIFace()->setPlot(pNewFlag, pLoopPlot, false);
						}

						gDLL->getFlagEntityIFace()->updateGraphicEra(pNewFlag);
					}

				}

				pLoopPlot->setFlagDirty(true);
				//pLoopPlot->updateGraphicEra();
			}
*/
		}

		//update flag eras
		gDLL->getInterfaceIFace()->setDirty(Flag_DIRTY_BIT, true);

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(Soundtrack_DIRTY_BIT, true);
		}

		gDLL->getInterfaceIFace()->makeInterfaceDirty();

		// Need to force redraw
		gDLL->getEngineIFace()->SetDirty(CultureBorders_DIRTY_BIT, true);
		gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true);
		gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);
		gDLL->getEngineIFace()->SetDirty(GlobePartialTexture_DIRTY_BIT, true);

		gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(HighlightPlot_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(UnitInfo_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(MinimapSection_DIRTY_BIT, true);
		gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Foreign_Screen_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionSound_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(GlobeInfo_DIRTY_BIT, true);
	}
	else if( isEverAlive() )
	{
		// Not currently alive, but may show on some people's scoreboard
		// or graphs
		// change colors
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}

	setupGraphical();
}

//
// for changing whether this player is human or not
//
void CvPlayer::setIsHuman( bool bNewValue )
{
	if (bNewValue == isHumanPlayer())
		return;

	if (bNewValue)
		GC.getInitCore().setSlotStatus( getID(), SS_TAKEN );
	else
		GC.getInitCore().setSlotStatus( getID(), SS_COMPUTER ); // or SS_OPEN for multiplayer?

}


void CvPlayer::setIsRebel(bool bNewValue)
{
	m_bRebel = bNewValue;
}

bool CvPlayer::isRebel() const
{
	return m_bRebel;
}


int CvPlayer::getStabilityIndex( ) const
{
	return m_iStabilityIndex;
}

void CvPlayer::setStabilityIndex( int iNewValue )
{
	m_iStabilityIndex = range(iNewValue,0,1000);
}

void CvPlayer::changeStabilityIndex( int iChange )
{
	setStabilityIndex( getStabilityIndex() + iChange );
}

int CvPlayer::getStabilityIndexAverage( ) const
{
	return m_iStabilityIndexAverage;
}

void CvPlayer::setStabilityIndexAverage( int iNewValue )
{
	m_iStabilityIndexAverage = range(iNewValue,0,1000);
}

void CvPlayer::updateStabilityIndexAverage( )
{
	setStabilityIndexAverage( (2*getStabilityIndexAverage() + getStabilityIndex())/3 );
}


//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvPlayer::setupGraphical()
{
	if (GC.IsGraphicsInitialized())
	{
		algo::for_each(cities(), CvCity::fn::setupGraphical());

		//algo::for_each(units(), CvUnit::fn::setupGraphical());
		algo::for_each(units(), CvUnit::fn::reloadEntity());
	}
}


void CvPlayer::initFreeState()
{
	setGold(0);
	int iGold = GC.getHandicapInfo(getHandicapType()).getStartingGold() + GC.getEraInfo(GC.getGame().getStartEra()).getStartingGold();

	iGold *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
	iGold /= 100;

	changeGold(iGold);
	clearResearchQueue();
}


void CvPlayer::initFreeUnits()
{
	PROFILE_EXTRA_FUNC();
	if (getStartingPlot() == NULL) return;

	if (GC.getGame().isOption(GAMEOPTION_CORE_CUSTOM_START))
	{
		int iPoints = GC.getGame().getNumAdvancedStartPoints();

		iPoints *= GC.getHandicapInfo(getHandicapType()).getAdvancedStartPointsMod();
		iPoints /= 100;

		if (!isHumanPlayer())
		{
			iPoints *= GC.getHandicapInfo(getHandicapType()).getAIAdvancedStartPercent();
			iPoints /= 100;
		}

		setAdvancedStartPoints(iPoints);

		// Starting visibility
		const CvPlot* pStartingPlot = getStartingPlot();
		if (pStartingPlot)
		{
			for (int iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); ++iPlotLoop)
			{
				CvPlot* pPlot = GC.getMap().plotByIndex(iPlotLoop);

				if (plotDistance(pPlot->getX(), pPlot->getY(), pStartingPlot->getX(), pStartingPlot->getY()) <= GC.getADVANCED_START_SIGHT_RANGE())
				{
					pPlot->setRevealed(getTeam(), true, false, NO_TEAM, false);
				}
			}
		}
	}
	else // Create Starting units
	{
		const EraTypes startEra = GC.getGame().getStartEra();
		const int iMult = (
			GC.getGame().isOption(GAMEOPTION_CHALLENGE_ONE_CITY) ? 1
			:
			std::max(1, GC.getEraInfo(startEra).getStartingUnitMultiplier())
		);

		// Settler units, can't start a game without one.
		addStartUnitAI(UNITAI_SETTLE, iMult);
		if(!isHumanPlayer() && GC.getGame().isOption(GAMEOPTION_AI_BOOST)){
			addStartUnitAI(UNITAI_SETTLE, iMult);
			addStartUnitAI(UNITAI_GREAT_HUNTER, iMult);
		}

		// Defensive units
		int iCount = GC.getEraInfo(startEra).getStartingDefenseUnits();
		iCount += isHumanPlayer() ? GC.getHandicapInfo(getHandicapType()).getStartingDefenseUnits() : GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIStartingDefenseUnits();

		if (iCount > 0 && !addStartUnitAI(UNITAI_CITY_DEFENSE, iCount * iMult))
		{
			// Almost any unit qualifies for the UNITAI_EXPLORE package.
			addStartUnitAI(UNITAI_EXPLORE, iCount * iMult);
		}
		// Worker units
		iCount = GC.getEraInfo(startEra).getStartingWorkerUnits();
		iCount += isHumanPlayer() ? GC.getHandicapInfo(getHandicapType()).getStartingWorkerUnits() : GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIStartingWorkerUnits();

		if (iCount > 0 && !addStartUnitAI(UNITAI_WORKER, iCount * iMult))
		{
			addStartUnitAI(UNITAI_EXPLORE, iCount * iMult);
		}
		// Explorer units
		iCount = GC.getEraInfo(GC.getGame().getStartEra()).getStartingExploreUnits();
		iCount += isHumanPlayer() ? GC.getHandicapInfo(getHandicapType()).getStartingExploreUnits() : GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIStartingExploreUnits();
		if (iCount > 0)
		{
			addStartUnitAI(UNITAI_EXPLORE, iCount * iMult);
		}
	}
}

bool CvPlayer::addStartUnitAI(const UnitAITypes eUnitAI, const int iCount)
{
	PROFILE_EXTRA_FUNC();
	UnitTypes eUnit = NO_UNIT;
	int iBestValue = 0;

	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		if (eUnitAI != UNITAI_SETTLE && isLimitedUnit((UnitTypes) iI))
		{
			continue;
		}
		const CvUnitInfo& kUnit = GC.getUnitInfo((UnitTypes) iI);

		if (!GET_TEAM(getTeam()).isUnitBonusEnabledByTech(kUnit, true))
		{
			continue;
		}
		if (!kUnit.isCivilizationUnit(getID()))
		{
			continue; // is a Civ unit, but not for this player.
		}
		if (canTrain((UnitTypes) iI, false, false, false, true))
		{
			int iValue = AI_unitValue((UnitTypes) iI, eUnitAI, NULL);
			if (iValue > 0 && kUnit.getDefaultUnitAIType() != eUnitAI)
			{
				iValue /= 4;
			}
			if (iValue > iBestValue)
			{
				eUnit = (UnitTypes) iI;
				iBestValue = iValue;
			}
		}
	}
	if (eUnit != NO_UNIT)
	{
		const CvPlot* pBestPlot = getStartingPlot();
		for (int iI = 0; iI < iCount; iI++)
		{
			initUnit(eUnit, pBestPlot->getX(), pBestPlot->getY(), eUnitAI, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
		}
		return true;
	}
	return false;
}

UnitTypes CvPlayer::getBestUnitType(UnitAITypes eUnitAI) const
{
	PROFILE_EXTRA_FUNC();
	UnitTypes eBestUnit = NO_UNIT;
	int iBestValue = 0;

	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		if (algo::none_of(cities(), CvCity::fn::canTrain((UnitTypes) iI)))
			continue;

		const int iValue = AI_unitValue((UnitTypes) iI, eUnitAI, NULL);

		if (iValue > iBestValue)
		{
			eBestUnit = (UnitTypes) iI;
			iBestValue = iValue;
		}
	}
	return eBestUnit;
}

int CvPlayer::getBestUnitTypeCargoVolume(UnitAITypes eUnitAI) const
{
	UnitTypes eBestUnitType = getBestUnitType(eUnitAI);
	return (eBestUnitType != NO_UNIT ? GC.getUnitInfo(eBestUnitType).getBaseCargoVolume() : 0);
}


int CvPlayer::startingPlotRange() const
{
	int iRange = 10 + GC.getMap().maxStepDistance();

	// GC.getSTARTING_DISTANCE_PERCENT() / 100
	// GC.getMap().getLandPlots() / (NUM_CITY_PLOTS * GC.getWorldInfo(GC.getMap().getWorldSize()).getTargetNumCities() * GC.getGame().countCivPlayersAlive())
	iRange *= GC.getSTARTING_DISTANCE_PERCENT() * GC.getMap().getLandPlots();
	iRange /= 100 * NUM_CITY_PLOTS * GC.getWorldInfo(GC.getMap().getWorldSize()).getTargetNumCities() * GC.getGame().countCivPlayersAlive();

	iRange += std::min((GC.getMap().getNumAreas() + 1) / 2, GC.getGame().countCivPlayersAlive());

	int iResult = 0;
	if (Cy::call_optional(GC.getMap().getMapScript(), "minStartingDistanceModifier", iResult))
	{
		iRange *= std::max<int>(0, iResult + 100);
		iRange /= 100;
	}
	return std::max(iRange, GC.getDefineINT("MIN_CIV_STARTING_DISTANCE"));
}


bool CvPlayer::startingPlotWithinRange(CvPlot* pPlot, PlayerTypes ePlayer, int iRange, int iPass) const
{
	//PROFILE_FUNC();

	//XXX changes to AI_foundValue (which are far more flexible) make this function
	//	redundant but it is still called from Python.
	return false;
}

int CvPlayer::startingPlotDistanceFactor(const CvPlot* pPlot, PlayerTypes ePlayer, int iRange) const
{
	PROFILE_FUNC();

	FAssert(ePlayer != getID());

	int iValue = 1000;

	const CvPlot* pStartingPlot = getStartingPlot();

	if (pStartingPlot)
	{
		if (GC.getGame().isTeamGame())
		{
			if (GET_PLAYER(ePlayer).getTeam() == getTeam())
			{
				iRange *= GC.getDefineINT("OWN_TEAM_STARTING_MODIFIER");
				iRange /= 100;
			}
			else
			{
				iRange *= GC.getDefineINT("RIVAL_TEAM_STARTING_MODIFIER");
				iRange /= 100;
			}
		}

		int iDistance = stepDistance(pPlot->getX(), pPlot->getY(), pStartingPlot->getX(), pStartingPlot->getY());
		if (pStartingPlot->getArea() != pPlot->getArea())
		{
			iDistance *= 4;
			iDistance /= 3;
		}

		iValue *= iDistance;
		iValue /= iRange ;

	}

	return std::max(1, iValue);

}

namespace {
	int calculateStartingAreaScore(const CvArea* area)
	{
		// iNumPlayersOnArea is the number of players starting on the area, plus this player
		const int iNumPlayersOnArea = area->getNumStartingPlots() + 1;
		const int iTileValue =
		(
			1
			+ area->calculateTotalBestNatureYield()
			+ area->countCoastalLand() * 2
			+ area->getNumRiverEdges()
			+ area->getNumTiles()
		);
		int iValue = iTileValue / iNumPlayersOnArea;

		iValue *= std::min(NUM_CITY_PLOTS + 1, area->getNumTiles() + 1);
		iValue /= NUM_CITY_PLOTS + 1;

		if (iNumPlayersOnArea <= 2)
		{
			iValue *= 4;
			iValue /= 3;
		}
		return iValue;
	}
}

// Returns the id of the best area, or -1 if it doesn't matter:
int CvPlayer::findStartingArea() const
{
	PROFILE_FUNC();

	int result = -1;
	if (Cy::call_override(GC.getMap().getMapScript(), "findStartingArea", Cy::Args() << getID(), result))
	{
		if (result == -1 || GC.getMap().getArea(result))
		{
			return result;
		}
		FErrorMsg("python findStartingArea() must return -1 or the ID of a valid area");
	}
	const CvArea* bestStartingArea =
	(
		scoring::max_score(
			GC.getMap().areas() | filtered(!CvArea::fn::isWater()), calculateStartingAreaScore
		).get_value_or(nullptr)
	);
	return bestStartingArea ? bestStartingArea->getID() : -1;
}


CvPlot* CvPlayer::findStartingPlot(bool bRandomize)
{
	PROFILE_FUNC();
	{
		int result = -1;
		if (Cy::call_override(GC.getMap().getMapScript(), "findStartingPlot", Cy::Args() << getID(), result))
		{
			CvPlot *pPlot = GC.getMap().plotByIndex(result);
			if (pPlot)
			{
				return pPlot;
			}
			FErrorMsg("python findStartingPlot() returned an invalid plot index!");
		}
	}
	int iBestArea = -1;

	bool bNew = false;
	if (getStartingPlot())
	{
		iBestArea = getStartingPlot()->getArea();
		setStartingPlot(NULL, true);
		bNew = true;
	}

	AI_updateFoundValues(true);

	if (!bNew)
	{
		iBestArea = findStartingArea();
	}

	for (int iPass = 0; iPass < 2; iPass++)
	{
		CvPlot *pBestPlot = NULL;
		int iBestValue = 0;

		for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
		{
			CvPlot* plot = GC.getMap().plotByIndex(iI);

			if (plot->isStartingPlot()
			|| (CURRENT_MAP == MAP_EARTH && !plot->isMapCategoryType(GC.getMAPCATEGORY_EARTH()))
			|| iBestArea != -1 && plot->getArea() != iBestArea)
			{
				continue;
			}
			// The distance factor is now done inside foundValue
			int iValue = plot->getFoundValue(getID());

			if (iValue > 0)
			{
				if (bRandomize)
				{
					iValue += GC.getGame().getSorenRandNum(500 + iValue/50, "Randomize Starting Location");
				}
				if (iValue > iBestValue && (!bRandomize || pBestPlot == NULL || GC.getGame().getSorenRandNum(2, "Randomize Starting Location") > 0))
				{
					//FErrorMsg(CvString::format("iBestValue=%d", iValue).c_str());
					iBestValue = iValue;
					pBestPlot = plot;
				}
			}
		}
		if (pBestPlot)
		{
			return pBestPlot;
		}
		FAssertMsg(iPass != 0, "CvPlayer::findStartingPlot - could not find starting plot in first pass. This happens for space scenarios");
		iBestArea = -1; // best area was in space (scenario specific), do another pass with no best area restriction.
	}
	FErrorMsg("Could not find starting plot.");
	return NULL;
}


CvPlotGroup* CvPlayer::initPlotGroup(CvPlot* pPlot, bool bRecalculateBonuses)
{
	CvPlotGroup* pPlotGroup = addPlotGroup();

	FAssertMsg(pPlotGroup, "PlotGroup is not assigned a valid value");

	pPlotGroup->init(pPlotGroup->getID(), getID(), pPlot, bRecalculateBonuses);

	return pPlotGroup;
}

CvCity* CvPlayer::initCity(int iX, int iY, bool bBumpUnits, bool bUpdatePlotGroups)
{
	PROFILE_FUNC();

	CvPlot*	pCityPlot = GC.getMap().plot(iX,iY);

	if (bUpdatePlotGroups)
	{
		// Take this plot out of any player plot-group to remove any existing bonuses from it.
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				CvPlotGroup* pOldPlotGroup = pCityPlot->getPlotGroup((PlayerTypes)iI);

				if (pOldPlotGroup)
				{
					pOldPlotGroup->removePlot(pCityPlot);
				}
			}
		}
	}

	CvCity* pCity = addCity();

	FAssertMsg(pCity, "City is not assigned a valid value");
	FAssertMsg(!GC.getMap().plot(iX, iY)->isCity(), "No city is expected at this plot when initializing new city");

	pCity->init(pCity->getID(), getID(), iX, iY, bBumpUnits, false);

	if (bUpdatePlotGroups)
	{
		// Reassign plot to player plot-group.
		for (int iPlayerX = 0; iPlayerX < MAX_PLAYERS; iPlayerX++)
		{
			if (GET_PLAYER((PlayerTypes)iPlayerX).isAlive())
			{
				// Check if my cultural border change sliced hostile iPlayerX trade network
				if (::atWar(getTeam(), GET_PLAYER((PlayerTypes)iPlayerX).getTeam()))
				{
					bool bCutTradeNetwork = false;
					foreach_(const CvPlot* pPlot, pCity->plots())
					{
						if (pPlot->getOwner() == getID() && pPlot->getPlotGroup((PlayerTypes)iPlayerX))
						{
							// iPlayerX, whom I am at war with, have a trade route intersecting my new border
							bCutTradeNetwork = true;
							break;
						}
					}
					if (bCutTradeNetwork)
					{
						// Refresh all plot-groups for iPlayerX
						GET_PLAYER((PlayerTypes)iPlayerX).updatePlotGroups();
						continue; // next iPlayerX
					}
				}
				// No meaningful change - just add this plot back to the plot-group for iPlayerX
				pCityPlot->updatePlotGroup((PlayerTypes)iPlayerX);
			}
		}
	}
	return pCity;
}


void CvPlayer::acquireCity(CvCity* pOldCity, bool bConquest, bool bTrade, bool bUpdatePlotGroups)
{
	PROFILE_FUNC();
	if (pOldCity->isMarkedForDestruction()) return;
	pOldCity->markForDestruction();

	const PlayerTypes eNewOwner = getID();
	const PlayerTypes eOldOwner = pOldCity->getOwner();
	const PlayerTypes eOriginalOwner = pOldCity->getOriginalOwner();
	const PlayerTypes eHighestCulturePlayer = pOldCity->findHighestCulture();
	const bool bHuman = isHumanPlayer();
	const bool bRecapture = (eHighestCulturePlayer != NO_PLAYER ? GET_PLAYER(eHighestCulturePlayer).getTeam() == getTeam() : false);

	CvPlayerAI& oldOwner = GET_PLAYER(eOldOwner);
	CvPlot* pCityPlot = pOldCity->plot();
	CvCity* pNewCity = NULL;

	const int iOldCityId = pOldCity->getID();

	algo::for_each(pCityPlot->units_safe()
		| filtered(bind(CvUnit::getTeam, _1) != getTeam() && bind(CvUnit::getDomainType, _1) == DOMAIN_IMMOBILE)
		, bind(CvUnit::kill, _1, false, eNewOwner, true)
	);
	const int iX = pOldCity->getX();
	const int iY = pOldCity->getY();
	const CvWString cityName = pOldCity->getNameKey();

	int iCaptureGold = 0;

	if (bConquest)
	{
		iCaptureGold = Cy::call<int>(PYGameModule, "doCityCaptureGold", Cy::Args() << pOldCity << eNewOwner);
		changeGold(iCaptureGold);
	}

	// We can skip a lot if city is just to be razed right away.
	const bool bAutoRaze = GC.getGame().isAutoRaze(const_cast<CvCity*>(pOldCity), eNewOwner, bConquest, bTrade, bRecapture);

	CvEventReporter::getInstance().cityAcquired(eOldOwner, eNewOwner, pOldCity, bConquest, bTrade, bAutoRaze);

	if (bAutoRaze)
	{
		if (bHuman && iCaptureGold > 0)
		{
			AddDLLMessage(
				eNewOwner, true, GC.getEVENT_MESSAGE_TIME(),
				gDLL->getText("TXT_KEY_MISC_PILLAGED_CITY_RAZED", iCaptureGold, cityName.GetCString()),
				"AS2D_CITYRAZE", MESSAGE_TYPE_MAJOR_EVENT,
				ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(),
				GC.getCOLOR_GREEN(), iX, iY, true, true
			);
		}
		if (eOriginalOwner == eOldOwner
		|| GC.getGame().isOption(GAMEOPTION_BARBARIAN_CIV) && pOldCity->isCapital() && eOriginalOwner == BARBARIAN_PLAYER)
		{
			oldOwner.changeCitiesLost(1);
		}
		raze(pOldCity);
	}
	else
	{
		CvTeam& newTeam = GET_TEAM(getTeam());

		// Notifications, gold on capture
		if (bConquest)
		{
			if (bHuman)
			{
				if (iCaptureGold > 0)
				{
					AddDLLMessage(
						eNewOwner, true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_PILLAGED_CITY_CAPTURED", iCaptureGold, cityName.GetCString()),
						"AS2D_CITYRAZE", MESSAGE_TYPE_MAJOR_EVENT,
						ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(),
						GC.getCOLOR_GREEN(), iX, iY, true, true
					);
				}
				else
				{
					AddDLLMessage(
						eNewOwner, true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_CAPTURED_CITY", cityName.GetCString()),
						"AS2D_CITYCAPTURE", MESSAGE_TYPE_MAJOR_EVENT,
						ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(),
						GC.getCOLOR_GREEN(), iX, iY, true, true
					);
				}
			}
			CvWString szName;
			szName.Format(L"%s (%s)", pOldCity->getName().GetCString(), oldOwner.getName());

			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (iI != eNewOwner && GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHumanPlayer())
				{
					if (pOldCity->isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
					{
						AddDLLMessage(
							(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(),
							gDLL->getText(
								"TXT_KEY_MISC_CITY_CAPTURED_BY",
								szName.GetCString(), getCivilizationDescriptionKey()
							),
							"AS2D_CITYCAPTURED", MESSAGE_TYPE_MAJOR_EVENT,
							ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(),
							GC.getCOLOR_RED(), iX, iY, true, true
						);
					}
					else if (GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasEmbassy(getTeam()))
					{
						AddDLLMessage(
							(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(),
							gDLL->getText(
								"TXT_KEY_MISC_CITY_CAPTURED_BY",
								szName.GetCString(), getCivilizationDescriptionKey()
							),
							"AS2D_CITYCAPTURED", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_RED()
						);
					}
				}
			}
			GC.getGame().addReplayMessage(
				REPLAY_MESSAGE_MAJOR_EVENT, eNewOwner,
				gDLL->getText("TXT_KEY_MISC_CITY_WAS_CAPTURED_BY", szName.GetCString(), getCivilizationDescriptionKey()),
				iX, iY, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT")
			);

			// Conquest "flashes" all tiles in workable range to the conquerer, giving map info of surrounding tiles.
			// They only actually retain tiles they could keep normally though, so no unpredictable tile flipping next turn.
			// If civ has fixed borders, they'll retain more due to the lower threshold for maintaining ownership.
			pOldCity->clearCultureDistanceCache();
			const int iCultureLevel = pOldCity->getCultureLevel();
			foreach_(CvPlot* pLoopPlot, pOldCity->plots(true))
			{
				const int iCultureDist = pOldCity->cultureDistance(*pLoopPlot);
				if (iCultureDist > iCultureLevel)
				{
					continue;
				}
				// Only grab non-city, non-fort tiles from old owner
				if (!pLoopPlot->isCity(true) && pLoopPlot->getOwner() == eOldOwner)
				{
					// Sets owner to conquerer regardless of their culture on it, but don't update yet
					pLoopPlot->setOwner(eNewOwner, false, false);
					// Sees if owner can actually hold onto it, resetting if not
					pLoopPlot->updateCulture(true, true);
				}
			}
		}

		// Adjust occupation time due to buildings
		const int iNumBuildingInfos = GC.getNumBuildingInfos();
		int iOccupationTimeModifier = 0;

		foreach_(const BuildingTypes eTypeX, pOldCity->getHasBuildings())
		{
			if (GC.getBuildingInfo(eTypeX).getOccupationTimeModifier() != 0 && !pOldCity->isDisabledBuilding(eTypeX))
			{
				iOccupationTimeModifier += GC.getBuildingInfo(eTypeX).getOccupationTimeModifier();
			}
		}

		CvPlotGroup* originalTradeNetworkConnectivity[MAX_PLAYERS];

		// Whose trade networks was this city relevant to prior to ownership change
		if (bUpdatePlotGroups)
		{
			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				originalTradeNetworkConnectivity[iI] = GET_PLAYER((PlayerTypes)iI).isAlive() ? pCityPlot->getPlotGroup((PlayerTypes)iI) : NULL;
			}
		}

		if (eOriginalOwner == eOldOwner
		|| GC.getGame().isOption(GAMEOPTION_BARBARIAN_CIV) && pOldCity->isCapital() && eOriginalOwner == BARBARIAN_PLAYER)
		{
			oldOwner.changeCitiesLost(1);
		}
		else if (eOriginalOwner == eNewOwner)
		{
			GET_PLAYER(eOriginalOwner).changeCitiesLost(-1);
		}

		for (int iI = 0; iI < GC.getNumVoteSourceInfos(); ++iI)
		{
			pOldCity->processVoteSourceBonus((VoteSourceTypes)iI, false);
		}

		// Koshling - Need to remove bonuses due to old owner's trade network
		//	else it can feed into yield modifiers which are then incorrectly copied over to the new owner
		for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
		{
			if (pOldCity->hasBonus((BonusTypes)iI))
			{
				pOldCity->processBonus((BonusTypes)iI, -1);
			}
		}

		bst::scoped_array<bool> pabHasReligion(new bool[GC.getNumReligionInfos()]);
		bst::scoped_array<bool> pabHolyCity(new bool[GC.getNumReligionInfos()]);
		bst::scoped_array<bool> pabHasCorporation(new bool[GC.getNumCorporationInfos()]);
		bst::scoped_array<bool> pabHeadquarters(new bool[GC.getNumCorporationInfos()]);

		const int iGameTurnFounded = pOldCity->getGameTurnFounded();
		const int iPopulation = pOldCity->getPopulation();
		const int iHighestPopulation = pOldCity->getHighestPopulation();
		const int iHurryAngerTimer = pOldCity->getHurryAngerTimer();
		const int iConscriptAngerTimer = pOldCity->getConscriptAngerTimer();
		const int iDefyResolutionAngerTimer = pOldCity->getDefyResolutionAngerTimer();
		const int iOccupationTimer = pOldCity->getOccupationTimer();

		std::string scriptData = pOldCity->getScriptData();
		const int iRevIdx = pOldCity->getRevolutionIndex();
		const int iLocalRevIdx = pOldCity->getLocalRevIndex();

		const int iDamage = pOldCity->getDefenseDamage();

		int iCiv = pOldCity->getCivilizationType();
		if (pOldCity->isNPC())
		{
			iCiv = NO_CIVILIZATION;
		}
		std::vector<int> aeFreeSpecialists;

		for (int iI = 0; iI < GC.getNumSpecialistInfos(); ++iI)
		{
			aeFreeSpecialists.push_back(pOldCity->getAddedFreeSpecialistCount((SpecialistTypes)iI));
		}

		bool abEverOwned[MAX_PLAYERS];
		int aiCulture[MAX_PLAYERS];

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			abEverOwned[iI] = pOldCity->isEverOwned((PlayerTypes)iI);
			aiCulture[iI] = pOldCity->getCultureTimes100((PlayerTypes)iI);
		}

		abEverOwned[eNewOwner] = true;

		for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			pabHasReligion[iI] = pOldCity->isHasReligion((ReligionTypes)iI);
			pabHolyCity[iI] = pOldCity->isHolyCity((ReligionTypes)iI);
		}

		for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
		{
			pabHasCorporation[iI] = pOldCity->isHasCorporation((CorporationTypes)iI);
			pabHeadquarters[iI] = pOldCity->isHeadquarters((CorporationTypes)iI);
		}

		std::map<BuildingTypes, BuiltBuildingData> buildingLedger;

		foreach_(const BuildingTypes eType, pOldCity->getHasBuildings())
		{
			if (!GC.getBuildingInfo(eType).isNeverCapture() && !isProductionMaxedBuilding(eType, true)
			&& (!bConquest || bRecapture || GC.getGame().getSorenRandNum(100, "Capture Probability") < GC.getBuildingInfo(eType).getConquestProbability()))
			{
				buildingLedger.insert(std::make_pair(eType, pOldCity->getBuildingData(eType)));
			}
		}

		BuildingChangeArray aBuildingHappyChange;
		BuildingChangeArray aBuildingHealthChange;
		for (int iI = 0; iI < iNumBuildingInfos; ++iI)
		{
			int iChange = pOldCity->getBuildingHappyChange((BuildingTypes)iI);
			if (0 != iChange)
			{
				aBuildingHappyChange.push_back(std::make_pair((BuildingTypes)iI, iChange));
			}

			iChange = pOldCity->getBuildingHealthChange((BuildingTypes)iI);
			if (0 != iChange)
			{
				aBuildingHealthChange.push_back(std::make_pair((BuildingTypes)iI, iChange));
			}
		}
		pOldCity->kill(false, false); // Invalidates pOldCity pointer.

		if (bTrade)
		{
			algo::for_each(pCityPlot->rect(1, 1), bind(&CvPlot::setCulture, _1, eOldOwner, 0, false, false, false));
		}
		pNewCity = initCity(pCityPlot->getX(), pCityPlot->getY(), !bConquest, false);

		FAssertMsg(pNewCity, "NewCity is not assigned a valid value");

		bool bHistoricalCalendar = GC.getGame().isModderGameOption(MODDERGAMEOPTION_USE_HISTORICAL_ACCURATE_CALENDAR);
		pNewCity->setPreviousOwner(eOldOwner);
		pNewCity->setOriginalOwner(eOriginalOwner);
		pNewCity->setGameTurnFounded(iGameTurnFounded, bHistoricalCalendar);
		pNewCity->setPopulation((bConquest && !bRecapture) ? std::max(1, (iPopulation - 1)) : iPopulation, false);
		pNewCity->setHighestPopulation(iHighestPopulation);
		pNewCity->setName(cityName);
		pNewCity->setNeverLost(false);
		pNewCity->changeDefenseDamage(iDamage);

		if (iCiv != NO_CIVILIZATION)
		{
			pNewCity->setCivilizationType(iCiv);
		}

		pNewCity->setScriptData(scriptData);
		pNewCity->setRevolutionIndex(iRevIdx);
		pNewCity->setLocalRevIndex(iLocalRevIdx);

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			pNewCity->setEverOwned(((PlayerTypes)iI), abEverOwned[iI]);
			pNewCity->setCultureTimes100(((PlayerTypes)iI), aiCulture[iI], false, false);
		}

		{
			std::vector<BuildingTypes> obsolete;
			for (std::map<BuildingTypes, BuiltBuildingData>::const_iterator itr = buildingLedger.begin(); itr != buildingLedger.end(); ++itr)
			{
				if (newTeam.isObsoleteBuilding(itr->first))
				{
					obsolete.push_back(itr->first);
				}
				else pNewCity->setHasBuilding(itr->first, true, itr->second.eBuiltBy, itr->second.iTimeBuilt, false);
			}
			foreach_(const BuildingTypes eType, obsolete)
			{
				BuildingTypes eObsoletesToBuilding = GC.getBuildingInfo(eType).getObsoletesToBuilding();
				while (eObsoletesToBuilding > NO_BUILDING)
				{
					if (hasBuilding(eObsoletesToBuilding))
					{
						break;
					}
					if (GC.getBuildingInfo(eObsoletesToBuilding).getObsoleteTech() < 0
					|| !GET_TEAM(getTeam()).isHasTech(GC.getBuildingInfo(eObsoletesToBuilding).getObsoleteTech()))
					{
						pNewCity->changeHasBuilding(eObsoletesToBuilding, true);
						break;
					}
					eObsoletesToBuilding = GC.getBuildingInfo(eObsoletesToBuilding).getObsoletesToBuilding();
				}
			}
		}

		for (BuildingChangeArray::iterator it = aBuildingHappyChange.begin(); it != aBuildingHappyChange.end(); ++it)
		{
			pNewCity->setBuildingHappyChange((*it).first, (*it).second);
		}

		for (BuildingChangeArray::iterator it = aBuildingHealthChange.begin(); it != aBuildingHealthChange.end(); ++it)
		{
			pNewCity->setBuildingHealthChange((*it).first, (*it).second);
		}

		for (int iI = 0; iI < GC.getNumSpecialistInfos(); ++iI)
		{
			pNewCity->changeFreeSpecialistCount((SpecialistTypes)iI, aeFreeSpecialists[iI], true);
		}

		for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			if (pabHasReligion[iI])
			{
				pNewCity->setHasReligion((ReligionTypes)iI, true, false, true);
			}

			if (pabHolyCity[iI])
			{
				GC.getGame().setHolyCity((ReligionTypes)iI, pNewCity, false);

				for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive() && GET_PLAYER((PlayerTypes)iI).getStateReligion() == (ReligionTypes)iI)
					{
						AI_invalidateAttitudeCache((PlayerTypes)iJ);
						GET_PLAYER((PlayerTypes)iJ).AI_invalidateAttitudeCache(eNewOwner);
					}
				}
			}
		}

		for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
		{
			if (pabHasCorporation[iI])
			{
				pNewCity->setHasCorporation((CorporationTypes)iI, true, false);
			}
			if (pabHeadquarters[iI])
			{
				GC.getGame().setHeadquarters((CorporationTypes)iI, pNewCity, false);
			}
		}

		if (bTrade)
		{
			if (bHuman || getTeam() == oldOwner.getTeam())
			{
				pNewCity->changeHurryAngerTimer(iHurryAngerTimer);
				pNewCity->changeConscriptAngerTimer(iConscriptAngerTimer);
				pNewCity->changeDefyResolutionAngerTimer(iDefyResolutionAngerTimer);
			}

			if (!bRecapture)
			{
				pNewCity->changeOccupationTimer(iOccupationTimer);
			}
		}

		if (bConquest)
		{
			const int iTeamCulturePercent = pNewCity->calculateTeamCulturePercent(getTeam());

			if (iTeamCulturePercent < GC.getDefineINT("OCCUPATION_CULTURE_PERCENT_THRESHOLD"))
			{
				int iOccupationTime = GC.getDefineINT("BASE_OCCUPATION_TURNS") + intSqrt(pNewCity->getPopulation());

				iOccupationTime *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent(); // Extra 100x

				// Normalize: Full timer at 0 culture, no timer when culture == occupation threshold.
				iOccupationTime *= 1000 - 1000 * iTeamCulturePercent / GC.getDefineINT("OCCUPATION_CULTURE_PERCENT_THRESHOLD"); // Extra 1000x

				iOccupationTime = getModifiedIntValue(iOccupationTime, iOccupationTimeModifier);

				iOccupationTime /= 100000; // 100*1000

				pNewCity->changeOccupationTimer(iOccupationTime);
			}
			GC.getMap().verifyUnitValidPlot();

			if (hasDraftsOnCityCapture())
			{
				pNewCity->conscript(true);
			}
		}

		pNewCity->checkBuildings(false);
		pNewCity->updateEspionageVisibility(false);

		// Don't bother with plot group calculations if they are immediately to be superseded by an auto raze
		if (bUpdatePlotGroups)
		{
			PROFILE("CvPlayer::acquireCity.UpdatePlotGroups");

			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (originalTradeNetworkConnectivity[iI])
					{
						originalTradeNetworkConnectivity[iI]->recalculatePlots();
					}
					else if (pCityPlot->isTradeNetwork(GET_PLAYER((PlayerTypes)iI).getTeam()))
					{
						GET_PLAYER((PlayerTypes)iI).updatePlotGroups(pCityPlot->area());
					}
				}
			}
		}
		if (gPlayerLogLevel >= 1)
		{
			logBBAI("  Player %d (%S) acquires city %S bConq %d bTrade %d", eNewOwner, getCivilizationDescription(0), pNewCity->getName(0).GetCString(), bConquest, bTrade );
		}

		if (!bHuman)
		{
			AI_setHasInquisitionTarget();
		}
	}
	if (!oldOwner.isHumanPlayer())
	{
		oldOwner.AI_setHasInquisitionTarget();
	}
	pCityPlot->setRevealed(oldOwner.getTeam(), true, false, NO_TEAM, false);
	pCityPlot->updateCulture(true, false);

	algo::for_each(pCityPlot->adjacent(), CvPlot::fn::updateCulture(true, false));

	if (!bAutoRaze)
	{
		if (!bConquest)
		{
			// Silences double ask for accepting new city from Revolution mod
			if (!bTrade && !GC.getGame().isOption(GAMEOPTION_UNSUPPORTED_REVOLUTION))
			{
				if (bHuman)
				{
					CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_DISBANDCITY);
					pInfo->setData1(pNewCity->getID());
					gDLL->getInterfaceIFace()->addPopup(pInfo, eNewOwner);
				}
				else CvEventReporter::getInstance().cityAcquiredAndKept(eOldOwner, eNewOwner, pNewCity, bConquest, bTrade);
			}
		}
		else if (bHuman)
		{
			//popup raze option
			const PlayerTypes eHighestCulturePlayer = pNewCity->getLiberationPlayer(true);
			const bool bGift =
			(
					eHighestCulturePlayer != NO_PLAYER
				&&	eHighestCulturePlayer != eNewOwner
				&&
				(
						getTeam() == GET_PLAYER(eHighestCulturePlayer).getTeam()
					||	GET_TEAM(getTeam()).isOpenBorders(GET_PLAYER(eHighestCulturePlayer).getTeam())
					||	GET_TEAM(GET_PLAYER(eHighestCulturePlayer).getTeam()).isVassal(getTeam())
				)
			);
			if (bGift || !GC.getGame().isOption(GAMEOPTION_NO_CITY_RAZING))
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_RAZECITY);
				pInfo->setData1(pNewCity->getID());
				pInfo->setData2(eHighestCulturePlayer);
				pInfo->setData3(iCaptureGold);
				pInfo->setOption1(bConquest);
				pInfo->setOption2(bTrade);
				gDLL->getInterfaceIFace()->addPopup(pInfo, eNewOwner);
			}
			else
			{
				CvEventReporter::getInstance().cityAcquiredAndKept(eOldOwner, eNewOwner, pNewCity, bConquest, bTrade);
			}
		}
		else AI_conquerCity(eOldOwner, pNewCity, bConquest, bTrade); // could delete the pNewCity pointer...
	}

	// Forcing events that deal with the old city not to expire just because we conquered that city
	for (CvEventMap::iterator it = m_mapEventsOccured.begin(); it != m_mapEventsOccured.end(); ++it)
	{
		EventTriggeredData &triggerData = it->second;
		if((triggerData.m_eOtherPlayer == eOldOwner) && (triggerData.m_iOtherPlayerCityId == iOldCityId))
		{
			triggerData.m_iOtherPlayerCityId = -1;
		}
	}
}


void CvPlayer::killCities()
{
	algo::for_each(cities(), CvCity::fn::kill(false));
	GC.getGame().updatePlotGroups();
}


CvWString CvPlayer::getNewCityName() const
{
	PROFILE_EXTRA_FUNC();
	CLLNode<CvWString>* pNode;
	CvWString szName;

	for (pNode = headCityNameNode(); pNode; pNode = nextCityNameNode(pNode))
	{
		szName = gDLL->getText(pNode->m_data);
		if (isCityNameValid(szName, true))
		{
			szName = pNode->m_data;
			break;
		}
	}

	if (szName.empty())
	{
		getCivilizationCityName(szName, getCivilizationType());
	}

	if (szName.empty())
	{
		// Pick a name from another random civ
		int iRandOffset = GC.getGame().getSorenRandNum(GC.getNumCivilizationInfos(), "Place Units (Player)");
		for (int iI = 0; iI < GC.getNumCivilizationInfos(); iI++)
		{
			int iLoopName = ((iI + iRandOffset) % GC.getNumCivilizationInfos());

			getCivilizationCityName(szName, ((CivilizationTypes)iLoopName));

			if (!szName.empty())
			{
				break;
			}
		}
	}

	if (szName.empty())
	{
		szName = "TXT_WORD_CITY";
	}
	return szName;
}


void CvPlayer::getCivilizationCityName(CvWString& szBuffer, CivilizationTypes eCivilization) const
{
	PROFILE_EXTRA_FUNC();
	int iRandOffset = 0;

	if (GC.getGame().isOption(GAMEOPTION_UNSUPPORTED_REVOLUTION) || GC.getGame().isOption(GAMEOPTION_BARBARIAN_CIV))
	{
		if (isNPC())
		{
			iRandOffset = GC.getGame().getSorenRandNum(GC.getCivilizationInfo(eCivilization).getNumCityNames(), "Place Units (Player)");
		}
	}
	else if (isNPC() || isMinorCiv())
	{
		iRandOffset = GC.getGame().getSorenRandNum(GC.getCivilizationInfo(eCivilization).getNumCityNames(), "Place Units (Player)");
	}

	for (int iI = 0; iI < GC.getCivilizationInfo(eCivilization).getNumCityNames(); iI++)
	{
		const int iLoopName = ((iI + iRandOffset) % GC.getCivilizationInfo(eCivilization).getNumCityNames());

		CvWString szName = gDLL->getText(GC.getCivilizationInfo(eCivilization).getCityNames(iLoopName));

		if (isCityNameValid(szName, true))
		{
			szBuffer = GC.getCivilizationInfo(eCivilization).getCityNames(iLoopName);
			break;
		}
	}
}


bool CvPlayer::isCityNameValid(CvWString& szName, bool bTestDestroyed) const
{
	PROFILE_EXTRA_FUNC();
	if (bTestDestroyed)
	{
		if (GC.getGame().isDestroyedCityName(szName))
		{
			return false;
		}

		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			if (algo::any_of(GET_PLAYER((PlayerTypes)iPlayer).cities(), CvCity::fn::getName() == szName))
			{
				return false;
			}
		}
	}
	else if (algo::any_of(cities(), CvCity::fn::getName() == szName))
	{
		return false;
	}

	return true;
}

CvUnit* CvPlayer::getTempUnit(UnitTypes eUnit, int iX, int iY)
{
	if (m_pTempUnit)
	{
		if (m_pTempUnit->plot())
		{
			//GC.getGame().logOOSSpecial(8, m_pTempUnit->getID(), INVALID_PLOT_COORD, INVALID_PLOT_COORD);
			m_pTempUnit->setXY(INVALID_PLOT_COORD, INVALID_PLOT_COORD, true, false);
		}
		m_pTempUnit->changeIdentity(eUnit);
		//GC.getGame().logOOSSpecial(9, m_pTempUnit->getID(), iX, iY);
		m_pTempUnit->setXY(iX, iY, true, false);
	}
	else
	{
		m_pTempUnit = initUnit(eUnit, iX, iY, NO_UNITAI, NO_DIRECTION, UNIT_BIRTHMARK_TEMP_UNIT);
		((CvPlayerAI*)this)->AI_changeNumAIUnits(m_pTempUnit->AI_getUnitAIType(), -1); // This one doesn't count
		removeGroupCycle(m_pTempUnit->getGroup()->getID());
	}
	// Set an arbitrary automation type - just need it to be flagged as automated
	m_pTempUnit->getGroup()->setAutomateType(AUTOMATE_BUILD);

	return m_pTempUnit;
}

void CvPlayer::releaseTempUnit()
{
	//GC.getGame().logOOSSpecial(10, m_pTempUnit->getID(), INVALID_PLOT_COORD, INVALID_PLOT_COORD);
	m_pTempUnit->setXY(INVALID_PLOT_COORD, INVALID_PLOT_COORD, true, false);
}

CvUnit* CvPlayer::initUnit(UnitTypes eUnit, int iX, int iY, UnitAITypes eUnitAI, DirectionTypes eFacingDirection, int iBirthmark)
{
	PROFILE_FUNC();

	FAssertMsg(eUnit != NO_UNIT, "Unit is not assigned a valid value");

	CvUnit* pUnit = addUnit();
	FAssertMsg(pUnit, "Unit is not assigned a valid value");
	if (pUnit)
	{
		pUnit->init(pUnit->getID(), eUnit, ((eUnitAI == NO_UNITAI) ? GC.getUnitInfo(eUnit).getDefaultUnitAIType() : eUnitAI), getID(), iX, iY, eFacingDirection, iBirthmark);
	}
	return pUnit;
}


void CvPlayer::disbandUnit()
{
	PROFILE_EXTRA_FUNC();
	wchar_t szBuffer[1024];

	int iBestValue = MAX_INT;
	CvUnit* pBestUnit = NULL;

	foreach_(CvUnit* pLoopUnit, units())
	{
		if (!(pLoopUnit->hasCargo()))
		{
			if (!(pLoopUnit->isGoldenAge()))
			{
				if (pLoopUnit->getUnitInfo().getProductionCost() > 0)
				{
					if (!(pLoopUnit->isMilitaryHappiness()) || !(pLoopUnit->plot()->isCity()) || (pLoopUnit->plot()->plotCount(PUF_isMilitaryHappiness, -1, -1, NULL, getID()) > 1))
					{
						int iValue = (10000 + GC.getGame().getSorenRandNum(1000, "Disband Unit"));

						iValue += (pLoopUnit->getUnitInfo().getProductionCost() * 5);

						iValue += (pLoopUnit->getExperience() * 20);
						iValue += (pLoopUnit->getLevel() * 100);

						if (pLoopUnit->canDefend() && pLoopUnit->plot()->isCity())
						{
							iValue *= 2;
						}

						if (pLoopUnit->plot()->getTeam() == pLoopUnit->getTeam())
						{
							iValue *= 3;
						}

						if (pLoopUnit->plot()->getOwner() == pLoopUnit->getOwner())
						{
							iValue /= 6;
						}

						switch (pLoopUnit->AI_getUnitAIType())
						{
						case UNITAI_UNKNOWN:
						case UNITAI_PROPHET:
						case UNITAI_ARTIST:
						case UNITAI_SCIENTIST:
						case UNITAI_GENERAL:
						case UNITAI_GREAT_HUNTER:
						case UNITAI_GREAT_ADMIRAL:
						case UNITAI_MERCHANT:
						case UNITAI_ENGINEER:
						case UNITAI_ANIMAL:
						case UNITAI_BARB_CRIMINAL:
							break;

						case UNITAI_ATTACK:
						case UNITAI_ATTACK_CITY:
						case UNITAI_COLLATERAL:
						case UNITAI_PILLAGE:
						case UNITAI_RESERVE:
						case UNITAI_COUNTER:
						case UNITAI_PILLAGE_COUNTER:
						case UNITAI_PIRATE_SEA:
						case UNITAI_ATTACK_AIR:
						case UNITAI_INVESTIGATOR:
						case UNITAI_SEE_INVISIBLE:
							iValue *= 2;
							break;

						case UNITAI_SUBDUED_ANIMAL:
						case UNITAI_ATTACK_SEA:
						case UNITAI_RESERVE_SEA:
						case UNITAI_ESCORT_SEA:
						case UNITAI_DEFENSE_AIR:
						case UNITAI_CARRIER_AIR:
						case UNITAI_MISSILE_AIR:
						case UNITAI_SEE_INVISIBLE_SEA:
							iValue *= 3;
							break;

						case UNITAI_ICBM:
							iValue *= 4;
							break;

						case UNITAI_ASSAULT_SEA:
						case UNITAI_SETTLER_SEA:
						case UNITAI_MISSIONARY_SEA:
						case UNITAI_SPY_SEA:
						case UNITAI_CARRIER_SEA:
						case UNITAI_MISSILE_CARRIER_SEA:
							iValue *= 5;
							break;

						case UNITAI_PROPERTY_CONTROL:
						case UNITAI_HEALER:
						case UNITAI_PROPERTY_CONTROL_SEA:
						case UNITAI_HEALER_SEA:
						case UNITAI_CITY_DEFENSE:
						case UNITAI_CITY_COUNTER:
						case UNITAI_CITY_SPECIAL:
						case UNITAI_PARADROP:
							iValue *= 6;
							break;

						case UNITAI_MISSIONARY:
						case UNITAI_INFILTRATOR:
							iValue *= 8;
							break;

						case UNITAI_HUNTER_ESCORT:
						case UNITAI_HUNTER:
						case UNITAI_WORKER:
							iValue *= 10;
							break;

						case UNITAI_SPY:
							iValue *= 12;
							break;

						case UNITAI_EXPLORE:
						case UNITAI_ESCORT:
							iValue *= 15;
							break;

						case UNITAI_WORKER_SEA:
							iValue *= 18;
							break;

						case UNITAI_SETTLE:
							iValue *= 20;
							break;

						case UNITAI_EXPLORE_SEA:
							iValue *= 25;
							break;

						default:
							FErrorMsg("error");
							break;
						}

						iValue /= pLoopUnit->getUnitInfo().getBaseUpkeep() + 1;

						if (iValue < iBestValue)
						{
							iBestValue = iValue;
							pBestUnit = pLoopUnit;
						}
					}
				}
			}
		}
	}

	if (pBestUnit)
	{
		swprintf(szBuffer, gDLL->getText("TXT_KEY_MISC_UNIT_DISBANDED_NO_MONEY", pBestUnit->getNameKey()).GetCString());
		AddDLLMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNITDISBANDED", MESSAGE_TYPE_MINOR_EVENT, pBestUnit->getButton(), GC.getCOLOR_RED(), pBestUnit->getX(), pBestUnit->getY(), true, true);

		FAssert(!(pBestUnit->isGoldenAge()));

		if( gPlayerLogLevel >= 2 )
		{
			const CvWString szStringUnitAi = GC.getUnitAIInfo(pBestUnit->AI_getUnitAIType()).getType();
			logBBAI("	Player %d (%S) disbanding %S with UNITAI %S (forced)", getID(), getCivilizationDescription(0), pBestUnit->getName().GetCString(), szStringUnitAi.GetCString());
		}

		pBestUnit->kill(false);
	}
}


void CvPlayer::killUnits()
{
	algo::for_each(units(), CvUnit::fn::kill(false));
}


// Returns the next unit in the cycle...
CvSelectionGroup* CvPlayer::cycleSelectionGroups(const CvUnit* pUnit, bool bForward, bool bWorkers, bool* pbWrap, bool bAllowViewportSwitch)
{
	PROFILE_EXTRA_FUNC();
	if (pbWrap)
	{
		*pbWrap = false;
	}

	CLLNode<int>* pSelectionGroupNode = headGroupCycleNode();

	if (pUnit)
	{
		while (pSelectionGroupNode)
		{
			if (getSelectionGroup(pSelectionGroupNode->m_data) == pUnit->getGroup())
			{
				pSelectionGroupNode = bForward ? nextGroupCycleNode(pSelectionGroupNode) : previousGroupCycleNode(pSelectionGroupNode);
				break;
			}
			pSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);
		}
	}

	if (pSelectionGroupNode == NULL)
	{
		pSelectionGroupNode = bForward ? headGroupCycleNode() : tailGroupCycleNode();

		if (pbWrap)
		{
			*pbWrap = true;
		}
	}

	if (pSelectionGroupNode)
	{
		CvSelectionGroup* pNonViewportOption = NULL;

		CLLNode<int>* pFirstSelectionGroupNode = pSelectionGroupNode;
		const CvViewport* pCurrentViewport = GC.getCurrentViewport();

		while (true)
		{
			CvSelectionGroup* pLoopSelectionGroup = getSelectionGroup(pSelectionGroupNode->m_data);
			FAssertMsg(pLoopSelectionGroup, "LoopSelectionGroup is not assigned a valid value");

			if (pLoopSelectionGroup->readyToSelect() && (!bWorkers || pLoopSelectionGroup->hasWorker()))
			{
				if (pUnit && pLoopSelectionGroup == pUnit->getGroup() && pbWrap)
				{
					*pbWrap = true;
				}

				if (pCurrentViewport->isInViewport(pLoopSelectionGroup->getX(), pLoopSelectionGroup->getY(), GC.getVIEWPORT_FOCUS_BORDER()))
				{
					return pLoopSelectionGroup;
				}
				if (pNonViewportOption == NULL)
				{
					pNonViewportOption = pLoopSelectionGroup;
				}
			}

			pSelectionGroupNode = bForward ? nextGroupCycleNode(pSelectionGroupNode) : previousGroupCycleNode(pSelectionGroupNode);

			if (pSelectionGroupNode == NULL)
			{
				pSelectionGroupNode = bForward ? headGroupCycleNode() : tailGroupCycleNode();

				if (pbWrap)
				{
					*pbWrap = true;
				}
			}
			if (pSelectionGroupNode == pFirstSelectionGroupNode)
			{
				break;
			}
		}

		if (pNonViewportOption && bAllowViewportSwitch)
		{
			return pNonViewportOption;
		}
	}
	return NULL;
}


bool CvPlayer::hasTrait(TraitTypes eTrait) const
{
	FAssertMsg(getLeaderType() >= 0, "getLeaderType() is less than zero");
	FAssertMsg(eTrait >= 0, "eTrait is less than zero");
	return m_pabHasTrait ? m_pabHasTrait[eTrait] : false;
}


void CvPlayer::setHumanDisabled(bool newVal)
{
	m_bDisableHuman = newVal;
	setUnitUpkeepDirty();
	updateHuman();
}

/*DllExport*/ bool CvPlayer::isHuman() const
{
#ifdef _DEBUG
	OutputDebugString(CvString::format("exe is asking if player (%d) is human\n", m_eID).c_str());
#endif
	return m_bHuman;
}

bool CvPlayer::isNormalAI() const
{
	return !m_bHuman && !m_bDisableHuman && getID() < MAX_PC_PLAYERS;
}

void CvPlayer::updateHuman()
{
	m_bHuman = !m_bDisableHuman && getID() != NO_PLAYER && GC.getInitCore().getHuman(getID());
}


/*DllExport*/ bool CvPlayer::isBarbarian() const
{
#ifdef _DEBUG
	OutputDebugString(CvString::format("exe is asking if player (%d) is barbarian\n", m_eID).c_str());
#endif
	return getID() == BARBARIAN_PLAYER;
}

bool CvPlayer::isNPC() const
{
	return ((int)getID() >= MAX_PC_PLAYERS);
}

bool CvPlayer::isHominid() const
{
	return (getID() == BARBARIAN_PLAYER || getID() == NEANDERTHAL_PLAYER);
}

bool CvPlayer::isAnimal() const
{
	switch (getID())
	{
		case PREDATOR_PLAYER:
		case PREY_PLAYER:
		case BEAST_PLAYER:
		{
			return true;
		}
		default: return false;
	}
}


const wchar_t* CvPlayer::getName(uint uiForm) const
{
	if (!m_szName.empty())
	{
		return m_szName;
	}
	if (GC.getInitCore().getLeaderName(getID(), uiForm).empty() || GC.getGame().isMPOption(MPOPTION_ANONYMOUS) && isAlive() && GC.getGame().getGameState() == GAMESTATE_ON)
	{
		return GC.getLeaderHeadInfo(getLeaderType()).getDescription(uiForm);
	}
	return GC.getInitCore().getLeaderName(getID(), uiForm);
}


// This sets the name of the player, ie Peter or Montezuma
void CvPlayer::setName(std::wstring szNewValue)
{
	if (isCityNameValid(CvWString(szNewValue), false))
	{
		m_szName = szNewValue;
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Foreign_Screen_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Flag_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(MinimapSection_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(UnitInfo_DIRTY_BIT, true);
	}
}


const wchar_t* CvPlayer::getNameKey() const
{
	/* dynamic civ names
	if (GC.getInitCore().getLeaderNameKey(getID()).empty() || (GC.getGame().isMPOption(MPOPTION_ANONYMOUS) && isAlive()))
	{
		return GC.getLeaderHeadInfo(getLeaderType()).getTextKeyWide();
	}
	else
	{
		return GC.getInitCore().getLeaderNameKey(getID());
	}
	*/
	if (!m_szName.empty())
	{
		return m_szName;
	}
	if (GC.getInitCore().getLeaderNameKey(getID()).empty() || (GC.getGame().isMPOption(MPOPTION_ANONYMOUS) && isAlive()))
	{
		return GC.getLeaderHeadInfo(getLeaderType()).getTextKeyWide();
	}
	return GC.getInitCore().getLeaderNameKey(getID());
}


const wchar_t* CvPlayer::getCivilizationDescription(uint uiForm) const
{
	/* dynamic civ names
	if (GC.getInitCore().getCivDescription(getID(), uiForm).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getDescription(uiForm);
	}
	else
	{
		return GC.getInitCore().getCivDescription(getID(), uiForm);
	}
	*/
	if( !(m_szCivDesc.empty()) )
	{
		return m_szCivDesc;
	}

	else if (getCivilizationType() != NO_CIVILIZATION && GC.getInitCore().getCivDescription(getID(), uiForm).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getDescription(uiForm);
	}
	else
	{
		return GC.getInitCore().getCivDescription(getID(), uiForm);
	}
}


void CvPlayer::setCivName(std::wstring szNewDesc, std::wstring szNewShort, std::wstring szNewAdj)
{
	m_szCivDesc = szNewDesc;
	m_szCivShort = szNewShort;
	m_szCivAdj = szNewAdj;
	gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(Foreign_Screen_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(Flag_DIRTY_BIT, true);
	gDLL->getEngineIFace()->SetDirty(CultureBorders_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
}


const wchar_t* CvPlayer::getCivilizationDescriptionKey() const
{
	/* dynamic civ names
	if (GC.getInitCore().getCivDescriptionKey(getID()).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getTextKeyWide();
	}
	else
	{
		return GC.getInitCore().getCivDescriptionKey(getID());
	}
	*/
	if( !(m_szCivDesc.empty()) )
	{
		return m_szCivDesc;
	}
	else if (GC.getInitCore().getCivDescriptionKey(getID()).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getTextKeyWide();
	}
	else
	{
		return GC.getInitCore().getCivDescriptionKey(getID());
	}
}


const wchar_t* CvPlayer::getCivilizationShortDescription(uint uiForm) const
{
	/* dynamic civ names
	if (GC.getInitCore().getCivShortDesc(getID(), uiForm).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getShortDescription(uiForm);
	}
	else
	{
		return GC.getInitCore().getCivShortDesc(getID(), uiForm);
	}
	*/
	if( !(m_szCivShort.empty()) )
	{
		return m_szCivShort;
	}
	else if (GC.getInitCore().getCivShortDesc(getID(), uiForm).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getShortDescription(uiForm);
	}
	else
	{
		return GC.getInitCore().getCivShortDesc(getID(), uiForm);
	}
}


const wchar_t* CvPlayer::getCivilizationShortDescriptionKey() const
{
	/* dynamic civ names
	if (GC.getInitCore().getCivShortDescKey(getID()).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getShortDescriptionKey();
	}
	else
	{
		return GC.getInitCore().getCivShortDescKey(getID());
	}
	*/
	if( !(m_szCivShort.empty()) )
	{
		return m_szCivShort;
	}
	else if (GC.getInitCore().getCivShortDescKey(getID()).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getShortDescriptionKey();
	}
	else
	{
		return GC.getInitCore().getCivShortDescKey(getID());
	}
}


const wchar_t* CvPlayer::getCivilizationAdjective(uint uiForm) const
{
	/* dynamic civ names
	if (GC.getInitCore().getCivAdjective(getID(), uiForm).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getAdjective(uiForm);
	}
	else
	{
		return GC.getInitCore().getCivAdjective(getID(), uiForm);
	}
	*/
	if( !(m_szCivAdj.empty()) )
	{
		return m_szCivAdj;
	}
	else if (GC.getInitCore().getCivAdjective(getID(), uiForm).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getAdjective(uiForm);
	}
	else
	{
		return GC.getInitCore().getCivAdjective(getID(), uiForm);
	}
}

const wchar_t* CvPlayer::getCivilizationAdjectiveKey() const
{
	/* dynamic civ names
	if (GC.getInitCore().getCivAdjectiveKey(getID()).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey();
	}
	else
	{
		return GC.getInitCore().getCivAdjectiveKey(getID());
	}
	*/
	if( !(m_szCivAdj.empty()) )
	{
		return m_szCivAdj;
	}
	else if (GC.getInitCore().getCivAdjectiveKey(getID()).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey();
	}
	else
	{
		return GC.getInitCore().getCivAdjectiveKey(getID());
	}
}


CvWString CvPlayer::getFlagDecal() const
{
	if (GC.getInitCore().getFlagDecal(getID()).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getFlagTexture();
	}
	else
	{
		return GC.getInitCore().getFlagDecal(getID());
	}
}

bool CvPlayer::isWhiteFlag() const
{
	if (GC.getInitCore().getFlagDecal(getID()).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getArtInfo()->isWhiteFlag();
	}
	else
	{
		return GC.getInitCore().getWhiteFlag(getID());
	}
}


const wchar_t* CvPlayer::getStateReligionName(uint uiForm) const
{
	if (getStateReligion() != NO_RELIGION)
	{
		return GC.getReligionInfo(getStateReligion()).getDescription(uiForm);
	}
	else
	{
		return gDLL->getText("TXT_KEY_MISC_NO_STATE_RELIGION");
	}
}

const wchar_t* CvPlayer::getStateReligionKey() const
{
	if (getStateReligion() != NO_RELIGION)
	{
		return GC.getReligionInfo(getStateReligion()).getTextKeyWide();
	}

	return L"TXT_KEY_MISC_NO_STATE_RELIGION";
}


const CvWString CvPlayer::getBestAttackUnitName(uint uiForm) const
{
	return gDLL->getObjectText((CvString)getBestAttackUnitKey(), uiForm, true);
}


const CvWString CvPlayer::getWorstEnemyName() const
{
	const TeamTypes eWorstEnemy = GET_TEAM(getTeam()).AI_getWorstEnemy();

	if (eWorstEnemy != NO_TEAM)
	{
		return GET_TEAM(eWorstEnemy).getName();
	}

	return "";
}

const wchar_t* CvPlayer::getBestAttackUnitKey() const
{
	PROFILE_EXTRA_FUNC();
	int iDummyValue;

	UnitTypes eBestUnit = NO_UNIT;

	CvCity* pCapitalCity = getCapitalCity();

	if (pCapitalCity)
	{
		eBestUnit = pCapitalCity->AI_bestUnitAI(UNITAI_ATTACK, iDummyValue, true);
	}

	if (eBestUnit == NO_UNIT)
	{
		foreach_(CvCity* pLoopCity, cities())
		{
			eBestUnit = pLoopCity->AI_bestUnitAI(UNITAI_ATTACK, iDummyValue, true);

			if (eBestUnit != NO_UNIT)
			{
				break;
			}
		}
	}

	if (eBestUnit != NO_UNIT)
	{
		return GC.getUnitInfo(eBestUnit).getTextKeyWide();
	}

	return L"TXT_KEY_MISC_NO_UNIT";
}


ArtStyleTypes CvPlayer::getArtStyleType() const
{
	if (GC.getInitCore().getArtStyle(getID()) == NO_ARTSTYLE)
	{
		return ((ArtStyleTypes)(GC.getCivilizationInfo(getCivilizationType()).getArtStyleType()));
	}
	else
	{
		return GC.getInitCore().getArtStyle(getID());
	}
}

const char* CvPlayer::getUnitButton(UnitTypes eUnit) const
{
	const CvArtInfoUnit * pUnitArtInfo = GC.getUnitInfo(eUnit).getArtInfo(0, getCurrentEra(), (UnitArtStyleTypes) GC.getCivilizationInfo(getCivilizationType()).getUnitArtStyleType());

	return pUnitArtInfo ? pUnitArtInfo->getButton() : GC.getUnitInfo(eUnit).getArtInfo(0, getCurrentEra(), NO_UNIT_ARTSTYLE)->getButton();
}

void CvPlayer::doTurn()
{
	PROFILE_FUNC();

	// Only decrement the GA counter at the end of this function if GA started before this point, i.e last turn.
	const bool bWasGoldenAgeLastTurn = getGoldenAgeTurns() > 0; 

#ifdef VALIDATION_FOR_PLOT_GROUPS
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		const CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if ( pLoopPlot->getPlotGroupId(getID()) != -1 && pLoopPlot->getPlotGroup(getID()) == NULL )
		{
			::MessageBox(NULL, "Invalid plot group id found!", "CvGameCoreDLL", MB_OK);
		}
	}
#endif

	//	Each turn flush the movement cost cache for each player to avoid it getting too large
	CvPlot::flushMovementCostCache();

#ifdef CAN_TRAIN_CACHING
	//	Clear training caches at the start of each turn
	algo::for_each(cities(), CvCity::fn::clearCanTrainCache());
#endif

	m_canHaveBuilder.clear();

#ifdef _DEBUG
	if (!GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS) && !GC.getGame().isSimultaneousTeamTurns())
	{
		foreach_(CvSelectionGroup* group, groups()
		| filtered(bind(CvSelectionGroup::isBusy, _1)))
		{
			if (group->getNumUnits() == 0)
			{
				FErrorMsg(CvString::format("Empty selection group lingering on start of turn for Player %d (%S)", getID(), getCivilizationDescription(0)).c_str());
				group->kill();
			}
			if (group->getMissionTimer() > 0)
			{
				FErrorMsg(CvString::format("Mission Timer running for selection group at start of turn for player %d (%S)", getID(), getCivilizationDescription(0)).c_str());
				group->setMissionTimer(0);
			}
			foreach_(CvUnit* unitX, group->units())
			{
				if (unitX->isCombat())
				{
					FErrorMsg(CvString::format("Unit %S (%d) at (%d,%d) in combat at start of turn for player %d (%S)", unitX->getDescription().c_str(), unitX->getID(), unitX->getX(), unitX->getY(), getID(), getCivilizationDescription(0)).c_str());

					if (unitX->getCombatUnit())
					{
						FErrorMsg(CvString::format("In combat with Unit %S (%d) at (%d,%d)", unitX->getCombatUnit()->getDescription().c_str(), unitX->getCombatUnit()->getID(), unitX->getCombatUnit()->getX(), unitX->getCombatUnit()->getY()).c_str());
						unitX->setCombatUnit(NULL);
					}
					if (unitX->getAttackPlot())
					{
						FErrorMsg(CvString::format("Unit %S (%d) at (%d,%d) attacking (%d, %d) at start of turn for player %d (%S)", unitX->getDescription().c_str(), unitX->getID(), unitX->getX(), unitX->getY(), unitX->getAttackPlot()->getX(), unitX->getAttackPlot()->getY(), getID(), getCivilizationDescription(0)).c_str());
						unitX->setAttackPlot(NULL, false);
					}
				}
			}
		}
	}
#endif

	FAssertMsg(isAlive(), "isAlive is expected to be true");
	FAssertMsg(!hasBusyUnit() || GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS)  || GC.getGame().isSimultaneousTeamTurns(), "End of turn with busy units in a sequential-turn game");

	CvEventReporter::getInstance().beginPlayerTurn( GC.getGame().getGameTurn(),  getID());

	setBuildingListInvalid();

	//	Reset cache of best route type to build
	m_eBestRoute = NO_ROUTE;

#ifdef CAN_BUILD_VALUE_CACHING
	CvPlot::ClearCanBuildCache();
#endif

	if (!isNPC())
	{
		while (canLeaderPromote())
		{
			doPromoteLeader();
		}
	}

	doUpdateCacheOnTurn();

	GC.getGame().verifyDeals();

	AI_doTurnPre();

	if (getRevolutionTimer() > 0)
	{
		changeRevolutionTimer(-1);
	}

	if (getConversionTimer() > 0)
	{
		changeConversionTimer(-1);
	}

	setConscriptCount(0);

	resetUpgradeRoundCount();

	AI_assignWorkingPlots();

	if (0 == GET_TEAM(getTeam()).getHasMetCivCount(true))
	{
		setCommercePercent(COMMERCE_ESPIONAGE, 0);
	}

	//	Recalculate city growth rates from scratch each turn
	recalculatePopulationgrowthratepercentage();

	verifyGoldCommercePercent();

	doGold();

	doResearch();

	doEspionagePoints();

//#if 0 // AI_doCentralizedProduction is unfinished, no point calling it. [11/12/2019 billw]
//	// New function to handle wonder construction in a centralized manner
//	AI_doCentralizedProduction();
//#endif

	//Clear the cache each turn.
	recalculateAllResourceConsumption();

	if (GC.getGame().isOption(GAMEOPTION_ADVANCED_REALISTIC_CORPORATIONS))
	{
		updateCorporateMaintenance();
	}
	doAdvancedEconomy();

	{
		PROFILE("CvPlayer::doTurn.DoCityTurn");

		algo::for_each(cities_safe(), CvCity::fn::doTurn());
	}

	if (GC.isDCM_OPP_FIRE())
	{
		algo::for_each(units(), CvUnit::fn::doOpportunityFire());
	}
	if (GC.isDCM_ACTIVE_DEFENSE())
	{
		algo::for_each(units(), CvUnit::fn::doActiveDefense());
	}

	if (getGoldenAgeTurns() > 0)
	{
		FAssert(getAnarchyTurns() < 1);

		if (bWasGoldenAgeLastTurn)
		{
			changeGoldenAgeTurns(-1);
		}
	}
	else if (getAnarchyTurns() > 0)
	{
		m_iNumAnarchyTurns++; // Increment stat counter for turns we have spent in anarchy
		changeAnarchyTurns(-1);
	}

	verifyCivics();

	updateTradeRoutes();

	updateWarWearinessPercentAnger();

	if (!isNPC())
	{
		doEvents();

#ifdef ENABLE_FOGWAR_DECAY
		//Calvitix, Modmod FOGWAR PlotDecay
		if (isHumanPlayer() || GC.getGame().getAIAutoPlay(getID()) > 0 || gDLL->GetAutorun())
		{
			CvGame& GAME = GC.getGame();
			if (GAME.isModderGameOption(MODDERGAMEOPTION_FOGWAR_DECAY) && GET_TEAM(getTeam()).getVisibilityDecay() != NO_DECAY)
				GC.getMap().updateFog(true); //Calvitix, to applyPlotDecay
		}
#endif
	}

	recordHistory();

	expireMessages(); // turn log

	gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);

	AI_doTurnPost();

	// Toffer - Completely acceptable to only cache these once per turn.
	if (!isNPC())
	{
		cacheKeyFinanceNumbers();
	}

	if (GC.getGame().isDebugMode())
	{
		GC.getGame().updateColoredPlots();
	}
	if (gPlayerLogLevel >= 1)
	{
		dumpStats();
	}
	CvEventReporter::getInstance().endPlayerTurn( GC.getGame().getGameTurn(),  getID());
}

void CvPlayer::doMultiMapTurn()
{
#ifdef VALIDATION_FOR_PLOT_GROUPS
	foreach_(const CvPlot* pLoopPlot, GC.getMap().plots())
	{
		if ( pLoopPlot->getPlotGroupId(getID()) != -1 && pLoopPlot->getPlotGroup(getID()) == NULL )
		{
			::MessageBox(NULL, "Invalid plot group id found!", "CvGameCoreDLL", MB_OK);
		}
	}
#endif

	//	Each turn flush the movement cost cache for each player to avoid it getting too large
	CvPlot::flushMovementCostCache();

#ifdef CAN_TRAIN_CACHING
	//	Clear training caches at the start of each turn
	algo::for_each(cities(), CvCity::fn::clearCanTrainCache());
#endif

	setBuildingListInvalid();

#ifdef CAN_BUILD_VALUE_CACHING
	CvPlot::ClearCanBuildCache();
#endif

	doUpdateCacheOnTurn();

	//AI_doTurnPre();

	AI_assignWorkingPlots();

	{
		PROFILE("CvPlayer::doTurn.DoCityTurn");

		algo::for_each(cities_safe(), CvCity::fn::doTurn());
	}

	if (GC.isDCM_OPP_FIRE())
	{
		algo::for_each(units(), CvUnit::fn::doOpportunityFire());
	}
	if (GC.isDCM_ACTIVE_DEFENSE())
	{
		algo::for_each(units(), CvUnit::fn::doActiveDefense());
	}

	updateTradeRoutes();

	doTurnUnits();
}

void CvPlayer::recordHistory()
{
	m_mapEconomyHistory[GC.getGame().getGameTurn()] = calculateTotalCommerce();
	m_mapPowerHistory[GC.getGame().getGameTurn()] = getPower();
	m_mapIndustryHistory[GC.getGame().getGameTurn()] = calculateTotalYield(YIELD_PRODUCTION);
	m_mapAgricultureHistory[GC.getGame().getGameTurn()] = calculateTotalYield(YIELD_FOOD);
	m_mapCultureHistory[GC.getGame().getGameTurn()] = getCulture();
	m_mapEspionageHistory[GC.getGame().getGameTurn()] = GET_TEAM(getTeam()).getEspionagePointsEver();
	m_mapRevolutionStabilityHistory[GC.getGame().getGameTurn()] = getStabilityIndexAverage();
}

// Dump stats to BBAI log
void CvPlayer::dumpStats() const
{
	PROFILE_EXTRA_FUNC();

	logBBAI("%S stats for turn %d:", getCivilizationDescription(0), GC.getGame().getGameTurn());

	logBBAI("	Gold rate: %d", getCommercePercent(COMMERCE_GOLD));
	logBBAI("	Science rate: %d", getCommercePercent(COMMERCE_RESEARCH));
	logBBAI("	Culture rate: %d", getCommercePercent(COMMERCE_CULTURE));
	logBBAI("	Espionage rate: %d", getCommercePercent(COMMERCE_ESPIONAGE));
	logBBAI("	Treasury: %d", getGold());
	logBBAI("	Total gold income from self: %d", getCommerceRate(COMMERCE_GOLD));
	logBBAI("	Total gold income from trade agreements: %d", getGoldPerTurn());
	logBBAI("	Num units: %d", getNumUnits());
	logBBAI("	Num selection groups: %d", getNumSelectionGroups());

	// Economy stats
	{
		const int64_t iUnitUpkeep = getFinalUnitUpkeep();
		const int iUnitSupplyCosts = calculateUnitSupply();
		const int iMaintenanceCosts = getTotalMaintenance();
		const int iCivicUpkeepCosts = getCivicUpkeep();
		const int64_t iCorporateMaintenance = getCorporateMaintenance();
		const int64_t iTotalPreInflatedCosts = iUnitUpkeep + iUnitSupplyCosts + iMaintenanceCosts + iCivicUpkeepCosts + iCorporateMaintenance;
		const int64_t iTotalCosts = iTotalPreInflatedCosts * std::max(0, (getInflationMod10000() / 100 - 100) + 100) / 100;
		logBBAI("	Unit Upkeep (pre inflation): %I64u", iUnitUpkeep);
		logBBAI("	Unit supply cost (pre inflation): %d", iUnitSupplyCosts);
		logBBAI("	Maintenance cost (pre inflation): %d", iMaintenanceCosts);
		logBBAI("	Civic upkeep cost (pre inflation): %d", iCivicUpkeepCosts);
		logBBAI("	Corporate maintenance (pre inflation): %I64d", iCorporateMaintenance);
		logBBAI("	Inflation effect: %I64d", iTotalCosts - iTotalPreInflatedCosts);
	}
	logBBAI("	Is in financial difficulties: %s", AI_isFinancialTrouble() ? "yes" : "no");
	logBBAI("	Total science output: %d", calculateResearchRate());
	logBBAI("	Total espionage output: %d", getCommerceRate(COMMERCE_ESPIONAGE));
	logBBAI("	Total cultural output: %d", getCommerceRate(COMMERCE_CULTURE));

	// Accrue some stats off cities
	{
		int iTotalFood = 0;
		int iTotalProduction = 0;
		int	iTotalPopulation = 0;
		int	iCityCount = 0;
		foreach_(const CvCity* cityX, cities())
		{
			iTotalFood			+= cityX->getYieldRate(YIELD_FOOD);
			iTotalProduction	+= cityX->getYieldRate(YIELD_PRODUCTION);
			iTotalPopulation	+= cityX->getPopulation();
			iCityCount++;
		}
		logBBAI("	Total population: %d", iTotalPopulation);
		logBBAI("	Total food output: %d", iTotalFood);
		logBBAI("	Total production output: %d", iTotalProduction);
		logBBAI("	Num cities: %d", iCityCount);
	}
	logBBAI("	National rev index: %d", getRevIdxNational());
	logBBAI("	Number of barbarian units killed: %d", getWinsVsBarbs());
	logBBAI("	Number of animals subdued: %d", m_iNumAnimalsSubdued);
	logBBAI("	Civic switches: %d", m_iNumCivicSwitches);
	logBBAI("	Total num civics switched: %d", m_iNumCivicsSwitched);
	logBBAI("	Total turns in anarchy: %d (%d%%%%)", m_iNumAnarchyTurns, (100*m_iNumAnarchyTurns)/(std::max(1,GC.getGame().getGameTurn())));
	logBBAI("	Current civics:");

	for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		const CivicTypes eCivic = getCivics((CivicOptionTypes)iI);

		if (eCivic == NO_CIVIC)
		{
			logBBAI("		%S: NONE", GC.getCivicOptionInfo((CivicOptionTypes)iI).getDescription());
		}
		else logBBAI("		%S: %S", GC.getCivicOptionInfo((CivicOptionTypes)iI).getDescription(), GC.getCivicInfo(eCivic).getDescription());
	}

	//logBBAI("	Civic switch history:");
	//{
	//	int iTurn = -1;
	//	for (int iI = 0; iI < (int)m_civicSwitchHistory.size(); iI++)
	//	{
	//		if (m_civicSwitchHistory[iI].iTurn != iTurn)
	//		{
	//			iTurn = m_civicSwitchHistory[iI].iTurn;
	//			logBBAI("		Turn %d:", iTurn);
	//		}
	//		logBBAI("			%S -> %S%s",
	//				m_civicSwitchHistory[iI].eFromCivic == NO_CIVIC ? L"Unknown" : GC.getCivicInfo((CivicTypes)m_civicSwitchHistory[iI].eFromCivic).getDescription(),
	//				m_civicSwitchHistory[iI].eToCivic == NO_CIVIC ? L"Unknown" : GC.getCivicInfo((CivicTypes)m_civicSwitchHistory[iI].eToCivic).getDescription(),
	//				m_civicSwitchHistory[iI].bNoAnarchy ? " (no anarchy switch)" : "");
	//	}
	//	if (iTurn == -1)
	//	{
	//		logBBAI("		No switches made");
	//	}
	//}

	//	City stats
	foreach_(const CvCity* pLoopCity, cities())
	{
		logBBAI("		City %S:", pLoopCity->getName().c_str());
		logBBAI("			Population: %d", pLoopCity->getPopulation());
		logBBAI("			Production: %d", pLoopCity->getYieldRate(YIELD_PRODUCTION));
		logBBAI("			Food surplus: %d", pLoopCity->foodDifference());
		logBBAI("			Local rev index: %d", pLoopCity->getLocalRevIndex());
		logBBAI("			Maintenance: %d", pLoopCity->getMaintenance());
		logBBAI("			Income: %d", pLoopCity->getCommerceRate(COMMERCE_GOLD));
		logBBAI("			Science: %d", pLoopCity->getCommerceRate(COMMERCE_RESEARCH));
		logBBAI("			Espionage: %d", pLoopCity->getCommerceRate(COMMERCE_ESPIONAGE));
		logBBAI("			Culture: %d", pLoopCity->getCommerceRate(COMMERCE_CULTURE));
		logBBAI("			Net happyness: %d", pLoopCity->happyLevel() - pLoopCity->unhappyLevel());
		logBBAI("			Net health: %d", pLoopCity->goodHealth() - pLoopCity->badHealth());
		logBBAI("			Food trade yield: %d", pLoopCity->getTradeYield(YIELD_FOOD));
		logBBAI("			Production trade yield: %d", pLoopCity->getTradeYield(YIELD_PRODUCTION));
		logBBAI("			Commerce trade yield: %d", pLoopCity->getTradeYield(YIELD_COMMERCE));
		logBBAI("			Properties:");

		const CvProperties* pProperties = pLoopCity->getGameObject()->getProperties();

		for(int iJ = 0; iJ < pProperties->getNumProperties(); iJ++)
		{
			const PropertyTypes eProperty = (PropertyTypes)pProperties->getProperty(iJ);

			logBBAI("				%S: value(%d) change(%d)", GC.getPropertyInfo(eProperty).getDescription(), pProperties->getValue(iJ), pProperties->getChangeByProperty(eProperty));

		}

		logBBAI("			PropertyBuildings:");

		//for (int iJ = 0; iJ < pProperties->getNumProperties(); iJ++)
		//{
		//	const CvPropertyInfo& kInfo = GC.getPropertyInfo((PropertyTypes)pProperties->getProperty(iJ));

		//	for (int i = kInfo.getNumPropertyBuildings() - 1; i > -1; i--)
		//	{
		//		const PropertyBuilding& kBuilding = kInfo.getPropertyBuilding(i);
		//		if (pLoopCity->hasBuilding(kBuilding.eBuilding))
		//		{
		//			logBBAI("				%S: %S", kInfo.getDescription(), GC.getBuildingInfo(kBuilding.eBuilding).getDescription());
		//		}
		//	}
		//}
	}

	//	Unit stats
	std::map<int, int> unitCounts;

	logBBAI("	Units:");
	foreach_(const CvUnit* pLoopUnit, units())
	{
		int iMapKey = pLoopUnit->AI_getUnitAIType() + (pLoopUnit->getUnitType() << 16);

		std::map<int,int>::iterator itr = unitCounts.find(iMapKey);

		if (itr == unitCounts.end())
		{
			unitCounts.insert(std::make_pair(iMapKey, 1));
		}
		else (itr->second)++;
	}
	for (std::map<int,int>::const_iterator itr = unitCounts.begin(); itr != unitCounts.end(); ++itr)
	{
		logBBAI("		%S (%s): %d", GC.getUnitInfo((UnitTypes)(itr->first >> 16)).getDescription(), GC.getUnitAIInfo((UnitAITypes)(itr->first & 0xFFFF)).getType(), itr->second);
	}
	logBBAI("	EndUnits");

}


void CvPlayer::NoteAnimalSubdued()
{
	m_iNumAnimalsSubdued++;
}

void CvPlayer::NoteCivicsSwitched(int iNumChanges)
{
	m_iNumCivicSwitches++;
	m_iNumCivicsSwitched += iNumChanges;
}


void CvPlayer::doTurnUnits()
{
	PROFILE_FUNC();

	AI_doTurnUnitsPre();

	m_guardableResourcePlots.clear();

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		const CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot->getOwner() == getID())
		{
			const BonusTypes eNonObsoleteBonus = pLoopPlot->getNonObsoleteBonusType(getTeam());

			if (eNonObsoleteBonus != NO_BONUS)
			{
				const ImprovementTypes eImprovement = pLoopPlot->getImprovementType();

				if ((eImprovement != NO_IMPROVEMENT) && GC.getImprovementInfo(eImprovement).isImprovementBonusTrade(eNonObsoleteBonus))
				{
					m_guardableResourcePlots[iI] = eNonObsoleteBonus;
				}
			}
		}
	}

	algo::for_each(groups(), CvSelectionGroup::fn::doDelayedDeath());

	algo::for_each(groups(), CvSelectionGroup::fn::resetHealing());

	for (int iPass = 0; iPass < 4; iPass++)
	{
		foreach_(CvSelectionGroup* pLoopSelectionGroup, groups())
		{
			if ( pLoopSelectionGroup->plot() )
			{
				switch (pLoopSelectionGroup->getDomainType())
				{
				case DOMAIN_AIR:
					if (iPass == 1)
					{
						pLoopSelectionGroup->doTurn();
					}
					break;
				case DOMAIN_SEA:
					if (iPass == 2)
					{
						pLoopSelectionGroup->doTurn();
					}
					break;
				case DOMAIN_LAND:
					if (iPass == 3)
					{
						pLoopSelectionGroup->doTurn();
					}
					break;
				case DOMAIN_IMMOBILE:
					if (iPass == 0)
					{
						pLoopSelectionGroup->doTurn();
					}
					break;
				case NO_DOMAIN:
					FAssertMsg(!pLoopSelectionGroup->getHeadUnit(), "Unit with no Domain");
					break;
				default:
					if (iPass == 3)
					{
						pLoopSelectionGroup->doTurn();
					}
					break;
				}
			}
		}
	}

	if (getID() == GC.getGame().getActivePlayer())
	{
		gDLL->getFAStarIFace()->ForceReset(&GC.getInterfacePathFinder());

		gDLL->getInterfaceIFace()->setDirty(Waypoints_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
	}

	gDLL->getInterfaceIFace()->setDirty(UnitInfo_DIRTY_BIT, true);

	AI_doTurnUnitsPost();
}


void CvPlayer::verifyCivics()
{
	PROFILE_FUNC();

	if (!isAnarchy())
	{
		for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
		{
			if (!canDoCivics(getCivics((CivicOptionTypes)iI)))
			{
				for (int iJ = 0; iJ < GC.getNumCivicInfos(); iJ++)
				{
					if (GC.getCivicInfo((CivicTypes)iJ).getCivicOptionType() == iI)
					{
						if (canDoCivics((CivicTypes)iJ))
						{
							setCivics(((CivicOptionTypes)iI), ((CivicTypes)iJ));
							break;
						}
					}
				}
			}
		}
	}
}

void CvPlayer::inhibitPlotGroupCalcsUntilFullRebuild()	//	Ignore updates until an update with reInitialize set
{
	m_bInhibitPlotGroupRecalc = true;
}

void CvPlayer::updatePlotGroups(const CvArea* possibleNewInAreaOnly, bool reInitialize)
{
	PROFILE_FUNC();

	if (!(GC.getGame().isFinalInitialized()))
	{
		return;
	}

	if ( m_bInhibitPlotGroupRecalc && !reInitialize )
	{
		return;
	}

	m_bInhibitPlotGroupRecalc = false;

	algo::for_each(cities(), CvCity::fn::startDeferredBonusProcessing());

	if ( possibleNewInAreaOnly == NULL )
	{
		if ( reInitialize )
		{
			//	Throw away all existing plot groups
			for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
			{
				GC.getMap().plotByIndex(iI)->setPlotGroup(getID(), NULL, false);
			}
			m_plotGroups[CURRENT_MAP]->removeAll();
		}
		else
		{
			algo::for_each(plot_groups(), CvPlotGroup::fn::recalculatePlots());
		}
	}

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if ( possibleNewInAreaOnly == NULL || pLoopPlot->area() == possibleNewInAreaOnly )
		{
			if ( pLoopPlot->getPlotGroup(getID()) == NULL )
			{
				CvPlotGroup::colorRegion(pLoopPlot, getID(), true);
			}
			//pLoopPlot->updatePlotGroup(getID(), false);
		}
	}

#ifdef VALIDATION_FOR_PLOT_GROUPS
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		const CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if ( pLoopPlot->getPlotGroupId(getID()) != -1 && pLoopPlot->getPlotGroup(getID()) == NULL )
		{
			::MessageBox(NULL, "Invalid plot group id found after recalc!", "CvGameCoreDLL", MB_OK);
		}
	}
#endif

	algo::for_each(cities(), CvCity::fn::endDeferredBonusProcessing());

	updateTradeRoutes();
}

void CvPlayer::RecalculatePlotGroupHashes()
{
	PROFILE_FUNC();

	algo::for_each(plot_groups(), CvPlotGroup::fn::RecalculateHashes());
}

void CvPlayer::updateYield()
{
	algo::for_each(cities(), CvCity::fn::updateYield());
}


void CvPlayer::updateFeatureHappiness(bool bLimited)
{
	algo::for_each(cities(), CvCity::fn::updateFeatureHappiness(bLimited));
}

void CvPlayer::updateReligionHappiness(bool bLimited)
{
	algo::for_each(cities(), CvCity::fn::updateReligionHappiness(bLimited));
}

void CvPlayer::updateExtraSpecialistYield()
{
	algo::for_each(cities(), CvCity::fn::updateExtraSpecialistYield());
}


void CvPlayer::updateCommerce(CommerceTypes eCommerce, bool bForce) const
{
	PROFILE_EXTRA_FUNC();
	if ( eCommerce == NO_COMMERCE )
	{
		for(int iI = 0; iI < NUM_COMMERCE_TYPES; iI++ )
		{
			updateCommerce((CommerceTypes)iI, bForce);
		}
	}
	else if (bForce || m_abCommerceDirty[eCommerce])
	{
		m_abCommerceDirty[eCommerce] = false;

		algo::for_each(cities(), CvCity::fn::updateCommerce(eCommerce, bForce));
	}
}

void CvPlayer::setCommerceDirty(CommerceTypes eIndex, bool bPlayerOnly)
{
	PROFILE_EXTRA_FUNC();
	if (eIndex == NO_COMMERCE)
	{
		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			setCommerceDirty((CommerceTypes)iI, bPlayerOnly);
		}
	}
	else
	{
		m_abCommerceDirty[eIndex] = true;

		if (!bPlayerOnly)
		{
			algo::for_each(cities(), CvCity::fn::setCommerceDirty(eIndex));
		}
	}
}

void CvPlayer::updateBuildingCommerce()
{
	algo::for_each(cities(), CvCity::fn::updateBuildingCommerce());
}


void CvPlayer::updateReligionCommerce()
{
	algo::for_each(cities(), CvCity::fn::updateReligionCommerce());
}


void CvPlayer::updateCorporation()
{
	for_each(cities(), CvCity::fn::updateCorporation());
}


void CvPlayer::updateCitySight(bool bIncrement, bool bUpdatePlotGroups)
{
	PROFILE_EXTRA_FUNC();
	foreach_(const CvCity* pLoopCity, cities())
	{
		pLoopCity->plot()->updateSight(bIncrement, bUpdatePlotGroups);
	}
}


void CvPlayer::updateTradeRoutes()
{
	PROFILE_FUNC();

	CLLNode<int>* pCityNode;
	CLinkList<int> cityList;

	algo::for_each(cities(), CvCity::fn::clearTradeRoutes());

	cityList.clear();

	foreach_(const CvCity* pLoopCity, cities())
	{
		const int iTotalTradeModifier = pLoopCity->totalTradeModifier();

		pCityNode = cityList.head();

		while (pCityNode)
		{
			const CvCity* pListCity = getCity(pCityNode->m_data);

			if (iTotalTradeModifier > pListCity->totalTradeModifier())
			{
				cityList.insertBefore(pLoopCity->getID(), pCityNode);
				break;
			}
			else
			{
				pCityNode = cityList.next(pCityNode);
			}
		}

		if (pCityNode == NULL)
		{
			cityList.insertAtEnd(pLoopCity->getID());
		}
	}

	pCityNode = cityList.head();

	while (pCityNode)
	{
		getCity(pCityNode->m_data)->updateTradeRoutes();
		pCityNode = cityList.next(pCityNode);
	}
}

void CvPlayer::updatePlunder(int iChange, bool bUpdatePlotGroups)
{
	algo::for_each(units() | filtered(CvUnit::fn::isBlockading()),
		CvUnit::fn::updatePlunder(iChange, bUpdatePlotGroups)
	);
}

void CvPlayer::updateTimers()
{
	algo::for_each(groups(), CvSelectionGroup::fn::updateTimers()); // could destroy the selection group...

	// if a unit was busy, perhaps it was not quite deleted yet, give it one more try
	if (getNumSelectionGroups() > getNumUnits())
	{
		algo::for_each(groups(), CvSelectionGroup::fn::doDelayedDeath()); // could destroy the selection group...
	}
	FAssertMsg(getNumSelectionGroups() <= getNumUnits(), "The number of Units is expected not to exceed the number of Selection Groups");
}


bool CvPlayer::hasReadyUnautomatedUnit(bool bAny) const
{
	PROFILE_FUNC();

	return algo::any_of(groups() | filtered(!CvSelectionGroup::fn::isAutomated()),
		CvSelectionGroup::fn::readyToMove(bAny, !isHumanPlayer())
	);
}

bool CvPlayer::hasReadyUnit(bool bAny) const
{
	PROFILE_FUNC();

	return algo::any_of(groups(), CvSelectionGroup::fn::readyToMove(bAny, !isHumanPlayer()));
}


bool CvPlayer::hasAutoUnit() const
{
	PROFILE_FUNC();

	return algo::any_of(groups(), CvSelectionGroup::fn::readyToAuto());
}


bool CvPlayer::hasBusyUnit() const
{
	PROFILE_FUNC();

	foreach_(CvSelectionGroup* pLoopSelectionGroup, groups()
	| filtered(bind(CvSelectionGroup::isBusy, _1)))
	{
		return true;
	}
	return false;
}


bool CvPlayer::isChoosingFreeTech() const
{
	return (m_iChoosingFreeTech > 0);
}

void CvPlayer::startChoosingFreeTech()
{
	m_iChoosingFreeTech++;
}

void CvPlayer::endChoosingFreeTech()
{
	if (m_iChoosingFreeTech > 0)
	{
		m_iChoosingFreeTech--;
	}
}


void CvPlayer::chooseTech(int iDiscover, CvWString szText, bool bFront)
{
	if (iDiscover > 0)
	{
		startChoosingFreeTech();
	}

	CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSETECH);
	if (pInfo)
	{
		pInfo->setData1(iDiscover);
		pInfo->setText(szText);
		gDLL->getInterfaceIFace()->addPopup(pInfo, getID(), false, bFront);
	}
}


int CvPlayer::calculateScore(bool bFinal, bool bVictory) const
{
	PROFILE_FUNC();

	if (!isAlive())
	{
		return 0;
	}

	if (GET_TEAM(getTeam()).getNumMembers() == 0)
	{
		return 0;
	}

	return Cy::call<int>(PYGameModule, "calculateScore", Cy::Args() << getID() << bFinal << bVictory);

// A python clean-up by Toffer90 made the above code as fast as the hardcoded solution that's below.
// I'll keep this commented out in the case we ever need it again, however unlikely that is. - KaTiON

/*
	int iPopulationScore = getScoreComponent(getPopScore(), GC.getGame().getInitPopulation(), GC.getGame().getMaxPopulation(), GC.getSCORE_POPULATION_FACTOR(), true, bFinal, bVictory);

	int iLandScore = getScoreComponent(getLandScore(), GC.getGame().getInitLand(), GC.getGame().getMaxLand(), GC.getSCORE_LAND_FACTOR(), true, bFinal, bVictory);

	int iTechScore = getScoreComponent(getTechScore(), GC.getGame().getInitTech(), GC.getGame().getMaxTech(), GC.getSCORE_TECH_FACTOR(), true, bFinal, bVictory);

	int iWondersScore = getScoreComponent(getWondersScore(), GC.getGame().getInitWonders(), GC.getGame().getMaxWonders(), GC.getSCORE_WONDER_FACTOR(), false, bFinal, bVictory);

	return iPopulationScore + iLandScore + iTechScore + iWondersScore;
*/
}


int CvPlayer::findBestFoundValue() const
{
	return algo::max_element(GC.getMap().areas()
		| transformed(CvArea::fn::getBestFoundValue(getID()))
	).get_value_or(0);
}

int CvPlayer::upgradeAllPrice(UnitTypes eUpgradeUnit, UnitTypes eFromUnit) const
{
	// Loop through units and determine the total power of this player's military
	return algo::accumulate(units()
		| filtered(CvUnit::fn::getUnitType() == eFromUnit && CvUnit::fn::canUpgrade(eUpgradeUnit, true))
		| transformed(CvUnit::fn::upgradePrice(eUpgradeUnit)),
		0
	);
}


int CvPlayer::countReligionSpreadUnits(const CvArea* pArea, ReligionTypes eReligion, bool bIncludeTraining) const
{
	PROFILE_FUNC();

	int iCount = 0;

	foreach_(const CvUnit* unitX, units())
	{
		if (!unitX->isDead()
		&& unitX->getArea() == pArea->getID()
		&& unitX->getUnitInfo().getReligionSpreads(eReligion) > 0)
		{
			iCount++;
		}
	}

	if (bIncludeTraining)
	{
		foreach_(const CvCity* pLoopCity, cities())
		{
			const UnitTypes eUnit = pLoopCity->getProductionUnit();

			if (eUnit != NO_UNIT && GC.getUnitInfo(eUnit).getReligionSpreads(eReligion) > 0)
			{
				iCount++;
			}
		}
	}

	return iCount;
}

int CvPlayer::countCorporationSpreadUnits(const CvArea* pArea, CorporationTypes eCorporation, bool bIncludeTraining) const
{
	PROFILE_FUNC();

	int iCount = 0;
	foreach_(const CvUnit* pLoopUnit, units())
	{
		if (pLoopUnit->area() == pArea)
		{
			if (pLoopUnit->getUnitInfo().getCorporationSpreads(eCorporation) > 0)
			{
				++iCount;
			}
		}
	}

	if( bIncludeTraining )
	{
		foreach_(const CvCity* pLoopCity, cities())
		{
			const UnitTypes eUnit = pLoopCity->getProductionUnit();

			if( eUnit != NO_UNIT )
			{
				if(GC.getUnitInfo(eUnit).getCorporationSpreads(eCorporation) > 0)
				{
					iCount++;
				}
			}
		}
	}

	return iCount;
}


int CvPlayer::countNumCoastalCities() const
{
	return algo::count_if(cities(), CvCity::fn::isCoastal(GC.getWorldInfo(GC.getMap().getWorldSize()).getOceanMinAreaSize()));
}


int CvPlayer::countNumCoastalCitiesByArea(const CvArea* pArea) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	const int iAreaID = pArea->getID();
	foreach_(const CvCity* pLoopCity, cities())
	{
		if (pLoopCity->isCoastal(GC.getWorldInfo(GC.getMap().getWorldSize()).getOceanMinAreaSize()))
		{
			if ((pLoopCity->getArea() == iAreaID) || pLoopCity->plot()->isAdjacentToArea(iAreaID))
			{
				iCount++;
			}
		}
	}

	return iCount;
}


int CvPlayer::countNumCitiesWithOrbitalInfrastructure() const
{
	if (m_orbitalInfrastructureCountDirty)
	{
		m_orbitalInfrastructureCount = algo::count_if(cities(), CvCity::fn::hasOrbitalInfrastructure());
		m_orbitalInfrastructureCountDirty = false;
	}

	return m_orbitalInfrastructureCount;
}


int CvPlayer::countOwnedBonuses(BonusTypes eBonus) const
{
	PROFILE("CvPlayer::countOwnedBonuses");

	if (eBonus < 0 || eBonus >= GC.getNumBonusInfos())
	{
		FErrorMsg("Bonus value must be valid for countOwnedBonuses");
		return 0;
	}

	if (m_cachedBonusCount == NULL)
	{
		m_cachedBonusCount = new int[GC.getNumBonusInfos()];
		m_cachedBonusCountGameTurn = -1;
	}

	// We keep a per-turn cached result value for each bonus type.
	// This only gives an approximate tally, since bonuses can change hands mid-turn,
	// but for the purpose of the evaluation that's plenty good enough.
	if (GC.getGame().getGameTurn() != m_cachedBonusCountGameTurn)
	{
		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			m_cachedBonusCount[iI] = 0;
		}
		const bool bAdvancedStart = (getAdvancedStartPoints() >= 0) && (getCurrentEra() < 3);

		// Count bonuses outside city radius
		for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

			if (pLoopPlot->getOwner() == getID() && !pLoopPlot->isCityRadius()
			||	bAdvancedStart && pLoopPlot->isRevealed(getTeam(), false))
			{
				const BonusTypes ePlotBonus = pLoopPlot->getBonusType(getTeam());

				if (NO_BONUS != ePlotBonus)
				{
					m_cachedBonusCount[ePlotBonus]++;
				}
			}
		}
		// Count bonuses inside city radius or easily claimed
		foreach_(const CvCity* pLoopCity, cities())
		{
			const bool bCommerceCulture = (pLoopCity->getCommerceRate(COMMERCE_CULTURE) > 0);

			for (int iJ = 0; iJ < GC.getNumBonusInfos(); iJ++)
			{
				m_cachedBonusCount[iJ] += pLoopCity->AI_countNumBonuses((BonusTypes)iJ, true, bCommerceCulture, -1);
			}
		}
		m_cachedBonusCountGameTurn = GC.getGame().getGameTurn();
	}

	return m_cachedBonusCount[eBonus];
}


int CvPlayer::countUnimprovedBonuses(const CvArea* pArea, const CvPlot* pFromPlot) const
{
	PROFILE_FUNC();

	if (pFromPlot)
	{
		gDLL->getFAStarIFace()->ForceReset(&GC.getBorderFinder());
	}
	int iCount = 0;

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		const CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot->area() != pArea || pLoopPlot->getOwner() != getID() || pLoopPlot->isCity())
		{
			continue;
		}
		const BonusTypes eNonObsoleteBonus = pLoopPlot->getNonObsoleteBonusType(getTeam());

		if (eNonObsoleteBonus == NO_BONUS)
		{
			continue;
		}
		const ImprovementTypes eImprovement = pLoopPlot->getImprovementType();

		if (
			// Already improved correctly?
			eImprovement != NO_IMPROVEMENT
			&&
			GC.getImprovementInfo(eImprovement).isImprovementBonusTrade(eNonObsoleteBonus)
		|| // Unreachable?
			pFromPlot
			&&
			!gDLL->getFAStarIFace()->GeneratePath(&GC.getBorderFinder(), pFromPlot->getX(), pFromPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY(), false, getID(), true))
		{
			continue;
		}
		for (int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
		{
			const BuildTypes eBuild = (BuildTypes) iJ;

			if (GC.getBuildInfo(eBuild).getImprovement() != NO_IMPROVEMENT
			&& GC.getImprovementInfo(GC.getBuildInfo(eBuild).getImprovement()).isImprovementBonusTrade(eNonObsoleteBonus)
			&& canBuild(pLoopPlot, eBuild))
			{
				iCount++;
			}
		}
	}
	return iCount;
}


int CvPlayer::countCityFeatures(FeatureTypes eFeature) const
{
	PROFILE_FUNC();

	int iCount = 0;

	foreach_(const CvCity* city, cities())
	{
		iCount += algo::count_if(city->plots(NUM_CITY_PLOTS), CvPlot::fn::getFeatureType() == eFeature);
	}

	return iCount;
}


int CvPlayer::countNumBuildings(BuildingTypes eBuilding) const
{
	return algo::accumulate(cities() | transformed(CvCity::fn::hasBuilding(eBuilding)), 0);
}

bool CvPlayer::hasBuilding(const BuildingTypes eBuilding) const
{
	return eBuilding != NO_BUILDING && algo::any_of(cities(), CvCity::fn::hasBuilding(eBuilding));
}


int CvPlayer::countNumCitiesConnectedToCapital() const
{
	return algo::count_if(cities(), CvCity::fn::isConnectedToCapital());
}


int CvPlayer::countPotentialForeignTradeCities(const CvArea* pIgnoreArea) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getTeam()
		&& GET_TEAM(getTeam()).isFreeTrade((TeamTypes)iI))
		{
			int iTempValue = GET_TEAM((TeamTypes)iI).getNumCities();

			if (pIgnoreArea)
			{
				iTempValue -= GET_TEAM((TeamTypes)iI).countNumCitiesByArea(pIgnoreArea);
			}
			iCount += iTempValue;
		}
	}
	return iCount;
}


int CvPlayer::countPotentialForeignTradeCitiesConnected() const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	const CvCity* capitalCity = getCapitalCity();
	if (capitalCity)
	{
		const CvPlotGroup* pCapitalCityPlotGroup = capitalCity->plotGroup(getID());

		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getTeam(), false)
			&& GET_TEAM(getTeam()).isFreeTrade(GET_PLAYER((PlayerTypes)iI).getTeam()))
			{
				iCount += algo::count_if(GET_PLAYER((PlayerTypes)iI).cities(), CvCity::fn::plotGroup(getID()) == pCapitalCityPlotGroup);
			}
		}
	}
	return iCount;
}



bool CvPlayer::canContact(PlayerTypes ePlayer) const
{
	if (ePlayer == getID())
		return false;

	const CvPlayer& otherPlayer = GET_PLAYER(ePlayer);
	if (!isAlive() || !otherPlayer.isAlive())
		return false;

	if (isNPC() || otherPlayer.isNPC())
		return false;

	if (isMinorCiv() || otherPlayer.isMinorCiv())
		return false;

	const TeamTypes ourTeam = getTeam();
	const TeamTypes theirTeam = otherPlayer.getTeam();

	if (ourTeam != theirTeam)
		{
		if (!GET_TEAM(ourTeam).isHasMet(theirTeam))
			return false;

		if (::atWar(ourTeam, theirTeam) && !GET_TEAM(ourTeam).canChangeWarPeace(theirTeam))
			return false;
		}

	return true;
}


void CvPlayer::contact(PlayerTypes ePlayer)
{
	CvDiploParameters* pDiplo;

	if (!canContact(ePlayer) || isTurnDone())
	{
		return;
	}

	if (GET_PLAYER(ePlayer).isHumanPlayer())
	{
		if (GC.getGame().isPbem() || GC.getGame().isHotSeat() || (GC.getGame().isPitboss() && !gDLL->isConnected(GET_PLAYER(ePlayer).getNetID())))
		{
			if (gDLL->isMPDiplomacy())
			{
				gDLL->beginMPDiplomacy(ePlayer, false, false);
			}
		}
		else
		{
			if (gDLL->getInterfaceIFace()->isFlashing(ePlayer))
			{
				if (!gDLL->getInterfaceIFace()->isDiplomacyLocked())
				{
					gDLL->getInterfaceIFace()->setDiplomacyLocked(true);
					gDLL->sendContactCiv(NETCONTACT_RESPONSE, ePlayer);
				}
			}
			else
			{
				gDLL->sendContactCiv(NETCONTACT_INITIAL, ePlayer);
			}
		}
	}
	else
	{
		pDiplo = new CvDiploParameters(ePlayer);
		FAssert(pDiplo);
		if (gDLL->ctrlKey())
		{
			pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_TRADING"));
		}
		gDLL->getInterfaceIFace()->setDiploQueue(pDiplo, GC.getGame().getActivePlayer());
	}
}


void CvPlayer::handleDiploEvent(DiploEventTypes eDiploEvent, PlayerTypes ePlayer, int iData1, int iData2)
{
	PROFILE_EXTRA_FUNC();
	//OutputDebugString(CvString::format("UI interaction - diplo event for player %d (with player %d)\n", m_eID, ePlayer).c_str());

	switch (eDiploEvent)
	{
		case DIPLOEVENT_CONTACT:
		{
			AI_setFirstContact(ePlayer, true);
			GET_PLAYER(ePlayer).AI_setFirstContact(getID(), true);
			break;
		}
		case DIPLOEVENT_AI_CONTACT:
		{
			break;
		}
		case DIPLOEVENT_FAILED_CONTACT:
		{
			AI_setFirstContact(ePlayer, true);
			GET_PLAYER(ePlayer).AI_setFirstContact(getID(), true);
			break;
		}
		case DIPLOEVENT_GIVE_HELP:
		{
			AI_changeMemoryCount(ePlayer, MEMORY_GIVE_HELP, 1);
			forcePeace(ePlayer);
			break;
		}
		case DIPLOEVENT_REFUSED_HELP:
		{
			AI_changeMemoryCount(ePlayer, MEMORY_REFUSED_HELP, 1);
			break;
		}
		case DIPLOEVENT_ACCEPT_DEMAND:
		{
			AI_changeMemoryCount(ePlayer, MEMORY_ACCEPT_DEMAND, 1);
			forcePeace(ePlayer);
			break;
		}
		case DIPLOEVENT_REJECTED_DEMAND:
		{
			FAssertMsg(GET_PLAYER(ePlayer).getTeam() != getTeam(), "shouldn't call this function on our own team");

			AI_changeMemoryCount(ePlayer, MEMORY_REJECTED_DEMAND, 1);

			if (AI_demandRebukedSneak(ePlayer))
			{
				GET_TEAM(getTeam()).AI_setWarPlan(GET_PLAYER(ePlayer).getTeam(), WARPLAN_PREPARING_LIMITED);
			}
			break;
		}
		case DIPLOEVENT_DEMAND_WAR:
		{
			FAssertMsg(GET_PLAYER(ePlayer).getTeam() != getTeam(), "shouldn't call this function on our own team");

			if (gTeamLogLevel >= 2)
			{
				logBBAI("	Team %d (%S) declares war on team %d due to DIPLOEVENT", getTeam(), getCivilizationDescription(0), ePlayer );
			}
			GET_TEAM(getTeam()).declareWar(GET_PLAYER(ePlayer).getTeam(), false, WARPLAN_LIMITED);
			break;
		}
		case DIPLOEVENT_CONVERT:
		{
			AI_changeMemoryCount(ePlayer, MEMORY_ACCEPTED_RELIGION, 1);
			GET_PLAYER(ePlayer).convert(getStateReligion());
			break;
		}
		case DIPLOEVENT_NO_CONVERT:
		{
			AI_changeMemoryCount(ePlayer, MEMORY_DENIED_RELIGION, 1);
			break;
		}
		case DIPLOEVENT_REVOLUTION:
		{
			AI_changeMemoryCount(ePlayer, MEMORY_ACCEPTED_CIVIC, 1);

			CivicTypes* paeNewCivics = new CivicTypes[GC.getNumCivicOptionInfos()];

			for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
			{
				paeNewCivics[iI] = GET_PLAYER(ePlayer).getCivics((CivicOptionTypes)iI);
			}

			FAssertMsg(GC.getLeaderHeadInfo(getPersonalityType()).getFavoriteCivic() != NO_CIVIC, "getFavoriteCivic() must be valid");

			paeNewCivics[GC.getCivicInfo((CivicTypes)(GC.getLeaderHeadInfo(getPersonalityType())).getFavoriteCivic()).getCivicOptionType()] = ((CivicTypes)(GC.getLeaderHeadInfo(getPersonalityType()).getFavoriteCivic()));

			GET_PLAYER(ePlayer).revolution(paeNewCivics, true);

			SAFE_DELETE_ARRAY(paeNewCivics);
			break;
		}
		case DIPLOEVENT_NO_REVOLUTION:
		{
			AI_changeMemoryCount(ePlayer, MEMORY_DENIED_CIVIC, 1);
			break;
		}
		case DIPLOEVENT_JOIN_WAR:
		{
			AI_changeMemoryCount(ePlayer, MEMORY_ACCEPTED_JOIN_WAR, 1);

			if (gTeamLogLevel >= 2)
			{
				logBBAI("	Team %d (%S) declares war on team %d due to DIPLOEVENT", getTeam(), getCivilizationDescription(0), ePlayer );
			}
			GET_TEAM(GET_PLAYER(ePlayer).getTeam()).declareWar((TeamTypes)iData1, false, WARPLAN_DOGPILE);

			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam((TeamTypes)iData1))
				{
					GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount(getID(), MEMORY_HIRED_WAR_ALLY, 1);
				}
			}
			break;
		}
		case DIPLOEVENT_NO_JOIN_WAR:
		{
			AI_changeMemoryCount(ePlayer, MEMORY_DENIED_JOIN_WAR, 1);

			if (m_eDemandWarAgainstTeam != NO_TEAM)
			{
				for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(m_eDemandWarAgainstTeam)
					&& GET_PLAYER((PlayerTypes)iI).AI_getMemoryCount(ePlayer, MEMORY_EVENT_GOOD_TO_US) < 3)
					{
						GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount(ePlayer, MEMORY_EVENT_GOOD_TO_US, 1);
					}
				}
				m_eDemandWarAgainstTeam = NO_TEAM;
			}
			break;
		}
		case DIPLOEVENT_STOP_TRADING:
		{
			AI_changeMemoryCount(ePlayer, MEMORY_ACCEPTED_STOP_TRADING, 1);
			GET_PLAYER(ePlayer).stopTradingWithTeam((TeamTypes)iData1);

			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam((TeamTypes)iData1))
				{
					GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount(getID(), MEMORY_HIRED_TRADE_EMBARGO, 1);
				}
			}
			break;
		}
		case DIPLOEVENT_NO_STOP_TRADING:
		{
			AI_changeMemoryCount(ePlayer, MEMORY_DENIED_STOP_TRADING, 1);
			break;
		}
		case DIPLOEVENT_ASK_HELP:
		{
			AI_changeMemoryCount(ePlayer, MEMORY_MADE_DEMAND_RECENT, 1);
			break;
		}
		case DIPLOEVENT_MADE_DEMAND:
		{
			if (AI_getMemoryCount(ePlayer, MEMORY_MADE_DEMAND) < 10)
			{
				AI_changeMemoryCount(ePlayer, MEMORY_MADE_DEMAND, 1);
			}
			AI_changeMemoryCount(ePlayer, MEMORY_MADE_DEMAND_RECENT, 1);
			break;
		}
		case DIPLOEVENT_MADE_DEMAND_VASSAL:
		{
			break;
		}
		case DIPLOEVENT_RESEARCH_TECH:
		{
			pushResearch(((TechTypes)iData1), true);
			break;
		}
		case DIPLOEVENT_TARGET_CITY:
		{
			const CvCity* pCity = GET_PLAYER((PlayerTypes)iData1).getCity(iData2);
			if (pCity)
			{
				pCity->area()->setTargetCity(getID(), pCity);
			}
			break;
		}
		case DIPLOEVENT_MAKE_PEACE_WITH:
		{
			if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar((TeamTypes)iData1))
			{
				if (AI_getMemoryCount(ePlayer, MEMORY_EVENT_GOOD_TO_US) < 3)
				{
					AI_changeMemoryCount(ePlayer, MEMORY_EVENT_GOOD_TO_US, 1);
				}
				GET_TEAM(GET_PLAYER(ePlayer).getTeam()).makePeace((TeamTypes)iData1);
			}
			break;
		}
		case DIPLOEVENT_NO_MAKE_PEACE_WITH:
		{
			if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar((TeamTypes)iData1)

			&& AI_getMemoryCount(ePlayer, MEMORY_REFUSED_HELP) < 3)
			{
				AI_changeMemoryCount(ePlayer, MEMORY_REFUSED_HELP, 1);
			}
			break;
		}
		case DIPLOEVENT_DO_NOT_BOTHER:
		{
			if (!isHumanPlayer())
			{
				m_bDoNotBotherStatus = (PlayerTypes)iData1;
			}
			break;
		}
		case DIPLOEVENT_RESUME_BOTHER:
		{
			m_bDoNotBotherStatus = NO_PLAYER;
			break;
		}
		default:
		{
			FErrorMsg("error");
			break;
		}
	}
}


bool CvPlayer::canTradeWith(PlayerTypes eWhoTo) const
{
	if (::atWar(getTeam(), GET_PLAYER(eWhoTo).getTeam()))
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isTechTrading() && GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isTechTrading())
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isGoldTrading() && GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isGoldTrading())
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isMapTrading() && GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isMapTrading())
	{
		return true;
	}

	if (canTradeNetworkWith(eWhoTo))
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isOpenBordersTrading() && GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isOpenBordersTrading())
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isDefensivePactTrading() || GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isDefensivePactTrading())
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isPermanentAllianceTrading() || GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isPermanentAllianceTrading())
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isVassalStateTrading() || GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isVassalStateTrading())
	{
		return true;
	}

	return false;
}


bool CvPlayer::canTradeItem(PlayerTypes eWhoTo, TradeData item, bool bTestDenial) const
{
	// KOSHLING - do denial check at the end because it's expensive, and the trade
	// is not possible before having to make the denail check that way 95% of the time.
	bool bResult = false;

	switch (item.m_eItemType)
	{
		case TRADE_TECHNOLOGIES:
		{
			FAssertMsg(item.m_iData >= 0, "item.m_iData is expected to be non-negative (invalid Index)");

			if (!GC.getGame().isOption(GAMEOPTION_NO_TECH_TRADING)
			&& GC.getTechInfo((TechTypes)item.m_iData).isTrade()
			&& GET_TEAM(getTeam()).isHasTech((TechTypes)item.m_iData)
			&& !GET_TEAM(getTeam()).isNoTradeTech((TechTypes)item.m_iData)
			&& !GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isHasTech((TechTypes)item.m_iData)
			&& GET_TEAM(getTeam()).isTechTrading()
			&& GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isTechTrading()
			&& GET_PLAYER(eWhoTo).canResearch((TechTypes)item.m_iData))
			{
				bResult = true;
			}
			break;
		}
		case TRADE_RESOURCES:
		{
			FAssertMsg(item.m_iData > -1, "iData is expected to be non-negative");

			if (GC.getDefineINT("CAN_TRADE_RESOURCES") > 0 && canTradeNetworkWith(eWhoTo)
			&& !GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isBonusObsolete((BonusTypes)item.m_iData)
			&& !GET_TEAM(getTeam()).isBonusObsolete((BonusTypes)item.m_iData))
			{
				const bool bCanTradeAll =
				(
					isHumanPlayer()
					||
					getTeam() == GET_PLAYER(eWhoTo).getTeam()
					||
					GET_TEAM(getTeam()).isVassal(GET_PLAYER(eWhoTo).getTeam())
				);
				if (getNumTradeableBonuses((BonusTypes) item.m_iData) > (bCanTradeAll ? 0 : 1))
				{
					bResult = true;
				}
			}
			break;
		}
		case TRADE_CITIES:
		{
			CvCity* pCityTraded = getCity(item.m_iData);

			if (pCityTraded && pCityTraded->getLiberationPlayer(false) == eWhoTo)
			{
				bResult = true;
			}
			else if (GC.getDefineINT("CAN_TRADE_CITIES") > 0
			&& 0 == GC.getGame().getMaxCityElimination()
			&& !GET_TEAM(getTeam()).isAVassal()
			&& !GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isVassal(getTeam()))
			{
				CvCity* pOurCapitalCity = getCapitalCity();
				if (pOurCapitalCity && pOurCapitalCity->getID() != item.m_iData)
				{
					bResult = true;
				}
			}
			break;
		}
		case TRADE_WORKER:
		{
			CvUnit* pUnitTraded = getUnit(item.m_iData);

			if (pUnitTraded && GET_PLAYER(eWhoTo).getCapitalCity()
			&& GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY)
			&& GC.getDefineINT("CAN_TRADE_WORKERS") > 0
			&& GET_TEAM(getTeam()).isHasEmbassy(GET_PLAYER(eWhoTo).getTeam())
			&& GC.getUnitInfo(pUnitTraded->getUnitType()).isWorkerTrade()
			&& pUnitTraded->canMove()
			&& !GET_PLAYER(eWhoTo).isUnitMaxedOut(pUnitTraded->getUnitType(), GET_PLAYER(eWhoTo).getUnitMaking(pUnitTraded->getUnitType())))
			{
				bResult = true;
			}
			break;
		}
		case TRADE_MILITARY_UNIT:
		{
			CvUnit* pUnitTraded = getUnit(item.m_iData);
			CvCity* pTheirCapitalCity = GET_PLAYER(eWhoTo).getCapitalCity();

			if (pUnitTraded && pTheirCapitalCity && pUnitTraded->canMove()
			&& GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY)
			&& GC.getDefineINT("NO_MILITARY_UNIT_TRADING") == 0
			&& GET_TEAM(getTeam()).isHasEmbassy(GET_PLAYER(eWhoTo).getTeam())
			&& GC.getUnitInfo(pUnitTraded->getUnitType()).isMilitaryTrade())
			{
				CvCity* pTradingCity = pUnitTraded->plot()->getPlotCity();

				if (pTradingCity && pTradingCity->getOwner() == getID()
				&& pTradingCity->isConnectedTo(pTheirCapitalCity)
				&& pTradingCity->isRevealed(GET_PLAYER(eWhoTo).getTeam(), true)
				&& !GET_PLAYER(eWhoTo).isUnitMaxedOut(pUnitTraded->getUnitType(), GET_PLAYER(eWhoTo).getUnitMaking(pUnitTraded->getUnitType())))
				{
					bResult = true;
				}
			}
			break;
		}
		case TRADE_GOLD:
		{
			if (GC.getDefineINT("CAN_TRADE_GOLD") > 0
			&& GET_TEAM(getTeam()).isGoldTrading()
			&& GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isGoldTrading()
			&& getGold() >= item.m_iData)
			{
				bResult = true;
			}
			break;
		}
		case TRADE_GOLD_PER_TURN:
		{
			if (GET_TEAM(getTeam()).isGoldTrading()
			&& GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isGoldTrading()
			&& GC.getDefineINT("CAN_TRADE_GOLD_PER_TURN") > 0)
			{
				bResult = true;
			}
			break;
		}
		case TRADE_MAPS:
		{
			if (GC.getDefineINT("CAN_TRADE_MAPS") > 0 && getTeam() != GET_PLAYER(eWhoTo).getTeam()
			&& GET_TEAM(getTeam()).isMapTrading() && GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isMapTrading())
			{
				bResult = true;
			}
			break;
		}
		case TRADE_VASSAL:
		case TRADE_SURRENDER:
		{
			CvTeam& kMasterTeam = GET_TEAM(GET_PLAYER(eWhoTo).getTeam());

			if (kMasterTeam.isVassalStateTrading() && !kMasterTeam.isAVassal()
			&& !GET_TEAM(getTeam()).isAVassal() && getTeam() != GET_PLAYER(eWhoTo).getTeam())
			{
				if (item.m_eItemType == TRADE_SURRENDER)
				{
					if (kMasterTeam.isAtWar(getTeam()) || item.m_iData == 1)
					{
						bResult = true;
					}
				}
				else if (item.m_eItemType == TRADE_VASSAL && !kMasterTeam.isAtWar(getTeam()))
				{
					bResult = true;
				}
			}
			break;
		}
		case TRADE_EMBASSY:
		{
			if (GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY)
			&& GC.getDefineINT("CAN_TRADE_EMBASSIES") > 0
			&& getTeam() != GET_PLAYER(eWhoTo).getTeam()
			&& !::atWar(getTeam(), GET_PLAYER(eWhoTo).getTeam())
			&& !GET_TEAM(getTeam()).isHasEmbassy(GET_PLAYER(eWhoTo).getTeam())
			&& GET_TEAM(getTeam()).isEmbassyTrading()
			&& GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isEmbassyTrading()
			&& getCapitalCity()
			&& GET_PLAYER(eWhoTo).getCapitalCity())
			{
				bResult = true;
			}
			break;
		}
		case TRADE_CONTACT:
		{
			if (GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY)
			&& GC.getDefineINT("CAN_TRADE_CONTACT") > 0
			&& !GET_TEAM((TeamTypes)item.m_iData).isMinorCiv()
			&& !GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isHasMet((TeamTypes)item.m_iData)
			&& GET_TEAM(getTeam()).isHasMet((TeamTypes)item.m_iData))
			{
				bResult = true;
			}
			break;
		}
		case TRADE_CORPORATION:
		{
			if (GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY)
			&& GC.getDefineINT("CAN_TRADE_CORPORATIONS") > 0
			&& hasHeadquarters((CorporationTypes)item.m_iData)
			&& GET_PLAYER(eWhoTo).getBestHQCity((CorporationTypes)item.m_iData)
			&& GET_PLAYER(eWhoTo).isActiveCorporation((CorporationTypes)item.m_iData))
			{
				bResult = true;
			}
			break;
		}
		case TRADE_SECRETARY_GENERAL_VOTE:
		{
			/*
			if (GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY)
			&& !GET_TEAM(getTeam()).isHuman()
			&& GC.getGame().isDiploVote((VoteSourceTypes)item.m_iData)
			&& GC.getGame().isTeamVoteEligible(GET_PLAYER(eWhoTo).getTeam(), (VoteSourceTypes)item.m_iData)
			&& isVotingMember((VoteSourceTypes)item.m_iData) && getPledgedSecretaryGeneralVote() == NO_TEAM)
			{
				bResult = true;
			}
			*/
			break;
		}
		case TRADE_PLEDGE_VOTE:
			/*
			if (GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY))
			{
				VoteTriggeredData* pVoteTriggered = GC.getGame().getVoteTriggered(GC.getGame().getCurrentVoteID());

				if (pVoteTriggered && GC.getGame().getSecretaryGeneral(pVoteTriggered->eVoteSource) != getTeam())
				{
					const CvVoteInfo& kVote = GC.getVoteInfo(pVoteTriggered->kVoteOption.eVote);

					if (!GC.getGame().isTeamVote(pVoteTriggered->kVoteOption.eVote)
					&& isVotingMember(pVoteTriggered->eVoteSource)
					&& GET_PLAYER(eWhoTo).isVotingMember(pVoteTriggered->eVoteSource)
					&& getPledgedVote() == NO_PLAYER_VOTE)
					{
						bResult = true;
					}
				}
			}
			*/
			break;

		case TRADE_PEACE:
		{
			if (GC.getDefineINT("CAN_TRADE_PEACE") > 0
			&& !GET_TEAM(getTeam()).isHuman()
			&& !GET_TEAM(getTeam()).isAVassal()
			&& GET_TEAM(getTeam()).isHasMet((TeamTypes)item.m_iData)
			&& GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isHasMet((TeamTypes)item.m_iData)
			&& atWar(getTeam(), (TeamTypes)item.m_iData))
			{
				bResult = true;
			}
			break;
		}
		case TRADE_WAR:
		{
			if (GC.getDefineINT("CAN_TRADE_WAR") > 0
			&& !GET_TEAM(getTeam()).isHuman()
			&& !GET_TEAM(getTeam()).isAVassal()
			&& !GET_TEAM((TeamTypes)item.m_iData).isAVassal()
			&& GET_TEAM(getTeam()).isHasMet((TeamTypes)item.m_iData)
			&& GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isHasMet((TeamTypes)item.m_iData)
			&& GET_TEAM(getTeam()).canDeclareWar((TeamTypes)item.m_iData))
			{
				bResult = true;
			}
			break;
		}
		case TRADE_EMBARGO:
		{
			if (GC.getDefineINT("CAN_TRADE_EMBARGO") > 0
			&& !GET_TEAM(getTeam()).isHuman()
			&& GET_TEAM(getTeam()).isHasMet((TeamTypes)item.m_iData)
			&& GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isHasMet((TeamTypes)item.m_iData)
			&& canStopTradingWithTeam((TeamTypes)item.m_iData))
			{
				bResult = true;
			}
			break;
		}
		case TRADE_CIVIC:
		{
			if (GC.getDefineINT("CAN_TRADE_CIVICS") > 0 && canRevolution(NULL)
			&& (!GET_TEAM(getTeam()).isHuman() || getTeam() == GET_PLAYER(eWhoTo).getTeam())
			&& GET_PLAYER(eWhoTo).isCivic((CivicTypes)item.m_iData)
			&& canDoCivics((CivicTypes)item.m_iData)
			&& !isCivic((CivicTypes)item.m_iData))
			{
				bResult = true;
			}
			break;
		}
		case TRADE_RELIGION:
		{
			if (GC.getDefineINT("CAN_TRADE_RELIGIONS") > 0
			&& (!GET_TEAM(getTeam()).isHuman() || getTeam() == GET_PLAYER(eWhoTo).getTeam())
			&& GET_PLAYER(eWhoTo).getStateReligion() == (ReligionTypes)item.m_iData
			&& canConvert((ReligionTypes)item.m_iData))
			{
				bResult = true;
			}
			break;
		}
		case TRADE_OPEN_BORDERS:
		{
			if (GC.getDefineINT("CAN_TRADE_OPEN_BORDERS") > 0
			&& getTeam() != GET_PLAYER(eWhoTo).getTeam()
			&& !atWar(getTeam(), GET_PLAYER(eWhoTo).getTeam())
			&& !GET_TEAM(getTeam()).isOpenBorders(GET_PLAYER(eWhoTo).getTeam())
			&& GET_TEAM(getTeam()).isOpenBordersTrading()
			&& GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isOpenBordersTrading()
			&& GET_TEAM(getTeam()).canSignOpenBorders(GET_PLAYER(eWhoTo).getTeam()))
			{
				bResult = true;
			}
			break;
		}
		case TRADE_RITE_OF_PASSAGE:
		{
			if (GC.getDefineINT("CAN_TRADE_LIMITED_BORDERS") > 0
			&& GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY)
			&& getTeam() != GET_PLAYER(eWhoTo).getTeam()
			&& !atWar(getTeam(), GET_PLAYER(eWhoTo).getTeam())
			&& !GET_TEAM(getTeam()).isOpenBorders(GET_PLAYER(eWhoTo).getTeam())
			&& !GET_TEAM(getTeam()).isLimitedBorders(GET_PLAYER(eWhoTo).getTeam())
			&& GET_TEAM(getTeam()).isOpenBordersTrading()
			&& GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isOpenBordersTrading())
			{
				bResult = true;
			}
			break;
		}
		case TRADE_FREE_TRADE_ZONE:
		{
			if (GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY)
			&& getTeam() != GET_PLAYER(eWhoTo).getTeam()
			&& !atWar(getTeam(), GET_PLAYER(eWhoTo).getTeam())
			&& GET_TEAM(getTeam()).canSignOpenBorders(GET_PLAYER(eWhoTo).getTeam())
			&& !GET_TEAM(getTeam()).isFreeTradeAgreement(GET_PLAYER(eWhoTo).getTeam()))
			{
				bResult = true;
			}
			break;
		}
		case TRADE_DEFENSIVE_PACT:
		{
			if (GC.getDefineINT("CAN_TRADE_DEFENSIVE_PACT") > 0
			&& getTeam() != GET_PLAYER(eWhoTo).getTeam()
			&& !GET_TEAM(getTeam()).isDefensivePact(GET_PLAYER(eWhoTo).getTeam())
			&& GET_TEAM(getTeam()).canSignDefensivePact(GET_PLAYER(eWhoTo).getTeam()))
			{
				bResult = true;
			}
			break;
		}
		case TRADE_PERMANENT_ALLIANCE:
		{
			if (getTeam() != GET_PLAYER(eWhoTo).getTeam()
			&& !GET_TEAM(getTeam()).isAVassal()
			&& !GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isAVassal()
			&& !atWar(getTeam(), GET_PLAYER(eWhoTo).getTeam())
			&& GET_TEAM(getTeam()).isPermanentAllianceTrading()
			&& GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isPermanentAllianceTrading()
			&& GET_TEAM(getTeam()).getNumMembers() == 1
			&& GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).getNumMembers() == 1)
			{
				bResult = true;
			}
			break;
		}
		case TRADE_PEACE_TREATY:
		{
			bResult = true;
			break;
		}
	}

	if (bTestDenial && bResult)
	{
		bResult &= (getTradeDenial(eWhoTo, item) == NO_DENIAL);
	}

	return bResult;
}


DenialTypes CvPlayer::getTradeDenial(PlayerTypes eWhoTo, TradeData item) const
{
	switch (item.m_eItemType)
	{
		case TRADE_TECHNOLOGIES:
			return GET_TEAM(getTeam()).AI_techTrade(((TechTypes)(item.m_iData)), GET_PLAYER(eWhoTo).getTeam());

		case TRADE_RESOURCES:
			return AI_bonusTrade(((BonusTypes)(item.m_iData)), eWhoTo);

		case TRADE_CITIES:
		{
			CvCity* pCity = getCity(item.m_iData);
			if (pCity)
			{
				return AI_cityTrade(pCity, eWhoTo);
			}
			break;
		}
		case TRADE_MAPS:
			return GET_TEAM(getTeam()).AI_mapTrade(GET_PLAYER(eWhoTo).getTeam());

		case TRADE_SURRENDER:
			return GET_TEAM(getTeam()).AI_surrenderTrade(GET_PLAYER(eWhoTo).getTeam(), 140);

		case TRADE_VASSAL:
			return GET_TEAM(getTeam()).AI_vassalTrade(GET_PLAYER(eWhoTo).getTeam());

		case TRADE_PEACE:
			return GET_TEAM(getTeam()).AI_makePeaceTrade(((TeamTypes)(item.m_iData)), GET_PLAYER(eWhoTo).getTeam());

		case TRADE_WAR:
			return GET_TEAM(getTeam()).AI_declareWarTrade(((TeamTypes)(item.m_iData)), GET_PLAYER(eWhoTo).getTeam());

		case TRADE_EMBARGO:
			return AI_stopTradingTrade(((TeamTypes)(item.m_iData)), eWhoTo);

		case TRADE_CIVIC:
			return AI_civicTrade(((CivicTypes)(item.m_iData)), eWhoTo);

		case TRADE_RELIGION:
			return AI_religionTrade(((ReligionTypes)(item.m_iData)), eWhoTo);

		case TRADE_OPEN_BORDERS:
			return GET_TEAM(getTeam()).AI_openBordersTrade(GET_PLAYER(eWhoTo).getTeam());

		case TRADE_DEFENSIVE_PACT:
			return GET_TEAM(getTeam()).AI_defensivePactTrade(GET_PLAYER(eWhoTo).getTeam());

		case TRADE_PERMANENT_ALLIANCE:
			return GET_TEAM(getTeam()).AI_permanentAllianceTrade(GET_PLAYER(eWhoTo).getTeam());

		case TRADE_RITE_OF_PASSAGE:
			return GET_TEAM(getTeam()).AI_LimitedBordersTrade(GET_PLAYER(eWhoTo).getTeam());

		case TRADE_FREE_TRADE_ZONE:
			return GET_TEAM(getTeam()).AI_FreeTradeAgreement(GET_PLAYER(eWhoTo).getTeam());

		case TRADE_WORKER:
		{
			const CvUnit* pUnit = getUnit(item.m_iData);
			if (pUnit)
			{
				return AI_workerTrade(pUnit, eWhoTo);
			}
			break;
		}
		case TRADE_MILITARY_UNIT:
		{
			const CvUnit* pUnit = getUnit(item.m_iData);
			if (pUnit)
			{
				return AI_militaryUnitTrade(pUnit, eWhoTo);
			}
			break;
		}
		case TRADE_EMBASSY:
			return GET_TEAM(getTeam()).AI_embassyTrade(GET_PLAYER(eWhoTo).getTeam());

		case TRADE_CONTACT:
			return GET_TEAM(getTeam()).AI_contactTrade(((TeamTypes)item.m_iData), GET_PLAYER(eWhoTo).getTeam());

		case TRADE_CORPORATION:
			return AI_corporationTrade(((CorporationTypes)(item.m_iData)), eWhoTo);

		case TRADE_SECRETARY_GENERAL_VOTE:
			return AI_secretaryGeneralTrade(((VoteSourceTypes)(item.m_iData)), eWhoTo);

		case TRADE_GOLD:
		case TRADE_GOLD_PER_TURN:
		case TRADE_PEACE_TREATY:
		case TRADE_PLEDGE_VOTE:
			break;
	}
	return NO_DENIAL;
}


bool CvPlayer::canTradeNetworkWith(PlayerTypes ePlayer) const
{
	const CvCity* pOurCapitalCity = getCapitalCity();

	if (pOurCapitalCity)
	{
		if (pOurCapitalCity->isConnectedToCapital(ePlayer))
		{
			return true;
		}
	}

	return false;
}


int CvPlayer::getNumAvailableBonuses(BonusTypes eBonus) const
{
	const CvPlotGroup* pPlotGroup = ((getCapitalCity()) ? getCapitalCity()->plot()->getOwnerPlotGroup() : NULL);

	if (pPlotGroup)
	{
		return std::max(0, pPlotGroup->getNumBonuses(eBonus));
	}
	return 0;
}


int CvPlayer::getNumTradeableBonuses(BonusTypes eBonus) const
{
	return (getNumAvailableBonuses(eBonus) - getBonusImport(eBonus));
}

bool CvPlayer::hasBonus(BonusTypes eBonus) const
{
	return algo::any_of(cities(), CvCity::fn::hasBonus(eBonus));
}

int CvPlayer::getNumTradeBonusImports(PlayerTypes ePlayer) const
{
	PROFILE_EXTRA_FUNC();
	CLLNode<TradeData>* pNode;

	FAssert(ePlayer != getID());

	int iCount = 0;

	foreach_(const CvDeal& kLoopDeal, GC.getGame().deals())
	{
		if (kLoopDeal.getFirstPlayer() == getID() && kLoopDeal.getSecondPlayer() == ePlayer)
		{
			for (pNode = kLoopDeal.headSecondTradesNode(); pNode; pNode = kLoopDeal.nextSecondTradesNode(pNode))
			{
				if (pNode->m_data.m_eItemType == TRADE_RESOURCES)
				{
					iCount++;
				}
			}
		}

		if (kLoopDeal.getFirstPlayer() == ePlayer && kLoopDeal.getSecondPlayer() == getID())
		{
			for (pNode = kLoopDeal.headFirstTradesNode(); pNode; pNode = kLoopDeal.nextFirstTradesNode(pNode))
			{
				if (pNode->m_data.m_eItemType == TRADE_RESOURCES)
				{
					iCount++;
				}
			}
		}
	}

	return iCount;
}


bool CvPlayer::isTradingWithTeam(TeamTypes eTeam, bool bIncludeCancelable) const
{
	PROFILE_EXTRA_FUNC();
	if (eTeam == getTeam())
	{
		return false;
	}

	foreach_(CvDeal& kLoopDeal, GC.getGame().deals())
	{
		if (bIncludeCancelable || kLoopDeal.isCancelable(getID()))
		{
			if (!kLoopDeal.isPeaceDeal())
			{
				if (kLoopDeal.getFirstPlayer() == getID() && GET_PLAYER(kLoopDeal.getSecondPlayer()).getTeam() == eTeam)
				{
					if (kLoopDeal.getLengthFirstTrades() > 0)
					{
						return true;
					}
				}

				if (kLoopDeal.getSecondPlayer() == getID() && GET_PLAYER(kLoopDeal.getFirstPlayer()).getTeam() == eTeam)
				{
					if (kLoopDeal.getLengthSecondTrades() > 0)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}


bool CvPlayer::canStopTradingWithTeam(TeamTypes eTeam, bool bContinueNotTrading) const
{
	if (eTeam == getTeam())
	{
		return false;
	}

	if (GET_TEAM(getTeam()).isVassal(eTeam))
	{
		return false;
	}

	if (!isTradingWithTeam(eTeam, false))
	{
		if (bContinueNotTrading && !isTradingWithTeam(eTeam, true))
		{
			return true;
		}

		return false;
	}

	return true;
}


void CvPlayer::stopTradingWithTeam(TeamTypes eTeam)
{
	PROFILE_EXTRA_FUNC();
	FAssert(eTeam != getTeam());

	foreach_(CvDeal& kLoopDeal, GC.getGame().deals())
	{
		if (kLoopDeal.isCancelable(getID()) && !kLoopDeal.isPeaceDeal())
		{
			if (((kLoopDeal.getFirstPlayer() == getID()) && (GET_PLAYER(kLoopDeal.getSecondPlayer()).getTeam() == eTeam)) ||
				  ((kLoopDeal.getSecondPlayer() == getID()) && (GET_PLAYER(kLoopDeal.getFirstPlayer()).getTeam() == eTeam)))
			{
				kLoopDeal.kill();
			}
		}
	}

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(eTeam))
		{
			GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount(getID(), MEMORY_STOPPED_TRADING, 1);
			GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount(getID(), MEMORY_STOPPED_TRADING_RECENT, 1);
		}
	}
}


void CvPlayer::killAllDeals()
{
	algo::for_each(GC.getGame().deals()
		| filtered(bind(CvDeal::getFirstPlayer, _1) == getID() || bind(CvDeal::getSecondPlayer, _1) == getID())
		, bind(CvDeal::kill, _1, true)
	);
}


void CvPlayer::findNewCapital()
{
	PROFILE_EXTRA_FUNC();
	const CvCity* pOldCapital = getCapitalCity();

	int iBestValue = 0;
	CvCity* pBestCity = NULL;

	foreach_(CvCity* pLoopCity, cities())
	{
		if (pLoopCity != pOldCapital)
		{
			int iValue = pLoopCity->getPopulation() * 4;

			iValue += pLoopCity->getYieldRate(YIELD_FOOD);
			iValue += pLoopCity->getYieldRate(YIELD_PRODUCTION) * 3;
			iValue += pLoopCity->getYieldRate(YIELD_COMMERCE) * 2;
			iValue += pLoopCity->getCultureLevel();
			iValue += pLoopCity->getReligionCount();
			iValue += pLoopCity->getCorporationCount();
			iValue += pLoopCity->getNumGreatPeople() * 2;

			iValue *= 100 + pLoopCity->calculateCulturePercent(getID());
			iValue /= 100;

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestCity = pLoopCity;
			}
		}
	}
	if (pBestCity)
	{
		for (int iI = 0; iI < GC.getCivilizationInfo(getCivilizationType()).getNumCivilizationBuildings(); iI++)
		{
			pBestCity->changeHasBuilding((BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuilding(iI), true);
		}
	}
}


int CvPlayer::getNumGovernmentCenters() const
{
	return algo::count_if(cities(), CvCity::fn::isGovernmentCenter());
}

void CvPlayer::raze(CvCity* pCity)
{
	PROFILE_EXTRA_FUNC();
	wchar_t szBuffer[1024];
	{
		const PlayerTypes eHighestCulturePlayer = pCity->findHighestCulture();

		if (eHighestCulturePlayer != NO_PLAYER && GET_PLAYER(eHighestCulturePlayer).getTeam() != getTeam())
		{
			GET_PLAYER(eHighestCulturePlayer).AI_changeMemoryCount(getID(), MEMORY_RAZED_CITY, 1);
		}
	}

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if (pCity->isHolyCity((ReligionTypes)iI))
		{
			for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
			{
				if (GET_PLAYER((PlayerTypes)iJ).isAlive() && iJ != getID() && GET_PLAYER((PlayerTypes)iJ).getStateReligion() == (ReligionTypes)iI)
				{
					GET_PLAYER((PlayerTypes)iJ).AI_changeMemoryCount(getID(), MEMORY_RAZED_HOLY_CITY, 1);
				}
			}
		}
	}
	swprintf(szBuffer, gDLL->getText("TXT_KEY_MISC_DESTROYED_CITY", pCity->getNameKey()).GetCString());
	AddDLLMessage(
		getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITYRAZE", MESSAGE_TYPE_MAJOR_EVENT,
		ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), GC.getCOLOR_GREEN(), pCity->getX(), pCity->getY(), true, true
	);

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && iI != getID() && pCity->isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
		{
			swprintf(szBuffer, gDLL->getText("TXT_KEY_MISC_CITY_HAS_BEEN_RAZED_BY", pCity->getNameKey(), getCivilizationDescriptionKey()).GetCString());
			AddDLLMessage(
				(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITYRAZED", MESSAGE_TYPE_MAJOR_EVENT,
				ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), GC.getCOLOR_RED(), pCity->getX(), pCity->getY(), true, true
			);
		}
	}
	swprintf(szBuffer, gDLL->getText("TXT_KEY_MISC_CITY_RAZED_BY", pCity->getNameKey(), getCivilizationDescriptionKey()).GetCString());
	GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szBuffer, pCity->getX(), pCity->getY(), (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));

	// Report this event
	CvEventReporter::getInstance().cityRazed(pCity, getID());

	disband(pCity);
}


void CvPlayer::disband(CvCity* city)
{
	CvPlayer& owner = GET_PLAYER(city->getOwner());

	if (owner.getNumCities() == 1)
	{
		owner.setFoundedFirstCity(false);
	}
	GC.getGame().addDestroyedCityName(city->getName());
	city->kill(true);
}


bool CvPlayer::canReceiveGoody(const CvPlot* pPlot, GoodyTypes eGoody, const CvUnit* pUnit) const
{
	PROFILE_EXTRA_FUNC();
	const EraTypes eEra = (EraTypes)GC.getGoodyInfo(eGoody).getEraType();

	if (eEra != NO_ERA && eEra != getCurrentEra())
	{
		return false;
	}
	const EraTypes eNotEra = (EraTypes)GC.getGoodyInfo(eGoody).getNotEraType();

	if (eNotEra != NO_ERA && eNotEra == getCurrentEra())
	{
		return false;
	}
	if (GC.getGoodyInfo(eGoody).getResearch() != 0 && getCurrentResearch() == NO_TECH)
	{
		return false;
	}
	if (GC.getGoodyInfo(eGoody).getExperience() > 0 && (pUnit == NULL || !pUnit->canAcquirePromotionAny()))
	{
		return false;
	}
	if (GC.getGoodyInfo(eGoody).getDamagePrereq() > 0
	&& (pUnit == NULL || pUnit->getDamage() < pUnit->getMaxHP() * GC.getGoodyInfo(eGoody).getDamagePrereq() / 100))
	{
		return false;
	}

	bool bFound = false;
	if (GC.getGoodyInfo(eGoody).isTech())
	{
		foreach_(const TechTypes eTechX, GET_TEAM(getTeam()).getAdjacentResearch())
		{
			if (GC.getTechInfo(eTechX).isGoodyTech() && canResearch(eTechX))
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
	if (GC.getGoodyInfo(eGoody).isBad() && (pUnit == NULL || pUnit->isNoBadGoodies()))
	{
		return false;
	}

	if (GC.getGoodyInfo(eGoody).getGoodyUnit() != NO_UNIT)
	{
		if (GC.getGoodyInfo(eGoody).isNaval() != pPlot->isWater())
		{
			return false;
		}

		if (GC.getGame().isOption(GAMEOPTION_CHALLENGE_ONE_CITY)
		&& GC.getUnitInfo((UnitTypes)GC.getGoodyInfo(eGoody).getGoodyUnit()).isFound())
		{
			return false;
		}
	}

	if (GC.getGoodyInfo(eGoody).getBarbarianUnit() != NO_UNIT)
	{
		if (GC.getGame().isOption(GAMEOPTION_BARBARIAN_NONE) || pUnit == NULL)
		{
			return false;
		}

		if (GC.getGoodyInfo(eGoody).isNaval() != pPlot->isWater())
		{
			return false;
		}

		if (getNumCities() == 0)
		{
			return false;
		}
	}
	if (!isMapCategory(*pPlot, GC.getGoodyInfo(eGoody)))
	{
		return false;
	}
	return true;
}


void CvPlayer::receiveGoody(CvPlot* pPlot, GoodyTypes eGoody, CvUnit* pUnit)
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(canReceiveGoody(pPlot, eGoody, pUnit), "Instance is expected to be able to recieve goody");

	CvWString szBuffer = GC.getGoodyInfo(eGoody).getDescription();

	int iGold = GC.getGoodyInfo(eGoody).getGold();
	if (iGold != 0)
	{
		iGold += GC.getGame().getSorenRandNum(GC.getGoodyInfo(eGoody).getGoldRand1(), "Goody Gold 1") + GC.getGame().getSorenRandNum(GC.getGoodyInfo(eGoody).getGoldRand2(), "Goody Gold 2");

		iGold *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
		iGold /= 100;

		changeGold(iGold);

		szBuffer += gDLL->getText("TXT_KEY_MISC_RECEIVED_GOLD", iGold);
	}

	int iResearch = GC.getGoodyInfo(eGoody).getResearch();
	if (iResearch != 0)
	{
		int iTotalResearch = 0;
		const TechTypes eTech = getCurrentResearch();

		if (eTech != NO_TECH)
		{
			iResearch += GC.getGame().getSorenRandNum(GC.getGoodyInfo(eGoody).getGoldRand1(), "Goody Gold 1") + GC.getGame().getSorenRandNum(GC.getGoodyInfo(eGoody).getGoldRand2(), "Goody Gold 2");

			iTotalResearch = GET_TEAM(getTeam()).getResearchCost(eTech) * iResearch / 100;
			GET_TEAM(getTeam()).changeResearchProgress(eTech, iTotalResearch, getID());

			szBuffer += gDLL->getText("TXT_KEY_MISC_RECEIVED_RESEARCH", iTotalResearch, GC.getTechInfo(eTech).getTextKeyWide());
		}
	}

	if (!szBuffer.empty() && getID() == GC.getGame().getActivePlayer())
	{
		// Goody Hut Log
		// keep messages in event log forever

		AddDLLMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getGoodyInfo(eGoody).getSound(), MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getImprovementArtInfo("ART_DEF_IMPROVEMENT_GOODY_HUT")->getButton(), GC.getCOLOR_WHITE(), pPlot->getX(), pPlot->getY());
	}

	const int iRange = GC.getGoodyInfo(eGoody).getMapRange();

	if (iRange > 0)
	{
		CvPlot* pBestPlot = NULL;
		const int iOffset = GC.getGoodyInfo(eGoody).getMapOffset();

		if (iOffset > 0)
		{
			int iBestValue = 0;
			pBestPlot = NULL;

			foreach_(CvPlot* pLoopPlot, pPlot->rect(iOffset, iOffset)
			| filtered(!CvPlot::fn::isRevealed(getTeam(), false)))
			{
				int iValue = (1 + GC.getGame().getSorenRandNum(10000, "Goody Map"));

				iValue *= plotDistance(pPlot->getX(), pPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY());

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = pLoopPlot;
				}
			}
		}

		if (pBestPlot == NULL)
		{
			pBestPlot = pPlot;
		}

		foreach_(CvPlot* pLoopPlot, pBestPlot->rect(iRange, iRange))
		{
			if (plotDistance(pBestPlot->getX(), pBestPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY()) <= iRange)
			{
				if (GC.getGame().getSorenRandNum(100, "Goody Map") < GC.getGoodyInfo(eGoody).getMapProb())
				{
					pLoopPlot->setRevealed(getTeam(), true, false, NO_TEAM, true);
#ifdef ENABLE_FOGWAR_DECAY
					pLoopPlot->InitFogDecay(false);
#endif
				}
			}
		}
	}

	if (pUnit)
	{
		pUnit->changeExperience(GC.getGoodyInfo(eGoody).getExperience());
	}

	if (pUnit)
	{
		pUnit->changeDamage(-(GC.getGoodyInfo(eGoody).getHealing()));
	}

	if (GC.getGoodyInfo(eGoody).isTech())
	{
		int iBestValue = 0;
		TechTypes eBestTech = NO_TECH;

		foreach_(const TechTypes eTechX, GET_TEAM(getTeam()).getAdjacentResearch())
		{
			if (GC.getTechInfo(eTechX).isGoodyTech() && canResearch(eTechX))
			{
				const int iValue = (1 + GC.getGame().getSorenRandNum(10000, "Goody Tech"));

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestTech = eTechX;
				}
			}
		}
		FAssertMsg(eBestTech != NO_TECH, "BestTech is not assigned a valid value");

		GET_TEAM(getTeam()).setHasTech(eBestTech, true, getID(), true, true);

		if (GC.getGame().isOption(GAMEOPTION_NO_TECH_BROKERING))
		{
			GET_TEAM(getTeam()).setNoTradeTech(eBestTech, true);
		}
	}

	if (GC.getGoodyInfo(eGoody).getGoodyUnit() != NO_UNIT)
	{
		initUnit((UnitTypes)GC.getGoodyInfo(eGoody).getGoodyUnit(), pPlot->getX(), pPlot->getY(), NO_UNITAI, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
	}

	if (GC.getGoodyInfo(eGoody).getBarbarianUnit() != NO_UNIT)
	{
		const UnitTypes eUnit = (UnitTypes)GC.getGoodyInfo(eGoody).getBarbarianUnit();
		int iBarbCount = 0;

		for (int iPass = 0; iPass < 4; iPass++)
		{
			if (iBarbCount < GC.getGoodyInfo(eGoody).getMinBarbarians())
			{
				foreach_(const CvPlot* pLoopPlot, pPlot->adjacent())
				{
					if (pLoopPlot->getArea() == pPlot->getArea()
					&& !pLoopPlot->isImpassable(getTeam()) && pLoopPlot->getNumUnits() == 0
					&& (iPass > 0 || GC.getGame().getSorenRandNum(100, "Goody Barbs") < GC.getGoodyInfo(eGoody).getBarbarianUnitProb()))
					{
						GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pLoopPlot->getX(), pLoopPlot->getY(),
							(pLoopPlot->isWater() ? UNITAI_ATTACK_SEA : UNITAI_ATTACK), NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
						iBarbCount++;

						if (iPass > 1 && iBarbCount == GC.getGoodyInfo(eGoody).getMinBarbarians())
						{
							break;
						}
					}
				}
			}
			else
			{
				break;
			}
		}
	}
}


void CvPlayer::doGoody(CvPlot* pPlot, CvUnit* pUnit)
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(pPlot->isGoody(), "pPlot->isGoody is expected to be true");
	FAssertMsg(!isNPC(), "NPC's shouldn't try to pop goodies");

	pPlot->removeGoody();
	std::vector<GoodyTypes> aGoodies;

	for (int iI = 0; iI < GC.getHandicapInfo(getHandicapType()).getNumGoodies(); iI++)
	{
		if (GC.getHandicapInfo(getHandicapType()).getNumGoodies() > 0)
		{
			const GoodyTypes eGoody = (GoodyTypes)GC.getHandicapInfo(getHandicapType()).getGoodies(iI);
			if (canReceiveGoody(pPlot, eGoody, pUnit))
			{
				aGoodies.push_back(eGoody);
			}
		}
	}
	{
		const GoodyTypes eGoody = aGoodies[GC.getGame().getSorenRandNum(aGoodies.size(), "Select Goody Type")];

		receiveGoody(pPlot, eGoody, pUnit);

		FASSERT_BOUNDS(0, GC.getNumGoodyInfos(), eGoody);

		// Python Event
		CvEventReporter::getInstance().goodyReceived(getID(), pPlot, pUnit, eGoody);
	}

	if (hasExtraGoody())
	{
		const GoodyTypes eGoody = aGoodies[GC.getGame().getSorenRandNum(aGoodies.size(), "Select Goody Type 2")];
		receiveGoody(pPlot, eGoody, pUnit);

		// Python Event
		CvEventReporter::getInstance().goodyReceived(getID(), pPlot, pUnit, eGoody);
	}
}


bool CvPlayer::canFound(int iX, int iY, bool bTestVisible) const
{
	if (GC.getGame().isFinalInitialized() && GC.getGame().isOption(GAMEOPTION_CHALLENGE_ONE_CITY) && !isNPC() && getNumCities() > 0)
	{
		return false;
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);

	if (pPlot->isImpassable(getTeam()))
	{
		return false;
	}

	if (!isNPC() && !bTestVisible && getCityLimit() > 0 && getCityOverLimitUnhappy() == 0 && getNumCities() >= getCityLimit())
	{
		return false;
	}

	if (pPlot->isPeak() && !GET_TEAM(getTeam()).isCanFoundOnPeaks())
	{
		return false;
	}

	if (pPlot->getClaimingOwner() != NO_PLAYER && pPlot->getClaimingOwner() != getID())
	{
		return false;
	}

	if (pPlot->getFeatureType() != NO_FEATURE && GC.getFeatureInfo(pPlot->getFeatureType()).isNoCity())
	{
		return false;
	}

	if (pPlot->isOwned() && (pPlot->getOwner() != getID()))
	{
		return false;
	}

	bool bValid = false;

	if (!bValid && GC.getTerrainInfo(pPlot->getTerrainType()).isFound())
	{
		bValid = true;
	}

	if (!bValid && GC.getTerrainInfo(pPlot->getTerrainType()).isFoundCoast() && pPlot->isCoastalLand())
	{
		bValid = true;
	}

	if (!bValid && GC.getTerrainInfo(pPlot->getTerrainType()).isFoundFreshWater() && pPlot->isFreshWater())
	{
		bValid = true;
	}

	if (!bValid)
	{
		return false;
	}

	if (!bTestVisible)
	{
		const int iRange = GC.getGame().getModderGameOption(MODDERGAMEOPTION_MIN_CITY_DISTANCE);

		if (algo::any_of(pPlot->rect(iRange, iRange), CvPlot::fn::isCity(false, NO_TEAM) && CvPlot::fn::area() == pPlot->area()))
		{
			return false;
		}
	}
	return true;
}


void CvPlayer::found(int iX, int iY, CvUnit *pUnit)
{
	PROFILE_EXTRA_FUNC();
	//	This is checked by the caller on the main AI call path, but it is also usable (and used) form a few other paths, so best to leave it in
	if (!canFound(iX, iY))
	{
		return;
	}

	CvCity* pCity = initCity(iX, iY, true, true);
	FAssertMsg(pCity, "City is not assigned a valid value");

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (GC.getBuildingInfo((BuildingTypes)iI).isNewCityFree(pCity->getGameObject())
		&& pCity->canConstruct((BuildingTypes)iI, false, false, false, false, true))
		{
			pCity->changeHasBuilding((BuildingTypes)iI, true);
		}
	}

	if (isNPC())
	{
		int iDummyValue;
		UnitTypes eDefenderUnit = pCity->AI_bestUnitAI(UNITAI_CITY_DEFENSE, iDummyValue);

		if (eDefenderUnit == NO_UNIT)
		{
			eDefenderUnit = pCity->AI_bestUnitAI(UNITAI_ATTACK, iDummyValue);
		}

		if (eDefenderUnit == NO_UNIT)
		{
			if (getID() == NEANDERTHAL_PLAYER)
			{
				eDefenderUnit = (UnitTypes)GC.getInfoTypeForString("UNIT_NEANDERTHAL");
			}
			else eDefenderUnit = (UnitTypes)GC.getInfoTypeForString("UNIT_STONE_THROWER");
		}

		if (eDefenderUnit != NO_UNIT)
		{
			for (int iI = 0; iI < GC.getHandicapInfo(GC.getGame().getHandicapType()).getBarbarianInitialDefenders(); iI++)
			{
				initUnit(eDefenderUnit, iX, iY, UNITAI_CITY_DEFENSE, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
				if (gPlayerLogLevel >= 1)
				{
					logBBAI("	 Player %d (%S) initial defender (%S) initiated for city %S at %d, %d", getID(), getCivilizationDescription(0), GC.getUnitInfo(eDefenderUnit).getDescription(), pCity->getName(0).GetCString(), iX, iY);
				}
			}
		}
		else FErrorMsg(CvString::format("Player %d (%S) no initial defender availible for city %S at %d, %d", getID(), getCivilizationDescription(0), pCity->getName(0).GetCString(), iX, iY).c_str());
	}

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		const BuildingTypes eBuildingX = static_cast<BuildingTypes>(iI);

		if (GC.getBuildingInfo(eBuildingX).getFreeStartEra() != NO_ERA
		&& GC.getGame().getStartEra() >= GC.getBuildingInfo(eBuildingX).getFreeStartEra()
		&& pCity->canConstruct(eBuildingX))
		{
			pCity->changeHasBuilding(eBuildingX, true);
		}
	}

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		pCity->setFreeSpecialistCount(((SpecialistTypes)iI), 0);
	}

	{
		const int iCulture = getNationalCityStartCulture();
		if (iCulture > 0)
		{
			pCity->changeCulture(getID(), iCulture, true, true);
		}
	}
	{
		const int iBonusPop = getNationalCityStartBonusPopulation();
		if (iBonusPop > 0)
		{
			pCity->changePopulation(iBonusPop);
		}
	}

	if (hasCitiesStartwithStateReligion())
	{
		for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			if (getStateReligion() == ((ReligionTypes)iI))
			{
				pCity->setHasReligion(((ReligionTypes)iI), true, true, true);
			}
		}
	}

	pCity->doAutobuild();

	if (!isHumanPlayer() || getAdvancedStartPoints() > -1)
	{
		pCity->doFoundMessage();
	}

	CvEventReporter::getInstance().cityBuilt(pCity, pUnit);

	if (gPlayerLogLevel >= 1)
	{
		logBBAI("  Player %d (%S) founds new city %S at %d, %d", getID(), getCivilizationDescription(0), pCity->getName(0).GetCString(), iX, iY );
	}
}


bool CvPlayer::canTrain(UnitTypes eUnit, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bPropertySpawn) const
{
	PROFILE_FUNC();

	if (eUnit == NO_UNIT) return false;

	if (isNPC() && isWorldUnit(eUnit))
	{
		return false;
	}
	if (!GC.getGame().canEverTrain(eUnit))
	{
		return false;
	}
	const CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);

	if (!bIgnoreCost && kUnit.getProductionCost() == -1)
	{
		return false;
	}

	if (!GET_TEAM(getTeam()).isHasTech((TechTypes)kUnit.getPrereqAndTech()))
	{
		return false;
	}

	if (kUnit.getMaxStartEra() != NO_ERA && GC.getGame().getStartEra() > kUnit.getMaxStartEra())
	{
		return false;
	}

	if (kUnit.getObsoleteTech() != NO_TECH && GET_TEAM(getTeam()).isHasTech((TechTypes)kUnit.getObsoleteTech()))
	{
		return false;
	}

	foreach_(const TechTypes ePrereqTech, kUnit.getPrereqAndTechs())
	{
		if (!GET_TEAM(getTeam()).isHasTech(ePrereqTech))
		{
			return false;
		}
	}

	if (kUnit.getStateReligion() != NO_RELIGION && getStateReligion() != kUnit.getStateReligion())
	{
		return false;
	}

	if (!bPropertySpawn)
	{
		if (GC.getGame().isOption(GAMEOPTION_CHALLENGE_ONE_CITY) && kUnit.isFound())
		{
			return false;
		}

		if (kUnit.isPrereqOrCivics((int)NO_CIVIC))
		{
			bool bValid = false;
			for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
			{
				if (kUnit.isPrereqOrCivics(iI) && isCivic(CivicTypes(iI)))
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

		if (GC.getGame().isUnitMaxedOut(eUnit) || isUnitMaxedOut(eUnit))
		{
			return false;
		}
	}

	if (!bTestVisible)
	{
		foreach_(const HeritageTypes eTypeX, kUnit.getPrereqAndHeritage())
		{
			if (!hasHeritage(eTypeX))
			{
				return false;
			}
		}
		{
			bool bValid = true;
			foreach_(const HeritageTypes eTypeX, kUnit.getPrereqOrHeritage())
			{
				bValid = false;

				if (hasHeritage(eTypeX))
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

		if (!bPropertySpawn)
		{
			if (GC.getGame().isUnitMaxedOut(eUnit, GET_TEAM(getTeam()).getUnitMaking(eUnit) - bContinue)
			|| isUnitMaxedOut(eUnit, getUnitMaking(eUnit) - bContinue))
			{
				return false;
			}
		}

		if (!isNukesValid() && kUnit.getNukeRange() != -1)
		{
			return false;
		}

		if (kUnit.getSpecialUnitType() != NO_SPECIALUNIT && !GC.getGame().isSpecialUnitValid((SpecialUnitTypes)(kUnit.getSpecialUnitType())))
		{
			return false;
		}

		if (kUnit.isStateReligion() && getStateReligion() == NO_RELIGION)
		{
			return false;
		}

		if (kUnit.isInquisitor() && !isInquisitionConditions())
		{
			return false;
		}
	}
	return true;
}


bool CvPlayer::canConstruct(BuildingTypes eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost, TechTypes eIgnoreTechReq, int* probabilityEverConstructable, bool bAffliction, bool bExposed) const
{
	PROFILE_FUNC();

	if (eBuilding == NO_BUILDING) return false;

	bool bResult;
	bool bHaveCachedResult;

	//	Cache the param variant with false, true, true as this is used VERY heavily and
	//	also the default param flavor
	if ( !bContinue && bTestVisible && bIgnoreCost && eIgnoreTechReq == NO_TECH && probabilityEverConstructable == NULL && !bAffliction && !bExposed)
	{
		if ( m_bCanConstructCached[eBuilding] )
		{
			bResult = m_bCanConstruct[eBuilding];
			bHaveCachedResult = true;
		}
		else
		{
			bHaveCachedResult = false;
		}

		if ( !bHaveCachedResult )
		{
			bResult = canConstructInternal(eBuilding, bContinue, bTestVisible, bIgnoreCost);

			m_bCanConstruct[eBuilding] = bResult;
			m_bCanConstructCached[eBuilding] = true;
		}
	}
	else if ( !bContinue && !bTestVisible && !bIgnoreCost && eIgnoreTechReq == NO_TECH && probabilityEverConstructable == NULL && !bAffliction && !bExposed)
	{
		if ( m_bCanConstructCachedDefaultParam[eBuilding] )
		{
			bResult = m_bCanConstructDefaultParam[eBuilding];
			bHaveCachedResult = true;
		}
		else
		{
			bHaveCachedResult = false;
		}

		if ( !bHaveCachedResult )
		{
			bResult = canConstructInternal(eBuilding, bContinue, bTestVisible, bIgnoreCost);

			m_bCanConstructDefaultParam[eBuilding] = bResult;
			m_bCanConstructCachedDefaultParam[eBuilding] = true;
		}
	}
	else
	{
		bResult = canConstructInternal(eBuilding, bContinue, bTestVisible, bIgnoreCost, eIgnoreTechReq, probabilityEverConstructable, bExposed);
	}

	return bResult;
}

bool CvPlayer::canConstructInternal(BuildingTypes eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost, TechTypes eIgnoreTechReq, int* probabilityEverConstructable, bool bExposed) const
{
	PROFILE_FUNC();

	const CvTeamAI& currentTeam = GET_TEAM(getTeam());
	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	if ( probabilityEverConstructable )
	{
		*probabilityEverConstructable = 0;
	}

	const SpecialBuildingTypes eSpecialBuilding = kBuilding.getSpecialBuilding();
	if (!bExposed)
	{
		if (eIgnoreTechReq != kBuilding.getPrereqAndTech() && !currentTeam.isHasTech((TechTypes)kBuilding.getPrereqAndTech()))
		{
			return false;
		}

		foreach_(const TechTypes ePrereqTech, kBuilding.getPrereqAndTechs())
		{
			if (eIgnoreTechReq != ePrereqTech && !currentTeam.isHasTech(ePrereqTech))
			{
				return false;
			}
		}

		if (eSpecialBuilding != NO_SPECIALBUILDING)
		{
			const TechTypes eRequiredTech = GC.getSpecialBuildingInfo(eSpecialBuilding).getTechPrereq();
			if (eIgnoreTechReq != eRequiredTech && !currentTeam.isHasTech(eRequiredTech))
			{
				return false;
			}
		}

		if (!bTestVisible)
		{
			bool bValid = false;
			bool bRequires = false;
			foreach_(const HeritageTypes eTypeX, kBuilding.getPrereqOrHeritage())
			{
				bRequires = true;

				if (hasHeritage(eTypeX))
				{
					bValid = true;
					break;
				}
			}
			if (bRequires && !bValid)
			{
				return false;
			}
		}
	}

	if (currentTeam.isObsoleteBuilding(eBuilding))
	{
		return false;
	}

	if (isBuildingMaxedOut(eBuilding)
	|| currentTeam.isBuildingMaxedOut(eBuilding)
	|| GC.getGame().isBuildingMaxedOut(eBuilding))
	{
		return false;
	}

	if (eSpecialBuilding != NO_SPECIALBUILDING && isBuildingGroupMaxedOut(eSpecialBuilding))
	{
		return false;
	}

	if (!hasValidCivics(eBuilding))
	{
		if (probabilityEverConstructable)
		{
			*probabilityEverConstructable = 20;
		}
		return false;
	}

	if (kBuilding.isPrereqWar() && !bExposed && !currentTeam.isAtWar())
	{
		if (probabilityEverConstructable)
		{
			*probabilityEverConstructable = 10;
		}
		return false;
	}

	if (!GC.getGame().canEverConstruct(eBuilding))
	{
		return false;
	}

	if (!bIgnoreCost && kBuilding.getProductionCost() == -1)
	{
		return false;
	}

	if (!bExposed)
	{
		const ReligionTypes eType = (ReligionTypes) kBuilding.getPrereqStateReligion();

		if (eType != NO_RELIGION && eType != getStateReligion())
		{
			if (probabilityEverConstructable)
			{
				*probabilityEverConstructable = algo::any_of(cities(), CvCity::fn::isHasReligion(eType)) ? 20 : 5;
			}
			return false;
		}
	}

	if (GC.getGame().countCivTeamsEverAlive() < kBuilding.getNumTeamsPrereq())
	{
		return false;
	}

	if (kBuilding.getVictoryPrereq() != NO_VICTORY)
	{
		if (isMinorCiv()
		|| !GC.getGame().isVictoryValid((VictoryTypes)kBuilding.getVictoryPrereq())
		|| currentTeam.getVictoryCountdown((VictoryTypes)kBuilding.getVictoryPrereq()) >= 0)
		{
			return false;
		}
	}

	if (kBuilding.getMaxStartEra() != NO_ERA && GC.getGame().getStartEra() > kBuilding.getMaxStartEra())
	{
		return false;
	}

	const int numBuildingInfos = GC.getNumBuildingInfos();

	if (!kBuilding.getPrereqNumOfBuildings().empty())
	{
		for (int iI = 0; iI < numBuildingInfos; iI++)
		{
			const BuildingTypes eBuildingX = static_cast<BuildingTypes>(iI);

			if (!currentTeam.isObsoleteBuilding(eBuildingX)
			&& getBuildingCount(eBuildingX) < getBuildingPrereqBuilding(eBuilding, eBuildingX, 0))
			{
				return false;
			}
		}
	}

	if (!bTestVisible)
	{
		if (GC.getGame().isBuildingMaxedOut(eBuilding, (currentTeam.getBuildingMaking(eBuilding) + (bContinue ? -1 : 0))))
		{
			return false;
		}

		if (currentTeam.isBuildingMaxedOut(eBuilding, (currentTeam.getBuildingMaking(eBuilding) + (bContinue ? -1 : 0))))
		{
			return false;
		}

		if (isBuildingMaxedOut(eBuilding, (getBuildingMaking(eBuilding) + (bContinue ? -1 : 0))))
		{
			return false;
		}

		if (eSpecialBuilding != NO_SPECIALBUILDING && isBuildingGroupMaxedOut(eSpecialBuilding, getBuildingGroupMaking(eSpecialBuilding) + ((bContinue) ? -1 : 0)))
		{
			return false;
		}

		if (GC.getGame().isNoNukes() && kBuilding.isAllowsNukes())
		{
			return false;
		}

		if (eSpecialBuilding != NO_SPECIALBUILDING
		&& !GC.getGame().isSpecialBuildingValid(kBuilding.getSpecialBuilding()))
		{
			return false;
		}

		if (getNumCities() < kBuilding.getNumCitiesPrereq())
		{
			if (probabilityEverConstructable)
			{
				*probabilityEverConstructable = 20;
			}
			return false;
		}

		if (getHighestUnitLevel() < kBuilding.getUnitLevelPrereq())
		{
			if (probabilityEverConstructable)
			{
				*probabilityEverConstructable = 20;
			}
			return false;
		}

		if (!kBuilding.getPrereqNumOfBuildings().empty())
		{
			for (int iI = 0; iI < numBuildingInfos; iI++)
			{
				const BuildingTypes eBuildingX = static_cast<BuildingTypes>(iI);

				if (!currentTeam.isObsoleteBuilding(eBuildingX)
				&& getBuildingCount(eBuildingX) < getBuildingPrereqBuilding(eBuilding, eBuildingX, bContinue ? 0 : getBuildingMaking(eBuilding)))
				{
					if (probabilityEverConstructable)
					{
						*probabilityEverConstructable = 20;
					}
					return false;
				}
			}
		}

		if (!(*getPropertiesConst() <= *(kBuilding.getPrereqPlayerMaxProperties())))
			return false;

		if (!(*getPropertiesConst() >= *(kBuilding.getPrereqPlayerMinProperties())))
			return false;
	}

	return true;
}

bool CvPlayer::canCreate(ProjectTypes eProject, bool bContinue, bool bTestVisible) const
{
	PROFILE_EXTRA_FUNC();
	if (isNPC())
	{
		return false;
	}

	const CvProjectInfo& kProject = GC.getProjectInfo(eProject);
	if (kProject.getProductionCost() == -1)
	{
		return false;
	}

	if (!GET_TEAM(getTeam()).isHasTech(kProject.getTechPrereq()))
	{
		return false;
	}

	if (kProject.getVictoryPrereq() != NO_VICTORY)
	{
		if (!GC.getGame().isVictoryValid((VictoryTypes)kProject.getVictoryPrereq()))
		{
			return false;
		}

		if (isMinorCiv())
		{
			return false;
		}

		if (GET_TEAM(getTeam()).getVictoryCountdown((VictoryTypes)kProject.getVictoryPrereq()) >= 0)
		{
			return false;
		}
	}

	if (GC.getGame().isProjectMaxedOut(eProject))
	{
		return false;
	}

	if (GET_TEAM(getTeam()).isProjectMaxedOut(eProject))
	{
		return false;
	}

	if (!bTestVisible)
	{
		if (GC.getGame().isProjectMaxedOut(eProject, (GET_TEAM(getTeam()).getProjectMaking(eProject) + ((bContinue) ? -1 : 0))))
		{
			return false;
		}

		if (GET_TEAM(getTeam()).isProjectMaxedOut(eProject, (GET_TEAM(getTeam()).getProjectMaking(eProject) + ((bContinue) ? -1 : 0))))
		{
			return false;
		}

		if (GC.getGame().isNoNukes() && kProject.isAllowsNukes())
		{
			return false;
		}

		if (kProject.getAnyoneProjectPrereq() != NO_PROJECT)
		{
			if (GC.getGame().getProjectCreatedCount((ProjectTypes)kProject.getAnyoneProjectPrereq()) == 0)
			{
				return false;
			}
		}

		for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
		{
			if (GET_TEAM(getTeam()).getProjectCount((ProjectTypes)iI) < kProject.getProjectsNeeded(iI))
			{
				return false;
			}
		}
	}

	return true;
}


bool CvPlayer::canMaintain(ProcessTypes eProcess) const
{
	return GET_TEAM(getTeam()).isHasTech(GC.getProcessInfo(eProcess).getTechPrereq());
}


bool CvPlayer::isProductionMaxedUnit(UnitTypes eUnit) const
{
	return eUnit != NO_UNIT && (GC.getGame().isUnitMaxedOut(eUnit) || isUnitMaxedOut(eUnit));
}


bool CvPlayer::isProductionMaxedBuilding(BuildingTypes building, bool bAcquireCity) const
{
	if (building == NO_BUILDING)
	{
		return false;
	}

	if (!bAcquireCity && GC.getGame().isBuildingMaxedOut(building))
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isBuildingMaxedOut(building))
	{
		return true;
	}

	if (isBuildingMaxedOut(building, (bAcquireCity ? GC.getBuildingInfo(building).getExtraPlayerInstances() : 0)))
	{
		return true;
	}

	return false;
}


bool CvPlayer::isProductionMaxedProject(ProjectTypes eProject) const
{
	if (eProject == NO_PROJECT)
	{
		return false;
	}

	if (GC.getGame().isProjectMaxedOut(eProject))
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isProjectMaxedOut(eProject))
	{
		return true;
	}

	return false;
}


int64_t CvPlayer::getBaseUnitCost100(const UnitTypes eUnit) const
{
	int64_t iBaseCost = GC.getUnitInfo(eUnit).getProductionCost();

	if (iBaseCost < 0)
	{
		return -100;
	}
	if (iBaseCost > 0)
	{
		// We keep the 100 multiplier from the gamespeed modifier on purpose.
		iBaseCost *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent();

		int iMod = 100;
		if (!GC.getGame().isOption(GAMEOPTION_TECH_BEELINE_STINGS))
		{
			EraTypes eEra = (EraTypes)GC.getGame().getStartEra();
			if (GC.getUnitInfo(eUnit).getEraInfo() != NO_ERA)
			{
				eEra = (EraTypes)GC.getUnitInfo(eUnit).getEraInfo();
			}
			iMod = GC.getEraInfo(eEra).getTrainPercent();
		}
		else iMod = GC.getEraInfo((EraTypes)getCurrentEra()).getTrainPercent();

		iBaseCost *= iMod;
		iBaseCost /= 100;

		// Trade units aren't impacted by SM production reduction due to ROI oddities. They can't merge anyway, so...
		if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS) && GC.getUnitInfo(eUnit).getUnitCombatType() != GC.getInfoTypeForString("UNITCOMBAT_TRADE"))
		{
			iMod = GC.getUNIT_PRODUCTION_PERCENT_SM();
		}
		else iMod = GC.getUNIT_PRODUCTION_PERCENT();

		iBaseCost *= iMod;
		iBaseCost /= 100;

		if (isNormalAI())
		{
			if (isWorldUnit(eUnit))
			{
				iMod = GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIWorldTrainPercent() - 100;
			}
			else iMod = GC.getHandicapInfo(GC.getGame().getHandicapType()).getAITrainPercent() - 100;

			iMod += GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIPerEraModifier() * getCurrentEra();

			iBaseCost = getModifiedIntValue64(iBaseCost, iMod);
		}
	}
	// The getUnitExtraCost() is where we get the cost for a settler unit (that's ALL this does).
	// The cost scales to GROWTH factors rather than training factors; gamespeed and other relevant modifiers are factored in.
	return iBaseCost + 100 * getUnitExtraCost(eUnit);
}

int CvPlayer::getProductionNeeded(UnitTypes eUnit) const
{
	int64_t iProductionNeeded = getBaseUnitCost100(eUnit);
	if (iProductionNeeded < 1)
	{
		if (iProductionNeeded < 0)
		{
			return -1;
		}
		return 1;
	}
	// Toffer - I split this last modifier away from the rest as it is only interesting when intending to create a new unit.
	//	When upgrading a unit we need the cost of the unit to be upgraded, in the interest of getting rid of it rather than creating a new unit.
	//	So when upgerading, we only want this last modifier to apply to the unit it is going to upgrade to when comparing their hammer costs to derive the gold cost of the upgrade.
	{
		const int iInstanceCostModifier = GC.getUnitInfo(eUnit).getInstanceCostModifier();
		if (iInstanceCostModifier != 0)
		{
			if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
			{
				iProductionNeeded = getModifiedIntValue64(iProductionNeeded, getUnitCountSM(eUnit) * iInstanceCostModifier);
			}
			else iProductionNeeded = getModifiedIntValue64(iProductionNeeded, getUnitCount(eUnit) * iInstanceCostModifier);
		}
	}
	iProductionNeeded /= 100;

	if (iProductionNeeded >= MAX_INT)
	{
		return MAX_INT;
	}
	return std::max(1, static_cast<int>(iProductionNeeded));
}


int CvPlayer::getProductionNeeded(BuildingTypes eBuilding) const
{

	int iBaseCost = GC.getBuildingInfo(eBuilding).getProductionCost();
	if (iBaseCost < 1)
	{
		return -1;
	}

    const int iCostBySize = GC.getBuildingInfo(eBuilding).getProductionCostSize();
    const int iCostByCount = GC.getBuildingInfo(eBuilding).getProductionCostCount();
    const int iCostByMaterials = GC.getBuildingInfo(eBuilding).getProductionCostMaterials();
    const int iCostByComplexity = GC.getBuildingInfo(eBuilding).getProductionCostComplexity();

    // Calculate total modifier
    float totalModifier = 1.0f;

    if (iCostBySize == 0) {
        totalModifier -= 0.2f;
    } else if (iCostBySize == 2) {
        totalModifier += 0.15f;
    } else if (iCostBySize == 3) {
        totalModifier += 0.3f;
    }

    if (iCostByCount == 0) {
        totalModifier -= 0.05f;
    } else if (iCostByCount == 2) {
        totalModifier += 0.15f;
    }

    if (iCostByMaterials == 0) {
        totalModifier -= 0.1f;
    } else if (iCostByMaterials == 2) {
        totalModifier += 0.15f;
    }

    if (iCostByComplexity == 0) {
        totalModifier -= 0.2f;
    } else if (iCostByComplexity == 2) {
        totalModifier += 0.2f;
    }

    //iBaseCost = static_cast<int>(iBaseCost);

    // Apply total modifier to base cost
    if(GC.getGame().isOption(GAMEOPTION_REALISTIC_BUILDING_COST)){
		FAssert(iBaseCost * totalModifier < MAX_INT);
        iBaseCost = static_cast<int>(iBaseCost * totalModifier);
    }

	uint64_t iProductionNeeded = (uint64_t) 100*iBaseCost;

	iProductionNeeded *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent();
	iProductionNeeded /= 100;

	const EraTypes eEra = getCurrentEra();
	iProductionNeeded *= GC.getEraInfo(eEra).getConstructPercent();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getBUILDING_PRODUCTION_PERCENT();
	iProductionNeeded /= 100;

	iProductionNeeded = getModifiedIntValue64(iProductionNeeded, getBuildingCostModifier(eBuilding));

	if (isNormalAI())
	{
		int iMod = 0;
		if (eEra != NO_ERA)
		{
			iMod += eEra * GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIPerEraModifier();
		}
		if (isWorldWonder(eBuilding))
		{
			iMod += GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIWorldConstructPercent() - 100;
		}
		else iMod += GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIConstructPercent() - 100;

		if (iMod != 0)
		{
			iProductionNeeded = getModifiedIntValue64(iProductionNeeded, iMod);
		}
	}
	iProductionNeeded /= 100;

	if (iProductionNeeded >= MAX_INT) return MAX_INT;

	return std::max(1, (int)iProductionNeeded);
}


int CvPlayer::getProductionNeeded(ProjectTypes eProject) const
{
	int iInitialProduction = GC.getProjectInfo(eProject).getProductionCost();
	if (iInitialProduction < 0)
	{
		return -1;
	}
	uint64_t iProductionNeeded = (uint64_t)iInitialProduction;

	// Scale up by 100 at this point
	iProductionNeeded *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent();

	const EraTypes eEra = getCurrentEra();
	int iModifier = 0;
	if (!GC.getGame().isOption(GAMEOPTION_TECH_BEELINE_STINGS) && GC.getProjectInfo(eProject).getTechPrereq() != NO_TECH)
	{
		iModifier = GC.getEraInfo((EraTypes)GC.getTechInfo(GC.getProjectInfo(eProject).getTechPrereq()).getEra()).getCreatePercent();
	}
	else iModifier = GC.getEraInfo(eEra).getCreatePercent();

	iProductionNeeded *= iModifier;
	iProductionNeeded /= 100;

	iModifier = GC.getDefineINT("BUILDING_PRODUCTION_PERCENT");
	iProductionNeeded *= iModifier;
	iProductionNeeded /= 100;

	if (isNormalAI())
	{
		int iMod = 0;
		if (eEra != NO_ERA)
		{
			iMod += eEra * GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIPerEraModifier();
		}
		if (isWorldProject(eProject))
		{
			iMod += GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIWorldCreatePercent() - 100;
		}
		else
		{
			iMod += GC.getHandicapInfo(GC.getGame().getHandicapType()).getAICreatePercent() - 100;
		}
		if (iMod != 0)
		{
			iProductionNeeded = getModifiedIntValue64(iProductionNeeded, iMod);
		}
	}

	iProductionNeeded /= 100;

	if (iProductionNeeded > MAX_INT)
	{
		iProductionNeeded = MAX_INT;
	}

	int iTotal = (int)iProductionNeeded;

	return std::max(1, iTotal);
}

int CvPlayer::getProductionModifier(UnitTypes eUnit) const
{
	PROFILE_EXTRA_FUNC();
	int iMultiplier = 0;

	if (!GC.getUnitInfo(eUnit).isNoNonTypeProdMods() && GC.getUnitInfo(eUnit).isMilitaryProduction())
	{
		iMultiplier += getMilitaryProductionModifier();
	}
	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			const CvTraitInfo& kTrait = GC.getTraitInfo((TraitTypes)iI);

			for (int j = 0; j < kTrait.getNumUnitProductionModifiers(); j++)
			{
				if ((UnitTypes)kTrait.getUnitProductionModifier(j).eUnit == eUnit)
				{
					iMultiplier += kTrait.getUnitProductionModifier(j).iModifier;
					break;
				}
			}
			if (GC.getUnitInfo(eUnit).getSpecialUnitType() != NO_SPECIALUNIT)
			{
				for (int j = 0; j < kTrait.getNumSpecialUnitProductionModifiers(); j++)
				{
					if ((SpecialUnitTypes)kTrait.getSpecialUnitProductionModifier(j).eSpecialUnit == GC.getUnitInfo(eUnit).getSpecialUnitType())
					{
						iMultiplier += kTrait.getSpecialUnitProductionModifier(j).iModifier;
						break;
					}
				}
			}
		}
	}
	return iMultiplier;
}

int CvPlayer::getProductionModifier(BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	int iMultiplier = 0;
	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		const TraitTypes eTrait = ((TraitTypes)iI);
		if (hasTrait(eTrait))
		{
			const CvTraitInfo& kTrait = GC.getTraitInfo(eTrait);

			for (int j = 0; j < kTrait.getNumBuildingProductionModifiers(); j++)
			{
				if (kTrait.getBuildingProductionModifier(j).eBuilding == eBuilding)
				{
					iMultiplier += kTrait.getBuildingProductionModifier(j).iModifier;
				}
			}
			const SpecialBuildingTypes eSpecialBuilding = GC.getBuildingInfo(eBuilding).getSpecialBuilding();

			if (eSpecialBuilding != NO_SPECIALBUILDING)
			{
				foreach_(const SpecialBuildingModifier& pair, kTrait.getSpecialBuildingProductionModifiers())
				{
					if (pair.first == eSpecialBuilding)
					{
						iMultiplier += pair.second;
					}
				}
			}
		}
	}

	if (::isWorldWonder(eBuilding))
	{
		iMultiplier += getMaxGlobalBuildingProductionModifier();
	}

	if (::isTeamWonder(eBuilding))
	{
		iMultiplier += getMaxTeamBuildingProductionModifier();
	}

	if (::isNationalWonder(eBuilding))
	{
		iMultiplier += getMaxPlayerBuildingProductionModifier();
	}

	return iMultiplier;
}

int CvPlayer::getProductionModifier(ProjectTypes eProject) const
{
	int iMultiplier = 0;

	if (GC.getProjectInfo(eProject).isSpaceship())
	{
		iMultiplier += getSpaceProductionModifier();
	}
	return iMultiplier;
}

int CvPlayer::getBuildingPrereqBuilding(BuildingTypes eBuilding, BuildingTypes ePrereqBuilding, int iExtra) const
{
	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	int iPrereqs = kBuilding.getPrereqNumOfBuildings().getValue(ePrereqBuilding);

	// dont bother with the rest of the calcs if we have no prereqs
	if (iPrereqs < 1)
	{
		return 0;
	}
	if (kBuilding.isForceNoPrereqScaling() || isLimitedWonder(ePrereqBuilding))
	{
		if (iPrereqs > 1 && GC.getGame().isOption(GAMEOPTION_CHALLENGE_ONE_CITY))
		{
			return 1;
		}
		return iPrereqs;
	}
	iPrereqs = getModifiedIntValue(iPrereqs, GC.getWorldInfo(GC.getMap().getWorldSize()).getBuildingPrereqModifier());

	if (!isLimitedWonder(eBuilding))
	{
		iPrereqs *= 1 + getBuildingCount(eBuilding) + iExtra;
	}

	if (iPrereqs > 1 && GC.getGame().isOption(GAMEOPTION_CHALLENGE_ONE_CITY))
	{
		return 1;
	}
	return std::max(1, iPrereqs);
}


void CvPlayer::removeBuilding(BuildingTypes building)
{
	PROFILE_EXTRA_FUNC();
	if (building != NO_BUILDING)
	{
		foreach_(CvCity* pLoopCity, cities() | filtered(bind(CvCity::hasBuilding, _1, building)))
		{
			pLoopCity->changeHasBuilding(building, false);
			break;
		}
	}
}


void CvPlayer::processBuilding(BuildingTypes eBuilding, int iChange, CvArea* pArea, bool bReligiouslyDisabling)
{
	PROFILE_EXTRA_FUNC();
	FAssert(iChange == 1 || iChange == -1);

	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	if (!bReligiouslyDisabling)
	{
		const BuildingTypes eFreeBuilding = kBuilding.getFreeBuilding();
		if (eFreeBuilding != NO_BUILDING)
		{
			changeFreeBuildingCount(eFreeBuilding, iChange);
		}

		const BuildingTypes eFreeAreaBuilding = kBuilding.getFreeAreaBuilding();
		if (eFreeAreaBuilding != NO_BUILDING)
		{
			// Toffer - ToDo - Would make more sense to store this info in the CvArea object, mapped to player, rather than duplicating the info across all cities in the area.
			algo::for_each(cities() | filtered(CvCity::fn::area() == pArea), CvCity::fn::changeFreeAreaBuildingCount(eFreeAreaBuilding, iChange));
		}

		if (kBuilding.getCivicOption() != NO_CIVICOPTION)
		{
			changeHasCivicOptionCount(((CivicOptionTypes)kBuilding.getCivicOption()), iChange);
		}
	}
	changeMaintenanceModifier(kBuilding.getGlobalMaintenanceModifier() * iChange);
	changeDistanceMaintenanceModifier(kBuilding.getDistanceMaintenanceModifier() * iChange);
	changeNumCitiesMaintenanceModifier(kBuilding.getNumCitiesMaintenanceModifier() * iChange);
	changeCoastalDistanceMaintenanceModifier(kBuilding.getCoastalDistanceMaintenanceModifier() * iChange);
	changeConnectedCityMaintenanceModifier(kBuilding.getConnectedCityMaintenanceModifier() * iChange);

	changeGreatPeopleRateModifier(kBuilding.getGlobalGreatPeopleRateModifier() * iChange);
	changeGreatGeneralRateModifier(kBuilding.getGreatGeneralRateModifier() * iChange);
	changeDomesticGreatGeneralRateModifier(kBuilding.getDomesticGreatGeneralRateModifier() * iChange);
	changeAnarchyModifier(kBuilding.getAnarchyModifier() * iChange);
	changeGoldenAgeModifier(kBuilding.getGoldenAgeModifier() * iChange);
	changeHurryModifier(kBuilding.getGlobalHurryModifier() * iChange);
	changeFreeExperience(kBuilding.getGlobalFreeExperience() * iChange);
	changeWarWearinessModifier(kBuilding.getGlobalWarWearinessModifier() * iChange);
	pArea->changeFreeSpecialist(getID(), (kBuilding.getAreaFreeSpecialist() * iChange));
	changeFreeSpecialist(kBuilding.getGlobalFreeSpecialist() * iChange);
	changeCoastalTradeRoutes(kBuilding.getCoastalTradeRoutes() * iChange);
	changeTradeRoutes(kBuilding.getGlobalTradeRoutes() * iChange);

	changePopulationgrowthratepercentage(kBuilding.getGlobalPopulationgrowthratepercentage(), (iChange==1));
	changeForceAllTradeRoutes(kBuilding.isForceAllTradeRoutes() * iChange);
	changeBuildingInflation(kBuilding.getInflationModifier() * iChange);
	{
		const int iWorldTradeRoute = kBuilding.getWorldTradeRoutes() * iChange;
		if (iWorldTradeRoute != 0)
		{
			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				changeTradeRoutes(iWorldTradeRoute);
			}
		}
	}

	if (kBuilding.getAreaHealth() > 0)
	{
		pArea->changeBuildingGoodHealth(getID(), (kBuilding.getAreaHealth() * iChange));
	}
	else
	{
		pArea->changeBuildingBadHealth(getID(), (kBuilding.getAreaHealth() * iChange));
	}
	if (kBuilding.getGlobalHealth() > 0)
	{
		changeBuildingGoodHealth(kBuilding.getGlobalHealth() * iChange);
	}
	else
	{
		changeBuildingBadHealth(kBuilding.getGlobalHealth() * iChange);
	}
	pArea->changeBuildingHappiness(getID(), (kBuilding.getAreaHappiness() * iChange));
	changeBuildingHappiness(kBuilding.getGlobalHappiness() * iChange);
	changeWorkerSpeedModifier(kBuilding.getWorkerSpeedModifier() * iChange);
	changeSpaceProductionModifier(kBuilding.getGlobalSpaceProductionModifier() * iChange);
	changeCityDefenseModifier(kBuilding.getAllCityDefenseModifier() * iChange);

	changeUnitUpgradePriceModifier(kBuilding.getUnitUpgradePriceModifier() * iChange);
	changeRevIdxNational(kBuilding.getRevIdxNational() * iChange);

	pArea->changeBorderObstacleCount(getTeam(), kBuilding.isAreaBorderObstacle() ? iChange : 0);

	pArea->changeMaintenanceModifier(getID(), (kBuilding.getAreaMaintenanceModifier() * iChange));

	if (kBuilding.getOtherAreaMaintenanceModifier() != 0)
	{
		foreach_(CvArea * pLoopArea, GC.getMap().areas())
		{
			if (pLoopArea != pArea)
			{
				pLoopArea->changeMaintenanceModifier(getID(), (kBuilding.getOtherAreaMaintenanceModifier()  * iChange));
			}
		}
	}

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		changeSeaPlotYield(((YieldTypes)iI), (kBuilding.getGlobalSeaPlotYieldChange(iI) * iChange));
		pArea->changeYieldRateModifier(getID(), ((YieldTypes)iI), (kBuilding.getAreaYieldModifier(iI) * iChange));
		changeYieldRateModifier(((YieldTypes)iI), (kBuilding.getGlobalYieldModifier(iI) * iChange));
	}

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		changeCommerceRateModifierfromBuildings(((CommerceTypes)iI), (kBuilding.getGlobalCommerceModifier(iI) * iChange));
		changeSpecialistExtraCommerce(((CommerceTypes)iI), (kBuilding.getSpecialistExtraCommerce(iI) * iChange));
		changeStateReligionBuildingCommerce(((CommerceTypes)iI), (kBuilding.getStateReligionCommerce(iI) * iChange));
		changeCommerceFlexibleCount(((CommerceTypes)iI), (kBuilding.isCommerceFlexible(iI)) ? iChange : 0);
	}

	foreach_(const BuildingModifier2& pair, kBuilding.getBuildingHappinessChanges())
	{
		changeExtraBuildingHappiness(pair.first, pair.second * iChange);
	}
	foreach_(const BuildingModifier2& modifier, kBuilding.getGlobalBuildingProductionModifiers())
	{
		changeBuildingProductionModifier(modifier.first, modifier.second * iChange);
	}
	foreach_(const BuildingModifier2& modifier, kBuilding.getGlobalBuildingCostModifiers())
	{
		changeBuildingCostModifier(modifier.first, modifier.second * iChange);
	}

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			changeExtraSpecialistYield(((SpecialistTypes)iI), ((YieldTypes)iJ), (kBuilding.getSpecialistYieldChange(iI, iJ) * iChange));
		}
		for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			changeExtraSpecialistCommerce(((SpecialistTypes)iI), ((CommerceTypes)iJ), (kBuilding.getSpecialistCommerceChange(iI, iJ) * iChange));
		}
	}
	//TB Building tags
	changeExtraNationalCaptureProbabilityModifier(kBuilding.getNationalCaptureProbabilityModifier() * iChange);
	changeExtraNationalCaptureResistanceModifier(kBuilding.getNationalCaptureResistanceModifier() * iChange);

	for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
	{
		changeHurryCount(((HurryTypes)iI), ((kBuilding.isHurry(iI)) ? iChange : 0));
	}
}


// If this func returns false, the build is not available.
// bTestVisible are to show builds that are not yet available in the UI as greyed out options.
bool CvPlayer::canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestVisible, bool bIncludePythonOverrides) const
{
	PROFILE_FUNC();

	if (pPlot && !pPlot->canBuild(eBuild, getID(), bTestVisible, bIncludePythonOverrides))
	{
		return false;
	}

	const CvBuildInfo& kBuild = GC.getBuildInfo(eBuild);

	if (kBuild.isDisabled()
	||	kBuild.getObsoleteTech() != NO_TECH && GET_TEAM(getTeam()).isHasTech(kBuild.getObsoleteTech()))
	{
		return false;
	}

	// check money, terrain and feature prereqs
	if (pPlot)
	{
		// check gold
		if (!bTestVisible && std::max<int64_t>(0, getGold()) < getBuildCost(pPlot, eBuild))
		{
			return false;
		}

		const TechTypes eTechPrereq = kBuild.getTechPrereq();

		if (!bTestVisible || eTechPrereq != NO_TECH && getCurrentEra() < GC.getTechInfo(eTechPrereq).getEra())
		{
			if (eTechPrereq != NO_TECH && !GET_TEAM(getTeam()).isHasTech(eTechPrereq))
			{
				return false;
			}

			if (kBuild.getRoute() == NO_ROUTE || GC.getGame().isOption(GAMEOPTION_ADVANCED_ROUTES) || GC.getRouteInfo((RouteTypes)kBuild.getRoute()).isSeaTunnel())
			{
				{
					const FeatureTypes plotFeature = pPlot->getFeatureType();

					if (plotFeature != NO_FEATURE && !GET_TEAM(getTeam()).isHasTech(kBuild.getFeatureTech(plotFeature)))
					{
						return false;
					}
				}
				// terrain is similar to feature; can't build if don't have tech, etc, only diff is looping thru terrain structs because that's how we roll
				foreach_(const TerrainStructs& kTerrainStruct, kBuild.getTerrainStructs())
				{
					const TerrainTypes eTerrain = kTerrainStruct.eTerrain;
					if (
					(
							eTerrain == pPlot->getTerrainType()
						||	eTerrain == GC.getTERRAIN_PEAK() && pPlot->isAsPeak()
						||	eTerrain == GC.getTERRAIN_HILL() && pPlot->isHills()
					)
					&&	kTerrainStruct.ePrereqTech != NO_TECH
					&&	!GET_TEAM(getTeam()).isHasTech(kTerrainStruct.ePrereqTech))
					{
						return false;
					}
				}
			}
		}

		if (pPlot->isWater() && kBuild.getRoute() != NO_ROUTE && !GC.getRouteInfo((RouteTypes)kBuild.getRoute()).isSeaTunnel())
		{
			return false;
		}
	}

	// Unique improvement checks
	{
		const ImprovementTypes eImprovement = kBuild.getImprovement();

		if (
			eImprovement != NO_IMPROVEMENT
		&&
			(
				GC.getImprovementInfo(eImprovement).isNational() && getImprovementCount(eImprovement) > 0
				||
				GC.getImprovementInfo(eImprovement).isGlobal() && GC.getGame().getImprovementCount(eImprovement) > 0
			)
		) return false;
	}
	return true;
}


// Returns the cost
int CvPlayer::getBuildCost(const CvPlot* pPlot, BuildTypes eBuild) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumBuildInfos(), eBuild);

	if (pPlot->getBuildProgress(eBuild) > 0)
	{
		return 0;
	}

	int iCost = GC.getBuildInfo(eBuild).getCost();
	if (iCost != 0)
	{
		if (GC.getBuildInfo(eBuild).getRoute() != NO_ROUTE && pPlot->getRouteType() != NO_ROUTE && GC.getDefineINT("ROUTES_UPGRADE") > 0)
		{
			for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
			{
				if (GC.getBuildInfo((BuildTypes)iI).getRoute() == pPlot->getRouteType())
				{
					iCost = std::max(0, iCost - GC.getBuildInfo((BuildTypes)iI).getCost());
					break;
				}
			}
		}
		iCost *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent();
		iCost /= 100;
	}
	return iCost;
}

// Checks route validity from either plot/builder specific standpoint or player in-general standpoint based on args
bool CvPlayer::isRouteValid(RouteTypes eRoute, BuildTypes eRouteBuild, const CvPlot* pPlot, const CvUnit* pBuilder) const
{
	// Player in general; have tech reqs and not obsolete?
	if (pPlot == NULL)
	{
		if (!GET_TEAM(getTeam()).isHasTech(GC.getBuildInfo(eRouteBuild).getTechPrereq())
		|| GC.getBuildInfo(eRouteBuild).getObsoleteTech() != NO_TECH
		&& GET_TEAM(getTeam()).isHasTech(GC.getBuildInfo(eRouteBuild).getObsoleteTech()))
		{
			return false;
		}
	}
	else if (pPlot->getRouteType() != eRoute && !canBuild(pPlot, eRouteBuild))
	{
		return false;
	}
	// unit specific query; Cv::Unit canBuild is big func...
	return pBuilder == NULL || pBuilder->canBuild(pPlot,eRouteBuild);
}

// bConnect = true when looking in general regardless of plot; finding best available route in theory, on city center, etc
RouteTypes CvPlayer::getBestRoute(const CvPlot* pPlot, bool bConnect, const CvUnit* pBuilder) const
{
	PROFILE_FUNC();

	// Save best route type per player each turn, then only check others if can't do best.
	if (m_eBestRoute == NO_ROUTE)
	{
		// Set cache to the best route by tech level ignoring unit capability/map behavior
		m_eBestRoute = getBestRouteInternal(pPlot, bConnect, NULL, &m_eBestRouteBuild);
	}

	if (m_eBestRoute != NO_ROUTE && isRouteValid(m_eBestRoute, m_eBestRouteBuild, pPlot, pBuilder))
	{
		return m_eBestRoute;
		/* Blaze - Possible temporary hack to work around routes with same value. System MUST be overhauled if more are included...
		// Maybe store a bool per plot if tile has best route available, updating whenever improvement change/relevant tech acquired/worker on plot/etc?
		if (pPlot == NULL ||
			pPlot && GC.getRouteInfo(pPlot->getRouteType()).getValue() != GC.getRouteInfo(m_eBestRoute).getValue())
		{
			return m_eBestRoute;
		}  */
	}
	return getBestRouteInternal(pPlot, bConnect, pBuilder);
}

// This maybe should be in CvPlot from how often it calls pPlot...? Or investigate, use getRouteYieldChangesArray if applicable
// Find the yield difference from current plot yield for given yieldType, if eRoute is placed on pPlot
int CvPlayer::calculatePlotRouteYieldDifference(const CvPlot* pPlot, const RouteTypes eRoute, YieldTypes eYield) const
{
	// We're always gaining the yield type from the proposed route to place; might need to subtract existing route/improvement yields though.
	int iYield = GC.getRouteInfo(eRoute).getYieldChange(eYield);

	bool bIsImprovement = false;
	bool bIsRoute = false;
	if (pPlot->getImprovementType() != NO_IMPROVEMENT) 	bIsImprovement = true;
	if (pPlot->getRouteType() 		!= NO_ROUTE)		bIsRoute = true;

	if (bIsRoute && bIsImprovement)
	{
		iYield += GC.getImprovementInfo(pPlot->getImprovementType()).getRouteYieldChanges(eRoute, eYield)
			   -  GC.getImprovementInfo(pPlot->getImprovementType()).getRouteYieldChanges(pPlot->getRouteType(), eYield)
			   -  GC.getRouteInfo(pPlot->getRouteType()).getYieldChange(eYield);
	}
	else if (bIsRoute)
	{
		iYield -=  GC.getRouteInfo(pPlot->getRouteType()).getYieldChange(eYield);
	}
	else if (bIsImprovement)
	{
		iYield += GC.getImprovementInfo(pPlot->getImprovementType()).getRouteYieldChanges(eRoute, eYield);
	}
	return iYield;
}


RouteTypes CvPlayer::getBestRouteInternal(const CvPlot* pPlot, bool bConnect, const CvUnit* pBuilder, BuildTypes* eBestRouteBuild) const
{
	PROFILE_EXTRA_FUNC();
	int iBestValue = 0;
	RouteTypes eBestRoute = NO_ROUTE;

	const int numBuildInfos = GC.getNumBuildInfos();

	// Find best valid route; xml value most important, tiebreakers in form of yield change and cost.
	for (int iI = 0; iI < numBuildInfos; iI++)
	{
		const RouteTypes eRoute = (RouteTypes)GC.getBuildInfo((BuildTypes)iI).getRoute();

		if (eRoute != NO_ROUTE && isRouteValid(eRoute, (BuildTypes)iI, pPlot, pBuilder))
		{
			int iValue = 0;

			// When bConnect, use only value tiebroken by cheapness
			if (bConnect)
			{
				iValue  = 1000 * GC.getRouteInfo(eRoute).getValue();
				iValue -= GC.getBuildInfo((BuildTypes)iI).getCost();
			}
			// if AI is in trouble, only consider cost and nothing else
			else if (AI_isFinancialTrouble())
			{
				iValue = 1000 - GC.getBuildInfo((BuildTypes)iI).getCost();
			}
			else
			{
				iValue = 1000 * GC.getRouteInfo(eRoute).getValue();
				if (pPlot->getRouteType() != NO_ROUTE)
				{
					// Parallel upgrade check
					iValue -= 1000 * GC.getRouteInfo(pPlot->getRouteType()).getValue();
				}

				// Add (or subtract!) value based on yield diffs
				int iYieldSum = 0;
				for (int i = 0; i < NUM_YIELD_TYPES; i++)
				{
					iYieldSum += calculatePlotRouteYieldDifference(pPlot, eRoute, ((YieldTypes)i));
				}
				iValue += 100 * iYieldSum;

				iValue -= GC.getBuildInfo((BuildTypes)iI).getCost();

				// Extra movement is dependant on unit speed; iMovement != iFlatMovement, so this isn't a very useful measure...
				// iValue += std::max(0, baseMoves / std::min(baseMoves, GC.getRouteInfo(eRoute).getMovementCost()) - 1);
			}

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestRoute = eRoute;
				if ( eBestRouteBuild )
				{
					*eBestRouteBuild = (BuildTypes)iI;
				}
			}
		}
	}
	return eBestRoute;
}


int CvPlayer::getImprovementUpgradeProgressRate(const ImprovementTypes eImprovement) const
{
	const int iMod = getImprovementUpgradeRateModifier() + getImprovementUpgradeRateModifierSpecific(eImprovement);
	if (iMod < 0)
	{
		return std::max(1, 10000 / (100 - iMod));
	}
	return 100 + iMod;
}


int CvPlayer::calculateTotalYield(YieldTypes eYield) const
{
	int TotalYield = ((algo::accumulate(cities() | transformed(CvCity::fn::getYieldRate100(eYield)), 0)) / 100);
	if (TotalYield < MIN_TOL_FALSE_ACCUMULATE)
		return MAX_COMMERCE_RATE_VALUE;
	return TotalYield;
}


int CvPlayer::calculateTotalCityHappiness() const
{
	return algo::accumulate(cities() | transformed(CvCity::fn::happyLevel()), 0);
}


int CvPlayer::calculateTotalExports(YieldTypes eYield) const
{
	PROFILE_EXTRA_FUNC();
	int iTotalExports = 0;

	foreach_(const CvCity* pLoopCity, cities())
	{
// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
		int iCityExports = 0;
#endif

		for (int iTradeLoop = 0; iTradeLoop < pLoopCity->getTradeRoutes(); iTradeLoop++)
		{
			CvCity* pTradeCity = pLoopCity->getTradeCity(iTradeLoop);
			if (pTradeCity)
			{
				if (pTradeCity->getOwner() != getID())
				{
#ifdef _MOD_FRACTRADE
					iCityExports += pLoopCity->calculateTradeYield(eYield, pLoopCity->calculateTradeProfitTimes100(pTradeCity));
#else
					iTotalExports += pLoopCity->calculateTradeYield(eYield, pLoopCity->calculateTradeProfit(pTradeCity));
#endif
				}
			}
		}

#ifdef _MOD_FRACTRADE
		iTotalExports += iCityExports / 100;
#endif
// BUG - Fractional Trade Routes - end
	}

	return iTotalExports;
}


int CvPlayer::calculateTotalImports(YieldTypes eYield) const
{
	PROFILE_EXTRA_FUNC();
	int iTotalImports = 0;

	// Loop through players
	for (int iPlayerLoop = 0; iPlayerLoop < MAX_PC_PLAYERS; iPlayerLoop++)
	{
		if (iPlayerLoop != getID())
		{
// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
			int iCityImports = 0;
#endif

			foreach_(const CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iPlayerLoop).cities())
			{
				for (int iTradeLoop = 0; iTradeLoop < pLoopCity->getTradeRoutes(); iTradeLoop++)
				{
					CvCity* pTradeCity = pLoopCity->getTradeCity(iTradeLoop);
					if (pTradeCity)
					{
						if (pTradeCity->getOwner() == getID())
						{
#ifdef _MOD_FRACTRADE
							iCityImports += pLoopCity->calculateTradeYield(eYield, pLoopCity->calculateTradeProfitTimes100(pTradeCity));
#else
							iTotalImports += pLoopCity->calculateTradeYield(eYield, pLoopCity->calculateTradeProfit(pTradeCity));
#endif
						}
					}
				}
			}

#ifdef _MOD_FRACTRADE
			iTotalImports += iCityImports / 100;
#endif
// BUG - Fractional Trade Routes - end
		}
	}
	return iTotalImports;
}


int CvPlayer::calculateTotalCityUnhappiness() const
{
	return algo::accumulate(cities() | transformed(CvCity::fn::unhappyLevel()), 0);
}


int CvPlayer::calculateTotalCityHealthiness() const
{
	return algo::accumulate(cities() | transformed(CvCity::fn::goodHealth()), 0);
}

int CvPlayer::calculateTotalCityUnhealthiness() const
{
	return algo::accumulate(cities() | transformed(CvCity::fn::badHealth()), 0);
}

int CvPlayer::calculateUnitSupply() const
{
	if (isAnarchy() || isNPC())
	{
		return 0;
	}
	int iPaidUnits;
	int iBaseSupplyCost;

	return calculateUnitSupply(iPaidUnits, iBaseSupplyCost);
}

int CvPlayer::calculateUnitSupply(int& iPaidUnits, int& iBaseSupplyCost) const
{
	iPaidUnits = std::max(0, getNumOutsideUnits() - GC.getDefineINT("INITIAL_FREE_OUTSIDE_UNITS"));

	iBaseSupplyCost = iPaidUnits * GC.getDefineINT("INITIAL_OUTSIDE_UNIT_GOLD_PERCENT");
	iBaseSupplyCost /= 100;

	int iSupply = iBaseSupplyCost;
	int iMod = getDistantUnitSupportCostModifier();

	if (iMod != 0)
	{
		iBaseSupplyCost = getModifiedIntValue(iSupply, iMod);
	}
	if (isNormalAI())
	{
		CvHandicapInfo* handicap = &(GC.getHandicapInfo(GC.getGame().getHandicapType()));
		if (handicap != NULL)
		{
			iMod +=
				(
					handicap->getAIUnitSupplyPercent() - 100
					+
					handicap->getAIPerEraModifier() * getCurrentEra()
				);
		}
	}
	if (iMod != 0)
	{
		iSupply = getModifiedIntValue(iSupply, iMod);
	}
	FASSERT_NOT_NEGATIVE(iSupply);
	return iSupply;
}


int64_t CvPlayer::calculatePreInflatedCosts() const
{
	if (isAnarchy())
	{
		return 0;
	}
	return (
		getTreasuryUpkeep()
		+ getTotalMaintenance()
		+ getCivicUpkeep()
		+ getFinalUnitUpkeep()
		+ calculateUnitSupply()
		+ getCorporateMaintenance()
	);
}

int CvPlayer::getInflationMod10000() const
{
	int iInflationPerTurnTimes10000 = 100 * getHurriedCount();

	iInflationPerTurnTimes10000 *= GC.getHandicapInfo(getHandicapType()).getInflationPercent();
	iInflationPerTurnTimes10000 /= 100;

	int iMod = (
		m_iInflationModifier
		+ getCivicInflation()
		+ getProjectInflation()
		+ getTechInflation()
		+ getBuildingInflation()
		- 100 * isRebel()
	);

	if (iMod != 0)
	{
		iInflationPerTurnTimes10000 = getModifiedIntValue(iInflationPerTurnTimes10000, iMod);
	}

	try
	{
		if (isNormalAI())
		{
			CvHandicapInfo * handicap = &(GC.getHandicapInfo(GC.getGame().getHandicapType()));
			if (handicap != NULL)
			iMod = (
				handicap->getAIInflationPercent() - 100
				+
				handicap->getAIPerEraModifier() * getCurrentEra()
			);
			if (iMod != 0)
			{
				iInflationPerTurnTimes10000 = getModifiedIntValue(iInflationPerTurnTimes10000, iMod);
			}
		}
	}
	catch (...) { // Exception on Load Save
	}
	return 10000 + iInflationPerTurnTimes10000;
}

int64_t CvPlayer::getInflationCost() const
{
	const int64_t iPreInflatedCost = calculatePreInflatedCosts();
	return isAnarchy() ? 0 : iPreInflatedCost * getInflationMod10000() / 10000 - iPreInflatedCost;
}

int64_t CvPlayer::getFinalExpense() const
{
	return isAnarchy() ? 0 : calculatePreInflatedCosts() * getInflationMod10000() / 10000;
}


void CvPlayer::cacheKeyFinanceNumbers()
{
	PROFILE_EXTRA_FUNC();
	m_iMinTaxIncome = std::max(0, getGoldPerTurn());
	m_iMaxTaxIncome = m_iMinTaxIncome;

	foreach_(CvCity* cityX, cities())
	{
		m_iMinTaxIncome += cityX->getCommerceRateAtSliderPercent(COMMERCE_GOLD, 0) / 100;
		m_iMaxTaxIncome += cityX->getCommerceRateAtSliderPercent(COMMERCE_GOLD, 100) / 100;
	}
}

int64_t CvPlayer::getMinTaxIncome() const
{
	return m_iMinTaxIncome;
}

int64_t CvPlayer::getMaxTaxIncome() const
{
	return m_iMaxTaxIncome;
}

short CvPlayer::getProfitMargin(int64_t &iNetExpenses, int iExtraExpense, int iExtraExpenseMod) const
{
	PROFILE_FUNC();

	if (isAnarchy() || isNPC())
	{
		return 100;
	}
	// Afforess - iExtraExpense lets us "extrapolate" our cost percents if we have extra future expenses
	// iExtraExpense should be between 0 (default) and some positive extra gold per turn cost to us
	iNetExpenses = getFinalExpense() + std::max(0, iExtraExpense);

	if (iExtraExpenseMod > 0)
	{
		iNetExpenses *= 100 + iExtraExpenseMod;
		iNetExpenses /= 100;
	}
	// Mainly gold per turn from trade.
	iNetExpenses += std::max(0, -getGoldPerTurn());

	const int64_t iMaxTaxIncome = getMaxTaxIncome();

	//FErrorMsg(CvString::format("iNetExpenses=%I64d - iMaxTaxIncome=%I64d - iMinTaxIncome=%I64d", iNetExpenses, iMaxTaxIncome, getMinTaxIncome()).c_str());

	// Toffer - Profit margin at 100% taxation
	if (iNetExpenses >= iMaxTaxIncome)
	{
		return 0;
	}
	return static_cast<short>(100 - 100 * iNetExpenses / std::max<int64_t>(1, iMaxTaxIncome));
}

short CvPlayer::getProfitMargin(int iExtraExpense, int iExtraExpenseMod) const
{
	int64_t iNetExpenses;
	return getProfitMargin(iNetExpenses, iExtraExpense, iExtraExpenseMod);
}

int64_t CvPlayer::calculateBaseNetGold() const
{
	return getCommerceRate(COMMERCE_GOLD) + getGoldPerTurn() - getFinalExpense();
}

int CvPlayer::calculateResearchModifier(TechTypes eTech) const
{
	PROFILE_EXTRA_FUNC();
	if (NO_TECH == eTech)
	{
		return 0;
	}
	int iModifier = 0;

	if (GC.getGame().isOption(GAMEOPTION_TECH_WIN_FOR_LOSING)
	&& (!isHumanPlayer() || !GC.getGame().isOption(GAMEOPTION_TECH_NO_HANDICAPS_FOR_HUMANS)))
	{
		iModifier += GET_TEAM(getTeam()).getWinForLosingResearchModifier();
	}

	if (GC.getGame().isOption(GAMEOPTION_TECH_DIFFUSION)
	&& (!isHumanPlayer() || !GC.getGame().isOption(GAMEOPTION_TECH_NO_HANDICAPS_FOR_HUMANS)))
	{
		double knownExp = 0.0;
		// Tech flows better through open borders
		int iTeams = 0;
		int iMetTeams = 0;
		int iTeamsHaveTech = 0;
		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				iTeams++;
				if (GET_TEAM(getTeam()).isHasMet((TeamTypes)iI))
				{
					iMetTeams++;
					if (GET_TEAM((TeamTypes)iI).isHasTech(eTech))
					{
						iTeamsHaveTech++;

						knownExp += 0.5;

						if (GET_TEAM(getTeam()).isOpenBorders((TeamTypes)iI) || GET_TEAM((TeamTypes)iI).isVassal(getTeam()))
						{
							knownExp += 1.5;
						}
						else if (GET_TEAM(getTeam()).isAtWar((TeamTypes)iI) || GET_TEAM(getTeam()).isVassal((TeamTypes)iI))
						{
							knownExp += 0.5;
						}
					}
				}
			}
		}

		if (knownExp > 0.0)
		{
			//If 1/3 the teams don't have the tech, diffusion is slow
			if (iTeamsHaveTech * 3 < iTeams)
			{
				knownExp /= 100;
			}
			//If 2/3 the teams do have the tech, diffusion is fast
			else if (iTeamsHaveTech * 3 > 2 * iTeams)
			{
				knownExp *= 3;
			}
		}

		//Scale diffusion based on the number of teams we know of
		knownExp *= iMetTeams;
		knownExp /= iTeams;

		int techDiffMod = GC.getTECH_DIFFUSION_KNOWN_TEAM_MODIFIER();
		if (knownExp > 0.0)
		{
			//ensure tech diffusion can not hurt research, only help
			int iTechDiffusion = std::max(0, techDiffMod - (int)(techDiffMod * pow(0.85, knownExp) + 0.5));
			iModifier += iTechDiffusion;
			logging::logMsg("C2C.log", "Tech Diffusion base amount for %S: %d\n", getCivilizationDescription(), iTechDiffusion);
		}

		// Tech flows downhill to those who are far behind
		int iTechScorePercent = GET_TEAM(getTeam()).getBestKnownTechScorePercent();
		int iWelfareThreshold = GC.getTECH_DIFFUSION_WELFARE_THRESHOLD();
		if( iTechScorePercent < iWelfareThreshold )
		{
			if( knownExp > 0.0 )
			{
				int iWelfareMod = GC.getTECH_DIFFUSION_WELFARE_MODIFIER();
				//ensure tech diffusion can not hurt research, only help
				int iWelfareTechDiffusion = std::max(0, (iWelfareMod - (int)(iWelfareMod * pow(0.98, iWelfareThreshold - iTechScorePercent) + 0.5)));
				int iBestScore = 0;
				int iOurScore = calculateScore(false);
				for (int iI = 0; iI < MAX_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive() && iI != getID())
					{
						int iScore = GET_PLAYER((PlayerTypes)iI).calculateScore(false);
						if (iScore > iBestScore)
						{
							iBestScore = iScore;
						}
					}
				}
				//If iOurScore > iBestScore we are the best team in the game
				if (iOurScore < iBestScore)
				{
					float fRatio = iBestScore / ((float)iOurScore);
					logging::logMsg("C2C.log", "Tech Welfare amount: %d, iOurScore: %d, iBestScore: %d, fRatio: %f, modified welfare amt: %d for civ: %S\n", iWelfareTechDiffusion, iOurScore, iBestScore, fRatio, ((int)(fRatio * iWelfareTechDiffusion)), getCivilizationDescription());
					iWelfareTechDiffusion = (int)(iWelfareTechDiffusion * fRatio);
					iModifier += iWelfareTechDiffusion;

				}
			}
		}
	}
	return iModifier;
}

int CvPlayer::calculateBaseNetResearch(TechTypes eTech) const
{
	if (eTech == NO_TECH)
	{
		eTech = getCurrentResearch();

		if (eTech == NO_TECH)
		{
			return GC.getDefineINT("BASE_RESEARCH_RATE") + getCommerceRate(COMMERCE_RESEARCH);
		}
	}
	int iCalcResearch =  (
		GC.getDefineINT("BASE_RESEARCH_RATE")
		+
		getModifiedIntValue(
			getCommerceRate(COMMERCE_RESEARCH),
			getNationalTechResearchModifier(eTech) + calculateResearchModifier(eTech)
		)
	);
	if (iCalcResearch < MIN_TOL_FALSE_RESEARCH)
		return MAX_RESEARCH_RATE_VALUE;
	return std::min(MAX_RESEARCH_RATE_VALUE,iCalcResearch);
}


int CvPlayer::calculateGoldRate() const
{
	if (isCommerceFlexible(COMMERCE_RESEARCH))
	{
		return static_cast<int>(calculateBaseNetGold());
	}
	return static_cast<int>(std::min<int64_t>(0, calculateBaseNetResearch() + calculateBaseNetGold()));
}


int CvPlayer::calculateResearchRate(TechTypes eTech) const
{
	if (isCommerceFlexible(COMMERCE_RESEARCH))
	{
		return calculateBaseNetResearch(eTech);
	}
	return static_cast<int>(std::max<int64_t>(1, calculateBaseNetResearch(eTech) + calculateBaseNetGold()));

}

int CvPlayer::calculateTotalCommerce() const
{
	PROFILE_EXTRA_FUNC();
	int64_t iTotalCommerce = calculateBaseNetGold() + calculateBaseNetResearch();

	for (int i = 0; i < NUM_COMMERCE_TYPES; ++i)
	{
		if (COMMERCE_GOLD != i && COMMERCE_RESEARCH != i)
		{
			iTotalCommerce += getCommerceRate((CommerceTypes)i);
		}
	}
	return static_cast<int>(std::min<int64_t>(MAX_COMMERCE_VALUE,iTotalCommerce));
}

bool CvPlayer::canEverResearch(TechTypes eTech) const
{
	PROFILE_EXTRA_FUNC();
	if (eTech < 0 || eTech >= GC.getNumTechInfos()) {
		return false; // ID de tech invalide
	}
	CvTechInfo& kTechInfo = GC.getTechInfo(eTech);
	if (kTechInfo.isDisable()
	|| GC.getCivilizationInfo(getCivilizationType()).isCivilizationDisableTechs(eTech)
	|| !GC.getGame().canEverResearch(eTech)
	|| kTechInfo.isGlobal() && (isNPC() || GC.getGame().countKnownTechNumTeams(eTech) > 0))
	{
		return false;
	}

	// Limited religions must not allow a player to hoard religious techs to deny the religions from being founded anywhere.
	// Religion techs are global and can thus only be invented once by one player in a game.
	if (GC.getGame().isOption(GAMEOPTION_RELIGION_LIMITED) && !canFoundReligion())
	{
		foreach_(const CvReligionInfo* info, GC.getReligionInfos())
		{
			if (info->getTechPrereq() == eTech)
			{
				return false;
			}
		}
	}
	return true;
}


bool CvPlayer::canResearch(const TechTypes eTech, const bool bRightNow, const bool bSpecialRequirements) const
{
	PROFILE_EXTRA_FUNC();
	if (GET_TEAM(getTeam()).isHasTech(eTech) || !canEverResearch(eTech))
	{
		return false;
	}

	if (bSpecialRequirements && !hasValidBuildings(eTech))
	{
		return false;
	}

	if (bRightNow)
	{
		bool bOk = true;
		foreach_(const TechTypes ePrereq, GC.getTechInfo(eTech).getPrereqOrTechs())
		{
			bOk = false;

			if (GET_TEAM(getTeam()).isHasTech(ePrereq))
			{
				bOk = true;
				break;
			}
		}
		if (!bOk)
		{
			return false;
		}

		foreach_(const TechTypes ePrereq, GC.getTechInfo(eTech).getPrereqAndTechs())
		{
			if (canEverResearch(ePrereq) && !GET_TEAM(getTeam()).isHasTech(ePrereq))
			{
				return false;
			}
		}
	}
	return true;
}


TechTypes CvPlayer::getCurrentResearch() const
{
	const CLLNode<TechTypes>* pResearchNode = headResearchQueueNode();

	if (pResearchNode)
	{
		return pResearchNode->m_data;
	}
	return NO_TECH;
}


bool CvPlayer::isCurrentResearchRepeat() const
{
	const TechTypes eCurrentResearch = getCurrentResearch();

	if (eCurrentResearch == NO_TECH)
	{
		return false;
	}
	return GC.getTechInfo(eCurrentResearch).isRepeat();
}


bool CvPlayer::isNoResearchAvailable() const
{
	PROFILE_EXTRA_FUNC();
	if (getCurrentResearch() != NO_TECH)
	{
		return false;
	}
	foreach_(const TechTypes eTechX, GET_TEAM(getTeam()).getAdjacentResearch())
	{
		if (canResearch(eTechX))
		{
			return false;
		}
	}
	return true;
}


int CvPlayer::getResearchTurnsLeft(TechTypes eTech, bool bOverflow) const
{
	return std::max(1, (getResearchTurnsLeftTimes100(eTech, bOverflow) + 99) / 100); // round up
}

int CvPlayer::getResearchTurnsLeftTimes100(TechTypes eTech, bool bOverflow) const
{
	PROFILE_EXTRA_FUNC();
	int iResearchRate = 0;
	int iOverflow = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getTeam())
		&& (iI == getID() || GET_PLAYER((PlayerTypes)iI).getCurrentResearch() == eTech))
		{
			iResearchRate += GET_PLAYER((PlayerTypes)iI).calculateResearchRate(eTech);
			iOverflow += getModifiedIntValue(GET_PLAYER((PlayerTypes)iI).getOverflowResearch(), calculateResearchModifier(eTech));
		}
	}

	// Mainly just so debug display shows sensible value
	int iResearchLeft = GET_TEAM(getTeam()).getResearchLeft(eTech);

	if (bOverflow)
	{
		iResearchLeft -= iOverflow;
	}

	const int iMultiplier = iResearchLeft < MAX_INT / 100 ? 1 : 100;

	if (iMultiplier == 1)
	{
		iResearchLeft *= 100;
	}

	if (iResearchRate == 0)
	{
		return iResearchLeft;
	}

	int iTurnsLeft = iResearchLeft / iResearchRate;

	if (iTurnsLeft * iResearchRate < iResearchLeft)
	{
		++iTurnsLeft;
	}

	if (iTurnsLeft < MAX_INT/iMultiplier)
	{
		iTurnsLeft *= iMultiplier;
	}
	else iTurnsLeft = MAX_INT;

	return std::max(1, iTurnsLeft);
}


bool CvPlayer::isCivic(CivicTypes eCivic) const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		if (getCivics((CivicOptionTypes)iI) == eCivic)
		{
			return true;
		}
	}

	return false;
}


bool CvPlayer::canDoCivics(CivicTypes eCivic) const
{
	PROFILE_FUNC();

	if (eCivic == NO_CIVIC)
	{
		return true;
	}

	if (GC.getGame().isForceCivicOption((CivicOptionTypes)GC.getCivicInfo(eCivic).getCivicOptionType()))
	{
		return GC.getGame().isForceCivic(eCivic);
	}

	if (!isNPC()
	&& GC.getCivicInfo(eCivic).getCityLimit(getID()) > 0
	&& GC.getCivicInfo(eCivic).getCityOverLimitUnhappy() == 0
	&& GC.getCivicInfo(eCivic).getCityLimit(getID()) < getNumCities()
	|| !isHasCivicOption((CivicOptionTypes)GC.getCivicInfo(eCivic).getCivicOptionType())
	&& !GET_TEAM(getTeam()).isHasTech(GC.getCivicInfo(eCivic).getTechPrereq()))
	{
		return false;
	}
	return true;
}


bool CvPlayer::canRevolution(CivicTypes* paeNewCivics) const
{
	PROFILE_EXTRA_FUNC();
	if (isAnarchy() || isNPC())
	{
		return false;
	}

	if (getRevolutionTimer() > 0)
	{
		return false;
	}

	if (paeNewCivics == NULL)
	{
		// XXX is this necessary?
		for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
		{
			if (canDoCivics((CivicTypes)iI)
			&& getCivics((CivicOptionTypes)GC.getCivicInfo((CivicTypes)iI).getCivicOptionType()) != iI)
			{
				return true;
			}
		}
	}
	else
	{
		for (int iI = 0; iI < GC.getNumCivicOptionInfos(); ++iI)
		{
			if (GC.getGame().isForceCivicOption((CivicOptionTypes)iI)
			&& !GC.getGame().isForceCivic(paeNewCivics[iI]))
			{
				return false;
			}

			if (getCivics((CivicOptionTypes)iI) != paeNewCivics[iI])
			{
				return true;
			}
		}
	}
	return false;
}


void CvPlayer::revolution(CivicTypes* paeNewCivics, bool bForce)
{
	PROFILE_EXTRA_FUNC();
	if (!bForce && !canRevolution(paeNewCivics))
	{
		return;
	}
	bst::scoped_array<CivicTypes> paeOldCivics(new CivicTypes[GC.getNumCivicOptionInfos()]);

	for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		paeOldCivics[iI] = getCivics((CivicOptionTypes)iI);
	}

	int iAnarchyLength = getCivicAnarchyLength(paeNewCivics);

	if (iAnarchyLength > 0)
	{
		changeAnarchyTurns(iAnarchyLength);
	}
	int iCivicChanges = 0;

	for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		if (paeOldCivics[iI] != paeNewCivics[iI])
		{
			iCivicChanges++;
			if (gPlayerLogLevel > 0)
			{
				logBBAI
				(
					"Player %S switches from %S to %S",
					getCivilizationDescription(0),
					GC.getCivicInfo(paeOldCivics[iI]).getDescription(),
					GC.getCivicInfo(paeNewCivics[iI]).getDescription()
				);
			}

			setCivics(((CivicOptionTypes)iI), paeNewCivics[iI]);

			civcSwitchInstance	switchInstance;

			switchInstance.iTurn = GC.getGame().getGameTurn();
			switchInstance.eFromCivic = paeOldCivics[iI];
			switchInstance.eToCivic = paeNewCivics[iI];
			switchInstance.bNoAnarchy = (iAnarchyLength == 0);
			m_civicSwitchHistory.push_back(switchInstance);
		}
	}

	NoteCivicsSwitched(iCivicChanges);

	setRevolutionTimer(std::max(1, ((100 + getAnarchyModifier()) * GC.getDefineINT("MIN_REVOLUTION_TURNS")) / 100) + iAnarchyLength);

	if (getID() == GC.getGame().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(Popup_DIRTY_BIT, true); // to force an update of the civic chooser popup
	}
}


int CvPlayer::getCivicPercentAnger(CivicTypes eCivic) const
{
	if (getCivics((CivicOptionTypes)GC.getCivicInfo(eCivic).getCivicOptionType()) == eCivic)
	{
		// the divisor is 1000 so 1 percent would be represented by 10.
		return GC.getCivicInfo(eCivic).getCivicPercentAnger() * 10;
	}
	return 0;
}


bool CvPlayer::canDoReligion(ReligionTypes eReligion) const
{
	return GET_TEAM(getTeam()).getHasReligionCount(eReligion) != 0;
}


bool CvPlayer::canChangeReligion() const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if (canConvert((ReligionTypes)iI))
		{
			return true;
		}
	}
	return false;
}


bool CvPlayer::canConvert(ReligionTypes eReligion) const
{
	if (isNPC())
	{
		return false;
	}

	if (isAnarchy())
	{
		return false;
	}

	if (getConversionTimer() > 0)
	{
		return false;
	}

	if (!isStateReligion())
	{
		return false;
	}

	if (getLastStateReligion() == eReligion)
	{
		return false;
	}

	if (eReligion != NO_RELIGION)
	{
		if (!canDoReligion(eReligion))
		{
			return false;
		}
	}

	return true;
}


void CvPlayer::convert(ReligionTypes eReligion)
{
	if (!canConvert(eReligion))
	{
		return;
	}

	const int iAnarchyLength = getReligionAnarchyLength();

	changeAnarchyTurns(iAnarchyLength);

	setLastStateReligion(eReligion);

	//TB set religion - this will work even after Ideas project is implemented which will add the religion unitcombat to the unit trained in a city that has a primary faith.  If a Religion defining unitcombat is on the unit, giving the unit the state religion will be overridden.  (The discord between state religion and unit religion may also be programmed to later impact the unit morale when morale is implemented)
	algo::for_each(units(), CvUnit::fn::defineReligion());

	setConversionTimer(std::max(1, ((100 + getAnarchyModifier()) * GC.getDefineINT("MIN_CONVERSION_TURNS")) / 100) + iAnarchyLength);
}


bool CvPlayer::hasHolyCity(ReligionTypes eReligion) const
{
	FAssertMsg(eReligion != NO_RELIGION, "Religion is not assigned a valid value");

	const CvCity* pHolyCity = GC.getGame().getHolyCity(eReligion);

	if (pHolyCity)
	{
		return (pHolyCity->getOwner() == getID());
	}

	return false;
}


bool CvPlayer::hasHolyCity() const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if (hasHolyCity((ReligionTypes)iI))
		{
			return true;
		}
	}

	return false;
}


bool CvPlayer::hasStateReligionHolyCity() const
{
	const ReligionTypes eStateReligion = getStateReligion();
	return eStateReligion != NO_RELIGION && hasHolyCity(eStateReligion);
}


bool CvPlayer::hasStateReligionShrine() const
{
	const ReligionTypes eStateReligion = getStateReligion();

	if (eStateReligion == NO_RELIGION)
	{
		return false;
	}

	const CvCity* pHolyCity = GC.getGame().getHolyCity(eStateReligion);

	if (pHolyCity && pHolyCity->hasShrine(eStateReligion))
	{
		return pHolyCity->getOwner() == getID();
	}
	return false;
}


int CvPlayer::countHolyCities() const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if (hasHolyCity((ReligionTypes)iI))
		{
			iCount++;
		}
	}

	return iCount;
}


void CvPlayer::foundReligion(ReligionTypes eReligion, ReligionTypes eSlotReligion, bool bAward)
{
	PROFILE_EXTRA_FUNC();
	//TB Prophet Mod
	if(GC.getGame().isOption(GAMEOPTION_RELIGION_DIVINE_PROPHETS))
	{
		return;
	}

	// Limited religion GO
	FAssert(!isNPC());
	if (NO_RELIGION == eReligion || !canFoundReligion())
	{
		FAssert(NO_RELIGION != eReligion);
		FAssert(canFoundReligion());
		return;
	}
	FAssert(!GC.getGame().isReligionSlotTaken(eSlotReligion));

	// Clear queued religious techs for human player when founding a religion
	// Religious techs are not researchable after founding a religion under "Limited Religions" rules
	// AI always clear their entire tech queue when a religion is founded in CvTeam::setHasTech
	if (GC.getGame().isOption(GAMEOPTION_RELIGION_LIMITED))
	{
		foreach_(const CvReligionInfo* info, GC.getReligionInfos())
		{
			if (isResearchingTech(info->getTechPrereq()))
			{
				popResearch(info->getTechPrereq());
			}
			const std::vector<TechTypes>& adjacentResearch = GET_TEAM(getTeam()).getAdjacentResearch();

			if (find(adjacentResearch.begin(), adjacentResearch.end(), info->getTechPrereq()) != adjacentResearch.end())
			{
				GET_TEAM(getTeam()).setAdjacentResearch(info->getTechPrereq(), false);
			}
		}
	}
	if (GC.getGame().isReligionFounded(eReligion))
	{
		if (isHumanPlayer())
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_FOUND_RELIGION, eSlotReligion);
			if (pInfo)
			{
				gDLL->getInterfaceIFace()->addPopup(pInfo, getID());
			}
		}
		else
		{
			foundReligion(AI_chooseReligion(), eSlotReligion, bAward);
		}
		return;
	}

	// Limited religion GO
	const TechTypes eTechReq = GC.getReligionInfo(eSlotReligion).getTechPrereq();
	const bool bStarting = eTechReq == NO_TECH || GC.getGame().isGameStart();

	// Find founding city
	const int iRand = GC.getDefineINT("FOUND_RELIGION_CITY_RAND");
	int iBestValue = -1;
	CvCity* pBestCity = NULL;

	foreach_(CvCity* pLoopCity, cities())
	{
		if (!bStarting || !(pLoopCity->isHolyCity()))
		{
			int iValue = 10 + pLoopCity->getPopulation();

			if (iRand > 0)
			{
				iValue += 1 + GC.getGame().getSorenRandNum(iRand, "Found Religion");
			}

			iValue /= (pLoopCity->getReligionCount() + 1);

			if (pLoopCity->isCapital())
			{
				iValue /= 8;
			}

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestCity = pLoopCity;
			}
		}
	}

	if (pBestCity)
	{
		// Limited religion GO
		GC.getGame().setReligionSlotTaken(eSlotReligion, true);
		GC.getGame().setTechCanFoundReligion(eTechReq, true);

		// Found religion
		GC.getGame().setHolyCity(eReligion, pBestCity, true);

		if (bAward && GC.getReligionInfo(eSlotReligion).getNumFreeUnits() > 0)
		{
			const UnitTypes eFreeUnit = (UnitTypes)GC.getReligionInfo(eReligion).getFreeUnit();

			if (eFreeUnit != NO_UNIT)
			{
				for (int i = 0; i < GC.getReligionInfo(eSlotReligion).getNumFreeUnits(); ++i)
				{
					initUnit(eFreeUnit, pBestCity->getX(), pBestCity->getY(), NO_UNITAI, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
				}
			}
		}
	}
}


bool CvPlayer::hasHeadquarters(CorporationTypes eCorporation) const
{
	const CvCity* pHeadquarters = GC.getGame().getHeadquarters(eCorporation);

	FAssert(eCorporation != NO_CORPORATION);

	if (pHeadquarters)
	{
		return (pHeadquarters->getOwner() == getID());
	}

	return false;
}


int CvPlayer::countHeadquarters() const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (hasHeadquarters((CorporationTypes)iI))
		{
			iCount++;
		}
	}

	return iCount;
}


int CvPlayer::countCorporations(CorporationTypes eCorporation) const
{
	return algo::count_if(cities(), CvCity::fn::isHasCorporation(eCorporation));
}


void CvPlayer::foundCorporation(CorporationTypes eCorporation)
{
	PROFILE_EXTRA_FUNC();
	if (GC.getGame().isCorporationFounded(eCorporation))
	{
		return;
	}

	const bool bStarting = GC.getCorporationInfo(eCorporation).getTechPrereq() == NO_TECH || GC.getTechInfo(GC.getCorporationInfo(eCorporation).getTechPrereq()).getEra() < GC.getGame().getStartEra();

	int iBestValue = 0;
	CvCity* pBestCity = NULL;

	foreach_(CvCity* pLoopCity, cities())
	{
		if (!bStarting || !(pLoopCity->isHeadquarters()))
		{
			int iValue = 10;
			iValue += pLoopCity->getPopulation();

			foreach_(const BonusTypes eBonus, GC.getCorporationInfo(eCorporation).getPrereqBonuses())
			{
				iValue += 10 * pLoopCity->getNumBonuses(eBonus);
			}

			iValue += GC.getGame().getSorenRandNum(GC.getDefineINT("FOUND_CORPORATION_CITY_RAND"), "Found Corporation");

			iValue /= (pLoopCity->getCorporationCount() + 1);

			iValue = std::max(1, iValue);

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestCity = pLoopCity;
			}
		}
	}

	if (pBestCity)
	{
		pBestCity->setHeadquarters(eCorporation);
	}
}


int CvPlayer::getCivicAnarchyLength(CivicTypes* paeNewCivics) const
{
	PROFILE_EXTRA_FUNC();
	if (isGoldenAge())
	{
		return 0;
	}
	const int iMaxAnarchy = getMaxAnarchyTurns();

	if (iMaxAnarchy < 1)
	{
		return 0;
	}
	int iTotalAnarchyLength = 0;
	int iCount = 0;
	for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		if (paeNewCivics[iI] != getCivics((CivicOptionTypes)iI) && !GC.getCivicInfo(paeNewCivics[iI]).isPolicy())
		{
			const int iAnarchyLength = GC.getCivicInfo(paeNewCivics[iI]).getAnarchyLength();

			if (iAnarchyLength > 0)
			{
				iCount += 1;
				iTotalAnarchyLength += iAnarchyLength * 100;
			}
		}
	}

	if (iCount <= 0)
	{
		return 0;
	}
	if (iCount > 1)
	{ // Quantity discount when changing more than 1 civic at a time.
		iTotalAnarchyLength -= (iTotalAnarchyLength * (iCount * GC.getDefineINT("CIVIC_ANARCHY_QTY_DISCOUNT"))) / 100;
	} // CIVIC_ANARCHY_QTY_DISCOUNT: A global define that should be in the range 0-100.

	iTotalAnarchyLength *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
	iTotalAnarchyLength /= 100;

	iTotalAnarchyLength += getNumCities() * GC.getWorldInfo(GC.getMap().getWorldSize()).getNumCitiesAnarchyPercent();

	iTotalAnarchyLength *= getAnarchyModifier() + 100;
	iTotalAnarchyLength /= 100;

	iTotalAnarchyLength *= getCivicAnarchyModifier() + 100;
	iTotalAnarchyLength /= 100;

	iTotalAnarchyLength *= GC.getEraInfo(getCurrentEra()).getAnarchyPercent();
	iTotalAnarchyLength /= 100;

	if (isRebel())
	{
		iTotalAnarchyLength /= 2;
	}
	iTotalAnarchyLength /= 100;

	return std::max(1, range(iTotalAnarchyLength , getMinAnarchyTurns(), iMaxAnarchy));
}


int CvPlayer::getReligionAnarchyLength() const
{
	if (isGoldenAge())
	{
		return 0;
	}
	const int iMaxAnarchy = getMaxAnarchyTurns();

	if (iMaxAnarchy < 1)
	{
		return 0;
	}
	int iAnarchyLength = GC.getDefineINT("BASE_RELIGION_ANARCHY_LENGTH");

	iAnarchyLength *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
	iAnarchyLength /= 100;

	iAnarchyLength += (getNumCities() * GC.getWorldInfo(GC.getMap().getWorldSize()).getNumCitiesAnarchyPercent() / 100);

	iAnarchyLength = ((iAnarchyLength * std::max(0, (getAnarchyModifier() + 100))) / 100);

	if (iAnarchyLength == 0)
	{
		return 0;
	}

	iAnarchyLength *= GC.getEraInfo(getCurrentEra()).getAnarchyPercent();
	iAnarchyLength /= 100;

	//TB Traits begin
	if (getReligiousAnarchyModifier() !=0)
	{
		iAnarchyLength += iAnarchyLength * getReligiousAnarchyModifier() / 100;
	}
	//TB Traits end

	return range(iAnarchyLength, getMinAnarchyTurns(), iMaxAnarchy);
}



int CvPlayer::unitsRequiredForGoldenAge() const
{
	return (GC.getDefineINT("BASE_GOLDEN_AGE_UNITS") + (getNumUnitGoldenAges() * GC.getDefineINT("GOLDEN_AGE_UNITS_MULTIPLIER")));
}


int CvPlayer::unitsGoldenAgeCapable() const
{
	return algo::count_if(units(), CvUnit::fn::isGoldenAge());
}


int CvPlayer::unitsGoldenAgeReady() const
{
	PROFILE_FUNC();

	bool* pabUnitUsed;
	int iCount;
	int iI;

	pabUnitUsed = new bool[GC.getNumUnitInfos()];

	for (iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		pabUnitUsed[iI] = false;
	}

	iCount = 0;

	foreach_(const CvUnit* pLoopUnit, units())
	{
		if (!(pabUnitUsed[pLoopUnit->getUnitType()]))
		{
			if (pLoopUnit->isGoldenAge())
			{
				pabUnitUsed[pLoopUnit->getUnitType()] = true;
				iCount++;
			}
		}
	}

	SAFE_DELETE_ARRAY(pabUnitUsed);

	return iCount;
}


void CvPlayer::killGoldenAgeUnits(CvUnit* pUnitAlive)
{
	PROFILE_EXTRA_FUNC();
	CvUnit* pBestUnit;
	bool* pabUnitUsed;
	int iUnitsRequired;
	int iValue;
	int iBestValue;
	int iI;

	pabUnitUsed = new bool[GC.getNumUnitInfos()];

	for (iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		pabUnitUsed[iI] = false;
	}

	iUnitsRequired = unitsRequiredForGoldenAge();

	if (pUnitAlive)
	{
		pabUnitUsed[pUnitAlive->getUnitType()] = true;
		iUnitsRequired--;
	}

	for (iI = 0; iI < iUnitsRequired; iI++)
	{
		iBestValue = 0;
		pBestUnit = NULL;

		foreach_(CvUnit* pLoopUnit, units())
		{
			if (pLoopUnit->isGoldenAge())
			{
				if (!(pabUnitUsed[pLoopUnit->getUnitType()]))
				{
					iValue = 10000;

					iValue /= (plotDistance(pLoopUnit->getX(), pLoopUnit->getY(), pUnitAlive->getX(), pUnitAlive->getY()) + 1);

					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						pBestUnit = pLoopUnit;
					}
				}
			}
		}

		FAssert(pBestUnit);
		if (pBestUnit)
		{
			pabUnitUsed[pBestUnit->getUnitType()] = true;

			pBestUnit->kill(true);

			//play animations
			if (pBestUnit->plot()->isActiveVisible(false))
			{
				//kill removes bestUnit from any groups
				pBestUnit->getGroup()->pushMission(MISSION_GOLDEN_AGE, 0);
			}
		}
	}

	SAFE_DELETE_ARRAY(pabUnitUsed);
}

int CvPlayer::greatPeopleThresholdMilitary() const
{
	int64_t iThreshold = GC.getDefineINT("GREAT_GENERALS_THRESHOLD") * GC.getEraInfo(GC.getGame().getStartEra()).getGreatPeoplePercent();

	iThreshold = getModifiedIntValue64(iThreshold, getGreatGeneralsThresholdModifier());
	// Weaker than standard scaling.
	iThreshold = iThreshold * (GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent()+50) / 150;

	iThreshold /= 100;

	if (iThreshold >= MAX_INT)
	{
		return MAX_INT;
	}
	return std::max(1, static_cast<int>(iThreshold));
}

int CvPlayer::greatPeopleThresholdNonMilitary() const
{
	int64_t iThreshold = GC.getDefineINT("GREAT_PEOPLE_THRESHOLD") * GC.getEraInfo(GC.getGame().getStartEra()).getGreatPeoplePercent();

	iThreshold = getModifiedIntValue64(iThreshold, getGreatPeopleThresholdModifier());

	iThreshold *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
	iThreshold /= 10000;

	if (iThreshold >= MAX_INT)
	{
		return MAX_INT;
	}
	return std::max(1, static_cast<int>(iThreshold));
}

int CvPlayer::specialistYield(SpecialistTypes eSpecialist, YieldTypes eYield) const
{
	return (GC.getSpecialistInfo(eSpecialist).getYieldChange(eYield) + getExtraSpecialistYield(eSpecialist, eYield) + getSpecialistExtraYield(eYield) + (getSpecialistYieldPercentChanges(eSpecialist, eYield) / 100));
}


int CvPlayer::specialistCommerce(SpecialistTypes eSpecialist, CommerceTypes eCommerce) const
{
	return specialistCommerceTimes100(eSpecialist, eCommerce)/100;
}

int CvPlayer::specialistCommerceTimes100(SpecialistTypes eSpecialist, CommerceTypes eCommerce) const
{
	return
	(
		100 * GC.getSpecialistInfo(eSpecialist).getCommerceChange(eCommerce)
		+
		100 * getExtraSpecialistCommerce(eSpecialist, eCommerce)
		+
		100 * getSpecialistExtraCommerce(eCommerce)
		+
		getSpecialistCommercePercentChanges(eSpecialist, eCommerce)
	);
}

CvPlot* CvPlayer::getStartingPlot() const
{
	return m_startingCoords[CURRENT_MAP].plot();
}


void CvPlayer::setStartingPlot(CvPlot* newPlot, const bool bUpdateStartDist)
{
	PROFILE_EXTRA_FUNC();
	CvPlot* oldPlot = getStartingPlot();

	if (oldPlot != newPlot)
	{
		if (oldPlot)
		{
			oldPlot->area()->changeNumStartingPlots(-1);

			bool bValid = true;
			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (iI != getID() && GET_PLAYER((PlayerTypes)iI).getStartingPlot() == oldPlot)
				{
					bValid = false;
					break;
				}
			}
			if (bValid) oldPlot->setStartingPlot(false);
		}

		if (newPlot == NULL)
		{
			m_startingCoords[CURRENT_MAP] = XYCoords();

			if (bUpdateStartDist && oldPlot)
			{
				GC.getMap().updateMinOriginalStartDist(oldPlot->area());
			}
		}
		else
		{
			newPlot->setStartingPlot(true);

			m_startingCoords[CURRENT_MAP] = XYCoords(*newPlot);

			newPlot->area()->changeNumStartingPlots(1);

			if (bUpdateStartDist)
			{
				GC.getMap().updateMinOriginalStartDist(newPlot->area());
			}
		}
	}
}


int CvPlayer::getTotalPopulation() const
{
	return m_iTotalPopulation;
}

void CvPlayer::changeTotalPopulation(int iChange)
{
	m_iTotalPopulation += iChange;
	FAssertMsg(m_iTotalPopulation >= 0, "This shouldn't go negative!");

	changeAssets(iChange*10);
	changePower(iChange); // Should sync up with the total power value for this player from all CvArea.
	changePopScore(iChange);
}

int CvPlayer::getAveragePopulation() const
{
	return 0 == getNumCities() ? 0 : getTotalPopulation() / getNumCities();
}


int64_t CvPlayer::getRealPopulation() const
{
	return algo::accumulate(cities() | transformed(CvCity::fn::getRealPopulation()), (int64_t)0);
}


int CvPlayer::getTotalLand() const
{
	return m_iTotalLand;
}

void CvPlayer::changeTotalLand(int iChange)
{
	m_iTotalLand += iChange;
	FASSERT_NOT_NEGATIVE(getTotalLand());
}

int CvPlayer::getTotalLandScored() const
{
	return m_iTotalLandScored;
}

void CvPlayer::changeTotalLandScored(int iChange)
{
	if (iChange != 0)
	{
		changeAssets(iChange*10);
		changeLandScore(iChange);

		m_iTotalLandScored += iChange;
		FASSERT_NOT_NEGATIVE(getTotalLandScored());
	}
}


int64_t CvPlayer::getGold() const
{
	return m_iGold;
}

void CvPlayer::setGold(int64_t iNewValue)
{
	if (getGold() != iNewValue)
	{
		m_iGold = iNewValue;

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(MiscButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

void CvPlayer::changeGold(int64_t iChange)
{
	setGold(std::min(getGold() + iChange,MAX_GOLD_VALUE));
}

int CvPlayer::getGoldPerTurn() const
{
	return m_iGoldPerTurn;
}


int CvPlayer::getAdvancedStartPoints() const
{
	return m_iAdvancedStartPoints;
}


void CvPlayer::setAdvancedStartPoints(int iNewValue)
{
	if (getAdvancedStartPoints() != iNewValue)
	{
		m_iAdvancedStartPoints = iNewValue;

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(MiscButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

void CvPlayer::changeAdvancedStartPoints(int iChange)
{
	setAdvancedStartPoints(getAdvancedStartPoints() + iChange);
}


int CvPlayer::getGoldenAgeTurns() const
{
	return m_iGoldenAgeTurns;
}


bool CvPlayer::isGoldenAge() const
{
	return m_iGoldenAgeTurns > 0;
}

void CvPlayer::changeGoldenAgeTurns(int iChange)
{
	PROFILE_EXTRA_FUNC();
	if (iChange != 0)
	{
		const bool bWasGoldenAge = m_iGoldenAgeTurns > 0;

		m_iGoldenAgeTurns += iChange;
		FASSERT_NOT_NEGATIVE(m_iGoldenAgeTurns);

		if (bWasGoldenAge != isGoldenAge())
		{
			if (!bWasGoldenAge)
			{
				changeAnarchyTurns(-getAnarchyTurns());
				((CvPlayerAI*)this)->AI_startGoldenAge();
			}
			updateYield();

			if (bWasGoldenAge)
			{
				CvEventReporter::getInstance().endGoldenAge(getID());
			}
			else
			{
				const CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_GOLDEN_AGE_BEGINS", getNameKey());
				GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szBuffer, -1, -1, GC.getCOLOR_HIGHLIGHT_TEXT());

				CvEventReporter::getInstance().goldenAge(getID());
			}

			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (GET_TEAM(getTeam()).isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
					{
						if (bWasGoldenAge)
						{
							AddDLLMessage(
								(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(),
								gDLL->getText("TXT_KEY_MISC_PLAYER_GOLDEN_AGE_ENDED", getNameKey()),
								"AS2D_GOLDAGEEND", MESSAGE_TYPE_MINOR_EVENT, NULL, GC.getCOLOR_HIGHLIGHT_TEXT()
							);
						}
						else
						{
							AddDLLMessage(
								(PlayerTypes)iI, ((PlayerTypes)iI == getID()), GC.getEVENT_MESSAGE_TIME(),
								gDLL->getText("TXT_KEY_MISC_PLAYER_GOLDEN_AGE_HAS_BEGUN", getNameKey()),
								"AS2D_GOLDAGESTART", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_HIGHLIGHT_TEXT()
							);
						}
					}
				}
			}
		}

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

int CvPlayer::getGoldenAgeLength() const
{
	return std::max(1, getModifiedIntValue(GC.getGame().goldenAgeLength100(), getGoldenAgeModifier()) / 100);
}

int CvPlayer::getNumUnitGoldenAges() const
{
	return m_iNumUnitGoldenAges;
}


void CvPlayer::changeNumUnitGoldenAges(int iChange)
{
	m_iNumUnitGoldenAges += iChange;
	FASSERT_NOT_NEGATIVE(getNumUnitGoldenAges());
}


int CvPlayer::getStrikeTurns() const
{
	return m_iStrikeTurns;
}


void CvPlayer::changeStrikeTurns(int iChange)
{
	m_iStrikeTurns += iChange;
	FASSERT_NOT_NEGATIVE(getStrikeTurns());
}


int CvPlayer::getAnarchyTurns() const
{
	return m_iAnarchyTurns;
}


bool CvPlayer::isAnarchy() const
{
	return (getAnarchyTurns() > 0);
}


void CvPlayer::changeAnarchyTurns(int iChange, bool bHideMessages)
{
	if (iChange != 0)
	{
		const bool bOldAnarchy = isAnarchy();

		m_iAnarchyTurns += iChange;
		FASSERT_NOT_NEGATIVE(getAnarchyTurns());

		if (bOldAnarchy != isAnarchy())
		{
			setCommerceDirty();
			updateTradeRoutes();
			updateCorporation();

			AI_makeAssignWorkDirty();

			if (isAnarchy() && !bHideMessages)
			{
				AddDLLMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_MISC_REVOLUTION_HAS_BEGUN").GetCString(), "AS2D_REVOLTSTART", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
			}
			else if (!bHideMessages)
			{
				AddDLLMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_MISC_REVOLUTION_OVER").GetCString(), "AS2D_REVOLTEND", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
			}

			if (getID() == GC.getGame().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(MiscButtons_DIRTY_BIT, true);
			}

			if (getTeam() == GC.getGame().getActiveTeam())
			{
				gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
			}
		}

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getMaxAnarchyTurns() const
{
	return m_iMaxAnarchyTurns;
}


void CvPlayer::updateMaxAnarchyTurns()
{
	PROFILE_EXTRA_FUNC();
	int iBestValue = GC.getDefineINT("MAX_ANARCHY_TURNS");

	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (iBestValue == 0)
		{
			break;
		}
		if (hasTrait((TraitTypes)iI))
		{
			const int iMaxAnarchy = GC.getTraitInfo((TraitTypes)iI).getMaxAnarchy();

			if (iMaxAnarchy > -1 && iMaxAnarchy < iBestValue)
			{
				iBestValue = iMaxAnarchy;
			}
		}
	}

	m_iMaxAnarchyTurns = iBestValue;
	FASSERT_NOT_NEGATIVE(getMaxAnarchyTurns());
}

int CvPlayer::getMinAnarchyTurns() const
{
	return m_iMinAnarchyTurns;
}


void CvPlayer::updateMinAnarchyTurns()
{
	PROFILE_EXTRA_FUNC();
	int iWorstValue = 0;

	FAssertMsg((GC.getNumTraitInfos() > 0), "GC.getNumTraitInfos() is less than or equal to zero but is expected to be larger than zero in CvPlayer::updateMaxAnarchyTurns");
	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			if (GC.getTraitInfo((TraitTypes)iI).getMinAnarchy() > 0)
			{
				if (GC.getTraitInfo((TraitTypes)iI).getMinAnarchy() > iWorstValue)
				{
					iWorstValue = GC.getTraitInfo((TraitTypes)iI).getMinAnarchy();
				}
			}
		}
	}

	if (getMaxAnarchyTurns() < iWorstValue)
	{
		iWorstValue = getMaxAnarchyTurns();
	}
	m_iMinAnarchyTurns = iWorstValue;
	FASSERT_NOT_NEGATIVE(getMinAnarchyTurns());
}


int CvPlayer::getAnarchyModifier() const
{
	return m_iAnarchyModifier;
}


void CvPlayer::changeAnarchyModifier(int iChange)
{
	if (0 != iChange)
	{
		m_iAnarchyModifier += iChange;

		setRevolutionTimer(std::max(0, ((100 + iChange) * getRevolutionTimer()) / 100));
		setConversionTimer(std::max(0, ((100 + iChange) * getConversionTimer()) / 100));
	}
}


int CvPlayer::getGoldenAgeModifier() const
{
	return m_iGoldenAgeModifier;
}


void CvPlayer::changeGoldenAgeModifier(int iChange)
{
	m_iGoldenAgeModifier += iChange;
}


int CvPlayer::getHurryModifier() const
{
	return m_iGlobalHurryModifier;
}


void CvPlayer::changeHurryModifier(int iChange)
{
	m_iGlobalHurryModifier += iChange;
}


int CvPlayer::getGreatPeopleCreated() const
{
	return m_iGreatPeopleCreated;
}


void CvPlayer::incrementGreatPeopleCreated()
{
	m_iGreatPeopleCreated++;
}

int CvPlayer::getGreatGeneralsCreated() const
{
	return m_iGreatGeneralsCreated;
}

void CvPlayer::incrementGreatGeneralsCreated()
{
	m_iGreatGeneralsCreated++;
}

int CvPlayer::getGreatPeopleThresholdModifier() const
{
	return m_iGreatPeopleThresholdModifier;
}


void CvPlayer::changeGreatPeopleThresholdModifier(int iChange)
{
	m_iGreatPeopleThresholdModifier += iChange;
}


int CvPlayer::getGreatGeneralsThresholdModifier() const
{
	return m_iGreatGeneralsThresholdModifier;
}


void CvPlayer::changeGreatGeneralsThresholdModifier(int iChange)
{
	m_iGreatGeneralsThresholdModifier += iChange;
}


int CvPlayer::getGreatPeopleRateModifier() const
{
	return m_iGreatPeopleRateModifier;
}


void CvPlayer::changeGreatPeopleRateModifier(int iChange)
{
	m_iGreatPeopleRateModifier += iChange;
}


int CvPlayer::getGreatGeneralRateModifier() const
{
	return m_iGreatGeneralRateModifier;
}


void CvPlayer::changeGreatGeneralRateModifier(int iChange)
{
	m_iGreatGeneralRateModifier += iChange;
}


int CvPlayer::getDomesticGreatGeneralRateModifier() const
{
	return (GC.getDefineINT("COMBAT_EXPERIENCE_IN_BORDERS_PERCENT") + m_iDomesticGreatGeneralRateModifier);
}


void CvPlayer::changeDomesticGreatGeneralRateModifier(int iChange)
{
	m_iDomesticGreatGeneralRateModifier += iChange;
}


int CvPlayer::getStateReligionGreatPeopleRateModifier() const
{
	return m_iStateReligionGreatPeopleRateModifier;
}


void CvPlayer::changeStateReligionGreatPeopleRateModifier(int iChange)
{
	m_iStateReligionGreatPeopleRateModifier += iChange;
}


int CvPlayer::getMaxGlobalBuildingProductionModifier() const
{
	return m_iMaxGlobalBuildingProductionModifier;
}


void CvPlayer::changeMaxGlobalBuildingProductionModifier(int iChange)
{
	m_iMaxGlobalBuildingProductionModifier += iChange;
}


int CvPlayer::getMaxTeamBuildingProductionModifier() const
{
	return m_iMaxTeamBuildingProductionModifier;
}


void CvPlayer::changeMaxTeamBuildingProductionModifier(int iChange)
{
	m_iMaxTeamBuildingProductionModifier += iChange;
}


int CvPlayer::getMaxPlayerBuildingProductionModifier() const
{
	return m_iMaxPlayerBuildingProductionModifier;
}


void CvPlayer::changeMaxPlayerBuildingProductionModifier(int iChange)
{
	m_iMaxPlayerBuildingProductionModifier += iChange;
}


int CvPlayer::getFreeExperience() const
{
	return m_iFreeExperience;
}


void CvPlayer::changeFreeExperience(int iChange)
{
	m_iFreeExperience += iChange;
}


int CvPlayer::getFeatureProductionModifier() const
{
	return m_iFeatureProductionModifier;
}


void CvPlayer::changeFeatureProductionModifier(int iChange)
{
	m_iFeatureProductionModifier += iChange;
}


int CvPlayer::getWorkerSpeedModifier() const
{
	return m_iWorkerSpeedModifier;
}


void CvPlayer::changeWorkerSpeedModifier(int iChange)
{
	m_iWorkerSpeedModifier += iChange;
}

// BUG - Partial Builds - start
/*
 * Returns the work rate for the best available unit that can build <eBuild> the fastest.
 */
int CvPlayer::getWorkRate(BuildTypes eBuild) const
{
	PROFILE_EXTRA_FUNC();
	int iBestScore = 0;
	int iRate = 0;
	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		if (GC.getUnitInfo((UnitTypes)iI).hasBuild(eBuild))
		{
			const int iUnitRate = GC.getUnitInfo((UnitTypes)iI).getWorkRate();
			int iScore = 0;

			if (getUnitCount((UnitTypes)iI) > 0)
			{
				iScore = iUnitRate*10;
			}
			else if (canTrain((UnitTypes)iI))
			{
				iScore = iUnitRate;
			}

			if (iScore > iBestScore)
			{
				iBestScore = iScore;
				iRate = iUnitRate;
			}
		}
	}
	iRate = getModifiedIntValue(iRate, getWorkerSpeedModifier() + getBuildWorkerSpeedModifierSpecific(eBuild));

	if (isNormalAI())
	{
		const int iMod = (
			GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIWorkRateModifier()
			-
			GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIPerEraModifier() * getCurrentEra()
		);
		if (iMod != 0)
		{
			iRate = getModifiedIntValue(iRate, iMod);
		}
	}
	return iRate;
}
// BUG - Partial Builds - end


int CvPlayer::getImprovementUpgradeRateModifier() const
{
	return m_iImprovementUpgradeRateModifier;
}


void CvPlayer::changeImprovementUpgradeRateModifier(int iChange)
{
	m_iImprovementUpgradeRateModifier += iChange;
}


int CvPlayer::getMilitaryProductionModifier() const
{
	return m_iMilitaryProductionModifier;
}


void CvPlayer::changeMilitaryProductionModifier(int iChange)
{
	m_iMilitaryProductionModifier += iChange;
}


int CvPlayer::getSpaceProductionModifier() const
{
	return m_iSpaceProductionModifier;
}


void CvPlayer::changeSpaceProductionModifier(int iChange)
{
	m_iSpaceProductionModifier += iChange;
}


int CvPlayer::getCityDefenseModifier() const
{
	return m_iCityDefenseModifier + getExtraCityDefense() + getTraitExtraCityDefense();
}


void CvPlayer::changeCityDefenseModifier(int iChange)
{
	m_iCityDefenseModifier += iChange;
}


bool CvPlayer::isNonStateReligionCommerce() const
{
	return m_iNonStateReligionCommerceCount > 0;
}

void CvPlayer::changeNonStateReligionCommerce(int iNewValue)
{
	FASSERT_BOUNDS(0, 3, m_iNonStateReligionCommerceCount);

	m_iNonStateReligionCommerceCount += iNewValue;

	if(iNewValue != 0)
	{
		updateReligionCommerce();
		AI_makeAssignWorkDirty();
	}
}

bool CvPlayer::isUpgradeAnywhere() const
{
	return m_iUpgradeAnywhereCount > 0;
}

void CvPlayer::changeUpgradeAnywhere(int iNewValue)
{
	FASSERT_BOUNDS(0, 3, m_iUpgradeAnywhereCount);

	m_iUpgradeAnywhereCount += iNewValue;
}


int CvPlayer::getRevIdxLocal() const
{
	return m_iRevIdxLocal;
}

void CvPlayer::changeRevIdxLocal(int iChange)
{
	if (iChange != 0)
	{
		m_iRevIdxLocal += iChange;
	}
}


int CvPlayer::getRevIdxNational() const
{
	return m_iRevIdxNational;
}


void CvPlayer::changeRevIdxNational(int iChange)
{
	if (iChange != 0)
	{
		m_iRevIdxNational += iChange;
	}
}


int CvPlayer::getRevIdxDistanceModifier() const
{
	return m_iRevIdxDistanceModifier;
}

void CvPlayer::changeRevIdxDistanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iRevIdxDistanceModifier += iChange;
	}
}


int CvPlayer::getRevIdxHolyCityGood() const
{
	return m_iRevIdxHolyCityGood;
}

void CvPlayer::changeRevIdxHolyCityGood(int iChange)
{
	if (iChange != 0)
	{
		m_iRevIdxHolyCityGood += iChange;
	}
}


int CvPlayer::getRevIdxHolyCityBad() const
{
	return m_iRevIdxHolyCityBad;
}

void CvPlayer::changeRevIdxHolyCityBad(int iChange)
{
	if (iChange != 0)
	{
		m_iRevIdxHolyCityBad += iChange;
	}
}


float CvPlayer::getRevIdxNationalityMod() const
{
	return m_fRevIdxNationalityMod;
}

void CvPlayer::changeRevIdxNationalityMod(float fChange)
{
	if (fChange != 0)
	{
		m_fRevIdxNationalityMod += fChange;
	}
}


float CvPlayer::getRevIdxBadReligionMod() const
{
	return m_fRevIdxBadReligionMod;
}

void CvPlayer::changeRevIdxBadReligionMod(float fChange)
{
	if (fChange != 0)
	{
		m_fRevIdxBadReligionMod += fChange;
	}
}


float CvPlayer::getRevIdxGoodReligionMod() const
{
	return m_fRevIdxGoodReligionMod;
}

void CvPlayer::changeRevIdxGoodReligionMod(float fChange)
{
	if (fChange != 0)
	{
		m_fRevIdxGoodReligionMod += fChange;
	}
}


bool CvPlayer::isInquisitionConditions() const
{
	return GC.getGame().isOption(GAMEOPTION_RELIGION_INQUISITIONS) && getStateReligion() != NO_RELIGION && m_iInquisitionCount > 0;
}

//TB Note: Should be unnecessary now but leaving it in for a bit to allow for a reversal of method if needbe.
//void CvPlayer::setInquisitionConditions()
//{
//	m_bInquisitionConditions = false;
//
//	if (!GC.getGame().isOption(GAMEOPTION_RELIGION_INQUISITIONS) || getStateReligion() == NO_RELIGION)
//	{
//		return;
//	}
//
//	for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
//	{
//		if (GC.getCivicInfo(getCivics((CivicOptionTypes)iI)).isDisallowInquisitions())
//		{
//			return;
//		}
//		else if (GC.getCivicInfo(getCivics((CivicOptionTypes)iI)).isAllowInquisitions())
//		{
//			m_bInquisitionConditions = true;
//		}
//	}
//}


int CvPlayer::getUnitUpgradePriceModifier() const
{
	return m_iUnitUpgradePriceModifier;
}

void CvPlayer::changeUnitUpgradePriceModifier(int iChange)
{
	m_iUnitUpgradePriceModifier += iChange;
}


bool CvPlayer::canFoundReligion() const
{
	if( getNumCities() < 1 || isNPC()
	|| (GC.getGame().isGameStart() && GC.getGame().getElapsedGameTurns() < 3) )
	{
		return false;
	}

	if(GC.getGame().isOption(GAMEOPTION_RELIGION_LIMITED))
	{
		if( ((getNumCities() > 1) && !(isRebel())) || !GC.isLIMITED_RELIGIONS_EXCEPTIONS() )
		{
			if(hasHolyCity())
			{
				return false;
			}
		}
	}
	return true;
}


int CvPlayer::getNumNukeUnits() const
{
	return m_iNumNukeUnits;
}


void CvPlayer::changeNumNukeUnits(int iChange)
{
	m_iNumNukeUnits += iChange;
	FASSERT_NOT_NEGATIVE(getNumNukeUnits());
}


int CvPlayer::getNumOutsideUnits() const
{
	return m_iNumOutsideUnits;
}


void CvPlayer::changeNumOutsideUnits(int iChange)
{
	if (iChange != 0)
	{
		m_iNumOutsideUnits += iChange;
		FASSERT_NOT_NEGATIVE(getNumOutsideUnits());

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


// Toffer - Unit Upkeep

int CvPlayer::getBaseFreeUnitUpkeepCivilian() const
{
	return std::max(0, m_iBaseFreeUnitUpkeepCivilian);
}

void CvPlayer::changeBaseFreeUnitUpkeepCivilian(const int iChange)
{
	if (iChange != 0)
	{
		m_iBaseFreeUnitUpkeepCivilian += iChange;
		setUnitUpkeepDirty();
	}
}

int CvPlayer::getBaseFreeUnitUpkeepMilitary() const
{
	return std::max(0, m_iBaseFreeUnitUpkeepMilitary);
}

void CvPlayer::changeBaseFreeUnitUpkeepMilitary(const int iChange)
{
	if (iChange != 0)
	{
		m_iBaseFreeUnitUpkeepMilitary += iChange;
		setUnitUpkeepDirty();
	}
}

int CvPlayer::getFreeUnitUpkeepCivilianPopPercent() const
{
	return m_iFreeUnitUpkeepCivilianPopPercent;
}

void CvPlayer::changeFreeUnitUpkeepCivilianPopPercent(const int iChange)
{
	if (iChange != 0)
	{
		m_iFreeUnitUpkeepCivilianPopPercent += iChange;
		setUnitUpkeepDirty();
	}
}

int CvPlayer::getFreeUnitUpkeepMilitaryPopPercent() const
{
	return m_iFreeUnitUpkeepMilitaryPopPercent;
}

void CvPlayer::changeFreeUnitUpkeepMilitaryPopPercent(const int iChange)
{
	if (iChange != 0)
	{
		m_iFreeUnitUpkeepMilitaryPopPercent += iChange;
		setUnitUpkeepDirty();
	}
}

int CvPlayer::getFreeUnitUpkeepCivilian() const
{
	return std::max(0, getBaseFreeUnitUpkeepCivilian() + getModifiedIntValue(getTotalPopulation(), getFreeUnitUpkeepCivilianPopPercent()));
}

int CvPlayer::getFreeUnitUpkeepMilitary() const
{
	return std::max(0, getBaseFreeUnitUpkeepMilitary() + getModifiedIntValue(getTotalPopulation(), getFreeUnitUpkeepMilitaryPopPercent()));
}

int CvPlayer::getCivilianUnitUpkeepMod() const
{
	return m_iCivilianUnitUpkeepMod;
}
int CvPlayer::getMilitaryUnitUpkeepMod() const
{
	return m_iMilitaryUnitUpkeepMod;
}

void CvPlayer::changeCivilianUnitUpkeepMod(const int iChange)
{
	if (iChange != 0)
	{
		m_iCivilianUnitUpkeepMod += iChange;
		setUnitUpkeepDirty();
	}
}
void CvPlayer::changeMilitaryUnitUpkeepMod(const int iChange)
{
	if (iChange != 0)
	{
		m_iMilitaryUnitUpkeepMod += iChange;
		setUnitUpkeepDirty();
	}
}

void CvPlayer::changeUnitUpkeep(const int iChange, const bool bMilitary)
{
	if (iChange != 0)
	{
		FAssertMsg(iChange > 0 || bMilitary && -iChange <= m_iUnitUpkeepMilitary100 || !bMilitary && -iChange <= m_iUnitUpkeepCivilian100, "These should always be positive!");

		if (bMilitary)
			m_iUnitUpkeepMilitary100 += iChange;
		else m_iUnitUpkeepCivilian100 += iChange;

		setUnitUpkeepDirty();
	}
}

int64_t CvPlayer::getUnitUpkeepCivilian100() const
{
	return m_iUnitUpkeepCivilian100;
}

int64_t CvPlayer::getUnitUpkeepMilitary100() const
{
	return m_iUnitUpkeepMilitary100;
}

int64_t CvPlayer::getUnitUpkeepCivilian() const
{
	uint64_t iUpkeep = std::max<int64_t>(0, m_iUnitUpkeepCivilian100);

	if (m_iCivilianUnitUpkeepMod > 0)
	{
		iUpkeep = iUpkeep * (100 + m_iCivilianUnitUpkeepMod) / 100;
	}
	else if (m_iCivilianUnitUpkeepMod < 0)
	{
		iUpkeep = iUpkeep * 100 / (100 - m_iCivilianUnitUpkeepMod);
	}
	return static_cast<int64_t>(iUpkeep / 100);
}

int64_t CvPlayer::getUnitUpkeepCivilianNet() const
{
	return std::max<int64_t>(0, getUnitUpkeepCivilian() - getFreeUnitUpkeepCivilian());
}

int64_t CvPlayer::getUnitUpkeepMilitary() const
{
	uint64_t iUpkeep = std::max<int64_t>(0, m_iUnitUpkeepMilitary100);

	if (m_iMilitaryUnitUpkeepMod > 0)
	{
		iUpkeep = iUpkeep * (100 + m_iMilitaryUnitUpkeepMod) / 100;
	}
	else if (m_iMilitaryUnitUpkeepMod < 0)
	{
		iUpkeep = iUpkeep * 100 / (100 - m_iMilitaryUnitUpkeepMod);
	}
	return static_cast<int64_t>(iUpkeep / 100);
}

int64_t CvPlayer::getUnitUpkeepMilitaryNet() const
{
	return std::max<int64_t>(0, getUnitUpkeepMilitary() - getFreeUnitUpkeepMilitary());
}

int64_t CvPlayer::getUnitUpkeepNet(const bool bMilitary, const int iUnitUpkeep) const
{
	if (bMilitary)
	{
		return std::min<int64_t>(iUnitUpkeep, getUnitUpkeepMilitaryNet());
	}
	return std::min<int64_t>(iUnitUpkeep, getUnitUpkeepCivilianNet());
}

int64_t CvPlayer::getFinalUnitUpkeep() const
{
	return m_bUnitUpkeepDirty ? calcFinalUnitUpkeep() : m_iFinalUnitUpkeep;
}

int64_t CvPlayer::calcFinalUnitUpkeep(const bool bReal) const
{
	if (isNPC())
	{
		return 0;
	}
	int64_t iCalc = 0;

	iCalc += getUnitUpkeepCivilianNet();
	iCalc += getUnitUpkeepMilitaryNet();

	if (iCalc > 0)
	{
		// Difficulty adjustment
		iCalc *= GC.getHandicapInfo(getHandicapType()).getUnitUpkeepPercent();
		iCalc /= 100;

		if (!isHumanPlayer())
		{
			iCalc *= GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIUnitUpkeepPercent();
			iCalc /= 100;

			iCalc *= std::max(0, 100 + GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIPerEraModifier() * getCurrentEra());
			iCalc /= 100;
		}
	}
	if (iCalc < 0)
	{
		FErrorMsg("Total unit upkeep is negative! You don't earn gold from upkeep costs!");
		iCalc = 0;
	}
	if (bReal)
	{
		m_iFinalUnitUpkeep = iCalc;
		m_bUnitUpkeepDirty = false;

		// Refresh relevant UI
		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
	return iCalc;
}

void CvPlayer::setUnitUpkeepDirty() const
{
	m_bUnitUpkeepDirty = true;

	// Refresh relevant UI
	if (getID() == GC.getGame().getActivePlayer() && isTurnActive())
	{
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
	}
}

int CvPlayer::getFinalUnitUpkeepChange(const int iExtra, const bool bMilitary)
{
	if (iExtra == 0) return 0;

	// Temporary change
	if (bMilitary)
		m_iUnitUpkeepMilitary100 += iExtra;
	else m_iUnitUpkeepCivilian100 += iExtra;

	const int iChange = static_cast<int>(calcFinalUnitUpkeep(false) - getFinalUnitUpkeep());

	// Very important to restore the real value!
	if (bMilitary)
		m_iUnitUpkeepMilitary100 -= iExtra;
	else m_iUnitUpkeepCivilian100 -= iExtra;

	return iChange;
}
// ! Unit Upkeep


int CvPlayer::getNumMilitaryUnits() const
{
	return m_iNumMilitaryUnits;
}


void CvPlayer::changeNumMilitaryUnits(int iChange)
{
	if (iChange != 0)
	{
		m_iNumMilitaryUnits += iChange;
		FASSERT_NOT_NEGATIVE(getNumMilitaryUnits());

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

int CvPlayer::getHappyPerMilitaryUnit() const
{
	return m_iHappyPerMilitaryUnit;
}


void CvPlayer::changeHappyPerMilitaryUnit(int iChange, bool bLimited)
{
	if (iChange != 0)
	{
		m_iHappyPerMilitaryUnit += iChange;

		if (!bLimited)
		{
			AI_makeAssignWorkDirty();
		}
	}
}


int CvPlayer::getMilitaryFoodProductionCount() const
{
	return m_iMilitaryFoodProductionCount;
}


bool CvPlayer::isMilitaryFoodProduction() const
{
	return (getMilitaryFoodProductionCount() > 0);
}


void CvPlayer::changeMilitaryFoodProductionCount(int iChange, bool bLimited)
{
	if (iChange != 0)
	{
		m_iMilitaryFoodProductionCount += iChange;
		FASSERT_NOT_NEGATIVE(getMilitaryFoodProductionCount());

		if (!bLimited && getTeam() == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getHighestUnitLevel()	const
{
	return m_iHighestUnitLevel;
}


void CvPlayer::setHighestUnitLevel(int iNewValue)
{
	m_iHighestUnitLevel = iNewValue;
	FASSERT_NOT_NEGATIVE(getHighestUnitLevel());
}


int CvPlayer::getMaxConscript() const
{
	return std::max(0, m_iMaxConscript);
}


void CvPlayer::changeMaxConscript(int iChange)
{
	m_iMaxConscript += iChange;
	//TB Note: this should be allowed to be negative.  BUT there's a report to the core so I'm going to leave this here and cap at 0 in getMaxConscript
	FASSERT_NOT_NEGATIVE(getMaxConscript());
}


int CvPlayer::getConscriptCount() const
{
	return m_iConscriptCount;
}


void CvPlayer::setConscriptCount(int iNewValue)
{
	m_iConscriptCount = iNewValue;
	FASSERT_NOT_NEGATIVE(getConscriptCount());
}


void CvPlayer::changeConscriptCount(int iChange)
{
	setConscriptCount(getConscriptCount() + iChange);
}


int CvPlayer::getOverflowResearch() const
{
	return m_iOverflowResearch;
}

void CvPlayer::changeOverflowResearch(int iChange)
{
	m_iOverflowResearch += iChange;
	FASSERT_NOT_NEGATIVE(m_iOverflowResearch);
}


int CvPlayer::getNoUnhealthyPopulationCount() const
{
	return m_iNoUnhealthyPopulationCount;
}


bool CvPlayer::isNoUnhealthyPopulation() const
{
	return (getNoUnhealthyPopulationCount() > 0);
}


void CvPlayer::changeNoUnhealthyPopulationCount(int iChange, bool bLimited)
{
	if (iChange != 0)
	{
		m_iNoUnhealthyPopulationCount += iChange;
		FASSERT_NOT_NEGATIVE(getNoUnhealthyPopulationCount());

		if (!bLimited)
		{
			AI_makeAssignWorkDirty();
		}
	}
}


int CvPlayer::getExpInBorderModifier() const
{
	return m_iExpInBorderModifier;
}


void CvPlayer::changeExpInBorderModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iExpInBorderModifier += iChange;
	}
}


int CvPlayer::getBuildingOnlyHealthyCount() const
{
	return m_iBuildingOnlyHealthyCount;
}


bool CvPlayer::isBuildingOnlyHealthy() const
{
	return (getBuildingOnlyHealthyCount() > 0);
}


void CvPlayer::changeBuildingOnlyHealthyCount(int iChange, bool bLimited)
{
	if (iChange != 0)
	{
		m_iBuildingOnlyHealthyCount += iChange;
		FASSERT_NOT_NEGATIVE(getBuildingOnlyHealthyCount());

		if (!bLimited)
		{
			AI_makeAssignWorkDirty();
		}
	}
}


void CvPlayer::setMaintenanceDirty(const bool bDirty, const bool bCities) const
{
	PROFILE_EXTRA_FUNC();
	m_bMaintenanceDirty = bDirty;

	if (bCities)
	{
		foreach_(CvCity* cityX, cities())
		{
			cityX->setMaintenanceDirty(bDirty, false);
		}
	}
}

void CvPlayer::updateMaintenance() const
{
	PROFILE_EXTRA_FUNC();
	m_iTotalMaintenance = 0;

	foreach_(CvCity* cityX, cities())
	{
		m_iTotalMaintenance += cityX->getMaintenanceTimes100();
	}
	m_bMaintenanceDirty = false;

	FASSERT_NOT_NEGATIVE(m_iTotalMaintenance);
}

int CvPlayer::getMaintenanceModifier()
{
	return m_iMaintenanceModifier;
}

void CvPlayer::changeMaintenanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iMaintenanceModifier += iChange;
		setMaintenanceDirty(true);
	}
}

int CvPlayer::getCoastalDistanceMaintenanceModifier() const
{
	return m_iCoastalDistanceMaintenanceModifier;
}

void CvPlayer::changeCoastalDistanceMaintenanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iCoastalDistanceMaintenanceModifier += iChange;
		setMaintenanceDirty(true);
	}
}

int CvPlayer::getConnectedCityMaintenanceModifier()
{
	return m_iConnectedCityMaintenanceModifier;
}

void CvPlayer::changeConnectedCityMaintenanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iConnectedCityMaintenanceModifier += iChange;
		setMaintenanceDirty(true);
	}
}

void CvPlayer::changeDistanceMaintenanceModifier(const int iChange)
{
	if (iChange != 0)
	{
		m_iDistanceMaintenanceModifier += iChange;
		setMaintenanceDirty(true);
	}
}

void CvPlayer::changeNumCitiesMaintenanceModifier(const int iChange)
{
	if (iChange != 0)
	{
		m_iNumCitiesMaintenanceModifier += iChange;
		setMaintenanceDirty(true);
	}
}

void CvPlayer::changeCorporationMaintenanceModifier(const int iChange, const bool bLimited)
{
	if (iChange != 0)
	{
		m_iCorporationMaintenanceModifier += iChange;

		if (!bLimited)
		{
			setMaintenanceDirty(true);
		}
	}
}

void CvPlayer::changeHomeAreaMaintenanceModifier(const int iChange)
{
	if (iChange != 0)
	{
		m_iHomeAreaMaintenanceModifier += iChange;
		setMaintenanceDirty(true);
	}
}

void CvPlayer::changeOtherAreaMaintenanceModifier(const int iChange)
{
	if (iChange != 0)
	{
		m_iOtherAreaMaintenanceModifier += iChange;
		setMaintenanceDirty(true);
	}
}

int CvPlayer::getTotalMaintenance() const
{
	if (m_bMaintenanceDirty)
	{
		updateMaintenance();
	}
	return m_iTotalMaintenance / 100;
}


int CvPlayer::getUpkeepModifier() const
{
	return m_iUpkeepModifier;
}


void CvPlayer::changeUpkeepModifier(int iChange)
{
	m_iUpkeepModifier += iChange;
}


int CvPlayer::getLevelExperienceModifier() const
{
	return m_iLevelExperienceModifier;
}

void CvPlayer::changeLevelExperienceModifier(int iChange)
{
	m_iLevelExperienceModifier += iChange;
}


int CvPlayer::getCivicHealth() const
{
	// AIAndy: Barbarians do not get player wide unhealthiness
	if (isNPC() && m_iCivicHealth < 0)
	{
		return 0;
	}
	return m_iCivicHealth;
}

void CvPlayer::changeCivicHealth(const int iChange, const bool bLimited)
{
	if (iChange != 0)
	{
		m_iCivicHealth += iChange;

		if (!bLimited)
		{
			AI_makeAssignWorkDirty();
		}
	}
}

int CvPlayer::getExtraHealth() const
{
	// AIAndy: Barbarians do not get player wide unhealthiness
	if (isNPC() && m_iExtraHealth < 0)
	{
		return 0;
	}
	return m_iExtraHealth;
}

void CvPlayer::changeExtraHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraHealth += iChange;
		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getBuildingGoodHealth() const
{
	return m_iBuildingGoodHealth;
}


void CvPlayer::changeBuildingGoodHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingGoodHealth += iChange;
		FASSERT_NOT_NEGATIVE(getBuildingGoodHealth());

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getBuildingBadHealth() const
{
	return m_iBuildingBadHealth;
}


void CvPlayer::changeBuildingBadHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingBadHealth += iChange;
		FAssert(getBuildingBadHealth() <= 0);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getExtraHappiness() const
{
	// AIAndy: Barbarians do not get player wide unhappiness
	if (isNPC() && m_iExtraHappiness < 0)
	{
		return 0;
	}
	return m_iExtraHappiness;
}


void CvPlayer::changeExtraHappiness(int iChange, bool bUnattributed)
{
	if (iChange != 0)
	{
		m_iExtraHappiness += iChange;

		if ( bUnattributed )
		{
			m_iExtraHappinessUnattributed += iChange;
		}

		//AIAndy: Barbarians do not get player wide unhappiness
		if ((m_iExtraHappiness < 0) && isNPC())
		{
			m_iExtraHappiness = 0;
		}

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getBuildingHappiness() const
{
	return m_iBuildingHappiness;
}


void CvPlayer::changeBuildingHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingHappiness += iChange;

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getLargestCityHappiness() const
{
	return m_iLargestCityHappiness;
}


void CvPlayer::changeLargestCityHappiness(int iChange, bool bLimited)
{
	if (iChange != 0)
	{
		m_iLargestCityHappiness += iChange;

		if (!bLimited)
		{
			AI_makeAssignWorkDirty();
		}
	}
}


int CvPlayer::getWarWearinessPercentAnger() const
{
	return m_iWarWearinessPercentAnger;
}


void CvPlayer::updateWarWearinessPercentAnger()
{
	PROFILE_FUNC();

	int iNewWarWearinessPercentAnger = 0;

	if (!isNPC() && !isMinorCiv())
	{
		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			const CvTeam& kTeam = GET_TEAM((TeamTypes)iI);
			if (kTeam.isAlive() && !kTeam.isMinorCiv() && kTeam.isAtWar(getTeam()))
			{
				iNewWarWearinessPercentAnger += GET_TEAM(getTeam()).getWarWeariness((TeamTypes)iI) * std::max(0, 100 + kTeam.getEnemyWarWearinessModifier()) / 1000000;
			}
		}
	}
	if (iNewWarWearinessPercentAnger != 0)
	{
		iNewWarWearinessPercentAnger = getModifiedWarWearinessPercentAnger(iNewWarWearinessPercentAnger);
	}

	if (getWarWearinessPercentAnger() != iNewWarWearinessPercentAnger)
	{
		m_iWarWearinessPercentAnger = iNewWarWearinessPercentAnger;
		AI_makeAssignWorkDirty();
	}
}

int CvPlayer::getModifiedWarWearinessPercentAnger(int iWarWearinessPercentAnger) const
{
	iWarWearinessPercentAnger *= GC.getDefineINT("BASE_WAR_WEARINESS_MULTIPLIER");

	if (GC.getGame().isGameMultiPlayer())
	{
		iWarWearinessPercentAnger = getModifiedIntValue(iWarWearinessPercentAnger, GC.getDefineINT("MULTIPLAYER_WAR_WEARINESS_MODIFIER"));
	}
	iWarWearinessPercentAnger = getModifiedIntValue(iWarWearinessPercentAnger, GC.getWorldInfo(GC.getMap().getWorldSize()).getWarWearinessModifier());

	if (isNormalAI())
	{
		const int iMod =
		(
			GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIWarWearinessPercent() - 100
			+
			GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIPerEraModifier() * getCurrentEra()
		);
		if (iMod != 0)
		{
			iWarWearinessPercentAnger = getModifiedIntValue(iWarWearinessPercentAnger, iMod);
		}
	}
	return iWarWearinessPercentAnger;
}


int CvPlayer::getWarWearinessModifier() const
{
	return m_iWarWearinessModifier;
}


void CvPlayer::changeWarWearinessModifier(int iChange, bool bLimited)
{
	if (iChange != 0)
	{
		m_iWarWearinessModifier += iChange;

		if (!bLimited)
		{
			AI_makeAssignWorkDirty();
		}
	}
}


int CvPlayer::getFreeSpecialist() const
{
	return m_iFreeSpecialist;
}


void CvPlayer::changeFreeSpecialist(int iChange)
{
	if (iChange != 0)
	{
		m_iFreeSpecialist += iChange;
		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getNoForeignTradeCount() const
{
	return m_iNoForeignTradeCount;
}


bool CvPlayer::isNoForeignTrade() const
{
	return (getNoForeignTradeCount() > 0);
}


void CvPlayer::changeNoForeignTradeCount(int iChange, bool bLimited)
{
	if (iChange != 0)
	{
		m_iNoForeignTradeCount += iChange;
		FASSERT_NOT_NEGATIVE(getNoForeignTradeCount());

		if (!bLimited)
		{
			GC.getGame().updateTradeRoutes();
		}
	}
}


int CvPlayer::getNoCorporationsCount() const
{
	return m_iNoCorporationsCount;
}


bool CvPlayer::isNoCorporations() const
{
	return (getNoCorporationsCount() > 0);
}


void CvPlayer::changeNoCorporationsCount(int iChange, bool bLimited)
{
	if (iChange != 0)
	{
		m_iNoCorporationsCount += iChange;
		FASSERT_NOT_NEGATIVE(getNoCorporationsCount());

		if (!bLimited)
		{
			updateCorporation();
		}
	}
}


int CvPlayer::getNoForeignCorporationsCount() const
{
	return m_iNoForeignCorporationsCount;
}


bool CvPlayer::isNoForeignCorporations() const
{
	return (getNoForeignCorporationsCount() > 0);
}


void CvPlayer::changeNoForeignCorporationsCount(int iChange, bool bLimited)
{
	if (iChange != 0)
	{
		m_iNoForeignCorporationsCount += iChange;
		FASSERT_NOT_NEGATIVE(getNoForeignCorporationsCount());

		if (!bLimited)
		{
			updateCorporation();
		}
	}
}


int CvPlayer::getCoastalTradeRoutes() const
{
	return m_iCoastalTradeRoutes;
}


void CvPlayer::changeCoastalTradeRoutes(int iChange)
{
	if (iChange != 0)
	{
		m_iCoastalTradeRoutes += iChange;
		updateTradeRoutes();
	}
}


int CvPlayer::getTradeRoutes() const
{
	return m_iTradeRoutes;
}


void CvPlayer::changeTradeRoutes(int iChange)
{
	if (iChange != 0)
	{
		m_iTradeRoutes += iChange;
		updateTradeRoutes();
	}
}


int CvPlayer::getRevolutionTimer() const
{
	return m_iRevolutionTimer;
}


void CvPlayer::setRevolutionTimer(int iNewValue)
{
	if (getRevolutionTimer() != iNewValue)
	{
		m_iRevolutionTimer = iNewValue;
		FASSERT_NOT_NEGATIVE(getRevolutionTimer());

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(MiscButtons_DIRTY_BIT, true);
		}
	}
}


void CvPlayer::changeRevolutionTimer(int iChange)
{
	setRevolutionTimer(getRevolutionTimer() + iChange);
}


int CvPlayer::getConversionTimer() const
{
	return m_iConversionTimer;
}


void CvPlayer::setConversionTimer(int iNewValue)
{
	if (getConversionTimer() != iNewValue)
	{
		m_iConversionTimer = iNewValue;
		FASSERT_NOT_NEGATIVE(getConversionTimer());

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(MiscButtons_DIRTY_BIT, true);
		}
	}
}


void CvPlayer::changeConversionTimer(int iChange)
{
	setConversionTimer(getConversionTimer() + iChange);
}


int CvPlayer::getStateReligionCount() const
{
	return m_iStateReligionCount;
}


bool CvPlayer::isStateReligion() const
{
	return (getStateReligionCount() > 0);
}


void CvPlayer::changeStateReligionCount(int iChange, bool bLimited)
{
	if (iChange != 0)
	{
		// religion visibility now part of espionage
		//GC.getGame().updateCitySight(false, true);

		m_iStateReligionCount += iChange;
		FASSERT_NOT_NEGATIVE(getStateReligionCount());

		// religion visibility now part of espionage
		//GC.getGame().updateCitySight(true, true);

		if (!bLimited)
		{
			setMaintenanceDirty(true);
		}

		updateReligionHappiness(bLimited);

		if (!bLimited)
		{
			updateReligionCommerce();

			GC.getGame().AI_makeAssignWorkDirty();

			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		}

		checkReligiousDisablingAllBuildings();
	}
}


int CvPlayer::getNoNonStateReligionSpreadCount() const
{
	return m_iNoNonStateReligionSpreadCount;
}


bool CvPlayer::isNoNonStateReligionSpread() const
{
	return (getNoNonStateReligionSpreadCount() > 0);
}


void CvPlayer::changeNoNonStateReligionSpreadCount(int iChange)
{
	m_iNoNonStateReligionSpreadCount += iChange;
	FASSERT_NOT_NEGATIVE(getNoNonStateReligionSpreadCount());
}


int CvPlayer::getStateReligionHappiness() const
{
	return m_iStateReligionHappiness;
}


void CvPlayer::changeStateReligionHappiness(int iChange, bool bLimited)
{
	if (iChange != 0)
	{
		m_iStateReligionHappiness += iChange;

		updateReligionHappiness(bLimited);
	}
}


int CvPlayer::getNonStateReligionHappiness() const
{
	return m_iNonStateReligionHappiness;
}


//Fuyu bLimited
void CvPlayer::changeNonStateReligionHappiness(int iChange, bool bLimited)
{
	if (iChange != 0)
	{
		m_iNonStateReligionHappiness += iChange;

		updateReligionHappiness(bLimited);
	}
}


int CvPlayer::getStateReligionUnitProductionModifier() const
{
	return m_iStateReligionUnitProductionModifier;
}


void CvPlayer::changeStateReligionUnitProductionModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iStateReligionUnitProductionModifier += iChange;

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getStateReligionBuildingProductionModifier() const
{
	return m_iStateReligionBuildingProductionModifier;
}


void CvPlayer::changeStateReligionBuildingProductionModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iStateReligionBuildingProductionModifier += iChange;

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getStateReligionFreeExperience() const
{
	return m_iStateReligionFreeExperience;
}


void CvPlayer::changeStateReligionFreeExperience(int iChange)
{
	m_iStateReligionFreeExperience += iChange;
}


CvCity* CvPlayer::getCapitalCity() const
{
	return getCity(m_iCapitalCityID);
}


void CvPlayer::setCapitalCity(CvCity* pNewCapitalCity)
{
	PROFILE_EXTRA_FUNC();
	CvCity* pOldCapitalCity = getCapitalCity();

	if (pOldCapitalCity != pNewCapitalCity)
	{
		const bool bUpdatePlotGroups = pOldCapitalCity == NULL || pNewCapitalCity == NULL || pOldCapitalCity->plot()->getOwnerPlotGroup() != pNewCapitalCity->plot()->getOwnerPlotGroup();

		if (bUpdatePlotGroups)
		{
			startDeferredPlotGroupBonusCalculation();

			if (pOldCapitalCity)
			{
				pOldCapitalCity->plot()->updatePlotGroupBonus(false);
			}
			if (pNewCapitalCity)
			{
				pNewCapitalCity->plot()->updatePlotGroupBonus(false);
			}
		}

		if (pNewCapitalCity)
		{
			m_iCapitalCityID = pNewCapitalCity->getID();

			for (int iI = 0; iI < GC.getCivilizationInfo(getCivilizationType()).getNumCivilizationBuildings(); iI++)
			{
				pNewCapitalCity->changeHasBuilding((BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuilding(iI), true);

				if (pOldCapitalCity)
				{
					pOldCapitalCity->changeHasBuilding((BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuilding(iI), false);
				}
			}
		}
		else  m_iCapitalCityID = FFreeList::INVALID_INDEX;

		//ls612: Embassy Visibility Fix (by Damgo)
		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (GET_TEAM(getTeam()).isHasEmbassy((TeamTypes)iI))
			{
				if (pOldCapitalCity)
				{
					pOldCapitalCity->plot()->changeAdjacentSight((TeamTypes)iI, 1, false, NULL, bUpdatePlotGroups);
				}
				if (pNewCapitalCity)
				{
					pNewCapitalCity->plot()->changeAdjacentSight((TeamTypes)iI, 1, true, NULL, bUpdatePlotGroups);
				}
			}
		}

		if (bUpdatePlotGroups)
		{
			if (pOldCapitalCity)
			{
				pOldCapitalCity->plot()->updatePlotGroupBonus(true);
			}
			if (pNewCapitalCity)
			{
				pNewCapitalCity->plot()->updatePlotGroupBonus(true);
			}
			endDeferredPlotGroupBonusCalculation();
		}

		if (pNewCapitalCity)
		{
			if (!pOldCapitalCity)
			{
				pNewCapitalCity->area()->setHomeArea(getID(), NULL);
			}
			else if (pOldCapitalCity->area() != pNewCapitalCity->area())
			{
				pNewCapitalCity->area()->setHomeArea(getID(), pOldCapitalCity->area());
			}
		}

		setMaintenanceDirty(true);
		updateTradeRoutes();

		if (pOldCapitalCity)
		{
			for(int iI = 0; iI < NUM_COMMERCE_TYPES ; iI++ )
			{
				pOldCapitalCity->setCommerceModifierDirty((CommerceTypes)iI);
			}
			pOldCapitalCity->updateCommerce();
			pOldCapitalCity->setInfoDirty(true);
		}
		if (pNewCapitalCity)
		{
			for(int iI = 0; iI < NUM_COMMERCE_TYPES ; iI++ )
			{
				pNewCapitalCity->setCommerceModifierDirty((CommerceTypes)iI);
			}
			pNewCapitalCity->updateCommerce();
			pNewCapitalCity->setInfoDirty(true);
		}
	}
}


int CvPlayer::getCitiesLost() const
{
	return m_iCitiesLost;
}


void CvPlayer::changeCitiesLost(int iChange)
{
	m_iCitiesLost += iChange;
}


int CvPlayer::getWinsVsBarbs() const
{
	return m_iWinsVsBarbs;
}


void CvPlayer::changeWinsVsBarbs(int iChange)
{
	m_iWinsVsBarbs += iChange;
	FASSERT_NOT_NEGATIVE(m_iWinsVsBarbs);
}


int CvPlayer::getAssets() const
{
	return m_iAssets / 100;
}


void CvPlayer::changeAssets(int iChange)
{
	m_iAssets += iChange;
	FAssertRecalcMsg(m_iAssets >= 0, "Player assets value fell below 0");
}


int CvPlayer::getPower() const
{
	return (m_iPower + m_iTechPower + m_iUnitPower) / 100;
}

int CvPlayer::getTechPower() const
{
	return m_iTechPower / 100;
}

int CvPlayer::getUnitPower() const
{
	return m_iUnitPower / 100;
}


void CvPlayer::changePower(int iChange)
{
	m_iPower += iChange;
	FASSERT_NOT_NEGATIVE(m_iPower);
}

void CvPlayer::changeTechPower(int iChange)
{
	m_iTechPower += iChange;
	FASSERT_NOT_NEGATIVE(m_iTechPower);
}

void CvPlayer::changeUnitPower(int iChange)
{
	m_iUnitPower += iChange;
	FASSERT_NOT_NEGATIVE(m_iUnitPower);
}


int CvPlayer::getPopScore(bool bCheckVassal) const
{
	PROFILE_EXTRA_FUNC();
	if (bCheckVassal && GET_TEAM(getTeam()).isAVassal())
	{
		return m_iPopulationScore / 2;
	}

	int iVassalScore = 0;

	if (bCheckVassal)
	{
		for (int i = 0; i < MAX_PC_PLAYERS; i++)
		{
			if (i != getID())
			{
				const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)i);
				if (kLoopPlayer.isAlive() && GET_TEAM(kLoopPlayer.getTeam()).isVassal(getTeam()))
				{
					iVassalScore += kLoopPlayer.getPopScore(false) / 2;
				}
			}
		}
	}

	return (m_iPopulationScore + iVassalScore / std::max(1, GET_TEAM(getTeam()).getNumMembers()));
}

void CvPlayer::changePopScore(int iChange)
{
	if (iChange != 0)
	{
		m_iPopulationScore += iChange;
		FASSERT_NOT_NEGATIVE(getPopScore());

		GC.getGame().setScoreDirty(true);
	}
}


int CvPlayer::getLandScore(bool bCheckVassal) const
{
	PROFILE_EXTRA_FUNC();
	if (bCheckVassal && GET_TEAM(getTeam()).isAVassal())
	{
		return m_iLandScore / 2;
	}

	int iVassalScore = 0;

	if (bCheckVassal)
	{
		for (int i = 0; i < MAX_PC_PLAYERS; i++)
		{
			if (i != getID())
			{
				const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)i);
				if (kLoopPlayer.isAlive() && GET_TEAM(kLoopPlayer.getTeam()).isVassal(getTeam()))
				{
					iVassalScore += kLoopPlayer.getLandScore(false) / 2;
				}
			}
		}
	}

	return (m_iLandScore + iVassalScore  / std::max(1, GET_TEAM(getTeam()).getNumMembers()));
}


void CvPlayer::changeLandScore(int iChange)
{
	if (iChange != 0)
	{
		m_iLandScore += iChange;
		FASSERT_NOT_NEGATIVE(getLandScore());

		GC.getGame().setScoreDirty(true);
	}
}


int CvPlayer::getWondersScore() const
{
	//TB Debug
	if (m_iWondersScore < 0)
	{
		return 0;
	}
	return m_iWondersScore;
}


void CvPlayer::changeWondersScore(int iChange)
{
	if (iChange != 0)
	{
		m_iWondersScore += iChange;
		FASSERT_NOT_NEGATIVE(getWondersScore());

		GC.getGame().setScoreDirty(true);
	}
}


int CvPlayer::getTechScore() const
{
	return m_iTechScore;
}


void CvPlayer::changeTechScore(int iChange)
{
	if (iChange != 0)
	{
		m_iTechScore += iChange;
		FASSERT_NOT_NEGATIVE(m_iTechScore);

		GC.getGame().setScoreDirty(true);

		//	Change in techs invalidates cached player level buildability of buildings
		clearCanConstructCache(NO_BUILDING, true);
	}
}

int CvPlayer::getCombatExperience() const
{
	return m_iCombatExperience;
}

namespace {
	int calculateGreatGeneralSpawnCityScore(const CvCity* city, int numCities)
	{
		int iValue = 3 * GC.getGame().getSorenRandNum(numCities, "Warlord City Selection");

		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			iValue += city->findYieldRateRank((YieldTypes)i);
		}
		iValue += 3*city->findPopulationRank();

		if (!city->plot()->isMapCategoryType(GC.getMAPCATEGORY_EARTH()))
		{
			iValue *= 100;
		}
		return iValue;
	}
}

void CvPlayer::setCombatExperience(int iExperience, UnitTypes eGGType)
{
	if (iExperience < 0)
	{
		FErrorMsg("iExperience < 0");
		iExperience = 0;
	}
	if (iExperience == m_iCombatExperience)
	{
		return;
	}
	if (iExperience > m_iCombatExperience)
	{
		changeGreatGeneralPointsForType(eGGType == NO_UNIT ? (UnitTypes)GC.getInfoTypeForString("UNIT_GREAT_GENERAL") : eGGType, iExperience - m_iCombatExperience);
	}
	m_iCombatExperience = iExperience;

	if (!isNPC() || isHominid())
	{
		const int iExperienceThreshold = greatPeopleThresholdMilitary();

		if (m_iCombatExperience >= iExperienceThreshold)
		{
			// create great person
			CvCity* pBestCity = scoring::min_score(cities(), bind(calculateGreatGeneralSpawnCityScore, _1, getNumCities())).get_value_or(nullptr);
			if (pBestCity)
			{
				const UnitTypes eGGTypetoAssign = getGreatGeneralTypetoAssign();
				pBestCity->createGreatPeople(eGGTypetoAssign, false, true);
				// HACK: overflows can cause experience explosion, this will ensure we don't have more than 1 GG of experience accumulated
				m_iCombatExperience = (m_iCombatExperience - iExperienceThreshold) % iExperienceThreshold;
				// Normal code: if the overflows weren't present this code would be used
				// m_iCombatExperience = m_iCombatExperience - iExperienceThreshold;
				setGreatGeneralPointsForType(eGGTypetoAssign, 0);
			}
		}
	}
	FASSERT_NOT_NEGATIVE(m_iCombatExperience);
}

void CvPlayer::changeCombatExperience(int iChange, UnitTypes eGGType)
{
	setCombatExperience(getCombatExperience() + iChange, eGGType);
}


bool CvPlayer::isConnected() const
{
	return gDLL->isConnected( getNetID() );
}

int CvPlayer::getNetID() const
{
	return GC.getInitCore().getNetID(getID());
}


void CvPlayer::setNetID(int iNetID)
{
	GC.getInitCore().setNetID(getID(), iNetID);
}

void CvPlayer::sendReminder()
{
	CvWString szYearStr;

	// Only perform this step if we have a valid email address on record,
	// and we have provided information about how to send emails
	if ( !getPbemEmailAddress().empty() &&
		!gDLL->GetPitbossSmtpHost().empty() )
	{

		GAMETEXT.setTimeStr(szYearStr, GC.getGame().getGameTurn(), true);

		// Now send our email via Python
		int iReturnCode = Cy::call<int>(PYPitBossModule, "sendEmail", Cy::Args()
			<< getPbemEmailAddress()
			<< gDLL->GetPitbossSmtpHost()
			<< gDLL->GetPitbossSmtpLogin()
			<< gDLL->GetPitbossSmtpPassword()
			<< GC.getGame().getName()
			<< GC.getGame().isMPOption(MPOPTION_TURN_TIMER)
			<< GC.getGame().getPitbossTurnTime()
			<< gDLL->GetPitbossEmail()
			<< szYearStr);

		FAssertMsg(iReturnCode == 0, "Pitboss Python fn onSendEmail encountered an error" );
	}
}

uint CvPlayer::getStartTime() const
{
	return m_uiStartTime;
}


void CvPlayer::setStartTime(uint uiStartTime)
{
	m_uiStartTime = uiStartTime;
}


uint CvPlayer::getTotalTimePlayed() const
{
	return ((timeGetTime() - m_uiStartTime)/1000);
}


bool CvPlayer::isMinorCiv() const
{
	return GC.getInitCore().getMinorNationCiv(m_eID);
}


bool CvPlayer::isAlive() const
{
	return m_bAlive;
}


bool CvPlayer::isEverAlive() const
{
	return m_bEverAlive;
}


void CvPlayer::setAlive(bool bNewValue, bool bActivateTurn)
{
	PROFILE_EXTRA_FUNC();
	if (m_bAlive != bNewValue)
	{
		m_bAlive = bNewValue;

		GET_TEAM(getTeam()).changeAliveCount(bNewValue ? 1 : -1);

		// Report event to Python
		CvEventReporter::getInstance().setPlayerAlive(getID(), bNewValue);

		if (bNewValue)
		{
			if (!isEverAlive())
			{
				m_bEverAlive = true;

				GET_TEAM(getTeam()).changeEverAliveCount(1);
			}

			if (getNumCities() == 0)
			{
				setFoundedFirstCity(false);
			}

			updatePlotGroups();

			if (bActivateTurn)
			{
				if (GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS)
				|| (GC.getGame().getNumGameTurnActive() == 0)
				|| (GC.getGame().isSimultaneousTeamTurns() && GET_TEAM(getTeam()).isTurnActive()))
				{
					setTurnActive(true);
				}
			}

			gDLL->openSlot(getID());

			for( int iI = 0; iI < MAX_PLAYERS; iI++ )
			{
				GET_PLAYER((PlayerTypes)iI).AI_invalidateAttitudeCache(getID());
				AI_invalidateAttitudeCache((PlayerTypes)iI);
			}
		}
		else
		{
			clearResearchQueue();
			killAllDeals();
			killCities();
			killUnits();

			clearTileCulture();
			if (!GC.getGame().isOption(GAMEOPTION_UNSUPPORTED_REVOLUTION))
			{
				clearCityCulture();
			}

			if (isMinorCiv() && !GET_TEAM(getTeam()).isAlive())
			{
				GET_TEAM(getTeam()).setIsMinorCiv(false);
			}

			setTurnActive(false);

			gDLL->endMPDiplomacy();
			gDLL->endDiplomacy();

			if (!isHumanPlayer())
			{
				gDLL->closeSlot(getID());
			}

			if (GC.getGame().getElapsedGameTurns() > 0 && !isNPC())
			{
				CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CIV_DESTROYED", getCivilizationAdjectiveKey());

				// Report Deaths For Known Teams
				for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasMet(getTeam()))
					{
						AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVDESTROYED", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
					}
				}

				GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szBuffer, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
			}
		}

		GC.getGame().setScoreDirty(true);
	}
}


void CvPlayer::verifyAlive()
{
	PROFILE_EXTRA_FUNC();
	if (isAlive())
	{
		if (isNPC() || getAdvancedStartPoints() > -1)
		{
			return;
		}

		// Check if player is defeated
		if (
			// No city nor units is always defeat
			getNumCities() == 0
		&&
			( // No city confirmed
				getNumUnits() == 0
				||
				// Are units enough to stay alive?
				!GC.getGame().isOption(GAMEOPTION_EXP_COMPLETE_KILLS) // If option is active, YES.
				&&
				!isRebel() // Rebel players do not need cities
				&&
				!haveSettlerUnit() // A settler is a potential city

/* Toffer - the haveSettlerUnit check makes this redundant for now, may be useful for nomadic start though as you won't start with a settler unit then.
				&&
				//TBNOMADICSTART: Will need to add the condition HERE to enable civs on Nomadic Start with a qualifying Nomadic Unit fulfill the need of a city prerequisite to stay 'alive' in the game.
				isFoundedFirstCity()
*/
			)
		||
			// Max city elimination rule
			GC.getGame().getMaxCityElimination() > 0
			&&
			getCitiesLost() >= GC.getGame().getMaxCityElimination()
		)
		{
			setAlive(false);
			// Toffer - Take over alive AI with lowest index if we die when on autoplay.
			if (isHumanDisabled())
			{
				for (int x = 0; x < MAX_PC_PLAYERS; x++)
				{
					if (GET_PLAYER((PlayerTypes)x).isAlive() && !GET_PLAYER((PlayerTypes)x).isHumanPlayer(true))
					{
						GC.getGame().changeHumanPlayer((PlayerTypes)getID(), (PlayerTypes)x);
						break;
					}
				}
			}
		}
	}
	else if (getNumCities() > 0 || getNumUnits() > 0)
	{
		setAlive(true);
	}
}


bool CvPlayer::isTurnActive() const
{
	return m_bTurnActive;
}

void CvPlayer::setTurnActiveForPbem(bool bActive)
{
	FAssertMsg(GC.getGame().isPbem(), "You are using setTurnActiveForPbem. Are you sure you know what you're doing?");

	// does nothing more than to set the member variable before saving the game
	// the rest of the turn will be performed upon loading the game
	// This allows the player to browse the game in paused mode after he has generated the save
	if (m_bTurnActive != bActive)
	{
		m_bTurnActive = bActive;
		GC.getGame().changeNumGameTurnActive(bActive ? 1 : -1);
		GC.getMap().invalidateActivePlayerPlotCache();
	}
}


void CvPlayer::setTurnActive(bool bNewValue, bool bDoTurn)
{
	PROFILE_FUNC();

	bool bFinancialTrouble = false;
	
	if (m_bTurnActive != bNewValue)
	{
		m_bTurnActive = bNewValue;
		if (bNewValue)
		{
			PROFILE("CvPlayer::setTurnActive.SetActive");

			if (getID() == GC.getGame().getActivePlayer())
			{
				stopProfilingDLL(true);
			}

			if (GC.getLogging())
			{
				char szOut[1024];
				sprintf(szOut, "Player %d Turn ON\n", getID());
				gDLL->messageControlLog(szOut);
			}
			if (GC.getGame().getGameTurn() > 10)
			{  // Calvitix, problem on Scenario creation
				bFinancialTrouble = AI_isFinancialTrouble();
			}
			if (gPlayerLogLevel > 0)
			{
				PROFILE("CvPlayer::setTurnActive.SetActive.Log0");

				logBBAI("Player %d (%S) setTurnActive for turn %d", getID(), getCivilizationDescription(0), GC.getGame().getGameTurn() );

				if (GC.getGame().getGameTurn() > 0 && (GC.getGame().getGameTurn() % 25) == 0 && !isNPC())
				{
					CvWStringBuffer szBuffer;
					GAMETEXT.setScoreHelp(szBuffer, getID());
					logBBAI("%S", szBuffer.getCString());

					const int iGameTurn = GC.getGame().getGameTurn();
					logBBAI("  Total Score: %d, Population Score: %d (%d total pop), Land Score: %d, Tech Score: %d, Wonder Score: %d", calculateScore(), getPopScore(false), getTotalPopulation(), getLandScore(false), getTechScore(), getWondersScore());

					int64_t iEconomy = 0;
					int64_t iProduction = 0;
					int64_t iAgri = 0;
					int iCount = 0;
					for (int iI = 1; iI <= 5; iI++)
					{
						if (iGameTurn - iI >= 0)
						{
							iEconomy += getEconomyHistory(iGameTurn - iI);
							iProduction += getIndustryHistory(iGameTurn - iI);
							iAgri += getAgricultureHistory(iGameTurn - iI);
							iCount++;
						}
					}
					iEconomy /= std::max(1, iCount);
					iProduction /= std::max(1, iCount);
					iAgri /= std::max(1, iCount);

					logBBAI("  Economy avg: %d,  Industry avg: %d,  Agriculture avg: %d", iEconomy, iProduction, iAgri);
				}

				if (gPlayerLogLevel >= 2)
				{
					PROFILE("CvPlayer::setTurnActive.SetActive.Log2");

					CvWStringBuffer szBuffer;

					logBBAI("	Player %d (%S) has %d cities, %d pop, %d power, %d tech percent", getID(), getCivilizationDescription(0), getNumCities(), getTotalPopulation(), getPower(), GET_TEAM(getTeam()).getBestKnownTechScorePercent());

					if(bFinancialTrouble)
					{
						logBBAI("	Financial trouble!");
					}
					szBuffer.append(CvWString::format(L"	Team %d has met: ", getTeam()));

					for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
					{
						if (iI != getTeam() && GET_TEAM(getTeam()).isHasMet((TeamTypes)iI) && GET_TEAM((TeamTypes)iI).isAlive())
						{
							szBuffer.append(CvWString::format(L"%d,", iI));
						}
					}

					if (GET_TEAM(getTeam()).getVassalCount() > 0)
					{
						szBuffer.append(CvWString::format(L";  vassals: "));

						for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
						{
							if (iI != getTeam() && GET_TEAM((TeamTypes)iI).isVassal(getTeam()) && GET_TEAM((TeamTypes)iI).isAlive())
							{
								szBuffer.append(CvWString::format(L"%d,", iI));
							}
						}
					}

					if (GET_TEAM(getTeam()).isAtWar(true))
					{
						szBuffer.append(CvWString::format(L";  at war with: "));

						for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
						{
							if (iI != getTeam() && GET_TEAM(getTeam()).isAtWar((TeamTypes)iI) && GET_TEAM((TeamTypes)iI).isAlive())
							{
								szBuffer.append(CvWString::format(L"%d,", iI));
							}
						}
					}
					const bool bWarPlan = GET_TEAM(getTeam()).hasWarPlan(true);

					if (bWarPlan)
					{
						szBuffer.append(CvWString::format(L";  planning war with: "));

						for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
						{
							if (iI != getTeam() && GET_TEAM((TeamTypes)iI).isAlive()
							&& !GET_TEAM(getTeam()).isAtWar((TeamTypes)iI) && GET_TEAM(getTeam()).AI_getWarPlan((TeamTypes)iI) != NO_WARPLAN)
							{
								szBuffer.append(CvWString::format(L"%d,", iI));
							}
						}
						if (gPlayerLogLevel > 1)
						{
							logBBAI("%S\n\tEnemy power perc: %d (%d with others reduction)", szBuffer.getCString(), GET_TEAM(getTeam()).AI_getEnemyPowerPercent(), GET_TEAM(getTeam()).AI_getEnemyPowerPercent(true));
						}
					}
					else if (gPlayerLogLevel > 1)
					{
						logBBAI("%S", szBuffer.getCString());
					}
					szBuffer.clear();
				}
			}
			FAssertMsg(isAlive(), "isAlive is expected to be true");

			changeEndTurn(false);
			GC.getGame().resetTurnTimer();
			{
				PROFILE("CvPlayer::setTurnActive.SetActive.CalcDanger");

				//	Calculate plot danger values for this player
				for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
				{
					CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

					//	Decay danger count from the previous turn
					pLoopPlot->setDangerCount(m_eID, 2*pLoopPlot->getDangerCount(m_eID)/3);
				}

				for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
				{
					CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

					//	Add in known dangers from visible enemy units
					if (pLoopPlot->isVisible(getTeam(), false))
					{
						//	Enemies are unconditionally dangerous, but any non-ally
						//	might become an enemy so non-ally troop build ups are also
						//	suspicious except in your own territory (where their
						//	presence implies a treaty)
						int	iDangerCount = pLoopPlot->getVisibleEnemyStrength(m_eID)*2;

						if (getTeam() != pLoopPlot->getTeam())
						{
							iDangerCount += pLoopPlot->getVisibleNonAllyStrength(m_eID);
						}
						addPlotDangerSource(pLoopPlot, iDangerCount/3);
					}
				}
			}

			// Send email to disconnected human players if this is the pitboss.
			if (gDLL->IsPitbossHost() && isHumanPlayer() && !isConnected())
			{
				sendReminder();
			}

			if ((GC.getGame().isHotSeat() || GC.getGame().isPbem()) && isHumanPlayer() && bDoTurn)
			{
				gDLL->getInterfaceIFace()->clearEventMessages();
				GC.setResourceLayer(false);
				//gDLL->getEngineIFace()->setResourceLayer(false);

				GC.getGame().setActivePlayer(getID());
			}

			GC.getGame().changeNumGameTurnActive(1);

			//	Update the Great Wall at the start of the active player's turn
			if (GC.getGame().getActivePlayer() == getID())
			{
				PROFILE("CvPlayer::setTurnActive.ProcessGW");

				//	Out
				GC.getGame().processGreatWall(false);
				//	...and straight back in
				GC.getGame().processGreatWall(true);
			}
			else if (!GET_PLAYER(GC.getGame().getActivePlayer()).isTurnActive())
			{
				Cy::call(PYScreensModule, "updateWaitingForPlayer", Cy::Args() << getID());
			}

			if (bDoTurn)
			{
				PROFILE("CvPlayer::setTurnActive.SetActive.doTurn");

				// cleans up contractbroker, and finished contracts
				m_contractBroker.cleanup();

				if (isAlive() && !isHumanPlayer() && !isNPC() && getAdvancedStartPoints() >= 0)
				{
					AI_doAdvancedStart();
				}

				if (GC.getGame().getElapsedGameTurns() > 0 && isAlive())
				{
					// Koshling - Moved AI turn processing to the START of the turn
					//	(i.e. before unit movement) so as to allow units to answer city requests.
					doTurn();
					doTurnUnits(); // AI create colony (splitEmpire) evaluation at the end of doTurnUnits().
				}

				if (getID() == GC.getGame().getActivePlayer() && GC.getGame().getElapsedGameTurns() > 0)
				{
					if (GC.getGame().isNetworkMultiPlayer())
					{
						AddDLLMessage(
							getID(), true, GC.getEVENT_MESSAGE_TIME(),
							gDLL->getText("TXT_KEY_MISC_TURN_BEGINS").GetCString(),
							"AS2D_NEWTURN", MESSAGE_TYPE_DISPLAY_ONLY
						);
					}
					else gDLL->getInterfaceIFace()->playGeneralSound("AS2D_NEWTURN");
				}
				doWarnings();
			}
		}
		else
		{
			PROFILE("CvPlayer::setTurnActive.SetInactive");

			setTurnHadUIInteraction(false);

			// start profiling DLL if desired
			if (getID() == GC.getGame().getActivePlayer())
			{
				startProfilingDLL(true);
			}

			if (GC.getLogging() && GC.getGame().isDebugMode())
			{
				char szOut[1024];
				sprintf(szOut, "Player %d Turn OFF\n", getID());
				gDLL->messageControlLog(szOut);
			}

			if (getID() == GC.getGame().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setForcePopup(false);
				gDLL->getInterfaceIFace()->clearQueuedPopups();
				gDLL->getInterfaceIFace()->flushTalkingHeadMessages();
			}

			GC.getGame().changeNumGameTurnActive(-1);

			if (GC.getGame().getGameTurn() > 10)
			{  // Calvitix, problem on Scenario creation
				bFinancialTrouble = AI_isFinancialTrouble();
			}
			if (bDoTurn)
			{
				PROFILE("CvPlayer::setTurnActive.SetInactive.doTurn");

#ifdef USE_UNIT_TENDERING
				//const PlayerTypes eOwner = getOwner();
				//CvPlayerAI& player = GET_PLAYER(eOwner);
				if (isAlive() && !isHumanPlayer() &&!bFinancialTrouble)
				{
					getContractBroker().finalizeTenderContracts();
				}
#endif
				if (!GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
				{
					if ((GC.getGame().isPbem() || GC.getGame().isHotSeat()) && isHumanPlayer() && GC.getGame().countHumanPlayersAlive() > 1)
					{
						GC.getGame().setHotPbemBetweenTurns(true);
					}

					if (!GC.getGame().isSimultaneousTeamTurns())
					{
						for (int iI = (getID() + 1); iI < MAX_PLAYERS; iI++)
						{
							if (GET_PLAYER((PlayerTypes)iI).isAlive())
							{
								if (!GC.getGame().isPbem() || !GET_PLAYER((PlayerTypes)iI).isHumanPlayer())
								{
									GET_PLAYER((PlayerTypes)iI).setTurnActive(true);
								}
								else if (!GC.getGame().getPbemTurnSent())
								{
									gDLL->sendPbemTurn((PlayerTypes)iI);
								}
								break;
							}
						}
					}
					else if (!GET_TEAM(getTeam()).isTurnActive())
					{
						for (int iI = (getTeam() + 1); iI < MAX_TEAMS; iI++)
						{
							if (GET_TEAM((TeamTypes)iI).isAlive())
							{
								GET_TEAM((TeamTypes)iI).setTurnActive(true);
								break;
							}
						}
					}
				}
			}
		}
		gDLL->getInterfaceIFace()->updateCursorType();
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);

		GC.getMap().invalidateActivePlayerPlotCache();
	}
}


void CvPlayer::addPlotDangerSource(const CvPlot* pPlot, int iStrength)
{
	if (iStrength != 0)
	{
		PROFILE_FUNC();

		//	Radiate danger to the immediate vicinity
		for (int iDX = -2; iDX <= 2; iDX++)
		{
			for (int iDY = -2; iDY <= 2; iDY++)
			{
				CvPlot* pLoopPlot = plotXY(pPlot->getX(), pPlot->getY(), iDX, iDY);

				if (pLoopPlot && pLoopPlot->getArea() == pPlot->getArea())
				{
					int	iStepDistance = std::max(abs(iDX), abs(iDY));

					pLoopPlot->setDangerCount(m_eID, pLoopPlot->getDangerCount(m_eID) + iStrength / (1 << iStepDistance));
				}
			}
		}
	}
}

bool CvPlayer::isAutoMoves() const
{
	return m_bAutoMoves;
}


void CvPlayer::setAutoMoves(bool bAutoMoves)
{
	PROFILE_FUNC();

	if (m_bAutoMoves != bAutoMoves)
	{
		m_bAutoMoves = bAutoMoves;

		if (!bAutoMoves && (isEndTurn() || !isHumanPlayer() && !hasReadyUnit(true)))
		{
			setTurnActive(false);
		}
	}
}


/*DllExport*/ void CvPlayer::setEndTurn(bool bNewValue)
{
	if (bNewValue)
		OutputDebugString(CvString::format("Exe is ending player (%d) turn:\n", getID()).c_str());
	else OutputDebugString(CvString::format("Exe is starting player (%d) turn:\n", getID()).c_str());

	changeEndTurn(bNewValue);
}

void CvPlayer::changeEndTurn(const bool bNewValue, const bool bForce)
{
	PROFILE_EXTRA_FUNC();

	if (m_bEndTurn != bNewValue)
	{
		FAssertMsg(isTurnActive(), "isTurnActive is expected to be true");

		if (bNewValue)
		{
			if (!bForce && isHumanPlayer() && hasIdleCity())
			{
				CvCity* city = getIdleCity();
				if (city)
				{
					setForcedCityCycle(true);

					if (!getBugOptionBOOL("CityScreen__FullCityScreenOnEmptyBuildQueue", false))
					{
						gDLL->getInterfaceIFace()->addSelectedCity(city, false);
						GC.getCurrentViewport()->bringIntoView(city->getX(), city->getY());
					}
					else gDLL->getInterfaceIFace()->selectCity(city, true);
				}
				else
				{
					FErrorMsg("idleCity == NULL; fixing");
					resetIdleCities();
				}
				gDLL->getInterfaceIFace()->setEndTurnMessage(false);
				return;
			}

			foreach_(CvSelectionGroup* group, groups() | filtered(CvSelectionGroup::fn::getAutomateType() == AUTOMATE_SHADOW))
			{
				group->setForceUpdate(true);
				group->AI_update();
			}
			setAutoMoves(true);
		}
		else m_bForcedCityCycle = false;

		m_bEndTurn = bNewValue;
	}
}


bool CvPlayer::isForcedCityCycle() const
{
	return m_bForcedCityCycle || !getBugOptionBOOL("CityScreen__DelayCityCycleToEndOfTurn", false);
}

bool CvPlayer::isTurnDone() const
{
	// if this returns true, popups and diplomacy will wait to appear until next turn
	if (!GC.getGame().isPbem() && !GC.getGame().isHotSeat())
	{
		return false;
	}
	if (!isHumanPlayer())
	{
		return true;
	}
	if (!isEndTurn())
	{
		return false;
	}
	return (!isAutoMoves());
}

bool CvPlayer::isExtendedGame() const
{
	return m_bExtendedGame;
}


void CvPlayer::makeExtendedGame()
{
	m_bExtendedGame = true;
}


bool CvPlayer::isFoundedFirstCity() const
{
	return m_bFoundedFirstCity;
}


void CvPlayer::setFoundedFirstCity(bool bNewValue)
{
	if (isFoundedFirstCity() != bNewValue)
	{
		m_bFoundedFirstCity = bNewValue;

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(PercentButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(ResearchButtons_DIRTY_BIT, true);
		}
	}
}


bool CvPlayer::isStrike() const
{
	return m_bStrike;
}


void CvPlayer::setStrike(bool bNewValue)
{
	if (isStrike() != bNewValue)
	{
		m_bStrike = bNewValue;

		if (isStrike() && getID() == GC.getGame().getActivePlayer())
		{
			AddDLLMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_MISC_UNITS_ON_STRIKE").GetCString(), "AS2D_STRIKE", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));

			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

PlayerTypes CvPlayer::getID() const
{
	return m_eID;
}


HandicapTypes CvPlayer::getHandicapType() const
{
	return GC.getInitCore().getHandicap(getID());
}


CivilizationTypes CvPlayer::getCivilizationType() const
{
	return GC.getInitCore().getCiv(getID());
}


LeaderHeadTypes CvPlayer::getLeaderType() const
{
	return GC.getInitCore().getLeader(getID());
}


LeaderHeadTypes CvPlayer::getPersonalityType() const
{
	return m_ePersonalityType;
}


void CvPlayer::setPersonalityType(LeaderHeadTypes eNewValue)
{
	m_ePersonalityType = eNewValue;
}


EraTypes CvPlayer::getCurrentEra() const
{
	return m_eCurrentEra;
}


void CvPlayer::setCurrentEra(EraTypes eNewValue)
{
	PROFILE_EXTRA_FUNC();

	if (getCurrentEra() != eNewValue)
	{
		EraTypes eOldEra = m_eCurrentEra;
		m_eCurrentEra = eNewValue;

		// Toffer - Heritage may change commerce output with era.
		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			changeExtraCommerce100((CommerceTypes)iI, getHeritageCommerceEraChange((CommerceTypes)iI, eNewValue));
		}

		if (GC.getGame().getActiveTeam() != NO_TEAM)
		{
			for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
			{
				CvPlot* plotX = GC.getMap().plotByIndex(iI);
				plotX->updateGraphicEra();

				if (plotX->getRevealedImprovementType(GC.getGame().getActiveTeam(), true) != NO_IMPROVEMENT
				&& (plotX->getOwner() == getID() || !plotX->isOwned() && getID() == GC.getGame().getActivePlayer()))
				{
					plotX->setLayoutDirty(true);
				}
			}
		}

		// dirty all of this player's cities...
		foreach_ (CvCity* city, cities())
		{
			//TB Era Advance Free Specialist Type
			for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
			{
				if (getEraAdvanceFreeSpecialistCount((SpecialistTypes)iI) > 0)
				{
					city->changeFreeSpecialistCount((SpecialistTypes)iI, getEraAdvanceFreeSpecialistCount((SpecialistTypes)iI), true);
				}
			}
			city->setLayoutDirty(true);
		}

		//update unit eras
		foreach_ (CvUnit* unit, units())
		{
			if (!unit->isUsingDummyEntities())
			{
				gDLL->getEntityIFace()->updateGraphicEra(unit->getUnitEntity(), eOldEra);
			}
		}

		//update flag eras
		gDLL->getInterfaceIFace()->setDirty(Flag_DIRTY_BIT, true);

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(Soundtrack_DIRTY_BIT, true);
		}

		if (isHumanPlayer()
		&& getCurrentEra() != GC.getGame().getStartEra()
		&& !GC.getGame().isNetworkMultiPlayer()
		&& GC.getGame().isFinalInitialized()
		&& !gDLL->GetWorldBuilderMode())
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
			if (pInfo)
			{
				pInfo->setData1(eNewValue);
				pInfo->setText(L"showEraMovie");
				addPopup(pInfo);
			}
		}
	}
}


ReligionTypes CvPlayer::getLastStateReligion() const
{
	return m_eLastStateReligion;
}


ReligionTypes CvPlayer::getStateReligion() const
{
	return ((isStateReligion()) ? getLastStateReligion() : NO_RELIGION);
}


void CvPlayer::setLastStateReligion(const ReligionTypes eNewReligion)
{
	PROFILE_EXTRA_FUNC();
	const ReligionTypes eOldReligion = m_eLastStateReligion;

	if (eOldReligion != eNewReligion)
	{
		m_eLastStateReligion = eNewReligion;

		updateReligionHappiness();
		updateReligionCommerce();

		GC.getGame().updateSecretaryGeneral();
		GC.getGame().AI_makeAssignWorkDirty();

		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);

		if (GC.getGame().isFinalInitialized())
		{
			if (gDLL->isDiplomacy() && (gDLL->getDiplomacyPlayer() == getID()))
			{
				gDLL->updateDiplomacyAttitude(true);
			}

			// Silence announcement for civs who are not alive, ie rebels who may not be born
			if (!isNPC() && isAlive() && eNewReligion != NO_RELIGION)
			{
				for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
				{
					const CvPlayer& playerX = GET_PLAYER((PlayerTypes)iI);

					if (playerX.isAlive() && playerX.isHumanPlayer() && GET_TEAM(getTeam()).isHasMet(playerX.getTeam()))
					{
						AddDLLMessage(
							(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(),
							gDLL->getText(
								"TXT_KEY_MISC_PLAYER_CONVERT_RELIGION",
								getNameKey(), GC.getReligionInfo(eNewReligion).getTextKeyWide()
							)
							, "AS2D_RELIGION_CONVERT", MESSAGE_TYPE_MAJOR_EVENT
						);
					}
				}
				GC.getGame().addReplayMessage(
					REPLAY_MESSAGE_MAJOR_EVENT, getID(),
					gDLL->getText(
						"TXT_KEY_MISC_PLAYER_CONVERT_RELIGION",
						getNameKey(), GC.getReligionInfo(eNewReligion).getTextKeyWide()
					)
				);
			}

			// Python Event
			CvEventReporter::getInstance().playerChangeStateReligion(getID(), eNewReligion, eOldReligion);

			// Attitude cache
			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				CvPlayerAI& playerAI_X = GET_PLAYER((PlayerTypes)iI);

				if (playerAI_X.isAlive() && playerAI_X.getStateReligion() != NO_RELIGION)
				{
					AI_invalidateAttitudeCache((PlayerTypes)iI);
					playerAI_X.AI_invalidateAttitudeCache(getID());
				}
			}
		}

		clearCanConstructCache(NO_BUILDING, true);
	}
}


PlayerTypes CvPlayer::getParent() const
{
	return m_eParent;
}

void CvPlayer::setParent(PlayerTypes eParent)
{
	// Attitude cache
	if (m_eParent != eParent)
	{
		AI_invalidateAttitudeCache(eParent);
	}
	m_eParent = eParent;
}

TeamTypes CvPlayer::getTeam() const
{
	return m_eTeamType;
}

void CvPlayer::updateTeamType()
{
	if(getID() == NO_PLAYER)
	{
		m_eTeamType = NO_TEAM;
	}
	else
	{
		m_eTeamType = GC.getInitCore().getTeam(getID());
	}
}

void CvPlayer::setTeam(TeamTypes eTeam)
{
	PROFILE_EXTRA_FUNC();
	FAssert(eTeam != NO_TEAM);

	TeamTypes eOldTeam = getTeam();

	FAssert(eOldTeam != NO_TEAM);

	if (isAlive())
	{
		GET_TEAM(eOldTeam).changeAliveCount(-1);
	}
	if (isEverAlive())
	{
		GET_TEAM(eOldTeam).changeEverAliveCount(-1);
	}
	GET_TEAM(eOldTeam).changeNumMembers(-1);
	GET_TEAM(eOldTeam).changeNumCities(-(getNumCities()));
	GET_TEAM(eOldTeam).changeTotalPopulation(-(getTotalPopulation()));
	GET_TEAM(eOldTeam).changeTotalLand(-(getTotalLand()));

	GC.getInitCore().setTeam(getID(), eTeam);

	if (isAlive())
	{
		GET_TEAM(eTeam).changeAliveCount(1);
	}
	if (isEverAlive())
	{
		GET_TEAM(eTeam).changeEverAliveCount(1);
	}
	GET_TEAM(eTeam).changeNumMembers(1);
	GET_TEAM(eTeam).changeNumCities(getNumCities());
	GET_TEAM(eTeam).changeTotalPopulation(getTotalPopulation());
	GET_TEAM(eTeam).changeTotalLand(getTotalLand());

	CvEventReporter::getInstance().changeTeam(eOldTeam, eTeam);

	// Attitude cache
	if (GC.getGame().isFinalInitialized())
	{
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				AI_invalidateAttitudeCache((PlayerTypes)iI);
				GET_PLAYER((PlayerTypes)iI).AI_invalidateAttitudeCache(getID());
			}
		}
	}
}


bool CvPlayer::isDoNotBotherStatus(PlayerTypes playerID) const
{
	return m_bDoNotBotherStatus == playerID;
}


PlayerColorTypes CvPlayer::getPlayerColor() const
{
	return GC.getInitCore().getColor(getID());
}


int CvPlayer::getPlayerTextColorR() const
{
	FAssertMsg(getPlayerColor() != NO_PLAYERCOLOR, "getPlayerColor() is not expected to be equal with NO_PLAYERCOLOR");
	return ((int)(GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(getPlayerColor()).getTextColorType()).getColor().r * 255));
}


int CvPlayer::getPlayerTextColorG() const
{
	FAssertMsg(getPlayerColor() != NO_PLAYERCOLOR, "getPlayerColor() is not expected to be equal with NO_PLAYERCOLOR");
	return ((int)(GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(getPlayerColor()).getTextColorType()).getColor().g * 255));
}


int CvPlayer::getPlayerTextColorB() const
{
	FAssertMsg(getPlayerColor() != NO_PLAYERCOLOR, "getPlayerColor() is not expected to be equal with NO_PLAYERCOLOR");
	return ((int)(GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(getPlayerColor()).getTextColorType()).getColor().b * 255));
}


int CvPlayer::getPlayerTextColorA() const
{
	FAssertMsg(getPlayerColor() != NO_PLAYERCOLOR, "getPlayerColor() is not expected to be equal with NO_PLAYERCOLOR");
	return ((int)(GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(getPlayerColor()).getTextColorType()).getColor().a * 255));
}


int CvPlayer::getSeaPlotYield(YieldTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	return m_aiSeaPlotYield[eIndex];
}


void CvPlayer::changeSeaPlotYield(YieldTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiSeaPlotYield[eIndex] += iChange;

		updateYield();
	}
}


	//Team Project (7)
int CvPlayer::getGoldenAgeYield(YieldTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	return std::max(0,m_aiGoldenAgeYield[eIndex]);
}


void CvPlayer::changeGoldenAgeYield(YieldTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiGoldenAgeYield[eIndex] += iChange;

		updateYield();
	}
}


int CvPlayer::getGoldenAgeCommerce(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return std::max(0,m_aiGoldenAgeCommerce[eIndex]);
}


void CvPlayer::changeGoldenAgeCommerce(CommerceTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiGoldenAgeCommerce[eIndex] += iChange;

		setCommerceDirty(eIndex);
	}
}


int CvPlayer::getYieldRateModifier(YieldTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	return m_aiYieldRateModifier[eIndex];
}


void CvPlayer::changeYieldRateModifier(YieldTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiYieldRateModifier[eIndex] += iChange;

		invalidateYieldRankCache(eIndex);

		if (eIndex == YIELD_COMMERCE)
		{
			setCommerceDirty();
		}
		AI_makeAssignWorkDirty();

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getCapitalYieldRateModifier(YieldTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	return m_aiCapitalYieldRateModifier[eIndex];
}


void CvPlayer::changeCapitalYieldRateModifier(YieldTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiCapitalYieldRateModifier[eIndex] += iChange;

		invalidateYieldRankCache(eIndex);

		CvCity* pCapitalCity = getCapitalCity();

		if (pCapitalCity)
		{
			if (eIndex == YIELD_COMMERCE)
			{
				pCapitalCity->updateCommerce();
			}

			pCapitalCity->AI_setAssignWorkDirty(true);

			if (pCapitalCity->getTeam() == GC.getGame().getActiveTeam())
			{
				pCapitalCity->setInfoDirty(true);
			}
		}
	}
}


int CvPlayer::getExtraYieldThreshold(YieldTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	return m_aiExtraYieldThreshold[eIndex];
}


void CvPlayer::updateExtraYieldThreshold(YieldTypes eIndex)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);

	int iBestValue = 0;

	FAssertMsg((GC.getNumTraitInfos() > 0), "GC.getNumTraitInfos() is less than or equal to zero but is expected to be larger than zero in CvPlayer::updateExtraYieldThreshold");
	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			if (GC.getTraitInfo((TraitTypes) iI).getExtraYieldThreshold(eIndex) > 0)
			{
				if ((iBestValue == 0) || (GC.getTraitInfo((TraitTypes) iI).getExtraYieldThreshold(eIndex) < iBestValue))
				{
					iBestValue = GC.getTraitInfo((TraitTypes) iI).getExtraYieldThreshold(eIndex);
				}
			}
		}
	}

	if (getExtraYieldThreshold(eIndex) != iBestValue)
	{
		m_aiExtraYieldThreshold[eIndex] = iBestValue;
		FASSERT_NOT_NEGATIVE(getExtraYieldThreshold(eIndex));

		updateYield();
	}
}


int CvPlayer::getLessYieldThreshold(YieldTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	return m_aiLessYieldThreshold[eIndex];
}


void CvPlayer::updateLessYieldThreshold(YieldTypes eIndex)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);

	int iWorstValue = 0;

	FAssertMsg((GC.getNumTraitInfos() > 0), "GC.getNumTraitInfos() is less than or equal to zero but is expected to be larger than zero in CvPlayer::updateExtraYieldThreshold");
	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			if (GC.getTraitInfo((TraitTypes) iI).getLessYieldThreshold(eIndex) > 0)
			{
				if ((iWorstValue == 0) || (GC.getTraitInfo((TraitTypes) iI).getLessYieldThreshold(eIndex) < iWorstValue))
				{
					iWorstValue = GC.getTraitInfo((TraitTypes) iI).getLessYieldThreshold(eIndex);
				}
			}
		}
	}

	if (getLessYieldThreshold(eIndex) != iWorstValue)
	{
		m_aiLessYieldThreshold[eIndex] = iWorstValue;
		FASSERT_NOT_NEGATIVE(getLessYieldThreshold(eIndex));

		updateYield();
	}
}


int CvPlayer::getTradeYieldModifier(YieldTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	return m_aiTradeYieldModifier[eIndex];
}


void CvPlayer::changeTradeYieldModifier(YieldTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiTradeYieldModifier[eIndex] += iChange;

		updateTradeRoutes();
	}
}


int CvPlayer::getExtraCommerce100(const CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	return m_extraCommerce[eIndex];
}


void CvPlayer::changeExtraCommerce100(const CommerceTypes eIndex, const int iChange)
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (iChange != 0)
	{
		m_extraCommerce[eIndex] += iChange;
		setCommerceDirty(eIndex);
	}
}


int CvPlayer::getCommercePercent(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_aiCommercePercent[eIndex];
}


void CvPlayer::setCommercePercent(CommerceTypes eIndex, int iNewValue)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	const int iOldValue = m_aiCommercePercent[eIndex];

	m_aiCommercePercent[eIndex] = range(iNewValue, 0, 100);

	if (iOldValue != m_aiCommercePercent[eIndex])
	{
		int iTotalCommercePercent = 0;

		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			iTotalCommercePercent += getCommercePercent((CommerceTypes)iI);
		}

		for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			if (iJ != eIndex)
			{
				if (iTotalCommercePercent == 100)
				{
					break;
				}
				const int iAdjustment = std::min(m_aiCommercePercent[iJ], iTotalCommercePercent - 100);
				m_aiCommercePercent[iJ] -= iAdjustment;
				iTotalCommercePercent -= iAdjustment;
			}
		}
		FAssert(100 == iTotalCommercePercent);

		setCommerceDirty();

		AI_makeAssignWorkDirty();

		if (getTeam() == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(Financial_Screen_DIRTY_BIT, true);
		}
	}
}


void CvPlayer::changeCommercePercent(CommerceTypes eIndex, int iChange)
{
	setCommercePercent(eIndex, getCommercePercent(eIndex) + iChange);
}

int CvPlayer::getCommerceRate(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (m_abCommerceDirty[eIndex])
	{
		updateCommerce(eIndex, false);
	}
	return m_aiCommerceRate[eIndex] / 100;
}

int CvPlayer::getTotalCityBaseCommerceRate(CommerceTypes eIndex) const
{
	PROFILE_FUNC();

	if (m_cachedTotalCityBaseCommerceRate[eIndex] == MAX_INT)
	{
		m_cachedTotalCityBaseCommerceRate[eIndex] = algo::accumulate(cities() | transformed(CvCity::fn::getBaseCommerceRateTimes100(eIndex)), 0) / 100;
		if (m_cachedTotalCityBaseCommerceRate[eIndex] < MIN_TOL_FALSE_ACCUMULATE)
			m_cachedTotalCityBaseCommerceRate[eIndex] = MAX_COMMERCE_RATE_VALUE;
	}
	return m_cachedTotalCityBaseCommerceRate[eIndex];
}

void CvPlayer::changeCommerceRate(CommerceTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiCommerceRate[eIndex] = std::min(m_aiCommerceRate[eIndex]+iChange,MAX_COMMERCE_RATE_VALUE);
		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

int CvPlayer::getCommerceRateModifier(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_aiCommerceRateModifier[eIndex];
}


void CvPlayer::changeCommerceRateModifier(CommerceTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiCommerceRateModifier[eIndex] = std::min(m_aiCommerceRateModifier[eIndex] + iChange, MAX_COMMERCE_RATE_MODIFIER_VALUE);

		setCityCommerceModifierDirty(eIndex);
		setCommerceDirty(eIndex);

		AI_makeAssignWorkDirty();
	}
}

int CvPlayer::getCommerceRateModifierfromEvents(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_aiCommerceRateModifierfromEvents[eIndex];
}

void CvPlayer::changeCommerceRateModifierfromEvents(CommerceTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (iChange != 0)
	{
		//Totals into Events to split for display
		m_aiCommerceRateModifierfromEvents[eIndex] = std::min(m_aiCommerceRateModifierfromEvents[eIndex] + iChange, MAX_COMMERCE_RATE_MODIFIER_VALUE);
		//Also totals into generic Rate Modifier total
		m_aiCommerceRateModifier[eIndex] = std::min(m_aiCommerceRateModifier[eIndex] + iChange, MAX_COMMERCE_RATE_MODIFIER_VALUE);

		setCityCommerceModifierDirty(eIndex);
		setCommerceDirty(eIndex);

		AI_makeAssignWorkDirty();
	}
}

int CvPlayer::getCommerceRateModifierfromBuildings(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_aiCommerceRateModifierfromBuildings[eIndex];
}

void CvPlayer::changeCommerceRateModifierfromBuildings(CommerceTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (iChange != 0)
	{
		//Totals into Buildings to split for display
		m_aiCommerceRateModifierfromBuildings[eIndex] = std::min(m_aiCommerceRateModifierfromBuildings[eIndex] + iChange, MAX_COMMERCE_RATE_MODIFIER_VALUE);
		//Also totals into generic Rate Modifier total
		m_aiCommerceRateModifier[eIndex] = std::min(m_aiCommerceRateModifier[eIndex] + iChange, MAX_COMMERCE_RATE_MODIFIER_VALUE);

		setCityCommerceModifierDirty(eIndex);
		setCommerceDirty(eIndex);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getCapitalCommerceRateModifier(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_aiCapitalCommerceRateModifier[eIndex];
}


void CvPlayer::changeCapitalCommerceRateModifier(CommerceTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiCapitalCommerceRateModifier[eIndex] += iChange;

		CvCity* pCapitalCity = getCapitalCity();

		if (pCapitalCity)
		{
			pCapitalCity->setCommerceModifierDirty(eIndex);

			pCapitalCity->AI_setAssignWorkDirty(true);
		}
	}
}


int CvPlayer::getStateReligionBuildingCommerce(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_aiStateReligionBuildingCommerce[eIndex];
}


void CvPlayer::changeStateReligionBuildingCommerce(CommerceTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiStateReligionBuildingCommerce[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(getStateReligionBuildingCommerce(eIndex));

		setCommerceDirty(eIndex);
	}
}


int CvPlayer::getSpecialistExtraCommerce(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_aiSpecialistExtraCommerce[eIndex];
}


void CvPlayer::changeSpecialistExtraCommerce(CommerceTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiSpecialistExtraCommerce[eIndex] += iChange;

		setCommerceDirty(eIndex);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getCommerceFlexibleCount(CommerceTypes eIndex) const
{
	return m_aiCommerceFlexibleCount[eIndex];
}


bool CvPlayer::isCommerceFlexible(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (!isFoundedFirstCity() || eIndex == COMMERCE_ESPIONAGE && 0 == GET_TEAM(getTeam()).getHasMetCivCount(true))
	{
		return false;
	}
	return GC.getCommerceInfo(eIndex).isFlexiblePercent() || getCommerceFlexibleCount(eIndex) > 0 || GET_TEAM(getTeam()).isCommerceFlexible(eIndex);
}


void CvPlayer::changeCommerceFlexibleCount(CommerceTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiCommerceFlexibleCount[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(getCommerceFlexibleCount(eIndex));

		if (!isCommerceFlexible(eIndex))
		{
			setCommercePercent(eIndex, 0);
		}

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(PercentButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getGoldPerTurnByPlayer(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	return m_aiGoldPerTurnByPlayer[eIndex];
}


void CvPlayer::changeGoldPerTurnByPlayer(PlayerTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);

	if (iChange != 0)
	{
		m_iGoldPerTurn = std::min(m_iGoldPerTurn + iChange, MAX_GOLD_PER_TURN_VALUE);
		m_aiGoldPerTurnByPlayer[eIndex] = std::min(m_aiGoldPerTurnByPlayer[eIndex] + iChange, MAX_GOLD_PER_TURN_VALUE);

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}

		if (!isHumanPlayer())
		{
			AI_doCommerce();
		}
	}
}


bool CvPlayer::isFeatAccomplished(FeatTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_FEAT_TYPES, eIndex);
	return m_abFeatAccomplished[eIndex];
}


void CvPlayer::setFeatAccomplished(FeatTypes eIndex, bool bNewValue)
{
	FASSERT_BOUNDS(0, NUM_FEAT_TYPES, eIndex);
	m_abFeatAccomplished[eIndex] = bNewValue;
}


bool CvPlayer::isOption(PlayerOptionTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_PLAYEROPTION_TYPES, eIndex);
	return m_abOptions[eIndex];
}


void CvPlayer::setOption(PlayerOptionTypes eIndex, bool bNewValue)
{
	FASSERT_BOUNDS(0, NUM_PLAYEROPTION_TYPES, eIndex);
	m_abOptions[eIndex] = bNewValue;
}

bool CvPlayer::isPlayable() const
{
	return GC.getInitCore().getPlayableCiv(getID());
}

void CvPlayer::setPlayable(bool bNewValue)
{
	GC.getInitCore().setPlayableCiv(getID(), bNewValue);
}


void CvPlayer::changeBonusExport(const BonusTypes eBonus, const int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eBonus);
	FAssertMsg(iChange != 0, "This is not a change!");

	std::map<short, uint32_t>::const_iterator itr = m_bonusExport.find((short)eBonus);

	if (itr == m_bonusExport.end())
	{
		if (iChange > 0)
		{
			m_bonusExport.insert(std::make_pair((short)eBonus, iChange));
		}
		else FErrorMsg("Expected positive iChange for first unit of a kind");
	}
	else if (iChange < 0 && (int)(itr->second) <= -iChange)
	{
		FAssertMsg((int)(itr->second) >= -iChange, "This change would bring the count to a negative value! Code copes with it though")
		m_bonusExport.erase(itr->first);
	}
	else // change bonus count
	{
		m_bonusExport[itr->first] += iChange;
	}
	const CvCity* pCapitalCity = getCapitalCity();
	if (pCapitalCity)
	{
		CvPlotGroup* ownerPlotGroup = pCapitalCity->plotGroup(getID());
		if (ownerPlotGroup)
		{
			ownerPlotGroup->changeNumBonuses(eBonus, -iChange);
		}
	}
}

void CvPlayer::changeBonusImport(const BonusTypes eBonus, const int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eBonus);
	FAssertMsg(iChange != 0, "This is not a change!");

	std::map<short, uint32_t>::const_iterator itr = m_bonusImport.find((short)eBonus);

	if (itr == m_bonusImport.end())
	{
		if (iChange > 0)
		{
			m_bonusImport.insert(std::make_pair((short)eBonus, iChange));
		}
		else FErrorMsg("Expected positive iChange for first unit of a kind");
	}
	else if (iChange < 0 && (int)(itr->second) <= -iChange)
	{
		FAssertMsg((int)(itr->second) >= -iChange, "This change would bring the count to a negative value! Code copes with it though")
		m_bonusImport.erase(itr->first);
	}
	else // change bonus count
	{
		m_bonusImport[itr->first] += iChange;
	}
	const CvCity* pCapitalCity = getCapitalCity();
	if (pCapitalCity)
	{
		CvPlotGroup* ownerPlotGroup = pCapitalCity->plotGroup(getID());
		if (ownerPlotGroup)
		{
			ownerPlotGroup->changeNumBonuses(eBonus, iChange);
		}
	}
}

int CvPlayer::getBonusExport(const BonusTypes eBonus) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eBonus);
	std::map<short, uint32_t>::const_iterator itr = m_bonusExport.find((short)eBonus);
	return itr != m_bonusExport.end() ? itr->second : 0;
}

int CvPlayer::getBonusImport(const BonusTypes eBonus) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eBonus);
	std::map<short, uint32_t>::const_iterator itr = m_bonusImport.find((short)eBonus);
	return itr != m_bonusImport.end() ? itr->second : 0;
}


int CvPlayer::getImprovementCount(ImprovementTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), eIndex);
	return m_paiImprovementCount[eIndex];
}


void CvPlayer::changeImprovementCount(ImprovementTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), eIndex);
	m_paiImprovementCount[eIndex] += iChange;
	FASSERT_NOT_NEGATIVE(getImprovementCount(eIndex));
}


void CvPlayer::changeFreeBuildingCount(const BuildingTypes eIndex, const int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	FAssertMsg(iChange != 0, "This is not a change!");

	std::map<short, uint16_t>::const_iterator itr = m_freeBuildingCount.find((short)eIndex);

	if (itr == m_freeBuildingCount.end())
	{
		if (iChange > 0)
		{
			m_freeBuildingCount.insert(std::make_pair((short)eIndex, iChange));
			algo::for_each(cities(), CvCity::fn::setFreeBuilding(eIndex, true));
		}
		else FErrorMsg("Expected positive iChange for first building of a kind");
	}
	else if (iChange < 0 && (int)(itr->second) <= -iChange)
	{
		FAssertMsg((int)(itr->second) >= -iChange, "This change would bring the count to a negative value! Code copes with it though")
		m_freeBuildingCount.erase(itr->first);
		algo::for_each(cities(), CvCity::fn::setFreeBuilding(eIndex, false));
	}
	else // change building count
	{
		m_freeBuildingCount[itr->first] += iChange;
	}
}

uint16_t CvPlayer::getFreeBuildingCount(const BuildingTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	std::map<short, uint16_t>::const_iterator itr = m_freeBuildingCount.find((short)eIndex);
	return itr != m_freeBuildingCount.end() ? itr->second : 0;
}

void CvPlayer::checkFreeBuildings(CvCity* city)
{
	PROFILE_EXTRA_FUNC();
	for (std::map<short, uint16_t>::const_iterator itr = m_freeBuildingCount.begin(); itr != m_freeBuildingCount.end(); ++itr)
	{
		if (!city->isFreeBuilding((BuildingTypes)itr->first))
		{
			// This one will not register the free building if it is still not valid or if it was manually built.
			city->setFreeBuilding((BuildingTypes)itr->first, true);
		}
	}
}

// Used by newly settled cities to propagate the count in the area
uint16_t CvPlayer::getFreeAreaBuildingCount(BuildingTypes eIndex, const CvArea* area) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);

	foreach_(const CvCity* city, cities())
	{
		if (city->area() == area && city->getFreeAreaBuildingCount(eIndex) > 0)
		{
			return city->getFreeAreaBuildingCount(eIndex);
		}
	}
	return 0;
}

bool CvPlayer::isBuildingFree(BuildingTypes eIndex, const CvArea* area) const
{
	return getFreeBuildingCount(eIndex) > 0 || area && getFreeAreaBuildingCount(eIndex, area) > 0;
}


void CvPlayer::changeExtraBuildingHappiness(const BuildingTypes eIndex, const int iChange, const bool bLimited)
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	if (iChange == 0)
	{
		return;
	}
	std::map<short, int>::const_iterator itr = m_extraBuildingHappiness.find((short)eIndex);

	if (itr == m_extraBuildingHappiness.end())
	{
		m_extraBuildingHappiness.insert(std::make_pair((short)eIndex, iChange));
	}
	else if (itr->second == -iChange)
	{
		m_extraBuildingHappiness.erase(itr->first);
	}
	else // change happy count
	{
		m_extraBuildingHappiness[itr->first] += iChange;
	}
	algo::for_each(cities(), CvCity::fn::updateExtraBuildingHappiness(bLimited));
}

int CvPlayer::getExtraBuildingHappiness(const BuildingTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	std::map<short, int>::const_iterator itr = m_extraBuildingHappiness.find((short)eIndex);
	return itr != m_extraBuildingHappiness.end() ? itr->second : 0;
}


void CvPlayer::changeExtraBuildingHealth(const BuildingTypes eIndex, const int iChange, const bool bLimited)
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	if (iChange == 0)
	{
		return;
	}
	std::map<short, int>::const_iterator itr = m_extraBuildingHealth.find((short)eIndex);

	if (itr == m_extraBuildingHealth.end())
	{
		m_extraBuildingHealth.insert(std::make_pair((short)eIndex, iChange));
	}
	else if (itr->second == -iChange)
	{
		m_extraBuildingHealth.erase(itr->first);
	}
	else // change health count
	{
		m_extraBuildingHealth[itr->first] += iChange;
	}
	algo::for_each(cities(), CvCity::fn::updateExtraBuildingHealth(bLimited));
}

int CvPlayer::getExtraBuildingHealth(const BuildingTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	std::map<short, int>::const_iterator itr = m_extraBuildingHealth.find((short)eIndex);
	return itr != m_extraBuildingHealth.end() ? itr->second : 0;
}


int CvPlayer::getFeatureHappiness(FeatureTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), eIndex);
	return m_paiFeatureHappiness[eIndex];
}


/********************************************************************************/
/* 	New Civic AI						02.08.2010				Fuyu			*/
/********************************************************************************/
//Fuyu bLimited
void CvPlayer::changeFeatureHappiness(FeatureTypes eIndex, int iChange, bool bLimited)
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), eIndex);

	if (iChange != 0)
	{
		m_paiFeatureHappiness[eIndex] += iChange;

		updateFeatureHappiness(bLimited);
	}
}
/********************************************************************************/
/* 	New Civic AI												END 			*/
/********************************************************************************/


bool CvPlayer::isUnitMaxedOut(const UnitTypes eIndex, const int iExtra) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eIndex);

	// NPC's are not organized as nations so they shouldn't respect national limits.
	if (!isNationalUnit(eIndex) || isNPC())
	{
		return false;
	}
	if (GC.getGame().isOption(GAMEOPTION_NO_NATIONAL_UNIT_LIMIT)
	&& !GC.getUnitInfo(eIndex).isUnlimitedException())
	{
		return false;
	}
	/* Toffer: FAssertMsg(GC.getUnitInfo(eIndex).getMaxPlayerInstances() == 0 ...
	Special assert exception rule for the initial settler unit that is never trainable.
	Settler units are trainable even when their hammer cost is set to -1 due to their unique hammer cost calculation. */
	FAssertMsg(GC.getUnitInfo(eIndex).getMaxPlayerInstances() == 0 || getUnitCount(eIndex) <= GC.getUnitInfo(eIndex).getMaxPlayerInstances(),
		CvString::format("getUnitCount=%d is expected not to be greater than MaxPlayerInstances=%d for %s",
		getUnitCount(eIndex), GC.getUnitInfo(eIndex).getMaxPlayerInstances(), GC.getUnitInfo(eIndex).getType()).c_str());


//    this scale will be used for national units only... the idea is to start them 5 at prehistoric and scale by 5 per age...
//     as an alternative to the 10% per unit cost increase but unlimited
//     might have to use separate indicator but this will do for now for testing purposes
   int iMaxUnits = GC.getUnitInfo(eIndex).getMaxPlayerInstances();

   if (iMaxUnits == 5)
   {
   	int iEra = getCurrentEra(); // 0 = Prehistoric, 1 = Ancient, etc.

   	if (iEra > 0)
   	{
   		iMaxUnits += iEra * 5;
   	}
   }

   return (getUnitCount(eIndex) + iExtra) >= iMaxUnits;
}


void CvPlayer::recalculateUnitCounts()
{
	PROFILE_FUNC();

	foreach_(CvUnit* unit, units())
	{
		unit->recalculateUnitUpkeep();
		unit->area()->changePower(getID(), unit->getPowerValueTotal());
	}
}

void CvPlayer::changeUnitCount(const UnitTypes eUnit, const int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);
	FAssertMsg(iChange != 0, "This is not a change!");

	std::map<short, uint32_t>::const_iterator itr = m_unitCount.find((short)eUnit);

	if (itr == m_unitCount.end())
	{
		if (iChange > 0)
		{
			m_unitCount.insert(std::make_pair((short)eUnit, iChange));
		}
		else FErrorMsg("Expected positive iChange for first unit of a kind");
	}
	else if (iChange < 0 && (int)(itr->second) <= -iChange)
	{
		FAssertMsg((int)(itr->second) >= -iChange, "This change would bring the count to a negative value! Code copes with it though")
		m_unitCount.erase(itr->first);
	}
	else // change unit count
	{
		m_unitCount[itr->first] += iChange;
	}
	GET_TEAM(getTeam()).changeUnitCount(eUnit, iChange);
}

int CvPlayer::getUnitCount(const UnitTypes eUnit) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);
	std::map<short, uint32_t>::const_iterator itr = m_unitCount.find((short)eUnit);
	return itr != m_unitCount.end() ? itr->second : 0;
}

void CvPlayer::changeUnitCountSM(const UnitTypes eUnit, const int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);
	FAssertMsg(iChange != 0, "This is not a change!");

	std::map<short, uint32_t>::const_iterator itr = m_unitCountSM.find((short)eUnit);

	if (itr == m_unitCountSM.end())
	{
		if (iChange > 0)
		{
			m_unitCountSM.insert(std::make_pair((short)eUnit, iChange));
		}
		else FErrorMsg("Expected positive iChange for first unit of a kind");
	}
	else if (iChange < 0 && (int)(itr->second) <= -iChange)
	{
		FAssertMsg((int)(itr->second) >= -iChange, "This change would bring the count to a negative value! Code copes with it though")
		m_unitCountSM.erase(itr->first);
	}
	else // change unit count
	{
		m_unitCountSM[itr->first] += iChange;
	}
}

int CvPlayer::getUnitCountSM(const UnitTypes eUnit) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);
	std::map<short, uint32_t>::const_iterator itr = m_unitCountSM.find((short)eUnit);
	return itr != m_unitCountSM.end() ? itr->second / intPow(3, GC.getUnitInfo(eUnit).getBaseGroupRank() - 1) : 0;
}


void CvPlayer::changeUnitMaking(const UnitTypes eUnit, const int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);
	FAssertMsg(iChange != 0, "This is not a change!");

	std::map<short, uint32_t>::const_iterator itr = m_unitMaking.find((short)eUnit);

	if (itr == m_unitMaking.end())
	{
		if (iChange > 0)
		{
			m_unitMaking.insert(std::make_pair((short)eUnit, iChange));
		}
		else FErrorMsg("Expected positive iChange for first unit of a kind");
	}
	else if (iChange < 0 && (int)(itr->second) <= -iChange)
	{
		FAssertMsg((int)(itr->second) >= -iChange, "This change would bring the count to a negative value! Code copes with it though")
		m_unitMaking.erase(itr->first);
	}
	else // change unit count
	{
		m_unitMaking[itr->first] += iChange;
	}
}


int CvPlayer::getUnitMaking(const UnitTypes eUnit) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);
	std::map<short, uint32_t>::const_iterator itr = m_unitMaking.find((short)eUnit);
	return itr != m_unitMaking.end() ? itr->second : 0;
}


int CvPlayer::getUnitCountPlusMaking(const UnitTypes eIndex) const
{
	return getUnitCount(eIndex) + getUnitMaking(eIndex);
}


void CvPlayer::changeBuildingCount(BuildingTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	m_paiBuildingCount[eIndex] += iChange;
	FASSERT_NOT_NEGATIVE(getBuildingCount(eIndex));

	clearCanConstructCache(eIndex, true);
}

int CvPlayer::getBuildingCount(BuildingTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);

	if (m_paiBuildingCount[eIndex] < 0)
		return 0;

	return m_paiBuildingCount[eIndex];
}


void CvPlayer::changeBuildingMaking(const BuildingTypes eIndex, const int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	FAssertMsg(iChange != 0, "This is not a change!");

	std::map<short, uint32_t>::const_iterator itr = m_buildingMaking.find((short)eIndex);

	if (itr == m_buildingMaking.end())
	{
		if (iChange > 0)
		{
			m_buildingMaking.insert(std::make_pair((short)eIndex, iChange));
		}
		else FErrorMsg("Expected positive iChange for first building of a kind");
	}
	else if (iChange < 0 && (int)(itr->second) <= -iChange)
	{
		FAssertMsg((int)(itr->second) >= -iChange, "This change would bring the count to a negative value! Code copes with it though")
		m_buildingMaking.erase(itr->first);
	}
	else // change building count
	{
		m_buildingMaking[itr->first] += iChange;
	}
	clearCanConstructCache(eIndex, true);
}

int CvPlayer::getBuildingMaking(const BuildingTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	std::map<short, uint32_t>::const_iterator itr = m_buildingMaking.find((short)eIndex);
	return itr != m_buildingMaking.end() ? itr->second : 0;
}


int CvPlayer::getBuildingCountPlusMaking(BuildingTypes eIndex) const
{
	return (getBuildingCount(eIndex) + getBuildingMaking(eIndex));
}


void CvPlayer::changeBuildingGroupCount(SpecialBuildingTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumSpecialBuildingInfos(), eIndex);
	m_paiBuildingGroupCount[eIndex] += iChange;
	FASSERT_NOT_NEGATIVE(getBuildingGroupCount(eIndex));

	clearCanConstructCacheForGroup(eIndex, true);
}

int CvPlayer::getBuildingGroupCount(SpecialBuildingTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialBuildingInfos(), eIndex);

	if (m_paiBuildingGroupCount[eIndex] < 0)
		return 0;

	return m_paiBuildingGroupCount[eIndex];
}

bool CvPlayer::isBuildingMaxedOut(BuildingTypes eIndex, int iExtra) const
{
	PROFILE_FUNC();

	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);

	if (!isNationalWonder(eIndex))
	{
		return false;
	}

	FAssertMsg(getBuildingCount(eIndex) <= (GC.getBuildingInfo(eIndex).getMaxPlayerInstances() + GC.getBuildingInfo(eIndex).getExtraPlayerInstances()),
		CvString::format("BuildingCount for %s is expected to be less than or match the number of max player instances plus extra player instances (%d <= %d + %d)",
			GC.getBuildingInfo(eIndex).getType(),
			getBuildingCount(eIndex),
			GC.getBuildingInfo(eIndex).getMaxPlayerInstances(),
			GC.getBuildingInfo(eIndex).getExtraPlayerInstances()
		).c_str()
	);

	return ((getBuildingCount(eIndex) + iExtra) >= (GC.getBuildingInfo(eIndex).getMaxPlayerInstances() + GC.getBuildingInfo(eIndex).getExtraPlayerInstances()));
}

bool CvPlayer::isBuildingGroupMaxedOut(SpecialBuildingTypes eIndex, int iExtra) const
{
	PROFILE_FUNC();

	FASSERT_BOUNDS(0, GC.getNumSpecialBuildingInfos(), eIndex);

	if (!isNationalWonderGroupSpecialBuilding(eIndex))
	{
		return false;
	}

	const int iLimit = GC.getSpecialBuildingInfo(eIndex).getMaxPlayerInstances();
	FAssertMsg(getBuildingGroupCount(eIndex) <= iLimit, "SpecialbuildingCount is expected to be less than or match the number of max player instances plus extra player instances");

	return ((getBuildingGroupCount(eIndex) + iExtra) >= iLimit);
}

void CvPlayer::changeBuildingGroupMaking(SpecialBuildingTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumSpecialBuildingInfos(), eIndex);

	if (iChange != 0)
	{
		m_paiBuildingGroupMaking[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(getBuildingGroupMaking(eIndex));

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(Help_DIRTY_BIT, true);
		}

		clearCanConstructCacheForGroup(eIndex, true);
	}
}

int CvPlayer::getBuildingGroupMaking(SpecialBuildingTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialBuildingInfos(), eIndex);
	return m_paiBuildingGroupMaking[eIndex];
}

int CvPlayer::getBuildingGroupCountPlusMaking(SpecialBuildingTypes eIndex) const
{
	return (getBuildingGroupCount(eIndex) + getBuildingGroupMaking(eIndex));
}


int CvPlayer::getHurryCount(HurryTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumHurryInfos(), eIndex);
	return m_paiHurryCount[eIndex];
}


bool CvPlayer::canHurry(HurryTypes eIndex) const
{
	return (getHurryCount(eIndex) > 0);
}


bool CvPlayer::canPopRush() const
{
	return (m_iPopRushHurryCount > 0);
}

void CvPlayer::changeHurryCount(HurryTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumHurryInfos(), eIndex);

	const int oldHurryCount = m_paiHurryCount[eIndex];
	m_paiHurryCount[eIndex] += iChange;
	FASSERT_NOT_NEGATIVE(m_paiHurryCount[eIndex]);

	// if we just went from 0 to 1 (or the reverse)
	if ((oldHurryCount > 0) != (m_paiHurryCount[eIndex] > 0))
	{
		// does this hurry reduce population?
		if (GC.getHurryInfo(eIndex).getProductionPerPopulation() > 0)
		{
			m_iPopRushHurryCount += iChange;
			FASSERT_NOT_NEGATIVE(m_iPopRushHurryCount);
		}
	}
}

int CvPlayer::getSpecialBuildingNotRequiredCount(SpecialBuildingTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialBuildingInfos(), eIndex);
	return m_paiSpecialBuildingNotRequiredCount[eIndex];
}


bool CvPlayer::isSpecialBuildingNotRequired(SpecialBuildingTypes eIndex) const
{
	return (getSpecialBuildingNotRequiredCount(eIndex) > 0);
}


void CvPlayer::changeSpecialBuildingNotRequiredCount(SpecialBuildingTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumSpecialBuildingInfos(), eIndex);
	m_paiSpecialBuildingNotRequiredCount[eIndex] += iChange;
	FASSERT_NOT_NEGATIVE(getSpecialBuildingNotRequiredCount(eIndex));
}


int CvPlayer::getHasCivicOptionCount(CivicOptionTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumCivicOptionInfos(), eIndex);
	return m_paiHasCivicOptionCount[eIndex];
}


bool CvPlayer::isHasCivicOption(CivicOptionTypes eIndex) const
{
	return (getHasCivicOptionCount(eIndex) > 0);
}


void CvPlayer::changeHasCivicOptionCount(CivicOptionTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumCivicOptionInfos(), eIndex);
	m_paiHasCivicOptionCount[eIndex] += iChange;
	FASSERT_NOT_NEGATIVE(getHasCivicOptionCount(eIndex));
}


int CvPlayer::getNoCivicUpkeepCount(CivicOptionTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumCivicOptionInfos(), eIndex);
	return m_paiNoCivicUpkeepCount[eIndex];
}


bool CvPlayer::isNoCivicUpkeep(CivicOptionTypes eIndex) const
{
	return (getNoCivicUpkeepCount(eIndex) > 0);
}


void CvPlayer::changeNoCivicUpkeepCount(CivicOptionTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumCivicOptionInfos(), eIndex);

	if (iChange != 0)
	{
		m_paiNoCivicUpkeepCount[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(getNoCivicUpkeepCount(eIndex));

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getHasReligionCount(ReligionTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumReligionInfos(), eIndex);
	return m_paiHasReligionCount[eIndex];
}


int CvPlayer::countTotalHasReligion() const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		iCount += getHasReligionCount((ReligionTypes)iI);
	}

	return iCount;
}

int CvPlayer::getHasCorporationCount(CorporationTypes eIndex) const
{
	return isActiveCorporation(eIndex) ? m_paiHasCorporationCount[eIndex] : 0;
}


int CvPlayer::countTotalHasCorporation() const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		iCount += getHasCorporationCount((CorporationTypes)iI);
	}

	return iCount;
}

bool CvPlayer::isActiveCorporation(CorporationTypes eIndex) const
{
	if (isNoCorporations() || isNoForeignCorporations() && !hasHeadquarters(eIndex))
	{
		return false;
	}

	if (
		GC.getCorporationInfo(eIndex).getObsoleteTech() != NO_TECH
	&&
		GET_TEAM(getTeam()).isHasTech(GC.getCorporationInfo(eIndex).getObsoleteTech()))
	{
		return false;
	}

	return true;
}


int CvPlayer::findHighestHasReligionCount() const
{
	PROFILE_EXTRA_FUNC();
	int iBestValue = 0;

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		const int iValue = getHasReligionCount((ReligionTypes)iI);
		if (iValue > iBestValue)
		{
			iBestValue = iValue;
		}
	}

	return iBestValue;
}


void CvPlayer::changeHasReligionCount(ReligionTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumReligionInfos(), eIndex);

	if (iChange != 0)
	{
		m_paiHasReligionCount[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(getHasReligionCount(eIndex));

		//AIAndy: Commented out for now to make interfaith project work properly
		//GC.getGame().updateBuildingCommerce();

		//GC.getGame().AI_makeAssignWorkDirty();
	}
}


void CvPlayer::changeHasCorporationCount(CorporationTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumCorporationInfos(), eIndex)

	if (iChange != 0)
	{
		m_paiHasCorporationCount[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(getHasCorporationCount(eIndex));

		//AIAndy: Commented out to keep same as religion one, this expensive function will be called at next turn change anyway
		//GC.getGame().updateBuildingCommerce();

		//GC.getGame().AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getUpkeepCount(UpkeepTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumUpkeepInfos(), eIndex);
	FAssertMsg(m_paiUpkeepCount, "m_paiUpkeepCount is not expected to be equal with NULL");
	return m_paiUpkeepCount[eIndex];
}


void CvPlayer::changeUpkeepCount(UpkeepTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumUpkeepInfos(), eIndex);

	if (iChange != 0)
	{
		FAssertMsg(m_paiUpkeepCount, "m_paiUpkeepCount is not expected to be equal with NULL");
		m_paiUpkeepCount[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(getUpkeepCount(eIndex));

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getSpecialistValidCount(SpecialistTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex);
	FAssertMsg(m_paiSpecialistValidCount, "m_paiSpecialistValidCount is not expected to be equal with NULL");
	return m_paiSpecialistValidCount[eIndex];
}


bool CvPlayer::isSpecialistValid(SpecialistTypes eIndex) const
{
	return eIndex == GC.getDEFAULT_SPECIALIST() || getSpecialistValidCount(eIndex) > 0;
}


/********************************************************************************/
/* 	New Civic AI						19.08.2010				Fuyu			*/
/********************************************************************************/
//Fuyu bLimited
void CvPlayer::changeSpecialistValidCount(SpecialistTypes eIndex, int iChange, bool bLimited)
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex);

	if (iChange != 0)
	{
		FAssertMsg(m_paiSpecialistValidCount, "m_paiSpecialistValidCount is not expected to be equal with NULL");
		m_paiSpecialistValidCount[eIndex] += iChange;
		FAssertMsg(getSpecialistValidCount(eIndex) >= 0, "getSpecialistValidCount(eIndex) is expected to be non-negative (invalid Index)");

		if (!bLimited)
		{
			AI_makeAssignWorkDirty();
		}
	}
}
/********************************************************************************/
/* 	New Civic AI												END 			*/
/********************************************************************************/


bool CvPlayer::isResearchingTech(TechTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eIndex);
	return m_pabResearchingTech[eIndex];
}


void CvPlayer::setResearchingTech(TechTypes eIndex, bool bNewValue)
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eIndex);

	if (isResearchingTech(eIndex) != bNewValue)
	{
		m_pabResearchingTech[eIndex] = bNewValue;

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(Popup_DIRTY_BIT, true); // to check whether we still need the tech chooser popup
		}
	}
}

bool CvPlayer::isLoyalMember(VoteSourceTypes eVoteSource) const
{
	FASSERT_BOUNDS(0, GC.getNumVoteSourceInfos(), eVoteSource);
	return m_pabLoyalMember[eVoteSource];
}

void CvPlayer::setLoyalMember(VoteSourceTypes eVoteSource, bool bNewValue)
{
	//FASSERT_BOUNDS(0, GC.getNumVoteSourceInfos(), eVoteSource);
	FAssertMsg(eVoteSource >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eVoteSource < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (isLoyalMember(eVoteSource) != bNewValue)
	{
		processVoteSourceBonus(eVoteSource, false);
		m_pabLoyalMember[eVoteSource] = bNewValue;
		processVoteSourceBonus(eVoteSource, true);

		GC.getGame().updateSecretaryGeneral();
	}
}

CivicTypes CvPlayer::getCivics(CivicOptionTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumCivicOptionInfos(), eIndex);
	return m_paeCivics ? m_paeCivics[eIndex] : NO_CIVIC;
}


int CvPlayer::getSingleCivicUpkeep(CivicTypes eCivic, bool bIgnoreAnarchy) const
{
	if (eCivic == NO_CIVIC)
	{
		return 0;
	}
	if (isNoCivicUpkeep((CivicOptionTypes)GC.getCivicInfo(eCivic).getCivicOptionType()))
	{
		return 0;
	}
	if (GC.getCivicInfo(eCivic).getUpkeep() == NO_UPKEEP)
	{
		return 0;
	}
	if (!bIgnoreAnarchy && isAnarchy())
	{
		return 0;
	}
	int iUpkeep =
	(
		std::max(0, (getTotalPopulation() + GC.getDefineINT("UPKEEP_POPULATION_OFFSET")) * GC.getUpkeepInfo((UpkeepTypes)GC.getCivicInfo(eCivic).getUpkeep()).getPopulationPercent() / 100)
		+
		std::max(0, (getNumCities() + GC.getDefineINT("UPKEEP_CITY_OFFSET")) * GC.getUpkeepInfo((UpkeepTypes)GC.getCivicInfo(eCivic).getUpkeep()).getCityPercent() / 100)
	);
	iUpkeep = getModifiedIntValue(iUpkeep, getUpkeepModifier());

	iUpkeep *= GC.getHandicapInfo(getHandicapType()).getCivicUpkeepPercent();
	iUpkeep /= 100;

	if (isNormalAI())
	{
		const int iMod = (
			GC.getHandicapInfo(GC.getGame().getHandicapType()).getAICivicUpkeepPercent() - 100
			+
			GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIPerEraModifier() * getCurrentEra()
		);
		iUpkeep = getModifiedIntValue(iUpkeep, iMod);
	}
	return std::max(1, iUpkeep);//TB changed to a minimum of 1 if not NO upkeep for that civic or a cause to have no upkeep for any civic in that group, both reflected in earlier returns of 0.
}


int CvPlayer::getCivicUpkeep(bool bIgnoreAnarchy) const
{
	PROFILE_EXTRA_FUNC();
	int iTotalUpkeep = 0;

	for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		iTotalUpkeep += getSingleCivicUpkeep(m_paeCivics[iI], bIgnoreAnarchy);
	}

	if (isRebel()) iTotalUpkeep /= 2;

	return iTotalUpkeep;
}


int64_t CvPlayer::getTreasuryUpkeep() const
{
	const int64_t iTreasury = getGold();
	return (
		(iTreasury + 250 * intSqrt64(iTreasury))
		/
		// Scale by gamespeed as gold is worth less on slower speeds, expected treasury size is different.
		(25 * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent())
	);
}


void CvPlayer::setCivics(CivicOptionTypes eIndex, CivicTypes eNewValue)
{
	PROFILE_FUNC();

	CvWString szBuffer;

	FASSERT_BOUNDS(0, GC.getNumCivicOptionInfos(), eIndex);
	FASSERT_BOUNDS(0, GC.getNumCivicInfos(), eNewValue);

	const CivicTypes eOldCivic = getCivics(eIndex);

	if (eOldCivic != eNewValue)
	{
		m_paeCivics[eIndex] = eNewValue;
		if (isNPC()) return;

		if (eOldCivic != NO_CIVIC)
		{
			processCivics(eOldCivic, -1);
		}
		if (getCivics(eIndex) != NO_CIVIC)
		{
			processCivics(getCivics(eIndex), 1);
		}

		// Afforess - Check Buildings, Clear Caches
		bool bUpdateHealth = false;
		bool bUpdateHappiness = false;
		if (eNewValue != NO_CIVIC && eOldCivic != NO_CIVIC)
		{
			const CvCivicInfo& kCivic = GC.getCivicInfo(getCivics(eIndex));
			const CvCivicInfo& kOldCivic = GC.getCivicInfo(eOldCivic);
			for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
			{
				if (kCivic.getImprovementHealthPercentChanges(iI) != kOldCivic.getImprovementHealthPercentChanges(iI))
				{
					bUpdateHealth = true;
				}
				if (kCivic.getImprovementHappinessChanges(iI) != kOldCivic.getImprovementHappinessChanges(iI))
				{
					bUpdateHappiness = true;
				}
				if (bUpdateHappiness && bUpdateHealth)
				{
					break;
				}
			}
		}
		else
		{
			bUpdateHappiness = true;
			bUpdateHealth = true;
		}
		foreach_ (CvCity* city, cities())
		{
			city->checkBuildings();
			if (bUpdateHappiness)
				city->updateFeatureHappiness();
			if (bUpdateHealth)
				city->updateImprovementHealth();
		}

		GC.getGame().updateSecretaryGeneral();

		GC.getGame().AI_makeAssignWorkDirty();

		if (GC.getGame().isFinalInitialized())
		{
			if (gDLL->isDiplomacy() && (gDLL->getDiplomacyPlayer() == getID()))
			{
				gDLL->updateDiplomacyAttitude(true);
			}
			// Silence announcement for civs that are not alive, ie rebel civs who may not be born
			// and also for minor civs
			if (!isNPC() && !isMinorCiv() && isAlive())
			{
				if (getCivics(eIndex) != NO_CIVIC)
				{
					if (getCivics(eIndex) != GC.getCivilizationInfo(getCivilizationType()).getCivilizationInitialCivics(eIndex))
					{
						for (int iI = 0; iI < MAX_PLAYERS; iI++)
						{
							if (GET_PLAYER((PlayerTypes)iI).isAlive())
							{
								if (GET_TEAM(getTeam()).isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
								{

									szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_ADOPTED_CIVIC", getNameKey(), GC.getCivicOptionInfo((CivicOptionTypes)GC.getCivicInfo(getCivics(eIndex)).getCivicOptionType()).getTextKeyWide(), GC.getCivicInfo(getCivics(eIndex)).getTextKeyWide());
									AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MAJOR_EVENT);
								}
							}
						}

						szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_ADOPTED_CIVIC", getNameKey(), GC.getCivicOptionInfo((CivicOptionTypes)GC.getCivicInfo(getCivics(eIndex)).getCivicOptionType()).getTextKeyWide(), GC.getCivicInfo(getCivics(eIndex)).getTextKeyWide());
						GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szBuffer);

						CvEventReporter::getInstance().civicChanged(getID(), eOldCivic, getCivics(eIndex));
					}
				}
			}
		}

		// Attitude cache
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			AI_invalidateAttitudeCache((PlayerTypes)iI);
			GET_PLAYER((PlayerTypes)iI).AI_invalidateAttitudeCache(getID());
		}

		//	A new civic can effect best plot build decisions so mark stale in all cities
		algo::for_each(cities(), CvCity::fn::AI_markBestBuildValuesStale());

		clearCanConstructCache(NO_BUILDING, true);
	}
}


int CvPlayer::getExtraSpecialistYield(SpecialistTypes eIndex1, YieldTypes eIndex2) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex1);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex2);
	return m_ppaaiSpecialistExtraYield[eIndex1][eIndex2];
}


void CvPlayer::changeExtraSpecialistYield(SpecialistTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex1);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex2);

	if (iChange != 0)
	{
		m_ppaaiSpecialistExtraYield[eIndex1][eIndex2] += iChange;
		//TB Note: it should be ok to have a negative total Extra Specialist Yield now

		updateExtraSpecialistYield();

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), eIndex1);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex2);
	return m_ppaaiImprovementYieldChange[eIndex1][eIndex2];
}


void CvPlayer::changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), eIndex1);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex2);

	if (iChange != 0)
	{
		m_ppaaiImprovementYieldChange[eIndex1][eIndex2] += iChange;
		updateYield();
	}
}

//	When a node is updated in the cycle we assume it won't have chnaged
//	position by more than SEARCH_HORIZON nodes unless bFarMove is true.
//	This is basically just saying that we expect groups which have not moved very far
//	to still be near the units they weer near before they moved, and dramatuically
//	allows us to reduce serach times.  In the evnt of the assumtpion being incorrect
//	the only fall-out is that units will select in not quite the most convenient order
#define	REINSERT_SEARCH_HORIZON	5

// XXX should pUnit be a CvSelectionGroup???
void CvPlayer::updateGroupCycle(CvUnit* pUnit, bool bFarMove)
{
	PROFILE_FUNC();

	if (!pUnit->plot() || isTempUnit(pUnit))
	{
		return;
	}

	FAssertMsg(pUnit->getGroup(), "Unit->getGroup() is not assigned a valid value");

	CLLNode<int>* pReinsertSearchStart = removeGroupCycle(pUnit->getGroupID());

	CvUnit* pBeforeUnit = NULL;
	CvUnit* pAfterUnit = NULL;

	foreach_ (CvUnit* pLoopUnit, pUnit->plot()->units())
	{
		//AlbertS2 added this to fix a CTD that happened while loading a saved game
		if (pLoopUnit == NULL)	continue;

		FAssert(pLoopUnit->getGroup());

		if (pLoopUnit->isGroupHead() && pLoopUnit != pUnit)
		{
			if (!isBeforeUnitCycle(pLoopUnit, pUnit))
			{
				pBeforeUnit = pLoopUnit;
				break;
			}
			pAfterUnit = pLoopUnit;
		}
	}

	int	iSearchHorizon;
	if (!bFarMove && pBeforeUnit == NULL && pAfterUnit == NULL)
	{
		iSearchHorizon = REINSERT_SEARCH_HORIZON;
		while (pReinsertSearchStart && iSearchHorizon-- > 0)
		{
			pReinsertSearchStart = previousGroupCycleNode(pReinsertSearchStart);
		}
	}

	CLLNode<int>* pSelectionGroupNode;
	if (bFarMove)
	{
		pSelectionGroupNode = headGroupCycleNode();
		iSearchHorizon = MAX_INT;
	}
	else
	{
		pSelectionGroupNode = (pReinsertSearchStart == NULL ? headGroupCycleNode() : pReinsertSearchStart);
		iSearchHorizon = REINSERT_SEARCH_HORIZON*2;	//Horizon in each direction
	}

	int iBestValue = MAX_INT;
	CLLNode<int>* pBestSelectionGroupNode = NULL;

	while (pSelectionGroupNode && iSearchHorizon-- > 0)
	{
		CvSelectionGroup* pLoopSelectionGroup = getSelectionGroup(pSelectionGroupNode->m_data);
		FAssertMsg(pLoopSelectionGroup, "LoopSelectionGroup is not assigned a valid value");

		const CvUnit* pHeadUnit = pLoopSelectionGroup->getHeadUnit();

		if (pHeadUnit)
		{
			if (pBeforeUnit)
			{
				if (pBeforeUnit == pHeadUnit)
				{
					pBestSelectionGroupNode = pSelectionGroupNode;
					break;
				}
			}
			else if (pAfterUnit)
			{
				if (pAfterUnit == pHeadUnit)
				{
					pBestSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);
					break;
				}
			}
			else
			{
				const int iValue = plotDistance(pUnit->getX(), pUnit->getY(), pHeadUnit->getX(), pHeadUnit->getY());
				if (iValue < iBestValue)
				{
					iBestValue = iValue;
					pBestSelectionGroupNode = pSelectionGroupNode;
				}
			}
		}

		pSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);
	}

	if (pBestSelectionGroupNode)
	{
		m_groupCycles[CURRENT_MAP]->insertBefore(pUnit->getGroupID(), pBestSelectionGroupNode);
	}
	else
	{
		m_groupCycles[CURRENT_MAP]->insertAtEnd(pUnit->getGroupID());
	}
}


CLLNode<int>* CvPlayer::removeGroupCycle(int iID)
{
	PROFILE_FUNC();

	CLLNode<int>* pPreviousNode = NULL;
	CLLNode<int>* pSelectionGroupNode = headGroupCycleNode();

	while (pSelectionGroupNode)
	{
		if (pSelectionGroupNode->m_data == iID)
		{
			pSelectionGroupNode = deleteGroupCycleNode(pSelectionGroupNode);
			break;
		}
		else
		{
			pPreviousNode = pSelectionGroupNode;
			pSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);
		}
	}

	return pPreviousNode;
}


CLLNode<int>* CvPlayer::deleteGroupCycleNode(CLLNode<int>* pNode)
{
	return m_groupCycles[CURRENT_MAP]->deleteNode(pNode);
}


CLLNode<int>* CvPlayer::nextGroupCycleNode(CLLNode<int>* pNode) const
{
	return m_groupCycles[CURRENT_MAP]->next(pNode);
}


CLLNode<int>* CvPlayer::previousGroupCycleNode(CLLNode<int>* pNode) const
{
	return m_groupCycles[CURRENT_MAP]->prev(pNode);
}


CLLNode<int>* CvPlayer::headGroupCycleNode() const
{
	return m_groupCycles[CURRENT_MAP]->head();
}


CLLNode<int>* CvPlayer::tailGroupCycleNode() const
{
	return m_groupCycles[CURRENT_MAP]->tail();
}

// AIAndy: This can cause a large huge amount of paths to be created if there are lots of OR with lots of subsets the same (so especially a very expensive operation when the target tech is far away)
void CvPlayer::constructTechPathSet(TechTypes eTech, std::vector<techPath*>& pathSet, techPath& rootPath) const
{
	PROFILE_FUNC();

	// AIAndy: Consider using a set for efficient testing
	if (GET_TEAM(getTeam()).isHasTech(eTech) || algo::any_of_equal(rootPath, eTech))
	{
		//	We have this tech, no reason to add this to the pre-reqs
		return;
	}

	rootPath.push_back(eTech);

	//	Cycle through the and paths and add their tech paths
	foreach_(const TechTypes ePreReq, GC.getTechInfo(eTech).getPrereqAndTechs())
	{
		constructTechPathSet(ePreReq, pathSet, rootPath);
	}

	//	Find the shortest OR tech
	const std::vector<TechTypes>& prereqOrTechs = GC.getTechInfo(eTech).getPrereqOrTechs();
	for (uint32_t j = 0; j < prereqOrTechs.size(); j++)
	{
		techPath* seedPath;

		if (j == 0)
		{
			seedPath = &rootPath;
		}
		else
		{
			//	Spawn a copy
			seedPath = new techPath(rootPath);
			pathSet.push_back(seedPath);
		}

		//	Recurse
		constructTechPathSet(prereqOrTechs[j], pathSet, *seedPath);
	}
}


//	Finds the path length from this tech type to one you already know
int CvPlayer::findPathLength(TechTypes eTech, bool bCost) const
{
	PROFILE_FUNC();

	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eTech);

	if (eTech == NO_TECH || GET_TEAM(getTeam()).isHasTech(eTech))
	{
		// We have this tech, no reason to add this to the pre-reqs
		return 0; // Base case return 0, we know it...
	}

	int& cache = bCost ? m_aiCostPathLengthCache[eTech] : m_aiPathLengthCache[eTech];
	if (cache == -1)
	{
		// Utilisation de pointeurs bruts car constructTechPathSet attend un vector<techPath*>
		std::vector<techPath*> possiblePaths;
		possiblePaths.push_back(new techPath());

		constructTechPathSet(eTech, possiblePaths, *possiblePaths[0]);

		int iBestValue = MAX_INT;
		for (size_t i = 0; i < possiblePaths.size(); ++i)
		{
			techPath* pathPtr = possiblePaths[i];
			const techPath& path = *pathPtr;

			int iValue = 0;
			if (bCost)
			{
				for (size_t j = 0; j < path.size(); ++j)
				{
					TechTypes tech = path[j];
					iValue += GET_TEAM(getTeam()).getResearchCost(tech);
				}
			}
			else
			{
				iValue = static_cast<int>(path.size());
			}


			if (iValue < iBestValue)
			{
				iBestValue = iValue;
				if (iBestValue == 1) break; // Early exit
			}
		}

		// Nettoyage m�moire obligatoire (sinon fuite)
		for (size_t i = 0; i < possiblePaths.size(); ++i)
		{
			delete possiblePaths[i];
		}
		possiblePaths.clear();
		cache = iBestValue;
	}
	return cache;
}


//	Function specifically for python/tech chooser screen
int CvPlayer::getQueuePosition(TechTypes eTech) const
{
	PROFILE_EXTRA_FUNC();
	int i = 1;

	for (CLLNode<TechTypes>* pResearchNode = headResearchQueueNode(); pResearchNode; pResearchNode = nextResearchQueueNode(pResearchNode))
	{
		if (pResearchNode->m_data == eTech)
		{
			return i;
		}
		i++;
	}

	return -1;
}


void CvPlayer::clearResearchQueue()
{
	PROFILE_EXTRA_FUNC();
	m_researchQueue.clear();

	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		setResearchingTech(((TechTypes)iI), false);
	}

	if (getTeam() == GC.getGame().getActiveTeam())
	{
		gDLL->getInterfaceIFace()->setDirty(ResearchButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}
}


// Pushes research onto the queue.
//	If it is an append if will put it and its pre-reqs into the queue.
//	If it is not an append it will change research immediately and should be used with clear.
//	Clear will clear the entire queue.
bool CvPlayer::pushResearch(TechTypes eTech, bool bClear)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eTech);

	if (GET_TEAM(getTeam()).isHasTech(eTech) || isResearchingTech(eTech))
	{
		// We have this tech, no reason to add this to the pre-reqs
		return true;
	}

	if (!canResearch(eTech, false))
	{
		return false;
	}

	if (bClear)
	{
		clearResearchQueue();
	}

	//	Add in all the pre-reqs for the and techs...
	foreach_(const TechTypes ePreReq, GC.getTechInfo(eTech).getPrereqAndTechs())
	{
		if (!pushResearch(ePreReq))
		{
			return false;
		}
	}

	// Will return the shortest path of all the or techs.  Tie breaker goes to the first one...
	TechTypes eShortestOr = NO_TECH;
	int iShortestPath = MAX_INT;
	bool bOrPrereqFound = false;
	//	Cycle through all the OR techs
	foreach_(const TechTypes ePreReq, GC.getTechInfo(eTech).getPrereqOrTechs())
	{
		bOrPrereqFound = true;

		//	If the pre-req exists, and we have it, it is the shortest path, get out, we're done
		if (GET_TEAM(getTeam()).isHasTech(ePreReq))
		{
			eShortestOr = ePreReq;
			break;
		}

		if (canResearch(ePreReq, false))
		{
			// Find the length of the path to this pre-req
			const int iNumSteps = findPathLength(ePreReq);

			// If this pre-req is a valid tech, and its the shortest current path, set it as such
			if (iNumSteps < iShortestPath)
			{
				eShortestOr = ePreReq;
				iShortestPath = iNumSteps;
			}
		}
	}

	//	If the shortest path tech is valid, push it (and its children) on to the research queue recursively
	if (eShortestOr != NO_TECH)
	{
		if (!pushResearch(eShortestOr))
		{
			return false;
		}
	}
	else if (bOrPrereqFound)
	{
		return false;
	}

	m_researchQueue.insertAtEnd(eTech);

	setResearchingTech(eTech, true);

	if (getTeam() == GC.getGame().getActiveTeam())
	{
		gDLL->getInterfaceIFace()->setDirty(ResearchButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}

	CvEventReporter::getInstance().techSelected(eTech, getID());

	return true;
}


//	If bHead is true we delete the entire queue...
void CvPlayer::popResearch(TechTypes eTech)
{
	PROFILE_EXTRA_FUNC();
	for (CLLNode<TechTypes>* pResearchNode = headResearchQueueNode(); pResearchNode; pResearchNode = nextResearchQueueNode(pResearchNode))
	{
		if (pResearchNode->m_data == eTech)
		{
			m_researchQueue.deleteNode(pResearchNode);
			break;
		}
	}

	setResearchingTech(eTech, false);

	if (getTeam() == GC.getGame().getActiveTeam())
	{
		gDLL->getInterfaceIFace()->setDirty(ResearchButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}
}


int CvPlayer::getLengthResearchQueue() const
{
	return m_researchQueue.getLength();
}


CLLNode<TechTypes>* CvPlayer::nextResearchQueueNode(CLLNode<TechTypes>* pNode) const
{
	return m_researchQueue.next(pNode);
}


CLLNode<TechTypes>* CvPlayer::headResearchQueueNode() const
{
	return m_researchQueue.head();
}


CLLNode<TechTypes>* CvPlayer::tailResearchQueueNode() const
{
	return m_researchQueue.tail();
}


void CvPlayer::addCityName(const CvWString& szName)
{
	m_cityNames.insertAtEnd(szName);
}


int CvPlayer::getNumCityNames() const
{
	return m_cityNames.getLength();
}


CvWString CvPlayer::getCityName(int iIndex) const
{
	const CLLNode<CvWString>* pCityNameNode = m_cityNames.nodeNum(iIndex);
	return pCityNameNode ? pCityNameNode->m_data : L"";
}


CLLNode<CvWString>* CvPlayer::nextCityNameNode(CLLNode<CvWString>* pNode) const
{
	return m_cityNames.next(pNode);
}


CLLNode<CvWString>* CvPlayer::headCityNameNode() const
{
	return m_cityNames.head();
}


CvPlotGroup* CvPlayer::firstPlotGroup(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_plotGroups[CURRENT_MAP]->beginIter(pIterIdx) : m_plotGroups[CURRENT_MAP]->endIter(pIterIdx);
}


CvPlotGroup* CvPlayer::nextPlotGroup(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_plotGroups[CURRENT_MAP]->nextIter(pIterIdx) : m_plotGroups[CURRENT_MAP]->prevIter(pIterIdx);
}


int CvPlayer::getNumPlotGroups() const
{
	return m_plotGroups[CURRENT_MAP]->getCount();
}


CvPlotGroup* CvPlayer::getPlotGroup(int iID) const
{
	return m_plotGroups[CURRENT_MAP]->getAt(iID);
}


CvPlotGroup* CvPlayer::addPlotGroup()
{
	return m_plotGroups[CURRENT_MAP]->add();
}


void CvPlayer::deletePlotGroup(int iID)
{
	PROFILE_EXTRA_FUNC();
#ifdef VALIDATION_FOR_PLOT_GROUPS
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		const CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if ( pLoopPlot->getPlotGroup(getID()) && pLoopPlot->getPlotGroup(getID())->getID() == iID )
		{
			::MessageBox(NULL, "Deleting in-use plot group!", "CvGameCoreDLL", MB_OK);
		}
	}
#endif
	m_plotGroups[CURRENT_MAP]->removeAt(iID);
}


CvCity* CvPlayer::firstCity(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_cities[CURRENT_MAP]->beginIter(pIterIdx) : m_cities[CURRENT_MAP]->endIter(pIterIdx);
}

CvCity* CvPlayer::nextCity(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_cities[CURRENT_MAP]->nextIter(pIterIdx) : m_cities[CURRENT_MAP]->prevIter(pIterIdx);
}


CvCity* CvPlayer::nextCityExternal(int *pIterIdx, bool bRev) const
{
	PROFILE_EXTRA_FUNC();
	CvCity* pResult;
	do
	{
		pResult = (!bRev ? m_cities[CURRENT_MAP]->nextIter(pIterIdx) : m_cities[CURRENT_MAP]->prevIter(pIterIdx));
	} while(pResult && !pResult->isInViewport());

	return pResult;
}

CvCity* CvPlayer::firstCityExternal(int *pIterIdx, bool bRev) const
{
	CvCity* pResult = !bRev ? m_cities[CURRENT_MAP]->beginIter(pIterIdx) : m_cities[CURRENT_MAP]->endIter(pIterIdx);

	if (pResult == NULL || pResult->isInViewport())
	{
		return pResult;
	}
	return nextCityExternal(pIterIdx, bRev);
}


int CvPlayer::getNumCities() const
{
	return m_cities[CURRENT_MAP]->getCount();
}


CvCity* CvPlayer::getCity(int iID) const
{
	return m_cities[CURRENT_MAP]->getAt(iID);
}


CvCity* CvPlayer::addCity()
{
	return m_cities[CURRENT_MAP]->add();
}


void CvPlayer::deleteCity(int iID)
{
	m_cities[CURRENT_MAP]->removeAt(iID);
}


CvUnit* CvPlayer::firstUnit(int *pIterIdx, bool bRev) const
{
	CvUnit* pResult = !bRev ? m_units[CURRENT_MAP]->beginIter(pIterIdx) : m_units[CURRENT_MAP]->endIter(pIterIdx);
	if (pResult && pResult == m_pTempUnit)
	{
		pResult = nextUnit(pIterIdx, bRev);
	}
	return pResult;
}


CvUnit* CvPlayer::nextUnit(int *pIterIdx, bool bRev) const
{
	CvUnit* pResult = !bRev ? m_units[CURRENT_MAP]->nextIter(pIterIdx) : m_units[CURRENT_MAP]->prevIter(pIterIdx);
	if (pResult && pResult == m_pTempUnit)
	{
		pResult = nextUnit(pIterIdx, bRev);
	}
	return pResult;
}

CvUnit* CvPlayer::firstUnitExternal(int *pIterIdx, bool bRev) const
{
	CvUnit*	pResult = firstUnit(pIterIdx, bRev);

	if(pResult && !pResult->isInViewport())
	{
		pResult = nextUnitExternal(pIterIdx, bRev);
	}

	return pResult;
}


CvUnit* CvPlayer::nextUnitExternal(int *pIterIdx, bool bRev) const
{
	PROFILE_EXTRA_FUNC();
	CvUnit*	pResult = nextUnit(pIterIdx, bRev);

	while(pResult && !pResult->isInViewport())
	{
		pResult = nextUnit(pIterIdx, bRev);
	}

	return pResult;
}


int CvPlayer::getNumUnits() const
{
	return m_units[CURRENT_MAP]->getCount() - (m_pTempUnit ? 1 : 0);
}


CvUnit* CvPlayer::getUnit(int iID) const
{
	return m_units[CURRENT_MAP]->getAt(iID);
}

CvUnit* CvPlayer::addUnit()
{
	return m_units[CURRENT_MAP]->add();
}


void CvPlayer::deleteUnit(int iID)
{
	m_units[CURRENT_MAP]->removeAt(iID);
}

CvSelectionGroup* CvPlayer::firstSelectionGroup(int *pIterIdx, bool bRev) const
{
	CvSelectionGroup* pResult = !bRev ? m_selectionGroups[CURRENT_MAP]->beginIter(pIterIdx) : m_selectionGroups[CURRENT_MAP]->endIter(pIterIdx);
	if (pResult && pResult->getHeadUnit() && pResult->getHeadUnit() == m_pTempUnit)
	{
		pResult = nextSelectionGroup(pIterIdx, bRev);
	}

	return pResult;
}

CvSelectionGroup* CvPlayer::nextSelectionGroup(int *pIterIdx, bool bRev) const
{
	CvSelectionGroup* pResult = !bRev ? m_selectionGroups[CURRENT_MAP]->nextIter(pIterIdx) : m_selectionGroups[CURRENT_MAP]->prevIter(pIterIdx);
	if (pResult && pResult->getHeadUnit() && pResult->getHeadUnit() == m_pTempUnit)
	{
		pResult = nextSelectionGroup(pIterIdx, bRev);
	}

	return pResult;
}

CvSelectionGroup* CvPlayer::firstSelectionGroupNonEmpty(int* pIterIdx, bool bRev) const
{
	PROFILE_EXTRA_FUNC();
	CvSelectionGroup* pResult = firstSelectionGroup(pIterIdx, bRev);
	while (pResult != nullptr && pResult->getHeadUnit() == nullptr)
	{
		pResult = nextSelectionGroup(pIterIdx, bRev);
	}
	return pResult;
}

CvSelectionGroup* CvPlayer::nextSelectionGroupNonEmpty(int* pIterIdx, bool bRev) const
{
	PROFILE_EXTRA_FUNC();
	CvSelectionGroup* pResult = nextSelectionGroup(pIterIdx, bRev);
	while (pResult != nullptr && pResult->getHeadUnit() == nullptr)
	{
		pResult = nextSelectionGroup(pIterIdx, bRev);
	}
	return pResult;
}

int CvPlayer::getNumSelectionGroups() const
{
	return m_selectionGroups[CURRENT_MAP]->getCount() - (m_pTempUnit ? 1 : 0);
}


CvSelectionGroup* CvPlayer::getSelectionGroup(int iID) const
{
	return m_selectionGroups[CURRENT_MAP]->getAt(iID);
}


CvSelectionGroup* CvPlayer::addSelectionGroup()
{
	return m_selectionGroups[CURRENT_MAP]->add();
}


void CvPlayer::deleteSelectionGroup(int iID)
{
	bool bRemoved = m_selectionGroups[CURRENT_MAP]->removeAt(iID);
	FAssertMsg(bRemoved, "could not find group, delete failed");
}

EventTriggeredData* CvPlayer::firstEventTriggered(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_eventsTriggered.beginIter(pIterIdx) : m_eventsTriggered.endIter(pIterIdx);
}

EventTriggeredData* CvPlayer::nextEventTriggered(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_eventsTriggered.nextIter(pIterIdx) : m_eventsTriggered.prevIter(pIterIdx);
}

int CvPlayer::getNumEventsTriggered() const
{
	return m_eventsTriggered.getCount();
}

EventTriggeredData* CvPlayer::getEventTriggered(int iID) const
{
	return m_eventsTriggered.getAt(iID);
}

EventTriggeredData* CvPlayer::addEventTriggered()
{
	return m_eventsTriggered.add();
}

void CvPlayer::deleteEventTriggered(int iID)
{
	m_eventsTriggered.removeAt(iID);
}


void CvPlayer::addMessage(const CvTalkingHeadMessage& message)
{

	m_listGameMessages.push_back(message);
	
	if (gPlayerLogLevel >= 1)
	{
		logBBAI("  Player %d (%S) get message : %S", getID(), getCivilizationDescription(0), message.getDescription());
	}
	
}


void CvPlayer::clearMessages()
{
	m_listGameMessages.clear();
}


const CvMessageQueue& CvPlayer::getGameMessages() const
{
	return m_listGameMessages;
}


void CvPlayer::expireMessages()
{
	PROFILE_EXTRA_FUNC();
	CvMessageQueue::iterator it = m_listGameMessages.begin();
	bool bFoundExpired = false;
	while(it != m_listGameMessages.end())
	{
		CvTalkingHeadMessage& message = *it;
		if (GC.getGame().getGameTurn() >= message.getExpireTurn())
		{
			it = m_listGameMessages.erase(it);
			bFoundExpired = true;
		}
		else
		{
			++it;
		}
	}
	if (bFoundExpired)
	{
		gDLL->getInterfaceIFace()->dirtyTurnLog(getID());
	}
}


void CvPlayer::addPopup(CvPopupInfo* pInfo, bool bFront)
{
	if (isHumanPlayer())
	{
		if (bFront)
		{
			m_listPopups.push_front(pInfo);
		}
		else
		{
			m_listPopups.push_back(pInfo);
		}
	}
	else
	{
		SAFE_DELETE(pInfo);
	}
}


void CvPlayer::clearPopups()
{
	PROFILE_EXTRA_FUNC();
	foreach_(CvPopupInfo* pInfo, m_listPopups)
	{
		SAFE_DELETE(pInfo);
	}
	m_listPopups.clear();
}


CvPopupInfo* CvPlayer::popFrontPopup()
{
	CvPopupInfo* pInfo = NULL;
	if (!m_listPopups.empty())
	{
		pInfo = m_listPopups.front();
		m_listPopups.pop_front();
	}
	return pInfo;
}


const CvPopupQueue& CvPlayer::getPopups() const
{
	return m_listPopups;
}


void CvPlayer::addDiplomacy(CvDiploParameters* pDiplo)
{
	if (pDiplo)
	{
		m_listDiplomacy.push_back(pDiplo);
	}
}


void CvPlayer::clearDiplomacy()
{
	PROFILE_EXTRA_FUNC();
	foreach_(CvDiploParameters* pDiplo, m_listDiplomacy)
	{
		SAFE_DELETE(pDiplo);
	}
	m_listDiplomacy.clear();
}


const CvDiploQueue& CvPlayer::getDiplomacy() const
{
	return m_listDiplomacy;
}


CvDiploParameters* CvPlayer::popFrontDiplomacy()
{
	CvDiploParameters* pDiplo = NULL;
	if (!m_listDiplomacy.empty())
	{
		pDiplo = m_listDiplomacy.front();
		m_listDiplomacy.pop_front();
	}
	return pDiplo;
}


void CvPlayer::showSpaceShip()
{
	CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
	pInfo->setData1(-1);
	pInfo->setText(L"showSpaceShip");
	addPopup(pInfo);
}

void CvPlayer::clearSpaceShipPopups()
{
	PROFILE_EXTRA_FUNC();
	//clear all spaceship popups
	CvPopupQueue::iterator it;
	for (it = m_listPopups.begin(); it != m_listPopups.end(); )
	{
		CvPopupInfo* pInfo = *it;
		if (pInfo)
		{
			if(pInfo->getText().compare(L"showSpaceShip") == 0)
			{
				it = m_listPopups.erase(it);
				SAFE_DELETE(pInfo);
			}
			else
			{
				++it;
			}
		}
		else
		{
			++it;
		}
	}
}

int64_t CvPlayer::getScoreHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapScoreHistory.find(iTurn);
	return it != m_mapScoreHistory.end() ? it->second : 0;
}

void CvPlayer::updateScoreHistory(int iTurn, int64_t iBestScore)
{
	if ( isEverAlive() )
	{
		m_mapScoreHistory[iTurn] = iBestScore;
	}
}

int64_t CvPlayer::getEconomyHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapEconomyHistory.find(iTurn);
	return it != m_mapEconomyHistory.end() ? it->second : 0;
}

int64_t CvPlayer::getIndustryHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapIndustryHistory.find(iTurn);
	return it != m_mapIndustryHistory.end() ? it->second : 0;
}

int64_t CvPlayer::getAgricultureHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapAgricultureHistory.find(iTurn);
	return it != m_mapAgricultureHistory.end() ? it->second : 0;
}

int64_t CvPlayer::getPowerHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapPowerHistory.find(iTurn);
	return it != m_mapPowerHistory.end() ? it->second : 0;
}

int64_t CvPlayer::getCultureHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapCultureHistory.find(iTurn);
	return it != m_mapCultureHistory.end() ? it->second : 0;
}

int64_t CvPlayer::getEspionageHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapEspionageHistory.find(iTurn);
	return it != m_mapEspionageHistory.end() ? it->second : 0;
}

// RevolutionDCM - record revolution stability history
// Data is used for the CvInfo graphs
int64_t CvPlayer::getRevolutionStabilityHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapRevolutionStabilityHistory.find(iTurn);
	return it != m_mapRevolutionStabilityHistory.end() ? it->second : 0;
}


std::string CvPlayer::getScriptData() const
{
	return m_szScriptData;
}

void CvPlayer::setScriptData(std::string szNewValue)
{
	m_szScriptData = szNewValue;
}

const CvString CvPlayer::getPbemEmailAddress() const
{
	return GC.getInitCore().getEmail(getID());
}

void CvPlayer::setPbemEmailAddress(const char* szAddress)
{
	GC.getInitCore().setEmail(getID(), szAddress);
}

const CvString CvPlayer::getSmtpHost() const
{
	return GC.getInitCore().getSmtpHost(getID());
}

void CvPlayer::setSmtpHost(const char* szHost)
{
	GC.getInitCore().setSmtpHost(getID(), szHost);
}

// Protected Functions...

void CvPlayer::doGold()
{
	PROFILE_FUNC();

	changeGold(calculateGoldRate());

	if (getGold() < 0)
	{
		setGold(0);

		if (!isNPC() && getNumCities() > 0)
		{
			setStrike(true);
			changeStrikeTurns(1);

			if (getStrikeTurns() > 1)
			{
				const int iDisbandUnit = getStrikeTurns() / 2;

				for (int iI = 0; iI < iDisbandUnit; iI++)
				{
					disbandUnit();

					if (calculateGoldRate() >= 0)
					{
						break;
					}
				}
			}
		}
	}
	else setStrike(false);
}


void CvPlayer::doResearch()
{
	PROFILE_FUNC();
	if (isNPC()) return;

	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		m_aiPathLengthCache[iI] = -1;
		m_aiCostPathLengthCache[iI] = -1;
	}

	bool bForceResearchChoice = false;

	if (getCurrentResearch() == NO_TECH)
	{
		FAssert(isHumanPlayer())

		if (getID() == GC.getGame().getActivePlayer())
		{
			chooseTech();
		}

		if (GC.getGame().getElapsedGameTurns() > 4)
		{
			AI_chooseResearch();

			bForceResearchChoice = true;
		}
	}

	const TechTypes eCurrentTech = getCurrentResearch();
	if (eCurrentTech == NO_TECH)
	{
		changeOverflowResearch(getModifiedIntValue(calculateResearchRate(), -calculateResearchModifier(eCurrentTech)));
	}
	else
	{
		const int iOverflow = getOverflowResearch();
		changeOverflowResearch(-iOverflow);
		GET_TEAM(getTeam()).changeResearchProgress(eCurrentTech, calculateResearchRate(eCurrentTech) + getModifiedIntValue(iOverflow, calculateResearchModifier(eCurrentTech)), getID());
	}

	if (bForceResearchChoice)
	{
		clearResearchQueue();
	}
}

void CvPlayer::doEspionageOneOffPoints(int iChange)
{
	PROFILE_FUNC();

	if (iChange > 0)
	{
		GET_TEAM(getTeam()).changeEspionagePointsEver(iChange);

		// Divide up Espionage between Teams
		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (getTeam() != iI && GET_TEAM((TeamTypes)iI).isAlive()
			&& GET_TEAM(getTeam()).isHasMet((TeamTypes)iI))
			{
				const int iSpending = getEspionageSpending((TeamTypes)iI, iChange);

				if (iSpending > 0)
				{
					GET_TEAM(getTeam()).changeEspionagePointsAgainstTeam((TeamTypes)iI, iSpending);
				}
			}
		}
	}
}

void CvPlayer::doEspionagePoints()
{
	doEspionageOneOffPoints(getCommerceRate(COMMERCE_ESPIONAGE));
}

int CvPlayer::getEspionageSpending(TeamTypes eAgainstTeam, int iTotal) const
{
	PROFILE_EXTRA_FUNC();
	int iTotalWeight = 0;
	int iBestWeight = 0;

	bool bFoundTeam = false;

	// Get sum of all weights to be used later on
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (getTeam() != iI && GET_TEAM((TeamTypes)iI).isAlive()
		&& GET_TEAM(getTeam()).isHasMet((TeamTypes)iI))
		{
			if (iI == int(eAgainstTeam))
			{
				bFoundTeam = true;
			}
			const int iWeight = getEspionageSpendingWeightAgainstTeam((TeamTypes)iI);

			if (iWeight > iBestWeight)
			{
				iBestWeight = iWeight;
			}
			iTotalWeight += iWeight;
		}
	}
	// The player requested is not valid
	if (!bFoundTeam)
	{
		return -1;
	}
	int iSpendingValue = 0;
	int iTotalPoints = (iTotal == -1 ? getCommerceRate(COMMERCE_ESPIONAGE) : iTotal);
	int iAvailablePoints = iTotalPoints;

	// Split up Espionage Point budget based on weights (if any weights have been assigned)
	if (iTotalWeight > 0)
	{
		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (getTeam() != iI && GET_TEAM((TeamTypes)iI).isAlive()
			&& GET_TEAM(getTeam()).isHasMet((TeamTypes)iI))
			{
				const int iChange = (iTotalPoints * getEspionageSpendingWeightAgainstTeam((TeamTypes)iI) / iTotalWeight);
				iAvailablePoints -= iChange;

				if (iI == int(eAgainstTeam))
				{
					iSpendingValue += iChange;
				}
			}
		}
	}
	// Divide remainder evenly among top Teams
	while (iAvailablePoints > 0)
	{
		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (getTeam() != iI && GET_TEAM((TeamTypes)iI).isAlive()
			&& GET_TEAM(getTeam()).isHasMet((TeamTypes)iI)
			&& getEspionageSpendingWeightAgainstTeam((TeamTypes)iI) == iBestWeight)
			{
				if (iI == int(eAgainstTeam))
				{
					++iSpendingValue;
				}
				--iAvailablePoints;

				if (iAvailablePoints <= 0)
				{
					break;
				}
			}
		}
	}
	return iSpendingValue;
}

bool CvPlayer::canDoEspionageMission(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvPlot* pPlot, int iExtraData, const CvUnit* pUnit) const
{
	if (getID() == eTargetPlayer || NO_PLAYER == eTargetPlayer)
	{
		return false;
	}

	if (!GET_PLAYER(eTargetPlayer).isAlive() || !GET_TEAM(getTeam()).isHasMet(GET_PLAYER(eTargetPlayer).getTeam()))
	{
		return false;
	}

	const CvEspionageMissionInfo& kMission = GC.getEspionageMissionInfo(eMission);

	// Need Tech Prereq, if applicable
	if (kMission.getTechPrereq() != NO_TECH && !GET_TEAM(getTeam()).isHasTech(kMission.getTechPrereq()))
	{
		return false;
	}

	const int iCost = getEspionageMissionCost(eMission, eTargetPlayer, pPlot, iExtraData, pUnit);
	if (iCost < 0)
	{
		return false;
	}

	if (NO_PLAYER != eTargetPlayer)
	{
		int iEspionagePoints = GET_TEAM(getTeam()).getEspionagePointsAgainstTeam(GET_PLAYER(eTargetPlayer).getTeam());

		if (iEspionagePoints < iCost)
		{
			return false;
		}

		if (iEspionagePoints <= 0)
		{
			return false;
		}
	}

	// Super Spies
	// Mod espionage mission control to stop a mission from appearing in the interface where
	// any mission will appear there if it has a cost attached to it. The BTS mission system
	// has no mission state variable for enabling/disabling individual missions.
	const CvEspionageMissionInfo& kMissionInfo = GC.getEspionageMissionInfo((EspionageMissionTypes)eMission);

	//Bribe
	if (kMissionInfo.getBuyUnitCostFactor() > 0 && !GC.isSS_BRIBE())
	{
		return false;
	}
	//Assassinate
	if (kMissionInfo.getDestroyUnitCostFactor() > 0 && !GC.isSS_ASSASSINATE())
	{
		return false;
	}
	// ! Super Spies

	if (kMission.getPrereqGameOption() != NO_GAMEOPTION && !GC.getGame().isOption((GameOptionTypes)kMission.getPrereqGameOption()))
	{
		return false;
	}
	if (kMission.isNuke() && !isNukesValid())
	{
		return false;
	}
	if (kMission.isRevolt() && !GC.getGame().isOption(GAMEOPTION_UNSUPPORTED_REVOLUTION))
	{
		return false;
	}
	if (kMission.isDisablePower())
	{
		if (pPlot == NULL)
		{
			return false;
		}
		const CvCity* pCity = pPlot->getPlotCity();
		if (pCity == NULL)
		{
			return false;
		}
		if (!pCity->isPower())
		{
			return false;
		}
	}
	if (kMission.getWarWearinessCounter() > 0 && !GET_TEAM(GET_PLAYER(eTargetPlayer).getTeam()).isAtWar())
	{
		return false;
	}
	return true;
}

int CvPlayer::getEspionageMissionCost(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvPlot* pPlot, int iExtraData, const CvUnit* pSpyUnit) const
{
	int64_t iMissionCost = getEspionageMissionBaseCost(eMission, eTargetPlayer, pPlot, iExtraData, pSpyUnit);

	if (-1 == iMissionCost)
	{
		return -1;
	}

	iMissionCost *= getEspionageMissionCostModifier(eMission, eTargetPlayer, pPlot, pSpyUnit);
	iMissionCost /= 100;

	// Multiply cost of mission * number of team members
	iMissionCost *= GET_TEAM(getTeam()).getNumMembers();

	if (iMissionCost > MAX_INT)
	{
		iMissionCost = MAX_INT;
	}
	return std::max(0, (int)iMissionCost);
}

int64_t CvPlayer::getEspionageMissionBaseCost(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvPlot* pPlot, int iExtraData, const CvUnit* pSpyUnit) const
{
	PROFILE_EXTRA_FUNC();
	const CvEspionageMissionInfo& kMission = GC.getEspionageMissionInfo(eMission);
	int iBaseMissionCost = kMission.getCost();

	// -1 means this mission is disabled
	if (iBaseMissionCost == -1)
	{
		return -1;
	}

	CvCity* pCity = NULL;
	if (pPlot)
	{
		pCity = pPlot->getPlotCity();
	}

	if (kMission.isSelectPlot())
	{
		if (!pPlot)
		{
			return -1;
		}

		if (!pPlot->isRevealed(getTeam(), false))
		{
			return -1;
		}
	}

	if (!pCity && kMission.isTargetsCity())
	{
		return -1;
	}

	int64_t iMissionCost = -1;

	if (kMission.getStealTreasuryTypes() > 0)
	{
		// Steal Treasury
		const int64_t iGold = GET_PLAYER(eTargetPlayer).getGold() * kMission.getStealTreasuryTypes() / 100;
		int64_t iNumTotalGold = (iGold * kMission.getStealTreasuryTypes()) / 100;

		if (pCity)
		{
			iNumTotalGold *= pCity->getPopulation();
			iNumTotalGold /= std::max(1, GET_PLAYER(eTargetPlayer).getTotalPopulation());
		}

		if (iNumTotalGold > 0)
		{
			iMissionCost = (iBaseMissionCost * iNumTotalGold) / 100;
		}
	}
	else if (kMission.getBuyTechCostFactor() > 0)
	{
		// Buy (Steal) Tech
		TechTypes eTech = (TechTypes)iExtraData;
		int iProdCost = MAX_INT;

		if (NO_TECH == eTech)
		{
			for (int iTech = 0; iTech < GC.getNumTechInfos(); ++iTech)
			{
				if (canStealTech(eTargetPlayer, (TechTypes)iTech))
				{
					int iCost = GET_TEAM(getTeam()).getResearchCost((TechTypes)iTech);
					if (iCost < iProdCost)
					{
						iProdCost = iCost;
						eTech = (TechTypes)iTech;
					}
				}
			}
		}
		else
		{
			iProdCost = GET_TEAM(getTeam()).getResearchCost(eTech);
		}

		if (NO_TECH != eTech && canStealTech(eTargetPlayer, eTech))
		{
			iMissionCost = iBaseMissionCost + ((100 + kMission.getBuyTechCostFactor()) * iProdCost) / 100;
		}
	}
	else if (kMission.getSwitchCivicCostFactor() > 0)
	{
		// Switch Civics
		CivicTypes eCivic = (CivicTypes)iExtraData;

		if (NO_CIVIC == eCivic)
		{
			for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); ++iCivic)
			{
				if (canForceCivics(eTargetPlayer, (CivicTypes)iCivic))
				{
					eCivic = (CivicTypes)iCivic;
					break;
				}
			}
		}

		if (NO_CIVIC != eCivic && canForceCivics(eTargetPlayer, eCivic))
		{
			iMissionCost = iBaseMissionCost + kMission.getSwitchCivicCostFactor() * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 10000;
		}
	}
	else if (kMission.getSwitchReligionCostFactor() > 0)
	{
		// Switch Religions
		ReligionTypes eReligion = (ReligionTypes)iExtraData;

		if (NO_RELIGION == eReligion)
		{
			for (int iReligion = 0; iReligion < GC.getNumReligionInfos(); ++iReligion)
			{
				if (canForceReligion(eTargetPlayer, (ReligionTypes)iReligion))
				{
					eReligion = (ReligionTypes)iReligion;
					break;
				}
			}
		}

		if (NO_RELIGION != eReligion && canForceReligion(eTargetPlayer, eReligion))
		{
			iMissionCost = iBaseMissionCost + kMission.getSwitchReligionCostFactor() * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 10000;
		}
	}
	else if (kMission.getDestroyUnitCostFactor() > 0)
	{
		if (pSpyUnit && pSpyUnit->canAssassin(pPlot, false))
		{
			iMissionCost = iBaseMissionCost * kMission.getDestroyUnitCostFactor();
		}
	}
	else if (kMission.getDestroyProjectCostFactor() > 0)
	{
		ProjectTypes eProject = (ProjectTypes) iExtraData;
		int iCost = MAX_INT;

		if (NO_PROJECT == eProject)
		{
			for (int iProject = 0; iProject < GC.getNumProjectInfos(); ++iProject)
			{
				if (canSpyDestroyProject(eTargetPlayer, (ProjectTypes)iProject))
				{
					int iValue = getProductionNeeded((ProjectTypes)iProject);

					if (iValue < iCost)
					{
						iCost = iValue;
						eProject = (ProjectTypes)iProject;
					}
				}
			}
		}
		else
		{
			iCost = getProductionNeeded(eProject);
		}

		if (NO_PROJECT != eProject)
		{
			if (canSpyDestroyProject(eTargetPlayer, eProject))
			{
				iMissionCost = iBaseMissionCost + ((100 + kMission.getDestroyProjectCostFactor()) * iCost) / 100;
			}
		}
	}
	else if (kMission.getDestroyProductionCostFactor() > 0)
	{
		FAssert(pCity);
		if (pCity)
		{
			iMissionCost = iBaseMissionCost + ((100 + kMission.getDestroyProductionCostFactor()) * pCity->getProductionProgress()) / 100;
		}
	}
	else if (kMission.getBuyUnitCostFactor() > 0)
	{
		//Bribe
		if (pSpyUnit && pSpyUnit->canBribe(pPlot, false))
		{
			iMissionCost = iBaseMissionCost * kMission.getBuyUnitCostFactor();
		}
	}
	else if (kMission.getDestroyBuildingCostFactor() > 0)
	{
		if (pCity)
		{
			BuildingTypes eBuilding = (BuildingTypes) iExtraData;

			if (NO_BUILDING == eBuilding)
			{
				int iCost = MAX_INT;
				foreach_(const BuildingTypes eTypeX, pCity->getHasBuildings())
				{
					if (canSpyDestroyBuilding(eTargetPlayer, eTypeX) && !pCity->isDisabledBuilding(eTypeX))
					{
						const int iValue = getProductionNeeded(eTypeX);

						if (iValue < iCost)
						{
							iCost = iValue;
							eBuilding = eTypeX;
						}
					}
				}
				if (NO_BUILDING != eBuilding)
				{
					iMissionCost = iBaseMissionCost + (100 + kMission.getDestroyBuildingCostFactor()) * iCost / 100;
				}
			}
			else if (pCity->isActiveBuilding(eBuilding) && canSpyDestroyBuilding(eTargetPlayer, eBuilding))
			{
				iMissionCost = iBaseMissionCost + (100 + kMission.getDestroyBuildingCostFactor()) * getProductionNeeded(eBuilding) / 100;
			}
		}
	}
	else if (kMission.getBuyCityCostFactor() > 0)
	{
		// Buy City
		if (pCity)
		{
			iMissionCost = iBaseMissionCost + kMission.getBuyCityCostFactor() * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 10000;
		}
	}
	else if (kMission.getCityInsertCultureCostFactor() > 0)
	{
		// Insert Culture into City
		if (pPlot && pPlot->getCulture(getID()) > 0)
		{
			int iCultureAmount = kMission.getCityInsertCultureAmountFactor() *  pCity->countTotalCultureTimes100();
			iCultureAmount /= 10000;
			iCultureAmount = std::max(1, iCultureAmount);
			iMissionCost = iBaseMissionCost + (kMission.getCityInsertCultureCostFactor() * iCultureAmount) / 100;
		}
	}
	else if (kMission.isDestroyImprovement())
	{
		if (pPlot && !pPlot->isCity()
		&& (pPlot->getImprovementType() != NO_IMPROVEMENT || pPlot->getRouteType() != NO_ROUTE))
		{
			iMissionCost = iBaseMissionCost * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent() / 100;
		}
	}
	else if (kMission.getCityPoisonWaterCounter() > 0)
	{
		FAssert(pCity);
		// Cannot poison a city's water supply if it's already poisoned (value is negative when active)
		if (pCity && pCity->getEspionageHealthCounter() <= 0)
		{
			iMissionCost = iBaseMissionCost;
		}
	}

	// Make city unhappy
	else if (kMission.getCityUnhappinessCounter() > 0)
	{
		FAssert(pCity);
		// Cannot make a city unhappy if you've already done it (value is negative when active)
		if (pCity && pCity->getEspionageHappinessCounter() <= 0)
		{
			iMissionCost = iBaseMissionCost;
		}
	}

	// Make city Revolt
	else if (kMission.getCityRevoltCounter() > 0)
	{
		FAssert(pCity);
		// Cannot make a city revolt if it's already revolting
		if (pCity && pCity->getOccupationTimer() == 0)
		{
			iMissionCost = iBaseMissionCost;
		}
	}
	else if (kMission.getCounterespionageMod() > 0)
	{
		if (GET_TEAM(getTeam()).getCounterespionageTurnsLeftAgainstTeam(GET_PLAYER(eTargetPlayer).getTeam()) <= 0)
		{
			iMissionCost = iBaseMissionCost * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 100;
		}
	}
	else if (kMission.getPlayerAnarchyCounter() > 0)
	{
		// Player anarchy timer: can't add more turns of anarchy to player already in the midst of it
		if (!GET_PLAYER(eTargetPlayer).isAnarchy())
		{
			iMissionCost = iBaseMissionCost * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 100;
		}
	}
	else if (kMission.getSabatogeResearchCostFactor() > 0)
	{
		if (GET_PLAYER(eTargetPlayer).getCurrentResearch() != NO_TECH)
		{
			if (GET_TEAM(GET_PLAYER(eTargetPlayer).getTeam()).getResearchProgress(GET_PLAYER(eTargetPlayer).getCurrentResearch()) > 0)
			{
				iMissionCost = iBaseMissionCost + (kMission.getSabatogeResearchCostFactor() * GET_TEAM(GET_PLAYER(eTargetPlayer).getTeam()).getResearchProgress(GET_PLAYER(eTargetPlayer).getCurrentResearch()) / 100);
			}
		}
	}
	else if (kMission.getRemoveReligionsCostFactor() > 0)
	{
		if (pCity)
		{
			ReligionTypes eReligion = (ReligionTypes)iExtraData;

			if (NO_RELIGION == eReligion)
			{
				for (int iReligion = 0; iReligion < GC.getNumReligionInfos(); ++iReligion)
				{
					if (pCity->isHasReligion((ReligionTypes)iReligion) && !pCity->isHolyCity((ReligionTypes)iReligion))
					{
						eReligion = (ReligionTypes)iReligion;
						break;
					}
				}
			}

			if (NO_RELIGION != eReligion && pCity->isHasReligion(eReligion) && !pCity->isHolyCity(eReligion))
			{
				iMissionCost = iBaseMissionCost + kMission.getRemoveReligionsCostFactor() * pCity->getPopulation() / pCity->getNonHolyReligionCount();

				if (pCity->getNonHolyReligionCount() == 1)
				{
					iMissionCost *= 2;
				}
				if (GET_PLAYER(eTargetPlayer).getStateReligion() == eReligion)
				{
					iMissionCost *= 2;
				}
				iMissionCost *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
				iMissionCost /= 100;
			}
		}
	}
	else if (kMission.getRemoveCorporationsCostFactor() > 0)
	{
		if (pCity)
		{
			CorporationTypes eCorporation = (CorporationTypes)iExtraData;

			if (NO_CORPORATION == eCorporation)
			{
				for (int iCorp = 0; iCorp < GC.getNumCorporationInfos(); ++iCorp)
				{
					if (pCity->isActiveCorporation((CorporationTypes)iCorp) && GC.getGame().getHeadquarters((CorporationTypes)iCorp) != pCity)
					{
						eCorporation = (CorporationTypes)iCorp;
						break;
					}
				}
			}

			if (NO_CORPORATION != eCorporation && pCity->isActiveCorporation(eCorporation)&& GC.getGame().getHeadquarters(eCorporation) != pCity)
			{
				iMissionCost = iBaseMissionCost + (kMission.getRemoveCorporationsCostFactor() * pCity->getPopulation() / pCity->getCorporationCount());

				if (GC.getGame().getHeadquarters(eCorporation) && GC.getGame().getHeadquarters(eCorporation)->getOwner() == eTargetPlayer)
				{
					iMissionCost *= 2;
				}
				iMissionCost *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
				iMissionCost /= 100;
			}
		}
	}
	else if (kMission.isPassive())
	{
		iMissionCost = iBaseMissionCost * (100 + GET_TEAM(GET_PLAYER(eTargetPlayer).getTeam()).getEspionagePointsAgainstTeam(getTeam())) / 100;
	}
	else
	{
		iMissionCost = iBaseMissionCost * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 100;
	}

	if (iMissionCost < 0)
	{
		return -1;
	}

	return iMissionCost;
}


int CvPlayer::getEspionageMissionCostModifier(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvPlot* pPlot, const CvUnit* pSpyUnit) const
{
	const CvEspionageMissionInfo& kMission = GC.getEspionageMissionInfo(eMission);
	int64_t iModifier = 100;

	CvCity* pCity = NULL;
	if (pPlot)
	{
		pCity = pPlot->getPlotCity();
	}

	if (NO_PLAYER == eTargetPlayer)
	{
		eTargetPlayer = getID();
	}

	if (pCity && kMission.isTargetsCity())
	{
		// City Population
		iModifier *= 100 + (GC.getDefineINT("ESPIONAGE_CITY_POP_EACH_MOD") * (pCity->getPopulation() - 1));
		iModifier /= 100;

		// Trade Route
		if (pCity->isTradeRoute(getID()))
		{
			iModifier *= 100 + GC.getDefineINT("ESPIONAGE_CITY_TRADE_ROUTE_MOD");
			iModifier /= 100;
		}

		// City has Your State Religion
		ReligionTypes eReligion = getStateReligion();
		if (NO_RELIGION != eReligion && pCity->isHasReligion(eReligion))
		{
			int64_t iReligionModifier = 0;

			if (GET_PLAYER(eTargetPlayer).getStateReligion() != eReligion)
			{
				iReligionModifier += GC.getDefineINT("ESPIONAGE_CITY_RELIGION_STATE_MOD");
			}
			if (hasHolyCity(eReligion))
			{
				iReligionModifier += GC.getDefineINT("ESPIONAGE_CITY_HOLY_CITY_MOD");
			}
			iModifier *= 100 + iReligionModifier;
			iModifier /= 100;
		}

		// City's culture affects cost
		iModifier *= 100 - (pCity->getCultureTimes100(getID()) * GC.getDefineINT("ESPIONAGE_CULTURE_MULTIPLIER_MOD")) / std::max(1, pCity->getCultureTimes100(eTargetPlayer) + pCity->getCultureTimes100(getID()));
		iModifier /= 100;

		iModifier *= 100 + pCity->getEspionageDefenseModifier();
		iModifier /= 100;
	}

	// Distance mod
	if (pPlot)
	{
		int64_t iDistance = GC.getMap().maxPlotDistance();

		CvCity* pOurCapital = getCapitalCity();
		if (pOurCapital)
		{
			if (kMission.isSelectPlot() || kMission.isTargetsCity())
			{
				iDistance = plotDistance(pOurCapital->getX(), pOurCapital->getY(), pPlot->getX(), pPlot->getY());
			}
			else
			{
				CvCity* pTheirCapital = GET_PLAYER(eTargetPlayer).getCapitalCity();
				if (pTheirCapital)
				{
					iDistance = plotDistance(pOurCapital->getX(), pOurCapital->getY(), pTheirCapital->getX(), pTheirCapital->getY());
				}
			}
		}

		iModifier *= (iDistance + GC.getMap().maxPlotDistance()) * GC.getDefineINT("ESPIONAGE_DISTANCE_MULTIPLIER_MOD") / GC.getMap().maxPlotDistance();
		iModifier /= 100;
	}

	// Spy presence mission cost alteration
	if (pSpyUnit)
	{
		iModifier *= 100 - std::min(5, pSpyUnit->getFortifyTurns() + pSpyUnit->getUpgradeDiscount() / 10) * GC.getDefineINT("ESPIONAGE_EACH_TURN_UNIT_COST_DECREASE");
		iModifier /= 100;
	}

	// My points VS. Your points to mod cost
	int64_t iTargetPoints = GET_TEAM(GET_PLAYER(eTargetPlayer).getTeam()).getEspionagePointsEver();
	int64_t iOurPoints = GET_TEAM(getTeam()).getEspionagePointsEver();
	int64_t iEquation1 = 2 * iTargetPoints + iOurPoints;
	int64_t iEquation2 = iTargetPoints + 2 * iOurPoints;
	if (iEquation2 < 1)
	{
		iEquation2 = 1;
	}
	iModifier *= (((int64_t)GC.getDefineINT("ESPIONAGE_SPENDING_MULTIPLIER") * iEquation1) / iEquation2);
	iModifier /= 100;

	// Counterespionage Mission Mod
	CvTeam& kTargetTeam = GET_TEAM(GET_PLAYER(eTargetPlayer).getTeam());
	if (kTargetTeam.getCounterespionageModAgainstTeam(getTeam()) > 0)
	{
		iModifier *= kTargetTeam.getCounterespionageModAgainstTeam(getTeam());
		iModifier /= 100;
	}

	if (pCity)
	{
		if (pCity == GET_PLAYER(eTargetPlayer).getCapitalCity() && GET_TEAM(getTeam()).isHasEmbassy(GET_PLAYER(eTargetPlayer).getTeam()))
		{
			iModifier *= 100 - GC.getDefineINT("EMBASSY_ESPIONAGE_MISSION_COST_MODIFIER");
			iModifier /= 100;
		}
		if (GET_TEAM(getTeam()).isFreeTradeAgreement(GET_PLAYER(eTargetPlayer).getTeam()))
		{
			iModifier *= 100 - GC.getDefineINT("FREE_TRADE_AGREEMENT_ESPIONAGE_MISSION_COST_MODIFIER");
			iModifier /= 100;
		}
	}

	iModifier += GC.getESPIONAGE_MISSION_COST_END_TOTAL_PERCENT_ADJUSTMENT();

	if (iModifier > MAX_INT)
	{
		iModifier = MAX_INT;
	}
	return (int)iModifier;
}


bool CvPlayer::doEspionageMission(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, CvPlot* pPlot, int iExtraData, CvUnit* pSpyUnit, bool bCaught)
{
	PROFILE_EXTRA_FUNC();
	if (!canDoEspionageMission(eMission, eTargetPlayer, pPlot, iExtraData, pSpyUnit))
	{
		return false;
	}
	char szSound[1024] = "AS2D_DEAL_CANCELLED";

	const TeamTypes eTargetTeam = NO_PLAYER != eTargetPlayer ? GET_PLAYER(eTargetPlayer).getTeam() : NO_TEAM;

	const CvEspionageMissionInfo& kMission = GC.getEspionageMissionInfo(eMission);

	bool bSomethingHappened = false;
	bool bShowExplosion = false;
	CvWString szBuffer;
	const int iMissionCost = getEspionageMissionCost(eMission, eTargetPlayer, pPlot, iExtraData, pSpyUnit);

	CvCity* pCity;

	if (pPlot)
	{
		pCity = pPlot->getPlotCity();

		if (pCity)
		{
			// Destroy Building
			if (kMission.getDestroyBuildingCostFactor() > 0)
			{
				pCity->changeHasBuilding((BuildingTypes)iExtraData, false);
				bSomethingHappened = true;
				bShowExplosion = true;
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SOMETHING_DESTROYED_IN",
					GC.getBuildingInfo((BuildingTypes)iExtraData).getDescription(), pCity->getNameKey()).GetCString();
			}
			// Destroy Project
			if (kMission.getDestroyProjectCostFactor() > 0)
			{
				GET_TEAM(eTargetTeam).changeProjectCount((ProjectTypes)iExtraData, -1);
				bSomethingHappened = true;
				bShowExplosion = true;
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SOMETHING_DESTROYED_IN",
					GC.getProjectInfo((ProjectTypes)iExtraData).getDescription(), pCity->getNameKey()).GetCString();
			}
			// Destroy Production
			if (kMission.getDestroyProductionCostFactor() > 0)
			{
				pCity->setProductionProgress(0);
				bSomethingHappened = true;
				bShowExplosion = true;
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_PRODUCTION_DESTROYED_IN", pCity->getProductionName(), pCity->getNameKey());
			}
			// Assassinate (Specialist)
			if (pSpyUnit && kMission.getDestroyUnitCostFactor() > 0 && pSpyUnit->canAssassin(pPlot, false) && iExtraData >= 7)
			{
				pCity->changeFreeSpecialistCount((SpecialistTypes)iExtraData, -1);
				bSomethingHappened = true;
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SOMETHING_ASSASSINATED",
					GC.getSpecialistInfo((SpecialistTypes)iExtraData).getDescription(), pCity->getNameKey()).GetCString();
			}
			// Buy City
			if (kMission.getBuyCityCostFactor() > 0)
			{
				GET_PLAYER(pCity->getOwner()).AI_changeMemoryCount(getID(), MEMORY_EVENT_BAD_TO_US, std::max(4, pCity->getPopulation()));

				for (int iI = 0; iI < MAX_PLAYERS; iI++)
				{
					if(GET_PLAYER((PlayerTypes)iI).isAlive()
					&& GET_PLAYER((PlayerTypes)iI).AI_getAttitude(pCity->getOwner()) >= ATTITUDE_PLEASED)
					{
						GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount(getID(), MEMORY_EVENT_BAD_TO_US, std::max(2, pCity->getPopulation() / 3));
					}
				}
				if (GET_PLAYER(pCity->getOwner()).AI_getMemoryCount(getID(), MEMORY_EVENT_BAD_TO_US) > 15)
				{
					GET_TEAM(pCity->getTeam()).declareWar(getTeam(), true, WARPLAN_TOTAL);
				}
				strcpy(szSound, "AS2D_REVOLTSTART");
				acquireCity(pCity, false, true, true);
				bSomethingHappened = true;
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_CITY_BOUGHT", pCity->getNameKey()).GetCString();
			}
			// Insert Culture into City
			if (kMission.getCityInsertCultureCostFactor() > 0)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_CITY_CULTURE_INSERTED", pCity->getNameKey()).GetCString();

				const int iCultureAmount = std::max(1, kMission.getCityInsertCultureAmountFactor() * pCity->countTotalCultureTimes100() / 10000);

				int iPlotCulture = pCity->plot()->countTotalCulture();
				int iPlotCultureAmount;

				if (iPlotCulture > MAX_INT / 10000)
				{
					iPlotCulture /= 100;
					iPlotCultureAmount = kMission.getCityInsertCultureAmountFactor() * iPlotCulture;
				}
				else
				{
					iPlotCultureAmount = kMission.getCityInsertCultureAmountFactor() * iPlotCulture * 100;
					iPlotCultureAmount /= 10000;
				}
				iPlotCultureAmount = std::max(1, iPlotCultureAmount);

				const int iNumTurnsApplied = GC.getDefineINT("GREAT_WORKS_CULTURE_TURNS") * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 100;

				if (iNumTurnsApplied > 0)
				{
					for (int i = 0; i < iNumTurnsApplied; ++i)
					{
						pCity->changeCulture(getID(), iCultureAmount / iNumTurnsApplied, true, true);
						pCity->plot()->changeCulture(getID(), iPlotCultureAmount / iNumTurnsApplied, false);
					}
					pCity->changeCulture(getID(), iCultureAmount % iNumTurnsApplied, false, true);
					pCity->plot()->changeCulture(getID(), iPlotCultureAmount % iNumTurnsApplied, false);
				}
				bSomethingHappened = true;
			}
			// Poison City's Water Supply
			if (kMission.getCityPoisonWaterCounter() > 0)
			{
				int iTurns = kMission.getCityPoisonWaterCounter() * (100 + (pSpyUnit ? pSpyUnit->getExtraFriendlyHeal() : 0)) / 100;
					iTurns *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
					iTurns /= 100;
				pCity->changeEspionageHealthCounter(iTurns);
				bShowExplosion = true;
				bSomethingHappened = true;
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_CITY_POISONED", pCity->getNameKey()).GetCString();
			}
			// Make city Unhappy
			if (kMission.getCityUnhappinessCounter() > 0)
			{
				int iTurns = kMission.getCityUnhappinessCounter() * (100 + (pSpyUnit ? pSpyUnit->getExtraEnemyHeal() : 0)) / 100;
					iTurns *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
					iTurns /= 100;
				pCity->changeEspionageHappinessCounter(iTurns);
				bShowExplosion = true;
				bSomethingHappened = true;
				strcpy(szSound, "AS2D_REVOLTSTART");
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_CITY_UNHAPPY", pCity->getNameKey()).GetCString();
			}
			// Make city Revolt
			if (kMission.getCityRevoltCounter() > 0)
			{
				int iTurns = kMission.getCityRevoltCounter() * (100 + (pSpyUnit ? pSpyUnit->getExtraNeutralHeal() :0)) / 100;
					iTurns *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
					iTurns /= 100;
				pCity->changeCultureUpdateTimer(iTurns);
				pCity->changeOccupationTimer(iTurns);
				bSomethingHappened = true;
				bShowExplosion = true;
				strcpy(szSound, "AS2D_REVOLTSTART");
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_CITY_REVOLT", pCity->getNameKey()).GetCString();

				if (gUnitLogLevel >= 2)
				{
					logBBAI("	  Spy for player %d (%S) causes revolt in %S, owned by %S (%d)", getID(), getCivilizationDescription(0), pCity->getName().GetCString(), GET_PLAYER(pCity->getOwner()).getCivilizationDescription(0), pCity->getOwner() );
				}
			}
			// Incite City Revolution
			if (kMission.isRevolt())
			{
				pCity->changeRevolutionIndex(1000 + GC.getGame().getSorenRandNum(1000, "City Revolt"));
				pCity->changeOccupationTimer(kMission.getCityRevoltCounter());
				bSomethingHappened = true;
				strcpy(szSound, "AS2D_REVOLTSTART");
				szBuffer = bCaught ?
					gDLL->getText("TXT_KEY_ESPIONAGE_INCITE_REVOLUTION_CAUGHT", pCity->getNameKey(), getCivilizationDescription()).GetCString()
					:
					gDLL->getText("TXT_KEY_ESPIONAGE_INCITE_REVOLUTION", pCity->getNameKey()).GetCString();
			}
			// Disable Power
			if (kMission.isDisablePower())
			{
				pCity->changeDisabledPowerTimer(6*GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent()/100);
				bSomethingHappened = true;
				strcpy(szSound, "AS2D_BUILD_PLANTNUCLEAR");
				szBuffer = bCaught ?
					gDLL->getText("TXT_KEY_ESPIONAGE_POWER_CUT_CAUGHT", pCity->getNameKey(), getCivilizationDescription()).GetCString()
					:
					gDLL->getText("TXT_KEY_ESPIONAGE_POWER_CUT", pCity->getNameKey()).GetCString();
			}
			// Cause War Weariness
			if (kMission.getWarWearinessCounter() > 0)
			{
				int iAmount = kMission.getWarWearinessCounter();
				iAmount *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
				iAmount /= 100;
				pCity->changeWarWearinessTimer(iAmount);
				bSomethingHappened = true;
				strcpy(szSound, "AS2D_STRIKE");
				szBuffer = bCaught ?
					gDLL->getText("TXT_KEY_ESPIONAGE_WAR_WEARINESS_CAUGHT", pCity->getNameKey(), getCivilizationAdjectiveKey()).GetCString()
					:
					gDLL->getText("TXT_KEY_ESPIONAGE_WAR_WEARINESS", pCity->getNameKey()).GetCString();
			}
			// Sabatoge Research
			if (kMission.getSabatogeResearchCostFactor() > 0)
			{
				GET_TEAM(pCity->getTeam()).setResearchProgress(GET_PLAYER(pCity->getOwner()).getCurrentResearch(), 0, pCity->getOwner());
				bSomethingHappened = true;
				szBuffer = bCaught ?
					gDLL->getText("TXT_KEY_ESPIONAGE_RESEARCH_SABATAGED_CAUGHT", GC.getTechInfo(GET_PLAYER(pCity->getOwner()).getCurrentResearch()).getDescription(), getCivilizationAdjectiveKey()).GetCString()
					:
					gDLL->getText("TXT_KEY_ESPIONAGE_RESEARCH_SABATAGED", GC.getTechInfo(GET_PLAYER(pCity->getOwner()).getCurrentResearch()).getDescription());
			}
			// Remove Religion
			if (kMission.getRemoveReligionsCostFactor() > 0)
			{
				pCity->setHasReligion((ReligionTypes)iExtraData, false, false, false);
				strcpy(szSound, GC.getReligionInfo((ReligionTypes)iExtraData).getSound());
				bSomethingHappened = true;
				szBuffer = bCaught ?
					gDLL->getText("TXT_KEY_ESPIONAGE_RELIGION_REMOVED_CAUGHT", pCity->getNameKey(), getCivilizationAdjectiveKey(), GC.getReligionInfo((ReligionTypes)iExtraData).getTextKeyWide()).GetCString()
					:
					gDLL->getText("TXT_KEY_ESPIONAGE_RELIGION_REMOVED", pCity->getNameKey(), GC.getReligionInfo((ReligionTypes)iExtraData).getTextKeyWide());
			}
			// Remove Corporation
			if (kMission.getRemoveCorporationsCostFactor() > 0)
			{
				CorporationTypes eCorporation = (CorporationTypes)iExtraData;

				if (NO_CORPORATION != eCorporation && pCity->isActiveCorporation(eCorporation)&& GC.getGame().getHeadquarters(eCorporation) != pCity)
				{
					pCity->setHasCorporation(eCorporation, false, false, false);
					strcpy(szSound, GC.getCorporationInfo(eCorporation).getSound());
					bSomethingHappened = true;
					szBuffer = bCaught ?
						gDLL->getText("TXT_KEY_ESPIONAGE_CORPORATION_REMOVED_CAUGHT", pCity->getNameKey(), getCivilizationAdjectiveKey(), GC.getCorporationInfo(eCorporation).getTextKeyWide()).GetCString()
						:
						gDLL->getText("TXT_KEY_ESPIONAGE_CORPORATION_REMOVED", pCity->getNameKey(), GC.getCorporationInfo(eCorporation).getTextKeyWide());
				}
			}
		}
		// Destroy Improvement
		if (kMission.isDestroyImprovement())
		{
			CvCity* pNearCity = GC.getMap().findCity(pPlot->getX(), pPlot->getY(), eTargetPlayer, GET_PLAYER(eTargetPlayer).getTeam(), true, false);
			if (pNearCity)
			{
				if (pPlot->getImprovementType() != NO_IMPROVEMENT)
				{
					szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SOMETHING_DESTROYED_NEAR_CITY", GC.getImprovementInfo(pPlot->getImprovementType()).getDescription(), pNearCity->getNameKey()).GetCString();
					pPlot->setImprovementType(GC.getImprovementInfo(pPlot->getImprovementType()).getImprovementPillage());
					bSomethingHappened = true;
				}
				else if (pPlot->getRouteType() != NO_ROUTE)
				{
					szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SOMETHING_DESTROYED_NEAR_CITY", GC.getRouteInfo(pPlot->getRouteType()).getDescription(), pNearCity->getNameKey()).GetCString();
					pPlot->setRouteType(NO_ROUTE, true);
					bSomethingHappened = true;
				}
			}
			else if (pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SOMETHING_DESTROYED", GC.getImprovementInfo(pPlot->getImprovementType()).getDescription()).GetCString();
				pPlot->setImprovementType(GC.getImprovementInfo(pPlot->getImprovementType()).getImprovementPillage());
				bSomethingHappened = true;
			}
			else if (pPlot->getRouteType() != NO_ROUTE)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SOMETHING_DESTROYED", GC.getRouteInfo(pPlot->getRouteType()).getDescription()).GetCString();
				pPlot->setRouteType(NO_ROUTE, true);
				bSomethingHappened = true;
			}

			if (pSpyUnit && pSpyUnit->isAmphib())
			{
				//TSHEEP Radiation Effect
				pPlot->setFeatureType((FeatureTypes)(GC.getDefineINT("NUKE_FEATURE")));
				szBuffer.append(NEWLINE);
				szBuffer.append(gDLL->getText("TXT_KEY_ESPIONAGE_USED_RADIATION").GetCString());
			}
			if (bSomethingHappened)
			{
				bShowExplosion = true;
			}
		}
		// Bribe (Buy Unit)
		if (NO_PLAYER != eTargetPlayer && pSpyUnit
		&& kMission.getBuyUnitCostFactor() > 0 && pSpyUnit->canBribe(pPlot, false))
		{
			CvUnit* pTargetUnit;
			if (pPlot->plotCheck(PUF_isOtherTeam, getID(), -1, NULL, NO_PLAYER, NO_TEAM, PUF_isVisible, getID()))
			{
				for (int i = 0; i < pPlot->getNumUnits(); i++)
				{
					pTargetUnit = pPlot->getUnitByIndex(i);
					if (pTargetUnit && pTargetUnit->AI_getUnitAIType() == UNITAI_WORKER)
					{
						if (pTargetUnit->getTeam() == eTargetTeam) break;
						pTargetUnit = NULL;
					}
				}
			}
			if (pTargetUnit && pTargetUnit->getTeam() == eTargetTeam)
			{
				FAssert(pTargetUnit->plot() == pPlot);
				CvCity* pNearCity = GC.getMap().findCity(pPlot->getX(), pPlot->getY(), eTargetPlayer, GET_PLAYER(eTargetPlayer).getTeam(), true, false);
				if (pNearCity)
					szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SOMETHING_BRIBED_NEAR_CITY", pTargetUnit->getNameKey(), pNearCity->getNameKey()).GetCString();
				else
					szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SOMETHING_BRIBED", pTargetUnit->getNameKey()).GetCString();

				int iX = pTargetUnit->getX();
				int iY = pTargetUnit->getY();
				pTargetUnit->kill(false, getID());
				CvUnit* acquiredWorker = initUnit(pTargetUnit->getUnitType(), iX, iY, UNITAI_WORKER, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
				CvCity* pCapital = this->getCapitalCity();
				if (pCapital && acquiredWorker)
				{
					iX = pCapital->getX();
					iY = pCapital->getY();
					//GC.getGame().logOOSSpecial(11, acquiredWorker->getID(), iX, iY);
					acquiredWorker->setXY(iX, iY, false, false, false);
					acquiredWorker->finishMoves();
				}
				bSomethingHappened = true;
			}
		}
	}
	// Steal Treasury
	if (NO_PLAYER != eTargetPlayer && kMission.getStealTreasuryTypes() > 0)
	{
		int64_t iNumTotalGold = GET_PLAYER(eTargetPlayer).getGold();

		iNumTotalGold *= kMission.getStealTreasuryTypes();
		iNumTotalGold /= 100;
		iNumTotalGold *= kMission.getStealTreasuryTypes();
		iNumTotalGold /= 100;

		if (pCity)
		{
			iNumTotalGold *= pCity->getPopulation();
			iNumTotalGold /= std::max(1, GET_PLAYER(eTargetPlayer).getTotalPopulation());
		}

		strcpy(szSound, "AS2D_WONDERGOLD");
		szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_STEAL_TREASURY").GetCString();

		changeGold(iNumTotalGold);
		GET_PLAYER(eTargetPlayer).changeGold(-iNumTotalGold);
		bSomethingHappened = true;
	}
	// Buy (Steal) Tech
	if (kMission.getBuyTechCostFactor() > 0)
	{
		GET_TEAM(getTeam()).setHasTech((TechTypes) iExtraData, true, getID(), false, true);

		if (GC.getGame().isOption(GAMEOPTION_NO_TECH_BROKERING))
		{
			GET_TEAM(getTeam()).setNoTradeTech((TechTypes) iExtraData, true);
		}
		bSomethingHappened = true;
		szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_TECH_BOUGHT", GC.getTechInfo((TechTypes) iExtraData).getDescription()).GetCString();
	}
	// Switch Civic
	if (NO_PLAYER != eTargetPlayer && kMission.getSwitchCivicCostFactor() > 0)
	{
		GET_PLAYER(eTargetPlayer).setCivics((CivicOptionTypes) GC.getCivicInfo((CivicTypes)iExtraData).getCivicOptionType(), (CivicTypes)iExtraData);
		GET_PLAYER(eTargetPlayer).setRevolutionTimer(std::max(1, ((100 + GET_PLAYER(eTargetPlayer).getAnarchyModifier()) * GC.getDefineINT("MIN_REVOLUTION_TURNS")) / 100));
		bSomethingHappened = true;
		szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SWITCH_CIVIC", GC.getCivicInfo((CivicTypes)iExtraData).getDescription()).GetCString();
	}
	// Switch Religion
	if (NO_PLAYER != eTargetPlayer && kMission.getSwitchReligionCostFactor() > 0)
	{
		strcpy(szSound, GC.getReligionInfo((ReligionTypes)iExtraData).getSound());
		szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SWITCH_RELIGION", GC.getReligionInfo((ReligionTypes)iExtraData).getDescription()).GetCString();

		GET_PLAYER(eTargetPlayer).setLastStateReligion((ReligionTypes)iExtraData);
		GET_PLAYER(eTargetPlayer).setConversionTimer(std::max(1, ((100 + GET_PLAYER(eTargetPlayer).getAnarchyModifier()) * GC.getDefineINT("MIN_CONVERSION_TURNS")) / 100));
		bSomethingHappened = true;
	}
	// Player Anarchy
	if (NO_PLAYER != eTargetPlayer && kMission.getPlayerAnarchyCounter() > 0)
	{
		GET_PLAYER(eTargetPlayer).changeAnarchyTurns(kMission.getPlayerAnarchyCounter() * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 100);
		bSomethingHappened = true;
		strcpy(szSound, "AS2D_REVOLTSTART");
		szBuffer = bCaught ?
			gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_PLAYER_ANARCHY_CAUGHT", getCivilizationAdjectiveKey()).GetCString()
			:
			gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_PLAYER_ANARCHY").GetCString();
	}
	// Counterespionage
	if (kMission.getCounterespionageNumTurns() > 0 && kMission.getCounterespionageMod() > 0)
	{
		szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_COUNTERESPIONAGE").GetCString();

		if (NO_TEAM != eTargetTeam)
		{
			const int iTurns = kMission.getCounterespionageNumTurns() * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 100;
			GET_TEAM(getTeam()).changeCounterespionageTurnsLeftAgainstTeam(eTargetTeam, iTurns);
			GET_TEAM(getTeam()).changeCounterespionageModAgainstTeam(eTargetTeam, kMission.getCounterespionageMod() + (pSpyUnit ? 5 * pSpyUnit->currInterceptionProbability() : 0));
			bSomethingHappened = true;
		}
	}
	// Nuclear Bomb
	const bool bNuked = pPlot && pSpyUnit && kMission.isNuke();
	if (bNuked)
	{
		pSpyUnit->spyNuke(pPlot->getX(), pPlot->getY(), bCaught);
	}

	int iHave = 0;
	if (NO_TEAM != eTargetTeam)
	{
		iHave = GET_TEAM(getTeam()).getEspionagePointsAgainstTeam(eTargetTeam);

		if (bSomethingHappened || bNuked)
		{
			GET_TEAM(getTeam()).changeEspionagePointsAgainstTeam(eTargetTeam, -iMissionCost);
		}
	}

	if (pPlot && bShowExplosion && pPlot->isVisible(GC.getGame().getActiveTeam(), false))
	{
		EffectTypes eEffect = GC.getEntityEventInfo(GC.getMissionInfo(MISSION_BOMBARD).getEntityEvent()).getEffectType();
		gDLL->getEngineIFace()->TriggerEffect(eEffect, pPlot->getPoint(), (float)(GC.getASyncRand().get(360)));
		gDLL->getInterfaceIFace()->playGeneralSound("AS3D_UN_CITY_EXPLOSION", pPlot->getPoint());
	}

	if (bSomethingHappened)
	{
		int iX = -1;
		int iY = -1;
		if (pPlot)
		{
			iX = pPlot->getX();
			iY = pPlot->getY();
		}
		AddDLLMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_ESPIONAGE_MISSION_PERFORMED"), "AS2D_POSITIVE_DINK", MESSAGE_TYPE_INFO, ARTFILEMGR.getInterfaceArtInfo("ESPIONAGE_BUTTON")->getPath(), GC.getCOLOR_GREEN(), iX, iY, true, true);
	}
	else if (getID() == GC.getGame().getActivePlayer() && !bNuked)
	{
		CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_TEXT);
		if (iHave < iMissionCost)
		{
			pInfo->setText(gDLL->getText("TXT_KEY_ESPIONAGE_TOO_EXPENSIVE", iMissionCost, iHave));
		}
		else
		{
			pInfo->setText(gDLL->getText("TXT_KEY_ESPIONAGE_CANNOT_DO_MISSION"));
		}
		addPopup(pInfo);
	}

	if (bSomethingHappened && !szBuffer.empty())
	{
		int iX = -1;
		int iY = -1;
		if (pPlot)
		{
			iX = pPlot->getX();
			iY = pPlot->getY();
		}

		if (NO_PLAYER != eTargetPlayer)
		{
			AddDLLMessage(eTargetPlayer, true, GC.getEVENT_MESSAGE_TIME(), szBuffer, szSound, MESSAGE_TYPE_INFO, ARTFILEMGR.getInterfaceArtInfo("ESPIONAGE_BUTTON")->getPath(), GC.getCOLOR_RED(), iX, iY, true, true);
		}
	}
	return bSomethingHappened || bNuked;
}

int CvPlayer::getEspionageSpendingWeightAgainstTeam(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_aiEspionageSpendingWeightAgainstTeam[eIndex];
}

void CvPlayer::setEspionageSpendingWeightAgainstTeam(TeamTypes eIndex, int iValue)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	FASSERT_NOT_NEGATIVE(iValue);

	iValue = std::min(std::max(0, iValue), 99);

	if (iValue != getEspionageSpendingWeightAgainstTeam(eIndex))
	{
		m_aiEspionageSpendingWeightAgainstTeam[eIndex] = iValue;

		gDLL->getInterfaceIFace()->setDirty(Espionage_Advisor_DIRTY_BIT, true);
	}
}

void CvPlayer::changeEspionageSpendingWeightAgainstTeam(TeamTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	setEspionageSpendingWeightAgainstTeam(eIndex, getEspionageSpendingWeightAgainstTeam(eIndex) + iChange);
}

void CvPlayer::doAdvancedStartAction(AdvancedStartActionTypes eAction, int iX, int iY, int iData, bool bAdd)
{
	PROFILE_EXTRA_FUNC();
	if (getAdvancedStartPoints() < 0)
	{
		return;
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);

	if (0 == getNumCities())
	{
		switch (eAction)
		{
		case ADVANCEDSTARTACTION_EXIT:
			//Try to build this player's empire
			if (getID() == GC.getGame().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setBusy(true);
			}
			AI_doAdvancedStart(true);
			if (getID() == GC.getGame().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setBusy(false);
			}
			break;
		case ADVANCEDSTARTACTION_AUTOMATE:
		case ADVANCEDSTARTACTION_CITY:
			break;
		default:
			// The first action must be to place a city
			// so players can lose by spending everything
			return;
		}
	}

	switch (eAction)
	{
	case ADVANCEDSTARTACTION_EXIT:
		changeGold(getAdvancedStartPoints());
		setAdvancedStartPoints(-1);
		if (GC.getGame().getActivePlayer() == getID())
		{
			gDLL->getInterfaceIFace()->setInAdvancedStart(false);
		}

		if (isHumanPlayer())
		{
			chooseTech();

			if (canRevolution(NULL))
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHANGECIVIC);
				if (pInfo)
				{
					gDLL->getInterfaceIFace()->addPopup(pInfo, getID());
				}
			}
		}
		break;
	case ADVANCEDSTARTACTION_AUTOMATE:
		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setBusy(true);
		}
		AI_doAdvancedStart(true);
		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setBusy(false);
		}
		break;
	case ADVANCEDSTARTACTION_UNIT:
		{
			if (pPlot == NULL)
				return;

			UnitTypes eUnit = (UnitTypes) iData;
			int iCost = getAdvancedStartUnitCost(eUnit, bAdd, pPlot);

			if (bAdd && iCost < 0)
			{
				return;
			}

			// Add unit to the map
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					CvUnit* pUnit = initUnit(eUnit, iX, iY, NO_UNITAI, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
					if (pUnit)
					{
						pUnit->finishMoves();
						changeAdvancedStartPoints(-iCost);
					}
				}
			}

			// Remove unit from the map
			else
			{
				// If cost is -1 we already know this unit isn't present
				if (iCost != -1)
				{
					CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
					while (pUnitNode)
					{
						CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
						pUnitNode = pPlot->nextUnitNode(pUnitNode);

						if (pLoopUnit->getUnitType() == eUnit)
						{
							pLoopUnit->kill(false);
							changeAdvancedStartPoints(iCost);
							return;
						}
					}
				}

				// Proper unit not found above, delete first found
				CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
				if (pUnitNode)
				{
					CvUnit* pUnit = ::getUnit(pUnitNode->m_data);

					iCost = getAdvancedStartUnitCost(pUnit->getUnitType(), false);
					FAssertMsg(iCost != -1, "If this is -1 then that means it's going to try to delete a unit which shouldn't exist");
					pUnit->kill(false);
					changeAdvancedStartPoints(iCost);
				}
			}

			if (getID() == GC.getGame().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_CITY:
		{
			if (pPlot == NULL)
				return;

			int iCost = getAdvancedStartCityCost(bAdd, pPlot);

			if (iCost < 0)
			{
				return;
			}

			// Add City to the map
			if (bAdd)
			{
				if (getNumCities() == 0)
				{
					PlayerTypes eClosestPlayer = NO_PLAYER;
					int iMinDistance = MAX_INT;
					for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; iPlayer++)
					{
						const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
						if (kPlayer.isAliveAndTeam(getTeam()) && kPlayer.getNumCities() == 0)
						{
							FAssert(kPlayer.getStartingPlot());
							int iDistance = plotDistance(iX, iY, kPlayer.getStartingPlot()->getX(), kPlayer.getStartingPlot()->getY());
							if (iDistance < iMinDistance)
							{
								eClosestPlayer = kPlayer.getID();
								iMinDistance = iDistance;
							}
						}
					}
					FAssertMsg(eClosestPlayer != NO_PLAYER, "Self at a minimum should always be valid");
					if (eClosestPlayer != getID())
					{
						CvPlot* pTempPlot = GET_PLAYER(eClosestPlayer).getStartingPlot();
						GET_PLAYER(eClosestPlayer).setStartingPlot(getStartingPlot(), false);
						setStartingPlot(pTempPlot, false);
					}
				}
				if (getAdvancedStartPoints() >= iCost || 0 == getNumCities())
				{
					found(iX, iY);
					changeAdvancedStartPoints(-std::min(iCost, getAdvancedStartPoints()));
					GC.getGame().updateColoredPlots();
					CvCity* pCity = pPlot->getPlotCity();
					if (pCity && pCity->getPopulation() > 1)
					{
						pCity->setFood(pCity->growthThreshold() / 2);
					}
				}
			}
			else // Remove City from the map
			{
				pPlot->setRouteType(NO_ROUTE, true);
				pPlot->getPlotCity()->kill(true);
				pPlot->setImprovementType(NO_IMPROVEMENT);
				changeAdvancedStartPoints(iCost);
			}
			if (getID() == GC.getGame().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_POP:
		{
			if (!pPlot) return;

			CvCity* pCity = pPlot->getPlotCity();

			if (!pCity) return;

			const int iCost = getAdvancedStartPopCost(bAdd, pCity);

			if (iCost < 0) return;

			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					pCity->changePopulation(1);
					changeAdvancedStartPoints(-iCost);
					pCity->setHighestPopulation(pCity->getPopulation());
				}
			}
			else if (pCity->getPopulation() > 1)
			{
				pCity->changePopulation(-1);
				changeAdvancedStartPoints(iCost);
				pCity->setHighestPopulation(pCity->getPopulation());
			}
		}
		break;
	case ADVANCEDSTARTACTION_CULTURE:
		{
			if (pPlot == NULL)
				return;

			CvCity* pCity = pPlot->getPlotCity();

			if (pCity)
			{
				int iCost = getAdvancedStartCultureCost(bAdd, pCity);

				if (iCost < 0)
				{
					return;
				}

				// Add Culture to the City
				if (bAdd)
				{
					if (getAdvancedStartPoints() >= iCost)
					{
						pCity->setCulture(getID(), pCity->getCultureThreshold(), true, true);
						changeAdvancedStartPoints(-iCost);
					}
				}
				else // Remove Culture from the city
				{
					const CultureLevelTypes eNewLevel = static_cast<CultureLevelTypes>(pCity->getCultureLevel() - 1);
					if (eNewLevel > NO_CULTURELEVEL)
					{
						pCity->setCulture(getID(), GC.getGame().getCultureThreshold(eNewLevel), true, true);
					}
					changeAdvancedStartPoints(iCost);
				}
			}
		}
		break;
	case ADVANCEDSTARTACTION_BUILDING:
		{
			if (pPlot == NULL)
				return;

			CvCity* pCity = pPlot->getPlotCity();

			if (pCity)
			{
				BuildingTypes eBuilding = (BuildingTypes) iData;
				int iCost = getAdvancedStartBuildingCost(eBuilding, bAdd, pCity);

				if (iCost < 0)
				{
					return;
				}

				// Add Building to the City
				if (bAdd)
				{
					if (getAdvancedStartPoints() >= iCost)
					{
						pCity->changeHasBuilding(eBuilding, true);
						changeAdvancedStartPoints(-iCost);
					}
				}
				else // Remove Building from the city
				{
					pCity->changeHasBuilding(eBuilding, false);
					changeAdvancedStartPoints(iCost);
				}
			}

			if (getID() == GC.getGame().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_ROUTE:
		{
			if (pPlot == NULL)
				return;

			RouteTypes eRoute = (RouteTypes) iData;
			int iCost = getAdvancedStartRouteCost(eRoute, bAdd, pPlot);

			if (bAdd && iCost < 0)
			{
				return;
			}

			// Add Route to the plot
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					pPlot->setRouteType(eRoute, true);
					changeAdvancedStartPoints(-iCost);
				}
			}

			// Remove Route from the Plot
			else
			{
				if (pPlot->getRouteType() != eRoute)
				{
					eRoute = pPlot->getRouteType();
					iCost = getAdvancedStartRouteCost(eRoute, bAdd);
				}

				if (iCost < 0)
				{
					return;
				}

				pPlot->setRouteType(NO_ROUTE, true);
				changeAdvancedStartPoints(iCost);
			}

			if (getID() == GC.getGame().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_IMPROVEMENT:
		{
			if (pPlot == NULL)
				return;

			ImprovementTypes eImprovement = (ImprovementTypes) iData;
			int iCost = getAdvancedStartImprovementCost(eImprovement, bAdd, pPlot);

			if (bAdd && iCost < 0)
			{
				return;
			}

			// Add Improvement to the plot
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					if (pPlot->getFeatureType() != NO_FEATURE)
					{
						foreach_(const BuildTypes& eBuild, GC.getImprovementInfo(eImprovement).getBuildTypes())
						{
							if (GC.getBuildInfo(eBuild).isFeatureRemove(pPlot->getFeatureType()) && canBuild(pPlot, eBuild))
							{
								pPlot->setFeatureType(NO_FEATURE);
								break;
							}
						}
					}

					pPlot->setImprovementType(eImprovement);

					changeAdvancedStartPoints(-iCost);
				}
			}

			// Remove Improvement from the Plot
			else
			{
				if (pPlot->getImprovementType() != eImprovement)
				{
					eImprovement = pPlot->getImprovementType();
					iCost = getAdvancedStartImprovementCost(eImprovement, bAdd, pPlot);
				}

				if (iCost < 0)
				{
					return;
				}

				pPlot->setImprovementType(NO_IMPROVEMENT);
				changeAdvancedStartPoints(iCost);
			}

			if (getID() == GC.getGame().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_TECH:
		{
			TechTypes eTech = (TechTypes) iData;
			int iCost = getAdvancedStartTechCost(eTech, bAdd);

			if (iCost < 0)
			{
				return;
			}

			// Add Tech to team
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					GET_TEAM(getTeam()).setHasTech(eTech, true, getID(), false, false);
					changeAdvancedStartPoints(-iCost);
				}
			}

			// Remove Tech from the Team
			else
			{
				GET_TEAM(getTeam()).setHasTech(eTech, false, getID(), false, false);
				changeAdvancedStartPoints(iCost);
			}

			if (getID() == GC.getGame().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_VISIBILITY:
		{
			if (pPlot == NULL)
				return;

			int iCost = getAdvancedStartVisibilityCost(pPlot);
			if (iCost < 0)
			{
				return;
			}
			if (getAdvancedStartPoints() >= iCost)
			{
				pPlot->setRevealed(getTeam(), true, true, NO_TEAM, true);
				changeAdvancedStartPoints(-iCost);
				if (getAdvancedStartCityCost(true, pPlot) >= 0)
				{
					NiColorA color(GC.getColorInfo(GC.getCOLOR_WHITE()).getColor());
					color.a = 0.4f;
					gDLL->getEngineIFace()->fillAreaBorderPlot(pPlot->getViewportX(), pPlot->getViewportY(), color, AREA_BORDER_LAYER_CITY_RADIUS);
				}
			}
		}
		break;
	default:
		FErrorMsg("error");
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing a unit
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartUnitCost(UnitTypes eUnit, bool bAdd, const CvPlot* pPlot) const
{
	if (0 == getNumCities())
	{
		return -1;
	}

	int iCost = getProductionNeeded(eUnit) * GC.getUnitInfo(eUnit).getAdvancedStartCost() / 100;
	if (iCost < 0)
	{
		return -1;
	}

	if (!pPlot)
	{
		if (bAdd && algo::none_of(cities(), CvCity::fn::canTrain(eUnit)))
		{
			return -1;
		}
		return iCost;
	}

	if (pPlot->getOwner() != getID())
	{
		return -1;
	}
	CvCity* pCity = pPlot->getPlotCity();

	if (!pCity)
	{
		if (0 == GC.getDefineINT("ADVANCED_START_ALLOW_UNITS_OUTSIDE_CITIES"))
		{
			return -1;
		}
		iCost = getModifiedIntValue(iCost, pCity->getProductionModifier(eUnit));
	}
	else iCost = getModifiedIntValue(iCost, getProductionModifier(eUnit));


	if (bAdd)
	{
		int iMaxUnitsPerCity = GC.getDefineINT("ADVANCED_START_MAX_UNITS_PER_CITY");
		if (iMaxUnitsPerCity >= 0)
		{
			if (GC.getUnitInfo(eUnit).isMilitarySupport() && getNumMilitaryUnits() >= iMaxUnitsPerCity * getNumCities())
			{
				return -1;
			}
		}

		if (pCity)
		{
			if (!pCity->canTrain(eUnit))
			{
				return -1;
			}
		}
		else
		{
			if (!pPlot->canTrain(eUnit, false))
			{
				return -1;
			}

			if (pPlot->isImpassable() && !GC.getUnitInfo(eUnit).isCanMoveImpassable())
			{
				return -1;
			}

			if (pPlot->getFeatureType() != NO_FEATURE)
			{
				if (algo::any_of_equal(GC.getUnitInfo(eUnit).getImpassableFeatures(), pPlot->getFeatureType()))
				{
					const TechTypes eTech = (TechTypes)GC.getUnitInfo(eUnit).getFeaturePassableTech(pPlot->getFeatureType());
					if (NO_TECH == eTech || !GET_TEAM(getTeam()).isHasTech(eTech))
					{
						return -1;
					}
				}
			}
			else
			{
				if (algo::any_of_equal(GC.getUnitInfo(eUnit).getImpassableTerrains(), pPlot->getTerrainType()))
				{
					const TechTypes eTech = (TechTypes)GC.getUnitInfo(eUnit).getTerrainPassableTech(pPlot->getTerrainType());
					if (NO_TECH == eTech || !GET_TEAM(getTeam()).isHasTech(eTech))
					{
						return -1;
					}
				}
			}
		}
	}
	// Must be this unit at plot in order to remove
	else if (algo::none_of(pPlot->units(), CvUnit::fn::getUnitType() == eUnit))
	{
		return -1;
	}
	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing a City
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartCityCost(bool bAdd, const CvPlot* pPlot) const
{
	PROFILE_EXTRA_FUNC();
	int iCost = getNewCityProductionValue();

	if (iCost < 0)
	{
		return -1;
	}
	// Valid plot?
	if (pPlot)
	{
		// Need valid plot to found on if adding
		if (bAdd)
		{
			if (!canFound(pPlot->getX(), pPlot->getY(), false))
			{
				return -1;
			}
		}
		// Need your own city present to remove
		else if (!pPlot->isCity() || pPlot->getPlotCity()->getOwner() != getID())
		{
			return -1;
		}

		// Is there a distance limit on how far a city can be placed from a player's start/another city?
		if (GC.getDefineINT("ADVANCED_START_CITY_PLACEMENT_MAX_RANGE") > 0)
		{
			PlayerTypes eClosestPlayer = NO_PLAYER;
			int iClosestDistance = MAX_INT;
			const int iMax = GC.getDefineINT("ADVANCED_START_CITY_PLACEMENT_MAX_RANGE");

			for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; ++iPlayer)
			{
				const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);

				if (kPlayer.isAlive())
				{
					const CvPlot* pStartingPlot = kPlayer.getStartingPlot();

					if (pStartingPlot)
					{
						const int iDistance = ::plotDistance(pPlot->getX(), pPlot->getY(), pStartingPlot->getX(), pStartingPlot->getY());
						if (iDistance <= iMax && (iDistance < iClosestDistance || iDistance == iClosestDistance && getTeam() != kPlayer.getTeam()))
						{
							iClosestDistance = iDistance;
							eClosestPlayer = kPlayer.getID();
						}
					}
				}
			}

			if (NO_PLAYER == eClosestPlayer || GET_PLAYER(eClosestPlayer).getTeam() != getTeam())
			{
				return -1;
			}
			//Only allow founding at someone elses start point if we have no cities and they have no cities.
			if (getID() != eClosestPlayer && (getNumCities() > 0 || GET_PLAYER(eClosestPlayer).getNumCities() > 0))
			{
				return -1;
			}
		}
	}

	// Increase cost if the XML defines that additional units will cost more
	if (GC.getDefineINT("ADVANCED_START_CITY_COST_INCREASE") > 0)
	{
		const int iNumCities = (bAdd ? getNumCities() : getNumCities() - 1);
		if (iNumCities > 0)
		{
			iCost *= 100 + GC.getDefineINT("ADVANCED_START_CITY_COST_INCREASE") * iNumCities;
			iCost /= 100;
		}
	}
	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Population
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartPopCost(bool bAdd, const CvCity* pCity) const
{
	if (0 == getNumCities())
	{
		return -1;
	}

	int iCost = (getGrowthThreshold(1) * GC.getDefineINT("ADVANCED_START_POPULATION_COST")) / 100;

	if (pCity)
	{
		if (pCity->getOwner() != getID())
		{
			return -1;
		}

		int iPopulation = pCity->getPopulation();

		// Need to have Population to remove it
		if (!bAdd)
		{
			--iPopulation;

			if (iPopulation < GC.getDefineINT("INITIAL_CITY_POPULATION") + GC.getEraInfo(GC.getGame().getStartEra()).getFreePopulation())
			{
				return -1;
			}
		}

		iCost = (getGrowthThreshold(iPopulation) * GC.getDefineINT("ADVANCED_START_POPULATION_COST")) / 100;

		// Increase cost if the XML defines that additional Pop will cost more
		if (0 != GC.getDefineINT("ADVANCED_START_POPULATION_COST_INCREASE"))
		{
			--iPopulation;

			if (iPopulation > 0)
			{
				iCost *= 100 + GC.getDefineINT("ADVANCED_START_POPULATION_COST_INCREASE") * iPopulation;
				iCost /= 100;
			}
		}
	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Culture
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartCultureCost(bool bAdd, const CvCity* pCity) const
{
	if (0 == getNumCities())
	{
		return -1;
	}

	int iCost = GC.getDefineINT("ADVANCED_START_CULTURE_COST");
	if (iCost < 0)
	{
		return -1;
	}

	if (pCity)
	{
		if (pCity->getOwner() != getID())
		{
			return -1;
		}

		if (bAdd)
		{
			iCost *= pCity->getCultureThreshold() - pCity->getCulture(getID());
		}
		else
		{
			// Need to have enough culture to remove it
			if (pCity->getCultureLevel() < 1)
			{
				return -1;
			}
			iCost *= pCity->getCulture(getID()) - GC.getGame().getCultureThreshold(static_cast<CultureLevelTypes>(pCity->getCultureLevel() - 1));
		}
		iCost /= 100;
	}
	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing a Building from a city
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartBuildingCost(BuildingTypes eBuilding, bool bAdd, const CvCity* pCity) const
{
	PROFILE_EXTRA_FUNC();
	if (0 == getNumCities() || isLimitedWonder(eBuilding))
	{
		return -1;
	}

	int iCost = getProductionNeeded(eBuilding);

	if (iCost < 0)
	{
		return -1;
	}

	if (!pCity && bAdd && algo::none_of(cities(), CvCity::fn::canConstruct(eBuilding)))
	{
		return -1;
	}

	if (pCity)
	{
		if (pCity->getOwner() != getID())
		{
			return -1;
		}
		iCost = getModifiedIntValue(iCost, pCity->getProductionModifier(eBuilding));

		if (!bAdd)
		{
			if (!pCity->hasBuilding(eBuilding))
			{
				return -1;
			}

			// Check other buildings in this city and make sure none of them require this one
			foreach_(const BuildingTypes eType, pCity->getHasBuildings())
			{
				const CvBuildingInfo& building = GC.getBuildingInfo(eType);
				for (int iI = building.getNumPrereqInCityBuildings() - 1; iI > -1; iI--)
				{
					if (eBuilding == building.getPrereqInCityBuilding(iI))
					{
						return -1;
					}
				}
			}
		}
		else if (!pCity->canConstruct(eBuilding, true, false, false))
		{
			return -1;
		}
	}
	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Route
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartRouteCost(RouteTypes eRoute, bool bAdd, const CvPlot* pPlot) const
{
	PROFILE_EXTRA_FUNC();
	if (0 == getNumCities())
	{
		return -1;
	}

	if (eRoute == NO_ROUTE)
	{
		return -1;
	}

	int iCost = GC.getRouteInfo(eRoute).getAdvancedStartCost();

	// This denotes cities may not be purchased through Advanced Start
	if (iCost < 0)
	{
		return -1;
	}

	iCost *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent();
	iCost /= 100;

	// No invalid plots!
	if (pPlot)
	{
		if (pPlot->isCity())
		{
			return -1;
		}

		if (bAdd)
		{
			if (pPlot->isImpassable(getTeam()) || pPlot->isWater()) // added getTeam()
			{
				return -1;
			}
			// Can't place twice
			if (pPlot->getRouteType() == eRoute)
			{
				return -1;
			}
		}
		else if (pPlot->getRouteType() != eRoute)
		{
			return -1; // Need Route to remove it
		}

		// Must be owned by me
		if (pPlot->getOwner() != getID())
		{
			return -1;
		}
	}

	// Tech requirement
	foreach_(const CvBuildInfo* info, GC.getBuildInfos())
	{
		if (info->getRoute() == eRoute)
		{
			if (!GET_TEAM(getTeam()).isHasTech(info->getTechPrereq()))
			{
				return -1;
			}
			if (info->getObsoleteTech() != NO_TECH
			&& GET_TEAM(getTeam()).isHasTech(info->getObsoleteTech()))
			{
				return -1;
			}
		}
	}
	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Improvement
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartImprovementCost(ImprovementTypes eImprovement, bool bAdd, const CvPlot* pPlot) const
{
	PROFILE_EXTRA_FUNC();
	if (eImprovement == NO_IMPROVEMENT || 0 == getNumCities())
	{
		return -1;
	}

	int iCost = GC.getImprovementInfo(eImprovement).getAdvancedStartCost();
	if (iCost < 0)
	{
		return -1;
	}
	iCost *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent();
	iCost /= 100;

	// Can this Improvement be on our plot?
	if (pPlot)
	{
		if (bAdd)
		{
			// Valid Plot
			if (!pPlot->canHaveImprovement(eImprovement, getTeam(), false))
			{
				return -1;
			}

			bool bValid = false;

			foreach_(const BuildTypes& eBuild, GC.getImprovementInfo(eImprovement).getBuildTypes())
			{
				if (canBuild(pPlot, eBuild))
				{
					bValid = true;

					const FeatureTypes eFeature = pPlot->getFeatureType();
					if (NO_FEATURE != eFeature && GC.getBuildInfo(eBuild).isFeatureRemove(eFeature))
					{
						iCost += GC.getFeatureInfo(eFeature).getAdvancedStartRemoveCost();
					}
					break;
				}
			}
			if (!bValid)
			{
				return -1;
			}

			// Can't place twice
			if (pPlot->getImprovementType() == eImprovement)
			{
				return -1;
			}
		}
		else if (pPlot->getImprovementType() != eImprovement)
		{
			return -1; // Need this improvement in order to remove it
		}

		// Must be owned by me
		if (pPlot->getOwner() != getID())
		{
			return -1;
		}
	}

	// Tech requirement
	foreach_(const BuildTypes& eBuild, GC.getImprovementInfo(eImprovement).getBuildTypes())
	{
		if (!GET_TEAM(getTeam()).isHasTech(GC.getBuildInfo(eBuild).getTechPrereq()))
		{
			return -1;
		}
		if (GC.getBuildInfo(eBuild).getObsoleteTech() != NO_TECH
		&& GET_TEAM(getTeam()).isHasTech(GC.getBuildInfo(eBuild).getObsoleteTech()))
		{
			return -1;
		}
	}
	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Tech
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartTechCost(TechTypes eTech, bool bAdd) const
{
	PROFILE_EXTRA_FUNC();
	if (eTech == NO_TECH || 0 == getNumCities() || GC.getTechInfo(eTech).isGlobal())
	{
		return -1;
	}

	int iCost = GET_TEAM(getTeam()).getResearchCost(eTech);
	if (iCost < 0)
	{
		return -1;
	}

	if (!bAdd)
	{
		if (!GET_TEAM(getTeam()).isHasTech(eTech))
		{
			return -1;
		}

		// Search through all techs to see if any of the currently owned ones requires this tech
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			const TechTypes eTechLoop = static_cast<TechTypes>(iI);

			if (GET_TEAM(getTeam()).isHasTech(eTechLoop))
			{
				if (algo::any_of_equal(GC.getTechInfo(eTechLoop).getPrereqOrTechs(), eTech)
				||  algo::any_of_equal(GC.getTechInfo(eTechLoop).getPrereqAndTechs(), eTech))
				{
					return -1;
				}
			}
		}

		// If player has placed anything on the map which uses this tech then you cannot remove it
		foreach_(const CvUnit* pLoopUnit, units())
		{
			if (pLoopUnit->getUnitInfo().getPrereqAndTech() == eTech
			|| algo::any_of_equal(pLoopUnit->getUnitInfo().getPrereqAndTechs(), eTech))
			{
				return -1;
			}
		}

		// Cities
		foreach_(const CvCity* cityX, cities())
		{
			foreach_(const BuildingTypes eType, cityX->getHasBuildings())
			{
				if (GC.getBuildingInfo(eType).getPrereqAndTech() == eTech
				|| algo::any_of_equal(GC.getBuildingInfo(eType).getPrereqAndTechs(), eTech))
				{
					return -1;
				}
			}
		}
	}
	else if (!canResearch(eTech))
	{
		return -1;
	}
	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Visibility
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartVisibilityCost(const CvPlot* pPlot) const
{
	PROFILE_EXTRA_FUNC();
	if (0 == getNumCities())
	{
		return -1;
	}

	int iCost = GC.getDefineINT("ADVANCED_START_VISIBILITY_COST");

	// This denotes Visibility may not be purchased through Advanced Start
	if (iCost == -1)
	{
		return -1;
	}
	// Valid Plot?
	if (pPlot && (pPlot->isRevealed(getTeam(), false) || !pPlot->isAdjacentRevealed(getTeam())))
	{
		return -1;
	}

	// Increase cost if the XML defines that additional units will cost more
	if (GC.getDefineINT("ADVANCED_START_VISIBILITY_COST_INCREASE") > 0)
	{
		int iNumVisiblePlots = 0;
		int iPlotLoop = 0;
		CvPlot* pPlot;

		for (iPlotLoop = 0; iPlotLoop < GC.getMap().numPlots(); iPlotLoop++)
		{
			pPlot = GC.getMap().plotByIndex(iPlotLoop);

			if (pPlot->isRevealed(getTeam(), false))
			{
				++iNumVisiblePlots;
			}
		}
		if (iNumVisiblePlots > 0)
		{
			iCost *= 100 + GC.getDefineINT("ADVANCED_START_VISIBILITY_COST_INCREASE") * iNumVisiblePlots;
			iCost /= 100;
		}
	}
	return iCost;
}

void CvPlayer::doWarnings()
{
	PROFILE_FUNC();

	CvCity* pNearestCity;
	CvPlot* pLoopPlot;
	wchar_t szBuffer[1024];
	int iMaxCount;
	int iI;

	//update enemy unit in your territory glow
	foreach_(CvUnit* pLoopUnit, units())
	{
		//update glow
		if ( !pLoopUnit->isUsingDummyEntities() )
		{
			gDLL->getEntityIFace()->updateEnemyGlow(pLoopUnit->getUnitEntity());
		}
	}

// BUG - Ignore Harmless Barbarians - start
	bool bCheckBarbarians = false;
	bool bCheckBarbariansInitialized = !isHumanPlayer();
// BUG - Ignore Harmless Barbarians - end

	//update enemy units close to your territory
	iMaxCount = range(((getNumCities() + 4) / 7), 2, 5);
	for (iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		if (iMaxCount == 0)
		{
			break;
		}

		pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot->isAdjacentPlayer(getID()))
		{
			if (!(pLoopPlot->isCity()))
			{
				if (pLoopPlot->isVisible(getTeam(), false))
				{
					CvUnit *pUnit = pLoopPlot->getVisibleEnemyDefender(getID());
					if (pUnit)
					{
						if (!pUnit->isAnimal() || pUnit->canAnimalIgnoresBorders())
						{
// BUG - Ignore Harmless Barbarians - start
							if (!bCheckBarbariansInitialized && GC.getGame().getElapsedGameTurns() > 0)
							{
								bCheckBarbarians = getBugOptionBOOL("Actions__IgnoreHarmlessBarbarians", true, "BUG_IGNORE_HARMLESS_BARBARIANS");
								bCheckBarbariansInitialized = true;
							}
							if (bCheckBarbarians && pUnit->isHominid() && pUnit->getDomainType() == DOMAIN_LAND)
							{
								CvArea* pArea = pUnit->area();
								if (pArea && pArea->isBorderObstacle(getTeam()))
								{
									// don't show warning for land-based barbarians when player has Great Wall
									continue;
								}
							}
// BUG - Ignore Harmless Barbarians - end

							pNearestCity = GC.getMap().findCity(pLoopPlot->getX(), pLoopPlot->getY(), getID(), NO_TEAM, !(pLoopPlot->isWater()));

							if (pNearestCity)
							{
								swprintf(szBuffer, gDLL->getText("TXT_KEY_MISC_ENEMY_TROOPS_SPOTTED", pNearestCity->getNameKey()).GetCString());
								AddDLLMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_ENEMY_TROOPS", MESSAGE_TYPE_INFO, pUnit->getButton(), GC.getCOLOR_RED(), pLoopPlot->getX(), pLoopPlot->getY(), true, true);

								iMaxCount--;
							}
						}
					}
				}
			}
		}
	}
}


void CvPlayer::verifyGoldCommercePercent()
{
	PROFILE_FUNC();

	while (getGold() + calculateGoldRate() < 0)
	{
		changeCommercePercent(COMMERCE_GOLD, GC.getDefineINT("COMMERCE_PERCENT_CHANGE_INCREMENTS"));

		if (getCommercePercent(COMMERCE_GOLD) == 100)
		{
			break;
		}
	}
}


void CvPlayer::processCivics(const CivicTypes eCivic, const int iChange, const bool bLimited)
{
	PROFILE_EXTRA_FUNC();
	FAssert(iChange == 1 || iChange == -1);

	if (isNPC()) return;

	const CvCivicInfo& kCivic = GC.getCivicInfo(eCivic);

	//Speed Optimizations
	if (bLimited)
	{
		if (kCivic.isAnyBuildingHappinessChange() || kCivic.isAnyBuildingHealthChange())
		{
			for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
			{
				changeExtraBuildingHappiness((BuildingTypes)iI, kCivic.getBuildingHappinessChanges(iI) * iChange, bLimited);
				changeExtraBuildingHealth((BuildingTypes)iI, kCivic.getBuildingHealthChanges(iI) * iChange, bLimited);
			}
		}
		if (kCivic.isAnyFeatureHappinessChange())
		{
			for (int iI = 0; iI < GC.getNumFeatureInfos(); iI++)
			{
				changeFeatureHappiness((FeatureTypes)iI, kCivic.getFeatureHappinessChanges(iI) * iChange, bLimited);
			}
		}
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			changeSpecialistValidCount((SpecialistTypes)iI, kCivic.isSpecialistValid(iI) * iChange, bLimited);
		}
	}
	else
	{
		changeGreatPeopleRateModifier(kCivic.getGreatPeopleRateModifier() * iChange);
		changeGreatGeneralRateModifier(kCivic.getGreatGeneralRateModifier() * iChange);
		changeDomesticGreatGeneralRateModifier(kCivic.getDomesticGreatGeneralRateModifier() * iChange);
		changeStateReligionGreatPeopleRateModifier(kCivic.getStateReligionGreatPeopleRateModifier() * iChange);

		changeDistanceMaintenanceModifier(kCivic.getDistanceMaintenanceModifier() * iChange);
		changeNumCitiesMaintenanceModifier(kCivic.getNumCitiesMaintenanceModifier() * iChange);
		changeHomeAreaMaintenanceModifier(kCivic.getHomeAreaMaintenanceModifier() * iChange);
		changeOtherAreaMaintenanceModifier(kCivic.getOtherAreaMaintenanceModifier() * iChange);

		changeFreeExperience(kCivic.getFreeExperience() * iChange);
		changeWorkerSpeedModifier(kCivic.getWorkerSpeedModifier() * iChange);
		changeImprovementUpgradeRateModifier(kCivic.getImprovementUpgradeRateModifier() * iChange);
		changeMilitaryProductionModifier(kCivic.getMilitaryProductionModifier() * iChange);
		changeBaseFreeUnitUpkeepCivilian(kCivic.getFreeUnitUpkeepCivilian() * iChange);
		changeBaseFreeUnitUpkeepMilitary(kCivic.getFreeUnitUpkeepMilitary() * iChange);
		changeFreeUnitUpkeepCivilianPopPercent(kCivic.getFreeUnitUpkeepCivilianPopPercent() * iChange);
		changeFreeUnitUpkeepMilitaryPopPercent(kCivic.getFreeUnitUpkeepMilitaryPopPercent() * iChange);
		changeCivilianUnitUpkeepMod(kCivic.getCivilianUnitUpkeepMod() * iChange);
		changeMilitaryUnitUpkeepMod(kCivic.getMilitaryUnitUpkeepMod() * iChange);
		changeMaxConscript(getWorldSizeMaxConscript(eCivic) * iChange);
		changeFreeSpecialist(kCivic.getFreeSpecialist() * iChange);
		changeTradeRoutes(kCivic.getTradeRoutes() * iChange);
		changeStateReligionUnitProductionModifier(kCivic.getStateReligionUnitProductionModifier() * iChange);
		changeStateReligionBuildingProductionModifier(kCivic.getStateReligionBuildingProductionModifier() * iChange);
		changeStateReligionFreeExperience(kCivic.getStateReligionFreeExperience() * iChange);
		changeExpInBorderModifier(kCivic.getExpInBorderModifier() * iChange);

		changeUpgradeAnywhere((kCivic.isUpgradeAnywhere())? iChange : 0);
		changeRevIdxHolyCityGood(kCivic.getRevIdxHolyCityGood() * iChange);
		changeRevIdxHolyCityBad(kCivic.getRevIdxHolyCityBad() * iChange);
		changeRevIdxNationalityMod(kCivic.getRevIdxNationalityMod() * static_cast<float>(iChange));
		changeRevIdxBadReligionMod(kCivic.getRevIdxBadReligionMod() * static_cast<float>(iChange));
		changeRevIdxGoodReligionMod(kCivic.getRevIdxGoodReligionMod() * static_cast<float>(iChange));
		changeInquisitionCount((kCivic.isAllowInquisitions())? iChange : 0);
		changeInquisitionCount((kCivic.isDisallowInquisitions())? -iChange : 0);

		for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			changeYieldRateModifier(((YieldTypes)iI), (kCivic.getYieldModifier(iI) * iChange));
			changeCapitalYieldRateModifier(((YieldTypes)iI), (kCivic.getCapitalYieldModifier(iI) * iChange));
			changeTradeYieldModifier(((YieldTypes)iI), (kCivic.getTradeYieldModifier(iI) * iChange));
			changeLandmarkYield((YieldTypes)iI, kCivic.getLandmarkYieldChanges(iI) * iChange);
		}

		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			changeCommerceRateModifier(((CommerceTypes)iI), (kCivic.getCommerceModifier(iI) * iChange));
			changeCapitalCommerceRateModifier(((CommerceTypes)iI), (kCivic.getCapitalCommerceModifier(iI) * iChange));
			changeSpecialistExtraCommerce(((CommerceTypes)iI), (kCivic.getSpecialistExtraCommerce(iI) * iChange));
		}

		foreach_(const BuildingModifier2& modifier, kCivic.getBuildingProductionModifiers())
		{
			changeBuildingProductionModifier(modifier.first, modifier.second * iChange);
		}

		for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			changeExtraBuildingHappiness((BuildingTypes)iI, (kCivic.getBuildingHappinessChanges(iI) * iChange));
			changeExtraBuildingHealth((BuildingTypes)iI, (kCivic.getBuildingHealthChanges(iI) * iChange));
		}

		for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			changeUnitProductionModifier(((UnitTypes)iI), kCivic.getUnitProductionModifier(iI) * iChange);
		}

		for (int iI = 0; iI < GC.getNumFeatureInfos(); iI++)
		{
			changeFeatureHappiness(((FeatureTypes)iI), (kCivic.getFeatureHappinessChanges(iI) * iChange));
		}

		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			changeSpecialistValidCount((SpecialistTypes)iI, kCivic.isSpecialistValid(iI) * iChange);
			changeFreeSpecialistCount((SpecialistTypes)iI, (kCivic.getFreeSpecialistCount(iI) * iChange));
		}

		for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				changeImprovementYieldChange(((ImprovementTypes)iI), ((YieldTypes)iJ), (kCivic.getImprovementYieldChanges(iI, iJ) * iChange));
			}
		}

		changeForeignTradeRouteModifier(kCivic.getForeignTradeRouteModifier() * iChange);
		changeReligionSpreadRate(kCivic.getReligionSpreadRate() * iChange);
		changeCorporationSpreadModifier(kCivic.getCorporationSpreadRate() * iChange);
		changeDistantUnitSupportCostModifier(kCivic.getDistantUnitSupportCostModifier() * iChange);
		changeExtraCityDefense(kCivic.getExtraCityDefense() * iChange);
		changeExtraFreedomFighters(kCivic.getFreedomFighterChange() * iChange);
		changeEnslavementChance(kCivic.getEnslavementChance() * iChange);
		changeCivicInflation(kCivic.getInflationModifier() * iChange);
		changeHurryCostModifier(kCivic.getHurryCostModifier() * iChange);
		changeHurryInflationModifier(kCivic.getHurryInflationModifier() * iChange);
		changeLandmarkHappiness(kCivic.getLandmarkHappiness() * iChange);
		changeNoLandmarkAngerCount(kCivic.isNoLandmarkAnger() ? iChange : 0);

		changeFixedBordersCount(kCivic.IsFixedBorders() ? iChange : 0);
		changeFreedomFighterCount(kCivic.isFreedomFighter() ? iChange : 0);
		AI_makeAssignWorkDirty();
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				changeSpecialistYieldPercentChanges((SpecialistTypes)iI, (YieldTypes)iJ, kCivic.getSpecialistYieldPercentChanges(iI, iJ) * iChange);
			}
			for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				changeSpecialistCommercePercentChanges((SpecialistTypes)iI, (CommerceTypes)iJ, kCivic.getSpecialistCommercePercentChanges(iI, iJ) * iChange);
			}
		}

		for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				changeBuildingCommerceModifier((BuildingTypes)iI, (CommerceTypes)iJ, kCivic.getBuildingCommerceModifier(iI, iJ) * iChange);
			}
		}

		for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				changeBuildingCommerceChange((BuildingTypes)iI, (CommerceTypes)iJ, kCivic.getBuildingCommerceChange(iI, iJ) * iChange);
			}
		}

		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			changeBonusMintedPercent((BonusTypes)iI, kCivic.getBonusMintedPercent(iI) * iChange);
			for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				changeBonusCommerceModifier((BonusTypes)iI, (CommerceTypes)iJ, kCivic.getBonusCommerceModifier(iI, iJ) * iChange);
			}
		}
		if (iChange == -1)
		{
			for (int iI = 0; iI < GC.getNumVoteSourceInfos(); ++iI)
			{
				if (GC.getVoteSourceInfo((VoteSourceTypes)iI).getCivic() == eCivic)
				{
					if (GC.getGame().getSecretaryGeneral((VoteSourceTypes)iI) == getTeam())
					{
						GC.getGame().clearSecretaryGeneral((VoteSourceTypes)iI);
					}
				}
			}

		}
		for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
		{
			changeUnitCombatProductionModifier(((UnitCombatTypes)iI), (kCivic.getUnitCombatProductionModifier(iI) * iChange));
		}

		for (int iI = 0; iI < GC.getNumTerrainInfos(); iI++)
		{
			for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				changeTerrainYieldChange(((TerrainTypes)iI), ((YieldTypes)iJ), (kCivic.getTerrainYieldChanges(iI, iJ) * iChange));
			}
		}
		changeExtraNationalCaptureProbabilityModifier(kCivic.getNationalCaptureProbabilityModifier() * iChange);
		changeExtraNationalCaptureResistanceModifier(kCivic.getNationalCaptureResistanceModifier() * iChange);
	}

	changeCivicHappiness(kCivic.getCivicHappiness() * iChange);
	changeLargestCityHappiness(kCivic.getLargestCityHappiness() * iChange, bLimited);
	changeNoCapitalUnhappiness(kCivic.isNoCapitalUnhappiness() * iChange);
	changeTaxRateUnhappiness(kCivic.getTaxRateUnhappiness() * iChange);
	changeHappyPerMilitaryUnit(kCivic.getHappyPerMilitaryUnit() * iChange,  bLimited);

	changeCivicHealth(kCivic.getExtraHealth() * iChange,  bLimited);
	changeNoUnhealthyPopulationCount(kCivic.isNoUnhealthyPopulation() * iChange, bLimited);
	changeBuildingOnlyHealthyCount(kCivic.isBuildingOnlyHealthy() * iChange, bLimited);

	changePopulationgrowthratepercentage(kCivic.getPopulationgrowthratepercentage(), iChange == 1);
	changeCorporationMaintenanceModifier(kCivic.getCorporationMaintenanceModifier() * iChange, bLimited);
	changeMilitaryFoodProductionCount(kCivic.isMilitaryFoodProduction() * iChange, bLimited);

	changeNoForeignTradeCount(kCivic.isNoForeignTrade() * iChange, bLimited);
	changeNoCorporationsCount(kCivic.isNoCorporations() * iChange, bLimited);
	changeNoForeignCorporationsCount(kCivic.isNoForeignCorporations() * iChange, bLimited);

	changeStateReligionCount(kCivic.isStateReligion() * iChange, bLimited);
	changeNoNonStateReligionSpreadCount(kCivic.isNoNonStateReligionSpread() * iChange);
	changeStateReligionHappiness(kCivic.getStateReligionHappiness() * iChange, bLimited);
	changeNonStateReligionHappiness(kCivic.getNonStateReligionHappiness() * iChange, bLimited);

	changeRevIdxLocal(kCivic.getRevIdxLocal() * iChange);
	changeRevIdxNational(kCivic.getRevIdxNational() * iChange);
	changeRevIdxDistanceModifier(kCivic.getRevIdxDistanceModifier() * iChange);

	changeCityLimit(kCivic.getCityLimit(getID()) * iChange);
	changeCityOverLimitUnhappy(kCivic.getCityOverLimitUnhappy() * iChange);
	changeForeignUnhappyPercent(kCivic.getForeignerUnhappyPercent() * iChange);

	changeWarWearinessModifier(kCivic.getWarWearinessModifier() * iChange, bLimited);

	//TB Civics Tags
	changeAllReligionsActiveCount((kCivic.isAllReligionsActive())? iChange : 0);
	changeAllReligionsActiveCount((kCivic.isBansNonStateReligions())? -iChange : 0);

	for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
	{
		changeHurryCount((HurryTypes)iI, kCivic.isHurry(iI) * iChange);
	}
	for (int iI = 0; iI < GC.getNumSpecialBuildingInfos(); iI++)
	{
		changeSpecialBuildingNotRequiredCount((SpecialBuildingTypes)iI, kCivic.isSpecialBuildingNotRequired(iI) * iChange);
	}
}

void CvPlayer::showMissedMessages()
{
	PROFILE_EXTRA_FUNC();
	foreach_(CvTalkingHeadMessage& msg, m_listGameMessages)
	{
		if (!msg.getShown())
		{
			msg.setShown(true);
			gDLL->getInterfaceIFace()->showMessage(msg);
		}
	}
}

bool CvPlayer::isPbemNewTurn() const
{
	return m_bPbemNewTurn;
}

void CvPlayer::setPbemNewTurn(bool bNew)
{
	m_bPbemNewTurn = bNew;
}



//
// read object from a stream
// used during load
//
void CvPlayer::read(FDataStreamBase* pStream)
{
	PROFILE_EXTRA_FUNC();

	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	uint uiFlag=0;
	WRAPPER_READ(wrapper, "CvPlayer", &uiFlag);	// flags for expansion

	if ((uiFlag& PLAYER_UI_FLAG_OMITTED) == 0)
	{
		// Init data before load
		reset();

		int m_iStartingX, m_iStartingY;
		WRAPPER_READ(wrapper, "CvPlayer", &m_iStartingX);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iStartingY);
		m_startingCoords[MAP_EARTH] = XYCoords(m_iStartingX, m_iStartingY);

		WRAPPER_READ(wrapper, "CvPlayer", &m_iTotalPopulation);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iTotalLand);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iTotalLandScored);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iGoldPerTurn);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iAdvancedStartPoints);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iGoldenAgeTurns);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNumUnitGoldenAges);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iStrikeTurns);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iAnarchyTurns);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iMaxAnarchyTurns);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iAnarchyModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iGoldenAgeModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iGlobalHurryModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iGreatPeopleCreated);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iGreatGeneralsCreated);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iGreatPeopleThresholdModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iGreatGeneralsThresholdModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iGreatPeopleRateModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iGreatGeneralRateModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iDomesticGreatGeneralRateModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iStateReligionGreatPeopleRateModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iMaxGlobalBuildingProductionModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iMaxTeamBuildingProductionModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iMaxPlayerBuildingProductionModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iFreeExperience);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iFeatureProductionModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iWorkerSpeedModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iImprovementUpgradeRateModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iMilitaryProductionModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iSpaceProductionModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCityDefenseModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNonStateReligionCommerceCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iUpgradeAnywhereCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iRevIdxLocal);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iRevIdxNational);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iRevIdxDistanceModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iRevIdxHolyCityGood);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iRevIdxHolyCityBad);
		WRAPPER_READ(wrapper, "CvPlayer", &m_fRevIdxNationalityMod);
		WRAPPER_READ(wrapper, "CvPlayer", &m_fRevIdxBadReligionMod);
		WRAPPER_READ(wrapper, "CvPlayer", &m_fRevIdxGoodReligionMod);
		WRAPPER_READ(wrapper, "CvPlayer", &m_bInquisitionConditions);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iUnitUpgradePriceModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNumNukeUnits);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNumOutsideUnits);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNumMilitaryUnits);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iHappyPerMilitaryUnit);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iMilitaryFoodProductionCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iConscriptCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iMaxConscript);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iHighestUnitLevel);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iOverflowResearch);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNoUnhealthyPopulationCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iExpInBorderModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iBuildingOnlyHealthyCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iMaintenanceModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCoastalDistanceMaintenanceModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iConnectedCityMaintenanceModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iDistanceMaintenanceModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNumCitiesMaintenanceModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCorporationMaintenanceModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iTotalMaintenance);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iUpkeepModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iLevelExperienceModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iExtraHealth);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCivicHealth);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iBuildingGoodHealth);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iBuildingBadHealth);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iExtraHappiness);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iExtraHappinessUnattributed);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iBuildingHappiness);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iLargestCityHappiness);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iWarWearinessPercentAnger);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iWarWearinessModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iFreeSpecialist);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNoForeignTradeCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNoCorporationsCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNoForeignCorporationsCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCoastalTradeRoutes);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iTradeRoutes);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iRevolutionTimer);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iConversionTimer);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iStateReligionCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNoNonStateReligionSpreadCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iStateReligionHappiness);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNonStateReligionHappiness);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iStateReligionUnitProductionModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iStateReligionBuildingProductionModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iStateReligionFreeExperience);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCapitalCityID);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCitiesLost);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iWinsVsBarbs);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iAssets);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iPower);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iTechPower);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iUnitPower);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iPopulationScore);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iLandScore);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iWondersScore);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iTechScore);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCombatExperience);
		WRAPPER_READ(wrapper, "CvPlayer", &m_bAlive);
		WRAPPER_READ(wrapper, "CvPlayer", &m_bEverAlive);
		WRAPPER_READ(wrapper, "CvPlayer", &m_bTurnActive);
		WRAPPER_READ(wrapper, "CvPlayer", &m_bAutoMoves);
		WRAPPER_READ(wrapper, "CvPlayer", &m_bEndTurn);
		WRAPPER_READ(wrapper, "CvPlayer", &m_bPbemNewTurn);
		WRAPPER_READ(wrapper, "CvPlayer", &m_bExtendedGame);
		WRAPPER_READ(wrapper, "CvPlayer", &m_bFoundedFirstCity);
		WRAPPER_READ(wrapper, "CvPlayer", &m_bStrike);
		WRAPPER_READ(wrapper, "CvPlayer", &m_bNukesValid);
		WRAPPER_READ(wrapper, "CvPlayer", &m_bDisableHuman); // AI_AUTO_PLAY - 07/09/08 - jdog5000
		WRAPPER_READ(wrapper, "CvPlayer", &m_iStabilityIndex);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iStabilityIndexAverage);
		WRAPPER_READ(wrapper, "CvPlayer", &m_bRebel);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iMotherPlayer);
		WRAPPER_READ(wrapper, "CvPlayer", &m_bDoNotBotherStatus);

		// Used for DynamicCivNames
		WRAPPER_READ_STRING(wrapper, "CvPlayer", m_szName);
		WRAPPER_READ_STRING(wrapper, "CvPlayer", m_szCivDesc);
		WRAPPER_READ_STRING(wrapper, "CvPlayer", m_szCivShort);
		WRAPPER_READ_STRING(wrapper, "CvPlayer", m_szCivAdj);

		WRAPPER_READ(wrapper, "CvPlayer", (int*)&m_eID);

		m_contractBroker.init(m_eID);
		m_contractBroker.cleanup();

		// @SAVEBREAK EVALUATE
		WRAPPER_READ(wrapper, "CvPlayer", (int*)&m_ePersonalityType);
		//	The above should be a class enum but to avoid screwing up existing saves doing a manual translation
		m_ePersonalityType = (LeaderHeadTypes)wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_LEADERHEADS, m_ePersonalityType, true);
		if ( m_ePersonalityType == -1 )
		{
			//	Just accept the default for the leaderhead if the one that was being used has been removed
			m_ePersonalityType = getLeaderType();
		}
		// SAVEBREAK@

		WRAPPER_READ(wrapper, "CvPlayer", (int*)&m_eCurrentEra);
		WRAPPER_READ_CLASS_ENUM(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_RELIGIONS, (int*)&m_eLastStateReligion);
		WRAPPER_READ(wrapper, "CvPlayer", (int*)&m_eParent);
		updateTeamType(); //m_eTeamType not saved
		updateHuman();

		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_aiSeaPlotYield);
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_aiYieldRateModifier);
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_aiCapitalYieldRateModifier);
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_aiExtraYieldThreshold);
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_aiTradeYieldModifier);
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiFreeCityCommerce);
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiCommercePercent);
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiCommerceRate);
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiCommerceRateModifier);
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiCapitalCommerceRateModifier);
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiStateReligionBuildingCommerce);
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiSpecialistExtraCommerce);
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiCommerceFlexibleCount);
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", MAX_PLAYERS, m_aiGoldPerTurnByPlayer);
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", MAX_TEAMS, m_aiEspionageSpendingWeightAgainstTeam);

		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_FEAT_TYPES, m_abFeatAccomplished);
		WRAPPER_READ_ARRAY_ALLOW_TRUNCATE(wrapper, "CvPlayer", NUM_PLAYEROPTION_TYPES, m_abOptions);

		WRAPPER_READ_STRING(wrapper, "CvPlayer", m_szScriptData);

		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_IMPROVEMENTS, GC.getNumImprovementInfos(), m_paiImprovementCount);
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_FEATURES, GC.getNumFeatureInfos(), m_paiFeatureHappiness);
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiBuildingCount);

		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_HURRIES, GC.getNumHurryInfos(), m_paiHurryCount);
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_SPECIAL_BUILDINGS, GC.getNumSpecialBuildingInfos(), m_paiSpecialBuildingNotRequiredCount);
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_CIVIC_OPTIONS, GC.getNumCivicOptionInfos(), m_paiHasCivicOptionCount);
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_CIVIC_OPTIONS, GC.getNumCivicOptionInfos(), m_paiNoCivicUpkeepCount);
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_RELIGIONS, GC.getNumReligionInfos(), m_paiHasReligionCount);
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_CORPORATIONS, GC.getNumCorporationInfos(), m_paiHasCorporationCount);
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_UPKEEPS, GC.getNumUpkeepInfos(), m_paiUpkeepCount);
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiSpecialistValidCount);

		WRAPPER_READ(wrapper, "CvPlayer", &m_iForeignTradeRouteModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iTaxRateUnhappiness);

		WRAPPER_READ(wrapper, "CvPlayer", &m_fPopulationgrowthratepercentageLog);

		WRAPPER_READ(wrapper, "CvPlayer", &m_iReligionSpreadRate);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCivicHappiness);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iDistantUnitSupportCostModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iExtraCityDefense);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iEnslavementChance);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNoCapitalUnhappiness);

		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_aiLandmarkYield);

		WRAPPER_READ(wrapper, "CvPlayer", &m_iWorldHealth);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iWorldHappiness);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iProjectHealth);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iProjectHappiness);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iForceAllTradeRoutes);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCivilizationHealth);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iBuildingInflation);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iProjectInflation);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iTechInflation);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCivicInflation);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iHurryCostModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iHurryInflationModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iHurryCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNoLandmarkAngerCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iLandmarkHappiness);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCorporationSpreadModifier);
		// @SAVEBREAK - delete
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCorporateTaxIncome);
		// !SAVEBREAK
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCityLimit);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCityOverLimitUnhappy);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iForeignUnhappyPercent);

		//	Subdue and construct-by-unit stats
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNumAnimalsSubdued);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNumAnarchyTurns);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNumCivicSwitches);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNumCivicsSwitched);
		int iNumBuildingTypes = 0;
		WRAPPER_READ(wrapper, "CvPlayer", &iNumBuildingTypes);
		while(iNumBuildingTypes-- > 0)
		{
			int	eBuilding;
			int iConstructionCount;

			WRAPPER_READ(wrapper, "CvPlayer", &eBuilding);
			WRAPPER_READ(wrapper, "CvPlayer", &iConstructionCount);
			eBuilding = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDINGS, eBuilding, true);

			// Toffer - Remove @SAVEBREAK as it is only used for logging purposes, not particularly interesting logging.
			if (eBuilding > -1)
			{
				m_unitConstructionCounts.insert(std::make_pair((BuildingTypes)eBuilding, iConstructionCount));
			}
		}

		WRAPPER_READ_CLASS_ARRAY_ALLOW_MISSING(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_BUILDS, GC.getNumBuildInfos(), m_pabAutomatedCanBuild);

		FAssertMsg(0 < GC.getNumBonusInfos(), "GC.getNumBonusInfos() is not greater than zero but it is expected to be in CvPlayer::read");
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiResourceConsumption);
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiFreeSpecialistCount);

		WRAPPER_READ(wrapper, "CvPlayer", (int*)&m_ePledgedVote);
		WRAPPER_READ(wrapper, "CvPlayer", (int*)&m_eSecretaryGeneralVote);

		civcSwitchInstance switchInstance;

		do
		{
			switchInstance.iTurn = -1;
			WRAPPER_READ(wrapper, "CvPlayer", &switchInstance.iTurn);

			if ( switchInstance.iTurn != -1 )
			{
				WRAPPER_READ_CLASS_ENUM(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_CIVICS, &switchInstance.eFromCivic);
				WRAPPER_READ_CLASS_ENUM(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_CIVICS, &switchInstance.eToCivic);
				WRAPPER_READ(wrapper, "CvPlayer", &switchInstance.bNoAnarchy);

				m_civicSwitchHistory.push_back(switchInstance);
			}
		} while(switchInstance.iTurn != -1);

		m_Properties.readWrapper(pStream);

		for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_TERRAINS); ++i)
		{
			int	iI = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_TERRAINS, i, true);

			if ( iI != -1 )
			{
				WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_ppaaiTerrainYieldChange[iI]);
			}
			else
			{
				//	Consume the values
				WRAPPER_SKIP_ELEMENT(wrapper, "CvPlayer", m_ppaaiTerrainYieldChange[iI], SAVE_VALUE_TYPE_INT_ARRAY);
			}
		}
		for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_BUILDINGS); ++i)
		{
			int	iI = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDINGS, i, true);

			if ( iI != -1 )
			{
				WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_ppiBuildingCommerceModifier[iI]);
			}
			else
			{
				//	Consume the values
				WRAPPER_SKIP_ELEMENT(wrapper, "CvPlayer", m_ppiBuildingCommerceModifier[iI], SAVE_VALUE_TYPE_INT_ARRAY);
			}
		}
		for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_BUILDINGS); ++i)
		{
			int	iI = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDINGS, i, true);

			if ( iI != -1 )
			{
				WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_ppiBuildingCommerceChange[iI]);
			}
			else
			{
				//	Consume the values
				WRAPPER_SKIP_ELEMENT(wrapper, "CvPlayer", m_ppiBuildingCommerceChange[iI], SAVE_VALUE_TYPE_INT_ARRAY);
			}
		}
		for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_BONUSES); ++i)
		{
			int	iI = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BONUSES, i, true);

			if ( iI != -1 )
			{
				WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_ppiBonusCommerceModifier[iI]);
			}
			else
			{
				//	Consume the values
				WRAPPER_SKIP_ELEMENT(wrapper, "CvPlayer", m_ppiBonusCommerceModifier[iI], SAVE_VALUE_TYPE_INT_ARRAY);
			}
		}
		for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_SPECIALISTS); ++i)
		{
			int	iI = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_SPECIALISTS, i, true);

			if ( iI != -1 )
			{
				WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_ppiSpecialistYieldPercentChanges[iI]);
				WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_ppiSpecialistCommercePercentChanges[iI]);
			}
			else
			{
				//	Consume the values
				WRAPPER_SKIP_ELEMENT(wrapper, "CvPlayer", m_ppiSpecialistYieldPercentChanges[iI], SAVE_VALUE_TYPE_INT_ARRAY);
				WRAPPER_SKIP_ELEMENT(wrapper, "CvPlayer", m_ppiSpecialistCommercePercentChanges[iI], SAVE_VALUE_TYPE_INT_ARRAY);
			}
		}

		FAssertMsg((0 < GC.getNumTechInfos()), "GC.getNumTechInfos() is not greater than zero but it is expected to be in CvPlayer::read");
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_pabResearchingTech);

		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_VOTE_SOURCES, GC.getNumVoteSourceInfos(), m_pabLoyalMember);

		for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_CIVIC_OPTIONS); ++i)
		{
			int	iI = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_CIVIC_OPTIONS, i, true);

			if ( iI != -1 )
			{
				WRAPPER_READ_CLASS_ENUM_ALLOW_MISSING(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_CIVICS, (int*)&m_paeCivics[iI]);
			}
			else
			{
				//	Consume the values
				WRAPPER_SKIP_ELEMENT(wrapper, "CvPlayer", m_paeCivics[iI], SAVE_VALUE_TYPE_CLASS_ENUM);
			}
		}

		//	If any new civics were added they need to be initialized sensibly for extant civilizations
		if ( wrapper.isUsingTaggedFormat() && getCivilizationType() != NO_CIVILIZATION)
		{
			m_iCityLimit = 0;	//	Will be set correctly below for the civics in force
			m_iCityOverLimitUnhappy = 0;

			for(int i = 0; i < GC.getNumCivicOptionInfos(); i++)
			{
				// AIAndy: Check for no civic set or a civic of the wrong category set
				if ( (m_paeCivics[i] == NO_CIVIC) || (GC.getCivicInfo(m_paeCivics[i]).getCivicOptionType() != (CivicOptionTypes)i ) )
				{
					m_paeCivics[i] = (CivicTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationInitialCivics(i);
				}

				//	Also need to fix iCityLimits if its definition got changed in an active civic.  The way we do this
				//	is slightly kludgey, and is based on the fact that the iCityLimit system is anyway somewhat broken in
				//	that it processes as a cumulative number, which makes no sense if limits are defined on multiple
				//	civicOptions.  As such we ASSUME only one civivOption can be defining a limit and therefore it has
				//	a single source, so we can directly set its value from the currently in-use civic that has a limit (if any)
				if ( GC.getCivicInfo(m_paeCivics[i]).getCityLimit(getID()) != 0 )
				{
					m_iCityLimit = GC.getCivicInfo(m_paeCivics[i]).getCityLimit(getID());
					m_iCityOverLimitUnhappy = GC.getCivicInfo(m_paeCivics[i]).getCityOverLimitUnhappy();
				}
			}
		}

		for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_SPECIALISTS); ++i)
		{
			int	iI = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_SPECIALISTS, i, true);

			if ( iI != -1 )
			{
				WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_ppaaiSpecialistExtraYield[iI]);
			}
			else
			{
				//	Consume the values
				WRAPPER_SKIP_ELEMENT(wrapper, "CvPlayer", m_ppaaiSpecialistExtraYield[iI], SAVE_VALUE_TYPE_INT_ARRAY);
			}
		}

		for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_IMPROVEMENTS); ++i)
		{
			int	iI = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_IMPROVEMENTS, i, true);

			if ( iI != -1 )
			{
				WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_ppaaiImprovementYieldChange[iI]);
			}
			else
			{
				//	Consume the values
				WRAPPER_SKIP_ELEMENT(wrapper, "CvPlayer", m_ppaaiImprovementYieldChange[iI], SAVE_VALUE_TYPE_INT_ARRAY);
			}
		}
		m_researchQueue.Read(pStream);

		//	The research queue itself is not a streamable type so is serialized in raw
		//	binary image, which means we need to do some explicit translation on load
		//	if we are using the tagged format
		if (wrapper.isUsingTaggedFormat())
		{
			CLLNode<TechTypes>* pNode = headResearchQueueNode();
			while (pNode)
			{
				const bool bDeleteNode =
				(
					pNode->m_data != NO_TECH
					?
					(pNode->m_data = (TechTypes)wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_TECHS,pNode->m_data, true)) == -1
					:
					false
				);
				pNode = bDeleteNode ? m_researchQueue.deleteNode(pNode) : nextResearchQueueNode(pNode);
			}
		}

		{
			m_cityNames.clear();
			CvWString szBuffer;
			uint iSize;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numCities");
			for (uint i = 0; i < iSize; i++)
			{
				WRAPPER_READ_STRING_DECORATED(wrapper, "CvPlayer", szBuffer, "cityName");
				m_cityNames.insertAtEnd(szBuffer);
			}
		}
		WRAPPER_READ(wrapper, "CvPlayer", &m_iMaxTradeRoutesAdjustment);

		int iSize;
		WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "NUM_MAPS");

		for (int i = 0; i < iSize; i++)
		{
			m_groupCycles[i]->Read(pStream);
			ReadStreamableFFreeListTrashArray(*m_plotGroups[i], pStream);
			ReadStreamableFFreeListTrashArray(*m_cities[i], pStream);
			ReadStreamableFFreeListTrashArray(*m_units[i], pStream);
			ReadStreamableFFreeListTrashArray(*m_selectionGroups[i], pStream);
		}
		//Must be loaded AFTER units.
		m_commanders.clear();
		m_commodores.clear();
		foreach_(CvUnit* unitX, units())
		{
			if (unitX->isCommander()) //search for GGs among loaded units and add them to m_commanders array
			{
				m_commanders.push_back(unitX);
			}

			if (unitX->isCommodore()) //search for GAs among loaded units and add them to m_commodores array
            {
            	m_commodores.push_back(unitX);
            }

			if (unitX->isZoneOfControl())
			{
				GC.getGame().toggleAnyoneHasUnitZoneOfControl();
			}
		}
		ReadStreamableFFreeListTrashArray(m_eventsTriggered, pStream);

		std::map<CvUnit*,bool> unitsPresent;

		std::vector<CvUnit*> plotlessUnits;

		foreach_(CvSelectionGroup* groupX, groups())
		{
#ifdef _DEBUG
			OutputDebugString(CvString::format("Check group %d:\n", groupX->getID()).c_str());
#endif
			unitsPresent.clear();
			foreach_(CvUnit* pUnit, groupX->units())
			{
				CvSelectionGroup* putativeGroup = pUnit->getGroup();
#ifdef _DEBUG
				OutputDebugString(CvString::format("\tUnit %d\n", pUnit->getID()).c_str());
#endif
				if (putativeGroup != groupX)
				{
					FErrorMsg("Corrupt group detected on load");
					OutputDebugString(CvString::format("\t\tunit claims to be in group %d\n", putativeGroup->getID()).c_str());

					// Try to fix it
					groupX->removeUnit(pUnit);

					if (!putativeGroup->containsUnit(pUnit) && !putativeGroup->addUnit(pUnit,true))
					{
						pUnit->joinGroup(NULL);
					}
				}
				else if (unitsPresent.find(pUnit) != unitsPresent.end())
				{
					FErrorMsg("Duplicate unit in group detected on load");
					groupX->removeUnit(pUnit); // Duplicate
				}
				else unitsPresent.insert(std::make_pair(pUnit,true));

				if (!pUnit->plot()) plotlessUnits.push_back(pUnit);
			}

			if (!isNPC())
			{
				groupX->validateLocations(true);
				//	Koshling - added death processing on load to clean up units that should have been
				//	removed, but due to bugs are 'ghosting' in the save
				groupX->doDelayedDeath();
			}
		}

#ifdef _DEBUG
		if (!isNPC())
		{
			foreach_(CvSelectionGroup* groupX, groups())
			{
				groupX->validateLocations(false);
			}
		}
#endif

		{
			CvMessageQueue::_Alloc::size_type iSize;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numListGameMessages");
			for (CvMessageQueue::_Alloc::size_type i = 0; i < iSize; i++)
			{
				CvTalkingHeadMessage message;
				message.read(*pStream);
				m_listGameMessages.push_back(message);
			}
		}

		{
			clearPopups();
			CvPopupQueue::_Alloc::size_type iSize;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numPopups");
			for (CvPopupQueue::_Alloc::size_type i = 0; i < iSize; i++)
			{
				CvPopupInfo* pInfo = new CvPopupInfo();
				if (pInfo)
				{
					pInfo->read(*pStream);
					m_listPopups.push_back(pInfo);

					if ( pInfo->getButtonPopupType() == BUTTONPOPUP_CHOOSETECH && pInfo->getData1() > 0 )
					{
						startChoosingFreeTech();
					}
				}
			}
		}

		{
			clearDiplomacy();
			CvDiploQueue::_Alloc::size_type iSize;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numDiploParams");
			for (CvDiploQueue::_Alloc::size_type i = 0; i < iSize; i++)
			{
				CvDiploParameters* pDiplo = new CvDiploParameters(NO_PLAYER);
				if (pDiplo)
				{
					pDiplo->read(*pStream);
					m_listDiplomacy.push_back(pDiplo);
				}
			}
		}

		{
			uint iSize;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numScoreHistEntries");
			for (uint i = 0; i < iSize; i++)
			{
				int iTurn;
				double fScore = 0;
				WRAPPER_READ(wrapper, "CvPlayer", &iTurn);
				WRAPPER_READ(wrapper, "CvPlayer", &fScore);
				m_mapScoreHistory[iTurn] = static_cast<int64_t>(fScore);
			}
		}

		{
			m_mapEconomyHistory.clear();
			uint iSize;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numEconHistEntries");
			for (uint i = 0; i < iSize; i++)
			{
				int iTurn;
				double fScore = 0;
				WRAPPER_READ(wrapper, "CvPlayer", &iTurn);
				WRAPPER_READ(wrapper, "CvPlayer", &fScore);
				m_mapEconomyHistory[iTurn] = static_cast<int64_t>(fScore);
			}
		}

		{
			m_mapIndustryHistory.clear();
			uint iSize;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numIndHistEntries");
			for (uint i = 0; i < iSize; i++)
			{
				int iTurn;
				double fScore = 0;
				WRAPPER_READ(wrapper, "CvPlayer", &iTurn);
				WRAPPER_READ(wrapper, "CvPlayer", &fScore);
				m_mapIndustryHistory[iTurn] = static_cast<int64_t>(fScore);
			}
		}

		{
			m_mapAgricultureHistory.clear();
			uint iSize;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numAgriHistEntries");
			for (uint i = 0; i < iSize; i++)
			{
				int iTurn;
				double fScore = 0;
				WRAPPER_READ(wrapper, "CvPlayer", &iTurn);
				WRAPPER_READ(wrapper, "CvPlayer", &fScore);
				m_mapAgricultureHistory[iTurn] = static_cast<int64_t>(fScore);
			}
		}

		{
			m_mapPowerHistory.clear();
			uint iSize;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numPowerHistEntries");
			for (uint i = 0; i < iSize; i++)
			{
				int iTurn;
				double fScore = 0;
				WRAPPER_READ(wrapper, "CvPlayer", &iTurn);
				WRAPPER_READ(wrapper, "CvPlayer", &fScore);
				m_mapPowerHistory[iTurn] = static_cast<int64_t>(fScore);
			}
		}

		{
			m_mapCultureHistory.clear();
			uint iSize;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numCultHistEntries");
			for (uint i = 0; i < iSize; i++)
			{
				int iTurn;
				double fScore = 0;
				WRAPPER_READ(wrapper, "CvPlayer", &iTurn);
				WRAPPER_READ(wrapper, "CvPlayer", &fScore);
				m_mapCultureHistory[iTurn] = static_cast<int64_t>(fScore);
			}
		}

		{
			m_mapEspionageHistory.clear();
			uint iSize;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numEspHistEntries");
			for (uint i = 0; i < iSize; i++)
			{
				int iTurn;
				double fScore = 0;
				WRAPPER_READ(wrapper, "CvPlayer", &iTurn);
				WRAPPER_READ(wrapper, "CvPlayer", &fScore);
				m_mapEspionageHistory[iTurn] = static_cast<int64_t>(fScore);
			}
		}

		// RevolutionDCM - read revolution stability history data from disk
		{
			m_mapRevolutionStabilityHistory.clear();
			uint iSize;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numRevHistEntries");
			for (uint i = 0; i < iSize; i++)
			{
				int iTurn;
				double fScore = 0;
				WRAPPER_READ(wrapper, "CvPlayer", &iTurn);
				WRAPPER_READ(wrapper, "CvPlayer", &fScore);
				m_mapRevolutionStabilityHistory[iTurn] = static_cast<int64_t>(fScore);
			}
		}
		// RevolutionDCM end

		{
			m_mapEventsOccured.clear();
			uint iSize;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numEventsOcurred");
			for (uint i = 0; i < iSize; i++)
			{
				EventTriggeredData kData;
				EventTypes eEvent = NO_EVENT;
				WRAPPER_READ_CLASS_ENUM_ALLOW_MISSING(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_EVENTS, (int*)&eEvent);

				kData.read(pStream);
				if (eEvent != NO_EVENT)
				{
					m_mapEventsOccured[eEvent] = kData;
				}
			}
		}

		{
			m_mapEventCountdown.clear();
			uint iSize;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numCountdownEvents");
			for (uint i = 0; i < iSize; i++)
			{
				EventTriggeredData kData;
				EventTypes eEvent;
				WRAPPER_READ(wrapper, "CvPlayer", (int*)&eEvent);
				kData.read(pStream);
				m_mapEventCountdown[eEvent] = kData;
			}
		}

		{
			m_aFreeUnitCombatPromotions.clear();
			uint iSize;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numFreeCombatPromotions");
			for (uint i = 0; i < iSize; i++)
			{
				int iUnitCombat;
				int iPromotion;
				WRAPPER_READ_CLASS_ENUM(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_COMBATINFOS, &iUnitCombat);
				WRAPPER_READ_CLASS_ENUM(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_PROMOTIONS, &iPromotion);
				m_aFreeUnitCombatPromotions.push_back(std::make_pair((UnitCombatTypes)iUnitCombat, (PromotionTypes)iPromotion));
			}
		}

		{
			m_aFreeUnitPromotions.clear();
			uint iSize;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numFreeUnitPromotions");
			for (uint i = 0; i < iSize; i++)
			{
				int iUnit;
				int iPromotion;
				WRAPPER_READ_CLASS_ENUM(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_UNITS, &iUnit);
				WRAPPER_READ_CLASS_ENUM(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_PROMOTIONS, &iPromotion);
				m_aFreeUnitPromotions.push_back(std::make_pair((UnitTypes)iUnit, (PromotionTypes)iPromotion));
			}
		}

		{
			m_aVote.clear();
			uint iSize;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numVotes");
			for (uint i = 0; i < iSize; i++)
			{
				int iId;
				PlayerVoteTypes eVote;
				WRAPPER_READ(wrapper, "CvPlayer", &iId);
				WRAPPER_READ_CLASS_ENUM(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_VOTES, (int*)&eVote);
				m_aVote.push_back(std::make_pair(iId, eVote));
			}
		}

		{
			m_aUnitExtraCosts.clear();
			uint iSize;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numUnitCosts");
			for (uint i = 0; i < iSize; i++)
			{
				int iCost;
				UnitTypes eUnit;
				WRAPPER_READ_CLASS_ENUM(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_UNITS, (int*)&eUnit);
				WRAPPER_READ(wrapper, "CvPlayer", &iCost);
				m_aUnitExtraCosts.push_back(std::make_pair(eUnit, iCost));
			}
		}

		{
			m_triggersFired.clear();
			uint iSize;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numEventTriggers");
			for (uint i = 0; i < iSize; i++)
			{
				int iTrigger = -2;

				WRAPPER_READ_CLASS_ENUM_ALLOW_MISSING(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_EVENT_TRIGGERS, &iTrigger);

				if ( iTrigger == -2 && wrapper.isUsingTaggedFormat() )
				{
					//	Consume the value for older formats since the information cannot be
					//	believed anyway
					WRAPPER_READ(wrapper, "CvPlayer", &iTrigger);
				}
				else if ( iTrigger != -1 )
				{
					m_triggersFired.push_back((EventTriggerTypes)iTrigger);
				}
			}
		}

		if (!isNPC())
		{
			// Get the NetID from the initialization structure
			setNetID(gDLL->getAssignedNetworkID(getID()));
		}

		WRAPPER_READ(wrapper, "CvPlayer", &m_iPopRushHurryCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iInflationModifier);

		int	iTempUnitId = -1;
		WRAPPER_READ(wrapper, "CvPlayer", &iTempUnitId);
		if ( iTempUnitId != -1 )
		{
			m_pTempUnit = getUnit(iTempUnitId);

			FAssert(m_pTempUnit);
		}
		{
			int iCount = 0;
			foreach_(CvUnit* plotlessUnit, plotlessUnits)
			{
				if (plotlessUnit != m_pTempUnit)
				{
					plotlessUnit->kill(true);
					iCount++;
				}
			}
			FAssertMsg(iCount == 0, CvString::format("%d plotless units somehow got into the save!", iCount).c_str());
		}
		{
			// Handle units with invalid plots that somehow get into saves!
			const int iMaxX = GC.getMap().getGridWidth();
			const int iMaxY = GC.getMap().getGridHeight();
			int iCount = 0;
			foreach_(CvUnit* unitX, units())
			{
				if (unitX == m_pTempUnit)
				{
					continue;
				}
				if (!unitX->plot() || unitX->getX() < 0 || unitX->getX() >= iMaxX || unitX->getY() < 0 || unitX->getY() >= iMaxY)
				{
					CvSelectionGroup* pGroup = unitX->getGroup();

					// Toffer - X and Y can for some reason be stored as -1,
					//	and plot() will not return NULL unless they are INVALID_PLOT_COORD which cause problems.
					unitX->forceInvalidCoordinates();
					unitX->joinGroup(NULL, false, false);

					deleteUnit(unitX->getID());
					iCount++;
					if (pGroup->getNumUnits() == 0)
					{
						pGroup->kill();
					}
				}
			}
			FAssertMsg(iCount == 0, CvString::format("%d dead/plotless units somehow got into the save!", iCount).c_str());
		}
		CLLNode<int>* pCurrUnitNode;
		CLLNode<int>* pNextUnitNode;
		pCurrUnitNode = headGroupCycleNode();

		while (pCurrUnitNode)
		{
			CvSelectionGroup* pLoopSelectionGroup = getSelectionGroup(pCurrUnitNode->m_data);
			pNextUnitNode = nextGroupCycleNode(pCurrUnitNode);

			if ( pLoopSelectionGroup == NULL || pLoopSelectionGroup->plot() == NULL )
			{
				removeGroupCycle(pCurrUnitNode->m_data);

				if ( pLoopSelectionGroup )
				{
					deleteSelectionGroup(pCurrUnitNode->m_data);
				}
			}
			pCurrUnitNode = pNextUnitNode;
		}
		//TB Combat Mod begin
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiPlayerWideAfflictionCount);
		//TB Traits begin
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCivicAnarchyModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iReligiousAnarchyModifier);
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_IMPROVEMENTS, GC.getNumImprovementInfos(), m_paiImprovementUpgradeRateModifierSpecific);
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_IMPROVEMENTS, GC.getNumImprovementInfos(), m_paiBuildWorkerSpeedModifierSpecific);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iAIAttitudeModifier);

		for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_SPECIALISTS); ++i)
		{
			int	iI = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_SPECIALISTS, i, true);

			if ( iI != -1 )
			{
				WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_ppaaiSpecialistExtraCommerce[iI]);
			}
			else
			{
				//	Consume the values
				WRAPPER_SKIP_ELEMENT(wrapper, "CvPlayer", m_ppaaiSpecialistExtraCommerce[iI], SAVE_VALUE_TYPE_INT_ARRAY);
			}
		}
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_aiFreeCityYield);
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_aiSpecialistExtraYield);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iTraitExtraCityDefense);
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_TRAITS, GC.getNumTraitInfos(), m_pabHasTrait);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iLeaderHeadLevel);
		// @SAVEBREAK - Delete
		WRAPPER_SKIP_ELEMENT(wrapper, "CvPlayer", m_iTraitDisplayCount, SAVE_VALUE_ANY);
		// !SAVEBREAK
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNationalEspionageDefense);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iInquisitionCount);
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_aiLessYieldThreshold);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCompatCheckCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNationalHurryAngerModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNationalEnemyWarWearinessModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNationalBombardDefenseModifier);
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_DOMAIN_TYPES, m_paiNationalDomainFreeExperience);
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_DOMAIN_TYPES, m_paiNationalDomainProductionModifier);
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiNationalTechResearchModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iFixedBordersCount);

		WRAPPER_READ(wrapper, "CvPlayer", &m_iExtraNationalCaptureProbabilityModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iExtraNationalCaptureResistanceModifier);

		WRAPPER_READ(wrapper, "CvPlayer", &m_iAllReligionsActiveCount);

		WRAPPER_READ(wrapper, "CvPlayer", &m_iExtraStateReligionSpreadModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iExtraNonStateReligionSpreadModifier);
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiEraAdvanceFreeSpecialistCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNationalCityStartCulture);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNationalAirUnitCapacity);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCapitalXPModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iStateReligionHolyCityXPModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNonStateReligionHolyCityXPModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNationalCityStartBonusPopulation);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNationalMissileRangeChange);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNationalFlightOperationRangeChange);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNationalNavalCargoSpaceChange);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNationalMissileCargoSpaceChange);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCitiesStartwithStateReligionCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iDraftsOnCityCaptureCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iFreeSpecialistperWorldWonderCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iFreeSpecialistperNationalWonderCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iFreeSpecialistperTeamProjectCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iExtraGoodyCount);

		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_aiGoldenAgeYield);
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiGoldenAgeCommerce);

		WRAPPER_READ(wrapper, "CvPlayer", &m_iBaseMergeSelection);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iFirstMergeSelection);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iSecondMergeSelection);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iSplittingUnit);

		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiCommerceRateModifierfromEvents);
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiCommerceRateModifierfromBuildings);

		WRAPPER_READ(wrapper, "CvPlayer", (int*)&m_eGreatGeneralTypetoAssign);

		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_abCommerceDirty);

		WRAPPER_READ(wrapper, "CvPlayer", &m_iFocusPlotX);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iFocusPlotY);

		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_SPECIAL_BUILDINGS, GC.getNumSpecialBuildingInfos(), m_paiBuildingGroupCount);
		WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_SPECIAL_BUILDINGS, GC.getNumSpecialBuildingInfos(), m_paiBuildingGroupMaking);

		// @SAVEBREAK - remove
		WRAPPER_SKIP_ELEMENT(wrapper, "CvPlayer", m_iArrestingUnit, SAVE_VALUE_ANY);
		// !SAVEBREAK
		WRAPPER_READ(wrapper, "CvPlayer", &m_iUpgradeRoundCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iSelectionRegroup);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iFreedomFighterCount);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iExtraFreedomFighters);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iNationalGreatPeopleRate);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCivilianUnitUpkeepMod);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iMilitaryUnitUpkeepMod);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iUnitUpkeepCivilian100);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iUnitUpkeepMilitary100);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iBaseFreeUnitUpkeepCivilian);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iBaseFreeUnitUpkeepMilitary);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iFreeUnitUpkeepCivilianPopPercent);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iFreeUnitUpkeepMilitaryPopPercent);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iGold);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCulture);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iMinTaxIncome);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iMaxTaxIncome);

		// Toffer - Read maps
		{
			short iSize;
			short iType;
			char cCount;
			int iCount;
			short sCount;
			uint16_t sCountU;
			uint32_t iCountU;
			// Bonus counters
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "iBonusExportSize");
			while (iSize-- > 0)
			{
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iType, "iBonusExportType");
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iCountU, "iBonusExportCount");
				iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BONUSES, iType, true));

				if (iType > -1)
				{
					m_bonusExport.insert(std::make_pair(iType, iCountU));
				}
			}

			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "iBonusImportSize");
			while (iSize-- > 0)
			{
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iType, "iBonusImportType");
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iCountU, "iBonusImportCount");
				iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BONUSES, iType, true));

				if (iType > -1)
				{
					m_bonusImport.insert(std::make_pair(iType, iCountU));
				}
			}

			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "iBonusMintedPercentSize");
			while (iSize-- > 0)
			{
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iType, "iBonusMintedPercentType");
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iCount, "iBonusMintedPercentCount");
				iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BONUSES, iType, true));

				if (iType > -1)
				{
					m_bonusMintedPercent.insert(std::make_pair(iType, iCount));
				}
			}
			// Building counters

			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "iBuildingMakingSize");
			while (iSize-- > 0)
			{
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iType, "iBuildingMakingType");
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iCountU, "iBuildingMakingCount");
				iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDINGS, iType, true));

				if (iType > -1)
				{
					m_buildingMaking.insert(std::make_pair(iType, iCountU));
				}
			}

			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "iFreeBuildingCountSize");
			while (iSize-- > 0)
			{
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iType, "iFreeBuildingCountType");
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &sCountU, "iFreeBuildingCountCount");
				iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDINGS, iType, true));

				if (iType > -1)
				{
					m_freeBuildingCount.insert(std::make_pair(iType, sCountU));
				}
			}

			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "iExtraBuildingHappinessSize");
			while (iSize-- > 0)
			{
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iType, "iExtraBuildingHappinessType");
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iCount, "iExtraBuildingHappinessCount");
				iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDINGS, iType, true));

				if (iType > -1)
				{
					m_extraBuildingHappiness.insert(std::make_pair(iType, iCount));
				}
			}

			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "iExtraBuildingHealthSize");
			while (iSize-- > 0)
			{
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iType, "iExtraBuildingHealthType");
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iCount, "iExtraBuildingHealthCount");
				iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDINGS, iType, true));

				if (iType > -1)
				{
					m_extraBuildingHealth.insert(std::make_pair(iType, iCount));
				}
			}

			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "iBuildingProductionModSize");
			while (iSize-- > 0)
			{
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iType, "iBuildingProductionModType");
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iCount, "iBuildingProductionModCount");
				iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDINGS, iType, true));

				if (iType > -1)
				{
					m_buildingProductionMod.insert(std::make_pair(iType, iCount));
				}
			}

			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "iBuildingCostModSize");
			while (iSize-- > 0)
			{
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iType, "iBuildingCostModType");
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iCount, "iBuildingCostModCount");
				iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDINGS, iType, true));

				if (iType > -1)
				{
					m_buildingCostMod.insert(std::make_pair(iType, iCount));
				}
			}
			// Unit counters
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "iUnitCountSize");
			while (iSize-- > 0)
			{
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iType, "iUnitCountType");
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iCountU, "iUnitCountCount");
				iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_UNITS, iType, true));

				if (iType > -1)
				{
					m_unitCount.insert(std::make_pair(iType, iCountU));
				}
			}

			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "iUnitMakingSize");
			while (iSize-- > 0)
			{
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iType, "iUnitMakingType");
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iCountU, "iUnitMakingCount");
				iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_UNITS, iType, true));

				if (iType > -1)
				{
					m_unitMaking.insert(std::make_pair(iType, iCountU));
				}
			}

			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "iGreatGeneralPointsTypeSize");
			while (iSize-- > 0)
			{
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iType, "iGreatGeneralPointsTypeType");
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iCountU, "iGreatGeneralPointsTypeCount");
				iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_UNITS, iType, true));

				if (iType > -1)
				{
					m_greatGeneralPointsType.insert(std::make_pair(iType, iCountU));
				}
			}

			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "iGoldenAgeOnBirthOfGreatPersonCountSize");
			while (iSize-- > 0)
			{
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iType, "iGoldenAgeOnBirthOfGreatPersonCountType");
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &cCount, "iGoldenAgeOnBirthOfGreatPersonCountCount");
				iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_UNITS, iType, true));

				if (iType > -1)
				{
					m_goldenAgeOnBirthOfGreatPersonCount.insert(std::make_pair(iType, cCount));
				}
			}

			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "iGreatPeopleRateforUnitSize");
			while (iSize-- > 0)
			{
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iType, "iGreatPeopleRateforUnitType");
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iCount, "iGreatPeopleRateforUnitCount");
				iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_UNITS, iType, true));

				if (iType > -1)
				{
					m_greatPeopleRateforUnit.insert(std::make_pair(iType, iCount));
				}
			}

			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "iUnitProductionModSize");
			while (iSize-- > 0)
			{
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iType, "iUnitProductionModType");
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iCount, "iUnitProductionModCount");
				iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_UNITS, iType, true));

				if (iType > -1)
				{
					m_unitProductionMod.insert(std::make_pair(iType, iCount));
				}
			}
			// Unit-Combat counters
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "iUnitCombatProductionModSize");
			while (iSize-- > 0)
			{
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iType, "iUnitCombatProductionModType");
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iCount, "iUnitCombatProductionModCount");
				iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_COMBATINFOS, iType, true));

				if (iType > -1)
				{
					m_unitCombatProductionMod.insert(std::make_pair(iType, iCount));
				}
			}

			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "iUnitCombatFreeXPSize");
			while (iSize-- > 0)
			{
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iType, "iUnitCombatFreeXPType");
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &sCount, "iUnitCombatFreeXPCount");
				iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_COMBATINFOS, iType, true));

				if (iType > -1)
				{
					m_unitCombatFreeXP.insert(std::make_pair(iType, sCount));
				}
			}

			// Unit counters
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "UnitCountSMSize");
			while (iSize-- > 0)
			{
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iType, "UnitCountSMType");
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iCountU, "UnitCountSM");
				iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_UNITS, iType, true));

				if (iType > -1)
				{
					m_unitCountSM.insert(std::make_pair(iType, iCountU));
				}
			}
		}
		{
			int iMilitary = 0;

			foreach_(const CvUnit* unitX, units())
			{
				if (!unitX->isTempUnit() && GC.getUnitInfo(unitX->getUnitType()).isMilitarySupport())
				{
					iMilitary++;
				}
			}
			if (iMilitary != m_iNumMilitaryUnits)
			{
				FErrorMsg("count of military unit out of sync! Correcting");
				m_iNumMilitaryUnits = iMilitary;
			}
		}
		WRAPPER_READ(wrapper, "CvPlayer", &m_iHomeAreaMaintenanceModifier);
		WRAPPER_READ(wrapper, "CvPlayer", &m_iOtherAreaMaintenanceModifier);

		{
			m_myHeritage.clear();
			uint iSize = 0;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numHeritage");
			for (uint i = 0; i < iSize; i++)
			{
				int iType = -2;

				WRAPPER_READ_CLASS_ENUM_ALLOW_MISSING(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_HERITAGE, &iType);

				if (iType == -2 && wrapper.isUsingTaggedFormat())
				{
					// Consume the value for older formats since the information cannot be believed anyway
					WRAPPER_READ(wrapper, "CvPlayer", &iType);
				}
				else if (iType > -1)
				{
					m_myHeritage.push_back((HeritageTypes)iType);
				}
			}
		}
		WRAPPER_READ_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_extraCommerce);
		WRAPPER_READ(wrapper, "CvPlayer", &m_bHasLanguage);
		// Read Vector
		{
			uint iSize = 0;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numCommandFieldPlots");
			for (uint i = 0; i < iSize; i++)
			{
				short iX = -1;
				short iY = -1;
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iX, "CommandFieldPlotX");
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iY, "CommandFieldPlotY");
				if (iX > -1 && iY > -1)
				{
					setCommandFieldPlot(true, GC.getMap().plot(iX, iY));
				}
			}
		}
		{
			uint iSize = 0;
			WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iSize, "numCommodoreFieldPlots");
			for (uint i = 0; i < iSize; i++)
			{
				short iX = -1;
				short iY = -1;
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iX, "CommodoreFieldPlotX");
				WRAPPER_READ_DECORATED(wrapper, "CvPlayer", &iY, "CommodoreFieldPlotY");
				if (iX > -1 && iY > -1)
				{
					setCommodoreFieldPlot(true, GC.getMap().plot(iX, iY));
				}
			}
		}
		WRAPPER_READ(wrapper, "CvPlayer", &m_iCorporateMaintenance);
		// @SAVEBREAK - delete
		m_iCorporateMaintenance += m_iCorporateTaxIncome;
		// !SAVEBREAK

		//Example of how to skip element
		//WRAPPER_SKIP_ELEMENT(wrapper, "CvPlayer", m_iPopulationgrowthratepercentage, SAVE_VALUE_ANY);
	}
	else
	{
		m_bEverAlive = false;
		m_bAlive = false;

		uninit();
	}

	WRAPPER_READ_OBJECT_END(wrapper);

	if (GC.viewportsEnabled() && GC.getGame().getActivePlayer() == m_eID)
	{
		//	Set the initial viewport to centre on the active player's capital until we
		//	know better (avoids an initial viewport move in most cases)
		if ( GC.getCurrentViewport()->getState() == VIEWPORT_MODE_UNINITIALIZED )
		{
			const CvCity* pCapital = getCapitalCity();

			if ( pCapital )
			{
				GC.getCurrentViewport()->setOffsetToShow(pCapital->getX(), pCapital->getY());
			}
		}
	}
	// Toffer - To stop auto end turn on decision-less turns from kicking in immediately when loading a save
	setTurnHadUIInteraction(true);
}

//
// save object to a stream
// used during save
//
void CvPlayer::write(FDataStreamBase* pStream)
{
	PROFILE_EXTRA_FUNC();
	int iI;

	uint uiFlag = 1;

	if ( !m_bEverAlive )
	{
		uiFlag |= PLAYER_UI_FLAG_OMITTED;
	}

	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE(wrapper, "CvPlayer", uiFlag);		// flag for expansion

	if ( m_bEverAlive )
	{
		const int m_iStartingX = m_startingCoords[MAP_EARTH].iX;
		const int m_iStartingY = m_startingCoords[MAP_EARTH].iY;
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iStartingX);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iStartingY);

		WRAPPER_WRITE(wrapper, "CvPlayer", m_iTotalPopulation);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iTotalLand);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iTotalLandScored);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iGoldPerTurn);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iAdvancedStartPoints);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iGoldenAgeTurns);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNumUnitGoldenAges);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iStrikeTurns);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iAnarchyTurns);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iMaxAnarchyTurns);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iAnarchyModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iGoldenAgeModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iGlobalHurryModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iGreatPeopleCreated);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iGreatGeneralsCreated);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iGreatPeopleThresholdModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iGreatGeneralsThresholdModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iGreatPeopleRateModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iGreatGeneralRateModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iDomesticGreatGeneralRateModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iStateReligionGreatPeopleRateModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iMaxGlobalBuildingProductionModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iMaxTeamBuildingProductionModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iMaxPlayerBuildingProductionModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iFreeExperience);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iFeatureProductionModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iWorkerSpeedModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iImprovementUpgradeRateModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iMilitaryProductionModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iSpaceProductionModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCityDefenseModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNonStateReligionCommerceCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iUpgradeAnywhereCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iRevIdxLocal);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iRevIdxNational);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iRevIdxDistanceModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iRevIdxHolyCityGood);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iRevIdxHolyCityBad);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_fRevIdxNationalityMod);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_fRevIdxBadReligionMod);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_fRevIdxGoodReligionMod);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_bInquisitionConditions);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iUnitUpgradePriceModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNumNukeUnits);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNumOutsideUnits);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNumMilitaryUnits);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iHappyPerMilitaryUnit);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iMilitaryFoodProductionCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iConscriptCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iMaxConscript);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iHighestUnitLevel);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iOverflowResearch);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNoUnhealthyPopulationCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iExpInBorderModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iBuildingOnlyHealthyCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iMaintenanceModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCoastalDistanceMaintenanceModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iConnectedCityMaintenanceModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iDistanceMaintenanceModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNumCitiesMaintenanceModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCorporationMaintenanceModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iTotalMaintenance);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iUpkeepModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iLevelExperienceModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iExtraHealth);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCivicHealth);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iBuildingGoodHealth);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iBuildingBadHealth);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iExtraHappiness);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iExtraHappinessUnattributed);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iBuildingHappiness);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iLargestCityHappiness);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iWarWearinessPercentAnger);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iWarWearinessModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iFreeSpecialist);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNoForeignTradeCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNoCorporationsCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNoForeignCorporationsCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCoastalTradeRoutes);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iTradeRoutes);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iRevolutionTimer);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iConversionTimer);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iStateReligionCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNoNonStateReligionSpreadCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iStateReligionHappiness);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNonStateReligionHappiness);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iStateReligionUnitProductionModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iStateReligionBuildingProductionModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iStateReligionFreeExperience);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCapitalCityID);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCitiesLost);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iWinsVsBarbs);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iAssets);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iPower);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iTechPower);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iUnitPower);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iPopulationScore);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iLandScore);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iWondersScore);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iTechScore);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCombatExperience);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_bAlive);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_bEverAlive);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_bTurnActive);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_bAutoMoves);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_bEndTurn);

		WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", m_bPbemNewTurn && GC.getGame().isPbem(), "m_bPbemNewTurn");

		WRAPPER_WRITE(wrapper, "CvPlayer", m_bExtendedGame);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_bFoundedFirstCity);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_bStrike);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_bNukesValid);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_bDisableHuman); // AI_AUTO_PLAY - 07/09/08 - jdog5000
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iStabilityIndex);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iStabilityIndexAverage);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_bRebel);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iMotherPlayer);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_bDoNotBotherStatus);

		// Used for DynamicCivNames
		WRAPPER_WRITE_STRING(wrapper, "CvPlayer", m_szName);
		WRAPPER_WRITE_STRING(wrapper, "CvPlayer", m_szCivDesc);
		WRAPPER_WRITE_STRING(wrapper, "CvPlayer", m_szCivShort);
		WRAPPER_WRITE_STRING(wrapper, "CvPlayer", m_szCivAdj);

		WRAPPER_WRITE(wrapper, "CvPlayer", m_eID);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_ePersonalityType);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_eCurrentEra);
		WRAPPER_WRITE_CLASS_ENUM(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_RELIGIONS, m_eLastStateReligion);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_eParent);
		//m_eTeamType not saved

		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_aiSeaPlotYield);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_aiYieldRateModifier);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_aiCapitalYieldRateModifier);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_aiExtraYieldThreshold);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_aiTradeYieldModifier);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiFreeCityCommerce);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiCommercePercent);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiCommerceRate);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiCommerceRateModifier);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiCapitalCommerceRateModifier);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiStateReligionBuildingCommerce);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiSpecialistExtraCommerce);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiCommerceFlexibleCount);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", MAX_PLAYERS, m_aiGoldPerTurnByPlayer);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", MAX_TEAMS, m_aiEspionageSpendingWeightAgainstTeam);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_FEAT_TYPES, m_abFeatAccomplished);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_PLAYEROPTION_TYPES, m_abOptions);

		WRAPPER_WRITE_STRING(wrapper, "CvPlayer", m_szScriptData);

		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_IMPROVEMENTS, GC.getNumImprovementInfos(), m_paiImprovementCount);
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_FEATURES, GC.getNumFeatureInfos(), m_paiFeatureHappiness);
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiBuildingCount);

		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_HURRIES, GC.getNumHurryInfos(), m_paiHurryCount);
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_SPECIAL_BUILDINGS, GC.getNumSpecialBuildingInfos(), m_paiSpecialBuildingNotRequiredCount);
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_CIVIC_OPTIONS, GC.getNumCivicOptionInfos(), m_paiHasCivicOptionCount);
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_CIVIC_OPTIONS, GC.getNumCivicOptionInfos(), m_paiNoCivicUpkeepCount);
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_RELIGIONS, GC.getNumReligionInfos(), m_paiHasReligionCount);
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_CORPORATIONS, GC.getNumCorporationInfos(), m_paiHasCorporationCount);
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_UPKEEPS, GC.getNumUpkeepInfos(), m_paiUpkeepCount);
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiSpecialistValidCount);

		WRAPPER_WRITE(wrapper, "CvPlayer", m_iForeignTradeRouteModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iTaxRateUnhappiness);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_fPopulationgrowthratepercentageLog);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iReligionSpreadRate);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCivicHappiness);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iDistantUnitSupportCostModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iExtraCityDefense);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iEnslavementChance);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNoCapitalUnhappiness);

		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_aiLandmarkYield);

		WRAPPER_WRITE(wrapper, "CvPlayer", m_iWorldHealth);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iWorldHappiness);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iProjectHealth);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iProjectHappiness);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iForceAllTradeRoutes);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCivilizationHealth);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iBuildingInflation);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iProjectInflation);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iTechInflation);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCivicInflation);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iHurryCostModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iHurryInflationModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iHurryCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNoLandmarkAngerCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iLandmarkHappiness);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCorporationSpreadModifier);
		// @SAVEBREAK - delete
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCorporateTaxIncome);
		// !SAVEBREAK
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCityLimit);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCityOverLimitUnhappy);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iForeignUnhappyPercent);

		//	Subdue and construct-by-unit stats
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNumAnimalsSubdued);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNumAnarchyTurns);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNumCivicSwitches);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNumCivicsSwitched);

		// Toffer - Remove @SAVEBREAK as it is only used for logging purposes, not particularly interesting logging.
		const int iNumBuildingTypes = m_unitConstructionCounts.size();
		WRAPPER_WRITE(wrapper, "CvPlayer", iNumBuildingTypes);
		for(std::map<BuildingTypes,int>::const_iterator itr = m_unitConstructionCounts.begin(); itr != m_unitConstructionCounts.end(); ++itr)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", itr->first, "eBuilding");
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", itr->second, "iConstructionCount");
		}

		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_BUILDS, GC.getNumBuildInfos(), m_pabAutomatedCanBuild);
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiResourceConsumption);
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiFreeSpecialistCount);

		WRAPPER_WRITE(wrapper, "CvPlayer", m_ePledgedVote);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_eSecretaryGeneralVote);

		foreach_(const civcSwitchInstance& switchInstance, m_civicSwitchHistory)
		{
			WRAPPER_WRITE(wrapper, "CvPlayer", switchInstance.iTurn);
			WRAPPER_WRITE_CLASS_ENUM(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_CIVICS, switchInstance.eFromCivic);
			WRAPPER_WRITE_CLASS_ENUM(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_CIVICS, switchInstance.eToCivic);
			WRAPPER_WRITE(wrapper, "CvPlayer", switchInstance.bNoAnarchy);
		}

		m_Properties.writeWrapper(pStream);

		for (iI=0;iI<GC.getNumTerrainInfos();iI++)
		{
			WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_ppaaiTerrainYieldChange[iI]);
		}
		for (int i = 0; i < GC.getNumBuildingInfos(); ++i)
		{
			WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_ppiBuildingCommerceModifier[i]);
		}
		for (int i = 0; i < GC.getNumBuildingInfos(); ++i)
		{
			WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_ppiBuildingCommerceChange[i]);
		}
		for (int i = 0; i < GC.getNumBonusInfos(); ++i)
		{
			WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_ppiBonusCommerceModifier[i]);
		}
		for (int i = 0; i < GC.getNumSpecialistInfos(); ++i)
		{
			WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_ppiSpecialistYieldPercentChanges[i]);
			WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_ppiSpecialistCommercePercentChanges[i]);
		}

		FAssertMsg((0 < GC.getNumTechInfos()), "GC.getNumTechInfos() is not greater than zero but it is expected to be in CvPlayer::write");
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_pabResearchingTech);

		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_VOTE_SOURCES, GC.getNumVoteSourceInfos(), m_pabLoyalMember);

		for (iI=0;iI<GC.getNumCivicOptionInfos();iI++)
		{
			WRAPPER_WRITE_CLASS_ENUM(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_CIVICS, m_paeCivics[iI]);
		}

		for (iI=0;iI<GC.getNumSpecialistInfos();iI++)
		{
			WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_ppaaiSpecialistExtraYield[iI]);
		}

		for (iI=0;iI<GC.getNumImprovementInfos();iI++)
		{
			WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_ppaaiImprovementYieldChange[iI]);
		}
		m_researchQueue.Write(pStream);

		{
			CLLNode<CvWString>* pNode;
			uint iSize = m_cityNames.getLength();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numCities");
			pNode = m_cityNames.head();
			while (pNode)
			{
				WRAPPER_WRITE_STRING_DECORATED(wrapper, "CvPlayer", pNode->m_data, "cityName");
				pNode = m_cityNames.next(pNode);
			}
		}

		WRAPPER_WRITE(wrapper, "CvPlayer", m_iMaxTradeRoutesAdjustment);

		WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", NUM_MAPS, "NUM_MAPS");

		for (int i = 0; i < NUM_MAPS; i++)
		{
			m_groupCycles[i]->Write(pStream);
			WriteStreamableFFreeListTrashArray(*m_plotGroups[i], pStream);
			WriteStreamableFFreeListTrashArray(*m_cities[i], pStream);
			WriteStreamableFFreeListTrashArray(*m_units[i], pStream);
			WriteStreamableFFreeListTrashArray(*m_selectionGroups[i], pStream);
		}

		WriteStreamableFFreeListTrashArray(m_eventsTriggered, pStream);

		{
			CvMessageQueue::_Alloc::size_type iSize = m_listGameMessages.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numListGameMessages");
			foreach_(CvTalkingHeadMessage& message, m_listGameMessages)
			{
				message.write(*pStream);
			}
		}

		{
			CvPopupQueue currentPopups;
			if (GC.getGame().isNetworkMultiPlayer())
			{
				// don't save open popups in MP to avoid having different state on different machines
				currentPopups.clear();
			}
			else
			{
				gDLL->getInterfaceIFace()->getDisplayedButtonPopups(currentPopups);
			}
			CvPopupQueue::_Alloc::size_type iSize = m_listPopups.size() + currentPopups.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numPopups");
			foreach_(CvPopupInfo* pInfo, currentPopups)
			{
				if (pInfo)
				{
					pInfo->write(*pStream);
				}
			}
			foreach_(CvPopupInfo* pInfo, m_listPopups)
			{
				if (pInfo)
				{
					pInfo->write(*pStream);
				}
			}
		}

		{
			CvDiploQueue::_Alloc::size_type iSize = m_listDiplomacy.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numDiploParams");
			foreach_(CvDiploParameters* pDiplo, m_listDiplomacy)
			{
				if (pDiplo)
				{
					pDiplo->write(*pStream);
				}
			}
		}

		{
			uint iSize = m_mapScoreHistory.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numScoreHistEntries");
			CvTurnScoreMap::iterator it;
			for (it = m_mapScoreHistory.begin(); it != m_mapScoreHistory.end(); ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iTurn");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", static_cast<double>(it->second), "fScore");
			}
		}

		{
			uint iSize = m_mapEconomyHistory.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numEconHistEntries");
			CvTurnScoreMap::iterator it;
			for (it = m_mapEconomyHistory.begin(); it != m_mapEconomyHistory.end(); ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iTurn");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", static_cast<double>(it->second), "fScore");
			}
		}

		{
			uint iSize = m_mapIndustryHistory.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numIndHistEntries");
			CvTurnScoreMap::iterator it;
			for (it = m_mapIndustryHistory.begin(); it != m_mapIndustryHistory.end(); ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iTurn");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", static_cast<double>(it->second), "fScore");
			}
		}

		{
			uint iSize = m_mapAgricultureHistory.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numAgriHistEntries");
			CvTurnScoreMap::iterator it;
			for (it = m_mapAgricultureHistory.begin(); it != m_mapAgricultureHistory.end(); ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iTurn");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", static_cast<double>(it->second), "fScore");
			}
		}

		{
			uint iSize = m_mapPowerHistory.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numPowerHistEntries");
			CvTurnScoreMap::iterator it;
			for (it = m_mapPowerHistory.begin(); it != m_mapPowerHistory.end(); ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iTurn");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", static_cast<double>(it->second), "fScore");
			}
		}

		{
			uint iSize = m_mapCultureHistory.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numCultHistEntries");
			CvTurnScoreMap::iterator it;
			for (it = m_mapCultureHistory.begin(); it != m_mapCultureHistory.end(); ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iTurn");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", static_cast<double>(it->second), "fScore");
			}
		}

		{
			uint iSize = m_mapEspionageHistory.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numEspHistEntries");
			CvTurnScoreMap::iterator it;
			for (it = m_mapEspionageHistory.begin(); it != m_mapEspionageHistory.end(); ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iTurn");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", static_cast<double>(it->second), "fScore");
			}
		}

		// RevolutionDCM - save revolution stability history to disk
		{
			uint iSize = m_mapRevolutionStabilityHistory.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numRevHistEntries");
			CvTurnScoreMap::iterator it;
			for (it = m_mapRevolutionStabilityHistory.begin(); it != m_mapRevolutionStabilityHistory.end(); ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iTurn");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", static_cast<double>(it->second), "fScore");
			}
		}
		// RevolutionDCM - end

		{
			uint iSize = m_mapEventsOccured.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numEventsOcurred");
			CvEventMap::iterator it;
			for (it = m_mapEventsOccured.begin(); it != m_mapEventsOccured.end(); ++it)
			{
				WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_EVENTS, it->first, "eEvent");
				it->second.write(pStream);
			}
		}

		{
			uint iSize = m_mapEventCountdown.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numCountdownEvents");
			CvEventMap::iterator it;
			for (it = m_mapEventCountdown.begin(); it != m_mapEventCountdown.end(); ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "eEvent");
				it->second.write(pStream);
			}
		}

		{
			uint iSize = m_aFreeUnitCombatPromotions.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numFreeCombatPromotions");
			UnitCombatPromotionArray::iterator it;
			for (it = m_aFreeUnitCombatPromotions.begin(); it != m_aFreeUnitCombatPromotions.end(); ++it)
			{
				WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_COMBATINFOS, (*it).first, "iUnitCombat");
				WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_PROMOTIONS, (*it).second, "iPromotion");
			}
		}

		{
			uint iSize = m_aFreeUnitPromotions.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numFreeUnitPromotions");
			UnitPromotionArray::iterator it;
			for (it = m_aFreeUnitPromotions.begin(); it != m_aFreeUnitPromotions.end(); ++it)
			{
				WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_UNITS, (*it).first, "iUnit");
				WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_PROMOTIONS, (*it).second, "iPromotion");
			}
		}

		{
			uint iSize = m_aVote.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numVotes");
			std::vector< std::pair<int, PlayerVoteTypes> >::iterator it;
			for (it = m_aVote.begin(); it != m_aVote.end(); ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (*it).first, "iId");
				WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_VOTES, (*it).second, "eVote");
			}
		}

		{
			uint iSize = m_aUnitExtraCosts.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numUnitCosts");
			std::vector< std::pair<UnitTypes, int> >::iterator it;
			for (it = m_aUnitExtraCosts.begin(); it != m_aUnitExtraCosts.end(); ++it)
			{
				WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_UNITS, (*it).first, "eUnit");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (*it).second, "iCost");
			}
		}

		{
			uint iSize = m_triggersFired.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numEventTriggers");
			foreach_(const EventTriggerTypes eTrigger, m_triggersFired)
			{
				WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_EVENT_TRIGGERS, eTrigger, "iTrigger");
			}
		}

		WRAPPER_WRITE(wrapper, "CvPlayer", m_iPopRushHurryCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iInflationModifier);

		int	iTempUnitId = (m_pTempUnit ? m_pTempUnit->getID() : -1);
		WRAPPER_WRITE(wrapper, "CvPlayer", iTempUnitId);
		//TB Combat mod begin
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_PROMOTIONLINES, GC.getNumPromotionLineInfos(), m_paiPlayerWideAfflictionCount);
		//TB Combat mod end
		//TB Traits begin
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCivicAnarchyModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iReligiousAnarchyModifier);
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_IMPROVEMENTS, GC.getNumImprovementInfos(), m_paiImprovementUpgradeRateModifierSpecific);
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_IMPROVEMENTS, GC.getNumImprovementInfos(), m_paiBuildWorkerSpeedModifierSpecific);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iAIAttitudeModifier);

		for (iI=0;iI<GC.getNumSpecialistInfos();iI++)
		{
			WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_ppaaiSpecialistExtraCommerce[iI]);
		}
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_aiFreeCityYield);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_aiSpecialistExtraYield);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iTraitExtraCityDefense);
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_TRAITS, GC.getNumTraitInfos(), m_pabHasTrait);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iLeaderHeadLevel);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNationalEspionageDefense);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iInquisitionCount);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_aiLessYieldThreshold);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCompatCheckCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNationalHurryAngerModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNationalEnemyWarWearinessModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNationalBombardDefenseModifier);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_DOMAIN_TYPES, m_paiNationalDomainFreeExperience);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_DOMAIN_TYPES, m_paiNationalDomainProductionModifier);
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiNationalTechResearchModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iFixedBordersCount);

		WRAPPER_WRITE(wrapper, "CvPlayer", m_iExtraNationalCaptureProbabilityModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iExtraNationalCaptureResistanceModifier);

		WRAPPER_WRITE(wrapper, "CvPlayer", m_iAllReligionsActiveCount);

		WRAPPER_WRITE(wrapper, "CvPlayer", m_iExtraStateReligionSpreadModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iExtraNonStateReligionSpreadModifier);
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiEraAdvanceFreeSpecialistCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNationalCityStartCulture);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNationalAirUnitCapacity);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCapitalXPModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iStateReligionHolyCityXPModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNonStateReligionHolyCityXPModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNationalCityStartBonusPopulation);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNationalMissileRangeChange);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNationalFlightOperationRangeChange);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNationalNavalCargoSpaceChange);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNationalMissileCargoSpaceChange);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCitiesStartwithStateReligionCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iDraftsOnCityCaptureCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iFreeSpecialistperWorldWonderCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iFreeSpecialistperNationalWonderCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iFreeSpecialistperTeamProjectCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iExtraGoodyCount);

		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_YIELD_TYPES, m_aiGoldenAgeYield);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiGoldenAgeCommerce);

		WRAPPER_WRITE(wrapper, "CvPlayer", m_iBaseMergeSelection);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iFirstMergeSelection);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iSecondMergeSelection);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iSplittingUnit);

		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiCommerceRateModifierfromEvents);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_aiCommerceRateModifierfromBuildings);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_eGreatGeneralTypetoAssign);
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_abCommerceDirty);

		WRAPPER_WRITE(wrapper, "CvPlayer", m_iFocusPlotX);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iFocusPlotY);
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_SPECIAL_BUILDINGS, GC.getNumSpecialBuildingInfos(), m_paiBuildingGroupCount);
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_SPECIAL_BUILDINGS, GC.getNumSpecialBuildingInfos(), m_paiBuildingGroupMaking);

		WRAPPER_WRITE(wrapper, "CvPlayer", m_iUpgradeRoundCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iSelectionRegroup);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iFreedomFighterCount);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iExtraFreedomFighters);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iNationalGreatPeopleRate);
		//TB Traits end

		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCivilianUnitUpkeepMod);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iMilitaryUnitUpkeepMod);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iUnitUpkeepCivilian100);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iUnitUpkeepMilitary100);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iBaseFreeUnitUpkeepCivilian);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iBaseFreeUnitUpkeepMilitary);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iFreeUnitUpkeepCivilianPopPercent);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iFreeUnitUpkeepMilitaryPopPercent);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iGold);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCulture);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iMinTaxIncome);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iMaxTaxIncome);

		// Toffer - Write maps
		{
			// Bonus counters
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)m_bonusExport.size(), "iBonusExportSize");
			for (std::map<short, uint32_t>::const_iterator it = m_bonusExport.begin(), itEnd = m_bonusExport.end(); it != itEnd; ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iBonusExportType");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->second, "iBonusExportCount");
			}
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)m_bonusImport.size(), "iBonusImportSize");
			for (std::map<short, uint32_t>::const_iterator it = m_bonusImport.begin(), itEnd = m_bonusImport.end(); it != itEnd; ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iBonusImportType");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->second, "iBonusImportCount");
			}
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)m_bonusMintedPercent.size(), "iBonusMintedPercentSize");
			for (std::map<short, int>::const_iterator it = m_bonusMintedPercent.begin(), itEnd = m_bonusMintedPercent.end(); it != itEnd; ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iBonusMintedPercentType");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->second, "iBonusMintedPercentCount");
			}
			// Building counters
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)m_buildingMaking.size(), "iBuildingMakingSize");
			for (std::map<short, uint32_t>::const_iterator it = m_buildingMaking.begin(), itEnd = m_buildingMaking.end(); it != itEnd; ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iBuildingMakingType");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->second, "iBuildingMakingCount");
			}
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)m_freeBuildingCount.size(), "iFreeBuildingCountSize");
			for (std::map<short, uint16_t>::const_iterator it = m_freeBuildingCount.begin(), itEnd = m_freeBuildingCount.end(); it != itEnd; ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iFreeBuildingCountType");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->second, "iFreeBuildingCountCount");
			}
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)m_extraBuildingHappiness.size(), "iExtraBuildingHappinessSize");
			for (std::map<short, int>::const_iterator it = m_extraBuildingHappiness.begin(), itEnd = m_extraBuildingHappiness.end(); it != itEnd; ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iExtraBuildingHappinessType");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->second, "iExtraBuildingHappinessCount");
			}
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)m_extraBuildingHealth.size(), "iExtraBuildingHealthSize");
			for (std::map<short, int>::const_iterator it = m_extraBuildingHealth.begin(), itEnd = m_extraBuildingHealth.end(); it != itEnd; ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iExtraBuildingHealthType");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->second, "iExtraBuildingHealthCount");
			}
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)m_buildingProductionMod.size(), "iBuildingProductionModSize");
			for (std::map<short, int>::const_iterator it = m_buildingProductionMod.begin(), itEnd = m_buildingProductionMod.end(); it != itEnd; ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iBuildingProductionModType");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->second, "iBuildingProductionModCount");
			}
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)m_buildingCostMod.size(), "iBuildingCostModSize");
			for (std::map<short, int>::const_iterator it = m_buildingCostMod.begin(), itEnd = m_buildingCostMod.end(); it != itEnd; ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iBuildingCostModType");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->second, "iBuildingCostModCount");
			}
			// Unit counters
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)m_unitCount.size(), "iUnitCountSize");
			for (std::map<short, uint32_t>::const_iterator it = m_unitCount.begin(), itEnd = m_unitCount.end(); it != itEnd; ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iUnitCountType");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->second, "iUnitCountCount");
			}
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)m_unitMaking.size(), "iUnitMakingSize");
			for (std::map<short, uint32_t>::const_iterator it = m_unitMaking.begin(), itEnd = m_unitMaking.end(); it != itEnd; ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iUnitMakingType");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->second, "iUnitMakingCount");
			}
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)m_greatGeneralPointsType.size(), "iGreatGeneralPointsTypeSize");
			for (std::map<short, uint32_t>::const_iterator it = m_greatGeneralPointsType.begin(), itEnd = m_greatGeneralPointsType.end(); it != itEnd; ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iGreatGeneralPointsTypeType");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->second, "iGreatGeneralPointsTypeCount");
			}
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)m_goldenAgeOnBirthOfGreatPersonCount.size(), "iGoldenAgeOnBirthOfGreatPersonCountSize");
			for (std::map<short, char>::const_iterator it = m_goldenAgeOnBirthOfGreatPersonCount.begin(), itEnd = m_goldenAgeOnBirthOfGreatPersonCount.end(); it != itEnd; ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iGoldenAgeOnBirthOfGreatPersonCountType");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->second, "iGoldenAgeOnBirthOfGreatPersonCountCount");
			}
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)m_greatPeopleRateforUnit.size(), "iGreatPeopleRateforUnitSize");
			for (std::map<short, int>::const_iterator it = m_greatPeopleRateforUnit.begin(), itEnd = m_greatPeopleRateforUnit.end(); it != itEnd; ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iGreatPeopleRateforUnitType");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->second, "iGreatPeopleRateforUnitCount");
			}
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)m_unitProductionMod.size(), "iUnitProductionModSize");
			for (std::map<short, int>::const_iterator it = m_unitProductionMod.begin(), itEnd = m_unitProductionMod.end(); it != itEnd; ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iUnitProductionModType");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->second, "iUnitProductionModCount");
			}
			// Unit-Combat counters
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)m_unitCombatProductionMod.size(), "iUnitCombatProductionModSize");
			for (std::map<short, int>::const_iterator it = m_unitCombatProductionMod.begin(), itEnd = m_unitCombatProductionMod.end(); it != itEnd; ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iUnitCombatProductionModType");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->second, "iUnitCombatProductionModCount");
			}
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)m_unitCombatFreeXP.size(), "iUnitCombatFreeXPSize");
			for (std::map<short, short>::const_iterator it = m_unitCombatFreeXP.begin(), itEnd = m_unitCombatFreeXP.end(); it != itEnd; ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "iUnitCombatFreeXPType");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->second, "iUnitCombatFreeXPCount");
			}
			// Unit counters
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)m_unitCountSM.size(), "UnitCountSMSize");
			for (std::map<short, uint32_t>::const_iterator it = m_unitCountSM.begin(), itEnd = m_unitCountSM.end(); it != itEnd; ++it)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->first, "UnitCountSMType");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", it->second, "UnitCountSM");
			}
		}
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iHomeAreaMaintenanceModifier);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iOtherAreaMaintenanceModifier);
		{
			uint iSize = m_myHeritage.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numHeritage");
			foreach_(const HeritageTypes eType, m_myHeritage)
			{
				WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "CvPlayer", REMAPPED_CLASS_TYPE_HERITAGE, eType, "iType");
			}
		}
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlayer", NUM_COMMERCE_TYPES, m_extraCommerce);
		WRAPPER_WRITE(wrapper, "CvPlayer", m_bHasLanguage);
		// Write Vector
		{
			uint iSize = m_commandFieldPlots.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numCommandFieldPlots");
			foreach_(const CvPlot* plotX, m_commandFieldPlots)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)plotX->getX(), "CommandFieldPlotX");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)plotX->getY(), "CommandFieldPlotY");
			}
		}
		{
			uint iSize = m_commodoreFieldPlots.size();
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", iSize, "numCommodoreFieldPlots");
			foreach_(const CvPlot* plotX, m_commodoreFieldPlots)
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)plotX->getX(), "CommodoreFieldPlotX");
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlayer", (short)plotX->getY(), "CommodoreFieldPlotY");
			}
		}
		WRAPPER_WRITE(wrapper, "CvPlayer", m_iCorporateMaintenance);
	}
	WRAPPER_WRITE_OBJECT_END(wrapper);
}

void CvPlayer::createGreatPeople(UnitTypes eGreatPersonUnit, bool bIncrementThreshold, bool bIncrementExperience, int iX, int iY)
{
	PROFILE_EXTRA_FUNC();
	CvUnit* pGreatPeopleUnit = initUnit(eGreatPersonUnit, iX, iY, NO_UNITAI, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
	if (!pGreatPeopleUnit)
	{
		FErrorMsg("error");
		return;
	}

	if (getGoldenAgeOnBirthOfGreatPersonCount(eGreatPersonUnit) > 0)
	{
		changeGoldenAgeTurns(getGoldenAgeLength());
	}

	if (bIncrementThreshold)
	{
		incrementGreatPeopleCreated();

		changeGreatPeopleThresholdModifier(GC.getDefineINT("GREAT_PEOPLE_THRESHOLD_INCREASE") * ((getGreatPeopleCreated() / 5) + 2));

		//TB Note: In previous line's formula it was /10) + 1 but when removing this next portion, we're eliminating the normal double threshold increase and keeping the team members from being penalized for their partner(s) earning of GPs.
		//for (int iI = 0; iI < MAX_PLAYERS; iI++)
		//{
		//	if (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
		//	{
		//		GET_PLAYER((PlayerTypes)iI).changeGreatPeopleThresholdModifier(GC.getDefineINT("GREAT_PEOPLE_THRESHOLD_INCREASE_TEAM") * ((getGreatPeopleCreated() / 10) + 1));
		//	}
		//}
	}

	if (bIncrementExperience)
	{
		incrementGreatGeneralsCreated();

		changeGreatGeneralsThresholdModifier(GC.getDefineINT("GREAT_GENERALS_THRESHOLD_INCREASE") * ((getGreatGeneralsCreated() / 5) + 2));

		//TB Note: In previous line's formula it was /10) + 1 but when removing this next portion, we're eliminating the normal double threshold increase and keeping the team members from being penalized for their partner(s) earning of GPs.
		//for (int iI = 0; iI < MAX_PLAYERS; iI++)
		//{
		//	if (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
		//	{
		//		GET_PLAYER((PlayerTypes)iI).changeGreatGeneralsThresholdModifier(GC.getDefineINT("GREAT_GENERALS_THRESHOLD_INCREASE_TEAM") * ((getGreatGeneralsCreated() / 10) + 1));
		//	}
		//}
	}


	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	CvCity* pCity = NULL;
	CvWString szReplayMessage;

	if (pPlot)
	{
		pCity = pPlot->getPlotCity();
		if (pCity)
		{
			CvWString szCity;
			szCity.Format(L"%s (%s)", pCity->getName().GetCString(), GET_PLAYER(pCity->getOwner()).getName());
			szReplayMessage = gDLL->getText("TXT_KEY_MISC_GP_BORN", pGreatPeopleUnit->getName().GetCString(), szCity.GetCString());
		}
		else
		{
			szReplayMessage = gDLL->getText("TXT_KEY_MISC_GP_BORN_FIELD", pGreatPeopleUnit->getName().GetCString());
		}
		GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szReplayMessage, iX, iY, (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"));
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{

			if (pPlot->isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
			{
				AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szReplayMessage, "AS2D_UNIT_GREATPEOPLE", MESSAGE_TYPE_MAJOR_EVENT, pGreatPeopleUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"), iX, iY, true, true);
			}
			else
			{
				CvWString szMessage = gDLL->getText("TXT_KEY_MISC_GP_BORN_SOMEWHERE", pGreatPeopleUnit->getName().GetCString());
				AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_UNIT_GREATPEOPLE", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"));
			}
		}
	}

	// Python Event
	if (pCity)
	{
		CvEventReporter::getInstance().greatPersonBorn(pGreatPeopleUnit, getID(), pCity);
	}
}


const EventTriggeredData* CvPlayer::getEventOccured(EventTypes eEvent, bool bIncludeExpiredEvents) const
{
	FASSERT_BOUNDS(0, GC.getNumEventInfos(), eEvent);

	CvEventMap::const_iterator it = m_mapEventsOccured.find(eEvent);

	if (it == m_mapEventsOccured.end() || ((it->second).m_bExpired && !bIncludeExpiredEvents) )
	{
		return NULL;
	}

	return &(it->second);
}

bool CvPlayer::isTriggerFired(EventTriggerTypes eEventTrigger) const
{
	return algo::any_of_equal(m_triggersFired, eEventTrigger);
}

void CvPlayer::resetEventOccured(EventTypes eEvent, bool bAnnounce)
{
	FASSERT_BOUNDS(0, GC.getNumEventInfos(), eEvent);

	CvEventMap::iterator it = m_mapEventsOccured.find(eEvent);

	if (it != m_mapEventsOccured.end())
	{
		expireEvent(it->first, it->second, bAnnounce);
		m_mapEventsOccured.erase(it);
	}
}

void CvPlayer::setEventOccured(EventTypes eEvent, const EventTriggeredData& kEventTriggered, bool bOthers)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumEventInfos(), eEvent);

	m_mapEventsOccured[eEvent] = kEventTriggered;

	if (GC.getEventInfo(eEvent).isQuest())
	{
		CvWStringBuffer szMessageBuffer;
		szMessageBuffer.append(GC.getEventInfo(eEvent).getDescription());
		GAMETEXT.setEventHelp(szMessageBuffer, eEvent, kEventTriggered.getID(), getID());
		gDLL->getInterfaceIFace()->addQuestMessage(getID(), szMessageBuffer.getCString(), kEventTriggered.getID());
	}

	if (bOthers)
	{
		if (GC.getEventInfo(eEvent).isGlobal())
		{
			for (int i = 0; i < MAX_PC_PLAYERS; i++)
			{
				if (i != getID())
				{
					GET_PLAYER((PlayerTypes)i).setEventOccured(eEvent, kEventTriggered, false);
				}
			}
		}
		else if (GC.getEventInfo(eEvent).isTeam())
		{
			for (int i = 0; i < MAX_PC_PLAYERS; i++)
			{
				if (i != getID() && getTeam() == GET_PLAYER((PlayerTypes)i).getTeam())
				{
					GET_PLAYER((PlayerTypes)i).setEventOccured(eEvent, kEventTriggered, false);
				}
			}
		}
	}
}


const EventTriggeredData* CvPlayer::getEventCountdown(EventTypes eEvent) const
{
	FASSERT_BOUNDS(0, GC.getNumEventInfos(), eEvent);

	CvEventMap::const_iterator it = m_mapEventCountdown.find(eEvent);

	if (it == m_mapEventCountdown.end())
	{
		return NULL;
	}

	return &(it->second);
}

void CvPlayer::setEventCountdown(EventTypes eEvent, const EventTriggeredData& kEventTriggered)
{
	FASSERT_BOUNDS(0, GC.getNumEventInfos(), eEvent);

	m_mapEventCountdown[eEvent] = kEventTriggered;
}

void CvPlayer::resetEventCountdown(EventTypes eEvent)
{
	FASSERT_BOUNDS(0, GC.getNumEventInfos(), eEvent);

	CvEventMap::iterator it = m_mapEventCountdown.find(eEvent);

	if (it != m_mapEventCountdown.end())
	{
		m_mapEventCountdown.erase(it);
	}
}


void CvPlayer::resetTriggerFired(EventTriggerTypes eTrigger)
{
	std::vector<EventTriggerTypes>::iterator it = std::find(m_triggersFired.begin(), m_triggersFired.end(), eTrigger);

	if (it != m_triggersFired.end())
	{
		m_triggersFired.erase(it);
	}
}

void CvPlayer::setTriggerFired(const EventTriggeredData& kTriggeredData, bool bOthers, bool bAnnounce)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumEventTriggerInfos(), kTriggeredData.m_eTrigger);

	if (kTriggeredData.m_eTrigger == NO_EVENTTRIGGER)
	{
		return;
	}
	const CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(kTriggeredData.m_eTrigger);
	const TeamTypes eTeam = getTeam();

	if (!isTriggerFired(kTriggeredData.m_eTrigger))
	{
		m_triggersFired.push_back(kTriggeredData.m_eTrigger);

		if (bOthers)
		{
			if (kTrigger.isGlobal())
			{
				for (int i = 0; i < MAX_PC_PLAYERS; i++)
				{
					if (i != getID())
					{
						GET_PLAYER((PlayerTypes)i).setTriggerFired(kTriggeredData, false, false);
					}
				}
			}
			else if (kTrigger.isTeam())
			{
				for (int i = 0; i < MAX_PC_PLAYERS; i++)
				{
					if (i != getID() && eTeam == GET_PLAYER((PlayerTypes)i).getTeam())
					{
						GET_PLAYER((PlayerTypes)i).setTriggerFired(kTriggeredData, false, false);
					}
				}
			}
		}
	}

	if (!CvString(kTrigger.getPythonCallback()).empty())
	{
		Cy::call(PYRandomEventModule, kTrigger.getPythonCallback(), Cy::Args() << &kTriggeredData);
	}

	if (bAnnounce)
	{
		CvPlot* pPlot = GC.getMap().plot(kTriggeredData.m_iPlotX, kTriggeredData.m_iPlotY);

		if (!kTriggeredData.m_szGlobalText.empty())
		{
			CvTeam& team = GET_TEAM(eTeam);

			for (int i = 0; i < MAX_PC_PLAYERS; ++i)
			{
				const CvPlayer& playerX = GET_PLAYER((PlayerTypes)i);

				if (
					playerX.isAlive()
					&&
					(
						team.isHasMet(playerX.getTeam())
						&&
						(
							NO_PLAYER == kTriggeredData.m_eOtherPlayer
							||
							team.isHasMet(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam())
						)
					)
				)
				{
					if (
						kTrigger.isShowPlot()
						&&
						(
							playerX.getTeam() == eTeam
							||
							pPlot
							&&
							pPlot->isRevealed(playerX.getTeam(), false)
							&&
							pPlot->isInViewport()
						)
					)
					{
						AddDLLMessage(
							(PlayerTypes)i, false, GC.getEVENT_MESSAGE_TIME(),
							kTriggeredData.m_szGlobalText, "AS2D_CIVIC_ADOPT",
							MESSAGE_TYPE_MINOR_EVENT, NULL, GC.getCOLOR_WHITE(),
							kTriggeredData.m_iPlotX, kTriggeredData.m_iPlotY, true, true
						);
					}
					else
					{
						AddDLLMessage(
							(PlayerTypes)i, false, GC.getEVENT_MESSAGE_TIME(),
							kTriggeredData.m_szGlobalText, "AS2D_CIVIC_ADOPT",
							MESSAGE_TYPE_MINOR_EVENT
						);
					}
				}
			}
		}
		else if (!kTriggeredData.m_szText.empty())
		{
			if (kTrigger.isShowPlot() && pPlot && pPlot->isRevealed(eTeam, false))
			{
				AddDLLMessage(
					getID(), false, GC.getEVENT_MESSAGE_TIME(),
					kTriggeredData.m_szText, "AS2D_CIVIC_ADOPT",
					MESSAGE_TYPE_MINOR_EVENT, NULL, GC.getCOLOR_WHITE(),
					kTriggeredData.m_iPlotX, kTriggeredData.m_iPlotY, true, true
				);
			}
			else
			{
				AddDLLMessage(
					getID(), false, GC.getEVENT_MESSAGE_TIME(),
					kTriggeredData.m_szText, "AS2D_CIVIC_ADOPT",
					MESSAGE_TYPE_MINOR_EVENT, NULL, GC.getCOLOR_WHITE()
				);
			}
		}
	}
}

EventTriggeredData* CvPlayer::initTriggeredData(EventTriggerTypes eEventTrigger, bool bFire, int iCityId, int iPlotX, int iPlotY, PlayerTypes eOtherPlayer, int iOtherPlayerCityId, ReligionTypes eReligion, CorporationTypes eCorporation, int iUnitId, BuildingTypes eBuilding)
{
	PROFILE_EXTRA_FUNC();

	if (NO_EVENTTRIGGER == eEventTrigger)
	{
		FErrorMsg("unexpected!");
		return NULL;
	}
	const CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eEventTrigger);

	CvCity* pCity = getCity(iCityId);
	CvCity* pOtherPlayerCity = NULL;
	if (NO_PLAYER != eOtherPlayer)
	{
		pOtherPlayerCity = GET_PLAYER(eOtherPlayer).getCity(iOtherPlayerCityId);
	}
	CvPlot* pPlot = GC.getMap().plot(iPlotX, iPlotY);
	CvUnit* pUnit = getUnit(iUnitId);

	std::vector<CvPlot*> apPlots;
	bool bPickPlot = kTrigger.getNumPlotsRequired() > 0;

	if (kTrigger.getNumOnGameOptions() > 0)
	{
		for (int iI = 0; iI < kTrigger.getNumOnGameOptions(); iI++)
		{
			if (!GC.getGame().isOption((GameOptionTypes)kTrigger.getOnGameOption(iI)))
			{
				return NULL;
			}
		}
	}

	if (kTrigger.getNumNotOnGameOptions() > 0)
	{
		for (int iI = 0; iI < kTrigger.getNumNotOnGameOptions(); iI++)
		{
			if (GC.getGame().isOption((GameOptionTypes)kTrigger.getNotOnGameOption(iI)))
			{
				return NULL;
			}
		}
	}

	if (kTrigger.isPickCity())
	{
		if (!pCity)
		{
			pCity = pickTriggerCity(eEventTrigger);

			if (!pCity)
			{
				return NULL;
			}
		}
		if (bPickPlot)
		{
			foreach_(CvPlot* pLoopPlot, pCity->plots(NUM_CITY_PLOTS, true))
			{
				if (pLoopPlot->canTrigger(eEventTrigger, getID()))
				{
					apPlots.push_back(pLoopPlot);
				}
			}
		}
	}
	else
	{
		if (kTrigger.getNumBuildings() > 0 && kTrigger.getNumBuildingsRequired() > 0)
		{
			int iFoundValid = 0;

			for (int i = 0; i < kTrigger.getNumBuildingsRequired(); ++i)
			{
				if (kTrigger.getBuildingRequired(i) != NO_BUILDING)
				{
					iFoundValid += getBuildingCount((BuildingTypes)kTrigger.getBuildingRequired(i));
				}
			}

			if (iFoundValid < kTrigger.getNumBuildings())
			{
				return NULL;
			}
		}

		if (kTrigger.getNumReligions() > 0)
		{
			int iFoundValid = 0;

			if (kTrigger.getNumReligionsRequired() > 0)
			{
				for (int i = 0; i < kTrigger.getNumReligionsRequired(); ++i)
				{
					if (kTrigger.getReligionRequired(i) != NO_RELIGION)
					{
						if (getHasReligionCount((ReligionTypes)kTrigger.getReligionRequired(i)) > 0)
						{
							++iFoundValid;
						}
					}
				}
			}
			else
			{
				for (int i = 0; i < GC.getNumReligionInfos(); ++i)
				{
					if (getHasReligionCount((ReligionTypes)i) > 0)
					{
						++iFoundValid;
					}
				}
			}

			if (iFoundValid < kTrigger.getNumReligions())
			{
				return NULL;
			}
		}

		if (kTrigger.getNumCorporations() > 0)
		{
			int iFoundValid = 0;

			if (kTrigger.getNumCorporationsRequired() > 0)
			{
				for (int i = 0; i < kTrigger.getNumCorporationsRequired(); ++i)
				{
					if (kTrigger.getCorporationRequired(i) != NO_CORPORATION)
					{
						if (getHasCorporationCount((CorporationTypes)kTrigger.getCorporationRequired(i)) > 0)
						{
							++iFoundValid;
						}
					}
				}
			}
			else
			{
				for (int i = 0; i < GC.getNumCorporationInfos(); ++i)
				{
					if (getHasCorporationCount((CorporationTypes)i) > 0)
					{
						++iFoundValid;
					}
				}
			}

			if (iFoundValid < kTrigger.getNumCorporations())
			{
				return NULL;
			}
		}

		if (kTrigger.getMinPopulation() > 0)
		{
			if (getTotalPopulation() < kTrigger.getMinPopulation())
			{
				return NULL;
			}
		}

		if (kTrigger.getMaxPopulation() > 0)
		{
			if (getTotalPopulation() > kTrigger.getMaxPopulation())
			{
				return NULL;
			}
		}

		if (bPickPlot)
		{
			for (int iPlot = 0; iPlot < GC.getMap().numPlots(); ++iPlot)
			{
				CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);

				if (pLoopPlot->canTrigger(eEventTrigger, getID()))
				{
					apPlots.push_back(pLoopPlot);
				}
			}
		}
	}

	if (!pPlot)
	{
		if (!apPlots.empty())
		{
			if (static_cast<int>(apPlots.size()) < kTrigger.getNumPlotsRequired())
			{
				return NULL;
			}
			pPlot = apPlots[GC.getGame().getSorenRandNum(apPlots.size(), "Event pick plot")];

			if (!pCity)
			{
				pCity = GC.getMap().findCity(pPlot->getX(), pPlot->getY(), getID(), NO_TEAM, false);
			}
		}
		else
		{
			if (bPickPlot)
			{
				return NULL;
			}

			if (pCity)
			{
				pPlot = pCity->plot();
			}
		}
	}

	if (!pUnit && kTrigger.getNumUnits() > 0)
	{
		pUnit = pickTriggerUnit(eEventTrigger, pPlot, kTrigger.isUnitsOnPlot());

		if (!pUnit)
		{
			return NULL;
		}
	}


	if (!pPlot && pUnit)
	{
		pPlot = pUnit->plot();
	}

	if (!pPlot && (pCity || pUnit))
	{
		return NULL;
	}

	if (kTrigger.isPickReligion())
	{
		if (NO_RELIGION == eReligion)
		{
			if (kTrigger.isStateReligion())
			{
				ReligionTypes eStateReligion = getStateReligion();
				if (NO_RELIGION != eStateReligion && isValidTriggerReligion(kTrigger, pCity, eStateReligion))
				{
					eReligion = getStateReligion();
				}
			}
			else
			{
				int iOffset = GC.getGame().getSorenRandNum(GC.getNumReligionInfos(), "Event pick religion");

				for (int i = 0; i < GC.getNumReligionInfos(); ++i)
				{
					int iReligion = (i + iOffset) % GC.getNumReligionInfos();

					if (isValidTriggerReligion(kTrigger, pCity, (ReligionTypes)iReligion))
					{
						eReligion = (ReligionTypes)iReligion;
						break;
					}
				}
			}
		}

		if (NO_RELIGION == eReligion)
		{
			return NULL;
		}
	}

	if (kTrigger.isPickCorporation())
	{
		if (NO_CORPORATION == eCorporation)
		{
			int iOffset = GC.getGame().getSorenRandNum(GC.getNumCorporationInfos(), "Event pick corporation");

			for (int i = 0; i < GC.getNumCorporationInfos(); ++i)
			{
				int iCorporation = (i + iOffset) % GC.getNumCorporationInfos();

				if (isValidTriggerCorporation(kTrigger, pCity, (CorporationTypes)iCorporation))
				{
					eCorporation = (CorporationTypes)iCorporation;
					break;
				}
			}
		}

		if (NO_CORPORATION == eCorporation)
		{
			return NULL;
		}
	}

	if (kTrigger.getNumBuildings() > 0)
	{
		if (pCity && NO_BUILDING == eBuilding)
		{
			std::vector<BuildingTypes> aeBuildings;
			for (int i = 0; i < kTrigger.getNumBuildingsRequired(); ++i)
			{
				const BuildingTypes eTestBuilding = static_cast<BuildingTypes>(kTrigger.getBuildingRequired(i));
				if (eTestBuilding != NO_BUILDING && pCity->isActiveBuilding(eTestBuilding))
				{
					aeBuildings.push_back(eTestBuilding);
				}
			}

			if (aeBuildings.empty())
			{
				return NULL;
			}
			eBuilding = aeBuildings[GC.getGame().getSorenRandNum(aeBuildings.size(), "Event pick building")];
		}
	}

	if (kTrigger.getNumUnitsGlobal() > 0)
	{
		int iNumUnits = 0;
		for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; ++iPlayer)
		{
			const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

			if (kLoopPlayer.isAlive())
			{
				iNumUnits += algo::count_if(kLoopPlayer.units(),
					CvUnit::fn::getTriggerValue(eEventTrigger, pPlot, true) != MIN_INT);
			}
		}

		if (iNumUnits < kTrigger.getNumUnitsGlobal())
		{
			return NULL;
		}
	}

	if (kTrigger.getNumBuildingsGlobal() > 0)
	{
		int iNumBuildings = 0;
		for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

			if (kLoopPlayer.isAlive())
			{
				for (int i = 0; i < kTrigger.getNumBuildingsRequired(); ++i)
				{
					if (kTrigger.getBuildingRequired(i) != NO_BUILDING)
					{
						iNumBuildings += getBuildingCount((BuildingTypes)kTrigger.getBuildingRequired(i));
					}
				}
			}
		}

		if (iNumBuildings < kTrigger.getNumBuildingsGlobal())
		{
			return NULL;
		}
	}

	if (kTrigger.isPickPlayer())
	{
		std::vector<PlayerTypes> aePlayers;
		std::vector<CvCity*> apCities;

		if (NO_PLAYER == eOtherPlayer)
		{
			for (int i = 0; i < MAX_PC_PLAYERS; i++)
			{
				if (GET_PLAYER((PlayerTypes)i).canTrigger(eEventTrigger, getID(), eReligion))
				{
					if (kTrigger.isPickOtherPlayerCity())
					{
						CvCity* pBestCity = NULL;

						if (pCity)
						{
							pBestCity = GC.getMap().findCity(pCity->getX(), pCity->getY(), (PlayerTypes)i);
						}
						else
						{
							pBestCity = GET_PLAYER((PlayerTypes)i).pickTriggerCity(eEventTrigger);
						}

						if (pBestCity)
						{
							apCities.push_back(pBestCity);
							aePlayers.push_back((PlayerTypes)i);
						}
					}
					else
					{
						apCities.push_back(NULL);
						aePlayers.push_back((PlayerTypes)i);
					}
				}
			}

			if (aePlayers.empty())
			{
				return NULL;
			}
			const int iChosen = GC.getGame().getSorenRandNum(aePlayers.size(), "Event pick player");
			eOtherPlayer = aePlayers[iChosen];
			pOtherPlayerCity = apCities[iChosen];
		}
	}

	EventTriggeredData* pTriggerData = addEventTriggered();

	pTriggerData->m_eTrigger = eEventTrigger;
	pTriggerData->m_ePlayer = getID();
	pTriggerData->m_iTurn = GC.getGame().getGameTurn();
	pTriggerData->m_iCityId = pCity ? pCity->getID() : -1;
	pTriggerData->m_iPlotX = pPlot ? pPlot->getX() : INVALID_PLOT_COORD;
	pTriggerData->m_iPlotY = pPlot ? pPlot->getY() : INVALID_PLOT_COORD;
	pTriggerData->m_eOtherPlayer = eOtherPlayer;
	pTriggerData->m_iOtherPlayerCityId = pOtherPlayerCity ? pOtherPlayerCity->getID() : -1;
	pTriggerData->m_eReligion = eReligion;
	pTriggerData->m_eCorporation = eCorporation;
	pTriggerData->m_iUnitId = pUnit ? pUnit->getID() : -1;
	pTriggerData->m_eBuilding = eBuilding;
	pTriggerData->m_bExpired = false;


	if (!CvString(kTrigger.getPythonCanDo()).empty())
	{
		if (!Cy::call<bool>(PYRandomEventModule, kTrigger.getPythonCanDo(), Cy::Args() << pTriggerData))
		{
			deleteEventTriggered(pTriggerData->getID());
			return NULL;
		}

		// python may change pTriggerData
		pCity = getCity(pTriggerData->m_iCityId);
		pPlot = GC.getMap().plot(pTriggerData->m_iPlotX, pTriggerData->m_iPlotY);
		pUnit = getUnit(pTriggerData->m_iUnitId);
		eOtherPlayer = pTriggerData->m_eOtherPlayer;
		if (NO_PLAYER != eOtherPlayer)
		{
			pOtherPlayerCity = GET_PLAYER(eOtherPlayer).getCity(pTriggerData->m_iOtherPlayerCityId);
		}
		eReligion = pTriggerData->m_eReligion;
		eCorporation = pTriggerData->m_eCorporation;
		eBuilding = pTriggerData->m_eBuilding;
	}

	std::vector<CvWString> aszTexts;
	for (int i = 0; i < kTrigger.getNumTexts(); ++i)
	{
		if (NO_ERA == kTrigger.getTextEra(i) || kTrigger.getTextEra(i) == getCurrentEra())
		{
			aszTexts.push_back(kTrigger.getText(i));
		}
	}

	if (!aszTexts.empty())
	{
		int iText = GC.getGame().getSorenRandNum(aszTexts.size(), "Event Text choice");

		pTriggerData->m_szText = gDLL->getText(aszTexts[iText].GetCString(),
			eOtherPlayer != NO_PLAYER ? GET_PLAYER(eOtherPlayer).getCivilizationAdjectiveKey() : L"",
			pCity ? pCity->getNameKey() : L"",
			pUnit ? pUnit->getNameKey() : L"",
			NO_RELIGION != eReligion ? GC.getReligionInfo(eReligion).getAdjectiveKey() : L"",
			NO_BUILDING != eBuilding ? GC.getBuildingInfo(eBuilding).getTextKeyWide() : L"",
			pOtherPlayerCity ? pOtherPlayerCity->getNameKey() : L"",
			pPlot && NO_TERRAIN != pPlot->getTerrainType() ? GC.getTerrainInfo(pPlot->getTerrainType()).getTextKeyWide() : L"",
			pPlot && NO_IMPROVEMENT != pPlot->getImprovementType() ? GC.getImprovementInfo(pPlot->getImprovementType()).getTextKeyWide() : L"",
			pPlot && NO_BONUS != pPlot->getBonusType() ? GC.getBonusInfo(pPlot->getBonusType()).getTextKeyWide() : L"",
			pPlot && NO_ROUTE != pPlot->getRouteType() ? GC.getRouteInfo(pPlot->getRouteType()).getTextKeyWide() : L"",
			NO_CORPORATION != eCorporation ? GC.getCorporationInfo(eCorporation).getTextKeyWide() : L""
			);
	}
	else
	{
		pTriggerData->m_szText = L"";
	}

	if (kTrigger.getNumWorldNews() > 0)
	{
		pTriggerData->m_szGlobalText = (
			gDLL->getText(
				kTrigger.getWorldNews(GC.getGame().getSorenRandNum(kTrigger.getNumWorldNews(), "Trigger World News choice")).GetCString(),
				getCivilizationAdjectiveKey(),
				pCity ? pCity->getNameKey() : L"",
				pTriggerData->m_eReligion != NO_RELIGION ? GC.getReligionInfo(pTriggerData->m_eReligion).getAdjectiveKey() : L"",
				eOtherPlayer != NO_PLAYER ? GET_PLAYER(eOtherPlayer).getCivilizationAdjectiveKey() : L"",
				pOtherPlayerCity ? pOtherPlayerCity->getNameKey() : L"",
				pTriggerData->m_eCorporation != NO_CORPORATION ? GC.getCorporationInfo(pTriggerData->m_eCorporation).getTextKeyWide() : L""
			)
		);
	}
	else
	{
		pTriggerData->m_szGlobalText.clear();
	}

	if (bFire)
	{
		trigger(*pTriggerData);
	}

	return pTriggerData;
}


bool CvPlayer::canDoEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const
{
	PROFILE_EXTRA_FUNC();
	if (eEvent == NO_EVENT || kTriggeredData.m_eTrigger == NO_EVENTTRIGGER)
	{
		FErrorMsg("error");
		return false;
	}

	const CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (kEvent.getPrereqGameOption() != NO_GAMEOPTION)
	{
		if (!(GC.getGame().isOption((GameOptionTypes)kEvent.getPrereqGameOption())))
		{
			return false;
		}
	}

	int iGold = std::min(getEventCost(eEvent, kTriggeredData.m_eOtherPlayer, false), getEventCost(eEvent, kTriggeredData.m_eOtherPlayer, true));

	if (iGold != 0)
	{
		if (iGold > 0 && NO_PLAYER != kTriggeredData.m_eOtherPlayer && kEvent.isGoldToPlayer())
		{
			if (GET_PLAYER(kTriggeredData.m_eOtherPlayer).getGold() < iGold)
			{
				return false;
			}
		}
		else if (iGold < 0 && getGold() < -iGold)
		{
			return false;
		}
	}

	if (0 != kEvent.getSpaceProductionModifier())
	{
		bool bValid = false;
		for (int iProject = 0; iProject < GC.getNumProjectInfos(); ++iProject)
		{
			const CvProjectInfo& kProject = GC.getProjectInfo((ProjectTypes)iProject);
			if (kProject.isSpaceship())
			{
				if (kProject.getVictoryPrereq() != NO_VICTORY)
				{
					if (GC.getGame().isVictoryValid((VictoryTypes)(kProject.getVictoryPrereq())))
					{
						bValid = true;
						break;
					}
				}
			}
		}

		if (!bValid)
		{
			return false;
		}
	}

	if (NO_PLAYER != kTriggeredData.m_eOtherPlayer)
	{
		if (kEvent.getEspionagePoints() + GET_TEAM(getTeam()).getEspionagePointsAgainstTeam(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam()) < 0)
		{
			return false;
		}
	}

	if (0 != kEvent.getTechPercent() || 0 != kEvent.getTechCostPercent())
	{
		if (NO_TECH == getBestEventTech(eEvent, kTriggeredData.m_eOtherPlayer))
		{
			return false;
		}
	}

	if (NO_TECH != kEvent.getPrereqTech())
	{
		if (!GET_TEAM(getTeam()).isHasTech(kEvent.getPrereqTech()))
		{
			return false;
		}
	}

	const BonusTypes eBonus = (BonusTypes)kEvent.getBonusGift();
	if (NO_BONUS != eBonus)
	{
		if (NO_PLAYER == kTriggeredData.m_eOtherPlayer)
		{
			return false;
		}

		if (!canTradeNetworkWith(kTriggeredData.m_eOtherPlayer))
		{
			return false;
		}

		if (GET_PLAYER(kTriggeredData.m_eOtherPlayer).getNumAvailableBonuses(eBonus) > 0)
		{
			return false;
		}

		if (getNumTradeableBonuses(eBonus) <= 1)
		{
			return false;
		}
	}

	if (kEvent.isCityEffect())
	{
		const CvCity* pCity = getCity(kTriggeredData.m_iCityId);
		if (!pCity || !pCity->canApplyEvent(eEvent, kTriggeredData))
		{
			return false;
		}
	}
	else if (kEvent.isOtherPlayerCityEffect())
	{
		if (NO_PLAYER == kTriggeredData.m_eOtherPlayer)
		{
			return false;
		}

		const CvCity* pCity = GET_PLAYER(kTriggeredData.m_eOtherPlayer).getCity(kTriggeredData.m_iOtherPlayerCityId);
		if (!pCity || !pCity->canApplyEvent(eEvent, kTriggeredData))
		{
			return false;
		}
	}

	if (kTriggeredData.m_iPlotX > -1 && kTriggeredData.m_iPlotY > -1)
	{
		const CvPlot* pPlot = GC.getMap().plot(kTriggeredData.m_iPlotX, kTriggeredData.m_iPlotY);
		if (pPlot && !pPlot->canApplyEvent(eEvent))
		{
			return false;
		}
	}

	const CvUnit* pUnit = getUnit(kTriggeredData.m_iUnitId);
	if (pUnit && !pUnit->canApplyEvent(eEvent))
	{
		return false;
	}

	if (NO_BONUS != kEvent.getBonusRevealed())
	{
		if (GET_TEAM(getTeam()).isHasTech((TechTypes)GC.getBonusInfo((BonusTypes)kEvent.getBonusRevealed()).getTechReveal()))
		{
			return false;
		}

		if (GET_TEAM(getTeam()).isForceRevealedBonus((BonusTypes)kEvent.getBonusRevealed()))
		{
			return false;
		}
	}

	if (kEvent.getConvertOwnCities() > 0)
	{
		bool bFoundValid = false;

		if (NO_RELIGION != kTriggeredData.m_eReligion)
		{
			foreach_(const CvCity* pLoopCity, cities())
			{
				if (!pLoopCity->isHasReligion(kTriggeredData.m_eReligion))
				{
					if (-1 == kEvent.getMaxNumReligions() || pLoopCity->getReligionCount() <= kEvent.getMaxNumReligions())
					{
						bFoundValid = true;
						break;
					}
				}
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (kEvent.getConvertOtherCities() > 0)
	{
		bool bFoundValid = false;

		if (NO_RELIGION != kTriggeredData.m_eReligion)
		{
			if (NO_PLAYER != kTriggeredData.m_eOtherPlayer)
			{
				foreach_(const CvCity* pLoopCity, GET_PLAYER(kTriggeredData.m_eOtherPlayer).cities())
				{
					if (!pLoopCity->isHasReligion(kTriggeredData.m_eReligion))
					{
						if (-1 == kEvent.getMaxNumReligions() || pLoopCity->getReligionCount() <= kEvent.getMaxNumReligions())
						{
							bFoundValid = true;
							break;
						}
					}
				}
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (0 != kEvent.getAttitudeModifier())
	{
		if (NO_PLAYER == kTriggeredData.m_eOtherPlayer)
		{
			return false;
		}

		if (GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam() == getTeam())
		{
			return false;
		}

		if (GET_PLAYER(kTriggeredData.m_eOtherPlayer).isHumanPlayer())
		{
			if (0 == kEvent.getOurAttitudeModifier())
			{
				return false;
			}
		}
	}

	if (0 != kEvent.getTheirEnemyAttitudeModifier())
	{
		if (NO_PLAYER == kTriggeredData.m_eOtherPlayer)
		{
			return false;
		}

		TeamTypes eWorstEnemy = GET_TEAM(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam()).AI_getWorstEnemy();
		if (NO_TEAM == eWorstEnemy || eWorstEnemy == getTeam())
		{
			return false;
		}

		if (!GET_TEAM(eWorstEnemy).isAlive())
		{
			return false;
		}

		if (eWorstEnemy == getTeam())
		{
			return false;
		}
	}

	if (kEvent.isDeclareWar())
	{
		if (NO_PLAYER == kTriggeredData.m_eOtherPlayer)
		{
			return false;
		}

		if (!GET_TEAM(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam()).canDeclareWar(getTeam()) || !GET_TEAM(getTeam()).canDeclareWar(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam()))
		{
			return false;
		}
	}

	if (kEvent.isQuest())
	{
		for (int iTrigger = 0; iTrigger < GC.getNumEventTriggerInfos(); ++iTrigger)
		{
			const CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo((EventTriggerTypes)iTrigger);
			if (!kTrigger.isRecurring())
			{
				for (int i = 0; i < kTrigger.getNumPrereqEvents(); ++i)
				{
					if (kTrigger.getPrereqEvent(i) == eEvent)
					{
						if (isTriggerFired((EventTriggerTypes)iTrigger))
						{
							return false;
						}
					}
				}
			}
		}
	}

	if (!CvString(kEvent.getPythonCanDo()).empty())
	{
		if (!Cy::call<bool>(PYRandomEventModule, kEvent.getPythonCanDo(), Cy::Args() << eEvent << &kTriggeredData))
		{
			return false;
		}
	}

	return true;
}


void CvPlayer::applyEvent(EventTypes eEvent, int iEventTriggeredId, bool bUpdateTrigger)
{
	PROFILE_FUNC();

	EventTriggeredData* pTriggeredData = NULL;

	FASSERT_BOUNDS(0, GC.getNumEventInfos(), eEvent);

	// -1 iEventTriggeredId implies a replay after a reset of modifiers and only modifier effects should be applied
	const bool adjustModifiersOnly = (iEventTriggeredId == -1);

	if ( !adjustModifiersOnly )
	{
		pTriggeredData = getEventTriggered(iEventTriggeredId);

		if (!pTriggeredData)
		{
			deleteEventTriggered(iEventTriggeredId);
			return;
		}

		if (bUpdateTrigger)
		{
			setTriggerFired(*pTriggeredData, true);
		}

		if (!canDoEvent(eEvent, *pTriggeredData))
		{
			if (bUpdateTrigger)
			{
				deleteEventTriggered(iEventTriggeredId);
			}
			return;
		}

		setEventOccured(eEvent, *pTriggeredData);
	}

	const CvEventInfo& kEvent = GC.getEventInfo(eEvent);
	CvCity* pCity =	(pTriggeredData == NULL ? NULL : getCity(pTriggeredData->m_iCityId));
	CvCity* pOtherPlayerCity = NULL;

	if (pTriggeredData && NO_PLAYER != pTriggeredData->m_eOtherPlayer)
	{
		pOtherPlayerCity = GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCity(pTriggeredData->m_iOtherPlayerCityId);
	}

	if ( !adjustModifiersOnly )
	{
		int iGold = getEventCost(eEvent, pTriggeredData->m_eOtherPlayer, false);
		int iRandomGold = getEventCost(eEvent, pTriggeredData->m_eOtherPlayer, true);

		if (iGold > 0)
			iGold += GC.getGame().getSorenRandNum(iRandomGold - iGold + 1, "Event random gold");
		else if (iGold < 0)
			iGold -= GC.getGame().getSorenRandNum(abs(iRandomGold - iGold - 1), "Event random gold");

		if (iGold != 0)
		{
			changeGold(iGold);

			if (NO_PLAYER != pTriggeredData->m_eOtherPlayer && kEvent.isGoldToPlayer())
			{
				GET_PLAYER(pTriggeredData->m_eOtherPlayer).changeGold(-iGold);
			}
		}

		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			int iValue = kEvent.getEspionagePoints();
			if (iValue != 0)
			{
				if (kEvent.isGameSpeedScale())
				{
					iValue = iValue * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 100;
				}
				GET_TEAM(getTeam()).changeEspionagePointsAgainstTeam(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam(), iValue);
			}
		}

		if (0 != kEvent.getTechPercent())
		{
			const TechTypes eBestTech = getBestEventTech(eEvent, pTriggeredData->m_eOtherPlayer);

			if (eBestTech != NO_TECH)
			{
				const int iBeakers  = GET_TEAM(getTeam()).changeResearchProgressPercent(eBestTech, kEvent.getTechPercent(), getID());

				if (iBeakers > 0)
				{
					for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getTeam()))
						{
							AddDLLMessage(
								(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(),
								gDLL->getText(
									"TXT_KEY_MISC_PROGRESS_TOWARDS_TECH",
									iBeakers, GC.getTechInfo(eBestTech).getTextKeyWide()
								),
								NULL, MESSAGE_TYPE_MINOR_EVENT, NULL,
								(ColorTypes)GC.getInfoTypeForString("COLOR_TECH_TEXT")
							);
						}
					}
				}
			}
		}

		if (kEvent.isGoldenAge())
		{
			changeGoldenAgeTurns(getGoldenAgeLength());
		}

		if (kEvent.isDeclareWar() && NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			if (gTeamLogLevel >= 2)
			{
				logBBAI("	Team %d (%S) declares war on team %d due to event", GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam(), GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCivilizationDescription(0), getTeam() );
			}
			GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).declareWar(getTeam(), false, WARPLAN_LIMITED);
		}

		if (NO_BONUS != kEvent.getBonusGift() && NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			CLinkList<TradeData> ourList;
			CLinkList<TradeData> theirList;
			TradeData kTradeData;
			setTradeItem(&kTradeData, TRADE_RESOURCES, kEvent.getBonusGift());
			ourList.insertAtEnd(kTradeData);
			GC.getGame().implementDeal(getID(), pTriggeredData->m_eOtherPlayer, &ourList, &theirList);
		}

		for (int iEvent = 0; iEvent < GC.getNumEventInfos(); ++iEvent)
		{
			if (kEvent.getClearEventChance(iEvent) > 0)
			{
				bool bClear = GC.getGame().getSorenRandNum(100, "Event Clear") < kEvent.getClearEventChance(iEvent);
				if (bClear)
				{
					if (kEvent.isGlobal())
					{
						for (int j = 0; j < MAX_PC_PLAYERS; j++)
						{
							GET_PLAYER((PlayerTypes)j).resetEventOccured((EventTypes)iEvent, j != getID());
						}
					}
					else if (kEvent.isTeam())
					{
						for (int j = 0; j < MAX_PC_PLAYERS; j++)
						{
							if (getTeam() == GET_PLAYER((PlayerTypes)j).getTeam())
							{
								GET_PLAYER((PlayerTypes)j).resetEventOccured((EventTypes)iEvent, j != getID());
							}
						}
					}
					else
					{
						resetEventOccured((EventTypes)iEvent, false);
					}

					if (pCity && kEvent.isCityEffect())
					{
						pCity->setEventOccured((EventTypes)iEvent, false);
					}
					else if (pOtherPlayerCity && kEvent.isOtherPlayerCityEffect())
					{
						pOtherPlayerCity->setEventOccured((EventTypes)iEvent, false);
					}
				}
			}
		}

		CvPlot* pPlot = GC.getMap().plot(pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY);
		if (pPlot && pTriggeredData->m_eTrigger > NO_EVENTTRIGGER)
		{
			FAssert(pPlot->canApplyEvent(eEvent));
			pPlot->applyEvent(eEvent);
		}

		CvUnit* pUnit = getUnit(pTriggeredData->m_iUnitId);
		if (pUnit)
		{
			FAssert(pUnit->canApplyEvent(eEvent));
			pUnit->applyEvent(eEvent);   // might kill the unit
		}
		if (NO_BONUS != kEvent.getBonusRevealed())
		{
			GET_TEAM(getTeam()).setForceRevealedBonus((BonusTypes)kEvent.getBonusRevealed(), true);
		}

		std::vector<CvCity*> apSpreadReligionCities;

		if (kEvent.getConvertOwnCities() > 0 && NO_RELIGION != pTriggeredData->m_eReligion)
		{
			foreach_(CvCity* pLoopCity, cities())
			{
				if (!pLoopCity->isHasReligion(pTriggeredData->m_eReligion)
				&& (-1 == kEvent.getMaxNumReligions() || pLoopCity->getReligionCount() <= kEvent.getMaxNumReligions()))
				{
					apSpreadReligionCities.push_back(pLoopCity);
				}
			}
		}

		while ((int)apSpreadReligionCities.size() > kEvent.getConvertOwnCities())
		{
			const int iChosen = GC.getGame().getSorenRandNum(apSpreadReligionCities.size(), "Even Spread Religion (own)");
			int i = 0;
			for (std::vector<CvCity*>::iterator it = apSpreadReligionCities.begin(); it != apSpreadReligionCities.end(); ++it)
			{
				if (i == iChosen)
				{
					apSpreadReligionCities.erase(it);
					break;
				}
				i++;
			}
		}

		foreach_(CvCity* city, apSpreadReligionCities)
		{
			city->setHasReligion(pTriggeredData->m_eReligion, true, true, false);
		}

		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			apSpreadReligionCities.clear();

			if (kEvent.getConvertOtherCities() > 0 && NO_RELIGION != pTriggeredData->m_eReligion)
			{
				foreach_(CvCity* pLoopCity, GET_PLAYER(pTriggeredData->m_eOtherPlayer).cities())
				{
					if (
						!pLoopCity->isHasReligion(pTriggeredData->m_eReligion)
					&&
						(
							-1 == kEvent.getMaxNumReligions()
							||
							pLoopCity->getReligionCount() <= kEvent.getMaxNumReligions()
						)
					) apSpreadReligionCities.push_back(pLoopCity);
				}
			}
			if (!apSpreadReligionCities.empty())
			{
				while (static_cast<int>(apSpreadReligionCities.size()) > kEvent.getConvertOtherCities())
				{
					const int iChosen = GC.getGame().getSorenRandNum(apSpreadReligionCities.size(), "Even Spread Religion (other)");
					int i = 0;
					for (std::vector<CvCity*>::iterator it = apSpreadReligionCities.begin(); it != apSpreadReligionCities.end(); ++it)
					{
						if (i == iChosen)
						{
							apSpreadReligionCities.erase(it);
							break;
						}
						i++;
					}
				}
				foreach_(CvCity* city, apSpreadReligionCities)
				{
					city->setHasReligion(pTriggeredData->m_eReligion, true, true, false);
				}
			}

			if (0 != kEvent.getOurAttitudeModifier())
			{
				if (kEvent.getOurAttitudeModifier() > 0)
				{
					AI_changeMemoryCount(pTriggeredData->m_eOtherPlayer, MEMORY_EVENT_GOOD_TO_US, kEvent.getOurAttitudeModifier());
				}
				else AI_changeMemoryCount(pTriggeredData->m_eOtherPlayer, MEMORY_EVENT_BAD_TO_US, -kEvent.getOurAttitudeModifier());
			}

			if (0 != kEvent.getAttitudeModifier())
			{
				if (kEvent.getAttitudeModifier() > 0)
				{
					GET_PLAYER(pTriggeredData->m_eOtherPlayer).AI_changeMemoryCount(getID(), MEMORY_EVENT_GOOD_TO_US, kEvent.getAttitudeModifier());
				}
				else GET_PLAYER(pTriggeredData->m_eOtherPlayer).AI_changeMemoryCount(getID(), MEMORY_EVENT_BAD_TO_US, -kEvent.getAttitudeModifier());
			}

			if (0 != kEvent.getTheirEnemyAttitudeModifier())
			{
				const TeamTypes eWorstEnemy = GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).AI_getWorstEnemy();
				if (NO_TEAM != eWorstEnemy)
				{
					for (int i = 0; i < MAX_PC_PLAYERS; ++i)
					{
						if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(eWorstEnemy))
						{
							if (kEvent.getTheirEnemyAttitudeModifier() > 0)
							{
								GET_PLAYER((PlayerTypes)i).AI_changeMemoryCount(getID(), MEMORY_EVENT_GOOD_TO_US, kEvent.getTheirEnemyAttitudeModifier());
								AI_changeMemoryCount((PlayerTypes)i, MEMORY_EVENT_GOOD_TO_US, kEvent.getTheirEnemyAttitudeModifier());
							}
							else
							{
								GET_PLAYER((PlayerTypes)i).AI_changeMemoryCount(getID(), MEMORY_EVENT_BAD_TO_US, -kEvent.getTheirEnemyAttitudeModifier());
								AI_changeMemoryCount((PlayerTypes)i, MEMORY_EVENT_BAD_TO_US, -kEvent.getTheirEnemyAttitudeModifier());
							}
						}
					}
				}
			}
		}

		if (pCity && kEvent.isCityEffect())
		{
			pCity->applyEvent(eEvent, pTriggeredData);
		}
		else if (pOtherPlayerCity && kEvent.isOtherPlayerCityEffect())
		{
			pOtherPlayerCity->applyEvent(eEvent, pTriggeredData);
		}

		if (!CvString(kEvent.getPythonCallback()).empty())
		{
			Cy::call(PYRandomEventModule, kEvent.getPythonCallback(), Cy::Args() << eEvent << pTriggeredData);
		}

		if (kEvent.getNumWorldNews() > 0)
		{
			int iText = GC.getGame().getSorenRandNum(kEvent.getNumWorldNews(), "Event World News choice");

			CvWString szGlobalText;

			TeamTypes eTheirWorstEnemy = NO_TEAM;
			if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
			{
				eTheirWorstEnemy = GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).AI_getWorstEnemy();
			}

			szGlobalText = gDLL->getText(kEvent.getWorldNews(iText).GetCString(),
				getCivilizationAdjectiveKey(),
				pCity ? pCity->getNameKey() : L"",
				pTriggeredData->m_eOtherPlayer != NO_PLAYER ? GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCivilizationAdjectiveKey() : L"",
				pOtherPlayerCity ? pOtherPlayerCity->getNameKey() : L"",
				NO_RELIGION != pTriggeredData->m_eReligion ? GC.getReligionInfo(pTriggeredData->m_eReligion).getAdjectiveKey() : L"",
				NO_TEAM != eTheirWorstEnemy ? GET_TEAM(eTheirWorstEnemy).getName().GetCString() : L"",
				NO_CORPORATION != pTriggeredData->m_eCorporation ? GC.getCorporationInfo(pTriggeredData->m_eCorporation).getTextKeyWide() : L""
				);

			for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; ++iPlayer)
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

				if (kLoopPlayer.isAlive())
				{
					if (GET_TEAM(kLoopPlayer.getTeam()).isHasMet(getTeam()) && (NO_PLAYER == pTriggeredData->m_eOtherPlayer || GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).isHasMet(getTeam())))
					{
						bool bShowPlot = (pTriggeredData->m_eTrigger > NO_EVENTTRIGGER && GC.getEventTriggerInfo(pTriggeredData->m_eTrigger).isShowPlot());

						if (bShowPlot)
						{
							if (kLoopPlayer.getTeam() != getTeam())
							{
								if (!pPlot || !pPlot->isRevealed(kLoopPlayer.getTeam(), false))
								{
									bShowPlot = false;
								}
							}
						}


						if (bShowPlot)
						{
							AddDLLMessage((PlayerTypes)iPlayer, false, GC.getEVENT_MESSAGE_TIME(), szGlobalText, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, GC.getCOLOR_WHITE(), pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY, true, true);
						}
						else
						{
							AddDLLMessage((PlayerTypes)iPlayer, false, GC.getEVENT_MESSAGE_TIME(), szGlobalText, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT);
						}
					}
				}
			}

	/************************************************************************************************/
	/* REVOLUTION_MOD						 02/01/08								jdog5000	  */
	/*																							  */
	/* Silence replay messages for events ... too crowded by late game							  */
	/************************************************************************************************/
	/*
			GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szGlobalText, pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY, GC.getCOLOR_HIGHLIGHT_TEXT());
	*/
	/************************************************************************************************/
	/* REVOLUTION_MOD						  END												  */
	/************************************************************************************************/
		}

		if (!CvWString(kEvent.getLocalInfoTextKey()).empty())
		{
			CvWString szLocalText;

			TeamTypes eTheirWorstEnemy = NO_TEAM;
			if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
			{
				eTheirWorstEnemy = GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).AI_getWorstEnemy();
			}

			szLocalText = gDLL->getText(kEvent.getLocalInfoTextKey(),
			getCivilizationAdjectiveKey(),
			pCity ? pCity->getNameKey() : L"",
			pTriggeredData->m_eOtherPlayer != NO_PLAYER ? GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCivilizationAdjectiveKey() : L"",
			pOtherPlayerCity ? pOtherPlayerCity->getNameKey() : L"",
			NO_RELIGION != pTriggeredData->m_eReligion ? GC.getReligionInfo(pTriggeredData->m_eReligion).getAdjectiveKey() : L"",
			NO_TEAM != eTheirWorstEnemy ? GET_TEAM(eTheirWorstEnemy).getName().GetCString() : L"",
			NO_CORPORATION != pTriggeredData->m_eCorporation ? GC.getCorporationInfo(pTriggeredData->m_eCorporation).getTextKeyWide() : L""
			);


			if (pTriggeredData->m_eTrigger > NO_EVENTTRIGGER && GC.getEventTriggerInfo(pTriggeredData->m_eTrigger).isShowPlot())
			{
				AddDLLMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szLocalText, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, GC.getCOLOR_WHITE(), pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY, true, true);
			}
			else
			{
				AddDLLMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szLocalText, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, GC.getCOLOR_WHITE());
			}
		}

		if (!CvWString(kEvent.getOtherPlayerPopup()).empty())
		{
			if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
			{
				CvWString szText = gDLL->getText(kEvent.getOtherPlayerPopup(),
					getCivilizationAdjectiveKey(),
					pCity ? pCity->getNameKey() : L"",
					pTriggeredData->m_eOtherPlayer != NO_PLAYER ? GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCivilizationAdjectiveKey() : L"",
					pOtherPlayerCity ? pOtherPlayerCity->getNameKey() : L"",
					NO_RELIGION != pTriggeredData->m_eReligion ? GC.getReligionInfo(pTriggeredData->m_eReligion).getAdjectiveKey() : L"",
					NO_CORPORATION != pTriggeredData->m_eCorporation ? GC.getCorporationInfo(pTriggeredData->m_eCorporation).getTextKeyWide() : L""
					);

				CvPopupInfo* pInfo = new CvPopupInfo();

				if (pInfo)
				{
					pInfo->setText(szText);

					GET_PLAYER(pTriggeredData->m_eOtherPlayer).addPopup(pInfo);
				}
			}
		}

		bool bDeleteTrigger = bUpdateTrigger;

		for (int iEvent = 0; iEvent < GC.getNumEventInfos(); ++iEvent)
		{
			const int iFollowupChance = kEvent.getAdditionalEventChance(iEvent);

			if (0 == kEvent.getAdditionalEventTime(iEvent))
			{
				if (iFollowupChance > 0 && canDoEvent((EventTypes)iEvent, *pTriggeredData)
				&& GC.getGame().getSorenRandNum(100, "Additional Event") < iFollowupChance)
				{
					applyEvent((EventTypes)iEvent, iEventTriggeredId, false);
				}
			}
			else if (iFollowupChance < 1 || GC.getGame().getSorenRandNum(100, "Additional Event 2") < iFollowupChance)
			{
				EventTriggeredData kTriggered = *pTriggeredData;
				kTriggered.m_iTurn =
				(
					GC.getGame().getGameTurn()
					+
					kEvent.getAdditionalEventTime((EventTypes)iEvent)
					*
					GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 100
				);
				const EventTriggeredData* pExistingTriggered = getEventCountdown((EventTypes)iEvent);

				if (pExistingTriggered)
				{
					kTriggered.m_iTurn = std::min(kTriggered.m_iTurn, pExistingTriggered->m_iTurn);
				}
				setEventCountdown((EventTypes)iEvent, kTriggered);
				bDeleteTrigger = false;
			}
		}

		if (bDeleteTrigger)
		{
			deleteEventTriggered(iEventTriggeredId);
		}
	}

	if (!kEvent.isCityEffect() && !kEvent.isOtherPlayerCityEffect())
	{
		for (int i = 0; i < NUM_COMMERCE_TYPES; ++i)
		{
			if (kEvent.getCommerceModifier(i) != 0)
			{
				changeCommerceRateModifierfromEvents((CommerceTypes)i, kEvent.getCommerceModifier(i));
			}
			if (kEvent.getNumBuildingCommerceModifiers() > 0)
			{
				for (int j = 0; j < GC.getNumBuildingInfos(); j++)
				{
					if (kEvent.getBuildingCommerceModifier((BuildingTypes)j, i) != 0)
					{
						changeBuildingCommerceModifier((BuildingTypes)j, (CommerceTypes)i, kEvent.getBuildingCommerceModifier((BuildingTypes)j, i));
					}
				}
			}
		}
		for (int i = 0; i < NUM_YIELD_TYPES; ++i)
		{
			if (kEvent.getYieldModifier(i) != 0)
			{
				changeYieldRateModifier((YieldTypes)i, kEvent.getYieldModifier(i));
			}
		}
	}

	if (0 != kEvent.getInflationModifier())
	{
		m_iInflationModifier += kEvent.getInflationModifier();
	}

	if (0 != kEvent.getSpaceProductionModifier())
	{
		changeSpaceProductionModifier(kEvent.getSpaceProductionModifier());
	}

	if (0 != kEvent.getFreeUnitSupport())
	{
		changeBaseFreeUnitUpkeepCivilian(kEvent.getFreeUnitSupport());
	}

	const CvProperties* pProp = kEvent.getPropertiesAllCities();
	if (!pProp->isEmpty())
	{
		addPropertiesAllCities(pProp);
	}

	if (!kEvent.isCityEffect() && !kEvent.isOtherPlayerCityEffect())
	{
		if (kEvent.getHappy() != 0)
		{
			changeExtraHappiness(kEvent.getHappy());
		}

		if (kEvent.getHealth() != 0)
		{
			changeExtraHealth(kEvent.getHealth());
		}

		if (kEvent.getNumBuildingYieldChanges() > 0)
		{
			for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); ++iBuilding)
			{
				for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
				{
					foreach_(CvCity* pLoopCity, cities())
					{
						pLoopCity->changeBuildingYieldChange((BuildingTypes)iBuilding, (YieldTypes)iYield, kEvent.getBuildingYieldChange(iBuilding, iYield));
					}
				}
			}
		}

		if (kEvent.getNumBuildingCommerceChanges() > 0)
		{
			for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); ++iBuilding)
			{
				for (int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; ++iCommerce)
				{
					foreach_(CvCity* pLoopCity, cities())
					{
						pLoopCity->changeBuildingCommerceChange((BuildingTypes)iBuilding, (CommerceTypes)iCommerce, kEvent.getBuildingCommerceChange(iBuilding, iCommerce));
					}
				}
			}
		}

		if (kEvent.getNumBuildingHappyChanges() > 0)
		{
			for (int i = 0; i < GC.getNumBuildingInfos(); ++i)
			{
				if (0 != kEvent.getBuildingHappyChange(i))
				{
					changeExtraBuildingHappiness((BuildingTypes)i, kEvent.getBuildingHappyChange(i));
				}
			}
		}

		if (kEvent.getNumBuildingHealthChanges() > 0)
		{
			for (int i = 0; i < GC.getNumBuildingInfos(); ++i)
			{
				if (0 != kEvent.getBuildingHealthChange(i))
				{
					changeExtraBuildingHealth((BuildingTypes)i, kEvent.getBuildingHealthChange(i));
				}
			}
		}

		if (!adjustModifiersOnly)
		{
			if (kEvent.getHurryAnger() != 0)
			{
				foreach_(CvCity* pLoopCity, cities())
				{
					pLoopCity->changeHurryAngerTimer(kEvent.getHurryAnger() * pLoopCity->flatHurryAngerLength());
				}
			}

			if (kEvent.getHappyTurns() > 0)
			{
				foreach_(CvCity* pLoopCity, cities())
				{
					pLoopCity->changeHappinessTimer(kEvent.getHappyTurns());
				}
			}

			/* Toffer - commented out, something is wrong here.

			if (kEvent.getMaxPillage() > 0)
			{
				FAssert(kEvent.getMaxPillage() >= kEvent.getMinPillage());
				int iNumPillage = kEvent.getMinPillage() + GC.getGame().getSorenRandNum(kEvent.getMaxPillage() - kEvent.getMinPillage(), "Pick number of event pillaged plots");

				int iNumPillaged = 0;
				for (int i = 0; i < iNumPillage; ++i)
				{
					const int iRandOffset = GC.getGame().getSorenRandNum(GC.getMap().numPlots(), "Pick event pillage plot (any city)");

					for (int j = 0; j < GC.getMap().numPlots(); ++j)
					{
						CvPlot* pPlot = GC.getMap().plotByIndex((j + iRandOffset) % GC.getMap().numPlots());

						// Toffer - Something is wrong here... pPlot->isCity() && pPlot->isImprovementDestructible() doesn't make much sense
						//	I think it's looking for a plot that belong to a city rather than a plot with a city on it.
						if (pPlot && pPlot->getOwner() == getID() && pPlot->isCity() && pPlot->isImprovementDestructible())
						{
							AddDLLMessage(
								getID(), false, GC.getEVENT_MESSAGE_TIME(),
								gDLL->getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", GC.getImprovementInfo(pPlot->getImprovementType()).getTextKeyWide()),
								"AS2D_PILLAGED", MESSAGE_TYPE_INFO, GC.getImprovementInfo(pPlot->getImprovementType()).getButton(), GC.getCOLOR_RED(), pPlot->getX(), pPlot->getY(), true, true
							);
							pPlot->setImprovementType(NO_IMPROVEMENT);
							++iNumPillaged;
							break;
						}
					}
				}

				if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
				{

					const CvWString szBuffer = gDLL->getText("TXT_KEY_EVENT_NUM_CITY_IMPROVEMENTS_DESTROYED", iNumPillaged, getCivilizationAdjectiveKey());
					AddDLLMessage(pTriggeredData->m_eOtherPlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGED", MESSAGE_TYPE_INFO);
				}
			}
			*/

			if (kEvent.getFood() != 0)
			{
				algo::for_each(cities(), CvCity::fn::changeFood(kEvent.getFood()));
			}

			if (kEvent.getFoodPercent() != 0)
			{
				foreach_(CvCity* pLoopCity, cities())
				{
					pLoopCity->changeFood(pLoopCity->getFood() * kEvent.getFoodPercent() / 100);
				}
			}

			if (kEvent.getPopulationChange() != 0)
			{
				foreach_(CvCity* pLoopCity, cities())
				{
					if (pLoopCity->getPopulation() + kEvent.getPopulationChange() > 0)
					{
						pLoopCity->changePopulation(kEvent.getPopulationChange());
					}
				}
			}

			if (kEvent.getCulture() != 0)
			{
				int iCulture = 100 * kEvent.getCulture();

				if (kEvent.isGameSpeedScale())
				{
					iCulture = iCulture * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 100;
				}

				foreach_(CvCity* cityX, cities())
				{
					cityX->changeCultureTimes100(cityX->getOwner(), iCulture, true, true);
				}
			}

			if (kEvent.getRevolutionIndexChange() != 0)
			{
				foreach_(CvCity* pLoopCity, cities())
				{
					if (kEvent.getRevolutionIndexChange() > 0)
					{
						pLoopCity->changeLocalRevIndex(kEvent.getRevolutionIndexChange());
					}
					else if (kEvent.getRevolutionIndexChange() < 0)
					{
						pLoopCity->changeLocalRevIndex(std::max(-(pLoopCity->getLocalRevIndex()), kEvent.getRevolutionIndexChange()));
					}
				}
			}

			if (kEvent.getFreeUnit() != NO_UNIT)
			{
				CvCity* pUnitCity = pCity;

				if (!pUnitCity)
				{
					pUnitCity = getCapitalCity();
				}

				if (pUnitCity)
				{
					for (int i = 0; i < kEvent.getNumUnits(); ++i)
					{
						initUnit((UnitTypes)kEvent.getFreeUnit(), pUnitCity->getX(), pUnitCity->getY(), NO_UNITAI, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
					}
				}
			}
		}
	}

	for (int i = 0; i < GC.getNumUnitCombatInfos(); ++i)
	{
		if (NO_PROMOTION != kEvent.getUnitCombatPromotion(i))
		{
			foreach_(CvUnit* pLoopUnit, units())
			{
				//TB SubCombat Mod Begin
				if (pLoopUnit->isHasUnitCombat((UnitCombatTypes)i))
				{
					pLoopUnit->setHasPromotion((PromotionTypes)kEvent.getUnitCombatPromotion(i), true, true);
				}
				//TB SubCombat Mod End
			}
			setFreePromotion((UnitCombatTypes)i, (PromotionTypes)kEvent.getUnitCombatPromotion(i), true);
		}
	}

	for (int i = 0; i < GC.getNumUnitInfos(); ++i)
	{
		if (NO_PROMOTION != kEvent.getUnitPromotion(i))
		{
			foreach_(CvUnit* pLoopUnit, units())
			{
				if (pLoopUnit->getUnitType() == i)
				{
					pLoopUnit->setHasPromotion((PromotionTypes)kEvent.getUnitPromotion(i), true, true);
				}
			}
			setFreePromotion((UnitTypes)i, (PromotionTypes)kEvent.getUnitPromotion(i), true);
		}
	}

}

bool CvPlayer::isValidEventTech(TechTypes eTech, EventTypes eEvent, PlayerTypes eOtherPlayer) const
{
	const CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (0 == kEvent.getTechPercent() && 0 == kEvent.getTechCostPercent())
	{
		return false;
	}

	if (kEvent.getTechPercent() < 0 && GET_TEAM(getTeam()).getResearchProgress(eTech) <= 0)
	{
		return false;
	}

	if (!canResearch(eTech))
	{
		return false;
	}

	if (getResearchTurnsLeft(eTech, true) < kEvent.getTechMinTurnsLeft())
	{
		return false;
	}

	if (NO_PLAYER != eOtherPlayer && !GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).isHasTech(eTech))
	{
		return false;
	}

	return true;
}


TechTypes CvPlayer::getBestEventTech(EventTypes eEvent, PlayerTypes eOtherPlayer) const
{
	PROFILE_EXTRA_FUNC();
	TechTypes eBestTech = NO_TECH;
	const CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (0 == kEvent.getTechPercent() && 0 == kEvent.getTechCostPercent())
	{
		return NO_TECH;
	}

	if (NO_TECH != kEvent.getTech())
	{
		eBestTech = (TechTypes)kEvent.getTech();
	}
	else
	{
		bool bFoundFlavor = false;
		for (int i = 0; i < GC.getNumFlavorTypes(); ++i)
		{
			if (0 != kEvent.getTechFlavorValue(i))
			{
				bFoundFlavor = true;
				break;
			}
		}

		if (!bFoundFlavor)
		{
			eBestTech = getCurrentResearch();
		}
	}

	if (NO_TECH != eBestTech)
	{
		if (!isValidEventTech(eBestTech, eEvent, eOtherPlayer))
		{
			eBestTech = NO_TECH;
		}
	}
	else
	{
		int iBestValue = 0;
		for (int iTech = 0; iTech < GC.getNumTechInfos(); ++iTech)
		{
			if (isValidEventTech((TechTypes)iTech, eEvent, eOtherPlayer))
			{
				int iValue = 0;
				for (int i = 0; i < GC.getNumFlavorTypes(); ++i)
				{
					iValue += kEvent.getTechFlavorValue(i) * GC.getTechInfo((TechTypes)iTech).getFlavorValue(i);
				}

				if (iValue > iBestValue)
				{
					eBestTech = (TechTypes)iTech;
					iBestValue = iValue;
				}
			}
		}
	}

	return eBestTech;
}

int CvPlayer::getEventCost(EventTypes eEvent, PlayerTypes eOtherPlayer, bool bRandom) const
{
	const CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	int iGold = kEvent.getGold();
	if (bRandom)
	{
		iGold += kEvent.getRandomGold();
	}

	if (kEvent.isGameSpeedScale())
	{
		iGold = iGold * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 100;
	}

	const TechTypes eBestTech = getBestEventTech(eEvent, eOtherPlayer);

	if (NO_TECH != eBestTech)
	{
		iGold -= kEvent.getTechCostPercent() * GET_TEAM(getTeam()).getResearchCost(eBestTech) / 100;
	}

	return iGold;
}


void CvPlayer::doEvents()
{
	PROFILE_EXTRA_FUNC();
	if (GC.getGame().isOption(GAMEOPTION_NO_EVENTS))
	{
		return;
	}

	CvEventMap::iterator it = m_mapEventsOccured.begin();
	while (it != m_mapEventsOccured.end())
	{
		if (checkExpireEvent(it->first, it->second))
		{
			expireEvent(it->first, it->second, true);
		}
		++it;
	}

	const bool bNewEventEligible =
	(
		GC.getGame().getElapsedGameTurns() > 0
		&&
		GC.getGame().getSorenRandNum(GC.getDefineINT("EVENT_PROBABILITY_ROLL_SIDES"), "Global event check") < GC.getEraInfo(getCurrentEra()).getEventChancePerTurn()
	);

	std::vector< std::pair<EventTriggeredData*, int> > aePossibleEventTriggerWeights;
	int iTotalWeight = 0;
	for (int i = 0; i < GC.getNumEventTriggerInfos(); ++i)
	{
		const int iWeight = getEventTriggerWeight((EventTriggerTypes)i);
		if (iWeight == -1)
		{
			trigger((EventTriggerTypes)i);
		}
		else if (bNewEventEligible && iWeight > 0)
		{
			EventTriggeredData* pTriggerData = initTriggeredData((EventTriggerTypes)i);
			if (pTriggerData)
			{
				iTotalWeight += iWeight;
				aePossibleEventTriggerWeights.push_back(std::make_pair(pTriggerData, iTotalWeight));
			}
		}
	}

	if (iTotalWeight > 0)
	{
		bool bFired = false;
		const int iValue = GC.getGame().getSorenRandNum(iTotalWeight, "Event trigger");
		for (std::vector< std::pair<EventTriggeredData*, int> >::iterator it = aePossibleEventTriggerWeights.begin(); it != aePossibleEventTriggerWeights.end(); ++it)
		{
			EventTriggeredData* pTriggerData = (*it).first;
			if (pTriggerData)
			{
				if (iValue < (*it).second && !bFired)
				{
					trigger(*pTriggerData);
					bFired = true;
				}
				else deleteEventTriggered(pTriggerData->getID());
			}
		}
	}

	const int iTurn = GC.getGame().getGameTurn();
	std::vector<int> aCleanup;
	for (int i = 0; i < GC.getNumEventInfos(); ++i)
	{
		const EventTriggeredData* pTriggeredData = getEventCountdown((EventTypes)i);

		if (pTriggeredData && iTurn >= pTriggeredData->m_iTurn)
		{
			applyEvent((EventTypes)i, pTriggeredData->m_iId);
			resetEventCountdown((EventTypes)i);
			aCleanup.push_back(pTriggeredData->m_iId);
		}
	}

	foreach_(const int& it, aCleanup)
	{
		bool bDelete = true;

		for (int i = 0; i < GC.getNumEventInfos(); ++i)
		{
			const EventTriggeredData* pTriggeredData = getEventCountdown((EventTypes)i);
			if (pTriggeredData && pTriggeredData->m_iId == it)
			{
				bDelete = false;
				break;
			}
		}
		if (bDelete)
		{
			deleteEventTriggered(it);
		}
	}
}


void CvPlayer::expireEvent(EventTypes eEvent, EventTriggeredData& kTriggeredData, bool bFail)
{
	PROFILE_EXTRA_FUNC();
	if (kTriggeredData.m_iTurn <= 0)
	{
		kTriggeredData.m_bExpired = true;
		return;
	}
	FAssert(getEventOccured(eEvent) == &kTriggeredData);
	FAssert(GC.getEventInfo(eEvent).isQuest() || GC.getGame().getGameTurn() - kTriggeredData.m_iTurn <= 4);

	if (GC.getEventInfo(eEvent).isQuest())
	{
		for (CvMessageQueue::iterator it = m_listGameMessages.begin(); it != m_listGameMessages.end(); ++it)
		{
			// the trigger ID is stored in the otherwise unused length field
			if ((*it).getLength() == kTriggeredData.getID())
			{
				m_listGameMessages.erase(it);
				gDLL->getInterfaceIFace()->dirtyTurnLog(getID());
				break;
			}
		}

		if (bFail)
		{
			AddDLLMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), gDLL->getText(GC.getEventInfo(eEvent).getQuestFailTextKey()), "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, GC.getCOLOR_RED());
		}
	}

	kTriggeredData.m_bExpired = true;
}

bool CvPlayer::checkExpireEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const
{
	PROFILE_EXTRA_FUNC();
	if ( !kTriggeredData.m_bExpired )
	{
		const CvEventInfo& kEvent = GC.getEventInfo(eEvent);

		if (!CvString(kEvent.getPythonExpireCheck()).empty())
		{
			FAssertMsg(kTriggeredData.m_ePlayer != NO_PLAYER, "Event player is expected to be valid on call to python exire check");
			if (Cy::call<bool>(PYRandomEventModule, kEvent.getPythonExpireCheck(), Cy::Args() << eEvent << &kTriggeredData))
			{
				return true;
			}
		}

		if (!kEvent.isQuest())
		{
			if (GC.getGame().getGameTurn() - kTriggeredData.m_iTurn > 2)
			{
				return true;
			}

			return false;
		}

		if (kTriggeredData.m_eTrigger == NO_EVENTTRIGGER)
		{
			return false;
		}

		const CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(kTriggeredData.m_eTrigger);

		FAssert(kTriggeredData.m_ePlayer != NO_PLAYER);

		const CvPlayer& kPlayer = GET_PLAYER(kTriggeredData.m_ePlayer);

		if (kTrigger.isStateReligion() && kTrigger.isPickReligion())
		{
			if (kPlayer.getStateReligion() != kTriggeredData.m_eReligion)
			{
				return true;
			}
		}

		if (NO_CIVIC != kTrigger.getCivic())
		{
			if (!kPlayer.isCivic((CivicTypes)kTrigger.getCivic()))
			{
				return true;
			}
		}

		if (kTriggeredData.m_iCityId != -1)
		{
			if (!kPlayer.getCity(kTriggeredData.m_iCityId))
			{
				return true;
			}
		}

		if (kTriggeredData.m_iUnitId != -1)
		{
			if (!kPlayer.getUnit(kTriggeredData.m_iUnitId))
			{
				return true;
			}
		}

		if (NO_PLAYER != kTriggeredData.m_eOtherPlayer)
		{
			if (!GET_PLAYER(kTriggeredData.m_eOtherPlayer).isAlive())
			{
				return true;
			}

			if (kTriggeredData.m_iOtherPlayerCityId != -1)
			{
				if (!GET_PLAYER(kTriggeredData.m_eOtherPlayer).getCity(kTriggeredData.m_iOtherPlayerCityId))
				{
					return true;
				}
			}
		}

		if (kTrigger.getNumObsoleteTechs() > 0)
		{
			for (int iI = 0; iI < kTrigger.getNumObsoleteTechs(); iI++)
			{
				if (GET_TEAM(getTeam()).isHasTech((TechTypes)(kTrigger.getObsoleteTech(iI))))
				{
					return true;
				}
			}
		}
	}

	return false;
}


void CvPlayer::trigger(EventTriggerTypes eTrigger)
{
	initTriggeredData(eTrigger, true);
}

void CvPlayer::trigger(const EventTriggeredData& kData)
{
	if (isHumanPlayer())
	{
		CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_EVENT, kData.getID());
		addPopup(pInfo);
	}
	else
	{
		EventTypes eEvent = AI_chooseEvent(kData.getID());
		if (NO_EVENT != eEvent)
		{
			applyEvent(eEvent, kData.getID());
		}
	}
}

bool CvPlayer::canTrigger(EventTriggerTypes eTrigger, PlayerTypes ePlayer, ReligionTypes eReligion) const
{
	PROFILE_EXTRA_FUNC();
	if (!isAlive())
	{
		return false;
	}

	if (getID() == ePlayer)
	{
		return false;
	}

	const CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	const CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eTrigger);

	if (getTeam() == kPlayer.getTeam())
	{
		return false;
	}

	if (!kTrigger.isPickPlayer())
	{
		return false;
	}

	if (!GET_TEAM(getTeam()).isHasMet(kPlayer.getTeam()))
	{
		return false;
	}

	if (isHumanPlayer() && kTrigger.isOtherPlayerAI())
	{
		return false;
	}

	if (GET_TEAM(getTeam()).isAtWar(kPlayer.getTeam()) != kTrigger.isOtherPlayerWar())
	{
		return false;
	}

	if (NO_TECH != kTrigger.getOtherPlayerHasTech())
	{
		if (!GET_TEAM(getTeam()).isHasTech((TechTypes)kTrigger.getOtherPlayerHasTech()))
		{
			return false;
		}
	}

	if (kTrigger.getOtherPlayerShareBorders() > 0)
	{
		int iCount = 0;

		for (int iI = 0; iI < GC.getMap().numPlots(); ++iI)
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

			if (!pLoopPlot->isWater())
			{
				if ((pLoopPlot->getOwner() == getID()) && pLoopPlot->isAdjacentPlayer(ePlayer, true))
				{
					++iCount;
				}
			}
		}

		if (iCount < kTrigger.getOtherPlayerShareBorders())
		{
			return false;
		}
	}

	if (NO_RELIGION != eReligion)
	{
		bool bHasReligion = kTrigger.isStateReligion() ? (getStateReligion() == eReligion) : (getHasReligionCount(eReligion) > 0);

		if (kTrigger.isOtherPlayerHasReligion())
		{
			if (!bHasReligion)
			{
				return false;
			}
		}

		if (kTrigger.isOtherPlayerHasOtherReligion())
		{
			if (bHasReligion)
			{
				return false;
			}

			if (kTrigger.isStateReligion() && getStateReligion() == NO_RELIGION)
			{
				return false;
			}

		}
	}

	return true;
}

CvCity* CvPlayer::pickTriggerCity(EventTriggerTypes eTrigger) const
{
	PROFILE_EXTRA_FUNC();

	std::vector<CvCity*> apCities;
	int iBestValue = MIN_INT;

	foreach_(CvCity* pLoopCity, cities())
	{
		const int iValue = pLoopCity->getTriggerValue(eTrigger);

		if (iValue >= iBestValue && iValue != MIN_INT)
		{
			if (iValue > iBestValue)
			{
				apCities.clear();
				iBestValue = iValue;
			}

			apCities.push_back(pLoopCity);
		}
	}

	if (!apCities.empty())
	{
		return apCities[GC.getGame().getSorenRandNum(apCities.size(), "Event pick city")];
	}
	return NULL;
}

CvUnit* CvPlayer::pickTriggerUnit(EventTriggerTypes eTrigger, const CvPlot* pPlot, bool bCheckPlot) const
{
	PROFILE_EXTRA_FUNC();

	std::vector<CvUnit*> apUnits;
	int iBestValue = MIN_INT;

	foreach_(CvUnit* pLoopUnit, units())
	{
		const int iValue = pLoopUnit->getTriggerValue(eTrigger, pPlot, bCheckPlot);

		if (iValue >= iBestValue && iValue != MIN_INT)
		{
			if (iValue > iBestValue)
			{
				apUnits.clear();
				iBestValue = iValue;
			}
			apUnits.push_back(pLoopUnit);
		}
	}

	if (!apUnits.empty())
	{
		return apUnits[GC.getGame().getSorenRandNum(apUnits.size(), "Event pick unit")];
	}
	return NULL;
}

bool CvPlayer::isEventTriggerPossible(EventTriggerTypes eTrigger, bool bIgnoreActive) const
{
	PROFILE_EXTRA_FUNC();
	const CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eTrigger);

	if (kTrigger.getProbability() == 0)
	{
		return false;
	}

	if (NO_HANDICAP != kTrigger.getMinDifficulty() && GC.getGame().getHandicapType() < kTrigger.getMinDifficulty())
	{
		return false;
	}

	if (kTrigger.isSinglePlayer() && GC.getGame().isGameMultiPlayer())
	{
		return false;
	}

	if (!bIgnoreActive && !GC.getGame().isEventActive(eTrigger))
	{
		return false;
	}

	if (kTrigger.getNumObsoleteTechs() > 0)
	{
		for (int iI = 0; iI < kTrigger.getNumObsoleteTechs(); iI++)
		{
			if (GET_TEAM(getTeam()).isHasTech((TechTypes)(kTrigger.getObsoleteTech(iI))))
			{
				return false;
			}
		}
	}

	if (!kTrigger.isRecurring() && isTriggerFired(eTrigger))
	{
		return false;
	}

	if (kTrigger.getNumPrereqOrTechs() > 0)
	{
		bool bFoundValid = false;

		for (int iI = 0; iI < kTrigger.getNumPrereqOrTechs(); iI++)
		{
			if (GET_TEAM(getTeam()).isHasTech((TechTypes)(kTrigger.getPrereqOrTechs(iI))))
			{
				bFoundValid = true;
				break;
			}
		}
		if (!bFoundValid)
		{
			return false;
		}
	}


	if (kTrigger.getNumPrereqAndTechs() > 0)
	{
		for (int iI = 0; iI < kTrigger.getNumPrereqAndTechs(); iI++)
		{
			if (!GET_TEAM(getTeam()).isHasTech((TechTypes)(kTrigger.getPrereqAndTechs(iI))))
			{
				return false;
			}
		}
	}

	if (kTrigger.getNumPrereqEvents() > 0)
	{
		for (int iI = 0; iI < kTrigger.getNumPrereqEvents(); iI++)
		{
			if (!getEventOccured((EventTypes)kTrigger.getPrereqEvent(iI)))
			{
				return false;
			}
		}
	}

	if (NO_CIVIC != kTrigger.getCivic())
	{
		bool bFoundValid = false;

		for (int iI = 0; iI < GC.getNumCivicOptionInfos(); ++iI)
		{
			if (getCivics((CivicOptionTypes)iI) == kTrigger.getCivic())
			{
				bFoundValid = true;
				break;
			}
		}
		if (!bFoundValid)
		{
			return false;
		}
	}

	if (kTrigger.getMinTreasury() > 0 && getGold() < kTrigger.getMinTreasury())
	{
		return false;
	}

	if (!((*getPropertiesConst()) >= *kTrigger.getPrereqPlayerMinProperties()))
	{
		return false;
	}

	if (!((*getPropertiesConst()) <= *kTrigger.getPrereqPlayerMaxProperties()))
	{
		return false;
	}

	if (GC.getMap().getNumLandAreas() < kTrigger.getMinMapLandmass())
	{
		return false;
	}

	if (kTrigger.getMinOurLandmass() > 0 || kTrigger.getMaxOurLandmass() != -1)
	{
		const int iNumLandmass = algo::count_if(GC.getMap().areas(),
			!CvArea::fn::isWater() && CvArea::fn::getCitiesPerPlayer(getID()) > 0
		);

		if (iNumLandmass < kTrigger.getMinOurLandmass())
		{
			return false;
		}

		if (kTrigger.getMaxOurLandmass() != -1 && iNumLandmass > kTrigger.getMaxOurLandmass())
		{
			return false;
		}
	}
	return true;
}

int CvPlayer::getEventTriggerWeight(EventTriggerTypes eTrigger) const
{
	PROFILE_EXTRA_FUNC();

	if (!isEventTriggerPossible(eTrigger))
	{
		return 0;
	}
	const CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eTrigger);

	int iProbability = kTrigger.getProbability();
	if (iProbability < 0) return -1;

	if (kTrigger.isProbabilityUnitMultiply() && kTrigger.getNumUnits() > 0)
	{
		const int iNumUnits = algo::count_if(units(), CvUnit::fn::getTriggerValue(eTrigger, NULL, true) != MIN_INT);
		iProbability *= iNumUnits;
	}

	if (kTrigger.isProbabilityBuildingMultiply() && kTrigger.getNumBuildings() > 0)
	{
		int iNumBuildings = 0;
		for (int i = 0; i < kTrigger.getNumBuildingsRequired(); ++i)
		{
			if (kTrigger.getBuildingRequired(i) != NO_BUILDING)
			{
				iNumBuildings += getBuildingCount((BuildingTypes)kTrigger.getBuildingRequired(i));
			}
		}
		iProbability *= iNumBuildings;
	}
	return iProbability;
}


PlayerTypes CvPlayer::getSplitEmpirePlayer(int iAreaId) const
{
	PROFILE_EXTRA_FUNC();
	// can't create different derivative civs on the same continent
	for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; ++iPlayer)
	{
		const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.isAlive() && kLoopPlayer.getParent() == getID())
		{
			const CvCity* pLoopCapital = kLoopPlayer.getCapitalCity();
			if (pLoopCapital != nullptr && pLoopCapital->area()->getID() == iAreaId)
			{
				return NO_PLAYER;
			}
		}
	}

	PlayerTypes eNewPlayer = NO_PLAYER;

	// Try to find a player who's never been in the game before
	for (int i = 0; i < MAX_PC_PLAYERS; ++i)
	{
		if (!GET_PLAYER((PlayerTypes)i).isEverAlive())
		{
			eNewPlayer = (PlayerTypes)i;
			break;
		}
	}

	if (eNewPlayer == NO_PLAYER)
	{
		// Try to recycle a dead player
		for (int i = 0; i < MAX_PC_PLAYERS; ++i)
		{
			if (!GET_PLAYER((PlayerTypes)i).isAlive())
			{
				eNewPlayer = (PlayerTypes)i;
				break;
			}
		}
	}

	return eNewPlayer;
}

bool CvPlayer::canSplitEmpire() const
{
	PROFILE_EXTRA_FUNC();
	if (isMinorCiv() || GC.getGame().isOption(GAMEOPTION_NO_VASSAL_STATES))
	{
		return false;
	}

	if (GET_TEAM(getTeam()).isAVassal())
	{
		return false;
	}

	CivLeaderArray aLeaders;
	if (!getSplitEmpireLeaders(aLeaders))
	{
		return false;
	}

	foreach_(const CvArea* pLoopArea, GC.getMap().areas())
	{
		if (canSplitArea(pLoopArea->getID()))
		{
			return true;
		}
	}
	return false;
}

bool CvPlayer::canSplitArea(int iAreaId) const
{
	const CvCity* pCapital = getCapitalCity();
	if (!pCapital)
	{
		return false;
	}

	const CvArea* pArea = GC.getMap().getArea(iAreaId);
	if (!pArea || pArea == pCapital->area())
	{
		return false;
	}

	if (0 == pArea->getCitiesPerPlayer(getID()))
	{
		return false;
	}

	const PlayerTypes ePlayer = getSplitEmpirePlayer(pArea->getID());
	if (NO_PLAYER == ePlayer)
	{
		return false;
	}

	if (!GET_PLAYER(ePlayer).isAlive() && pArea->getCitiesPerPlayer(getID()) <= 1)
	{
		return false;
	}

	return true;
}

bool CvPlayer::getSplitEmpireLeaders(CivLeaderArray& aLeaders) const
{
	PROFILE_EXTRA_FUNC();
	aLeaders.clear();

	for (int i = 0; i < GC.getNumCivilizationInfos(); ++i)
	{

		if (getCivilizationType() == i)
		{
			continue;
		}

		if (!GC.getCivilizationInfo((CivilizationTypes)i).isPlayable() || !GC.getCivilizationInfo((CivilizationTypes)i).isAIPlayable())
		{
			continue;
		}

		bool bValid = true;
		for (int j = 0; j < MAX_PC_PLAYERS; ++j)
		{
			if (getID() != j && GET_PLAYER((PlayerTypes)j).isEverAlive() && GET_PLAYER((PlayerTypes)j).getCivilizationType() == i)
			{
				bValid = false;
				break;
			}
		}
		if (bValid)
		{
			for (int j = 0; j < GC.getNumLeaderHeadInfos(); ++j)
			{
				if (!GC.getCivilizationInfo((CivilizationTypes)i).isLeaders(j) && !GC.getGame().isOption(GAMEOPTION_LEADER_UNRESTRICTED))
				{
					continue;
				}

				bValid = true;
				for (int k = 0; k < MAX_PC_PLAYERS; ++k)
				{
					if (GET_PLAYER((PlayerTypes)k).isEverAlive() && GET_PLAYER((PlayerTypes)k).getPersonalityType() == j)
					{
						bValid = false;
						break;
					}
				}
				if (bValid)
				{
					aLeaders.push_back(std::make_pair((CivilizationTypes)i, (LeaderHeadTypes)j));
				}
			}
		}
	}

	return (!aLeaders.empty());
}

bool CvPlayer::splitEmpire(int iAreaId)
{
	PROFILE_FUNC();

	if (!canSplitEmpire())
	{
		return false;
	}

	if (!canSplitArea(iAreaId))
	{
		return false;
	}

	const CvArea* pArea = GC.getMap().getArea(iAreaId);
	if (!pArea)
	{
		return false;
	}

	const PlayerTypes eNewPlayer = getSplitEmpirePlayer(iAreaId);
	if (eNewPlayer == NO_PLAYER)
	{
		return false;
	}

	const bool bPlayerExists = GET_TEAM(GET_PLAYER(eNewPlayer).getTeam()).isAlive();
	FAssertMsg(!bPlayerExists, "New player already exists");
	if (!bPlayerExists)
	{
		int iBestValue = -1;
		LeaderHeadTypes eBestLeader = NO_LEADER;
		CivilizationTypes eBestCiv = NO_CIVILIZATION;

		CivLeaderArray aLeaders;
		if (getSplitEmpireLeaders(aLeaders))
		{
			for (CivLeaderArray::const_iterator it = aLeaders.begin(); it != aLeaders.end(); ++it)
			{
				int iValue = (1 + GC.getGame().getSorenRandNum(100, "Choosing Split Personality"));

				if (GC.getCivilizationInfo(getCivilizationType()).getDerivativeCiv() == it->first)
				{
					iValue += 1000;
				}

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestLeader = it->second;
					eBestCiv = it->first;
				}
			}
		}

		if (eBestLeader == NO_LEADER || eBestCiv == NO_CIVILIZATION)
		{
			return false;
		}

		CvWString szMessage = gDLL->getText("TXT_KEY_MISC_EMPIRE_SPLIT", getNameKey(), GC.getCivilizationInfo(eBestCiv).getShortDescriptionKey(), GC.getLeaderHeadInfo(eBestLeader).getTextKeyWide());
		for (int i = 0; i < MAX_PC_PLAYERS; ++i)
		{
			if (GET_PLAYER((PlayerTypes)i).isAlive())
			{
				if (i == getID() || i == eNewPlayer || GET_TEAM(GET_PLAYER((PlayerTypes)i).getTeam()).isHasMet(getTeam()))
				{

					AddDLLMessage((PlayerTypes)i, false, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_REVOLTEND", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_CITY_BAR_CAPITAL_TEXTURE")->getPath());
				}
			}
		}
		GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szMessage, -1, -1, GC.getCOLOR_HIGHLIGHT_TEXT());

		GC.getGame().addPlayer(eNewPlayer, eBestLeader, eBestCiv);
		GET_PLAYER(eNewPlayer).setParent(getID());
		GC.getInitCore().setLeaderName(eNewPlayer, GC.getLeaderHeadInfo(eBestLeader).getTextKeyWide());

		CvTeam& kNewTeam = GET_TEAM(GET_PLAYER(eNewPlayer).getTeam());
		for (int iTeam = 0; iTeam < MAX_TEAMS; ++iTeam)
		{
			CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iTeam);

			if (kLoopTeam.isAlive())
			{
				kNewTeam.setEspionagePointsAgainstTeam((TeamTypes)iTeam, GET_TEAM(getTeam()).getEspionagePointsAgainstTeam((TeamTypes)iTeam));
				kLoopTeam.setEspionagePointsAgainstTeam(GET_PLAYER(eNewPlayer).getTeam(), kLoopTeam.getEspionagePointsAgainstTeam(getTeam()));
			}
		}
		kNewTeam.setEspionagePointsEver(GET_TEAM(getTeam()).getEspionagePointsEver());

		GET_TEAM(getTeam()).assignVassal(GET_PLAYER(eNewPlayer).getTeam(), false);

		AI_updateBonusValue();
	}

	std::vector< std::pair<int, int> > aCultures;
	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); ++iPlot)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);

		bool bTranferPlot = pLoopPlot->area() == pArea;

		if (!bTranferPlot)
		{
			const CvCity* pWorkingCity = pLoopPlot->getWorkingCity();
			if (pWorkingCity && pWorkingCity->getOwner() == getID() && pWorkingCity->area() == pArea)
			{
				bTranferPlot = true;
			}
		}

		if (!bTranferPlot && pLoopPlot->isWater() && pLoopPlot->isAdjacentToArea(pArea))
		{
			bTranferPlot = true;
		}

		if (bTranferPlot)
		{
			int iCulture = pLoopPlot->getCulture(getID());

			if (bPlayerExists)
			{
				iCulture = std::max(iCulture, pLoopPlot->getCulture(eNewPlayer));
			}

			aCultures.push_back(std::make_pair(iPlot, iCulture));
		}

		if (pLoopPlot->isRevealed(getTeam(), false))
		{
			pLoopPlot->setRevealed(GET_PLAYER(eNewPlayer).getTeam(), true, false, getTeam(), false);
		}
	}

	foreach_(CvCity* pLoopCity, cities())
	{
		if (pLoopCity->area() == pArea)
		{
			const int iCulture = pLoopCity->getCultureTimes100(getID());
			const CvPlot* pPlot = pLoopCity->plot();

			GET_PLAYER(eNewPlayer).acquireCity(pLoopCity, false, false, false);

			if (pPlot)
			{
				CvCity* pCity = pPlot->getPlotCity();
				if (pCity)
				{
					pCity->setCultureTimes100(eNewPlayer, iCulture, false, false);
				}
			}
		}
	}

	for (uint i = 0; i < aCultures.size(); i++)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(aCultures[i].first);
		pPlot->setCulture(eNewPlayer, aCultures[i].second, true, false);
		pPlot->setCulture(getID(), 0, true, false);

		for (int iTeam = 0; iTeam < MAX_TEAMS; ++iTeam)
		{
			if (pPlot->getRevealedOwner((TeamTypes)iTeam, false) == getID())
			{
				pPlot->setRevealedOwner((TeamTypes)iTeam, eNewPlayer);
			}
		}
	}

	for (int i = 0; i < GC.getNumTechInfos(); ++i)
	{
		if (GET_TEAM(getTeam()).isHasTech((TechTypes)i))
		{
			CvTeam& kNewTeam = GET_TEAM(GET_PLAYER(eNewPlayer).getTeam());

			kNewTeam.setHasTech((TechTypes)i, true, eNewPlayer, false, false);
			if (GET_TEAM(getTeam()).isNoTradeTech((TechTypes)i) || GC.getGame().isOption(GAMEOPTION_NO_TECH_BROKERING))
			{
				kNewTeam.setNoTradeTech((TechTypes)i, true);
			}
		}
	}

	//	Don't initialise concripts until after the techs are set up else you'll get stone throwers!
	foreach_(CvCity* pLoopCity, GET_PLAYER(eNewPlayer).cities())
	{
		for (int i = 0; i < GC.getDefineINT("COLONY_NUM_FREE_DEFENDERS"); ++i)
		{
			pLoopCity->initConscriptedUnit();
		}
	}


	GC.getGame().updatePlotGroups();

	return true;
}


// Add ePlayer's cities, units, techs, etc to this player
bool CvPlayer::assimilatePlayer(PlayerTypes ePlayer)
{
	PROFILE_EXTRA_FUNC();
	if (getCapitalCity() == NULL) return false;

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	if (!kPlayer.isAlive()) return false;

	CvTeam& kTeam = GET_TEAM(kPlayer.getTeam());

	if (kTeam.isAtWar(getTeam()))
	{
		kTeam.makePeace(getTeam(),false);
	}

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (kPlayer.canContact((PlayerTypes)iI))
		{

			const CvWString szBuffer = gDLL->getText("TXT_KEY_REV_MESS_ASSIMILATE", kPlayer.getCivilizationDescription(), getCivilizationDescription());
			AddDLLMessage((PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_HIGHLIGHT_TEXT());
		}
	}

	// Convert units
	foreach_(CvUnit* pLoopUnit, kPlayer.units())
	{
		CvUnit* pNewUnit = initUnit(pLoopUnit->getUnitType(), pLoopUnit->getX(), pLoopUnit->getY(), NO_UNITAI, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
		if (pNewUnit == NULL)
		{
			FErrorMsg("CvPlayer::initUnit returned NULL");
			break;
		}
		pNewUnit->convert(pLoopUnit);
	}

	// Convert cities
	foreach_(CvCity* pLoopCity, kPlayer.cities())
	{
		CvPlot* pPlot = pLoopCity->plot();
		pPlot->setOwner(getID(), false, false);
		pLoopCity = pPlot->getPlotCity();

		pLoopCity->changeCultureTimes100(getID(), pLoopCity->getCultureTimes100(kPlayer.getID())/2, true, false);
		pPlot->changeCulture(getID(), pPlot->getCulture(kPlayer.getID())/2, false);
	}

	// Add Techs
	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		if( kTeam.isHasTech((TechTypes)iI) && !(GET_TEAM(getTeam()).isHasTech((TechTypes)iI)) )
		{
			GET_TEAM(getTeam()).setHasTech((TechTypes)iI, true, getID(), false, false);
		}
	}

	// Share map
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot->isRevealed(kTeam.getID(), false))
		{
			pLoopPlot->setRevealed(getTeam(), true, false, kTeam.getID(), false);
#ifdef ENABLE_FOGWAR_DECAY
			pLoopPlot->InitFogDecay(false);
#endif
		}
	}

	// Diplomacy
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (iI != getTeam() && iI != kTeam.getID()
		&& GET_TEAM((TeamTypes)iI).isAlive()
		&& kTeam.isHasMet((TeamTypes)iI))
		{
			GET_TEAM(getTeam()).meet(((TeamTypes)iI), false);
		}
	}

	// Make sure assimilated player is dead
	kPlayer.killUnits();
	kPlayer.killCities();

	for (int i = 0; i < GC.getMap().numPlots(); ++i)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(i);
		if (pLoopPlot && (pLoopPlot->getTeam() == getTeam() || pLoopPlot->getTeam() == kTeam.getID()))
		{
			pLoopPlot->updateCulture(true, false);
		}
	}

	GC.getGame().updatePlotGroups();
	GC.getMap().verifyUnitValidPlot();

	GET_TEAM(getTeam()).AI_updateWorstEnemy();
	GET_TEAM(getTeam()).AI_updateAreaStragies();

	GC.getGame().updateScore(true);

	return true;
}


bool CvPlayer::isValidTriggerReligion(const CvEventTriggerInfo& kTrigger, const CvCity* pCity, ReligionTypes eReligion) const
{
	PROFILE_EXTRA_FUNC();
	if (kTrigger.getNumReligionsRequired() > 0)
	{
		bool bFound = false;

		for (int i = 0; i < kTrigger.getNumReligionsRequired(); ++i)
		{
			if (eReligion == kTrigger.getReligionRequired(i))
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

	if (pCity)
	{
		if (!pCity->isHasReligion(eReligion))
		{
			return false;
		}

		if (kTrigger.isHolyCity() && !pCity->isHolyCity(eReligion))
		{
			return false;
		}
	}
	else
	{
		if (0 == getHasReligionCount(eReligion))
		{
			return false;
		}

		if (kTrigger.isHolyCity())
		{
			const CvCity* pHolyCity = GC.getGame().getHolyCity(eReligion);
			if (!pHolyCity || pHolyCity->getOwner() != getID())
			{
				return false;
			}
		}
	}
	return true;
}

bool CvPlayer::isValidTriggerCorporation(const CvEventTriggerInfo& kTrigger, const CvCity* pCity, CorporationTypes eCorporation) const
{
	PROFILE_EXTRA_FUNC();
	if (kTrigger.getNumCorporationsRequired() > 0)
	{
		bool bFound = false;

		for (int i = 0; i < kTrigger.getNumCorporationsRequired(); ++i)
		{
			if (eCorporation == kTrigger.getCorporationRequired(i))
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

	if (pCity)
	{
		if (!pCity->isHasCorporation(eCorporation))
		{
			return false;
		}

		if (kTrigger.isHeadquarters() && !pCity->isHeadquarters(eCorporation))
		{
			return false;
		}
	}
	else
	{
		if (getHasCorporationCount(eCorporation) > 0)
		{
			return true;
		}

		if (kTrigger.isHeadquarters())
		{
			const CvCity* pHeadquarters = GC.getGame().getHeadquarters(eCorporation);
			if (!pHeadquarters || pHeadquarters->getOwner() != getID())
			{
				return false;
			}
		}
	}

	return false;
}

void CvPlayer::launch(VictoryTypes eVictory)
{
	PROFILE_EXTRA_FUNC();
	CvTeam& kTeam = GET_TEAM(getTeam());

	if (!kTeam.canLaunch(eVictory))
	{
		return;
	}

	kTeam.finalizeProjectArtTypes();
	kTeam.setVictoryCountdown(eVictory, kTeam.getVictoryDelay(eVictory));

	gDLL->getEngineIFace()->AddLaunch(getID());

	kTeam.setCanLaunch(eVictory, false);

	const CvCity *capital = getCapitalCity();

	//message
	CvWString szBuffer;
	for(int i = 0; i < MAX_PLAYERS; ++i)
	{
		if (GET_PLAYER((PlayerTypes)i).isAlive())
		{
			int plotX = -1;
			int plotY = -1;
			if((capital) && capital->isRevealed(GET_PLAYER((PlayerTypes) i).getTeam(), false))
			{
				plotX = capital->getX();
				plotY = capital->getY();
			}

			if (GET_PLAYER((PlayerTypes)i).getTeam() == getTeam())
			{
				szBuffer = gDLL->getText("TXT_KEY_VICTORY_YOU_HAVE_LAUNCHED");
			}
			else
			{
				szBuffer = gDLL->getText("TXT_KEY_VICTORY_TEAM_HAS_LAUNCHED", GET_TEAM(getTeam()).getName().GetCString());
			}


			AddDLLMessage(((PlayerTypes)i), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTURELEVEL", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getMiscArtInfo("SPACE_SHIP_BUTTON")->getPath(), GC.getCOLOR_HIGHLIGHT_TEXT(), plotX, plotY, true, true);
		}
	}
}

bool CvPlayer::isFreePromotion(UnitCombatTypes eUnitCombat, PromotionTypes ePromotion) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatPromotionArray::const_iterator it = m_aFreeUnitCombatPromotions.begin(); it != m_aFreeUnitCombatPromotions.end(); ++it)
	{
		if ((*it).first == eUnitCombat && (*it).second == ePromotion)
		{
			return true;
		}
	}

	return false;
}

void CvPlayer::setFreePromotion(UnitCombatTypes eUnitCombat, PromotionTypes ePromotion, bool bFree)
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatPromotionArray::iterator it = m_aFreeUnitCombatPromotions.begin(); it != m_aFreeUnitCombatPromotions.end(); ++it)
	{
		if ((*it).first == eUnitCombat && (*it).second == ePromotion)
		{
			if (!bFree)
			{
				m_aFreeUnitCombatPromotions.erase(it);
			}
			return;
		}
	}

	if (bFree)
	{
		m_aFreeUnitCombatPromotions.push_back(std::make_pair(eUnitCombat, ePromotion));
	}
}

bool CvPlayer::isFreePromotion(UnitTypes eUnit, PromotionTypes ePromotion) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitPromotionArray::const_iterator it = m_aFreeUnitPromotions.begin(); it != m_aFreeUnitPromotions.end(); ++it)
	{
		if ((*it).first == eUnit && (*it).second == ePromotion)
		{
			return true;
		}
	}
	return false;
}

void CvPlayer::setFreePromotion(UnitTypes eUnit, PromotionTypes ePromotion, bool bFree)
{
	PROFILE_EXTRA_FUNC();
	for (UnitPromotionArray::iterator it = m_aFreeUnitPromotions.begin(); it != m_aFreeUnitPromotions.end(); ++it)
	{
		if ((*it).first == eUnit && (*it).second == ePromotion)
		{
			if (!bFree)
			{
				m_aFreeUnitPromotions.erase(it);
			}
			return;
		}
	}
	if (bFree)
	{
		m_aFreeUnitPromotions.push_back(std::make_pair(eUnit, ePromotion));
	}
}

PlayerVoteTypes CvPlayer::getVote(int iId) const
{
	PROFILE_EXTRA_FUNC();
	for (std::vector< std::pair<int, PlayerVoteTypes> >::const_iterator it = m_aVote.begin(); it != m_aVote.end(); ++it)
	{
		if ((*it).first == iId)
		{
			return ((*it).second);
		}
	}

	return NO_PLAYER_VOTE;
}

void CvPlayer::setVote(int iId, PlayerVoteTypes ePlayerVote)
{
	PROFILE_EXTRA_FUNC();
	for (std::vector< std::pair<int, PlayerVoteTypes> >::iterator it = m_aVote.begin(); it != m_aVote.end(); ++it)
	{
		if ((*it).first == iId)
		{
			if (ePlayerVote == NO_PLAYER_VOTE)
			{
				m_aVote.erase(it);
			}
			else
			{
				(*it).second = ePlayerVote;
			}
			return;
		}
	}

	if (ePlayerVote != NO_PLAYER_VOTE)
	{
		m_aVote.push_back(std::make_pair(iId, ePlayerVote));
	}
}

int CvPlayer::getUnitExtraCost(UnitTypes eUnit) const
{
	PROFILE_EXTRA_FUNC();
	for (std::vector< std::pair<UnitTypes, int> >::const_iterator it = m_aUnitExtraCosts.begin(); it != m_aUnitExtraCosts.end(); ++it)
	{
		if ((*it).first == eUnit)
		{
			return ((*it).second);
		}
	}

	return 0;
}

void CvPlayer::setUnitExtraCost(UnitTypes eUnit, int iCost)
{
	PROFILE_EXTRA_FUNC();
	for (std::vector< std::pair<UnitTypes, int> >::iterator it = m_aUnitExtraCosts.begin(); it != m_aUnitExtraCosts.end(); ++it)
	{
		if ((*it).first == eUnit)
		{
			if (0 == iCost)
			{
				m_aUnitExtraCosts.erase(it);
			}
			else
			{
				(*it).second = iCost;
			}
			return;
		}
	}

	if (0 != iCost)
	{
		m_aUnitExtraCosts.push_back(std::make_pair(eUnit, iCost));
	}
}

bool CvPlayer::hasShrine(ReligionTypes eReligion) const
{
	if (eReligion != NO_RELIGION)
	{
		const CvCity* pHolyCity = GC.getGame().getHolyCity(eReligion);

		// if the holy city exists, and we own it
		if (pHolyCity && pHolyCity->getOwner() == getID())
			return pHolyCity->hasShrine(eReligion);
	}
	return false;
}

void CvPlayer::invalidatePopulationRankCache()
{
	algo::for_each(cities(), CvCity::fn::invalidatePopulationRankCache());
}

void CvPlayer::invalidateYieldRankCache(YieldTypes eYield)
{
	algo::for_each(cities(), CvCity::fn::invalidateYieldRankCache());
}

void CvPlayer::invalidateCommerceRankCache(CommerceTypes eCommerce)
{
	algo::for_each(cities(), CvCity::fn::invalidateCommerceRankCache());
}


void CvPlayer::doUpdateCacheOnTurn()
{
	PROFILE_EXTRA_FUNC();
	// add this back, after testing without it
	// invalidateYieldRankCache();

	clearCanConstructCache(NO_BUILDING);
	for (int i = 0; i < NUM_COMMERCE_TYPES; ++i)
	{
		m_cachedTotalCityBaseCommerceRate[i] = MAX_INT;
	}
}

void CvPlayer::processVoteSourceBonus(VoteSourceTypes eVoteSource, bool bActive)
{
	algo::for_each(cities(), CvCity::fn::processVoteSourceBonus(eVoteSource, bActive));
}

int CvPlayer::getVotes(VoteTypes eVote, VoteSourceTypes eVoteSource) const
{
	uint64_t iVotes = 0;

	const ReligionTypes eReligion = GC.getGame().getVoteSourceReligion(eVoteSource);

	if (eVote != NO_VOTE)
	{
		if (!GC.getVoteInfo(eVote).isVoteSourceType(eVoteSource))
		{
			return 0;
		}

		if (GC.getVoteInfo(eVote).isCivVoting())
		{
			if (NO_RELIGION == eReligion || getHasReligionCount(eReligion) > 0)
			{
				iVotes = 1;
			}
		}
		else if (GC.getVoteInfo(eVote).isCityVoting())
		{
			if (NO_RELIGION != eReligion)
			{
				iVotes = (int64_t)getHasReligionCount(eReligion);
			}
			else iVotes = (int64_t)getNumCities();
		}
		else if (NO_RELIGION == eReligion)
		{
			iVotes = (int64_t)getTotalPopulation();
		}
		else iVotes = (int64_t)getReligionPopulation(eReligion);

		if (NO_RELIGION != eReligion && getStateReligion() == eReligion)
		{
			iVotes *= (100 + GC.getVoteInfo(eVote).getStateReligionVotePercent());
			iVotes /= 100;
		}
	}
	else if (NO_RELIGION != eReligion)
	{
		iVotes = (int64_t)getReligionPopulation(eReligion);
	}
	else iVotes = (int64_t)getTotalPopulation();

	if (iVotes > MAX_INT)
	{
		return MAX_INT;
	}
	return static_cast<int>(iVotes);
}

bool CvPlayer::canDoResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData) const
{
	PROFILE_EXTRA_FUNC();
	const CvTeam& kOurTeam = GET_TEAM(getTeam());

	if (NO_PLAYER != kData.ePlayer)
	{
		if (!kOurTeam.isHasMet(GET_PLAYER(kData.ePlayer).getTeam()))
		{
			return false;
		}
	}

	if (GC.getVoteInfo(kData.eVote).isOpenBorders())
	{
		for (int iTeam2 = 0; iTeam2 < MAX_PC_TEAMS; ++iTeam2)
		{
			if (GET_TEAM((TeamTypes)iTeam2).isVotingMember(eVoteSource))
			{
				if (!kOurTeam.isOpenBordersTrading() && !GET_TEAM((TeamTypes)iTeam2).isOpenBordersTrading())
				{
					return false;
				}

				if (kOurTeam.isAtWar((TeamTypes)iTeam2))
				{
					return false;
				}
			}
		}
	}
	else if (GC.getVoteInfo(kData.eVote).isDefensivePact())
	{
		for (int iTeam2 = 0; iTeam2 < MAX_PC_TEAMS; ++iTeam2)
		{
			if (GET_TEAM((TeamTypes)iTeam2).isVotingMember(eVoteSource)
			&& !kOurTeam.canSignDefensivePact((TeamTypes)iTeam2))
			{
				return false;
			}
		}
	}
	else if (GC.getVoteInfo(kData.eVote).isForcePeace())
	{
		FAssert(NO_PLAYER != kData.ePlayer);
		CvPlayer& kPlayer = GET_PLAYER(kData.ePlayer);

		if (kPlayer.getTeam() != getTeam())
		{
			if (kOurTeam.isAtWar(kPlayer.getTeam()))
			{
				TeamTypes eMaster = getTeam();
				for (int iMaster = 0; iMaster < MAX_PC_TEAMS; ++iMaster)
				{
					if (iMaster != getID() && kOurTeam.isVassal((TeamTypes)iMaster))
					{
						if (GET_TEAM((TeamTypes)iMaster).isVotingMember(eVoteSource))
						{
							eMaster = (TeamTypes)iMaster;
							break;
						}
					}
				}

				if (!GET_TEAM(eMaster).canContact(kPlayer.getTeam()))
				{
					return false;
				}
			}
		}
	}
	else if (GC.getVoteInfo(kData.eVote).isForceWar())
	{
		FAssert(NO_PLAYER != kData.ePlayer);
		CvPlayer& kPlayer = GET_PLAYER(kData.ePlayer);

		if (!kOurTeam.isAtWar(kPlayer.getTeam()))
		{
			TeamTypes eMaster = getTeam();
			for (int iMaster = 0; iMaster < MAX_PC_TEAMS; ++iMaster)
			{
				if (iMaster != getID() && kOurTeam.isVassal((TeamTypes)iMaster))
				{
					if (GET_TEAM((TeamTypes)iMaster).isVotingMember(eVoteSource))
					{
						eMaster = (TeamTypes)iMaster;
						break;
					}
				}
			}

			if (!GET_TEAM(eMaster).canDeclareWar(kPlayer.getTeam()))
			{
				return false;
			}
		}
	}
	else if (GC.getVoteInfo(kData.eVote).isForceNoTrade())
	{
		FAssert(NO_PLAYER != kData.ePlayer);
		CvPlayer& kPlayer = GET_PLAYER(kData.ePlayer);

		if (!canStopTradingWithTeam(kPlayer.getTeam(), true))
		{
			return false;
		}

	}
	else if (GC.getVoteInfo(kData.eVote).isAssignCity())
	{
		if (GET_TEAM(GET_PLAYER(kData.eOtherPlayer).getTeam()).isVassal(GET_PLAYER(kData.ePlayer).getTeam()))
		{
			return false;
		}
	}

	return true;
}

bool CvPlayer::canDefyResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData) const
{
	PROFILE_EXTRA_FUNC();
	if (GC.getGame().getSecretaryGeneral(eVoteSource) == getTeam())
	{
		return false;
	}

	if (GC.getVoteInfo(kData.eVote).isOpenBorders())
	{
		for (int iTeam = 0; iTeam < MAX_PC_TEAMS; ++iTeam)
		{
			if ((TeamTypes)iTeam != getTeam())
			{
				const CvTeam& kTeam = GET_TEAM((TeamTypes)iTeam);
				if (kTeam.isVotingMember(eVoteSource))
				{
					if (!kTeam.isOpenBorders(getTeam()))
					{
						return true;
					}
				}
			}
		}
	}
	else if (GC.getVoteInfo(kData.eVote).isDefensivePact())
	{
		for (int iTeam = 0; iTeam < MAX_PC_TEAMS; ++iTeam)
		{
			if ((TeamTypes)iTeam != getTeam())
			{
				const CvTeam& kTeam = GET_TEAM((TeamTypes)iTeam);
				if (kTeam.isVotingMember(eVoteSource))
				{
					if (!kTeam.isDefensivePact(getTeam()))
					{
						return true;
					}
				}
			}
		}
	}
	else if (GC.getVoteInfo(kData.eVote).isForceNoTrade())
	{
		return true;
	}
	else if (GC.getVoteInfo(kData.eVote).isForceWar())
	{
		if (!::atWar(getTeam(), GET_PLAYER(kData.ePlayer).getTeam()))
		{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					  12/31/08								jdog5000	  */
/*																							  */
/*																							  */
/************************************************************************************************/
			// Vassals can't defy declarations of war
			if( !GET_TEAM(getTeam()).isAVassal() )
			{
				return true;
			}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					   END												  */
/************************************************************************************************/
		}
	}
	else if (GC.getVoteInfo(kData.eVote).isForcePeace())
	{
		if (GET_PLAYER(kData.ePlayer).getTeam() == getTeam())
		{
			return true;
		}

		if (::atWar(getTeam(), GET_PLAYER(kData.ePlayer).getTeam()))
		{
			return true;
		}
	}
	else if (GC.getVoteInfo(kData.eVote).isAssignCity())
	{
		if (kData.ePlayer == getID())
		{
			return true;
		}
	}
	else if (!GC.getGame().isTeamVote(kData.eVote))
	{
		return true;
	}

	return false;
}


void CvPlayer::setDefiedResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData)
{
	PROFILE_EXTRA_FUNC();
	FAssert(canDefyResolution(eVoteSource, kData));

	// cities get unhappiness
	foreach_(CvCity* pLoopCity, cities())
	{
		const ReligionTypes eReligion = GC.getGame().getVoteSourceReligion(eVoteSource);

		if (NO_RELIGION == eReligion || pLoopCity->isHasReligion(eReligion))
		{
			int iAngerLength = pLoopCity->flatDefyResolutionAngerLength();
			if (NO_RELIGION != eReligion && pLoopCity->isHasReligion(eReligion))
			{
				iAngerLength /= std::max(1, pLoopCity->getReligionCount());
			}

			pLoopCity->changeDefyResolutionAngerTimer(iAngerLength);
		}
	}

	setLoyalMember(eVoteSource, false);
}


void CvPlayer::setEndorsedResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData)
{
	setLoyalMember(eVoteSource, true);
}

bool CvPlayer::isFullMember(VoteSourceTypes eVoteSource) const
{
	if (!isLoyalMember(eVoteSource)

	|| NO_CIVIC != GC.getVoteSourceInfo(eVoteSource).getCivic()
	&& !isCivic((CivicTypes)GC.getVoteSourceInfo(eVoteSource).getCivic())

	|| NO_RELIGION != GC.getGame().getVoteSourceReligion(eVoteSource)
	&& getStateReligion() != GC.getGame().getVoteSourceReligion(eVoteSource))
	{
		return false;
	}
	return isVotingMember(eVoteSource);
}

bool CvPlayer::isVotingMember(VoteSourceTypes eVoteSource) const
{
	return getVotes(NO_VOTE, eVoteSource) > 0;
}

PlayerTypes CvPlayer::pickConqueredCityOwner(const CvCity& kCity) const
{
	PlayerTypes eBestPlayer = kCity.getLiberationPlayer(true);

	if (NO_PLAYER != eBestPlayer)
	{
		if (GET_TEAM(getTeam()).isVassal(GET_PLAYER(eBestPlayer).getTeam()))
		{
			return eBestPlayer;
		}
	}

	return getID();
}

bool CvPlayer::canHaveTradeRoutesWith(PlayerTypes ePlayer) const
{
	const CvPlayer& kOtherPlayer = GET_PLAYER(ePlayer);

	if (!kOtherPlayer.isAlive())
	{
		return false;
	}

	if (getTeam() == kOtherPlayer.getTeam())
	{
		return true;
	}


	if (GET_TEAM(getTeam()).isFreeTrade(kOtherPlayer.getTeam()) || (GET_TEAM(getTeam()).isLimitedBorders(kOtherPlayer.getTeam())) || (getForceAllTradeRoutes() > 0))
	{
		if (GET_TEAM(getTeam()).isVassal(kOtherPlayer.getTeam()))
		{
			return true;
		}

		if (GET_TEAM(kOtherPlayer.getTeam()).isVassal(getTeam()))
		{
			return true;
		}

		if (!isNoForeignTrade() && !kOtherPlayer.isNoForeignTrade())
		{
			return true;
		}
	}

	return false;
}

bool CvPlayer::canStealTech(PlayerTypes eTarget, TechTypes eTech) const
{
	if (GET_TEAM(GET_PLAYER(eTarget).getTeam()).isHasTech(eTech))
	{
		if (canResearch(eTech))
		{
			return true;
		}
	}

	return false;
}

bool CvPlayer::canForceCivics(PlayerTypes eTarget, CivicTypes eCivic) const
{
	return (GET_PLAYER(eTarget).canDoCivics(eCivic) && !GET_PLAYER(eTarget).isCivic(eCivic) && isCivic(eCivic));
}

bool CvPlayer::canForceReligion(PlayerTypes eTarget, ReligionTypes eReligion) const
{
	return (GET_PLAYER(eTarget).canDoReligion(eReligion) && GET_PLAYER(eTarget).getStateReligion() != eReligion && getStateReligion() == eReligion);
}

bool CvPlayer::canSpyDestroyUnit(PlayerTypes eTarget, const CvUnit& kUnit) const
{
	if (kUnit.getTeam() == getTeam())
	{
		return false;
	}

	if (kUnit.getUnitInfo().getProductionCost() <= 0)
	{
		return false;
	}

	if (!kUnit.plot()->isVisible(getTeam(), false))
	{
		return false;
	}

	return true;
}

bool CvPlayer::canSpyBribeUnit(PlayerTypes eTarget, const CvUnit& kUnit) const
{
	PROFILE_EXTRA_FUNC();
	if (!canSpyDestroyUnit(eTarget, kUnit))
	{
		return false;
	}

	// Can't buy units when at war
	if (kUnit.isEnemy(getTeam()))
	{
		return false;
	}

	// Can't buy units if they are not in a legal plot
	if (!GET_TEAM(getTeam()).isFriendlyTerritory(GET_PLAYER(eTarget).getTeam()) && !GET_TEAM(getTeam()).isOpenBorders(GET_PLAYER(eTarget).getTeam()))
	{
		return false;
	}

	foreach_(const CvUnit* pLoopUnit, kUnit.plot()->units())
	{
		if (pLoopUnit != &kUnit)
		{
			if (pLoopUnit->isEnemy(getTeam()))
			{
				// If we buy the unit, we will be on the same plot as an enemy unit! Not good.
				return false;
			}
		}
	}

	return true;
}

bool CvPlayer::canSpyDestroyBuilding(PlayerTypes eTarget, BuildingTypes eBuilding) const
{
	return (GC.getBuildingInfo(eBuilding).getProductionCost() > 0 && !isLimitedWonder(eBuilding));
}

bool CvPlayer::canSpyDestroyProject(PlayerTypes eTarget, ProjectTypes eProject) const
{
	const CvProjectInfo& kProject = GC.getProjectInfo(eProject);
	if (kProject.getProductionCost() <= 0)
	{
		return false;
	}

	if (GET_TEAM(GET_PLAYER(eTarget).getTeam()).getProjectCount(eProject) <= 0)
	{
		return false;
	}

	if (::isWorldProject(eProject))
	{
		return false;
	}

	if (!kProject.isSpaceship())
	{
		return false;
	}
	else
	{
		const VictoryTypes eVicotry = (VictoryTypes)kProject.getVictoryPrereq();
		if (NO_VICTORY != eVicotry)
		{
			// Can't destroy spaceship components if we have already launched
			if (GET_TEAM(GET_PLAYER(eTarget).getTeam()).getVictoryCountdown(eVicotry) >= 0)
			{
				return false;
			}
		}
	}

	return true;
}

void CvPlayer::forcePeace(PlayerTypes ePlayer)
{
	if (!GET_TEAM(getTeam()).isAVassal())
	{
		FAssert(GET_TEAM(getTeam()).canChangeWarPeace(GET_PLAYER(ePlayer).getTeam()));

		CLinkList<TradeData> playerList;
		CLinkList<TradeData> loopPlayerList;
		TradeData kTradeData;
		setTradeItem(&kTradeData, TRADE_PEACE_TREATY);
		playerList.insertAtEnd(kTradeData);
		loopPlayerList.insertAtEnd(kTradeData);

		GC.getGame().implementDeal(getID(), ePlayer, &playerList, &loopPlayerList);
	}
}

bool CvPlayer::canSpiesEnterBorders(PlayerTypes ePlayer) const
{
	PROFILE_EXTRA_FUNC();
	for (int iMission = 0; iMission < GC.getNumEspionageMissionInfos(); ++iMission)
	{
		if (GC.getEspionageMissionInfo((EspionageMissionTypes)iMission).isNoActiveMissions() && GC.getEspionageMissionInfo((EspionageMissionTypes)iMission).isPassive())
		{
			if (GET_PLAYER(ePlayer).canDoEspionageMission((EspionageMissionTypes)iMission, getID(), NULL, -1, NULL))
			{
				return false;
			}
		}
	}

	return true;
}

int CvPlayer::getReligionPopulation(ReligionTypes eReligion) const
{
	return algo::accumulate(cities()
		| filtered(CvCity::fn::isHasReligion(eReligion))
		| transformed(CvCity::fn::getPopulation()), 0);
}

int CvPlayer::getNewCityProductionValue() const
{
	PROFILE_EXTRA_FUNC();
	int iValue = 0;

	for (int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
	{
		const BuildingTypes eBuilding = static_cast<BuildingTypes>(iJ);

		if (GC.getBuildingInfo(eBuilding).getFreeStartEra() != NO_ERA
		&& GC.getGame().getStartEra() >= GC.getBuildingInfo(eBuilding).getFreeStartEra())
		{
			iValue += 100 * getProductionNeeded(eBuilding) / std::max(1, 100 + getProductionModifier(eBuilding));
		}
	}

	iValue *= 100 + GC.getDefineINT("NEW_CITY_BUILDING_VALUE_MODIFIER");
	iValue /= 100;

	iValue += GC.getDefineINT("ADVANCED_START_CITY_COST") * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 100;

	const int iPopulation = GC.getINITIAL_CITY_POPULATION() + GC.getEraInfo(GC.getGame().getStartEra()).getFreePopulation();
	for (int i = 1; i <= iPopulation; ++i)
	{
		iValue += (getGrowthThreshold(i) * GC.getDefineINT("ADVANCED_START_POPULATION_COST")) / 100;
	}

	return iValue;
}

int CvPlayer::getGrowthThreshold(int iPopulation) const
{
	int iThreshold = GC.getDefineINT("BASE_CITY_GROWTH_THRESHOLD");

	if (iPopulation > 1)
	{
		iThreshold += (iPopulation - 1) * GC.getDefineINT("CITY_GROWTH_MULTIPLIER");
	}
	iThreshold *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
	iThreshold /= 100;

	iThreshold *= GC.getEraInfo(getCurrentEra()).getGrowthPercent();
	iThreshold /= 100;

	if (isNormalAI())
	{
		const int iMod =
		(
			GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIGrowthPercent() - 100
			+
			GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIPerEraModifier() * getCurrentEra()
		);
		if (iMod != 0)
		{
			iThreshold = getModifiedIntValue(iThreshold, iMod);
		}
	}
	if (isGoldenAge())
	{
		iThreshold = getModifiedIntValue(iThreshold, GC.getDefineINT("GOlDEN_AGE_PERCENT_LESS_FOOD_FOR_GROWTH"));
	}
	return std::max(1, iThreshold);
}

void CvPlayer::verifyUnitStacksValid()
{
	algo::for_each(units(), CvUnit::fn::verifyStackValid());
}

//TB Prophet Mod begin
UnitTypes CvPlayer::getTechFreeProphet(TechTypes eTech) const
{
	if (GC.getGame().isOption(GAMEOPTION_RELIGION_DIVINE_PROPHETS))
	{
		return (UnitTypes) GC.getTechInfo(eTech).getFirstFreeProphet();
	}
	return NO_UNIT;
}
//TB Prophet Mod end

// BUG - Trade Totals - start
/*
 * Adds the yield and count for each trade route with eWithPlayer.
 *
 * The yield and counts are not reset to zero.
 * If Fractional Trade Routes is enabled and bRound is false, the yield values are left times 100.
 */
void CvPlayer::calculateTradeTotals(YieldTypes eIndex, int& iDomesticYield, int& iDomesticRoutes, int& iForeignYield, int& iForeignRoutes, PlayerTypes eWithPlayer, bool bRound, bool bBase) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(CvCity* pCity, cities())
	{
		pCity->calculateTradeTotals(eIndex, iDomesticYield, iDomesticRoutes, iForeignYield, iForeignRoutes, eWithPlayer, bRound, bBase);
	}
}

/*
 * Returns the total trade yield with eWithPlayer.
 *
 * If Fractional Trade Routes is enabled, the yield value is left times 100.
 * UNUSED
 */
int CvPlayer::calculateTotalTradeYield(YieldTypes eIndex, PlayerTypes eWithPlayer, bool bRound, bool bBase) const
{
	int iDomesticYield = 0;
	int iDomesticRoutes = 0;
	int iForeignYield = 0;
	int iForeignRoutes = 0;

	calculateTradeTotals(eIndex, iDomesticYield, iDomesticRoutes, iForeignYield, iForeignRoutes, eWithPlayer, bRound, bBase);
	return iDomesticYield + iForeignRoutes;
}
// BUG - Trade Totals - end


void CvPlayer::buildTradeTable(PlayerTypes eOtherPlayer, CLinkList<TradeData>& ourList) const
{
	PROFILE_EXTRA_FUNC();
	TradeData item;

	//	Put the gold and maps into the table
	setTradeItem(&item, TRADE_GOLD);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	//	Gold per turn
	setTradeItem(&item, TRADE_GOLD_PER_TURN);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	//	Maps
	setTradeItem(&item, TRADE_MAPS, 0);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	//	Vassal
	setTradeItem(&item, TRADE_VASSAL, 0);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	 //Embassy
	setTradeItem(&item, TRADE_EMBASSY);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	//Free Trade
	setTradeItem(&item, TRADE_FREE_TRADE_ZONE);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	//Right of Passage
	setTradeItem(&item, TRADE_RITE_OF_PASSAGE);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	//	Open Borders
	setTradeItem(&item, TRADE_OPEN_BORDERS);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	//	Defensive Pact
	setTradeItem(&item, TRADE_DEFENSIVE_PACT);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	//	Permanent Alliance
	setTradeItem(&item, TRADE_PERMANENT_ALLIANCE);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	if (::atWar(getTeam(), GET_PLAYER(eOtherPlayer).getTeam()))
	{
		//	We are at war, allow a peace treaty option
		setTradeItem(&item, TRADE_PEACE_TREATY);
		ourList.insertAtEnd(item);

		//	Capitulation
		setTradeItem(&item, TRADE_SURRENDER, 0);
		if (canTradeItem(eOtherPlayer, item))
		{
			ourList.insertAtEnd(item);
		}
	}

	//	Initial build of the inventory lists and buttons.
	//	Go through all the possible headings
	for (int i = NUM_BASIC_ITEMS; i < NUM_TRADEABLE_HEADINGS; i++)
	{
		//bool bFoundItemUs = false;

		//	Build what we need to build for this item
		switch (i)
		{
		case TRADE_TECHNOLOGIES:
			for (int j = 0; j < GC.getNumTechInfos(); j++)
			{
				setTradeItem(&item, TRADE_TECHNOLOGIES, j);
				if (canTradeItem(eOtherPlayer, item))
				{
					//bFoundItemUs = true;
					ourList.insertAtEnd(item);
				}
			}
			break;

		case TRADE_RESOURCES:
			for (int j = 0; j < GC.getNumBonusInfos(); j++)
			{
				setTradeItem(&item, TRADE_RESOURCES, j);
				if (canTradeItem(eOtherPlayer, item))
				{
					//bFoundItemUs = true;
					ourList.insertAtEnd(item);
				}
			}
			break;

		case TRADE_CITIES:
			foreach_(const CvCity* pLoopCity, cities())
			{
				setTradeItem(&item, TRADE_CITIES, pLoopCity->getID());
				if (canTradeItem(eOtherPlayer, item))
				{
					//bFoundItemUs = true;
					ourList.insertAtEnd(item);
				}
			}
			break;

		case TRADE_PEACE:
			if (!isHumanPlayer())
			{
				for (int j = 0; j < MAX_PC_TEAMS; j++)
				{
					if (GET_TEAM((TeamTypes)j).isAlive())
					{
						if (j != getTeam() && j != GET_PLAYER(eOtherPlayer).getTeam())
						{
							setTradeItem(&item, TRADE_PEACE, j);
							if (canTradeItem(eOtherPlayer, item))
							{
								ourList.insertAtEnd(item);
								//bFoundItemUs = true;
							}
						}
					}
				}
			}
			break;

		case TRADE_WAR:
			if (!isHumanPlayer())
			{
				for (int j = 0; j < MAX_PC_TEAMS; j++)
				{
					if (GET_TEAM((TeamTypes)j).isAlive())
					{
						if (j != getTeam() && j != GET_PLAYER(eOtherPlayer).getTeam())
						{
							setTradeItem(&item, TRADE_WAR, j);
							if (canTradeItem(eOtherPlayer, item))
							{
								ourList.insertAtEnd(item);
								//bFoundItemUs = true;
							}
						}
					}
				}
			}
			break;

		case TRADE_EMBARGO:
			if (!isHumanPlayer())
			{
				for (int j = 0; j < MAX_PC_TEAMS; j++)
				{
					if (GET_TEAM((TeamTypes)j).isAlive())
					{
						if (j != getTeam() && j != GET_PLAYER(eOtherPlayer).getTeam())
						{
							setTradeItem(&item, TRADE_EMBARGO, j);
							if (canTradeItem(eOtherPlayer, item))
							{
								ourList.insertAtEnd(item);
								//bFoundItemUs = true;
							}
						}
					}
				}
			}
			break;

		case TRADE_CIVIC:
			for (int j = 0; j < GC.getNumCivicInfos(); j++)
			{
				setTradeItem(&item, TRADE_CIVIC, j);
				if (canTradeItem(eOtherPlayer, item))
				{
					//bFoundItemUs = true;
					ourList.insertAtEnd(item);
				}
			}
			break;

		case TRADE_RELIGION:
			for (int j = 0; j < GC.getNumReligionInfos(); j++)
			{
				setTradeItem(&item, TRADE_RELIGION, j);
				if (canTradeItem(eOtherPlayer, item))
				{
					//bFoundItemUs = true;
					ourList.insertAtEnd(item);
				}
			}
			break;

		case TRADE_CORPORATION:
			for (int j = 0; j < GC.getNumCorporationInfos(); j++)
			{
				setTradeItem(&item, TRADE_CORPORATION, j);
				if (canTradeItem(eOtherPlayer, item))
				{
					//bFoundItemUs = true;
					ourList.insertAtEnd(item);
				}
			}
			break;

		case TRADE_SECRETARY_GENERAL_VOTE:
			for (int j = 0; j < GC.getNumVoteSourceInfos(); j++)
			{
				setTradeItem(&item, TRADE_SECRETARY_GENERAL_VOTE, j);
				if (canTradeItem(eOtherPlayer, item))
				{
					//bFoundItemUs = true;
					ourList.insertAtEnd(item);
				}
			}
			break;

		case TRADE_PLEDGE_VOTE:
			setTradeItem(&item, TRADE_PLEDGE_VOTE, PLAYER_VOTE_YES);
			if (canTradeItem(eOtherPlayer, item))
			{
				//bFoundItemUs = true;
				ourList.insertAtEnd(item);
			}

			setTradeItem(&item, TRADE_PLEDGE_VOTE, PLAYER_VOTE_NO);
			if (canTradeItem(eOtherPlayer, item))
			{
				//bFoundItemUs = true;
				ourList.insertAtEnd(item);
			}

			setTradeItem(&item, TRADE_PLEDGE_VOTE, PLAYER_VOTE_ABSTAIN);
			if (canTradeItem(eOtherPlayer, item))
			{
				//bFoundItemUs = true;
				ourList.insertAtEnd(item);
			}
			break;
		case TRADE_WORKER:
			foreach_(const CvUnit* pLoopUnit, units())
			{
				setTradeItem(&item, TRADE_WORKER, pLoopUnit->getID());
				if (canTradeItem(eOtherPlayer, item))
				{
					//bFoundItemUs = true;
					ourList.insertAtEnd(item);
				}
			}
			break;

		case TRADE_MILITARY_UNIT:
			foreach_(const CvUnit* pLoopUnit, units())
			{
				setTradeItem(&item, TRADE_MILITARY_UNIT, pLoopUnit->getID());
				if (canTradeItem(eOtherPlayer, item))
				{
					//bFoundItemUs = true;
					ourList.insertAtEnd(item);
				}
			}
			break;
		case TRADE_CONTACT:
			for (int j = 0; j < MAX_PC_TEAMS; j++)
			{
				if (GET_TEAM((TeamTypes)j).isAlive())
				{
					if (j != getTeam() && j != GET_PLAYER(eOtherPlayer).getTeam())
					{
						setTradeItem(&item, TRADE_CONTACT, j);
						if (canTradeItem(eOtherPlayer, item))
						{
							ourList.insertAtEnd(item);
							//bFoundItemUs = true;
						}
					}
				}
			}
			break;
		}
	}
}

bool CvPlayer::getHeadingTradeString(PlayerTypes eOtherPlayer, TradeableItems eItem, CvWString& szString, CvString& szIcon) const
{
	szIcon.clear();

	switch ( eItem )
	{
	case TRADE_TECHNOLOGIES:
		szString = gDLL->getText("TXT_KEY_CONCEPT_TECHNOLOGY");
		break;

	case TRADE_RESOURCES:
		szString = gDLL->getText("TXT_KEY_TRADE_RESOURCES");
		break;

	case TRADE_CITIES:
		szString = gDLL->getText("TXT_KEY_TRADE_CITIES");
		break;

	case TRADE_WORKER:
		szString = gDLL->getText("TXT_KEY_TRADE_WORKERS");
		break;

	case TRADE_MILITARY_UNIT:
		szString = gDLL->getText("TXT_KEY_TRADE_MILITARY_UNIT");
		break;
	case TRADE_CONTACT:
		szString = gDLL->getText("TXT_KEY_TRADE_MAKE_CONTACT_WITH");
		break;

	case TRADE_PEACE:
		szString = gDLL->getText("TXT_KEY_TRADE_MAKE_PEACE_WITH");
		break;

	case TRADE_WAR:
		szString = gDLL->getText("TXT_KEY_TRADE_DECLARE_WAR_ON");
		break;

	case TRADE_EMBARGO:
		szString = gDLL->getText("TXT_KEY_TRADE_STOP_TRADING_WITH");
		break;

	case TRADE_CIVIC:
		szString = gDLL->getText("TXT_KEY_TRADE_ADOPT");
		break;

	case TRADE_RELIGION:
		szString = gDLL->getText("TXT_KEY_TRADE_CONVERT");
		break;

	case TRADE_CORPORATION:
		szString = gDLL->getText("TXT_KEY_TRADE_BUY_CORPORATION");
		break;
	case TRADE_PLEDGE_VOTE:
		szString = gDLL->getText("TXT_KEY_TRADE_PLEDGE_VOTE");
		break;
	case TRADE_SECRETARY_GENERAL_VOTE:
		szString = gDLL->getText("TXT_KEY_TRADE_SECRETARY_GENERAL_VOTE");
		break;
	case TRADE_EMBASSY:
		szString = gDLL->getText("TXT_KEY_TRADE_EMBASSY_STRING");
		break;

	default:
		szString.clear();
		return false;
		break;
	}

	return true;
}


bool CvPlayer::getItemTradeString(PlayerTypes eOtherPlayer, bool bOffer, bool bShowingCurrent, const TradeData& zTradeData, CvWString& szString, CvString& szIcon) const
{
	szIcon.clear();

	switch (zTradeData.m_eItemType)
	{
	case TRADE_GOLD:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_GOLD_NUM", zTradeData.m_iData);
		}
		else
		{
			szString = gDLL->getText("TXT_KEY_TRADE_GOLD_NUM", AI_maxGoldTrade(eOtherPlayer));
		}
		break;
	case TRADE_GOLD_PER_TURN:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_GOLD_PER_TURN_NUM", zTradeData.m_iData);
		}
		else
		{
			szString = gDLL->getText("TXT_KEY_TRADE_GOLD_PER_TURN_NUM", AI_maxGoldPerTurnTrade(eOtherPlayer));
		}
		break;
	case TRADE_MAPS:
		szString = gDLL->getText("TXT_KEY_TRADE_WORLD_MAP_STRING");
		break;
	case TRADE_VASSAL:
		szString = gDLL->getText("TXT_KEY_TRADE_VASSAL_STRING");
		break;
	case TRADE_SURRENDER:
		szString = gDLL->getText("TXT_KEY_TRADE_CAPITULATE_STRING");
		break;
	case TRADE_OPEN_BORDERS:
		szString = gDLL->getText("TXT_KEY_TRADE_OPEN_BORDERS_STRING");
		break;
	case TRADE_DEFENSIVE_PACT:
		szString = gDLL->getText("TXT_KEY_TRADE_DEFENSIVE_PACT_STRING");
		break;
	case TRADE_PERMANENT_ALLIANCE:
		szString = gDLL->getText("TXT_KEY_TRADE_PERMANENT_ALLIANCE_STRING");
		break;
	case TRADE_PEACE_TREATY:
		szString = gDLL->getText("TXT_KEY_TRADE_PEACE_TREATY_STRING", getTreatyLength());
		break;
	case TRADE_TECHNOLOGIES:
		szString = GC.getTechInfo((TechTypes)zTradeData.m_iData).getDescription();
		szIcon = GC.getTechInfo((TechTypes)zTradeData.m_iData).getButton();
		break;
	case TRADE_RESOURCES:
        {
            BonusTypes eBonus = (BonusTypes)zTradeData.m_iData;
            szIcon = GC.getBonusInfo(eBonus).getButton();

            if (bOffer)
            {
                int iNumResources = GET_PLAYER(eOtherPlayer).getNumTradeableBonuses((BonusTypes)zTradeData.m_iData);
                if (bShowingCurrent)
                {
                    ++iNumResources;
                }
                szString = gDLL->getText("TXT_KEY_TRADE_RESOURCE", GC.getBonusInfo((BonusTypes)zTradeData.m_iData).getDescription(), iNumResources);
            }
            else
            {

                int iHisResources = GET_PLAYER(eOtherPlayer).getNumAvailableBonuses(eBonus);
                int iMyResources = getNumAvailableBonuses(eBonus);
                if (bShowingCurrent)
                {
                    ++iHisResources;
                }

                if (iMyResources == 0 && iHisResources > 0)
                {
                    szString = gDLL->getText("TXT_KEY_TRADE_RESOURCE_COLORED_GREEN", GC.getBonusInfo(eBonus).getDescription(), iMyResources);
                }
                else if (iMyResources > 0 && iHisResources == 0)
                {
                    szString = gDLL->getText("TXT_KEY_TRADE_RESOURCE_COLORED_GREEN", GC.getBonusInfo(eBonus).getDescription(), iMyResources);
                }
                else
                {
                    szString = gDLL->getText("TXT_KEY_TRADE_RESOURCE_GRAY", GC.getBonusInfo(eBonus).getDescription(), iMyResources);
                }
            }
            break;
        }
	case TRADE_CITIES:
		{
			CvCity* pCity = NULL;
			if (bOffer)
			{
				pCity = GET_PLAYER(eOtherPlayer).getCity(zTradeData.m_iData);
			}
			else
			{
				pCity = getCity(zTradeData.m_iData);
			}
			if (pCity)
			{
				if (pCity->getLiberationPlayer(false) == eOtherPlayer)
				{
					szString.Format(L"%s (%s)", pCity->getName().GetCString(), gDLL->getText("TXT_KEY_LIBERATE_CITY").GetCString());
				}
				else
				{
					szString = gDLL->getText("TXT_KEY_CITY_OF", pCity->getNameKey());
				}
			}
		}
		break;

	case TRADE_PEACE:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_PEACE_WITH");
			szString += GET_TEAM((TeamTypes)zTradeData.m_iData).getName();
		}
		else
		{
			szString = GET_TEAM((TeamTypes)zTradeData.m_iData).getName();
		}
		break;
	case TRADE_WAR:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_WAR_WITH");
			szString += GET_TEAM((TeamTypes)zTradeData.m_iData).getName();
		}
		else
		{
			szString = GET_TEAM((TeamTypes)zTradeData.m_iData).getName();
		}
		break;
	case TRADE_EMBARGO:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_STOP_TRADING_WITH");
			szString += L" " + GET_TEAM((TeamTypes)zTradeData.m_iData).getName();
		}
		else
		{
			szString = GET_TEAM((TeamTypes)zTradeData.m_iData).getName();
		}
		break;
	case TRADE_CIVIC:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_ADOPT");
			szString += GC.getCivicInfo((CivicTypes)zTradeData.m_iData).getDescription();
		}
		else
		{
			szString = GC.getCivicInfo((CivicTypes)zTradeData.m_iData).getDescription();
		}
		szIcon = GC.getCivicInfo((CivicTypes)zTradeData.m_iData).getButton();
		break;
	case TRADE_RELIGION:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_CONVERT");
			szString += GC.getReligionInfo((ReligionTypes)zTradeData.m_iData).getDescription();
		}
		else
		{
			szString = GC.getReligionInfo((ReligionTypes)zTradeData.m_iData).getDescription();
		}
		szIcon = GC.getReligionInfo((ReligionTypes)zTradeData.m_iData).getButton();
		break;

	case TRADE_WORKER:
		{
			CvUnit* pUnit = NULL;
			if (bOffer)
			{
				pUnit = GET_PLAYER(eOtherPlayer).getUnit(zTradeData.m_iData);
			}
			else
			{
				pUnit = getUnit(zTradeData.m_iData);
			}
			if (pUnit)
			{
				szString.Format(L"%s", pUnit->getName().GetCString());
				szIcon = GC.getUnitInfo((UnitTypes)pUnit->getUnitType()).getButton();
			}
		}
		break;

	case TRADE_MILITARY_UNIT:
		{
			CvUnit* pUnit = NULL;
			if (bOffer)
			{
				pUnit = GET_PLAYER(eOtherPlayer).getUnit(zTradeData.m_iData);
			}
			else
			{
				pUnit = getUnit(zTradeData.m_iData);
			}
			if (pUnit)
			{
				CvWStringBuffer szTemp;
				//GAMETEXT.setUnitHelp(szTemp, pUnit, true);
				//szString = CvWString(szTemp.getCString());
				szString.Format(L"%s", pUnit->getName().GetCString());
				szIcon = GC.getUnitInfo((UnitTypes)pUnit->getUnitType()).getButton();
			}
		}
		break;
	case TRADE_CONTACT:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_CONTACT_WITH");
			szString += gDLL->getText(" ");
			szString += GET_TEAM((TeamTypes)zTradeData.m_iData).getName();
		}
		else
		{
			szString = gDLL->getText(" ");
			szString += GET_TEAM((TeamTypes)zTradeData.m_iData).getName();
		}
		break;

	case TRADE_CORPORATION:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_BUY");
			szString += gDLL->getText(" ");
			szString += GC.getCorporationInfo((CorporationTypes)zTradeData.m_iData).getDescription();
		}
		else
		{
			szString = gDLL->getText(" ");
			szString += GC.getCorporationInfo((CorporationTypes)zTradeData.m_iData).getDescription();
		}
		szIcon = GC.getCorporationInfo((CorporationTypes)zTradeData.m_iData).getButton();
		break;
	case TRADE_PLEDGE_VOTE:
		szString = gDLL->getText("TXT_KEY_TRADE_VOTE");
		if ((PlayerVoteTypes)zTradeData.m_iData == PLAYER_VOTE_YES)
		{
			szString = gDLL->getText("TXT_KEY_POPUP_YES");
		}
		else if ((PlayerVoteTypes)zTradeData.m_iData == PLAYER_VOTE_NO)
		{
			szString = gDLL->getText("TXT_KEY_POPUP_NO");
		}
		else if ((PlayerVoteTypes)zTradeData.m_iData == PLAYER_VOTE_ABSTAIN)
		{
			szString = gDLL->getText("TXT_KEY_POPUP_ABSTAIN");
		}
		break;
	case TRADE_SECRETARY_GENERAL_VOTE:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_SEC_GEN_VOTE", GET_PLAYER(eOtherPlayer).getNameKey(), GC.getVoteSourceInfo((VoteSourceTypes)zTradeData.m_iData).getDescription());
		}
		else
		{
			szString = GC.getVoteSourceInfo((VoteSourceTypes)zTradeData.m_iData).getDescription();
		}
		break;
	case TRADE_RITE_OF_PASSAGE:
		szString = gDLL->getText("TXT_KEY_TRADE_LIMITED_BORDERS_STRING");
		break;
	case TRADE_FREE_TRADE_ZONE:
		szString = gDLL->getText("TXT_KEY_MISC_FREE_TRADE_ZONE");
		break;
	case TRADE_EMBASSY:
		szString = gDLL->getText("TXT_KEY_TRADE_EMBASSY_STRING");
		break;

	default:
		szString.clear();
		return false;
	}

	return true;
}

void CvPlayer::updateTradeList(PlayerTypes eOtherPlayer, CLinkList<TradeData>& ourInventory, const CLinkList<TradeData>& ourOffer, const CLinkList<TradeData>& theirOffer) const
{
	PROFILE_EXTRA_FUNC();
	for (CLLNode<TradeData>* pNode = ourInventory.head(); pNode; pNode = ourInventory.next(pNode))
	{
		pNode->m_data.m_bHidden = false;

		// Don't show peace treaties when not at war
		if (!::atWar(getTeam(), GET_PLAYER(eOtherPlayer).getTeam()))
		{
			if (pNode->m_data.m_eItemType == TRADE_PEACE_TREATY || pNode->m_data.m_eItemType == TRADE_SURRENDER)
			{
				pNode->m_data.m_bHidden = true;
			}
		}

		// Don't show technologies with no tech trading game option
		if (GC.getGame().isOption(GAMEOPTION_NO_TECH_TRADING) && pNode->m_data.m_eItemType == TRADE_TECHNOLOGIES)
		{
			pNode->m_data.m_bHidden = true;
		}
	}

	for (CLLNode<TradeData>* pNode = ourInventory.head(); pNode; pNode = ourInventory.next(pNode))
	{
		switch (pNode->m_data.m_eItemType)
		{
		case TRADE_PEACE_TREATY:
			for (CLLNode<TradeData>* pOfferNode = ourOffer.head(); pOfferNode; pOfferNode = ourOffer.next(pOfferNode))
			{
				// Don't show vassal deals if peace treaty is already on the table
				if (CvDeal::isVassal(pOfferNode->m_data.m_eItemType))
				{
					pNode->m_data.m_bHidden = true;
					break;
				}
			}
			break;
		case TRADE_VASSAL:
		case TRADE_SURRENDER:
			for (CLLNode<TradeData>* pOfferNode = theirOffer.head(); pOfferNode; pOfferNode = theirOffer.next(pOfferNode))
			{
				// Don't show vassal deals if another type of vassal deal is on the table
				if (CvDeal::isVassal(pOfferNode->m_data.m_eItemType))
				{
					pNode->m_data.m_bHidden = true;
					break;
				}
			}

			if (!pNode->m_data.m_bHidden)
			{
				for (CLLNode<TradeData>* pOfferNode = ourOffer.head(); pOfferNode; pOfferNode = ourOffer.next(pOfferNode))
				{
					// Don't show peace deals if the other player is offering to be a vassal
					if (CvDeal::isEndWar(pOfferNode->m_data.m_eItemType))
					{
						pNode->m_data.m_bHidden = true;
						break;
					}
				}
			}
			break;

		case TRADE_PLEDGE_VOTE:
			for (CLLNode<TradeData>* pOfferNode = theirOffer.head(); pOfferNode; pOfferNode = theirOffer.next(pOfferNode))
			{
				// Don't show other trade items of the same category if it's a single option
				if (CvDeal::isSingleOption(pOfferNode->m_data.m_eItemType) && (pOfferNode->m_data.m_eItemType == pNode->m_data.m_eItemType))
				{
					pNode->m_data.m_bHidden = true;
					break;
				}
			}
			break;

		default:
			break;
		}
	}
	/************************************************************************************************/
	/* Afforess					  Start		5/29/11												 */
	/*																							  */
	/* Allow city trades for other items															*/
	/************************************************************************************************/
	/*if (!isHumanPlayer() || !GET_PLAYER(eOtherPlayer).isHumanPlayer())  // everything allowed in human-human trades
	{
		CLLNode<TradeData>* pFirstOffer = ourOffer.head();
		if (pFirstOffer == NULL)
		{
			pFirstOffer = theirOffer.head();
		}

		if (pFirstOffer)
		{
			if (!CvDeal::isEndWar(pFirstOffer->m_data.m_eItemType) || !::atWar(getTeam(), GET_PLAYER(eOtherPlayer).getTeam()))
			{
				for (CLLNode<TradeData>* pNode = ourInventory.head(); pNode; pNode = ourInventory.next(pNode))
				{
					if (pFirstOffer->m_data.m_eItemType == TRADE_CITIES || pNode->m_data.m_eItemType == TRADE_CITIES)
					{
						pNode->m_data.m_bHidden = true;
					}
					else if (CvDeal::isAnnual(pFirstOffer->m_data.m_eItemType) != CvDeal::isAnnual(pNode->m_data.m_eItemType))
					{
						pNode->m_data.m_bHidden = true;
					}
				}
			}
		}
	}*/
	/************************************************************************************************/
	/* Afforess						 END															*/
	/************************************************************************************************/
}

int CvPlayer::getIntroMusicScriptId(PlayerTypes eForPlayer) const
{
	const CvPlayer& kForPlayer = GET_PLAYER(eForPlayer);
	const EraTypes eEra = kForPlayer.getCurrentEra();
	const CvLeaderHeadInfo& kLeader = GC.getLeaderHeadInfo(getLeaderType());
	if (GET_TEAM(kForPlayer.getTeam()).isAtWar(getTeam()))
	{
		return kLeader.getDiploWarIntroMusicScriptIds(eEra);
	}
	else
	{
		return kLeader.getDiploPeaceIntroMusicScriptIds(eEra);
	}
}

int CvPlayer::getMusicScriptId(PlayerTypes eForPlayer) const
{
	const CvPlayer& kForPlayer = GET_PLAYER(eForPlayer);
	const EraTypes eEra = kForPlayer.getCurrentEra();
	const CvLeaderHeadInfo& kLeader = GC.getLeaderHeadInfo(getLeaderType());
	if (GET_TEAM(kForPlayer.getTeam()).isAtWar(getTeam()))
	{
		return kLeader.getDiploWarMusicScriptIds(eEra);
	}
	else
	{
		return kLeader.getDiploPeaceMusicScriptIds(eEra);
	}
}


void CvPlayer::getGlobeLayerColors(GlobeLayerTypes eGlobeLayerType, int iOption, std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const
{
	switch (eGlobeLayerType)
	{
	case GLOBE_LAYER_TRADE:
		getTradeLayerColors(aColors, aIndicators);
		break;
	case GLOBE_LAYER_UNIT:
		getUnitLayerColors((GlobeLayerUnitOptionTypes) iOption, aColors, aIndicators);
		break;
	case GLOBE_LAYER_RESOURCE:
		getResourceLayerColors((GlobeLayerResourceOptionTypes) iOption, aColors, aIndicators);
		break;
	case GLOBE_LAYER_RELIGION:
		getReligionLayerColors((ReligionTypes) iOption, aColors, aIndicators);
		break;
	case GLOBE_LAYER_CULTURE:
		getCultureLayerColors(aColors, aIndicators);
		break;
	default:
		FErrorMsg("Unknown globe layer type");
		break;
	}
}

void CvPlayer::getTradeLayerColors(std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const
{
	PROFILE_EXTRA_FUNC();
	aColors.resize(GC.getCurrentViewport()->numPlots(), NiColorA(0, 0, 0, 0));
	aIndicators.clear();

	typedef std::map< int, std::vector<int> > PlotGroupMap;
	PlotGroupMap mapPlotGroups;
	for (int iI = 0; iI < GC.getCurrentViewport()->numPlots(); ++iI)
	{
		const CvPlot* pLoopPlot = GC.getCurrentViewport()->plotByIndex(iI);
		const CvPlotGroup* pPlotGroup = pLoopPlot->getPlotGroup(getID());
		if (pPlotGroup && pLoopPlot->isRevealed(getTeam(), true) && pLoopPlot->getTeam() == getTeam())
		{
			mapPlotGroups[pPlotGroup->getID()].push_back(iI);
		}
	}

	CvRandom kRandom;
	kRandom.init(42);
	for (PlotGroupMap::iterator it = mapPlotGroups.begin(); it != mapPlotGroups.end(); ++it)
	{
		NiColorA kColor(kRandom.getFloat(), kRandom.getFloat(), kRandom.getFloat(), 0.8f);
		foreach_(const int plotIndex, it->second)
		{
			aColors[plotIndex] = kColor;
		}
	}
}

void CvPlayer::getUnitLayerColors(GlobeLayerUnitOptionTypes eOption, std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const
{
	PROFILE_EXTRA_FUNC();
	aColors.resize(GC.getCurrentViewport()->numPlots(), NiColorA(0, 0, 0, 0));
	aIndicators.clear();

	std::vector< std::vector<float> > aafPlayerPlotStrength(MAX_PLAYERS);
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAlive())
		{
			aafPlayerPlotStrength[i].resize(GC.getCurrentViewport()->numPlots());
		}
	}

	float fMaxPlotStrength = 0.0f;

	// create unit plot indicators...
	// build the trade group texture
	//typedef std::map<int, NiColor> GroupMap;
	//GroupMap mapColors;

	// Loop through all the players
	CvWStringBuffer szBuffer;
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		if (GET_PLAYER((PlayerTypes)iPlayer).isAlive())
		{
			for (int iI = 0; iI < GC.getCurrentViewport()->numPlots(); ++iI)
			{
				CvPlot* pLoopPlot = GC.getCurrentViewport()->plotByIndex(iI);
				int iNumUnits = pLoopPlot->getNumUnits();
				float fPlotStrength = 0.0f;

				if (iNumUnits > 0 && pLoopPlot->isVisible(getTeam(), true))
				{
					bool bShowIndicator = false;

					foreach_(const CvUnit* pUnit, pLoopPlot->units())
					{
						if (pUnit->getVisualOwner() == iPlayer && !pUnit->isInvisible(getTeam(), GC.getGame().isDebugMode()))
						{
							// now, is this unit of interest?
							bool bIsMilitary = pUnit->baseCombatStr() > 0;
							bool bIsEnemy = pUnit->isEnemy(getTeam());
							bool bIsOnOurTeam = pUnit->getTeam() == getTeam();
							bool bOfInterest = false;

							switch (eOption)
							{
							case SHOW_ALL_MILITARY:
								{
									bOfInterest = bIsMilitary;
									if (bOfInterest)
									{
										fPlotStrength += ((float) pUnit->getHP() / (float) pUnit->getMaxHP() * (float) pUnit->baseCombatStr());
									}
									break;
								}
							case SHOW_TEAM_MILITARY:
								{
									bOfInterest = bIsMilitary && bIsOnOurTeam;
									if (bOfInterest)
										fPlotStrength += ((float) pUnit->getHP() / (float) pUnit->getMaxHP() * (float) pUnit->baseCombatStr());
									break;
								}
							case SHOW_ENEMIES:
								{
									bOfInterest = bIsMilitary && bIsEnemy;
									if (bOfInterest)
										fPlotStrength += ((float) pUnit->getHP() / (float) pUnit->getMaxHP() * (float) pUnit->baseCombatStr());
									break;
								}

							case SHOW_ENEMIES_IN_TERRITORY:
								{
									bOfInterest = bIsMilitary;
									break;
								}

							case SHOW_PLAYER_DOMESTICS:
								{
									bOfInterest = !bIsMilitary;// && (pUnit->getVisualOwner() == eCurPlayer);
									break;
								}
							default:
								bOfInterest = false;
								break;
							}

							// create the indicator
							if (bOfInterest)
							{
								bShowIndicator = true;
							}

							fMaxPlotStrength = std::max(fPlotStrength, fMaxPlotStrength);
							aafPlayerPlotStrength[iPlayer][iI] = fPlotStrength;
						}
					}

					if (bShowIndicator)
					{
						const CvUnit* pUnit = pLoopPlot->getBestDefender(NO_PLAYER);
						if (pUnit)
						{
							const PlayerColorTypes eUnitColor = GET_PLAYER(pUnit->getVisualOwner()).getPlayerColor();
							const NiColorA& kColor = GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(eUnitColor).getColorTypePrimary()).getColor();

							szBuffer.clear();
							GAMETEXT.setPlotListHelp(szBuffer, pLoopPlot, true, true);

							CvPlotIndicatorData kIndicator;
							kIndicator.m_pUnit = pUnit;
							kIndicator.m_strLabel = "UNITS";
							kIndicator.m_strIcon = pUnit->getButton();

							if (eOption == SHOW_ENEMIES_IN_TERRITORY)
							{
								kIndicator.m_kColor.r = 1;
								kIndicator.m_kColor.g = 0;
								kIndicator.m_kColor.b = 0;
							}
							else
							{
								kIndicator.m_kColor.r = kColor.r;
								kIndicator.m_kColor.g = kColor.g;
								kIndicator.m_kColor.b = kColor.b;
							}
							kIndicator.m_strHelpText = szBuffer.getCString();

							//setup visibility
							switch (eOption)
							{
							case SHOW_ENEMIES_IN_TERRITORY:
								kIndicator.m_bTestEnemyVisibility = true;
								kIndicator.m_eVisibility = PLOT_INDICATOR_VISIBLE_ALWAYS;
								break;
							case SHOW_ENEMIES:
								kIndicator.m_eVisibility = PLOT_INDICATOR_VISIBLE_ALWAYS;
								break;
							default:
								kIndicator.m_eVisibility = PLOT_INDICATOR_VISIBLE_ONSCREEN_ONLY;
								break;
							}
							aIndicators.push_back(kIndicator);
						}
					}
				}
			}
		}
	}

	if (fMaxPlotStrength > 0)
	{
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
		{
			if (GET_PLAYER((PlayerTypes)iPlayer).isAlive())
			{
				const PlayerColorTypes eCurPlayerColor = GET_PLAYER((PlayerTypes) iPlayer).getPlayerColor();
				const NiColorA& kColor = GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(eCurPlayerColor).getColorTypePrimary()).getColor();

				for (int iI = 0; iI < GC.getCurrentViewport()->numPlots(); iI++)
				{
					const CvPlot* pLoopPlot = GC.getCurrentViewport()->plotByIndex(iI);
					if (pLoopPlot->isVisible(getTeam(), true))
					{
						const float fPlotStrength = aafPlayerPlotStrength[iPlayer][iI];
						if (fPlotStrength > 0)
						{
							const float fAlpha = (fPlotStrength / fMaxPlotStrength * 0.75f + 0.25f) * 0.8f;
							if (fAlpha > aColors[iI].a)
							{
								aColors[iI] = kColor;
								aColors[iI].a = fAlpha;
							}
						}
					}
				}
			}
		}
	}
}

void CvPlayer::getResourceLayerColors(GlobeLayerResourceOptionTypes eOption, std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const
{
	PROFILE_EXTRA_FUNC();
	aColors.clear();
	aIndicators.clear();

	CvWStringBuffer szBuffer;
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		const CvPlot* plotX = GC.getMap().plotByIndex(iI);

		if (plotX->isRevealed(getTeam(), true) && plotX->isInViewport())
		{
			const PlayerTypes eOwner = plotX->getRevealedOwner(getTeam(), true);
			const BonusTypes eCurType = plotX->getBonusType((GC.getGame().isDebugMode()) ? NO_TEAM : getTeam());
			if (eCurType != NO_BONUS)
			{
				const CvBonusInfo& kBonusInfo = GC.getBonusInfo(eCurType);

				bool bOfInterest = false;
				switch (eOption)
				{
				case SHOW_RESOURCES_ALL:
					bOfInterest = true;
					break;
				case SHOW_RESOURCES_STRATEGIC:
					bOfInterest = kBonusInfo.getBonusClassType() == GC.getInfoTypeForString("BONUSCLASS_STRATEGIC");
					break;
				case SHOW_RESOURCES_LUXURY:
					bOfInterest = kBonusInfo.getHappiness() > 0 && kBonusInfo.getHealth() < 1
						|| kBonusInfo.getBonusClassType() == GC.getInfoTypeForString("BONUSCLASS_LUXURY");
					break;
				case SHOW_RESOURCES_PRODUCTION:
					bOfInterest = kBonusInfo.getBonusClassType() == GC.getInfoTypeForString("BONUSCLASS_PRODUCTION");
					break;
				case SHOW_RESOURCES_GROWTH:
					bOfInterest = kBonusInfo.getHealth() > 0 && kBonusInfo.getHappiness() < 1
						|| kBonusInfo.getBonusClassType() == GC.getInfoTypeForString("BONUSCLASS_CROP")
						|| kBonusInfo.getBonusClassType() == GC.getInfoTypeForString("BONUSCLASS_LIVESTOCK")
						|| kBonusInfo.getBonusClassType() == GC.getInfoTypeForString("BONUSCLASS_SEAFOOD");
					break;
				case SHOW_RESOURCES_MISC:
					bOfInterest = kBonusInfo.getBonusClassType() == GC.getInfoTypeForString("BONUSCLASS_MISC");
					break;
				case SHOW_RESOURCES_UNCLAIMED:
					bOfInterest = !plotX->isBonusExtracted(getTeam());
					break;
				case SHOW_RESOURCES_CANCLAIM:
					bOfInterest = !plotX->isBonusExtracted(getTeam()) && eOwner != NO_PLAYER && GET_TEAM(GET_PLAYER(eOwner).getTeam()).isHasTech((TechTypes)kBonusInfo.getTechCityTrade());
					break;
				}

				if (bOfInterest)
				{
					CvPlotIndicatorData kData;
					kData.m_strLabel = "RESOURCES";
					kData.m_eVisibility = PLOT_INDICATOR_VISIBLE_ONSCREEN_ONLY;
					kData.m_strIcon = GC.getBonusInfo(eCurType).getButton();

					int x = plotX->getViewportX();
					int y = plotX->getViewportY();
					kData.m_Target = NiPoint2(GC.getCurrentViewport()->plotXToPointX(x), GC.getCurrentViewport()->plotYToPointY(y));

					if (eOwner == NO_PLAYER)
					{
						kData.m_kColor.r = 0.8f;
						kData.m_kColor.g = 0.8f;
						kData.m_kColor.b = 0.8f;
					}
					else
					{
						PlayerColorTypes eCurPlayerColor = GET_PLAYER(eOwner).getPlayerColor();
						const NiColorA& kColor = GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(eCurPlayerColor).getColorTypePrimary()).getColor();
						kData.m_kColor.r = kColor.r;
						kData.m_kColor.g = kColor.g;
						kData.m_kColor.b = kColor.b;
					}

					szBuffer.clear();
					GAMETEXT.setBonusHelp(szBuffer, eCurType, false);
					kData.m_strHelpText = szBuffer.getCString();

					aIndicators.push_back(kData);
				}
			}
		}
	}
}

void CvPlayer::getReligionLayerColors(ReligionTypes eSelectedReligion, std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const
{
	PROFILE_EXTRA_FUNC();
	aColors.resize(GC.getCurrentViewport()->numPlots(), NiColorA(0, 0, 0, 0));
	aIndicators.clear();

	CvRandom kRandom;
	kRandom.init(42 * eSelectedReligion);
	const NiColorA kBaseColor(kRandom.getFloat(), kRandom.getFloat(), kRandom.getFloat(), 1.0f);

	for (int iI = 0; iI  < MAX_PLAYERS; iI ++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			foreach_(const CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities()
			| filtered(CvCity::fn::isRevealed(getTeam(), true) && CvCity::fn::isHasReligion(eSelectedReligion)))
			{
				float fAlpha = 0.8f;
				if (!pLoopCity->isHolyCity(eSelectedReligion))
				{
					fAlpha *= 0.5f;
				}

				foreach_(const CvPlot* pLoopPlot, pLoopCity->plots(NUM_CITY_PLOTS)
				| filtered(CvPlot::fn::isRevealed(getTeam(), true) && CvPlot::fn::isInViewport()))
				{
					const int iIndex = GC.getCurrentViewport()->plotNum(pLoopPlot->getViewportX(), pLoopPlot->getViewportY());
					if (fAlpha > aColors[iIndex].a)
					{
						aColors[iIndex] = kBaseColor;
						aColors[iIndex].a = fAlpha;
					}
				}
			}
		}
	}
}

void CvPlayer::getCultureLayerColors(std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const
{
	PROFILE_EXTRA_FUNC();
	const int iColorsPerPlot = 4;
	aColors.resize(GC.getCurrentViewport()->numPlots() * iColorsPerPlot, NiColorA(0, 0, 0, 0));
	aIndicators.clear();

	// find maximum total culture
	int iMaxTotalCulture = INT_MIN;
	int iMinTotalCulture = INT_MAX;
	for (int iI = 0; iI < GC.getCurrentViewport()->numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getCurrentViewport()->plotByIndex(iI);
		int iTotalCulture = pLoopPlot->countTotalCulture();
		if (iTotalCulture > iMaxTotalCulture)
		{
			iMaxTotalCulture = iTotalCulture;
		}
		if (iTotalCulture < iMinTotalCulture && iTotalCulture > 0)
		{
			iMinTotalCulture = iTotalCulture;
		}
	}
	iMinTotalCulture = 0;

	// find culture percentages
	for (int iI = 0; iI < GC.getCurrentViewport()->numPlots(); iI++)
	{
		const CvPlot* pLoopPlot = GC.getCurrentViewport()->plotByIndex(iI);
		//PlayerTypes eOwner = pLoopPlot->getRevealedOwner(getTeam(), true);

		// how many people own this plot?
		std::vector < std::pair<int,int> > plot_owners;
		int iNumNonzeroOwners = 0;
		for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; iPlayer++)
		{
			if (GET_PLAYER((PlayerTypes)iPlayer).isAlive())
			{
				int iCurCultureAmount = pLoopPlot->getCulture((PlayerTypes)iPlayer);
				if (iCurCultureAmount != 0)
				{
					iNumNonzeroOwners ++;
					plot_owners.push_back(std::pair<int,int>(iCurCultureAmount, iPlayer));
				}
			}
		}

		// ensure that it is revealed
		if (!plot_owners.empty() && pLoopPlot->getRevealedOwner(getTeam(), true) != NO_PLAYER)
		{
			for (int i = 0; i < iColorsPerPlot; ++i)
			{
				int iCurOwnerIdx = i % plot_owners.size();
				PlayerTypes eCurOwnerID = (PlayerTypes) plot_owners[iCurOwnerIdx].second;
				int iCurCulture = plot_owners[iCurOwnerIdx].first;
				const NiColorA& kCurColor = GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(GET_PLAYER(eCurOwnerID).getPlayerColor()).getColorTypePrimary()).getColor();

				// damp the color by the value...
				aColors[iI * iColorsPerPlot + i] = kCurColor;
				float blend_factor = 0.5f * std::min(1.0f, std::max(0.0f, (float)(iCurCulture - iMinTotalCulture) / iMaxTotalCulture));
				aColors[iI * iColorsPerPlot + i].a = std::min(0.8f * blend_factor + 0.5f, 1.0f);
			}
		}
	}
}

void CvPlayer::cheat(bool bCtrl, bool bAlt, bool bShift)
{
	if (gDLL->getChtLvl() > 0 || GC.getGame().isDebugMode())
	{
		GET_TEAM(getTeam()).setHasTech(getCurrentResearch(), true, getID(), true, false);
	}
}

const CvArtInfoUnit* CvPlayer::getUnitArtInfo(UnitTypes eUnit, int iMeshGroup) const
{
	CivilizationTypes eCivilization = getCivilizationType();
	if (eCivilization == NO_CIVILIZATION)
	{
		eCivilization = (CivilizationTypes) GC.getDefineINT("BARBARIAN_CIVILIZATION");
	}
	UnitArtStyleTypes eStyle = (UnitArtStyleTypes) GC.getCivilizationInfo(eCivilization).getUnitArtStyleType();
	EraTypes eEra = getCurrentEra();
	if (eEra == NO_ERA)
	{
		eEra = (EraTypes) 0;
	}
	return GC.getUnitInfo(eUnit).getArtInfo(iMeshGroup, eEra, eStyle);
}

bool CvPlayer::hasSpaceshipArrived() const
{
	VictoryTypes eSpaceVictory = GC.getGame().getSpaceVictory();
	if (eSpaceVictory != NO_VICTORY)
	{
		int iVictoryCountdown = GET_TEAM(getTeam()).getVictoryCountdown(eSpaceVictory);
		if (((GC.getGame().getGameState() == GAMESTATE_EXTENDED) && (iVictoryCountdown > 0)) || (iVictoryCountdown == 0))
		{
			return true;
		}
	}

	return false;
}

int CvPlayer::getEnslavementChance() const
{
	return m_iEnslavementChance;
}

void CvPlayer::changeEnslavementChance(int iChange)
{
	if (iChange != 0)
	{
		m_iEnslavementChance += iChange;
	}
}

bool CvPlayer::isTradingMilitaryBonus(PlayerTypes ePlayer) const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		if (GET_PLAYER(ePlayer).AI_militaryBonusVal((BonusTypes)iI) > 0)
		{
			if (getNumTradeImportsByBonus(ePlayer, ((BonusTypes)iI)) > 0)
			{
				return true;
			}
		}
	}
	return false;
}

int CvPlayer::getNumTradeImportsByBonus(PlayerTypes ePlayer, BonusTypes eBonus) const
{
	PROFILE_EXTRA_FUNC();
	CLLNode<TradeData>* pNode;

	FAssert(ePlayer != getID());

	int iCount = 0;

	foreach_(const CvDeal& kLoopDeal, GC.getGame().deals())
	{
		if ((kLoopDeal.getFirstPlayer() == getID()) && (kLoopDeal.getSecondPlayer() == ePlayer))
		{
			for (pNode = kLoopDeal.headSecondTradesNode(); pNode; pNode = kLoopDeal.nextSecondTradesNode(pNode))
			{
				if (pNode->m_data.m_eItemType == TRADE_RESOURCES)
				{
					if (pNode->m_data.m_iData == eBonus)
					{
						iCount++;
					}
				}
			}
		}

		if ((kLoopDeal.getFirstPlayer() == ePlayer) && (kLoopDeal.getSecondPlayer() == getID()))
		{
			for (pNode = kLoopDeal.headFirstTradesNode(); pNode; pNode = kLoopDeal.nextFirstTradesNode(pNode))
			{
				if (pNode->m_data.m_eItemType == TRADE_RESOURCES)
				{
					if (pNode->m_data.m_iData == eBonus)
					{
						iCount++;
					}
				}
			}
		}
	}

	return iCount;
}

CvCity* CvPlayer::getBestHQCity(CorporationTypes eCorporation) const
{
	PROFILE_EXTRA_FUNC();
	CvCity* pCurrentHQ = GC.getGame().getHeadquarters(eCorporation);
	CvCity* pBestCity = NULL;
	int iBestValue = 0;

	if (GC.getCorporationInfo(eCorporation).getObsoleteTech() != NO_TECH)
	{
		if (GET_TEAM(getTeam()).isHasTech(GC.getCorporationInfo(eCorporation).getObsoleteTech()))
			return NULL;
	}
	foreach_(CvCity* pLoopCity, cities())
	{
		int iValue = 0;
		//If we find no best city, the capital will do.
		if (pLoopCity->isCapital())
			pBestCity = pLoopCity;
		if (!pLoopCity->isHasCorporation(eCorporation))
		{
			if (pLoopCity->isConnectedTo(pCurrentHQ))
			{
				for (int iCorporation = 0; iCorporation < GC.getNumCorporationInfos(); ++iCorporation)
				{
					if (pLoopCity->isHeadquarters((CorporationTypes)iCorporation))
					{
						if (GC.getGame().isCompetingCorporation((CorporationTypes)iCorporation, eCorporation))
						{
							continue;
						}
						iValue -= 25;
					}
				}

				bool bFoundBonus = false;
				foreach_(const BonusTypes eBonus, GC.getCorporationInfo(eCorporation).getPrereqBonuses())
				{
					if (pLoopCity->hasBonus(eBonus))
					{
						bFoundBonus = true;
						iValue += 50;
					}
				}

				if (!bFoundBonus)
				{
					continue;
				}

				iValue += 100;
			}
		}
		else
		{
			iValue = 300;

			if (pLoopCity->isConnectedTo(pCurrentHQ))
			{
				iValue += 100;
			}

			foreach_(const BonusTypes eBonus, GC.getCorporationInfo(eCorporation).getPrereqBonuses())
			{
				if (pLoopCity->hasBonus(eBonus))
				{
					iValue += 50;
				}
			}

			for (int iCorporation = 0; iCorporation < GC.getNumCorporationInfos(); ++iCorporation)
			{
				if (pLoopCity->isHeadquarters((CorporationTypes)iCorporation))
				{
					iValue -= 25;
				}
			}
		}

		if (iValue > iBestValue)
		{
			iBestValue = iValue;
			pBestCity = pLoopCity;
		}
	}

	return pBestCity;
}


PlayerVoteTypes CvPlayer::getPledgedVote() const
{
	return (PlayerVoteTypes)m_ePledgedVote;
}

void CvPlayer::setPledgedVote(PlayerVoteTypes eIndex)
{
	m_ePledgedVote = eIndex;
}

TeamTypes CvPlayer::getPledgedSecretaryGeneralVote() const
{
	return (TeamTypes)m_eSecretaryGeneralVote;
}

void CvPlayer::setPledgedSecretaryGeneralVote(TeamTypes eIndex)
{
	m_eSecretaryGeneralVote = eIndex;
}

DenialTypes CvPlayer::AI_pledgeVoteTrade(VoteTriggeredData* kData, PlayerVoteTypes ePlayerVote, PlayerTypes ePlayer) const
{
	if (GET_TEAM(getTeam()).AI_getWorstEnemy() == GET_PLAYER(ePlayer).getTeam())
	{
		return DENIAL_WORST_ENEMY;
	}

	return NO_DENIAL;
}

DenialTypes CvPlayer::AI_corporationTrade(CorporationTypes eCorporation, PlayerTypes ePlayer) const
{
	PROFILE_EXTRA_FUNC();
	if (isNoCorporations())
	{
		return DENIAL_NO_GAIN;
	}

	bool bValid = false;
	bool bRequiresBonus = false;

	foreach_(const CvCity* pLoopCity, cities())
	{
		//If we don't have the corporation
		if (!(pLoopCity->isHasCorporation(eCorporation)))
		{
			foreach_(const BonusTypes eBonus, GC.getCorporationInfo(eCorporation).getPrereqBonuses())
			{
				bRequiresBonus = true;
				//if the city can have the corporation
				if (pLoopCity->hasBonus(eBonus))
				{
					bValid = true;
				}
			}
		}
	}

	if (!bValid && bRequiresBonus)
	{
		return DENIAL_NO_GAIN;
	}

	if (GET_PLAYER(ePlayer).getTeam() == getTeam())
	{
		return NO_DENIAL;
	}

	if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(getTeam()))
	{
		return NO_DENIAL;
	}

	if (GET_TEAM(getTeam()).isVassal(GET_PLAYER(ePlayer).getTeam()))
	{
		return NO_DENIAL;
	}

	if (isHumanPlayer())
	{
		return NO_DENIAL;
	}

	if (GET_TEAM(getTeam()).AI_getWorstEnemy() == GET_PLAYER(ePlayer).getTeam())
	{
		return DENIAL_WORST_ENEMY;
	}

	if ((getHasCorporationCount(eCorporation) * 2) > getNumCities())
	{
		return DENIAL_NO_GAIN;
	}

	if (GET_PLAYER(ePlayer).isNoCorporations())
	{
		return DENIAL_NO_GAIN;
	}

	if (AI_getAttitude(ePlayer) <= GC.getLeaderHeadInfo(getPersonalityType()).getCorporationRefuseAttitudeThreshold())
	{
		return DENIAL_ATTITUDE;
	}

	return NO_DENIAL;
}

DenialTypes CvPlayer::AI_secretaryGeneralTrade(VoteSourceTypes eVoteSource, PlayerTypes ePlayer) const
{

	if (GET_PLAYER(ePlayer).getTeam() == getTeam())
	{
		return NO_DENIAL;
	}

	if (GET_TEAM(getTeam()).isVassal(GET_PLAYER(ePlayer).getTeam()))
	{
		return NO_DENIAL;
	}

	if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(getTeam()))
	{
		return NO_DENIAL;
	}

	if (GET_TEAM(getTeam()).AI_getWorstEnemy() == GET_PLAYER(ePlayer).getTeam())
	{
		return DENIAL_WORST_ENEMY;
	}

	if (GC.getGame().isTeamVoteEligible(getTeam(), eVoteSource))
	{
		return DENIAL_JOKING;
	}

	if (AI_getAttitude(ePlayer) <= GC.getLeaderHeadInfo(getPersonalityType()).getSecretaryGeneralVoteRefuseAttitudeThreshold())
	{
		return DENIAL_ATTITUDE;
	}

	return NO_DENIAL;
}

DenialTypes CvPlayer::AI_workerTrade(const CvUnit* pUnit, PlayerTypes ePlayer) const
{

	if (isHumanPlayer())
	{
		return NO_DENIAL;
	}

	if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(getTeam()))
	{
		return NO_DENIAL;
	}

	if (GET_TEAM(getTeam()).isVassal(GET_PLAYER(ePlayer).getTeam()))
	{
		return NO_DENIAL;
	}

	if (GET_PLAYER(ePlayer).getTeam() == getTeam())
	{
		return NO_DENIAL;
	}

	if (pUnit->getOriginalOwner() == ePlayer)
	{
		return DENIAL_JOKING;
	}

	if (GET_TEAM(getTeam()).AI_getWorstEnemy() == GET_PLAYER(ePlayer).getTeam())
	{
		return DENIAL_WORST_ENEMY;
	}

	if (GET_TEAM(getTeam()).AI_isSneakAttackPreparing(GET_PLAYER(ePlayer).getTeam()))
	{
		return DENIAL_NO_GAIN;
	}

	if (GET_PLAYER(ePlayer).AI_totalUnitAIs(UNITAI_WORKER) > GET_PLAYER(ePlayer).getNumCities()
	|| getUnitUpkeepNet(pUnit->isMilitaryBranch(), pUnit->getUpkeep100()) < 1)
	{
		return DENIAL_NO_GAIN;
	}

	const AttitudeTypes eAttitude = AI_getAttitude(ePlayer);

	if (eAttitude <= GC.getLeaderHeadInfo(getPersonalityType()).getWorkerRefuseAttitudeThreshold())
	{
		return DENIAL_ATTITUDE;
	}

	return NO_DENIAL;
}

DenialTypes CvPlayer::AI_militaryUnitTrade(const CvUnit* pUnit, PlayerTypes ePlayer) const
{

	if (isHumanPlayer())
	{
		return NO_DENIAL;
	}

	if (GET_PLAYER(ePlayer).getTeam() == getTeam())
	{
		return NO_DENIAL;
	}

	if (GET_TEAM(getTeam()).isVassal(GET_PLAYER(ePlayer).getTeam()))
	{
		return NO_DENIAL;
	}

	if (GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(getTeam()))
	{
		return NO_DENIAL;
	}

	if (GET_TEAM(getTeam()).AI_isSneakAttackPreparing(GET_PLAYER(ePlayer).getTeam()))
	{
		return DENIAL_NO_GAIN;
	}

	if (GET_PLAYER(pUnit->getOriginalOwner()).getID() == GET_PLAYER(ePlayer).getID())
	{
		return DENIAL_JOKING;
	}

	if (GET_TEAM(getTeam()).hasWarPlan(true) && !GET_TEAM(getTeam()).AI_shareWar(GET_PLAYER(ePlayer).getTeam()))
	{
		return DENIAL_NO_GAIN;
	}

	if (getUnitUpkeepNet(pUnit->isMilitaryBranch(), pUnit->getUpkeep100()) < 1)
	{
		return DENIAL_NO_GAIN;
	}

	if (pUnit->nukeRange() > 0 && GET_PLAYER(ePlayer).getNumNukeUnits() == 0)
	{
		return DENIAL_JOKING;
	}

	if (AI_getAttitude(ePlayer) <= GC.getLeaderHeadInfo(getPersonalityType()).getMilitaryUnitRefuseAttitudeThreshold())
	{
		return DENIAL_ATTITUDE;
	}

	return NO_DENIAL;
}

bool CvPlayer::hasValidCivics(BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	bool bValidOrCivic = false;
	bool bOrReq = false;

	for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		if (GC.getBuildingInfo(eBuilding).isPrereqOrCivics(iI))
		{
			bOrReq = true;
			if (isCivic(CivicTypes(iI)))
			{
				bValidOrCivic = true;
			}
		}

		if (GC.getBuildingInfo(eBuilding).isPrereqAndCivics(iI) && !isCivic(CivicTypes(iI)))
		{
			return false;
		}
	}
	if (bOrReq && !bValidOrCivic)
	{
		return false;
	}
	return true;
}


void CvPlayer::changeBonusMintedPercent(const BonusTypes eBonus, const int iChange)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eBonus);

	if (iChange == 0)
	{
		return;
	}
	// Process bonus out of cities before change
	foreach_(CvCity* city, cities())
	{
		if (city->getNumBonuses(eBonus) > 0)
		{
			city->processBonus(eBonus, -1);
		}
	}
	std::map<short, int>::const_iterator itr = m_bonusMintedPercent.find((short)eBonus);

	if (itr == m_bonusMintedPercent.end())
	{
		m_bonusMintedPercent.insert(std::make_pair((short)eBonus, iChange));
	}
	else if (itr->second == -iChange)
	{
		m_bonusMintedPercent.erase(itr->first);
	}
	else // change minted percent
	{
		m_bonusMintedPercent[itr->first] += iChange;
	}
	// Process bonus back into cities after change
	foreach_(CvCity* city, cities())
	{
		if (city->getNumBonuses(eBonus) > 0)
		{
			city->processBonus(eBonus, 1);
		}
		city->updateCorporation();
		city->AI_setAssignWorkDirty(true);
		city->setInfoDirty(true);
	}
}

int CvPlayer::getBonusMintedPercent(const BonusTypes eBonus) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eBonus);
	std::map<short, int>::const_iterator itr = m_bonusMintedPercent.find((short)eBonus);
	return itr != m_bonusMintedPercent.end() ? itr->second : 0;
}


void CvPlayer::changeUnitCombatFreeExperience(const UnitCombatTypes eIndex, const int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eIndex);
	if (iChange == 0)
	{
		return;
	}
	std::map<short, short>::const_iterator itr = m_unitCombatFreeXP.find((short)eIndex);

	if (itr == m_unitCombatFreeXP.end())
	{
		m_unitCombatFreeXP.insert(std::make_pair((short)eIndex, iChange));
	}
	else if (itr->second == -iChange)
	{
		m_unitCombatFreeXP.erase(itr->first);
	}
	else // change mod
	{
		m_unitCombatFreeXP[itr->first] += iChange;
	}
}

int CvPlayer::getUnitCombatFreeExperience(const UnitCombatTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eIndex);
	std::map<short, short>::const_iterator itr = m_unitCombatFreeXP.find((short)eIndex);
	return itr != m_unitCombatFreeXP.end() ? itr->second : 0;
}


void CvPlayer::changeBuildingProductionModifier(const BuildingTypes eIndex, const int iChange)
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

int CvPlayer::getBuildingProductionModifier(const BuildingTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	std::map<short, int>::const_iterator itr = m_buildingProductionMod.find((short)eIndex);
	return itr != m_buildingProductionMod.end() ? itr->second : 0;
}


void CvPlayer::changeBuildingCostModifier(const BuildingTypes eIndex, const int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	if (iChange == 0)
	{
		return;
	}
	std::map<short, int>::const_iterator itr = m_buildingCostMod.find((short)eIndex);

	if (itr == m_buildingCostMod.end())
	{
		m_buildingCostMod.insert(std::make_pair((short)eIndex, iChange));
	}
	else if (itr->second == -iChange)
	{
		m_buildingCostMod.erase(itr->first);
	}
	else // change building mod
	{
		m_buildingCostMod[itr->first] += iChange;
	}
}

int CvPlayer::getBuildingCostModifier(const BuildingTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	std::map<short, int>::const_iterator itr = m_buildingCostMod.find((short)eIndex);
	return itr != m_buildingCostMod.end() ? itr->second : 0;
}


void CvPlayer::changeUnitProductionModifier(const UnitTypes eUnit, const int iChange)
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

int CvPlayer::getUnitProductionModifier(const UnitTypes eUnit) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);
	std::map<short, int>::const_iterator itr = m_unitProductionMod.find((short)eUnit);
	return itr != m_unitProductionMod.end() ? itr->second : 0;
}


void CvPlayer::changeUnitCombatProductionModifier(const UnitCombatTypes eIndex, const int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eIndex);
	if (iChange == 0)
	{
		return;
	}
	std::map<short, int>::const_iterator itr = m_unitCombatProductionMod.find((short)eIndex);

	if (itr == m_unitCombatProductionMod.end())
	{
		m_unitCombatProductionMod.insert(std::make_pair((short)eIndex, iChange));
	}
	else if (itr->second == -iChange)
	{
		m_unitCombatProductionMod.erase(itr->first);
	}
	else // change mod
	{
		m_unitCombatProductionMod[itr->first] += iChange;
	}
}

int CvPlayer::getUnitCombatProductionModifier(const UnitCombatTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), eIndex);
	std::map<short, int>::const_iterator itr = m_unitCombatProductionMod.find((short)eIndex);
	return itr != m_unitCombatProductionMod.end() ? itr->second : 0;
}


bool CvPlayer::isAutomatedCanBuild(BuildTypes eBuild) const
{
	return m_pabAutomatedCanBuild[eBuild];
}

void CvPlayer::setAutomatedCanBuild(BuildTypes eBuild, bool bNewValue)
{
	m_pabAutomatedCanBuild[eBuild] = bNewValue;
}

bool CvPlayer::hasEnemyDefenderUnit(const CvPlot* pPlot) const
{
	PROFILE_EXTRA_FUNC();
	std::vector<CvUnit *> plotUnits;

	GC.getGame().getPlotUnits(pPlot, plotUnits);

	foreach_(CvUnit* pLoopUnit, plotUnits)
	{
		if (atWar(getTeam(), GET_PLAYER(pLoopUnit->getOwner()).getTeam()))
		{
			return true;
		}
	}

	return false;
}

int CvPlayer::getCivicHappiness() const
{
	return m_iCivicHappiness;
}

void CvPlayer::changeCivicHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iCivicHappiness = (m_iCivicHappiness + iChange);

		AI_makeAssignWorkDirty();
	}
}

int CvPlayer::getForeignTradeRouteModifier() const
{
	return m_iForeignTradeRouteModifier;
}

void CvPlayer::changeForeignTradeRouteModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iForeignTradeRouteModifier = (m_iForeignTradeRouteModifier + iChange);

		updateTradeRoutes();
	}
}

int CvPlayer::getTaxRateUnhappiness() const
{
	return m_iTaxRateUnhappiness;
}

void CvPlayer::changeTaxRateUnhappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iTaxRateUnhappiness = (m_iTaxRateUnhappiness + iChange);
	}
}

int CvPlayer::calculateTaxRateUnhappiness() const
{
	return (getCommercePercent(COMMERCE_GOLD) * getTaxRateUnhappiness() / 100);
}

//Sevo Begin -VCM
/*  getSevoWonderScore
This function returns the number of wonders BUILT by the playerID (not OWNED!)

If Mode == 1, it will return the total number of Wonders BUILT in the world.
If Mode == 2, it will return the total number of Wonders available to build in the game.
Else, it will return the total number of Wonders BUILT by this player.
*/
int CvPlayer::getSevoWondersScore(int mode)
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iJ = GC.getNumBuildingInfos() - 1; iJ > -1; iJ--)
	{
		if (isWorldWonder((BuildingTypes)iJ))
		{
			if (mode == 2)
			{
				iCount++;
				continue;
			}
			bool bFound = false;

			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive() && !GET_PLAYER((PlayerTypes)iI).isMinorCiv())
				{
					foreach_(const CvCity* cityX, GET_PLAYER((PlayerTypes)iI).cities())
					{
						if (cityX->hasBuilding((BuildingTypes)iJ))
						{
							if (mode == 1 || cityX->getBuildingData((BuildingTypes)iJ).eBuiltBy == getID())
							{
								iCount++;
							}
							bFound = true; break;
						}
					}
					if (bFound) break;
				}
			}
		}
	}
	return iCount;
}


void CvPlayer::recalculatePopulationgrowthratepercentage()
{
	PROFILE_FUNC();

	m_fPopulationgrowthratepercentageLog = 0;

	for (int iI = GC.getNumBuildingInfos() - 1; iI > -1; iI--)
	{
		const BuildingTypes eType = static_cast<BuildingTypes>(iI);

		if (GC.getBuildingInfo(eType).getGlobalPopulationgrowthratepercentage() != 0)
		{
			foreach_(const CvCity* city, cities())
			{
				if (city->isActiveBuilding(eType))
				{
					changePopulationgrowthratepercentage(GC.getBuildingInfo(eType).getGlobalPopulationgrowthratepercentage(),true);
				}
			}
		}
	}

	for(int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		const CivicTypes eCivic = getCivics((CivicOptionTypes)iI);

		if (eCivic != NO_CIVIC && GC.getCivicInfo(eCivic).getPopulationgrowthratepercentage() != 0)
		{
			changePopulationgrowthratepercentage(GC.getCivicInfo(eCivic).getPopulationgrowthratepercentage(),true);
		}
	}

	for(int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		const TraitTypes eTrait = static_cast<TraitTypes>(iI);
		if (hasTrait(eTrait) && GC.getTraitInfo(eTrait).getGlobalPopulationgrowthratepercentage() != 0)
		{
			changePopulationgrowthratepercentage(GC.getTraitInfo(eTrait).getGlobalPopulationgrowthratepercentage(),true);
		}
	}
}

int CvPlayer::getPopulationgrowthratepercentage() const
{
	return (int)(exp(m_fPopulationgrowthratepercentageLog)*100 - 100);
}

void CvPlayer::setPopulationgrowthratepercentage(int iNewValue)
{
	m_fPopulationgrowthratepercentageLog = log((100+(float)iNewValue)/100);
}

void CvPlayer::changePopulationgrowthratepercentage(int iChange, bool bAdd)
{
	m_fPopulationgrowthratepercentageLog += (bAdd ? 1 : -1)*log((100+(float)iChange)/100);
}

int CvPlayer::getReligionSpreadRate() const
{
	return m_iReligionSpreadRate;
}

void CvPlayer::changeReligionSpreadRate(int iChange)
{
	m_iReligionSpreadRate += iChange;
	FASSERT_NOT_NEGATIVE(m_iReligionSpreadRate);
}

int CvPlayer::getDistantUnitSupportCostModifier() const
{
	return m_iDistantUnitSupportCostModifier;
}

void CvPlayer::setDistantUnitSupportCostModifier(int iValue)
{
	m_iDistantUnitSupportCostModifier = iValue;
}

void CvPlayer::changeDistantUnitSupportCostModifier(int iChange)
{
	setDistantUnitSupportCostModifier(getDistantUnitSupportCostModifier() + iChange);
}

int CvPlayer::getExtraCityDefense() const
{
	return m_iExtraCityDefense;
}

void CvPlayer::setExtraCityDefense(int iValue)
{
	m_iExtraCityDefense = iValue;
}

void CvPlayer::changeExtraCityDefense(int iChange)
{
	setExtraCityDefense(getExtraCityDefense() + iChange);
}

int CvPlayer::getWorldHappiness() const
{
	return m_iWorldHappiness;
}

void CvPlayer::changeWorldHappiness(int iChange)
{
	m_iWorldHappiness += iChange;
}

int CvPlayer::getWorldHealth() const
{
	return m_iWorldHealth;
}

void CvPlayer::changeWorldHealth(int iChange)
{
	m_iWorldHealth += iChange;
}

int CvPlayer::getProjectHappiness() const
{
	return m_iProjectHappiness;
}

void CvPlayer::changeProjectHappiness(int iChange)
{
	m_iProjectHappiness += iChange;
}

int CvPlayer::getProjectHealth() const
{
	return m_iProjectHealth;
}

void CvPlayer::changeProjectHealth(int iChange)
{
	m_iProjectHealth += iChange;
}

int CvPlayer::getForceAllTradeRoutes() const
{
	return m_iForceAllTradeRoutes;
}

void CvPlayer::changeForceAllTradeRoutes(int iChange)
{
	if (iChange != 0)
	{
		m_iForceAllTradeRoutes += iChange;

		updateTradeRoutes();
	}
}

void CvPlayer::changeNoCapitalUnhappiness(int iChange)
{
	if (iChange != 0)
	{
		const bool bWasTrue = isNoCapitalUnhappiness();

		m_iNoCapitalUnhappiness += iChange;

		if (bWasTrue != isNoCapitalUnhappiness())
		{
			CvCity* pCapitalCity = getCapitalCity();

			if (pCapitalCity)
			{
				pCapitalCity->AI_setAssignWorkDirty(true);

				if (pCapitalCity->getTeam() == GC.getGame().getActiveTeam())
				{
					pCapitalCity->setInfoDirty(true);
				}
			}
		}
	}
}

int CvPlayer::getFreeSpecialistCount(SpecialistTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex);
	return m_paiFreeSpecialistCount[eIndex];
}

void CvPlayer::setFreeSpecialistCount(SpecialistTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex);

	const int iOldValue = getFreeSpecialistCount(eIndex);

	if (iOldValue != iNewValue)
	{
		m_paiFreeSpecialistCount[eIndex] = iNewValue;
		FASSERT_NOT_NEGATIVE(getFreeSpecialistCount(eIndex));

		algo::for_each(cities(), CvCity::fn::changeFreeSpecialistCount(eIndex, iNewValue - iOldValue));
	}
}

void CvPlayer::changeFreeSpecialistCount(SpecialistTypes eIndex, int iChange)
{
	setFreeSpecialistCount(eIndex, (getFreeSpecialistCount(eIndex) + iChange));
}

int CvPlayer::getTerrainYieldChange(TerrainTypes eIndex1, YieldTypes eIndex2) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), eIndex1);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex2);
	return m_ppaaiTerrainYieldChange[eIndex1][eIndex2];
}

void CvPlayer::changeTerrainYieldChange(TerrainTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), eIndex1);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex2);

	if (iChange != 0)
	{
		m_ppaaiTerrainYieldChange[eIndex1][eIndex2] += iChange;

		updateYield();
	}
}

int CvPlayer::doMultipleResearch(int iOverflow)
{
	PROFILE_EXTRA_FUNC();
	TechTypes eCurrentTech = getCurrentResearch();

	FAssertMsg(eCurrentTech < GC.getNumTechInfos(), "eCurrentTech is expected to be within maximum bounds (invalid Index)");

	if (eCurrentTech == NO_TECH || GET_TEAM(getTeam()).isHasTech(eCurrentTech))
	{
		if (!isHumanPlayer())
		{
			AI_chooseResearch();
			eCurrentTech = getCurrentResearch();
		}
		else // This shouldn't really happen but it's  safety precaution
		{
			clearResearchQueue();
		}
	}

	while (eCurrentTech != NO_TECH
	&& getModifiedIntValue(GET_TEAM(getTeam()).getResearchCost(eCurrentTech) - GET_TEAM(getTeam()).getResearchProgress(eCurrentTech), -calculateResearchModifier(eCurrentTech)) <= iOverflow)
	{
		//The Future Tech can cause strange infinite loops
		if (GC.getTechInfo(eCurrentTech).isRepeat()) break;

		iOverflow -= getModifiedIntValue(
			GET_TEAM(getTeam()).getResearchCost(eCurrentTech) - GET_TEAM(getTeam()).getResearchProgress(eCurrentTech),
			calculateResearchModifier(eCurrentTech)
		);
		GET_TEAM(getTeam()).setHasTech(eCurrentTech, true, getID(), true, true);

		if (!GC.getGame().isMPOption(MPOPTION_SIMULTANEOUS_TURNS) && !GC.getGame().isOption(GAMEOPTION_NO_TECH_BROKERING))
		{
			GET_TEAM(getTeam()).setNoTradeTech(eCurrentTech, true);
		}
		if (!isHumanPlayer())
		{
			AI_chooseResearch();
		}
		eCurrentTech = getCurrentResearch();
	}
	return std::max(0, iOverflow);
}

int CvPlayer::getCivilizationHealth() const
{
	return m_iCivilizationHealth;
}


void CvPlayer::changeCivilizationHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iCivilizationHealth += iChange;

		AI_makeAssignWorkDirty();
	}
}

int CvPlayer::getNoLandmarkAngerCount() const
{
	return m_iNoLandmarkAngerCount;
}

bool CvPlayer::isNoLandmarkAnger() const
{
	return getNoLandmarkAngerCount() > 0;
}

void CvPlayer::changeNoLandmarkAngerCount(int iChange)
{
	if (iChange != 0)
	{
		m_iNoLandmarkAngerCount += iChange;

		AI_makeAssignWorkDirty();
	}
}

int CvPlayer::getLandmarkHappiness() const
{
	return m_iLandmarkHappiness;
}

void CvPlayer::changeLandmarkHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iLandmarkHappiness += iChange;

		AI_makeAssignWorkDirty();
	}
}

bool CvPlayer::isShowLandmarks() const
{
	return m_bShowLandmarks && GC.getGame().isOption(GAMEOPTION_MAP_PERSONALIZED);
}

void CvPlayer::setShowLandmarks(bool bNewVal)
{
	PROFILE_EXTRA_FUNC();
	bool bOldVal = m_bShowLandmarks;
	if (bOldVal && !bNewVal)
	{
		for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);
			if (pLoopPlot->getLandmarkType() != NO_LANDMARK)
			{
				Cy::call(PYCivModule, "AddSign", Cy::Args() << pLoopPlot << getID() << "");
			}
		}
	}
	else
	{
		for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);
			if (pLoopPlot->getLandmarkType() != NO_LANDMARK)
			{
				Cy::call(PYCivModule, "AddSign", Cy::Args() << pLoopPlot << getID() << pLoopPlot->getLandmarkMessage().GetCString());
			}
		}
	}
	m_bShowLandmarks = bNewVal;
}

void CvPlayer::acquireFort(CvPlot* pPlot)
{
	pPlot->setOwner(getID(),true,true);
}

int CvPlayer::getResourceConsumption(BonusTypes eBonus) const
{
	return m_paiResourceConsumption[eBonus];
}


//void CvPlayer::recalculateResourceConsumption(BonusTypes eBonus)
//{
//	PROFILE_FUNC();
//
//	if (eBonus == NO_BONUS || m_paiResourceConsumption == NULL)
//		return;
//
//	int iTotalConsumption = 0;
//
//	// 1. City resource consumption
//	for (int i = 0; i < numCities(); ++i)
//	{
//		CvCity* pCity = getCity(i);
//		if (pCity)
//		{
//			iTotalConsumption += pCity->getResourceConsumption(eBonus);
//		}
//	}
//
//	// 2. Unit resource consumption
//	for (int i = 0; i < getNumUnits(); ++i)
//	{
//		CvUnit* pUnit = getUnit(i);
//		if (pUnit)
//		{
//			iTotalConsumption += pUnit->getResourceConsumption(eBonus);
//		}
//	}
//
//	// If buildings contribute, add a similar loop here.
//
//	// 3. Set the total consumption
//	m_paiResourceConsumption[eBonus] = iTotalConsumption;
//}

//recalculate resource consumption only measures how much we are using
//the resource at this very moment, not the past. It ignores existing buildings
//units, etc... UNLESS they are generating income from the resource.
void CvPlayer::recalculateResourceConsumption(BonusTypes eBonus)
{
	PROFILE_FUNC();

	if (!hasBonus(eBonus))
	{
		m_paiResourceConsumption[eBonus] = 0;
		return;
	}
	int iConsumption = 0;

	for (city_iterator cityIt = beginCities(); cityIt != endCities(); ++cityIt)
	{
		const CvCity* cityX = *cityIt;
		// --- Construction Consumption ---
		YieldTypes prodYield = YIELD_PRODUCTION;
		int baseProd = cityX->getBaseYieldRate(prodYield);
		int prodYieldRate = cityX->getYieldRate(prodYield);

		BuildingTypes prodBuilding = cityX->getProductionBuilding();
		UnitTypes prodUnit = cityX->getProductionUnit();
		ProjectTypes prodProject = cityX->getProductionProject();

		if (prodBuilding != NO_BUILDING)
		{
			const CvBuildingInfo& kBuilding = GC.getBuildingInfo(prodBuilding);

			if (kBuilding.getPrereqAndBonus() == eBonus
			|| kBuilding.getPrereqVicinityBonus() == eBonus
			|| algo::any_of_equal(kBuilding.getPrereqOrBonuses(), eBonus)
			|| algo::any_of_equal(kBuilding.getPrereqOrVicinityBonuses(), eBonus))
			{
				iConsumption += prodYieldRate;
			}

			int mod = kBuilding.getBonusProductionModifier(eBonus);
			if (mod != 0)
			{
				iConsumption += baseProd * mod / 100;
			}
		}
		else if (prodUnit != NO_UNIT)
		{
			const CvUnitInfo& kUnit = GC.getUnitInfo(prodUnit);

			if (kUnit.getPrereqAndBonus() == eBonus
			|| kUnit.getPrereqVicinityBonus() == eBonus
			|| algo::any_of_equal(kUnit.getPrereqOrBonuses(), eBonus)
			|| algo::any_of_equal(kUnit.getPrereqOrVicinityBonuses(), eBonus))
			{
				iConsumption += prodYieldRate;
			}

			int mod = kUnit.getBonusProductionModifier(eBonus);
			if (mod != 0)
			{
				iConsumption += baseProd * mod / 100;
			}
		}
		else if (prodProject != NO_PROJECT)
		{
			int mod = GC.getProjectInfo(prodProject).getBonusProductionModifier(eBonus);
			if (mod != 0)
			{
				iConsumption += baseProd * mod / 100;
			}
		}

		// --- Precompute base commerce rates ---
		int aiBaseCommerceRate[NUM_COMMERCE_TYPES];
		for (int iI = 0; iI < NUM_COMMERCE_TYPES; ++iI)
		{
			aiBaseCommerceRate[iI] = cityX->getBaseCommerceRate((CommerceTypes)iI);
		}

		// --- Building Effects ---
		const std::vector<BuildingTypes>& hasBuildings = cityX->getHasBuildings();
		for (std::vector<BuildingTypes>::const_iterator bldIt = hasBuildings.begin(); bldIt != hasBuildings.end(); ++bldIt)
		{
			BuildingTypes eTypeX = *bldIt;
			if (cityX->isDisabledBuilding(eTypeX))
				continue;

			const CvBuildingInfo& buildingX = GC.getBuildingInfo(eTypeX);
			iConsumption += (
					buildingX.getBonusHappinessChanges().getValue(eBonus) * 12
				+	buildingX.getBonusHealthChanges().getValue(eBonus) * 8
				+	buildingX.getBonusDefenseChanges(eBonus)
			);

			for (int iJ = 0; iJ < NUM_YIELD_TYPES; ++iJ)
			{
				int baseYield = cityX->getBaseYieldRate((YieldTypes)iJ);
				iConsumption += (
						buildingX.getBonusYieldChanges(eBonus, iJ) * 3
					+ buildingX.getBonusYieldModifier(eBonus, iJ) * baseYield / 33
				);
			}
			for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; ++iJ)
			{
				iConsumption += aiBaseCommerceRate[iJ] * buildingX.getBonusCommerceModifier(eBonus, iJ) / 33;
			}
		}
	}

	// --- Resource Export: Trading Partners ---
	int iBonusExport = getBonusExport(eBonus);
	if (iBonusExport > 0)
		{
			int iTempValue = 0;
			int iTradingPartnerCount = 0;
		for (int iI = 0; iI < MAX_PC_PLAYERS; ++iI)
			{
			const CvPlayer& kOther = GET_PLAYER((PlayerTypes)iI);
			if (kOther.isAlive()
				&& GET_TEAM(getTeam()).isHasMet(kOther.getTeam())
				&& kOther.getBonusImport(eBonus) > 0)
				{
				iTempValue += kOther.getResourceConsumption(eBonus);
				++iTradingPartnerCount;
				}
			}
			if (iTradingPartnerCount > 0)
			{
			iTempValue = (iTempValue / iTradingPartnerCount) * iBonusExport;
				iConsumption += iTempValue;
			}
		}

	m_paiResourceConsumption[eBonus] = iConsumption;
}

void CvPlayer::recalculateAllResourceConsumption()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		recalculateResourceConsumption((BonusTypes)iI);
	}
}

int CvPlayer::getBuildingInflation() const
{
	return m_iBuildingInflation;
}

void CvPlayer::changeBuildingInflation(int iChange)
{
	m_iBuildingInflation += iChange;
}

int CvPlayer::getProjectInflation() const
{
	return m_iProjectInflation;
}

void CvPlayer::changeProjectInflation(int iChange)
{
	m_iProjectInflation += iChange;
}

int CvPlayer::getTechInflation() const
{
	return m_iTechInflation;
}

void CvPlayer::changeTechInflation(int iChange)
{
	m_iTechInflation += iChange;
}

int CvPlayer::getCivicInflation() const
{
	return m_iCivicInflation;
}

void CvPlayer::changeCivicInflation(int iChange)
{
	m_iCivicInflation += iChange;
}

int CvPlayer::getHurryCostModifier() const
{
	return m_iHurryCostModifier;
}

void CvPlayer::changeHurryCostModifier(int iChange)
{
	m_iHurryCostModifier += iChange;
}

int CvPlayer::getHurryInflationModifier() const
{
	return m_iHurryInflationModifier;
}

void CvPlayer::changeHurryInflationModifier(int iChange)
{
	m_iHurryInflationModifier += iChange;
}

int CvPlayer::getHurriedCount() const
{
	return m_iHurryCount;
}

void CvPlayer::changeHurriedCount(int iChange)
{
	m_iHurryCount += iChange;
}

bool CvPlayer::hasValidBuildings(TechTypes eTech) const
{
	PROFILE_EXTRA_FUNC();
	bool bRequiresOrBuilding = false;
	bool bHasOneOrBuilding = false;

	for (int iI = 0; iI < GC.getTechInfo(eTech).getNumPrereqBuildings(); iI++)
	{
		const int iRequired = GC.getTechInfo(eTech).getPrereqBuilding(iI).iMinimumRequired;
		if (iRequired > 0)
		{
			if (getBuildingCount(GC.getTechInfo(eTech).getPrereqBuilding(iI).eBuilding) < iRequired)
			{
				return false;
			}
		}
	}
	for (int iI = 0; iI < GC.getTechInfo(eTech).getNumPrereqOrBuildings(); iI++)
	{
		if (!bHasOneOrBuilding)
		{
			const int iRequiredOr = GC.getTechInfo(eTech).getPrereqOrBuilding(iI).iMinimumRequired;
			if (iRequiredOr > 0)
			{
				bRequiresOrBuilding = true;
				if (getBuildingCount(GC.getTechInfo(eTech).getPrereqOrBuilding(iI).eBuilding) >= iRequiredOr)
				{
					bHasOneOrBuilding = true;
				}
			}
		}
	}
	if (!bHasOneOrBuilding && bRequiresOrBuilding)
	{
		return false;
	}
	return true;
}

void CvPlayer::checkAIStrategy()
{
	const bool bValidStrategy = (
	(GC.getLeaderHeadInfo(getPersonalityType()).getCultureVictoryWeight() != 0) 	||
	(GC.getLeaderHeadInfo(getPersonalityType()).getSpaceVictoryWeight() != 0) 		||
	(GC.getLeaderHeadInfo(getPersonalityType()).getConquestVictoryWeight() != 0)	||
	(GC.getLeaderHeadInfo(getPersonalityType()).getDominationVictoryWeight() != 0)	||
	(GC.getLeaderHeadInfo(getPersonalityType()).getDiplomacyVictoryWeight() != 0)
	);

	//The XML for this leader's strategies are empty, we need to create some
	if (!bValidStrategy)
	{
		//TODO: Make this based off of leaders traits
		GC.getLeaderHeadInfo(getPersonalityType()).setCultureVictoryWeight(GC.getGame().getSorenRandNum(100, "AI Strategy"));
		GC.getLeaderHeadInfo(getPersonalityType()).setSpaceVictoryWeight(GC.getGame().getSorenRandNum(100, "AI Strategy"));
		GC.getLeaderHeadInfo(getPersonalityType()).setConquestVictoryWeight(GC.getGame().getSorenRandNum(100, "AI Strategy"));
		GC.getLeaderHeadInfo(getPersonalityType()).setDominationVictoryWeight(GC.getGame().getSorenRandNum(100, "AI Strategy"));
		GC.getLeaderHeadInfo(getPersonalityType()).setDiplomacyVictoryWeight(GC.getGame().getSorenRandNum(100, "AI Strategy"));
	}
}

int CvPlayer::getBuildingCommerceChange(BuildingTypes eType, CommerceTypes CommerceType) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eType);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, CommerceType);
	return m_ppiBuildingCommerceChange[eType][CommerceType];
}

void CvPlayer::changeBuildingCommerceChange(BuildingTypes eType, CommerceTypes CommerceType, int iChange)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eType);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, CommerceType);

	if (iChange != 0)
	{
		m_ppiBuildingCommerceChange[eType][CommerceType] += iChange;

		foreach_(CvCity* cityX, cities())
		{
			if (cityX->hasFullyActiveBuilding(eType))
			{
				cityX->changeBuildingCommerceChange(eType, CommerceType, iChange);
			}
		}
		setCommerceDirty();
	}
}

int CvPlayer::getBuildingCommerceModifier(BuildingTypes eType, CommerceTypes eCommerce) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eType);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eCommerce);
	return m_ppiBuildingCommerceModifier[eType][eCommerce];
}

void CvPlayer::changeBuildingCommerceModifier(BuildingTypes eType, CommerceTypes eCommerce, int iChange)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eType);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eCommerce);

	if (iChange != 0)
	{
		m_ppiBuildingCommerceModifier[eType][eCommerce] += iChange;

		foreach_(CvCity* cityX, cities())
		{
			if (cityX->hasFullyActiveBuilding(eType))
			{
				cityX->changeBuildingCommerceModifier(eCommerce, iChange);
			}
		}
		setCommerceDirty();
	}
}

int CvPlayer::getBonusCommerceModifier(BonusTypes eIndex1, CommerceTypes eIndex2) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eIndex1);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex2);
	return m_ppiBonusCommerceModifier[eIndex1][eIndex2];
}

void CvPlayer::changeBonusCommerceModifier(BonusTypes eIndex1, CommerceTypes eIndex2, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eIndex1);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex2);

	if (iChange != 0)
	{
		//int iOldValue = getBonusCommerceModifier(eIndex1, eIndex2);

		m_ppiBonusCommerceModifier[eIndex1][eIndex2] += iChange;
		setCityCommerceModifierDirty(eIndex2);
		setCommerceDirty();
	}
}

void CvPlayer::setCityCommerceModifierDirty(CommerceTypes eCommerce)
{
	algo::for_each(cities(), CvCity::fn::setCommerceModifierDirty(eCommerce));
}

int CvPlayer::getLandmarkYield(YieldTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	return m_aiLandmarkYield[eIndex];
}


void CvPlayer::changeLandmarkYield(YieldTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiLandmarkYield[eIndex] += iChange;

		updateYield();
	}
}


int CvPlayer::getBuildingCount(BuildingTypes eBuilding, bool bUpgrades) const
{
	if (bUpgrades)
	{
		return getBuildingCountWithUpgrades(eBuilding);
	}
	return getBuildingCount(eBuilding);
}

int CvPlayer::getBuildingCountWithUpgrades(BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eBuilding);

	if (eBuilding == NO_BUILDING)
	{
		return 0;
	}
	int iCount = getBuildingCount(eBuilding);
	const CvBuildingInfo& building = GC.getBuildingInfo(eBuilding);
	for (int iI = 0; iI < building.getNumReplacementBuilding(); ++iI)
	{
		iCount += getBuildingCount((BuildingTypes)building.getReplacementBuilding(iI));
	}
	return iCount;
}

void CvPlayer::setColor(PlayerColorTypes eColor)
{
	GC.getInitCore().setColor( getID(), eColor );
	gDLL->getInterfaceIFace()->makeInterfaceDirty();

	algo::for_each(cities(), CvCity::fn::setLayoutDirty(true));

// Forces update of units flags
	EraTypes eEra = getCurrentEra();
	//Suppresses Era popups
	bool bHuman = m_bHuman;
	m_bHuman = false;
	setCurrentEra((EraTypes)0);
	setCurrentEra((EraTypes)(GC.getNumEraInfos() - 1));
	setCurrentEra(eEra);
	m_bHuman = bHuman;
	gDLL->getInterfaceIFace()->makeInterfaceDirty();

	// Need to force redraw
	gDLL->getEngineIFace()->SetDirty(CultureBorders_DIRTY_BIT, true);
	gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true);
	gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);
	gDLL->getEngineIFace()->SetDirty(GlobePartialTexture_DIRTY_BIT, true);

	gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(HighlightPlot_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(UnitInfo_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(MinimapSection_DIRTY_BIT, true);
	gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(Foreign_Screen_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(GlobeInfo_DIRTY_BIT, true);

	setupGraphical();
}

void CvPlayer::setHandicap(int iNewVal, bool bAdjustGameHandicap)
{
	FASSERT_BOUNDS(0, GC.getNumHandicapInfos(), iNewVal);

	if (iNewVal != getHandicapType())
	{
		GC.getInitCore().setHandicap(getID(), (HandicapTypes)iNewVal);

		if (bAdjustGameHandicap)
		{
			GC.getGame().averageHandicaps();
		}
		setMaintenanceDirty(true);
		setUnitUpkeepDirty();
	}
}

bool CvPlayer::canBuild(const CvPlot* pPlot, ImprovementTypes eImprovement, bool bTestVisible) const
{
	return algo::any_of(GC.getImprovementInfo(eImprovement).getBuildTypes(),
		bind(CvPlayer::canBuild, this, pPlot, _1, bTestVisible, true)
	);
}

bool CvPlayer::isModderOption(ModderOptionTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_MODDEROPTION_TYPES, eIndex);
	return m_aiModderOptions[eIndex] != 0;
}

int CvPlayer::getModderOption(ModderOptionTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_MODDEROPTION_TYPES, eIndex);
	return m_aiModderOptions[eIndex];
}

void CvPlayer::setModderOption(ModderOptionTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, NUM_MODDEROPTION_TYPES, eIndex);
	m_aiModderOptions[eIndex] = iNewValue;
}

void CvPlayer::setModderOption(ModderOptionTypes eIndex, bool bNewValue)
{
	FASSERT_BOUNDS(0, NUM_MODDEROPTION_TYPES, eIndex);
	m_aiModderOptions[eIndex] = bNewValue;
}

int CvPlayer::getCorporationSpreadModifier() const
{
	return m_iCorporationSpreadModifier;
}

void CvPlayer::changeCorporationSpreadModifier(int iChange)
{
	m_iCorporationSpreadModifier += iChange;
	FAssertMsg(getCorporationSpreadModifier() >= -100, "Corporation Spread Rate is Below Zero!");
}

int64_t CvPlayer::getCorporateMaintenance() const
{
	return m_iCorporateMaintenance;
}

void CvPlayer::updateCorporateMaintenance()
{
	PROFILE_FUNC();

	m_iCorporateMaintenance = 0;

	foreach_(CvCity* cityX, cities())
	{
		m_iCorporateMaintenance += cityX->calcCorporateMaintenance();
	}
}

int CvPlayer::getCorporationInfluence(CorporationTypes eIndex) const
{
	PROFILE_EXTRA_FUNC();
	if (GC.getCorporationInfo(eIndex).getObsoleteTech() != NO_TECH && GET_TEAM(getTeam()).isHasTech(GC.getCorporationInfo(eIndex).getObsoleteTech()))
	{
		return 0;
	}
	int iInfluence = 100;

	iInfluence += GET_TEAM(getTeam()).getCorporationRevenueModifier() / 2;

	//Find the prereq tech for corporate HQ
	TechTypes ePrereqTech = GC.getCorporationInfo(eIndex).getTechPrereq();

	for (int i = 0; ePrereqTech == NO_TECH && i < GC.getNumBuildingInfos(); i++)
	{
		if ((CorporationTypes)GC.getBuildingInfo((BuildingTypes)i).getGlobalCorporationCommerce() == eIndex)
		{
			ePrereqTech = (TechTypes)GC.getBuildingInfo((BuildingTypes)i).getPrereqAndTech();
		}
	}
	if (ePrereqTech != NO_TECH && !GET_TEAM(getTeam()).isHasTech(ePrereqTech))
	{
		iInfluence *= 2;
		iInfluence /= 3;
	}

	//Compare Technological Advancement to the owner of the corporation
	if (GC.getGame().getHeadquarters(eIndex))
	{
		const PlayerTypes eCorpOwner = GC.getGame().getHeadquarters(eIndex)->getOwner();

		if (eCorpOwner != getID())
		{
			int iExtraTechs = 0;
			for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
			{
				if (GET_TEAM(getTeam()).isHasTech((TechTypes)iI) && !GET_TEAM(GET_PLAYER(eCorpOwner).getTeam()).isHasTech((TechTypes)iI))
				{
					iExtraTechs++;
				}
				else if (!GET_TEAM(getTeam()).isHasTech((TechTypes)iI) && GET_TEAM(GET_PLAYER(eCorpOwner).getTeam()).isHasTech((TechTypes)iI))
				{
					iExtraTechs--;
				}
			}
			iInfluence += iExtraTechs;

			if (GET_TEAM(getTeam()).isAtWar(GET_PLAYER(eCorpOwner).getTeam()))
			{
				iInfluence /= 3;
			}
		}
	}
	iInfluence = getModifiedIntValue(iInfluence, (-5) * getEnvironmentalProtection() - 2 * getLaborFreedom());

	logBBAI("  Player %d (%S) Has a Corporation Influence of %d for Corporation %s", getID(), getCivilizationDescription(0), iInfluence, GC.getCorporationInfo(eIndex).getDescription());

	return iInfluence;
}

int CvPlayer::getEnvironmentalProtection() const
{
	PROFILE_EXTRA_FUNC();
	int iValue = 0;
	for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		if (getCivics((CivicOptionTypes)iI) != NO_CIVIC)
		{
			iValue += GC.getCivicInfo(getCivics((CivicOptionTypes)iI)).getRevEnvironmentalProtection();
		}
	}
	return iValue;
}

int CvPlayer::getLaborFreedom() const
{
	PROFILE_EXTRA_FUNC();
	int iValue = 0;
	for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		if (getCivics((CivicOptionTypes)iI) != NO_CIVIC)
		{
			iValue += GC.getCivicInfo(getCivics((CivicOptionTypes)iI)).getRevLaborFreedom();
		}
	}
	return iValue;
}

int CvPlayer::getScoreComponent(int iRawScore, int iInitial, int iMax, int iFactor, bool bExponential, bool bFinal, bool bVictory) const
{
	if (GC.getGame().getEstimateEndTurn() == 0)
	{
		return 0;
	}

	if (bFinal && bVictory)
	{
		float fTurnRatio = float(GC.getGame().getGameTurn()) / float(GC.getGame().getEstimateEndTurn());
		if (bExponential && (iInitial != 0))
		{
			float fRatio = float(iMax) / float(iInitial);
			iMax = iInitial * int(pow(fRatio, fTurnRatio));
		}
		else
		{
			iMax = iInitial + int((fTurnRatio * float(iMax - iInitial)));
		}
	}
	int iScore;
	int iFree = (GC.getSCORE_FREE_PERCENT() * iMax) / 100;
	if ((iFree + iMax) != 0)
	{
		iScore = (iFactor * (iRawScore + iFree)) / (iFree + iMax);
	}
	else
	{
		iScore = iFactor;
	}

	if (bVictory)
	{
		iScore = ((100 + GC.getDefineINT("SCORE_VICTORY_PERCENT")) * iScore) / 100;
	}

	if (bFinal)
	{
		iScore = ((100 + GC.getDefineINT("SCORE_HANDICAP_PERCENT_OFFSET") + (GC.getGame().getHandicapType() * GC.getDefineINT("SCORE_HANDICAP_PERCENT_PER"))) * iScore) / 100;
	}

	return iScore;
}


void CvPlayer::doAdvancedEconomy()
{
	PROFILE_FUNC();

	if (getHurriedCount() > 0)
	{
		const int iTurnIncrement1000 = (
			getModifiedIntValue(
				(
						GC.getHURRY_INFLATION_DECAY_RATE()
					*	GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent()
				),
				getHurryInflationModifier()
			)
		);
		if (GC.getGame().getElapsedGameTurns() % std::max(1, iTurnIncrement1000 / 1000) == 0)
		{
			changeHurriedCount(
				-std::min(
					getHurriedCount(),
					(
						iTurnIncrement1000 < 1000
						?
						1000 / std::max(1, iTurnIncrement1000)
						:
						1
					)
				)
			);
		}
	}
}

int CvPlayer::getWonderConstructRand() const
{
	int iNumBTSWonders = 62;
	int iNumModWonders = GC.getGame().getNumWonders();
	int iWonderConstructRand = GC.getLeaderHeadInfo(getPersonalityType()).getWonderConstructRand();
	iWonderConstructRand *= iNumModWonders;
	iWonderConstructRand /= iNumBTSWonders;
	return iWonderConstructRand;
}

int CvPlayer::getSpecialistCommercePercentChanges(SpecialistTypes eIndex1, CommerceTypes eIndex2) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex1);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex2);
	return m_ppiSpecialistCommercePercentChanges[eIndex1][eIndex2];
}

void CvPlayer::changeSpecialistCommercePercentChanges(SpecialistTypes eIndex1, CommerceTypes eIndex2, int iChange)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex1);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex2);

	if (iChange != 0)
	{
		const int iOldValue = getSpecialistCommercePercentChanges(eIndex1, eIndex2);
		m_ppiSpecialistCommercePercentChanges[eIndex1][eIndex2] += iChange;

		foreach_(CvCity* pLoopCity, cities())
		{
			const int iExistingValue = (pLoopCity->getFreeSpecialistCount(eIndex1) + pLoopCity->getSpecialistCount(eIndex1)) * (getSpecialistCommercePercentChanges(eIndex1, eIndex2) - iOldValue);
			// set the new
			pLoopCity->changeSpecialistCommerceTimes100(eIndex2, iExistingValue);
		}
	}
}

int CvPlayer::getSpecialistYieldPercentChanges(SpecialistTypes eIndex1, YieldTypes eIndex2) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex1);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex2);
	return m_ppiSpecialistYieldPercentChanges[eIndex1][eIndex2];
}

void CvPlayer::changeSpecialistYieldPercentChanges(SpecialistTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex1);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex2);

	if (iChange != 0)
	{
		const int iOldValue = getSpecialistYieldPercentChanges(eIndex1, eIndex2);
		m_ppiSpecialistYieldPercentChanges[eIndex1][eIndex2] += iChange;

		foreach_(CvCity* pLoopCity, cities())
		{
			const int iExistingValue = (pLoopCity->getSpecialistCount(eIndex1) * getSpecialistYieldPercentChanges(eIndex1, eIndex2) - iOldValue) / 100;
			// set the new
			pLoopCity->changeExtraYield(eIndex2, iExistingValue);
		}
	}
}

int CvPlayer::getFractionalCombatExperience() const
{
	return m_iFractionalCombatExperience;
}

void CvPlayer::changeFractionalCombatExperience(int iChange, UnitTypes eGGType)
{
	m_iFractionalCombatExperience += iChange;
	const int iNewXP = std::max(1, m_iFractionalCombatExperience / 100);
	changeCombatExperience(iNewXP, eGGType);
	m_iFractionalCombatExperience -= iNewXP * 100;
}


void CvPlayer::clearTileCulture()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		GC.getMap().plotByIndex(iI)->setCulture(getID(), 0, true, true);
	}
}

void CvPlayer::clearCityCulture()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			foreach_(CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
			{
				pLoopCity->setCulture(getID(), 0, true, true);
			}
		}
	}
}

int CvPlayer::getCityOverLimitUnhappy() const {
	// AIAndy: No unhappiness for barbarians from number of cities
	if (isNPC())
	{
		return 0;
	}
	return m_iCityOverLimitUnhappy;
}

void CvPlayer::changeCityOverLimitUnhappy(int iChange)
{
	m_iCityOverLimitUnhappy += iChange;
}

void CvPlayer::changeCityLimit(int iChange)
{
	m_iCityLimit += iChange;
}

int CvPlayer::getForeignUnhappyPercent() const {
	return m_iForeignUnhappyPercent;
}

void CvPlayer::changeForeignUnhappyPercent(int iChange) {
	if (iChange != 0)
	{
		m_iForeignUnhappyPercent += iChange;
	}
}


// BUG - Reminder Mod - start
#include "CvMessageControl.h"
void CvPlayer::addReminder(int iGameTurn, CvWString szMessage) const
{
	CvMessageControl::getInstance().sendAddReminder(getID(), iGameTurn, szMessage);
}
// BUG - Reminder Mod - end

CvContractBroker& CvPlayer::getContractBroker()
{
	return m_contractBroker;
}

int64_t CvPlayer::getCulture() const
{
	return m_iCulture;
}

void CvPlayer::changeCulture(int64_t iAddValue)
{
	m_iCulture += iAddValue;
	FASSERT_NOT_NEGATIVE(m_iCulture);
}


void CvPlayer::clearCanConstructCache(BuildingTypes building, bool bIncludeCities) const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (building == NO_BUILDING || building == static_cast<BuildingTypes>(iI))
		{
			m_bCanConstructCached[iI] = false;
			m_bCanConstructCachedDefaultParam[iI] = false;

			if (bIncludeCities)
			{
				algo::for_each(cities(), CvCity::fn::FlushCanConstructCache(building));
			}
		}
	}

	algo::for_each(cities(), CvCity::fn::setBuildingListInvalid());
}

void CvPlayer::clearCanConstructCacheForGroup(SpecialBuildingTypes eSpecialBuilding, bool bIncludeCities) const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		SpecialBuildingTypes eLoopSpecialBuilding = NO_SPECIALBUILDING;

		if (eSpecialBuilding != NO_SPECIALBUILDING)
		{
			eLoopSpecialBuilding = GC.getBuildingInfo((BuildingTypes)iI).getSpecialBuilding();
		}

		if (eSpecialBuilding == eLoopSpecialBuilding)
		{
			m_bCanConstructCached[iI] = false;
			m_bCanConstructCachedDefaultParam[iI] = false;

			if (bIncludeCities)
			{
				algo::for_each(cities(), CvCity::fn::FlushCanConstructCache((BuildingTypes)iI));
			}
		}
	}

	algo::for_each(cities(), CvCity::fn::setBuildingListInvalid());
}

void CvPlayer::clearModifierTotals()
{
	PROFILE_EXTRA_FUNC();
	//	Clear city-sourced modifiers
	algo::for_each(cities(), CvCity::fn::clearModifierTotals());

	//	Project stuff
	m_iWorldHappiness = 0;
	m_iProjectHappiness = 0;
	m_iWorldHealth = 0;
	m_iProjectHealth = 0;

	m_iForceAllTradeRoutes = 0;
	m_iNoCapitalUnhappiness = 0;
	m_iCivilizationHealth = 0;

	m_iForeignTradeRouteModifier = 0;
	m_iTaxRateUnhappiness = 0;
	m_iCivicHappiness = 0;
	m_iUnitUpgradePriceModifier = 0;
	m_iAnarchyModifier = 0;
	m_iGoldenAgeModifier = 0;
	m_iGlobalHurryModifier = 0;
	m_iGreatPeopleRateModifier = 0;
	m_iGreatGeneralRateModifier = 0;
	m_iDomesticGreatGeneralRateModifier = 0;
	m_iStateReligionGreatPeopleRateModifier = 0;
	m_iMaxGlobalBuildingProductionModifier = 0;
	m_iMaxTeamBuildingProductionModifier = 0;
	m_iMaxPlayerBuildingProductionModifier = 0;
	m_iFreeExperience = 0;
	m_iFeatureProductionModifier = 0;
	m_iWorkerSpeedModifier = 0;
	m_iImprovementUpgradeRateModifier = 0;
	m_iMilitaryProductionModifier = 0;
	m_iSpaceProductionModifier = 0;
	m_iCityDefenseModifier = 0;

	m_iNonStateReligionCommerceCount = 0;
	m_iUpgradeAnywhereCount = 0;
	m_iRevIdxDistanceModifier = 0;
	m_iRevIdxHolyCityGood = 0;
	m_iRevIdxHolyCityBad = 0;
	m_fRevIdxNationalityMod = 0;
	m_fRevIdxBadReligionMod = 0;
	m_fRevIdxGoodReligionMod = 0;
	m_iCityLimit = 0;
	m_iCityOverLimitUnhappy = 0;
	m_iForeignUnhappyPercent = 0;

	m_iBaseFreeUnitUpkeepCivilian = 0;
	m_iBaseFreeUnitUpkeepMilitary = 0;
	m_iFreeUnitUpkeepCivilianPopPercent = 0;
	m_iFreeUnitUpkeepMilitaryPopPercent = 0;
	m_iCivilianUnitUpkeepMod = 0;
	m_iMilitaryUnitUpkeepMod = 0;
	m_iUnitUpkeepMilitary100 = 0;
	m_iUnitUpkeepCivilian100 = 0;
	m_iFinalUnitUpkeep = 0;

	m_iHappyPerMilitaryUnit = 0;
	m_iMilitaryFoodProductionCount = 0;
	m_iNoUnhealthyPopulationCount = 0;
	m_iExpInBorderModifier = 0;
	m_iBuildingOnlyHealthyCount = 0;

	m_iMaintenanceModifier = 0;
	m_iCoastalDistanceMaintenanceModifier = 0;
	m_iConnectedCityMaintenanceModifier = 0;
	m_iDistanceMaintenanceModifier = 0;
	m_iNumCitiesMaintenanceModifier = 0;
	m_iCorporationMaintenanceModifier = 0;
	m_iHomeAreaMaintenanceModifier = 0;
	m_iOtherAreaMaintenanceModifier = 0;

	m_iUpkeepModifier = 0;
	m_iLevelExperienceModifier = 0;
	m_iExtraHealth = 0;
	m_iCivicHealth = 0;
	m_iBuildingGoodHealth = 0;
	m_iBuildingBadHealth = 0;
	m_iExtraHappiness = 0;
	m_iBuildingHappiness = 0;
	m_iLargestCityHappiness = 0;
	m_iWarWearinessModifier = 0;
	m_iFreeSpecialist = 0;
	m_iNoForeignTradeCount = 0;
	m_iNoCorporationsCount = 0;
	m_iNoForeignCorporationsCount = 0;
	m_iCoastalTradeRoutes = 0;
	m_iTradeRoutes = 0;
	m_iStateReligionCount = 0;
	m_iNoNonStateReligionSpreadCount = 0;
	m_iStateReligionHappiness = 0;
	m_iNonStateReligionHappiness = 0;
	m_iStateReligionUnitProductionModifier = 0;
	m_iStateReligionBuildingProductionModifier = 0;
	m_iStateReligionFreeExperience = 0;

	m_iBuildingInflation = 0;
	m_iCivicInflation = 0;
	m_iProjectInflation = 0;
	m_iTechInflation = 0;

	m_iHurryCostModifier = 0;
	m_iHurryInflationModifier = 0;

	m_iTechScore = 0;
	m_iWondersScore = 0;

	m_iInquisitionCount = 0;
	m_iNationalEspionageDefense = 0;
	m_iCivicAnarchyModifier = 0;
	m_iReligiousAnarchyModifier = 0;
	m_iAIAttitudeModifier = 0;
	m_iMaxTradeRoutesAdjustment = 0;
	m_iNationalHurryAngerModifier = 0;
	m_iNationalEnemyWarWearinessModifier = 0;
	m_iNationalBombardDefenseModifier = 0;
	m_iFixedBordersCount = 0;
	m_iFreedomFighterCount = 0;
	m_iExtraFreedomFighters = 0;
	//Team Project (6)
	m_iNationalCityStartCulture = 0;
	m_iNationalAirUnitCapacity = 0;
	m_iCapitalXPModifier = 0;
	m_iStateReligionHolyCityXPModifier = 0;
	m_iNonStateReligionHolyCityXPModifier = 0;
	m_iNationalCityStartBonusPopulation = 0;
	m_iNationalMissileRangeChange = 0;
	m_iNationalFlightOperationRangeChange = 0;
	m_iNationalNavalCargoSpaceChange = 0;
	m_iNationalMissileCargoSpaceChange = 0;
	m_iCitiesStartwithStateReligionCount = 0;
	m_iDraftsOnCityCaptureCount = 0;
	m_iFreeSpecialistperWorldWonderCount = 0;
	m_iFreeSpecialistperNationalWonderCount = 0;
	m_iFreeSpecialistperTeamProjectCount = 0;
	m_iExtraGoodyCount = 0;

	m_iAllReligionsActiveCount = 0;

	m_iExtraNationalCaptureProbabilityModifier = 0;
	m_iExtraNationalCaptureResistanceModifier = 0;

	m_iExtraStateReligionSpreadModifier = 0;
	m_iExtraNonStateReligionSpreadModifier = 0;
	m_iNationalGreatPeopleRate = 0;

	m_bHasLanguage = false;

	m_iBaseMergeSelection = FFreeList::INVALID_INDEX;
	m_iFirstMergeSelection = FFreeList::INVALID_INDEX;
	m_iSecondMergeSelection = FFreeList::INVALID_INDEX;
	m_iSplittingUnit = FFreeList::INVALID_INDEX;
	m_iAmbushingUnit = FFreeList::INVALID_INDEX;
	m_bAssassinate = false;

	setPopulationgrowthratepercentage(0);
	m_iReligionSpreadRate = 0;
	setDistantUnitSupportCostModifier(0);
	setExtraCityDefense(0);
	setTraitExtraCityDefense(0);

	m_bonusMintedPercent.clear();
	m_freeBuildingCount.clear();
	m_extraBuildingHappiness.clear();
	m_extraBuildingHealth.clear();
	m_buildingProductionMod.clear();
	m_buildingCostMod.clear();
	m_unitProductionMod.clear();
	m_unitCombatProductionMod.clear();
	m_unitCombatFreeXP.clear();
	m_goldenAgeOnBirthOfGreatPersonCount.clear();
	m_greatPeopleRateforUnit.clear();
	m_commandFieldPlots.clear();
	m_commodoreFieldPlots.clear();

	for (int iI = 0; iI < GC.getNumFeatureInfos(); iI++)
	{
		m_paiFeatureHappiness[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumSpecialBuildingInfos(); iI++)
	{
		m_paiSpecialBuildingNotRequiredCount[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
	{
		m_paiHurryCount[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		m_paiHasCivicOptionCount[iI] = 0;
		m_paiNoCivicUpkeepCount[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
	{
		m_paiPlayerWideAfflictionCount[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		m_paiSpecialistValidCount[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			m_ppaaiSpecialistExtraYield[iI][iJ] = 0;
		}
		//TB Traits begin
		for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			m_ppaaiSpecialistExtraCommerce[iI][iJ] = 0;
		}
		//TB Traits end

		m_paiFreeSpecialistCount[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			m_ppaaiImprovementYieldChange[iI][iJ] = 0;
		}
		m_paiImprovementUpgradeRateModifierSpecific[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumTerrainInfos(); iI++)
	{
		for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			m_ppaaiTerrainYieldChange[iI][iJ] = 0;
		}
	}

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			m_ppiBuildingCommerceModifier[iI][iJ] = 0;
		}
	}

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		m_paiBuildingCount[iI] = 0;

		for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			m_ppiBuildingCommerceChange[iI][iJ] = 0;
		}
	}

	for (int iI = 0; iI < GC.getNumSpecialBuildingInfos(); iI++)
	{
		m_paiBuildingGroupCount[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			m_ppiBonusCommerceModifier[iI][iJ] = 0;
		}
	}

	for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		m_paiBuildWorkerSpeedModifierSpecific[iI] = 0;
	}

	for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		m_paiNationalDomainFreeExperience[iI] = 0;
		m_paiNationalDomainProductionModifier[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		m_paiNationalTechResearchModifier[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			m_ppiSpecialistCommercePercentChanges[iI][iJ] = 0;
		}
		for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			m_ppiSpecialistYieldPercentChanges[iI][iJ] = 0;
		}
		//Team Project (6)
		m_paiEraAdvanceFreeSpecialistCount[iI] = 0;
	}
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiSeaPlotYield[iI] = 0;
		m_aiYieldRateModifier[iI] = 0;
		m_aiCapitalYieldRateModifier[iI] = 0;
		m_aiExtraYieldThreshold[iI] = 0;
		m_aiTradeYieldModifier[iI] = 0;
		m_aiLandmarkYield[iI] = 0;

		//TB Traits begin
		m_aiFreeCityYield[iI] = 0;
		m_aiSpecialistExtraYield[iI] = 0;
		m_aiLessYieldThreshold[iI] = 0;
		m_aiGoldenAgeYield[iI] = 0;
		//TB Traits end
	}

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		m_aiFreeCityCommerce[iI] = 0;
		m_extraCommerce[iI] = 0;
		m_aiCommerceRate[iI] = 0;
		m_abCommerceDirty[iI] = false;
		m_aiCommerceRateModifier[iI] = 0;
		m_aiCommerceRateModifierfromEvents[iI] = 0;
		m_aiCommerceRateModifierfromBuildings[iI] = 0;
		m_aiCapitalCommerceRateModifier[iI] = 0;
		m_aiStateReligionBuildingCommerce[iI] = 0;
		m_aiSpecialistExtraCommerce[iI] = 0;
		m_aiCommerceFlexibleCount[iI] = 0;
		m_aiGoldenAgeCommerce[iI] = 0;
	}

	// Reset power to just that due to pop. Other contributions will be re-added during the recalc
	m_iPower = getTotalPopulation();
	m_iTechPower = 0;

	// Similarly assets for pop, land, and units
	m_iAssets = 10 * (getTotalPopulation() + getTotalLandScored());

	foreach_(const CvUnit* pLoopUnit, units())
	{
		m_iAssets += pLoopUnit->assetValueTotal();
		m_iUnitPower += pLoopUnit->getPowerValueTotal();
	}
}

void CvPlayer::processTrait(TraitTypes eTrait, int iChange)
{
	PROFILE_EXTRA_FUNC();
	changeNonStateReligionCommerce(iChange*((GC.getTraitInfo(eTrait).isNonStateReligionCommerce())? 1 : 0));
	changeUpgradeAnywhere(iChange*((GC.getTraitInfo(eTrait).isUpgradeAnywhere())? 1 : 0));
	changeRevIdxLocal(iChange*GC.getTraitInfo(eTrait).getRevIdxLocal());
	changeRevIdxNational(iChange*GC.getTraitInfo(eTrait).getRevIdxNational());
	changeRevIdxDistanceModifier(iChange*GC.getTraitInfo(eTrait).getRevIdxDistanceModifier());
	changeRevIdxHolyCityGood(iChange*GC.getTraitInfo(eTrait).getRevIdxHolyCityGood());
	changeRevIdxHolyCityBad(iChange*GC.getTraitInfo(eTrait).getRevIdxHolyCityBad());
	changeRevIdxNationalityMod(iChange*GC.getTraitInfo(eTrait).getRevIdxNationalityMod());
	changeRevIdxBadReligionMod(iChange*GC.getTraitInfo(eTrait).getRevIdxBadReligionMod());
	changeRevIdxGoodReligionMod(iChange*GC.getTraitInfo(eTrait).getRevIdxGoodReligionMod());

	changeCivilizationHealth(iChange*GC.getTraitInfo(eTrait).getHealth());
	changeExtraHappiness(iChange*GC.getTraitInfo(eTrait).getHappiness());

	foreach_(const BuildingModifier2& pair, GC.getTraitInfo(eTrait).getBuildingHappinessModifiersFiltered())
	{
		changeExtraBuildingHappiness(pair.first, iChange * pair.second);
	}

	changeUpkeepModifier(iChange*GC.getTraitInfo(eTrait).getUpkeepModifier());
	changeLevelExperienceModifier(iChange*GC.getTraitInfo(eTrait).getLevelExperienceModifier());
	changeGreatPeopleRateModifier(iChange*GC.getTraitInfo(eTrait).getGreatPeopleRateModifier());
	changeGreatGeneralRateModifier(iChange*GC.getTraitInfo(eTrait).getGreatGeneralRateModifier());
	changeDomesticGreatGeneralRateModifier(iChange*GC.getTraitInfo(eTrait).getDomesticGreatGeneralRateModifier());

	changeMaxGlobalBuildingProductionModifier(iChange*GC.getTraitInfo(eTrait).getMaxGlobalBuildingProductionModifier());
	changeMaxTeamBuildingProductionModifier(iChange*GC.getTraitInfo(eTrait).getMaxTeamBuildingProductionModifier());
	changeMaxPlayerBuildingProductionModifier(iChange*GC.getTraitInfo(eTrait).getMaxPlayerBuildingProductionModifier());

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		changeTradeYieldModifier(((YieldTypes)iI), iChange*GC.getTraitInfo(eTrait).getTradeYieldModifier(iI));
	}

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		changeExtraCommerce100((CommerceTypes)iI, 100*iChange*GC.getTraitInfo(eTrait).getCommerceChange(iI));
		changeCommerceRateModifier((CommerceTypes)iI, iChange*GC.getTraitInfo(eTrait).getCommerceModifier(iI));
	}

	for (int iI = 0; iI < GC.getTraitInfo(eTrait).getNumCivicOptionNoUpkeepTypes(); iI++)
	{
		if ((CivicOptionTypes)GC.getTraitInfo(eTrait).isCivicOptionNoUpkeepType(iI).eCivicOption != NO_CIVICOPTION)
		{
			CivicOptionTypes eCivicOption = ((CivicOptionTypes)GC.getTraitInfo(eTrait).isCivicOptionNoUpkeepType(iI).eCivicOption);
			if(GC.getTraitInfo(eTrait).isCivicOptionNoUpkeepType(iI).bBool)
			{
				changeNoCivicUpkeepCount(eCivicOption, iChange);
			}
		}
	}

	//for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	//{
	//	if (GC.getCivicOptionInfo((CivicOptionTypes) iI).getTraitNoUpkeep(eTrait))
	//	{
	//		changeNoCivicUpkeepCount(((CivicOptionTypes)iI), iChange);
	//	}
	//}

	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			changeImprovementYieldChange(((ImprovementTypes)iI), ((YieldTypes)iJ), (GC.getTraitInfo(eTrait).getImprovementYieldChange(iI, iJ) * iChange));
		}
	}

	//TB Traits begin
	if (GC.getTraitInfo(eTrait).getMaxAnarchy() > -1)
	{
		updateMaxAnarchyTurns();
	}
	if (GC.getTraitInfo(eTrait).getMinAnarchy() > 0)
	{
		updateMinAnarchyTurns();
	}
	changeWarWearinessModifier(iChange*GC.getTraitInfo(eTrait).getWarWearinessAccumulationModifier());
	changeCivicAnarchyModifier(iChange*GC.getTraitInfo(eTrait).getCivicAnarchyTimeModifier());
	changeReligiousAnarchyModifier(iChange*GC.getTraitInfo(eTrait).getReligiousAnarchyTimeModifier());
	changeImprovementUpgradeRateModifier(iChange*GC.getTraitInfo(eTrait).getImprovementUpgradeRateModifier());
	changeWorkerSpeedModifier(iChange*GC.getTraitInfo(eTrait).getWorkerSpeedModifier());
	changeMaxConscript(iChange*GC.getTraitInfo(eTrait).getMaxConscript());
	changeDistanceMaintenanceModifier(iChange*GC.getTraitInfo(eTrait).getDistanceMaintenanceModifier());
	changeNumCitiesMaintenanceModifier(iChange*GC.getTraitInfo(eTrait).getNumCitiesMaintenanceModifier());
	changeCorporationMaintenanceModifier(iChange*GC.getTraitInfo(eTrait).getCorporationMaintenanceModifier());
	changeStateReligionGreatPeopleRateModifier(iChange*GC.getTraitInfo(eTrait).getStateReligionGreatPeopleRateModifier());
	changeFreeExperience(iChange*GC.getTraitInfo(eTrait).getFreeExperience());
	changeBaseFreeUnitUpkeepCivilian(iChange*GC.getTraitInfo(eTrait).getFreeUnitUpkeepCivilian());
	changeBaseFreeUnitUpkeepMilitary(iChange*GC.getTraitInfo(eTrait).getFreeUnitUpkeepMilitary());
	changeFreeUnitUpkeepCivilianPopPercent(iChange*GC.getTraitInfo(eTrait).getFreeUnitUpkeepCivilianPopPercent());
	changeFreeUnitUpkeepMilitaryPopPercent(iChange*GC.getTraitInfo(eTrait).getFreeUnitUpkeepMilitaryPopPercent());
	changeCivilianUnitUpkeepMod(iChange*GC.getTraitInfo(eTrait).getCivilianUnitUpkeepMod());
	changeMilitaryUnitUpkeepMod(iChange*GC.getTraitInfo(eTrait).getMilitaryUnitUpkeepMod());
	changeHappyPerMilitaryUnit(iChange*GC.getTraitInfo(eTrait).getHappyPerMilitaryUnit());
	changeLargestCityHappiness(iChange*GC.getTraitInfo(eTrait).getLargestCityHappiness());
	changeFreeSpecialist(iChange*GC.getTraitInfo(eTrait).getFreeSpecialist());
	changeTradeRoutes(iChange*GC.getTraitInfo(eTrait).getTradeRoutes());
	changeStateReligionHappiness(iChange*GC.getTraitInfo(eTrait).getStateReligionHappiness());
	changeNonStateReligionHappiness(iChange*GC.getTraitInfo(eTrait).getNonStateReligionHappiness());
	changeStateReligionUnitProductionModifier(iChange*GC.getTraitInfo(eTrait).getStateReligionUnitProductionModifier());
	changeStateReligionBuildingProductionModifier(iChange*GC.getTraitInfo(eTrait).getStateReligionBuildingProductionModifier());
	changeStateReligionFreeExperience(iChange*GC.getTraitInfo(eTrait).getStateReligionFreeExperience());
	changeExpInBorderModifier(iChange*GC.getTraitInfo(eTrait).getExpInBorderModifier());
	changeTraitExtraCityDefense(iChange*GC.getTraitInfo(eTrait).getCityDefenseBonus());
	changeMilitaryProductionModifier(iChange*GC.getTraitInfo(eTrait).getMilitaryProductionModifier());
	changeAIAttitudeModifier(iChange*GC.getTraitInfo(eTrait).getAttitudeModifier());
	changeNationalEspionageDefense(iChange*GC.getTraitInfo(eTrait).getEspionageDefense());
	changeMaxTradeRoutesAdjustment(iChange*GC.getTraitInfo(eTrait).getMaxTradeRoutesChange());
	changeGoldenAgeModifier(GC.getTraitInfo(eTrait).getGoldenAgeDurationModifier() * iChange);
	changeNationalHurryAngerModifier(GC.getTraitInfo(eTrait).getHurryAngerModifier() * iChange);
	changeHurryCostModifier(GC.getTraitInfo(eTrait).getHurryCostModifier() * iChange);
	changeForeignTradeRouteModifier(GC.getTraitInfo(eTrait).getForeignTradeRouteModifier() * iChange);
	changeNationalEnemyWarWearinessModifier(GC.getTraitInfo(eTrait).getEnemyWarWearinessModifier() * iChange);
	changeNationalBombardDefenseModifier(GC.getTraitInfo(eTrait).getBombardDefense() * iChange);
	changeUnitUpgradePriceModifier(GC.getTraitInfo(eTrait).getUnitUpgradePriceModifier() * iChange);
	changeCoastalTradeRoutes(GC.getTraitInfo(eTrait).getCoastalTradeRoutes() * iChange);
	changeMilitaryFoodProductionCount((GC.getTraitInfo(eTrait).isMilitaryFoodProduction()) ? iChange : 0);
	changeInquisitionCount((GC.getTraitInfo(eTrait).isAllowsInquisitions())? iChange : 0);

	changePopulationgrowthratepercentage(GC.getTraitInfo(eTrait).getGlobalPopulationgrowthratepercentage(),(iChange==1));
	changeNationalCityStartCulture(GC.getTraitInfo(eTrait).getCityStartCulture() * iChange);
	changeNationalAirUnitCapacity(GC.getTraitInfo(eTrait).getGlobalAirUnitCapacity() * iChange);
	changeCapitalXPModifier(GC.getTraitInfo(eTrait).getCapitalXPModifier() * iChange);
	changeStateReligionHolyCityXPModifier(GC.getTraitInfo(eTrait).getHolyCityofStateReligionXPModifier() * iChange);
	changeNonStateReligionHolyCityXPModifier(GC.getTraitInfo(eTrait).getHolyCityofNonStateReligionXPModifier() * iChange);
	changeNationalCityStartBonusPopulation(GC.getTraitInfo(eTrait).getBonusPopulationinNewCities() * iChange);
	changeNationalMissileRangeChange(GC.getTraitInfo(eTrait).getMissileRange() * iChange);
	changeNationalFlightOperationRangeChange(GC.getTraitInfo(eTrait).getFlightOperationRange() * iChange);
	changeNationalNavalCargoSpaceChange(GC.getTraitInfo(eTrait).getNavalCargoSpace() * iChange);
	changeNationalMissileCargoSpaceChange(GC.getTraitInfo(eTrait).getMissileCargoSpace() * iChange);
	changeExtraFreedomFighters(GC.getTraitInfo(eTrait).getFreedomFighterChange() * iChange);

	changeExtraNationalCaptureProbabilityModifier(GC.getTraitInfo(eTrait).getNationalCaptureProbabilityModifier() * iChange);
	changeExtraNationalCaptureResistanceModifier(GC.getTraitInfo(eTrait).getNationalCaptureResistanceModifier() * iChange);

	changeExtraStateReligionSpreadModifier(GC.getTraitInfo(eTrait).getStateReligionSpreadProbabilityModifier() * iChange);
	changeExtraNonStateReligionSpreadModifier(GC.getTraitInfo(eTrait).getNonStateReligionSpreadProbabilityModifier() * iChange);
	changeCitiesStartwithStateReligionCount((GC.getTraitInfo(eTrait).isCitiesStartwithStateReligion())? iChange : 0);
	changeDraftsOnCityCaptureCount((GC.getTraitInfo(eTrait).isDraftsOnCityCapture())? iChange : 0);
	changeFreeSpecialistperWorldWonderCount((GC.getTraitInfo(eTrait).isFreeSpecialistperWorldWonder())? iChange : 0);
	changeFreeSpecialistperNationalWonderCount((GC.getTraitInfo(eTrait).isFreeSpecialistperNationalWonder())? iChange : 0);
	changeFreeSpecialistperTeamProjectCount((GC.getTraitInfo(eTrait).isFreeSpecialistperTeamProject())? iChange : 0);
	changeExtraGoodyCount((GC.getTraitInfo(eTrait).isExtraGoody())? iChange : 0);

	changeAllReligionsActiveCount((GC.getTraitInfo(eTrait).isAllReligionsActive())? iChange : 0);
	changeAllReligionsActiveCount((GC.getTraitInfo(eTrait).isBansNonStateReligions())? -iChange : 0);
	changeFreedomFighterCount(GC.getTraitInfo(eTrait).isFreedomFighter() ? iChange : 0);

	foreach_(const ImprovementModifier& pair, GC.getTraitInfo(eTrait).getImprovementUpgradeModifiers())
	{
		changeImprovementUpgradeRateModifierSpecific(pair.first, iChange * pair.second);
	}

	foreach_(const BuildModifier2& pair, GC.getTraitInfo(eTrait).getBuildWorkerSpeedModifiers())
	{
		changeBuildWorkerSpeedModifierSpecific(pair.first, iChange * pair.second);
	}

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			changeExtraSpecialistYield(((SpecialistTypes)iI), ((YieldTypes)iJ), (GC.getTraitInfo(eTrait).getSpecialistYieldChange(iI, iJ) * iChange));
		}
		for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			changeExtraSpecialistCommerce(((SpecialistTypes)iI), ((CommerceTypes)iJ), (GC.getTraitInfo(eTrait).getSpecialistCommerceChange(iI, iJ) * iChange));
		}

		if ((SpecialistTypes)GC.getTraitInfo(eTrait).getEraAdvanceFreeSpecialistType() == ((SpecialistTypes)iI))
		{
			changeEraAdvanceFreeSpecialistCount((SpecialistTypes)GC.getTraitInfo(eTrait).getEraAdvanceFreeSpecialistType(), iChange);
		}
	}

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		const YieldTypes eYield = static_cast<YieldTypes>(iI);
		changeSeaPlotYield(eYield, GC.getTraitInfo(eTrait).getSeaPlotYieldChanges(iI) * iChange);
		changeFreeCityYield(eYield, GC.getTraitInfo(eTrait).getYieldChange(iI) * iChange);
		changeYieldRateModifier(eYield, GC.getTraitInfo(eTrait).getYieldModifier(iI) * iChange);
		changeCapitalYieldRateModifier(eYield, GC.getTraitInfo(eTrait).getCapitalYieldModifier(iI) * iChange);
		changeSpecialistExtraYield(eYield, GC.getTraitInfo(eTrait).getSpecialistExtraYield(iI) * iChange);
		updateExtraYieldThreshold(eYield);
		updateLessYieldThreshold(eYield);
		changeGoldenAgeYield(eYield, GC.getTraitInfo(eTrait).getGoldenAgeYieldChanges(iI) * iChange);
	}

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		changeCapitalCommerceRateModifier((CommerceTypes)iI, GC.getTraitInfo(eTrait).getCapitalCommerceModifier(iI) * iChange);
		changeSpecialistExtraCommerce((CommerceTypes)iI, GC.getTraitInfo(eTrait).getSpecialistExtraCommerce(iI) * iChange);
		changeGoldenAgeCommerce((CommerceTypes)iI, GC.getTraitInfo(eTrait).getGoldenAgeCommerceChanges(iI) * iChange);
	}

	int iGPRateChange = GC.getTraitInfo(eTrait).getGreatPeopleRateChange();
	if (iGPRateChange > 0)
	{
		UnitTypes eGreatPeopleUnit = (UnitTypes)GC.getTraitInfo(eTrait).getGreatPeopleUnitType();
		changeNationalGreatPeopleUnitRate(eGreatPeopleUnit, GC.getTraitInfo(eTrait).getGreatPeopleRateChange() * iChange);
	}

	const UnitTypes eGreatPeopleUnit = (UnitTypes)GC.getTraitInfo(eTrait).getGoldenAgeonBirthofGreatPeopleType();
	if (eGreatPeopleUnit != NO_UNIT)
	{
		changeGoldenAgeOnBirthOfGreatPersonCount(eGreatPeopleUnit, iChange);
	}

	foreach_(const DomainModifier2& pair, GC.getTraitInfo(eTrait).getDomainFreeExperience())
	{
		changeNationalDomainFreeExperience(pair.first, pair.second);
	}

	foreach_(const DomainModifier2& pair, GC.getTraitInfo(eTrait).getDomainProductionModifiers())
	{
		changeNationalDomainProductionModifier(pair.first, pair.second);
	}

	foreach_(const TechModifier& pair, GC.getTraitInfo(eTrait).getTechResearchModifiers())
	{
		changeNationalTechResearchModifier(pair.first, pair.second);
	}

	for (int iI = 0; iI < GC.getTraitInfo(eTrait).getNumUnitCombatFreeExperiences(); iI++)
	{
		if ((UnitCombatTypes)GC.getTraitInfo(eTrait).getUnitCombatFreeExperience(iI).eUnitCombat != NO_UNITCOMBAT)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)GC.getTraitInfo(eTrait).getUnitCombatFreeExperience(iI).eUnitCombat);
			if (GC.getTraitInfo(eTrait).getUnitCombatFreeExperience(iI).iModifier != 0)
			{
				changeUnitCombatFreeExperience(eUnitCombat, iChange*GC.getTraitInfo(eTrait).getUnitCombatFreeExperience(iI).iModifier);
			}
		}
	}

	for (int iI = 0; iI < GC.getTraitInfo(eTrait).getNumUnitCombatProductionModifiers(); iI++)
	{
		const UnitCombatTypes eUnitCombat = (UnitCombatTypes)GC.getTraitInfo(eTrait).getUnitCombatProductionModifier(iI).eUnitCombat;
		if (eUnitCombat != NO_UNITCOMBAT)
		{
			if (GC.getTraitInfo(eTrait).getUnitCombatProductionModifier(iI).iModifier != 0)
			{
				changeUnitCombatProductionModifier(eUnitCombat, iChange*GC.getTraitInfo(eTrait).getUnitCombatProductionModifier(iI).iModifier);
			}
		}
	}

	//Run through Unit Promotion Changes
	algo::for_each(units(), CvUnit::fn::doSetFreePromotions(iChange > 0, eTrait));
}

void CvPlayer::recalculateModifiers()
{
	PROFILE_EXTRA_FUNC();
	OutputDebugString("\nStarting recalculateModifiers...");

	noteOrbitalInfrastructureCountDirty();

	//	Put back starting defaults
	changeBaseFreeUnitUpkeepCivilian(GC.getBASE_FREE_UNITS_UPKEEP_CIVILIAN());
	changeBaseFreeUnitUpkeepMilitary(GC.getBASE_FREE_UNITS_UPKEEP_MILITARY());
	changeFreeUnitUpkeepCivilianPopPercent(GC.getBASE_FREE_UNITS_UPKEEP_CIVILIAN_PER_100_POP());
	changeFreeUnitUpkeepMilitaryPopPercent(GC.getBASE_FREE_UNITS_UPKEEP_MILITARY_PER_100_POP());
	changeTradeRoutes(GC.getINITIAL_TRADE_ROUTES());
	changeStateReligionHappiness(GC.getINITIAL_STATE_RELIGION_HAPPINESS());
	changeNonStateReligionHappiness(GC.getINITIAL_NON_STATE_RELIGION_HAPPINESS());

	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		changeTradeYieldModifier(((YieldTypes)iI), GC.getYieldInfo((YieldTypes)iI).getTradeModifier() - getTradeYieldModifier((YieldTypes)iI));
	}

	//	Put back any unattributed (ie - from Python) happiness
	changeExtraHappiness(m_iExtraHappinessUnattributed);

	//	Put back trait-sourced modifiers
	// Game Compatability issue resolution
	//bool bHasCompiled = false;
	//Apparently some games have displayed the ability to have a non-leaderhead (iLinePriority 0) trait being assigned to leaders in a Developing Leaderhead Game.
	//This section should at least convert them if this takes place.  In the meantime... need to figure out HOW that's happening at all.  I've seen rev leaders initiated in the middle of the game come in with extra traits... may have something to do with that?
	if (GC.getGame().isOption(GAMEOPTION_LEADER_DEVELOPING))
	{
		for (int iI = 0; iI <GC.getNumTraitInfos(); iI++)
		{
			const TraitTypes eTrait = ((TraitTypes)iI);
			////removes the extra negative trait if it shouldn't be there.  After a version or two, this should be removed entirely and m_iCompatCheckCount can be repurposed later.
			//if (hasTrait(eTrait) && GC.getTraitInfo(eTrait).isNegativeTrait() && GC.getGame().isOption(GAMEOPTION_LEADER_START_NO_POSITIVE_TRAITS))
			//{
			//	if (GC.getLeaderHeadInfo(getLeaderType()).hasTrait(eTrait) && m_iCompatCheckCount != 1)
			//	{
			//		m_pabHasTrait[eTrait] = false;
			//		m_iCompatCheckCount = 1;
			//	}
			//}
			if (!GC.getTraitInfo(eTrait).isCivilizationTrait() && !GC.getTraitInfo(eTrait).isBarbarianSelectionOnly())
			{
				if (hasTrait(eTrait) && GC.getTraitInfo(eTrait).getLinePriority() == 0 && GC.getTraitInfo(eTrait).getPromotionLine() != NO_PROMOTIONLINE)
				{
					for (int iJ = 0; iJ < GC.getNumTraitInfos(); iJ++)
					{
						const TraitTypes eReplacementTrait = ((TraitTypes)iJ);
						if (GC.getTraitInfo(eReplacementTrait).getPromotionLine() != NO_PROMOTIONLINE)
						{
							if (GC.getTraitInfo(eReplacementTrait).getPromotionLine() == GC.getTraitInfo(eTrait).getPromotionLine())
							{
								if (GC.getTraitInfo(eReplacementTrait).isNegativeTrait() && GC.getTraitInfo(eTrait).isNegativeTrait() && GC.getTraitInfo(eReplacementTrait).getLinePriority() == -1)
								{
									m_pabHasTrait[eReplacementTrait] = true;
									m_pabHasTrait[eTrait] = false;
								}
								else if (!GC.getTraitInfo(eReplacementTrait).isNegativeTrait() && !GC.getTraitInfo(eTrait).isNegativeTrait() && GC.getTraitInfo(eReplacementTrait).getLinePriority() == 1)
								{
									m_pabHasTrait[eReplacementTrait] = true;
									m_pabHasTrait[eTrait] = false;
								}
							}
						}
					}
				}
			}
		}
	}
	else //Since Options can be changed in game now, this section should be able to restructure leaders according to original non-leaderhead development traits if changed back.
	{
		for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
			const TraitTypes eTrait = ((TraitTypes)iI);
			bool bLeaderHasTrait = false;
			if (!GC.getTraitInfo(eTrait).isCivilizationTrait() && !GC.getTraitInfo(eTrait).isBarbarianSelectionOnly())
			{
				if (hasTrait(eTrait) && GC.getTraitInfo(eTrait).getLinePriority() != 0)
				{
					m_pabHasTrait[eTrait] = false;
				}
				if (GC.getGame().isOption(GAMEOPTION_LEADER_COMPLEX_TRAITS))
				{
					if (GC.getLeaderHeadInfo(getLeaderType()).isDefaultComplexTrait((int)eTrait))
					{
						bLeaderHasTrait = true;
					}
				}
				else if (GC.getLeaderHeadInfo(getLeaderType()).isDefaultTrait((int)eTrait))
				{
					bLeaderHasTrait = true;
				}
				else if (GC.getLeaderHeadInfo(getLeaderType()).hasTrait(eTrait))
				{
					bLeaderHasTrait = true;
				}
				if (bLeaderHasTrait && !hasTrait(eTrait) && GC.getTraitInfo(eTrait).getLinePriority() == 0)
				{
					if (GC.getTraitInfo(eTrait).isNegativeTrait())
					{
						if (!GC.getGame().isOption(GAMEOPTION_LEADER_NO_NEGATIVE_TRAITS))
						{
							m_pabHasTrait[eTrait] = true;
						}
					}
					else if (!GC.getGame().isOption(GAMEOPTION_LEADER_START_NO_POSITIVE_TRAITS))
					{
						m_pabHasTrait[eTrait] = true;
					}
				}
			}
		}
	}
	//Remove trait promos during recalc - it will add them back when processing in the traits.
	algo::for_each(units(), CvUnit::fn::doSetFreePromotions(false, NO_TRAIT));

	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			processTrait((TraitTypes)iI, 1);
		}
	}
	foreach_(const HeritageTypes eTypeX, getHeritage())
	{
		processHeritage(eTypeX, 1);
	}

	// Put back city-sourced modifiers
	algo::for_each(cities(), CvCity::fn::recalculateModifiers());

	// Put back civic-sourced modifiers
	for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		const CivicTypes eCivic = getCivics((CivicOptionTypes)iI);

		if ( eCivic != NO_CIVIC )
		{
			processCivics(eCivic, 1);
		}
	}
// TB Nukefix
//	m_bNukesValid = false;

	//	Replay events in so far as they effect modifiers
	for (CvEventMap::const_iterator it = m_mapEventsOccured.begin(); it != m_mapEventsOccured.end(); ++it)
	{
		applyEvent((*it).first, -1, false);
	}

	//	Recalculate unitAI counts
	//	Note - would ideally have this be a virtual to avoid the cast but adding virtuals to this
	//	class screws up the core engine which accessing it via the DLL interafce
	((CvPlayerAI*)this)->AI_recalculateUnitCounts();
	recalculateUnitCounts();

	setMaintenanceDirty(true);

	AI_updateBonusValue();
	AI_updateFoundValues(true);
#ifdef OUTBREAKS_AND_AFFLICTIONS
	recalculateAfflictedUnitCount();
#endif
	//	Re-establish blockades
	updatePlunder(1, false);
	resetCivTypeEffects();

	// Recalc plot commander counts
	for (int i = m_commanders.size() - 1; i > -1; i--)
	{
		CvUnit* com = m_commanders[i];

		if (com->isCommanderReady())
		{
			com->plot()->countCommander(true, com);
		}
	}

    // Recalc plot commodore counts
	for (int i = m_commodores.size() - 1; i > -1; i--)
	{
		CvUnit* com = m_commodores[i];

		if (com->isCommodoreReady())
		{
			com->plot()->countCommodore(true, com);
		}
	}
}

bool CvPlayer::upgradeAvailable(UnitTypes eFromUnit, UnitTypes eToUnit) const
{
	if ( !m_upgradeCache )
	{
		m_upgradeCache.reset(new CvUpgradeCache());
	}
	return m_upgradeCache->upgradeAvailable(eFromUnit, eToUnit);
}

CvProperties* CvPlayer::getProperties()
{
	return &m_Properties;
}

const CvProperties* CvPlayer::getPropertiesConst() const
{
	return &m_Properties;
}

void CvPlayer::addPropertiesAllCities(const CvProperties *pProp)
{
	PROFILE_EXTRA_FUNC();
	foreach_(CvCity* city, cities())
	{
		city->getProperties()->addProperties(pProp);
	}
	//getProperties()->addProperties(pProp);
}

void CvPlayer::subtractPropertiesAllCities(const CvProperties *pProp)
{
	PROFILE_EXTRA_FUNC();
	foreach_(CvCity* city, cities())
	{
		city->getProperties()->subtractProperties(pProp);
	}
	//getProperties()->subtractProperties(pProp);
}


void CvPlayer::setBuildingListInvalid()
{
	m_BuildingList.setInvalid();
}

bool CvPlayer::getBuildingListFilterActive(BuildingFilterTypes eFilter)
{
	return m_BuildingList.getFilterActive(eFilter);
}

void CvPlayer::setBuildingListFilterActive(BuildingFilterTypes eFilter, bool bActive)
{
	m_BuildingList.setFilterActive(eFilter, bActive);
}

BuildingGroupingTypes CvPlayer::getBuildingListGrouping()
{
	return m_BuildingList.getGroupingActive();
}

void CvPlayer::setBuildingListGrouping(BuildingGroupingTypes eGrouping)
{
	m_BuildingList.setGroupingActive(eGrouping);
}

BuildingSortTypes CvPlayer::getBuildingListSorting()
{
	return m_BuildingList.getSortingActive();
}

void CvPlayer::setBuildingListSorting(BuildingSortTypes eSorting)
{
	m_BuildingList.setSortingActive(eSorting);
}

int CvPlayer::getBuildingListGroupNum()
{
	return m_BuildingList.getGroupNum();
}

int CvPlayer::getBuildingListNumInGroup(int iGroup)
{
	return m_BuildingList.getNumInGroup(iGroup);
}

BuildingTypes CvPlayer::getBuildingListType(int iGroup, int iPos)
{
	return m_BuildingList.getBuildingType(iGroup, iPos);
}

int CvPlayer::getBuildingListSelectedBuildingRow()
{
	return m_BuildingList.getBuildingSelectionRow();
}

int CvPlayer::getBuildingListSelectedWonderRow()
{
	return m_BuildingList.getWonderSelectionRow();
}

BuildingTypes CvPlayer::getBuildingListSelectedBuilding()
{
	return m_BuildingList.getSelectedBuilding();
}

BuildingTypes CvPlayer::getBuildingListSelectedWonder()
{
	return m_BuildingList.getSelectedWonder();
}

void CvPlayer::setBuildingListSelectedBuilding(BuildingTypes eBuilding)
{
	m_BuildingList.setSelectedBuilding(eBuilding);
}

void CvPlayer::setBuildingListSelectedWonder(BuildingTypes eWonder)
{
	m_BuildingList.setSelectedWonder(eWonder);
}


void CvPlayer::setUnitListInvalid()
{
	m_UnitList.setInvalid();
}

bool CvPlayer::getUnitListFilterActive(UnitFilterTypes eFilter)
{
	return m_UnitList.getFilterActive(eFilter);
}

void CvPlayer::setUnitListFilterActive(UnitFilterTypes eFilter, bool bActive)
{
	m_UnitList.setFilterActive(eFilter, bActive);
}

UnitGroupingTypes CvPlayer::getUnitListGrouping()
{
	return m_UnitList.getGroupingActive();
}

void CvPlayer::setUnitListGrouping(UnitGroupingTypes eGrouping)
{
	m_UnitList.setGroupingActive(eGrouping);
}

UnitSortTypes CvPlayer::getUnitListSorting()
{
	return m_UnitList.getSortingActive();
}

void CvPlayer::setUnitListSorting(UnitSortTypes eSorting)
{
	m_UnitList.setSortingActive(eSorting);
}

int CvPlayer::getUnitListGroupNum()
{
	return m_UnitList.getGroupNum();
}

int CvPlayer::getUnitListNumInGroup(int iGroup)
{
	return m_UnitList.getNumInGroup(iGroup);
}

UnitTypes CvPlayer::getUnitListType(int iGroup, int iPos)
{
	return m_UnitList.getUnitListType(iGroup, iPos);
}

int CvPlayer::getUnitListSelectedRow()
{
	return m_UnitList.getSelectionRow();
}

UnitTypes CvPlayer::getUnitListSelected()
{
	return m_UnitList.getSelectedUnit();
}

void CvPlayer::setUnitListSelected(UnitTypes eUnit)
{
	m_UnitList.setSelectedUnit(eUnit);
}

void CvPlayer::processNewRoutes()
{
	PROFILE_EXTRA_FUNC();
	//	Invalidate best route cache
	m_eBestRoute = NO_ROUTE;

	//	Process auto-upgrade of city tile routes
	foreach_(const CvCity* pLoopCity, cities())
	{
		pLoopCity->plot()->updateCityRoute(true);
	}
}

bool CvPlayer::canHaveBuilder(BuildTypes eBuild) const
{
	PROFILE_EXTRA_FUNC();
	std::map<int, bool>::const_iterator itr = m_canHaveBuilder.find((int)eBuild);

	if (itr != m_canHaveBuilder.end())
	{
		return itr->second;
	}
	bool hasSuitableUnit = algo::any_of(units(), CvUnit::fn::hasBuild(eBuild));

	if (!hasSuitableUnit)
	{
		// Could we build one?
		for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			if (GC.getUnitInfo((UnitTypes)iI).hasBuild(eBuild) && canTrain((UnitTypes)iI))
			{
				hasSuitableUnit = true;
				break;
			}
		}
		m_canHaveBuilder[eBuild] = hasSuitableUnit;
	}
	return hasSuitableUnit;
}


//TB Nukefix
bool CvPlayer::isNukesValid() const
{
	if (GC.getGame().isOption(GAMEOPTION_NO_NUKES))
	{
		return false;
	}
	return m_bNukesValid;
}


void CvPlayer::makeNukesValid(bool bValid)
{
	m_bNukesValid = bValid;
}

typedef struct buildingCommerceStruct
{
	int				iMultiplier;
	int				iGlobalMultiplier;
	float			fContribution;
} buildingCommerceStruct;

#ifdef OUTBREAKS_AND_AFFLICTIONS
int CvPlayer::getPlayerWideAfflictionCount(PromotionLineTypes ePromotionLineType) const
{
	FASSERT_BOUNDS(0, GC.getNumPromotionLineInfos(), ePromotionLineType);
	return m_paiPlayerWideAfflictionCount[ePromotionLineType];
}

void CvPlayer::changePlayerWideAfflictionCount(PromotionLineTypes ePromotionLineType, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumPromotionLineInfos(), ePromotionLineType);

	if (iChange != 0)
	{
		m_paiPlayerWideAfflictionCount[ePromotionLineType] += iChange;
		FASSERT_NOT_NEGATIVE(getPlayerWideAfflictionCount(ePromotionLineType));

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

void CvPlayer::setPlayerWideAfflictionCount(PromotionLineTypes ePromotionLineType, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumPromotionLineInfos(), ePromotionLineType);

	if (iChange != 0)
	{
		m_paiPlayerWideAfflictionCount[ePromotionLineType] = iChange;
		FASSERT_NOT_NEGATIVE(getPlayerWideAfflictionCount(ePromotionLineType));

		if (getID() == GC.getGame().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

int CvPlayer::countAfflictedUnits (PromotionLineTypes eAfflictionLine)
{
	return algo::count_if(units(), CvUnit::fn::hasAfflictionLine(eAfflictionLine));
}

void CvPlayer::recalculateAfflictedUnitCount()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
	{
		if (GC.getPromotionLineInfo((PromotionLineTypes)iI).isAffliction())
		{
			PromotionLineTypes eAfflictionLine = ((PromotionLineTypes)iI);
			const int iRecalc = countAfflictedUnits(eAfflictionLine);
			setPlayerWideAfflictionCount(eAfflictionLine, iRecalc);
		}
	}
}
#endif

CvCity*	CvPlayer::findClosestCity(const CvPlot* pPlot) const
{
	PROFILE_EXTRA_FUNC();
	int	iClosest = MAX_INT;
	CvCity*	pResult = NULL;

	if ( pPlot == NULL )
	{
		int iLoop;
		return firstCity(&iLoop);
	}

	foreach_(CvCity* pLoopCity, cities())
	{
		const int iDistance = plotDistance(pLoopCity->getX(), pLoopCity->getY(), pPlot->getX(), pPlot->getY());

		if ( iDistance < iClosest )
		{
			iClosest = iDistance;
			pResult = pLoopCity;
		}
	}

	return pResult;
}

int CvPlayer::getCivicAnarchyModifier( ) const
{
	return m_iCivicAnarchyModifier;
}

void CvPlayer::setCivicAnarchyModifier( int iNewValue )
{
	m_iCivicAnarchyModifier = iNewValue;
}

void CvPlayer::changeCivicAnarchyModifier( int iChange )
{
	setCivicAnarchyModifier( getCivicAnarchyModifier() + iChange );
}

int CvPlayer::getReligiousAnarchyModifier( ) const
{
	return m_iReligiousAnarchyModifier;
}

void CvPlayer::setReligiousAnarchyModifier( int iNewValue )
{
	m_iReligiousAnarchyModifier = iNewValue;
}

void CvPlayer::changeReligiousAnarchyModifier( int iChange )
{
	setReligiousAnarchyModifier( getReligiousAnarchyModifier() + iChange );
}

int CvPlayer::getImprovementUpgradeRateModifierSpecific(ImprovementTypes eImprovement) const
{
	return m_paiImprovementUpgradeRateModifierSpecific[eImprovement];
}

void CvPlayer::changeImprovementUpgradeRateModifierSpecific(ImprovementTypes eImprovement, int iChange)
{
	m_paiImprovementUpgradeRateModifierSpecific[eImprovement] = (m_paiImprovementUpgradeRateModifierSpecific[eImprovement] + iChange);
}

int CvPlayer::getBuildWorkerSpeedModifierSpecific(BuildTypes eBuild) const
{
	return m_paiBuildWorkerSpeedModifierSpecific[eBuild];
}

void CvPlayer::changeBuildWorkerSpeedModifierSpecific(BuildTypes eBuild, int iChange)
{
	m_paiBuildWorkerSpeedModifierSpecific[eBuild] = (m_paiBuildWorkerSpeedModifierSpecific[eBuild] + iChange);
}

int CvPlayer::getAIAttitudeModifier( ) const
{
	return m_iAIAttitudeModifier;
}

void CvPlayer::setAIAttitudeModifier( int iNewValue )
{
	m_iAIAttitudeModifier = iNewValue;
}

void CvPlayer::changeAIAttitudeModifier( int iChange )
{
	setAIAttitudeModifier( getAIAttitudeModifier() + iChange );
}


int CvPlayer::getExtraSpecialistCommerce(SpecialistTypes eIndex1, CommerceTypes eIndex2) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex1);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex2);
	return m_ppaaiSpecialistExtraCommerce[eIndex1][eIndex2];
}


void CvPlayer::changeExtraSpecialistCommerce(SpecialistTypes eIndex1, CommerceTypes eIndex2, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex1);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex2);

	if (iChange != 0)
	{
		m_ppaaiSpecialistExtraCommerce[eIndex1][eIndex2] += iChange;
		//TB Note: should be allowed to be negative.
		//FASSERT_NOT_NEGATIVE(getExtraSpecialistCommerce(eIndex1, eIndex2));

		updateExtraSpecialistCommerce();

		AI_makeAssignWorkDirty();
	}
}

void CvPlayer::updateExtraSpecialistCommerce()
{
	algo::for_each(cities(), CvCity::fn::updateExtraSpecialistCommerce());
}

int CvPlayer::getSpecialistExtraYield(YieldTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_aiSpecialistExtraYield[eIndex];
}

void CvPlayer::changeSpecialistExtraYield(YieldTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiSpecialistExtraYield[eIndex] += iChange;

		algo::for_each(cities(), CvCity::fn::onYieldChange());

		AI_makeAssignWorkDirty();
	}
}

int CvPlayer::getFreeCityYield(YieldTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);

	return m_aiFreeCityYield[eIndex];
}

void CvPlayer::changeFreeCityYield(YieldTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiFreeCityYield[eIndex] += iChange;

		algo::for_each(cities(), CvCity::fn::onYieldChange());
	}
}

int CvPlayer::getTraitExtraCityDefense() const
{
	return m_iTraitExtraCityDefense;
}

void CvPlayer::setTraitExtraCityDefense(int iValue)
{
	m_iTraitExtraCityDefense = iValue;
}

void CvPlayer::changeTraitExtraCityDefense(int iChange)
{
	setTraitExtraCityDefense(getTraitExtraCityDefense() + iChange);
}

void CvPlayer::setHasTrait(TraitTypes eIndex, bool bNewValue)
{
	PROFILE_EXTRA_FUNC();
	if (eIndex == NO_TRAIT || hasTrait(eIndex) == bNewValue)
	{
		return;
	}

	if (!bNewValue || (bNewValue && GC.getTraitInfo(eIndex).isValidTrait()))
	{
		m_pabHasTrait[eIndex] = bNewValue;
		processTrait(eIndex, bNewValue ? 1 : -1);

		if (GC.getGame().isOption(GAMEOPTION_LEADER_DEVELOPING) && GC.getTraitInfo(eIndex).getLinePriority() != 0)
		{
			const bool bNegativeTrait = GC.getTraitInfo(eIndex).isNegativeTrait();

			if (!bNewValue && bNegativeTrait || bNewValue && !bNegativeTrait)
			{
				for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive() && iI != getID() && GET_PLAYER((PlayerTypes)iI).isHumanPlayer()
					&& GET_TEAM(getTeam()).isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
					{
						if (bNegativeTrait)
						{
							AddDLLMessage(
								(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(),
								gDLL->getText(
									"TXT_KEY_MISC_TRAIT_REMOVED",
									GC.getLeaderHeadInfo(getLeaderType()).getTextKeyWide(),
									GC.getTraitInfo(eIndex).getDescription()
								),
								"AS2D_WELOVEKING", MESSAGE_TYPE_MAJOR_EVENT, 0, GC.getCOLOR_HIGHLIGHT_TEXT()
							);
						}
						else
						{
							AddDLLMessage(
								(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(),
								gDLL->getText(
									"TXT_KEY_MISC_TRAIT_ADOPTED",
									GC.getLeaderHeadInfo(getLeaderType()).getTextKeyWide(),
									GC.getTraitInfo(eIndex).getDescription()
								),
								"AS2D_WELOVEKING", MESSAGE_TYPE_MAJOR_EVENT, 0, GC.getCOLOR_HIGHLIGHT_TEXT()
							);
						}
					}
				}
			}
			else
			{
				for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive() && iI != getID() && GET_PLAYER((PlayerTypes)iI).isHumanPlayer()
					&& GET_TEAM(getTeam()).isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
					{
						if (bNegativeTrait)
						{
							AddDLLMessage(
								(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(),
								gDLL->getText(
									"TXT_KEY_MISC_TRAIT_ADOPTED",
									GC.getLeaderHeadInfo(getLeaderType()).getTextKeyWide(),
									GC.getTraitInfo(eIndex).getDescription()
								),
								"AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MAJOR_EVENT, 0, GC.getCOLOR_HIGHLIGHT_TEXT()
							);
						}
						else
						{
							AddDLLMessage(
								(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(),
								gDLL->getText(
									"TXT_KEY_MISC_TRAIT_REMOVED",
									GC.getLeaderHeadInfo(getLeaderType()).getTextKeyWide(),
									GC.getTraitInfo(eIndex).getDescription()
								),
								"AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MAJOR_EVENT, 0, GC.getCOLOR_HIGHLIGHT_TEXT()
							);
						}
					}
				}
			}
		}
	}
}

bool CvPlayer::canLearnTrait(TraitTypes eIndex, bool isSelectingNegative) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(NO_TRAIT, GC.getNumTraitInfos(), eIndex);

	if (eIndex == NO_TRAIT || isNPC() || hasTrait(eIndex))
	{
		return false;
	}

	const CvTraitInfo& kTrait = GC.getTraitInfo(eIndex);

	if (kTrait.getLinePriority() == 0 || !kTrait.isValidTrait()) return false;


	const TraitTypes eTraitPrerequisite = kTrait.getPrereqTrait();
	const TraitTypes eTraitPrerequisite1 = kTrait.getPrereqOrTrait1();
	const TraitTypes eTraitPrerequisite2 = kTrait.getPrereqOrTrait2();

	bool bbypass = true;
	if (eTraitPrerequisite != NO_TRAIT)
	{
		bbypass = false;
		if (hasTrait(eTraitPrerequisite))
		{
			bbypass = true;
		}
	}
	if (!bbypass)//then we DON'T have a prereq that's been removed thanks to the override mechanism.
	{
		return false;// this is the only line to remain if mod is removed
	}

	if (eTraitPrerequisite1 != NO_TRAIT)
	{
		bbypass = false;
		if (hasTrait(eTraitPrerequisite1))
		{
			bbypass = true;
		}

		if (!bbypass && eTraitPrerequisite2 != NO_TRAIT)
		{
			if (hasTrait(eTraitPrerequisite2))
			{
				bbypass = true;
			}
		}

		if (!bbypass)//then we DON'T have a prereq that's been removed thanks to the override mechanism.
		{
			return false;// this is the only line to remain if mod is removed
		}
	}

	if (kTrait.getPrereqTech() != NO_TECH && !GET_TEAM(getTeam()).isHasTech(kTrait.getPrereqTech()))
	{
		return false;
	}

	if (kTrait.getPromotionLine() != NO_PROMOTIONLINE)
	{
		if (GC.getPromotionLineInfo((PromotionLineTypes) kTrait.getPromotionLine()).getPrereqTech() != NO_TECH
		&& !GET_TEAM(getTeam()).isHasTech(GC.getPromotionLineInfo((PromotionLineTypes) kTrait.getPromotionLine()).getPrereqTech()))
		{
			return false;
		}
		for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
			const TraitTypes ePrereq = ((TraitTypes)iI);
			if (!hasTrait(ePrereq) && GC.getTraitInfo(ePrereq).getPromotionLine() != NO_PROMOTIONLINE)
			{
				if (!kTrait.isNegativeTrait() && kTrait.getLinePriority() > 1)
				{
					if (GC.getTraitInfo(ePrereq).getPromotionLine() == kTrait.getPromotionLine()
					&& (GC.getTraitInfo(ePrereq).getLinePriority() == (kTrait.getLinePriority() - 1)))
					{
						return false;
					}
				}
				else if (kTrait.isNegativeTrait() && isSelectingNegative && (kTrait.getLinePriority() < -1))
				{
					if (GC.getTraitInfo(ePrereq).getPromotionLine() == kTrait.getPromotionLine()
					&& (GC.getTraitInfo(ePrereq).getLinePriority() == (kTrait.getLinePriority() + 1)))
					{
						return false;
					}
				}
			}
		}
	}

	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		const TraitTypes eTrait = ((TraitTypes)iI);
		if (hasTrait(eTrait))
		{
			for (int iJ = 0; iJ < GC.getTraitInfo(eTrait).getNumDisallowedTraitTypes(); iJ++)
			{
				if (GC.getTraitInfo(eTrait).isDisallowedTraitType(iJ).eTrait == eIndex)
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool CvPlayer::canUnlearnTrait(TraitTypes eTrait, bool bPositive) const
{
	PROFILE_EXTRA_FUNC();
	if (GC.getTraitInfo(eTrait).getLinePriority() == 0)
	{
		return false;
	}

	if (GC.getTraitInfo(eTrait).getPromotionLine() == NO_PROMOTIONLINE)
	{
		return false;
	}

	if (!hasTrait(eTrait))
	{
		return false;
	}

	if (GC.getGame().isOption(GAMEOPTION_LEADER_DEVELOPING) && GC.getTraitInfo(eTrait).getLinePriority() == 0)
	{
		return false;
	}
	else if (!GC.getGame().isOption(GAMEOPTION_LEADER_DEVELOPING) && GC.getTraitInfo(eTrait).getLinePriority() != 0)
	{
		return false;
	}

	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		const TraitTypes qTrait = ((TraitTypes)iI);
		if (hasTrait(qTrait) && GC.getTraitInfo(eTrait).getPromotionLine() == GC.getTraitInfo(qTrait).getPromotionLine())
		{
			if (bPositive)
			{
				if (GC.getTraitInfo(eTrait).isNegativeTrait())
				{
					if (GC.getTraitInfo(qTrait).getLinePriority() < GC.getTraitInfo(eTrait).getLinePriority())
					{
						return false;
					}
				}
			}
			else
			{
				if (GC.getTraitInfo(qTrait).getLinePriority() > GC.getTraitInfo(eTrait).getLinePriority())
				{
					return false;
				}
			}
		}
	}
	if (bPositive)
	{
		if (!GC.getTraitInfo(eTrait).isNegativeTrait())
		{
			return false;
		}
	}
	else
	{
		if (GC.getTraitInfo(eTrait).isNegativeTrait())
		{
			return false;
		}
	}
	return true;
}


int CvPlayer::getLeaderHeadLevel() const
{
	if (!GC.getGame().isOption(GAMEOPTION_LEADER_DEVELOPING))
	{
		return 0;
	}
	return m_iLeaderHeadLevel;
}

void CvPlayer::setLeaderHeadLevel(int iValue)
{
	m_iLeaderHeadLevel = iValue;
}

void CvPlayer::changeLeaderHeadLevel(int iChange)
{
	setLeaderHeadLevel(getLeaderHeadLevel() + iChange);
}

uint64_t CvPlayer::getLeaderLevelupNextCultureTotal() const
{
	PROFILE_EXTRA_FUNC();
	uint64_t iPromoThreshold = 1000;
	uint64_t iX = 1000;
	int iY = 10 * GC.getNEXT_TRAIT_CULTURE_REQ_PERCENT() / 100;

	if (GC.getGame().isOption(GAMEOPTION_LEADER_START_NO_POSITIVE_TRAITS))
	{
		iX = 10;
		iY = 8 * GC.getNEXT_TRAIT_CULTURE_REQ_PERCENT() / 100;
	}
	const int iIteratorA = getLeaderHeadLevel() + 1;
	for (int x = 0; x < iIteratorA; x++)
	{
		const uint64_t iZ = (iX * iY);
		iPromoThreshold = iX + iZ;
		iX = iPromoThreshold;
		iY--;
		iY = std::max(1, iY);
	}
	return iPromoThreshold * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent() / 25;
}

uint64_t CvPlayer::getLeaderLevelupCultureToEarn() const
{
	const int64_t iCurrent = getCulture();
	const uint64_t iNext = getLeaderLevelupNextCultureTotal();

	if (iCurrent < 0)
	{
		FErrorMsg("Negative total national culture, Ss baad m'kay.");
		return iNext;
	}
	if (iNext <= static_cast<uint64_t>(iCurrent))
	{
		return 0;
	}
	return iNext - iCurrent;
}

bool CvPlayer::canLeaderPromote() const
{
	return GC.getGame().isOption(GAMEOPTION_LEADER_DEVELOPING) && getLeaderLevelupCultureToEarn() == 0;
}

void CvPlayer::doPromoteLeader()
{
	PROFILE_EXTRA_FUNC();
	int iValue = 0;
	int iBestValue = 0;
	int iFlavorValue = 0;
	TraitTypes eBestTrait = NO_TRAIT;
	TraitTypes eTrait = NO_TRAIT;

	changeLeaderHeadLevel(1);
	if (isHumanPlayer())
	{
		CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSE_TRAIT);
		pInfo->setData1(getID());
		gDLL->getInterfaceIFace()->addPopup(pInfo, getID());
	}
	else
	{
		for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
		//go over all traits that can be selected:
			eTrait = ((TraitTypes)iI);
			if (canLearnTrait(eTrait) || canUnlearnTrait(eTrait, true))
			{
				iValue = 0;
				for (int iJ = 0; iJ < GC.getNumFlavorTypes(); iJ++)
				{
					FlavorTypes eFlavor = ((FlavorTypes)iJ);
					iFlavorValue = (GC.getLeaderHeadInfo(getPersonalityType()).getFlavorValue(eFlavor)+1) * (GC.getTraitInfo(eTrait).getFlavorValue(eFlavor)+1);
					if (GC.getTraitInfo(eTrait).getLinePriority() > 0)
					{
						int iLevelModifier = getLeaderHeadLevel();
						if (GC.getGame().isOption(GAMEOPTION_LEADER_START_NO_POSITIVE_TRAITS))
						{
							iLevelModifier -= 2;
						}
						int iMultModifier = std::max(1, iLevelModifier);
						int iPriorityModifier = 0;
						if (GC.getTraitInfo(eTrait).isNegativeTrait())
						{
							iPriorityModifier = 1;
						}
						else
						{
							iPriorityModifier = GC.getTraitInfo(eTrait).getLinePriority();
						}
						int iMult = (iPriorityModifier * 10 * iMultModifier);
						iFlavorValue *= iMult;
					}
					if (GC.getTraitInfo(eTrait).isCoastalAIInfluence())
					{
						iFlavorValue *= getCoastalAIInfluence();
						iFlavorValue /= 100;
					}
					int iRandomModifier = GC.getGame().getSorenRandNum(200, "Trait AI Selection Modifier");
					iFlavorValue *= iRandomModifier;
					iFlavorValue /= 100;
					iValue += iFlavorValue;
				}
				if (iValue > iBestValue)
				{
					eBestTrait = eTrait;
					iBestValue = iValue;
				}
			}
		}

		if (eBestTrait != NO_TRAIT)
		{
			if (GC.getTraitInfo(eBestTrait).isNegativeTrait())
			{
				setHasTrait(eBestTrait, false);
			}
			else
			{
				setHasTrait(eBestTrait, true);
			}
		}
	}
	//Negative Trait select/assign
	if (!GC.getGame().isOption(GAMEOPTION_LEADER_NO_NEGATIVE_TRAITS))
	{
		int iLL = getLeaderHeadLevel();
		if (iLL > 1 && iLL%2 == 0)
		{
			if (isHumanPlayer())
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSE_TRAIT_NEGATIVE);
				pInfo->setData1(getID());
				gDLL->getInterfaceIFace()->addPopup(pInfo, getID());
				return;
			}
			for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
			{
				eTrait = ((TraitTypes)iI);

				if (canLearnTrait(eTrait) || canUnlearnTrait(eTrait, false))
				{
					iValue = 0;
					for (int iJ = 0; iJ < GC.getNumFlavorTypes(); iJ++)
					{
						FlavorTypes eFlavor = ((FlavorTypes)iJ);
						iFlavorValue = (GC.getLeaderHeadInfo(getPersonalityType()).getFlavorValue(eFlavor)+1) * (100 - GC.getTraitInfo(eTrait).getFlavorValue(eFlavor)+1);
						if (GC.getTraitInfo(eTrait).getLinePriority() < 0)
						{
							int iLevelModifier = getLeaderHeadLevel();
							if (GC.getGame().isOption(GAMEOPTION_LEADER_START_NO_POSITIVE_TRAITS))
							{
								iLevelModifier -= 2;
							}
							int iMultModifier = std::max(1, iLevelModifier);
							int iPriorityModifier = 0;
							if (GC.getTraitInfo(eTrait).isNegativeTrait())
							{
								iPriorityModifier = -GC.getTraitInfo(eTrait).getLinePriority();
							}
							else
							{
								iPriorityModifier = -1;
							}
							int iMult = (iPriorityModifier * 10 * iMultModifier);
							iFlavorValue *= iMult;
						}
						if (GC.getTraitInfo(eTrait).isCoastalAIInfluence())
						{
							iFlavorValue *= getCoastalAIInfluence();
							iFlavorValue /= 100;
						}
						int iRandomModifier = GC.getGame().getSorenRandNum(200, "Trait AI Selection Modifier");
						iFlavorValue *= iRandomModifier;
						iFlavorValue /= 100;
						iValue += iFlavorValue;
					}
					if (iValue > iBestValue)
					{
						eBestTrait = eTrait;
						iBestValue = iValue;
					}
				}
			}

			if (eBestTrait != NO_TRAIT)
			{
				if (GC.getTraitInfo(eBestTrait).isNegativeTrait())
				{
					setHasTrait(eBestTrait, true);
				}
				else
				{
					setHasTrait(eBestTrait, false);
				}
			}
		}
	}
	return;
}

void CvPlayer::clearLeaderTraits()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		TraitTypes eTrait = ((TraitTypes)iI);
		if (hasTrait(eTrait) && !GC.getTraitInfo(eTrait).isCivilizationTrait())
		{
			m_pabHasTrait[eTrait] = false;
			processTrait((TraitTypes)iI, -1);
		}
	}
	setLeaderHeadLevel(0);
}

int CvPlayer::getNationalEspionageDefense() const
{
	return m_iNationalEspionageDefense;
}

void CvPlayer::setNationalEspionageDefense(int iValue)
{
	m_iNationalEspionageDefense = iValue;
}

void CvPlayer::changeNationalEspionageDefense(int iChange)
{
	setNationalEspionageDefense(getNationalEspionageDefense() + iChange);
}

int CvPlayer::getInquisitionCount() const
{
	return m_iInquisitionCount;
}

void CvPlayer::changeInquisitionCount(int iChange)
{
	m_iInquisitionCount += iChange;
}

void CvPlayer::changeNationalGreatPeopleUnitRate(const UnitTypes eUnit, const int iChange)
{
	FASSERT_BOUNDS(-1, GC.getNumUnitInfos(), eUnit);

	if (iChange == 0)
	{
		FErrorMsg("This is not a change!");
		return;
	}
	m_iNationalGreatPeopleRate += iChange;

	if (eUnit == NO_UNIT)
	{
		return;
	}
	std::map<short, int>::const_iterator itr = m_greatPeopleRateforUnit.find((short)eUnit);

	if (itr == m_greatPeopleRateforUnit.end())
	{
		m_greatPeopleRateforUnit.insert(std::make_pair((short)eUnit, iChange));
	}
	else if (itr->second == -iChange)
	{
		m_greatPeopleRateforUnit.erase(itr->first);
	}
	else // change unit count
	{
		m_greatPeopleRateforUnit[itr->first] += iChange;
	}
}

int CvPlayer::getNationalGreatPeopleUnitRate(const UnitTypes eUnit) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);
	std::map<short, int>::const_iterator itr = m_greatPeopleRateforUnit.find((short)eUnit);
	return itr != m_greatPeopleRateforUnit.end() ? itr->second : 0;
}

int CvPlayer::getNationalGreatPeopleRate() const
{
	return std::max(0, m_iNationalGreatPeopleRate);
}


int CvPlayer::getMaxTradeRoutesAdjustment() const
{
	return m_iMaxTradeRoutesAdjustment;
}

void CvPlayer::changeMaxTradeRoutesAdjustment(int iChange)
{
	m_iMaxTradeRoutesAdjustment += iChange;
}

int CvPlayer::getNationalHurryAngerModifier() const
{
	return m_iNationalHurryAngerModifier;
}

void CvPlayer::setNationalHurryAngerModifier(int iValue)
{
	m_iNationalHurryAngerModifier = iValue;
}

void CvPlayer::changeNationalHurryAngerModifier(int iChange)
{
	setNationalHurryAngerModifier(getNationalHurryAngerModifier() + iChange);
}

int CvPlayer::getNationalEnemyWarWearinessModifier() const
{
	return m_iNationalEnemyWarWearinessModifier;
}

void CvPlayer::setNationalEnemyWarWearinessModifier(int iValue)
{
	m_iNationalEnemyWarWearinessModifier = iValue;
}

void CvPlayer::changeNationalEnemyWarWearinessModifier(int iChange)
{
	setNationalEnemyWarWearinessModifier(getNationalEnemyWarWearinessModifier() + iChange);
}

int CvPlayer::getNationalBombardDefenseModifier() const
{
	return m_iNationalBombardDefenseModifier;
}

void CvPlayer::setNationalBombardDefenseModifier(int iValue)
{
	m_iNationalBombardDefenseModifier = iValue;
}

void CvPlayer::changeNationalBombardDefenseModifier(int iChange)
{
	setNationalBombardDefenseModifier(getNationalBombardDefenseModifier() + iChange);
}

int CvPlayer::getNationalDomainFreeExperience(DomainTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_DOMAIN_TYPES, eIndex);
	return m_paiNationalDomainFreeExperience[eIndex];
}

void CvPlayer::setNationalDomainFreeExperience(DomainTypes eIndex, int iValue)
{
	FASSERT_BOUNDS(0, NUM_DOMAIN_TYPES, eIndex);

	m_paiNationalDomainFreeExperience[eIndex] = iValue;
}

void CvPlayer::changeNationalDomainFreeExperience(DomainTypes eIndex, int iChange)
{
	setNationalDomainFreeExperience(eIndex, (getNationalDomainFreeExperience(eIndex) + iChange));
}

int CvPlayer::getNationalDomainProductionModifier(DomainTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_DOMAIN_TYPES, eIndex);
	return m_paiNationalDomainProductionModifier[eIndex];
}

void CvPlayer::setNationalDomainProductionModifier(DomainTypes eIndex, int iValue)
{
	FASSERT_BOUNDS(0, NUM_DOMAIN_TYPES, eIndex);

	m_paiNationalDomainProductionModifier[eIndex] = iValue;
}

void CvPlayer::changeNationalDomainProductionModifier(DomainTypes eIndex, int iChange)
{
	setNationalDomainProductionModifier(eIndex, (getNationalDomainProductionModifier(eIndex) + iChange));
}

int CvPlayer::getNationalTechResearchModifier(TechTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eIndex);
	return m_paiNationalTechResearchModifier[eIndex];
}

void CvPlayer::setNationalTechResearchModifier(TechTypes eIndex, int iValue)
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eIndex);

	m_paiNationalTechResearchModifier[eIndex] = iValue;
}

void CvPlayer::changeNationalTechResearchModifier(TechTypes eIndex, int iChange)
{
	setNationalTechResearchModifier(eIndex, (getNationalTechResearchModifier(eIndex) + iChange));
}

int CvPlayer::getCoastalAIInfluence() const
{
	PROFILE_EXTRA_FUNC();
	int iInfluence = 0;
	int iNumCities = 0;
	int iModifier = 0;

	foreach_(const CvCity* pLoopCity, cities())
	{
		if (pLoopCity->isCoastal(1))
		{
			iInfluence++;
		}
		iNumCities++;
	}
	iInfluence *= 100;
	if (iNumCities != 0)
	{
		iModifier = iInfluence/iNumCities;
	}
	iModifier += 50;
	return iModifier;
}

	//Team Project (6)
int CvPlayer::getEraAdvanceFreeSpecialistCount(SpecialistTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex);
	return m_paiEraAdvanceFreeSpecialistCount[eIndex];
}

void CvPlayer::setEraAdvanceFreeSpecialistCount(SpecialistTypes eIndex, int iValue)
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex);

	m_paiEraAdvanceFreeSpecialistCount[eIndex] = iValue;
}

void CvPlayer::changeEraAdvanceFreeSpecialistCount(SpecialistTypes eIndex, int iChange)
{
	setEraAdvanceFreeSpecialistCount(eIndex, (getEraAdvanceFreeSpecialistCount(eIndex) + iChange));
}

void CvPlayer::changeGoldenAgeOnBirthOfGreatPersonCount(const UnitTypes eUnit, const char iChange)
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);

	if (iChange == 0)
	{
		FErrorMsg("This is not a change!");
		return;
	}
	std::map<short, char>::const_iterator itr = m_goldenAgeOnBirthOfGreatPersonCount.find((short)eUnit);

	if (itr == m_goldenAgeOnBirthOfGreatPersonCount.end())
	{
		m_goldenAgeOnBirthOfGreatPersonCount.insert(std::make_pair((short)eUnit, iChange));
	}
	else if (itr->second == -iChange)
	{
		m_goldenAgeOnBirthOfGreatPersonCount.erase(itr->first);
	}
	else // change unit count
	{
		m_goldenAgeOnBirthOfGreatPersonCount[itr->first] += iChange;
	}
}

int CvPlayer::getGoldenAgeOnBirthOfGreatPersonCount(const UnitTypes eUnit) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);
	std::map<short, char>::const_iterator itr = m_goldenAgeOnBirthOfGreatPersonCount.find((short)eUnit);
	return itr != m_goldenAgeOnBirthOfGreatPersonCount.end() ? itr->second : 0;
}

int CvPlayer::getNationalCityStartCulture() const
{
	return m_iNationalCityStartCulture;
}

void CvPlayer::setNationalCityStartCulture(int iValue)
{
	m_iNationalCityStartCulture = iValue;
}

void CvPlayer::changeNationalCityStartCulture(int iChange)
{
	setNationalCityStartCulture(getNationalCityStartCulture() + iChange);
}

int CvPlayer::getNationalAirUnitCapacity() const
{
	return m_iNationalAirUnitCapacity;
}

void CvPlayer::setNationalAirUnitCapacity(int iValue)
{
	m_iNationalAirUnitCapacity = iValue;
}

void CvPlayer::changeNationalAirUnitCapacity(int iChange)
{
	setNationalAirUnitCapacity(getNationalAirUnitCapacity() + iChange);
}

int CvPlayer::getCapitalXPModifier() const
{
	return m_iCapitalXPModifier;
}

void CvPlayer::setCapitalXPModifier(int iValue)
{
	m_iCapitalXPModifier = iValue;
}

void CvPlayer::changeCapitalXPModifier(int iChange)
{
	setCapitalXPModifier(getCapitalXPModifier() + iChange);
}

int CvPlayer::getStateReligionHolyCityXPModifier() const
{
	return m_iStateReligionHolyCityXPModifier;
}

void CvPlayer::setStateReligionHolyCityXPModifier(int iValue)
{
	m_iStateReligionHolyCityXPModifier = iValue;
}

void CvPlayer::changeStateReligionHolyCityXPModifier(int iChange)
{
	setStateReligionHolyCityXPModifier(getStateReligionHolyCityXPModifier() + iChange);
}

int CvPlayer::getNonStateReligionHolyCityXPModifier() const
{
	return m_iNonStateReligionHolyCityXPModifier;
}

void CvPlayer::setNonStateReligionHolyCityXPModifier(int iValue)
{
	m_iNonStateReligionHolyCityXPModifier = iValue;
}

void CvPlayer::changeNonStateReligionHolyCityXPModifier(int iChange)
{
	setNonStateReligionHolyCityXPModifier(getNonStateReligionHolyCityXPModifier() + iChange);
}

int CvPlayer::getNationalCityStartBonusPopulation() const
{
	return m_iNationalCityStartBonusPopulation;
}

void CvPlayer::setNationalCityStartBonusPopulation(int iValue)
{
	m_iNationalCityStartBonusPopulation = iValue;
}

void CvPlayer::changeNationalCityStartBonusPopulation(int iChange)
{
	setNationalCityStartBonusPopulation(getNationalCityStartBonusPopulation() + iChange);
}

int CvPlayer::getNationalMissileRangeChange() const
{
	return m_iNationalMissileRangeChange;
}

void CvPlayer::setNationalMissileRangeChange(int iValue)
{
	m_iNationalMissileRangeChange = iValue;
}

void CvPlayer::changeNationalMissileRangeChange(int iChange)
{
	setNationalMissileRangeChange(getNationalMissileRangeChange() + iChange);
}

int CvPlayer::getNationalFlightOperationRangeChange() const
{
	return m_iNationalFlightOperationRangeChange;
}

void CvPlayer::setNationalFlightOperationRangeChange(int iValue)
{
	m_iNationalFlightOperationRangeChange = iValue;
}

void CvPlayer::changeNationalFlightOperationRangeChange(int iChange)
{
	setNationalFlightOperationRangeChange(getNationalFlightOperationRangeChange() + iChange);
}

int CvPlayer::getNationalNavalCargoSpaceChange() const
{
	return m_iNationalNavalCargoSpaceChange;
}

void CvPlayer::setNationalNavalCargoSpaceChange(int iValue)
{
	m_iNationalNavalCargoSpaceChange = iValue;
}

void CvPlayer::changeNationalNavalCargoSpaceChange(int iChange)
{
	setNationalNavalCargoSpaceChange(getNationalNavalCargoSpaceChange() + iChange);
}

int CvPlayer::getNationalMissileCargoSpaceChange() const
{
	return m_iNationalMissileCargoSpaceChange;
}

void CvPlayer::setNationalMissileCargoSpaceChange(int iValue)
{
	m_iNationalMissileCargoSpaceChange = iValue;
}

void CvPlayer::changeNationalMissileCargoSpaceChange(int iChange)
{
	setNationalMissileCargoSpaceChange(getNationalMissileCargoSpaceChange() + iChange);
}

bool CvPlayer::hasCitiesStartwithStateReligion() const
{
	return (m_iCitiesStartwithStateReligionCount > 0);
}

void CvPlayer::setCitiesStartwithStateReligionCount(int iValue)
{
	m_iCitiesStartwithStateReligionCount = iValue;
}

void CvPlayer::changeCitiesStartwithStateReligionCount(int iChange)
{
	m_iCitiesStartwithStateReligionCount += iChange;
}

bool CvPlayer::hasDraftsOnCityCapture() const
{
	return (m_iDraftsOnCityCaptureCount > 0);
}

void CvPlayer::setDraftsOnCityCaptureCount(int iValue)
{
	m_iDraftsOnCityCaptureCount = iValue;
}

void CvPlayer::changeDraftsOnCityCaptureCount(int iChange)
{
	m_iDraftsOnCityCaptureCount += iChange;
}

bool CvPlayer::hasFreeSpecialistperWorldWonder() const
{
	return (m_iFreeSpecialistperWorldWonderCount > 0);
}

void CvPlayer::setFreeSpecialistperWorldWonderCount(int iValue)
{
	m_iFreeSpecialistperWorldWonderCount = iValue;
}

void CvPlayer::changeFreeSpecialistperWorldWonderCount(int iChange)
{
	m_iFreeSpecialistperWorldWonderCount += iChange;
}

bool CvPlayer::hasFreeSpecialistperNationalWonder() const
{
	return (m_iFreeSpecialistperNationalWonderCount > 0);
}

void CvPlayer::setFreeSpecialistperNationalWonderCount(int iValue)
{
	m_iFreeSpecialistperNationalWonderCount = iValue;
}

void CvPlayer::changeFreeSpecialistperNationalWonderCount(int iChange)
{
	m_iFreeSpecialistperNationalWonderCount += iChange;
}

bool CvPlayer::hasFreeSpecialistperTeamProject() const
{
	return (m_iFreeSpecialistperTeamProjectCount > 0);
}

void CvPlayer::setFreeSpecialistperTeamProjectCount(int iValue)
{
	m_iFreeSpecialistperTeamProjectCount = iValue;
}

void CvPlayer::changeFreeSpecialistperTeamProjectCount(int iChange)
{
	m_iFreeSpecialistperTeamProjectCount += iChange;
}

bool CvPlayer::hasExtraGoody() const
{
	return (m_iExtraGoodyCount > 0);
}

void CvPlayer::setExtraGoodyCount(int iValue)
{
	m_iExtraGoodyCount = iValue;
}

void CvPlayer::changeExtraGoodyCount(int iChange)
{
	m_iExtraGoodyCount += iChange;
}

	//Team Project (5)
bool CvPlayer::hasBannedNonStateReligions() const
{
	return (m_iAllReligionsActiveCount < 0);
}

bool CvPlayer::hasAllReligionsActive() const
{
	return (m_iAllReligionsActiveCount > 0);
}

int CvPlayer::getAllReligionsActiveCount() const
{
	return m_iAllReligionsActiveCount;
}

void CvPlayer::setAllReligionsActiveCount(int iValue)
{
	m_iAllReligionsActiveCount = iValue;
}

void CvPlayer::changeAllReligionsActiveCount(int iChange)
{
	m_iAllReligionsActiveCount += iChange;
}

void CvPlayer::startDeferredPlotGroupBonusCalculation()
{
	PROFILE_FUNC();

	algo::for_each(cities(), CvCity::fn::startDeferredBonusProcessing());
}

void CvPlayer::endDeferredPlotGroupBonusCalculation()
{
	PROFILE_FUNC();

	algo::for_each(cities(), CvCity::fn::endDeferredBonusProcessing());
}

bool CvPlayer::hasFixedBorders() const
{
	if (!isNPC() && GC.getGame().isOption(GAMEOPTION_CULTURE_FIXED_BORDERS) )
	{
		return (m_iFixedBordersCount > 0);
	}

	return false;
}

void CvPlayer::setFixedBordersCount(int iValue)
{
	m_iFixedBordersCount = iValue;
}

void CvPlayer::changeFixedBordersCount(int iChange)
{
	m_iFixedBordersCount += iChange;
}

//Team Project (3)
int CvPlayer::getExtraNationalCaptureProbabilityModifier() const
{
	return m_iExtraNationalCaptureProbabilityModifier;
}

void CvPlayer::setExtraNationalCaptureProbabilityModifier(int iValue)
{
	m_iExtraNationalCaptureProbabilityModifier = iValue;
}

void CvPlayer::changeExtraNationalCaptureProbabilityModifier(int iChange)
{
	m_iExtraNationalCaptureProbabilityModifier += iChange;
}

int CvPlayer::getExtraNationalCaptureResistanceModifier() const
{
	return m_iExtraNationalCaptureResistanceModifier;
}

void CvPlayer::setExtraNationalCaptureResistanceModifier(int iValue)
{
	m_iExtraNationalCaptureResistanceModifier = iValue;
}

void CvPlayer::changeExtraNationalCaptureResistanceModifier(int iChange)
{
	m_iExtraNationalCaptureResistanceModifier += iChange;
}

//Team Project (6)
int CvPlayer::getExtraStateReligionSpreadModifier() const
{
	return m_iExtraStateReligionSpreadModifier;
}

void CvPlayer::setExtraStateReligionSpreadModifier(int iValue)
{
	m_iExtraStateReligionSpreadModifier = iValue;
}

void CvPlayer::changeExtraStateReligionSpreadModifier(int iChange)
{
	m_iExtraStateReligionSpreadModifier += iChange;
}

int CvPlayer::getExtraNonStateReligionSpreadModifier() const
{
	return m_iExtraNonStateReligionSpreadModifier;
}

void CvPlayer::setExtraNonStateReligionSpreadModifier(int iValue)
{
	m_iExtraNonStateReligionSpreadModifier = iValue;
}

void CvPlayer::changeExtraNonStateReligionSpreadModifier(int iChange)
{
	m_iExtraNonStateReligionSpreadModifier += iChange;
}

void CvPlayer::checkReligiousDisablingAllBuildings()
{
	PROFILE_FUNC();

	for_each(cities(), CvCity::fn::checkReligiousDisablingAllBuildings());
}

int CvPlayer::getBaseMergeSelectionUnit() const
{
	return m_iBaseMergeSelection;
}

void CvPlayer::setBaseMergeSelectionUnit(int iNewValue)
{
	m_iBaseMergeSelection = iNewValue;
}

int CvPlayer::getFirstMergeSelectionUnit() const
{
	return m_iFirstMergeSelection;
}

void CvPlayer::setFirstMergeSelectionUnit(int iNewValue)
{
	m_iFirstMergeSelection = iNewValue;
}

int CvPlayer::getSecondMergeSelectionUnit() const
{
	return m_iSecondMergeSelection;
}

void CvPlayer::setSecondMergeSelectionUnit(int iNewValue)
{
	m_iSecondMergeSelection = iNewValue;
}

int CvPlayer::getSplittingUnit() const
{
	return m_iSplittingUnit;
}

void CvPlayer::setSplittingUnit(int iNewValue)
{
	m_iSplittingUnit = iNewValue;
}

int CvPlayer::getAmbushingUnit() const
{
	return m_iAmbushingUnit;
}

int CvPlayer::getAmbushingTargetUnit() const
{
	return m_iAmbushingTargetUnit;
}

bool CvPlayer::isAssassinate() const
{
	return m_bAssassinate;
}

void CvPlayer::setAmbushingUnit(int iNewValue, bool bAssassinate)
{
	m_iAmbushingUnit = iNewValue;
	m_bAssassinate = bAssassinate;
}

void CvPlayer::setAmbushingTargetUnit(int iNewValue, bool bAssassinate)
{
	m_iAmbushingTargetUnit = iNewValue;
	//m_bAssassinate = bAssassinate;
}

void CvPlayer::setGreatGeneralPointsForType(const UnitTypes eUnit, const int iValue)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);
	FAssertMsg(iValue > -1, "Expected positive value! Code copes with it though");

	std::map<short, uint32_t>::const_iterator itr = m_greatGeneralPointsType.find((short)eUnit);

	if (itr == m_greatGeneralPointsType.end())
	{
		if (iValue > 0)
		{
			m_greatGeneralPointsType.insert(std::make_pair((short)eUnit, iValue));
		}
		else
		{
			FErrorMsg("Meaningless to store this data");
			return;
		}
	}
	else if (iValue < 1)
	{
		m_greatGeneralPointsType.erase(itr->first);
	}
	else // change GG count
	{
		m_greatGeneralPointsType[itr->first] = iValue;
	}

	// Evaluate what GG is now first in line
	m_eGreatGeneralTypetoAssign = getGreatGeneralTypetoAssign();
	uint32_t iBest = getGreatGeneralPointsForType(m_eGreatGeneralTypetoAssign);

	for (std::map<short, uint32_t>::const_iterator itr = m_greatGeneralPointsType.begin(); itr != m_greatGeneralPointsType.end(); ++itr)
	{
		if (itr->second > iBest)
		{
			iBest = itr->second;
			m_eGreatGeneralTypetoAssign = (UnitTypes)itr->first;
		}
	}
}

void CvPlayer::changeGreatGeneralPointsForType(const UnitTypes eUnit, const int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);
	FAssertMsg(iChange != 0, "This is not a change!");

	setGreatGeneralPointsForType(eUnit, getGreatGeneralPointsForType(eUnit) + iChange);
}

int CvPlayer::getGreatGeneralPointsForType(const UnitTypes eUnit) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);
	std::map<short, uint32_t>::const_iterator itr = m_greatGeneralPointsType.find((short)eUnit);
	return itr != m_greatGeneralPointsType.end() ? itr->second : 0;
}

UnitTypes CvPlayer::getGreatGeneralTypetoAssign() const
{
	if (m_eGreatGeneralTypetoAssign == NO_UNIT)
	{
		return GC.getUNIT_GREAT_GENERAL();
	}
	return m_eGreatGeneralTypetoAssign;
}


void CvPlayer::setSMValues()
{
	algo::for_each(units(), CvUnit::fn::setSMValues());
}

void CvPlayer::upgradePlotPopup(ImprovementTypes eImprovement, int iX, int iY)
{
	CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_IMPROVEMENT_UPGRADE_OPTIONS);
	pInfo->setData1((int)eImprovement);
	pInfo->setData2(iX);
	pInfo->setData3(iY);
	gDLL->getInterfaceIFace()->addPopup(pInfo, getID(), false, false);
}

void CvPlayer::upgradePlot(int iX, int iY, ImprovementTypes eImprovement, bool bConfirm)
{
	CvPlot* pPlot = GC.getMap().plot(iX, iY);

	if (pPlot && bConfirm)
	{
		pPlot->setImprovementType(eImprovement);
	}
	if (GC.getGame().getActivePlayer() == getID())
	{
		setFocusPlots(-1, -1);
		gDLL->getInterfaceIFace()->releaseLockedCamera();
	}
}

void CvPlayer::setFocusPlots(int iX, int iY)
{
	m_iFocusPlotX = iX;
	m_iFocusPlotY = iY;
}

int CvPlayer::getFocusPlotX() const
{
	return m_iFocusPlotX;
}

int CvPlayer::getFocusPlotY() const
{
	return m_iFocusPlotY;
}

// K-Mod
int CvPlayer::getTypicalUnitValue(UnitAITypes eUnitAI) const
{
	PROFILE_EXTRA_FUNC();
	int iHighestValue = 0;

	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		if (GC.getUnitInfo((UnitTypes)iI).getUnitAIType(eUnitAI) && canTrain((UnitTypes)iI))
		{
			const int iValue = GC.getGame().AI_combatValue((UnitTypes)iI);
			if (iValue > iHighestValue)
			{
				iHighestValue = iValue;
			}
		}
	}
	return iHighestValue;
}

int CvPlayer::getUpgradeRoundCount() const
{
	return m_iUpgradeRoundCount;
}

void CvPlayer::changeUpgradeRoundCount(int iChange)
{
	m_iUpgradeRoundCount += iChange;
}

void CvPlayer::resetUpgradeRoundCount()
{
	m_iUpgradeRoundCount = 0;
}

int CvPlayer::getSelectionRegroup() const
{
	return m_iSelectionRegroup;
}

void CvPlayer::setSelectionRegroup(int iGroupID)
{
	m_iSelectionRegroup = iGroupID;
}

bool CvPlayer::hasFreedomFighter() const
{
	return (m_iFreedomFighterCount > 0);
}

void CvPlayer::changeFreedomFighterCount(int iChange)
{
	m_iFreedomFighterCount += iChange;
}

int CvPlayer::getExtraFreedomFighters() const
{
	return (m_iExtraFreedomFighters > 0);
}

void CvPlayer::changeExtraFreedomFighters(int iChange)
{
	m_iExtraFreedomFighters += iChange;
}


// Toffer - returns true if player own any units that can found a city
bool CvPlayer::haveSettlerUnit() const
{
	PROFILE_EXTRA_FUNC();
	foreach_(CvUnit* pLoopUnit, units())
	{
		if (pLoopUnit->isFound())
		{
			return true;
		}
	}
	return false;
}


// Toffer - A very common check
bool CvPlayer::isAliveAndTeam(const TeamTypes eTeam, const bool bSameTeam, const TeamTypes eTeamAlt) const
{
	return
	(
		isAlive()
		&&
		(
			eTeam == NO_TEAM
			||
			(
				bSameTeam // Can only be on one team, so either or is implied.
				?
				(
					eTeamAlt == NO_TEAM
					?
					eTeam == getTeam()
					:
					eTeam == getTeam() || eTeamAlt == getTeam()
				)
				: // Different team, both must be different.
				(
					eTeamAlt == NO_TEAM
					?
					eTeam != getTeam()
					:
					eTeam != getTeam() && eTeamAlt != getTeam()
				)
			)
		)
	);
	// Could probably get away with the second evaluation even when eTeamAlt == NO_TEAM
	// Best to be on the safe side though
}

void CvPlayer::setIdleCity(const int iCityID, const bool bNewValue)
{
	FAssert(bNewValue || !m_idleCities.empty());
	std::vector<int>::iterator itr = find(m_idleCities.begin(), m_idleCities.end(), iCityID);

	if (bNewValue)
	{
		if (itr == m_idleCities.end())
		{
			m_idleCities.push_back(iCityID);
			FAssert(!getCity(iCityID)->isProduction());
		}
		else FErrorMsg("Tried to add a duplicate vector element!");
	}
	else if (itr != m_idleCities.end())
	{
		m_idleCities.erase(itr);

		if (m_idleCities.empty())
		{
			setForcedCityCycle(false);
		}
	}
	else FErrorMsg("Vector element to remove was missing!");
}

CvCity* CvPlayer::getIdleCity() const
{
	FAssert(!m_idleCities.empty());
	return getCity(m_idleCities[0]);
}

bool CvPlayer::isIdleCity(const int iCityID) const
{
	return find(m_idleCities.begin(), m_idleCities.end(), iCityID) != m_idleCities.end();
}

bool CvPlayer::hasIdleCity() const
{
	return !m_idleCities.empty();
}

void CvPlayer::resetIdleCities()
{
	PROFILE_EXTRA_FUNC();
	m_idleCities.clear();

	foreach_(CvCity* city, cities())
	{
		if (!city->isProduction())
		{
			setIdleCity(city->getID(), true);
		}
	}
}

void CvPlayer::listCommander(bool bAdd, CvUnit* unit)
{
	PROFILE_EXTRA_FUNC();
	if (bAdd)
	{
		m_commanders.push_back(unit);
		return;
	}
	const int iID = unit->getID();

	for (int i = 0; i < static_cast<int>(m_commanders.size()); i++)
	{
		if (m_commanders[i]->getID() == iID)
		{
			m_commanders.erase(m_commanders.begin() + i);

			foreach_(CvUnit* unitX, units())
			{
				if (unitX->getLastCommander() && unitX->getLastCommander()->getID() == iID)
				{
					unitX->nullLastCommander();
				}
			}
			break;
		}
	}
}


void CvPlayer::listCommodore(bool bAdd, CvUnit* unit)
{
	PROFILE_EXTRA_FUNC();
	if (bAdd)
	{
		m_commodores.push_back(unit);
		return;
	}
	const int iID = unit->getID();

	for (int i = 0; i < static_cast<int>(m_commodores.size()); i++)
	{
		if (m_commodores[i]->getID() == iID)
		{
			m_commodores.erase(m_commodores.begin() + i);

			foreach_(CvUnit* unitX, units())
			{
				if (unitX->getLastCommodore() && unitX->getLastCommodore()->getID() == iID)
				{
					unitX->nullLastCommodore();
				}
			}
			break;
		}
	}
}


void CvPlayer::setCommandFieldPlot(bool bNewValue, CvPlot* aPlot)
{
	FAssert(bNewValue || !m_commandFieldPlots.empty());

	std::vector<CvPlot*>::iterator itr = find(m_commandFieldPlots.begin(), m_commandFieldPlots.end(), aPlot);

	if (bNewValue)
	{
		if (itr == m_commandFieldPlots.end())
		{
			m_commandFieldPlots.push_back(aPlot);
		}
		else FErrorMsg("Tried to add a duplicate vector element!");
	}
	else if (itr != m_commandFieldPlots.end())
	{
		m_commandFieldPlots.erase(itr);
	}
	else FErrorMsg("Vector element to remove was missing!");
}


void CvPlayer::setCommodoreFieldPlot(bool bNewValue, CvPlot* aPlot)
{
	FAssert(bNewValue || !m_commodoreFieldPlots.empty());

	std::vector<CvPlot*>::iterator itr = find(m_commodoreFieldPlots.begin(), m_commodoreFieldPlots.end(), aPlot);

	if (bNewValue)
	{
		if (itr == m_commodoreFieldPlots.end())
		{
			m_commodoreFieldPlots.push_back(aPlot);
		}
		else FErrorMsg("Tried to add a duplicate vector element!");
	}
	else if (itr != m_commodoreFieldPlots.end())
	{
		m_commodoreFieldPlots.erase(itr);
	}
	else FErrorMsg("Vector element to remove was missing!");
}


void CvPlayer::processTech(const TechTypes eTech, const int iChange)
{
	PROFILE_EXTRA_FUNC();
	const CvTechInfo& tech = GC.getTechInfo(eTech);

	changeFeatureProductionModifier(tech.getFeatureProductionModifier() * iChange);
	changeWorkerSpeedModifier(tech.getWorkerSpeedModifier() * iChange);
	changeTradeRoutes(tech.getTradeRoutes() * iChange);
	changeExtraHealth(tech.getHealth() * iChange);
	changeExtraHappiness(tech.getHappiness() * iChange);
	changeDistanceMaintenanceModifier(tech.getDistanceMaintenanceModifier() * iChange);
	changeNumCitiesMaintenanceModifier(tech.getNumCitiesMaintenanceModifier() * iChange);
	changeMaintenanceModifier(tech.getMaintenanceModifier() * iChange);
	changeCoastalDistanceMaintenanceModifier(tech.getCoastalDistanceMaintenanceModifier() * iChange);
	changeAssets(tech.getAssetValue() * iChange);
	changeTechPower(tech.getPowerValue() * iChange);
	changeTechScore(getScoreValueOfTech(eTech) * iChange);
	changeTechInflation(tech.getInflationModifier() * iChange);

	if (tech.isLanguage())
	{
		m_bHasLanguage = true;
	}

	for (int i = 0; i < NUM_COMMERCE_TYPES; i++)
	{
		changeCommerceRateModifier(static_cast<CommerceTypes>(i), tech.getCommerceModifier(i) * iChange);
	}
	algo::for_each(cities(), CvCity::fn::processTech(eTech, iChange));
}


bool CvPlayer::hasHeritage(const HeritageTypes eType) const
{
	return algo::any_of_equal(m_myHeritage, eType);
}

bool CvPlayer::canAddHeritage(const HeritageTypes eType, const bool bTestVisible) const
{
	PROFILE_EXTRA_FUNC();

	if (hasHeritage(eType))
	{
		return false;
	}
	const CvHeritageInfo& heritage = GC.getHeritageInfo(eType);

	if (heritage.needLanguage() && !m_bHasLanguage)
	{
		return false;
	}

	if (!GET_TEAM(getTeam()).isHasTech((TechTypes)heritage.getPrereqTech()))
	{
		return false;
	}

	if (!bTestVisible)
	{
		bool bValid = false;
		bool bRequires = false;
		foreach_(const HeritageTypes eTypeX, heritage.getPrereqOrHeritage())
		{
			bRequires = true;

			if (hasHeritage(eTypeX))
			{
				bValid = true;
				break;
			}
		}
		if (bRequires && !bValid)
		{
			return false;
		}
	}

	return true;
}

void CvPlayer::setHeritage(const HeritageTypes eType, const bool bNewValue)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumHeritageInfos(), eType);

	std::vector<HeritageTypes>::iterator itr = find(m_myHeritage.begin(), m_myHeritage.end(), eType);

	if (bNewValue)
	{
		if (itr == m_myHeritage.end())
		{
			m_myHeritage.push_back(eType);
			processHeritage(eType, 1);
		}
		else FErrorMsg("Tried to add a duplicate vector element!");
	}
	else if (itr != m_myHeritage.end())
	{
		m_myHeritage.erase(itr);
		processHeritage(eType, -1);
	}
	else FErrorMsg("Vector element to remove was missing!");

	clearCanConstructCache(NO_BUILDING, true);
}

void CvPlayer::processHeritage(const HeritageTypes eType, const int iChange)
{
	PROFILE_EXTRA_FUNC();

	const CvHeritageInfo& heritage = GC.getHeritageInfo(eType);
	const EraTypes eEra = getCurrentEra();

	foreach_(const EraCommerceArray& pair, heritage.getEraCommerceChanges100())
	{
		if (eEra >= pair.first)
		{
			for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
			{
				changeExtraCommerce100((CommerceTypes)iI, iChange * pair.second[(CommerceTypes)iI]);
			}
		}
	}
}

int CvPlayer::getHeritageCommerceEraChange(const CommerceTypes eType, const EraTypes eEra) const
{
	PROFILE_EXTRA_FUNC();
	int iCommerce100 = 0;
	foreach_(const HeritageTypes eTypeX, getHeritage())
	{
		foreach_(const EraCommerceArray& pair, GC.getHeritageInfo(eTypeX).getEraCommerceChanges100())
		{
			if (eEra == pair.first)
			{
				iCommerce100 += pair.second[eType];
			}
		}
	}
	return iCommerce100;
}

#ifdef CVARMY_BREAKSAVE

CvArmy* CvPlayer::getArmy(int iArmyID) const
{
	if (iArmyID < 0)
		return NULL;

	// FFreeListTrashArray fournit getAt(id) qui retourne NULL si l'objet n'existe pas
	return m_armies.getAt(iArmyID);
}

void CvPlayer::deleteArmy(int iArmyID)
{
	CvArmy* pArmy = m_armies.getAt(iArmyID);
	if (pArmy != NULL)
	{
		pArmy->disband();       // nettoyer les groupes
		m_armies.removeAt(iArmyID); // supprime l’objet de la liste
	}
}

#endif