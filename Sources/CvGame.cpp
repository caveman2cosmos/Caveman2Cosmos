// game.cpp

#include "CvGameCoreDLL.h"
#include "CvArea.h"
#include "CvBuildingInfo.h"
#include "CvCity.h"
#include "CvDeal.h"
#include "CvEventReporter.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvInitCore.h"
#include "CvInfos.h"
#include "CvImprovementInfo.h"
#include "CvMap.h"
#include "CvMapGenerator.h"
#include "CvMessageControl.h"
#include "CvPathGenerator.h"
#include "CvPlayerAI.h"
#include "CvPlot.h"
#include "CvPopupInfo.h"
#include "CvPython.h"
#include "CvReplayInfo.h"
#include "CvReplayMessage.h"
#include "CvSelectionGroup.h"
#include "CvTeamAI.h"
#include "CvUnit.h"
#include "CvDLLEntityIFaceBase.h"

//	Koshling - save game compatibility between (most) builds
//	UI flag values in game serialization.  These are bitwise combinable
#define	GAME_SAVE_UI_FLAG_VALUE_AND_BASE		0x00000001

// Public Functions...

#pragma warning( disable : 4355 )
CvGame::CvGame()
	: m_GameObject()
	, m_Properties(this)
	, m_iChokePointCalculationVersion(0)
	, m_eCurrentMap(MAP_EARTH)
{
	m_aiRankPlayer = new int[MAX_PLAYERS];        // Ordered by rank...
	m_aiPlayerRank = new int[MAX_PLAYERS];        // Ordered by player ID...
	m_aiPlayerScore = new int[MAX_PLAYERS];       // Ordered by player ID...
	m_aiRankTeam = new int[MAX_TEAMS];						// Ordered by rank...
	m_aiTeamRank = new int[MAX_TEAMS];						// Ordered by team ID...
	m_aiTeamScore = new int[MAX_TEAMS];						// Ordered by team ID...

	m_paiImprovementCount = NULL;
	m_paiUnitCreatedCount = NULL;
	m_paiBuildingCreatedCount = NULL;
	m_paiProjectCreatedCount = NULL;
	m_paiForceCivicCount = NULL;
	m_paiVoteOutcome = NULL;
	m_paiReligionGameTurnFounded = NULL;
	m_paiTechGameTurnDiscovered = NULL;
	m_paiCorporationGameTurnFounded = NULL;
	m_aiSecretaryGeneralTimer = NULL;
	m_aiVoteTimer = NULL;
	m_aiDiploVote = NULL;

	m_pabSpecialUnitValid = NULL;
	m_pabSpecialBuildingValid = NULL;
	m_abReligionSlotTaken = NULL;

	m_abTechCanFoundReligion = NULL;

	m_paHolyCity = NULL;
	m_paHeadquarters = NULL;

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		starshipLaunched[iI] = false;
		diplomaticVictoryAchieved[iI] = false;
	}

	m_abPreviousRequest = new bool[MAX_PLAYERS];
	m_aiModderGameOption = new int[NUM_MODDERGAMEOPTION_TYPES];
	m_aiFlexibleDifficultyTimer = new int[MAX_PLAYERS];

	m_pReplayInfo = NULL;

	m_aiShrineBuilding = NULL;
	m_aiShrineReligion = NULL;

	m_bRecalculatingModifiers = false;

	reset(NO_HANDICAP, true);
}


CvGame::~CvGame()
{
	uninit();

	SAFE_DELETE_ARRAY(m_aiRankPlayer);
	SAFE_DELETE_ARRAY(m_aiPlayerRank);
	SAFE_DELETE_ARRAY(m_aiPlayerScore);
	SAFE_DELETE_ARRAY(m_aiRankTeam);
	SAFE_DELETE_ARRAY(m_aiTeamRank);
	SAFE_DELETE_ARRAY(m_aiTeamScore);
	SAFE_DELETE_ARRAY(m_abPreviousRequest);
	SAFE_DELETE_ARRAY(m_aiModderGameOption);
	SAFE_DELETE_ARRAY(m_aiFlexibleDifficultyTimer);
}


void CvGame::init(HandicapTypes eHandicap)
{
	GC.getInitCore().endGameSetup();
	//--------------------------------
	// Init saved data
	reset(eHandicap);

	//--------------------------------
	// Init containers
	m_deals.init();
	m_voteSelections.init();
	m_votesTriggered.init();

	m_mapRand.init(GC.getInitCore().getMapRandSeed() % 73637381);
	m_sorenRand.init(GC.getInitCore().getSyncRandSeed() % 52319761);

	//--------------------------------
	// Init non-saved data

	//--------------------------------
	// Init other game data

	// Turn off all MP options if it's a single player game
	if (GC.getInitCore().getType() == GAME_SP_NEW
	||  GC.getInitCore().getType() == GAME_SP_SCENARIO)
	{
		for (int iI = 0; iI < NUM_MPOPTION_TYPES; ++iI)
		{
			setMPOption((MultiplayerOptionTypes)iI, false);
		}
	}

	// If this is a hot seat game, simultaneous turns is always off
	if (isHotSeat() || isPbem())
	{
		setMPOption(MPOPTION_SIMULTANEOUS_TURNS, false);
	}
	// If we didn't set a time in the Pitboss, turn timer off
	if (isPitboss() && getPitbossTurnTime() == 0)
	{
		setMPOption(MPOPTION_TURN_TIMER, false);
	}

	if (isMPOption(MPOPTION_SHUFFLE_TEAMS))
	{
		int aiTeams[MAX_PC_PLAYERS];

		int iNumPlayers = 0;
		for (int i = 0; i < MAX_PC_PLAYERS; i++)
		{
			if (GC.getInitCore().getSlotStatus((PlayerTypes)i) == SS_TAKEN)
			{
				aiTeams[iNumPlayers] = GC.getInitCore().getTeam((PlayerTypes)i);
				++iNumPlayers;
			}
		}

		for (int i = 0; i < iNumPlayers; i++)
		{
			int j = (getSorenRand().get(iNumPlayers - i, NULL) + i);

			if (i != j)
			{
				int iTemp = aiTeams[i];
				aiTeams[i] = aiTeams[j];
				aiTeams[j] = iTemp;
			}
		}

		iNumPlayers = 0;
		for (int i = 0; i < MAX_PC_PLAYERS; i++)
		{
			if (GC.getInitCore().getSlotStatus((PlayerTypes)i) == SS_TAKEN)
			{
				GC.getInitCore().setTeam((PlayerTypes)i, (TeamTypes)aiTeams[iNumPlayers]);
				++iNumPlayers;
			}
		}
	}
	//AlbertS2 set hidden options to their default setting
	for (int iI = 0; iI < NUM_GAMEOPTION_TYPES; iI++)
	{
		const CvGameOptionInfo& kGameOption = GC.getGameOptionInfo((GameOptionTypes)iI);
		if (!kGameOption.getVisible())
		{
			setOption((GameOptionTypes)iI, kGameOption.getDefault());
		}
	}
	//TB GameOption compatibility enforcement project
	for (int iI = 0; iI < NUM_GAMEOPTION_TYPES; iI++)
	{
		const GameOptionTypes eGameOption = ((GameOptionTypes)iI);
		if (isOption(eGameOption))
		{
			enforceOptionCompatibility(eGameOption);
		}
	}

	if (isOption(GAMEOPTION_LOCK_MODS))
	{
		if (!isGameMultiPlayer())
		{
			static const int iPasswordSize = 8;
			char szRandomPassword[iPasswordSize];
			for (int i = 0; i < iPasswordSize-1; i++)
			{
				szRandomPassword[i] = getSorenRandNum(128, NULL);
			}
			szRandomPassword[iPasswordSize-1] = 0;

			GC.getInitCore().setAdminPassword(szRandomPassword);
		}
		else setOption(GAMEOPTION_LOCK_MODS, false);
	}

	// Alberts2: Recalculate which info class replacements are currently active
	GC.updateReplacements();

	//TB: Set Statuses
	setStatusPromotions();

	//establish improvement costs
	//for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	//{
	//	GC.getImprovementInfo((ImprovementTypes)iI).setHighestCost();
	//}

	if (getGameTurn() == 0)
	{
		int iStartTurn = 0;

		for (iI = 0; iI < GC.getGameSpeedInfo(getGameSpeedType()).getNumTurnIncrements(); iI++)
		{
			iStartTurn += GC.getGameSpeedInfo(getGameSpeedType()).getGameTurnInfo(iI).iNumGameTurnsPerIncrement;
		}

		iStartTurn *= GC.getEraInfo(getStartEra()).getStartPercent();
		iStartTurn /= 100;

		setGameTurn(iStartTurn);
	}

	setStartTurn(getGameTurn());

	if (getMaxTurns() == 0)
	{
		int iEstimateEndTurn = 0;

		for (int iI = 0; iI < GC.getGameSpeedInfo(getGameSpeedType()).getNumTurnIncrements(); iI++)
		{
			iEstimateEndTurn += GC.getGameSpeedInfo(getGameSpeedType()).getGameTurnInfo(iI).iNumGameTurnsPerIncrement;
		}

		setEstimateEndTurn(iEstimateEndTurn);

//Sevo Begin--VCM
	// This looks like a reasonable place for me to place my own interrupts for the victory conditions
	// I need to ensure that the AI understands it has to go after everything, so ALL the victory options
	// need to be "enabled" so starships can be built, etc. etc.
	//Only exception is diplo victory, but whatever.

		if (getEstimateEndTurn() > getGameTurn())
		{
			for (int iI = 0; iI < GC.getNumVictoryInfos(); iI++)
			{
				if (isVictoryValid((VictoryTypes)iI) && (GC.getVictoryInfo((VictoryTypes)iI).isEndScore() || GC.getVictoryInfo((VictoryTypes)iI).isTotalVictory()))
				{
					setMaxTurns(getEstimateEndTurn() - getGameTurn());
					break;
				}
			}
		}
	}
	else setEstimateEndTurn(getGameTurn() + getMaxTurns());

	// Okay, now we'll actually set up the other VC's to agree with the total victory
	// If Total Victory is enabled, TURN ON ALL OTHERS AS WELL
	// This allows starship construction, gets the AI thinking correctly, etc.
	// The TV condition overrides the others but requires they be "on"
	for (int iI = 0; iI < GC.getNumVictoryInfos(); iI++)
	{
		if (GC.getVictoryInfo((VictoryTypes)iI).isTotalVictory() && isVictoryValid((VictoryTypes)iI))
		{
			for (iI = 0; iI < GC.getNumVictoryInfos(); iI++)
			{
				setVictoryValid((VictoryTypes)iI, true);
			}
			break;
		}
	}
//Sevo End VCM

	m_plotGroupHashesInitialized = false;

	//Ruthless AI means Aggressive AI is on too.
	if (isOption(GAMEOPTION_RUTHLESS_AI) && !isOption(GAMEOPTION_AGGRESSIVE_AI))
	{
		setOption(GAMEOPTION_AGGRESSIVE_AI, true);
	}

	if (!isOption(GAMEOPTION_GREAT_COMMANDERS))
	{
		for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			if (GC.getUnitInfo((UnitTypes)iI).isGreatGeneral())
			{
				GC.getUnitInfo((UnitTypes)iI).setPowerValue(GC.getUnitInfo((UnitTypes)iI).getPowerValue() / 10);
			}
		}
	}

	if (isOption(GAMEOPTION_UNITED_NATIONS))
	{
		//Find the diplomatic victory
		for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			const CvBuildingInfo& info = GC.getBuildingInfo((BuildingTypes) iI);
			if (info.getVictoryPrereq() != NO_VICTORY && info.getVoteSourceType() != NO_VOTESOURCE)
			{
				m_bDiploVictoryEnabled = isVictoryValid((VictoryTypes) info.getVictoryPrereq());
				setVictoryValid((VictoryTypes) info.getVictoryPrereq(), true);
				break;
			}
		}
	}

	setStartYear(GC.getSTART_YEAR());
	m_iDateTurn = -500;

	for (int iI = 0; iI < GC.getNumSpecialUnitInfos(); iI++)
	{
		if (GC.getSpecialUnitInfo((SpecialUnitTypes)iI).isValid())
		{
			makeSpecialUnitValid((SpecialUnitTypes)iI);
		}
	}

	for (int iI = 0; iI < GC.getNumSpecialBuildingInfos(); iI++)
	{
		if (GC.getSpecialBuildingInfo((SpecialBuildingTypes)iI).isValid())
		{
			makeSpecialBuildingValid((SpecialBuildingTypes)iI);
		}
	}

	for (int iI = MAX_PC_PLAYERS; iI < MAX_PLAYERS-1; iI++)
	{
		PlayerTypes ePlayer = (PlayerTypes)iI;

		FAssertMsg(!GET_PLAYER(ePlayer).isAlive(), "NPC is not expected to be alive at this point of the game initialization.");

		if (ePlayer == BEAST_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("ANIMAL_LEADER"),
				(CivilizationTypes)GC.getDefineINT("BEAST_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			GET_TEAM(BEAST_TEAM).init(BEAST_TEAM);
			GC.getInitCore().setTeam(ePlayer, BEAST_TEAM);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		else if (ePlayer == PREDATOR_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("ANIMAL_LEADER"),
				(CivilizationTypes)GC.getDefineINT("PREDATOR_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			GET_TEAM(PREDATOR_TEAM).init(PREDATOR_TEAM);
			GC.getInitCore().setTeam(ePlayer, PREDATOR_TEAM);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		else if (ePlayer == PREY_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("ANIMAL_LEADER"),
				(CivilizationTypes)GC.getDefineINT("PREY_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			GET_TEAM(PREY_TEAM).init(PREY_TEAM);
			GC.getInitCore().setTeam(ePlayer, PREY_TEAM);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		else if (ePlayer == INSECT_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("INSECT_LEADER"),
				(CivilizationTypes)GC.getDefineINT("INSECT_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			GET_TEAM(INSECT_TEAM).init(INSECT_TEAM);
			GC.getInitCore().setTeam(ePlayer, INSECT_TEAM);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		/*
		else if (ePlayer == NPC4_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("NPC4_LEADER"),
				(CivilizationTypes)GC.getDefineINT("NPC4_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			GET_TEAM(NPC4_TEAM).init(NPC4_TEAM);
			GC.getInitCore().setTeam(ePlayer, NPC4_TEAM);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		else if (ePlayer == NPC3_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("NPC3_LEADER"),
				(CivilizationTypes)GC.getDefineINT("NPC3_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			GET_TEAM(NPC3_TEAM).init(NPC3_TEAM);
			GC.getInitCore().setTeam(ePlayer, NPC3_TEAM);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		else if (ePlayer == NPC2_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("NPC2_LEADER"),
				(CivilizationTypes)GC.getDefineINT("NPC2_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			GET_TEAM(NPC2_TEAM).init(NPC2_TEAM);
			GC.getInitCore().setTeam(ePlayer, NPC2_TEAM);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		else if (ePlayer == NPC1_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("NPC1_LEADER"),
				(CivilizationTypes)GC.getDefineINT("NPC1_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			GET_TEAM(NPC1_TEAM).init(NPC1_TEAM);
			GC.getInitCore().setTeam(ePlayer, NPC1_TEAM);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		else if (ePlayer == NPC0_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("NPC0_LEADER"),
				(CivilizationTypes)GC.getDefineINT("NPC0_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			GET_TEAM(NPC0_TEAM).init(NPC0_TEAM);
			GC.getInitCore().setTeam(ePlayer, NPC0_TEAM);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		*/
		else if (ePlayer == NEANDERTHAL_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("NEANDERTHAL_LEADER"),
				(CivilizationTypes)GC.getDefineINT("NEANDERTHAL_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			GET_TEAM(NEANDERTHAL_TEAM).init(NEANDERTHAL_TEAM);
			GC.getInitCore().setTeam(ePlayer, NEANDERTHAL_TEAM);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		/* Toffer - No point, the exe does this slightly after this code anyway.
		else if (ePlayer == BARBARIAN_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("BARBARIAN_LEADER"),
				(CivilizationTypes)GC.getDefineINT("BARBARIAN_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			GET_TEAM(BARBARIAN_TEAM).init(BARBARIAN_TEAM);
			GC.getInitCore().setTeam(ePlayer, BARBARIAN_TEAM);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		*/
	}
	AI_init();

	doUpdateCacheOnTurn();
}

//
// Set initial items (units, techs, etc...)
//
void CvGame::setInitialItems()
{
	PROFILE_FUNC();

	initFreeState();
	assignStartingPlots();

	if (isOption(GAMEOPTION_CULTURALLY_LINKED_STARTS))
	{
		Cy::call("CvCultureLinkInterface", "assignCulturallyLinkedStarts");
	}
	normalizeStartingPlots();

	// BarbarianWorld
	if (!isOption(GAMEOPTION_NO_BARBARIANS) && isOption(GAMEOPTION_BARBARIAN_WORLD))
	{
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive() && !foundBarbarianCity())
			{
				break;
			}
		}
	}

	initFreeUnits();

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAlive())
		{
			GET_PLAYER((PlayerTypes)i).AI_updateFoundValues(true);
		}
	}
	if (isOption(GAMEOPTION_PERSONALIZED_MAP))
	{
		clearLandmarks();
		findMountainRanges();
		findLonePeaks();
		findBays();
		findForests();
		findJungles();
		findDeserts();
		findLakes();
		pruneLandmarks();
		addLandmarkSigns();
		updateInitialSigns();
	}
}


bool CvGame::canRegenerateMap() const
{
	return !isGameMultiPlayer() && getElapsedGameTurns() == 0 && !GC.getInitCore().getWBMapScript();
}

void CvGame::regenerateMap()
{
	setFinalInitialized(false);

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		GET_PLAYER((PlayerTypes)iI).killUnits();
		GET_PLAYER((PlayerTypes)iI).killCities();
		GET_PLAYER((PlayerTypes)iI).killAllDeals();
		GET_PLAYER((PlayerTypes)iI).setFoundedFirstCity(false);
		GET_PLAYER((PlayerTypes)iI).setStartingPlot(NULL, false);
	}

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		GC.getMap().setRevealedPlots(((TeamTypes)iI), false);

		for (int iJ = 0; iJ < MAX_PC_TEAMS; iJ++)
		{
			if (iJ != iI) GET_TEAM((TeamTypes)iI).setHasMet((TeamTypes)iJ, false);
		}
	}

	gDLL->getEngineIFace()->clearSigns();

	GC.getMap().erasePlots();

	m_iChokePointCalculationVersion = 0;

	CvMapGenerator::GetInstance().generateRandomMap();
	CvMapGenerator::GetInstance().addGameElements();

	gDLL->getEngineIFace()->RebuildAllPlots();

	CvEventReporter::getInstance().resetStatistics();

	setInitialItems();

	initScoreCalculation();
	setFinalInitialized(true);

	GC.getMap().setupGraphical();
	gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);
	gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
	if (isInAdvancedStart())
	{
		gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
	}
	else gDLL->getInterfaceIFace()->setCycleSelectionCounter(1);

	CvEventReporter::getInstance().mapRegen();

	gDLL->getEngineIFace()->AutoSave(true);
	// Toffer - Move camera after autosave as the latter interrupts the former from completing succsessfully.
	GC.getCurrentViewport()->bringIntoView(GET_PLAYER(GC.getGame().getActivePlayer()).getStartingPlot()->getX(), GET_PLAYER(GC.getGame().getActivePlayer()).getStartingPlot()->getY());
}

void CvGame::uninit()
{
	SAFE_DELETE_ARRAY(m_aiShrineBuilding);
	SAFE_DELETE_ARRAY(m_aiShrineReligion);
	SAFE_DELETE_ARRAY(m_paiImprovementCount);
	SAFE_DELETE_ARRAY(m_paiUnitCreatedCount);
	SAFE_DELETE_ARRAY(m_paiBuildingCreatedCount);
	SAFE_DELETE_ARRAY(m_paiProjectCreatedCount);
	SAFE_DELETE_ARRAY(m_paiForceCivicCount);
	SAFE_DELETE_ARRAY(m_paiVoteOutcome);
	SAFE_DELETE_ARRAY(m_paiReligionGameTurnFounded);
	SAFE_DELETE_ARRAY(m_paiTechGameTurnDiscovered);
	SAFE_DELETE_ARRAY(m_paiCorporationGameTurnFounded);
	SAFE_DELETE_ARRAY(m_aiSecretaryGeneralTimer);
	SAFE_DELETE_ARRAY(m_aiVoteTimer);
	SAFE_DELETE_ARRAY(m_aiDiploVote);
	SAFE_DELETE_ARRAY(m_pabSpecialUnitValid);
	SAFE_DELETE_ARRAY(m_pabSpecialBuildingValid);
	SAFE_DELETE_ARRAY(m_abReligionSlotTaken);
	SAFE_DELETE_ARRAY(m_abTechCanFoundReligion);
	SAFE_DELETE_ARRAY(m_paHolyCity);
	SAFE_DELETE_ARRAY(m_paHeadquarters);

	m_aszDestroyedCities.clear();
	m_aszGreatPeopleBorn.clear();

	m_deals.uninit();
	m_voteSelections.uninit();
	m_votesTriggered.uninit();

	m_mapRand.uninit();
	m_sorenRand.uninit();

	clearReplayMessageMap();
	SAFE_DELETE(m_pReplayInfo);

	m_aPlotExtraYields.clear();
	m_aPlotExtraCosts.clear();
	m_mapVoteSourceReligions.clear();
	m_aeInactiveTriggers.clear();

#ifdef CAN_BUILD_VALUE_CACHING
	CvPlot::ClearCanBuildCache();
#endif

	if ( CvPlayerAI::areStaticsInitialized() )
	{
		CvMapInitData defaultMapData;

		algo::for_each(GC.getMaps()
			| filtered(bind(CvMap::plotsInitialized, _1))
			, bind(CvMap::reset, _1, &defaultMapData)
		);

		for(int i = 0; i < MAX_PLAYERS; i++)
		{
			GET_PLAYER((PlayerTypes)i).uninit();
		}
	}
}


CvString create_game_id()
{
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
	return CvString(buffer);
}


// FUNCTION: reset()
// Initializes data members that are serialized.
void CvGame::reset(HandicapTypes eHandicap, bool bConstructorCall)
{
	CvPlotPaging::ResetPaging();

	//--------------------------------
	// Uninit class
	uninit();

	m_gameId = create_game_id();
	m_iElapsedGameTurns = 0;
	m_iStartTurn = 0;
	m_iStartYear = 0;
	m_iEstimateEndTurn = 0;
	m_iDateTurn = -500;
	m_iTurnSlice = 0;
	m_iCutoffSlice = 0;
	m_iNumGameTurnActive = 0;
	m_iNumCities = 0;
	m_iTotalPopulation = 0;
	m_iTradeRoutes = 0;
	m_iFreeTradeCount = 0;
	m_iNoNukesCount = 0;
	m_iNukesExploded = 0;
	m_iMaxPopulation = 0;
	m_iMaxLand = 0;
	m_iMaxTech = 0;
	m_iMaxWonders = 0;
	m_iInitPopulation = 0;
	m_iInitLand = 0;
	m_iInitTech = 0;
	m_iInitWonders = 0;
	m_iCurrentVoteID = 0;
	m_iMercyRuleCounter = 0;
	m_iCutLosersCounter = 0;
	m_iHighToLowCounter = 0;
	m_iIncreasingDifficultyCounter = 0;
	m_iRiverBuildings = 0;
	m_iNumWonders = 0;
	m_bDiploVictoryEnabled = false;
	m_bAnyoneHasUnitZoneOfControl = false;
	calculateRiverBuildings();
	calculateCoastalBuildings();
	calculateNumWonders();
	m_iLastNukeStrikeX = INVALID_PLOT_COORD;
	m_iLastNukeStrikeY = INVALID_PLOT_COORD;

	for (int loopI = 0; loopI < MAX_TEAMS; loopI++)
	{
	  starshipLaunched[loopI] = false;
	  diplomaticVictoryAchieved[loopI] = false;
	}

	m_uiInitialTime = 0;

	m_bScoreDirty = false;
	m_circumnavigatingTeam = NO_TEAM;
	m_bDebugMode = false;
	m_bDebugModeCache = false;
	m_bFinalInitialized = false;
	m_bPbemTurnSent = false;
	m_bHotPbemBetweenTurns = false;
	m_bPlayerOptionsSent = false;
	//TB Nukefix
	//m_bNukesValid = false;

	m_eHandicap = eHandicap;
	m_ePausePlayer = NO_PLAYER;
	m_eBestLandUnit = NO_UNIT;
	m_eWinner = NO_TEAM;
	m_eVictory = NO_VICTORY;
	m_eGameState = GAMESTATE_ON;

	m_szScriptData = "";

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aiRankPlayer[iI] = 0;
		m_aiPlayerRank[iI] = 0;
		m_aiPlayerScore[iI] = 0;

		m_iAIAutoPlay[iI] = 0;
		m_iForcedAIAutoPlay[iI] = 0;
	}

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiRankTeam[iI] = 0;
		m_aiTeamRank[iI] = 0;
		m_aiTeamScore[iI] = 0;
	}

	for (iI = 0; iI < NUM_MODDERGAMEOPTION_TYPES; iI++)
	{
		m_aiModderGameOption[iI] = 0;
	}
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_abPreviousRequest[iI] = false;
		m_aiFlexibleDifficultyTimer[iI] = 0;
	}

	const int iNumUnits = GC.getNumUnitInfos();

	if (!bConstructorCall)
	{
		FAssertMsg(m_paiImprovementCount==NULL, "about to leak memory, CvGame::m_paiImprovementCount");
		m_paiImprovementCount = new int[GC.getNumImprovementInfos()];
		for (iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			m_paiImprovementCount[iI] = 0;
		}
		FAssertMsg(m_paiUnitCreatedCount==NULL, "about to leak memory, CvGame::m_paiUnitCreatedCount");
		m_paiUnitCreatedCount = new int[iNumUnits];
		for (iI = 0; iI < iNumUnits; iI++)
		{
			m_paiUnitCreatedCount[iI] = 0;
		}
		FAssertMsg(m_paiBuildingCreatedCount==NULL, "about to leak memory, CvGame::m_paiBuildingCreatedCount");
		m_paiBuildingCreatedCount = new int[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_paiBuildingCreatedCount[iI] = 0;
		}

		FAssertMsg(m_paiProjectCreatedCount==NULL, "about to leak memory, CvGame::m_paiProjectCreatedCount");
		m_paiProjectCreatedCount = new int[GC.getNumProjectInfos()];
		for (iI = 0; iI < GC.getNumProjectInfos(); iI++)
		{
			m_paiProjectCreatedCount[iI] = 0;
		}

		FAssertMsg(m_paiForceCivicCount==NULL, "about to leak memory, CvGame::m_paiForceCivicCount");
		m_paiForceCivicCount = new int[GC.getNumCivicInfos()];
		for (iI = 0; iI < GC.getNumCivicInfos(); iI++)
		{
			m_paiForceCivicCount[iI] = 0;
		}

		FAssertMsg(0 < GC.getNumVoteInfos(), "GC.getNumVoteInfos() is not greater than zero in CvGame::reset");
		FAssertMsg(m_paiVoteOutcome==NULL, "about to leak memory, CvGame::m_paiVoteOutcome");
		m_paiVoteOutcome = new PlayerVoteTypes[GC.getNumVoteInfos()];
		for (iI = 0; iI < GC.getNumVoteInfos(); iI++)
		{
			m_paiVoteOutcome[iI] = NO_PLAYER_VOTE;
		}

		FAssertMsg(0 < GC.getNumVoteSourceInfos(), "GC.getNumVoteSourceInfos() is not greater than zero in CvGame::reset");
		FAssertMsg(m_aiDiploVote==NULL, "about to leak memory, CvGame::m_aiDiploVote");
		m_aiDiploVote = new int[GC.getNumVoteSourceInfos()];
		for (iI = 0; iI < GC.getNumVoteSourceInfos(); iI++)
		{
			m_aiDiploVote[iI] = 0;
		}

		FAssertMsg(m_pabSpecialUnitValid==NULL, "about to leak memory, CvGame::m_pabSpecialUnitValid");
		m_pabSpecialUnitValid = new bool[GC.getNumSpecialUnitInfos()];
		for (iI = 0; iI < GC.getNumSpecialUnitInfos(); iI++)
		{
			m_pabSpecialUnitValid[iI] = false;
		}

		FAssertMsg(m_pabSpecialBuildingValid==NULL, "about to leak memory, CvGame::m_pabSpecialBuildingValid");
		m_pabSpecialBuildingValid = new bool[GC.getNumSpecialBuildingInfos()];
		for (iI = 0; iI < GC.getNumSpecialBuildingInfos(); iI++)
		{
			m_pabSpecialBuildingValid[iI] = false;
		}

		FAssertMsg(m_paiReligionGameTurnFounded==NULL, "about to leak memory, CvGame::m_paiReligionGameTurnFounded");
		m_paiReligionGameTurnFounded = new int[GC.getNumReligionInfos()];
		FAssertMsg(m_abReligionSlotTaken==NULL, "about to leak memory, CvGame::m_abReligionSlotTaken");
		m_abReligionSlotTaken = new bool[GC.getNumReligionInfos()];
		FAssertMsg(m_paHolyCity==NULL, "about to leak memory, CvGame::m_paHolyCity");
		m_paHolyCity = new IDInfo[GC.getNumReligionInfos()];
		for (iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			m_paiReligionGameTurnFounded[iI] = -1;
			m_paHolyCity[iI].reset();
			m_abReligionSlotTaken[iI] = false;
		}

		FAssertMsg(m_abTechCanFoundReligion==NULL, "about to leak memory, CvGame::m_abTechCanFoundReligion");
		m_abTechCanFoundReligion = new bool[GC.getNumTechInfos()];
		FAssertMsg(m_paiTechGameTurnDiscovered==NULL, "about to leak memory, CvGame::m_abTechGameTurnDiscovered");
		m_paiTechGameTurnDiscovered = new int[GC.getNumTechInfos()];
		for (iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			m_abTechCanFoundReligion[TechTypes(iI)] = false;
			m_paiTechGameTurnDiscovered[iI] = -1;
		}
		for (iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			m_abTechCanFoundReligion[GC.getReligionInfo((ReligionTypes)iI).getTechPrereq()] = true;
		}

		FAssertMsg(m_paiCorporationGameTurnFounded==NULL, "about to leak memory, CvGame::m_paiCorporationGameTurnFounded");
		m_paiCorporationGameTurnFounded = new int[GC.getNumCorporationInfos()];
		m_paHeadquarters = new IDInfo[GC.getNumCorporationInfos()];
		for (iI = 0; iI < GC.getNumCorporationInfos(); iI++)
		{
			m_paiCorporationGameTurnFounded[iI] = -1;
			m_paHeadquarters[iI].reset();
		}

		FAssertMsg(m_aiShrineBuilding==NULL, "about to leak memory, CvGame::m_aiShrineBuilding");
		FAssertMsg(m_aiShrineReligion==NULL, "about to leak memory, CvGame::m_aiShrineReligion");
		m_aiShrineBuilding = new int[GC.getNumBuildingInfos()];
		m_aiShrineReligion = new int[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_aiShrineBuilding[iI] = (int) NO_BUILDING;
			m_aiShrineReligion[iI] = (int) NO_RELIGION;
		}

		FAssertMsg(m_aiSecretaryGeneralTimer==NULL, "about to leak memory, CvGame::m_aiSecretaryGeneralTimer");
		FAssertMsg(m_aiVoteTimer==NULL, "about to leak memory, CvGame::m_aiVoteTimer");
		m_aiSecretaryGeneralTimer = new int[GC.getNumVoteSourceInfos()];
		m_aiVoteTimer = new int[GC.getNumVoteSourceInfos()];
		for (iI = 0; iI < GC.getNumVoteSourceInfos(); iI++)
		{
			m_aiSecretaryGeneralTimer[iI] = 0;
			m_aiVoteTimer[iI] = 0;
		}
	}

	m_deals.removeAll();
	m_voteSelections.removeAll();
	m_votesTriggered.removeAll();

	m_mapRand.reset();
	m_sorenRand.reset();

	m_iNumSessions = 1;

	m_iShrineBuildingCount = 0;
	m_iNumCultureVictoryCities = 0;
	m_eCultureVictoryCultureLevel = NO_CULTURELEVEL;

	if (!bConstructorCall)
	{
		AI_reset();
	}

	// Sanguo Mod Performance start, added by poyuzhe 07.27.09
	UnitTypes eUnit;
	std::vector<UnitTypes> aUpgradeUnits;

	//Establish speedy promotion & Building reference by line
	for (iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
	{
		PromotionLineTypes ePromotionLine = (PromotionLineTypes)iI;
		GC.getPromotionLineInfo(ePromotionLine).setPromotions();
		GC.getPromotionLineInfo(ePromotionLine).setBuildings();
	}

	//Establish Unit post load factors
	for (iI = 0; iI < iNumUnits; iI++)
	{
		eUnit = (UnitTypes)iI;
		CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);
		aUpgradeUnits.clear();
		//set post mod-load unitinfo details
		kUnit.setReligionSubCombat();
		kUnit.setCultureSubCombat();
		kUnit.setEraSubCombat();
		kUnit.setSM();
		kUnit.setHealAsTypes();

		do
		{
			if (eUnit != NO_UNIT)
			{
				for (int iJ = 0; iJ < GC.getUnitInfo(eUnit).getNumUnitUpgrades(); iJ++)
				{
					kUnit.addUnitToUpgradeChain(GC.getUnitInfo(eUnit).getUnitUpgrade(iJ));
					aUpgradeUnits.push_back((UnitTypes) GC.getUnitInfo(eUnit).getUnitUpgrade(iJ));
				}
			}
			if (aUpgradeUnits.empty())
			{
				break;
			}
			else
			{
				eUnit = aUpgradeUnits.front();
				aUpgradeUnits.erase(aUpgradeUnits.begin());
			}
		}
		while (true);
	}
	// Sanguo Mod Performance, end

	m_Properties.clear();

	// Alberts2: Recalculate which info class replacements are currently active
	if (!bConstructorCall)
		GC.updateReplacements();

	m_lastGraphicUpdateRequestTickCount = -1;

	m_plotGroupHashesInitialized = false;

	CvPlotPaging::ResetPaging();
}


void CvGame::initDiplomacy()
{
	PROFILE_FUNC();

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		CvTeam& team = GET_TEAM((TeamTypes)iI);
		team.meet((TeamTypes)iI, false);

		if (team.isMinorCiv())
		{
			for (int iJ = 0; iJ < MAX_PC_TEAMS; iJ++)
			{
				if (iI != iJ)
				{
					team.declareWar((TeamTypes)iJ, false, NO_WARPLAN);
				}
			}
		}
	}
	for (int iI = MAX_PC_TEAMS; iI < MAX_TEAMS; iI++)
	{
		CvTeam& team = GET_TEAM((TeamTypes)iI);
		team.meet((TeamTypes)iI, false);

		for (int iJ = 0; iJ < MAX_PC_TEAMS; iJ++)
		{
			team.declareWar((TeamTypes)iJ, false, NO_WARPLAN);
		}
		if (!isOption(GAMEOPTION_PEACE_AMONG_NPCS))
		{
			for (int iJ = iI + 1; iJ < MAX_TEAMS; iJ++)
			{
				team.declareWar((TeamTypes)iJ, false, NO_WARPLAN);
			}
		}
	}

	// Forced peace at the beginning of Advanced starts
	if (isOption(GAMEOPTION_ADVANCED_START))
	{
		CLinkList<TradeData> player1List;
		CLinkList<TradeData> player2List;
		TradeData kTradeData;
		setTradeItem(&kTradeData, TRADE_PEACE_TREATY);
		player1List.insertAtEnd(kTradeData);
		player2List.insertAtEnd(kTradeData);

		for (int iPlayer1 = 0; iPlayer1 < MAX_PC_PLAYERS; ++iPlayer1)
		{
			CvPlayer& kLoopPlayer1 = GET_PLAYER((PlayerTypes)iPlayer1);

			if (kLoopPlayer1.isAlive())
			{
				for (int iPlayer2 = iPlayer1 + 1; iPlayer2 < MAX_PC_PLAYERS; ++iPlayer2)
				{
					CvPlayer& kLoopPlayer2 = GET_PLAYER((PlayerTypes)iPlayer2);

					if (kLoopPlayer2.isAlive() && GET_TEAM(kLoopPlayer1.getTeam()).canChangeWarPeace(kLoopPlayer2.getTeam()))
					{
						implementDeal((PlayerTypes)iPlayer1, (PlayerTypes)iPlayer2, &player1List, &player2List);
					}
				}
			}
		}
	}
}


void CvGame::initFreeState()
{
	const EraTypes iStartEra = getStartEra();

	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		const CvTechInfo& tech = GC.getTechInfo((TechTypes)iI);

		for (int iJ = 0; iJ < MAX_TEAMS; iJ++)
		{
			if (GET_TEAM((TeamTypes)iJ).isAlive())
			{
				bool bValid = false;
				if (tech.getEra() < iStartEra && !tech.isDisable())
				{
					bValid = true;
				}

				if (!bValid)
				{
					for (int iK = 0; iK < MAX_PLAYERS; iK++)
					{
						if (GET_PLAYER((PlayerTypes)iK).isAliveAndTeam((TeamTypes)iJ)
						&& GC.getCivilizationInfo(GET_PLAYER((PlayerTypes)iK).getCivilizationType()).isCivilizationFreeTechs(iI))
						{
							bValid = true;
							break;
						}
					}
				}
				GET_TEAM((TeamTypes)iJ).setHasTech((TechTypes)iI, bValid, NO_PLAYER, false, false);
			}
		}
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			GET_PLAYER((PlayerTypes)iI).initFreeState();
		}
	}
}


void CvGame::initFreeUnits()
{
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive()
		&& GET_PLAYER((PlayerTypes)iI).getNumUnits() == 0
		&& GET_PLAYER((PlayerTypes)iI).getNumCities() == 0)
		{
			GET_PLAYER((PlayerTypes)iI).initFreeUnits();
		}
	}
}


void setBestStartingPlotFromSet(CvPlayerAI* playerX, std::vector<CvPlot*>& startingPlots)
{
	playerX->AI_updateFoundValues(true);

	int iBestValue = 0;
	CvPlot* pBestPlot = startingPlots.front();
	int iBestPlot = 0;

	for (uint16_t iJ = 0; iJ < startingPlots.size(); iJ++)
	{
		int iValue = startingPlots[iJ]->getFoundValue(playerX->getID());
		iValue += GC.getGame().getSorenRandNum(500 + iValue/50, "Randomize Starting Location");

		if (iValue > iBestValue)
		{
			iBestValue = iValue;
			iBestPlot = iJ;
			pBestPlot = startingPlots[iJ];
		}
	}
	playerX->setStartingPlot(pBestPlot, true);
	startingPlots.erase(startingPlots.begin() + iBestPlot);
}


void CvGame::assignStartingPlots(const bool bScenario, const bool bMapScript)
{
	PROFILE_FUNC();
	gDLL->callUpdater(); // allow window updates during launch

	if (!bScenario && !bMapScript)
	{
		// Python override - Some mapscripts overide
		bool bAssignStartingPlots = false;

		if (Cy::call_override(GC.getMap().getMapScript(), "assignStartingPlots", bAssignStartingPlots) && bAssignStartingPlots)
		{
			gDLL->callUpdater();
			return;
		}
	}
	std::vector<CvPlayerAI*> alivePlayers;
	{
		bool bAllDone = true;
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			CvPlayerAI* playerX = &GET_PLAYER((PlayerTypes)iI);

			if (playerX->isAlive())
			{
				alivePlayers.push_back(playerX);

				if (bAllDone && playerX->getStartingPlot() == NULL)
				{
					bAllDone = false;
				}
			}
		}
		gDLL->callUpdater();
		if (bAllDone)
		{
			return;
		}
	}
	const int iNumPlayers = alivePlayers.size();
	std::vector<CvPlot*> startingPlots;

	// Scenario may arbitrary define the starting plot for only some of the player slots.
	// World mapscript only assigns starting plot flag to plots, not starting plot to players.
	if (bScenario || bMapScript)
	{
		for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
		{
			CvPlot* pPlot = GC.getMap().plotByIndex(iI);

			if (pPlot->isStartingPlot())
			{
				if (bScenario)
				{
					// Only scenarios assign starting plots to players.
					// Check if the starting plot is already assigned any players.
					for (int iJ = 0; iJ < iNumPlayers; iJ++)
					{
						if (alivePlayers[iJ]->getStartingPlot() == pPlot)
						{
							pPlot = NULL;
							break;
						}
					}
					if (!pPlot)
					{
						continue;
					}
				}
				startingPlots.push_back(pPlot);
			}
		}
		gDLL->callUpdater();
	}
	std::vector<CvPlayerAI*> playersWaiting;

	for (int iI = 0; iI < iNumPlayers; iI++)
	{
		CvPlayerAI* playerX = alivePlayers[iI];

		if (playerX->getStartingPlot() == NULL)
		{
			playersWaiting.push_back(playerX);
		}
	}
	algo::random_shuffle(playersWaiting, SorenRand("start plot assignment order shuffle"));
	gDLL->callUpdater();

	for (int iI = playersWaiting.size() - 1; iI > -1; iI--)
	{
		if (!startingPlots.empty())
		{
			setBestStartingPlotFromSet(playersWaiting[iI], startingPlots);
		}
		else playersWaiting[iI]->setStartingPlot(playersWaiting[iI]->findStartingPlot(bScenario), true);
	}
	gDLL->callUpdater();
}

// Swaps starting locations until we have reached the optimal closeness between teams
// (caveat: this isn't quite "optimal" because we could get stuck in local minima, but it's pretty good)
// Toffer - This conflict a bit with the "starting plot quality by difficulty" setup.
void CvGame::normalizeStartingPlotLocations()
{
	int* aaiDistances[MAX_PC_PLAYERS];
	int aiStartingLocs[MAX_PC_PLAYERS];

	// Precalculate distances between all starting positions:
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			gDLL->callUpdater();	// allow window to update during launch
			aaiDistances[iI] = new int[iI];
			for (int iJ = 0; iJ < iI; iJ++)
			{
				aaiDistances[iI][iJ] = 0;
			}
			CvPlot *pPlotI = GET_PLAYER((PlayerTypes)iI).getStartingPlot();
			if (pPlotI != NULL)
			{
				for (int iJ = 0; iJ < iI; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive())
					{
						const CvPlot *pPlotJ = GET_PLAYER((PlayerTypes)iJ).getStartingPlot();
						if (pPlotJ != NULL)
						{
							int iDist = GC.getMap().calculatePathDistance(pPlotI, pPlotJ);
							if (iDist == -1)
							{
								// 5x penalty for not being on the same area, or having no passable route
								iDist = 5*plotDistance(pPlotI->getX(), pPlotI->getY(), pPlotJ->getX(), pPlotJ->getY());
							}
							aaiDistances[iI][iJ] = iDist;
						}
					}
				}
			}
		}
		else aaiDistances[iI] = NULL;
	}

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		aiStartingLocs[iI] = iI; // each player starting in own location
	}

	int iBestScore = getTeamClosenessScore(aaiDistances, aiStartingLocs);
	bool bFoundSwap = true;
	while (bFoundSwap)
	{
		bFoundSwap = false;
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				for (int iJ = 0; iJ < iI; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive())
					{
						int iTemp = aiStartingLocs[iI];
						aiStartingLocs[iI] = aiStartingLocs[iJ];
						aiStartingLocs[iJ] = iTemp;
						const int iScore = getTeamClosenessScore(aaiDistances, aiStartingLocs);
						if (iScore < iBestScore)
						{
							iBestScore = iScore;
							bFoundSwap = true;
						}
						else
						{
							// Swap them back:
							iTemp = aiStartingLocs[iI];
							aiStartingLocs[iI] = aiStartingLocs[iJ];
							aiStartingLocs[iJ] = iTemp;
						}
					}
				}
			}
		}
	}

	CvPlot* apNewStartPlots[MAX_PC_PLAYERS];

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && aiStartingLocs[iI] != iI)
		{
			apNewStartPlots[iI] = GET_PLAYER((PlayerTypes)aiStartingLocs[iI]).getStartingPlot();
		}
		else apNewStartPlots[iI] = NULL;
	}

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && apNewStartPlots[iI] != NULL)
		{
			GET_PLAYER((PlayerTypes)iI).setStartingPlot(apNewStartPlots[iI], false);
		}
	}

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		SAFE_DELETE_ARRAY(aaiDistances[iI]);
	}
}


void CvGame::normalizeAddRiver()
{
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			CvPlot* pStartingPlot = GET_PLAYER((PlayerTypes)iI).getStartingPlot();

			if (pStartingPlot != NULL && !pStartingPlot->isFreshWater())
			{
				// if we will be able to add a lake, then use old river code
				if (normalizeFindLakePlot((PlayerTypes)iI) != NULL)
				{
					CvMapGenerator::GetInstance().doRiver(pStartingPlot);
				}
				// otherwise, use new river code which is much more likely to succeed
				else
				{
					CvMapGenerator::GetInstance().addRiver(pStartingPlot);
				}

				// add floodplains to any desert tiles the new river passes through
				for (int iK = 0; iK < GC.getMap().numPlots(); iK++)
				{
					CvPlot* pPlot = GC.getMap().plotByIndex(iK);
					FAssert(pPlot != NULL);

					for (int iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
					{
						if (GC.getFeatureInfo((FeatureTypes)iJ).isRequiresRiver() && pPlot->canHaveFeature((FeatureTypes)iJ))
						{
							if (GC.getFeatureInfo((FeatureTypes)iJ).getAppearanceProbability() == 10000)
							{
								if (pPlot->getBonusType() != NO_BONUS)
								{
									pPlot->setBonusType(NO_BONUS);
								}
								pPlot->setFeatureType((FeatureTypes)iJ);
								break;
							}
						}
					}
				}
			}
		}
	}
}


void CvGame::normalizeRemovePeaks()
{
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			const CvPlot* pStartingPlot = GET_PLAYER((PlayerTypes)iI).getStartingPlot();

			if (pStartingPlot != NULL)
			{
				algo::for_each(pStartingPlot->rect(3, 3)
					| filtered(bind(CvPlot::isPeak, _1))
					, bind(CvPlot::setPlotType, _1, PLOT_HILLS, true, true)
				);
			}
		}
	}
}

void CvGame::normalizeAddLakes()
{
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			CvPlot* pLakePlot = normalizeFindLakePlot((PlayerTypes)iI);
			if (pLakePlot != NULL)
			{
				pLakePlot->setPlotType(PLOT_OCEAN);
			}
		}
	}
}

CvPlot* CvGame::normalizeFindLakePlot(PlayerTypes ePlayer)
{
	if (GET_PLAYER(ePlayer).isAlive())
	{
		const CvPlot* pStartingPlot = GET_PLAYER(ePlayer).getStartingPlot();

		if (pStartingPlot != NULL && !pStartingPlot->isFreshWater())
		{
			for (int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
			{
				CvPlot* pLoopPlot = plotCity(pStartingPlot->getX(), pStartingPlot->getY(), iJ);

				if (pLoopPlot != NULL && !pLoopPlot->isWater() && !pLoopPlot->isCoastalLand() && !pLoopPlot->isRiver() && pLoopPlot->getBonusType() == NO_BONUS)
				{
					bool bStartingPlot = false;

					for (int iK = 0; iK < MAX_PC_PLAYERS; iK++)
					{
						if (GET_PLAYER((PlayerTypes)iK).isAlive() && GET_PLAYER((PlayerTypes)iK).getStartingPlot() == pLoopPlot)
						{
							bStartingPlot = true;
							break;
						}
					}
					if (!bStartingPlot)
					{
						return pLoopPlot;
					}
				}
			}
		}
	}
	return NULL;
}


void CvGame::normalizeRemoveBadFeatures()
{
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			const CvPlot* pStartingPlot = GET_PLAYER((PlayerTypes)iI).getStartingPlot();

			if (pStartingPlot != NULL)
			{
				for (int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
				{
					CvPlot* pLoopPlot = plotCity(pStartingPlot->getX(), pStartingPlot->getY(), iJ);

					if (pLoopPlot != NULL && pLoopPlot->getFeatureType() != NO_FEATURE
					&& GC.getFeatureInfo(pLoopPlot->getFeatureType()).getYieldChange(YIELD_FOOD) <= 0
					&& GC.getFeatureInfo(pLoopPlot->getFeatureType()).getYieldChange(YIELD_PRODUCTION) <= 0)
					{
						pLoopPlot->setFeatureType(NO_FEATURE);
					}
				}

				const int iMaxRange = CITY_PLOTS_RADIUS + 2;

				foreach_(CvPlot* pLoopPlot, pStartingPlot->rect(iMaxRange, iMaxRange))
				{
					const int iDistance = plotDistance(pStartingPlot->getX(), pStartingPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY());

					if (iDistance <= iMaxRange && pLoopPlot->getFeatureType() != NO_FEATURE
					&& GC.getFeatureInfo(pLoopPlot->getFeatureType()).getYieldChange(YIELD_FOOD) <= 0
					&& GC.getFeatureInfo(pLoopPlot->getFeatureType()).getYieldChange(YIELD_PRODUCTION) <= 0)
					{
						if (pLoopPlot->isWater())
						{
							if (pLoopPlot->isAdjacentToLand() || iDistance != iMaxRange && 0 == getSorenRandNum(2, "Remove Bad Feature"))
							{
								pLoopPlot->setFeatureType(NO_FEATURE);
							}
						}
						else if (iDistance != iMaxRange && 0 == getSorenRandNum(pLoopPlot->getBonusType() == NO_BONUS ? 2 : 4, "Remove Bad Feature"))
						{
							pLoopPlot->setFeatureType(NO_FEATURE);
						}
					}
				}
			}
		}
	}
}


void CvGame::normalizeRemoveBadTerrain()
{
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (!GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			continue;
		}
		const CvPlot* pStartingPlot = GET_PLAYER((PlayerTypes)iI).getStartingPlot();
		if (pStartingPlot == NULL)
		{
			continue;
		}
		const int iX = pStartingPlot->getX();
		const int iY = pStartingPlot->getY();

		for (int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
		{
			CvPlot* plotX = plotCity(iX, iY, iJ);

			if (plotX == NULL || plotX->isWater())
			{
				continue;
			}
			if (GC.getTerrainInfo(plotX->getTerrainType()).getYield(YIELD_FOOD) + GC.getTerrainInfo(plotX->getTerrainType()).getYield(YIELD_PRODUCTION) < 2)
			{
				const CvFeatureInfo* feature = plotX->getFeatureType() == NO_FEATURE ? NULL : &GC.getFeatureInfo(plotX->getFeatureType());
				const CvBonusInfo* bonus = plotX->getBonusType() == NO_BONUS ? NULL : &GC.getBonusInfo(plotX->getBonusType());
				TerrainTypes eTerrain = NO_TERRAIN;
				for (int iK = 0; iK < GC.getNumTerrainInfos(); iK++)
				{
					if ((feature == NULL || feature->isTerrain(iK))
					&& (bonus == NULL || bonus->isTerrain(iK)))
					{
						const CvTerrainInfo& terrainK = GC.getTerrainInfo((TerrainTypes)iK);

						if (!terrainK.isWaterTerrain() && terrainK.getYield(YIELD_FOOD) > 0
						&& terrainK.getYield(YIELD_FOOD) + terrainK.getYield(YIELD_PRODUCTION) > 1)
						{
							eTerrain = (TerrainTypes)iK;
							if (!getSorenRandNum(3, "variance"))
							{
								break;
							}
						}
					}
				}
				if (eTerrain != NO_TERRAIN)
				{
					plotX->setTerrainType(eTerrain);
				}
			}
		}
	}
}


void CvGame::normalizeAddFoodBonuses()
{
	const bool bIgnoreLatitude = pythonIsBonusIgnoreLatitudes();

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (!GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			continue;
		}
		const CvPlot* pStartingPlot = GET_PLAYER((PlayerTypes)iI).getStartingPlot();
		if (pStartingPlot == NULL)
		{
			continue;
		}
		const int iX = pStartingPlot->getX();
		const int iY = pStartingPlot->getY();

		const TeamTypes eTeam = GET_PLAYER((PlayerTypes)iI).getTeam();
		int iGoodNatureTileCount = 0;
		int iFoodBonus = 0;
		for (int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
		{
			const CvPlot* plotX = plotCity(iX, iY, iJ);

			if (plotX != NULL)
			{
				const BonusTypes eBonus = plotX->getBonusType(eTeam);

				if (eBonus != NO_BONUS)
				{
					if (GC.getBonusInfo(eBonus).getYieldChange(YIELD_FOOD) > 0)
					{
						if (GC.getBonusInfo(eBonus).getTechCityTrade() == NO_TECH || GC.getTechInfo((TechTypes)GC.getBonusInfo(eBonus).getTechCityTrade()).getEra() <= getStartEra())
						{
							if (plotX->isWater())
								iFoodBonus += 2;
							else iFoodBonus += 3;
						}
					}
					else if (plotX->calculateBestNatureYield(YIELD_FOOD, eTeam) >= 2)
					{
						iGoodNatureTileCount++;
					}
				}
				else if (plotX->calculateBestNatureYield(YIELD_FOOD, eTeam) >= 3)
				{
					iGoodNatureTileCount++;
				}
			}
		}
		const int iTargetFoodBonusCount = 3 + (iGoodNatureTileCount == 0 ? 2 : 0);

		for (int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
		{
			if (iFoodBonus >= iTargetFoodBonusCount)
			{
				break;
			}
			CvPlot* plotX = plotCity(iX, iY, iJ);

			if (plotX != NULL && plotX != pStartingPlot && plotX->getBonusType() == NO_BONUS)
			{
				for (int iK = 0; iK < GC.getNumBonusInfos(); iK++)
				{
					const CvBonusInfo& bonus = GC.getBonusInfo((BonusTypes)iK);

					if (bonus.isNormalize() && bonus.getYieldChange(YIELD_FOOD) > 0
					&& (bonus.getTechCityTrade() == NO_TECH || GC.getTechInfo((TechTypes)bonus.getTechCityTrade()).getEra() <= getStartEra())
					&& plotX->canHaveBonus(((BonusTypes)iK), bIgnoreLatitude))
					{
						if (plotX->isWater())
							iFoodBonus += 2;
						else iFoodBonus += 3;

						plotX->setBonusType((BonusTypes)iK);
						break;
					}
				}
			}
		}
	}
}


void CvGame::normalizeAddGoodTerrain()
{
	const int iConsumption = GC.getFOOD_CONSUMPTION_PER_POPULATION();

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (!GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			continue;
		}
		const CvPlot* pStartingPlot = GET_PLAYER((PlayerTypes)iI).getStartingPlot();

		if (pStartingPlot == NULL)
		{
			continue;
		}
		const TeamTypes eTeam = GET_PLAYER((PlayerTypes)iI).getTeam();
		int iGoodPlot = 0;
		const int iStartX = pStartingPlot->getX();
		const int iStartY = pStartingPlot->getY();

		for (int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
		{
			const CvPlot* plot = plotCity(iStartX, iStartY, iJ);

			if (plot != NULL && plot != pStartingPlot)
			{
				if (plot->calculateNatureYield(YIELD_FOOD, eTeam) >= iConsumption
				&&  plot->calculateNatureYield(YIELD_PRODUCTION, eTeam) > 0)
				{
					iGoodPlot++;
				}
			}
		}

		for (int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
		{
			if (iGoodPlot >= 4)
			{
				break;
			}

			CvPlot* plot = plotCity(iStartX, iStartY, iJ);

			if (plot != NULL && plot != pStartingPlot
			&& !plot->isWater() && !plot->isHills()
			&&  plot->getBonusType() == NO_BONUS)
			{
				bool bChanged = false;

				if (plot->calculateNatureYield(YIELD_FOOD, eTeam) < iConsumption)
				{
					for (int iK = 0; iK < GC.getNumTerrainInfos(); iK++)
					{
						if (!GC.getTerrainInfo((TerrainTypes)iK).isWaterTerrain()
						&& GC.getTerrainInfo((TerrainTypes)iK).getYield(YIELD_FOOD) >= iConsumption)
						{
							plot->setTerrainType((TerrainTypes)iK);
							bChanged = true;
							break;
						}
					}
				}
				if (plot->calculateNatureYield(YIELD_PRODUCTION, eTeam) == 0)
				{
					for (int iK = 0; iK < GC.getNumFeatureInfos(); iK++)
					{
						if (GC.getFeatureInfo((FeatureTypes)iK).getYieldChange(YIELD_FOOD) >= 0
						&&  GC.getFeatureInfo((FeatureTypes)iK).getYieldChange(YIELD_PRODUCTION) > 0
						&&  GC.getFeatureInfo((FeatureTypes)iK).isTerrain(plot->getTerrainType()))
						{
							plot->setFeatureType((FeatureTypes)iK);
							bChanged = true;
							break;
						}
					}
				}
				if (bChanged)
				{
					iGoodPlot++;
				}
			}
		}
	}
}


void CvGame::normalizeAddExtras()
{
	bool bIgnoreLatitude = pythonIsBonusIgnoreLatitudes();

	int iTotalValue = 0;
	int iPlayerCount = 0;
	int iBestValue = 0;
	int iWorstValue = MAX_INT;
	int	iI, iJ, iK;

	for (iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			const CvPlot* pStartingPlot = GET_PLAYER((PlayerTypes)iI).getStartingPlot();

			if (pStartingPlot != NULL)
			{
				int iValue = GET_PLAYER((PlayerTypes)iI).AI_foundValue(pStartingPlot->getX(), pStartingPlot->getY(), -1, true);
				iTotalValue += iValue;
				iPlayerCount++;

				iBestValue = std::max(iValue, iBestValue);
				iWorstValue = std::min(iValue, iWorstValue);
			}
		}
	}

	//iTargetValue = (iTotalValue + iBestValue) / (iPlayerCount + 1);
	int iTargetValue = (iBestValue * 4) / 5;

	for (iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			gDLL->callUpdater();	// allow window to update during launch
			CvPlot* pStartingPlot = GET_PLAYER((PlayerTypes)iI).getStartingPlot();

			if (pStartingPlot != NULL)
			{
				int iCount = 0;
				int iFeatureCount = 0;
				int aiShuffle[NUM_CITY_PLOTS];
				shuffleArray(aiShuffle, NUM_CITY_PLOTS, getMapRand());

				for (iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iI).AI_foundValue(pStartingPlot->getX(), pStartingPlot->getY(), -1, true) >= iTargetValue)
					{
						break;
					}
					if (getSorenRandNum(iCount + 2, "Setting Feature Type") <= 1)
					{
						CvPlot* plotX = plotCity(pStartingPlot->getX(), pStartingPlot->getY(), aiShuffle[iJ]);

						if (plotX != NULL && plotX != pStartingPlot && plotX->getBonusType() == NO_BONUS)
						{
							if (plotX->getFeatureType() == NO_FEATURE)
							{
								for (int iK = 0; iK < GC.getNumFeatureInfos(); iK++)
								{
									if (GC.getFeatureInfo((FeatureTypes)iK).getAppearanceProbability() > -1
									&& GC.getFeatureInfo((FeatureTypes)iK).getYieldChange(YIELD_FOOD) + GC.getFeatureInfo((FeatureTypes)iK).getYieldChange(YIELD_PRODUCTION) > 0
									&& plotX->canHaveFeature((FeatureTypes)iK))
									{
										plotX->setFeatureType((FeatureTypes)iK);
										iCount++;
										iFeatureCount++;
										break;
									}
								}
							}
							else iFeatureCount++;
						}
					}
				}

				int iCoastFoodCount = 0;
				int iOceanFoodCount = 0;
				int iOtherCount = 0;
				int iWaterCount = 0;
				for (iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
				{
					const CvPlot* plotX = plotCity(pStartingPlot->getX(), pStartingPlot->getY(), iJ);
					if (plotX != NULL && plotX != pStartingPlot)
					{
						if (plotX->isWater())
						{
							iWaterCount++;
							if (plotX->getBonusType() != NO_BONUS)
							{
								if (plotX->isAdjacentToLand())
								{
									iCoastFoodCount++;
								}
								else
								{
									iOceanFoodCount++;
								}
							}
						}
						else if (plotX->getBonusType() != NO_BONUS)
						{
							iOtherCount++;
						}
					}
				}

				bool bLandBias = (iWaterCount > NUM_CITY_PLOTS / 2);

				shuffleArray(aiShuffle, NUM_CITY_PLOTS, getMapRand());

				for (iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
				{
					CvPlot* plotX = plotCity(pStartingPlot->getX(), pStartingPlot->getY(), aiShuffle[iJ]);

					if (plotX != NULL && plotX != pStartingPlot)
					{
						if (getSorenRandNum(((bLandBias && plotX->isWater()) ? 2 : 1), "Placing Bonuses") == 0)
						{
							if ((iOtherCount * 3 + iOceanFoodCount * 2 + iCoastFoodCount * 2) >= 12)
							{
								break;
							}

							if (GET_PLAYER((PlayerTypes)iI).AI_foundValue(pStartingPlot->getX(), pStartingPlot->getY(), -1, true) >= iTargetValue)
							{
								break;
							}

							bool bCoast = (plotX->isWater() && plotX->isAdjacentToLand());
							bool bOcean = (plotX->isWater() && !bCoast);
							if ((plotX != pStartingPlot)
								&& !(bCoast && (iCoastFoodCount > 2))
								&& !(bOcean && (iOceanFoodCount > 2)))
							{
								for (int iPass = 0; iPass < 2; iPass++)
								{
									if (plotX->getBonusType() == NO_BONUS)
									{
										for (iK = 0; iK < GC.getNumBonusInfos(); iK++)
										{
											if (GC.getBonusInfo((BonusTypes)iK).isNormalize())
											{
												//???no bonuses with negative yields?
												if ((GC.getBonusInfo((BonusTypes)iK).getYieldChange(YIELD_FOOD) >= 0) &&
													  (GC.getBonusInfo((BonusTypes)iK).getYieldChange(YIELD_PRODUCTION) >= 0))
												{
													if ((GC.getBonusInfo((BonusTypes)iK).getTechCityTrade() == NO_TECH) || (GC.getTechInfo((TechTypes)(GC.getBonusInfo((BonusTypes)iK).getTechCityTrade())).getEra() <= getStartEra()))
													{
														if (GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasTech((TechTypes)(GC.getBonusInfo((BonusTypes)iK).getTechReveal())))
														{
															if ((iPass == 0) ? CvMapGenerator::GetInstance().canPlaceBonusAt(((BonusTypes)iK), plotX->getX(), plotX->getY(), bIgnoreLatitude) : plotX->canHaveBonus(((BonusTypes)iK), bIgnoreLatitude))
															{
																plotX->setBonusType((BonusTypes)iK);
																iCoastFoodCount += bCoast ? 1 : 0;
																iOceanFoodCount += bOcean ? 1 : 0;
																iOtherCount += !(bCoast || bOcean) ? 1 : 0;
																break;
															}
														}
													}
												}
											}
										}

										if (bLandBias && !plotX->isWater() && plotX->getBonusType() == NO_BONUS)
										{
											if (((iFeatureCount > 4) && (plotX->getFeatureType() != NO_FEATURE))
												&& ((iCoastFoodCount + iOceanFoodCount) > 2))
											{
												if (getSorenRandNum(2, "Clear feature to add bonus") == 0)
												{
													plotX->setFeatureType(NO_FEATURE);

													for (iK = 0; iK < GC.getNumBonusInfos(); iK++)
													{
														if (GC.getBonusInfo((BonusTypes)iK).isNormalize())
														{
															//???no bonuses with negative yields?
															if ((GC.getBonusInfo((BonusTypes)iK).getYieldChange(YIELD_FOOD) >= 0) &&
																  (GC.getBonusInfo((BonusTypes)iK).getYieldChange(YIELD_PRODUCTION) >= 0))
															{
																if ((GC.getBonusInfo((BonusTypes)iK).getTechCityTrade() == NO_TECH) || (GC.getTechInfo((TechTypes)(GC.getBonusInfo((BonusTypes)iK).getTechCityTrade())).getEra() <= getStartEra()))
																{
																	if ((iPass == 0) ? CvMapGenerator::GetInstance().canPlaceBonusAt(((BonusTypes)iK), plotX->getX(), plotX->getY(), bIgnoreLatitude) : plotX->canHaveBonus(((BonusTypes)iK), bIgnoreLatitude))
																	{
																		plotX->setBonusType((BonusTypes)iK);
																		iOtherCount++;
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
								}
							}
						}
					}
				}
				shuffleArray(aiShuffle, NUM_CITY_PLOTS, getMapRand());

				for (iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iI).AI_foundValue(pStartingPlot->getX(), pStartingPlot->getY(), -1, true) >= iTargetValue)
					{
						break;
					}

					CvPlot* plotX = plotCity(pStartingPlot->getX(), pStartingPlot->getY(), aiShuffle[iJ]);

					if (plotX != NULL && plotX != pStartingPlot
					&& plotX->getBonusType() == NO_BONUS
					&& plotX->getFeatureType() == NO_FEATURE)
					{
						for (iK = 0; iK < GC.getNumFeatureInfos(); iK++)
						{
							if (GC.getFeatureInfo((FeatureTypes)iK).getAppearanceProbability() > -1
							&& GC.getFeatureInfo((FeatureTypes)iK).getYieldChange(YIELD_FOOD) + GC.getFeatureInfo((FeatureTypes)iK).getYieldChange(YIELD_PRODUCTION) > 0
							&& plotX->canHaveFeature((FeatureTypes)iK))
							{
								plotX->setFeatureType((FeatureTypes)iK);
								break;
							}
						}
					}
				}

				int iHillsCount = 0;

				for (iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
				{
					const CvPlot* plotX = plotCity(pStartingPlot->getX(), pStartingPlot->getY(), iJ);

					if (plotX != NULL && plotX->isHills())
					{
						iHillsCount++;
					}
				}
				shuffleArray(aiShuffle, NUM_CITY_PLOTS, getMapRand());
				for (iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
				{
					if (iHillsCount >= 3)
					{
						break;
					}
					CvPlot* plotX = plotCity(pStartingPlot->getX(), pStartingPlot->getY(), aiShuffle[iJ]);

					if (plotX != NULL && !plotX->isWater() && !plotX->isHills()
					&& (plotX->getFeatureType() == NO_FEATURE || !GC.getFeatureInfo(plotX->getFeatureType()).isRequiresFlatlands())
					&& (plotX->getBonusType() == NO_BONUS || GC.getBonusInfo(plotX->getBonusType()).isHills()))
					{
						plotX->setPlotType(PLOT_HILLS, false, true);
						iHillsCount++;
					}
				}
			}
		}
	}
}


void CvGame::normalizeStartingPlots()
{
	PROFILE_FUNC();

	if ((!GC.getInitCore().getWBMapScript() || GC.getInitCore().getWBMapNoPlayers())
	&& !Cy::call_override(GC.getMap().getMapScript(), "normalizeStartingPlotLocations")
	&& !isOption(GAMEOPTION_CULTURALLY_LINKED_STARTS))
	{
		normalizeStartingPlotLocations();
	}

	if (GC.getInitCore().getWBMapScript())
	{
		return;
	}

	if (!Cy::call_override(GC.getMap().getMapScript(), "normalizeAddRiver")  || gDLL->getPythonIFace()->pythonUsingDefaultImpl())
	{
		normalizeAddRiver();
	}

	if (!Cy::call_override(GC.getMap().getMapScript(), "normalizeRemovePeaks"))
	{
		normalizeRemovePeaks();
	}

	if (!Cy::call_override(GC.getMap().getMapScript(), "normalizeAddLakes"))
	{
		normalizeAddLakes();
	}

	if (!Cy::call_override(GC.getMap().getMapScript(), "normalizeRemoveBadFeatures"))
	{
		normalizeRemoveBadFeatures();
	}

	if (!Cy::call_override(GC.getMap().getMapScript(), "normalizeRemoveBadTerrain"))
	{
		normalizeRemoveBadTerrain();
	}

	if (!Cy::call_override(GC.getMap().getMapScript(), "normalizeAddFoodBonuses"))
	{
		normalizeAddFoodBonuses();
	}

	if (!Cy::call_override(GC.getMap().getMapScript(), "normalizeAddGoodTerrain"))
	{
		normalizeAddGoodTerrain();
	}

	if (!Cy::call_override(GC.getMap().getMapScript(), "normalizeAddExtras"))
	{
		normalizeAddExtras();
	}
}

// For each of n teams, let the closeness score for that team be the average distance of an edge between two players on that team.
// This function calculates the closeness score for each team and returns the sum of those n scores.
// The lower the result, the better "clumped" the players' starting locations are.
//
// Note: for the purposes of this function, player i will be assumed to start in the location of player aiStartingLocs[i]

int CvGame::getTeamClosenessScore(int** aaiDistances, int* aiStartingLocs)
{
	int iScore = 0;

	for (int iTeam = 0; iTeam < MAX_PC_TEAMS; iTeam++)
	{
		if (GET_TEAM((TeamTypes)iTeam).isAlive())
		{
			int iTeamTotalDist = 0;
			int iNumEdges = 0;
			for (int iPlayerA = 0; iPlayerA < MAX_PC_PLAYERS; iPlayerA++)
			{
				if (GET_PLAYER((PlayerTypes)iPlayerA).isAliveAndTeam((TeamTypes)iTeam))
				{
					for (int iPlayerB = 0; iPlayerB < iPlayerA; iPlayerB++)
					{
						if (GET_PLAYER((PlayerTypes)iPlayerB).getTeam() == (TeamTypes)iTeam)
						{
							// Add the edge between these two players that are on the same team
							iNumEdges++;
							int iPlayerStart = aiStartingLocs[iPlayerA];
							int iOtherPlayerStart = aiStartingLocs[iPlayerB];

							if (iPlayerStart < iOtherPlayerStart) // Make sure that iPlayerStart > iOtherPlayerStart
							{
								int iTemp = iPlayerStart;
								iPlayerStart = iOtherPlayerStart;
								iOtherPlayerStart = iTemp;
							}
							else if (iPlayerStart == iOtherPlayerStart)
							{
								FErrorMsg("Two players are (hypothetically) assigned to the same starting location!");
							}
							iTeamTotalDist += aaiDistances[iPlayerStart][iOtherPlayerStart];
						}
					}
				}
			}
			if (iNumEdges != 0)
			{
				// the avg distance between team edges is the team score
				iScore += iTeamTotalDist / iNumEdges;
			}
		}
	}
	return iScore;
}

void CvGame::update()
{
#ifdef LOG_AI
	gPlayerLogLevel = getBugOptionINT("Autolog__BBAILevel", 0);
	gTeamLogLevel = gPlayerLogLevel;
	gCityLogLevel = gPlayerLogLevel;
	gUnitLogLevel = gPlayerLogLevel;

	if ( gPlayerLogLevel > 0 )
	{
		GC.setXMLLogging(true);
	}
#endif

	startProfilingDLL(false);

	CvPlotPaging::UpdatePaging();

	//OutputDebugString(CvString::format("Start profiling(false) for CvGame::update()\n").c_str());
	PROFILE_BEGIN("CvGame::update");

	if ( !m_plotGroupHashesInitialized )
	{
		PROFILE("CvGame::update.OneTimeInit");

		//	First opportunity after a load or save is here since sadly the DLL structure
		//	gives us no load/save-completed event
		CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

		//	Close will free any resources and display any warnings if we've just finished loading/saving
		wrapper.close();

		GC.getLoadedInitCore().checkVersions();

		for(int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				GET_PLAYER((PlayerTypes)iI).RecalculatePlotGroupHashes();
			}
		}

		m_plotGroupHashesInitialized = true;

		gDLL->getEngineIFace()->clearSigns();
		gDLL->getEngineIFace()->setResourceLayer(GC.getResourceLayer());

		//	Remove and re-add the GW on load
		processGreatWall(false, true, false);
		processGreatWall(true, true);

		// Recalculate vision on load (a stickytape - can't find where it's dropping visibility on loading)

		//The tracking really cannot work unless it starts right.  Plus, this is not a bad procedural step just in case it's coming from an old save or somesuch.  Doesn't cost us much time on load.
		for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
		{
			CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);
			pLoopPlot->unitGameStateCorrections();
			pLoopPlot->clearVisibilityCounts();
			CvCity* pCity = pLoopPlot->getPlotCity();
			if (pCity != NULL && pCity->isNPC() && pCity->isRevealed(GET_PLAYER(getActivePlayer()).getTeam(), false))
			{
				pCity->updateVisibility();
			}
		}
		GC.getMap().updateSight(true, false);

		for (int iI = 0; iI < NUM_GAMEOPTION_TYPES; iI++)
		{
			const GameOptionTypes eGameOption = ((GameOptionTypes)iI);
			if (isOption(eGameOption))
			{
				enforceOptionCompatibility(eGameOption);
			}
		}

		//TB: Set Statuses
		setStatusPromotions();

		//	Super forts adaptation to C2C - make sure this map has had
		//	its choke points calculated - note we check this every turn
		//	because of a future intent to force periodic relcalulation
		//	on significant events (discovery of mountaineering by someone,
		//	terra-forming leading to water<->land transformations, etc.)
		ensureChokePointsEvaluated();
		gDLL->getInterfaceIFace()->setEndTurnCounter(2 * getBugOptionINT("MainInterface__AutoEndTurnDelay", 2));
		// Toffer - Attempt to fix the lack of unit cycle process on load
		gDLL->getInterfaceIFace()->setCycleSelectionCounter(1);
	}

	{
		PROFILE("CvGame::update.ViewportInit");

		//	Some processing that is done in viewport tranisitions has to occur
		//	over several messaging timeslices (empirically - the game engine
		//	gets things wrong if you don't give it a chnace to process messages
		//	in between).  To enact that we use a state machine which performs state
		//	transitions on each call to CvGame::update().
		GC.getCurrentViewport()->processActionState();
		//	On each time slice put back the signs if they have been temporarily removed
		//	(as happens across a save or a map/viewport switch)
		GC.reprocessSigns();
		//	If a rebuild of the graphics is needed and about a second has gone by since a new refresh was last
		//	requested perform the rebuild (this amalgamates all the rebuild requests pertaining to a move or
		//	a sequence of moves)
		if ( m_lastGraphicUpdateRequestTickCount != -1 && (GetTickCount() - m_lastGraphicUpdateRequestTickCount) > 500 )
		{
			m_lastGraphicUpdateRequestTickCount = -1;

			gDLL->getEngineIFace()->RebuildAllPlots();
			gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);
		}
	}

again:
	if (!gDLL->GetWorldBuilderMode() || isInAdvancedStart())
	{
		PROFILE("CvGame::update.Turn");

		sendPlayerOptions();
		// sample generic event
		CyArgsList pyArgs;
		pyArgs.add(getTurnSlice());
		CvEventReporter::getInstance().genericEvent("gameUpdate", pyArgs.makeFunctionArgs());

		if (getTurnSlice() == 0)
		{
			PROFILE("CvGame::update.AutoSave");

			gDLL->getEngineIFace()->AutoSave(true);
		}

		if (getNumGameTurnActive() == 0 && (!isPbem() || !getPbemTurnSent()))
		{
			doTurn();
		}
		updateScore();
		updateMoves();
		updateTimers();
		updateTurnTimer();

		AI_updateAssignWork();
		testAlive();

		if (getAIAutoPlay(getActivePlayer()) <= 0 && !gDLL->GetAutorun() && GAMESTATE_EXTENDED != getGameState() && countHumanPlayersAlive() == 0)
		{
			setGameState(GAMESTATE_OVER);
		}
		changeTurnSlice(1);

		if (NO_PLAYER != getActivePlayer() && GET_PLAYER(getActivePlayer()).getAdvancedStartPoints() >= 0 && !gDLL->getInterfaceIFace()->isInAdvancedStart())
		{
			gDLL->getInterfaceIFace()->setInAdvancedStart(true);
			gDLL->getInterfaceIFace()->setWorldBuilder(true);
		}
	}

	//OutputDebugString(CvString::format("Stop profiling(false) after CvGame::update()\n").c_str());
	const CvPlayerAI& kActivePlayer = GET_PLAYER(getActivePlayer());

	if (!isGameMultiPlayer() && getBugOptionBOOL("MainInterface__MinimizeAITurnSlices", false)
	&& (!kActivePlayer.isTurnActive() || kActivePlayer.isAutoMoves() && kActivePlayer.isOption(PLAYEROPTION_QUICK_MOVES))
	// Toffer - isAlive check is needed for the "you have been defeated" popups to appear as they should.
	// Without it the game will just pass turns between the AI's without ever refreshing your screen, making it seem like the game freezed the moment you were defeated.
	&& kActivePlayer.isAlive())
	{
		updateTimers();
		goto again;
	}

	PROFILE_END();
	stopProfilingDLL(false);
}


void CvGame::updateScore(bool bForce)
{
	PROFILE_FUNC();


	bool abPlayerScored[MAX_PC_PLAYERS];
	bool abTeamScored[MAX_PC_TEAMS];
	int iScore;
	int iBestScore;
	PlayerTypes eBestPlayer;
	TeamTypes eBestTeam;
	int iI, iJ, iK;

	if (!isScoreDirty() && !bForce)
	{
		return;
	}

	setScoreDirty(false);

	for (iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		abPlayerScored[iI] = false;
	}

	for (iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		iBestScore = MIN_INT;
		eBestPlayer = NO_PLAYER;

		for (iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
		{
			if (!abPlayerScored[iJ])
			{
				iScore = GET_PLAYER((PlayerTypes)iJ).calculateScore(false);

				if (iScore >= iBestScore)
				{
					iBestScore = iScore;
					eBestPlayer = (PlayerTypes)iJ;
				}
			}
		}

		abPlayerScored[eBestPlayer] = true;

		setRankPlayer(iI, eBestPlayer);
		setPlayerRank(eBestPlayer, iI);
		setPlayerScore(eBestPlayer, iBestScore);
		GET_PLAYER(eBestPlayer).updateScoreHistory(getGameTurn(), iBestScore);
	}

	for (iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		abTeamScored[iI] = false;
	}

	for (iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		iBestScore = MIN_INT;
		eBestTeam = NO_TEAM;

		for (iJ = 0; iJ < MAX_PC_TEAMS; iJ++)
		{
			if (!abTeamScored[iJ])
			{
				iScore = 0;

				for (iK = 0; iK < MAX_PC_PLAYERS; iK++)
				{
					if (GET_PLAYER((PlayerTypes)iK).getTeam() == iJ)
					{
						iScore += getPlayerScore((PlayerTypes)iK);
					}
				}

				if (iScore >= iBestScore)
				{
					iBestScore = iScore;
					eBestTeam = (TeamTypes)iJ;
				}
			}
		}

		abTeamScored[eBestTeam] = true;

		setRankTeam(iI, eBestTeam);
		setTeamRank(eBestTeam, iI);
		setTeamScore(eBestTeam, iBestScore);
	}
}

void CvGame::updatePlotGroups(bool reInitialize)
{
	PROFILE_FUNC();

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			GET_PLAYER((PlayerTypes)iI).updatePlotGroups(NULL, reInitialize);
		}
	}
}


void CvGame::updateBuildingCommerce()
{
	PROFILE_FUNC();

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			GET_PLAYER((PlayerTypes)iI).updateBuildingCommerce();
		}
	}
}


void CvGame::updateCitySight(bool bIncrement)
{
	PROFILE_FUNC();

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			GET_PLAYER((PlayerTypes)iI).updateCitySight(bIncrement, false);
		}
	}
	updatePlotGroups();
}


void CvGame::updateTradeRoutes()
{
	PROFILE_FUNC();

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			GET_PLAYER((PlayerTypes)iI).updateTradeRoutes();
		}
	}
}


void CvGame::testExtendedGame()
{
	PROFILE_FUNC();

	if (getGameState() != GAMESTATE_OVER)
	{
		return;
	}
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if(GET_PLAYER((PlayerTypes)iI).isAlive()
		&& GET_PLAYER((PlayerTypes)iI).isHuman()
		&& GET_PLAYER((PlayerTypes)iI).isExtendedGame())
		{
			setGameState(GAMESTATE_EXTENDED);
			break;
		}
	}
}


void CvGame::cityPushOrder(const CvCity* pCity, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl) const
{
	CvMessageControl::getInstance().sendPushOrder(pCity->getID(), eOrder, iData, bAlt, bShift, bCtrl);
}


void CvGame::selectUnit(CvUnit* pUnit, bool bClear, bool bToggle, bool bSound) const
{
	PROFILE_FUNC();

	bool bSelectGroup;
	bool bGroup;

	if (gDLL->getInterfaceIFace()->getHeadSelectedUnit() == NULL)
	{
		bSelectGroup = true;
	}
	else if (gDLL->getInterfaceIFace()->getHeadSelectedUnit()->getGroup() != pUnit->getGroup())
	{
		bSelectGroup = true;
	}
	else if (pUnit->IsSelected() && !(gDLL->getInterfaceIFace()->mirrorsSelectionGroup()))
	{
		bSelectGroup = !bToggle;
	}
	else
	{
		bSelectGroup = false;
	}

	gDLL->getInterfaceIFace()->clearSelectedCities();

	if (bClear)
	{
		gDLL->getInterfaceIFace()->clearSelectionList();
		bGroup = false;
	}
	else
	{
		bGroup = gDLL->getInterfaceIFace()->mirrorsSelectionGroup();
	}

	if (bSelectGroup)
	{
		const CvSelectionGroup* pSelectionGroup = pUnit->getGroup();

		gDLL->getInterfaceIFace()->selectionListPreChange();

		foreach_(CvUnit* pLoopUnit, pSelectionGroup->units())
		{
			gDLL->getInterfaceIFace()->insertIntoSelectionList(pLoopUnit, false, bToggle, bGroup, bSound, true);
		}

		gDLL->getInterfaceIFace()->selectionListPostChange();
	}
	else
	{
		gDLL->getInterfaceIFace()->insertIntoSelectionList(pUnit, false, bToggle, bGroup, bSound);
	}

	gDLL->getInterfaceIFace()->makeSelectionListDirty();
}


void CvGame::selectGroup(CvUnit* pUnit, bool bShift, bool bCtrl, bool bAlt) const
{
	PROFILE_FUNC();
	FAssertMsg(pUnit != NULL, "pUnit == NULL unexpectedly");

	if (bAlt || bCtrl)
	{
		gDLL->getInterfaceIFace()->clearSelectedCities();

		if (!bShift)
		{
			gDLL->getInterfaceIFace()->clearSelectionList();
		}
		const bool bGroup = bShift ? gDLL->getInterfaceIFace()->mirrorsSelectionGroup() : true;

		const CvPlot* pUnitPlot = pUnit->plot();

		CLLNode<IDInfo>* pUnitNode = pUnitPlot->headUnitNode();

		gDLL->getInterfaceIFace()->selectionListPreChange();

		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = pUnitPlot->nextUnitNode(pUnitNode);

			if (pLoopUnit->getOwner() == getActivePlayer() && pLoopUnit->canMove()
			&& (!isMPOption(MPOPTION_SIMULTANEOUS_TURNS) || getTurnSlice() - pLoopUnit->getLastMoveTurn() > GC.getMIN_TIMER_UNIT_DOUBLE_MOVES())
			&& (bAlt || pLoopUnit->getUnitType() == pUnit->getUnitType()))
			{
				gDLL->getInterfaceIFace()->insertIntoSelectionList(pLoopUnit, false, false, bGroup, false, true);
			}
		}
		gDLL->getInterfaceIFace()->selectionListPostChange();
	}
	else
	{
		gDLL->getInterfaceIFace()->selectUnit(pUnit, !bShift, bShift, true);
	}
}


void CvGame::selectAll(CvPlot* pPlot) const
{
	if (pPlot != NULL)
	{
		CvUnit* pCenterUnit = pPlot->getDebugCenterUnit();

		if (pCenterUnit != NULL && pCenterUnit->getOwner() == getActivePlayer())
		{
			gDLL->getInterfaceIFace()->selectGroup(pCenterUnit, false, false, true);
		}
	}
}


bool CvGame::selectionListIgnoreBuildingDefense() const
{
	PROFILE_FUNC();

	bool bIgnoreBuilding = false;
	bool bAttackLandUnit = false;

	CLLNode<IDInfo>* pSelectedUnitNode = gDLL->getInterfaceIFace()->headSelectionListNode();

	while (pSelectedUnitNode != NULL && (!bIgnoreBuilding || !bAttackLandUnit))
	{
		CvUnit* pSelectedUnit = ::getUnit(pSelectedUnitNode->m_data);
		pSelectedUnitNode = gDLL->getInterfaceIFace()->nextSelectionListNode(pSelectedUnitNode);

		if (pSelectedUnit != NULL)
		{
			if (pSelectedUnit->ignoreBuildingDefense())
			{
				bIgnoreBuilding = true;
			}

			if (pSelectedUnit->getDomainType() == DOMAIN_LAND && pSelectedUnit->canAttack())
			{
				bAttackLandUnit = true;
			}
		}
	}

	if (!bIgnoreBuilding && !bAttackLandUnit && getBestLandUnit() != NO_UNIT)
	{
		bIgnoreBuilding = GC.getUnitInfo(getBestLandUnit()).isIgnoreBuildingDefense();
	}
	return bIgnoreBuilding;
}


void CvGame::implementDeal(PlayerTypes eWho, PlayerTypes eOtherWho, CLinkList<TradeData>* pOurList, CLinkList<TradeData>* pTheirList, bool bForce)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eWho);
	FASSERT_BOUNDS(0, MAX_PLAYERS, eOtherWho);
	FAssertMsg(eWho != eOtherWho, "eWho is not expected to be equal with eOtherWho");

	CvDeal* pDeal = addDeal();
	pDeal->init(pDeal->getID(), eWho, eOtherWho);
	pDeal->addTrades(pOurList, pTheirList, !bForce);
	if ((pDeal->getLengthFirstTrades() == 0) && (pDeal->getLengthSecondTrades() == 0))
	{
		pDeal->kill();
	}
}


void CvGame::verifyDeals()
{
	int iLoop;
	for (CvDeal* pLoopDeal = firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = nextDeal(&iLoop))
	{
		pLoopDeal->verify();
	}
}


/* Globeview configuration control:
If bStarsVisible, then there will be stars visible behind the globe when it is on
If bWorldIsRound, then the world will bend into a globe; otherwise, it will show up as a plane  */
void CvGame::getGlobeviewConfigurationParameters(TeamTypes eTeam, bool& bStarsVisible, bool& bWorldIsRound)
{
	if (GET_TEAM(eTeam).isMapCentering() || getCircumnavigatedTeam() != NO_TEAM)
	{
		bStarsVisible = true;
		bWorldIsRound = true;
	}
	else
	{
		bStarsVisible = false;
		bWorldIsRound = false;
	}
}


int CvGame::getSymbolID(int iSymbol)
{
	return gDLL->getInterfaceIFace()->getSymbolID(iSymbol);
}


int CvGame::getAdjustedPopulationPercent(VictoryTypes eVictory) const
{
	if (GC.getVictoryInfo(eVictory).getPopulationPercentLead() == 0)
	{
		return 0;
	}

	if (getTotalPopulation() == 0)
	{
		return 100;
	}

	int iBestPopulation = 0;
	int iNextBestPopulation = 0;

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			const int iPopulation = GET_TEAM((TeamTypes)iI).getTotalPopulation();

			if (iPopulation > iBestPopulation)
			{
				iNextBestPopulation = iBestPopulation;
				iBestPopulation = iPopulation;
			}
			else if (iPopulation > iNextBestPopulation)
			{
				iNextBestPopulation = iPopulation;
			}
		}
	}

	return std::min(100, (((iNextBestPopulation * 100) / getTotalPopulation()) + GC.getVictoryInfo(eVictory).getPopulationPercentLead()));
}


int CvGame::getProductionPerPopulation(HurryTypes eHurry) const
{
	return eHurry != NO_HURRY ? GC.getHurryInfo(eHurry).getProductionPerPopulation() : 0;
}


int CvGame::getAdjustedLandPercent(VictoryTypes eVictory) const
{
	if (GC.getVictoryInfo(eVictory).getLandPercent() == 0)
	{
		return 0;
	}

	int iPercent = GC.getVictoryInfo(eVictory).getLandPercent();

	iPercent -= (countCivTeamsEverAlive() * 2);

	return std::max(iPercent, GC.getVictoryInfo(eVictory).getMinLandPercent());
}


bool CvGame::isTeamVote(VoteTypes eVote) const
{
	return (GC.getVoteInfo(eVote).isSecretaryGeneral() || GC.getVoteInfo(eVote).isVictory());
}


bool CvGame::isChooseElection(VoteTypes eVote) const
{
	return !(GC.getVoteInfo(eVote).isSecretaryGeneral());
}


bool CvGame::isTeamVoteEligible(TeamTypes eTeam, VoteSourceTypes eVoteSource) const
{
	const CvTeam& kTeam = GET_TEAM(eTeam);

	if (kTeam.isForceTeamVoteEligible(eVoteSource))
	{
		return true;
	}

	if (!kTeam.isFullMember(eVoteSource))
	{
		return false;
	}

	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		const CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
		if (kLoopTeam.isAlive() && kLoopTeam.isForceTeamVoteEligible(eVoteSource))
		{
			++iCount;
		}
	}

	int iExtraEligible = GC.getTEAM_VOTE_MIN_CANDIDATES() - iCount;
	if (iExtraEligible <= 0)
	{
		return false;
	}

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (iI != eTeam)
		{
			const CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
			if (kLoopTeam.isAlive() && !kLoopTeam.isForceTeamVoteEligible(eVoteSource) && kLoopTeam.isFullMember(eVoteSource))
			{
				const int iLoopVotes = kLoopTeam.getVotes(NO_VOTE, eVoteSource);
				const int iVotes = kTeam.getVotes(NO_VOTE, eVoteSource);
				if (iLoopVotes > iVotes || (iLoopVotes == iVotes && iI < eTeam))
				{
					iExtraEligible--;
				}
			}
		}
	}

	return iExtraEligible > 0;
}


int CvGame::countVote(const VoteTriggeredData& kData, PlayerVoteTypes eChoice) const
{
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && getPlayerVote(((PlayerTypes)iI), kData.getID()) == eChoice)
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getVotes(kData.kVoteOption.eVote, kData.eVoteSource);
		}
	}
	return iCount;
}


int CvGame::countPossibleVote(VoteTypes eVote, VoteSourceTypes eVoteSource) const
{
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if ( GET_PLAYER((PlayerTypes)iI).isAlive() )
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getVotes(eVote, eVoteSource);
		}
	}
	return iCount;
}



TeamTypes CvGame::findHighestVoteTeam(const VoteTriggeredData& kData) const
{
	TeamTypes eBestTeam = NO_TEAM;
	int iBestCount = 0;

	if (isTeamVote(kData.kVoteOption.eVote))
	{
		for (int iI = 0; iI < MAX_PC_TEAMS; ++iI)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				const int iCount = countVote(kData, (PlayerVoteTypes)iI);
				if (iCount > iBestCount)
				{
					iBestCount = iCount;
					eBestTeam = (TeamTypes)iI;
				}
			}
		}
	}
	return eBestTeam;
}


int CvGame::getVoteRequired(VoteTypes eVote, VoteSourceTypes eVoteSource) const
{
	return countPossibleVote(eVote, eVoteSource) * GC.getVoteInfo(eVote).getPopulationThreshold() / 100;
}


TeamTypes CvGame::getSecretaryGeneral(VoteSourceTypes eVoteSource) const
{
	if (!canHaveSecretaryGeneral(eVoteSource))
	{
		for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); ++iBuilding)
		{
			if (GC.getBuildingInfo((BuildingTypes)iBuilding).getVoteSourceType() == eVoteSource)
			{
				for (int iI = 0; iI < MAX_PC_PLAYERS; ++iI)
				{
					const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
					if (kLoopPlayer.isAlive() && kLoopPlayer.getBuildingCount((BuildingTypes)iBuilding) > 0)
					{
						const ReligionTypes eReligion = getVoteSourceReligion(eVoteSource);
						if (NO_RELIGION == eReligion || kLoopPlayer.getStateReligion() == eReligion)
						{
							return kLoopPlayer.getTeam();
						}
					}
				}
			}
		}
	}
	else
	{
		for (int iI = 0; iI < GC.getNumVoteInfos(); iI++)
		{
			if (GC.getVoteInfo((VoteTypes)iI).isVoteSourceType(eVoteSource)
			&&  GC.getVoteInfo((VoteTypes)iI).isSecretaryGeneral()
			&& isVotePassed((VoteTypes)iI))
			{
				return (TeamTypes)getVoteOutcome((VoteTypes)iI);
			}
		}
	}
	return NO_TEAM;
}

bool CvGame::canHaveSecretaryGeneral(VoteSourceTypes eVoteSource) const
{
	for (int iI = 0; iI < GC.getNumVoteInfos(); iI++)
	{
		if (GC.getVoteInfo((VoteTypes)iI).isVoteSourceType(eVoteSource) && GC.getVoteInfo((VoteTypes)iI).isSecretaryGeneral())
		{
			return true;
		}
	}
	return false;
}

void CvGame::clearSecretaryGeneral(VoteSourceTypes eVoteSource)
{
	for (int j = 0; j < GC.getNumVoteInfos(); ++j)
	{
		const CvVoteInfo& kVote = GC.getVoteInfo((VoteTypes)j);

		if (kVote.isVoteSourceType(eVoteSource) && kVote.isSecretaryGeneral())
		{
			VoteTriggeredData kData;
			kData.eVoteSource = eVoteSource;
			kData.kVoteOption.eVote = (VoteTypes)j;
			kData.kVoteOption.iCityId = -1;
			kData.kVoteOption.szText.clear();
			kData.kVoteOption.ePlayer = NO_PLAYER;
			setVoteOutcome(kData, NO_PLAYER_VOTE);
			setSecretaryGeneralTimer(eVoteSource, 0);
		}
	}
}

void CvGame::updateSecretaryGeneral()
{
	PROFILE_FUNC();

	for (int i = 0; i < GC.getNumVoteSourceInfos(); ++i)
	{
		const TeamTypes eSecretaryGeneral = getSecretaryGeneral((VoteSourceTypes)i);
		if (NO_TEAM != eSecretaryGeneral && !GET_TEAM(eSecretaryGeneral).isFullMember((VoteSourceTypes)i))
		{
			clearSecretaryGeneral((VoteSourceTypes)i);
		}
	}
}

int CvGame::countCivPlayersAlive() const
{
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive()) iCount++;
	}
	return iCount;
}


int CvGame::countCivPlayersEverAlive() const
{
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)//Might be best to USE CIV rather than PC but it's hard to guess.  The exe could be accessing for a loop setup and that could make this the ultimate variable that CIV is supposed to represent.  But what's inside the dll referencing this does not jive with that and better fits a PC count.
	{
		const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kPlayer.isEverAlive() && kPlayer.getParent() == NO_PLAYER)
		{
			iCount++;
		}
	}
	return iCount;
}


int CvGame::countCivTeamsAlive() const
{
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive()) iCount++;
	}
	return iCount;
}


int CvGame::countCivTeamsEverAlive() const
{
	bool teamsAlive[MAX_PC_TEAMS];
	memset(teamsAlive, 0, sizeof(bool)*MAX_PC_TEAMS);

	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iI);
		if (kPlayer.isEverAlive() && kPlayer.getParent() == NO_PLAYER)
		{
			FASSERT_BOUNDS(0, MAX_PC_TEAMS, kPlayer.getTeam());

			if (!teamsAlive[kPlayer.getTeam()])
			{
				teamsAlive[kPlayer.getTeam()] = true;
				iCount++;
			}
		}
	}
	return iCount;
}


int CvGame::countHumanPlayersAlive() const
{
	int iCount = 0;

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHuman())
		{
			iCount++;
		}
	}
	return iCount;
}


int CvGame::countTotalCivPower() const
{
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getPower();
		}
	}
	return iCount;
}


int CvGame::countTotalNukeUnits() const
{
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getNumNukeUnits();
		}
	}
	return iCount;
}


int CvGame::countKnownTechNumTeams(TechTypes eTech) const
{
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isEverAlive() && GET_TEAM((TeamTypes)iI).isHasTech(eTech))
		{
			iCount++;
		}
	}
	return iCount;
}


int CvGame::getNumFreeBonuses(BuildingTypes eBuilding) const
{
	if (GC.getBuildingInfo(eBuilding).getNumFreeBonuses() == -1)
	{
		return GC.getWorldInfo(GC.getMap().getWorldSize()).getNumFreeBuildingBonuses();
	}
	return GC.getBuildingInfo(eBuilding).getNumFreeBonuses();
}


int CvGame::countReligionLevels(ReligionTypes eReligion) const
{
	int iCount = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iCount += GET_PLAYER((PlayerTypes)iI).getHasReligionCount(eReligion);
		}
	}
	return iCount;
}

int CvGame::countCorporationLevels(CorporationTypes eCorporation) const
{
	int iCount = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.isAlive())
		{
			iCount += kLoopPlayer.getHasCorporationCount(eCorporation);
		}
	}
	return iCount;
}

void CvGame::replaceCorporation(CorporationTypes eCorporation1, CorporationTypes eCorporation2)
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const CvPlayer& player = GET_PLAYER((PlayerTypes)iI);
		if (player.isAlive())
		{
			foreach_(CvCity* pLoopCity, player.cities())
			{
				if (pLoopCity->isHasCorporation(eCorporation1))
				{
					pLoopCity->setHasCorporation(eCorporation1, false, false, false);
					pLoopCity->setHasCorporation(eCorporation2, true, true);
				}
			}
			foreach_(CvUnit* pLoopUnit, player.units())
			{
				if (pLoopUnit->getUnitInfo().getCorporationSpreads(eCorporation1) > 0)
				{
					pLoopUnit->kill(false);
				}
			}
		}
	}
}


int CvGame::calculateReligionPercent(ReligionTypes eReligion) const
{
	if (getTotalPopulation() == 0)
	{
		return 0;
	}
	int iCount = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const CvPlayer& player = GET_PLAYER((PlayerTypes)iI);
		if (player.isAlive())
		{
			foreach_(const CvCity* pLoopCity, player.cities())
			{
				if (pLoopCity->isHasReligion(eReligion))
				{
					iCount += ((pLoopCity->getPopulation() + (pLoopCity->getReligionCount() / 2)) / pLoopCity->getReligionCount());
				}
			}
		}
	}
	return iCount * 100 / getTotalPopulation();
}


int CvGame::goldenAgeLength() const
{
	return GC.getGOLDEN_AGE_LENGTH() * GC.getGameSpeedInfo(getGameSpeedType()).getSpeedPercent() / 100;
}

int CvGame::victoryDelay(VictoryTypes eVictory) const
{
	FASSERT_BOUNDS(0, GC.getNumVictoryInfos(), eVictory);

	return GC.getVictoryInfo(eVictory).getVictoryDelayTurns() * GC.getGameSpeedInfo(getGameSpeedType()).getSpeedPercent() / 100;
}



int CvGame::getImprovementUpgradeTime(ImprovementTypes eImprovement) const
{
	int iTime = GC.getImprovementInfo(eImprovement).getUpgradeTime();
	if (iTime < 1) return 0;

	iTime *= GC.getGameSpeedInfo(getGameSpeedType()).getHammerCostPercent();
	iTime /= 100;

	iTime *= GC.getEraInfo(getCurrentEra()).getImprovementPercent();
	iTime /= 100;

	return std::max(1, iTime);
}


bool CvGame::canTrainNukes() const
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const CvPlayer& player = GET_PLAYER((PlayerTypes)iI);
		if (player.isAlive())
		{
			for (int iJ = 0; iJ < GC.getNumUnitInfos(); iJ++)
			{
				if (GC.getUnitInfo((UnitTypes)iJ).getNukeRange() != -1 && player.canTrain((UnitTypes)iJ))
				{
					return true;
				}
			}
		}
	}
	return false;
}


EraTypes CvGame::getHighestEra() const
{
	int iHighestEra = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			const int iLoopEra = GET_PLAYER((PlayerTypes)iI).getCurrentEra();
			if (iLoopEra > iHighestEra)
			{
				iHighestEra = iLoopEra;
			}
		}
	}
	return EraTypes(iHighestEra);
}

EraTypes CvGame::getCurrentEra() const
{
	int iEra = 0;
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iEra += GET_PLAYER((PlayerTypes)iI).getCurrentEra();
			iCount++;
		}
	}
	return (iCount > 0 ? (EraTypes)(iEra/iCount) : (EraTypes)0);
}


TeamTypes CvGame::getActiveTeam() const
{
	return (getActivePlayer() != NO_PLAYER) ? (TeamTypes)GET_PLAYER(getActivePlayer()).getTeam() : NO_TEAM;
}


CivilizationTypes CvGame::getActiveCivilizationType() const
{
	return (getActivePlayer() != NO_PLAYER) ? (CivilizationTypes)GET_PLAYER(getActivePlayer()).getCivilizationType() : NO_CIVILIZATION;
}


bool CvGame::isNetworkMultiPlayer() const
{
	return GC.getInitCore().getMultiplayer();
}


bool CvGame::isGameMultiPlayer() const
{
	return isNetworkMultiPlayer() || isPbem() || isHotSeat();
}


bool CvGame::isTeamGame() const
{
	FAssert(countCivPlayersAlive() >= countCivTeamsAlive());
	return (countCivPlayersAlive() > countCivTeamsAlive());
}


bool CvGame::isModem() const
{
	return gDLL->IsModem();
}
void CvGame::setModem(bool bModem)
{
	if (bModem)
		gDLL->ChangeINIKeyValue("CONFIG", "Bandwidth", "modem");
	else gDLL->ChangeINIKeyValue("CONFIG", "Bandwidth", "broadband");

	gDLL->SetModem(bModem);
}


void CvGame::reviveActivePlayer()
{
	if (!GET_PLAYER(getActivePlayer()).isAlive())
	{
		if (isForcedAIAutoPlay(getActivePlayer()))
		{
			setForcedAIAutoPlay(getActivePlayer(), 0);
		}
		else setAIAutoPlay(getActivePlayer(), 0);

		GC.getInitCore().setSlotStatus(getActivePlayer(), SS_TAKEN);

		GET_PLAYER(getActivePlayer()).initUnit((UnitTypes)0, 0, 0, NO_UNITAI, NO_DIRECTION, 0);
	}
}


int CvGame::getNumHumanPlayers()
{
	return GC.getInitCore().getNumHumans();
}

int CvGame::getGameTurn()
{
	return GC.getInitCore().getGameTurn();
}


void CvGame::setGameTurn(int iNewValue)
{
	if (getGameTurn() != iNewValue)
	{
		GC.getInitCore().setGameTurn(iNewValue);
		FASSERT_NOT_NEGATIVE(getGameTurn());

		if (m_iDateTurn == iNewValue - 1)
		{
			m_currentDate.increment();
		}
		else
		{
			m_currentDate = CvDate::getDate(iNewValue);
		}
		m_iDateTurn = iNewValue;

		updateBuildingCommerce();

		setScoreDirty(true);

		gDLL->getInterfaceIFace()->setDirty(TurnTimer_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
	}
}


void CvGame::incrementGameTurn()
{
	setGameTurn(getGameTurn() + 1);
}


int CvGame::getTurnYear(int iGameTurn)
{
	// moved the body of this method to Game Core Utils so we have access for other games than the current one (replay screen in HOF)
	return getTurnYearForGame(iGameTurn, getStartYear(), getCalendar(), getGameSpeedType());
}


int CvGame::getGameTurnYear()
{
	return getTurnYear(getGameTurn());
}

CvDate CvGame::getCurrentDate()
{
	if (m_iDateTurn != getGameTurn())
	{
		m_currentDate = CvDate::getDate(getGameTurn());
		m_iDateTurn = getGameTurn();
	}
	return m_currentDate;
}


int CvGame::getElapsedGameTurns() const
{
	return m_iElapsedGameTurns;
}


void CvGame::incrementElapsedGameTurns()
{
	m_iElapsedGameTurns++;
}


int CvGame::getMaxTurns() const
{
	return GC.getInitCore().getMaxTurns();
}


void CvGame::setMaxTurns(int iNewValue)
{
	GC.getInitCore().setMaxTurns(iNewValue);
	FASSERT_NOT_NEGATIVE(getMaxTurns());
}


void CvGame::changeMaxTurns(int iChange)
{
	setMaxTurns(getMaxTurns() + iChange);
}


int CvGame::getMaxCityElimination() const
{
	return GC.getInitCore().getMaxCityElimination();
}


void CvGame::setMaxCityElimination(int iNewValue)
{
	GC.getInitCore().setMaxCityElimination(iNewValue);
	FASSERT_NOT_NEGATIVE(getMaxCityElimination());
}

int CvGame::getNumAdvancedStartPoints() const
{
	return GC.getInitCore().getNumAdvancedStartPoints();
}


void CvGame::setNumAdvancedStartPoints(int iNewValue)
{
	GC.getInitCore().setNumAdvancedStartPoints(iNewValue);
	FASSERT_NOT_NEGATIVE(getNumAdvancedStartPoints());
}

int CvGame::getStartTurn() const
{
	return m_iStartTurn;
}


void CvGame::setStartTurn(int iNewValue)
{
	m_iStartTurn = iNewValue;
}


int CvGame::getStartYear() const
{
	return m_iStartYear;
}


void CvGame::setStartYear(int iNewValue)
{
	m_iStartYear = iNewValue;
}


int CvGame::getEstimateEndTurn() const
{
	return m_iEstimateEndTurn;
}


void CvGame::setEstimateEndTurn(int iNewValue)
{
	m_iEstimateEndTurn = iNewValue;
}


void CvGame::changeTurnSlice(int iChange)
{
	m_iTurnSlice += iChange;
}

int CvGame::getTurnSlice() const
{
	return m_iTurnSlice;
}


int CvGame::getMinutesPlayed() const
{
	return getTurnSlice() / gDLL->getTurnsPerMinute();
}



int CvGame::getCutoffSlice() const
{
	return m_iCutoffSlice;
}


void CvGame::setCutoffSlice(int iNewValue)
{
	m_iCutoffSlice = iNewValue;
}


void CvGame::changeCutoffSlice(int iChange)
{
	setCutoffSlice(getCutoffSlice() + iChange);
}


int CvGame::getTurnSlicesRemaining()
{
	return getCutoffSlice() - getTurnSlice();
}


void CvGame::resetTurnTimer()
{
	// We should only use the turn timer if we are in multiplayer
	if (isMPOption(MPOPTION_TURN_TIMER)
	&& (getElapsedGameTurns() > 0 || !isOption(GAMEOPTION_ADVANCED_START)))
	{
		// Determine how much time we should allow
		if (getElapsedGameTurns() == 0 && !isPitboss())
		{
			// Let's allow more time for the initial turn
			FASSERT_BOUNDS(0, GC.getNumTurnTimerInfos(), GC.getInitCore().getTurnTimer());
			setCutoffSlice(getTurnSlice() + getMaxTurnLen() * GC.getTurnTimerInfo(GC.getInitCore().getTurnTimer()).getFirstTurnMultiplier());
		}
		// Set the current turn slice to start the 'timer'
		else setCutoffSlice(getTurnSlice() + getMaxTurnLen());
	}
}

void CvGame::incrementTurnTimer(int iNumTurnSlices)
{
	// If the turn timer has expired, we shouldn't increment it as we've sent our turn complete message
	if (isMPOption(MPOPTION_TURN_TIMER) && getTurnSlice() <= getCutoffSlice())
	{
		changeCutoffSlice(iNumTurnSlices);
	}
}


int CvGame::getMaxTurnLen() const
{
	if (isPitboss())
	{
		// Use the user provided input, turn time is in hours
		return getPitbossTurnTime() * 3600 * 4;
	}
	// Find out who has the most units and who has the most cities
	// Calculate the max turn time based on the max number of units and cities
	int iMaxCities = 0;
	int iMaxUnits = 0;
	for (int i = 0; i < MAX_PC_PLAYERS; ++i)
	{
		if (GET_PLAYER((PlayerTypes)i).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)i).getNumCities() > iMaxCities)
			{
				iMaxCities = GET_PLAYER((PlayerTypes)i).getNumCities();
			}
			if (GET_PLAYER((PlayerTypes)i).getNumUnits() > iMaxUnits)
			{
				iMaxUnits = GET_PLAYER((PlayerTypes)i).getNumUnits();
			}
		}
	}
	// Now return turn len based on base len and unit and city bonuses
	const TurnTimerTypes eTurnTimer = GC.getInitCore().getTurnTimer();
	FASSERT_BOUNDS(0, GC.getNumTurnTimerInfos(), eTurnTimer);
	return
	(
		GC.getTurnTimerInfo(eTurnTimer).getBaseTime() +
		GC.getTurnTimerInfo(eTurnTimer).getCityBonus() * iMaxCities +
		GC.getTurnTimerInfo(eTurnTimer).getUnitBonus() * iMaxUnits
	);
}


int CvGame::getTargetScore() const
{
	return GC.getInitCore().getTargetScore();
}


void CvGame::setTargetScore(int iNewValue)
{
	GC.getInitCore().setTargetScore(iNewValue);
	FASSERT_NOT_NEGATIVE(getTargetScore());
}


int CvGame::getNumGameTurnActive() const
{
	return m_iNumGameTurnActive;
}


int CvGame::countNumHumanGameTurnActive() const
{
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isHuman() && GET_PLAYER((PlayerTypes)iI).isTurnActive())
		{
			iCount++;
		}
	}
	return iCount;
}


void CvGame::changeNumGameTurnActive(int iChange)
{
	m_iNumGameTurnActive += iChange;
	FASSERT_NOT_NEGATIVE(m_iNumGameTurnActive);
}


int CvGame::getNumCities() const
{
	return m_iNumCities;
}
void CvGame::changeNumCities(int iChange)
{
	m_iNumCities += iChange;
	FASSERT_NOT_NEGATIVE(m_iNumCities);
}


int CvGame::getNumCivCities() const
{
	return
	(
		getNumCities()
		- GET_PLAYER(BARBARIAN_PLAYER).getNumCities()
		- GET_PLAYER(NEANDERTHAL_PLAYER).getNumCities()
	);
}


int CvGame::getStatusPromotion(int i) const
{
	return m_aiStatusPromotions[i];
}

int CvGame::getNumStatusPromotions() const
{
	return (int)m_aiStatusPromotions.size();
}

void CvGame::setStatusPromotions()
{
	m_aiStatusPromotions.clear();
	for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		if (GC.getPromotionInfo((PromotionTypes)iI).isStatus())
		{
			m_aiStatusPromotions.push_back(iI);
		}
	}
}


int CvGame::getTotalPopulation() const
{
	return m_iTotalPopulation;
}

void CvGame::changeTotalPopulation(int iChange)
{
	m_iTotalPopulation += iChange;
	FASSERT_NOT_NEGATIVE(m_iTotalPopulation);
}


int CvGame::getTradeRoutes() const
{
	return m_iTradeRoutes;
}


void CvGame::changeTradeRoutes(int iChange)
{
	if (iChange != 0)
	{
		m_iTradeRoutes += iChange;
		FASSERT_NOT_NEGATIVE(getTradeRoutes());

		updateTradeRoutes();
	}
}


int CvGame::getFreeTradeCount() const
{
	return m_iFreeTradeCount;
}

bool CvGame::isFreeTrade() const
{
	return m_iFreeTradeCount > 0;
}

void CvGame::changeFreeTradeCount(int iChange)
{
	if (iChange != 0)
	{
		const bool bOldFreeTrade = isFreeTrade();

		m_iFreeTradeCount += iChange;
		FASSERT_NOT_NEGATIVE(m_iFreeTradeCount);

		if (bOldFreeTrade != isFreeTrade())
		{
			updateTradeRoutes();
		}
	}
}


int CvGame::getNoNukesCount() const
{
	return m_iNoNukesCount;
}

bool CvGame::isNoNukes() const
{
	return isOption(GAMEOPTION_NO_NUKES) || getNoNukesCount() > 0;
}

void CvGame::changeNoNukesCount(int iChange)
{
	m_iNoNukesCount += iChange;
	FASSERT_NOT_NEGATIVE(m_iNoNukesCount);
}


int CvGame::getSecretaryGeneralTimer(VoteSourceTypes eVoteSource) const
{
	FASSERT_BOUNDS(0, GC.getNumVoteSourceInfos(), eVoteSource);
	return m_aiSecretaryGeneralTimer[eVoteSource];
}


void CvGame::setSecretaryGeneralTimer(VoteSourceTypes eVoteSource, int iNewValue)
{
	FASSERT_BOUNDS(0, GC.getNumVoteSourceInfos(), eVoteSource);
	m_aiSecretaryGeneralTimer[eVoteSource] = iNewValue;
	FASSERT_NOT_NEGATIVE(getSecretaryGeneralTimer(eVoteSource));
}


void CvGame::changeSecretaryGeneralTimer(VoteSourceTypes eVoteSource, int iChange)
{
	setSecretaryGeneralTimer(eVoteSource, getSecretaryGeneralTimer(eVoteSource) + iChange);
}


int CvGame::getVoteTimer(VoteSourceTypes eVoteSource) const
{
	FASSERT_BOUNDS(0, GC.getNumVoteSourceInfos(), eVoteSource);
	return m_aiVoteTimer[eVoteSource];
}


void CvGame::setVoteTimer(VoteSourceTypes eVoteSource, int iNewValue)
{
	FASSERT_BOUNDS(0, GC.getNumVoteSourceInfos(), eVoteSource);
	m_aiVoteTimer[eVoteSource] = iNewValue;
	FASSERT_NOT_NEGATIVE(getVoteTimer(eVoteSource));
}


void CvGame::changeVoteTimer(VoteSourceTypes eVoteSource, int iChange)
{
	setVoteTimer(eVoteSource, getVoteTimer(eVoteSource) + iChange);
}


int CvGame::getNukesExploded() const
{
	return m_iNukesExploded;
}


void CvGame::changeNukesExploded(int iChange)
{
	m_iNukesExploded += iChange;
}


int CvGame::getMaxPopulation() const
{
	return m_iMaxPopulation;
}


int CvGame::getMaxLand() const
{
	return m_iMaxLand;
}


int CvGame::getMaxTech() const
{
	return m_iMaxTech;
}


int CvGame::getMaxWonders() const
{
	return m_iMaxWonders;
}


int CvGame::getInitPopulation() const
{
	return m_iInitPopulation;
}


int CvGame::getInitLand() const
{
	return m_iInitLand;
}


int CvGame::getInitTech() const
{
	return m_iInitTech;
}


int CvGame::getInitWonders() const
{
	return m_iInitWonders;
}


void CvGame::initScoreCalculation()
{
	// initialize score calculation
	int iMaxFood = 0;
	for (int i = 0; i < GC.getMap().numPlots(); i++)
	{
		const CvPlot* pPlot = GC.getMap().plotByIndex(i);
		if (!pPlot->isWater() || pPlot->isAdjacentToLand())
		{
			iMaxFood += pPlot->calculateBestNatureYield(YIELD_FOOD, NO_TEAM);
		}
	}
	m_iMaxPopulation = iMaxFood / std::max(1, GC.getFOOD_CONSUMPTION_PER_POPULATION());
	m_iMaxLand = GC.getMap().getLandPlots();
	m_iMaxTech = 0;
	for (int i = 0; i < GC.getNumTechInfos(); i++)
	{
		m_iMaxTech += getTechScore((TechTypes)i);
	}
	m_iMaxWonders = 0;
	for (int i = 0; i < GC.getNumBuildingInfos(); i++)
	{
		m_iMaxWonders += getWonderScore((BuildingTypes)i);
	}

	if (NO_ERA != getStartEra())
	{
		const int iNumSettlers = GC.getEraInfo(getStartEra()).getStartingUnitMultiplier();
		m_iInitPopulation = iNumSettlers * (GC.getEraInfo(getStartEra()).getFreePopulation() + 1);
		m_iInitLand = iNumSettlers *  NUM_CITY_PLOTS;
	}
	else
	{
		m_iInitPopulation = 0;
		m_iInitLand = 0;
	}

	m_iInitTech = 0;
	for (int i = 0; i < GC.getNumTechInfos(); i++)
	{
		if (GC.getTechInfo((TechTypes)i).getEra() >= getStartEra())
		{
			// count all possible free techs as initial to lower the score from immediate retirement
			for (int iCiv = 0; iCiv < GC.getNumCivilizationInfos(); iCiv++)
			{
				if (GC.getCivilizationInfo((CivilizationTypes)iCiv).isPlayable()
				&&  GC.getCivilizationInfo((CivilizationTypes)iCiv).isCivilizationFreeTechs(i))
				{
					m_iInitTech += getTechScore((TechTypes)i);
					break;
				}
			}
		}
		else m_iInitTech += getTechScore((TechTypes)i);
	}
	m_iInitWonders = 0;
}


int CvGame::getAIAutoPlay(PlayerTypes iPlayer) const
{
	return m_iAIAutoPlay[iPlayer];
}

void CvGame::setAIAutoPlay(PlayerTypes iPlayer, int iNewValue, bool bForced)
{
	FASSERT_NOT_NEGATIVE(iNewValue);

	if (isForcedAIAutoPlay(iPlayer) && !bForced)
	{
		return;
	}

	if (GC.getLogging())
	{
		char szOut[1024];
		sprintf(szOut, "setAutoPlay called for player %d - set to: %d\n", iPlayer, iNewValue);
		gDLL->messageControlLog(szOut);
	}

	if (m_iAIAutoPlay[iPlayer] != iNewValue)
	{
		m_iAIAutoPlay[iPlayer] = std::max(0, iNewValue);

		if (iNewValue > 0)
		{
			if (!GET_PLAYER(iPlayer).isHumanDisabled())
			{
				GET_PLAYER(iPlayer).setHumanDisabled(true);
			}
		}
		else GET_PLAYER(iPlayer).setHumanDisabled(false);
	}
}

void CvGame::setAIAutoPlayExternal(int iNewValue)
{
	return setAIAutoPlay(getActivePlayer(), iNewValue);
}

void CvGame::changeAIAutoPlay(PlayerTypes iPlayer, int iChange)
{
	setAIAutoPlay(iPlayer, getAIAutoPlay(iPlayer) + iChange);
}


bool CvGame::isForcedAIAutoPlay(PlayerTypes iPlayer) const
{
	FASSERT_NOT_NEGATIVE(getForcedAIAutoPlay(iPlayer));
	return getForcedAIAutoPlay(iPlayer) > 0;
}

int CvGame::getForcedAIAutoPlay(PlayerTypes iPlayer) const
{
	return m_iForcedAIAutoPlay[iPlayer];
}

void CvGame::setForcedAIAutoPlay(PlayerTypes iPlayer, int iNewValue, bool bForced)
{
	FASSERT_NOT_NEGATIVE(iNewValue);

	if (!bForced)
	{
		m_iForcedAIAutoPlay[iPlayer] = 0;

		if (m_iAIAutoPlay[iPlayer] != iNewValue)
		{
			setAIAutoPlay(iPlayer, iNewValue);
		}
	}
	else if (getForcedAIAutoPlay(iPlayer) != iNewValue)
	{
		m_iForcedAIAutoPlay[iPlayer] = std::max(0, iNewValue);
		setAIAutoPlay(iPlayer, iNewValue, true);
	}
	else setAIAutoPlay(iPlayer, iNewValue, true);
}

void CvGame::changeForcedAIAutoPlay(PlayerTypes iPlayer, int iChange)
{
	if (isForcedAIAutoPlay(iPlayer))
	{
		setForcedAIAutoPlay(iPlayer, (getAIAutoPlay(iPlayer) + iChange), true);
	}
	else setForcedAIAutoPlay(iPlayer, (getAIAutoPlay(iPlayer) + iChange));
}


// Toffer - Unused, but I spared it in my M.A.D. purge as it looks somewhat useful
CvPlot* CvGame::getLastNukeStrikePlot() const
{
	return GC.getMap().plotSorenINLINE(m_iLastNukeStrikeX, m_iLastNukeStrikeY);
}

void CvGame::setLastNukeStrikePlot(CvPlot* pPlot)
{
	if (getLastNukeStrikePlot() != pPlot)
	{
		if (pPlot != NULL)
		{
			m_iLastNukeStrikeX = pPlot->getX();
			m_iLastNukeStrikeY = pPlot->getY();
		}
		else
		{
			m_iLastNukeStrikeX = INVALID_PLOT_COORD;
			m_iLastNukeStrikeY = INVALID_PLOT_COORD;
		}
	}
}
// ! Toffer


unsigned int CvGame::getInitialTime() const
{
	return m_uiInitialTime;
}


void CvGame::setInitialTime(unsigned int uiNewValue)
{
	m_uiInitialTime = uiNewValue;
}


bool CvGame::isScoreDirty() const
{
	return m_bScoreDirty;
}


void CvGame::setScoreDirty(bool bNewValue)
{
	m_bScoreDirty = bNewValue;
}


TeamTypes CvGame::getCircumnavigatedTeam() const
{
	return m_circumnavigatingTeam;
}

void CvGame::setCircumnavigatedTeam(TeamTypes eTeam)
{
	if (eTeam == m_circumnavigatingTeam)
	{
		return;
	}
	if (m_circumnavigatingTeam != NO_TEAM)
	{
		GET_TEAM(m_circumnavigatingTeam).setCircumnavigated(false);
	}
	m_circumnavigatingTeam = eTeam;

	if (m_circumnavigatingTeam != NO_TEAM)
	{
		GET_TEAM(eTeam).setCircumnavigated(true);
	}
}

bool CvGame::circumnavigationAvailable() const
{
	if (getCircumnavigatedTeam() != NO_TEAM || getElapsedGameTurns() < 1)
	{
		return false;
	}

	const CvMap& kMap = GC.getMap();

	if (!kMap.isWrapX() && !kMap.isWrapY())
	{
		return false;
	}
	if (kMap.getLandPlots() > kMap.numPlots() * 2 / 3)
	{
		return false;
	}
	return true;
}


bool CvGame::isDiploVote(VoteSourceTypes eVoteSource) const
{
	return getDiploVoteCount(eVoteSource) > 0;
}


int CvGame::getDiploVoteCount(VoteSourceTypes eVoteSource) const
{
	FASSERT_BOUNDS(0, GC.getNumVoteSourceInfos(), eVoteSource);
	return m_aiDiploVote[eVoteSource];
}

void CvGame::changeDiploVote(VoteSourceTypes eVoteSource, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumVoteSourceInfos(), eVoteSource);

	if (0 != iChange)
	{
		if (!m_bRecalculatingModifiers)
		{
			for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
			{
				GET_PLAYER((PlayerTypes)iPlayer).processVoteSourceBonus(eVoteSource, false);
			}
		}

		m_aiDiploVote[eVoteSource] += iChange;
		FASSERT_NOT_NEGATIVE(getDiploVoteCount(eVoteSource));

		if (!m_bRecalculatingModifiers)
		{
			for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
			{
				GET_PLAYER((PlayerTypes)iPlayer).processVoteSourceBonus(eVoteSource, true);
			}
		}
	}
}

bool CvGame::canDoResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData) const
{
	const bool bVictory = GC.getVoteInfo(kData.eVote).isVictory();

	if (bVictory)
	{
		const bool bUN = isOption(GAMEOPTION_UNITED_NATIONS);
		for (int iTeam = 0; iTeam < MAX_PC_TEAMS; ++iTeam)
		{
			if (GET_TEAM((TeamTypes)iTeam).isVotingMember(eVoteSource)
			&& (bUN || GET_TEAM((TeamTypes)iTeam).getVotes(kData.eVote, eVoteSource) >= getVoteRequired(kData.eVote, eVoteSource)))
			{
				// Can't vote on a winner if one team already has all the votes necessary to win
				return false;
			}
		}
	}

	for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; ++iPlayer)
	{
		const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);

		if (kPlayer.isAlive())
		{
			if (kPlayer.isVotingMember(eVoteSource))
			{
				if (!kPlayer.canDoResolution(eVoteSource, kData))
				{
					return false;
				}
			}
			else if (bVictory && !kPlayer.isMinorCiv())
			{
				// all players need to be able to vote for a diplo victory
				return false;
			}
		}
	}

	return true;
}

bool CvGame::isValidVoteSelection(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData) const
{
	if (NO_PLAYER != kData.ePlayer)
	{
		const CvPlayer& kPlayer = GET_PLAYER(kData.ePlayer);
		if (!kPlayer.isAlive() || kPlayer.isNPC() || kPlayer.isMinorCiv())
		{
			return false;
		}
	}

	if (NO_PLAYER != kData.eOtherPlayer)
	{
		const CvPlayer& kPlayer = GET_PLAYER(kData.eOtherPlayer);
		if (!kPlayer.isAlive() || kPlayer.isNPC() || kPlayer.isMinorCiv())
		{
			return false;
		}
	}

	int iNumVoters = 0;
	for (int iTeam = 0; iTeam < MAX_PC_TEAMS; ++iTeam)
	{
		if (GET_TEAM((TeamTypes)iTeam).isVotingMember(eVoteSource))
		{
			++iNumVoters;
		}
	}
	if (iNumVoters  < GC.getVoteInfo(kData.eVote).getMinVoters())
	{
		return false;
	}

	if (GC.getVoteInfo(kData.eVote).isOpenBorders())
	{
		bool bOpenWithEveryone = true;
		for (int iTeam1 = 0; iTeam1 < MAX_PC_TEAMS; ++iTeam1)
		{
			const CvTeam& team1 = GET_TEAM((TeamTypes)iTeam1);

			if (team1.isFullMember(eVoteSource))
			{
				for (int iTeam2 = iTeam1 + 1; iTeam2 < MAX_PC_TEAMS; ++iTeam2)
				{
					if (!team1.isOpenBorders((TeamTypes)iTeam2)
					&& GET_TEAM((TeamTypes)iTeam2).isFullMember(eVoteSource))
					{
						bOpenWithEveryone = false;
						break;
					}
				}
				if (!bOpenWithEveryone) break;
			}
		}
		if (bOpenWithEveryone) return false;
	}
	else if (GC.getVoteInfo(kData.eVote).isDefensivePact())
	{
		bool bPactWithEveryone = true;
		for (int iTeam1 = 0; iTeam1 < MAX_PC_TEAMS; ++iTeam1)
		{
			const CvTeam& team1 = GET_TEAM((TeamTypes)iTeam1);

			if (team1.isFullMember(eVoteSource))
			{
				for (int iTeam2 = iTeam1 + 1; iTeam2 < MAX_PC_TEAMS; ++iTeam2)
				{
					if (!team1.isDefensivePact((TeamTypes)iTeam2)
					&& GET_TEAM((TeamTypes)iTeam2).isFullMember(eVoteSource))
					{
						bPactWithEveryone = false;
						break;
					}
				}
				if (!bPactWithEveryone) break;
			}
		}
		if (bPactWithEveryone)
		{
			return false;
		}
	}
	else if (GC.getVoteInfo(kData.eVote).isForcePeace())
	{
		const CvPlayer& kPlayer = GET_PLAYER(kData.ePlayer);

		if (GET_TEAM(kPlayer.getTeam()).isAVassal())
		{
			return false;
		}

		if (!kPlayer.isFullMember(eVoteSource))
		{
			return false;
		}

		bool bValid = false;
		for (int iTeam2 = 0; iTeam2 < MAX_PC_TEAMS; ++iTeam2)
		{
			if (atWar(kPlayer.getTeam(), (TeamTypes)iTeam2))
			{
				if (GET_TEAM((TeamTypes)iTeam2).isVotingMember(eVoteSource))
				{
					bValid = true;
					break;
				}
			}
		}
		if (!bValid)
		{
			return false;
		}
	}
	else if (GC.getVoteInfo(kData.eVote).isForceNoTrade())
	{
		if (GET_PLAYER(kData.ePlayer).isFullMember(eVoteSource))
		{
			return false;
		}
		const TeamTypes eTeam = GET_PLAYER(kData.ePlayer).getTeam();
		bool bNoTradeWithEveryone = true;
		for (int iI = 0; iI < MAX_PC_PLAYERS; ++iI)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(eTeam, false)
			&&  GET_PLAYER((PlayerTypes)iI).isFullMember(eVoteSource)
			&&  GET_PLAYER((PlayerTypes)iI).canStopTradingWithTeam(eTeam))
			{
				bNoTradeWithEveryone = false;
				break;
			}
		}
		// Not an option if already at war with everyone
		if (bNoTradeWithEveryone)
		{
			return false;
		}
	}
	else if (GC.getVoteInfo(kData.eVote).isForceWar())
	{
		const CvPlayer& kPlayer = GET_PLAYER(kData.ePlayer);
		const CvTeam& kTeam = GET_TEAM(kPlayer.getTeam());

		if (kTeam.isAVassal())
		{
			return false;
		}

		if (kPlayer.isFullMember(eVoteSource))
		{
			return false;
		}

		bool bAtWarWithEveryone = true;
		for (int iTeam2 = 0; iTeam2 < MAX_PC_TEAMS; ++iTeam2)
		{
			if (iTeam2 != kPlayer.getTeam())
			{
				const CvTeam& kTeam2 = GET_TEAM((TeamTypes)iTeam2);
				if (kTeam2.isFullMember(eVoteSource)
				&& !kTeam2.isAtWar(kPlayer.getTeam())
				&&  kTeam2.canChangeWarPeace(kPlayer.getTeam()))
				{
					bAtWarWithEveryone = false;
					break;
				}
			}
		}
		// Not an option if already at war with everyone
		if (bAtWarWithEveryone)
		{
			return false;
		}

		// Can be passed against a non-member only if he is already at war with a member
		if (!kPlayer.isVotingMember(eVoteSource))
		{
			bool bValid = false;
			for (int iTeam2 = 0; iTeam2 < MAX_PC_TEAMS; ++iTeam2)
			{
				if (atWar(kPlayer.getTeam(), (TeamTypes)iTeam2)
				&& GET_TEAM((TeamTypes)iTeam2).isFullMember(eVoteSource))
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
	}
	else if (GC.getVoteInfo(kData.eVote).isAssignCity())
	{
		const CvPlayer& kPlayer = GET_PLAYER(kData.ePlayer);
		if (kPlayer.isFullMember(eVoteSource) || !kPlayer.isVotingMember(eVoteSource))
		{
			return false;
		}

		const CvCity* pCity = kPlayer.getCity(kData.iCityId);
		if (NULL == pCity)
		{
			FErrorMsg("Referenced city not found");
			return false;
		}

		if (NO_PLAYER == kData.eOtherPlayer)
		{
			return false;
		}

		const CvPlayer& kOtherPlayer = GET_PLAYER(kData.eOtherPlayer);
		if (kOtherPlayer.getTeam() == kPlayer.getTeam())
		{
			return false;
		}

		if (atWar(kPlayer.getTeam(), GET_PLAYER(kData.eOtherPlayer).getTeam()))
		{
			return false;
		}

		if (!kOtherPlayer.isFullMember(eVoteSource))
		{
			return false;
		}

		if (isOption(GAMEOPTION_ONE_CITY_CHALLENGE))
		{
			return false;
		}
	}

	if (!canDoResolution(eVoteSource, kData))
	{
		return false;
	}

	return true;
}


bool CvGame::isDebugMode() const
{
	return m_bDebugModeCache;
}


void CvGame::toggleDebugMode()
{
	m_bDebugMode = ((m_bDebugMode) ? false : true);
	updateDebugModeCache();

	gDLL->getEngineIFace()->RebuildAllPlots();

	GC.getMap().setupGraphical();
	GC.getMap().updateVisibility();
	GC.getMap().updateSymbols();
	GC.getMap().updateFlagSymbols();
	GC.getMap().updateMinimapColor();

	gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(MinimapSection_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(UnitInfo_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);

	//gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);
	gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true);
	gDLL->getEngineIFace()->SetDirty(CultureBorders_DIRTY_BIT, true);

	if (m_bDebugMode)
	{
		gDLL->getEngineIFace()->PushFogOfWar(FOGOFWARMODE_OFF);
	}
	else
	{
		gDLL->getEngineIFace()->PopFogOfWar();
	}
	gDLL->getEngineIFace()->setFogOfWarFromStack();
}

void CvGame::noteGraphicRebuildNeeded()
{
	m_lastGraphicUpdateRequestTickCount = GetTickCount();
}

void CvGame::updateDebugModeCache()
{
	//if ((gDLL->getChtLvl() > 0) || (gDLL->GetWorldBuilderMode()))
	//{
		m_bDebugModeCache = m_bDebugMode;
	//}
	//else
	//{
	//	m_bDebugModeCache = false;
	//}
}

int CvGame::getPitbossTurnTime() const
{
	return GC.getInitCore().getPitbossTurnTime();
}

void CvGame::setPitbossTurnTime(int iHours)
{
	GC.getInitCore().setPitbossTurnTime(iHours);
}


bool CvGame::isHotSeat() const
{
	return GC.getInitCore().getHotseat();
}

bool CvGame::isPbem() const
{
	return GC.getInitCore().getPbem();
}



bool CvGame::isPitboss() const
{
	return GC.getInitCore().getPitboss();
}

bool CvGame::isSimultaneousTeamTurns() const
{
	return isNetworkMultiPlayer() && !isMPOption(MPOPTION_SIMULTANEOUS_TURNS);
}

bool CvGame::isFinalInitialized() const
{
	return m_bFinalInitialized;
}


void CvGame::setFinalInitialized(bool bNewValue)
{
	OutputDebugString("Setting FinalInitialized: Start\n");
	PROFILE_FUNC();

	if (m_bFinalInitialized != bNewValue)
	{
		m_bFinalInitialized = bNewValue;

		if (bNewValue)
		{
			updatePlotGroups();

			GC.getMap().updateIrrigated();

			for (int iI = 0; iI < MAX_TEAMS; iI++)
			{
				if (GET_TEAM((TeamTypes)iI).isAlive())
				{
					GET_TEAM((TeamTypes)iI).AI_updateAreaStragies();
				}
			}
		}
	}
	OutputDebugString("Setting FinalInitialized: End\n");
}


bool CvGame::getPbemTurnSent() const
{
	return m_bPbemTurnSent;
}


void CvGame::setPbemTurnSent(bool bNewValue)
{
	m_bPbemTurnSent = bNewValue;
}


bool CvGame::getHotPbemBetweenTurns() const
{
	return m_bHotPbemBetweenTurns;
}


void CvGame::setHotPbemBetweenTurns(bool bNewValue)
{
	m_bHotPbemBetweenTurns = bNewValue;
}


bool CvGame::isPlayerOptionsSent() const
{
	return m_bPlayerOptionsSent;
}


void CvGame::sendPlayerOptions(bool bForce)
{
	if (getActivePlayer() != NO_PLAYER && (bForce || !isPlayerOptionsSent()))
	{
		m_bPlayerOptionsSent = true;

		for (int iI = 0; iI < NUM_PLAYEROPTION_TYPES; iI++)
		{
			gDLL->sendPlayerOption(((PlayerOptionTypes)iI), gDLL->getPlayerOption((PlayerOptionTypes)iI));
		}

		for (int i=0; i<MAX_PLAYERS; i++)
		{
			SAFE_DELETE(GET_PLAYER((PlayerTypes)i).m_pBuildLists);
			GET_PLAYER((PlayerTypes)i).m_pBuildLists = new CvBuildLists();
		}
		GET_PLAYER(getActivePlayer()).m_pBuildLists->readFromFile();
		CvMessageControl::getInstance().sendBuildLists();
	}
}


PlayerTypes CvGame::getActivePlayer() const
{
	return GC.getInitCore().getActivePlayer();
}


void CvGame::setActivePlayer(PlayerTypes eNewValue, bool bForceHotSeat)
{
	const PlayerTypes eOldActivePlayer = getActivePlayer();
	if (eOldActivePlayer != eNewValue)
	{
		const int iActiveNetId = ((NO_PLAYER != eOldActivePlayer) ? GET_PLAYER(eOldActivePlayer).getNetID() : -1);
		GC.getInitCore().setActivePlayer(eNewValue);

		if (GET_PLAYER(eNewValue).isHuman() && (isHotSeat() || isPbem() || bForceHotSeat))
		{
			gDLL->getPassword(eNewValue);
			setHotPbemBetweenTurns(false);
			gDLL->getInterfaceIFace()->dirtyTurnLog(eNewValue);

			if (NO_PLAYER != eOldActivePlayer)
			{
				const int iInactiveNetId = GET_PLAYER(eNewValue).getNetID();
				GET_PLAYER(eNewValue).setNetID(iActiveNetId);
				GET_PLAYER(eOldActivePlayer).setNetID(iInactiveNetId);
			}

			GET_PLAYER(eNewValue).showMissedMessages();

			if (countHumanPlayersAlive() == 1 && isPbem())
			{
				// Nobody else left alive
				GC.getInitCore().setType(GAME_HOTSEAT_NEW);
			}

			if (isHotSeat() || bForceHotSeat)
			{
				sendPlayerOptions(true);
			}
		}

		if (GC.IsGraphicsInitialized())
		{
			GC.getMap().updateFog();
			GC.getMap().updateVisibility();
			GC.getMap().updateSymbols();
			GC.getMap().updateMinimapColor();

			updateUnitEnemyGlow();

			gDLL->getInterfaceIFace()->setEndTurnMessage(false);

			gDLL->getInterfaceIFace()->clearSelectedCities();
			gDLL->getInterfaceIFace()->clearSelectionList();

			gDLL->getInterfaceIFace()->setDirty(PercentButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(ResearchButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(MinimapSection_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(UnitInfo_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(Flag_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);

			gDLL->getEngineIFace()->SetDirty(CultureBorders_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(BlockadedPlots_DIRTY_BIT, true);
		}
	}
}

void CvGame::updateUnitEnemyGlow()
{
	PROFILE_FUNC();

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		foreach_(CvUnit* pLoopUnit, GET_PLAYER((PlayerTypes)iI).units()
		| filtered(!bind(CvUnit::isUsingDummyEntities, _1)))
		{
			gDLL->getEntityIFace()->updateEnemyGlow(pLoopUnit->getUnitEntity());
		}
	}
}

HandicapTypes CvGame::getHandicapType() const
{
	return m_eHandicap;
}

void CvGame::setHandicapType(HandicapTypes eHandicap)
{
	m_eHandicap = eHandicap;
}


PlayerTypes CvGame::getPausePlayer() const
{
	return m_ePausePlayer;
}

bool CvGame::isPaused() const
{
	return getPausePlayer() != NO_PLAYER;
}

void CvGame::setPausePlayer(PlayerTypes eNewValue)
{
	m_ePausePlayer = eNewValue;
}


UnitTypes CvGame::getBestLandUnit() const
{
	if (m_eBestLandUnit == NO_UNIT)
	{
		// This can occur after a load in which the previous best land unit was deleted from the assets
		// Just recalculate on demand
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				foreach_(const CvUnit* pLoopUnit, GET_PLAYER((PlayerTypes)iI).units())
				{
					if (pLoopUnit->baseCombatStr() > 0
					&& (m_eBestLandUnit == NO_UNIT || pLoopUnit->baseCombatStrNonGranular() > getBestLandUnitCombat()))
					{
						m_eBestLandUnit = pLoopUnit->getUnitType();
					}
				}
			}
		}
	}
	return m_eBestLandUnit;
}


int CvGame::getBestLandUnitCombat() const
{
	return getBestLandUnit() == NO_UNIT ? 1 : std::max(1, GC.getUnitInfo(getBestLandUnit()).getCombat());
}


void CvGame::setBestLandUnit(UnitTypes eNewValue)
{
	if (getBestLandUnit() != eNewValue)
	{
		m_eBestLandUnit = eNewValue;

		gDLL->getInterfaceIFace()->setDirty(UnitInfo_DIRTY_BIT, true);
	}
}


TeamTypes CvGame::getWinner() const
{
	return m_eWinner;
}


VictoryTypes CvGame::getVictory() const
{
	return m_eVictory;
}


void CvGame::setWinner(TeamTypes eNewWinner, VictoryTypes eNewVictory)
{
	if (m_eWinner != eNewWinner || m_eVictory != eNewVictory)
	{
		m_eWinner = eNewWinner;
		m_eVictory = eNewVictory;

		CvEventReporter::getInstance().victory(eNewWinner, eNewVictory);

		if (eNewVictory != NO_VICTORY)
		{
			if (eNewWinner != NO_TEAM)
			{
				addReplayMessage(
					REPLAY_MESSAGE_MAJOR_EVENT, GET_TEAM(eNewWinner).getLeaderID(),
					gDLL->getText(
						"TXT_KEY_GAME_WON",
						GET_TEAM(eNewWinner).getName().GetCString(),
						GC.getVictoryInfo(eNewVictory).getTextKeyWide()
					),
					-1, -1, GC.getCOLOR_HIGHLIGHT_TEXT()
				);
			}
			if (getAIAutoPlay(getActivePlayer()) > 0 || gDLL->GetAutorun())
			{
				setGameState(GAMESTATE_EXTENDED);
			}
			else setGameState(GAMESTATE_OVER);
		}
		gDLL->getInterfaceIFace()->setDirty(Center_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Soundtrack_DIRTY_BIT, true);
	}
}


GameStateTypes CvGame::getGameState() const
{
	return m_eGameState;
}


void CvGame::setGameState(GameStateTypes eNewValue)
{
	if (m_eGameState != eNewValue)
	{
		m_eGameState = eNewValue;

		if (eNewValue == GAMESTATE_OVER)
		{
			CvEventReporter::getInstance().gameEnd(getGameTurn());

			showEndGameSequence();

			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isHuman())
				{
					// One more turn?
					CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_EXTENDED_GAME);
					if (NULL != pInfo)
					{
						GET_PLAYER((PlayerTypes)iI).addPopup(pInfo);
					}
				}
			}
		}
		gDLL->getInterfaceIFace()->setDirty(Cursor_DIRTY_BIT, true);
	}
}


GameSpeedTypes CvGame::getGameSpeedType() const
{
	return GC.getInitCore().getGameSpeed();
}


EraTypes CvGame::getStartEra() const
{
	return GC.getInitCore().getEra();
}


CalendarTypes CvGame::getCalendar() const
{
	return GC.getInitCore().getCalendar();
}


PlayerTypes CvGame::getRankPlayer(int iRank) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, iRank);
	return (PlayerTypes)m_aiRankPlayer[iRank];
}


void CvGame::setRankPlayer(int iRank, PlayerTypes ePlayer)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, iRank);

	if (getRankPlayer(iRank) != ePlayer)
	{
		m_aiRankPlayer[iRank] = ePlayer;

		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}
}


int CvGame::getPlayerRank(PlayerTypes ePlayer) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, ePlayer);
	return m_aiPlayerRank[ePlayer];
}


void CvGame::setPlayerRank(PlayerTypes ePlayer, int iRank)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, ePlayer);

	// Attitude cache
	if (iRank != m_aiPlayerRank[ePlayer])
	{
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				GET_PLAYER(ePlayer).AI_invalidateAttitudeCache((PlayerTypes)iI);
				GET_PLAYER((PlayerTypes)iI).AI_invalidateAttitudeCache(ePlayer);
			}
		}
	}
	m_aiPlayerRank[ePlayer] = iRank;
	FASSERT_NOT_NEGATIVE(getPlayerRank(ePlayer));
}


int CvGame::getPlayerScore(PlayerTypes ePlayer)	const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, ePlayer);
	return m_aiPlayerScore[ePlayer];
}


void CvGame::setPlayerScore(PlayerTypes ePlayer, int iScore)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, ePlayer);

	if (getPlayerScore(ePlayer) != iScore)
	{
		m_aiPlayerScore[ePlayer] = iScore;
		FASSERT_NOT_NEGATIVE(getPlayerScore(ePlayer));

		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}
}


TeamTypes CvGame::getRankTeam(int iRank) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, iRank);
	return (TeamTypes)m_aiRankTeam[iRank];
}


void CvGame::setRankTeam(int iRank, TeamTypes eTeam)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

	if (getRankTeam(iRank) != eTeam)
	{
		m_aiRankTeam[iRank] = eTeam;

		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}
}


int CvGame::getTeamRank(TeamTypes eTeam) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);
	return m_aiTeamRank[eTeam];
}


void CvGame::setTeamRank(TeamTypes eTeam, int iRank)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);
	m_aiTeamRank[eTeam] = iRank;
	FASSERT_NOT_NEGATIVE(getTeamRank(eTeam));
}


int CvGame::getTeamScore(TeamTypes eTeam) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);
	return m_aiTeamScore[eTeam];
}


void CvGame::setTeamScore(TeamTypes eTeam, int iScore)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam)
	m_aiTeamScore[eTeam] = iScore;
	FASSERT_NOT_NEGATIVE(getTeamScore(eTeam));
}


bool CvGame::isOption(GameOptionTypes eIndex) const
{
	// Added this because TB TraitInfos call this function while the xmls are still being loaded
	if (GC.getInitCore().getMode() == NO_GAMEMODE)
	{
		return false;
	}

	return GC.getInitCore().getOption(eIndex);
}


void CvGame::setOption(GameOptionTypes eIndex, bool bEnabled)
{
	GC.getInitCore().setOption(eIndex, bEnabled);
}


bool CvGame::isMPOption(MultiplayerOptionTypes eIndex) const
{
	return GC.getInitCore().getMPOption(eIndex);
}


void CvGame::setMPOption(MultiplayerOptionTypes eIndex, bool bEnabled)
{
	GC.getInitCore().setMPOption(eIndex, bEnabled);
}


bool CvGame::isForcedControl(ForceControlTypes eIndex) const
{
	return GC.getInitCore().getForceControl(eIndex);
}


void CvGame::setForceControl(ForceControlTypes eIndex, bool bEnabled)
{
	GC.getInitCore().setForceControl(eIndex, bEnabled);
}


int CvGame::getUnitCreatedCount(UnitTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eIndex);
	return m_paiUnitCreatedCount[eIndex];
}


void CvGame::incrementUnitCreatedCount(UnitTypes eIndex)
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eIndex);
	m_paiUnitCreatedCount[eIndex]++;
}


bool CvGame::isUnitMaxedOut(UnitTypes eIndex, int iExtra) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eIndex);

	if (!isWorldUnit(eIndex))
	{
		return false;
	}
	FAssertMsg(getUnitCreatedCount(eIndex) <= GC.getUnitInfo(eIndex).getMaxGlobalInstances(), "Index is expected to be within maximum bounds (invalid Index)");

	return getUnitCreatedCount(eIndex) + iExtra >= GC.getUnitInfo(eIndex).getMaxGlobalInstances();
}


int CvGame::getBuildingCreatedCount(BuildingTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	return m_paiBuildingCreatedCount[eIndex];
}


bool CvGame::isBuildingMaxedOut(BuildingTypes eIndex, int iExtra) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);

	if (!isWorldWonder(eIndex))
	{
		return false;
	}

	return ((getBuildingCreatedCount(eIndex) + iExtra) >= GC.getBuildingInfo(eIndex).getMaxGlobalInstances());
}


void CvGame::changeNumBuildings(const BuildingTypes eIndex, const short iChange)
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eIndex);
	m_paiBuildingCreatedCount[eIndex] += iChange;
	FAssert(m_paiBuildingCreatedCount[eIndex] > -1);
}


int CvGame::getProjectCreatedCount(ProjectTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumProjectInfos(), eIndex);
	return m_paiProjectCreatedCount[eIndex];
}


bool CvGame::isProjectMaxedOut(ProjectTypes eIndex, int iExtra) const
{
	FASSERT_BOUNDS(0, GC.getNumProjectInfos(), eIndex);

	if (!isWorldProject(eIndex))
	{
		return false;
	}

	FAssertMsg(getProjectCreatedCount(eIndex) <= GC.getProjectInfo(eIndex).getMaxGlobalInstances(), "Index is expected to be within maximum bounds (invalid Index)");

	return ((getProjectCreatedCount(eIndex) + iExtra) >= GC.getProjectInfo(eIndex).getMaxGlobalInstances());
}


void CvGame::incrementProjectCreatedCount(ProjectTypes eIndex, int iExtra)
{
	FASSERT_BOUNDS(0, GC.getNumProjectInfos(), eIndex);
	m_paiProjectCreatedCount[eIndex] += iExtra;
}


int CvGame::getForceCivicCount(CivicTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumCivicInfos(), eIndex);
	return m_paiForceCivicCount[eIndex];
}


bool CvGame::isForceCivic(CivicTypes eIndex) const
{
	return (getForceCivicCount(eIndex) > 0);
}


bool CvGame::isForceCivicOption(CivicOptionTypes eCivicOption) const
{
	for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		if (GC.getCivicInfo((CivicTypes)iI).getCivicOptionType() == eCivicOption && isForceCivic((CivicTypes)iI))
		{
			return true;
		}
	}
	return false;
}


void CvGame::changeForceCivicCount(CivicTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumCivicInfos(), eIndex);

	if (iChange != 0)
	{
		const bool bOldForceCivic = isForceCivic(eIndex);

		m_paiForceCivicCount[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(getForceCivicCount(eIndex));

		if (bOldForceCivic != isForceCivic(eIndex))
		{
			verifyCivics();
		}
	}
}


PlayerVoteTypes CvGame::getVoteOutcome(VoteTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumVoteInfos(), eIndex);
	return m_paiVoteOutcome[eIndex];
}


bool CvGame::isVotePassed(VoteTypes eIndex) const
{
	const PlayerVoteTypes ePlayerVote = getVoteOutcome(eIndex);

	if (isTeamVote(eIndex))
	{
		return ePlayerVote >= 0 && ePlayerVote < MAX_PC_TEAMS;
	}
	return ePlayerVote == PLAYER_VOTE_YES;
}


void CvGame::setVoteOutcome(const VoteTriggeredData& kData, PlayerVoteTypes eNewValue)
{
	const VoteTypes eIndex = kData.kVoteOption.eVote;
	FASSERT_BOUNDS(0, GC.getNumVoteInfos(), eIndex);

	if (getVoteOutcome(eIndex) != eNewValue)
	{
		const bool bOldPassed = isVotePassed(eIndex);

		m_paiVoteOutcome[eIndex] = eNewValue;

		if (bOldPassed != isVotePassed(eIndex))
		{
			processVote(kData, isVotePassed(eIndex) ? 1 : -1);
		}
	}

	for (int iI = 0; iI < MAX_PC_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			GET_PLAYER((PlayerTypes)iI).setVote(kData.getID(), NO_PLAYER_VOTE);
		}
	}
}


int CvGame::getReligionGameTurnFounded(ReligionTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumReligionInfos(), eIndex);
	return m_paiReligionGameTurnFounded[eIndex];
}


bool CvGame::isReligionFounded(ReligionTypes eIndex) const
{
	return (getReligionGameTurnFounded(eIndex) != -1);
}


void CvGame::makeReligionFounded(ReligionTypes eIndex, PlayerTypes ePlayer)
{
	FASSERT_BOUNDS(0, GC.getNumReligionInfos(), eIndex);

	if (!isReligionFounded(eIndex))
	{
		FAssertMsg(getGameTurn() != -1, "getGameTurn() is not expected to be equal with -1");
		m_paiReligionGameTurnFounded[eIndex] = getGameTurn();

		CvEventReporter::getInstance().religionFounded(eIndex, ePlayer);

		// Sanguo Mod Performance, added by poyuzhe 07.26.09
		for (int iI = 0; iI < GC.getMAX_PLAYERS(); iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getStateReligion() == eIndex)
			{
				GET_PLAYER(ePlayer).AI_invalidateAttitudeCache((PlayerTypes)iI);
				GET_PLAYER((PlayerTypes)iI).AI_invalidateAttitudeCache(ePlayer);
			}
		}
	}
}


int CvGame::getTechGameTurnDiscovered(TechTypes eTech) const
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eTech);
	return m_paiTechGameTurnDiscovered[eTech];
}


bool CvGame::isTechDiscovered(TechTypes eTech) const
{
	return getTechGameTurnDiscovered(eTech) != -1;
}


void CvGame::makeTechDiscovered(TechTypes eTech)
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eTech);

	if (!isTechDiscovered(eTech))
	{
		FAssertMsg(getGameTurn() != -1, "getGameTurn() is not expected to be equal with -1");
		m_paiTechGameTurnDiscovered[eTech] = getGameTurn();
	}
}

bool CvGame::isReligionSlotTaken(ReligionTypes eReligion) const
{
	FASSERT_BOUNDS(0, GC.getNumReligionInfos(), eReligion);
	return m_abReligionSlotTaken[eReligion];
}

void CvGame::setReligionSlotTaken(ReligionTypes eReligion, bool bTaken)
{
	FASSERT_BOUNDS(0, GC.getNumReligionInfos(), eReligion);
	m_abReligionSlotTaken[eReligion] = bTaken;
}

bool CvGame::isTechCanFoundReligion(TechTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eIndex);
	return m_abTechCanFoundReligion[eIndex];
}

void CvGame::setTechCanFoundReligion(TechTypes eIndex, bool bUsed)
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eIndex);
	m_abTechCanFoundReligion[eIndex] = !bUsed;
}

bool CvGame::isGameStart()
{
	if (m_bGameStart)
	{
		checkGameStart();
	}
	return m_bGameStart;
}

void CvGame::checkGameStart()
{
	if (getHighestEra() >= getStartEra())
	{
		m_bGameStart = false;
	}
}

int CvGame::countNumReligionsFounded() const
{
	int iCount = 0;
	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if (isReligionFounded((ReligionTypes)iI))
		{
			iCount++;
		}
	}
	return iCount;
}

int CvGame::countNumReligionTechsDiscovered() const
{
	int iCount = 0;
	foreach_(const CvReligionInfo* info, GC.getReligionInfos())
	{
		if (countKnownTechNumTeams(info->getTechPrereq()) > 0)
		{
			iCount++;
		}
	}
	return iCount;
}

int CvGame::getCorporationGameTurnFounded(CorporationTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumCorporationInfos(), eIndex);
	return m_paiCorporationGameTurnFounded[eIndex];
}


bool CvGame::isCorporationFounded(CorporationTypes eIndex) const
{
	return getCorporationGameTurnFounded(eIndex) != -1;
}


void CvGame::makeCorporationFounded(CorporationTypes eIndex, PlayerTypes ePlayer)
{
	FASSERT_BOUNDS(0, GC.getNumCorporationInfos(), eIndex);

	if (!isCorporationFounded(eIndex))
	{
		FAssertMsg(getGameTurn() != -1, "getGameTurn() is not expected to be equal with -1");
		m_paiCorporationGameTurnFounded[eIndex] = getGameTurn();

		CvEventReporter::getInstance().corporationFounded(eIndex, ePlayer);
	}
}

bool CvGame::isVictoryValid(VictoryTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumVictoryInfos(), eIndex);
	return GC.getInitCore().getVictory(eIndex);
}

void CvGame::setVictoryValid(VictoryTypes eIndex, bool bValid)
{
	FASSERT_BOUNDS(0, GC.getNumVictoryInfos(), eIndex);
	GC.getInitCore().setVictory(eIndex, bValid);
}


bool CvGame::isSpecialUnitValid(SpecialUnitTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialUnitInfos(), eIndex);
	return m_pabSpecialUnitValid[eIndex];
}


void CvGame::makeSpecialUnitValid(SpecialUnitTypes eIndex)
{
	FASSERT_BOUNDS(0, GC.getNumSpecialUnitInfos(), eIndex);
	m_pabSpecialUnitValid[eIndex] = true;
}


bool CvGame::isSpecialBuildingValid(SpecialBuildingTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialBuildingInfos(), eIndex);
	return m_pabSpecialBuildingValid[eIndex];
}


void CvGame::makeSpecialBuildingValid(SpecialBuildingTypes eIndex, bool bAnnounce)
{
	FASSERT_BOUNDS(0, GC.getNumSpecialBuildingInfos(), eIndex);

	if (!m_pabSpecialBuildingValid[eIndex])
	{
		m_pabSpecialBuildingValid[eIndex] = true;

		if (bAnnounce)
		{
			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHuman())
				{
					AddDLLMessage(
						(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_SPECIAL_BUILDING_VALID", GC.getSpecialBuildingInfo(eIndex).getTextKeyWide()),
						"AS2D_PROJECT_COMPLETED", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_HIGHLIGHT_TEXT()
					);
				}
			}
		}
	}
}


bool CvGame::isInAdvancedStart() const
{
	for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; iPlayer++)
	{
		if (GET_PLAYER((PlayerTypes)iPlayer).isHuman() && GET_PLAYER((PlayerTypes)iPlayer).getAdvancedStartPoints() >= 0)
		{
			return true;
		}
	}
	return false;
}

void CvGame::setVoteChosen(int iSelection, int iVoteId)
{
	VoteSelectionData* pVoteSelectionData = getVoteSelection(iVoteId);
	if (NULL != pVoteSelectionData)
	{
		addVoteTriggered(*pVoteSelectionData, iSelection);
	}
	deleteVoteSelection(iVoteId);
}


CvCity* CvGame::getHolyCity(ReligionTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumReligionInfos(), eIndex);
	return getCity(m_paHolyCity[eIndex]);
}


void CvGame::setHolyCity(ReligionTypes eIndex, const CvCity* pNewValue, bool bAnnounce)
{
	FASSERT_BOUNDS(0, GC.getNumReligionInfos(), eIndex);

	CvCity* pOldValue = getHolyCity(eIndex);

	if (pOldValue == pNewValue)
	{
		return;
	}
	if (pNewValue != NULL)
	{
		m_paHolyCity[eIndex] = pNewValue->getIDInfo();
	}
	else m_paHolyCity[eIndex].reset();


	if (pOldValue != NULL)
	{
		pOldValue->changeReligionInfluence(eIndex, -GC.getHOLY_CITY_INFLUENCE());
		pOldValue->updateReligionCommerce();
		pOldValue->setInfoDirty(true);
	}

	if (getHolyCity(eIndex) != NULL)
	{
		CvCity* pHolyCity = getHolyCity(eIndex);

		pHolyCity->setHasReligion(eIndex, true, bAnnounce, true);
		pHolyCity->changeReligionInfluence(eIndex, GC.getHOLY_CITY_INFLUENCE());
		pHolyCity->updateReligionCommerce();
		pHolyCity->setInfoDirty(true);

		if (bAnnounce && isFinalInitialized() && !gDLL->GetWorldBuilderMode())
		{
			addReplayMessage(
				REPLAY_MESSAGE_MAJOR_EVENT, pHolyCity->getOwner(),
				gDLL->getText(
					"TXT_KEY_MISC_REL_FOUNDED",
					GC.getReligionInfo(eIndex).getTextKeyWide(), pHolyCity->getNameKey()
				),
				pHolyCity->getX(), pHolyCity->getY(), GC.getCOLOR_HIGHLIGHT_TEXT()
			);
			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHuman())
				{
					if (pHolyCity->isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
					{
						AddDLLMessage(
							(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME_LONG(),
							gDLL->getText(
								"TXT_KEY_MISC_REL_FOUNDED",
								GC.getReligionInfo(eIndex).getTextKeyWide(), pHolyCity->getNameKey()
							),
							GC.getReligionInfo(eIndex).getSound(), MESSAGE_TYPE_MAJOR_EVENT,
							GC.getReligionInfo(eIndex).getButton(), GC.getCOLOR_HIGHLIGHT_TEXT(),
							pHolyCity->getX(), pHolyCity->getY()
						);
					}
					else
					{
						AddDLLMessage(
							(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME_LONG(),
							gDLL->getText(
								"TXT_KEY_MISC_REL_FOUNDED_UNKNOWN",
								GC.getReligionInfo(eIndex).getTextKeyWide()
							),
							GC.getReligionInfo(eIndex).getSound(), MESSAGE_TYPE_MAJOR_EVENT,
							GC.getReligionInfo(eIndex).getButton(), GC.getCOLOR_HIGHLIGHT_TEXT()
						);
					}
				}
			}
		}
	}
	AI_makeAssignWorkDirty();

	// Attitude cache
	if (isFinalInitialized())
	{
		for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
		{
			if (GET_PLAYER((PlayerTypes)iJ).isAlive() && GET_PLAYER((PlayerTypes)iJ).getStateReligion() == eIndex)
			{
				if (pNewValue != NULL)
				{
					GET_PLAYER(pNewValue->getOwner()).AI_invalidateAttitudeCache((PlayerTypes)iJ);
					GET_PLAYER((PlayerTypes)iJ).AI_invalidateAttitudeCache(pNewValue->getOwner());
				}
				if (pOldValue != NULL)
				{
					GET_PLAYER(pOldValue->getOwner()).AI_invalidateAttitudeCache((PlayerTypes)iJ);
					GET_PLAYER((PlayerTypes)iJ).AI_invalidateAttitudeCache(pOldValue->getOwner());
				}
			}
		}
	}
}


CvCity* CvGame::getHeadquarters(CorporationTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumCorporationInfos(), eIndex);
	return getCity(m_paHeadquarters[eIndex]);
}


void CvGame::setHeadquarters(CorporationTypes eIndex, CvCity* pNewValue, bool bAnnounce)
{
	FASSERT_BOUNDS(0, GC.getNumCorporationInfos(), eIndex);

	CvCity* pOldValue = getHeadquarters(eIndex);

	if (pOldValue != pNewValue)
	{
		if (pNewValue != NULL)
		{
			m_paHeadquarters[eIndex] = pNewValue->getIDInfo();
		}
		else
		{
			m_paHeadquarters[eIndex].reset();
		}

		if (pOldValue != NULL)
		{
			pOldValue->updateCorporation();

			pOldValue->setInfoDirty(true);
		}

		CvCity* pHeadquarters = getHeadquarters(eIndex);

		// Set and Replace Corporation HQ's
		if (pOldValue != NULL)
		{
			for (int i = 0; i < GC.getNumBuildingInfos(); i++)
			{
				if ((CorporationTypes)GC.getBuildingInfo((BuildingTypes)i).getGlobalCorporationCommerce() == eIndex)
				{
					pOldValue->setNumRealBuilding((BuildingTypes)i, 0);
					break;
				}
			}
		}
		if (pNewValue != NULL)
		{
			for (int i = 0; i < GC.getNumBuildingInfos(); i++)
			{
				if ((CorporationTypes)GC.getBuildingInfo((BuildingTypes)i).getGlobalCorporationCommerce() == eIndex)
				{
					pNewValue->setNumRealBuilding((BuildingTypes)i, 1);
					break;
				}
			}
		}

		if (NULL != pHeadquarters)
		{
			pHeadquarters->setHasCorporation(eIndex, true, bAnnounce);
			pHeadquarters->updateCorporation();
			pHeadquarters->setInfoDirty(true);

			if (bAnnounce && isFinalInitialized() && !gDLL->GetWorldBuilderMode())
			{
				const CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CORPORATION_FOUNDED", GC.getCorporationInfo(eIndex).getTextKeyWide(), pHeadquarters->getNameKey());
				addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, pHeadquarters->getOwner(), szBuffer, pHeadquarters->getX(), pHeadquarters->getY(), GC.getCOLOR_HIGHLIGHT_TEXT());

				for (int iI = 0; iI < MAX_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive())
					{
						if (pHeadquarters->isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
						{
							AddDLLMessage(
								(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME_LONG(),
								szBuffer, GC.getCorporationInfo(eIndex).getSound(),
								MESSAGE_TYPE_MAJOR_EVENT, GC.getCorporationInfo(eIndex).getButton(),
								GC.getCOLOR_HIGHLIGHT_TEXT(), pHeadquarters->getX(), pHeadquarters->getY()
							);
						}
						else
						{
							AddDLLMessage(
								(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME_LONG(),
								gDLL->getText("TXT_KEY_MISC_CORPORATION_FOUNDED_UNKNOWN", GC.getCorporationInfo(eIndex).getTextKeyWide()),
								GC.getCorporationInfo(eIndex).getSound(), MESSAGE_TYPE_MAJOR_EVENT,
								GC.getCorporationInfo(eIndex).getButton(), GC.getCOLOR_HIGHLIGHT_TEXT()
							);
						}
					}
				}
			}
		}
		AI_makeAssignWorkDirty();
	}
}


PlayerVoteTypes CvGame::getPlayerVote(PlayerTypes eOwnerIndex, int iVoteId) const
{
	FASSERT_BOUNDS(0, MAX_PC_PLAYERS, eOwnerIndex);
	FAssert(NULL != getVoteTriggered(iVoteId));

	return GET_PLAYER(eOwnerIndex).getVote(iVoteId);
}


void CvGame::setPlayerVote(PlayerTypes eOwnerIndex, int iVoteId, PlayerVoteTypes eNewValue)
{
	FASSERT_BOUNDS(0, MAX_PC_PLAYERS, eOwnerIndex);
	FAssert(NULL != getVoteTriggered(iVoteId));

	GET_PLAYER(eOwnerIndex).setVote(iVoteId, eNewValue);
}


void CvGame::castVote(PlayerTypes eOwnerIndex, int iVoteId, PlayerVoteTypes ePlayerVote)
{
	const VoteTriggeredData* pTriggeredData = getVoteTriggered(iVoteId);
	if (NULL != pTriggeredData)
	{
		if (GC.getVoteInfo(pTriggeredData->kVoteOption.eVote).isAssignCity())
		{
			FAssert(pTriggeredData->kVoteOption.ePlayer != NO_PLAYER);
			CvPlayer& kCityPlayer = GET_PLAYER(pTriggeredData->kVoteOption.ePlayer);

			if (GET_PLAYER(eOwnerIndex).getTeam() != kCityPlayer.getTeam())
			{
				switch (ePlayerVote)
				{
				case PLAYER_VOTE_YES:
					kCityPlayer.AI_changeMemoryCount(eOwnerIndex, MEMORY_VOTED_AGAINST_US, 1);
					break;
				case PLAYER_VOTE_NO:
					kCityPlayer.AI_changeMemoryCount(eOwnerIndex, MEMORY_VOTED_FOR_US, 1);
					break;
				default:
					break;
				}
			}
		}
		else if (isTeamVote(pTriggeredData->kVoteOption.eVote))
		{
			FASSERT_BOUNDS(0, MAX_TEAMS, ePlayerVote);

			if (GET_PLAYER(eOwnerIndex).getTeam() != (TeamTypes)ePlayerVote)
			{
				// Voted on another team, should be appreciated.
				for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; ++iPlayer)
				{
					if (GET_PLAYER((PlayerTypes)iPlayer).isAliveAndTeam((TeamTypes)ePlayerVote))
					{
						GET_PLAYER((PlayerTypes)iPlayer).AI_changeMemoryCount(eOwnerIndex, MEMORY_VOTED_FOR_US, 1);
					}
				}
			}
		}
		setPlayerVote(eOwnerIndex, iVoteId, ePlayerVote);
	}
}


std::string CvGame::getScriptData() const
{
	return m_szScriptData;
}


void CvGame::setScriptData(std::string szNewValue)
{
	m_szScriptData = szNewValue;
}

const CvWString & CvGame::getName()
{
	return GC.getInitCore().getGameName();
}


void CvGame::setName(const char* szName)
{
	GC.getInitCore().setGameName(szName);
}


bool CvGame::isDestroyedCityName(CvWString& szName) const
{
	return algo::any_of_equal(m_aszDestroyedCities, szName);
}

void CvGame::addDestroyedCityName(const CvWString& szName)
{
	m_aszDestroyedCities.push_back(szName);
}


bool CvGame::isGreatPersonBorn(CvWString& szName) const
{
	return algo::any_of_equal(m_aszGreatPeopleBorn, szName);
}

void CvGame::addGreatPersonBornName(const CvWString& szName)
{
	m_aszGreatPeopleBorn.push_back(szName);
}


// Protected Functions...

void CvGame::doTurn()
{
	PROFILE_BEGIN("CvGame::doTurn()");

	// END OF TURN
	CvEventReporter::getInstance().beginGameTurn( getGameTurn() );

	doUpdateCacheOnTurn();

	updateScore();

	// solve property system
	m_PropertySolver.doTurn();

	doDeals();

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			GET_TEAM((TeamTypes)iI).doTurn();
		}
	}

	reverse_foreach_(CvMap* map, GC.getMaps())
	{
		map->doTurn();
	}

	createBarbarianCities(false);
	if (isOption(GAMEOPTION_NEANDERTHAL_CITIES))
	{
		createBarbarianCities(true);
	}
	createBarbarianUnits();

	if (getElapsedGameTurns() > GC.getGameSpeedInfo(getGameSpeedType()).getGameTurnInfo(getStartEra()).iNumGameTurnsPerIncrement/80)
	{
		for (iI = MAX_PC_PLAYERS; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				doSpawns((PlayerTypes)iI);
			}
		}
	}
#ifdef GLOBAL_WARMING
	doGlobalWarming();
#endif

	doHeadquarters();

	doDiploVote();

	// Recalculate vision on load (a stickytape - can't find where it's skewing visibility counts)
	//Hopefully won't create a noteable delay but it may
	//disabled when debugging only - units should now be tracking and staying within a range of 0-1 for number of positive updates - negative updates.
	//TBVIS
	for (int iJ = 0; iJ < GC.getMap().numPlots(); iJ++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iJ);
		pLoopPlot->clearVisibilityCounts();
	}
	GC.getMap().updateSight(true, false);

	CvEventReporter::getInstance().preEndGameTurn(getGameTurn());

	gDLL->getInterfaceIFace()->setEndTurnMessage(false);
	gDLL->getInterfaceIFace()->setHasMovedUnit(false);

	for (int n=0; n < MAX_PLAYERS; n++)
	{
		if (isForcedAIAutoPlay((PlayerTypes)n))
		{
			if (getForcedAIAutoPlay((PlayerTypes)n) > 0)
			{
				changeForcedAIAutoPlay((PlayerTypes)n, -1);
			}
		}
		else if (getAIAutoPlay((PlayerTypes)n) > 0)
		{
			changeAIAutoPlay((PlayerTypes)n, -1);
		}
	}
	CvEventReporter::getInstance().endGameTurn(getGameTurn());
	incrementGameTurn();
	incrementElapsedGameTurns();

	if (isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
	{
		int aiShuffle[MAX_PLAYERS];
		shuffleArray(aiShuffle, MAX_PLAYERS, getSorenRand());

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			int iLoopPlayer = aiShuffle[iI];

			if (GET_PLAYER((PlayerTypes)iLoopPlayer).isAlive())
			{
				GET_PLAYER((PlayerTypes)iLoopPlayer).setTurnActive(true);
			}
		}
	}
	else if (isSimultaneousTeamTurns())
	{
		for (iI = 0; iI < MAX_TEAMS; iI++)
		{
			CvTeam& kTeam = GET_TEAM((TeamTypes)iI);
			if (kTeam.isAlive())
			{
				kTeam.setTurnActive(true);
				FAssert(getNumGameTurnActive() == kTeam.getAliveCount());
				break;
			}
		}
	}
	else
	{
		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (!isPbem() || GET_PLAYER((PlayerTypes)iI).isHuman())
				{
					GET_PLAYER((PlayerTypes)iI).setTurnActive(true);
					FAssert(getNumGameTurnActive() == 1);
				}
				else if (iI == getActivePlayer())
				{
					// Nobody else left alive
					GC.getInitCore().setType(GAME_HOTSEAT_NEW);
					GET_PLAYER((PlayerTypes)iI).setTurnActive(true);
				}
				else if (!getPbemTurnSent())
				{
					gDLL->sendPbemTurn((PlayerTypes)iI);
				}
				break;
			}
		}
	}
	doIncreasingDifficulty();
	doFlexibleDifficulty();
	doFinalFive();
	doHightoLow();
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		setPreviousRequest((PlayerTypes)iI, false);
	}
	doFoundCorporations();

	testVictory();

	gDLL->getEngineIFace()->SetDirty(GlobePartialTexture_DIRTY_BIT, true);
	gDLL->getEngineIFace()->DoTurn();

	PROFILE_END();

	foreach_(CvMap* map, GC.getMaps())
	{
		if (!map->plotsInitialized())
		{
			map->updateIncomingUnits();
		}
	}

	stopProfilingDLL(true);
	gDLL->getEngineIFace()->AutoSave();

#ifdef MEMTRACK
	//MemTrack::TrackListMemoryUsage(); // total
	MemTrack::TrackListMemoryUsageTurnDiff(); // leaks
#endif
}


void CvGame::doDeals()
{
	verifyDeals();

	int iLoop;
	for (CvDeal* pLoopDeal = firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = nextDeal(&iLoop))
	{
		pLoopDeal->doTurn();
	}
}

//Enumerates all currently possible spawn plots for a spawning rule, for use in a thread, local density is not checked
void enumSpawnPlots(int iSpawnInfo, std::vector<CvPlot*>* plots)
{
	const CvSpawnInfo& spawnInfo = GC.getSpawnInfo((SpawnTypes)iSpawnInfo);
	//logging::logMsg("C2C.log", "Spawn thread start for %s\n", spawnInfo.getType());

	if (spawnInfo.getRateOverride() == 0)
	{
		return;
	}
	if (spawnInfo.getTreatAsBarbarian() && GC.getGame().isOption(GAMEOPTION_NO_BARBARIANS))
	{
		return;
	}
	const TechTypes ePrereqTech = spawnInfo.getPrereqTech();
	const TechTypes eObsoleteTech = spawnInfo.getObsoleteTech();

	if (ePrereqTech != NO_TECH || eObsoleteTech != NO_TECH)
	{
		bool bAnyTeamHasPrereqTech = false;

		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (eObsoleteTech != NO_TECH && GET_TEAM((TeamTypes)iI).isHasTech(eObsoleteTech))
				{
					return;
				}
				if (GET_TEAM((TeamTypes)iI).isHasTech(ePrereqTech))
				{
					bAnyTeamHasPrereqTech = true;
					break;
				}
			}
		}
		if (!bAnyTeamHasPrereqTech)
		{
			return;
		}
	}
	else // Enables in this turn year?
	{
		const int iTurnYear = GC.getGame().getTurnYear(GC.getGame().getGameTurn());
		if (iTurnYear < spawnInfo.getStartDate() || iTurnYear > spawnInfo.getEndDate())
		{
			return;
		}
	}
	const CvUnitInfo& unitInfo = GC.getUnitInfo(spawnInfo.getUnitType());

	const bool bNoTerrainFeatureBonus = spawnInfo.getNumTerrains() == 0 && spawnInfo.getNumFeatures() == 0 && spawnInfo.getNumBonuses() == 0 && !spawnInfo.getPeaks();
	const bool bHills = spawnInfo.getHills();
	const bool bFlatLand = spawnInfo.getFlatlands();
	const bool bFreshWaterOnly = spawnInfo.getFreshWaterOnly();
	const bool bNotInView = spawnInfo.getNotInView();
	const bool bWildAnimal = unitInfo.isWildAnimal();
	const bool bAnimalBarred = spawnInfo.getNeutralOnly() || bWildAnimal && GC.getGame().isOption(GAMEOPTION_ANIMALS_STAY_OUT);

	const PlayerTypes ePlayer = spawnInfo.getPlayer();

	for (int i = 0; i < GC.getMap().numPlots(); ++i)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(i);

		if (pPlot->plotCount(PUF_isOtherPlayer, ePlayer) == 0)
		{
			if (bAnimalBarred && pPlot->getOwner() != NO_PLAYER && pPlot->getOwner() != ePlayer)
			{
				continue;
			}
			// TB - Actually, here the unit is selected already based on the spawn info itself.
			// Ensure that pPlot has the MapType of the unit... no need to include MapTypes on the Spawn Info file.
			if (!isMapCategory(*pPlot, unitInfo))
			{
				continue;
			}

			// Encroaching animals?
			if (
				bWildAnimal
				&&
				(
					pPlot->isOwned()
					&&
					(
						!unitInfo.canAnimalIgnoresBorders()

						|| pPlot->getImprovementType() != NO_IMPROVEMENT
						&& !unitInfo.canAnimalIgnoresImprovements()
					)
					|| pPlot->isCity(true)
					&& !unitInfo.canAnimalIgnoresCities()
				)
			) continue;

			bool bValid = pPlot->isHills() ? bHills : (pPlot->isFlatlands() ? bFlatLand : true);
			if (!bValid) continue;

			// Check latitude
			const int lat = spawnInfo.getLatitudeAbs() ? pPlot->getLatitude() : pPlot->getLatitudeRaw();

			bValid = lat >= spawnInfo.getMinLatitude() && lat <= spawnInfo.getMaxLatitude();
			if (!bValid) continue;

			const int iLongitude = pPlot->getLongitudeMinutes() / 60;
			const int iMinLongitude = spawnInfo.getMinLongitude();
			const int iMaxLongitude = spawnInfo.getMaxLongitude();
			if (iMinLongitude <= iMaxLongitude)
			{
				bValid = iLongitude >= iMinLongitude && iLongitude <= iMaxLongitude;
			}
			else bValid = iLongitude >= iMinLongitude || iLongitude <= iMaxLongitude;

			if (!bValid || bFreshWaterOnly && !pPlot->isFreshWater())
			{
				continue;
			}

			if (!bNoTerrainFeatureBonus)
			{
				if (!pPlot->isAsPeak())
				{
					bValid = false;
					BonusTypes bonusType = pPlot->getBonusType();
					for (int k = 0; k < spawnInfo.getNumBonuses(); k++)
					{
						if (spawnInfo.getBonus(k) == bonusType)
						{
							bValid = true;
							break;
						}
					}

					if (!bValid)
					{
						const FeatureTypes featureType = pPlot->getFeatureType();
						const TerrainTypes terrainType = pPlot->getTerrainType();
						if (featureType == NO_FEATURE)
						{
							for(int k = 0; k < spawnInfo.getNumTerrains(); k++)
							{
								if (spawnInfo.getTerrain(k) == terrainType)
								{
									bValid = true;
									break;
								}
							}
						}
						else
						{
							for (int k = 0; k < spawnInfo.getNumFeatures(); k++)
							{
								if (spawnInfo.getFeature(k) == featureType)
								{
									bValid = true;
									break;
								}
							}

							if (bValid)
							{
								bValid = spawnInfo.getNumFeatureTerrains() == 0;

								for (int k = 0; k < spawnInfo.getNumFeatureTerrains(); k++)
								{
									if (spawnInfo.getFeatureTerrain(k) == terrainType)
									{
										bValid = true;
										break;
									}
								}
							}
						}
					}
				}
				else bValid = spawnInfo.getPeaks();
			}

			if (!bValid || bNotInView && pPlot->isVisibleToCivTeam())
			{
				continue;
			}

			const BoolExpr* pCondition = spawnInfo.getSpawnCondition();

			if (pCondition && !pCondition->evaluate(pPlot->getGameObject()))
			{
				continue;
			}

			plots->push_back(pPlot); // Valid spawning spot
		}
	}
}

// Helpers
namespace Game {
	bool isGroupUpgradePromotion(const CvUnit* unit, PromotionTypes promotion)
	{
		return GC.getPromotionInfo(promotion).getGroupChange() > 0 && unit->canAcquirePromotion(promotion, PromotionRequirements::ForStatus);
	}
	bool isGroupDowngradePromotion(const CvUnit* unit, PromotionTypes promotion)
	{
		return GC.getPromotionInfo(promotion).getGroupChange() < 0 && unit->canAcquirePromotion(promotion, PromotionRequirements::ForStatus);
	}
}

void CvGame::doSpawns(PlayerTypes ePlayer)
{
	PROFILE_FUNC();

	//	We need to know the number of each unit type in each area, which is not soemthing CvArea
	//	can do (nor worth implementing it there), so do a pre-pass here to build that data
	std::map< int, std::map<int,int> >	areaPopulationMap;

	for (int i = 0; i < GC.getMap().numPlots(); ++i)
	{
		const CvPlot* pPlot = GC.getMap().plotByIndex(i);

		foreach_(const CvUnit* pLoopUnit, pPlot->units() | filtered(CvUnit::fn::getOwner() == ePlayer))
		{
			if (areaPopulationMap[pPlot->getArea()].find(pLoopUnit->getUnitType()) != areaPopulationMap[pPlot->getArea()].end())
			{
				areaPopulationMap[pPlot->getArea()][pLoopUnit->getUnitType()]++;
			}
			else areaPopulationMap[pPlot->getArea()][pLoopUnit->getUnitType()] = 1;
		}
	}
	const bool bRagingBarbarians = isOption(GAMEOPTION_RAGING_BARBARIANS);
	const bool bSizeMatters = isOption(GAMEOPTION_SIZE_MATTERS);

	for (int j = 0; j < GC.getNumSpawnInfos(); j++)
	{
		const CvSpawnInfo& spawnInfo = GC.getSpawnInfo((SpawnTypes)j);

		//TB Note: It is at this point that we need to isolate out the player type on spawn info.
		//I think for the sake of speed and data efficiency we can get away with a singular player reference rather than
		//making it possible to specify more than one on a spawn info.

		if (spawnInfo.getPlayer() != ePlayer || spawnInfo.getRateOverride() == 0 || bSizeMatters && spawnInfo.getNumSpawnGroup() > 1)
		{
			continue;
		}

		std::vector<CvPlot*> validPlots;

		enumSpawnPlots(j, &validPlots);

		const int iPlotNum = validPlots.size();
		logging::logMsg("C2C.log", "Spawn thread finished and joined for %s, found %d valid plots.", spawnInfo.getType(), iPlotNum);

		if (iPlotNum == 0)
		{
			continue;
		}
		algo::random_shuffle(validPlots, SorenRand("Spawn Shuffle"));

		double adjustedSpawnRate = (double)spawnInfo.getTurnRate();
		double fGlobalSpawnRate = (double)spawnInfo.getGlobalTurnRate() * iPlotNum / 100.0;
		if (adjustedSpawnRate <= 0)
		{
			adjustedSpawnRate = fGlobalSpawnRate;
		}
		else if (fGlobalSpawnRate > 0)
		{
			adjustedSpawnRate = 1 / (1 / adjustedSpawnRate + 1 / fGlobalSpawnRate);
		}

		if (!spawnInfo.getNoSpeedNormalization())
		{
			adjustedSpawnRate = adjustedSpawnRate * (GC.getGameSpeedInfo(getGameSpeedType()).getHammerCostPercent()+666) / 300;
		}

		if (isOption(GAMEOPTION_PEACE_AMONG_NPCS))
		{
			adjustedSpawnRate *= 100 + GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getSpawnRateNPCPeaceModifier();
		}
		else
		{
			adjustedSpawnRate *= 100 + GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getSpawnRateModifier();
		}
		// Adjust for any rate override
		adjustedSpawnRate /= spawnInfo.getRateOverride();

		int iMinAreaPlotsPerPlayerUnit = spawnInfo.getMinAreaPlotsPerPlayerUnit();
		int iMinAreaPlotsPerUnitType = spawnInfo.getMinAreaPlotsPerUnitType();

		// Double for barbarians if raging
		if (bRagingBarbarians && spawnInfo.getTreatAsBarbarian())
		{
			adjustedSpawnRate /= 2;
			iMinAreaPlotsPerPlayerUnit = iMinAreaPlotsPerPlayerUnit * 3/4;
			iMinAreaPlotsPerUnitType = iMinAreaPlotsPerUnitType * 2/3;
		}

		logging::logMsg("C2C.log", "Spawn chance per plot for %s is 1 to %d .", spawnInfo.getType(), (int)adjustedSpawnRate);

		//So we ARE going by spawn here but it's still a random check per plot rather than placing an amount.  Before this, determine how many should spawn this round, then pick a plot for each of those spawns.
		//The density factor is going to be interesting.  Perhaps each plot should get a likelihood value and vary that by the density factor around that plot.
		//The spawn rate... is high more likely or low and what kind of numeric range are we working with?

		/* Toffer - Disable this, this should be cached far less often than for each player each end turn. Not worth it.
		foreach_(CvArea* pArea, GC.getMap().areas())
		{
			const int iValidPlots = algo::count_if(validPlots, CvPlot::fn::area() == pArea);
			pArea->setNumValidPlotsbySpawn(eSpawn, iValidPlots);
		}
		*/
		const UnitTypes eUnit = spawnInfo.getUnitType();
		const int iMaxLocalDensity = spawnInfo.getMaxLocalDensity();
		int spawnCount = 0;

		foreach_(const CvPlot* pPlot, validPlots)
		{
			// Safety valve - if there are over a certain number of total barbarian units in existance halt all spawns
			// else the game can run into fatal issues when it uses up the entire id space of units (8191 at once per player)
			if (GET_PLAYER(ePlayer).getNumUnits() > MAX_BARB_UNITS_FOR_SPAWNING)
			{
				break;
			}

			const CvArea* pArea = pPlot->area();
			const int iTotalAreaSize = pArea->getNumTiles();

			// Toffer - Bar spawn if player has the max unit density allowed in this area.
			//	Ignore small areas, max local density limit will be adequate for those.
			if (iTotalAreaSize > std::max(49, iMinAreaPlotsPerPlayerUnit))
			{
				const int iPlayerUnitsInArea = pArea->getUnitsPerPlayer(ePlayer);
				if (iPlayerUnitsInArea > 0 && iTotalAreaSize / iPlayerUnitsInArea <= iMinAreaPlotsPerPlayerUnit)
				{
					continue;
				}
			}
			//TB Spawn Modification Begin
			// Automatically adapts chances of spawn to normalize between units that have a huge amount of plots available to spawn in and those that have relatively few.
			// Keeping in mind higher is less chance.
			// Produces the % of plots valid in area, base 50 so 1% becomes 51% while 90% becomes 140%
			//const int iFrequencyAdjustment = 50 + pArea->getNumValidPlotsbySpawn(eSpawn) * 100 / iTotalAreaSize;

			int iLocalSpawnRate = (int)adjustedSpawnRate/* * iFrequencyAdjustment / 100*/;
			//TB Spawn Modification End

			//	Adjust spawn rate for area ownership percentage
			if (!pPlot->isWater() && pArea->getNumCities() == pArea->getCitiesPerPlayer(BARBARIAN_PLAYER) + pArea->getCitiesPerPlayer(NEANDERTHAL_PLAYER))
			{
				// No non-barb cities so dramatically reduce spawns since its essentially an unplayed-in area for now
				iLocalSpawnRate *= 10;
			}
			else // Adjust spawn rate upward by up to tripling the normal rate in proportion with how little wilderness is left in this area
			{
				iLocalSpawnRate *= 100;
				iLocalSpawnRate /= 100 + 200 * pArea->getNumOwnedTiles() / pArea->getNumTiles();
			}

			if (getSorenRandNum(std::max(1, iLocalSpawnRate), "Unit spawn") == 0)
			{
				const int iArea = pPlot->getArea();
				// Check area unit type density not exceeded if specified
				if (iMinAreaPlotsPerUnitType > 0
				&& areaPopulationMap[iArea].find(eUnit) != areaPopulationMap[iArea].end()
				&& areaPopulationMap[iArea][eUnit] >= iTotalAreaSize / iMinAreaPlotsPerUnitType)
				{
					continue; // Total area density exceeded
				}

				// Check local max density not already exceeded
				int localAreaSize = 0;
				int localUnitTypeCount = 0;
				int localPlayerUnitCount = 0;

				// 49 plot local area, radius = 3
				const int LocalRange = 3;
				const int TotalLocalArea = (LocalRange * 2 + 1) * (LocalRange * 2 + 1);

				foreach_(const CvPlot* pLoopPlot, pPlot->rect(LocalRange, LocalRange))
				{
					if (pLoopPlot->area() == pArea)
					{
						localAreaSize++;
						localUnitTypeCount += pLoopPlot->plotCount(PUF_isUnitType, eUnit);
						localPlayerUnitCount += pLoopPlot->plotCount(PUF_isPlayer, ePlayer);
					}
				}
				if (localUnitTypeCount == 0
				// Max 0.50 unit per plot in local area owned by spawn owner.
				|| localPlayerUnitCount * 100 / localAreaSize < 50
				// Max local density limit for this specific unit type.
				&& localUnitTypeCount * 100 / localAreaSize < iMaxLocalDensity * 100 / TotalLocalArea)
				{
					// Spawn a new unit
					CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);

					CvUnit* pUnit = GET_PLAYER(ePlayer).initUnit(eUnit, pPlot->getX(), pPlot->getY(), kUnit.getDefaultUnitAIType(), NO_DIRECTION, getSorenRandNum(10000, "AI Unit Birthmark"));
					if (pUnit == NULL)
					{
						FErrorMsg("pUnit is expected to be assigned a valid unit object");
						return;
					}

					if (bSizeMatters)
					{
						const int iCount = kUnit.getNumGroupSpawnUnitCombatTypes();
						if (iCount > 0)
						{
							std::vector<int> aRandomList;
							//generate random list
							for (int iIndex = 0; iIndex < iCount; iIndex++)
							{
								for (int iJ = 0; iJ < kUnit.getGroupSpawnUnitCombatType(iIndex).iChance; iJ++)
								{
									aRandomList.push_back(iIndex);
								}
							}
							//generate result from list
							const int iFinalIndex = aRandomList[getSorenRandNum(aRandomList.size(), "Spawn Group Random Roll")];
							const UnitCombatTypes eGroupVolume = kUnit.getGroupSpawnUnitCombatType(iFinalIndex).eUnitCombat;
							const CvWString szTitle = kUnit.getGroupSpawnUnitCombatType(iFinalIndex).m_szTitle;

							//remove old group volume unitcombat
							if (eGroupVolume != NO_UNITCOMBAT)
							{
								for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
								{
									const UnitCombatTypes eUnitCombat = static_cast<UnitCombatTypes>(iI);

									if (GC.getUnitCombatInfo(eUnitCombat).getGroupBase() != -10 && pUnit->isHasUnitCombat(eUnitCombat) && eUnitCombat != eGroupVolume)
									{
										CvUnit::normalizeUnitPromotions(
											pUnit,
											GC.getUnitCombatInfo(eGroupVolume).getGroupBase() - GC.getUnitCombatInfo(eUnitCombat).getGroupBase(),
											bind(Game::isGroupUpgradePromotion, pUnit, _2),
											bind(Game::isGroupDowngradePromotion, pUnit, _2)
										);
									}
								}
							}
							//adjust the name of the unit
							if (!szTitle.empty())
							{
								pUnit->setName(gDLL->getText(szTitle, pUnit->getNameKey()));
							}
						}
					}
					pUnit->finishMoves();
					spawnCount++;
					areaPopulationMap[iArea][eUnit]++;

					// Spawn unit group
					if (!bSizeMatters)
					{
						for (int k = 0; k < spawnInfo.getNumSpawnGroup(); k++)
						{
							kUnit = GC.getUnitInfo(spawnInfo.getSpawnGroup(k));

							pUnit = GET_PLAYER(ePlayer).initUnit(spawnInfo.getSpawnGroup(k), pPlot->getX(), pPlot->getY(), kUnit.getDefaultUnitAIType(), NO_DIRECTION, getSorenRandNum(10000, "AI Unit Birthmark"));
							FAssertMsg(pUnit != NULL, "pUnit is expected to be assigned a valid unit object");
							pUnit->finishMoves();
							spawnCount++;
							areaPopulationMap[iArea][spawnInfo.getSpawnGroup(k)]++;
						}
					}
				}
			}
		}
		logging::logMsg("C2C.log", "%d units spawned for %s\n", spawnCount, spawnInfo.getType());
	}
}

#ifdef GLOBAL_WARMING
void CvGame::doGlobalWarming()
{
	// Loop to look for environmentalism written by EmperorFool
	bool abTreeHugger[MAX_PC_PLAYERS];
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		abTreeHugger[iI] = false;
		const CvPlayer& player = GET_PLAYER((PlayerTypes) iI);
		if (player.isAlive())
		{
			for (int iJ = 0; iJ < GC.getNumCivicOptionInfos(); iJ++)
			{
				const CivicTypes eCivic = player.getCivics((CivicOptionTypes)iJ);

				if (eCivic != NO_CIVIC && GC.getCivicInfo(eCivic).getExtraHealth() != 0)
				{
					abTreeHugger[iI] = true;
					break;
				}
			}
		}
	}
	const int iNumPlots = GC.getMap().numPlots();

	const TerrainTypes eFrozenTerrain = (TerrainTypes)GC.getDefineINT("FROZEN_TERRAIN");
	const TerrainTypes eColdTerrain = (TerrainTypes)GC.getDefineINT("COLD_TERRAIN");
	const FeatureTypes eColdFeature = (FeatureTypes)GC.getDefineINT("COLD_FEATURE");
	const FeatureTypes eFalloutFeature = (FeatureTypes)GC.getDefineINT("NUKE_FEATURE");

	//Global Warming
	const int iUnhealthWeight = GC.getDefineINT("GLOBAL_WARMING_UNHEALTH_WEIGHT");
	const int iBonusWeight = GC.getDefineINT("GLOBAL_WARMING_BONUS_WEIGHT");
	int iGlobalWarmingValue = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		const CvPlayer& player = GET_PLAYER((PlayerTypes) iI);
		if (player.isAlive())
		{
			foreach_(const CvCity* pCity, player.cities())
			{
				iGlobalWarmingValue -=
				(
					pCity->totalBadBuildingHealth() * iUnhealthWeight +
					pCity->getBonusBadHealth() * iBonusWeight
				);
			}
		}
	}
	iGlobalWarmingValue /= iNumPlots;

	if (iGlobalWarmingValue > 0)
	{
		const TerrainTypes eMarshTerrain = (TerrainTypes)GC.getDefineINT("MARSH_TERRAIN");
		const TerrainTypes eTemperateTerrain = (TerrainTypes)GC.getDefineINT("TEMPERATE_TERRAIN");
		const TerrainTypes eDryTerrain = (TerrainTypes)GC.getDefineINT("DRY_TERRAIN");
		const TerrainTypes eBarrenTerrain = (TerrainTypes)GC.getDefineINT("BARREN_TERRAIN");
		const TerrainTypes eShallowsTerrain = (TerrainTypes)GC.getDefineINT("SHALLOW_WATER_TERRAIN");
		const FeatureTypes eTemperateFeature = (FeatureTypes)GC.getDefineINT("TEMPERATE_FEATURE");
		const FeatureTypes eWarmFeature = (FeatureTypes)GC.getDefineINT("WARM_FEATURE");

		const int iTreeHuggerDefenseBonus = GC.getDefineINT("TREEHUGGER_DEFENSE_BONUS");
		int iGlobalWarmingDefense = 0;
		for (int i = 0; i < iNumPlots; ++i)
		{
			const CvPlot* pPlot = GC.getMap().plotByIndex(i);
			if (pPlot->getFeatureType() != NO_FEATURE)
			{
				const int iFeatureWarmingDefense = GC.getFeatureInfo(pPlot->getFeatureType()).getWarmingDefense();
				if (iFeatureWarmingDefense > 0)
				{
					PlayerTypes eCulturalOwner = pPlot->getOwner();
					if (eCulturalOwner != NO_PLAYER)
					{
						if (abTreeHugger[eCulturalOwner])
						{
							iGlobalWarmingDefense += iFeatureWarmingDefense * iTreeHuggerDefenseBonus;
						}
						else iGlobalWarmingDefense += iFeatureWarmingDefense;
					}
					else iGlobalWarmingDefense += iFeatureWarmingDefense;
				}
			}
		}
		iGlobalWarmingDefense *= GC.getDefineINT("GLOBAL_WARMING_FOREST");
		iGlobalWarmingDefense /= std::max(1, GC.getMap().getLandPlots());

		int iGlobalWarmingProb =
		(
			100 * (GC.getDefineINT("GLOBAL_WARMING_PROB") - iGlobalWarmingDefense)
			/ GC.getGameSpeedInfo(getGameSpeedType()).getSpeedPercent()
		);

		for (int iI = 0; iI < iGlobalWarmingValue; iI++)
		{
			if (getSorenRandNum(100, "Global Warming") >= iGlobalWarmingProb)
			{
				continue;
			}
			// Toffer - Multimap note - Need to make this whole thing specific to which map global warming is happening on.
			// For now we could restrict these effects to earth mapcategory terrains by adding a RANDPLOT_NOT_SPACE flag for syncRandPlot (ToDo).
			CvPlot* pPlot = GC.getMap().syncRandPlot(RANDPLOT_NOT_CITY); // GWMod removed check for water tile M.A.

			if (pPlot == NULL)
			{
				continue;
			}
			bool bChanged = false;

			if (pPlot->getFeatureType() != NO_FEATURE)
			{
				if (pPlot->getFeatureType() != eFalloutFeature)
				{
					// Feature change first
					if (pPlot->getFeatureType() == eColdFeature)
					{
						pPlot->setFeatureType(NO_FEATURE);
						bChanged = true;
					}
					else if (pPlot->calculateBestNatureYield(YIELD_FOOD, NO_TEAM) > 1 && pPlot->getFeatureType() == eTemperateFeature)
					{
						pPlot->setFeatureType(eWarmFeature);
						bChanged = true;
					}
					else if (pPlot->getTerrainType() == eColdTerrain)
					{
						pPlot->setTerrainType(eTemperateTerrain);
						bChanged = true;
					}
					else if (pPlot->getTerrainType() == eFrozenTerrain)
					{
						pPlot->setTerrainType(eColdTerrain);
						bChanged = true;
					}
					else if (pPlot->getTerrainType() == eMarshTerrain)
					{
						pPlot->setTerrainType(eTemperateTerrain);
						bChanged = true;
					}
					else
					{
						pPlot->setFeatureType(NO_FEATURE);
						bChanged = true;
					}
				}
			}
			else if (!pPlot->isWater())
			{
				if (pPlot->getTerrainType() == eBarrenTerrain)
				{
					if (GC.getDefineINT("GW_MOD_ENABLED") && pPlot->isCoastalLand()
					&& !pPlot->isHills() && !pPlot->isAsPeak())
					{
						pPlot->setTerrainType(eShallowsTerrain);
						bChanged = true;
					}
				}
				else if (pPlot->getTerrainType() == eDryTerrain)
				{
					pPlot->setTerrainType(eBarrenTerrain);
					bChanged = true;
				}
				else if (pPlot->getTerrainType() == eTemperateTerrain)
				{
					pPlot->setTerrainType(eDryTerrain);
					bChanged = true;
				}
				else if (pPlot->getTerrainType() == eColdTerrain)
				{
					pPlot->setTerrainType(eTemperateTerrain);
					bChanged = true;
				}
				else if (pPlot->getTerrainType() == eMarshTerrain)
				{
					pPlot->setTerrainType(eTemperateTerrain);
					bChanged = true;
				}
				else if (pPlot->getTerrainType() == eFrozenTerrain)
				{
					pPlot->setTerrainType(eColdTerrain);
					bChanged = true;
				}
			}
			if (bChanged)
			{
				pPlot->setImprovementType(NO_IMPROVEMENT);
				const CvCity* pCity = GC.getMap().findCity(pPlot->getX(), pPlot->getY());

				if (pCity != NULL && pPlot->isVisible(pCity->getTeam(), false))
				{
					AddDLLMessage(
						pCity->getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_GLOBAL_WARMING_NEAR_CITY", pCity->getNameKey()),
						"AS2D_GLOBALWARMING", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_RED(),
						pPlot->getX(), pPlot->getY(), true, true
					);
				}
			}
		}
	}
	//Nuclear Winter
	const int iNuclearWinterValue = getNukesExploded() * GC.getDefineINT("GLOBAL_WARMING_NUKE_WEIGHT") / 100;
	if (iNuclearWinterValue > 0)
	{
		const int iNuclearWinterProb = GC.getDefineINT("NUCLEAR_WINTER_PROB") * 100 / GC.getGameSpeedInfo(getGameSpeedType()).getSpeedPercent();

		for (int iI = 0; iI < iNuclearWinterValue; iI++)
		{
			if (getSorenRandNum(100, "Nuclear Fallout") >= iNuclearWinterProb)
			{
				continue;
			}
			// Toffer - Multimap note - Need to make this whole thing specific to which map the nukes were used on.
			// For now we could restrict these effects to earth mapcategory terrains by adding a RANDPLOT_NOT_SPACE flag for syncRandPlot (ToDo).
			CvPlot* pPlot = GC.getMap().syncRandPlot(RANDPLOT_LAND | RANDPLOT_NOT_CITY);

			if (pPlot == NULL)
			{
				continue;
			}
			bool bChanged = false;
			if (pPlot->getFeatureType() == NO_FEATURE)
			{
				pPlot->setFeatureType(eFalloutFeature);
				bChanged = true;
			}
			else if (pPlot->getFeatureType() != eFalloutFeature && pPlot->getFeatureType() != eColdFeature)
			{
				pPlot->setFeatureType(NO_FEATURE);
				bChanged = true;
			}

			if (getSorenRandNum(100, "Nuclear Winter") < iNuclearWinterProb)
			{
				if (pPlot->getTerrainType() == eColdTerrain)
				{
					pPlot->setTerrainType(eFrozenTerrain);
					bChanged = true;
				}
				if (pPlot->calculateTotalBestNatureYield(NO_TEAM) > 1)
				{
					pPlot->setTerrainType(eColdTerrain);
					bChanged = true;
				}
			}
			if (bChanged)
			{
				pPlot->setImprovementType(NO_IMPROVEMENT);
				const CvCity* pCity = GC.getMap().findCity(pPlot->getX(), pPlot->getY());

				if (pCity != NULL && pPlot->isVisible(pCity->getTeam(), false))
				{
					AddDLLMessage(
						pCity->getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_NUCLEAR_WINTER_NEAR_CITY", pCity->getNameKey()),
						"AS2D_GLOBALWARMING", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_RED(),
						pPlot->getX(), pPlot->getY(), true, true
					);
				}
			}
		}
	}
}
#endif // GLOBAL_WARMING


void CvGame::doHeadquarters()
{
	if (Cy::call_optional(PYGameModule, "doHeadquarters"))
	{
		return;
	}

	if (getElapsedGameTurns() < 5)
	{
		return;
	}

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		const CvCorporationInfo& kCorporation = GC.getCorporationInfo((CorporationTypes)iI);
		if (!isCorporationFounded((CorporationTypes)iI))
		{
			const TechTypes eTechPrereq = kCorporation.getTechPrereq();
			if (NO_TECH == eTechPrereq)
			{
				continue;
			}
			int iBestValue = MAX_INT;
			TeamTypes eBestTeam = NO_TEAM;

			for (int iJ = 0; iJ < MAX_PC_TEAMS; iJ++)
			{
				const CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iJ);

				if(kLoopTeam.isAlive()
				&& kLoopTeam.isHasTech(eTechPrereq)
				&& kLoopTeam.getNumCities() > 0)
				{
					foreach_(const BonusTypes eBonus, kCorporation.getPrereqBonuses())
					{
						if (kLoopTeam.hasBonus(eBonus))
						{
							int iValue = getSorenRandNum(10, "Found Corporation (Team)");

							for (int iK = 0; iK < GC.getNumCorporationInfos(); iK++)
							{
								iValue += GET_PLAYER((PlayerTypes)iJ).getHasCorporationCount((CorporationTypes)iK) * 20;
							}

							if (iValue < iBestValue)
							{
								iBestValue = iValue;
								eBestTeam = (TeamTypes) iJ;
							}
							break;
						}
					}
				}
			}

			if (eBestTeam != NO_TEAM)
			{
				iBestValue = MAX_INT;
				PlayerTypes eBestPlayer = NO_PLAYER;

				for (int iJ = 0; iJ < MAX_PLAYERS; iJ++)
				{
					const CvPlayer& playerX = GET_PLAYER((PlayerTypes)iJ);

					if (playerX.isAliveAndTeam(eBestTeam) && playerX.getNumCities() > 0)
					{
						foreach_(const BonusTypes eBonus, kCorporation.getPrereqBonuses())
						{
							if (playerX.hasBonus(eBonus))
							{
								int iValue = getSorenRandNum(25, "Found Corporation (Player)") - playerX.getNumAvailableBonuses(eBonus);

								for (int iK = 0; iK < GC.getNumCorporationInfos(); iK++)
								{
									iValue += GET_PLAYER((PlayerTypes)iJ).getHasCorporationCount((CorporationTypes)iK) * 20;
								}

								if (iValue < iBestValue)
								{
									iBestValue = iValue;
									eBestPlayer = (PlayerTypes) iJ;
								}
								break;
							}
						}
					}
				}
				if (eBestPlayer != NO_PLAYER)
				{
					GET_PLAYER(eBestPlayer).foundCorporation((CorporationTypes)iI);
				}
			}
		}
	}
}


void CvGame::doDiploVote()
{
	doVoteResults();
	doVoteSelection();
}


void CvGame::createBarbarianCities(bool bNeanderthal)
{
	if (
		getMaxCityElimination() > 0
	||
		isOption(GAMEOPTION_NO_BARBARIANS)
	||
		GC.getHandicapInfo(getHandicapType()).getUnownedTilesPerBarbarianCity() <= 0
	|| (
			bNeanderthal
			?
			(int)getCurrentEra() > 0
			:
			GC.getEraInfo(getCurrentEra()).isNoBarbCities()
		)
	) return;

	const int iCivCities = getNumCivCities();
	if (iCivCities < 1) return;

	const int iBarbPercent = GC.getGameSpeedInfo(getGameSpeedType()).getSpeedPercent();
	const bool bBarbWorld = isOption(GAMEOPTION_BARBARIAN_WORLD);

	// No barb city spawn the first X turns (difficulty and gamespeed decide X), unless it's a barbarian world.
	if (!bBarbWorld && getElapsedGameTurns() < GC.getHandicapInfo(getHandicapType()).getBarbarianCityCreationTurnsElapsed() * iBarbPercent / 100)
	{
		return;
	}

	const PlayerTypes eBarb = bNeanderthal ? NEANDERTHAL_PLAYER : BARBARIAN_PLAYER;
	CvPlayer& pBarb = GET_PLAYER(eBarb);

	const int iBarbCities = pBarb.getNumCities();

	// Hard limit on barb/civ city ratio.
	const int iMax = iCivCities / 2 + (countCivPlayersAlive() + GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()) / 2;

	if (bBarbWorld ? iMax + 4 < iBarbCities * 2 : iMax < iBarbCities * 7)
	{
		return;
	}
	// Random roll scaled by gamespeed
	int iRand = getSorenRandNum(iBarbPercent * 10, "Barb City Creation");

	// Probability reduction when relatively many barb cities exist
	if (iBarbCities >= iCivCities/3)
	{
		iRand *= 4;
	}
	else if (iBarbCities >= iCivCities/4)
	{
		iRand *= 3;
	}
	else if (iBarbCities >= iCivCities/5)
	{
		iRand *= 2;
	}
	else if (iBarbCities >= iCivCities/6)
	{
		iRand *= 3;
		iRand /= 2;
	}
	// Odds based on handicap
	if (iRand >= 10 * GC.getHandicapInfo(getHandicapType()).getBarbarianCityCreationProb())
	{
		return;
	}

	int iTargetCitiesMultiplier = 100;
	{
		const int iTargetBarbCities = 5*iCivCities * GC.getHandicapInfo(getHandicapType()).getBarbarianCityCreationProb() / 100;
		if (iBarbCities < iTargetBarbCities)
		{
			iTargetCitiesMultiplier += 300 * (iTargetBarbCities - iBarbCities) / iTargetBarbCities;
		}
	}

	const bool bBarbCiv = isOption(GAMEOPTION_BARBARIAN_CIV);
/* jdog5000 - BarbarianCiv
	Reduces emphasis on creating barb cities in large occupied areas.
*/
	// New variable for emphaizing spawning cities on populated continents
	int iOccupiedAreaMultiplier = 50;

	if (bBarbCiv)
	{
		int iOwnedPlots = 0;
		for (int iI = 0; iI < GC.getMAX_PLAYERS(); iI++)
		{
			iOwnedPlots += GET_PLAYER((PlayerTypes)iI).getTotalLand();
		}

		// When map mostly open, emphasize areas with other civs
		iOccupiedAreaMultiplier += 100 - 100 * iOwnedPlots / GC.getMap().getLandPlots();
	}
// ! jdog5000

	if (isOption(GAMEOPTION_RAGING_BARBARIANS))
	{
		iTargetCitiesMultiplier *= 3;
		iTargetCitiesMultiplier /= 2;

		// If raging barbs is on, emphasize areas with other civs
		iOccupiedAreaMultiplier *= 3;
		iOccupiedAreaMultiplier /= 2;
	}

	// Find plot
	const MapCategoryTypes earth = GC.getMAPCATEGORY_EARTH();
	const int iUnownedTilesThreshold = GC.getHandicapInfo(getHandicapType()).getUnownedTilesPerBarbarianCity();

	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot->isMapCategoryType(earth) && !pLoopPlot->isWater() && !pLoopPlot->isVisibleToCivTeam())
		{
			int iTargetCities = pLoopPlot->area()->getNumUnownedTiles();

			if (pLoopPlot->area()->getNumCities() == pLoopPlot->area()->getCitiesPerPlayer(eBarb))
			{
				iTargetCities *= 3;
			}

			if (pLoopPlot->area()->getNumTiles() < iUnownedTilesThreshold / 3)
			{
				iTargetCities *= iTargetCitiesMultiplier;
				iTargetCities /= 100;
			}

			iTargetCities /= std::max(1, iUnownedTilesThreshold);

			if (pLoopPlot->area()->getCitiesPerPlayer(eBarb) < iTargetCities)
			{
				int iValue = pBarb.AI_foundValue(pLoopPlot->getX(), pLoopPlot->getY(), GC.getMIN_BARBARIAN_CITY_STARTING_DISTANCE());
				iValue *= 100 + getSorenRandNum(50, "Variance");
				iValue /= 100;

				if (bBarbCiv)
				{
					if (pLoopPlot->area()->getNumCities() == pLoopPlot->area()->getCitiesPerPlayer(eBarb))
					{
						// Counteracts the AI_foundValue emphasis on empty areas
						iValue *= 2;
						iValue /= 3;
					}

					if (iTargetCitiesMultiplier > 100) // Either raging barbs is set or fewer barb cities than desired
					{
						// Emphasis on placing barb cities in populated areas
						iValue += iOccupiedAreaMultiplier * (pLoopPlot->area()->getNumCities() - pLoopPlot->area()->getCitiesPerPlayer(eBarb)) / iCivCities;
					}
				}
				else if (iTargetCitiesMultiplier > 100)
				{
					iValue *= pLoopPlot->area()->getNumOwnedTiles();
				}

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestPlot = pLoopPlot;
				}
			}
		}
	}
	if (pBestPlot != NULL)
	{
		pBarb.found(pBestPlot->getX(), pBestPlot->getY());
	}
}

namespace {
	bool isValidBarbarianSpawnUnit(const CvArea* area, const CvUnitInfo& unitInfo, const UnitTypes unitType)
	{
		return unitInfo.getCombat() > 0 && !unitInfo.isOnlyDefensive()
			// Make sure its the correct unit type for the area type (land or water)
			&& (area->isWater() && unitInfo.getDomainType() == DOMAIN_SEA || !area->isWater() && unitInfo.getDomainType() == DOMAIN_LAND)
			// Barbs don't need the bonus, but the bonus must be enabled by tech
			&& GET_TEAM(BARBARIAN_TEAM).isUnitBonusEnabledByTech(unitInfo, true) // "true" to invalidate units that require cultural bonuses
			&& !unitInfo.isCivilizationUnit() // Invalidates Neanderthal units and units that require the palace.
			// General requirements that must be fulfilled, also invalidates World Units for NPC
			&& GET_PLAYER(BARBARIAN_PLAYER).canTrain(unitType, false, false, false, true);
	}

	bool barbarianCityShouldSpawnWorker(CvGame* game, CvCity* city)
	{
		return (city->getPopulation() > 1 || game->getGameTurn() - city->getGameTurnAcquired() > 10 * GC.getGameSpeedInfo(game->getGameSpeedType()).getHammerCostPercent() / 100)
			&& city->AI_getWorkersHave() == 0
			&& city->AI_getWorkersNeeded() > 0
			&& (7 * city->getPopulation()) > game->getSorenRandNum(100, "Barb - workers");
	}

	int countPlayerShipsInArea(CvArea* area)
	{
		int iPlayerSeaUnits = 0;
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			const CvPlayerAI& playerX = GET_PLAYER((PlayerTypes)iI);
			if (playerX.isAlive())
			{
				iPlayerSeaUnits += playerX.AI_totalWaterAreaUnitAIs(area, UNITAI_ATTACK_SEA);
				iPlayerSeaUnits += playerX.AI_totalWaterAreaUnitAIs(area, UNITAI_EXPLORE_SEA);
				iPlayerSeaUnits += playerX.AI_totalWaterAreaUnitAIs(area, UNITAI_ASSAULT_SEA);
				iPlayerSeaUnits += playerX.AI_totalWaterAreaUnitAIs(area, UNITAI_SETTLER_SEA);
			}
		}
		return iPlayerSeaUnits;
	}

	int getNeededBarbsInArea(CvGame* game, CvArea* area)
	{
		// Spawn barbarian ships only
		if (!area->isWater())
			return 0;

		int iDivisor = GC.getHandicapInfo(game->getHandicapType()).getUnownedWaterTilesPerBarbarianUnit();

		if (game->isOption(GAMEOPTION_RAGING_BARBARIANS))
		{
			iDivisor = std::max(1, (iDivisor / 2));
		}

		if (iDivisor <= 0)
			return 0;

		// XXX eventually need to measure how many barbs of eBarbUnitAI we have in this area...
		int iNeededBarbs = ((area->getNumUnownedTiles() / iDivisor) - area->getUnitsPerPlayer(BARBARIAN_PLAYER));

		if (iNeededBarbs <= 0)
			return 0;

		iNeededBarbs = ((iNeededBarbs / 4) + 1);

		// Limit construction of barb ships based on player navies
		// Keeps barb ship count in check in early game since generation is greatly increased for BTS 3.17
		const int iPlayerSeaUnits = countPlayerShipsInArea(area);

		if (area->getUnitsPerPlayer(BARBARIAN_PLAYER) > (iPlayerSeaUnits / 3 + 1))
		{
			iNeededBarbs = 0;
		}
		return iNeededBarbs;
	}
}

void CvGame::createBarbarianUnits()
{
	if (isOption(GAMEOPTION_NO_BARBARIANS) || GC.getEraInfo(getCurrentEra()).isNoBarbUnits())
	{
		return;
	}

	foreach_(CvArea * pLoopArea, GC.getMap().areas())
	{
		const UnitAITypes eBarbUnitAI = pLoopArea->isWater()? UNITAI_ATTACK_SEA : UNITAI_ATTACK;

		const int iNeededBarbs = getNeededBarbsInArea(this, pLoopArea);

		// Spawn barbarians
		for (int iI = 0; iI < iNeededBarbs; iI++)
		{
			const CvPlot* pPlot = GC.getMap().syncRandPlot((RANDPLOT_NOT_VISIBLE_TO_CIV | RANDPLOT_ADJACENT_LAND | RANDPLOT_PASSIBLE), pLoopArea->getID(), GC.getMIN_BARBARIAN_STARTING_DISTANCE());

			if (pPlot == NULL)
				continue;

			UnitTypes eBestUnit = NO_UNIT;
			int iBestValue = 0;

			for (int iJ = 0; iJ < GC.getNumUnitInfos(); iJ++)
			{
				const CvUnitInfo& kUnit = GC.getUnitInfo((UnitTypes) iJ);

				if (isValidBarbarianSpawnUnit(pLoopArea, kUnit, (UnitTypes) iJ))
				{
					int iValue = 500 + getSorenRandNum(500, "Barb Unit Selection");

					if (kUnit.getUnitAIType(eBarbUnitAI))
					{
						iValue += 200;
					}
					iValue += kUnit.getCargoSpace() * (25 + getSorenRandNum(25, "Cargo Space Value"));

					if (iValue > iBestValue)
					{
						eBestUnit = (UnitTypes) iJ;
						iBestValue = iValue;
					}
				}
			}

			if (eBestUnit != NO_UNIT)
			{
				CvUnit* pUnit = GET_PLAYER(BARBARIAN_PLAYER).initUnit(eBestUnit, pPlot->getX(), pPlot->getY(), eBarbUnitAI, NO_DIRECTION, getSorenRandNum(10000, "AI Unit Birthmark"));
				if (GC.getUnitInfo(eBestUnit).getDomainType() == DOMAIN_SEA && pUnit != NULL)
				{
					loadPirateShip(pUnit);
				}
			}
		}

		// Give barb cities in occupied areas free workers so that if the city settles it has some infrastructure
		if (isOption(GAMEOPTION_BARBARIAN_CIV))
		{
			const int iBarbCities = pLoopArea->getCitiesPerPlayer(BARBARIAN_PLAYER);

			// There are barbarian AND non-barbarian cities in the area
			if(iBarbCities > 0 && pLoopArea->getNumCities() - iBarbCities > 0)
			{
				foreach_(CvCity* pLoopCity, GET_PLAYER(BARBARIAN_PLAYER).cities())
				{
					if(pLoopCity->area() == pLoopArea
						&& pLoopCity->getOriginalOwner() == BARBARIAN_PLAYER
						&& barbarianCityShouldSpawnWorker(this, pLoopCity))
					{
						int iUnitValue;

						UnitTypes eBestUnit = pLoopCity->AI_bestUnitAI(UNITAI_WORKER, iUnitValue);
						if (eBestUnit != NO_UNIT)
						{
							GET_PLAYER(BARBARIAN_PLAYER).initUnit(eBestUnit, pLoopCity->getX(), pLoopCity->getY(), UNITAI_WORKER, NO_DIRECTION, getSorenRandNum(10000, "AI Unit Birthmark"));
							pLoopCity->AI_setChooseProductionDirty(true);
						}
					}
				}
			}
		}
	}
/* TB Note: Commented out as a test for now to see if this would be preferred as I suspect.
	//Kill Stranded Units
	for (pLoopUnit = GET_PLAYER(BARBARIAN_PLAYER).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(BARBARIAN_PLAYER).nextUnit(&iLoop))
	{
		if (pLoopUnit->getGroup()->isStranded())
		{
			pLoopUnit->kill(false);
			break;
		}
	}
	//Kill off extra ships
	int iRand = 10000;
	int iBarbSeaUnits = 0;
	foreach_(CvArea* pLoopArea, GC.getMap().areas())
	{
		if (pLoopArea->isWater())
		{
			iBarbSeaUnits += GET_PLAYER(BARBARIAN_PLAYER).AI_totalWaterAreaUnitAIs(pLoopArea,UNITAI_ATTACK_SEA);
			iBarbSeaUnits += GET_PLAYER(BARBARIAN_PLAYER).AI_totalWaterAreaUnitAIs(pLoopArea,UNITAI_EXPLORE_SEA);
			iBarbSeaUnits += GET_PLAYER(BARBARIAN_PLAYER).AI_totalWaterAreaUnitAIs(pLoopArea,UNITAI_ASSAULT_SEA);
			iBarbSeaUnits += GET_PLAYER(BARBARIAN_PLAYER).AI_totalWaterAreaUnitAIs(pLoopArea,UNITAI_SETTLER_SEA);
		}
	}
	iRand /= std::max(iBarbSeaUnits, 1);
	if (iBarbSeaUnits > 0)
	{
		for (pLoopUnit = GET_PLAYER(BARBARIAN_PLAYER).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(BARBARIAN_PLAYER).nextUnit(&iLoop))
		{
			if (pLoopUnit->plot()->isCity() && pLoopUnit->getDomainType() == DOMAIN_SEA)
			{
				if (getSorenRandNum(iRand, "Remove Stranded Barbarian Ships") == 0)
				{
					pLoopUnit->kill(false);
				}
			}
		}
	}

	for (pLoopUnit = GET_PLAYER(BARBARIAN_PLAYER).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER(BARBARIAN_PLAYER).nextUnit(&iLoop))
	{
		if (pLoopUnit->isAnimal())
		{
			pLoopUnit->kill(false);
			break;
		}
	}
*/
}

void CvGame::updateMoves()
{
	PROFILE_FUNC();

	int aiShuffle[MAX_PLAYERS];

	if (!isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
	{
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			aiShuffle[iI] = iI;
		}
	}
	else shuffleArray(aiShuffle, MAX_PLAYERS, getSorenRand());

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& player = GET_PLAYER((PlayerTypes)(aiShuffle[iI]));

		if (player.isAlive() && player.isTurnActive())
		{
			// For the human we want auto-moves last so the player can change
			// orders on their automated units before hitting enter, but for the AI
			// it is desirable to start with automated units in case the automation is broken
			// be events, so that we know this before all other units process
			if (player.isHuman())
			{
				PROFILE("CvGame::updateMoves.Human");

				if (player.isAutoMoves())
				{
					if (player.hasReadyUnautomatedUnit(false))
					{
						// Pre-emptorary turn end so make sure we run the unit AIs now
						player.AI_unitUpdate();
					}
					algo::for_each(player.groups(), CvSelectionGroup::fn::autoMission());

					if (!player.hasBusyUnit())
					{
						player.setAutoMoves(false);
					}
				}
				else
				{
					if (!player.hasReadyUnautomatedUnit(false))
					{
						player.AI_unitUpdate();
					}
					// A unit ready to move at this point is one the player needs to interact with
					if (player.hasReadyUnautomatedUnit(true))
					{
						player.setTurnHadUIInteraction(true);
					}
				}
			}
			else
			{
				PROFILE("CvGame::updateMoves.AI");

				// Always try to do automations first for the AI
				algo::for_each(player.groups(), CvSelectionGroup::fn::autoMission());

				if (!player.isAutoMoves())
				{
					player.AI_unitUpdate();

					if (!player.hasBusyUnit() && !player.hasReadyUnit(true))
					{
						player.setAutoMoves(true);
					}
				}
				else if (!player.hasBusyUnit())
				{
					player.setAutoMoves(false);
				}
			}
		}
	}
}


void CvGame::verifyCivics()
{
	PROFILE_FUNC();

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			GET_PLAYER((PlayerTypes)iI).verifyCivics();
		}
	}
}


void CvGame::updateTimers()
{
	PROFILE_FUNC();

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			GET_PLAYER((PlayerTypes)iI).updateTimers();
		}
	}
}


void CvGame::updateTurnTimer()
{
	PROFILE_FUNC();

	// Are we using a turn timer?
	if (isMPOption(MPOPTION_TURN_TIMER))
	{
		if (getElapsedGameTurns() > 0 || !isOption(GAMEOPTION_ADVANCED_START))
		{
			// Has the turn expired?
			if (getTurnSlice() > getCutoffSlice())
			{
				for (int iI = 0; iI < MAX_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isTurnActive())
					{
						GET_PLAYER((PlayerTypes)iI).setEndTurn(true);

						if (!isMPOption(MPOPTION_SIMULTANEOUS_TURNS) && !isSimultaneousTeamTurns())
						{
							break;
						}
					}
				}
			}
		}
	}
}


void CvGame::testAlive()
{
	PROFILE_FUNC();

	if (m_eCurrentMap != MAP_EARTH)	// XXX - Currently players are killed after switching to a new map.
		return;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		GET_PLAYER((PlayerTypes)iI).verifyAlive();
	}
}


bool CvGame::testVictory(VictoryTypes eVictory, TeamTypes eTeam, bool* pbEndScore)
{
	PROFILE_FUNC();

	FASSERT_BOUNDS(0, GC.getNumVictoryInfos(), eVictory);
	FASSERT_BOUNDS(0, MAX_PC_TEAMS, eTeam);
	FAssert(GET_TEAM(eTeam).isAlive());

	if (pbEndScore)
	{
		*pbEndScore = false;
	}
	if (!isVictoryValid(eVictory))
	{
		return false;
	}

	bool bValid = true;
	// Time Victory
	if (bValid && GC.getVictoryInfo(eVictory).isEndScore())
	{
		if (pbEndScore)
		{
			*pbEndScore = true;
		}

		if (getMaxTurns() == 0)
		{
			bValid = false;
		}
		else if (getElapsedGameTurns() >= getMaxTurns())
		{
			for (int iK = 0; iK < MAX_PC_TEAMS; iK++)
			{
				if (iK != eTeam && GET_TEAM((TeamTypes)iK).isAlive() && getTeamScore((TeamTypes)iK) >= getTeamScore(eTeam))
				{
					bValid = false;
					break;
				}
			}
		}
		else bValid = false;
	}
	// Score Victory
	if (bValid && GC.getVictoryInfo(eVictory).isTargetScore())
	{
		if (getTargetScore() == 0)
		{
			bValid = false;
		}
		else if (getTeamScore(eTeam) >= getTargetScore())
		{
			for (int iK = 0; iK < MAX_PC_TEAMS; iK++)
			{
				if (iK != eTeam && GET_TEAM((TeamTypes)iK).isAlive() && getTeamScore((TeamTypes)iK) >= getTeamScore(eTeam))
				{
					bValid = false;
					break;
				}
			}
		}
		else bValid = false;
	}
	// Conquest Victory
	if (bValid && GC.getVictoryInfo(eVictory).isConquest())
	{
		if (GET_TEAM(eTeam).getNumCities() != 0)
		{
			for (int iK = 0; iK < MAX_PC_TEAMS; iK++)
			{
				if (iK != eTeam && GET_TEAM((TeamTypes)iK).isAlive() && !GET_TEAM((TeamTypes)iK).isVassal(eTeam)
				&& GET_TEAM((TeamTypes)iK).getNumCities() > 0)
				{
					bValid = false;
					break;
				}
			}
		}
		else bValid = false;
	}
	// Diplomatic Victory
	if (bValid && GC.getVictoryInfo(eVictory).isDiploVote())
	{
		bValid = false;
		for (int iK = 0; iK < GC.getNumVoteInfos(); iK++)
		{
			if (GC.getVoteInfo((VoteTypes)iK).isVictory() && getVoteOutcome((VoteTypes)iK) == eTeam)
			{
				bValid = true;
				break;
			}
		}
	}
	// Domination Victory
	if (bValid && getAdjustedPopulationPercent(eVictory) > 0 && 100 * GET_TEAM(eTeam).getTotalPopulation() < getTotalPopulation() * getAdjustedPopulationPercent(eVictory))
	{
		bValid = false;
	}
	if (bValid && getAdjustedLandPercent(eVictory) > 0 && 100 * GET_TEAM(eTeam).getTotalLand() < GC.getMap().getLandPlots() * getAdjustedLandPercent(eVictory))
	{
		bValid = false;
	}
	// Religious Victory
	if (bValid && GC.getVictoryInfo(eVictory).getReligionPercent() > 0)
	{
		bValid = false;
		if (getNumCivCities() > (countCivPlayersAlive() * 2))
		{
			for (int iK = 0; iK < GC.getNumReligionInfos(); iK++)
			{
				if (GET_TEAM(eTeam).hasHolyCity((ReligionTypes)iK)
				&& calculateReligionPercent((ReligionTypes)iK) >= GC.getVictoryInfo(eVictory).getReligionPercent())
				{
					bValid = true;
					break;
				}
			}
		}
	}
	// Cultural Victory
	if (bValid && GC.getVictoryInfo(eVictory).getCityCulture() != NO_CULTURELEVEL && GC.getVictoryInfo(eVictory).getNumCultureCities() > 0)
	{
		int iCount = 0;
		for (int iK = 0; iK < MAX_PC_PLAYERS; iK++)
		{
			if (GET_PLAYER((PlayerTypes)iK).isAliveAndTeam(eTeam))
			{
				foreach_(const CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iK).cities())
				{
					if (pLoopCity->getCultureLevel() >= GC.getVictoryInfo(eVictory).getCityCulture())
					{
						iCount++;
					}
				}
			}
		}
		if (iCount < GC.getVictoryInfo(eVictory).getNumCultureCities())
		{
			bValid = false;
		}
	}
	// Not in use...
	if (bValid && GC.getVictoryInfo(eVictory).getTotalCultureRatio() > 0)
	{
		const uint64_t iThreshold = GET_TEAM(eTeam).countTotalCulture() * 100 / GC.getVictoryInfo(eVictory).getTotalCultureRatio();

		for (int iK = 0; iK < MAX_PC_TEAMS; iK++)
		{
			if (iK != eTeam && GET_TEAM((TeamTypes)iK).isAlive() && GET_TEAM((TeamTypes)iK).countTotalCulture() > iThreshold)
			{
				bValid = false;
				break;
			}
		}
	}
	// Scientific Victory
	if (bValid)
	{
		for (int iK = 0; iK < GC.getNumBuildingInfos(); iK++)
		{
			if (GC.getBuildingInfo((BuildingTypes)iK).getVictoryThreshold(eVictory) > GET_TEAM(eTeam).getBuildingCount((BuildingTypes)iK))
			{
				bValid = false;
				break;
			}
		}
	}
	// Space Race Victory
	if (bValid && !starshipLaunched[eTeam])
	{
		for (int iK = 0; iK < GC.getNumProjectInfos(); iK++)
		{
			if (GC.getProjectInfo((ProjectTypes) iK).getVictoryMinThreshold(eVictory) > GET_TEAM(eTeam).getProjectCount((ProjectTypes)iK))
			{
				bValid = false;
				break;
			}
		}
		if (bValid)
		{
			starshipLaunched[eTeam] = true;
		}
	}
	return bValid;
}

void CvGame::testVictory()
{
	PROFILE_FUNC();

	if (getVictory() != NO_VICTORY || getGameState() == GAMESTATE_EXTENDED
	|| getElapsedGameTurns() <= GC.getGameSpeedInfo(getGameSpeedType()).getSpeedPercent() / 10)
	{
		return;
	}
	// Make sure score is up to date.
	updateScore();

	// Do victory test
	bool bEndScore = false;

	std::vector<std::vector<int> > aaiWinners;

	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iI);
		if (kLoopTeam.isAlive() && !kLoopTeam.isMinorCiv())
		{
			for (int iJ = 0; iJ < GC.getNumVictoryInfos(); iJ++)
			{
				if (testVictory((VictoryTypes)iJ, (TeamTypes)iI, &bEndScore))
				{
					if (kLoopTeam.getVictoryCountdown((VictoryTypes)iJ) < 0
					&&  kLoopTeam.getVictoryDelay((VictoryTypes)iJ) == 0)
					{
						kLoopTeam.setVictoryCountdown((VictoryTypes)iJ, 0);
					}

					//update victory countdown
					if (kLoopTeam.getVictoryCountdown((VictoryTypes)iJ) > 0)
					{
						kLoopTeam.changeVictoryCountdown((VictoryTypes)iJ, -1);
					}

					if (kLoopTeam.getVictoryCountdown((VictoryTypes)iJ) == 0)
					{
						if (getSorenRandNum(100, "Victory Success") < kLoopTeam.getLaunchSuccessRate((VictoryTypes)iJ))
						{
							std::vector<int> aWinner;
							aWinner.push_back(iI);
							aWinner.push_back(iJ);
							aaiWinners.push_back(aWinner);
						}
						else kLoopTeam.resetVictoryProgress();
					}
				}
			}
		}
	}

	bool bForceEndGame = false;
	for (int iJ = 0; iJ < GC.getNumVictoryInfos(); iJ++)
	{
		if (!GC.getVictoryInfo((VictoryTypes)iJ).isTotalVictory() || !isVictoryValid((VictoryTypes)iJ))
		{
			continue;
		}
		aaiWinners.clear(); // sorry, folks, no winners today by usual means, only Mastery Victory is achievable;)
		if (getMaxTurns() == 0)
		{
			break;
		}//There's more game yet to play.
		if (getElapsedGameTurns() < getMaxTurns())
		{
			if (isModderGameOption(MODDERGAMEOPTION_MERCY_RULE))
			{
				int64_t iTotalScore = 0;
				int64_t iTopScore = 0;

				TeamTypes eBestTeam = (TeamTypes)0;
				for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
				{
					if (GET_TEAM((TeamTypes)iI).isAlive()
					&& (!GET_TEAM((TeamTypes)iI).isMinorCiv() || isOption(GAMEOPTION_START_AS_MINORS)))
					{
						const int64_t iTempScore = GET_TEAM((TeamTypes)iI).getTotalVictoryScore();
						iTotalScore += iTempScore;
						if (iTempScore > iTopScore)
						{
							iTopScore = iTempScore;
							eBestTeam = (TeamTypes)iI;
						}
					}
				}
				//Remove the top player
				iTotalScore -= iTopScore;
				//One player is dominating! Mercy Rule!
				if (iTopScore > iTotalScore)
				{
					if (getMercyRuleCounter() == 0)
					{
						// Ten Turns remain! (on normal gamespeed)
						setMercyRuleCounter(GC.getGameSpeedInfo(getGameSpeedType()).getSpeedPercent() / 10);
						// Inform Players
						for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
						{
							if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHuman())
							{
								if (GET_PLAYER((PlayerTypes)iI).getTeam() == eBestTeam)
								{
									AddDLLMessage(
										(PlayerTypes)iI, true, GC.getEVENT_MESSAGE_TIME(),
										gDLL->getText("TXT_KEY_WINNER_MERCY_RULE_HAS_BEGUN", getMercyRuleCounter()).GetCString(),
										"AS2D_FEAT_ACCOMPLISHED", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_GREEN()
									);
								}
								else
								{
									AddDLLMessage(
										(PlayerTypes)iI, true, GC.getEVENT_MESSAGE_TIME(),
										gDLL->getText("TXT_KEY_LOSER_MERCY_RULE_HAS_BEGUN", getMercyRuleCounter()).GetCString(),
										"AS2D_FIRSTTOTECH", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_RED()
									);
								}
							}
						}
					}
					else
					{
						changeMercyRuleCounter(-1);
						//Times Up!
						if (getMercyRuleCounter() != 0)
						{
							for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
							{
								if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHuman())
								{

									if (GET_PLAYER((PlayerTypes)iI).getTeam() == eBestTeam)
									{
										AddDLLMessage(
											(PlayerTypes)iI, true, GC.getEVENT_MESSAGE_TIME(),
											gDLL->getText("TXT_KEY_WINNER_MERCY_RULE_TURNS", getMercyRuleCounter()).GetCString(),
											"AS2D_FEAT_ACCOMPLISHED", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_GREEN()
										);
									}
									else
									{
										AddDLLMessage(
											(PlayerTypes)iI, true, GC.getEVENT_MESSAGE_TIME(),
											gDLL->getText("TXT_KEY_LOSER_MERCY_RULE_TURNS", getMercyRuleCounter()).GetCString(),
											"AS2D_FIRSTTOTECH", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_WARNING_TEXT()
										);
									}
								}
							}
						}
						else bForceEndGame = true;
					}
				}
				//Abort any existing mercy countdown
				else if (getMercyRuleCounter() > 0)
				{
					setMercyRuleCounter(0);
					for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHuman())
						{
							AddDLLMessage(
								(PlayerTypes)iI, true, GC.getEVENT_MESSAGE_TIME(),
								gDLL->getText("TXT_KEY_MERCY_RULE_ABORTED").GetCString(),
								"AS2D_FEAT_ACCOMPLISHED", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_GREEN()
							);
						}
					}
				}
			}
			else break;
		}
		if (getElapsedGameTurns() >= getMaxTurns() || bForceEndGame)
		{
			int64_t topScore = 0;
			// End of game and Total Victory is selected.  Calculate the topscore
			for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
			{
				if (GET_TEAM((TeamTypes)iI).isAlive() && !GET_TEAM((TeamTypes)iI).isMinorCiv())
				{
					const int64_t score1 = GET_TEAM((TeamTypes)iI).getTotalVictoryScore();
					if (score1 > topScore)
					{
						topScore = score1;
					}
				}
			}
			for (int iI = 0; iI < MAX_PC_TEAMS; iI++) // have to do again because of ties, determine winners
			{
				if (GET_TEAM((TeamTypes)iI).isAlive() && !GET_TEAM((TeamTypes)iI).isMinorCiv())
				{
					const int64_t score2 = GET_TEAM((TeamTypes)iI).getTotalVictoryScore();
					if (score2 >= topScore)
					{
						std::vector<int> aWinner;
						aWinner.push_back(iI);
						aWinner.push_back(iJ);
						aaiWinners.push_back(aWinner);
					}
				}
			}
		}
	}

	if (isOption(GAMEOPTION_UNITED_NATIONS) && !m_bDiploVictoryEnabled)
	{
		//Find the diplomatic victory
		VictoryTypes eVictoryUN = NO_VICTORY;

		for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			const int iV = GC.getBuildingInfo((BuildingTypes)iI).getVictoryPrereq();
			if (iV != NO_VICTORY && GC.getBuildingInfo((BuildingTypes)iI).getVoteSourceType() != NO_VOTESOURCE)
			{
				eVictoryUN = (VictoryTypes) iV;
				break;
			}
		}
		if (eVictoryUN != NO_VICTORY && getVictory() == eVictoryUN)
		{
			aaiWinners.clear();
		}
	}

	if (!aaiWinners.empty())
	{
		const int iWinner = getSorenRandNum(aaiWinners.size(), "Victory tie breaker");
		setWinner((TeamTypes)aaiWinners[iWinner][0], (VictoryTypes)aaiWinners[iWinner][1]);
	}

	if (getVictory() == NO_VICTORY && getMaxTurns() > 0 && getElapsedGameTurns() >= getMaxTurns() && !bEndScore)
	{
		if (getAIAutoPlay(getActivePlayer()) > 0 || gDLL->GetAutorun())
		{
			setGameState(GAMESTATE_EXTENDED);
		}
		else setGameState(GAMESTATE_OVER);
	}
}


void CvGame::processVote(const VoteTriggeredData& kData, int iChange)
{
	PROFILE_FUNC();

	const CvVoteInfo& kVote = GC.getVoteInfo(kData.kVoteOption.eVote);

	changeTradeRoutes(kVote.getTradeRoutes() * iChange);
	changeFreeTradeCount(kVote.isFreeTrade() ? iChange : 0);
	changeNoNukesCount(kVote.isNoNukes() ? iChange : 0);

	for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		changeForceCivicCount((CivicTypes)iI, kVote.isForceCivic(iI) ? iChange : 0);
	}

	if (iChange <= 0)
	{
		return;
	}
	if (kVote.isOpenBorders())
	{
		for (int i1 = 0; i1 < MAX_PC_PLAYERS; ++i1)
		{
			CvTeam& team1 = GET_TEAM((TeamTypes)i1);
			if (team1.isVotingMember(kData.eVoteSource))
			{
				for (int i2 = i1 + 1; i2 < MAX_PC_PLAYERS; ++i2)
				{
					if (GET_TEAM((TeamTypes)i2).isVotingMember(kData.eVoteSource))
					{
						team1.signOpenBorders((TeamTypes)i2);
					}
				}
			}
		}
		setVoteOutcome(kData, NO_PLAYER_VOTE);
	}
	else if (kVote.isDefensivePact())
	{
		for (int i1 = 0; i1 < MAX_PC_PLAYERS; ++i1)
		{
			CvTeam& team1 = GET_TEAM((TeamTypes)i1);
			if (team1.isVotingMember(kData.eVoteSource))
			{
				for (int i2 = i1 + 1; i2 < MAX_PC_PLAYERS; ++i2)
				{
					if (GET_TEAM((TeamTypes)i2).isVotingMember(kData.eVoteSource))
					{
						team1.signDefensivePact((TeamTypes)i2);
					}
				}
			}
		}
		setVoteOutcome(kData, NO_PLAYER_VOTE);
	}
	else if (kVote.isForcePeace())
	{
		FAssert(NO_PLAYER != kData.kVoteOption.ePlayer);
		CvPlayer& kPlayer = GET_PLAYER(kData.kVoteOption.ePlayer);
		const TeamTypes eTeam = kPlayer.getTeam();

		if (gTeamLogLevel >= 1)
		{
			logBBAI("  Vote for forcing peace against team %d (%S) passes", eTeam, kPlayer.getCivilizationDescription(0) );
		}

		for (int iI = 0; iI < MAX_PC_PLAYERS; ++iI)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(eTeam)
			&&  GET_PLAYER((PlayerTypes)iI).isVotingMember(kData.eVoteSource))
			{
				GET_PLAYER((PlayerTypes)iI).forcePeace(kData.kVoteOption.ePlayer);
			}
		}
		setVoteOutcome(kData, NO_PLAYER_VOTE);
	}
	else if (kVote.isForceNoTrade())
	{
		FAssert(NO_PLAYER != kData.kVoteOption.ePlayer);
		CvPlayer& kPlayer = GET_PLAYER(kData.kVoteOption.ePlayer);

		for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; ++iPlayer)
		{
			CvPlayer& playerX = GET_PLAYER((PlayerTypes)iPlayer);
			if (playerX.isAlive()
			&&  playerX.isVotingMember(kData.eVoteSource)
			&&  playerX.canStopTradingWithTeam(kPlayer.getTeam()))
			{
				playerX.stopTradingWithTeam(kPlayer.getTeam());
			}
		}

		setVoteOutcome(kData, NO_PLAYER_VOTE);
	}
	else if (kVote.isForceWar())
	{
		FAssert(NO_PLAYER != kData.kVoteOption.ePlayer);
		CvPlayer& kPlayer = GET_PLAYER(kData.kVoteOption.ePlayer);

		if (gTeamLogLevel >= 1)
		{
			logBBAI("  Vote for war against team %d (%S) passes", kPlayer.getTeam(), kPlayer.getCivilizationDescription(0) );
		}

		for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; ++iPlayer)
		{
			CvPlayer& playerX = GET_PLAYER((PlayerTypes)iPlayer);
			if (playerX.isAlive() && playerX.isVotingMember(kData.eVoteSource)
			&& GET_TEAM(playerX.getTeam()).canChangeWarPeace(kPlayer.getTeam()))
			{
				GET_TEAM(playerX.getTeam()).declareWar(kPlayer.getTeam(), false, WARPLAN_DOGPILE);
			}
		}

		setVoteOutcome(kData, NO_PLAYER_VOTE);
	}
	else if (kVote.isAssignCity())
	{
		FAssert(NO_PLAYER != kData.kVoteOption.ePlayer);
		CvCity* pCity = GET_PLAYER(kData.kVoteOption.ePlayer).getCity(kData.kVoteOption.iCityId);
		FAssert(NULL != pCity);

		if (NULL != pCity && NO_PLAYER != kData.kVoteOption.eOtherPlayer && kData.kVoteOption.eOtherPlayer != pCity->getOwner())
		{
			if (gTeamLogLevel >= 1)
			{
				logBBAI(
					"  Vote for assigning %S to %d (%S) passes",
					pCity->getName().GetCString(),
					GET_PLAYER(kData.kVoteOption.eOtherPlayer).getTeam(),
					GET_PLAYER(kData.kVoteOption.eOtherPlayer).getCivilizationDescription(0)
				);
			}
			GET_PLAYER(kData.kVoteOption.eOtherPlayer).acquireCity(pCity, false, true, true);
		}
		setVoteOutcome(kData, NO_PLAYER_VOTE);
	}
}


int CvGame::getIndexAfterLastDeal()
{
	return m_deals.getIndexAfterLast();
}


int CvGame::getNumDeals() const
{
	return m_deals.getCount();
}


CvDeal* CvGame::getDeal(int iID)
{
	return ((CvDeal *)(m_deals.getAt(iID)));
}


CvDeal* CvGame::addDeal()
{
	return ((CvDeal *)(m_deals.add()));
}


void CvGame::deleteDeal(int iID)
{
	m_deals.removeAt(iID);
	gDLL->getInterfaceIFace()->setDirty(Foreign_Screen_DIRTY_BIT, true);
}

CvDeal* CvGame::firstDeal(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_deals.beginIter(pIterIdx) : m_deals.endIter(pIterIdx);
}


CvDeal* CvGame::nextDeal(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_deals.nextIter(pIterIdx) : m_deals.prevIter(pIterIdx);
}


CvRandom& CvGame::getMapRand()
{
	return m_mapRand;
}


int CvGame::getMapRandNum(int iNum, const char* pszLog)
{
	return m_mapRand.get(iNum, pszLog);
}


CvRandom& CvGame::getSorenRand()
{
	return m_sorenRand;
}

void CvGame::logRandomResult(const wchar_t* szStreamName, const char* pszLog, int iMax, int iResult)
{
	if (GC.isXMLLogging() || isNetworkMultiPlayer())
	{
		if (isFinalInitialized())
		{
			static int iLine = 0;
			logging::logMsg(
				(bst::format("RandomLogger - Player %d - Set %d.log") % getActivePlayer() % (getGameTurn()/50)).str().c_str(),
				"%d\t%d\t%S\t%s\t%d\t%d\n", ++iLine, getGameTurn()+1, szStreamName, pszLog, iMax, iResult
			);
		}
	}
}

int CvGame::getSorenRandNum(int iNum, const char* pszLog)
{
	int iScale = 0;
	while(iNum > MAX_UNSIGNED_SHORT)
	{
		iNum /= 2;
		iScale++;
	}

	int Result = m_sorenRand.get(iNum, pszLog);

	while(iScale-- > 0)
	{
		Result *= 2;
	}

	logRandomResult(L"Global", pszLog, iNum, Result);

	return Result;
}


int CvGame::calculateSyncChecksum()
{
	// Toffer - 15.08.20
	// No point in calculating the network sync hash value when not in a network game.
	if (!isNetworkMultiPlayer())
	{
		return 0;
	}
	// Added lots of integer overflow protection using '% MAX_INT',
	// should be quite water tight now, when 64 bit integers overflowed easily here before.
	// ! Toffer

	PROFILE_FUNC();

	int64_t iValue = (getMapRand().getSeed() * getSorenRand().getSeed()) % MAX_INT;

	iValue += getNumCities();
	iValue += getTotalPopulation();
	iValue += getNumDeals();

	iValue += GC.getMap().getOwnedPlots();
	iValue += GC.getMap().getNumAreas();

	int64_t iFinalMult = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isEverAlive())
		{
			int64_t iMultiplier = getPlayerScore((PlayerTypes)iI);

			switch (getTurnSlice() % 4)
			{
			case 0:
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getTotalPopulation() * 543);
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getTotalLand() * 327);
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getGold() % MAX_INT);
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getAssets());
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getPower());
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getNumCities() * 436);
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getNumUnits());
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getNumSelectionGroups());
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getHandicapType() * 271);
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getStateReligion() * 382);
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getCulture());
				break;

			case 1:
				for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).calculateTotalYield((YieldTypes)iJ));
				}

				for (int iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
				{
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).getCommerceRate((CommerceTypes)iJ));
				}

				foreach_(const CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
				{
					for (int iJ = 0; iJ < GC.getNumEventInfos(); iJ++)
					{
						iMultiplier += (iJ + 1) * pLoopCity->isEventOccured((EventTypes)iJ);
					}
				}

				foreach_(const CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
				{
					iMultiplier += pLoopCity->getX();
					iMultiplier += pLoopCity->getY();
					iMultiplier += pLoopCity->getPopulation();
					iMultiplier += pLoopCity->getNumBuildings();
					iMultiplier += pLoopCity->countNumImprovedPlots();
					iMultiplier += pLoopCity->getWorkingPopulation();
					iMultiplier += pLoopCity->getSpecialistPopulation();
					iMultiplier += pLoopCity->getNumGreatPeople();
					iMultiplier += pLoopCity->goodHealth();
					iMultiplier += pLoopCity->badHealth();
					iMultiplier += pLoopCity->happyLevel();
					iMultiplier += pLoopCity->unhappyLevel();
					iMultiplier += pLoopCity->getFood();
				}
				break;

			case 2:
				for (int iJ = 0; iJ < GC.getNumBonusInfos(); iJ++)
				{
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).getNumAvailableBonuses((BonusTypes)iJ) * 945);
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).getBonusImport((BonusTypes)iJ) * 326);
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).getBonusExport((BonusTypes)iJ) * 932);
				}

				for (int iJ = 0; iJ < GC.getNumImprovementInfos(); iJ++)
				{
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).getImprovementCount((ImprovementTypes)iJ) * 883);
				}

				for (int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
				{
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).getBuildingCountPlusMaking((BuildingTypes)iJ) * 95);
				}

				for (int iJ = 0; iJ < GC.getNumUnitInfos(); iJ++)
				{
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).getUnitCountPlusMaking((UnitTypes)iJ) * 75);
				}

				for (int iJ = 0; iJ < NUM_UNITAI_TYPES; iJ++)
				{
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).AI_totalUnitAIs((UnitAITypes)iJ) * 64);
				}

				foreach_(const CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
				{
					for (int iJ = 0; iJ < GC.getNumReligionInfos(); iJ++)
					{
						if (pLoopCity->isHasReligion((ReligionTypes)iJ))
							iMultiplier += pLoopCity->getID() * (iJ + 1);
					}
					for (int iJ = 0; iJ < GC.getNumCorporationInfos(); iJ++)
					{
						if (pLoopCity->isHasCorporation((CorporationTypes)iJ))
							iMultiplier += (pLoopCity->getID() + 1) * (iJ + 1);
					}
				}
				break;

			case 3:
				foreach_(CvUnit* pLoopUnit, GET_PLAYER((PlayerTypes)iI).units())
				{
					iMultiplier += (pLoopUnit->getX() * 87);
					iMultiplier += (pLoopUnit->getY() * 98);
					iMultiplier += (pLoopUnit->getDamage() * 73);
					iMultiplier += (pLoopUnit->getExperience() * 82);
					iMultiplier += (pLoopUnit->getLevel() * 36);
					for (PromotionIterator it = pLoopUnit->getPromotionBegin(), itEnd = pLoopUnit->getPromotionEnd(); it != itEnd; ++it)
					{
						if (it->second.m_bHasPromotion)
						{
							iMultiplier += ((int)it->first) * 57;
						}
					}
					for (std::map<UnitCombatTypes, UnitCombatKeyedInfo>::const_iterator it = pLoopUnit->getUnitCombatKeyedInfo().begin(), end = pLoopUnit->getUnitCombatKeyedInfo().end(); it != end; ++it)
					{
						if (it->second.m_bHasUnitCombat)
						{
							iMultiplier += ((int)it->first) * 13;
						}
					}
				}
				break;
			}

			iFinalMult += iMultiplier % MAX_INT;
		}
	}
	iFinalMult %= MAX_INT;

	if (iFinalMult != 0)
	{
		iValue *= iFinalMult;
	}

	return static_cast<int>(iValue % MAX_INT);
}


int CvGame::calculateOptionsChecksum()
{
	PROFILE_FUNC();

	int iValue = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		for (int iJ = 0; iJ < NUM_PLAYEROPTION_TYPES; iJ++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isOption((PlayerOptionTypes)iJ))
			{
				iValue += (iI * 943097);
				iValue += (iJ * 281541);
			}
		}
	}
	return iValue;
}


void CvGame::addReplayMessage(ReplayMessageTypes eType, PlayerTypes ePlayer, CvWString pszText, int iPlotX, int iPlotY, ColorTypes eColor)
{
	CvReplayMessage* pMessage = new CvReplayMessage(getGameTurn(), eType, ePlayer);
	if (NULL != pMessage)
	{
		pMessage->setPlot(iPlotX, iPlotY);
		pMessage->setText(pszText);
		if (NO_COLOR == eColor)
		{
			eColor = GC.getCOLOR_WHITE();
		}
		pMessage->setColor(eColor);
		m_listReplayMessages.push_back(pMessage);
	}
}

void CvGame::clearReplayMessageMap()
{
	foreach_(const CvReplayMessage* pMessage, m_listReplayMessages)
	{
		if (NULL != pMessage)
		{
			delete pMessage;
		}
	}
	m_listReplayMessages.clear();
}

int CvGame::getReplayMessageTurn(uint i) const
{
	if (i >= m_listReplayMessages.size())
	{
		return -1;
	}
	const CvReplayMessage* pMessage =  m_listReplayMessages[i];

	return pMessage != NULL ? pMessage->getTurn() : -1;
}

ReplayMessageTypes CvGame::getReplayMessageType(uint i) const
{
	if (i >= m_listReplayMessages.size())
	{
		return (NO_REPLAY_MESSAGE);
	}
	const CvReplayMessage* pMessage =  m_listReplayMessages[i];
	if (NULL == pMessage)
	{
		return (NO_REPLAY_MESSAGE);
	}
	return pMessage->getType();
}

int CvGame::getReplayMessagePlotX(uint i) const
{
	if (i >= m_listReplayMessages.size())
	{
		return (-1);
	}
	const CvReplayMessage* pMessage =  m_listReplayMessages[i];
	if (NULL == pMessage)
	{
		return (-1);
	}
	return pMessage->getPlotX();
}

int CvGame::getReplayMessagePlotY(uint i) const
{
	if (i >= m_listReplayMessages.size())
	{
		return (-1);
	}
	const CvReplayMessage* pMessage =  m_listReplayMessages[i];
	if (NULL == pMessage)
	{
		return (-1);
	}
	return pMessage->getPlotY();
}

PlayerTypes CvGame::getReplayMessagePlayer(uint i) const
{
	if (i >= m_listReplayMessages.size())
	{
		return (NO_PLAYER);
	}
	const CvReplayMessage* pMessage =  m_listReplayMessages[i];
	if (NULL == pMessage)
	{
		return (NO_PLAYER);
	}
	return pMessage->getPlayer();
}

LPCWSTR CvGame::getReplayMessageText(uint i) const
{
	if (i >= m_listReplayMessages.size())
	{
		return (NULL);
	}
	const CvReplayMessage* pMessage =  m_listReplayMessages[i];
	if (NULL == pMessage)
	{
		return (NULL);
	}
	return pMessage->getText().GetCString();
}

ColorTypes CvGame::getReplayMessageColor(uint i) const
{
	if (i >= m_listReplayMessages.size())
	{
		return (NO_COLOR);
	}
	const CvReplayMessage* pMessage =  m_listReplayMessages[i];
	if (NULL == pMessage)
	{
		return (NO_COLOR);
	}
	return pMessage->getColor();
}


uint CvGame::getNumReplayMessages() const
{
	return m_listReplayMessages.size();
}

// Private Functions...

void CvGame::read(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	reset(NO_HANDICAP);

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	WRAPPER_READ_STRING(wrapper, "CvGame", m_gameId);	// flags for expansion

	uint uiFlag=0;
	WRAPPER_READ(wrapper,"CvGame",&uiFlag);	// flags for expansion

	if ((uiFlag & GAME_SAVE_UI_FLAG_VALUE_AND_BASE) == 0)
	{
		int iEndTurnMessagesSent;
		WRAPPER_READ(wrapper,"CvGame",&iEndTurnMessagesSent);
	}
	WRAPPER_READ(wrapper,"CvGame",&m_iElapsedGameTurns);
	WRAPPER_READ(wrapper,"CvGame",&m_iStartTurn);
	WRAPPER_READ(wrapper,"CvGame",&m_iStartYear);
	WRAPPER_READ(wrapper,"CvGame",&m_iEstimateEndTurn);
	WRAPPER_READ(wrapper,"CvGame",&m_iTurnSlice);
	WRAPPER_READ(wrapper,"CvGame",&m_iCutoffSlice);
	WRAPPER_READ(wrapper,"CvGame",&m_iNumGameTurnActive);
	WRAPPER_READ(wrapper,"CvGame",&m_iNumCities);
	WRAPPER_READ(wrapper,"CvGame",&m_iTotalPopulation);
	WRAPPER_READ(wrapper,"CvGame",&m_iTradeRoutes);
	WRAPPER_READ(wrapper,"CvGame",&m_iFreeTradeCount);
	WRAPPER_READ(wrapper,"CvGame",&m_iNoNukesCount);
	WRAPPER_READ(wrapper,"CvGame",&m_iNukesExploded);
	WRAPPER_READ(wrapper,"CvGame",&m_iMaxPopulation);
	WRAPPER_READ(wrapper,"CvGame",&m_iMaxLand);
	WRAPPER_READ(wrapper,"CvGame",&m_iMaxTech);
	WRAPPER_READ(wrapper,"CvGame",&m_iMaxWonders);
	WRAPPER_READ(wrapper,"CvGame",&m_iInitPopulation);
	WRAPPER_READ(wrapper,"CvGame",&m_iInitLand);
	WRAPPER_READ(wrapper,"CvGame",&m_iInitTech);
	WRAPPER_READ(wrapper,"CvGame",&m_iInitWonders);
	WRAPPER_READ(wrapper,"CvGame",&m_iLastNukeStrikeX);
	WRAPPER_READ(wrapper,"CvGame",&m_iLastNukeStrikeY);
	WRAPPER_READ(wrapper,"CvGame",&m_iCurrentVoteID);
	// @SAVEBREAK DELETE
	WRAPPER_SKIP_ELEMENT(wrapper,"CvGame", m_iWaterAnimalSpawnChance, SAVE_VALUE_ANY);
	// SAVEBREAK@
	WRAPPER_READ(wrapper,"CvGame",&m_iCutLosersCounter);
	WRAPPER_READ(wrapper,"CvGame",&m_iHighToLowCounter);
	WRAPPER_READ(wrapper,"CvGame",&m_iIncreasingDifficultyCounter);
	WRAPPER_READ(wrapper,"CvGame",&m_iMercyRuleCounter);
	WRAPPER_READ(wrapper,"CvGame",&m_bDiploVictoryEnabled);
	WRAPPER_READ_ARRAY(wrapper,"CvGame",MAX_PLAYERS, m_aiFlexibleDifficultyTimer);

	//	Super forts adaptation to C2C - has this game state had its choke points calculated?
	m_iChokePointCalculationVersion = 0;	//	Ensure old format loads get their values calculated later
	WRAPPER_READ(wrapper,"CvGame",&m_iChokePointCalculationVersion);

	calculateRiverBuildings();
	calculateCoastalBuildings();
	calculateNumWonders();

	// m_uiInitialTime not saved

	WRAPPER_READ(wrapper,"CvGame",&m_bScoreDirty);
	WRAPPER_READ(wrapper,"CvGame",(int*)&m_circumnavigatingTeam);
	// m_bDebugMode not saved
	WRAPPER_READ(wrapper,"CvGame",&m_bFinalInitialized);
	// m_bPbemTurnSent not saved
	WRAPPER_READ(wrapper,"CvGame",&m_bHotPbemBetweenTurns);
	// m_bPlayerOptionsSent not saved

	WRAPPER_READ(wrapper,"CvGame",(int*)&m_eHandicap);
	WRAPPER_READ(wrapper,"CvGame",(int*)&m_ePausePlayer);

	m_eBestLandUnit = NO_UNIT;
	WRAPPER_READ_CLASS_ENUM_ALLOW_MISSING(wrapper,"CvGame",REMAPPED_CLASS_TYPE_UNITS,(int*)&m_eBestLandUnit);

	WRAPPER_READ(wrapper,"CvGame",(int*)&m_eWinner);
	WRAPPER_READ(wrapper,"CvGame",(int*)&m_eVictory);
	WRAPPER_READ(wrapper,"CvGame",(int*)&m_eGameState);

	WRAPPER_READ_STRING(wrapper,"CvGame",m_szScriptData);

	if ((uiFlag & GAME_SAVE_UI_FLAG_VALUE_AND_BASE) == 0)
	{
		std::vector<int> aiEndTurnMessagesReceived(MAX_PLAYERS);
		WRAPPER_READ_ARRAY(wrapper,"CvGame",MAX_PLAYERS, &aiEndTurnMessagesReceived[0]);
	}
	WRAPPER_READ_ARRAY(wrapper,"CvGame",MAX_PLAYERS, m_aiRankPlayer);
	WRAPPER_READ_ARRAY(wrapper,"CvGame",MAX_PLAYERS, m_aiPlayerRank);
	WRAPPER_READ_ARRAY(wrapper,"CvGame",MAX_PLAYERS, m_aiPlayerScore);
	WRAPPER_READ_ARRAY(wrapper,"CvGame",MAX_TEAMS, starshipLaunched);
	WRAPPER_READ_ARRAY(wrapper,"CvGame",MAX_TEAMS, diplomaticVictoryAchieved);
	WRAPPER_READ_ARRAY(wrapper,"CvGame",MAX_TEAMS, m_aiRankTeam);
	WRAPPER_READ_ARRAY(wrapper,"CvGame",MAX_TEAMS, m_aiTeamRank);
	WRAPPER_READ_ARRAY(wrapper,"CvGame",MAX_TEAMS, m_aiTeamScore);
	WRAPPER_READ_ARRAY(wrapper,"CvGame",MAX_PLAYERS, m_iAIAutoPlay);
	WRAPPER_READ_ARRAY(wrapper,"CvGame",MAX_PLAYERS, m_iForcedAIAutoPlay);

	WRAPPER_READ_CLASS_ARRAY(wrapper,"CvGame", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_paiUnitCreatedCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper,"CvGame", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiBuildingCreatedCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper,"CvGame", REMAPPED_CLASS_TYPE_PROJECTS, GC.getNumProjectInfos(), m_paiProjectCreatedCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper,"CvGame", REMAPPED_CLASS_TYPE_CIVICS, GC.getNumCivicInfos(), m_paiForceCivicCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper,"CvGame", REMAPPED_CLASS_TYPE_VOTES, GC.getNumVoteInfos(), (int*)m_paiVoteOutcome);
	WRAPPER_READ_CLASS_ARRAY(wrapper,"CvGame", REMAPPED_CLASS_TYPE_RELIGIONS, GC.getNumReligionInfos(), m_paiReligionGameTurnFounded);
	WRAPPER_READ_CLASS_ARRAY(wrapper,"CvGame", REMAPPED_CLASS_TYPE_CORPORATIONS, GC.getNumCorporationInfos(), m_paiCorporationGameTurnFounded);
	WRAPPER_READ_CLASS_ARRAY(wrapper,"CvGame", REMAPPED_CLASS_TYPE_VOTE_SOURCES, GC.getNumVoteSourceInfos(), m_aiSecretaryGeneralTimer);
	WRAPPER_READ_CLASS_ARRAY(wrapper,"CvGame", REMAPPED_CLASS_TYPE_VOTE_SOURCES, GC.getNumVoteSourceInfos(), m_aiVoteTimer);
	WRAPPER_READ_CLASS_ARRAY(wrapper,"CvGame", REMAPPED_CLASS_TYPE_VOTE_SOURCES, GC.getNumVoteSourceInfos(), m_aiDiploVote);

	WRAPPER_READ_CLASS_ARRAY(wrapper,"CvGame", REMAPPED_CLASS_TYPE_SPECIAL_UNITS, GC.getNumSpecialUnitInfos(), m_pabSpecialUnitValid);
	WRAPPER_READ_CLASS_ARRAY(wrapper,"CvGame", REMAPPED_CLASS_TYPE_SPECIAL_BUILDINGS, GC.getNumSpecialBuildingInfos(), m_pabSpecialBuildingValid);
	WRAPPER_READ_CLASS_ARRAY(wrapper,"CvGame", REMAPPED_CLASS_TYPE_RELIGIONS, GC.getNumReligionInfos(), m_abReligionSlotTaken);

	WRAPPER_READ(wrapper,"CvGame",&m_bGameStart);
	WRAPPER_READ_CLASS_ARRAY_ALLOW_MISSING(wrapper,"CvGame",REMAPPED_CLASS_TYPE_TECHS,GC.getNumTechInfos(), m_abTechCanFoundReligion);

	//	If ther latest asets define NEW religions we'll need to initialize their tech can-found flag
	if (wrapper.isUsingTaggedFormat())
	{
		for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
		{
			bool bReligionIsNew = true;

			for (int iJ = 0; iJ < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_RELIGIONS); iJ++)
			{
				if ( wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_RELIGIONS, iJ) == iI)
				{
					bReligionIsNew = false;
					break;
				}
			}

			if (bReligionIsNew)
			{
				const CvReligionInfo& newReligion = GC.getReligionInfo((ReligionTypes)iI);
				const TechTypes eFoundingTech = newReligion.getTechPrereq();

				setTechCanFoundReligion(eFoundingTech, false);
			}
		}
	}

	for (int iI = 0; iI < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_RELIGIONS); iI++)
	{
		int	eOwner;
		int	iID;
		int	iRemappedReligion = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_RELIGIONS, iI);

		WRAPPER_READ_DECORATED(wrapper,"CvGame",&eOwner, "m_paHolyCity[].eOwner");
		WRAPPER_READ_DECORATED(wrapper,"CvGame",&iID, "m_paHolyCity[].iID");

		if (iRemappedReligion != -1)
		{
			m_paHolyCity[iRemappedReligion].eOwner = (PlayerTypes)eOwner;
			m_paHolyCity[iRemappedReligion].iID = iID;
		}
	}

	for (int iI = 0; iI < wrapper.getNumClassEnumValues(REMAPPED_CLASS_TYPE_CORPORATIONS); iI++)
	{
		int eOwner;
		int	iID;
		int	iRemappedCorporation = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_CORPORATIONS, iI);

		WRAPPER_READ_DECORATED(wrapper,"CvGame",&eOwner,"m_paHeadquarters[].eOwner");
		WRAPPER_READ_DECORATED(wrapper,"CvGame",&iID,"m_paHeadquarters[].iID");

		if ( iRemappedCorporation != -1 )
		{
			m_paHeadquarters[iRemappedCorporation].eOwner = (PlayerTypes)eOwner;
			m_paHeadquarters[iRemappedCorporation].iID = iID;
		}
	}

	{
		CvWString szBuffer;
		uint iSize;

		m_aszDestroyedCities.clear();
		WRAPPER_READ_DECORATED(wrapper,"CvGame",&iSize,"DestroyedCityCount");
		for (uint i = 0; i < iSize; i++)
		{
			WRAPPER_READ_STRING_DECORATED(wrapper,"CvGame",szBuffer,"DestroyedCityName");
			m_aszDestroyedCities.push_back(szBuffer);
		}

		m_aszGreatPeopleBorn.clear();
		WRAPPER_READ_DECORATED(wrapper,"CvGame",&iSize,"GreatPeopleBornCount");
		for (uint i = 0; i < iSize; i++)
		{
			WRAPPER_READ_STRING_DECORATED(wrapper,"CvGame",szBuffer,"GreatPersonName");
			m_aszGreatPeopleBorn.push_back(szBuffer);
		}
	}

	ReadStreamableFFreeListTrashArray(m_deals, pStream);
	ReadStreamableFFreeListTrashArray(m_voteSelections, pStream);
	ReadStreamableFFreeListTrashArray(m_votesTriggered, pStream);

	m_mapRand.read(pStream);
	m_sorenRand.read(pStream);

	{
		clearReplayMessageMap();
		ReplayMessageList::_Alloc::size_type iSize;
		WRAPPER_READ(wrapper,"CvGame",&iSize);
		for (ReplayMessageList::_Alloc::size_type i = 0; i < iSize; i++)
		{
			CvReplayMessage* pMessage = new CvReplayMessage(0);
			if (NULL != pMessage)
			{
				pMessage->read(*pStream);
			}
			m_listReplayMessages.push_back(pMessage);
		}
	}
	// m_pReplayInfo not saved

	WRAPPER_READ(wrapper,"CvGame",&m_iNumSessions);
	if (!isNetworkMultiPlayer())
	{
		++m_iNumSessions;
	}

	{
		unsigned int iSize;
		m_aPlotExtraYields.clear();
		WRAPPER_READ_DECORATED(wrapper,"CvGame",&iSize,"PlotYieldCount");
		for (unsigned int i = 0; i < iSize; ++i)
		{
			PlotExtraYield kPlotYield;
			kPlotYield.read(pStream);
			m_aPlotExtraYields.push_back(kPlotYield);
		}
	}

	{
		unsigned int iSize;
		m_aPlotExtraCosts.clear();
		WRAPPER_READ_DECORATED(wrapper,"CvGame",&iSize,"PlotExtraCostsCount");
		for (unsigned int i = 0; i < iSize; ++i)
		{
			PlotExtraCost kPlotCost;
			kPlotCost.read(pStream);
			m_aPlotExtraCosts.push_back(kPlotCost);
		}
	}

	{
		unsigned int iSize;
		m_mapVoteSourceReligions.clear();
		WRAPPER_READ_DECORATED(wrapper,"CvGame",&iSize,"VoteSourceReligionsCount");
		for (unsigned int i = 0; i < iSize; ++i)
		{
			VoteSourceTypes eVoteSource;
			ReligionTypes eReligion;
			WRAPPER_READ_CLASS_ENUM_DECORATED(wrapper,"CvGame",REMAPPED_CLASS_TYPE_VOTE_SOURCES,(int*)&eVoteSource, "VoteSourceReligionMapSource");
			WRAPPER_READ_CLASS_ENUM_DECORATED(wrapper,"CvGame",REMAPPED_CLASS_TYPE_RELIGIONS,(int*)&eReligion, "VoteSourceReligionMapReligion");
			m_mapVoteSourceReligions[eVoteSource] = eReligion;
		}
	}

	{
		unsigned int iSize;
		m_aeInactiveTriggers.clear();
		WRAPPER_READ_DECORATED(wrapper,"CvGame",&iSize,"InactiveTriggersCount");
		for (unsigned int i = 0; i < iSize; ++i)
		{
			int iTrigger = -2;
			WRAPPER_READ_CLASS_ENUM_DECORATED_ALLOW_MISSING(wrapper,"CvGame",REMAPPED_CLASS_TYPE_EVENT_TRIGGERS,&iTrigger,"InactiveTrigger");

			if ( iTrigger == -2 && wrapper.isUsingTaggedFormat() )
			{
				//	Consume the value for older formats since the information cannot be
				//	believed anyway
				WRAPPER_READ_DECORATED(wrapper, "CvGame", &iTrigger, "InactiveTrigger");
			}
			else if (iTrigger != -1)
			{
				m_aeInactiveTriggers.push_back((EventTriggerTypes)iTrigger);
			}
		}
	}

	// Get the active player information from the initialization structure
	if (!isGameMultiPlayer())
	{
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isHuman())
			{
				setActivePlayer((PlayerTypes)iI);
				break;
			}
		}
		addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getActivePlayer(), gDLL->getText("TXT_KEY_MISC_RELOAD", m_iNumSessions));
	}

	if (isOption(GAMEOPTION_NEW_RANDOM_SEED) && !isNetworkMultiPlayer())
	{
		m_sorenRand.reseed(timeGetTime());
	}

	WRAPPER_READ(wrapper,"CvGame",&m_iShrineBuildingCount);
	WRAPPER_READ_CLASS_ENUM_ARRAY_ALLOW_MISSING(wrapper, "CvGame", REMAPPED_CLASS_TYPE_BUILDINGS, m_iShrineBuildingCount, m_aiShrineBuilding);
	WRAPPER_READ_CLASS_ENUM_ARRAY_ALLOW_MISSING(wrapper, "CvGame", REMAPPED_CLASS_TYPE_RELIGIONS, m_iShrineBuildingCount, m_aiShrineReligion);

	WRAPPER_READ(wrapper,"CvGame",&m_iNumCultureVictoryCities);
	WRAPPER_READ(wrapper,"CvGame",&m_eCultureVictoryCultureLevel);

	m_Properties.readWrapper(pStream);

	int iCurrentHandicap = range(getHandicapType(), 0, GC.getNumHandicapInfos() - 1);
	setHandicapType((HandicapTypes)iCurrentHandicap);

	m_plotGroupHashesInitialized = false;	//	Force calculation in first update timeslice

	//Example of how to skip element
	//WRAPPER_SKIP_ELEMENT(wrapper,"CvGame",m_bCircumnavigated, SAVE_VALUE_ANY);
	WRAPPER_READ_CLASS_ARRAY(wrapper,"CvGame", REMAPPED_CLASS_TYPE_IMPROVEMENTS, GC.getNumImprovementInfos(), m_paiImprovementCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper,"CvGame", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiTechGameTurnDiscovered);
	WRAPPER_READ_OBJECT_END(wrapper);

	//establish improvement costs
	//for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	//{
	//	GC.getImprovementInfo((ImprovementTypes)iI).setHighestCost();
	//}
}

#define TAGGED_SAVES 1

void CvGame::write(FDataStreamBase* pStream)
{
	int iI;
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	uint uiFlag = GAME_SAVE_UI_FLAG_VALUE_AND_BASE;

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE_STRING(wrapper, "CvGame", m_gameId);	// flags for expansion

	WRAPPER_WRITE(wrapper, "CvGame", uiFlag);		// flag for expansion

	WRAPPER_WRITE(wrapper, "CvGame", m_iElapsedGameTurns);
	WRAPPER_WRITE(wrapper, "CvGame", m_iStartTurn);
	WRAPPER_WRITE(wrapper, "CvGame", m_iStartYear);
	WRAPPER_WRITE(wrapper, "CvGame", m_iEstimateEndTurn);
	WRAPPER_WRITE(wrapper, "CvGame", m_iTurnSlice);
	WRAPPER_WRITE(wrapper, "CvGame", m_iCutoffSlice);
	WRAPPER_WRITE(wrapper, "CvGame", m_iNumGameTurnActive);
	WRAPPER_WRITE(wrapper, "CvGame", m_iNumCities);
	WRAPPER_WRITE(wrapper, "CvGame", m_iTotalPopulation);
	WRAPPER_WRITE(wrapper, "CvGame", m_iTradeRoutes);
	WRAPPER_WRITE(wrapper, "CvGame", m_iFreeTradeCount);
	WRAPPER_WRITE(wrapper, "CvGame", m_iNoNukesCount);
	WRAPPER_WRITE(wrapper, "CvGame", m_iNukesExploded);
	WRAPPER_WRITE(wrapper, "CvGame", m_iMaxPopulation);
	WRAPPER_WRITE(wrapper, "CvGame", m_iMaxLand);
	WRAPPER_WRITE(wrapper, "CvGame", m_iMaxTech);
	WRAPPER_WRITE(wrapper, "CvGame", m_iMaxWonders);
	WRAPPER_WRITE(wrapper, "CvGame", m_iInitPopulation);
	WRAPPER_WRITE(wrapper, "CvGame", m_iInitLand);
	WRAPPER_WRITE(wrapper, "CvGame", m_iInitTech);
	WRAPPER_WRITE(wrapper, "CvGame", m_iInitWonders);

	WRAPPER_WRITE(wrapper, "CvGame", m_iLastNukeStrikeX);
	WRAPPER_WRITE(wrapper, "CvGame", m_iLastNukeStrikeY);

	WRAPPER_WRITE(wrapper, "CvGame", m_iCurrentVoteID);
	WRAPPER_WRITE(wrapper, "CvGame", m_iCutLosersCounter);
	WRAPPER_WRITE(wrapper, "CvGame", m_iHighToLowCounter);
	WRAPPER_WRITE(wrapper, "CvGame", m_iIncreasingDifficultyCounter);
	WRAPPER_WRITE(wrapper, "CvGame", m_iMercyRuleCounter);
	WRAPPER_WRITE(wrapper, "CvGame", m_bDiploVictoryEnabled);
	WRAPPER_WRITE_ARRAY(wrapper, "CvGame", MAX_PLAYERS, m_aiFlexibleDifficultyTimer);

	//	Super forts adaptation to C2C - has this game state had its choke points calculated?
	WRAPPER_WRITE(wrapper,"CvGame", m_iChokePointCalculationVersion);

	// m_uiInitialTime not saved

	WRAPPER_WRITE(wrapper, "CvGame", m_bScoreDirty);
	WRAPPER_WRITE(wrapper, "CvGame", (int)m_circumnavigatingTeam);
	// m_bDebugMode not saved
	WRAPPER_WRITE(wrapper, "CvGame", m_bFinalInitialized);
	// m_bPbemTurnSent not saved
	WRAPPER_WRITE(wrapper, "CvGame", m_bHotPbemBetweenTurns);
	// m_bPlayerOptionsSent not saved
	// TB Nukefix
	// WRAPPER_WRITE(wrapper, "CvGame", m_bNukesValid);

	WRAPPER_WRITE(wrapper, "CvGame", m_eHandicap);
	WRAPPER_WRITE(wrapper, "CvGame", m_ePausePlayer);
	WRAPPER_WRITE_CLASS_ENUM(wrapper, "CvGame", REMAPPED_CLASS_TYPE_UNITS, m_eBestLandUnit);
	WRAPPER_WRITE(wrapper, "CvGame", m_eWinner);
	WRAPPER_WRITE(wrapper, "CvGame", m_eVictory);
	WRAPPER_WRITE(wrapper, "CvGame", m_eGameState);

	WRAPPER_WRITE_STRING(wrapper, "CvGame", m_szScriptData);

	WRAPPER_WRITE_ARRAY(wrapper, "CvGame", MAX_PLAYERS, m_aiRankPlayer);
	WRAPPER_WRITE_ARRAY(wrapper, "CvGame", MAX_PLAYERS, m_aiPlayerRank);
	WRAPPER_WRITE_ARRAY(wrapper, "CvGame", MAX_PLAYERS, m_aiPlayerScore);

	WRAPPER_WRITE_ARRAY(wrapper, "CvGame", MAX_TEAMS, starshipLaunched);
	WRAPPER_WRITE_ARRAY(wrapper, "CvGame", MAX_TEAMS, diplomaticVictoryAchieved);

	WRAPPER_WRITE_ARRAY(wrapper, "CvGame", MAX_TEAMS, m_aiRankTeam);
	WRAPPER_WRITE_ARRAY(wrapper, "CvGame", MAX_TEAMS, m_aiTeamRank);
	WRAPPER_WRITE_ARRAY(wrapper, "CvGame", MAX_TEAMS, m_aiTeamScore);

	WRAPPER_WRITE_ARRAY(wrapper, "CvGame", MAX_PLAYERS, m_iAIAutoPlay);
	WRAPPER_WRITE_ARRAY(wrapper, "CvGame", MAX_PLAYERS, m_iForcedAIAutoPlay);

	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvGame", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_paiUnitCreatedCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvGame", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_paiBuildingCreatedCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvGame", REMAPPED_CLASS_TYPE_PROJECTS, GC.getNumProjectInfos(), m_paiProjectCreatedCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvGame", REMAPPED_CLASS_TYPE_CIVICS, GC.getNumCivicInfos(), m_paiForceCivicCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvGame", REMAPPED_CLASS_TYPE_VOTES, GC.getNumVoteInfos(), (int*)m_paiVoteOutcome);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvGame", REMAPPED_CLASS_TYPE_RELIGIONS, GC.getNumReligionInfos(), m_paiReligionGameTurnFounded);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvGame", REMAPPED_CLASS_TYPE_CORPORATIONS, GC.getNumCorporationInfos(), m_paiCorporationGameTurnFounded);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvGame", REMAPPED_CLASS_TYPE_VOTE_SOURCES, GC.getNumVoteSourceInfos(), m_aiSecretaryGeneralTimer);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvGame", REMAPPED_CLASS_TYPE_VOTE_SOURCES, GC.getNumVoteSourceInfos(), m_aiVoteTimer);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvGame", REMAPPED_CLASS_TYPE_VOTE_SOURCES, GC.getNumVoteSourceInfos(), m_aiDiploVote);

	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvGame", REMAPPED_CLASS_TYPE_SPECIAL_UNITS, GC.getNumSpecialUnitInfos(), m_pabSpecialUnitValid);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvGame", REMAPPED_CLASS_TYPE_SPECIAL_BUILDINGS, GC.getNumSpecialBuildingInfos(), m_pabSpecialBuildingValid);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvGame", REMAPPED_CLASS_TYPE_RELIGIONS, GC.getNumReligionInfos(), m_abReligionSlotTaken);

	WRAPPER_WRITE(wrapper, "CvGame", m_bGameStart);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvGame", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_abTechCanFoundReligion);

	for (iI=0;iI<GC.getNumReligionInfos();iI++)
	{
		WRAPPER_WRITE(wrapper, "CvGame", m_paHolyCity[iI].eOwner);
		WRAPPER_WRITE(wrapper, "CvGame", m_paHolyCity[iI].iID);
	}

	for (iI=0;iI<GC.getNumCorporationInfos();iI++)
	{
		WRAPPER_WRITE(wrapper, "CvGame", m_paHeadquarters[iI].eOwner);
		WRAPPER_WRITE(wrapper, "CvGame", m_paHeadquarters[iI].iID);
	}

	{
		std::vector<CvWString>::iterator ws_it;

		WRAPPER_WRITE_DECORATED(wrapper, "CvGame", m_aszDestroyedCities.size(), "DestroyedCityCount");
		for (ws_it = m_aszDestroyedCities.begin(); ws_it != m_aszDestroyedCities.end(); ++ws_it)
		{
			WRAPPER_WRITE_STRING_DECORATED(wrapper, "CvGame", *ws_it, "DestroyedCityName");
		}

		WRAPPER_WRITE_DECORATED(wrapper, "CvGame", m_aszGreatPeopleBorn.size(), "GreatPeopleBornCount");
		for (ws_it = m_aszGreatPeopleBorn.begin(); ws_it != m_aszGreatPeopleBorn.end(); ++ws_it)
		{
			WRAPPER_WRITE_STRING_DECORATED(wrapper, "CvGame", *ws_it, "GreatPersonName");
		}
	}

	WriteStreamableFFreeListTrashArray(m_deals, pStream);
	WriteStreamableFFreeListTrashArray(m_voteSelections, pStream);
	WriteStreamableFFreeListTrashArray(m_votesTriggered, pStream);

	m_mapRand.write(pStream);
	m_sorenRand.write(pStream);

	ReplayMessageList::_Alloc::size_type iSize = m_listReplayMessages.size();
	WRAPPER_WRITE(wrapper, "CvGame", iSize);
	ReplayMessageList::const_iterator it;
	for (it = m_listReplayMessages.begin(); it != m_listReplayMessages.end(); ++it)
	{
		const CvReplayMessage* pMessage = *it;
		if (NULL != pMessage)
		{
			pMessage->write(*pStream);
		}
	}
	// m_pReplayInfo not saved

	WRAPPER_WRITE(wrapper, "CvGame", m_iNumSessions);

	WRAPPER_WRITE_DECORATED(wrapper, "CvGame", m_aPlotExtraYields.size(), "PlotYieldCount");
	for (std::vector<PlotExtraYield>::iterator it = m_aPlotExtraYields.begin(); it != m_aPlotExtraYields.end(); ++it)
	{
		(*it).write(pStream);
	}

	WRAPPER_WRITE_DECORATED(wrapper, "CvGame", m_aPlotExtraCosts.size(), "PlotExtraCostsCount");
	for (std::vector<PlotExtraCost>::iterator it = m_aPlotExtraCosts.begin(); it != m_aPlotExtraCosts.end(); ++it)
	{
		(*it).write(pStream);
	}

	WRAPPER_WRITE_DECORATED(wrapper, "CvGame", m_mapVoteSourceReligions.size(), "VoteSourceReligionsCount");
	for (stdext::hash_map<VoteSourceTypes, ReligionTypes>::iterator it = m_mapVoteSourceReligions.begin(); it != m_mapVoteSourceReligions.end(); ++it)
	{
		WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "CvGame", REMAPPED_CLASS_TYPE_VOTE_SOURCES, it->first, "VoteSourceReligionMapSource");
		WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "CvGame", REMAPPED_CLASS_TYPE_RELIGIONS, it->second, "VoteSourceReligionMapReligion");
	}

	WRAPPER_WRITE_DECORATED(wrapper, "CvGame", m_aeInactiveTriggers.size(), "InactiveTriggersCount");
	for (std::vector<EventTriggerTypes>::iterator e_it = m_aeInactiveTriggers.begin(); e_it != m_aeInactiveTriggers.end(); ++e_it)
	{
		WRAPPER_WRITE_CLASS_ENUM_DECORATED(wrapper, "CvGame", REMAPPED_CLASS_TYPE_EVENT_TRIGGERS, *e_it, "InactiveTrigger");
	}

	WRAPPER_WRITE(wrapper, "CvGame", m_iShrineBuildingCount);
	//	Retain bug-compatibility with the old format which needlessly saved the entire max
	//	size the array could possibly be here
	if ( wrapper.isUsingTaggedFormat() )
	{
		WRAPPER_WRITE_CLASS_ENUM_ARRAY(wrapper, "CvGame", REMAPPED_CLASS_TYPE_BUILDINGS, m_iShrineBuildingCount, m_aiShrineBuilding);
		WRAPPER_WRITE_CLASS_ENUM_ARRAY(wrapper, "CvGame", REMAPPED_CLASS_TYPE_RELIGIONS, m_iShrineBuildingCount, m_aiShrineReligion);
	}
	else
	{
		WRAPPER_WRITE_CLASS_ENUM_ARRAY(wrapper,"CvGame",REMAPPED_CLASS_TYPE_BUILDINGS,GC.getNumBuildingInfos(), m_aiShrineBuilding);
		WRAPPER_WRITE_CLASS_ENUM_ARRAY(wrapper,"CvGame",REMAPPED_CLASS_TYPE_RELIGIONS,GC.getNumBuildingInfos(), m_aiShrineReligion);
	}
	WRAPPER_WRITE(wrapper, "CvGame", m_iNumCultureVictoryCities);
	WRAPPER_WRITE(wrapper, "CvGame", m_eCultureVictoryCultureLevel);

	m_Properties.writeWrapper(pStream);

	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvGame", REMAPPED_CLASS_TYPE_IMPROVEMENTS, GC.getNumImprovementInfos(), m_paiImprovementCount);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvGame", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiTechGameTurnDiscovered);

	WRAPPER_WRITE_OBJECT_END(wrapper);

}

void CvGame::writeReplay(FDataStreamBase& stream, PlayerTypes ePlayer)
{
	SAFE_DELETE(m_pReplayInfo);
	m_pReplayInfo = new CvReplayInfo();
	if (m_pReplayInfo)
	{
		m_pReplayInfo->createInfo(ePlayer);

		m_pReplayInfo->write(stream);
	}
}

void CvGame::saveReplay(PlayerTypes ePlayer)
{
	//	First opportunity after a load of a won game to close the wrapper.  This is important since
	//	hall of fame replay info does not use max compatability format and so must not have the
	//	current wrapper selecting that
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	//	Close will free any resources and display any warnings if we've just finished loading/saving
	wrapper.close();

	gDLL->getEngineIFace()->SaveReplay(ePlayer);
}


void CvGame::showEndGameSequence()
{
	const int iHours = getMinutesPlayed() / 60;
	const int iMinutes = getMinutesPlayed() % 60;

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		CvPlayer& player = GET_PLAYER((PlayerTypes)iI);
		if (player.isHuman())
		{
			addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, (PlayerTypes)iI, gDLL->getText("TXT_KEY_MISC_TIME_SPENT", iHours, iMinutes));

			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_TEXT);
			if (NULL != pInfo)
			{
				if (getWinner() != NO_TEAM && getVictory() != NO_VICTORY)
				{
					pInfo->setText(gDLL->getText("TXT_KEY_GAME_WON", GET_TEAM(getWinner()).getName().GetCString(), GC.getVictoryInfo(getVictory()).getTextKeyWide()));
				}
				else pInfo->setText(gDLL->getText("TXT_KEY_MISC_DEFEAT"));

				player.addPopup(pInfo);
			}

			if (getWinner() == player.getTeam())
			{
				if (!CvString(GC.getVictoryInfo(getVictory()).getMovie()).empty())
				{
					// show movie
					pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
					if (NULL != pInfo)
					{
						pInfo->setText(L"showVictoryMovie");
						pInfo->setData1((int)getVictory());
						player.addPopup(pInfo);
					}
				}
				else if (GC.getVictoryInfo(getVictory()).isDiploVote())
				{
					pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
					if (NULL != pInfo)
					{
						pInfo->setText(L"showUnVictoryScreen");
						player.addPopup(pInfo);
					}
				}
			}

			// show replay
			pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
			if (NULL != pInfo)
			{
				pInfo->setText(L"showReplay");
				pInfo->setData1(iI);
				pInfo->setOption1(false); // don't go to HOF on exit
				player.addPopup(pInfo);
			}

			// show top cities / stats
			pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
			if (NULL != pInfo)
			{
				pInfo->setText(L"showInfoScreen");
				pInfo->setData1(0);
				pInfo->setData2(1);
				player.addPopup(pInfo);
			}

			// show Dan
			pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
			if (NULL != pInfo)
			{
				pInfo->setText(L"showDanQuayleScreen");
				player.addPopup(pInfo);
			}

			// show Hall of Fame
			pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
			if (NULL != pInfo)
			{
				pInfo->setText(L"showHallOfFame");
				player.addPopup(pInfo);
			}
		}
	}
}

CvReplayInfo* CvGame::getReplayInfo() const
{
	return m_pReplayInfo;
}

void CvGame::setReplayInfo(CvReplayInfo* pReplay)
{
	SAFE_DELETE(m_pReplayInfo);
	m_pReplayInfo = pReplay;
}

bool CvGame::hasSkippedSaveChecksum() const
{
	return gDLL->hasSkippedSaveChecksum();
}

void CvGame::logNetMsgData(char* format, ...)
{
	if (isNetworkMultiPlayer())
	{
		char szOut[1024];
		if (getActivePlayer() != -1)
			sprintf(szOut, "Player %d - Multiplayer Game Log.log", getActivePlayer());
		else
			sprintf(szOut, "Pitboss Multiplayer Game Log.log");

		static char buf[2048];
		_vsnprintf(buf, 2048 - 4, format, (char*)(&format + 1));
		gDLL->logMsg(szOut, buf, false, false);
	}
}


void CvGame::addPlayer(PlayerTypes eNewPlayer, LeaderHeadTypes eLeader, CivilizationTypes eCiv, bool bSetAlive)
{
	// Reset names for recycled player slot
	CvWString szEmptyString = L"";
	const LeaderHeadTypes eOldLeader = GET_PLAYER(eNewPlayer).getLeaderType();

	if (eOldLeader != NO_LEADER && eOldLeader != eLeader)
	{
		GC.getInitCore().setLeaderName(eNewPlayer, szEmptyString);
	}
	const CivilizationTypes eOldCiv = GET_PLAYER(eNewPlayer).getCivilizationType();

	if (eOldCiv != NO_CIVILIZATION && eOldCiv != eCiv)
	{
		GC.getInitCore().setCivAdjective(eNewPlayer, szEmptyString);
		GC.getInitCore().setCivDescription(eNewPlayer, szEmptyString);
		GC.getInitCore().setCivShortDesc(eNewPlayer, szEmptyString);
	}
	// Get valid player color
	PlayerColorTypes eColor = (PlayerColorTypes)GC.getCivilizationInfo(eCiv).getDefaultPlayerColor();

	if (!GET_PLAYER(eNewPlayer).isNPC())
	{
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			// Don't invalidate color choice if it's taken by this player
			if (eColor == NO_PLAYERCOLOR || GET_PLAYER((PlayerTypes)iI).getPlayerColor() == eColor && (PlayerTypes)iI != eNewPlayer)
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
	}
	// init new player
	GC.getInitCore().setLeader(eNewPlayer, eLeader);
	GC.getInitCore().setCiv(eNewPlayer, eCiv);
	GC.getInitCore().setSlotStatus(eNewPlayer, SS_COMPUTER);
	GC.getInitCore().setColor(eNewPlayer, eColor);
	GET_PLAYER(eNewPlayer).initInGame(eNewPlayer, bSetAlive);
}

void CvGame::changeHumanPlayer( PlayerTypes eOldHuman, PlayerTypes eNewHuman )
{
	// It's a multiplayer game, eep!
	if(GC.getInitCore().getMultiplayer())
	{
		int netID = GC.getInitCore().getNetID(eOldHuman);
		GC.getInitCore().setNetID(eNewHuman, netID);
		GC.getInitCore().setNetID(eOldHuman, -1);
	}

	GET_PLAYER(eNewHuman).setIsHuman(true);
	if(getActivePlayer()==eOldHuman)
	setActivePlayer(eNewHuman, false);

	for (int iI = 0; iI < NUM_PLAYEROPTION_TYPES; iI++)
	{
		GET_PLAYER(eNewHuman).setOption( (PlayerOptionTypes)iI, GET_PLAYER(eOldHuman).isOption((PlayerOptionTypes)iI) );
	}

	for (iI = 0; iI < NUM_PLAYEROPTION_TYPES; iI++)
	{
		gDLL->sendPlayerOption(((PlayerOptionTypes)iI), GET_PLAYER(eNewHuman).isOption((PlayerOptionTypes)iI));
	}

	GET_PLAYER(eOldHuman).setIsHuman(false);
}

bool CvGame::isCompetingCorporation(CorporationTypes eCorporation1, CorporationTypes eCorporation2) const
{
	if (GC.getCorporationInfo(eCorporation1).isCompetingCorporation(eCorporation2) || GC.getCorporationInfo(eCorporation2).isCompetingCorporation(eCorporation1))
		return true;

	foreach_(const BonusTypes eBonus, GC.getCorporationInfo(eCorporation1).getPrereqBonuses())
	{
		if (algo::any_of_equal(GC.getCorporationInfo(eCorporation2).getPrereqBonuses(), eBonus))
		{
			return true;
		}
	}

	return false;
}

// Toffer - ToDo - I think we want to use this more extensively,
//	so move it over to CvPlot and store extra yield in an array of three elements per plot.
int CvGame::getPlotExtraYield(int iX, int iY, YieldTypes eYield) const
{
	foreach_(const PlotExtraYield& extraYield, m_aPlotExtraYields)
	{
		if (extraYield.m_iX == iX && extraYield.m_iY == iY)
		{
			return extraYield.m_aeExtraYield[eYield];
		}
	}
	return 0;
}

void CvGame::setPlotExtraYield(int iX, int iY, YieldTypes eYield, int iExtraYield)
{
	bool bFound = false;

	foreach_(PlotExtraYield& extraYield, m_aPlotExtraYields)
	{
		if (extraYield.m_iX == iX && extraYield.m_iY == iY)
		{
			extraYield.m_aeExtraYield[eYield] += iExtraYield;
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		PlotExtraYield kExtraYield;
		kExtraYield.m_iX = iX;
		kExtraYield.m_iY = iY;
		for (int i = 0; i < NUM_YIELD_TYPES; ++i)
		{
			if (eYield == i)
			{
				kExtraYield.m_aeExtraYield.push_back(iExtraYield);
			}
			else
			{
				kExtraYield.m_aeExtraYield.push_back(0);
			}
		}
		m_aPlotExtraYields.push_back(kExtraYield);
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if (NULL != pPlot)
	{
		pPlot->updateYield();
	}
}

/* Toffer - Unused, but might be needed for recalc...
void CvGame::removePlotExtraYield(int iX, int iY)
{
	for (std::vector<PlotExtraYield>::iterator it = m_aPlotExtraYields.begin(); it != m_aPlotExtraYields.end(); ++it)
	{
		if ((*it).m_iX == iX && (*it).m_iY == iY)
		{
			m_aPlotExtraYields.erase(it);
			break;
		}
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if (NULL != pPlot)
	{
		pPlot->updateYield();
	}
}
*/

int CvGame::getPlotExtraCost(int iX, int iY) const
{
	foreach_(const PlotExtraCost& it, m_aPlotExtraCosts)
	{
		if (it.m_iX == iX && it.m_iY == iY)
		{
			return it.m_iCost;
		}
	}

	return 0;
}

void CvGame::changePlotExtraCost(int iX, int iY, int iCost)
{
	bool bFound = false;

	foreach_(PlotExtraCost& it, m_aPlotExtraCosts)
	{
		if (it.m_iX == iX && it.m_iY == iY)
		{
			it.m_iCost += iCost;
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		PlotExtraCost kExtraCost;
		kExtraCost.m_iX = iX;
		kExtraCost.m_iY = iY;
		kExtraCost.m_iCost = iCost;
		m_aPlotExtraCosts.push_back(kExtraCost);
	}
}

void CvGame::removePlotExtraCost(int iX, int iY)
{
	for (std::vector<PlotExtraCost>::iterator it = m_aPlotExtraCosts.begin(); it != m_aPlotExtraCosts.end(); ++it)
	{
		if ((*it).m_iX == iX && (*it).m_iY == iY)
		{
			m_aPlotExtraCosts.erase(it);
			break;
		}
	}
}

ReligionTypes CvGame::getVoteSourceReligion(VoteSourceTypes eVoteSource) const
{
	stdext::hash_map<VoteSourceTypes, ReligionTypes>::const_iterator it;

	it = m_mapVoteSourceReligions.find(eVoteSource);
	if (it == m_mapVoteSourceReligions.end())
	{
		return NO_RELIGION;
	}
	return it->second;
}

void CvGame::setVoteSourceReligion(VoteSourceTypes eVoteSource, ReligionTypes eReligion, bool bAnnounce)
{
	FAssert(eReligion != NO_RELIGION);

	m_mapVoteSourceReligions[eVoteSource] = eReligion;

	if (bAnnounce && NO_RELIGION != eReligion)
	{
		const CvWString szBuffer =
		(
			gDLL->getText(
				"TXT_KEY_VOTE_SOURCE_RELIGION",
				GC.getReligionInfo(eReligion).getTextKeyWide(),
				GC.getReligionInfo(eReligion).getAdjectiveKey(),
				GC.getVoteSourceInfo(eVoteSource).getTextKeyWide()
			)
		);
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				AddDLLMessage(
					(PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(),
					szBuffer, GC.getReligionInfo(eReligion).getSound(),
					MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_HIGHLIGHT_TEXT()
				);
			}
		}
	}
}


// CACHE: cache frequently used values
///////////////////////////////////////


int CvGame::getShrineBuildingCount(ReligionTypes eReligion)
{
	int	iShrineBuildingCount = 0;

	if (eReligion == NO_RELIGION)
		iShrineBuildingCount = m_iShrineBuildingCount;
	else for (int iI = 0; iI < m_iShrineBuildingCount; iI++)
		if (m_aiShrineReligion[iI] == eReligion)
			iShrineBuildingCount++;

	return iShrineBuildingCount;
}

BuildingTypes CvGame::getShrineBuilding(int eIndex, ReligionTypes eReligion)
{
	FASSERT_BOUNDS(0, m_iShrineBuildingCount, eIndex);

	BuildingTypes eBuilding = NO_BUILDING;

	if (eIndex >= 0 && eIndex < m_iShrineBuildingCount)
	{
		if (eReligion == NO_RELIGION)
			eBuilding = (BuildingTypes) m_aiShrineBuilding[eIndex];
		else for (int iI = 0, iReligiousBuilding = 0; iI < m_iShrineBuildingCount; iI++)
			if (m_aiShrineReligion[iI] == (int) eReligion)
			{
				if (iReligiousBuilding == eIndex)
				{
					// found it
					eBuilding = (BuildingTypes) m_aiShrineBuilding[iI];
					break;
				}

				iReligiousBuilding++;
			}
	}

	return eBuilding;
}

void CvGame::changeShrineBuilding(BuildingTypes eBuilding, ReligionTypes eReligion, bool bRemove)
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eBuilding);
	FAssertMsg(bRemove || m_iShrineBuildingCount < GC.getNumBuildingInfos(), "trying to add too many buildings to CvGame::changeShrineBuilding");

	if (bRemove)
	{
		bool bFound = false;

		for (int iI = 0; iI < m_iShrineBuildingCount; iI++)
		{
			if (!bFound)
			{
				// note, eReligion is not important if we removing, since each building is always one religion
				if (m_aiShrineBuilding[iI] == (int) eBuilding)
					bFound = true;
			}

			if (bFound)
			{
				int iToMove = iI + 1;
				if (iToMove < m_iShrineBuildingCount)
				{
					m_aiShrineBuilding[iI] = m_aiShrineBuilding[iToMove];
					m_aiShrineReligion[iI] = m_aiShrineReligion[iToMove];
				}
				else
				{
					m_aiShrineBuilding[iI] = (int) NO_BUILDING;
					m_aiShrineReligion[iI] = (int) NO_RELIGION;
				}

				m_iShrineBuildingCount--;
			}
		}
	}
	else if (m_iShrineBuildingCount < GC.getNumBuildingInfos())
	{
		// add this item to the end
		m_aiShrineBuilding[m_iShrineBuildingCount] = eBuilding;
		m_aiShrineReligion[m_iShrineBuildingCount] = eReligion;
		m_iShrineBuildingCount++;
	}
}

bool CvGame::culturalVictoryValid() const
{
	return (m_iNumCultureVictoryCities > 0);
}

int CvGame::culturalVictoryNumCultureCities() const
{
	return m_iNumCultureVictoryCities;
}

CultureLevelTypes CvGame::culturalVictoryCultureLevel() const
{
	return (m_iNumCultureVictoryCities > 0) ? (CultureLevelTypes) m_eCultureVictoryCultureLevel : NO_CULTURELEVEL;
}

int CvGame::getCultureThreshold(CultureLevelTypes eLevel) const
{
	return GC.getCultureLevelInfo(eLevel).getSpeedThreshold(getGameSpeedType());
}

void CvGame::doUpdateCacheOnTurn()
{

	// reset shrine count
	m_iShrineBuildingCount = 0;

	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		const CvBuildingInfo& kBuildingInfo = GC.getBuildingInfo((BuildingTypes) iI);

		// if it is for holy city, then its a shrine-thing, add it
		if (kBuildingInfo.getHolyCity() != NO_RELIGION)
		{
			changeShrineBuilding((BuildingTypes) iI, (ReligionTypes) kBuildingInfo.getReligionType());
		}
	}

	// reset cultural victories
	m_iNumCultureVictoryCities = 0;
	for (iI = 0; iI < GC.getNumVictoryInfos(); iI++)
	{
		if (isVictoryValid((VictoryTypes) iI))
		{
			const CvVictoryInfo& kVictoryInfo = GC.getVictoryInfo((VictoryTypes) iI);
			if (kVictoryInfo.getCityCulture() > 0)
			{
				int iNumCultureCities = kVictoryInfo.getNumCultureCities();
				if (iNumCultureCities > m_iNumCultureVictoryCities)
				{
					m_iNumCultureVictoryCities = iNumCultureCities;
					m_eCultureVictoryCultureLevel = kVictoryInfo.getCityCulture();
				}
			}
		}
	}
}

VoteSelectionData* CvGame::getVoteSelection(int iID) const
{
	return ((VoteSelectionData*)(m_voteSelections.getAt(iID)));
}

VoteSelectionData* CvGame::addVoteSelection(VoteSourceTypes eVoteSource)
{
	VoteSelectionData* pData = ((VoteSelectionData*)(m_voteSelections.add()));

	if  (NULL != pData)
	{
		pData->eVoteSource = eVoteSource;

		for (int iI = 0; iI < GC.getNumVoteInfos(); iI++)
		{
			if (GC.getVoteInfo((VoteTypes)iI).isVoteSourceType(eVoteSource))
			{
				if (isChooseElection((VoteTypes)iI))
				{
					VoteSelectionSubData kData;
					kData.eVote = (VoteTypes)iI;
					kData.iCityId = -1;
					kData.ePlayer = NO_PLAYER;
					kData.eOtherPlayer = NO_PLAYER;

					if (GC.getVoteInfo(kData.eVote).isOpenBorders())
					{
						if (isValidVoteSelection(eVoteSource, kData))
						{
							kData.szText = gDLL->getText("TXT_KEY_POPUP_ELECTION_OPEN_BORDERS", getVoteRequired(kData.eVote, eVoteSource), countPossibleVote(kData.eVote, eVoteSource));
							pData->aVoteOptions.push_back(kData);
						}
					}
					else if (GC.getVoteInfo(kData.eVote).isDefensivePact())
					{
						if (isValidVoteSelection(eVoteSource, kData))
						{
							kData.szText = gDLL->getText("TXT_KEY_POPUP_ELECTION_DEFENSIVE_PACT", getVoteRequired(kData.eVote, eVoteSource), countPossibleVote(kData.eVote, eVoteSource));
							pData->aVoteOptions.push_back(kData);
						}
					}
					else if (GC.getVoteInfo(kData.eVote).isForcePeace())
					{
						for (int iTeam1 = 0; iTeam1 < MAX_PC_TEAMS; ++iTeam1)
						{
							CvTeam& kTeam1 = GET_TEAM((TeamTypes)iTeam1);

							if (kTeam1.isAlive())
							{
								kData.ePlayer = kTeam1.getLeaderID();

								if (isValidVoteSelection(eVoteSource, kData))
								{
									kData.szText = gDLL->getText("TXT_KEY_POPUP_ELECTION_FORCE_PEACE", kTeam1.getName().GetCString(), getVoteRequired(kData.eVote, eVoteSource), countPossibleVote(kData.eVote, eVoteSource));
									pData->aVoteOptions.push_back(kData);
								}
							}
						}
					}
					else if (GC.getVoteInfo(kData.eVote).isForceNoTrade())
					{
						for (int iTeam1 = 0; iTeam1 < MAX_PC_TEAMS; ++iTeam1)
						{
							CvTeam& kTeam1 = GET_TEAM((TeamTypes)iTeam1);

							if (kTeam1.isAlive())
							{
								kData.ePlayer = kTeam1.getLeaderID();

								if (isValidVoteSelection(eVoteSource, kData))
								{
									kData.szText = gDLL->getText("TXT_KEY_POPUP_ELECTION_FORCE_NO_TRADE", kTeam1.getName().GetCString(), getVoteRequired(kData.eVote, eVoteSource), countPossibleVote(kData.eVote, eVoteSource));
									pData->aVoteOptions.push_back(kData);
								}
							}
						}
					}
					else if (GC.getVoteInfo(kData.eVote).isForceWar())
					{
						for (int iTeam1 = 0; iTeam1 < MAX_PC_TEAMS; ++iTeam1)
						{
							CvTeam& kTeam1 = GET_TEAM((TeamTypes)iTeam1);

							if (kTeam1.isAlive())
							{
								kData.ePlayer = kTeam1.getLeaderID();

								if (isValidVoteSelection(eVoteSource, kData))
								{
									kData.szText = gDLL->getText("TXT_KEY_POPUP_ELECTION_FORCE_WAR", kTeam1.getName().GetCString(), getVoteRequired(kData.eVote, eVoteSource), countPossibleVote(kData.eVote, eVoteSource));
									pData->aVoteOptions.push_back(kData);
								}
							}
						}
					}
					else if (GC.getVoteInfo(kData.eVote).isAssignCity())
					{
						for (int iPlayer1 = 0; iPlayer1 < MAX_PC_PLAYERS; ++iPlayer1)
						{
							const CvPlayer& kPlayer1 = GET_PLAYER((PlayerTypes)iPlayer1);

							if (kPlayer1.isAlive())
							{
								foreach_(const CvCity* pLoopCity, kPlayer1.cities())
								{
									const PlayerTypes eNewOwner = pLoopCity->plot()->findHighestCulturePlayer();
									if (NO_PLAYER != eNewOwner)
									{
										kData.ePlayer = (PlayerTypes)iPlayer1;
										kData.iCityId =	pLoopCity->getID();
										kData.eOtherPlayer = eNewOwner;

										if (isValidVoteSelection(eVoteSource, kData))
										{
											kData.szText = gDLL->getText("TXT_KEY_POPUP_ELECTION_ASSIGN_CITY", kPlayer1.getCivilizationAdjectiveKey(), pLoopCity->getNameKey(), GET_PLAYER(eNewOwner).getNameKey(), getVoteRequired(kData.eVote, eVoteSource), countPossibleVote(kData.eVote, eVoteSource));
											pData->aVoteOptions.push_back(kData);
										}
									}
								}
							}
						}
					}
					else
					{
						kData.szText = gDLL->getText("TXT_KEY_POPUP_ELECTION_OPTION", GC.getVoteInfo(kData.eVote).getTextKeyWide(), getVoteRequired(kData.eVote, eVoteSource), countPossibleVote(kData.eVote, eVoteSource));
						if (isVotePassed(kData.eVote))
						{
							kData.szText += gDLL->getText("TXT_KEY_POPUP_PASSED");
						}

						if (canDoResolution(eVoteSource, kData))
						{
							pData->aVoteOptions.push_back(kData);
						}
					}
				}
			}
		}

		if (pData->aVoteOptions.empty())
		{
			deleteVoteSelection(pData->getID());
			pData = NULL;
		}
	}

	return pData;
}

void CvGame::deleteVoteSelection(int iID)
{
	m_voteSelections.removeAt(iID);
}

VoteTriggeredData* CvGame::getVoteTriggered(int iID) const
{
	return ((VoteTriggeredData*)(m_votesTriggered.getAt(iID)));
}

VoteTriggeredData* CvGame::addVoteTriggered(const VoteSelectionData& kData, int iChoice)
{
	if (-1 == iChoice || iChoice >= (int)kData.aVoteOptions.size())
	{
		return NULL;
	}

	return addVoteTriggered(kData.eVoteSource, kData.aVoteOptions[iChoice]);
}

VoteTriggeredData* CvGame::addVoteTriggered(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kOptionData)
{
	VoteTriggeredData* pData = ((VoteTriggeredData*)(m_votesTriggered.add()));

	if (NULL != pData)
	{
		pData->eVoteSource = eVoteSource;
		pData->kVoteOption = kOptionData;

		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
			if (kPlayer.isAlive() && kPlayer.isVotingMember(eVoteSource))
			{
				if (kPlayer.isHuman())
				{
					CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_DIPLOVOTE);
					if (NULL != pInfo)
					{
						pInfo->setData1(pData->getID());
						gDLL->getInterfaceIFace()->addPopup(pInfo, (PlayerTypes)iI);
					}
				}
				else
				{
					castVote(((PlayerTypes)iI), pData->getID(), kPlayer.AI_diploVote(kOptionData, eVoteSource, false));
				}
			}
		}
	}

	return pData;
}

void CvGame::deleteVoteTriggered(int iID)
{
	m_votesTriggered.removeAt(iID);
}

void CvGame::doVoteResults()
{
	int iLoop;
	for (VoteTriggeredData* pVoteTriggered = m_votesTriggered.beginIter(&iLoop); NULL != pVoteTriggered; pVoteTriggered = m_votesTriggered.nextIter(&iLoop))
	{
		const VoteTypes eVote = pVoteTriggered->kVoteOption.eVote;
		VoteSourceTypes eVoteSource = pVoteTriggered->eVoteSource;
		bool bPassed = false;

		if (!canDoResolution(eVoteSource, pVoteTriggered->kVoteOption))
		{
			for (int iI = 0; iI < MAX_PC_PLAYERS; ++iI)
			{
				if (GET_PLAYER((PlayerTypes) iI).isAlive()
				&& GET_PLAYER((PlayerTypes) iI).isVotingMember(eVoteSource))
				{
					CvWString szMessage;
					szMessage.Format(L"%s: %s", gDLL->getText("TXT_KEY_ELECTION_CANCELLED").GetCString(), GC.getVoteInfo(eVote).getDescription());
					AddDLLMessage((PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_NEW_ERA", MESSAGE_TYPE_INFO, NULL, GC.getCOLOR_HIGHLIGHT_TEXT());
				}
			}
		}
		else
		{
			bool bAllVoted = true;
			for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
			{
				const PlayerTypes ePlayer = (PlayerTypes) iJ;
				if (GET_PLAYER(ePlayer).isAlive() && GET_PLAYER(ePlayer).isVotingMember(eVoteSource))
				{
					if (getPlayerVote(ePlayer, pVoteTriggered->getID()) == NO_PLAYER_VOTE)
					{
						//give player one more turn to submit vote
						setPlayerVote(ePlayer, pVoteTriggered->getID(), NO_PLAYER_VOTE_CHECKED);
						bAllVoted = false;
						break;
					}
					else if (getPlayerVote(ePlayer, pVoteTriggered->getID()) == NO_PLAYER_VOTE_CHECKED)
					{
						//default player vote to abstain
						setPlayerVote(ePlayer, pVoteTriggered->getID(), PLAYER_VOTE_ABSTAIN);
					}
				}
			}
			if (!bAllVoted) continue;

			CvWString szBuffer;

			if (isTeamVote(eVote))
			{
				TeamTypes eTeam = findHighestVoteTeam(*pVoteTriggered);

				if (NO_TEAM != eTeam)
				{
					bPassed = countVote(*pVoteTriggered, (PlayerVoteTypes)eTeam) >= getVoteRequired(eVote, eVoteSource);
				}

				szBuffer = GC.getVoteInfo(eVote).getDescription();

				if (eTeam != NO_TEAM)
				{
					szBuffer += NEWLINE + gDLL->getText("TXT_KEY_POPUP_DIPLOMATIC_VOTING_VICTORY", GET_TEAM(eTeam).getName().GetCString(), countVote(*pVoteTriggered, (PlayerVoteTypes)eTeam), getVoteRequired(eVote, eVoteSource), countPossibleVote(eVote, eVoteSource));
				}

				for (int iI = MAX_PC_TEAMS; iI >= 0; --iI)
				{
					for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
					{
						if (GET_PLAYER((PlayerTypes)iJ).isAlive() && GET_PLAYER((PlayerTypes)iJ).isVotingMember(eVoteSource)
						&& getPlayerVote(((PlayerTypes)iJ), pVoteTriggered->getID()) == (PlayerVoteTypes)iI)
						{
							szBuffer += NEWLINE + gDLL->getText("TXT_KEY_POPUP_VOTES_FOR", GET_PLAYER((PlayerTypes)iJ).getNameKey(), GET_TEAM((TeamTypes)iI).getName().GetCString(), GET_PLAYER((PlayerTypes)iJ).getVotes(eVote, eVoteSource));
						}
					}
				}

				for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive() && GET_PLAYER((PlayerTypes)iJ).isVotingMember(eVoteSource)
					&& getPlayerVote(((PlayerTypes)iJ), pVoteTriggered->getID()) == PLAYER_VOTE_ABSTAIN)
					{
						szBuffer += NEWLINE + gDLL->getText("TXT_KEY_POPUP_ABSTAINS", GET_PLAYER((PlayerTypes)iJ).getNameKey(), GET_PLAYER((PlayerTypes)iJ).getVotes(eVote, eVoteSource));
					}
				}

				if (NO_TEAM != eTeam && bPassed)
				{
					setVoteOutcome(*pVoteTriggered, (PlayerVoteTypes)eTeam);
				}
				else setVoteOutcome(*pVoteTriggered, PLAYER_VOTE_ABSTAIN);
			}
			else
			{
				bPassed = countVote(*pVoteTriggered, PLAYER_VOTE_YES) >= getVoteRequired(eVote, eVoteSource);

				// Defying resolution
				if (bPassed)
				{
					for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
					{
						if (getPlayerVote((PlayerTypes)iJ, pVoteTriggered->getID()) == PLAYER_VOTE_NEVER)
						{
							bPassed = false;
							GET_PLAYER((PlayerTypes)iJ).setDefiedResolution(eVoteSource, pVoteTriggered->kVoteOption);
						}
					}
				}
				if (bPassed)
				{
					for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
					{
						if (GET_PLAYER((PlayerTypes)iJ).isAlive() && GET_PLAYER((PlayerTypes)iJ).isVotingMember(eVoteSource)
						&& getPlayerVote(((PlayerTypes)iJ), pVoteTriggered->getID()) == PLAYER_VOTE_YES)
						{
							GET_PLAYER((PlayerTypes)iJ).setEndorsedResolution(eVoteSource, pVoteTriggered->kVoteOption);
						}
					}
				}
				szBuffer += NEWLINE + gDLL->getText((bPassed ? "TXT_KEY_POPUP_DIPLOMATIC_VOTING_SUCCEEDS" : "TXT_KEY_POPUP_DIPLOMATIC_VOTING_FAILURE"), GC.getVoteInfo(eVote).getTextKeyWide(), countVote(*pVoteTriggered, PLAYER_VOTE_YES), getVoteRequired(eVote, eVoteSource), countPossibleVote(eVote, eVoteSource));

				for (int iI = PLAYER_VOTE_NEVER; iI <= PLAYER_VOTE_YES; ++iI)
				{
					for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
					{
						if (GET_PLAYER((PlayerTypes)iJ).isAlive()
						&&  GET_PLAYER((PlayerTypes)iJ).isVotingMember(eVoteSource)
						&& getPlayerVote(((PlayerTypes)iJ), pVoteTriggered->getID()) == (PlayerVoteTypes)iI)
						{
							switch ((PlayerVoteTypes)iI)
							{
							case PLAYER_VOTE_ABSTAIN:
								szBuffer += NEWLINE + gDLL->getText("TXT_KEY_POPUP_ABSTAINS", GET_PLAYER((PlayerTypes)iJ).getNameKey(), GET_PLAYER((PlayerTypes)iJ).getVotes(eVote, eVoteSource));
								break;
							case PLAYER_VOTE_NEVER:
								szBuffer += NEWLINE + gDLL->getText("TXT_KEY_POPUP_VOTES_YES_NO", GET_PLAYER((PlayerTypes)iJ).getNameKey(), L"TXT_KEY_POPUP_VOTE_NEVER", GET_PLAYER((PlayerTypes)iJ).getVotes(eVote, eVoteSource));
								break;
							case PLAYER_VOTE_NO:
								szBuffer += NEWLINE + gDLL->getText("TXT_KEY_POPUP_VOTES_YES_NO", GET_PLAYER((PlayerTypes)iJ).getNameKey(), L"TXT_KEY_POPUP_NO", GET_PLAYER((PlayerTypes)iJ).getVotes(eVote, eVoteSource));
								break;
							case PLAYER_VOTE_YES:
								szBuffer += NEWLINE + gDLL->getText("TXT_KEY_POPUP_VOTES_YES_NO", GET_PLAYER((PlayerTypes)iJ).getNameKey(), L"TXT_KEY_POPUP_YES", GET_PLAYER((PlayerTypes)iJ).getVotes(eVote, eVoteSource));
								break;
							default:
								FErrorMsg("error");
								break;
							}
						}
					}
				}
				setVoteOutcome(*pVoteTriggered, bPassed ? PLAYER_VOTE_YES : PLAYER_VOTE_NO);
			}

			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
				if (kPlayer.isHuman())
				{
					bool bShow = kPlayer.isVotingMember(pVoteTriggered->eVoteSource);

					if (bShow)
					{
						CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_TEXT);
						if (NULL != pInfo)
						{
							pInfo->setText(szBuffer);
							gDLL->getInterfaceIFace()->addPopup(pInfo, (PlayerTypes)iI);
						}
					}

					if (!bShow)
					{
						if (iI == pVoteTriggered->kVoteOption.ePlayer && GET_PLAYER(pVoteTriggered->kVoteOption.ePlayer).isVotingMember(pVoteTriggered->eVoteSource))
						{
							bShow = true;
						}
					}

					if (!bShow)
					{
						if (iI == pVoteTriggered->kVoteOption.eOtherPlayer && GET_PLAYER(pVoteTriggered->kVoteOption.eOtherPlayer).isVotingMember(pVoteTriggered->eVoteSource))
						{
							bShow = true;
						}
					}

					if (bShow && bPassed)
					{

						const CvWString szMessage = gDLL->getText("TXT_KEY_VOTE_RESULTS", GC.getVoteSourceInfo(eVoteSource).getTextKeyWide(), pVoteTriggered->kVoteOption.szText.GetCString());
						AddDLLMessage((PlayerTypes)iI, false, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_NEW_ERA", MESSAGE_TYPE_MINOR_EVENT, NULL, GC.getCOLOR_HIGHLIGHT_TEXT());
					}

				}
			}
		}

		if (!bPassed && GC.getVoteInfo(eVote).isSecretaryGeneral())
		{
			setSecretaryGeneralTimer(eVoteSource, 0);
		}
		deleteVoteTriggered(pVoteTriggered->getID());
	}
}

void CvGame::doVoteSelection()
{
	for (int iI = 0; iI < GC.getNumVoteSourceInfos(); ++iI)
	{
		const VoteSourceTypes eVoteSource = (VoteSourceTypes)iI;

		if (!isDiploVote(eVoteSource)) continue;

		if (getVoteTimer(eVoteSource) <= 0)
		{
			setVoteTimer(eVoteSource, GC.getVoteSourceInfo(eVoteSource).getVoteInterval() * GC.getGameSpeedInfo(getGameSpeedType()).getSpeedPercent() / 100);

			for (int iTeam1 = 0; iTeam1 < MAX_PC_TEAMS; ++iTeam1)
			{
				CvTeam& kTeam1 = GET_TEAM((TeamTypes)iTeam1);

				if (kTeam1.isAlive() && kTeam1.isVotingMember(eVoteSource))
				{
					for (int iTeam2 = iTeam1 + 1; iTeam2 < MAX_PC_TEAMS; ++iTeam2)
					{
						const CvTeam& kTeam2 = GET_TEAM((TeamTypes)iTeam2);

						if (kTeam2.isAlive() && kTeam2.isVotingMember(eVoteSource))
						{
							kTeam1.meet((TeamTypes)iTeam2, true);
						}
					}
				}
			}

			const TeamTypes eSecretaryGeneral = getSecretaryGeneral(eVoteSource);
			const PlayerTypes eSecretaryPlayer =
			(
				eSecretaryGeneral == NO_TEAM
				?
				NO_PLAYER
				:
				GET_TEAM(eSecretaryGeneral).getSecretaryID()
			);

			bool bSecretaryGeneralVote = false;
			if (canHaveSecretaryGeneral(eVoteSource))
			{
				if (getSecretaryGeneralTimer(eVoteSource) <= 0)
				{
					setSecretaryGeneralTimer(eVoteSource, GC.getDefineINT("DIPLO_VOTE_SECRETARY_GENERAL_INTERVAL"));

					for (int iJ = 0; iJ < GC.getNumVoteInfos(); iJ++)
					{
						if (GC.getVoteInfo((VoteTypes)iJ).isSecretaryGeneral() && GC.getVoteInfo((VoteTypes)iJ).isVoteSourceType(iI))
						{
							VoteSelectionSubData kOptionData;
							kOptionData.iCityId = -1;
							kOptionData.ePlayer = NO_PLAYER;
							kOptionData.eOtherPlayer = NO_PLAYER;
							kOptionData.eVote = (VoteTypes)iJ;
							kOptionData.szText = gDLL->getText("TXT_KEY_POPUP_ELECTION_OPTION", GC.getVoteInfo((VoteTypes)iJ).getTextKeyWide(), getVoteRequired((VoteTypes)iJ, eVoteSource), countPossibleVote((VoteTypes)iJ, eVoteSource));
							addVoteTriggered(eVoteSource, kOptionData);
							bSecretaryGeneralVote = true;
							break;
						}
					}
				}
				else changeSecretaryGeneralTimer(eVoteSource, -1);
			}

			if (!bSecretaryGeneralVote && eSecretaryGeneral != NO_TEAM && eSecretaryPlayer != NO_PLAYER)
			{
				VoteSelectionData* pData = addVoteSelection(eVoteSource);
				if (NULL == pData)
				{
					setVoteTimer(eVoteSource, 0);
				}
				else if (GET_PLAYER(eSecretaryPlayer).isHuman())
				{
					CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSEELECTION);
					if (NULL != pInfo)
					{
						pInfo->setData1(pData->getID());
						gDLL->getInterfaceIFace()->addPopup(pInfo, eSecretaryPlayer);
					}
				}
				else setVoteChosen(GET_TEAM(eSecretaryGeneral).AI_chooseElection(*pData), pData->getID());
			}
		}
		else changeVoteTimer(eVoteSource, -1);
	}
}

bool CvGame::isEventActive(EventTriggerTypes eTrigger) const
{
	return algo::none_of_equal(m_aeInactiveTriggers, eTrigger);
}

void CvGame::initEvents()
{
	for (int iTrigger = 0; iTrigger < GC.getNumEventTriggerInfos(); ++iTrigger)
	{
		if (isOption(GAMEOPTION_NO_EVENTS) || getSorenRandNum(100, "Event Active?") >= GC.getEventTriggerInfo((EventTriggerTypes)iTrigger).getPercentGamesActive())
		{
			m_aeInactiveTriggers.push_back((EventTriggerTypes)iTrigger);
		}
	}
}

bool CvGame::isCivEverActive(CivilizationTypes eCivilization) const
{
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.isEverAlive() && kLoopPlayer.getCivilizationType() == eCivilization)
		{
			return true;
		}
	}

	return false;
}

bool CvGame::isLeaderEverActive(LeaderHeadTypes eLeader) const
{
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.isEverAlive() && kLoopPlayer.getLeaderType() == eLeader)
		{
			return true;
		}
	}

	return false;
}

void CvGame::processBuilding(BuildingTypes eBuilding, int iChange)
{
	PROFILE_FUNC();

	for (int iI = 0; iI < GC.getNumVoteSourceInfos(); ++iI)
	{
		if (GC.getBuildingInfo(eBuilding).getVoteSourceType() == (VoteSourceTypes)iI)
		{
			changeDiploVote((VoteSourceTypes)iI, iChange);
		}
	}
}

bool CvGame::pythonIsBonusIgnoreLatitudes() const
{
	bool lResult = false;
	if (Cy::call_override(GC.getMap().getMapScript(), "isBonusIgnoreLatitude", lResult))
	{
		return lResult;
	}
	return false;
}

// BarbarianWorld
bool CvGame::foundBarbarianCity()
{
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	const MapCategoryTypes earth = GC.getMAPCATEGORY_EARTH();

	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
	{
		CvPlot* plotX = GC.getMap().plotByIndex(iPlot);
		if (plotX->isWater() || plotX->isImpassable() || plotX->isCity() || plotX->getImprovementType() != NO_IMPROVEMENT || !plotX->isMapCategoryType(earth))
		{
			continue;
		}
		bool bValid = true;
		int iValue = 0;
		bool bOccupiedArea = false;
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				const CvPlot* pPlotI = GET_PLAYER((PlayerTypes)iI).getStartingPlot();

				if (pPlotI != NULL)
				{
					const int iDist = GC.getMap().calculatePathDistance(pPlotI, plotX);
					if (iDist >= 0)
					{
						bOccupiedArea = true;

						if (iDist < 5)
						{
							bValid = false;
						}
						else if (iDist < 10)
						{
							iValue -= (10 - iDist)*7;
						}
					}
				}
			}
		}

		if (bOccupiedArea)
		{
			iValue += (isOption(GAMEOPTION_BARBARIAN_CIV) ? 1000 : 250);
		}
		else if (isOption(GAMEOPTION_BARBARIAN_CIV))
		{
			bValid = false;
		}

		if (bValid)
		{
			iValue += GET_PLAYER(BARBARIAN_PLAYER).AI_foundValue(plotX->getX(), plotX->getY(), GC.getDefineINT("MIN_BARBARIAN_CITY_STARTING_DISTANCE"));
			iValue += getSorenRandNum(250, "Barb City Found");
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestPlot = plotX;
			}
		}
	}

	if (pBestPlot != NULL)
	{
		GET_PLAYER(BARBARIAN_PLAYER).found(pBestPlot->getX(), pBestPlot->getY());
		return true;
	}
	return false;
}

bool CvGame::getStarshipLaunched(int ID) const
{
	return starshipLaunched[GET_PLAYER((PlayerTypes)ID).getTeam()];
}

bool CvGame::getDiplomaticVictoryAchieved(int ID) const
{
	return diplomaticVictoryAchieved[GET_PLAYER((PlayerTypes)ID).getTeam()];
}

int CvGame::getCurrentVoteID() const
{
	return m_iCurrentVoteID;
}

void CvGame::setCurrentVoteID(int iNewValue)
{
	if (iNewValue != getCurrentVoteID())
	{
		m_iCurrentVoteID = iNewValue;
	}
}

int CvGame::getCutLosersCounter() const
{
	return m_iCutLosersCounter;
}

void CvGame::changeCutLosersCounter(int iChange)
{
	m_iCutLosersCounter += iChange;
}

int CvGame::getHighToLowCounter() const
{
	return m_iHighToLowCounter;
}

void CvGame::changeHighToLowCounter(int iChange)
{
	m_iHighToLowCounter += iChange;
}

int CvGame::getIncreasingDifficultyCounter() const
{
	return m_iIncreasingDifficultyCounter;
}

void CvGame::changeIncreasingDifficultyCounter(int iChange)
{
	m_iIncreasingDifficultyCounter += iChange;
}

void CvGame::doFinalFive()
{
	if (!isGameMultiPlayer() && isOption(GAMEOPTION_CHALLENGE_CUT_LOSERS) && countCivPlayersAlive() > 5)
	{
		changeCutLosersCounter(1);
		if (getCutLosersCounter() >= GC.getDefineINT("CUT_LOSERS_TURN_INCREMENT") * GC.getGameSpeedInfo(getGameSpeedType()).getSpeedPercent() / 100)
		{
			GET_PLAYER(getRankPlayer(countCivPlayersAlive() -1)).setAlive(false);
			changeCutLosersCounter(getCutLosersCounter() * -1);

			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHuman())
				{
					AddDLLMessage(
						(PlayerTypes)iI, true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_LOW_PLAYER_DROPPED").GetCString(),
						"AS2D_FIRSTTOTECH", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_WARNING_TEXT()
					);
				}
			}
		}
	}
}

void CvGame::doHightoLow()
{

	if (!isGameMultiPlayer() && isOption(GAMEOPTION_CHALLENGE_HIGH_TO_LOW)
	&& getGameTurn() >= GC.getDefineINT("HIGH_TO_LOW_FIRST_TURN_CHECK")
	&& getHighToLowCounter() < 2)
	{
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHuman() && getPlayerRank((PlayerTypes)iI) == 0)
			{
				GC.getInitCore().reassignPlayerAdvanced((PlayerTypes)iI, getRankPlayer(countCivPlayersAlive() -1));
				changeHighToLowCounter(1);

				AddDLLMessage(
					(PlayerTypes)iI, true, GC.getEVENT_MESSAGE_TIME(),
					gDLL->getText("TXT_KEY_PLAYER_REASSIGNED").GetCString(),
					"AS2D_WELOVEKING", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_GREEN()
				);
			}
		}
	}
}

void CvGame::doIncreasingDifficulty()
{

	if (isOption(GAMEOPTION_CHALLENGE_INCREASING_DIFFICULTY))
	{
		changeIncreasingDifficultyCounter(1);
		if (getIncreasingDifficultyCounter() >= GC.getDefineINT("INCREASING_DIFFICULTY_TURN_CHECK_INCREMENTS") * GC.getGameSpeedInfo(getGameSpeedType()).getSpeedPercent() / 100
		&& getHandicapType() < GC.getNumHandicapInfos() - 1)
		{
			setHandicapType((HandicapTypes)(getHandicapType() + 1));
			changeIncreasingDifficultyCounter(getIncreasingDifficultyCounter() * -1);

			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHuman())
				{
					GC.getInitCore().setHandicap((PlayerTypes)iI, getHandicapType());
					GET_PLAYER((PlayerTypes)iI).AI_makeAssignWorkDirty();
					algo::for_each(GET_PLAYER((PlayerTypes)iI).cities(), CvCity::fn::setInfoDirty(true));

					AddDLLMessage(
						(PlayerTypes)iI, true, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_DIFFICULTY_INCREASED").GetCString(),
						"AS2D_FEAT_ACCOMPLISHED", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_WARNING_TEXT()
					);
				}
			}
		}
	}
}

void CvGame::doFlexibleDifficulty()
{
	logging::logMsg("C2C.log", "doFlexibleDifficulty");

	const bool bFlexDiffForAI = isModderGameOption(MODDERGAMEOPTION_AI_USE_FLEXIBLE_DIFFICULTY);

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		const PlayerTypes ePlayer = static_cast<PlayerTypes>(iI);
		CvPlayer& kPlayer = GET_PLAYER(ePlayer);
		//Check we are rebalancing the score for the team leader (and ignoring vassals)
		if (kPlayer.isAlive() && GET_TEAM(kPlayer.getTeam()).getLeaderID() == ePlayer)
		{
			int iTurns = kPlayer.getModderOption(MODDEROPTION_FLEXIBLE_DIFFICULTY_TURN_INCREMENTS);
			int iTimer = getFlexibleDifficultyTimer(ePlayer);

			if (iTurns <= 0 || bFlexDiffForAI && !kPlayer.isHuman())
			{
				iTurns = GC.getDefineINT("DEFAULT_FLEXIBLE_DIFFICULTY_TURN_INCREMENTS", 25);
				iTurns *= GC.getGameSpeedInfo(getGameSpeedType()).getSpeedPercent();
				iTurns /= 100;
			}

			logging::logMsg("C2C.log", "[Flexible Difficulty] (%d / %d) turns until next flexible difficulty check for Player: %S\n", iTimer, iTurns, kPlayer.getName());

			//Increase timer
			iTimer++;
			setFlexibleDifficultyTimer(ePlayer, iTimer);

			if (iTimer < iTurns) continue;

			logging::logMsg("C2C.log", "[Flexible Difficulty] Player: %S, Checking Flexible Difficulty\n", kPlayer.getName());

			if (!kPlayer.isModderOption(MODDEROPTION_FLEXIBLE_DIFFICULTY)
			&& (!bFlexDiffForAI || kPlayer.isHuman()))
			{
				continue;
			}
			logging::logMsg("C2C.log", "[Flexible Difficulty] Player: %S has Flexible Difficulty Enabled\n", kPlayer.getName());
			int iMinHandicap = kPlayer.getModderOption(MODDEROPTION_FLEXIBLE_DIFFICULTY_MIN_DIFFICULTY);
			int iMaxHandicap = kPlayer.getModderOption(MODDEROPTION_FLEXIBLE_DIFFICULTY_MAX_DIFFICULTY);

			if (bFlexDiffForAI && !kPlayer.isHuman())
			{
				iMinHandicap = 0;
				iMaxHandicap = GC.getNumHandicapInfos() - 1;
			}
			else if (iMaxHandicap < 0)
			{
				iMaxHandicap = MAX_INT;
			}
			//Reset counter
			setFlexibleDifficultyTimer(ePlayer, 0);

			//int iMinRank = kPlayer.getModderOption(MODDEROPTION_FLEXIBLE_DIFFICULTY_MIN_RANK);
			//int iMaxRank = kPlayer.getModderOption(MODDEROPTION_FLEXIBLE_DIFFICULTY_MAX_RANK);
			//if (iMaxRank < 0)
			//	iMaxRank = MAX_INT;
			//int iCurrentRank = getPlayerRank(ePlayer);

			int iBestScore = MIN_INT;
			int iAliveCount = 0;
			int iTotalScore = 0;
			for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
			{
				const CvPlayerAI& pPlayer = GET_PLAYER((PlayerTypes)iJ);
				if (pPlayer.isAlive() && GET_TEAM(pPlayer.getTeam()).getLeaderID() == iJ)
				{
					iAliveCount++;
					const int iScore = getPlayerScore((PlayerTypes)iJ);
					iTotalScore += iScore;
					if (iScore > iBestScore)
					{
						iBestScore = iScore;
					}
				}
			}
			FAssertMsg(iAliveCount > 0, "iAliveCount can not be <= 0");
			const int iMeanScore = iTotalScore / iAliveCount;
			int iVariance = 0;
			for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
			{
				const CvPlayerAI& pPlayer = GET_PLAYER((PlayerTypes)iJ);

				if (pPlayer.isAlive() && GET_TEAM(pPlayer.getTeam()).getLeaderID() == iJ)
				{
					const int iScore = getPlayerScore((PlayerTypes)iJ);
					// iVariance is sum of squared difference from mean
					iVariance += (iMeanScore - iScore) * (iMeanScore - iScore);
					logging::logMsg("C2C.log", "[Flexible Difficulty] Adding score for player %S, score: %d\n", pPlayer.getName(), iScore);
				}
			}
			const int stddev = iVariance >= 0 ? intSqrt(10000 * iVariance / iAliveCount) : 0;

			const int iCurrentScore = getPlayerScore(ePlayer);
			logging::logMsg("C2C.log",
				"[Flexible Difficulty] Player: %S, Score: %d, Difficulty: %S, Avg Score: %d, Std Dev: %d/100\n",
				kPlayer.getName(), iCurrentScore,
				GC.getHandicapInfo((HandicapTypes)kPlayer.getHandicapType()).getDescription(),
				iMeanScore, stddev
			);
			int iNewHandicap =
			(
				kPlayer.getHandicapType() > iMaxHandicap
				?
				iMaxHandicap
				:
				(
					kPlayer.getHandicapType() < iMinHandicap
					?
					iMinHandicap
					:
					kPlayer.getHandicapType()
				)
			);
			//Increased Difficulty (player's score is > 1 std dev away)
			if (100*iCurrentScore > 100*iMeanScore + stddev)
			{
				logging::logMsg("C2C.log", "[Flexible Difficulty] Player: %S score is > 1 std dev above average.\n", kPlayer.getName());
				if (iNewHandicap < (GC.getNumHandicapInfos() - 1) && iNewHandicap < iMaxHandicap)
				{
					logging::logMsg("C2C.log",
						"[Flexible Difficulty] Player: %S difficulty is increasing from %S to %S\n",
						kPlayer.getName(),
						GC.getHandicapInfo(kPlayer.getHandicapType()).getDescription(),
						GC.getHandicapInfo((HandicapTypes)(kPlayer.getHandicapType() + 1)).getDescription()
					);
					iNewHandicap++;
				}
			}
			else if (100*iCurrentScore < 100*iMeanScore - stddev)
			{
				logging::logMsg("C2C.log", "[Flexible Difficulty] Player: %S score is > 1 std dev below average.\n", kPlayer.getName());
				if (iNewHandicap > 0 && iNewHandicap > iMinHandicap)
				{
					logging::logMsg("C2C.log",
						"[Flexible Difficulty] Player: %S difficulty is decreasing from %S to %S\n",
						kPlayer.getName(),
						GC.getHandicapInfo(kPlayer.getHandicapType()).getDescription(),
						GC.getHandicapInfo((HandicapTypes)(kPlayer.getHandicapType() - 1)).getDescription()
					);
					iNewHandicap--;
				}
			}
			if (iNewHandicap != kPlayer.getHandicapType())
			{
				const bool diffIncreasing = iNewHandicap > kPlayer.getHandicapType();
				const char* TXT_KEY_DIFFICULTY_PLAYER_ID =
				(
					diffIncreasing
					?
					"TXT_KEY_DIFFICULTY_INCREASED_FOR_PLAYER"
					:
					"TXT_KEY_DIFFICULTY_DECREASED_FOR_PLAYER"
				);
				const char* AS2D_ID = diffIncreasing ? "AS2D_FEAT_ACCOMPLISHED" : "AS2D_FIRSTTOTECH";

				//Inform all human players of any other players difficulty changes
				for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
				{
					const CvPlayer& player = GET_PLAYER((PlayerTypes)iJ);
					if (player.isAlive() && player.isHuman() && GET_TEAM(player.getTeam()).isHasMet(kPlayer.getTeam()))
					{
						if (iJ == iI) // Difficulty increased for us
						{
							AddDLLMessage(
								ePlayer, true, GC.getEVENT_MESSAGE_TIME(),
								gDLL->getText(
									diffIncreasing
									?
									"TXT_KEY_DIFFICULTY_INCREASED"
									:
									"TXT_KEY_DIFFICULTY_DECREASED"
								).GetCString(),
								AS2D_ID, MESSAGE_TYPE_MAJOR_EVENT, NULL,
								diffIncreasing ? GC.getCOLOR_WARNING_TEXT() : GC.getCOLOR_GREEN()
							);
						}
						else // Difficulty increased for someone else
						{
							AddDLLMessage(
								(PlayerTypes)iJ, true, GC.getEVENT_MESSAGE_TIME(),
								gDLL->getText(
									TXT_KEY_DIFFICULTY_PLAYER_ID,
									kPlayer.getCivilizationDescriptionKey()
								).GetCString(),
								AS2D_ID, MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_WARNING_TEXT()
							);
						}
					}
				}
				//Change the difficulty
				GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)iNewHandicap);

				//Clean the interface
				kPlayer.AI_makeAssignWorkDirty();
				algo::for_each(kPlayer.cities(), CvCity::fn::setInfoDirty(true));
			}
		}
	}
	averageHandicaps();
}

void CvGame::averageHandicaps()
{
	int iAverageHandicap = 0;
	int iHumanCount = 0;
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHuman())
		{
			iAverageHandicap += GET_PLAYER((PlayerTypes)iI).getHandicapType();
			iHumanCount++;
		}
	}
	if (iHumanCount > 0)
	{
		iAverageHandicap /= iHumanCount;
		iAverageHandicap = range(iAverageHandicap, 0, GC.getNumHandicapInfos() - 1);
		setHandicapType((HandicapTypes)iAverageHandicap);
	}
}

int CvGame::getMercyRuleCounter() const
{
	return m_iMercyRuleCounter;
}

void CvGame::changeMercyRuleCounter(int iChange)
{
	m_iMercyRuleCounter += iChange;
}

void CvGame::setMercyRuleCounter(int iNewVal)
{
	m_iMercyRuleCounter = iNewVal;
}

void CvGame::clearLandmarks(bool bClear)
{
	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);
		if (bClear)
		{
			if (pLoopPlot->getLandmarkType() != NO_LANDMARK)
			{
				pLoopPlot->setLandmarkType(NO_LANDMARK);
				pLoopPlot->setLandmarkMessage("");
				pLoopPlot->setLandmarkName("");
				pLoopPlot->removeSignForAllPlayers();
			}
		}
		else if (!CvWString(pLoopPlot->getLandmarkName()).empty() && pLoopPlot->getLandmarkType() == NO_LANDMARK)
		{
			pLoopPlot->removeSignForAllPlayers();
		}
	}
}


void CvGame::pruneLandmarks()
{
	int iWorldSize = GC.getMap().getWorldSize();

	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);
		if (pLoopPlot->getLandmarkType() != NO_LANDMARK)
		{
			removeAdjacentLandmarks(pLoopPlot, pLoopPlot, std::max(1, iWorldSize / 2));
		}
	}
}

void CvGame::removeAdjacentLandmarks(const CvPlot* pCenterPlot, const CvPlot* pExceptionPlot, int iRadius)
{
	if (pCenterPlot == NULL || iRadius <= 0)
	{
		return;
	}
	foreach_(CvPlot* pAdjacentPlot, pCenterPlot->adjacent())
	{
		if (pExceptionPlot == NULL || pExceptionPlot != pAdjacentPlot)
		{
			if (pAdjacentPlot->getLandmarkType() != NO_LANDMARK)
			{
				pAdjacentPlot->setLandmarkType(NO_LANDMARK);
				pAdjacentPlot->setLandmarkMessage("");
				pAdjacentPlot->setLandmarkName("");
			}
			removeAdjacentLandmarks(pAdjacentPlot, pExceptionPlot, iRadius - 1);
		}
	}
}

void CvGame::findMountainRanges()
{
	int iMinimumRangeSize = GC.getDefineINT("MINIMUM_MOUNTAIN_RANGE_SIZE");
	iMinimumRangeSize += (int)GC.getMap().getWorldSize();
	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);
		int iPeakCount = 0;
		if (!pLoopPlot->isCountedPlot())
		{
			//Mountain Ranges
			if (pLoopPlot->isPeak())
			{
				iPeakCount += countPeaks(pLoopPlot);
				pLoopPlot->setCountedPlot(true);
			}
			if (iPeakCount > iMinimumRangeSize)
			{
				pLoopPlot->setLandmarkType(LANDMARK_MOUNTAIN_RANGE);
			}
		}
	}
	//Clear Plot Cache when done
	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);
		pLoopPlot->setCountedPlot(false);
	}
}

void CvGame::findLonePeaks()
{
	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);
		if (pLoopPlot->isPeak())
		{
			bool bFoundPeak = false;
			int iHillCount = 0;
			foreach_(const CvPlot* pAdjacentPlot, pLoopPlot->adjacent())
			{//Check to see if it is alone
				if (pAdjacentPlot->isPeak())
				{
					bFoundPeak = true;
					break;
				}
				if (pAdjacentPlot->isHills())
					iHillCount++;
				//if a peak is surrounded by hills, don't count it as alone
				if (iHillCount > 2)
				{
					bFoundPeak = true;
					break;
				}
			}//Lone Peak Found
			if (!bFoundPeak)
			{
				pLoopPlot->setLandmarkType(LANDMARK_PEAK);
			}
		}
	}
}

// O O O
// X O X
// Y X X
//(X and Y are land, O, water. We are looking for Y, which should only be adjacent to 1 water tile... the bay!)
void CvGame::findBays()
{
	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);
		if (!pLoopPlot->isCountedPlot() && pLoopPlot->isCoastalLand())
		{
			//Find the Water
			int iCoastCount = 0;
			CvPlot* pCoast = NULL;
			foreach_(CvPlot* pAdjacentPlot, pLoopPlot->adjacent())
			{
				if (pAdjacentPlot->isWater() && !pAdjacentPlot->isLake())
				{
					iCoastCount++;
					pCoast = pAdjacentPlot;
				}
			}
			//Bay!
			if (iCoastCount == 1 && !pCoast->isCountedPlot())
			{
				markBayPlots(pCoast);
				pCoast->setCountedPlot(true);
				pCoast->setLandmarkType(LANDMARK_BAY);
			}
		}
	}
	//Clear Plot Cache when done
	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);
		pLoopPlot->setCountedPlot(false);
	}
}

void CvGame::findForests()
{
	const FeatureTypes eForest = GC.getFEATURE_FOREST();
	int iMinimumForestSize = GC.getMINIMUM_FOREST_SIZE();
	iMinimumForestSize += (int)GC.getMap().getWorldSize() * 2;
	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);
		int iForestCount = 0;
		if (!pLoopPlot->isCountedPlot() && pLoopPlot->getFeatureType() == eForest)
		{
			iForestCount++;
			pLoopPlot->setCountedPlot(true);
			iForestCount += countForest(pLoopPlot, iForestCount);
			if (iForestCount > iMinimumForestSize)
			{
				pLoopPlot->setLandmarkType(LANDMARK_FOREST);
			}
		}
	}
	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);
		pLoopPlot->setCountedPlot(false);
	}
}

void CvGame::findJungles()
{
	const FeatureTypes eJungle = GC.getFEATURE_JUNGLE();
	int iMinimumJungleSize = GC.getMINIMUM_JUNGLE_SIZE();
	iMinimumJungleSize += (int)GC.getMap().getWorldSize() * 2;
	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);
		int iJungleCount = 0;
		if (!pLoopPlot->isCountedPlot() && pLoopPlot->getFeatureType() == eJungle)
		{
			iJungleCount++;
			pLoopPlot->setCountedPlot(true);
			iJungleCount += countJungle(pLoopPlot, iJungleCount);
			if (iJungleCount > iMinimumJungleSize)
			{
				pLoopPlot->setCountedPlot(true);
				pLoopPlot->setLandmarkType(LANDMARK_JUNGLE);
			}
		}
	}
	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);
		pLoopPlot->setCountedPlot(false);
	}
}

void CvGame::findDeserts()
{
	const TerrainTypes eDesert = GC.getTERRAIN_DESERT();
	int iMinimumDesertSize = GC.getMINIMUM_DESERT_SIZE();
	iMinimumDesertSize += (int)GC.getMap().getWorldSize() * 2;
	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);
		int iDesertCount = 0;
		if (pLoopPlot->getTerrainType() == eDesert && !pLoopPlot->isHills() && !pLoopPlot->isPeak() && !pLoopPlot->isCountedPlot())
		{
			iDesertCount++;
			pLoopPlot->setCountedPlot(true);
			iDesertCount += countDesert(pLoopPlot);
			if (iDesertCount > 5)
			{
				pLoopPlot->setLandmarkType(LANDMARK_DESERT);
			}
		}
	}
	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);
		pLoopPlot->setCountedPlot(false);
	}
}

void CvGame::findLakes()
{
	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);
		if (pLoopPlot->isLake())
		{
			if (!pLoopPlot->isCountedPlot())
			{
				pLoopPlot->setCountedPlot(true);
				markLakePlots(pLoopPlot);
				pLoopPlot->setLandmarkType(LANDMARK_LAKE);
			}
		}
	}
	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);
		pLoopPlot->setCountedPlot(false);
	}
}

//Finds all adjacent lake plots to the given plot and marks them as counted.
//Used to ensure no more than 1 sign gets placed on each lake.
void CvGame::markLakePlots(const CvPlot* pPlot)
{
	foreach_(CvPlot* pAdjacentPlot, pPlot->adjacent())
	{
		if (!pAdjacentPlot->isCountedPlot() && pAdjacentPlot->isLake())
		{
			pAdjacentPlot->setCountedPlot(true);
			markLakePlots(pAdjacentPlot);
		}
	}
}

//Returns vowels based on the most common letters and the frequency that they occur
char CvGame::getRandomVowel()
{
	int iRand = getSorenRandNum(38100, "Random Vowel");

	char ch;
	if (iRand < 8167)
		ch = 'a';
	else if (iRand < 20869)
		ch = 'e';
	else if (iRand < 27835)
		ch = 'i';
	else if (iRand < 35342)
		ch = 'o';
	else
		ch = 'u';
	return ch;
}

char CvGame::getRandomConsonant()
{
	int iRand = 0;
	iRand += getSorenRandNum(34550, "Random Consonant");
	iRand += getSorenRandNum(34550, "Random Consonant");
	char ch;
	if (iRand < 1492)
		ch = 'b';
	else if (iRand < 4274)
		ch = 'c';
	else if (iRand < 8527)
		ch = 'd';
	else if (iRand < 10755)
		ch = 'f';
	else if (iRand < 12770)
		ch = 'g';
	else if (iRand < 18864)
		ch = 'h';
	else if (iRand < 19017)
		ch = 'j';
	else if (iRand < 19789)
		ch = 'k';
	else if (iRand < 23814)
		ch = 'l';
	else if (iRand < 26220)
		ch = 'm';
	else if (iRand < 32969)
		ch = 'n';
	else if (iRand < 34898)
		ch = 'p';
	else if (iRand < 34993)
		ch = 'q';
	else if (iRand < 40980)
		ch = 'r';
	else if (iRand < 47307)
		ch = 's';
	else if (iRand < 56363)
		ch = 't';
	else if (iRand < 57341)
		ch = 'v';
	else if (iRand < 59701)
		ch = 'w';
	else if (iRand < 59851)
		ch = 'x';
	else if (iRand < 61825)
		ch = 'y';
	else
		ch = 'z';

   return ch;
}
CvWString CvGame::generateRandomWord(int iMaxLength)
{
	CvWString szName;
	char chNextLetter;

	bool bBreak = false;
	//4 letter word minimum
	int iLength = std::min(iMaxLength, std::max(4, getSorenRandNum(iMaxLength, "Random Word Length")));
	for (int iI = 0; iI < iLength; iI++)
	{
		int iRand = getSorenRandNum(1000, "RandomLetter");
		if (iRand <= 381 || bBreak)
		{
			chNextLetter = getRandomVowel();
			bBreak = false;
		}
		else
		{
			chNextLetter = getRandomConsonant();
			bBreak = getSorenRandNum(3, "Break Consonants") == 0;
		}
		//Civ's RNG sucks
		getSorenRandNum(100, "Throw Away Number");
		//Capitalize First Letter
		if (iI == 0)
			chNextLetter = (char)toupper(chNextLetter);
		szName += chNextLetter;
	}
	return szName;
}

bool CvGame::isValidName(CvWString szName) const
{
	int iConsonantCount = 0;
	int iVowelCount = 0;
	int iVowelStreak = 0;
	int iConsonantStreak = 0;
	for (int iI = 0; iI < (int)szName.length(); iI++)
	{
		char ch = tolower(szName[iI]);
		if (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u')
		{
			iVowelCount++;
			iVowelStreak++;
			iConsonantStreak = 0;
		}
		else
		{
			iConsonantCount++;
			iConsonantStreak++;
			iVowelStreak = 0;
		}
		if (iConsonantStreak > 3)
		{
			return false;
		}
		if (iVowelStreak > 4)
		{
			return false;
		}
	}
	//More than 75% of the word is vowels
	if ((iVowelCount * 100 / std::max(1, iVowelCount + iConsonantCount)) >= 75)
	{
		return false;
	}
	//More than 70% of the word is consonants
	if ((iConsonantCount * 100 / std::max(1, iVowelCount + iConsonantCount)) >= 70)
	{
		return false;
	}

	return true;
}

CvWString CvGame::getRandomName(int iMaxLength)
{
	CvWString szName;
	CvWString szRandomName;
	int iLength = 0;
	int iMaxTries = 3;
	iMaxLength = std::max(4, iMaxLength);
	while (true)
	{
		szName = generateRandomWord(iMaxLength);
		if (isValidName(szName))
		{
			//First word is always valid
			if (iLength == 0)
			{
				szRandomName += szName;
				iLength += szName.length();
			}
			else
			{
				int iCurLength = iMaxLength - iLength - szName.length();
				if (iCurLength > 0)
				{
					szRandomName += ' ';
					szRandomName += szName;
					iLength += szName.length();
				}
				else
				{
					iMaxTries--;
				}
				iCurLength = iMaxLength - iLength;
				if (iMaxTries <= 0 || iCurLength < 4)
				{
					break;
				}
			}
		}
	}
	return szRandomName;
}

int CvGame::countDesert(const CvPlot* pPlot)
{
	int iDesert = 0;
	foreach_(CvPlot* pAdjacentPlot, pPlot->adjacent())
	{
		if (!pAdjacentPlot->isCountedPlot())
		{
			if (pAdjacentPlot->getTerrainType() == GC.getTERRAIN_DESERT() && !pAdjacentPlot->isHills() && !pAdjacentPlot->isPeak())
			{
				pAdjacentPlot->setCountedPlot(true);
				iDesert++;
				iDesert += countDesert(pAdjacentPlot);
			}
		}
	}
	return iDesert;
}

int CvGame::countJungle(const CvPlot* pPlot, int iJungle)
{
	//This is a big jungle
	if (iJungle > GC.getMINIMUM_JUNGLE_SIZE() * 2)
		return iJungle;
	foreach_(CvPlot* pAdjacentPlot, pPlot->adjacent())
	{
		if (!pAdjacentPlot->isCountedPlot())
		{
			if (pAdjacentPlot->getFeatureType() == GC.getFEATURE_JUNGLE())
			{
				iJungle++;
				pAdjacentPlot->setCountedPlot(true);
				iJungle += countJungle(pAdjacentPlot, iJungle);
			}
		}
	}
	return iJungle;
}

int CvGame::countForest(const CvPlot* pPlot, int iForest)
{
	//This is a big forest
	if (iForest > GC.getMINIMUM_FOREST_SIZE() * 2)
		return iForest;
	foreach_(CvPlot* pAdjacentPlot, pPlot->adjacent())
	{
		if (!pAdjacentPlot->isCountedPlot())
		{
			if (pAdjacentPlot->getFeatureType() == GC.getFEATURE_FOREST())
			{
				iForest++;
				pAdjacentPlot->setCountedPlot(true);
				iForest += countForest(pAdjacentPlot, iForest);
			}
		}
	}
	return iForest;
}

//Maximum length of message is 21 chars
void CvGame::addLandmarkSigns()
{
	CvWString szSign;
	CvWString szName;
	CvWString szTextKey;
	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);
		if (pLoopPlot->getLandmarkType() != NO_LANDMARK)
		{
			szSign.clear();
			szName.clear();
			switch (pLoopPlot->getLandmarkType())
			{
				case LANDMARK_BAY:
					szTextKey = "TXT_KEY_LANDMARK_BAY";
					szTextKey.append(CvWString::format(L"_%d", (getSorenRandNum(GC.getDefineINT("NUM_BAY_NAMES"), "Landmark Naming"))));
					break;
				case LANDMARK_FOREST:
					szTextKey = "TXT_KEY_LANDMARK_FOREST";
					szTextKey.append(CvWString::format(L"_%d", (getSorenRandNum(GC.getDefineINT("NUM_FOREST_NAMES"), "Landmark Naming"))));
					break;
				case LANDMARK_JUNGLE:
					szTextKey = "TXT_KEY_LANDMARK_JUNGLE";
					szTextKey.append(CvWString::format(L"_%d", (getSorenRandNum(GC.getDefineINT("NUM_JUNGLE_NAMES"), "Landmark Naming"))));
					break;
				case LANDMARK_PEAK:
					szTextKey = "TXT_KEY_LANDMARK_PEAK";
					szTextKey.append(CvWString::format(L"_%d", (getSorenRandNum(GC.getDefineINT("NUM_PEAK_NAMES"), "Landmark Naming"))));
					break;
				case LANDMARK_MOUNTAIN_RANGE:
					szTextKey = "TXT_KEY_LANDMARK_MOUNTAINS";
					szTextKey.append(CvWString::format(L"_%d", (getSorenRandNum(GC.getDefineINT("NUM_MOUNTAINS_NAMES"), "Landmark Naming"))));
					break;
				case LANDMARK_DESERT:
					szTextKey = "TXT_KEY_LANDMARK_DESERT";
					szTextKey.append(CvWString::format(L"_%d", (getSorenRandNum(GC.getDefineINT("NUM_DESERT_NAMES"), "Landmark Naming"))));
					break;
				case LANDMARK_LAKE:
					szTextKey = "TXT_KEY_LANDMARK_LAKE";
					szTextKey.append(CvWString::format(L"_%d", (getSorenRandNum(GC.getDefineINT("NUM_LAKE_NAMES"), "Landmark Naming"))));
					break;
				default:
					FErrorMsg("Unknown Landmark Type");
			}
			szSign = szTextKey;
			CvWString szDummy = " ";
			szTextKey = gDLL->getText(szTextKey, szDummy.GetCString());
			szName = getRandomName(21- szTextKey.length());
			szSign = gDLL->getText(szSign, szName.GetCString());

			pLoopPlot->setLandmarkMessage(szSign);
			pLoopPlot->setLandmarkName(szName);
		}
	}
}

//Marks nearby tiles as counted so that they will not receive multiple signs
void CvGame::markBayPlots(const CvPlot* pPlot)
{
	//Marks all plots around the given plot, and all plots around the adjacent plots as counted
	foreach_(CvPlot* pAdjacentPlot, pPlot->adjacent())
	{
		if (!pAdjacentPlot->isCountedPlot() && pAdjacentPlot->isWater())
		{
			pAdjacentPlot->setCountedPlot(true);
			foreach_(CvPlot* pAdjacentPlot2, pAdjacentPlot->adjacent())
			{
				if (!pAdjacentPlot2->isCountedPlot() && pAdjacentPlot2->isWater())
				{
					pAdjacentPlot2->setCountedPlot(true);
				}
			}
		}
	}
}

int CvGame::countPeaks(const CvPlot* pPlot, bool bCountHill)
{
	int iPeak = 0;
	foreach_(CvPlot* pAdjacentPlot, pPlot->adjacent())
	{
		if (!pAdjacentPlot->isCountedPlot())
		{
			if (pAdjacentPlot->isPeak())
			{
				iPeak++;
				pAdjacentPlot->setCountedPlot(true);
				iPeak += countPeaks(pAdjacentPlot, true);
			}
			//Use hills to find sparser mountain ranges
			else if (pAdjacentPlot->isHills() && bCountHill)
			{
				pAdjacentPlot->setCountedPlot(true);
				iPeak += countPeaks(pAdjacentPlot, false);
			}
		}
	}
	return iPeak;
}

void CvGame::updateInitialSigns()
{
	PROFILE_FUNC();

	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iPlot);
		if (pLoopPlot->getLandmarkType() != NO_LANDMARK)
		{
			for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
			{
				if (GET_PLAYER((PlayerTypes)iJ).isAlive() && pLoopPlot->isRevealed(GET_PLAYER((PlayerTypes)iJ).getTeam(), false))
				{
					pLoopPlot->addSign((PlayerTypes)iJ, pLoopPlot->getLandmarkMessage());
				}
			}
		}
	}
}

bool CvGame::isPreviousRequest(PlayerTypes ePlayer) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, ePlayer);
	return m_abPreviousRequest[ePlayer];
}


void CvGame::setPreviousRequest(PlayerTypes ePlayer, bool bNewValue)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, ePlayer);
	m_abPreviousRequest[ePlayer] = bNewValue;
}

bool CvGame::isModderGameOption(ModderGameOptionTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_MODDERGAMEOPTION_TYPES, eIndex);
	return m_aiModderGameOption[eIndex] > 0;
}

int CvGame::getModderGameOption(ModderGameOptionTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_MODDERGAMEOPTION_TYPES, eIndex);
	return m_aiModderGameOption[eIndex];
}

void CvGame::setModderGameOption(ModderGameOptionTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, NUM_MODDERGAMEOPTION_TYPES, eIndex);
	m_aiModderGameOption[eIndex] = iNewValue;
}

void CvGame::doFoundCorporations()
{
	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		doFoundCorporation((CorporationTypes)iI, false);
	}
}

// Toffer - Realistic Corporation specific routine
void CvGame::doFoundCorporation(CorporationTypes eCorporation, bool bForce)
{
	if (!isOption(GAMEOPTION_REALISTIC_CORPORATIONS)
	|| !canEverSpread(eCorporation)
	|| getHeadquarters(eCorporation) != NULL)
	{
		return;
	}
	const int iCheckTurns = (
		GC.getDefineINT("CORPORATION_FOUND_CHECK_TURNS")
		*
		GC.getGameSpeedInfo(getGameSpeedType()).getSpeedPercent() / 100
	);
	if (getElapsedGameTurns() % std::max(1, iCheckTurns) != 0 && !bForce)
	{
		return;
	}
	if (GC.getGame().isModderGameOption(MODDERGAMEOPTION_NO_AUTO_CORPORATION_FOUNDING))
	{
		for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			if (canEverTrain((UnitTypes)iI) && GC.getUnitInfo((UnitTypes)iI).getCorporationSpreads(eCorporation) > 0)
			{
				return;
			}
		}
	}
	TechTypes ePrereqTech = GC.getCorporationInfo(eCorporation).getTechPrereq();
	//Find the prereq tech for corporate HQ
	if (ePrereqTech == NO_TECH)
	{
		for (int i = 0; i < GC.getNumBuildingInfos(); i++)
		{
			if ((CorporationTypes)GC.getBuildingInfo((BuildingTypes)i).getGlobalCorporationCommerce() == eCorporation)
			{
				ePrereqTech = (TechTypes)GC.getBuildingInfo((BuildingTypes)i).getPrereqAndTech();
				if (ePrereqTech != NO_TECH)
				{
					//Found a tech, exit the loop, else keep looking
					break;
				}
			}
		}
		if (ePrereqTech == NO_TECH)
		{
			return;
		}
	}
	if (countKnownTechNumTeams(ePrereqTech) <= GC.getDefineINT("MINIMUM_PLAYERS_WITH_TECH_FOR_AUTO_CORPORATION_FOUNDING"))
	{
		return;
	}
	int iBestSpread = getSorenRandNum(GC.getDefineINT("RAND_CORPORATION_BEST_SPREAD"), "Corporation Founding Best Spread");
	CvCity* pBestCity = NULL;
	const TechTypes eObsoleteTech = GC.getCorporationInfo(eCorporation).getObsoleteTech();

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kPlayer.isAlive() && (eObsoleteTech == NO_TECH || !GET_TEAM(kPlayer.getTeam()).isHasTech(eObsoleteTech)))
		{
			foreach_(CvCity* pLoopCity, kPlayer.cities())
			{
				int iSpread = pLoopCity->getCorporationInfluence(eCorporation) * GC.getCorporationInfo(eCorporation).getSpread() / 100;

				if (iSpread > 0)
				{
					iSpread += getSorenRandNum(100, "Corporation Founding Random Luck");
					if (iSpread > iBestSpread)
					{
						pBestCity = pLoopCity;
						iBestSpread = iSpread;
					}
				}
			}
			//Discourage players with more than 2 corporations from getting more
			if (pBestCity != NULL && pBestCity->getOwner() == iI)
			{
				int iCorporationCount = 0;
				for (int iJ = 0; iJ < GC.getNumCorporationInfos(); iJ++)
				{
					if (getHeadquarters((CorporationTypes)iJ) != NULL
					&&  getHeadquarters((CorporationTypes)iJ)->getOwner() == iI)
					{
						iCorporationCount++;
					}
				}
				iBestSpread /= std::max(1, iCorporationCount);
			}
		}
	}
	if (pBestCity != NULL
	&& (getSorenRandNum(GC.getDefineINT("CORPORATION_SPREAD_RAND"), "Corporation Founding Rand") < iBestSpread || bForce))
	{
		setHeadquarters(eCorporation, pBestCity, true);
	}
}

int CvGame::getAverageCorporationInfluence(const CvCity* pCity, const CorporationTypes eCorporation) const
{
	if (getHeadquarters(eCorporation) == NULL)
	{
		return 100;
	}
	const int iSpread0 = GC.getCorporationInfo(eCorporation).getSpread();
	if (iSpread0 == 0) return 0;

	const int iSpreadMod = 100 + GET_PLAYER(pCity->getOwner()).getCorporationSpreadModifier();
	if (iSpreadMod == 0) return 0;

	const int iInfluence = GET_PLAYER(pCity->getOwner()).getCorporationInfluence(eCorporation);
	if (iInfluence == 0) return 0;

	const int iSpreadDistanceDivisor = GC.getDefineINT("CORPORATION_SPREAD_DISTANCE_DIVISOR");
	const int iMaxPlotDistance = GC.getMap().maxPlotDistance();

	int iTotalSpread = 0;
	int iPlayerCount = 0;
	int iRandThreshold = 0;

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);

		if (!kPlayer.isAlive() || kPlayer.isNoCorporations()
		||
			kPlayer.isNoForeignCorporations()
		&&	getHeadquarters(eCorporation)->getOwner() != pCity->getOwner())
		{
			continue;
		}
		iPlayerCount++;
		foreach_(const CvCity* pLoopCity, kPlayer.cities())
		{
			if (pLoopCity->isConnectedTo(pCity))
			{
				const int iSpread = iSpread0 * pLoopCity->getCorporationInfluence(eCorporation) / 100;
				if (iSpread > 0)
				{
					iRandThreshold = std::max(
						iRandThreshold,
						iSpread
						/
						std::max(
							1,
							iSpreadDistanceDivisor
							* plotDistance(
								pCity->getX(), pCity->getY(),
								pLoopCity->getX(), pLoopCity->getY()
							)
							/ iMaxPlotDistance - 5
						)
					);
				}
			}
		}
		iRandThreshold = iRandThreshold * iSpreadMod * iInfluence / 10000;

		iTotalSpread += iRandThreshold;
	}
	if (iPlayerCount > 0)
	{
		return iTotalSpread / iPlayerCount;
	}
	return 0;
}

bool CvGame::canEverResearch(TechTypes eTech) const
{
	if (isOption(GAMEOPTION_NO_FUTURE))
	{
		if ((EraTypes)GC.getTechInfo(eTech).getEra() > GC.getC2C_ERA_ATOMIC())
		{
			if (!GC.getTechInfo(eTech).isRepeat())
			{
				return false;
			}
		}
	}
	if (GC.getTechInfo(eTech).getPrereqGameOption() != NO_GAMEOPTION)
	{
		if (!isOption((GameOptionTypes)GC.getTechInfo(eTech).getPrereqGameOption()))
		{
			return false;
		}
	}
	return true;
}


int CvGame::getFlexibleDifficultyTimer(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	return m_aiFlexibleDifficultyTimer[eIndex];
}

void CvGame::setFlexibleDifficultyTimer(PlayerTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	m_aiFlexibleDifficultyTimer[eIndex] = iNewValue;
}

void CvGame::calculateRiverBuildings()
{
	m_iRiverBuildings = 0;
	for (int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
	{
		if (GC.getBuildingInfo((BuildingTypes)iJ).isRiver())
		{
			m_iRiverBuildings++;
		}
	}
}

int CvGame::getRiverBuildings() const
{
	return m_iRiverBuildings;
}

void CvGame::calculateCoastalBuildings()
{
	m_iCoastalBuildings = 0;
	for (int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
	{
		if (GC.getBuildingInfo((BuildingTypes)iJ).isWater())
		{
			m_iCoastalBuildings++;
		}
	}
}

int CvGame::getCoastalBuildings() const
{
	return m_iCoastalBuildings;
}

bool CvGame::canExitToMainMenu() const
{
	if (gDLL->getInterfaceIFace()->isDebugMenuCreated())
	{
		return false;
	}
	return true;
}

void CvGame::calculateNumWonders()
{
	m_iNumWonders = 0;
	for (int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
	{
		if (isLimitedWonder((BuildingTypes)iJ))
		{
			m_iNumWonders++;
		}
	}
}

int CvGame::getNumWonders() const
{
	return m_iNumWonders;
}

bool CvGame::canEverConstruct(BuildingTypes eBuilding) const
{
	if (eBuilding == NO_BUILDING)
	{
		return false;
	}
	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	if (kBuilding.getPrereqGameOption() != NO_GAMEOPTION && !isOption((GameOptionTypes)kBuilding.getPrereqGameOption()))
	{
		return false;
	}

	if (kBuilding.getNotGameOption() != NO_GAMEOPTION && isOption((GameOptionTypes)kBuilding.getNotGameOption()))
	{
		return false;
	}

	if (kBuilding.getPrereqCorporation() != NO_CORPORATION && !canEverSpread((CorporationTypes)kBuilding.getPrereqCorporation()))
	{
		return false;
	}

	if (kBuilding.getFoundsCorporation() != NO_CORPORATION)
	{
		if (!canEverSpread((CorporationTypes)kBuilding.getFoundsCorporation()))
		{
			return false;
		}
		if (isOption(GAMEOPTION_REALISTIC_CORPORATIONS))
		{
			return false;
		}
	}
	return true;
}

bool CvGame::canEverTrain(UnitTypes eUnit) const
{
	if (eUnit == NO_UNIT)
	{
		return false;
	}
	const CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);

	if (kUnit.getPrereqGameOption() != NO_GAMEOPTION && !isOption((GameOptionTypes)kUnit.getPrereqGameOption()))
	{
		return false;
	}

	if (kUnit.getNotGameOption() != NO_GAMEOPTION && isOption((GameOptionTypes)kUnit.getNotGameOption()))
	{
		return false;
	}

	if (kUnit.getPrereqCorporation() != NO_CORPORATION && !canEverSpread((CorporationTypes)kUnit.getPrereqCorporation()))
	{
		return false;
	}

	for (int iI = 0; iI < kUnit.getNumPrereqAndBuildings(); ++iI)
	{
		if (!canEverConstruct((BuildingTypes)kUnit.getPrereqAndBuilding(iI)))
		{
			return false;
		}
	}
	return true;
}

bool CvGame::canEverSpread(CorporationTypes eCorporation) const
{
	if (eCorporation == NO_CORPORATION)
	{
		return false;
	}
	if (GC.getCorporationInfo(eCorporation).getPrereqGameOption() != NO_GAMEOPTION
	&& !isOption((GameOptionTypes)GC.getCorporationInfo(eCorporation).getPrereqGameOption()))
	{
		return false;
	}
	return true;
}

namespace {
	bool validBarbarianShipUnit(const CvUnitInfo& unitInfo, const UnitTypes unitType)
	{
		return unitInfo.getCombat() > 0 && !unitInfo.isOnlyDefensive()
			&& unitInfo.getDomainType() == DOMAIN_LAND
			&& GET_TEAM(BARBARIAN_TEAM).isUnitBonusEnabledByTech(unitInfo, true)
			&& !unitInfo.isCivilizationUnit()
			&& GET_PLAYER(BARBARIAN_PLAYER).canTrain(unitType, false, false, false, true);
	}
}

void CvGame::loadPirateShip(CvUnit* pUnit)
{
	FAssertMsg(pUnit->getDomainType() == DOMAIN_SEA, "loadPirateShip expects to be passed a water unit");

	const bool bSM = GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS);

	for (int iI = 0; iI < pUnit->cargoSpace(); iI++)
	{
		int iBestUnit = -1;
		int iBestValue = 0;
		for (int iJ = 0; iJ < GC.getNumUnitInfos(); iJ++)
		{
			const CvUnitInfo& unitInfo = GC.getUnitInfo((UnitTypes) iJ);

			if (validBarbarianShipUnit(unitInfo, (UnitTypes) iJ) && (!bSM || pUnit->SMcargoSpaceAvailable((SpecialUnitTypes)unitInfo.getSpecialUnitType(), unitInfo.getDomainType()) > 0))
			{
				int iValue = 1 + getSorenRandNum(1000, "Barb Unit Selection");

				if (unitInfo.getUnitAIType(UNITAI_ATTACK))
				{
					iValue += 250;
				}

				if (iValue > iBestValue)
				{
					iBestUnit = iJ;
					iBestValue = iValue;
				}
			}
		}
		if (iBestUnit > -1)
		{
			CvUnit* pPirate = GET_PLAYER(BARBARIAN_PLAYER).initUnit((UnitTypes)iBestUnit, pUnit->getX(), pUnit->getY(), UNITAI_ATTACK, NO_DIRECTION, getSorenRandNum(10000, "AI Unit Birthmark"));
			if (pPirate != NULL
			&& pUnit->SMcargoSpaceAvailable(pPirate->getSpecialUnitType(), pPirate->getDomainType()) >= pPirate->SMCargoVolume())
			{
				pPirate->setTransportUnit(pUnit);
				pUnit->AI_setUnitAIType(UNITAI_ASSAULT_SEA);
			}
			else // Not enough space to load it on the ship...
			{
				pPirate->kill(false);
			}
		}
	}
}


void CvGame::recalculateModifiers()
{
	OutputDebugString("Start profiling(false) for modifier recalc\n");
	startProfilingDLL(false);
	PROFILE_FUNC();

	m_bRecalculatingModifiers = true;

	//establish improvement costs
	//for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	//{
	//	GC.getImprovementInfo((ImprovementTypes)iI).setHighestCost();
	//}

	//	Inhibit plot group manipulation until we rebuild at the end of everything else
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			GET_PLAYER((PlayerTypes)iI).inhibitPlotGroupCalcsUntilFullRebuild();
		}
	}

	// AIAndy: Recalculate which info class replacements are currently active
	GC.updateReplacements();

	for (int iI = 0; iI < GC.getNumVoteSourceInfos(); iI++)
	{
		m_aiDiploVote[iI] = 0;
	}

	for (int iI = 0; iI < GC.getNumSpecialUnitInfos(); iI++)
	{
		m_pabSpecialUnitValid[iI] = false;
		if (GC.getSpecialUnitInfo((SpecialUnitTypes)iI).isValid())
		{
			makeSpecialUnitValid((SpecialUnitTypes)iI);
		}
	}

	for (int iI = 0; iI < GC.getNumSpecialBuildingInfos(); iI++)
	{
		m_pabSpecialBuildingValid[iI] = false;
		if (GC.getSpecialBuildingInfo((SpecialBuildingTypes)iI).isValid())
		{
			makeSpecialBuildingValid((SpecialBuildingTypes)iI);
		}
	}

	//TB Nukefix
	//m_bNukesValid = false;

	// AIAndy: Clear properties for recalculate, should be before any recalculation of any team because change propagation can cause cross team propagation (depending on usage)
	// units are not supposed to have that kind of property (needs extra code if that changes)
	getProperties()->clearForRecalculate();

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* plotX = GC.getMap().plotByIndex(iI);
		plotX->clearModifierTotals();

		// Toffer - Yield cache - Maybe not the perfect spot for this, but it should be done early.
		plotX->recalculateBaseYield();
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			GET_TEAM((TeamTypes)iI).getProperties()->clearForRecalculate();
		}
	}

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.isAlive())
		{
			kLoopPlayer.getProperties()->clearForRecalculate();

			foreach_(CvCity* pLoopCity, kLoopPlayer.cities())
			{
				pLoopCity->getProperties()->clearForRecalculate();
			}
		}
	}

	algo::for_each(GC.getMap().areas(), CvArea::fn::clearModifierTotals());

	for(int iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			GET_TEAM((TeamTypes)iI).recalculateModifiers();
		}
	}

	for (int iI = 0; iI < GC.getNumVoteSourceInfos(); iI++)
	{
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
		{
			if (GET_PLAYER((PlayerTypes)iPlayer).isAlive())
			{
				GET_PLAYER((PlayerTypes)iPlayer).processVoteSourceBonus((VoteSourceTypes)iI, true);
			}
		}
	}

	updatePlotGroups(true);
	updateTradeRoutes();

	//	Recheck for disabled buildings everywhere (this has to be done after plot group establishment
	//	or else resource dependencies will mean it gets the wrong answer, which will in turn force
	//	(automatic) recalculation the following turn (which is very inefficient)
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.isAlive())
		{
			foreach_(CvCity* pLoopCity, kLoopPlayer.cities())
			{
				pLoopCity->doVicinityBonus();
				pLoopCity->checkBuildings(false);
			}
		}
	}

	GC.getMap().updateSight(true, false);

	gDLL->getEngineIFace()->RebuildAllPlots();

	GC.getMap().setupGraphical();
	GC.getMap().updateVisibility();
	GC.getMap().updateFog();

	Cy::call_optional(PYCivModule, "recalculateModifiers");

	m_bRecalculatingModifiers = false;

	//	Force a one-time reclaculation of all city commerce after coming out
	//	of the in-recalc section (which inhibits this doing any work)
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.isAlive())
		{
			foreach_(CvCity* pLoopCity, kLoopPlayer.cities())
			{
				pLoopCity->updateBuildingCommerce();
				pLoopCity->updateCommerce(NO_COMMERCE, true);
			}
			kLoopPlayer.recordHistory();
		}
	}

	stopProfilingDLL(false);
	OutputDebugString("Stop profiling(false) after modifier recalc\n");
}

CvProperties* CvGame::getProperties()
{
	return &m_Properties;
}

const CvProperties* CvGame::getPropertiesConst() const
{
	return &m_Properties;
}

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/

MapTypes CvGame::getCurrentMap() const
{
	FASSERT_BOUNDS(0, NUM_MAPS, m_eCurrentMap);
	return m_eCurrentMap;
}

void CvGame::setCurrentMap(MapTypes eNewMap)
{
	FASSERT_BOUNDS(0, NUM_MAPS, eNewMap);
	m_eCurrentMap = eNewMap;
}

void CvGame::processGreatWall(bool bIn, bool bForce, bool bSeeded) const
{
#ifdef THE_GREAT_WALL
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.isAlive())
		{
			foreach_(CvCity* pLoopCity, kLoopPlayer.cities())
			{
				if (pLoopCity->processGreatWall(bIn, bForce, bSeeded))
				{
					//	There can be only one!
					return;
				}
			}
		}
	}
#endif // THE_GREAT_WALL
}
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

//	Super forts adaptation to C2C - calc choke points if not already done
#define CURRENT_CHOKE_POINT_VERSION	2	//	Increment this whenever the choke point/canal value calculation
										//	mechanism gets updated

void CvGame::ensureChokePointsEvaluated()
{
	if ( m_iChokePointCalculationVersion < CURRENT_CHOKE_POINT_VERSION )
	{
		m_iChokePointCalculationVersion = CURRENT_CHOKE_POINT_VERSION;

		GC.getMap().calculateCanalAndChokePoints();
	}
}

int CvGame::getBaseAirUnitIncrementsbyCargoVolume() const
{
	const int multiplier = std::max(1, GC.getSIZE_MATTERS_MOST_VOLUMETRIC_MULTIPLIER());
	int iBase = 10000 / multiplier;
	return iBase;
}

int CvGame::getBaseMissileUnitIncrementsbyCargoVolume() const
{
	const int multiplier = std::max(1, GC.getSIZE_MATTERS_MOST_VOLUMETRIC_MULTIPLIER());
	int iBase = 10000 / (multiplier * multiplier * multiplier);
	return std::max(1, iBase);
}

bool CvGame::isAnyoneHasUnitZoneOfControl() const
{
	return m_bAnyoneHasUnitZoneOfControl;
}

void CvGame::toggleAnyoneHasUnitZoneOfControl()
{
	m_bAnyoneHasUnitZoneOfControl = true;
}

void CvGame::logOOSSpecial(int iLocID, int iVar, int iVar2, int iVar3)
{
	if (isNetworkMultiPlayer())
	{
		char szFile[1024];
		sprintf(szFile, "OOSSpecialLogger - Player %d - Set %d.log", getActivePlayer(), isFinalInitialized() ? getGameTurn()/50 : -1);
		char szOut[1024];
		sprintf(szOut, "iLocID %d - iVar %d - iVar2 %d - iVar3 %d\n\tmapRand %d\n\tSorenRand %d", iLocID, iVar, iVar2, iVar3, getMapRand().getSeed(), getSorenRand().getSeed());
		gDLL->logMsg(szFile, szOut, false, false);
	}
}

int CvGame::getTopCityCount() const
{
	int iBest = 0;
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		const int iCount = GET_TEAM((TeamTypes)iI).getNumCities();
		if (iCount > iBest)
		{
			iBest = iCount;
		}
	}
	return iBest;
}

int CvGame::getTopPopCount() const
{
	int iBest = 0;
	for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
	{
		const int iCount = GET_TEAM((TeamTypes)iI).getTotalPopulation(false);
		if (iCount > iBest)
		{
			iBest = iCount;
		}
	}
	return iBest;
}


int CvGame::getImprovementCount(ImprovementTypes eIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), eIndex);
	return m_paiImprovementCount[eIndex];
}


void CvGame::changeImprovementCount(ImprovementTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), eIndex);
	m_paiImprovementCount[eIndex] += iChange;
	FASSERT_NOT_NEGATIVE(getImprovementCount(eIndex));
}

bool CvGame::isValidByGameOption(const CvUnitCombatInfo& info) const
{
	for (int iI = 0; iI < info.getNumNotOnGameOptions(); iI++)
	{
		if (isOption((GameOptionTypes) info.getNotOnGameOption(iI)))
		{
			return false;
		}
	}
	for (int iI = 0; iI < info.getNumOnGameOptions(); iI++)
	{
		if (!isOption((GameOptionTypes) info.getOnGameOption(iI)))
		{
			return false;
		}
	}
	return true;
}

int CvGame::SorenRand::operator()(const int maxVal) const
{
	return GC.getGame().getSorenRandNum(maxVal, logMsg);
}


void CvGame::enforceOptionCompatibility(GameOptionTypes eOption)
{
	const CvGameOptionInfo& info = GC.getGameOptionInfo(eOption);

	foreach_(const GameOptionTypes& eOptionMustOn, info.getEnforcesGameOptionOnTypes())
	{
		if (!isOption(eOptionMustOn))
		{
			setOption(eOptionMustOn, true);
		}
	}
	foreach_(const GameOptionTypes& eOptionMustOff, info.getEnforcesGameOptionOffTypes())
	{
		if (isOption(eOptionMustOff))
		{
			setOption(eOptionMustOff, false);
		}
	}
}
