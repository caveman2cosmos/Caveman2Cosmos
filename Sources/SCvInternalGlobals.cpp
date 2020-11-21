#include "CvGameCoreDLL.h"
#include "CvGlobals.h"

CvBonusInfo* SCvInternalGlobals::getBonusInfoByType(const std::string& szType)
{
	const std::vector<CvBonusInfo*>& bonuses = GC.getBonusInfos();
	for (std::vector<CvBonusInfo*>::const_iterator i = bonuses.begin(); i != bonuses.end(); ++i)
		if ((*i)->getType() == szType) 
			return *i;
	return NULL;
}

int SCvInternalGlobals::getBonusInfoIndexByType(const std::string& szType)
{
	const std::vector<CvBonusInfo*>& bonuses = cvInternalGlobals::getInstance().getBonusInfos();
	for (int i = 0; i != bonuses.size(); ++i)
		if (bonuses[i]->getType() == szType)
			return i;
	return -1;
}
