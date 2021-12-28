#pragma once

#ifndef CIV_OUTPUTRATIOS_H
#define CIV_OUTPUTRATIOS_H
struct OutputRatios
{
	OutputRatios(int food, int production, int commerce);
	void WeightOutputs(int foodWeight, int productionWeight, int commerceWeight);
	void WeightFood(int foodWeight);
	void WeightProduction(int productionWeight);
	void WeightCommerce(int commerceWeight);
	int CalculateOutputValue(int food, int production, int commerce) const;
	int food_ratio;
	int production_ratio;
	int commerce_ratio;
};

#endif