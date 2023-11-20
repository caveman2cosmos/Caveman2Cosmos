#include "CvGameCoreDLL.h"
#include "CvRandom.h"
#include <boost/python/overloads.hpp>

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(CvRandom_get_overloads, CvRandom::get, 1, 2);

//
// published python interface for CvRandom
//
void CyRandomPythonInterface()
{
	OutputDebugString("Python Extension Module - CyRandomPythonInterface\n");

	boost::python::class_<CvRandom>("CyRandom")
		.def("get", &CvRandom::get, CvRandom_get_overloads())
		.def("init", &CvRandom::init, "void (unsigned long int ulSeed)")
		.def("getSeed", &CvRandom::getSeed, "int () current seed")
		;
}
