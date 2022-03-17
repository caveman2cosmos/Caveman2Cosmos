#pragma once

#ifndef CyPlot_h__
#define CyPlot_h__

//
// Python wrapper class for CvPlot
//

class CvPlot;
class CyArea;
class CyUnit;
class CyCity;

class CyPlot
{
public:
	CyPlot();
	DllExport explicit CyPlot(CvPlot*);			// Call from C++
	CyPlot(CvPlot*, bool bInViewportSpace);

	CvPlot* getPlot() const { return m_pPlot; }	// Call from C++
	void setPlot(CvPlot* p) { m_pPlot = p; }	// Call from C++
	bool isNone() const { return m_pPlot == NULL; }

	void erase();
	DllExport NiPoint3 getPoint() const;
	int getTeam() const;

	void nukeExplosion(int iRange, const CyUnit& kNukeUnit);

	bool isConnectedTo(const CyCity& kCity) const;
	bool isConnectedToCapital(int /*PlayerTypes*/ ePlayer) const;

	bool isAdjacentToArea(const CyArea& kArea) const;
	bool isCoastal() const;

	bool isLake() const;
	bool isFreshWater() const;

	bool isRiverSide() const;
	bool isRiver() const;

	CyPlot* getNearestLandPlot() const;
	bool canHaveBonus(int /*BonusTypes*/ eBonus, bool bIgnoreLatitude) const;
	bool canHaveImprovement(int /*ImprovementTypes*/ eImprovement, int /*TeamTypes*/ eTeam, bool bPotential) const;
	bool canBuild(int /*BuildTypes*/ eBuild, int /*PlayerTypes*/ ePlayer, bool bTestVisible) const;
	int getBuildTurnsLeft(int /*BuildTypes*/ eBuild, int iNowExtra, int iThenExtra) const;
	int getFeatureProduction(int /*BuildTypes*/ eBuild, int /*TeamTypes*/ eTeam, const CyCity* ppCity) const;

	int getUnitPower(int /*PlayerTypes*/ eOwner) const;

	int defenseModifier(int /*TeamTypes*/ iDefendTeam, bool bIgnoreBuilding, bool bHelp) const;

	bool isAdjacentPlayer(int /*PlayerTypes*/ ePlayer, bool bLandOnly) const;
	int /*PlayerTypes*/ calculateCulturalOwner() const;
	bool isOwned() const;
	bool isNPC() const;
	bool isHominid() const;
	bool isVisible(int /*TeamTypes*/ eTeam, bool bDebug) const;
	bool isActiveVisible(bool bDebug) const;
	bool isAdjacentRevealed(int /*TeamTypes*/ eTeam) const;
	bool isAdjacentNonrevealed(int /*TeamTypes*/ eTeam) const;
	bool isGoody() const;
	bool isCity() const;
	bool isOccupation() const;
	bool isUnit() const;
	int getNumDefenders(int /*PlayerTypes*/ ePlayer) const;
	int getNumVisiblePotentialEnemyDefenders(const CyUnit& kUnit) const;
	bool isVisibleEnemyUnit(int /*PlayerTypes*/ ePlayer) const;
	bool isFighting() const;

	bool canHaveFeature(int /*FeatureTypes*/ eFeature) const;
	bool isBonusNetwork(int /*TeamTypes*/ eTeam) const;

	bool isTradeNetworkImpassable(int /*TeamTypes*/ eTeam) const;
	bool isTradeNetwork(int /*TeamTypes*/ eTeam) const;
	bool isImpassable() const;

	int getX() const;
	int getY() const;
	int getLatitude() const;
	int getLongitude() const;
	CyArea* area() const;
	CyArea* waterArea() const;
	int getArea() const;
	int getFeatureVariety() const;

	int getOwnershipDuration() const;
	bool isOwnershipScore() const;
	void setOwnershipDuration(int iNewValue);

	int getUpgradeTimeLeft(int /*ImprovementTypes*/ eImprovement, int /*PlayerTypes*/ ePlayer) const;
	void changeImprovementUpgradeProgress(int iChange);

	bool isStartingPlot() const;
	void setStartingPlot(bool bNewValue);
	bool isNOfRiver() const;
	void setNOfRiver(bool bNewValue, CardinalDirectionTypes eRiverDir);
	bool isWOfRiver() const;
	void setWOfRiver(bool bNewValue, CardinalDirectionTypes eRiverDir);
	CardinalDirectionTypes getRiverWEDirection() const;
	CardinalDirectionTypes getRiverNSDirection() const;
	bool isIrrigated() const;

	bool isPotentialCityWork() const;

	int /*PlayerTypes*/ getOwner() const;
	void setOwner(int /*PlayerTypes*/ eNewValue);
	PlotTypes getPlotType() const;
	bool isWater() const;
	bool isFlatlands() const;
	bool isHills() const;
	bool isPeak() const;

	void setPlotType(PlotTypes eNewValue, bool bRecalculate, bool bRebuildGraphics);
	int /*TerrainTypes*/ getTerrainType() const;
	void setTerrainType(int /*TerrainTypes*/ eNewValue, bool bRecalculate, bool bRebuildGraphics);
	int /*FeatureTypes*/ getFeatureType() const;
	void setFeatureType(int /*FeatureTypes*/ eNewValue, int iVariety);
	int /*BonusTypes*/ getBonusType(int /*TeamTypes*/ eTeam) const;
	void setBonusType(int /*BonusTypes*/ eNewValue);
	int /*ImprovementTypes*/ getImprovementType() const;
	void setImprovementType(int /*ImprovementTypes*/ eNewValue);
	int /*RouteTypes*/ getRouteType() const;
	void setRouteType(int /*RouteTypes*/ eNewValue);

	CyCity* getPlotCity() const;
	CyCity* getWorkingCity() const;
	int getRiverID() const;
	void setRiverID(int iNewValue);
	int getYield(YieldTypes eIndex) const;
	int calculateBestNatureYield(YieldTypes eIndex, TeamTypes eTeam) const;
	int calculateTotalBestNatureYield(TeamTypes eTeam) const;
	int calculateImprovementYieldChange(int /*ImprovementTypes*/ eImprovement, YieldTypes eYield, int /*PlayerTypes*/ ePlayer, bool bOptimal) const;
	bool hasYield() const;

	int getCulture(int /*PlayerTypes*/ eIndex) const;
	int countTotalCulture() const;

	int calculateCulturePercent(int /*PlayerTypes*/ eIndex) const;
	void setCulture(int /*PlayerTypes*/ eIndex, int iNewValue, bool bUpdate);
	void changeCulture(int /*PlayerTypes*/ eIndex, int iChange, bool bUpdate);

	int getFoundValue(int /*PlayerTypes*/ eIndex) const;

	bool isPlayerCityRadius(int /*PlayerTypes*/ eIndex) const;

	int /*PlayerTypes*/ getRevealedOwner(int /*TeamTypes*/ eTeam, bool bDebug) const;

	bool isRiverCrossing(DirectionTypes eIndex) const;

	bool isRevealed(int /*TeamTypes*/ eTeam, bool bDebug) const;
	void setRevealed(int /*TeamTypes*/ eTeam, bool bNewValue, bool bTerrainOnly, int /*TeamTypes*/ eFromTeam);

	int getInvisibleVisibilityCount(int /*TeamTypes*/ eTeam, int /*InvisibleTypes*/ eInvisible) const;
	bool isSpotterInSight(int /*TeamTypes*/ eTeam, int /*InvisibleTypes*/ eInvisible) const;
	void changeInvisibleVisibilityCount(int iTeam, int iInvisible, int iChange);

	python::list units() const;
	int getNumUnits() const;
	CyUnit* getUnit(int iIndex) const;

	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);

	int getRevoltProtection() const;

	//	Multi-maps
	bool isInViewport() const;
	CyPlot* cloneToViewport() const;

	python::list adjacent() const;
	python::list rect(int halfWid, int halfHgt) const;

private:
	CvPlot* m_pPlot;
	bool	m_bIsInViewportSpace;
};

DECLARE_PY_WRAPPER(CyPlot, CvPlot*);

#endif // CyPlot_h__