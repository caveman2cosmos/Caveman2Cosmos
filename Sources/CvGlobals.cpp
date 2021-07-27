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
#include "CyGlobalContext.h"
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

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

CvGlobals gGlobalsProxy;	// for debugging
cvInternalGlobals* gGlobals = NULL;
CvDLLUtilityIFaceBase* gDLL = NULL;

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
	, m_iViewportCenterOnSelectionCenterBorder(5)
	, m_szAlternateProfilSampleName("")
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
	OutputDebugString("Initializing Internal Globals: Start\n");

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

	OutputDebugString("Initializing Internal Globals: End\n");
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
	m_aInfoArrays.clear();
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
	return m_ENABLE_VIEWPORTS;
}

bool cvInternalGlobals::getReprocessGreatWallDynamically() const
{
	return m_ENABLE_VIEWPORTS || getDefineBOOL("DYNAMIC_GREAT_WALL");
}

int cvInternalGlobals::getNumMapInfos() const
{
	return m_paMapInfo.size();
}

CvMapInfo& cvInternalGlobals::getMapInfo(MapTypes eMap) const
{
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
	return *(m_paWorldInfo[e]);
}

int cvInternalGlobals::getNumClimateInfos() const
{
	return (int)m_paClimateInfo.size();
}

CvClimateInfo& cvInternalGlobals::getClimateInfo(ClimateTypes e) const
{
	return *(m_paClimateInfo[e]);
}

int cvInternalGlobals::getNumSeaLevelInfos() const
{
	return (int)m_paSeaLevelInfo.size();
}

CvSeaLevelInfo& cvInternalGlobals::getSeaLevelInfo(SeaLevelTypes e) const
{
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
	algo::for_each(m_aInfoArrays, bind(CvInfoArray<CvInfoBase>::updateReplacements, _1));
}

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/

int cvInternalGlobals::getNumColorInfos() const
{
	return m_paColorInfo.size();
}

CvColorInfo& cvInternalGlobals::getColorInfo(ColorTypes e) const
{
	return *m_paColorInfo[e];
}


int cvInternalGlobals::getNumPlayerColorInfos() const
{
	return m_paPlayerColorInfo.size();
}

CvPlayerColorInfo& cvInternalGlobals::getPlayerColorInfo(PlayerColorTypes e) const
{
	return *m_paPlayerColorInfo[e];
}

int cvInternalGlobals::getNumAdvisorInfos() const
{
	return (int)m_paAdvisorInfo.size();
}

CvAdvisorInfo& cvInternalGlobals::getAdvisorInfo(AdvisorTypes e) const
{
	return *(m_paAdvisorInfo[e]);
}

int cvInternalGlobals::getNumRouteModelInfos() const
{
	return (int)m_paRouteModelInfo.size();
}

CvRouteModelInfo& cvInternalGlobals::getRouteModelInfo(int i) const
{
	return *(m_paRouteModelInfo[i]);
}

int cvInternalGlobals::getNumRiverModelInfos() const
{
	return (int)m_paRiverModelInfo.size();
}

CvRiverModelInfo& cvInternalGlobals::getRiverModelInfo(int i) const
{
	return *(m_paRiverModelInfo[i]);
}

int cvInternalGlobals::getNumWaterPlaneInfos() const
{
	return (int)m_paWaterPlaneInfo.size();
}

CvWaterPlaneInfo& cvInternalGlobals::getWaterPlaneInfo(int i) const
{
	return *(m_paWaterPlaneInfo[i]);
}

int cvInternalGlobals::getNumTerrainPlaneInfos() const
{
	return (int)m_paTerrainPlaneInfo.size();
}

CvTerrainPlaneInfo& cvInternalGlobals::getTerrainPlaneInfo(int i) const
{
	return *(m_paTerrainPlaneInfo[i]);
}

int cvInternalGlobals::getNumCameraOverlayInfos() const
{
	return (int)m_paCameraOverlayInfo.size();
}

CvCameraOverlayInfo& cvInternalGlobals::getCameraOverlayInfo(int i) const
{
	return *(m_paCameraOverlayInfo[i]);
}

int cvInternalGlobals::getNumAnimationPathInfos() const
{
	return (int)m_paAnimationPathInfo.size();
}

CvAnimationPathInfo& cvInternalGlobals::getAnimationPathInfo(AnimationPathTypes e) const
{
	return *(m_paAnimationPathInfo[e]);
}

int cvInternalGlobals::getNumAnimationCategoryInfos() const
{
	return (int)m_paAnimationCategoryInfo.size();
}

CvAnimationCategoryInfo& cvInternalGlobals::getAnimationCategoryInfo(AnimationCategoryTypes e) const
{
	return *(m_paAnimationCategoryInfo[e]);
}

int cvInternalGlobals::getNumEntityEventInfos() const
{
	return (int)m_paEntityEventInfo.size();
}

CvEntityEventInfo& cvInternalGlobals::getEntityEventInfo(EntityEventTypes e) const
{
	return *(m_paEntityEventInfo[e]);
}

int cvInternalGlobals::getNumEffectInfos() const
{
	return (int)m_paEffectInfo.size();
}

CvEffectInfo& cvInternalGlobals::getEffectInfo(int i) const
{
	return *(m_paEffectInfo[i]);
}


int cvInternalGlobals::getNumAttachableInfos() const
{
	return (int)m_paAttachableInfo.size();
}

CvAttachableInfo& cvInternalGlobals::getAttachableInfo(int i) const
{
	return *(m_paAttachableInfo[i]);
}

int cvInternalGlobals::getNumUnitFormationInfos() const
{
	return (int)m_paUnitFormationInfo.size();
}

CvUnitFormationInfo& cvInternalGlobals::getUnitFormationInfo(int i) const
{
	return *(m_paUnitFormationInfo[i]);
}


int cvInternalGlobals::getNumLandscapeInfos() const
{
	return (int)m_paLandscapeInfo.size();
}

CvLandscapeInfo& cvInternalGlobals::getLandscapeInfo(int iIndex) const
{
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
	return *(m_paTerrainInfo[eTerrainNum]);
}

int cvInternalGlobals::getNumBonusClassInfos() const
{
	return (int)m_paBonusClassInfo.size();
}

CvBonusClassInfo& cvInternalGlobals::getBonusClassInfo(BonusClassTypes eBonusNum) const
{
	return *(m_paBonusClassInfo[eBonusNum]);
}


int cvInternalGlobals::getNumBonusInfos() const
{
	return (int)m_paBonusInfo.size();
}

CvBonusInfo& cvInternalGlobals::getBonusInfo(BonusTypes eBonusNum) const
{
	return *(m_paBonusInfo[eBonusNum]);
}

int cvInternalGlobals::getNumFeatureInfos() const
{
	return (int)m_paFeatureInfo.size();
}

CvFeatureInfo& cvInternalGlobals::getFeatureInfo(FeatureTypes eFeatureNum) const
{
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
	return *(m_paCivilizationInfo[eCivilizationNum]);
}


int cvInternalGlobals::getNumLeaderHeadInfos() const
{
	return (int)m_paLeaderHeadInfo.size();
}

CvLeaderHeadInfo& cvInternalGlobals::getLeaderHeadInfo(LeaderHeadTypes eLeaderHeadNum) const
{
	return *(m_paLeaderHeadInfo[eLeaderHeadNum]);
}


int cvInternalGlobals::getNumTraitInfos() const
{
	return (int)m_paTraitInfo.size();
}

CvTraitInfo& cvInternalGlobals::getTraitInfo(TraitTypes eTraitNum) const
{
	return *(m_paTraitInfo[eTraitNum]);
}


int cvInternalGlobals::getNumCursorInfos() const
{
	return (int)m_paCursorInfo.size();
}

CvCursorInfo& cvInternalGlobals::getCursorInfo(CursorTypes eCursorNum) const
{
	return *(m_paCursorInfo[eCursorNum]);
}

int cvInternalGlobals::getNumThroneRoomCameras() const
{
	return (int)m_paThroneRoomCamera.size();
}

CvThroneRoomCamera& cvInternalGlobals::getThroneRoomCamera(int iIndex) const
{
	return *(m_paThroneRoomCamera[iIndex]);
}

int cvInternalGlobals::getNumThroneRoomInfos() const
{
	return (int)m_paThroneRoomInfo.size();
}

CvThroneRoomInfo& cvInternalGlobals::getThroneRoomInfo(int iIndex) const
{
	return *(m_paThroneRoomInfo[iIndex]);
}

int cvInternalGlobals::getNumThroneRoomStyleInfos() const
{
	return (int)m_paThroneRoomStyleInfo.size();
}

CvThroneRoomStyleInfo& cvInternalGlobals::getThroneRoomStyleInfo(int iIndex) const
{
	return *(m_paThroneRoomStyleInfo[iIndex]);
}

int cvInternalGlobals::getNumSlideShowInfos() const
{
	return (int)m_paSlideShowInfo.size();
}

CvSlideShowInfo& cvInternalGlobals::getSlideShowInfo(int iIndex) const
{
	return *(m_paSlideShowInfo[iIndex]);
}

int cvInternalGlobals::getNumSlideShowRandomInfos() const
{
	return (int)m_paSlideShowRandomInfo.size();
}

CvSlideShowRandomInfo& cvInternalGlobals::getSlideShowRandomInfo(int iIndex) const
{
	return *(m_paSlideShowRandomInfo[iIndex]);
}

int cvInternalGlobals::getNumWorldPickerInfos() const
{
	return (int)m_paWorldPickerInfo.size();
}

CvWorldPickerInfo& cvInternalGlobals::getWorldPickerInfo(int iIndex) const
{
	return *(m_paWorldPickerInfo[iIndex]);
}

int cvInternalGlobals::getNumSpaceShipInfos() const
{
	return (int)m_paSpaceShipInfo.size();
}

CvSpaceShipInfo& cvInternalGlobals::getSpaceShipInfo(int iIndex) const
{
	return *(m_paSpaceShipInfo[iIndex]);
}

int cvInternalGlobals::getNumUnitInfos() const
{
	return (int)m_paUnitInfo.size();
}

CvUnitInfo& cvInternalGlobals::getUnitInfo(UnitTypes eUnitNum) const
{
	return *(m_paUnitInfo[eUnitNum]);
}

int cvInternalGlobals::getNumSpawnInfos() const
{
	return (int)m_paSpawnInfo.size();
}

CvSpawnInfo& cvInternalGlobals::getSpawnInfo(SpawnTypes eSpawnNum) const
{
	return *(m_paSpawnInfo[eSpawnNum]);
}

int cvInternalGlobals::getNumSpecialUnitInfos() const
{
	return (int)m_paSpecialUnitInfo.size();
}

CvSpecialUnitInfo& cvInternalGlobals::getSpecialUnitInfo(SpecialUnitTypes eSpecialUnitNum) const
{
	return *(m_paSpecialUnitInfo[eSpecialUnitNum]);
}


int cvInternalGlobals::getNumConceptInfos() const
{
	return (int)m_paConceptInfo.size();
}

CvInfoBase& cvInternalGlobals::getConceptInfo(ConceptTypes e) const
{
	return *(m_paConceptInfo[e]);
}


int cvInternalGlobals::getNumNewConceptInfos() const
{
	return (int)m_paNewConceptInfo.size();
}

CvInfoBase& cvInternalGlobals::getNewConceptInfo(NewConceptTypes e) const
{
	return *(m_paNewConceptInfo[e]);
}


int cvInternalGlobals::getNumCityTabInfos() const
{
	return (int)m_paCityTabInfo.size();
}

CvInfoBase& cvInternalGlobals::getCityTabInfo(CityTabTypes e) const
{
	return *(m_paCityTabInfo[e]);
}


int cvInternalGlobals::getNumCalendarInfos() const
{
	return (int)m_paCalendarInfo.size();
}

CvInfoBase& cvInternalGlobals::getCalendarInfo(CalendarTypes e) const
{
	return *(m_paCalendarInfo[e]);
}


int cvInternalGlobals::getNumSeasonInfos() const
{
	return (int)m_paSeasonInfo.size();
}

CvInfoBase& cvInternalGlobals::getSeasonInfo(SeasonTypes e) const
{
	return *(m_paSeasonInfo[e]);
}


int cvInternalGlobals::getNumMonthInfos() const
{
	return (int)m_paMonthInfo.size();
}

CvInfoBase& cvInternalGlobals::getMonthInfo(MonthTypes e) const
{
	return *(m_paMonthInfo[e]);
}


int cvInternalGlobals::getNumDenialInfos() const
{
	return (int)m_paDenialInfo.size();
}

CvInfoBase& cvInternalGlobals::getDenialInfo(DenialTypes e) const
{
	return *(m_paDenialInfo[e]);
}


int cvInternalGlobals::getNumInvisibleInfos() const
{
	return (int)m_paInvisibleInfo.size();
}

CvInvisibleInfo& cvInternalGlobals::getInvisibleInfo(InvisibleTypes e) const
{
	return *(m_paInvisibleInfo[e]);
}


int cvInternalGlobals::getNumVoteSourceInfos() const
{
	return (int)m_paVoteSourceInfo.size();
}

CvVoteSourceInfo& cvInternalGlobals::getVoteSourceInfo(VoteSourceTypes e) const
{
	return *(m_paVoteSourceInfo[e]);
}


int cvInternalGlobals::getNumUnitCombatInfos() const
{
	return (int)m_paUnitCombatInfo.size();
}

CvUnitCombatInfo& cvInternalGlobals::getUnitCombatInfo(UnitCombatTypes e) const
{
	return *(m_paUnitCombatInfo[e]);
}


CvInfoBase& cvInternalGlobals::getDomainInfo(DomainTypes e) const
{
	return *(m_paDomainInfo[e]);
}

int cvInternalGlobals::getNumPromotionLineInfos() const
{
	return (int)m_paPromotionLineInfo.size();
}

CvPromotionLineInfo& cvInternalGlobals::getPromotionLineInfo(PromotionLineTypes e) const
{
	return *(m_paPromotionLineInfo[e]);
}

int cvInternalGlobals::getNumMapCategoryInfos() const
{
	return (int)m_paMapCategoryInfo.size();
}

CvMapCategoryInfo& cvInternalGlobals::getMapCategoryInfo(MapCategoryTypes e) const
{
	return *(m_paMapCategoryInfo[e]);
}

int cvInternalGlobals::getNumIdeaClassInfos() const
{
	return (int)m_paIdeaClassInfo.size();
}

CvIdeaClassInfo& cvInternalGlobals::getIdeaClassInfo(IdeaClassTypes e) const
{
	return *(m_paIdeaClassInfo[e]);
}

int cvInternalGlobals::getNumIdeaInfos() const
{
	return (int)m_paIdeaInfo.size();
}

CvIdeaInfo& cvInternalGlobals::getIdeaInfo(IdeaTypes e) const
{
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
	return *(m_paMemoryInfos[eMemoryNum]);
}


int cvInternalGlobals::getNumGameOptionInfos() const
{
	return (int)m_paGameOptionInfos.size();
}

CvGameOptionInfo& cvInternalGlobals::getGameOptionInfo(GameOptionTypes eGameOptionNum) const
{
	return *(m_paGameOptionInfos[eGameOptionNum]);
}

int cvInternalGlobals::getNumMPOptionInfos() const
{
	return (int)m_paMPOptionInfos.size();
}

CvMPOptionInfo& cvInternalGlobals::getMPOptionInfo(MultiplayerOptionTypes eMPOptionNum) const
{
	return *(m_paMPOptionInfos[eMPOptionNum]);
}

int cvInternalGlobals::getNumForceControlInfos() const
{
	return (int)m_paForceControlInfos.size();
}

CvForceControlInfo& cvInternalGlobals::getForceControlInfo(ForceControlTypes eForceControlNum) const
{
	return *(m_paForceControlInfos[eForceControlNum]);
}

CvPlayerOptionInfo& cvInternalGlobals::getPlayerOptionInfo(PlayerOptionTypes ePlayerOptionNum) const
{
	return *(m_paPlayerOptionInfos[ePlayerOptionNum]);
}

CvGraphicOptionInfo& cvInternalGlobals::getGraphicOptionInfo(GraphicOptionTypes eGraphicOptionNum) const
{
	return *(m_paGraphicOptionInfos[eGraphicOptionNum]);
}

CvYieldInfo& cvInternalGlobals::getYieldInfo(YieldTypes eYieldNum) const
{
	return *(m_paYieldInfo[eYieldNum]);
}

CvCommerceInfo& cvInternalGlobals::getCommerceInfo(CommerceTypes eCommerceNum) const
{
	return *(m_paCommerceInfo[eCommerceNum]);
}

int cvInternalGlobals::getNumRouteInfos() const
{
	return (int)m_paRouteInfo.size();
}

CvRouteInfo& cvInternalGlobals::getRouteInfo(RouteTypes eRouteNum) const
{
	return *(m_paRouteInfo[eRouteNum]);
}

int cvInternalGlobals::getNumImprovementInfos() const
{
	return (int)m_paImprovementInfo.size();
}

CvImprovementInfo& cvInternalGlobals::getImprovementInfo(ImprovementTypes eImprovementNum) const
{
	return *(m_paImprovementInfo[eImprovementNum]);
}

int cvInternalGlobals::getNumGoodyInfos() const
{
	return (int)m_paGoodyInfo.size();
}

CvGoodyInfo& cvInternalGlobals::getGoodyInfo(GoodyTypes eGoodyNum) const
{
	return *(m_paGoodyInfo[eGoodyNum]);
}

int cvInternalGlobals::getNumBuildInfos() const
{
	return (int)m_paBuildInfo.size();
}

CvBuildInfo& cvInternalGlobals::getBuildInfo(BuildTypes eBuildNum) const
{
	return *(m_paBuildInfo[eBuildNum]);
}

int cvInternalGlobals::getNumHandicapInfos() const
{
	return (int)m_paHandicapInfo.size();
}

CvHandicapInfo& cvInternalGlobals::getHandicapInfo(HandicapTypes eHandicapNum) const
{
	return *(m_paHandicapInfo[eHandicapNum]);
}

int cvInternalGlobals::getNumGameSpeedInfos() const
{
	return (int)m_paGameSpeedInfo.size();
}

CvGameSpeedInfo& cvInternalGlobals::getGameSpeedInfo(GameSpeedTypes eGameSpeedNum) const
{
	return *(m_paGameSpeedInfo[eGameSpeedNum]);
}

int cvInternalGlobals::getNumTurnTimerInfos() const
{
	return (int)m_paTurnTimerInfo.size();
}

CvTurnTimerInfo& cvInternalGlobals::getTurnTimerInfo(TurnTimerTypes eTurnTimerNum) const
{
	return *(m_paTurnTimerInfo[eTurnTimerNum]);
}

int cvInternalGlobals::getNumProcessInfos() const
{
	return (int)m_paProcessInfo.size();
}

CvProcessInfo& cvInternalGlobals::getProcessInfo(ProcessTypes e) const
{
	return *(m_paProcessInfo[e]);
}

int cvInternalGlobals::getNumVoteInfos() const
{
	return (int)m_paVoteInfo.size();
}

CvVoteInfo& cvInternalGlobals::getVoteInfo(VoteTypes e) const
{
	return *(m_paVoteInfo[e]);
}

int cvInternalGlobals::getNumProjectInfos() const
{
	return (int)m_paProjectInfo.size();
}

CvProjectInfo& cvInternalGlobals::getProjectInfo(ProjectTypes e) const
{
	return *(m_paProjectInfo[e]);
}

int cvInternalGlobals::getNumBuildingInfos() const
{
	return (int)m_paBuildingInfo.size();
}

CvBuildingInfo& cvInternalGlobals::getBuildingInfo(BuildingTypes eBuildingNum) const
{
	return *(m_paBuildingInfo[eBuildingNum]);
}

int cvInternalGlobals::getNumSpecialBuildingInfos() const
{
	return (int)m_paSpecialBuildingInfo.size();
}

CvSpecialBuildingInfo& cvInternalGlobals::getSpecialBuildingInfo(SpecialBuildingTypes eSpecialBuildingNum) const
{
	return *(m_paSpecialBuildingInfo[eSpecialBuildingNum]);
}

int cvInternalGlobals::getNumActionInfos() const
{
	return (int)m_paActionInfo.size();
}

CvActionInfo& cvInternalGlobals::getActionInfo(int i) const
{
	return *(m_paActionInfo[i]);
}

CvMissionInfo& cvInternalGlobals::getMissionInfo(MissionTypes eMissionNum) const
{
	return *(m_paMissionInfo[eMissionNum]);
}

CvControlInfo& cvInternalGlobals::getControlInfo(ControlTypes eControlNum) const
{
	return *(m_paControlInfo[eControlNum]);
}

CvCommandInfo& cvInternalGlobals::getCommandInfo(CommandTypes eCommandNum) const
{
	return *(m_paCommandInfo[eCommandNum]);
}

int cvInternalGlobals::getNumAutomateInfos() const
{
	return (int)m_paAutomateInfo.size();
}

CvAutomateInfo& cvInternalGlobals::getAutomateInfo(int iAutomateNum) const
{
	return *(m_paAutomateInfo[iAutomateNum]);
}

int cvInternalGlobals::getNumPromotionInfos() const
{
	return (int)m_paPromotionInfo.size();
}

CvPromotionInfo& cvInternalGlobals::getPromotionInfo(PromotionTypes ePromotionNum) const
{
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
	return *(m_paTechInfo[eTechNum]);
}

int cvInternalGlobals::getNumReligionInfos() const
{
	return (int)m_paReligionInfo.size();
}

CvReligionInfo& cvInternalGlobals::getReligionInfo(ReligionTypes eReligionNum) const
{
	return *(m_paReligionInfo[eReligionNum]);
}

int cvInternalGlobals::getNumCorporationInfos() const
{
	return (int)m_paCorporationInfo.size();
}

CvCorporationInfo& cvInternalGlobals::getCorporationInfo(CorporationTypes eCorporationNum) const
{
	return *(m_paCorporationInfo[eCorporationNum]);
}

int cvInternalGlobals::getNumSpecialistInfos() const
{
	return (int)m_paSpecialistInfo.size();
}

CvSpecialistInfo& cvInternalGlobals::getSpecialistInfo(SpecialistTypes eSpecialistNum) const
{
	return *(m_paSpecialistInfo[eSpecialistNum]);
}

int cvInternalGlobals::getNumCivicOptionInfos() const
{
	return (int)m_paCivicOptionInfo.size();
}

CvCivicOptionInfo& cvInternalGlobals::getCivicOptionInfo(CivicOptionTypes eCivicOptionNum) const
{
	return *(m_paCivicOptionInfo[eCivicOptionNum]);
}

int cvInternalGlobals::getNumCivicInfos() const
{
	return (int)m_paCivicInfo.size();
}

CvCivicInfo& cvInternalGlobals::getCivicInfo(CivicTypes eCivicNum) const
{
	return *(m_paCivicInfo[eCivicNum]);
}

int cvInternalGlobals::getNumDiplomacyInfos() const
{
	return (int)m_paDiplomacyInfo.size();
}

CvDiplomacyInfo& cvInternalGlobals::getDiplomacyInfo(int iDiplomacyNum) const
{
	return *(m_paDiplomacyInfo[iDiplomacyNum]);
}

int cvInternalGlobals::getNumEraInfos() const
{
	return (int)m_aEraInfo.size();
}

CvEraInfo& cvInternalGlobals::getEraInfo(EraTypes eEraNum) const
{
	return *(m_aEraInfo[eEraNum]);
}

int cvInternalGlobals::getNumHurryInfos() const
{
	return (int)m_paHurryInfo.size();
}

CvHurryInfo& cvInternalGlobals::getHurryInfo(HurryTypes eHurryNum) const
{
	return *(m_paHurryInfo[eHurryNum]);
}

int cvInternalGlobals::getNumEmphasizeInfos() const
{
	return (int)m_paEmphasizeInfo.size();
}

CvEmphasizeInfo& cvInternalGlobals::getEmphasizeInfo(EmphasizeTypes eEmphasizeNum) const
{
	return *(m_paEmphasizeInfo[eEmphasizeNum]);
}

int cvInternalGlobals::getNumUpkeepInfos() const
{
	return (int)m_paUpkeepInfo.size();
}

CvUpkeepInfo& cvInternalGlobals::getUpkeepInfo(UpkeepTypes eUpkeepNum) const
{
	return *(m_paUpkeepInfo[eUpkeepNum]);
}

int cvInternalGlobals::getNumCultureLevelInfos() const
{
	return (int)m_paCultureLevelInfo.size();
}

CvCultureLevelInfo& cvInternalGlobals::getCultureLevelInfo(CultureLevelTypes eCultureLevelNum) const
{
	return *(m_paCultureLevelInfo[eCultureLevelNum]);
}

int cvInternalGlobals::getNumVictoryInfos() const
{
	return (int)m_paVictoryInfo.size();
}

CvVictoryInfo& cvInternalGlobals::getVictoryInfo(VictoryTypes eVictoryNum) const
{
	return *(m_paVictoryInfo[eVictoryNum]);
}

int cvInternalGlobals::getNumEventTriggerInfos() const
{
	return (int)m_paEventTriggerInfo.size();
}

CvEventTriggerInfo& cvInternalGlobals::getEventTriggerInfo(EventTriggerTypes eEventTrigger) const
{
	return *(m_paEventTriggerInfo[eEventTrigger]);
}

int cvInternalGlobals::getNumEventInfos() const
{
	return (int)m_paEventInfo.size();
}

CvEventInfo& cvInternalGlobals::getEventInfo(EventTypes eEvent) const
{
	return *(m_paEventInfo[eEvent]);
}

int cvInternalGlobals::getNumEspionageMissionInfos() const
{
	return (int)m_paEspionageMissionInfo.size();
}

CvEspionageMissionInfo& cvInternalGlobals::getEspionageMissionInfo(EspionageMissionTypes eEspionageMissionNum) const
{
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
	return *(m_paPropertyInfo[ePropertyNum]);
}

int cvInternalGlobals::getNumOutcomeInfos() const
{
	return (int)m_paOutcomeInfo.size();
}

CvOutcomeInfo& cvInternalGlobals::getOutcomeInfo(OutcomeTypes eOutcomeNum) const
{
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
	algo::for_each(m_aInfoArrays,
		bind(CvInfoArray<CvInfoBase>::deleteInfos, _1)
	);
	m_aInfoArrays.clear();
	clearTypesMap();
	m_paModLoadControlVector.clear();

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

void cvInternalGlobals::addToInfoArrays(void* infoArray)
{
	m_aInfoArrays.push_back(static_cast<CvInfoArray<CvInfoBase>*>(infoArray));
}

void cvInternalGlobals::infosReset()
{
	foreach_(const CvInfoArray<CvInfoBase>* infoArray, m_aInfoArrays)
	{
		algo::for_each(*infoArray,
			bind(CvInfoBase::reset, _1)
		);
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
	FASSERT_BOUNDS(0, NUM_MAPS, eMap)

	if (eMap != CURRENT_MAP)
	{
		getMap().beforeSwitch();
		getGame().setCurrentMap(eMap);
		*CyGlobalContext::getInstance().getCyMap() = getMap();
		getMap().afterSwitch();
	}
}

CvViewport* cvInternalGlobals::getCurrentViewport() const
{
	return m_maps[CURRENT_MAP]->getCurrentViewport();
}

int	cvInternalGlobals::getViewportCenteringBorder() const
{
	return m_iViewportCenterOnSelectionCenterBorder;
}


CvMapExternal& cvInternalGlobals::getMapExternal() const
{
	const CvViewport* currentViewport = getCurrentViewport();

	FAssert(currentViewport != NULL);

	return currentViewport->getProxy();
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
		m_ENABLE_VIEWPORTS = true;

		// Push them back inot the globals so that a reload of the globals cache preserves these values
		setDefineINT("ENABLE_VIEWPORTS", 1, false);
		setDefineINT("VIEWPORT_SIZE_X", getBugOptionINT("MainInterface__ViewportX", 40), false);
		setDefineINT("VIEWPORT_SIZE_Y", getBugOptionINT("MainInterface__ViewportY", 40), false);
		setDefineINT("VIEWPORT_FOCUS_BORDER", getBugOptionINT("MainInterface__ViewportAutoSwitchBorder", 2), false);
		m_iViewportCenterOnSelectionCenterBorder = getBugOptionINT("MainInterface__ViewportAutoCenterBorder", 5);

		// This happens after the maps load on first load, so resize existing viewports
		foreach_(const CvMap* map, m_maps)
		{
			foreach_(CvViewport* viewport, map->getViewports())
			{
				viewport->resizeForMap();
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
	foreach_(const CvInfoArray<CvInfoBase>* infoArray, m_aInfoArrays)
	{
		foreach_(const CvInfoBase* info, *infoArray)
		{
			info->getCheckSum(iSum);
			logging::logMsg("Checksum.log", "%s : %u", info->getType(), iSum);
		}
	}
	return iSum;
}
