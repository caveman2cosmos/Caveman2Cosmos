#pragma once

#ifndef CV_EMPHASIZE_INFO_H
#define CV_EMPHASIZE_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEmphasizeInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEmphasizeInfo
	: public CvInfoBase
	, private bst::noncopyable
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvEmphasizeInfo();
	virtual ~CvEmphasizeInfo();

	bool isAvoidGrowth() const;
	bool isGreatPeople() const;

	// Afforess 02/10/10
	bool isAvoidAngryCitizens() const;
	bool isAvoidUnhealthyCitizens() const;
	// ! Afforess

	// Arrays
	int getYieldChange(int i) const;
	int getCommerceChange(int i) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvEmphasizeInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:
	bool m_bAvoidGrowth;
	bool m_bGreatPeople;

	// Afforess 02/10/10
	bool m_bAvoidAngryCitizens;
	bool m_bAvoidUnhealthyCitizens;
	// ! Afforess

	// Arrays
	int* m_piYieldModifiers;
	int* m_piCommerceModifiers;

};

#endif // CV_EMPHASIZE_INFO_H
