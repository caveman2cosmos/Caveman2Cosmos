#include "CvGameCoreDLL.h"
#include "IntExpr.h"

void CyIntExprPythonInterface()
{
	python::class_<IntExpr, boost::noncopyable>("IntExpr", python::no_init)
	;

	python::class_<IntExprConstant, python::bases<IntExpr> >("IntExprConstant", python::no_init)
		.def_readonly("iValue", &IntExprConstant::m_iValue)
	;
}