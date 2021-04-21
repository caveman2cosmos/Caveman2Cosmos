#pragma once

#ifndef CVPLOT_EVAL_H
#define CVPLOT_EVAL_H
#include "CvPlot.h"


class CvPlotEvaluation
{
protected: 
	CvPlot* pPlot;
public:
	CvPlotEvaluation(CvPlot* plotPointer);
	int CvPlotEvaluation::evaluate(CvPlot* plot, int foodWeight, int productionWeight, int commerceWeight);
};

#endif CVPLOT_EVAL_H
