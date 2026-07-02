#pragma once

#ifndef CV_UPKEEP_INFO_H
#define CV_UPKEEP_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvUpkeepInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUpkeepInfo
	: public CvInfoBase
	, private bst::noncopyable
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvUpkeepInfo();
	virtual ~CvUpkeepInfo();

	int getPopulationPercent() const;
	int getCityPercent() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvUpkeepInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iPopulationPercent;
	int m_iCityPercent;
};

#endif // CV_UPKEEP_INFO_H
