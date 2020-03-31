#include "CvGameCoreDLL.h"

//
// published python interface for CyMap
//

void CyMapPythonInterface()
{
	OutputDebugString("Python Extension Module - CyMapPythonInterface\n");

	python::class_<CyMap>("CyMap")
		.def("isNone", &CyMap::isNone, "bool () - valid CyMap() interface")

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
		.def("getViewportWidth", &CyMap::getViewportWidth, "int ()")
		.def("getViewportHeight", &CyMap::getViewportHeight, "int ()")
		.def("getViewportXFromMapX", &CyMap::getViewportXFromMapX, "int (int iX)")
		.def("getViewportYFromMapY", &CyMap::getViewportYFromMapY, "int (int iY)")
		.def("isInViewport", &CyMap::isInViewport, "bool (int iX, int iY)")

		.def("closeAdvisor", &CyMap::closeAdvisor, "void (int advisorWidth, int iMinimapLeft, int iMinimapRight, int iMinimapTop, int iMinimapBottom)")
		.def("bringIntoView", &CyMap::bringIntoView, "void (int iX, int iY, bool bDisplayCityScreen)")
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

		.def("resetRevealedPlots", &CyMap::resetRevealedPlots, "void (int /*TeamTypes*/ eTeam) - removes reveal status on all plots to eteam but then resets reveal for that plot if is currently visible")
		.def("setAllPlotTypes", &CyMap::setAllPlotTypes, "void (int /*PlotTypes*/ ePlotType) - sets all plots to ePlotType")

		.def("verifyUnitValidPlot", &CyMap::verifyUnitValidPlot, "void ()")

		.def("findCity", &CyMap::findCity, python::return_value_policy<python::manage_new_object>(), "CyCity* (int iX, int iY, int (PlayerTypes) eOwner = NO_PLAYER, int (TeamTypes) eTeam = NO_TEAM, bool bSameArea = true, bool bCoastalOnly = false, int (TeamTypes) eTeamAtWarWith = NO_TEAM, int (DirectionTypes) eDirection = NO_DIRECTION, CvCity* pSkipCity = NULL) - finds city")

		.def("findBiggestArea", &CyMap::findBiggestArea, python::return_value_policy<python::manage_new_object>(), "CyArea* ()")

		.def("getMapFractalFlags", &CyMap::getMapFractalFlags, "int ()")
		.def("numPlots", &CyMap::numPlots, "int () - total plots in the map")
		.def("plotNum", &CyMap::plotNum, "int (iX,iY) - the index for a given plot") 
		.def("getGridWidth", &CyMap::getGridWidth, "int () - the width of the map, in plots")
		.def("getGridHeight", &CyMap::getGridHeight, "int () - the height of the map, in plots")

		.def("getLandPlots", &CyMap::getLandPlots, "int () - total land plots")
		.def("getOwnedPlots", &CyMap::getOwnedPlots, "int () - total owned plots")

		.def("getTopLatitude", &CyMap::getTopLatitude, "int () - top latitude (usually 90)")
		.def("getBottomLatitude", &CyMap::getBottomLatitude, "int () - bottom latitude (usually -90)")

		.def("getNextRiverID", &CyMap::getNextRiverID, "int ()")
		.def("incrementNextRiverID", &CyMap::incrementNextRiverID, "void ()")

		.def("isWrapX", &CyMap::isWrapX, "bool () - whether the map wraps in the X axis")
		.def("isWrapY", &CyMap::isWrapY, "bool () - whether the map wraps in the Y axis")
		.def("getMapScriptName", &CyMap::getMapScriptName, "wstring () - name of the map script")
		.def("getWorldSize", &CyMap::getWorldSize, "WorldSizeTypes () - size of the world")
		.def("getClimate", &CyMap::getClimate, "ClimateTypes () - climate of the world")
		.def("getSeaLevel", &CyMap::getSeaLevel, "SeaLevelTypes () - sealevel of the world")

		.def("getNumCustomMapOptions", &CyMap::getNumCustomMapOptions, "int () - number of custom map settings")
		.def("getCustomMapOption", &CyMap::getCustomMapOption, "CustomMapOptionTypes () - user defined map setting at this option id")

		.def("getNumBonuses", &CyMap::getNumBonuses, "int () - total bonuses")

		.def("plotByIndex", &CyMap::plotByIndex, python::return_value_policy<python::manage_new_object>(), "CyPlot (iIndex) - get a plot by its Index")
		.def("sPlotByIndex", &CyMap::sPlotByIndex, python::return_value_policy<python::reference_existing_object>(), "CyPlot (iIndex) - static - get plot by iIndex")
		.def("plot", &CyMap::plot, python::return_value_policy<python::manage_new_object>(), "CyPlot (iX,iY) - get CyPlot at (iX,iY)")
		.def("sPlot", &CyMap::sPlot, python::return_value_policy<python::reference_existing_object>(), "CyPlot (iX,iY) - static - get CyPlot at (iX,iY)")
		.def("getIndexAfterLastArea", &CyMap::getIndexAfterLastArea, "int () - index for handling NULL areas")
		.def("getNumAreas", &CyMap::getNumAreas, "int () - total areas")
		.def("getNumLandAreas", &CyMap::getNumLandAreas, "int () - total land areas")
		.def("getArea", &CyMap::getArea, python::return_value_policy<python::manage_new_object>(), "CyArea (iID) - get CyArea at iID")
		.def("recalculateAreas", &CyMap::recalculateAreas, "void () - Recalculates the areaID for each plot. Should be preceded by CyMap.setPlotTypes(...)")

		.def("rebuild", &CyMap::rebuild, "used to initialize the map during WorldBuilder load")
		.def("updateMinimapColor", &CyMap::updateMinimapColor, "void ()")

		// AIAndy: Expose path generation functionality to Python
		.def("generatePathForHypotheticalUnit", &CyMap::generatePathForHypotheticalUnit, "bool (CyPlot*, CyPlot*, int, int, int, int)")
		.def("getLastPathStepNum", &CyMap::getLastPathStepNum, "int ()")
		.def("getLastPathPlotByIndex", &CyMap::getLastPathPlotByIndex, python::return_value_policy<python::manage_new_object>(), "CyPlot (index) - get a plot on the path by its Index")
		;
}
