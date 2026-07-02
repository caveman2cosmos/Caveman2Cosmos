#pragma once

#ifndef CV_SEA_LEVEL_INFO_H
#define CV_SEA_LEVEL_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvSeaLevelInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSeaLevelInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:

	CvSeaLevelInfo();
	virtual ~CvSeaLevelInfo();

	int getSeaLevelChange() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvSeaLevelInfo* pClassInfo);

protected:

	int m_iSeaLevelChange;
};

#endif // CV_SEA_LEVEL_INFO_H
