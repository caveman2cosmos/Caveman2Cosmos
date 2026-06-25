//------------------------------------------------------------------------------------------------
//  FILE:    CvYieldInfo.cpp
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
#include "CvYieldInfo.h"


//======================================================================================================
//					CvYieldInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvYieldInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvYieldInfo::CvYieldInfo() :
m_iChar(0),
m_iHillsChange(0),
m_iPeakChange(0),
m_iRiverChange(0),
m_iCityChange(0),
m_iPopulationChangeDivisor(0),
m_iMinCity(0),
m_iTradeModifier(0),
m_iGoldenAgeYield(0),
m_iGoldenAgeYieldThreshold(0),
m_iAIWeightPercent(0),
m_iColorType(NO_COLOR),
m_paszSymbolPath(NULL)
{
}



//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvYieldInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvYieldInfo::~CvYieldInfo()
{
	SAFE_DELETE_ARRAY(m_paszSymbolPath);
}


int CvYieldInfo::getChar() const
{
	return m_iChar;
}


void CvYieldInfo::setChar(int i)
{
	m_iChar = i;
}


int CvYieldInfo::getHillsChange() const
{
	return m_iHillsChange;
}


int CvYieldInfo::getPeakChange() const
{
	return m_iPeakChange;
}


int CvYieldInfo::getRiverChange() const
{
	return m_iRiverChange;
}


int CvYieldInfo::getCityChange() const
{
	return m_iCityChange;
}


int CvYieldInfo::getPopulationChangeDivisor() const
{
	return m_iPopulationChangeDivisor;
}


int CvYieldInfo::getMinCity() const
{
	return m_iMinCity;
}


int CvYieldInfo::getTradeModifier() const
{
	return m_iTradeModifier;
}


int CvYieldInfo::getGoldenAgeYield() const
{
	return m_iGoldenAgeYield;
}


int CvYieldInfo::getGoldenAgeYieldThreshold() const
{
	return m_iGoldenAgeYieldThreshold;
}


int CvYieldInfo::getAIWeightPercent() const
{
	return m_iAIWeightPercent;
}


int CvYieldInfo::getColorType() const
{
	return m_iColorType;
}


// Arrays

const char* CvYieldInfo::getSymbolPath(int i) const
{
	FASSERT_BOUNDS(0, GC.getDefineINT("MAX_YIELD_STACK"), i);
	return m_paszSymbolPath ? m_paszSymbolPath[i] : reinterpret_cast<const char*>(-1);
}


bool CvYieldInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	int iNumSibs, j;

	pXML->GetOptionalChildXmlValByName(&m_iHillsChange, L"iHillsChange");
	pXML->GetOptionalChildXmlValByName(&m_iPeakChange, L"iPeakChange");
	pXML->GetOptionalChildXmlValByName(&m_iRiverChange, L"iRiverChange");
	pXML->GetOptionalChildXmlValByName(&m_iCityChange, L"iCityChange");
	pXML->GetOptionalChildXmlValByName(&m_iPopulationChangeDivisor, L"iPopulationChangeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iMinCity, L"iMinCity");
	pXML->GetOptionalChildXmlValByName(&m_iTradeModifier, L"iTradeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGoldenAgeYield, L"iGoldenAgeYield");
	pXML->GetOptionalChildXmlValByName(&m_iGoldenAgeYieldThreshold, L"iGoldenAgeYieldThreshold");
	pXML->GetOptionalChildXmlValByName(&m_iAIWeightPercent, L"iAIWeightPercent");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ColorType");
	m_iColorType = pXML->GetInfoClass(szTextVal);

	if (pXML->TryMoveToXmlFirstChild(L"SymbolPaths"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		FAssertMsg((0 < GC.getDefineINT("MAX_YIELD_STACK")) ,"Allocating zero or less memory in SetGlobalYieldInfo");
		m_paszSymbolPath = new CvString[GC.getDefineINT("MAX_YIELD_STACK")];

		if (0 < iNumSibs)
		{
			if (pXML->GetChildXmlVal(szTextVal))
			{
				FAssertMsg((iNumSibs <= GC.getDefineINT("MAX_YIELD_STACK")) ,"There are more siblings than memory allocated for them in SetGlobalYieldInfo");
				for (j=0;j<iNumSibs;j++)
				{
					m_paszSymbolPath[j] = szTextVal;
					if (!pXML->GetNextXmlVal(szTextVal))
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}
	else
	{
		CvString cDefault = CvString::format("").GetCString();
		m_paszSymbolPath = new CvString[GC.getDefineINT("MAX_YIELD_STACK")];
		for ( int i = 0; i < GC.getDefineINT("MAX_YIELD_STACK"); i++)
		{
			m_paszSymbolPath[i] = cDefault;
		}
	}

	return true;
}


void CvYieldInfo::copyNonDefaults(const CvYieldInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const int iDefault = 0;
	const int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getHillsChange() == iDefault) m_iHillsChange = pClassInfo->getHillsChange();
	if (getPeakChange() == iDefault) m_iPeakChange = pClassInfo->getPeakChange();
	if (m_iRiverChange == iDefault) m_iRiverChange = pClassInfo->getRiverChange();
	if (getCityChange() == iDefault) m_iCityChange = pClassInfo->getCityChange();
	if (getPopulationChangeDivisor() == iDefault) m_iPopulationChangeDivisor = pClassInfo->getPopulationChangeDivisor();
	if (getMinCity() == iDefault) m_iMinCity = pClassInfo->getMinCity();
	if (getTradeModifier() == iDefault) m_iTradeModifier = pClassInfo->getTradeModifier();
	if (getGoldenAgeYield() == iDefault) m_iGoldenAgeYield = pClassInfo->getGoldenAgeYield();
	if (getGoldenAgeYieldThreshold() == iDefault) m_iGoldenAgeYieldThreshold = pClassInfo->getGoldenAgeYieldThreshold();
	if (getAIWeightPercent() == iDefault) m_iAIWeightPercent = pClassInfo->getAIWeightPercent();
	if (getColorType() == iTextDefault) m_iColorType = pClassInfo->getColorType();

	for ( int i = 0; i < GC.getDefineINT("MAX_YIELD_STACK"); i++)
	{
		if ( m_paszSymbolPath[i] == cDefault)
		{
			m_paszSymbolPath[i] = pClassInfo->getSymbolPath(i);
		}
	}
}


void CvYieldInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iHillsChange);
	CheckSum(iSum, m_iPeakChange);
	CheckSum(iSum, m_iRiverChange);
	CheckSum(iSum, m_iCityChange);
	CheckSum(iSum, m_iPopulationChangeDivisor);
	CheckSum(iSum, m_iMinCity);
	CheckSum(iSum, m_iTradeModifier);
	CheckSum(iSum, m_iGoldenAgeYield);
	CheckSum(iSum, m_iGoldenAgeYieldThreshold);
	CheckSum(iSum, m_iAIWeightPercent);
}

