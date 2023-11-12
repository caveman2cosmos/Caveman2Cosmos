#pragma once

#ifndef Cv_Python_Plot_Loader
#define Cv_Python_Plot_Loader

#include "CvGameCoreDLL.h"
#include "CyPlot.h"
#include <boost/python/class.hpp>

class CyPlot;

class CvPythonPlotLoader
{
public:
	static void CyPlotPythonInterface1(boost::python::class_<CyPlot>& x);
};


#endif