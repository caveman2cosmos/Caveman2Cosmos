//------------------------------------------------------------------------------------------------
//  FILE:    CvCursorInfo.cpp
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
#include "CvCursorInfo.h"



//======================================================================================================
//					CvCursorInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCursorInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCursorInfo::CvCursorInfo()
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCursorInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCursorInfo::~CvCursorInfo()
{
}


const char* CvCursorInfo::getPath()
{
	return m_szPath;
}


bool CvCursorInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"CursorPath");
	m_szPath = szTextVal;

	return true;
}


void CvCursorInfo::copyNonDefaults(const CvCursorInfo* pClassInfo)
{
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getPath() == cDefault) m_szPath = pClassInfo->m_szPath;
}

