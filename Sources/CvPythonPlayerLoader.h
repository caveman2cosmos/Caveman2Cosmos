#pragma once


#ifndef Cv_Python_Player_Loader
#define Cv_Python_Player_Loader

#include "CyPlayer.h"
#include <boost/python/class.hpp>

class CvPythonPlayerLoader
{
public:
	static void CyPlayerPythonInterface1(boost::python::class_<CyPlayer>& inst);
	static void CyPlayerPythonInterface2(boost::python::class_<CyPlayer>& inst);
	static void CyPlayerPythonInterface3(boost::python::class_<CyPlayer>& inst);
};

#endif