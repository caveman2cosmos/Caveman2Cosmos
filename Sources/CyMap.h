#pragma once

#ifndef CyMap_h
#define CyMap_h

//
// Python wrapper class for CvMap 
// SINGLETON
//

class CyPlot;
class CvMap;
class CyCity;
class CySelectionGroup;
class CyArea;
class CyMap
{
public:
	CyMap();
	explicit CyMap(CvMap* pMap);		// Call from C++
	//const CvMapInterfaceBase* getMap() const { return m_pMap; }	// Call from C++
	bool isNone() const { return m_pMap == NULL; }
	
/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	int getType();
	void setType(int iNewType);
	CyMap& operator = (CvMap& kMap);

	bool viewportsEnabled();
	int	getViewportWidth();
	int	getViewportHeight();
	int	getMapXFromViewportX(int iX);
	int	getMapYFromViewportY(int iY);
	int	getViewportXFromMapX(int iX);
	int	getViewportYFromMapY(int iY);
	bool isInViewport(int X, int Y);

	void closeAdvisor(int advisorWidth, int iMinimapLeft, int iMinimapRight, int iMinimapTop, int iMinimapBottom);
	void bringIntoView(int iX, int iY, bool bLookAt, bool bForceCenter, bool bDisplayCityScreen, bool bSelectCity, bool bAddSelectedCity);
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/	
	
/************************************************************************************************/
/* REVOLUTION_MOD                         02/29/08                                jdog5000      */
/*                                                                                              */
/* Used by barbarian civ                                                                        */
/************************************************************************************************/
	void verifyUnitValidPlot();
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

	void erasePlots();
	void setRevealedPlots(int /*TeamTypes*/ eTeam, bool bNewValue, bool bTerrainOnly);
	void resetRevealedPlots(int /*TeamTypes*/ eTeam);
	void setAllPlotTypes(int /*PlotTypes*/ ePlotType);

	void updateVisibility();
	CyPlot* syncRandPlot(int iFlags, int iArea, int iMinUnitDistance, int iTimeout);

	CyCity* findCity(int iX, int iY, int /*PlayerTypes*/ eOwner, int /*TeamTypes*/ eTeam, bool bSameArea, bool bCoastalOnly, int /*TeamTypes*/ eTeamAtWarWith, int /*DirectionTypes*/ eDirection, CyCity* pSkipCity);
	CySelectionGroup* findSelectionGroup(int iX, int iY, int /*PlayerTypes*/ eOwner, bool bReadyToSelect, bool bWorkers);

	CyArea* findBiggestArea(bool bWater);

	int getMapFractalFlags();
	bool findWater(CyPlot* pPlot, int iRange, bool bFreshWater);
	bool isPlot(int iX, int iY);
	int numPlots();
	int plotNum(int iX, int iY);
	int plotX(int iIndex);
	int plotY(int iIndex);
	int getGridWidth();
	int getGridHeight();

	int getLandPlots();
	int getOwnedPlots();

	int getTopLatitude();
	int getBottomLatitude();

	int getNextRiverID();
	void incrementNextRiverID();

	bool isWrapX();
	bool isWrapY();
	std::wstring getMapScriptName();
	WorldSizeTypes getWorldSize();
	ClimateTypes getClimate();
	SeaLevelTypes getSeaLevel();
	int getNumCustomMapOptions();
	CustomMapOptionTypes getCustomMapOption(int iOption);
	int getNumBonuses(int /* BonusTypes */ eIndex);
	int getNumBonusesOnLand(int /* BonusTypes */ eIndex);

	CyPlot* plotByIndex(int iIndex);
	CyPlot* sPlotByIndex(int iIndex);
	CyPlot* plot(int iX, int iY);
	CyPlot* sPlot(int iX, int iY) ;
	CyPlot* pointToPlot(float fX, float fY);
	int getIndexAfterLastArea();
	int getNumAreas();
	int getNumLandAreas();
	CyArea* getArea(int iID);
	void recalculateAreas();
	void resetPathDistance();

	int calculatePathDistance(const CyPlot* pSource, const CyPlot* pDest) const;
	void rebuild(int iGridW, int iGridH, int iTopLatitude, int iBottomLatitude, bool bWrapX, bool bWrapY, WorldSizeTypes eWorldSize, ClimateTypes eClimate, SeaLevelTypes eSeaLevel, int iNumCustomMapOptions, CustomMapOptionTypes * aeCustomMapOptions);
	void regenerateGameElements();
	void updateFog();
	void updateMinimapColor();
	void updateMinOriginalStartDist(const CyArea* pArea);

	// AIAndy: Expose path generation functionality to Python
	bool generatePathForHypotheticalUnit(const CyPlot* pFrom, const CyPlot* pTo, int /*PlayerTypes*/ ePlayer, int /*UnitTypes*/ eUnit, int iFlags, int iMaxTurns) const;
	int getLastPathStepNum() const;
	CyPlot* getLastPathPlotByIndex(int index) const;

	// Super Forts begin *canal* *choke*
	void calculateCanalAndChokePoints();
	// Super Forts end

	// PYTHON HELPER FUNCTIONS
	//int getNumPlayerOwnedPlots(int /*PlayerTypes*/ iPlayer);
/************************************************************************************************/
/* Afforess	                  Start		 07/15/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	python::tuple firstArea(bool bRev);	// returns tuple of (CyArea, iterOut)
	python::tuple nextArea(int iterIn, bool bRev);		// returns tuple of (CyArea, iterOut)
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
protected:
	CvMap* m_pMap;
};

#endif	// CyMap_h
