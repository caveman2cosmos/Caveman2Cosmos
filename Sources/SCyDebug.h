#pragma once

#include "SCvDebug.h"

class SCyDebug
{
public:
	bool checkMode(SCvDebug::Mode mode) const;
	void setMode(SCvDebug::Mode mode, bool on);

	static void installInPython();
};
