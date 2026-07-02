#pragma once

#ifndef CV_MAP_CATEGORY_INFO_H
#define CV_MAP_CATEGORY_INFO_H

#include "CvInfoBase.h"

//TB Promotion Line Mod end

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvMapCategoryInfo
//
//  DESC:   Contains info about Map Category Types
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMapCategoryInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:
	CvMapCategoryInfo();
	virtual ~CvMapCategoryInfo();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvMapCategoryInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;
};

#endif // CV_MAP_CATEGORY_INFO_H
