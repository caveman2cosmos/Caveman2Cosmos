#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvUnitFilters.h
//
//  PURPOSE: Filter classes for units
//
//------------------------------------------------------------------------------------------------

#ifndef CV_UNIT_FILTERS_H
#define CV_UNIT_FILTERS_H

#include "CvEnums.h"

class CvPlayer;

enum UnitFilterTypes
{
	NO_UNIT_FILTER = -1,

	UNIT_FILTER_HIDE_BUILDABLE,
	UNIT_FILTER_HIDE_UNBUILDABLE,
	UNIT_FILTER_HIDE_LIMITED,
	UNIT_FILTER_SHOW_LAND,
	UNIT_FILTER_SHOW_AIR,
	UNIT_FILTER_SHOW_WATER,
	UNIT_FILTER_SHOW_WORKERS,
	UNIT_FILTER_SHOW_CIVILIAN,
	UNIT_FILTER_SHOW_SIEGE,
	UNIT_FILTER_SHOW_MOUNTED,
	UNIT_FILTER_SHOW_HEROES,
	UNIT_FILTER_SHOW_MILITARY,
	UNIT_FILTER_SHOW_DEFENSE,
	UNIT_FILTER_SHOW_MISSIONARY,
	UNIT_FILTER_HIDE_BASIC_INVISIBLE,

	NUM_UNIT_FILTERS
};

class UnitFilterBase
{
public:
	UnitFilterBase(bool bInvert = false): m_bInvert(bInvert), m_bActive(false) {};
	virtual ~UnitFilterBase();
	// Returns true if the unit has the filter property
	bool isFiltered(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);
	bool isActive();
	bool setActive(bool bActive);
	void Activate();
	void Deactivate();

protected:
	virtual bool isFilteredUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit) = 0;
	bool m_bActive;
	bool m_bInvert;
};

class UnitFilterCanBuild : public UnitFilterBase
{
public:
	UnitFilterCanBuild(bool bShowSomeUnconstructable = false, bool bInvert = false) : UnitFilterBase(bInvert), m_bShowSomeUnconstructable(bShowSomeUnconstructable) {};
	bool isFilteredUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);

protected:
	bool m_bShowSomeUnconstructable;
};

class UnitFilterIsCombat : public UnitFilterBase
{
public:
	UnitFilterIsCombat(UnitCombatTypes eCombat, bool bInvert = false) : UnitFilterBase(bInvert), m_eCombat(eCombat) {};
	bool isFilteredUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);

protected:
	UnitCombatTypes m_eCombat;
};

class UnitFilterIsCombats : public UnitFilterBase
{
public:
	UnitFilterIsCombats(bool bInvert = false) : UnitFilterBase(bInvert) {};
	bool isFilteredUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);
	void addCombat(UnitCombatTypes eCombat);
	bool isEmpty();

protected:
	std::vector<UnitCombatTypes> m_eCombats;
};

class UnitFilterIsLimited : public UnitFilterBase
{
public:
	UnitFilterIsLimited(bool bInvert = false) : UnitFilterBase(bInvert) {};
	bool isFilteredUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);
};

class UnitFilterIsDomain : public UnitFilterBase
{
public:
	UnitFilterIsDomain(DomainTypes eDomain, bool bInvert = false) : UnitFilterBase(bInvert), m_eDomain(eDomain) {};
	bool isFilteredUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);

protected:
	DomainTypes m_eDomain;
};

class UnitFilterIsDefense : public UnitFilterBase
{
public:
	UnitFilterIsDefense(bool bInvert = false) : UnitFilterBase(bInvert) {};
	bool isFilteredUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);
};

class UnitFilterList
{
public:
	UnitFilterList(CvPlayer *pPlayer = NULL, CvCity *pCity = NULL);
	~UnitFilterList();
	void init();
	bool isFilterActive(UnitFilterTypes i);
	bool setFilterActive(UnitFilterTypes i, bool bActive);
	int getNumFilters();
	void setPlayer(CvPlayer* pPlayer);
	void setCity(CvCity* pCity);
	bool isFiltered(UnitTypes eUnit);

	static void setFilterActiveAll(UnitFilterTypes eFilter, bool bActive);

protected:
	UnitFilterBase* m_apUnitFilters[NUM_UNIT_FILTERS];
	CvCity* m_pCity;
	CvPlayer* m_pPlayer;
	bool m_bInit;
};

#endif