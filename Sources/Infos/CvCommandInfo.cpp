//------------------------------------------------------------------------------------------------
//  FILE:    CvCommandInfo.cpp
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
#include "CvCommandInfo.h"


//======================================================================================================
//					CvCommandInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCommandInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCommandInfo::CvCommandInfo() :
m_iAutomate(NO_AUTOMATE),
m_bConfirmCommand(false),
m_bVisible(false),
m_bAll(false)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCommandInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCommandInfo::~CvCommandInfo()
{
}


int CvCommandInfo::getAutomate() const
{
	return m_iAutomate;
}


bool CvCommandInfo::getConfirmCommand() const
{
	return m_bConfirmCommand;
}


bool CvCommandInfo::getVisible() const
{
	return m_bVisible;
}


bool CvCommandInfo::getAll() const
{
	return m_bAll;
}


bool CvCommandInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalTypeEnum(m_iAutomate, L"Automate");
	pXML->GetOptionalChildXmlValByName(&m_bConfirmCommand, L"bConfirmCommand");
	pXML->GetOptionalChildXmlValByName(&m_bVisible, L"bVisible");
	pXML->GetOptionalChildXmlValByName(&m_bAll, L"bAll");

	return true;
}


void CvCommandInfo::copyNonDefaults(const CvCommandInfo* pClassInfo)
{
	const bool bDefault = false;
	const int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default

	CvHotkeyInfo::copyNonDefaults(pClassInfo);

	if (getAutomate() == iTextDefault) m_iAutomate = pClassInfo->getAutomate();
	if (getConfirmCommand() == bDefault) m_bConfirmCommand = pClassInfo->getConfirmCommand();
	if (getVisible() == bDefault) m_bVisible = pClassInfo->getVisible();
	if (getAll() == bDefault) m_bAll = pClassInfo->getAll();
}

