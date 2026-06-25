//------------------------------------------------------------------------------------------------
//  FILE:    CvOutcomeInfo.cpp
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
#include "CvOutcomeInfo.h"



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvOutcomeInfo
//
//  DESC:   Contains info about outcome types which can be the result of a kill or of actions
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvOutcomeInfo::CvOutcomeInfo() :
									m_ePrereqTech(NO_TECH),
									m_eObsoleteTech(NO_TECH),
									m_bToCoastalCity(false),
									m_bFriendlyTerritory(true),
									m_bNeutralTerritory(true),
									m_bHostileTerritory(true),
									m_bBarbarianTerritory(false),
									m_bCity(false),
									m_bNotCity(false),
									m_bCapture(false),
									m_ePrereqCivic(NO_CIVIC)
{
}


CvOutcomeInfo::~CvOutcomeInfo()
{
	GC.removeDelayedResolutionVector(m_aeReplaceOutcomes);
}


bool CvOutcomeInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szMessageText, L"Message");
	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqTech");
	m_ePrereqTech = (TechTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ObsoleteTech");
	m_eObsoleteTech = (TechTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqCivic");
	m_ePrereqCivic = (CivicTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_bToCoastalCity, L"bToCoastalCity");
	pXML->GetOptionalChildXmlValByName(&m_bFriendlyTerritory, L"bFriendlyTerritory", true);
	pXML->GetOptionalChildXmlValByName(&m_bNeutralTerritory, L"bNeutralTerritory", true);
	pXML->GetOptionalChildXmlValByName(&m_bHostileTerritory, L"bHostileTerritory", true);
	pXML->GetOptionalChildXmlValByName(&m_bBarbarianTerritory, L"bBarbarianTerritory");
	pXML->GetOptionalChildXmlValByName(&m_bCity, L"bCity");
	pXML->GetOptionalChildXmlValByName(&m_bNotCity, L"bNotCity");
	pXML->GetOptionalChildXmlValByName(&m_bCapture, L"bCapture");

	if(pXML->TryMoveToXmlFirstChild(L"ExtraChancePromotions"))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"ExtraChancePromotion"))
			{
				do
				{
					int iExtraChance;
					pXML->GetChildXmlValByName(szTextVal, L"PromotionType");
					PromotionTypes ePromotion = (PromotionTypes) pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&iExtraChance, L"iExtraChance");
					m_aeiExtraChancePromotions.push_back(std::pair<PromotionTypes,int>(ePromotion, iExtraChance));
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	pXML->SetOptionalVector(&m_aePrereqBuildings, L"PrereqBuildings");
	pXML->SetOptionalVectorWithDelayedResolution(m_aeReplaceOutcomes, L"ReplaceOutcomes");

	return true;
}


void CvOutcomeInfo::copyNonDefaults(const CvOutcomeInfo* pClassInfo)
{
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getMessageText() == wDefault) m_szMessageText = pClassInfo->getMessageText();
	if (getPrereqTech() == NO_TECH) m_ePrereqTech = pClassInfo->getPrereqTech();
	if (getObsoleteTech() == NO_TECH) m_eObsoleteTech = pClassInfo->getObsoleteTech();
	if (!getToCoastalCity()) m_bToCoastalCity = pClassInfo->getToCoastalCity();
	if (getFriendlyTerritory()) m_bFriendlyTerritory = pClassInfo->getFriendlyTerritory();
	if (getNeutralTerritory()) m_bNeutralTerritory = pClassInfo->getNeutralTerritory();
	if (getHostileTerritory()) m_bHostileTerritory = pClassInfo->getHostileTerritory();
	if (!getBarbarianTerritory()) m_bBarbarianTerritory = pClassInfo->getBarbarianTerritory();
	if (!getCity()) m_bCity = pClassInfo->getCity();
	if (!getNotCity()) m_bNotCity = pClassInfo->getNotCity();
	if (!isCapture()) m_bCapture = pClassInfo->isCapture();
	if (m_aePrereqBuildings.empty()) m_aePrereqBuildings = pClassInfo->m_aePrereqBuildings;
	if (getNumExtraChancePromotions() == 0) m_aeiExtraChancePromotions = pClassInfo->m_aeiExtraChancePromotions;
	if (getPrereqCivic() == NO_CIVIC) m_ePrereqCivic = pClassInfo->getPrereqCivic();

	GC.copyNonDefaultDelayedResolutionVector(m_aeReplaceOutcomes, pClassInfo->getReplaceOutcomes());
}


void CvOutcomeInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_ePrereqTech);
	CheckSum(iSum, m_eObsoleteTech);
	CheckSumC(iSum, m_aeiExtraChancePromotions);
	CheckSumC(iSum, m_aePrereqBuildings);
	CheckSum(iSum, m_bToCoastalCity);
	CheckSum(iSum, m_bFriendlyTerritory);
	CheckSum(iSum, m_bNeutralTerritory);
	CheckSum(iSum, m_bHostileTerritory);
	CheckSum(iSum, m_bBarbarianTerritory);
	CheckSum(iSum, m_bCity);
	CheckSum(iSum, m_bNotCity);
	CheckSum(iSum, m_bCapture);
	CheckSumC(iSum, m_aeReplaceOutcomes);
	CheckSum(iSum, m_ePrereqCivic);
}


CvWString CvOutcomeInfo::getMessageText() const
{
	return m_szMessageText;
}


bool CvOutcomeInfo::getToCoastalCity() const
{
	return m_bToCoastalCity;
}


bool CvOutcomeInfo::getFriendlyTerritory() const
{
	return m_bFriendlyTerritory;
}


bool CvOutcomeInfo::getNeutralTerritory() const
{
	return m_bNeutralTerritory;
}


bool CvOutcomeInfo::getHostileTerritory() const
{
	return m_bHostileTerritory;
}


bool CvOutcomeInfo::getBarbarianTerritory() const
{
	return m_bBarbarianTerritory;
}


bool CvOutcomeInfo::getCity() const
{
	return m_bCity;
}


bool CvOutcomeInfo::getNotCity() const
{
	return m_bNotCity;
}


bool CvOutcomeInfo::isCapture() const
{
	return m_bCapture;
}


TechTypes CvOutcomeInfo::getObsoleteTech() const
{
	return m_eObsoleteTech;
}


CivicTypes CvOutcomeInfo::getPrereqCivic() const
{
	return m_ePrereqCivic;
}


int CvOutcomeInfo::getNumExtraChancePromotions() const
{
	return m_aeiExtraChancePromotions.size();
}


PromotionTypes CvOutcomeInfo::getExtraChancePromotion(int i) const
{
	FASSERT_BOUNDS(0, getNumExtraChancePromotions(), i);
	return m_aeiExtraChancePromotions[i].first;
}


int CvOutcomeInfo::getExtraChancePromotionChance(int i) const
{
	FASSERT_BOUNDS(0, getNumExtraChancePromotions(), i);
	return m_aeiExtraChancePromotions[i].second;
}

