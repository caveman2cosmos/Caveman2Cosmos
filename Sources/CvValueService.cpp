#include "CvValueService.h"
#include "CvEnums.h"

std::vector<int> CvValueService::CalculateOutPutRatio(int food, int production, int commerce)
{
	std::vector<int> ratios = std::vector<int>(NUM_YIELD_TYPES);

	const int totalOutput = std::max(1, food + production + commerce);

	const int foodRatio = 100 - ((food * 100) / totalOutput);
	const int commerceRatio = 100 - ((commerce * 100) / totalOutput);
	const int productionRatio = 100 - ((production * 100) / totalOutput);

	ratios[YIELD_FOOD] = foodRatio;
	ratios[YIELD_PRODUCTION] = productionRatio;
	ratios[YIELD_COMMERCE] = commerceRatio;

	return ratios;
}

void CvValueService::WeighOutputs(double foodWeight, double productionWeight, double commerceWeight,
	std::vector<int>& ratios)
{
	ratios[YIELD_FOOD] = static_cast<int>(foodWeight * ratios[YIELD_FOOD]);
	ratios[YIELD_PRODUCTION] = static_cast<int>(productionWeight * ratios[YIELD_PRODUCTION]);
	ratios[YIELD_COMMERCE] = static_cast<int>(commerceWeight * ratios[YIELD_COMMERCE]);
}

double CvValueService::CalculateFoodImportance(int citySize, int era, int health, int waste)
{
	return 0;
}

double CvValueService::CalculatePlotValue(std::vector<int>* ratios, int food, int production, int commerce)
{
	return 0;
}

