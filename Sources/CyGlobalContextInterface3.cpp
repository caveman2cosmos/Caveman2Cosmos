//
// published python interface for CyGlobalContext
//

#include "CvGameCoreDLL.h"

void CyGlobalContextPythonInterface3(python::class_<CyGlobalContext>& x)
{
	OutputDebugString("Python Extension Module - CyGlobalContextPythonInterface3\n");

	x

		.def("getAttitudeInfo", &CyGlobalContext::getAttitudeInfo, python::return_value_policy<python::reference_existing_object>(), "AttitudeInfo (int id)")
		.def("getMemoryInfo", &CyGlobalContext::getMemoryInfo, python::return_value_policy<python::reference_existing_object>(), "MemoryInfo (int id)")

		.def("getPlayerOptionsInfoByIndex", &CyGlobalContext::getPlayerOptionsInfoByIndex, python::return_value_policy<python::reference_existing_object>(), "(PlayerOptionsInfoID) - PlayerOptionsInfo for PlayerOptionsInfo")

		.def("getGraphicOptionsInfoByIndex", &CyGlobalContext::getGraphicOptionsInfoByIndex, python::return_value_policy<python::reference_existing_object>(), "(GraphicOptionsInfoID) - GraphicOptionsInfo for GraphicOptionsInfo")

		.def("getNumHurryInfos", &CyGlobalContext::getNumHurryInfos, "() - Total Hurry Infos")

		.def("getNumConceptInfos", &CyGlobalContext::getNumConceptInfos, "int () - NumConceptInfos")
		.def("getConceptInfo", &CyGlobalContext::getConceptInfo, python::return_value_policy<python::reference_existing_object>(), "Concept Info () - Returns info object")

		.def("getNumNewConceptInfos", &CyGlobalContext::getNumNewConceptInfos, "int () - NumNewConceptInfos")
		.def("getNewConceptInfo", &CyGlobalContext::getNewConceptInfo, python::return_value_policy<python::reference_existing_object>(), "New Concept Info () - Returns info object")


		.def("getNumCalendarInfos", &CyGlobalContext::getNumCalendarInfos, "int () - Returns NumCalendarInfos")
		.def("getCalendarInfo", &CyGlobalContext::getCalendarInfo, python::return_value_policy<python::reference_existing_object>(), "CalendarInfo () - Returns Info object")
		 
		.def("getNumGameOptionInfos", &CyGlobalContext::getNumGameOptionInfos, "int () - Returns NumGameOptionInfos")
		.def("getGameOptionInfo", &CyGlobalContext::getGameOptionInfo, python::return_value_policy<python::reference_existing_object>(), "GameOptionInfo () - Returns Info object")

		.def("getNumMPOptionInfos", &CyGlobalContext::getNumMPOptionInfos, "int () - Returns NumMPOptionInfos")
		.def("getMPOptionInfo", &CyGlobalContext::getMPOptionInfo, python::return_value_policy<python::reference_existing_object>(), "MPOptionInfo () - Returns Info object")

		.def("getNumForceControlInfos", &CyGlobalContext::getNumForceControlInfos, "int () - Returns NumForceControlInfos")
		.def("getForceControlInfo", &CyGlobalContext::getForceControlInfo, python::return_value_policy<python::reference_existing_object>(), "ForceControlInfo () - Returns Info object")

		.def("getNumSeasonInfos", &CyGlobalContext::getNumSeasonInfos, "int () - Returns NumSeasonInfos")


		.def("getNumDenialInfos", &CyGlobalContext::getNumDenialInfos, "int () - Returns NumDenialInfos")
		;
}
