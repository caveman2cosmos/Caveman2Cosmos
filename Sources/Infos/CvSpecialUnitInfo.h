#pragma once

#ifndef CV_SPECIAL_UNIT_INFO_H
#define CV_SPECIAL_UNIT_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSpecialUnitInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpecialUnitInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvSpecialUnitInfo();
	virtual ~CvSpecialUnitInfo();

	bool isValid() const;
	bool isCityLoad() const;
	bool isSMLoadSame() const;

	// Arrays

	bool isCarrierUnitAIType(int i) const;

	int getCombatPercent() const;
	int getWithdrawalChange() const;
	int getPursuitChange() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvSpecialUnitInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	bool m_bValid;
	bool m_bCityLoad;
	bool m_bSMLoadSame;

	int m_iCombatPercent;
	int m_iWithdrawalChange;
	int m_iPursuitChange;

	// Arrays
	bool* m_pbCarrierUnitAITypes;
};

#endif // CV_SPECIAL_UNIT_INFO_H
