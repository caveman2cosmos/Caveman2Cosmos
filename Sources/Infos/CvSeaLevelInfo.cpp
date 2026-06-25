//------------------------------------------------------------------------------------------------
//  FILE:    CvSeaLevelInfo.cpp
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
#include "CvSeaLevelInfo.h"


//======================================================================================================
//					CvSeaLevelInfo
//======================================================================================================
CvSeaLevelInfo::CvSeaLevelInfo() :
m_iSeaLevelChange(0)
{
}


CvSeaLevelInfo::~CvSeaLevelInfo()
{
}


int CvSeaLevelInfo::getSeaLevelChange() const
{
	return m_iSeaLevelChange;
}


bool CvSeaLevelInfo::read(CvXMLLoadUtility* pXML)
{

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_iSeaLevelChange, L"iSeaLevelChange");

	return true;
}


void CvSeaLevelInfo::copyNonDefaults(const CvSeaLevelInfo* pClassInfo)
{
	const int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getSeaLevelChange() == iDefault) m_iSeaLevelChange = pClassInfo->getSeaLevelChange();
}

