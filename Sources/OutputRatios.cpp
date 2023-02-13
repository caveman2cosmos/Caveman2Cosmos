#include "OutputRatios.h"

#include <algorithm>
#include <string>

// Class variables with default values
static int iStaticWeightFood = 10;
static int iStaticWeightHammer = 8;
static int iStaticWeightCommerce = 6;

OutputRatios::OutputRatios(const int food, const int production, const int commerce)
{
	const int totalOutput = std::max(1, food + production + commerce);

	/* City unique values not implemented yet, use default for all for now
	food_weight = iStaticWeightFood;
	production_weight = iStaticWeightHammer;
	commerce_weight = iStaticWeightCommerce;
	*/

	food_ratio = 100 - ((food * 100) / totalOutput);
	production_ratio = 100 - ((production * 100) / totalOutput);
	commerce_ratio = 100 - ((commerce * 100) / totalOutput);
}

void OutputRatios::setBaseOutputWeights(int foodWeight, int productionWeight, int commerceWeight)
{
	iStaticWeightFood = foodWeight;
	iStaticWeightHammer = productionWeight;
	iStaticWeightCommerce = commerceWeight;
}

void OutputRatios::IncreaseOutputWeights(const int foodWeight, const int productionWeight, const int commerceWeight)
{
	food_ratio *= foodWeight;
	production_ratio *= productionWeight;
	commerce_ratio *= commerceWeight;
}
void OutputRatios::IncreaseFoodWeight(const int foodWeight)
{
	food_ratio += foodWeight;
}
void OutputRatios::DecraseFoodWeight(const int foodWeight)
{
	if (foodWeight > 0) food_ratio -= foodWeight;
}
void OutputRatios::IncreaseProductionWeight(const int productionWeight)
{
	production_ratio += productionWeight;
}
void OutputRatios::DecraseProductionWeight(const int productionWeight)
{
	if (productionWeight > 0) production_ratio -= productionWeight;
}
void OutputRatios::IncreaseCommerceWeight(const int commerceWeight)
{
	commerce_ratio += commerceWeight;
}
void OutputRatios::DecraseCommerceWeight(const int commerceWeight)
{
	if (commerceWeight > 0) commerce_ratio -= commerceWeight;
}

int OutputRatios::CalculateOutputValue(const int food, const int production, const int commerce) const
{
	return (iStaticWeightFood * food) + (iStaticWeightHammer * production) + (iStaticWeightCommerce * commerce);
}