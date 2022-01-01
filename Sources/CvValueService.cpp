#include "CvPlot.h"
#include "CvValueService.h"
#include "OutputRatios.h"



double CvValueService::CalculateFoodImportance(int citySize, int era, int health, int waste)
{
	return 0;
}

int CvValueService::CalculateUtilityValue(CvPlot* plot)
{
	if (plot->getWorkingCity() != NULL)
	{
		return 0;
	}
	return 0;
}

int CvValueService::CalculateCityPlotValue(OutputRatios& ratios, CvPlot* plot)
{
	return 0;
}

int CvValueService::CalculateEmpirePlotValue(CvPlot* plot)
{
	return 0;
}




