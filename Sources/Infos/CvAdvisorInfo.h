#pragma once

#ifndef CV_ADVISOR_INFO_H
#define CV_ADVISOR_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvAdvisorInfo (ADD to Python)
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAdvisorInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvAdvisorInfo();
	virtual ~CvAdvisorInfo();

	const char* getTexture() const;
	int getNumCodes() const;
	int getEnableCode(uint32_t uiCode) const;
	int getDisableCode(uint32_t uiCode) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvAdvisorInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:
	CvString m_szTexture;
	std::vector< std::pair< int, int > > m_vctEnableDisableCodes;
};

#endif // CV_ADVISOR_INFO_H
