//------------------------------------------------------------------------------------------------
//  FILE:    CvScalableInfo.cpp
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
#include "CvInfoBase.h"


//======================================================================================================
//					CvScalableInfo
//======================================================================================================
bool CvScalableInfo::read(CvXMLLoadUtility* pXML)
{
	pXML->GetOptionalChildXmlValByName(&m_fScale, L"fScale");
	pXML->GetOptionalChildXmlValByName(&m_fInterfaceScale, L"fInterfaceScale", 1.0f);
	return true;
}


void CvScalableInfo::copyNonDefaults(const CvScalableInfo* pClassInfo)
{
	const float fDefault = 0.0f;

	if (getScale() == fDefault) m_fScale = pClassInfo->getScale();
	if (getInterfaceScale() == 1.0f) m_fInterfaceScale = pClassInfo->getInterfaceScale();
}


float CvScalableInfo::getScale() const
{
	return m_fScale;
}


float CvScalableInfo::getInterfaceScale() const
{
	return m_fInterfaceScale;
}

