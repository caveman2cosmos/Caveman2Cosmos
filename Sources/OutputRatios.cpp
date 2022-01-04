#include "OutputRatios.h"

#include <algorithm>
#include <string>

//disable double to int conversion warning
#pragma warning( disable : 4244 )

OutputRatios::OutputRatios(const int food, const int production, const int commerce)
{
	const int totalOutput = std::max(1, food + production + commerce);

	food_ratio = 100 - ((food * 100) / totalOutput);
	production_ratio = 100 - ((production * 100) / totalOutput);
	commerce_ratio = 100 - ((commerce * 100) / totalOutput);
}
void OutputRatios::IncreaseOutputWeights(const double foodWeight, const double productionWeight, const double commerceWeight)
{
	food_ratio *= foodWeight;
	production_ratio *= productionWeight;
	commerce_ratio *= commerceWeight;
}
void OutputRatios::IncreaseFoodWeight(const double foodWeight)
{
	food_ratio *= foodWeight;
}
void OutputRatios::DecraseFoodWeight(const double foodWeight)
{
	if (foodWeight > 0) food_ratio /= foodWeight;
}
void OutputRatios::IncreaseProductionWeight(const double productionWeight)
{
	production_ratio *= productionWeight;
}
void OutputRatios::DecraseProductionWeight(const double productionWeight)
{
	if (productionWeight > 0) production_ratio /= productionWeight;
}
void OutputRatios::IncreaseCommerceWeight(const double commerceWeight)
{
	commerce_ratio *= commerceWeight;
}
void OutputRatios::DecraseCommerceWeight(const double commerceWeight)
{
	if (commerceWeight > 0) commerce_ratio /= commerceWeight;
}

int OutputRatios::CalculateOutputValue(const int food, const int production, const int commerce) const
{
	return (food_ratio * food) + (production_ratio * production) + (commerce_ratio * commerce);
}