//
// globals.cpp
//
#include "CvGameCoreDLL.h"
#include "CvMapExternal.h"
#include <time.h> 
#include <sstream>

static char gVersionString[1024] = { 0 };

// Use macro override when available. Version string might not be loaded in time for
// applying it to the mini-dump so we will use macro version string for releases
#ifndef C2C_VERSION
#	define C2C_VERSION gVersionString
#endif

#define COPY(dst, src, typeName) \
	{ \
		int iNum = sizeof(src)/sizeof(typeName); \
		dst = new typeName[iNum]; \
		for (int i =0;i<iNum;i++) \
			dst[i] = src[i]; \
	}

template <class T>
void deleteInfoArray(std::vector<T*>& array)
{
	for (std::vector<T*>::iterator it = array.begin(); it != array.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	array.clear();
}

template <class T>
bool readInfoArray(FDataStreamBase* pStream, std::vector<T*>& array, const char* szClassName)
{
	GC.addToInfosVectors(&array);

	int iSize;
	pStream->Read(&iSize);
	FAssertMsg(iSize==sizeof(T), CvString::format("class size doesn't match cache size - check info read/write functions:%s", szClassName).c_str());
	if (iSize!=sizeof(T))
		return false;
	pStream->Read(&iSize);

	deleteInfoArray(array);

	for (int i = 0; i < iSize; ++i)
	{
		array.push_back(new T);
	}

	int iIndex = 0;
	for (std::vector<T*>::iterator it = array.begin(); it != array.end(); ++it)
	{
		(*it)->read(pStream);
		GC.setInfoTypeFromString((*it)->getType(), iIndex);
		++iIndex;
	}

	return true;
}

template <class T>
bool writeInfoArray(FDataStreamBase* pStream,  std::vector<T*>& array)
{
	int iSize = sizeof(T);
	pStream->Write(iSize);
	pStream->Write(array.size());
	for (std::vector<T*>::iterator it = array.begin(); it != array.end(); ++it)
	{
		(*it)->write(pStream);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

CvGlobals gGlobalsProxy;	// for debugging
cvInternalGlobals* gGlobals = NULL;
CvDLLUtilityIFaceBase* g_DLL = NULL;

int g_iPercentDefault = 100;
int g_iModifierDefault = 0;

#ifdef _DEBUG
int inDLL = 0;
const char* fnName = NULL;

//	Wrapper for debugging so as to be able to always tell last method entered
ProxyTracker::ProxyTracker(const CvGlobals* proxy, const char* name)
{
	inDLL++;
	fnName = name;

	proxy->CheckProxy(name);
}

ProxyTracker::~ProxyTracker()
{
	inDLL--;
	fnName = NULL;
}
#endif

void CvGlobals::CheckProxy(const char* fnName) const
{
	//OutputDebugString(fnName);
	//OutputDebugString("\n");

	if ( gGlobals == NULL )
	{
		OutputDebugString("Method called prior to global instantiation\n");

		::MessageBoxA(NULL,"Method called prior to global instantiation",":CvGameCore",MB_OK);
		//throw new <exception>;
	}
}

//
// CONSTRUCTOR
//
cvInternalGlobals::cvInternalGlobals() 
/************************************************************************************************/
/* Mod Globals    Start                          09/13/10                           phungus420  */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	: m_paszEntityEventTypes2(NULL)
	, m_paszEntityEventTypes(NULL)
	, m_paszAnimationOperatorTypes(NULL)
	, m_paszFunctionTypes(NULL)
	, m_paszFlavorTypes(NULL)
	, m_paszArtStyleTypes(NULL)
	, m_paszCitySizeTypes(NULL)
	, m_paszContactTypes(NULL)
	, m_paszDiplomacyPowerTypes(NULL)
	, m_paszAutomateTypes(NULL)
	, m_paszDirectionTypes(NULL)
	, m_paszFootstepAudioTypes(NULL)
	, m_paszFootstepAudioTags(NULL)
	, m_bDCM_BATTLE_EFFECTS(false)
	, m_iBATTLE_EFFECT_LESS_FOOD(0)
	, m_iBATTLE_EFFECT_LESS_PRODUCTION(0)
	, m_iBATTLE_EFFECT_LESS_COMMERCE(0)
	, m_iBATTLE_EFFECTS_MINIMUM_TURN_INCREMENTS(0)
	, m_iMAX_BATTLE_TURNS(0)

	, m_bDCM_AIR_BOMBING(false)
	, m_bDCM_RANGE_BOMBARD(false)
	, m_iDCM_RB_CITY_INACCURACY(0)
	, m_iDCM_RB_CITYBOMBARD_CHANCE(0)
	, m_bDCM_ATTACK_SUPPORT(false)
	, m_bDCM_OPP_FIRE(false)
	, m_bDCM_ACTIVE_DEFENSE(false)
	, m_bDCM_FIGHTER_ENGAGE(false)

	, m_bDYNAMIC_CIV_NAMES(false)

	, m_bLIMITED_RELIGIONS_EXCEPTIONS(false)
	, m_bOC_RESPAWN_HOLY_CITIES(false)

	, m_bIDW_ENABLED(false)
	, m_fIDW_BASE_COMBAT_INFLUENCE(0)
	, m_fIDW_NO_CITY_DEFENDER_MULTIPLIER(1.0f)
	, m_fIDW_FORT_CAPTURE_MULTIPLIER(1.0f)
	, m_fIDW_EXPERIENCE_FACTOR(0)
	, m_fIDW_WARLORD_MULTIPLIER(1.0f)
	, m_iIDW_INFLUENCE_RADIUS(0)
	, m_fIDW_PLOT_DISTANCE_FACTOR(0)
	, m_fIDW_WINNER_PLOT_MULTIPLIER(1.0f)
	, m_fIDW_LOSER_PLOT_MULTIPLIER(1.0f)
	, m_bIDW_EMERGENCY_DRAFT_ENABLED(false)
	, m_iIDW_EMERGENCY_DRAFT_MIN_POPULATION(2)
	, m_fIDW_EMERGENCY_DRAFT_STRENGTH(1.0f)
	, m_fIDW_EMERGENCY_DRAFT_ANGER_MULTIPLIER(0)
	, m_bIDW_NO_BARBARIAN_INFLUENCE(false)
	, m_bIDW_NO_NAVAL_INFLUENCE(false)
	, m_bIDW_PILLAGE_INFLUENCE_ENABLED(false)
	, m_fIDW_BASE_PILLAGE_INFLUENCE(0)
	, m_fIDW_CITY_TILE_MULTIPLIER(0)

	, m_bSS_ENABLED(false)
	, m_bSS_BRIBE(false)
	, m_bSS_ASSASSINATE(false)
	/************************************************************************************************/
	/* Mod Globals                        END                                           phungus420  */
	/************************************************************************************************/
	, m_bGraphicsInitialized(false)
	, m_bLogging(false)
	, m_bRandLogging(false)
	, m_bOverwriteLogs(false)
	, m_bSynchLogging(false)
	, m_bDLLProfiler(false)
	, m_pkMainMenu(NULL)
	, m_iNewPlayers(0)
	, m_bZoomOut(false)
	, m_bZoomIn(false)
	, m_bLoadGameFromFile(false)
	, m_pFMPMgr(NULL)
	, m_asyncRand(NULL)
	, m_interface(NULL)
	, m_game(NULL)
	, m_messageQueue(NULL)
	, m_hotJoinMsgQueue(NULL)
	, m_messageControl(NULL)
	, m_messageCodes(NULL)
	, m_dropMgr(NULL)
	, m_portal(NULL)
	, m_setupData(NULL)
	, m_initCore(NULL)
	, m_statsReporter(NULL)
	, m_diplomacyScreen(NULL)
	, m_mpDiplomacyScreen(NULL)
	, m_pathFinder(NULL)
	, m_interfacePathFinder(NULL)
	, m_stepFinder(NULL)
	, m_routeFinder(NULL)
	, m_borderFinder(NULL)
	, m_areaFinder(NULL)
	, m_plotGroupFinder(NULL)
	, m_aiPlotDirectionX(NULL)
	, m_aiPlotDirectionY(NULL)
	, m_aiPlotCardinalDirectionX(NULL)
	, m_aiPlotCardinalDirectionY(NULL)
	, m_aiCityPlotX(NULL)
	, m_aiCityPlotY(NULL)
	, m_aiCityPlotPriority(NULL)
	, m_aeTurnLeftDirection(NULL)
	, m_aeTurnRightDirection(NULL)
	//, m_aGameOptionsInfo(NULL)
	//, m_aPlayerOptionsInfo(NULL)
	, m_Profiler(NULL)
	, m_VarSystem(NULL)
	, m_iMOVE_DENOMINATOR(0)
	, m_iNUM_UNIT_PREREQ_OR_BONUSES(0)
	, m_iNUM_BUILDING_PREREQ_OR_BONUSES(0)
	, m_iFOOD_CONSUMPTION_PER_POPULATION(0)
	, m_iMAX_HIT_POINTS(0)
	, m_iPATH_DAMAGE_WEIGHT(0)
	, m_iHILLS_EXTRA_DEFENSE(0)
	, m_iRIVER_ATTACK_MODIFIER(0)
	, m_iAMPHIB_ATTACK_MODIFIER(0)
	, m_iHILLS_EXTRA_MOVEMENT(0)
	, m_iRIVER_EXTRA_MOVEMENT(0)
	, m_iMAX_PLOT_LIST_ROWS(0)
	, m_iUNIT_MULTISELECT_MAX(0)
	, m_iPERCENT_ANGER_DIVISOR(0)
	, m_iEVENT_MESSAGE_TIME(0)
	, m_iROUTE_FEATURE_GROWTH_MODIFIER(0)
	, m_iFEATURE_GROWTH_MODIFIER(0)
	, m_iMIN_CITY_RANGE(0)
	, m_iCITY_MAX_NUM_BUILDINGS(0)
	, m_iNUM_UNIT_AND_TECH_PREREQS(0)
	, m_iNUM_AND_TECH_PREREQS(0)
	, m_iNUM_OR_TECH_PREREQS(0)
	, m_iLAKE_MAX_AREA_SIZE(0)
	, m_iNUM_ROUTE_PREREQ_OR_BONUSES(0)
	, m_iNUM_BUILDING_AND_TECH_PREREQS(0)
	, m_iMIN_WATER_SIZE_FOR_OCEAN(0)
	, m_iFORTIFY_MODIFIER_PER_TURN(0)
	, m_iESTABLISH_MODIFIER_PER_TURN(0)
	, m_iESCAPE_MODIFIER_PER_TURN(0)
	, m_iMAX_CITY_DEFENSE_DAMAGE(0)
	, m_iNUM_CORPORATION_PREREQ_BONUSES(0)
	, m_iPEAK_SEE_THROUGH_CHANGE(0)
	, m_iHILLS_SEE_THROUGH_CHANGE(0)
	, m_iSEAWATER_SEE_FROM_CHANGE(0)
	, m_iPEAK_SEE_FROM_CHANGE(0)
	, m_iHILLS_SEE_FROM_CHANGE(0)
	, m_iUSE_SPIES_NO_ENTER_BORDERS(0)
	, m_fCAMERA_MIN_YAW(0)
	, m_fCAMERA_MAX_YAW(0)
	, m_fCAMERA_FAR_CLIP_Z_HEIGHT(0)
	, m_fCAMERA_MAX_TRAVEL_DISTANCE(0)
	, m_fCAMERA_START_DISTANCE(0)
	, m_fAIR_BOMB_HEIGHT(0)
	, m_fPLOT_SIZE(0)
	, m_fCAMERA_SPECIAL_PITCH(0)
	, m_fCAMERA_MAX_TURN_OFFSET(0)
	, m_fCAMERA_MIN_DISTANCE(0)
	, m_fCAMERA_UPPER_PITCH(0)
	, m_fCAMERA_LOWER_PITCH(0)
	, m_fFIELD_OF_VIEW(0)
	, m_fSHADOW_SCALE(0)
	, m_fUNIT_MULTISELECT_DISTANCE(0)
	, m_fSAD_FACTOR_1(0)
	, m_fSAD_FACTOR_2(0)
	, m_fSAD_FACTOR_3(0)
	, m_fSAD_FACTOR_4(0)
	, m_iUSE_CANNOT_FOUND_CITY_CALLBACK(0)
	, m_iUSE_CAN_FOUND_CITIES_ON_WATER_CALLBACK(0)
	, m_iUSE_IS_PLAYER_RESEARCH_CALLBACK(0)
	, m_iUSE_CAN_RESEARCH_CALLBACK(0)
	, m_iUSE_CANNOT_DO_CIVIC_CALLBACK(0)
	, m_iUSE_CAN_DO_CIVIC_CALLBACK(0)
	, m_iUSE_CANNOT_CONSTRUCT_CALLBACK(0)
	, m_iUSE_CAN_CONSTRUCT_CALLBACK(0)
	, m_iUSE_CAN_DECLARE_WAR_CALLBACK(0)
	, m_iUSE_CANNOT_RESEARCH_CALLBACK(0)
	, m_iUSE_GET_UNIT_COST_MOD_CALLBACK(0)
	, m_iUSE_GET_CITY_FOUND_VALUE_CALLBACK(0)
	, m_iUSE_CANNOT_HANDLE_ACTION_CALLBACK(0)
	, m_iUSE_CAN_BUILD_CALLBACK(0)
	, m_iUSE_CANNOT_TRAIN_CALLBACK(0)
	, m_iUSE_CAN_TRAIN_CALLBACK(0)
	, m_iUSE_UNIT_CANNOT_MOVE_INTO_CALLBACK(0)
	, m_iUSE_USE_CANNOT_SPREAD_RELIGION_CALLBACK(0)
	, m_iUSE_FINISH_TEXT_CALLBACK(0)
	, m_iUSE_ON_UNIT_SET_XY_CALLBACK(0)
	, m_iUSE_ON_UNIT_SELECTED_CALLBACK(0)
	, m_iUSE_ON_UPDATE_CALLBACK(0)
	, m_iUSE_ON_UNIT_CREATED_CALLBACK(0)
	, m_iUSE_ON_UNIT_LOST_CALLBACK(0)
	, m_iLAND_UNITS_CAN_ATTACK_WATER_CITIES(0)
	, m_iBASE_UNIT_UPGRADE_COST(0)
	, m_iUPGRADE_ROUND_LIMIT(0)
	, m_iCITY_BARBARIAN_DEFENSE_MODIFIER(0)
	, m_iUNIT_VISIBILITY_RANGE(0)
	, m_iMAX_UNIT_VISIBILITY_RANGE(0)
	, m_iGREATER_COMMERCE_SWITCH_POINT(0)
	, m_iWORKER_TRADE_VALUE_PERCENT_ADJUSTMENT(0)
	, m_iTRADE_MISSION_END_TOTAL_PERCENT_ADJUSTMENT(0)
	, m_iINFILTRATE_MISSION_END_TOTAL_PERCENT_ADJUSTMENT(0)
	, m_iESPIONAGE_MISSION_COST_END_TOTAL_PERCENT_ADJUSTMENT(0)
	, m_iWATER_POTENTIAL_CITY_WORK_FOR_AREA(0)
	, m_iSAD_MAX_MODIFIER(0)
	, m_iUPSCALED_RESEARCH_COST_MODIFIER(0)
	, m_iENABLE_DYNAMIC_UNIT_ENTITIES(0)
	/************************************************************************************************/
	/* MODULES                                 11/13/07                            MRGENIE          */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/
	, m_iTGA_RELIGIONS(0)                            // GAMEFONT_TGA_RELIGIONS
	, m_iTGA_CORPORATIONS(0)                         // GAMEFONT_TGA_CORPORATIONS
	/************************************************************************************************/
	/* MODULES                                 END                                                  */
	/************************************************************************************************/
	, m_bMultimapsEnabled(false)
	, m_bViewportsEnabled(false)
	, m_iViewportFocusBorder(0)
	, m_iViewportCenterOnSelectionCenterBorder(5)
	, m_szAlternateProfilSampleName("")
	, m_bGraphicalDetailPagingEnabled(false)
	, m_paHints()
	/************************************************************************************************/
	/* MODULAR_LOADING_CONTROL                 10/30/07                            MRGENIE          */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/
	// MLF loading
	, m_paModLoadControlVector(NULL)
	, m_paModLoadControls(NULL)
	/************************************************************************************************/
	/* MODULAR_LOADING_CONTROL                 END                                                  */
	/************************************************************************************************/
	/************************************************************************************************/
	/* XML_MODULAR_ART_LOADING                 03/28/08                                MRGENIE      */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/
	/*
	m_paMainMenus(NULL)
	*/
	, m_paMainMenus(NULL)
	, m_cszModDir("NONE")
	/************************************************************************************************/
	/* XML_MODULAR_ART_LOADING                 END                                                  */
	/************************************************************************************************/
	/************************************************************************************************/
	/* Afforess	                  Start		 12/8/09                                                */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/
	, m_iPEAK_EXTRA_MOVEMENT(0)
	, m_iPEAK_EXTRA_DEFENSE(0)
	, m_bLoadedPlayerOptions(false)
	, m_bXMLLogging(false)
	, m_iSCORE_FREE_PERCENT(0)
	, m_iSCORE_POPULATION_FACTOR(0)
	, m_iSCORE_LAND_FACTOR(0)
	, m_iSCORE_TECH_FACTOR(0)
	, m_iSCORE_WONDER_FACTOR(0)
	//New Python Callbacks
	, m_iUSE_CAN_CREATE_PROJECT_CALLBACK(0)
	, m_iUSE_CANNOT_CREATE_PROJECT_CALLBACK(0)
	, m_iUSE_CAN_DO_MELTDOWN_CALLBACK(0)
	, m_iUSE_CAN_MAINTAIN_PROCESS_CALLBACK(0)
	, m_iUSE_CANNOT_MAINTAIN_PROCESS_CALLBACK(0)
	, m_iUSE_CAN_DO_GROWTH_CALLBACK(0)
	, m_iUSE_CAN_DO_CULTURE_CALLBACK(0)
	, m_iUSE_CAN_DO_PLOT_CULTURE_CALLBACK(0)
	, m_iUSE_CAN_DO_PRODUCTION_CALLBACK(0)
	, m_iUSE_CAN_DO_RELIGION_CALLBACK(0)
	, m_iUSE_CAN_DO_GREATPEOPLE_CALLBACK(0)
	, m_iUSE_CAN_RAZE_CITY_CALLBACK(0)
	, m_iUSE_CAN_DO_GOLD_CALLBACK(0)
	, m_iUSE_CAN_DO_RESEARCH_CALLBACK(0)
	, m_iUSE_UPGRADE_UNIT_PRICE_CALLBACK(0)
	, m_iUSE_IS_VICTORY_CALLBACK(0)
	, m_iUSE_AI_UPDATE_UNIT_CALLBACK(0)
	, m_iUSE_AI_CHOOSE_PRODUCTION_CALLBACK(0)
	, m_iUSE_EXTRA_PLAYER_COSTS_CALLBACK(0)
	, m_iUSE_AI_DO_DIPLO_CALLBACK(0)
	, m_iUSE_AI_BESTTECH_CALLBACK(0)
	, m_iUSE_CAN_DO_COMBAT_CALLBACK(0)
	, m_iUSE_AI_CAN_DO_WARPLANS_CALLBACK(0)

	/************************************************************************************************/
	/* Afforess	                     END                                                            */
	/************************************************************************************************/
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                      02/21/10                                jdog5000      */
	/*                                                                                              */
	/* Efficiency, Options                                                                          */
	/************************************************************************************************/
	// BBAI Options
	, m_bBBAI_AIR_COMBAT(false)
	, m_bBBAI_HUMAN_VASSAL_WAR_BUILD(false)
	, m_iBBAI_DEFENSIVE_PACT_BEHAVIOR(0)
	, m_bBBAI_HUMAN_AS_VASSAL_OPTION(false)

	// BBAI AI Variables
	, m_iWAR_SUCCESS_CITY_CAPTURING(25)
	, m_iBBAI_ATTACK_CITY_STACK_RATIO(110)
	, m_iBBAI_SKIP_BOMBARD_BEST_ATTACK_ODDS(12)
	, m_iBBAI_SKIP_BOMBARD_BASE_STACK_RATIO(300)
	, m_iBBAI_SKIP_BOMBARD_MIN_STACK_RATIO(140)

	// Tech Diffusion
	, m_bTECH_DIFFUSION_ENABLE(false)
	, m_iTECH_DIFFUSION_KNOWN_TEAM_MODIFIER(30)
	, m_iTECH_DIFFUSION_WELFARE_THRESHOLD(88)
	, m_iTECH_DIFFUSION_WELFARE_MODIFIER(30)
	, m_iTECH_COST_FIRST_KNOWN_PREREQ_MODIFIER(20)
	, m_iTECH_COST_KNOWN_PREREQ_MODIFIER(20)
	, m_iTECH_COST_MODIFIER(100)
	, m_iUNIT_PRODUCTION_PERCENT_SM(100)
	, m_iUNIT_PRODUCTION_PERCENT(100)
	, m_iBUILDING_PRODUCTION_PERCENT(100)

	, m_iCOMBAT_DIE_SIDES(-1)
	, m_iCOMBAT_DAMAGE(-1)
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                       END                                                  */
	/************************************************************************************************/
	, m_bIsInPedia(false)
	, m_iLastTypeID(-1)
	, m_iActiveLandscapeID(0)
	// uninitialized variables bugfix
	, m_iNumPlayableCivilizationInfos(0)
	, m_iNumAIPlayableCivilizationInfos(0)
	, m_iTotalNumModules(0) // Modular loading control
	, m_iNumEntityEventTypes(0)
	, iStuckUnitID(0)
	, iStuckUnitCount(0)
	, m_iniInitCore(NULL)
	, m_loadedInitCore (NULL)
	, m_bResourceLayerOn(false)
	, m_iNumAnimationOperatorTypes(0)
	, m_iNumFlavorTypes(0)
	, m_iNumArtStyleTypes(0)
	, m_iNumCitySizeTypes(0)
	, m_iNumFootstepAudioTypes(0)
	, m_iUSE_GET_BUILDING_COST_MOD_CALLBACK(0)
	, m_iViewportSizeX(0)
	, m_iViewportSizeY(0)
	, m_iStoreExeSettingsCommerceInfo(0)
	, m_iStoreExeSettingsYieldInfo(0)
	, m_iStoreExeSettingsReligionInfo(0)
	, m_iStoreExeSettingsCorporationInfo(0)
	, m_iStoreExeSettingsBonusInfo(0)
	, m_bSignsCleared(false)
{
}

cvInternalGlobals::~cvInternalGlobals()
{
}

/************************************************************************************************/
/* MINIDUMP_MOD                           04/10/11                                terkhen       */
/*                                                                                              */
/* See http://www.debuginfo.com/articles/effminidumps.html                                      */
/************************************************************************************************/
#define MINIDUMP
#ifdef MINIDUMP

#include <dbghelp.h>
#pragma comment (lib, "dbghelp.lib")


std::string getPyTrace()
{
	std::vector<Cy::StackFrame> trace = Cy::get_stack_trace();

	std::stringstream buffer;

	for (std::vector<Cy::StackFrame>::const_iterator itr = trace.begin(); itr != trace.end(); ++itr)
	{
		if (itr != trace.begin()) buffer << "\r\n";
		buffer << CvString::format("%s.py (%d): %s", itr->filename.c_str(), itr->line, itr->code.c_str());
	}

	return buffer.str();
}

void CreateMiniDump(EXCEPTION_POINTERS *pep)
{
	_TCHAR filename[256];

	time_t rawtime;
	struct tm* timeinfo;
	time (&rawtime);
	timeinfo = localtime (&rawtime);

	_stprintf(filename, _T("MiniDump-%s-%d%02d%02d-%02d%02d%02d.dmp"), C2C_VERSION, 1900 + timeinfo->tm_year, timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

	/* Open a file to store the minidump. */
	HANDLE hFile = CreateFile(filename,
							  GENERIC_READ | GENERIC_WRITE,
							  0,
							  NULL,
							  CREATE_ALWAYS,
							  FILE_ATTRIBUTE_NORMAL,
							  NULL);

	if((hFile == NULL) || (hFile == INVALID_HANDLE_VALUE)) 
	{
		return;
	}

	/* Create the minidump. */
	MINIDUMP_EXCEPTION_INFORMATION mdei;

	mdei.ThreadId           = GetCurrentThreadId();
	mdei.ExceptionPointers  = pep;
	mdei.ClientPointers     = FALSE;

	MINIDUMP_TYPE mdt       = MiniDumpNormal;

	BOOL result = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
									hFile,
									mdt,
									(pep != NULL) ? &mdei : NULL,
									NULL,
									NULL);

	/* Close the file. */
	CloseHandle(hFile);
	std::string pyTrace = getPyTrace();
	if(!pyTrace.empty())
	{
		gDLL->logMsg("PythonCallstack.log", pyTrace.c_str(), true, false);
	}
}

LONG WINAPI CustomFilter(EXCEPTION_POINTERS *ExceptionInfo)
{
	CreateMiniDump(ExceptionInfo);
	return EXCEPTION_EXECUTE_HANDLER;
}

#endif
/************************************************************************************************/
/* MINIDUMP_MOD                                END                                              */
/************************************************************************************************/

//
// allocate
//
void cvInternalGlobals::init()
{
	OutputDebugString("Initializing Internal Globals: Start");
/************************************************************************************************/
/* MINIDUMP_MOD                           04/10/11                                terkhen       */
/************************************************************************************************/

#ifdef MINIDUMP
	/* Enable our custom exception that will write the minidump for us. */
	SetUnhandledExceptionFilter(CustomFilter);
#endif

/************************************************************************************************/
/* MINIDUMP_MOD                                END                                              */
/************************************************************************************************/

	//
	// These vars are used to initialize the globals.
	//

	int aiPlotDirectionX[NUM_DIRECTION_TYPES] =
	{
		0,	// DIRECTION_NORTH
		1,	// DIRECTION_NORTHEAST
		1,	// DIRECTION_EAST
		1,	// DIRECTION_SOUTHEAST
		0,	// DIRECTION_SOUTH
		-1,	// DIRECTION_SOUTHWEST
		-1,	// DIRECTION_WEST
		-1,	// DIRECTION_NORTHWEST
	};

	int aiPlotDirectionY[NUM_DIRECTION_TYPES] =
	{
		1,	// DIRECTION_NORTH
		1,	// DIRECTION_NORTHEAST
		0,	// DIRECTION_EAST
		-1,	// DIRECTION_SOUTHEAST
		-1,	// DIRECTION_SOUTH
		-1,	// DIRECTION_SOUTHWEST
		0,	// DIRECTION_WEST
		1,	// DIRECTION_NORTHWEST
	};

	int aiPlotCardinalDirectionX[NUM_CARDINALDIRECTION_TYPES] =
	{
		0,	// CARDINALDIRECTION_NORTH
		1,	// CARDINALDIRECTION_EAST
		0,	// CARDINALDIRECTION_SOUTH
		-1,	// CARDINALDIRECTION_WEST
	};

	int aiPlotCardinalDirectionY[NUM_CARDINALDIRECTION_TYPES] =
	{
		1,	// CARDINALDIRECTION_NORTH
		0,	// CARDINALDIRECTION_EAST
		-1,	// CARDINALDIRECTION_SOUTH
		0,	// CARDINALDIRECTION_WEST
	};

	int aiCityPlotX[NUM_CITY_PLOTS] =
	{
		0,
		0, 1, 1, 1, 0,-1,-1,-1,
		0, 1, 2, 2, 2, 1, 0,-1,-2,-2,-2,-1,
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/17/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		0, 1, 2, 3, 3, 3, 2, 1, 0, -1, -2, -3, -3, -3, -2, -1,
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/
	};

	int aiCityPlotY[NUM_CITY_PLOTS] =
	{
		0,
		1, 1, 0,-1,-1,-1, 0, 1,
		2, 2, 1, 0,-1,-2,-2,-2,-1, 0, 1, 2,
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/17/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		3, 3, 2, 1, 0, -1, -2, -3, -3, -3, -2, -1, 0, 1, 2, 3,
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/
	};

	int aiCityPlotPriority[NUM_CITY_PLOTS] =
	{
		0,
		1, 2, 1, 2, 1, 2, 1, 2,
		3, 4, 4, 3, 4, 4, 3, 4, 4, 3, 4, 4,
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/17/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		5, 6, 7, 6, 5, 6, 7, 6, 5, 6, 7, 6, 5, 6, 7, 6,
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/
	};

	int aaiXYCityPlot[CITY_PLOTS_DIAMETER][CITY_PLOTS_DIAMETER] =
	{
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/17/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		{-1, -1, 32, 33, 34, -1, -1},
		{-1, 31, 17, 18, 19, 35, -1},
		{30, 16, 6,   7,  8, 20, 36},
		{29, 15, 5,   0,  1,  9, 21},
		{28, 14, 4,   3,  2, 10, 22},
		{-1, 27, 13, 12, 11, 23, -1},
		{-1, -1, 26, 25, 24, -1, -1},
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/
	};

	DirectionTypes aeTurnRightDirection[NUM_DIRECTION_TYPES] =
	{
		DIRECTION_NORTHEAST,	// DIRECTION_NORTH
		DIRECTION_EAST,				// DIRECTION_NORTHEAST
		DIRECTION_SOUTHEAST,	// DIRECTION_EAST
		DIRECTION_SOUTH,			// DIRECTION_SOUTHEAST
		DIRECTION_SOUTHWEST,	// DIRECTION_SOUTH
		DIRECTION_WEST,				// DIRECTION_SOUTHWEST
		DIRECTION_NORTHWEST,	// DIRECTION_WEST
		DIRECTION_NORTH,			// DIRECTION_NORTHWEST
	};

	DirectionTypes aeTurnLeftDirection[NUM_DIRECTION_TYPES] =
	{
		DIRECTION_NORTHWEST,	// DIRECTION_NORTH
		DIRECTION_NORTH,			// DIRECTION_NORTHEAST
		DIRECTION_NORTHEAST,	// DIRECTION_EAST
		DIRECTION_EAST,				// DIRECTION_SOUTHEAST
		DIRECTION_SOUTHEAST,	// DIRECTION_SOUTH
		DIRECTION_SOUTH,			// DIRECTION_SOUTHWEST
		DIRECTION_SOUTHWEST,	// DIRECTION_WEST
		DIRECTION_WEST,				// DIRECTION_NORTHWEST
	};

	DirectionTypes aaeXYDirection[DIRECTION_DIAMETER][DIRECTION_DIAMETER] =
	{
		DIRECTION_SOUTHWEST, DIRECTION_WEST,	DIRECTION_NORTHWEST,
		DIRECTION_SOUTH,     NO_DIRECTION,    DIRECTION_NORTH,
		DIRECTION_SOUTHEAST, DIRECTION_EAST,	DIRECTION_NORTHEAST,
	};

	FAssertMsg(gDLL != NULL, "Civ app needs to set gDLL");

	m_VarSystem = new FVariableSystem;
	m_asyncRand = new CvRandom;
	m_initCore = new CvInitCore;
	m_loadedInitCore = new CvInitCore;
	m_iniInitCore = new CvInitCore;

	gDLL->initGlobals();	// some globals need to be allocated outside the dll

	m_game = new CvGameAI;
	
/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	m_maps.push_back(new CvMap(MAP_INITIAL));
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

	CvPlayerAI::initStatics();
	CvTeamAI::initStatics();

	m_pt3Origin = NiPoint3(0.0f, 0.0f, 0.0f);

	COPY(m_aiPlotDirectionX, aiPlotDirectionX, int);
	COPY(m_aiPlotDirectionY, aiPlotDirectionY, int);
	COPY(m_aiPlotCardinalDirectionX, aiPlotCardinalDirectionX, int);
	COPY(m_aiPlotCardinalDirectionY, aiPlotCardinalDirectionY, int);
	COPY(m_aiCityPlotX, aiCityPlotX, int);
	COPY(m_aiCityPlotY, aiCityPlotY, int);
	COPY(m_aiCityPlotPriority, aiCityPlotPriority, int);
	COPY(m_aeTurnLeftDirection, aeTurnLeftDirection, DirectionTypes);
	COPY(m_aeTurnRightDirection, aeTurnRightDirection, DirectionTypes);
	memcpy(m_aaiXYCityPlot, aaiXYCityPlot, sizeof(m_aaiXYCityPlot));
	memcpy(m_aaeXYDirection, aaeXYDirection,sizeof(m_aaeXYDirection));

	m_bSignsCleared = false;
	m_bResourceLayerOn = false;

	OutputDebugString("Initializing Internal Globals: End");
}

//
// free
//
void cvInternalGlobals::uninit()
{
	//
	// See also CvXMLLoadUtilityInit.cpp::CleanUpGlobalVariables()
	//
	SAFE_DELETE_ARRAY(m_aiPlotDirectionX);
	SAFE_DELETE_ARRAY(m_aiPlotDirectionY);
	SAFE_DELETE_ARRAY(m_aiPlotCardinalDirectionX);
	SAFE_DELETE_ARRAY(m_aiPlotCardinalDirectionY);
	SAFE_DELETE_ARRAY(m_aiCityPlotX);
	SAFE_DELETE_ARRAY(m_aiCityPlotY);
	SAFE_DELETE_ARRAY(m_aiCityPlotPriority);
	SAFE_DELETE_ARRAY(m_aeTurnLeftDirection);
	SAFE_DELETE_ARRAY(m_aeTurnRightDirection);

	SAFE_DELETE(m_game);
	
/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	for (std::vector<CvMap*>::iterator it = m_maps.begin(); it != m_maps.end(); ++it)
	{
		SAFE_DELETE(*it);
	}
	m_maps.clear();
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

	CvPlayerAI::freeStatics();
	CvTeamAI::freeStatics();

	SAFE_DELETE(m_asyncRand);
	SAFE_DELETE(m_initCore);
	SAFE_DELETE(m_loadedInitCore);
	SAFE_DELETE(m_iniInitCore);
	gDLL->uninitGlobals();	// free globals allocated outside the dll
	SAFE_DELETE(m_VarSystem);

	// already deleted outside of the dll, set to null for safety
	m_messageQueue=NULL;
	m_hotJoinMsgQueue=NULL;
	m_messageControl=NULL;
	m_setupData=NULL;
	m_messageCodes=NULL;
	m_dropMgr=NULL;
	m_portal=NULL;
	m_statsReporter=NULL;
	m_interface=NULL;
	m_diplomacyScreen=NULL;
	m_mpDiplomacyScreen=NULL;
	m_pathFinder=NULL;
	m_interfacePathFinder=NULL;
	m_stepFinder=NULL;
	m_routeFinder=NULL;
	m_borderFinder=NULL;
	m_areaFinder=NULL;
	m_plotGroupFinder=NULL;

	m_typesMap.clear();
	m_aInfoVectors.clear();
}

void cvInternalGlobals::clearTypesMap()
{
	m_typesMap.clear();
	if (m_VarSystem)
	{
		m_VarSystem->UnInit();
	}
}


CvDiplomacyScreen* cvInternalGlobals::getDiplomacyScreen() const
{
	return m_diplomacyScreen;
}

CMPDiplomacyScreen* cvInternalGlobals::getMPDiplomacyScreen() const
{
	return m_mpDiplomacyScreen;
}

CvMessageCodeTranslator& cvInternalGlobals::getMessageCodes()
{
	return *m_messageCodes;
}

FMPIManager*& cvInternalGlobals::getFMPMgrPtr()
{
	return m_pFMPMgr;
}

CvPortal& cvInternalGlobals::getPortal()
{
	return *m_portal;
}

CvSetupData& cvInternalGlobals::getSetupData()
{
	return *m_setupData;
}

CvInitCore& cvInternalGlobals::getInitCore()
{
	return *m_initCore;
}

CvInitCore& cvInternalGlobals::getLoadedInitCore()
{
	return *m_loadedInitCore;
}

CvInitCore& cvInternalGlobals::getIniInitCore()
{
	return *m_iniInitCore;
}

CvStatsReporter& cvInternalGlobals::getStatsReporter()
{
	return *m_statsReporter;
}

CvStatsReporter* cvInternalGlobals::getStatsReporterPtr() const
{
	return m_statsReporter;
}

CvInterface& cvInternalGlobals::getInterface()
{
	return *m_interface;
}

CvInterface* cvInternalGlobals::getInterfacePtr() const
{
	return m_interface;
}

CvRandom& cvInternalGlobals::getASyncRand()
{
	return *m_asyncRand;
}

CMessageQueue& cvInternalGlobals::getMessageQueue()
{
	return *m_messageQueue;
}

CMessageQueue& cvInternalGlobals::getHotMessageQueue()
{
	return *m_hotJoinMsgQueue;
}

CMessageControl& cvInternalGlobals::getMessageControl()
{
	return *m_messageControl;
}

CvDropMgr& cvInternalGlobals::getDropMgr()
{
	return *m_dropMgr;
}

FAStar& cvInternalGlobals::getPathFinder()
{
	return *m_pathFinder;
}

FAStar& cvInternalGlobals::getInterfacePathFinder()
{
	return *m_interfacePathFinder;
}

FAStar& cvInternalGlobals::getStepFinder()
{
	return *m_stepFinder;
}

FAStar& cvInternalGlobals::getRouteFinder()
{
	return *m_routeFinder;
}

FAStar& cvInternalGlobals::getBorderFinder()
{
	return *m_borderFinder;
}

FAStar& cvInternalGlobals::getAreaFinder()
{
	return *m_areaFinder;
}

FAStar& cvInternalGlobals::getPlotGroupFinder()
{
	return *m_plotGroupFinder;
}

NiPoint3& cvInternalGlobals::getPt3Origin()
{
	return m_pt3Origin;
}

std::vector<CvInterfaceModeInfo*>& cvInternalGlobals::getInterfaceModeInfos()		// For Moose - XML Load Util and CvInfos
{
	return m_paInterfaceModeInfo;
}

CvInterfaceModeInfo& cvInternalGlobals::getInterfaceModeInfo(InterfaceModeTypes e)
{
	FAssertMsg(e >= 0 && e < NUM_INTERFACEMODE_TYPES, "InterfaceModeInfo index out of bounds");
	return *(m_paInterfaceModeInfo[e]);
}

NiPoint3& cvInternalGlobals::getPt3CameraDir()
{
	return m_pt3CameraDir;
}

bool& cvInternalGlobals::getLogging()
{
	return m_bLogging;
}

bool& cvInternalGlobals::getRandLogging()
{
	return m_bRandLogging;
}

bool& cvInternalGlobals::getSynchLogging()
{
	return m_bSynchLogging;
}

bool& cvInternalGlobals::overwriteLogs()
{
	return m_bOverwriteLogs;
}

int* cvInternalGlobals::getPlotDirectionX() const
{
	return m_aiPlotDirectionX;
}

int* cvInternalGlobals::getPlotDirectionY() const
{
	return m_aiPlotDirectionY;
}

int* cvInternalGlobals::getPlotCardinalDirectionX() const
{
	return m_aiPlotCardinalDirectionX;
}

int* cvInternalGlobals::getPlotCardinalDirectionY() const
{
	return m_aiPlotCardinalDirectionY;
}

int* cvInternalGlobals::getCityPlotX() const
{
	return m_aiCityPlotX;
}

int* cvInternalGlobals::getCityPlotY() const
{
	return m_aiCityPlotY;
}

int* cvInternalGlobals::getCityPlotPriority() const
{
	return m_aiCityPlotPriority;
}

int cvInternalGlobals::getXYCityPlot(const int i, const int j) const
{
	FAssertMsg(i >= 0 && i < CITY_PLOTS_DIAMETER, "XYCityPlot i index out of bounds");
	FAssertMsg(j >= 0 && j < CITY_PLOTS_DIAMETER, "XYCityPlot j index out of bounds");
	return m_aaiXYCityPlot[i][j];
}

DirectionTypes* cvInternalGlobals::getTurnLeftDirection() const
{
	return m_aeTurnLeftDirection;
}

DirectionTypes cvInternalGlobals::getTurnLeftDirection(const int i) const
{
	FAssertMsg(i >= 0 && i < DIRECTION_DIAMETER, "TurnLeftDirection index out of bounds");
	return m_aeTurnLeftDirection[i];
}

DirectionTypes* cvInternalGlobals::getTurnRightDirection() const
{
	return m_aeTurnRightDirection;
}

DirectionTypes cvInternalGlobals::getTurnRightDirection(const int i) const
{
	FAssertMsg(i >= 0 && i < DIRECTION_DIAMETER, "TurnRightDirection index out of bounds");
	return m_aeTurnRightDirection[i];
}

DirectionTypes cvInternalGlobals::getXYDirection(const int i, const int j) const
{
	FAssertMsg(i >= 0 && i < DIRECTION_DIAMETER, "XYDirection i index out of bounds");
	FAssertMsg(j >= 0 && j < DIRECTION_DIAMETER, "XYDirection j index out of bounds");
	return m_aaeXYDirection[i][j];
}

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/

bool cvInternalGlobals::multiMapsEnabled() const
{
	return m_bMultimapsEnabled;
}

bool cvInternalGlobals::viewportsEnabled() const
{
	return m_bViewportsEnabled;
}

bool cvInternalGlobals::getReprocessGreatWallDynamically() const
{
	return m_bViewportsEnabled || (getDefineBOOL("DYNAMIC_GREAT_WALL") != 0);
}

int cvInternalGlobals::getNumMapInfos() const
{
	return multiMapsEnabled() ? m_paMapInfo.size() : 1;
}

int cvInternalGlobals::getNumMapSwitchInfos() const
{
	return m_paMapSwitchInfo.size();
}

std::vector<CvMapInfo*>& cvInternalGlobals::getMapInfos()
{
	return m_paMapInfo;
}

std::vector<CvMapSwitchInfo*>& cvInternalGlobals::getMapSwitchInfos()
{
	return m_paMapSwitchInfo;
}

CvMapInfo& cvInternalGlobals::getMapInfo(const MapTypes eMap) const
{
	FAssertMsg(eMap > NO_MAP && eMap < GC.getNumMapInfos(), "MapInfo index out of bounds");
	return *(m_paMapInfo[eMap]);
}

CvMapSwitchInfo& cvInternalGlobals::getMapSwitchInfo(const MapSwitchTypes eMapSwitch) const
{
	FAssertMsg(eMapSwitch > NO_MAPSWITCH && eMapSwitch < GC.getNumMapSwitchInfos(), "MapSwitchInfo index out of bounds");
	return *(m_paMapSwitchInfo[eMapSwitch]);
}

void cvInternalGlobals::updateMaps()
{
	CvMap* pMap = NULL;
	int i;
	
	for (i = 1; i < GC.getNumMapInfos(); i++)
	{
		m_maps.push_back(pMap);
	}
	FAssert(m_maps.size() == GC.getNumMapInfos());
}

const std::vector<CvMap*>& cvInternalGlobals::getMaps() const
{
	return m_maps;
}

void cvInternalGlobals::setResourceLayer(bool bOn)
{
	m_bResourceLayerOn = bOn;

	gDLL->getEngineIFace()->setResourceLayer(bOn);
}

bool cvInternalGlobals::getResourceLayer() const
{
	return m_bResourceLayerOn;
}

/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

int cvInternalGlobals::getNumWorldInfos() const
{
	return (int)m_paWorldInfo.size();
}

std::vector<CvWorldInfo*>& cvInternalGlobals::getWorldInfos()
{
	return m_paWorldInfo;
}

CvWorldInfo& cvInternalGlobals::getWorldInfo(WorldSizeTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumWorldInfos(), "WorldInfo index out of bounds");
	return *(m_paWorldInfo[e]);
}

CvInfoReplacements<CvWorldInfo>* cvInternalGlobals::getWorldInfoReplacements()
{
	return &m_WorldInfoReplacements;
}

/////////////////////////////////////////////
// CLIMATE
/////////////////////////////////////////////

int cvInternalGlobals::getNumClimateInfos() const
{
	return (int)m_paClimateInfo.size();
}

std::vector<CvClimateInfo*>& cvInternalGlobals::getClimateInfos()
{
	return m_paClimateInfo;
}

CvClimateInfo& cvInternalGlobals::getClimateInfo(ClimateTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumClimateInfos(), "ClimateInfo index out of bounds");
	return *(m_paClimateInfo[e]);
}

/////////////////////////////////////////////
// SEALEVEL
/////////////////////////////////////////////

int cvInternalGlobals::getNumSeaLevelInfos() const
{
	return (int)m_paSeaLevelInfo.size();
}

std::vector<CvSeaLevelInfo*>& cvInternalGlobals::getSeaLevelInfos()
{
	return m_paSeaLevelInfo;
}

CvSeaLevelInfo& cvInternalGlobals::getSeaLevelInfo(SeaLevelTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumSeaLevelInfos(), "SeaLevelInfo index out of bounds");
	return *(m_paSeaLevelInfo[e]);
}

int cvInternalGlobals::getNumHints() const
{
	return (int)m_paHints.size();
}

std::vector<CvInfoBase*>& cvInternalGlobals::getHints()
{
	return m_paHints;
}

CvInfoBase& cvInternalGlobals::getHints(int i) const
{
	return *(m_paHints[i]);
}

int cvInternalGlobals::getNumMainMenus() const
{
	return (int)m_paMainMenus.size();
}

std::vector<CvMainMenuInfo*>& cvInternalGlobals::getMainMenus()
{
	return m_paMainMenus;
}

CvMainMenuInfo& cvInternalGlobals::getMainMenus(int i) const
{
	if (i >= getNumMainMenus())
	{
		return *(m_paMainMenus[0]);
	}

	return *(m_paMainMenus[i]);
}
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 10/30/07                            MRGENIE          */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
// MLF loading
void cvInternalGlobals::resetModLoadControlVector()
{
	m_paModLoadControlVector.clear();
}

int cvInternalGlobals::getModLoadControlVectorSize() const
{
	return (int)m_paModLoadControlVector.size();
}

void cvInternalGlobals::setModLoadControlVector(const char* szModule)
{
	m_paModLoadControlVector.push_back(szModule);
}

CvString cvInternalGlobals::getModLoadControlVector(int i) const
{
	return (CvString)m_paModLoadControlVector.at(i);
}

int cvInternalGlobals::getTotalNumModules() const
{
	return m_iTotalNumModules;
}

void cvInternalGlobals::setTotalNumModules()
{
	m_iTotalNumModules++;
}

int cvInternalGlobals::getNumModLoadControlInfos() const
{
	return (int)m_paModLoadControls.size();
}

std::vector<CvModLoadControlInfo*>& cvInternalGlobals::getModLoadControlInfos()
{
	return m_paModLoadControls;
}

CvModLoadControlInfo& cvInternalGlobals::getModLoadControlInfos(int iIndex) const
{
	FAssertMsg(iIndex >= 0 && iIndex < getNumModLoadControlInfos(), "ModLoadControlInfo index out of bounds");
	return *(m_paModLoadControls[iIndex]);
}

void cvInternalGlobals::updateReplacements()
{
//ReplacementStep
	m_WorldInfoReplacements.updateReplacements(getWorldInfos());
	m_UnitInfoReplacements.updateReplacements(getUnitInfos());
	m_TechInfoReplacements.updateReplacements(getTechInfos());
	m_TraitInfoReplacements.updateReplacements(getTraitInfos());
	m_PromotionInfoReplacements.updateReplacements(getPromotionInfos());
	m_BonusClassInfoReplacements.updateReplacements(getBonusClassInfos());
	m_BonusInfoReplacements.updateReplacements(getBonusInfos());
	m_ImprovementInfoReplacements.updateReplacements(getImprovementInfos());
	m_BuildingInfoReplacements.updateReplacements(getBuildingInfos());
	m_CivicInfoReplacements.updateReplacements(getCivicInfos());
	m_LeaderHeadInfoReplacements.updateReplacements(getLeaderHeadInfos());

	m_CivilizationInfoReplacements.updateReplacements(getCivilizationInfos());

	m_CultureLevelInfoReplacements.updateReplacements(getCultureLevelInfos());

	m_EventInfoReplacements.updateReplacements(getEventInfos());
	m_EventTriggerInfoReplacements.updateReplacements(getEventTriggerInfos());
	m_ProcessInfoReplacements.updateReplacements(getProcessInfos());
	m_TerrainInfoReplacements.updateReplacements(getTerrainInfos());

	m_SpecialistInfoReplacements.updateReplacements(getSpecialistInfos());

	m_FeatureInfoReplacements.updateReplacements(getFeatureInfos());

	m_ReligionInfoReplacements.updateReplacements(getReligionInfos());
	m_CorporationInfoReplacements.updateReplacements(getCorporationInfos());

	m_RouteInfoReplacements.updateReplacements(getRouteInfos());
	
	m_ProjectInfoReplacements.updateReplacements(getProjectInfos());

	m_BuildInfoReplacements.updateReplacements(getBuildInfos());
	
	m_SpawnInfoReplacements.updateReplacements(getSpawnInfos());
	m_GameSpeedInfoReplacements.updateReplacements(getGameSpeedInfos());
	m_EraInfoReplacements.updateReplacements(getEraInfos());
	
	m_SpecialBuildingInfoReplacements.updateReplacements(getSpecialBuildingInfos());
	
	m_HandicapInfoReplacements.updateReplacements(getHandicapInfos());
//ReplacementStep: search down here for 'CvInfoReplacements'
}

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING                 10/26/07                            MRGENIE          */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
void cvInternalGlobals::setModDir(const char* szModDir)
{
	m_cszModDir = szModDir;
}

std::string cvInternalGlobals::getModDir() const
{
	return m_cszModDir;
}
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING                 END                                                  */
/************************************************************************************************/

int cvInternalGlobals::getNumColorInfos() const
{
	return (int)m_paColorInfo.size();
}

std::vector<CvColorInfo*>& cvInternalGlobals::getColorInfos()
{
	return m_paColorInfo;
}

CvColorInfo& cvInternalGlobals::getColorInfo(ColorTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumColorInfos(), "ColorInfo index out of bounds");
	return *(m_paColorInfo[e]);
}


int cvInternalGlobals::getNumPlayerColorInfos() const
{
	return (int)m_paPlayerColorInfo.size();
}

std::vector<CvPlayerColorInfo*>& cvInternalGlobals::getPlayerColorInfos()
{
	return m_paPlayerColorInfo;
}

CvPlayerColorInfo& cvInternalGlobals::getPlayerColorInfo(PlayerColorTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumPlayerColorInfos(), "PlayerColorInfo index out of bounds");
	return *(m_paPlayerColorInfo[e]);
}

int cvInternalGlobals::getNumAdvisorInfos() const
{
	return (int)m_paAdvisorInfo.size();
}

std::vector<CvAdvisorInfo*>& cvInternalGlobals::getAdvisorInfos()
{
	return m_paAdvisorInfo;
}

CvAdvisorInfo& cvInternalGlobals::getAdvisorInfo(AdvisorTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumAdvisorInfos(), "AdvisorInfo index out of bounds");
	return *(m_paAdvisorInfo[e]);
}

int cvInternalGlobals::getNumRouteModelInfos() const
{
	return (int)m_paRouteModelInfo.size();
}

std::vector<CvRouteModelInfo*>& cvInternalGlobals::getRouteModelInfos()
{
	return m_paRouteModelInfo;
}

CvRouteModelInfo& cvInternalGlobals::getRouteModelInfo(int i) const
{
	FAssertMsg(i >= 0 && i < GC.getNumRouteModelInfos(), "RouteModelInfo index out of bounds");
	return *(m_paRouteModelInfo[i]);
}

int cvInternalGlobals::getNumRiverInfos() const
{
	return (int)m_paRiverInfo.size();
}

std::vector<CvRiverInfo*>& cvInternalGlobals::getRiverInfos()
{
	return m_paRiverInfo;
}

CvRiverInfo& cvInternalGlobals::getRiverInfo(RiverTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumRiverInfos(), "RiverInfo index out of bounds");
	return *(m_paRiverInfo[e]);
}

int cvInternalGlobals::getNumRiverModelInfos() const
{
	return (int)m_paRiverModelInfo.size();
}

std::vector<CvRiverModelInfo*>& cvInternalGlobals::getRiverModelInfos()
{
	return m_paRiverModelInfo;
}

CvRiverModelInfo& cvInternalGlobals::getRiverModelInfo(int i) const
{
	FAssertMsg(i >= 0 && i < GC.getNumRiverModelInfos(), "RiverModelInfo index out of bounds");
	return *(m_paRiverModelInfo[i]);
}

int cvInternalGlobals::getNumWaterPlaneInfos() const
{
	return (int)m_paWaterPlaneInfo.size();
}

std::vector<CvWaterPlaneInfo*>& cvInternalGlobals::getWaterPlaneInfos()		// For Moose - CvDecal and CvWater
{
	return m_paWaterPlaneInfo;
}

CvWaterPlaneInfo& cvInternalGlobals::getWaterPlaneInfo(int i) const
{
	FAssertMsg(i >= 0 && i < GC.getNumWaterPlaneInfos(), "WaterPlaneInfo index out of bounds");
	return *(m_paWaterPlaneInfo[i]);
}

int cvInternalGlobals::getNumTerrainPlaneInfos() const
{
	return (int)m_paTerrainPlaneInfo.size();
}

std::vector<CvTerrainPlaneInfo*>& cvInternalGlobals::getTerrainPlaneInfos()
{
	return m_paTerrainPlaneInfo;
}

CvTerrainPlaneInfo& cvInternalGlobals::getTerrainPlaneInfo(int i) const
{
	FAssertMsg(i >= 0 && i < GC.getNumTerrainPlaneInfos(), "TerrainPlaneInfo index out of bounds");
	return *(m_paTerrainPlaneInfo[i]);
}

int cvInternalGlobals::getNumCameraOverlayInfos() const
{
	return (int)m_paCameraOverlayInfo.size();
}

std::vector<CvCameraOverlayInfo*>& cvInternalGlobals::getCameraOverlayInfos()
{
	return m_paCameraOverlayInfo;
}

CvCameraOverlayInfo& cvInternalGlobals::getCameraOverlayInfo(int i) const
{
	FAssertMsg(i >= 0 && i < GC.getNumCameraOverlayInfos(), "CameraOverlayInfo index out of bounds");
	return *(m_paCameraOverlayInfo[i]);
}

int cvInternalGlobals::getNumAnimationPathInfos() const
{
	return (int)m_paAnimationPathInfo.size();
}

std::vector<CvAnimationPathInfo*>& cvInternalGlobals::getAnimationPathInfos()
{
	return m_paAnimationPathInfo;
}

CvAnimationPathInfo& cvInternalGlobals::getAnimationPathInfo(AnimationPathTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumAnimationPathInfos(), "AnimationPathInfo index out of bounds");
	return *(m_paAnimationPathInfo[e]);
}

int cvInternalGlobals::getNumAnimationCategoryInfos() const
{
	return (int)m_paAnimationCategoryInfo.size();
}

std::vector<CvAnimationCategoryInfo*>& cvInternalGlobals::getAnimationCategoryInfos()
{
	return m_paAnimationCategoryInfo;
}

CvAnimationCategoryInfo& cvInternalGlobals::getAnimationCategoryInfo(AnimationCategoryTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumAnimationCategoryInfos(), "AnimationCategoryInfo index out of bounds");
	return *(m_paAnimationCategoryInfo[e]);
}

int cvInternalGlobals::getNumEntityEventInfos() const
{
	return (int)m_paEntityEventInfo.size();
}

std::vector<CvEntityEventInfo*>& cvInternalGlobals::getEntityEventInfos()
{
	return m_paEntityEventInfo;
}

CvEntityEventInfo& cvInternalGlobals::getEntityEventInfo(EntityEventTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumEntityEventInfos(), "EntityEventInfo index out of bounds");
	return *(m_paEntityEventInfo[e]);
}

int cvInternalGlobals::getNumEffectInfos() const
{
	return (int)m_paEffectInfo.size();
}

std::vector<CvEffectInfo*>& cvInternalGlobals::getEffectInfos()
{
	return m_paEffectInfo;
}

CvEffectInfo& cvInternalGlobals::getEffectInfo(int i) const
{
	FAssertMsg(i >= 0 && i < GC.getNumEffectInfos(), "EffectInfo index out of bounds");
	return *(m_paEffectInfo[i]);
}


int cvInternalGlobals::getNumAttachableInfos() const
{
	return (int)m_paAttachableInfo.size();
}

std::vector<CvAttachableInfo*>& cvInternalGlobals::getAttachableInfos()
{
	return m_paAttachableInfo;
}

CvAttachableInfo& cvInternalGlobals::getAttachableInfo(int i) const
{
	FAssertMsg(i >= 0 && i < GC.getNumAttachableInfos(), "AttachableInfo index out of bounds");
	return *(m_paAttachableInfo[i]);
}

int cvInternalGlobals::getNumCameraInfos() const
{
	return (int)m_paCameraInfo.size();
}

std::vector<CvCameraInfo*>& cvInternalGlobals::getCameraInfos()
{
	return m_paCameraInfo;
}

CvCameraInfo& cvInternalGlobals::getCameraInfo(CameraAnimationTypes eCameraAnimationNum) const
{
	return *(m_paCameraInfo[eCameraAnimationNum]);
}

int cvInternalGlobals::getNumUnitFormationInfos() const
{
	return (int)m_paUnitFormationInfo.size();
}

std::vector<CvUnitFormationInfo*>& cvInternalGlobals::getUnitFormationInfos()		// For Moose - CvUnitEntity
{
	return m_paUnitFormationInfo;
}

CvUnitFormationInfo& cvInternalGlobals::getUnitFormationInfo(int i) const
{
	FAssertMsg(i >= 0 && i < GC.getNumUnitFormationInfos(), "UnitFormationInfo index out of bounds");
	return *(m_paUnitFormationInfo[i]);
}

// TEXT
int cvInternalGlobals::getNumGameTextXML() const
{
	return (int)m_paGameTextXML.size();
}

std::vector<CvGameText*>& cvInternalGlobals::getGameTextXMLs()
{
	return m_paGameTextXML;
}

// Landscape INFOS
int cvInternalGlobals::getNumLandscapeInfos() const
{
	return (int)m_paLandscapeInfo.size();
}

std::vector<CvLandscapeInfo*>& cvInternalGlobals::getLandscapeInfos()
{
	return m_paLandscapeInfo;
}

CvLandscapeInfo& cvInternalGlobals::getLandscapeInfo(int iIndex) const
{
	FAssertMsg(iIndex >= 0 && iIndex < GC.getNumLandscapeInfos(), "LandscapeInfo index out of bounds");
	return *(m_paLandscapeInfo[iIndex]);
}

int cvInternalGlobals::getActiveLandscapeID() const
{
	return m_iActiveLandscapeID;
}

void cvInternalGlobals::setActiveLandscapeID(int iLandscapeID)
{
	m_iActiveLandscapeID = iLandscapeID;
}


int cvInternalGlobals::getNumTerrainInfos() const
{
	return (int)m_paTerrainInfo.size();
}

std::vector<CvTerrainInfo*>& cvInternalGlobals::getTerrainInfos()		// For Moose - XML Load Util, CvInfos, CvTerrainTypeWBPalette
{
	return m_paTerrainInfo;
}

CvTerrainInfo& cvInternalGlobals::getTerrainInfo(TerrainTypes eTerrainNum) const
{
	FAssertMsg(eTerrainNum >= 0 && eTerrainNum < GC.getNumTerrainInfos(), "TerrainInfo index out of bounds");
	return *(m_paTerrainInfo[eTerrainNum]);
}

CvInfoReplacements<CvTerrainInfo>* cvInternalGlobals::getTerrainInfoReplacements()
{
	return &m_TerrainInfoReplacements;
}

int cvInternalGlobals::getNumBonusClassInfos() const
{
	return (int)m_paBonusClassInfo.size();
}

std::vector<CvBonusClassInfo*>& cvInternalGlobals::getBonusClassInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paBonusClassInfo;
}

CvBonusClassInfo& cvInternalGlobals::getBonusClassInfo(BonusClassTypes eBonusNum) const
{
	FAssertMsg(eBonusNum >= 0 && eBonusNum < GC.getNumBonusClassInfos(), "BonusClassInfo index out of bounds");
	return *(m_paBonusClassInfo[eBonusNum]);
}

CvInfoReplacements<CvBonusClassInfo>* cvInternalGlobals::getBonusClassInfoReplacements()
{
	return &m_BonusClassInfoReplacements;
}


int cvInternalGlobals::getNumBonusInfos() const
{
	return (int)m_paBonusInfo.size();
}

std::vector<CvBonusInfo*>& cvInternalGlobals::getBonusInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paBonusInfo;
}

CvBonusInfo& cvInternalGlobals::getBonusInfo(BonusTypes eBonusNum) const
{
	FAssertMsg(eBonusNum >= 0 && eBonusNum < GC.getNumBonusInfos(), "BonusInfo index out of bounds");
	return *(m_paBonusInfo[eBonusNum]);
}

CvInfoReplacements<CvBonusInfo>* cvInternalGlobals::getBonusInfoReplacements()
{
	return &m_BonusInfoReplacements;
}

int cvInternalGlobals::getNumFeatureInfos() const
{
	return (int)m_paFeatureInfo.size();
}

std::vector<CvFeatureInfo*>& cvInternalGlobals::getFeatureInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paFeatureInfo;
}

CvFeatureInfo& cvInternalGlobals::getFeatureInfo(FeatureTypes eFeatureNum) const
{
	FAssertMsg(eFeatureNum >= 0 && eFeatureNum < GC.getNumFeatureInfos(), "FeatureInfo index out of bounds");
	return *(m_paFeatureInfo[eFeatureNum]);
}

CvInfoReplacements<CvFeatureInfo>* cvInternalGlobals::getFeatureInfoReplacements()
{
	return &m_FeatureInfoReplacements;
}

int& cvInternalGlobals::getNumPlayableCivilizationInfos()
{
	return m_iNumPlayableCivilizationInfos;
}

int& cvInternalGlobals::getNumAIPlayableCivilizationInfos()
{
	return m_iNumAIPlayableCivilizationInfos;
}

int cvInternalGlobals::getNumCivilizationInfos() const
{
	return (int)m_paCivilizationInfo.size();
}

std::vector<CvCivilizationInfo*>& cvInternalGlobals::getCivilizationInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paCivilizationInfo;
}

CvCivilizationInfo& cvInternalGlobals::getCivilizationInfo(CivilizationTypes eCivilizationNum) const
{
	FAssertMsg(eCivilizationNum >= 0 && eCivilizationNum < GC.getNumCivilizationInfos(), "CivilizationInfo index out of bounds");
	return *(m_paCivilizationInfo[eCivilizationNum]);
}

CvInfoReplacements<CvCivilizationInfo>* cvInternalGlobals::getCivilizationInfoReplacements()
{
	return &m_CivilizationInfoReplacements;
}


int cvInternalGlobals::getNumLeaderHeadInfos() const
{
	return (int)m_paLeaderHeadInfo.size();
}

std::vector<CvLeaderHeadInfo*>& cvInternalGlobals::getLeaderHeadInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paLeaderHeadInfo;
}

CvLeaderHeadInfo& cvInternalGlobals::getLeaderHeadInfo(LeaderHeadTypes eLeaderHeadNum) const
{
	FAssertMsg(eLeaderHeadNum >= 0 && eLeaderHeadNum < GC.getNumLeaderHeadInfos(), "LeaderHeadInfo index out of bounds");
	return *(m_paLeaderHeadInfo[eLeaderHeadNum]);
}

CvInfoReplacements<CvLeaderHeadInfo>* cvInternalGlobals::getLeaderHeadInfoReplacements()
{
	return &m_LeaderHeadInfoReplacements;
}


int cvInternalGlobals::getNumTraitInfos() const
{
	return (int)m_paTraitInfo.size();
}

std::vector<CvTraitInfo*>& cvInternalGlobals::getTraitInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paTraitInfo;
}

CvTraitInfo& cvInternalGlobals::getTraitInfo(TraitTypes eTraitNum) const
{
	FAssertMsg(eTraitNum >= 0 && eTraitNum < GC.getNumTraitInfos(), "TraitInfo index out of bounds");
	return *(m_paTraitInfo[eTraitNum]);
}

CvInfoReplacements<CvTraitInfo>* cvInternalGlobals::getTraitInfoReplacements()
{
	return &m_TraitInfoReplacements;
}


int cvInternalGlobals::getNumCursorInfos() const
{
	return (int)m_paCursorInfo.size();
}

std::vector<CvCursorInfo*>& cvInternalGlobals::getCursorInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paCursorInfo;
}

CvCursorInfo& cvInternalGlobals::getCursorInfo(CursorTypes eCursorNum) const
{
	FAssertMsg(eCursorNum >= 0 && eCursorNum < GC.getNumCursorInfos(), "CursorInfo index out of bounds");
	return *(m_paCursorInfo[eCursorNum]);
}

int cvInternalGlobals::getNumThroneRoomCameras() const
{
	return (int)m_paThroneRoomCamera.size();
}

std::vector<CvThroneRoomCamera*>& cvInternalGlobals::getThroneRoomCameras()	// For Moose - XML Load Util, CvInfos
{
	return m_paThroneRoomCamera;
}

CvThroneRoomCamera& cvInternalGlobals::getThroneRoomCamera(int iIndex) const
{
	FAssertMsg(iIndex >= 0 && iIndex < GC.getNumThroneRoomCameras(), "ThroneRoomCamera index out of bounds");
	return *(m_paThroneRoomCamera[iIndex]);
}

int cvInternalGlobals::getNumThroneRoomInfos() const
{
	return (int)m_paThroneRoomInfo.size();
}

std::vector<CvThroneRoomInfo*>& cvInternalGlobals::getThroneRoomInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paThroneRoomInfo;
}

CvThroneRoomInfo& cvInternalGlobals::getThroneRoomInfo(int iIndex) const
{
	FAssertMsg(iIndex >= 0 && iIndex < GC.getNumThroneRoomInfos(), "ThroneRoomInfo index out of bounds");
	return *(m_paThroneRoomInfo[iIndex]);
}

int cvInternalGlobals::getNumThroneRoomStyleInfos() const
{
	return (int)m_paThroneRoomStyleInfo.size();
}

std::vector<CvThroneRoomStyleInfo*>& cvInternalGlobals::getThroneRoomStyleInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paThroneRoomStyleInfo;
}

CvThroneRoomStyleInfo& cvInternalGlobals::getThroneRoomStyleInfo(int iIndex) const
{
	FAssertMsg(iIndex >= 0 && iIndex < GC.getNumThroneRoomStyleInfos(), "ThroneRoomStyleInfo index out of bounds");
	return *(m_paThroneRoomStyleInfo[iIndex]);
}

int cvInternalGlobals::getNumSlideShowInfos() const
{
	return (int)m_paSlideShowInfo.size();
}

std::vector<CvSlideShowInfo*>& cvInternalGlobals::getSlideShowInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paSlideShowInfo;
}

CvSlideShowInfo& cvInternalGlobals::getSlideShowInfo(int iIndex) const
{
	FAssertMsg(iIndex >= 0 && iIndex < GC.getNumSlideShowInfos(), "SlideShowInfo index out of bounds");
	return *(m_paSlideShowInfo[iIndex]);
}

int cvInternalGlobals::getNumSlideShowRandomInfos() const
{
	return (int)m_paSlideShowRandomInfo.size();
}

std::vector<CvSlideShowRandomInfo*>& cvInternalGlobals::getSlideShowRandomInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paSlideShowRandomInfo;
}

CvSlideShowRandomInfo& cvInternalGlobals::getSlideShowRandomInfo(int iIndex) const
{
	FAssertMsg(iIndex >= 0 && iIndex < GC.getNumSlideShowRandomInfos(), "SlideShowRandomInfo index out of bounds");
	return *(m_paSlideShowRandomInfo[iIndex]);
}

int cvInternalGlobals::getNumWorldPickerInfos() const
{
	return (int)m_paWorldPickerInfo.size();
}

std::vector<CvWorldPickerInfo*>& cvInternalGlobals::getWorldPickerInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paWorldPickerInfo;
}

CvWorldPickerInfo& cvInternalGlobals::getWorldPickerInfo(int iIndex) const
{
	FAssertMsg(iIndex >= 0 && iIndex < GC.getNumWorldPickerInfos(), "WorldPickerInfo index out of bounds");
	return *(m_paWorldPickerInfo[iIndex]);
}

int cvInternalGlobals::getNumSpaceShipInfos() const
{
	return (int)m_paSpaceShipInfo.size();
}

std::vector<CvSpaceShipInfo*>& cvInternalGlobals::getSpaceShipInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paSpaceShipInfo;
}

CvSpaceShipInfo& cvInternalGlobals::getSpaceShipInfo(int iIndex) const
{
	FAssertMsg(iIndex >= 0 && iIndex < GC.getNumSpaceShipInfos(), "SpaceShipInfo index out of bounds");
	return *(m_paSpaceShipInfo[iIndex]);
}

int cvInternalGlobals::getNumUnitInfos() const
{
	return (int)m_paUnitInfo.size();
}

std::vector<CvUnitInfo*>& cvInternalGlobals::getUnitInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paUnitInfo;
}

CvUnitInfo& cvInternalGlobals::getUnitInfo(UnitTypes eUnitNum) const
{
	FAssertMsg(eUnitNum >= 0 && eUnitNum < GC.getNumUnitInfos(), "UnitInfo index out of bounds");
	return *(m_paUnitInfo[eUnitNum]);
}

CvInfoReplacements<CvUnitInfo>* cvInternalGlobals::getUnitInfoReplacements()
{
	return &m_UnitInfoReplacements;
}

int cvInternalGlobals::getNumSpawnInfos() const
{
	return (int)m_paSpawnInfo.size();
}

std::vector<CvSpawnInfo*>& cvInternalGlobals::getSpawnInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paSpawnInfo;
}

CvSpawnInfo& cvInternalGlobals::getSpawnInfo(SpawnTypes eSpawnNum) const
{
	FAssertMsg(eSpawnNum >= 0 && eSpawnNum < GC.getNumSpawnInfos(), "SpawnInfo index out of bounds");
	return *(m_paSpawnInfo[eSpawnNum]);
}

CvInfoReplacements<CvSpawnInfo>* cvInternalGlobals::getSpawnInfoReplacements()
{
	return &m_SpawnInfoReplacements;
}

int cvInternalGlobals::getNumSpecialUnitInfos() const
{
	return (int)m_paSpecialUnitInfo.size();
}

std::vector<CvSpecialUnitInfo*>& cvInternalGlobals::getSpecialUnitInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paSpecialUnitInfo;
}

CvSpecialUnitInfo& cvInternalGlobals::getSpecialUnitInfo(SpecialUnitTypes eSpecialUnitNum) const
{
	FAssertMsg(eSpecialUnitNum >= 0 && eSpecialUnitNum < GC.getNumSpecialUnitInfos(), "SpecialUnitInfo index out of bounds");
	return *(m_paSpecialUnitInfo[eSpecialUnitNum]);
}


int cvInternalGlobals::getNumConceptInfos() const
{
	return (int)m_paConceptInfo.size();
}

std::vector<CvInfoBase*>& cvInternalGlobals::getConceptInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paConceptInfo;
}

CvInfoBase& cvInternalGlobals::getConceptInfo(ConceptTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumConceptInfos(), "ConceptInfo index out of bounds");
	return *(m_paConceptInfo[e]);
}


int cvInternalGlobals::getNumNewConceptInfos() const
{
	return (int)m_paNewConceptInfo.size();
}

std::vector<CvInfoBase*>& cvInternalGlobals::getNewConceptInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paNewConceptInfo;
}

CvInfoBase& cvInternalGlobals::getNewConceptInfo(NewConceptTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumNewConceptInfos(), "NewConceptInfo index out of bounds");
	return *(m_paNewConceptInfo[e]);
}


int cvInternalGlobals::getNumCityTabInfos() const
{
	return (int)m_paCityTabInfo.size();
}

std::vector<CvInfoBase*>& cvInternalGlobals::getCityTabInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paCityTabInfo;
}

CvInfoBase& cvInternalGlobals::getCityTabInfo(CityTabTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumCityTabInfos(), "CityTabInfo index out of bounds");
	return *(m_paCityTabInfo[e]);
}


int cvInternalGlobals::getNumCalendarInfos() const
{
	return (int)m_paCalendarInfo.size();
}

std::vector<CvInfoBase*>& cvInternalGlobals::getCalendarInfos()
{
	return m_paCalendarInfo;
}

CvInfoBase& cvInternalGlobals::getCalendarInfo(CalendarTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumCalendarInfos(), "CalendarInfo index out of bounds");
	return *(m_paCalendarInfo[e]);
}


int cvInternalGlobals::getNumSeasonInfos() const
{
	return (int)m_paSeasonInfo.size();
}

std::vector<CvInfoBase*>& cvInternalGlobals::getSeasonInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paSeasonInfo;
}

CvInfoBase& cvInternalGlobals::getSeasonInfo(SeasonTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumSeasonInfos(), "SeasonInfo index out of bounds");
	return *(m_paSeasonInfo[e]);
}


int cvInternalGlobals::getNumMonthInfos() const
{
	return (int)m_paMonthInfo.size();
}

std::vector<CvInfoBase*>& cvInternalGlobals::getMonthInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paMonthInfo;
}

CvInfoBase& cvInternalGlobals::getMonthInfo(MonthTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumMonthInfos(), "MonthInfo index out of bounds");
	return *(m_paMonthInfo[e]);
}


int cvInternalGlobals::getNumDenialInfos() const
{
	return (int)m_paDenialInfo.size();
}

std::vector<CvInfoBase*>& cvInternalGlobals::getDenialInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paDenialInfo;
}

CvInfoBase& cvInternalGlobals::getDenialInfo(DenialTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumDenialInfos(), "DenialInfo index out of bounds");
	return *(m_paDenialInfo[e]);
}


int cvInternalGlobals::getNumInvisibleInfos() const
{
	return (int)m_paInvisibleInfo.size();
}

std::vector<CvInvisibleInfo*>& cvInternalGlobals::getInvisibleInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paInvisibleInfo;
}

CvInvisibleInfo& cvInternalGlobals::getInvisibleInfo(InvisibleTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumInvisibleInfos(), "InvisibleInfo index out of bounds");
	return *(m_paInvisibleInfo[e]);
}


int cvInternalGlobals::getNumVoteSourceInfos() const
{
	return (int)m_paVoteSourceInfo.size();
}

std::vector<CvVoteSourceInfo*>& cvInternalGlobals::getVoteSourceInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paVoteSourceInfo;
}

CvVoteSourceInfo& cvInternalGlobals::getVoteSourceInfo(VoteSourceTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumVoteSourceInfos(), "VoteSourceInfo index out of bounds");
	return *(m_paVoteSourceInfo[e]);
}


int cvInternalGlobals::getNumUnitCombatInfos() const
{
	return (int)m_paUnitCombatInfo.size();
}

std::vector<CvUnitCombatInfo*>& cvInternalGlobals::getUnitCombatInfos()
{
	return m_paUnitCombatInfo;
}

CvUnitCombatInfo& cvInternalGlobals::getUnitCombatInfo(UnitCombatTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumUnitCombatInfos(), "UnitCombatInfo index out of bounds");
	return *(m_paUnitCombatInfo[e]);
}


std::vector<CvInfoBase*>& cvInternalGlobals::getDomainInfos()
{
	return m_paDomainInfo;
}

CvInfoBase& cvInternalGlobals::getDomainInfo(DomainTypes e) const
{
	FAssertMsg(e >= 0 && e < NUM_DOMAIN_TYPES, "DomainInfo index out of bounds");
	return *(m_paDomainInfo[e]);
}

//TB Promotion Line Mod begin
int cvInternalGlobals::getNumPromotionLineInfos() const
{
	return (int)m_paPromotionLineInfo.size();
}

std::vector<CvPromotionLineInfo*>& cvInternalGlobals::getPromotionLineInfos()
{
	return m_paPromotionLineInfo;
}

CvPromotionLineInfo& cvInternalGlobals::getPromotionLineInfo(PromotionLineTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumPromotionLineInfos(), "PromotionLineInfo index out of bounds");
	return *(m_paPromotionLineInfo[e]);
}
//TB Promotion Line Mod end

int cvInternalGlobals::getNumMapCategoryInfos() const
{
	return (int)m_paMapCategoryInfo.size();
}

std::vector<CvMapCategoryInfo*>& cvInternalGlobals::getMapCategoryInfos()
{
	return m_paMapCategoryInfo;
}

CvMapCategoryInfo& cvInternalGlobals::getMapCategoryInfo(MapCategoryTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumMapCategoryInfos(), "MapCategoryInfo index out of bounds");
	return *(m_paMapCategoryInfo[e]);
}

int cvInternalGlobals::getNumIdeaClassInfos() const
{
	return (int)m_paIdeaClassInfo.size();
}

std::vector<CvIdeaClassInfo*>& cvInternalGlobals::getIdeaClassInfos()
{
	return m_paIdeaClassInfo;
}

CvIdeaClassInfo& cvInternalGlobals::getIdeaClassInfo(IdeaClassTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumIdeaClassInfos(), "IdeaClassInfo index out of bounds");
	return *(m_paIdeaClassInfo[e]);
}

int cvInternalGlobals::getNumIdeaInfos() const
{
	return (int)m_paIdeaInfo.size();
}

std::vector<CvIdeaInfo*>& cvInternalGlobals::getIdeaInfos()
{
	return m_paIdeaInfo;
}

CvIdeaInfo& cvInternalGlobals::getIdeaInfo(IdeaTypes e) const
{
	FAssertMsg(e >= 0 && e < GC.getNumIdeaInfos(), "IdeaInfo index out of bounds");
	return *(m_paIdeaInfo[e]);
}
//int cvInternalGlobals::getNumTraitOptionEditsInfos() const
//{
//	return (int)m_paTraitOptionEditsInfo.size();
//}
//
//std::vector<CvTraitOptionEditsInfo*>& cvInternalGlobals::getTraitOptionEditsInfo()
//{
//	return m_paTraitOptionEditsInfo;
//}
//
//CvTraitOptionEditsInfo& cvInternalGlobals::getTraitOptionEditsInfo(TraitOptionEditsTypes e) const
//{
//	FAssert(e > -1);
//	FAssert(e < GC.getNumTraitOptionEditsInfos());
//	return *(m_paTraitOptionEditsInfo[e]);
//}

std::vector<CvInfoBase*>& cvInternalGlobals::getUnitAIInfos()
{
	return m_paUnitAIInfos;
}

CvInfoBase& cvInternalGlobals::getUnitAIInfo(UnitAITypes eUnitAINum) const
{
	FAssertMsg(eUnitAINum >= 0 && eUnitAINum < NUM_UNITAI_TYPES, "UnitAIInfo index out of bounds");
	return *(m_paUnitAIInfos[eUnitAINum]);
}

//	Koshling - added internal registration of supported UnitAI types, not reliant
//	on external definition in XML
void cvInternalGlobals::registerUnitAI(const char* szType, int enumVal)
{
	FAssertMsg(m_paUnitAIInfos.size() == enumVal, "enumVal not expected value");

	CvInfoBase* entry = new	CvInfoBase(szType);

	m_paUnitAIInfos.push_back(entry);
	setInfoTypeFromString(szType, enumVal);
}

#define	REGISTER_UNITAI(x)	registerUnitAI(#x,x)

void cvInternalGlobals::registerUnitAIs()
{
	//	Sadly C++ doesn't have any reflection capability so need to do this explicitly
	REGISTER_UNITAI(UNITAI_UNKNOWN);
	REGISTER_UNITAI(UNITAI_ANIMAL);
	REGISTER_UNITAI(UNITAI_SETTLE);
	REGISTER_UNITAI(UNITAI_WORKER);
	REGISTER_UNITAI(UNITAI_ATTACK);
	REGISTER_UNITAI(UNITAI_ATTACK_CITY);
	REGISTER_UNITAI(UNITAI_COLLATERAL);
	REGISTER_UNITAI(UNITAI_PILLAGE);
	REGISTER_UNITAI(UNITAI_RESERVE);
	REGISTER_UNITAI(UNITAI_COUNTER);
	REGISTER_UNITAI(UNITAI_CITY_DEFENSE);
	REGISTER_UNITAI(UNITAI_CITY_COUNTER);
	REGISTER_UNITAI(UNITAI_CITY_SPECIAL);
	REGISTER_UNITAI(UNITAI_EXPLORE);
	REGISTER_UNITAI(UNITAI_MISSIONARY);
	REGISTER_UNITAI(UNITAI_PROPHET);
	REGISTER_UNITAI(UNITAI_ARTIST);
	REGISTER_UNITAI(UNITAI_SCIENTIST);
	REGISTER_UNITAI(UNITAI_GENERAL);
	REGISTER_UNITAI(UNITAI_MERCHANT);
	REGISTER_UNITAI(UNITAI_ENGINEER);
	REGISTER_UNITAI(UNITAI_SPY);
	REGISTER_UNITAI(UNITAI_ICBM);
	REGISTER_UNITAI(UNITAI_WORKER_SEA);
	REGISTER_UNITAI(UNITAI_ATTACK_SEA);
	REGISTER_UNITAI(UNITAI_RESERVE_SEA);
	REGISTER_UNITAI(UNITAI_ESCORT_SEA);
	REGISTER_UNITAI(UNITAI_EXPLORE_SEA);
	REGISTER_UNITAI(UNITAI_ASSAULT_SEA);
	REGISTER_UNITAI(UNITAI_SETTLER_SEA);
	REGISTER_UNITAI(UNITAI_MISSIONARY_SEA);
	REGISTER_UNITAI(UNITAI_SPY_SEA);
	REGISTER_UNITAI(UNITAI_CARRIER_SEA);
	REGISTER_UNITAI(UNITAI_MISSILE_CARRIER_SEA);
	REGISTER_UNITAI(UNITAI_PIRATE_SEA);
	REGISTER_UNITAI(UNITAI_ATTACK_AIR);
	REGISTER_UNITAI(UNITAI_DEFENSE_AIR);
	REGISTER_UNITAI(UNITAI_CARRIER_AIR);
	REGISTER_UNITAI(UNITAI_MISSILE_AIR);
	REGISTER_UNITAI(UNITAI_PARADROP);
	REGISTER_UNITAI(UNITAI_ATTACK_CITY_LEMMING);
	REGISTER_UNITAI(UNITAI_PILLAGE_COUNTER);
	REGISTER_UNITAI(UNITAI_SUBDUED_ANIMAL);
	REGISTER_UNITAI(UNITAI_HUNTER);
	REGISTER_UNITAI(UNITAI_GREAT_HUNTER);
	REGISTER_UNITAI(UNITAI_GREAT_ADMIRAL);
	REGISTER_UNITAI(UNITAI_PROPERTY_CONTROL);
	REGISTER_UNITAI(UNITAI_HEALER);
	REGISTER_UNITAI(UNITAI_PROPERTY_CONTROL_SEA);
	REGISTER_UNITAI(UNITAI_HEALER_SEA);
	REGISTER_UNITAI(UNITAI_HUNTER_ESCORT);
	REGISTER_UNITAI(UNITAI_BARB_CRIMINAL);
	REGISTER_UNITAI(UNITAI_INVESTIGATOR);
	REGISTER_UNITAI(UNITAI_INFILTRATOR);
	REGISTER_UNITAI(UNITAI_SEE_INVISIBLE);
	REGISTER_UNITAI(UNITAI_SEE_INVISIBLE_SEA);
	REGISTER_UNITAI(UNITAI_ESCORT);
}

//	AIAndy - added internal registration of supported AIScale types similar to UnitAIs but without info class
#define	REGISTER_AISCALE(x)	setInfoTypeFromString(#x,x)

void cvInternalGlobals::registerAIScales()
{
	//	Sadly C++ doesn't have any reflection capability so need to do this explicitly
	REGISTER_AISCALE(AISCALE_NONE);
	REGISTER_AISCALE(AISCALE_CITY);
	REGISTER_AISCALE(AISCALE_AREA);
	REGISTER_AISCALE(AISCALE_PLAYER);
	REGISTER_AISCALE(AISCALE_TEAM);
}

//	AIAndy: Register game object types
#define	REGISTER_GAMEOBJECT(x)	setInfoTypeFromString(#x,x)

void cvInternalGlobals::registerGameObjects()
{
	//	Sadly C++ doesn't have any reflection capability so need to do this explicitly
	REGISTER_GAMEOBJECT(NO_GAMEOBJECT);
	REGISTER_GAMEOBJECT(GAMEOBJECT_GAME);
	REGISTER_GAMEOBJECT(GAMEOBJECT_TEAM);
	REGISTER_GAMEOBJECT(GAMEOBJECT_PLAYER);
	REGISTER_GAMEOBJECT(GAMEOBJECT_CITY);
	REGISTER_GAMEOBJECT(GAMEOBJECT_UNIT);
	REGISTER_GAMEOBJECT(GAMEOBJECT_PLOT);
}

//	AIAndy: Register game object types
#define	REGISTER_GOM(x)	setInfoTypeFromString(#x,x)

void cvInternalGlobals::registerGOMs()
{
	//	Sadly C++ doesn't have any reflection capability so need to do this explicitly
	REGISTER_GOM(NO_GOM);
	REGISTER_GOM(GOM_BUILDING);
	REGISTER_GOM(GOM_PROMOTION);
	REGISTER_GOM(GOM_TRAIT);
	REGISTER_GOM(GOM_FEATURE);
	REGISTER_GOM(GOM_OPTION);
	REGISTER_GOM(GOM_TERRAIN);
	REGISTER_GOM(GOM_GAMESPEED);
	REGISTER_GOM(GOM_ROUTE);
	REGISTER_GOM(GOM_BONUS);
	REGISTER_GOM(GOM_UNITTYPE);
	REGISTER_GOM(GOM_TECH);
	REGISTER_GOM(GOM_CIVIC);
	REGISTER_GOM(GOM_RELIGION);
	REGISTER_GOM(GOM_CORPORATION);
	REGISTER_GOM(GOM_IMPROVEMENT);
	REGISTER_GOM(GOM_UNITCOMBAT);
	REGISTER_GOM(GOM_HANDICAP);
}

//	AIAndy: Register game object relation types
#define	REGISTER_RELATION(x)	setInfoTypeFromString(#x,x)

void cvInternalGlobals::registerRelations()
{
	//	Sadly C++ doesn't have any reflection capability so need to do this explicitly
	REGISTER_RELATION(NO_RELATION);
	REGISTER_RELATION(RELATION_ASSOCIATED);
	REGISTER_RELATION(RELATION_TRADE);
	REGISTER_RELATION(RELATION_NEAR);
	REGISTER_RELATION(RELATION_SAME_PLOT);
	REGISTER_RELATION(RELATION_WORKING);
}

//	AIAndy: Register game object attribute types
#define	REGISTER_ATTRIBUTE(x)	setInfoTypeFromString(#x,x)

void cvInternalGlobals::registerAttributes()
{
	//	Sadly C++ doesn't have any reflection capability so need to do this explicitly
	REGISTER_ATTRIBUTE(NO_ATTRIBUTE);
	REGISTER_ATTRIBUTE(ATTRIBUTE_POPULATION);
	REGISTER_ATTRIBUTE(ATTRIBUTE_HEALTH);
	REGISTER_ATTRIBUTE(ATTRIBUTE_HAPPINESS);
	REGISTER_ATTRIBUTE(ATTRIBUTE_PLAYERS);
	REGISTER_ATTRIBUTE(ATTRIBUTE_TEAMS);
}

//	AIAndy: Register game object tag types
#define	REGISTER_TAG(x)	setInfoTypeFromString(#x,x)

void cvInternalGlobals::registerTags()
{
	//	Sadly C++ doesn't have any reflection capability so need to do this explicitly
	REGISTER_TAG(NO_TAG);
	REGISTER_TAG(TAG_ONLY_DEFENSIVE);
	REGISTER_TAG(TAG_SPY);
	REGISTER_TAG(TAG_FIRST_STRIKE_IMMUNE);
	REGISTER_TAG(TAG_NO_DEFENSIVE_BONUS);
	REGISTER_TAG(TAG_CAN_MOVE_IMPASSABLE);
	REGISTER_TAG(TAG_HIDDEN_NATIONALITY);
	REGISTER_TAG(TAG_BLITZ);
	REGISTER_TAG(TAG_ALWAYS_HEAL);
	REGISTER_TAG(TAG_ENEMY_ROUTE);
	REGISTER_TAG(TAG_WATER);
	REGISTER_TAG(TAG_FRESH_WATER);
	REGISTER_TAG(TAG_PEAK);
	REGISTER_TAG(TAG_HILL);
	REGISTER_TAG(TAG_FLATLAND);
	REGISTER_TAG(TAG_OWNED);
	REGISTER_TAG(TAG_CITY);
	REGISTER_TAG(TAG_ANARCHY);
	REGISTER_TAG(TAG_COASTAL);
}

//	AIAndy: Register property source types
#define	REGISTER_PROPERTYSOURCE(x)	setInfoTypeFromString(#x,x)

void cvInternalGlobals::registerPropertySources()
{
	//	Sadly C++ doesn't have any reflection capability so need to do this explicitly
	REGISTER_PROPERTYSOURCE(NO_PROPERTYSOURCE);
	REGISTER_PROPERTYSOURCE(PROPERTYSOURCE_CONSTANT);
	REGISTER_PROPERTYSOURCE(PROPERTYSOURCE_CONSTANT_LIMITED);
	REGISTER_PROPERTYSOURCE(PROPERTYSOURCE_DECAY);
	REGISTER_PROPERTYSOURCE(PROPERTYSOURCE_ATTRIBUTE_CONSTANT);
}

//	AIAndy: Register property interaction types
#define	REGISTER_PROPERTYINTERACTION(x)	setInfoTypeFromString(#x,x)

void cvInternalGlobals::registerPropertyInteractions()
{
	//	Sadly C++ doesn't have any reflection capability so need to do this explicitly
	REGISTER_PROPERTYINTERACTION(NO_PROPERTYINTERACTION);
	REGISTER_PROPERTYINTERACTION(PROPERTYINTERACTION_CONVERT_CONSTANT);
	REGISTER_PROPERTYINTERACTION(PROPERTYINTERACTION_INHIBITED_GROWTH);
	REGISTER_PROPERTYINTERACTION(PROPERTYINTERACTION_CONVERT_PERCENT);
}

//	AIAndy: Register property propagator types
#define	REGISTER_PROPERTYPROPAGATOR(x)	setInfoTypeFromString(#x,x)

void cvInternalGlobals::registerPropertyPropagators()
{
	//	Sadly C++ doesn't have any reflection capability so need to do this explicitly
	REGISTER_PROPERTYPROPAGATOR(NO_PROPERTYPROPAGATOR);
	REGISTER_PROPERTYPROPAGATOR(PROPERTYPROPAGATOR_SPREAD);
	REGISTER_PROPERTYPROPAGATOR(PROPERTYPROPAGATOR_GATHER);
	REGISTER_PROPERTYPROPAGATOR(PROPERTYPROPAGATOR_DIFFUSE);
}

//	AlbertS2: Register mission types
void cvInternalGlobals::registerMission(const char* szType, int enumVal)
{
	FAssert(m_paMissionInfo.size() == enumVal);

	CvMissionInfo* entry = new CvMissionInfo(szType);

	m_paMissionInfo.push_back(entry);
	setInfoTypeFromString(szType, enumVal);
}

#define	REGISTER_MISSION(x)	registerMission(#x,x)

void cvInternalGlobals::registerMissions()
{
	//	Sadly C++ doesn't have any reflection capability so need to do this explicitly
	REGISTER_MISSION(MISSION_MOVE_TO);
	REGISTER_MISSION(MISSION_ROUTE_TO);
	REGISTER_MISSION(MISSION_MOVE_TO_UNIT);
	REGISTER_MISSION(MISSION_SKIP);
	REGISTER_MISSION(MISSION_SLEEP);
	REGISTER_MISSION(MISSION_FORTIFY);
	REGISTER_MISSION(MISSION_PLUNDER);
	REGISTER_MISSION(MISSION_AIRPATROL);
	REGISTER_MISSION(MISSION_SEAPATROL);
	REGISTER_MISSION(MISSION_HEAL);
	REGISTER_MISSION(MISSION_SENTRY);
	REGISTER_MISSION(MISSION_AIRLIFT);
	REGISTER_MISSION(MISSION_NUKE);
	REGISTER_MISSION(MISSION_RECON);
	REGISTER_MISSION(MISSION_PARADROP);
	REGISTER_MISSION(MISSION_AIRBOMB);
	REGISTER_MISSION(MISSION_RANGE_ATTACK);
	REGISTER_MISSION(MISSION_BOMBARD);
	REGISTER_MISSION(MISSION_PILLAGE);
	REGISTER_MISSION(MISSION_SABOTAGE);
	REGISTER_MISSION(MISSION_DESTROY);
	REGISTER_MISSION(MISSION_STEAL_PLANS);
	REGISTER_MISSION(MISSION_FOUND);
	REGISTER_MISSION(MISSION_SPREAD);
	REGISTER_MISSION(MISSION_SPREAD_CORPORATION);
	REGISTER_MISSION(MISSION_JOIN);
	REGISTER_MISSION(MISSION_CONSTRUCT);
	REGISTER_MISSION(MISSION_DISCOVER);
	REGISTER_MISSION(MISSION_HURRY);
	REGISTER_MISSION(MISSION_TRADE);
	REGISTER_MISSION(MISSION_GREAT_WORK);
	REGISTER_MISSION(MISSION_INFILTRATE);
	REGISTER_MISSION(MISSION_GOLDEN_AGE);
	REGISTER_MISSION(MISSION_BUILD);
	REGISTER_MISSION(MISSION_LEAD);
	REGISTER_MISSION(MISSION_ESPIONAGE);
	REGISTER_MISSION(MISSION_DIE_ANIMATION);
	REGISTER_MISSION(MISSION_BEGIN_COMBAT);
	REGISTER_MISSION(MISSION_END_COMBAT);
	REGISTER_MISSION(MISSION_AIRSTRIKE);
	REGISTER_MISSION(MISSION_SURRENDER);
	REGISTER_MISSION(MISSION_CAPTURED);
	REGISTER_MISSION(MISSION_IDLE);
	REGISTER_MISSION(MISSION_DIE);
	REGISTER_MISSION(MISSION_DAMAGE);
	REGISTER_MISSION(MISSION_MULTI_SELECT);
	REGISTER_MISSION(MISSION_MULTI_DESELECT);
	REGISTER_MISSION(MISSION_AIRBOMB1);
	REGISTER_MISSION(MISSION_AIRBOMB2);
	REGISTER_MISSION(MISSION_AIRBOMB3);
	REGISTER_MISSION(MISSION_AIRBOMB4);
	REGISTER_MISSION(MISSION_AIRBOMB5);
	REGISTER_MISSION(MISSION_RBOMBARD);
	REGISTER_MISSION(MISSION_FENGAGE);
	// BUG - Sentry Actions - start
#ifdef _MOD_SENTRY
	REGISTER_MISSION(MISSION_MOVE_TO_SENTRY);
	REGISTER_MISSION(MISSION_SENTRY_WHILE_HEAL);
	REGISTER_MISSION(MISSION_SENTRY_NAVAL_UNITS);
	REGISTER_MISSION(MISSION_SENTRY_LAND_UNITS);
#endif
	// BUG - Sentry Actions - end

	REGISTER_MISSION(MISSION_INQUISITION);
	REGISTER_MISSION(MISSION_CLAIM_TERRITORY);
	REGISTER_MISSION(MISSION_HURRY_FOOD);
	REGISTER_MISSION(MISSION_ESPIONAGE_SLEEP);
	REGISTER_MISSION(MISSION_GREAT_COMMANDER);
	REGISTER_MISSION(MISSION_SHADOW);
	REGISTER_MISSION(MISSION_WAIT_FOR_TECH);
	REGISTER_MISSION(MISSION_GOTO);
	REGISTER_MISSION(MISSION_PRETARGET_NUKE);
	REGISTER_MISSION(MISSION_BUTCHER);
	REGISTER_MISSION(MISSION_DIPLOMAT_ASSIMULATE_IND_PEOPLE);
	REGISTER_MISSION(MISSION_DIPLOMAT_PRAISE_IND_PEOPLE);
	REGISTER_MISSION(MISSION_DIPLOMAT_SPEAK_TO_BARBARIAN_LEADERS);
	REGISTER_MISSION(MISSION_DIPLOMAT_SPREAD_RELIGION);
	REGISTER_MISSION(MISSION_LAWYER_REMOVE_CORPORATIONS);
	REGISTER_MISSION(MISSION_JOIN_CITY_POPULATION);
	REGISTER_MISSION(MISSION_CURE);
	REGISTER_MISSION(MISSION_ESTABLISH);
	REGISTER_MISSION(MISSION_ESCAPE);
	REGISTER_MISSION(MISSION_BUILDUP);
	REGISTER_MISSION(MISSION_AUTO_BUILDUP);
	REGISTER_MISSION(MISSION_HEAL_BUILDUP);
	REGISTER_MISSION(MISSION_AMBUSH);
	REGISTER_MISSION(MISSION_ASSASSINATE);
	REGISTER_MISSION(MISSION_ENTERTAIN_CITY);
	REGISTER_MISSION(MISSION_HURRY_PRODUCTION_CARAVAN);
	REGISTER_MISSION(MISSION_CAPTIVE_UPGRADE_TO_SETTLER);
	REGISTER_MISSION(MISSION_CAPTIVE_UPGRADE_TO_GATHERER);
	REGISTER_MISSION(MISSION_CAPTIVE_UPGRADE_TO_WORKER);
	REGISTER_MISSION(MISSION_CAPTIVE_UPGRADE_TO_IMMIGRANT);
	REGISTER_MISSION(MISSION_CAPTIVE_UPGRADE_TO_STONE_THROWER);
	REGISTER_MISSION(MISSION_CAPTIVE_UPGRADE_TO_ARCHER);
	REGISTER_MISSION(MISSION_CAPTIVE_UPGRADE_TO_AXEMAN);
	REGISTER_MISSION(MISSION_CAPTIVE_UPGRADE_TO_SPEARMAN);
	REGISTER_MISSION(MISSION_CAPTIVE_UPGRADE_TO_NEANDERTHAL_WARRIOR);
	REGISTER_MISSION(MISSION_SELL_CAPTIVE);
	REGISTER_MISSION(MISSION_FREE_CAPTIVE);
	REGISTER_MISSION(MISSION_JOIN_CITY_FREED_SLAVE);
	REGISTER_MISSION(MISSION_RECORD_TALE);
	REGISTER_MISSION(MISSION_RECORD_TALE_ORAL);
	REGISTER_MISSION(MISSION_RECORD_TALE_WRITTEN);
	REGISTER_MISSION(MISSION_ANIMAL_COMBAT);
	REGISTER_MISSION(MISSION_ANIMAL_STUDY);
	REGISTER_MISSION(MISSION_ANIMAL_SACRIFICE);
	REGISTER_MISSION(MISSION_BUILD_DOMESTICATED_HERD);
	REGISTER_MISSION(MISSION_CAPTIVE_UPGRADE_TO_NEANDERTHAL_GATHERER);
	REGISTER_MISSION(MISSION_CAPTIVE_UPGRADE_TO_NEANDERTHAL_TRACKER);
}

std::vector<CvInfoBase*>& cvInternalGlobals::getAttitudeInfos()
{
	return m_paAttitudeInfos;
}

CvInfoBase& cvInternalGlobals::getAttitudeInfo(AttitudeTypes eAttitudeNum) const
{
	FAssert(eAttitudeNum >= 0);
	FAssert(eAttitudeNum < NUM_ATTITUDE_TYPES);
	return *(m_paAttitudeInfos[eAttitudeNum]);
}


std::vector<CvInfoBase*>& cvInternalGlobals::getMemoryInfos()
{
	return m_paMemoryInfos;
}

CvInfoBase& cvInternalGlobals::getMemoryInfo(MemoryTypes eMemoryNum) const
{
	FAssert(eMemoryNum >= 0);
	FAssert(eMemoryNum < NUM_MEMORY_TYPES);
	return *(m_paMemoryInfos[eMemoryNum]);
}


int cvInternalGlobals::getNumGameOptionInfos() const
{
	return (int)m_paGameOptionInfos.size();
}

std::vector<CvGameOptionInfo*>& cvInternalGlobals::getGameOptionInfos()
{
	return m_paGameOptionInfos;
}

CvGameOptionInfo& cvInternalGlobals::getGameOptionInfo(GameOptionTypes eGameOptionNum) const
{
	FAssert(eGameOptionNum >= 0);
	FAssert(eGameOptionNum < GC.getNumGameOptionInfos());
	return *(m_paGameOptionInfos[eGameOptionNum]);
}

int cvInternalGlobals::getNumMPOptionInfos() const
{
	return (int)m_paMPOptionInfos.size();
}

std::vector<CvMPOptionInfo*>& cvInternalGlobals::getMPOptionInfos()
{
	 return m_paMPOptionInfos;
}

CvMPOptionInfo& cvInternalGlobals::getMPOptionInfo(MultiplayerOptionTypes eMPOptionNum) const
{
	FAssert(eMPOptionNum >= 0);
	FAssert(eMPOptionNum < GC.getNumMPOptionInfos());
	return *(m_paMPOptionInfos[eMPOptionNum]);
}

int cvInternalGlobals::getNumForceControlInfos() const
{
	return (int)m_paForceControlInfos.size();
}

std::vector<CvForceControlInfo*>& cvInternalGlobals::getForceControlInfos()
{
	return m_paForceControlInfos;
}

CvForceControlInfo& cvInternalGlobals::getForceControlInfo(ForceControlTypes eForceControlNum) const
{
	FAssert(eForceControlNum >= 0);
	FAssert(eForceControlNum < GC.getNumForceControlInfos());
	return *(m_paForceControlInfos[eForceControlNum]);
}

std::vector<CvPlayerOptionInfo*>& cvInternalGlobals::getPlayerOptionInfos()
{
	return m_paPlayerOptionInfos;
}

CvPlayerOptionInfo& cvInternalGlobals::getPlayerOptionInfo(PlayerOptionTypes ePlayerOptionNum) const
{
	FAssert(ePlayerOptionNum >= 0);
	FAssert(ePlayerOptionNum < NUM_PLAYEROPTION_TYPES);
	return *(m_paPlayerOptionInfos[ePlayerOptionNum]);
}

std::vector<CvGraphicOptionInfo*>& cvInternalGlobals::getGraphicOptionInfos()
{
	return m_paGraphicOptionInfos;
}

CvGraphicOptionInfo& cvInternalGlobals::getGraphicOptionInfo(GraphicOptionTypes eGraphicOptionNum) const
{
	FAssert(eGraphicOptionNum >= 0);
	FAssert(eGraphicOptionNum < NUM_GRAPHICOPTION_TYPES);
	return *(m_paGraphicOptionInfos[eGraphicOptionNum]);
}


std::vector<CvYieldInfo*>& cvInternalGlobals::getYieldInfos()	// For Moose - XML Load Util
{
	return m_paYieldInfo;
}

CvYieldInfo& cvInternalGlobals::getYieldInfo(YieldTypes eYieldNum) const
{
	FAssert(eYieldNum > -1);
	FAssert(eYieldNum < NUM_YIELD_TYPES);
	return *(m_paYieldInfo[eYieldNum]);
}


std::vector<CvCommerceInfo*>& cvInternalGlobals::getCommerceInfos()	// For Moose - XML Load Util
{
	return m_paCommerceInfo;
}

CvCommerceInfo& cvInternalGlobals::getCommerceInfo(CommerceTypes eCommerceNum) const
{
	FAssert(eCommerceNum > -1);
	FAssert(eCommerceNum < NUM_COMMERCE_TYPES);
	return *(m_paCommerceInfo[eCommerceNum]);
}

int cvInternalGlobals::getNumRouteInfos() const
{
	return (int)m_paRouteInfo.size();
}

std::vector<CvRouteInfo*>& cvInternalGlobals::getRouteInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paRouteInfo;
}

CvRouteInfo& cvInternalGlobals::getRouteInfo(RouteTypes eRouteNum) const
{
	FAssert(eRouteNum > -1);
	FAssert(eRouteNum < GC.getNumRouteInfos());
	return *(m_paRouteInfo[eRouteNum]);
}

CvInfoReplacements<CvRouteInfo>* cvInternalGlobals::getRouteInfoReplacements()
{
	return &m_RouteInfoReplacements;
}

int cvInternalGlobals::getNumImprovementInfos() const
{
	return (int)m_paImprovementInfo.size();
}

std::vector<CvImprovementInfo*>& cvInternalGlobals::getImprovementInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paImprovementInfo;
}

CvImprovementInfo& cvInternalGlobals::getImprovementInfo(ImprovementTypes eImprovementNum) const
{
	FAssert(eImprovementNum > -1);
	FAssert(eImprovementNum < GC.getNumImprovementInfos());
	return *(m_paImprovementInfo[eImprovementNum]);
}

CvInfoReplacements<CvImprovementInfo>* cvInternalGlobals::getImprovementInfoReplacements()
{
	return &m_ImprovementInfoReplacements;
}

int cvInternalGlobals::getNumGoodyInfos() const
{
	return (int)m_paGoodyInfo.size();
}

std::vector<CvGoodyInfo*>& cvInternalGlobals::getGoodyInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paGoodyInfo;
}

CvGoodyInfo& cvInternalGlobals::getGoodyInfo(GoodyTypes eGoodyNum) const
{
	FAssert(eGoodyNum > -1);
	FAssert(eGoodyNum < GC.getNumGoodyInfos());
	return *(m_paGoodyInfo[eGoodyNum]);
}

int cvInternalGlobals::getNumBuildInfos() const
{
	return (int)m_paBuildInfo.size();
}

std::vector<CvBuildInfo*>& cvInternalGlobals::getBuildInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paBuildInfo;
}

CvBuildInfo& cvInternalGlobals::getBuildInfo(BuildTypes eBuildNum) const
{
	FAssert(eBuildNum > -1);
	FAssert(eBuildNum < GC.getNumBuildInfos());
	return *(m_paBuildInfo[eBuildNum]);
}

CvInfoReplacements<CvBuildInfo>* cvInternalGlobals::getBuildInfoReplacements()
{
	return &m_BuildInfoReplacements;
}

int cvInternalGlobals::getNumHandicapInfos() const
{
	return (int)m_paHandicapInfo.size();
}

std::vector<CvHandicapInfo*>& cvInternalGlobals::getHandicapInfos()	// Do NOT export outside of the DLL	// For Moose - XML Load Util
{
	return m_paHandicapInfo;
}

CvHandicapInfo& cvInternalGlobals::getHandicapInfo(HandicapTypes eHandicapNum) const
{
	FAssert(eHandicapNum > -1);
	FAssert(eHandicapNum < GC.getNumHandicapInfos());
	return *(m_paHandicapInfo[eHandicapNum]);
}

CvInfoReplacements<CvHandicapInfo>* cvInternalGlobals::getHandicapInfoReplacements()
{
	return &m_HandicapInfoReplacements;
}

int cvInternalGlobals::getNumGameSpeedInfos() const
{
	return (int)m_paGameSpeedInfo.size();
}

std::vector<CvGameSpeedInfo*>& cvInternalGlobals::getGameSpeedInfos()	// Do NOT export outside of the DLL	// For Moose - XML Load Util
{
	return m_paGameSpeedInfo;
}

CvGameSpeedInfo& cvInternalGlobals::getGameSpeedInfo(GameSpeedTypes eGameSpeedNum) const
{
	FAssert(eGameSpeedNum > -1);
	FAssert(eGameSpeedNum < GC.getNumGameSpeedInfos());
	return *(m_paGameSpeedInfo[eGameSpeedNum]);
}

CvInfoReplacements<CvGameSpeedInfo>* cvInternalGlobals::getGameSpeedInfoReplacements()
{
	return &m_GameSpeedInfoReplacements;
}

int cvInternalGlobals::getNumTurnTimerInfos() const
{
	return (int)m_paTurnTimerInfo.size();
}

std::vector<CvTurnTimerInfo*>& cvInternalGlobals::getTurnTimerInfos()	// Do NOT export outside of the DLL	// For Moose - XML Load Util
{
	return m_paTurnTimerInfo;
}

CvTurnTimerInfo& cvInternalGlobals::getTurnTimerInfo(TurnTimerTypes eTurnTimerNum) const
{
	FAssert(eTurnTimerNum > -1);
	FAssert(eTurnTimerNum < GC.getNumTurnTimerInfos());
	return *(m_paTurnTimerInfo[eTurnTimerNum]);
}

int cvInternalGlobals::getNumProcessInfos() const
{
	return (int)m_paProcessInfo.size();
}

std::vector<CvProcessInfo*>& cvInternalGlobals::getProcessInfos()
{
	return m_paProcessInfo;
}

CvProcessInfo& cvInternalGlobals::getProcessInfo(ProcessTypes e) const
{
	FAssert(e > -1);
	FAssert(e < GC.getNumProcessInfos());
	return *(m_paProcessInfo[e]);
}

CvInfoReplacements<CvProcessInfo>* cvInternalGlobals::getProcessInfoReplacements()
{
	return &m_ProcessInfoReplacements;
}

int cvInternalGlobals::getNumVoteInfos() const
{
	return (int)m_paVoteInfo.size();
}

std::vector<CvVoteInfo*>& cvInternalGlobals::getVoteInfos()
{
	return m_paVoteInfo;
}

CvVoteInfo& cvInternalGlobals::getVoteInfo(VoteTypes e) const
{
	FAssert(e > -1);
	FAssert(e < GC.getNumVoteInfos());
	return *(m_paVoteInfo[e]);
}

int cvInternalGlobals::getNumProjectInfos() const
{
	return (int)m_paProjectInfo.size();
}

std::vector<CvProjectInfo*>& cvInternalGlobals::getProjectInfos()
{
	return m_paProjectInfo;
}

CvProjectInfo& cvInternalGlobals::getProjectInfo(ProjectTypes e) const
{
	FAssert(e > -1);
	FAssert(e < GC.getNumProjectInfos());
	return *(m_paProjectInfo[e]);
}

CvInfoReplacements<CvProjectInfo>* cvInternalGlobals::getProjectInfoReplacements()
{
	return &m_ProjectInfoReplacements;
}

int cvInternalGlobals::getNumBuildingInfos() const
{
	return (int)m_paBuildingInfo.size();
}

std::vector<CvBuildingInfo*>& cvInternalGlobals::getBuildingInfos()	// For Moose - XML Load Util, CvInfos, CvCacheObject
{
	return m_paBuildingInfo;
}

CvBuildingInfo& cvInternalGlobals::getBuildingInfo(BuildingTypes eBuildingNum) const
{
	FAssert(eBuildingNum > -1);
	FAssert(eBuildingNum < GC.getNumBuildingInfos());
	return *(m_paBuildingInfo[eBuildingNum]);
}

CvInfoReplacements<CvBuildingInfo>* cvInternalGlobals::getBuildingInfoReplacements()
{
	return &m_BuildingInfoReplacements;
}

int cvInternalGlobals::getNumSpecialBuildingInfos() const
{
	return (int)m_paSpecialBuildingInfo.size();
}

std::vector<CvSpecialBuildingInfo*>& cvInternalGlobals::getSpecialBuildingInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paSpecialBuildingInfo;
}

CvSpecialBuildingInfo& cvInternalGlobals::getSpecialBuildingInfo(SpecialBuildingTypes eSpecialBuildingNum) const
{
	FAssert(eSpecialBuildingNum > -1);
	FAssert(eSpecialBuildingNum < GC.getNumSpecialBuildingInfos());
	return *(m_paSpecialBuildingInfo[eSpecialBuildingNum]);
}

CvInfoReplacements<CvSpecialBuildingInfo>* cvInternalGlobals::getSpecialBuildingInfoReplacements()
{
	return &m_SpecialBuildingInfoReplacements;
}

int cvInternalGlobals::getNumActionInfos() const
{
	return (int)m_paActionInfo.size();
}

std::vector<CvActionInfo*>& cvInternalGlobals::getActionInfos()	// For Moose - XML Load Util
{
	return m_paActionInfo;
}

CvActionInfo& cvInternalGlobals::getActionInfo(int i) const
{
	FAssertMsg(i < getNumActionInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return *(m_paActionInfo[i]);
}

std::vector<CvMissionInfo*>& cvInternalGlobals::getMissionInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paMissionInfo;
}

CvMissionInfo& cvInternalGlobals::getMissionInfo(MissionTypes eMissionNum) const
{
	FAssert(eMissionNum > -1);
	FAssert(eMissionNum < NUM_MISSION_TYPES);
	return *(m_paMissionInfo[eMissionNum]);
}

std::vector<CvControlInfo*>& cvInternalGlobals::getControlInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paControlInfo;
}

CvControlInfo& cvInternalGlobals::getControlInfo(ControlTypes eControlNum) const
{
	FAssert(eControlNum > -1);
	FAssert(eControlNum < NUM_CONTROL_TYPES);
	FAssert(!m_paControlInfo.empty());
	return *(m_paControlInfo[eControlNum]);
}

std::vector<CvCommandInfo*>& cvInternalGlobals::getCommandInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paCommandInfo;
}

CvCommandInfo& cvInternalGlobals::getCommandInfo(CommandTypes eCommandNum) const
{
	FAssert(eCommandNum > -1);
	FAssert(eCommandNum < NUM_COMMAND_TYPES);
	return *(m_paCommandInfo[eCommandNum]);
}

int cvInternalGlobals::getNumAutomateInfos() const
{
	return (int)m_paAutomateInfo.size();
}

std::vector<CvAutomateInfo*>& cvInternalGlobals::getAutomateInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paAutomateInfo;
}

CvAutomateInfo& cvInternalGlobals::getAutomateInfo(int iAutomateNum) const
{
	FAssertMsg(iAutomateNum < getNumAutomateInfos(), "Index out of bounds");
	FAssertMsg(iAutomateNum > -1, "Index out of bounds");
	return *(m_paAutomateInfo[iAutomateNum]);
}

int cvInternalGlobals::getNumPromotionInfos() const
{
	return (int)m_paPromotionInfo.size();
}

std::vector<CvPromotionInfo*>& cvInternalGlobals::getPromotionInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paPromotionInfo;
}

CvPromotionInfo& cvInternalGlobals::getPromotionInfo(PromotionTypes ePromotionNum) const
{
	FAssert(ePromotionNum > -1);
	FAssert(ePromotionNum < GC.getNumPromotionInfos());
	return *(m_paPromotionInfo[ePromotionNum]);
}

CvInfoReplacements<CvPromotionInfo>* cvInternalGlobals::getPromotionInfoReplacements()
{
	return &m_PromotionInfoReplacements;
}

PromotionTypes cvInternalGlobals::findPromotion(PromotionPredicateFn predicateFn) const
{
	for (int idx = 0; idx < static_cast<int>(m_paPromotionInfo.size()); ++idx)
	{
		if (predicateFn(m_paPromotionInfo[idx], static_cast<PromotionTypes>(idx)))
		{
			return static_cast<PromotionTypes>(idx);
		}
	}
	return static_cast<PromotionTypes>(-1);
}

int cvInternalGlobals::getNumTechInfos() const
{
	return (int)m_paTechInfo.size();
}

std::vector<CvTechInfo*>& cvInternalGlobals::getTechInfos()	// For Moose - XML Load Util, CvInfos, CvCacheObject
{
	return m_paTechInfo;
}

CvTechInfo& cvInternalGlobals::getTechInfo(TechTypes eTechNum) const
{
	FAssert(eTechNum > -1);
	FAssert(eTechNum < GC.getNumTechInfos());
	return *(m_paTechInfo[eTechNum]);
}

CvInfoReplacements<CvTechInfo>* cvInternalGlobals::getTechInfoReplacements()
{
	return &m_TechInfoReplacements;
}

int cvInternalGlobals::getNumReligionInfos() const
{
	return (int)m_paReligionInfo.size();
}

std::vector<CvReligionInfo*>& cvInternalGlobals::getReligionInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paReligionInfo;
}

CvReligionInfo& cvInternalGlobals::getReligionInfo(ReligionTypes eReligionNum) const
{
	FAssert(eReligionNum > -1);
	FAssert(eReligionNum < GC.getNumReligionInfos());
	return *(m_paReligionInfo[eReligionNum]);
}

CvInfoReplacements<CvReligionInfo>* cvInternalGlobals::getReligionInfoReplacements()
{
	return &m_ReligionInfoReplacements;
}

int cvInternalGlobals::getNumCorporationInfos() const
{
	return (int)m_paCorporationInfo.size();
}

std::vector<CvCorporationInfo*>& cvInternalGlobals::getCorporationInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paCorporationInfo;
}

CvCorporationInfo& cvInternalGlobals::getCorporationInfo(CorporationTypes eCorporationNum) const
{
	FAssert(eCorporationNum > -1);
	FAssert(eCorporationNum < GC.getNumCorporationInfos());
	return *(m_paCorporationInfo[eCorporationNum]);
}

CvInfoReplacements<CvCorporationInfo>* cvInternalGlobals::getCorporationInfoReplacements()
{
	return &m_CorporationInfoReplacements;
}

int cvInternalGlobals::getNumSpecialistInfos() const
{
	return (int)m_paSpecialistInfo.size();
}

std::vector<CvSpecialistInfo*>& cvInternalGlobals::getSpecialistInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paSpecialistInfo;
}

CvSpecialistInfo& cvInternalGlobals::getSpecialistInfo(SpecialistTypes eSpecialistNum) const
{
	FAssert(eSpecialistNum > -1);
	FAssert(eSpecialistNum < GC.getNumSpecialistInfos());
	return *(m_paSpecialistInfo[eSpecialistNum]);
}

CvInfoReplacements<CvSpecialistInfo>* cvInternalGlobals::getSpecialistInfoReplacements()
{
	return &m_SpecialistInfoReplacements;
}

int cvInternalGlobals::getNumCivicOptionInfos() const
{
	return (int)m_paCivicOptionInfo.size();
}

std::vector<CvCivicOptionInfo*>& cvInternalGlobals::getCivicOptionInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paCivicOptionInfo;
}

CvCivicOptionInfo& cvInternalGlobals::getCivicOptionInfo(CivicOptionTypes eCivicOptionNum) const
{
	FAssert(eCivicOptionNum > -1);
	FAssert(eCivicOptionNum < GC.getNumCivicOptionInfos());
	return *(m_paCivicOptionInfo[eCivicOptionNum]);
}

int cvInternalGlobals::getNumCivicInfos() const
{
	return (int)m_paCivicInfo.size();
}

std::vector<CvCivicInfo*>& cvInternalGlobals::getCivicInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paCivicInfo;
}

CvCivicInfo& cvInternalGlobals::getCivicInfo(CivicTypes eCivicNum) const
{
	FAssert(eCivicNum > -1);
	FAssert(eCivicNum < GC.getNumCivicInfos());
	return *(m_paCivicInfo[eCivicNum]);
}

CvInfoReplacements<CvCivicInfo>* cvInternalGlobals::getCivicInfoReplacements()
{
	return &m_CivicInfoReplacements;
}

int cvInternalGlobals::getNumDiplomacyInfos() const
{
	return (int)m_paDiplomacyInfo.size();
}

std::vector<CvDiplomacyInfo*>& cvInternalGlobals::getDiplomacyInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paDiplomacyInfo;
}

CvDiplomacyInfo& cvInternalGlobals::getDiplomacyInfo(int iDiplomacyNum) const
{
	FAssertMsg(iDiplomacyNum < getNumDiplomacyInfos(), "Index out of bounds");
	FAssertMsg(iDiplomacyNum > -1, "Index out of bounds");
	return *(m_paDiplomacyInfo[iDiplomacyNum]);
}

int cvInternalGlobals::getNumEraInfos() const
{
	return (int)m_aEraInfo.size();
}

std::vector<CvEraInfo*>& cvInternalGlobals::getEraInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_aEraInfo;
}

CvEraInfo& cvInternalGlobals::getEraInfo(EraTypes eEraNum) const
{
	FAssert(eEraNum > -1);
	FAssert(eEraNum < GC.getNumEraInfos());
	return *(m_aEraInfo[eEraNum]);
}

CvInfoReplacements<CvEraInfo>* cvInternalGlobals::getEraInfoReplacements()
{
	return &m_EraInfoReplacements;
}

int cvInternalGlobals::getNumHurryInfos() const
{
	return (int)m_paHurryInfo.size();
}

std::vector<CvHurryInfo*>& cvInternalGlobals::getHurryInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paHurryInfo;
}

CvHurryInfo& cvInternalGlobals::getHurryInfo(HurryTypes eHurryNum) const
{
	FAssert(eHurryNum > -1);
	FAssert(eHurryNum < GC.getNumHurryInfos());
	return *(m_paHurryInfo[eHurryNum]);
}

int cvInternalGlobals::getNumEmphasizeInfos() const
{
	return (int)m_paEmphasizeInfo.size();
}

std::vector<CvEmphasizeInfo*>& cvInternalGlobals::getEmphasizeInfos()	// For Moose - XML Load Util
{
	return m_paEmphasizeInfo;
}

CvEmphasizeInfo& cvInternalGlobals::getEmphasizeInfo(EmphasizeTypes eEmphasizeNum) const
{
	FAssert(eEmphasizeNum > -1);
	FAssert(eEmphasizeNum < GC.getNumEmphasizeInfos());
	return *(m_paEmphasizeInfo[eEmphasizeNum]);
}

int cvInternalGlobals::getNumUpkeepInfos() const
{
	return (int)m_paUpkeepInfo.size();
}

std::vector<CvUpkeepInfo*>& cvInternalGlobals::getUpkeepInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paUpkeepInfo;
}

CvUpkeepInfo& cvInternalGlobals::getUpkeepInfo(UpkeepTypes eUpkeepNum) const
{
	FAssert(eUpkeepNum > -1);
	FAssert(eUpkeepNum < GC.getNumUpkeepInfos());
	return *(m_paUpkeepInfo[eUpkeepNum]);
}

int cvInternalGlobals::getNumCultureLevelInfos() const
{
	return (int)m_paCultureLevelInfo.size();
}

std::vector<CvCultureLevelInfo*>& cvInternalGlobals::getCultureLevelInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paCultureLevelInfo;
}

CvCultureLevelInfo& cvInternalGlobals::getCultureLevelInfo(CultureLevelTypes eCultureLevelNum) const
{
	FAssert(eCultureLevelNum > -1);
	FAssert(eCultureLevelNum < GC.getNumCultureLevelInfos());
	return *(m_paCultureLevelInfo[eCultureLevelNum]);
}

CvInfoReplacements<CvCultureLevelInfo>* cvInternalGlobals::getCultureLevelInfoReplacements()
{
	return &m_CultureLevelInfoReplacements;
}

int cvInternalGlobals::getNumVictoryInfos() const
{
	return (int)m_paVictoryInfo.size();
}

std::vector<CvVictoryInfo*>& cvInternalGlobals::getVictoryInfos()	// For Moose - XML Load Util, CvInfos
{
	return m_paVictoryInfo;
}

CvVictoryInfo& cvInternalGlobals::getVictoryInfo(VictoryTypes eVictoryNum) const
{
	FAssert(eVictoryNum > -1);
	FAssert(eVictoryNum < GC.getNumVictoryInfos());
	return *(m_paVictoryInfo[eVictoryNum]);
}

int cvInternalGlobals::getNumQuestInfos() const
{
	return (int)m_paQuestInfo.size();
}

std::vector<CvQuestInfo*>& cvInternalGlobals::getQuestInfos()
{
	return m_paQuestInfo;
}

CvQuestInfo& cvInternalGlobals::getQuestInfo(int iIndex) const
{
	FAssert(iIndex > -1);
	FAssert(iIndex < GC.getNumQuestInfos());
	return *(m_paQuestInfo[iIndex]);
}

int cvInternalGlobals::getNumTutorialInfos() const
{
	return (int)m_paTutorialInfo.size();
}

std::vector<CvTutorialInfo*>& cvInternalGlobals::getTutorialInfos()
{
	return m_paTutorialInfo;
}

CvTutorialInfo& cvInternalGlobals::getTutorialInfo(int iIndex) const
{
	FAssert(iIndex > -1);
	FAssert(iIndex < GC.getNumTutorialInfos());
	return *(m_paTutorialInfo[iIndex]);
}

int cvInternalGlobals::getNumEventTriggerInfos() const
{
	return (int)m_paEventTriggerInfo.size();
}

std::vector<CvEventTriggerInfo*>& cvInternalGlobals::getEventTriggerInfos()
{
	return m_paEventTriggerInfo;
}

CvEventTriggerInfo& cvInternalGlobals::getEventTriggerInfo(EventTriggerTypes eEventTrigger) const
{
	FAssert(eEventTrigger > -1);
	FAssert(eEventTrigger < GC.getNumEventTriggerInfos());
	return *(m_paEventTriggerInfo[eEventTrigger]);
}

CvInfoReplacements<CvEventTriggerInfo>* cvInternalGlobals::getEventTriggerInfoReplacements()
{
	return &m_EventTriggerInfoReplacements;
}

int cvInternalGlobals::getNumEventInfos() const
{
	return (int)m_paEventInfo.size();
}

std::vector<CvEventInfo*>& cvInternalGlobals::getEventInfos()
{
	return m_paEventInfo;
}

CvEventInfo& cvInternalGlobals::getEventInfo(EventTypes eEvent) const
{
	FAssert(eEvent > -1);
	FAssert(eEvent < GC.getNumEventInfos());
	return *(m_paEventInfo[eEvent]);
}

CvInfoReplacements<CvEventInfo>* cvInternalGlobals::getEventInfoReplacements()
{
	return &m_EventInfoReplacements;
}

int cvInternalGlobals::getNumEspionageMissionInfos() const
{
	return (int)m_paEspionageMissionInfo.size();
}

std::vector<CvEspionageMissionInfo*>& cvInternalGlobals::getEspionageMissionInfos()
{
	return m_paEspionageMissionInfo;
}

CvEspionageMissionInfo& cvInternalGlobals::getEspionageMissionInfo(EspionageMissionTypes eEspionageMissionNum) const
{
	FAssert(eEspionageMissionNum > -1);
	FAssert(eEspionageMissionNum < GC.getNumEspionageMissionInfos());
	return *(m_paEspionageMissionInfo[eEspionageMissionNum]);
}

int& cvInternalGlobals::getNumEntityEventTypes()
{
	return m_iNumEntityEventTypes;
}

CvString*& cvInternalGlobals::getEntityEventTypes()
{
	return m_paszEntityEventTypes;
}

CvString& cvInternalGlobals::getEntityEventTypes(EntityEventTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumEntityEventTypes());
	return m_paszEntityEventTypes[e];
}

int& cvInternalGlobals::getNumAnimationOperatorTypes()
{
	return m_iNumAnimationOperatorTypes;
}

CvString*& cvInternalGlobals::getAnimationOperatorTypes()
{
	return m_paszAnimationOperatorTypes;
}

CvString& cvInternalGlobals::getAnimationOperatorTypes(AnimationOperatorTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumAnimationOperatorTypes());
	return m_paszAnimationOperatorTypes[e];
}

CvString*& cvInternalGlobals::getFunctionTypes()
{
	return m_paszFunctionTypes;
}

CvString& cvInternalGlobals::getFunctionTypes(FunctionTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_FUNC_TYPES);
	return m_paszFunctionTypes[e];
}

int& cvInternalGlobals::getNumFlavorTypes()
{
	return m_iNumFlavorTypes;
}

CvString*& cvInternalGlobals::getFlavorTypes()
{
	return m_paszFlavorTypes;
}

CvString& cvInternalGlobals::getFlavorTypes(FlavorTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumFlavorTypes());
	return m_paszFlavorTypes[e];
}

int& cvInternalGlobals::getNumArtStyleTypes()
{
	return m_iNumArtStyleTypes;
}

CvString*& cvInternalGlobals::getArtStyleTypes()
{
	return m_paszArtStyleTypes;
}

CvString& cvInternalGlobals::getArtStyleTypes(ArtStyleTypes e)
{
	FAssert(e > -1);
	FAssert(e < GC.getNumArtStyleTypes());
	return m_paszArtStyleTypes[e];
}

int cvInternalGlobals::getNumUnitArtStyleTypeInfos() const
{
	return (int)m_paUnitArtStyleTypeInfo.size();
}

std::vector<CvUnitArtStyleTypeInfo*>& cvInternalGlobals::getUnitArtStyleTypeInfos()
{
	return m_paUnitArtStyleTypeInfo;
}

CvUnitArtStyleTypeInfo& cvInternalGlobals::getUnitArtStyleTypeInfo(UnitArtStyleTypes eUnitArtStyleTypeNum) const
{
	FAssert(eUnitArtStyleTypeNum > -1);
	FAssert(eUnitArtStyleTypeNum < GC.getNumUnitArtStyleTypeInfos());
	return *(m_paUnitArtStyleTypeInfo[eUnitArtStyleTypeNum]);
}

int& cvInternalGlobals::getNumCitySizeTypes()
{
	return m_iNumCitySizeTypes;
}

CvString*& cvInternalGlobals::getCitySizeTypes()
{
	return m_paszCitySizeTypes;
}

CvString& cvInternalGlobals::getCitySizeTypes(int i)
{
	FAssertMsg(i < getNumCitySizeTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paszCitySizeTypes[i];
}

CvString*& cvInternalGlobals::getContactTypes()
{
	return m_paszContactTypes;
}

CvString& cvInternalGlobals::getContactTypes(ContactTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_CONTACT_TYPES);
	return m_paszContactTypes[e];
}

CvString*& cvInternalGlobals::getDiplomacyPowerTypes()
{
	return m_paszDiplomacyPowerTypes;
}

CvString& cvInternalGlobals::getDiplomacyPowerTypes(DiplomacyPowerTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_DIPLOMACYPOWER_TYPES);
	return m_paszDiplomacyPowerTypes[e];
}

CvString*& cvInternalGlobals::getAutomateTypes()
{
	return m_paszAutomateTypes;
}

CvString& cvInternalGlobals::getAutomateTypes(AutomateTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_AUTOMATE_TYPES);
	return m_paszAutomateTypes[e];
}

CvString*& cvInternalGlobals::getDirectionTypes()
{
	return m_paszDirectionTypes;
}

CvString& cvInternalGlobals::getDirectionTypes(AutomateTypes e)
{
	FAssert(e > -1);
	FAssert(e < NUM_AUTOMATE_TYPES);
	return m_paszDirectionTypes[e];
}

int cvInternalGlobals::getNumPropertyInfos() const
{
	return (int)m_paPropertyInfo.size();
}

std::vector<CvPropertyInfo*>& cvInternalGlobals::getPropertyInfos()
{
	return m_paPropertyInfo;
}

CvPropertyInfo& cvInternalGlobals::getPropertyInfo(PropertyTypes ePropertyNum) const
{
	FAssert(ePropertyNum > -1);
	FAssert(ePropertyNum < GC.getNumPropertyInfos());
	return *(m_paPropertyInfo[ePropertyNum]);
}

int cvInternalGlobals::getNumOutcomeInfos() const
{
	return (int)m_paOutcomeInfo.size();
}

std::vector<CvOutcomeInfo*>& cvInternalGlobals::getOutcomeInfos()
{
	return m_paOutcomeInfo;
}

CvOutcomeInfo& cvInternalGlobals::getOutcomeInfo(OutcomeTypes eOutcomeNum) const
{
	FAssert(eOutcomeNum > -1);
	FAssert(eOutcomeNum < GC.getNumOutcomeInfos());
	return *(m_paOutcomeInfo[eOutcomeNum]);
}

int& cvInternalGlobals::getNumFootstepAudioTypes()
{
	return m_iNumFootstepAudioTypes;
}

CvString*& cvInternalGlobals::getFootstepAudioTypes()
{
	return m_paszFootstepAudioTypes;
}

CvString& cvInternalGlobals::getFootstepAudioTypes(int i)
{
	FAssertMsg(i < getNumFootstepAudioTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paszFootstepAudioTypes[i];
}

int cvInternalGlobals::getFootstepAudioTypeByTag(CvString strTag)
{
	int iIndex = -1;

	if ( strTag.GetLength() <= 0 )
	{
		return iIndex;
	}

	for ( int i = 0; i < m_iNumFootstepAudioTypes; i++ )
	{
		if ( strTag.CompareNoCase(m_paszFootstepAudioTypes[i]) == 0 )
		{
			iIndex = i;
			break;
		}
	}

	return iIndex;
}

CvString*& cvInternalGlobals::getFootstepAudioTags()
{
	return m_paszFootstepAudioTags;
}

CvString& cvInternalGlobals::getFootstepAudioTags(int i)
{
	static CvString*	emptyString = NULL;

	if ( emptyString == NULL )
	{
		emptyString = new CvString("");
	}
	FAssertMsg(i < GC.getNumFootstepAudioTypes(), "Index out of bounds")
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paszFootstepAudioTags ? m_paszFootstepAudioTags[i] : *emptyString;
}

void cvInternalGlobals::setCurrentXMLFile(const TCHAR* szFileName)
{
	m_szCurrentXMLFile = szFileName;
}

CvString& cvInternalGlobals::getCurrentXMLFile()
{
	return m_szCurrentXMLFile;
}

FVariableSystem* cvInternalGlobals::getDefinesVarSystem() const
{
	return m_VarSystem;
}

void cvInternalGlobals::cacheGlobals()
{
	OutputDebugString("Caching Globals: Start");

	strcpy(gVersionString, getDefineSTRING("C2C_VERSION"));

	/************************************************************************************************/
	/* Mod Globals    Start                          09/13/10                           phungus420  */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/
	m_bDCM_BATTLE_EFFECTS = (getDefineINT("DCM_BATTLE_EFFECTS") > 0) ? true : false;
	m_iBATTLE_EFFECT_LESS_FOOD = getDefineINT("BATTLE_EFFECT_LESS_FOOD");
	m_iBATTLE_EFFECT_LESS_PRODUCTION = getDefineINT("BATTLE_EFFECT_LESS_PRODUCTION");
	m_iBATTLE_EFFECT_LESS_COMMERCE = getDefineINT("BATTLE_EFFECT_LESS_COMMERCE");
	m_iBATTLE_EFFECTS_MINIMUM_TURN_INCREMENTS = getDefineINT("BATTLE_EFFECTS_MINIMUM_TURN_INCREMENTS");
	m_iMAX_BATTLE_TURNS = getDefineINT("MAX_BATTLE_TURNS");

	m_bDCM_AIR_BOMBING = (getDefineINT("DCM_AIR_BOMBING") > 0) ? true : false;
	m_bDCM_RANGE_BOMBARD = (getDefineINT("DCM_RANGE_BOMBARD") > 0) ? true : false;
	m_iDCM_RB_CITY_INACCURACY = getDefineINT("DCM_RB_CITY_INACCURACY");
	m_iDCM_RB_CITYBOMBARD_CHANCE = getDefineINT("DCM_RB_CITYBOMBARD_CHANCE");
	m_bDCM_ATTACK_SUPPORT = (getDefineINT("DCM_ATTACK_SUPPORT") > 0) ? true : false;
	m_bDCM_OPP_FIRE = (getDefineINT("DCM_OPP_FIRE") > 0) ? true : false;
	m_bDCM_ACTIVE_DEFENSE = (getDefineINT("DCM_ACTIVE_DEFENSE") > 0) ? true : false;
	m_bDCM_FIGHTER_ENGAGE = (getDefineINT("DCM_FIGHTER_ENGAGE") > 0) ? true : false;

	m_bDYNAMIC_CIV_NAMES = (getDefineINT("DYNAMIC_CIV_NAMES") > 0) ? true : false;

	m_bLIMITED_RELIGIONS_EXCEPTIONS = (getDefineINT("LIMITED_RELIGIONS_EXCEPTIONS") > 0) ? true : false;
	m_bOC_RESPAWN_HOLY_CITIES = (getDefineINT("OC_RESPAWN_HOLY_CITIES") > 0) ? true : false;

	m_bIDW_ENABLED = (getDefineINT("IDW_ENABLED") > 0) ? true : false;
	m_fIDW_BASE_COMBAT_INFLUENCE = getDefineFLOAT("IDW_BASE_COMBAT_INFLUENCE");
	m_fIDW_NO_CITY_DEFENDER_MULTIPLIER = getDefineFLOAT("IDW_NO_CITY_DEFENDER_MULTIPLIER");
	m_fIDW_FORT_CAPTURE_MULTIPLIER = getDefineFLOAT("IDW_FORT_CAPTURE_MULTIPLIER");
	m_fIDW_EXPERIENCE_FACTOR = getDefineFLOAT("IDW_EXPERIENCE_FACTOR");
	m_fIDW_WARLORD_MULTIPLIER = getDefineFLOAT("IDW_WARLORD_MULTIPLIER");
	m_iIDW_INFLUENCE_RADIUS = getDefineINT("IDW_INFLUENCE_RADIUS");
	m_fIDW_PLOT_DISTANCE_FACTOR = getDefineFLOAT("IDW_PLOT_DISTANCE_FACTOR");
	m_fIDW_WINNER_PLOT_MULTIPLIER = getDefineFLOAT("IDW_WINNER_PLOT_MULTIPLIER");
	m_fIDW_LOSER_PLOT_MULTIPLIER = getDefineFLOAT("IDW_LOSER_PLOT_MULTIPLIER");
	m_bIDW_EMERGENCY_DRAFT_ENABLED = (getDefineINT("IDW_EMERGENCY_DRAFT_ENABLED") > 0) ? true : false;
	m_iIDW_EMERGENCY_DRAFT_MIN_POPULATION = (getDefineINT("IDW_EMERGENCY_DRAFT_MIN_POPULATION") > 1) ? getDefineINT("IDW_EMERGENCY_DRAFT_ENABLED") : 2;
	m_fIDW_EMERGENCY_DRAFT_STRENGTH = getDefineFLOAT("IDW_EMERGENCY_DRAFT_STRENGTH");
	m_fIDW_EMERGENCY_DRAFT_ANGER_MULTIPLIER = getDefineFLOAT("IDW_EMERGENCY_DRAFT_ANGER_MULTIPLIER");
	m_bIDW_NO_BARBARIAN_INFLUENCE = (getDefineINT("IDW_NO_BARBARIAN_INFLUENCE") > 0) ? true : false;
	m_bIDW_NO_NAVAL_INFLUENCE = (getDefineINT("IDW_NO_NAVAL_INFLUENCE") > 0) ? true : false;
	m_bIDW_PILLAGE_INFLUENCE_ENABLED = (getDefineINT("IDW_PILLAGE_INFLUENCE_ENABLED") > 0) ? true : false;
	m_fIDW_BASE_PILLAGE_INFLUENCE = getDefineFLOAT("IDW_BASE_PILLAGE_INFLUENCE");
	m_fIDW_CITY_TILE_MULTIPLIER = getDefineFLOAT("IDW_CITY_TILE_MULTIPLIER");

	m_bSS_ENABLED = (getDefineINT("SS_ENABLED") > 0) ? true : false;
	m_bSS_BRIBE = (getDefineINT("SS_BRIBE") > 0) ? true : false;
	m_bSS_ASSASSINATE = (getDefineINT("SS_ASSASSINATE") > 0) ? true : false;
/************************************************************************************************/
/* Mod Globals                        END                                           phungus420  */
/************************************************************************************************/
	m_iMOVE_DENOMINATOR = getDefineINT("MOVE_DENOMINATOR");
	m_iNUM_UNIT_PREREQ_OR_BONUSES = getDefineINT("NUM_UNIT_PREREQ_OR_BONUSES");
	m_iNUM_BUILDING_PREREQ_OR_BONUSES = getDefineINT("NUM_BUILDING_PREREQ_OR_BONUSES");
	m_iFOOD_CONSUMPTION_PER_POPULATION = getDefineINT("FOOD_CONSUMPTION_PER_POPULATION");
	m_iMAX_HIT_POINTS = getDefineINT("MAX_HIT_POINTS");
	m_iPATH_DAMAGE_WEIGHT = getDefineINT("PATH_DAMAGE_WEIGHT");
	m_iHILLS_EXTRA_DEFENSE = getDefineINT("HILLS_EXTRA_DEFENSE");
	m_iRIVER_ATTACK_MODIFIER = getDefineINT("RIVER_ATTACK_MODIFIER");
	m_iAMPHIB_ATTACK_MODIFIER = getDefineINT("AMPHIB_ATTACK_MODIFIER");
	m_iHILLS_EXTRA_MOVEMENT = getDefineINT("HILLS_EXTRA_MOVEMENT");
	m_iRIVER_EXTRA_MOVEMENT = getDefineINT("RIVER_EXTRA_MOVEMENT");
	m_iMAX_PLOT_LIST_ROWS = getDefineINT("MAX_PLOT_LIST_ROWS");
	m_iUNIT_MULTISELECT_MAX = getDefineINT("UNIT_MULTISELECT_MAX");
	m_iPERCENT_ANGER_DIVISOR = getDefineINT("PERCENT_ANGER_DIVISOR");
	m_iEVENT_MESSAGE_TIME = getDefineINT("EVENT_MESSAGE_TIME");
	m_iROUTE_FEATURE_GROWTH_MODIFIER = getDefineINT("ROUTE_FEATURE_GROWTH_MODIFIER");
	m_iFEATURE_GROWTH_MODIFIER = getDefineINT("FEATURE_GROWTH_MODIFIER");
	m_iMIN_CITY_RANGE = getDefineINT("MIN_CITY_RANGE");
	m_iCITY_MAX_NUM_BUILDINGS = getDefineINT("CITY_MAX_NUM_BUILDINGS");
	m_iNUM_UNIT_AND_TECH_PREREQS = getDefineINT("NUM_UNIT_AND_TECH_PREREQS");
	m_iNUM_AND_TECH_PREREQS = getDefineINT("NUM_AND_TECH_PREREQS");
	m_iNUM_OR_TECH_PREREQS = getDefineINT("NUM_OR_TECH_PREREQS");
	m_iLAKE_MAX_AREA_SIZE = getDefineINT("LAKE_MAX_AREA_SIZE");
	m_iNUM_ROUTE_PREREQ_OR_BONUSES = getDefineINT("NUM_ROUTE_PREREQ_OR_BONUSES");
	m_iNUM_BUILDING_AND_TECH_PREREQS = getDefineINT("NUM_BUILDING_AND_TECH_PREREQS");
	m_iMIN_WATER_SIZE_FOR_OCEAN = getDefineINT("MIN_WATER_SIZE_FOR_OCEAN");
	m_iFORTIFY_MODIFIER_PER_TURN = getDefineINT("FORTIFY_MODIFIER_PER_TURN");
	m_iESTABLISH_MODIFIER_PER_TURN = getDefineINT("ESTABLISH_MODIFIER_PER_TURN");
	m_iESCAPE_MODIFIER_PER_TURN = getDefineINT("ESCAPE_MODIFIER_PER_TURN");
	m_iMAX_CITY_DEFENSE_DAMAGE = getDefineINT("MAX_CITY_DEFENSE_DAMAGE");
	m_iNUM_CORPORATION_PREREQ_BONUSES = getDefineINT("NUM_CORPORATION_PREREQ_BONUSES");
	m_iPEAK_SEE_THROUGH_CHANGE = getDefineINT("PEAK_SEE_THROUGH_CHANGE");
	m_iHILLS_SEE_THROUGH_CHANGE = getDefineINT("HILLS_SEE_THROUGH_CHANGE");
	m_iSEAWATER_SEE_FROM_CHANGE = getDefineINT("SEAWATER_SEE_FROM_CHANGE");
	m_iPEAK_SEE_FROM_CHANGE = getDefineINT("PEAK_SEE_FROM_CHANGE");
	m_iHILLS_SEE_FROM_CHANGE = getDefineINT("HILLS_SEE_FROM_CHANGE");
	m_iUSE_SPIES_NO_ENTER_BORDERS = getDefineINT("USE_SPIES_NO_ENTER_BORDERS");
	
	m_fCAMERA_MIN_YAW = getDefineFLOAT("CAMERA_MIN_YAW");
	m_fCAMERA_MAX_YAW = getDefineFLOAT("CAMERA_MAX_YAW");
	m_fCAMERA_FAR_CLIP_Z_HEIGHT = getDefineFLOAT("CAMERA_FAR_CLIP_Z_HEIGHT");
	m_fCAMERA_MAX_TRAVEL_DISTANCE = getDefineFLOAT("CAMERA_MAX_TRAVEL_DISTANCE");
	m_fCAMERA_START_DISTANCE = getDefineFLOAT("CAMERA_START_DISTANCE");
	m_fAIR_BOMB_HEIGHT = getDefineFLOAT("AIR_BOMB_HEIGHT");
	m_fPLOT_SIZE = getDefineFLOAT("PLOT_SIZE");
	m_fCAMERA_SPECIAL_PITCH = getDefineFLOAT("CAMERA_SPECIAL_PITCH");
	m_fCAMERA_MAX_TURN_OFFSET = getDefineFLOAT("CAMERA_MAX_TURN_OFFSET");
	m_fCAMERA_MIN_DISTANCE = getDefineFLOAT("CAMERA_MIN_DISTANCE");
	m_fCAMERA_UPPER_PITCH = getDefineFLOAT("CAMERA_UPPER_PITCH");
	m_fCAMERA_LOWER_PITCH = getDefineFLOAT("CAMERA_LOWER_PITCH");
	m_fFIELD_OF_VIEW = getDefineFLOAT("FIELD_OF_VIEW");
	m_fSHADOW_SCALE = getDefineFLOAT("SHADOW_SCALE");
	m_fUNIT_MULTISELECT_DISTANCE = getDefineFLOAT("UNIT_MULTISELECT_DISTANCE");
	m_fSAD_FACTOR_1 =getDefineFLOAT("SAD_FACTOR_1");
	m_fSAD_FACTOR_2 =getDefineFLOAT("SAD_FACTOR_2");
	m_fSAD_FACTOR_3 =getDefineFLOAT("SAD_FACTOR_3");
	m_fSAD_FACTOR_4 =getDefineFLOAT("SAD_FACTOR_4");

	m_iUSE_CANNOT_FOUND_CITY_CALLBACK = getDefineINT("USE_CANNOT_FOUND_CITY_CALLBACK");
	m_iUSE_CAN_FOUND_CITIES_ON_WATER_CALLBACK = getDefineINT("USE_CAN_FOUND_CITIES_ON_WATER_CALLBACK");
	m_iUSE_IS_PLAYER_RESEARCH_CALLBACK = getDefineINT("USE_IS_PLAYER_RESEARCH_CALLBACK");
	m_iUSE_CAN_RESEARCH_CALLBACK = getDefineINT("USE_CAN_RESEARCH_CALLBACK");
	m_iUSE_CANNOT_DO_CIVIC_CALLBACK = getDefineINT("USE_CANNOT_DO_CIVIC_CALLBACK");
	m_iUSE_CAN_DO_CIVIC_CALLBACK = getDefineINT("USE_CAN_DO_CIVIC_CALLBACK");
	m_iUSE_CANNOT_CONSTRUCT_CALLBACK = getDefineINT("USE_CANNOT_CONSTRUCT_CALLBACK");
	m_iUSE_CAN_CONSTRUCT_CALLBACK = getDefineINT("USE_CAN_CONSTRUCT_CALLBACK");
	m_iUSE_CAN_DECLARE_WAR_CALLBACK = getDefineINT("USE_CAN_DECLARE_WAR_CALLBACK");
	m_iUSE_CANNOT_RESEARCH_CALLBACK = getDefineINT("USE_CANNOT_RESEARCH_CALLBACK");
	m_iUSE_GET_UNIT_COST_MOD_CALLBACK = getDefineINT("USE_GET_UNIT_COST_MOD_CALLBACK");
	m_iUSE_GET_BUILDING_COST_MOD_CALLBACK = getDefineINT("USE_GET_BUILDING_COST_MOD_CALLBACK");
	m_iUSE_GET_CITY_FOUND_VALUE_CALLBACK = getDefineINT("USE_GET_CITY_FOUND_VALUE_CALLBACK");
	m_iUSE_CANNOT_HANDLE_ACTION_CALLBACK = getDefineINT("USE_CANNOT_HANDLE_ACTION_CALLBACK");
	m_iUSE_CAN_BUILD_CALLBACK = getDefineINT("USE_CAN_BUILD_CALLBACK");
	m_iUSE_CANNOT_TRAIN_CALLBACK = getDefineINT("USE_CANNOT_TRAIN_CALLBACK");
	m_iUSE_CAN_TRAIN_CALLBACK = getDefineINT("USE_CAN_TRAIN_CALLBACK");
	m_iUSE_UNIT_CANNOT_MOVE_INTO_CALLBACK = getDefineINT("USE_UNIT_CANNOT_MOVE_INTO_CALLBACK");
	m_iUSE_USE_CANNOT_SPREAD_RELIGION_CALLBACK = getDefineINT("USE_USE_CANNOT_SPREAD_RELIGION_CALLBACK");
	m_iUSE_FINISH_TEXT_CALLBACK = getDefineINT("USE_FINISH_TEXT_CALLBACK");
	m_iUSE_ON_UNIT_SET_XY_CALLBACK = getDefineINT("USE_ON_UNIT_SET_XY_CALLBACK");
	m_iUSE_ON_UNIT_SELECTED_CALLBACK = getDefineINT("USE_ON_UNIT_SELECTED_CALLBACK");
	m_iUSE_ON_UPDATE_CALLBACK = getDefineINT("USE_ON_UPDATE_CALLBACK");
	m_iUSE_ON_UNIT_CREATED_CALLBACK = getDefineINT("USE_ON_UNIT_CREATED_CALLBACK");
	m_iUSE_ON_UNIT_LOST_CALLBACK = getDefineINT("USE_ON_UNIT_LOST_CALLBACK");

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      02/21/10                                jdog5000      */
/*                                                                                              */
/* Efficiency, Options                                                                          */
/************************************************************************************************/
// BBAI Options
	m_bBBAI_AIR_COMBAT = !(getDefineINT("BBAI_AIR_COMBAT") == 0);
	m_bBBAI_HUMAN_VASSAL_WAR_BUILD = !(getDefineINT("BBAI_HUMAN_VASSAL_WAR_BUILD") == 0);
	m_iBBAI_DEFENSIVE_PACT_BEHAVIOR = getDefineINT("BBAI_DEFENSIVE_PACT_BEHAVIOR");
	m_bBBAI_HUMAN_AS_VASSAL_OPTION = !(getDefineINT("BBAI_HUMAN_AS_VASSAL_OPTION") == 0);

// BBAI AI Variables
	m_iWAR_SUCCESS_CITY_CAPTURING = getDefineINT("WAR_SUCCESS_CITY_CAPTURING", m_iWAR_SUCCESS_CITY_CAPTURING);
	m_iBBAI_ATTACK_CITY_STACK_RATIO = getDefineINT("BBAI_ATTACK_CITY_STACK_RATIO", m_iBBAI_ATTACK_CITY_STACK_RATIO);
	m_iBBAI_SKIP_BOMBARD_BEST_ATTACK_ODDS = getDefineINT("BBAI_SKIP_BOMBARD_BEST_ATTACK_ODDS", m_iBBAI_SKIP_BOMBARD_BEST_ATTACK_ODDS);
	m_iBBAI_SKIP_BOMBARD_BASE_STACK_RATIO = getDefineINT("BBAI_SKIP_BOMBARD_BASE_STACK_RATIO", m_iBBAI_SKIP_BOMBARD_BASE_STACK_RATIO);
	m_iBBAI_SKIP_BOMBARD_MIN_STACK_RATIO = getDefineINT("BBAI_SKIP_BOMBARD_MIN_STACK_RATIO", m_iBBAI_SKIP_BOMBARD_MIN_STACK_RATIO);

// Tech Diffusion
	m_bTECH_DIFFUSION_ENABLE = !(getDefineINT("TECH_DIFFUSION_ENABLE") == 0);
	m_iTECH_DIFFUSION_KNOWN_TEAM_MODIFIER = getDefineINT("TECH_DIFFUSION_KNOWN_TEAM_MODIFIER", m_iTECH_DIFFUSION_KNOWN_TEAM_MODIFIER);
	m_iTECH_DIFFUSION_WELFARE_THRESHOLD = getDefineINT("TECH_DIFFUSION_WELFARE_THRESHOLD", m_iTECH_DIFFUSION_WELFARE_THRESHOLD);
	m_iTECH_DIFFUSION_WELFARE_MODIFIER = getDefineINT("TECH_DIFFUSION_WELFARE_MODIFIER", m_iTECH_DIFFUSION_WELFARE_MODIFIER);
	m_iTECH_COST_FIRST_KNOWN_PREREQ_MODIFIER = getDefineINT("TECH_COST_FIRST_KNOWN_PREREQ_MODIFIER", m_iTECH_COST_FIRST_KNOWN_PREREQ_MODIFIER);
	m_iTECH_COST_KNOWN_PREREQ_MODIFIER = getDefineINT("TECH_COST_KNOWN_PREREQ_MODIFIER", m_iTECH_COST_KNOWN_PREREQ_MODIFIER);
	m_iTECH_COST_MODIFIER = getDefineINT("TECH_COST_MODIFIER", m_iTECH_COST_MODIFIER);
	m_iUNIT_PRODUCTION_PERCENT_SM = getDefineINT("UNIT_PRODUCTION_PERCENT_SM", m_iUNIT_PRODUCTION_PERCENT_SM);
	m_iUNIT_PRODUCTION_PERCENT = getDefineINT("UNIT_PRODUCTION_PERCENT", m_iUNIT_PRODUCTION_PERCENT);
	m_iBUILDING_PRODUCTION_PERCENT = getDefineINT("BUILDING_PRODUCTION_PERCENT", m_iBUILDING_PRODUCTION_PERCENT);
	
	m_iCOMBAT_DIE_SIDES = getDefineINT("COMBAT_DIE_SIDES");
	m_iCOMBAT_DAMAGE = getDefineINT("COMBAT_DAMAGE");
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

/************************************************************************************************/
/* Afforess	                  Start		 12/8/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	m_iPEAK_EXTRA_MOVEMENT = getDefineINT("PEAK_EXTRA_MOVEMENT");
	m_iPEAK_EXTRA_DEFENSE = getDefineINT("PEAK_EXTRA_DEFENSE");
	m_bXMLLogging = getDefineINT("XML_LOGGING_ENABLED");
	m_iSCORE_FREE_PERCENT = getDefineINT("SCORE_FREE_PERCENT");
	m_iSCORE_POPULATION_FACTOR = getDefineINT("SCORE_POPULATION_FACTOR");
	m_iSCORE_LAND_FACTOR = getDefineINT("SCORE_LAND_FACTOR");
	m_iSCORE_TECH_FACTOR = getDefineINT("SCORE_TECH_FACTOR");
	m_iSCORE_WONDER_FACTOR = getDefineINT("SCORE_WONDER_FACTOR");
	
	//New Python Callbacks
	m_iUSE_CAN_CREATE_PROJECT_CALLBACK = getDefineINT("USE_CAN_CREATE_PROJECT_CALLBACK");
	m_iUSE_CANNOT_CREATE_PROJECT_CALLBACK = getDefineINT("USE_CANNOT_CREATE_PROJECT_CALLBACK");
	m_iUSE_CAN_DO_MELTDOWN_CALLBACK = getDefineINT("USE_CAN_DO_MELTDOWN_CALLBACK");
	m_iUSE_CAN_MAINTAIN_PROCESS_CALLBACK = getDefineINT("USE_CAN_MAINTAIN_PROCESS_CALLBACK");
	m_iUSE_CANNOT_MAINTAIN_PROCESS_CALLBACK = getDefineINT("USE_CANNOT_MAINTAIN_PROCESS_CALLBACK");
	m_iUSE_CAN_DO_GROWTH_CALLBACK = getDefineINT("USE_CAN_DO_GROWTH_CALLBACK");
	m_iUSE_CAN_DO_CULTURE_CALLBACK = getDefineINT("USE_CAN_DO_CULTURE_CALLBACK");
	m_iUSE_CAN_DO_PLOT_CULTURE_CALLBACK = getDefineINT("USE_CAN_DO_PLOT_CULTURE_CALLBACK");
	m_iUSE_CAN_DO_PRODUCTION_CALLBACK = getDefineINT("USE_CAN_DO_PRODUCTION_CALLBACK");
	m_iUSE_CAN_DO_RELIGION_CALLBACK = getDefineINT("USE_CAN_DO_RELIGION_CALLBACK");
	m_iUSE_CAN_DO_GREATPEOPLE_CALLBACK = getDefineINT("USE_CAN_DO_GREATPEOPLE_CALLBACK");
	m_iUSE_CAN_RAZE_CITY_CALLBACK = getDefineINT("USE_CAN_RAZE_CITY_CALLBACK");
	m_iUSE_CAN_DO_GOLD_CALLBACK = getDefineINT("USE_CAN_DO_GOLD_CALLBACK");
	m_iUSE_CAN_DO_RESEARCH_CALLBACK = getDefineINT("USE_CAN_DO_RESEARCH_CALLBACK");
	m_iUSE_UPGRADE_UNIT_PRICE_CALLBACK = getDefineINT("USE_UPGRADE_UNIT_PRICE_CALLBACK");
	m_iUSE_IS_VICTORY_CALLBACK = getDefineINT("USE_IS_VICTORY_CALLBACK");
	m_iUSE_AI_UPDATE_UNIT_CALLBACK = getDefineINT("USE_AI_UPDATE_UNIT_CALLBACK");
	m_iUSE_AI_CHOOSE_PRODUCTION_CALLBACK = getDefineINT("USE_AI_CHOOSE_PRODUCTION_CALLBACK");
	m_iUSE_EXTRA_PLAYER_COSTS_CALLBACK = getDefineINT("USE_EXTRA_PLAYER_COSTS_CALLBACK");
	m_iUSE_AI_DO_DIPLO_CALLBACK = getDefineINT("USE_AI_DO_DIPLO_CALLBACK");
	m_iUSE_AI_BESTTECH_CALLBACK = getDefineINT("USE_AI_BESTTECH_CALLBACK");
	m_iUSE_CAN_DO_COMBAT_CALLBACK = getDefineINT("USE_CAN_DO_COMBAT_CALLBACK");
	m_iUSE_AI_CAN_DO_WARPLANS_CALLBACK = getDefineINT("USE_AI_CAN_DO_WARPLANS_CALLBACK");
	
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
/************************************************************************************************/
/* MODULES                                 11/13/07                            MRGENIE          */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	m_iTGA_RELIGIONS = getDefineINT("GAMEFONT_TGA_RELIGIONS");													// GAMEFONT_TGA_RELIGIONS
	m_iTGA_CORPORATIONS = getDefineINT("GAMEFONT_TGA_CORPORATIONS");											// GAMEFONT_TGA_CORPORATIONS
/************************************************************************************************/
/* MODULES                                 END                                                  */
/************************************************************************************************/
	m_bMultimapsEnabled = (getDefineINT("ENABLE_MULTIMAPS") != 0);
	m_bViewportsEnabled = (getDefineINT("ENABLE_VIEWPORTS") != 0);
	m_iViewportFocusBorder = GC.getDefineINT("VIEWPORT_FOCUS_BORDER");
	m_iViewportSizeX = GC.getDefineINT("VIEWPORT_SIZE_X");
	m_iViewportSizeY = GC.getDefineINT("VIEWPORT_SIZE_Y");

	m_szAlternateProfilSampleName = getDefineSTRING("PROFILER_ALTERNATE_SAMPLE_SET_SOURCE");
	if ( m_szAlternateProfilSampleName == NULL )
	{
		m_szAlternateProfilSampleName = "";
	}

	m_iLAND_UNITS_CAN_ATTACK_WATER_CITIES = getDefineINT("LAND_UNITS_CAN_ATTACK_WATER_CITIES");
	m_iBASE_UNIT_UPGRADE_COST = getDefineINT("BASE_UNIT_UPGRADE_COST");
	m_iUPGRADE_ROUND_LIMIT = getDefineINT("UPGRADE_ROUND_LIMIT");
	m_iCITY_BARBARIAN_DEFENSE_MODIFIER = getDefineINT("CITY_BARBARIAN_DEFENSE_MODIFIER");
	m_iUNIT_VISIBILITY_RANGE = getDefineINT("UNIT_VISIBILITY_RANGE");
	m_iMAX_UNIT_VISIBILITY_RANGE = getDefineINT("MAX_UNIT_VISIBILITY_RANGE");
	m_iGREATER_COMMERCE_SWITCH_POINT = getDefineINT("GREATER_COMMERCE_SWITCH_POINT");
	m_iWORKER_TRADE_VALUE_PERCENT_ADJUSTMENT = getDefineINT("WORKER_TRADE_VALUE_PERCENT_ADJUSTMENT");
	m_iTRADE_MISSION_END_TOTAL_PERCENT_ADJUSTMENT = getDefineINT("TRADE_MISSION_END_TOTAL_PERCENT_ADJUSTMENT");
	m_iINFILTRATE_MISSION_END_TOTAL_PERCENT_ADJUSTMENT = getDefineINT("INFILTRATE_MISSION_END_TOTAL_PERCENT_ADJUSTMENT");
	m_iESPIONAGE_MISSION_COST_END_TOTAL_PERCENT_ADJUSTMENT = getDefineINT("ESPIONAGE_MISSION_COST_END_TOTAL_PERCENT_ADJUSTMENT");
	m_iWATER_POTENTIAL_CITY_WORK_FOR_AREA = getDefineINT("WATER_POTENTIAL_CITY_WORK_FOR_AREA");
	m_iSAD_MAX_MODIFIER = getDefineINT("SAD_MAX_MODIFIER");
	m_iUPSCALED_RESEARCH_COST_MODIFIER = getDefineINT("UPSCALED_RESEARCH_COST_MODIFIER");
	m_iENABLE_DYNAMIC_UNIT_ENTITIES = getDefineINT("ENABLE_DYNAMIC_UNIT_ENTITIES");

	OutputDebugString("Caching Globals: End");
}

/************************************************************************************************/
/* MOD COMPONENT CONTROL                   08/02/07                            MRGENIE          */
/*                                                                                              */
/* Return true/false from                                                                       */
/************************************************************************************************/
bool cvInternalGlobals::getDefineBOOL( const char * szName ) const
{
	bool bReturn = false;
	bool success = GC.getDefinesVarSystem()->GetValue( szName, bReturn );
	//FAssertMsg( success, szName );
	return bReturn;
}
/************************************************************************************************/
/* MOD COMPONENT CONTROL                   END                                                  */
/************************************************************************************************/

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      02/21/10                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
int cvInternalGlobals::getDefineINT( const char * szName, const int iDefault ) const
{
	int iReturn = 0;

	if( GC.getDefinesVarSystem()->GetValue( szName, iReturn ) )
	{
		return iReturn;
	}

	return iDefault;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/


int cvInternalGlobals::getDefineINT( const char * szName ) const
{
	int iReturn = 0;
	bool success = GC.getDefinesVarSystem()->GetValue( szName, iReturn );
	//FAssertMsg( success, szName );
	return iReturn;
}

float cvInternalGlobals::getDefineFLOAT( const char * szName ) const
{
	float fReturn = 0;
	bool success = GC.getDefinesVarSystem()->GetValue( szName, fReturn );
	//FAssertMsg( success, szName );
	return fReturn;
}

const char * cvInternalGlobals::getDefineSTRING( const char * szName ) const
{
	const char * szReturn = NULL;
	bool success = GC.getDefinesVarSystem()->GetValue( szName, szReturn );
	//FAssertMsg( success, szName );
	return szReturn;
}
/************************************************************************************************/
/* Afforess	                  Start		 08/18/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
void cvInternalGlobals::setDefineINT( const char * szName, int iValue, bool bUpdate )
{
	if (getDefineINT(szName) != iValue)
	{
		if (bUpdate)
			CvMessageControl::getInstance().sendGlobalDefineUpdate(szName, iValue, -1.0f, "");
		else
			GC.getDefinesVarSystem()->SetValue( szName, iValue );
		cacheGlobals();
	}
}

void cvInternalGlobals::setDefineFLOAT( const char * szName, float fValue, bool bUpdate )
{
	if (getDefineFLOAT(szName) != fValue)
	{
		if (bUpdate)
			CvMessageControl::getInstance().sendGlobalDefineUpdate(szName, -1, fValue, "");
		else
			GC.getDefinesVarSystem()->SetValue( szName, fValue );
		cacheGlobals();
	}
}

void cvInternalGlobals::setDefineSTRING( const char * szName, const char * szValue, bool bUpdate )
{
	if (getDefineSTRING(szName) != szValue)
	{
		if (bUpdate)
			CvMessageControl::getInstance().sendGlobalDefineUpdate(szName, -1, -1.0f, szValue);
		else
			GC.getDefinesVarSystem()->SetValue( szName, szValue );
		cacheGlobals(); // TO DO : we should not cache all globals at each single set
	}
}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

int cvInternalGlobals::getMOVE_DENOMINATOR() const
{
	return m_iMOVE_DENOMINATOR;
}

int cvInternalGlobals::getNUM_UNIT_PREREQ_OR_BONUSES() const
{
	return m_iNUM_UNIT_PREREQ_OR_BONUSES;
}

int cvInternalGlobals::getNUM_BUILDING_PREREQ_OR_BONUSES() const
{
	return m_iNUM_BUILDING_PREREQ_OR_BONUSES;
}

int cvInternalGlobals::getFOOD_CONSUMPTION_PER_POPULATION() const
{
	return m_iFOOD_CONSUMPTION_PER_POPULATION;
}

int cvInternalGlobals::getMAX_HIT_POINTS() const
{
	return m_iMAX_HIT_POINTS;
}

int cvInternalGlobals::getPATH_DAMAGE_WEIGHT() const
{
	return m_iPATH_DAMAGE_WEIGHT;
}

int cvInternalGlobals::getHILLS_EXTRA_DEFENSE() const
{
	return m_iHILLS_EXTRA_DEFENSE;
}

int cvInternalGlobals::getRIVER_ATTACK_MODIFIER() const
{
	return m_iRIVER_ATTACK_MODIFIER;
}

int cvInternalGlobals::getAMPHIB_ATTACK_MODIFIER() const
{
	return m_iAMPHIB_ATTACK_MODIFIER;
}

int cvInternalGlobals::getHILLS_EXTRA_MOVEMENT() const
{
	return m_iHILLS_EXTRA_MOVEMENT;
}

int cvInternalGlobals::getRIVER_EXTRA_MOVEMENT() const
{
	return m_iRIVER_EXTRA_MOVEMENT;
}

int cvInternalGlobals::getMAX_PLOT_LIST_ROWS() const
{
	return m_iMAX_PLOT_LIST_ROWS;
}

int cvInternalGlobals::getUNIT_MULTISELECT_MAX() const
{
	return m_iUNIT_MULTISELECT_MAX;
}

int cvInternalGlobals::getPERCENT_ANGER_DIVISOR() const
{
	return m_iPERCENT_ANGER_DIVISOR;
}

int cvInternalGlobals::getEVENT_MESSAGE_TIME() const
{
	return m_iEVENT_MESSAGE_TIME;
}

int cvInternalGlobals::getROUTE_FEATURE_GROWTH_MODIFIER() const
{
	return m_iROUTE_FEATURE_GROWTH_MODIFIER;
}

int cvInternalGlobals::getFEATURE_GROWTH_MODIFIER() const
{
	return m_iFEATURE_GROWTH_MODIFIER;
}

int cvInternalGlobals::getMIN_CITY_RANGE() const
{
	return m_iMIN_CITY_RANGE;
}

int cvInternalGlobals::getCITY_MAX_NUM_BUILDINGS() const
{
	return m_iCITY_MAX_NUM_BUILDINGS;
}

int cvInternalGlobals::getNUM_UNIT_AND_TECH_PREREQS() const
{
	return m_iNUM_UNIT_AND_TECH_PREREQS;
}

int cvInternalGlobals::getNUM_AND_TECH_PREREQS() const
{
	return m_iNUM_AND_TECH_PREREQS;
}

int cvInternalGlobals::getNUM_OR_TECH_PREREQS() const
{
	return m_iNUM_OR_TECH_PREREQS;
}

int cvInternalGlobals::getLAKE_MAX_AREA_SIZE() const
{
	return m_iLAKE_MAX_AREA_SIZE;
}

int cvInternalGlobals::getNUM_ROUTE_PREREQ_OR_BONUSES() const
{
	return m_iNUM_ROUTE_PREREQ_OR_BONUSES;
}

int cvInternalGlobals::getNUM_BUILDING_AND_TECH_PREREQS() const
{
	return m_iNUM_BUILDING_AND_TECH_PREREQS;
}

int cvInternalGlobals::getMIN_WATER_SIZE_FOR_OCEAN() const
{
	return m_iMIN_WATER_SIZE_FOR_OCEAN;
}

int cvInternalGlobals::getFORTIFY_MODIFIER_PER_TURN() const
{
	return m_iFORTIFY_MODIFIER_PER_TURN;
}

int cvInternalGlobals::getESTABLISH_MODIFIER_PER_TURN() const
{
	return m_iESTABLISH_MODIFIER_PER_TURN;
}

int cvInternalGlobals::getESCAPE_MODIFIER_PER_TURN() const
{
	return m_iESCAPE_MODIFIER_PER_TURN;
}

int cvInternalGlobals::getMAX_CITY_DEFENSE_DAMAGE() const
{
	return m_iMAX_CITY_DEFENSE_DAMAGE;
}

int cvInternalGlobals::getPEAK_SEE_THROUGH_CHANGE() const
{
	return m_iPEAK_SEE_THROUGH_CHANGE;
}

int cvInternalGlobals::getHILLS_SEE_THROUGH_CHANGE() const
{
	return m_iHILLS_SEE_THROUGH_CHANGE;
}

int cvInternalGlobals::getSEAWATER_SEE_FROM_CHANGE() const
{
	return m_iSEAWATER_SEE_FROM_CHANGE;
}

int cvInternalGlobals::getPEAK_SEE_FROM_CHANGE() const
{
	return m_iPEAK_SEE_FROM_CHANGE;
}

int cvInternalGlobals::getHILLS_SEE_FROM_CHANGE() const
{
	return m_iHILLS_SEE_FROM_CHANGE;
}

int cvInternalGlobals::getUSE_SPIES_NO_ENTER_BORDERS() const
{
	return m_iUSE_SPIES_NO_ENTER_BORDERS;
}

int cvInternalGlobals::getNUM_CORPORATION_PREREQ_BONUSES() const
{
	return m_iNUM_CORPORATION_PREREQ_BONUSES;
}

float cvInternalGlobals::getCAMERA_MIN_YAW() const
{
	return m_fCAMERA_MIN_YAW;
}

float cvInternalGlobals::getCAMERA_MAX_YAW() const
{
	return m_fCAMERA_MAX_YAW;
}

float cvInternalGlobals::getCAMERA_FAR_CLIP_Z_HEIGHT() const
{
	return m_fCAMERA_FAR_CLIP_Z_HEIGHT;
}

float cvInternalGlobals::getCAMERA_MAX_TRAVEL_DISTANCE() const
{
	return m_fCAMERA_MAX_TRAVEL_DISTANCE;
}

float cvInternalGlobals::getCAMERA_START_DISTANCE() const
{
	return m_fCAMERA_START_DISTANCE;
}

float cvInternalGlobals::getAIR_BOMB_HEIGHT() const
{
	return m_fAIR_BOMB_HEIGHT;
}

float cvInternalGlobals::getPLOT_SIZE() const
{
	CvMapExternal& kMap = GC.getMapExternal();
	kMap.mapCoordinates(true);

	return m_fPLOT_SIZE;
}

float cvInternalGlobals::getCAMERA_SPECIAL_PITCH() const
{
	return m_fCAMERA_SPECIAL_PITCH;
}

float cvInternalGlobals::getCAMERA_MAX_TURN_OFFSET() const
{
	return m_fCAMERA_MAX_TURN_OFFSET;
}

float cvInternalGlobals::getCAMERA_MIN_DISTANCE() const
{
	return m_fCAMERA_MIN_DISTANCE;
}

float cvInternalGlobals::getCAMERA_UPPER_PITCH() const
{
	return m_fCAMERA_UPPER_PITCH;
}

float cvInternalGlobals::getCAMERA_LOWER_PITCH() const
{
	return m_fCAMERA_LOWER_PITCH;
}

float cvInternalGlobals::getFIELD_OF_VIEW() const
{
	return m_fFIELD_OF_VIEW;
}

float cvInternalGlobals::getSHADOW_SCALE() const
{
	return m_fSHADOW_SCALE;
}

float cvInternalGlobals::getUNIT_MULTISELECT_DISTANCE() const
{
	return m_fUNIT_MULTISELECT_DISTANCE;
}

int cvInternalGlobals::getUSE_CANNOT_FOUND_CITY_CALLBACK() const
{
	return m_iUSE_CANNOT_FOUND_CITY_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_FOUND_CITIES_ON_WATER_CALLBACK() const
{
	return m_iUSE_CAN_FOUND_CITIES_ON_WATER_CALLBACK;
}

int cvInternalGlobals::getUSE_IS_PLAYER_RESEARCH_CALLBACK() const
{
	return m_iUSE_IS_PLAYER_RESEARCH_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_RESEARCH_CALLBACK() const
{
	return m_iUSE_CAN_RESEARCH_CALLBACK;
}

int cvInternalGlobals::getUSE_CANNOT_DO_CIVIC_CALLBACK() const
{
	return m_iUSE_CANNOT_DO_CIVIC_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_DO_CIVIC_CALLBACK() const
{
	return m_iUSE_CAN_DO_CIVIC_CALLBACK;
}

int cvInternalGlobals::getUSE_CANNOT_CONSTRUCT_CALLBACK() const
{
	return m_iUSE_CANNOT_CONSTRUCT_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_CONSTRUCT_CALLBACK() const
{
	return m_iUSE_CAN_CONSTRUCT_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_DECLARE_WAR_CALLBACK() const
{
	return m_iUSE_CAN_DECLARE_WAR_CALLBACK;
}

int cvInternalGlobals::getUSE_CANNOT_RESEARCH_CALLBACK() const
{
	return m_iUSE_CANNOT_RESEARCH_CALLBACK;
}

int cvInternalGlobals::getUSE_GET_UNIT_COST_MOD_CALLBACK() const
{
	return m_iUSE_GET_UNIT_COST_MOD_CALLBACK;
}

int cvInternalGlobals::getUSE_GET_BUILDING_COST_MOD_CALLBACK() const
{
	return m_iUSE_GET_BUILDING_COST_MOD_CALLBACK;
}

int cvInternalGlobals::getUSE_GET_CITY_FOUND_VALUE_CALLBACK() const
{
	return m_iUSE_GET_CITY_FOUND_VALUE_CALLBACK;
}

int cvInternalGlobals::getUSE_CANNOT_HANDLE_ACTION_CALLBACK() const
{
	return m_iUSE_CANNOT_HANDLE_ACTION_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_BUILD_CALLBACK() const
{
	return m_iUSE_CAN_BUILD_CALLBACK;
}

int cvInternalGlobals::getUSE_CANNOT_TRAIN_CALLBACK() const
{
	return m_iUSE_CANNOT_TRAIN_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_TRAIN_CALLBACK() const
{
	return m_iUSE_CAN_TRAIN_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_BUILD_CALLBACK(BuildTypes eBuild) const
{
	return m_iUSE_CAN_BUILD_CALLBACK;
}

int cvInternalGlobals::getUSE_CANNOT_TRAIN_CALLBACK(UnitTypes eUnit) const
{
	return m_iUSE_CANNOT_TRAIN_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_TRAIN_CALLBACK(UnitTypes eUnit) const
{
	return m_iUSE_CAN_TRAIN_CALLBACK;
}

int cvInternalGlobals::getUSE_UNIT_CANNOT_MOVE_INTO_CALLBACK() const
{
	return m_iUSE_UNIT_CANNOT_MOVE_INTO_CALLBACK;
}

int cvInternalGlobals::getUSE_USE_CANNOT_SPREAD_RELIGION_CALLBACK() const
{
	return m_iUSE_USE_CANNOT_SPREAD_RELIGION_CALLBACK;
}

int cvInternalGlobals::getUSE_FINISH_TEXT_CALLBACK() const
{
	return m_iUSE_FINISH_TEXT_CALLBACK;
}

int cvInternalGlobals::getUSE_ON_UNIT_SET_XY_CALLBACK() const
{
	return m_iUSE_ON_UNIT_SET_XY_CALLBACK;
}

int cvInternalGlobals::getUSE_ON_UNIT_SELECTED_CALLBACK() const
{
	return m_iUSE_ON_UNIT_SELECTED_CALLBACK;
}

int cvInternalGlobals::getUSE_ON_UPDATE_CALLBACK() const
{
	return m_iUSE_ON_UPDATE_CALLBACK;
}

int cvInternalGlobals::getUSE_ON_UNIT_CREATED_CALLBACK() const
{
	return m_iUSE_ON_UNIT_CREATED_CALLBACK;
}

int cvInternalGlobals::getUSE_ON_UNIT_LOST_CALLBACK() const
{
	return m_iUSE_ON_UNIT_LOST_CALLBACK;
}

int cvInternalGlobals::getLAND_UNITS_CAN_ATTACK_WATER_CITIES() const
{
	return m_iLAND_UNITS_CAN_ATTACK_WATER_CITIES;
}

int cvInternalGlobals::getBASE_UNIT_UPGRADE_COST() const
{
	return m_iBASE_UNIT_UPGRADE_COST;
}

int cvInternalGlobals::getUPGRADE_ROUND_LIMIT() const
{
	return m_iUPGRADE_ROUND_LIMIT;
}

int cvInternalGlobals::getCITY_BARBARIAN_DEFENSE_MODIFIER() const
{
	return m_iCITY_BARBARIAN_DEFENSE_MODIFIER;
}

int cvInternalGlobals::getUNIT_VISIBILITY_RANGE() const
{
	return m_iUNIT_VISIBILITY_RANGE;
}

int cvInternalGlobals::getMAX_UNIT_VISIBILITY_RANGE() const
{
	return m_iMAX_UNIT_VISIBILITY_RANGE;
}

int cvInternalGlobals::getGREATER_COMMERCE_SWITCH_POINT() const
{
	return m_iGREATER_COMMERCE_SWITCH_POINT;
}

int cvInternalGlobals::getWORKER_TRADE_VALUE_PERCENT_ADJUSTMENT() const
{
	return m_iWORKER_TRADE_VALUE_PERCENT_ADJUSTMENT;
}

int cvInternalGlobals::getTRADE_MISSION_END_TOTAL_PERCENT_ADJUSTMENT() const
{
	return m_iTRADE_MISSION_END_TOTAL_PERCENT_ADJUSTMENT;
}

int cvInternalGlobals::getINFILTRATE_MISSION_END_TOTAL_PERCENT_ADJUSTMENT() const
{
	return m_iINFILTRATE_MISSION_END_TOTAL_PERCENT_ADJUSTMENT;
}

int cvInternalGlobals::getESPIONAGE_MISSION_COST_END_TOTAL_PERCENT_ADJUSTMENT() const
{
	return m_iESPIONAGE_MISSION_COST_END_TOTAL_PERCENT_ADJUSTMENT;
}

int cvInternalGlobals::getWATER_POTENTIAL_CITY_WORK_FOR_AREA() const
{
	return m_iWATER_POTENTIAL_CITY_WORK_FOR_AREA;
}

int cvInternalGlobals::getSAD_MAX_MODIFIER() const
{
	return m_iSAD_MAX_MODIFIER;
}

int cvInternalGlobals::getUPSCALED_RESEARCH_COST_MODIFIER() const
{
	return m_iUPSCALED_RESEARCH_COST_MODIFIER;
}

int cvInternalGlobals::getENABLE_DYNAMIC_UNIT_ENTITIES() const
{
	return m_iENABLE_DYNAMIC_UNIT_ENTITIES;
}

float cvInternalGlobals::getSAD_FACTOR_1() const
{
	return m_fSAD_FACTOR_1;
}

float cvInternalGlobals::getSAD_FACTOR_2() const
{
	return m_fSAD_FACTOR_2;
}

float cvInternalGlobals::getSAD_FACTOR_3() const
{
	return m_fSAD_FACTOR_3;
}

float cvInternalGlobals::getSAD_FACTOR_4() const
{
	return m_fSAD_FACTOR_4;
}

/************************************************************************************************/
/* MODULES                                 11/13/07                            MRGENIE          */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
int cvInternalGlobals::getTGA_RELIGIONS() const								// GAMEFONT_TGA_RELIGIONS
{
	return m_iTGA_RELIGIONS;
}

int cvInternalGlobals::getTGA_CORPORATIONS() const							// GAMEFONT_TGA_CORPORATIONS
{
	return m_iTGA_CORPORATIONS;
}

/************************************************************************************************/
/* MODULES                                 END                                                  */
/************************************************************************************************/
int cvInternalGlobals::getMAX_CIV_PLAYERS() const
{
	return MAX_PC_PLAYERS;
}

int cvInternalGlobals::getMAX_PC_PLAYERS() const
{
	return MAX_PC_PLAYERS;
}

int cvInternalGlobals::getMAX_PLAYERS() const
{
	return MAX_PLAYERS;
}

int cvInternalGlobals::getMAX_PC_TEAMS() const
{
	return MAX_PC_TEAMS;
}

int cvInternalGlobals::getMAX_TEAMS() const
{
	return MAX_TEAMS;
}

int cvInternalGlobals::getBARBARIAN_PLAYER() const
{
	return BARBARIAN_PLAYER;
}
int cvInternalGlobals::getBARBARIAN_TEAM() const
{
	return BARBARIAN_TEAM;
}

int cvInternalGlobals::getNEANDERTHAL_PLAYER() const
{
	return NEANDERTHAL_PLAYER;
}
int cvInternalGlobals::getNEANDERTHAL_TEAM() const
{
	return NEANDERTHAL_TEAM;
}

int cvInternalGlobals::getBEAST_PLAYER() const
{
	return BEAST_PLAYER;
}
int cvInternalGlobals::getBEAST_TEAM() const
{
	return BEAST_TEAM;
}

int cvInternalGlobals::getPREDATOR_PLAYER() const
{
	return PREDATOR_PLAYER;
}
int cvInternalGlobals::getPREDATOR_TEAM() const
{
	return PREDATOR_TEAM;
}

int cvInternalGlobals::getPREY_PLAYER() const
{
	return PREY_PLAYER;
}
int cvInternalGlobals::getPREY_TEAM() const
{
	return PREY_TEAM;
}

int cvInternalGlobals::getINSECT_PLAYER() const
{
	return INSECT_PLAYER;
}
int cvInternalGlobals::getINSECT_TEAM() const
{
	return INSECT_TEAM;
}

int cvInternalGlobals::getNPC4_PLAYER() const
{
	return NPC4_PLAYER;
}

int cvInternalGlobals::getNPC4_TEAM() const
{
	return NPC4_TEAM;
}

int cvInternalGlobals::getNPC3_PLAYER() const
{
	return NPC3_PLAYER;
}

int cvInternalGlobals::getNPC3_TEAM() const
{
	return NPC3_TEAM;
}

int cvInternalGlobals::getNPC2_PLAYER() const
{
	return NPC2_PLAYER;
}

int cvInternalGlobals::getNPC2_TEAM() const
{
	return NPC2_TEAM;
}

int cvInternalGlobals::getNPC1_PLAYER() const
{
	return NPC1_PLAYER;
}

int cvInternalGlobals::getNPC1_TEAM() const
{
	return NPC1_TEAM;
}

int cvInternalGlobals::getNPC0_PLAYER() const
{
	return NPC0_PLAYER;
}

int cvInternalGlobals::getNPC0_TEAM() const
{
	return NPC0_TEAM;
}

void cvInternalGlobals::setDLLProfiler(FProfiler* prof)
{
	m_Profiler = prof;
}

FProfiler* cvInternalGlobals::getDLLProfiler() const
{
	return m_Profiler;
}

void cvInternalGlobals::enableDLLProfiler(bool bEnable)
{
	m_bDLLProfiler = bEnable;

#ifdef USE_INTERNAL_PROFILER
	if ( bEnable )
	{
		g_bTraceBackgroundThreads = getDefineBOOL("ENABLE_BACKGROUND_PROFILING");
	}
#endif
}

bool cvInternalGlobals::isDLLProfilerEnabled() const
{
	return m_bDLLProfiler;
}

const char* cvInternalGlobals::alternateProfileSampleName() const
{
	return m_szAlternateProfilSampleName;
}

bool cvInternalGlobals::readBuildingInfoArray(FDataStreamBase* pStream)
{
	return readInfoArray(pStream, m_paBuildingInfo, "CvBuildingInfo");
}

void cvInternalGlobals::writeBuildingInfoArray(FDataStreamBase* pStream)
{
	writeInfoArray(pStream, m_paBuildingInfo);
}

bool cvInternalGlobals::readTechInfoArray(FDataStreamBase* pStream)
{
	return readInfoArray(pStream, m_paTechInfo, "CvTechInfo");
}

void cvInternalGlobals::writeTechInfoArray(FDataStreamBase* pStream)
{
	writeInfoArray(pStream, m_paTechInfo);
}

bool cvInternalGlobals::readUnitInfoArray(FDataStreamBase* pStream)
{
	return readInfoArray(pStream, m_paUnitInfo, "CvUnitInfo");
}

void cvInternalGlobals::writeUnitInfoArray(FDataStreamBase* pStream)
{
	writeInfoArray(pStream, m_paUnitInfo);
}

bool cvInternalGlobals::readLeaderHeadInfoArray(FDataStreamBase* pStream)
{
	return readInfoArray(pStream, m_paLeaderHeadInfo, "CvLeaderHeadInfo");
}

void cvInternalGlobals::writeLeaderHeadInfoArray(FDataStreamBase* pStream)
{
	writeInfoArray(pStream, m_paLeaderHeadInfo);
}

bool cvInternalGlobals::readCivilizationInfoArray(FDataStreamBase* pStream)
{
	return readInfoArray(pStream, m_paCivilizationInfo, "CvCivilizationInfo");
}

void cvInternalGlobals::writeCivilizationInfoArray(FDataStreamBase* pStream)
{
	writeInfoArray(pStream, m_paCivilizationInfo);
}

bool cvInternalGlobals::readPromotionInfoArray(FDataStreamBase* pStream)
{
	return readInfoArray(pStream, m_paPromotionInfo, "CvPromotionInfo");
}

void cvInternalGlobals::writePromotionInfoArray(FDataStreamBase* pStream)
{
	writeInfoArray(pStream, m_paPromotionInfo);
}

bool cvInternalGlobals::readDiplomacyInfoArray(FDataStreamBase* pStream)
{
	return readInfoArray(pStream, m_paDiplomacyInfo, "CvDiplomacyInfo");
}

void cvInternalGlobals::writeDiplomacyInfoArray(FDataStreamBase* pStream)
{
	writeInfoArray(pStream, m_paDiplomacyInfo);
}

bool cvInternalGlobals::readCivicInfoArray(FDataStreamBase* pStream)
{
	return readInfoArray(pStream, m_paCivicInfo, "CvCivicInfo");
}

void cvInternalGlobals::writeCivicInfoArray(FDataStreamBase* pStream)
{
	writeInfoArray(pStream, m_paCivicInfo);
}

bool cvInternalGlobals::readHandicapInfoArray(FDataStreamBase* pStream)
{
	return readInfoArray(pStream, m_paHandicapInfo, "CvHandicapInfo");
}

void cvInternalGlobals::writeHandicapInfoArray(FDataStreamBase* pStream)
{
	writeInfoArray(pStream, m_paHandicapInfo);
}

bool cvInternalGlobals::readBonusInfoArray(FDataStreamBase* pStream)
{
	return readInfoArray(pStream, m_paBonusInfo, "CvBonusInfo");
}

void cvInternalGlobals::writeBonusInfoArray(FDataStreamBase* pStream)
{
	writeInfoArray(pStream, m_paBonusInfo);
}

bool cvInternalGlobals::readImprovementInfoArray(FDataStreamBase* pStream)
{
	return readInfoArray(pStream, m_paImprovementInfo, "CvImprovementInfo");
}

void cvInternalGlobals::writeImprovementInfoArray(FDataStreamBase* pStream)
{
	writeInfoArray(pStream, m_paImprovementInfo);
}

bool cvInternalGlobals::readEventInfoArray(FDataStreamBase* pStream)
{
	return readInfoArray(pStream, m_paEventInfo, "CvEventInfo");
}

void cvInternalGlobals::writeEventInfoArray(FDataStreamBase* pStream)
{
	writeInfoArray(pStream, m_paEventInfo);
}

bool cvInternalGlobals::readEventTriggerInfoArray(FDataStreamBase* pStream)
{
	return readInfoArray(pStream, m_paEventTriggerInfo, "CvEventTriggerInfo");
}

void cvInternalGlobals::writeEventTriggerInfoArray(FDataStreamBase* pStream)
{
	writeInfoArray(pStream, m_paEventTriggerInfo);
}


//
// Global Types Hash Map
//

int cvInternalGlobals::getTypesEnum(const char* szType) const
{
	FAssertMsg(szType, "null type string");
	TypesMap::const_iterator it = m_typesMap.find(szType);
	if (it!=m_typesMap.end())
	{
		return it->second;
	}

	FAssertMsg(strcmp(szType, "NONE")==0 || strcmp(szType, "")==0, CvString::format("type %s not found", szType).c_str());
	return -1;
}

void cvInternalGlobals::setTypesEnum(const char* szType, int iEnum)
{
	FAssertMsg(szType, "null type string");
	FAssertMsg(m_typesMap.find(szType)==m_typesMap.end(), "types entry already exists");
	m_typesMap[szType] = iEnum;
}


void cvInternalGlobals::deleteInfoArrays()
{
	deleteInfoArray(m_paBuildingInfo);
	deleteInfoArray(m_paSpecialBuildingInfo);

	deleteInfoArray(m_paLeaderHeadInfo);
	deleteInfoArray(m_paTraitInfo);
	deleteInfoArray(m_paCivilizationInfo);
	deleteInfoArray(m_paUnitArtStyleTypeInfo);

	deleteInfoArray(m_paVoteSourceInfo);
	deleteInfoArray(m_paHints);
	deleteInfoArray(m_paMainMenus);
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 11/01/07                            MRGENIE          */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	// MLF loading
	m_paModLoadControlVector.clear();
	deleteInfoArray(m_paModLoadControls);
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
	deleteInfoArray(m_paGoodyInfo);
	deleteInfoArray(m_paHandicapInfo);
	deleteInfoArray(m_paGameSpeedInfo);
	deleteInfoArray(m_paTurnTimerInfo);
	deleteInfoArray(m_paVictoryInfo);
	deleteInfoArray(m_paHurryInfo);
	deleteInfoArray(m_paWorldInfo);
	deleteInfoArray(m_paSeaLevelInfo);
	deleteInfoArray(m_paClimateInfo);
	deleteInfoArray(m_paProcessInfo);
	deleteInfoArray(m_paVoteInfo);
	deleteInfoArray(m_paProjectInfo);
	deleteInfoArray(m_paReligionInfo);
	deleteInfoArray(m_paCorporationInfo);
	deleteInfoArray(m_paCommerceInfo);
	deleteInfoArray(m_paEmphasizeInfo);
	deleteInfoArray(m_paUpkeepInfo);
	deleteInfoArray(m_paCultureLevelInfo);

	deleteInfoArray(m_paColorInfo);
	deleteInfoArray(m_paPlayerColorInfo);
	deleteInfoArray(m_paInterfaceModeInfo);
	deleteInfoArray(m_paCameraInfo);
	deleteInfoArray(m_paAdvisorInfo);
	deleteInfoArray(m_paThroneRoomCamera);
	deleteInfoArray(m_paThroneRoomInfo);
	deleteInfoArray(m_paThroneRoomStyleInfo);
	deleteInfoArray(m_paSlideShowInfo);
	deleteInfoArray(m_paSlideShowRandomInfo);
	deleteInfoArray(m_paWorldPickerInfo);
	deleteInfoArray(m_paSpaceShipInfo);

	deleteInfoArray(m_paCivicInfo);
	deleteInfoArray(m_paImprovementInfo);

	deleteInfoArray(m_paRouteInfo);
	deleteInfoArray(m_paRouteModelInfo);
	deleteInfoArray(m_paRiverInfo);
	deleteInfoArray(m_paRiverModelInfo);

	deleteInfoArray(m_paWaterPlaneInfo);
	deleteInfoArray(m_paTerrainPlaneInfo);
	deleteInfoArray(m_paCameraOverlayInfo);

	deleteInfoArray(m_aEraInfo);
	deleteInfoArray(m_paEffectInfo);
	deleteInfoArray(m_paAttachableInfo);

	deleteInfoArray(m_paTechInfo);
	deleteInfoArray(m_paDiplomacyInfo);

	deleteInfoArray(m_paBuildInfo);
	deleteInfoArray(m_paUnitInfo);
	deleteInfoArray(m_paSpawnInfo);
	deleteInfoArray(m_paSpecialUnitInfo);
	deleteInfoArray(m_paSpecialistInfo);
	deleteInfoArray(m_paActionInfo);
	deleteInfoArray(m_paMissionInfo);
	deleteInfoArray(m_paControlInfo);
	deleteInfoArray(m_paCommandInfo);
	deleteInfoArray(m_paAutomateInfo);
	deleteInfoArray(m_paPromotionInfo);

	deleteInfoArray(m_paConceptInfo);
	deleteInfoArray(m_paNewConceptInfo);
	deleteInfoArray(m_paCityTabInfo);
	deleteInfoArray(m_paCalendarInfo);
	deleteInfoArray(m_paSeasonInfo);
	deleteInfoArray(m_paMonthInfo);
	deleteInfoArray(m_paDenialInfo);
	deleteInfoArray(m_paInvisibleInfo);
	deleteInfoArray(m_paUnitCombatInfo);
	//TB Promotion Line mod begin
	deleteInfoArray(m_paPromotionLineInfo);
	//TB Promotion Line mod end
	deleteInfoArray(m_paMapCategoryInfo);
	deleteInfoArray(m_paIdeaClassInfo);
	deleteInfoArray(m_paIdeaInfo);
	//deleteInfoArray(m_paTraitOptionEditsInfo);
	deleteInfoArray(m_paDomainInfo);
	deleteInfoArray(m_paUnitAIInfos);
	deleteInfoArray(m_paAttitudeInfos);
	deleteInfoArray(m_paMemoryInfos);
	deleteInfoArray(m_paGameOptionInfos);
	deleteInfoArray(m_paMPOptionInfos);
	deleteInfoArray(m_paForceControlInfos);
	deleteInfoArray(m_paPlayerOptionInfos);
	deleteInfoArray(m_paGraphicOptionInfos);

	deleteInfoArray(m_paYieldInfo);
	deleteInfoArray(m_paTerrainInfo);
	deleteInfoArray(m_paFeatureInfo);
	deleteInfoArray(m_paBonusClassInfo);
	deleteInfoArray(m_paBonusInfo);
	deleteInfoArray(m_paLandscapeInfo);

	deleteInfoArray(m_paUnitFormationInfo);
	deleteInfoArray(m_paCivicOptionInfo);
	deleteInfoArray(m_paCursorInfo);

	SAFE_DELETE_ARRAY(GC.getEntityEventTypes());
	SAFE_DELETE_ARRAY(GC.getAnimationOperatorTypes());
	SAFE_DELETE_ARRAY(GC.getFunctionTypes());
	SAFE_DELETE_ARRAY(GC.getFlavorTypes());
	SAFE_DELETE_ARRAY(GC.getArtStyleTypes());
	SAFE_DELETE_ARRAY(GC.getCitySizeTypes());
	SAFE_DELETE_ARRAY(GC.getContactTypes());
	SAFE_DELETE_ARRAY(GC.getDiplomacyPowerTypes());
	SAFE_DELETE_ARRAY(GC.getAutomateTypes());
	SAFE_DELETE_ARRAY(GC.getDirectionTypes());
	SAFE_DELETE_ARRAY(GC.getFootstepAudioTypes());
	SAFE_DELETE_ARRAY(GC.getFootstepAudioTags());
	deleteInfoArray(m_paQuestInfo);
	deleteInfoArray(m_paTutorialInfo);

	deleteInfoArray(m_paEventInfo);
	deleteInfoArray(m_paEventTriggerInfo);
	deleteInfoArray(m_paEspionageMissionInfo);

	deleteInfoArray(m_paEntityEventInfo);
	deleteInfoArray(m_paAnimationCategoryInfo);
	deleteInfoArray(m_paAnimationPathInfo);
	deleteInfoArray(m_paPropertyInfo);

	clearTypesMap();
	m_aInfoVectors.clear();
}
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 11/30/07                                MRGENIE      */
/*                                                                                              */
/* Savegame compatibility                                                                       */
/************************************************************************************************/
void cvInternalGlobals::doResetInfoClasses(int iNumSaveGameVector, std::vector<CvString> m_aszSaveGameVector)
{
	// Store stuff being set by the exe in temp arrays
	StoreExeSettings();

	//delete obsolete InfoClasses
	deleteInfoArrays();

	// reset the ModLoadControlVector
	m_paModLoadControlVector.erase(m_paModLoadControlVector.begin(), m_paModLoadControlVector.end());

	//Load the Savegame vector to the ModLoadControlVector(being used by the enum)
	for ( int i = 0; i < iNumSaveGameVector; i++ )
	{
		m_paModLoadControlVector.push_back(m_aszSaveGameVector[i]);
	}

	//Delete InfoMap Keys
	infoTypeFromStringReset();

	//reload the new infoclasses
	CvXMLLoadUtility XMLLoadUtility;
	XMLLoadUtility.doResetGlobalInfoClasses();

	//Reload Arts with the Current MLF
	CvArtFileMgr ArtFileMgr = ArtFileMgr.GetInstance();
	ArtFileMgr.Reset();

	XMLLoadUtility.doResetInfoClasses();		// Reloads/allocs Art Defines

	// Load stuff being set by the exe from temp arrays
	LoadExeSettings();
}
void cvInternalGlobals::StoreExeSettings()
{
	// Chars from TGA files, CommerceInfo
	m_iStoreExeSettingsCommerceInfo =  new int[NUM_COMMERCE_TYPES];
	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++ )
	{
		m_iStoreExeSettingsCommerceInfo[i] = getCommerceInfo((CommerceTypes)i).getChar();
	}
	// Chars from TGA files, YieldInfo
	m_iStoreExeSettingsYieldInfo =  new int[NUM_YIELD_TYPES];
	for ( int i = 0; i < NUM_YIELD_TYPES; i++ )
	{
		m_iStoreExeSettingsYieldInfo[i] = getYieldInfo((YieldTypes)i).getChar();
	}	
	// Chars from TGA files, ReligionInfo
	m_iStoreExeSettingsReligionInfo =  new int[getNumReligionInfos()];
	for ( int i = 0; i < getNumReligionInfos(); i++ )
	{
		m_iStoreExeSettingsReligionInfo[i] = getReligionInfo((ReligionTypes)i).getChar();
	}
	// Chars from TGA files, CorporationInfo
	m_iStoreExeSettingsCorporationInfo =  new int[getNumCorporationInfos()];
	for ( int i = 0; i < getNumCorporationInfos(); i++ )
	{
		m_iStoreExeSettingsCorporationInfo[i] = getCorporationInfo((CorporationTypes)i).getChar();
	}
	// Chars from TGA files, BonusInfo
	m_iStoreExeSettingsBonusInfo =  new int[getNumBonusInfos()];
	for ( int i = 0; i < getNumBonusInfos(); i++ )
	{
		m_iStoreExeSettingsBonusInfo[i] = getBonusInfo((BonusTypes)i).getChar();
	}
}
void cvInternalGlobals::LoadExeSettings()
{
	// Chars from TGA files, CommerceInfo
	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++ )
	{
		getCommerceInfo((CommerceTypes)i).setChar(m_iStoreExeSettingsCommerceInfo[i]);
	}
	SAFE_DELETE_ARRAY(m_iStoreExeSettingsCommerceInfo);
	// Chars from TGA files, YieldInfo
	for ( int i = 0; i < NUM_YIELD_TYPES; i++ )
	{
		getYieldInfo((YieldTypes)i).setChar(m_iStoreExeSettingsYieldInfo[i]);
	}
	SAFE_DELETE_ARRAY(m_iStoreExeSettingsYieldInfo);
	// Chars from TGA files, ReligionInfo
	for ( int i = 0; i < getNumReligionInfos(); i++ )
	{
		getReligionInfo((ReligionTypes)i).setChar(m_iStoreExeSettingsReligionInfo[i]);
	}
	SAFE_DELETE_ARRAY(m_iStoreExeSettingsReligionInfo);
	// Chars from TGA files, CorporationInfo
	for ( int i = 0; i < getNumCorporationInfos(); i++ )
	{
		getCorporationInfo((CorporationTypes)i).setChar(m_iStoreExeSettingsCorporationInfo[i]);
	}
	SAFE_DELETE_ARRAY(m_iStoreExeSettingsCorporationInfo);
	// Chars from TGA files, BonusInfo
	for ( int i = 0; i < getNumBonusInfos(); i++ )
	{
		getBonusInfo((BonusTypes)i).setChar(m_iStoreExeSettingsBonusInfo[i]);
	}
	SAFE_DELETE_ARRAY(m_iStoreExeSettingsBonusInfo);
}
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/


//
// Global Infos Hash Map
//

int cvInternalGlobals::getInfoTypeForString(const char* szType, bool hideAssert) const
{
	FAssertMsg(szType, "null info type string");
	InfosMap::const_iterator it = m_infosMap.find(szType);
	if (it!=m_infosMap.end())
	{
		return it->second;
	}

	if(!(stricmp(szType, "NONE")==0 || strcmp(szType, "")==0) && !hideAssert && !getDefineINT(szType))
	{
		CvString szError;
		szError.Format("info type '%s' not found, Current XML file is: %s", szType, GC.getCurrentXMLFile().GetCString());
		FAssertMsg(stricmp(szType, "NONE")==0 || strcmp(szType, "")==0, szError.c_str());

		LogMissingType(szType);
	}

	return -1;
}

bool cvInternalGlobals::hasInfoTypeForString(const char* szType, bool hideAssert) const
{
	FAssertMsg(szType, "null info type string");
	InfosMap::const_iterator it = m_infosMap.find(szType);
	if (it!=m_infosMap.end())
	{
		return true;
	}

	if(!(stricmp(szType, "NONE")==0 || strcmp(szType, "")==0) && !getDefineINT(szType))
	{
		if (!hideAssert)
		{
			CvString szError;
			szError.Format("info type '%s' not found, Current XML file is: %s", szType, GC.getCurrentXMLFile().GetCString());
			FAssertMsg2(stricmp(szType, "NONE")==0 || strcmp(szType, "")==0, szError.c_str());
		}
		LogMissingType(szType);
	}

	return false;
}

void cvInternalGlobals::LogMissingType( const char* szType ) const
{
	CvString szError;
	szError.Format("info type '%s' not found, Current XML file is: %s", szType, GC.getCurrentXMLFile().GetCString());
	gDLL->logMsg("Xml_MissingTypes.log", szError);
}

/************************************************************************************************/
/* SORT_ALPHABET                           11/19/07                                MRGENIE      */
/*                                                                                              */
/* Rearranging the infos map                                                                    */
/************************************************************************************************/
/*
void cvInternalGlobals::setInfoTypeFromString(const char* szType, int idx)
{
	FAssertMsg(szType, "null info type string");
#ifdef _DEBUG
	InfosMap::const_iterator it = m_infosMap.find(szType);
	int iExisting = (it!=m_infosMap.end()) ? it->second : -1;
	FAssertMsg(iExisting==-1 || iExisting==idx || strcmp(szType, "ERROR")==0, CvString::format("xml info type entry %s already exists", szType).c_str());
#endif
	m_infosMap[szType] = idx;
}
*/
void cvInternalGlobals::setInfoTypeFromString(const char* szType, int idx)
{
	FAssertMsg(szType, "null info type string");
#ifdef _DEBUG
	OutputDebugString(CvString::format("%s -> %d\n", szType, idx).c_str());
#endif
	char* strCpy = new char[strlen(szType)+1];

	m_infosMap[strcpy(strCpy, szType)] = idx;
}

// returns the ID if it exists, otherwise assigns a new ID
int cvInternalGlobals::getOrCreateInfoTypeForString(const char* szType)
{
	int iID = getInfoTypeForString(szType, true);
	if (iID < 0)
	{
		m_iLastTypeID++;
		iID = m_iLastTypeID;
		setInfoTypeFromString(szType, iID);
	}
	return iID;
}

void cvInternalGlobals::logInfoTypeMap(const char* tagMsg)
{
	if (GC.isXMLLogging())
	{
		CvString szDebugBuffer;
		szDebugBuffer.Format(" === Info Type Map Dump BEGIN: %s ===", tagMsg);
		gDLL->logMsg("cvInternalGlobals_logInfoTypeMap.log", szDebugBuffer.c_str());

		int iCnt = 0;
		std::vector<std::string> vInfoMapKeys;
		for (InfosMap::const_iterator it = m_infosMap.begin(); it != m_infosMap.end(); ++it)
		{
			std::string sKey = it->first;
			vInfoMapKeys.push_back(sKey);
		}

		std::sort(vInfoMapKeys.begin(), vInfoMapKeys.end());

		for (std::vector<std::string>::const_iterator it = vInfoMapKeys.begin(); it != vInfoMapKeys.end(); ++it)
		{
			std::string sKey = *it;
			int iVal = m_infosMap[sKey.c_str()];
			szDebugBuffer.Format(" * %i --  %s: %i", iCnt, sKey.c_str(), iVal);
			gDLL->logMsg("cvInternalGlobals_logInfoTypeMap.log", szDebugBuffer.c_str());
			iCnt++;
		}

		szDebugBuffer.Format("Entries in total: %i", iCnt);
		gDLL->logMsg("cvInternalGlobals_logInfoTypeMap.log", szDebugBuffer.c_str());
		szDebugBuffer.Format(" === Info Type Map Dump END: %s ===", tagMsg);
		gDLL->logMsg("cvInternalGlobals_logInfoTypeMap.log", szDebugBuffer.c_str());
	}
}
/************************************************************************************************/
/* SORT_ALPHABET                           END                                                  */
/************************************************************************************************/

void cvInternalGlobals::infoTypeFromStringReset()
{
	for (InfosMap::const_iterator it = m_infosMap.begin(); it != m_infosMap.end(); ++it)
	{
		delete[] it->first;
	}

	m_infosMap.clear();
}

void cvInternalGlobals::addToInfosVectors(void *infoVector)
{
	std::vector<CvInfoBase *> *infoBaseVector = (std::vector<CvInfoBase *> *) infoVector;
	m_aInfoVectors.push_back(infoBaseVector);
}

void cvInternalGlobals::infosReset()
{
	for(int i=0;i<(int)m_aInfoVectors.size();i++)
	{
		std::vector<CvInfoBase *> *infoBaseVector = m_aInfoVectors[i];
		for(int j=0;j<(int)infoBaseVector->size();j++)
			infoBaseVector->at(j)->reset();
	}
}
/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/

void cvInternalGlobals::switchMap(MapTypes eMap)
{	
	//bool bInitial = false;

	GC.getMap().beforeSwitch();

	if (eMap > NO_MAP && eMap < GC.getNumMapInfos())
		GC.getGame().setCurrentMap(eMap);

	GC.getMap().afterSwitch();
}

CvViewport* cvInternalGlobals::getCurrentViewport()
{
	return m_maps[GC.getGame().getCurrentMap()]->getCurrentViewport();
}

int	cvInternalGlobals::getViewportSizeX() const
{
	return GC.viewportsEnabled() ? std::min(m_iViewportSizeX, m_maps[GC.getGame().getCurrentMap()]->getGridWidth()) : m_maps[GC.getGame().getCurrentMap()]->getGridWidth();
}

int	cvInternalGlobals::getViewportSizeY() const
{
	return GC.viewportsEnabled() ? std::min(m_iViewportSizeY, m_maps[GC.getGame().getCurrentMap()]->getGridHeight()) : m_maps[GC.getGame().getCurrentMap()]->getGridHeight();
}

int	cvInternalGlobals::getViewportSelectionBorder() const
{
	return m_iViewportFocusBorder;
}

int	cvInternalGlobals::getViewportCenteringBorder() const
{
	return m_iViewportCenterOnSelectionCenterBorder;
}


CvMapExternal& cvInternalGlobals::getMapExternal()
{
	CvViewport*	currentViewport = getCurrentViewport();

	FAssert(currentViewport != NULL);

	return *(currentViewport->getProxy());
}

CvMap& cvInternalGlobals::getMapByIndex(MapTypes eIndex)
{
	FAssert(eIndex > NO_MAP);
	FAssert(eIndex < GC.getNumMapInfos());
	return *m_maps[eIndex];
}

void cvInternalGlobals::clearSigns()
{
	gDLL->getEngineIFace()->clearSigns();

	m_bSignsCleared = true;
}

void cvInternalGlobals::reprocessSigns()
{
	if ( m_bSignsCleared )
	{
		Cy::call(PYCivModule, "AddSign", Cy::Args() << (CvPlot*)NULL << NO_PLAYER << "");
		m_bSignsCleared = false;
	}
}

void cvInternalGlobals::initializeMap(MapTypes eMap)
{
	OutputDebugString("Initializing Map: Start");
	while ( m_maps.size() < (size_t)eMap )
	{
		//	Sparse or out of order initialization
		m_maps.push_back(NULL);
	}

	FAssertMsg(m_maps[eMap] == NULL, "Memory leak allocating a map that already exists");

	m_maps[eMap] = new CvMap(eMap);
	OutputDebugString("Initializing Map: End");
}

bool cvInternalGlobals::mapInitialized(MapTypes eMap) const
{
	return (m_maps.size() > (size_t)eMap && m_maps[eMap] != NULL);
}


/*******************************/
/***** Parallel Maps - End *****/
/*******************************/


void cvInternalGlobals::addDelayedResolution(int *pType, CvString szString)
{
	m_delayedResolutionMap[pType] = std::make_pair(szString,  GC.getCurrentXMLFile());
	//m_delayedResolutionMap.insert(DelayedResolutionMap::value_type(pType, szString));
}

CvString* cvInternalGlobals::getDelayedResolution(int *pType)
{
	DelayedResolutionMap::iterator it = m_delayedResolutionMap.find(pType);
	if (it == m_delayedResolutionMap.end())
	{
		return NULL;
	}
	return &(it->second.first);
}

void cvInternalGlobals::removeDelayedResolution(int *pType)
{
	m_delayedResolutionMap.erase(pType);
}

void cvInternalGlobals::copyNonDefaultDelayedResolution(int* pTypeSelf, int* pTypeOther)
{
	if (getDelayedResolution(pTypeSelf) == NULL)
	{
		CvString* pszOther = getDelayedResolution(pTypeOther);
		if (pszOther != NULL)
		{
			addDelayedResolution(pTypeSelf, *pszOther);
		}
	}
}
	
void cvInternalGlobals::resolveDelayedResolution()
{
	for (DelayedResolutionMap::iterator it = m_delayedResolutionMap.begin(); it != m_delayedResolutionMap.end(); ++it)
	{
		GC.setCurrentXMLFile(it->second.second);
		*(it->first) = getInfoTypeForString(it->second.first);
	}
	m_delayedResolutionMap.clear();
}

int cvInternalGlobals::getNumMissionInfos() const
{ 
#ifdef FIXED_MISSION_NUMBER
	return NUM_MISSION_TYPES;
#else
	return (int) m_paMissionInfo.size();
#endif
}

inline CvMap& cvInternalGlobals::getMap() const
{
	return *m_maps[GC.getGame().getCurrentMap()];
}

CvGameAI* cvInternalGlobals::getGamePointer() { return m_game; }

// This determines how many starting civs can be chosen in the custom game staging room screen.
// Currently it allows you to add more civs than there are civ slots.
// If this is reduced to the correct number, the city bar and city graphics for the neanderthal NPC will never be updated as the exe doesn't understand that their cities exist.
int cvInternalGlobals::getMaxCivPlayers() const
{
	return MAX_PLAYERS-1;
}

int cvInternalGlobals::getMaxPCPlayers() const
{
	return MAX_PC_PLAYERS;
}

bool cvInternalGlobals::IsGraphicsInitialized() const { return m_bGraphicsInitialized; }
void cvInternalGlobals::SetGraphicsInitialized(bool bVal) { m_bGraphicsInitialized = bVal; }
void cvInternalGlobals::setInterface(CvInterface* pVal) { m_interface = pVal; }
void cvInternalGlobals::setDiplomacyScreen(CvDiplomacyScreen* pVal) { m_diplomacyScreen = pVal; }
void cvInternalGlobals::setMPDiplomacyScreen(CMPDiplomacyScreen* pVal) { m_mpDiplomacyScreen = pVal; }
void cvInternalGlobals::setMessageQueue(CMessageQueue* pVal) { m_messageQueue = pVal; }
void cvInternalGlobals::setHotJoinMessageQueue(CMessageQueue* pVal) { m_hotJoinMsgQueue = pVal; }
void cvInternalGlobals::setMessageControl(CMessageControl* pVal) { m_messageControl = pVal; }
void cvInternalGlobals::setSetupData(CvSetupData* pVal) { m_setupData = pVal; }
void cvInternalGlobals::setMessageCodeTranslator(CvMessageCodeTranslator* pVal) { m_messageCodes = pVal; }
void cvInternalGlobals::setDropMgr(CvDropMgr* pVal) { m_dropMgr = pVal; }
void cvInternalGlobals::setPortal(CvPortal* pVal) { m_portal = pVal; }
void cvInternalGlobals::setStatsReport(CvStatsReporter* pVal) { m_statsReporter = pVal; }
void cvInternalGlobals::setPathFinder(FAStar* pVal) { m_pathFinder = pVal; }
void cvInternalGlobals::setInterfacePathFinder(FAStar* pVal) { m_interfacePathFinder = pVal; }
void cvInternalGlobals::setStepFinder(FAStar* pVal) { m_stepFinder = pVal; }
void cvInternalGlobals::setRouteFinder(FAStar* pVal) { m_routeFinder = pVal; }
void cvInternalGlobals::setBorderFinder(FAStar* pVal) { m_borderFinder = pVal; }
void cvInternalGlobals::setAreaFinder(FAStar* pVal) { m_areaFinder = pVal; }
void cvInternalGlobals::setPlotGroupFinder(FAStar* pVal) { m_plotGroupFinder = pVal; }
CvDLLUtilityIFaceBase* cvInternalGlobals::getDLLIFaceNonInl() { return g_DLL; }

// BUG - BUG Info - start
static bool bBugInitCalled = false;

bool cvInternalGlobals::bugInitCalled() const
{
	return bBugInitCalled;
}

void cvInternalGlobals::setIsBug(bool bIsBug)
{
	bBugInitCalled = true;

	::setIsBug(bIsBug);

	bool bBUGViewportsEnabled = getBugOptionBOOL("MainInterface__EnableViewports", false);

	//	If viewports are truned on in BUG the settinsg there override those in the global defines
	if ( bBUGViewportsEnabled )
	{
		m_bViewportsEnabled = true;

		int iViewportSizeX = getBugOptionINT("MainInterface__ViewportX", 40);
		int iViewportSizeY = getBugOptionINT("MainInterface__ViewportY", 40);

		int iViewportFocusBorder = getBugOptionINT("MainInterface__ViewportAutoSwitchBorder", 2);

		//	Push them back inot the globals so that a reload of the globals cache preserves these values
		setDefineINT("ENABLE_VIEWPORTS", 1, false);
		setDefineINT("VIEWPORT_SIZE_X", iViewportSizeX, false);
		setDefineINT("VIEWPORT_SIZE_Y", iViewportSizeY, false);
		setDefineINT("VIEWPORT_FOCUS_BORDER", iViewportFocusBorder, false);
		m_iViewportCenterOnSelectionCenterBorder = getBugOptionINT("MainInterface__ViewportAutoCenterBorder", 5);

		//	This happens after the maps load on first load, so resize existing viewports
		for(int iI = 0; iI < GC.getNumMapInfos(); iI++)
		{
			for(int iJ = 0; iJ < GC.getMapByIndex((MapTypes)iI).getNumViewports(); iJ++)
			{
				GC.getMapByIndex((MapTypes)iI).getViewport(iJ)->resizeForMap();
			}
		}
	}
}
// BUG - BUG Info - end

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      02/21/10                                jdog5000      */
/*                                                                                              */
/* Efficiency, Options                                                                          */
/************************************************************************************************/
// BBAI Options
bool cvInternalGlobals::getBBAI_AIR_COMBAT() const
{
	return m_bBBAI_AIR_COMBAT;
}

bool cvInternalGlobals::getBBAI_HUMAN_VASSAL_WAR_BUILD() const
{
	return m_bBBAI_HUMAN_VASSAL_WAR_BUILD;
}

int cvInternalGlobals::getBBAI_DEFENSIVE_PACT_BEHAVIOR() const
{
	return m_iBBAI_DEFENSIVE_PACT_BEHAVIOR;
}

bool cvInternalGlobals::getBBAI_HUMAN_AS_VASSAL_OPTION() const
{
	return m_bBBAI_HUMAN_AS_VASSAL_OPTION;
}

	
// BBAI AI Variables
int cvInternalGlobals::getWAR_SUCCESS_CITY_CAPTURING() const
{
	return m_iWAR_SUCCESS_CITY_CAPTURING;
}

int cvInternalGlobals::getBBAI_ATTACK_CITY_STACK_RATIO() const
{
	return m_iBBAI_ATTACK_CITY_STACK_RATIO;
}

int cvInternalGlobals::getBBAI_SKIP_BOMBARD_BEST_ATTACK_ODDS() const
{
	return m_iBBAI_SKIP_BOMBARD_BEST_ATTACK_ODDS;
}

int cvInternalGlobals::getBBAI_SKIP_BOMBARD_BASE_STACK_RATIO() const
{
	return m_iBBAI_SKIP_BOMBARD_BASE_STACK_RATIO;
}

int cvInternalGlobals::getBBAI_SKIP_BOMBARD_MIN_STACK_RATIO() const
{
	return m_iBBAI_SKIP_BOMBARD_MIN_STACK_RATIO;
}

// Tech Diffusion
bool cvInternalGlobals::getTECH_DIFFUSION_ENABLE() const
{
	return m_bTECH_DIFFUSION_ENABLE;
}

int cvInternalGlobals::getTECH_DIFFUSION_KNOWN_TEAM_MODIFIER() const
{
	return m_iTECH_DIFFUSION_KNOWN_TEAM_MODIFIER;
}

int cvInternalGlobals::getTECH_DIFFUSION_WELFARE_THRESHOLD() const
{
	return m_iTECH_DIFFUSION_WELFARE_THRESHOLD;
}

int cvInternalGlobals::getTECH_DIFFUSION_WELFARE_MODIFIER() const
{
	return m_iTECH_DIFFUSION_WELFARE_MODIFIER;
}

int cvInternalGlobals::getTECH_COST_FIRST_KNOWN_PREREQ_MODIFIER() const
{
	return m_iTECH_COST_FIRST_KNOWN_PREREQ_MODIFIER;
}

int cvInternalGlobals::getTECH_COST_KNOWN_PREREQ_MODIFIER() const
{
	return m_iTECH_COST_KNOWN_PREREQ_MODIFIER;
}

int cvInternalGlobals::getTECH_COST_MODIFIER() const
{
	return m_iTECH_COST_MODIFIER;
}

int cvInternalGlobals::getUNIT_PRODUCTION_PERCENT_SM() const
{
	return m_iUNIT_PRODUCTION_PERCENT_SM;
}

int cvInternalGlobals::getUNIT_PRODUCTION_PERCENT() const
{
	return m_iUNIT_PRODUCTION_PERCENT;
}

int cvInternalGlobals::getBUILDING_PRODUCTION_PERCENT() const
{
	return m_iBUILDING_PRODUCTION_PERCENT;
}

int cvInternalGlobals::getCOMBAT_DIE_SIDES() const
{
	return m_iCOMBAT_DIE_SIDES;
}

int cvInternalGlobals::getCOMBAT_DAMAGE() const
{
	return m_iCOMBAT_DAMAGE;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

/************************************************************************************************/
/* Afforess	                  Start		 12/8/09                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
int cvInternalGlobals::getPEAK_EXTRA_MOVEMENT() const
{
	return m_iPEAK_EXTRA_MOVEMENT;
}

int cvInternalGlobals::getPEAK_EXTRA_DEFENSE() const
{
	return m_iPEAK_EXTRA_DEFENSE;
}

bool cvInternalGlobals::isLoadedPlayerOptions() const
{
	return m_bLoadedPlayerOptions;
}

void cvInternalGlobals::setLoadedPlayerOptions(bool bNewVal)
{
	m_bLoadedPlayerOptions = bNewVal;
}

void cvInternalGlobals::setXMLLogging(bool bNewVal)
{
	m_bXMLLogging = bNewVal;
}

bool cvInternalGlobals::isXMLLogging() const
{
	return m_bXMLLogging;
}

int cvInternalGlobals::getSCORE_FREE_PERCENT() const
{
	return m_iSCORE_FREE_PERCENT;
}

int cvInternalGlobals::getSCORE_POPULATION_FACTOR() const
{
	return m_iSCORE_POPULATION_FACTOR;
}

int cvInternalGlobals::getSCORE_LAND_FACTOR() const
{
	return m_iSCORE_LAND_FACTOR;
}

int cvInternalGlobals::getSCORE_TECH_FACTOR() const
{
	return m_iSCORE_TECH_FACTOR;
}

int cvInternalGlobals::getSCORE_WONDER_FACTOR() const
{
	return m_iSCORE_WONDER_FACTOR;
}

//New Python Callbacks
int cvInternalGlobals::getUSE_CAN_CREATE_PROJECT_CALLBACK() const
{
	return m_iUSE_CAN_CREATE_PROJECT_CALLBACK;
}

int cvInternalGlobals::getUSE_CANNOT_CREATE_PROJECT_CALLBACK() const
{
	return m_iUSE_CANNOT_CREATE_PROJECT_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_DO_MELTDOWN_CALLBACK() const
{
	return m_iUSE_CAN_DO_MELTDOWN_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_MAINTAIN_PROCESS_CALLBACK() const
{
	return m_iUSE_CAN_MAINTAIN_PROCESS_CALLBACK;
}

int cvInternalGlobals::getUSE_CANNOT_MAINTAIN_PROCESS_CALLBACK() const
{
	return m_iUSE_CANNOT_MAINTAIN_PROCESS_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_DO_GROWTH_CALLBACK() const
{
	return m_iUSE_CAN_DO_GROWTH_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_DO_CULTURE_CALLBACK() const
{
	return m_iUSE_CAN_DO_CULTURE_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_DO_PLOT_CULTURE_CALLBACK() const
{
	return m_iUSE_CAN_DO_PLOT_CULTURE_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_DO_PRODUCTION_CALLBACK() const
{
	return m_iUSE_CAN_DO_PRODUCTION_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_DO_RELIGION_CALLBACK() const
{
	return m_iUSE_CAN_DO_RELIGION_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_DO_GREATPEOPLE_CALLBACK() const
{
	return m_iUSE_CAN_DO_GREATPEOPLE_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_RAZE_CITY_CALLBACK() const
{
	return m_iUSE_CAN_RAZE_CITY_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_DO_GOLD_CALLBACK() const
{
	return m_iUSE_CAN_DO_GOLD_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_DO_RESEARCH_CALLBACK() const
{
	return m_iUSE_CAN_DO_RESEARCH_CALLBACK;
}

int cvInternalGlobals::getUSE_UPGRADE_UNIT_PRICE_CALLBACK() const
{
	return m_iUSE_UPGRADE_UNIT_PRICE_CALLBACK;
}

int cvInternalGlobals::getUSE_IS_VICTORY_CALLBACK() const
{
	return m_iUSE_IS_VICTORY_CALLBACK;
}

int cvInternalGlobals::getUSE_AI_UPDATE_UNIT_CALLBACK() const
{
	return m_iUSE_AI_UPDATE_UNIT_CALLBACK;
}

int cvInternalGlobals::getUSE_AI_CHOOSE_PRODUCTION_CALLBACK() const
{
	return m_iUSE_AI_CHOOSE_PRODUCTION_CALLBACK;
}

int cvInternalGlobals::getUSE_EXTRA_PLAYER_COSTS_CALLBACK() const
{
	return m_iUSE_EXTRA_PLAYER_COSTS_CALLBACK;
}

int cvInternalGlobals::getUSE_AI_DO_DIPLO_CALLBACK() const
{
	return m_iUSE_AI_DO_DIPLO_CALLBACK;
}

int cvInternalGlobals::getUSE_AI_BESTTECH_CALLBACK() const
{
	return m_iUSE_AI_BESTTECH_CALLBACK;
}

int cvInternalGlobals::getUSE_CAN_DO_COMBAT_CALLBACK() const
{
	return m_iUSE_CAN_DO_COMBAT_CALLBACK;
}

int cvInternalGlobals::getUSE_AI_CAN_DO_WARPLANS_CALLBACK() const
{
	return m_iUSE_AI_CAN_DO_WARPLANS_CALLBACK;
}

/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

/************************************************************************************************/
/* Mod Globals    Start                          09/13/10                           phungus420  */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
bool cvInternalGlobals::isDCM_BATTLE_EFFECTS() const
{
	return m_bDCM_BATTLE_EFFECTS;
}

int cvInternalGlobals::getBATTLE_EFFECT_LESS_FOOD() const
{
	return m_iBATTLE_EFFECT_LESS_FOOD;
}

int cvInternalGlobals::getBATTLE_EFFECT_LESS_PRODUCTION() const
{
	return m_iBATTLE_EFFECT_LESS_PRODUCTION;
}

int cvInternalGlobals::getBATTLE_EFFECT_LESS_COMMERCE() const
{
	return m_iBATTLE_EFFECT_LESS_COMMERCE;
}

int cvInternalGlobals::getBATTLE_EFFECTS_MINIMUM_TURN_INCREMENTS() const
{
	return m_iBATTLE_EFFECTS_MINIMUM_TURN_INCREMENTS;
}

int cvInternalGlobals::getMAX_BATTLE_TURNS() const
{
	return m_iMAX_BATTLE_TURNS;
}

bool cvInternalGlobals::isDCM_AIR_BOMBING() const
{
	return m_bDCM_AIR_BOMBING;
}

bool cvInternalGlobals::isDCM_RANGE_BOMBARD() const
{
	return m_bDCM_RANGE_BOMBARD;
}

int cvInternalGlobals::getDCM_RB_CITY_INACCURACY() const
{
	return m_iDCM_RB_CITY_INACCURACY;
}

int cvInternalGlobals::getDCM_RB_CITYBOMBARD_CHANCE() const
{
	return m_iDCM_RB_CITYBOMBARD_CHANCE;
}

bool cvInternalGlobals::isDCM_ATTACK_SUPPORT() const
{
	return m_bDCM_ATTACK_SUPPORT;
}

bool cvInternalGlobals::isDCM_OPP_FIRE() const
{
	return m_bDCM_OPP_FIRE;
}

bool cvInternalGlobals::isDCM_ACTIVE_DEFENSE() const
{
	return m_bDCM_ACTIVE_DEFENSE;
}

bool cvInternalGlobals::isDCM_FIGHTER_ENGAGE() const
{
	return m_bDCM_FIGHTER_ENGAGE;
}

bool cvInternalGlobals::isDYNAMIC_CIV_NAMES() const
{
	return m_bDYNAMIC_CIV_NAMES;
}

bool cvInternalGlobals::isLIMITED_RELIGIONS_EXCEPTIONS() const
{
	return m_bLIMITED_RELIGIONS_EXCEPTIONS;
}

bool cvInternalGlobals::isOC_RESPAWN_HOLY_CITIES() const
{
	return m_bOC_RESPAWN_HOLY_CITIES;
}

bool cvInternalGlobals::isIDW_ENABLED() const
{
	return m_bIDW_ENABLED;
}

float cvInternalGlobals::getIDW_BASE_COMBAT_INFLUENCE() const
{
	return m_fIDW_BASE_COMBAT_INFLUENCE;
}

float cvInternalGlobals::getIDW_NO_CITY_DEFENDER_MULTIPLIER() const
{
	return m_fIDW_NO_CITY_DEFENDER_MULTIPLIER;
}

float cvInternalGlobals::getIDW_FORT_CAPTURE_MULTIPLIER() const
{
	return m_fIDW_FORT_CAPTURE_MULTIPLIER;
}

float cvInternalGlobals::getIDW_EXPERIENCE_FACTOR() const
{
	return m_fIDW_EXPERIENCE_FACTOR;
}

float cvInternalGlobals::getIDW_WARLORD_MULTIPLIER() const
{
	return m_fIDW_WARLORD_MULTIPLIER;
}

int cvInternalGlobals::getIDW_INFLUENCE_RADIUS() const
{
	return m_iIDW_INFLUENCE_RADIUS;
}

float cvInternalGlobals::getIDW_PLOT_DISTANCE_FACTOR() const
{
	return m_fIDW_PLOT_DISTANCE_FACTOR;
}

float cvInternalGlobals::getIDW_WINNER_PLOT_MULTIPLIER() const
{
	return m_fIDW_WINNER_PLOT_MULTIPLIER;
}

float cvInternalGlobals::getIDW_LOSER_PLOT_MULTIPLIER() const
{
	return m_fIDW_LOSER_PLOT_MULTIPLIER;
}

bool cvInternalGlobals::isIDW_EMERGENCY_DRAFT_ENABLED() const
{
	return m_bIDW_EMERGENCY_DRAFT_ENABLED;
}

int cvInternalGlobals::getIDW_EMERGENCY_DRAFT_MIN_POPULATION() const
{
	return m_iIDW_EMERGENCY_DRAFT_MIN_POPULATION;
}

float cvInternalGlobals::getIDW_EMERGENCY_DRAFT_STRENGTH() const
{
	return m_fIDW_EMERGENCY_DRAFT_STRENGTH;
}

float cvInternalGlobals::getIDW_EMERGENCY_DRAFT_ANGER_MULTIPLIER() const
{
	return m_fIDW_EMERGENCY_DRAFT_ANGER_MULTIPLIER;
}

bool cvInternalGlobals::isIDW_NO_BARBARIAN_INFLUENCE() const
{
	return m_bIDW_NO_BARBARIAN_INFLUENCE;
}

bool cvInternalGlobals::isIDW_NO_NAVAL_INFLUENCE() const
{
	return m_bIDW_NO_NAVAL_INFLUENCE;
}

bool cvInternalGlobals::isIDW_PILLAGE_INFLUENCE_ENABLED() const
{
	return m_bIDW_PILLAGE_INFLUENCE_ENABLED;
}

float cvInternalGlobals::getIDW_BASE_PILLAGE_INFLUENCE() const
{
	return m_fIDW_BASE_PILLAGE_INFLUENCE;
}

float cvInternalGlobals::getIDW_CITY_TILE_MULTIPLIER() const
{
	return m_fIDW_CITY_TILE_MULTIPLIER;
}

bool cvInternalGlobals::isSS_ENABLED() const
{
	return m_bSS_ENABLED;
}

bool cvInternalGlobals::isSS_BRIBE() const
{
	return m_bSS_BRIBE;
}

bool cvInternalGlobals::isSS_ASSASSINATE() const
{
	return m_bSS_ASSASSINATE;
}

void cvInternalGlobals::setGraphicalDetailPagingEnabled(bool bEnabled)
{
	m_bGraphicalDetailPagingEnabled = bEnabled;
}

bool cvInternalGlobals::getGraphicalDetailPagingEnabled() const
{
	return m_bGraphicalDetailPagingEnabled;
}

int cvInternalGlobals::getGraphicalDetailPageInRange()
{
	return std::max(getGame().getXResolution(), getGame().getYResolution())/150;
}

/************************************************************************************************/
/* Mod Globals                        END                                           phungus420  */
/************************************************************************************************/

// calculate asset checksum
unsigned int cvInternalGlobals::getAssetCheckSum()
{
	CvString szLog;
	unsigned int iSum = 0;
	for (std::vector<std::vector<CvInfoBase*> *>::iterator itOuter = m_aInfoVectors.begin(); itOuter != m_aInfoVectors.end(); ++itOuter)
	{
		for (std::vector<CvInfoBase*>::iterator itInner = (*itOuter)->begin(); itInner != (*itOuter)->end(); ++itInner)
		{
			(*itInner)->getCheckSum(iSum);
			szLog.Format("%s : %u", (*itInner)->getType(), iSum );
			gDLL->logMsg("Checksum.log", szLog.c_str());
		}
	}
	return iSum;
}
