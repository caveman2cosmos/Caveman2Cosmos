//------------------------------------------------------------------------------------------------
//  FILE:    CvCategoryInfo.cpp
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
#include "CvCategoryInfo.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCategoryInfo
//
//  DESC:   Contains info about Categories
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvCategoryInfo::CvCategoryInfo() :
	m_bInitialized(true)
{
}


CvCategoryInfo::~CvCategoryInfo()
{

	GC.removeDelayedResolutionVector(m_aiCategories);
}


bool CvCategoryInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->SetOptionalVectorWithDelayedResolution(m_aiCategories, L"Categories");

	return true;
}


void CvCategoryInfo::copyNonDefaults(const CvCategoryInfo* pClassInfo)
{
	const int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	GC.copyNonDefaultDelayedResolutionVector(m_aiCategories, pClassInfo->m_aiCategories);
}


void CvCategoryInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSumC(iSum, m_aiCategories);
}


int CvCategoryInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}

int CvCategoryInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}

bool CvCategoryInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}

