#pragma once

#ifndef CV_COMMERCE_INFO_H
#define CV_COMMERCE_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCommerceInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCommerceInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvCommerceInfo();
	virtual ~CvCommerceInfo();

	int getChar() const;
	void setChar(int i);
	int getInitialPercent() const;
	int getInitialHappiness() const;
	int getAIWeightPercent() const;

	bool isFlexiblePercent() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvCommerceInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iChar;
	int m_iInitialPercent;
	int m_iInitialHappiness;
	int m_iAIWeightPercent;

	bool m_bFlexiblePercent;
};

#endif // CV_COMMERCE_INFO_H
