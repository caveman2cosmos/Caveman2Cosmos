//------------------------------------------------------------------------------------------------
//  FILE:    CvIdeaClassInfo.cpp
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
#include "CvIdeaClassInfo.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvIdeaClassInfo
//
//  DESC:   Contains info about Idea Classes
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvIdeaClassInfo::CvIdeaClassInfo() :
m_bInitialized(true)
{
}


CvIdeaClassInfo::~CvIdeaClassInfo()
{
}


bool CvIdeaClassInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	return true;
}


void CvIdeaClassInfo::copyNonDefaults(const CvIdeaClassInfo* pClassInfo)
{
	//CvInfoBase::copyNonDefaults(pClassInfo);
}


void CvIdeaClassInfo::getCheckSum(uint32_t& iSum) const
{
}

