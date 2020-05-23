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

//#include "CvEnums.h"
//#include "CvUnitFilters.h"

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
	int getGroup(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);

protected:
	virtual int getGroupUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit) = 0;
	bool m_bInvert;
};

class UnitGroupingSingle : public UnitGroupingBase
{
public:
	UnitGroupingSingle(bool bInvert = false) : UnitGroupingBase(bInvert) {};
	int getGroupUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);
};

class UnitGroupingCombat : public UnitGroupingBase
{
public:
	UnitGroupingCombat(bool bInvert = false) : UnitGroupingBase(bInvert) {};
	int getGroupUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);
};

class UnitGroupingDomain : public UnitGroupingBase
{
public:
	UnitGroupingDomain(bool bInvert = false) : UnitGroupingBase(bInvert) {};
	int getGroupUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);
};

class UnitGroupingFilters : public UnitGroupingBase
{
public:
	UnitGroupingFilters(bool bInvert = false) : UnitGroupingBase(bInvert) {};
	~UnitGroupingFilters();
	void addGroupingFilter(UnitFilterBase* pFilter);
	int getGroupUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);
protected:
	std::vector<UnitFilterBase*> m_apFilters;
};

class UnitGroupingList
{
public:
	UnitGroupingList(CvPlayer *pPlayer = NULL, CvCity *pCity = NULL);
	~UnitGroupingList();
	void init();
	UnitGroupingTypes getActiveGrouping();
	bool setActiveGrouping(UnitGroupingTypes eActiveGrouping);
	int getNumGrouping();
	void setPlayer(CvPlayer* pPlayer);
	void setCity(CvCity* pCity);
	int getGroup(UnitTypes eUnit);

protected:
	UnitGroupingBase* m_apUnitGrouping[NUM_UNIT_GROUPING];
	CvCity* m_pCity;
	CvPlayer* m_pPlayer;
	UnitGroupingTypes m_eActiveGrouping;
	bool m_bInit;
};

#endif