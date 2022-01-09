#include "PlotInfo.h"

#include <sstream>
#include <string>

#include "CvEnums.h"

plotInfo::plotInfo() :
	index(0),
	worked(false),
	owned(false),
	bonusImproved(false),
	value(0),
	newValue(0),
	yields(),
	newYields(),
	bonus(NO_BONUS),
	currentImprovement(NO_IMPROVEMENT),
	newImprovement(NO_IMPROVEMENT),
	currentFeature(NO_FEATURE),
	newFeature(NO_FEATURE),
	currentBuild(NO_BUILD),
	newBuild(NO_BUILD)
{
}

std::string plotInfo::ToJSON()
{
	std::ostringstream oss;
	oss << "{ plotIndex: " << index << ", worked: " << worked << ", owned:" << owned << ", yieldValue: " << value
		<< ",yields: { food: " << yields[YIELD_FOOD] << ", production: " << yields[YIELD_PRODUCTION] << ", commerce: " << yields[YIELD_COMMERCE]
		<< "}, currentBuild: " << currentBuild << " ,currentImprovement: " << currentImprovement << ", currentFeature: " << currentFeature << "}" << std::endl;

	const std::string output = oss.str();
	return output;
}