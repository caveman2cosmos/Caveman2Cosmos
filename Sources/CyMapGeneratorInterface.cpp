#include "CvGameCoreDLL.h"
#include "CyMapGenerator.h"

//
// published python interface for CyMapGenerator
//

void CyMapGeneratorPythonInterface()
{
	OutputDebugString("Python Extension Module - CyMapGeneratorPythonInterface\n");

	python::class_<CyMapGenerator>("CyMapGenerator")
		.def("doRiver", &CyMapGenerator::doRiver, "void (CyPlot* pStartPlot, int /*CardinalDirectionTypes*/ eCardinalDirection)")
		.def("addFeatures", &CyMapGenerator::addFeatures, "void ()")
		.def("addBonuses", &CyMapGenerator::addBonuses, "void ()")

		.def("generatePlotTypes", &CyMapGenerator::generatePlotTypes, "void ()")
		.def("generateTerrain", &CyMapGenerator::generateTerrain, "void ()")

		.def("setPlotTypes", &CyMapGenerator::setPlotTypes, "void (list lPlotTypes) - set plot types to the contents of the given list")
		;
}
