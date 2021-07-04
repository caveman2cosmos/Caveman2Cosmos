#include "CvGameCoreDLL.h"

//
// Python interface for info classes (formerly structs)
// These are simple enough to be exposed directly - no wrappers
//

void CyInfoPythonInterface4()
{
	OutputDebugString("Python Extension Module - CyInfoPythonInterface4\n");

	python::class_<CvAutomateInfo, python::bases<CvInfoBase> >("CvAutomateInfo")
		;

	python::class_<CvCommandInfo, python::bases<CvInfoBase> >("CvCommandInfo")
		;

	python::class_<CvGameOptionInfo, python::bases<CvInfoBase> >("CvGameOptionInfo")
		.def("getDefault", &CvGameOptionInfo::getDefault, "bool ()")
		.def("getVisible", &CvGameOptionInfo::getVisible, "bool ()")
		;

	python::class_<CvMPOptionInfo, python::bases<CvInfoBase> >("CvMPOptionInfo")
		.def("getDefault", &CvMPOptionInfo::getDefault, "bool ()")
		;

	python::class_<CvForceControlInfo, python::bases<CvInfoBase> >("CvForceControlInfo")
		.def("getDefault", &CvForceControlInfo::getDefault, "bool ()")
		;

	python::class_<CvPlayerOptionInfo, python::bases<CvInfoBase> >("CvPlayerOptionInfo")
		.def("getDefault", &CvPlayerOptionInfo::getDefault, "bool ()")
		;

	python::class_<CvGraphicOptionInfo, python::bases<CvInfoBase> >("CvGraphicOptionInfo")
		.def("getDefault", &CvGraphicOptionInfo::getDefault, "bool ()")
		;

	python::class_<CvEventTriggerInfo, python::bases<CvInfoBase> >("CvEventTriggerInfo")
		.def("getNumUnits", &CvEventTriggerInfo::getNumUnits, "int ()")
		.def("getNumBuildings", &CvEventTriggerInfo::getNumBuildings, "int ()")
		.def("getCivic", &CvEventTriggerInfo::getCivic, "int ()")
		.def("getMinPopulation", &CvEventTriggerInfo::getMinPopulation, "int ()")
		.def("getMaxPopulation", &CvEventTriggerInfo::getMaxPopulation, "int ()")
		.def("getAngry", &CvEventTriggerInfo::getAngry, "int ()")

		.def("getBuildingRequired", &CvEventTriggerInfo::getBuildingRequired, "int (int)")
		.def("getPrereqOrTechs", &CvEventTriggerInfo::getPrereqOrTechs, "int (int)")
		.def("getPrereqAndTechs", &CvEventTriggerInfo::getPrereqAndTechs, "int (int)")
		.def("getObsoleteTech", &CvEventTriggerInfo::getObsoleteTech, "int (int)")
		.def("getEvent", &CvEventTriggerInfo::getEvent, "int (int)")
		.def("getPrereqEvent", &CvEventTriggerInfo::getPrereqEvent, "int (int)")

		.def("isTeam", &CvEventTriggerInfo::isTeam, "bool ()")
		.def("isStateReligion", &CvEventTriggerInfo::isStateReligion, "bool ()")
		;

	python::class_<CvEventInfo, python::bases<CvInfoBase> >("CvEventInfo")
		.def("isQuest", &CvEventInfo::isQuest, "bool ()")
		.def("isTeam", &CvEventInfo::isTeam, "bool ()")
		.def("isGoldenAge", &CvEventInfo::isGoldenAge, "bool ()")
		.def("getGold", &CvEventInfo::getGold, "int ()")
		.def("getCulture", &CvEventInfo::getCulture, "int ()")
		.def("getEspionagePoints", &CvEventInfo::getEspionagePoints, "int ()")
		.def("getTech", &CvEventInfo::getTech, "int ()")
		.def("getPrereqTech", &CvEventInfo::getPrereqTech, "int ()")
		.def("getNumUnits", &CvEventInfo::getNumUnits, "int ()")
		.def("getBuilding", &CvEventInfo::getBuilding, "int ()")
		.def("getHappy", &CvEventInfo::getHappy, "int ()")
		.def("getHealth", &CvEventInfo::getHealth, "int ()")
		.def("getHurryAnger", &CvEventInfo::getHurryAnger, "int ()")
		.def("getFood", &CvEventInfo::getFood, "int ()")
		.def("getFeature", &CvEventInfo::getFeature, "int ()")
		.def("getImprovement", &CvEventInfo::getImprovement, "int ()")
		.def("getImprovementChange", &CvEventInfo::getImprovementChange, "int ()")
		.def("getBonus", &CvEventInfo::getBonus, "int ()")
		.def("getRoute", &CvEventInfo::getRoute, "int ()")
		.def("getRouteChange", &CvEventInfo::getRouteChange, "int ()")
		.def("getSpaceProductionModifier", &CvEventInfo::getSpaceProductionModifier, "int ()")

		.def("getPlotExtraYield", &CvEventInfo::getPlotExtraYield, "int (int)")
		.def("getFreeSpecialistCount", &CvEventInfo::getFreeSpecialistCount, "int (int)")

		.def("getBuildingYieldChange", &CvEventInfo::getBuildingYieldChange, "int (int /*BuildingTypes*/, int /*YieldTypes*/)")
		.def("getBuildingCommerceChange", &CvEventInfo::getBuildingCommerceChange, "int (int /*BuildingTypes*/, int /*CommerceTypes*/)")
		.def("getBuildingHappyChange", &CvEventInfo::getBuildingHappyChange, "int (int /*BuildingTypes*/)")
		.def("getBuildingHealthChange", &CvEventInfo::getBuildingHealthChange, "int (int /*BuildingTypes*/)")
		;

	python::class_<CvEspionageMissionInfo, python::bases<CvInfoBase> >("CvEspionageMissionInfo")
		.def("getCost", &CvEspionageMissionInfo::getCost, "int ()")
		.def("isPassive", &CvEspionageMissionInfo::isPassive, "bool ()")
		.def("isTargetsCity", &CvEspionageMissionInfo::isTargetsCity, "bool ()")
		.def("getTechPrereq", &CvEspionageMissionInfo::getTechPrereq, "int ()")
		.def("isInvestigateCity", &CvEspionageMissionInfo::isInvestigateCity, "bool ()")
		.def("isSeeDemographics", &CvEspionageMissionInfo::isSeeDemographics, "bool ()")
		.def("isSeeResearch", &CvEspionageMissionInfo::isSeeResearch, "bool ()")
		.def("getCounterespionageMod", &CvEspionageMissionInfo::getCounterespionageMod, "int ()")
		;

	python::class_<CvVoteSourceInfo, python::bases<CvInfoBase> >("CvVoteSourceInfo")
		.def("getCivic", &CvVoteSourceInfo::getCivic, "int ()")
		.def("getFreeSpecialist", &CvVoteSourceInfo::getFreeSpecialist, "int ()")
		.def("getReligionCommerce", &CvVoteSourceInfo::getReligionCommerce, "int (int)")
		.def("getSecretaryGeneralText", &CvVoteSourceInfo::pyGetSecretaryGeneralText, "wstring ()")
		;

	python::class_<CvMainMenuInfo, python::bases<CvInfoBase> >("CvMainMenuInfo")
		;

	python::class_<CvPropertyInfo, python::bases<CvInfoBase> >("CvPropertyInfo")
		.def("getChar", &CvPropertyInfo::getChar, "int ()")
		;
}
