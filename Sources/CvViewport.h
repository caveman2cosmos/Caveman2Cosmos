#pragma once

#ifndef CIV4_VIEWPORT_H
#define CIV4_VIEWPORT_H

//
//	FILE:	 CvViewport.h
//	AUTHOR:  Steve Draper
//	PURPOSE: Viewport presented as a map to the game engine

#include "CvGlobals.h"
#include "CvMap.h"

class CvCity;
class CvPlot;
class CvUnit;

enum ViewportMode
{
	VIEWPORT_MODE_UNINITIALIZED,
	VIEWPORT_MODE_INITIALIZED
};

enum ViewportTransformType
{
	VIEWPORT_TRANSFORM_TYPE_WINDOW,
	VIEWPORT_TRANSFORM_TYPE_SCALE
};

//	Some processing that is done in viewport tranisitions has to occur
//	over several messaging timeslices (empirically - the game engine
//	gets things wrong if you don't give it a chnace to process messages
//	in between).  To enact that we use a state machine which performs state
//	transitions on each call to CvGame::update().
enum ViewportDeferredActionState
{
	VIEWPORT_ACTION_STATE_NONE,
	VIEWPORT_ACTION_STATE_LAUNCH_MILITARY_ADVISOR,
	VIEWPORT_ACTION_STATE_MILITARY_ADVISOR_LAUNCHING,
	VIEWPORT_ACTION_STATE_MILITARY_ADVISOR_LAUNCHED,
	VIEWPORT_ACTION_STATE_BRING_INTO_VIEW,
	VIEWPORT_ACTION_STATE_BRING_INTO_VIEW_COMPLETE,
	VIEWPORT_ACTION_STATE_SET_LOOKAT,
	VIEWPORT_ACTION_STATE_SET_SELECTION,
	VIEWPORT_ACTION_STATE_LOADING,
	VIEWPORT_ACTION_STATE_SAVING,
	VIEWPORT_ACTION_STATE_AFTER_SWITCH
};

//	Graphics have to be spoofed to prevent unrevealed territory being visible
//	durign teh fade down (as th ame engine THINKS a plot ios going from revealed
//	to unrevealed in a viewport switch, when it's actually a totally different plot
//	There is no totally clean way to do this, so we compromise with 3 states as
//	per this enum.  Durin a viewport switch we transition between the last two states,
//	initially using VIEWPORT_SPOOF_ALL_UNREVEALED, which ensures no 'escaping' visibility
//	at the cost of some artifcating around the edges of the revealed region.  A few seconds
//	later (after the fade down) we chnage to VIEWPORT_SPOOF_NOT_ADJACENT_TO_REVEALED
//	which fixes the artifacting
enum ViewportGraphicalSpoofingState
{
	VIEWPORT_SPOOF_NONE,
	VIEWPORT_SPOOF_ALL_UNREVEALED,
	VIEWPORT_SPOOF_NOT_ADJACENT_TO_REVEALED
};

//
// CvViewport
//
class CvViewport : public CvMapInterfaceBase
{
public:
	CvViewport(CvMap* map);
	virtual ~CvViewport();

	virtual CvMapInterfaceBase*	getUnderlyingMap() const { return m_pMap; }

	virtual void init(CvMapInitData* pInitData=NULL);
	virtual void setupGraphical();
	virtual void reset(CvMapInitData* pInitData);

	void getMapOffset(int& iX, int& iY) const;
	void setMapOffset(int iX, int iY);
	void setOffsetToShow(int iX, int iY);	//	Make this the centre or near as appropiate
	void resizeForMap();
	void bringIntoView(int iX, int iY, const CvUnit* pSelectionUnit = NULL, bool bForceCenter = false, bool bDisplayCityScreen = false, bool bSelectCity = false, bool bAddSelectedCity = false);
	void centerOnSelection();
	void panLeft();
	void panRight();
	void panUp();
	void panDown();

	inline ViewportMode getState() const { return m_mode; }

	virtual void erasePlots();

	virtual void updateFlagSymbols();
	virtual void updateFog(const bool bApplyDecay);
	virtual void updateSymbolVisibility();
	virtual void updateMinimapColor();
	virtual void updateCenterUnit();

	virtual CvCity* findCity(int iX, int iY, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, bool bSameArea = true, bool bCoastalOnly = false, TeamTypes eTeamAtWarWith = NO_TEAM, DirectionTypes eDirection = NO_DIRECTION, const CvCity* pSkipCity = NULL) const;

	virtual bool isPlot(int iX, int iY) const;
	virtual int numPlots() const;

	int plotNum(int iX, int iY) const;
	//int plotX(int iIndex) const;
	//int plotY(int iIndex) const;

	int pointXToPlotX(float fX) const;
	virtual float plotXToPointX(int iX) const;

	int pointYToPlotY(float fY) const;
	virtual float plotYToPointY(int iY) const;

	//float getWidthCoords() const;
	//float getHeightCoords() const;

	virtual int getGridWidth() const;
	virtual int getGridHeight() const;

	virtual bool isWrapX() const;
	virtual bool isWrapY() const;
	virtual bool isWrap() const;

	virtual CvPlot* plotByIndex(int iIndex) const;
	virtual CvPlot* plot(int iX, int iY) const;
	virtual CvPlot* pointToPlot(float fX, float fY) const;

	//inline CvPlot* plotSorenINLINE(int iX, int iY) const
	//{
	//	if ((iX == INVALID_PLOT_COORD) || (iY == INVALID_PLOT_COORD))
	//	{
	//		return NULL;
	//	}
	//	return plot(iX, iY);
	//}

	// Public methods used in coordinate transformation
	inline int	getViewportXFromMapX(int iMapX) const
	{
		if ( m_transformType == VIEWPORT_TRANSFORM_TYPE_WINDOW )
		{
			if ( isInViewportX(iMapX) )
			{
				int iResult = iMapX - m_iXOffset;

				if ( iResult < 0 )
				{
					iResult += m_pMap->getGridWidth();
				}
				else if ( iResult >= m_iXSize )
				{
					iResult -= m_pMap->getGridWidth();
				}

				FASSERT_BOUNDS(0, m_iXSize, iResult);

				return iResult;
			}
			else
			{
				return INVALID_PLOT_COORD;
			}
		}
		else if ( m_transformType == VIEWPORT_TRANSFORM_TYPE_SCALE )
		{
			return (iMapX * m_iXSize)/m_pMap->getGridWidth();
		}
		else
		{
			FErrorMsg("Invalid viewport transform type");
			return -1;
		}
	}
	inline int	getViewportYFromMapY(int iMapY) const
	{
		if ( m_transformType == VIEWPORT_TRANSFORM_TYPE_WINDOW )
		{
			if ( isInViewportY(iMapY) )
			{
				int iResult = iMapY - m_iYOffset;

				if ( iResult < 0 )
				{
					iResult += m_pMap->getGridHeight();
				}
				else if ( iResult >= m_iYSize )
				{
					iResult -= m_pMap->getGridHeight();
				}

				FASSERT_BOUNDS(0, m_iYSize, iResult);

				return iResult;
			}
			else
			{
				return INVALID_PLOT_COORD;
			}
		}
		else if ( m_transformType == VIEWPORT_TRANSFORM_TYPE_SCALE )
		{
			return (iMapY * m_iYSize)/m_pMap->getGridHeight();
		}
		else
		{
			FErrorMsg("Invalid viewport transform type");
			return -1;
		}
	}
	inline int	getMapXFromViewportX(int iViewportX) const
	{
		if ( m_transformType == VIEWPORT_TRANSFORM_TYPE_WINDOW )
		{
			if (iViewportX == INVALID_PLOT_COORD)
			{
				return INVALID_PLOT_COORD;
			}
			else if ( !isWrapX() && (iViewportX < 0 || iViewportX >= m_iXSize) )
			{
				return INVALID_PLOT_COORD;
			}
			else
			{
				return (iViewportX + m_iXOffset + m_pMap->getGridWidth()) % m_pMap->getGridWidth();
			}
		}
		else if ( m_transformType == VIEWPORT_TRANSFORM_TYPE_SCALE )
		{
			return (iViewportX * m_pMap->getGridWidth() + m_iXSize - 1)/m_iXSize;
		}
		else
		{
			FErrorMsg("Invalid viewport transform type");
			return -1;
		}
	}
	inline int	getMapYFromViewportY(int iViewportY) const
	{
		if ( m_transformType == VIEWPORT_TRANSFORM_TYPE_WINDOW )
		{
			if (iViewportY == INVALID_PLOT_COORD)
			{
				return INVALID_PLOT_COORD;
			}
			else if ( !isWrapY() && (iViewportY < 0 || iViewportY >= m_iYSize) )
			{
				return INVALID_PLOT_COORD;
			}
			else
			{
				return (iViewportY + m_iYOffset + m_pMap->getGridHeight()) % m_pMap->getGridHeight();
			}
		}
		else if ( m_transformType == VIEWPORT_TRANSFORM_TYPE_SCALE )
		{
			return (iViewportY * m_pMap->getGridHeight() + m_iYSize - 1)/m_iYSize;
		}
		else
		{
			FErrorMsg("Invalid viewport transform type");
			return -1;
		}
	}
	inline bool isInViewportX(int iMapX) const
	{
		if ( m_transformType == VIEWPORT_TRANSFORM_TYPE_WINDOW )
		{
			//	Normalize for map wrappings
			if ( m_pMap->isWrapX() )
			{
				if ( m_iXOffset < 0 && iMapX >= m_pMap->getGridWidth() + m_iXOffset )
				{
					iMapX -= m_pMap->getGridWidth();
				}
				else if ( m_iXOffset > m_pMap->getGridWidth() - m_iXSize && iMapX < m_iXOffset - m_iXSize )
				{
					iMapX += m_pMap->getGridWidth();
				}
			}
			return (iMapX >= m_iXOffset && iMapX - m_iXOffset < m_iXSize);
		}
		else if ( m_transformType == VIEWPORT_TRANSFORM_TYPE_SCALE )
		{
			//	Check if this plot maps back to itself after rounding (else hide it)
			return (getMapXFromViewportX(getViewportXFromMapX(iMapX)) == iMapX);
		}
		else
		{
			FErrorMsg("Invalid viewport transform type");
			return false;
		}
	}
	inline bool isInViewportY(int iMapY) const
	{
		if ( m_transformType == VIEWPORT_TRANSFORM_TYPE_WINDOW )
		{
			//	Normalize for map wrappings
			if ( m_pMap->isWrapY() )
			{
				if ( m_iYOffset < 0 && iMapY >= m_pMap->getGridHeight() + m_iYOffset )
				{
					iMapY -= m_pMap->getGridHeight();
				}
				else if ( m_iYOffset > m_pMap->getGridHeight() - m_iYSize && iMapY < m_iYOffset - m_iYSize )
				{
					iMapY += m_pMap->getGridHeight();
				}
			}
			return (iMapY >= m_iYOffset && iMapY - m_iYOffset < m_iYSize);
		}
		else if ( m_transformType == VIEWPORT_TRANSFORM_TYPE_SCALE )
		{
			//	Check if this plot maps back to itself after rounding (else hide it)
			return (getMapYFromViewportY(getViewportYFromMapY(iMapY)) == iMapY);
		}
		else
		{
			FErrorMsg("Invalid viewport transform type");
			return false;
		}
	}
	inline bool isInViewport(int iMapX, int iMapY, int comfortBorderSize = 0) const
	{
		if ( !GC.viewportsEnabled() )
		{
			return true;
		}
		else if ( comfortBorderSize > 0 )
		{
			int iMinX = coordRange(iMapX - comfortBorderSize, m_pMap->getGridWidth(), m_pMap->isWrapX());
			int iMaxX = coordRange(iMapX + comfortBorderSize, m_pMap->getGridWidth(), m_pMap->isWrapX());
			int iMinY = coordRange(iMapY - comfortBorderSize, m_pMap->getGridHeight(), m_pMap->isWrapY());
			int iMaxY = coordRange(iMapY + comfortBorderSize, m_pMap->getGridHeight(), m_pMap->isWrapY());

			return isInViewportX(iMinX) && isInViewportX(iMaxX) && isInViewportY(iMinY) && isInViewportY(iMaxY);
		}
		else
		{
			return isInViewportX(iMapX) && isInViewportY(iMapY);
		}
	}

	inline ViewportTransformType getTransformType() const
	{
		return m_transformType;
	};

	//	Close an advisor screen which was using a full-map minimap
	void closeAdvisor(int advisorWidth, int iMinimapLeft, int iMinimapRight, int iMinimapTop, int iMinimapBottom);
	//	Process the current action state (which may include transitioning to another state)
	void processActionState();
	//	Transition to a new action state
	void setActionState(ViewportDeferredActionState newState, bool bProcessImmediately = false);

	//	During some state transitions we need to inhibit calls to update the selection cycle.  This
	//	retrieves the inhibition status
	inline bool isSelectionInhibitted() const
	{
		return m_inhibitSelection;
	}

	inline ViewportGraphicalSpoofingState getSpoofHiddenGraphics() const
	{
		return m_eSpoofHiddenGraphics;
	}
	void setSpoofHiddenGraphics(ViewportGraphicalSpoofingState eValue);


private:
	void beforeSwitch();
	void afterSwitch();

	CvMap*	m_pMap;
	int		m_iXOffset;
	int		m_iYOffset;
	int		m_iXSize;
	int		m_iYSize;

	ViewportMode					m_mode;
	ViewportTransformType			m_transformType;
	ViewportDeferredActionState		m_state;
	IDInfo							m_preservedHeadSelectedUnitId;
	CvPlot* m_pLookatPlot;
	bool							m_inhibitSelection;
	bool							m_bDisplayCityScreen;
	bool							m_bSelectCity;
	bool							m_bAddSelectedCity;
	//int							m_countdown;
	ViewportGraphicalSpoofingState	m_eSpoofHiddenGraphics;
	DWORD							m_spoofTransitionStartTickCount;
};

#endif
