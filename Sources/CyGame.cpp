#include "CvGameCoreDLL.h"
#include "CvCity.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvInitCore.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvDLLUtilityIFaceBase.h"
#include "CyCity.h"
#include "CyDeal.h"
#include "CyGame.h"
#include "CyPlot.h"
#include "CyReplayInfo.h"
#include "CvReplayInfo.h"

//
// Python wrapper class for CvGame
//

CyGame::CyGame() : m_pGame(GC.getGame()) {}

CyGame::CyGame(CvGame& pGame) : m_pGame(pGame) {}

CyGame::CyGame(CvGameAI& pGame) : m_pGame(pGame) {}

MapTypes CyGame::getCurrentMap() const
{
	return m_pGame.getCurrentMap();
}

bool CyGame::isMultiplayer() const
{
	return GC.getInitCore().getMultiplayer();
}

void CyGame::updateScore(bool bForce)
{
	m_pGame.updateScore(bForce);
}

void CyGame::selectionListMove(const CyPlot* pPlot, bool bAlt, bool bShift, bool bCtrl)
{
	GC.getGame().selectionListMove(pPlot->getPlot(), bAlt, bShift, bCtrl);
}

void CyGame::selectionListGameNetMessage(int eMessage, int iData2, int iData3, int iData4, int iFlags, bool bAlt, bool bShift)
{
	GC.getGame().selectionListGameNetMessage(eMessage, iData2, iData3, iData4, iFlags, bAlt, bShift);
}

void CyGame::selectedCitiesGameNetMessage(int eMessage, int iData2, int iData3, int iData4, bool bOption, bool bAlt, bool bShift, bool bCtrl)
{
	GC.getGame().selectedCitiesGameNetMessage(eMessage, iData2, iData3, iData4, bOption, bAlt, bShift, bCtrl);
}

void CyGame::cityPushOrder(const CyCity* pCity, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl)
{
	GC.getGame().cityPushOrder(pCity->getCity(), eOrder, iData, bAlt, bShift, bCtrl);
}

int CyGame::getSymbolID(int iSymbol) const
{
	return m_pGame.getSymbolID(iSymbol);
}

int CyGame::getProductionPerPopulation(HurryTypes eHurry) const
{
	return m_pGame.getProductionPerPopulation(eHurry);
}

int CyGame::getAdjustedPopulationPercent(VictoryTypes eVictory) const
{
	return m_pGame.getAdjustedPopulationPercent(eVictory);
}

int CyGame::getAdjustedLandPercent(VictoryTypes eVictory) const
{
	return m_pGame.getAdjustedLandPercent(eVictory);
}

bool CyGame::isTeamVote(VoteTypes eVote) const
{
	return m_pGame.isTeamVote(eVote);
}

bool CyGame::isChooseElection(VoteTypes eVote) const
{
	return m_pGame.isChooseElection(eVote);
}

bool CyGame::isTeamVoteEligible(TeamTypes eTeam, VoteSourceTypes eVoteSource) const
{
	return m_pGame.isTeamVoteEligible(eTeam, eVoteSource);
}

int CyGame::countPossibleVote(VoteTypes eVote, VoteSourceTypes eVoteSource) const
{
	return m_pGame.countPossibleVote(eVote, eVoteSource);
}

int CyGame::getVoteRequired(VoteTypes eVote, VoteSourceTypes eVoteSource) const
{
	return m_pGame.getVoteRequired(eVote, eVoteSource);
}

int CyGame::getSecretaryGeneral(VoteSourceTypes eVoteSource) const
{
	return m_pGame.getSecretaryGeneral(eVoteSource);
}

bool CyGame::canHaveSecretaryGeneral(VoteSourceTypes eVoteSource) const
{
	return m_pGame.canHaveSecretaryGeneral(eVoteSource);
}

int CyGame::getVoteSourceReligion(VoteSourceTypes eVoteSource) const
{
	return m_pGame.getVoteSourceReligion(eVoteSource);
}

int CyGame::countCivPlayersAlive() const
{
	return m_pGame.countCivPlayersAlive();
}

int CyGame::countCivPlayersEverAlive() const
{
	return m_pGame.countCivPlayersEverAlive();
}

int CyGame::countCivTeamsAlive() const
{
	return m_pGame.countCivTeamsAlive();
}

int CyGame::countCivTeamsEverAlive() const
{
	return m_pGame.countCivTeamsEverAlive();
}

int CyGame::countTotalCivPower() const
{
	return m_pGame.countTotalCivPower();
}

int CyGame::countTotalNukeUnits() const
{
	return m_pGame.countTotalNukeUnits();
}

int CyGame::countKnownTechNumTeams(TechTypes eTech) const
{
	return m_pGame.countKnownTechNumTeams(eTech);
}

int CyGame::countReligionLevels(ReligionTypes eReligion) const
{
	return m_pGame.countReligionLevels(eReligion);
}

int CyGame::countCorporationLevels(CorporationTypes eCorporation) const
{
	return m_pGame.countCorporationLevels(eCorporation);
}

int CyGame::calculateReligionPercent(ReligionTypes eReligion) const
{
	return m_pGame.calculateReligionPercent(eReligion);
}

int CyGame::goldenAgeLength() const
{
	return m_pGame.goldenAgeLength();
}

int CyGame::victoryDelay(VictoryTypes iVictory) const
{
	return m_pGame.victoryDelay(iVictory);
}

int CyGame::getImprovementUpgradeTime(ImprovementTypes eImprovement) const
{
	return m_pGame.getImprovementUpgradeTime(eImprovement);
}

bool CyGame::canTrainNukes() const
{
	return m_pGame.canTrainNukes();
}

EraTypes CyGame::getHighestEra() const
{
	return m_pGame.getHighestEra();
}

EraTypes CyGame::getCurrentEra() const
{
	return m_pGame.getCurrentEra();
}

int CyGame::getActiveTeam() const
{
	return m_pGame.getActiveTeam();
}

CivilizationTypes CyGame::getActiveCivilizationType() const
{
	return m_pGame.getActiveCivilizationType();
}

bool CyGame::isNetworkMultiPlayer() const
{
	return m_pGame.isNetworkMultiPlayer();
}

bool CyGame::isGameMultiPlayer() const
{
	return m_pGame.isGameMultiPlayer();
}

bool CyGame::isTeamGame() const
{
	return m_pGame.isTeamGame();
}

bool CyGame::isModem() const
{
	return m_pGame.isModem();
}

void CyGame::setModem(bool bModem)
{
	m_pGame.setModem(bModem);
}

int CyGame::getNumHumanPlayers() const
{
	return m_pGame.getNumHumanPlayers();
}

int CyGame::getGameTurn() const
{
	return m_pGame.getGameTurn();
}

void CyGame::setGameTurn(int iNewValue)
{
	m_pGame.setGameTurn(iNewValue);
}

int CyGame::getTurnYear(int iGameTurn) const
{
	return m_pGame.getTurnYear(iGameTurn);
}

int CyGame::getGameTurnYear() const
{
	return m_pGame.getGameTurnYear();
}

int CyGame::getElapsedGameTurns() const
{
	return m_pGame.getElapsedGameTurns();
}

int CyGame::getMaxTurns() const
{
	return m_pGame.getMaxTurns();
}

void CyGame::setMaxTurns(int iNewValue)
{
	m_pGame.setMaxTurns(iNewValue);
}

void CyGame::changeMaxTurns(int iChange)
{
	m_pGame.changeMaxTurns(iChange);
}

int CyGame::getMaxCityElimination() const
{
	return m_pGame.getMaxCityElimination();
}

void CyGame::setMaxCityElimination(int iNewValue)
{
	m_pGame.setMaxCityElimination(iNewValue);
}

int CyGame::getNumAdvancedStartPoints() const
{
	return m_pGame.getNumAdvancedStartPoints();
}

void CyGame::setNumAdvancedStartPoints(int iNewValue)
{
	m_pGame.setNumAdvancedStartPoints(iNewValue);
}

int CyGame::getStartTurn() const
{
	return m_pGame.getStartTurn();
}

int CyGame::getStartYear() const
{
	return m_pGame.getStartYear();
}

void CyGame::setStartYear(int iNewValue)
{
	m_pGame.setStartYear(iNewValue);
}

int CyGame::getEstimateEndTurn() const
{
	return m_pGame.getEstimateEndTurn();
}

void CyGame::setEstimateEndTurn(int iNewValue)
{
	m_pGame.setEstimateEndTurn(iNewValue);
}

int CyGame::getTurnSlice() const
{
	return m_pGame.getTurnSlice();
}

int CyGame::getMinutesPlayed() const
{
	return m_pGame.getMinutesPlayed();
}

int CyGame::getTargetScore() const
{
	return m_pGame.getTargetScore();
}

void CyGame::setTargetScore(int iNewValue)
{
	m_pGame.setTargetScore(iNewValue);
}

int CyGame::getNumGameTurnActive() const
{
	return m_pGame.getNumGameTurnActive();
}

int CyGame::countNumHumanGameTurnActive() const
{
	return m_pGame.countNumHumanGameTurnActive();
}

int CyGame::getNumCities() const
{
	return m_pGame.getNumCities();
}

int CyGame::getNumCivCities() const
{
	return m_pGame.getNumCivCities();
}

int CyGame::getTotalPopulation() const
{
	return m_pGame.getTotalPopulation();
}

int CyGame::getTradeRoutes() const
{
	return m_pGame.getTradeRoutes();
}

void CyGame::changeTradeRoutes(int iChange)
{
	m_pGame.changeTradeRoutes(iChange);
}

int CyGame::getFreeTradeCount() const
{
	return m_pGame.getFreeTradeCount();
}

bool CyGame::isFreeTrade() const
{
	return m_pGame.isFreeTrade();
}

void CyGame::changeFreeTradeCount(int iChange)
{
	m_pGame.changeFreeTradeCount(iChange);
}

int CyGame::getNoNukesCount() const
{
	return m_pGame.getNoNukesCount();
}

bool CyGame::isNoNukes() const
{
	return m_pGame.isNoNukes();
}

void CyGame::changeNoNukesCount(int iChange)
{
	m_pGame.changeNoNukesCount(iChange);
}

int CyGame::getSecretaryGeneralTimer(int iVoteSource) const
{
	return m_pGame.getSecretaryGeneralTimer((VoteSourceTypes)iVoteSource);
}

int CyGame::getVoteTimer(int iVoteSource) const
{
	return m_pGame.getVoteTimer((VoteSourceTypes)iVoteSource);
}

int CyGame::getNukesExploded() const
{
	return m_pGame.getNukesExploded();
}

void CyGame::changeNukesExploded(int iChange)
{
	m_pGame.changeNukesExploded(iChange);
}

int CyGame::getMaxPopulation() const
{
	return m_pGame.getMaxPopulation();
}

int CyGame::getMaxLand() const
{
	return m_pGame.getMaxLand();
}

int CyGame::getMaxTech() const
{
	return m_pGame.getMaxTech();
}

int CyGame::getMaxWonders() const
{
	return m_pGame.getMaxWonders();
}

int CyGame::getInitPopulation() const
{
	return m_pGame.getInitPopulation();
}

int CyGame::getInitLand() const
{
	return m_pGame.getInitLand();
}

int CyGame::getInitTech() const
{
	return m_pGame.getInitTech();
}

int CyGame::getInitWonders() const
{
	return m_pGame.getInitWonders();
}

int CyGame::getAIAutoPlay(int iPlayer) const
{
	return m_pGame.getAIAutoPlay((PlayerTypes)iPlayer);
}

void CyGame::setAIAutoPlay(int iPlayer, int iNewValue)
{
	m_pGame.setAIAutoPlay((PlayerTypes)iPlayer, iNewValue);
}

bool CyGame::isForcedAIAutoPlay(int iPlayer) const
{
	return m_pGame.isForcedAIAutoPlay((PlayerTypes)iPlayer);
}

int CyGame::getForcedAIAutoPlay(int iPlayer) const
{
	return m_pGame.getForcedAIAutoPlay((PlayerTypes)iPlayer);
}

void CyGame::setForcedAIAutoPlay(int iPlayer, int iNewValue, bool bForced)
{
	m_pGame.setForcedAIAutoPlay((PlayerTypes)iPlayer, iNewValue, bForced);
}

bool CyGame::isScoreDirty() const
{
	return m_pGame.isScoreDirty();
}

void CyGame::setScoreDirty(bool bNewValue)
{
	m_pGame.setScoreDirty(bNewValue);
}

int CyGame::getCircumnavigatedTeam() const
{
	return m_pGame.getCircumnavigatedTeam();
}

void CyGame::setCircumnavigatedTeam(int iTeamType)
{
	m_pGame.setCircumnavigatedTeam((TeamTypes) iTeamType);
}

bool CyGame::isDiploVote(VoteSourceTypes eVoteSource) const
{
	return m_pGame.isDiploVote((VoteSourceTypes)eVoteSource);
}

void CyGame::changeDiploVote(VoteSourceTypes eVoteSource, int iChange)
{
	m_pGame.changeDiploVote((VoteSourceTypes)eVoteSource, iChange);
}

bool CyGame::isDebugMode() const
{
	return m_pGame.isDebugMode();
}

void CyGame::toggleDebugMode()
{
	m_pGame.toggleDebugMode();
}

int CyGame::getChtLvl() const
{
	return gDLL->getChtLvl();
}

int CyGame::getPitbossTurnTime() const
{
	return m_pGame.getPitbossTurnTime();
}

void CyGame::setPitbossTurnTime(int iHours)
{
	m_pGame.setPitbossTurnTime(iHours);
}

bool CyGame::isHotSeat() const
{
	return m_pGame.isHotSeat();
}

bool CyGame::isPbem() const
{
	return m_pGame.isPbem();
}

bool CyGame::isPitboss() const
{
	return m_pGame.isPitboss();
}

bool CyGame::isSimultaneousTeamTurns() const
{
	return m_pGame.isSimultaneousTeamTurns();
}

bool CyGame::isFinalInitialized() const
{
	return m_pGame.isFinalInitialized();
}

PlayerTypes CyGame::getActivePlayer() const
{
	return m_pGame.getActivePlayer();
}

void CyGame::setActivePlayer(PlayerTypes eNewValue, bool bForceHotSeat)
{
	m_pGame.setActivePlayer(eNewValue, bForceHotSeat);
}

int CyGame::getPausePlayer() const
{
	return m_pGame.getPausePlayer();
}

bool CyGame::isPaused() const
{
	return m_pGame.isPaused();
}

UnitTypes CyGame::getBestLandUnit() const
{
	return m_pGame.getBestLandUnit();
}

int CyGame::getBestLandUnitCombat() const
{
	return m_pGame.getBestLandUnitCombat();
}

bool CyGame::getStarshipLaunched(int playaID) const
{
	return m_pGame.getStarshipLaunched(playaID);
}

bool CyGame::getDiplomaticVictoryAchieved(int playaID) const
{
	return m_pGame.getDiplomaticVictoryAchieved(playaID);
}

TeamTypes CyGame::getWinner() const
{
	return m_pGame.getWinner();
}

VictoryTypes CyGame::getVictory() const
{
	return m_pGame.getVictory();
}

void CyGame::setWinner(TeamTypes eNewWinner, VictoryTypes eNewVictory)
{
	m_pGame.setWinner(eNewWinner, eNewVictory);
}

GameStateTypes CyGame::getGameState() const
{
	return m_pGame.getGameState();
}

HandicapTypes CyGame::getHandicapType() const
{
	return m_pGame.getHandicapType();
}

CalendarTypes CyGame::getCalendar() const
{
	return m_pGame.getCalendar();
}

EraTypes CyGame::getStartEra() const
{
	return m_pGame.getStartEra();
}

GameSpeedTypes CyGame::getGameSpeedType() const
{
	return m_pGame.getGameSpeedType();
}

PlayerTypes CyGame::getRankPlayer(int iRank) const
{
	return m_pGame.getRankPlayer(iRank);
}

int CyGame::getPlayerRank(PlayerTypes ePlayer) const
{
	return m_pGame.getPlayerRank(ePlayer);
}

int CyGame::getPlayerScore(PlayerTypes ePlayer) const
{
	return m_pGame.getPlayerScore(ePlayer);
}

TeamTypes CyGame::getRankTeam(int iRank) const
{
	return m_pGame.getRankTeam(iRank);
}

int CyGame::getTeamRank(TeamTypes eTeam) const
{
	return m_pGame.getTeamRank(eTeam);
}

int CyGame::getTeamScore(TeamTypes eTeam) const
{
	return m_pGame.getTeamScore(eTeam);
}

bool CyGame::isOption(GameOptionTypes eIndex) const
{
	return m_pGame.isOption(eIndex);
}

void CyGame::setOption(GameOptionTypes eIndex, bool bEnabled)
{
	m_pGame.setOption(eIndex, bEnabled);
	if (bEnabled)
		m_pGame.enforceOptionCompatibility(eIndex);
}

bool CyGame::isMPOption(MultiplayerOptionTypes eIndex) const
{
	return m_pGame.isMPOption(eIndex);
}

bool CyGame::isForcedControl(ForceControlTypes eIndex) const
{
	return m_pGame.isForcedControl(eIndex);
}

int CyGame::getUnitCreatedCount(UnitTypes eIndex) const
{
	return m_pGame.getUnitCreatedCount(eIndex);
}

bool CyGame::isUnitMaxedOut(UnitTypes eIndex, int iExtra) const
{
	return m_pGame.isUnitMaxedOut(eIndex, iExtra);
}

int CyGame::getBuildingCreatedCount(BuildingTypes eIndex) const
{
	return m_pGame.getBuildingCreatedCount(eIndex);
}

bool CyGame::isBuildingMaxedOut(BuildingTypes eIndex, int iExtra) const
{
	return m_pGame.isBuildingMaxedOut(eIndex, iExtra);
}

int CyGame::getProjectCreatedCount(ProjectTypes eIndex) const
{
	return m_pGame.getProjectCreatedCount(eIndex);
}

bool CyGame::isProjectMaxedOut(ProjectTypes eIndex, int iExtra) const
{
	return m_pGame.isProjectMaxedOut(eIndex, iExtra);
}

int CyGame::getForceCivicCount(CivicTypes eIndex) const
{
	return m_pGame.getForceCivicCount(eIndex);
}

bool CyGame::isForceCivic(CivicTypes eIndex) const
{
	return m_pGame.isForceCivic(eIndex);
}

bool CyGame::isForceCivicOption(CivicOptionTypes eCivicOption) const
{
	return m_pGame.isForceCivicOption(eCivicOption);
}

int CyGame::getVoteOutcome(VoteTypes eIndex) const
{
	return m_pGame.getVoteOutcome(eIndex);
}

int CyGame::getReligionGameTurnFounded(ReligionTypes eIndex) const
{
	return m_pGame.getReligionGameTurnFounded(eIndex);
}

bool CyGame::isReligionFounded(ReligionTypes eIndex) const
{
	return m_pGame.isReligionFounded(eIndex);
}

bool CyGame::isReligionSlotTaken(ReligionTypes eIndex) const
{
	return m_pGame.isReligionSlotTaken(eIndex);
}

bool CyGame::isGameStart() const
{
	return m_pGame.isGameStart();
}

int CyGame::countNumReligionsFounded() const
{
	return m_pGame.countNumReligionsFounded();
}

int CyGame::countNumReligionTechsDiscovered() const
{
	return m_pGame.countNumReligionTechsDiscovered();
}

bool CyGame::isTechCanFoundReligion(TechTypes eIndex) const
{
	return m_pGame.isTechCanFoundReligion(eIndex);
}

int CyGame::getCorporationGameTurnFounded(CorporationTypes eIndex) const
{
	return m_pGame.getCorporationGameTurnFounded(eIndex);
}

bool CyGame::isCorporationFounded(CorporationTypes eIndex) const
{
	return m_pGame.isCorporationFounded(eIndex);
}

bool CyGame::isVotePassed(VoteTypes eIndex) const
{
	return m_pGame.isVotePassed(eIndex);
}

bool CyGame::isVictoryValid(VictoryTypes eIndex) const
{
	return m_pGame.isVictoryValid(eIndex);
}

bool CyGame::isSpecialUnitValid(SpecialUnitTypes eSpecialUnitType) const
{
	return m_pGame.isSpecialUnitValid(eSpecialUnitType);
}

void CyGame::makeSpecialUnitValid(SpecialUnitTypes eSpecialUnitType)
{
	m_pGame.makeSpecialUnitValid(eSpecialUnitType);
}

bool CyGame::isSpecialBuildingValid(SpecialBuildingTypes eIndex) const
{
	return m_pGame.isSpecialBuildingValid(eIndex);
}

void CyGame::makeSpecialBuildingValid(SpecialBuildingTypes eIndex)
{
	m_pGame.makeSpecialBuildingValid(eIndex);
}

bool CyGame::isInAdvancedStart() const
{
	return m_pGame.isInAdvancedStart();
}

CyCity* CyGame::getHolyCity(ReligionTypes eIndex) const
{
	CvCity* city = m_pGame.getHolyCity(eIndex);
	return city ? new CyCity(city) : NULL;
}

void CyGame::setHolyCity(ReligionTypes eIndex, CyCity* pNewValue, bool bAnnounce)
{
	m_pGame.setHolyCity(eIndex, pNewValue->getCity(), bAnnounce);
}

void CyGame::clearHolyCity(ReligionTypes eIndex)
{
	m_pGame.setHolyCity(eIndex, NULL, false);
}

CyCity* CyGame::getHeadquarters(CorporationTypes eIndex) const
{
	CvCity* city = m_pGame.getHeadquarters(eIndex);
	return city ? new CyCity(city) : NULL;
}

void CyGame::setHeadquarters(CorporationTypes eIndex, CyCity* pNewValue, bool bAnnounce)
{
	m_pGame.setHeadquarters(eIndex, pNewValue->getCity(), bAnnounce);
}

void CyGame::clearHeadquarters(CorporationTypes eIndex)
{
	m_pGame.setHeadquarters(eIndex, NULL, false);
}

int CyGame::getPlayerVote(PlayerTypes eOwnerIndex, int iVoteId) const
{
	return m_pGame.getPlayerVote(eOwnerIndex, iVoteId);
}

std::string CyGame::getScriptData() const
{
	return m_pGame.getScriptData();
}

void CyGame::setScriptData(std::string szNewValue)
{
	m_pGame.setScriptData(szNewValue);
}

void CyGame::setName(const char* szNewValue)
{
	m_pGame.setName(szNewValue);
}

std::wstring CyGame::getName() const
{
	return m_pGame.getName();
}

int CyGame::getIndexAfterLastDeal() const
{
	return m_pGame.getIndexAfterLastDeal();
}

int CyGame::getNumDeals() const
{
	return m_pGame.getNumDeals();
}

CyDeal* CyGame::getDeal(int iID) const
{
	return new CyDeal(m_pGame.getDeal(iID));
}

CyDeal* CyGame::addDeal()
{
	return new CyDeal(m_pGame.addDeal());
}

void CyGame::deleteDeal(int iID)
{
	m_pGame.deleteDeal(iID);
}

CvRandom& CyGame::getMapRand() const
{
	return (m_pGame.getMapRand());
}

int CyGame::getMapRandNum(int iNum, const char* pszLog) const
{
	return m_pGame.getMapRandNum(iNum, pszLog);
}

CvRandom& CyGame::getSorenRand() const
{
	return (m_pGame.getSorenRand());
}

int CyGame::getSorenRandNum(int iNum, const char* pszLog) const
{
	return m_pGame.getSorenRandNum(iNum, pszLog);
}

int CyGame::calculateSyncChecksum() const
{
	return m_pGame.calculateSyncChecksum();
}

int CyGame::calculateOptionsChecksum() const
{
	return m_pGame.calculateOptionsChecksum();
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
	return m_pGame.getReplayMessageTurn(i);
}

ReplayMessageTypes CyGame::getReplayMessageType(int i) const
{
	return m_pGame.getReplayMessageType(i);
}

int CyGame::getReplayMessagePlotX(int i) const
{
	return m_pGame.getReplayMessagePlotX(i);
}

int CyGame::getReplayMessagePlotY(int i) const
{
	return m_pGame.getReplayMessagePlotY(i);
}

int CyGame::getReplayMessagePlayer(int i) const
{
	return m_pGame.getReplayMessagePlayer(i);
}

ColorTypes CyGame::getReplayMessageColor(int i) const
{
	return m_pGame.getReplayMessageColor(i);
}

std::wstring CyGame::getReplayMessageText(int i) const
{
	return m_pGame.getReplayMessageText(i);
}

uint32_t CyGame::getNumReplayMessages() const
{
	return m_pGame.getNumReplayMessages();
}

CyReplayInfo* CyGame::getReplayInfo() const
{
	return new CyReplayInfo(m_pGame.getReplayInfo());
}

void CyGame::saveReplay(PlayerTypes ePlayer)
{
	m_pGame.saveReplay(ePlayer);
}

void CyGame::addReplayMessage(ReplayMessageTypes eType, PlayerTypes ePlayer, std::wstring pszText, int iPlotX, int iPlotY, ColorTypes eColor)
{
	m_pGame.addReplayMessage(eType, ePlayer, pszText, iPlotX, iPlotY, eColor);
}

bool CyGame::hasSkippedSaveChecksum() const
{
	return gDLL->hasSkippedSaveChecksum();
}

void CyGame::addPlayer(PlayerTypes eNewPlayer, LeaderHeadTypes eLeader, CivilizationTypes eCiv, bool bSetAlive)
{
	m_pGame.addPlayer(eNewPlayer, eLeader, eCiv, bSetAlive);
}

void CyGame::changeHumanPlayer(PlayerTypes eOldHuman, PlayerTypes eNewHuman)
{
	m_pGame.changeHumanPlayer(eOldHuman, eNewHuman);
}

void CyGame::logw(std::wstring str)
{
	logging::logMsgW("C2C.log", (wchar_t*)(CvString(str) + "\n").c_str());
}

void CyGame::log(const char* file, char* str)
{
	gDLL->logMsg(file, str, false, false);
#ifdef _DEBUG
	strcat(str, "\n");
	OutputDebugString(str);
#endif
}

int CyGame::getCultureThreshold(CultureLevelTypes eLevel) const
{
	return m_pGame.getCultureThreshold(eLevel);
}

void CyGame::setPlotExtraYield(int iX, int iY, YieldTypes eYield, int iExtraYield)
{
	m_pGame.setPlotExtraYield(iX, iY, eYield, iExtraYield);
}

bool CyGame::isCivEverActive(CivilizationTypes eCivilization) const
{
	return m_pGame.isCivEverActive(eCivilization);
}

bool CyGame::isLeaderEverActive(LeaderHeadTypes eLeader) const
{
	return m_pGame.isLeaderEverActive(eLeader);
}

bool CyGame::isEventActive(EventTriggerTypes eTrigger) const
{
	return m_pGame.isEventActive(eTrigger);
}

void CyGame::doControl(ControlTypes iControl)
{
	m_pGame.doControl(iControl);
}

bool CyGame::canRegenerateMap() const
{
	return m_pGame.canRegenerateMap();
}

bool CyGame::regenerateMap()
{
	if (canRegenerateMap())
	{
		m_pGame.regenerateMap();
		return true;
	}
	return false;
}

void CyGame::saveGame(std::string fileName) const
{
	gDLL->getEngineIFace()->SaveGame((CvString &)fileName, SAVEGAME_NORMAL);
}

std::string CyGame::getDLLPath() const
{
	return GC.getInitCore().getDLLPath();
}

std::string CyGame::getExePath() const
{
	return GC.getInitCore().getExePath();
}

int CyGame::getCutLosersCounter() const
{
	return m_pGame.getCutLosersCounter();
}

int CyGame::getHighToLowCounter() const
{
	return m_pGame.getHighToLowCounter();
}

bool CyGame::isModderGameOption(ModderGameOptionTypes eIndex) const
{
	return m_pGame.isModderGameOption(eIndex);
}

int CyGame::getModderGameOption(ModderGameOptionTypes eIndex) const
{
	return m_pGame.getModderGameOption(eIndex);
}

void CyGame::setModderGameOption(ModderGameOptionTypes eIndex, int iNewValue)
{
	m_pGame.setModderGameOption(eIndex, iNewValue);
}

bool CyGame::canEverResearch(TechTypes iTech) const
{
	return m_pGame.canEverResearch(iTech);
}

bool CyGame::canEverConstruct(BuildingTypes iBuilding) const
{
	return m_pGame.canEverConstruct(iBuilding);
}

bool CyGame::canEverTrain(UnitTypes iUnit) const
{
	return m_pGame.canEverTrain(iUnit);
}

bool CyGame::canEverSpread(CorporationTypes iCorporation) const
{
	return m_pGame.canEverSpread(iCorporation);
}

const char* CyGame::getC2CVersion() const
{
	return GC.getDefineSTRING("C2C_VERSION");
}

void CyGame::assignStartingPlots(bool bScenario, bool bMapScript)
{
	m_pGame.assignStartingPlots(bScenario, bMapScript);
}