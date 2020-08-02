#pragma once

// game.h

#ifndef CIV4_GAME_H
#define CIV4_GAME_H

#include "CvDeal.h"
#include "CvRandom.h"
#include "CvGameObject.h"
#include "CvPropertySolver.h"
#include "CvDate.h"

class CvPlot;
class CvCity;
class CvReplayMessage;
class CvReplayInfo;

//	Max number of barbarian units in existence for a spawn of a new one to be allowed
//	This allows a 'space' for 'real' barbarians to be built before we use up the entire
//	simultaneous unit space of 8191 for any one player
#define	MAX_BARB_UNITS_FOR_SPAWNING	7000

typedef std::vector<const CvReplayMessage*> ReplayMessageList;

class CvGame
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

	bool canRegenerateMap() const; // Exposed to Python
	DllExport void regenerateMap(); // Exposed to Python

	DllExport void initDiplomacy();
	void initFreeState();
	DllExport void initFreeUnits();

	void assignStartingPlots();
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
	void cycleSelectionGroupsInternal(bool bClear, bool bForward, bool bWorkers, bool bSetCamera, bool bAllowViewportSwitch) const; // Exposed to Python

	void processGreatWall(bool bIn, bool bForce = false, bool bSeeded = true) const;
	void noteGraphicRebuildNeeded();
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

	DllExport CvUnit* getPlotUnit(const CvPlot* pPlot, int iIndex) const;
	DllExport void getPlotUnits(const CvPlot *pPlot, std::vector<CvUnit*>& plotUnits) const;

	DllExport void cycleCities(bool bForward = true, bool bAdd = false) const; // Exposed to Python
	void cycleSelectionGroups(bool bClear, bool bForward = true, bool bWorkers = false) const; // Exposed to Python
	DllExport bool cyclePlotUnits(CvPlot* pPlot, bool bForward = true, bool bAuto = false, int iCount = -1) const; // Exposed to Python
	DllExport bool selectCity(CvCity* pSelectCity, bool bCtrl, bool bAlt, bool bShift) const;

	DllExport void selectionListMove(CvPlot* pPlot, bool bAlt, bool bShift, bool bCtrl) const; // Exposed to Python
	DllExport void selectionListGameNetMessage(int eMessage, int iData2 = -1, int iData3 = -1, int iData4 = -1, int iFlags = 0, bool bAlt = false, bool bShift = false) const; // Exposed to Python
	void selectionListGameNetMessageInternal(int eMessage, int iData2, int iData3, int iData4, int iFlags, bool bAlt, bool bShift, bool bInViewportCoordinates) const;
	DllExport void selectedCitiesGameNetMessage(int eMessage, int iData2 = -1, int iData3 = -1, int iData4 = -1, bool bOption = false, bool bAlt = false, bool bShift = false, bool bCtrl = false) const; // Exposed to Python
	void cityPushOrder(const CvCity* pCity, OrderTypes eOrder, int iData, bool bAlt = false, bool bShift = false, bool bCtrl = false) const; // Exposed to Python

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

	int getSymbolID(int iSymbol); // Exposed to Python

	int getProductionPerPopulation(HurryTypes eHurry) const; // Exposed to Python

	int getAdjustedPopulationPercent(VictoryTypes eVictory) const; // Exposed to Python
	int getAdjustedLandPercent(VictoryTypes eVictory) const; // Exposed to Python

	bool isTeamVote(VoteTypes eVote) const; // Exposed to Python
	bool isChooseElection(VoteTypes eVote) const; // Exposed to Python
	bool isTeamVoteEligible(TeamTypes eTeam, VoteSourceTypes eVoteSource) const; // Exposed to Python
	int countVote(const VoteTriggeredData& kData, PlayerVoteTypes eChoice) const;
	int countPossibleVote(VoteTypes eVote, VoteSourceTypes eVoteSource) const; // Exposed to Python
	TeamTypes findHighestVoteTeam(const VoteTriggeredData& kData) const;
	int getVoteRequired(VoteTypes eVote, VoteSourceTypes eVoteSource) const; // Exposed to Python
	TeamTypes getSecretaryGeneral(VoteSourceTypes eVoteSource) const; // Exposed to Python
	bool canHaveSecretaryGeneral(VoteSourceTypes eVoteSource) const; // Exposed to Python
	void clearSecretaryGeneral(VoteSourceTypes eVoteSource);
	void updateSecretaryGeneral();

	int countCivPlayersAlive() const; // Exposed to Python
	int countCivPlayersEverAlive() const; // Exposed to Python
	int countCivTeamsAlive() const; // Exposed to Python
	int countCivTeamsEverAlive() const; // Exposed to Python
	int countHumanPlayersAlive() const; // Exposed to Python

	int countTotalCivPower() const; // Exposed to Python
	int countTotalNukeUnits()const; // Exposed to Python
	int countKnownTechNumTeams(TechTypes eTech) const; // Exposed to Python
	int getNumFreeBonuses(BuildingTypes eBuilding) const; // Exposed to Python

	int countReligionLevels(ReligionTypes eReligion) const; // Exposed to Python 
	int calculateReligionPercent(ReligionTypes eReligion) const; // Exposed to Python
	int countCorporationLevels(CorporationTypes eCorporation) const; // Exposed to Python 
	void replaceCorporation(CorporationTypes eCorporation1, CorporationTypes eCorporation2);

	int goldenAgeLength() const; // Exposed to Python
	int victoryDelay(VictoryTypes eVictory) const; // Exposed to Python
	int getImprovementUpgradeTime(ImprovementTypes eImprovement) const; // Exposed to Python

	bool canTrainNukes() const; // Exposed to Python

	EraTypes getHighestEra() const; // Exposed to Python
	EraTypes getCurrentEra() const; // Exposed to Python

	DllExport TeamTypes getActiveTeam() const; // Exposed to Python
	CivilizationTypes getActiveCivilizationType() const; // Exposed to Python

	DllExport bool isNetworkMultiPlayer() const; // Exposed to Python
	DllExport bool isGameMultiPlayer() const; // Exposed to Python
	DllExport bool isTeamGame() const; // Exposed to Python

	bool isModem() const;
	void setModem(bool bModem);

	DllExport void reviveActivePlayer(); // Exposed to Python
	void reviveActivePlayer(PlayerTypes iPlayer); // Exposed to Python

	void drawBattleEffects();

	DllExport int getNumHumanPlayers(); // Exposed to Python

	DllExport int getGameTurn(); // Exposed to Python
	void setGameTurn(int iNewValue); // Exposed to Python
	void incrementGameTurn();
	int getTurnYear(int iGameTurn); // Exposed to Python
	int getGameTurnYear(); // Exposed to Python
	CvDate getCurrentDate();

	// A unique ID for this game that is persistent across sessions
	const CvString getGameId() const { return m_gameId; }

	int getElapsedGameTurns() const; // Exposed to Python
	void incrementElapsedGameTurns();

	int getMaxTurns() const; // Exposed to Python
	void setMaxTurns(int iNewValue); // Exposed to Python
	void changeMaxTurns(int iChange); // Exposed to Python

	int getMaxCityElimination() const; // Exposed to Python
	void setMaxCityElimination(int iNewValue); // Exposed to Python

	int getNumAdvancedStartPoints() const; // Exposed to Python
	void setNumAdvancedStartPoints(int iNewValue); // Exposed to Python

	int getStartTurn() const; // Exposed to Python
	void setStartTurn(int iNewValue);

	int getStartYear() const; // Exposed to Python
	void setStartYear(int iNewValue); // Exposed to Python

	int getEstimateEndTurn() const; // Exposed to Python
	void setEstimateEndTurn(int iNewValue); // Exposed to Python

	DllExport int getTurnSlice() const; // Exposed to Python
	int getMinutesPlayed() const; // Exposed to Python
	void setTurnSlice(int iNewValue);
	void changeTurnSlice(int iChange);

	int getCutoffSlice() const;
	void setCutoffSlice(int iNewValue);
	void changeCutoffSlice(int iChange);

	DllExport int getTurnSlicesRemaining();
	void resetTurnTimer();
	void incrementTurnTimer(int iNumTurnSlices);
	int getMaxTurnLen() const;

	int getTargetScore() const; // Exposed to Python
	void setTargetScore(int iNewValue); // Exposed to Python

	int getNumGameTurnActive() const; // Exposed to Python
	DllExport int countNumHumanGameTurnActive() const; // Exposed to Python
	void changeNumGameTurnActive(int iChange);

	int getNumCities() const; // Exposed to Python
	int getNumCivCities() const; // Exposed to Python
	void changeNumCities(int iChange);

	int getStatusPromotion(int i) const;
	int getNumStatusPromotions() const;
	void setStatusPromotions();

	int getTotalPopulation() const; // Exposed to Python
	void changeTotalPopulation(int iChange);

	int getTradeRoutes() const; // Exposed to Python
	void changeTradeRoutes(int iChange); // Exposed to Python

	int getFreeTradeCount() const; // Exposed to Python
	bool isFreeTrade() const; // Exposed to Python
	void changeFreeTradeCount(int iChange); // Exposed to Python

	int getNoNukesCount() const; // Exposed to Python
	bool isNoNukes() const; // Exposed to Python
	void changeNoNukesCount(int iChange); // Exposed to Python

	int getSecretaryGeneralTimer(VoteSourceTypes eVoteSource) const; // Exposed to Python
	void setSecretaryGeneralTimer(VoteSourceTypes eVoteSource, int iNewValue);
	void changeSecretaryGeneralTimer(VoteSourceTypes eVoteSource, int iChange);

	int getVoteTimer(VoteSourceTypes eVoteSource) const; // Exposed to Python
	void setVoteTimer(VoteSourceTypes eVoteSource, int iNewValue);
	void changeVoteTimer(VoteSourceTypes eVoteSource, int iChange);

	int getNukesExploded() const; // Exposed to Python
	void changeNukesExploded(int iChange); // Exposed to Python

	int getMaxPopulation() const; // Exposed to Python
	int getMaxLand() const; // Exposed to Python
	int getMaxTech() const; // Exposed to Python
	int getMaxWonders() const; // Exposed to Python
	int getInitPopulation() const; // Exposed to Python
	int getInitLand() const; // Exposed to Python
	int getInitTech() const; // Exposed to Python
	int getInitWonders() const; // Exposed to Python
	DllExport void initScoreCalculation();
	// < M.A.D. Nukes Start >
	CvPlot* getLastNukeStrikePlot() const; // Exposed to Python
	void setLastNukeStrikePlot(CvPlot* pPlot); // Exposed to Python
	// < M.A.D. Nukes End   >

	int getAIAutoPlay(PlayerTypes iPlayer) const; // Exposed to Python
		void setAIAutoPlay(PlayerTypes iPlayer, int iNewValue, bool bForced = false);
	DllExport void setAIAutoPlayExternal(int iNewValue); // Exposed to Python
	void changeAIAutoPlay(PlayerTypes iPlayer, int iChange);

	bool isForcedAIAutoPlay(PlayerTypes iPlayer) const; // Exposed to Python
	int getForcedAIAutoPlay(PlayerTypes iPlayer) const; // Exposed to Python
	void setForcedAIAutoPlay(PlayerTypes iPlayer, int iNewValue, bool bForced = false); // Exposed to Python
	void changeForcedAIAutoPlay(PlayerTypes iPlayer, int iNewValue);

	int getWaterAnimalSpawnChance() const; // Exposed to Python
	void setWaterAnimalSpawnChance(int iNewValue); // Exposed to Python
	void changeWaterAnimalSpawnChance(int iChange); // Exposed to Python

	bool getStarshipLaunched(int ID) const;
	bool getDiplomaticVictoryAchieved(int ID) const;

	int getCurrentVoteID() const;
	void setCurrentVoteID(int iNewValue);

	int getXResolution() const; // Exposed to Python
	void setXResolution(int iNewValue); // Exposed to Python
	void changeXResolution(int iChange); // Exposed to Python

	int getYResolution() const; // Exposed to Python
	void setYResolution(int iNewValue); // Exposed to Python
	void changeYResolution(int iChange); // Exposed to Python

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

	int getModderGameOption(ModderGameOptionTypes eIndex) const; // Exposed to Python
	bool isModderGameOption(ModderGameOptionTypes eIndex) const; // Exposed to Python
	void setModderGameOption(ModderGameOptionTypes eIndex, bool bNewValue); // Exposed to Python
	void setModderGameOption(ModderGameOptionTypes eIndex, int iNewValue); // Exposed to Python

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
	void setFutureEras();

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

	bool isEarlyGame() const;

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

	bool isScoreDirty() const; // Exposed to Python
	void setScoreDirty(bool bNewValue); // Exposed to Python

	TeamTypes getCircumnavigatedTeam() const; // Exposed to Python
	void setCircumnavigatedTeam(TeamTypes eTeam); // Exposed to Python
	bool circumnavigationAvailable() const;

	bool isDiploVote(VoteSourceTypes eVoteSource) const; // Exposed to Python
	int getDiploVoteCount(VoteSourceTypes eVoteSource) const;
	void changeDiploVote(VoteSourceTypes eVoteSource, int iChange); // Exposed to Python
	bool canDoResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData) const;
	bool isValidVoteSelection(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData) const;

	DllExport bool isDebugMode() const; // Exposed to Python
	DllExport void toggleDebugMode(); // Exposed to Python
	DllExport void updateDebugModeCache();

	int getPitbossTurnTime() const; // Exposed to Python
	void setPitbossTurnTime(int iHours); // Exposed to Python

	DllExport bool isHotSeat() const; // Exposed to Python
	DllExport bool isPbem() const; // Exposed to Python
	DllExport bool isPitboss() const; // Exposed to Python
	bool isSimultaneousTeamTurns() const; // Exposed to Python

	DllExport bool isFinalInitialized() const; // Exposed to Python
	DllExport void setFinalInitialized(bool bNewValue);

	bool getPbemTurnSent() const;
	DllExport void setPbemTurnSent(bool bNewValue);

	DllExport bool getHotPbemBetweenTurns() const;
	void setHotPbemBetweenTurns(bool bNewValue);

	bool isPlayerOptionsSent() const;
	void sendPlayerOptions(bool bForce = false);

	DllExport PlayerTypes getActivePlayer() const; // Exposed to Python
	DllExport void setActivePlayer(PlayerTypes eNewValue, bool bForceHotSeat = false); // Exposed to Python
	DllExport void updateUnitEnemyGlow();

	HandicapTypes getHandicapType() const;
	void setHandicapType(HandicapTypes eHandicap);

	DllExport PlayerTypes getPausePlayer() const; // Exposed to Python
	DllExport bool isPaused() const; // Exposed to Python
	DllExport void setPausePlayer(PlayerTypes eNewValue);

	UnitTypes getBestLandUnit() const; // Exposed to Python
	int getBestLandUnitCombat() const; // Exposed to Python
	void setBestLandUnit(UnitTypes eNewValue);

	TeamTypes getWinner() const; // Exposed to Python
	VictoryTypes getVictory() const; // Exposed to Python
	void setWinner(TeamTypes eNewWinner, VictoryTypes eNewVictory); // Exposed to Python

	DllExport GameStateTypes getGameState() const; // Exposed to Python
	DllExport void setGameState(GameStateTypes eNewValue);

	EraTypes getStartEra() const; // Exposed to Python

	CalendarTypes getCalendar() const; // Exposed to Python

	GameSpeedTypes getGameSpeedType() const; // Exposed to Python 

	PlayerTypes getRankPlayer(int iRank) const; // Exposed to Python
	void setRankPlayer(int iRank, PlayerTypes ePlayer);

	int getPlayerRank(PlayerTypes ePlayer) const; // Exposed to Python 
	void setPlayerRank(PlayerTypes ePlayer, int iRank);

	DllExport int getPlayerScore(PlayerTypes ePlayer) const; // Exposed to Python
	void setPlayerScore(PlayerTypes ePlayer, int iScore);

	TeamTypes getRankTeam(int iRank) const; // Exposed to Python
	void setRankTeam(int iRank, TeamTypes eTeam);

	int getTeamRank(TeamTypes eTeam)const; // Exposed to Python
	void setTeamRank(TeamTypes eTeam, int iRank);

	DllExport int getTeamScore(TeamTypes eTeam) const; // Exposed to Python
	void setTeamScore(TeamTypes eTeam, int iScore);

	DllExport bool isOption(GameOptionTypes eIndex) const; // Exposed to Python
	void setOption(GameOptionTypes eIndex, bool bEnabled);

	DllExport bool isMPOption(MultiplayerOptionTypes eIndex) const; // Exposed to Python
	void setMPOption(MultiplayerOptionTypes eIndex, bool bEnabled);

	bool isForcedControl(ForceControlTypes eIndex) const; // Exposed to Python
	void setForceControl(ForceControlTypes eIndex, bool bEnabled);

	int getUnitCreatedCount(UnitTypes eIndex) const; // Exposed to Python
	void incrementUnitCreatedCount(UnitTypes eIndex);
	bool isUnitMaxedOut(UnitTypes eIndex, int iExtra = 0) const; // Exposed to Python

	int getBuildingCreatedCount(BuildingTypes eIndex) const; // Exposed to Python
	bool isBuildingMaxedOut(BuildingTypes eIndex, int iExtra = 0) const; // Exposed to Python
	void incrementBuildingCreatedCount(BuildingTypes eIndex);

	int getProjectCreatedCount(ProjectTypes eIndex) const; // Exposed to Python
	bool isProjectMaxedOut(ProjectTypes eIndex, int iExtra = 0) const; // Exposed to Python
	void incrementProjectCreatedCount(ProjectTypes eIndex, int iExtra = 1);

	int getForceCivicCount(CivicTypes eIndex) const; // Exposed to Python
	bool isForceCivic(CivicTypes eIndex) const; // Exposed to Python
	bool isForceCivicOption(CivicOptionTypes eCivicOption) const; // Exposed to Python
	void changeForceCivicCount(CivicTypes eIndex, int iChange);

	PlayerVoteTypes getVoteOutcome(VoteTypes eIndex) const; // Exposed to Python
	bool isVotePassed(VoteTypes eIndex) const; // Exposed to Python
	void setVoteOutcome(const VoteTriggeredData& kData, PlayerVoteTypes eNewValue);

	bool isVictoryValid(VictoryTypes eIndex) const; // Exposed to Python
	void setVictoryValid(VictoryTypes eIndex, bool bValid);

	bool isSpecialUnitValid(SpecialUnitTypes eIndex) const; // Exposed to Python  
	void makeSpecialUnitValid(SpecialUnitTypes eIndex); // Exposed to Python

	bool isSpecialBuildingValid(SpecialBuildingTypes eIndex) const; // Exposed to Python
	void makeSpecialBuildingValid(SpecialBuildingTypes eIndex, bool bAnnounce = false); // Exposed to Python

	//TB Nukefix (Reversal) Comment out the next two lines
	//bool isNukesValid() const; // Exposed to Python  
	//void makeNukesValid(bool bValid = true); // Exposed to Python

	bool isInAdvancedStart() const; // Exposed to Python  

	void setVoteChosen(int iSelection, int iVoteId);

	int getReligionGameTurnFounded(ReligionTypes eIndex) const; // Exposed to Python
	bool isReligionFounded(ReligionTypes eIndex) const; // Exposed to Python
	void makeReligionFounded(ReligionTypes eIndex, PlayerTypes ePlayer);

	int getTechGameTurnDiscovered(TechTypes eTech) const; // Exposed to Python
	bool isTechDiscovered(TechTypes eTech) const; // Exposed to Python
	void makeTechDiscovered(TechTypes eTech);

	bool isReligionSlotTaken(ReligionTypes eReligion) const; // Exposed to Python
	void setReligionSlotTaken(ReligionTypes eReligion, bool bTaken);

	bool isGameStart(); // Exposed to Python

	int countNumReligionsFounded() const; // Exposed to Python
	int countNumReligionTechsDiscovered() const; // Exposed to Python

	bool isTechCanFoundReligion(TechTypes eIndex) const; // Exposed to Python
	void setTechCanFoundReligion(TechTypes eIndex, bool bUsed);

	CvCity* getHolyCity(ReligionTypes eIndex) const; // Exposed to Python
	void setHolyCity(ReligionTypes eIndex, const CvCity* pNewValue, bool bAnnounce); // Exposed to Python

	int getCorporationGameTurnFounded(CorporationTypes eIndex) const; // Exposed to Python
	bool isCorporationFounded(CorporationTypes eIndex) const; // Exposed to Python
	void makeCorporationFounded(CorporationTypes eIndex, PlayerTypes ePlayer);

	CvCity* getHeadquarters(CorporationTypes eIndex) const; // Exposed to Python
	void setHeadquarters(CorporationTypes eIndex, CvCity* pNewValue, bool bAnnounce); // Exposed to Python

	PlayerVoteTypes getPlayerVote(PlayerTypes eOwnerIndex, int iVoteId) const; // Exposed to Python
	void setPlayerVote(PlayerTypes eOwnerIndex, int iVoteId, PlayerVoteTypes eNewValue);
	void castVote(PlayerTypes eOwnerIndex, int iVoteId, PlayerVoteTypes ePlayerVote);

	DllExport const CvWString & getName();
	void setName(const TCHAR* szName);

	// Script data needs to be a narrow string for pickling in Python
	std::string getScriptData() const; // Exposed to Python
	void setScriptData(std::string szNewValue); // Exposed to Python

	bool isDestroyedCityName(CvWString& szName) const;
	void addDestroyedCityName(const CvWString& szName);

	bool isGreatPersonBorn(CvWString& szName) const;
	void addGreatPersonBornName(const CvWString& szName);

	DllExport int getIndexAfterLastDeal(); // Exposed to Python
	int getNumDeals() const; // Exposed to Python
	DllExport CvDeal* getDeal(int iID); // Exposed to Python
	CvDeal* addDeal();
	void deleteDeal(int iID);
	// iteration
	CvDeal* firstDeal(int *pIterIdx, bool bRev=false) const; // Exposed to Python
	CvDeal* nextDeal(int *pIterIdx, bool bRev=false) const; // Exposed to Python

	VoteSelectionData* getVoteSelection(int iID) const;
	VoteSelectionData* addVoteSelection(VoteSourceTypes eVoteSource);
	void deleteVoteSelection(int iID);

	VoteTriggeredData* getVoteTriggered(int iID) const;
	VoteTriggeredData* addVoteTriggered(const VoteSelectionData& kData, int iChoice);
	VoteTriggeredData* addVoteTriggered(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kOptionData);
	void deleteVoteTriggered(int iID);

	CvRandom& getMapRand(); // Exposed to Python
	int getMapRandNum(int iNum, const char* pszLog);

	CvRandom& getSorenRand(); // Exposed to Python
	int getSorenRandNum(int iNum, const char* pszLog);
	struct SorenRand
	{
		SorenRand(const char* logMsg) : logMsg(logMsg) {}
		int operator()(const int maxVal) const;
		CvString logMsg;
	};

	void logRandomResult(const wchar* szStreamName, const char* pszLog, int iMax, int iNum);

	DllExport int calculateSyncChecksum(); // Exposed to Python
	DllExport int calculateOptionsChecksum(); // Exposed to Python

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

	void logMsg(char* format, ... );
	void logDebugMsg(char* format, ...);
	void logMsgTo(const TCHAR* logFile, char* format, ...);

	void addPlayer(PlayerTypes eNewPlayer, LeaderHeadTypes eLeader, CivilizationTypes eCiv, bool bSetAlive = true); // Exposed to Python
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
	void setPlotExtraYield(int iX, int iY, YieldTypes eYield, int iCost);   // exposed to Python
	void removePlotExtraYield(int iX, int iY);

	int getPlotExtraCost(int iX, int iY) const;
	void changePlotExtraCost(int iX, int iY, int iCost);   // exposed to Python
	void removePlotExtraCost(int iX, int iY);

	ReligionTypes getVoteSourceReligion(VoteSourceTypes eVoteSource) const; // Exposed to Python
	void setVoteSourceReligion(VoteSourceTypes eVoteSource, ReligionTypes eReligion, bool bAnnounce = false); // Exposed to Python

	bool isEventActive(EventTriggerTypes eTrigger) const;    // exposed to Python
	DllExport void initEvents();

	bool isCivEverActive(CivilizationTypes eCivilization) const; // Exposed to Python
	bool isLeaderEverActive(LeaderHeadTypes eLeader) const; // Exposed to Python

	void processBuilding(BuildingTypes eBuilding, int iChange);

	bool foundBarbarianCity();

	bool pythonIsBonusIgnoreLatitudes() const;

	inline bool isRecalculatingModifiers() { return m_bRecalculatingModifiers; }

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
/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	MapTypes m_eCurrentMap;
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/
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
	int m_iWaterAnimalSpawnChance;
	int m_iXResolution;
	int m_iYResolution;
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
	std::vector<PlotExtraCost> m_aPlotExtraCosts;
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
	CvMainPropertySolver m_PropertySolver;

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
};

#endif
