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
	explicit CyGame(CvGame* pGame);			// Call from C++
	explicit CyGame(CvGameAI* pGame);		// Call from C++;
	const CvGame* getGame() const { return m_pGame; }	// Call from C++
	bool isNone() const { return m_pGame == NULL; }

	int getCurrentMap() const;

	bool isMultiplayer();

	void updateScore(bool bForce);
	void cycleCities(bool bForward, bool bAdd);
	void cycleSelectionGroups(bool bClear, bool bForward, bool bWorkers);
	bool cyclePlotUnits(CyPlot* pPlot, bool bForward, bool bAuto, int iCount);

	void selectionListMove(CyPlot* pPlot, bool bAlt, bool bShift, bool bCtrl);
	void selectionListGameNetMessage(int eMessage, int iData2, int iData3, int iData4, int iFlags, bool bAlt, bool bShift);
	void selectedCitiesGameNetMessage(int eMessage, int iData2, int iData3, int iData4, bool bOption, bool bAlt, bool bShift, bool bCtrl);
	void cityPushOrder(CyCity* pCity, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl);

	int getSymbolID(int iSymbol);

	int getProductionPerPopulation(int /*HurryTypes*/ eHurry);

	int getAdjustedPopulationPercent(int /*VictoryTypes*/ eVictory);
	int getAdjustedLandPercent(int /* VictoryTypes*/ eVictory);

	bool isTeamVote(int /*VoteTypes*/ eVote) const;
	bool isChooseElection(int /*VoteTypes*/ eVote) const;
	bool isTeamVoteEligible(int /*TeamTypes*/ eTeam, int /*VoteSourceTypes*/ eVoteSource) const;
	int countPossibleVote(int /*VoteTypes*/ eVote, int /*VoteSourceTypes*/ eVoteSource) const;
	int getVoteRequired(int /*VoteTypes*/ eVote, int /*VoteSourceTypes*/ eVoteSource) const;
	int getSecretaryGeneral(int /*VoteSourceTypes*/ eVoteSource) const;
	bool canHaveSecretaryGeneral(int /*VoteSourceTypes*/ eVoteSource) const;
	int getVoteSourceReligion(int /*VoteSourceTypes*/ eVoteSource) const;
	void setVoteSourceReligion(int /*VoteSourceTypes*/ eVoteSource, int /*ReligionTypes*/ eReligion, bool bAnnounce);

	int countCivPlayersAlive();
	int countCivPlayersEverAlive();
	int countCivTeamsAlive();
	int countCivTeamsEverAlive();
	int countHumanPlayersAlive();

	int countTotalCivPower();
	int countTotalNukeUnits();
	int countKnownTechNumTeams(int /*TechTypes*/ eTech);
	int getNumFreeBonuses(int /*BuildingTypes*/ eBuilding);

	int countReligionLevels(int /*ReligionTypes*/ eReligion);
	int calculateReligionPercent(int /* ReligionTypes*/ eReligion);
	int countCorporationLevels(int /*CorporationTypes*/ eCorporation);

	int goldenAgeLength();
	int victoryDelay(int /*VictoryTypes*/ eVictory);
	int getImprovementUpgradeTime(int /* ImprovementTypes*/ eImprovement);
	bool canTrainNukes();

	int /* EraTypes */ getHighestEra();
	int /* EraTypes */ getCurrentEra();

	int getActiveTeam();
	int /* CivilizationTypes */ getActiveCivilizationType();
	bool isNetworkMultiPlayer();
	bool isGameMultiPlayer();
	bool isTeamGame();

	bool isModem();
	void setModem(bool bModem);

	void reviveActivePlayer();
	void revivePlayer(int /*PlayerTypes*/ iPlayer);

	int getNumHumanPlayers();
	int getGameTurn();
	void setGameTurn(int iNewValue);
	int getTurnYear(int iGameTurn);
	int getGameTurnYear();

	int getElapsedGameTurns();
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

	int getNumGameTurnActive();
	int countNumHumanGameTurnActive();
	int getNumCities();
	int getNumCivCities();
	int getTotalPopulation();

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

	CyPlot* getLastNukeStrikePlot();
	void setLastNukeStrikePlot(CyPlot* pPlot);

	int getAIAutoPlay(int iPlayer) const;
	void setAIAutoPlay(int iPlayer, int iNewValue);

	bool isForcedAIAutoPlay(int iPlayer) const;
	int getForcedAIAutoPlay(int iPlayer) const;
	void setForcedAIAutoPlay(int iPlayer, int iNewValue, bool bForced = false);

	// < Water Animals Start >
	int getWaterAnimalSpawnChance() const;
	void setWaterAnimalSpawnChance(int iNewValue);
	void changeWaterAnimalSpawnChance(int iChange);

	int getXResolution() const;
	void setXResolution(int iNewValue);
	void changeXResolution(int iChange);

	int getYResolution() const;
	void setYResolution(int iNewValue);
	void changeYResolution(int iChange);
	// < Water Animals End   >

	bool isScoreDirty() const;
	void setScoreDirty(bool bNewValue);

	int getCircumnavigatedTeam() const;
	void setCircumnavigatedTeam(int iTeamType);

	bool isDiploVote(int /*VoteSourceTypes*/ eVoteSource) const;
	void changeDiploVote(int /*VoteSourceTypes*/ eVoteSource, int iChange);
	bool isDebugMode() const;
	void toggleDebugMode();

	int getChtLvl() const;

	int getPitbossTurnTime();
	void setPitbossTurnTime(int iHours);
	bool isHotSeat();
	bool isPbem();
	bool isPitboss();
	bool isSimultaneousTeamTurns();

	bool isFinalInitialized();

	int /*PlayerTypes*/ getActivePlayer();
	void setActivePlayer(int /*PlayerTypes*/ eNewValue, bool bForceHotSeat);
	int getPausePlayer();
	bool isPaused();
	int /*UnitTypes*/ getBestLandUnit();
	int getBestLandUnitCombat();

	bool getStarshipLaunched(int ID);
	bool getDiplomaticVictoryAchieved(int ID);

	int /*TeamTypes*/ getWinner();
	int /*VictoryTypes*/ getVictory();
	void setWinner(int /*TeamTypes*/ eNewWinner, int /*VictoryTypes*/ eNewVictory);
	int /*GameStateTypes*/ getGameState();
	int /*HandicapTypes*/ getHandicapType();
	CalendarTypes getCalendar() const;
	int /*EraTypes*/ getStartEra();
	int /*GameSpeedTypes*/ getGameSpeedType();
	/*PlayerTypes*/ int getRankPlayer(int iRank);
	int getPlayerRank(int /*PlayerTypes*/ iIndex);
	int getPlayerScore(int /*PlayerTypes*/ iIndex);
	int /*TeamTypes*/ getRankTeam(int iRank);
	int getTeamRank(int /*TeamTypes*/ iIndex);
	int getTeamScore(int /*TeamTypes*/ iIndex);
	bool isOption(int /*GameOptionTypes*/ eIndex);
	void setOption(int /*GameOptionTypes*/ eIndex, bool bEnabled);
	bool isMPOption(int /*MultiplayerOptionTypes*/ eIndex);
	bool isForcedControl(int /*ForceControlTypes*/ eIndex);
	int getUnitCreatedCount(int /*UnitTypes*/ eIndex);
	int getBuildingCreatedCount(int /*BuildingTypes*/ eIndex) const;
	bool isBuildingMaxedOut(int /*BuildingTypes*/ eIndex, int iExtra);
	bool isUnitMaxedOut(int /*UnitTypes*/ eIndex, int iExtra);

	int getProjectCreatedCount(int /*ProjectTypes*/ eIndex);
	bool isProjectMaxedOut(int /*ProjectTypes*/ eIndex, int iExtra);

	int getForceCivicCount(int /*CivicTypes*/ eIndex);
	bool isForceCivic(int /*CivicTypes*/ eIndex);
	bool isForceCivicOption(int /*CivicOptionTypes*/ eCivicOption);

	int getVoteOutcome(int /*VoteTypes*/ eIndex);

	int getReligionGameTurnFounded(int /*ReligionTypes*/ eIndex);
	bool isReligionFounded(int /*ReligionTypes*/ eIndex);
	bool isReligionSlotTaken(int /*ReligionTypes*/ eIndex);

	bool isGameStart();
	int countNumReligionsFounded();
	int countNumReligionTechsDiscovered();
	bool isTechCanFoundReligion(int /*TechTypes*/ eIndex);

	int getCorporationGameTurnFounded(int /*CorporationTypes*/ eIndex);
	bool isCorporationFounded(int /*CorporationTypes*/ eIndex);
	bool isVotePassed(int /*VoteTypes*/ eIndex) const;
	bool isVictoryValid(int /*VictoryTypes*/ eIndex);
	bool isSpecialUnitValid(int /*SpecialUnitTypes*/ eSpecialUnitType);
	void makeSpecialUnitValid(int /*SpecialUnitTypes*/ eSpecialUnitType);
	bool isSpecialBuildingValid(int /*SpecialBuildingTypes*/ eIndex);
	void makeSpecialBuildingValid(int /*SpecialBuildingTypes*/ eIndex);
	//TB Nukefix (Reversal) The following 2 lines should be commented out
	//bool isNukesValid();
	//void makeNukesValid(bool bValid);
	bool isInAdvancedStart();

	CyCity* getHolyCity(int /*ReligionTypes*/ eIndex);
	void setHolyCity(int /*ReligionTypes*/ eIndex, CyCity* pNewValue, bool bAnnounce);
	void clearHolyCity(int /*ReligionTypes*/ eIndex);

	CyCity* getHeadquarters(int /*CorporationTypes*/ eIndex);
	void setHeadquarters(int /*CorporationTypes*/ eIndex, CyCity* pNewValue, bool bAnnounce);
	void clearHeadquarters(int /*CorporationTypes*/ eIndex);

	int getPlayerVote(int /*PlayerTypes*/ eOwnerIndex, int iVoteId);

	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);

	void setName(TCHAR* szName);
	std::wstring getName();
	int getIndexAfterLastDeal();
	int getNumDeals();
	CyDeal* getDeal(int iID);
	CyDeal* addDeal();
	void deleteDeal(int iID);
	CvRandom& getMapRand();
	int getMapRandNum(int iNum, TCHAR* pszLog);
	CvRandom& getSorenRand();
	int getSorenRandNum(int iNum, TCHAR* pszLog);
	int calculateSyncChecksum();
	int calculateOptionsChecksum();
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

	void drawBattleEffects();

	int getCultureThreshold(int /*CultureLevelTypes*/ eLevel);

	void setPlotExtraYield(int iX, int iY, int /*YieldTypes*/ eYield, int iExtraYield);
	void changePlotExtraCost(int iX, int iY, int iExtraCost);

	bool isCivEverActive(int /*CivilizationTypes*/ eCivilization);
	bool isLeaderEverActive(int /*LeaderHeadTypes*/ eLeader);

	bool isEventActive(int /*EventTriggerTypes*/ eTrigger);
	void doControl(int iControl);

	bool canRegenerateMap() const;
	bool regenerateMap();

	void saveGame(std::string fileName) const;

	std::string getDLLPath() const;
	std::string getExePath() const;

	int getCutLosersCounter() const;
	int getHighToLowCounter() const;
	bool cheatCodesEnabled();
	void setVictoryValid(int iVictory, bool bNewVal);
	bool isModderGameOption(int /*ModderGameOptionTypes*/ eIndex);
	int getModderGameOption(int /*ModderGameOptionTypes*/ eIndex);
	void setModderGameOption(int /*ModderGameOptionTypes*/ eIndex, int iNewValue);
	void setFutureEras();
	bool canEverResearch(int iTech) const;
	bool canEverConstruct(int iBuilding) const;
	bool canEverTrain(int iUnit) const;
	bool canEverSpread(int iCorporation) const;

	const char* getC2CVersion() const;
protected:
	CvGame* m_pGame;
};

#endif	// #ifndef CyGame
