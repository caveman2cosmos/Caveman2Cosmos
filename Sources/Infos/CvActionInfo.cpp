//------------------------------------------------------------------------------------------------
//  FILE:    CvActionInfo.cpp
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvArtFileMgr.h"
#include "CvBuildingInfo.h"
#include "CvHeritageInfo.h"
#include "CvGameAI.h"
#include "CvGameTextMgr.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvInfoUtil.h"
#include "CvPlayerAI.h"
#include "CvPython.h"
#include "CvXMLLoadUtility.h"
#include "CvXMLLoadUtilityModTools.h"
#include "CheckSum.h"
#include "CvImprovementInfo.h"
#include "CvBonusInfo.h"
#include "CvActionInfo.h"


//======================================================================================================
//					CvActionInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvActionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvActionInfo::CvActionInfo() :
m_iOriginalIndex(-1),
m_eSubType(NO_ACTIONSUBTYPE)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvActionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvActionInfo::~CvActionInfo()
{
}


int CvActionInfo::getMissionData() const
{
	switch (m_eSubType)
	{
		case ACTIONSUBTYPE_BUILD:
		case ACTIONSUBTYPE_RELIGION:
		case ACTIONSUBTYPE_CORPORATION:
		case ACTIONSUBTYPE_SPECIALIST:
		case ACTIONSUBTYPE_BUILDING:
		case ACTIONSUBTYPE_HERITAGE:
			return m_iOriginalIndex;
	}
	return -1;
}


int CvActionInfo::getCommandData() const
{
	switch (m_eSubType)
	{
		case ACTIONSUBTYPE_PROMOTION:
		case ACTIONSUBTYPE_UNIT:
			return m_iOriginalIndex;

		case ACTIONSUBTYPE_COMMAND:
			return GC.getCommandInfo((CommandTypes)m_iOriginalIndex).getAutomate();

		case ACTIONSUBTYPE_AUTOMATE:
			return GC.getAutomateInfo(m_iOriginalIndex).getAutomate();
	}
	return -1;
}


int CvActionInfo::getAutomateType() const
{
	switch (m_eSubType)
	{
		case ACTIONSUBTYPE_COMMAND:
			return GC.getCommandInfo((CommandTypes)m_iOriginalIndex).getAutomate();

		case ACTIONSUBTYPE_AUTOMATE:
			return GC.getAutomateInfo(m_iOriginalIndex).getAutomate();
	}
	return NO_AUTOMATE;
}


int CvActionInfo::getInterfaceModeType() const
{
	switch (m_eSubType)
	{
		case ACTIONSUBTYPE_INTERFACEMODE:
			return m_iOriginalIndex;
	}
	return NO_INTERFACEMODE;
}


int CvActionInfo::getMissionType() const
{
	switch (m_eSubType)
	{
		case ACTIONSUBTYPE_BUILD:
			return GC.getBuildInfo((BuildTypes)m_iOriginalIndex).getMissionType();

		case ACTIONSUBTYPE_RELIGION:
			return GC.getReligionInfo((ReligionTypes)m_iOriginalIndex).getMissionType();

		case ACTIONSUBTYPE_CORPORATION:
			return GC.getCorporationInfo((CorporationTypes)m_iOriginalIndex).getMissionType();

		case ACTIONSUBTYPE_SPECIALIST:
			return GC.getSpecialistInfo((SpecialistTypes)m_iOriginalIndex).getMissionType();

		case ACTIONSUBTYPE_BUILDING:
			return GC.getBuildingInfo((BuildingTypes)m_iOriginalIndex).getMissionType();

		case ACTIONSUBTYPE_HERITAGE:
			return GC.getHeritageInfo((HeritageTypes)m_iOriginalIndex).getMissionType();

		case ACTIONSUBTYPE_MISSION:
			return m_iOriginalIndex;
	}
	return NO_MISSION;
}


int CvActionInfo::getCommandType() const
{
	switch (m_eSubType)
	{
		case ACTIONSUBTYPE_COMMAND:
			return m_iOriginalIndex;

		case ACTIONSUBTYPE_PROMOTION:
			return GC.getPromotionInfo((PromotionTypes)m_iOriginalIndex).getCommandType();

		case ACTIONSUBTYPE_UNIT:
			return GC.getUnitInfo((UnitTypes)m_iOriginalIndex).getCommandType();

		case ACTIONSUBTYPE_AUTOMATE:
			return GC.getAutomateInfo(m_iOriginalIndex).getCommand();
	}
	return NO_COMMAND;
}


int CvActionInfo::getControlType() const
{
	switch (m_eSubType)
	{
		case ACTIONSUBTYPE_CONTROL:
			return m_iOriginalIndex;
	}
	return -1;
}


int CvActionInfo::getOriginalIndex() const
{
	return m_iOriginalIndex;
}


bool CvActionInfo::isConfirmCommand() const
{
	switch (m_eSubType)
	{
		case ACTIONSUBTYPE_COMMAND:
			return GC.getCommandInfo((CommandTypes)m_iOriginalIndex).getConfirmCommand();

		case ACTIONSUBTYPE_AUTOMATE:
			return GC.getAutomateInfo(m_iOriginalIndex).getConfirmCommand();
	}
	return false;
}


bool CvActionInfo::isVisible() const
{
	switch (m_eSubType)
	{
		case ACTIONSUBTYPE_CONTROL:
			return false;

		case ACTIONSUBTYPE_COMMAND:
			return GC.getCommandInfo((CommandTypes)m_iOriginalIndex).getVisible();

		case ACTIONSUBTYPE_AUTOMATE:
			return GC.getAutomateInfo(m_iOriginalIndex).getVisible();

		case ACTIONSUBTYPE_MISSION:
			return GC.getMissionInfo((MissionTypes)m_iOriginalIndex).getVisible();

		case ACTIONSUBTYPE_INTERFACEMODE:
			return GC.getInterfaceModeInfo((InterfaceModeTypes)m_iOriginalIndex).getVisible();
	}
	return true;
}


ActionSubTypes CvActionInfo::getSubType() const
{
	return m_eSubType;
}


const CvHotkeyInfo* CvActionInfo::getHotkeyInfo() const
{
	switch (m_eSubType)
	{
		case ACTIONSUBTYPE_INTERFACEMODE:
			return &GC.getInterfaceModeInfo((InterfaceModeTypes)getOriginalIndex());

		case ACTIONSUBTYPE_COMMAND:
			return &GC.getCommandInfo((CommandTypes)getOriginalIndex());

		case ACTIONSUBTYPE_BUILD:
			return &GC.getBuildInfo((BuildTypes)getOriginalIndex());

		case ACTIONSUBTYPE_PROMOTION:
			return &GC.getPromotionInfo((PromotionTypes)getOriginalIndex());

		case ACTIONSUBTYPE_UNIT:
			return &GC.getUnitInfo((UnitTypes)getOriginalIndex());

		case ACTIONSUBTYPE_RELIGION:
			return &GC.getReligionInfo((ReligionTypes)getOriginalIndex());

		case ACTIONSUBTYPE_CORPORATION:
			return &GC.getCorporationInfo((CorporationTypes)getOriginalIndex());

		case ACTIONSUBTYPE_SPECIALIST:
			return &GC.getSpecialistInfo((SpecialistTypes)getOriginalIndex());

		case ACTIONSUBTYPE_BUILDING:
			return &GC.getBuildingInfo((BuildingTypes)getOriginalIndex());

		case ACTIONSUBTYPE_HERITAGE:
			return &GC.getHeritageInfo((HeritageTypes)getOriginalIndex());

		case ACTIONSUBTYPE_CONTROL:
			return &GC.getControlInfo((ControlTypes)getOriginalIndex());

		case ACTIONSUBTYPE_AUTOMATE:
			return &GC.getAutomateInfo(getOriginalIndex());

		case ACTIONSUBTYPE_MISSION:
			return &GC.getMissionInfo((MissionTypes)getOriginalIndex());
	}
	FErrorMsg("Unknown Action Subtype in CvActionInfo::getHotkeyInfo");
	return NULL;
}


const char* CvActionInfo::getType() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getType() : NULL;
}


const wchar_t* CvActionInfo::getDescription() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getDescription() : L"";
}


const wchar_t* CvActionInfo::getCivilopedia() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getCivilopedia() : L"";
}


const wchar_t* CvActionInfo::getHelp() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getHelp() : L"";
}


const wchar_t* CvActionInfo::getStrategy() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getStrategy() : L"";
}


const char* CvActionInfo::getButton() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getButton() : NULL;
}


const wchar_t* CvActionInfo::getTextKeyWide() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getTextKeyWide() : NULL;
}


int CvActionInfo::getActionInfoIndex() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getActionInfoIndex() : -1;
}


int CvActionInfo::getHotKeyVal() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getHotKeyVal() : -1;
}


int CvActionInfo::getHotKeyPriority() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getHotKeyPriority() : -1;
}


int CvActionInfo::getHotKeyValAlt() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getHotKeyValAlt() : -1;
}


int CvActionInfo::getHotKeyPriorityAlt() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getHotKeyPriorityAlt() : -1;
}


int CvActionInfo::getOrderPriority() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getOrderPriority() : 0;
}


bool CvActionInfo::isAltDown() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->isAltDown() : false;
}


bool CvActionInfo::isShiftDown() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->isShiftDown() : false;
}


bool CvActionInfo::isCtrlDown() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->isCtrlDown() : false;
}


bool CvActionInfo::isAltDownAlt() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->isAltDownAlt() : false;
}


bool CvActionInfo::isShiftDownAlt() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->isShiftDownAlt() : false;
}


bool CvActionInfo::isCtrlDownAlt() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->isCtrlDownAlt() : false;
}


const char* CvActionInfo::getHotKey() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getHotKey() : NULL;
}


std::wstring CvActionInfo::getHotKeyDescription() const
{
	const CvHotkeyInfo* pHotkeyInfo = getHotkeyInfo();
	return pHotkeyInfo ? pHotkeyInfo->getHotKeyDescription() : L"";
}

