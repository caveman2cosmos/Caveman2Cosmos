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
class CyUnit;
class CyArea;
class CyMap
{
public:
	CyMap();
	explicit CyMap(CvMap* pMap);		// Call from C++
	//CvMapInterfaceBase* getMap() { return m_pMap;	}	// Call from C++
	bool isNone() { return (m_pMap==NULL); }
	
/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	CyMap& operator = (CvMap& kMap);

	int	getViewportWidth() const;
	int	getViewportHeight() const;
	int	getViewportXFromMapX(int iX) const;
	int	getViewportYFromMapY(int iY) const;
	bool isInViewport(int X, int Y) const;

	void closeAdvisor(int advisorWidth, int iMinimapLeft, int iMinimapRight, int iMinimapTop, int iMinimapBottom);
	void bringIntoView(int iX, int iY, bool bLookAt, bool bForceCenter, bool bDisplayCityScreen, bool bSelectCity, bool bAddSelectedCity);
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/	
	
	void verifyUnitValidPlot();

	void resetRevealedPlots(int /*TeamTypes*/ eTeam);
	void setAllPlotTypes(int /*PlotTypes*/ ePlotType);

	void updateVisibility();
	CyPlot* syncRandPlot(int iFlags, int iArea, int iMinUnitDistance, int iTimeout) const;

	CyCity* findCity(int iX, int iY, int /*PlayerTypes*/ eOwner, int /*TeamTypes*/ eTeam, bool bSameArea, bool bCoastalOnly, int /*TeamTypes*/ eTeamAtWarWith, int /*DirectionTypes*/ eDirection, CyCity* pSkipCity) const;

	CyArea* findBiggestArea(bool bWater) const;

	int getMapFractalFlags() const;
	int numPlots() const;
	int plotNum(int iX, int iY) const;
	int getGridWidth() const;
	int getGridHeight() const;

	int getLandPlots() const;
	int getOwnedPlots() const;

	int getTopLatitude() const;
	int getBottomLatitude() const;

	int getNextRiverID() const;
	void incrementNextRiverID();

	bool isWrapX() const;
	bool isWrapY() const;
	std::wstring getMapScriptName() const;
	WorldSizeTypes getWorldSize() const;
	ClimateTypes getClimate() const;
	SeaLevelTypes getSeaLevel() const;
	int getNumCustomMapOptions() const;
	CustomMapOptionTypes getCustomMapOption(int iOption) const;
	int getNumBonuses(int /*BonusTypes*/ eIndex) const;

	CyPlot* plotByIndex(int iIndex) const;
	CyPlot* sPlotByIndex(int iIndex) const;
	CyPlot* plot(int iX, int iY) const;
	CyPlot* sPlot(int iX, int iY) const;
	int getIndexAfterLastArea() const;
	int getNumAreas() const;
	int getNumLandAreas() const;
	CyArea* getArea(int iID) const;
	void recalculateAreas();

	void rebuild(int iGridW, int iGridH, int iTopLatitude, int iBottomLatitude, bool bWrapX, bool bWrapY, WorldSizeTypes eWorldSize, ClimateTypes eClimate, SeaLevelTypes eSeaLevel, int iNumCustomMapOptions, CustomMapOptionTypes * aeCustomMapOptions);
	void updateMinimapColor();

	// AIAndy: Expose path generation functionality to Python
	bool generatePathForHypotheticalUnit(CyPlot* pFrom, CyPlot* pTo, int /*PlayerTypes*/ ePlayer, int /*UnitTypes*/ eUnit, int iFlags, int iMaxTurns) const;
	int getLastPathStepNum() const;
	CyPlot* getLastPathPlotByIndex(int index) const;

	// PYTHON HELPER FUNCTIONS
	//int getNumPlayerOwnedPlots(int /*PlayerTypes*/ iPlayer);

protected:
	CvMap* m_pMap;
};

#endif	// CyMap_h
