#pragma once

#ifndef CySelectionGroup_h__
#define CySelectionGroup_h__

struct MissionData;

class CyPlot;
class CyArea;
class CyUnit;
class CvSelectionGroup;

//
// Python wrapper class for CySelectionGroup
//
class CySelectionGroup
{
public:
	CySelectionGroup();
	explicit CySelectionGroup(CvSelectionGroup* pSelectionGroup);		// Call from C++
	const CvSelectionGroup* getSelectionGroup() const { return m_pSelectionGroup;	}	// Call from C++
	bool isNone() const { return m_pSelectionGroup == NULL; }

	void pushMission(MissionTypes eMission, int iData1, int iData2, int iFlags, bool bAppend, bool bManual, MissionAITypes eMissionAI, CyPlot* pMissionAIPlot, CyUnit* pMissionAIUnit);
	void pushMoveToMission(int iX, int iY);
	void popMission();
	CyPlot* lastMissionPlot();
	bool canStartMission(int iMission, int iData1, int iData2, CyPlot* pPlot, bool bTestVisible);

	bool canDoInterfaceMode(InterfaceModeTypes eInterfaceMode);
	bool canDoInterfaceModeAt(InterfaceModeTypes eInterfaceMode, CyPlot* pPlot);

	bool canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible);

	bool isHuman();
	int baseMoves();	
	bool isWaiting();
	bool isFull();
	bool hasCargo();
	bool canAllMove();
	bool canAnyMove();
	bool hasMoved();
	bool canEnterTerritory(int /*TeamTypes*/ eTeam, bool bIgnoreRightOfPassage);
	bool canEnterArea(int /*TeamTypes*/ eTeam, CyArea* pArea, bool bIgnoreRightOfPassage);
	bool canMoveInto(CyPlot* pPlot, bool bAttack);
	bool canMoveOrAttackInto(CyPlot* pPlot, bool bDeclareWar);
	bool canMoveThrough(CyPlot* pPlot);
	bool canFight();
	bool canDefend();	
	bool alwaysInvisible();	
	bool isInvisible(int /*TeamTypes*/ eTeam);	
	int countNumUnitAIType(UnitAITypes eUnitAI);
	bool hasWorker();
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      11/30/08                                jdog5000      */
/*                                                                                              */
/* General AI                                                                                   */
/************************************************************************************************/
	bool isStranded();
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	bool at(int iX, int iY);
	bool atPlot(CyPlot* pPlot);
	CyPlot* plot();
	CyArea* area();
	int /*RouteTypes*/ getBestBuildRoute(CyPlot* pPlot, BuildTypes* peBestBuild);

	bool isAmphibPlot(CyPlot* pPlot);

	bool readyToSelect(bool bAny);
	bool readyToMove(bool bAny);
	bool readyToAuto();	
	int getID();
	int /*PlayerTypes*/ getOwner();
	int /*TeamTypes*/ getTeam();
	int /*ActivityTypes*/ getActivityType();
	void setActivityType(int /*ActivityTypes*/ eNewValue);
	int /*AutomateTypes*/ getAutomateType();
	bool isAutomated();
	void setAutomateType(int /*AutomateTypes*/ eNewValue);
	CyPlot* getPathFirstPlot();
	CyPlot* getPathEndTurnPlot();
	bool generatePath(CyPlot* pFromPlot, CyPlot* pToPlot, int iFlags, bool bReuse, int* piPathTurns);
	void resetPath();
	int getNumUnits();
	void clearMissionQueue();
	int getLengthMissionQueue();
	int getMissionType( int iNode );
	int getMissionData1( int iNode );
	int getMissionData2( int iNode );
	MissionData* getMissionFromQueue(int iIndex);
	CyUnit* getHeadUnit();
	CyUnit* getUnitAt(int index);

protected:
	CvSelectionGroup* m_pSelectionGroup;
};

DECLARE_PY_WRAPPER(CySelectionGroup, CvSelectionGroup*);

#endif // CySelectionGroup_h__