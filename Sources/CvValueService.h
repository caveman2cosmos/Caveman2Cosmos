#pragma once

#ifndef CV_VALUESERVICE
#define CV_VALUESERVICE

#include "OutputRatios.h"

class CvValueService
{
public:
	static double CalculateFoodImportance(int citySize, int era, int health, int waste);
};

#endif

