#pragma once

#ifndef CV_CULTURE_LEVEL_INFO_H
#define CV_CULTURE_LEVEL_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCultureLevelInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCultureLevelInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvCultureLevelInfo();
	virtual ~CvCultureLevelInfo();

	int getLevel() const { return m_iLevel; }
	void setLevel(int iLevel) { m_iLevel = iLevel; }

	int getCityDefenseModifier() const;

	int getCityRadius() const;
	int getMaxWorldWonders() const;
	int getMaxTeamWonders() const;
	int getMaxNationalWonders() const;
	int getMaxNationalWondersOCC() const;
	int getPrereqGameOption() const;

	int getSpeedThreshold(int i) const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvCultureLevelInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:
	int m_iLevel;
	int m_iCityDefenseModifier;
	int m_iCityRadius;
	int m_iMaxWorldWonders;
	int m_iMaxTeamWonders;
	int m_iMaxNationalWonders;
	int m_iMaxNationalWondersOCC;
	int m_iPrereqGameOption;

	int* m_paiSpeedThreshold;
};

#endif // CV_CULTURE_LEVEL_INFO_H
