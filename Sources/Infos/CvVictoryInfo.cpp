//------------------------------------------------------------------------------------------------
//  FILE:    CvVictoryInfo.cpp
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
#include "CvVictoryInfo.h"


//======================================================================================================
//					CvVictoryInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvVictoryInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvVictoryInfo::CvVictoryInfo() :
 m_iPopulationPercentLead(0)
,m_iLandPercent(0)
,m_iMinLandPercent(0)
,m_iReligionPercent(0)
,m_iCityCulture(NO_CULTURELEVEL)
,m_iNumCultureCities(0)
,m_iTotalCultureRatio(0)
,m_iVictoryDelayTurns(0)
,m_bTargetScore(false)
,m_bEndScore(false)
,m_bConquest(false)
,m_bDiploVote(false)
,m_bPermanent(false)
,m_bTotalVictory(false)
{ }


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvVictoryInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvVictoryInfo::~CvVictoryInfo() {}


int CvVictoryInfo::getPopulationPercentLead() const
{
	return m_iPopulationPercentLead;
}


int CvVictoryInfo::getLandPercent() const
{
	return m_iLandPercent;
}


int CvVictoryInfo::getMinLandPercent() const
{
	return m_iMinLandPercent;
}


int CvVictoryInfo::getReligionPercent() const
{
	return m_iReligionPercent;
}


int CvVictoryInfo::getCityCulture() const
{
	return m_iCityCulture;
}


int CvVictoryInfo::getNumCultureCities() const
{
	return m_iNumCultureCities;
}


int CvVictoryInfo::getTotalCultureRatio() const
{
	return m_iTotalCultureRatio;
}


int CvVictoryInfo::getVictoryDelayTurns() const
{
	return m_iVictoryDelayTurns;
}


bool CvVictoryInfo::isTargetScore() const
{
	return m_bTargetScore;
}


bool CvVictoryInfo::isEndScore() const
{
	return m_bEndScore;
}


bool CvVictoryInfo::isConquest() const
{
	return m_bConquest;
}


bool CvVictoryInfo::isDiploVote() const
{
	return m_bDiploVote;
}


bool CvVictoryInfo::isPermanent() const
{
	return m_bPermanent;
}


const char* CvVictoryInfo::getMovie() const
{
	return m_szMovie;
}


bool CvVictoryInfo::isTotalVictory() const
{
	return m_bTotalVictory;
}



//
// read from xml
//
bool CvVictoryInfo::read(CvXMLLoadUtility* pXML)
{

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_bTargetScore, L"bTargetScore");
	pXML->GetOptionalChildXmlValByName(&m_bEndScore, L"bEndScore");
	pXML->GetOptionalChildXmlValByName(&m_bConquest, L"bConquest");
	pXML->GetOptionalChildXmlValByName(&m_bDiploVote, L"bDiploVote");
	pXML->GetOptionalChildXmlValByName(&m_bPermanent, L"bPermanent");
	pXML->GetOptionalChildXmlValByName(&m_bTotalVictory, L"bTotalVictory");

	pXML->GetOptionalChildXmlValByName(&m_iPopulationPercentLead, L"iPopulationPercentLead");
	pXML->GetOptionalChildXmlValByName(&m_iLandPercent, L"iLandPercent");
	pXML->GetOptionalChildXmlValByName(&m_iMinLandPercent, L"iMinLandPercent");
	pXML->GetOptionalChildXmlValByName(&m_iReligionPercent, L"iReligionPercent");
	pXML->GetOptionalChildXmlValByName(&m_iNumCultureCities, L"iNumCultureCities");
	pXML->GetOptionalChildXmlValByName(&m_iTotalCultureRatio, L"iTotalCultureRatio");
	pXML->GetOptionalChildXmlValByName(&m_iVictoryDelayTurns, L"iVictoryDelayTurns");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CityCulture");
	m_iCityCulture = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(m_szMovie, L"VictoryMovie");

	return true;
}


void CvVictoryInfo::copyNonDefaults(const CvVictoryInfo* pClassInfo)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (isTargetScore() == bDefault) m_bTargetScore = pClassInfo->isTargetScore();
	if (isEndScore() == bDefault) m_bEndScore = pClassInfo->isEndScore();
	if (isConquest() == bDefault) m_bConquest = pClassInfo->isConquest();
	if (isDiploVote() == bDefault) m_bDiploVote = pClassInfo->isDiploVote();
	if (isPermanent() == bDefault) m_bPermanent = pClassInfo->isPermanent();

	if (getPopulationPercentLead() == iDefault) m_iPopulationPercentLead = pClassInfo->getPopulationPercentLead();
	if (getLandPercent() == iDefault) m_iLandPercent = pClassInfo->getLandPercent();
	if (getMinLandPercent() == iDefault) m_iMinLandPercent = pClassInfo->getMinLandPercent();
	if (getReligionPercent() == iDefault) m_iReligionPercent = pClassInfo->getReligionPercent();

	if (getCityCulture() == iTextDefault) m_iCityCulture = pClassInfo->getCityCulture();

	if (getNumCultureCities() == iDefault) m_iNumCultureCities = pClassInfo->getNumCultureCities();
	if (getTotalCultureRatio() == iDefault) m_iTotalCultureRatio = pClassInfo->getTotalCultureRatio();
	if (getVictoryDelayTurns() == iDefault) m_iVictoryDelayTurns = pClassInfo->getVictoryDelayTurns();

	if (getMovie() == cDefault) m_szMovie = pClassInfo->getMovie();

	if (isTotalVictory() == bDefault) m_bTotalVictory = pClassInfo->isTotalVictory();
}


void CvVictoryInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iPopulationPercentLead);
	CheckSum(iSum, m_iLandPercent);
	CheckSum(iSum, m_iMinLandPercent);
	CheckSum(iSum, m_iReligionPercent);
	CheckSum(iSum, m_iCityCulture);
	CheckSum(iSum, m_iNumCultureCities);
	CheckSum(iSum, m_iTotalCultureRatio);
	CheckSum(iSum, m_iVictoryDelayTurns);

	CheckSum(iSum, m_bTargetScore);
	CheckSum(iSum, m_bEndScore);
	CheckSum(iSum, m_bConquest);
	CheckSum(iSum, m_bDiploVote);
	CheckSum(iSum, m_bPermanent);
}

