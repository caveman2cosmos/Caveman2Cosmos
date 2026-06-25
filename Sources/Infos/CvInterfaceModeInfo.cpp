//------------------------------------------------------------------------------------------------
//  FILE:    CvInterfaceModeInfo.cpp
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
#include "CvInterfaceModeInfo.h"


//======================================================================================================
//					CvInterfaceModeInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvInterfaceModeInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvInterfaceModeInfo::CvInterfaceModeInfo() :
m_iCursorIndex(NO_CURSOR),
m_iMissionType(NO_MISSION),
m_bVisible(false),
m_bGotoPlot(false),
m_bHighlightPlot(false),
m_bSelectType(false),
m_bSelectAll(false)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvInterfaceModeInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvInterfaceModeInfo::~CvInterfaceModeInfo()
{
}


int CvInterfaceModeInfo::getCursorIndex() const
{
	return m_iCursorIndex;
}


int CvInterfaceModeInfo::getMissionType() const
{
	return m_iMissionType;
}


bool CvInterfaceModeInfo::getVisible() const
{
	return m_bVisible;
}


bool CvInterfaceModeInfo::getGotoPlot() const
{
	return m_bGotoPlot;
}


bool CvInterfaceModeInfo::getHighlightPlot() const
{
	return m_bHighlightPlot;
}


bool CvInterfaceModeInfo::getSelectType() const
{
	return m_bSelectType;
}


bool CvInterfaceModeInfo::getSelectAll() const
{
	return m_bSelectAll;
}


bool CvInterfaceModeInfo::read(CvXMLLoadUtility* pXML)
{

	CvString szTextVal;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CursorType");
	m_iCursorIndex = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Mission");
	m_iMissionType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_bVisible, L"bVisible");
	pXML->GetOptionalChildXmlValByName(&m_bGotoPlot, L"bGotoPlot");
	pXML->GetOptionalChildXmlValByName(&m_bHighlightPlot, L"bHighlightPlot");
	pXML->GetOptionalChildXmlValByName(&m_bSelectType, L"bSelectType");
	pXML->GetOptionalChildXmlValByName(&m_bSelectAll, L"bSelectAll");

	return true;
}


void CvInterfaceModeInfo::copyNonDefaults(const CvInterfaceModeInfo* pClassInfo)
{
	const bool bDefault = false;
	const int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default

	CvHotkeyInfo::copyNonDefaults(pClassInfo);

	if (getCursorIndex() == iTextDefault) m_iCursorIndex = pClassInfo->getCursorIndex();
	if (getMissionType() == iTextDefault) m_iMissionType = pClassInfo->getMissionType();

	if (getVisible() == bDefault) m_bVisible = pClassInfo->getVisible();
	if (getGotoPlot() == bDefault) m_bGotoPlot = pClassInfo->getGotoPlot();
	if (getHighlightPlot() == bDefault) m_bHighlightPlot = pClassInfo->getHighlightPlot();
	if (getSelectType() == bDefault) m_bSelectType = pClassInfo->getSelectType();
	if (getSelectAll() == bDefault) m_bSelectAll = pClassInfo->getSelectAll();
}

