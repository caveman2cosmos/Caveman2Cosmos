#pragma once

#ifndef CV_IDEA_CLASS_INFO_H
#define CV_IDEA_CLASS_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvIdeaClassInfo
//
//  DESC:   Contains info about Idea Class Types
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvIdeaClassInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:

	CvIdeaClassInfo();
	virtual ~CvIdeaClassInfo();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvIdeaClassInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	bool isInitialized();

protected:
	bool m_bInitialized;
};

#endif // CV_IDEA_CLASS_INFO_H
