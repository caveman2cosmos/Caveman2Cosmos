#pragma once

#ifndef Cv_Python_Unit_Loader
#define Cv_Python_Unit_Loader

#include <boost/python/class.hpp>
#include "CyUnit.h"


class CvPythonUnitLoader
{
public:
	static void CyUnitPythonInterface1(boost::python::class_<CyUnit>& inst);
};


#endif // Cv_Python_Unit_Loader