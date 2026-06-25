#pragma once

#ifndef CV_IDEA_INFO_H
#define CV_IDEA_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvIdeaInfo
//
//  DESC:   Contains info about Idea Types
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvIdeaInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:

	CvIdeaInfo();
	virtual ~CvIdeaInfo();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvIdeaInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	IdeaClassTypes getIdeaClass() const;

protected:
	IdeaClassTypes m_eIdeaClass;
};

#endif // CV_IDEA_INFO_H
