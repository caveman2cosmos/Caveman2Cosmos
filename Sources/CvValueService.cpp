#include "CvValueService.h"

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

int CvValueService::CalculateCityPlotValue(OutputRatios& ratios, short yields[], bool hasBonus, bool activatesBonus)
	{
	int plotValue = ratios.CalculateOutputValue(yields[YIELD_FOOD], yields[YIELD_PRODUCTION], yields[YIELD_COMMERCE]);

	if (hasBonus) {
		plotValue = activatesBonus ? plotValue * 5 : plotValue / 5;
	}
	return plotValue;
}

int CvValueService::CalculateEmpirePlotValue(CvPlot* plot)
{
	return 0;
}




