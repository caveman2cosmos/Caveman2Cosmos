#pragma once

#ifndef Cv_Python_GlobalContext_Loader
#define Cv_Python_GlobalContext_Loader

#include <boost/python/class.hpp>
class CyGlobalContext;

class CvPythonGlobalContextLoader
{
public:
	static void CyGlobalContextPythonInterface1(boost::python::class_<CyGlobalContext>& x);
	static void CyGlobalContextPythonInterface2(boost::python::class_<CyGlobalContext>& x);
	static void CyGlobalContextPythonInterface3(boost::python::class_<CyGlobalContext>& x);
	static void CyGlobalContextPythonInterface4(boost::python::class_<CyGlobalContext>& x);
};

#endif
