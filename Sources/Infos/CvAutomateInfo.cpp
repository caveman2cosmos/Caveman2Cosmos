//------------------------------------------------------------------------------------------------
//  FILE:    CvAutomateInfo.cpp
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
#include "CvAutomateInfo.h"


//======================================================================================================
//					CvAutomateInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvAutomateInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvAutomateInfo::CvAutomateInfo() :
m_iCommand(NO_COMMAND),
m_iAutomate(NO_AUTOMATE),
m_bConfirmCommand(false),
m_bVisible(false)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvAutomateInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvAutomateInfo::~CvAutomateInfo()
{
}


int CvAutomateInfo::getCommand() const
{
	return m_iCommand;
}


int CvAutomateInfo::getAutomate() const
{
	return m_iAutomate;
}


bool CvAutomateInfo::getConfirmCommand() const
{
	return m_bConfirmCommand;
}


bool CvAutomateInfo::getVisible() const
{
	return m_bVisible;
}


bool CvAutomateInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalTypeEnum(m_iCommand, L"Command");
	pXML->GetOptionalTypeEnum(m_iAutomate, L"Automate");
	pXML->GetOptionalChildXmlValByName(&m_bConfirmCommand, L"bConfirmCommand");
	pXML->GetOptionalChildXmlValByName(&m_bVisible, L"bVisible");

	return true;
}


void CvAutomateInfo::copyNonDefaults(const CvAutomateInfo* pClassInfo)
{
	const bool bDefault = false;
	const int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default

	CvHotkeyInfo::copyNonDefaults(pClassInfo);

	if (getCommand() == iTextDefault) m_iCommand = pClassInfo->getCommand();
	if (getAutomate() == iTextDefault) m_iAutomate = pClassInfo->getAutomate();

	if (getConfirmCommand() == bDefault) m_bConfirmCommand = pClassInfo->getConfirmCommand();
	if (getVisible() == bDefault) m_bVisible = pClassInfo->getVisible();
}

