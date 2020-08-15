// game.cpp

#include "CvGameCoreDLL.h"
#include "CvGameAI.h"
#include "CvInitCore.h"
#include "CvMapGenerator.h"
#include "CvReplayMessage.h"
#include "CvReplayInfo.h"
#include "CvPlayerAI.h"
#include "CvTeamAI.h"

//	Koshling - save game compatibility between (most) builds
//	UI flag values in game serialization.  These are bitwise combinable
#define	GAME_SAVE_UI_FLAG_VALUE_AND_BASE		0x00000001

// Public Functions...

#pragma warning( disable : 4355 )
CvGame::CvGame()
: m_GameObject()
, m_Properties(this)
, m_iChokePointCalculationVersion(0)
, m_eCurrentMap(MAP_INITIAL)

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

namespace {
	void cacheInfoTypeEnums()
	{
		CvColorInfo::setGreen((ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"));
		CvColorInfo::setRed((ColorTypes)GC.getInfoTypeForString("COLOR_RED"));
		CvColorInfo::setYellow((ColorTypes)GC.getInfoTypeForString("COLOR_YELLOW"));
		CvColorInfo::setWhite((ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"));
		CvImprovementInfo::setImprovementCity((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_CITY"));
		CvImprovementInfo::setImprovementRuins((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_CITY_RUINS"));
		CvTerrainInfo::setTerrainPeak((TerrainTypes)GC.getInfoTypeForString("TERRAIN_PEAK"));
		CvTerrainInfo::setTerrainHill((TerrainTypes)GC.getInfoTypeForString("TERRAIN_HILL"));
		CvUnitCombatInfo::setUnitCombatAnimal((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_ANIMAL"));
		CvUnitCombatInfo::setUnitCombatWildAnimal((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_WILD"));
	}
}

void CvGame::init(HandicapTypes eHandicap)
{
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
	if (GC.getInitCore().getType() == GAME_SP_NEW ||
		GC.getInitCore().getType() == GAME_SP_SCENARIO)
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
		GameOptionTypes eGameOption = ((GameOptionTypes)iI);
		if (isOption(eGameOption))
		{
			if (GC.getGameOptionInfo(eGameOption).getNumEnforcesGameOptionOnTypes() > 0)
			{
				for (int iJ = 0; iJ < GC.getGameOptionInfo(eGameOption).getNumEnforcesGameOptionOnTypes(); iJ++)
				{
					GameOptionTypes eGameOptionMustOn = ((GameOptionTypes)GC.getGameOptionInfo(eGameOption).isEnforcesGameOptionOnType(iJ).eGameOption);
					if (GC.getGameOptionInfo(eGameOption).isEnforcesGameOptionOnType(iJ).bBool && !isOption(eGameOptionMustOn))
					{
						setOption(eGameOptionMustOn, true);
					}
				}
			}
			if (GC.getGameOptionInfo(eGameOption).getNumEnforcesGameOptionOffTypes() > 0)
			{
				for (int iJ = 0; iJ < GC.getGameOptionInfo(eGameOption).getNumEnforcesGameOptionOffTypes(); iJ++)
				{
					GameOptionTypes eGameOptionMustOff = ((GameOptionTypes)GC.getGameOptionInfo(eGameOption).isEnforcesGameOptionOffType(iJ).eGameOption);
					if (GC.getGameOptionInfo(eGameOption).isEnforcesGameOptionOffType(iJ).bBool && isOption(eGameOptionMustOff))
					{
						setOption(eGameOptionMustOff, false);
					}
				}
			}
		}
	}

	if (isOption(GAMEOPTION_LOCK_MODS))
	{
		if (isGameMultiPlayer())
		{
			setOption(GAMEOPTION_LOCK_MODS, false);
		}
		else
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
	}

	// Alberts2: Recalculate which info class replacements are currently active
	GC.updateReplacements();

	// Alberts2: cache higly used Types
	cacheInfoTypeEnums();

	//TB: Set Statuses
	setStatusPromotions();

	//establish improvement costs
	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		GC.getImprovementInfo((ImprovementTypes)iI).setHighestCost();
	}

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
	else
	{
		setEstimateEndTurn(getGameTurn() + getMaxTurns());
	}

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

	//resetOptionEdits();
	//setOptionEdits();

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

	setFutureEras();

	if (isOption(GAMEOPTION_UNITED_NATIONS))
	{
		//Find the diplomatic victory
		BuildingTypes eUnitedNations = NO_BUILDING;
		for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			if (GC.getBuildingInfo((BuildingTypes)iI).getVictoryPrereq() != NO_VICTORY)
			{
				if (GC.getBuildingInfo((BuildingTypes)iI).getVoteSourceType() != NO_VOTESOURCE)
				{
					eUnitedNations = (BuildingTypes)iI;
					break;
				}
			}
		}
		if (eUnitedNations != NO_BUILDING)
		{
			m_bDiploVictoryEnabled = isVictoryValid((VictoryTypes)GC.getBuildingInfo(eUnitedNations).getVictoryPrereq());
			setVictoryValid((VictoryTypes)GC.getBuildingInfo(eUnitedNations).getVictoryPrereq(), true);
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

	for (int iI = MAX_PC_PLAYERS; iI < MAX_PLAYERS; iI++)
	{
		PlayerTypes ePlayer = (PlayerTypes)iI;

		FAssertMsg(!GET_PLAYER(ePlayer).isAlive(), "NPC is not expected to be alive at this point of the game initialization.");

		if (ePlayer == BARBARIAN_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("BARBARIAN_LEADER"),
				(CivilizationTypes)GC.getDefineINT("BARBARIAN_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			TeamTypes eTeam = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getID();
			GET_TEAM(eTeam).init(eTeam);
			GC.getInitCore().setTeam(ePlayer, eTeam);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		else if (ePlayer == NEANDERTHAL_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("NEANDERTHAL_LEADER"),
				(CivilizationTypes)GC.getDefineINT("NEANDERTHAL_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			TeamTypes eTeam = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getID();
			GET_TEAM(eTeam).init(eTeam);
			GC.getInitCore().setTeam(ePlayer, eTeam);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		else if (ePlayer == BEAST_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("ANIMAL_LEADER"),
				(CivilizationTypes)GC.getDefineINT("BEAST_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			TeamTypes eTeam = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getID();
			GET_TEAM(eTeam).init(eTeam);
			GC.getInitCore().setTeam(ePlayer, eTeam);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		else if (ePlayer == PREDATOR_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("ANIMAL_LEADER"),
				(CivilizationTypes)GC.getDefineINT("PREDATOR_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			TeamTypes eTeam = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getID();
			GET_TEAM(eTeam).init(eTeam);
			GC.getInitCore().setTeam(ePlayer, eTeam);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		else if (ePlayer == PREY_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("ANIMAL_LEADER"),
				(CivilizationTypes)GC.getDefineINT("PREY_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			TeamTypes eTeam = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getID();
			GET_TEAM(eTeam).init(eTeam);
			GC.getInitCore().setTeam(ePlayer, eTeam);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		else if (ePlayer == INSECT_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("INSECT_LEADER"),
				(CivilizationTypes)GC.getDefineINT("INSECT_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			TeamTypes eTeam = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getID();
			GET_TEAM(eTeam).init(eTeam);
			GC.getInitCore().setTeam(ePlayer, eTeam);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		else if (ePlayer == NPC4_PLAYER)
		{
			break; // Remove me the day I'm added, shift it one "else" down per NPC added.
			// I will only be initialized when starting a new game, old saves won't ever see me.
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("NPC4_LEADER"),
				(CivilizationTypes)GC.getDefineINT("NPC4_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			TeamTypes eTeam = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getID();
			GET_TEAM(eTeam).init(eTeam);
			GC.getInitCore().setTeam(ePlayer, eTeam);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		else if (ePlayer == NPC3_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("NPC3_LEADER"),
				(CivilizationTypes)GC.getDefineINT("NPC3_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			TeamTypes eTeam = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getID();
			GET_TEAM(eTeam).init(eTeam);
			GC.getInitCore().setTeam(ePlayer, eTeam);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		else if (ePlayer == NPC2_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("NPC2_LEADER"),
				(CivilizationTypes)GC.getDefineINT("NPC2_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			TeamTypes eTeam = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getID();
			GET_TEAM(eTeam).init(eTeam);
			GC.getInitCore().setTeam(ePlayer, eTeam);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		else if (ePlayer == NPC1_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("NPC1_LEADER"),
				(CivilizationTypes)GC.getDefineINT("NPC1_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			TeamTypes eTeam = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getID();
			GET_TEAM(eTeam).init(eTeam);
			GC.getInitCore().setTeam(ePlayer, eTeam);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
		else if (ePlayer == NPC0_PLAYER)
		{
			addPlayer(
				ePlayer, (LeaderHeadTypes)GC.getDefineINT("NPC0_LEADER"),
				(CivilizationTypes)GC.getDefineINT("NPC0_CIVILIZATION"), false
			);
			GET_PLAYER(ePlayer).setNewPlayerAlive(true);
			TeamTypes eTeam = GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getID();
			GET_TEAM(eTeam).init(eTeam);
			GC.getInitCore().setTeam(ePlayer, eTeam);
			GC.getInitCore().setHandicap(ePlayer, (HandicapTypes)GC.getDefineINT("BARBARIAN_HANDICAP"));
		}
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
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)i);
		if (kPlayer.isAlive())
		{
			kPlayer.AI_updateFoundValues(true);
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


// BUG - MapFinder - start
// from HOF Mod - Dianthus
bool CvGame::canRegenerateMap() const
{
	if (isGameMultiPlayer() || getElapsedGameTurns() != 0 || GC.getInitCore().getWBMapScript())
	{
		return false;
	}
	return true;
}
// BUG - MapFinder - end

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
	Cy::call(PYBugModule, "gameStartSave");
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
/************************************************************************************************/
/* RevDCM	                  Start		 4/29/10                                                */
/*                                                                                              */
/* OC_LIMITED_RELIGIONS                                                                         */
/************************************************************************************************/
	SAFE_DELETE_ARRAY(m_abTechCanFoundReligion);
/************************************************************************************************/
/* LIMITED_RELIGIONS               END                                                          */
/************************************************************************************************/

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
		for (int i = 0; i < GC.getNumMapInfos(); i++)
		{
			if ( GC.mapInitialized((MapTypes)i) )
			{
				// Cast to the internal class
				CvMap&	map = GC.getMapByIndex((MapTypes)i);

				CvMapInitData defaultMapData;

				map.reset(&defaultMapData);
			}
		}

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
	m_iWaterAnimalSpawnChance = 0;
	m_iXResolution = 1024;
	m_iYResolution = 720;
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
			TechTypes eIndex = TechTypes(GC.getReligionInfo((ReligionTypes)iI).getTechPrereq());
			m_abTechCanFoundReligion[eIndex] = true;
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

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		GET_TEAM((TeamTypes)iI).meet(((TeamTypes)iI), false);

		if (GET_TEAM((TeamTypes)iI).isNPC() || GET_TEAM((TeamTypes)iI).isMinorCiv())
		{
			for (int iJ = 0; iJ < MAX_TEAMS; iJ++)
			{
				if (iI != iJ)
				{
					if (isOption(GAMEOPTION_PEACE_AMONG_NPCS) && GET_TEAM((TeamTypes)iI).isNPC() && GET_TEAM((TeamTypes)iJ).isNPC())
					{
						continue;
					}
					else
					{
						GET_TEAM((TeamTypes)iI).declareWar(((TeamTypes)iJ), false, NO_WARPLAN);
					}
				}
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
	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		for (int iJ = 0; iJ < MAX_TEAMS; iJ++)
		{
			if (GET_TEAM((TeamTypes)iJ).isAlive())
			{
				bool bValid = false;
				if (!bValid)
				{
					if ((GC.getHandicapInfo(getHandicapType()).isFreeTechs(iI)) ||
						  (!(GET_TEAM((TeamTypes)iJ).isHuman())&& GC.getHandicapInfo(getHandicapType()).isAIFreeTechs(iI)) ||
						  ((GC.getTechInfo((TechTypes)iI).getEra() < getStartEra()) && !GC.getTechInfo((TechTypes)iI).isDisable()))
					{
						bValid = true;
					}
				}

				if (!bValid)
				{
					for (int iK = 0; iK < MAX_PLAYERS; iK++)
					{
						if (GET_PLAYER((PlayerTypes)iK).isAlive() && GET_PLAYER((PlayerTypes)iK).getTeam() == iJ)
						{
							if (GC.getCivilizationInfo(GET_PLAYER((PlayerTypes)iK).getCivilizationType()).isCivilizationFreeTechs(iI))
							{
								bValid = true;
								break;
							}
						}
					}
				}

				GET_TEAM((TeamTypes)iJ).setHasTech(((TechTypes)iI), bValid, NO_PLAYER, false, false);
				if (bValid && GC.getTechInfo((TechTypes)iI).isMapVisible())
				{
					GC.getMap().setRevealedPlots((TeamTypes)iJ, true, true);
				}
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


void CvGame::assignStartingPlots()
{
	PROFILE_FUNC();
	{
		// Python override - Most mapscripts overide
		bool bAssignStartingPlots = false;
		if (Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "assignStartingPlots", bAssignStartingPlots)
		&& bAssignStartingPlots)
		{
			return;
		}
	}
	// Toffer - N.B.
// Scenarios with random players also use this code, and the scenario may define the starting plot for some arbitrary player slots; i.e. not all of them.
// P.S. I have no insight into why the code below is as convoluted as it is, so don't ask me.

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getStartingPlot() == NULL)
		{
			int iBestValue = 0;
			CvPlot* pBestPlot = NULL;

			for (int iJ = 0; iJ < GC.getMap().numPlots(); iJ++)
			{
				gDLL->callUpdater(); // allow window updates during launch

				CvPlot* pPlot = GC.getMap().plotByIndex(iJ);

				if (pPlot->isStartingPlot())
				{
					bool bValid = true;

					for (int iK = 0; iK < MAX_PC_PLAYERS; iK++)
					{
						if (GET_PLAYER((PlayerTypes)iK).isAlive() && GET_PLAYER((PlayerTypes)iK).getStartingPlot() == pPlot)
						{
							bValid = false;
							break;
						}
					}
					if (bValid)
					{
						const int iValue = 1 + getSorenRandNum(1000, "Starting Plot");
						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							pBestPlot = pPlot;
						}
					}
				}
			}
			if (pBestPlot != NULL)
			{
				GET_PLAYER((PlayerTypes)iI).setStartingPlot(pBestPlot, true);
			}
		}
	}
	std::vector<int> playerOrder;

	if (isTeamGame())
	{
		for (int iPass = 0; iPass < 2 * MAX_PLAYERS; ++iPass)
		{
			bool bStartFound = false;
			const int iRandOffset = getSorenRandNum(countCivTeamsAlive(), "Team Starting Plot");

			for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
			{
				const int iLoopTeam = ((iI + iRandOffset) % MAX_PC_TEAMS);

				if (GET_TEAM((TeamTypes)iLoopTeam).isAlive())
				{
					for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
					{
						CvPlayer& playerX = GET_PLAYER((PlayerTypes)iJ);

						if (playerX.isAlive() && playerX.getTeam() == iLoopTeam && playerX.getStartingPlot() == NULL)
						{
							CvPlot* pStartingPlot = playerX.findStartingPlot();

							if (NULL != pStartingPlot)
							{
								playerX.setStartingPlot(pStartingPlot, true);
								playerOrder.push_back(iJ);
							}
							bStartFound = true;
							break;
						}
					}
				}
			}
			if (!bStartFound)
			{
				break;
			}
		}
		// Check that all players have starting plots
		for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
		{
			FAssertMsg(!GET_PLAYER((PlayerTypes)iJ).isAlive() || GET_PLAYER((PlayerTypes)iJ).getStartingPlot() != NULL, "Player has no starting plot");
		}
	}
	else if (isGameMultiPlayer())
	{
		const int iRandOffset = getSorenRandNum(countCivPlayersAlive(), "Player Starting Plot");

		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			const int iPlayerX = (iI + iRandOffset) % MAX_PC_PLAYERS;
			CvPlayer& playerX = GET_PLAYER((PlayerTypes)iPlayerX);

			if (playerX.isAlive() && playerX.isHuman() && playerX.getStartingPlot() == NULL)
			{
				playerX.setStartingPlot(playerX.findStartingPlot(), true);
				playerOrder.push_back(iPlayerX);
			}
		}
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			CvPlayer& playerX = GET_PLAYER((PlayerTypes)iI);

			if (playerX.isAlive() && !playerX.isHuman() && playerX.getStartingPlot() == NULL)
			{
				playerX.setStartingPlot(playerX.findStartingPlot(), true);
				playerOrder.push_back(iI);
			}
		}
	}
	else
	{
		const int iHumanSlot = range((countCivPlayersAlive() - 1) * GC.getHandicapInfo(getHandicapType()).getStartingLocationPercent() / 100, 0, countCivPlayersAlive() - 1);

		for (int iI = 0; iI < iHumanSlot; iI++)
		{
			CvPlayer& playerX = GET_PLAYER((PlayerTypes)iI);

			if (playerX.isAlive() && !playerX.isHuman() && playerX.getStartingPlot() == NULL)
			{
				playerX.setStartingPlot(playerX.findStartingPlot(), true);
				playerOrder.push_back(iI);
			}
		}
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			CvPlayer& playerX = GET_PLAYER((PlayerTypes)iI);

			if (playerX.isAlive() && playerX.isHuman() && playerX.getStartingPlot() == NULL)
			{
				playerX.setStartingPlot(playerX.findStartingPlot(), true);
				playerOrder.push_back(iI);
			}
		}
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			CvPlayer& playerX = GET_PLAYER((PlayerTypes)iI);

			if (playerX.isAlive() && playerX.getStartingPlot() == NULL)
			{
				playerX.setStartingPlot(playerX.findStartingPlot(), true);
				playerOrder.push_back(iI);
			}
		}
	}
	// Now iterate over the player starts in the original order and re-place them.
	std::vector<int>::iterator iter;

	for (iter = playerOrder.begin(); iter != playerOrder.end(); ++iter)
	{
		GET_PLAYER((PlayerTypes)(*iter)).setStartingPlot(GET_PLAYER((PlayerTypes)(*iter)).findStartingPlot(), true);
	}
}

// Swaps starting locations until we have reached the optimal closeness between teams
// (caveat: this isn't quite "optimal" because we could get stuck in local minima, but it's pretty good)
void CvGame::normalizeStartingPlotLocations()
{
	CvPlot* apNewStartPlots[MAX_PC_PLAYERS];
	int* aaiDistances[MAX_PC_PLAYERS];
	int aiStartingLocs[MAX_PC_PLAYERS];
	int iI, iJ;

	// Precalculate distances between all starting positions:
	for (iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			gDLL->callUpdater();	// allow window to update during launch
			aaiDistances[iI] = new int[iI];
			for (iJ = 0; iJ < iI; iJ++)
			{
				aaiDistances[iI][iJ] = 0;
			}
			CvPlot *pPlotI = GET_PLAYER((PlayerTypes)iI).getStartingPlot();
			if (pPlotI != NULL)
			{
				for (iJ = 0; iJ < iI; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive())
					{
						CvPlot *pPlotJ = GET_PLAYER((PlayerTypes)iJ).getStartingPlot();
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
		else
		{
			aaiDistances[iI] = NULL;
		}
	}

	for (iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		aiStartingLocs[iI] = iI; // each player starting in own location
	}

	int iBestScore = getTeamClosenessScore(aaiDistances, aiStartingLocs);
	bool bFoundSwap = true;
	while (bFoundSwap)
	{
		bFoundSwap = false;
		for (iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				for (iJ = 0; iJ < iI; iJ++)
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

	for (iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		apNewStartPlots[iI] = NULL;
	}

	for (iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && aiStartingLocs[iI] != iI)
		{
			apNewStartPlots[iI] = GET_PLAYER((PlayerTypes)aiStartingLocs[iI]).getStartingPlot();
		}
	}

	for (iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && apNewStartPlots[iI] != NULL)
		{
			GET_PLAYER((PlayerTypes)iI).setStartingPlot(apNewStartPlots[iI], false);
		}
	}

	for (iI = 0; iI < MAX_PC_PLAYERS; iI++)
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
				const int iRange = 3;

				for (int iDX = -(iRange); iDX <= iRange; iDX++)
				{
					for (int iDY = -(iRange); iDY <= iRange; iDY++)
					{
						CvPlot* pLoopPlot = plotXY(pStartingPlot->getX(), pStartingPlot->getY(), iDX, iDY);

						if (pLoopPlot != NULL && pLoopPlot->isPeak())
						{
							pLoopPlot->setPlotType(PLOT_HILLS);
						}
					}
				}
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
	if (!GET_PLAYER(ePlayer).isAlive())
	{
		return NULL;
	}

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

	return NULL;
}


void CvGame::normalizeRemoveBadFeatures()
{
	CvPlot* pLoopPlot;
	int iI, iJ;

	for (iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			const CvPlot* pStartingPlot = GET_PLAYER((PlayerTypes)iI).getStartingPlot();

			if (pStartingPlot != NULL)
			{
				for (iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
				{
					pLoopPlot = plotCity(pStartingPlot->getX(), pStartingPlot->getY(), iJ);

					if (pLoopPlot != NULL)
					{
						if (pLoopPlot->getFeatureType() != NO_FEATURE)
						{
							if ((GC.getFeatureInfo(pLoopPlot->getFeatureType()).getYieldChange(YIELD_FOOD) <= 0) &&
								(GC.getFeatureInfo(pLoopPlot->getFeatureType()).getYieldChange(YIELD_PRODUCTION) <= 0))
							{
								pLoopPlot->setFeatureType(NO_FEATURE);
							}
						}
					}
				}

				int iX, iY;
				const int iCityRange = CITY_PLOTS_RADIUS;
				const int iExtraRange = 2;
				const int iMaxRange = iCityRange + iExtraRange;

				for (iX = -iMaxRange; iX <= iMaxRange; iX++)
				{
					for (iY = -iMaxRange; iY <= iMaxRange; iY++)
					{
						pLoopPlot = plotXY(pStartingPlot->getX(), pStartingPlot->getY(), iX, iY);
						if (pLoopPlot != NULL)
						{
							const int iDistance = plotDistance(pStartingPlot->getX(), pStartingPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY());
							if (iDistance <= iMaxRange)
							{
								if (pLoopPlot->getFeatureType() != NO_FEATURE)
								{
									if ((GC.getFeatureInfo(pLoopPlot->getFeatureType()).getYieldChange(YIELD_FOOD) <= 0) &&
										(GC.getFeatureInfo(pLoopPlot->getFeatureType()).getYieldChange(YIELD_PRODUCTION) <= 0))
									{
										if (pLoopPlot->isWater())
										{
											if (pLoopPlot->isAdjacentToLand() || (!(iDistance == iMaxRange) && (getSorenRandNum(2, "Remove Bad Feature") == 0)))
											{
												pLoopPlot->setFeatureType(NO_FEATURE);
											}
										}
										else
										{
											if (!(iDistance == iMaxRange) && (getSorenRandNum((2 + ((pLoopPlot->getBonusType() == NO_BONUS) ? 0 : 2)), "Remove Bad Feature") == 0))
											{
												pLoopPlot->setFeatureType(NO_FEATURE);
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


void CvGame::normalizeRemoveBadTerrain()
{
	const int iCityRange = CITY_PLOTS_RADIUS;
	const int iMaxRange = iCityRange + 1;

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (!GET_PLAYER((PlayerTypes)iI).isAlive())
			continue;

		const CvPlot* pStartingPlot = GET_PLAYER((PlayerTypes)iI).getStartingPlot();
		if (pStartingPlot == NULL)
			continue;

		for (int iX = -iMaxRange; iX <= iMaxRange; iX++)
		{
			for (int iY = -iMaxRange; iY <= iMaxRange; iY++)
			{
				CvPlot* pLoopPlot = plotXY(pStartingPlot->getX(), pStartingPlot->getY(), iX, iY);

				if (pLoopPlot == NULL)
					continue;

				const int iDistance = plotDistance(pStartingPlot->getX(), pStartingPlot->getY(), pLoopPlot->getX(), pLoopPlot->getY());

				if (iDistance <= iMaxRange && !pLoopPlot->isWater()
				&& (iDistance <= iCityRange || pLoopPlot->isCoastalLand() || 0 == getSorenRandNum(1 + iDistance - iCityRange, "Map Upgrade Terrain Food")))
				{
					const int iPlotFood = GC.getTerrainInfo(pLoopPlot->getTerrainType()).getYield(YIELD_FOOD);

					if (iPlotFood + GC.getTerrainInfo(pLoopPlot->getTerrainType()).getYield(YIELD_PRODUCTION) <= 1)
					{
						int iTargetFood;

						if (pLoopPlot->getBonusType(GET_PLAYER((PlayerTypes)iI).getTeam()) != NO_BONUS)
						{
							iTargetFood = 1;
						}
						else if (iPlotFood == 1 || iDistance <= iCityRange)
						{
							iTargetFood = 1 + getSorenRandNum(2, "Map Upgrade Terrain Food");
						}
						else
						{
							iTargetFood = pLoopPlot->isCoastalLand() ? 2 : 1;
						}

						for (int iK = 0; iK < GC.getNumTerrainInfos(); iK++)
						{
							if (!GC.getTerrainInfo((TerrainTypes)iK).isWaterTerrain()
							&& GC.getTerrainInfo((TerrainTypes)iK).getYield(YIELD_FOOD) >= iTargetFood
							&& GC.getTerrainInfo((TerrainTypes)iK).getYield(YIELD_FOOD) + GC.getTerrainInfo((TerrainTypes)iK).getYield(YIELD_PRODUCTION) == 2
							&& (pLoopPlot->getFeatureType() == NO_FEATURE || GC.getFeatureInfo(pLoopPlot->getFeatureType()).isTerrain(iK)))
							{
								pLoopPlot->setTerrainType((TerrainTypes)iK);
							}
						}
					}
				}
			}
		}
	}
}


void CvGame::normalizeAddFoodBonuses()
{
	bool bIgnoreLatitude = pythonIsBonusIgnoreLatitudes();

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			CvPlot* pStartingPlot = GET_PLAYER((PlayerTypes)iI).getStartingPlot();

			if (pStartingPlot != NULL)
			{
				int iFoodBonus = 0;
				int iGoodNatureTileCount = 0;

				for (int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
				{
					CvPlot* pLoopPlot = plotCity(pStartingPlot->getX(), pStartingPlot->getY(), iJ);

					if (pLoopPlot != NULL)
					{
						BonusTypes eBonus = pLoopPlot->getBonusType(GET_PLAYER((PlayerTypes)iI).getTeam());

						if (eBonus != NO_BONUS)
						{
							if (GC.getBonusInfo(eBonus).getYieldChange(YIELD_FOOD) > 0)
							{
								if ((GC.getBonusInfo(eBonus).getTechCityTrade() == NO_TECH) || (GC.getTechInfo((TechTypes)(GC.getBonusInfo(eBonus).getTechCityTrade())).getEra() <= getStartEra()))
								{
									if (pLoopPlot->isWater())
									{
										iFoodBonus += 2;
									}
									else
									{
										iFoodBonus += 3;
									}
								}
							}
							else if (pLoopPlot->calculateBestNatureYield(YIELD_FOOD, GET_PLAYER((PlayerTypes)iI).getTeam()) >= 2)
							{
								iGoodNatureTileCount++;
							}
						}
						else
						{
							if (pLoopPlot->calculateBestNatureYield(YIELD_FOOD, GET_PLAYER((PlayerTypes)iI).getTeam()) >= 3)
							{
								iGoodNatureTileCount++;
							}
						}
					}
				}

				int iTargetFoodBonusCount = 3;
				iTargetFoodBonusCount += (iGoodNatureTileCount == 0) ? 2 : 0;

				for (int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
				{
					if (iFoodBonus >= iTargetFoodBonusCount)
					{
						break;
					}

					CvPlot* pLoopPlot = plotCity(pStartingPlot->getX(), pStartingPlot->getY(), iJ);

					if (pLoopPlot != NULL)
					{
						if (pLoopPlot != pStartingPlot)
						{
							if (pLoopPlot->getBonusType() == NO_BONUS)
							{
								for (int iK = 0; iK < GC.getNumBonusInfos(); iK++)
								{
									if (GC.getBonusInfo((BonusTypes)iK).isNormalize())
									{
										if (GC.getBonusInfo((BonusTypes)iK).getYieldChange(YIELD_FOOD) > 0)
										{
											if ((GC.getBonusInfo((BonusTypes)iK).getTechCityTrade() == NO_TECH) || (GC.getTechInfo((TechTypes)(GC.getBonusInfo((BonusTypes)iK).getTechCityTrade())).getEra() <= getStartEra()))
											{
												if (GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasTech((TechTypes)(GC.getBonusInfo((BonusTypes)iK).getTechReveal())))
												{
													if (pLoopPlot->canHaveBonus(((BonusTypes)iK), bIgnoreLatitude))
													{
														pLoopPlot->setBonusType((BonusTypes)iK);
														if (pLoopPlot->isWater())
														{
															iFoodBonus += 2;
														}
														else
														{
															iFoodBonus += 3;
														}
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


void CvGame::normalizeAddGoodTerrain()
{
	CvPlot* pStartingPlot;
	CvPlot* pLoopPlot;
	bool bChanged;
	int iGoodPlot;
	int iI, iJ, iK;

	for (iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			pStartingPlot = GET_PLAYER((PlayerTypes)iI).getStartingPlot();

			if (pStartingPlot != NULL)
			{
				iGoodPlot = 0;

				for (iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
				{
					pLoopPlot = plotCity(pStartingPlot->getX(), pStartingPlot->getY(), iJ);

					if (pLoopPlot != NULL)
					{
						if (pLoopPlot != pStartingPlot)
						{
							if ((pLoopPlot->calculateNatureYield(YIELD_FOOD, GET_PLAYER((PlayerTypes)iI).getTeam()) >= GC.getFOOD_CONSUMPTION_PER_POPULATION()) &&
								  (pLoopPlot->calculateNatureYield(YIELD_PRODUCTION, GET_PLAYER((PlayerTypes)iI).getTeam()) > 0))
							{
								iGoodPlot++;
							}
						}
					}
				}

				for (iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
				{
					if (iGoodPlot >= 4)
					{
						break;
					}

					pLoopPlot = plotCity(pStartingPlot->getX(), pStartingPlot->getY(), iJ);

					if (pLoopPlot != NULL)
					{
						if (pLoopPlot != pStartingPlot)
						{
							if (!(pLoopPlot->isWater()))
							{
								if (!(pLoopPlot->isHills()))
								{
									if (pLoopPlot->getBonusType() == NO_BONUS)
									{
										bChanged = false;

										if (pLoopPlot->calculateNatureYield(YIELD_FOOD, GET_PLAYER((PlayerTypes)iI).getTeam()) < GC.getFOOD_CONSUMPTION_PER_POPULATION())
										{
											for (iK = 0; iK < GC.getNumTerrainInfos(); iK++)
											{
												if (!(GC.getTerrainInfo((TerrainTypes)iK).isWaterTerrain()))
												{
													if (GC.getTerrainInfo((TerrainTypes)iK).getYield(YIELD_FOOD) >= GC.getFOOD_CONSUMPTION_PER_POPULATION())
													{
														pLoopPlot->setTerrainType((TerrainTypes)iK);
														bChanged = true;
														break;
													}
												}
											}
										}

										if (pLoopPlot->calculateNatureYield(YIELD_PRODUCTION, GET_PLAYER((PlayerTypes)iI).getTeam()) == 0)
										{
											for (iK = 0; iK < GC.getNumFeatureInfos(); iK++)
											{
												if ((GC.getFeatureInfo((FeatureTypes)iK).getYieldChange(YIELD_FOOD) >= 0) &&
													  (GC.getFeatureInfo((FeatureTypes)iK).getYieldChange(YIELD_PRODUCTION) > 0))
												{
													if (GC.getFeatureInfo((FeatureTypes)iK).isTerrain(pLoopPlot->getTerrainType()))
													{
														pLoopPlot->setFeatureType((FeatureTypes)iK);
														bChanged = true;
														break;
													}
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
					}
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
			CvPlot* pStartingPlot = GET_PLAYER((PlayerTypes)iI).getStartingPlot();

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
					if (getSorenRandNum((iCount + 2), "Setting Feature Type") <= 1)
					{
						CvPlot* pLoopPlot = plotCity(pStartingPlot->getX(), pStartingPlot->getY(), aiShuffle[iJ]);

						if (pLoopPlot != NULL)
						{
							if (pLoopPlot != pStartingPlot)
							{
								if (pLoopPlot->getBonusType() == NO_BONUS)
								{
									if (pLoopPlot->getFeatureType() == NO_FEATURE)
									{
										for (int iK = 0; iK < GC.getNumFeatureInfos(); iK++)
										{
											if ((GC.getFeatureInfo((FeatureTypes)iK).getYieldChange(YIELD_FOOD) + GC.getFeatureInfo((FeatureTypes)iK).getYieldChange(YIELD_PRODUCTION)) > 0)
											{
												if (pLoopPlot->canHaveFeature((FeatureTypes)iK))
												{
													pLoopPlot->setFeatureType((FeatureTypes)iK);
													iCount++;
													break;
												}
											}
										}
									}

									iFeatureCount += (pLoopPlot->getFeatureType() != NO_FEATURE) ? 1 : 0;
								}
							}
						}
					}
				}

				int iCoastFoodCount = 0;
				int iOceanFoodCount = 0;
				int iOtherCount = 0;
				int iWaterCount = 0;
				for (iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
				{
					CvPlot* pLoopPlot = plotCity(pStartingPlot->getX(), pStartingPlot->getY(), iJ);
					if (pLoopPlot != NULL)
					{
						if (pLoopPlot != pStartingPlot)
						{
							if (pLoopPlot->isWater())
							{
								iWaterCount++;
								if (pLoopPlot->getBonusType() != NO_BONUS)
								{
									if (pLoopPlot->isAdjacentToLand())
									{
										iCoastFoodCount++;
									}
									else
									{
										iOceanFoodCount++;
									}
								}
							}
							else
							{
								if (pLoopPlot->getBonusType() != NO_BONUS)
								{
									iOtherCount++;
								}
							}
						}
					}
				}

				bool bLandBias = (iWaterCount > NUM_CITY_PLOTS / 2);

				shuffleArray(aiShuffle, NUM_CITY_PLOTS, getMapRand());

				for (iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
				{
					CvPlot* pLoopPlot = plotCity(pStartingPlot->getX(), pStartingPlot->getY(), aiShuffle[iJ]);

					if ((pLoopPlot != NULL) && (pLoopPlot != pStartingPlot))
					{
						if (getSorenRandNum(((bLandBias && pLoopPlot->isWater()) ? 2 : 1), "Placing Bonuses") == 0)
						{
							if ((iOtherCount * 3 + iOceanFoodCount * 2 + iCoastFoodCount * 2) >= 12)
							{
								break;
							}

							if (GET_PLAYER((PlayerTypes)iI).AI_foundValue(pStartingPlot->getX(), pStartingPlot->getY(), -1, true) >= iTargetValue)
							{
								break;
							}

							bool bCoast = (pLoopPlot->isWater() && pLoopPlot->isAdjacentToLand());
							bool bOcean = (pLoopPlot->isWater() && !bCoast);
							if ((pLoopPlot != pStartingPlot)
								&& !(bCoast && (iCoastFoodCount > 2))
								&& !(bOcean && (iOceanFoodCount > 2)))
							{
								for (int iPass = 0; iPass < 2; iPass++)
								{
									if (pLoopPlot->getBonusType() == NO_BONUS)
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
															if ((iPass == 0) ? CvMapGenerator::GetInstance().canPlaceBonusAt(((BonusTypes)iK), pLoopPlot->getX(), pLoopPlot->getY(), bIgnoreLatitude) : pLoopPlot->canHaveBonus(((BonusTypes)iK), bIgnoreLatitude))
															{
																pLoopPlot->setBonusType((BonusTypes)iK);
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

										if (bLandBias && !pLoopPlot->isWater() && pLoopPlot->getBonusType() == NO_BONUS)
										{
											if (((iFeatureCount > 4) && (pLoopPlot->getFeatureType() != NO_FEATURE))
												&& ((iCoastFoodCount + iOceanFoodCount) > 2))
											{
												if (getSorenRandNum(2, "Clear feature to add bonus") == 0)
												{
													pLoopPlot->setFeatureType(NO_FEATURE);

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
																	if ((iPass == 0) ? CvMapGenerator::GetInstance().canPlaceBonusAt(((BonusTypes)iK), pLoopPlot->getX(), pLoopPlot->getY(), bIgnoreLatitude) : pLoopPlot->canHaveBonus(((BonusTypes)iK), bIgnoreLatitude))
																	{
																		pLoopPlot->setBonusType((BonusTypes)iK);
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

					CvPlot* pLoopPlot = plotCity(pStartingPlot->getX(), pStartingPlot->getY(), aiShuffle[iJ]);

					if (pLoopPlot != NULL)
					{
						if (pLoopPlot != pStartingPlot)
						{
							if (pLoopPlot->getBonusType() == NO_BONUS)
							{
								if (pLoopPlot->getFeatureType() == NO_FEATURE)
								{
									for (iK = 0; iK < GC.getNumFeatureInfos(); iK++)
									{
										if ((GC.getFeatureInfo((FeatureTypes)iK).getYieldChange(YIELD_FOOD) + GC.getFeatureInfo((FeatureTypes)iK).getYieldChange(YIELD_PRODUCTION)) > 0)
										{
											if (pLoopPlot->canHaveFeature((FeatureTypes)iK))
											{
												pLoopPlot->setFeatureType((FeatureTypes)iK);
												break;
											}
										}
									}
								}
							}
						}
					}
				}

				int iHillsCount = 0;

				for (iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
				{
					CvPlot* pLoopPlot =plotCity(pStartingPlot->getX(), pStartingPlot->getY(), iJ);
					if (pLoopPlot != NULL)
					{
						if (pLoopPlot->isHills())
						{
							iHillsCount++;
						}
					}
				}
				shuffleArray(aiShuffle, NUM_CITY_PLOTS, getMapRand());
				for (iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
				{
					if (iHillsCount >= 3)
					{
						break;
					}
					CvPlot* pLoopPlot = plotCity(pStartingPlot->getX(), pStartingPlot->getY(), aiShuffle[iJ]);
					if (pLoopPlot != NULL)
					{
						if (!pLoopPlot->isWater())
						{
							if (!pLoopPlot->isHills())
							{
								if ((pLoopPlot->getFeatureType() == NO_FEATURE) ||
									!GC.getFeatureInfo(pLoopPlot->getFeatureType()).isRequiresFlatlands())
								{
									if ((pLoopPlot->getBonusType() == NO_BONUS) ||
										GC.getBonusInfo(pLoopPlot->getBonusType()).isHills())
									{
										pLoopPlot->setPlotType(PLOT_HILLS, false, true);
										iHillsCount++;
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


void CvGame::normalizeStartingPlots()
{
	PROFILE_FUNC();

	if ((!GC.getInitCore().getWBMapScript() || GC.getInitCore().getWBMapNoPlayers())
	&& !Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "normalizeStartingPlotLocations")
	&& !isOption(GAMEOPTION_CULTURALLY_LINKED_STARTS))
	{
		normalizeStartingPlotLocations();
	}

	if (GC.getInitCore().getWBMapScript())
	{
		return;
	}

	if (!Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "normalizeAddRiver")  || gDLL->getPythonIFace()->pythonUsingDefaultImpl())
	{
		normalizeAddRiver();
	}

	if (!Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "normalizeRemovePeaks"))
	{
		normalizeRemovePeaks();
	}

	if (!Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "normalizeAddLakes"))
	{
		normalizeAddLakes();
	}

	if (!Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "normalizeRemoveBadFeatures"))
	{
		normalizeRemoveBadFeatures();
	}

	if (!Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "normalizeRemoveBadTerrain"))
	{
		normalizeRemoveBadTerrain();
	}

	if (!Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "normalizeAddFoodBonuses"))
	{
		normalizeAddFoodBonuses();
	}

	if (!Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "normalizeAddGoodTerrain"))
	{
		normalizeAddGoodTerrain();
	}

	if (!Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "normalizeAddExtras"))
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
			for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; iPlayer++)
			{
				if (GET_PLAYER((PlayerTypes)iPlayer).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iPlayer).getTeam() == (TeamTypes)iTeam)
					{
						for (int iOtherPlayer = 0; iOtherPlayer < iPlayer; iOtherPlayer++)
						{
							if (GET_PLAYER((PlayerTypes)iOtherPlayer).getTeam() == (TeamTypes)iTeam)
							{
								// Add the edge between these two players that are on the same team
								iNumEdges++;
								int iPlayerStart = aiStartingLocs[iPlayer];
								int iOtherPlayerStart = aiStartingLocs[iOtherPlayer];

								if (iPlayerStart < iOtherPlayerStart) // Make sure that iPlayerStart > iOtherPlayerStart
								{
									int iTemp = iPlayerStart;
									iPlayerStart = iOtherPlayerStart;
									iOtherPlayerStart = iTemp;
								}
								else if (iPlayerStart == iOtherPlayerStart)
								{
									FAssertMsg(false, "Two players are (hypothetically) assigned to the same starting location!");
								}
								iTeamTotalDist += aaiDistances[iPlayerStart][iOtherPlayerStart];
							}
						}
					}
				}
			}

			int iTeamScore;
			if (iNumEdges == 0)
			{
				iTeamScore = 0;
			}
			else
			{
				iTeamScore = iTeamTotalDist/iNumEdges; // the avg distance between team edges is the team score
			}

			iScore += iTeamScore;
		}
	}
	return iScore;
}

void CvGame::update()
{
	MEMORY_TRACE_FUNCTION();

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
			if ( GET_PLAYER((PlayerTypes)iI).isAlive() )
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

		for (int iI= 0; iI < NUM_GAMEOPTION_TYPES; iI++)
		{
			GameOptionTypes eGameOption = ((GameOptionTypes)iI);
			if (isOption(eGameOption))
			{
				if (GC.getGameOptionInfo(eGameOption).getNumEnforcesGameOptionOnTypes() > 0)
				{
					for (int iJ = 0; iJ < GC.getGameOptionInfo(eGameOption).getNumEnforcesGameOptionOnTypes(); iJ++)
					{
						GameOptionTypes eGameOptionMustOn = ((GameOptionTypes)GC.getGameOptionInfo(eGameOption).isEnforcesGameOptionOnType(iJ).eGameOption);
						if (GC.getGameOptionInfo(eGameOption).isEnforcesGameOptionOnType(iJ).bBool && !isOption(eGameOptionMustOn))
						{
							setOption(eGameOptionMustOn, true);
						}
					}
				}
				if (GC.getGameOptionInfo(eGameOption).getNumEnforcesGameOptionOffTypes() > 0)
				{
					for (int iJ = 0; iJ < GC.getGameOptionInfo(eGameOption).getNumEnforcesGameOptionOffTypes(); iJ++)
					{
						GameOptionTypes eGameOptionMustOff = ((GameOptionTypes)GC.getGameOptionInfo(eGameOption).isEnforcesGameOptionOffType(iJ).eGameOption);
						if (GC.getGameOptionInfo(eGameOption).isEnforcesGameOptionOffType(iJ).bBool && isOption(eGameOptionMustOff))
						{
							setOption(eGameOptionMustOff, false);
						}
					}
				}
			}
		}

		// Alberts2: cache higly used Types
		cacheInfoTypeEnums();

		//TB: Set Statuses
		setStatusPromotions();

		//	Super forts adaptation to C2C - make sure this map has had
		//	its choke points calculated - note we check this every turn
		//	because of a future intent to force periodic relcalulation
		//	on significant events (discovery of mountaineering by someone,
		//	terra-forming leading to water<->land transformations, etc.)
		ensureChokePointsEvaluated();
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
	CvPlayerAI& kActivePlayer = GET_PLAYER(getActivePlayer());
	if ( (!kActivePlayer.isTurnActive() || kActivePlayer.isAutoMoves()) && !kActivePlayer.hasBusyUnit() && !isGameMultiPlayer() &&
		 getBugOptionBOOL("MainInterface__MinimizeAITurnSlices", false) )
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

	MEMORY_TRACE_FUNCTION();

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

	CLLNode<IDInfo>* pEntityNode;
	CvSelectionGroup* pSelectionGroup;
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
		pSelectionGroup = pUnit->getGroup();

		gDLL->getInterfaceIFace()->selectionListPreChange();

		pEntityNode = pSelectionGroup->headUnitNode();

		while (pEntityNode != NULL)
		{
			FAssertMsg(::getUnit(pEntityNode->m_data), "null entity in selection group");
			gDLL->getInterfaceIFace()->insertIntoSelectionList(::getUnit(pEntityNode->m_data), false, bToggle, bGroup, bSound, true);

			pEntityNode = pSelectionGroup->nextUnitNode(pEntityNode);
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

	bool bGroup;

	FAssertMsg(pUnit != NULL, "pUnit == NULL unexpectedly");

	if (bAlt || bCtrl)
	{
		gDLL->getInterfaceIFace()->clearSelectedCities();

		if (!bShift)
		{
			gDLL->getInterfaceIFace()->clearSelectionList();
			bGroup = true;
		}
		else
		{
			bGroup = gDLL->getInterfaceIFace()->mirrorsSelectionGroup();
		}

		CvPlot* pUnitPlot = pUnit->plot();

		CLLNode<IDInfo>* pUnitNode = pUnitPlot->headUnitNode();

		gDLL->getInterfaceIFace()->selectionListPreChange();

		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = pUnitPlot->nextUnitNode(pUnitNode);

			if (pLoopUnit->getOwner() == getActivePlayer())
			{
				if (pLoopUnit->canMove())
				{
					if (!isMPOption(MPOPTION_SIMULTANEOUS_TURNS) || getTurnSlice() - pLoopUnit->getLastMoveTurn() > GC.getMIN_TIMER_UNIT_DOUBLE_MOVES())
					{
						if (bAlt || (pLoopUnit->getUnitType() == pUnit->getUnitType()))
						{
							gDLL->getInterfaceIFace()->insertIntoSelectionList(pLoopUnit, false, false, bGroup, false, true);
						}
					}
				}
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
	CvUnit* pSelectUnit = NULL;

	if (pPlot != NULL)
	{
		CvUnit* pCenterUnit = pPlot->getDebugCenterUnit();

		if ((pCenterUnit != NULL) && (pCenterUnit->getOwner() == getActivePlayer()))
		{
			pSelectUnit = pCenterUnit;
		}
	}

	if (pSelectUnit != NULL)
	{
		gDLL->getInterfaceIFace()->selectGroup(pSelectUnit, false, false, true);
	}
}


bool CvGame::selectionListIgnoreBuildingDefense() const
{
	PROFILE_FUNC();

	bool bIgnoreBuilding = false;
	bool bAttackLandUnit = false;

	CLLNode<IDInfo>* pSelectedUnitNode = gDLL->getInterfaceIFace()->headSelectionListNode();

	while (pSelectedUnitNode != NULL)
	{
		CvUnit* pSelectedUnit = ::getUnit(pSelectedUnitNode->m_data);
		pSelectedUnitNode = gDLL->getInterfaceIFace()->nextSelectionListNode(pSelectedUnitNode);

		if (pSelectedUnit != NULL)
		{
			if (pSelectedUnit->ignoreBuildingDefense())
			{
				bIgnoreBuilding = true;
			}

			if ((pSelectedUnit->getDomainType() == DOMAIN_LAND) && pSelectedUnit->canAttack())
			{
				bAttackLandUnit = true;
			}
		}
	}

	if (!bIgnoreBuilding && !bAttackLandUnit)
	{
		if (getBestLandUnit() != NO_UNIT)
		{
			bIgnoreBuilding = GC.getUnitInfo(getBestLandUnit()).isIgnoreBuildingDefense();
		}
	}

	return bIgnoreBuilding;
}


void CvGame::implementDeal(PlayerTypes eWho, PlayerTypes eOtherWho, CLinkList<TradeData>* pOurList, CLinkList<TradeData>* pTheirList, bool bForce)
{
	FAssertMsg(eWho != NO_PLAYER, "Who is not assigned a valid value");
	FAssertMsg(eOtherWho != NO_PLAYER, "OtherWho is not assigned a valid value");
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

	for(CvDeal* pLoopDeal = firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = nextDeal(&iLoop))
	{
		pLoopDeal->verify();
	}
}


/* Globeview configuration control:
If bStarsVisible, then there will be stars visible behind the globe when it is on
If bWorldIsRound, then the world will bend into a globe; otherwise, it will show up as a plane  */
void CvGame::getGlobeviewConfigurationParameters(TeamTypes eTeam, bool& bStarsVisible, bool& bWorldIsRound)
{
	if(GET_TEAM(eTeam).isMapCentering() || getCircumnavigatedTeam() != NO_TEAM)
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
	if (NO_HURRY == eHurry)
	{
		return 0;
	}
	return (GC.getHurryInfo(eHurry).getProductionPerPopulation() * 100) / std::max(1, GC.getGameSpeedInfo(getGameSpeedType()).getHurryPercent());
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

	return (iExtraEligible > 0);
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
	return ((countPossibleVote(eVote, eVoteSource) * GC.getVoteInfo(eVote).getPopulationThreshold()) / 100);
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
					if (kLoopPlayer.isAlive())
					{
						if (kLoopPlayer.getBuildingCount((BuildingTypes)iBuilding) > 0)
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
	}
	else
	{
		for (int iI = 0; iI < GC.getNumVoteInfos(); iI++)
		{
			if (GC.getVoteInfo((VoteTypes)iI).isVoteSourceType(eVoteSource))
			{
				if (GC.getVoteInfo((VoteTypes)iI).isSecretaryGeneral())
				{
					if (isVotePassed((VoteTypes)iI))
					{
						return ((TeamTypes)(getVoteOutcome((VoteTypes)iI)));
					}
				}
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
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iCount++;
		}
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
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			iCount++;
		}
	}

	return iCount;
}


int CvGame::countCivTeamsEverAlive() const
{
	bool	teamsAlive[MAX_PC_TEAMS]; // very hard to say if this should be PC or CIV but I'm betting PC.
	int		iCount = 0;

	memset(teamsAlive,0,sizeof(bool)*MAX_PC_TEAMS);

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iI);
		if (kPlayer.isEverAlive() && kPlayer.getParent() == NO_PLAYER)
		{
			FAssert(0 <= kPlayer.getTeam());
			FAssert(MAX_PC_TEAMS > kPlayer.getTeam());

			if ( !teamsAlive[kPlayer.getTeam()] )
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

/************************************************************************************************/
/* REVOLUTION_MOD                         02/01/08                                jdog5000      */
/*                                                                                              */
/* Allow humans to control barbs                                                                */
/************************************************************************************************/
/* original code
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
*/
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
	{
		const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iI);
		if (kPlayer.isAlive() && kPlayer.isHuman())
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
		const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iI);
		if (kPlayer.isAlive())
		{
			iCount += kPlayer.getPower();
		}
	}

	return iCount;
}


int CvGame::countTotalNukeUnits() const
{
	int iCount = 0;
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iI);
		if (kPlayer.isAlive())
		{
			iCount += kPlayer.getNumNukeUnits();
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
	else
	{
		return GC.getBuildingInfo(eBuilding).getNumFreeBonuses();
	}
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
		const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.isAlive())
		{
			foreach_(CvCity* pLoopCity, kLoopPlayer.cities())
			{
				if (pLoopCity->isHasCorporation(eCorporation1))
				{
					pLoopCity->setHasCorporation(eCorporation1, false, false, false);
					pLoopCity->setHasCorporation(eCorporation2, true, true);
				}
			}

			foreach_(CvUnit* pLoopUnit, kLoopPlayer.units())
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
		const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kLoopPlayer.isAlive())
		{
			foreach_(const CvCity* pLoopCity, kLoopPlayer.cities())
			{
				if (pLoopCity->isHasReligion(eReligion))
				{
					iCount += ((pLoopCity->getPopulation() + (pLoopCity->getReligionCount() / 2)) / pLoopCity->getReligionCount());
				}
			}
		}
	}

	return ((iCount * 100) / getTotalPopulation());
}


int CvGame::goldenAgeLength() const
{
	int iLength = GC.getGOLDEN_AGE_LENGTH();

	iLength *= GC.getGameSpeedInfo(getGameSpeedType()).getGoldenAgePercent();
	iLength /= 100;

	return iLength;
}

int CvGame::victoryDelay(VictoryTypes eVictory) const
{
	FAssert(eVictory >= 0 && eVictory < GC.getNumVictoryInfos());

	int iLength = GC.getVictoryInfo(eVictory).getVictoryDelayTurns();

	iLength *= GC.getGameSpeedInfo(getGameSpeedType()).getVictoryDelayPercent();
	iLength /= 100;

	return iLength;
}



int CvGame::getImprovementUpgradeTime(ImprovementTypes eImprovement) const
{
	int iTime = GC.getImprovementInfo(eImprovement).getUpgradeTime();
	if (iTime < 1) return 0;

	iTime *= GC.getGameSpeedInfo(getGameSpeedType()).getImprovementPercent();
	iTime /= 100;

	iTime *= GC.getEraInfo(getCurrentEra()).getImprovementPercent();
	iTime /= 100;

	return std::max(1, iTime);
}


bool CvGame::canTrainNukes() const
{
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kPlayer.isAlive())
		{
			for (int iJ = 0; iJ < GC.getNumUnitInfos(); iJ++)
			{
				if (GC.getUnitInfo((UnitTypes)iJ).getNukeRange() != -1 && kPlayer.canTrain((UnitTypes)iJ))
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
	return (isNetworkMultiPlayer() || isPbem() || isHotSeat());
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
	{
		gDLL->ChangeINIKeyValue("CONFIG", "Bandwidth", "modem");
	}
	else
	{
		gDLL->ChangeINIKeyValue("CONFIG", "Bandwidth", "broadband");
	}

	gDLL->SetModem(bModem);
}
/************************************************************************************************/
/* REVOLUTION_MOD                                                                 lemmy101      */
/*                                                                                jdog5000      */
/*                                                                                              */
/************************************************************************************************/
void CvGame::reviveActivePlayer()
{
	if (!(GET_PLAYER(getActivePlayer()).isAlive()))
	{
		if(isForcedAIAutoPlay(getActivePlayer()))
		{
			setForcedAIAutoPlay(getActivePlayer(), 0);
		} else
		{
			setAIAutoPlay(getActivePlayer(), 0);
		}

		GC.getInitCore().setSlotStatus(getActivePlayer(), SS_TAKEN);

		// Let Python handle it
		if (Cy::call<bool>(PYGameModule, "doReviveActivePlayer", Cy::Args() << getActivePlayer()))
		{
			return;
		}

		GET_PLAYER(getActivePlayer()).initUnit(((UnitTypes)0), 0, 0, NO_UNITAI, NO_DIRECTION, 0);
	}
}

void CvGame::reviveActivePlayer(PlayerTypes iPlayer)
{
	if (!(GET_PLAYER(iPlayer).isAlive()))
	{
		if(isForcedAIAutoPlay(iPlayer))
		{
			setForcedAIAutoPlay(iPlayer, 0);
		} else
		{
			setAIAutoPlay(iPlayer, 0);
		}

		GC.getInitCore().setSlotStatus(iPlayer, SS_TAKEN);

		// Let Python handle it
		if (Cy::call<bool>(PYGameModule, "doReviveActivePlayer", Cy::Args() << iPlayer))
		{
			return;
		}

		GET_PLAYER(iPlayer).initUnit(((UnitTypes)0), 0, 0, NO_UNITAI, NO_DIRECTION, 0);
	}
}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/


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
		FAssert(getGameTurn() >= 0);

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
	FAssert(getMaxTurns() >= 0);
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
	FAssert(getMaxCityElimination() >= 0);
}

int CvGame::getNumAdvancedStartPoints() const
{
	return GC.getInitCore().getNumAdvancedStartPoints();
}


void CvGame::setNumAdvancedStartPoints(int iNewValue)
{
	GC.getInitCore().setNumAdvancedStartPoints(iNewValue);
	FAssert(getNumAdvancedStartPoints() >= 0);
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


int CvGame::getTurnSlice() const
{
	return m_iTurnSlice;
}


int CvGame::getMinutesPlayed() const
{
	return (getTurnSlice() / gDLL->getTurnsPerMinute());
}


void CvGame::setTurnSlice(int iNewValue)
{
	m_iTurnSlice = iNewValue;
}


void CvGame::changeTurnSlice(int iChange)
{
	PROFILE_FUNC();

	setTurnSlice(getTurnSlice() + iChange);
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
	return (getCutoffSlice() - getTurnSlice());
}


void CvGame::resetTurnTimer()
{
	// We should only use the turn timer if we are in multiplayer
	if (isMPOption(MPOPTION_TURN_TIMER))
	{
		if (getElapsedGameTurns() > 0 || !isOption(GAMEOPTION_ADVANCED_START))
		{
			// Determine how much time we should allow
			int iTurnLen = getMaxTurnLen();
			if (getElapsedGameTurns() == 0 && !isPitboss())
			{
				// Let's allow more time for the initial turn
				const TurnTimerTypes eTurnTimer = GC.getInitCore().getTurnTimer();
				FAssertMsg(eTurnTimer >= 0 && eTurnTimer < GC.getNumTurnTimerInfos(), "Invalid TurnTimer selection in InitCore");
				iTurnLen = (iTurnLen * GC.getTurnTimerInfo(eTurnTimer).getFirstTurnMultiplier());
			}
			// Set the current turn slice to start the 'timer'
			setCutoffSlice(getTurnSlice() + iTurnLen);
		}
	}
}

void CvGame::incrementTurnTimer(int iNumTurnSlices)
{
	if (isMPOption(MPOPTION_TURN_TIMER))
	{
		// If the turn timer has expired, we shouldn't increment it as we've sent our turn complete message
		if (getTurnSlice() <= getCutoffSlice())
		{
			changeCutoffSlice(iNumTurnSlices);
		}
	}
}


int CvGame::getMaxTurnLen() const
{
	if (isPitboss())
	{
		// Use the user provided input
		// Turn time is in hours
		return ( getPitbossTurnTime() * 3600 * 4);
	}
	else
	{
		int iMaxUnits = 0;
		int iMaxCities = 0;

		// Find out who has the most units and who has the most cities
		// Calculate the max turn time based on the max number of units and cities
		for (int i = 0; i < MAX_PC_PLAYERS; ++i)
		{
			if (GET_PLAYER((PlayerTypes)i).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)i).getNumUnits() > iMaxUnits)
				{
					iMaxUnits = GET_PLAYER((PlayerTypes)i).getNumUnits();
				}
				if (GET_PLAYER((PlayerTypes)i).getNumCities() > iMaxCities)
				{
					iMaxCities = GET_PLAYER((PlayerTypes)i).getNumCities();
				}
			}
		}

		// Now return turn len based on base len and unit and city bonuses
		const TurnTimerTypes eTurnTimer = GC.getInitCore().getTurnTimer();
		FAssertMsg(eTurnTimer >= 0 && eTurnTimer < GC.getNumTurnTimerInfos(), "Invalid TurnTimer Selection in InitCore");
		return ( GC.getTurnTimerInfo(eTurnTimer).getBaseTime() +
				(GC.getTurnTimerInfo(eTurnTimer).getCityBonus()*iMaxCities) +
				(GC.getTurnTimerInfo(eTurnTimer).getUnitBonus()*iMaxUnits) );
	}
}


int CvGame::getTargetScore() const
{
	return GC.getInitCore().getTargetScore();
}


void CvGame::setTargetScore(int iNewValue)
{
	GC.getInitCore().setTargetScore(iNewValue);
	FAssert(getTargetScore() >= 0);
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
	m_iNumGameTurnActive = (m_iNumGameTurnActive + iChange);
	FAssert(getNumGameTurnActive() >= 0);
}


int CvGame::getNumCities() const
{
	return m_iNumCities;
}


int CvGame::getNumCivCities() const
{
	int iAnswer = getNumCities();
	iAnswer -= GET_PLAYER(BARBARIAN_PLAYER).getNumCities();
	iAnswer -= GET_PLAYER(NEANDERTHAL_PLAYER).getNumCities();
	return iAnswer;
}


void CvGame::changeNumCities(int iChange)
{
	m_iNumCities = (m_iNumCities + iChange);
	FAssert(getNumCities() >= 0);
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
	m_iTotalPopulation = (m_iTotalPopulation + iChange);
	FAssert(getTotalPopulation() >= 0);
}


int CvGame::getTradeRoutes() const
{
	return m_iTradeRoutes;
}


void CvGame::changeTradeRoutes(int iChange)
{
	if (iChange != 0)
	{
		m_iTradeRoutes = (m_iTradeRoutes + iChange);
		FAssert(getTradeRoutes() >= 0);

		updateTradeRoutes();
	}
}


int CvGame::getFreeTradeCount() const
{
	return m_iFreeTradeCount;
}


bool CvGame::isFreeTrade() const
{
	return (getFreeTradeCount() > 0);
}


void CvGame::changeFreeTradeCount(int iChange)
{
	if (iChange != 0)
	{
		const bool bOldFreeTrade = isFreeTrade();

		m_iFreeTradeCount = (m_iFreeTradeCount + iChange);
		FAssert(getFreeTradeCount() >= 0);

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
	return (isOption(GAMEOPTION_NO_NUKES) || getNoNukesCount() > 0);
}


void CvGame::changeNoNukesCount(int iChange)
{
	m_iNoNukesCount = (m_iNoNukesCount + iChange);
	FAssert(getNoNukesCount() >= 0);
}


int CvGame::getSecretaryGeneralTimer(VoteSourceTypes eVoteSource) const
{
	FAssert(eVoteSource >= 0);
	FAssert(eVoteSource < GC.getNumVoteSourceInfos());
	return m_aiSecretaryGeneralTimer[eVoteSource];
}


void CvGame::setSecretaryGeneralTimer(VoteSourceTypes eVoteSource, int iNewValue)
{
	FAssert(eVoteSource >= 0);
	FAssert(eVoteSource < GC.getNumVoteSourceInfos());
	m_aiSecretaryGeneralTimer[eVoteSource] = iNewValue;
	FAssert(getSecretaryGeneralTimer(eVoteSource) >= 0);
}


void CvGame::changeSecretaryGeneralTimer(VoteSourceTypes eVoteSource, int iChange)
{
	setSecretaryGeneralTimer(eVoteSource, getSecretaryGeneralTimer(eVoteSource) + iChange);
}


int CvGame::getVoteTimer(VoteSourceTypes eVoteSource) const
{
	FAssert(eVoteSource >= 0);
	FAssert(eVoteSource < GC.getNumVoteSourceInfos());
	return m_aiVoteTimer[eVoteSource];
}


void CvGame::setVoteTimer(VoteSourceTypes eVoteSource, int iNewValue)
{
	FAssert(eVoteSource >= 0);
	FAssert(eVoteSource < GC.getNumVoteSourceInfos());
	m_aiVoteTimer[eVoteSource] = iNewValue;
	FAssert(getVoteTimer(eVoteSource) >= 0);
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
	m_iNukesExploded = (m_iNukesExploded + iChange);
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
	m_iMaxPopulation = getPopulationScore(iMaxFood / std::max(1, GC.getFOOD_CONSUMPTION_PER_POPULATION()));
	m_iMaxLand = getLandPlotsScore(GC.getMap().getLandPlots());
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
		m_iInitPopulation = getPopulationScore(iNumSettlers * (GC.getEraInfo(getStartEra()).getFreePopulation() + 1));
		m_iInitLand = getLandPlotsScore(iNumSettlers *  NUM_CITY_PLOTS);
	}
	else
	{
		m_iInitPopulation = 0;
		m_iInitLand = 0;
	}

	m_iInitTech = 0;
	for (int i = 0; i < GC.getNumTechInfos(); i++)
	{
		if (GC.getTechInfo((TechTypes)i).getEra() < getStartEra())
		{
			m_iInitTech += getTechScore((TechTypes)i);
		}
		else
		{
			// count all possible free techs as initial to lower the score from immediate retirement
			for (int iCiv = 0; iCiv < GC.getNumCivilizationInfos(); iCiv++)
			{
				if (GC.getCivilizationInfo((CivilizationTypes)iCiv).isPlayable())
				{
					if (GC.getCivilizationInfo((CivilizationTypes)iCiv).isCivilizationFreeTechs(i))
					{
						m_iInitTech += getTechScore((TechTypes)i);
						break;
					}
				}
			}
		}
	}
	m_iInitWonders = 0;
}

/************************************************************************************************/
/* REVOLUTION_MOD                                                                  lemmy101     */
/*                                                                                 jdog5000     */
/*                                                                                              */
/************************************************************************************************/
int CvGame::getAIAutoPlay(PlayerTypes iPlayer) const
{
	return m_iAIAutoPlay[iPlayer];
}

void CvGame::setAIAutoPlay(PlayerTypes iPlayer, int iNewValue, bool bForced)
{
	FAssert(iNewValue >= 0);
	if(isForcedAIAutoPlay(iPlayer) && !bForced)
	{
		return;
	}

	if (GC.getLogging() )
	{
		TCHAR szOut[1024];
		sprintf(szOut, "setAutoPlay called for player %d - set to: %d\n", iPlayer, iNewValue);
		gDLL->messageControlLog(szOut);
	}

	const int iOldValue = getAIAutoPlay(iPlayer);

	if (iOldValue != iNewValue)
	{
		m_iAIAutoPlay[iPlayer] = std::max(0, iNewValue);

		if (iNewValue > 0)
		{
			GET_PLAYER(iPlayer).setHumanDisabled(true);
		} else
		{
			GET_PLAYER(iPlayer).setHumanDisabled(false);
		}
	}
}

void CvGame::setAIAutoPlayExternal(int iNewValue)
{
	const PlayerTypes iPlayer = getActivePlayer();
	return setAIAutoPlay(iPlayer, iNewValue);
}

void CvGame::changeAIAutoPlay(PlayerTypes iPlayer, int iChange)
{
	setAIAutoPlay(iPlayer, (getAIAutoPlay(iPlayer) + iChange));
}


bool CvGame::isForcedAIAutoPlay(PlayerTypes iPlayer) const
{
	FAssert(getForcedAIAutoPlay(iPlayer) >= 0)

	return (getForcedAIAutoPlay(iPlayer) > 0);
}

int CvGame::getForcedAIAutoPlay(PlayerTypes iPlayer) const
{
	return m_iForcedAIAutoPlay[iPlayer];
}

void CvGame::setForcedAIAutoPlay(PlayerTypes iPlayer, int iNewValue, bool bForced)
{
	FAssert(iNewValue >= 0);

	if(bForced == true)
	{
		const int iOldValue = getForcedAIAutoPlay(iPlayer);

		if (iOldValue != iNewValue)
		{
			m_iForcedAIAutoPlay[iPlayer] = std::max(0, iNewValue);
			setAIAutoPlay(iPlayer, iNewValue, true);
		} else
		{
			setAIAutoPlay(iPlayer, iNewValue, true);
		}
	} else
	{
		m_iForcedAIAutoPlay[iPlayer] = 0;

		const int iOldValue = m_iAIAutoPlay[iPlayer];

		if(iOldValue != iNewValue)
		{
			setAIAutoPlay(iPlayer, iNewValue);
		}
	}
}

void CvGame::changeForcedAIAutoPlay(PlayerTypes iPlayer, int iChange)
{
	if(isForcedAIAutoPlay(iPlayer))
	{
		setForcedAIAutoPlay(iPlayer, (getAIAutoPlay(iPlayer) + iChange), true);
	} else
	{
		setForcedAIAutoPlay(iPlayer, (getAIAutoPlay(iPlayer) + iChange));
	}
}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/


// < M.A.D. Nukes Start >
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
// < M.A.D. Nukes End   >

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
	if (getCircumnavigatedTeam() != NO_TEAM
	|| getElapsedGameTurns() < 1)
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
	return (getDiploVoteCount(eVoteSource) > 0);
}


int CvGame::getDiploVoteCount(VoteSourceTypes eVoteSource) const
{
	FAssert(eVoteSource >= 0 && eVoteSource < GC.getNumVoteSourceInfos());
	return m_aiDiploVote[eVoteSource];
}

void CvGame::changeDiploVote(VoteSourceTypes eVoteSource, int iChange)
{
	FAssert(eVoteSource >= 0 && eVoteSource < GC.getNumVoteSourceInfos());

	if (0 != iChange)
	{
		if ( !m_bRecalculatingModifiers )
		{
			for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
			{
				GET_PLAYER((PlayerTypes)iPlayer).processVoteSourceBonus(eVoteSource, false);
			}
		}

		m_aiDiploVote[eVoteSource] += iChange;
		FAssert(getDiploVoteCount(eVoteSource) >= 0);

		if ( !m_bRecalculatingModifiers )
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
	if (GC.getVoteInfo(kData.eVote).isVictory())
	{
		for (int iTeam = 0; iTeam < MAX_PC_TEAMS; ++iTeam)
		{
			const CvTeam& kTeam = GET_TEAM((TeamTypes)iTeam);

			if (kTeam.isVotingMember(eVoteSource))
			{
				if (kTeam.getVotes(kData.eVote, eVoteSource) >= getVoteRequired(kData.eVote, eVoteSource) || isOption(GAMEOPTION_UNITED_NATIONS))
				{
					// Can't vote on a winner if one team already has all the votes necessary to win
					return false;
				}
			}
		}
	}

	for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; ++iPlayer)
	{
		const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);

		if ( kPlayer.isAlive() )
		{
			if (kPlayer.isVotingMember(eVoteSource))
			{
				if (!kPlayer.canDoResolution(eVoteSource, kData))
				{
					return false;
				}
			}
			else if (!kPlayer.isMinorCiv())
			{
				// all players need to be able to vote for a diplo victory
				if (GC.getVoteInfo(kData.eVote).isVictory())
				{
					return false;
				}
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
			if (GET_TEAM((TeamTypes)iTeam1).isFullMember(eVoteSource))
			{
				for (int iTeam2 = iTeam1 + 1; iTeam2 < MAX_PC_TEAMS; ++iTeam2)
				{
					const CvTeam& kTeam2 = GET_TEAM((TeamTypes)iTeam2);

					if (kTeam2.isFullMember(eVoteSource))
					{
						if (!kTeam2.isOpenBorders((TeamTypes)iTeam1))
						{
							bOpenWithEveryone = false;
							break;
						}
					}
				}
			}
		}
		if (bOpenWithEveryone)
		{
			return false;
		}
	}
	else if (GC.getVoteInfo(kData.eVote).isDefensivePact())
	{
		bool bPactWithEveryone = true;
		for (int iTeam1 = 0; iTeam1 < MAX_PC_TEAMS; ++iTeam1)
		{
			if (GET_TEAM((TeamTypes)iTeam1).isFullMember(eVoteSource))
			{
				for (int iTeam2 = iTeam1 + 1; iTeam2 < MAX_PC_TEAMS; ++iTeam2)
				{
					const CvTeam& kTeam2 = GET_TEAM((TeamTypes)iTeam2);

					if (kTeam2.isFullMember(eVoteSource))
					{
						if (!kTeam2.isDefensivePact((TeamTypes)iTeam1))
						{
							bPactWithEveryone = false;
							break;
						}
					}
				}
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
		const CvPlayer& kPlayer = GET_PLAYER(kData.ePlayer);

		if (kPlayer.isFullMember(eVoteSource))
		{
			return false;
		}

		bool bNoTradeWithEveryone = true;
		for (int iPlayer2 = 0; iPlayer2 < MAX_PC_PLAYERS; ++iPlayer2)
		{
			const CvPlayer& kPlayer2 = GET_PLAYER((PlayerTypes)iPlayer2);
			if (kPlayer2.isAlive() && kPlayer2.getTeam() != kPlayer.getTeam())
			{
				if (kPlayer2.isFullMember(eVoteSource))
				{
					if (kPlayer2.canStopTradingWithTeam(kPlayer.getTeam()))
					{
						bNoTradeWithEveryone = false;
						break;
					}
				}
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
				if (kTeam2.isFullMember(eVoteSource))
				{
					if (!kTeam2.isAtWar(kPlayer.getTeam()) && kTeam2.canChangeWarPeace(kPlayer.getTeam()))
					{
						bAtWarWithEveryone = false;
						break;
					}
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
				if (atWar(kPlayer.getTeam(), (TeamTypes)iTeam2))
				{
					if (GET_TEAM((TeamTypes)iTeam2).isFullMember(eVoteSource))
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

		if (kOtherPlayer.isHuman() && isOption(GAMEOPTION_ONE_CITY_CHALLENGE))
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
	return (GC.getInitCore().getHotseat());
}

bool CvGame::isPbem() const
{
	return (GC.getInitCore().getPbem());
}



bool CvGame::isPitboss() const
{
	return (GC.getInitCore().getPitboss());
}

bool CvGame::isSimultaneousTeamTurns() const
{
	if (!isNetworkMultiPlayer())
	{
		return false;
	}

	if (isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
	{
		return false;
	}

	return true;
}

bool CvGame::isFinalInitialized() const
{
	return m_bFinalInitialized;
}


void CvGame::setFinalInitialized(bool bNewValue)
{
	OutputDebugString("Setting FinalInitialized: Start");

	PROFILE_FUNC();

	if (isFinalInitialized() != bNewValue)
	{
		m_bFinalInitialized = bNewValue;

		if (isFinalInitialized())
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

	OutputDebugString("Setting FinalInitialized: End");
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
	if (getActivePlayer() == NO_PLAYER)
	{
		return;
	}

	if (!isPlayerOptionsSent() || bForce)
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
		foreach_(CvUnit* pLoopUnit, GET_PLAYER((PlayerTypes)iI).units())
		{
 			if (!pLoopUnit->isUsingDummyEntities())
			{
				gDLL->getEntityIFace()->updateEnemyGlow(pLoopUnit->getUnitEntity());
			}
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
	return (getPausePlayer() != NO_PLAYER);
}


void CvGame::setPausePlayer(PlayerTypes eNewValue)
{
	m_ePausePlayer = eNewValue;
}


UnitTypes CvGame::getBestLandUnit() const
{
	if ( m_eBestLandUnit == NO_UNIT )
	{
		//	This can occur after a load in which the previous best land unit was deleted from the assets
		//	Just recalculate on demand
		for( int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if ( GET_PLAYER((PlayerTypes)iI).isAlive() )
			{
				foreach_(const CvUnit* pLoopUnit, GET_PLAYER((PlayerTypes)iI).units())
				{
					if (pLoopUnit->baseCombatStr() > 0)
					{
						if (m_eBestLandUnit == NO_UNIT || (pLoopUnit->baseCombatStrNonGranular() > getBestLandUnitCombat()))
						{
							m_eBestLandUnit = pLoopUnit->getUnitType();
						}
					}
				}
			}
		}
	}

	return m_eBestLandUnit;
}


int CvGame::getBestLandUnitCombat() const
{
	if (getBestLandUnit() == NO_UNIT)
	{
		return 1;
	}

	return std::max(1, GC.getUnitInfo(getBestLandUnit()).getCombat());
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
	CvWString szBuffer;

	if ((getWinner() != eNewWinner) || (getVictory() != eNewVictory))
	{
		m_eWinner = eNewWinner;
		m_eVictory = eNewVictory;

/************************************************************************************************/
/* AI_AUTO_PLAY_MOD                        07/09/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		CvEventReporter::getInstance().victory(eNewWinner, eNewVictory);
/************************************************************************************************/
/* AI_AUTO_PLAY_MOD                        END                                                  */
/************************************************************************************************/

		if (getVictory() != NO_VICTORY)
		{
			if (getWinner() != NO_TEAM)
			{
				szBuffer = gDLL->getText("TXT_KEY_GAME_WON", GET_TEAM(getWinner()).getName().GetCString(), GC.getVictoryInfo(getVictory()).getTextKeyWide());
				addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, GET_TEAM(getWinner()).getLeaderID(), szBuffer, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
			}
/************************************************************************************************/
/* REVOLUTION_MOD                                                                 lemmy101      */
/*                                                                                jdog5000      */
/*                                                                                              */
/************************************************************************************************/
			if ((getAIAutoPlay(getActivePlayer()) > 0) || gDLL->GetAutorun())
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
			{
				setGameState(GAMESTATE_EXTENDED);
			}
			else
			{
				setGameState(GAMESTATE_OVER);
			}
		}

		gDLL->getInterfaceIFace()->setDirty(Center_DIRTY_BIT, true);

/************************************************************************************************/
/* AI_AUTO_PLAY_MOD                        07/09/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/* original code
		CvEventReporter::getInstance().victory(eNewWinner, eNewVictory);
*/
/************************************************************************************************/
/* AI_AUTO_PLAY_MOD                        END                                                  */
/************************************************************************************************/

		gDLL->getInterfaceIFace()->setDirty(Soundtrack_DIRTY_BIT, true);
	}
}


GameStateTypes CvGame::getGameState() const
{
	return m_eGameState;
}


void CvGame::setGameState(GameStateTypes eNewValue)
{
	if (getGameState() != eNewValue)
	{
		m_eGameState = eNewValue;

		if (eNewValue == GAMESTATE_OVER)
		{
			CvEventReporter::getInstance().gameEnd();

// BUG - AutoSave - start
			Cy::call(PYBugModule, "gameEndSave");
// BUG - AutoSave - end

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
	FAssertMsg(iRank >= 0, "iRank is expected to be non-negative (invalid Rank)");
	FAssertMsg(iRank < MAX_PLAYERS, "iRank is expected to be within maximum bounds (invalid Rank)");
	return (PlayerTypes)m_aiRankPlayer[iRank];
}


void CvGame::setRankPlayer(int iRank, PlayerTypes ePlayer)
{
	FAssertMsg(iRank >= 0, "iRank is expected to be non-negative (invalid Rank)");
	FAssertMsg(iRank < MAX_PLAYERS, "iRank is expected to be within maximum bounds (invalid Rank)");

	if (getRankPlayer(iRank) != ePlayer)
	{
		m_aiRankPlayer[iRank] = ePlayer;

		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}
}


int CvGame::getPlayerRank(PlayerTypes ePlayer) const
{
	FAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	return m_aiPlayerRank[ePlayer];
}


void CvGame::setPlayerRank(PlayerTypes ePlayer, int iRank)
{
	FAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");

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
	FAssert(getPlayerRank(ePlayer) >= 0);
}


int CvGame::getPlayerScore(PlayerTypes ePlayer)	const
{
	FAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	return m_aiPlayerScore[ePlayer];
}


void CvGame::setPlayerScore(PlayerTypes ePlayer, int iScore)
{
	FAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	if (getPlayerScore(ePlayer) != iScore)
	{
		m_aiPlayerScore[ePlayer] = iScore;
		FAssert(getPlayerScore(ePlayer) >= 0);

		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}
}


TeamTypes CvGame::getRankTeam(int iRank) const
{
	FAssertMsg(iRank >= 0, "iRank is expected to be non-negative (invalid Rank)");
	FAssertMsg(iRank < MAX_TEAMS, "iRank is expected to be within maximum bounds (invalid Index)");
	return (TeamTypes)m_aiRankTeam[iRank];
}


void CvGame::setRankTeam(int iRank, TeamTypes eTeam)
{
	FAssertMsg(iRank >= 0, "iRank is expected to be non-negative (invalid Rank)");
	FAssertMsg(iRank < MAX_TEAMS, "iRank is expected to be within maximum bounds (invalid Index)");

	if (getRankTeam(iRank) != eTeam)
	{
		m_aiRankTeam[iRank] = eTeam;

		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}
}


int CvGame::getTeamRank(TeamTypes eTeam) const
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	return m_aiTeamRank[eTeam];
}


void CvGame::setTeamRank(TeamTypes eTeam, int iRank)
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	m_aiTeamRank[eTeam] = iRank;
	FAssert(getTeamRank(eTeam) >= 0);
}


int CvGame::getTeamScore(TeamTypes eTeam) const
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	return m_aiTeamScore[eTeam];
}


void CvGame::setTeamScore(TeamTypes eTeam, int iScore)
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	m_aiTeamScore[eTeam] = iScore;
	FAssert(getTeamScore(eTeam) >= 0);
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
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitCreatedCount[eIndex];
}


void CvGame::incrementUnitCreatedCount(UnitTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiUnitCreatedCount[eIndex]++;
}


bool CvGame::isUnitMaxedOut(UnitTypes eIndex, int iExtra) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (!isWorldUnit(eIndex))
	{
		return false;
	}
	FAssertMsg(getUnitCreatedCount(eIndex) <= GC.getUnitInfo(eIndex).getMaxGlobalInstances(), "Index is expected to be within maximum bounds (invalid Index)");

	return getUnitCreatedCount(eIndex) + iExtra >= GC.getUnitInfo(eIndex).getMaxGlobalInstances();
}


int CvGame::getBuildingCreatedCount(BuildingTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiBuildingCreatedCount[eIndex];
}


bool CvGame::isBuildingMaxedOut(BuildingTypes eIndex, int iExtra) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (!isWorldWonder(eIndex))
	{
		return false;
	}

	return ((getBuildingCreatedCount(eIndex) + iExtra) >= GC.getBuildingInfo(eIndex).getMaxGlobalInstances());
}


void CvGame::incrementBuildingCreatedCount(BuildingTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiBuildingCreatedCount[eIndex]++;
}


int CvGame::getProjectCreatedCount(ProjectTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiProjectCreatedCount[eIndex];
}


bool CvGame::isProjectMaxedOut(ProjectTypes eIndex, int iExtra) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (!isWorldProject(eIndex))
	{
		return false;
	}

	FAssertMsg(getProjectCreatedCount(eIndex) <= GC.getProjectInfo(eIndex).getMaxGlobalInstances(), "Index is expected to be within maximum bounds (invalid Index)");

	return ((getProjectCreatedCount(eIndex) + iExtra) >= GC.getProjectInfo(eIndex).getMaxGlobalInstances());
}


void CvGame::incrementProjectCreatedCount(ProjectTypes eIndex, int iExtra)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumProjectInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiProjectCreatedCount[eIndex] += iExtra;
}


int CvGame::getForceCivicCount(CivicTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCivicInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
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
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCivicInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		const bool bOldForceCivic = isForceCivic(eIndex);

		m_paiForceCivicCount[eIndex] += iChange;
		FAssert(getForceCivicCount(eIndex) >= 0);

		if (bOldForceCivic != isForceCivic(eIndex))
		{
			verifyCivics();
		}
	}
}


PlayerVoteTypes CvGame::getVoteOutcome(VoteTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumVoteInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiVoteOutcome[eIndex];
}


bool CvGame::isVotePassed(VoteTypes eIndex) const
{
	const PlayerVoteTypes ePlayerVote = getVoteOutcome(eIndex);

	if (isTeamVote(eIndex))
	{
		return (ePlayerVote >= 0 && ePlayerVote < MAX_PC_TEAMS);
	}
	else
	{
		return (ePlayerVote == PLAYER_VOTE_YES);
	}
}


void CvGame::setVoteOutcome(const VoteTriggeredData& kData, PlayerVoteTypes eNewValue)
{
	VoteTypes eIndex = kData.kVoteOption.eVote;
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumVoteInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (getVoteOutcome(eIndex) != eNewValue)
	{
		const bool bOldPassed = isVotePassed(eIndex);

		m_paiVoteOutcome[eIndex] = eNewValue;

		if (bOldPassed != isVotePassed(eIndex))
		{
			processVote(kData, ((isVotePassed(eIndex)) ? 1 : -1));
		}
	}

	for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; ++iPlayer)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kPlayer.isAlive())
		{
			kPlayer.setVote(kData.getID(), NO_PLAYER_VOTE);
		}
	}
}


int CvGame::getReligionGameTurnFounded(ReligionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumReligionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiReligionGameTurnFounded[eIndex];
}


bool CvGame::isReligionFounded(ReligionTypes eIndex) const
{
	return (getReligionGameTurnFounded(eIndex) != -1);
}


void CvGame::makeReligionFounded(ReligionTypes eIndex, PlayerTypes ePlayer)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumReligionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

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
	FAssertMsg(eTech >= 0, "eTech is expected to be non-negative (invalid Index)");
	FAssertMsg(eTech < GC.getNumTechInfos(), "eTech is expected to be within maximum bounds (invalid Index)");
	return m_paiTechGameTurnDiscovered[eTech];
}


bool CvGame::isTechDiscovered(TechTypes eTech) const
{
	return (getTechGameTurnDiscovered(eTech) != -1);
}


void CvGame::makeTechDiscovered(TechTypes eTech)
{
	FAssertMsg(eTech >= 0, "eTech is expected to be non-negative (invalid Index)");
	FAssertMsg(eTech < GC.getNumTechInfos(), "eTech is expected to be within maximum bounds (invalid Index)");

	if (!isTechDiscovered(eTech))
	{
		FAssertMsg(getGameTurn() != -1, "getGameTurn() is not expected to be equal with -1");
		m_paiTechGameTurnDiscovered[eTech] = getGameTurn();
	}
}

bool CvGame::isReligionSlotTaken(ReligionTypes eReligion) const
{
	FAssertMsg(eReligion >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eReligion < GC.getNumReligionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abReligionSlotTaken[eReligion];
}

void CvGame::setReligionSlotTaken(ReligionTypes eReligion, bool bTaken)
{
	FAssertMsg(eReligion >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eReligion < GC.getNumReligionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_abReligionSlotTaken[eReligion] = bTaken;
}

bool CvGame::isTechCanFoundReligion(TechTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	return m_abTechCanFoundReligion[eIndex];
}

void CvGame::setTechCanFoundReligion(TechTypes eIndex, bool bUsed)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_abTechCanFoundReligion[eIndex] = !bUsed;
}

bool CvGame::isGameStart()
{
	if(m_bGameStart)
	{
		checkGameStart();
	}
	return m_bGameStart;
}

void CvGame::checkGameStart()
{
	if(getHighestEra() >= getStartEra())
	{
		m_bGameStart = false;
	}
}

int CvGame::countNumReligionsFounded() const
{
	int iCount = 0;

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if( isReligionFounded((ReligionTypes)iI) )
		{
			iCount++;
		}
	}

	return iCount;
}

int CvGame::countNumReligionTechsDiscovered() const
{
	int iCount = 0;

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if( countKnownTechNumTeams((TechTypes)GC.getReligionInfo((ReligionTypes)iI).getTechPrereq()) > 0)
		{
			iCount++;
		}
	}

	return iCount;
}

int CvGame::getCorporationGameTurnFounded(CorporationTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCorporationInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiCorporationGameTurnFounded[eIndex];
}


bool CvGame::isCorporationFounded(CorporationTypes eIndex) const
{
	return (getCorporationGameTurnFounded(eIndex) != -1);
}


void CvGame::makeCorporationFounded(CorporationTypes eIndex, PlayerTypes ePlayer)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCorporationInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (!isCorporationFounded(eIndex))
	{
		FAssertMsg(getGameTurn() != -1, "getGameTurn() is not expected to be equal with -1");
		m_paiCorporationGameTurnFounded[eIndex] = getGameTurn();

		CvEventReporter::getInstance().corporationFounded(eIndex, ePlayer);

	}
}

bool CvGame::isVictoryValid(VictoryTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumVictoryInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return GC.getInitCore().getVictory(eIndex);
}

void CvGame::setVictoryValid(VictoryTypes eIndex, bool bValid)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumVictoryInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	GC.getInitCore().setVictory(eIndex, bValid);
}


bool CvGame::isSpecialUnitValid(SpecialUnitTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabSpecialUnitValid[eIndex];
}


void CvGame::makeSpecialUnitValid(SpecialUnitTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialUnitInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_pabSpecialUnitValid[eIndex] = true;
}


bool CvGame::isSpecialBuildingValid(SpecialBuildingTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabSpecialBuildingValid[eIndex];
}


void CvGame::makeSpecialBuildingValid(SpecialBuildingTypes eIndex, bool bAnnounce)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (!m_pabSpecialBuildingValid[eIndex])
	{
		m_pabSpecialBuildingValid[eIndex] = true;

		if (bAnnounce)
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_SPECIAL_BUILDING_VALID", GC.getSpecialBuildingInfo(eIndex).getTextKeyWide());

			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					MEMORY_TRACK_EXEMPT();

					AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PROJECT_COMPLETED", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
				}
			}
		}
	}
}


//TB Nukefix
//bool CvGame::isNukesValid() const
//{
//	return m_bNukesValid;
//}


//void CvGame::makeNukesValid(bool bValid)
//{
//	m_bNukesValid = bValid;
//}

bool CvGame::isInAdvancedStart() const
{
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		if ((GET_PLAYER((PlayerTypes)iPlayer).getAdvancedStartPoints() >= 0) && GET_PLAYER((PlayerTypes)iPlayer).isHuman())
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
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumReligionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return getCity(m_paHolyCity[eIndex]);
}


void CvGame::setHolyCity(ReligionTypes eIndex, const CvCity* pNewValue, bool bAnnounce)
{
	CvWString szBuffer;
	CvCity* pHolyCity;
	int iI;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumReligionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	CvCity* pOldValue = getHolyCity(eIndex);

	if (pOldValue != pNewValue)
	{
		if (pNewValue != NULL)
		{
			m_paHolyCity[eIndex] = pNewValue->getIDInfo();
		}
		else
		{
			m_paHolyCity[eIndex].reset();
		}

		if (pOldValue != NULL)
		{
			pOldValue->changeReligionInfluence(eIndex, -(GC.getHOLY_CITY_INFLUENCE()));

			pOldValue->updateReligionCommerce();

			pOldValue->setInfoDirty(true);
		}

		if (getHolyCity(eIndex) != NULL)
		{
			pHolyCity = getHolyCity(eIndex);

			pHolyCity->setHasReligion(eIndex, true, bAnnounce, true);
			pHolyCity->changeReligionInfluence(eIndex, GC.getHOLY_CITY_INFLUENCE());

			pHolyCity->updateReligionCommerce();

			pHolyCity->setInfoDirty(true);

			if (bAnnounce)
			{
				if (isFinalInitialized() && !(gDLL->GetWorldBuilderMode()))
				{
					szBuffer = gDLL->getText("TXT_KEY_MISC_REL_FOUNDED", GC.getReligionInfo(eIndex).getTextKeyWide(), pHolyCity->getNameKey());
					addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, pHolyCity->getOwner(), szBuffer, pHolyCity->getX(), pHolyCity->getY(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));

					for (iI = 0; iI < MAX_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).isAlive())
						{
							MEMORY_TRACK_EXEMPT();

							if (pHolyCity->isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
							{
								szBuffer = gDLL->getText("TXT_KEY_MISC_REL_FOUNDED", GC.getReligionInfo(eIndex).getTextKeyWide(), pHolyCity->getNameKey());
								AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME_LONG(), szBuffer, GC.getReligionInfo(eIndex).getSound(), MESSAGE_TYPE_MAJOR_EVENT, GC.getReligionInfo(eIndex).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), pHolyCity->getX(), pHolyCity->getY());
							}
							else
							{
								szBuffer = gDLL->getText("TXT_KEY_MISC_REL_FOUNDED_UNKNOWN", GC.getReligionInfo(eIndex).getTextKeyWide());
								AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME_LONG(), szBuffer, GC.getReligionInfo(eIndex).getSound(), MESSAGE_TYPE_MAJOR_EVENT, GC.getReligionInfo(eIndex).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
							}
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
}


CvCity* CvGame::getHeadquarters(CorporationTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCorporationInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return getCity(m_paHeadquarters[eIndex]);
}


void CvGame::setHeadquarters(CorporationTypes eIndex, CvCity* pNewValue, bool bAnnounce)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCorporationInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

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

// Afforess 06/17/10
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
// ! Afforess

		if (NULL != pHeadquarters)
		{
			pHeadquarters->setHasCorporation(eIndex, true, bAnnounce);
			pHeadquarters->updateCorporation();
			pHeadquarters->setInfoDirty(true);

			if (bAnnounce)
			{
				if (isFinalInitialized() && !(gDLL->GetWorldBuilderMode()))
				{
					CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CORPORATION_FOUNDED", GC.getCorporationInfo(eIndex).getTextKeyWide(), pHeadquarters->getNameKey());
					addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, pHeadquarters->getOwner(), szBuffer, pHeadquarters->getX(), pHeadquarters->getY(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));

					for (int iI = 0; iI < MAX_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).isAlive())
						{
							MEMORY_TRACK_EXEMPT();

							if (pHeadquarters->isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
							{
								AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME_LONG(), szBuffer, GC.getCorporationInfo(eIndex).getSound(), MESSAGE_TYPE_MAJOR_EVENT, GC.getCorporationInfo(eIndex).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), pHeadquarters->getX(), pHeadquarters->getY());
							}
							else
							{
								CvWString szBuffer2 = gDLL->getText("TXT_KEY_MISC_CORPORATION_FOUNDED_UNKNOWN", GC.getCorporationInfo(eIndex).getTextKeyWide());
								AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME_LONG(), szBuffer2, GC.getCorporationInfo(eIndex).getSound(), MESSAGE_TYPE_MAJOR_EVENT, GC.getCorporationInfo(eIndex).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
							}
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
	FAssert(eOwnerIndex >= 0);
	FAssert(eOwnerIndex < MAX_PC_PLAYERS);
	FAssert(NULL != getVoteTriggered(iVoteId));

	return GET_PLAYER(eOwnerIndex).getVote(iVoteId);
}


void CvGame::setPlayerVote(PlayerTypes eOwnerIndex, int iVoteId, PlayerVoteTypes eNewValue)
{
	FAssert(eOwnerIndex >= 0);
	FAssert(eOwnerIndex < MAX_PC_PLAYERS);
	FAssert(NULL != getVoteTriggered(iVoteId));

	GET_PLAYER(eOwnerIndex).setVote(iVoteId, eNewValue);
}


void CvGame::castVote(PlayerTypes eOwnerIndex, int iVoteId, PlayerVoteTypes ePlayerVote)
{
	VoteTriggeredData* pTriggeredData = getVoteTriggered(iVoteId);
	if (NULL != pTriggeredData)
	{
		const CvVoteInfo& kVote = GC.getVoteInfo(pTriggeredData->kVoteOption.eVote);
		if (kVote.isAssignCity())
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
			if ((PlayerVoteTypes)GET_PLAYER(eOwnerIndex).getTeam() != ePlayerVote)
			{
				for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; ++iPlayer)
				{
					CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
					if (kLoopPlayer.isAlive())
					{
						if (kLoopPlayer.getTeam() != GET_PLAYER(eOwnerIndex).getTeam() && kLoopPlayer.getTeam() == (TeamTypes)ePlayerVote)
						{
							kLoopPlayer.AI_changeMemoryCount(eOwnerIndex, MEMORY_VOTED_FOR_US, 1);
						}
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


void CvGame::setName(const TCHAR* szName)
{
	GC.getInitCore().setGameName(szName);
}


bool CvGame::isDestroyedCityName(CvWString& szName) const
{
	std::vector<CvWString>::const_iterator it;

	for (it = m_aszDestroyedCities.begin(); it != m_aszDestroyedCities.end(); ++it)
	{
		if (*it == szName)
		{
			return true;
		}
	}

	return false;
}

void CvGame::addDestroyedCityName(const CvWString& szName)
{
	m_aszDestroyedCities.push_back(szName);
}

bool CvGame::isGreatPersonBorn(CvWString& szName) const
{
	std::vector<CvWString>::const_iterator it;

	for (it = m_aszGreatPeopleBorn.begin(); it != m_aszGreatPeopleBorn.end(); ++it)
	{
		if (*it == szName)
		{
			return true;
		}
	}

	return false;
}

void CvGame::addGreatPersonBornName(const CvWString& szName)
{
	m_aszGreatPeopleBorn.push_back(szName);
}


// Protected Functions...

void CvGame::doTurn()
{
	MEMORY_TRACE_FUNCTION();
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

	GC.getMap().doTurn();

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

	for(int n=0; n < MAX_PLAYERS; n++)
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
				if (isPbem() && GET_PLAYER((PlayerTypes)iI).isHuman())
				{
					if (iI == getActivePlayer())
					{
						// Nobody else left alive
						GC.getInitCore().setType(GAME_HOTSEAT_NEW);
						GET_PLAYER((PlayerTypes)iI).setTurnActive(true);
					}
					else if (!getPbemTurnSent())
					{
						gDLL->sendPbemTurn((PlayerTypes)iI);
					}
				}
				else
				{
					GET_PLAYER((PlayerTypes)iI).setTurnActive(true);
					FAssert(getNumGameTurnActive() == 1);
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

	stopProfilingDLL(true);
	gDLL->getEngineIFace()->AutoSave();

#ifdef MEMTRACK
	//MemTrack::TrackListMemoryUsage(); // total
	MemTrack::TrackListMemoryUsageTurnDiff(); // leaks
#endif
}


void CvGame::doDeals()
{
	MEMORY_TRACE_FUNCTION();

	verifyDeals();

	int iLoop;
	for(CvDeal* pLoopDeal = firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = nextDeal(&iLoop))
	{
		pLoopDeal->doTurn();
	}
}

//Enumerates all currently possible spawn plots for a spawning rule, for use in a thread, local density is not checked
void enumSpawnPlots(int iSpawnInfo, std::vector<CvPlot*>* plots)
{
	const CvSpawnInfo& spawnInfo = GC.getSpawnInfo((SpawnTypes)iSpawnInfo);
	//GC.getGame().logMsg("Spawn thread start for %s", spawnInfo.getType());
	plots->clear();

	const TechTypes ePrereqTech = spawnInfo.getPrereqTechType();
	const TechTypes eObsoleteTech = spawnInfo.getObsoleteTechType();
	const bool bUseTechGuides = ePrereqTech != NO_TECH || eObsoleteTech != NO_TECH;
	bool bAnyTeamHasPrereqTech = false;
	//	Enables in this turn year?
	const int iTurnYear = GC.getGame().getTurnYear(GC.getGame().getGameTurn());
	if (!bUseTechGuides)
	{
		if(iTurnYear < spawnInfo.getStartDate() || iTurnYear > spawnInfo.getEndDate())
		{
			return;
		}
	}
	else //in essence this means that ePrereqTech!=NO_TECH has already been established.
	{
		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (eObsoleteTech != NO_TECH && GET_TEAM((TeamTypes)iI).isHasTech(eObsoleteTech))
				{
					return;
				}
				if (!bAnyTeamHasPrereqTech && GET_TEAM((TeamTypes)iI).isHasTech(ePrereqTech))
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

	const CvUnitInfo& unitInfo = GC.getUnitInfo(spawnInfo.getUnitType());
	const PlayerTypes ePlayer = spawnInfo.getPlayer();
	const bool bIsBarbarian = spawnInfo.getTreatAsBarbarian() && GET_PLAYER(ePlayer).isHominid();
	if (bIsBarbarian && GC.getGame().isOption(GAMEOPTION_NO_BARBARIANS))
	{
		return;
	}

	if ( spawnInfo.getRateOverride() == 0 )
	{
		return;
	}

	const bool bNoTerrainFeatureBonus = (spawnInfo.getNumTerrains() == 0) && (spawnInfo.getNumFeatures() == 0) && (spawnInfo.getNumBonuses() == 0) && !spawnInfo.getPeaks();

	const bool bAnimalBarred = GC.getGame().isOption(GAMEOPTION_ANIMALS_STAY_OUT) && GC.getUnitInfo(spawnInfo.getUnitType()).isWildAnimal();
	for (int i = 0; i < GC.getMap().numPlots(); ++i)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(i);

		if ( pPlot->plotCount(PUF_isOtherPlayer, ePlayer) == 0 )
		{
			if ( (bAnimalBarred || spawnInfo.getNeutralOnly()) && (pPlot->getOwner() != NO_PLAYER && pPlot->getOwner() != ePlayer) )
			{
				continue;
			}

			if (GC.getUnitInfo(spawnInfo.getUnitType()).isWildAnimal())
			{
				if (pPlot->isOwned() && !GC.getUnitInfo(spawnInfo.getUnitType()).canAnimalIgnoresBorders())
				{
					continue;
				}

				if (pPlot->isOwned() && pPlot->getImprovementType() != NO_IMPROVEMENT && !GC.getUnitInfo(spawnInfo.getUnitType()).canAnimalIgnoresImprovements())
				{
					continue;
				}

				if (pPlot->isCity(true) && !GC.getUnitInfo(spawnInfo.getUnitType()).canAnimalIgnoresCities())
				{
					continue;
				}
			}

			bool bValid = true;
			if (pPlot->isHills())
				bValid = spawnInfo.getHills();
			else if (pPlot->isFlatlands())
				bValid = spawnInfo.getFlatlands();

			// Check latitude
			if (bValid)
			{
				int lat;
				if (spawnInfo.getLatitudeAbs())
					lat = pPlot->getLatitude();
				else
					lat = pPlot->getLatitudeRaw();
				bValid = (lat >= spawnInfo.getMinLatitude()) && (lat <= spawnInfo.getMaxLatitude());

				const int iLongitude = pPlot->getLongitudeMinutes() / 60;
				const int iMinLongitude = spawnInfo.getMinLongitude();
				const int iMaxLongitude = spawnInfo.getMaxLongitude();
				if (iMinLongitude <= iMaxLongitude)
				{
					bValid = bValid && (iLongitude >= iMinLongitude) && (iLongitude <= iMaxLongitude);
				}
				else
				{
					bValid = bValid && ((iLongitude >= iMinLongitude) || (iLongitude <= iMaxLongitude));
				}

				bValid = bValid && (!spawnInfo.getFreshWaterOnly() || pPlot->isFreshWater());
			}

			if ( bValid )
			{
				if (bNoTerrainFeatureBonus)
				{
					bValid = true;
				}
				else if (pPlot->isPeak2(true))
				{
					bValid = spawnInfo.getPeaks();
				}
				else
				{
					bValid = false;
					BonusTypes bonusType = pPlot->getBonusType();
					for(int k = 0; k < spawnInfo.getNumBonuses(); k++) // Check if bonus for spawning is present
					{
						if ( spawnInfo.getBonus(k) == bonusType )
						{
							bValid = true;
							break;
						}
					}

					if ( !bValid)
					{
						const FeatureTypes featureType = pPlot->getFeatureType();
						const TerrainTypes terrainType = pPlot->getTerrainType();
						if (featureType == NO_FEATURE)
						{
							for(int k = 0; k < spawnInfo.getNumTerrains(); k++) // Check if terrain for spawning is present
							{
								if ( spawnInfo.getTerrain(k) == terrainType )
								{
									bValid = true;
									break;
								}
							}
						}
						else
						{
							for(int k = 0; k < spawnInfo.getNumFeatures(); k++) // Check if feature for spawning is present
							{
								if ( spawnInfo.getFeature(k) == featureType )
								{
									bValid = true;
									break;
								}
							}

							if (bValid)  // need also proper terrain
							{

								bValid = spawnInfo.getNumFeatureTerrains() == 0;

								for(int k = 0; k < spawnInfo.getNumFeatureTerrains(); k++) // Check if terrain for spawning is present
								{
									if ( spawnInfo.getFeatureTerrain(k) == terrainType )
									{
										bValid = true;
										break;
									}
								}
							}
						}
					}
				}
			}

			if ( bValid )
			{
				if ( spawnInfo.getNotInView() )
				{
					if (pPlot->isVisibleToCivTeam())
					{
						continue;
					}
				}

				BoolExpr* pCondition = spawnInfo.getSpawnCondition();

				if (pCondition)
				{
					if (!pCondition->evaluate(pPlot->getGameObject()))
					{
						continue;
					}
				}

				// Valid spawning spot
				plots->push_back(pPlot);
			}
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
	MEMORY_TRACE_FUNCTION();
	PROFILE_FUNC();

	std::vector<std::vector<CvPlot*> > validPlots;
	validPlots.resize(GC.getNumSpawnInfos());

	//	We need to know the number of each unit type in each area, which is not soemthign CvArea
	//	can do (nor worth implementing it there), so do a pre-pass here to build that data
	std::map< int, std::map<int,int> >	areaPopulationMap;

	for (int i = 0; i < GC.getMap().numPlots(); ++i)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(i);

		CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();

		while (pUnitNode != NULL)
		{
			const CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);

			if (pLoopUnit != NULL)
			{
				//	Only care about evaluated NPC
				if ( pLoopUnit->getOwner() == ePlayer )
				{
					if ( areaPopulationMap[pPlot->getArea()].find(pLoopUnit->getUnitType()) != areaPopulationMap[pPlot->getArea()].end() )
					{
						areaPopulationMap[pPlot->getArea()][pLoopUnit->getUnitType()]++;
					}
					else
					{
						areaPopulationMap[pPlot->getArea()][pLoopUnit->getUnitType()] = 1;
					}
				}
				pUnitNode = pPlot->nextUnitNode(pUnitNode);
			}
			else
			{
				pPlot->verifyUnitValidPlot();
				pUnitNode = NULL;
			}
		}
	}

	std::vector<int> aRandomList;
	CvWString szNewNameBuffer;

	for(int j = 0; j < GC.getNumSpawnInfos(); j++)
	{
		const SpawnTypes eSpawn = (SpawnTypes)j;
		const CvSpawnInfo& spawnInfo = GC.getSpawnInfo((SpawnTypes)j);

		//TB Note: It is at this point that we need to isolate out the player type on spawn info.
		//I think for the sake of speed and data efficiency we can get away with a singular player reference rather than
		//making it possible to specify more than one on a spawn info.

		if ( spawnInfo.getPlayer() != ePlayer )
		{
			continue;
		}

		if (isOption(GAMEOPTION_SIZE_MATTERS) && spawnInfo.getNumSpawnGroup() > 1)
		{
			continue;
		}
		if ( spawnInfo.getRateOverride() == 0 )
		{
			continue;
		}

		enumSpawnPlots(j, &(validPlots[j]));

		int iPlotNum = validPlots[j].size();
		logMsg("Spawn thread finished and joined for %s, found %d valid plots.", spawnInfo.getType(), iPlotNum);

		if (iPlotNum == 0)
		{
			continue;
		}

		double adjustedSpawnRate = (double)spawnInfo.getTurnRate();
		double fGlobalSpawnRate = (double)spawnInfo.getGlobalTurnRate() * iPlotNum / 100.0;
		if (adjustedSpawnRate <= 0)
		{
			adjustedSpawnRate = fGlobalSpawnRate;
		}
		else if (fGlobalSpawnRate > 0)
		{
			adjustedSpawnRate = 1/(1/adjustedSpawnRate + 1/fGlobalSpawnRate);
		}

		if ( !spawnInfo.getNoSpeedNormalization() )
		{
			adjustedSpawnRate = (adjustedSpawnRate*((GC.getGameSpeedInfo(getGameSpeedType()).getTrainPercent()+666)/3) / 100);
		}

		if (isOption(GAMEOPTION_PEACE_AMONG_NPCS))
		{
			adjustedSpawnRate *= (100+GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getSpawnRateNPCPeaceModifier());
		}
		else
		{
			adjustedSpawnRate *= (100+GC.getCivilizationInfo(GET_PLAYER(ePlayer).getCivilizationType()).getSpawnRateModifier());
		}
		adjustedSpawnRate /= 100;
		//	Adjust for any rate override
		adjustedSpawnRate = (adjustedSpawnRate*100)/spawnInfo.getRateOverride();


		//	Double for barbarians if raging
		const bool bIsBarbarian = spawnInfo.getTreatAsBarbarian();
		int iMaxAreaTotalDensity = spawnInfo.getMaxAreaTotalDensity();
		int iMaxAreaUnitDensity = spawnInfo.getMaxAreaUnitDensity();
		if (bIsBarbarian && isOption(GAMEOPTION_RAGING_BARBARIANS))
		{
			adjustedSpawnRate /= 2;
			iMaxAreaTotalDensity /= 2;
			iMaxAreaUnitDensity /= 2;
		}

		logMsg("Spawn chance per plot for %s is 1 to %d .", spawnInfo.getType(), (int)adjustedSpawnRate);


		int spawnCount = 0;
		//So we ARE going by spawn here but it's still a random check per plot rather than placing an amount.  Before this, determine how many should spawn this round, then pick a plot for each of those spawns.
		//The density factor is going to be interesting.  Perhaps each plot should get a likelihood value and vary that by the density factor around that plot.
		//The spawn rate... is high more likely or low and what kind of numeric range are we working with?
		const int iNumAreas = GC.getMap().getNumAreas();

		for (int iI = 0; iI < iNumAreas; iI++)
		{
			CvArea* pArea = GC.getMap().getArea(iI);
			int iValidPlots = 0;
			for (std::vector<CvPlot*>::iterator it = validPlots[j].begin(); it < validPlots[j].end(); ++it)
			{
				CvPlot* pPlot = *it;
				if (pPlot->area() == pArea)
				{
					iValidPlots++;
				}
			}
			pArea->setNumValidPlotsbySpawn(eSpawn, iValidPlots);
		}

		for (std::vector<CvPlot*>::iterator it = validPlots[j].begin(); it < validPlots[j].end(); ++it)
		{
			const CvPlot* pPlot = *it;
			const int iArea = pPlot->getArea();
			const CvArea* pArea = pPlot->area();
			const int iTotalAreaSize = pArea->getNumTiles();
			int iLocalSpawnRate = (int)adjustedSpawnRate;
			//TB Spawn Modification Begin
			//Automatically adapts chances of spawn to normalize between units that have a huge amount of plots available to spawn in and those that have relatively few.
			//Keeping in mind higher is less chance.
			const int iValidPlotsThisArea = pArea->getNumValidPlotsbySpawn(eSpawn);
			int iFrequencyAdjustment = (iValidPlotsThisArea * 100)/iTotalAreaSize;//Produces the % of plots valid in area. - note: divide by 0 should be literally impossible or we wouldn't have gotten this far as there would not be a plot to be checking this.
			iFrequencyAdjustment += 50;// Thus 1% becomes 51% while 90% becomes 140%

			iLocalSpawnRate *= iFrequencyAdjustment;
			iLocalSpawnRate /= 100;
			//TB Spawn Modification End


			//Actually, here the unit is selected already based on the spawn info itself.  Derive that then derive the MapCategoryTypes
			//of the unit and ensure that pPlot has the MapCategoryType of the unit.  Easy... no need to include MapCategoryTypes on the
			//Spawn Info file.
			const int iCount = GC.getUnitInfo(spawnInfo.getUnitType()).getNumMapCategoryTypes();
			bool bFound = (iCount < 1);
			for (int iI = 0; iI < iCount; iI++)
			{
				if (pPlot->isMapCategoryType((MapCategoryTypes)GC.getUnitInfo(spawnInfo.getUnitType()).getMapCategoryType(iI)))
				{
					bFound = true;
				}
			}
			if (!bFound)
			{
				continue;
			}
			//Note: correct MapCategoryType checks throughout the code so that if one is valid that gets checked later, the validation is still there!

			// Safety valve - if there are over a certain number of total barbarian units in existance halt all spawns
			// else the game can run into fatal issues when it uses up the entire id space of units (8191 at once per player)
			if (GET_PLAYER(ePlayer).getNumUnits() > MAX_BARB_UNITS_FOR_SPAWNING)
			{
				break;
			}

			// Could be in the threads but this way the units already spawned in this turn by this system are considered
			// iMaxAreaTotalDensity isn't really meant for small areas where units cannot wander out of the local 49 plot spawn area.
			if (iTotalAreaSize > (iMaxAreaTotalDensity > 49 ? iMaxAreaTotalDensity : 49)
			&& iTotalAreaSize / std::max(1, pPlot->area()->getUnitsPerPlayer(ePlayer)) <= iMaxAreaTotalDensity)
			{
				continue;
			}

			//	Adjust spawn rate for area ownership percentage
			if (!pPlot->isWater() && pPlot->area()->getNumCities() == pPlot->area()->getCitiesPerPlayer(ePlayer))
			{
				// No non-barb cities so dramatically reduce spawns since its essentially an unplayed-in area for now
				iLocalSpawnRate *= 10;
			}
			else
			{
				const int percentOccupied = (100*pPlot->area()->getNumOwnedTiles())/pPlot->area()->getNumTiles();
				//	Adjust spawn rate upward by up to trebbleing the normal rate in proportion with how
				//	little wilderness is left in this area
				iLocalSpawnRate = iLocalSpawnRate * 100 / (100 + 2*percentOccupied);
			}

			if (getSorenRandNum(std::max(1, iLocalSpawnRate), "Unit spawn") == 0)
			{
				// Check area unit type density not exceeded if specified
				if (iMaxAreaUnitDensity != 0)
				{
					int	iExistingUnits = 0;

					if (areaPopulationMap[pPlot->getArea()].find(spawnInfo.getUnitType()) != areaPopulationMap[pPlot->getArea()].end())
					{
						iExistingUnits = areaPopulationMap[pPlot->getArea()][spawnInfo.getUnitType()];
					}

					if (iExistingUnits >= std::max(1, iTotalAreaSize / iMaxAreaUnitDensity))
					{
						continue; // Total area density exceeded
					}
				}

				// Check local max density not already exceeded
				int localAreaSize = 0;
				int localUnitTypeCount = 0;
				int localPlayerUnitCount = 0;

				// 49 plot local area, radius = 3
				const int LocalRange = 3;
				const int TotalLocalArea = (LocalRange * 2 + 1) * (LocalRange * 2 + 1);

				for (int iX = -LocalRange; iX <= LocalRange; iX++)
				{
					for (int iY = -LocalRange; iY <= LocalRange; iY++)
					{
						const CvPlot* pLoopPlot = plotXY(pPlot->getX(), pPlot->getY(), iX, iY);
						if (pLoopPlot != NULL && pLoopPlot->area() == pPlot->area())
						{
							localAreaSize++;
							localUnitTypeCount += pLoopPlot->plotCount(PUF_isUnitType, spawnInfo.getUnitType());
							localPlayerUnitCount += pLoopPlot->plotCount(PUF_isPlayer, spawnInfo.getPlayer());
						}
					}
				}
				// We know that localAreaSize > 0 because pLoopPlot == pPlot in one of the above iterations.
				if (localUnitTypeCount == 0
				// Max 0.50 unit per plot in local area owned by spawn owner.
				|| localPlayerUnitCount * 100 / localAreaSize < 50
				// Max local density limit for this specific unit type.
				&& localUnitTypeCount * 100 / localAreaSize < spawnInfo.getMaxLocalDensity() * 100 / TotalLocalArea)
				{
					//	Spawn a new unit
					CvUnitInfo& kUnit = GC.getUnitInfo(spawnInfo.getUnitType());

					CvUnit* pUnit = GET_PLAYER(ePlayer).initUnit(spawnInfo.getUnitType(), pPlot->getX(), pPlot->getY(), (UnitAITypes)kUnit.getDefaultUnitAIType(), NO_DIRECTION, getSorenRandNum(10000, "AI Unit Birthmark"));
					if (pUnit == NULL)
					{
						FErrorMsg("pUnit is expected to be assigned a valid unit object");
						return;
					}

					if (isOption(GAMEOPTION_SIZE_MATTERS))
					{
						const int iCount = kUnit.getNumGroupSpawnUnitCombatTypes();
						if (iCount > 0)
						{
							//clear old list
							aRandomList.clear();
							//generate random list
							for (int iIndex = 0; iIndex < iCount; iIndex++)
							{
								for (int iJ = 0; iJ < kUnit.getGroupSpawnUnitCombatType(iIndex).iChance; iJ++)
								{
									aRandomList.push_back(iIndex);
								}
							}
							//generate result from list
							const int iListCount = (int)aRandomList.size();
							const int iResult = getSorenRandNum(iListCount, "Spawn Group Random Roll");
							const int iFinalIndex = aRandomList[iResult];
							const UnitCombatTypes eGroupVolume = kUnit.getGroupSpawnUnitCombatType(iFinalIndex).eUnitCombat;
							CvWString szTitle = kUnit.getGroupSpawnUnitCombatType(iFinalIndex).m_szTitle;
							const CvWString szOriginal = pUnit->getName();
							//remove old group volume unitcombat
							if (eGroupVolume != NO_UNITCOMBAT)
							{
								for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
								{
									if (GC.getUnitCombatInfo((UnitCombatTypes)iI).getGroupBase() != -10)
									{
										const UnitCombatTypes eUnitCombat = ((UnitCombatTypes)iI);
										if (pUnit->isHasUnitCombat(eUnitCombat) && eUnitCombat != eGroupVolume)
										{
											const int iDifference = GC.getUnitCombatInfo(eGroupVolume).getGroupBase() - GC.getUnitCombatInfo(eUnitCombat).getGroupBase();
											CvUnit::normalizeUnitPromotions(pUnit, iDifference,
												bst::bind(Game::isGroupUpgradePromotion, pUnit, _2),
												bst::bind(Game::isGroupDowngradePromotion, pUnit, _2)
											);
										}
									}
								}
							}
							//adjust the name of the unit
							if (!szTitle.empty())
							{
								szNewNameBuffer.append(gDLL->getText(szTitle));
								szNewNameBuffer.append(gDLL->getText(szOriginal));
								pUnit->setName(szNewNameBuffer);
								szNewNameBuffer.clear();
								szTitle.clear();
							}
						}
					}
					pUnit->finishMoves();
					spawnCount++;

					// Spawn unit group
					if (!isOption(GAMEOPTION_SIZE_MATTERS))
					{
						for(int k = 0; k < spawnInfo.getNumSpawnGroup(); k++)
						{
							kUnit = GC.getUnitInfo(spawnInfo.getSpawnGroup(k));

							pUnit = GET_PLAYER(ePlayer).initUnit(spawnInfo.getSpawnGroup(k), pPlot->getX(), pPlot->getY(), (UnitAITypes)kUnit.getDefaultUnitAIType(), NO_DIRECTION, getSorenRandNum(10000, "AI Unit Birthmark"));
							FAssertMsg(pUnit != NULL, "pUnit is expected to be assigned a valid unit object");
							pUnit->finishMoves();
							spawnCount++;
						}
					}
				}
			}
		}

		logMsg("%d units spawned for %s", spawnCount, spawnInfo.getType());
	}
}

#ifdef GLOBAL_WARMING
void CvGame::doGlobalWarming()
{//GWMod Start M.A.
	MEMORY_TRACE_FUNCTION();

	int iGlobalWarmingDefense = 0;
	int iTreeHuggerDefenseBonus = GC.getDefineINT("TREEHUGGER_DEFENSE_BONUS");
	bool abTreeHugger[MAX_PLAYERS];

	for (int iI = 0; iI < MAX_PLAYERS; iI++)//GWMod Loop to look for environmentalism witten by EmperorFool
	{
		abTreeHugger[iI] = false;
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			for (int iJ = 0; iJ < GC.getNumCivicOptionInfos(); iJ++)
			{
				CivicTypes eCivic = GET_PLAYER((PlayerTypes)iI).getCivics((CivicOptionTypes)iJ);
				if (eCivic != NO_CIVIC && GC.getCivicInfo(eCivic).getExtraHealth() != 0)
				{
					abTreeHugger[iI] = true;
					break;
				}
			}
		}
	}

	for (int i = 0; i < GC.getMap().numPlots(); ++i)
	{
		CvPlot* pPlot = GC.getMap().plotByIndex(i);
		if (pPlot->getFeatureType() != NO_FEATURE)
		{
			int iFeatureWarmingDefense = GC.getFeatureInfo(pPlot->getFeatureType()).getWarmingDefense();
			if (iFeatureWarmingDefense > 0)
			{
				PlayerTypes eCulturalOwner = pPlot->getOwner();
				if (eCulturalOwner != NO_PLAYER)
				{
					if (abTreeHugger[eCulturalOwner])
					{
						iGlobalWarmingDefense += (iFeatureWarmingDefense) * (iTreeHuggerDefenseBonus);
					}
					else
					{
						iGlobalWarmingDefense += iFeatureWarmingDefense;
					}
				}
				else
				{
					iGlobalWarmingDefense += iFeatureWarmingDefense;
				}
			}
		}
	}
	iGlobalWarmingDefense = iGlobalWarmingDefense * GC.getDefineINT("GLOBAL_WARMING_FOREST") / std::max(1, GC.getMap().getLandPlots());

	int iUnhealthWeight = GC.getDefineINT("GLOBAL_WARMING_UNHEALTH_WEIGHT");
	int iBonusWeight = GC.getDefineINT("GLOBAL_WARMING_BONUS_WEIGHT");
	int iPowerWeight = GC.getDefineINT("GLOBAL_WARMING_POWER_WEIGHT");
	int iGlobalWarmingValue = 0;
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
		if (kPlayer.isAlive())
		{
			foreach_(const CvCity* pCity, kPlayer.cities())
			{
				iGlobalWarmingValue -= (pCity->totalBadBuildingHealth() * iUnhealthWeight) + (pCity->getBonusBadHealth() * iBonusWeight) + (pCity->getPowerBadHealth() * iPowerWeight); //GWMod Changed to be total building bad health and to include power and bonuses M.A.
			}
		}
	}
	iGlobalWarmingValue /= GC.getMap().numPlots();

	TerrainTypes eWarmingTerrain = ((TerrainTypes)(GC.getDefineINT("GLOBAL_WARMING_TERRAIN")));
	TerrainTypes eFrozenTerrain = ((TerrainTypes)(GC.getDefineINT("FROZEN_TERRAIN")));
	TerrainTypes eColdTerrain = ((TerrainTypes)(GC.getDefineINT("COLD_TERRAIN")));
	TerrainTypes eMarshTerrain = ((TerrainTypes)(GC.getDefineINT("MARSH_TERRAIN")));
	TerrainTypes eTemperateTerrain = ((TerrainTypes)(GC.getDefineINT("TEMPERATE_TERRAIN")));
	TerrainTypes eDryTerrain = ((TerrainTypes)(GC.getDefineINT("DRY_TERRAIN")));
	TerrainTypes eBarrenTerrain = ((TerrainTypes)(GC.getDefineINT("BARREN_TERRAIN")));
	TerrainTypes eShallowsTerrain = ((TerrainTypes)(GC.getDefineINT("SHALLOW_WATER_TERRAIN")));

	FeatureTypes eColdFeature = ((FeatureTypes)(GC.getDefineINT("COLD_FEATURE")));
	FeatureTypes eTemperateFeature = ((FeatureTypes)(GC.getDefineINT("TEMPERATE_FEATURE")));
	FeatureTypes eWarmFeature = ((FeatureTypes)(GC.getDefineINT("WARM_FEATURE")));
	FeatureTypes eFalloutFeature = ((FeatureTypes)(GC.getDefineINT("NUKE_FEATURE")));

	//Global Warming
	for (int iI = 0; iI < iGlobalWarmingValue; iI++)
	{
		int iOdds = GC.getDefineINT("GLOBAL_WARMING_PROB") - iGlobalWarmingDefense;
		iOdds *= 100;
		iOdds /= GC.getGameSpeedInfo(getGameSpeedType()).getVictoryDelayPercent();

		if (getSorenRandNum(100, "Global Warming") < iOdds)
		{
			CvPlot* pPlot = GC.getMap().syncRandPlot(RANDPLOT_NOT_CITY); // GWMod removed check for water tile M.A.

			if (pPlot != NULL)
			{
				//FeatureTypes eFeature = pPlot->getFeatureType();
				bool bChanged = false;

				if (pPlot->getFeatureType() != NO_FEATURE)
				{
					if (pPlot->getFeatureType() != GC.getDefineINT("NUKE_FEATURE"))
					{// GWMod won't remove features if underlaying terrain can melt
						if (pPlot->getFeatureType() != eColdFeature)
						{
							if ((pPlot->calculateBestNatureYield(YIELD_FOOD, NO_TEAM) > 1) && (pPlot->getFeatureType() == eTemperateFeature))
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
						else
						{
							pPlot->setFeatureType(NO_FEATURE);
							bChanged = true;
						}
					}
				}
				else if (!pPlot->isWater())  // GWMod added check for water tile M.A.
				{// GWMod stepped terrain changes M.A.
					if (pPlot->getTerrainType() == eBarrenTerrain)
					{
						if (GC.getDefineINT("GW_MOD_ENABLED"))
						{
							if (pPlot->isCoastalLand())
							{
								if (!pPlot->isHills() && !pPlot->isPeak2(true))
								{
									pPlot->setTerrainType(eShallowsTerrain);
									bChanged = true;
								}
							}
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

					CvCity* pCity = GC.getMap().findCity(pPlot->getX(), pPlot->getY());
					if (pCity != NULL)
					{
						if (pPlot->isVisible(pCity->getTeam(), false))
						{
							MEMORY_TRACK_EXEMPT();

							CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_GLOBAL_WARMING_NEAR_CITY", pCity->getNameKey());
							AddDLLMessage(pCity->getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_GLOBALWARMING", MESSAGE_TYPE_INFO, NULL, CvColorInfo::red(), pPlot->getX(), pPlot->getY(), true, true);
						}
					}
				}
			}
		}
	}
	//Nuclear Winter
	int iNuclearWinterValue = 0;
	iNuclearWinterValue += getNukesExploded() * GC.getDefineINT("GLOBAL_WARMING_NUKE_WEIGHT") / 100;

	for (int iI = 0; iI < iNuclearWinterValue; iI++)
	{
		int iOdds = GC.getDefineINT("NUCLEAR_WINTER_PROB"); //Fuyu: iGlobalWarmingDefense does no longer protect from nuclear winters
		iOdds *= 100;
		iOdds /= GC.getGameSpeedInfo(getGameSpeedType()).getVictoryDelayPercent();

		if (getSorenRandNum(100, "Nuclear Fallout") < iOdds)
		{
			CvPlot* pPlot = GC.getMap().syncRandPlot(RANDPLOT_LAND | RANDPLOT_NOT_CITY);

			if (pPlot != NULL)
			{
				bool bChanged = false;
				if (pPlot->getFeatureType() != NO_FEATURE)
				{
					if (pPlot->getFeatureType() != GC.getDefineINT("NUKE_FEATURE"))
					{
						if (pPlot->getFeatureType() != eColdFeature)
						{
							pPlot->setFeatureType(NO_FEATURE);
							bChanged = true;
						}
					}
				}
				else
				{
					pPlot->setFeatureType(eFalloutFeature);
					bChanged = true;
				}
				if (getSorenRandNum(100, "Nuclear Winter") + iGlobalWarmingDefense < GC.getDefineINT("NUCLEAR_WINTER_PROB"))
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

					CvCity* pCity = GC.getMap().findCity(pPlot->getX(), pPlot->getY());
					if (pCity != NULL)
					{
						if (pPlot->isVisible(pCity->getTeam(), false))
						{
							MEMORY_TRACK_EXEMPT();

							CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_NUCLEAR_WINTER_NEAR_CITY", pCity->getNameKey());
							AddDLLMessage(pCity->getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_GLOBALWARMING", MESSAGE_TYPE_INFO, NULL, CvColorInfo::red(), pPlot->getX(), pPlot->getY(), true, true);
						}
					}
				}
			}
		}
	}
}//GWMod end M.A.
#endif // GLOBAL_WARMING


void CvGame::doHeadquarters()
{
	MEMORY_TRACE_FUNCTION();

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
			int iBestValue = MAX_INT;
			TeamTypes eBestTeam = NO_TEAM;

			for (int iJ = 0; iJ < MAX_PC_TEAMS; iJ++)
			{
				const CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iJ);
				if (kLoopTeam.isAlive())
				{
					if (NO_TECH != kCorporation.getTechPrereq() && kLoopTeam.isHasTech((TechTypes)(kCorporation.getTechPrereq())))
					{
						if (kLoopTeam.getNumCities() > 0)
						{
							bool bHasBonus = false;
							for (int i = 0; i < GC.getNUM_CORPORATION_PREREQ_BONUSES(); ++i)
							{
								if (NO_BONUS != kCorporation.getPrereqBonus(i) && kLoopTeam.hasBonus((BonusTypes)kCorporation.getPrereqBonus(i)))
								{
									bHasBonus = true;
									break;
								}
							}

							if (bHasBonus)
							{
								int iValue = getSorenRandNum(10, "Found Corporation (Team)");

								for (int iK = 0; iK < GC.getNumCorporationInfos(); iK++)
								{
									const int iCorporationCount = GET_PLAYER((PlayerTypes)iJ).getHasCorporationCount((CorporationTypes)iK);

									if (iCorporationCount > 0)
									{
										iValue += iCorporationCount * 20;
									}
								}

								if (iValue < iBestValue)
								{
									iBestValue = iValue;
									eBestTeam = ((TeamTypes)iJ);
								}
							}
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
					CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iJ);
					if (kLoopPlayer.isAlive())
					{
						if (kLoopPlayer.getTeam() == eBestTeam)
						{
							if (kLoopPlayer.getNumCities() > 0)
							{
								bool bHasBonus = false;
								for (int i = 0; i < GC.getNUM_CORPORATION_PREREQ_BONUSES(); ++i)
								{
									if (NO_BONUS != kCorporation.getPrereqBonus(i) && kLoopPlayer.hasBonus((BonusTypes)kCorporation.getPrereqBonus(i)))
									{
										bHasBonus = true;
										break;
									}
								}

								if (bHasBonus)
								{
									int iValue = getSorenRandNum(10, "Found Religion (Player)");

									if (!kLoopPlayer.isHuman())
									{
										iValue += 10;
									}

									for (int iK = 0; iK < GC.getNumCorporationInfos(); iK++)
									{
										int iCorporationCount = GET_PLAYER((PlayerTypes)iJ).getHasCorporationCount((CorporationTypes)iK);

										if (iCorporationCount > 0)
										{
											iValue += iCorporationCount * 20;
										}
									}

									if (iValue < iBestValue)
									{
										iBestValue = iValue;
										eBestPlayer = ((PlayerTypes)iJ);
									}
								}
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
	MEMORY_TRACE_FUNCTION();

	doVoteResults();

	doVoteSelection();
}


void CvGame::createBarbarianCities(bool bNeanderthal)
{
	MEMORY_TRACE_FUNCTION();

	if (getMaxCityElimination() > 0 || isOption(GAMEOPTION_NO_BARBARIANS)
	|| Cy::call<bool>(PYGameModule, "createBarbarianCities")
	|| GC.getHandicapInfo(getHandicapType()).getUnownedTilesPerBarbarianCity() <= 0
	|| (bNeanderthal ? (int)getCurrentEra() > 0 : GC.getEraInfo(getCurrentEra()).isNoBarbCities()))
	{
		return;
	}
	const int iCivCities = getNumCivCities();
	if (iCivCities < 1) { return; }

	const int iBarbPercent = GC.getGameSpeedInfo(getGameSpeedType()).getBarbPercent();

	// No barb city spawn the first X turns; X is based on difficulty and gamespeed.
	if (getElapsedGameTurns() < GC.getHandicapInfo(getHandicapType()).getBarbarianCityCreationTurnsElapsed() * iBarbPercent / 100)
	{
		return;
	}

	const PlayerTypes ePlayer = bNeanderthal ? NEANDERTHAL_PLAYER : BARBARIAN_PLAYER;

	const int iBarbCities = GET_PLAYER(ePlayer).getNumCities();

	// Hard limit on barb/civ city ratio.
	const int iMax = iCivCities + (countCivPlayersAlive() + GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()) / 2;
	if (isOption(GAMEOPTION_BARBARIAN_WORLD) ? iMax <= iBarbCities * 3 : iMax <= iBarbCities * 8)
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
	// Find plot
	int iBestValue = 0;
	CvPlot* pBestPlot = NULL;

	int iTargetCitiesMultiplier = 100;

	int iTargetBarbCities = (iCivCities * 5 * GC.getHandicapInfo(getHandicapType()).getBarbarianCityCreationProb()) / 100;
	if (iBarbCities < iTargetBarbCities)
	{
		iTargetCitiesMultiplier += (300 * (iTargetBarbCities - iBarbCities)) / iTargetBarbCities;
	}

	if (isOption(GAMEOPTION_RAGING_BARBARIANS))
	{
		iTargetCitiesMultiplier *= 3;
		iTargetCitiesMultiplier /= 2;
	}
/************************************************************************************************/
/* REVOLUTION_MOD                         04/19/08                                jdog5000      */
/*                                                                                              */
/* For BarbarianCiv, reduces emphasis on creating barb cities in                                */
/* large, occupied areas, allows barbs more readily in unoccupied areas                         */
/************************************************************************************************/
	// New variable for emphaizing spawning cities on populated continents
	int iOccupiedAreaMultiplier = 50;
	int iOwnedPlots = 0;

	if (isOption(GAMEOPTION_BARBARIAN_CIV))
	{
		for (int iI = 0; iI < GC.getMAX_PLAYERS(); iI++)
		{
			iOwnedPlots += GET_PLAYER((PlayerTypes)iI).getTotalLand();
		}

		// When map mostly open, emphasize areas with other civs
		iOccupiedAreaMultiplier += 100 - (iOwnedPlots)/GC.getMap().getLandPlots();

		// If raging barbs is on, emphasize areas with other civs
		if (isOption(GAMEOPTION_RAGING_BARBARIANS))
		{
			iOccupiedAreaMultiplier *= 3;
			iOccupiedAreaMultiplier /= 2;
		}
	}
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

	static const MapCategoryTypes earth = static_cast<MapCategoryTypes>(GC.getInfoTypeForString("MAPCATEGORY_EARTH"));

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

		if (pLoopPlot->isMapCategoryType(earth) && !pLoopPlot->isWater() && !pLoopPlot->isVisibleToCivTeam())
		{
			int iTargetCities = pLoopPlot->area()->getNumUnownedTiles();

			if (pLoopPlot->area()->getNumCities() == pLoopPlot->area()->getCitiesPerPlayer(ePlayer))
			{
				iTargetCities *= 3;
			}

			int iUnownedTilesThreshold = GC.getHandicapInfo(getHandicapType()).getUnownedTilesPerBarbarianCity();

			if (pLoopPlot->area()->getNumTiles() < iUnownedTilesThreshold / 3)
			{
				iTargetCities *= iTargetCitiesMultiplier;
				iTargetCities /= 100;
			}

			iTargetCities /= std::max(1, iUnownedTilesThreshold);

			if (pLoopPlot->area()->getCitiesPerPlayer(ePlayer) < iTargetCities)
			{
				int iValue = GET_PLAYER(ePlayer).AI_foundValue(pLoopPlot->getX(), pLoopPlot->getY(), GC.getMIN_BARBARIAN_CITY_STARTING_DISTANCE());
				iValue *= 100 + getSorenRandNum(50, "Variance");
				iValue /= 100;

				if (isOption(GAMEOPTION_BARBARIAN_CIV))
				{
					if (pLoopPlot->area()->getNumCities() == pLoopPlot->area()->getCitiesPerPlayer(ePlayer))
					{
						// Counteracts the AI_foundValue emphasis on empty areas
						iValue *= 2;
						iValue /= 3;
					}

					if (iTargetCitiesMultiplier > 100) // Either raging barbs is set or fewer barb cities than desired
					{
						// Emphasis on placing barb cities in populated areas
						iValue += (iOccupiedAreaMultiplier * (pLoopPlot->area()->getNumCities() - pLoopPlot->area()->getCitiesPerPlayer(ePlayer))) / iCivCities;
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
		GET_PLAYER(ePlayer).found(pBestPlot->getX(), pBestPlot->getY());
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
		return ((city->getPopulation() > 1) || (game->getGameTurn() - city->getGameTurnAcquired()) > (10 * GC.getGameSpeedInfo(game->getGameSpeedType()).getTrainPercent()) / 100)
			&& city->AI_getWorkersHave() == 0
			&& city->AI_getWorkersNeeded() > 0
			&& (7 * city->getPopulation()) > game->getSorenRandNum(100, "Barb - workers");
	}

	int countPlayerShipsInArea(CvArea* area)
	{
		int iPlayerSeaUnits = 0;
		for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				iPlayerSeaUnits += GET_PLAYER((PlayerTypes)iI).AI_totalWaterAreaUnitAIs(area, UNITAI_ATTACK_SEA);
				iPlayerSeaUnits += GET_PLAYER((PlayerTypes)iI).AI_totalWaterAreaUnitAIs(area, UNITAI_EXPLORE_SEA);
				iPlayerSeaUnits += GET_PLAYER((PlayerTypes)iI).AI_totalWaterAreaUnitAIs(area, UNITAI_ASSAULT_SEA);
				iPlayerSeaUnits += GET_PLAYER((PlayerTypes)iI).AI_totalWaterAreaUnitAIs(area, UNITAI_SETTLER_SEA);
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
	MEMORY_TRACE_FUNCTION();

	if (Cy::call<bool>(PYGameModule, "createBarbarianUnits"))
	{
		return;
	}

	if (isOption(GAMEOPTION_NO_BARBARIANS) || GC.getEraInfo(getCurrentEra()).isNoBarbUnits())
	{
		return;
	}

	foreach_(CvArea * pLoopArea, GC.getMap().areas())
	{
		UnitAITypes eBarbUnitAI = pLoopArea->isWater()? UNITAI_ATTACK_SEA : UNITAI_ATTACK;

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
				if (GC.getUnitInfo(eBestUnit).getDomainType() == DOMAIN_SEA)
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
	for(pLoopArea = GC.getMap().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMap().nextArea(&iLoop))
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

	MEMORY_TRACE_FUNCTION();

	CvSelectionGroup* pLoopSelectionGroup;
	int aiShuffle[MAX_PLAYERS];
	int iLoop;
	int iI;

	if (isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
	{
		shuffleArray(aiShuffle, MAX_PLAYERS, getSorenRand());
	}
	else
	{
		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			aiShuffle[iI] = iI;
		}
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayer& player = GET_PLAYER((PlayerTypes)(aiShuffle[iI]));

		if (player.isAlive())
		{
			if (player.isTurnActive())
			{
				//	For the human we want auto-moves last so the player can change
				//	orders on their automated units before hitting enter, but for the AI
				//	it is desirable to start with automated units in case the automation is broken
				//	be events, so that we know this before all other units process
				if ( player.isHuman() )
				{
					PROFILE("CvGame::updateMoves.Human");

					if (player.isAutoMoves())
					{
						if ( player.hasReadyUnautomatedUnit(false) )
						{
							//	Pre-emptorary turn end so make sure we run the unit AIs now
							player.AI_unitUpdate();
						}

						for(pLoopSelectionGroup = player.firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = player.nextSelectionGroup(&iLoop))
						{
							pLoopSelectionGroup->autoMission();
						}

						if (!(player.hasBusyUnit()))
						{
							player.setAutoMoves(false);
						}
					}
					else
					{
						if ( !player.hasReadyUnautomatedUnit(false) )
						{
							player.AI_unitUpdate();
						}

						//	A unit ready to move at this point is one the player needs to interact with
						if ( player.hasReadyUnautomatedUnit(true) )
						{
							player.setTurnHadUIInteraction(true);
						}
					}
				}
				else
				{
					PROFILE("CvGame::updateMoves.AI");

					CvPathGenerator::EnableMaxPerformance(true);

					//	Always try to do automations first for the AI
					for(pLoopSelectionGroup = player.firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = player.nextSelectionGroup(&iLoop))
					{
						FAssert(pLoopSelectionGroup != NULL);
						if (pLoopSelectionGroup != NULL)
						{
							pLoopSelectionGroup->autoMission();
						}
					}

					CvPathGenerator::EnableMaxPerformance(false);

					if (!(player.isAutoMoves()))
					{
						player.AI_unitUpdate();

						if (!(player.hasBusyUnit()) && !(player.hasReadyUnit(true)))
						{
							player.setAutoMoves(true);
						}
					}
					else
					{
						if (!(player.hasBusyUnit()))
						{
							player.setAutoMoves(false);
						}
					}
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

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		GET_PLAYER((PlayerTypes)iI).verifyAlive();
	}
}


bool CvGame::testVictory(VictoryTypes eVictory, TeamTypes eTeam, bool* pbEndScore)
{
	PROFILE_FUNC();

	FAssert(eVictory >= 0 && eVictory < GC.getNumVictoryInfos());
	FAssert(eTeam >=0 && eTeam < MAX_PC_TEAMS);
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
		else if (getElapsedGameTurns() < getMaxTurns())
		{
			bValid = false;
		}
		else
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
	}
	// Score Victory
	if (bValid && GC.getVictoryInfo(eVictory).isTargetScore())
	{
		if (getTargetScore() == 0)
		{
			bValid = false;
		}
		else if (getTeamScore(eTeam) < getTargetScore())
		{
			bValid = false;
		}
		else
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
	}
	// Conquest Victory
	if (bValid && GC.getVictoryInfo(eVictory).isConquest())
	{
		if (GET_TEAM(eTeam).getNumCities() == 0)
		{
			bValid = false;
		}
		else
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
			if (GET_PLAYER((PlayerTypes)iK).isAlive() && GET_PLAYER((PlayerTypes)iK).getTeam() == eTeam)
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
		const unsigned long long iThreshold = GET_TEAM(eTeam).countTotalCulture() * 100 / GC.getVictoryInfo(eVictory).getTotalCultureRatio();

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
	MEMORY_TRACE_FUNCTION();

	if (getVictory() != NO_VICTORY || getGameState() == GAMESTATE_EXTENDED || !Cy::call<bool>(PYGameModule, "isVictoryTest"))
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
						else
						{
							kLoopTeam.resetVictoryProgress();
						}
					}
				}
			}
		}
	}

	bool bForceEndGame = false;
	for (int iJ = 0; iJ < GC.getNumVictoryInfos(); iJ++)
	{
		if (GC.getVictoryInfo((VictoryTypes)iJ).isTotalVictory() && isVictoryValid((VictoryTypes)iJ))
		{
			aaiWinners.clear(); // sorry, folks, no winners today by usual means, only Mastery Victory is achievable;)
			if (getMaxTurns() == 0)
			{
				break;
			}//There's more game yet to play.
			if (getElapsedGameTurns() < getMaxTurns())
			{
				if (isModderGameOption(MODDERGAMEOPTION_MERCY_RULE))
				{
					int iTotalScore = 0;
					int iTopScore = 0;
					int iTempScore = 0;

					TeamTypes eBestTeam = (TeamTypes)0;
					for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
					{
						if (GET_TEAM((TeamTypes)iI).isAlive()
						&& (!GET_TEAM((TeamTypes)iI).isMinorCiv() || isOption(GAMEOPTION_START_AS_MINORS)))
						{
							iTempScore = GET_TEAM((TeamTypes)iI).getTotalVictoryScore();
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
							//Ten Turns remain!
							int iTurns = 10;
							iTurns *= GC.getGameSpeedInfo(getGameSpeedType()).getVictoryDelayPercent();
							iTurns /= 100;
							setMercyRuleCounter(iTurns);
							//Inform Players
							for (int iI = 0; iI < MAX_PLAYERS; iI++)
							{
								if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHuman())
								{
									MEMORY_TRACK_EXEMPT();

									if (GET_PLAYER((PlayerTypes)iI).getTeam() == eBestTeam)
										AddDLLMessage((PlayerTypes)iI, true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_WINNER_MERCY_RULE_HAS_BEGUN", getMercyRuleCounter()).GetCString(), "AS2D_FEAT_ACCOMPLISHED", MESSAGE_TYPE_MAJOR_EVENT, NULL, CvColorInfo::green());
									else
										AddDLLMessage((PlayerTypes)iI, true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_LOSER_MERCY_RULE_HAS_BEGUN", getMercyRuleCounter()).GetCString(), "AS2D_FIRSTTOTECH", MESSAGE_TYPE_MAJOR_EVENT, NULL, CvColorInfo::red());
								}
							}
						}
						else
						{
							changeMercyRuleCounter(-1);
							//Times Up!
							if (getMercyRuleCounter() == 0)
							{
								bForceEndGame = true;
							}
							else
							{
								for (int iI = 0; iI < MAX_PLAYERS; iI++)
								{
									if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHuman())
									{
										MEMORY_TRACK_EXEMPT();

										if (GET_PLAYER((PlayerTypes)iI).getTeam() == eBestTeam)
											AddDLLMessage((PlayerTypes)iI, true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_WINNER_MERCY_RULE_TURNS", getMercyRuleCounter()).GetCString(), "AS2D_FEAT_ACCOMPLISHED", MESSAGE_TYPE_MAJOR_EVENT, NULL, CvColorInfo::green());
										else
											AddDLLMessage((PlayerTypes)iI, true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_LOSER_MERCY_RULE_TURNS", getMercyRuleCounter()).GetCString(), "AS2D_FIRSTTOTECH", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
									}
								}
							}
						}
					}
					//Abort any existing mercy countdown
					else if (getMercyRuleCounter() > 0)
					{
						setMercyRuleCounter(0);
						for (int iI = 0; iI < MAX_PLAYERS; iI++)
						{
							if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHuman())
							{
								MEMORY_TRACK_EXEMPT();

								AddDLLMessage((PlayerTypes)iI, true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_MERCY_RULE_ABORTED").GetCString(), "AS2D_FEAT_ACCOMPLISHED", MESSAGE_TYPE_MAJOR_EVENT, NULL, CvColorInfo::green());
							}
						}
					}
				}
				else
					break;
			}
			if (getElapsedGameTurns() >= getMaxTurns() || bForceEndGame)
			{
				int topScore = 0;
				// End of game and Total Victory is selected.  Calculate the topscore
				for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
				{
					if (GET_TEAM((TeamTypes)iI).isAlive() && !GET_TEAM((TeamTypes)iI).isMinorCiv())
					{
						const int score1 = GET_TEAM((TeamTypes)iI).getTotalVictoryScore();
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
						const int score2 = GET_TEAM((TeamTypes)iI).getTotalVictoryScore();
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
	}
	if (isOption(GAMEOPTION_UNITED_NATIONS) && !m_bDiploVictoryEnabled)
	{
		//Find the diplomatic victory
		BuildingTypes eUnitedNations = NO_BUILDING;
		for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			if (GC.getBuildingInfo((BuildingTypes)iI).getVictoryPrereq() != NO_VICTORY)
			{
				if (GC.getBuildingInfo((BuildingTypes)iI).getVoteSourceType() != NO_VOTESOURCE)
				{
					eUnitedNations = (BuildingTypes)iI;
					break;
				}
			}
		}
		if (eUnitedNations != NO_BUILDING && getVictory() == GC.getBuildingInfo(eUnitedNations).getVictoryPrereq())
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
		if ((getAIAutoPlay(getActivePlayer()) > 0) || gDLL->GetAutorun())
		{
			setGameState(GAMESTATE_EXTENDED);
		}
		else
		{
			setGameState(GAMESTATE_OVER);
		}
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

	if (iChange > 0)
	{
		if (kVote.isOpenBorders())
		{
			for (int iTeam1 = 0; iTeam1 < MAX_PC_PLAYERS; ++iTeam1)
			{
				CvTeam& kLoopTeam1 = GET_TEAM((TeamTypes)iTeam1);
				if (kLoopTeam1.isVotingMember(kData.eVoteSource))
				{
					for (int iTeam2 = iTeam1 + 1; iTeam2 < MAX_PC_PLAYERS; ++iTeam2)
					{
						CvTeam& kLoopTeam2 = GET_TEAM((TeamTypes)iTeam2);
						if (kLoopTeam2.isVotingMember(kData.eVoteSource))
						{
							kLoopTeam1.signOpenBorders((TeamTypes)iTeam2);
						}
					}
				}
			}

			setVoteOutcome(kData, NO_PLAYER_VOTE);
		}
		else if (kVote.isDefensivePact())
		{
			for (int iTeam1 = 0; iTeam1 < MAX_PC_PLAYERS; ++iTeam1)
			{
				CvTeam& kLoopTeam1 = GET_TEAM((TeamTypes)iTeam1);
				if (kLoopTeam1.isVotingMember(kData.eVoteSource))
				{
					for (int iTeam2 = iTeam1 + 1; iTeam2 < MAX_PC_PLAYERS; ++iTeam2)
					{
						CvTeam& kLoopTeam2 = GET_TEAM((TeamTypes)iTeam2);
						if (kLoopTeam2.isVotingMember(kData.eVoteSource))
						{
							kLoopTeam1.signDefensivePact((TeamTypes)iTeam2);
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

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      10/02/09                                jdog5000      */
/*                                                                                              */
/* AI logging                                                                                   */
/************************************************************************************************/
			if( gTeamLogLevel >= 1 )
			{
				logBBAI("  Vote for forcing peace against team %d (%S) passes", kPlayer.getTeam(), kPlayer.getCivilizationDescription(0) );
			}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

			for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; ++iPlayer)
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
				if (kLoopPlayer.isAlive() && kLoopPlayer.getTeam() != kPlayer.getTeam())
				{
					if (kLoopPlayer.isVotingMember(kData.eVoteSource))
					{
						kLoopPlayer.forcePeace(kData.kVoteOption.ePlayer);
					}
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
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
				if (kLoopPlayer.isAlive() && kLoopPlayer.isVotingMember(kData.eVoteSource))
				{
					if (kLoopPlayer.canStopTradingWithTeam(kPlayer.getTeam()))
					{
						kLoopPlayer.stopTradingWithTeam(kPlayer.getTeam());
					}
				}
			}

			setVoteOutcome(kData, NO_PLAYER_VOTE);
		}
		else if (kVote.isForceWar())
		{
			FAssert(NO_PLAYER != kData.kVoteOption.ePlayer);
			CvPlayer& kPlayer = GET_PLAYER(kData.kVoteOption.ePlayer);

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      10/02/09                                jdog5000      */
/*                                                                                              */
/* AI logging                                                                                   */
/************************************************************************************************/
			if( gTeamLogLevel >= 1 )
			{
				logBBAI("  Vote for war against team %d (%S) passes", kPlayer.getTeam(), kPlayer.getCivilizationDescription(0) );
			}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

			for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; ++iPlayer)
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
				if (kLoopPlayer.isAlive() && kLoopPlayer.isVotingMember(kData.eVoteSource))
				{
					if (GET_TEAM(kLoopPlayer.getTeam()).canChangeWarPeace(kPlayer.getTeam()))
					{
						GET_TEAM(kLoopPlayer.getTeam()).declareWar(kPlayer.getTeam(), false, WARPLAN_DOGPILE);
					}
				}
			}

			setVoteOutcome(kData, NO_PLAYER_VOTE);
		}
		else if (kVote.isAssignCity())
		{
			FAssert(NO_PLAYER != kData.kVoteOption.ePlayer);
			CvPlayer& kPlayer = GET_PLAYER(kData.kVoteOption.ePlayer);
			CvCity* pCity = kPlayer.getCity(kData.kVoteOption.iCityId);
			FAssert(NULL != pCity);

			if (NULL != pCity)
			{
				if (NO_PLAYER != kData.kVoteOption.eOtherPlayer && kData.kVoteOption.eOtherPlayer != pCity->getOwner())
				{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      10/02/09                                jdog5000      */
/*                                                                                              */
/* AI logging                                                                                   */
/************************************************************************************************/
					if( gTeamLogLevel >= 1 )
					{
						logBBAI("  Vote for assigning %S to %d (%S) passes", pCity->getName().GetCString(), GET_PLAYER(kData.kVoteOption.eOtherPlayer).getTeam(), GET_PLAYER(kData.kVoteOption.eOtherPlayer).getCivilizationDescription(0) );
					}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
					GET_PLAYER(kData.kVoteOption.eOtherPlayer).acquireCity(pCity, false, true, true);
				}
			}

			setVoteOutcome(kData, NO_PLAYER_VOTE);
		}
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

void CvGame::logRandomResult(const wchar* szStreamName, const char* pszLog, int iMax, int iResult)
{
	if (GC.isXMLLogging() || isNetworkMultiPlayer())
	{
		if (isFinalInitialized())
		{
			static int				iLine = 0;
			TCHAR szFile[1024];
			sprintf(szFile, "RandomLogger - Player %d - Set %d.log", getActivePlayer(), getGameTurn()/50);
			TCHAR szOut[1024];

			sprintf(szOut, "%d\t%d\t%S\t%s\t%d\t%d\n", ++iLine, getGameTurn()+1, szStreamName, pszLog, iMax, iResult);
			gDLL->logMsg(szFile,szOut, false, false);
		}
	}
}

int CvGame::getSorenRandNum(int iNum, const char* pszLog)
{
/*************************************************************************************************/
/**	Xienwolf Tweak							12/10/08											**/
/**																								**/
/**					Reveals all Random checks if Debugging is enabled							**/
/*************************************************************************************************/
/**								---- Start Original Code ----									**
	return m_sorenRand.get(iNum, pszLog);
/**								----  End Original Code  ----									**/
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
/*************************************************************************************************/
/**	Tweak									END													**/
/*************************************************************************************************/
}


int CvGame::calculateSyncChecksum()
{
	PROFILE_FUNC();

	int iJ;

	int64_t iValue = 0;

	iValue += getMapRand().getSeed();
	iValue += getSorenRand().getSeed();

	iValue += getNumCities();
	iValue += getTotalPopulation();
	iValue += getNumDeals();

	iValue += GC.getMap().getOwnedPlots();
	iValue += GC.getMap().getNumAreas();

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
				iMultiplier += (GET_PLAYER((PlayerTypes)iI).getGold() % 1000000);
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
				for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).calculateTotalYield((YieldTypes)iJ));
				}

				for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
				{
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).getCommerceRate((CommerceTypes)iJ));
				}

				foreach_(const CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
				{
					for (iJ = 0; iJ < GC.getNumEventInfos(); iJ++)
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
				for (iJ = 0; iJ < GC.getNumBonusInfos(); iJ++)
				{
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).getNumAvailableBonuses((BonusTypes)iJ) * 945);
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).getBonusImport((BonusTypes)iJ) * 326);
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).getBonusExport((BonusTypes)iJ) * 932);
				}

				for (iJ = 0; iJ < GC.getNumImprovementInfos(); iJ++)
				{
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).getImprovementCount((ImprovementTypes)iJ) * 883);
				}

				for (iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
				{
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).getBuildingCountPlusMaking((BuildingTypes)iJ) * 95);
				}

				for (iJ = 0; iJ < GC.getNumUnitInfos(); iJ++)
				{
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).getUnitCountPlusMaking((UnitTypes)iJ) * 75);
				}

				for (iJ = 0; iJ < NUM_UNITAI_TYPES; iJ++)
				{
					iMultiplier += (GET_PLAYER((PlayerTypes)iI).AI_totalUnitAIs((UnitAITypes)iJ) * 64);
				}

				foreach_(const CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
				{
					for (iJ = 0; iJ < GC.getNumReligionInfos(); iJ++)
					{
						if (pLoopCity->isHasReligion((ReligionTypes)iJ))
							iMultiplier += pLoopCity->getID() * (iJ + 1);
					}
					for (iJ = 0; iJ < GC.getNumCorporationInfos(); iJ++)
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

			if (iMultiplier != 0)
			{
				iValue *= iMultiplier;
			}
		}
	}

	return (int)iValue;
}


int CvGame::calculateOptionsChecksum()
{
	PROFILE_FUNC();

	int iValue = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
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
	int iGameTurn = getGameTurn();
	CvReplayMessage* pMessage = new CvReplayMessage(iGameTurn, eType, ePlayer);
	if (NULL != pMessage)
	{
		pMessage->setPlot(iPlotX, iPlotY);
		pMessage->setText(pszText);
		if (NO_COLOR == eColor)
		{
			eColor = CvColorInfo::white();
		}
		pMessage->setColor(eColor);
		m_listReplayMessages.push_back(pMessage);
	}
}

void CvGame::clearReplayMessageMap()
{
	for (ReplayMessageList::const_iterator itList = m_listReplayMessages.begin(); itList != m_listReplayMessages.end(); ++itList)
	{
		const CvReplayMessage* pMessage = *itList;
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
		return (-1);
	}
	const CvReplayMessage* pMessage =  m_listReplayMessages[i];
	if (NULL == pMessage)
	{
		return (-1);
	}
	return pMessage->getTurn();
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
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

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
	WRAPPER_READ(wrapper,"CvGame",&m_iWaterAnimalSpawnChance);
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
				const CvReligionInfo&	newReligion = GC.getReligionInfo((ReligionTypes)iI);
				const TechTypes eFoundingTech = (TechTypes)newReligion.getTechPrereq();

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

	if (isOption(GAMEOPTION_NEW_RANDOM_SEED) && !GC.getDISABLE_RAND_SEED_ON_LOAD())
	{
		if (!isNetworkMultiPlayer())
		{
			m_sorenRand.reseed(timeGetTime());
		}
	}

	WRAPPER_READ(wrapper,"CvGame",&m_iShrineBuildingCount);
	//	Retain bug-compatibility with the old format which needlessly saved the entire max
	//	size the array could possibly be here
	if ( wrapper.isUsingTaggedFormat() )
	{
		WRAPPER_READ_CLASS_ENUM_ARRAY_ALLOW_MISSING(wrapper,"CvGame",REMAPPED_CLASS_TYPE_BUILDINGS,m_iShrineBuildingCount, m_aiShrineBuilding);
		WRAPPER_READ_CLASS_ENUM_ARRAY_ALLOW_MISSING(wrapper,"CvGame",REMAPPED_CLASS_TYPE_RELIGIONS,m_iShrineBuildingCount, m_aiShrineReligion);
	}
	else
	{
		WRAPPER_READ_CLASS_ENUM_ARRAY(wrapper,"CvGame",REMAPPED_CLASS_TYPE_BUILDINGS,GC.getNumBuildingInfos(), m_aiShrineBuilding);
		WRAPPER_READ_CLASS_ENUM_ARRAY(wrapper,"CvGame",REMAPPED_CLASS_TYPE_RELIGIONS,GC.getNumBuildingInfos(), m_aiShrineReligion);
	}
	WRAPPER_READ(wrapper,"CvGame",&m_iNumCultureVictoryCities);
	WRAPPER_READ(wrapper,"CvGame",&m_eCultureVictoryCultureLevel);

	m_Properties.readWrapper(pStream);

	setFutureEras();
	int iCurrentHandicap = range(getHandicapType(), 0, GC.getNumHandicapInfos() - 1);
	setHandicapType((HandicapTypes)iCurrentHandicap);

	m_plotGroupHashesInitialized = false;	//	Force calculation in first update timeslice

	//Example of how to skip element
	//WRAPPER_SKIP_ELEMENT(wrapper,"CvGame",m_bCircumnavigated, SAVE_VALUE_ANY);
	WRAPPER_READ_CLASS_ARRAY(wrapper,"CvGame", REMAPPED_CLASS_TYPE_IMPROVEMENTS, GC.getNumImprovementInfos(), m_paiImprovementCount);
	WRAPPER_READ_CLASS_ARRAY(wrapper,"CvGame", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_paiTechGameTurnDiscovered);
	WRAPPER_READ_OBJECT_END(wrapper);

	//establish improvement costs
	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		GC.getImprovementInfo((ImprovementTypes)iI).setHighestCost();
	}
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
	// < M.A.D. Nukes Start >
	WRAPPER_WRITE(wrapper, "CvGame", m_iLastNukeStrikeX);
	WRAPPER_WRITE(wrapper, "CvGame", m_iLastNukeStrikeY);
	// < M.A.D. Nukes End   >

/************************************************************************************************/
/* Afforess                                     12/7/09                                         */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	WRAPPER_WRITE(wrapper, "CvGame", m_iCurrentVoteID);
	WRAPPER_WRITE(wrapper, "CvGame", m_iWaterAnimalSpawnChance);
	WRAPPER_WRITE(wrapper, "CvGame", m_iCutLosersCounter);
	WRAPPER_WRITE(wrapper, "CvGame", m_iHighToLowCounter);
	WRAPPER_WRITE(wrapper, "CvGame", m_iIncreasingDifficultyCounter);
	WRAPPER_WRITE(wrapper, "CvGame", m_iMercyRuleCounter);
	WRAPPER_WRITE(wrapper, "CvGame", m_bDiploVictoryEnabled);
	WRAPPER_WRITE_ARRAY(wrapper, "CvGame", MAX_PLAYERS, m_aiFlexibleDifficultyTimer);
/************************************************************************************************/
/* Afforess	                         END                                                        */
/************************************************************************************************/

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
/************************************************************************************************/
/* Afforess                                     12/7/09                                         */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	WRAPPER_WRITE_ARRAY(wrapper, "CvGame", MAX_TEAMS, starshipLaunched);
	WRAPPER_WRITE_ARRAY(wrapper, "CvGame", MAX_TEAMS, diplomaticVictoryAchieved);
/************************************************************************************************/
/* Afforess	                         END                                                        */
/************************************************************************************************/
	WRAPPER_WRITE_ARRAY(wrapper, "CvGame", MAX_TEAMS, m_aiRankTeam);
	WRAPPER_WRITE_ARRAY(wrapper, "CvGame", MAX_TEAMS, m_aiTeamRank);
	WRAPPER_WRITE_ARRAY(wrapper, "CvGame", MAX_TEAMS, m_aiTeamScore);
/************************************************************************************************/
/* REVOLUTION_MOD                                                                 lemmy101      */
/*                                                                                jdog5000      */
/*                                                                                              */
/************************************************************************************************/
	WRAPPER_WRITE_ARRAY(wrapper, "CvGame", MAX_PLAYERS, m_iAIAutoPlay);
	WRAPPER_WRITE_ARRAY(wrapper, "CvGame", MAX_PLAYERS, m_iForcedAIAutoPlay);
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

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

/************************************************************************************************/
/* RevDCM	                  Start		 4/29/10                                                */
/*                                                                                              */
/* OC_LIMITED_RELIGIONS                                                                         */
/************************************************************************************************/
	WRAPPER_WRITE(wrapper, "CvGame", m_bGameStart);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvGame", REMAPPED_CLASS_TYPE_TECHS, GC.getNumTechInfos(), m_abTechCanFoundReligion);
/************************************************************************************************/
/* LIMITED_RELIGIONS               END                                                          */
/************************************************************************************************/

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
	CvWString szBuffer;

	long iHours = getMinutesPlayed() / 60;
	long iMinutes = getMinutesPlayed() % 60;

	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		CvPlayer& player = GET_PLAYER((PlayerTypes)iI);
		if (player.isHuman())
		{
			addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, (PlayerTypes)iI, gDLL->getText("TXT_KEY_MISC_TIME_SPENT", iHours, iMinutes));

			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_TEXT);
			if (NULL != pInfo)
			{
				if ((getWinner() != NO_TEAM) && (getVictory() != NO_VICTORY))
				{
					pInfo->setText(gDLL->getText("TXT_KEY_GAME_WON", GET_TEAM(getWinner()).getName().GetCString(), GC.getVictoryInfo(getVictory()).getTextKeyWide()));
				}
				else
				{
					pInfo->setText(gDLL->getText("TXT_KEY_MISC_DEFEAT"));
				}
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

/************************************************************************************************/
/* REVOLUTION_MOD                                                                 jdog5000      */
/*                                                                                lemmy101      */
/*                                                                                              */
/************************************************************************************************/
//
// for logging
//
void CvGame::logMsg(char* format, ... )
{
	static char buf[2048];
	_vsnprintf( buf, 2048-4, format, (char*)(&format+1) );
	gDLL->logMsg("ANewDawn.log", buf);
}

void CvGame::logDebugMsg(char* format, ...)
{
#ifdef _DEBUG
	TCHAR szOut[1024];
	if (getActivePlayer() != -1)
		sprintf(szOut, "Player %d - Multiplayer Game Log.log", getActivePlayer());
	else
		sprintf(szOut, "Pitboss Multiplayer Game Log.log");

	static char buf[2048];
	_vsnprintf(buf, 2048 - 4, format, (char*)(&format + 1));
	gDLL->logMsg(szOut, buf);
#endif
}

void CvGame::logMsgTo(const TCHAR* logFile, char* format, ...)
{
	static char buf[2048];
	_vsnprintf(buf, 2048 - 4, format, (char*)(&format + 1));
	gDLL->logMsg(logFile, buf);
}

//
//
//
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
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
bool CvGame::isCompetingCorporation(CorporationTypes eCorporation1, CorporationTypes eCorporation2) const
{
/************************************************************************************************/
/* Afforess	                  Start		 02/09/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (GC.getCorporationInfo(eCorporation1).isCompetingCorporation(eCorporation2) || GC.getCorporationInfo(eCorporation2).isCompetingCorporation(eCorporation1))
		return true;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	for (int i = 0; i < GC.getNUM_CORPORATION_PREREQ_BONUSES(); ++i)
	{
		if (GC.getCorporationInfo(eCorporation1).getPrereqBonus(i) != NO_BONUS)
		{
			for (int j = 0; j < GC.getNUM_CORPORATION_PREREQ_BONUSES(); ++j)
			{
				if (GC.getCorporationInfo(eCorporation2).getPrereqBonus(j) != NO_BONUS)
				{
					if (GC.getCorporationInfo(eCorporation1).getPrereqBonus(i) == GC.getCorporationInfo(eCorporation2).getPrereqBonus(j))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

int CvGame::getPlotExtraYield(int iX, int iY, YieldTypes eYield) const
{
	for (std::vector<PlotExtraYield>::const_iterator it = m_aPlotExtraYields.begin(); it != m_aPlotExtraYields.end(); ++it)
	{
		if ((*it).m_iX == iX && (*it).m_iY == iY)
		{
			return (*it).m_aeExtraYield[eYield];
		}
	}

	return 0;
}

void CvGame::setPlotExtraYield(int iX, int iY, YieldTypes eYield, int iExtraYield)
{
	bool bFound = false;

	for (std::vector<PlotExtraYield>::iterator it = m_aPlotExtraYields.begin(); it != m_aPlotExtraYields.end(); ++it)
	{
		if ((*it).m_iX == iX && (*it).m_iY == iY)
		{
			(*it).m_aeExtraYield[eYield] += iExtraYield;
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

int CvGame::getPlotExtraCost(int iX, int iY) const
{
	for (std::vector<PlotExtraCost>::const_iterator it = m_aPlotExtraCosts.begin(); it != m_aPlotExtraCosts.end(); ++it)
	{
		if ((*it).m_iX == iX && (*it).m_iY == iY)
		{
			return (*it).m_iCost;
		}
	}

	return 0;
}

void CvGame::changePlotExtraCost(int iX, int iY, int iCost)
{
	bool bFound = false;

	for (std::vector<PlotExtraCost>::iterator it = m_aPlotExtraCosts.begin(); it != m_aPlotExtraCosts.end(); ++it)
	{
		if ((*it).m_iX == iX && (*it).m_iY == iY)
		{
			(*it).m_iCost += iCost;
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
	m_mapVoteSourceReligions[eVoteSource] = eReligion;

	if (bAnnounce && NO_RELIGION != eReligion)
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_VOTE_SOURCE_RELIGION", GC.getReligionInfo(eReligion).getTextKeyWide(), GC.getReligionInfo(eReligion).getAdjectiveKey(), GC.getVoteSourceInfo(eVoteSource).getTextKeyWide());

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				MEMORY_TRACK_EXEMPT();

				AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getReligionInfo(eReligion).getSound(), MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
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
	FAssertMsg(eIndex >= 0 && eIndex < m_iShrineBuildingCount, "invalid index to CvGame::getShrineBuilding");

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
	FAssertMsg(eBuilding >= 0 && eBuilding < GC.getNumBuildingInfos(), "invalid index to CvGame::changeShrineBuilding");
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
	int iThreshold = GC.getCultureLevelInfo(eLevel).getSpeedThreshold(getGameSpeedType());
	if (isOption(GAMEOPTION_NO_ESPIONAGE))
	{
		//Alberts2: made this a long because a integer overflow is possible here.
		long long lThreshold = iThreshold;
		lThreshold *= 100 + GC.getDefineINT("NO_ESPIONAGE_CULTURE_LEVEL_MODIFIER");
		lThreshold /= 100;
		iThreshold = static_cast<int>(lThreshold);
	}
	return iThreshold;
}

void CvGame::doUpdateCacheOnTurn()
{
	MEMORY_TRACE_FUNCTION();

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

							if ( kPlayer1.isAlive() )
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
		CvWString szBuffer;
		CvWString szMessage;
		VoteTypes eVote = pVoteTriggered->kVoteOption.eVote;
		VoteSourceTypes eVoteSource = pVoteTriggered->eVoteSource;
		bool bPassed = false;

		if (!canDoResolution(eVoteSource, pVoteTriggered->kVoteOption))
		{
			for (int iPlayer = 0; iPlayer < MAX_PC_PLAYERS; ++iPlayer)
			{
				CvPlayer& kPlayer = GET_PLAYER((PlayerTypes) iPlayer);
				if (kPlayer.isAlive() && kPlayer.isVotingMember(eVoteSource))
				{
					MEMORY_TRACK_EXEMPT();

					CvWString szMessage;
					szMessage.Format(L"%s: %s", gDLL->getText("TXT_KEY_ELECTION_CANCELLED").GetCString(), GC.getVoteInfo(eVote).getDescription());
					AddDLLMessage((PlayerTypes)iPlayer, false, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_NEW_ERA", MESSAGE_TYPE_INFO, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
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

			if (!bAllVoted)
			{
				continue;
			}

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
						if (GET_PLAYER((PlayerTypes)iJ).isAlive() && GET_PLAYER((PlayerTypes)iJ).isVotingMember(eVoteSource))
						{
							if (getPlayerVote(((PlayerTypes)iJ), pVoteTriggered->getID()) == (PlayerVoteTypes)iI)
							{
								szBuffer += NEWLINE + gDLL->getText("TXT_KEY_POPUP_VOTES_FOR", GET_PLAYER((PlayerTypes)iJ).getNameKey(), GET_TEAM((TeamTypes)iI).getName().GetCString(), GET_PLAYER((PlayerTypes)iJ).getVotes(eVote, eVoteSource));
							}
						}
					}
				}

				for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive() && GET_PLAYER((PlayerTypes)iJ).isVotingMember(eVoteSource))
					{
						if (getPlayerVote(((PlayerTypes)iJ), pVoteTriggered->getID()) == PLAYER_VOTE_ABSTAIN)
						{
							szBuffer += NEWLINE + gDLL->getText("TXT_KEY_POPUP_ABSTAINS", GET_PLAYER((PlayerTypes)iJ).getNameKey(), GET_PLAYER((PlayerTypes)iJ).getVotes(eVote, eVoteSource));
						}
					}
				}

				if (NO_TEAM != eTeam && bPassed)
				{
					setVoteOutcome(*pVoteTriggered, (PlayerVoteTypes)eTeam);
				}
				else
				{
					setVoteOutcome(*pVoteTriggered, PLAYER_VOTE_ABSTAIN);
				}
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
						if (GET_PLAYER((PlayerTypes)iJ).isAlive() && GET_PLAYER((PlayerTypes)iJ).isVotingMember(eVoteSource))
						{
							if (getPlayerVote(((PlayerTypes)iJ), pVoteTriggered->getID()) == PLAYER_VOTE_YES)
							{
								GET_PLAYER((PlayerTypes)iJ).setEndorsedResolution(eVoteSource, pVoteTriggered->kVoteOption);
							}
						}
					}
				}

				szBuffer += NEWLINE + gDLL->getText((bPassed ? "TXT_KEY_POPUP_DIPLOMATIC_VOTING_SUCCEEDS" : "TXT_KEY_POPUP_DIPLOMATIC_VOTING_FAILURE"), GC.getVoteInfo(eVote).getTextKeyWide(), countVote(*pVoteTriggered, PLAYER_VOTE_YES), getVoteRequired(eVote, eVoteSource), countPossibleVote(eVote, eVoteSource));

				for (int iI = PLAYER_VOTE_NEVER; iI <= PLAYER_VOTE_YES; ++iI)
				{
					for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
					{
						if (GET_PLAYER((PlayerTypes)iJ).isAlive() && GET_PLAYER((PlayerTypes)iJ).isVotingMember(eVoteSource))
						{
							if (getPlayerVote(((PlayerTypes)iJ), pVoteTriggered->getID()) == (PlayerVoteTypes)iI)
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
									FAssert(false);
									break;
								}
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
						MEMORY_TRACK_EXEMPT();

						CvWString szMessage = gDLL->getText("TXT_KEY_VOTE_RESULTS", GC.getVoteSourceInfo(eVoteSource).getTextKeyWide(), pVoteTriggered->kVoteOption.szText.GetCString());
						AddDLLMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_NEW_ERA", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
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
		VoteSourceTypes eVoteSource = (VoteSourceTypes)iI;

		if (isDiploVote(eVoteSource))
		{
			if (getVoteTimer(eVoteSource) > 0)
			{
				changeVoteTimer(eVoteSource, -1);
			}
			else
			{
/************************************************************************************************/
/* Afforess	                  Start		 06/26/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
				setVoteTimer(eVoteSource, (GC.getVoteSourceInfo(eVoteSource).getVoteInterval() * GC.getGameSpeedInfo(getGameSpeedType()).getVictoryDelayPercent()) / 100);
*/
				setVoteTimer(eVoteSource, (GC.getVoteSourceInfo(eVoteSource).getVoteInterval() * GC.getGameSpeedInfo(getGameSpeedType()).getTrainPercent()) / 100);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
				for (int iTeam1 = 0; iTeam1 < MAX_PC_TEAMS; ++iTeam1)
				{
					CvTeam& kTeam1 = GET_TEAM((TeamTypes)iTeam1);

					if (kTeam1.isAlive() && kTeam1.isVotingMember(eVoteSource))
					{
						for (int iTeam2 = iTeam1 + 1; iTeam2 < MAX_PC_TEAMS; ++iTeam2)
						{
							CvTeam& kTeam2 = GET_TEAM((TeamTypes)iTeam2);

							if (kTeam2.isAlive() && kTeam2.isVotingMember(eVoteSource))
							{
								kTeam1.meet((TeamTypes)iTeam2, true);
							}
						}
					}
				}

				TeamTypes eSecretaryGeneral = getSecretaryGeneral(eVoteSource);
				PlayerTypes eSecretaryPlayer;

				if (eSecretaryGeneral != NO_TEAM)
				{
					eSecretaryPlayer = GET_TEAM(eSecretaryGeneral).getSecretaryID();
				}
				else
				{
					eSecretaryPlayer = NO_PLAYER;
				}

				bool bSecretaryGeneralVote = false;
				if (canHaveSecretaryGeneral(eVoteSource))
				{
					if (getSecretaryGeneralTimer(eVoteSource) > 0)
					{
						changeSecretaryGeneralTimer(eVoteSource, -1);
					}
					else
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
				}

				if (!bSecretaryGeneralVote && eSecretaryGeneral != NO_TEAM && eSecretaryPlayer != NO_PLAYER)
				{
					VoteSelectionData* pData = addVoteSelection(eVoteSource);
					if (NULL != pData)
					{
						if (GET_PLAYER(eSecretaryPlayer).isHuman())
						{
							CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSEELECTION);
							if (NULL != pInfo)
							{
								pInfo->setData1(pData->getID());
								gDLL->getInterfaceIFace()->addPopup(pInfo, eSecretaryPlayer);
							}
						}
						else
						{
							setVoteChosen(GET_TEAM(eSecretaryGeneral).AI_chooseElection(*pData), pData->getID());
						}
					}
					else
					{
						setVoteTimer(eVoteSource, 0);
					}
				}
			}
		}
	}
}

bool CvGame::isEventActive(EventTriggerTypes eTrigger) const
{
	for (std::vector<EventTriggerTypes>::const_iterator it = m_aeInactiveTriggers.begin(); it != m_aeInactiveTriggers.end(); ++it)
	{
		if (*it == eTrigger)
		{
			return false;
		}
	}

	return true;
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
	if (Cy::call_override(gDLL->getPythonIFace()->getMapScriptModule(), "isBonusIgnoreLatitude", lResult))
	{
		return lResult;
	}
	return false;
}

// BarbarianWorld
bool CvGame::foundBarbarianCity()
{
	bool bValid = true;
	int iBestValue = 0;
	int iDist = 0;
	int iValue = 0;
	CvPlot* pPlotI = NULL;
	CvPlot* pBestPlot = NULL;
	CvPlot* pLoopPlot = NULL;

	for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
	{
		pLoopPlot = GC.getMap().plotByIndex(iPlot);
		bValid = true;
		iValue = 0;
		if (pLoopPlot->isWater())
		{
			bValid = false;
		}
		else if (pLoopPlot->isImpassable())
		{
			bValid = false;
		}
		else if (pLoopPlot->isCity())
		{
			bValid = false;
		}
		else if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			bValid = false;
		}
		else
		{
			bool bOccupiedArea = false;
			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					pPlotI = GET_PLAYER((PlayerTypes)iI).getStartingPlot();

					if (pPlotI != NULL)
					{
						iDist = GC.getMap().calculatePathDistance(pPlotI, pLoopPlot);
						if (iDist >= 0)
						{
							bOccupiedArea = true;

							if (iDist < 5)
							{
								bValid = false;
							}
							else if( iDist < 10 )
							{
								iValue -= (10 - iDist)*7;
							}
						}
					}
				}
			}

			if( bOccupiedArea )
			{
				iValue += (isOption(GAMEOPTION_BARBARIAN_CIV) ? 1000 : 250);
			}
			else if (isOption(GAMEOPTION_BARBARIAN_CIV))
			{
				bValid = false;
			}
		}

		if (bValid)
		{
			iValue += GET_PLAYER(BARBARIAN_PLAYER).AI_foundValue(pLoopPlot->getX(), pLoopPlot->getY(), GC.getDefineINT("MIN_BARBARIAN_CITY_STARTING_DISTANCE"));
			iValue += getSorenRandNum(250, "Barb City Found");
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestPlot = pLoopPlot;
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


void CvGame::drawBattleEffects()
{
	if (GC.isDCM_BATTLE_EFFECTS())
	{
		CvPlot* pLoopPlot;
		for (int iPlot = 0; iPlot < GC.getMap().numPlots(); iPlot++)
		{
			pLoopPlot = GC.getMap().plotByIndex(iPlot);
			if (pLoopPlot->isBattle())
			{
				gDLL->getEngineIFace()->TriggerEffect(pLoopPlot->getBattleEffect(), pLoopPlot->getPoint(), 0);
			}
		}
	}
}

int CvGame::getWaterAnimalSpawnChance() const
{
	return m_iWaterAnimalSpawnChance;
}

void CvGame::setWaterAnimalSpawnChance(int iNewValue)
{
	m_iWaterAnimalSpawnChance = iNewValue;
}

void CvGame::changeWaterAnimalSpawnChance(int iChange)
{
	setWaterAnimalSpawnChance(getWaterAnimalSpawnChance() + iChange);
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

int CvGame::getXResolution() const
{
	return m_iXResolution;
}

void CvGame::setXResolution(int iNewValue)
{
	m_iXResolution = iNewValue;
}

void CvGame::changeXResolution(int iChange)
{
	setXResolution(getXResolution() + iChange);
}

int CvGame::getYResolution() const
{
	return m_iYResolution;
}

void CvGame::setYResolution(int iNewValue)
{
	m_iYResolution = iNewValue;
}

void CvGame::changeYResolution(int iChange)
{
	setYResolution(getYResolution() + iChange);
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
	MEMORY_TRACE_FUNCTION();

	if (!isGameMultiPlayer() && isOption(GAMEOPTION_CHALLENGE_CUT_LOSERS) && countCivPlayersAlive() > 5)
	{
		changeCutLosersCounter(1);
		if (getCutLosersCounter() >= ((GC.getDefineINT("CUT_LOSERS_TURN_INCREMENT") * GC.getGameSpeedInfo(getGameSpeedType()).getAnarchyPercent()) / 100))
		{
			GET_PLAYER(getRankPlayer(countCivPlayersAlive() -1)).setAlive(false);
			changeCutLosersCounter(getCutLosersCounter() * -1);
			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					MEMORY_TRACK_EXEMPT();

					AddDLLMessage((PlayerTypes)iI, true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_LOW_PLAYER_DROPPED").GetCString(), "AS2D_FIRSTTOTECH", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
				}
			}
		}
	}
}

void CvGame::doHightoLow()
{
	MEMORY_TRACE_FUNCTION();

	if (!isGameMultiPlayer() && isOption(GAMEOPTION_CHALLENGE_HIGH_TO_LOW))
	{
		if (getGameTurn() >= GC.getDefineINT("HIGH_TO_LOW_FIRST_TURN_CHECK") && getHighToLowCounter() < 2)
		{
			for (int iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHuman() && getPlayerRank((PlayerTypes)iI) == 0)
				{
					MEMORY_TRACK_EXEMPT();

					GC.getInitCore().reassignPlayerAdvanced((PlayerTypes)iI, getRankPlayer(countCivPlayersAlive() -1));
					changeHighToLowCounter(1);
					AddDLLMessage((PlayerTypes)iI, true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_PLAYER_REASSIGNED").GetCString(), "AS2D_WELOVEKING", MESSAGE_TYPE_MAJOR_EVENT, NULL, CvColorInfo::green());
				}
			}
		}
	}
}
void CvGame::doIncreasingDifficulty()
{
	MEMORY_TRACE_FUNCTION();

	if (isOption(GAMEOPTION_CHALLENGE_INCREASING_DIFFICULTY))
	{
		changeIncreasingDifficultyCounter(1);
		if (getIncreasingDifficultyCounter() >= ((GC.getDefineINT("INCREASING_DIFFICULTY_TURN_CHECK_INCREMENTS") * GC.getGameSpeedInfo(getGameSpeedType()).getAnarchyPercent()) / 100))
		{
			if (getHandicapType() < (GC.getNumHandicapInfos() - 1))
			{
				setHandicapType((HandicapTypes)(getHandicapType() + 1));
				changeIncreasingDifficultyCounter(getIncreasingDifficultyCounter() * -1);
				for (int iI = 0; iI < MAX_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).isHuman())
					{
						GC.getInitCore().setHandicap((PlayerTypes)iI, (HandicapTypes)getHandicapType());
						GET_PLAYER((PlayerTypes)iI).AI_makeAssignWorkDirty();
						foreach_(CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
						{
							pLoopCity->setInfoDirty(true);
						}
						MEMORY_TRACK_EXEMPT();

						AddDLLMessage((PlayerTypes)iI, true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_DIFFICULTY_INCREASED").GetCString(), "AS2D_FEAT_ACCOMPLISHED", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
					}
				}
			}
		}
	}
}

void CvGame::doFlexibleDifficulty()
{
	MEMORY_TRACE_FUNCTION();

	logMsg("doFlexibleDifficulty");
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlayerAI& kPlayer = GET_PLAYER((PlayerTypes)iI);
		CvTeamAI& kTeam = GET_TEAM(kPlayer.getTeam());
		//Check we are rebalancing the score for the team leader (and ignoring vassals)
		if (kPlayer.isAlive() && kTeam.getLeaderID() == kPlayer.getID() && !kPlayer.isNPC())
		{
			int iTurns = kPlayer.getModderOption(MODDEROPTION_FLEXIBLE_DIFFICULTY_TURN_INCREMENTS);
			int iTimer = getFlexibleDifficultyTimer((PlayerTypes)iI);

			if (iTurns <= 0 || (!kPlayer.isHuman() && isModderGameOption(MODDERGAMEOPTION_AI_USE_FLEXIBLE_DIFFICULTY)))
			{
				iTurns = GC.getDefineINT("DEFAULT_FLEXIBLE_DIFFICULTY_TURN_INCREMENTS", 25);
				iTurns *= GC.getGameSpeedInfo(getGameSpeedType()).getResearchPercent();
				iTurns /= 100;
			}

			logMsg("[Flexible Difficulty] (%d / %d) turns until next flexible difficulty check for Player: %S", iTimer, iTurns, kPlayer.getName());

			//Increase timer
			iTimer++;
			setFlexibleDifficultyTimer((PlayerTypes)iI, iTimer);

			if (iTimer >= iTurns)
			{
				logMsg("[Flexible Difficulty] Player: %S, Checking Flexible Difficulty", kPlayer.getName());

				if (kPlayer.isModderOption(MODDEROPTION_FLEXIBLE_DIFFICULTY) || (!kPlayer.isHuman() && isModderGameOption(MODDERGAMEOPTION_AI_USE_FLEXIBLE_DIFFICULTY)))
				{
					logMsg("[Flexible Difficulty] Player: %S has Flexible Difficulty Enabled", kPlayer.getName());
					int iMinHandicap = kPlayer.getModderOption(MODDEROPTION_FLEXIBLE_DIFFICULTY_MIN_DIFFICULTY);
					int iMaxHandicap = kPlayer.getModderOption(MODDEROPTION_FLEXIBLE_DIFFICULTY_MAX_DIFFICULTY);
					if (iMaxHandicap < 0)
						iMaxHandicap = MAX_INT;

					if (!kPlayer.isHuman() && isModderGameOption(MODDERGAMEOPTION_AI_USE_FLEXIBLE_DIFFICULTY))
					{
						iMinHandicap = 0;
						iMaxHandicap = GC.getNumHandicapInfos() - 1;
					}

					//Reset counter
					setFlexibleDifficultyTimer((PlayerTypes)iI, 0);

					//int iMinRank = kPlayer.getModderOption(MODDEROPTION_FLEXIBLE_DIFFICULTY_MIN_RANK);
					//int iMaxRank = kPlayer.getModderOption(MODDEROPTION_FLEXIBLE_DIFFICULTY_MAX_RANK);
					//if (iMaxRank < 0)
					//	iMaxRank = MAX_INT;
					//int iCurrentRank = getPlayerRank((PlayerTypes)iI);

					int iBestScore = MIN_INT;
					int iAliveCount = 0;
					int iTotalScore = 0;
					for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
					{
						const CvPlayerAI& pPlayer = GET_PLAYER((PlayerTypes)iJ);
						if (pPlayer.isAlive() && GET_TEAM(pPlayer.getTeam()).getLeaderID() == pPlayer.getID())
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
					double variance = 0.0;
					for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
					{
						const CvPlayerAI& pPlayer = GET_PLAYER((PlayerTypes)iJ);
						if (pPlayer.isAlive() && GET_TEAM(pPlayer.getTeam()).getLeaderID() == pPlayer.getID())
						{
							int iScore = getPlayerScore((PlayerTypes)iJ);
							//variance is sum of squared difference from mean
							variance += (iMeanScore - iScore) * (iMeanScore - iScore);
							logMsg("[Flexible Difficulty] Adding score for player %S, score: %d", pPlayer.getName(), iScore);
						}
					}
					variance /= iAliveCount;
					double stddev = sqrt(variance);

					int iCurrentScore = getPlayerScore((PlayerTypes)iI);

					logMsg("[Flexible Difficulty] Player: %S, Score: %d, Difficulty: %S, Avg Score: %d, Std Dev: %f", kPlayer.getName(), iCurrentScore, GC.getHandicapInfo((HandicapTypes)kPlayer.getHandicapType()).getDescription(), iMeanScore, stddev);

					int newHandicap = (kPlayer.getHandicapType() > iMaxHandicap) ? iMaxHandicap : ((kPlayer.getHandicapType() < iMinHandicap) ? iMinHandicap : kPlayer.getHandicapType());

					//Increased Difficulty (player's score is > 1 std dev away)
					if (iCurrentScore > (iMeanScore + stddev))
					{
						logMsg("[Flexible Difficulty] Player: %S score is > 1 std dev above average.", kPlayer.getName());
						if (newHandicap < (GC.getNumHandicapInfos() - 1) && newHandicap < iMaxHandicap)
						{
							logMsg("[Flexible Difficulty] Player: %S difficulty is increasing from %S to %S", kPlayer.getName(), GC.getHandicapInfo((HandicapTypes)kPlayer.getHandicapType()).getDescription(), GC.getHandicapInfo((HandicapTypes)(kPlayer.getHandicapType() + 1)).getDescription());

							newHandicap++;
						}
					}
					else if (iCurrentScore < (iMeanScore - stddev))
					{
						logMsg("[Flexible Difficulty] Player: %S score is > 1 std dev below average.", kPlayer.getName());
						if (newHandicap > 0 && newHandicap > iMinHandicap)
						{
							logMsg("[Flexible Difficulty] Player: %S difficulty is decreasing from %S to %S", kPlayer.getName(), GC.getHandicapInfo((HandicapTypes)kPlayer.getHandicapType()).getDescription(), GC.getHandicapInfo((HandicapTypes)(kPlayer.getHandicapType() - 1)).getDescription());

							newHandicap--;
						}
					}
					if (newHandicap != kPlayer.getHandicapType())
					{
						bool diffIncreasing = newHandicap > kPlayer.getHandicapType();
						const char* TXT_KEY_DIFFICULTY_ID = diffIncreasing ? "TXT_KEY_DIFFICULTY_INCREASED" : "TXT_KEY_DIFFICULTY_DECREASED";
						const char* TXT_KEY_DIFFICULTY_PLAYER_ID = diffIncreasing ? "TXT_KEY_DIFFICULTY_INCREASED_FOR_PLAYER" : "TXT_KEY_DIFFICULTY_DECREASED_FOR_PLAYER";
						const char* AS2D_ID = diffIncreasing ? "AS2D_FEAT_ACCOMPLISHED" : "AS2D_FIRSTTOTECH";
						const char* PLAYER_COLOR_ID = diffIncreasing ? "COLOR_WARNING_TEXT" : "COLOR_GREEN";
						//Inform all human players of any other players difficulty changes
						for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
						{
							const CvPlayerAI& player = GET_PLAYER((PlayerTypes)iJ);
							if (player.isAlive() && player.isHuman() && GET_TEAM(player.getTeam()).isHasMet(kPlayer.getTeam()))
							{
								//Difficulty increased for us
								if (iJ == iI)
								{
									MEMORY_TRACK_EXEMPT();
									AddDLLMessage((PlayerTypes)iJ, true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText(TXT_KEY_DIFFICULTY_ID).GetCString(), AS2D_ID, MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString(PLAYER_COLOR_ID));
								}
								//Difficulty increased for someone else
								else
								{
									MEMORY_TRACK_EXEMPT();
									AddDLLMessage((PlayerTypes)iJ, true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText(TXT_KEY_DIFFICULTY_PLAYER_ID, kPlayer.getCivilizationDescriptionKey()).GetCString(), AS2D_ID, MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
								}
							}
						}

						//Change the difficulty
						GC.getInitCore().setHandicap((PlayerTypes)iI, (HandicapTypes)newHandicap);

						//Clean the interface
						kPlayer.AI_makeAssignWorkDirty();
						foreach_(CvCity* pLoopCity, kPlayer.cities())
						{
							pLoopCity->setInfoDirty(true);
						}
					}
				}
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
	const FeatureTypes eForest = (FeatureTypes)GC.getInfoTypeForString("FEATURE_FOREST");
	int iMinimumForestSize = GC.getDefineINT("MINIMUM_FOREST_SIZE");
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
	const FeatureTypes eJungle = (FeatureTypes)GC.getInfoTypeForString("FEATURE_JUNGLE");
	int iMinimumJungleSize = GC.getDefineINT("MINIMUM_JUNGLE_SIZE");
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
	const TerrainTypes eDesert = (TerrainTypes)GC.getInfoTypeForString("TERRAIN_DESERT");
	int iMinimumDesertSize = GC.getDefineINT("MINIMUM_DESERT_SIZE");
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
			if (pAdjacentPlot->getTerrainType() == (TerrainTypes)GC.getInfoTypeForString("TERRAIN_DESERT") && !pAdjacentPlot->isHills() && !pAdjacentPlot->isPeak())
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
	if (iJungle > GC.getDefineINT("MINIMUM_JUNGLE_SIZE") * 2)
		return iJungle;
	foreach_(CvPlot* pAdjacentPlot, pPlot->adjacent())
	{
		if (!pAdjacentPlot->isCountedPlot())
		{
			if (pAdjacentPlot->getFeatureType() == (FeatureTypes)GC.getInfoTypeForString("FEATURE_JUNGLE"))
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
	if (iForest > GC.getDefineINT("MINIMUM_FOREST_SIZE") * 2)
		return iForest;
	foreach_(CvPlot* pAdjacentPlot, pPlot->adjacent())
	{
		if (!pAdjacentPlot->isCountedPlot())
		{
			if (pAdjacentPlot->getFeatureType() == (FeatureTypes)GC.getInfoTypeForString("FEATURE_FOREST"))
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
			szTextKey = "TXT_KEY_LANDMARK";
			szSign.clear();
			szName.clear();
			switch (pLoopPlot->getLandmarkType())
			{
				case LANDMARK_BAY:
					szTextKey.append((CvWString)"_BAY");
					szTextKey.append(CvWString::format(L"_%d", (getSorenRandNum(GC.getDefineINT("NUM_BAY_NAMES"), "Landmark Naming"))));
					break;
				case LANDMARK_ISLAND:
					break;
				case LANDMARK_FOREST:
					szTextKey.append((CvWString)"_FOREST");
					szTextKey.append(CvWString::format(L"_%d", (getSorenRandNum(GC.getDefineINT("NUM_FOREST_NAMES"), "Landmark Naming"))));
					break;
				case LANDMARK_JUNGLE:
					szTextKey.append((CvWString)"_JUNGLE");
					szTextKey.append(CvWString::format(L"_%d", (getSorenRandNum(GC.getDefineINT("NUM_JUNGLE_NAMES"), "Landmark Naming"))));
					break;
				case LANDMARK_PEAK:
					szTextKey.append((CvWString)"_PEAK");
					szTextKey.append(CvWString::format(L"_%d", (getSorenRandNum(GC.getDefineINT("NUM_PEAK_NAMES"), "Landmark Naming"))));
					break;
				case LANDMARK_MOUNTAIN_RANGE:
					szTextKey.append((CvWString)"_MOUNTAINS");
					szTextKey.append(CvWString::format(L"_%d", (getSorenRandNum(GC.getDefineINT("NUM_MOUNTAINS_NAMES"), "Landmark Naming"))));
					break;
				case LANDMARK_PLAINS:
					break;
				case LANDMARK_DESERT:
					szTextKey.append((CvWString)"_DESERT");
					szTextKey.append(CvWString::format(L"_%d", (getSorenRandNum(GC.getDefineINT("NUM_DESERT_NAMES"), "Landmark Naming"))));
					break;
				case LANDMARK_OCEAN:
				case LANDMARK_LAKE:
					szTextKey.append((CvWString)"_LAKE");
					szTextKey.append(CvWString::format(L"_%d", (getSorenRandNum(GC.getDefineINT("NUM_LAKE_NAMES"), "Landmark Naming"))));
					break;
				default:
					FAssertMsg(false, "Unknown Landmark Type");
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

//Marks nearby tiles as counted so that they will not recieve multiple signs
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
	FAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	FAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	return m_abPreviousRequest[ePlayer];
}


void CvGame::setPreviousRequest(PlayerTypes ePlayer, bool bNewValue)
{
	FAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	FAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");
	m_abPreviousRequest[ePlayer] = bNewValue;
}

bool CvGame::isModderGameOption(ModderGameOptionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_MODDERGAMEOPTION_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiModderGameOption[eIndex] > 0;
}

int CvGame::getModderGameOption(ModderGameOptionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_MODDERGAMEOPTION_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiModderGameOption[eIndex];
}

void CvGame::setModderGameOption(ModderGameOptionTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_MODDERGAMEOPTION_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiModderGameOption[eIndex] = iNewValue;
}

void CvGame::setModderGameOption(ModderGameOptionTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_MODDERGAMEOPTION_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_aiModderGameOption[eIndex] = bNewValue;
}

void CvGame::doFoundCorporations()
{
	MEMORY_TRACE_FUNCTION();

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		doFoundCorporation((CorporationTypes)iI, false);
	}
}
void CvGame::doFoundCorporation(CorporationTypes eCorporation, bool bForce)
{
	if (!isOption(GAMEOPTION_REALISTIC_CORPORATIONS))
	{
		return;
	}
	int iSpread;
	int iBestSpread;
	int iI;
	CvCity* pBestCity = NULL;


	int iCheckTurns = GC.getDefineINT("CORPORATION_FOUND_CHECK_TURNS");
	iCheckTurns *= GC.getGameSpeedInfo(getGameSpeedType()).getAnarchyPercent();
	iCheckTurns /= 100;

	if (getElapsedGameTurns() % std::max(1, iCheckTurns) != 0 && !bForce)
	{
		return;
	}

	if (!canEverSpread(eCorporation))
	{
		return;
	}

	if (GC.getGame().isModderGameOption(MODDERGAMEOPTION_NO_AUTO_CORPORATION_FOUNDING))
	{
		bool bValid = false;
		for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			if (canEverTrain((UnitTypes)iI))
			{
				if (GC.getUnitInfo((UnitTypes)iI).getCorporationSpreads(eCorporation) > 0)
				{
					bValid = true;
					break;
				}
			}
		}
		if (bValid)
		{
			return;
		}
	}

	iBestSpread = getSorenRandNum(GC.getDefineINT("RAND_CORPORATION_BEST_SPREAD"), "Corporation Founding Best Spread");

	if (getHeadquarters(eCorporation) == NULL)
	{
		TechTypes ePrereqTech = (TechTypes)GC.getCorporationInfo(eCorporation).getTechPrereq();
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
		}

		if (ePrereqTech != NO_TECH)
		{
			if (countKnownTechNumTeams(ePrereqTech) > GC.getDefineINT("MINIMUM_PLAYERS_WITH_TECH_FOR_AUTO_CORPORATION_FOUNDING"))
			{
				const TechTypes eObsoleteTech = (TechTypes)GC.getCorporationInfo(eCorporation).getObsoleteTech();
				for (iI = 0; iI < MAX_PLAYERS; iI++)
				{
					const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
					if (kPlayer.isAlive())
					{
						if (eObsoleteTech == NO_TECH || !GET_TEAM(kPlayer.getTeam()).isHasTech(eObsoleteTech))
						{
							foreach_(CvCity* pLoopCity, kPlayer.cities())
							{
								iSpread = pLoopCity->getCorporationInfluence(eCorporation);

								iSpread *= GC.getCorporationInfo(eCorporation).getSpread();

								iSpread /= 100;

								if (iSpread > 0)
								{
									//Random Luck
									iSpread += getSorenRandNum(100, "Corporation Founding Rand Luck");
									if (iSpread > iBestSpread)
									{
										pBestCity = pLoopCity;
										iBestSpread = iSpread;
									}
								}
							}
							//Discourage players with more than 2 corporations from getting more
							if (pBestCity != NULL)
							{
								if (pBestCity->getOwner() == iI)
								{
									int iCorporationCount = 0;
									for (int iJ = 0; iJ < GC.getNumCorporationInfos(); iJ++)
									{
										if (getHeadquarters((CorporationTypes)iJ) != NULL)
										{
											if (getHeadquarters((CorporationTypes)iJ)->getOwner() == iI)
											{
												iCorporationCount++;
											}
										}
									}
									iBestSpread /= std::max(1, iCorporationCount);
								}
							}
						}

					}
				}
				if (pBestCity != NULL)
				{
					if ((getSorenRandNum(GC.getDefineINT("CORPORATION_SPREAD_RAND"), "Corporation Founding Rand")  < iBestSpread) || bForce)
					{
						setHeadquarters(eCorporation, pBestCity, true);
					}
				}
			}
		}
	}
}

int CvGame::getAverageCorporationInfluence(const CvCity* pCity, const CorporationTypes eCorporation) const
{
	int iSpread = GC.getCorporationInfo(eCorporation).getSpread();
	int iTotalSpread = 0;
	int iI;
	int iPlayerCount = 0;
	int iRandThreshold = 0;

	if (getHeadquarters(eCorporation) == NULL)
	{
		return 100;
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iI);
		if (kPlayer.isAlive())
		{
			if (kPlayer.isNoCorporations())
			{
				continue;
			}
			if (kPlayer.isNoForeignCorporations() && getHeadquarters(eCorporation)->getOwner() != pCity->getOwner())
			{
				continue;
			}
			iPlayerCount++;
			foreach_(const CvCity* pLoopCity, kPlayer.cities())
			{
				if (pLoopCity->isConnectedTo(pCity))
				{
					iSpread = pLoopCity->getCorporationInfluence(eCorporation);

					iSpread *= GC.getCorporationInfo(eCorporation).getSpread();

					iSpread /= 100;

					if (iSpread > 0)
					{
						iSpread /= std::max(1, (((GC.getDefineINT("CORPORATION_SPREAD_DISTANCE_DIVISOR") * plotDistance(pCity->getX(), pCity->getY(), pLoopCity->getX(), pLoopCity->getY())) / GC.getMap().maxPlotDistance()) - 5));

						iRandThreshold = std::max(iRandThreshold, iSpread);
					}
				}
			}
			iRandThreshold *= GET_PLAYER(pCity->getOwner()).getCorporationSpreadModifier() + 100;
			iRandThreshold /= 100;
			iRandThreshold *= GET_PLAYER(pCity->getOwner()).getCorporationInfluence(eCorporation);
			iRandThreshold /= 100;

			iTotalSpread += iRandThreshold;
		}
	}

	if (iPlayerCount > 0)
	{
		iTotalSpread /= iPlayerCount;
	}
	else
	{
		iTotalSpread = 0;
	}

	return iTotalSpread;
}

bool CvGame::canEverResearch(TechTypes eTech) const
{
	if (isOption(GAMEOPTION_NO_FUTURE))
	{
		if (GC.getTechInfo(eTech).getEra() > GC.getInfoTypeForString("C2C_ERA_ATOMIC"))
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


void CvGame::setFutureEras()
{
	TechTypes eFutureTech =  NO_TECH;
	for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		if (GC.getTechInfo((TechTypes)iI).isRepeat())
		{
			eFutureTech = (TechTypes)iI;
			break;
		}
	}
	if (eFutureTech != NO_TECH)
	{
		bool bSet = false;
		for (int iI = 0; iI < GC.getNUM_OR_TECH_PREREQS(); iI++)
		{
			if (GC.getTechInfo(eFutureTech).getOriginalPrereqOrTechs(iI) != NO_TECH)
			{
				bSet = true;
				break;
			}

		}
		if (!bSet)
		{
			for (int iJ = 0; iJ < GC.getNumTechInfos(); iJ++)
			{
				for (int iI = 0; iI < GC.getNUM_OR_TECH_PREREQS(); iI++)
				{
					GC.getTechInfo((TechTypes)iJ).setOriginalPrereqOrTech(iI, GC.getTechInfo((TechTypes)iJ).getPrereqOrTechs(iI));
					GC.getTechInfo((TechTypes)iJ).setOriginalPrereqAndTech(iI, GC.getTechInfo((TechTypes)iJ).getPrereqAndTechs(iI));
				}
			}
		}
		else
		{
			for (int iJ = 0; iJ < GC.getNumTechInfos(); iJ++)
			{
				for (int iI = 0; iI < GC.getNUM_OR_TECH_PREREQS(); iI++)
				{
					GC.getTechInfo((TechTypes)iJ).setPrereqOrTech(iI, GC.getTechInfo((TechTypes)iJ).getOriginalPrereqOrTechs(iI));
					GC.getTechInfo((TechTypes)iJ).setPrereqAndTech(iI, GC.getTechInfo((TechTypes)iJ).getOriginalPrereqAndTechs(iI));
				}
			}
			int iBestX = 0;
			for (int iJ = 0; iJ < GC.getNumTechInfos(); iJ++)
			{
				if (GC.getTechInfo((TechTypes)iJ).getGridX() > iBestX)
				{
					iBestX = GC.getTechInfo((TechTypes)iJ).getGridX();
				}
			}
			GC.getTechInfo(eFutureTech).setGridX(iBestX + 1);
		}
	}
	if (isOption(GAMEOPTION_NO_FUTURE))
	{
		if (eFutureTech != NO_TECH)
		{
			//Remove existing techs prereqs
			for (int iI = 0; iI < GC.getNUM_OR_TECH_PREREQS(); iI++)
			{
				GC.getTechInfo(eFutureTech).setPrereqOrTech(iI, NO_TECH);
				GC.getTechInfo(eFutureTech).setPrereqAndTech(iI, NO_TECH);
			}
			//Find the next best tech to require the future tech
			int iBestX = 0;
			for (int iJ = 0; iJ < GC.getNumTechInfos(); iJ++)
			{
				if (canEverResearch((TechTypes)iJ) && !GC.getTechInfo((TechTypes)iJ).isRepeat())
				{
					if (GC.getTechInfo((TechTypes)iJ).getGridX() > iBestX)
					{
						iBestX = GC.getTechInfo((TechTypes)iJ).getGridX();
					}
				}
			}
			int iCount = 0;
			for (int iJ = 0; iJ < GC.getNumTechInfos(); iJ++)
			{
				if (GC.getTechInfo((TechTypes)iJ).getGridX() == iBestX)
				{
					if (canEverResearch((TechTypes)iJ))
					{
						if (iCount > GC.getNUM_OR_TECH_PREREQS())
						{
							break;
						}
						GC.getTechInfo(eFutureTech).setPrereqOrTech(iCount, iJ);
						iCount++;
					}
				}
			}
			GC.getTechInfo(eFutureTech).setGridX(iBestX + 1);
		}
	}

	//Remove Prerequisite Techs for which can never be researched
	//for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
	//{
	//	if (!canEverResearch((TechTypes)iI))
	//	{
	//		for (int iJ = 0; iJ < GC.getNumTechInfos(); iJ++)
	//		{
	//			for (int iK = 0; iK < GC.getNUM_OR_TECH_PREREQS(); iK++)
	//			{
	//				if (GC.getTechInfo((TechTypes)iJ).getPrereqOrTechs(iK) == iI)
	//				{
	//					GC.getTechInfo((TechTypes)iJ).setPrereqOrTech(iK, NO_TECH);
	//				}
	//				if (GC.getTechInfo((TechTypes)iJ).getPrereqAndTechs(iK) == iI)
	//				{
	//					GC.getTechInfo((TechTypes)iJ).setPrereqAndTech(iK, NO_TECH);
	//				}
	//			}
	//		}
	//	}
	//}
}

int CvGame::getFlexibleDifficultyTimer(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiFlexibleDifficultyTimer[eIndex];
}

void CvGame::setFlexibleDifficultyTimer(PlayerTypes eIndex, int iNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");
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

	bool bFound = false;
	bool bRequires = false;
	const int numGameSpeedInfos = GC.getNumGameSpeedInfos();
	for (int iI = 0; iI < numGameSpeedInfos; iI++)
	{
		if (kBuilding.isPrereqOrGameSpeed(iI))
		{
			bRequires = true;
			if (getGameSpeedType() == iI)
			{
				bFound = true;
				break;
			}
		}
	}
	if (!bFound && bRequires)
	{
		return false;
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

	for (int iI = 0; iI < pUnit->cargoSpace(); iI++)
	{
		UnitTypes eBestUnit = NO_UNIT;
		int iBestValue = 0;
		for (int iJ = 0; iJ < GC.getNumUnitInfos(); iJ++)
		{
			if (validBarbarianShipUnit(GC.getUnitInfo((UnitTypes) iJ), (UnitTypes) iJ))
			{
				int iValue = (1 + getSorenRandNum(1000, "Barb Unit Selection"));

				if (GC.getUnitInfo((UnitTypes) iJ).getUnitAIType(UNITAI_ATTACK))
				{
					iValue += 250;
				}

				if (iValue > iBestValue)
				{
					eBestUnit = (UnitTypes) iJ;
					iBestValue = iValue;
				}
			}
		}
		if (eBestUnit != NO_UNIT)
		{
			CvUnit* pPirate = GET_PLAYER(BARBARIAN_PLAYER).initUnit(eBestUnit, pUnit->plot()->getX(), pUnit->plot()->getY(), UNITAI_ATTACK, NO_DIRECTION, getSorenRandNum(10000, "AI Unit Birthmark"));
			pPirate->setTransportUnit(pUnit);
			pUnit->AI_setUnitAIType(UNITAI_ASSAULT_SEA);
		}
	}
}


void CvGame::recalculateModifiers()
{
	OutputDebugString(CvString::format("Start profiling(false) for modifier recalc\n").c_str());
	startProfilingDLL(false);
	PROFILE_FUNC();

	m_bRecalculatingModifiers = true;

	//establish improvement costs
	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		GC.getImprovementInfo((ImprovementTypes)iI).setHighestCost();
	}

	//	Inhibit plot group manipulation until we rebuild at the end of everything else
	for(iI = 0; iI < MAX_PLAYERS; iI++ )
	{
		if ( GET_PLAYER((PlayerTypes)iI).isAlive() )
		{
			GET_PLAYER((PlayerTypes)iI).inhibitPlotGroupCalcsUntilFullRebuild();
		}
	}

	// AIAndy: Recalculate which info class replacements are currently active
	GC.updateReplacements();

	for (iI = 0; iI < GC.getNumVoteSourceInfos(); iI++)
	{
		m_aiDiploVote[iI] = 0;
	}

	for (iI = 0; iI < GC.getNumSpecialUnitInfos(); iI++)
	{
		m_pabSpecialUnitValid[iI] = false;
		if (GC.getSpecialUnitInfo((SpecialUnitTypes)iI).isValid())
		{
			makeSpecialUnitValid((SpecialUnitTypes)iI);
		}
	}

	for (iI = 0; iI < GC.getNumSpecialBuildingInfos(); iI++)
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

	for (iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);
		pLoopPlot->getProperties()->clearForRecalculate();

		//	We will recalculate visibility from first principles
		pLoopPlot->clearVisibilityCounts();

		//	Fix any spurious routes that are on water tiles
		const RouteTypes eRoute = pLoopPlot->getRouteType();
		if (eRoute != NO_ROUTE && pLoopPlot->isWater() && !GC.getRouteInfo(eRoute).isSeaTunnel() )
		{
			pLoopPlot->setRouteType(NO_ROUTE, false);
		}
		pLoopPlot->unitGameStateCorrections();

		//	Recalc blockades from scratch
		pLoopPlot->resetBlockadedCounts();
	}

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		if ( GET_TEAM((TeamTypes)iI).isAlive() )
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

	foreach_(CvArea * pLoopArea, GC.getMap().areas())
	{
		pLoopArea->clearModifierTotals();
	}

	for(iI = 0; iI < MAX_TEAMS; iI++)
	{
		if ( GET_TEAM((TeamTypes)iI).isAlive() )
		{
			GET_TEAM((TeamTypes)iI).recalculateModifiers();
		}
	}

	for(iI = 0; iI < GC.getNumVoteSourceInfos(); iI++ )
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
	for (iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.isAlive())
		{
			foreach_(CvCity* pLoopCity, kLoopPlayer.cities())
			{
				pLoopCity->doVicinityBonus();
				pLoopCity->checkBuildings(true, true, true, true, true, false);
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
	for (iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.isAlive())
		{
			foreach_(CvCity* pLoopCity, kLoopPlayer.cities())
			{
				pLoopCity->updateBuildingCommerce();
				pLoopCity->updateCommerce(NO_COMMERCE, true);
			}
		}
	}

	stopProfilingDLL(false);
	OutputDebugString(CvString::format("Stop profiling(false) after modifier recalc\n").c_str());
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
	FAssert(m_eCurrentMap > NO_MAP);
	FAssert(m_eCurrentMap < GC.getNumMapInfos());
	return m_eCurrentMap;
}

void CvGame::setCurrentMap(MapTypes eNewMap)
{
	FAssert(eNewMap > NO_MAP);
	FAssert(eNewMap < GC.getNumMapInfos());
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
	const int multiplier = std::max(1, GC.getDefineINT("SIZE_MATTERS_MOST_VOLUMETRIC_MULTIPLIER", 1));
	int iBase = 10000 / multiplier;
	return iBase;
}

int CvGame::getBaseMissileUnitIncrementsbyCargoVolume() const
{
	const int multiplier = std::max(1, GC.getDefineINT("SIZE_MATTERS_MOST_VOLUMETRIC_MULTIPLIER", 1));
	int iBase = 10000 / (multiplier * multiplier * multiplier);
	return std::max(1, iBase);
}

bool CvGame::isEarlyGame() const
{
	int iEarlyTurnThreshold = GC.getDefineINT("EARLY_GAME_TURN_THRESHOLD", 50);
	iEarlyTurnThreshold *= GC.getGameSpeedInfo(getGameSpeedType()).getGrowthPercent();
	iEarlyTurnThreshold /= 100;
	return getElapsedGameTurns() < iEarlyTurnThreshold;
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
	if (GC.isXMLLogging() || isNetworkMultiPlayer())
	{
		TCHAR szFile[1024];
		sprintf(szFile, "OOSSpecialLogger - Player %d - Set %d.log", getActivePlayer(), getGameTurn()/50);
		TCHAR szOut[1024];
		sprintf(szOut, "iLocID %d - iVar %d - iVar2 %d - iVar3 %d", iLocID, iVar, iVar2, iVar3);
		gDLL->logMsg(szFile,szOut, false, false);
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
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumImprovementInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiImprovementCount[eIndex];
}


void CvGame::changeImprovementCount(ImprovementTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumImprovementInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiImprovementCount[eIndex] = (m_paiImprovementCount[eIndex] + iChange);
	FAssert(getImprovementCount(eIndex) >= 0);
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
