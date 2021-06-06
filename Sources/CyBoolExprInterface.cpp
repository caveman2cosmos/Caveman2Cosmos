#include "CvGameCoreDLL.h"
#include "BoolExpr.h"
//#include "CyBoolExpr.h"

void CyBoolExprPythonInterface()
{
	python::class_<BoolExpr>("CyBoolExpr")
		.def("getType", &BoolExpr::getType)
	;

	python::class_<BoolExprHas, python::bases<BoolExpr> >("CyBoolExprHas")
		.def("getType", &BoolExprHas::getType)
		.def("getGOMType", &BoolExprHas::getGOMType)
		.def("getID", &BoolExprHas::getID)
	;

	python::class_<BoolExprAnd, python::bases<BoolExpr> >("CyBoolExprAnd")
		.def("getType", &BoolExprAnd::getType)
		.def("getFirstExpr", &BoolExprAnd::getFirstExpr, python::return_value_policy<python::reference_existing_object>())
		.def("getSecondExpr", &BoolExprAnd::getSecondExpr, python::return_value_policy<python::reference_existing_object>())
	;

	python::enum_<BoolExprTypes>("BoolExprTypes")
		.value("NO_BOOLEXPR", NO_BOOLEXPR)
		.value("BOOLEXPR_HAS", BOOLEXPR_HAS)
		.value("BOOLEXPR_AND", BOOLEXPR_AND)
	;

	python::enum_<GOMTypes>("GOMTypes")
		.value("NO_GOM", NO_GOM)
		.value("GOM_BONUS", GOM_BONUS)
	;
}