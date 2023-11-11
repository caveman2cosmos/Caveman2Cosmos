#pragma once

#ifndef Cv_Python_City_Loader
#define Cv_Python_City_Loader

#include "CvGameCoreDLL.h"
#include <boost/python/class.hpp>

class CyCity;

class CvPythonCityLoader
{
public:
	static void CyCityPythonInterface1(boost::python::class_<CyCity>& inst);
	static void CyCityPythonInterface2(boost::python::class_<CyCity>& inst);
};

#endif // Cv_Python_City_Loader