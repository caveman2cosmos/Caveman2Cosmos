//------------------------------------------------------------------------------------------------
//  FILE:    CvAnimationCategoryInfo.cpp
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
#include "CvAnimationCategoryInfo.h"


//======================================================================================================
//					CvAnimationCategoryInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvAnimationCategoryInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvAnimationCategoryInfo::CvAnimationCategoryInfo()
{
	m_kCategory.second = -7540; // invalid.
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvAnimationCategoryInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvAnimationCategoryInfo::~CvAnimationCategoryInfo()
{
}


int CvAnimationCategoryInfo::getCategoryBaseID( )
{
	return m_kCategory.first;
}


int CvAnimationCategoryInfo::getCategoryDefaultTo( )
{
	if ( m_kCategory.second < -1 )
	{
		// CvXMLLoadUtility *pXML = new CvXMLLoadUtility();
		m_kCategory.second = CvXMLLoadUtility::GetInfoClass( m_szDefaultTo);
	}
	return (int)m_kCategory.second;
}


bool CvAnimationCategoryInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	int		iBaseID;						// Temporary
	pXML->GetChildXmlValByName( m_szDefaultTo, L"DefaultTo");
	pXML->GetChildXmlValByName( &iBaseID, L"BaseID");
	m_kCategory.first = iBaseID;
	return true;
}


void CvAnimationCategoryInfo::copyNonDefaults(const CvAnimationCategoryInfo* pClassInfo)
{
	const int iDefault = 0;
	const int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getCategoryBaseID() == iDefault) m_kCategory.first = pClassInfo->m_kCategory.first;
	if (getCategoryDefaultTo() == iTextDefault) m_kCategory.second = pClassInfo->m_kCategory.second;
}

