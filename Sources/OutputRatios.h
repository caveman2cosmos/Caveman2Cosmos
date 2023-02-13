#pragma once

#ifndef CIV_OUTPUTRATIOS_H
#define CIV_OUTPUTRATIOS_H
struct OutputRatios
{
	OutputRatios(int food, int production, int commerce);
	static void setBaseOutputWeights(int foodWeight, int productionWeight, int commerceWeight);
	void IncreaseOutputWeights(int foodWeight, int productionWeight, int commerceWeight);
	void IncreaseFoodWeight(const int foodWeight);
	void DecraseFoodWeight(const int foodWeight);
	void IncreaseProductionWeight(const int productionWeight);
	void DecraseProductionWeight(const int productionWeight);
	void IncreaseCommerceWeight(const int commerceWeight);
	void DecraseCommerceWeight(const int commerceWeight);
	int CalculateOutputValue(int food, int production, int commerce) const;
	int food_ratio;
	int production_ratio;
	int commerce_ratio;
	/*
	int food_weight;
	int production_weight;
	int commerce_weight;
	*/
};

#endif