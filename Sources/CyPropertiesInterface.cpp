#include "CvGameCoreDLL.h"
#include "BoolExpr.h"
#include "IntExpr.h"

//
// Python interface for CvProperties class
// simple enough to be exposed directly - no wrapper
//

void CyPropertiesPythonInterface()
{
	OutputDebugString("Python Extension Module - CyPropertiesPythonInterface\n");

	python::class_<CvProperties>("CvProperties", python::no_init)

		.def("getProperty", &CvProperties::getProperty, "int (int)")
		.def("getValue", &CvProperties::getValue, "int (int)")
		.def("setValue", &CvProperties::setValue, "void (int, int)")
		.def("changeValue", &CvProperties::changeValue, "void (int, int)")
		.def("getNumProperties", &CvProperties::getNumProperties, "int ()")
		.def("getValueByProperty", &CvProperties::getValueByProperty, "int (int)")
		.def("setValueByProperty", &CvProperties::setValueByProperty, "void (int, int)")
		.def("changeValueByProperty", &CvProperties::changeValueByProperty, "void (int, int)")
		.def("getChangeByProperty", &CvProperties::getChangeByProperty, "int (int)")
		.def("getPropertyDisplay", &CvProperties::getPropertyDisplay, "wstring (int)")
		;

	python::class_<CvPropertyManipulators>("CvPropertyManipulators", python::no_init)
		.def("getNumSources", &CvPropertyManipulators::getNumSources)
		.def("getSource", &CvPropertyManipulators::getSource, python::return_value_policy<python::reference_existing_object>())
	;

	python::class_<CvPropertySource, boost::noncopyable>("CvPropertySource", python::no_init)
		.def("getProperty", &CvPropertySource::getProperty)
	;

	python::class_<CvPropertySourceConstant, python::bases<CvPropertySource> >("CvPropertySourceConstant", python::no_init)
		.def("getAmountPerTurnExpr", &CvPropertySourceConstant::getAmountPerTurnExpr, python::return_value_policy<python::reference_existing_object>())
	;
}