//
//	FILE:	 CvMapExternal.cpp
//	AUTHOR:  Steve Draper
//	PURPOSE: Game map externals class
//

#include "CvGameCoreDLL.h"
#include "CvCity.h"
#include "CvGlobals.h"
#include "CvInitCore.h"
#include "CvMap.h"
#include "CvMapExternal.h"
#include "CvPlot.h"
#include "CvTaggedSaveFormatWrapper.h"
#include "CvViewport.h"

CvMapExternal::CvMapExternal(CvMapInterfaceBase* proxiedMap)
	: m_proxiedMap(proxiedMap)
{
}


CvMapExternal::~CvMapExternal()
{
}


void CvMapExternal::init(CvMapInitData* pInitInfo/*=NULL*/)
{
	m_proxiedMap->init(pInitInfo);
}


void CvMapExternal::reset(CvMapInitData* pInitInfo)
{
	m_proxiedMap->reset(pInitInfo);
}


void CvMapExternal::setupGraphical()
{
	m_proxiedMap->setupGraphical();
}


void CvMapExternal::erasePlots()
{
	m_proxiedMap->erasePlots();
}


void CvMapExternal::updateFlagSymbols()
{
	m_proxiedMap->updateFlagSymbols();
}


void CvMapExternal::updateFog()
{
	m_proxiedMap->updateFog();
}


void CvMapExternal::updateSymbolVisibility()
{
	m_proxiedMap->updateSymbolVisibility();
}


void CvMapExternal::updateMinimapColor()
{
	m_proxiedMap->updateMinimapColor();
}


void CvMapExternal::updateCenterUnit()
{
	m_proxiedMap->updateCenterUnit();
}


CvCity* CvMapExternal::findCity(int iX, int iY, PlayerTypes eOwner, TeamTypes eTeam, bool bSameArea, bool bCoastalOnly, TeamTypes eTeamAtWarWith, DirectionTypes eDirection, CvCity* pSkipCity)
{
	return m_proxiedMap->findCity(iX, iY, eOwner, eTeam, bSameArea, bCoastalOnly, eTeamAtWarWith, eDirection, pSkipCity);
}


bool CvMapExternal::isPlot(int iX, int iY) const
{
	return m_proxiedMap->isPlot(iX, iY);
}


int CvMapExternal::numPlots() const
{
	return m_proxiedMap->numPlots();
}


float CvMapExternal::plotXToPointX(int iX)
{
	return m_proxiedMap->plotXToPointX(iX);
}


float CvMapExternal::plotYToPointY(int iY)
{
	return m_proxiedMap->plotYToPointY(iY);
}


int CvMapExternal::getGridWidth() const
{
	return m_proxiedMap->getGridWidth();
}


int CvMapExternal::getGridHeight() const
{
	return m_proxiedMap->getGridHeight();
}


bool CvMapExternal::isWrapX()
{
	return m_proxiedMap->isWrapX();
}


bool CvMapExternal::isWrapY()
{
	return m_proxiedMap->isWrapY();
}


bool CvMapExternal::isWrap()
{
	return m_proxiedMap->isWrap();
}


WorldSizeTypes CvMapExternal::getWorldSize()
{
	return GC.getInitCore().getWorldSize();
}


CvPlot* CvMapExternal::plotByIndex(int iIndex) const
{
	FASSERT_BOUNDS(0, numPlots(), iIndex);
	CvPlot* result = m_proxiedMap->plotByIndex(iIndex);

	if (result == NULL)
	{
		OutputDebugString(CvString::format("    Plot at (%d,%d): NULL\n").c_str());
	}
	return result;
}


CvPlot* CvMapExternal::plot(int iX, int iY) const
{
	return m_proxiedMap->plot(iX, iY);
}


CvPlot* CvMapExternal::pointToPlot(float fX, float fY)
{
	return m_proxiedMap->pointToPlot(fX, fY);
}


// Serialization:
void CvMapExternal::read(FDataStreamBase* pStream)
{
	PROFILE_FUNC();

	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

#ifndef BREAK_SAVES
	bool bMultiMapFormat = false;

	WRAPPER_READ(wrapper, "CvMapExternal", &bMultiMapFormat);
#endif

	foreach_(CvMap* map, GC.getMaps())
	{
		bool bInitialized = false;
		WRAPPER_READ(wrapper, "CvMapExternal", &bInitialized);

		if (bInitialized)
		{
			map->read(pStream);
		}
	}

	GC.getCurrentViewport()->setActionState(VIEWPORT_ACTION_STATE_LOADING);
}

void CvMapExternal::write(FDataStreamBase* pStream)
{
	PROFILE_FUNC();

	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

#ifndef BREAK_SAVES
	bool bMultiMapFormat = true;	//	Always save in multimap format

	WRAPPER_WRITE(wrapper, "CvMapExternal", bMultiMapFormat);
#endif

	foreach_(CvMap* map, GC.getMaps())
	{
		const bool bInitialized = map->plotsInitialized();

		WRAPPER_WRITE(wrapper, "CvMapExternal", bInitialized);

		if (bInitialized)
		{
			map->write(pStream);
		}
	}
}
