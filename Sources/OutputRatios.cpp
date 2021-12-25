#include "CvGameCoreDLL.h"
#include "CvDLLIniParserIFaceBase.h"
#include "OutputRatios.h"

#include <algorithm>
#include <string>

YieldArray OutputRatios::default_weight = { 18, 10, 6 };

OutputRatios::OutputRatios()
	: food_ratio(default_weight[YIELD_FOOD])
	, production_ratio(default_weight[YIELD_PRODUCTION])
	, commerce_ratio(default_weight[YIELD_COMMERCE])
{}
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

void OutputRatios::readDefaultWeightsFromIniFile()
{
	CvDLLIniParserIFaceBase* parserIFace = gDLL->getIniParserIFace();

	FIniParser* parser = parserIFace->create((getModDir() + "/Caveman2Cosmos.ini").c_str());
	FAssert(parser != NULL);

	const bool bSectionSet = parserIFace->SetGroupKey(parser, "AI");
	FAssertMsg(bSectionSet, "Ini file section not found: AI");

	const bool bFoodSet = parserIFace->GetKeyValue(parser, "FoodWeight", &default_weight[YIELD_FOOD]);
	FAssertMsg(bFoodSet, "Ini file option not found: FoodWeight");

	const bool bProductionSet = parserIFace->GetKeyValue(parser, "ProductionWeight", &default_weight[YIELD_PRODUCTION]);
	FAssertMsg(bProductionSet, "Ini file option not found: ProductionWeight");

	const bool bCommerceSet = parserIFace->GetKeyValue(parser, "CommerceWeight", &default_weight[YIELD_COMMERCE]);
	FAssertMsg(bCommerceSet, "Ini file option not found: CommerceWeight");

	parserIFace->destroy(parser);
}