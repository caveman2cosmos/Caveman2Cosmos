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

	int getCurrentMap() const;

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

	int getProductionPerPopulation(int /*HurryTypes*/ eHurry) const;

	int getAdjustedPopulationPercent(int /*VictoryTypes*/ eVictory) const;
	int getAdjustedLandPercent(int /* VictoryTypes*/ eVictory) const;

	bool isTeamVote(int /*VoteTypes*/ eVote) const;
	bool isChooseElection(int /*VoteTypes*/ eVote) const;
	bool isTeamVoteEligible(int /*TeamTypes*/ eTeam, int /*VoteSourceTypes*/ eVoteSource) const;
	int countPossibleVote(int /*VoteTypes*/ eVote, int /*VoteSourceTypes*/ eVoteSource) const;
	int getVoteRequired(int /*VoteTypes*/ eVote, int /*VoteSourceTypes*/ eVoteSource) const;
	int getSecretaryGeneral(int /*VoteSourceTypes*/ eVoteSource) const;
	bool canHaveSecretaryGeneral(int /*VoteSourceTypes*/ eVoteSource) const;
	int getVoteSourceReligion(int /*VoteSourceTypes*/ eVoteSource) const;
	void setVoteSourceReligion(int /*VoteSourceTypes*/ eVoteSource, int /*ReligionTypes*/ eReligion, bool bAnnounce);

	int countCivPlayersAlive() const;
	int countCivPlayersEverAlive() const;
	int countCivTeamsAlive() const;
	int countCivTeamsEverAlive() const;

	int countTotalCivPower() const;
	int countTotalNukeUnits() const;
	int countKnownTechNumTeams(int /*TechTypes*/ eTech) const;
	int getNumFreeBonuses(int /*BuildingTypes*/ eBuilding) const;

	int countReligionLevels(int /*ReligionTypes*/ eReligion) const;
	int calculateReligionPercent(int /* ReligionTypes*/ eReligion) const;
	int countCorporationLevels(int /*CorporationTypes*/ eCorporation) const;

	int goldenAgeLength() const;
	int victoryDelay(int /*VictoryTypes*/ eVictory) const;
	int getImprovementUpgradeTime(int /* ImprovementTypes*/ eImprovement) const;
	bool canTrainNukes() const;

	int /* EraTypes */ getHighestEra() const;
	int /* EraTypes */ getCurrentEra() const;

	int getActiveTeam() const;
	int /* CivilizationTypes */ getActiveCivilizationType() const;
	bool isNetworkMultiPlayer() const;
	bool isGameMultiPlayer() const;
	bool isTeamGame() const;

	bool isModem() const;
	void setModem(bool bModem);

	void reviveActivePlayer();
	void revivePlayer(int /*PlayerTypes*/ iPlayer);

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

	CyPlot* getLastNukeStrikePlot() const;
	void setLastNukeStrikePlot(CyPlot* pPlot);

	int getAIAutoPlay(int iPlayer) const;
	void setAIAutoPlay(int iPlayer, int iNewValue);

	bool isForcedAIAutoPlay(int iPlayer) const;
	int getForcedAIAutoPlay(int iPlayer) const;
	void setForcedAIAutoPlay(int iPlayer, int iNewValue, bool bForced = false);

	bool isScoreDirty() const;
	void setScoreDirty(bool bNewValue);

	int getCircumnavigatedTeam() const;
	void setCircumnavigatedTeam(int iTeamType);

	bool isDiploVote(int /*VoteSourceTypes*/ eVoteSource) const;
	void changeDiploVote(int /*VoteSourceTypes*/ eVoteSource, int iChange);
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

	int /*PlayerTypes*/ getActivePlayer() const;
	void setActivePlayer(int /*PlayerTypes*/ eNewValue, bool bForceHotSeat);
	int getPausePlayer() const;
	bool isPaused() const;
	int /*UnitTypes*/ getBestLandUnit() const;
	int getBestLandUnitCombat() const;

	bool getStarshipLaunched(int ID) const;
	bool getDiplomaticVictoryAchieved(int ID) const;

	int /*TeamTypes*/ getWinner() const;
	int /*VictoryTypes*/ getVictory() const;
	void setWinner(int /*TeamTypes*/ eNewWinner, int /*VictoryTypes*/ eNewVictory);
	int /*GameStateTypes*/ getGameState() const;
	int /*HandicapTypes*/ getHandicapType() const;
	CalendarTypes getCalendar() const;
	int /*EraTypes*/ getStartEra() const;
	int /*GameSpeedTypes*/ getGameSpeedType() const;
	/*PlayerTypes*/ int getRankPlayer(int iRank) const;
	int getPlayerRank(int /*PlayerTypes*/ iIndex) const;
	int getPlayerScore(int /*PlayerTypes*/ iIndex) const;
	int /*TeamTypes*/ getRankTeam(int iRank) const;
	int getTeamRank(int /*TeamTypes*/ iIndex) const;
	int getTeamScore(int /*TeamTypes*/ iIndex) const;
	bool isOption(int /*GameOptionTypes*/ eIndex) const;
	void setOption(int /*GameOptionTypes*/ eIndex, bool bEnabled);
	bool isMPOption(int /*MultiplayerOptionTypes*/ eIndex) const;
	bool isForcedControl(int /*ForceControlTypes*/ eIndex) const;
	int getUnitCreatedCount(int /*UnitTypes*/ eIndex) const;
	int getBuildingCreatedCount(int /*BuildingTypes*/ eIndex) const;
	bool isBuildingMaxedOut(int /*BuildingTypes*/ eIndex, int iExtra) const;
	bool isUnitMaxedOut(int /*UnitTypes*/ eIndex, int iExtra) const;

	int getProjectCreatedCount(int /*ProjectTypes*/ eIndex) const;
	bool isProjectMaxedOut(int /*ProjectTypes*/ eIndex, int iExtra) const;

	int getForceCivicCount(int /*CivicTypes*/ eIndex) const;
	bool isForceCivic(int /*CivicTypes*/ eIndex) const;
	bool isForceCivicOption(int /*CivicOptionTypes*/ eCivicOption) const;

	int getVoteOutcome(int /*VoteTypes*/ eIndex) const;

	int getReligionGameTurnFounded(int /*ReligionTypes*/ eIndex) const;
	bool isReligionFounded(int /*ReligionTypes*/ eIndex) const;
	bool isReligionSlotTaken(int /*ReligionTypes*/ eIndex) const;

	bool isGameStart() const;
	int countNumReligionsFounded() const;
	int countNumReligionTechsDiscovered() const;
	bool isTechCanFoundReligion(int /*TechTypes*/ eIndex) const;

	int getCorporationGameTurnFounded(int /*CorporationTypes*/ eIndex) const;
	bool isCorporationFounded(int /*CorporationTypes*/ eIndex) const;
	bool isVotePassed(int /*VoteTypes*/ eIndex) const;
	bool isVictoryValid(int /*VictoryTypes*/ eIndex) const;
	bool isSpecialUnitValid(int /*SpecialUnitTypes*/ eSpecialUnitType) const;
	void makeSpecialUnitValid(int /*SpecialUnitTypes*/ eSpecialUnitType);
	bool isSpecialBuildingValid(int /*SpecialBuildingTypes*/ eIndex) const;
	void makeSpecialBuildingValid(int /*SpecialBuildingTypes*/ eIndex);

	bool isInAdvancedStart() const;

	CyCity* getHolyCity(int /*ReligionTypes*/ eIndex) const;
	void setHolyCity(int /*ReligionTypes*/ eIndex, CyCity* pNewValue, bool bAnnounce);
	void clearHolyCity(int /*ReligionTypes*/ eIndex);

	CyCity* getHeadquarters(int /*CorporationTypes*/ eIndex) const;
	void setHeadquarters(int /*CorporationTypes*/ eIndex, CyCity* pNewValue, bool bAnnounce);
	void clearHeadquarters(int /*CorporationTypes*/ eIndex);

	int getPlayerVote(int /*PlayerTypes*/ eOwnerIndex, int iVoteId) const;

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
	bool hasSkippedSaveChecksum() const;

	void saveReplay(int iPlayer);

	void addPlayer(int /*PlayerTypes*/ eNewPlayer, int /*LeaderHeadTypes*/ eLeader, int /*CivilizationTypes*/ eCiv, bool bSetAlive );
	void changeHumanPlayer( int /*PlayerTypes*/ eOldHuman , int /*PlayerTypes*/ eNewHuman );
	void addReplayMessage(int /*ReplayMessageTypes*/ eType, int /*PlayerTypes*/ ePlayer, std::wstring pszText, int iPlotX, int iPlotY, int /*ColorTypes*/ eColor);
	void log(TCHAR* str);
	void logw(std::wstring str);

	int getCultureThreshold(int /*CultureLevelTypes*/ eLevel) const;

	void setPlotExtraYield(int iX, int iY, int /*YieldTypes*/ eYield, int iExtraYield);
	void changePlotExtraCost(int iX, int iY, int iExtraCost);

	bool isCivEverActive(int /*CivilizationTypes*/ eCivilization) const;
	bool isLeaderEverActive(int /*LeaderHeadTypes*/ eLeader) const;

	bool isEventActive(int /*EventTriggerTypes*/ eTrigger) const;
	void doControl(int iControl);

	bool canRegenerateMap() const;
	bool regenerateMap();

	void saveGame(std::string fileName) const;

	std::string getDLLPath() const;
	std::string getExePath() const;

	int getCutLosersCounter() const;
	int getHighToLowCounter() const;
	bool cheatCodesEnabled() const;
	void setVictoryValid(int iVictory, bool bNewVal);

	bool isModderGameOption(int /*ModderGameOptionTypes*/ eIndex) const;
	int getModderGameOption(int /*ModderGameOptionTypes*/ eIndex) const;
	void setModderGameOption(int /*ModderGameOptionTypes*/ eIndex, int iNewValue);

	bool canEverResearch(int iTech) const;
	bool canEverConstruct(int iBuilding) const;
	bool canEverTrain(int iUnit) const;
	bool canEverSpread(int iCorporation) const;

	const char* getC2CVersion() const;

	void assignScenarioStartingPlots();

protected:
	CvGame& m_pGame;
};

#endif	// #ifndef CyGame
