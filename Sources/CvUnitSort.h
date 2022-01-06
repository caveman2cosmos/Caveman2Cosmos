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

class CvCity;
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
	virtual int getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const = 0;
	// Returns if eUnit1 is before eUnit2, default for the value based sorts is greater first
	virtual bool isLesserUnit(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit1, UnitTypes eUnit2) const;
	bool isInverse() const;
	bool setInverse(bool bInvert);
protected:
	bool m_bInvert;
};

class UnitSortStrength : public UnitSortBase
{
public:
	UnitSortStrength(bool bInvert = false) : UnitSortBase(bInvert) {};
	int getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const;
};

class UnitSortMove : public UnitSortBase
{
public:
	UnitSortMove(bool bInvert = false) : UnitSortBase(bInvert) {};
	int getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const;
};

class UnitSortCollateral : public UnitSortBase
{
public:
	UnitSortCollateral(bool bInvert = false) : UnitSortBase(bInvert) {};
	int getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const;
};

class UnitSortRange : public UnitSortBase
{
public:
	UnitSortRange(bool bInvert = false) : UnitSortBase(bInvert) {};
	int getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const;
};

class UnitSortBombard : public UnitSortBase
{
public:
	UnitSortBombard(bool bInvert = false) : UnitSortBase(bInvert) {};
	int getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const;
};

class UnitSortCargo : public UnitSortBase
{
public:
	UnitSortCargo(bool bInvert = false) : UnitSortBase(bInvert) {};
	int getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const;
};

class UnitSortWithdrawal : public UnitSortBase
{
public:
	UnitSortWithdrawal(bool bInvert = false) : UnitSortBase(bInvert) {};
	int getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const;
};

class UnitSortPower : public UnitSortBase
{
public:
	UnitSortPower(bool bInvert = false) : UnitSortBase(bInvert) {};
	int getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const;
};

class UnitSortCost : public UnitSortBase
{
public:
	UnitSortCost(bool bInvert = false) : UnitSortBase(bInvert) {};
	int getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const;
};

class UnitSortName : public UnitSortBase
{
public:
	UnitSortName(bool bInvert = false) : UnitSortBase(bInvert) {};
	bool isLesserUnit(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit1, UnitTypes eUnit2) const;
	int getUnitValue(const CvPlayer *pPlayer, const CvCity *pCity, UnitTypes eUnit) const;
};

class UnitSortList
{
public:
	UnitSortList(const CvPlayer *pPlayer = NULL, const CvCity *pCity = NULL);
	~UnitSortList();
	UnitSortTypes getActiveSort() const;
	bool setActiveSort(UnitSortTypes eActiveSort);
	void setPlayer(const CvPlayer* pPlayer);
	void setCity(const CvCity* pCity);
	bool operator()(UnitTypes eUnit1, UnitTypes eUnit2) const;

protected:
	UnitSortBase* m_apUnitSort[NUM_UNIT_SORT];
	const CvCity* m_pCity;
	const CvPlayer* m_pPlayer;
	UnitSortTypes m_eActiveSort;
};

class UnitSortListWrapper
{
public:
	explicit UnitSortListWrapper(const UnitSortList* pList) : m_pList(pList) {}
	bool operator()(UnitTypes eUnit1, UnitTypes eUnit2) const { return m_pList->operator()(eUnit1, eUnit2); }
protected:
	const UnitSortList* m_pList;
};

#endif