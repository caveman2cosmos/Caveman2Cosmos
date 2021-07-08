#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvPlayerAI.h"
#include "CvStatistics.h"

CvGameRecord::CvGameRecord()
{
	init();
}

CvGameRecord::~CvGameRecord()
{
	uninit();
}

void CvGameRecord::init()
{
	reset();
}

void CvGameRecord::uninit()
{
}

void CvGameRecord::reset()
{
	uninit();

	m_eEra = NO_ERA;

	m_szMapName.clear();
}

void CvGameRecord::setMapName(const char * szMapName)
{
	m_szMapName = szMapName;
}

const CvString& CvGameRecord::getMapName() const
{
	return m_szMapName;
}

void CvGameRecord::setEra( EraTypes eEra )
{
	m_eEra = eEra;
}

EraTypes CvGameRecord::getEra() const
{
	return m_eEra;
}

void CvGameRecord::read(FDataStreamBase* pStream)
{
	// reset before loading
	reset();

	pStream->Read((int*)&m_eEra);

	pStream->ReadString(m_szMapName);
}

void CvGameRecord::write(FDataStreamBase* pStream)
{
	pStream->Write(m_eEra);

	pStream->WriteString(m_szMapName);
}

CvPlayerRecord::CvPlayerRecord()
{
	m_piNumUnitsBuilt = NULL;
	m_piNumUnitsKilled = NULL;
	m_piNumUnitsWasKilled = NULL;
	m_piNumBuildingsBuilt = NULL;
	m_pbReligionFounded = NULL;

	init();
}

CvPlayerRecord::~CvPlayerRecord()
{
	uninit();
}

void CvPlayerRecord::init()
{
	reset();
}

void CvPlayerRecord::uninit()
{
	SAFE_DELETE_ARRAY(m_piNumUnitsBuilt);
	SAFE_DELETE_ARRAY(m_piNumUnitsKilled);
	SAFE_DELETE_ARRAY(m_piNumUnitsWasKilled);
	SAFE_DELETE_ARRAY(m_piNumBuildingsBuilt);

	SAFE_DELETE_ARRAY(m_pbReligionFounded);
}

void CvPlayerRecord::reset()
{
	uninit();

	m_iID = -1;
	m_iTime = 0;

	m_eVictory = NO_VICTORY;
	m_eLeader = NO_LEADER;

	m_piNumUnitsBuilt = new int[GC.getNumUnitInfos()];
	m_piNumUnitsKilled = new int[GC.getNumUnitInfos()];
	m_piNumUnitsWasKilled = new int[GC.getNumUnitInfos()];

	m_piNumBuildingsBuilt = new int[GC.getNumBuildingInfos()];

	m_pbReligionFounded = new bool[GC.getNumReligionInfos()];

	int i;
	for (i = 0; i < GC.getNumUnitInfos(); ++i)
	{
		m_piNumUnitsBuilt[i] = 0;
		m_piNumUnitsKilled[i] = 0;
		m_piNumUnitsWasKilled[i] = 0;
	}
	for (i = 0; i < GC.getNumBuildingInfos(); ++i)
	{
		m_piNumBuildingsBuilt[i] = 0;
	}
	for (i = 0; i < GC.getNumReligionInfos(); ++i)
	{
		m_pbReligionFounded[i] = false;
	}

	m_iNumCitiesBuilt = 0;
	m_iNumCitiesRazed = 0;
	m_iNumGoldenAges = 0;
}

void CvPlayerRecord::unitBuilt( CvUnit *pUnit )
{
	++m_piNumUnitsBuilt[pUnit->getUnitType()];
}

int CvPlayerRecord::getNumUnitsBuilt(int iUnitType) const
{
	return m_piNumUnitsBuilt[iUnitType];
}

void CvPlayerRecord::unitKilled( CvUnit * pUnit )
{
	++m_piNumUnitsKilled[pUnit->getUnitType()];
}

int CvPlayerRecord::getNumUnitsKilled(int iUnitType) const
{
	return m_piNumUnitsKilled[iUnitType];
}

void CvPlayerRecord::unitWasKilled( CvUnit * pUnit )
{
	++m_piNumUnitsWasKilled[pUnit->getUnitType()];
}

int CvPlayerRecord::getNumUnitsWasKilled(int iUnitType) const
{
	return m_piNumUnitsWasKilled[iUnitType];
}

void CvPlayerRecord::buildingBuilt( BuildingTypes eBuilding )
{
	++m_piNumBuildingsBuilt[(int)eBuilding];
}

int CvPlayerRecord::getNumBuildingsBuilt(BuildingTypes eBuilding) const
{
	return m_piNumBuildingsBuilt[(int)eBuilding];
}

void CvPlayerRecord::religionFounded( ReligionTypes eReligion )
{
	m_pbReligionFounded[(int)eReligion] = true;
}

bool CvPlayerRecord::getReligionFounded( ReligionTypes eReligion ) const
{
	return m_pbReligionFounded[(int)eReligion];
}

void CvPlayerRecord::setPlayerID( int iID )
{
	m_iID = iID;
}

int CvPlayerRecord::getPlayerID() const
{
	return m_iID;
}

void CvPlayerRecord::setVictory(VictoryTypes eVictory)
{
	m_eVictory = eVictory;
}

int CvPlayerRecord::getVictory() const
{
	return m_eVictory;
}

void CvPlayerRecord::setTimePlayed( int iTime )
{
	m_iTime = iTime;
}

int CvPlayerRecord::getMinutesPlayed() const
{
	return m_iTime;
}

void CvPlayerRecord::setLeader( LeaderHeadTypes eLeader )
{
	m_eLeader = eLeader;
}

LeaderHeadTypes CvPlayerRecord::getLeader() const
{
	return m_eLeader;
}

void CvPlayerRecord::cityBuilt()
{
	++m_iNumCitiesBuilt;
}

int CvPlayerRecord::getNumCitiesBuilt() const
{
	return m_iNumCitiesBuilt;
}

void CvPlayerRecord::cityRazed()
{
	++m_iNumCitiesRazed;
}

int CvPlayerRecord::getNumCitiesRazed() const
{
	return m_iNumCitiesRazed;
}

void CvPlayerRecord::goldenAge()
{
	++m_iNumGoldenAges;
}

int CvPlayerRecord::getNumGoldenAges() const
{
	return m_iNumGoldenAges;
}

void CvPlayerRecord::read(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	// reset before loading
	reset();

	WRAPPER_READ(wrapper, "CvPlayerRecord", &m_iID);
	WRAPPER_READ(wrapper, "CvPlayerRecord", &m_iTime);

	WRAPPER_READ(wrapper, "CvPlayerRecord", (int*)&m_eVictory);
	WRAPPER_READ_CLASS_ENUM(wrapper, "CvPlayerRecord", REMAPPED_CLASS_TYPE_LEADERHEADS, (int*)&m_eLeader);

	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayerRecord", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_piNumUnitsBuilt);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayerRecord", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_piNumUnitsKilled);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayerRecord", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_piNumUnitsWasKilled);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayerRecord", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_piNumBuildingsBuilt);
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvPlayerRecord", REMAPPED_CLASS_TYPE_RELIGIONS, GC.getNumReligionInfos(), m_pbReligionFounded);

	WRAPPER_READ(wrapper, "CvPlayerRecord", &m_iNumCitiesBuilt);
	WRAPPER_READ(wrapper, "CvPlayerRecord", &m_iNumCitiesRazed);
	WRAPPER_READ(wrapper, "CvPlayerRecord", &m_iNumGoldenAges);

	WRAPPER_READ_OBJECT_END(wrapper);
}

void CvPlayerRecord::write(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE(wrapper, "CvPlayerRecord", m_iID);
	WRAPPER_WRITE(wrapper, "CvPlayerRecord", m_iTime);

	WRAPPER_WRITE(wrapper, "CvPlayerRecord", m_eVictory);
	WRAPPER_WRITE_CLASS_ENUM(wrapper, "CvPlayerRecord", REMAPPED_CLASS_TYPE_LEADERHEADS, m_eLeader);

	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayerRecord", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_piNumUnitsBuilt);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayerRecord", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_piNumUnitsKilled);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayerRecord", REMAPPED_CLASS_TYPE_UNITS, GC.getNumUnitInfos(), m_piNumUnitsWasKilled);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayerRecord", REMAPPED_CLASS_TYPE_BUILDINGS, GC.getNumBuildingInfos(), m_piNumBuildingsBuilt);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlayerRecord", REMAPPED_CLASS_TYPE_RELIGIONS, GC.getNumReligionInfos(), m_pbReligionFounded);

	WRAPPER_WRITE(wrapper, "CvPlayerRecord", m_iNumCitiesBuilt);
	WRAPPER_WRITE(wrapper, "CvPlayerRecord", m_iNumCitiesRazed);
	WRAPPER_WRITE(wrapper, "CvPlayerRecord", m_iNumGoldenAges);

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

////////////////////////////////////////////////////////////////////
// CvStatistics
///////////////////////////////////////////////////////////////////

CvStatistics::~CvStatistics()
{
	uninit();
}

//
// Initialization
//
void CvStatistics::init()
{
	reset();
}
void CvStatistics::uninit()
{
	for(int i=0;i<(int)m_PlayerRecords.size();i++)
	{
		SAFE_DELETE(m_PlayerRecords[i]);	// free memory - MT
	}
	m_PlayerRecords.clear();
}
void CvStatistics::reset()
{
	uninit();
}

//
// Setting game-specific stats
//
void CvStatistics::setMapName(const char * szMapName)
{
	m_GameRecord.setMapName(szMapName);
}
void CvStatistics::setEra(EraTypes eEra)
{
	m_GameRecord.setEra(eEra);
}

//
// Setting player-specific stats
//
void CvStatistics::setVictory( TeamTypes eWinner, VictoryTypes eVictory )
{
	// Report a victory for all players on this team...
	for (int i = 0; i < MAX_PC_PLAYERS; ++i)
	{
		if (GET_PLAYER((PlayerTypes)i).isEverAlive())
		{
			// DAN: They could be eliminated and still watching the game and get a win!
			// How to prevent this?
			if ( (GET_PLAYER((PlayerTypes)i).isHuman()) && (GET_PLAYER((PlayerTypes)i).getTeam() == eWinner) )
			{
				// If this guy is still alive and on the winning team, record the victory
				getPlayerRecord(i)->setVictory(eVictory);
			}
			else
			{
				// Record the loss
				getPlayerRecord(i)->setVictory(NO_VICTORY);
			}
		}
	}
}
void CvStatistics::setTimePlayed( PlayerTypes ePlayer, int iTime )
{
	getPlayerRecord((int)ePlayer)->setTimePlayed(iTime);
}
void CvStatistics::setLeader( PlayerTypes ePlayer, LeaderHeadTypes eLeader )
{
	getPlayerRecord((int)ePlayer)->setLeader(eLeader);
}

//
// Player-specific stat events
//
void CvStatistics::unitBuilt( CvUnit *pUnit )
{
	getPlayerRecord( pUnit->getOwner() )->unitBuilt( pUnit );
}
void CvStatistics::unitKilled( CvUnit *pUnit, PlayerTypes eAttacker )
{
	getPlayerRecord( eAttacker )->unitKilled( pUnit );
	getPlayerRecord( pUnit->getOwner() )->unitWasKilled( pUnit );
}
void CvStatistics::cityBuilt( CvCity *pCity )
{
	getPlayerRecord( pCity->getOwner() )->cityBuilt();
}
void CvStatistics::cityRazed( CvCity * pCity, PlayerTypes ePlayer )
{
	getPlayerRecord( ePlayer )->cityRazed();
}
void CvStatistics::buildingBuilt( CvCity *pCity, BuildingTypes eBuilding )
{
	getPlayerRecord( pCity->getOwner() )->buildingBuilt(eBuilding);
}
void CvStatistics::religionFounded( ReligionTypes eReligion, PlayerTypes eFounder )
{
	getPlayerRecord( eFounder )->religionFounded(eReligion);
}
void CvStatistics::goldenAge( PlayerTypes ePlayer )
{
	getPlayerRecord( ePlayer )->goldenAge();
}

void CvStatistics::read(FDataStreamBase* pStream)
{
	// Read game data into record
	m_GameRecord.read(pStream);

	// Read player data into records
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		if (GET_PLAYER((PlayerTypes)i).isEverAlive())
		{
			getPlayerRecord(i)->read(pStream);
		}
	}
}

void CvStatistics::write(FDataStreamBase* pStream)
{
	// Write game data into record
	m_GameRecord.write(pStream);

	// Write player data into records
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		if (GET_PLAYER((PlayerTypes)i).isEverAlive())
		{
			getPlayerRecord(i)->write(pStream);
		}
	}
}

//
// Player record accessor
//
CvPlayerRecord *CvStatistics::getPlayerRecord(int iIndex)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, iIndex)

	if ( iIndex >= (int)m_PlayerRecords.size() || m_PlayerRecords[iIndex] == NULL )
	{
		CvPlayerRecord *pRecord = new CvPlayerRecord;

		pRecord->init();
		pRecord->setPlayerID( iIndex );
		m_PlayerRecords.resize(iIndex + 1, NULL);
		m_PlayerRecords[iIndex] = pRecord;
	}

	return m_PlayerRecords[iIndex];
}

const char* CvStatistics::getMapName() const
{
	return m_GameRecord.getMapName().GetCString();
}

EraTypes CvStatistics::getEra() const
{
	return m_GameRecord.getEra();
}
