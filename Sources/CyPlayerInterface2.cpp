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
		.def("AI_foundValue", &CyPlayer::AI_foundValue, "int (int, int, int, bool)")
		.def("AI_isFinancialTrouble", &CyPlayer::AI_isFinancialTrouble, "bool ()")
		.def("AI_demandRebukedWar", &CyPlayer::AI_demandRebukedWar, "bool (int /*PlayerTypes*/)")
		.def("AI_getAttitude", &CyPlayer::AI_getAttitude, "AttitudeTypes (int /*PlayerTypes*/) - Gets the attitude of the player towards the player passed in")
		.def("AI_unitValue", &CyPlayer::AI_unitValue, "int (int /*UnitTypes*/ eUnit, int /*UnitAITypes*/ eUnitAI, CyArea* pArea)")
		.def("AI_civicValue", &CyPlayer::AI_civicValue, "int (int /*CivicTypes*/ eCivic)")
		.def("AI_totalUnitAIs", &CyPlayer::AI_totalUnitAIs, "int (int /*UnitAITypes*/ eUnitAI)")
		.def("AI_totalAreaUnitAIs", &CyPlayer::AI_totalAreaUnitAIs, "int (CyArea* pArea, int /*UnitAITypes*/ eUnitAI)")
		.def("AI_totalWaterAreaUnitAIs", &CyPlayer::AI_totalWaterAreaUnitAIs, "int (CyArea* pArea, int /*UnitAITypes*/ eUnitAI)")
		.def("AI_getNumAIUnits", &CyPlayer::AI_getNumAIUnits, "int (UnitAIType) - Returns # of UnitAITypes the player current has of UnitAIType")
		.def("AI_getAttitudeExtra", &CyPlayer::AI_getAttitudeExtra, "int (int /*PlayerTypes*/ eIndex) - Returns the extra attitude for this player - usually scenario specific")
		.def("AI_setAttitudeExtra", &CyPlayer::AI_setAttitudeExtra, "void (int /*PlayerTypes*/ eIndex, int iNewValue) - Sets the extra attitude for this player - usually scenario specific")
		.def("AI_changeAttitudeExtra", &CyPlayer::AI_changeAttitudeExtra, "void (int /*PlayerTypes*/ eIndex, int iChange) - Changes the extra attitude for this player - usually scenario specific")
		.def("AI_getMemoryCount", &CyPlayer::AI_getMemoryCount, "int (/*PlayerTypes*/ eIndex1, /*MemoryTypes*/ eIndex2)")
		.def("AI_changeMemoryCount", &CyPlayer::AI_changeMemoryCount, "void (/*PlayerTypes*/ eIndex1, /*MemoryTypes*/ eIndex2, int iChange)")
		.def("AI_getExtraGoldTarget", &CyPlayer::AI_getExtraGoldTarget, "int ()")
		.def("AI_setExtraGoldTarget", &CyPlayer::AI_setExtraGoldTarget, "void (int)")
// BUG - Refuses to Talk - start
		.def("AI_isWillingToTalk", &CyPlayer::AI_isWillingToTalk, "bool (int /*PlayerTypes*/)")
// BUG - Refuses to Talk - end

		.def("getScoreHistory", &CyPlayer::getScoreHistory, "int (int iTurn)")
		.def("getEconomyHistory", &CyPlayer::getEconomyHistory, "int (int iTurn)")
		.def("getIndustryHistory", &CyPlayer::getIndustryHistory, "int (int iTurn)")
		.def("getAgricultureHistory", &CyPlayer::getAgricultureHistory, "int (int iTurn)")
		.def("getPowerHistory", &CyPlayer::getPowerHistory, "int (int iTurn)")
		.def("getCultureHistory", &CyPlayer::getCultureHistory, "int (int iTurn)")
		.def("getEspionageHistory", &CyPlayer::getEspionageHistory, "int (int iTurn)")

		.def("getScriptData", &CyPlayer::getScriptData, "str () - Get stored custom data (via pickle)")
		.def("setScriptData", &CyPlayer::setScriptData, "void (str) - Set stored custom data (via pickle)")

		.def("chooseTech", &CyPlayer::chooseTech, "void (int iDiscover, wstring szText, bool bFront)")

		.def("AI_maxGoldTrade", &CyPlayer::AI_maxGoldTrade, "int (int)")
		.def("AI_maxGoldPerTurnTrade", &CyPlayer::AI_maxGoldPerTurnTrade, "int (int)")

		.def("splitEmpire", &CyPlayer::splitEmpire, "bool (int iAreaId)")
		.def("canSplitEmpire", &CyPlayer::canSplitEmpire, "bool ()")
		.def("canSplitArea", &CyPlayer::canSplitArea, "bool (int)")
/************************************************************************************************/
/* REVOLUTION_MOD                         11/15/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		.def("assimilatePlayer", &CyPlayer::assimilatePlayer, "bool ( int iPlayer ) - acquire iPlayer's units and cities")
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
		.def("canHaveTradeRoutesWith", &CyPlayer::canHaveTradeRoutesWith, "bool (int)")
		.def("forcePeace", &CyPlayer::forcePeace, "void (int)")

/************************************************************************************************/
/* REVOLUTION_MOD                         06/11/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/

// BUG - Reminder Mod - start
		.def("addReminder", &CyPlayer::addReminder, "void (int iGameTurn, string szMessage)")
// BUG - Reminder Mod - end
		.def("setFoundedFirstCity", &CyPlayer::setFoundedFirstCity, "void (bool bNewValue)")
		.def("setAlive", &CyPlayer::setAlive, "void (bool bNewValue)")
		.def("setNewPlayerAlive", &CyPlayer::setNewPlayerAlive, "void (bool bNewValue) - like setAlive, but without firing turn logic")
		.def("changeTechScore", &CyPlayer::changeTechScore, "void (int iChange)" )
		.def("getStabilityIndex", &CyPlayer::getStabilityIndex, "int ( )")
		.def("setStabilityIndex", &CyPlayer::setStabilityIndex, "void ( int iNewValue )")
		.def("changeStabilityIndex", &CyPlayer::changeStabilityIndex, "void ( int iChange )")
		.def("getStabilityIndexAverage", &CyPlayer::getStabilityIndexAverage, "int ( )")
		.def("setStabilityIndexAverage", &CyPlayer::setStabilityIndexAverage, "void ( int iNewValue )")
		.def("updateStabilityIndexAverage", &CyPlayer::updateStabilityIndexAverage, "void ( )")																												// Exposed to Python
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

		// RevolutionDCM - revolution stability history
		.def("getRevolutionStabilityHistory", &CyPlayer::getRevolutionStabilityHistory, "int (int iTurn)")
/************************************************************************************************/
/* REVDCM                                 02/16/10                                phungus420    */
/*                                                                                              */
/* RevTrait Effects                                                                             */
/************************************************************************************************/
		.def("isNonStateReligionCommerce", &CyPlayer::isNonStateReligionCommerce, "bool ()")
		.def("isUpgradeAnywhere", &CyPlayer::isUpgradeAnywhere, "bool ()")
/************************************************************************************************/
/* REVDCM                                  END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/* Afforess	                  Start		 04/15/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		.def("hasValidBuildings", &CyPlayer::hasValidBuildings, "bool (int iTech)")
		.def("getBonusCommerceModifier", &CyPlayer::getBonusCommerceModifier, "int (int i, int j)")
		.def("isShowLandmarks", &CyPlayer::isShowLandmarks, "bool ()")
		.def("setShowLandmarks", &CyPlayer::setShowLandmarks, "void (bool bNewVal)")
		.def("getBuildingCountWithUpgrades", &CyPlayer::getBuildingCountWithUpgrades, "int (int iBuilding)")
		.def("setColor", &CyPlayer::setColor, "void (int iColor)")
		.def("setHandicap", &CyPlayer::setHandicap, "void (int iNewVal)")
		.def("isModderOption", &CyPlayer::isModderOption, "bool ()")
		.def("getModderOption", &CyPlayer::getModderOption, "bool ()")
		.def("setModderOption", &CyPlayer::setModderOption, "void ()")
		.def("doRevolution", &CyPlayer::doRevolution, "void (int (CivicTypes*) paeNewCivics, bool bForce)")
		.def("isAutomatedCanBuild", &CyPlayer::isAutomatedCanBuild, "bool ()")
		.def("setAutomatedCanBuild", &CyPlayer::setAutomatedCanBuild, "void ()")
		.def("setTeam", &CyPlayer::setTeam, "void ()")
		.def("getBestUnitType", &CyPlayer::getBestUnitType, "int (int /*UnitAITypes*/ eUnitAI)")
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		.def("getCulture", &CyPlayer::getCulture, "int ()")
		.def("setCulture", &CyPlayer::setCulture, "void (int)")
		.def("changeCulture", &CyPlayer::changeCulture, "void (int)")
		.def("getProperties", &CyPlayer::getProperties, python::return_value_policy<python::reference_existing_object>(), "CvProperties ()")

		.def("setBuildingListInvalid", &CyPlayer::setBuildingListInvalid, "void ()")
		.def("getBuildingListFilterActive", &CyPlayer::getBuildingListFilterActive, "bool (int)")
		.def("setBuildingListFilterActive", &CyPlayer::setBuildingListFilterActive, "void (int,bool)")
		.def("getBuildingListGrouping", &CyPlayer::getBuildingListGrouping, "int ()")
		.def("setBuildingListGrouping", &CyPlayer::setBuildingListGrouping, "void (int)")
		.def("getBuildingListSorting", &CyPlayer::getBuildingListSorting, "int ()")
		.def("setBuildingListSorting", &CyPlayer::setBuildingListSorting, "void (int)")
		.def("getBuildingListGroupNum", &CyPlayer::getBuildingListGroupNum, "int ()")
		.def("getBuildingListNumInGroup", &CyPlayer::getBuildingListNumInGroup, "int (int)")
		.def("getBuildingListType", &CyPlayer::getBuildingListType, "int (int,int)")
		.def("getBuildingListSelectedBuildingRow", &CyPlayer::getBuildingListSelectedBuildingRow, "int ()")
		.def("getBuildingListSelectedWonderRow", &CyPlayer::getBuildingListSelectedWonderRow, "int ()")
		.def("setBuildingListSelectedBuilding", &CyPlayer::setBuildingListSelectedBuilding, "void (int)")
		.def("setBuildingListSelectedWonder", &CyPlayer::setBuildingListSelectedWonder, "void (int)")
		.def("getBuildingListSelectedBuilding", &CyPlayer::getBuildingListSelectedBuilding, "int ()")
		.def("getBuildingListSelectedWonder", &CyPlayer::getBuildingListSelectedWonder, "int ()")

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
		.def("getUnitListSelectedRow", &CyPlayer::getUnitListSelectedRow, "int ()")
		.def("setUnitListSelected", &CyPlayer::setUnitListSelected, "void (int)")
		.def("getUnitListSelected", &CyPlayer::getUnitListSelected, "int ()")
		.def("isNukesValid", &CyPlayer::isNukesValid, "bool")
		.def("makeNukesValid", &CyPlayer::makeNukesValid, " void (bool bValid)")

		//TB Combat Mod begin
		.def("getPlayerWideAfflictionCount", &CyPlayer::getPlayerWideAfflictionCount, "int (int)")
		.def("changePlayerWideAfflictionCount", &CyPlayer::changePlayerWideAfflictionCount, "void (int,int)")
		.def("setPlayerWideAfflictionCount", &CyPlayer::setPlayerWideAfflictionCount, "void (int,int)")
		.def("countAfflictedUnits", &CyPlayer::countAfflictedUnits, "int (int)")
		.def("recalculateAfflictedUnitCount", &CyPlayer::recalculateAfflictedUnitCount, "void ()")
		//TB Combat Mod end

		//TB Traits begin
		.def("getCivicAnarchyModifier", &CyPlayer::getCivicAnarchyModifier, "int ()")
		.def("setCivicAnarchyModifier", &CyPlayer::setCivicAnarchyModifier, "void (int)")
		.def("changeCivicAnarchyModifier", &CyPlayer::changeCivicAnarchyModifier, "void (int)")
		.def("getReligiousAnarchyModifier", &CyPlayer::getReligiousAnarchyModifier, "int ()")
		.def("setReligiousAnarchyModifier", &CyPlayer::setReligiousAnarchyModifier, "void (int)")
		.def("changeReligiousAnarchyModifier", &CyPlayer::changeReligiousAnarchyModifier, "void (int)")
		.def("getImprovementUpgradeRateModifierSpecific", &CyPlayer::getImprovementUpgradeRateModifierSpecific, "int (int)")
		.def("changeImprovementUpgradeRateModifierSpecific", &CyPlayer::changeImprovementUpgradeRateModifierSpecific, "void (int,int)")
		.def("getBuildWorkerSpeedModifierSpecific", &CyPlayer::getBuildWorkerSpeedModifierSpecific, "int (int)")
		.def("changeBuildWorkerSpeedModifierSpecific", &CyPlayer::changeBuildWorkerSpeedModifierSpecific, "void (int,int)")
		.def("getAIAttitudeModifier", &CyPlayer::getAIAttitudeModifier, "int ()")
		.def("setAIAttitudeModifier", &CyPlayer::setAIAttitudeModifier, "void (int)")
		.def("changeAIAttitudeModifier", &CyPlayer::changeAIAttitudeModifier, "void (int)")
		.def("getExtraSpecialistCommerce", &CyPlayer::getExtraSpecialistCommerce, "int (int,int)")
		.def("changeExtraSpecialistCommerce", &CyPlayer::changeExtraSpecialistCommerce, "void (int,int,int)")
		.def("updateExtraSpecialistCommerce", &CyPlayer::updateExtraSpecialistCommerce, "void ()")
		.def("getSpecialistExtraYield", &CyPlayer::getSpecialistExtraYield, "int (int)")
		.def("changeSpecialistExtraYield", &CyPlayer::changeSpecialistExtraYield, "void (int,int)")
		.def("getFreeCityYield", &CyPlayer::getFreeCityYield, "int (int)")
		.def("changeFreeCityYield", &CyPlayer::changeFreeCityYield, "void (int,int)")
		.def("getTraitExtraCityDefense", &CyPlayer::getTraitExtraCityDefense, "int ()")
		.def("setTraitExtraCityDefense", &CyPlayer::setTraitExtraCityDefense, "void (int)")
		.def("changeTraitExtraCityDefense", &CyPlayer::changeTraitExtraCityDefense, "void (int)")
		.def("setHasTrait", &CyPlayer::setHasTrait, "void (int,bool)")
		.def("canLearnTrait", &CyPlayer::canLearnTrait, "bool (int)")

		// AIAndy: Build Lists
		.def("getBLNumLists", &CyPlayer::getBLNumLists, "int ()")
		.def("getBLIndexByID", &CyPlayer::getBLIndexByID, "int (int)")
		.def("getBLID", &CyPlayer::getBLID, "int (int)")
		.def("getBLListName", &CyPlayer::getBLListName, "string (int)")
		.def("getBLListLength", &CyPlayer::getBLListLength, "int (int)")
		.def("getBLOrder", &CyPlayer::getBLOrder, python::return_value_policy<python::reference_existing_object>(), "OrderData (int, int)")
		.def("writeBLToFile", &CyPlayer::writeBLToFile, "void ()")
		.def("getBLCurrentList", &CyPlayer::getBLCurrentList, "int ()")
		.def("setBLCurrentList", &CyPlayer::setBLCurrentList, "void (int)")
		.def("addBLList", &CyPlayer::addBLList, "void ()")
		.def("renameBLList", &CyPlayer::renameBLList, "void (int)")
		.def("removeBLList", &CyPlayer::removeBLList, "void (int)")

		;
}
