#include "CvGameCoreDLL.h"
#include "SCyDebug.h"

bool SCyDebug::checkMode(SCvDebug::Mode mode) const
{
	return SCvDebug::checkMode(mode);
}

void SCyDebug::setMode(SCvDebug::Mode mode, bool on)
{
	SCvDebug::setMode(mode, on);
}

void SCyDebug::installInPython()
{
	OutputDebugString("Python Extension Module - SCvDebugInterface\n");

	python::class_<SCyDebug>("SCyDebug")
		.def("checkMode", &SCyDebug::checkMode, "bool (Mode)")
		.def("setMode", &SCyDebug::setMode, "void (Mode, bool)");

	python::enum_<SCvDebug::Mode>("SCyDebug_Mode")
		.export_values();
}
