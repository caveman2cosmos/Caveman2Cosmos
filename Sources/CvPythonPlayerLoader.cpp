#include "CvGameCoreDLL.h"
#include "CvPythonPlayerLoader.h"
#include "CyPlayer.h"
#include <boost/python/manage_new_object.hpp>
#include <boost/python/scope.hpp>

void CvPythonPlayerLoader::CyPlayerPythonInterface1(boost::python::class_<CyPlayer>& inst)
{
	OutputDebugString("Python Extension Module - CyPlayerPythonInterface1\n");

	// set the docstring of the current module scope
	boost::python::scope().attr("__doc__") = "Civilization IV Player Class";
	inst
		.def("changeLeader", &CyPlayer::changeLeader, "void (int /*LeaderHeadTypes*/ eNewLeader) - change leader of player")
		.def("changeCiv", &CyPlayer::changeCiv, "void (int /*CivilizationTypes*/ eNewCiv) - change civilization of player")
		.def("setIsRebel", &CyPlayer::setIsRebel, "void (bool bNewValue) - set whether the player is considered a rebel")
		.def("isRebel", &CyPlayer::isRebel, "bool () - true if player is a rebel")

		.def("isDoNotBotherStatus", &CyPlayer::isDoNotBotherStatus, "bool (int /*PlayerTypes*/ playerID) - set if player instructed not to contact playerID" )

		.def("startingPlotRange", &CyPlayer::startingPlotRange, "int ()")
		.def("startingPlotWithinRange", &CyPlayer::startingPlotWithinRange, "bool (CyPlot *pPlot, int /*PlayerTypes*/ ePlayer, int iRange, int iPass)")

		.def("initCity", &CyPlayer::initCity, boost::python::return_value_policy<boost::python::manage_new_object>(), "initCity(plotX, plotY) - spawns a city at x,y")
		.def("acquireCity", &CyPlayer::acquireCity, "void (CyCity* pCity, bool bConquest, bool bTrade)")
		.def("killCities", &CyPlayer::killCities, "void ()")

		.def("getNewCityName", &CyPlayer::getNewCityName, "wstring ()")

		.def("initUnit", &CyPlayer::initUnit, boost::python::return_value_policy<boost::python::manage_new_object>(), "CyUnit* initUnit(UnitTypes iIndex, plotX, plotY, UnitAITypes iIndex)  - place Unit at X,Y   NOTE: Always use UnitAITypes.NO_UNITAI")

		.def("killUnits", &CyPlayer::killUnits, "void ()")
		.def("hasTrait", &CyPlayer::hasTrait, "bool hasTrait(int /*TraitTypes*/ iIndex) - returns True if player is the Trait Type.")
		.def("isHuman", &CyPlayer::isHuman, "bool ()")
		.def("isHumanDisabled", &CyPlayer::isHumanDisabled, "bool ()")
		.def("isNPC", &CyPlayer::isNPC, "bool () - returns True if player is an NPC")
		.def("isHominid", &CyPlayer::isHominid, "bool () - returns True if player is a Hominid")
		.def("getName", &CyPlayer::getName, "str ()")

		.def("setCivName", &CyPlayer::setCivName)
		.def("setName", &CyPlayer::setName, "void (std::wstring szNewValue)")
		.def("getNameForm", &CyPlayer::getNameForm, "str ()")
		.def("getNameKey", &CyPlayer::getNameKey, "str ()")
		.def("getCivilizationDescription", &CyPlayer::getCivilizationDescription, "str() - returns the Civilization Description String")
		.def("getCivilizationShortDescription", &CyPlayer::getCivilizationShortDescription, "str() - returns the short Civilization Description")
		.def("getCivilizationDescriptionKey", &CyPlayer::getCivilizationDescriptionKey, "str() - returns the Civilization Description String")
		.def("getCivilizationShortDescriptionKey", &CyPlayer::getCivilizationShortDescriptionKey, "str() - returns the short Civilization Description")
		.def("getCivilizationAdjective", &CyPlayer::getCivilizationAdjective, "str() - returns the Civilization name in adjective form")
		.def("getCivilizationAdjectiveKey", &CyPlayer::getCivilizationAdjectiveKey, "str() - returns the Civilization name in adjective form")
		.def("getFlagDecal", &CyPlayer::getFlagDecal, "str() - returns the Civilization flag decal")
		.def("isWhiteFlag", &CyPlayer::isWhiteFlag, "bool () - Whether or not this player is using a custom texture flag (set in WBS)")
		.def("getStateReligionName", &CyPlayer::getStateReligionName, "str() - returns the name of the Civilizations State Religion")
		.def("getBestAttackUnitName", &CyPlayer::getBestAttackUnitName, "str () - returns the name of the best attack unit")
		.def("getWorstEnemyName", &CyPlayer::getWorstEnemyName, "str () - returns the name of the worst enemy")
		.def("getStateReligionKey", &CyPlayer::getStateReligionKey, "str() - returns the name of the Civilizations State Religion")
		.def("getArtStyleType", &CyPlayer::getArtStyleType, " int () - Returns the ArtStyleType for this player (e.g. European)")
		.def("getUnitButton", &CyPlayer::getUnitButton, " string (int eUnit) - Returns the unit button for this player")

		.def("countNumCoastalCities", &CyPlayer::countNumCoastalCities, "int ()")

		.def("countOwnedBonuses", &CyPlayer::countOwnedBonuses, "int (int (BonusTypes) eBonus) ")
		.def("countNumBuildings", &CyPlayer::countNumBuildings, "int (int /*BuildingTypes*/ eBuilding) - Returns the number of buildings")
		.def("hasBuilding", &CyPlayer::hasBuilding, "bool (int /*BuildingTypes*/ eBuilding)")

		.def("getBuildingPrereqBuilding", &CyPlayer::getBuildingPrereqBuilding, "int (int /*BuildingTypes*/ eBuilding, int /*BuildingTypes*/ ePrereqBuilding, int iExtra)")

		.def("canContact", &CyPlayer::canContact, "bool (int ePlayer)")
		.def("contact", &CyPlayer::contact, "void (int ePlayer)")
		.def("canTradeWith", &CyPlayer::canTradeWith, "bool (int ePlayer)")
		.def("canTradeItem", &CyPlayer::canTradeItem, "bool (int ePlayer, bool bTestDenial)")
		.def("getTradeDenial", &CyPlayer::getTradeDenial, "DenialTypes (int eWhoTo, TradeData item)")
		.def("canTradeNetworkWith", &CyPlayer::canTradeNetworkWith, "bool (int (PlayerTypes) iPlayer)")
		.def("getNumAvailableBonuses", &CyPlayer::getNumAvailableBonuses, "int (int (BonusTypes) eBonus)")
		.def("getNumTradeableBonuses", &CyPlayer::getNumTradeableBonuses, "int (int (BonusTypes) eBonus)")
		.def("hasBonus", &CyPlayer::hasBonus, "int (int /*BonusType*/ ePlayer)")

		.def("receiveGoody", &CyPlayer::receiveGoody, "void (CyPlot* pPlot, int /*GoodyTypes*/ eGoody, CyUnit* pUnit)")
		.def("doGoody", &CyPlayer::doGoody, "void (CyPlot* pPlot, CyUnit* pUnit)")
		.def("canFound", &CyPlayer::canFound, "bool (int iX, int iY)")
		.def("found", &CyPlayer::found, "void (int iX, int iY)")
		.def("canTrain", &CyPlayer::canTrain, "bool (int eUnit, bool bContinue, bool bTestVisible)")
		.def("canConstruct", &CyPlayer::canConstruct, "bool (int /*BuildingTypes*/eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost)")
		.def("canCreate", &CyPlayer::canCreate, "bool (int /*ProjectTypes*/ eProject, bool bContinue, bool bTestVisible)")
		.def("getUnitProductionNeeded", &CyPlayer::getUnitProductionNeeded, "int (int /*UnitTypes*/ iIndex)")
		.def("getBuildingProductionNeeded", &CyPlayer::getBuildingProductionNeeded, "int (int /*BuildingTypes*/ iIndex)")
		.def("getProjectProductionNeeded", &CyPlayer::getProjectProductionNeeded, "bool (int /*ProjectTypes*/ eProject, bool bContinue, bool bTestVisible)")

		.def("calculateTotalYield", &CyPlayer::calculateTotalYield, "int (int /*YieldTypes*/ eYield) - Returns the total sum of all city yield")
		.def("calculateTotalExports", &CyPlayer::calculateTotalExports, "int (int /*YieldTypes*/ eYield) - Returns the total sum of all city gold generated for other civs via trade routes")
		.def("calculateTotalImports", &CyPlayer::calculateTotalImports, "int (int /*YieldTypes*/ eYield) - Returns the total sum of all city gold generated for this civ via trade routes with others")

		.def("calculateTotalCityHappiness", &CyPlayer::calculateTotalCityHappiness, "int () - Returns the total sum of all city Happiness values")
		.def("calculateTotalCityUnhappiness", &CyPlayer::calculateTotalCityUnhappiness, "int () - Returns the total sum of all city Unhappiness values")

		.def("calculateTotalCityHealthiness", &CyPlayer::calculateTotalCityHealthiness, "int () - Returns the total sum of all city Healthiness values")
		.def("calculateTotalCityUnhealthiness", &CyPlayer::calculateTotalCityUnhealthiness, "int () - Returns the total sum of all city Unhealthiness values")

		.def("getFinalUnitUpkeep", &CyPlayer::getFinalUnitUpkeep, "int64 ()")
		.def("calculateUnitSupply", &CyPlayer::calculateUnitSupply, "int ()")
		.def("calculatePreInflatedCosts", &CyPlayer::calculatePreInflatedCosts, "int64 ()")
		.def("getFinalExpense", &CyPlayer::getFinalExpense, "int64 ()")

		.def("calculateGoldRate", &CyPlayer::calculateGoldRate, "int ()")
		.def("calculateTotalCommerce", &CyPlayer::calculateTotalCommerce, "int ()")
		.def("calculateResearchRate", &CyPlayer::calculateResearchRate, "int (int /*TechTypes*/ eTech)")
		.def("calculateBaseNetResearch", &CyPlayer::calculateBaseNetResearch, "int ()")
		.def("calculateResearchModifier", &CyPlayer::calculateResearchModifier, "int (int /*TechTypes*/ eTech)")
		.def("canResearch", &CyPlayer::canResearch, "bool (int iTech, bool bRightNow, bool bSpecialRequirements)")
		.def("getCurrentResearch", &CyPlayer::getCurrentResearch, "int ()")
		.def("isCurrentResearchRepeat", &CyPlayer::isCurrentResearchRepeat, "bool ()")
		.def("getResearchTurnsLeft", &CyPlayer::getResearchTurnsLeft, "int (int /*TechTypes*/ eTech, bool bOverflow)")

		.def("isCivic", &CyPlayer::isCivic, "bool (int (CivicTypes) eCivic)")
		.def("canDoCivics", &CyPlayer::canDoCivics, "bool (int (CivicTypes) eCivic)")
		.def("canRevolution", &CyPlayer::canRevolution, "bool ()")

		.def("canChangeReligion", &CyPlayer::canChangeReligion, "bool ()")
		.def("canConvert", &CyPlayer::canConvert, "bool (int /*ReligionTypes*/ iIndex)")
		.def("convert", &CyPlayer::convert, "void (int /*ReligionTypes*/ iIndex)")
		.def("hasHolyCity", &CyPlayer::hasHolyCity, "bool (int (ReligionTypes) eReligion)")

		.def("countHolyCities", &CyPlayer::countHolyCities, "int () - Counts the # of holy cities this player has")

		.def("hasHeadquarters", &CyPlayer::hasHeadquarters, "bool (int (CorporationTypes) eCorporation)")

		.def("getCivicAnarchyLength", &CyPlayer::getCivicAnarchyLength, "int (int (CivicTypes*) paeNewCivics)")
		.def("getReligionAnarchyLength", &CyPlayer::getReligionAnarchyLength, "int ()")

		.def("unitsRequiredForGoldenAge", &CyPlayer::unitsRequiredForGoldenAge, "int ()")
		.def("greatPeopleThresholdNonMilitary", &CyPlayer::greatPeopleThresholdNonMilitary, "int ()")
		.def("greatPeopleThresholdMilitary", &CyPlayer::greatPeopleThresholdMilitary, "int ()")
		.def("specialistYield", &CyPlayer::specialistYield, "int (int (SpecialistTypes) eSpecialist, int (YieldTypes) eCommerce)")

		.def("getStartingPlot", &CyPlayer::getStartingPlot, boost::python::return_value_policy<boost::python::manage_new_object>(), "CyPlot* ()")
		.def("setStartingPlot", &CyPlayer::setStartingPlot, "void (CyPlot*, bool) - sets the player's starting plot")
		.def("getTotalPopulation", &CyPlayer::getTotalPopulation, "int ()")
		.def("getRealPopulation", &CyPlayer::getRealPopulation, "long int ()")

		.def("getTotalLand", &CyPlayer::getTotalLand, "int ()")
		.def("getGold", &CyPlayer::getGold, "int64 ()")
		.def("setGold", &CyPlayer::setGold, "void (int iNewValue)")
		.def("changeGold", &CyPlayer::changeGold, "void (int iChange)")
		.def("getGoldPerTurn", &CyPlayer::getGoldPerTurn, "int ()")
	;
}
void CvPythonPlayerLoader::CyPlayerPythonInterface2(boost::python::class_<CyPlayer>& inst)
{
	OutputDebugString("Python Extension Module - CyPlayerPythonInterface2\n");

	// set the docstring of the current module scope
	boost::python::scope().attr("__doc__") = "Civilization IV Player Class";
	inst
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
		.def("setAlive", &CyPlayer::setAlive, "void (bool bNewValue, bool bActivateTurn)")
		.def("getStabilityIndex", &CyPlayer::getStabilityIndex, "int ( )")
		.def("changeStabilityIndex", &CyPlayer::changeStabilityIndex, "void ( int iChange )")
		.def("getStabilityIndexAverage", &CyPlayer::getStabilityIndexAverage, "int ( )")
		.def("updateStabilityIndexAverage", &CyPlayer::updateStabilityIndexAverage, "void ( )")
		.def("getRevolutionStabilityHistory", &CyPlayer::getRevolutionStabilityHistory, "int (int iTurn)")

		.def("isNonStateReligionCommerce", &CyPlayer::isNonStateReligionCommerce, "bool ()")

		.def("getBuildingCountWithUpgrades", &CyPlayer::getBuildingCountWithUpgrades, "int (int iBuilding)")
		.def("setColor", &CyPlayer::setColor, "void (int iColor)")
		.def("setHandicap", &CyPlayer::setHandicap, "void (int iNewVal, bool bAverageGameHandicap)")
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
		.def("getBLOrder", &CyPlayer::getBLOrder, boost::python::return_value_policy<boost::python::reference_existing_object>(), "OrderData (int, int)")
		.def("writeBLToFile", &CyPlayer::writeBLToFile, "void ()")
		.def("getBLCurrentList", &CyPlayer::getBLCurrentList, "int ()")
		.def("addBLList", &CyPlayer::addBLList, "void ()")
		.def("renameBLList", &CyPlayer::renameBLList, "void (int)")
		.def("removeBLList", &CyPlayer::removeBLList, "void (int)")
	;
}
void CvPythonPlayerLoader::CyPlayerPythonInterface3(boost::python::class_<CyPlayer>& inst)
{
	OutputDebugString("Python Extension Module - CyPlayerPythonInterface1\n");

	// set the docstring of the current module scope
	boost::python::scope().attr("__doc__") = "Civilization IV Player Class";
	inst
		.def("getAdvancedStartPoints", &CyPlayer::getAdvancedStartPoints, "int ()")
		.def("getAdvancedStartUnitCost", &CyPlayer::getAdvancedStartUnitCost, "int (int (UnitTypes) eUnit, bool bAdd, CyPlot* pPlot)")
		.def("getAdvancedStartCityCost", &CyPlayer::getAdvancedStartCityCost, "int (int (bool bAdd, CyPlot* pPlot)")
		.def("getAdvancedStartPopCost", &CyPlayer::getAdvancedStartPopCost, "int (int (bool bAdd, CyCity* pCity)")
		.def("getAdvancedStartCultureCost", &CyPlayer::getAdvancedStartCultureCost, "int (int (bool bAdd, CyCity* pCity)")
		.def("getAdvancedStartBuildingCost", &CyPlayer::getAdvancedStartBuildingCost, "int (int (BuildingTypes) eUnit, bool bAdd, CyCity* pCity)")
		.def("getAdvancedStartImprovementCost", &CyPlayer::getAdvancedStartImprovementCost, "int (int (ImprovementTypes) eImprovement, bool bAdd, CyPlot* pPlot)")
		.def("getAdvancedStartRouteCost", &CyPlayer::getAdvancedStartRouteCost, "int (int (RouteTypes) eUnit, bool bAdd, CyPlot* pPlot)")
		.def("getAdvancedStartTechCost", &CyPlayer::getAdvancedStartTechCost, "int (int (TechTypes) eUnit, bool bAdd)")
		.def("getAdvancedStartVisibilityCost", &CyPlayer::getAdvancedStartVisibilityCost, "int (CyPlot* pPlot)")

		.def("getEspionageSpending", &CyPlayer::getEspionageSpending, "int (PlayerTypes eIndex)")
		.def("canDoEspionageMission", &CyPlayer::canDoEspionageMission, "bool (EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, CyPlot* pPlot, int iExtraData)")
		.def("getEspionageMissionCost", &CyPlayer::getEspionageMissionCost, "int (EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, CyPlot* pPlot, int iExtraData)")
		.def("getEspionageSpendingWeightAgainstTeam", &CyPlayer::getEspionageSpendingWeightAgainstTeam, "int (TeamTypes eIndex)")
		.def("setEspionageSpendingWeightAgainstTeam", &CyPlayer::setEspionageSpendingWeightAgainstTeam, "void (TeamTypes eIndex, int iValue)")
		.def("changeEspionageSpendingWeightAgainstTeam", &CyPlayer::changeEspionageSpendingWeightAgainstTeam, "void (TeamTypes eIndex, int iChange)")

		.def("getGoldenAgeTurns", &CyPlayer::getGoldenAgeTurns, "int ()")
		.def("getGoldenAgeLength", &CyPlayer::getGoldenAgeLength, "int ()")
		.def("isGoldenAge", &CyPlayer::isGoldenAge, "bool ()")
		.def("changeGoldenAgeTurns", &CyPlayer::changeGoldenAgeTurns, "void (int iChange)")
		.def("changeNumUnitGoldenAges", &CyPlayer::changeNumUnitGoldenAges, "void (int iChange)")
		.def("getAnarchyTurns", &CyPlayer::getAnarchyTurns, "int ()")
		.def("isAnarchy", &CyPlayer::isAnarchy, "bool ()")
		.def("changeAnarchyTurns", &CyPlayer::changeAnarchyTurns, "void ()")
		.def("getAnarchyModifier", &CyPlayer::getAnarchyModifier, "int ()")
		.def("getGoldenAgeModifier", &CyPlayer::getGoldenAgeModifier, "int ()")
		.def("getGreatPeopleRateModifier", &CyPlayer::getGreatPeopleRateModifier, "int ()")
		.def("getGreatGeneralRateModifier", &CyPlayer::getGreatGeneralRateModifier, "int ()")
		.def("greatPeopleThresholdMilitary", &CyPlayer::greatPeopleThresholdMilitary, "int ()")
		.def("greatPeopleThresholdNonMilitary", &CyPlayer::greatPeopleThresholdNonMilitary, "int ()")
		.def("getGreatGeneralsThresholdModifier", &CyPlayer::getGreatGeneralsThresholdModifier, "int ()")
		.def("changeGreatGeneralsThresholdModifier", &CyPlayer::changeGreatGeneralsThresholdModifier, "void (int iChange)")

		.def("getFreeExperience", &CyPlayer::getFreeExperience, "int ()")
		.def("getFeatureProductionModifier", &CyPlayer::getFeatureProductionModifier, "int ()")
		.def("getWorkerSpeedModifier", &CyPlayer::getWorkerSpeedModifier, "int ()")
		.def("getMilitaryProductionModifier", &CyPlayer::getMilitaryProductionModifier, "int ()")
		.def("getSpaceProductionModifier", &CyPlayer::getSpaceProductionModifier, "int ()")

		.def("getRevIdxNational", &CyPlayer::getRevIdxNational, "int ()")
		.def("getRevIdxDistanceModifier", &CyPlayer::getRevIdxDistanceModifier, "int ()")

		.def("canFoundReligion", &CyPlayer::canFoundReligion, "bool ()")

		.def("hasIdleCity", &CyPlayer::hasIdleCity, "bool ()")
		.def("setForcedCityCycle", &CyPlayer::setForcedCityCycle, "void (bool bNewValue)")

		.def("getNumMilitaryUnits", &CyPlayer::getNumMilitaryUnits, "int ()")

		.def("getOverflowResearch", &CyPlayer::getOverflowResearch, "int ()")
		.def("isBuildingOnlyHealthy", &CyPlayer::isBuildingOnlyHealthy, "bool ()")

		.def("getTreasuryUpkeep", &CyPlayer::getTreasuryUpkeep, "int64 ()")
		.def("getTotalMaintenance", &CyPlayer::getTotalMaintenance, "int ()")
		.def("getLevelExperienceModifier", &CyPlayer::getLevelExperienceModifier, "int ()")

		.def("getExtraHealth", &CyPlayer::getExtraHealth, "int ()")
		.def("changeExtraHealth", &CyPlayer::changeExtraHealth, "void (int iChange)")

		.def("getExtraHappiness", &CyPlayer::getExtraHappiness, "int ()")
		.def("changeExtraHappiness", &CyPlayer::changeExtraHappiness, "void (int iChange)")

		.def("getBuildingHappiness", &CyPlayer::getBuildingHappiness, "int ()")
		.def("getWarWearinessPercentAnger", &CyPlayer::getWarWearinessPercentAnger, "int ()")
		.def("getWarWearinessModifier", &CyPlayer::getWarWearinessModifier, "int ()")
		.def("getCoastalTradeRoutes", &CyPlayer::getCoastalTradeRoutes, "int ()")
		.def("changeCoastalTradeRoutes", &CyPlayer::changeCoastalTradeRoutes, "void (int iChange)")
		.def("getTradeRoutes", &CyPlayer::getTradeRoutes, "int ()")

		.def("getRevolutionTimer", &CyPlayer::getRevolutionTimer, "int ()")
		.def("changeRevolutionTimer", &CyPlayer::changeRevolutionTimer, "void (int addTime)")

		.def("isStateReligion", &CyPlayer::isStateReligion, "bool ()")
		.def("getStateReligionHappiness", &CyPlayer::getStateReligionHappiness, "int ()")
		.def("getStateReligionUnitProductionModifier", &CyPlayer::getStateReligionUnitProductionModifier, "int ()")
		.def("changeStateReligionUnitProductionModifier", &CyPlayer::changeStateReligionUnitProductionModifier, "void (int iChange)")
		.def("getStateReligionBuildingProductionModifier", &CyPlayer::getStateReligionBuildingProductionModifier, "int ()")
		.def("changeStateReligionBuildingProductionModifier", &CyPlayer::changeStateReligionBuildingProductionModifier, "void (int iChange)")
		.def("getStateReligionFreeExperience", &CyPlayer::getStateReligionFreeExperience, "int ()")
		.def("getCapitalCity", &CyPlayer::getCapitalCity, boost::python::return_value_policy<boost::python::manage_new_object>(), "CyCity* (int iID)")
		.def("getCitiesLost", &CyPlayer::getCitiesLost, "int ()")

		.def("getAssets", &CyPlayer::getAssets, "int ()")
		.def("getPower", &CyPlayer::getPower, "int ()")
		.def("getUnitPower", &CyPlayer::getUnitPower, "int ()")
		.def("getPopScore", &CyPlayer::getPopScore, "int ()")
		.def("getLandScore", &CyPlayer::getLandScore, "int ()")

		.def("getSevoWondersScore", &CyPlayer::getSevoWondersScore, "int ()")

		.def("getWondersScore", &CyPlayer::getWondersScore, "int ()")
		.def("getTechScore", &CyPlayer::getTechScore, "int ()")
		.def("isMinorCiv", &CyPlayer::isMinorCiv, "bool ()")

		.def("isAlive", &CyPlayer::isAlive, "bool ()")
		.def("isEverAlive", &CyPlayer::isEverAlive, "bool ()")
		.def("isFoundedFirstCity", &CyPlayer::isFoundedFirstCity, "bool ()")
		.def("isStrike", &CyPlayer::isStrike, "bool ()")

		.def("getID", &CyPlayer::getID, "int ()")
		.def("getHandicapType", &CyPlayer::getHandicapType, "int ()")
		.def("getCivilizationType", &CyPlayer::getCivilizationType, "int ()")
		.def("getLeaderType", &CyPlayer::getLeaderType, "int ()")
		.def("getPersonalityType", &CyPlayer::getPersonalityType, "int ()")
		.def("setPersonalityType", &CyPlayer::setPersonalityType, "void (int /*LeaderHeadTypes*/ eNewValue)")
		.def("getCurrentEra", &CyPlayer::getCurrentEra, "int ()")
		.def("setCurrentEra", &CyPlayer::setCurrentEra, "void (int /*EraTypes*/ iNewValue)")
		.def("getStateReligion", &CyPlayer::getStateReligion, "int ()")
		.def("setLastStateReligion", &CyPlayer::setLastStateReligion, "void (int iReligionID) - Sets the player's state religion to iReligionID")
		.def("getTeam", &CyPlayer::getTeam, "int ()")
		.def("isTurnActive", &CyPlayer::isTurnActive, "bool ()")

		.def("getPlayerColor", &CyPlayer::getPlayerColor, "int (PlayerColorTypes) () - returns the color ID of the player")
		.def("getPlayerTextColorR", &CyPlayer::getPlayerTextColorR, "int ()")
		.def("getPlayerTextColorG", &CyPlayer::getPlayerTextColorG, "int ()")
		.def("getPlayerTextColorB", &CyPlayer::getPlayerTextColorB, "int ()")
		.def("getPlayerTextColorA", &CyPlayer::getPlayerTextColorA, "int ()")

		.def("getYieldRateModifier", &CyPlayer::getYieldRateModifier, "int (YieldTypes eIndex)")
		.def("getCommercePercent", &CyPlayer::getCommercePercent, "int (CommerceTypes eIndex)")
		.def("changeCommercePercent", &CyPlayer::changeCommercePercent, "int (CommerceTypes eIndex, int iChange)")
		.def("getCommerceRate", &CyPlayer::getCommerceRate, "int (CommerceTypes eIndex)")
		.def("getCommerceRateModifier", &CyPlayer::getCommerceRateModifier, "int (CommerceTypes eIndex)")
		.def("getCapitalCommerceRateModifier", &CyPlayer::getCapitalCommerceRateModifier, "int (CommerceTypes eIndex)")
		.def("getSpecialistExtraCommerce", &CyPlayer::getSpecialistExtraCommerce, "int (CommerceTypes eIndex)")
		.def("isCommerceFlexible", &CyPlayer::isCommerceFlexible, "bool (CommerceTypes eIndex)")

		.def("isFeatAccomplished", &CyPlayer::isFeatAccomplished, "bool ()")
		.def("setFeatAccomplished", &CyPlayer::setFeatAccomplished, "void ()")
		.def("isOption", &CyPlayer::isOption, "bool ()")
		.def("setOption", &CyPlayer::setOption, "void ()")
		.def("getVotes", &CyPlayer::getVotes, "int (int /*VoteTypes*/ eVote, int /*VoteSourceTypes*/ eVoteSource)")
		.def("isFullMember", &CyPlayer::isFullMember, "bool ()")
		.def("isVotingMember", &CyPlayer::isVotingMember, "bool ()")
		.def("isPlayable", &CyPlayer::isPlayable, "bool ()")
		.def("getBonusExport", &CyPlayer::getBonusExport, "int (CommerceTypes eIndex)")
		.def("getBonusImport", &CyPlayer::getBonusImport, "int (CommerceTypes eIndex)")

		.def("getImprovementCount", &CyPlayer::getImprovementCount, "int (int /*ImprovementTypes*/ iIndex)")

		.def("getExtraBuildingHappiness", &CyPlayer::getExtraBuildingHappiness, "int (int /*BuildingTypes*/ eIndex)")
		.def("getExtraBuildingHealth", &CyPlayer::getExtraBuildingHealth, "int (int /*BuildingTypes*/ eIndex)")

		.def("getUnitCount", &CyPlayer::getUnitCount, "int (int (UnitTypes) eIndex)")
		.def("isUnitMaxedOut", &CyPlayer::isUnitMaxedOut, "bool (int (UnitTypes) eIndex, int iExtra)")
		.def("getUnitMaking", &CyPlayer::getUnitMaking, "int (int (UnitTypes) eIndex)")
		.def("getUnitCountPlusMaking", &CyPlayer::getUnitCountPlusMaking, "int (int (UnitTypes) eIndex)")

		.def("getBuildingCount", &CyPlayer::getBuildingCount, "int (int /*BuildingTypes*/ eIndex)")
		.def("isBuildingMaxedOut", &CyPlayer::isBuildingMaxedOut, "bool (int /*BuildingTypes*/ iIndex, int iExtra)")
		.def("getBuildingCountPlusMaking", &CyPlayer::getBuildingCountPlusMaking, "int (int /*BuildingTypes*/ iIndex)")
		.def("canHurry", &CyPlayer::canHurry, "int (int (HurryTypes) eIndex)")

		.def("getHasReligionCount", &CyPlayer::getHasReligionCount)
		.def("getHasCorporationCount", &CyPlayer::getHasCorporationCount)

		.def("isSpecialistValid", &CyPlayer::isSpecialistValid, "bool (int /*SpecialistTypes*/ iIndex)")
		.def("isResearchingTech", &CyPlayer::isResearchingTech, "bool (int /*TechTypes*/ iIndex)")
		.def("getCivics", &CyPlayer::getCivics, "int /*CivicTypes*/ (int /*CivicOptionTypes*/ iIndex)")
		.def("getSingleCivicUpkeep", &CyPlayer::getSingleCivicUpkeep, "int (int /*CivicTypes*/ eCivic, bool bIgnoreAnarchy)")
		.def("getCivicUpkeep", &CyPlayer::getCivicUpkeep, "int (bool bIgnoreAnarchy)")
		.def("setCivics", &CyPlayer::setCivics, "void (int iCivicOptionType, int iCivicType) - Used to forcibly set civics with no anarchy")

		.def("getCombatExperience", &CyPlayer::getCombatExperience, "int () - Combat experience used to produce Warlords")
		.def("changeCombatExperience", &CyPlayer::changeCombatExperience, "void (int) - Combat experience used to produce Warlords")
		.def("setCombatExperience", &CyPlayer::setCombatExperience, "void (int) - Combat experience used to produce Warlords")

		.def("getQueuePosition", &CyPlayer::getQueuePosition, "int")
		.def("clearResearchQueue", &CyPlayer::clearResearchQueue, "void ()")
		.def("pushResearch", &CyPlayer::pushResearch, "void (int /*TechTypes*/ iIndex, bool bClear)")
		.def("getLengthResearchQueue", &CyPlayer::getLengthResearchQueue, "int ()")

		.def("addCityName", &CyPlayer::addCityName, "void (std::wstring szName)")
		.def("getNumCityNames", &CyPlayer::getNumCityNames, "int ()")
		.def("getCityName", &CyPlayer::getCityName, "std::wstring (int iIndex)")

		.def("cities", &CyPlayer::cities)
		.def("firstCity", &CyPlayer::firstCity, "tuple(CyCity, int iterOut) (bool bReverse) - gets the first city")
		.def("nextCity", &CyPlayer::nextCity, "tuple(CyCity, int iterOut) (int iterIn, bool bReverse) - gets the next city")
		.def("getNumCities", &CyPlayer::getNumCities, "int ()")
		.def("getCity", &CyPlayer::getCity, boost::python::return_value_policy<boost::python::manage_new_object>(), "CyCity* (int iID)")

		.def("units", &CyPlayer::units)
		.def("firstUnit", &CyPlayer::firstUnit, "tuple(CyUnit, int iterOut) (bool bReverse) - gets the first unit")
		.def("nextUnit", &CyPlayer::nextUnit, "tuple(CyUnit, int iterOut) (int iterIn, bool bReverse) - gets the next unit")
		.def("getNumUnits", &CyPlayer::getNumUnits, "int ()")
		.def("getUnit", &CyPlayer::getUnit, boost::python::return_value_policy<boost::python::manage_new_object>(), "CyUnit* (int iID)")

		.def("groups", &CyPlayer::groups)
		.def("getNumSelectionGroups", &CyPlayer::getNumSelectionGroups, "int ()")
		.def("getSelectionGroup", &CyPlayer::getSelectionGroup, boost::python::return_value_policy<boost::python::manage_new_object>(), "CvSelectionGroup* (int iID)")

		.def("trigger", &CyPlayer::trigger, "void (/*EventTriggerTypes*/int eEventTrigger)")
		.def("getEventOccured", &CyPlayer::getEventOccured, boost::python::return_value_policy<boost::python::reference_existing_object>(), "EventTriggeredData* (int /*EventTypes*/ eEvent)")
		.def("resetEventOccured", &CyPlayer::resetEventOccured, "void (int /*EventTypes*/ eEvent)")
		.def("getEventTriggered", &CyPlayer::getEventTriggered, boost::python::return_value_policy<boost::python::reference_existing_object>(), "EventTriggeredData* (int iID)")
		.def("initTriggeredData", &CyPlayer::initTriggeredData, boost::python::return_value_policy<boost::python::reference_existing_object>(), "EventTriggeredData* (int eEventTrigger, bool bFire, int iCityId, int iPlotX, int iPlotY, PlayerTypes eOtherPlayer, int iOtherPlayerCityId, ReligionTypes eReligion, CorporationTypes eCorporation, int iUnitId, BuildingTypes eBuilding)")
		.def("hasHeritage", &CyPlayer::hasHeritage, "bool hasHeritage(int iType)")
	;
}
