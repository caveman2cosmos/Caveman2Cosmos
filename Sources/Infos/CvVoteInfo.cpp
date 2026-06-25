//------------------------------------------------------------------------------------------------
//  FILE:    CvVoteInfo.cpp
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
#include "CvVoteInfo.h"


//======================================================================================================
//					CvVoteInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvVoteInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvVoteInfo::CvVoteInfo() :
m_iPopulationThreshold(0),
m_iStateReligionVotePercent(0),
m_iTradeRoutes(0),
m_iMinVoters(0),
m_bSecretaryGeneral(false),
m_bVictory(false),
m_bFreeTrade(false),
m_bNoNukes(false),
m_bCityVoting(false),
m_bCivVoting(false),
m_bDefensivePact(false),
m_bOpenBorders(false),
m_bForcePeace(false),
m_bForceNoTrade(false),
m_bForceWar(false),
m_bAssignCity(false),
m_pbForceCivic(NULL),
m_abVoteSourceTypes(NULL)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvVoteInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvVoteInfo::~CvVoteInfo()
{
	SAFE_DELETE_ARRAY(m_pbForceCivic);
	SAFE_DELETE_ARRAY(m_abVoteSourceTypes);
}


int CvVoteInfo::getPopulationThreshold() const
{
	return m_iPopulationThreshold;
}


int CvVoteInfo::getStateReligionVotePercent() const
{
	return m_iStateReligionVotePercent;
}


int CvVoteInfo::getTradeRoutes() const
{
	return m_iTradeRoutes;
}


int CvVoteInfo::getMinVoters() const
{
	return m_iMinVoters;
}


bool CvVoteInfo::isSecretaryGeneral() const
{
	return m_bSecretaryGeneral;
}


bool CvVoteInfo::isVictory() const
{
	return m_bVictory;
}


bool CvVoteInfo::isFreeTrade() const
{
	return m_bFreeTrade;
}


bool CvVoteInfo::isNoNukes() const
{
	return m_bNoNukes;
}


bool CvVoteInfo::isCityVoting() const
{
	return m_bCityVoting;
}


bool CvVoteInfo::isCivVoting() const
{
	return m_bCivVoting;
}


bool CvVoteInfo::isDefensivePact() const
{
	return m_bDefensivePact;
}


bool CvVoteInfo::isOpenBorders() const
{
	return m_bOpenBorders;
}


bool CvVoteInfo::isForcePeace() const
{
	return m_bForcePeace;
}


bool CvVoteInfo::isForceNoTrade() const
{
	return m_bForceNoTrade;
}


bool CvVoteInfo::isForceWar() const
{
	return m_bForceWar;
}


bool CvVoteInfo::isAssignCity() const
{
	return m_bAssignCity;
}


bool CvVoteInfo::isForceCivic(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumCivicInfos(), i);
	return m_pbForceCivic ? m_pbForceCivic[i] : false;
}


bool CvVoteInfo::isVoteSourceType(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumVoteSourceInfos(), i);
	return m_abVoteSourceTypes ? m_abVoteSourceTypes[i] : false;
}


bool CvVoteInfo::read(CvXMLLoadUtility* pXML)
{

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iPopulationThreshold, L"iPopulationThreshold");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionVotePercent, L"iStateReligionVotePercent");
	pXML->GetOptionalChildXmlValByName(&m_iTradeRoutes, L"iTradeRoutes");
	pXML->GetOptionalChildXmlValByName(&m_iMinVoters, L"iMinVoters");

	pXML->GetOptionalChildXmlValByName(&m_bSecretaryGeneral, L"bSecretaryGeneral");
	pXML->GetOptionalChildXmlValByName(&m_bVictory, L"bVictory");
	pXML->GetOptionalChildXmlValByName(&m_bFreeTrade, L"bFreeTrade");
	pXML->GetOptionalChildXmlValByName(&m_bNoNukes, L"bNoNukes");
	pXML->GetOptionalChildXmlValByName(&m_bCityVoting, L"bCityVoting");
	pXML->GetOptionalChildXmlValByName(&m_bCivVoting, L"bCivVoting");
	pXML->GetOptionalChildXmlValByName(&m_bDefensivePact, L"bDefensivePact");
	pXML->GetOptionalChildXmlValByName(&m_bOpenBorders, L"bOpenBorders");
	pXML->GetOptionalChildXmlValByName(&m_bForcePeace, L"bForcePeace");
	pXML->GetOptionalChildXmlValByName(&m_bForceNoTrade, L"bForceNoTrade");
	pXML->GetOptionalChildXmlValByName(&m_bForceWar, L"bForceWar");
	pXML->GetOptionalChildXmlValByName(&m_bAssignCity, L"bAssignCity");

	pXML->SetVariableListTagPair(&m_pbForceCivic, L"ForceCivics",  GC.getNumCivicInfos());
	pXML->SetVariableListTagPair(&m_abVoteSourceTypes, L"DiploVotes",  GC.getNumVoteSourceInfos());

	return true;
}


void CvVoteInfo::copyNonDefaults(const CvVoteInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getPopulationThreshold() == iDefault) m_iPopulationThreshold = pClassInfo->getPopulationThreshold();
	if (getStateReligionVotePercent() == iDefault) m_iStateReligionVotePercent = pClassInfo->getStateReligionVotePercent();
	if (getTradeRoutes() == iDefault) m_iTradeRoutes = pClassInfo->getTradeRoutes();
	if (getMinVoters() == iDefault) m_iMinVoters = pClassInfo->getMinVoters();
	if (isSecretaryGeneral() == bDefault) m_bSecretaryGeneral = pClassInfo->isSecretaryGeneral();
	if (isVictory() == bDefault) m_bVictory = pClassInfo->isVictory();
	if (isFreeTrade() == bDefault) m_bFreeTrade = pClassInfo->isFreeTrade();
	if (isNoNukes() == bDefault) m_bNoNukes = pClassInfo->isNoNukes();
	if (isCityVoting() == bDefault) m_bCityVoting = pClassInfo->isCityVoting();
	if (isCivVoting() == bDefault) m_bCivVoting = pClassInfo->isCivVoting();
	if (isDefensivePact() == bDefault) m_bDefensivePact = pClassInfo->isDefensivePact();
	if (isOpenBorders() == bDefault) m_bOpenBorders = pClassInfo->isOpenBorders();
	if (isForcePeace() == bDefault) m_bForcePeace = pClassInfo->isForcePeace();
	if (isForceNoTrade() == bDefault) m_bForceNoTrade = pClassInfo->isForceNoTrade();
	if (isForceWar() == bDefault) m_bForceWar = pClassInfo->isForceWar();
	if (isAssignCity() == bDefault) m_bAssignCity = pClassInfo->isAssignCity();

	for ( int i = 0; i < GC.getNumCivicInfos(); i++ )
	{
		if ( isForceCivic(i) == bDefault && pClassInfo->isForceCivic(i) != bDefault)
		{
			if ( NULL == m_pbForceCivic )
			{
				CvXMLLoadUtility::InitList(&m_pbForceCivic,GC.getNumCivicInfos(),bDefault);
			}
			m_pbForceCivic[i] = pClassInfo->isForceCivic(i);
		}
	}

	for ( int i = 0; i < GC.getNumVoteSourceInfos(); i++ )
	{
		if ( isVoteSourceType(i) == bDefault && pClassInfo->isVoteSourceType(i) != bDefault)
		{
			if ( NULL == m_abVoteSourceTypes )
			{
				CvXMLLoadUtility::InitList(&m_abVoteSourceTypes,GC.getNumVoteSourceInfos(),bDefault);
			}
			m_abVoteSourceTypes[i] = pClassInfo->isVoteSourceType(i);
		}
	}
}


void CvVoteInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iPopulationThreshold);
	CheckSum(iSum, m_iStateReligionVotePercent);
	CheckSum(iSum, m_iTradeRoutes);
	CheckSum(iSum, m_iMinVoters);

	CheckSum(iSum, m_bSecretaryGeneral);
	CheckSum(iSum, m_bVictory);
	CheckSum(iSum, m_bFreeTrade);
	CheckSum(iSum, m_bNoNukes);
	CheckSum(iSum, m_bCityVoting);
	CheckSum(iSum, m_bCivVoting);
	CheckSum(iSum, m_bDefensivePact);
	CheckSum(iSum, m_bOpenBorders);
	CheckSum(iSum, m_bForcePeace);
	CheckSum(iSum, m_bForceNoTrade);
	CheckSum(iSum, m_bForceWar);
	CheckSum(iSum, m_bAssignCity);

	// Arrays

	CheckSum(iSum, m_pbForceCivic, GC.getNumCivicInfos());
	CheckSum(iSum, m_abVoteSourceTypes, GC.getNumVoteSourceInfos());
}

