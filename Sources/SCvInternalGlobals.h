#pragma once

#include "../CvInfos.h"


class SCvInternalGlobals
{
public:
	static CvDLLUtilityIFaceBase* engineUtils();

	static CvBonusInfo* getBonusInfoByType(const std::string& szType);
	static         int  getBonusInfoIndexByType(const std::string& szType);
};