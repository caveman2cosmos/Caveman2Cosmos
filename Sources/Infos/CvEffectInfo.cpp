//------------------------------------------------------------------------------------------------
//  FILE:    CvEffectInfo.cpp
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
#include "CvEffectInfo.h"



//////////////////////////////////////////////////////////////////////////
//
//	CvEffectInfo			Misc\CIV4EffectInfos.xml
//
//

CvEffectInfo::CvEffectInfo() :
 m_fUpdateRate(0.0f)
,m_bProjectile(false)
,m_bSticky(false)
,m_fProjectileSpeed(0.0f)
,m_fProjectileArc(0.0f)
{ }


CvEffectInfo::~CvEffectInfo() {}


bool CvEffectInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	CvScalableInfo::read(pXML);

	pXML->GetChildXmlValByName(szTextVal, L"Path");
	setPath(szTextVal);

	pXML->GetChildXmlValByName(&m_fUpdateRate, L"fUpdateRate" );

	int iTemporary;
	pXML->GetOptionalChildXmlValByName(&iTemporary, L"bIsProjectile" );
	m_bProjectile = iTemporary != 0;

	pXML->GetOptionalChildXmlValByName(&m_fProjectileSpeed, L"fSpeed");
	pXML->GetOptionalChildXmlValByName(&m_fProjectileArc, L"fArcValue");
	pXML->GetOptionalChildXmlValByName(&m_bSticky, L"bSticky");

	return true;
}


void CvEffectInfo::copyNonDefaults(const CvEffectInfo* pClassInfo)
{
	const bool bDefault = false;
	const float fDefault = 0.0f;
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);
	CvScalableInfo::copyNonDefaults(pClassInfo);

	if (getPath() == cDefault) setPath(pClassInfo->getPath());
	if (getUpdateRate() == fDefault) m_fUpdateRate = pClassInfo->getUpdateRate();

	if (isProjectile() == bDefault) m_bProjectile = pClassInfo->isProjectile();

	if (getProjectileSpeed() == fDefault) m_fProjectileSpeed = pClassInfo->getProjectileSpeed();
	if (getProjectileArc() == fDefault) m_fProjectileArc = pClassInfo->getProjectileArc();

	if (isSticky() == bDefault) m_bSticky = pClassInfo->isSticky();
}

