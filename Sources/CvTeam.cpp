// team.cpp


#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvArea.h"
#include "CvBuildingInfo.h"
#include "CvBonusInfo.h"
#include "CvCity.h"
#include "CvDeal.h"
#include "CvDiploParameters.h"
#include "CvEventReporter.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvImprovementInfo.h"
#include "CvInfos.h"
#include "CvInitCore.h"
#include "CvMap.h"
#include "CvPlayerAI.h"
#include "CvPlot.h"
#include "CvPopupInfo.h"
#include "CvPython.h"
#include "CvSelectionGroup.h"
#include "CvTeamAI.h"
#include "CvUnit.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLUtilityIFaceBase.h"

// Public Functions...
#pragma warning( disable : 4355 )
CvTeam::CvTeam() : m_GameObject(this),
m_Properties(this)
{
	m_aiStolenVisibilityTimer = new int[MAX_TEAMS];
	m_aiWarWearinessTimes100 = new int[MAX_TEAMS];
	m_aiTechShareCount = new int[MAX_TEAMS];
	m_aiCommerceFlexibleCount = new int[NUM_COMMERCE_TYPES];
	m_aiExtraMoves = new int[NUM_DOMAIN_TYPES];

	m_aiEspionagePointsAgainstTeam = new int[MAX_TEAMS];
	m_aiCounterespionageTurnsLeftAgainstTeam = new int[MAX_TEAMS];
	m_aiCounterespionageModAgainstTeam = new int[MAX_TEAMS];

	m_abAtWar = new bool[MAX_TEAMS];
	m_abHasMet = new bool[MAX_TEAMS];
	m_abPermanentWarPeace = new bool[MAX_TEAMS];
	m_abOpenBorders = new bool[MAX_TEAMS];
	m_abDefensivePact = new bool[MAX_TEAMS];
	m_abForcePeace = new bool[MAX_TEAMS];
	m_abVassal = new bool[MAX_TEAMS];
	m_abCanLaunch = NULL;

	m_abIsRebelAgainst = new bool[MAX_TEAMS];

	m_paiRouteChange = NULL;
	m_paiProjectCount = NULL;
	m_paiProjectDefaultArtTypes = NULL;
	m_pavProjectArtTypes = NULL;
	m_paiProjectMaking = NULL;
	m_paiBuildingCount = NULL;
	m_paiObsoleteBuildingCount = NULL;
	m_paiResearchProgress = NULL;
	m_paiTechCount = NULL;
	m_paiTerrainTradeCount = NULL;
	m_aiVictoryCountdown = NULL;
	m_aiForceTeamVoteEligibilityCount = NULL;

	m_pabHasTech = NULL;

	m_ppaaiImprovementYieldChange = NULL;

	m_ppiBuildingSpecialistChange = NULL;
	m_ppiBuildingCommerceModifier = NULL;
	m_abEmbassy = new bool[MAX_TEAMS];
	m_abLimitedBorders = new bool[MAX_TEAMS];
	m_abFreeTrade = new bool[MAX_TEAMS];
	m_paiFreeSpecialistCount = NULL;

	reset((TeamTypes)0, true);
}


CvTeam::~CvTeam()
{
	uninit();

	SAFE_DELETE_ARRAY(m_aiStolenVisibilityTimer);
	SAFE_DELETE_ARRAY(m_aiWarWearinessTimes100);
	SAFE_DELETE_ARRAY(m_aiTechShareCount);
	SAFE_DELETE_ARRAY(m_aiCommerceFlexibleCount);
	SAFE_DELETE_ARRAY(m_aiExtraMoves);
	SAFE_DELETE_ARRAY(m_aiEspionagePointsAgainstTeam);
	SAFE_DELETE_ARRAY(m_aiCounterespionageTurnsLeftAgainstTeam);
	SAFE_DELETE_ARRAY(m_aiCounterespionageModAgainstTeam);
	SAFE_DELETE_ARRAY(m_abAtWar);
	SAFE_DELETE_ARRAY(m_abHasMet);
	SAFE_DELETE_ARRAY(m_abPermanentWarPeace);
	SAFE_DELETE_ARRAY(m_abOpenBorders);
	SAFE_DELETE_ARRAY(m_abDefensivePact);
	SAFE_DELETE_ARRAY(m_abForcePeace);
	SAFE_DELETE_ARRAY(m_abVassal);
	SAFE_DELETE_ARRAY(m_abIsRebelAgainst);
	SAFE_DELETE_ARRAY(m_abEmbassy);
	SAFE_DELETE_ARRAY(m_abLimitedBorders);
	SAFE_DELETE_ARRAY(m_abFreeTrade);
}


void CvTeam::init(TeamTypes eID)
{
	PROFILE_EXTRA_FUNC();
	//--------------------------------
	// Init saved data
	reset(eID);
	//--------------------------------
	// Init non-saved data
	//--------------------------------
	// Init other game data
	AI_init();

	if (GC.getGame().isFinalInitialized())
	{
		cacheAdjacentResearch(); // Mid-game re-initialization
		//logging::logMsg("C2C.log", "   Checking for declarations of war against reset team %d\n", (int)getID());

		const bool bMinorCiv = isMinorCiv();
		const bool bNPC = isNPC();

		for (int iI = 0; iI < MAX_TEAMS; iI++)
		{
			if (iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (bMinorCiv || GET_TEAM((TeamTypes)iI).isMinorCiv())
				{
					GET_TEAM((TeamTypes)iI).declareWar(getID(), false, WARPLAN_LIMITED);
					GET_TEAM((TeamTypes)iI).setHasMet(getID(), false);
					setHasMet((TeamTypes)iI, false);
					//logging::logMsg("C2C.log", "   Minor team %d declared war, using war plan %d\n", iI, (int)GET_TEAM((TeamTypes)iI).AI_getWarPlan(getID()));
				}
				//TBNOTE: THIS WILL NEED TO BE ADJUSTED so that it's not so patently sweeping for all NPCs IF AN NPC IS
				//SUPPOSED TO BE FRIENDLY WITH PLAYERS or any other NPC faction!
				if (bNPC || GET_TEAM((TeamTypes)iI).isNPC())
				{
					if (bNPC && GC.getGame().isOption(GAMEOPTION_ANIMAL_PEACE_AMONG_NPCS) && GET_TEAM((TeamTypes)iI).isNPC())
					{
						continue;
					}
					GET_TEAM((TeamTypes)iI).declareWar(getID(), false, WARPLAN_LIMITED);
					//logging::logMsg("C2C.log", "   Barb team %d declared war, using war plan %d\n", iI, (int)GET_TEAM((TeamTypes)iI).AI_getWarPlan(getID()));
				}
			}
		}
	}
}


void CvTeam::uninit()
{
	m_vNoTradeTech.clear();
	m_unitCount.clear();

	SAFE_DELETE_ARRAY(m_abCanLaunch);
	SAFE_DELETE_ARRAY(m_paiRouteChange);
	SAFE_DELETE_ARRAY(m_paiProjectCount);
	SAFE_DELETE_ARRAY(m_paiProjectDefaultArtTypes);
	SAFE_DELETE_ARRAY(m_pavProjectArtTypes);
	SAFE_DELETE_ARRAY(m_paiProjectMaking);
	SAFE_DELETE_ARRAY(m_paiBuildingCount);
	SAFE_DELETE_ARRAY(m_paiObsoleteBuildingCount);
	SAFE_DELETE_ARRAY(m_paiResearchProgress);
	SAFE_DELETE_ARRAY(m_paiTechCount);
	SAFE_DELETE_ARRAY(m_paiTerrainTradeCount);
	SAFE_DELETE_ARRAY(m_aiVictoryCountdown);
	SAFE_DELETE_ARRAY(m_aiForceTeamVoteEligibilityCount);
	SAFE_DELETE_ARRAY(m_pabHasTech);
	SAFE_DELETE_ARRAY2(m_ppaaiImprovementYieldChange, GC.getNumImprovementInfos());
	SAFE_DELETE_ARRAY(m_paiFreeSpecialistCount);
	SAFE_DELETE_ARRAY2(m_ppiBuildingSpecialistChange, GC.getNumBuildingInfos());
	SAFE_DELETE_ARRAY2(m_ppiBuildingCommerceModifier, GC.getNumBuildingInfos());
}


// FUNCTION: reset()
// Initializes data members that are serialized.
void CvTeam::reset(TeamTypes eID, bool bConstructorCall)
{
	PROFILE_EXTRA_FUNC();
	int iI, iJ;

	//--------------------------------
	// Uninit class
	uninit();

	m_iNumMembers = 0;
	m_iAliveCount = 0;
	m_iEverAliveCount = 0;
	m_iNumCities = 0;
	m_iTotalPopulation = 0;
	m_iTotalLand = 0;
	m_iNukeInterception = 0;
	m_iExtraWaterSeeFromCount = 0;
	m_iMapTradingCount = 0;
	m_iTechTradingCount = 0;
	m_iGoldTradingCount = 0;
	m_iCanPassPeaksCount = 0;
	m_iMoveFastPeaksCount = 0;
	m_iCanFoundOnPeaksCount = 0;
	m_iRebaseAnywhereCount = 0;
	m_iForeignTradeModifier = 0;
	m_iTradeModifier = 0;
	m_iTradeMissionModifier = 0;
	m_iCorporationRevenueModifier = 0;
	m_iCorporationMaintenanceModifier = 0;
	m_iEmbassyTradingCount = 0;
	m_iLimitedBordersTradingCount = 0;
	m_iCanFarmDesertCount = 0;
	m_iOpenBordersTradingCount = 0;
	m_iDefensivePactTradingCount = 0;
	m_iPermanentAllianceTradingCount = 0;
	m_iVassalTradingCount = 0;
	m_iBridgeBuildingCount = 0;
	m_iIrrigationCount = 0;
	m_iIgnoreIrrigationCount = 0;
	m_iWaterWorkCount = 0;
	m_iVassalPower = 0;
	m_iMasterPower = 0;
	m_iEnemyWarWearinessModifier = 0;
	m_iRiverTradeCount = 0;
	m_iEspionagePointsEver = 0;
	m_iLastRoundOfValidImprovementCacheUpdate = 0;

	m_bMapCentering = false;
	m_bCapitulated = false;

	m_eID = eID;

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiStolenVisibilityTimer[iI] = 0;
		m_aiWarWearinessTimes100[iI] = 0;
		m_aiTechShareCount[iI] = 0;
		m_aiEspionagePointsAgainstTeam[iI] = 0;
		m_aiCounterespionageTurnsLeftAgainstTeam[iI] = 0;
		m_aiCounterespionageModAgainstTeam[iI] = 0;
		m_abHasMet[iI] = false;
		m_abAtWar[iI] = false;
		m_abPermanentWarPeace[iI] = false;
		m_abOpenBorders[iI] = false;
		m_abLimitedBorders[iI] = false;
		m_abFreeTrade[iI] = false;
		m_abEmbassy[iI] = false;
		m_abDefensivePact[iI] = false;
		m_abForcePeace[iI] = false;
		m_abVassal[iI] = false;
		m_abIsRebelAgainst[iI] = false;

		if (!bConstructorCall && getID() != NO_TEAM)
		{
			CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
			kLoopTeam.m_aiStolenVisibilityTimer[getID()] = 0;
			kLoopTeam.m_aiWarWearinessTimes100[getID()] = 0;
			kLoopTeam.m_aiTechShareCount[getID()] = 0;
			kLoopTeam.m_aiEspionagePointsAgainstTeam[getID()] = 0;
			kLoopTeam.m_aiCounterespionageTurnsLeftAgainstTeam[getID()] = 0;
			kLoopTeam.m_aiCounterespionageModAgainstTeam[getID()] = 0;
			kLoopTeam.m_abHasMet[getID()] = false;
			kLoopTeam.m_abAtWar[getID()] = false;
			kLoopTeam.m_abPermanentWarPeace[getID()] = false;
			kLoopTeam.m_abOpenBorders[getID()] = false;
			kLoopTeam.m_abEmbassy[getID()] = false;
			kLoopTeam.m_abLimitedBorders[getID()] = false;
			kLoopTeam.m_abFreeTrade[getID()] = false;
			kLoopTeam.m_abDefensivePact[getID()] = false;
			kLoopTeam.m_abForcePeace[getID()] = false;
			kLoopTeam.m_abVassal[getID()] = false;
		}
	}

	for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		m_aiCommerceFlexibleCount[iI] = 0;
	}

	for (iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		m_aiExtraMoves[iI] = 0;
	}

	if (!bConstructorCall)
	{
		FAssertMsg(m_abCanLaunch == NULL, "about to leak memory, CvTeam::m_abCanLaunch");
		m_abCanLaunch = new bool[GC.getNumVictoryInfos()];
		for (iI = 0; iI < GC.getNumVictoryInfos(); iI++)
		{
			m_abCanLaunch[iI] = false;
		}

		FAssertMsg(m_paiRouteChange == NULL, "about to leak memory, CvTeam::m_paiRouteChange");
		m_paiRouteChange = new int[GC.getNumRouteInfos()];
		for (iI = 0; iI < GC.getNumRouteInfos(); iI++)
		{
			m_paiRouteChange[iI] = 0;
		}

		FAssertMsg(m_paiProjectCount == NULL, "about to leak memory, CvPlayer::m_paiProjectCount");
		m_paiProjectCount = new int[GC.getNumProjectInfos()];
		FAssertMsg(m_paiProjectDefaultArtTypes == NULL, "about to leak memory, CvPlayer::m_paiProjectDefaultArtTypes");
		m_paiProjectDefaultArtTypes = new int[GC.getNumProjectInfos()];
		FAssertMsg(m_pavProjectArtTypes == NULL, "about to leak memory, CvPlayer::m_pavProjectArtTypes");
		m_pavProjectArtTypes = new std::vector<int>[GC.getNumProjectInfos()];
		FAssertMsg(m_paiProjectMaking == NULL, "about to leak memory, CvPlayer::m_paiProjectMaking");
		m_paiProjectMaking = new int[GC.getNumProjectInfos()];
		for (iI = 0; iI < GC.getNumProjectInfos(); iI++)
		{
			m_paiProjectCount[iI] = 0;
			m_paiProjectDefaultArtTypes[iI] = 0;
			m_paiProjectMaking[iI] = 0;
		}

		FAssertMsg(m_paiBuildingCount == NULL, "about to leak memory, CvTeam::m_paiBuildingCount");
		m_paiBuildingCount = new int[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_paiBuildingCount[iI] = 0;
		}

		FAssertMsg(m_paiObsoleteBuildingCount == NULL, "about to leak memory, CvTeam::m_paiObsoleteBuildingCount");
		m_paiObsoleteBuildingCount = new int[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_paiObsoleteBuildingCount[iI] = 0;
		}

		FAssertMsg(m_paiResearchProgress == NULL, "about to leak memory, CvPlayer::m_paiResearchProgress");
		m_paiResearchProgress = new int[GC.getNumTechInfos()];
		FAssertMsg(m_paiTechCount == NULL, "about to leak memory, CvPlayer::m_paiTechCount");
		m_paiTechCount = new int[GC.getNumTechInfos()];
		for (iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			m_paiResearchProgress[iI] = 0;
			m_paiTechCount[iI] = 0;
		}

		FAssertMsg(m_paiTerrainTradeCount == NULL, "about to leak memory, CvTeam::m_paiTerrainTradeCount");
		m_paiTerrainTradeCount = new int[GC.getNumTerrainInfos()];
		for (iI = 0; iI < GC.getNumTerrainInfos(); iI++)
		{
			m_paiTerrainTradeCount[iI] = 0;
		}

		FAssertMsg(m_aiVictoryCountdown == NULL, "about to leak memory, CvTeam::m_aiVictoryCountdown");
		m_aiVictoryCountdown = new int[GC.getNumVictoryInfos()];
		for (iI = 0; iI < GC.getNumVictoryInfos(); iI++)
		{
			m_aiVictoryCountdown[iI] = -1;
		}

		FAssertMsg(m_pabHasTech == NULL, "about to leak memory, CvTeam::m_pabHasTech");
		m_pabHasTech = new bool[GC.getNumTechInfos()];
		for (iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			m_pabHasTech[iI] = false;
		}

		FAssertMsg(m_ppaaiImprovementYieldChange == NULL, "about to leak memory, CvTeam::m_ppaaiImprovementYieldChange");
		m_ppaaiImprovementYieldChange = new int* [GC.getNumImprovementInfos()];
		for (iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			m_ppaaiImprovementYieldChange[iI] = new int[NUM_YIELD_TYPES];
			for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				m_ppaaiImprovementYieldChange[iI][iJ] = 0;
			}
		}

		FAssertMsg(m_aiForceTeamVoteEligibilityCount == NULL, "about to leak memory, CvTeam::m_aiForceTeamVoteEligibilityCount");
		m_aiForceTeamVoteEligibilityCount = new int[GC.getNumVoteSourceInfos()];
		for (iI = 0; iI < GC.getNumVoteSourceInfos(); iI++)
		{
			m_aiForceTeamVoteEligibilityCount[iI] = 0;
		}

		m_aeRevealedBonuses.clear();

		FAssertMsg(m_ppiBuildingSpecialistChange == NULL, "about to leak memory, CvTeam::m_ppiBuildingSpecialistChange");
		m_ppiBuildingSpecialistChange = new int* [GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_ppiBuildingSpecialistChange[iI] = new int[GC.getNumSpecialistInfos()];
			for (iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
			{
				m_ppiBuildingSpecialistChange[iI][iJ] = 0;
			}
		}
		FAssertMsg(m_paiFreeSpecialistCount == NULL, "about to leak memory, CvTeam::m_paiFreeSpecialistCount");
		m_paiFreeSpecialistCount = new int[GC.getNumSpecialistInfos()];
		for (iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
		{
			m_paiFreeSpecialistCount[iJ] = 0;
		}

		FAssertMsg(m_ppiBuildingCommerceModifier == NULL, "about to leak memory, CvTeam::m_ppiBuildingCommerceModifier");
		m_ppiBuildingCommerceModifier = new int* [GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_ppiBuildingCommerceModifier[iI] = new int[NUM_COMMERCE_TYPES];
			for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				m_ppiBuildingCommerceModifier[iI][iJ] = 0;
			}
		}
		m_adjacentResearch.clear();
		m_Properties.clear();

		AI_reset(false);
	}
}

//
// for clearing data stored in plots and cities for this team
//
void CvTeam::resetPlotAndCityData()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		pLoopPlot->setRevealedOwner(getID(), NO_PLAYER);
		pLoopPlot->setRevealedImprovementType(getID(), NO_IMPROVEMENT);
		pLoopPlot->setRevealedRouteType(getID(), NO_ROUTE);
		pLoopPlot->setRevealed(getID(), false, false, getID(), true);

		CvCity* pLoopCity = pLoopPlot->getPlotCity();
		if (pLoopCity != NULL)
		{
			pLoopCity->setRevealed(getID(), false);
			pLoopCity->setEspionageVisibility(getID(), false, true);
		}
	}
}

bool CvTeam::isRebel() const
{
	PROFILE_EXTRA_FUNC();
	bool bValid = false;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			if (!GET_PLAYER((PlayerTypes)iI).isRebel())
			{
				return false;
			}
			bValid = true;
		}
	}
	return bValid;
}

bool CvTeam::isSingleCityTeam() const
{
	PROFILE_EXTRA_FUNC();
	int iCities = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iCities += GET_PLAYER((PlayerTypes)iI).getNumCities();
		}
		if (iCities > 1)
		{
			return false;
		}
	}
	return true;
}

bool CvTeam::isRebelAgainst(TeamTypes eTeam) const
{
	if (eTeam >= 0 && eTeam < MAX_TEAMS)
	{
		return m_abIsRebelAgainst[(int)eTeam];
	}
	return false;
}

void CvTeam::setRebelAgainst(TeamTypes eTeam, bool bNewValue)
{
	if (eTeam >= 0 && eTeam < MAX_TEAMS)
		m_abIsRebelAgainst[(int)eTeam] = bNewValue;
}

int CvTeam::countRebelAgainst() const
{
	PROFILE_EXTRA_FUNC();
	int count = 0;
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (isRebelAgainst((TeamTypes)iI)) count++;
	}
	return count;
}

int CvTeam::getNumMilitaryUnits() const
{
	PROFILE_EXTRA_FUNC();
	int count = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			count += GET_PLAYER((PlayerTypes)iI).getNumMilitaryUnits();
		}
	}
	return count;
}

void CvTeam::addTeam(TeamTypes eTeam)
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(eTeam != NO_TEAM && eTeam != getID() && eTeam < MAX_PC_TEAMS, CvString::format("eTeam = %d", (int)eTeam).c_str());

	CLLNode<TradeData>* pNode;

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		// Alive, not on same team as us and eTeam, all three have met.
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID(), false, eTeam)
		&& isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam())
		&& GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasMet(eTeam))
		{
			AddDLLMessage(
				(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(),
				gDLL->getText(
					"TXT_KEY_MISC_PLAYER_PERMANENT_ALLIANCE",
					getName().GetCString(), GET_TEAM(eTeam).getName().GetCString()
				),
				"AS2D_THEIRALLIANCE", MESSAGE_TYPE_MINOR_EVENT, NULL, GC.getCOLOR_HIGHLIGHT_TEXT()
			);
		}
	}
	const CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_PERMANENT_ALLIANCE", getName().GetCString(), GET_TEAM(eTeam).getName().GetCString());
	GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), szBuffer, -1, -1, GC.getCOLOR_HIGHLIGHT_TEXT());

	foreach_(CvDeal & kLoopDeal, GC.getGame().deals())
	{
		if (GET_PLAYER(kLoopDeal.getFirstPlayer()).getTeam() == getID() && GET_PLAYER(kLoopDeal.getSecondPlayer()).getTeam() == eTeam
		|| GET_PLAYER(kLoopDeal.getFirstPlayer()).getTeam() == eTeam && GET_PLAYER(kLoopDeal.getSecondPlayer()).getTeam() == getID())
		{
			bool bValid = true;

			if (kLoopDeal.getFirstTrades() != NULL)
			{
				for (pNode = kLoopDeal.getFirstTrades()->head(); pNode; pNode = kLoopDeal.getFirstTrades()->next(pNode))
				{
					if (pNode->m_data.m_eItemType == TRADE_OPEN_BORDERS
					|| pNode->m_data.m_eItemType == TRADE_DEFENSIVE_PACT
					|| pNode->m_data.m_eItemType == TRADE_PEACE_TREATY
					|| pNode->m_data.m_eItemType == TRADE_VASSAL
					|| pNode->m_data.m_eItemType == TRADE_RITE_OF_PASSAGE
					|| pNode->m_data.m_eItemType == TRADE_FREE_TRADE_ZONE
					|| pNode->m_data.m_eItemType == TRADE_SURRENDER)
					{
						bValid = false;
						break;
					}
				}
			}
			if (bValid && kLoopDeal.getSecondTrades() != NULL)
			{
				for (pNode = kLoopDeal.getSecondTrades()->head(); pNode; pNode = kLoopDeal.getSecondTrades()->next(pNode))
				{
					if (pNode->m_data.m_eItemType == TRADE_OPEN_BORDERS
					|| pNode->m_data.m_eItemType == TRADE_DEFENSIVE_PACT
					|| pNode->m_data.m_eItemType == TRADE_RITE_OF_PASSAGE
					|| pNode->m_data.m_eItemType == TRADE_FREE_TRADE_ZONE
					|| pNode->m_data.m_eItemType == TRADE_PEACE_TREATY
					|| pNode->m_data.m_eItemType == TRADE_VASSAL
					|| pNode->m_data.m_eItemType == TRADE_SURRENDER)
					{
						bValid = false;
						break;
					}
				}
			}
			if (!bValid)
			{
				kLoopDeal.kill();
			}
		}
	}
	shareItems(eTeam);
	GET_TEAM(eTeam).shareItems(getID());

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (iI != eTeam && iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (GET_TEAM(eTeam).isHasMet((TeamTypes)iI))
			{
				meet((TeamTypes)iI, false);
			}
			else if (isHasMet((TeamTypes)iI))
			{
				GET_TEAM(eTeam).meet((TeamTypes)iI, false);
			}
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (iI != eTeam && iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (GET_TEAM(eTeam).isAtWar((TeamTypes)iI))
			{
				declareWar((TeamTypes)iI, false, GET_TEAM(eTeam).AI_getWarPlan((TeamTypes)iI));
			}
			else if (isAtWar((TeamTypes)iI))
			{
				GET_TEAM(eTeam).declareWar((TeamTypes)iI, false, AI_getWarPlan((TeamTypes)iI));
			}
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (iI != eTeam && iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (GET_TEAM(eTeam).isPermanentWarPeace((TeamTypes)iI))
			{
				setPermanentWarPeace((TeamTypes)iI, true);
			}
			else if (isPermanentWarPeace((TeamTypes)iI))
			{
				GET_TEAM(eTeam).setPermanentWarPeace((TeamTypes)iI, true);
			}

			if (GET_TEAM(eTeam).isOpenBorders((TeamTypes)iI))
			{
				setOpenBorders((TeamTypes)iI, true);
				GET_TEAM((TeamTypes)iI).setOpenBorders(getID(), true);
			}
			else if (isOpenBorders((TeamTypes)iI))
			{
				GET_TEAM(eTeam).setOpenBorders((TeamTypes)iI, true);
				GET_TEAM((TeamTypes)iI).setOpenBorders(eTeam, true);
			}

			if (GET_TEAM(eTeam).isLimitedBorders((TeamTypes)iI))
			{
				setLimitedBorders(((TeamTypes)iI), true);
				GET_TEAM((TeamTypes)iI).setLimitedBorders(getID(), true);
			}
			else if (isLimitedBorders((TeamTypes)iI))
			{
				GET_TEAM(eTeam).setLimitedBorders((TeamTypes)iI, true);
				GET_TEAM((TeamTypes)iI).setLimitedBorders(eTeam, true);
			}
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (iI != eTeam && iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (GET_TEAM(eTeam).isHasEmbassy((TeamTypes)iI))
			{
				setHasEmbassy(((TeamTypes)iI), true);
				GET_TEAM((TeamTypes)iI).setHasEmbassy(getID(), true);
			}
			else if (isDefensivePact((TeamTypes)iI) || isOpenBorders((TeamTypes)iI))
			{
				GET_TEAM(eTeam).setHasEmbassy((TeamTypes)iI, true);
				GET_TEAM((TeamTypes)iI).setHasEmbassy(eTeam, true);
			}
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (iI != eTeam && iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (GET_TEAM(eTeam).isDefensivePact((TeamTypes)iI))
			{
				setDefensivePact((TeamTypes)iI, true);
				GET_TEAM((TeamTypes)iI).setDefensivePact(getID(), true);
			}
			else if (isDefensivePact((TeamTypes)iI))
			{
				GET_TEAM(eTeam).setDefensivePact((TeamTypes)iI, true);
				GET_TEAM((TeamTypes)iI).setDefensivePact(eTeam, true);
			}

			if (GET_TEAM(eTeam).isForcePeace((TeamTypes)iI))
			{
				setForcePeace((TeamTypes)iI, true);
				GET_TEAM((TeamTypes)iI).setForcePeace(getID(), true);
			}
			else if (isForcePeace((TeamTypes)iI))
			{
				GET_TEAM(eTeam).setForcePeace((TeamTypes)iI, true);
				GET_TEAM((TeamTypes)iI).setForcePeace(eTeam, true);
			}
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (iI != eTeam && iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (GET_TEAM(eTeam).isVassal((TeamTypes)iI))
			{
				setVassal((TeamTypes)iI, true, isCapitulated());
			}
			else if (isVassal((TeamTypes)iI))
			{
				GET_TEAM(eTeam).setVassal((TeamTypes)iI, true, isCapitulated());
			}
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (iI != eTeam && iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (GET_TEAM((TeamTypes)iI).isVassal(eTeam))
			{
				GET_TEAM((TeamTypes)iI).setVassal(getID(), true, GET_TEAM((TeamTypes)iI).isCapitulated());
			}
			else if (GET_TEAM((TeamTypes)iI).isVassal(getID()))
			{
				GET_TEAM((TeamTypes)iI).setVassal(eTeam, true, GET_TEAM((TeamTypes)iI).isCapitulated());
			}
		}
	}
	shareCounters(eTeam);
	GET_TEAM(eTeam).shareCounters(getID());

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
		{
			GET_PLAYER((PlayerTypes)iI).setTeam(getID());
		}
	}

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* plotX = GC.getMap().plotByIndex(iI);

		plotX->changeVisibilityCount(getID(), plotX->getVisibilityCount(eTeam), NO_INVISIBLE, false);

		for (int iJ = 0; iJ < GC.getNumInvisibleInfos(); iJ++)
		{
			const InvisibleTypes eInvisible = static_cast<InvisibleTypes>(iJ);

			plotX->changeInvisibleVisibilityCount(getID(), eInvisible, plotX->getInvisibleVisibilityCount(eTeam, eInvisible));

			if (GC.getGame().isOption(GAMEOPTION_COMBAT_HIDE_SEEK))
			{
				for (int iK = 0; iK < plotX->getNumPlotTeamVisibilityIntensity(); iK++)
				{
					if (plotX->getPlotTeamVisibilityIntensity(iK).eInvisibility == eInvisible
					&& plotX->getPlotTeamVisibilityIntensity(iK).eTeam == eTeam)
					{
						if (plotX->getPlotTeamVisibilityIntensity(iK).iIntensity > plotX->getHighestPlotTeamVisibilityIntensity(eInvisible, getID()))
						{
							plotX->setSpotIntensity(
								getID(), eInvisible,
								plotX->getPlotTeamVisibilityIntensity(iK).iUnitID,
								plotX->getPlotTeamVisibilityIntensity(iK).iIntensity
							);
						}
					}
				}
			}
		}
		if (plotX->isRevealed(eTeam, false))
		{
			plotX->setRevealed(getID(), true, false, eTeam, false);
		}
	}
	GC.getGame().updatePlotGroups();

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (iI != eTeam && iI != getID())
		{
			GET_TEAM((TeamTypes)iI).setWarWeariness(getID(), ((GET_TEAM((TeamTypes)iI).getWarWeariness(getID()) + GET_TEAM((TeamTypes)iI).getWarWeariness(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).setStolenVisibilityTimer(getID(), ((GET_TEAM((TeamTypes)iI).getStolenVisibilityTimer(getID()) + GET_TEAM((TeamTypes)iI).getStolenVisibilityTimer(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setAtWarCounter(getID(), ((GET_TEAM((TeamTypes)iI).AI_getAtWarCounter(getID()) + GET_TEAM((TeamTypes)iI).AI_getAtWarCounter(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setAtPeaceCounter(getID(), ((GET_TEAM((TeamTypes)iI).AI_getAtPeaceCounter(getID()) + GET_TEAM((TeamTypes)iI).AI_getAtPeaceCounter(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setHasMetCounter(getID(), ((GET_TEAM((TeamTypes)iI).AI_getHasMetCounter(getID()) + GET_TEAM((TeamTypes)iI).AI_getHasMetCounter(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setDefensivePactCounter(getID(), ((GET_TEAM((TeamTypes)iI).AI_getDefensivePactCounter(getID()) + GET_TEAM((TeamTypes)iI).AI_getDefensivePactCounter(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setShareWarCounter(getID(), ((GET_TEAM((TeamTypes)iI).AI_getShareWarCounter(getID()) + GET_TEAM((TeamTypes)iI).AI_getShareWarCounter(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setWarSuccess(getID(), ((GET_TEAM((TeamTypes)iI).AI_getWarSuccess(getID()) + GET_TEAM((TeamTypes)iI).AI_getWarSuccess(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setEnemyPeacetimeTradeValue(getID(), ((GET_TEAM((TeamTypes)iI).AI_getEnemyPeacetimeTradeValue(getID()) + GET_TEAM((TeamTypes)iI).AI_getEnemyPeacetimeTradeValue(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).AI_setEnemyPeacetimeGrantValue(getID(), ((GET_TEAM((TeamTypes)iI).AI_getEnemyPeacetimeGrantValue(getID()) + GET_TEAM((TeamTypes)iI).AI_getEnemyPeacetimeGrantValue(eTeam)) / 2));
			GET_TEAM((TeamTypes)iI).setEspionagePointsAgainstTeam(getID(), std::max(GET_TEAM((TeamTypes)iI).getEspionagePointsAgainstTeam(getID()), GET_TEAM((TeamTypes)iI).getEspionagePointsAgainstTeam(eTeam)));

			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				GET_TEAM((TeamTypes)iI).AI_setWarPlan(getID(), NO_WARPLAN, false);
				GET_TEAM((TeamTypes)iI).AI_setWarPlan(eTeam, NO_WARPLAN, false);
			}
		}
	}
	AI_updateWorstEnemy();
	AI_updateAreaStragies();
	GC.getGame().updateScore(true);
}


void CvTeam::shareItems(TeamTypes eTeam)
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(eTeam != NO_TEAM && eTeam != getID() && eTeam < MAX_PC_TEAMS, CvString::format("eTeam = %d", (int)eTeam).c_str());

	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		if (GET_TEAM(eTeam).isHasTech((TechTypes)iI))
		{
			setHasTech((TechTypes)iI, true, NO_PLAYER, true, false);
		}
	}

	for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
	{
		if (GET_TEAM(eTeam).isForceRevealedBonus((BonusTypes)iI))
		{
			setForceRevealedBonus((BonusTypes)iI, true);
		}
	}

	for (int iI = 0; iI < MAX_PC_TEAMS; ++iI)
	{
		setEspionagePointsAgainstTeam((TeamTypes)iI, std::max(GET_TEAM(eTeam).getEspionagePointsAgainstTeam((TeamTypes)iI), getEspionagePointsAgainstTeam((TeamTypes)iI)));
	}

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(eTeam))
		{
			foreach_(const CvCity * cityX, GET_PLAYER((PlayerTypes)iI).cities())
			{
				foreach_(const BuildingTypes eTypeX, cityX->getHasBuildings())
				{
					if (cityX->isDisabledBuilding(eTypeX))
					{
						continue;
					}
					if (GC.getBuildingInfo(eTypeX).isTeamShare())
					{
						for (int iK = 0; iK < MAX_PC_PLAYERS; iK++)
						{
							if (GET_PLAYER((PlayerTypes)iK).isAliveAndTeam(getID()))
							{
								GET_PLAYER((PlayerTypes)iK).processBuilding(eTypeX, 1, cityX->area());
							}
						}
					}
					processBuilding(eTypeX, 1);
				}
			}
		}
	}

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(eTeam))
		{
			GET_PLAYER((PlayerTypes)iI).AI_updateBonusValue();
		}
	}
}


void CvTeam::shareCounters(TeamTypes eTeam)
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if ((iI != getID()) && (iI != eTeam))
		{
			if (GET_TEAM(eTeam).getWarWeariness((TeamTypes)iI) > getWarWeariness((TeamTypes)iI))
			{
				setWarWeariness(((TeamTypes)iI), GET_TEAM(eTeam).getWarWeariness((TeamTypes)iI));
			}

			if (GET_TEAM(eTeam).getStolenVisibilityTimer((TeamTypes)iI) > getStolenVisibilityTimer((TeamTypes)iI))
			{
				setStolenVisibilityTimer(((TeamTypes)iI), GET_TEAM(eTeam).getStolenVisibilityTimer((TeamTypes)iI));
			}

			if (GET_TEAM(eTeam).AI_getAtWarCounter((TeamTypes)iI) > AI_getAtWarCounter((TeamTypes)iI))
			{
				AI_setAtWarCounter(((TeamTypes)iI), GET_TEAM(eTeam).AI_getAtWarCounter((TeamTypes)iI));
			}

			if (GET_TEAM(eTeam).AI_getAtPeaceCounter((TeamTypes)iI) > AI_getAtPeaceCounter((TeamTypes)iI))
			{
				AI_setAtPeaceCounter(((TeamTypes)iI), GET_TEAM(eTeam).AI_getAtPeaceCounter((TeamTypes)iI));
			}

			if (GET_TEAM(eTeam).AI_getHasMetCounter((TeamTypes)iI) > AI_getHasMetCounter((TeamTypes)iI))
			{
				AI_setHasMetCounter(((TeamTypes)iI), GET_TEAM(eTeam).AI_getHasMetCounter((TeamTypes)iI));
			}

			if (GET_TEAM(eTeam).AI_getOpenBordersCounter((TeamTypes)iI) > AI_getOpenBordersCounter((TeamTypes)iI))
			{
				AI_setOpenBordersCounter(((TeamTypes)iI), GET_TEAM(eTeam).AI_getOpenBordersCounter((TeamTypes)iI));
			}

			if (GET_TEAM(eTeam).AI_getDefensivePactCounter((TeamTypes)iI) > AI_getDefensivePactCounter((TeamTypes)iI))
			{
				AI_setDefensivePactCounter(((TeamTypes)iI), GET_TEAM(eTeam).AI_getDefensivePactCounter((TeamTypes)iI));
			}

			if (GET_TEAM(eTeam).AI_getShareWarCounter((TeamTypes)iI) > AI_getShareWarCounter((TeamTypes)iI))
			{
				AI_setShareWarCounter(((TeamTypes)iI), GET_TEAM(eTeam).AI_getShareWarCounter((TeamTypes)iI));
			}

			if (GET_TEAM(eTeam).AI_getWarSuccess((TeamTypes)iI) > AI_getWarSuccess((TeamTypes)iI))
			{
				AI_setWarSuccess(((TeamTypes)iI), GET_TEAM(eTeam).AI_getWarSuccess((TeamTypes)iI));
			}

			if (GET_TEAM(eTeam).AI_getEnemyPeacetimeTradeValue((TeamTypes)iI) > AI_getEnemyPeacetimeTradeValue((TeamTypes)iI))
			{
				AI_setEnemyPeacetimeTradeValue(((TeamTypes)iI), GET_TEAM(eTeam).AI_getEnemyPeacetimeTradeValue((TeamTypes)iI));
			}

			if (GET_TEAM(eTeam).AI_getEnemyPeacetimeGrantValue((TeamTypes)iI) > AI_getEnemyPeacetimeGrantValue((TeamTypes)iI))
			{
				AI_setEnemyPeacetimeGrantValue(((TeamTypes)iI), GET_TEAM(eTeam).AI_getEnemyPeacetimeGrantValue((TeamTypes)iI));
			}

			GET_TEAM(eTeam).AI_setWarPlan(((TeamTypes)iI), NO_WARPLAN, false);
		}
	}

	for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		int iExtraProjects = GET_TEAM(eTeam).getProjectCount((ProjectTypes)iI) - getProjectCount((ProjectTypes)iI);
		if (iExtraProjects > 0)
		{
			changeProjectCount((ProjectTypes)iI, iExtraProjects);
			GC.getGame().incrementProjectCreatedCount((ProjectTypes)iI, -iExtraProjects);
		}

		changeProjectMaking(((ProjectTypes)iI), GET_TEAM(eTeam).getProjectMaking((ProjectTypes)iI));
	}

	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		changeUnitCount(((UnitTypes)iI), GET_TEAM(eTeam).getUnitCount((UnitTypes)iI));
	}

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		changeBuildingCount((BuildingTypes)iI, GET_TEAM(eTeam).getBuildingCount((BuildingTypes)iI));
	}

	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		// Overflow from techs this team already has can cause bugged behavior
		if (!isHasTech((TechTypes)iI)
		&& GET_TEAM(eTeam).getResearchProgress((TechTypes)iI) > getResearchProgress((TechTypes)iI))
		{
			setResearchProgress(((TechTypes)iI), GET_TEAM(eTeam).getResearchProgress((TechTypes)iI), getLeaderID());
		}
		// Clear no tech trade if it is false for other team
		// Fixes bug where if, with no tech brokering, team A trades a tech to team B, then later joins B in
		// a permanent alliance.  Previous code would block the AB alliance from "brokering" the tech, even
		// though A had researched it on their own.
		if (GET_TEAM(eTeam).isHasTech((TechTypes)iI) && !GET_TEAM(eTeam).isNoTradeTech((TechTypes)iI))
		{
			setNoTradeTech(((TechTypes)iI), false);
		}
	}
}


void CvTeam::processBuilding(BuildingTypes eBuilding, int iChange, bool bReligiouslyDisabling)
{
	PROFILE_FUNC();

	if (!bReligiouslyDisabling)
	{
		for (int i = 0; i < GC.getNumVoteSourceInfos(); ++i)
		{
			if (GC.getBuildingInfo(eBuilding).getVoteSourceType() == i)
			{
				changeForceTeamVoteEligibilityCount((VoteSourceTypes)i, (GC.getBuildingInfo(eBuilding).isForceTeamVoteEligible()) ? iChange : 0);
			}
		}
		if (iChange > 0 && GC.getBuildingInfo(eBuilding).isMapCentering())
		{
			setMapCentering(true);
		}
	}
	changeEnemyWarWearinessModifier(GC.getBuildingInfo(eBuilding).getEnemyWarWearinessModifier() * iChange);
}


void CvTeam::doTurn()
{
	PROFILE("CvTeam::doTurn()");

	FAssertMsg(isAlive(), "isAlive is expected to be true");

	AI_doTurnPre();

	// TB - Keep this valid only for barbs and neanderthals for now.
	// We may need new rules here for aliens and such, but there's really no need to do anything for animals regarding technology unless we want to represent evolution somehow... lol.
	if (isHominid())
	{
		foreach_(const TechTypes eTechX, getAdjacentResearch())
		{
			if (GET_PLAYER(getLeaderID()).canResearch(eTechX))
			{
				int iPossibleCount = 0;
				int iCount = 0;
				for (int iJ = 0; iJ < MAX_PC_TEAMS; iJ++)
				{
					if (GET_TEAM((TeamTypes)iJ).isAlive())
					{
						if (GET_TEAM((TeamTypes)iJ).isHasTech(eTechX))
						{
							iCount++;
						}
						iPossibleCount++;
					}
				}
				if (iCount > 0)
				{
					changeResearchProgress(eTechX, std::max(1, getResearchCost(eTechX) * GC.getBARBARIAN_FREE_TECH_PERCENT() * iCount / (100 * iPossibleCount)), getLeaderID());
				}
			}
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (getStolenVisibilityTimer((TeamTypes)iI) > 0)
			{
				changeStolenVisibilityTimer(((TeamTypes)iI), -1);
			}

			if (getCounterespionageTurnsLeftAgainstTeam((TeamTypes)iI) > 0)
			{
				changeCounterespionageTurnsLeftAgainstTeam((TeamTypes)iI, -1);
			}

			if (getCounterespionageTurnsLeftAgainstTeam((TeamTypes)iI) == 0)
			{
				setCounterespionageModAgainstTeam((TeamTypes)iI, 0);
			}
		}
	}
	doWarWeariness();

	testCircumnavigated();

	AI_doTurnPost();
}


void CvTeam::updateYield()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			GET_PLAYER((PlayerTypes)iI).updateYield();
		}
	}
}


void CvTeam::updateCommerce()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			GET_PLAYER((PlayerTypes)iI).updateCommerce();
		}
	}
}


bool CvTeam::canChangeWarPeace(TeamTypes eTeam, bool bAllowVassal) const
{
	PROFILE_EXTRA_FUNC();
	if (eTeam == getID())
	{
		return false;
	}

	if (isPermanentWarPeace(eTeam) || GET_TEAM(eTeam).isPermanentWarPeace(getID()))
	{
		return false;
	}

	if (isMinorCiv() || GET_TEAM(eTeam).isMinorCiv())
	{
		return false; // No dealing with minor civs
	}

	for (int iLoopTeam = 0; iLoopTeam < MAX_PC_TEAMS; ++iLoopTeam)
	{
		CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iLoopTeam);
		if (kLoopTeam.isAlive())
		{
			if (kLoopTeam.isVassal(getID()) && kLoopTeam.isPermanentWarPeace(eTeam)
			|| kLoopTeam.isVassal(eTeam) && kLoopTeam.isPermanentWarPeace(getID()))
			{
				return false;
			}
		}
	}

	if (isAVassal())
	{
		return false;
	}

	if (bAllowVassal)
	{
		if (GET_TEAM(eTeam).isVassal(getID()))
		{
			return false;
		}
	}
	else if (GET_TEAM(eTeam).isAVassal())
	{
		return false;
	}
	return true;
}


bool CvTeam::canDeclareWar(TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	if (eTeam == getID() || !isAlive() || !GET_TEAM(eTeam).isAlive())
	{
		return false;
	}
	if (!isHasMet(eTeam) || isAtWar(eTeam) || isForcePeace(eTeam))
	{
		return false;
	}
	for (int i = 0; i < MAX_PC_TEAMS; ++i)
	{
		if (i != eTeam && i != getID() && GET_TEAM(eTeam).isVassal((TeamTypes)i) && isForcePeace((TeamTypes)i))
		{
			return false;
		}
	}
	if (!canChangeWarPeace(eTeam, true))
	{
		return false;
	}
	return true;
}

/// \brief Version of canDeclareWar which ignores temporary peace treaties.
///
/// This function is for AIs considering who to start war preparations against, so they're future
/// plans aren't unnecessarily affected by current conditions.
///
/// Could not change definition of canDeclareWar, some sporadic crash-inducing compatibility issue
/// with the DLL it seems.  Lost a lot of time tracking down the source of the crash, it really is
/// just from adding bool bWhatever = false to canDeclareWar in CvTeam.h.  So, that's why there's
/// this overlapping second function.
bool CvTeam::canEventuallyDeclareWar(TeamTypes eTeam) const
{
	if (eTeam == getID() || !isAlive() || !GET_TEAM(eTeam).isAlive())
	{
		return false;
	}
	if (isAtWar(eTeam) || !isHasMet(eTeam) || !canChangeWarPeace(eTeam, true))
	{
		return false;
	}
	return true;
}


void CvTeam::declareWar(TeamTypes eTeam, bool bNewDiplo, WarPlanTypes eWarPlan)
{
	PROFILE_FUNC();
	FAssertMsg(eTeam != NO_TEAM, "eTeam is not assigned a valid value");
	FAssertMsg(eTeam != getID(), "eTeam is not expected to be equal with getID()");

	CvTeamAI& teamFoe = GET_TEAM(eTeam);

	if (GC.getGame().isOption(GAMEOPTION_ANIMAL_PEACE_AMONG_NPCS) && isNPC() && teamFoe.isNPC())
	{
		if (isAtWar(eTeam))
		{
			setAtWar(eTeam, false);
			teamFoe.setAtWar(getID(), false);
		}
		return;
	}

	if (!isAtWar(eTeam))
	{
		if (gTeamLogLevel >= 1)
		{
			logBBAI("  Team %d (%S) declares war on team %d", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eTeam);
		}
		foreach_(CvDeal & kLoopDeal, GC.getGame().deals())
		{
			if (GET_PLAYER(kLoopDeal.getFirstPlayer()).getTeam() == getID() && GET_PLAYER(kLoopDeal.getSecondPlayer()).getTeam() == eTeam
			|| GET_PLAYER(kLoopDeal.getFirstPlayer()).getTeam() == eTeam && GET_PLAYER(kLoopDeal.getSecondPlayer()).getTeam() == getID())
			{
				kLoopDeal.kill();
			}
		}
		const bool bInFull = (!teamFoe.isNPC() || teamFoe.isHominid()) && (!isNPC() || isHominid());

		if (bInFull)
		{
			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				CvPlayer& playerX = GET_PLAYER((PlayerTypes)iI);

				if (playerX.isAliveAndTeam(getID(), true, eTeam))
				{
					playerX.updatePlunder(-1, false);

					if (playerX.getTeam() == eTeam)
					{
						if (playerX.getPledgedSecretaryGeneralVote() == getID())
						{
							playerX.setPledgedSecretaryGeneralVote(NO_TEAM);
						}
					}
					else if (playerX.getPledgedSecretaryGeneralVote() == eTeam)
					{
						playerX.setPledgedSecretaryGeneralVote(NO_TEAM);
					}
				}
			}
		}
		setAtWar(eTeam, true);
		teamFoe.setAtWar(getID(), true);

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID(), true, eTeam))
			{
				GET_PLAYER((PlayerTypes)iI).updatePlunder(1, false);
			}
		}
		meet(eTeam, false);

		AI_setAtPeaceCounter(eTeam, 0);
		teamFoe.AI_setAtPeaceCounter(getID(), 0);

		AI_setShareWarCounter(eTeam, 0);
		teamFoe.AI_setShareWarCounter(getID(), 0);

		teamFoe.AI_setWarPlan(getID(), isNPC() || isMinorCiv() ? WARPLAN_ATTACKED : WARPLAN_ATTACKED_RECENT, true, bInFull);

		if (!isNPC() && !isMinorCiv() && !teamFoe.isMinorCiv() && !teamFoe.isNPC())
		{
			for (int iI = 0; iI < MAX_TEAMS; iI++)
			{
				if (GET_TEAM((TeamTypes)iI).isAlive()
				&& !teamFoe.isAtWar((TeamTypes)iI) && teamFoe.AI_isChosenWar((TeamTypes)iI))
				{
					teamFoe.AI_setWarPlan((TeamTypes)iI, NO_WARPLAN);
				}
			}
		}

		if (NO_WARPLAN != eWarPlan)
		{
			AI_setWarPlan(eTeam, eWarPlan, true, bInFull);
		}

		FAssert(!AI_isSneakAttackPreparing(eTeam));
		if (AI_getWarPlan(eTeam) == NO_WARPLAN || AI_isSneakAttackPreparing(eTeam))
		{
			if (isHuman())
			{
				AI_setWarPlan(eTeam, WARPLAN_TOTAL);
			}
			else if (isMinorCiv() || isNPC() || teamFoe.getAtWarCount(true) == 1)
			{
				AI_setWarPlan(eTeam, WARPLAN_LIMITED);
			}
			else
			{
				AI_setWarPlan(eTeam, WARPLAN_DOGPILE);
			}
		}

		if (bInFull)
		{
			GC.getMap().verifyUnitValidPlot();
		}

		//isolated NPC war declarations as it happens now on each load process at a point when such validation cannot take place in the environment that isn't fully initialized yet.
		if (!teamFoe.isNPC())
		{
			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
				{
					GET_PLAYER((PlayerTypes)iI).verifyUnitStacksValid();
				}
			}
		}

		GC.getGame().AI_makeAssignWorkDirty();

		if ((getID() == GC.getGame().getActiveTeam()) || (eTeam == GC.getGame().getActiveTeam()))
		{
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}

		if (!isNPC() && !teamFoe.isNPC() && !isMinorCiv() && !teamFoe.isMinorCiv())
		{
			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
				{
					for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
					{
						if (GET_PLAYER((PlayerTypes)iJ).isAliveAndTeam(eTeam))
						{
							GET_PLAYER((PlayerTypes)iI).AI_setFirstContact((PlayerTypes)iJ, true);
							GET_PLAYER((PlayerTypes)iJ).AI_setFirstContact((PlayerTypes)iI, true);
						}
					}
				}
			}

			if (GC.getGame().isOption(GAMEOPTION_AI_RUTHLESS))
			{
				for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
					{
						//Calculate players we just backstabbed
						for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
						{
							//Player is on the team we are declaring war on
							if (GET_PLAYER((PlayerTypes)iJ).isAliveAndTeam(eTeam)
							// and have good relation with us
							&& GET_PLAYER((PlayerTypes)iJ).AI_getAttitude((PlayerTypes)iI) >= ATTITUDE_PLEASED)
							{
								GET_PLAYER((PlayerTypes)iJ).AI_changeMemoryCount((PlayerTypes)iI, MEMORY_BACKSTAB, 1);
							}
						}
						//Calculate players whose friend we just backstabbed
						for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
						{
							// Player is not on our team nor on the team we declared war on.
							if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID(), false) && GET_PLAYER((PlayerTypes)iJ).getTeam() != eTeam
							// Friends with the leader of the team we declared war on
							&& GET_PLAYER((PlayerTypes)iJ).AI_getAttitude(teamFoe.getLeaderID()) >= ATTITUDE_PLEASED)
							{
								GET_PLAYER((PlayerTypes)iJ).AI_changeMemoryCount(((PlayerTypes)iI), MEMORY_BACKSTAB_FRIEND, 1);
							}
						}
					}
				}
			}

			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				// Players from our Team
				if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
				{
					for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
					{
						// Players not from our Team
						if (GET_PLAYER((PlayerTypes)iJ).isAliveAndTeam(getID(), false))
						{
							// Who we declared war on ...
							if (GET_PLAYER((PlayerTypes)iJ).getTeam() == eTeam)
							{
								GET_PLAYER((PlayerTypes)iJ).AI_changeMemoryCount((PlayerTypes)iI, MEMORY_DECLARED_WAR, 1);
							}
							// ... or their friends
							else if (teamFoe.isHasMet(GET_PLAYER((PlayerTypes)iJ).getTeam())
							&& GET_TEAM(GET_PLAYER((PlayerTypes)iJ).getTeam()).AI_getAttitude(eTeam) >= ATTITUDE_PLEASED
							&& !GET_TEAM(GET_PLAYER((PlayerTypes)iJ).getTeam()).isAtWar(eTeam))
							{
								GET_PLAYER((PlayerTypes)iJ).AI_changeMemoryCount((PlayerTypes)iI, MEMORY_DECLARED_WAR_ON_FRIEND, 1);
							}
						}
					}
				}
			}
		}

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID(), true, eTeam))
			{
				GET_PLAYER((PlayerTypes)iI).updateWarWearinessPercentAnger();
				GET_PLAYER((PlayerTypes)iI).updatePlotGroups();
			}
		}
		// updatePlotGroups calls updateTradeRoutes at the end, so I'm not sure this is needed.
		// Maybe all plot groups must be up to date for all players before trade routes can be updated properly?
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID(), true, eTeam))
			{
				GET_PLAYER((PlayerTypes)iI).updateTradeRoutes();
			}
		}

		// Attitude cache
		if (GC.getGame().isFinalInitialized())
		{
			for (int a = 0; a < MAX_PC_PLAYERS; a++)
			{
				CvPlayerAI& playerA = GET_PLAYER((PlayerTypes)a);

				if (playerA.isAlive())
				{
					const TeamTypes eTeamA = playerA.getTeam();

					if (eTeamA == getID() || eTeamA == eTeam || GET_TEAM(eTeamA).isAtWar(getID()) || GET_TEAM(eTeamA).isAtWar(eTeam))
					{
						for (int b = 0; b < MAX_PC_PLAYERS; b++)
						{
							CvPlayerAI& playerB = GET_PLAYER((PlayerTypes)b);

							if (playerB.isAliveAndTeam(eTeamA, false) && (playerB.getTeam() == getID() || playerB.getTeam() == eTeam))
							{
								playerA.AI_invalidateAttitudeCache((PlayerTypes)b);
								playerB.AI_invalidateAttitudeCache((PlayerTypes)a);
							}
						}
					}
				}
			}
		}

		if (GC.getGame().isFinalInitialized() && !(gDLL->GetWorldBuilderMode()))
		{
			if (bNewDiplo && !isHuman())
			{
				for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(eTeam)
					&& GET_PLAYER((PlayerTypes)iI).isHumanPlayer()
					&& GET_PLAYER(getLeaderID()).canContact((PlayerTypes)iI))
					{
						CvDiploParameters* pDiplo = new CvDiploParameters(getLeaderID());
						FAssertMsg(pDiplo != NULL, "pDiplo must be valid");
						pDiplo->setDiploComment(GC.getAI_DIPLOCOMMENT_DECLARE_WAR());
						pDiplo->setAIContact(true);
						gDLL->beginDiplomacy(pDiplo, (PlayerTypes)iI);
					}
				}
			}

			if (!isNPC() && !teamFoe.isNPC() && !isMinorCiv() && !teamFoe.isMinorCiv())
			{
				for (int iI = 0; iI < MAX_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive())
					{

						if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
						{
							AddDLLMessage(
								(PlayerTypes)iI, true, GC.getEVENT_MESSAGE_TIME(),
								gDLL->getText("TXT_KEY_MISC_YOU_DECLARED_WAR_ON", teamFoe.getName().GetCString()),
								"AS2D_DECLAREWAR", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_WARNING_TEXT()
							);
						}
						else if (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
						{
							AddDLLMessage(
								(PlayerTypes)iI, true, GC.getEVENT_MESSAGE_TIME(),
								gDLL->getText("TXT_KEY_MISC_DECLARED_WAR_ON_YOU", getName().GetCString()),
								"AS2D_DECLAREWAR", MESSAGE_TYPE_MAJOR_EVENT,
								NULL, GC.getCOLOR_WARNING_TEXT()
							);
						}
						else if (isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam())
						&& teamFoe.isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
						{
							AddDLLMessage(
								(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(),
								gDLL->getText(
									"TXT_KEY_MISC_SOMEONE_DECLARED_WAR",
									getName().GetCString(), teamFoe.getName().GetCString()
								),
								"AS2D_THEIRDECLAREWAR", MESSAGE_TYPE_MAJOR_EVENT,
								NULL, GC.getCOLOR_WARNING_TEXT()
							);
						}
					}
				}
				GC.getGame().addReplayMessage(
					REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(),
					gDLL->getText(
						"TXT_KEY_MISC_SOMEONE_DECLARES_WAR",
						getName().GetCString(), teamFoe.getName().GetCString()
					),
					-1, -1, GC.getCOLOR_WARNING_TEXT()
				);
			}
		}

		CvEventReporter::getInstance().changeWar(true, getID(), eTeam);

		if (!isNPC() && !teamFoe.isNPC() && !isMinorCiv() && !teamFoe.isMinorCiv())
		{
			if (GC.getDefineINT("BBAI_ALLIANCE_OPTION") != 1)
			{
				cancelDefensivePacts();
			}

			for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
			{
				if (iI != getID() && iI != eTeam && GET_TEAM((TeamTypes)iI).isAlive())
				{
					if (GET_TEAM((TeamTypes)iI).isDefensivePact(eTeam))
					{
						GET_TEAM((TeamTypes)iI).declareWar(getID(), bNewDiplo, WARPLAN_DOGPILE);
					}
					else if (GET_TEAM((TeamTypes)iI).isDefensivePact(getID()))
					{
						// For alliance option.  This teams pacts are canceled above if not using alliance option.
						GET_TEAM((TeamTypes)iI).declareWar(eTeam, bNewDiplo, WARPLAN_DOGPILE);
					}
				}
			}

			if (GC.getDefineINT("BBAI_ALLIANCE_OPTION") != 1)
			{
				teamFoe.cancelDefensivePacts();
			}

			for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
			{
				if (iI != eTeam && iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
				{
					if (GET_TEAM((TeamTypes)iI).isVassal(eTeam) || teamFoe.isVassal((TeamTypes)iI))
					{
						declareWar((TeamTypes)iI, bNewDiplo, AI_getWarPlan(eTeam));
					}
					else if (GET_TEAM((TeamTypes)iI).isVassal(getID()) || isVassal((TeamTypes)iI))
					{
						GET_TEAM((TeamTypes)iI).declareWar(eTeam, bNewDiplo, WARPLAN_DOGPILE);
					}
				}
			}
		}
	}
}


void CvTeam::makePeace(TeamTypes eTeam, bool bBumpUnits)
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(eTeam != NO_TEAM, "eTeam is not assigned a valid value");
	FAssertMsg(eTeam != getID(), "eTeam is not expected to be equal with getID()");

	if (isAtWar(eTeam))
	{
		if (gTeamLogLevel >= 1 && isAlive() && GET_TEAM(eTeam).isAlive())
		{
			logBBAI("Team %d (%S) and team %d (%S) make peace", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eTeam, GET_PLAYER(GET_TEAM(eTeam).getLeaderID()).getCivilizationDescription(0));
		}

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			//	Check isEverAlive() here rather than isAlive() since it's just an optimization (updatePlunder() will
			//	do nothing for a truely dead player as they have no units), and this method is called by REV Python
			//	in response to a plyer setAlive (false) event notification DURING the processing tof that player's
			//	death.  If we do not treat THAT player as alive here their blockades will not be lifted
			if ((GET_PLAYER((PlayerTypes)iI).getTeam() == getID() || GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam) && GET_PLAYER((PlayerTypes)iI).isEverAlive())
			{
				GET_PLAYER((PlayerTypes)iI).updatePlunder(-1, false);
			}
		}

		FAssertMsg(eTeam != getID(), "eTeam is not expected to be equal with getID()");
		setAtWar(eTeam, false);
		GET_TEAM(eTeam).setAtWar(getID(), false);

		// Plot danger cache
		GC.getMap().invalidateBorderDangerCache(eTeam);
		GC.getMap().invalidateBorderDangerCache(getID());

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID(), true, eTeam))
			{
				GET_PLAYER((PlayerTypes)iI).updatePlunder(1, false);
			}
		}

		AI_setAtWarCounter(eTeam, 0);
		GET_TEAM(eTeam).AI_setAtWarCounter(getID(), 0);

		AI_setWarSuccess(eTeam, 0);
		GET_TEAM(eTeam).AI_setWarSuccess(getID(), 0);

		AI_setWarPlan(eTeam, NO_WARPLAN);
		GET_TEAM(eTeam).AI_setWarPlan(getID(), NO_WARPLAN);

		if (bBumpUnits)
		{
			GC.getMap().verifyUnitValidPlot();
		}

		GC.getGame().AI_makeAssignWorkDirty();

		if ((getID() == GC.getGame().getActiveTeam()) || (eTeam == GC.getGame().getActiveTeam()))
		{
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID(), true, eTeam))
			{
				GET_PLAYER((PlayerTypes)iI).updateWarWearinessPercentAnger();
				GET_PLAYER((PlayerTypes)iI).updatePlotGroups();
			}
		}
		// updatePlotGroups calls updateTradeRoutes at the end, so I'm not sure this is needed.
		// Maybe all plot groups must be up to date for all players before trade routes can be updated properly?
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID(), true, eTeam))
			{
				GET_PLAYER((PlayerTypes)iI).updateTradeRoutes();
			}
		}

		// Attitude cache
		if (GC.getGame().isFinalInitialized())
		{
			for (int iA = 0; iA < MAX_PC_PLAYERS; iA++)
			{
				CvPlayerAI& playerA = GET_PLAYER((PlayerTypes)iA);

				if (playerA.isAlive())
				{
					const TeamTypes eTeamA = playerA.getTeam();

					if (eTeamA == getID() || eTeamA == eTeam || GET_TEAM(eTeamA).isAtWar(getID()) || GET_TEAM(eTeamA).isAtWar(eTeam))
					{
						for (int iB = 0; iB < MAX_PC_PLAYERS; iB++)
						{
							CvPlayerAI& playerB = GET_PLAYER((PlayerTypes)iB);

							if (playerB.isAliveAndTeam(eTeamA, false) && (playerB.getTeam() == getID() || playerB.getTeam() == eTeam))
							{
								playerA.AI_invalidateAttitudeCache((PlayerTypes)iB);
								playerB.AI_invalidateAttitudeCache((PlayerTypes)iA);
							}
						}
					}
				}
			}
		}

		// Silence announcement when one team has no units or cities
		// Needed because civs are now set to peace with all when dieing since they keep at war state when being reborn
		bool bHasUnitsOrCities = getNumCities() > 0;
		bool bHasUnitsOrCities2 = GET_TEAM(eTeam).getNumCities() > 0;

		// Could be homeless rebel, must check
		if (!bHasUnitsOrCities)
		{
			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID())
				&& GET_PLAYER((PlayerTypes)iI).getNumUnits() > 0)
				{
					bHasUnitsOrCities = true;
					break;
				}
			}
		}
		if (!bHasUnitsOrCities2)
		{
			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(eTeam)
				&& GET_PLAYER((PlayerTypes)iI).getNumUnits() > 0)
				{
					bHasUnitsOrCities2 = true;
					break;
				}
			}
		}

		// Anounce only if neither is minor
		if (!isMinorCiv() && !GET_TEAM(eTeam).isMinorCiv())
		{
			// Announce if both are really alive
			if (bHasUnitsOrCities && bHasUnitsOrCities2)
			{
				for (int iI = 0; iI < MAX_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive())
					{

						if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
						{
							const CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_MADE_PEACE_WITH", GET_TEAM(eTeam).getName().GetCString());
							AddDLLMessage(((PlayerTypes)iI), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_MAKEPEACE", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_HIGHLIGHT_TEXT());
						}
						else if (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
						{
							const CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_MADE_PEACE_WITH", getName().GetCString());
							AddDLLMessage(((PlayerTypes)iI), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_MAKEPEACE", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_HIGHLIGHT_TEXT());
						}
						else if (GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasMet(getID()) && GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasMet(eTeam))
						{
							const CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_SOMEONE_MADE_PEACE", getName().GetCString(), GET_TEAM(eTeam).getName().GetCString());
							AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_THEIRMAKEPEACE", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_HIGHLIGHT_TEXT());
						}
					}
				}

				const CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_SOMEONE_MADE_PEACE", getName().GetCString(), GET_TEAM(eTeam).getName().GetCString());
				GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), szBuffer, -1, -1, GC.getCOLOR_HIGHLIGHT_TEXT());
			}
		}

		CvEventReporter::getInstance().changeWar(false, getID(), eTeam);

		for (int iI = 0; iI < MAX_TEAMS; iI++)
		{
			if (iI != getID() && iI != eTeam && GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (GET_TEAM((TeamTypes)iI).isVassal(eTeam))
				{
					GET_TEAM((TeamTypes)iI).makePeace(getID(), bBumpUnits);
				}
				else if (GET_TEAM((TeamTypes)iI).isVassal(getID()))
				{
					GET_TEAM((TeamTypes)iI).makePeace(eTeam, bBumpUnits);
				}
			}
		}
	}
}


bool CvTeam::canContact(TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
			{
				if (GET_PLAYER((PlayerTypes)iJ).isAliveAndTeam(eTeam)
				&& GET_PLAYER((PlayerTypes)iI).canContact((PlayerTypes)iJ))
				{
					return true;
				}
			}
		}
	}
	return false;
}


void CvTeam::meet(TeamTypes eTeam, bool bNewDiplo)
{
	if (!isHasMet(eTeam))
	{
		makeHasMet(eTeam, bNewDiplo);
		GET_TEAM(eTeam).makeHasMet(getID(), bNewDiplo);

		if (gTeamLogLevel >= 2 && GC.getGame().isFinalInitialized() && eTeam != getID() && isAlive() && GET_TEAM(eTeam).isAlive())
		{
			logBBAI("    Team %d (%S) meets team %d (%S)", getID(), GET_PLAYER(getLeaderID()).getCivilizationDescription(0), eTeam, GET_PLAYER(GET_TEAM(eTeam).getLeaderID()).getCivilizationDescription(0));
		}
	}
}


void CvTeam::signOpenBorders(TeamTypes eTeam)
{
	CLinkList<TradeData> ourList;
	CLinkList<TradeData> theirList;
	TradeData item;

	FAssert(eTeam != NO_TEAM);
	FAssert(eTeam != getID());

	if (!isAtWar(eTeam) && (getID() != eTeam))
	{
		setTradeItem(&item, TRADE_OPEN_BORDERS);

		if (GET_PLAYER(getLeaderID()).canTradeItem(GET_TEAM(eTeam).getLeaderID(), item) && GET_PLAYER(GET_TEAM(eTeam).getLeaderID()).canTradeItem(getLeaderID(), item))
		{
			ourList.clear();
			theirList.clear();

			ourList.insertAtEnd(item);
			theirList.insertAtEnd(item);

			GC.getGame().implementDeal(getLeaderID(), (GET_TEAM(eTeam).getLeaderID()), &ourList, &theirList);
		}
	}
}

void CvTeam::signDefensivePact(TeamTypes eTeam)
{
	CLinkList<TradeData> ourList;
	CLinkList<TradeData> theirList;
	TradeData item;

	FAssert(eTeam != NO_TEAM);
	FAssert(eTeam != getID());

	if (!isAtWar(eTeam) && (getID() != eTeam))
	{
		setTradeItem(&item, TRADE_DEFENSIVE_PACT);

		if (GET_PLAYER(getLeaderID()).canTradeItem(GET_TEAM(eTeam).getLeaderID(), item) && GET_PLAYER(GET_TEAM(eTeam).getLeaderID()).canTradeItem(getLeaderID(), item))
		{
			ourList.clear();
			theirList.clear();

			ourList.insertAtEnd(item);
			theirList.insertAtEnd(item);

			GC.getGame().implementDeal(getLeaderID(), (GET_TEAM(eTeam).getLeaderID()), &ourList, &theirList);
		}
	}
}

bool CvTeam::canSignDefensivePact(TeamTypes eTeamB) const
{
	PROFILE_EXTRA_FUNC();
	if (!isDefensivePactTrading() || isAVassal() || isAtWar())
	{
		return false;
	}
	if (GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY) && !isHasEmbassy(eTeamB))
	{
		return false;
	}
	const CvTeam& teamB = GET_TEAM(eTeamB);
	if (!teamB.isDefensivePactTrading() || teamB.isAVassal() || teamB.isAtWar())
	{
		return false;
	}

	for (int iI = 0; iI < MAX_PC_TEAMS; ++iI)
	{
		if (iI != getID() && iI != eTeamB)
		{
			const CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
			if (kLoopTeam.isPermanentWarPeace(eTeamB) != kLoopTeam.isPermanentWarPeace(getID()))
			{
				return false;
			}

			if (isPermanentWarPeace((TeamTypes)iI) != teamB.isPermanentWarPeace((TeamTypes)iI))
			{
				return false;
			}
		}
	}
	return true;
}

int CvTeam::getAssets() const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getAssets();
		}
	}
	return iCount;
}

int CvTeam::getPower(bool bIncludeVassals) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.isAlive() && (kLoopPlayer.getTeam() == getID() || (bIncludeVassals && GET_TEAM(kLoopPlayer.getTeam()).isVassal(getID()))))
		{
			iCount += kLoopPlayer.getPower();

			if (GET_TEAM(kLoopPlayer.getTeam()).isVassal(getID()))
			{
				iCount -= kLoopPlayer.getTechPower();
			}
		}
	}

	return iCount;
}

int CvTeam::getDefensivePower(TeamTypes eExcludeTeam) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	FAssert(eExcludeTeam != getID());

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
		if (kLoopTeam.isAlive() && !kLoopTeam.isAVassal())
		{
			// K-Mod: added "eExcludeTeam" argument, so that defensive power can take into account the cancelation of pacts.
			if (iI != eExcludeTeam && (getID() == iI || isVassal((TeamTypes)iI) || isDefensivePact((TeamTypes)iI)))
			{
				iCount += kLoopTeam.getPower(true);
			}
		}
	}

	return iCount;
}

int CvTeam::getEnemyPower() const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		const CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
		if (kLoopTeam.isAlive() && (getID() != iI && isAtWar((TeamTypes)iI)))
		{
			iCount += kLoopTeam.getPower(false);
		}
	}

	return iCount;
}

int CvTeam::getNumNukeUnits() const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.isAlive() && (kLoopPlayer.getTeam() == getID() || GET_TEAM(kLoopPlayer.getTeam()).isVassal(getID())))
		{
			iCount += kLoopPlayer.getNumNukeUnits();
		}
	}

	return iCount;
}

// Toffer - Used by barbarian unit spawning "CvGame::createBarbarianUnits()",
//		and for the new game starting units "CvPlayer::addStartUnitAI(...)".
// Barbs don't need the bonus req for units, but they must have the tech that enables it.
bool CvTeam::isUnitBonusEnabledByTech(const CvUnitInfo& unit, const bool bNoWorldBonuses) const
{
	PROFILE_EXTRA_FUNC();
	if (unit.getPrereqAndBonus() != NO_BONUS)
	{
		if (!isHasTech((TechTypes)GC.getBonusInfo((BonusTypes)unit.getPrereqAndBonus()).getTechCityTrade())
		|| bNoWorldBonuses // Toffer - TODO - Add maxGlobalInstances tag to CvBonusInfo.
		&& GC.getBonusInfo((BonusTypes)unit.getPrereqAndBonus()).getBonusClassType() == GC.getInfoTypeForString("BONUSCLASS_CULTURE"))
		{
			return false;
		}
	}
	bool bMet = true;
	foreach_(const BonusTypes ePrereqBonus, unit.getPrereqOrBonuses())
	{
		if (isHasTech((TechTypes)GC.getBonusInfo(ePrereqBonus).getTechCityTrade())
		&& (!bNoWorldBonuses || GC.getBonusInfo(ePrereqBonus).getBonusClassType() != GC.getInfoTypeForString("BONUSCLASS_CULTURE")))
		{
			bMet = true;
			break;
		}
		bMet = false;
	}
	return bMet;
}


int CvTeam::getVotes(VoteTypes eVote, VoteSourceTypes eVoteSource) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getVotes(eVote, eVoteSource);
		}
	}
	return iCount;
}

bool CvTeam::isVotingMember(VoteSourceTypes eVoteSource) const
{
	return (getVotes(NO_VOTE, eVoteSource) > 0);
}

bool CvTeam::isFullMember(VoteSourceTypes eVoteSource) const
{
	PROFILE_EXTRA_FUNC();
	if (isForceTeamVoteEligible(eVoteSource))
	{
		return true;
	}

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID())
		&& !GET_PLAYER((PlayerTypes)iI).isFullMember(eVoteSource))
		{
			return false;
		}
	}
	return true;
}

int CvTeam::getAtWarCount(bool bIgnoreMinors, bool bIgnoreVassals) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive()
		&& (!bIgnoreMinors || !GET_TEAM((TeamTypes)iI).isMinorCiv())
		&& (!bIgnoreVassals || !GET_TEAM((TeamTypes)iI).isAVassal())
		&& isAtWar((TeamTypes)iI))
		{
			FAssert(iI != getID());
			FAssert(!AI_isSneakAttackPreparing((TeamTypes)iI));
			iCount++;
		}
	}
	return iCount;
}

int CvTeam::getWarPlanCount(WarPlanTypes eWarPlan, bool bIgnoreMinors) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive()
		&& (!bIgnoreMinors || !GET_TEAM((TeamTypes)iI).isMinorCiv())
		&& AI_getWarPlan((TeamTypes)iI) == eWarPlan)
		{
			FAssert(iI != getID());
			iCount++;
		}
	}
	return iCount;
}


int CvTeam::getAnyWarPlanCount(bool bIgnoreMinors) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive()
		&& (!bIgnoreMinors || !GET_TEAM((TeamTypes)iI).isMinorCiv())
		&& AI_getWarPlan((TeamTypes)iI) != NO_WARPLAN)
		{
			FAssert(iI != getID());
			iCount++;
		}
	}
	FAssert(iCount >= getAtWarCount(bIgnoreMinors));

	return iCount;
}

bool CvTeam::hasWarPlan(bool bIgnoreMinors) const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive()
		&& (!bIgnoreMinors || !GET_TEAM((TeamTypes)iI).isMinorCiv())
		&& AI_getWarPlan((TeamTypes)iI) != NO_WARPLAN)
		{
			return true;
		}
	}
	return false;
}


int CvTeam::getChosenWarCount(bool bIgnoreMinors) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive()
		&& (!bIgnoreMinors || !GET_TEAM((TeamTypes)iI).isMinorCiv())
		&& AI_isChosenWar((TeamTypes)iI))
		{
			FAssert(iI != getID());
			iCount++;
		}
	}
	return iCount;
}


int CvTeam::getHasMetCivCount(bool bIgnoreMinors) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getID()
		&& isHasMet((TeamTypes)iI)
		&& (!bIgnoreMinors || !GET_TEAM((TeamTypes)iI).isMinorCiv()))
		{
			iCount++;
		}
	}
	return iCount;
}


bool CvTeam::hasMetAnyCiv(bool bIgnoreMinors) const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getID() && isHasMet((TeamTypes)iI)
		&& (!bIgnoreMinors || !GET_TEAM((TeamTypes)iI).isMinorCiv()))
		{
			return true;
		}
	}
	return false;
}


int CvTeam::getDefensivePactCount(TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getID() && isDefensivePact((TeamTypes)iI))
		{
			if (NO_TEAM == eTeam || GET_TEAM(eTeam).isHasMet((TeamTypes)iI))
			{
				iCount++;
			}
		}
	}
	return iCount;
}


int CvTeam::getVassalCount(TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getID()
		&& GET_TEAM((TeamTypes)iI).isVassal(getID())
		// Only count vassals that eTeam knows about, if valid eTeam.
		&& (NO_TEAM == eTeam || GET_TEAM(eTeam).isHasMet((TeamTypes)iI)))
		{
			iCount++;
		}
	}
	return iCount;
}

bool CvTeam::isAVassal() const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getID() && isVassal((TeamTypes)iI))
		{
			FAssert(iI != getID());
			return true;
		}
	}
	return false;
}

bool CvTeam::canVassalRevolt(TeamTypes eMaster) const
{
	FAssert(NO_TEAM != eMaster);

	if (isVassal(eMaster))
	{
		if (100 * getTotalLand(false) < GC.getVASSAL_REVOLT_OWN_LOSSES_FACTOR() * getVassalPower())
		{
			return true;
		}

		if (100 * GET_TEAM(eMaster).getTotalLand() < GC.getVASSAL_REVOLT_MASTER_LOSSES_FACTOR() * getMasterPower())
		{
			return true;
		}
	}

	if (GC.getFREE_VASSAL_LAND_PERCENT() < 0
	|| 100 * getTotalLand(false) < GET_TEAM(eMaster).getTotalLand(false) * GC.getFREE_VASSAL_LAND_PERCENT())
	{
		return false;
	}

	if (GC.getFREE_VASSAL_POPULATION_PERCENT() < 0
	|| 100 * getTotalPopulation(false) < GET_TEAM(eMaster).getTotalPopulation(false) * GC.getFREE_VASSAL_POPULATION_PERCENT())
	{
		return false;
	}
	return true;
}


int CvTeam::getCurrentMasterPower(bool bIncludeVassals) const
{
	PROFILE_EXTRA_FUNC();
	if (isAVassal())
	{
		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getID() && isVassal((TeamTypes)iI))
			{
				return GET_TEAM((TeamTypes)iI).getPower(bIncludeVassals);
			}
		}
	}
	FErrorMsg("error"); // Should never get here
	return 0;
}

bool CvTeam::isMasterPlanningLandWar(const CvArea* pArea) const
{
	PROFILE_EXTRA_FUNC();
	if (!isAVassal())
	{
		return false;
	}
	if (pArea->getAreaAIType(getID()) == AREAAI_OFFENSIVE
	|| pArea->getAreaAIType(getID()) == AREAAI_DEFENSIVE
	|| pArea->getAreaAIType(getID()) == AREAAI_MASSING)
	{
		return true;
	}
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (isVassal((TeamTypes)iI))
		{
			if (GET_TEAM((TeamTypes)iI).hasWarPlan(true))
			{
				if (pArea->getAreaAIType((TeamTypes)iI) == AREAAI_OFFENSIVE
				|| pArea->getAreaAIType((TeamTypes)iI) == AREAAI_DEFENSIVE
				|| pArea->getAreaAIType((TeamTypes)iI) == AREAAI_MASSING)
				{
					return true;
				}
				if (pArea->getAreaAIType((TeamTypes)iI) == AREAAI_NEUTRAL
				// And master has no presence here
				&& pArea->getNumCities() - countNumCitiesByArea(pArea) > 2)
				{
					return (GC.getGame().getSorenRandNum((isCapitulated() ? 6 : 4), "Vassal land war") == 0);
				}
			}
			else if (GET_TEAM((TeamTypes)iI).isHuman() && GC.getBBAI_HUMAN_VASSAL_WAR_BUILD()
			&& pArea->getNumCities() - countNumCitiesByArea(pArea) - GET_TEAM((TeamTypes)iI).countNumCitiesByArea(pArea) > 2)
			{
				return GC.getGame().getSorenRandNum(4, "Vassal land war") == 0;
			}
			return false;
		}
	}
	return false;
}

bool CvTeam::isMasterPlanningSeaWar(const CvArea* pArea) const
{
	PROFILE_EXTRA_FUNC();
	if (!isAVassal())
	{
		return false;
	}
	if (pArea->getAreaAIType(getID()) == AREAAI_ASSAULT
	|| pArea->getAreaAIType(getID()) == AREAAI_ASSAULT_ASSIST
	|| pArea->getAreaAIType(getID()) == AREAAI_ASSAULT_MASSING)
	{
		return true;
	}
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (isVassal((TeamTypes)iI))
		{
			if (GET_TEAM((TeamTypes)iI).hasWarPlan(true))
			{
				if (pArea->getAreaAIType((TeamTypes)iI) == AREAAI_ASSAULT
				|| pArea->getAreaAIType((TeamTypes)iI) == AREAAI_ASSAULT_ASSIST
				|| pArea->getAreaAIType((TeamTypes)iI) == AREAAI_ASSAULT_MASSING)
				{
					return (GC.getGame().getSorenRandNum((isCapitulated() ? 3 : 2), "Vassal sea war") == 0);
				}
				if (pArea->getAreaAIType((TeamTypes)iI) == AREAAI_NEUTRAL)
				{
					return false; // Master has no presence here
				}
			}
			else if (GET_TEAM((TeamTypes)iI).isHuman())
			{
				return false;
			}
			break;
		}
	}
	return false;
}


int CvTeam::getUnitMaking(UnitTypes eIndex) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getUnitMaking(eIndex);
		}
	}
	return iCount;
}


int CvTeam::getUnitCountPlusMaking(UnitTypes eIndex) const
{
	return getUnitCount(eIndex) + getUnitMaking(eIndex);
}


int CvTeam::getBuildingMaking(BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getBuildingMaking(eBuilding);
		}
	}
	return iCount;
}


int CvTeam::getBuildingCountPlusMaking(BuildingTypes eIndex) const
{
	return (getBuildingCount(eIndex) + getBuildingMaking(eIndex));
}


int CvTeam::getHasReligionCount(ReligionTypes eReligion) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getHasReligionCount(eReligion);
		}
	}
	return iCount;
}


int CvTeam::getHasCorporationCount(CorporationTypes eCorporation) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getHasCorporationCount(eCorporation);
		}
	}
	return iCount;
}


uint64_t CvTeam::countTotalCulture() const
{
	PROFILE_EXTRA_FUNC();
	uint64_t iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getCulture();
		}
	}
	return iCount;
}


int CvTeam::countNumUnitsByArea(const CvArea* pArea) const
{
	PROFILE_FUNC();

	int iCount = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iCount += pArea->getUnitsPerPlayer((PlayerTypes)iI);
		}
	}
	return iCount;
}


int CvTeam::countNumCitiesByArea(const CvArea* pArea) const
{
	PROFILE_FUNC();

	int iCount = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iCount += pArea->getCitiesPerPlayer((PlayerTypes)iI);
		}
	}
	return iCount;
}


int CvTeam::countTotalPopulationByArea(const CvArea* pArea) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iCount += pArea->getPopulationPerPlayer((PlayerTypes)iI);
		}
	}
	return iCount;
}


int CvTeam::countPowerByArea(const CvArea* pArea) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iCount += pArea->getPower((PlayerTypes)iI);
		}
	}
	return iCount;
}


int CvTeam::countEnemyPowerByArea(const CvArea* pArea) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID(), false)
		&& AI_getWarPlan(GET_PLAYER((PlayerTypes)iI).getTeam()) != NO_WARPLAN)
		{
			iCount += pArea->getPower((PlayerTypes)iI);
		}
	}
	return iCount;
}

int CvTeam::countEnemyPopulationByArea(const CvArea* pArea) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID(), false))
		{
			const WarPlanTypes eWarPlan = AI_getWarPlan(GET_PLAYER((PlayerTypes)iI).getTeam());
			if (eWarPlan != NO_WARPLAN)
			{
				int iContribution = pArea->getPopulationPerPlayer((PlayerTypes)iI);

				if (eWarPlan == WARPLAN_LIMITED && (isMinorCiv() || GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isMinorCiv()))
				{
					iContribution /= 2;
				}
				iCount += iContribution;
			}
		}
	}
	return iCount;
}

int CvTeam::countNumAIUnitsByArea(const CvArea* pArea, UnitAITypes eUnitAI) const
{
	PROFILE_FUNC();

	int iCount = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iCount += pArea->getNumAIUnits(((PlayerTypes)iI), eUnitAI);
		}
	}
	return iCount;
}

int CvTeam::countEnemyDangerByArea(const CvArea* pArea, TeamTypes eEnemyTeam) const
{
	PROFILE_FUNC();

	int iCount = 0;
	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot != NULL && pLoopPlot->area() == pArea && pLoopPlot->getTeam() == getID())
		{
			//	Koshling - don't count animals - they are not actively attacking us so they shouldn't trigger things like a defensive stance
			iCount += pLoopPlot->plotCount(PUF_canDefendEnemyNoAnimal, getLeaderID(), 0, NULL, NO_PLAYER, eEnemyTeam, PUF_isVisible, getLeaderID());
		}
	}
	return iCount;
}


// This function needs to be converted to return an uint64_t.
int CvTeam::getResearchCost(TechTypes eTech) const
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(eTech != NO_TECH, "Tech is not assigned a valid value");

	const int iInitialCost = GC.getTechInfo(eTech).getResearchCost();
	if (iInitialCost < 0)
	{
		return -1;
	}
	uint64_t iCost = 100 * iInitialCost;

	int iBeelineStingsTechCostModifier = 0;
	if (GC.getGame().isOption(GAMEOPTION_TECH_BEELINE_STINGS))
	{
		const int iTechEra = GC.getTechInfo(eTech).getEra();
		int iPlayerEra = MAX_INT;
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iPlayerEra = (int)GET_PLAYER((PlayerTypes)iI).getCurrentEra();
				break;
			}
		}
		FAssertMsg(iPlayerEra != MAX_INT, "No player found on team!");

		if (iTechEra < iPlayerEra)
		{
			for (int iI = 0; iI < GC.getNumEraInfos(); iI++)
			{
				if (iI >= iTechEra && iI < iPlayerEra)
				{
					iBeelineStingsTechCostModifier += GC.getEraInfo((EraTypes)iI).getBeelineStingsTechCostModifier();
					//just need to add the tag iBeelineStingsModifier to Era Infos.
				}
				else if (iI >= iPlayerEra)
				{
					break;
				}
			}
		}
	}
	iCost *= GC.getTECH_COST_MODIFIER();
	iCost /= 100;

	iCost *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();
	iCost /= 100;

	iCost *= GC.getEraInfo((EraTypes)GC.getTechInfo(eTech).getEra()).getResearchPercent();
	iCost /= 100;

	iCost *= std::max(0, GC.getTECH_COST_EXTRA_TEAM_MEMBER_MODIFIER() * getNumMembers());
	iCost /= 100;

	int iMod = iBeelineStingsTechCostModifier;

	if (!isNPC() && !isHuman(true))
	{
		iMod =
			(
				GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIResearchPercent() - 100
				+
				GC.getHandicapInfo(GC.getGame().getHandicapType()).getAIPerEraModifier() * GET_PLAYER(getLeaderID()).getCurrentEra()
			);
	}
	if (GC.getGame().isOption(GAMEOPTION_TECH_UPSCALED_COSTS))
	{
		iMod += GC.getUPSCALED_RESEARCH_COST_MODIFIER();
	}
	if (iMod != 0)
	{
		iCost = getModifiedIntValue64(iCost, iMod);
	}
	iCost /= 100;

	return std::max(1, iCost < MAX_INT ? static_cast<int>(iCost) : MAX_INT);
}


int CvTeam::getResearchLeft(TechTypes eTech) const
{
	return std::max(0, getResearchCost(eTech) - getResearchProgress(eTech));
}


bool CvTeam::hasHolyCity(ReligionTypes eReligion) const
{
	FAssertMsg(eReligion != NO_RELIGION, "Religion is not assigned a valid value");

	const CvCity* pHolyCity = GC.getGame().getHolyCity(eReligion);

	if (pHolyCity != NULL)
	{
		return (pHolyCity->getTeam() == getID());
	}

	return false;
}


bool CvTeam::hasHeadquarters(CorporationTypes eCorporation) const
{
	FAssertMsg(eCorporation != NO_CORPORATION, "Corporation is not assigned a valid value");

	const CvCity* pHeadquarters = GC.getGame().getHeadquarters(eCorporation);

	if (pHeadquarters != NULL)
	{
		return (pHeadquarters->getTeam() == getID());
	}

	return false;
}

bool CvTeam::hasBonus(BonusTypes eBonus) const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()) && GET_PLAYER((PlayerTypes)iI).hasBonus(eBonus))
		{
			return true;
		}
	}
	return false;
}

bool CvTeam::isBonusObsolete(BonusTypes eBonus) const
{
	TechTypes eObsoleteTech = (TechTypes)GC.getBonusInfo(eBonus).getTechObsolete();
	if (eObsoleteTech != NO_TECH && isHasTech(eObsoleteTech))
	{
		return true;
	}
	return false;
}


bool CvTeam::isHuman(const bool bCountDisabledHuman) const
{
	PROFILE_FUNC();

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID())
		&& GET_PLAYER((PlayerTypes)iI).isHumanPlayer(bCountDisabledHuman))
		{
			return true;
		}
	}
	return false;
}

bool CvTeam::isNPC() const
{
	return getID() >= MAX_PC_TEAMS;
}

bool CvTeam::isHominid() const
{
	return getID() == BARBARIAN_TEAM || getID() == NEANDERTHAL_TEAM;
}


bool CvTeam::isMinorCiv() const
{
	PROFILE_EXTRA_FUNC();
	bool bValid = false;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			if (!GET_PLAYER((PlayerTypes)iI).isMinorCiv())
			{
				return false;
			}
			bValid = true;
		}
	}
	return bValid;
}

void CvTeam::setIsMinorCiv(bool bNewValue)
{
	PROFILE_EXTRA_FUNC();

	const TeamTypes eTeam = getID();
	if (bNewValue != isMinorCiv())
	{
		if (isAlive())
		{
			if (bNewValue)
			{
				logging::logMsg("C2C.log", "Switching team %d to minor\n", eTeam);
			}
			else
			{
				logging::logMsg("C2C.log", "Switching minor team %d to full\n", eTeam);
			}
		}
		else if (bNewValue)
		{
			logging::logMsg("C2C.log", "Setting non-alive team %d to minor\n", eTeam);
		}
		else
		{
			logging::logMsg("C2C.log", "Setting non-alive minor team %d to full\n", eTeam);
		}

		// Convert all team members
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
			{
				GC.getInitCore().setMinorNationCiv((PlayerTypes)iI, bNewValue);
			}
		}

		if (!isAlive())
		{
			return;
		}

		if (bNewValue)
		{
			declareWarAsMinor(); // Declare war on all outside teams
		}
		else
		{
			// Have to check hasMet status first because of vassal states ... dow on master causes hasMet for vassal
			bool abHasMet[MAX_PC_TEAMS];

			for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
			{
				abHasMet[iI] = isHasMet((TeamTypes)iI);
			}

			TeamTypes eBarbCivVictim = NO_TEAM;
			{
				int iMaxVal = 0;
				for (int iJ = 0; iJ < MAX_PC_TEAMS; iJ++)
				{
					const TeamTypes eTeamX = static_cast<TeamTypes>(iJ);
					if (eTeamX != eTeam && abHasMet[iJ] && GET_TEAM(eTeamX).isAlive() && !GET_TEAM(eTeamX).isMinorCiv())
					{
						const int iValue = GET_TEAM(eTeam).AI_getBarbarianCivWarVal(eTeamX, 16);
						if (iValue > iMaxVal)
						{
							logging::logMsg("C2C.log", "    BarbCiv team %d is considering declaring war against victim Team %d\n", eTeam, iJ);
							iMaxVal = iValue;
							eBarbCivVictim = eTeamX;
						}
					}
				}
				logging::logMsg("C2C.log", "    BarbCiv team %d will declare war against victim Team %d\n", eTeam, eBarbCivVictim);
			}
			// Keep war againt some of those this team has met
			for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
			{
				const TeamTypes eTeamX = static_cast<TeamTypes>(iI);
				if (eTeamX != eTeam && GET_TEAM(eTeamX).isAlive() && !GET_TEAM(eTeamX).isMinorCiv())
				{
					if (!isAtWar(eTeamX))
					{
						FErrorMsg("!isAtWar(eTeamX); Shouldn't happen here");
						continue;
					}
					if (abHasMet[iI])
					{
						if (eBarbCivVictim == eTeamX)
						{
							AI_setWarPlan(eBarbCivVictim, WARPLAN_TOTAL, true);
							logging::logMsg("C2C.log", "    Team %d is the victim of ex-minor\n", iI);
							continue;
						}

						if (GET_TEAM(eTeam).AI_minorPeaceOut(eTeamX))
						{
							makePeace(eTeamX, false);

							// Toffer: Guess this is done to trigger the first meeting diplomacy pop-up.
							setHasMet(eTeamX, false);
							GET_TEAM(eTeamX).setHasMet(eTeam, false);
						}
						else
						{
							AI_setWarPlan(eTeamX, WARPLAN_LIMITED, true);
							GET_TEAM(eTeamX).AI_setWarPlan(eTeam, WARPLAN_LIMITED, true);
							logging::logMsg("C2C.log", "    ex-minor decides to keep limited war against Team %d\n", iI);
						}
					}
					else
					{
						makePeace(eTeamX, false);
						setHasMet(eTeamX, false);
						GET_TEAM(eTeamX).setHasMet(eTeam, false);
					}
				}
			}
		}
		// Update graphics to reflect changes
		gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(UnitInfo_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Flag_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Foreign_Screen_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(GlobeInfo_DIRTY_BIT, true);
	}
	return;
}

void CvTeam::declareWarAsMinor()
{
	PROFILE_EXTRA_FUNC();
	if (isAlive())
	{
		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			// Declare war on everyone except barbs, barbs always do their own declaring
			if (iI != getID() && !isAtWar((TeamTypes)iI))
			{
				bool bHasMet = isHasMet((TeamTypes)iI);

				declareWar((TeamTypes)iI, false, WARPLAN_LIMITED);
				if (!bHasMet)
				{
					// Declaring war creates contacts, remove those the player didn't have already
					setHasMet((TeamTypes)iI, false);
					GET_TEAM((TeamTypes)iI).setHasMet(getID(), false);
				}
			}

			if (isAtWar((TeamTypes)iI))
			{
				AI_setWarPlan((TeamTypes)iI, WARPLAN_LIMITED, true);
				GET_TEAM((TeamTypes)iI).AI_setWarPlan(getID(), WARPLAN_LIMITED, true);
			}
		}
	}
}


PlayerTypes CvTeam::getLeaderID() const
{
	PROFILE_EXTRA_FUNC();
	for (int iAlivePlayerId = 0; iAlivePlayerId < MAX_PLAYERS; iAlivePlayerId++)
	{
		if (GET_PLAYER((PlayerTypes)iAlivePlayerId).isAliveAndTeam(getID()))
		{
			return (PlayerTypes)iAlivePlayerId;
		}
	}
	for (int iPlayerId = 0; iPlayerId < MAX_PLAYERS; iPlayerId++)
	{
		if (GET_PLAYER((PlayerTypes)iPlayerId).getTeam() == getID())
		{
			return (PlayerTypes)iPlayerId;
		}
	}
	return NO_PLAYER;
}


PlayerTypes CvTeam::getSecretaryID() const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()) && GET_PLAYER((PlayerTypes)iI).isHumanPlayer())
		{
			return (PlayerTypes)iI;
		}
	}
	return getLeaderID();
}


HandicapTypes CvTeam::getHandicapType() const
{
	PROFILE_EXTRA_FUNC();
	int iGameHandicap = 0;
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iGameHandicap += GET_PLAYER((PlayerTypes)iI).getHandicapType();
			iCount++;
		}
	}

	if (iCount > 0)
	{
		FAssertMsg((iGameHandicap / iCount) >= 0, "(iGameHandicap / iCount) is expected to be non-negative (invalid Index)");
		return ((HandicapTypes)(iGameHandicap / iCount));
	}
	return (HandicapTypes)GC.getDefineINT("STANDARD_HANDICAP");
}


CvWString CvTeam::getName() const
{
	PROFILE_EXTRA_FUNC();
	CvWString szBuffer;
	bool bFirst = true;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			setListHelp(szBuffer, L"", GET_PLAYER((PlayerTypes)iI).getName(), L"/", bFirst);
			bFirst = false;
		}
	}
	return szBuffer;
}


int CvTeam::getNumMembers() const
{
	return m_iNumMembers;
}


void CvTeam::changeNumMembers(int iChange)
{
	m_iNumMembers += iChange;
	FASSERT_NOT_NEGATIVE(m_iNumMembers);
}


int CvTeam::getAliveCount() const
{
	return m_iAliveCount;
}

bool CvTeam::isAlive() const
{
	return m_iAliveCount > 0;
}

void CvTeam::changeAliveCount(int iChange)
{
	PROFILE_EXTRA_FUNC();
	m_iAliveCount += iChange;
	FASSERT_NOT_NEGATIVE(m_iAliveCount);

	// free vassals
	if (0 == m_iAliveCount)
	{
		for (int iTeam = 0; iTeam < MAX_TEAMS; iTeam++)
		{
			if (iTeam != getID())
			{
				CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iTeam);
				if (kLoopTeam.isAlive() && kLoopTeam.isVassal(getID()))
				{
					kLoopTeam.setVassal(getID(), false, false);
				}
			}
		}
	}
}


int CvTeam::isEverAlive() const
{
	return m_iEverAliveCount > 0;
}

void CvTeam::changeEverAliveCount(int iChange)
{
	m_iEverAliveCount += iChange;
	FASSERT_NOT_NEGATIVE(m_iEverAliveCount);
}


int CvTeam::getNumCities() const
{
	return m_iNumCities;
}

void CvTeam::changeNumCities(int iChange)
{
	m_iNumCities += iChange;
	FASSERT_NOT_NEGATIVE(m_iNumCities);
}


int CvTeam::getTotalPopulation(bool bCheckVassals) const
{
	PROFILE_EXTRA_FUNC();
	if (isNPC() || !bCheckVassals)
	{
		return m_iTotalPopulation;
	}
	int iVassalPop = 0;

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && iI != getID() && GET_TEAM((TeamTypes)iI).isVassal(getID()))
		{
			iVassalPop += GET_TEAM((TeamTypes)iI).getTotalPopulation(false) / 2;
		}
	}
	return m_iTotalPopulation + iVassalPop;
}

void CvTeam::changeTotalPopulation(int iChange)
{
	m_iTotalPopulation += iChange;
	FASSERT_NOT_NEGATIVE(getTotalPopulation());
}


int CvTeam::getTotalLand(bool bCheckVassals) const
{
	PROFILE_EXTRA_FUNC();
	if (isNPC() || !bCheckVassals)
	{
		return m_iTotalLand;
	}
	int iVassalLand = 0;

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if ((TeamTypes)iI != getID())
		{
			CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
			if (kLoopTeam.isAlive() && kLoopTeam.isVassal(getID()))
			{
				iVassalLand += kLoopTeam.getTotalLand(false) / 2;
			}
		}
	}
	return m_iTotalLand + iVassalLand;
}

void CvTeam::changeTotalLand(int iChange)
{
	m_iTotalLand += iChange;
	FASSERT_NOT_NEGATIVE(getTotalLand());
}


int CvTeam::getNukeInterception() const
{
	return m_iNukeInterception;
}

void CvTeam::changeNukeInterception(int iChange)
{
	m_iNukeInterception += iChange;
	FASSERT_NOT_NEGATIVE(m_iNukeInterception);
}


int CvTeam::getForceTeamVoteEligibilityCount(VoteSourceTypes eVoteSource) const
{
	return m_aiForceTeamVoteEligibilityCount[eVoteSource];
}


bool CvTeam::isForceTeamVoteEligible(VoteSourceTypes eVoteSource) const
{
	return getForceTeamVoteEligibilityCount(eVoteSource) > 0 && !isMinorCiv();
}


void CvTeam::changeForceTeamVoteEligibilityCount(VoteSourceTypes eVoteSource, int iChange)
{
	m_aiForceTeamVoteEligibilityCount[eVoteSource] += iChange;
	FASSERT_NOT_NEGATIVE(getForceTeamVoteEligibilityCount(eVoteSource));
}


int CvTeam::getExtraWaterSeeFromCount() const
{
	return m_iExtraWaterSeeFromCount;
}

bool CvTeam::isExtraWaterSeeFrom() const
{
	return m_iExtraWaterSeeFromCount > 0;
}

void CvTeam::changeExtraWaterSeeFromCount(int iChange)
{
	if (iChange != 0)
	{
		GC.getMap().updateSight(false);

		m_iExtraWaterSeeFromCount += iChange;
		FASSERT_NOT_NEGATIVE(m_iExtraWaterSeeFromCount);

		GC.getMap().updateSight(true);
	}
}


int CvTeam::getMapTradingCount() const
{
	return m_iMapTradingCount;
}

bool CvTeam::isMapTrading()	const
{
	return m_iMapTradingCount > 0;
}

void CvTeam::changeMapTradingCount(int iChange)
{
	m_iMapTradingCount += iChange;
	FASSERT_NOT_NEGATIVE(m_iMapTradingCount);
}


int CvTeam::getTechTradingCount() const
{
	return m_iTechTradingCount;
}

bool CvTeam::isTechTrading() const
{
	return m_iTechTradingCount > 0;
}

void CvTeam::changeTechTradingCount(int iChange)
{
	m_iTechTradingCount += iChange;
	FASSERT_NOT_NEGATIVE(m_iTechTradingCount);
}


int CvTeam::getGoldTradingCount() const
{
	return m_iGoldTradingCount;
}

bool CvTeam::isGoldTrading() const
{
	return m_iGoldTradingCount > 0;
}

void CvTeam::changeGoldTradingCount(int iChange)
{
	m_iGoldTradingCount += iChange;
	FASSERT_NOT_NEGATIVE(m_iGoldTradingCount);
}


int CvTeam::getOpenBordersTradingCount() const
{
	return m_iOpenBordersTradingCount;
}

bool CvTeam::isOpenBordersTrading() const
{
	return m_iOpenBordersTradingCount > 0;
}

void CvTeam::changeOpenBordersTradingCount(int iChange)
{
	m_iOpenBordersTradingCount += iChange;
	FASSERT_NOT_NEGATIVE(m_iOpenBordersTradingCount);
}


int CvTeam::getDefensivePactTradingCount() const
{
	return m_iDefensivePactTradingCount;
}

bool CvTeam::isDefensivePactTrading() const
{
	return m_iDefensivePactTradingCount > 0;
}

void CvTeam::changeDefensivePactTradingCount(int iChange)
{
	m_iDefensivePactTradingCount += iChange;
	FASSERT_NOT_NEGATIVE(m_iDefensivePactTradingCount);
}


int CvTeam::getPermanentAllianceTradingCount() const
{
	return m_iPermanentAllianceTradingCount;
}

bool CvTeam::isPermanentAllianceTrading() const
{
	return GC.getGame().isOption(GAMEOPTION_ENABLE_PERMANENT_ALLIANCES) && m_iPermanentAllianceTradingCount > 0;
}

void CvTeam::changePermanentAllianceTradingCount(int iChange)
{
	m_iPermanentAllianceTradingCount += iChange;
	FASSERT_NOT_NEGATIVE(m_iPermanentAllianceTradingCount);
}


int CvTeam::getVassalTradingCount() const
{
	return m_iVassalTradingCount;
}

bool CvTeam::isVassalStateTrading() const
{
	return !GC.getGame().isOption(GAMEOPTION_NO_VASSAL_STATES) && m_iVassalTradingCount > 0;
}

void CvTeam::changeVassalTradingCount(int iChange)
{
	m_iVassalTradingCount += iChange;
	FASSERT_NOT_NEGATIVE(m_iVassalTradingCount);
}


int CvTeam::getBridgeBuildingCount() const
{
	return m_iBridgeBuildingCount;
}

bool CvTeam::isBridgeBuilding()	const
{
	return m_iBridgeBuildingCount > 0;
}

void CvTeam::changeBridgeBuildingCount(int iChange)
{
	if (iChange != 0)
	{
		m_iBridgeBuildingCount += iChange;
		FASSERT_NOT_NEGATIVE(m_iBridgeBuildingCount);

		if (GC.IsGraphicsInitialized())
		{
			gDLL->getEngineIFace()->MarkBridgesDirty();
		}
	}
}


int CvTeam::getIrrigationCount() const
{
	return m_iIrrigationCount;
}

bool CvTeam::isIrrigation() const
{
	return m_iIrrigationCount > 0;
}

void CvTeam::changeIrrigationCount(int iChange)
{
	if (iChange != 0)
	{
		m_iIrrigationCount += iChange;
		FASSERT_NOT_NEGATIVE(m_iIrrigationCount);

		GC.getMap().updateIrrigated();
	}
}


int CvTeam::getIgnoreIrrigationCount() const
{
	return m_iIgnoreIrrigationCount;
}

bool CvTeam::isIgnoreIrrigation() const
{
	return m_iIgnoreIrrigationCount > 0;
}

void CvTeam::changeIgnoreIrrigationCount(int iChange)
{
	m_iIgnoreIrrigationCount += iChange;
	FASSERT_NOT_NEGATIVE(m_iIgnoreIrrigationCount);
}


int CvTeam::getWaterWorkCount() const
{
	return m_iWaterWorkCount;
}

bool CvTeam::isWaterWork() const
{
	return m_iWaterWorkCount > 0;
}

void CvTeam::changeWaterWorkCount(int iChange)
{
	if (iChange != 0)
	{
		m_iWaterWorkCount += iChange;
		FASSERT_NOT_NEGATIVE(m_iWaterWorkCount);

		AI_makeAssignWorkDirty();
	}
}


int CvTeam::getVassalPower() const
{
	return m_iVassalPower;
}

void CvTeam::setVassalPower(int iPower)
{
	m_iVassalPower = iPower;
}

int CvTeam::getMasterPower() const
{
	return m_iMasterPower;
}

void CvTeam::setMasterPower(int iPower)
{
	m_iMasterPower = iPower;
}


int CvTeam::getEnemyWarWearinessModifier() const
{
	PROFILE_EXTRA_FUNC();
	int iEnemyWarWearinessTotal = m_iEnemyWarWearinessModifier;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			iEnemyWarWearinessTotal += GET_PLAYER((PlayerTypes)iI).getNationalEnemyWarWearinessModifier();
		}
	}
	return iEnemyWarWearinessTotal;
}

void CvTeam::changeEnemyWarWearinessModifier(int iChange)
{
	m_iEnemyWarWearinessModifier += iChange;
}


bool CvTeam::isMapCentering() const
{
	return m_bMapCentering;
}

void CvTeam::setMapCentering(bool bNewValue)
{
	if (m_bMapCentering != bNewValue)
	{
		m_bMapCentering = bNewValue;

		if (getID() == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(MinimapSection_DIRTY_BIT, true);
		}
	}
}


TeamTypes CvTeam::getID() const
{
	return m_eID;
}


int CvTeam::getStolenVisibilityTimer(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	return m_aiStolenVisibilityTimer[eIndex];
}

bool CvTeam::isStolenVisibility(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	return (getStolenVisibilityTimer(eIndex) > 0);
}

void CvTeam::setStolenVisibilityTimer(TeamTypes eIndex, int iNewValue)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	if (getStolenVisibilityTimer(eIndex) != iNewValue)
	{
		const bool bOldStolenVisibility = isStolenVisibility(eIndex);

		m_aiStolenVisibilityTimer[eIndex] = iNewValue;
		FASSERT_NOT_NEGATIVE(getStolenVisibilityTimer(eIndex));

		if (bOldStolenVisibility != isStolenVisibility(eIndex))
		{
			for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
			{
				CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

				if (pLoopPlot->isVisible(eIndex, false))
				{
					pLoopPlot->changeStolenVisibilityCount(getID(), ((isStolenVisibility(eIndex)) ? 1 : -1));
				}
			}
		}
	}
}

void CvTeam::changeStolenVisibilityTimer(TeamTypes eIndex, int iChange)
{
	setStolenVisibilityTimer(eIndex, (getStolenVisibilityTimer(eIndex) + iChange));
}


int CvTeam::getWarWeariness(TeamTypes eIndex) const
{
	return getWarWearinessTimes100(eIndex) / 100;
}

int CvTeam::getWarWearinessTimes100(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_aiWarWearinessTimes100[eIndex];
}

void CvTeam::setWarWeariness(TeamTypes eIndex, int iNewValue)
{
	setWarWearinessTimes100(eIndex, 100 * iNewValue);
}

void CvTeam::setWarWearinessTimes100(TeamTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	m_aiWarWearinessTimes100[eIndex] = std::max(0, iNewValue);
}

void CvTeam::changeWarWeariness(TeamTypes eIndex, int iChange)
{
	changeWarWearinessTimes100(eIndex, 100 * iChange);
}

void CvTeam::changeWarWearinessTimes100(TeamTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	setWarWearinessTimes100(eIndex, getWarWearinessTimes100(eIndex) + iChange);
}

void CvTeam::changeWarWeariness(TeamTypes eOtherTeam, const CvPlot& kPlot, int iFactor)
{
	changeWarWearinessTimes100(eOtherTeam, kPlot, iFactor * 100);
}

void CvTeam::changeWarWearinessTimes100(TeamTypes eOtherTeam, const CvPlot& kPlot, int iFactor)
{
	PROFILE_FUNC();

	const int iOurCulture = kPlot.countFriendlyCulture(getID());
	const int iTheirCulture = kPlot.countFriendlyCulture(eOtherTeam);

	int iRatio = 100;
	if (0 != iOurCulture + iTheirCulture)
	{
		iRatio = 100 * iTheirCulture / (iOurCulture + iTheirCulture);
	}
	// Rebels don't feel as if they are fighting on foreign soil
	if (isRebelAgainst(eOtherTeam))
	{
		iRatio = std::min(40, iRatio);
	}
	else if (isRebel())
	{
		iRatio = std::min(60, iRatio);
	}
	changeWarWearinessTimes100(eOtherTeam, iRatio * iFactor);
}


bool CvTeam::isTechShare(int iIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, iIndex);
	return m_aiTechShareCount[iIndex] > 0;
}

void CvTeam::changeTechShareCount(int iIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, iIndex);

	if (iChange != 0)
	{
		m_aiTechShareCount[iIndex] += iChange;
		FASSERT_NOT_NEGATIVE(m_aiTechShareCount[iIndex]);

		if (isTechShare(iIndex))
		{
			updateTechShare();
		}
	}
}


int CvTeam::getCommerceFlexibleCount(CommerceTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);
	return m_aiCommerceFlexibleCount[eIndex];
}

bool CvTeam::isCommerceFlexible(CommerceTypes eIndex) const
{
	return getCommerceFlexibleCount(eIndex) > 0;
}

void CvTeam::changeCommerceFlexibleCount(CommerceTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eIndex);

	if (iChange != 0)
	{
		m_aiCommerceFlexibleCount[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(m_aiCommerceFlexibleCount[eIndex]);

		if (getID() == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(PercentButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvTeam::getExtraMoves(DomainTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_DOMAIN_TYPES, eIndex);
	return m_aiExtraMoves[eIndex];
}

void CvTeam::changeExtraMoves(DomainTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, NUM_DOMAIN_TYPES, eIndex);
	m_aiExtraMoves[eIndex] += iChange;
	FASSERT_NOT_NEGATIVE(m_aiExtraMoves[eIndex]);
}


bool CvTeam::isHasMet(TeamTypes eIndex)	const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	//FAssert((eIndex != getID()) || m_abHasMet[eIndex]);
	return m_abHasMet[eIndex];
}

// For minor civs
void CvTeam::setHasMet(TeamTypes eIndex, bool bNewValue)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	if (bNewValue != isHasMet(eIndex)) m_abHasMet[eIndex] = bNewValue;
}

void CvTeam::makeHasMet(TeamTypes eIndex, bool bNewDiplo)
{
	PROFILE_EXTRA_FUNC();
	CvDiploParameters* pDiplo;

	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	if (!isHasMet(eIndex))
	{
		m_abHasMet[eIndex] = true;

		if (!isNPC() && !GET_TEAM(eIndex).isNPC())
		{
			updateTechShare();

			if (GET_TEAM(eIndex).isHuman())
			{
				for (int iI = 0; iI < MAX_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID())
					&& !GET_PLAYER((PlayerTypes)iI).isHumanPlayer())
					{
						GET_PLAYER((PlayerTypes)iI).clearResearchQueue();
						GET_PLAYER((PlayerTypes)iI).AI_makeProductionDirty();
					}
				}
			}
		}

		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive()
			&& (GET_TEAM((TeamTypes)iI).isVassal(getID()) || isVassal((TeamTypes)iI)))
			{
				GET_TEAM((TeamTypes)iI).meet(eIndex, bNewDiplo);
			}
		}

		if (getID() == GC.getGame().getActiveTeam() || eIndex == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
			// RevolutionDCM - redraw espionage percent buttons
			// gDLL->getInterfaceIFace()->setDirty(PercentButtons_DIRTY_BIT, true); // Toffer - don't think this is needed.
		}
		// Move reporting to Python before diplo popup to all war declarations on first contact
		// report event to Python, along with some other key state
		CvEventReporter::getInstance().firstContact(getID(), eIndex);

		if (bNewDiplo && GC.getGame().isFinalInitialized() && !gDLL->GetWorldBuilderMode() && !isHuman() && !isAtWar(eIndex))
		{
			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(eIndex)
				&& GET_PLAYER(getLeaderID()).canContact((PlayerTypes)iI)
				&& GET_PLAYER((PlayerTypes)iI).isHumanPlayer())
				{
					pDiplo = new CvDiploParameters(getLeaderID());
					FAssertMsg(pDiplo != NULL, "pDiplo must be valid");
					pDiplo->setDiploComment(GC.getAI_DIPLOCOMMENT_FIRST_CONTACT());
					pDiplo->setAIContact(true);
					gDLL->beginDiplomacy(pDiplo, (PlayerTypes)iI);
				}
			}
		}
	}
}


bool CvTeam::isAtWar(TeamTypes eIndex) const
{
	if (eIndex == NO_TEAM)
	{
		return false;
	}
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_abAtWar[eIndex];
}

bool CvTeam::isAtWar(const bool bCountMinors) const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		const TeamTypes eTeamX = (TeamTypes)iI;

		if (isAtWar(eTeamX) && GET_TEAM(eTeamX).isAlive()
		&& (bCountMinors || !GET_TEAM(eTeamX).isMinorCiv()))
		{
			FAssert(eTeamX != getID());
			FAssert(!AI_isSneakAttackPreparing(eTeamX));
			return true;
		}
	}
	return false;
}


void CvTeam::setAtWar(TeamTypes eIndex, bool bNewValue)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	if (m_abAtWar[eIndex] != bNewValue)
	{
		// Koshling - have seen game states with team flagged as at war with themslves.
		// No idea how this arises but we can at least prevent it here as a fallback.
		if (!bNewValue || eIndex != getID())
		{
			m_abAtWar[eIndex] = bNewValue;

			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
				{
					GET_PLAYER((PlayerTypes)iI).AI_noteWarStatusChange(eIndex, bNewValue);
				}
			}
		}
		else FErrorMsg("Team trying to mark itself as at war with itself");
	}
}


bool CvTeam::isPermanentWarPeace(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_abPermanentWarPeace[eIndex];
}


void CvTeam::setPermanentWarPeace(TeamTypes eIndex, bool bNewValue)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	m_abPermanentWarPeace[eIndex] = bNewValue;
}

bool CvTeam::isFreeTrade(TeamTypes eIndex) const
{
	if (isAtWar(eIndex) || !isHasMet(eIndex))
	{
		return false;
	}

	if (isOpenBorders(eIndex))
	{
		return true;
	}
	//If the secretary general voted for open markets
	if (GC.getGame().isFreeTrade())
	{
		return true;
	}

	if (isLimitedBorders(eIndex))
	{
		return true;
	}

	if (isFreeTradeAgreement(eIndex))
	{
		return true;
	}

	return false;
}


bool CvTeam::isOpenBorders(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_abOpenBorders[eIndex];
}


void CvTeam::setOpenBorders(TeamTypes eIndex, bool bNewValue)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	if (isOpenBorders(eIndex) != bNewValue)
	{
		const bool bOldFreeTrade = isFreeTrade(eIndex);

		m_abOpenBorders[eIndex] = bNewValue;

		AI_setOpenBordersCounter(eIndex, 0);

		GC.getMap().verifyUnitValidPlot();

		if (getID() == GC.getGame().getActiveTeam() || eIndex == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		}

		if (bOldFreeTrade != isFreeTrade(eIndex))
		{
			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
				{
					GET_PLAYER((PlayerTypes)iI).updateTradeRoutes();
				}
			}
		}
	}
}

bool CvTeam::isDefensivePact(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_abDefensivePact[eIndex];
}


void CvTeam::setDefensivePact(TeamTypes eIndex, bool bNewValue)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	if (isDefensivePact(eIndex) != bNewValue)
	{
		m_abDefensivePact[eIndex] = bNewValue;

		if (getID() == GC.getGame().getActiveTeam() || eIndex == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		}

		if (bNewValue && !GET_TEAM(eIndex).isDefensivePact(getID()))
		{
			const CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYERS_SIGN_DEFENSIVE_PACT", getName().GetCString(), GET_TEAM(eIndex).getName().GetCString());

			GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), szBuffer, -1, -1, GC.getCOLOR_HIGHLIGHT_TEXT());


			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
				if (kPlayer.isAlive() && isHasMet(kPlayer.getTeam()) && GET_TEAM(eIndex).isHasMet(kPlayer.getTeam()))
				{
					AddDLLMessage((PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_WELOVEKING", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_HIGHLIGHT_TEXT());
				}
			}
		}

		// Attitude cache
		for (int iA = 0; iA < MAX_PC_PLAYERS; iA++)
		{
			CvPlayerAI& playerA = GET_PLAYER((PlayerTypes)iA);

			if (playerA.isAlive())
			{
				const TeamTypes eTeamA = playerA.getTeam();

				for (int iB = 0; iB < MAX_PC_PLAYERS; iB++)
				{
					CvPlayerAI& playerB = GET_PLAYER((PlayerTypes)iB);

					if (playerB.isAliveAndTeam(eTeamA, false) && (playerB.getTeam() == getID() || playerB.getTeam() == eIndex))
					{
						playerA.AI_invalidateAttitudeCache((PlayerTypes)iB);
						playerB.AI_invalidateAttitudeCache((PlayerTypes)iA);
					}
				}
			}
		}
	}
}


bool CvTeam::isForcePeace(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_abForcePeace[eIndex];
}


void CvTeam::setForcePeace(TeamTypes eIndex, bool bNewValue)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	m_abForcePeace[eIndex] = bNewValue;

	if (isForcePeace(eIndex))
	{
		if (AI_isSneakAttackPreparing(eIndex))
		{
			AI_setWarPlan(eIndex, NO_WARPLAN);
		}
		for (int iTeam = 0; iTeam < MAX_TEAMS; ++iTeam)
		{
			if (GET_TEAM((TeamTypes)iTeam).isVassal(eIndex)
			&& AI_isSneakAttackPreparing((TeamTypes)iTeam))
			{
				AI_setWarPlan((TeamTypes)iTeam, NO_WARPLAN);
			}
		}
	}
}

bool CvTeam::isVassal(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_abVassal[eIndex];
}

void CvTeam::setVassal(TeamTypes eIndex, bool bNewValue, bool bCapitulated)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	FAssertMsg(!bNewValue || !GET_TEAM(eIndex).isAVassal(), "can't become a vassal of a vassal");

	if (isVassal(eIndex) != bNewValue)
	{
		for (int i = 0; i < MAX_PC_PLAYERS; i++)
		{
			if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(getID()))
			{
				GET_PLAYER((PlayerTypes)i).updateCitySight(false, false);
			}
		}

		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& playerX = GET_PLAYER((PlayerTypes)iPlayer);

			if (playerX.isAliveAndTeam(eIndex))
			{
				foreach_(const CvUnit * unitX, playerX.units())
				{
					const CvPlot* pPlot = unitX->plot();
					if (unitX->getTeam() != pPlot->getTeam() && (pPlot->getTeam() == NO_TEAM || !GET_TEAM(pPlot->getTeam()).isVassal(unitX->getTeam())))
					{
						playerX.changeNumOutsideUnits(-1);
					}
				}
			}
		}

		m_abVassal[eIndex] = bNewValue;

		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& playerX = GET_PLAYER((PlayerTypes)iPlayer);

			if (playerX.isAliveAndTeam(eIndex))
			{
				foreach_(const CvUnit * unitX, playerX.units())
				{
					const CvPlot* pPlot = unitX->plot();
					if (unitX->getTeam() != pPlot->getTeam() && (pPlot->getTeam() == NO_TEAM || !GET_TEAM(pPlot->getTeam()).isVassal(unitX->getTeam())))
					{
						playerX.changeNumOutsideUnits(1);
					}
				}
			}
		}

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(getID()))
			{
				GET_PLAYER((PlayerTypes)i).updateCitySight(true, false);
			}
		}

		for (int i = 0; i < GC.getMap().numPlots(); ++i)
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndex(i);
			if (pLoopPlot && (pLoopPlot->getTeam() == getID() || pLoopPlot->getTeam() == eIndex))
			{
				pLoopPlot->updateCulture(true, false);
			}
		}

		GC.getGame().updatePlotGroups();

		if (isVassal(eIndex))
		{
			m_bCapitulated = bCapitulated;

			CLLNode<TradeData>* pNode;

			foreach_(CvDeal & kLoopDeal, GC.getGame().deals())
			{
				if (GET_PLAYER(kLoopDeal.getFirstPlayer()).getTeam() == getID() || GET_PLAYER(kLoopDeal.getSecondPlayer()).getTeam() == getID())
				{
					bool bValid = true;

					if (kLoopDeal.getFirstTrades() != NULL)
					{
						for (pNode = kLoopDeal.getFirstTrades()->head(); pNode; pNode = kLoopDeal.getFirstTrades()->next(pNode))
						{
							if ((pNode->m_data.m_eItemType == TRADE_DEFENSIVE_PACT) ||
								(pNode->m_data.m_eItemType == TRADE_PEACE_TREATY))
							{
								bValid = false;
								break;
							}
						}
					}

					if (bValid && kLoopDeal.getSecondTrades() != NULL)
					{
						for (pNode = kLoopDeal.getSecondTrades()->head(); pNode; pNode = kLoopDeal.getSecondTrades()->next(pNode))
						{
							if ((pNode->m_data.m_eItemType == TRADE_DEFENSIVE_PACT) ||
								(pNode->m_data.m_eItemType == TRADE_PEACE_TREATY))
							{
								bValid = false;
								break;
							}
						}
					}

					if (!bValid)
					{
						kLoopDeal.kill();
					}
				}
			}

			setForcePeace(eIndex, false);
			GET_TEAM(eIndex).setForcePeace(getID(), false);

			// Declare war on teams you should be at war with
			for (int iI = 0; iI < MAX_TEAMS; iI++)
			{
				if (iI != getID() && iI != eIndex && GET_TEAM((TeamTypes)iI).isAlive())
				{
					if (GET_TEAM(eIndex).isHasMet((TeamTypes)iI))
					{
						meet((TeamTypes)iI, true);
					}

					if (isHasMet((TeamTypes)iI))
					{
						GET_TEAM(eIndex).meet((TeamTypes)iI, true);
					}

					if (GET_TEAM(eIndex).isAtWar((TeamTypes)iI))
					{
						declareWar((TeamTypes)iI, false, WARPLAN_DOGPILE);
					}
					else if (isAtWar((TeamTypes)iI))
					{
						if (bCapitulated)
						{
							makePeace((TeamTypes)iI);
						}
						else
						{
							GET_TEAM(eIndex).declareWar((TeamTypes)iI, false, WARPLAN_DOGPILE);
						}
					}
				}
			}

			for (int iI = 0; iI < MAX_TEAMS; iI++)
			{
				CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
				if (kLoopTeam.isAlive())
				{
					if (!kLoopTeam.isAtWar(getID()))
					{
						kLoopTeam.AI_setWarPlan(getID(), NO_WARPLAN);
						AI_setWarPlan((TeamTypes)iI, NO_WARPLAN);
					}

					if (!kLoopTeam.isAtWar(eIndex))
					{
						kLoopTeam.AI_setWarPlan(eIndex, NO_WARPLAN);
					}

				}
			}

			// All our vassals now become free
			for (int iI = 0; iI < MAX_TEAMS; iI++)
			{
				if (iI != getID() && iI != eIndex)
				{
					CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
					if (kLoopTeam.isAlive() && kLoopTeam.isVassal(getID()))
					{
						freeVassal((TeamTypes)iI);
					}
				}
			}

			setMasterPower(GET_TEAM(eIndex).getTotalLand());
			setVassalPower(getTotalLand(false));

			if (GC.getGame().isFinalInitialized() && !(gDLL->GetWorldBuilderMode()))
			{
				CvWString szReplayMessage;

				if (bCapitulated)
				{
					szReplayMessage = gDLL->getText("TXT_KEY_MISC_CAPITULATE_AGREEMENT", getName().GetCString(), GET_TEAM(eIndex).getName().GetCString());
				}
				else
				{
					szReplayMessage = gDLL->getText("TXT_KEY_MISC_VASSAL_AGREEMENT", getName().GetCString(), GET_TEAM(eIndex).getName().GetCString());
				}
				GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), szReplayMessage, -1, -1, GC.getCOLOR_HIGHLIGHT_TEXT());

				for (int iI = 0; iI < MAX_PLAYERS; iI++)
				{
					const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
					if (kPlayer.isAlive() && isHasMet(kPlayer.getTeam()) && GET_TEAM(eIndex).isHasMet(kPlayer.getTeam()))
					{
						AddDLLMessage(
							(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(), szReplayMessage,
							"AS2D_WELOVEKING", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_HIGHLIGHT_TEXT()
						);
					}
				}
			}
		}
		else
		{
			setMasterPower(0);
			setVassalPower(0);

			if (GC.getGame().isFinalInitialized() && !(gDLL->GetWorldBuilderMode()) && isAlive() && GET_TEAM(eIndex).isAlive())
			{
				CvWString szReplayMessage;

				if (m_bCapitulated)
				{
					szReplayMessage = gDLL->getText("TXT_KEY_MISC_SURRENDER_REVOLT", getName().GetCString(), GET_TEAM(eIndex).getName().GetCString());
				}
				else szReplayMessage = gDLL->getText("TXT_KEY_MISC_VASSAL_REVOLT", getName().GetCString(), GET_TEAM(eIndex).getName().GetCString());

				GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), szReplayMessage, -1, -1, GC.getCOLOR_HIGHLIGHT_TEXT());

				for (int iI = 0; iI < MAX_PLAYERS; iI++)
				{
					const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
					if (kPlayer.isAlive())
					{
						CvWString szBuffer;
						if (getID() == kPlayer.getTeam() || eIndex == kPlayer.getTeam() || isHasMet(kPlayer.getTeam()) && GET_TEAM(eIndex).isHasMet(kPlayer.getTeam()))
						{
							szBuffer = szReplayMessage;
						}

						if (!szBuffer.empty())
						{

							AddDLLMessage((PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_REVOLTSTART", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_HIGHLIGHT_TEXT());
						}
					}
				}
			}

			m_bCapitulated = false;
		}

		for (int i = 0; i < MAX_PC_PLAYERS; i++)
		{
			if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(eIndex))
			{
				GET_PLAYER((PlayerTypes)i).setMaintenanceDirty(true);
			}
		}

		if (GC.getGame().isFinalInitialized() && !gDLL->GetWorldBuilderMode())
		{
			CvEventReporter::getInstance().vassalState(eIndex, getID(), bNewValue);
		}

		// Attitude cache
		for (int iA = 0; iA < MAX_PC_PLAYERS; iA++)
		{
			CvPlayerAI& playerA = GET_PLAYER((PlayerTypes)iA);

			if (playerA.isAlive())
			{
				const TeamTypes eTeamA = playerA.getTeam();

				if (eTeamA == getID() || eTeamA == eIndex)
				{
					for (int iB = 0; iB < MAX_PC_PLAYERS; iB++)
					{
						CvPlayerAI& playerB = GET_PLAYER((PlayerTypes)iB);

						if (playerB.isAliveAndTeam(eTeamA, false) && (playerB.getTeam() == getID() || playerB.getTeam() == eIndex))
						{
							playerA.AI_invalidateAttitudeCache((PlayerTypes)iB);
							playerB.AI_invalidateAttitudeCache((PlayerTypes)iA);
						}
					}
				}
			}
		}
	}
}


void CvTeam::assignVassal(TeamTypes eVassal, bool bSurrender) const
{
	PROFILE_EXTRA_FUNC();
	CLinkList<TradeData> ourList;
	CLinkList<TradeData> theirList;
	TradeData item;

	GET_TEAM(eVassal).setVassal(getID(), true, bSurrender);

	setTradeItem(&item, bSurrender ? TRADE_SURRENDER : TRADE_VASSAL);
	item.m_iData = 1;

	for (int iA = 0; iA < MAX_PC_PLAYERS; iA++)
	{
		if (GET_PLAYER((PlayerTypes)iA).isAliveAndTeam(eVassal))
		{
			ourList.clear();
			theirList.clear();
			theirList.insertAtEnd(item);

			for (int iB = 0; iB < MAX_PC_PLAYERS; iB++)
			{
				if (GET_PLAYER((PlayerTypes)iB).isAliveAndTeam(getID()))
				{
					GC.getGame().implementDeal((PlayerTypes)iB, (PlayerTypes)iA, &ourList, &theirList, true);
				}
			}
		}
	}
}


void CvTeam::freeVassal(TeamTypes eVassal) const
{
	PROFILE_EXTRA_FUNC();
	CLLNode<TradeData>* pNode;

	foreach_(CvDeal & kLoopDeal, GC.getGame().deals())
	{
		bool bValid = true;

		if (GET_PLAYER(kLoopDeal.getFirstPlayer()).getTeam() == eVassal && GET_PLAYER(kLoopDeal.getSecondPlayer()).getTeam() == getID())
		{

			if (kLoopDeal.getFirstTrades() != NULL)
			{
				for (pNode = kLoopDeal.getFirstTrades()->head(); pNode; pNode = kLoopDeal.getFirstTrades()->next(pNode))
				{
					if ((pNode->m_data.m_eItemType == TRADE_VASSAL) ||
						(pNode->m_data.m_eItemType == TRADE_SURRENDER))
					{
						bValid = false;
					}
				}
			}
		}

		if (bValid)
		{
			if (GET_PLAYER(kLoopDeal.getFirstPlayer()).getTeam() == getID() && GET_PLAYER(kLoopDeal.getSecondPlayer()).getTeam() == eVassal)
			{
				if (kLoopDeal.getSecondTrades() != NULL)
				{
					for (pNode = kLoopDeal.getSecondTrades()->head(); pNode; pNode = kLoopDeal.getSecondTrades()->next(pNode))
					{
						if ((pNode->m_data.m_eItemType == TRADE_VASSAL) ||
							(pNode->m_data.m_eItemType == TRADE_SURRENDER))
						{
							bValid = false;
						}
					}
				}
			}
		}

		if (!bValid)
		{
			kLoopDeal.kill();
		}
	}
}

bool CvTeam::isCapitulated() const
{
	FAssert(isAVassal());

	return m_bCapitulated;
}


int CvTeam::getRouteChange(RouteTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumRouteInfos(), eIndex);
	return m_paiRouteChange[eIndex];
}


void CvTeam::changeRouteChange(RouteTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumRouteInfos(), eIndex);
	m_paiRouteChange[eIndex] += iChange;
}


int CvTeam::getProjectCount(ProjectTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumProjectInfos(), eIndex);
	return m_paiProjectCount[eIndex];
}

int CvTeam::getProjectDefaultArtType(ProjectTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumProjectInfos(), eIndex);
	return m_paiProjectDefaultArtTypes[eIndex];
}

void CvTeam::setProjectDefaultArtType(ProjectTypes eIndex, int value)
{
	FASSERT_BOUNDS(0, GC.getNumProjectInfos(), eIndex);
	m_paiProjectDefaultArtTypes[eIndex] = value;
}

int CvTeam::getProjectArtType(ProjectTypes eIndex, int number) const
{
	FASSERT_BOUNDS(0, GC.getNumProjectInfos(), eIndex);
	FASSERT_BOUNDS(0, getProjectCount(eIndex), number);
	return m_pavProjectArtTypes[eIndex][number];
}

void CvTeam::setProjectArtType(ProjectTypes eIndex, int number, int value)
{
	FASSERT_BOUNDS(0, GC.getNumProjectInfos(), eIndex);
	FASSERT_BOUNDS(0, getProjectCount(eIndex), number);
	m_pavProjectArtTypes[eIndex][number] = value;
}

bool CvTeam::isProjectMaxedOut(ProjectTypes eIndex, int iExtra) const
{
	FASSERT_BOUNDS(0, GC.getNumProjectInfos(), eIndex);

	if (!isTeamProject(eIndex))
	{
		return false;
	}

	FAssertMsg(getProjectCount(eIndex) <= GC.getProjectInfo(eIndex).getMaxTeamInstances(), "Current Project count is expected to not exceed the maximum number of instances for this project");

	return ((getProjectCount(eIndex) + iExtra) >= GC.getProjectInfo(eIndex).getMaxTeamInstances());
}

bool CvTeam::isProjectAndArtMaxedOut(ProjectTypes eIndex) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumProjectInfos(), eIndex);

	if (getProjectCount(eIndex) >= GC.getProjectInfo(eIndex).getMaxTeamInstances())
	{
		int count = getProjectCount(eIndex);
		for (int i = 0; i < count; i++)
		{
			if (getProjectArtType(eIndex, i) == -1) //undefined
				return false;
		}

		return true;
	}
	else
	{
		return false;
	}
}

void CvTeam::finalizeProjectArtTypes()
{
	PROFILE_EXTRA_FUNC();
	//loop through each project and fill in default art values
	for (int i = 0; i < GC.getNumProjectInfos(); i++)
	{
		ProjectTypes eProject = (ProjectTypes)i;
		int projectCount = getProjectCount(eProject);
		for (int j = 0; j < projectCount; j++)
		{
			int projectArtType = getProjectArtType(eProject, j);
			if (projectArtType == -1) //undefined
			{
				int defaultArtType = getProjectDefaultArtType(eProject);
				setProjectArtType(eProject, j, defaultArtType);
			}
		}
	}
}


void CvTeam::changeProjectCount(ProjectTypes eIndex, int iChange)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumProjectInfos(), eIndex);

	if (iChange != 0)
	{
		GC.getGame().incrementProjectCreatedCount(eIndex, iChange);

		const int iOldProjectCount = getProjectCount(eIndex);
		m_paiProjectCount[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(getProjectCount(eIndex));

		//adjust default art types
		if (iChange >= 0)
		{
			int defaultType = -1;
			for (int i = 0; i < iChange; i++)
				m_pavProjectArtTypes[eIndex].push_back(defaultType);
		}
		else
		{
			for (int i = 0; i < -iChange; i++)
				m_pavProjectArtTypes[eIndex].pop_back();
		}
		FAssertMsg(getProjectCount(eIndex) == (int)m_pavProjectArtTypes[eIndex].size(), "[Jason] Unbalanced project art types.");

		processProjectChange(eIndex, iChange, iOldProjectCount);

		if (iChange > 0 && GC.getGame().isFinalInitialized() && !gDLL->GetWorldBuilderMode())
		{
			const CvProjectInfo& kProject = GC.getProjectInfo(eIndex);

			GC.getGame().addReplayMessage(
				REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(),
				gDLL->getText("TXT_KEY_MISC_COMPLETES_PROJECT", getName().GetCString(), kProject.getTextKeyWide()),
				-1, -1, GC.getCOLOR_HIGHLIGHT_TEXT()
			);

			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					AddDLLMessage(
						(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_SOMEONE_HAS_COMPLETED", getName().GetCString(), kProject.getTextKeyWide()),
						"AS2D_PROJECT_COMPLETED", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_HIGHLIGHT_TEXT()
					);
				}
			}
		}
	}
}


void CvTeam::processProjectChange(ProjectTypes eIndex, int iChange, int iOldProjectCount)
{
	PROFILE_EXTRA_FUNC();
	const CvProjectInfo& kProject = GC.getProjectInfo(eIndex);

	changeNukeInterception(kProject.getNukeInterception() * iChange);

	if (kProject.getTechShare() > 0 && kProject.getTechShare() <= MAX_TEAMS)
	{
		changeTechShareCount((kProject.getTechShare() - 1), iChange);
	}

	for (int iI = 0; iI < GC.getNumVictoryInfos(); ++iI)
	{
		if (kProject.getVictoryThreshold(iI) > 0)
		{
			m_abCanLaunch[iI] = GC.getGame().testVictory((VictoryTypes)iI, getID());
		}
	}

	if (iChange > 0)
	{
		if (kProject.getEveryoneSpecialUnit() != NO_SPECIALUNIT)
		{
			GC.getGame().makeSpecialUnitValid((SpecialUnitTypes)(kProject.getEveryoneSpecialUnit()));
		}

		if (kProject.getEveryoneSpecialBuilding() != NO_SPECIALBUILDING)
		{
			GC.getGame().makeSpecialBuildingValid((SpecialBuildingTypes)(kProject.getEveryoneSpecialBuilding()));
		}

		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			CvPlayer& player = GET_PLAYER((PlayerTypes)iI);
			if (player.isAlive())
			{
				if (player.getTeam() == getID())
				{
					if (!player.isHumanPlayer())
					{
						for (int iJ = 0; iJ < GC.getNumProjectInfos(); iJ++)
						{
							if (getProjectCount(eIndex) >= GC.getProjectInfo((ProjectTypes)iJ).getProjectsNeeded(eIndex)
							&& iOldProjectCount < GC.getProjectInfo((ProjectTypes)iJ).getProjectsNeeded(eIndex))
							{
								player.AI_makeProductionDirty();
								break;
							}
						}
					}
					player.changeProjectHappiness(kProject.getGlobalHappiness());
					player.changeProjectHealth(kProject.getGlobalHealth());
					player.changeProjectInflation(kProject.getInflationModifier());
					player.changeMaintenanceModifier(kProject.getGlobalMaintenanceModifier());
					player.changeDistanceMaintenanceModifier(kProject.getDistanceMaintenanceModifier());
					player.changeNumCitiesMaintenanceModifier(kProject.getNumCitiesMaintenanceModifier());
					player.changeConnectedCityMaintenanceModifier(kProject.getConnectedCityMaintenanceModifier());

					for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
					{
						player.changeCommerceRateModifierfromBuildings(((CommerceTypes)iJ), kProject.getCommerceModifier(iJ));
					}
				}
				player.changeWorldHappiness(kProject.getWorldHappiness());
				player.changeWorldHealth(kProject.getWorldHealth());
				player.changeTradeRoutes(kProject.getWorldTradeRoutes());
			}
		}
	}
}

int CvTeam::getProjectMaking(ProjectTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumProjectInfos(), eIndex);
	return m_paiProjectMaking[eIndex];
}


void CvTeam::changeProjectMaking(ProjectTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumProjectInfos(), eIndex);
	m_paiProjectMaking[eIndex] += iChange;
	FASSERT_NOT_NEGATIVE(getProjectMaking(eIndex));
}


void CvTeam::changeUnitCount(const UnitTypes eUnit, const int iChange)
{
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
}

int CvTeam::getUnitCount(const UnitTypes eUnit) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnit);
	std::map<short, uint32_t>::const_iterator itr = m_unitCount.find((short)eUnit);
	return itr != m_unitCount.end() ? itr->second : 0;
}


int CvTeam::getBuildingCount(BuildingTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	return m_paiBuildingCount[eIndex];
}


bool CvTeam::isBuildingMaxedOut(BuildingTypes eIndex, int iExtra) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);

	if (!isTeamWonder(eIndex))
	{
		return false;
	}

	FAssertMsg(getBuildingCount(eIndex) <= GC.getBuildingInfo(eIndex).getMaxTeamInstances(), "The current building count is expected not to exceed the maximum number of instances allowed for this team");

	return ((getBuildingCount(eIndex) + iExtra) >= GC.getBuildingInfo(eIndex).getMaxTeamInstances());
}


void CvTeam::changeBuildingCount(BuildingTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);

	int iValue = m_paiBuildingCount[eIndex] + iChange;
	if (iValue < 0)
	{
		iValue = 0;
	}
	m_paiBuildingCount[eIndex] = iValue;
	FASSERT_NOT_NEGATIVE(getBuildingCount(eIndex));
}


int CvTeam::getObsoleteBuildingCount(BuildingTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	return m_paiObsoleteBuildingCount[eIndex];
}


bool CvTeam::isObsoleteBuilding(BuildingTypes eIndex) const
{
	return getObsoleteBuildingCount(eIndex) > 0;
}


void CvTeam::changeObsoleteBuildingCount(BuildingTypes eIndex, int iChange)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);

	if (iChange != 0)
	{
		const bool bWasObsolete = m_paiObsoleteBuildingCount[eIndex] > 0;

		m_paiObsoleteBuildingCount[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(getObsoleteBuildingCount(eIndex));

		if (!bWasObsolete && iChange > 0)
		{
			BuildingTypes eObsoletesToBuilding = GC.getBuildingInfo(eIndex).getObsoletesToBuilding();

			while (eObsoletesToBuilding > NO_BUILDING)
			{
				if (GC.getBuildingInfo(eObsoletesToBuilding).getObsoleteTech() < 0
				|| !isHasTech(GC.getBuildingInfo(eObsoletesToBuilding).getObsoleteTech()))
				{
					break;
				}
				eObsoletesToBuilding = GC.getBuildingInfo(eObsoletesToBuilding).getObsoletesToBuilding();
			}
			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
				{
					foreach_(CvCity * cityX, GET_PLAYER((PlayerTypes)iI).cities())
					{
						if (cityX->hasBuilding(eIndex))
						{
							cityX->changeHasBuilding(eIndex, false);

							if (eObsoletesToBuilding != NO_BUILDING && !cityX->hasBuilding(eObsoletesToBuilding))
							{
								cityX->changeHasBuilding(eObsoletesToBuilding, true);
							}
						}
					}
				}
			}
		}
	}
}


int CvTeam::getResearchProgress(TechTypes eIndex) const
{
	if (eIndex != NO_TECH)
	{
		return m_paiResearchProgress[eIndex];
	}
	return 0;
}


void CvTeam::setResearchProgress(TechTypes eIndex, int iNewValue, PlayerTypes ePlayer)
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eIndex);
	FASSERT_BOUNDS(0, MAX_PLAYERS, ePlayer);
	FASSERT_NOT_NEGATIVE(iNewValue);

	if (getResearchProgress(eIndex) != iNewValue)
	{
		m_paiResearchProgress[eIndex] = iNewValue;

		if (getID() == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		}

		if (iNewValue >= getResearchCost(eIndex))
		{
			setHasTech(eIndex, true, ePlayer, true, true);

			// Multiple Research
			if (!isNPC())
			{
				GET_PLAYER(ePlayer).changeOverflowResearch(
					GET_PLAYER(ePlayer).doMultipleResearch(
						getModifiedIntValue(
							(iNewValue - getResearchCost(eIndex)),
							-GET_PLAYER(ePlayer).calculateResearchModifier(eIndex)
						)
					)
				);
			}
		}
	}
}


void CvTeam::changeResearchProgress(TechTypes eIndex, int iChange, PlayerTypes ePlayer)
{
	setResearchProgress(eIndex, std::max(0, getResearchProgress(eIndex) + iChange), ePlayer);
}

int CvTeam::changeResearchProgressPercent(TechTypes eIndex, int iPercent, PlayerTypes ePlayer)
{
	int iBeakers = 0;

	if (0 != iPercent && !isHasTech(eIndex))
	{
		if (iPercent > 0)
		{
			iBeakers = std::min(getResearchLeft(eIndex), (getResearchCost(eIndex) * iPercent) / 100);
		}
		else
		{
			iBeakers = std::max(getResearchLeft(eIndex) - getResearchCost(eIndex), (getResearchCost(eIndex) * iPercent) / 100);
		}
		changeResearchProgress(eIndex, iBeakers, ePlayer);
	}
	return iBeakers;
}


int CvTeam::getTechCount(TechTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eIndex);
	return m_paiTechCount[eIndex];
}


int CvTeam::getBestKnownTechScorePercent() const
{
	PROFILE_EXTRA_FUNC();
	int iOurTechScore = 0;
	int iBestKnownTechScore = 0;

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				iOurTechScore = std::max(iOurTechScore, GET_PLAYER((PlayerTypes)iI).getTechScore());
			}
			else if (isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
			{
				iBestKnownTechScore = std::max(iBestKnownTechScore, GET_PLAYER((PlayerTypes)iI).getTechScore());
			}
		}
	}

	iBestKnownTechScore = std::max(iBestKnownTechScore, iOurTechScore);

	return ((100 * iOurTechScore) / std::max(iBestKnownTechScore, 1));
}


int CvTeam::getTerrainTradeCount(TerrainTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), eIndex);
	return m_paiTerrainTradeCount[eIndex];
}


bool CvTeam::isTerrainTrade(TerrainTypes eIndex) const
{
	if (isNPC())
	{
		return false;
	}
	return (getTerrainTradeCount(eIndex) > 0);
}


void CvTeam::changeTerrainTradeCount(TerrainTypes eIndex, int iChange)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), eIndex);

	if (iChange != 0)
	{
		m_paiTerrainTradeCount[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(getTerrainTradeCount(eIndex));

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
			{
				GET_PLAYER((PlayerTypes)iI).updatePlotGroups();
			}
		}
	}
}


int CvTeam::getRiverTradeCount() const
{
	return m_iRiverTradeCount;
}


bool CvTeam::isRiverTrade() const
{
	return (getRiverTradeCount() > 0);
}


void CvTeam::changeRiverTradeCount(int iChange)
{
	PROFILE_EXTRA_FUNC();
	if (iChange != 0)
	{
		m_iRiverTradeCount += iChange;
		FASSERT_NOT_NEGATIVE(getRiverTradeCount());

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
			{
				GET_PLAYER((PlayerTypes)iI).updatePlotGroups();
			}
		}
	}
}


int CvTeam::getVictoryCountdown(VictoryTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumVictoryInfos(), eIndex);
	return m_aiVictoryCountdown[eIndex];
}

void CvTeam::setVictoryCountdown(VictoryTypes eIndex, int iTurnsLeft)
{
	FASSERT_BOUNDS(0, GC.getNumVictoryInfos(), eIndex);
	m_aiVictoryCountdown[eIndex] = iTurnsLeft;
}


void CvTeam::changeVictoryCountdown(VictoryTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumVictoryInfos(), eIndex);

	if (iChange != 0)
	{
		m_aiVictoryCountdown[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(m_aiVictoryCountdown[eIndex]);
	}
}

int CvTeam::getVictoryDelay(VictoryTypes eVictory) const
{
	PROFILE_EXTRA_FUNC();
	const int iDelay = GC.getGame().victoryDelay(eVictory);
	if (iDelay < 1) return iDelay;

	int iExtraDelayPercent = 0;
	for (int iProject = 0; iProject < GC.getNumProjectInfos(); ++iProject)
	{
		const CvProjectInfo& kProject = GC.getProjectInfo((ProjectTypes)iProject);

		const int iMinThreshold = kProject.getVictoryMinThreshold(eVictory);
		if (iMinThreshold < 1) continue;

		const int victoryThreshold = kProject.getVictoryThreshold(eVictory);
		if (victoryThreshold < 1) continue;

		const int iCount = getProjectCount((ProjectTypes)iProject);

		// Toffer - Only increase the delay from this.
		if (iCount >= iMinThreshold && iCount < victoryThreshold)
		{
			iExtraDelayPercent += (victoryThreshold - iCount) * kProject.getVictoryDelayPercent() / victoryThreshold;
		}
	}
	return iDelay * (100 + iExtraDelayPercent) / 100;
}

void CvTeam::setCanLaunch(VictoryTypes eVictory, bool bCan)
{
	m_abCanLaunch[eVictory] = bCan;
}

bool CvTeam::canLaunch(VictoryTypes eVictory) const
{
	return m_abCanLaunch[eVictory];
}

int CvTeam::getLaunchSuccessRate(VictoryTypes eVictory) const
{
	PROFILE_EXTRA_FUNC();
	int iSuccessRate = 100;
	for (int iProject = 0; iProject < GC.getNumProjectInfos(); ++iProject)
	{
		const CvProjectInfo& kProject = GC.getProjectInfo((ProjectTypes)iProject);
		const int iCount = getProjectCount((ProjectTypes)iProject);

		if (iCount < kProject.getVictoryMinThreshold(eVictory))
		{
			return 0;
		}
		if (iCount < kProject.getVictoryThreshold(eVictory) && kProject.getSuccessRate() > 0)
		{
			iSuccessRate -= (kProject.getSuccessRate() * (kProject.getVictoryThreshold(eVictory) - iCount));
		}
	}
	return iSuccessRate;
}

void CvTeam::resetVictoryProgress()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getNumVictoryInfos(); ++iI)
	{
		if (getVictoryCountdown((VictoryTypes)iI) >= 0 && GC.getGame().getGameState() == GAMESTATE_ON)
		{
			setVictoryCountdown((VictoryTypes)iI, -1);

			for (int iK = 0; iK < GC.getNumProjectInfos(); iK++)
			{
				if (GC.getProjectInfo((ProjectTypes)iK).getVictoryMinThreshold((VictoryTypes)iI) > 0)
				{
					changeProjectCount((ProjectTypes)iK, -getProjectCount((ProjectTypes)iK));
				}
			}

			bool bValid = false;
			for (int iJ = 0; iJ < GC.getNumVictoryInfos(); ++iJ)
			{
				if (GC.getGame().isVictoryValid((VictoryTypes)iJ))
				{
					if (GC.getVictoryInfo((VictoryTypes)iJ).isTotalVictory())
					{
						bValid = true;
						break;
					}
				}
			}
			if (!bValid)
			{
				const CvWString szBuffer = gDLL->getText("TXT_KEY_VICTORY_RESET", getName().GetCString(), GC.getVictoryInfo((VictoryTypes)iI).getTextKeyWide());

				for (int iJ = 0; iJ < MAX_PLAYERS; ++iJ)
				{
					const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iJ);
					if (kPlayer.isAlive())
					{

						AddDLLMessage(((PlayerTypes)iJ), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_MELTDOWN", MESSAGE_TYPE_MAJOR_EVENT);

						if (kPlayer.getTeam() == getID())
						{
							CvPopupInfo* pInfo = new CvPopupInfo();
							pInfo->setText(szBuffer);
							gDLL->getInterfaceIFace()->addPopup(pInfo, (PlayerTypes)iJ);
						}
					}
				}
				GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), szBuffer, -1, -1, GC.getCOLOR_HIGHLIGHT_TEXT());
			}
		}
	}
}

bool CvTeam::isParent(TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

	if (GET_TEAM(eTeam).isVassal(getID()))
	{
		for (int i = 0; i < MAX_PC_PLAYERS; ++i)
		{
			const CvPlayer& player = GET_PLAYER((PlayerTypes)i);

			if (player.isAliveAndTeam(eTeam) && player.getParent() != NO_PLAYER
			&& GET_PLAYER(player.getParent()).getTeam() == getID())
			{
				return true;
			}
		}
	}
	return false;
}

bool CvTeam::isHasTech(TechTypes eIndex) const
{
	if (eIndex <= NO_TECH)
	{
		return true;
	}
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eIndex);
	FAssertMsg(m_pabHasTech, "m_pabHasTech is not expected to be equal with NULL");
	if (m_pabHasTech)
		return m_pabHasTech[eIndex];
	return false;
}


void CvTeam::cacheAdjacentResearch()
{
	PROFILE_EXTRA_FUNC();
	//OutputDebugString(CvString::format("cacheAdjacentResearch team=%d\n", getID()).c_str());
	if (isNPC() && !isHominid())
	{
		return; // Animal NPC's can't research anyway.
	}
	m_adjacentResearch.clear();

	const CvPlayer& leader = GET_PLAYER(getLeaderID());

	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		if (leader.canResearch((TechTypes)iI, true, false))
		{
			m_adjacentResearch.push_back((TechTypes)iI);
			//OutputDebugString(CvString::format("\tcacheAdjacentResearch tech=%d\n", iI).c_str());
		}
	}
}

void CvTeam::setAdjacentResearch(const TechTypes eTech, const bool bNewValue)
{
	if (isNPC() && !isHominid())
	{
		return; // Animal NPC's can't research anyway.
	}
	//OutputDebugString(CvString::format("setAdjacentResearch team=%d, tech=%d, bNewValue=%d\n", getID(), (int)eTech, (int)bNewValue).c_str());
	std::vector<TechTypes>::iterator itr = find(m_adjacentResearch.begin(), m_adjacentResearch.end(), eTech);

	if (bNewValue)
	{
		if (itr == m_adjacentResearch.end())
		{
			m_adjacentResearch.push_back(eTech);
		}
	}
	else if (itr != m_adjacentResearch.end())
	{
		m_adjacentResearch.erase(itr);
	}
}

void CvTeam::announceTechToPlayers(TechTypes eIndex, bool bPartial)
{
	PROFILE_EXTRA_FUNC();
	const bool bSound = !bPartial && (GC.getGame().isNetworkMultiPlayer() || gDLL->getInterfaceIFace()->noTechSplash());

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			AddDLLMessage(
				(PlayerTypes)iI, false, (bSound ? GC.getEVENT_MESSAGE_TIME() : -1),
				gDLL->getText(
					(bPartial ? "TXT_KEY_MISC_PROGRESS_TOWARDS_TECH" : "TXT_KEY_MISC_YOU_DISCOVERED_TECH"),
					GC.getTechInfo(eIndex).getTextKeyWide()
				),
				(bSound ? GC.getTechInfo(eIndex).getSoundMP() : NULL),
				MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_TECH_TEXT()
			);
		}
	}
}

void CvTeam::setHasTech(TechTypes eTech, bool bNewValue, PlayerTypes ePlayer, bool bFirst, bool bAnnounce)
{
	PROFILE_FUNC();

	if (eTech == NO_TECH)
	{
		return;
	}
	if (ePlayer == NO_PLAYER)
	{
		ePlayer = getLeaderID();
	}
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eTech);
	FASSERT_BOUNDS(0, MAX_PLAYERS, ePlayer);

	const CvTechInfo& kTech = GC.getTechInfo(eTech);

	if (isHasTech(eTech) == bNewValue && (!kTech.isRepeat() || m_paiTechCount[eTech] < 1))
	{
		return;
	}
	const int iChange = bNewValue ? 1 : -1;

	if (kTech.isRepeat())
	{
		if (bNewValue)
		{
			m_paiTechCount[eTech]++;

			setResearchProgress(eTech, 0, ePlayer);

			// report event to Python
			CvEventReporter::getInstance().techAcquired(eTech, getID(), ePlayer, bAnnounce && 1 == m_paiTechCount[eTech]);

			if (1 == m_paiTechCount[eTech] && bAnnounce && GC.getGame().isFinalInitialized())
			{
				announceTechToPlayers(eTech);
			}
		}
		else
		{
			m_paiTechCount[eTech]--;
			setResearchProgress(eTech, 0, ePlayer);
		}
		processTech(eTech, iChange, bAnnounce);
		return;
	}

	GET_PLAYER(ePlayer).resetBonusClassTallyCache(-1, false);

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot->getBonusType() != NO_BONUS && pLoopPlot->getTeam() == getID())
		{
			const CvBonusInfo& bonus = GC.getBonusInfo(pLoopPlot->getBonusType());

			if (bonus.getTechReveal() == eTech
			|| bonus.getTechCityTrade() == eTech
			|| bonus.getTechObsolete() == eTech)
			{
				pLoopPlot->updatePlotGroupBonus(false);
			}
		}
	}

	m_pabHasTech[eTech] = bNewValue;

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot->getBonusType() != NO_BONUS && pLoopPlot->getTeam() == getID())
		{
			const CvBonusInfo& bonus = GC.getBonusInfo(pLoopPlot->getBonusType());

			if (bonus.getTechReveal() == eTech
			|| bonus.getTechCityTrade() == eTech
			|| bonus.getTechObsolete() == eTech)
			{
				pLoopPlot->updatePlotGroupBonus(true);
			}
		}
	}
	processTech(eTech, iChange, bAnnounce);

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& player = GET_PLAYER((PlayerTypes)iI);

		if (player.isAliveAndTeam(getID()))
		{
			player.updateCorporation();

			foreach_(CvCity * cityX, player.cities())
			{
				// Toffer - Buildings may change commerce output with tech.
				//	Not part of processTech to avoid it applying twice during recalc, would apply in processBuilding first then in processTech again after.
				for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
				{
					cityX->changeBuildingCommerceTechChange((CommerceTypes)iJ, iChange * cityX->getBuildingCommerceTechChange((CommerceTypes)iJ, eTech));
					cityX->changeBuildingCommerceModifier((CommerceTypes)iJ, iChange * cityX->getBuildingCommerceTechModifier((CommerceTypes)iJ, eTech));
				}
				for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					cityX->changeBuildingExtraYield100((YieldTypes)iJ, iChange * cityX->getBuildingYieldTechChange((YieldTypes)iJ, eTech));
					cityX->changeBuildingYieldModifier((YieldTypes)iJ, iChange * cityX->getBuildingYieldTechModifier((YieldTypes)iJ, eTech));
				}
				// A new tech can effect best plot build decisions so mark stale in all cities
				cityX->AI_markBestBuildValuesStale();
			}
		}
	}

	if (isHasTech(eTech))
	{
		if (GC.getGame().isFinalInitialized())
		{
			// Toffer - Tracking of this cache causes issues during game initialization.
			//	A sweeping cache update will be done at when the game is finally initialized anyway.
			setAdjacentResearch(eTech, false);

			foreach_(const TechTypes eTechX, kTech.getLeadsToTechs())
			{
				if (GET_PLAYER(getLeaderID()).canResearch(eTechX, true, false))
				{
					setAdjacentResearch(eTechX, true);
				}
			}
		}

		if (gTeamLogLevel >= 2)
		{
			logBBAI("    Team %d (%S) acquires tech %S", getID(), GET_PLAYER(ePlayer).getCivilizationDescription(0), kTech.getDescription());
		}
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			CvPlayerAI& player = GET_PLAYER((PlayerTypes)iI);

			if (player.getTeam() == getID())
			{
				if (player.getCurrentEra() < kTech.getEra())
				{
					player.setCurrentEra((EraTypes)(kTech.getEra()));
				}

				//	Reconsider civics on acquiring tech
				player.AI_setCivicTimer(0);

				//	Recalculate bonus values on acquiring a new tech
				player.AI_updateBonusValue();
			}
		}
		if (kTech.isMapVisible())
		{
			GC.getMap().setRevealedPlots(getID(), true, true);
		}
		// report event to Python, along with some other key state
		CvEventReporter::getInstance().techAcquired(eTech, getID(), ePlayer, bAnnounce);

		bool bReligionFounded = false;
		bool bClearResearchQueueAI = false;

		if (bFirst && GC.getGame().countKnownTechNumTeams(eTech) == 1)
		{
			if (!GC.getGame().isOption(GAMEOPTION_RELIGION_DIVINE_PROPHETS)
			&& GC.getGame().isTechCanFoundReligion(eTech))
			{
				for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
				{
					int iBestValue = MAX_INT;
					PlayerTypes eBestPlayer = NO_PLAYER;
					const ReligionTypes eSlotReligion = ReligionTypes(iI);

					if (GC.getReligionInfo(eSlotReligion).getTechPrereq() == eTech
					&& !GC.getGame().isReligionSlotTaken(eSlotReligion))
					{
						ReligionTypes eReligion = NO_RELIGION;

						for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
						{
							const CvPlayer& playerX = GET_PLAYER((PlayerTypes)iJ);
							if (playerX.isAliveAndTeam(getID()) && playerX.canFoundReligion())
							{
								int iValue = 10 + GC.getGame().getSorenRandNum(10, "Found Religion (Player)");

								for (int iK = 0; iK < GC.getNumReligionInfos(); iK++)
								{
									iValue += playerX.getHasReligionCount((ReligionTypes)iK);
								}
								if (playerX.getCurrentResearch() != eTech)
								{
									iValue *= 10;
								}

								if (iValue < iBestValue)
								{
									iBestValue = iValue;
									eBestPlayer = (PlayerTypes)iJ;
									eReligion = (ReligionTypes)iI;
								}
							}
						}
						if (eBestPlayer != NO_PLAYER)
						{
							if (GC.getGame().isOption(GAMEOPTION_RELIGION_PICK))
							{
								if (GET_PLAYER(eBestPlayer).isHumanPlayer())
								{
									GET_PLAYER(eBestPlayer).m_bChoosingReligion = true;
									CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_FOUND_RELIGION, iI);
									if (NULL != pInfo)
									{
										gDLL->getInterfaceIFace()->addPopup(pInfo, eBestPlayer);
									}
								}
								else
								{
									eReligion = GET_PLAYER(eBestPlayer).AI_chooseReligion();
									GET_PLAYER(eBestPlayer).foundReligion(eReligion, eSlotReligion, true);
									bReligionFounded = true;
								}
							}
							else
							{
								GET_PLAYER(eBestPlayer).foundReligion(eReligion, eSlotReligion, true);
								bReligionFounded = true;
							}
							bClearResearchQueueAI = true;
						}
						break; // Only one religion can be founded by a single tech as they are global techs.
					}
				}
			}
			for (int iI = 0; iI < GC.getNumCorporationInfos(); ++iI)
			{
				if (GC.getCorporationInfo((CorporationTypes)iI).getTechPrereq() == eTech)
				{
					PlayerTypes eBestPlayer = NO_PLAYER;
					if (!GC.getGame().isCorporationFounded((CorporationTypes)iI))
					{
						int iBestValue = MAX_INT;

						for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
						{
							if (GET_PLAYER((PlayerTypes)iJ).isAliveAndTeam(getID()))
							{
								int iValue = 10 + GC.getGame().getSorenRandNum(10, "Found Corporation (Player)");

								if (GET_PLAYER((PlayerTypes)iJ).getCurrentResearch() != eTech)
								{
									iValue *= 10;
								}

								if (iValue < iBestValue)
								{
									iBestValue = iValue;
									eBestPlayer = ((PlayerTypes)iJ);
								}
							}
						}
					}
					if (eBestPlayer != NO_PLAYER)
					{
						GET_PLAYER(eBestPlayer).foundCorporation((CorporationTypes)iI);
						bClearResearchQueueAI = true;
					}
				}
			}
		}
		{
			const bool bGlobal = kTech.isGlobal();

			if (bGlobal)
			{
				for (int iI = 0; iI < MAX_TEAMS; iI++)
				{
					if (iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
					{
						const std::vector<TechTypes>& adjacentResearch = GET_TEAM((TeamTypes)iI).getAdjacentResearch();

						if (find(adjacentResearch.begin(), adjacentResearch.end(), eTech) != adjacentResearch.end())
						{
							GET_TEAM((TeamTypes)iI).setAdjacentResearch(eTech, false);
						}
					}
				}
			}

			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				CvPlayerAI& playerX = GET_PLAYER((PlayerTypes)iI);

				if (playerX.isAlive())
				{
					if (playerX.getTeam() == getID())
					{
						if (playerX.isResearchingTech(eTech))
						{
							playerX.popResearch(eTech);
						}
						playerX.AI_nowHasTech(eTech);
						playerX.invalidateYieldRankCache();
					}
					else if (bGlobal && playerX.isResearchingTech(eTech))
					{
						playerX.popResearch(eTech);
					}
				}
			}
		}

		if (bFirst && GC.getGame().countKnownTechNumTeams(eTech) == 1)
		{
			const UnitTypes eFreeUnit = (UnitTypes)kTech.getFirstFreeUnit();
			if (eFreeUnit != NO_UNIT)
			{
				bClearResearchQueueAI = true;
				CvCity* pCapitalCity = GET_PLAYER(ePlayer).getCapitalCity();

				if (pCapitalCity != NULL)
				{
					pCapitalCity->createGreatPeople(eFreeUnit, false, false);
				}
			}
			//TB Prophet Mod begin
			if (GC.getGame().isOption(GAMEOPTION_RELIGION_DIVINE_PROPHETS))
			{
				const UnitTypes eFreeProphet = GET_PLAYER(ePlayer).getTechFreeProphet(eTech);
				if (eFreeProphet != NO_UNIT)
				{
					bClearResearchQueueAI = true;
					CvCity* pCapitalCity = GET_PLAYER(ePlayer).getCapitalCity();

					if (pCapitalCity != NULL)
					{
						pCapitalCity->createGreatPeople(eFreeProphet, false, false);
					}
				}
			}
			//TB Prophet Mod end
			if (kTech.getFirstFreeTechs() > 0)
			{
				bClearResearchQueueAI = true;

				if (!isHuman())
				{
					for (int iI = 0; iI < kTech.getFirstFreeTechs(); iI++)
					{
						GET_PLAYER(ePlayer).AI_chooseFreeTech();
					}
				}
				else
				{
					GET_PLAYER(ePlayer).chooseTech(kTech.getFirstFreeTechs(), gDLL->getText("TXT_KEY_MISC_FIRST_TECH_CHOOSE_FREE", kTech.getTextKeyWide()));
				}

				CvWString szBuffer;
				for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive())
					{
						if (isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
						{
							szBuffer = gDLL->getText("TXT_KEY_MISC_SOMEONE_FIRST_TO_TECH", GET_PLAYER(ePlayer).getNameKey(), kTech.getTextKeyWide());
						}
						else
						{
							szBuffer = gDLL->getText("TXT_KEY_MISC_UNKNOWN_FIRST_TO_TECH", kTech.getTextKeyWide());
						}
						AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_FIRSTTOTECH", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_HIGHLIGHT_TEXT());
					}
				}
				if (!GET_PLAYER(ePlayer).isNPC())
				{
					szBuffer = gDLL->getText("TXT_KEY_MISC_SOMEONE_FIRST_TO_TECH", GET_PLAYER(ePlayer).getName(), kTech.getTextKeyWide());
					GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, ePlayer, szBuffer, -1, -1, GC.getCOLOR_HIGHLIGHT_TEXT());
				}
			}

			if (bClearResearchQueueAI)
			{
				for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive() && !GET_PLAYER((PlayerTypes)iI).isHumanPlayer()
					&& GET_PLAYER((PlayerTypes)iI).isResearchingTech(eTech))
					{
						GET_PLAYER((PlayerTypes)iI).clearResearchQueue();
					}
				}
			}
		}

		if (bAnnounce)
		{
			if (GC.getGame().isFinalInitialized() && !(gDLL->GetWorldBuilderMode()))
			{
				announceTechToPlayers(eTech);

				for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
				{
					CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

					if (pLoopPlot->getTeam() == getID())
					{
						const BonusTypes eBonus = pLoopPlot->getBonusType();

						if (eBonus != NO_BONUS && GC.getBonusInfo(eBonus).getTechReveal() == eTech && !isForceRevealedBonus(eBonus))
						{
							const CvCity* pCity = GC.getMap().findCity(pLoopPlot->getX(), pLoopPlot->getY(), NO_PLAYER, getID(), false);

							if (pCity != NULL)
							{
								AddDLLMessage(
									pLoopPlot->getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
									gDLL->getText("TXT_KEY_MISC_YOU_DISCOVERED_BONUS", GC.getBonusInfo(eBonus).getTextKeyWide(), pCity->getNameKey()),
									"AS2D_DISCOVERBONUS", MESSAGE_TYPE_INFO, GC.getBonusInfo(eBonus).getButton(),
									GC.getCOLOR_WHITE(), pLoopPlot->getX(), pLoopPlot->getY(), true, true
								);
							}
						}
					}
				}
			}

			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				const CvPlayer& playerX = GET_PLAYER((PlayerTypes)iI);

				if (playerX.isAliveAndTeam(getID()) && playerX.isHumanPlayer() && playerX.canRevolution(NULL)
				&& (!bReligionFounded || playerX.getLastStateReligion() != NO_RELIGION || iI != ePlayer))
				{
					CivicOptionTypes eCivicOptionType = NO_CIVICOPTION;
					CivicTypes eCivicType = NO_CIVIC;

					for (int iJ = 0; iJ < GC.getNumCivicOptionInfos(); iJ++)
					{
						if (!playerX.isHasCivicOption((CivicOptionTypes)iJ))
						{
							for (int iK = 0; iK < GC.getNumCivicInfos(); iK++)
							{
								if (GC.getCivicInfo((CivicTypes)iK).getCivicOptionType() == iJ
								&& GC.getCivicInfo((CivicTypes)iK).getTechPrereq() == eTech)
								{
									eCivicOptionType = (CivicOptionTypes)iJ;
									eCivicType = (CivicTypes)iK;
								}
							}
						}
					}

					if (eCivicOptionType != NO_CIVICOPTION && eCivicType != NO_CIVIC)
					{
						CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHANGECIVIC);
						if (NULL != pInfo)
						{
							pInfo->setData1(eCivicOptionType);
							pInfo->setData2(eCivicType);
							gDLL->getInterfaceIFace()->addPopup(pInfo, (PlayerTypes)iI);
						}
					}
				}
			}
		}

		for (int iI = 0; iI < MAX_TEAMS; iI++)
		{
			if (iI != getID() && GET_TEAM((TeamTypes)iI).isAlive())
			{
				GET_TEAM((TeamTypes)iI).updateTechShare(eTech);
			}
		}

		if (bNewValue && bAnnounce && GC.getGame().isFinalInitialized() && !gDLL->GetWorldBuilderMode()
		&& GET_PLAYER(ePlayer).getCurrentResearch() == NO_TECH && ePlayer == GC.getGame().getActivePlayer())
		{
			GET_PLAYER(ePlayer).chooseTech(0, gDLL->getText("TXT_KEY_MISC_WHAT_TO_RESEARCH_NEXT"));
		}

		if (getID() == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(MiscButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(ResearchButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);
		}
	}
	else
	{
		// Rarely are techs removed, probably worldbuilder.
		cacheAdjacentResearch(); // Harder to remove than add.
	}
}


bool CvTeam::isNoTradeTech(const short iTech) const
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), iTech);
	return algo::any_of_equal(m_vNoTradeTech, iTech);
}


void CvTeam::setNoTradeTech(const short iTech, const bool bNewValue)
{
	std::vector<short>::iterator itr = find(m_vNoTradeTech.begin(), m_vNoTradeTech.end(), iTech);

	FASSERT_BOUNDS(0, GC.getNumTechInfos(), iTech);
	FAssertMsg(bNewValue != (itr != m_vNoTradeTech.end()), "This is no change!");

	if (bNewValue)
	{
		if (itr == m_vNoTradeTech.end())
		{
			m_vNoTradeTech.push_back(iTech);
		}
	}
	else if (itr != m_vNoTradeTech.end())
	{
		m_vNoTradeTech.erase(itr);
	}
}


int CvTeam::getImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), eIndex1);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex2);
	return m_ppaaiImprovementYieldChange[eIndex1][eIndex2];
}

void CvTeam::changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), eIndex1);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex2);

	if (iChange != 0)
	{
		m_ppaaiImprovementYieldChange[eIndex1][eIndex2] += iChange;

		updateYield();
	}
}


int CvTeam::getBuildingYieldTechChange(const YieldTypes eYield, const BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	int iYield100 = 0;
	foreach_(const TechArray & pair, GC.getBuildingInfo(eBuilding).getTechYieldChanges100())
	{
		if (isHasTech(pair.first))
		{
			iYield100 += pair.second[eYield];
		}
	}
	return iYield100;
}


int CvTeam::getBuildingYieldTechModifier(const YieldTypes eYield, const BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	int iMod = 0;
	foreach_(const TechArray & pair, GC.getBuildingInfo(eBuilding).getTechYieldModifiers())
	{
		if (isHasTech(pair.first))
		{
			iMod += pair.second[eYield];
		}
	}
	return iMod;
}


int CvTeam::getBuildingCommerceTechChange(const CommerceTypes eIndex, const BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	int iCommerce100 = 0;
	foreach_(const TechCommerceArray & pair, GC.getBuildingInfo(eBuilding).getTechCommerceChanges100())
	{
		if (isHasTech(pair.first))
		{
			iCommerce100 += pair.second[eIndex];
		}
	}
	return iCommerce100;
}


int CvTeam::getBuildingCommerceTechModifier(const CommerceTypes eIndex, const BuildingTypes eBuilding) const
{
	PROFILE_EXTRA_FUNC();
	int iMod = 0;
	foreach_(const TechCommerceArray & pair, GC.getBuildingInfo(eBuilding).getTechCommerceModifiers())
	{
		if (isHasTech(pair.first))
		{
			iMod += pair.second[eIndex];
		}
	}
	return iMod;
}

// Protected Functions...

void CvTeam::doWarWeariness()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (getWarWeariness((TeamTypes)iI) > 0)
		{
			changeWarWeariness((TeamTypes)iI, 100 * GC.getWW_DECAY_RATE());

			// War weariness decays faster when enemy can't fight, usually a rebel with just spies
			if (!GET_TEAM((TeamTypes)iI).isAlive() || GET_TEAM((TeamTypes)iI).getNumMilitaryUnits() == 0 || !isAtWar((TeamTypes)iI))
			{
				setWarWeariness(((TeamTypes)iI), ((getWarWeariness((TeamTypes)iI) * GC.getWW_DECAY_PEACE_PERCENT()) / 100));
			}
		}
	}
}

void CvTeam::updateTechShare(TechTypes eTech)
{
	PROFILE_EXTRA_FUNC();
	if (isHasTech(eTech))
	{
		return;
	}

	int iBestShare = MAX_INT;

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (isTechShare(iI))
		{
			iBestShare = std::min(iBestShare, iI + 1);
		}
	}

	if (iBestShare != MAX_INT)
	{
		int iCount = 0;

		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive()
			&& GET_TEAM((TeamTypes)iI).isHasTech(eTech)
			&& isHasMet((TeamTypes)iI))
			{
				FAssertMsg(iI != getID(), "iI is not expected to be equal with getID()");
				iCount++;
			}
		}

		if (iCount >= iBestShare)
		{
			setHasTech(eTech, true, NO_PLAYER, true, true);
		}
	}
}


void CvTeam::updateTechShare()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		updateTechShare((TechTypes)iI);
	}
}


void CvTeam::testCircumnavigated()
{
	PROFILE_EXTRA_FUNC();
	if (isNPC() || !GC.getGame().circumnavigationAvailable())
	{
		return;
	}
	if (GC.getMap().isWrapX())
	{
		for (int iX = 0; iX < GC.getMap().getGridWidth(); iX++)
		{
			bool bFoundVisible = false;

			for (int iY = 0; iY < GC.getMap().getGridHeight(); iY++)
			{
				const CvPlot* pPlot = GC.getMap().plotSorenINLINE(iX, iY);

				if (pPlot->isRevealed(getID(), false))
				{
					bFoundVisible = true;
					break;
				}
			}
			if (!bFoundVisible)
			{
				return;
			}
		}
	}
	if (GC.getMap().isWrapY())
	{
		for (int iY = 0; iY < GC.getMap().getGridHeight(); iY++)
		{
			bool bFoundVisible = false;

			for (int iX = 0; iX < GC.getMap().getGridWidth(); iX++)
			{
				const CvPlot* pPlot = GC.getMap().plotSorenINLINE(iX, iY);

				if (pPlot->isRevealed(getID(), false))
				{
					bFoundVisible = true;
					break;
				}
			}
			if (!bFoundVisible)
			{
				return;
			}
		}
	}
	GC.getGame().setCircumnavigatedTeam(getID());

	// Announce and record history
	CvWString szBuffer;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{

			if (getID() == GET_PLAYER((PlayerTypes)iI).getTeam())
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_YOU_CIRC_GLOBE", GC.getCIRCUMNAVIGATE_FREE_MOVES());
			}
			else if (isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_SOMEONE_CIRC_GLOBE", getName().GetCString());
			}
			else
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_UNKNOWN_CIRC_GLOBE");
			}
			AddDLLMessage((PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_GLOBECIRCUMNAVIGATED",
				MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_HIGHLIGHT_TEXT());
		}
	}
	szBuffer = gDLL->getText("TXT_KEY_MISC_SOMEONE_CIRC_GLOBE", getName().GetCString());
	GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getLeaderID(), szBuffer, -1, -1, GC.getCOLOR_HIGHLIGHT_TEXT());
}

void CvTeam::setCircumnavigated(bool bNewValue)
{
	if (bNewValue)
	{
		changeExtraMoves(DOMAIN_SEA, GC.getCIRCUMNAVIGATE_FREE_MOVES());
	}
	else changeExtraMoves(DOMAIN_SEA, -GC.getCIRCUMNAVIGATE_FREE_MOVES());
}


void CvTeam::processTech(TechTypes eTech, int iChange, bool bAnnounce)
{
	PROFILE_FUNC();
	const CvTechInfo& tech = GC.getTechInfo(eTech);

	if (tech.isExtraWaterSeeFrom())
	{
		changeExtraWaterSeeFromCount(iChange);
	}

	if (iChange > 0 && tech.isMapCentering())
	{
		setMapCentering(true);
	}

	if (tech.isMapTrading())
	{
		changeMapTradingCount(iChange);
	}

	if (tech.isCanPassPeaks())
	{
		changeCanPassPeaksCount(iChange);
		//	Koshling - makes peaks workable which chnages the yield calculation
		updateYield();
	}
	if (tech.isMoveFastPeaks())
	{
		changeMoveFastPeaksCount(iChange);
	}
	if (tech.isCanFoundOnPeaks())
	{
		changeCanFoundOnPeaksCount(iChange);
	}

	if (iChange > 0)
	{
		ObsoletePromotions(eTech);
		ObsoleteCorporations(eTech);
		GC.getGame().makeTechDiscovered(eTech);
	}

	if (tech.isEmbassyTrading())
	{
		changeEmbassyTradingCount(iChange);
	}

	if (tech.isRebaseAnywhere())
	{
		changeRebaseAnywhereCount(iChange);
	}

	if (tech.getGlobalTradeModifier() != 0)
	{
		changeTradeModifier(tech.getGlobalTradeModifier() * iChange);
	}

	if (tech.getGlobalForeignTradeModifier() != 0)
	{
		changeForeignTradeModifier(tech.getGlobalForeignTradeModifier() * iChange);
	}

	if (tech.getTradeMissionModifier() != 0)
	{
		changeTradeMissionModifier(tech.getTradeMissionModifier() * iChange);
	}

	if (tech.getCorporationRevenueModifier() != 0)
	{
		changeCorporationRevenueModifier(tech.getCorporationRevenueModifier() * iChange);
	}

	if (tech.getCorporationMaintenanceModifier() != 0)
	{
		changeCorporationMaintenanceModifier(tech.getCorporationMaintenanceModifier() * iChange);
	}

	if (tech.isEnablesDesertFarming())
	{
		changeCanFarmDesertCount(iChange);
		setLastRoundOfValidImprovementCacheUpdate();
	}

	if (tech.isTechTrading())
	{
		changeTechTradingCount(iChange);
	}

	if (tech.isGoldTrading())
	{
		changeGoldTradingCount(iChange);
	}

	if (tech.isOpenBordersTrading())
	{
		changeOpenBordersTradingCount(iChange);
		changeLimitedBordersTradingCount(iChange);
	}

	if (tech.isDefensivePactTrading())
	{
		changeDefensivePactTradingCount(iChange);
	}

	if (tech.isPermanentAllianceTrading())
	{
		changePermanentAllianceTradingCount(iChange);
	}

	if (tech.isVassalStateTrading())
	{
		changeVassalTradingCount(iChange);
	}

	if (tech.isBridgeBuilding())
	{
		changeBridgeBuildingCount(iChange);
	}

	if (tech.isIrrigation())
	{
		changeIrrigationCount(iChange);
		setLastRoundOfValidImprovementCacheUpdate();
	}

	if (tech.isIgnoreIrrigation())
	{
		changeIgnoreIrrigationCount(iChange);
		setLastRoundOfValidImprovementCacheUpdate();
	}

	if (tech.isWaterWork())
	{
		changeWaterWorkCount(iChange);
		setLastRoundOfValidImprovementCacheUpdate();
	}

	for (int iI = 0; iI < GC.getNumRouteInfos(); iI++)
	{
		changeRouteChange(((RouteTypes)iI), (GC.getRouteInfo((RouteTypes)iI).getTechMovementChange(eTech) * iChange));
	}

	for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		changeExtraMoves(((DomainTypes)iI), (tech.getDomainExtraMoves(iI) * iChange));
	}

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		if (tech.isCommerceFlexible(iI))
		{
			changeCommerceFlexibleCount(((CommerceTypes)iI), iChange);
		}
	}

	for (int iI = 0; iI < GC.getNumTerrainInfos(); iI++)
	{
		if (tech.isTerrainTrade(iI))
		{
			changeTerrainTradeCount(((TerrainTypes)iI), iChange);
		}
	}

	if (tech.isRiverTrade())
	{
		changeRiverTradeCount(iChange);
	}

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		if (GC.getBuildingInfo((BuildingTypes)iI).getObsoleteTech() == eTech)
		{
			changeObsoleteBuildingCount((BuildingTypes)iI, iChange);
		}
		if (GC.getBuildingInfo((BuildingTypes)iI).getSpecialBuilding() != NO_SPECIALBUILDING
		&& GC.getSpecialBuildingInfo(GC.getBuildingInfo((BuildingTypes)iI).getSpecialBuilding()).getObsoleteTech() == eTech)
		{
			changeObsoleteBuildingCount((BuildingTypes)iI, iChange);
		}

		for (int iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
		{
			changeBuildingSpecialistChange(((BuildingTypes)iI), ((SpecialistTypes)iJ), (GC.getBuildingInfo((BuildingTypes)iI).getTechSpecialistChange(eTech, iJ) * iChange));
		}
	}

	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			changeImprovementYieldChange(((ImprovementTypes)iI), ((YieldTypes)iJ), (GC.getImprovementInfo((ImprovementTypes)iI).getTechYieldChanges(eTech, iJ) * iChange));
		}
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& playerX = GET_PLAYER((PlayerTypes)iI);

		if (playerX.isAliveAndTeam(getID()))
		{
			playerX.processTech(eTech, iChange);
		}
	}

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		const BonusTypes eBonus = pLoopPlot->getBonusType();

		if (eBonus != NO_BONUS && GC.getBonusInfo(eBonus).getTechReveal() == eTech)
		{
			pLoopPlot->updateYield();
			pLoopPlot->setLayoutDirty(true);
		}
	}

	for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		if (GC.getBuildInfo((BuildTypes)iI).getTechPrereq() == eTech)
		{
			setLastRoundOfValidImprovementCacheUpdate();
			if (GC.getBuildInfo((BuildTypes)iI).getRoute() != NO_ROUTE)
			{
				for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAliveAndTeam(getID()))
					{
						GET_PLAYER((PlayerTypes)iJ).processNewRoutes();
					}
				}
			}
			break;
		}
	}
	if (getLastRoundOfValidImprovementCacheUpdate() != GC.getGame().getGameTurn())
	{
		for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			if (GC.getImprovementInfo((ImprovementTypes)iI).getPrereqTech() == eTech)
			{
				setLastRoundOfValidImprovementCacheUpdate();
				break;
			}
		}
	}

	if (iChange > 0)
	{
		for (int iI = 0; iI < GC.getNumSpecialBuildingInfos(); ++iI)
		{
			if (eTech == GC.getSpecialBuildingInfo((SpecialBuildingTypes)iI).getTechPrereqAnyone())
			{
				GC.getGame().makeSpecialBuildingValid((SpecialBuildingTypes)iI, bAnnounce);
			}
		}
	}
}


void CvTeam::cancelDefensivePacts()
{
	PROFILE_EXTRA_FUNC();
	CLLNode<TradeData>* pNode;

	foreach_(CvDeal & kLoopDeal, GC.getGame().deals())
	{
		bool bCancelDeal = false;

		if ((GET_PLAYER(kLoopDeal.getFirstPlayer()).getTeam() == getID()) ||
			(GET_PLAYER(kLoopDeal.getSecondPlayer()).getTeam() == getID()))
		{
			for (pNode = kLoopDeal.headFirstTradesNode(); (pNode != NULL); pNode = kLoopDeal.nextFirstTradesNode(pNode))
			{
				if (pNode->m_data.m_eItemType == TRADE_DEFENSIVE_PACT)
				{
					bCancelDeal = true;
					break;
				}
			}

			if (!bCancelDeal)
			{
				for (pNode = kLoopDeal.headSecondTradesNode(); (pNode != NULL); pNode = kLoopDeal.nextSecondTradesNode(pNode))
				{
					if (pNode->m_data.m_eItemType == TRADE_DEFENSIVE_PACT)
					{
						bCancelDeal = true;
						break;
					}
				}
			}
		}

		if (bCancelDeal)
		{
			kLoopDeal.kill();
		}
	}
}


bool CvTeam::isFriendlyTerritory(TeamTypes eTeam) const
{
	if (eTeam == NO_TEAM)
	{
		return false;
	}

	if (eTeam == getID())
	{
		return true;
	}

	if (GET_TEAM(eTeam).isVassal(getID()))
	{
		return true;
	}

	if (isVassal(eTeam) && isOpenBorders(eTeam))
	{
		return true;
	}

	return false;
}

int CvTeam::getEspionagePointsAgainstTeam(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_aiEspionagePointsAgainstTeam[eIndex];
}

void CvTeam::setEspionagePointsAgainstTeam(TeamTypes eIndex, int iValue)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	if (iValue != getEspionagePointsAgainstTeam(eIndex))
	{
		m_aiEspionagePointsAgainstTeam[eIndex] = iValue;

		verifySpyUnitsValidPlot();
		GET_TEAM(eIndex).verifySpyUnitsValidPlot();
	}
}

void CvTeam::changeEspionagePointsAgainstTeam(TeamTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	setEspionagePointsAgainstTeam(eIndex, getEspionagePointsAgainstTeam(eIndex) + iChange);
}

int CvTeam::getEspionagePointsEver() const
{
	return m_iEspionagePointsEver;
}

void CvTeam::setEspionagePointsEver(int iValue)
{
	if (iValue != getEspionagePointsEver())
	{
		m_iEspionagePointsEver = iValue;
	}
}

void CvTeam::changeEspionagePointsEver(int iChange)
{
	setEspionagePointsEver(getEspionagePointsEver() + iChange);
}

int CvTeam::getCounterespionageTurnsLeftAgainstTeam(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_aiCounterespionageTurnsLeftAgainstTeam[eIndex];
}

void CvTeam::setCounterespionageTurnsLeftAgainstTeam(TeamTypes eIndex, int iValue)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	if (iValue != getCounterespionageTurnsLeftAgainstTeam(eIndex))
	{
		m_aiCounterespionageTurnsLeftAgainstTeam[eIndex] = iValue;

		gDLL->getInterfaceIFace()->setDirty(Espionage_Advisor_DIRTY_BIT, true);
	}
}

void CvTeam::changeCounterespionageTurnsLeftAgainstTeam(TeamTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	setCounterespionageTurnsLeftAgainstTeam(eIndex, getCounterespionageTurnsLeftAgainstTeam(eIndex) + iChange);
}

int CvTeam::getCounterespionageModAgainstTeam(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_aiCounterespionageModAgainstTeam[eIndex];
}

void CvTeam::setCounterespionageModAgainstTeam(TeamTypes eIndex, int iValue)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	if (iValue != getCounterespionageModAgainstTeam(eIndex))
	{
		m_aiCounterespionageModAgainstTeam[eIndex] = iValue;

		gDLL->getInterfaceIFace()->setDirty(Espionage_Advisor_DIRTY_BIT, true);
	}
}

void CvTeam::changeCounterespionageModAgainstTeam(TeamTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	setCounterespionageModAgainstTeam(eIndex, getCounterespionageModAgainstTeam(eIndex) + iChange);
}

void CvTeam::verifySpyUnitsValidPlot()
{
	PROFILE_EXTRA_FUNC();
	std::vector<CvUnit*> aUnits;

	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		const CvPlayer& player = GET_PLAYER((PlayerTypes)i);

		if (player.isAliveAndTeam(getID()))
		{
			foreach_(CvUnit * pUnit, player.units() | filtered(CvUnit::fn::plot() && CvUnit::fn::isSpy()))
			{
				const PlayerTypes eOwner = pUnit->plot()->getOwner();
				if (NO_PLAYER != eOwner && !player.canSpiesEnterBorders(eOwner))
				{
					aUnits.push_back(pUnit);
				}
			}
		}
	}
	algo::for_each(aUnits, bind(&CvUnit::jumpToNearestValidPlot, _1, true));
}


void CvTeam::setForceRevealedBonus(BonusTypes eBonus, bool bRevealed)
{
	PROFILE_EXTRA_FUNC();
	if (isForceRevealedBonus(eBonus) == bRevealed)
	{
		return;
	}

	for (int iI = 0; iI < GC.getMap().numPlots(); ++iI)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot->getBonusType() == eBonus)
		{
			if (pLoopPlot->getTeam() == getID())
			{
				pLoopPlot->updatePlotGroupBonus(false);
			}
		}
	}

	if (bRevealed)
	{
		m_aeRevealedBonuses.push_back(eBonus);
	}
	else
	{
		foreach_(BonusTypes & eRevealedBonus, m_aeRevealedBonuses)
		{
			if (eRevealedBonus == eBonus)
			{
				m_aeRevealedBonuses.erase(&eRevealedBonus);
				break;
			}
		}
	}

	for (int iI = 0; iI < GC.getMap().numPlots(); ++iI)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot->getBonusType() == eBonus)
		{
			if (pLoopPlot->getTeam() == getID())
			{
				pLoopPlot->updatePlotGroupBonus(true);
			}
		}
	}

	for (int iI = 0; iI < GC.getMap().numPlots(); ++iI)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot->getBonusType() == eBonus)
		{
			pLoopPlot->updateYield();
			pLoopPlot->setLayoutDirty(true);
		}
	}
}

bool CvTeam::isForceRevealedBonus(BonusTypes eBonus) const
{
	PROFILE_EXTRA_FUNC();
	std::vector<BonusTypes>::const_iterator it;

	for (it = m_aeRevealedBonuses.begin(); it != m_aeRevealedBonuses.end(); ++it)
	{
		if (*it == eBonus)
		{
			return true;
		}
	}

	return false;
}

int CvTeam::countNumHumanGameTurnActive() const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		const CvPlayer& playerX = GET_PLAYER((PlayerTypes)iI);

		if (playerX.isHumanPlayer() && playerX.getTeam() == getID())
		{
			if (playerX.isTurnActive())
			{
				++iCount;
			}
		}
	}

	return iCount;
}

void CvTeam::setTurnActive(bool bNewValue, bool bDoTurn)
{
	PROFILE_EXTRA_FUNC();
	FAssert(GC.getGame().isSimultaneousTeamTurns());

	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(getID()))
		{
			GET_PLAYER((PlayerTypes)i).setTurnActive(bNewValue, bDoTurn);
		}
	}
}

bool CvTeam::isTurnActive() const
{
	PROFILE_EXTRA_FUNC();
	FAssert(GC.getGame().isSimultaneousTeamTurns());

	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(getID())
		&& GET_PLAYER((PlayerTypes)i).isTurnActive())
		{
			return true;
		}
	}
	return false;
}


void CvTeam::read(FDataStreamBase* pStream)
{
	PROFILE_EXTRA_FUNC();
	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	// Init data before load
	reset();

	WRAPPER_READ(wrapper, "CvTeam", &m_iNumMembers);
	WRAPPER_READ(wrapper, "CvTeam", &m_iAliveCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iEverAliveCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iNumCities);
	WRAPPER_READ(wrapper, "CvTeam", &m_iTotalPopulation);
	WRAPPER_READ(wrapper, "CvTeam", &m_iTotalLand);
	WRAPPER_READ(wrapper, "CvTeam", &m_iNukeInterception);
	WRAPPER_READ(wrapper, "CvTeam", &m_iExtraWaterSeeFromCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iMapTradingCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iTechTradingCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iGoldTradingCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iOpenBordersTradingCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iDefensivePactTradingCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iPermanentAllianceTradingCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iVassalTradingCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iBridgeBuildingCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iIrrigationCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iIgnoreIrrigationCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iWaterWorkCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iVassalPower);
	WRAPPER_READ(wrapper, "CvTeam", &m_iMasterPower);
	WRAPPER_READ(wrapper, "CvTeam", &m_iEnemyWarWearinessModifier);
	WRAPPER_READ(wrapper, "CvTeam", &m_iRiverTradeCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iEspionagePointsEver);

	WRAPPER_READ(wrapper, "CvTeam", &m_bMapCentering);
	WRAPPER_READ(wrapper, "CvTeam", &m_bCapitulated);

	WRAPPER_READ(wrapper, "CvTeam", (int*)&m_eID);

	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_aiStolenVisibilityTimer);

	//	Format change - we now store 100 times the actual value, but no need to change the save format - just
	//	convert on load and save
	WRAPPER_READ_ARRAY_DECORATED(wrapper, "CvTeam", MAX_TEAMS, m_aiWarWearinessTimes100, "m_aiWarWeariness");

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiWarWearinessTimes100[iI] *= 100;
	}

	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_aiTechShareCount);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_aiEspionagePointsAgainstTeam);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_aiCounterespionageTurnsLeftAgainstTeam);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_aiCounterespionageModAgainstTeam);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", NUM_COMMERCE_TYPES, m_aiCommerceFlexibleCount);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", NUM_DOMAIN_TYPES, m_aiExtraMoves);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_VOTE_SOURCES, GC.getNumVoteSourceInfos(), m_aiForceTeamVoteEligibilityCount);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abHasMet);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abAtWar);
	//	Koshling - fix bugged state where the ganme thinks a team is at war with itself!
	if (m_abAtWar[getID()])
	{
		FErrorMsg("Team flagged as at war with itself (correcting)");
		m_abAtWar[getID()] = false;
	}
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abPermanentWarPeace);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abOpenBorders);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abDefensivePact);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abForcePeace);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abVassal);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_VICTORIES, GC.getNumVictoryInfos(), m_abCanLaunch);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abIsRebelAgainst);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_ROUTES, GC.getNumRouteInfos(), m_paiRouteChange);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_PROJECTS, GC.getNumProjectInfos(), m_paiProjectCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_PROJECTS, GC.getNumProjectInfos(), m_paiProjectDefaultArtTypes);

	//project art types
	for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_PROJECTS); i++)
	{
		int temp;
		ProjectTypes eNewProjectType = (ProjectTypes)wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_PROJECTS, i, true);
		if (eNewProjectType != -1)
		{
			for (int j = 0; j < m_paiProjectCount[eNewProjectType]; j++)
			{
				//	Decorasted this with a rather obscure name to match the export since when I fixed this
				//	bug I didn't want the export format to chnage uneccessarily
				WRAPPER_READ_DECORATED(wrapper, "CvTeam", &temp, "m_pavProjectArtTypes[][]");
				m_pavProjectArtTypes[eNewProjectType].push_back(temp);
			}
		}
	}

	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_PROJECTS, GC.getNumProjectInfos(), m_paiProjectMaking);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiBuildingCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiObsoleteBuildingCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiResearchProgress);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiTechCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_TERRAINS, GC.getNumTerrainInfos(), m_paiTerrainTradeCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_VICTORIES, GC.getNumVictoryInfos(), m_aiVictoryCountdown);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_pabHasTech);

	for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_IMPROVEMENTS); ++i)
	{
		int	newIndex = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_IMPROVEMENTS, i, true);

		if (newIndex != -1)
		{
			WRAPPER_READ_ARRAY(wrapper, "CvTeam", NUM_YIELD_TYPES, m_ppaaiImprovementYieldChange[newIndex]);
		}
		else
		{
			//	Consume the values
			WRAPPER_SKIP_ELEMENT(wrapper, "CvTeam", m_ppaaiImprovementYieldChange[i], SAVE_VALUE_ANY);
		}
	}

	// Toffer - Read vectors
	{
		short iSize = 0;
		short iType = -1;
		// Tech
		WRAPPER_READ_DECORATED(wrapper, "CvTeam", &iSize, "NoTradeTechSize");
		for (short i = 0; i < iSize; ++i)
		{
			WRAPPER_READ_DECORATED(wrapper, "CvTeam", &iType, "NoTradeTechIndex");
			iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_TECHS, iType, true));

			if (iType > -1)
			{
				m_vNoTradeTech.push_back(iType);
			}
		}
	}
	// Toffer - Read maps
	{
		short iSize = 0;
		short iType;
		uint32_t iCountU;
		// Unit counters
		WRAPPER_READ_DECORATED(wrapper, "CvTeam", &iSize, "UnitCountSize");
		while (iSize-- > 0)
		{
			WRAPPER_READ_DECORATED(wrapper, "CvTeam", &iType, "UnitCountType");
			WRAPPER_READ_DECORATED(wrapper, "CvTeam", &iCountU, "UnitCount");
			iType = static_cast<short>(wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_UNITS, iType, true));

			if (iType > -1)
			{
				m_unitCount.insert(std::make_pair(iType, iCountU));
			}
		}
	}

	uint32_t iSize = 0;
	m_aeRevealedBonuses.clear();
	WRAPPER_READ(wrapper, "CvTeam", &iSize);
	for (uint32_t i = 0; i < iSize; ++i)
	{
		BonusTypes eBonus;
		WRAPPER_READ_CLASS_ENUM(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_BONUSES, (int*)&eBonus);
		m_aeRevealedBonuses.push_back(eBonus);
	}
	WRAPPER_READ(wrapper, "CvTeam", &m_iCanPassPeaksCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iMoveFastPeaksCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iCanFoundOnPeaksCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iEmbassyTradingCount);

	WRAPPER_READ(wrapper, "CvTeam", &m_iLimitedBordersTradingCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iCanFarmDesertCount);

	WRAPPER_READ(wrapper, "CvTeam", &m_iRebaseAnywhereCount);
	WRAPPER_READ(wrapper, "CvTeam", &m_iForeignTradeModifier);
	WRAPPER_READ(wrapper, "CvTeam", &m_iTradeModifier);
	WRAPPER_READ(wrapper, "CvTeam", &m_iTradeMissionModifier);
	WRAPPER_READ(wrapper, "CvTeam", &m_iCorporationRevenueModifier);
	WRAPPER_READ(wrapper, "CvTeam", &m_iCorporationMaintenanceModifier);

	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abEmbassy);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abLimitedBorders);
	WRAPPER_READ_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abFreeTrade);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiFreeSpecialistCount);

	for (int i = 0; i < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_BUILDINGS); ++i)
	{
		int	newIndex = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDINGS, i, true);

		if (newIndex != -1)
		{
			WRAPPER_READ_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_ppiBuildingSpecialistChange[newIndex]);
			WRAPPER_READ_ARRAY(wrapper, "CvTeam", NUM_COMMERCE_TYPES, m_ppiBuildingCommerceModifier[newIndex]);
		}
		else
		{
			// Consume the values
			WRAPPER_SKIP_ELEMENT(wrapper, "CvTeam", m_ppiBuildingSpecialistChange[newIndex], SAVE_VALUE_TYPE_CLASS_INT_ARRAY);
			WRAPPER_SKIP_ELEMENT(wrapper, "CvTeam", m_ppiBuildingCommerceModifier[newIndex], SAVE_VALUE_TYPE_INT_ARRAY);
		}
	}

	m_Properties.readWrapper(pStream);

	WRAPPER_READ(wrapper, "CvTeam", &m_iLastRoundOfValidImprovementCacheUpdate);

	WRAPPER_READ_OBJECT_END(wrapper);
}


void CvTeam::write(FDataStreamBase* pStream)
{
	PROFILE_EXTRA_FUNC();
	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE(wrapper, "CvTeam", m_iNumMembers);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iAliveCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iEverAliveCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iNumCities);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iTotalPopulation);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iTotalLand);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iNukeInterception);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iExtraWaterSeeFromCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iMapTradingCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iTechTradingCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iGoldTradingCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iOpenBordersTradingCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iDefensivePactTradingCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iPermanentAllianceTradingCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iVassalTradingCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iBridgeBuildingCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iIrrigationCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iIgnoreIrrigationCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iWaterWorkCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iVassalPower);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iMasterPower);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iEnemyWarWearinessModifier);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iRiverTradeCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iEspionagePointsEver);
	WRAPPER_WRITE(wrapper, "CvTeam", m_bMapCentering);
	WRAPPER_WRITE(wrapper, "CvTeam", m_bCapitulated);
	WRAPPER_WRITE(wrapper, "CvTeam", m_eID);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_aiStolenVisibilityTimer);

	//	Format change - we now store 100 times the actual value, but no need to change the save format - just
	//	convert on load and save
	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiWarWearinessTimes100[iI] /= 100;
	}

	WRAPPER_WRITE_ARRAY_DECORATED(wrapper, "CvTeam", MAX_TEAMS, m_aiWarWearinessTimes100, "m_aiWarWeariness");
	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiWarWearinessTimes100[iI] *= 100;
	}

	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_aiTechShareCount);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_aiEspionagePointsAgainstTeam);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_aiCounterespionageTurnsLeftAgainstTeam);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_aiCounterespionageModAgainstTeam);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", NUM_COMMERCE_TYPES, m_aiCommerceFlexibleCount);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", NUM_DOMAIN_TYPES, m_aiExtraMoves);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_VOTE_SOURCES, GC.getNumVoteSourceInfos(), m_aiForceTeamVoteEligibilityCount);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abHasMet);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abAtWar);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abPermanentWarPeace);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abOpenBorders);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abDefensivePact);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abForcePeace);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abVassal);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_VICTORIES, GC.getNumVictoryInfos(), m_abCanLaunch);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abIsRebelAgainst);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_ROUTES, GC.getNumRouteInfos(), m_paiRouteChange);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_PROJECTS, GC.getNumProjectInfos(), m_paiProjectCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_PROJECTS, GC.getNumProjectInfos(), m_paiProjectDefaultArtTypes);

	//project art types
	for (int i = 0; i < GC.getNumProjectInfos(); i++)
	{
		for (int j = 0; j < m_paiProjectCount[i]; j++)
		{
			WRAPPER_WRITE(wrapper, "CvTeam", m_pavProjectArtTypes[i][j]);
		}
	}

	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_PROJECTS, GC.getNumProjectInfos(), m_paiProjectMaking);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiBuildingCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiObsoleteBuildingCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiResearchProgress);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiTechCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_TERRAINS, GC.getNumTerrainInfos(), m_paiTerrainTradeCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_VICTORIES, GC.getNumVictoryInfos(), m_aiVictoryCountdown);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_pabHasTech);

	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", NUM_YIELD_TYPES, m_ppaaiImprovementYieldChange[iI]);
	}

	// Toffer - Write vectors
	{
		// Tech
		WRAPPER_WRITE_DECORATED(wrapper, "CvTeam", (short)m_vNoTradeTech.size(), "NoTradeTechSize");
		for (std::vector<short>::const_iterator it = m_vNoTradeTech.begin(); it != m_vNoTradeTech.end(); ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvTeam", *it, "NoTradeTechIndex");
		}
	}
	// Toffer - Write maps
	{
		// Unit counters
		WRAPPER_WRITE_DECORATED(wrapper, "CvTeam", (short)m_unitCount.size(), "UnitCountSize");
		for (std::map<short, uint32_t>::const_iterator it = m_unitCount.begin(), itEnd = m_unitCount.end(); it != itEnd; ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvTeam", it->first, "UnitCountType");
			WRAPPER_WRITE_DECORATED(wrapper, "CvTeam", it->second, "UnitCount");
		}
	}

	WRAPPER_WRITE_DECORATED(wrapper, "CvTeam", m_aeRevealedBonuses.size(), "iSize");
	foreach_(const BonusTypes eBonus, m_aeRevealedBonuses)
	{
		WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_BONUSES, eBonus, "eBonus");
	}

	WRAPPER_WRITE(wrapper, "CvTeam", m_iCanPassPeaksCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iMoveFastPeaksCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iCanFoundOnPeaksCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iEmbassyTradingCount);

	WRAPPER_WRITE(wrapper, "CvTeam", m_iLimitedBordersTradingCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iCanFarmDesertCount);

	WRAPPER_WRITE(wrapper, "CvTeam", m_iRebaseAnywhereCount);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iForeignTradeModifier);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iTradeModifier);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iTradeMissionModifier);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iCorporationRevenueModifier);
	WRAPPER_WRITE(wrapper, "CvTeam", m_iCorporationMaintenanceModifier);

	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abEmbassy);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abLimitedBorders);
	WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", MAX_TEAMS, m_abFreeTrade);

	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_paiFreeSpecialistCount);

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvTeam", REMAPPED_CLASS_TYPE_SPECIALISTS, GC.getNumSpecialistInfos(), m_ppiBuildingSpecialistChange[iI]);
		WRAPPER_WRITE_ARRAY(wrapper, "CvTeam", NUM_COMMERCE_TYPES, m_ppiBuildingCommerceModifier[iI]);
	}

	m_Properties.writeWrapper(pStream);

	WRAPPER_WRITE(wrapper, "CvTeam", m_iLastRoundOfValidImprovementCacheUpdate);

	WRAPPER_WRITE_OBJECT_END(wrapper);
}


// CACHE: cache frequently used values
///////////////////////////////////////
bool CvTeam::hasShrine(ReligionTypes eReligion) const
{
	if (eReligion != NO_RELIGION)
	{
		CvCity* pHolyCity = GC.getGame().getHolyCity(eReligion);

		if (pHolyCity != NULL && GET_PLAYER(pHolyCity->getOwner()).getTeam() == getID())
			return pHolyCity->hasShrine(eReligion);
	}

	return false;
}

void CvTeam::getCompletedSpaceshipProjects(std::map<ProjectTypes, int>& mapProjects) const
{
	PROFILE_EXTRA_FUNC();
	for (int i = 0; i < GC.getNumProjectInfos(); i++)
	{
		if (GC.getProjectInfo((ProjectTypes)i).isSpaceship())
		{
			mapProjects[(ProjectTypes)i] = getProjectCount((ProjectTypes)i);
		}
	}
}

int CvTeam::getProjectPartNumber(ProjectTypes eProject, bool bAssert) const
{
	PROFILE_EXTRA_FUNC();
	int iNumBuilt = getProjectCount(eProject);
	for (int i = 0; i < iNumBuilt; i++)
	{
		int artType = getProjectArtType(eProject, i);
		if (artType < 0)
		{
			return i;
		}
	}

	//didn't find empty part number
	if (bAssert)
	{
		FErrorMsg("Unknown part number.");
	}

	//return the last one
	return std::min(iNumBuilt, GC.getProjectInfo(eProject).getMaxTeamInstances() - 1);
}

bool CvTeam::hasLaunched() const
{
	const VictoryTypes spaceVictory = GC.getGame().getSpaceVictory();
	return spaceVictory != NO_VICTORY && getVictoryCountdown(spaceVictory) >= 0;
}

bool CvTeam::isCanPassPeaks() const
{
	return (getCanPassPeaksCount() > 0);
}

int CvTeam::getCanPassPeaksCount() const
{
	return m_iCanPassPeaksCount;
}

void CvTeam::changeCanPassPeaksCount(int iChange)
{
	m_iCanPassPeaksCount += iChange;
	FASSERT_NOT_NEGATIVE(getCanPassPeaksCount());
}

bool CvTeam::isMoveFastPeaks() const
{
	return (getMoveFastPeaksCount() > 0);
}

int CvTeam::getMoveFastPeaksCount() const
{
	return m_iMoveFastPeaksCount;
}

void CvTeam::changeMoveFastPeaksCount(int iChange)
{
	m_iMoveFastPeaksCount += iChange;
	FASSERT_NOT_NEGATIVE(getMoveFastPeaksCount());
}

bool CvTeam::isCanFoundOnPeaks() const
{
	return (getCanFoundOnPeaksCount() > 0);
}

int CvTeam::getCanFoundOnPeaksCount() const
{
	return m_iCanFoundOnPeaksCount;
}

void CvTeam::changeCanFoundOnPeaksCount(int iChange)
{
	m_iCanFoundOnPeaksCount += iChange;
	FASSERT_NOT_NEGATIVE(getCanFoundOnPeaksCount());
}

int CvTeam::getRebaseAnywhereCount() const
{
	return m_iRebaseAnywhereCount;
}

bool CvTeam::isRebaseAnywhere() const
{
	return m_iRebaseAnywhereCount > 0;
}

void CvTeam::changeRebaseAnywhereCount(int iChange)
{
	m_iRebaseAnywhereCount += iChange;
}


void CvTeam::AI_setAssignWorkDirtyInEveryPlayerCityWithActiveBuilding(BuildingTypes eBuilding)
{
	PROFILE_EXTRA_FUNC();
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(getID()))
		{
			algo::for_each(
				GET_PLAYER((PlayerTypes)i).cities() | filtered(CvCity::fn::isActiveBuilding(eBuilding)),
				CvCity::fn::AI_setAssignWorkDirty(true)
			);
		}
	}
}


int CvTeam::getCanFarmDesertCount() const
{
	return m_iCanFarmDesertCount;
}

bool CvTeam::isCanFarmDesert() const
{
	return (getCanFarmDesertCount() > 0);
}

void CvTeam::changeCanFarmDesertCount(int iChange)
{
	m_iCanFarmDesertCount += iChange;
	FASSERT_NOT_NEGATIVE(getCanFarmDesertCount());
}

int CvTeam::getLimitedBordersTradingCount() const
{
	return m_iLimitedBordersTradingCount;
}

bool CvTeam::isLimitedBordersTrading() const
{
	return (getLimitedBordersTradingCount() > 0);
}

void CvTeam::changeLimitedBordersTradingCount(int iChange)
{
	m_iLimitedBordersTradingCount += iChange;
	FASSERT_NOT_NEGATIVE(getLimitedBordersTradingCount());
}

void CvTeam::signLimitedBorders(TeamTypes eTeam)
{
	CLinkList<TradeData> ourList;
	CLinkList<TradeData> theirList;
	TradeData item;

	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);
	FAssert(eTeam != getID());

	if (!isAtWar(eTeam) && (getID() != eTeam) && (!isOpenBorders(eTeam)))
	{
		setTradeItem(&item, TRADE_RITE_OF_PASSAGE);

		if (GET_PLAYER(getLeaderID()).canTradeItem(GET_TEAM(eTeam).getLeaderID(), item) && GET_PLAYER(GET_TEAM(eTeam).getLeaderID()).canTradeItem(getLeaderID(), item))
		{
			ourList.clear();
			theirList.clear();

			ourList.insertAtEnd(item);
			theirList.insertAtEnd(item);

			GC.getGame().implementDeal(getLeaderID(), (GET_TEAM(eTeam).getLeaderID()), &ourList, &theirList);
		}
	}
}

bool CvTeam::canSignOpenBorders(TeamTypes eTeam) const
{
	if (GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY) && !isHasEmbassy(eTeam))
	{
		return false;
	}
	return true;
}

void CvTeam::sendAmbassador(TeamTypes eTeam)
{
	CLinkList<TradeData> ourList;
	CLinkList<TradeData> theirList;
	TradeData item;

	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);
	FAssert(eTeam != getID());

	if (!isAtWar(eTeam) && (getID() != eTeam))
	{
		setTradeItem(&item, TRADE_EMBASSY);

		if (GET_PLAYER(getLeaderID()).canTradeItem(GET_TEAM(eTeam).getLeaderID(), item) && GET_PLAYER(GET_TEAM(eTeam).getLeaderID()).canTradeItem(getLeaderID(), item))
		{
			ourList.clear();
			theirList.clear();

			ourList.insertAtEnd(item);
			theirList.insertAtEnd(item);

			GC.getGame().implementDeal(getLeaderID(), (GET_TEAM(eTeam).getLeaderID()), &ourList, &theirList);
		}
	}
}


bool CvTeam::isLimitedBorders(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_abLimitedBorders[eIndex];
}


bool CvTeam::isHasEmbassy(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_abEmbassy[eIndex];
}

void CvTeam::setHasEmbassy(TeamTypes eIndex, bool bNewValue)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	if (isHasEmbassy(eIndex) != bNewValue)
	{
		for (int iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				const CvCity* pCapital = GET_PLAYER((PlayerTypes)iI).getCapitalCity();
				if (pCapital != NULL)
				{
					pCapital->plot()->updateSight(false, true);
				}
			}
		}
		m_abEmbassy[eIndex] = bNewValue;
		for (int iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				const CvCity* pCapital = GET_PLAYER((PlayerTypes)iI).getCapitalCity();
				if (pCapital != NULL)
				{
					pCapital->plot()->updateSight(true, true);
				}
			}
		}
	}
}

int CvTeam::getEmbassyTradingCount() const
{
	return m_iEmbassyTradingCount;
}

bool CvTeam::isEmbassyTrading() const
{
	return (getEmbassyTradingCount() > 0);
}

void CvTeam::changeEmbassyTradingCount(int iChange)
{
	if (iChange != 0)
	{
		m_iEmbassyTradingCount += iChange;
	}
}

void CvTeam::ObsoletePromotions(TechTypes eObsoleteTech)
{
	PROFILE_EXTRA_FUNC();
	if (eObsoleteTech != NO_TECH)
	{
		for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			if (GC.getPromotionInfo((PromotionTypes)iI).getObsoleteTech() == eObsoleteTech)
			{
				for (int i = 0; i < MAX_PLAYERS; ++i)
				{
					if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(getID()))
					{
						algo::for_each(GET_PLAYER((PlayerTypes)i).units(), CvUnit::fn::checkPromotionObsoletion());
					}
				}
				break;
			}
		}
	}
}

void CvTeam::ObsoleteCorporations(TechTypes eObsoleteTech)
{
	PROFILE_EXTRA_FUNC();
	bool bValid = false;
	if (eObsoleteTech != NO_TECH)
	{
		for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
		{
			if (GC.getCorporationInfo((CorporationTypes)iI).getObsoleteTech() == eObsoleteTech)
			{
				bValid = true;
				break;
			}
		}
		if (bValid)
		{
			for (int i = 0; i < MAX_PLAYERS; ++i)
			{
				if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(getID()))
				{
					foreach_(CvCity * pLoopCity, GET_PLAYER((PlayerTypes)i).cities())
					{
						for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
						{
							if (pLoopCity->isHasCorporation((CorporationTypes)iI) && GC.getCorporationInfo((CorporationTypes)iI).getObsoleteTech() == eObsoleteTech)
							{
								pLoopCity->setHasCorporation((CorporationTypes)iI, false, false, false);
								//Remove the Corp HQ Building
								if (pLoopCity->isHeadquarters((CorporationTypes)iI))
								{
									for (int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
									{
										if ((CorporationTypes)GC.getBuildingInfo((BuildingTypes)iJ).getGlobalCorporationCommerce() == ((CorporationTypes)iI)
										|| GC.getBuildingInfo((BuildingTypes)iJ).getFoundsCorporation() == ((CorporationTypes)iI)
										|| GC.getBuildingInfo((BuildingTypes)iJ).getPrereqCorporation() == ((CorporationTypes)iI))
										{
											pLoopCity->changeHasBuilding((BuildingTypes)iJ, false);
										}
									}
									GC.getGame().setHeadquarters((CorporationTypes)iI, NULL, false);
									//Be Sure to Create a New Headquarters, ASAP
									GC.getGame().doFoundCorporation((CorporationTypes)iI, true);
								}
							}
						}
					}
				}
			}
		}
	}
}

int CvTeam::getTradeModifier() const
{
	return m_iTradeModifier;
}
void CvTeam::changeTradeModifier(int iChange)
{
	PROFILE_EXTRA_FUNC();
	if (iChange != 0)
	{
		m_iTradeModifier += iChange;
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(getID()))
			{
				GET_PLAYER((PlayerTypes)i).updateTradeRoutes();
			}
		}
	}
}

int CvTeam::getForeignTradeModifier() const
{
	return m_iForeignTradeModifier;
}

void CvTeam::changeForeignTradeModifier(int iChange)
{
	PROFILE_EXTRA_FUNC();
	if (iChange != 0)
	{
		m_iForeignTradeModifier += iChange;
		for (int i = 0; i < MAX_PC_PLAYERS; i++)
		{
			if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(getID()))
			{
				GET_PLAYER((PlayerTypes)i).updateTradeRoutes();
			}
		}
	}
}

int CvTeam::getTradeMissionModifier() const
{
	return m_iTradeMissionModifier;
}
void CvTeam::changeTradeMissionModifier(int iChange)
{
	m_iTradeMissionModifier += iChange;
}

int CvTeam::getCorporationRevenueModifier() const
{
	return m_iCorporationRevenueModifier;
}
void CvTeam::changeCorporationRevenueModifier(int iChange)
{
	PROFILE_EXTRA_FUNC();
	if (iChange != 0)
	{
		m_iCorporationRevenueModifier += iChange;

		for (int i = 0; i < MAX_PC_PLAYERS; ++i)
		{
			if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(getID()))
			{
				algo::for_each(GET_PLAYER((PlayerTypes)i).cities(), CvCity::fn::updateCorporation());
			}
		}
	}
}

int CvTeam::getCorporationMaintenanceModifier() const
{
	return m_iCorporationMaintenanceModifier;
}

void CvTeam::changeCorporationMaintenanceModifier(int iChange)
{
	PROFILE_EXTRA_FUNC();
	if (iChange != 0)
	{
		m_iCorporationMaintenanceModifier += iChange;

		for (int i = 0; i < MAX_PC_PLAYERS; ++i)
		{
			if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(getID()))
			{
				algo::for_each(GET_PLAYER((PlayerTypes)i).cities(), CvCity::fn::updateCorporation());
			}
		}
	}
}

int64_t CvTeam::getTotalVictoryScore() const
{
	PROFILE_EXTRA_FUNC();
	int64_t iTotalVictoryScore = 0;

	int64_t globalCulture = 0;
	int globalPopulation = 0;
	int globalWonderScore = 0;

	int teamWonderScore = 0;

	int totalTeamReligion = 0;
	int totalTeamMonumentalCities = 0;

	int64_t globalPowerHistory = 0;
	int64_t teamPowerHistory = 0;

	for (int iK = 0; iK < GC.getNumBuildingInfos(); iK++)
	{
		if (GC.getBuildingInfo((BuildingTypes)iK).getMaxGlobalInstances() == 1)
		{
			globalWonderScore++;
		}
	}

	// Get the Religion Info First
	// By definition, global religion percent is 100, so we don't need a variable for it.
	// Note: This detects whether the TEAM has the holy city.
	for (int iK = 0; iK < GC.getNumReligionInfos(); iK++)
	{
		if (hasHolyCity((ReligionTypes)iK))
		{
			// Player has holy city
			const int tempReligion = GC.getGame().calculateReligionPercent((ReligionTypes)iK);
			if (tempReligion > totalTeamReligion)
			{
				totalTeamReligion = tempReligion;
			}
		}
	}

	// Get land, population, culture totals for player and globally.
	// Also get the starship launches and diplovictories achieved.
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			//Calculate global totals while looping through

			globalCulture += GET_PLAYER((PlayerTypes)iI).getCulture();
			globalPopulation += GET_PLAYER((PlayerTypes)iI).getTotalPopulation();

			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				teamWonderScore += GET_PLAYER((PlayerTypes)iI).getSevoWondersScore(0);
			}
		}
	}

	// Get the power history sums
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			//Calculate global totals while looping through
			int64_t tempPower = 0;
			for (int iL = GC.getGame().getGameTurn(); iL > -1; iL--)
			{
				tempPower += GET_PLAYER((PlayerTypes)iI).getPowerHistory(iL);
			}

			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
			{
				teamPowerHistory += tempPower;
			}
			globalPowerHistory += tempPower;
		}
	}

	// Get the number of monumental cities owned by this team
	if (GC.getGame().culturalVictoryValid())
	{
		const CultureLevelTypes eVictoryLevel = GC.getGame().culturalVictoryCultureLevel();

		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
			{
				foreach_(const CvCity * pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
				{
					if (pLoopCity->getCultureLevel() >= eVictoryLevel)
					{
						totalTeamMonumentalCities++;
					}
				}
			}
		}
	}

	// Add the WonderScore component
	if (globalWonderScore > 0)
	{
		iTotalVictoryScore += teamWonderScore * 100 / globalWonderScore;
	}

	// Add the population score component
	if (globalPopulation > 0)
	{
		iTotalVictoryScore += getTotalPopulation() * 100 / globalPopulation;
	}

	// Add the land score component
	const int globalLand = GC.getMap().getLandPlots();

	if (globalLand > 0)
	{
		iTotalVictoryScore += getTotalLand() * 100 / globalLand;
	}

	// Add the culture score component
	if (globalCulture > 0)
	{
		uint64_t iTotalCulture = countTotalCulture();

		if (iTotalCulture > MAX_INT / 100)
		{
			iTotalVictoryScore += countTotalCulture() / (globalCulture / 100);
		}
		else
		{
			iTotalVictoryScore += countTotalCulture() * 100 / globalCulture;
		}
	}

	// Add the monumental cities component
	if (totalTeamMonumentalCities > 0)
	{
		iTotalVictoryScore += 30 * totalTeamMonumentalCities;
	}

	// Add the Power component
	if (globalPowerHistory > 0)
	{
		iTotalVictoryScore += (teamPowerHistory * 100 / globalPowerHistory);
	}

	// Add the Religion component
	iTotalVictoryScore += totalTeamReligion;

	//Starship points!  Big money.
	if (GC.getGame().getStarshipLaunched(getID()))
	{
		iTotalVictoryScore += 100;
	}

	return iTotalVictoryScore;
}


int CvTeam::getBuildingSpecialistChange(BuildingTypes eIndex1, SpecialistTypes eIndex2) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex1);
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex2);
	return m_ppiBuildingSpecialistChange[eIndex1][eIndex2];
}

void CvTeam::changeBuildingSpecialistChange(BuildingTypes eIndex1, SpecialistTypes eIndex2, int iChange)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex1);
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex2);

	if (iChange != 0)
	{
		const int iOldValue = getBuildingSpecialistChange(eIndex1, eIndex2);
		m_ppiBuildingSpecialistChange[eIndex1][eIndex2] += iChange;

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
			{
				foreach_(CvCity * pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
				{
					// remove the old
					pLoopCity->updateMaxSpecialistCount(eIndex1, eIndex2, -iOldValue);
					// set the new
					pLoopCity->updateMaxSpecialistCount(eIndex1, eIndex2, getBuildingSpecialistChange(eIndex1, eIndex2));
				}
			}
		}
	}
}


void CvTeam::setLimitedBorders(TeamTypes eIndex, bool bNewValue)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	if (isLimitedBorders(eIndex) != bNewValue)
	{
		const bool bOldFreeTrade = isFreeTrade(eIndex);

		m_abLimitedBorders[eIndex] = bNewValue;

		GC.getMap().verifyUnitValidPlot();

		if ((getID() == GC.getGame().getActiveTeam()) || (eIndex == GC.getGame().getActiveTeam()))
		{
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		}

		if (bOldFreeTrade != isFreeTrade(eIndex))
		{
			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
				{
					GET_PLAYER((PlayerTypes)iI).updateTradeRoutes();
				}
			}
		}
	}
}

int CvTeam::getFreeSpecialistCount(SpecialistTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex);
	return m_paiFreeSpecialistCount[eIndex];
}

void CvTeam::setFreeSpecialistCount(SpecialistTypes eIndex, int iNewValue)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eIndex);

	const int iOldValue = getFreeSpecialistCount(eIndex);

	if (iOldValue != iNewValue)
	{
		m_paiFreeSpecialistCount[eIndex] = iNewValue;
		FASSERT_NOT_NEGATIVE(getFreeSpecialistCount(eIndex));

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
			{
				algo::for_each(GET_PLAYER((PlayerTypes)iI).cities(), CvCity::fn::changeFreeSpecialistCount(eIndex, 0));
			}
		}
	}
}

void CvTeam::changeFreeSpecialistCount(SpecialistTypes eIndex, int iChange)
{
	setFreeSpecialistCount(eIndex, (getFreeSpecialistCount(eIndex) + iChange));
}


bool CvTeam::isAnyVassal() const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (iI != getID() && GET_TEAM((TeamTypes)iI).isAlive() && isVassal((TeamTypes)iI))
		{
			return true;
		}
	}
	return false;
}

ImprovementTypes CvTeam::getImprovementUpgrade(ImprovementTypes eImprovement) const
{
	const ImprovementTypes eUpgrade = GC.getImprovementInfo(eImprovement).getImprovementUpgrade();

	if (eUpgrade != NO_IMPROVEMENT && GC.getImprovementInfo(eUpgrade).getPrereqTech() != NO_TECH
	&& !isHasTech(GC.getImprovementInfo(eUpgrade).getPrereqTech()))
	{
		return NO_IMPROVEMENT;
	}
	return eUpgrade;
}

ImprovementTypes CvTeam::finalImprovementUpgrade(ImprovementTypes eImprovement) const
{
	if (getImprovementUpgrade(eImprovement) != NO_IMPROVEMENT)
	{
		return finalImprovementUpgrade(getImprovementUpgrade(eImprovement));
	}
	return eImprovement;
}

bool CvTeam::isFreeTradeAgreement(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_abFreeTrade[eIndex];
}


void CvTeam::setFreeTradeAgreement(TeamTypes eIndex, bool bNewValue)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	if (isFreeTradeAgreement(eIndex) != bNewValue)
	{
		const bool bOldFreeTrade = isFreeTrade(eIndex);

		m_abFreeTrade[eIndex] = bNewValue;

		if (getID() == GC.getGame().getActiveTeam() || eIndex == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		}

		if (bOldFreeTrade != isFreeTrade(eIndex))
		{
			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
				{
					GET_PLAYER((PlayerTypes)iI).updateTradeRoutes();
				}
			}
		}
	}
}

void CvTeam::AI_updateBonusValue(BonusTypes eBonus)
{
	PROFILE_EXTRA_FUNC();
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAliveAndTeam(getID()))
		{
			GET_PLAYER((PlayerTypes)i).AI_updateBonusValue(eBonus);
		}
	}
}

void CvTeam::addPropertiesAllCities(const CvProperties* pProp)
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			GET_PLAYER((PlayerTypes)iI).addPropertiesAllCities(pProp);
		}
	}
}

void CvTeam::subtractPropertiesAllCities(const CvProperties* pProp)
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			GET_PLAYER((PlayerTypes)iI).subtractPropertiesAllCities(pProp);
		}
	}
}

void CvTeam::recalculateModifiers()
{
	PROFILE_EXTRA_FUNC();
	// Clear player modifiers
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			GET_PLAYER((PlayerTypes)iI).clearModifierTotals();
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiTechShareCount[iI] = 0;
	}

	//	Clear team level info (from techs)
	m_iExtraWaterSeeFromCount = 0;
	m_iCanPassPeaksCount = 0;
	m_iMoveFastPeaksCount = 0;
	m_iCanFoundOnPeaksCount = 0;
	m_iRebaseAnywhereCount = 0;
	m_iForeignTradeModifier = 0;
	m_iTradeModifier = 0;
	m_iTradeMissionModifier = 0;
	m_iCorporationRevenueModifier = 0;
	m_iCorporationMaintenanceModifier = 0;
	m_iCanFarmDesertCount = 0;
	m_iOpenBordersTradingCount = 0;
	m_iDefensivePactTradingCount = 0;
	m_iPermanentAllianceTradingCount = 0;
	m_iVassalTradingCount = 0;
	m_iBridgeBuildingCount = 0;
	m_iIrrigationCount = 0;
	m_iIgnoreIrrigationCount = 0;
	m_iWaterWorkCount = 0;
	m_iVassalPower = 0;
	m_iMasterPower = 0;
	m_iEnemyWarWearinessModifier = 0;
	m_iRiverTradeCount = 0;
	m_iNukeInterception = 0;

	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			m_ppaaiImprovementYieldChange[iI][iJ] = 0;
		}
	}

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		m_paiBuildingCount[iI] = 0;
		m_paiObsoleteBuildingCount[iI] = 0;

		for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			m_ppiBuildingCommerceModifier[iI][iJ] = 0;
		}
		for (int iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
		{
			m_ppiBuildingSpecialistChange[iI][iJ] = 0;
		}
	}

	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		m_paiFreeSpecialistCount[iI] = 0;
	}

	for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		m_aiExtraMoves[iI] = 0;
	}

	for (int iI = 0, num = GC.getNumRouteInfos(); iI < num; iI++)
	{
		m_paiRouteChange[iI] = 0;
	}

	// Recalculate player modifiers
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(getID()))
		{
			GET_PLAYER((PlayerTypes)iI).recalculateModifiers();
		}
	}
	// Reapply techs
	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		if (isHasTech((TechTypes)iI))
		{
			processTech((TechTypes)iI, 1);
		}
	}
	// Reapply circumnavigation bonus
	if (GC.getGame().getCircumnavigatedTeam() == getID())
	{
		setCircumnavigated(true);
	}
	// Reapply projects
	for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		if (getProjectCount((ProjectTypes)iI) > 0)
		{
			processProjectChange((ProjectTypes)iI, getProjectCount((ProjectTypes)iI), getProjectCount((ProjectTypes)iI));
		}
	}
}

CvProperties* CvTeam::getProperties()
{
	return &m_Properties;
}

const CvProperties* CvTeam::getPropertiesConst() const
{
	return &m_Properties;
}

int CvTeam::getLastRoundOfValidImprovementCacheUpdate() const
{
	return m_iLastRoundOfValidImprovementCacheUpdate;
}

void CvTeam::setLastRoundOfValidImprovementCacheUpdate()
{
	m_iLastRoundOfValidImprovementCacheUpdate = GC.getGame().getGameTurn();
}


int CvTeam::getTypicalUnitValue(UnitAITypes eUnitAI) const
{
	PROFILE_EXTRA_FUNC();
	int iMax = 0;
	for (int iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getID())
		{
			iMax = std::max(iMax, GET_PLAYER((PlayerTypes)iI).getTypicalUnitValue(eUnitAI));
		}
	}
	return iMax;
}


int CvTeam::getWinForLosingResearchModifier() const
{
	if (getNumCities() < 1)
	{
		return 0;
	}
	return GC.getGame().getWinForLosingResearchModifier(getNumCities(), getTotalPopulation(false));
}

