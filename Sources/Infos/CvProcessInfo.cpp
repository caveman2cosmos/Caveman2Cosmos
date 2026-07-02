//------------------------------------------------------------------------------------------------
//  FILE:    CvProcessInfo.cpp
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
#include "CvProcessInfo.h"


//======================================================================================================
//					CvProcessInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvProcessInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvProcessInfo::CvProcessInfo() :
m_iTechPrereq(NO_TECH),
m_paiProductionToCommerceModifier(NULL)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvProcessInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvProcessInfo::~CvProcessInfo()
{
	SAFE_DELETE_ARRAY(m_paiProductionToCommerceModifier);
}


int CvProcessInfo::getProductionToCommerceModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_paiProductionToCommerceModifier ? m_paiProductionToCommerceModifier[i] : 0;
}


bool CvProcessInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalTypeEnum(m_iTechPrereq, L"TechPrereq");

	if (pXML->TryMoveToXmlFirstChild(L"ProductionToCommerceModifiers"))
	{
		pXML->SetCommerce(&m_paiProductionToCommerceModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiProductionToCommerceModifier);
	}

	return true;
}


void CvProcessInfo::copyNonDefaults(const CvProcessInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getTechPrereq() == iTextDefault) m_iTechPrereq = pClassInfo->getTechPrereq();

	if (!m_paiProductionToCommerceModifier) CvXMLLoadUtility::InitList(&m_paiProductionToCommerceModifier, NUM_COMMERCE_TYPES);
	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++ )
	{
		if ( m_paiProductionToCommerceModifier[i] == iDefault )
		{
			m_paiProductionToCommerceModifier[i] = pClassInfo->getProductionToCommerceModifier(i);
		}
	}
}


void CvProcessInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iTechPrereq);

	// Arrays

	CheckSum(iSum, m_paiProductionToCommerceModifier, NUM_COMMERCE_TYPES);
}

