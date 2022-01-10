#pragma once

#ifndef CvArea_h__
#define CvArea_h__

//#include "CvEnums.h"

class CvCity;
class CvPlot;

// Koshling - record of a single combat result
typedef struct CombatResultRecord
{
	PlayerTypes	eLoser;
	UnitTypes	eDefeatedUnitType;
	UnitTypes	eVictoriousEnemyUnitType;
} CombatResultRecord;

typedef std::vector<CombatResultRecord> TurnCombatResults;

// Rolling record of this number of turns combat are kept for AI analysis
#define COMBAT_RECORD_LENGTH 20

class CvArea
	: private bst::noncopyable // disable copy: we have owned pointers so we can't use the default copy implementation
{
//
// Functions
//
public:

	CvArea();
	virtual ~CvArea();

	void init(int iID, bool bWater);

	void uninit();
	void reset(int iID = 0, bool bWater = false, bool bConstructorCall = false);

	void clearModifierTotals();

	// for serialization
	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);

	int calculateTotalBestNatureYield() const;

	int countCoastalLand() const;
	int countNumUniqueBonusTypes() const;
	int countHasReligion(ReligionTypes eReligion, PlayerTypes eOwner = NO_PLAYER) const;
	int countHasCorporation(CorporationTypes eCorporation, PlayerTypes eOwner = NO_PLAYER) const;

	int getID() const;
	void setID(int iID);

	int getNumTiles() const;
	void changeNumTiles(int iChange);

	int getNumOwnedTiles() const;
	int getNumUnownedTiles() const;
	void changeNumOwnedTiles(int iChange);

	bool isLake() const;

	int getNumRiverEdges() const;
	void changeNumRiverEdges(int iChange);

	int getNumCities() const;
	int getNumUnits() const;
	int getTotalPopulation() const;

	int getNumStartingPlots() const;
	void changeNumStartingPlots(int iChange);

	bool isWater() const;

	int getUnitsPerPlayer(PlayerTypes eIndex) const;
	void changeUnitsPerPlayer(PlayerTypes eIndex, int iChange);

	int getAnimalsPerPlayer(PlayerTypes eIndex) const;
	void changeAnimalsPerPlayer(PlayerTypes eIndex, int iChange);

	int getCitiesPerPlayer(PlayerTypes eIndex) const;
	void changeCitiesPerPlayer(PlayerTypes eIndex, int iChange);

	int getPopulationPerPlayer(PlayerTypes eIndex) const;
	void changePopulationPerPlayer(PlayerTypes eIndex, int iChange);

	int getBuildingGoodHealth(PlayerTypes eIndex) const;
	void changeBuildingGoodHealth(PlayerTypes eIndex, int iChange);

	int getBuildingBadHealth(PlayerTypes eIndex) const;
	void changeBuildingBadHealth(PlayerTypes eIndex, int iChange);

	int getBuildingHappiness(PlayerTypes eIndex) const;
	void changeBuildingHappiness(PlayerTypes eIndex, int iChange);

	int getFreeSpecialist(PlayerTypes eIndex) const;
	void changeFreeSpecialist(PlayerTypes eIndex, int iChange);

	int getPower(PlayerTypes eIndex) const;
	void changePower(PlayerTypes eIndex, int iChange);

	bool hasBestFoundValue(PlayerTypes eIndex) const;
	int getBestFoundValue(PlayerTypes eIndex) const;
	void setBestFoundValue(PlayerTypes eIndex, int iNewValue);

	int getMaintenanceModifier(PlayerTypes eIndex) const;
	void changeMaintenanceModifier(PlayerTypes eIndex, int iChange);

	int getHomeAreaMaintenanceModifier(PlayerTypes eIndex) const;
	void changeHomeAreaMaintenanceModifier(PlayerTypes eIndex, int iChange);
	void setHomeAreaMaintenanceModifier(PlayerTypes eIndex, int iNewValue);

	int getOtherAreaMaintenanceModifier(PlayerTypes eIndex) const;
	void changeOtherAreaMaintenanceModifier(PlayerTypes eIndex, int iChange);
	void setOtherAreaMaintenanceModifier(PlayerTypes eIndex, int iNewValue);

	int getTotalAreaMaintenanceModifier(PlayerTypes ePlayer) const;

	bool isHomeArea(PlayerTypes eIndex) const;
	void setHomeArea(PlayerTypes ePlayer, CvArea* pOldHomeArea);

	int getNumRevealedTiles(TeamTypes eIndex) const;
	int getNumUnrevealedTiles(TeamTypes eIndex) const;
	void changeNumRevealedTiles(TeamTypes eIndex, int iChange);
	int getNumRevealedFeatureTiles(TeamTypes eTeam, FeatureTypes eFeature) const;
	int getNumRevealedTerrainTiles(TeamTypes eTeam, TerrainTypes eTerrain) const;

	bool isCleanPower(TeamTypes eIndex) const;
	void changeCleanPowerCount(TeamTypes eIndex, int iChange);

	int getBorderObstacleCount(TeamTypes eIndex) const;
	bool isBorderObstacle(TeamTypes eIndex) const;
	void changeBorderObstacleCount(TeamTypes eIndex, int iChange);

	AreaAITypes getAreaAIType(TeamTypes eIndex) const;
	void setAreaAIType(TeamTypes eIndex, AreaAITypes eNewValue);

	CvCity* getTargetCity(PlayerTypes eIndex) const;
	void setTargetCity(PlayerTypes eIndex, const CvCity* pNewValue);

	int getYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2) const;
	void changeYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getNumTrainAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2) const;
	void changeNumTrainAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2, int iChange);

	int getNumAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2) const;
	void changeNumAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2, int iChange);

	int getNumBonuses(BonusTypes eBonus) const;
	int getNumTotalBonuses() const;
	void changeNumBonuses(BonusTypes eBonus, int iChange);

	int getNumImprovements(ImprovementTypes eImprovement) const;
	void changeNumImprovements(ImprovementTypes eImprovement, int iChange);

	void setNumValidPlotsbySpawn(SpawnTypes eSpawn, int iAmount);
	int getNumValidPlotsbySpawn(SpawnTypes eSpawn) const;

	//	Koshling - record rolling history of the last N turns of our combat losses and what we lost to
	void recordCombatDeath(PlayerTypes ePlayer, UnitTypes lostUnitType, UnitTypes lostToUnitType);
	//	Return the number of units of the specified type recently lost per 100 turns (normalised figure)
	//	If eUnit is NO_UNIT all types will be tallied
	int	getRecentCombatDeathRate(PlayerTypes ePlayer, UnitTypes eUnit) const;
	//	Return the number of units of the specified AI type recently lost per 100 turns (normalised figure)
	//	If eUnit is NO_UNITAI all types will be tallied
	int	getRecentCombatDeathRate(PlayerTypes ePlayer, UnitAITypes eUnitAIType) const;

//
// Variables
//
protected:

	int m_iID;
	int m_iNumTiles;
	int m_iNumOwnedTiles;
	int m_iNumRiverEdges;
	int m_iNumUnits;
	int m_iNumCities;
	int m_iTotalPopulation;
	int m_iNumStartingPlots;

	bool m_bWater;

	int* m_aiUnitsPerPlayer;
	int* m_aiAnimalsPerPlayer;
	int* m_aiCitiesPerPlayer;
	int* m_aiPopulationPerPlayer;
	int* m_aiBuildingGoodHealth;
	int* m_aiBuildingBadHealth;
	int* m_aiBuildingHappiness;
	int* m_aiFreeSpecialist;
	int* m_aiPower;
	int* m_aiBestFoundValue;
	int* m_aiMaintenanceModifier;
	int* m_aiHomeAreaMaintenanceModifier;
	int* m_aiOtherAreaMaintenanceModifier;
	int* m_aiNumRevealedTiles;
	int* m_aiCleanPowerCount;
	int* m_aiBorderObstacleCount;
	int* m_aiSpawnValidPlotCount;
	int* m_paiNumBonuses;
	int* m_paiNumImprovements;

	bool* m_abHomeArea;

	AreaAITypes* m_aeAreaAIType;

	IDInfo* m_aTargetCities;

	int** m_aaiYieldRateModifier;
	int** m_aaiNumTrainAIUnits;
	int** m_aaiNumAIUnits;

	mutable TeamTypes m_eCachedTeamPlotTypeCounts;
	mutable int m_iCachedTurnPlotTypeCounts;

	mutable std::map<FeatureTypes,int> m_plotFeatureCountCache;
	mutable std::map<TerrainTypes,int> m_plotTerrainCountCache;

private:
	int m_iLastGameTurnRecorded;
	TurnCombatResults m_combatRecord[COMBAT_RECORD_LENGTH];

//
// Algorithm/range helpers
//
public:
	struct fn {
		DECLARE_MAP_FUNCTOR(CvArea, void, clearModifierTotals);

		DECLARE_MAP_FUNCTOR_2(CvArea, void, setBestFoundValue, PlayerTypes, int);

		DECLARE_MAP_FUNCTOR_CONST(CvArea, bool, isWater);
		DECLARE_MAP_FUNCTOR_CONST(CvArea, int, getNumTiles);

		DECLARE_MAP_FUNCTOR_CONST_1(CvArea, int, getBestFoundValue, PlayerTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvArea, int, getCitiesPerPlayer, PlayerTypes);
	};
};

#endif // CvArea_h__
