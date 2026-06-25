#pragma once

#ifndef CV_SPECIAL_BUILDING_INFO_H
#define CV_SPECIAL_BUILDING_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSpecialBuildingInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpecialBuildingInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvSpecialBuildingInfo();
	virtual ~CvSpecialBuildingInfo();

	TechTypes getObsoleteTech() const;
	TechTypes getTechPrereq() const { return m_iTechPrereq; }
	int getTechPrereqAnyone() const;
	int getMaxPlayerInstances() const;

	bool isValid() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvSpecialBuildingInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	TechTypes m_iObsoleteTech;
	TechTypes m_iTechPrereq;
	int m_iTechPrereqAnyone;
	int m_iMaxPlayerInstances;

	bool m_bValid;
};

#endif // CV_SPECIAL_BUILDING_INFO_H
