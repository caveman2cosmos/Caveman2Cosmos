#include "CvGameCoreDLL.h"
#include "CyArtFileMgr.h"

//
// published python interface for CyArtFileMgr
//

void CyArtFileMgrPythonInterface()
{
	OutputDebugString("Python Extension Module - CyArtFileMgrPythonInterface\n");

	python::class_<CyArtFileMgr>("CyArtFileMgr")
		.def("getInterfaceArtInfo", &CyArtFileMgr::getInterfaceArtInfo,  python::return_value_policy<python::reference_existing_object>(), "CvArtInfoInterface ()")
		.def("getMovieArtInfo", &CyArtFileMgr::getMovieArtInfo,  python::return_value_policy<python::reference_existing_object>(), "CvArtInfoMovie ()")
		.def("getMiscArtInfo", &CyArtFileMgr::getMiscArtInfo, python::return_value_policy<python::reference_existing_object>(), "CvArtInfoMisc ()")
		.def("getUnitArtInfo", &CyArtFileMgr::getUnitArtInfo, python::return_value_policy<python::reference_existing_object>(), "CvArtInfoUnit ()")
		.def("getBuildingArtInfo", &CyArtFileMgr::getBuildingArtInfo, python::return_value_policy<python::reference_existing_object>(), "CvArtInfoBuilding ()")
		.def("getCivilizationArtInfo", &CyArtFileMgr::getCivilizationArtInfo, python::return_value_policy<python::reference_existing_object>(), "CvArtInfoCivilization ()")
		.def("getBonusArtInfo", &CyArtFileMgr::getBonusArtInfo, python::return_value_policy<python::reference_existing_object>(), "CvArtInfoBonus ()")
		.def("getImprovementArtInfo", &CyArtFileMgr::getImprovementArtInfo, python::return_value_policy<python::reference_existing_object>(), "CvArtInfoImprovement ()")
	;
}
