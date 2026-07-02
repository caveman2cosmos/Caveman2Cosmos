//------------------------------------------------------------------------------------------------
//  FILE:    CvSlideShowInfo.cpp
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
#include "CvSlideShowInfo.h"


//======================================================================================================
//					CvSlideShowInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSlideShowInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSlideShowInfo::CvSlideShowInfo() :
m_fStartTime(0.0f)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSlideShowInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSlideShowInfo::~CvSlideShowInfo()
{
}


bool CvSlideShowInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(m_szPath, L"Path");
	pXML->GetChildXmlValByName(m_szTransitionType, L"TransitionType");
	pXML->GetChildXmlValByName(&m_fStartTime, L"fStartTime");

	return true;
}

void CvSlideShowInfo::copyNonDefaults(const CvSlideShowInfo* pClassInfo)
{
	const float fDefault = 0.0f;
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getPath() == cDefault) m_szPath = pClassInfo->m_szPath;
	if (getTransitionType() == cDefault) m_szTransitionType = pClassInfo->m_szTransitionType;
	if (getStartTime() == fDefault) m_fStartTime = pClassInfo->m_fStartTime;
}

