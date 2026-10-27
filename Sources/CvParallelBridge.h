#pragma once

#ifndef CV_PARALLEL_BRIDGE_H
#define CV_PARALLEL_BRIDGE_H

#include "CvGameCoreDLL.h"

#pragma pack(push, 4)

struct C2CMapConfigBridge {
    int32_t mapId;
    int32_t width;
    int32_t height;
    uint8_t wrapX;
    uint8_t wrapY;
    uint16_t reserved;
};

struct C2CGroupMissionEntryBridge {
    int32_t groupId;
    int32_t headUnitAi;
    int32_t missionAi;
    int16_t targetX;
    int16_t targetY;
    int16_t currX;
    int16_t currY;
};

struct C2CDangerUnitEntryBridge {
    int16_t x;
    int16_t y;
    int16_t dangerRange;
    uint8_t canAttack;
    uint8_t isEnemy;
};

struct C2CPlotMovementPropsBridge {
    uint16_t baseMoveCost;
    uint16_t routeMoveCost;
    uint8_t domainMask;
    uint8_t isImpassable;
    uint8_t isDanger;
    uint8_t territoryTeam;
};

struct C2CPropertySourceOpBridge {
    int32_t objectId;
    int32_t propertyId;
    int32_t currentAmount;
    int32_t amountPerTurn;
    int32_t limit;
    int32_t opType;
    int32_t param1;
    int32_t param2;
};

struct C2CPropertyInteractionOpBridge {
    int32_t objectId;
    int32_t sourcePropId;
    int32_t targetPropId;
    int32_t currentSource;
    int32_t currentTarget;
    int32_t opType;
    int32_t param1;
    int32_t param2;
};

struct C2CPropertyEdgeOpBridge {
    int32_t sourceObjectId;
    int32_t targetObjectId;
    int32_t propertyId;
    int32_t currentSource;
    int32_t currentTarget;
    int32_t opType;
    int32_t param1;
    int32_t targetCount;
};

struct C2CCityPlotCandidateBridge {
    int16_t plotSlot;
    int16_t food;
    int16_t production;
    int16_t commerce;
    uint8_t canWork;
    uint8_t forcedWork;
};

struct C2CCityPlotAssignRequestBridge {
    int32_t cityId;
    int32_t populationToAssign;
    int16_t foodWeight;
    int16_t prodWeight;
    int16_t commWeight;
    int16_t foodTargetDelta;
    C2CCityPlotCandidateBridge plots[37];
};

struct C2CCityPlotAssignResultBridge {
    int32_t cityId;
    uint8_t workedMask[37];
    int32_t assignedCount;
    int32_t totalScore;
};

struct C2CCityProdCandidateBridge {
    int32_t itemType;
    uint8_t isUnit;
    uint8_t isDominatedOrObsolete;
    int16_t turnsToBuild;
    int32_t baseValue;
    int32_t yieldScore;
    int32_t militaryScore;
    int32_t propertyResolveScore;
};

enum C2CUnitUpdateStatusBridge {
    C2C_UNIT_COMMIT_MOVE_BRIDGE = 1,
    C2C_UNIT_NO_OP_BRIDGE = 0,
    C2C_UNIT_NEEDS_SERIAL_RETRY_BRIDGE = -1
};

struct C2CUnitTargetCandidateBridge {
    int16_t targetX;
    int16_t targetY;
    int32_t baseValue;
    int32_t missionType;
    int32_t missionAi;
    uint8_t exclusiveTarget;
    uint8_t changesPlotOwnership;
};

struct C2CUnitDryRunRequestBridge {
    int32_t groupId;
    int32_t headUnitId;
    int16_t startX;
    int16_t startY;
    int32_t maxMovesPerTurn;
    uint8_t domainType;
    uint8_t ignoreDanger;
    int32_t candidateOffset;
    int32_t candidateCount;
};

struct C2CUnitDryRunDecisionBridge {
    int32_t groupId;
    int32_t status;
    int32_t chosenCandidateIdx;
    int16_t targetX;
    int16_t targetY;
    int16_t firstStepX;
    int16_t firstStepY;
    int32_t missionType;
    int32_t missionAi;
    int32_t pathTurns;
    int32_t netScore;
};

#pragma pack(pop)

class CvParallelBridge {
public:
    static CvParallelBridge& getInstance();

    bool init();
    bool isAvailable() const { return m_hModule != NULL; }

    void beginTurn(int turnNumber);
    void configureMap(int mapId, int width, int height, bool wrapX, bool wrapY);
    void updatePlotMovementProps(const C2CPlotMovementPropsBridge* props, int numPlots);

    // Spatial caches
    void rebuildMissionTargetCache(int playerId, const C2CGroupMissionEntryBridge* entries, int numEntries, int excludeGroupId);
    int countPlotTargetMissionAIs(int playerId, int plotX, int plotY, int missionAi, int range);
    void rebuildPlotDangerCache(int playerId, const C2CDangerUnitEntryBridge* enemyUnits, int numEnemyUnits);
    int getPlotDanger(int playerId, int plotX, int plotY, int range);

    // Property solver
    int solveProperties(int numObjects, int numProperties, const int32_t* initialValues,
                        const C2CPropertySourceOpBridge* sources, int numSources,
                        const C2CPropertyInteractionOpBridge* interactions, int numInteractions,
                        const C2CPropertyEdgeOpBridge* propagators, int numPropagators,
                        int32_t* outTotalDeltas);

    // City AI
    int batchAssignCityPlots(const C2CCityPlotAssignRequestBridge* requests, int numCities, C2CCityPlotAssignResultBridge* outResults);
    int scoreAndPruneCityProduction(int turnNumber, int playerId, int cityId,
                                   const C2CCityProdCandidateBridge* candidates, int numCandidates,
                                   int* outBestItemType, int* outBestIsUnit, int* outBestScore);

    // Unit AI
    int evaluateAndResolveUnitBatch(int turnNumber, int playerId,
                                    const C2CUnitDryRunRequestBridge* unitRequests, int numUnits,
                                    const C2CUnitTargetCandidateBridge* allCandidates, int totalCandidates,
                                    C2CUnitDryRunDecisionBridge* outDecisions);

    uint32_t getVersion() const;
    uint32_t getMemoryUsage() const;

private:
    CvParallelBridge();
    ~CvParallelBridge();

    HMODULE m_hModule;

    // Function pointers
    uint32_t (__cdecl *m_pfnGetVersion)(void);
    uint32_t (__cdecl *m_pfnGetMemoryUsageBytes)(void);
    int32_t  (__cdecl *m_pfnInit)(int32_t, int32_t);
    void     (__cdecl *m_pfnBeginTurn)(int32_t);
    void     (__cdecl *m_pfnConfigureMap)(const void*);
    void     (__cdecl *m_pfnUpdatePlotMovementProps)(const void*, int32_t);
    void     (__cdecl *m_pfnRebuildMissionTargetCache)(int32_t, const void*, int32_t, int32_t);
    int32_t  (__cdecl *m_pfnCountPlotTargetMissionAIs)(int32_t, int32_t, int32_t, int32_t, int32_t);
    void     (__cdecl *m_pfnRebuildPlotDangerCache)(int32_t, const void*, int32_t);
    int32_t  (__cdecl *m_pfnGetPlotDanger)(int32_t, int32_t, int32_t, int32_t);
    int32_t  (__cdecl *m_pfnSolveProperties)(int32_t, int32_t, const int32_t*, const void*, int32_t, const void*, int32_t, const void*, int32_t, int32_t*);
    int32_t  (__cdecl *m_pfnBatchAssignCityPlots)(const void*, int32_t, void*);
    int32_t  (__cdecl *m_pfnScoreAndPruneCityProduction)(int32_t, int32_t, int32_t, const void*, int32_t, int32_t*, int32_t*, int32_t*);
    int32_t  (__cdecl *m_pfnEvaluateAndResolveUnitBatch)(int32_t, int32_t, const void*, int32_t, const void*, int32_t, void*);
};

#endif // CV_PARALLEL_BRIDGE_H
