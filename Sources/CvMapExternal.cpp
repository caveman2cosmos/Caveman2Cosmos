//
//	FILE:	 CvMapExternal.cpp
//	AUTHOR:  Steve Draper
//	PURPOSE: Game map externals class
//

#include "CvGameCoreDLL.h"
#include "CvCity.h"
#include "CvGlobals.h"
#include "CvMap.h"
#include "CvMapExternal.h"
#include "CvPlot.h"
#include "CvTaggedSaveFormatWrapper.h"
#include "CvViewport.h"

CvMapExternal::CvMapExternal() : m_proxiedMap(NULL),
								 m_bMapCoordinates(false)
{
}

CvMapExternal::CvMapExternal(CvMapInterfaceBase* proxiedMap) : m_proxiedMap(proxiedMap),
															   m_bMapCoordinates(false)
{
}


CvMapExternal::~CvMapExternal()
{
}

// FUNCTION: init()
// Initializes the map.
// Parameters:
//	pInitInfo					- Optional init structure (used for WB load)
// Returns:
//	nothing.
void CvMapExternal::init(CvMapInitData* pInitInfo/*=NULL*/)
{
	m_proxiedMap->init(pInitInfo);
}

// FUNCTION: reset()
// Initializes data members that are serialized.
void CvMapExternal::reset(CvMapInitData* pInitInfo)
{
	m_proxiedMap->reset(pInitInfo);
}


//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
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
	return m_proxiedMap->getWorldSize();
}


CvPlot* CvMapExternal::plotByIndex(int iIndex) const
{
	FASSERT_BOUNDS(0, numPlots(), iIndex)
	CvPlot* result = m_proxiedMap->plotByIndex(iIndex);

	if (result == NULL)
	{
		OutputDebugString(CvString::format("    Plot at (%d,%d): NULL\n").c_str());
	}
	return result;
}


CvPlot* CvMapExternal::plot(int iX, int iY) const
{	
	if (!m_bMapCoordinates)
	{
		return m_proxiedMap->getUnderlyingMap()->plot(iX, iY);
	}
	else
	{
		return m_proxiedMap->plot(iX, iY);
	}
}


CvPlot* CvMapExternal::pointToPlot(float fX, float fY)
{
	return m_proxiedMap->pointToPlot(fX, fY);
}


// Serialization:
void CvMapExternal::read(FDataStreamBase* pStream)
{
	PROFILE_FUNC();

	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	bool bInitialized = false;
	bool bMultiMapFormat = false;

	WRAPPER_READ(wrapper, "CvMapExternal", &bMultiMapFormat);

	if ( bMultiMapFormat )
	{
		for (int i = 0; i < GC.getNumMapInfos(); i++)
		{
			WRAPPER_READ(wrapper, "CvMapExternal", &bInitialized);

			if (bInitialized)
			{
				//	If this is a load straight from the startup menus then only map 0 will
				//	have been initialized, so any others in the save must be initialized too before
				//	they can be loaded
				if ( !GC.mapInitialized((MapTypes)i) )
				{
					GC.initializeMap((MapTypes)i);
				}

				// Cast to the internal class
				CvMap&	map = GC.getMapByIndex((MapTypes)i);

				map.read(pStream);
			}
		}
	}
	else
	{
		GC.getMap().read(pStream);
	}

	GC.getCurrentViewport()->setActionState(VIEWPORT_ACTION_STATE_LOADING);
}

void CvMapExternal::write(FDataStreamBase* pStream)
{
	PROFILE_FUNC();

	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	bool bMultiMapFormat = true;	//	Always save in multimap format

	WRAPPER_WRITE(wrapper, "CvMapExternal", bMultiMapFormat);

	for (int iI = 0; iI < GC.getNumMapInfos(); iI++)
	{
		const bool bInitialized = GC.mapInitialized((MapTypes)iI);

		WRAPPER_WRITE(wrapper, "CvMapExternal", bInitialized);

		if ( bInitialized )
		{
			// Cast to the internal class
			CvMap&	map = GC.getMapByIndex((MapTypes)iI);

			map.write(pStream);
		}
	}
}
