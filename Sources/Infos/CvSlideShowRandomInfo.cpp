//------------------------------------------------------------------------------------------------
//  FILE:    CvSlideShowRandomInfo.cpp
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
#include "CvSlideShowRandomInfo.h"


//======================================================================================================
//					CvSlideShowRandomInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSlideShowRandomInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSlideShowRandomInfo::CvSlideShowRandomInfo()
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSlideShowRandomInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSlideShowRandomInfo::~CvSlideShowRandomInfo()
{
}


const char* CvSlideShowRandomInfo::getPath()
{
	return m_szPath;
}


bool CvSlideShowRandomInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetChildXmlValByName(m_szPath, L"Path");

	return true;
}


void CvSlideShowRandomInfo::copyNonDefaults(const CvSlideShowRandomInfo* pClassInfo)
{
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getPath() == cDefault) m_szPath = pClassInfo->m_szPath;
}

