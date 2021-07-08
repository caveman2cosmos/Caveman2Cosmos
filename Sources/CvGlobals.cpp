//
// globals.cpp
//
#include "CvGameCoreDLL.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvInfoWater.h"
#include "CvInitCore.h"
#include "CvMap.h"
#include "CvMapExternal.h"
#include "CvMessageControl.h"
#include "CvPlayerAI.h"
#include "CvPlot.h"
#include "CvPython.h"
#include "CvRandom.h"
#include "CvTeamAI.h"
#include "CvViewport.h"
#include "CvXMLLoadUtility.h"
#include "FVariableSystem.h"
#include "CvImprovementInfo.h"
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

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

CvGlobals gGlobalsProxy;	// for debugging
cvInternalGlobals* gGlobals = NULL;
CvDLLUtilityIFaceBase* gDLL = NULL;

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
	FAssertMsg(gGlobals != NULL, "Method called prior to global instantiation");
}

ProxyTracker::~ProxyTracker()
{
	inDLL--;
	fnName = NULL;
}
#endif

//
// CONSTRUCTOR
//
cvInternalGlobals::cvInternalGlobals()
	: m_paszAnimationOperatorTypes(NULL)
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
	, m_bGraphicsInitialized(false)
	, m_bLogging(false)
	, m_bRandLogging(false)
	, m_bOverwriteLogs(false)
	, m_bSynchLogging(false)
	, m_bDLLProfiler(false)
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
	, m_Profiler(NULL)
	, m_VarSystem(NULL)
	, m_fPLOT_SIZE(0)
	, m_bViewportsEnabled(false)
	, m_iViewportFocusBorder(0)
	, m_iViewportCenterOnSelectionCenterBorder(5)
	, m_szAlternateProfilSampleName("")
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
	, m_bXMLLogging(true)

	// BBAI Options
	, m_bBBAI_AIR_COMBAT(false)
	, m_bBBAI_HUMAN_VASSAL_WAR_BUILD(false)

	// Tech Diffusion
	, m_bTECH_DIFFUSION_ENABLE(false)

	, m_bIsInPedia(false)
	, m_iLastTypeID(-1)
	, m_iActiveLandscapeID(0)
	// uninitialized variables bugfix
	, m_iNumPlayableCivilizationInfos(0)
	, m_iNumAIPlayableCivilizationInfos(0)
	, m_iTotalNumModules(0) // Modular loading control
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
	, m_iViewportSizeX(0)
	, m_iViewportSizeY(0)
	, m_bSignsCleared(false)

#define ADD_TO_CONSTRUCTOR(dataType, VAR) \
	, m_##VAR((dataType)0)

	DO_FOR_EACH_GLOBAL_DEFINE(ADD_TO_CONSTRUCTOR)

#define ADD_INFO_TYPE_TO_CONSTRUCTOR(dataType, VAR) \
	, m_##VAR((dataType)-1)

	DO_FOR_EACH_INFO_TYPE(ADD_INFO_TYPE_TO_CONSTRUCTOR)
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

#ifdef MINIDUMP
	/* Enable our custom exception that will write the minidump for us. */
	SetUnhandledExceptionFilter(CustomFilter);
#endif

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
		0, 1, 2, 3, 3, 3, 2, 1, 0, -1, -2, -3, -3, -3, -2, -1,
	};

	int aiCityPlotY[NUM_CITY_PLOTS] =
	{
		0,
		1, 1, 0,-1,-1,-1, 0, 1,
		2, 2, 1, 0,-1,-2,-2,-2,-1, 0, 1, 2,
		3, 3, 2, 1, 0, -1, -2, -3, -3, -3, -2, -1, 0, 1, 2, 3,
	};

	int aiCityPlotPriority[NUM_CITY_PLOTS] =
	{
		0,
		1, 2, 1, 2, 1, 2, 1, 2,
		3, 4, 4, 3, 4, 4, 3, 4, 4, 3, 4, 4,
		5, 6, 7, 6, 5, 6, 7, 6, 5, 6, 7, 6, 5, 6, 7, 6,
	};

	int aaiXYCityPlot[CITY_PLOTS_DIAMETER][CITY_PLOTS_DIAMETER] =
	{
		{-1, -1, 32, 33, 34, -1, -1},
		{-1, 31, 17, 18, 19, 35, -1},
		{30, 16, 6,   7,  8, 20, 36},
		{29, 15, 5,   0,  1,  9, 21},
		{28, 14, 4,   3,  2, 10, 22},
		{-1, 27, 13, 12, 11, 23, -1},
		{-1, -1, 26, 25, 24, -1, -1},
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

	for (int i = 0; i < NUM_MAPS; i++)
	{
		m_maps[i] = new CvMap((MapTypes)i);
	}

	CvPlayerAI::initStatics();
	CvTeamAI::initStatics();

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

	foreach_(const CvMap* map, m_maps)
	{
		SAFE_DELETE(map);
	}

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

std::vector<CvInterfaceModeInfo*>& cvInternalGlobals::getInterfaceModeInfos()
{
	return m_paInterfaceModeInfo;
}

CvInterfaceModeInfo& cvInternalGlobals::getInterfaceModeInfo(InterfaceModeTypes e) const
{
	FASSERT_BOUNDS(0, NUM_INTERFACEMODE_TYPES, e)
	return *(m_paInterfaceModeInfo[e]);
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

int cvInternalGlobals::getXYCityPlot(int i, int j) const
{
	FASSERT_BOUNDS(0, CITY_PLOTS_DIAMETER, i)
	FASSERT_BOUNDS(0, CITY_PLOTS_DIAMETER, j)
	return m_aaiXYCityPlot[i][j];
}

DirectionTypes* cvInternalGlobals::getTurnLeftDirection() const
{
	return m_aeTurnLeftDirection;
}

DirectionTypes cvInternalGlobals::getTurnLeftDirection(int i) const
{
	FASSERT_BOUNDS(0, DIRECTION_DIAMETER, i)
	return m_aeTurnLeftDirection[i];
}

DirectionTypes* cvInternalGlobals::getTurnRightDirection() const
{
	return m_aeTurnRightDirection;
}

DirectionTypes cvInternalGlobals::getTurnRightDirection(int i) const
{
	FASSERT_BOUNDS(0, DIRECTION_DIAMETER, i)
	return m_aeTurnRightDirection[i];
}

DirectionTypes cvInternalGlobals::getXYDirection(int i, int j) const
{
	FASSERT_BOUNDS(0, DIRECTION_DIAMETER, i)
	FASSERT_BOUNDS(0, DIRECTION_DIAMETER, j)
	return m_aaeXYDirection[i][j];
}

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
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
	return m_paMapInfo.size();
}

CvMapInfo& cvInternalGlobals::getMapInfo(MapTypes eMap) const
{
	FASSERT_BOUNDS(0, NUM_MAPS, eMap)
	return *(m_paMapInfo[eMap]);
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

CvWorldInfo& cvInternalGlobals::getWorldInfo(WorldSizeTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumWorldInfos(), e)
	return *(m_paWorldInfo[e]);
}

int cvInternalGlobals::getNumClimateInfos() const
{
	return (int)m_paClimateInfo.size();
}

CvClimateInfo& cvInternalGlobals::getClimateInfo(ClimateTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumClimateInfos(), e)
	return *(m_paClimateInfo[e]);
}

int cvInternalGlobals::getNumSeaLevelInfos() const
{
	return (int)m_paSeaLevelInfo.size();
}

CvSeaLevelInfo& cvInternalGlobals::getSeaLevelInfo(SeaLevelTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumSeaLevelInfos(), e)
	return *(m_paSeaLevelInfo[e]);
}

int cvInternalGlobals::getNumHints() const
{
	return (int)m_paHints.size();
}

CvInfoBase& cvInternalGlobals::getHints(int i) const
{
	return *(m_paHints[i]);
}

int cvInternalGlobals::getNumMainMenus() const
{
	return (int)m_paMainMenus.size();
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

CvModLoadControlInfo& cvInternalGlobals::getModLoadControlInfos(int iIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumModLoadControlInfos(), iIndex)
	return *(m_paModLoadControls[iIndex]);
}

void cvInternalGlobals::updateReplacements()
{
//ReplacementStep
	m_WorldInfoReplacements.updateReplacements(m_paWorldInfo);
	m_UnitInfoReplacements.updateReplacements(m_paUnitInfo);
	m_TechInfoReplacements.updateReplacements(m_paTechInfo);
	m_TraitInfoReplacements.updateReplacements(m_paTraitInfo);
	m_PromotionInfoReplacements.updateReplacements(m_paPromotionInfo);
	m_BonusClassInfoReplacements.updateReplacements(m_paBonusClassInfo);
	m_BonusInfoReplacements.updateReplacements(m_paBonusInfo);
	m_ImprovementInfoReplacements.updateReplacements(m_paImprovementInfo);
	m_BuildingInfoReplacements.updateReplacements(m_paBuildingInfo);
	m_CivicInfoReplacements.updateReplacements(m_paCivicInfo);
	m_LeaderHeadInfoReplacements.updateReplacements(m_paLeaderHeadInfo);

	m_CivilizationInfoReplacements.updateReplacements(m_paCivilizationInfo);

	m_CultureLevelInfoReplacements.updateReplacements(m_paCultureLevelInfo);

	m_EventInfoReplacements.updateReplacements(m_paEventInfo);
	m_EventTriggerInfoReplacements.updateReplacements(m_paEventTriggerInfo);
	m_ProcessInfoReplacements.updateReplacements(m_paProcessInfo);
	m_TerrainInfoReplacements.updateReplacements(m_paTerrainInfo);

	m_SpecialistInfoReplacements.updateReplacements(m_paSpecialistInfo);

	m_FeatureInfoReplacements.updateReplacements(m_paFeatureInfo);

	m_ReligionInfoReplacements.updateReplacements(m_paReligionInfo);
	m_CorporationInfoReplacements.updateReplacements(m_paCorporationInfo);

	m_RouteInfoReplacements.updateReplacements(m_paRouteInfo);

	m_ProjectInfoReplacements.updateReplacements(m_paProjectInfo);

	m_BuildInfoReplacements.updateReplacements(m_paBuildInfo);

	m_SpawnInfoReplacements.updateReplacements(m_paSpawnInfo);
	m_GameSpeedInfoReplacements.updateReplacements(m_paGameSpeedInfo);
	m_EraInfoReplacements.updateReplacements(m_aEraInfo);

	m_SpecialBuildingInfoReplacements.updateReplacements(m_paSpecialBuildingInfo);

	m_HandicapInfoReplacements.updateReplacements(m_paHandicapInfo);
//ReplacementStep: search down here for 'CvInfoReplacements'
}

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/

int cvInternalGlobals::getNumColorInfos() const
{
	return (int)m_paColorInfo.size();
}

CvColorInfo& cvInternalGlobals::getColorInfo(ColorTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumColorInfos(), e)
	return *(m_paColorInfo[e]);
}


int cvInternalGlobals::getNumPlayerColorInfos() const
{
	return (int)m_paPlayerColorInfo.size();
}

CvPlayerColorInfo& cvInternalGlobals::getPlayerColorInfo(PlayerColorTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumPlayerColorInfos(), e)
	return *(m_paPlayerColorInfo[e]);
}

int cvInternalGlobals::getNumAdvisorInfos() const
{
	return (int)m_paAdvisorInfo.size();
}

CvAdvisorInfo& cvInternalGlobals::getAdvisorInfo(AdvisorTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumAdvisorInfos(), e)
	return *(m_paAdvisorInfo[e]);
}

int cvInternalGlobals::getNumRouteModelInfos() const
{
	return (int)m_paRouteModelInfo.size();
}

CvRouteModelInfo& cvInternalGlobals::getRouteModelInfo(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumRouteModelInfos(), i)
	return *(m_paRouteModelInfo[i]);
}

int cvInternalGlobals::getNumRiverModelInfos() const
{
	return (int)m_paRiverModelInfo.size();
}

CvRiverModelInfo& cvInternalGlobals::getRiverModelInfo(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumRiverModelInfos(), i)
	return *(m_paRiverModelInfo[i]);
}

int cvInternalGlobals::getNumWaterPlaneInfos() const
{
	return (int)m_paWaterPlaneInfo.size();
}

CvWaterPlaneInfo& cvInternalGlobals::getWaterPlaneInfo(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumWaterPlaneInfos(), i)
	return *(m_paWaterPlaneInfo[i]);
}

int cvInternalGlobals::getNumTerrainPlaneInfos() const
{
	return (int)m_paTerrainPlaneInfo.size();
}

CvTerrainPlaneInfo& cvInternalGlobals::getTerrainPlaneInfo(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainPlaneInfos(), i)
	return *(m_paTerrainPlaneInfo[i]);
}

int cvInternalGlobals::getNumCameraOverlayInfos() const
{
	return (int)m_paCameraOverlayInfo.size();
}

CvCameraOverlayInfo& cvInternalGlobals::getCameraOverlayInfo(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumCameraOverlayInfos(), i)
	return *(m_paCameraOverlayInfo[i]);
}

int cvInternalGlobals::getNumAnimationPathInfos() const
{
	return (int)m_paAnimationPathInfo.size();
}

CvAnimationPathInfo& cvInternalGlobals::getAnimationPathInfo(AnimationPathTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumAnimationPathInfos(), e)
	return *(m_paAnimationPathInfo[e]);
}

int cvInternalGlobals::getNumAnimationCategoryInfos() const
{
	return (int)m_paAnimationCategoryInfo.size();
}

CvAnimationCategoryInfo& cvInternalGlobals::getAnimationCategoryInfo(AnimationCategoryTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumAnimationCategoryInfos(), e)
	return *(m_paAnimationCategoryInfo[e]);
}

int cvInternalGlobals::getNumEntityEventInfos() const
{
	return (int)m_paEntityEventInfo.size();
}

CvEntityEventInfo& cvInternalGlobals::getEntityEventInfo(EntityEventTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumEntityEventInfos(), e)
	return *(m_paEntityEventInfo[e]);
}

int cvInternalGlobals::getNumEffectInfos() const
{
	return (int)m_paEffectInfo.size();
}

CvEffectInfo& cvInternalGlobals::getEffectInfo(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumEffectInfos(), i)
	return *(m_paEffectInfo[i]);
}


int cvInternalGlobals::getNumAttachableInfos() const
{
	return (int)m_paAttachableInfo.size();
}

CvAttachableInfo& cvInternalGlobals::getAttachableInfo(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumAttachableInfos(), i)
	return *(m_paAttachableInfo[i]);
}

int cvInternalGlobals::getNumUnitFormationInfos() const
{
	return (int)m_paUnitFormationInfo.size();
}

CvUnitFormationInfo& cvInternalGlobals::getUnitFormationInfo(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitFormationInfos(), i)
	return *(m_paUnitFormationInfo[i]);
}


int cvInternalGlobals::getNumLandscapeInfos() const
{
	return (int)m_paLandscapeInfo.size();
}

CvLandscapeInfo& cvInternalGlobals::getLandscapeInfo(int iIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumLandscapeInfos(), iIndex)
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

CvTerrainInfo& cvInternalGlobals::getTerrainInfo(TerrainTypes eTerrainNum) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), eTerrainNum)
	return *(m_paTerrainInfo[eTerrainNum]);
}

int cvInternalGlobals::getNumBonusClassInfos() const
{
	return (int)m_paBonusClassInfo.size();
}

CvBonusClassInfo& cvInternalGlobals::getBonusClassInfo(BonusClassTypes eBonusNum) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusClassInfos(), eBonusNum)
	return *(m_paBonusClassInfo[eBonusNum]);
}


int cvInternalGlobals::getNumBonusInfos() const
{
	return (int)m_paBonusInfo.size();
}

const std::vector<CvBonusInfo*>& cvInternalGlobals::getBonusInfos() const
{
	return m_paBonusInfo;
}

CvBonusInfo& cvInternalGlobals::getBonusInfo(BonusTypes eBonusNum) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eBonusNum)
	return *(m_paBonusInfo[eBonusNum]);
}

int cvInternalGlobals::getNumFeatureInfos() const
{
	return (int)m_paFeatureInfo.size();
}

CvFeatureInfo& cvInternalGlobals::getFeatureInfo(FeatureTypes eFeatureNum) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), eFeatureNum)
	return *(m_paFeatureInfo[eFeatureNum]);
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

CvCivilizationInfo& cvInternalGlobals::getCivilizationInfo(CivilizationTypes eCivilizationNum) const
{
	FASSERT_BOUNDS(0, GC.getNumCivilizationInfos(), eCivilizationNum)
	return *(m_paCivilizationInfo[eCivilizationNum]);
}


int cvInternalGlobals::getNumLeaderHeadInfos() const
{
	return (int)m_paLeaderHeadInfo.size();
}

CvLeaderHeadInfo& cvInternalGlobals::getLeaderHeadInfo(LeaderHeadTypes eLeaderHeadNum) const
{
	FASSERT_BOUNDS(0, GC.getNumLeaderHeadInfos(), eLeaderHeadNum)
	return *(m_paLeaderHeadInfo[eLeaderHeadNum]);
}


int cvInternalGlobals::getNumTraitInfos() const
{
	return (int)m_paTraitInfo.size();
}

CvTraitInfo& cvInternalGlobals::getTraitInfo(TraitTypes eTraitNum) const
{
	FASSERT_BOUNDS(0, GC.getNumTraitInfos(), eTraitNum)
	return *(m_paTraitInfo[eTraitNum]);
}


int cvInternalGlobals::getNumCursorInfos() const
{
	return (int)m_paCursorInfo.size();
}

CvCursorInfo& cvInternalGlobals::getCursorInfo(CursorTypes eCursorNum) const
{
	FASSERT_BOUNDS(0, GC.getNumCursorInfos(), eCursorNum)
	return *(m_paCursorInfo[eCursorNum]);
}

int cvInternalGlobals::getNumThroneRoomCameras() const
{
	return (int)m_paThroneRoomCamera.size();
}

CvThroneRoomCamera& cvInternalGlobals::getThroneRoomCamera(int iIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumThroneRoomCameras(), iIndex)
	return *(m_paThroneRoomCamera[iIndex]);
}

int cvInternalGlobals::getNumThroneRoomInfos() const
{
	return (int)m_paThroneRoomInfo.size();
}

CvThroneRoomInfo& cvInternalGlobals::getThroneRoomInfo(int iIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumThroneRoomInfos(), iIndex)
	return *(m_paThroneRoomInfo[iIndex]);
}

int cvInternalGlobals::getNumThroneRoomStyleInfos() const
{
	return (int)m_paThroneRoomStyleInfo.size();
}

CvThroneRoomStyleInfo& cvInternalGlobals::getThroneRoomStyleInfo(int iIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumThroneRoomStyleInfos(), iIndex)
	return *(m_paThroneRoomStyleInfo[iIndex]);
}

int cvInternalGlobals::getNumSlideShowInfos() const
{
	return (int)m_paSlideShowInfo.size();
}

CvSlideShowInfo& cvInternalGlobals::getSlideShowInfo(int iIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumSlideShowInfos(), iIndex)
	return *(m_paSlideShowInfo[iIndex]);
}

int cvInternalGlobals::getNumSlideShowRandomInfos() const
{
	return (int)m_paSlideShowRandomInfo.size();
}

CvSlideShowRandomInfo& cvInternalGlobals::getSlideShowRandomInfo(int iIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumSlideShowRandomInfos(), iIndex)
	return *(m_paSlideShowRandomInfo[iIndex]);
}

int cvInternalGlobals::getNumWorldPickerInfos() const
{
	return (int)m_paWorldPickerInfo.size();
}

CvWorldPickerInfo& cvInternalGlobals::getWorldPickerInfo(int iIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumWorldPickerInfos(), iIndex)
	return *(m_paWorldPickerInfo[iIndex]);
}

int cvInternalGlobals::getNumSpaceShipInfos() const
{
	return (int)m_paSpaceShipInfo.size();
}

CvSpaceShipInfo& cvInternalGlobals::getSpaceShipInfo(int iIndex) const
{
	FASSERT_BOUNDS(0, GC.getNumSpaceShipInfos(), iIndex)
	return *(m_paSpaceShipInfo[iIndex]);
}

int cvInternalGlobals::getNumUnitInfos() const
{
	return (int)m_paUnitInfo.size();
}

CvUnitInfo& cvInternalGlobals::getUnitInfo(UnitTypes eUnitNum) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), eUnitNum)
	return *(m_paUnitInfo[eUnitNum]);
}

int cvInternalGlobals::getNumSpawnInfos() const
{
	return (int)m_paSpawnInfo.size();
}

CvSpawnInfo& cvInternalGlobals::getSpawnInfo(SpawnTypes eSpawnNum) const
{
	FASSERT_BOUNDS(0, GC.getNumSpawnInfos(), eSpawnNum)
	return *(m_paSpawnInfo[eSpawnNum]);
}

int cvInternalGlobals::getNumSpecialUnitInfos() const
{
	return (int)m_paSpecialUnitInfo.size();
}

CvSpecialUnitInfo& cvInternalGlobals::getSpecialUnitInfo(SpecialUnitTypes eSpecialUnitNum) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialUnitInfos(), eSpecialUnitNum)
	return *(m_paSpecialUnitInfo[eSpecialUnitNum]);
}


int cvInternalGlobals::getNumConceptInfos() const
{
	return (int)m_paConceptInfo.size();
}

CvInfoBase& cvInternalGlobals::getConceptInfo(ConceptTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumConceptInfos(), e)
	return *(m_paConceptInfo[e]);
}


int cvInternalGlobals::getNumNewConceptInfos() const
{
	return (int)m_paNewConceptInfo.size();
}

CvInfoBase& cvInternalGlobals::getNewConceptInfo(NewConceptTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumNewConceptInfos(), e)
	return *(m_paNewConceptInfo[e]);
}


int cvInternalGlobals::getNumCityTabInfos() const
{
	return (int)m_paCityTabInfo.size();
}

CvInfoBase& cvInternalGlobals::getCityTabInfo(CityTabTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumCityTabInfos(), e)
	return *(m_paCityTabInfo[e]);
}


int cvInternalGlobals::getNumCalendarInfos() const
{
	return (int)m_paCalendarInfo.size();
}

CvInfoBase& cvInternalGlobals::getCalendarInfo(CalendarTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumCalendarInfos(), e)
	return *(m_paCalendarInfo[e]);
}


int cvInternalGlobals::getNumSeasonInfos() const
{
	return (int)m_paSeasonInfo.size();
}

CvInfoBase& cvInternalGlobals::getSeasonInfo(SeasonTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumSeasonInfos(), e)
	return *(m_paSeasonInfo[e]);
}


int cvInternalGlobals::getNumMonthInfos() const
{
	return (int)m_paMonthInfo.size();
}

CvInfoBase& cvInternalGlobals::getMonthInfo(MonthTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumMonthInfos(), e)
	return *(m_paMonthInfo[e]);
}


int cvInternalGlobals::getNumDenialInfos() const
{
	return (int)m_paDenialInfo.size();
}

CvInfoBase& cvInternalGlobals::getDenialInfo(DenialTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumDenialInfos(), e)
	return *(m_paDenialInfo[e]);
}


int cvInternalGlobals::getNumInvisibleInfos() const
{
	return (int)m_paInvisibleInfo.size();
}

CvInvisibleInfo& cvInternalGlobals::getInvisibleInfo(InvisibleTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumInvisibleInfos(), e)
	return *(m_paInvisibleInfo[e]);
}


int cvInternalGlobals::getNumVoteSourceInfos() const
{
	return (int)m_paVoteSourceInfo.size();
}

CvVoteSourceInfo& cvInternalGlobals::getVoteSourceInfo(VoteSourceTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumVoteSourceInfos(), e)
	return *(m_paVoteSourceInfo[e]);
}


int cvInternalGlobals::getNumUnitCombatInfos() const
{
	return (int)m_paUnitCombatInfo.size();
}

CvUnitCombatInfo& cvInternalGlobals::getUnitCombatInfo(UnitCombatTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), e)
	return *(m_paUnitCombatInfo[e]);
}


CvInfoBase& cvInternalGlobals::getDomainInfo(DomainTypes e) const
{
	FASSERT_BOUNDS(0, NUM_DOMAIN_TYPES, e)
	return *(m_paDomainInfo[e]);
}

int cvInternalGlobals::getNumPromotionLineInfos() const
{
	return (int)m_paPromotionLineInfo.size();
}

CvPromotionLineInfo& cvInternalGlobals::getPromotionLineInfo(PromotionLineTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumPromotionLineInfos(), e)
	return *(m_paPromotionLineInfo[e]);
}

int cvInternalGlobals::getNumIdeaClassInfos() const
{
	return (int)m_paIdeaClassInfo.size();
}

CvIdeaClassInfo& cvInternalGlobals::getIdeaClassInfo(IdeaClassTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumIdeaClassInfos(), e)
	return *(m_paIdeaClassInfo[e]);
}

int cvInternalGlobals::getNumIdeaInfos() const
{
	return (int)m_paIdeaInfo.size();
}

CvIdeaInfo& cvInternalGlobals::getIdeaInfo(IdeaTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumIdeaInfos(), e)
	return *(m_paIdeaInfo[e]);
}
//int cvInternalGlobals::getNumTraitOptionEditsInfos() const
//{
//	return (int)m_paTraitOptionEditsInfo.size();
//}
//
//CvTraitOptionEditsInfo& cvInternalGlobals::getTraitOptionEditsInfo(TraitOptionEditsTypes e) const
//{
//	FASSERT_BOUNDS(0, GC.getNumTraitOptionEditsInfos(), e)
//	return *(m_paTraitOptionEditsInfo[e]);
//}

CvInfoBase& cvInternalGlobals::getUnitAIInfo(UnitAITypes eUnitAINum) const
{
	FASSERT_BOUNDS(0, NUM_UNITAI_TYPES, eUnitAINum)
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

CvInfoBase& cvInternalGlobals::getAttitudeInfo(AttitudeTypes eAttitudeNum) const
{
	FASSERT_BOUNDS(0, NUM_ATTITUDE_TYPES, eAttitudeNum)
	return *(m_paAttitudeInfos[eAttitudeNum]);
}


CvInfoBase& cvInternalGlobals::getMemoryInfo(MemoryTypes eMemoryNum) const
{
	FASSERT_BOUNDS(0, NUM_MEMORY_TYPES, eMemoryNum)
	return *(m_paMemoryInfos[eMemoryNum]);
}


int cvInternalGlobals::getNumGameOptionInfos() const
{
	return (int)m_paGameOptionInfos.size();
}

CvGameOptionInfo& cvInternalGlobals::getGameOptionInfo(GameOptionTypes eGameOptionNum) const
{
	FASSERT_BOUNDS(0, GC.getNumGameOptionInfos(), eGameOptionNum)
	return *(m_paGameOptionInfos[eGameOptionNum]);
}

int cvInternalGlobals::getNumMPOptionInfos() const
{
	return (int)m_paMPOptionInfos.size();
}

CvMPOptionInfo& cvInternalGlobals::getMPOptionInfo(MultiplayerOptionTypes eMPOptionNum) const
{
	FASSERT_BOUNDS(0, GC.getNumMPOptionInfos(), eMPOptionNum)
	return *(m_paMPOptionInfos[eMPOptionNum]);
}

int cvInternalGlobals::getNumForceControlInfos() const
{
	return (int)m_paForceControlInfos.size();
}

CvForceControlInfo& cvInternalGlobals::getForceControlInfo(ForceControlTypes eForceControlNum) const
{
	FASSERT_BOUNDS(0, GC.getNumForceControlInfos(), eForceControlNum)
	return *(m_paForceControlInfos[eForceControlNum]);
}

CvPlayerOptionInfo& cvInternalGlobals::getPlayerOptionInfo(PlayerOptionTypes ePlayerOptionNum) const
{
	FASSERT_BOUNDS(0, NUM_PLAYEROPTION_TYPES, ePlayerOptionNum)
	return *(m_paPlayerOptionInfos[ePlayerOptionNum]);
}

CvGraphicOptionInfo& cvInternalGlobals::getGraphicOptionInfo(GraphicOptionTypes eGraphicOptionNum) const
{
	FASSERT_BOUNDS(0, NUM_GRAPHICOPTION_TYPES, eGraphicOptionNum)
	return *(m_paGraphicOptionInfos[eGraphicOptionNum]);
}

CvYieldInfo& cvInternalGlobals::getYieldInfo(YieldTypes eYieldNum) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eYieldNum)
	return *(m_paYieldInfo[eYieldNum]);
}

CvCommerceInfo& cvInternalGlobals::getCommerceInfo(CommerceTypes eCommerceNum) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, eCommerceNum)
	return *(m_paCommerceInfo[eCommerceNum]);
}

int cvInternalGlobals::getNumRouteInfos() const
{
	return (int)m_paRouteInfo.size();
}

CvRouteInfo& cvInternalGlobals::getRouteInfo(RouteTypes eRouteNum) const
{
	FASSERT_BOUNDS(0, GC.getNumRouteInfos(), eRouteNum)
	return *(m_paRouteInfo[eRouteNum]);
}

int cvInternalGlobals::getNumImprovementInfos() const
{
	return (int)m_paImprovementInfo.size();
}

CvImprovementInfo& cvInternalGlobals::getImprovementInfo(ImprovementTypes eImprovementNum) const
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), eImprovementNum)
	return *(m_paImprovementInfo[eImprovementNum]);
}

int cvInternalGlobals::getNumGoodyInfos() const
{
	return (int)m_paGoodyInfo.size();
}

CvGoodyInfo& cvInternalGlobals::getGoodyInfo(GoodyTypes eGoodyNum) const
{
	FASSERT_BOUNDS(0, GC.getNumGoodyInfos(), eGoodyNum)
	return *(m_paGoodyInfo[eGoodyNum]);
}

int cvInternalGlobals::getNumBuildInfos() const
{
	return (int)m_paBuildInfo.size();
}

CvBuildInfo& cvInternalGlobals::getBuildInfo(BuildTypes eBuildNum) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildInfos(), eBuildNum)
	return *(m_paBuildInfo[eBuildNum]);
}

int cvInternalGlobals::getNumHandicapInfos() const
{
	return (int)m_paHandicapInfo.size();
}

CvHandicapInfo& cvInternalGlobals::getHandicapInfo(HandicapTypes eHandicapNum) const
{
	FASSERT_BOUNDS(0, GC.getNumHandicapInfos(), eHandicapNum)
	return *(m_paHandicapInfo[eHandicapNum]);
}

int cvInternalGlobals::getNumGameSpeedInfos() const
{
	return (int)m_paGameSpeedInfo.size();
}

CvGameSpeedInfo& cvInternalGlobals::getGameSpeedInfo(GameSpeedTypes eGameSpeedNum) const
{
	FASSERT_BOUNDS(0, GC.getNumGameSpeedInfos(), eGameSpeedNum)
	return *(m_paGameSpeedInfo[eGameSpeedNum]);
}

int cvInternalGlobals::getNumTurnTimerInfos() const
{
	return (int)m_paTurnTimerInfo.size();
}

CvTurnTimerInfo& cvInternalGlobals::getTurnTimerInfo(TurnTimerTypes eTurnTimerNum) const
{
	FASSERT_BOUNDS(0, GC.getNumTurnTimerInfos(), eTurnTimerNum)
	return *(m_paTurnTimerInfo[eTurnTimerNum]);
}

int cvInternalGlobals::getNumProcessInfos() const
{
	return (int)m_paProcessInfo.size();
}

CvProcessInfo& cvInternalGlobals::getProcessInfo(ProcessTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumProcessInfos(), e)
	return *(m_paProcessInfo[e]);
}

int cvInternalGlobals::getNumVoteInfos() const
{
	return (int)m_paVoteInfo.size();
}

CvVoteInfo& cvInternalGlobals::getVoteInfo(VoteTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumVoteInfos(), e)
	return *(m_paVoteInfo[e]);
}

int cvInternalGlobals::getNumProjectInfos() const
{
	return (int)m_paProjectInfo.size();
}

CvProjectInfo& cvInternalGlobals::getProjectInfo(ProjectTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumProjectInfos(), e)
	return *(m_paProjectInfo[e]);
}

int cvInternalGlobals::getNumBuildingInfos() const
{
	return (int)m_paBuildingInfo.size();
}

CvBuildingInfo& cvInternalGlobals::getBuildingInfo(BuildingTypes eBuildingNum) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), eBuildingNum)
	return *(m_paBuildingInfo[eBuildingNum]);
}

int cvInternalGlobals::getNumSpecialBuildingInfos() const
{
	return (int)m_paSpecialBuildingInfo.size();
}

CvSpecialBuildingInfo& cvInternalGlobals::getSpecialBuildingInfo(SpecialBuildingTypes eSpecialBuildingNum) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialBuildingInfos(), eSpecialBuildingNum)
	return *(m_paSpecialBuildingInfo[eSpecialBuildingNum]);
}

int cvInternalGlobals::getNumActionInfos() const
{
	return (int)m_paActionInfo.size();
}

CvActionInfo& cvInternalGlobals::getActionInfo(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumActionInfos(), i)
	return *(m_paActionInfo[i]);
}

CvMissionInfo& cvInternalGlobals::getMissionInfo(MissionTypes eMissionNum) const
{
	FASSERT_BOUNDS(0, GC.getNumMissionInfos(), eMissionNum)
	return *(m_paMissionInfo[eMissionNum]);
}

CvControlInfo& cvInternalGlobals::getControlInfo(ControlTypes eControlNum) const
{
	FASSERT_BOUNDS(0, NUM_CONTROL_TYPES, eControlNum)
	FAssert(!m_paControlInfo.empty());
	return *(m_paControlInfo[eControlNum]);
}

CvCommandInfo& cvInternalGlobals::getCommandInfo(CommandTypes eCommandNum) const
{
	FASSERT_BOUNDS(0, NUM_COMMAND_TYPES, eCommandNum)
	return *(m_paCommandInfo[eCommandNum]);
}

int cvInternalGlobals::getNumAutomateInfos() const
{
	return (int)m_paAutomateInfo.size();
}

CvAutomateInfo& cvInternalGlobals::getAutomateInfo(int iAutomateNum) const
{
	FASSERT_BOUNDS(0, getNumAutomateInfos(), iAutomateNum)
	return *(m_paAutomateInfo[iAutomateNum]);
}

int cvInternalGlobals::getNumPromotionInfos() const
{
	return (int)m_paPromotionInfo.size();
}

CvPromotionInfo& cvInternalGlobals::getPromotionInfo(PromotionTypes ePromotionNum) const
{
	FASSERT_BOUNDS(0, GC.getNumPromotionInfos(), ePromotionNum)
	return *(m_paPromotionInfo[ePromotionNum]);
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

CvTechInfo& cvInternalGlobals::getTechInfo(TechTypes eTechNum) const
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eTechNum)
	return *(m_paTechInfo[eTechNum]);
}

int cvInternalGlobals::getNumReligionInfos() const
{
	return (int)m_paReligionInfo.size();
}

CvReligionInfo& cvInternalGlobals::getReligionInfo(ReligionTypes eReligionNum) const
{
	FASSERT_BOUNDS(0, GC.getNumReligionInfos(), eReligionNum)
	return *(m_paReligionInfo[eReligionNum]);
}

int cvInternalGlobals::getNumCorporationInfos() const
{
	return (int)m_paCorporationInfo.size();
}

CvCorporationInfo& cvInternalGlobals::getCorporationInfo(CorporationTypes eCorporationNum) const
{
	FASSERT_BOUNDS(0, GC.getNumCorporationInfos(), eCorporationNum)
	return *(m_paCorporationInfo[eCorporationNum]);
}

int cvInternalGlobals::getNumSpecialistInfos() const
{
	return (int)m_paSpecialistInfo.size();
}

CvSpecialistInfo& cvInternalGlobals::getSpecialistInfo(SpecialistTypes eSpecialistNum) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), eSpecialistNum)
	return *(m_paSpecialistInfo[eSpecialistNum]);
}

int cvInternalGlobals::getNumCivicOptionInfos() const
{
	return (int)m_paCivicOptionInfo.size();
}

CvCivicOptionInfo& cvInternalGlobals::getCivicOptionInfo(CivicOptionTypes eCivicOptionNum) const
{
	FASSERT_BOUNDS(0, GC.getNumCivicOptionInfos(), eCivicOptionNum)
	return *(m_paCivicOptionInfo[eCivicOptionNum]);
}

int cvInternalGlobals::getNumCivicInfos() const
{
	return (int)m_paCivicInfo.size();
}

CvCivicInfo& cvInternalGlobals::getCivicInfo(CivicTypes eCivicNum) const
{
	FASSERT_BOUNDS(0, GC.getNumCivicInfos(), eCivicNum)
	return *(m_paCivicInfo[eCivicNum]);
}

int cvInternalGlobals::getNumDiplomacyInfos() const
{
	return (int)m_paDiplomacyInfo.size();
}

CvDiplomacyInfo& cvInternalGlobals::getDiplomacyInfo(int iDiplomacyNum) const
{
	FASSERT_BOUNDS(0, GC.getNumDiplomacyInfos(), iDiplomacyNum)
	return *(m_paDiplomacyInfo[iDiplomacyNum]);
}

int cvInternalGlobals::getNumEraInfos() const
{
	return (int)m_aEraInfo.size();
}

CvEraInfo& cvInternalGlobals::getEraInfo(EraTypes eEraNum) const
{
	FASSERT_BOUNDS(0, GC.getNumEraInfos(), eEraNum)
	return *(m_aEraInfo[eEraNum]);
}

int cvInternalGlobals::getNumHurryInfos() const
{
	return (int)m_paHurryInfo.size();
}

CvHurryInfo& cvInternalGlobals::getHurryInfo(HurryTypes eHurryNum) const
{
	FASSERT_BOUNDS(0, GC.getNumHurryInfos(), eHurryNum)
	return *(m_paHurryInfo[eHurryNum]);
}

int cvInternalGlobals::getNumEmphasizeInfos() const
{
	return (int)m_paEmphasizeInfo.size();
}

CvEmphasizeInfo& cvInternalGlobals::getEmphasizeInfo(EmphasizeTypes eEmphasizeNum) const
{
	FASSERT_BOUNDS(0, GC.getNumEmphasizeInfos(), eEmphasizeNum)
	return *(m_paEmphasizeInfo[eEmphasizeNum]);
}

int cvInternalGlobals::getNumUpkeepInfos() const
{
	return (int)m_paUpkeepInfo.size();
}

CvUpkeepInfo& cvInternalGlobals::getUpkeepInfo(UpkeepTypes eUpkeepNum) const
{
	FASSERT_BOUNDS(0, GC.getNumUpkeepInfos(), eUpkeepNum)
	return *(m_paUpkeepInfo[eUpkeepNum]);
}

int cvInternalGlobals::getNumCultureLevelInfos() const
{
	return (int)m_paCultureLevelInfo.size();
}

CvCultureLevelInfo& cvInternalGlobals::getCultureLevelInfo(CultureLevelTypes eCultureLevelNum) const
{
	FASSERT_BOUNDS(0, GC.getNumCultureLevelInfos(), eCultureLevelNum)
	return *(m_paCultureLevelInfo[eCultureLevelNum]);
}

int cvInternalGlobals::getNumVictoryInfos() const
{
	return (int)m_paVictoryInfo.size();
}

CvVictoryInfo& cvInternalGlobals::getVictoryInfo(VictoryTypes eVictoryNum) const
{
	FASSERT_BOUNDS(0, GC.getNumVictoryInfos(), eVictoryNum)
	return *(m_paVictoryInfo[eVictoryNum]);
}

int cvInternalGlobals::getNumEventTriggerInfos() const
{
	return (int)m_paEventTriggerInfo.size();
}

CvEventTriggerInfo& cvInternalGlobals::getEventTriggerInfo(EventTriggerTypes eEventTrigger) const
{
	FASSERT_BOUNDS(0, GC.getNumEventTriggerInfos(), eEventTrigger)
	return *(m_paEventTriggerInfo[eEventTrigger]);
}

int cvInternalGlobals::getNumEventInfos() const
{
	return (int)m_paEventInfo.size();
}

CvEventInfo& cvInternalGlobals::getEventInfo(EventTypes eEvent) const
{
	FASSERT_BOUNDS(0, GC.getNumEventInfos(), eEvent)
	return *(m_paEventInfo[eEvent]);
}

int cvInternalGlobals::getNumEspionageMissionInfos() const
{
	return (int)m_paEspionageMissionInfo.size();
}

CvEspionageMissionInfo& cvInternalGlobals::getEspionageMissionInfo(EspionageMissionTypes eEspionageMissionNum) const
{
	FASSERT_BOUNDS(0, GC.getNumEspionageMissionInfos(), eEspionageMissionNum)
	return *(m_paEspionageMissionInfo[eEspionageMissionNum]);
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
	FASSERT_BOUNDS(0, GC.getNumAnimationOperatorTypes(), e)
	return m_paszAnimationOperatorTypes[e];
}

CvString*& cvInternalGlobals::getFunctionTypes()
{
	return m_paszFunctionTypes;
}

CvString& cvInternalGlobals::getFunctionTypes(FunctionTypes e)
{
	FASSERT_BOUNDS(0, NUM_FUNC_TYPES, e)
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
	FASSERT_BOUNDS(0, GC.getNumFlavorTypes(), e)
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
	FASSERT_BOUNDS(0, GC.getNumArtStyleTypes(), e)
	return m_paszArtStyleTypes[e];
}

int cvInternalGlobals::getNumUnitArtStyleTypeInfos() const
{
	return (int)m_paUnitArtStyleTypeInfo.size();
}

CvUnitArtStyleTypeInfo& cvInternalGlobals::getUnitArtStyleTypeInfo(UnitArtStyleTypes eUnitArtStyleTypeNum) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitArtStyleTypeInfos(), eUnitArtStyleTypeNum)
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
	FASSERT_BOUNDS(0, GC.getNumCitySizeTypes(), i)
	return m_paszCitySizeTypes[i];
}

CvString*& cvInternalGlobals::getContactTypes()
{
	return m_paszContactTypes;
}

CvString& cvInternalGlobals::getContactTypes(ContactTypes e)
{
	FASSERT_BOUNDS(0, NUM_CONTACT_TYPES, e)
	return m_paszContactTypes[e];
}

CvString*& cvInternalGlobals::getDiplomacyPowerTypes()
{
	return m_paszDiplomacyPowerTypes;
}

CvString& cvInternalGlobals::getDiplomacyPowerTypes(DiplomacyPowerTypes e)
{
	FASSERT_BOUNDS(0, NUM_DIPLOMACYPOWER_TYPES, e)
	return m_paszDiplomacyPowerTypes[e];
}

CvString*& cvInternalGlobals::getAutomateTypes()
{
	return m_paszAutomateTypes;
}

CvString& cvInternalGlobals::getAutomateTypes(AutomateTypes e)
{
	FASSERT_BOUNDS(0, NUM_AUTOMATE_TYPES, e)
	return m_paszAutomateTypes[e];
}

CvString*& cvInternalGlobals::getDirectionTypes()
{
	return m_paszDirectionTypes;
}

CvString& cvInternalGlobals::getDirectionTypes(AutomateTypes e)
{
	FASSERT_BOUNDS(0, NUM_AUTOMATE_TYPES, e)
	return m_paszDirectionTypes[e];
}

int cvInternalGlobals::getNumPropertyInfos() const
{
	return (int)m_paPropertyInfo.size();
}

CvPropertyInfo& cvInternalGlobals::getPropertyInfo(PropertyTypes ePropertyNum) const
{
	FASSERT_BOUNDS(0, GC.getNumPropertyInfos(), ePropertyNum)
	return *(m_paPropertyInfo[ePropertyNum]);
}

int cvInternalGlobals::getNumOutcomeInfos() const
{
	return (int)m_paOutcomeInfo.size();
}

CvOutcomeInfo& cvInternalGlobals::getOutcomeInfo(OutcomeTypes eOutcomeNum) const
{
	FASSERT_BOUNDS(0, GC.getNumOutcomeInfos(), eOutcomeNum)
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
	FASSERT_BOUNDS(0, GC.getNumFootstepAudioTypes(), i)
	return m_paszFootstepAudioTypes[i];
}

int cvInternalGlobals::getFootstepAudioTypeByTag(const CvString strTag) const
{
	if (strTag.GetLength() > 0)
	{
		for (int i = 0; i < m_iNumFootstepAudioTypes; i++)
		{
			if (strTag.CompareNoCase(m_paszFootstepAudioTypes[i]) == 0)
			{
				return i;
			}
		}
	}
	return -1;
}

CvString*& cvInternalGlobals::getFootstepAudioTags()
{
	return m_paszFootstepAudioTags;
}

CvString& cvInternalGlobals::getFootstepAudioTags(int i) const
{
	static CvString* emptyString = NULL;

	if ( emptyString == NULL )
	{
		emptyString = new CvString("");
	}
	FASSERT_BOUNDS(0, GC.getNumFootstepAudioTypes(), i)
	return m_paszFootstepAudioTags ? m_paszFootstepAudioTags[i] : *emptyString;
}

void cvInternalGlobals::setCurrentXMLFile(const TCHAR* szFileName)
{
	m_szCurrentXMLFile = szFileName;
}

const CvString& cvInternalGlobals::getCurrentXMLFile() const
{
	return m_szCurrentXMLFile;
}

FVariableSystem* cvInternalGlobals::getDefinesVarSystem() const
{
	return m_VarSystem;
}

void cvInternalGlobals::cacheEnumGlobals()
{
#define CACHE_ENUM_GLOBAL_DEFINE(dataType, VAR) \
	m_##VAR = getDefineINT(#VAR);
	DO_FOR_EACH_ENUM_GLOBAL_DEFINE(CACHE_ENUM_GLOBAL_DEFINE)
}

void cvInternalGlobals::cacheGlobals()
{
	OutputDebugString("Caching Globals: Start/n");

	strcpy(gVersionString, getDefineSTRING("C2C_VERSION"));

#define CACHE_INT_GLOBAL_DEFINE(dataType, VAR) \
	m_##VAR = getDefineINT(#VAR);
	DO_FOR_EACH_INT_GLOBAL_DEFINE(CACHE_INT_GLOBAL_DEFINE)
	DO_FOR_EACH_BOOL_GLOBAL_DEFINE(CACHE_INT_GLOBAL_DEFINE)

#define CACHE_FLOAT_GLOBAL_DEFINE(dataType, VAR) \
	m_##VAR = getDefineFLOAT(#VAR);
	DO_FOR_EACH_FLOAT_GLOBAL_DEFINE(CACHE_FLOAT_GLOBAL_DEFINE)

	m_fPLOT_SIZE = getDefineFLOAT("PLOT_SIZE");

	m_bBBAI_AIR_COMBAT = !(getDefineINT("BBAI_AIR_COMBAT") == 0);
	m_bBBAI_HUMAN_VASSAL_WAR_BUILD = !(getDefineINT("BBAI_HUMAN_VASSAL_WAR_BUILD") == 0);

	m_bTECH_DIFFUSION_ENABLE = !(getDefineINT("TECH_DIFFUSION_ENABLE") == 0);

	m_bXMLLogging = getDefineINT("XML_LOGGING_ENABLED");

	m_bViewportsEnabled = (getDefineINT("ENABLE_VIEWPORTS") != 0);
	m_iViewportFocusBorder = GC.getDefineINT("VIEWPORT_FOCUS_BORDER");
	m_iViewportSizeX = GC.getDefineINT("VIEWPORT_SIZE_X");
	m_iViewportSizeY = GC.getDefineINT("VIEWPORT_SIZE_Y");

	m_szAlternateProfilSampleName = getDefineSTRING("PROFILER_ALTERNATE_SAMPLE_SET_SOURCE");
	if (m_szAlternateProfilSampleName == NULL)
	{
		m_szAlternateProfilSampleName = "";
	}

	OutputDebugString("Caching Globals: End/n");
}


bool cvInternalGlobals::getDefineBOOL(const char * szName) const
{
	bool bReturn = false;
	bool success = GC.getDefinesVarSystem()->GetValue( szName, bReturn );
	//FAssertMsg( success, szName );
	return bReturn;
}

int cvInternalGlobals::getDefineINT(const char * szName, const int iDefault) const
{
	int iReturn = 0;

	if (GC.getDefinesVarSystem()->GetValue(szName, iReturn))
	{
		return iReturn;
	}
	return iDefault;
}

int cvInternalGlobals::getDefineINT(const char * szName) const
{
	int iReturn = 0;
	GC.getDefinesVarSystem()->GetValue(szName, iReturn);
	return iReturn;
}

float cvInternalGlobals::getDefineFLOAT(const char * szName) const
{
	float fReturn = 0;
	GC.getDefinesVarSystem()->GetValue(szName, fReturn);
	return fReturn;
}

const char * cvInternalGlobals::getDefineSTRING(const char * szName) const
{
	const char * szReturn = NULL;
	GC.getDefinesVarSystem()->GetValue(szName, szReturn);
	return szReturn;
}

void cvInternalGlobals::setDefineINT(const char * szName, int iValue, bool bUpdate)
{
	if (getDefineINT(szName) != iValue)
	{
		if (bUpdate)
		{
			CvMessageControl::getInstance().sendGlobalDefineUpdate(szName, iValue, -1.0f, "");
		}
		else GC.getDefinesVarSystem()->SetValue(szName, iValue);

		cacheEnumGlobals();
		cacheGlobals();
	}
}

void cvInternalGlobals::setDefineFLOAT(const char * szName, float fValue, bool bUpdate)
{
	if (getDefineFLOAT(szName) != fValue)
	{
		if (bUpdate)
		{
			CvMessageControl::getInstance().sendGlobalDefineUpdate(szName, -1, fValue, "");
		}
		else GC.getDefinesVarSystem()->SetValue(szName, fValue);

		cacheGlobals();
	}
}

void cvInternalGlobals::setDefineSTRING( const char * szName, const char * szValue, bool bUpdate )
{
	if (getDefineSTRING(szName) != szValue)
	{
		if (bUpdate)
		{
			CvMessageControl::getInstance().sendGlobalDefineUpdate(szName, -1, -1.0f, szValue);
		}
		else GC.getDefinesVarSystem()->SetValue( szName, szValue );

		cacheGlobals(); // TO DO : we should not cache all globals at each single set
	}
}


float cvInternalGlobals::getPLOT_SIZE() const
{
	CvMapExternal& kMap = GC.getMapExternal();
	kMap.mapCoordinates(true);

	return m_fPLOT_SIZE;
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
	if (bEnable)
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

//
// Global Types Hash Map
//
int cvInternalGlobals::getTypesEnum(const char* szType) const
{
	FAssertMsg(szType, "null type string");
	TypesMap::const_iterator it = m_typesMap.find(szType);
	if (it != m_typesMap.end())
	{
		return it->second;
	}

	FAssertMsg(strcmp(szType, "NONE") == 0 || strcmp(szType, "") == 0, CvString::format("type %s not found", szType).c_str());
	return -1;
}

void cvInternalGlobals::setTypesEnum(const char* szType, int iEnum)
{
	FAssertMsg(szType, "null type string");
	FAssertMsg(m_typesMap.find(szType) == m_typesMap.end(), "types entry already exists");
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
	deleteInfoArray(m_paPromotionLineInfo);
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

//
// Global Infos Hash Map
//
int cvInternalGlobals::getInfoTypeForString(const char* szType, bool hideAssert) const
{
	FAssertMsg(szType, "null info type string");

	InfosMap::const_iterator it = m_infosMap.find(szType);

	if (it != m_infosMap.end())
	{
		return it->second;
	}
	if (stricmp(szType, "NONE") != 0 && strcmp(szType, "") != 0 && !hideAssert && !getDefineINT(szType))
	{
		CvString szError;
		szError.Format("info type '%s' not found, Current XML file is: %s", szType, GC.getCurrentXMLFile().GetCString());
		FAssertMsg(stricmp(szType, "NONE") == 0 || strcmp(szType, "") == 0, szError.c_str());

		logging::logMsg("Xml_MissingTypes.log", szError.c_str());
	}
	return -1;
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
	OutputDebugString(CvString::format("%s -> %d\n", szType, idx).c_str());

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
		logging::logMsg("cvInternalGlobals_logInfoTypeMap.log", " === Info Type Map Dump BEGIN: %s ===", tagMsg);

		int iCnt = 0;
		std::vector<std::string> vInfoMapKeys;
		for (InfosMap::const_iterator it = m_infosMap.begin(); it != m_infosMap.end(); ++it)
		{
			std::string sKey = it->first;
			vInfoMapKeys.push_back(sKey);
		}

		std::sort(vInfoMapKeys.begin(), vInfoMapKeys.end());

		foreach_(const std::string& sKey, vInfoMapKeys)
		{
			logging::logMsg("cvInternalGlobals_logInfoTypeMap.log", " * %i --  %s: %i", iCnt, sKey.c_str(), m_infosMap[sKey.c_str()]);
			iCnt++;
		}

		logging::logMsg("cvInternalGlobals_logInfoTypeMap.log", "Entries in total: %i", iCnt);
		logging::logMsg("cvInternalGlobals_logInfoTypeMap.log", " === Info Type Map Dump END: %s ===", tagMsg);
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
	foreach_(const std::vector<CvInfoBase*>* infoVector, m_aInfoVectors)
	{
		foreach_(CvInfoBase* info, *infoVector)
			info->reset();
	}
}

void cvInternalGlobals::cacheInfoTypes()
{
#define CACHE_INFO_TYPE(type, VAR) \
	m_##VAR = (type)getInfoTypeForString(#VAR);

	DO_FOR_EACH_INFO_TYPE(CACHE_INFO_TYPE)
}

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/

void cvInternalGlobals::switchMap(MapTypes eMap)
{
	FASSERT_BOUNDS(0, NUM_MAPS, eMap);
	FAssert(eMap != CURRENT_MAP);

	GC.getMap().beforeSwitch();
	GC.getGame().setCurrentMap(eMap);
	GC.getMap().afterSwitch();
}

CvViewport* cvInternalGlobals::getCurrentViewport() const
{
	return m_maps[CURRENT_MAP]->getCurrentViewport();
}

int	cvInternalGlobals::getViewportSizeX() const
{
	return GC.viewportsEnabled() ? std::min(m_iViewportSizeX, m_maps[CURRENT_MAP]->getGridHeight()) : m_maps[CURRENT_MAP]->getGridWidth();
}

int	cvInternalGlobals::getViewportSizeY() const
{
	return GC.viewportsEnabled() ? std::min(m_iViewportSizeY, m_maps[CURRENT_MAP]->getGridHeight()) : m_maps[CURRENT_MAP]->getGridHeight();
}

int	cvInternalGlobals::getViewportSelectionBorder() const
{
	return m_iViewportFocusBorder;
}

int	cvInternalGlobals::getViewportCenteringBorder() const
{
	return m_iViewportCenterOnSelectionCenterBorder;
}


CvMapExternal& cvInternalGlobals::getMapExternal() const
{
	CvViewport* currentViewport = getCurrentViewport();

	FAssert(currentViewport != NULL);

	return *currentViewport->getProxy();
}

CvMap& cvInternalGlobals::getMapByIndex(MapTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_MAPS, eIndex)
	return *m_maps[eIndex];
}

void cvInternalGlobals::clearSigns()
{
	gDLL->getEngineIFace()->clearSigns();

	m_bSignsCleared = true;
}

void cvInternalGlobals::reprocessSigns()
{
	if (m_bSignsCleared)
	{
		Cy::call(PYCivModule, "AddSign", Cy::Args() << (CvPlot*)NULL << NO_PLAYER << "");
		m_bSignsCleared = false;
	}
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
	return (int) m_paMissionInfo.size();
}

CvMap& cvInternalGlobals::getMap() const
{
	return *m_maps[GC.getGame().getCurrentMap()];
}

CvGameAI* cvInternalGlobals::getGamePointer() { return m_game; }

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


static bool bBugInitCalled = false;

bool cvInternalGlobals::bugInitCalled() const
{
	return bBugInitCalled;
}

void cvInternalGlobals::setIsBug()
{
	bBugInitCalled = true;

	::setIsBug();

	//	If viewports are truned on in BUG the settinsg there override those in the global defines
	if (getBugOptionBOOL("MainInterface__EnableViewports", false))
	{
		m_bViewportsEnabled = true;

		// Push them back inot the globals so that a reload of the globals cache preserves these values
		setDefineINT("ENABLE_VIEWPORTS", 1, false);
		setDefineINT("VIEWPORT_SIZE_X", getBugOptionINT("MainInterface__ViewportX", 40), false);
		setDefineINT("VIEWPORT_SIZE_Y", getBugOptionINT("MainInterface__ViewportY", 40), false);
		setDefineINT("VIEWPORT_FOCUS_BORDER", getBugOptionINT("MainInterface__ViewportAutoSwitchBorder", 2), false);
		m_iViewportCenterOnSelectionCenterBorder = getBugOptionINT("MainInterface__ViewportAutoCenterBorder", 5);

		// This happens after the maps load on first load, so resize existing viewports
		foreach_(const CvMap* map, m_maps)
		{
			for (int iJ = 0; iJ < map->getNumViewports(); iJ++)
			{
				map->getViewport(iJ)->resizeForMap();
			}
		}
	}
}


bool cvInternalGlobals::getBBAI_AIR_COMBAT() const
{
	return m_bBBAI_AIR_COMBAT;
}

bool cvInternalGlobals::getBBAI_HUMAN_VASSAL_WAR_BUILD() const
{
	return m_bBBAI_HUMAN_VASSAL_WAR_BUILD;
}

bool cvInternalGlobals::getTECH_DIFFUSION_ENABLE() const
{
	return m_bTECH_DIFFUSION_ENABLE;
}


void cvInternalGlobals::setXMLLogging(bool bNewVal)
{
	m_bXMLLogging = bNewVal;
}

bool cvInternalGlobals::isXMLLogging() const
{
	return m_bXMLLogging;
}


// calculate asset checksum
uint32_t cvInternalGlobals::getAssetCheckSum() const
{
	uint32_t iSum = 0;
	foreach_(const std::vector<CvInfoBase*>* infoVector, m_aInfoVectors)
	{
		foreach_(const CvInfoBase* info, *infoVector)
		{
			info->getCheckSum(iSum);
			logging::logMsg("Checksum.log", "%s : %u", info->getType(), iSum);
		}
	}
	return iSum;
}
