#pragma once

#ifndef CIV_OUTPUTRATIOS_H
#define CIV_OUTPUTRATIOS_H

class CvPlayer;

struct OutputRatios
{
	OutputRatios();
	OutputRatios(int food, int production, int commerce);
	void reset();
	void WeightOutputs(int foodWeight, int productionWeight, int commerceWeight);
	void WeightFood(int foodWeight);
	void WeightProduction(int productionWeight);
	void WeightCommerce(int commerceWeight);
	int CalculateOutputValue(int food, int production, int commerce) const;
	void addLeaderFlavorWeight(const CvPlayer& player);
	static void readDefaultWeightsFromIniFile();
	int food_ratio;
	int production_ratio;
	int commerce_ratio;
private:
	static YieldArray default_weight;
};

#endif