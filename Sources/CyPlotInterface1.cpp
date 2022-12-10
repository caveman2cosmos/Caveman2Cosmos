#include "CvGameCoreDLL.h"
#include "CyArea.h"
#include "CyCity.h"
#include "CyPlot.h"
#include "CyUnit.h"

//
// published python interface for CyPlot
//

void CyPlotPythonInterface1(python::class_<CyPlot>& x)
{
	OutputDebugString("Python Extension Module - CyPlotPythonInterface1\n");

	x
		.def("isNone", &CyPlot::isNone, "bool ()")
		.def("erase", &CyPlot::erase, "void ()")
		.def("getPoint", &CyPlot::getPoint, "NiPoint3 ()")
		.def("getTeam", &CyPlot::getTeam, "int ()")

		.def("nukeExplosion", &CyPlot::nukeExplosion, "void (int iRange, CyUnit* pNukeUnit)")

		.def("isConnectedTo", &CyPlot::isConnectedTo, "bool (CvCity* pCity) - returns whether this plot is connected to the provided city")
		.def("isConnectedToCapital", &CyPlot::isConnectedToCapital, "bool (int (PlayerTypes) ePlayer) - returns whether this plot is connected to the capital of the provided player")

		.def("isAdjacentToArea", &CyPlot::isAdjacentToArea, "bool (CyArea)")
		.def("isCoastal", &CyPlot::isCoastal, "bool ()")

		.def("isLake", &CyPlot::isLake, "bool ()")
		.def("isFreshWater", &CyPlot::isFreshWater, "bool ()")

		.def("isRiverSide", &CyPlot::isRiverSide, "bool ()")
		.def("isRiver", &CyPlot::isRiver, "bool ()")

		.def("getNearestLandPlot", &CyPlot::getNearestLandPlot, python::return_value_policy<python::manage_new_object>(), "CyPlot* ()")
		.def("canHaveBonus", &CyPlot::canHaveBonus, "bool (int /*BonusTypes*/ eBonus, bool bIgnoreLatitude)")
		.def("canHaveImprovement", &CyPlot::canHaveImprovement, "bool (int (ImprovementTypes) eImprovement, int (TeamTypes) eTeam, bool bPotential)")
		.def("canBuild", &CyPlot::canBuild, "bool (int (BuildTypes) eBuild, int (PlayerTypes) ePlayer, bool bTestVisible)")
		.def("getBuildTurnsLeft", &CyPlot::getBuildTurnsLeft, "int (int (BuildTypes) eBuild, int iNowExtra, int iThenExtra)")
		.def("getFeatureProduction", &CyPlot::getFeatureProduction, "int (int (BuildTypes) eBuild, int (TeamTypes) eTeam, CvCity** ppCity)")

		.def("getUnitPower", &CyPlot::getUnitPower, "int (int /*PlayerTypes*/ eOwner)")

		.def("defenseModifier", &CyPlot::defenseModifier, "int (int /*TeamTypes*/, bool bIgnoreBuilding, bool bHelp)")

		.def("isAdjacentPlayer", &CyPlot::isAdjacentPlayer, "bool (int /*PlayerTypes*/ ePlayer, bool bLandOnly)")
		.def("calculateCulturalOwner", &CyPlot::calculateCulturalOwner, "int ()")
		.def("isOwned", &CyPlot::isOwned, "bool ()")
		.def("isNPC", &CyPlot::isNPC, "bool ()")
		.def("isHominid", &CyPlot::isHominid, "bool ()")
		.def("isVisible", &CyPlot::isVisible, "bool (int /*TeamTypes*/ eTeam, bool bDebug)")
		.def("isActiveVisible", &CyPlot::isActiveVisible, "bool (bool bDebug)")
		.def("isAdjacentNonrevealed", &CyPlot::isAdjacentNonrevealed)
		.def("isAdjacentRevealed", &CyPlot::isAdjacentRevealed)

		.def("isGoody", &CyPlot::isGoody, "bool ()")

		.def("isCity", &CyPlot::isCity, "bool ()")
		.def("isOccupation", &CyPlot::isOccupation, "bool ()")

		.def("isUnit", &CyPlot::isUnit, "bool ()")
		.def("getNumDefenders", &CyPlot::getNumDefenders, "int (int /*PlayerTypes*/ ePlayer)")
		.def("getNumVisibleEnemyDefenders", &CyPlot::getNumVisiblePotentialEnemyDefenders, "int (CyUnit* pUnit)")
		.def("isVisibleEnemyUnit", &CyPlot::isVisibleEnemyUnit, "bool (int /*PlayerTypes*/ ePlayer)")

		.def("canHaveFeature", &CyPlot::canHaveFeature, "bool (int /*FeatureTypes*/ eFeature)")
		.def("isBonusNetwork", &CyPlot::isBonusNetwork, "bool (int (TeamTypes) eTeam)")

		.def("isTradeNetworkImpassable", &CyPlot::isTradeNetworkImpassable, "bool (int (TeamTypes) eTeam)")
		.def("isTradeNetwork", &CyPlot::isTradeNetwork, "bool (int eTeam)")
		.def("isImpassable", &CyPlot::isImpassable, "bool ()")

		.def("getX", &CyPlot::getX, "int ()")
		.def("getY", &CyPlot::getY, "int ()")
		.def("getLatitude", &CyPlot::getLatitude, "int ()")
		.def("getLongitude", &CyPlot::getLongitude, "int ()")
		.def("area", &CyPlot::area, python::return_value_policy<python::manage_new_object>(), "CyArea* ()")
		.def("waterArea", &CyPlot::waterArea, python::return_value_policy<python::manage_new_object>(), "CyArea* ()")
		.def("getArea", &CyPlot::getArea, "int ()")
		.def("getFeatureVariety", &CyPlot::getFeatureVariety, "int ()")

		.def("getUpgradeTimeLeft", &CyPlot::getUpgradeTimeLeft, "int (int /*ImprovementTypes*/ eImprovement, int /*PlayerTypes*/ ePlayer)")

		.def("changeImprovementUpgradeProgress", &CyPlot::changeImprovementUpgradeProgress, "void (int iChange)")

		.def("isStartingPlot", &CyPlot::isStartingPlot, "bool ()")
		.def("setStartingPlot", &CyPlot::setStartingPlot, "void (bool bNewValue)")
		.def("isNOfRiver", &CyPlot::isNOfRiver, "bool ()")
		.def("setNOfRiver", &CyPlot::setNOfRiver, "void (bool bNewValue, CardinalDirectionTypes eRiverDir)")
		.def("isWOfRiver", &CyPlot::isWOfRiver, "bool ()")
		.def("setWOfRiver", &CyPlot::setWOfRiver, "void (bool bNewValue, CardinalDirectionTypes eRiverDir)")
		.def("getRiverWEDirection", &CyPlot::getRiverWEDirection, "CardinalDirectionTypes ()")
		.def("getRiverNSDirection", &CyPlot::getRiverNSDirection, "CardinalDirectionTypes ()")

		.def("isPotentialCityWork", &CyPlot::isPotentialCityWork, "bool ()")

		.def("getOwner", &CyPlot::getOwner, "int ()")
		.def("setOwner", &CyPlot::setOwner, "void (int /*PlayerTypes*/ eNewValue)")
		.def("getPlotType", &CyPlot::getPlotType, "int ()")
		.def("isWater", &CyPlot::isWater, "bool ()")
		.def("isFlatlands", &CyPlot::isFlatlands, "bool ()")
		.def("isHills", &CyPlot::isHills, "bool ()")
		.def("isPeak", &CyPlot::isPeak, "bool ()")
		.def("setPlotType", &CyPlot::setPlotType, "void (PlotTypes eNewValue, bool bRecalculate, bool bRebuildGraphics)")
		.def("getTerrainType", &CyPlot::getTerrainType, "int ()")
		.def("setTerrainType", &CyPlot::setTerrainType, "void (TerrainTypes eNewValue, bool bRecalculate, bool bRebuildGraphics)")
		.def("getFeatureType", &CyPlot::getFeatureType, "int ()")
		.def("setFeatureType", &CyPlot::setFeatureType, "void (int /*FeatureTypes*/ eNewValue, int iVariety)")
		.def("getBonusType", &CyPlot::getBonusType, "int (int /*TeamTypes*/ eTeam)")
		.def("setBonusType", &CyPlot::setBonusType, "void (int eNewValue)")
		.def("getImprovementType", &CyPlot::getImprovementType, "int ()")
		.def("setImprovementType", &CyPlot::setImprovementType, "void (int eNewValue)")
		.def("getRouteType", &CyPlot::getRouteType, "int ()")
		.def("setRouteType", &CyPlot::setRouteType, "void (int (RouteTypes) eNewValue)")

		.def("getPlotCity", &CyPlot::getPlotCity, python::return_value_policy<python::manage_new_object>(), "CyCity* ()")
		.def("getWorkingCity", &CyPlot::getWorkingCity, python::return_value_policy<python::manage_new_object>(), "CyCity* ()")
		.def("getRiverID", &CyPlot::getRiverID, "int ()")
		.def("setRiverID", &CyPlot::setRiverID, "void (int)")
		.def("getYield", &CyPlot::getYield, "int (YieldTypes eIndex)")
		.def("calculateBestNatureYield", &CyPlot::calculateBestNatureYield, "int (int (YieldTypes) eYield, int (TeamTypes) eTeam)")
		.def("calculateTotalBestNatureYield", &CyPlot::calculateTotalBestNatureYield, "int (int (TeamTypes) eTeam)")
		.def("calculateImprovementYieldChange", &CyPlot::calculateImprovementYieldChange, "int (int (ImprovementTypes) eImprovement, int (YieldTypes) eYield, int (PlayerTypes) ePlayer, bool bOptimal)")
		.def("hasYield", &CyPlot::hasYield, "bool ()")

		.def("getCulture", &CyPlot::getCulture, "int (int /*PlayerTypes*/ eIndex)")
		.def("countTotalCulture", &CyPlot::countTotalCulture, "int ()")

		.def("calculateCulturePercent", &CyPlot::calculateCulturePercent, "int (int /*PlayerTypes*/ eIndex)")
		.def("setCulture", &CyPlot::setCulture, "void (int /*PlayerTypes*/ eIndex, int iNewValue, bool bUpdate)")
		.def("changeCulture", &CyPlot::changeCulture, "void (int /*PlayerTypes*/ eIndex, int iChange, bool bUpdate)")

		.def("getFoundValue", &CyPlot::getFoundValue, "int (int /*PlayerTypes*/ eIndex)")

		.def("isPlayerCityRadius", &CyPlot::isPlayerCityRadius, "bool (int /*PlayerTypes*/ eIndex)")

		.def("getRevealedOwner", &CyPlot::getRevealedOwner, "int (int (TeamTypes) eTeam, bool bDebug)")

		.def("isRiverCrossing", &CyPlot::isRiverCrossing, "bool (DirectionTypes eIndex)")

		.def("isRevealed", &CyPlot::isRevealed, "bool (int /*TeamTypes*/ eTeam, bool bDebug)")
		.def("setRevealed", &CyPlot::setRevealed, "void (int /*TeamTypes*/ eTeam, bool bNewValue, bool bTerrainOnly, int /*TeamTypes*/ eFromTeam)")

		.def("getInvisibleVisibilityCount", &CyPlot::getInvisibleVisibilityCount, "int (int (TeamTypes eTeam), int (InvisibleTypes) eInvisible)")
		.def("isSpotterInSight", &CyPlot::isSpotterInSight, "int (int (TeamTypes eTeam), int (InvisibleTypes) eInvisible)")
		.def("changeInvisibleVisibilityCount", &CyPlot::changeInvisibleVisibilityCount, "int (int iTeam, int iInvisible, int iChange)")

		.def("units", &CyPlot::units)
		.def("getNumUnits", &CyPlot::getNumUnits, "int ()")
		.def("getUnit", &CyPlot::getUnit, python::return_value_policy<python::manage_new_object>(), "CyUnit* (int iIndex)")

		.def("getScriptData", &CyPlot::getScriptData, "str () - Get stored custom data")
		.def("setScriptData", &CyPlot::setScriptData, "void (str) - Set stored custom data")

		.def("getRevoltProtection", &CyPlot::getRevoltProtection, "int ()")

		.def("isInViewport", &CyPlot::isInViewport, "bool ()")
		.def("cloneToViewport", &CyPlot::cloneToViewport, python::return_value_policy<python::manage_new_object>(), "CyPlot* ()")

		.def("adjacent", &CyPlot::adjacent)
		.def("rect", &CyPlot::rect)
	;
}
