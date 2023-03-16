// area.cpp


#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvArea.h"
#include "CvBonusInfo.h"
#include "CvCity.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvMap.h"
#include "CvInfos.h"
#include "CvPlayerAI.h"
#include "CvPlot.h"
#include "CvTeamAI.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLUtilityIFaceBase.h"

// Public Functions...

CvArea::CvArea()
{
	PROFILE_EXTRA_FUNC();
	m_aiUnitsPerPlayer = new int[MAX_PLAYERS];
	m_aiAnimalsPerPlayer = new int[MAX_PLAYERS];
	m_aiCitiesPerPlayer = new int[MAX_PLAYERS];
	m_aiPopulationPerPlayer = new int[MAX_PLAYERS];
	m_aiBuildingGoodHealth = new int[MAX_PLAYERS];
	m_aiBuildingBadHealth = new int[MAX_PLAYERS];
	m_aiBuildingHappiness = new int[MAX_PLAYERS];
	m_aiFreeSpecialist = new int[MAX_PLAYERS];
	m_aiPower = new int[MAX_PLAYERS];
	m_aiBestFoundValue = new int[MAX_PLAYERS];
	m_aiMaintenanceModifier = new int[MAX_PLAYERS];
	m_abHomeArea = new bool[MAX_PLAYERS];

	m_aiNumRevealedTiles = new int[MAX_TEAMS];
	m_aiCleanPowerCount = new int[MAX_TEAMS];
	m_aiBorderObstacleCount = new int[MAX_TEAMS];

	m_aeAreaAIType = new AreaAITypes[MAX_TEAMS];

	m_aTargetCities = new IDInfo[MAX_PLAYERS];

	m_aaiYieldRateModifier = new int*[MAX_PLAYERS];
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		m_aaiYieldRateModifier[i] = new int[NUM_YIELD_TYPES];
	}
	m_aaiNumTrainAIUnits = new int*[MAX_PLAYERS];
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		m_aaiNumTrainAIUnits[i] = new int[NUM_UNITAI_TYPES];
	}
	m_aaiNumAIUnits = new int*[MAX_PLAYERS];
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		m_aaiNumAIUnits[i] = new int[NUM_UNITAI_TYPES];
	}

	m_paiNumBonuses = NULL;
	m_paiNumImprovements = NULL;
	m_aiSpawnValidPlotCount = NULL;

	reset(0, false, true);
}


CvArea::~CvArea()
{
	uninit();

	SAFE_DELETE_ARRAY(m_aiUnitsPerPlayer);
	SAFE_DELETE_ARRAY(m_aiAnimalsPerPlayer);
	SAFE_DELETE_ARRAY(m_aiCitiesPerPlayer);
	SAFE_DELETE_ARRAY(m_aiPopulationPerPlayer);
	SAFE_DELETE_ARRAY(m_aiBuildingGoodHealth);
	SAFE_DELETE_ARRAY(m_aiBuildingBadHealth);
	SAFE_DELETE_ARRAY(m_aiBuildingHappiness);
	SAFE_DELETE_ARRAY(m_aiFreeSpecialist);
	SAFE_DELETE_ARRAY(m_aiPower);
	SAFE_DELETE_ARRAY(m_aiBestFoundValue);
	SAFE_DELETE_ARRAY(m_aiMaintenanceModifier);
	SAFE_DELETE_ARRAY(m_abHomeArea);
	SAFE_DELETE_ARRAY(m_aiNumRevealedTiles);
	SAFE_DELETE_ARRAY(m_aiCleanPowerCount);
	SAFE_DELETE_ARRAY(m_aiBorderObstacleCount);
	SAFE_DELETE_ARRAY(m_aeAreaAIType);
	SAFE_DELETE_ARRAY(m_aTargetCities);
	SAFE_DELETE_ARRAY2(m_aaiYieldRateModifier, MAX_PLAYERS);
	SAFE_DELETE_ARRAY2(m_aaiNumTrainAIUnits, MAX_PLAYERS);
	SAFE_DELETE_ARRAY2(m_aaiNumAIUnits, MAX_PLAYERS);
}

void CvArea::init(int iID, bool bWater)
{
	reset(iID, bWater);
}


void CvArea::uninit()
{
	SAFE_DELETE_ARRAY(m_paiNumBonuses);
	SAFE_DELETE_ARRAY(m_paiNumImprovements);
	SAFE_DELETE_ARRAY(m_aiSpawnValidPlotCount);
}

// Initializes data members that are serialized.
void CvArea::reset(int iID, bool bWater, bool bConstructorCall)
{
	PROFILE_EXTRA_FUNC();
	uninit();

	m_iID = iID;
	m_bWater = bWater;

	m_iNumTiles = 0;
	m_iNumOwnedTiles = 0;
	m_iNumRiverEdges = 0;
	m_iNumUnits = 0;
	m_iNumCities = 0;
	m_iTotalPopulation = 0;
	m_iNumStartingPlots = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aiUnitsPerPlayer[iI] = 0;
		m_aiAnimalsPerPlayer[iI] = 0;
		m_aiCitiesPerPlayer[iI] = 0;
		m_aiPopulationPerPlayer[iI] = 0;
		m_aiBuildingGoodHealth[iI] = 0;
		m_aiBuildingBadHealth[iI] = 0;
		m_aiBuildingHappiness[iI] = 0;
		m_aiFreeSpecialist[iI] = 0;
		m_aiPower[iI] = 0;
		m_aiBestFoundValue[iI] = 0;
		m_aiMaintenanceModifier[iI] = 0;
		m_abHomeArea[iI] = 0;
		m_aTargetCities[iI].reset();

		for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			m_aaiYieldRateModifier[iI][iJ] = 0;
		}
		for (int iJ = 0; iJ < NUM_UNITAI_TYPES; iJ++)
		{
			m_aaiNumTrainAIUnits[iI][iJ] = 0;
			m_aaiNumAIUnits[iI][iJ] = 0;
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiNumRevealedTiles[iI] = 0;
		m_aiCleanPowerCount[iI] = 0;
		m_aiBorderObstacleCount[iI] = 0;
		m_aeAreaAIType[iI] = NO_AREAAI;
	}

	//	Empty the combat record
	m_iLastGameTurnRecorded = -1;
	for(int i = 0; i < COMBAT_RECORD_LENGTH; i++)
	{
		m_combatRecord[i].clear();
	}

	if (!bConstructorCall)
	{
		FAssertMsg(0 < GC.getNumBonusInfos(), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated in CvArea::reset");
		m_paiNumBonuses = new int[GC.getNumBonusInfos()];
		for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			m_paiNumBonuses[iI] = 0;
		}

		FAssertMsg((0 < GC.getNumImprovementInfos()), "GC.getNumImprovementInfos() is not greater than zero but an array is being allocated in CvArea::reset");
		m_paiNumImprovements = new int[GC.getNumImprovementInfos()];
		for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			m_paiNumImprovements[iI] = 0;
		}
	}

	m_eCachedTeamPlotTypeCounts = NO_TEAM;
	m_iCachedTurnPlotTypeCounts = -1;
}

void CvArea::clearModifierTotals()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aiBuildingGoodHealth[iI] = 0;
		m_aiBuildingBadHealth[iI] = 0;
		m_aiBuildingHappiness[iI] = 0;
		m_aiFreeSpecialist[iI] = 0;
		m_aiPower[iI] = 0;
		m_aiMaintenanceModifier[iI] = 0;

		for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			m_aaiYieldRateModifier[iI][iJ] = 0;
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiCleanPowerCount[iI] = 0;
	}
}


void CvArea::read(FDataStreamBase* pStream)
{
	PROFILE_EXTRA_FUNC();
	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	// Init saved data
	reset();

	WRAPPER_READ(wrapper, "CvArea", &m_iID);
	WRAPPER_READ(wrapper, "CvArea", &m_iNumTiles);
	WRAPPER_READ(wrapper, "CvArea", &m_iNumOwnedTiles);
	WRAPPER_READ(wrapper, "CvArea", &m_iNumRiverEdges);
	WRAPPER_READ(wrapper, "CvArea", &m_iNumUnits);
	WRAPPER_READ(wrapper, "CvArea", &m_iNumCities);
	WRAPPER_READ(wrapper, "CvArea", &m_iTotalPopulation);
	WRAPPER_READ(wrapper, "CvArea", &m_iNumStartingPlots);

	WRAPPER_READ(wrapper, "CvArea", &m_bWater);

	WRAPPER_READ_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiUnitsPerPlayer);
	WRAPPER_READ_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiAnimalsPerPlayer);
	WRAPPER_READ_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiCitiesPerPlayer);
	WRAPPER_READ_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiPopulationPerPlayer);
	WRAPPER_READ_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiBuildingGoodHealth);
	WRAPPER_READ_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiBuildingBadHealth);
	WRAPPER_READ_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiBuildingHappiness);
	WRAPPER_READ_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiFreeSpecialist);
	WRAPPER_READ_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiPower);
	WRAPPER_READ_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiBestFoundValue);
	WRAPPER_READ_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiMaintenanceModifier);
	WRAPPER_READ_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_abHomeArea);

	WRAPPER_READ_ARRAY(wrapper, "CvArea", MAX_TEAMS, m_aiNumRevealedTiles);
	WRAPPER_READ_ARRAY(wrapper, "CvArea", MAX_TEAMS, m_aiCleanPowerCount);
	WRAPPER_READ_ARRAY(wrapper, "CvArea", MAX_TEAMS, m_aiBorderObstacleCount);

	WRAPPER_READ_ARRAY(wrapper, "CvArea", MAX_TEAMS, (int*)m_aeAreaAIType);

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		WRAPPER_READ(wrapper, "CvArea", (int*)&m_aTargetCities[iI].eOwner);
		WRAPPER_READ(wrapper, "CvArea", &m_aTargetCities[iI].iID);
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		WRAPPER_READ_ARRAY(wrapper, "CvArea", NUM_YIELD_TYPES, m_aaiYieldRateModifier[iI]);
	}
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		WRAPPER_READ_OPTIONAL_CLASS_ARRAY(wrapper, "CvArea", REMAPPED_CLASS_TYPE_UNITAIS, NUM_UNITAI_TYPES, m_aaiNumTrainAIUnits[iI]);
	}
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		WRAPPER_READ_OPTIONAL_CLASS_ARRAY(wrapper, "CvArea", REMAPPED_CLASS_TYPE_UNITAIS, NUM_UNITAI_TYPES, m_aaiNumAIUnits[iI]);
	}

	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvArea", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNumBonuses);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvArea", REMAPPED_CLASS_TYPE_IMPROVEMENTS, GC.getNumImprovementInfos(), m_paiNumImprovements);

	WRAPPER_READ(wrapper, "CvArea", &m_iLastGameTurnRecorded); FASSERT_BOUNDS(-1, (GC.getGame().getGameTurn() + 1), m_iLastGameTurnRecorded);

	for (int iI = 0; iI < COMBAT_RECORD_LENGTH; iI++)
	{
		TurnCombatResults& turnRecord = m_combatRecord[iI];
		int numRecords = 0;

		WRAPPER_READ(wrapper, "CvArea", &numRecords);

		for (int iJ = 0; iJ < numRecords; iJ++)
		{
			CombatResultRecord record;

			WRAPPER_READ(wrapper, "CvArea", (int*)&record.eLoser);
			WRAPPER_READ_CLASS_ENUM(wrapper, "CvArea", REMAPPED_CLASS_TYPE_UNITS, (int*)&record.eDefeatedUnitType);
			WRAPPER_READ_CLASS_ENUM(wrapper, "CvArea", REMAPPED_CLASS_TYPE_UNITS, (int*)&record.eVictoriousEnemyUnitType);

			turnRecord.push_back(record);
		}
	}

	WRAPPER_READ_OBJECT_END(wrapper);
}

void CvArea::write(FDataStreamBase* pStream)
{
	PROFILE_EXTRA_FUNC();
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE(wrapper, "CvArea", m_iID);
	WRAPPER_WRITE(wrapper, "CvArea", m_iNumTiles);
	WRAPPER_WRITE(wrapper, "CvArea", m_iNumOwnedTiles);
	WRAPPER_WRITE(wrapper, "CvArea", m_iNumRiverEdges);
	WRAPPER_WRITE(wrapper, "CvArea", m_iNumUnits);
	WRAPPER_WRITE(wrapper, "CvArea", m_iNumCities);
	WRAPPER_WRITE(wrapper, "CvArea", m_iTotalPopulation);
	WRAPPER_WRITE(wrapper, "CvArea", m_iNumStartingPlots);

	WRAPPER_WRITE(wrapper, "CvArea", m_bWater);

	WRAPPER_WRITE_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiUnitsPerPlayer);
	WRAPPER_WRITE_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiAnimalsPerPlayer);
	WRAPPER_WRITE_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiCitiesPerPlayer);
	WRAPPER_WRITE_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiPopulationPerPlayer);
	WRAPPER_WRITE_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiBuildingGoodHealth);
	WRAPPER_WRITE_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiBuildingBadHealth);
	WRAPPER_WRITE_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiBuildingHappiness);
	WRAPPER_WRITE_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiFreeSpecialist);
	WRAPPER_WRITE_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiPower);
	WRAPPER_WRITE_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiBestFoundValue);
	WRAPPER_WRITE_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_aiMaintenanceModifier);
	WRAPPER_WRITE_ARRAY(wrapper, "CvArea", MAX_PLAYERS, m_abHomeArea);

	WRAPPER_WRITE_ARRAY(wrapper, "CvArea", MAX_TEAMS, m_aiNumRevealedTiles);
	WRAPPER_WRITE_ARRAY(wrapper, "CvArea", MAX_TEAMS, m_aiCleanPowerCount);
	WRAPPER_WRITE_ARRAY(wrapper, "CvArea", MAX_TEAMS, m_aiBorderObstacleCount);

	WRAPPER_WRITE_ARRAY(wrapper, "CvArea", MAX_TEAMS, (int*)m_aeAreaAIType);

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		WRAPPER_WRITE(wrapper, "CvArea", m_aTargetCities[iI].eOwner);
		WRAPPER_WRITE(wrapper, "CvArea", m_aTargetCities[iI].iID);
	}

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		WRAPPER_WRITE_ARRAY(wrapper, "CvArea", NUM_YIELD_TYPES, m_aaiYieldRateModifier[iI]);
	}
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvArea", REMAPPED_CLASS_TYPE_UNITAIS, NUM_UNITAI_TYPES, m_aaiNumTrainAIUnits[iI]);
	}
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvArea", REMAPPED_CLASS_TYPE_UNITAIS, NUM_UNITAI_TYPES, m_aaiNumAIUnits[iI]);
	}

	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvArea", REMAPPED_CLASS_TYPE_BONUSES, GC.getNumBonusInfos(), m_paiNumBonuses);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvArea", REMAPPED_CLASS_TYPE_IMPROVEMENTS, GC.getNumImprovementInfos(), m_paiNumImprovements);

	WRAPPER_WRITE(wrapper, "CvArea", m_iLastGameTurnRecorded);

	for (int iI = 0; iI < COMBAT_RECORD_LENGTH; iI++)
	{
		TurnCombatResults& turnRecord = m_combatRecord[iI];
		int numRecords = turnRecord.size();

		WRAPPER_WRITE(wrapper, "CvArea", numRecords);

		for (int iJ = 0; iJ < numRecords; iJ++)
		{
			CombatResultRecord& record = turnRecord[iJ];

			WRAPPER_WRITE(wrapper, "CvArea", record.eLoser);
			WRAPPER_WRITE_CLASS_ENUM(wrapper, "CvArea", REMAPPED_CLASS_TYPE_UNITS, record.eDefeatedUnitType);
			WRAPPER_WRITE_CLASS_ENUM(wrapper, "CvArea", REMAPPED_CLASS_TYPE_UNITS, record.eVictoriousEnemyUnitType);
		}
	}

	WRAPPER_WRITE_OBJECT_END(wrapper);
}


int CvArea::getID() const
{
	return m_iID;
}

void CvArea::setID(int iID)
{
	m_iID = iID;
}


int CvArea::calculateTotalBestNatureYield() const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		const CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);
		if (pLoopPlot->getArea() == getID())
		{
			iCount += pLoopPlot->calculateTotalBestNatureYield(NO_TEAM);
		}
	}
	return iCount;
}


int CvArea::countCoastalLand() const
{
	PROFILE_EXTRA_FUNC();
	if (isWater())
	{
		return 0;
	}
	int iCount = 0;

	for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
	{
		const CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);
		if (pLoopPlot->getArea() == getID() && pLoopPlot->isCoastalLand())
		{
			iCount++;
		}
	}
	return iCount;
}


int CvArea::countNumUniqueBonusTypes() const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = GC.getNumMapBonuses() - 1; iI > -1; iI--)
	{
		if (GC.getBonusInfo(GC.getMapBonus(iI)).isOneArea() && getNumBonuses(GC.getMapBonus(iI)) > 0)
		{
			iCount++;
		}
	}
	return iCount;
}


int CvArea::countHasReligion(ReligionTypes eReligion, PlayerTypes eOwner) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && (eOwner == NO_PLAYER || iI == eOwner))
		{
			foreach_(const CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
			{
				if (pLoopCity->area()->getID() == getID() && pLoopCity->isHasReligion(eReligion))
				{
					iCount++;
				}
			}
		}
	}
	return iCount;
}

int CvArea::countHasCorporation(CorporationTypes eCorporation, PlayerTypes eOwner) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && (eOwner == NO_PLAYER || iI == eOwner))
		{
			foreach_(const CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
			{
				if (pLoopCity->area()->getID() == getID() && pLoopCity->isHasCorporation(eCorporation))
				{
					++iCount;
				}
			}
		}
	}
	return iCount;
}

int CvArea::getNumTiles() const
{
	return m_iNumTiles;
}


bool CvArea::isLake() const
{
	return isWater() && getNumTiles() < GC.getWorldInfo(GC.getMap().getWorldSize()).getOceanMinAreaSize();
}


void CvArea::changeNumTiles(int iChange)
{
	if (iChange != 0)
	{
		m_iNumTiles += iChange;
		FASSERT_NOT_NEGATIVE(getNumTiles());
	}
}


int CvArea::getNumOwnedTiles() const
{
	return m_iNumOwnedTiles;
}


int CvArea::getNumUnownedTiles() const
{
	return getNumTiles() - getNumOwnedTiles();
}


void CvArea::changeNumOwnedTiles(int iChange)
{
	m_iNumOwnedTiles += iChange;
	FASSERT_NOT_NEGATIVE(getNumOwnedTiles());
	FASSERT_NOT_NEGATIVE(getNumUnownedTiles());
}


int CvArea::getNumRiverEdges() const
{
	return m_iNumRiverEdges;
}


void CvArea::changeNumRiverEdges(int iChange)
{
	m_iNumRiverEdges += iChange;
	FASSERT_NOT_NEGATIVE(getNumRiverEdges());
}


int CvArea::getNumUnits() const
{
	return m_iNumUnits;
}


int CvArea::getNumCities() const
{
	return m_iNumCities;
}


int CvArea::getTotalPopulation() const
{
	return m_iTotalPopulation;
}


int CvArea::getNumStartingPlots() const
{
	return m_iNumStartingPlots;
}


void CvArea::changeNumStartingPlots(int iChange)
{
	m_iNumStartingPlots += iChange;
	FASSERT_NOT_NEGATIVE(getNumStartingPlots());
}


bool CvArea::isWater() const
{
	return m_bWater;
}


int CvArea::getUnitsPerPlayer(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	return m_aiUnitsPerPlayer[eIndex];
}


void CvArea::changeUnitsPerPlayer(PlayerTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	m_iNumUnits += iChange;
	FASSERT_NOT_NEGATIVE(getNumUnits());
	m_aiUnitsPerPlayer[eIndex] += iChange;
	FASSERT_NOT_NEGATIVE(getUnitsPerPlayer(eIndex));
}


int CvArea::getAnimalsPerPlayer(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	return m_aiAnimalsPerPlayer[eIndex];
}


void CvArea::changeAnimalsPerPlayer(PlayerTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	m_aiAnimalsPerPlayer[eIndex] += iChange;
	FASSERT_NOT_NEGATIVE(getAnimalsPerPlayer(eIndex));
}


int CvArea::getCitiesPerPlayer(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	return m_aiCitiesPerPlayer[eIndex];
}


void CvArea::changeCitiesPerPlayer(PlayerTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	m_iNumCities += iChange;
	FASSERT_NOT_NEGATIVE(getNumCities());
	m_aiCitiesPerPlayer[eIndex] += iChange;
	FASSERT_NOT_NEGATIVE(getCitiesPerPlayer(eIndex));
}


int CvArea::getPopulationPerPlayer(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	return m_aiPopulationPerPlayer[eIndex];
}


void CvArea::changePopulationPerPlayer(PlayerTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	m_iTotalPopulation += iChange;
	FASSERT_NOT_NEGATIVE(getTotalPopulation());
	m_aiPopulationPerPlayer[eIndex] += iChange;
	FASSERT_NOT_NEGATIVE(getPopulationPerPlayer(eIndex));
	changePower(eIndex, iChange);
}


int CvArea::getBuildingGoodHealth(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	return m_aiBuildingGoodHealth[eIndex];
}


void CvArea::changeBuildingGoodHealth(PlayerTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);

	if (iChange != 0)
	{
		m_aiBuildingGoodHealth[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(getBuildingGoodHealth(eIndex));

		GET_PLAYER(eIndex).AI_makeAssignWorkDirty();
	}
}


int CvArea::getBuildingBadHealth(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	return m_aiBuildingBadHealth[eIndex];
}


void CvArea::changeBuildingBadHealth(PlayerTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);

	if (iChange != 0)
	{
		m_aiBuildingBadHealth[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(getBuildingBadHealth(eIndex));

		GET_PLAYER(eIndex).AI_makeAssignWorkDirty();
	}
}


int CvArea::getBuildingHappiness(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	return m_aiBuildingHappiness[eIndex];
}


void CvArea::changeBuildingHappiness(PlayerTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);

	if (iChange != 0)
	{
		m_aiBuildingHappiness[eIndex] += iChange;

		GET_PLAYER(eIndex).AI_makeAssignWorkDirty();
	}
}


int CvArea::getFreeSpecialist(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	return m_aiFreeSpecialist[eIndex];
}


void CvArea::changeFreeSpecialist(PlayerTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);

	if (iChange != 0)
	{
		m_aiFreeSpecialist[eIndex] += iChange;
		GET_PLAYER(eIndex).AI_makeAssignWorkDirty();
	}
}


int CvArea::getPower(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	return std::max(0, m_aiPower[eIndex]);
}


void CvArea::changePower(PlayerTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	m_aiPower[eIndex] += iChange;
	FASSERT_NOT_NEGATIVE(m_aiPower[eIndex]);
}

bool CvArea::hasBestFoundValue(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	return m_aiBestFoundValue[eIndex] != -1;
}

int CvArea::getBestFoundValue(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	int iResult = m_aiBestFoundValue[eIndex];

	// Calculate on demand
	if (iResult == -1)
	{
		GET_PLAYER(eIndex).AI_updateFoundValues(false, const_cast<CvArea*>(this));
		iResult = m_aiBestFoundValue[eIndex];

		if (iResult == -1)
		{
			return 0; // No valid found spots in this area
		}
	}
	return iResult;
}


void CvArea::setBestFoundValue(PlayerTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	FAssert(iNewValue >= -1);
	m_aiBestFoundValue[eIndex] = iNewValue;
}


int CvArea::getMaintenanceModifier(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	return m_aiMaintenanceModifier[eIndex];
}

void CvArea::changeMaintenanceModifier(PlayerTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	if (iChange != 0)
	{
		m_aiMaintenanceModifier[eIndex] += iChange;
		GET_PLAYER(eIndex).setMaintenanceDirty(true);
	}
}


bool CvArea::isHomeArea(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	return m_abHomeArea[eIndex];
}

/* DPII: Any new area modifiers for things like yield, happiness or health have to have the modifiers switched here
 in the setHomeArea code.  The HomeArea modifiers for the new home area needs to be set and the HomeArea modifiers for the
 old home area needs to be cleared.  The process is the same for setting and clearing the OtherArea modifiers but in reverse.

 If you've done this correctly, no Area should have both a HomeArea and an OtherArea modifier value.*/
void CvArea::setHomeArea(PlayerTypes ePlayer, CvArea* pOldHomeArea)
{
	if (pOldHomeArea != this)
	{
		if (pOldHomeArea)
		{
			pOldHomeArea->m_abHomeArea[ePlayer] = false;
		}
		m_abHomeArea[ePlayer] = true;
	}
}

int CvArea::getTotalAreaMaintenanceModifier(PlayerTypes ePlayer) const
{
	if (isHomeArea(ePlayer))
	{
		return getMaintenanceModifier(ePlayer) + GET_PLAYER(ePlayer).getHomeAreaMaintenanceModifier();
	}
	return getMaintenanceModifier(ePlayer) + GET_PLAYER(ePlayer).getOtherAreaMaintenanceModifier();
}


int CvArea::getNumRevealedTiles(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_aiNumRevealedTiles[eIndex];
}


int CvArea::getNumUnrevealedTiles(TeamTypes eIndex) const
{
	return getNumTiles() - getNumRevealedTiles(eIndex);
}

int CvArea::getNumRevealedFeatureTiles(TeamTypes eTeam, FeatureTypes eFeature) const
{
	PROFILE_EXTRA_FUNC();
	if (m_iCachedTurnPlotTypeCounts != GC.getGame().getGameTurn() ||
		m_eCachedTeamPlotTypeCounts != eTeam)
	{
		m_plotFeatureCountCache.clear();
		m_plotTerrainCountCache.clear();
	}

	m_iCachedTurnPlotTypeCounts = GC.getGame().getGameTurn();
	m_eCachedTeamPlotTypeCounts = eTeam;

	std::map<FeatureTypes, int>::const_iterator	itr = m_plotFeatureCountCache.find(eFeature);
	if (itr == m_plotFeatureCountCache.end())
	{
		int	iResult = 0;

		for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
		{
			const CvPlot* pPlot = GC.getMap().plotByIndex(iI);
			if (pPlot != NULL &&
				pPlot->area() == this &&
				pPlot->isRevealed(eTeam, false) &&
				pPlot->getFeatureType() == eFeature)
			{
				iResult++;
			}
		}

		m_plotFeatureCountCache.insert(std::make_pair(eFeature, iResult));

		return iResult;
	}
	return itr->second;
}

int CvArea::getNumRevealedTerrainTiles(TeamTypes eTeam, TerrainTypes eTerrain) const
{
	PROFILE_EXTRA_FUNC();
	if (m_iCachedTurnPlotTypeCounts != GC.getGame().getGameTurn() ||
		m_eCachedTeamPlotTypeCounts != eTeam)
	{
		m_plotFeatureCountCache.clear();
		m_plotTerrainCountCache.clear();
	}

	m_iCachedTurnPlotTypeCounts = GC.getGame().getGameTurn();
	m_eCachedTeamPlotTypeCounts = eTeam;

	std::map<TerrainTypes, int>::const_iterator	itr = m_plotTerrainCountCache.find(eTerrain);
	if (itr == m_plotTerrainCountCache.end())
	{
		int	iResult = 0;

		for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
		{
			const CvPlot* pPlot = GC.getMap().plotByIndex(iI);
			if (pPlot != NULL &&
				pPlot->area() == this &&
				pPlot->isRevealed(eTeam, false) &&
				pPlot->getTerrainType() == eTerrain)
			{
				iResult++;
			}
		}

		m_plotTerrainCountCache.insert(std::make_pair(eTerrain, iResult));

		return iResult;
	}
	return itr->second;
}

void CvArea::changeNumRevealedTiles(TeamTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	m_aiNumRevealedTiles[eIndex] += iChange;
	FASSERT_NOT_NEGATIVE(getNumRevealedTiles(eIndex));
}


bool CvArea::isCleanPower(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_aiCleanPowerCount[eIndex] > 0;
}


void CvArea::changeCleanPowerCount(TeamTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	if (iChange != 0)
	{
		const bool bWasCleanPower = m_aiCleanPowerCount[eIndex] > 0;

		m_aiCleanPowerCount[eIndex] += iChange;

		if (bWasCleanPower != (m_aiCleanPowerCount[eIndex] > 0))
		{
			GET_TEAM(eIndex).updateCommerce();

			if (eIndex == GC.getGame().getActiveTeam())
			{
				gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
			}
		}
	}
}


int CvArea::getBorderObstacleCount(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_aiBorderObstacleCount[eIndex];
}

bool CvArea::isBorderObstacle(TeamTypes eIndex) const
{
	return eIndex != NO_TEAM ? getBorderObstacleCount(eIndex) > 0 : false;
}


void CvArea::changeBorderObstacleCount(TeamTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);

	m_aiBorderObstacleCount[eIndex] += iChange;

	if (iChange > 0 && m_aiBorderObstacleCount[eIndex] == iChange)
	{
		GC.getMap().verifyUnitValidPlot();
	}
}


AreaAITypes CvArea::getAreaAIType(TeamTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	return m_aeAreaAIType[eIndex];
}


void CvArea::setAreaAIType(TeamTypes eIndex, AreaAITypes eNewValue)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	m_aeAreaAIType[eIndex] = eNewValue;
}


CvCity* CvArea::getTargetCity(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	return getCity(m_aTargetCities[eIndex]);
}


void CvArea::setTargetCity(PlayerTypes eIndex, const CvCity* pNewValue)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);

	if (pNewValue != NULL)
	{
		m_aTargetCities[eIndex] = pNewValue->getIDInfo();
	}
	else m_aTargetCities[eIndex].reset();
}


int CvArea::getYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex1);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex2);
	return m_aaiYieldRateModifier[eIndex1][eIndex2];
}


void CvArea::changeYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex1);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex2);

	if (iChange != 0)
	{
		m_aaiYieldRateModifier[eIndex1][eIndex2] += iChange;

		GET_PLAYER(eIndex1).invalidateYieldRankCache(eIndex2);

		if (eIndex2 == YIELD_COMMERCE)
		{
			GET_PLAYER(eIndex1).setCommerceDirty();
		}

		GET_PLAYER(eIndex1).AI_makeAssignWorkDirty();

		if (GET_PLAYER(eIndex1).getTeam() == GC.getGame().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


int CvArea::getNumTrainAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex1);
	FASSERT_BOUNDS(0, NUM_UNITAI_TYPES, eIndex2);
	return m_aaiNumTrainAIUnits[eIndex1][eIndex2];
}


void CvArea::changeNumTrainAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2, int iChange)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex1);
	FASSERT_BOUNDS(0, NUM_UNITAI_TYPES, eIndex2);
	m_aaiNumTrainAIUnits[eIndex1][eIndex2] += iChange;
	FASSERT_NOT_NEGATIVE(getNumTrainAIUnits(eIndex1, eIndex2));
}


int CvArea::getNumAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex1);
	FASSERT_BOUNDS(0, NUM_UNITAI_TYPES, eIndex2);
	return m_aaiNumAIUnits[eIndex1][eIndex2];
}


void CvArea::changeNumAIUnits(PlayerTypes eIndex1, UnitAITypes eIndex2, int iChange)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex1);
	FASSERT_BOUNDS(0, NUM_UNITAI_TYPES, eIndex2);
	m_aaiNumAIUnits[eIndex1][eIndex2] += iChange;
	FASSERT_NOT_NEGATIVE(getNumAIUnits(eIndex1, eIndex2));
}


int CvArea::getNumBonuses(BonusTypes eBonus) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eBonus);
	return m_paiNumBonuses[eBonus];
}


int CvArea::getNumTotalBonuses() const
{
	PROFILE_EXTRA_FUNC();
	int iTotal = 0;

	for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
	{
		iTotal += m_paiNumBonuses[iI];
	}
	return iTotal;
}


void CvArea::changeNumBonuses(BonusTypes eBonus, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eBonus);
	m_paiNumBonuses[eBonus] += iChange;
	FASSERT_NOT_NEGATIVE(m_paiNumBonuses[eBonus]);
}


int CvArea::getNumImprovements(ImprovementTypes eImprovement) const
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), eImprovement);
	return m_paiNumImprovements[eImprovement];
}


void CvArea::changeNumImprovements(ImprovementTypes eImprovement, int iChange)
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), eImprovement);
	m_paiNumImprovements[eImprovement] += iChange;
	FASSERT_NOT_NEGATIVE(getNumImprovements(eImprovement));
}


// Koshling - record rolling history of the last N turns of our combat losses and what we lost to
void CvArea::recordCombatDeath(PlayerTypes ePlayer, UnitTypes lostUnitType, UnitTypes lostToUnitType)
{
	PROFILE_EXTRA_FUNC();
	CombatResultRecord record;

	record.eLoser = ePlayer;
	record.eDefeatedUnitType = lostUnitType;
	record.eVictoriousEnemyUnitType = lostToUnitType;
	const int iGameTurn = GC.getGame().getGameTurn();

	if (m_iLastGameTurnRecorded > -1 && m_iLastGameTurnRecorded < iGameTurn)
	{
		while (m_iLastGameTurnRecorded != iGameTurn)
		{
			m_combatRecord[++m_iLastGameTurnRecorded % COMBAT_RECORD_LENGTH].clear();
		}
	}
	else m_iLastGameTurnRecorded = iGameTurn;

	m_combatRecord[m_iLastGameTurnRecorded % COMBAT_RECORD_LENGTH].push_back(record);
}

// Return the number of units of the specified type recently lost per 100 turns (normalized figure)
// If eUnit is NO_UNIT all types will be tallied
int	CvArea::getRecentCombatDeathRate(PlayerTypes ePlayer, UnitTypes eUnit) const
{
	PROFILE_EXTRA_FUNC();
	int	totalDeaths = 0;

	for (int i = 0; i < COMBAT_RECORD_LENGTH; i++)
	{
		const TurnCombatResults& turnResults = m_combatRecord[i % COMBAT_RECORD_LENGTH];

		foreach_(const CombatResultRecord& record, turnResults)
		{
			if (record.eLoser == ePlayer && (eUnit == NO_UNIT || record.eDefeatedUnitType == eUnit))
			{
				totalDeaths++;
			}
		}
	}

	return 100 * totalDeaths / COMBAT_RECORD_LENGTH;
}

// Return the number of units of the specified type recently lost per 100 turns (normalized figure)
// If eUnit is NO_UNIT all types will be tallied
int	CvArea::getRecentCombatDeathRate(PlayerTypes ePlayer, UnitAITypes eUnitAIType) const
{
	PROFILE_EXTRA_FUNC();
	int	totalDeaths = 0;

	for (int i = 0; i < COMBAT_RECORD_LENGTH; i++)
	{
		const TurnCombatResults& turnResults = m_combatRecord[i % COMBAT_RECORD_LENGTH];

		foreach_(const CombatResultRecord& record, turnResults)
		{
			if (record.eLoser == ePlayer
			&& (eUnitAIType == NO_UNITAI || record.eDefeatedUnitType != NO_UNIT && GC.getUnitInfo(record.eDefeatedUnitType).getDefaultUnitAIType() == eUnitAIType))
			{
				totalDeaths++;
			}
		}
	}

	return (100*totalDeaths)/COMBAT_RECORD_LENGTH;
}


void CvArea::setNumValidPlotsbySpawn(SpawnTypes eSpawn, int iAmount)
{
	PROFILE_EXTRA_FUNC();
	if (NULL == m_aiSpawnValidPlotCount)
	{
		m_aiSpawnValidPlotCount = new int[GC.getNumSpawnInfos()];
		for (int iI = 0; iI < GC.getNumSpawnInfos(); ++iI)
		{
			m_aiSpawnValidPlotCount[iI] = -1;
		}
	}
	m_aiSpawnValidPlotCount[eSpawn] = iAmount;
}

int CvArea::getNumValidPlotsbySpawn(SpawnTypes eSpawn) const
{
	return m_aiSpawnValidPlotCount != NULL ? m_aiSpawnValidPlotCount[eSpawn] : -1;
}