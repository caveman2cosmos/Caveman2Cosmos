#pragma once

#ifndef CV_HURRY_INFO_H
#define CV_HURRY_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvHurryInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvHurryInfo
	: public CvInfoBase
	, private bst::noncopyable
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvHurryInfo();
	virtual ~CvHurryInfo();

	int getGoldPerProduction() const;
	int getProductionPerPopulation() const;

	bool isAnger() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvHurryInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

//---------------------------------------PUBLIC MEMBER VARIABLES---------------------------------
protected:

	int m_iGoldPerProduction;
	int m_iProductionPerPopulation;

	bool m_bAnger;
};

#endif // CV_HURRY_INFO_H
