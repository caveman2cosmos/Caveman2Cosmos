//------------------------------------------------------------------------------------------------
//  FILE:    CvClimateInfo.cpp
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
#include "CvClimateInfo.h"


//======================================================================================================
//					CvClimateInfo
//======================================================================================================
CvClimateInfo::CvClimateInfo() :
m_iDesertPercentChange(0),
m_iJungleLatitude(0),
m_iHillRange(0),
m_iPeakPercent(0),
m_fSnowLatitudeChange(0.0f),
m_fTundraLatitudeChange(0.0f),
m_fGrassLatitudeChange(0.0f),
m_fDesertBottomLatitudeChange(0.0f),
m_fDesertTopLatitudeChange(0.0f),
m_fIceLatitude(0.0f),
m_fRandIceLatitude(0.0f)
{
}


CvClimateInfo::~CvClimateInfo()
{
}


int CvClimateInfo::getDesertPercentChange() const
{
	return m_iDesertPercentChange;
}


int CvClimateInfo::getJungleLatitude() const
{
	return m_iJungleLatitude;
}


int CvClimateInfo::getHillRange() const
{
	return m_iHillRange;
}


int CvClimateInfo::getPeakPercent() const
{
	return m_iPeakPercent;
}


float CvClimateInfo::getSnowLatitudeChange() const
{
	return m_fSnowLatitudeChange;
}


float CvClimateInfo::getTundraLatitudeChange() const
{
	return m_fTundraLatitudeChange;
}


float CvClimateInfo::getGrassLatitudeChange() const
{
	return m_fGrassLatitudeChange;
}


float CvClimateInfo::getDesertBottomLatitudeChange() const
{
	return m_fDesertBottomLatitudeChange;
}


float CvClimateInfo::getDesertTopLatitudeChange() const
{
	return m_fDesertTopLatitudeChange;
}


float CvClimateInfo::getIceLatitude() const
{
	return m_fIceLatitude;
}


float CvClimateInfo::getRandIceLatitude() const
{
	return m_fRandIceLatitude;
}


bool CvClimateInfo::read(CvXMLLoadUtility* pXML)
{

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iDesertPercentChange, L"iDesertPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iJungleLatitude, L"iJungleLatitude");
	pXML->GetOptionalChildXmlValByName(&m_iHillRange, L"iHillRange");
	pXML->GetOptionalChildXmlValByName(&m_iPeakPercent, L"iPeakPercent");

	pXML->GetOptionalChildXmlValByName(&m_fSnowLatitudeChange, L"fSnowLatitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_fTundraLatitudeChange, L"fTundraLatitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_fGrassLatitudeChange, L"fGrassLatitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_fDesertBottomLatitudeChange, L"fDesertBottomLatitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_fDesertTopLatitudeChange, L"fDesertTopLatitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_fIceLatitude, L"fIceLatitude");
	pXML->GetOptionalChildXmlValByName(&m_fRandIceLatitude, L"fRandIceLatitude");

	return true;
}


void CvClimateInfo::copyNonDefaults(const CvClimateInfo* pClassInfo)
{
	int iDefault = 0;
	float fDefault = 0.0f;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getDesertPercentChange() == iDefault) m_iDesertPercentChange = pClassInfo->getDesertPercentChange();
	if (getJungleLatitude() == iDefault) m_iJungleLatitude = pClassInfo->getJungleLatitude();
	if (getHillRange() == iDefault) m_iHillRange = pClassInfo->getHillRange();
	if (getPeakPercent() == iDefault) m_iPeakPercent = pClassInfo->getPeakPercent();

	if (getSnowLatitudeChange() == fDefault) m_fSnowLatitudeChange = pClassInfo->getSnowLatitudeChange();
	if (getTundraLatitudeChange() == fDefault) m_fTundraLatitudeChange = pClassInfo->getTundraLatitudeChange();
	if (getGrassLatitudeChange() == fDefault) m_fGrassLatitudeChange = pClassInfo->getGrassLatitudeChange();
	if (getDesertBottomLatitudeChange() == fDefault) m_fDesertBottomLatitudeChange = pClassInfo->getDesertBottomLatitudeChange();
	if (getDesertTopLatitudeChange() == fDefault) m_fDesertTopLatitudeChange = pClassInfo->getDesertTopLatitudeChange();
	if (getIceLatitude() == fDefault) m_fIceLatitude = pClassInfo->getIceLatitude();
	if (getRandIceLatitude() == fDefault) m_fRandIceLatitude = pClassInfo->getRandIceLatitude();
}

