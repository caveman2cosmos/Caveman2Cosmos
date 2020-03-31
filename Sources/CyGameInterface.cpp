#include "CvGameCoreDLL.h"
#include "CyReplayInfo.h"
//
// published python interface for CyGame
//

void CyGamePythonInterface()
{
	OutputDebugString("Python Extension Module - CyGamePythonInterface\n");

	python::class_<CyGame>("CyGame")
		.def("isNone", &CyGame::isNone, "CyGame* () - is the instance valid?")

		.def("isMultiplayer", &CyGame::isMultiplayer, "CyGame* () - is the instance valid?")

		.def("updateScore", &CyGame::updateScore, "void (bool bForce)")

		.def("selectedCitiesGameNetMessage", &CyGame::selectedCitiesGameNetMessage, "void (int eMessage, int iData2, int iData3, int iData4, bool bOption, bool bAlt, bool bShift, bool bCtrl)")

		.def("getSymbolID", &CyGame::getSymbolID, "int (int iSymbol)")

		.def("getProductionPerPopulation", &CyGame::getProductionPerPopulation, "int (int /*HurryTypes*/ eHurry)")

		.def("getAdjustedPopulationPercent", &CyGame::getAdjustedPopulationPercent, "int (int eVictory)")
		.def("getAdjustedLandPercent", &CyGame::getAdjustedLandPercent, "int (int eVictory)")

		.def("isChooseElection", &CyGame::isChooseElection, "bool (int eVote)")
		.def("isTeamVoteEligible", &CyGame::isTeamVoteEligible, "bool (int eTeam, int eVoteSource)")
		.def("countPossibleVote", &CyGame::countPossibleVote, "int (int eVote, int eVoteSource)")
		.def("getVoteRequired", &CyGame::getVoteRequired, "int (int eVote, int eVoteSource)")
		.def("getSecretaryGeneral", &CyGame::getSecretaryGeneral, "int (int eVoteSource)")
		.def("canHaveSecretaryGeneral", &CyGame::canHaveSecretaryGeneral, "bool (int eVoteSource)")
		.def("getVoteSourceReligion", &CyGame::getVoteSourceReligion, "int (int /*VoteSourceTypes*/ eVoteSource)")

		.def("countCivPlayersAlive", &CyGame::countCivPlayersAlive, "int ()")
		.def("countCivPlayersEverAlive", &CyGame::countCivPlayersEverAlive, "int ()")
		.def("countCivTeamsAlive", &CyGame::countCivTeamsAlive, "int ()")

		.def("countKnownTechNumTeams", &CyGame::countKnownTechNumTeams, "int (int eTech)")

		.def("countReligionLevels", &CyGame::countReligionLevels, "int (int eReligion)")
		.def("calculateReligionPercent", &CyGame::calculateReligionPercent, "int (int eReligion)")
		.def("countCorporationLevels", &CyGame::countCorporationLevels, "int (int eCorporation)")

		.def("goldenAgeLength", &CyGame::goldenAgeLength, "int ()")

		.def("getHighestEra", &CyGame::getHighestEra, "int /*EratTypes*/ ()")
		.def("getCurrentEra", &CyGame::getCurrentEra, "int /*EratTypes*/ ()")

		.def("getActiveTeam", &CyGame::getActiveTeam, "int () - returns ID for the group")
		.def("getActiveCivilizationType", &CyGame::getActiveCivilizationType, "int () - returns CivilizationID" )
		.def("isNetworkMultiPlayer", &CyGame::isNetworkMultiPlayer, "bool () - NetworkMultiplayer()? ")
		.def("isGameMultiPlayer", &CyGame::isGameMultiPlayer, "bool () - GameMultiplayer()? ")
		.def("getNumHumanPlayers", &CyGame::getNumHumanPlayers, "int () - # of human players in-game")

		.def("isModem", &CyGame::isModem, "bool () - Using a modem? ")
		.def("setModem", &CyGame::setModem, "void (bool bModem) - Use a modem! (or don't)")

		.def("getGameTurn", &CyGame::getGameTurn, "int () - current game turn")
		.def("getTurnYear", &CyGame::getTurnYear, "int (iGameTurn) - turn Time")
		.def("getGameTurnYear", &CyGame::getGameTurnYear)
		.def("getElapsedGameTurns", &CyGame::getElapsedGameTurns, "int () - Elapsed turns thus far")
		.def("getMaxTurns", &CyGame::getMaxTurns)
		.def("getMaxCityElimination", &CyGame::getMaxCityElimination)
		.def("getNumAdvancedStartPoints", &CyGame::getNumAdvancedStartPoints)
		.def("getStartTurn", &CyGame::getStartTurn, "int () - Returns the starting Turn (0 unless a scenario or advanced era start)")
		.def("getStartYear", &CyGame::getStartYear, "int () - Returns the starting year (e.g. -4000)")
		.def("setStartYear", &CyGame::setStartYear, "void () - Sets the starting year (e.g. -4000)")
		.def("getEstimateEndTurn", &CyGame::getEstimateEndTurn)
		.def("getMinutesPlayed", &CyGame::getMinutesPlayed, "Returns the number of minutes since the game began")
		.def("getTargetScore", &CyGame::getTargetScore)

		.def("getNumCities", &CyGame::getNumCities, "int () - total cities in Game")
		.def("getTotalPopulation", &CyGame::getTotalPopulation, "int () - total game population")

		.def("getTradeRoutes", &CyGame::getTradeRoutes)
		.def("changeNoNukesCount", &CyGame::changeNoNukesCount)
		.def("getSecretaryGeneralTimer", &CyGame::getSecretaryGeneralTimer)
		.def("getVoteTimer", &CyGame::getVoteTimer)

		.def("getMaxPopulation", &CyGame::getMaxPopulation)
		.def("getMaxLand", &CyGame::getMaxLand)
		.def("getMaxTech", &CyGame::getMaxTech)
		.def("getMaxWonders", &CyGame::getMaxWonders)
		.def("getInitPopulation", &CyGame::getInitPopulation)
		.def("getInitLand", &CyGame::getInitLand)
		.def("getInitTech", &CyGame::getInitTech)
		.def("getInitWonders", &CyGame::getInitWonders)

		.def("getAIAutoPlay", &CyGame::getAIAutoPlay, "int (int iPlayer)")
		.def("setAIAutoPlay", &CyGame::setAIAutoPlay, "void (int iPlayer, int iValue)")
		.def("isForcedAIAutoPlay", &CyGame::isForcedAIAutoPlay, "bool (int iPlayer)")
		.def("setForcedAIAutoPlay", &CyGame::setForcedAIAutoPlay, "void (int iPlayer, int iValue, bool bForced)")

		.def("getXResolution", &CyGame::getXResolution, "int ()")
		.def("getYResolution", &CyGame::getYResolution, "int ()")

		.def("isDiploVote", &CyGame::isDiploVote, "bool (VoteSourceTypes)")
		.def("isDebugMode", &CyGame::isDebugMode, "bool () - is the game in Debug Mode?")
		.def("toggleDebugMode", &CyGame::toggleDebugMode)

		.def("getPitbossTurnTime", &CyGame::getPitbossTurnTime, "int ()")
		.def("isHotSeat", &CyGame::isHotSeat, "bool ()")
		.def("isPbem", &CyGame::isPbem, "bool ()")
		.def("isPitboss", &CyGame::isPitboss, "bool ()")

		.def("isFinalInitialized", &CyGame::isFinalInitialized, "bool () - Returns whether or not the game initialization process has ended (game has started)")

		.def("getActivePlayer", &CyGame::getActivePlayer, "returns index of the active player")
		.def("setActivePlayer", &CyGame::setActivePlayer, "void (int /*PlayerTypes*/ eNewValue, bool bForceHotSeat)")
		.def("getPausePlayer", &CyGame::getPausePlayer, "int () - will get who paused us")
		.def("isPaused", &CyGame::isPaused, "bool () - will say if the game is paused")
		.def("getVictory", &CyGame::getVictory)
		.def("getGameState", &CyGame::getGameState)
		.def("getHandicapType", &CyGame::getHandicapType, "HandicapType () - difficulty level settings")
		.def("getCalendar", &CyGame::getCalendar, "CalendarType ()")
		.def("getStartEra", &CyGame::getStartEra)
		.def("getGameSpeedType", &CyGame::getGameSpeedType)
		.def("getRankPlayer", &CyGame::getRankPlayer)
		.def("getPlayerRank", &CyGame::getPlayerRank)
		.def("getPlayerScore", &CyGame::getPlayerScore)
		.def("getRankTeam", &CyGame::getRankTeam)
		.def("getTeamScore", &CyGame::getTeamScore)
		.def("isOption", &CyGame::isOption, "bool (eIndex) - returns whether Game Option is valid")
		.def("setOption", &CyGame::setOption, "void (GameOptionIndex, bEnabled) - sets a Game Option")
		.def("isMPOption", &CyGame::isMPOption, "bool (eIndex) - returns whether MP Option is valid")
		.def("isForcedControl", &CyGame::isForcedControl, "bool (eIndex) - returns whether Control should be forced")
		.def("isBuildingMaxedOut", &CyGame::isBuildingMaxedOut, "bool (BuildingType) - max # reached?")
		.def("isUnitMaxedOut", &CyGame::isUnitMaxedOut, "bool (eIndex, iExtra) - returns whether or not this unit class is maxed out (e.g. spies)")

		.def("getProjectCreatedCount", &CyGame::getProjectCreatedCount, "int (ProjectTypes eIndex)")

		.def("getReligionGameTurnFounded", &CyGame::getReligionGameTurnFounded)
		.def("isReligionSlotTaken", &CyGame::isReligionSlotTaken, "bool (ReligionID) - is religion in that tech slot founded?")

		.def("getCorporationGameTurnFounded", &CyGame::getCorporationGameTurnFounded)
		.def("isCorporationFounded", &CyGame::isCorporationFounded, "bool (CorporationID) - is corporation founded?")
		.def("isVictoryValid", &CyGame::isVictoryValid)
		.def("isVotePassed", &CyGame::isVotePassed)

		.def("isInAdvancedStart", &CyGame::isInAdvancedStart, "bool")

		.def("getHolyCity", &CyGame::getHolyCity, python::return_value_policy<python::manage_new_object>(), "CyCity getHolyCity()")
		.def("setHolyCity", &CyGame::setHolyCity, "void (int eIndex, CyCity *pNewValue, bAnnounce) - Sets holy city for religion eIndex to pNewValue")
		.def("clearHolyCity", &CyGame::clearHolyCity, "void (int eIndex) - clears the holy city for religion eIndex")

		.def("getHeadquarters", &CyGame::getHeadquarters, python::return_value_policy<python::manage_new_object>(), "CyCity getHeadquarters()")
		.def("setHeadquarters", &CyGame::setHeadquarters, "void (int eIndex, CyCity *pNewValue, bAnnounce) - Sets headquarters for corporation eIndex to pNewValue")
		.def("clearHeadquarters", &CyGame::clearHeadquarters, "void (int eIndex) - clears the headquarters for corporation eIndex")

		.def("getScriptData", &CyGame::getScriptData, "str () - Returns ScriptData member (used to store custom data)")
		.def("setScriptData", &CyGame::setScriptData, "void (str) - Sets ScriptData member (used to store custom data)")

		.def("setName", &CyGame::setName)
		.def("getName", &CyGame::getName)
		.def("getIndexAfterLastDeal", &CyGame::getIndexAfterLastDeal)
		.def("getNumDeals", &CyGame::getNumDeals)
		.def("getDeal", &CyGame::getDeal, python::return_value_policy<python::manage_new_object>())
		.def("getMapRand", &CyGame::getMapRand, python::return_value_policy<python::reference_existing_object>())
		.def("getSorenRand", &CyGame::getSorenRand, python::return_value_policy<python::reference_existing_object>())
		.def("getSorenRandNum", &CyGame::getSorenRandNum)

		.def("GetWorldBuilderMode", &CyGame::GetWorldBuilderMode)
		.def("isPitbossHost", &CyGame::isPitbossHost)
		.def("getCurrentLanguage", &CyGame::getCurrentLanguage)
		.def("setCurrentLanguage", &CyGame::setCurrentLanguage)

		.def("getReplayMessageTurn", &CyGame::getReplayMessageTurn)
		.def("getReplayMessageText", &CyGame::getReplayMessageText)
		.def("getNumReplayMessages", &CyGame::getNumReplayMessages)
		.def("getReplayInfo", &CyGame::getReplayInfo, python::return_value_policy<python::manage_new_object>())
		.def("hasSkippedSaveChecksum", &CyGame::hasSkippedSaveChecksum)
		.def("saveReplay", &CyGame::saveReplay)

		.def("addPlayer", &CyGame::addPlayer, "void (int eNewPlayer, int eLeader, int eCiv)")
		//.def("addPlayer", &CyGame::addPlayer, "void (int eNewPlayer, int eLeader, int eCiv, [bool bSetAlive = true]) - if bSetAlive = false new player isn't set to be alive and won't die if not given units or cities")
		.def("changeHumanPlayer", &CyGame::changeHumanPlayer, "void ( int /*PlayerTypes*/ eOldHuman, int /*PlayerTypes*/ eNewHuman )" )
		.def("addReplayMessage", &CyGame::addReplayMessage, "void (int /*ReplayMessageTypes*/ eType, int /*PlayerTypes*/ ePlayer, std::wstring pszText, int iPlotX, int iPlotY, int /*ColorTypes*/ eColor)" )
		.def("log", &CyGame::log, "void log(str)")

		.def("drawBattleEffects", &CyGame::drawBattleEffects, "void ()")

		.def("getCultureThreshold", &CyGame::getCultureThreshold, "int getCultureThreshold(CultureLevelTypes eLevel)")

		.def("setPlotExtraYield", &CyGame::setPlotExtraYield, "void (int iX, int iY, int /*YieldTypes*/ eYield, int iExtraYield)")

		.def("isCivEverActive", &CyGame::isCivEverActive, "bool (int /*CivilizationTypes*/ eCivilization)")
		.def("isLeaderEverActive", &CyGame::isLeaderEverActive, "bool (int /*LeaderHeadTypes*/ eLeader)")

		.def("isEventActive", &CyGame::isEventActive, "bool (int /*EventTriggerTypes*/ eTrigger)")
		.def("doControl", &CyGame::doControl, "void (int /*ControlTypes*/ iControl)")

		.def("saveGame", &CyGame::saveGame, "void saveGame(string filename)")

		.def("getStarshipLaunched", &CyGame::getStarshipLaunched, "bool ()")
		.def("cheatCodesEnabled", &CyGame::cheatCodesEnabled)
		.def("setModderGameOption", &CyGame::setModderGameOption, "void ()")
		.def("canEverResearch", &CyGame::canEverResearch, "bool (int iTech)")
		.def("canEverSpread", &CyGame::canEverSpread, "bool (int iCorporation)")
		.def("getC2CVersion", &CyGame::getC2CVersion, "const char* ()")
	;


	python::class_<CyDeal>("CyDeal")
		.def("isNone", &CyDeal::isNone)
		.def("getID", &CyDeal::getID)
		.def("getInitialGameTurn", &CyDeal::getInitialGameTurn)
		.def("getFirstPlayer", &CyDeal::getFirstPlayer)
		.def("getSecondPlayer", &CyDeal::getSecondPlayer)
		.def("getLengthFirstTrades", &CyDeal::getLengthFirstTrades)
		.def("getLengthSecondTrades", &CyDeal::getLengthSecondTrades)
		.def("getFirstTrade", &CyDeal::getFirstTrade, python::return_value_policy<python::reference_existing_object>())
		.def("getSecondTrade", &CyDeal::getSecondTrade, python::return_value_policy<python::reference_existing_object>())
		.def("kill", &CyDeal::kill)

		.def("isCancelable", &CyDeal::isCancelable, "bool isCancelable(int /*PlayerTypes*/ eByPlayer, bool bIgnoreWaitingPeriod)")
		.def("getCannotCancelReason", &CyDeal::getCannotCancelReason, "string getCannotCancelReason(int /*PlayerTypes*/ eByPlayer)")
		.def("turnsToCancel", &CyDeal::turnsToCancel, "int turnsToCancel(int /*PlayerTypes*/ eByPlayer)")
	;
}
