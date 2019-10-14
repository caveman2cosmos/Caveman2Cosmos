#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvUnitList.h
//
//  PURPOSE: Filter, group, sort and cache a unit list for a city or player
//
//------------------------------------------------------------------------------------------------

#ifndef CV_UNIT_LIST_H
#define CV_UNIT_LIST_H
#include "CvUnitFilters.h"
#include "CvUnitGrouping.h"
#include "CvUnitSort.h"

void CyEnumsUnitListPythonInterface();

class CvUnitList
{
  public:
	CvUnitList(CvPlayer* pPlayer = NULL, CvCity* pCity = NULL);
	void			  init();
	void			  setPlayerToOwner();
	void			  setInvalid();
	bool			  getFilterActive(UnitFilterTypes eFilter);
	void			  setFilterActive(UnitFilterTypes eFilter, bool bActive);
	UnitGroupingTypes getGroupingActive();
	void			  setGroupingActive(UnitGroupingTypes eGrouping);
	UnitSortTypes	  getSortingActive();
	void			  setSortingActive(UnitSortTypes eSorting);
	int				  getGroupNum();
	int				  getNumInGroup(int iGroup);
	UnitTypes		  getUnitType(int iGroup, int iPos);
	int				  getSelectionRow();
	void			  setSelectedUnit(UnitTypes eSelectedUnit);
	UnitTypes		  getSelectedUnit();

  protected:
	void								 doFilter();
	void								 doGroup();
	void								 doSort();
	UnitFilterList						 m_UnitFilters;
	UnitGroupingList					 m_UnitGrouping;
	UnitSortList						 m_UnitSort;
	std::vector<UnitTypes>				 m_aiUnitList;
	std::vector<std::vector<UnitTypes>*> m_aaiGroupedUnitList;
	bool								 m_bFilteringValid;
	bool								 m_bGroupingValid;
	bool								 m_bSortingValid;
	CvCity*								 m_pCity;
	CvPlayer*							 m_pPlayer;
	UnitTypes							 m_eSelectedUnit;
};


#endif
