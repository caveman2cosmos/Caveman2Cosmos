#include "CvGameCoreDLL.h"
#include "CyPlayer.h"

//
// published python interface for CyPlayer
//

void CyPlayerPythonInterface2(python::class_<CyPlayer>& x)
{
	OutputDebugString("Python Extension Module - CyPlayerPythonInterface2\n");

	// set the docstring of the current module scope
	python::scope().attr("__doc__") = "Civilization IV Player Class";
	x
		.def("AI_updateFoundValues", &CyPlayer::AI_updateFoundValues, "void (bool bStartingLoc)")
		.def("AI_isFinancialTrouble", &CyPlayer::AI_isFinancialTrouble, "bool ()")
		.def("AI_demandRebukedWar", &CyPlayer::AI_demandRebukedWar, "bool (int /*PlayerTypes*/)")
		.def("AI_getAttitude", &CyPlayer::AI_getAttitude, "AttitudeTypes (int /*PlayerTypes*/) - Gets the attitude of the player towards the player passed in")
		.def("AI_unitValue", &CyPlayer::AI_unitValue, "int (int /*UnitTypes*/ eUnit, int /*UnitAITypes*/ eUnitAI, CyArea* pArea)")
		.def("AI_civicValue", &CyPlayer::AI_civicValue, "int (int /*CivicTypes*/ eCivic)")
		.def("AI_totalUnitAIs", &CyPlayer::AI_totalUnitAIs, "int (int /*UnitAITypes*/ eUnitAI)")
		.def("AI_totalAreaUnitAIs", &CyPlayer::AI_totalAreaUnitAIs, "int (CyArea* pArea, int /*UnitAITypes*/ eUnitAI)")
		.def("AI_getNumAIUnits", &CyPlayer::AI_getNumAIUnits, "int (UnitAIType) - Returns # of UnitAITypes the player current has of UnitAIType")
		.def("AI_getAttitudeExtra", &CyPlayer::AI_getAttitudeExtra, "int (int /*PlayerTypes*/ eIndex) - Returns the extra attitude for this player - usually scenario specific")
		.def("AI_setAttitudeExtra", &CyPlayer::AI_setAttitudeExtra, "void (int /*PlayerTypes*/ eIndex, int iNewValue) - Sets the extra attitude for this player - usually scenario specific")
		.def("AI_changeAttitudeExtra", &CyPlayer::AI_changeAttitudeExtra, "void (int /*PlayerTypes*/ eIndex, int iChange) - Changes the extra attitude for this player - usually scenario specific")
		.def("AI_getMemoryCount", &CyPlayer::AI_getMemoryCount, "int (/*PlayerTypes*/ eIndex1, /*MemoryTypes*/ eIndex2)")
		.def("AI_changeMemoryCount", &CyPlayer::AI_changeMemoryCount, "void (/*PlayerTypes*/ eIndex1, /*MemoryTypes*/ eIndex2, int iChange)")
		.def("AI_isWillingToTalk", &CyPlayer::AI_isWillingToTalk, "bool (int /*PlayerTypes*/)")

		.def("getScoreHistory", &CyPlayer::getScoreHistory, "int (int iTurn)")
		.def("getEconomyHistory", &CyPlayer::getEconomyHistory, "int (int iTurn)")
		.def("getIndustryHistory", &CyPlayer::getIndustryHistory, "int (int iTurn)")
		.def("getAgricultureHistory", &CyPlayer::getAgricultureHistory, "int (int iTurn)")
		.def("getPowerHistory", &CyPlayer::getPowerHistory, "int (int iTurn)")
		.def("getCultureHistory", &CyPlayer::getCultureHistory, "int (int iTurn)")
		.def("getEspionageHistory", &CyPlayer::getEspionageHistory, "int (int iTurn)")

		.def("getScriptData", &CyPlayer::getScriptData, "str () - Get stored custom data (via pickle)")
		.def("setScriptData", &CyPlayer::setScriptData, "void (str) - Set stored custom data (via pickle)")

		.def("AI_maxGoldTrade", &CyPlayer::AI_maxGoldTrade, "int (int)")
		.def("AI_maxGoldPerTurnTrade", &CyPlayer::AI_maxGoldPerTurnTrade, "int (int)")

		.def("canSplitEmpire", &CyPlayer::canSplitEmpire, "bool ()")
		.def("canSplitArea", &CyPlayer::canSplitArea, "bool (int)")

		.def("assimilatePlayer", &CyPlayer::assimilatePlayer, "bool ( int iPlayer ) - acquire iPlayer's units and cities")

		.def("canHaveTradeRoutesWith", &CyPlayer::canHaveTradeRoutesWith, "bool (int)")
		.def("forcePeace", &CyPlayer::forcePeace, "void (int)")

		.def("addReminder", &CyPlayer::addReminder, "void (int iGameTurn, string szMessage)")
		.def("setFoundedFirstCity", &CyPlayer::setFoundedFirstCity, "void (bool bNewValue)")
		.def("setAlive", &CyPlayer::setAlive, "void (bool bNewValue)")
		.def("setNewPlayerAlive", &CyPlayer::setNewPlayerAlive, "void (bool bNewValue) - like setAlive, but without firing turn logic")
		.def("changeTechScore", &CyPlayer::changeTechScore, "void (int iChange)" )
		.def("getStabilityIndex", &CyPlayer::getStabilityIndex, "int ( )")
		.def("changeStabilityIndex", &CyPlayer::changeStabilityIndex, "void ( int iChange )")
		.def("getStabilityIndexAverage", &CyPlayer::getStabilityIndexAverage, "int ( )")
		.def("updateStabilityIndexAverage", &CyPlayer::updateStabilityIndexAverage, "void ( )")
		.def("getRevolutionStabilityHistory", &CyPlayer::getRevolutionStabilityHistory, "int (int iTurn)")

		.def("isNonStateReligionCommerce", &CyPlayer::isNonStateReligionCommerce, "bool ()")

		.def("getBuildingCountWithUpgrades", &CyPlayer::getBuildingCountWithUpgrades, "int (int iBuilding)")
		.def("setColor", &CyPlayer::setColor, "void (int iColor)")
		.def("setHandicap", &CyPlayer::setHandicap, "void (int iNewVal)")
		.def("setModderOption", &CyPlayer::setModderOption, "void ()")
		.def("doRevolution", &CyPlayer::doRevolution, "void (int (CivicTypes*) paeNewCivics, bool bForce)")
		.def("isAutomatedCanBuild", &CyPlayer::isAutomatedCanBuild, "bool ()")
		.def("setAutomatedCanBuild", &CyPlayer::setAutomatedCanBuild, "void ()")
		.def("getBestUnitType", &CyPlayer::getBestUnitType, "int (int /*UnitAITypes*/ eUnitAI)")

		.def("getCulture", &CyPlayer::getCulture, "int ()")

		.def("getBuildingListFilterActive", &CyPlayer::getBuildingListFilterActive, "bool (int)")
		.def("setBuildingListFilterActive", &CyPlayer::setBuildingListFilterActive, "void (int,bool)")
		.def("getBuildingListGrouping", &CyPlayer::getBuildingListGrouping, "int ()")
		.def("setBuildingListGrouping", &CyPlayer::setBuildingListGrouping, "void (int)")
		.def("getBuildingListSorting", &CyPlayer::getBuildingListSorting, "int ()")
		.def("setBuildingListSorting", &CyPlayer::setBuildingListSorting, "void (int)")
		.def("getBuildingListGroupNum", &CyPlayer::getBuildingListGroupNum, "int ()")
		.def("getBuildingListNumInGroup", &CyPlayer::getBuildingListNumInGroup, "int (int)")
		.def("getBuildingListType", &CyPlayer::getBuildingListType, "int (int,int)")

		.def("setUnitListInvalid", &CyPlayer::setUnitListInvalid, "void ()")
		.def("getUnitListFilterActive", &CyPlayer::getUnitListFilterActive, "bool (int)")
		.def("setUnitListFilterActive", &CyPlayer::setUnitListFilterActive, "void (int,bool)")
		.def("getUnitListGrouping", &CyPlayer::getUnitListGrouping, "int ()")
		.def("setUnitListGrouping", &CyPlayer::setUnitListGrouping, "void (int)")
		.def("getUnitListSorting", &CyPlayer::getUnitListSorting, "int ()")
		.def("setUnitListSorting", &CyPlayer::setUnitListSorting, "void (int)")
		.def("getUnitListGroupNum", &CyPlayer::getUnitListGroupNum, "int ()")
		.def("getUnitListNumInGroup", &CyPlayer::getUnitListNumInGroup, "int (int)")
		.def("getUnitListType", &CyPlayer::getUnitListType, "int (int,int)")
		.def("makeNukesValid", &CyPlayer::makeNukesValid, " void (bool bValid)")

		// AIAndy: Build Lists
		.def("getBLNumLists", &CyPlayer::getBLNumLists, "int ()")
		.def("getBLIndexByID", &CyPlayer::getBLIndexByID, "int (int)")
		.def("getBLID", &CyPlayer::getBLID, "int (int)")
		.def("getBLListName", &CyPlayer::getBLListName, "string (int)")
		.def("getBLListLength", &CyPlayer::getBLListLength, "int (int)")
		.def("getBLOrder", &CyPlayer::getBLOrder, python::return_value_policy<python::reference_existing_object>(), "OrderData (int, int)")
		.def("writeBLToFile", &CyPlayer::writeBLToFile, "void ()")
		.def("getBLCurrentList", &CyPlayer::getBLCurrentList, "int ()")
		.def("addBLList", &CyPlayer::addBLList, "void ()")
		.def("renameBLList", &CyPlayer::renameBLList, "void (int)")
		.def("removeBLList", &CyPlayer::removeBLList, "void (int)")
	;
}
