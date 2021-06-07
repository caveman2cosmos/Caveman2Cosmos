//
// Python wrapper class for CvGame
//

#include "CvGameCoreDLL.h"
#include "CvCity.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvInitCore.h"
#include "CyCity.h"
#include "CyDeal.h"
#include "CyGame.h"
#include "CyPlot.h"
#include "CyReplayInfo.h"
#include "CvReplayInfo.h"

CyGame::CyGame() : m_pGame(GC.getGame()) {}

CyGame::CyGame(CvGame& pGame) : m_pGame(pGame) {}

CyGame::CyGame(CvGameAI& pGame) : m_pGame(pGame) {}

int CyGame::getCurrentMap() const
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

void CyGame::cycleCities(bool bForward, bool bAdd)
{
	m_pGame.cycleCities(bForward, bAdd);
}

void CyGame::cycleSelectionGroups(bool bClear, bool bForward, bool bWorkers)
{
	m_pGame.cycleSelectionGroups(bClear, bForward, bWorkers);
}

bool CyGame::cyclePlotUnits(const CyPlot* pPlot, bool bForward, bool bAuto, int iCount)
{
	return m_pGame.cyclePlotUnits(pPlot->getPlot(), bForward, bAuto, iCount);
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

int CyGame::getProductionPerPopulation(int /*HurryTypes*/ eHurry) const
{
	return m_pGame.getProductionPerPopulation((HurryTypes) eHurry);
}

int CyGame::getAdjustedPopulationPercent(int /*VictoryTypes*/ eVictory) const
{
	return m_pGame.getAdjustedPopulationPercent((VictoryTypes) eVictory);
}

int CyGame::getAdjustedLandPercent(int /*VictoryTypes*/ eVictory) const
{
	return m_pGame.getAdjustedLandPercent((VictoryTypes) eVictory);
}

bool CyGame::isTeamVote(int /*VoteTypes*/ eVote) const
{
	return m_pGame.isTeamVote((VoteTypes) eVote);
}

bool CyGame::isChooseElection(int /*VoteTypes*/ eVote) const
{
	return m_pGame.isChooseElection((VoteTypes) eVote);
}

bool CyGame::isTeamVoteEligible(int /*TeamTypes*/ eTeam, int /*VoteSourceTypes*/ eVoteSource) const
{
	return m_pGame.isTeamVoteEligible((TeamTypes) eTeam, (VoteSourceTypes)eVoteSource);
}

int CyGame::countPossibleVote(int /*VoteTypes*/ eVote, int /*VoteSourceTypes*/ eVoteSource) const
{
	return m_pGame.countPossibleVote((VoteTypes) eVote, (VoteSourceTypes)eVoteSource);
}

int CyGame::getVoteRequired(int /*VoteTypes*/ eVote, int /*VoteSourceTypes*/ eVoteSource) const
{
	return m_pGame.getVoteRequired((VoteTypes)eVote, (VoteSourceTypes) eVoteSource);
}

int CyGame::getSecretaryGeneral(int /*VoteSourceTypes*/ eVoteSource) const
{
	return m_pGame.getSecretaryGeneral((VoteSourceTypes) eVoteSource);
}

bool CyGame::canHaveSecretaryGeneral(int /*VoteSourceTypes*/ eVoteSource) const
{
	return m_pGame.canHaveSecretaryGeneral((VoteSourceTypes) eVoteSource);
}

int CyGame::getVoteSourceReligion(int /*VoteSourceTypes*/ eVoteSource) const
{
	return m_pGame.getVoteSourceReligion((VoteSourceTypes) eVoteSource);
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

int CyGame::countKnownTechNumTeams(int /*TechTypes*/ eTech) const
{
	return m_pGame.countKnownTechNumTeams((TechTypes) eTech);
}

int CyGame::getNumFreeBonuses(int /*BuildingTypes*/ eBuilding) const
{
	return m_pGame.getNumFreeBonuses((BuildingTypes) eBuilding);
}

int CyGame::countReligionLevels(int /*ReligionTypes*/ eReligion) const
{
	return m_pGame.countReligionLevels((ReligionTypes) eReligion);
}

int CyGame::countCorporationLevels(int /*CorporationTypes*/ eCorporation) const
{
	return m_pGame.countCorporationLevels((CorporationTypes) eCorporation);
}

int CyGame::calculateReligionPercent(int /*ReligionTypes*/ eReligion) const
{
	return m_pGame.calculateReligionPercent((ReligionTypes) eReligion);
}

int CyGame::goldenAgeLength() const
{
	return m_pGame.goldenAgeLength();
}

int CyGame::victoryDelay(int iVictory) const
{
	return m_pGame.victoryDelay((VictoryTypes)iVictory);
}

int CyGame::getImprovementUpgradeTime(int /*ImprovementTypes*/ eImprovement) const
{
	return m_pGame.getImprovementUpgradeTime((ImprovementTypes) eImprovement);
}

bool CyGame::canTrainNukes() const
{
	return m_pGame.canTrainNukes();
}

int CyGame::getHighestEra() const
{
	return m_pGame.getHighestEra();
}

int CyGame::getCurrentEra() const
{
	return m_pGame.getCurrentEra();
}

int CyGame::getActiveTeam() const
{
	return m_pGame.getActiveTeam();
}

int /*CivilizationTypes*/ CyGame::getActiveCivilizationType() const
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

void CyGame::reviveActivePlayer()
{
	m_pGame.reviveActivePlayer();
}

void CyGame::revivePlayer(int /*PlayerTypes*/ iPlayer)
{
	m_pGame.reviveActivePlayer((PlayerTypes)iPlayer);
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

CyPlot* CyGame::getLastNukeStrikePlot() const
{
	return new CyPlot(m_pGame.getLastNukeStrikePlot());
}

void CyGame::setLastNukeStrikePlot(CyPlot* pPlot)
{
	m_pGame.setLastNukeStrikePlot(pPlot->getPlot());
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

bool CyGame::isDiploVote(int /*VoteSourceTypes*/ eVoteSource) const
{
	return m_pGame.isDiploVote((VoteSourceTypes)eVoteSource);
}

void CyGame::changeDiploVote(int /*VoteSourceTypes*/ eVoteSource, int iChange)
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

int /*PlayerTypes*/ CyGame::getActivePlayer() const
{
	return m_pGame.getActivePlayer();
}

void CyGame::setActivePlayer(int /*PlayerTypes*/ eNewValue, bool bForceHotSeat)
{
	m_pGame.setActivePlayer((PlayerTypes)eNewValue, bForceHotSeat);
}

int CyGame::getPausePlayer() const
{
	return m_pGame.getPausePlayer();
}

bool CyGame::isPaused() const
{
	return m_pGame.isPaused();
}

int /*PlayerTypes*/ CyGame::getBestLandUnit() const
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

int /*TeamTypes*/ CyGame::getWinner() const
{
	return m_pGame.getWinner();
}

int /*VictoryTypes*/ CyGame::getVictory() const
{
	return m_pGame.getVictory();
}

void CyGame::setWinner(int /*TeamTypes*/ eNewWinner, int /*VictoryTypes*/ eNewVictory)
{
	m_pGame.setWinner((TeamTypes) eNewWinner, (VictoryTypes) eNewVictory);
}

int /*GameStateTypes*/ CyGame::getGameState() const
{
	return m_pGame.getGameState();
}

int /*HandicapTypes*/ CyGame::getHandicapType() const
{
	return m_pGame.getHandicapType();
}

CalendarTypes CyGame::getCalendar() const
{
	return m_pGame.getCalendar();
}

int /*EraTypes*/ CyGame::getStartEra() const
{
	return m_pGame.getStartEra();
}

int /*GameSpeedTypes*/ CyGame::getGameSpeedType() const
{
	return m_pGame.getGameSpeedType();
}

int /*PlayerTypes*/ CyGame::getRankPlayer(int iRank) const
{
	return m_pGame.getRankPlayer(iRank);
}

int CyGame::getPlayerRank(int /*PlayerTypes*/ ePlayer) const
{
	return m_pGame.getPlayerRank((PlayerTypes)ePlayer);
}

int CyGame::getPlayerScore(int /*PlayerTypes*/ ePlayer) const
{
	return m_pGame.getPlayerScore((PlayerTypes)ePlayer);
}

int /*TeamTypes*/ CyGame::getRankTeam(int iRank) const
{
	return m_pGame.getRankTeam(iRank);
}

int CyGame::getTeamRank(int /*TeamTypes*/ eTeam) const
{
	return m_pGame.getTeamRank((TeamTypes)eTeam);
}

int CyGame::getTeamScore(int /*TeamTypes*/ eTeam) const
{
	return m_pGame.getTeamScore((TeamTypes)eTeam);
}

bool CyGame::isOption(int /*GameOptionTypes*/ eIndex) const
{
	return m_pGame.isOption((GameOptionTypes)eIndex);
}

void CyGame::setOption(int /*GameOptionTypes*/ eIndex, bool bEnabled)
{
	m_pGame.setOption((GameOptionTypes)eIndex, bEnabled);
	if (bEnabled)
		m_pGame.enforceOptionCompatibility((GameOptionTypes)eIndex);
}

bool CyGame::isMPOption(int /*MultiplayerOptionTypes*/ eIndex) const
{
	return m_pGame.isMPOption((MultiplayerOptionTypes)eIndex);
}

bool CyGame::isForcedControl(int /*ForceControlTypes*/ eIndex) const
{
	return m_pGame.isForcedControl((ForceControlTypes)eIndex);
}

int CyGame::getUnitCreatedCount(int /*UnitTypes*/ eIndex) const
{
	return m_pGame.getUnitCreatedCount((UnitTypes)eIndex);
}

bool CyGame::isUnitMaxedOut(int /*UnitTypes*/ eIndex, int iExtra) const
{
	return m_pGame.isUnitMaxedOut((UnitTypes)eIndex, iExtra);
}

int CyGame::getBuildingCreatedCount(int /*BuildingTypes*/ eIndex) const
{
	return m_pGame.getBuildingCreatedCount((BuildingTypes)eIndex);
}

bool CyGame::isBuildingMaxedOut(int /*BuildingTypes*/ eIndex, int iExtra) const
{
	return m_pGame.isBuildingMaxedOut((BuildingTypes)eIndex, iExtra);
}

int CyGame::getProjectCreatedCount(int /*ProjectTypes*/ eIndex) const
{
	return m_pGame.getProjectCreatedCount((ProjectTypes) eIndex);
}

bool CyGame::isProjectMaxedOut(int /*ProjectTypes*/ eIndex, int iExtra) const
{
	return m_pGame.isProjectMaxedOut((ProjectTypes)eIndex, iExtra);
}

int CyGame::getForceCivicCount(int /*CivicTypes*/ eIndex) const
{
	return m_pGame.getForceCivicCount((CivicTypes) eIndex);
}

bool CyGame::isForceCivic(int /*CivicTypes*/ eIndex) const
{
	return m_pGame.isForceCivic((CivicTypes)eIndex);
}

bool CyGame::isForceCivicOption(int /*CivicOptionTypes*/ eCivicOption) const
{
	return m_pGame.isForceCivicOption((CivicOptionTypes)eCivicOption);
}

int CyGame::getVoteOutcome(int /*VoteTypes*/ eIndex) const
{
	return m_pGame.getVoteOutcome((VoteTypes) eIndex);
}

int CyGame::getReligionGameTurnFounded(int /*ReligionTypes*/ eIndex) const
{
	return m_pGame.getReligionGameTurnFounded((ReligionTypes) eIndex);
}

bool CyGame::isReligionFounded(int /*ReligionTypes*/ eIndex) const
{
	return m_pGame.isReligionFounded((ReligionTypes) eIndex);
}

bool CyGame::isReligionSlotTaken(int /*ReligionTypes*/ eIndex) const
{
	return m_pGame.isReligionSlotTaken((ReligionTypes) eIndex);
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

bool CyGame::isTechCanFoundReligion(int /*TechTypes*/ eIndex) const
{
	return m_pGame.isTechCanFoundReligion((TechTypes) eIndex);
}

int CyGame::getCorporationGameTurnFounded(int /*CorporationTypes*/ eIndex) const
{
	return m_pGame.getCorporationGameTurnFounded((CorporationTypes) eIndex);
}

bool CyGame::isCorporationFounded(int /*CorporationTypes*/ eIndex) const
{
	return m_pGame.isCorporationFounded((CorporationTypes) eIndex);
}

bool CyGame::isVotePassed(int /*VoteTypes*/ eIndex) const
{
	return m_pGame.isVotePassed((VoteTypes)eIndex);
}

bool CyGame::isVictoryValid(int /*VictoryTypes*/ eIndex) const
{
	return m_pGame.isVictoryValid((VictoryTypes)eIndex);
}

bool CyGame::isSpecialUnitValid(int /*SpecialUnitTypes*/ eSpecialUnitType) const
{
	return m_pGame.isSpecialUnitValid((SpecialUnitTypes)eSpecialUnitType);
}

void CyGame::makeSpecialUnitValid(int /*SpecialUnitTypes*/ eSpecialUnitType)
{
	m_pGame.makeSpecialUnitValid((SpecialUnitTypes) eSpecialUnitType);
}

bool CyGame::isSpecialBuildingValid(int /*SpecialBuildingTypes*/ eIndex) const
{
	return m_pGame.isSpecialBuildingValid((SpecialBuildingTypes)eIndex);
}

void CyGame::makeSpecialBuildingValid(int /*SpecialBuildingTypes*/ eIndex)
{
	m_pGame.makeSpecialBuildingValid((SpecialBuildingTypes) eIndex);
}

bool CyGame::isInAdvancedStart() const
{
	return m_pGame.isInAdvancedStart();
}

CyCity* CyGame::getHolyCity(int /*ReligionTypes*/ eIndex) const
{
	CvCity* city = m_pGame.getHolyCity((ReligionTypes)eIndex);
	return city ? new CyCity(city) : NULL;
}

void CyGame::setHolyCity(int /*ReligionTypes*/ eIndex, CyCity* pNewValue, bool bAnnounce)
{
	m_pGame.setHolyCity((ReligionTypes) eIndex, pNewValue->getCity(), bAnnounce);
}

void CyGame::clearHolyCity(int /*ReligionTypes*/ eIndex)
{
	m_pGame.setHolyCity((ReligionTypes) eIndex, NULL, false);
}

CyCity* CyGame::getHeadquarters(int /*CorporationTypes*/ eIndex) const
{
	CvCity* city = m_pGame.getHeadquarters((CorporationTypes)eIndex);
	return city ? new CyCity(city) : NULL;
}

void CyGame::setHeadquarters(int /*CorporationTypes*/ eIndex, CyCity* pNewValue, bool bAnnounce)
{
	m_pGame.setHeadquarters((CorporationTypes) eIndex, pNewValue->getCity(), bAnnounce);
}

void CyGame::clearHeadquarters(int /*CorporationTypes*/ eIndex)
{
	m_pGame.setHeadquarters((CorporationTypes) eIndex, NULL, false);
}

int CyGame::getPlayerVote(int /*PlayerTypes*/ eOwnerIndex, int iVoteId) const
{
	return m_pGame.getPlayerVote((PlayerTypes) eOwnerIndex, iVoteId);
}

std::string CyGame::getScriptData() const
{
	return m_pGame.getScriptData();
}

void CyGame::setScriptData(std::string szNewValue)
{
	m_pGame.setScriptData(szNewValue);
}

void CyGame::setName(TCHAR* szNewValue)
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

int CyGame::getMapRandNum(int iNum, TCHAR* pszLog) const
{
	return m_pGame.getMapRandNum(iNum, pszLog);
}

CvRandom& CyGame::getSorenRand() const
{
	return (m_pGame.getSorenRand());
}

int CyGame::getSorenRandNum(int iNum, TCHAR* pszLog) const
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

uint CyGame::getNumReplayMessages() const
{
	return m_pGame.getNumReplayMessages();
}

CyReplayInfo* CyGame::getReplayInfo() const
{
	return new CyReplayInfo(m_pGame.getReplayInfo());
}

bool CyGame::hasSkippedSaveChecksum() const
{
	return m_pGame.hasSkippedSaveChecksum();
}

void CyGame::saveReplay(int iPlayer)
{
	m_pGame.saveReplay((PlayerTypes)iPlayer);
}

void CyGame::addPlayer(int eNewPlayer, int eLeader, int eCiv, bool bSetAlive)
{
	m_pGame.addPlayer((PlayerTypes)eNewPlayer, (LeaderHeadTypes)eLeader, (CivilizationTypes)eCiv, bSetAlive);
}

void CyGame::changeHumanPlayer(int eOldHuman, int eNewHuman)
{
	m_pGame.changeHumanPlayer((PlayerTypes)eOldHuman, (PlayerTypes)eNewHuman);
}

void CyGame::logw(std::wstring str)
{
	logging::logMsgW("C2C.log", (wchar_t*)(CvString(str) + "\n").c_str());
}

void CyGame::log(TCHAR* str)
{
	logging::logMsg("C2C.log", str);
}

void CyGame::addReplayMessage(int /*ReplayMessageTypes*/ eType, int /*PlayerTypes*/ ePlayer, std::wstring pszText, int iPlotX, int iPlotY, int /*ColorTypes*/ eColor)
{
	m_pGame.addReplayMessage((ReplayMessageTypes)eType, (PlayerTypes)ePlayer, pszText, iPlotX, iPlotY, (ColorTypes)eColor);
}

int CyGame::getCultureThreshold(int eLevel) const
{
	return m_pGame.getCultureThreshold((CultureLevelTypes) eLevel);
}

void CyGame::setPlotExtraYield(int iX, int iY, int /*YieldTypes*/ eYield, int iExtraYield)
{
	m_pGame.setPlotExtraYield(iX, iY, (YieldTypes)eYield, iExtraYield);
}

void CyGame::changePlotExtraCost(int iX, int iY, int iCost)
{
	m_pGame.changePlotExtraCost(iX, iY, iCost);
}

bool CyGame::isCivEverActive(int /*CivilizationTypes*/ eCivilization) const
{
	return m_pGame.isCivEverActive((CivilizationTypes)eCivilization);
}

bool CyGame::isLeaderEverActive(int /*LeaderHeadTypes*/ eLeader) const
{
	return m_pGame.isLeaderEverActive((LeaderHeadTypes)eLeader);
}

bool CyGame::isEventActive(int /*EventTriggerTypes*/ eTrigger) const
{
	return m_pGame.isEventActive((EventTriggerTypes)eTrigger);
}

void CyGame::doControl(int iControl)
{
	m_pGame.doControl((ControlTypes) iControl);
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

void CyGame::setVictoryValid(int iVictory, bool bNewVal)
{
	m_pGame.setVictoryValid((VictoryTypes)iVictory, bNewVal);
}

bool CyGame::isModderGameOption(int /*ModderGameOptionTypes*/ eIndex) const
{
	return m_pGame.isModderGameOption((ModderGameOptionTypes)eIndex);
}

int CyGame::getModderGameOption(int /*ModderGameOptionTypes*/ eIndex) const
{
	return m_pGame.getModderGameOption((ModderGameOptionTypes)eIndex);
}

void CyGame::setModderGameOption(int /*ModderGameOptionTypes*/ eIndex, int iNewValue)
{
	m_pGame.setModderGameOption((ModderGameOptionTypes)eIndex, iNewValue);
}

bool CyGame::canEverResearch(int iTech) const
{
	return m_pGame.canEverResearch((TechTypes)iTech);
}

bool CyGame::canEverConstruct(int iBuilding) const
{
	return m_pGame.canEverConstruct((BuildingTypes)iBuilding);
}

bool CyGame::canEverTrain(int iUnit) const
{
	return m_pGame.canEverTrain((UnitTypes)iUnit);
}

bool CyGame::canEverSpread(int iCorporation) const
{
	return m_pGame.canEverSpread((CorporationTypes)iCorporation);
}

const char* CyGame::getC2CVersion() const
{
	return GC.getDefineSTRING("C2C_VERSION");
}

void CyGame::assignStartingPlots(bool bScenario, bool bMapScript)
{
	m_pGame.assignStartingPlots(bScenario, bMapScript);
}