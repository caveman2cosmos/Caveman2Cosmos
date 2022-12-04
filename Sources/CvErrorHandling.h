#pragma once

#ifndef CV_ERROR_HANDLING_H
#define CV_ERROR_HANDLING_H

#include "FAssert.h"
#include <exception>

class CvErrorHandling
{
public:
	static void assertFail(const FAssertInfo& info, bool ensure);
	static void error(const FAssertInfo& info, bool ensure);
	static void fatal(const std::exception& ex);
};

#endif