#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvBuildingSort.h
//
//  PURPOSE: Sorting classes for buildings
//
//------------------------------------------------------------------------------------------------

#ifndef CV_BUILDING_SORT_H
#define CV_BUILDING_SORT_H

class CvCity;
class CvPlayer;

enum BuildingSortTypes
{
	NO_BUILDING_SORT = -1,

	BUILDING_SORT_NAME,
	BUILDING_SORT_COST,
	BUILDING_SORT_SCIENCE,
	BUILDING_SORT_CULTURE,
	BUILDING_SORT_ESPIONAGE,
	BUILDING_SORT_GOLD,
	BUILDING_SORT_FOOD,
	BUILDING_SORT_PRODUCTION,
	BUILDING_SORT_HAPPINESS,
	BUILDING_SORT_HEALTH,
	BUILDING_SORT_CRIME,
	BUILDING_SORT_FLAMMABILITY,

	NUM_BUILDING_SORT
};

class BuildingSortBase
{
public:
	BuildingSortBase(bool bInvert = false): m_bInvert(bInvert) {};
	virtual ~BuildingSortBase();
	// Returns the value of eBuilding in the sorting category
	virtual int getBuildingValue(const CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding) const = 0;
	// Returns if eBuilding1 is before eBuilding2, default for the value based sorts is greater first
	virtual bool isLesserBuilding(const CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding1, BuildingTypes eBuilding2);
	bool isInverse() const;
	bool setInverse(bool bInvert);
	void deleteCache();
protected:
	bool m_bInvert;
	stdext::hash_map<BuildingTypes, int> m_mapValueCache;
};

class BuildingSortCommerce : public BuildingSortBase
{
public:
	explicit BuildingSortCommerce(CommerceTypes eCommerce, bool bInvert = false);
	int getBuildingValue(const CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding) const;

protected:
	CommerceTypes m_eCommerce;
};

class BuildingSortYield : public BuildingSortBase
{
public:
	explicit BuildingSortYield(YieldTypes eYield, bool bInvert = false);
	int getBuildingValue(const CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding) const;

protected:
	YieldTypes m_eYield;
};

class BuildingSortHappiness : public BuildingSortBase
{
public:
	BuildingSortHappiness(bool bInvert = false) : BuildingSortBase(bInvert) {};
	int getBuildingValue(const CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding) const;
};

class BuildingSortHealth : public BuildingSortBase
{
public:
	BuildingSortHealth(bool bInvert = false) : BuildingSortBase(bInvert) {};
	int getBuildingValue(const CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding) const;
};

class BuildingSortCost : public BuildingSortBase
{
public:
	BuildingSortCost(bool bInvert = false) : BuildingSortBase(bInvert) {};
	int getBuildingValue(const CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding) const;
};

class BuildingSortName : public BuildingSortBase
{
public:
	BuildingSortName(bool bInvert = false) : BuildingSortBase(bInvert) {};
	bool isLesserBuilding(const CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding1, BuildingTypes eBuilding2) const;
	int getBuildingValue(const CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding) const;
};

class BuildingSortProperty : public BuildingSortBase
{
public:
	explicit BuildingSortProperty(PropertyTypes eProperty, bool bInvert = false) : BuildingSortBase(bInvert), m_eProperty(eProperty) {};
	int getBuildingValue(const CvPlayer* pPlayer, CvCity* pCity, BuildingTypes eBuilding) const;

protected:
	PropertyTypes m_eProperty;
};

class BuildingSortList
{
public:
	BuildingSortList(CvPlayer *pPlayer = NULL, CvCity *pCity = NULL);
	~BuildingSortList();
	void init();
	BuildingSortTypes getActiveSort() const;
	bool setActiveSort(BuildingSortTypes eActiveSort);
	void setPlayer(CvPlayer* pPlayer);
	void setCity(CvCity* pCity);
	bool operator()(BuildingTypes eBuilding1, BuildingTypes eBuilding2);
	void deleteCache();

protected:
	BuildingSortBase* m_apBuildingSort[NUM_BUILDING_SORT];
	CvCity* m_pCity;
	CvPlayer* m_pPlayer;
	BuildingSortTypes m_eActiveSort;
};

class BuildingSortListWrapper
{
public:
	explicit BuildingSortListWrapper(BuildingSortList* pList) : m_pList(pList) {}
	bool operator()(BuildingTypes eBuilding1, BuildingTypes eBuilding2) {return m_pList->operator()(eBuilding1, eBuilding2);}
	void deleteCache() {m_pList->deleteCache();}
protected:
	BuildingSortList* m_pList;
};

#endif