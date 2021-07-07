#include "CvGameCoreDLL.h"
#include "CyCity.h"

//
// published python interface for CyCity
//

void CyCityPythonInterface2(python::class_<CyCity>& x)
{
	OutputDebugString("Python Extension Module - CyCityPythonInterface2\n");
	x
		.def("clearOrderQueue", &CyCity::clearOrderQueue, "void ()")
		.def("pushOrder", &CyCity::pushOrder, "void (OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bForce)")
		.def("popOrder", &CyCity::popOrder, "int (int iNum, bool bFinish, bool bChoose)")
		.def("getOrderQueueLength", &CyCity::getOrderQueueLength, "void ()")
		.def("getOrderFromQueue", &CyCity::getOrderFromQueue, python::return_value_policy<python::return_by_value>(), "OrderData* (int iIndex)")  // only use local

		.def("AI_isEmphasize", &CyCity::AI_isEmphasize, "bool (int iEmphasizeType)")
		.def("AI_countBestBuilds", &CyCity::AI_countBestBuilds, "int (CyArea* pArea)")
		.def("AI_cityValue", &CyCity::AI_cityValue, "int ()")

		.def("getScriptData", &CyCity::getScriptData, "str () - Get stored custom data (via pickle)")
		.def("setScriptData", &CyCity::setScriptData, "void (str) - Set stored custom data (via pickle)")

		.def("getBuildingYieldChange", &CyCity::getBuildingYieldChange, "int (int /*BuildingTypes*/ eBuilding, int /*YieldTypes*/ eYield)")
		.def("setBuildingYieldChange", &CyCity::setBuildingYieldChange, "void (int /*BuildingTypes*/ eBuilding, int /*YieldTypes*/ eYield, int iChange)")
		.def("getBuildingCommerceChange", &CyCity::getBuildingCommerceChange, "int (int /*BuildingTypes*/ eBuilding, int /*CommerceTypes*/ eCommerce)")
		.def("setBuildingCommerceChange", &CyCity::setBuildingCommerceChange, "void (int /*BuildingTypes*/ eBuilding, int /*CommerceTypes*/ eCommerce, int iChange)")
		.def("getBuildingHappyChange", &CyCity::getBuildingHappyChange, "int (int /*BuildingTypes*/ eBuilding)")
		.def("setBuildingHappyChange", &CyCity::setBuildingHappyChange, "void (int /*BuildingTypes*/ eBuilding, int iChange)")
		.def("getBuildingHealthChange", &CyCity::getBuildingHealthChange, "int (int /*BuildingTypes*/ eBuilding)")
		.def("setBuildingHealthChange", &CyCity::setBuildingHealthChange, "void (int /*BuildingTypes*/ eBuilding, int iChange)")

		.def("getLiberationPlayer", &CyCity::getLiberationPlayer, "int ()")
		//.def("liberate", &CyCity::liberate, "void ()")

// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
		.def("calculateTradeProfitTimes100", &CyCity::calculateTradeProfitTimes100, "int (CyCity) - returns the unrounded trade profit created by CyCity")
#endif
// BUG - Fractional Trade Routes - end
		.def("calculateTradeProfit", &CyCity::calculateTradeProfit, "int (CyCity) - returns the trade profit created by CyCity")
		.def("calculateTradeYield", &CyCity::calculateTradeYield, "int (YieldType, int iTradeProfit) - calculates Trade Yield")

		.def("getCommerceRate", &CyCity::getCommerceRate, "int (int /*CommerceTypes*/) - total Commerce rate")
		.def("getCommerceRateTimes100", &CyCity::getCommerceRateTimes100, "int (int /*CommerceTypes*/) - total Commerce rate")
		.def("getBaseCommerceRate", &CyCity::getBaseCommerceRate, "int (int /*CommerceTypes*/)")
		.def("getProductionToCommerceModifier", &CyCity::getProductionToCommerceModifier, "int (int /*CommerceTypes*/) - value of production to commerce modifier")
		.def("getBuildingCommerceByBuilding", &CyCity::getBuildingCommerceByBuilding, "int (int /*CommerceTypes*/, BuildingTypes) - total value of CommerceType from BuildingTypes")
		.def("getSpecialistCommerce", &CyCity::getSpecialistCommerce, "int (int /*CommerceTypes*/) - value of CommerceType adjustment from Specialists")
		.def("changeSpecialistCommerce", &CyCity::changeSpecialistCommerce, "void (int /*CommerceTypes*/, iChange) - adjusts Specialist contribution to CommerceType by iChange")
		.def("getReligionCommerceByReligion", &CyCity::getReligionCommerceByReligion, "int (int /*CommerceTypes*/, ReligionType) - CommerceType effect from ReligionType")
		.def("getCorporationCommerce", &CyCity::getCorporationCommerce, "int (int /*CommerceTypes*/) - effect on CommerceType by Corporation")
		.def("getCorporationCommerceByCorporation", &CyCity::getCorporationCommerceByCorporation, "int (int /*CommerceTypes*/, CorporationType) - CommerceType effect from CorporationType")
		.def("getCorporationYield", &CyCity::getCorporationYield, "int (int /*CommerceTypes*/) - effect on YieldTypes by Corporation")
		.def("getCorporationYieldByCorporation", &CyCity::getCorporationYieldByCorporation, "int (int /*YieldTypes*/, CorporationType) - YieldTypes effect from CorporationType")
		.def("getCommerceRateModifier", &CyCity::getCommerceRateModifier, "int (int /*CommerceTypes*/) - indicates the total rate modifier on CommerceType")
		.def("getCommerceHappinessByType", &CyCity::getCommerceHappinessByType, "int (int /*CommerceTypes*/) - happiness from CommerceType")
		.def("getDomainFreeExperience", &CyCity::getDomainFreeExperience, "int (int /*DomainTypes*/)")
		.def("getDomainProductionModifier", &CyCity::getDomainProductionModifier, "int (int /*DomainTypes*/)")
		.def("getCulture", &CyCity::getCulture, "int /*PlayerTypes*/ ()")
		.def("getCultureTimes100", &CyCity::getCultureTimes100, "int /*PlayerTypes*/ ()")
		.def("countTotalCultureTimes100", &CyCity::countTotalCultureTimes100, "int ()")
		.def("findHighestCulture", &CyCity::findHighestCulture, "PlayerTypes ()")
		.def("calculateCulturePercent", &CyCity::calculateCulturePercent, "int (int eIndex)")
		.def("setCulture", &CyCity::setCulture, "void (int /*PlayerTypes*/ eIndex, bool bPlots)")
		.def("setCultureTimes100", &CyCity::setCultureTimes100, "void (int /*PlayerTypes*/ eIndex, int iNewValue, bool bPlots)")
		.def("changeCulture", &CyCity::changeCulture, "void (int /*PlayerTypes*/ eIndex, int iChange, bool bPlots)")


		.def("isRevealed", &CyCity::isRevealed, "bool (int /*TeamTypes*/ eIndex, bool bDebug)")
		.def("setRevealed", &CyCity::setRevealed, "void (int /*TeamTypes*/ eIndex, bool bNewValue)")
		.def("getEspionageVisibility", &CyCity::getEspionageVisibility, "bool (int /*TeamTypes*/ eIndex)")
		.def("getName", &CyCity::getName, "string () - city name")
		.def("getNameForm", &CyCity::getNameForm, "string () - city name")
		.def("getNameKey", &CyCity::getNameKey, "string () - city name")
		.def("setName", &CyCity::setName, "void (TCHAR szNewValue, bool bFound) - sets the name to szNewValue")
		.def("isNoBonus", &CyCity::isNoBonus, "bool (int eIndex)")
		.def("changeNoBonusCount", &CyCity::changeNoBonusCount, "void (int eIndex, int iChange)")
		.def("getFreeBonus", &CyCity::getFreeBonus, "int (int eIndex)")
		.def("changeFreeBonus", &CyCity::changeFreeBonus, "void (int eIndex, int iChange)")
		.def("getNumBonuses", &CyCity::getNumBonuses, "int (PlayerID)")
		.def("hasBonus", &CyCity::hasBonus, "bool - (BonusID) - is BonusID connected to the city?")
		.def("getBuildingProduction", &CyCity::getBuildingProduction, "int (BuildingID) - current production towards BuildingID")
		.def("setBuildingProduction", &CyCity::setBuildingProduction, "void (BuildingID, iNewValue) - set progress towards BuildingID as iNewValue")
		.def("getBuildingProductionTime", &CyCity::getBuildingProductionTime, "int (int eIndex)")

		.def("getGreatPeopleUnitProgress", &CyCity::getGreatPeopleUnitProgress, "int (int /*UnitTypes*/ iIndex)")
		.def("setGreatPeopleUnitProgress", &CyCity::setGreatPeopleUnitProgress, "int (int /*UnitTypes*/ iIndex, int iNewValue)")
		.def("changeGreatPeopleUnitProgress", &CyCity::changeGreatPeopleUnitProgress, "int (int /*UnitTypes*/ iIndex, int iChange)")
		.def("getSpecialistCount", &CyCity::getSpecialistCount, "int (int /*SpecialistTypes*/ eIndex)")
		.def("getMaxSpecialistCount", &CyCity::getMaxSpecialistCount, "int (int /*SpecialistTypes*/ eIndex)")
		.def("isSpecialistValid", &CyCity::isSpecialistValid, "bool (int /*SpecialistTypes*/ eIndex, int iExtra)")
		.def("getForceSpecialistCount", &CyCity::getForceSpecialistCount, "int (int /*SpecialistTypes*/ eIndex)")
		.def("setForceSpecialistCount", &CyCity::setForceSpecialistCount, "int (int /*SpecialistTypes*/ eIndex, int iNewValue")
		.def("getFreeSpecialistCount", &CyCity::getFreeSpecialistCount, "int (int /*SpecialistTypes*/ eIndex")
		.def("changeFreeSpecialistCount", &CyCity::changeFreeSpecialistCount, "int (int /*SpecialistTypes*/ eIndex, iChange")
		.def("getAddedFreeSpecialistCount", &CyCity::getAddedFreeSpecialistCount, "int (int /*SpecialistTypes*/ eIndex")
		.def("getImprovementFreeSpecialists", &CyCity::getImprovementFreeSpecialists, "int (ImprovementID)")
		.def("changeImprovementFreeSpecialists", &CyCity::changeImprovementFreeSpecialists, "void (ImprovementID, iChange) - adjust ImprovementID free specialists by iChange")
		.def("getReligionInfluence", &CyCity::getReligionInfluence, "int (ReligionID) - value of influence from ReligionID")
		.def("changeReligionInfluence", &CyCity::changeReligionInfluence, "void (ReligionID, iChange) - adjust ReligionID influence by iChange")

		.def("getStateReligionHappiness", &CyCity::getStateReligionHappiness, "int (int /*ReligionTypes*/ ReligionID)")
		.def("changeStateReligionHappiness", &CyCity::changeStateReligionHappiness, "void (int /*ReligionTypes*/ ReligionID, iChange)")

		.def("getSpecialistFreeExperience", &CyCity::getSpecialistFreeExperience, "int ()")
		.def("getEspionageDefenseModifier", &CyCity::getEspionageDefenseModifier, "int ()")

		.def("isWorkingPlot", &CyCity::isWorkingPlot, "bool (iIndex) - true if a worker is working this city's pPlot")
		.def("setNumRealBuilding", &CyCity::setNumRealBuilding, "(BuildingID, iNum) - Sets number of buildings in this city of BuildingID type")
		.def("getNumRealBuilding", &CyCity::getNumRealBuilding, "int (BuildingID) - get # Actual building count")
		.def("getNumActiveBuilding", &CyCity::getNumActiveBuilding, "int (BuildingID) - get # Filters out disabled buildings")
		.def("isHasReligion", &CyCity::isHasReligion, "bool (ReligionID) - does city have ReligionID?")
		.def("setHasReligion", &CyCity::setHasReligion, "void (ReligionID, bool bNewValue, bool bAnnounce, bool bArrows) - religion begins to spread")
		.def("isHasCorporation", &CyCity::isHasCorporation, "bool (CorporationID) - does city have CorporationID?")
		.def("setHasCorporation", &CyCity::setHasCorporation, "void (CorporationID, bool bNewValue, bool bAnnounce, bool bArrows) - corporation begins to spread")
		.def("isActiveCorporation", &CyCity::isActiveCorporation, "bool (CorporationID) - does city have active CorporationID?")
		.def("getTradeCity", &CyCity::getTradeCity, python::return_value_policy<python::manage_new_object>(), "CyCity (int iIndex) - remove SpecialistType[iIndex]")
		.def("getTradeRoutes", &CyCity::getTradeRoutes, "int ()")


		/********************************************************************************/
		/**		REVOLUTION_MOD							03/29/09			jdog5000	*/
		/**																				*/
		/**		 																		*/
		/********************************************************************************/
		.def("getRevolutionIndex", &CyCity::getRevolutionIndex, "int ()")
		.def("setRevolutionIndex", &CyCity::setRevolutionIndex, "void ( int iNewValue )")
		.def("changeRevolutionIndex", &CyCity::changeRevolutionIndex, "void ( int iChange )" )

		.def("getLocalRevIndex", &CyCity::getLocalRevIndex, "int ()" )
		.def("setLocalRevIndex", &CyCity::setLocalRevIndex, "void ( int iNewValue )" )

		.def("getRevIndexAverage", &CyCity::getRevIndexAverage, "int ()" )
		.def("setRevIndexAverage", &CyCity::setRevIndexAverage, "void (int iNewValue)" )
		.def("updateRevIndexAverage", &CyCity::updateRevIndexAverage, "void ( )" )

		.def("getRevolutionCounter", &CyCity::getRevolutionCounter, "int ()")
		.def("setRevolutionCounter", &CyCity::setRevolutionCounter, "void ( int iNewValue )")
		.def("changeRevolutionCounter", &CyCity::changeRevolutionCounter, "void ( int iChange )" )

		.def("getReinforcementCounter", &CyCity::getReinforcementCounter, "int ()")
		.def("setReinforcementCounter", &CyCity::setReinforcementCounter, "void ( int iNewValue )")
		.def("changeReinforcementCounter", &CyCity::changeReinforcementCounter, "void ( int iChange )" )

		.def("getRevIndexPercentAnger", &CyCity::getRevIndexPercentAnger, "int ()")
		.def("getRevRequestAngerTimer", &CyCity::getRevRequestAngerTimer, "int () - Anger caused by refusing a request")
		.def("changeRevRequestAngerTimer", &CyCity::changeRevRequestAngerTimer, "void (int iChange) - adjust Rev Request anger timer by iChange")
		.def("changeRevSuccessTimer", &CyCity::changeRevSuccessTimer, "void (int iChange) - adjust Rev Request anger timer by iChange")

		.def("getNumRevolts", &CyCity::getNumRevolts, "int (PlayerTypes eIndex)")
		.def("changeNumRevolts", &CyCity::changeNumRevolts, "int (PlayerTypes eIndex, int iChange)" )
// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
		.def("calculateTradeProfitTimes100", &CyCity::calculateTradeProfitTimes100, "int (CyCity) - returns the unrounded trade profit created by CyCity")
#endif
// BUG - Fractional Trade Routes - end
// BUG - Production Decay - start
		.def("isBuildingProductionDecay", &CyCity::isBuildingProductionDecay, "bool (int eIndex)")
		.def("getBuildingProductionDecayTurns", &CyCity::getBuildingProductionDecayTurns, "int (int eIndex)")
// BUG - Production Decay - end
// BUG - Production Decay - start
		.def("getUnitProductionTime", &CyCity::getUnitProductionTime, "int (int eIndex)")
		.def("isUnitProductionDecay", &CyCity::isUnitProductionDecay, "bool (int eIndex)")
		.def("getUnitProductionDecayTurns", &CyCity::getUnitProductionDecayTurns, "int (int eIndex)")
// BUG - Production Decay - end

// BUG - Production Decay - start
		.def("isBuildingProductionDecay", &CyCity::isBuildingProductionDecay, "bool (int /*BuildingTypes*/ eIndex)")
		.def("getBuildingProductionDecayTurns", &CyCity::getBuildingProductionDecayTurns, "int (int /*BuildingTypes*/ eIndex)")
// BUG - Production Decay - end
		.def("getBuildingOriginalOwner", &CyCity::getBuildingOriginalOwner, "int (BuildingType) - index of original building owner")
		.def("getBuildingOriginalTime", &CyCity::getBuildingOriginalTime, "int (int BuildingType) - original build date")
		.def("setBuildingOriginalTime", &CyCity::setBuildingOriginalTime, "void (int iBuildingType, int iNewValue) - original build date")
		.def("getUnitProduction", &CyCity::getUnitProduction, "int (UnitID) - gets current production towards UnitID")
		.def("setUnitProduction", &CyCity::setUnitProduction, "void (UnitID, iNewValue) - sets production towards UnitID as iNewValue")
// BUG - Production Decay - start
		.def("getUnitProductionTime", &CyCity::getUnitProductionTime, "int (int /*UnitTypes*/ eIndex)")
		.def("isUnitProductionDecay", &CyCity::isUnitProductionDecay, "bool (int /*UnitTypes*/ eIndex)")
		.def("getUnitProductionDecayTurns", &CyCity::getUnitProductionDecayTurns, "int (int /*UnitTypes*/ eIndex)")
// BUG - Production Decay - end
// BUG - Project Production - start
		.def("getProjectProduction", &CyCity::getProjectProduction, "int (int /*ProjectTypes*/ eIndex)")
// BUG - Project Production - end

		.def("getArea", &CyCity::getArea, "int ()")
		.def("isWeLoveTheKingDay", &CyCity::isWeLoveTheKingDay, "bool ()")
		.def("setWeLoveTheKingDay", &CyCity::setWeLoveTheKingDay, "void (bool bWeLoveTheKingDay)")
		.def("calculateCorporateTaxes", &CyCity::calculateCorporateTaxes, "int ()")
		.def("getBonusCommerceRateModifier", &CyCity::getBonusCommerceRateModifier, "int (int /*CommerceTypes*/)")
		.def("changePowerCount", &CyCity::changePowerCount, "void (int iChange, bool bDirty)")

		.def("changeEventAnger", &CyCity::changeEventAnger, "void (int iChange)")
		.def("getNumPopulationEmployed", &CyCity::getNumPopulationEmployed, "int ()")

		.def("getBonusCommercePercentChanges", &CyCity::getBonusCommercePercentChanges, "int (eCommerce, eBuilding)")
		.def("isAutomatedCanBuild", &CyCity::isAutomatedCanBuild, "bool ()")
		.def("setAutomatedCanBuild", &CyCity::setAutomatedCanBuild, "void ()")

		.def("getProperties", &CyCity::getProperties, python::return_value_policy<python::reference_existing_object>(), "CvProperties ()")

		.def("getBuildingListFilterActive", &CyCity::getBuildingListFilterActive, "bool (int)")
		.def("setBuildingListFilterActive", &CyCity::setBuildingListFilterActive, "void (int,bool)")
		.def("getBuildingListGrouping", &CyCity::getBuildingListGrouping, "int ()")
		.def("setBuildingListGrouping", &CyCity::setBuildingListGrouping, "void (int)")
		.def("getBuildingListSorting", &CyCity::getBuildingListSorting, "int ()")
		.def("setBuildingListSorting", &CyCity::setBuildingListSorting, "void (int)")
		.def("getBuildingListGroupNum", &CyCity::getBuildingListGroupNum, "int ()")
		.def("getBuildingListNumInGroup", &CyCity::getBuildingListNumInGroup, "int (int)")
		.def("getBuildingListType", &CyCity::getBuildingListType, "int (int,int)")

		.def("setUnitListInvalid", &CyCity::setUnitListInvalid, "void ()")
		.def("getUnitListFilterActive", &CyCity::getUnitListFilterActive, "bool (int)")
		.def("setUnitListFilterActive", &CyCity::setUnitListFilterActive, "void (int,bool)")
		.def("getUnitListGrouping", &CyCity::getUnitListGrouping, "int ()")
		.def("setUnitListGrouping", &CyCity::setUnitListGrouping, "void (int)")
		.def("getUnitListSorting", &CyCity::getUnitListSorting, "int ()")
		.def("setUnitListSorting", &CyCity::setUnitListSorting, "void (int)")
		.def("getUnitListGroupNum", &CyCity::getUnitListGroupNum, "int ()")
		.def("getUnitListNumInGroup", &CyCity::getUnitListNumInGroup, "int (int)")
		.def("getUnitListType", &CyCity::getUnitListType, "int (int,int)")

		;
}
