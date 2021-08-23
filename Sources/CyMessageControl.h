#pragma once

#ifndef _CYMESSAGECONTROL_H
#define _CYMESSAGECONTROL_H

class CyMessageControl
{
public:
	void sendPushOrder(int iCityID, int eOrder, int iData, bool bAlt, bool bShift, bool bCtrl);
	void sendDoTask(int iCity, int eTask, int iData1, int iData2, bool bOption, bool bAlt, bool bShift, bool bCtrl);
	void sendUpdateCivics(const python::list& lCivics);
	void sendResearch(int eTech, bool bShift);
	void sendPlayerOption(int /*PlayerOptionTypes*/ eOption, bool bValue);
	void sendEspionageSpendingWeightChange(int /*TeamTypes*/ eTargetTeam, int iChange);
	void sendAdvancedStartAction(int /*AdvancedStartActionTypes*/ eAction, int /*PlayerTypes*/ ePlayer, int iX, int iY, int iData, bool bAdd);
	void sendModNetMessage(int iData1, int iData2, int iData3, int iData4, int iData5);
	void sendEmpireSplit(int /*PlayerTypes*/ ePlayer, int iAreaId);

	//	Helper function to determine the first bad connection...
	int GetFirstBadConnection() const;
	int GetConnState(int iPlayer) const;
};

#endif // _CYMESSAGECONTROL_H
