#include "CvGameCoreDLL.h"
#include "CityOutputHistory.h"
//
// Python interface for CityOutputHistory class
// simple enough to be exposed directly - no wrapper
//

void CyCityOutputHistoryInterface()
{
	OutputDebugString("Python Extension Module - CyCityOutputHistoryInterface\n");

	python::class_<CityOutputHistory>("CityOutputHistory", python::no_init)

		.def("getRecentOutputTurn", &CityOutputHistory::getRecentOutputTurn, "int (int i)")
		.def("getCityOutputHistoryNumEntries", &CityOutputHistory::getCityOutputHistoryNumEntries, "int (int i)")
		.def("getCityOutputHistoryEntry", &CityOutputHistory::getCityOutputHistoryEntry, "int (int i, int iEntry, bool bFirst)")
		.def("getSize", &CityOutputHistory::getSize, "int ()")
	;
}