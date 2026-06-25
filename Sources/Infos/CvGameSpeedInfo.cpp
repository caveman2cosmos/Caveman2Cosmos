//------------------------------------------------------------------------------------------------
//  FILE:    CvGameSpeedInfo.cpp
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
#include "CvGameSpeedInfo.h"


//======================================================================================================
//					CvGameSpeedInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvGameSpeedInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvGameSpeedInfo::CvGameSpeedInfo() :
m_iSpeedPercent(0),
m_iNumTurnIncrements(0),
m_pGameTurnInfo(NULL),
m_bEndDatesCalculated(false)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvGameSpeedInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvGameSpeedInfo::~CvGameSpeedInfo()
{
	SAFE_DELETE_ARRAY(m_pGameTurnInfo);
}


int CvGameSpeedInfo::getSpeedPercent() const
{
	return m_iSpeedPercent;
}


int CvGameSpeedInfo::getHammerCostPercent() const
{
	if (GC.getGame().isOption(GAMEOPTION_EXP_UPSCALED_BUILDING_AND_UNIT_COSTS))
	{
		return getModifiedIntValue(m_iSpeedPercent, GC.getUPSCALED_HAMMER_COST_MODIFIER());
	}
	return m_iSpeedPercent;
}


int CvGameSpeedInfo::getNumTurnIncrements() const
{
	return m_iNumTurnIncrements;
}


const CvDateIncrement& CvGameSpeedInfo::getDateIncrement(int iIndex) const
{
	return m_aIncrements[iIndex];
}


std::vector<CvDateIncrement>& CvGameSpeedInfo::getIncrements()
{
	return m_aIncrements;
}


bool CvGameSpeedInfo::getEndDatesCalculated() const
{
	return m_bEndDatesCalculated;
}


void CvGameSpeedInfo::setEndDatesCalculated(bool bCalculated)
{
	m_bEndDatesCalculated = bCalculated;
}


const GameTurnInfo& CvGameSpeedInfo::getGameTurnInfo(int iIndex) const
{
	return m_pGameTurnInfo[iIndex];
}


void CvGameSpeedInfo::allocateGameTurnInfos(int iSize)
{
	m_pGameTurnInfo = new GameTurnInfo[iSize];
}


int CvGameSpeedInfo::getPercent(int iID) const
{
	return m_Percent.getValue(iID);
}


bool CvGameSpeedInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetOptionalChildXmlValByName(&m_iSpeedPercent, L"iSpeedPercent");

	if (pXML->TryMoveToXmlFirstChild(L"GameTurnInfos"))
	{
		m_bEndDatesCalculated = false;
		m_iNumTurnIncrements = pXML->GetXmlChildrenNumber(L"GameTurnInfo");
		char szLog[1000];
		sprintf(szLog, ":: %i", m_iNumTurnIncrements);
#ifdef _DEBUG
		OutputDebugString(szLog);
#endif
		if (pXML->TryMoveToXmlFirstChild(L"GameTurnInfo"))
		{
			allocateGameTurnInfos(getNumTurnIncrements());
			int iTempVal;

			// loop through each tag
			for (int j = 0; j < getNumTurnIncrements(); j++)
			{
				CvDateIncrement inc;
				pXML->GetOptionalChildXmlValByName(&iTempVal, L"iMonthIncrement");
				m_pGameTurnInfo[j].iMonthIncrement = iTempVal;
				inc.m_iIncrementMonth = iTempVal;
				pXML->GetOptionalChildXmlValByName(&iTempVal, L"iTurnsPerIncrement");
				m_pGameTurnInfo[j].iNumGameTurnsPerIncrement = iTempVal;
				inc.m_iendTurn = iTempVal + (m_aIncrements.empty() ? 0 : m_aIncrements[m_aIncrements.size()-1].m_iendTurn);
				pXML->GetOptionalChildXmlValByName(&iTempVal, L"iDayIncrement");
				inc.m_iIncrementDay = iTempVal;
				m_aIncrements.push_back(inc);

				// if we cannot set the current xml node to it's next sibling then we will break out of the for loop
				// otherwise we will continue looping
				if (!pXML->TryMoveToXmlNextSibling(L"GameTurnInfo"))
				{
					break;
				}
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}
	m_Percent.read(pXML, L"Percents");

	return true;
}


void CvGameSpeedInfo::copyNonDefaults(const CvGameSpeedInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (m_iSpeedPercent == iDefault) m_iSpeedPercent = pClassInfo->getSpeedPercent();

	if (getNumTurnIncrements() == iDefault)
	{
		m_iNumTurnIncrements = pClassInfo->getNumTurnIncrements();
		allocateGameTurnInfos(getNumTurnIncrements());
		// loop through each tag
		for (int j = 0; j < getNumTurnIncrements(); j++)
		{
			m_pGameTurnInfo[j] = pClassInfo->getGameTurnInfo(j);
			m_aIncrements[j] = pClassInfo->getDateIncrement(j);
		}
	}

	m_Percent.copyNonDefaults(pClassInfo->m_Percent);
}


void CvGameSpeedInfo::getCheckSum(uint32_t& iSum) const
{
	PROFILE_EXTRA_FUNC();
	CheckSum(iSum, m_iSpeedPercent);

	for (int j = 0; j < m_iNumTurnIncrements; j++)
	{
		CheckSum(iSum, m_pGameTurnInfo[j].iMonthIncrement);
		CheckSum(iSum, m_pGameTurnInfo[j].iNumGameTurnsPerIncrement);
		CheckSum(iSum, m_aIncrements[j].m_iIncrementDay);
	}
	CheckSumC(iSum, m_Percent);
}

