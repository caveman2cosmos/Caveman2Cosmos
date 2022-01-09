#pragma once

#ifndef CIV_OUTPUTRATIOS_H
#define CIV_OUTPUTRATIOS_H
struct OutputRatios
{
	OutputRatios(int food, int production, int commerce);
	void IncreaseOutputWeights(double foodWeight, double productionWeight, double commerceWeight);
	void IncreaseFoodWeight(const double foodWeight);
	void DecraseFoodWeight(const double foodWeight);
	void IncreaseProductionWeight(const double productionWeight);
	void DecraseProductionWeight(const double productionWeight);
	void IncreaseCommerceWeight(const double commerceWeight);
	void DecraseCommerceWeight(const double commerceWeight);
	int CalculateOutputValue(int food, int production, int commerce) const;
	int food_ratio;
	int production_ratio;
	int commerce_ratio;
};

#endif