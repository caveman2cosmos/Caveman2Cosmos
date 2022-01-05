#pragma once

#ifndef CV_VALUESERVICE
#define CV_VALUESERVICE

class CvPlot;
struct OutputRatios;

class CvValueService
{
public:
	static double CalculateFoodImportance(int citySize, int era, int health, int waste);
	static int CalculateUtilityValue(CvPlot* plot);
	static int CalculateCityPlotValue(OutputRatios& ratios, CvPlot* plot);
	static int CalculateEmpirePlotValue(CvPlot* plot);
};

#endif

