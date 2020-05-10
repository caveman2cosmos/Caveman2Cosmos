#include "CvGameCoreDLL.h"
#include "CyCity.h"
#include "CyPlayer.h"
#include "CyPlot.h"
#include "CyUnit.h"

//
// published python interface for CyPlayer
//

void CyPlayerPythonInterface1(python::class_<CyPlayer>& x)
{
	OutputDebugString("Python Extension Module - CyPlayerPythonInterface1\n");

	// set the docstring of the current module scope 
	python::scope().attr("__doc__") = "Civilization IV Player Class";
	x
		.def("isNone", &CyPlayer::isNone, "checks for a null player")

		.def( "changeLeader", &CyPlayer::changeLeader, "void ( int /*LeaderHeadTypes*/ eNewLeader ) - change leader of player")
		.def( "changeCiv", &CyPlayer::changeCiv, "void ( int /*CivilizationTypes*/ eNewCiv ) - change civilization of player" )
		.def( "setIsHuman", &CyPlayer::setIsHuman, "void ( bool bNewValue ) - set whether player is human" )
		.def( "setIsRebel", &CyPlayer::setIsRebel, "void ( bool bNewValue ) - set whether the player is considered a rebel" )
		.def( "isRebel", &CyPlayer::isRebel, "bool ( ) - true if player is a rebel" )

		.def("isDoNotBotherStatus", &CyPlayer::isDoNotBotherStatus, "bool (int /*PlayerTypes*/ playerID ) - set if player instructed not to contact playerID" )

		.def("startingPlotRange", &CyPlayer::startingPlotRange, "int ()")
		.def("startingPlotWithinRange", &CyPlayer::startingPlotWithinRange, "bool (CyPlot *pPlot, int /*PlayerTypes*/ ePlayer, int iRange, int iPass)")

		.def("findStartingPlot", &CyPlayer::findStartingPlot, python::return_value_policy<python::manage_new_object>(), "findStartingPlot(bool bRandomize) - Finds a starting plot for player")

		.def("initCity", &CyPlayer::initCity, python::return_value_policy<python::manage_new_object>(), "initCity( plotX, plotY ) - spawns a city at x,y")
		.def("acquireCity", &CyPlayer::acquireCity, "void (CyCity* pCity, bool bConquest, bool bTrade)")
		.def("killCities", &CyPlayer::killCities, "void ()")

		.def("getNewCityName", &CyPlayer::getNewCityName, "wstring ()")

		.def("initUnit", &CyPlayer::initUnit, python::return_value_policy<python::manage_new_object>(), "CyUnit* initUnit(UnitTypes iIndex, plotX, plotY, UnitAITypes iIndex)  - place Unit at X,Y   NOTE: Always use UnitAITypes.NO_UNITAI")
		.def("disbandUnit", &CyPlayer::disbandUnit, "void (bool bAnnounce)")

		.def("killUnits", &CyPlayer::killUnits, "void ()")
		.def("hasTrait", &CyPlayer::hasTrait, "bool hasTrait(int /*TraitTypes*/ iIndex) - returns True if player is the Trait Type.")
		.def("isHuman", &CyPlayer::isHuman, "bool ()")
		.def("isHumanDisabled", &CyPlayer::isHumanDisabled, "bool ()")
		.def("isBarbarian", &CyPlayer::isBarbarian, "bool () - returns True if player is a Barbarian")
		.def("isNPC", &CyPlayer::isNPC, "bool () - returns True if player is an NPC")
		.def("isHominid", &CyPlayer::isHominid, "bool () - returns True if player is a Hominid")
		.def("getName", &CyPlayer::getName, "str ()")

		.def("setName", &CyPlayer::setName, "void(std::wstring szNewValue)" ) // Exposed to Python
		.def("getNameForm", &CyPlayer::getNameForm, "str ()")
		.def("getNameKey", &CyPlayer::getNameKey, "str ()")
		.def("getCivilizationDescription", &CyPlayer::getCivilizationDescription, "str() - returns the Civilization Description String")
		.def("setCivName", &CyPlayer::setCivName, "void(std::wstring szNewDesc, std::wstring szNewShort, std::wstring szNewAdj)" ) // Exposed to Python
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
		.def("getBestAttackUnitKey", &CyPlayer::getBestAttackUnitKey, "str () - returns the name of the best attack unit")
		.def("getArtStyleType", &CyPlayer::getArtStyleType, " int () - Returns the ArtStyleType for this player (e.g. European)")
		.def("getUnitButton", &CyPlayer::getUnitButton, " string (int eUnit) - Returns the unit button for this player")

		.def("getCurrentInflationCostModifier", &CyPlayer::getCurrentInflationCostModifier, " int () - effective inflation cost modifer in the current turn")
		.def("getEquilibriumInflationCostModifier", &CyPlayer::getEquilibriumInflationCostModifier, " int () - equilibrium inflation cost modifer")

		.def("findBestFoundValue", &CyPlayer::findBestFoundValue, " int () - Finds best found value")

		.def("countNumCoastalCities", &CyPlayer::countNumCoastalCities, "int ()")
		.def("countNumCoastalCitiesByArea", &CyPlayer::countNumCoastalCitiesByArea, "(int (CyArea* pArea)")

		.def("countTotalCulture", &CyPlayer::countTotalCulture, "int ()")
		.def("countOwnedBonuses", &CyPlayer::countOwnedBonuses, "int (int (BonusTypes) eBonus) - ")
		.def("countUnimprovedBonuses", &CyPlayer::countUnimprovedBonuses, "int (int (CyArea* pArea, CyPlot* pFromPlot) - ")
		.def("countCityFeatures", &CyPlayer::countCityFeatures, "int (int /*FeatureTypes*/ eFeature) - Returns ?")
		.def("countNumBuildings", &CyPlayer::countNumBuildings, "int (int /*BuildingTypes*/ eBuilding) - Returns the number of buildings?")
		.def("countPotentialForeignTradeCities", &CyPlayer::countPotentialForeignTradeCities, "int (CyArea* pIgnoreArea) - Returns the number of potential foreign trade cities")
		.def("countPotentialForeignTradeCitiesConnected", &CyPlayer::countPotentialForeignTradeCitiesConnected, "int () - Returns the number of potential foreign trade cities which are also connected to this player's capital")

		.def("canContact", &CyPlayer::canContact, "bool (int ePlayer)")
		.def("contact", &CyPlayer::contact, "void (int ePlayer)")
		.def("canTradeWith", &CyPlayer::canTradeWith, "bool (int ePlayer)")
		.def("canTradeItem", &CyPlayer::canTradeItem, "bool (int ePlayer, bool bTestDenial)")
		.def("getTradeDenial", &CyPlayer::getTradeDenial, "DenialTypes (int eWhoTo, TradeData item)")
		.def("canTradeNetworkWith", &CyPlayer::canTradeNetworkWith, "bool (int (PlayerTypes) iPlayer)")
		.def("getNumAvailableBonuses", &CyPlayer::getNumAvailableBonuses, "int (int (BonusTypes) eBonus)")
		.def("getNumTradeableBonuses", &CyPlayer::getNumTradeableBonuses, "int (int (BonusTypes) eBonus)")
		.def("getNumTradeBonusImports", &CyPlayer::getNumTradeBonusImports, "int (int /*PlayerTypes*/ ePlayer)")
		.def("hasBonus", &CyPlayer::hasBonus, "int (int /*BonusType*/ ePlayer)")
		.def("canStopTradingWithTeam", &CyPlayer::canStopTradingWithTeam, "int (int /*TeamTypes*/ eTeam)")
		.def("stopTradingWithTeam", &CyPlayer::stopTradingWithTeam, "int (int /*TeamTypes*/ eTeam)")
		.def("killAllDeals", &CyPlayer::killAllDeals, "void ()")

		.def("findNewCapital", &CyPlayer::findNewCapital, "void ()")
		.def("getNumGovernmentCenters", &CyPlayer::getNumGovernmentCenters, "int ()")
		.def("canRaze", &CyPlayer::canRaze, "bool (CyCity pCity)")
		.def("raze", &CyPlayer::raze, "void (CyCity pCity)")
		.def("disband", &CyPlayer::disband, "void (CyCity pCity)")
		.def("canReceiveGoody", &CyPlayer::canReceiveGoody, "bool (CyPlot* pPlot, int /*GoodyTypes*/ eGoody, CyUnit* pUnit)")
		.def("receiveGoody", &CyPlayer::receiveGoody, "void (CyPlot* pPlot, int /*GoodyTypes*/ eGoody, CyUnit* pUnit)")
		.def("doGoody", &CyPlayer::doGoody, "void (CyPlot* pPlot, CyUnit* pUnit)")
		.def("canFound", &CyPlayer::canFound, "bool (int iX, int iY)")
		.def("found", &CyPlayer::found, "void (int iX, int iY)")
		.def("canTrain", &CyPlayer::canTrain, "bool (int eUnit, bool bContinue, bool bTestVisible)")
		.def("canConstruct", &CyPlayer::canConstruct, "bool (int /*BuildingTypes*/eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost)")
		.def("canCreate", &CyPlayer::canCreate, "bool (int /*ProjectTypes*/ eProject, bool bContinue, bool bTestVisible)")
		.def("canMaintain", &CyPlayer::canMaintain, "bool (int /*ProcessTypes*/ eProcess, bool bContinue)")
		.def("isProductionMaxedBuilding", &CyPlayer::isProductionMaxedBuilding, "int (int /*BuildingTypes*/ eBuilding, bool bAcquireCity)")
		.def("isProductionMaxedUnit", &CyPlayer::isProductionMaxedUnit, "int (int /*UnitTypes*/ eUnit)")
		.def("isProductionMaxedProject", &CyPlayer::isProductionMaxedProject, "int (int /*ProjectTypes*/ eProject)")
		.def("getUnitProductionNeeded", &CyPlayer::getUnitProductionNeeded, "int (int /*UnitTypes*/ iIndex)")
		.def("getBuildingProductionNeeded", &CyPlayer::getBuildingProductionNeeded, "int (int /*BuildingTypes*/ iIndex)")
		.def("getProjectProductionNeeded", &CyPlayer::getProjectProductionNeeded, "bool (int /*ProjectTypes*/ eProject, bool bContinue, bool bTestVisible)")
		.def("getBuildingPrereqBuilding", &CyPlayer::getBuildingPrereqBuilding, "int (int /*BuildingTypes*/ eBuilding, int /*BuildingTypes*/ ePrereqBuilding, iExtra) -")

		.def("removeBuilding", &CyPlayer::removeBuilding, "void (int /*BuildingTypes*/ eBuilding)")
		.def("canBuild", &CyPlayer::canBuild, "bool (CyPlot* pPlot, int (BuildTypes) eBuild, bool bTestEra, bool bTestVisible)")

		.def("calculateTotalYield", &CyPlayer::calculateTotalYield, "int (int /*YieldTypes*/ eYield) - Returns the total sum of all city yield")
		.def("calculateTotalExports", &CyPlayer::calculateTotalExports, "int (int /*YieldTypes*/ eYield) - Returns the total sum of all city gold generated for other civs via trade routes")
		.def("calculateTotalImports", &CyPlayer::calculateTotalImports, "int (int /*YieldTypes*/ eYield) - Returns the total sum of all city gold generated for this civ via trade routes with others")

		.def("calculateTotalCityHappiness", &CyPlayer::calculateTotalCityHappiness, "int () - Returns the total sum of all city Happiness values")
		.def("calculateTotalCityUnhappiness", &CyPlayer::calculateTotalCityUnhappiness, "int () - Returns the total sum of all city Unhappiness values")

		.def("calculateTotalCityHealthiness", &CyPlayer::calculateTotalCityHealthiness, "int () - Returns the total sum of all city Healthiness values")
		.def("calculateTotalCityUnhealthiness", &CyPlayer::calculateTotalCityUnhealthiness, "int () - Returns the total sum of all city Unhealthiness values")

		.def("calculateUnitCost", &CyPlayer::calculateUnitCost, "int ()")
		.def("calculateUnitSupply", &CyPlayer::calculateUnitSupply, "int ()")
		.def("calculatePreInflatedCosts", &CyPlayer::calculatePreInflatedCosts, "int ()")
		.def("calculateInflationRate", &CyPlayer::calculateInflationRate, "int ()")
		.def("calculateInflatedCosts", &CyPlayer::calculateInflatedCosts, "int ()")

		.def("getFreeUnitCountdown", &CyPlayer::getFreeUnitCountdown, "int ()")
		.def("setFreeUnitCountdown", &CyPlayer::setFreeUnitCountdown, "void ( int iValue )")

		.def("calculateGoldRate", &CyPlayer::calculateGoldRate, "int ()")
		.def("calculateTotalCommerce", &CyPlayer::calculateTotalCommerce, "int ()")
		.def("calculateResearchRate", &CyPlayer::calculateResearchRate, "int (int /*TechTypes*/ eTech)")
		.def("calculateBaseNetResearch", &CyPlayer::calculateBaseNetResearch, "int ()")
		.def("calculateResearchModifier", &CyPlayer::calculateResearchModifier, "int (int /*TechTypes*/ eTech)")
		.def("isResearch", &CyPlayer::isResearch, "bool ()")
		.def("canEverResearch", &CyPlayer::canEverResearch, "bool (int /*TechTypes*/ iIndex)")
		.def("canResearch", &CyPlayer::canResearch, "bool (int /*TechTypes*/ iIndex)")
		.def("getCurrentResearch", &CyPlayer::getCurrentResearch, "int ()")
		.def("isCurrentResearchRepeat", &CyPlayer::isCurrentResearchRepeat, "bool ()")
		.def("isNoResearchAvailable", &CyPlayer::isNoResearchAvailable, "bool ()")
		.def("getResearchTurnsLeft", &CyPlayer::getResearchTurnsLeft, "int (int /*TechTypes*/ eTech, bool bOverflow)")

		.def("isCivic", &CyPlayer::isCivic, "bool (int (CivicTypes) eCivic)")
		.def("canDoCivics", &CyPlayer::canDoCivics, "bool (int (CivicTypes) eCivic)")
		.def("canRevolution", &CyPlayer::canRevolution, "bool (int (CivicTypes*) paeNewCivics)")
		.def("revolution", &CyPlayer::revolution, "void (int (CivicTypes*) paeNewCivics, bool bForce)")
		.def("getCivicPercentAnger", &CyPlayer::getCivicPercentAnger, "int (int /*CivicTypes*/ eCivic)")

		.def("canDoReligion", &CyPlayer::canDoReligion, "int (int /*ReligionTypes*/ eReligion)")
		.def("canChangeReligion", &CyPlayer::canChangeReligion, "bool ()")
		.def("canConvert", &CyPlayer::canConvert, "bool (int /*ReligionTypes*/ iIndex)")
		.def("convert", &CyPlayer::convert, "void (int /*ReligionTypes*/ iIndex)")
		.def("hasHolyCity", &CyPlayer::hasHolyCity, "bool (int (ReligionTypes) eReligion)")

		.def("hasAnyHolyCity", &CyPlayer::hasAnyHolyCity, "bool ()")
		.def("hasStateReligionHolyCity", &CyPlayer::hasStateReligionHolyCity, "bool ()")
		.def("hasStateReligionShrine", &CyPlayer::hasStateReligionShrine, "bool ()")
		.def("countHolyCities", &CyPlayer::countHolyCities, "int () - Counts the # of holy cities this player has")
		.def("foundReligion", &CyPlayer::foundReligion, "void (int /*ReligionTypes*/ eReligion, int /*ReligionTypes*/ iSlotReligion, bool)")

		.def("hasHeadquarters", &CyPlayer::hasHeadquarters, "bool (int (CorporationTypes) eCorporation)")
		.def("countHeadquarters", &CyPlayer::countHeadquarters, "int () - Counts the # of headquarters this player has")
		.def("countCorporations", &CyPlayer::countCorporations, "int (CorporationTypes) - Counts the # of corporations this player has")
		.def("foundCorporation", &CyPlayer::foundCorporation, "void (int /*CorporationTypes*/ eCorporation)")

		.def("getCivicAnarchyLength", &CyPlayer::getCivicAnarchyLength, "int (int (CivicTypes*) paeNewCivics)")
		.def("getReligionAnarchyLength", &CyPlayer::getReligionAnarchyLength, "int ()")

		.def("unitsRequiredForGoldenAge", &CyPlayer::unitsRequiredForGoldenAge, "int ()")
		.def("unitsGoldenAgeCapable", &CyPlayer::unitsGoldenAgeCapable, "int ()")
		.def("unitsGoldenAgeReady", &CyPlayer::unitsGoldenAgeReady, "int ()")
		.def("greatPeopleThresholdNonMilitary", &CyPlayer::greatPeopleThresholdNonMilitary, "int ()")
		.def("greatPeopleThresholdMilitary", &CyPlayer::greatPeopleThresholdMilitary, "int ()")
		.def("specialistYield", &CyPlayer::specialistYield, "int (int (SpecialistTypes) eSpecialist, int (YieldTypes) eCommerce)")
		.def("specialistCommerce", &CyPlayer::specialistCommerce, "int (int (SpecialistTypes) eSpecialist, int (CommerceTypes) eCommerce)")

		.def("getStartingPlot", &CyPlayer::getStartingPlot, python::return_value_policy<python::manage_new_object>(), "CyPlot* ()")
		.def("setStartingPlot", &CyPlayer::setStartingPlot, "void (CyPlot*, bool) - sets the player's starting plot")
		.def("getTotalPopulation", &CyPlayer::getTotalPopulation, "int ()")
		.def("getAveragePopulation", &CyPlayer::getAveragePopulation, "int ()")
		.def("getRealPopulation", &CyPlayer::getRealPopulation, "long int ()")

		.def("getTotalLand", &CyPlayer::getTotalLand, "int ()")
		.def("getTotalLandScored", &CyPlayer::getTotalLandScored, "int ()")
		.def("getEffectiveGold", &CyPlayer::getEffectiveGold, "int ()")
		.def("getGold", &CyPlayer::getGold, "int ()")
		.def("getGreaterGold", &CyPlayer::getGreaterGold, "int ()")
		.def("setGold", &CyPlayer::setGold, "void (int iNewValue)")
		.def("setGreaterGold", &CyPlayer::setGreaterGold, "void (int iNewValue)")
		.def("changeGold", &CyPlayer::changeGold, "void (int iChange)")
		.def("changeGreaterGold", &CyPlayer::changeGreaterGold, "void (int iChange)")
		.def("getGoldPerTurn", &CyPlayer::getGoldPerTurn, "int ()")
	;
}
