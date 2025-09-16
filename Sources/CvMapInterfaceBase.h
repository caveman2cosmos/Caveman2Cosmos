#pragma once

#ifndef CIV4_MAP_INTERFACE_BASE_H
#define CIV4_MAP_INTERFACE_BASE_H

//
//	FILE:	 CvMapInterfaceBase.h
//	AUTHOR:  Soren Johnson
//	PURPOSE: Game map class
//-----------------------------------------------------------------------------
//	Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//-----------------------------------------------------------------------------
//

class CvCity;
class CvMapExternal;
class CvPlot;

//
// holds initialization info
//
struct CvMapInitData
{
	int m_iGridW;
	int m_iGridH;
	int m_iTopLatitude;
	int m_iBottomLatitude;

	bool m_bWrapX;
	bool m_bWrapY;

	CvMapInitData(int iGridW=0, int iGridH=0, int iTopLatitude=90, int iBottomLatitude=-90, bool bWrapX=false, bool bWrapY=false) :
		m_iGridH(iGridH), m_iGridW(iGridW), m_iTopLatitude(iTopLatitude), m_iBottomLatitude(iBottomLatitude), m_bWrapY(bWrapY), m_bWrapX(bWrapX)
	{ }
};

//
// CvMapInterfaceBase
//
class CvMapInterfaceBase
{
public:
	CvMapInterfaceBase();
	virtual ~CvMapInterfaceBase();

	CvMapExternal& getProxy() const;

	virtual CvMapInterfaceBase*	getUnderlyingMap() const = 0;

	virtual void init(CvMapInitData* pInitData=NULL) = 0;
	virtual void setupGraphical() = 0;
	virtual void reset(CvMapInitData* pInitData) = 0;

	virtual void erasePlots() = 0;

	virtual void updateFlagSymbols() = 0;
	virtual void updateFog(const bool bApplyDecay = false) = 0;
	virtual void updateSymbolVisibility() = 0;
	virtual void updateMinimapColor() = 0;
	virtual void updateCenterUnit() = 0;

	virtual CvCity* findCity(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, bool bSameArea = true, bool bCoastalOnly = false, TeamTypes eTeamAtWarWith = NO_TEAM, DirectionTypes eDirection = NO_DIRECTION, const CvCity* pSkipCity = NULL) const = 0;

	virtual bool isPlot(int iX, int iY) const = 0;
	virtual int numPlots() const = 0;

	virtual float plotXToPointX(int iX) const = 0;
	virtual float plotYToPointY(int iY) const = 0;

	virtual int getGridWidth() const = 0;
	virtual int getGridHeight() const = 0;

	virtual bool isWrapX() const = 0;
	virtual bool isWrapY() const = 0;
	virtual bool isWrap() const = 0;

	virtual CvPlot* plotByIndex(int iIndex) const = 0;
	virtual CvPlot* plot(int iX, int iY) const = 0;
	virtual CvPlot* pointToPlot(float fX, float fY) const = 0;

private:
	CvMapExternal* m_proxy;
};

#endif
