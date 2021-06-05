#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvBuildingFilters.h
//
//  PURPOSE: Filter classes for buildings
//
//------------------------------------------------------------------------------------------------

#ifndef CV_BUILDING_FILTERS_H
#define CV_BUILDING_FILTERS_H

class CvCity;
class CvPlayer;

enum BuildingFilterTypes
{
	NO_BUILDING_FILTER = -1,

	BUILDING_FILTER_HIDE_BUILDABLE,
	BUILDING_FILTER_HIDE_UNBUILDABLE,
	BUILDING_FILTER_HIDE_GREAT_WONDER,
	BUILDING_FILTER_HIDE_NATIONAL_WONDER,
	BUILDING_FILTER_HIDE_NORMAL,
	BUILDING_FILTER_SHOW_SCIENCE,
	BUILDING_FILTER_SHOW_FOOD,
	BUILDING_FILTER_SHOW_CULTURE,
	BUILDING_FILTER_SHOW_ESPIONAGE,
	BUILDING_FILTER_SHOW_GOLD,
	BUILDING_FILTER_SHOW_PRODUCTION,
	BUILDING_FILTER_SHOW_HAPPINESS,
	BUILDING_FILTER_SHOW_HEALTH,
	BUILDING_FILTER_SHOW_MILITARY,
	BUILDING_FILTER_SHOW_CITY_DEFENSE,
	BUILDING_FILTER_HIDE_UNHAPPINESS,
	BUILDING_FILTER_HIDE_UNHEALTHINESS,
	BUILDING_FILTER_SHOW_CRIME,
	BUILDING_FILTER_SHOW_FLAMMABILITY,
	BUILDING_FILTER_SHOW_EDUCATION,
	BUILDING_FILTER_SHOW_DISEASE,
	BUILDING_FILTER_SHOW_AIR_POLLUTION,
	BUILDING_FILTER_SHOW_WATER_POLLUTION,
	BUILDING_FILTER_SHOW_TOURISM,
	BUILDING_FILTER_HIDE_BASIC_INVISIBLE,

	NUM_BUILDING_FILTERS
};

class BuildingFilterBase
{
public:
	BuildingFilterBase(bool bInvert = false): m_bInvert(bInvert), m_bActive(false) {};
	virtual ~BuildingFilterBase();
	// Returns true if the building has the filter property
	bool isFiltered(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const;
	bool isActive() const;
	bool setActive(bool bActive);
	void Activate();
	void Deactivate();

protected:
	virtual bool isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const = 0;
	bool m_bActive;
	const bool m_bInvert;
};

class BuildingFilterCanBuild : public BuildingFilterBase
{
public:
	BuildingFilterCanBuild(bool bShowSomeUnconstructable = false, bool bInvert = false) : BuildingFilterBase(bInvert), m_bShowSomeUnconstructable(bShowSomeUnconstructable) {};
	bool isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const;

protected:
	const bool m_bShowSomeUnconstructable;
};

class BuildingFilterIsWonder : public BuildingFilterBase
{
public:
	BuildingFilterIsWonder(bool bInvert = false) : BuildingFilterBase(bInvert) {};
	bool isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const;
};

class BuildingFilterIsGreatWonder : public BuildingFilterBase
{
public:
	BuildingFilterIsGreatWonder(bool bInvert = false) : BuildingFilterBase(bInvert) {};
	bool isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const;
};

class BuildingFilterIsNationalWonder : public BuildingFilterBase
{
public:
	BuildingFilterIsNationalWonder(bool bInvert = false) : BuildingFilterBase(bInvert) {};
	bool isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const;
};

class BuildingFilterIsCommerce : public BuildingFilterBase
{
public:
	explicit BuildingFilterIsCommerce(CommerceTypes eCommerce, bool bInvert = false);
	bool isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const;

protected:
	const CommerceTypes m_eCommerce;
};

class BuildingFilterIsYield : public BuildingFilterBase
{
public:
	explicit BuildingFilterIsYield(YieldTypes eYield, bool bInvert = false);
	bool isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const;

protected:
	const YieldTypes m_eYield;
};

class BuildingFilterIsHappiness : public BuildingFilterBase
{
public:
	BuildingFilterIsHappiness(bool bInvert = false) : BuildingFilterBase(bInvert) {};
	bool isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const;
};

class BuildingFilterIsHealth : public BuildingFilterBase
{
public:
	BuildingFilterIsHealth(bool bInvert = false) : BuildingFilterBase(bInvert) {};
	bool isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const;
};

class BuildingFilterIsUnhappiness : public BuildingFilterBase
{
public:
	BuildingFilterIsUnhappiness(bool bInvert = false) : BuildingFilterBase(bInvert) {};
	bool isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const;
};

class BuildingFilterIsUnhealthiness : public BuildingFilterBase
{
public:
	BuildingFilterIsUnhealthiness(bool bInvert = false) : BuildingFilterBase(bInvert) {};
	bool isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const;
};

class BuildingFilterIsMilitary : public BuildingFilterBase
{
public:
	BuildingFilterIsMilitary(bool bInvert = false) : BuildingFilterBase(bInvert) {};
	bool isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const;
};

class BuildingFilterIsCityDefense : public BuildingFilterBase
{
public:
	BuildingFilterIsCityDefense(bool bInvert = false) : BuildingFilterBase(bInvert) {};
	bool isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const;
};

class BuildingFilterIsProperty : public BuildingFilterBase
{
public:
	explicit BuildingFilterIsProperty(PropertyTypes eProperty, bool bInvert = false) : BuildingFilterBase(bInvert), m_eProperty(eProperty) {};
	bool isFilteredBuilding(const CvPlayer *pPlayer, CvCity *pCity, BuildingTypes eBuilding) const;

protected:
	const PropertyTypes m_eProperty;
};

class BuildingFilterList
{
public:
	BuildingFilterList(CvPlayer *pPlayer = NULL, CvCity *pCity = NULL);
	~BuildingFilterList();
	void init();
	bool isFilterActive(BuildingFilterTypes i) const;
	bool setFilterActive(BuildingFilterTypes i, bool bActive);
	void setPlayer(CvPlayer* pPlayer);
	void setCity(CvCity* pCity);
	bool isFiltered(BuildingTypes eBuilding) const;

	static void setFilterActiveAll(BuildingFilterTypes eFilter, bool bActive);

protected:
	BuildingFilterBase* m_apBuildingFilters[NUM_BUILDING_FILTERS];
	CvCity* m_pCity;
	const CvPlayer* m_pPlayer;
};

#endif