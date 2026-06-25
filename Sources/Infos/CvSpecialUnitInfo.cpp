//------------------------------------------------------------------------------------------------
//  FILE:    CvSpecialUnitInfo.cpp
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
#include "CvSpecialUnitInfo.h"



//======================================================================================================
//					CvSpecialUnitInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSpecialUnitInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialUnitInfo::CvSpecialUnitInfo() :
	m_bValid(false),
	m_bCityLoad(false),
	m_bSMLoadSame(false),
	m_iCombatPercent(0),
	m_iWithdrawalChange(0),
	m_iPursuitChange(0),
	m_pbCarrierUnitAITypes(NULL)
{ }


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSpecialUnitInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialUnitInfo::~CvSpecialUnitInfo()
{
	SAFE_DELETE_ARRAY(m_pbCarrierUnitAITypes);
}


bool CvSpecialUnitInfo::isValid() const
{
	return m_bValid;
}


bool CvSpecialUnitInfo::isCityLoad() const
{
	return m_bCityLoad;
}


bool CvSpecialUnitInfo::isSMLoadSame() const
{
	return m_bSMLoadSame;
}


int CvSpecialUnitInfo::getCombatPercent() const
{
	return m_iCombatPercent;
}


int CvSpecialUnitInfo::getWithdrawalChange() const
{
	return m_iWithdrawalChange;
}


int CvSpecialUnitInfo::getPursuitChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_FIGHT_OR_FLIGHT))
	{
		return 0;
	}
	return m_iPursuitChange;
}


// Arrays

bool CvSpecialUnitInfo::isCarrierUnitAIType(int i) const
{
	FASSERT_BOUNDS(0, NUM_UNITAI_TYPES, i);
	return m_pbCarrierUnitAITypes ? m_pbCarrierUnitAITypes[i] : false;
}


bool CvSpecialUnitInfo::read(CvXMLLoadUtility* pXML)
{

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetOptionalChildXmlValByName(&m_bValid, L"bValid");
	pXML->GetOptionalChildXmlValByName(&m_bCityLoad, L"bCityLoad");
	pXML->GetOptionalChildXmlValByName(&m_bSMLoadSame, L"bSMLoadSame");
	pXML->GetOptionalChildXmlValByName(&m_iCombatPercent, L"iCombatPercent");
	pXML->GetOptionalChildXmlValByName(&m_iWithdrawalChange, L"iWithdrawalChange");
	pXML->GetOptionalChildXmlValByName(&m_iPursuitChange, L"iPursuitChange");

	pXML->SetVariableListTagPair(&m_pbCarrierUnitAITypes, L"CarrierUnitAITypes", NUM_UNITAI_TYPES);

	return true;
}


void CvSpecialUnitInfo::copyNonDefaults(const CvSpecialUnitInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const bool bDefault = false;
	const int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (isValid() == bDefault) m_bValid = pClassInfo->isValid();
	if (isCityLoad() == bDefault) m_bCityLoad = pClassInfo->isCityLoad();
	if (isSMLoadSame() == bDefault) m_bSMLoadSame = pClassInfo->isSMLoadSame();
	if (getCombatPercent() == iDefault) m_iCombatPercent = pClassInfo->getCombatPercent();
	if (getWithdrawalChange() == iDefault) m_iWithdrawalChange = pClassInfo->getWithdrawalChange();
	if (m_iPursuitChange == iDefault) m_iPursuitChange = pClassInfo->m_iPursuitChange;

	for ( int i = 0; i < NUM_UNITAI_TYPES; i++ )
	{
		if ( isCarrierUnitAIType(i) == bDefault && pClassInfo->isCarrierUnitAIType(i) != bDefault)
		{
			if ( NULL == m_pbCarrierUnitAITypes )
			{
				CvXMLLoadUtility::InitList(&m_pbCarrierUnitAITypes,NUM_UNITAI_TYPES,bDefault);
			}
			m_pbCarrierUnitAITypes[i] = pClassInfo->isCarrierUnitAIType(i);
		}
	}
}


void CvSpecialUnitInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_bValid);
	CheckSum(iSum, m_bSMLoadSame);
	CheckSum(iSum, m_iCombatPercent);
	CheckSum(iSum, m_iWithdrawalChange);
	CheckSum(iSum, m_iPursuitChange);

	CheckSum(iSum, m_pbCarrierUnitAITypes, NUM_UNITAI_TYPES);
}

