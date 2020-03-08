#include "CvGameCoreDLL.h"
#include <algorithm>

CvDLLUtilityIFaceBase* SCvInternalGlobals::engineUtils()
{
	return cvInternalGlobals::getInstance().getDLLIFace();
}

CvBonusInfo* SCvInternalGlobals::getBonusInfoByType(const std::string& szType)
{
	for (int i = 0; i < GC.getNumBonusInfos(); ++i)
		CvBonusInfo& bonusInfo = GC.getBonusInfo(static_cast<BonusTypes>(i));
		if (bonusInfo.getType() == szType)
			return &bonusInfo;
	return NULL;
}

int SCvInternalGlobals::getBonusInfoIndexByType(const std::string& szType)
{
	for (int i = 0; i < GC.getNumBonusInfos(); ++i)
		if (GC.getBonusInfo(static_cast<BonusTypes>(i)).getType() == szType)
			return i;
	return -1;
}
