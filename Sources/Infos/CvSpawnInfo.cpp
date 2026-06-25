//------------------------------------------------------------------------------------------------
//  FILE:    CvSpawnInfo.cpp
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
#include "CvSpawnInfo.h"


//======================================================================================================
//					CvSpawnInfo
//======================================================================================================
CvSpawnInfo::CvSpawnInfo()
	:
	m_eUnitType(NO_UNIT),
	m_ePrereqTech(NO_TECH),
	m_eObsoleteTechType(NO_TECH),
	m_iPlayerType(-1),
	m_iTurns(-1),
	m_iGlobalTurns(-1),
	m_iMaxLocalDensity(0),
	m_iMinAreaPlotsPerPlayerUnit(0),
	m_iMinAreaPlotsPerUnitType(0),
	m_iStartDate(-200000),
	m_iEndDate(50000),
	m_bTreatAsBarbarian(false),
	m_bNeutralOnly(true),
	m_bNotInView(false),
	m_bNoSpeedNormalization(false),
	m_iRateOverride(100),
	m_bHills(false),
	m_bFlatlands(false),
	m_bPeaks(false),
	m_bFreshWaterOnly(false),
	m_bLatitudeAbs(true),
	m_iMinLatitude(-90),
	m_iMaxLatitude(90),
	m_iMinLongitude(-180),
	m_iMaxLongitude(180),
	m_pExprSpawnCondition(NULL)
{ }


CvSpawnInfo::~CvSpawnInfo()
{
	SAFE_DELETE(m_pExprSpawnCondition);
}


bool CvSpawnInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	CvString szTextVal2;

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"UnitType");
	m_eUnitType = (UnitTypes)pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqTech");
	m_ePrereqTech = (TechTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ObsoleteTech");
	m_eObsoleteTechType = (TechTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalTypeEnum(m_iPlayerType, L"PlayerType");

	pXML->GetOptionalChildXmlValByName(&m_iTurns, L"iTurns");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalTurns, L"iGlobalTurns", -1);
	pXML->GetOptionalChildXmlValByName(&m_iMaxLocalDensity, L"iMaxLocalDensity");
	pXML->GetOptionalChildXmlValByName(&m_iMinAreaPlotsPerPlayerUnit, L"iMinAreaPlotsPerPlayerUnit");
	pXML->GetOptionalChildXmlValByName(&m_iMinAreaPlotsPerUnitType, L"iMinAreaPlotsPerUnitType");
	pXML->GetOptionalChildXmlValByName(&m_iStartDate, L"iStartDate", -200000);
	pXML->GetOptionalChildXmlValByName(&m_iEndDate, L"iEndDate", 50000);
	pXML->GetOptionalChildXmlValByName(&m_bTreatAsBarbarian, L"bTreatAsBarbarian", false);
	pXML->GetOptionalChildXmlValByName(&m_bNeutralOnly, L"bNeutralOnly", true);
	pXML->GetOptionalChildXmlValByName(&m_bNoSpeedNormalization, L"bNoSpeedNormalization");
	pXML->GetOptionalChildXmlValByName(&m_bNotInView, L"bNotInView");
	pXML->GetOptionalChildXmlValByName(&m_bHills, L"bHills");
	pXML->GetOptionalChildXmlValByName(&m_bFlatlands, L"bFlatlands", true);
	pXML->GetOptionalChildXmlValByName(&m_bPeaks, L"bPeaks");
	pXML->GetOptionalChildXmlValByName(&m_bFreshWaterOnly, L"bFreshWaterOnly");
	pXML->GetOptionalChildXmlValByName(&m_bLatitudeAbs, L"bLatitudeAbs", true);
	pXML->GetOptionalChildXmlValByName(&m_iMinLatitude, L"iMinLatitude", -90);
	pXML->GetOptionalChildXmlValByName(&m_iMaxLatitude, L"iMaxLatitude", 90);
	pXML->GetOptionalChildXmlValByName(&m_iMinLongitude, L"iMinLongitude", -180);
	pXML->GetOptionalChildXmlValByName(&m_iMaxLongitude, L"iMaxLongitude", 180);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"rateOverrideDefineName");
	if ( szTextVal.GetLength() != 0 )
	{
		m_iRateOverride = std::max(GC.getDefineINT(szTextVal, 100),0);
	}

	if(pXML->TryMoveToXmlFirstChild(L"BonusTypes"))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"BonusType"))
			{
				do
				{
					int iBonusType;

					pXML->GetXmlVal(szTextVal);
					iBonusType = pXML->GetInfoClass(szTextVal);

					m_bonusTypes.push_back((BonusTypes)iBonusType);
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"TerrainTypes"))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"TerrainType"))
			{
				do
				{
					int iTerrainType;

					pXML->GetXmlVal(szTextVal);
					iTerrainType = pXML->GetInfoClass(szTextVal);

					m_terrainTypes.push_back((TerrainTypes)iTerrainType);
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"FeatureTypes"))
	{
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"FeatureType"))
			{
				do
				{
					int iFeatureType;

					pXML->GetXmlVal(szTextVal);
					iFeatureType = pXML->GetInfoClass(szTextVal);

					m_featureTypes.push_back((FeatureTypes)iFeatureType);
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"FeatureTerrainTypes"))
	{
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"TerrainType"))
			{
				do
				{
					int iTerrainType;

					pXML->GetXmlVal(szTextVal);
					iTerrainType = pXML->GetInfoClass(szTextVal);

					m_featureTerrainTypes.push_back((TerrainTypes)iTerrainType);
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"SpawnGroup"))
	{
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"UnitType"))
			{
				do
				{
					int iUnitType;

					pXML->GetXmlVal(szTextVal);
					iUnitType = pXML->GetInfoClass(szTextVal);

					m_spawnGroup.push_back((UnitTypes)iUnitType);
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpawnCondition"))
	{
		m_pExprSpawnCondition = BoolExpr::read(pXML);
		pXML->MoveToXmlParent();
	}

	return true;
}


void CvSpawnInfo::copyNonDefaults(CvSpawnInfo* pClassInfo)
{
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (!m_pExprSpawnCondition)
	{
		m_pExprSpawnCondition = pClassInfo->m_pExprSpawnCondition;
		pClassInfo->m_pExprSpawnCondition = NULL;
	}

	// not yet implemented
}


const BoolExpr* CvSpawnInfo::getSpawnCondition() const
{
	return m_pExprSpawnCondition;
}


int CvSpawnInfo::getTurnRate() const
{
	return m_iTurns;
}


int CvSpawnInfo::getGlobalTurnRate() const
{
	return m_iGlobalTurns;
}


int CvSpawnInfo::getMaxLocalDensity() const
{
	return m_iMaxLocalDensity;
}


int CvSpawnInfo::getMinAreaPlotsPerPlayerUnit() const
{
	return m_iMinAreaPlotsPerPlayerUnit;
}


int CvSpawnInfo::getMinAreaPlotsPerUnitType() const
{
	return m_iMinAreaPlotsPerUnitType;
}


int CvSpawnInfo::getStartDate() const
{
	return m_iStartDate;
}


int	CvSpawnInfo::getEndDate() const
{
	return m_iEndDate;
}


UnitTypes CvSpawnInfo::getUnitType() const
{
	return m_eUnitType;
}


PlayerTypes	CvSpawnInfo::getPlayer() const
{
	return (PlayerTypes)m_iPlayerType;
}


bool CvSpawnInfo::getTreatAsBarbarian() const
{
	return m_bTreatAsBarbarian;
}


bool CvSpawnInfo::getNeutralOnly() const
{
	if (GC.getUnitInfo(getUnitType()).isWildAnimal() && GC.getGame().isOption(GAMEOPTION_ANIMAL_DANGEROUS))
	{
		return false;
	}
	return m_bNeutralOnly;
}


bool CvSpawnInfo::getNoSpeedNormalization() const
{
	return m_bNoSpeedNormalization;
}


bool CvSpawnInfo::getNotInView() const
{
	return m_bNotInView;
}


bool CvSpawnInfo::getHills() const
{
	return m_bHills;
}


bool CvSpawnInfo::getFlatlands() const
{
	return m_bFlatlands;
}


bool CvSpawnInfo::getPeaks() const
{
	return m_bPeaks;
}


bool CvSpawnInfo::getFreshWaterOnly() const
{
	return m_bFreshWaterOnly;
}


bool CvSpawnInfo::getLatitudeAbs() const
{
	return m_bLatitudeAbs;
}


int CvSpawnInfo::getMinLatitude() const
{
	return m_iMinLatitude;
}


int CvSpawnInfo::getMaxLatitude() const
{
	return m_iMaxLatitude;
}


int CvSpawnInfo::getMinLongitude() const
{
	return m_iMinLongitude;
}


int CvSpawnInfo::getMaxLongitude() const
{
	return m_iMaxLongitude;
}


int CvSpawnInfo::getRateOverride() const
{
	return m_iRateOverride;
}


void CvSpawnInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_eUnitType);
	CheckSum(iSum, m_ePrereqTech);
	CheckSum(iSum, m_eObsoleteTechType);
	CheckSum(iSum, m_iPlayerType);
	CheckSum(iSum, m_iTurns);
	CheckSum(iSum, m_iGlobalTurns);
	CheckSum(iSum, m_iMaxLocalDensity);
	CheckSum(iSum, m_iMinAreaPlotsPerPlayerUnit);
	CheckSum(iSum, m_iMinAreaPlotsPerUnitType);
	CheckSum(iSum, m_iStartDate);
	CheckSum(iSum, m_iEndDate);
	CheckSum(iSum, m_bTreatAsBarbarian);
	CheckSum(iSum, m_bNeutralOnly);
	CheckSum(iSum, m_bNotInView);
	CheckSum(iSum, m_bNoSpeedNormalization);
	CheckSum(iSum, m_bHills);
	CheckSum(iSum, m_bFlatlands);
	CheckSum(iSum, m_bPeaks);
	CheckSum(iSum, m_bFreshWaterOnly);

	CheckSum(iSum, m_bLatitudeAbs);
	CheckSum(iSum, m_iMinLatitude);
	CheckSum(iSum, m_iMaxLatitude);
	CheckSum(iSum, m_iMinLongitude);
	CheckSum(iSum, m_iMaxLongitude);

	CheckSum(iSum, m_iRateOverride);
	CheckSumC(iSum, m_bonusTypes);
	CheckSumC(iSum, m_terrainTypes);
	CheckSumC(iSum, m_featureTypes);
	CheckSumC(iSum, m_featureTerrainTypes);
	CheckSumC(iSum, m_spawnGroup);

	if (m_pExprSpawnCondition)
	{
		m_pExprSpawnCondition->getCheckSum(iSum);
	}
}

