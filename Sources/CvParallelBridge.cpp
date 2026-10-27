#include "CvGameCoreDLL.h"
#include "CvParallelBridge.h"
#include <cstdio>

CvParallelBridge::CvParallelBridge()
    : m_hModule(NULL)
    , m_pfnGetVersion(NULL)
    , m_pfnGetMemoryUsageBytes(NULL)
    , m_pfnInit(NULL)
    , m_pfnBeginTurn(NULL)
    , m_pfnConfigureMap(NULL)
    , m_pfnUpdatePlotMovementProps(NULL)
    , m_pfnRebuildMissionTargetCache(NULL)
    , m_pfnCountPlotTargetMissionAIs(NULL)
    , m_pfnRebuildPlotDangerCache(NULL)
    , m_pfnGetPlotDanger(NULL)
    , m_pfnSolveProperties(NULL)
    , m_pfnBatchAssignCityPlots(NULL)
    , m_pfnScoreAndPruneCityProduction(NULL)
    , m_pfnEvaluateAndResolveUnitBatch(NULL)
{
}

CvParallelBridge::~CvParallelBridge() {
    if (m_hModule) {
        FreeLibrary(m_hModule);
        m_hModule = NULL;
    }
}

CvParallelBridge& CvParallelBridge::getInstance() {
    static CvParallelBridge s_instance;
    return s_instance;
}

bool CvParallelBridge::init() {
    if (m_hModule) return true;

    // Try loading C2CParallel.dll
    m_hModule = LoadLibraryA("C2CParallel.dll");
    if (!m_hModule) {
        m_hModule = LoadLibraryA("Assets\\C2CParallel.dll");
    }
    if (!m_hModule) {
        m_hModule = LoadLibraryA("Mods\\Caveman2Cosmos\\Assets\\C2CParallel.dll");
    }

    if (!m_hModule) {
        OutputDebugStringA("CvParallelBridge: C2CParallel.dll not found; running in serial fallback.\n");
        return false;
    }

    m_pfnGetVersion = (uint32_t(__cdecl*)(void))GetProcAddress(m_hModule, "C2CParallel_GetVersion");
    m_pfnGetMemoryUsageBytes = (uint32_t(__cdecl*)(void))GetProcAddress(m_hModule, "C2CParallel_GetMemoryUsageBytes");
    m_pfnInit = (int32_t(__cdecl*)(int32_t, int32_t))GetProcAddress(m_hModule, "C2CParallel_Init");
    m_pfnBeginTurn = (void(__cdecl*)(int32_t))GetProcAddress(m_hModule, "C2CParallel_BeginTurn");
    m_pfnConfigureMap = (void(__cdecl*)(const void*))GetProcAddress(m_hModule, "C2CParallel_ConfigureMap");
    m_pfnUpdatePlotMovementProps = (void(__cdecl*)(const void*, int32_t))GetProcAddress(m_hModule, "C2CParallel_UpdatePlotMovementProps");
    m_pfnRebuildMissionTargetCache = (void(__cdecl*)(int32_t, const void*, int32_t, int32_t))GetProcAddress(m_hModule, "C2CParallel_RebuildMissionTargetCache");
    m_pfnCountPlotTargetMissionAIs = (int32_t(__cdecl*)(int32_t, int32_t, int32_t, int32_t, int32_t))GetProcAddress(m_hModule, "C2CParallel_CountPlotTargetMissionAIs");
    m_pfnRebuildPlotDangerCache = (void(__cdecl*)(int32_t, const void*, int32_t))GetProcAddress(m_hModule, "C2CParallel_RebuildPlotDangerCache");
    m_pfnGetPlotDanger = (int32_t(__cdecl*)(int32_t, int32_t, int32_t, int32_t))GetProcAddress(m_hModule, "C2CParallel_GetPlotDanger");
    m_pfnSolveProperties = (int32_t(__cdecl*)(int32_t, int32_t, const int32_t*, const void*, int32_t, const void*, int32_t, const void*, int32_t, int32_t*))GetProcAddress(m_hModule, "C2CParallel_SolveProperties");
    m_pfnBatchAssignCityPlots = (int32_t(__cdecl*)(const void*, int32_t, void*))GetProcAddress(m_hModule, "C2CParallel_BatchAssignCityPlots");
    m_pfnScoreAndPruneCityProduction = (int32_t(__cdecl*)(int32_t, int32_t, int32_t, const void*, int32_t, int32_t*, int32_t*, int32_t*))GetProcAddress(m_hModule, "C2CParallel_ScoreAndPruneCityProduction");
    m_pfnEvaluateAndResolveUnitBatch = (int32_t(__cdecl*)(int32_t, int32_t, const void*, int32_t, const void*, int32_t, void*))GetProcAddress(m_hModule, "C2CParallel_EvaluateAndResolveUnitBatch");

    if (m_pfnInit) {
        int32_t threads = m_pfnInit(16, 1);
        char buf[128];
        sprintf(buf, "CvParallelBridge: Initialized C2CParallel.dll with %d threads.\n", threads);
        OutputDebugStringA(buf);
    }

    return true;
}

void CvParallelBridge::beginTurn(int turnNumber) {
    if (m_pfnBeginTurn) {
        m_pfnBeginTurn(turnNumber);
    }
}

void CvParallelBridge::configureMap(int mapId, int width, int height, bool wrapX, bool wrapY) {
    if (m_pfnConfigureMap) {
        C2CMapConfigBridge cfg;
        cfg.mapId = mapId;
        cfg.width = width;
        cfg.height = height;
        cfg.wrapX = wrapX ? 1 : 0;
        cfg.wrapY = wrapY ? 1 : 0;
        cfg.reserved = 0;
        m_pfnConfigureMap(&cfg);
    }
}

void CvParallelBridge::updatePlotMovementProps(const C2CPlotMovementPropsBridge* props, int numPlots) {
    if (m_pfnUpdatePlotMovementProps && props && numPlots > 0) {
        m_pfnUpdatePlotMovementProps(props, numPlots);
    }
}

void CvParallelBridge::rebuildMissionTargetCache(int playerId, const C2CGroupMissionEntryBridge* entries, int numEntries, int excludeGroupId) {
    if (m_pfnRebuildMissionTargetCache) {
        m_pfnRebuildMissionTargetCache(playerId, entries, numEntries, excludeGroupId);
    }
}

int CvParallelBridge::countPlotTargetMissionAIs(int playerId, int plotX, int plotY, int missionAi, int range) {
    if (m_pfnCountPlotTargetMissionAIs) {
        return m_pfnCountPlotTargetMissionAIs(playerId, plotX, plotY, missionAi, range);
    }
    return 0;
}

void CvParallelBridge::rebuildPlotDangerCache(int playerId, const C2CDangerUnitEntryBridge* enemyUnits, int numEnemyUnits) {
    if (m_pfnRebuildPlotDangerCache) {
        m_pfnRebuildPlotDangerCache(playerId, enemyUnits, numEnemyUnits);
    }
}

int CvParallelBridge::getPlotDanger(int playerId, int plotX, int plotY, int range) {
    if (m_pfnGetPlotDanger) {
        return m_pfnGetPlotDanger(playerId, plotX, plotY, range);
    }
    return 0;
}

int CvParallelBridge::solveProperties(int numObjects, int numProperties, const int32_t* initialValues,
                                      const C2CPropertySourceOpBridge* sources, int numSources,
                                      const C2CPropertyInteractionOpBridge* interactions, int numInteractions,
                                      const C2CPropertyEdgeOpBridge* propagators, int numPropagators,
                                      int32_t* outTotalDeltas) {
    if (m_pfnSolveProperties) {
        return m_pfnSolveProperties(numObjects, numProperties, initialValues,
                                    sources, numSources,
                                    interactions, numInteractions,
                                    propagators, numPropagators,
                                    outTotalDeltas);
    }
    return 0;
}

int CvParallelBridge::batchAssignCityPlots(const C2CCityPlotAssignRequestBridge* requests, int numCities, C2CCityPlotAssignResultBridge* outResults) {
    if (m_pfnBatchAssignCityPlots && requests && outResults && numCities > 0) {
        return m_pfnBatchAssignCityPlots(requests, numCities, outResults);
    }
    return 0;
}

int CvParallelBridge::scoreAndPruneCityProduction(int turnNumber, int playerId, int cityId,
                                                 const C2CCityProdCandidateBridge* candidates, int numCandidates,
                                                 int* outBestItemType, int* outBestIsUnit, int* outBestScore) {
    if (m_pfnScoreAndPruneCityProduction && candidates && numCandidates > 0) {
        int32_t item = -1, isUnit = 0, score = 0;
        int32_t res = m_pfnScoreAndPruneCityProduction(turnNumber, playerId, cityId, candidates, numCandidates, &item, &isUnit, &score);
        if (outBestItemType) *outBestItemType = item;
        if (outBestIsUnit) *outBestIsUnit = isUnit;
        if (outBestScore) *outBestScore = score;
        return res;
    }
    return 0;
}

int CvParallelBridge::evaluateAndResolveUnitBatch(int turnNumber, int playerId,
                                                  const C2CUnitDryRunRequestBridge* unitRequests, int numUnits,
                                                  const C2CUnitTargetCandidateBridge* allCandidates, int totalCandidates,
                                                  C2CUnitDryRunDecisionBridge* outDecisions) {
    if (m_pfnEvaluateAndResolveUnitBatch && unitRequests && allCandidates && outDecisions && numUnits > 0) {
        return m_pfnEvaluateAndResolveUnitBatch(turnNumber, playerId, unitRequests, numUnits, allCandidates, totalCandidates, outDecisions);
    }
    return 0;
}

uint32_t CvParallelBridge::getVersion() const {
    return m_pfnGetVersion ? m_pfnGetVersion() : 0;
}

uint32_t CvParallelBridge::getMemoryUsage() const {
    return m_pfnGetMemoryUsageBytes ? m_pfnGetMemoryUsageBytes() : 0;
}
