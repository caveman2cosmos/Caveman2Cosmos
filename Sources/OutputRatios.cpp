#include "CvGameCoreDLL.h"
#include "CvDLLIniParserIFaceBase.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvPlayer.h"
#include "OutputRatios.h"

#include <algorithm>
#include <string>

YieldArray OutputRatios::default_weight = { 18, 10, 6 };

OutputRatios::OutputRatios()
{
	reset();
}
OutputRatios::OutputRatios(const int food, const int production, const int commerce)
{
	const int totalOutput = std::max(1, food + production + commerce);

	food_ratio = 100 - ((food * 100) / totalOutput);
	production_ratio = 100 - ((production * 100) / totalOutput);
	commerce_ratio = 100 - ((commerce * 100) / totalOutput);
}
void OutputRatios::reset()
{
	food_ratio       = default_weight[YIELD_FOOD];
	production_ratio = default_weight[YIELD_PRODUCTION];
	commerce_ratio   = default_weight[YIELD_COMMERCE];
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

void OutputRatios::addLeaderFlavorWeight(const CvPlayer& player)
{
	if (const int* flavors = GC.getLeaderHeadInfo(player.getPersonalityType()).getFlavorValues())
	{
		const int foodFlavor = flavors[GC.getFLAVOR_GROWTH()];
		if (foodFlavor != 0)
			food_ratio *= foodFlavor;

		const int productionFlavor = flavors[GC.getFLAVOR_PRODUCTION()];
		if (productionFlavor != 0)
			production_ratio *= productionFlavor;

		const int commerceFlavor = flavors[GC.getFLAVOR_SCIENCE()];
		if (commerceFlavor != 0)
			commerce_ratio *= commerceFlavor;
	}
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