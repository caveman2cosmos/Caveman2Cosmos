//------------------------------------------------------------------------------------------------
//  FILE:    CvAdvisorInfo.cpp
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
#include "CvAdvisorInfo.h"


//======================================================================================================
//					CvAdvisorInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvAdvisorInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvAdvisorInfo::CvAdvisorInfo()
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvAdvisorInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvAdvisorInfo::~CvAdvisorInfo()
{
}


const char* CvAdvisorInfo::getTexture() const
{
	return m_szTexture;
}


int CvAdvisorInfo::getNumCodes() const
{
	return m_vctEnableDisableCodes.size();
}


int CvAdvisorInfo::getEnableCode(uint32_t uiCode) const
{
	FASSERT_BOUNDS(0, m_vctEnableDisableCodes.size(), uiCode);
	return m_vctEnableDisableCodes[uiCode].first;
}


int CvAdvisorInfo::getDisableCode(uint32_t uiCode) const
{
	FASSERT_BOUNDS(0, m_vctEnableDisableCodes.size(), uiCode);
	return m_vctEnableDisableCodes[uiCode].second;
}


bool CvAdvisorInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szTexture, L"Texture", "");

	if(pXML->TryMoveToXmlFirstChild())
	{
		while (pXML->TryMoveToXmlNextSibling(L"EventCodes"))
		{
			int iEnableCode, iDisableCode;
			pXML->GetChildXmlValByName(&iEnableCode, L"iEnableCode");
			pXML->GetChildXmlValByName(&iDisableCode, L"iDisableCode");
			m_vctEnableDisableCodes.push_back(std::make_pair(iEnableCode, iDisableCode));
		}
		pXML->MoveToXmlParent();
	}

	return true;
}


void CvAdvisorInfo::copyNonDefaults(const CvAdvisorInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getTexture() == cDefault) m_szTexture = pClassInfo->getTexture();

	if ( getNumCodes() == 0 )  //Only copy old values if the new doesn't hold a tag
	{
		for ( int iI = 0; iI < pClassInfo->getNumCodes(); iI++)
		{
			m_vctEnableDisableCodes.push_back(std::make_pair(pClassInfo->getEnableCode(iI), pClassInfo->getDisableCode(iI)));
		}
	}
}

