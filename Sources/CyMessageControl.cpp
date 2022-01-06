#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvMessageControl.h"
#include "CyMessageControl.h"

void CyMessageControl::sendPushOrder(int iCityID, int eOrder, int iData, bool bAlt, bool bShift, bool bCtrl)
{
	CvMessageControl::getInstance().sendPushOrder(iCityID, (OrderTypes) eOrder, iData, bAlt, bShift, bCtrl);
}

void CyMessageControl::sendDoTask(int iCity, int eTask, int iData1, int iData2, bool bOption, bool bAlt, bool bShift, bool bCtrl)
{
	CvMessageControl::getInstance().sendDoTask(iCity, (TaskTypes) eTask, iData1, iData2, bOption, bAlt, bShift, bCtrl);
}

void CyMessageControl::sendUpdateCivics(const python::list& lCivics)
{
	std::vector<CivicTypes> v;
	python::container_utils::extend_container(v, lCivics);
	CvMessageControl::getInstance().sendUpdateCivics(v);
}

void CyMessageControl::sendEmpireSplit(int /*PlayerTypes*/ ePlayer, int iAreaId)
{
	CvMessageControl::getInstance().sendEmpireSplit((PlayerTypes) ePlayer, iAreaId);
}

void CyMessageControl::sendResearch(int eTech, bool bShift)
{
	CvMessageControl::getInstance().sendResearch((TechTypes)eTech, -1, bShift);
}

void CyMessageControl::sendPlayerOption(int /*PlayerOptionTypes*/ eOption, bool bValue)
{
	gDLL->sendPlayerOption((PlayerOptionTypes) eOption, bValue);
}

void CyMessageControl::sendEspionageSpendingWeightChange(int /*TeamTypes*/ eTargetTeam, int iChange)
{
	CvMessageControl::getInstance().sendEspionageSpendingWeightChange((TeamTypes) eTargetTeam, iChange);
}

void CyMessageControl::sendAdvancedStartAction(int /*AdvancedStartActionTypes*/ eAction, int /*PlayerTypes*/ ePlayer, int iX, int iY, int iData, bool bAdd)
{
	CvMessageControl::getInstance().sendAdvancedStartAction((AdvancedStartActionTypes) eAction, (PlayerTypes) ePlayer, iX, iY, iData, bAdd);
}

void CyMessageControl::sendModNetMessage(int iData1, int iData2, int iData3, int iData4, int iData5)
{
	CvMessageControl::getInstance().sendModNetMessage(iData1, iData2, iData3, iData4, iData5);
}

//
// return true if succeeded
//
int CyMessageControl::GetFirstBadConnection() const
{
	return gDLL->getFirstBadConnection();
}

int CyMessageControl::GetConnState(int iPlayer) const
{
	return gDLL->getConnState((PlayerTypes)iPlayer);
}
