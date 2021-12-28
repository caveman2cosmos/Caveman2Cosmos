#include "CvGameCoreDLL.h"
#include "CyGameCoreUtils.h"
#include "CyCity.h"
#include "CyPlot.h"
#include "CyUnit.h"

//
// Python interface for CvGameCoreUtils.h.
//

void CyGameCoreUtilsPythonInterface()
{
	OutputDebugString("Python Extension Module - CyGameCoreUtilsPythonInterface\n");

	python::def("cyIntRange", cyIntRange,"int (int iNum, int iLow, int iHigh)");
	python::def("cyFloatRange", cyFloatRange,"float (float fNum, float fLow, float fHigh)");
	python::def("dxWrap", cyDxWrap,"int (int iDX)");
	python::def("dyWrap", cyDyWrap,"int (int iDY)");
	python::def("plotDistance", cyPlotDistance,"int (int iX1, int iY1, int iX2, int iY2)");
	python::def("stepDistance", cyStepDistance,"int (int iX1, int iY1, int iX2, int iY2)");
	python::def("plotDirection", cyPlotDirection, python::return_value_policy<python::manage_new_object>(), "CyPlot* (int iX, int iY, DirectionTypes eDirection)");
	python::def("plotCardinalDirection", cyPlotCardinalDirection, python::return_value_policy<python::manage_new_object>(), "CyPlot* (int iX, int iY, CardinalDirectionTypes eCardDirection)");
	python::def("splotCardinalDirection", cysPlotCardinalDirection, python::return_value_policy<python::reference_existing_object>(), "CyPlot* (int iX, int iY, CardinalDirectionTypes eCardDirection)");
	python::def("plotXY", cyPlotXY, python::return_value_policy<python::manage_new_object>(), "CyPlot* (int iX, int iY, int iDX, int iDY)");
	python::def("splotXY", cysPlotXY, python::return_value_policy<python::reference_existing_object>(), "CyPlot* (int iX, int iY, int iDX, int iDY)");
	python::def("directionXY", cyDirectionXYFromInt,"DirectionTypes (int iDX, int iDY)");
	python::def("directionXYFromPlot", cyDirectionXYFromPlot,"DirectionTypes (CyPlot* pFromPlot, CyPlot* pToPlot)");
	python::def("plotCity", cyPlotCity, python::return_value_policy<python::manage_new_object>(), "CyPlot* (int iX, int iY, int iIndex)");
	python::def("plotCityXY", cyPlotCityXYFromInt,"int (int iDX, int iDY)");
	python::def("plotCityXYFromCity", cyPlotCityXYFromCity,"int (CyCity* pCity, CyPlot* pPlot)");

	python::def("estimateDirection", cyEstimateDirection, "DirectionTypes (int iDX, int iDY)");

	python::def("atWar", cyAtWar,"bool (int eTeamA, int eTeamB)");
	python::def("isPotentialEnemy", cyIsPotentialEnemy,"bool (int eOurTeam, int eTheirTeam)");

	python::def("getCity", cyGetCity, python::return_value_policy<python::manage_new_object>(), "CyPlot* (IDInfo city)");
	python::def("getUnit", cyGetUnit, python::return_value_policy<python::manage_new_object>(), "CyUnit* (IDInfo unit)");

	python::def("finalImprovementUpgrade", cyFinalImprovementUpgrade, "int /*ImprovementTypes*/ (int /*ImprovementTypes*/ eImprovement, int iCount)");

	python::def("getWorldSizeMaxConscript", cyGetWorldSizeMaxConscript, "int (int /*CivicTypes*/ eCivic)");
	python::def("isReligionTech", cyIsReligionTech, "int (int /*TechTypes*/ eTech)");

	python::def("isTechRequiredForUnit", cyIsTechRequiredForUnit, "bool (int /*TechTypes*/ eTech, int /*UnitTypes*/ eUnit)");
	python::def("isTechRequiredForBuilding", cyIsTechRequiredForBuilding, "bool (int /*TechTypes*/ eTech, int /*BuildingTypes*/ eBuilding)");
	python::def("isTechRequiredForProject", cyIsTechRequiredForProject, "bool (int /*TechTypes*/ eTech, int /*ProjectTypes*/ eProject)");
	python::def("isWorldUnit", cyIsWorldUnit, "bool (int /*UnitTypes*/ eUnit)");
	python::def("isNationalUnit", cyIsNationalUnit, "bool (int /*UnitTypes*/ eUnit)");
	python::def("isLimitedUnit", cyIsLimitedUnit, "bool (int /*UnitTypes*/ eUnit)");
	python::def("isWorldWonder", cyIsWorldWonder, "bool (int /*BuildingTypes*/ eBuilding)");
	python::def("isTeamWonder", cyIsTeamWonder, "bool (int /*BuildingTypes*/ eBuilding)");
	python::def("isNationalWonder", cyIsNationalWonder, "bool (int /*BuildingTypes*/ eBuilding)");
	python::def("isLimitedWonder", cyIsLimitedWonder, "bool (int /*BuildingTypes*/ eBuilding)");
	python::def("isWorldProject", cyIsWorldProject, "bool (int /*ProjectTypes*/ eProject)");
	python::def("isTeamProject", cyIsTeamProject, "bool (int /*ProjectTypes*/ eProject)");
	python::def("isLimitedProject", cyIsLimitedProject, "bool (int /*ProjectTypes*/ eProject)");
	python::def("getCombatOdds", cyGetCombatOdds, "int (CyUnit* pAttacker, CyUnit* pDefender)");
	python::def("getEspionageModifier", cyGetEspionageModifier, "int (int /*TeamTypes*/ iOurTeam, int /*TeamTypes*/ iTargetTeam)");

	python::def("getOppositeCardinalDirection", getOppositeCardinalDirection,"CardinalDirectionTypes (CardinalDirectionTypes eDir)");
	python::def("cardinalDirectionToDirection", cardinalDirectionToDirection, "DirectionTypes (CardinalDirectionTypes eDir) - converts a CardinalDirectionType to the corresponding DirectionType");

	python::def("isCardinalDirection", isCardinalDirection,"bool (DirectionTypes eDirection)");

	python::def("getTechScore", getTechScore, "int (int /*TechTypes*/ eTech)");
	python::def("getWonderScore", getWonderScore, "int (int /*BuildingTypes*/ eWonder)");

	python::def("intSqrt", cyIntSqrt64, "int64_t (uint64_t iValue)");

	python::def("getModDir", getModDir);
}
