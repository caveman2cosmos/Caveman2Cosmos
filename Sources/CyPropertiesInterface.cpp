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
		.def("getSources", &CvPropertyManipulators::cyGetSources)
	;

	python::class_<CvPropertySource>("CvPropertySource", python::no_init)
		.def("getType", &CvPropertySource::getType)
		.def("getProperty", &CvPropertySource::getProperty)
		.def("getObjectType", &CvPropertySource::getObjectType)
		.def("getRelation", &CvPropertySource::getRelation)
		.def("getRelationData", &CvPropertySource::getRelationData)
		.def("getIsActiveExpr", &CvPropertySource::getIsActiveExpr, python::return_value_policy<python::reference_existing_object>())
	;

	python::class_<CvPropertySourceConstant, python::bases<CvPropertySource> >("CvPropertySourceConstant", python::no_init)
		.def("getType", &CvPropertySourceConstant::getType)
		.def("getAmountPerTurnExpr", &CvPropertySourceConstant::getAmountPerTurnExpr, python::return_value_policy<python::reference_existing_object>())
	;

	python::class_<CvPropertySourceConstantLimited, python::bases<CvPropertySource> >("CvPropertySourceConstantLimited", python::no_init)
		.def("getType", &CvPropertySourceConstantLimited::getType)
		.def("getAmountPerTurn", &CvPropertySourceConstantLimited::getAmountPerTurn)
		.def("getLimit", &CvPropertySourceConstantLimited::getLimit)
	;

	python::class_<CvPropertySourceDecay, python::bases<CvPropertySource> >("CvPropertySourceDecay", python::no_init)
		.def("getType", &CvPropertySourceDecay::getType)
		.def("getPercent", &CvPropertySourceDecay::getPercent)
		.def("getNoDecayAmount", &CvPropertySourceDecay::getNoDecayAmount)
	;

	python::class_<CvPropertySourceAttributeConstant, python::bases<CvPropertySource> >("CvPropertySourceAttributeConstant", python::no_init)
		.def("getType", &CvPropertySourceAttributeConstant::getType)
		.def("getAttribute", &CvPropertySourceAttributeConstant::getAttribute)
		.def("getAmountPerTurn", &CvPropertySourceAttributeConstant::getAmountPerTurn)
	;
}