#pragma once

#ifndef CV_BONUS_CLASS_INFO_H
#define CV_BONUS_CLASS_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvBonusClassInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBonusClassInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvBonusClassInfo();
	virtual ~CvBonusClassInfo();

	int getUniqueRange() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvBonusClassInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:
	int m_iUniqueRange;
};

#endif // CV_BONUS_CLASS_INFO_H
