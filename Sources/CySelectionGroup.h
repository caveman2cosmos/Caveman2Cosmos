#pragma once

#ifndef CySelectionGroup_h__
#define CySelectionGroup_h__

//
// Python wrapper class for CvSelectionGroup
//

class CvSelectionGroup;
class CyPlot;
class CyArea;
class CyUnit;

class CySelectionGroup
{
public:
	explicit CySelectionGroup(CvSelectionGroup* pSelectionGroup);		// Call from C++

	//CvSelectionGroup* getSelectionGroup() const { return m_pSelectionGroup; }	// Call from C++

	void pushMission(MissionTypes eMission, int iData1, int iData2, int iFlags, bool bAppend, bool bManual, MissionAITypes eMissionAI, const CyPlot& kMissionAIPlot, const CyUnit& kMissionAIUnit);
	bool canStartMission(int iMission, int iData1, int iData2, const CyPlot& kPlot, bool bTestVisible) const;

	bool isHuman() const;
	int baseMoves() const;
	bool isWaiting() const;
	bool isFull() const;
	bool hasMoved() const;
	bool canFight() const;
	bool isInvisible(int /*TeamTypes*/ eTeam) const;

	CyPlot* plot() const;
	CyArea* area() const;

	bool readyToMove(bool bAny) const;

	int getID() const;
	int /*PlayerTypes*/ getOwner() const;
	int /*TeamTypes*/ getTeam() const;
	int /*ActivityTypes*/ getActivityType() const;
	void setActivityType(int /*ActivityTypes*/ eNewValue);
	int /*AutomateTypes*/ getAutomateType() const;
	bool isAutomated() const;
	void setAutomateType(int /*AutomateTypes*/ eNewValue);

	int getNumUnits() const;
	int getLengthMissionQueue() const;
	int getMissionType(int iNode) const;
	int getMissionData1(int iNode) const;
	CyUnit* getHeadUnit() const;

protected:
	CvSelectionGroup* m_pSelectionGroup;
};

DECLARE_PY_WRAPPER(CySelectionGroup, CvSelectionGroup*);

#endif // CySelectionGroup_h__