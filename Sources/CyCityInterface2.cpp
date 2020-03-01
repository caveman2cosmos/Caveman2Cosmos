#include "CvGameCoreDLL.h"

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

		.def("setWallOverridePoints", &CyCity::setWallOverridePoints, "setWallOverridePoints(const python::tuple& kPoints)")
		.def("getWallOverridePoints", &CyCity::getWallOverridePoints, "python::tuple getWallOverridePoints()")

		.def("AI_avoidGrowth", &CyCity::AI_avoidGrowth, "bool ()")
		.def("AI_isEmphasize", &CyCity::AI_isEmphasize, "bool (int iEmphasizeType)")
		.def("AI_countBestBuilds", &CyCity::AI_countBestBuilds, "int (CyArea* pArea)")
		.def("AI_cityValue", &CyCity::AI_cityValue, "int ()")

		.def("getScriptData", &CyCity::getScriptData, "str () - Get stored custom data (via pickle)")
		.def("setScriptData", &CyCity::setScriptData, "void (str) - Set stored custom data (via pickle)")

		.def("visiblePopulation", &CyCity::visiblePopulation, "int ()")

		.def("getBuildingYieldChange", &CyCity::getBuildingYieldChange, "int (int /*BuildingTypes*/ eBuilding, int /*YieldTypes*/ eYield)")
		.def("setBuildingYieldChange", &CyCity::setBuildingYieldChange, "void (int /*BuildingTypes*/ eBuilding, int /*YieldTypes*/ eYield, int iChange)")
		.def("getBuildingCommerceChange", &CyCity::getBuildingCommerceChange, "int (int /*BuildingTypes*/ eBuilding, int /*CommerceTypes*/ eCommerce)")
		.def("setBuildingCommerceChange", &CyCity::setBuildingCommerceChange, "void (int /*BuildingTypes*/ eBuilding, int /*CommerceTypes*/ eCommerce, int iChange)")
		.def("getBuildingHappyChange", &CyCity::getBuildingHappyChange, "int (int /*BuildingTypes*/ eBuilding)")
		.def("setBuildingHappyChange", &CyCity::setBuildingHappyChange, "void (int /*BuildingTypes*/ eBuilding, int iChange)")
		.def("getBuildingHealthChange", &CyCity::getBuildingHealthChange, "int (int /*BuildingTypes*/ eBuilding)")
		.def("setBuildingHealthChange", &CyCity::setBuildingHealthChange, "void (int /*BuildingTypes*/ eBuilding, int iChange)")

		.def("getLiberationPlayer", &CyCity::getLiberationPlayer, "int ()")
		.def("liberate", &CyCity::liberate, "void ()")
		
// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
		.def("calculateTradeProfitTimes100", &CyCity::calculateTradeProfitTimes100, "int (CyCity) - returns the unrounded trade profit created by CyCity")
#endif
// BUG - Fractional Trade Routes - end
		.def("calculateTradeProfit", &CyCity::calculateTradeProfit, "int (CyCity) - returns the trade profit created by CyCity")
		.def("calculateTradeYield", &CyCity::calculateTradeYield, "int (YieldType, int iTradeProfit) - calculates Trade Yield")

		.def("getExtraSpecialistYield", &CyCity::getExtraSpecialistYield, "int (int /*YieldTypes*/ eIndex)")
		.def("getExtraSpecialistYieldOfType", &CyCity::getExtraSpecialistYieldOfType, "int (int /*YieldTypes*/ eIndex, int /*SpecialistTypes*/ eSpecialist)")

		.def("getCommerceRate", &CyCity::getCommerceRate, "int (int /*CommerceTypes*/) - total Commerce rate")
		.def("getCommerceRateTimes100", &CyCity::getCommerceRateTimes100, "int (int /*CommerceTypes*/) - total Commerce rate")
		.def("getCommerceFromPercent", &CyCity::getCommerceFromPercent, "int (int /*CommerceTypes*/, int iYieldRate)")
		.def("getBaseCommerceRate", &CyCity::getBaseCommerceRate, "int (int /*CommerceTypes*/)")
		.def("getBaseCommerceRateTimes100", &CyCity::getBaseCommerceRateTimes100, "int (int /*CommerceTypes*/)")
		.def("getTotalCommerceRateModifier", &CyCity::getTotalCommerceRateModifier, "int (int /*CommerceTypes*/)")
		.def("getProductionToCommerceModifier", &CyCity::getProductionToCommerceModifier, "int (int /*CommerceTypes*/) - value of production to commerce modifier")
		.def("getBuildingCommerce", &CyCity::getBuildingCommerce, "int (int /*CommerceTypes*/) - total effect of cities buildings on CommerceTypes")
		.def("getBuildingCommerceByBuilding", &CyCity::getBuildingCommerceByBuilding, "int (int /*CommerceTypes*/, BuildingTypes) - total value of CommerceType from BuildingTypes")
// BUG - Building Additional Commerce - start
		.def("getAdditionalCommerceByBuilding", &CyCity::getAdditionalCommerceByBuilding, "int (int /*CommerceTypes*/, int /*BuildingTypes*/) - rounded change of CommerceType from adding one BuildingType")
		.def("getAdditionalCommerceTimes100ByBuilding", &CyCity::getAdditionalCommerceTimes100ByBuilding, "int (int /*CommerceTypes*/, int /*BuildingTypes*/) - total change of CommerceType from adding one BuildingType")
		.def("getAdditionalBaseCommerceRateByBuilding", &CyCity::getAdditionalBaseCommerceRateByBuilding, "int (int /*CommerceTypes*/, int /*BuildingTypes*/) - base rate change of CommerceType from adding one BuildingType")
		.def("getAdditionalCommerceRateModifierByBuilding", &CyCity::getAdditionalCommerceRateModifierByBuilding, "int (int /*CommerceTypes*/, int /*BuildingTypes*/) - rate modifier change of CommerceType from adding one BuildingType")
// BUG - Building Additional Commerce - end
		.def("getSpecialistCommerce", &CyCity::getSpecialistCommerce, "int (int /*CommerceTypes*/) - value of CommerceType adjustment from Specialists")
		.def("changeSpecialistCommerce", &CyCity::changeSpecialistCommerce, "void (int /*CommerceTypes*/, iChange) - adjusts Specialist contribution to CommerceType by iChange")
		.def("getReligionCommerce", &CyCity::getReligionCommerce, "int (int /*CommerceTypes*/) - effect on CommerceType by Religions")
		.def("getReligionCommerceByReligion", &CyCity::getReligionCommerceByReligion, "int (int /*CommerceTypes*/, ReligionType) - CommerceType effect from ReligionType")
		.def("getCorporationCommerce", &CyCity::getCorporationCommerce, "int (int /*CommerceTypes*/) - effect on CommerceType by Corporation")
		.def("getCorporationCommerceByCorporation", &CyCity::getCorporationCommerceByCorporation, "int (int /*CommerceTypes*/, CorporationType) - CommerceType effect from CorporationType")
		.def("getCorporationYield", &CyCity::getCorporationYield, "int (int /*CommerceTypes*/) - effect on YieldTypes by Corporation")
		.def("getCorporationYieldByCorporation", &CyCity::getCorporationYieldByCorporation, "int (int /*YieldTypes*/, CorporationType) - YieldTypes effect from CorporationType")
		.def("getCommerceRateModifier", &CyCity::getCommerceRateModifier, "int (int /*CommerceTypes*/) - indicates the total rate modifier on CommerceType")
		.def("getCommerceHappinessPer", &CyCity::getCommerceHappinessPer, "int (int /*CommerceTypes*/) - happiness from each level of entertainment")
		.def("getCommerceHappinessByType", &CyCity::getCommerceHappinessByType, "int (int /*CommerceTypes*/) - happiness from CommerceType")
		.def("getCommerceHappiness", &CyCity::getCommerceHappiness, "int () - happiness from all CommerceTypes")
		.def("getDomainFreeExperience", &CyCity::getDomainFreeExperience, "int (int /*DomainTypes*/)")
		.def("getDomainProductionModifier", &CyCity::getDomainProductionModifier, "int (int /*DomainTypes*/)")
		.def("getCulture", &CyCity::getCulture, "int /*PlayerTypes*/ ()")
		.def("getCultureTimes100", &CyCity::getCultureTimes100, "int /*PlayerTypes*/ ()")
		.def("countTotalCultureTimes100", &CyCity::countTotalCultureTimes100, "int ()")
		.def("findHighestCulture", &CyCity::findHighestCulture, "PlayerTypes ()")
		.def("calculateCulturePercent", &CyCity::calculateCulturePercent, "int (int eIndex)")
		.def("calculateTeamCulturePercent", &CyCity::calculateTeamCulturePercent, "int /*TeamTypes*/ ()")
		.def("setCulture", &CyCity::setCulture, "void (int /*PlayerTypes*/ eIndex, bool bPlots)")
		.def("setCultureTimes100", &CyCity::setCultureTimes100, "void (int /*PlayerTypes*/ eIndex, int iNewValue, bool bPlots)")
		.def("changeCulture", &CyCity::changeCulture, "void (int /*PlayerTypes*/ eIndex, int iChange, bool bPlots)")
		.def("changeCultureTimes100", &CyCity::changeCultureTimes100, "void (int /*PlayerTypes*/ eIndex, int iChange, bool bPlots)")

		.def("isTradeRoute", &CyCity::isTradeRoute, "bool ()")
		.def("isEverOwned", &CyCity::isEverOwned, "bool ()")

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
		.def("hasVicinityBonus", &CyCity::hasVicinityBonus, "bool - (BonusID)")
		.def("getBuildingProduction", &CyCity::getBuildingProduction, "int (BuildingID) - current production towards BuildingID")
		.def("setBuildingProduction", &CyCity::setBuildingProduction, "void (BuildingID, iNewValue) - set progress towards BuildingID as iNewValue")
		.def("changeBuildingProduction", &CyCity::changeBuildingProduction, "void (BuildingID, iChange) - adjusts progress towards BuildingID by iChange")
		.def("getBuildingProductionTime", &CyCity::getBuildingProductionTime, "int (int eIndex)")
		.def("setBuildingProductionTime", &CyCity::setBuildingProductionTime, "int (int eIndex, int iNewValue)")
		.def("changeBuildingProductionTime", &CyCity::changeBuildingProductionTime, "int (int eIndex, int iChange)")

		.def("getGreatPeopleUnitRate", &CyCity::getGreatPeopleUnitRate, "int (int /*UnitTypes*/ iIndex)")
		.def("getGreatPeopleUnitProgress", &CyCity::getGreatPeopleUnitProgress, "int (int /*UnitTypes*/ iIndex)")
		.def("setGreatPeopleUnitProgress", &CyCity::setGreatPeopleUnitProgress, "int (int /*UnitTypes*/ iIndex, int iNewValue)")
		.def("changeGreatPeopleUnitProgress", &CyCity::changeGreatPeopleUnitProgress, "int (int /*UnitTypes*/ iIndex, int iChange)")
		.def("getSpecialistCount", &CyCity::getSpecialistCount, "int (int /*SpecialistTypes*/ eIndex)")
		.def("alterSpecialistCount", &CyCity::alterSpecialistCount, "int (int /*SpecialistTypes*/ eIndex, int iChange)")
		.def("getMaxSpecialistCount", &CyCity::getMaxSpecialistCount, "int (int /*SpecialistTypes*/ eIndex)")
		.def("isSpecialistValid", &CyCity::isSpecialistValid, "bool (int /*SpecialistTypes*/ eIndex, int iExtra)")
		.def("getForceSpecialistCount", &CyCity::getForceSpecialistCount, "int (int /*SpecialistTypes*/ eIndex)")
		.def("isSpecialistForced", &CyCity::isSpecialistForced, "bool ()")
		.def("setForceSpecialistCount", &CyCity::setForceSpecialistCount, "int (int /*SpecialistTypes*/ eIndex, int iNewValue")
		.def("changeForceSpecialistCount", &CyCity::changeForceSpecialistCount, "int (int /*SpecialistTypes*/ eIndex, int iChange")
		.def("getFreeSpecialistCount", &CyCity::getFreeSpecialistCount, "int (int /*SpecialistTypes*/ eIndex")
		.def("changeFreeSpecialistCount", &CyCity::changeFreeSpecialistCount, "int (int /*SpecialistTypes*/ eIndex, iChange")
		.def("getAddedFreeSpecialistCount", &CyCity::getAddedFreeSpecialistCount, "int (int /*SpecialistTypes*/ eIndex")
		.def("getImprovementFreeSpecialists", &CyCity::getImprovementFreeSpecialists, "int (ImprovementID)")
		.def("changeImprovementFreeSpecialists", &CyCity::changeImprovementFreeSpecialists, "void (ImprovementID, iChange) - adjust ImprovementID free specialists by iChange")
		.def("getReligionInfluence", &CyCity::getReligionInfluence, "int (ReligionID) - value of influence from ReligionID")
		.def("changeReligionInfluence", &CyCity::changeReligionInfluence, "void (ReligionID, iChange) - adjust ReligionID influence by iChange")

		.def("getCurrentStateReligionHappiness", &CyCity::getCurrentStateReligionHappiness, "int ()")
		.def("getStateReligionHappiness", &CyCity::getStateReligionHappiness, "int (int /*ReligionTypes*/ ReligionID)")
		.def("changeStateReligionHappiness", &CyCity::changeStateReligionHappiness, "void (int /*ReligionTypes*/ ReligionID, iChange)")

		.def("getUnitCombatFreeExperience", &CyCity::getUnitCombatFreeExperience, "int (int /*UnitCombatTypes*/ eIndex)")
		.def("getFreePromotionCount", &CyCity::getFreePromotionCount, "int (int /*PromotionTypes*/ eIndex)")
		.def("isFreePromotion", &CyCity::isFreePromotion, "bool (int /*PromotionTypes*/ eIndex)")
		.def("getSpecialistFreeExperience", &CyCity::getSpecialistFreeExperience, "int ()")
		.def("getEspionageDefenseModifier", &CyCity::getEspionageDefenseModifier, "int ()")

		.def("isWorkingPlotByIndex", &CyCity::isWorkingPlotByIndex, "bool (iIndex) - true if a worker is working this city's plot iIndex")
		.def("isWorkingPlot", &CyCity::isWorkingPlot, "bool (iIndex) - true if a worker is working this city's pPlot")
		.def("alterWorkingPlot", &CyCity::alterWorkingPlot, "void (iIndex)")	
		.def("getNumRealBuilding", &CyCity::getNumRealBuilding, "int (BuildingID) - get # real building of this type")
		.def("setNumRealBuilding", &CyCity::setNumRealBuilding, "(BuildingID, iNum) - Sets number of buildings in this city of BuildingID type")
		.def("getNumFreeBuilding", &CyCity::getNumFreeBuilding, "int (BuildingID) - # of free Building ID (ie: from a Wonder)")
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
		.def("changeLocalRevIndex", &CyCity::changeLocalRevIndex, "void ( int iChange )" )

		.def("getRevIndexAverage", &CyCity::getRevIndexAverage, "int ()" )
		.def("setRevIndexAverage", &CyCity::setRevIndexAverage, "void (int iNewValue)" )
		.def("updateRevIndexAverage", &CyCity::updateRevIndexAverage, "void ( )" )

		.def("getRevolutionCounter", &CyCity::getRevolutionCounter, "int ()")
		.def("setRevolutionCounter", &CyCity::setRevolutionCounter, "void ( int iNewValue )")
		.def("changeRevolutionCounter", &CyCity::changeRevolutionCounter, "void ( int iChange )" )

		.def("getReinforcementCounter", &CyCity::getReinforcementCounter, "int ()")
		.def("setReinforcementCounter", &CyCity::setReinforcementCounter, "void ( int iNewValue )")
		.def("changeReinforcementCounter", &CyCity::changeReinforcementCounter, "void ( int iChange )" )

		.def("isRecentlyAcquired", &CyCity::isRecentlyAcquired, "bool ()")
		/********************************************************************************/
		/**		REVOLUTION_MOD							END								*/
		/********************************************************************************/
		/********************************************************************************/
		/**		REVOLUTION_MOD							4/19/08				jdog5000	*/
		/**																				*/
		/**																				*/
		/********************************************************************************/
		.def("getRevRequestPercentAnger", &CyCity::getRevRequestPercentAnger, "int ()")
		.def("getRevIndexPercentAnger", &CyCity::getRevIndexPercentAnger, "int ()")
		/********************************************************************************/
		/**		REVOLUTION_MOD							END								*/
		/********************************************************************************/
		/********************************************************************************/
		/**		REVOLUTION_MOD							04/28/08			jdog5000	*/
		/**																				*/
		/**																				*/
		/********************************************************************************/
		.def("getRevRequestAngerTimer", &CyCity::getRevRequestAngerTimer, "int () - Anger caused by refusing a request")
		.def("changeRevRequestAngerTimer", &CyCity::changeRevRequestAngerTimer, "void (int iChange) - adjust Rev Request anger timer by iChange")
		.def("getRevSuccessTimer", &CyCity::getRevSuccessTimer, "int () - Anger caused by refusing a request")
		.def("changeRevSuccessTimer", &CyCity::changeRevSuccessTimer, "void (int iChange) - adjust Rev Request anger timer by iChange")
		/********************************************************************************/
		/**		REVOLUTION_MOD							END								*/
		/********************************************************************************/
		/********************************************************************************/
		/**		REVOLUTION_MOD							08/23/08			jdog5000	*/
		/**																				*/
		/**		 																		*/
		/********************************************************************************/
		.def("getNumRevolts", &CyCity::getNumRevolts, "int (PlayerTypes eIndex)")
		.def("changeNumRevolts", &CyCity::changeNumRevolts, "int (PlayerTypes eIndex, int iChange)" )
		/********************************************************************************/
		/**		REVOLUTION_MOD							END								*/
		/********************************************************************************/
		/************************************************************************************************/
		/* REVOLUTION_MOD                         02/01/09                                jdog5000      */
		/*                                                                                              */
		/*                                                                                              */
		/************************************************************************************************/
		.def("AI_bestUnit", &CyCity::AI_bestUnit, "int /*UnitTypes*/ ()")
		.def("AI_bestUnitAI", &CyCity::AI_bestUnitAI, "int /*UnitTypes*/ (int iUnitAIType)")
		/************************************************************************************************/
		/* REVOLUTION_MOD                          END                                                  */
		/************************************************************************************************/
// BUG - Building Additional Great People - start
		.def("getAdditionalGreatPeopleRateByBuilding", &CyCity::getAdditionalGreatPeopleRateByBuilding, "int (int /*BuildingTypes*/)")
		.def("getAdditionalBaseGreatPeopleRateByBuilding", &CyCity::getAdditionalBaseGreatPeopleRateByBuilding, "int (int /*BuildingTypes*/)")
		.def("getAdditionalGreatPeopleRateModifierByBuilding", &CyCity::getAdditionalGreatPeopleRateModifierByBuilding, "int (int /*BuildingTypes*/)")
// BUG - Building Additional Great People - end
// BUG - Building Saved Maintenance - start
		.def("getSavedMaintenanceByBuilding", &CyCity::getSavedMaintenanceByBuilding, "int (int /*BuildingTypes*/)")
		.def("getSavedMaintenanceTimes100ByBuilding", &CyCity::getSavedMaintenanceTimes100ByBuilding, "int (int /*BuildingTypes*/)")
// BUG - Building Saved Maintenance - end
// BUG - Building Additional Yield - start
		.def("getAdditionalYieldByBuilding", &CyCity::getAdditionalYieldByBuilding, "int (int /*YieldTypes*/, int /*BuildingTypes*/) - total change of YieldType from adding one BuildingType")
		.def("getAdditionalBaseYieldRateByBuilding", &CyCity::getAdditionalBaseYieldRateByBuilding, "int (int /*YieldTypes*/, int /*BuildingTypes*/) - base rate change of YieldType from adding one BuildingType")
		.def("getAdditionalYieldRateModifierByBuilding", &CyCity::getAdditionalYieldRateModifierByBuilding, "int (int /*YieldTypes*/, int /*BuildingTypes*/) - rate modifier change of YieldType from adding one BuildingType")
// BUG - Building Additional Yield - end
// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
		.def("calculateTradeProfitTimes100", &CyCity::calculateTradeProfitTimes100, "int (CyCity) - returns the unrounded trade profit created by CyCity")
#endif
// BUG - Fractional Trade Routes - end
// BUG - Building Additional Commerce - start
		.def("getAdditionalCommerceByBuilding", &CyCity::getAdditionalCommerceByBuilding, "int (int /*CommerceTypes*/, int /*BuildingTypes*/) - rounded change of CommerceType from adding one BuildingType")
		.def("getAdditionalCommerceTimes100ByBuilding", &CyCity::getAdditionalCommerceTimes100ByBuilding, "int (int /*CommerceTypes*/, int /*BuildingTypes*/) - total change of CommerceType from adding one BuildingType")
		.def("getAdditionalBaseCommerceRateByBuilding", &CyCity::getAdditionalBaseCommerceRateByBuilding, "int (int /*CommerceTypes*/, int /*BuildingTypes*/) - base rate change of CommerceType from adding one BuildingType")
		.def("getAdditionalCommerceRateModifierByBuilding", &CyCity::getAdditionalCommerceRateModifierByBuilding, "int (int /*CommerceTypes*/, int /*BuildingTypes*/) - rate modifier change of CommerceType from adding one BuildingType")
// BUG - Building Additional Commerce - end
// BUG - Production Decay - start
		.def("isBuildingProductionDecay", &CyCity::isBuildingProductionDecay, "bool (int eIndex)")
		.def("getBuildingProductionDecay", &CyCity::getBuildingProductionDecay, "int (int eIndex)")
		.def("getBuildingProductionDecayTurns", &CyCity::getBuildingProductionDecayTurns, "int (int eIndex)")
// BUG - Production Decay - end
// BUG - Production Decay - start
		.def("getUnitProductionTime", &CyCity::getUnitProductionTime, "int (int eIndex)")
		.def("setUnitProductionTime", &CyCity::setUnitProductionTime, "int (int eIndex, int iNewValue)")
		.def("changeUnitProductionTime", &CyCity::changeUnitProductionTime, "int (int eIndex, int iChange)")
		.def("isUnitProductionDecay", &CyCity::isUnitProductionDecay, "bool (int eIndex)")
		.def("getUnitProductionDecay", &CyCity::getUnitProductionDecay, "int (int eIndex)")
		.def("getUnitProductionDecayTurns", &CyCity::getUnitProductionDecayTurns, "int (int eIndex)")
// BUG - Production Decay - end

// BUG - Production Decay - start
		.def("isBuildingProductionDecay", &CyCity::isBuildingProductionDecay, "bool (int /*BuildingTypes*/ eIndex)")
		.def("getBuildingProductionDecay", &CyCity::getBuildingProductionDecay, "int (int /*BuildingTypes*/ eIndex)")
		.def("getBuildingProductionDecayTurns", &CyCity::getBuildingProductionDecayTurns, "int (int /*BuildingTypes*/ eIndex)")
// BUG - Production Decay - end
		.def("getBuildingOriginalOwner", &CyCity::getBuildingOriginalOwner, "int (BuildingType) - index of original building owner")
		.def("getBuildingOriginalTime", &CyCity::getBuildingOriginalTime, "int (BuildingType) - original build date")
		.def("getUnitProduction", &CyCity::getUnitProduction, "int (UnitID) - gets current production towards UnitID")
		.def("setUnitProduction", &CyCity::setUnitProduction, "void (UnitID, iNewValue) - sets production towards UnitID as iNewValue")
		.def("changeUnitProduction", &CyCity::changeUnitProduction, "void (UnitID, iChange) - adjusts production towards UnitID by iChange")
// BUG - Production Decay - start
		.def("getUnitProductionTime", &CyCity::getUnitProductionTime, "int (int /*UnitTypes*/ eIndex)")
		.def("setUnitProductionTime", &CyCity::setUnitProductionTime, "int (int /*UnitTypes*/ eIndex, int iNewValue)")
		.def("changeUnitProductionTime", &CyCity::changeUnitProductionTime, "int (int /*UnitTypes*/ eIndex, int iChange)")
		.def("isUnitProductionDecay", &CyCity::isUnitProductionDecay, "bool (int /*UnitTypes*/ eIndex)")
		.def("getUnitProductionDecay", &CyCity::getUnitProductionDecay, "int (int /*UnitTypes*/ eIndex)")
		.def("getUnitProductionDecayTurns", &CyCity::getUnitProductionDecayTurns, "int (int /*UnitTypes*/ eIndex)")
// BUG - Production Decay - end
// BUG - Project Production - start
		.def("getProjectProduction", &CyCity::getProjectProduction, "int (int /*ProjectTypes*/ eIndex)")
		.def("setProjectProduction", &CyCity::setProjectProduction, "void (int /*ProjectTypes*/ eIndex, int iNewValue)")
		.def("changeProjectProduction", &CyCity::changeProjectProduction, "void (int /*ProjectTypes*/ eIndex, int iChange)")
// BUG - Project Production - end

		.def("AI_isDefended", &CyCity::AI_isDefended, "bool (int iExtra)")
		.def("getArea", &CyCity::getArea, "int ()")
		.def("isWeLoveTheKingDay", &CyCity::isWeLoveTheKingDay, "bool ()")
		.def("setWeLoveTheKingDay", &CyCity::setWeLoveTheKingDay, "void (bool bWeLoveTheKingDay)")
		.def("calculateCorporateTaxes", &CyCity::calculateCorporateTaxes, "int ()")
		.def("getCivicHappiness", &CyCity::getCivicHappiness, "int ()")
		.def("getNumCityPlots", &CyCity::getNumCityPlots, "int ()")
		.def("getPopulationgrowthratepercentage", &CyCity::getPopulationgrowthratepercentage, "int ()")
		.def("changePopulationgrowthratepercentage", &CyCity::changePopulationgrowthratepercentage, "void (int iChange)")
		.def("isValidTerrainForBuildings", &CyCity::isValidTerrainForBuildings, "bool (int /*BuildingTypes*/)")
		.def("hasFreshWater", &CyCity::hasFreshWater, "bool ()")
		.def("canUpgradeUnit", &CyCity::canUpgradeUnit, "bool (int /*UnitTypes*/)")
		.def("getBuildingProductionModifier", &CyCity::getBuildingProductionModifier, "int (int /*BuildingTypes*/)")
		.def("changeBuildingProductionModifier", &CyCity::changeBuildingProductionModifier, "void (int /*BuildingTypes*/, int iChange)")
		.def("getUnitProductionModifier", &CyCity::getUnitProductionModifier, "int (int /*UnitTypes*/)")
		.def("changeUnitProductionModifier", &CyCity::changeUnitProductionModifier, "void (int /*UnitTypes*/, int iChange)")
		.def("getBonusDefenseChanges", &CyCity::getBonusDefenseChanges, "int (int /*CommerceTypes*/)")
		.def("changeBonusDefenseChanges", &CyCity::changeBonusDefenseChanges, "void (int /*BonusTypes*/, int iChange)")
		.def("getBonusCommerceRateModifier", &CyCity::getBonusCommerceRateModifier, "int (int /*CommerceTypes*/)")
		.def("changeBonusCommerceRateModifier", &CyCity::changeBonusCommerceRateModifier, "void (int /*CommerceTypes*/, int iChange)")
		.def("changePowerCount", &CyCity::changePowerCount, "void (int iChange, bool bDirty)")

		.def("getEventAnger", &CyCity::getEventAnger, "void ()")
		.def("changeEventAnger", &CyCity::changeEventAnger, "void (int iChange)")
		.def("getNumPopulationEmployed", &CyCity::getNumPopulationEmployed, "int ()")

		.def("getBonusCommercePercentChanges", &CyCity::getBonusCommercePercentChanges, "int (eCommerce, eBuilding)")
		.def("isAutomatedCanBuild", &CyCity::isAutomatedCanBuild, "bool ()")
		.def("setAutomatedCanBuild", &CyCity::setAutomatedCanBuild, "void ()")

		.def("getProperties", &CyCity::getProperties, python::return_value_policy<python::reference_existing_object>(), "CvProperties ()")

		.def("setBuildingListInvalid", &CyCity::setBuildingListInvalid, "void ()")
		.def("getBuildingListFilterActive", &CyCity::getBuildingListFilterActive, "bool (int)")
		.def("setBuildingListFilterActive", &CyCity::setBuildingListFilterActive, "void (int,bool)")
		.def("getBuildingListGrouping", &CyCity::getBuildingListGrouping, "int ()")
		.def("setBuildingListGrouping", &CyCity::setBuildingListGrouping, "void (int)")
		.def("getBuildingListSorting", &CyCity::getBuildingListSorting, "int ()")
		.def("setBuildingListSorting", &CyCity::setBuildingListSorting, "void (int)")
		.def("getBuildingListGroupNum", &CyCity::getBuildingListGroupNum, "int ()")
		.def("getBuildingListNumInGroup", &CyCity::getBuildingListNumInGroup, "int (int)")
		.def("getBuildingListType", &CyCity::getBuildingListType, "int (int,int)")
		.def("getBuildingListSelectedBuildingRow", &CyCity::getBuildingListSelectedBuildingRow, "int ()")
		.def("getBuildingListSelectedWonderRow", &CyCity::getBuildingListSelectedWonderRow, "int ()")
		.def("setBuildingListSelectedBuilding", &CyCity::setBuildingListSelectedBuilding, "void (int)")
		.def("setBuildingListSelectedWonder", &CyCity::setBuildingListSelectedWonder, "void (int)")
		.def("getBuildingListSelectedBuilding", &CyCity::getBuildingListSelectedBuilding, "int ()")
		.def("getBuildingListSelectedWonder", &CyCity::getBuildingListSelectedWonder, "int ()")

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
		.def("getUnitListSelectedRow", &CyCity::getUnitListSelectedRow, "int ()")
		.def("setUnitListSelected", &CyCity::setUnitListSelected, "void (int)")
		.def("getUnitListSelected", &CyCity::getUnitListSelected, "int ()")

		;
}
