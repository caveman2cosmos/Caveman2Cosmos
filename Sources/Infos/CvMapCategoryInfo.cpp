//------------------------------------------------------------------------------------------------
//  FILE:    CvMapCategoryInfo.cpp
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
#include "CvMapCategoryInfo.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvMapCategoryInfo
//
//  DESC:   Contains info about Map Categories
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvMapCategoryInfo::CvMapCategoryInfo()
{
}


CvMapCategoryInfo::~CvMapCategoryInfo()
{
}


bool CvMapCategoryInfo::read(CvXMLLoadUtility* pXML)
{
	CvInfoBase::read(pXML);

	return true;
}


void CvMapCategoryInfo::copyNonDefaults(const CvMapCategoryInfo* pClassInfo)
{
}


void CvMapCategoryInfo::getCheckSum(uint32_t& iSum) const
{
}

