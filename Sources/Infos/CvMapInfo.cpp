//------------------------------------------------------------------------------------------------
//  FILE:    CvMapInfo.cpp
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
#include "CvMapInfo.h"


//======================================================================================================
//					CvMapInfo
//======================================================================================================

CvMapInfo::CvMapInfo()
	: m_iGridWidth(0)
	, m_iGridHeight(0)
	, m_iWrapX(-1)
	, m_iWrapY(-1)
	, m_bStartRevealed(false)
	, m_szInitialWBMap(NULL)
	, m_szMapScript(NULL)
{
}


CvMapInfo::~CvMapInfo()
{
}


bool CvMapInfo::read(CvXMLLoadUtility* pXML)
{
	CvHotkeyInfo::read(pXML);

	pXML->GetOptionalChildXmlValByName(&m_iGridWidth, L"iGridWidth");
	pXML->GetOptionalChildXmlValByName(&m_iGridHeight, L"iGridHeight");
	pXML->GetOptionalChildXmlValByName(&m_iWrapX, L"bWrapX");
	pXML->GetOptionalChildXmlValByName(&m_iWrapY, L"bWrapY");
	pXML->GetOptionalChildXmlValByName(&m_bStartRevealed, L"bStartRevealed");
	pXML->GetOptionalChildXmlValByName(m_szInitialWBMap, L"InitialWBMap");
	pXML->GetOptionalChildXmlValByName(m_szMapScript, L"MapScript");

	return true;
}

