#include "CvGameCoreDLL.h"
#include "BoolExpr.h"

void CyBoolExprPythonInterface()
{
	python::enum_<GOMTypes>("GOMTypes")
		.value("NO_GOM", NO_GOM)
		.value("GOM_BUILDING", GOM_BUILDING)
		.value("GOM_BONUS", GOM_BONUS)
		.value("GOM_TECH", GOM_TECH)
	;

	python::enum_<BoolExprTypes>("BoolExprTypes")
		.value("NO_BOOLEXPR", NO_BOOLEXPR)
		.value("BOOLEXPR_HAS", BOOLEXPR_HAS)
		.value("BOOLEXPR_AND", BOOLEXPR_AND)
		.value("BOOLEXPR_OR", BOOLEXPR_OR)
	;

	python::class_<BoolExpr, boost::noncopyable>("CyBoolExpr", python::no_init)
		.def("getType", &BoolExpr::getType)
	;

	python::class_<BoolExprHas, python::bases<BoolExpr> >("CyBoolExprHas", python::no_init)
		.def("getType", &BoolExprHas::getType)
		.def("getGOMType", &BoolExprHas::getGOMType)
		.def("getID", &BoolExprHas::getID)
	;

	python::class_<BoolExprAnd, python::bases<BoolExpr> >("CyBoolExprAnd", python::no_init)
		.def("getType", &BoolExprAnd::getType)
		.def("getFirstExpr", &BoolExprAnd::getFirstExpr, python::return_value_policy<python::reference_existing_object>())
		.def("getSecondExpr", &BoolExprAnd::getSecondExpr, python::return_value_policy<python::reference_existing_object>())
	;

	python::class_<BoolExprOr, python::bases<BoolExpr> >("CyBoolExprOr", python::no_init)
		.def("getType", &BoolExprOr::getType)
		.def("getFirstExpr", &BoolExprOr::getFirstExpr, python::return_value_policy<python::reference_existing_object>())
		.def("getSecondExpr", &BoolExprOr::getSecondExpr, python::return_value_policy<python::reference_existing_object>())
	;
}