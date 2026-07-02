//------------------------------------------------------------------------------------------------
//  FILE:    CvSpecialBuildingInfo.cpp
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
#include "CvSpecialBuildingInfo.h"


//======================================================================================================
//					CvSpecialBuildingInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSpecialBuildingInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialBuildingInfo::CvSpecialBuildingInfo() :
	m_iObsoleteTech(NO_TECH),
	m_iTechPrereq(NO_TECH),
	m_iTechPrereqAnyone(NO_TECH),
	m_iMaxPlayerInstances(-1),
	m_bValid(false)
{}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSpecialBuildingInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialBuildingInfo::~CvSpecialBuildingInfo() { }


TechTypes CvSpecialBuildingInfo::getObsoleteTech() const
{
	return m_iObsoleteTech;
}


int CvSpecialBuildingInfo::getTechPrereqAnyone() const
{
	return m_iTechPrereqAnyone;
}


int CvSpecialBuildingInfo::getMaxPlayerInstances() const
{
	return m_iMaxPlayerInstances;
}


bool CvSpecialBuildingInfo::isValid() const
{
	return m_bValid;
}


bool CvSpecialBuildingInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetOptionalChildXmlValByName(szTextVal, L"ObsoleteTech");
	m_iObsoleteTech = static_cast<TechTypes>(pXML->GetInfoClass(szTextVal));

	pXML->GetOptionalTypeEnum(m_iTechPrereq, L"TechPrereq");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TechPrereqAnyone");
	m_iTechPrereqAnyone = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iMaxPlayerInstances, L"iMaxPlayerInstances", -1);

	pXML->GetOptionalChildXmlValByName(&m_bValid, L"bValid");

	return true;
}


void CvSpecialBuildingInfo::copyNonDefaults(const CvSpecialBuildingInfo* pClassInfo)
{
	CvInfoBase::copyNonDefaults(pClassInfo);

	const bool bDefault = false;
	const int iDefault = 0;

	if (getObsoleteTech() == iDefault) m_iObsoleteTech = pClassInfo->getObsoleteTech();
	if (getTechPrereq() == iDefault) m_iTechPrereq = pClassInfo->getTechPrereq();
	if (getTechPrereqAnyone() == iDefault) m_iTechPrereqAnyone = pClassInfo->getTechPrereqAnyone();
	if (getMaxPlayerInstances() == -1) m_iMaxPlayerInstances = pClassInfo->getMaxPlayerInstances();

	if (isValid() == bDefault) m_bValid = pClassInfo->isValid();
}


void CvSpecialBuildingInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iObsoleteTech);
	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iTechPrereqAnyone);
	CheckSum(iSum, m_iMaxPlayerInstances);

	CheckSum(iSum, m_bValid);
}

