#pragma once

#ifndef CyMap_h
#define CyMap_h

//
// Python wrapper class for CvMap
//

class CyPlot;
class CvMap;
class CyArea;
class CyUnit;

class CyMap
{
public:
	CyMap();
	explicit CyMap(MapTypes eMap);

	//const CvMapInterfaceBase* getMap() const { return m_pMap; } // Call from C++

	int getType();
	CyMap& operator = (CvMap& kMap);

	bool plotsInitialized() const;

	bool viewportsEnabled();
	int	getViewportWidth();
	int	getViewportHeight();
	int	getMapXFromViewportX(int iX);
	int	getMapYFromViewportY(int iY);
	int	getViewportXFromMapX(int iX);
	int	getViewportYFromMapY(int iY);
	bool isInViewport(int X, int Y);
	bool isMidSwitch() const;

	void closeAdvisor(int advisorWidth, int iMinimapLeft, int iMinimapRight, int iMinimapTop, int iMinimapBottom);
	void bringIntoView(int iX, int iY, bool bLookAt, bool bForceCenter, bool bDisplayCityScreen, bool bSelectCity, bool bAddSelectedCity);

	void verifyUnitValidPlot();

	void erasePlots();
	void setRevealedPlots(int /*TeamTypes*/ eTeam, bool bNewValue, bool bTerrainOnly);
	void resetRevealedPlots(int /*TeamTypes*/ eTeam);
	void setAllPlotTypes(int /*PlotTypes*/ ePlotType);

	void updateVisibility();
	CyPlot* syncRandPlot(int iFlags, int iArea, int iMinUnitDistance, int iTimeout);

	CyArea* findBiggestArea(bool bWater);

	int getMapFractalFlags();
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

	python::list plots() const;
	CyPlot* plotByIndex(int iIndex);
	CyPlot* sPlotByIndex(int iIndex);
	CyPlot* plot(int iX, int iY);
	CyPlot* sPlot(int iX, int iY) ;
	CyPlot* pointToPlot(float fX, float fY);

	int getNumAreas();
	int getNumLandAreas();
	CyArea* getArea(int iID);
	python::list areas() const;
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

	void moveUnitToMap(const CyUnit* unit, int numTravelTurns);

	// Super Forts *canal* *choke*
	void calculateCanalAndChokePoints();

protected:
	CvMap* m_pMap;
};

#endif	// CyMap_h
