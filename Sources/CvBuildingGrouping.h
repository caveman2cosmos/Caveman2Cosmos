#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvBuildingGrouping.h
//
//  PURPOSE: Grouping classes for buildings
//
//------------------------------------------------------------------------------------------------

#ifndef CV_BUILDING_GROUPING_H
#define CV_BUILDING_GROUPING_H

#include "CvEnums.h"
#include "CvBuildingFilters.h"
#include <vector>

enum BuildingGroupingTypes
{
	NO_BUILDING_GROUPING = -1,

	BUILDING_GROUPING_SINGLE, // only one group
	BUILDING_GROUPING_WONDER_TYPE, // group by normal building, national wonder, team wonder, world wonder
	BUILDING_GROUPING_DOMAIN, // group by military, city defense, other

	NUM_BUILDING_GROUPING
};

class BuildingGroupingBase
{
  public:
	BuildingGroupingBase(bool bInvert = false)
		: m_bInvert(bInvert){};
	virtual ~BuildingGroupingBase();
	// Returns the number of the group of the building
	int getGroup(CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding);

  protected:
	virtual int getGroupBuilding(CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding) = 0;
	bool		m_bInvert;
};

class BuildingGroupingSingle : public BuildingGroupingBase
{
  public:
	BuildingGroupingSingle(bool bInvert = false)
		: BuildingGroupingBase(bInvert){};
	int getGroupBuilding(CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding);
};

class BuildingGroupingWonderType : public BuildingGroupingBase
{
  public:
	BuildingGroupingWonderType(bool bInvert = false)
		: BuildingGroupingBase(bInvert){};
	int getGroupBuilding(CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding);
};

class BuildingGroupingFilters : public BuildingGroupingBase
{
  public:
	BuildingGroupingFilters(bool bInvert = false)
		: BuildingGroupingBase(bInvert){};
	~BuildingGroupingFilters();
	void addGroupingFilter(BuildingFilterBase* pFilter);
	int	 getGroupBuilding(CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding);

  protected:
	std::vector<BuildingFilterBase*> m_apFilters;
};

class BuildingGroupingList
{
  public:
	BuildingGroupingList(CvPlayer* pPlayer = NULL, CvCity* pCity = NULL);
	~BuildingGroupingList();
	BuildingGroupingTypes getActiveGrouping();
	bool				  setActiveGrouping(BuildingGroupingTypes eActiveGrouping);
	int					  getNumGrouping();
	void				  setPlayer(CvPlayer* pPlayer);
	void				  setCity(CvCity* pCity);
	int					  getGroup(BuildingTypes eBuilding);

  protected:
	BuildingGroupingBase* m_apBuildingGrouping[NUM_BUILDING_GROUPING];
	CvCity*				  m_pCity;
	CvPlayer*			  m_pPlayer;
	BuildingGroupingTypes m_eActiveGrouping;
};

#endif