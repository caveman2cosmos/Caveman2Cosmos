#pragma once

#ifndef CV_ERROR_HANDLING_H
#define CV_ERROR_HANDLING_H

#include "FAssert.h"
#include <exception>

class CvErrorHandling
{
public:
	static void assertFail(const FAssertInfo& info);
	static void ensureFail(const FAssertInfo& info);
	static void error(const FAssertInfo& info);
	static void fatal(const std::exception& ex);
};

#endif