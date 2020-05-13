#pragma once

#ifndef SCV_INTERNAL_GLOBALS_H
#define SCV_INTERNAL_GLOBALS_H

#include "CvInfos.h"

class SCvInternalGlobals
{
public:
	static CvDLLUtilityIFaceBase* engineUtils();

	static CvBonusInfo* getBonusInfoByType(const std::string& szType);
	static         int  getBonusInfoIndexByType(const std::string& szType);
};

#endif
