#include "OutputRatios.h"

#include <algorithm>
#include <string>

OutputRatios::OutputRatios(const int food, const int production, const int commerce)
{
	const int totalOutput = std::max(1, food + production + commerce);

	food_ratio = 100 - ((food * 100) / totalOutput);
	production_ratio = 100 - ((production * 100) / totalOutput);
	commerce_ratio = 100 - ((commerce * 100) / totalOutput);
}
void OutputRatios::WeightOutputs(const int foodWeight, const int productionWeight, const int commerceWeight)
{
	food_ratio *= foodWeight;
	production_ratio *= productionWeight;
	commerce_ratio *= commerceWeight;
}
void OutputRatios::WeightFood(const int foodWeight)
{
	food_ratio *= foodWeight;
}
void OutputRatios::WeightProduction(const int productionWeight)
{
	production_ratio *= productionWeight;
}
void OutputRatios::WeightCommerce(const int commerceWeight)
{
	commerce_ratio *= commerceWeight;
}

int OutputRatios::CalculateOutputValue(const int food, const int production, const int commerce) const
{
	return (food_ratio * food) + (production_ratio * production) + (commerce_ratio * commerce);
}