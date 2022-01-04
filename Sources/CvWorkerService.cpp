#include "CvGlobals.h"
#include "CvWorkerService.h"
#include "CvPlot.H"


bool CvWorkerService::ShouldImproveCity(CvCity* targetCity)
{
	if (targetCity == NULL) return false;
	foreach_(const CvPlot * pLoopPlot, targetCity->plots())
	{
		const int plotIndex = targetCity->getCityPlotIndex(pLoopPlot);
		if (pLoopPlot != NULL
		&& pLoopPlot->getWorkingCity() == targetCity
		&& pLoopPlot->isBeingWorked() // plot should be improved even if it is not currently worked by the city
		&& pLoopPlot->getImprovementType() == NO_IMPROVEMENT //should be considered for improvement even if there already is an improvement there?
		&& targetCity->AI_getBestBuildValue(plotIndex) > 0
		&& GC.getBuildInfo((BuildTypes)targetCity->AI_getBestBuild(plotIndex)).getImprovement() != NO_IMPROVEMENT)
		{
			return true;
		}
	}
	return false;

}