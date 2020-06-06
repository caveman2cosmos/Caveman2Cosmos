#pragma once

#ifndef CyArea_h
#define CyArea_h

//
// Python wrapper class for CvArea
//

class CyCity;
class CvArea;

class CyArea 
{
public:
	CyArea();
	explicit CyArea(CvArea* pArea);					// Call from C++

	CvArea* getArea() { return m_pArea; }	// Call from C++
	const CvArea* getArea() const { return m_pArea; }	// Call from C++
	bool isNone() { return m_pArea == NULL; }

	int calculateTotalBestNatureYield() const;
	int countCoastalLand() const;
	int countNumUniqueBonusTypes() const;
	int getID() const;
	int getNumTiles() const;
	bool isLake() const;
	int getNumRiverEdges() const;
	int getNumCities() const;
	int getNumUnits() const;
	int getTotalPopulation() const;
	int getNumStartingPlots() const;
	bool isWater() const;

	int getUnitsPerPlayer(int /*PlayerTypes*/ eIndex) const;
	int getCitiesPerPlayer(int /*PlayerTypes*/ eIndex) const;
	int getBuildingHappiness(int /*PlayerTypes*/ eIndex) const;
	int getPower(int /*PlayerTypes*/ eIndex) const;
	int getBestFoundValue(int /*PlayerTypes*/ eIndex) const;

	bool isCleanPower(int /*TeamTypes*/ eIndex) const;
	bool isBorderObstacle(int /*TeamTypes*/ eIndex) const;

	int getYieldRateModifier(int /*PlayerTypes*/ eIndex1, int /*YieldTypes*/ eIndex2) const;

	int getNumBonuses(int /*BonusTypes*/ eBonus) const;
	int getNumTotalBonuses() const;

	void changeCleanPowerCount(int /*TeamTypes*/ eIndex, int iChange) const;

protected:
	CvArea* m_pArea;
};

#endif	// #ifndef CyArea
