#include "CvGameCoreDLL.h"

//
// Python interface for CvProperties class
// simple enough to be exposed directly - no wrapper
//

void CyPropertiesPythonInterface()
{
	OutputDebugString("Python Extension Module - CyPropertiesPythonInterface\n");

	python::class_<CvProperties>("CvProperties")

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
}