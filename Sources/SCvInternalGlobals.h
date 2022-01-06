#pragma once

class CvBonusInfo;

class SCvInternalGlobals
{
public:
	static CvBonusInfo* getBonusInfoByType(const std::string& szType);
	static         int  getBonusInfoIndexByType(const std::string& szType);
};
