//------------------------------------------------------------------------------------------------
//  FILE:    CvVoteSourceInfo.cpp
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
#include "CvVoteSourceInfo.h"


CvVoteSourceInfo::CvVoteSourceInfo() :
	m_iVoteInterval(0),
	m_iFreeSpecialist(NO_SPECIALIST),
	m_iCivic(NO_CIVIC),
	m_aiReligionYields(NULL),
	m_aiReligionCommerces(NULL)
{
}


CvVoteSourceInfo::~CvVoteSourceInfo()
{
	SAFE_DELETE_ARRAY(m_aiReligionYields);
	SAFE_DELETE_ARRAY(m_aiReligionCommerces);

	GC.removeDelayedResolution((int*)&m_iCivic);
}


int CvVoteSourceInfo::getVoteInterval() const
{
	return m_iVoteInterval;
}


int CvVoteSourceInfo::getFreeSpecialist() const
{
	return m_iFreeSpecialist;
}


int CvVoteSourceInfo::getCivic() const
{
	return m_iCivic;
}


int CvVoteSourceInfo::getReligionYield(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_aiReligionYields[i];
}


int CvVoteSourceInfo::getReligionCommerce(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_aiReligionCommerces[i];
}


const CvWString CvVoteSourceInfo::getPopupText() const
{
	return gDLL->getText(m_szPopupText);
}


const CvWString CvVoteSourceInfo::getSecretaryGeneralText() const
{
	return gDLL->getText(m_szSecretaryGeneralText);
}


bool CvVoteSourceInfo::read(CvXMLLoadUtility* pXML)
{

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iVoteInterval, L"iVoteInterval");
	pXML->GetOptionalChildXmlValByName(m_szPopupText, L"PopupText");
	pXML->GetOptionalChildXmlValByName(m_szSecretaryGeneralText, L"SecretaryGeneralText");

	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"FreeSpecialist");
	m_iFreeSpecialist = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Civic");
	GC.addDelayedResolution((int*)&m_iCivic, szTextVal);

	if (pXML->TryMoveToXmlFirstChild(L"ReligionYields"))
	{
		pXML->SetYields(&m_aiReligionYields);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_aiReligionYields);
	}

	if (pXML->TryMoveToXmlFirstChild(L"ReligionCommerces"))
	{
		pXML->SetCommerce(&m_aiReligionCommerces);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_aiReligionCommerces);
	}

	return true;
}


void CvVoteSourceInfo::copyNonDefaults(const CvVoteSourceInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getVoteInterval() == iDefault) m_iVoteInterval = pClassInfo->getVoteInterval();
	if (getPopupText() == wDefault) m_szPopupText = pClassInfo->m_szPopupText;
	if (getSecretaryGeneralText() == wDefault) m_szSecretaryGeneralText = pClassInfo->m_szSecretaryGeneralText;
	if (getFreeSpecialist() == iTextDefault) m_iFreeSpecialist = pClassInfo->getFreeSpecialist();
	GC.copyNonDefaultDelayedResolution((int*)&m_iCivic, (int*)&pClassInfo->m_iCivic);

	for ( int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		if ( getReligionYield(i) == iDefault )
		{
			m_aiReligionYields[i] = pClassInfo->getReligionYield(i);
		}
	}

	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++)
	{
		if ( getReligionCommerce(i) == iDefault )
		{
			m_aiReligionCommerces[i] = pClassInfo->getReligionCommerce(i);
		}
	}
}


void CvVoteSourceInfo::getCheckSum(uint32_t &iSum) const
{
	CheckSum(iSum, m_iVoteInterval);
	CheckSum(iSum, m_iFreeSpecialist);
	CheckSum(iSum, m_iCivic);

	CheckSum(iSum, m_aiReligionYields, NUM_YIELD_TYPES);
	CheckSum(iSum, m_aiReligionCommerces, NUM_COMMERCE_TYPES);

	CheckSumC(iSum, m_szPopupText);
	CheckSumC(iSum, m_szSecretaryGeneralText);
}

