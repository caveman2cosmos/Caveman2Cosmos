#pragma once

// CvDLLButtonPopup.h

#ifndef CIV4_DLL_BUTTON_POPUP_H
#define CIV4_DLL_BUTTON_POPUP_H


#define AMBUSH_FLAG 1


class CvPopup;
class CvPopupInfo;
class PopupReturn;

class CvDLLButtonPopup
{
public:
	CvDLLButtonPopup();
	virtual ~CvDLLButtonPopup();

	DllExport static CvDLLButtonPopup& getInstance();
	DllExport static void freeInstance();

	virtual void OnOkClicked(CvPopup* pPopup, PopupReturn *pPopupReturn, CvPopupInfo &info);
	virtual void OnAltExecute(CvPopup& popup, const PopupReturn& popupReturn, CvPopupInfo &info);
	virtual void OnFocus(CvPopup* pPopup, CvPopupInfo &info);

	DllExport bool launchButtonPopup(CvPopup* pPopup, CvPopupInfo &info);

private:

	static CvDLLButtonPopup* m_pInst;

	bool launchTextPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchProductionPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchChangeReligionPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchChooseElectionPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchDiploVotePopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchRazeCityPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchDisbandCityPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchChooseTechPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchChangeCivicsPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchAlarmPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchDeclareWarMovePopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchConfirmCommandPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchLoadUnitPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchLeadUnitPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchDoEspionagePopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchDoEspionageTargetPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchMainMenuPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchConfirmMenu(CvPopup *pPopup, CvPopupInfo &info);
	bool launchPythonScreen(CvPopup* pPopup, CvPopupInfo &info);
	bool launchPythonPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchDetailsPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchAdminPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchAdminPasswordPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchCancelDeal(CvPopup* pPopup, CvPopupInfo &info);
	bool launchExtendedGamePopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchDiplomacyPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchAddBuddyPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchForcedDisconnectPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchPitbossDisconnectPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchKickedPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchVassalDemandTributePopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchVassalGrantTributePopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchEventPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchFreeColonyPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchLaunchPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchFoundReligionPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchGetSaveFormatPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchModifierRecalculationPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchNameListPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchGoToCityPopup(CvPopup* pPopup, CvPopupInfo &info);

	bool launchSelectShadowUnitPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchSelectDiscoveryTechPopup(CvPopup* pPopup, CvPopupInfo &info);

	bool launchChooseBuildUpPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchChooseTraitPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchChooseTraitPopupNegative(CvPopup* pPopup, CvPopupInfo &info);
	bool launchSelectMergeUnitPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchConfirmSplitUnitPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchImprovementUpgradeOptionsPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchSelectArrestUnitPopup(CvPopup* pPopup, CvPopupInfo &info);
	bool launchConfirmAmbushPopup(CvPopup* pPopup, CvPopupInfo &info);
};

#endif
