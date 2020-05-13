#pragma once

#ifndef SC_DEBUG_H
#define SC_DEBUG_H

#include "SCvDebug.h"

class SCyDebug
{
public:
	bool checkMode(SCvDebug::Mode mode);
	void setMode(SCvDebug::Mode mode, bool on);

	static void installInPython();
};

#endif
