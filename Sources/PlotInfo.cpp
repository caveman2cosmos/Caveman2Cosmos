#include "PlotInfo.h"

#include <sstream>
#include <string>

#include "CvEnums.h"

plotInfo::plotInfo() :
	index(0),
	worked(false),
	owned(false),
	bonusImproved(false),
	yieldValue(0), yields(),
	currentBonus(NO_BONUS),
	currentImprovement(NO_IMPROVEMENT),
	currentFeature(NO_FEATURE),
	currentBuild(NO_BUILD)
{
}

std::string plotInfo::ToJSON()
{
	std::ostringstream oss;
	oss << "{ plotIndex: " << index << ", worked: " << worked << ", owned:" << owned << ", yieldValue: " << yieldValue
		<< ",yields: { food: " << yields[YIELD_FOOD] << ", production: " << yields[YIELD_PRODUCTION] << ", commerce: " << yields[YIELD_COMMERCE]
		<< "}, currentBuild: " << currentBuild << " ,currentImprovement: " << currentImprovement << ", currentFeature: " << currentFeature << "}" << std::endl;

	const std::string output = oss.str();
	return output;
}