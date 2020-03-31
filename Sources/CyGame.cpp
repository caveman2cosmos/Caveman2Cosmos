//
// Python wrapper class for CvGame
//

#include "CvGameCoreDLL.h"
#include "CyReplayInfo.h"
#include "CvReplayInfo.h"

CyGame::CyGame() : m_pGame(NULL) { m_pGame = &GC.getGame(); }

CyGame::CyGame(CvGame* pGame) : m_pGame(pGame) {}

CyGame::CyGame(CvGameAI* pGame) : m_pGame(pGame) {}

bool CyGame::isMultiplayer() const
{
	return GC.getInitCore().getMultiplayer();
}

void CyGame::updateScore(bool bForce)
{
	if (m_pGame) m_pGame->updateScore(bForce);
}

void CyGame::selectedCitiesGameNetMessage(int eMessage, int iData2, int iData3, int iData4, bool bOption, bool bAlt, bool bShift, bool bCtrl)
{
	GC.getGame().selectedCitiesGameNetMessage(eMessage, iData2, iData3, iData4, bOption, bAlt, bShift, bCtrl);
}

int CyGame::getSymbolID(int iSymbol) const
{
	return m_pGame ? m_pGame->getSymbolID(iSymbol) : -1;
}

int CyGame::getProductionPerPopulation(int /*HurryTypes*/ eHurry) const
{
	return m_pGame ? m_pGame->getProductionPerPopulation((HurryTypes) eHurry) : -1;
}

int CyGame::getAdjustedPopulationPercent(int /*VictoryTypes*/ eVictory) const
{
	return m_pGame ? m_pGame->getAdjustedPopulationPercent((VictoryTypes) eVictory) : -1;
}

int CyGame::getAdjustedLandPercent(int /* VictoryTypes*/ eVictory) const
{
	return m_pGame ? m_pGame->getAdjustedLandPercent((VictoryTypes) eVictory) : -1;
}

bool CyGame::isChooseElection(int /*VoteTypes*/ eVote) const
{
	return m_pGame ? m_pGame->isChooseElection((VoteTypes) eVote) : false;
}

bool CyGame::isTeamVoteEligible(int /*TeamTypes*/ eTeam, int /*VoteSourceTypes*/ eVoteSource) const
{
	return m_pGame ? m_pGame->isTeamVoteEligible((TeamTypes) eTeam, (VoteSourceTypes)eVoteSource) : false;
}

int CyGame::countPossibleVote(int /*VoteTypes*/ eVote, int /*VoteSourceTypes*/ eVoteSource) const
{
	return m_pGame ? m_pGame->countPossibleVote((VoteTypes) eVote, (VoteSourceTypes)eVoteSource) : -1;
}

int CyGame::getVoteRequired(int /*VoteTypes*/ eVote, int /*VoteSourceTypes*/ eVoteSource) const
{
	return m_pGame ? (int)m_pGame->getVoteRequired((VoteTypes)eVote, (VoteSourceTypes) eVoteSource) : -1;
}

int CyGame::getSecretaryGeneral(int /*VoteSourceTypes*/ eVoteSource) const
{
	return m_pGame ? (int)m_pGame->getSecretaryGeneral((VoteSourceTypes) eVoteSource) : -1;
}

bool CyGame::canHaveSecretaryGeneral(int /*VoteSourceTypes*/ eVoteSource) const
{
	return m_pGame ? (int)m_pGame->canHaveSecretaryGeneral((VoteSourceTypes) eVoteSource) : -1;
}

int CyGame::getVoteSourceReligion(int /*VoteSourceTypes*/ eVoteSource) const
{
	return m_pGame ? (int)m_pGame->getVoteSourceReligion((VoteSourceTypes) eVoteSource) : -1;
}

int CyGame::countCivPlayersAlive() const
{
	return m_pGame ? m_pGame->countCivPlayersAlive() : -1;
}

int CyGame::countCivPlayersEverAlive() const
{
	return m_pGame ? m_pGame->countCivPlayersEverAlive() : -1;
}

int CyGame::countCivTeamsAlive() const
{
	return m_pGame ? m_pGame->countCivTeamsAlive() : -1;
}

int CyGame::countKnownTechNumTeams(int /*TechTypes*/ eTech) const
{
	return m_pGame ? m_pGame->countKnownTechNumTeams((TechTypes) eTech) : -1;
}

int CyGame::countReligionLevels(int /*ReligionTypes*/ eReligion) const
{
	return m_pGame ? m_pGame->countReligionLevels((ReligionTypes) eReligion) : -1;
}

int CyGame::countCorporationLevels(int /*CorporationTypes*/ eCorporation) const
{
	return m_pGame ? m_pGame->countCorporationLevels((CorporationTypes) eCorporation) : -1;
}

int CyGame::calculateReligionPercent(int /*ReligionTypes*/ eReligion) const
{
	return m_pGame ? m_pGame->calculateReligionPercent((ReligionTypes) eReligion) : -1;
}

int CyGame::goldenAgeLength() const
{
	return m_pGame ? m_pGame->goldenAgeLength() : -1;
}

int CyGame::getHighestEra() const
{
	return m_pGame ? (int) m_pGame->getHighestEra() : (int) NO_ERA;
}

int CyGame::getCurrentEra() const
{
	return m_pGame ? (int) m_pGame->getCurrentEra() : (int) NO_ERA;
}

int CyGame::getActiveTeam() const
{
	return m_pGame ? (int) m_pGame->getActiveTeam() : (int) NO_TEAM;
}

int /* CivilizationTypes */ CyGame::getActiveCivilizationType() const
{
	return m_pGame ? (int) m_pGame->getActiveCivilizationType() : (int) NO_CIVILIZATION;
}

bool CyGame::isNetworkMultiPlayer() const
{
	return m_pGame ? m_pGame->isNetworkMultiPlayer() : false;
}

bool CyGame::isGameMultiPlayer() const
{
	return m_pGame ? m_pGame->isGameMultiPlayer() : false;
}

bool CyGame::isModem() const
{
	return m_pGame ? m_pGame->isModem() : true;	// err on the side of caution
}

void CyGame::setModem(bool bModem)
{
	if (m_pGame) m_pGame->setModem(bModem);
}

int CyGame::getNumHumanPlayers() const
{
	return m_pGame ? m_pGame->getNumHumanPlayers() : -1;
}

int CyGame::getGameTurn() const
{
	return m_pGame ? m_pGame->getGameTurn() : -1;
}

int CyGame::getTurnYear(int iGameTurn) const
{
	return  m_pGame ? m_pGame->getTurnYear(iGameTurn) : -1;
}

int CyGame::getGameTurnYear() const
{
	return  m_pGame ? m_pGame->getGameTurnYear() : -1;
}

int CyGame::getElapsedGameTurns() const
{
	return m_pGame ? m_pGame->getElapsedGameTurns() : -1;
}

int CyGame::getMaxTurns() const
{
	return m_pGame ? m_pGame->getMaxTurns() : -1;
}

int CyGame::getMaxCityElimination() const
{
	return m_pGame ? m_pGame->getMaxCityElimination() : -1;
}

int CyGame::getNumAdvancedStartPoints() const
{
	return m_pGame ? m_pGame->getNumAdvancedStartPoints() : -1;
}

int CyGame::getStartTurn() const
{
	return m_pGame ? m_pGame->getStartTurn() : -1;
}

int CyGame::getStartYear() const
{
	return m_pGame ? m_pGame->getStartYear() : -1;
}

void CyGame::setStartYear(int iNewValue)
{
	if (m_pGame) m_pGame->setStartYear(iNewValue);
}

int CyGame::getEstimateEndTurn() const
{
	return m_pGame ? m_pGame->getEstimateEndTurn() : -1;
}

int CyGame::getMinutesPlayed() const
{
	return m_pGame ? m_pGame->getMinutesPlayed() : 0;
}

int CyGame::getTargetScore() const
{
	return m_pGame ? m_pGame->getTargetScore() : -1;
}

int CyGame::getNumCities() const
{
	return m_pGame ? m_pGame->getNumCities() : -1;
}

int CyGame::getTotalPopulation() const
{
	return m_pGame ? m_pGame->getTotalPopulation() : -1;
}

int CyGame::getTradeRoutes() const
{
	return m_pGame ? m_pGame->getTradeRoutes() : -1;
}

void CyGame::changeNoNukesCount(int iChange)
{
	if (m_pGame) m_pGame->changeNoNukesCount(iChange);
}

int CyGame::getSecretaryGeneralTimer(int iVoteSource) const
{
	return m_pGame ? m_pGame->getSecretaryGeneralTimer((VoteSourceTypes)iVoteSource) : -1;
}

int CyGame::getVoteTimer(int iVoteSource) const
{
	return m_pGame ? m_pGame->getVoteTimer((VoteSourceTypes)iVoteSource) : -1;
}

int CyGame::getMaxPopulation() const
{
	return m_pGame ? m_pGame->getMaxPopulation() : 0;
}

int CyGame::getMaxLand() const
{
	return m_pGame ? m_pGame->getMaxLand() : 0;
}

int CyGame::getMaxTech() const
{
	return m_pGame ? m_pGame->getMaxTech() : 0;
}

int CyGame::getMaxWonders() const
{
	return m_pGame ? m_pGame->getMaxWonders() : 0;
}

int CyGame::getInitPopulation() const
{
	return m_pGame ? m_pGame->getInitPopulation() : 0;
}

int CyGame::getInitLand() const
{
	return m_pGame ? m_pGame->getInitLand() : 0;
}

int CyGame::getInitTech() const
{
	return m_pGame ? m_pGame->getInitTech() : 0;
}

int CyGame::getInitWonders() const
{
	return m_pGame ? m_pGame->getInitWonders() : 0;
}

int CyGame::getAIAutoPlay(int iPlayer) const
{
	return m_pGame ? m_pGame->getAIAutoPlay((PlayerTypes)iPlayer) : 0;
}

void CyGame::setAIAutoPlay(int iPlayer, int iNewValue)
{
	if (m_pGame) m_pGame->setAIAutoPlay((PlayerTypes)iPlayer, iNewValue);
}

bool CyGame::isForcedAIAutoPlay(int iPlayer) const
{
	return m_pGame ? m_pGame->isForcedAIAutoPlay((PlayerTypes)iPlayer) : false;
}

void CyGame::setForcedAIAutoPlay(int iPlayer, int iNewValue, bool bForced)
{
	if (m_pGame) m_pGame->setForcedAIAutoPlay((PlayerTypes)iPlayer, iNewValue, bForced);
}

bool CyGame::isDiploVote(int /*VoteSourceTypes*/ eVoteSource) const
{
	return m_pGame ? m_pGame->isDiploVote((VoteSourceTypes)eVoteSource) : false;
}

bool CyGame::isDebugMode() const
{
	return m_pGame ? m_pGame->isDebugMode() : false;
}

void CyGame::toggleDebugMode()
{
	if (m_pGame) m_pGame->toggleDebugMode();
}

int CyGame::getPitbossTurnTime() const
{
	return m_pGame ? m_pGame->getPitbossTurnTime() : -1;
}

bool CyGame::isHotSeat() const
{
	return m_pGame ? m_pGame->isHotSeat() : false;
}

bool CyGame::isPbem() const
{
	return m_pGame ? m_pGame->isPbem() : false;
}

bool CyGame::isPitboss() const
{
	return m_pGame ? m_pGame->isPitboss() : false;
}

bool CyGame::isFinalInitialized() const
{
	return m_pGame ? m_pGame->isFinalInitialized() : false;
}

int /*PlayerTypes*/ CyGame::getActivePlayer() const
{
	return m_pGame ? (int)m_pGame->getActivePlayer() : -1;
}

void CyGame::setActivePlayer(int /*PlayerTypes*/ eNewValue, bool bForceHotSeat)
{
	if (m_pGame) m_pGame->setActivePlayer((PlayerTypes)eNewValue, bForceHotSeat);
}

int CyGame::getPausePlayer() const
{
	return m_pGame ? m_pGame->getPausePlayer() : -1;
}

bool CyGame::isPaused() const
{
	return m_pGame ? m_pGame->isPaused() : false;
}

bool CyGame::getStarshipLaunched(int playaID) const
{
	return m_pGame ? m_pGame->getStarshipLaunched(playaID) : false;
}

int /*VictoryTypes*/ CyGame::getVictory() const
{
	return m_pGame ? (int)m_pGame->getVictory() : -1;
}

int /*GameStateTypes*/ CyGame::getGameState() const
{
	return m_pGame ? (int)m_pGame->getGameState() : -1;
}

int /* HandicapTypes */ CyGame::getHandicapType() const
{
	return m_pGame ? (int) m_pGame->getHandicapType() : (int) NO_HANDICAP;
}

CalendarTypes CyGame::getCalendar() const
{
	return m_pGame ? m_pGame->getCalendar() : CALENDAR_DEFAULT;
}

int /*EraTypes*/ CyGame::getStartEra() const
{
	return m_pGame ? m_pGame->getStartEra() : -1;
}

int /*GameSpeedTypes*/ CyGame::getGameSpeedType() const
{
	return m_pGame ? m_pGame->getGameSpeedType() : -1;
}

int /*PlayerTypes*/ CyGame::getRankPlayer(int iRank) const
{
	return m_pGame ? m_pGame->getRankPlayer(iRank) : -1;
}

int CyGame::getPlayerRank(int /*PlayerTypes*/ ePlayer) const
{
	return m_pGame ? m_pGame->getPlayerRank((PlayerTypes)ePlayer) : -1;
}

int CyGame::getPlayerScore(int /*PlayerTypes*/ ePlayer) const
{
	return m_pGame ? m_pGame->getPlayerScore((PlayerTypes)ePlayer) : -1;
}

int /*TeamTypes*/ CyGame::getRankTeam(int iRank) const
{
	return m_pGame ? m_pGame->getRankTeam(iRank) : -1;
}

int CyGame::getTeamScore(int /*TeamTypes*/ eTeam) const
{
	return m_pGame ? m_pGame->getTeamScore((TeamTypes)eTeam) : -1;
}

bool CyGame::isOption(int /*GameOptionTypes*/ eIndex) const
{
	return m_pGame ? m_pGame->isOption((GameOptionTypes)eIndex) : -1;
}

void CyGame::setOption(int /*GameOptionTypes*/ eIndex, bool bEnabled)
{
	if (m_pGame) m_pGame->setOption((GameOptionTypes)eIndex, bEnabled);
}

bool CyGame::isMPOption(int /*MultiplayerOptionTypes*/ eIndex) const
{
	return m_pGame ? m_pGame->isMPOption((MultiplayerOptionTypes)eIndex) : -1;
}

bool CyGame::isForcedControl(int /*ForceControlTypes*/ eIndex) const
{
	return m_pGame ? m_pGame->isForcedControl((ForceControlTypes)eIndex) : -1;
}

bool CyGame::isUnitMaxedOut(int /*UnitTypes*/ eIndex, int iExtra) const
{
	return m_pGame ? m_pGame->isUnitMaxedOut((UnitTypes)eIndex, iExtra) : -1;
}

bool CyGame::isBuildingMaxedOut(int /*BuildingTypes*/ eIndex, int iExtra) const
{
	return m_pGame ? m_pGame->isBuildingMaxedOut((BuildingTypes)eIndex, iExtra) : false;
}

int CyGame::getProjectCreatedCount(int /*ProjectTypes*/ eIndex) const
{
	return m_pGame ? m_pGame->getProjectCreatedCount((ProjectTypes) eIndex) : -1;
}

int CyGame::getReligionGameTurnFounded(int /*ReligionTypes*/ eIndex) const
{
	return m_pGame ? m_pGame->getReligionGameTurnFounded((ReligionTypes) eIndex) : -1;
}

bool CyGame::isReligionSlotTaken(int /*ReligionTypes*/ eIndex) const
{
	return m_pGame ? m_pGame->isReligionSlotTaken((ReligionTypes) eIndex) : false;
}

int CyGame::getCorporationGameTurnFounded(int /*CorporationTypes*/ eIndex) const
{
	return m_pGame ? m_pGame->getCorporationGameTurnFounded((CorporationTypes) eIndex) : -1;
}

bool CyGame::isCorporationFounded(int /*CorporationTypes*/ eIndex) const
{
	return m_pGame ? m_pGame->isCorporationFounded((CorporationTypes) eIndex) : false;
}

bool CyGame::isVotePassed(int /*VoteTypes*/ eIndex) const
{
	return m_pGame ? m_pGame->isVotePassed((VoteTypes)eIndex) : false;
}

bool CyGame::isVictoryValid(int /*VictoryTypes*/ eIndex) const
{
	return m_pGame ? m_pGame->isVictoryValid((VictoryTypes)eIndex) : false;
}

bool CyGame::isInAdvancedStart() const
{
	return m_pGame ? m_pGame->isInAdvancedStart() : false;
}

CyCity* CyGame::getHolyCity(int /*ReligionTypes*/ eIndex) const
{
	return m_pGame ? new CyCity(m_pGame->getHolyCity((ReligionTypes) eIndex)) : NULL;
}

void CyGame::setHolyCity(int /*ReligionTypes*/ eIndex, CyCity* pNewValue, bool bAnnounce)
{
	if (m_pGame) m_pGame->setHolyCity((ReligionTypes) eIndex, pNewValue->getCity(), bAnnounce);
}

void CyGame::clearHolyCity(int /*ReligionTypes*/ eIndex)
{
	if (m_pGame) m_pGame->setHolyCity((ReligionTypes) eIndex, NULL, false);
}

CyCity* CyGame::getHeadquarters(int /*CorporationTypes*/ eIndex) const
{
	return m_pGame ? new CyCity(m_pGame->getHeadquarters((CorporationTypes) eIndex)) : NULL;
}

void CyGame::setHeadquarters(int /*CorporationTypes*/ eIndex, CyCity* pNewValue, bool bAnnounce)
{
	if (m_pGame) m_pGame->setHeadquarters((CorporationTypes) eIndex, pNewValue->getCity(), bAnnounce);
}

void CyGame::clearHeadquarters(int /*CorporationTypes*/ eIndex)
{
	if (m_pGame) m_pGame->setHeadquarters((CorporationTypes) eIndex, NULL, false);
}

std::string CyGame::getScriptData() const
{
	return m_pGame ? m_pGame->getScriptData() : "";
}

void CyGame::setScriptData(std::string szNewValue)
{
	if (m_pGame) m_pGame->setScriptData(szNewValue);
}

void CyGame::setName(TCHAR* szNewValue)
{
	if (m_pGame) m_pGame->setName(szNewValue);
}

std::wstring CyGame::getName() const
{
	return m_pGame ? m_pGame->getName() : "";
}

int CyGame::getIndexAfterLastDeal() const
{
	return m_pGame ? m_pGame->getIndexAfterLastDeal() : -1;
}

int CyGame::getNumDeals() const
{
	return m_pGame ? m_pGame->getNumDeals() : -1;
}

CyDeal* CyGame::getDeal(int iID) const
{
	return m_pGame ? new CyDeal(m_pGame->getDeal(iID)) : NULL;
}

void CyGame::deleteDeal(int iID)
{
	if (m_pGame) m_pGame->deleteDeal(iID);
}

CvRandom& CyGame::getMapRand() const
{
	FAssert(m_pGame);
	return (m_pGame->getMapRand());
}

CvRandom& CyGame::getSorenRand() const
{
	FAssert(m_pGame);
	return (m_pGame->getSorenRand());
}

int CyGame::getSorenRandNum(int iNum, TCHAR* pszLog) const
{
	return m_pGame ? m_pGame->getSorenRandNum(iNum, pszLog) : -1;
}

bool CyGame::GetWorldBuilderMode() const				// remove once CvApp is exposed
{
	return gDLL->GetWorldBuilderMode();
}

bool CyGame::isPitbossHost() const				// remove once CvApp is exposed
{
	return gDLL->IsPitbossHost();
}

int CyGame::getCurrentLanguage() const				// remove once CvApp is exposed
{
	return gDLL->getCurrentLanguage();
}

void CyGame::setCurrentLanguage(int iNewLanguage)			// remove once CvApp is exposed
{
	gDLL->setCurrentLanguage(iNewLanguage);
}

int CyGame::getReplayMessageTurn(int i) const
{
	return m_pGame ? m_pGame->getReplayMessageTurn(i) : -1;
}

std::wstring CyGame::getReplayMessageText(int i) const
{
	return m_pGame ? m_pGame->getReplayMessageText(i) : L"";
}

uint CyGame::getNumReplayMessages() const
{
	return m_pGame ? m_pGame->getNumReplayMessages() : 0;
}

CyReplayInfo* CyGame::getReplayInfo() const
{
	return m_pGame ? new CyReplayInfo(m_pGame->getReplayInfo()) : NULL;
}

bool CyGame::hasSkippedSaveChecksum() const
{
	return m_pGame ? m_pGame->hasSkippedSaveChecksum() : false;
}

void CyGame::saveReplay(int iPlayer)
{
	if (m_pGame) m_pGame->saveReplay((PlayerTypes)iPlayer);
}

void CyGame::addPlayer(int eNewPlayer, int eLeader, int eCiv, bool bSetAlive)
{
	if (m_pGame) m_pGame->addPlayer((PlayerTypes)eNewPlayer, (LeaderHeadTypes)eLeader, (CivilizationTypes)eCiv, bSetAlive);
}

void CyGame::changeHumanPlayer( int eOldHuman, int eNewHuman )
{
	if (m_pGame) m_pGame->changeHumanPlayer((PlayerTypes)eOldHuman, (PlayerTypes)eNewHuman);
}

void CyGame::log(TCHAR* str)
{
	if (m_pGame) m_pGame->logMsg(str);
}

void CyGame::addReplayMessage(int /*ReplayMessageTypes*/ eType, int /*PlayerTypes*/ ePlayer, std::wstring pszText, int iPlotX, int iPlotY, int /*ColorTypes*/ eColor)
{
	if (m_pGame) m_pGame->addReplayMessage((ReplayMessageTypes)eType, (PlayerTypes)ePlayer, pszText, iPlotX, iPlotY, (ColorTypes)eColor);
}

void CyGame::drawBattleEffects()
{
	if (m_pGame) m_pGame->drawBattleEffects();
}

int CyGame::getCultureThreshold(int eLevel) const
{
	return m_pGame ? m_pGame->getCultureThreshold((CultureLevelTypes) eLevel) : -1;
}

void CyGame::setPlotExtraYield(int iX, int iY, int /*YieldTypes*/ eYield, int iExtraYield)
{
	if (m_pGame) m_pGame->setPlotExtraYield(iX, iY, (YieldTypes)eYield, iExtraYield);
}

bool CyGame::isCivEverActive(int /*CivilizationTypes*/ eCivilization) const
{
	return m_pGame ? m_pGame->isCivEverActive((CivilizationTypes)eCivilization) : false;
}

bool CyGame::isLeaderEverActive(int /*LeaderHeadTypes*/ eLeader) const
{
	return m_pGame ? m_pGame->isLeaderEverActive((LeaderHeadTypes)eLeader) : false;
}

bool CyGame::isEventActive(int /*EventTriggerTypes*/ eTrigger) const
{
	return m_pGame ? m_pGame->isEventActive((EventTriggerTypes)eTrigger) : false;
}

void CyGame::doControl(int iControl)
{
	if (m_pGame) m_pGame->doControl((ControlTypes) iControl);
}

void CyGame::saveGame(std::string fileName) const
{
	gDLL->getEngineIFace()->SaveGame((CvString &)fileName, SAVEGAME_NORMAL);
}

bool CyGame::cheatCodesEnabled() const
{
	return gDLL->getChtLvl() > 0;
}

void CyGame::setModderGameOption(int /*ModderGameOptionTypes*/ eIndex, int iNewValue)
{
	if (m_pGame) m_pGame->setModderGameOption((ModderGameOptionTypes)eIndex, iNewValue);
}

int CyGame::getXResolution() const
{
	return m_pGame ? m_pGame->getXResolution() : 0;
}

int CyGame::getYResolution() const
{
	return m_pGame ? m_pGame->getYResolution() : 0;
}

bool CyGame::canEverResearch(int iTech) const
{
	return m_pGame ? m_pGame->canEverResearch((TechTypes)iTech) : true;
}

bool CyGame::canEverSpread(int iCorporation) const
{
	return m_pGame ? m_pGame->canEverSpread((CorporationTypes)iCorporation) : true;
}

const char* CyGame::getC2CVersion() const
{
	return GC.getDefineSTRING("C2C_VERSION");
}
