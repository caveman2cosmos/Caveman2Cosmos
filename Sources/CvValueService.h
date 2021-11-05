#pragma once

#ifndef CV_VALUESERVICE
#define CV_VALUESERVICE
#include <vector>

class CvValueService
{
public:
	static std::vector<int> CalculateOutPutRatio(int food, int production, int commerce);
	static void WeighOutputs(double foodWeight, double productionWeight, double commerceWeight, std::vector<int>& ratios);
	static double CalculateFoodImportance(int citySize, int era, int health, int waste);
	static double CalculatePlotValue(std::vector<int> *ratios, int food, int production, int commerce);
};

#endif

