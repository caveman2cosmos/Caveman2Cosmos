#pragma once

#ifndef CyGame_h
#define CyGame_h
//
// Python wrapper class for CvGame
// SINGLETON
// updated 6-5

class CvGame;
class CvGameAI;
class CyCity;
class CvRandom;
class CyDeal;
class CyReplayInfo;
class CyPlot;

class CyGame
{
public:
	CyGame();
	explicit CyGame(CvGame& pGame);			// Call from C++
	explicit CyGame(CvGameAI& pGame);		// Call from C++;

	MapTypes getCurrentMap() const;

	bool isMultiplayer() const;

	void updateScore(bool bForce);
	void cycleCities(bool bForward, bool bAdd);
	void cycleSelectionGroups(bool bClear, bool bForward, bool bWorkers);
	bool cyclePlotUnits(const CyPlot* pPlot, bool bForward, bool bAuto, int iCount);

	void selectionListMove(const CyPlot* pPlot, bool bAlt, bool bShift, bool bCtrl);
	void selectionListGameNetMessage(int eMessage, int iData2, int iData3, int iData4, int iFlags, bool bAlt, bool bShift);
	void selectedCitiesGameNetMessage(int eMessage, int iData2, int iData3, int iData4, bool bOption, bool bAlt, bool bShift, bool bCtrl);
	void cityPushOrder(const CyCity* pCity, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl);

	int getSymbolID(int iSymbol) const;

	int getProductionPerPopulation(HurryTypes eHurry) const;

	int getAdjustedPopulationPercent(VictoryTypes eVictory) const;
	int getAdjustedLandPercent( VictoryTypes eVictory) const;

	bool isTeamVote(VoteTypes eVote) const;
	bool isChooseElection(VoteTypes eVote) const;
	bool isTeamVoteEligible(TeamTypes eTeam, VoteSourceTypes eVoteSource) const;
	int countPossibleVote(VoteTypes eVote, VoteSourceTypes eVoteSource) const;
	int getVoteRequired(VoteTypes eVote, VoteSourceTypes eVoteSource) const;
	int getSecretaryGeneral(VoteSourceTypes eVoteSource) const;
	bool canHaveSecretaryGeneral(VoteSourceTypes eVoteSource) const;
	int getVoteSourceReligion(VoteSourceTypes eVoteSource) const;

	int countCivPlayersAlive() const;
	int countCivPlayersEverAlive() const;
	int countCivTeamsAlive() const;
	int countCivTeamsEverAlive() const;

	int countTotalCivPower() const;
	int countTotalNukeUnits() const;
	int countKnownTechNumTeams(TechTypes eTech) const;
	int getNumFreeBonuses(BuildingTypes eBuilding) const;

	int countReligionLevels(ReligionTypes eReligion) const;
	int calculateReligionPercent(ReligionTypes eReligion) const;
	int countCorporationLevels(CorporationTypes eCorporation) const;

	int goldenAgeLength() const;
	int victoryDelay(VictoryTypes eVictory) const;
	int getImprovementUpgradeTime(ImprovementTypes eImprovement) const;
	bool canTrainNukes() const;

	EraTypes getHighestEra() const;
	EraTypes getCurrentEra() const;

	int getActiveTeam() const;
	CivilizationTypes  getActiveCivilizationType() const;
	bool isNetworkMultiPlayer() const;
	bool isGameMultiPlayer() const;
	bool isTeamGame() const;

	bool isModem() const;
	void setModem(bool bModem);

	int getNumHumanPlayers() const;
	int getGameTurn() const;
	void setGameTurn(int iNewValue);
	int getTurnYear(int iGameTurn) const;
	int getGameTurnYear() const;

	int getElapsedGameTurns() const;
	int getMaxTurns() const;
	void setMaxTurns(int iNewValue);
	void changeMaxTurns(int iChange);
	int getMaxCityElimination() const;
	void setMaxCityElimination(int iNewValue);
	int getNumAdvancedStartPoints() const;
	void setNumAdvancedStartPoints(int iNewValue);
	int getStartTurn() const;
	int getStartYear() const;
	void setStartYear(int iNewValue);
	int getEstimateEndTurn() const;
	void setEstimateEndTurn(int iNewValue);
	int getTurnSlice() const;
	int getMinutesPlayed() const;
	int getTargetScore() const;
	void setTargetScore(int iNewValue);

	int getNumGameTurnActive() const;
	int countNumHumanGameTurnActive() const;
	int getNumCities() const;
	int getNumCivCities() const;
	int getTotalPopulation() const;

	int getTradeRoutes() const;
	void changeTradeRoutes(int iChange);
	int getFreeTradeCount() const;
	bool isFreeTrade() const;
	void changeFreeTradeCount(int iChange);
	int getNoNukesCount() const;
	bool isNoNukes() const;
	void changeNoNukesCount(int iChange);
	int getSecretaryGeneralTimer(int iVoteSource) const;
	int getVoteTimer(int iVoteSource) const;
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

	int getAIAutoPlay(int iPlayer) const;
	void setAIAutoPlay(int iPlayer, int iNewValue);

	bool isForcedAIAutoPlay(int iPlayer) const;
	int getForcedAIAutoPlay(int iPlayer) const;
	void setForcedAIAutoPlay(int iPlayer, int iNewValue, bool bForced = false);

	bool isScoreDirty() const;
	void setScoreDirty(bool bNewValue);

	int getCircumnavigatedTeam() const;
	void setCircumnavigatedTeam(int iTeamType);

	bool isDiploVote(VoteSourceTypes eVoteSource) const;
	void changeDiploVote(VoteSourceTypes eVoteSource, int iChange);
	bool isDebugMode() const;
	void toggleDebugMode();

	int getChtLvl() const;

	int getPitbossTurnTime() const;
	void setPitbossTurnTime(int iHours);
	bool isHotSeat() const;
	bool isPbem() const;
	bool isPitboss() const;
	bool isSimultaneousTeamTurns() const;

	bool isFinalInitialized() const;

	PlayerTypes getActivePlayer() const;
	void setActivePlayer(PlayerTypes eNewValue, bool bForceHotSeat);
	int getPausePlayer() const;
	bool isPaused() const;
	UnitTypes getBestLandUnit() const;
	int getBestLandUnitCombat() const;

	bool getStarshipLaunched(int ID) const;
	bool getDiplomaticVictoryAchieved(int ID) const;

	TeamTypes getWinner() const;
	VictoryTypes getVictory() const;
	void setWinner(TeamTypes eNewWinner, VictoryTypes eNewVictory);
	GameStateTypes getGameState() const;
	HandicapTypes getHandicapType() const;
	CalendarTypes getCalendar() const;
	EraTypes getStartEra() const;
	GameSpeedTypes getGameSpeedType() const;
	PlayerTypes getRankPlayer(int iRank) const;
	int getPlayerRank(PlayerTypes iIndex) const;
	int getPlayerScore(PlayerTypes iIndex) const;
	TeamTypes getRankTeam(int iRank) const;
	int getTeamRank(TeamTypes iIndex) const;
	int getTeamScore(TeamTypes iIndex) const;
	bool isOption(GameOptionTypes eIndex) const;
	void setOption(GameOptionTypes eIndex, bool bEnabled);
	bool isMPOption(MultiplayerOptionTypes eIndex) const;
	bool isForcedControl(ForceControlTypes eIndex) const;
	int getUnitCreatedCount(UnitTypes eIndex) const;
	int getBuildingCreatedCount(BuildingTypes eIndex) const;
	bool isBuildingMaxedOut(BuildingTypes eIndex, int iExtra) const;
	bool isUnitMaxedOut(UnitTypes eIndex, int iExtra) const;

	int getProjectCreatedCount(ProjectTypes eIndex) const;
	bool isProjectMaxedOut(ProjectTypes eIndex, int iExtra) const;

	int getForceCivicCount(CivicTypes eIndex) const;
	bool isForceCivic(CivicTypes eIndex) const;
	bool isForceCivicOption(CivicOptionTypes eCivicOption) const;

	int getVoteOutcome(VoteTypes eIndex) const;

	int getReligionGameTurnFounded(ReligionTypes eIndex) const;
	bool isReligionFounded(ReligionTypes eIndex) const;
	bool isReligionSlotTaken(ReligionTypes eIndex) const;

	bool isGameStart() const;
	int countNumReligionsFounded() const;
	int countNumReligionTechsDiscovered() const;
	bool isTechCanFoundReligion(TechTypes eIndex) const;

	int getCorporationGameTurnFounded(CorporationTypes eIndex) const;
	bool isCorporationFounded(CorporationTypes eIndex) const;
	bool isVotePassed(VoteTypes eIndex) const;
	bool isVictoryValid(VictoryTypes eIndex) const;
	bool isSpecialUnitValid(SpecialUnitTypes eSpecialUnitType) const;
	void makeSpecialUnitValid(SpecialUnitTypes eSpecialUnitType);
	bool isSpecialBuildingValid(SpecialBuildingTypes eIndex) const;
	void makeSpecialBuildingValid(SpecialBuildingTypes eIndex);

	bool isInAdvancedStart() const;

	CyCity* getHolyCity(ReligionTypes eIndex) const;
	void setHolyCity(ReligionTypes eIndex, CyCity* pNewValue, bool bAnnounce);
	void clearHolyCity(ReligionTypes eIndex);

	CyCity* getHeadquarters(CorporationTypes eIndex) const;
	void setHeadquarters(CorporationTypes eIndex, CyCity* pNewValue, bool bAnnounce);
	void clearHeadquarters(CorporationTypes eIndex);

	int getPlayerVote(PlayerTypes eOwnerIndex, int iVoteId) const;

	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);

	void setName(TCHAR* szName);
	std::wstring getName() const;
	int getIndexAfterLastDeal() const;
	int getNumDeals() const;
	CyDeal* getDeal(int iID) const;
	CyDeal* addDeal();
	void deleteDeal(int iID);
	CvRandom& getMapRand() const;
	int getMapRandNum(int iNum, TCHAR* pszLog) const;
	CvRandom& getSorenRand() const;
	int getSorenRandNum(int iNum, TCHAR* pszLog) const;
	int calculateSyncChecksum() const;
	int calculateOptionsChecksum() const;
	bool GetWorldBuilderMode() const;				// remove once CvApp is exposed
	bool isPitbossHost() const;				// remove once CvApp is exposed
	int getCurrentLanguage() const;				// remove once CvApp is exposed
	void setCurrentLanguage(int iNewLanguage);				// remove once CvApp is exposed

	int getReplayMessageTurn(int i) const;
	ReplayMessageTypes getReplayMessageType(int i) const;
	int getReplayMessagePlotX(int i) const;
	int getReplayMessagePlotY(int i) const;
	int getReplayMessagePlayer(int i) const;
	ColorTypes getReplayMessageColor(int i) const;
	std::wstring getReplayMessageText(int i) const;
	uint getNumReplayMessages() const;
	CyReplayInfo* getReplayInfo() const;
	void saveReplay(PlayerTypes ePlayer);
	void addReplayMessage(ReplayMessageTypes eType, PlayerTypes ePlayer, std::wstring pszText, int iPlotX, int iPlotY, ColorTypes eColor);

	bool hasSkippedSaveChecksum() const;

	void addPlayer(PlayerTypes eNewPlayer, LeaderHeadTypes eLeader, CivilizationTypes eCiv, bool bSetAlive);
	void changeHumanPlayer(PlayerTypes eOldHuman, PlayerTypes eNewHuman);

	void log(const char* file, char* str);
	void logw(std::wstring str);

	int getCultureThreshold(CultureLevelTypes eLevel) const;

	void setPlotExtraYield(int iX, int iY, YieldTypes eYield, int iExtraYield);
	void changePlotExtraCost(int iX, int iY, int iExtraCost);

	bool isCivEverActive(CivilizationTypes eCivilization) const;
	bool isLeaderEverActive(LeaderHeadTypes eLeader) const;

	bool isEventActive(EventTriggerTypes eTrigger) const;
	void doControl(ControlTypes iControl);

	bool canRegenerateMap() const;
	bool regenerateMap();

	void saveGame(std::string fileName) const;

	std::string getDLLPath() const;
	std::string getExePath() const;

	int getCutLosersCounter() const;
	int getHighToLowCounter() const;
	void setVictoryValid(int iVictory, bool bNewVal);

	bool isModderGameOption(ModderGameOptionTypes eIndex) const;
	int getModderGameOption(ModderGameOptionTypes eIndex) const;
	void setModderGameOption(ModderGameOptionTypes eIndex, int iNewValue);

	bool canEverResearch(TechTypes iTech) const;
	bool canEverConstruct(BuildingTypes iBuilding) const;
	bool canEverTrain(UnitTypes iUnit) const;
	bool canEverSpread(CorporationTypes iCorporation) const;

	const char* getC2CVersion() const;

	void assignStartingPlots(bool bScenario, bool bMapScript);

protected:
	CvGame& m_pGame;
};

#endif	// #ifndef CyGame
