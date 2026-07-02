#pragma once

#ifndef CV_ROUTE_INFO_H
#define CV_ROUTE_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvRouteInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvRouteInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvRouteInfo();
	virtual ~CvRouteInfo();

	int getAdvancedStartCost() const;

	int getValue() const;
	int getMovementCost() const;
	int getFlatMovementCost() const;
	int getPrereqBonus() const;

	bool isSeaTunnel() const;

	// Arrays
	int getYieldChange(int i) const;
	int* getYieldChangeArray() const;
	int getTechMovementChange(int i) const;

	int getCategory(int i) const;
	int getNumCategories() const;
	bool isCategory(int i) const;

	const std::vector<BonusTypes>& getPrereqOrBonuses() const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }
	//	This really belongs on CvInfoBase but you can't change the size of that
	//	object without crashing the core engine :-(
	inline int getZobristValue() const { return m_zobristValue; }

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvRouteInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

private:
	CvPropertyManipulators m_PropertyManipulators;

	int m_iAdvancedStartCost;
	int m_iValue;
	int m_iMovementCost;
	int m_iFlatMovementCost;
	int m_iPrereqBonus;
	int m_zobristValue;

	bool m_bSeaTunnel;

	// Arrays
	int* m_piYieldChange;
	int* m_piTechMovementChange;
	std::vector<int> m_aiCategories;
	std::vector<BonusTypes> m_piPrereqOrBonuses;
};

#endif // CV_ROUTE_INFO_H
