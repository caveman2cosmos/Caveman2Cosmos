#include "CvGameCoreDLL.h"
#include "CyGlobalContext.h"
#include "CvInfos.h"

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CyGlobalContext_getInfoTypeForString_overloads, CyGlobalContext::getInfoTypeForString, 1, 2)

//
// published python interface for CyGlobalContext
//

void CyGlobalContextPythonInterface4(python::class_<CyGlobalContext>& x)
{
	OutputDebugString("Python Extension Module - CyGlobalContextPythonInterface4\n");
	x
		.def("getNumMissionInfos", &CyGlobalContext::getNumMissionInfos, "() - Total Mission Infos XML\\Units\\CIV4MissionInfos.xml")
		.def("getMissionInfo", &CyGlobalContext::getMissionInfo, python::return_value_policy<python::reference_existing_object>(), "(MissionID) - CvInfo for MissionID")

		.def("getNumCommandInfos", &CyGlobalContext::getNumCommandInfos, "() - Total Command Infos XML\\Units\\CIV4CommandInfos.xml")
		.def("getCommandInfo", &CyGlobalContext::getCommandInfo, python::return_value_policy<python::reference_existing_object>(), "(CommandID) - CvInfo for CommandID")

		.def("getNumControlInfos", &CyGlobalContext::getNumControlInfos, "() - Total Control Infos XML\\Units\\CIV4ControlInfos.xml")
		.def("getControlInfo", &CyGlobalContext::getControlInfo, python::return_value_policy<python::reference_existing_object>(), "(ControlID) - CvInfo for ControlID")

		.def("getNumPromotionInfos", &CyGlobalContext::getNumPromotionInfos, "() - Total Promotion Infos XML\\Units\\CIV4PromotionInfos.xml")
		.def("getPromotionInfo", &CyGlobalContext::getPromotionInfo, python::return_value_policy<python::reference_existing_object>(), "(PromotionID) - CvInfo for PromotionID")

		.def("getNumTechInfos", &CyGlobalContext::getNumTechInfos, "() - Total Technology Infos XML\\Technologies\\CIV4TechInfos.xml")
		.def("getTechInfo", &CyGlobalContext::getTechInfo, python::return_value_policy<python::reference_existing_object>(), "(TechID) - CvInfo for TechID")

		.def("getNumSpecialBuildingInfos", &CyGlobalContext::getNumSpecialBuildingInfos, "() - Total Special Building Infos")
		.def("getSpecialBuildingInfo", &CyGlobalContext::getSpecialBuildingInfo, python::return_value_policy<python::reference_existing_object>(), "(SpecialBuildingID) - CvInfo for SpecialBuildingID")

		.def("getNumReligionInfos", &CyGlobalContext::getNumReligionInfos, "() - Total Religion Infos XML\\GameInfo\\CIV4ReligionInfos.xml")
		.def("getReligionInfo", &CyGlobalContext::getReligionInfo, python::return_value_policy<python::reference_existing_object>(), "(ReligionID) - CvInfo for ReligionID")

		.def("getNumCorporationInfos", &CyGlobalContext::getNumCorporationInfos, "() - Total Religion Infos XML\\GameInfo\\CIV4CorporationInfos.xml")
		.def("getCorporationInfo", &CyGlobalContext::getCorporationInfo, python::return_value_policy<python::reference_existing_object>(), "(CorporationID) - CvInfo for CorporationID")

		.def("getNumVictoryInfos", &CyGlobalContext::getNumVictoryInfos, "() - Total Victory Infos XML\\GameInfo\\CIV4VictoryInfos.xml")
		.def("getVictoryInfo", &CyGlobalContext::getVictoryInfo, python::return_value_policy<python::reference_existing_object>(), "(VictoryID) - CvInfo for VictoryID")

		.def("getNumSpecialistInfos", &CyGlobalContext::getNumSpecialistInfos, "() - Total Specialist Infos XML\\Units\\CIV4SpecialistInfos.xml")
		.def("getSpecialistInfo", &CyGlobalContext::getSpecialistInfo, python::return_value_policy<python::reference_existing_object>(), "(SpecialistID) - CvInfo for SpecialistID")

		.def("getNumCivicOptionInfos", &CyGlobalContext::getNumCivicOptionInfos, "() - Total Civic Infos XML\\Misc\\CIV4CivicOptionInfos.xml")
		.def("getCivicOptionInfo", &CyGlobalContext::getCivicOptionInfo, python::return_value_policy<python::reference_existing_object>(), "(CivicID) - CvInfo for CivicID")

		.def("getNumCivicInfos", &CyGlobalContext::getNumCivicInfos, "() - Total Civic Infos XML\\Misc\\CIV4CivicInfos.xml")
		.def("getCivicInfo", &CyGlobalContext::getCivicInfo, python::return_value_policy<python::reference_existing_object>(), "(CivicID) - CvInfo for CivicID")

		.def("getNumDiplomacyInfos", &CyGlobalContext::getNumDiplomacyInfos, "() - Total diplomacy Infos XML\\GameInfo\\CIV4DiplomacyInfos.xml")
		.def("getDiplomacyInfo", &CyGlobalContext::getDiplomacyInfo, python::return_value_policy<python::reference_existing_object>(), "(DiplomacyID) - CvInfo for DiplomacyID")

		.def("getNumProjectInfos", &CyGlobalContext::getNumProjectInfos, "() - Total Project Infos XML\\GameInfo\\CIV4ProjectInfos.xml")
		.def("getProjectInfo", &CyGlobalContext::getProjectInfo, python::return_value_policy<python::reference_existing_object>(), "(ProjectID) - CvInfo for ProjectID")

		.def("getNumVoteInfos", &CyGlobalContext::getNumVoteInfos, "() - Total VoteInfos")
		.def("getVoteInfo", &CyGlobalContext::getVoteInfo, python::return_value_policy<python::reference_existing_object>(), "(VoteID) - CvInfo for VoteID")

		.def("getNumProcessInfos", &CyGlobalContext::getNumProcessInfos, "() - Total ProcessInfos")
		.def("getProcessInfo", &CyGlobalContext::getProcessInfo, python::return_value_policy<python::reference_existing_object>(), "(ProcessID) - CvInfo for ProcessID")

		.def("getNumEmphasizeInfos", &CyGlobalContext::getNumEmphasizeInfos, "() - Total EmphasizeInfos")
		.def("getEmphasizeInfo", &CyGlobalContext::getEmphasizeInfo, python::return_value_policy<python::reference_existing_object>(), "(EmphasizeID) - CvInfo for EmphasizeID")

		.def("getHurryInfo", &CyGlobalContext::getHurryInfo, python::return_value_policy<python::reference_existing_object>(), "(HurryID) - CvInfo for HurryID")

		.def("getUnitAIInfo", &CyGlobalContext::getUnitAIInfo, python::return_value_policy<python::reference_existing_object>(), "UnitAIInfo (int id)")

		.def("getColorInfo", &CyGlobalContext::getColorInfo, python::return_value_policy<python::reference_existing_object>(), "ColorInfo (int id)")

		.def("getInfoTypeForString", &CyGlobalContext::getInfoTypeForString, CyGlobalContext_getInfoTypeForString_overloads())

		.def("getNumPlayerColorInfos", &CyGlobalContext::getNumPlayerColorInfos, "int () - Returns number of PlayerColorInfos")
		.def("getPlayerColorInfo", &CyGlobalContext::getPlayerColorInfo, python::return_value_policy<python::reference_existing_object>(), "PlayerColorInfo (int id)")

		.def("getNumEventTriggerInfos", &CyGlobalContext::getNumEventTriggerInfos, "int () - Returns number of EventTriggerInfos")
		.def("getEventTriggerInfo", &CyGlobalContext::getEventTriggerInfo, python::return_value_policy<python::reference_existing_object>(), "EventTriggerInfo () - Returns info object")

		.def("getNumEventInfos", &CyGlobalContext::getNumEventInfos, "int () - Returns number of EventInfos")
		.def("getEventInfo", &CyGlobalContext::getEventInfo, python::return_value_policy<python::reference_existing_object>(), "EventInfo () - Returns info object")

		.def("getNumEspionageMissionInfos", &CyGlobalContext::getNumEspionageMissionInfos, "int () - Returns number of EspionageMissionInfos")
		.def("getEspionageMissionInfo", &CyGlobalContext::getEspionageMissionInfo, python::return_value_policy<python::reference_existing_object>(), "EspionageMissionInfo () - Returns info object")

		.def("getNumMainMenus", &CyGlobalContext::getNumMainMenus, "int () - Returns number")
		.def("getMainMenus", &CyGlobalContext::getMainMenus, python::return_value_policy<python::reference_existing_object>(), "MainMenus () - Returns info object")

		.def("getNumVoteSourceInfos", &CyGlobalContext::getNumVoteSourceInfos, "int ()")
		.def("getVoteSourceInfo", &CyGlobalContext::getVoteSourceInfo, python::return_value_policy<python::reference_existing_object>(), "Returns info object")

		.def("getNumVoteSourceInfos", &CyGlobalContext::getNumVoteSourceInfos, "int ()")
		.def("getVoteSourceInfo", &CyGlobalContext::getVoteSourceInfo, python::return_value_policy<python::reference_existing_object>(), "Returns info object")

		.def("getArtStyleTypes", &CyGlobalContext::getArtStyleTypes, "string () - Returns enum string")

		.def("getNumPropertyInfos", &CyGlobalContext::getNumPropertyInfos, "int () - Returns number of PropertyInfos")
		.def("getPropertyInfo", &CyGlobalContext::getPropertyInfo, python::return_value_policy<python::reference_existing_object>(), "PropertyInfo (int)")
	;
}
