#pragma once

// game.h

#ifndef CIV4_GAME_H
#define CIV4_GAME_H

#include "CvDeal.h"
#include "CvRandom.h"
#include "CvGameObject.h"
#include "CvProperties.h"
#include "CvPropertySolver.h"
#include "CvDate.h"
#include "CvAllocator.h"

class CvArtInfoBuilding;
class CvCity;
class CvPlot;
class CvReplayMessage;
class CvReplayInfo;
class CvUnit;
class CvUnitCombatInfo;

//	Max number of barbarian units in existence for a spawn of a new one to be allowed
//	This allows a 'space' for 'real' barbarians to be built before we use up the entire
//	simultaneous unit space of 8191 for any one player
#define	MAX_BARB_UNITS_FOR_SPAWNING	7000

typedef std::vector<const CvReplayMessage*> ReplayMessageList;

class CvGame : bst::noncopyable
{
public:

	CvGame();
	virtual ~CvGame();

	DllExport void init(HandicapTypes eHandicap);
	DllExport void reset(HandicapTypes eHandicap, bool bConstructorCall = false);

	CvGameObjectGame* getGameObject() {return &m_GameObject;};

protected:

	CvGameObjectGame m_GameObject;

	void uninit();

public:

	DllExport void setInitialItems();

	bool canRegenerateMap() const;
	DllExport void regenerateMap();

	DllExport void initDiplomacy();
	void initFreeState();
	DllExport void initFreeUnits();

	void assignStartingPlots(const bool bScenario = false, const bool bMapScript = false);
	void normalizeStartingPlots();

	DllExport void update();
	void updateScore(bool bForce = false);

	DllExport void updateColoredPlots();
	DllExport void updateBlockadedPlots();

	void updatePlotGroups(bool reInitialize = false);
	void updateBuildingCommerce();
	void updateCitySight(bool bIncrement);
	void updateTradeRoutes();

	DllExport void updateSelectionList();
	DllExport void updateTestEndTurn();

	DllExport void testExtendedGame();

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	MapTypes getCurrentMap() const;
	void setCurrentMap(MapTypes eNewMap);

	void updateSelectionListInternal(bool bSetCamera, bool bAllowViewportSwitch, bool bForceAcceptCurrent = false);
	void cycleSelectionGroupsInternal(bool bClear, bool bForward, bool bWorkers, bool bSetCamera, bool bAllowViewportSwitch) const;

	void processGreatWall(bool bIn, bool bForce = false, bool bSeeded = true) const;
	void noteGraphicRebuildNeeded();
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

	DllExport CvUnit* getPlotUnit(const CvPlot* pPlot, int iIndex) const;
	DllExport void getPlotUnits(const CvPlot *pPlot, std::vector<CvUnit*>& plotUnits) const;

	DllExport void cycleCities(bool bForward = true, bool bAdd = false) const;
	void cycleSelectionGroups(bool bClear, bool bForward = true, bool bWorkers = false) const;
	DllExport bool cyclePlotUnits(const CvPlot* pPlot, bool bForward = true, bool bAuto = false, int iCount = -1) const;
	DllExport bool selectCity(CvCity* pSelectCity, bool bCtrl, bool bAlt, bool bShift) const;

	DllExport void selectionListMove(CvPlot* pPlot, bool bAlt, bool bShift, bool bCtrl) const;
	DllExport void selectionListGameNetMessage(int eMessage, int iData2 = -1, int iData3 = -1, int iData4 = -1, int iFlags = 0, bool bAlt = false, bool bShift = false) const;
	void selectionListGameNetMessageInternal(int eMessage, int iData2, int iData3, int iData4, int iFlags, bool bAlt, bool bShift, bool bInViewportCoordinates) const;
	DllExport void selectedCitiesGameNetMessage(int eMessage, int iData2 = -1, int iData3 = -1, int iData4 = -1, bool bOption = false, bool bAlt = false, bool bShift = false, bool bCtrl = false) const;
	void cityPushOrder(const CvCity* pCity, OrderTypes eOrder, int iData, bool bAlt = false, bool bShift = false, bool bCtrl = false) const;

	DllExport void selectUnit(CvUnit* pUnit, bool bClear, bool bToggle = false, bool bSound = false) const;
	DllExport void selectGroup(CvUnit* pUnit, bool bShift, bool bCtrl, bool bAlt) const;
	DllExport void selectAll(CvPlot* pPlot) const;

	DllExport bool selectionListIgnoreBuildingDefense() const;

	DllExport bool canHandleAction(int iAction, CvPlot* pPlot = NULL, bool bTestVisible = false, bool bUseCache = false) const;
	DllExport void setupActionCache() const;
	DllExport void handleAction(int iAction);

	bool canDoControl(ControlTypes eControl) const;
	void doControl(ControlTypes eControl);

	DllExport void implementDeal(PlayerTypes eWho, PlayerTypes eOtherWho, CLinkList<TradeData>* pOurList, CLinkList<TradeData>* pTheirList, bool bForce = false);
	void verifyDeals();

	DllExport void getGlobeviewConfigurationParameters(TeamTypes eTeam, bool& bStarsVisible, bool& bWorldIsRound);

	int getSymbolID(int iSymbol);

	int getProductionPerPopulation(HurryTypes eHurry) const;

	int getAdjustedPopulationPercent(VictoryTypes eVictory) const;
	int getAdjustedLandPercent(VictoryTypes eVictory) const;

	bool isTeamVote(VoteTypes eVote) const;
	bool isChooseElection(VoteTypes eVote) const;
	bool isTeamVoteEligible(TeamTypes eTeam, VoteSourceTypes eVoteSource) const;
	int countVote(const VoteTriggeredData& kData, PlayerVoteTypes eChoice) const;
	int countPossibleVote(VoteTypes eVote, VoteSourceTypes eVoteSource) const;
	TeamTypes findHighestVoteTeam(const VoteTriggeredData& kData) const;
	int getVoteRequired(VoteTypes eVote, VoteSourceTypes eVoteSource) const;
	TeamTypes getSecretaryGeneral(VoteSourceTypes eVoteSource) const;
	bool canHaveSecretaryGeneral(VoteSourceTypes eVoteSource) const;
	void clearSecretaryGeneral(VoteSourceTypes eVoteSource);
	void updateSecretaryGeneral();

	int countCivPlayersAlive() const;
	int countCivPlayersEverAlive() const;
	int countCivTeamsAlive() const;
	int countCivTeamsEverAlive() const;
	int countHumanPlayersAlive() const;

	int countTotalCivPower() const;
	int countTotalNukeUnits() const;
	int countKnownTechNumTeams(TechTypes eTech) const;
	int getNumFreeBonuses(BuildingTypes eBuilding) const;

	int countReligionLevels(ReligionTypes eReligion) const;
	int calculateReligionPercent(ReligionTypes eReligion) const;
	int countCorporationLevels(CorporationTypes eCorporation) const;
	void replaceCorporation(CorporationTypes eCorporation1, CorporationTypes eCorporation2);

	int goldenAgeLength() const;
	int victoryDelay(VictoryTypes eVictory) const;
	int getImprovementUpgradeTime(ImprovementTypes eImprovement) const;

	bool canTrainNukes() const;

	EraTypes getHighestEra() const;
	EraTypes getCurrentEra() const;

	DllExport TeamTypes getActiveTeam() const;
	CivilizationTypes getActiveCivilizationType() const;

	DllExport bool isNetworkMultiPlayer() const;
	DllExport bool isGameMultiPlayer() const;
	DllExport bool isTeamGame() const;

	bool isModem() const;
	void setModem(bool bModem);

	DllExport void reviveActivePlayer();
	DllExport int getNumHumanPlayers();

	DllExport int getGameTurn();
	void setGameTurn(int iNewValue);
	void incrementGameTurn();
	int getTurnYear(int iGameTurn);
	int getGameTurnYear();
	CvDate getCurrentDate();

	// A unique ID for this game that is persistent across sessions
	const CvString getGameId() const { return m_gameId; }

	int getElapsedGameTurns() const;
	void incrementElapsedGameTurns();

	int getMaxTurns() const;
	void setMaxTurns(int iNewValue);
	void changeMaxTurns(int iChange);

	int getMaxCityElimination() const;
	void setMaxCityElimination(int iNewValue);

	int getNumAdvancedStartPoints() const;
	void setNumAdvancedStartPoints(int iNewValue);

	int getStartTurn() const;
	void setStartTurn(int iNewValue);

	int getStartYear() const;
	void setStartYear(int iNewValue);

	int getEstimateEndTurn() const;
	void setEstimateEndTurn(int iNewValue);

	DllExport int getTurnSlice() const;
	int getMinutesPlayed() const;
	void changeTurnSlice(int iChange);

	int getCutoffSlice() const;
	void setCutoffSlice(int iNewValue);
	void changeCutoffSlice(int iChange);

	DllExport int getTurnSlicesRemaining();
	void resetTurnTimer();
	void incrementTurnTimer(int iNumTurnSlices);
	int getMaxTurnLen() const;

	int getTargetScore() const;
	void setTargetScore(int iNewValue);

	int getNumGameTurnActive() const;
	DllExport int countNumHumanGameTurnActive() const;
	void changeNumGameTurnActive(int iChange);

	int getNumCities() const;
	int getNumCivCities() const;
	void changeNumCities(int iChange);

	int getStatusPromotion(int i) const;
	int getNumStatusPromotions() const;
	void setStatusPromotions();

	int getTotalPopulation() const;
	void changeTotalPopulation(int iChange);

	int getTradeRoutes() const;
	void changeTradeRoutes(int iChange);

	int getFreeTradeCount() const;
	bool isFreeTrade() const;
	void changeFreeTradeCount(int iChange);

	int getNoNukesCount() const;
	bool isNoNukes() const;
	void changeNoNukesCount(int iChange);

	int getSecretaryGeneralTimer(VoteSourceTypes eVoteSource) const;
	void setSecretaryGeneralTimer(VoteSourceTypes eVoteSource, int iNewValue);
	void changeSecretaryGeneralTimer(VoteSourceTypes eVoteSource, int iChange);

	int getVoteTimer(VoteSourceTypes eVoteSource) const;
	void setVoteTimer(VoteSourceTypes eVoteSource, int iNewValue);
	void changeVoteTimer(VoteSourceTypes eVoteSource, int iChange);

	int getNukesExploded() const;
	void changeNukesExploded(int iChange);

	int getMaxPopulation() const;
	int getMaxLand() const;
	int getMaxTech() const;
	int getMaxWonders() const;
	int getInitPopulation() const;
	int getInitLand() const;
	int getInitTech() const;
	int getInitWonders() const;
	DllExport void initScoreCalculation();
	// < M.A.D. Nukes Start >
	CvPlot* getLastNukeStrikePlot() const;
	void setLastNukeStrikePlot(CvPlot* pPlot);
	// < M.A.D. Nukes End   >

	int getAIAutoPlay(PlayerTypes iPlayer) const;
		void setAIAutoPlay(PlayerTypes iPlayer, int iNewValue, bool bForced = false);
	DllExport void setAIAutoPlayExternal(int iNewValue);
	void changeAIAutoPlay(PlayerTypes iPlayer, int iChange);

	bool isForcedAIAutoPlay(PlayerTypes iPlayer) const;
	int getForcedAIAutoPlay(PlayerTypes iPlayer) const;
	void setForcedAIAutoPlay(PlayerTypes iPlayer, int iNewValue, bool bForced = false);
	void changeForcedAIAutoPlay(PlayerTypes iPlayer, int iNewValue);

	bool getStarshipLaunched(int ID) const;
	bool getDiplomaticVictoryAchieved(int ID) const;

	int getCurrentVoteID() const;
	void setCurrentVoteID(int iNewValue);

	int getCutLosersCounter() const;
	void changeCutLosersCounter(int iChange);
	int getHighToLowCounter() const;
	void changeHighToLowCounter(int iChange);
	int getIncreasingDifficultyCounter() const;
	void changeIncreasingDifficultyCounter(int iChange);

	void averageHandicaps();

	int getMercyRuleCounter() const;
	void changeMercyRuleCounter(int iChange);
	void setMercyRuleCounter(int iNewVal);

	int countPeaks(const CvPlot* pPlot, bool bCountHill = false);
	void markBayPlots(const CvPlot* pPlot);
	int countForest(const CvPlot* pPlot, int iForest);
	void addLandmarkSigns();
	int countDesert(const CvPlot* pPlot);
	void markLakePlots(const CvPlot* pPlot);
	int countJungle(const CvPlot* pPlot, int iJungle);
	char getRandomVowel();
	char getRandomConsonant();
	bool isValidName(CvWString szName) const;
	CvWString generateRandomWord(int iMaxLength);
	CvWString getRandomName(int iMaxLength);

	void setPreviousRequest(PlayerTypes ePlayer, bool bNewValue);
	bool isPreviousRequest(PlayerTypes ePlayer) const;

	int getModderGameOption(ModderGameOptionTypes eIndex) const;
	bool isModderGameOption(ModderGameOptionTypes eIndex) const;
	void setModderGameOption(ModderGameOptionTypes eIndex, int iNewValue);

	void findMountainRanges();
	void findLonePeaks();
	void findBays();
	void findForests();
	void findJungles();
	void findDeserts();
	void findLakes();
	void pruneLandmarks();
	void removeAdjacentLandmarks(const CvPlot* pCenterPlot, const CvPlot* pExceptionPlot = NULL, int iRadius = 1);
	void clearLandmarks(bool bClear = true);
	void updateInitialSigns();

	void doFoundCorporation(CorporationTypes eCorporation, bool bForce);
	int getAverageCorporationInfluence(const CvCity* pCity, const CorporationTypes eCorporation) const;

	bool canEverResearch(TechTypes eTech) const;
	bool canEverConstruct(BuildingTypes eBuilding) const;
	bool canEverTrain(UnitTypes eUnit) const;
	bool canEverSpread(CorporationTypes eCorporation) const;

	int getFlexibleDifficultyTimer(PlayerTypes eIndex) const;
	void setFlexibleDifficultyTimer(PlayerTypes eIndex, int iNewValue);
	void calculateRiverBuildings();
	int getRiverBuildings() const;
	void calculateCoastalBuildings();
	int getCoastalBuildings() const;
	void calculateNumWonders();
	int getNumWonders() const;
	bool canExitToMainMenu() const;

	void loadPirateShip(CvUnit* pUnit);

	bool isAnyoneHasUnitZoneOfControl() const;
	void toggleAnyoneHasUnitZoneOfControl();
	//TB OOSSPECIAL
	void logOOSSpecial(int iLocID, int iVar, int iVar2 = 0, int iVar3 = 0);
	int getTopCityCount() const;
	int getTopPopCount() const;
	int getImprovementCount(ImprovementTypes eIndex) const;
	void changeImprovementCount(ImprovementTypes eIndex, int iChange);
protected:
	void doFlexibleDifficulty();
	void doHightoLow();
	void doIncreasingDifficulty();
	void doFinalFive();
	void doFoundCorporations();
	bool* m_abPreviousRequest;
	int* m_aiModderGameOption;
	int* m_aiFlexibleDifficultyTimer;
	int m_iRiverBuildings;
	int m_iCoastalBuildings;
	int m_iNumWonders;
	bool m_bDiploVictoryEnabled;
	bool m_bAnyoneHasUnitZoneOfControl;
public:

	unsigned int getInitialTime() const;
	DllExport void setInitialTime(unsigned int uiNewValue);

	bool isScoreDirty() const;
	void setScoreDirty(bool bNewValue);

	TeamTypes getCircumnavigatedTeam() const;
	void setCircumnavigatedTeam(TeamTypes eTeam);
	bool circumnavigationAvailable() const;

	bool isDiploVote(VoteSourceTypes eVoteSource) const;
	int getDiploVoteCount(VoteSourceTypes eVoteSource) const;
	void changeDiploVote(VoteSourceTypes eVoteSource, int iChange);
	bool canDoResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData) const;
	bool isValidVoteSelection(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData) const;

	DllExport bool isDebugMode() const;
	DllExport void toggleDebugMode();
	DllExport void updateDebugModeCache();

	int getPitbossTurnTime() const;
	void setPitbossTurnTime(int iHours);

	DllExport bool isHotSeat() const;
	DllExport bool isPbem() const;
	DllExport bool isPitboss() const;
	bool isSimultaneousTeamTurns() const;

	DllExport bool isFinalInitialized() const;
	DllExport void setFinalInitialized(bool bNewValue);

	bool getPbemTurnSent() const;
	DllExport void setPbemTurnSent(bool bNewValue);

	DllExport bool getHotPbemBetweenTurns() const;
	void setHotPbemBetweenTurns(bool bNewValue);

	bool isPlayerOptionsSent() const;
	void sendPlayerOptions(bool bForce = false);

	DllExport PlayerTypes getActivePlayer() const;
	DllExport void setActivePlayer(PlayerTypes eNewValue, bool bForceHotSeat = false);
	DllExport void updateUnitEnemyGlow();

	HandicapTypes getHandicapType() const;
	void setHandicapType(HandicapTypes eHandicap);

	DllExport PlayerTypes getPausePlayer() const;
	DllExport bool isPaused() const;
	DllExport void setPausePlayer(PlayerTypes eNewValue);

	UnitTypes getBestLandUnit() const;
	int getBestLandUnitCombat() const;
	void setBestLandUnit(UnitTypes eNewValue);

	TeamTypes getWinner() const;
	VictoryTypes getVictory() const;
	void setWinner(TeamTypes eNewWinner, VictoryTypes eNewVictory);

	DllExport GameStateTypes getGameState() const;
	DllExport void setGameState(GameStateTypes eNewValue);

	EraTypes getStartEra() const;

	CalendarTypes getCalendar() const;

	GameSpeedTypes getGameSpeedType() const;

	PlayerTypes getRankPlayer(int iRank) const;
	void setRankPlayer(int iRank, PlayerTypes ePlayer);

	int getPlayerRank(PlayerTypes ePlayer) const;
	void setPlayerRank(PlayerTypes ePlayer, int iRank);

	DllExport int getPlayerScore(PlayerTypes ePlayer) const;
	void setPlayerScore(PlayerTypes ePlayer, int iScore);

	TeamTypes getRankTeam(int iRank) const;
	void setRankTeam(int iRank, TeamTypes eTeam);

	int getTeamRank(TeamTypes eTeam) const;
	void setTeamRank(TeamTypes eTeam, int iRank);

	DllExport int getTeamScore(TeamTypes eTeam) const;
	void setTeamScore(TeamTypes eTeam, int iScore);

	DllExport bool isOption(GameOptionTypes eIndex) const;
	void setOption(GameOptionTypes eIndex, bool bEnabled);

	DllExport bool isMPOption(MultiplayerOptionTypes eIndex) const;
	void setMPOption(MultiplayerOptionTypes eIndex, bool bEnabled);

	bool isForcedControl(ForceControlTypes eIndex) const;
	void setForceControl(ForceControlTypes eIndex, bool bEnabled);

	int getUnitCreatedCount(UnitTypes eIndex) const;
	void incrementUnitCreatedCount(UnitTypes eIndex);
	bool isUnitMaxedOut(UnitTypes eIndex, int iExtra = 0) const;

	int getBuildingCreatedCount(BuildingTypes eIndex) const;
	bool isBuildingMaxedOut(BuildingTypes eIndex, int iExtra = 0) const;
	void changeNumBuildings(const BuildingTypes eIndex, const short iChange);

	int getProjectCreatedCount(ProjectTypes eIndex) const;
	bool isProjectMaxedOut(ProjectTypes eIndex, int iExtra = 0) const;
	void incrementProjectCreatedCount(ProjectTypes eIndex, int iExtra = 1);

	int getForceCivicCount(CivicTypes eIndex) const;
	bool isForceCivic(CivicTypes eIndex) const;
	bool isForceCivicOption(CivicOptionTypes eCivicOption) const;
	void changeForceCivicCount(CivicTypes eIndex, int iChange);

	PlayerVoteTypes getVoteOutcome(VoteTypes eIndex) const;
	bool isVotePassed(VoteTypes eIndex) const;
	void setVoteOutcome(const VoteTriggeredData& kData, PlayerVoteTypes eNewValue);

	bool isVictoryValid(VictoryTypes eIndex) const;
	void setVictoryValid(VictoryTypes eIndex, bool bValid);

	bool isSpecialUnitValid(SpecialUnitTypes eIndex) const;
	void makeSpecialUnitValid(SpecialUnitTypes eIndex);

	bool isSpecialBuildingValid(SpecialBuildingTypes eIndex) const;
	void makeSpecialBuildingValid(SpecialBuildingTypes eIndex, bool bAnnounce = false);

	bool isInAdvancedStart() const;

	void setVoteChosen(int iSelection, int iVoteId);

	int getReligionGameTurnFounded(ReligionTypes eIndex) const;
	bool isReligionFounded(ReligionTypes eIndex) const;
	void makeReligionFounded(ReligionTypes eIndex, PlayerTypes ePlayer);

	int getTechGameTurnDiscovered(TechTypes eTech) const;
	bool isTechDiscovered(TechTypes eTech) const;
	void makeTechDiscovered(TechTypes eTech);

	bool isReligionSlotTaken(ReligionTypes eReligion) const;
	void setReligionSlotTaken(ReligionTypes eReligion, bool bTaken);

	bool isGameStart();

	int countNumReligionsFounded() const;
	int countNumReligionTechsDiscovered() const;

	bool isTechCanFoundReligion(TechTypes eIndex) const;
	void setTechCanFoundReligion(TechTypes eIndex, bool bUsed);

	CvCity* getHolyCity(ReligionTypes eIndex) const;
	void setHolyCity(ReligionTypes eIndex, const CvCity* pNewValue, bool bAnnounce);

	int getCorporationGameTurnFounded(CorporationTypes eIndex) const;
	bool isCorporationFounded(CorporationTypes eIndex) const;
	void makeCorporationFounded(CorporationTypes eIndex, PlayerTypes ePlayer);

	CvCity* getHeadquarters(CorporationTypes eIndex) const;
	void setHeadquarters(CorporationTypes eIndex, CvCity* pNewValue, bool bAnnounce);

	PlayerVoteTypes getPlayerVote(PlayerTypes eOwnerIndex, int iVoteId) const;
	void setPlayerVote(PlayerTypes eOwnerIndex, int iVoteId, PlayerVoteTypes eNewValue);
	void castVote(PlayerTypes eOwnerIndex, int iVoteId, PlayerVoteTypes ePlayerVote);

	DllExport const CvWString & getName();
	void setName(const char* szName);

	// Script data needs to be a narrow string for pickling in Python
	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);

	bool isDestroyedCityName(CvWString& szName) const;
	void addDestroyedCityName(const CvWString& szName);

	bool isGreatPersonBorn(CvWString& szName) const;
	void addGreatPersonBornName(const CvWString& szName);

	DllExport int getIndexAfterLastDeal();
	int getNumDeals() const;
	DllExport CvDeal* getDeal(int iID);
	CvDeal* addDeal();
	void deleteDeal(int iID);
	FFreeListTrashArray<CvDeal>::Range_t deals() const { return m_deals.range(); }

	VoteSelectionData* getVoteSelection(int iID) const;
	VoteSelectionData* addVoteSelection(VoteSourceTypes eVoteSource);
	void deleteVoteSelection(int iID);

	VoteTriggeredData* getVoteTriggered(int iID) const;
	VoteTriggeredData* addVoteTriggered(const VoteSelectionData& kData, int iChoice);
	VoteTriggeredData* addVoteTriggered(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kOptionData);
	void deleteVoteTriggered(int iID);

	CvRandom& getMapRand();
	int getMapRandNum(int iNum, const char* pszLog);

	CvRandom& getSorenRand();
	int getSorenRandNum(int iNum, const char* pszLog);
	struct SorenRand
	{
		SorenRand(const char* logMsg) : logMsg(logMsg) {}
		int operator()(const int maxVal) const;
		CvString logMsg;
	};

	void logRandomResult(const wchar_t* szStreamName, const char* pszLog, int iMax, int iNum);

	DllExport int calculateSyncChecksum();
	DllExport int calculateOptionsChecksum();

	void addReplayMessage(ReplayMessageTypes eType = NO_REPLAY_MESSAGE, PlayerTypes ePlayer = NO_PLAYER, CvWString pszText = CvWString(),
		int iPlotX = -1, int iPlotY = -1, ColorTypes eColor = NO_COLOR);
	void clearReplayMessageMap();
	int getReplayMessageTurn(uint i) const;
	ReplayMessageTypes getReplayMessageType(uint i) const;
	int getReplayMessagePlotX(uint i) const;
	int getReplayMessagePlotY(uint i) const;
	PlayerTypes getReplayMessagePlayer(uint i) const;
	LPCWSTR getReplayMessageText(uint i) const;
	uint getNumReplayMessages() const;
	ColorTypes getReplayMessageColor(uint i) const;

	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);
	virtual void writeReplay(FDataStreamBase& stream, PlayerTypes ePlayer);

	virtual void AI_init() = 0;
	virtual void AI_reset() = 0;
	virtual void AI_makeAssignWorkDirty() = 0;
	virtual void AI_updateAssignWork() = 0;
	virtual int AI_combatValue(const UnitTypes eUnit) const = 0;

	CvReplayInfo* getReplayInfo() const;
	DllExport void setReplayInfo(CvReplayInfo* pReplay);
	void saveReplay(PlayerTypes ePlayer);

	bool hasSkippedSaveChecksum() const;

	void logNetMsgData(char* format, ...);

	void addPlayer(PlayerTypes eNewPlayer, LeaderHeadTypes eLeader, CivilizationTypes eCiv, bool bSetAlive = true);
	void changeHumanPlayer( PlayerTypes eOldHuman, PlayerTypes eNewHuman );

	bool testVictory(VictoryTypes eVictory, TeamTypes eTeam, bool* pbEndScore = NULL); //removed const

	bool isCompetingCorporation(CorporationTypes eCorporation1, CorporationTypes eCorporation2) const;

	int getShrineBuildingCount(ReligionTypes eReligion = NO_RELIGION);
	BuildingTypes getShrineBuilding(int eIndex, ReligionTypes eReligion = NO_RELIGION);
	void changeShrineBuilding(BuildingTypes eBuilding, ReligionTypes eReligion, bool bRemove = false);

	bool culturalVictoryValid() const;
	int culturalVictoryNumCultureCities() const;
	CultureLevelTypes culturalVictoryCultureLevel() const;
	int getCultureThreshold(CultureLevelTypes eLevel) const;

	int getPlotExtraYield(int iX, int iY, YieldTypes eYield) const;
	void setPlotExtraYield(int iX, int iY, YieldTypes eYield, int iCost);
	//void removePlotExtraYield(int iX, int iY); // Toffer - Unused, but might be needed for recalc...

	ReligionTypes getVoteSourceReligion(VoteSourceTypes eVoteSource) const;
	void setVoteSourceReligion(VoteSourceTypes eVoteSource, ReligionTypes eReligion, bool bAnnounce = false);

	bool isEventActive(EventTriggerTypes eTrigger) const;
	DllExport void initEvents();

	bool isCivEverActive(CivilizationTypes eCivilization) const;
	bool isLeaderEverActive(LeaderHeadTypes eLeader) const;

	void processBuilding(BuildingTypes eBuilding, int iChange);

	bool foundBarbarianCity();

	bool pythonIsBonusIgnoreLatitudes() const;

	inline bool isRecalculatingModifiers() const { return m_bRecalculatingModifiers; }

	DllExport void getGlobeLayers(std::vector<CvGlobeLayerData>& aLayers) const;
	DllExport void startFlyoutMenu(const CvPlot* pPlot, std::vector<CvFlyoutMenuData>& aFlyoutItems) const;
	DllExport void applyFlyoutMenu(const CvFlyoutMenuData& kItem);
	DllExport CvPlot* getNewHighlightPlot() const;
	DllExport ColorTypes getPlotHighlightColor(CvPlot* pPlot) const;
	DllExport void cheatSpaceship() const;
	DllExport VictoryTypes getSpaceVictory() const;
	DllExport void nextActivePlayer(bool bForward);

	DllExport DomainTypes getUnitDomain(UnitTypes eUnit) const;
	DllExport const CvArtInfoBuilding* getBuildingArtInfo(BuildingTypes eBuilding) const;
	DllExport bool isWaterBuilding(BuildingTypes eBuilding) const;
	DllExport CivilopediaWidgetShowTypes getWidgetShow(BonusTypes eBonus) const;
	DllExport CivilopediaWidgetShowTypes getWidgetShow(ImprovementTypes eImprovement) const;

	DllExport void loadBuildQueue(const CvString& strItem) const;

	DllExport int getNextSoundtrack(EraTypes eLastEra, int iLastSoundtrack) const;
	DllExport int getSoundtrackSpace() const;
	DllExport bool isSoundtrackOverride(CvString& strSoundtrack) const;

	DllExport void initSelection() const;
	DllExport bool canDoPing(CvPlot* pPlot, PlayerTypes ePlayer) const;
	DllExport bool shouldDisplayReturn() const;
	DllExport bool shouldDisplayEndTurn() const;
	DllExport bool shouldDisplayWaitingOthers() const;
	DllExport bool shouldDisplayWaitingYou() const;
	DllExport bool shouldDisplayEndTurnButton() const;
	DllExport bool shouldDisplayFlag() const;
	DllExport bool shouldDisplayUnitModel() const;
	DllExport bool shouldShowResearchButtons() const;
	DllExport bool shouldCenterMinimap() const;
	DllExport EndTurnButtonStates getEndTurnState() const;

	DllExport void handleCityScreenPlotPicked(CvCity* pCity, CvPlot* pPlot, bool bAlt, bool bShift, bool bCtrl) const;
	DllExport void handleCityScreenPlotDoublePicked(CvCity* pCity, CvPlot* pPlot, bool bAlt, bool bShift, bool bCtrl) const;
	DllExport void handleCityScreenPlotRightPicked(CvCity* pCity, CvPlot* pPlot, bool bAlt, bool bShift, bool bCtrl) const;
	DllExport void handleCityPlotRightPicked(CvCity* pCity, CvPlot* pPlot, bool bAlt, bool bShift, bool bCtrl) const;
	DllExport void handleMiddleMouse(bool bCtrl, bool bAlt, bool bShift);

	DllExport void handleDiplomacySetAIComment(DiploCommentTypes eComment) const;

	void recalculateModifiers();

protected:
	MapTypes m_eCurrentMap;

	CvString m_gameId;
	int m_iElapsedGameTurns;
	int m_iStartTurn;
	int m_iStartYear;
	int m_iEstimateEndTurn;
	CvDate m_currentDate;
	int m_iDateTurn;

	int m_iTurnSlice;
	int m_iCutoffSlice;
	int m_iNumGameTurnActive;
	int m_iNumCities;
	int m_iTotalPopulation;
	int m_iTradeRoutes;
	int m_iFreeTradeCount;
	int m_iNoNukesCount;
	int m_iNukesExploded;
	int m_iMaxPopulation;
	int m_iMaxLand;
	int m_iMaxTech;
	int m_iMaxWonders;
	int m_iInitPopulation;
	int m_iInitLand;
	int m_iInitTech;
	int m_iInitWonders;

	int m_iAIAutoPlay[MAX_PLAYERS];
	int m_iForcedAIAutoPlay[MAX_PLAYERS];

	int m_iCurrentVoteID;
	int m_iCutLosersCounter;
	int m_iHighToLowCounter;
	int m_iIncreasingDifficultyCounter;
	int m_iMercyRuleCounter;

	bool starshipLaunched[MAX_TEAMS]; 				//Ordered by team ID (both)
	bool diplomaticVictoryAchieved[MAX_TEAMS];

	unsigned int m_uiInitialTime;
	// < M.A.D. Nukes Start >
	int m_iLastNukeStrikeX;
	int m_iLastNukeStrikeY;
	// < M.A.D. Nukes End   >
	bool m_bScoreDirty;
	bool m_bDebugMode;
	bool m_bDebugModeCache;
	bool m_bFinalInitialized;
	bool m_bPbemTurnSent;
	bool m_bHotPbemBetweenTurns;
	bool m_bPlayerOptionsSent;
	//TB Nukefix (reversal) Next line should be commented out
	//bool m_bNukesValid;
	TeamTypes m_circumnavigatingTeam;

	std::vector<int> m_aiStatusPromotions;

	HandicapTypes m_eHandicap;
	PlayerTypes m_ePausePlayer;
	mutable UnitTypes m_eBestLandUnit;
	TeamTypes m_eWinner;
	VictoryTypes m_eVictory;
	GameStateTypes m_eGameState;
	PlayerTypes m_eEventPlayer;

	CvString m_szScriptData;

	int* m_aiRankPlayer;        // Ordered by rank...
	int* m_aiPlayerRank;        // Ordered by player ID...
	int* m_aiPlayerScore;       // Ordered by player ID...
	int* m_aiRankTeam;						// Ordered by rank...
	int* m_aiTeamRank;						// Ordered by team ID...
	int* m_aiTeamScore;						// Ordered by team ID...

	int* m_paiImprovementCount;
	int* m_paiUnitCreatedCount;
	int* m_paiBuildingCreatedCount;
	int* m_paiProjectCreatedCount;
	int* m_paiForceCivicCount;
	PlayerVoteTypes* m_paiVoteOutcome;
	int* m_paiReligionGameTurnFounded;
	int* m_paiTechGameTurnDiscovered;
	int* m_paiCorporationGameTurnFounded;
	int* m_aiSecretaryGeneralTimer;
	int* m_aiVoteTimer;
	int* m_aiDiploVote;

	bool* m_pabSpecialUnitValid;
	bool* m_pabSpecialBuildingValid;
	bool* m_abReligionSlotTaken;

	bool m_bGameStart;
	bool* m_abTechCanFoundReligion;

	IDInfo* m_paHolyCity;
	IDInfo* m_paHeadquarters;

	int** m_apaiPlayerVote;

	std::vector<CvWString> m_aszDestroyedCities;
	std::vector<CvWString> m_aszGreatPeopleBorn;

	FFreeListTrashArray<CvDeal> m_deals;
	FFreeListTrashArray<VoteSelectionData> m_voteSelections;
	FFreeListTrashArray<VoteTriggeredData> m_votesTriggered;

	CvRandom m_mapRand;
	CvRandom m_sorenRand;

	ReplayMessageList m_listReplayMessages;
	CvReplayInfo* m_pReplayInfo;

	DWORD	m_lastGraphicUpdateRequestTickCount;

	int m_iNumSessions;

	std::vector<PlotExtraYield> m_aPlotExtraYields;
	stdext::hash_map<VoteSourceTypes, ReligionTypes> m_mapVoteSourceReligions;
	std::vector<EventTriggerTypes> m_aeInactiveTriggers;

	// CACHE: cache frequently used values
	int		m_iShrineBuildingCount;
	int*	m_aiShrineBuilding;
	int*	m_aiShrineReligion;

	int		m_iNumCultureVictoryCities;
	int		m_eCultureVictoryCultureLevel;

	bool	m_plotGroupHashesInitialized;
	bool	m_bRecalculatingModifiers;

	void doTurn();
	void doDeals();
	void doSpawns(PlayerTypes ePlayer);
#ifdef GLOBAL_WARMING
	void doGlobalWarming();
#endif

	void checkGameStart();

	void doHeadquarters();
	void doDiploVote();
	void doVoteResults();
	void doVoteSelection();

	void createBarbarianCities(bool bNeanderthal);
	void createBarbarianUnits();
	//void createAnimals();

	void verifyCivics();

	void updateMoves();
	void updateTimers();
	void updateTurnTimer();

	void testAlive();
	void testVictory();

	void processVote(const VoteTriggeredData& kData, int iChange);

	int getTeamClosenessScore(int** aaiDistances, int* aiStartingLocs);
	void normalizeStartingPlotLocations();
	void normalizeAddRiver();
	void normalizeRemovePeaks();
	void normalizeAddLakes();
	void normalizeRemoveBadFeatures();
	void normalizeRemoveBadTerrain();
	void normalizeAddFoodBonuses();
	void normalizeAddGoodTerrain();
	void normalizeAddExtras();

	void showEndGameSequence();

	CvPlot* normalizeFindLakePlot(PlayerTypes ePlayer);

	void doUpdateCacheOnTurn();

	// AIAndy: Properties
	CvProperties m_Properties;
	CvPropertySolver m_PropertySolver;

	//	Super forts adaptation to C2C - record whether this game has had its choke
	//	points evaluated
	int m_iChokePointCalculationVersion;

public:
	//	Super forts adaptation to C2C - calc choke points if not already done
	void ensureChokePointsEvaluated();
	int getBaseAirUnitIncrementsbyCargoVolume() const;
	int getBaseMissileUnitIncrementsbyCargoVolume() const;

	CvProperties* getProperties();
	const CvProperties* getPropertiesConst() const;

	bool isValidByGameOption(const CvUnitCombatInfo& info) const;

	void enforceOptionCompatibility(GameOptionTypes eOption);
};

#define CURRENT_MAP GC.getGame().getCurrentMap()

#endif
