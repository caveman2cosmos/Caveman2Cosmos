#pragma once

class CvBonusInfo;

class SCvInternalGlobals
{
public:
	static CvDLLUtilityIFaceBase* engineUtils();

	static CvBonusInfo* getBonusInfoByType(const std::string& szType);
	static         int  getBonusInfoIndexByType(const std::string& szType);
};
