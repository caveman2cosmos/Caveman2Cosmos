//------------------------------------------------------------------------------------------------
//  FILE:    CvInvisibleInfo.cpp
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
#include "CvInvisibleInfo.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvInvisibleInfo
//
//  DESC:   Contains info about Invisibles
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvInvisibleInfo::CvInvisibleInfo() :
 m_iChar(0)
,m_iFontButtonIndex(0)
,m_bIntrinsic(false)
{
}


CvInvisibleInfo::~CvInvisibleInfo()
{
}


bool CvInvisibleInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iFontButtonIndex, L"FontButtonIndex");
	pXML->GetOptionalChildXmlValByName(&m_bIntrinsic, L"bIntrinsic");

	return true;
}


void CvInvisibleInfo::copyNonDefaults(const CvInvisibleInfo* pClassInfo)
{
	const int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (m_iFontButtonIndex == iDefault) m_iFontButtonIndex = pClassInfo->getFontButtonIndex();
	if (m_bIntrinsic == iDefault) m_bIntrinsic = pClassInfo->isIntrinsic();
}


void CvInvisibleInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iChar);
	CheckSum(iSum, m_iFontButtonIndex);
	CheckSum(iSum, m_bIntrinsic);
}


void CvInvisibleInfo::setChar(int i)
{
	m_iChar = i;
}

