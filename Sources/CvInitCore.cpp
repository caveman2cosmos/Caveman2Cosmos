// game.cpp

#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvInitCore.h"
#include "CvPlayerAI.h"
#include "CvPopupInfo.h"
#include "CvPython.h"
#include "CvSelectionGroup.h"
#include "CvXMLLoadUtility.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLUtilityIFaceBase.h"

// BUG - EXE/DLL Paths - start
//#include <shlobj.h>

CvString* CvInitCore::dllPath = NULL;
CvString* CvInitCore::dllName = NULL;
CvString* CvInitCore::exePath = NULL;
CvString* CvInitCore::exeName = NULL;
bool CvInitCore::bPathsSet;
// BUG - EXE/DLL Paths - end

//	Koshling - save game compataibility between (most) builds
//	UI flag values in game serialization.  These are bitwise combinable
#define	GAME_SAVE_UI_FLAG_VALUE_TAGGED_FORMAT	0x00000002

// Used to signal the BULL saved game format is used
#define BUG_DLL_SAVE_FORMAT		64

uint32_t CvInitCore::m_uiAssetCheckSum = 0;
uint32_t CvInitCore::m_uiSavegameAssetCheckSum = 0;

CvInitCore::CvInitCore()
{
	OutputDebugString("Calling constructor for InitCore: Start\n");

	// Moved to Init as the number is no more predetermined
	//m_abOptions = new bool[GC.getNumGameOptionInfos()];
	m_abOptions = NULL;
	m_abMPOptions = new bool[NUM_MPOPTION_TYPES];
	m_abForceControls = new bool[NUM_FORCECONTROL_TYPES];

	m_aszLeaderName = new CvWString[MAX_PLAYERS];
	m_aszCivDescription = new CvWString[MAX_PLAYERS];
	m_aszCivShortDesc = new CvWString[MAX_PLAYERS];
	m_aszCivAdjective = new CvWString[MAX_PLAYERS];
	m_aszCivPassword = new CvWString[MAX_PLAYERS];
	m_aszEmail = new CvString[MAX_PLAYERS];
	m_aszSmtpHost = new CvString[MAX_PLAYERS];

	m_abWhiteFlag = new bool[MAX_PLAYERS];
	m_aszFlagDecal = new CvWString[MAX_PLAYERS];

	m_aeCiv = new CivilizationTypes[MAX_PLAYERS];
	m_aeLeader = new LeaderHeadTypes[MAX_PLAYERS];
	m_aeTeam = new TeamTypes[MAX_PLAYERS];
	m_aeHandicap = new HandicapTypes[MAX_PLAYERS];
	m_aeColor = new PlayerColorTypes[MAX_PLAYERS];
	m_aeArtStyle = new ArtStyleTypes[MAX_PLAYERS];

	// Slot data
	m_aeSlotStatus = new SlotStatus[MAX_PLAYERS];
	m_aeSlotClaim = new SlotClaim[MAX_PLAYERS];

	// Civ flags
	m_abPlayableCiv = new bool[MAX_PLAYERS];
	m_abMinorNationCiv = new bool[MAX_PLAYERS];

	// Unsaved player data
	m_aiNetID = new int[MAX_PLAYERS];
	m_abReady = new bool[MAX_PLAYERS];

	m_aszPythonCheck = new CvString[MAX_PLAYERS];
	m_aszXMLCheck = new CvString[MAX_PLAYERS];

	m_aeCustomMapOptions = NULL;
	m_abVictories = NULL;

// BUG - EXE/DLL Paths - start
	bPathsSet = false;
// BUG - EXE/DLL Paths - end

	reset(NO_GAMEMODE);

	OutputDebugString("Calling constructor for InitCore: End\n");
}


CvInitCore::~CvInitCore()
{
	uninit();

	SAFE_DELETE_ARRAY(m_abOptions);
	SAFE_DELETE_ARRAY(m_abMPOptions);
	SAFE_DELETE_ARRAY(m_abForceControls);
	SAFE_DELETE_ARRAY(m_aszLeaderName);
	SAFE_DELETE_ARRAY(m_aszCivDescription);
	SAFE_DELETE_ARRAY(m_aszCivShortDesc);
	SAFE_DELETE_ARRAY(m_aszCivAdjective);
	SAFE_DELETE_ARRAY(m_aszCivPassword);
	SAFE_DELETE_ARRAY(m_aszEmail);
	SAFE_DELETE_ARRAY(m_aszSmtpHost);
	SAFE_DELETE_ARRAY(m_abWhiteFlag);
	SAFE_DELETE_ARRAY(m_aszFlagDecal);
	SAFE_DELETE_ARRAY(m_aeCiv);
	SAFE_DELETE_ARRAY(m_aeLeader);
	SAFE_DELETE_ARRAY(m_aeTeam);
	SAFE_DELETE_ARRAY(m_aeHandicap);
	SAFE_DELETE_ARRAY(m_aeColor);
	SAFE_DELETE_ARRAY(m_aeArtStyle);
	SAFE_DELETE_ARRAY(m_aeSlotStatus);
	SAFE_DELETE_ARRAY(m_aeSlotClaim);
	SAFE_DELETE_ARRAY(m_abPlayableCiv);
	SAFE_DELETE_ARRAY(m_abMinorNationCiv);
	SAFE_DELETE_ARRAY(m_aiNetID);
	SAFE_DELETE_ARRAY(m_abReady);
	SAFE_DELETE_ARRAY(m_aszPythonCheck);
	SAFE_DELETE_ARRAY(m_aszXMLCheck);
}

void CvInitCore::init(GameMode eMode)
{
	OutputDebugString("Initialize InitCore: Start\n");

	if (m_abOptions == NULL)
		m_abOptions = new bool[GC.getNumGameOptionInfos()];
	//--------------------------------
	// Init saved data
	reset(eMode);

	OutputDebugString("Initialize InitCore: Start\n");
}

void CvInitCore::uninit()
{
	clearCustomMapOptions();
	clearVictories();
}


// FUNCTION: reset()
// Initializes data members that are serialized.
void CvInitCore::reset(GameMode eMode)
{
	OutputDebugString("Reseting InitCore: Start\n");

	//--------------------------------
	// Uninit class
	uninit();

	resetGame();
	resetPlayers();

	setMode(eMode);

	if (getMode() != NO_GAMEMODE)
	{
		setDefaults();
	}
	CvXMLLoadUtility::RemoveTGAFiller();
	OutputDebugString("Reseting InitCore: End\n");
}

void CvInitCore::setDefaults()
{
	PROFILE_EXTRA_FUNC();
	for (int i = 0; i < GC.getNumGameOptionInfos(); ++i)
	{
		//	Allow the DLL to run against older assets that define fewer options, leaving
		//	more recent ones turned off by default always
		if ( i < GC.getNumGameOptionInfos() )
		{
			m_abOptions[i] = GC.getGameOptionInfo((GameOptionTypes)i).getDefault();
		}
	}
	for (int i = 0; i < NUM_MPOPTION_TYPES; ++i)
	{
		//	Allow the DLL to run against older assets that define fewer options, leaving
		//	more recent ones turned off by defaul always
		if ( i < GC.getNumMPOptionInfos() )
		{
			m_abMPOptions[i] = GC.getMPOptionInfo((MultiplayerOptionTypes)i).getDefault();
		}
	}
}


bool CvInitCore::getHuman(PlayerTypes eID) const
{
	if (eID < MAX_PC_PLAYERS)
	{
		if (getSlotStatus(eID) == SS_TAKEN)
		{
			return true;
		}
		if (getSlotStatus(eID) == SS_OPEN)
		{
			return gDLL->isGameActive() || getHotseat() || getPitboss() || getPbem();
		}
	}
	return false;
}

int CvInitCore::getNumHumans() const
{
	PROFILE_EXTRA_FUNC();
	int iNumHumans = 0;
	for (int i = 0; i < MAX_PC_PLAYERS; ++i)
	{
		if (getHuman((PlayerTypes)i))
		{
			++iNumHumans;
		}
	}
	return iNumHumans;
}

int CvInitCore::getNumDefinedPlayers() const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		if (getCiv((PlayerTypes)i) != NO_CIVILIZATION && getLeader((PlayerTypes)i) != NO_LEADER)
		{
			iCount++;
		}
	}
	return iCount;
}

bool CvInitCore::getMultiplayer() const
{
	switch (getType())
	{
	case GAME_MP_NEW:
	case GAME_MP_LOAD:
	case GAME_MP_SCENARIO:
		return true;
	default:
		return false;
	}
}

bool CvInitCore::getNewGame() const
{
	switch (getType())
	{
	case GAME_SP_NEW:
	case GAME_SP_SCENARIO:
	case GAME_MP_NEW:
	case GAME_MP_SCENARIO:
	case GAME_HOTSEAT_NEW:
	case GAME_HOTSEAT_SCENARIO:
	case GAME_PBEM_NEW:
	case GAME_PBEM_SCENARIO:
		return true;
	default:
		return false;
	}
}

bool CvInitCore::getSavedGame() const
{
	switch (getType())
	{
	case GAME_SP_LOAD:
	case GAME_MP_LOAD:
	case GAME_HOTSEAT_LOAD:
	case GAME_PBEM_LOAD:
		return true;
	default:
		return false;
	}
}

bool CvInitCore::getPitboss() const
{
	return getMode() == GAMEMODE_PITBOSS;
}

bool CvInitCore::getHotseat() const
{
	switch (getType())
	{
	case GAME_HOTSEAT_NEW:
	case GAME_HOTSEAT_SCENARIO:
	case GAME_HOTSEAT_LOAD:
		return true;
	default:
		return false;
	}
}


bool CvInitCore::getPbem() const
{
	switch (getType())
	{
	case GAME_PBEM_NEW:
	case GAME_PBEM_SCENARIO:
	case GAME_PBEM_LOAD:
		return true;
	default:
		return false;
	}
}


bool CvInitCore::checkBounds(int iValue, int iLower, int iUpper) const
{
	if (iValue >= iLower)
	{
		if (iValue < iUpper)
		{
			return true;
		}
	}
	return false;
}


bool CvInitCore::getSlotVacant(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PC_PLAYERS, eID);

	bool bRetVal = false;

	if (checkBounds(eID, 0, MAX_PC_PLAYERS))
	{
		const SlotStatus eStatus = getSlotStatus(eID);

		// Check the status of this slot
		if (eStatus == SS_OPEN || getMPOption(MPOPTION_TAKEOVER_AI) && eStatus == SS_COMPUTER)
		{
			bRetVal = getSlotClaim(eID) != SLOTCLAIM_ASSIGNED;
		}
	}
	return bRetVal;
}

PlayerTypes CvInitCore::getAvailableSlot()
{
	PROFILE_EXTRA_FUNC();
	// Get the next ID available ID
	// First check for open slots only
	for (int i = 0; i < MAX_PC_PLAYERS; ++i)
	{
		const PlayerTypes eID = static_cast<PlayerTypes>(i);

		if (getSlotClaim(eID) == SLOTCLAIM_UNASSIGNED && getSlotStatus(eID) == SS_OPEN)
		{
			setSlotClaim(eID, SLOTCLAIM_ASSIGNED);
			return eID;
		}
	}

	// That didn't work, check to see if we can assign computer slots
	if (getMPOption(MPOPTION_TAKEOVER_AI))
	{
		for (int i = 0; i < MAX_PC_PLAYERS; ++i)
		{
			const PlayerTypes eID = static_cast<PlayerTypes>(i);

			if (getSlotClaim(eID) == SLOTCLAIM_UNASSIGNED && getSlotStatus(eID) == SS_COMPUTER)
			{
				setSlotClaim(eID, SLOTCLAIM_ASSIGNED);
				return eID;
			}
		}
	}
	// None available at all...
	return NO_PLAYER;
}

void CvInitCore::reassignPlayer(PlayerTypes eOldID, PlayerTypes eNewID)
{
	/* Toffer - Not really much point in not allowing empty slots inbetween taken slots.
	//	This is called by the exe upon creating a new game to give players a lower player slot index if one is available.
	FASSERT_BOUNDS(0, 0, eOldID);
	FASSERT_BOUNDS(0, 0, eNewID);

	if (checkBounds(eOldID, 0, MAX_PC_PLAYERS) && checkBounds(eNewID, 0, MAX_PC_PLAYERS))
	{
		// *** SAVE TARGET SLOT DETAILS TEMPORARILY
		// Temp civ details
		CvWString szLeaderName = m_aszLeaderName[eNewID];
		CvWString szCivDescription = m_aszCivDescription[eNewID];
		CvWString szCivShortDesc = m_aszCivShortDesc[eNewID];
		CvWString szCivAdjective = m_aszCivAdjective[eNewID];
		CvWString szCivPassword = m_aszCivPassword[eNewID];
		CvString szEmail = m_aszEmail[eNewID];
		CvString szSmtpHost = m_aszSmtpHost[eNewID];
		bool bWhiteFlag = m_abWhiteFlag[eNewID];
		CvWString szFlagDecal = m_aszFlagDecal[eNewID];
		CivilizationTypes eCiv = m_aeCiv[eNewID];
		LeaderHeadTypes eLeader = m_aeLeader[eNewID];
		TeamTypes eTeam = m_aeTeam[eNewID];
		HandicapTypes eHandicap = m_aeHandicap[eNewID];
		PlayerColorTypes eColor = m_aeColor[eNewID];
		ArtStyleTypes eArtStyle = m_aeArtStyle[eNewID];
		// Temp slot data
		SlotStatus eSlotStatus = m_aeSlotStatus[eNewID];
		SlotClaim eSlotClaim = m_aeSlotClaim[eNewID];
		// Temp civ flags
		bool bPlayableCiv = m_abPlayableCiv[eNewID];
		bool bMinorNationCiv = m_abMinorNationCiv[eNewID];
		// Temp unsaved player data
		int iNetID = m_aiNetID[eNewID];
		bool bReady = m_abReady[eNewID];
		CvString szPythonCheck = m_aszPythonCheck[eNewID];
		CvString szXMLCheck = m_aszXMLCheck[eNewID];

		// *** SAVE OLD SLOT DETAILS IN NEW SLOT
		// New civ details
		m_aszLeaderName[eNewID] = m_aszLeaderName[eOldID];
		m_aszCivDescription[eNewID] = m_aszCivDescription[eOldID];
		m_aszCivShortDesc[eNewID] = m_aszCivShortDesc[eOldID];
		m_aszCivAdjective[eNewID] = m_aszCivAdjective[eOldID];
		m_aszCivPassword[eNewID] = m_aszCivPassword[eOldID];
		m_aszEmail[eNewID] = m_aszEmail[eOldID];
		m_aszSmtpHost[eNewID] = m_aszSmtpHost[eOldID];
		m_abWhiteFlag[eNewID] = m_abWhiteFlag[eOldID];
		m_aszFlagDecal[eNewID] = m_aszFlagDecal[eOldID];
		m_aeCiv[eNewID] = m_aeCiv[eOldID];
		m_aeLeader[eNewID] = m_aeLeader[eOldID];
		m_aeTeam[eNewID] = m_aeTeam[eOldID];
		m_aeHandicap[eNewID] = m_aeHandicap[eOldID];
		m_aeColor[eNewID] = m_aeColor[eOldID];
		m_aeArtStyle[eNewID] = m_aeArtStyle[eOldID];
		// New slot data
		m_aeSlotStatus[eNewID] = m_aeSlotStatus[eOldID];
		m_aeSlotClaim[eNewID] = m_aeSlotClaim[eOldID];
		// New civ flags
		m_abPlayableCiv[eNewID] = m_abPlayableCiv[eOldID];
		m_abMinorNationCiv[eNewID] = m_abMinorNationCiv[eOldID];
		// New unsaved player data
		m_aiNetID[eNewID] = m_aiNetID[eOldID];
		m_abReady[eNewID] = m_abReady[eOldID];
		m_aszPythonCheck[eNewID] = m_aszPythonCheck[eOldID];
		m_aszXMLCheck[eNewID] = m_aszXMLCheck[eOldID];

		// *** SAVE TEMP DETAILS IN OLD SLOT
		// New civ details
		m_aszLeaderName[eOldID] = szLeaderName;
		m_aszCivDescription[eOldID] = szCivDescription;
		m_aszCivShortDesc[eOldID] = szCivShortDesc;
		m_aszCivAdjective[eOldID] = szCivAdjective;
		m_aszCivPassword[eOldID] = szCivPassword;
		m_aszEmail[eOldID] = szEmail;
		m_aszSmtpHost[eOldID] = szSmtpHost;
		m_abWhiteFlag[eOldID] = bWhiteFlag;
		m_aszFlagDecal[eOldID] = szFlagDecal;
		m_aeCiv[eOldID] = eCiv;
		m_aeLeader[eOldID] = eLeader;
		m_aeTeam[eOldID] = eTeam;
		m_aeHandicap[eOldID] = eHandicap;
		m_aeColor[eOldID] = eColor;
		m_aeArtStyle[eOldID] = eArtStyle;
		// New slot data
		m_aeSlotStatus[eOldID] = eSlotStatus;
		m_aeSlotClaim[eOldID] = eSlotClaim;
		// New civ flags
		m_abPlayableCiv[eOldID] = bPlayableCiv;
		m_abMinorNationCiv[eOldID] = bMinorNationCiv;
		// New unsaved player data
		m_aiNetID[eOldID] = iNetID;
		m_abReady[eOldID] = bReady;
		m_aszPythonCheck[eOldID] = szPythonCheck;
		m_aszXMLCheck[eOldID] = szXMLCheck;

		// We may have a new active player id...
		if (getActivePlayer() == eOldID)
		{
			setActivePlayer(eNewID);
		}
		else if (getActivePlayer() == eNewID)
		{
			setActivePlayer(eOldID);
		}

		if(CvPlayerAI::areStaticsInitialized())
		{
			GET_PLAYER(eOldID).updateTeamType();
			GET_PLAYER(eNewID).updateTeamType();
			GET_PLAYER(eOldID).updateHuman();
			GET_PLAYER(eNewID).updateHuman();
		}
	}
	*/
}

void CvInitCore::endGameSetup()
{
	PROFILE_EXTRA_FUNC();
	// Open inactive slots mean different things to different game modes and types...
	// Let's figure out what they mean for us
	for (int i = 0; i < MAX_PC_PLAYERS; i++)
	{
		const PlayerTypes eID = static_cast<PlayerTypes>(i);
		if (getSlotStatus(eID) == SS_OPEN)
		{
			if (getPitboss() || getHotseat() || getPbem())
			{
				// Pitboss & hotseat - all "open" slots are non-present human players
				setSlotStatus(eID, SS_TAKEN);
			}
			else if (getType() == GAME_MP_SCENARIO)
			{
				// Multiplayer scenario - all "open" slots should be filled with an AI player
				setSlotStatus(eID, SS_COMPUTER);
			}
			else
			{
				// If it's a normal game, all "open" slots should be closed.
				setSlotStatus(eID, SS_CLOSED);
			}
			setSlotClaim(eID, SLOTCLAIM_UNASSIGNED);

			gDLL->sendPlayerInfo(eID);
		}
	}

	// Toffer - Not all NPC slots are in use, so just close them all at this point.
	for (int i = MAX_PC_PLAYERS; i < MAX_PLAYERS; i++)
	{
		const PlayerTypes eID = static_cast<PlayerTypes>(i);
		m_aeSlotStatus[eID] = SS_CLOSED;
		m_aeSlotClaim[eID] = SLOTCLAIM_UNASSIGNED;
	}
}


// Toffer - Only called for host when starting a MP game, called by exe.
//	Be careful of what you add to this one, OOS alert.
//	Moved its content to endGameSetup() above, which is called shortly after for all players.
void CvInitCore::closeInactiveSlots() { }

void CvInitCore::reopenInactiveSlots()
{
	PROFILE_EXTRA_FUNC();
	// "Inactive" open slots will only be in Pitboss and Hotseat
	if (getPitboss() || getHotseat() || getPbem())
	{
		for (int i = 0; i < MAX_PC_PLAYERS; ++i)
		{
			const PlayerTypes eID = static_cast<PlayerTypes>(i);
			// Reopen all slots that don't have active connections
			if (getSlotStatus(eID) == SS_TAKEN && getSlotClaim(eID) != SLOTCLAIM_ASSIGNED)
			{
				setSlotStatus(eID, SS_OPEN);
			}
		}
	}
}

void CvInitCore::resetGame()
{
	PROFILE_EXTRA_FUNC();
	OutputDebugString("Reseting Game: Start\n");

	// Descriptive strings about game and map
	m_eType = GAME_NONE;
	m_szGameName.clear();
	m_szGamePassword.clear();
	m_szAdminPassword.clear();
	m_szMapScriptName.clear();

	m_bWBMapNoPlayers = false;

	// Standard game parameters
	m_eWorldSize = NO_WORLDSIZE;											// STANDARD_ option?
	m_eClimate = (ClimateTypes)GC.getDefineINT("STANDARD_CLIMATE");			// NO_ option?
	m_eSeaLevel = (SeaLevelTypes)GC.getDefineINT("STANDARD_SEALEVEL");		// NO_ option?
	m_eEra = (EraTypes)GC.getDefineINT("STANDARD_ERA");						// NO_ option?
	m_eGameSpeed = (GameSpeedTypes)GC.getDefineINT("STANDARD_GAMESPEED");	// NO_ option?
	m_eTurnTimer = (TurnTimerTypes)GC.getDefineINT("STANDARD_TURNTIMER");	// NO_ option?
	m_eCalendar = (CalendarTypes)GC.getDefineINT("STANDARD_CALENDAR");		// NO_ option?

	// Map-specific custom parameters
	clearCustomMapOptions();

	// Data-defined victory conditions
	refreshVictories();


	// Standard game options
	int i;
	for (i = 0; i < GC.getNumGameOptionInfos(); ++i)
	{
		m_abOptions[i] = false;
	}
	for (i = 0; i < NUM_MPOPTION_TYPES; ++i)
	{
		m_abMPOptions[i] = false;
	}
	m_bStatReporting = false;

	for (i = 0; i < NUM_FORCECONTROL_TYPES; ++i)
	{
		m_abForceControls[i] = false;
	}

	// Game turn mgmt
	m_iGameTurn = 0;
	m_iMaxTurns = 0;
	m_iPitbossTurnTime = 0;
	m_iTargetScore = 0;

	// City Elimination
	m_iMaxCityElimination = 0;

	m_iNumAdvancedStartPoints = 0;

	// Unsaved game data
	m_uiSyncRandSeed = 0;
	m_uiMapRandSeed = 0;
	m_eActivePlayer = NO_PLAYER;

	// Temp vars
	m_szTemp.clear();

	OutputDebugString("Reseting Game: End\n");
}

void CvInitCore::resetGame(CvInitCore * pSource, bool bClear, bool bSaveGameType)
{
	PROFILE_EXTRA_FUNC();
	OutputDebugString("Reseting Game with Source: Start\n");

	FAssertMsg(pSource, "Passed null pointer to CvInitCore::resetGame");
	FAssertMsg(!bClear || !bSaveGameType, "Should not be clearing data while trying to preserve gametype info in CvInitCore::resetGame");

	if (bClear || !pSource)
	{
		resetGame();
	}
	if (pSource)
	{
		// Only copy over saved data

		// Descriptive strings about game and map
		if (!bSaveGameType || getGameMultiplayer() != pSource->getGameMultiplayer())
		{
			setType(pSource->getType());
		}
		setGameName(pSource->getGameName());
		setGamePassword(pSource->getGamePassword());
		setAdminPassword(pSource->getAdminPassword(), false);
		setMapScriptName(pSource->getMapScriptName());

		setWBMapNoPlayers(pSource->getWBMapNoPlayers());

		// Standard game parameters
		setWorldSize(pSource->getWorldSize());
		setClimate(pSource->getClimate());
		setSeaLevel(pSource->getSeaLevel());
		setEra(pSource->getEra());
		setGameSpeed(pSource->getGameSpeed());
		setTurnTimer(pSource->getTurnTimer());
		setCalendar(pSource->getCalendar());

		// Map-specific custom parameters
		setCustomMapOptions(pSource->getNumCustomMapOptions(), pSource->getCustomMapOptions());
		m_iNumHiddenCustomMapOptions = pSource->getNumHiddenCustomMapOptions();
		setVictories(pSource->getNumVictories(), pSource->getVictories());

		// Standard game options
		int i;
		for (i = 0; i < GC.getNumGameOptionInfos(); ++i)
		{
			setOption((GameOptionTypes)i, pSource->getOption((GameOptionTypes)i));
		}

		for (i = 0; i < NUM_MPOPTION_TYPES; ++i)
		{
			setMPOption((MultiplayerOptionTypes)i, pSource->getMPOption((MultiplayerOptionTypes)i));
		}
		setStatReporting(pSource->getStatReporting());

		// Game turn mgmt
		setGameTurn(pSource->getGameTurn());
		setMaxTurns(pSource->getMaxTurns());
		setPitbossTurnTime(pSource->getPitbossTurnTime());
		setTargetScore(pSource->getTargetScore());

		// City Elimination
		setMaxCityElimination(pSource->getMaxCityElimination());

		setNumAdvancedStartPoints(pSource->getNumAdvancedStartPoints());

		setSyncRandSeed(pSource->getSyncRandSeed());
		setMapRandSeed(pSource->getMapRandSeed());
	}

	OutputDebugString("Reseting Game with Source: End\n");
}

void CvInitCore::resetPlayers()
{
	PROFILE_EXTRA_FUNC();
	OutputDebugString("Reseting Players: Start\n");
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		resetPlayer((PlayerTypes)i);
	}
	OutputDebugString("Reseting Players: End\n");
}

void CvInitCore::resetPlayers(CvInitCore * pSource, bool bClear, bool bSaveSlotInfo)
{
	PROFILE_EXTRA_FUNC();
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		resetPlayer((PlayerTypes)i, pSource, bClear, bSaveSlotInfo);
	}
}

void CvInitCore::resetPlayer(PlayerTypes eID)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);

	if ( checkBounds(eID, 0, MAX_PLAYERS) )
	{
		// Only copy over saved data

		// Civ details
		m_aszLeaderName[eID].clear();
		m_aszCivDescription[eID].clear();
		m_aszCivShortDesc[eID].clear();
		m_aszCivAdjective[eID].clear();
		m_aszCivPassword[eID].clear();
		m_aszEmail[eID].clear();
		m_aszSmtpHost[eID].clear();

		m_abWhiteFlag[eID] = false;
		m_aszFlagDecal[eID].clear();

		m_aeCiv[eID] = NO_CIVILIZATION;
		m_aeLeader[eID] = NO_LEADER;
		m_aeTeam[eID] = (TeamTypes)eID;
		m_aeHandicap[eID] = (HandicapTypes)GC.getDefineINT("STANDARD_HANDICAP");
		m_aeColor[eID] = NO_PLAYERCOLOR;
		m_aeArtStyle[eID] = NO_ARTSTYLE;


		// Slot data
		if (eID < MAX_PC_PLAYERS)
		{
			m_aeSlotStatus[eID] = SS_CLOSED;
			m_aeSlotClaim[eID] = SLOTCLAIM_UNASSIGNED;
		}
		else
		{
			m_aeSlotStatus[eID] = SS_MAX_SLOT_STATUS;
			m_aeSlotClaim[eID] = NUM_SLOTCLAIMS;
		}

		// Civ flags
		m_abPlayableCiv[eID] = false;
		m_abMinorNationCiv[eID] = false;

		// Unsaved player data
		m_aiNetID[eID] = -1;
		m_abReady[eID] = false;
		m_aszPythonCheck[eID].clear();
		m_aszXMLCheck[eID].clear();

		if (CvPlayerAI::areStaticsInitialized())
		{
			GET_PLAYER(eID).updateTeamType();
			GET_PLAYER(eID).updateHuman();
		}
	}
}

void CvInitCore::resetPlayer(PlayerTypes eID, CvInitCore * pSource, bool bClear, bool bSaveSlotInfo)
{
	FAssertMsg(pSource, "Passed null pointer to CvInitCore::resetPlayer");
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);

	FAssertMsg(!bClear || !bSaveSlotInfo, "Should not be clearing data while trying to preserve slot info in CvInitCore::resetPlayer");

	if ( checkBounds(eID, 0, MAX_PLAYERS) )
	{
		if (bClear || !pSource)
		{
			resetPlayer(eID);
		}
		if (pSource)
		{
			// Civ details
			setCivDescription(eID, pSource->getCivDescription(eID));
			setCivShortDesc(eID, pSource->getCivShortDesc(eID));
			setCivAdjective(eID, pSource->getCivAdjective(eID));

			setCivPassword(eID, pSource->getCivPassword(eID), false);
			setEmail(eID, pSource->getEmail(eID));
			setSmtpHost(eID, pSource->getSmtpHost(eID));
			setFlagDecal(eID, pSource->getFlagDecal(eID));
			setWhiteFlag(eID, pSource->getWhiteFlag(eID));

			setHandicap(eID, (HandicapTypes)range(pSource->getHandicap(eID), 0, GC.getNumHandicapInfos() - 1));
			setCiv(eID, pSource->getCiv(eID));
			setTeam(eID, pSource->getTeam(eID));
			setLeader(eID, pSource->getLeader(eID));
			setColor(eID, pSource->getColor(eID));
			setArtStyle(eID, pSource->getArtStyle(eID));

			setPlayableCiv(eID, pSource->getPlayableCiv(eID));
			setMinorNationCiv(eID, pSource->getMinorNationCiv(eID));

			// Slot data
			if (!bSaveSlotInfo)
			{
				// We don't wanna reset the slot data if we are loading a game
				// from init - we want to keep the changes we made during init
				setLeaderName(eID, pSource->getLeaderName(eID));
				setSlotStatus(eID, pSource->getSlotStatus(eID));
				setSlotClaim(eID, pSource->getSlotClaim(eID));
			}
		}
	}
}


CvWString CvInitCore::getMapScriptName() const
{
	if (gDLL->getTransferredMap() && !getWBMapScript())
	{
		// If it's a transferred Python file, we have to hack in the transferred extension
		return m_szMapScriptName + CvWString(MAP_TRANSFER_EXT);
	}
	return m_szMapScriptName;
}

void CvInitCore::setMapScriptName(const CvWString & szMapScriptName)
{
	m_szMapScriptName = szMapScriptName;
	refreshCustomMapOptions();
}

bool CvInitCore::getWBMapScript() const
{
	return (gDLL->isDescFileName( CvString(m_szMapScriptName).GetCString() ));
}


void CvInitCore::setWorldSize(const CvWString & szWorldSize)
{
	PROFILE_EXTRA_FUNC();
	for (int i = 0; i < GC.getNumWorldInfos(); i++)
	{
		if ( wcsicmp(szWorldSize.GetCString(), CvWString(GC.getWorldInfo((WorldSizeTypes)i).getType()).GetCString()) == 0 )
		{
			setWorldSize((WorldSizeTypes)i);
		}
	}
}

const CvWString & CvInitCore::getWorldSizeKey(CvWString & szBuffer) const
{
	if (checkBounds(getWorldSize(), 0, GC.getNumWorldInfos()))
	{
		szBuffer = GC.getWorldInfo(getWorldSize()).getType();
		return szBuffer;
	}
	szBuffer = L"NO_WORLDSIZE";
	return szBuffer;
}

void CvInitCore::setClimate(const CvWString & szClimate)
{
	PROFILE_EXTRA_FUNC();
	for (int i = 0; i < GC.getNumClimateInfos(); i++)
	{
		if ( wcsicmp(szClimate.GetCString(), CvWString(GC.getClimateInfo((ClimateTypes)i).getType()).GetCString()) == 0 )
		{
			setClimate((ClimateTypes)i);
		}
	}
}

const CvWString & CvInitCore::getClimateKey(CvWString & szBuffer) const
{
	if ( checkBounds(getClimate(), 0, GC.getNumClimateInfos()) )
	{
		szBuffer = GC.getClimateInfo(getClimate()).getType();
		return szBuffer;
	}
	szBuffer = L"NO_CLIMATE";
	return szBuffer;
}

void CvInitCore::setSeaLevel(const CvWString & szSeaLevel)
{
	PROFILE_EXTRA_FUNC();
	for (int i = 0; i < GC.getNumSeaLevelInfos(); i++)
	{
		if ( wcsicmp(szSeaLevel.GetCString(), CvWString(GC.getSeaLevelInfo((SeaLevelTypes)i).getType()).GetCString()) == 0 )
		{
			setSeaLevel((SeaLevelTypes)i);
		}
	}
}

const CvWString & CvInitCore::getSeaLevelKey(CvWString & szBuffer) const
{
	if ( checkBounds(getSeaLevel(), 0, GC.getNumSeaLevelInfos()) )
	{
		szBuffer = GC.getSeaLevelInfo(getSeaLevel()).getType();
		return szBuffer;
	}
	szBuffer = L"NO_SEALEVEL";
	return szBuffer;
}

void CvInitCore::setEra(const CvWString & szEra)
{
	PROFILE_EXTRA_FUNC();
	for (int i = 0; i < GC.getNumEraInfos(); i++)
	{
		if ( wcsicmp(szEra.GetCString(), CvWString(GC.getEraInfo((EraTypes)i).getType()).GetCString()) == 0 )
		{
			setEra((EraTypes)i);
		}
	}
}

const CvWString & CvInitCore::getEraKey(CvWString & szBuffer) const
{
	if ( checkBounds(getEra(), 0, GC.getNumEraInfos()) )
	{
		szBuffer = GC.getEraInfo(getEra()).getType();
		return szBuffer;
	}
	szBuffer = L"NO_ERA";
	return szBuffer;
}

void CvInitCore::setGameSpeed(const CvWString & szGameSpeed)
{
	PROFILE_EXTRA_FUNC();
	for (int i = 0; i < GC.getNumGameSpeedInfos(); i++)
	{
		if ( wcsicmp(szGameSpeed.GetCString(), CvWString(GC.getGameSpeedInfo((GameSpeedTypes)i).getType()).GetCString()) == 0 )
		{
			setGameSpeed((GameSpeedTypes)i);
		}
	}
}

const CvWString & CvInitCore::getGameSpeedKey(CvWString & szBuffer) const
{
	if ( checkBounds(getGameSpeed(), 0, GC.getNumGameSpeedInfos()) )
	{
		szBuffer = GC.getGameSpeedInfo(getGameSpeed()).getType();
		return szBuffer;
	}
	szBuffer = L"NO_GAMESPEED";
	return szBuffer;
}

void CvInitCore::setTurnTimer(const CvWString & szTurnTimer)
{
	PROFILE_EXTRA_FUNC();
	for (int i = 0; i < GC.getNumTurnTimerInfos(); i++)
	{
		if ( wcsicmp(szTurnTimer.GetCString(), CvWString(GC.getTurnTimerInfo((TurnTimerTypes)i).getType()).GetCString()) == 0 )
		{
			setTurnTimer((TurnTimerTypes)i);
		}
	}
}

const CvWString & CvInitCore::getTurnTimerKey(CvWString & szBuffer) const
{
	if ( checkBounds(getTurnTimer(), 0, GC.getNumTurnTimerInfos()) )
	{
		szBuffer = GC.getTurnTimerInfo(getTurnTimer()).getType();
		return szBuffer;
	}
	szBuffer = L"NO_TURNTIMER";
	return szBuffer;
}

void CvInitCore::setCalendar(const CvWString & szCalendar)
{
	PROFILE_EXTRA_FUNC();
	for (int i = 0; i < GC.getNumCalendarInfos(); i++)
	{
		if ( wcsicmp(szCalendar.GetCString(), CvWString(GC.getCalendarInfo((CalendarTypes)i).getType()).GetCString()) == 0 )
		{
			setCalendar((CalendarTypes)i);
		}
	}
}

const CvWString & CvInitCore::getCalendarKey(CvWString & szBuffer) const
{
	if ( checkBounds(getCalendar(), 0, GC.getNumCalendarInfos()) )
	{
		szBuffer = GC.getCalendarInfo(getCalendar()).getType();
		return szBuffer;
	}
	szBuffer = L"NO_CALENDAR";
	return szBuffer;
}


void CvInitCore::clearCustomMapOptions()
{
	SAFE_DELETE_ARRAY(m_aeCustomMapOptions);
	m_iNumCustomMapOptions = 0;
}

void CvInitCore::refreshCustomMapOptions()
{
	PROFILE_EXTRA_FUNC();
	clearCustomMapOptions();

	if (!getWBMapScript() && gDLL->pythonMapExists(CvString(getMapScriptName()).GetCString()))
	{
		Cy::call_optional<int>(CvString(getMapScriptName()).GetCString(), "getNumHiddenCustomMapOptions", m_iNumHiddenCustomMapOptions);
		int iNumOptions = 0;
		if (Cy::call_optional<int>(CvString(getMapScriptName()).GetCString(), "getNumCustomMapOptions", iNumOptions) && iNumOptions > 0)
		{
			// Got number of custom map options - now get the option defaults
			std::vector<CustomMapOptionTypes> aeMapOptions(iNumOptions, NO_CUSTOM_MAPOPTION);
			for (int i = 0; i < iNumOptions; ++i)
			{
				if (!Cy::call_optional(CvString(getMapScriptName()).GetCString(), "getCustomMapOptionDefault", Cy::Args() << i, aeMapOptions[i]))
				{
					FErrorMsg(CvString::format("Python function getCustomMapOptionDefault in mapscript %s failed to return correctly for option index %d", getMapScriptName(), i).c_str());
				}
			}
			setCustomMapOptions(aeMapOptions.size(), &aeMapOptions[0]);
			// SAFE_DELETE_ARRAY(aeMapOptions);
		}
	}
}


void CvInitCore::clearVictories()
{
	SAFE_DELETE_ARRAY(m_abVictories);
	m_iNumVictories = 0;
}

void CvInitCore::refreshVictories()
{
	PROFILE_EXTRA_FUNC();
	clearVictories();

	m_iNumVictories = GC.getNumVictoryInfos();
	if (m_iNumVictories > 0)
	{
		m_abVictories = new bool[m_iNumVictories];
		for (int i = 0; i < m_iNumVictories; ++i)
		{
			m_abVictories[i] = true;
		}
	}
}

void CvInitCore::setCustomMapOptions(int iNumCustomMapOptions, const CustomMapOptionTypes * aeCustomMapOptions)
{
	PROFILE_EXTRA_FUNC();
	clearCustomMapOptions();
	if (iNumCustomMapOptions)
	{
		FAssertMsg(aeCustomMapOptions, "CustomMap Num/Pointer mismatch in CvInitCore::setCustomMapOptions");
		m_iNumCustomMapOptions = iNumCustomMapOptions;

		m_aeCustomMapOptions = new CustomMapOptionTypes[m_iNumCustomMapOptions];
		for (int i = 0; i < m_iNumCustomMapOptions; ++i)
		{
			m_aeCustomMapOptions[i] = aeCustomMapOptions[i];
		}
	}
}

CustomMapOptionTypes CvInitCore::getCustomMapOption(int iOptionID) const
{
	if (checkBounds(iOptionID, 0, m_iNumCustomMapOptions))
	{
		return m_aeCustomMapOptions[iOptionID];
	}
	return NO_CUSTOM_MAPOPTION;
}

void CvInitCore::setCustomMapOption(int iOptionID, CustomMapOptionTypes eCustomMapOption)
{
	FASSERT_BOUNDS(0, m_iNumCustomMapOptions, iOptionID);

	if (checkBounds(iOptionID, 0, m_iNumCustomMapOptions))
	{
		m_aeCustomMapOptions[iOptionID] = eCustomMapOption;
	}
}


void CvInitCore::setVictories(int iNumVictories, const bool * abVictories)
{
	PROFILE_EXTRA_FUNC();
	clearVictories();
	if (iNumVictories)
	{
		FAssertMsg(abVictories, "Victory Num/Pointer mismatch in CvInitCore::setVictories");
		m_iNumVictories = iNumVictories;

		m_abVictories = new bool[m_iNumVictories];
		for (int i = 0; i < m_iNumVictories; ++i)
		{
			m_abVictories[i] = abVictories[i];
		}
	}
}

bool CvInitCore::getVictory(VictoryTypes eVictoryID) const
{
	FASSERT_BOUNDS(0, m_iNumVictories, eVictoryID);
	if ( checkBounds(eVictoryID, 0, m_iNumVictories) )
	{
		return m_abVictories[eVictoryID];
	}
	return false;
}

void CvInitCore::setVictory(VictoryTypes eVictoryID, bool bVictory)
{
	FASSERT_BOUNDS(0, m_iNumVictories, eVictoryID);
	if ( checkBounds(eVictoryID, 0, m_iNumVictories) )
	{
		m_abVictories[eVictoryID] = bVictory;
	}
}


bool CvInitCore::getOption(GameOptionTypes eIndex) const
{
	const int numGameOptionTypes = GC.getNumGameOptionInfos();
	FASSERT_BOUNDS(0, numGameOptionTypes, eIndex);
	FAssertMsg(m_abOptions != NULL, "Access to unconstructed game option array");
	if ( m_abOptions != NULL && checkBounds(eIndex, 0, numGameOptionTypes) )
	{
		return m_abOptions[eIndex];
	}
	return false;
}

void CvInitCore::setOption(GameOptionTypes eIndex, bool bOption)
{
	FASSERT_BOUNDS(0, GC.getNumGameOptionInfos(), eIndex);
	if (checkBounds(eIndex, 0, GC.getNumGameOptionInfos()))
	{
		m_abOptions[eIndex] = bOption;
	}
}

bool CvInitCore::getMPOption(MultiplayerOptionTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_MPOPTION_TYPES, eIndex);
	if ( checkBounds(eIndex, 0, NUM_MPOPTION_TYPES) )
	{
		return m_abMPOptions[eIndex];
	}
	return false;
}

void CvInitCore::setMPOption(MultiplayerOptionTypes eIndex, bool bOption)
{
	FASSERT_BOUNDS(0, NUM_MPOPTION_TYPES, eIndex);
	if ( checkBounds(eIndex, 0, NUM_MPOPTION_TYPES) )
	{
		m_abMPOptions[eIndex] = bOption;
	}
}

bool CvInitCore::getForceControl(ForceControlTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_FORCECONTROL_TYPES, eIndex);
	if ( checkBounds(eIndex, 0, NUM_FORCECONTROL_TYPES) )
	{
		return m_abForceControls[eIndex];
	}
	return false;
}

void CvInitCore::setForceControl(ForceControlTypes eIndex, bool bOption)
{
	FASSERT_BOUNDS(0, NUM_FORCECONTROL_TYPES, eIndex);
	if ( checkBounds(eIndex, 0, NUM_FORCECONTROL_TYPES) )
	{
		m_abForceControls[eIndex] = bOption;
	}
}


void CvInitCore::setActivePlayer(PlayerTypes eActivePlayer)
{
	m_eActivePlayer = eActivePlayer;

	if (m_eActivePlayer != NO_PLAYER)
	{
		// Automatically claim this slot
		setSlotClaim(m_eActivePlayer, SLOTCLAIM_ASSIGNED);
	}
}

void CvInitCore::setType(GameType eType)
{
	PROFILE_EXTRA_FUNC();
	if (getType() != eType)
	{
		m_eType = eType;

		if (CvPlayerAI::areStaticsInitialized())
		{
			for (int i = 0; i < MAX_PC_PLAYERS; i++)
			{
				GET_PLAYER((PlayerTypes)i).updateHuman();
			}
		}
	}
}

void CvInitCore::setType(const CvWString & szType)
{
	if (wcsicmp(szType.GetCString(), L"singleplayer") == 0)
	{
		setType(GAME_SP_NEW);
	}
	else if (wcsicmp(szType.GetCString(), L"spload") == 0)
	{
		setType(GAME_SP_LOAD);
	}
	else
	{
		//FErrorMsg("Invalid game type in ini file!");
		setType(GAME_NONE);
	}
}

void CvInitCore::setMode(GameMode eMode)
{
	PROFILE_EXTRA_FUNC();
	if (getMode() != eMode)
	{
		m_eMode = eMode;

		if (CvPlayerAI::areStaticsInitialized())
		{
			for (int i = 0; i < MAX_PC_PLAYERS; i++)
			{
				GET_PLAYER((PlayerTypes)i).updateHuman();
			}
		}
	}
}


const CvWString & CvInitCore::getLeaderName(PlayerTypes eID, uint uiForm) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	m_szTemp = gDLL->getObjectText(CvString(m_aszLeaderName[eID]).GetCString(), uiForm, true);
	return m_szTemp;
}

void CvInitCore::setLeaderName(PlayerTypes eID, const CvWString & szLeaderName)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	CvWString szName = szLeaderName;
	gDLL->stripSpecialCharacters(szName);

	m_aszLeaderName[eID] = szName;
}

const CvWString & CvInitCore::getLeaderNameKey(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);

	return m_aszLeaderName[eID];
}

const CvWString & CvInitCore::getCivDescription(PlayerTypes eID, uint uiForm) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	m_szTemp = gDLL->getObjectText(CvString(m_aszCivDescription[eID]).GetCString(), uiForm, true);
	return m_szTemp;
}

void CvInitCore::setCivDescription(PlayerTypes eID, const CvWString & szCivDescription)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	CvWString szName = szCivDescription;
	gDLL->stripSpecialCharacters(szName);
	m_aszCivDescription[eID] = szName;
}

const CvWString & CvInitCore::getCivDescriptionKey(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	return m_aszCivDescription[eID];
}

const CvWString & CvInitCore::getCivShortDesc(PlayerTypes eID, uint uiForm) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);

	// Assume we have stored the key
	m_szTemp = gDLL->getObjectText(CvString(m_aszCivShortDesc[eID]).GetCString(), uiForm, true);
	return m_szTemp;
}

void CvInitCore::setCivShortDesc(PlayerTypes eID, const CvWString & szCivShortDesc)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	CvWString szName = szCivShortDesc;
	gDLL->stripSpecialCharacters(szName);
	m_aszCivShortDesc[eID] = szName;
}

const CvWString & CvInitCore::getCivShortDescKey(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	return m_aszCivShortDesc[eID];
}

const CvWString & CvInitCore::getCivAdjective(PlayerTypes eID, uint uiForm) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);

	// Assume we have stored the key
	m_szTemp = gDLL->getObjectText(CvString(m_aszCivAdjective[eID]).GetCString(), uiForm, true);
	return m_szTemp;
}

void CvInitCore::setCivAdjective(PlayerTypes eID, const CvWString & szCivAdjective)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	CvWString szName = szCivAdjective;
	gDLL->stripSpecialCharacters(szName);
	m_aszCivAdjective[eID] = szName;
}

const CvWString & CvInitCore::getCivAdjectiveKey(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	return m_aszCivAdjective[eID];
}

const CvWString & CvInitCore::getCivPassword(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	return m_aszCivPassword[eID];
}

void CvInitCore::setCivPassword(PlayerTypes eID, const CvWString & szCivPassword, bool bEncrypt)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	if (szCivPassword.empty() || !bEncrypt)
	{
		m_aszCivPassword[eID] = szCivPassword;
	}
	else
	{
		m_aszCivPassword[eID] = CvWString(gDLL->md5String((char*)CvString(szCivPassword).GetCString()));
	}
}


const CvString & CvInitCore::getEmail(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	return m_aszEmail[eID];
}

void CvInitCore::setEmail(PlayerTypes eID, const CvString & szEmail)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	m_aszEmail[eID] = szEmail;
}

const CvString & CvInitCore::getSmtpHost(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	return m_aszSmtpHost[eID];
}

void CvInitCore::setSmtpHost(PlayerTypes eID, const CvString & szHost)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	m_aszSmtpHost[eID] = szHost;
}


bool CvInitCore::getWhiteFlag(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	return m_abWhiteFlag[eID];
}

void CvInitCore::setWhiteFlag(PlayerTypes eID, bool bWhiteFlag)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	m_abWhiteFlag[eID] = bWhiteFlag;
}

const CvWString & CvInitCore::getFlagDecal(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	return m_aszFlagDecal[eID];
}

void CvInitCore::setFlagDecal(PlayerTypes eID, const CvWString & szFlagDecal)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	m_aszFlagDecal[eID] = szFlagDecal;
}


CivilizationTypes CvInitCore::getCiv(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	return m_aeCiv[eID];
}

void CvInitCore::setCiv(PlayerTypes eID, CivilizationTypes eCiv)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	m_aeCiv[eID] = eCiv;
}

LeaderHeadTypes CvInitCore::getLeader(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	return m_aeLeader[eID];
}

void CvInitCore::setLeader(PlayerTypes eID, LeaderHeadTypes eLeader)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	m_aeLeader[eID] = eLeader;
}

TeamTypes CvInitCore::getTeam(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	return m_aeTeam[eID];
}

void CvInitCore::setTeam(PlayerTypes eID, TeamTypes eTeam)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	if (getTeam(eID) != eTeam)
	{
		m_aeTeam[eID] = eTeam;

		if (CvPlayerAI::areStaticsInitialized())
		{
			GET_PLAYER(eID).updateTeamType();
		}
	}
}

HandicapTypes CvInitCore::getHandicap(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	return m_aeHandicap[eID];
}

void CvInitCore::setHandicap(PlayerTypes eID, HandicapTypes eHandicap)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	m_aeHandicap[eID] = eHandicap;
}

PlayerColorTypes CvInitCore::getColor(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	return m_aeColor[eID];
}

void CvInitCore::setColor(PlayerTypes eID, PlayerColorTypes eColor)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	m_aeColor[eID] = eColor;
}

ArtStyleTypes CvInitCore::getArtStyle(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	return m_aeArtStyle[eID];
}

void CvInitCore::setArtStyle(PlayerTypes eID, ArtStyleTypes eArtStyle)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	m_aeArtStyle[eID] = eArtStyle;
}


SlotStatus CvInitCore::getSlotStatus(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);

	return m_aeSlotStatus[eID] == SS_MAX_SLOT_STATUS ? SS_CLOSED : m_aeSlotStatus[eID];
}

void CvInitCore::setSlotStatus(PlayerTypes eID, SlotStatus eSlotStatus)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	if (m_aeSlotStatus[eID] != SS_MAX_SLOT_STATUS && m_aeSlotStatus[eID] != eSlotStatus)
	{
		m_aeSlotStatus[eID] = eSlotStatus;

		if (eID < MAX_PC_PLAYERS && CvPlayerAI::areStaticsInitialized())
		{
			GET_PLAYER(eID).updateHuman();
		}
	}
}

SlotClaim CvInitCore::getSlotClaim(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);

	return m_aeSlotClaim[eID] == NUM_SLOTCLAIMS ? SLOTCLAIM_UNASSIGNED : m_aeSlotClaim[eID];
}

void CvInitCore::setSlotClaim(PlayerTypes eID, SlotClaim eSlotClaim)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);

	if (m_aeSlotStatus[eID] != NUM_SLOTCLAIMS)
	{
		m_aeSlotClaim[eID] = eSlotClaim;
	}
}

bool CvInitCore::getReady(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	return m_abReady[eID];
}

void CvInitCore::setReady(PlayerTypes eID, bool bReady)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	m_abReady[eID] = bReady;
}


bool CvInitCore::getPlayableCiv(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	if (getWBMapScript() && !getWBMapNoPlayers())
	{
		return m_abPlayableCiv[eID];
	}

	if (getCiv(eID) != NO_CIVILIZATION)
	{
		return GC.getCivilizationInfo(getCiv(eID)).isPlayable();
	}

	// Don't allow people to play an NPC civ
	return (eID < MAX_PC_PLAYERS);
}

void CvInitCore::setPlayableCiv(PlayerTypes eID, bool bPlayableCiv)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	m_abPlayableCiv[eID] = bPlayableCiv;
}

bool CvInitCore::getMinorNationCiv(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	return m_abMinorNationCiv[eID];
}

void CvInitCore::setMinorNationCiv(PlayerTypes eID, bool bMinorNationCiv)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	m_abMinorNationCiv[eID] = bMinorNationCiv;
}

int CvInitCore::getNetID(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	return m_aiNetID[eID];
}

void CvInitCore::setNetID(PlayerTypes eID, int iNetID)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	m_aiNetID[eID] = iNetID;
}

const CvString & CvInitCore::getPythonCheck(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	return m_aszPythonCheck[eID];
}

void CvInitCore::setPythonCheck(PlayerTypes eID, const CvString & szPythonCheck)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	m_aszPythonCheck[eID] = szPythonCheck;
}

const CvString & CvInitCore::getXMLCheck(PlayerTypes eID) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	return m_aszXMLCheck[eID];
}

void CvInitCore::setXMLCheck(PlayerTypes eID, const CvString & szXMLCheck)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eID);
	m_aszXMLCheck[eID] = szXMLCheck;
}

void CvInitCore::setAdminPassword(const CvWString & szAdminPassword, bool bEncrypt)
{
	if (szAdminPassword.empty() || !bEncrypt)
	{
		m_szAdminPassword = szAdminPassword;
	}
	else
	{
		m_szAdminPassword = CvWString(gDLL->md5String((char*)CvString(szAdminPassword).GetCString()));
	}
}

void CvInitCore::resetAdvancedStartPoints()
{
	int iPoints = 0;

	if (NO_ERA != getEra())
	{
		iPoints += GC.getEraInfo(getEra()).getAdvancedStartPoints();
	}

	if (NO_WORLDSIZE != getWorldSize())
	{
		iPoints *= GC.getWorldInfo(getWorldSize()).getAdvancedStartPointsMod();
		iPoints /= 100;
	}

	if (NO_GAMESPEED != getGameSpeed())
	{
		iPoints *= GC.getGameSpeedInfo(getGameSpeed()).getSpeedPercent();
		iPoints /= 100;
	}

	setNumAdvancedStartPoints(iPoints);
}


void CvInitCore::read(FDataStreamBase* pStream)
{
	PROFILE_EXTRA_FUNC();
	uint uiSaveFlag=0;
	pStream->Read(&uiSaveFlag);		// flags for expansion (see SaveBits)

	//	Reset selection group static caches on load
	CvSelectionGroup::setGroupToCacheFor(NULL);

// BUG - Save Format - start
	bool bugSaveFlag = uiSaveFlag & BUG_DLL_SAVE_FORMAT;
	uiSaveFlag &= ~BUG_DLL_SAVE_FORMAT;
// RevolutionDCM - end BUG

	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	//	New save format or old?
	if ((uiSaveFlag & GAME_SAVE_UI_FLAG_VALUE_TAGGED_FORMAT) != 0)
	{
		wrapper.reset(true);
	}
	else
	{
		wrapper.reset(false);
	}

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	// SAVE_FORMAT_VERSION of the build that did the save
	int saveVersion;
	WRAPPER_READ_DECORATED(wrapper, "CvInitCore", &saveVersion, "SAVE_FORMAT_VERSION")
	// AlbertS2: Save file format version, can be use to make a new dll incompatible with older saves
	if (saveVersion != SAVE_FORMAT_VERSION)
	{
		const char* reason = CvString::format("Save format version is not compatible, version=(%d) expected version=(%d)!", saveVersion, SAVE_FORMAT_VERSION).c_str();

		::MessageBox(NULL, reason, "Unreadable Save Game!", MB_OK);
		throw std::invalid_argument(reason);
	}

	//	Asset checksum of the build that did the save
	WRAPPER_READ_DECORATED(wrapper, "CvInitCore", &m_uiSavegameAssetCheckSum, "m_uiSavegameAssetCheckSum");
	//OutputDebugString(CvString::format("Asset CheckSum READ %I32u = m_uiAssetCheckSum\n", m_uiAssetCheckSum).c_str());
	//OutputDebugString(CvString::format("Asset CheckSum READ %I32u = m_uiSavegameAssetCheckSum\n", m_uiSavegameAssetCheckSum).c_str());

	// GAME DATA
	WRAPPER_READ(wrapper, "CvInitCore", (int*)&m_eType);
	WRAPPER_READ_STRING(wrapper, "CvInitCore", m_szGameName);
	WRAPPER_READ_STRING(wrapper, "CvInitCore", m_szGamePassword);
	WRAPPER_READ_STRING(wrapper, "CvInitCore", m_szAdminPassword);
	WRAPPER_READ_STRING(wrapper, "CvInitCore", m_szMapScriptName);

	WRAPPER_READ(wrapper, "CvInitCore", &m_bWBMapNoPlayers);

	WRAPPER_READ(wrapper, "CvInitCore", (int*)&m_eWorldSize);
	WRAPPER_READ(wrapper, "CvInitCore", (int*)&m_eClimate);
	WRAPPER_READ(wrapper, "CvInitCore", (int*)&m_eSeaLevel);
	WRAPPER_READ(wrapper, "CvInitCore", (int*)&m_eEra);

	m_eGameSpeed = NO_GAMESPEED;
	WRAPPER_READ_CLASS_ENUM(wrapper, "CvInitCore", REMAPPED_CLASS_TYPE_GAMESPEEDS, (int*)&m_eGameSpeed);

	WRAPPER_READ(wrapper, "CvInitCore", (int*)&m_eTurnTimer);
	WRAPPER_READ(wrapper, "CvInitCore", (int*)&m_eCalendar);

	SAFE_DELETE_ARRAY(m_aeCustomMapOptions);
	WRAPPER_READ(wrapper, "CvInitCore", &m_iNumCustomMapOptions);
	WRAPPER_READ(wrapper, "CvInitCore", &m_iNumHiddenCustomMapOptions);
	if (m_iNumCustomMapOptions > 0)
	{
		m_aeCustomMapOptions = new CustomMapOptionTypes[m_iNumCustomMapOptions];
	}

	WRAPPER_READ_ARRAY(wrapper, "CvInitCore", m_iNumCustomMapOptions, (int*)m_aeCustomMapOptions);

	SAFE_DELETE_ARRAY(m_abVictories);
	WRAPPER_READ(wrapper, "CvInitCore", &m_iNumVictories);
	if (m_iNumVictories > 0)
	{
		m_abVictories = new bool[m_iNumVictories];
		WRAPPER_READ_ARRAY(wrapper, "CvInitCore", m_iNumVictories, m_abVictories);
	}

	if (bugSaveFlag)
	{
		// read and ignore number of game options as it's only for external tools
		int iNumGameOptions = 0;
		WRAPPER_READ_DECORATED(wrapper, "CvInitCore", &iNumGameOptions, "GC.getNumGameOptionInfos()");
	}

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 11/30/07                                MRGENIE      */
/*                                                                                              */
/* Savegame compatibility                                                                       */
/************************************************************************************************/
	// Set options to default values to handle cases of loading games that pre-dated an added otpion
	setDefaults();

	WRAPPER_READ_CLASS_ARRAY_ALLOW_MISSING(wrapper, "CvInitCore", REMAPPED_CLASS_TYPE_GAMEOPTIONS, GC.getNumGameOptionInfos(), m_abOptions);
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
	WRAPPER_READ_CLASS_ARRAY(wrapper, "CvInitCore", REMAPPED_CLASS_TYPE_MPOPTIONS, NUM_MPOPTION_TYPES, m_abMPOptions);

	WRAPPER_READ(wrapper, "CvInitCore", &m_bStatReporting);

	WRAPPER_READ(wrapper, "CvInitCore", &m_iGameTurn);
	WRAPPER_READ(wrapper, "CvInitCore", &m_iMaxTurns);
	WRAPPER_READ(wrapper, "CvInitCore", &m_iPitbossTurnTime);
	WRAPPER_READ(wrapper, "CvInitCore", &m_iTargetScore);

	WRAPPER_READ(wrapper, "CvInitCore", &m_iMaxCityElimination);
	WRAPPER_READ(wrapper, "CvInitCore", &m_iNumAdvancedStartPoints);

	// PLAYER DATA
	WRAPPER_READ_STRING_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_aszLeaderName);
	WRAPPER_READ_STRING_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_aszCivDescription);
	WRAPPER_READ_STRING_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_aszCivShortDesc);
	WRAPPER_READ_STRING_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_aszCivAdjective);
	WRAPPER_READ_STRING_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_aszCivPassword);
	WRAPPER_READ_STRING_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_aszEmail);
	WRAPPER_READ_STRING_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_aszSmtpHost);

	WRAPPER_READ_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_abWhiteFlag);
	WRAPPER_READ_STRING_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_aszFlagDecal);

	WRAPPER_READ_CLASS_ENUM_ARRAY_ALLOW_MISSING(wrapper, "CvInitCore", REMAPPED_CLASS_TYPE_CIVILIZATIONS, MAX_PLAYERS, (int*)m_aeCiv);
	WRAPPER_READ_CLASS_ENUM_ARRAY_ALLOW_MISSING(wrapper, "CvInitCore", REMAPPED_CLASS_TYPE_LEADERHEADS, MAX_PLAYERS, (int*)m_aeLeader);

	WRAPPER_READ_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, (int*)m_aeTeam);

	WRAPPER_READ_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, (int*)m_aeHandicap);
	WRAPPER_READ_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, (int*)m_aeColor);
	WRAPPER_READ_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, (int*)m_aeArtStyle);

	WRAPPER_READ_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, (int*)m_aeSlotStatus);
	WRAPPER_READ_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, (int*)m_aeSlotClaim);

	WRAPPER_READ_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_abPlayableCiv);
	WRAPPER_READ_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_abMinorNationCiv);

	//	Special case for LEADERHEADS - if one that is in use is no longer defined we just warn
	//	and use another one from the same Civ
	//	Similarly if no Civ yet the slot status implies there should be assign a new one
	if (wrapper.isUsingTaggedFormat())
	{
		bool bWarned = false;

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			if (m_aeColor[i] >= GC.getNumPlayerColorInfos())
			{
				for (int j = 0; j < GC.getNumPlayerColorInfos(); j++)
				{
					bool colorUsed = false;

					for (int k = 0; k < MAX_PLAYERS; k++)
					{
						if (m_aeColor[k] == (PlayerColorTypes)j)
						{
							colorUsed = true;
							break;
						}
					}

					if (!colorUsed)
					{
						m_aeColor[i] = (PlayerColorTypes)j;
						break;
					}
				}
			}

			if (m_aeCiv[i] == NO_CIVILIZATION && (m_aeSlotStatus[i] == SS_TAKEN || m_aeSlotStatus[i] == SS_COMPUTER || m_aeColor[i] != NO_PLAYERCOLOR))
			{
				//	Must have been removed in an asset change - assign a new one from those
				//	not currently in use
				for (int j = 0; j < GC.getNumCivilizationInfos(); j++)
				{
					//	Game structures not yet loaded so have to search just via the arrays loaded here
					bool civUsed = false;

					for (int k = 0; k < MAX_PLAYERS; k++)
					{
						if (m_aeSlotStatus[i] == SS_TAKEN || m_aeSlotStatus[i] == SS_COMPUTER)
						{
							if (m_aeCiv[k] == j)
							{
								civUsed = true;
								break;
							}
						}
					}

					if (!civUsed)
					{
						m_aeCiv[i] = (CivilizationTypes)j;
						m_aeLeader[i] = NO_LEADER;
						m_aeColor[i] = (PlayerColorTypes)GC.getCivilizationInfo((CivilizationTypes)j).getDefaultPlayerColor();
						m_aeArtStyle[i] = (ArtStyleTypes)GC.getCivilizationInfo((CivilizationTypes)j).getArtStyleType();
						m_aszFlagDecal[i] = (CvWString)GC.getCivilizationInfo((CivilizationTypes)j).getFlagTexture();
						wrapper.warning(CvString::format("An in-use civilization is no longer defined.  Using civilization %S in its place", GC.getCivilizationInfo((CivilizationTypes)j).getDescription()));
						break;
					}
				}
			}

			if (m_aeCiv[i] != NO_CIVILIZATION && m_aeLeader[i] == NO_LEADER)
			{
				bool bNoSubstitute = true;

				for (int j = 0; j < GC.getNumLeaderHeadInfos(); ++j)
				{
					if (GC.getCivilizationInfo(m_aeCiv[i]).isLeaders(j))
					{
						bNoSubstitute = false;
						if (!bWarned)
						{
							const CvLeaderHeadInfo& leader = GC.getLeaderHeadInfo((LeaderHeadTypes)j);
							wrapper.warning(CvString::format("An in-use leader is no longer defined.  Using leader %S in its place (others may also have been replaced)", leader.getDescription()));
							bWarned = true;
						}
						m_aeLeader[i] = (LeaderHeadTypes)j;
						break;
					}
				}
				if (bNoSubstitute)
				{
					wrapper.error(CvString::format("An in-use leader is no longer defined and no subsitute can be found"));
				}
			}
		}
	}

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (m_aeSlotClaim[i] == SLOTCLAIM_ASSIGNED)
		{
			m_aeSlotClaim[i] = SLOTCLAIM_RESERVED;
		}
	}

	if (CvPlayerAI::areStaticsInitialized())
	{
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			GET_PLAYER((PlayerTypes)i).updateHuman();
			GET_PLAYER((PlayerTypes) i).updateTeamType();
		}
	}
	WRAPPER_READ_OBJECT_END(wrapper);
}


void CvInitCore::write(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	//	Need to clear signs across a save to stop the game engine persisting them, since it only sees
	//	those within the current viewport (and in viewport coordinates).  They are actually persisted via
	//	Python anyway
	GC.clearSigns();
	GC.getGame().processGreatWall(false);
	GC.getCurrentViewport()->setActionState(VIEWPORT_ACTION_STATE_SAVING);

	/*
	uint uiSaveFlag=1;
	*/
	uint uiSaveFlag=0;

// BUG - Save Format - start
	// If any optional mod alters the number of game options or save format in any way,
	// set the BUG save format bit and write out the number of game options later.
	uiSaveFlag |= BUG_DLL_SAVE_FORMAT;
// BUG - Save Format - end

	if ( wrapper.m_requestUseTaggedFormat || GC.getDefineINT("ALWAYS_USE_MAX_COMPAT_SAVES") )
	{
		uiSaveFlag |= GAME_SAVE_UI_FLAG_VALUE_TAGGED_FORMAT;

		wrapper.reset(true);
	}
	else
	{
		wrapper.reset(false);
	}

	pStream->Write(uiSaveFlag);		// flag for expansion, see SaveBits)

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE_DECORATED(wrapper, "CvInitCore", (int)SAVE_FORMAT_VERSION, "SAVE_FORMAT_VERSION");

	// record the asset checksum of the build doing the save
	WRAPPER_WRITE_DECORATED(wrapper, "CvInitCore", m_uiAssetCheckSum, "m_uiSavegameAssetCheckSum");
	//OutputDebugString(CvString::format("Asset CheckSum WRITE:\n%I32u = m_uiAssetCheckSum\n%I32u = m_uiSavegameAssetCheckSum", m_uiAssetCheckSum, m_uiSavegameAssetCheckSum).c_str());

	// GAME DATA
	WRAPPER_WRITE(wrapper, "CvInitCore", m_eType);
	WRAPPER_WRITE_STRING(wrapper, "CvInitCore", m_szGameName);
	WRAPPER_WRITE_STRING(wrapper, "CvInitCore", m_szGamePassword);
	WRAPPER_WRITE_STRING(wrapper, "CvInitCore", m_szAdminPassword);
	WRAPPER_WRITE_STRING(wrapper, "CvInitCore", m_szMapScriptName);

	WRAPPER_WRITE(wrapper, "CvInitCore", m_bWBMapNoPlayers);

	WRAPPER_WRITE(wrapper, "CvInitCore", m_eWorldSize);
	WRAPPER_WRITE(wrapper, "CvInitCore", m_eClimate);
	WRAPPER_WRITE(wrapper, "CvInitCore", m_eSeaLevel);
	WRAPPER_WRITE(wrapper, "CvInitCore", m_eEra);
	WRAPPER_WRITE_CLASS_ENUM(wrapper, "CvInitCore", REMAPPED_CLASS_TYPE_GAMESPEEDS, m_eGameSpeed);
	WRAPPER_WRITE(wrapper, "CvInitCore", m_eTurnTimer);
	WRAPPER_WRITE(wrapper, "CvInitCore", m_eCalendar);

	WRAPPER_WRITE(wrapper, "CvInitCore", m_iNumCustomMapOptions);
	WRAPPER_WRITE(wrapper, "CvInitCore", m_iNumHiddenCustomMapOptions);
	WRAPPER_WRITE_ARRAY(wrapper, "CvInitCore", m_iNumCustomMapOptions, (int*)m_aeCustomMapOptions);

	WRAPPER_WRITE(wrapper, "CvInitCore", m_iNumVictories);
	WRAPPER_WRITE_ARRAY(wrapper, "CvInitCore", m_iNumVictories, m_abVictories);

// BUG - Save Format - start
	// write out the number of game options for the external parser tool
	WRAPPER_WRITE(wrapper, "CvInitCore", GC.getNumGameOptionInfos());
// BUG - Save Format - end

	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvInitCore", REMAPPED_CLASS_TYPE_GAMEOPTIONS, GC.getNumGameOptionInfos(), m_abOptions);
	WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvInitCore", REMAPPED_CLASS_TYPE_MPOPTIONS, NUM_MPOPTION_TYPES, m_abMPOptions);

	WRAPPER_WRITE(wrapper, "CvInitCore", m_bStatReporting);

	WRAPPER_WRITE(wrapper, "CvInitCore", m_iGameTurn);
	WRAPPER_WRITE(wrapper, "CvInitCore", m_iMaxTurns);
	WRAPPER_WRITE(wrapper, "CvInitCore", m_iPitbossTurnTime);
	WRAPPER_WRITE(wrapper, "CvInitCore", m_iTargetScore);

	WRAPPER_WRITE(wrapper, "CvInitCore", m_iMaxCityElimination);
	WRAPPER_WRITE(wrapper, "CvInitCore", m_iNumAdvancedStartPoints);

	// PLAYER DATA
	WRAPPER_WRITE_STRING_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_aszLeaderName);
	WRAPPER_WRITE_STRING_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_aszCivDescription);
	WRAPPER_WRITE_STRING_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_aszCivShortDesc);
	WRAPPER_WRITE_STRING_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_aszCivAdjective);
	WRAPPER_WRITE_STRING_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_aszCivPassword);
	WRAPPER_WRITE_STRING_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_aszEmail);
	WRAPPER_WRITE_STRING_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_aszSmtpHost);

	WRAPPER_WRITE_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_abWhiteFlag);
	WRAPPER_WRITE_STRING_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_aszFlagDecal);

	WRAPPER_WRITE_CLASS_ENUM_ARRAY(wrapper, "CvInitCore", REMAPPED_CLASS_TYPE_CIVILIZATIONS, MAX_PLAYERS, (int*)m_aeCiv);
	WRAPPER_WRITE_CLASS_ENUM_ARRAY(wrapper, "CvInitCore",REMAPPED_CLASS_TYPE_LEADERHEADS,  MAX_PLAYERS, (int*)m_aeLeader);
	WRAPPER_WRITE_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, (int*)m_aeTeam);
	WRAPPER_WRITE_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, (int*)m_aeHandicap);
	WRAPPER_WRITE_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, (int*)m_aeColor);
	WRAPPER_WRITE_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, (int*)m_aeArtStyle);

	WRAPPER_WRITE_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, (int*)m_aeSlotStatus);
	WRAPPER_WRITE_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, (int*)m_aeSlotClaim);

	WRAPPER_WRITE_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_abPlayableCiv);
	WRAPPER_WRITE_ARRAY(wrapper, "CvInitCore", MAX_PLAYERS, m_abMinorNationCiv);

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

// BUG - EXE/DLL Paths - start
CvString CvInitCore::getDLLPath() const
{
	setPathNames();
	return *dllPath;
}

CvString CvInitCore::getDLLName() const
{
	setPathNames();
	return *dllName;
}

CvString CvInitCore::getExePath() const
{
	setPathNames();
	return *exePath;
}

CvString CvInitCore::getExeName() const
{
	setPathNames();
	return *exeName;
}

extern HANDLE dllModule;
void CvInitCore::setPathNames()
{
	PROFILE_EXTRA_FUNC();
	if (bPathsSet)
	{
		return;
	}

	//	Alloctae path cache strings
	dllPath = new CvString();
	dllName = new CvString();
	exePath = new CvString();
	exeName = new CvString();

	char pathBuffer[4096];

	DWORD result = GetModuleFileName(NULL, pathBuffer, sizeof(pathBuffer));
	char* pos = strchr(pathBuffer, '\\');
	while (pos != NULL && *pos != NULL)
	{
		char* next = strchr(pos + 1, '\\');
		if (!next)
		{
			*pos = 0;
			*exePath = pathBuffer;
			*exeName = pos + 1;
		}
		pos = next;
	}

	result = GetModuleFileName((HMODULE)dllModule, pathBuffer, sizeof(pathBuffer));
	pos = strchr(pathBuffer, '\\');
	while (pos != NULL && *pos != NULL)
	{
		char* next = strchr(pos + 1, '\\');
		if (!next)
		{
			*pos = 0;
			*dllPath = pathBuffer;
			*dllName = pos + 1;
		}
		pos = next;
	}

	bPathsSet = true;
}
// BUG - EXE/DLL Paths - end


// Afforess - 01/12/10
void CvInitCore::reassignPlayerAdvanced(PlayerTypes eOldID, PlayerTypes eNewID)
{
	PROFILE_EXTRA_FUNC();
	if ( checkBounds(eOldID, 0, MAX_PC_PLAYERS) && checkBounds(eNewID, 0, MAX_PC_PLAYERS) )
	{
		// *** SAVE TARGET SLOT DETAILS TEMPORARILY
		// Temp civ details
		CvString szEmail = m_aszEmail[eNewID];
		CvString szSmtpHost = m_aszSmtpHost[eNewID];
		HandicapTypes eHandicap = m_aeHandicap[eNewID];
		SlotStatus eSlotStatus = m_aeSlotStatus[eNewID];
		SlotClaim eSlotClaim = m_aeSlotClaim[eNewID];
		bool bPlayableCiv = m_abPlayableCiv[eNewID];
		int iNetID = m_aiNetID[eNewID];
		bool bReady = m_abReady[eNewID];
		CvString szPythonCheck = m_aszPythonCheck[eNewID];
		CvString szXMLCheck = m_aszXMLCheck[eNewID];

		m_aszEmail[eNewID] = m_aszEmail[eOldID];
		m_aszSmtpHost[eNewID] = m_aszSmtpHost[eOldID];
		m_aeHandicap[eNewID] = m_aeHandicap[eOldID];
		setSlotStatus(eNewID, m_aeSlotStatus[eOldID]);
		m_aeSlotClaim[eNewID] = m_aeSlotClaim[eOldID];
		m_abPlayableCiv[eNewID] = m_abPlayableCiv[eOldID];
		m_aiNetID[eNewID] = m_aiNetID[eOldID];
		m_abReady[eNewID] = m_abReady[eOldID];
		m_aszPythonCheck[eNewID] = m_aszPythonCheck[eOldID];
		m_aszXMLCheck[eNewID] = m_aszXMLCheck[eOldID];

		m_aszEmail[eOldID] = szEmail;
		m_aszSmtpHost[eOldID] = szSmtpHost;
		m_aeHandicap[eOldID] = eHandicap;
		setSlotStatus(eOldID, eSlotStatus);
		m_aeSlotClaim[eOldID] = eSlotClaim;
		m_abPlayableCiv[eOldID] = bPlayableCiv;
		m_aiNetID[eOldID] = iNetID;
		m_abReady[eOldID] = bReady;
		m_aszPythonCheck[eOldID] = szPythonCheck;
		m_aszXMLCheck[eOldID] = szXMLCheck;

		for (int iI = 0; iI < NUM_PLAYEROPTION_TYPES; iI++)
		{
			GET_PLAYER(eNewID).setOption((PlayerOptionTypes)iI,GET_PLAYER(eOldID).isOption((PlayerOptionTypes)iI));
			GET_PLAYER(eOldID).setOption((PlayerOptionTypes)iI,false);
		}

		for (int iI = 0; iI < NUM_MODDEROPTION_TYPES; iI++)
		{
			GET_PLAYER(eNewID).setModderOption((ModderOptionTypes)iI,GET_PLAYER(eOldID).getModderOption((ModderOptionTypes)iI));
			GET_PLAYER(eOldID).setModderOption((ModderOptionTypes)iI,0);
		}

		// We may have a new active player id...
		if (getActivePlayer() == eOldID)
		{
			GC.getGame().setActivePlayer(eNewID);
		}
		else if (getActivePlayer() == eNewID)
		{
			setActivePlayer(eOldID);
		}

		if(CvPlayerAI::areStaticsInitialized())
		{
			GET_PLAYER(eOldID).updateTeamType();
			GET_PLAYER(eNewID).updateTeamType();
			GET_PLAYER(eOldID).updateHuman();
			GET_PLAYER(eNewID).updateHuman();
		}
	}
}
// ! Afforess


void CvInitCore::calculateAssetCheckSum()
{
	m_uiAssetCheckSum = GC.getAssetCheckSum();
	/*
	OutputDebugString(
		CvString::format(
			"Asset CheckSum calculateAssetCheckSum:\n%I32u = m_uiAssetCheckSum\n%I32u = m_uiSavegameAssetCheckSum",
			m_uiAssetCheckSum, m_uiSavegameAssetCheckSum
		)
		.c_str()
	);
	*/
}

void CvInitCore::checkVersions()
{
	/*
	OutputDebugString(
		CvString::format(
			"Asset CheckSum checkVersions:\n%I32u = m_uiAssetCheckSum\n%I32u = m_uiSavegameAssetCheckSum",
			m_uiAssetCheckSum, m_uiSavegameAssetCheckSum
		)
		.c_str()
	);
	*/
	// If assets changed
	if (m_uiSavegameAssetCheckSum != m_uiAssetCheckSum)
	{
		const PlayerTypes ePlayer = GC.getGame().getActivePlayer();
		// DLL or assets changed, recommend modifier reloading
		if (NO_PLAYER != ePlayer && GET_PLAYER(ePlayer).isAlive() && GET_PLAYER(ePlayer).isHumanPlayer())
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_MODIFIER_RECALCULATION);
			if (NULL != pInfo)
			{
				gDLL->getInterfaceIFace()->addPopup(pInfo, ePlayer, false, true);
			}
		}
	}
}