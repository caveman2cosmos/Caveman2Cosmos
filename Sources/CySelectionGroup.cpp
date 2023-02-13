#include "CvGameCoreDLL.h"
#include "CvSelectionGroup.h"
#include "CyArea.h"
#include "CyPlot.h"
#include "CySelectionGroup.h"
#include "CyUnit.h"

//
// Python wrapper class for CvSelectionGroup
//

CySelectionGroup::CySelectionGroup(CvSelectionGroup* pSelectionGroup) : m_pSelectionGroup(pSelectionGroup)
{
	FAssert(m_pSelectionGroup != NULL);
}

void CySelectionGroup::pushMission(MissionTypes eMission, int iData1, int iData2, int iFlags, bool bAppend, bool bManual, MissionAITypes eMissionAI, const CyPlot& kMissionAIPlot, const CyUnit& kMissionAIUnit)
{
	m_pSelectionGroup->pushMission(eMission, iData1, iData2, iFlags, bAppend, bManual, eMissionAI, kMissionAIPlot.getPlot(), kMissionAIUnit.getUnit());
}

bool CySelectionGroup::canStartMission(int iMission, int iData1, int iData2, const CyPlot& kPlot, bool bTestVisible) const
{
	return m_pSelectionGroup->canStartMission(iMission, iData1, iData2, kPlot.getPlot(), bTestVisible);
}

bool CySelectionGroup::isHuman() const
{
	return m_pSelectionGroup->isHuman();
}

int CySelectionGroup::baseMoves() const
{
	return m_pSelectionGroup->baseMoves();
}

bool CySelectionGroup::isWaiting() const
{
	return m_pSelectionGroup->isWaiting();
}

bool CySelectionGroup::isFull() const
{
	return m_pSelectionGroup->isFull();
}

bool CySelectionGroup::hasMoved() const
{
	return m_pSelectionGroup->hasMoved();
}

bool CySelectionGroup::canFight() const
{
	return m_pSelectionGroup->canFight();
}

bool CySelectionGroup::isInvisible(int /*TeamTypes*/ eTeam) const
{
	return m_pSelectionGroup->isInvisible((TeamTypes) eTeam);
}

CyPlot* CySelectionGroup::plot() const
{
	return new CyPlot(m_pSelectionGroup->plot());
}

CyArea* CySelectionGroup::area() const
{
	return new CyArea(m_pSelectionGroup->area());
}

bool CySelectionGroup::readyToMove(bool bAny) const
{
	return m_pSelectionGroup->readyToMove(bAny);
}

int CySelectionGroup::getID() const
{
	return m_pSelectionGroup->getID();
}

int /*PlayerTypes*/ CySelectionGroup::getOwner() const
{
	return m_pSelectionGroup->getOwner();
}

int /*TeamTypes*/ CySelectionGroup::getTeam() const
{
	return m_pSelectionGroup->getTeam();
}

int /*ActivityTypes*/ CySelectionGroup::getActivityType() const
{
	return m_pSelectionGroup->getActivityType();
}

void CySelectionGroup::setActivityType(int /*ActivityTypes*/ eNewValue)
{
	m_pSelectionGroup->setActivityType((ActivityTypes) eNewValue);
}

int /*AutomateTypes*/ CySelectionGroup::getAutomateType() const
{
	return m_pSelectionGroup->getAutomateType();
}

bool CySelectionGroup::isAutomated() const
{
	return m_pSelectionGroup->isAutomated();
}

void CySelectionGroup::setAutomateType(int /*AutomateTypes*/ eNewValue)
{
	m_pSelectionGroup->setAutomateType((AutomateTypes) eNewValue);
}

int CySelectionGroup::getNumUnits() const
{
	return m_pSelectionGroup->getNumUnits();
}

int CySelectionGroup::getLengthMissionQueue() const
{
	return m_pSelectionGroup->getLengthMissionQueue();
}

CyUnit* CySelectionGroup::getHeadUnit() const
{
	return new CyUnit(m_pSelectionGroup->getHeadUnit());
}

int CySelectionGroup::getMissionType(int iNode) const
{
	return m_pSelectionGroup->getMissionType(iNode);
}

int CySelectionGroup::getMissionData1(int iNode) const
{
	return m_pSelectionGroup->getMissionData1(iNode);
}
