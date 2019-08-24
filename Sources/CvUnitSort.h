#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvUnitSort.h
//
//  PURPOSE: Sorting classes for units
//
//------------------------------------------------------------------------------------------------

#ifndef CV_UNIT_SORT_H
#define CV_UNIT_SORT_H

#include "CvEnums.h"

class CvPlayer;

enum UnitSortTypes
{
	NO_UNIT_SORT = -1,

	UNIT_SORT_NAME,
	UNIT_SORT_COST,
	UNIT_SORT_STRENGTH,
	UNIT_SORT_MOVE,
	UNIT_SORT_COLLATERAL,
	UNIT_SORT_RANGE,
	UNIT_SORT_BOMBARD,
	UNIT_SORT_CARGO,
	UNIT_SORT_WITHDRAWAL,
	UNIT_SORT_POWER,

	NUM_UNIT_SORT
};

class UnitSortBase
{
public:
	UnitSortBase(bool bInvert = false): m_bInvert(bInvert) {};
	virtual ~UnitSortBase();
	// Returns the value of eUnit in the sorting category
	virtual int getUnitValue(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit) = 0;
	// Returns if eUnit1 is before eUnit2, default for the value based sorts is greater first
	virtual bool isLesserUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit1, UnitTypes eUnit2);
	bool isInverse();
	bool setInverse(bool bInvert);
protected:
	bool m_bInvert;
};

class UnitSortStrength : public UnitSortBase
{
public:
	UnitSortStrength(bool bInvert = false) : UnitSortBase(bInvert) {};
	int getUnitValue(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);
};

class UnitSortMove : public UnitSortBase
{
public:
	UnitSortMove(bool bInvert = false) : UnitSortBase(bInvert) {};
	int getUnitValue(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);
};

class UnitSortCollateral : public UnitSortBase
{
public:
	UnitSortCollateral(bool bInvert = false) : UnitSortBase(bInvert) {};
	int getUnitValue(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);
};

class UnitSortRange : public UnitSortBase
{
public:
	UnitSortRange(bool bInvert = false) : UnitSortBase(bInvert) {};
	int getUnitValue(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);
};

class UnitSortBombard : public UnitSortBase
{
public:
	UnitSortBombard(bool bInvert = false) : UnitSortBase(bInvert) {};
	int getUnitValue(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);
};

class UnitSortCargo : public UnitSortBase
{
public:
	UnitSortCargo(bool bInvert = false) : UnitSortBase(bInvert) {};
	int getUnitValue(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);
};

class UnitSortWithdrawal : public UnitSortBase
{
public:
	UnitSortWithdrawal(bool bInvert = false) : UnitSortBase(bInvert) {};
	int getUnitValue(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);
};

class UnitSortPower : public UnitSortBase
{
public:
	UnitSortPower(bool bInvert = false) : UnitSortBase(bInvert) {};
	int getUnitValue(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);
};

class UnitSortCost : public UnitSortBase
{
public:
	UnitSortCost(bool bInvert = false) : UnitSortBase(bInvert) {};
	int getUnitValue(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);
};

class UnitSortName : public UnitSortBase
{
public:
	UnitSortName(bool bInvert = false) : UnitSortBase(bInvert) {};
	bool isLesserUnit(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit1, UnitTypes eUnit2);
	int getUnitValue(CvPlayer *pPlayer, CvCity *pCity, UnitTypes eUnit);
};

class UnitSortList
{
public:
	UnitSortList(CvPlayer *pPlayer = NULL, CvCity *pCity = NULL);
	~UnitSortList();
	UnitSortTypes getActiveSort();
	bool setActiveSort(UnitSortTypes eActiveSort);
	int getNumSort();
	void setPlayer(CvPlayer* pPlayer);
	void setCity(CvCity* pCity);
	bool operator()(UnitTypes eUnit1, UnitTypes eUnit2);

protected:
	UnitSortBase* m_apUnitSort[NUM_UNIT_SORT];
	CvCity* m_pCity;
	CvPlayer* m_pPlayer;
	UnitSortTypes m_eActiveSort;
};

class UnitSortListWrapper
{
public:
	explicit UnitSortListWrapper(UnitSortList* pList) : m_pList(pList) {}
	bool operator()(UnitTypes eUnit1, UnitTypes eUnit2) {return m_pList->operator()(eUnit1, eUnit2);}
protected:
	UnitSortList* m_pList;
};

#endif