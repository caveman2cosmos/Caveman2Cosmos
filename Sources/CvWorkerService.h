#pragma once


#ifndef CV_WORKER_SERVICE
#define CV_WORKER_SERVICE
#include "CvCity.h"

class CvWorkerService {
public:
	static bool ShouldImproveCity(CvCity* targetCity);
};



#endif