//------------------------------------------------------------------------------------------------
//  FILE:    CvCommerceInfo.cpp
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
#include "CvCommerceInfo.h"


//======================================================================================================
//					CvCommerceInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCommerceInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCommerceInfo::CvCommerceInfo() :
m_iChar(0),
m_iInitialPercent(0),
m_iInitialHappiness(0),
m_iAIWeightPercent(0),
m_bFlexiblePercent(false)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCommerceInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCommerceInfo::~CvCommerceInfo()
{
}


int CvCommerceInfo::getChar() const
{
	return m_iChar;
}


void CvCommerceInfo::setChar(int i)
{
	m_iChar = i;
}


int CvCommerceInfo::getInitialPercent() const
{
	return m_iInitialPercent;
}


int CvCommerceInfo::getInitialHappiness() const
{
	return m_iInitialHappiness;
}


int CvCommerceInfo::getAIWeightPercent() const
{
	return m_iAIWeightPercent;
}


bool CvCommerceInfo::isFlexiblePercent() const
{
	return m_bFlexiblePercent;
}


bool CvCommerceInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_iInitialPercent, L"iInitialPercent");
	pXML->GetChildXmlValByName(&m_iInitialHappiness, L"iInitialHappiness");
	pXML->GetChildXmlValByName(&m_iAIWeightPercent, L"iAIWeightPercent");
	pXML->GetChildXmlValByName(&m_bFlexiblePercent, L"bFlexiblePercent");

	return true;
}


void CvCommerceInfo::copyNonDefaults(const CvCommerceInfo* pClassInfo)
{
	const bool bDefault = false;
	const int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getInitialPercent() == iDefault) m_iInitialPercent = pClassInfo->getInitialPercent();
	if (getInitialHappiness() == iDefault) m_iInitialHappiness = pClassInfo->getInitialHappiness();
	if (getAIWeightPercent() == iDefault) m_iAIWeightPercent = pClassInfo->getAIWeightPercent();
	if (isFlexiblePercent() == bDefault) m_bFlexiblePercent = pClassInfo->isFlexiblePercent();

}


void CvCommerceInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iInitialPercent);
	CheckSum(iSum, m_iInitialHappiness);
	CheckSum(iSum, m_iAIWeightPercent);

	CheckSum(iSum, m_bFlexiblePercent);
}

