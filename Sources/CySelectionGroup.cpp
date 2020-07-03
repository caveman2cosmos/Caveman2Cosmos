#include "CvGameCoreDLL.h"
#include "CvSelectionGroup.h"
#include "CyArea.h"
#include "CyPlot.h"
#include "CySelectionGroup.h"
#include "CyUnit.h"

//
// Python wrapper class for CvSelectionGroup
//

CySelectionGroup::CySelectionGroup() : m_pSelectionGroup(NULL) {}

CySelectionGroup::CySelectionGroup(CvSelectionGroup* pSelectionGroup) : m_pSelectionGroup(pSelectionGroup) {}


void CySelectionGroup::pushMission(MissionTypes eMission, int iData1, int iData2, int iFlags, bool bAppend, bool bManual, MissionAITypes eMissionAI, const CyPlot& kMissionAIPlot, const CyUnit& kMissionAIUnit)
{
	if (m_pSelectionGroup)
		m_pSelectionGroup->pushMission(eMission, iData1, iData2, iFlags, bAppend, bManual, eMissionAI, kMissionAIPlot.getPlot(), kMissionAIUnit.getUnit());
}

bool CySelectionGroup::canStartMission(int iMission, int iData1, int iData2, const CyPlot& kPlot, bool bTestVisible) const
{
	return m_pSelectionGroup ? m_pSelectionGroup->canStartMission(iMission, iData1, iData2, kPlot.getPlot(), bTestVisible) : false;
}

bool CySelectionGroup::isHuman() const
{
	return m_pSelectionGroup ? m_pSelectionGroup->isHuman() : false;
}

int CySelectionGroup::baseMoves() const
{
	return m_pSelectionGroup ? m_pSelectionGroup->baseMoves() : -1;
}

bool CySelectionGroup::isWaiting() const
{
	return m_pSelectionGroup ? m_pSelectionGroup->isWaiting() : false;
}

bool CySelectionGroup::isFull() const
{
	return m_pSelectionGroup ? m_pSelectionGroup->isFull() : false;
}

bool CySelectionGroup::hasMoved() const
{
	return m_pSelectionGroup ? m_pSelectionGroup->hasMoved() : false;
}

bool CySelectionGroup::canMoveInto(const CyPlot& kPlot, bool bAttack) const
{
	return m_pSelectionGroup ? m_pSelectionGroup->canMoveInto(kPlot.getPlot(), bAttack) : false;
}

bool CySelectionGroup::canMoveOrAttackInto(const CyPlot& kPlot, bool bDeclareWar) const
{
	return m_pSelectionGroup ? m_pSelectionGroup->canMoveOrAttackInto(kPlot.getPlot(), bDeclareWar) : false;
}

bool CySelectionGroup::canFight() const
{
	return m_pSelectionGroup ? m_pSelectionGroup->canFight() : false;
}

bool CySelectionGroup::isInvisible(int /*TeamTypes*/ eTeam) const
{
	return m_pSelectionGroup ? m_pSelectionGroup->isInvisible((TeamTypes) eTeam) : false;
}

CyPlot* CySelectionGroup::plot() const
{
	return m_pSelectionGroup ? new CyPlot(m_pSelectionGroup->plot()) : NULL;
}

CyArea* CySelectionGroup::area() const
{
	return m_pSelectionGroup ? new CyArea(m_pSelectionGroup->area()) : NULL;
}

bool CySelectionGroup::readyToMove(bool bAny) const
{
	return m_pSelectionGroup ? m_pSelectionGroup->readyToMove(bAny) : false;
}

int CySelectionGroup::getID() const
{
	return m_pSelectionGroup ? m_pSelectionGroup->getID() : -1;
}

int /*PlayerTypes*/ CySelectionGroup::getOwner() const
{
	return m_pSelectionGroup ? m_pSelectionGroup->getOwner() : -1;
}

int /*TeamTypes*/ CySelectionGroup::getTeam() const
{
	return m_pSelectionGroup ? m_pSelectionGroup->getTeam() : -1;
}

int /*ActivityTypes*/ CySelectionGroup::getActivityType() const
{
	return m_pSelectionGroup ? m_pSelectionGroup->getActivityType() : -1;
}

void CySelectionGroup::setActivityType(int /*ActivityTypes*/ eNewValue)
{
	if (m_pSelectionGroup)
		m_pSelectionGroup->setActivityType((ActivityTypes) eNewValue);
}

int /*AutomateTypes*/ CySelectionGroup::getAutomateType() const
{
	return m_pSelectionGroup ? m_pSelectionGroup->getAutomateType() : -1;
}

bool CySelectionGroup::isAutomated() const
{
	return m_pSelectionGroup ? m_pSelectionGroup->isAutomated() : false;
}

void CySelectionGroup::setAutomateType(int /*AutomateTypes*/ eNewValue)
{
	if (m_pSelectionGroup)
		m_pSelectionGroup->setAutomateType((AutomateTypes) eNewValue);
}

int CySelectionGroup::getNumUnits() const
{
	return m_pSelectionGroup ? m_pSelectionGroup->getNumUnits() : -1;
}

int CySelectionGroup::getLengthMissionQueue() const
{
	return m_pSelectionGroup ? m_pSelectionGroup->getLengthMissionQueue() : -1;
}

CyUnit* CySelectionGroup::getHeadUnit() const
{
	return m_pSelectionGroup ? new CyUnit(m_pSelectionGroup->getHeadUnit()) : NULL;
}

int CySelectionGroup::getMissionType(int iNode) const
{
	return m_pSelectionGroup ? m_pSelectionGroup->getMissionType(iNode) : -1;
}

int CySelectionGroup::getMissionData1(int iNode) const
{
	return m_pSelectionGroup ? m_pSelectionGroup->getMissionData1(iNode) : -1;
}
