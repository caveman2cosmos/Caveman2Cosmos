#include "CvGameCoreDLL.h"
#include "CyGlobalContext.h"
#include "CyMap.h"

//
// published python interface for CyGlobalContext
//

void CyGlobalContextPythonInterface3(python::class_<CyGlobalContext>& x)
{
	OutputDebugString("Python Extension Module - CyGlobalContextPythonInterface3\n");

	x
		.def("enableMultiMaps", &CyGlobalContext::enableMultiMaps)
		.def("multiMapsEnabled", &CyGlobalContext::multiMapsEnabled, "bool ()")
		.def("getNumMapInfos", &CyGlobalContext::getNumMapInfos, "int ()")
		.def("getMapInfo", &CyGlobalContext::getMapInfo, python::return_value_policy<python::reference_existing_object>(), "void (int)")
		.def("switchMap", &CyGlobalContext::switchMap, "void (int)")
		.def("getMapByIndex", &CyGlobalContext::getMapByIndex, python::return_value_policy<python::reference_existing_object>(), "CyMap (int)")
		.def("updateMaps", &CyGlobalContext::updateMaps, "void ()")
		.def("initializeMap", &CyGlobalContext::initializeMap, "void (int)")
		.def("mapInitialized", &CyGlobalContext::mapInitialized, "bool (int)")

		.def("getAttitudeInfo", &CyGlobalContext::getAttitudeInfo, python::return_value_policy<python::reference_existing_object>(), "AttitudeInfo (int id)")
		.def("getMemoryInfo", &CyGlobalContext::getMemoryInfo, python::return_value_policy<python::reference_existing_object>(), "MemoryInfo (int id)")

		.def("getNumPlayerOptionInfos", &CyGlobalContext::getNumPlayerOptionInfos)
		.def("getPlayerOptionsInfo", &CyGlobalContext::getPlayerOptionsInfoByIndex, python::return_value_policy<python::reference_existing_object>(), "(PlayerOptionsInfoID) - PlayerOptionsInfo for PlayerOptionsInfo")
		.def("getPlayerOptionsInfoByIndex", &CyGlobalContext::getPlayerOptionsInfoByIndex, python::return_value_policy<python::reference_existing_object>(), "(PlayerOptionsInfoID) - PlayerOptionsInfo for PlayerOptionsInfo")

		.def("getGraphicOptionsInfo", &CyGlobalContext::getGraphicOptionsInfoByIndex, python::return_value_policy<python::reference_existing_object>(), "(GraphicOptionsInfoID) - GraphicOptionsInfo for GraphicOptionsInfo")
		.def("getGraphicOptionsInfoByIndex", &CyGlobalContext::getGraphicOptionsInfoByIndex, python::return_value_policy<python::reference_existing_object>(), "(GraphicOptionsInfoID) - GraphicOptionsInfo for GraphicOptionsInfo")

		.def("getNumHurryInfos", &CyGlobalContext::getNumHurryInfos, "() - Total Hurry Infos")

		.def("getNumConceptInfos", &CyGlobalContext::getNumConceptInfos, "int () - NumConceptInfos")
		.def("getConceptInfo", &CyGlobalContext::getConceptInfo, python::return_value_policy<python::reference_existing_object>(), "Concept Info () - Returns info object")

		.def("getNumNewConceptInfos", &CyGlobalContext::getNumNewConceptInfos, "int () - NumNewConceptInfos")
		.def("getNewConceptInfo", &CyGlobalContext::getNewConceptInfo, python::return_value_policy<python::reference_existing_object>(), "New Concept Info () - Returns info object")

		.def("getNumCityTabInfos", &CyGlobalContext::getNumCityTabInfos, "int () - Returns NumCityTabInfos")
		.def("getCityTabInfo", &CyGlobalContext::getCityTabInfo, python::return_value_policy<python::reference_existing_object>(), "CityTabInfo - () - Returns Info object")

		.def("getNumCalendarInfos", &CyGlobalContext::getNumCalendarInfos, "int () - Returns NumCalendarInfos")
		.def("getCalendarInfo", &CyGlobalContext::getCalendarInfo, python::return_value_policy<python::reference_existing_object>(), "CalendarInfo () - Returns Info object")
		 
		.def("getNumGameOptionInfos", &CyGlobalContext::getNumGameOptionInfos, "int () - Returns NumGameOptionInfos")
		.def("getGameOptionInfo", &CyGlobalContext::getGameOptionInfo, python::return_value_policy<python::reference_existing_object>(), "GameOptionInfo () - Returns Info object")

		.def("getNumMPOptionInfos", &CyGlobalContext::getNumMPOptionInfos, "int () - Returns NumMPOptionInfos")
		.def("getMPOptionInfo", &CyGlobalContext::getMPOptionInfo, python::return_value_policy<python::reference_existing_object>(), "MPOptionInfo () - Returns Info object")

		.def("getNumForceControlInfos", &CyGlobalContext::getNumForceControlInfos, "int () - Returns NumForceControlInfos")
		.def("getForceControlInfo", &CyGlobalContext::getForceControlInfo, python::return_value_policy<python::reference_existing_object>(), "ForceControlInfo () - Returns Info object")

		.def("getNumSeasonInfos", &CyGlobalContext::getNumSeasonInfos, "int () - Returns NumSeasonInfos")
		.def("getSeasonInfo", &CyGlobalContext::getSeasonInfo, python::return_value_policy<python::reference_existing_object>(), "SeasonInfo () - Returns Info object")

		.def("getNumMonthInfos", &CyGlobalContext::getNumMonthInfos, "int () - Returns NumMonthInfos")
		.def("getMonthInfo", &CyGlobalContext::getMonthInfo, python::return_value_policy<python::reference_existing_object>(), "MonthInfo () - Returns Info object")

		.def("getNumDenialInfos", &CyGlobalContext::getNumDenialInfos, "int () - Returns NumDenialInfos")
		.def("getDenialInfo", &CyGlobalContext::getDenialInfo, python::return_value_policy<python::reference_existing_object>(), "DenialInfo () - Returns Info object")
		;
}
