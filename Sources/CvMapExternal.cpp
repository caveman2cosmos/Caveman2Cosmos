//
//	FILE:	 CvMapExternal.cpp
//	AUTHOR:  Steve Draper
//	PURPOSE: Game map externals class
//


#include "CvGameCoreDLL.h"
#include "CvMapExternal.h"
#include "CvMapGenerator.h"
#include "CvFractal.h"

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
#include <direct.h>			// for getcwd()
#include <stdlib.h>			// for MAX_PATH
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

// Public Functions...

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


void CvMapExternal::updateVisibility()
{
	m_proxiedMap->updateVisibility();
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

CvSelectionGroup* CvMapExternal::findSelectionGroup(int iX, int iY, PlayerTypes eOwner, bool bReadyToSelect, bool bWorkers)
{
	return m_proxiedMap->findSelectionGroup(iX, iY, eOwner, bReadyToSelect, bWorkers);
}

bool CvMapExternal::isPlot(int iX, int iY) const																		// Exposed to Python
{
	return m_proxiedMap->isPlot(iX, iY);
}

int CvMapExternal::numPlots() const 																								// Exposed to Python
{
	return m_proxiedMap->numPlots();
}


int CvMapExternal::plotNum(int iX, int iY) const																		// Exposed to Python
{
	return m_proxiedMap->plotNum(iX, iY);
}


int CvMapExternal::pointXToPlotX(float fX) const
{
	return m_proxiedMap->pointXToPlotX(fX);
}


float CvMapExternal::plotXToPointX(int iX)
{
	return m_proxiedMap->plotXToPointX(iX);
}


int CvMapExternal::pointYToPlotY(float fY) const
{
	return m_proxiedMap->pointYToPlotY(fY);
}


float CvMapExternal::plotYToPointY(int iY)
{
	return m_proxiedMap->plotYToPointY(iY);
}


int CvMapExternal::getGridWidth() const																		// Exposed to Python
{
	return m_proxiedMap->getGridWidth();
}


int CvMapExternal::getGridHeight() const																	// Exposed to Python
{
	return m_proxiedMap->getGridHeight();
}


int CvMapExternal::getLandPlots() const																					// Exposed to Python
{
	return m_proxiedMap->getLandPlots();
}


int CvMapExternal::getOwnedPlots() const																				// Exposed to Python
{
	return m_proxiedMap->getOwnedPlots();
}


bool CvMapExternal::isWrapX()																							// Exposed to Python
{
	return m_proxiedMap->isWrapX();
}


bool CvMapExternal::isWrapY()																							// Exposed to Python
{
	return m_proxiedMap->isWrapY();
}


bool CvMapExternal::isWrap()																							// Exposed to Python
{
	return m_proxiedMap->isWrap();
}


WorldSizeTypes CvMapExternal::getWorldSize()															// Exposed to Python
{
	return m_proxiedMap->getWorldSize();
}


ClimateTypes CvMapExternal::getClimate() const																	// Exposed to Python
{
	return m_proxiedMap->getClimate();
}


SeaLevelTypes CvMapExternal::getSeaLevel() const																// Exposed to Python
{
	return m_proxiedMap->getSeaLevel();
}


int CvMapExternal::getNumCustomMapOptions() const
{
	return m_proxiedMap->getNumCustomMapOptions();
}


CustomMapOptionTypes CvMapExternal::getCustomMapOption(int iOption) const				// Exposed to Python
{
	return m_proxiedMap->getCustomMapOption(iOption);
}


CvPlot* CvMapExternal::plotByIndex(int iIndex) const											// Exposed to Python
{
	FAssert( 0 <= iIndex && iIndex < numPlots());
	CvPlot* result = m_proxiedMap->plotByIndex(iIndex);

	if ( result == NULL )
	{
		OutputDebugString(CvString::format("    Plot at (%d,%d): NULL\n").c_str());
	}
	return result;
}


CvPlot* CvMapExternal::plot(int iX, int iY) const													// Exposed to Python
{
	CvPlot* result;
	
	if ( !m_bMapCoordinates )
	{
		result = m_proxiedMap->getUnderlyingMap()->plot(iX, iY);
	}
	else
	{
		result = m_proxiedMap->plot(iX, iY);
	}

	return result;
}


CvPlot* CvMapExternal::pointToPlot(float fX, float fY)
{
	return m_proxiedMap->pointToPlot(fX, fY);
}


int CvMapExternal::getNumAreas() const														// Exposed to Python
{
	return m_proxiedMap->getNumAreas();
}


int CvMapExternal::getNumLandAreas() const
{
	return m_proxiedMap->getNumLandAreas();
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
		GC.getMapINLINE().read(pStream);
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
		bool bInitialized = GC.mapInitialized((MapTypes)iI);

		WRAPPER_WRITE(wrapper, "CvMapExternal", bInitialized);

		if ( bInitialized )
		{
			// Cast to the internal class
			CvMap&	map = GC.getMapByIndex((MapTypes)iI);

			map.write(pStream);
		}
	}
}
