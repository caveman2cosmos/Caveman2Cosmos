#include "CvGameCoreDLL.h"
#include "CyArea.h"
#include "CyCity.h"
//
// published python interface for CyArea
//

void CyAreaPythonInterface()
{
	OutputDebugString("Python Extension Module - CyAreaPythonInterface\n");

	python::class_<CyArea>("CyArea")
		.def("isNone", &CyArea::isNone, "bool () - Returns whether the pointer points to a real Area")

		.def("calculateTotalBestNatureYield", &CyArea::calculateTotalBestNatureYield, "int () - Returns the total tile yield from the area")
		.def("countCoastalLand", &CyArea::countCoastalLand)
		.def("countNumUniqueBonusTypes", &CyArea::countNumUniqueBonusTypes, "int () - Returns the number of unique bonus types in this area")
		.def("getID", &CyArea::getID, "int () - Return's the Areas ID")
		.def("getNumTiles", &CyArea::getNumTiles, "int () - Returns the number of tiles in this area")
		.def("isLake", &CyArea::isLake, "bool () - Returns whether the area is a fresh water lake")
		.def("getNumRiverEdges", &CyArea::getNumRiverEdges, "int () - Returns the number of River Edges in this area")
		.def("getNumCities", &CyArea::getNumCities, "int () - Returns the total number of cities for all players in this area")
		.def("getNumUnits", &CyArea::getNumUnits, "int () - Returns the total number of units for all players in this area")
		.def("getTotalPopulation", &CyArea::getTotalPopulation, "int () - Returns the total population for all players in this area")
		.def("getNumStartingPlots", &CyArea::getNumStartingPlots, "int () - total number of players that are starting on this area")
		.def("isWater", &CyArea::isWater, "bool () - Returns whether or not this area is water")

		.def("getUnitsPerPlayer", &CyArea::getUnitsPerPlayer, "int (int (PlayerTypes) iIndex) - Returns the number of units in this area for this player")
		.def("getCitiesPerPlayer", &CyArea::getCitiesPerPlayer, "int (int (PlayerTypes) iIndex) - Returns the number of cities in this area for this player")
		.def("getBuildingHappiness", &CyArea::getBuildingHappiness, "int (int (PlayerTypes) iIndex) - Returns ?")
		.def("getPower", &CyArea::getPower, "int (int (PlayerTypes) iIndex) - Returns power of this area for this player")
		.def("getBestFoundValue", &CyArea::getBestFoundValue, "int (int (PlayerTypes) iIndex) - Returns the best found value for a plot in this area")


		.def("isCleanPower", &CyArea::isCleanPower, "bool (int (TeamTypes) iIndex)")
		.def("isBorderObstacle", &CyArea::isBorderObstacle, "bool (int (TeamTypes) iIndex)")

		.def("getYieldRateModifier", &CyArea::getYieldRateModifier, "int (int (PlayerTypes) iPlayer, int (YieldTypes) iIndex2 - Returns ?")

		.def("getNumBonuses", &CyArea::getNumBonuses, "int(BonusID) - total # of BonusID")
		.def("getNumTotalBonuses", &CyArea::getNumTotalBonuses, "int () - total number of bonuses, of all types")
/************************************************************************************************/
/* Afforess	                  Start		 07/15/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		.def("changeCleanPowerCount", &CyArea::changeCleanPowerCount, "int(TeamTypes eIndex, int iChange)")
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	;
}
