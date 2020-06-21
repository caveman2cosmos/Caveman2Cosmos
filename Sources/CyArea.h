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
	CvArea* getArea() const { return m_pArea; }	// Call from C++
	bool isNone() const { return m_pArea == NULL; }

	int calculateTotalBestNatureYield() const;
	int countCoastalLand() const;
	int countNumUniqueBonusTypes() const;
	int countHasReligion(int /*ReligionTypes*/ eReligion, int /*PlayerTypes*/ eOwner) const;
	int countHasCorporation(int /*CorporationTypes*/ eCorporation, int /*PlayerTypes*/ eOwner) const;
	int getID() const;
	int getNumTiles() const;
	bool isLake() const;
	int getNumOwnedTiles() const;
	int getNumUnownedTiles() const;
	int getNumRiverEdges() const;
	int getNumCities() const;
	int getNumUnits() const;
	int getTotalPopulation() const;
	int getNumStartingPlots() const;
	bool isWater() const;

	int getUnitsPerPlayer(int /*PlayerTypes*/ eIndex) const;
	int getAnimalsPerPlayer(int /*PlayerTypes*/ eIndex) const;
	int getCitiesPerPlayer(int /*PlayerTypes*/ eIndex) const;
	int getPopulationPerPlayer(int /*PlayerTypes*/ eIndex) const;
	int getBuildingGoodHealth(int /*PlayerTypes*/ eIndex) const;
	int getBuildingBadHealth(int /*PlayerTypes*/ eIndex) const;
	int getBuildingHappiness(int /*PlayerTypes*/ eIndex) const;
	int getFreeSpecialist(int /*PlayerTypes*/ eIndex) const;
	int getPower(int /*PlayerTypes*/ eIndex) const;
	int getBestFoundValue(int /*PlayerTypes*/ eIndex) const;

	int getNumRevealedTiles(int /*TeamTypes*/ eIndex) const;
	int getNumUnrevealedTiles(int /*TeamTypes*/ eIndex) const;

	bool isCleanPower(int /*TeamTypes*/ eIndex) const;
	bool isBorderObstacle(int /*TeamTypes*/ eIndex) const;

	int /*AreaAITypes*/ getAreaAIType(int /*TeamTypes*/ eIndex) const;
	CyCity* getTargetCity(int /*PlayerTypes*/ eIndex) const;
	int getYieldRateModifier(int /*PlayerTypes*/ eIndex1, int /*YieldTypes*/ eIndex2) const;
	int getNumTrainAIUnits(int /*PlayerTypes*/ eIndex1, int /*UnitAITypes*/ eIndex2) const;
	int getNumAIUnits(int /*PlayerTypes*/ eIndex1, int /*UnitAITypes*/ eIndex2) const;

	int getNumBonuses(int /*BonusTypes*/ eBonus) const;
	int getNumTotalBonuses() const;
	int getNumImprovements(int /*ImprovementTypes*/ eImprovement) const;
/************************************************************************************************/
/* Afforess	                  Start		 07/15/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	void changeCleanPowerCount(int /*TeamTypes*/ eIndex, int iChange) const;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
protected:

	CvArea* m_pArea;
};

#endif	// #ifndef CyArea
