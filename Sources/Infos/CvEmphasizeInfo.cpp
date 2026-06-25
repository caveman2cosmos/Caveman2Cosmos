//------------------------------------------------------------------------------------------------
//  FILE:    CvEmphasizeInfo.cpp
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
#include "CvEmphasizeInfo.h"


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvEmphasizeInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvEmphasizeInfo::CvEmphasizeInfo() :
 m_bAvoidGrowth(false)
,m_bGreatPeople(false)
,m_piYieldModifiers(NULL)
,m_piCommerceModifiers(NULL)
,m_bAvoidAngryCitizens(false)
,m_bAvoidUnhealthyCitizens(false)
{ }


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvEmphasizeInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvEmphasizeInfo::~CvEmphasizeInfo()
{
	SAFE_DELETE_ARRAY(m_piYieldModifiers);
	SAFE_DELETE_ARRAY(m_piCommerceModifiers);
}


bool CvEmphasizeInfo::isAvoidGrowth() const
{
	return m_bAvoidGrowth;
}


bool CvEmphasizeInfo::isGreatPeople() const
{
	return m_bGreatPeople;
}


bool CvEmphasizeInfo::isAvoidAngryCitizens() const
{
	return m_bAvoidAngryCitizens;
}


bool CvEmphasizeInfo::isAvoidUnhealthyCitizens() const
{
	return m_bAvoidUnhealthyCitizens;
}



// Arrays

int CvEmphasizeInfo::getYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piYieldModifiers ? m_piYieldModifiers[i] : 0;
}


int CvEmphasizeInfo::getCommerceChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_piCommerceModifiers ? m_piCommerceModifiers[i] : 0;
}


//
// read from XML
//
bool CvEmphasizeInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_bAvoidGrowth, L"bAvoidGrowth");
	pXML->GetChildXmlValByName(&m_bGreatPeople, L"bGreatPeople");

	if (pXML->TryMoveToXmlFirstChild(L"YieldModifiers"))
	{
		pXML->SetYields(&m_piYieldModifiers);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldModifiers);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
	{
		pXML->SetCommerce(&m_piCommerceModifiers);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCommerceModifiers);
	}
	pXML->GetChildXmlValByName(&m_bAvoidAngryCitizens, L"bAvoidAngryCitizens");
	pXML->GetChildXmlValByName(&m_bAvoidUnhealthyCitizens, L"bAvoidUnhealthyCitizens");

	return true;
}

void CvEmphasizeInfo::copyNonDefaults(const CvEmphasizeInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (isAvoidGrowth() == bDefault) m_bAvoidGrowth = pClassInfo->isAvoidGrowth();
	if (isGreatPeople() == bDefault) m_bGreatPeople = pClassInfo->isGreatPeople();

	if (!m_piYieldModifiers) CvXMLLoadUtility::InitList(&m_piYieldModifiers, NUM_YIELD_TYPES);
	for ( int i = 0; i < NUM_YIELD_TYPES; i++ )
	{
		if ( m_piYieldModifiers[i] == iDefault )
		{
			m_piYieldModifiers[i] = pClassInfo->getYieldChange(i);
		}
	}

	if (!m_piCommerceModifiers) CvXMLLoadUtility::InitList(&m_piCommerceModifiers, NUM_COMMERCE_TYPES);
	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++ )
	{
		if ( m_piCommerceModifiers[i] == iDefault )
		{
			m_piCommerceModifiers[i] = pClassInfo->getCommerceChange(i);
		}
	}
	if (isAvoidAngryCitizens() == false) m_bAvoidAngryCitizens = pClassInfo->isAvoidAngryCitizens();
	if (isAvoidUnhealthyCitizens() == false) m_bAvoidUnhealthyCitizens = pClassInfo->isAvoidUnhealthyCitizens();
}

