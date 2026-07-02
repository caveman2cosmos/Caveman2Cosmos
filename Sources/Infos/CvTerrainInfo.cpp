//------------------------------------------------------------------------------------------------
//  FILE:    CvTerrainInfo.cpp
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
#include "CvTerrainInfo.h"


//======================================================================================================
//					CvTerrainInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvTerrainInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvTerrainInfo::CvTerrainInfo() :
m_iMovementCost(0),
m_iBuildModifier(0),
m_iDefenseModifier(0),
m_iDistanceToLand(0),
m_eClimate(NO_CLIMATE_ZONE),
m_bImpassable(false),
m_bFound(false),
m_bFoundCoast(false),
m_bFoundFreshWater(false),
m_bFreshWaterTerrain(false),
m_iWorldSoundscapeScriptId(0),
m_piYields(NULL),
m_pi3DAudioScriptFootstepIndex(NULL)
,m_iCultureDistance(0)
,m_iHealthPercent(0)
,m_PropertyManipulators()
{
	m_zobristValue = GC.getGame().getSorenRand().getInt();
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvTerrainInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvTerrainInfo::~CvTerrainInfo()
{
	SAFE_DELETE_ARRAY(m_piYields);
	SAFE_DELETE_ARRAY(m_pi3DAudioScriptFootstepIndex);

}


int CvTerrainInfo::getMovementCost() const
{
	return m_iMovementCost;
}


int CvTerrainInfo::getBuildModifier() const
{
	return m_iBuildModifier;
}


int CvTerrainInfo::getDefenseModifier() const
{
	return m_iDefenseModifier;
}


bool CvTerrainInfo::isImpassable() const
{
	return m_bImpassable;
}


bool CvTerrainInfo::isFound() const
{
	return m_bFound;
}


bool CvTerrainInfo::isFoundCoast() const
{
	return m_bFoundCoast;
}


bool CvTerrainInfo::isFoundFreshWater() const
{
	return m_bFoundFreshWater;
}


bool CvTerrainInfo::isFreshWaterTerrain() const
{
	return m_bFreshWaterTerrain;
}


const char* CvTerrainInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}


int CvTerrainInfo::getWorldSoundscapeScriptId() const
{
	return m_iWorldSoundscapeScriptId;
}


// Arrays

int CvTerrainInfo::getYield(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piYields ? m_piYields[i] : 0;
}


int CvTerrainInfo::get3DAudioScriptFootstepIndex(int i) const
{
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pi3DAudioScriptFootstepIndex ? m_pi3DAudioScriptFootstepIndex[i] : 0;
}


int CvTerrainInfo::getCultureDistance() const
{
	return m_iCultureDistance;
}

int CvTerrainInfo::getHealthPercent() const
{
	return m_iHealthPercent;
}


int CvTerrainInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}


int CvTerrainInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}


bool CvTerrainInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}


bool CvTerrainInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szArtDefineTag, L"ArtDefineTag");

	if (pXML->TryMoveToXmlFirstChild(L"Yields"))
	{
		pXML->SetYields(&m_piYields);
		pXML->MoveToXmlParent();
	}
	else SAFE_DELETE_ARRAY(m_piYields);

	pXML->GetOptionalChildXmlValByName(&m_iDistanceToLand, L"iDistanceToLand");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"ClimateZoneType");
	m_eClimate = (ClimateZoneTypes) pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_bImpassable, L"bImpassable");
	pXML->GetOptionalChildXmlValByName(&m_bFound, L"bFound");
	pXML->GetOptionalChildXmlValByName(&m_bFoundCoast, L"bFoundCoast");
	pXML->GetOptionalChildXmlValByName(&m_bFoundFreshWater, L"bFoundFreshWater");
	pXML->GetOptionalChildXmlValByName(&m_bFreshWaterTerrain, L"bFreshWaterTerrain");

	pXML->GetOptionalChildXmlValByName(&m_iMovementCost, L"iMovement");
	pXML->GetOptionalChildXmlValByName(&m_iBuildModifier, L"iBuildModifier");
	pXML->GetOptionalChildXmlValByName(&m_iDefenseModifier, L"iDefense");

	pXML->SetVariableListTagPairForAudioScripts(&m_pi3DAudioScriptFootstepIndex, L"FootstepSounds", GC.getNumFootstepAudioTypes());

	pXML->SetOptionalVector(&m_aiCategories, L"Categories");
	pXML->SetOptionalVector(&m_aeMapCategoryTypes, L"MapCategoryTypes");

	if (pXML->GetOptionalChildXmlValByName(szTextVal, L"WorldSoundscapeAudioScript"))
		m_iWorldSoundscapeScriptId = gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_SOUNDSCAPE );
	else
		m_iWorldSoundscapeScriptId = -1;

	pXML->GetOptionalChildXmlValByName(&m_iCultureDistance, L"iCultureDistance");
	pXML->GetOptionalChildXmlValByName(&m_iHealthPercent, L"iHealthPercent");

	m_PropertyManipulators.read(pXML);
	return true;
}


void CvTerrainInfo::copyNonDefaults(const CvTerrainInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();

	if (getArtDefineTag() == cDefault) m_szArtDefineTag = pClassInfo->getArtDefineTag();

	CvInfoBase::copyNonDefaults(pClassInfo);

	for ( int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		if (!m_piYields)
		{
			if (pClassInfo->getYield(i))
			{
				CvXMLLoadUtility::InitList(&m_piYields, NUM_YIELD_TYPES);
			}
		}

		if (m_piYields && (m_piYields[i] == iDefault))
		{
			m_piYields[i] = pClassInfo->getYield(i);
		}
	}
	if (m_iDistanceToLand == 0) m_iDistanceToLand = pClassInfo->isWaterTerrain();
	if (m_eClimate == CLIMATE_ZONE_TEMPERATE) m_eClimate = pClassInfo->getClimate();
	if (isImpassable() == bDefault) m_bImpassable = pClassInfo->isImpassable();
	if (isFound() == bDefault) m_bFound = pClassInfo->isFound();
	if (isFoundCoast() == bDefault) m_bFoundCoast = pClassInfo->isFoundCoast();
	if (isFoundFreshWater() == bDefault) m_bFoundFreshWater = pClassInfo->isFoundFreshWater();
	if (isFreshWaterTerrain() == bDefault) m_bFreshWaterTerrain = pClassInfo->isFreshWaterTerrain();
	if (getMovementCost() == iDefault) m_iMovementCost = pClassInfo->getMovementCost();
	if (getBuildModifier() == iDefault) m_iBuildModifier = pClassInfo->getBuildModifier();
	if (getDefenseModifier() == iDefault) m_iDefenseModifier = pClassInfo->getDefenseModifier();

	for ( int i = 0; i < GC.getNumFootstepAudioTypes(); i++)
	{
		if (get3DAudioScriptFootstepIndex(i) == -1 && pClassInfo->get3DAudioScriptFootstepIndex(i) != -1)
		{
			if ( NULL == m_pi3DAudioScriptFootstepIndex )
			{
				CvXMLLoadUtility::InitList(&m_pi3DAudioScriptFootstepIndex,GC.getNumFootstepAudioTypes(),-1);
			}
			m_pi3DAudioScriptFootstepIndex[i] = pClassInfo->get3DAudioScriptFootstepIndex(i);
		}
	}

	if (getWorldSoundscapeScriptId() == iTextDefault) m_iWorldSoundscapeScriptId = pClassInfo->getWorldSoundscapeScriptId();

	if (getCultureDistance() == iDefault) m_iCultureDistance = pClassInfo->getCultureDistance();
	if (getHealthPercent() == iDefault) m_iHealthPercent = pClassInfo->getHealthPercent();

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aeMapCategoryTypes, pClassInfo->getMapCategories());
	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);

}



void CvTerrainInfo::getCheckSum(uint32_t &iSum) const
{
	PROFILE_EXTRA_FUNC();
	CheckSum(iSum, m_iMovementCost);
	CheckSum(iSum, m_iBuildModifier);
	CheckSum(iSum, m_iDefenseModifier);
	CheckSum(iSum, m_iDistanceToLand);
	CheckSum(iSum, m_eClimate);

	CheckSum(iSum, m_bImpassable);
	CheckSum(iSum, m_bFound);
	CheckSum(iSum, m_bFoundCoast);
	CheckSum(iSum, m_bFoundFreshWater);
	CheckSum(iSum, m_bFreshWaterTerrain);

	// Arrays

	CheckSum(iSum, m_piYields, NUM_YIELD_TYPES);

	CheckSum(iSum, m_iCultureDistance);
	CheckSum(iSum, m_iHealthPercent);

	m_PropertyManipulators.getCheckSum(iSum);
	//TB Combat Mods begin
	CheckSumC(iSum, m_aiCategories);
	CheckSumC(iSum, m_aeMapCategoryTypes);
	//TB Combat Mods end
}


const char* CvTerrainInfo::getButton() const
{
	const CvArtInfoTerrain* pTerrainArtInfo = getArtInfo();
	return pTerrainArtInfo ? pTerrainArtInfo->getButton() : NULL;
}


const CvArtInfoTerrain* CvTerrainInfo::getArtInfo() const
{
	return ARTFILEMGR.getTerrainArtInfo(getArtDefineTag());
}

