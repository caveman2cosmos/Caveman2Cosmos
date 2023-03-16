
#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvBonusInfo.h"
#include "CvGlobals.h"
#include "CvInfos.h"

CvBonusInfo* SCvInternalGlobals::getBonusInfoByType(const std::string& szType)
{
	PROFILE_EXTRA_FUNC();
	foreach_(CvBonusInfo* info, GC.getBonusInfos())
		if (info->getType() == szType)
			return info;
	return NULL;
}

int SCvInternalGlobals::getBonusInfoIndexByType(const std::string& szType)
{
	PROFILE_EXTRA_FUNC();
	const std::vector<CvBonusInfo*>& bonuses = cvInternalGlobals::getInstance().getBonusInfos();
	for (int i = 0; i != bonuses.size(); ++i)
		if (bonuses[i]->getType() == szType)
			return i;
	return -1;
}
