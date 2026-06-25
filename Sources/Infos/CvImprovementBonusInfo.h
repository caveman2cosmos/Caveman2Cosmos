#pragma once

#ifndef CV_IMPROVEMENT_BONUS_INFO_H
#define CV_IMPROVEMENT_BONUS_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvImprovementBonusInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvImprovementBonusInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	friend class CvImprovementInfo;
	friend class CvXMLLoadUtility;

	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvImprovementBonusInfo();
	virtual ~CvImprovementBonusInfo();

	int getDiscoverRand() const;
	int getDepletionRand() const;

	bool isBonusMakesValid() const;
	bool isObsoleteBonusMakesValid() const;
	bool isBonusTrade() const;

	int getYieldChange(int i) const;

	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iDiscoverRand;
	int m_iDepletionRand;

	bool m_bBonusMakesValid;
	bool m_bObsoleteBonusMakesValid;
	bool m_bBonusTrade;

	// Arrays
	int* m_piYieldChange;
};

#endif // CV_IMPROVEMENT_BONUS_INFO_H
