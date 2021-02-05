#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvUnitGrouping.h
//
//  PURPOSE: Grouping classes for units
//
//------------------------------------------------------------------------------------------------

#ifndef CV_UNIT_GROUPING_H
#define CV_UNIT_GROUPING_H

//#include "CvUnitFilters.h"

class CvCity;
class CvPlayer;

enum UnitGroupingTypes
{
	NO_UNIT_GROUPING = -1,

	UNIT_GROUPING_SINGLE, // only one group
	UNIT_GROUPING_COMBAT, // group by type
	UNIT_GROUPING_DOMAIN, // group by land, air, water
	UNIT_GROUPING_HERO, // group by hero, limited, normal

	NUM_UNIT_GROUPING
};

class UnitGroupingBase
{
public:
	UnitGroupingBase(bool bInvert = false): m_bInvert(bInvert) {};
	virtual ~UnitGroupingBase();
	// Returns the number of the group of the unit
	int getGroup(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const;

protected:
	virtual int getGroupUnit(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const = 0;
	const bool m_bInvert;
};

class UnitGroupingSingle : public UnitGroupingBase
{
public:
	UnitGroupingSingle(bool bInvert = false) : UnitGroupingBase(bInvert) {};
	int getGroupUnit(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const;
};

class UnitGroupingCombat : public UnitGroupingBase
{
public:
	UnitGroupingCombat(bool bInvert = false) : UnitGroupingBase(bInvert) {};
	int getGroupUnit(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const;
};

class UnitGroupingDomain : public UnitGroupingBase
{
public:
	UnitGroupingDomain(bool bInvert = false) : UnitGroupingBase(bInvert) {};
	int getGroupUnit(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const;
};

class UnitGroupingFilters : public UnitGroupingBase
{
public:
	UnitGroupingFilters(bool bInvert = false) : UnitGroupingBase(bInvert) {};
	~UnitGroupingFilters();
	void addGroupingFilter(UnitFilterBase* pFilter);
	int getGroupUnit(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const;
protected:
	std::vector<UnitFilterBase*> m_apFilters;
};

class UnitGroupingList
{
public:
	UnitGroupingList(const CvPlayer *pPlayer = NULL, const CvCity *pCity = NULL);
	~UnitGroupingList();
	void init();
	UnitGroupingTypes getActiveGrouping() const;
	bool setActiveGrouping(UnitGroupingTypes eActiveGrouping);
	void setPlayer(const CvPlayer* pPlayer);
	void setCity(const CvCity* pCity);
	int getGroup(UnitTypes eUnit) const;

protected:
	UnitGroupingBase* m_apUnitGrouping[NUM_UNIT_GROUPING];
	const CvCity* m_pCity;
	const CvPlayer* m_pPlayer;
	UnitGroupingTypes m_eActiveGrouping;
	bool m_bInit;
};

#endif