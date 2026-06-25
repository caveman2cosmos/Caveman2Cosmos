#pragma once

#ifndef CV_CATEGORY_INFO_H
#define CV_CATEGORY_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCategoryInfo
//
//  DESC:   Contains info about Category Types
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCategoryInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:

	CvCategoryInfo();
	virtual ~CvCategoryInfo();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvCategoryInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	bool isInitialized();

	int getCategory(int i) const;
	int getNumCategories() const;
	bool isCategory(int i) const;

protected:
	bool m_bInitialized;
	std::vector<int> m_aiCategories;
};

#endif // CV_CATEGORY_INFO_H
