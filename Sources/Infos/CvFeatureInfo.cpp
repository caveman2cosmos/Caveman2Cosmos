//------------------------------------------------------------------------------------------------
//  FILE:    CvFeatureInfo.cpp
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
#include "CvFeatureInfo.h"


//======================================================================================================
//					CvBonusInfo
//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvBonusInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------



//======================================================================================================
//					CvFeatureInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvFeatureInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvFeatureInfo::CvFeatureInfo() :
m_iMovementCost(0),
m_iSeeThroughChange(0),
m_iHealthPercent(0),
m_iAppearanceProbability(0),
m_iDisappearanceProbability(0),
m_iGrowthProbability(0),
m_iDefenseModifier(0),
m_iAdvancedStartRemoveCost(0),
m_iTurnDamage(0),
m_iWarmingDefense(0), //GWMod new xml field M.A.
m_bNoCoast(false),
m_bNoRiver(false),
m_bNoAdjacent(false),
m_bRequiresFlatlands(false),
m_bRequiresRiver(false),
m_bCoastalOnly(false),
m_bAddsFreshWater(false),
m_bImpassable(false),
m_bNoCity(false),
m_bNoImprovement(false),
m_bNoBonus(false),
m_bVisibleAlways(false),
m_bNukeImmune(false),
m_bCountsAsPeak(false),
m_iWorldSoundscapeScriptId(0),
m_iEffectProbability(0),
m_piYieldChange(NULL),
m_piRiverYieldChange(NULL),
m_pi3DAudioScriptFootstepIndex(NULL),
m_pbTerrain(NULL)
,m_iSpreadProbability(0)
,m_iCultureDistance(0)
,m_iPopDestroys(-1)
,m_bIgnoreTerrainCulture(false)
,m_bCanGrowAnywhere(false)
,m_PropertyManipulators()
{
	m_zobristValue = GC.getGame().getSorenRand().getInt();
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvFeatureInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvFeatureInfo::~CvFeatureInfo()
{
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piRiverYieldChange);
	SAFE_DELETE_ARRAY(m_pi3DAudioScriptFootstepIndex);
	SAFE_DELETE_ARRAY(m_pbTerrain);

}


int CvFeatureInfo::getMovementCost() const
{
	return m_iMovementCost;
}


int CvFeatureInfo::getSeeThroughChange() const
{
	return m_iSeeThroughChange;
}


int CvFeatureInfo::getHealthPercent() const
{
	return m_iHealthPercent;
}


int CvFeatureInfo::getAppearanceProbability() const
{
	return m_iAppearanceProbability;
}


int CvFeatureInfo::getDisappearanceProbability() const
{
	return m_iDisappearanceProbability;
}


int CvFeatureInfo::getGrowthProbability() const
{
	return m_iGrowthProbability;
}


int CvFeatureInfo::getDefenseModifier() const
{
	return m_iDefenseModifier;
}


int CvFeatureInfo::getAdvancedStartRemoveCost() const
{
	return m_iAdvancedStartRemoveCost;
}


int CvFeatureInfo::getTurnDamage() const
{
	return m_iTurnDamage;
}


int CvFeatureInfo::getWarmingDefense() const			//GWMod new xml field M.A.
{
	return m_iWarmingDefense;
}


int CvFeatureInfo::getPopDestroys() const			//GWMod new xml field M.A.
{
	return m_iPopDestroys;
}


bool CvFeatureInfo::isNoCoast() const
{
	return m_bNoCoast;
}


bool CvFeatureInfo::isNoRiver() const
{
	return m_bNoRiver;
}


bool CvFeatureInfo::isNoAdjacent() const
{
	return m_bNoAdjacent;
}


bool CvFeatureInfo::isRequiresFlatlands() const
{
	return m_bRequiresFlatlands;
}


bool CvFeatureInfo::isRequiresRiver() const
{
	return m_bRequiresRiver;
}


bool CvFeatureInfo::isAddsFreshWater() const
{
	return m_bAddsFreshWater;
}


bool CvFeatureInfo::isImpassable() const
{
	return m_bImpassable;
}


bool CvFeatureInfo::isNoCity() const
{
	return m_bNoCity;
}


bool CvFeatureInfo::isNoImprovement() const
{
	return m_bNoImprovement;
}


bool CvFeatureInfo::isVisibleAlways() const
{
	return m_bVisibleAlways;
}


bool CvFeatureInfo::isNukeImmune() const
{
	return m_bNukeImmune;
}


bool CvFeatureInfo::isCountsAsPeak() const
{
	return m_bCountsAsPeak;
}


// BUG - City Plot Status - start
bool CvFeatureInfo::isOnlyBad() const
{
	PROFILE_EXTRA_FUNC();
	if (getHealthPercent() > 0 || isAddsFreshWater())
	{
		return false;
	}
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (getYieldChange(iI) > 0)
		{
			return false;
		}
	}

	return true;
}

// BUG - City Plot Status - end

const char* CvFeatureInfo::getOnUnitChangeTo() const
{
	return m_szOnUnitChangeTo;
}


const char* CvFeatureInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}


int CvFeatureInfo::getWorldSoundscapeScriptId() const
{
	return m_iWorldSoundscapeScriptId;
}


const char* CvFeatureInfo::getEffectType() const
{
	return m_szEffectType;
}


int CvFeatureInfo::getEffectProbability() const
{
	return m_iEffectProbability;
}


// Arrays

int CvFeatureInfo::getYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}


int CvFeatureInfo::getRiverYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piRiverYieldChange ? m_piRiverYieldChange[i] : 0;
}


int CvFeatureInfo::get3DAudioScriptFootstepIndex(int i) const
{
	//	FAssertMsg(i < ?, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pi3DAudioScriptFootstepIndex ? m_pi3DAudioScriptFootstepIndex[i] : -1;
}


bool CvFeatureInfo::isTerrain(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), i);
	return m_pbTerrain ? m_pbTerrain[i] : false;
}


int CvFeatureInfo::getNumVarieties() const
{
	return getArtInfo()->getNumVarieties();
}


const char* CvFeatureInfo::getButton() const
{
	const CvArtInfoFeature* pFeatureArtInfo = getArtInfo();
	return pFeatureArtInfo ? pFeatureArtInfo->getButton() : NULL;
}


const CvArtInfoFeature* CvFeatureInfo::getArtInfo() const
{
	return ARTFILEMGR.getFeatureArtInfo( getArtDefineTag());
}


int CvFeatureInfo::getSpreadProbability() const
{
	return m_iSpreadProbability;
}


int CvFeatureInfo::getCultureDistance() const
{
	return m_iCultureDistance;
}


const char* CvFeatureInfo::getGrowthSound() const
{
	return m_szGrowthSound;
}


bool CvFeatureInfo::isIgnoreTerrainCulture() const
{
	return m_bIgnoreTerrainCulture;
}


bool CvFeatureInfo::isCanGrowAnywhere() const
{
	return m_bCanGrowAnywhere;
}


// AIAndy: Returns true if the feature can be displayed properly as secondary feature
bool CvFeatureInfo::canBeSecondary() const
{
	return !(getArtInfo()->isRiverArt() || (getArtInfo()->getTileArtType() != TILE_ART_TYPE_NONE));
}


int CvFeatureInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}


int CvFeatureInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}


bool CvFeatureInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}


bool CvFeatureInfo::read(CvXMLLoadUtility* pXML)
{

	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szArtDefineTag, L"ArtDefineTag");

	if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
	{
		pXML->SetYields(&m_piYieldChange);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_piYieldChange);

	if (pXML->TryMoveToXmlFirstChild(L"RiverYieldChange"))
	{
		pXML->SetYields(&m_piRiverYieldChange);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_piRiverYieldChange);

	pXML->GetOptionalChildXmlValByName(&m_iMovementCost, L"iMovement");
	pXML->GetOptionalChildXmlValByName(&m_iSeeThroughChange, L"iSeeThrough");
	pXML->GetOptionalChildXmlValByName(&m_iHealthPercent, L"iHealthPercent");
	pXML->GetOptionalChildXmlValByName(&m_iDefenseModifier, L"iDefense");
	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartRemoveCost, L"iAdvancedStartRemoveCost");
	pXML->GetOptionalChildXmlValByName(&m_iTurnDamage, L"iTurnDamage");
	pXML->GetOptionalChildXmlValByName(&m_iWarmingDefense, L"iWarmingDefense"); //GWMod new xml field M.A.
	pXML->GetOptionalChildXmlValByName(&m_iAppearanceProbability, L"iAppearance");
	pXML->GetOptionalChildXmlValByName(&m_iDisappearanceProbability, L"iDisappearance");
	pXML->GetOptionalChildXmlValByName(&m_iGrowthProbability, L"iGrowth");
	pXML->GetOptionalChildXmlValByName(&m_iPopDestroys, L"iPopDestroys", -1);
	pXML->GetOptionalChildXmlValByName(&m_bNoCoast, L"bNoCoast");
	pXML->GetOptionalChildXmlValByName(&m_bNoRiver, L"bNoRiver");
	pXML->GetOptionalChildXmlValByName(&m_bNoAdjacent, L"bNoAdjacent");
	pXML->GetOptionalChildXmlValByName(&m_bRequiresFlatlands, L"bRequiresFlatlands");
	pXML->GetOptionalChildXmlValByName(&m_bRequiresRiver, L"bRequiresRiver");
	pXML->GetOptionalChildXmlValByName(&m_bCoastalOnly, L"bCoastalOnly");
	pXML->GetOptionalChildXmlValByName(&m_bAddsFreshWater, L"bAddsFreshWater");
	pXML->GetOptionalChildXmlValByName(&m_bImpassable, L"bImpassable");
	pXML->GetOptionalChildXmlValByName(&m_bNoCity, L"bNoCity");
	pXML->GetOptionalChildXmlValByName(&m_bNoImprovement, L"bNoImprovement");
	pXML->GetOptionalChildXmlValByName(&m_bNoBonus, L"bNoBonus");
	pXML->GetOptionalChildXmlValByName(&m_bVisibleAlways, L"bVisibleAlways");
	pXML->GetOptionalChildXmlValByName(&m_bNukeImmune, L"bNukeImmune");
	pXML->GetOptionalChildXmlValByName(&m_bCountsAsPeak, L"bCountsAsPeak");
	pXML->GetOptionalChildXmlValByName(m_szOnUnitChangeTo, L"OnUnitChangeTo");

	pXML->SetVariableListTagPairForAudioScripts(&m_pi3DAudioScriptFootstepIndex, L"FootstepSounds", GC.getNumFootstepAudioTypes());

	pXML->GetOptionalChildXmlValByName(szTextVal, L"WorldSoundscapeAudioScript");
	if ( szTextVal.GetLength() > 0 )
		m_iWorldSoundscapeScriptId = gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_SOUNDSCAPE );
	else
		m_iWorldSoundscapeScriptId = -1;

	pXML->GetOptionalChildXmlValByName(m_szEffectType, L"EffectType");
	pXML->GetOptionalChildXmlValByName(&m_iEffectProbability, L"iEffectProbability");

	pXML->SetVariableListTagPair(&m_pbTerrain, L"TerrainBooleans", GC.getNumTerrainInfos());

	pXML->GetOptionalChildXmlValByName(&m_iSpreadProbability, L"iSpread");
	pXML->GetOptionalChildXmlValByName(&m_iCultureDistance, L"iCultureDistance");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreTerrainCulture, L"bIgnoreTerrainCulture");
	pXML->GetOptionalChildXmlValByName(&m_bCanGrowAnywhere, L"bCanGrowAnywhere");
	pXML->GetOptionalChildXmlValByName(m_szGrowthSound, L"GrowthSound");
	pXML->SetOptionalVector(&m_aiCategories, L"Categories");
	pXML->SetOptionalVector(&m_aeMapCategoryTypes, L"MapCategoryTypes");

	m_PropertyManipulators.read(pXML);
	return true;
}


void CvFeatureInfo::copyNonDefaults(const CvFeatureInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();

	if (getArtDefineTag() == cDefault) m_szArtDefineTag = pClassInfo->getArtDefineTag();

	CvInfoBase::copyNonDefaults(pClassInfo);

	for ( int i = 0; i < NUM_YIELD_TYPES; i++ )
	{
		if ( getYieldChange(i) == iDefault && pClassInfo->getYieldChange(i) != iDefault)
		{
			if ( m_piYieldChange == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piYieldChange, NUM_YIELD_TYPES);
			}
			m_piYieldChange[i] = pClassInfo->getYieldChange(i);
		}
		if ( getRiverYieldChange(i) == iDefault && pClassInfo->getRiverYieldChange(i) != iDefault)
		{
			if ( m_piYieldChange == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piRiverYieldChange, NUM_YIELD_TYPES);
			}
			m_piRiverYieldChange[i] = pClassInfo->getRiverYieldChange(i);
		}
	}

	if (getMovementCost() == iDefault) m_iMovementCost = pClassInfo->getMovementCost();
	if (getSeeThroughChange() == iDefault) m_iSeeThroughChange = pClassInfo->getSeeThroughChange();
	if (getHealthPercent() == iDefault) m_iHealthPercent = pClassInfo->getHealthPercent();
	if (getDefenseModifier() == iDefault) m_iDefenseModifier = pClassInfo->getDefenseModifier();
	if (getAdvancedStartRemoveCost() == iDefault) m_iAdvancedStartRemoveCost = pClassInfo->getAdvancedStartRemoveCost();
	if (getTurnDamage() == iDefault) m_iTurnDamage = pClassInfo->getTurnDamage();
	if (getAppearanceProbability() == iDefault) m_iAppearanceProbability = pClassInfo->getAppearanceProbability();
	if (getDisappearanceProbability() == iDefault) m_iDisappearanceProbability = pClassInfo->getDisappearanceProbability();
	if (getGrowthProbability() == iDefault) m_iGrowthProbability = pClassInfo->getGrowthProbability();
	if (getPopDestroys() == -1) m_iPopDestroys = pClassInfo->getPopDestroys();
	if (isNoCoast() == bDefault) m_bNoCoast = pClassInfo->isNoCoast();
	if (isNoRiver() == bDefault) m_bNoRiver = pClassInfo->isNoRiver();
	if (isNoAdjacent() == bDefault) m_bNoAdjacent = pClassInfo->isNoAdjacent();
	if (isRequiresFlatlands() == bDefault) m_bRequiresFlatlands = pClassInfo->isRequiresFlatlands();
	if (isRequiresRiver() == bDefault) m_bRequiresRiver = pClassInfo->isRequiresRiver();
	if (m_bCoastalOnly == bDefault) m_bCoastalOnly = pClassInfo->isCoastalOnly();
	if (isAddsFreshWater() == bDefault) m_bAddsFreshWater = pClassInfo->isAddsFreshWater();
	if (isImpassable() == bDefault) m_bImpassable = pClassInfo->isImpassable();
	if (isNoCity() == bDefault) m_bNoCity = pClassInfo->isNoCity();
	if (isNoImprovement() == bDefault) m_bNoImprovement = pClassInfo->isNoImprovement();
	if (m_bNoBonus == bDefault) m_bNoBonus = pClassInfo->isNoBonus();
	if (isVisibleAlways() == bDefault) m_bVisibleAlways = pClassInfo->isVisibleAlways();
	if (isNukeImmune() == bDefault) m_bNukeImmune = pClassInfo->isNukeImmune();
	if (isCountsAsPeak() == bDefault) m_bCountsAsPeak = pClassInfo->isCountsAsPeak();

	if (getOnUnitChangeTo() == cDefault) m_szOnUnitChangeTo = pClassInfo->getOnUnitChangeTo();

	for ( int i = 0; i < GC.getNumFootstepAudioTypes(); i++ )
	{
		if ( get3DAudioScriptFootstepIndex(i) == iDefault && pClassInfo->get3DAudioScriptFootstepIndex(i) != iDefault)
		{
			if ( NULL == m_pi3DAudioScriptFootstepIndex )
			{
				CvXMLLoadUtility::InitList(&m_pi3DAudioScriptFootstepIndex,GC.getNumFootstepAudioTypes(),iDefault);
			}
			m_pi3DAudioScriptFootstepIndex[i] = pClassInfo->get3DAudioScriptFootstepIndex(i);
		}
	}
	if (getWorldSoundscapeScriptId() == iTextDefault) m_iWorldSoundscapeScriptId = pClassInfo->getWorldSoundscapeScriptId();

	if (getEffectType() == cDefault) m_szEffectType = pClassInfo->getEffectType();
	if (getEffectProbability() == iDefault) m_iEffectProbability = pClassInfo->getEffectProbability();

	for ( int i = 0; i < GC.getNumTerrainInfos(); i++ )
	{
		if ( isTerrain(i) == bDefault && pClassInfo->isTerrain(i) != bDefault )
		{
			if ( m_pbTerrain == NULL )
			{
				CvXMLLoadUtility::InitList(&m_pbTerrain, GC.getNumTerrainInfos());
			}

			m_pbTerrain[i] = pClassInfo->isTerrain(i);
		}
	}

	if (getSpreadProbability() == iDefault) m_iSpreadProbability = pClassInfo->getSpreadProbability();
	if (getCultureDistance() == iDefault) m_iCultureDistance = pClassInfo->getCultureDistance();
	if (!isIgnoreTerrainCulture()) m_bIgnoreTerrainCulture = pClassInfo->isIgnoreTerrainCulture();

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aeMapCategoryTypes, pClassInfo->getMapCategories());

	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);
}


void CvFeatureInfo::getCheckSum(uint32_t &iSum) const
{
	PROFILE_EXTRA_FUNC();
	CheckSum(iSum, m_iSpreadProbability);
	CheckSum(iSum, m_iCultureDistance);
	CheckSum(iSum, m_bIgnoreTerrainCulture);
	CheckSum(iSum, m_bCanGrowAnywhere);
	CheckSum(iSum, m_iMovementCost);
	CheckSum(iSum, m_iSeeThroughChange);
	CheckSum(iSum, m_iHealthPercent);
	CheckSum(iSum, m_iAppearanceProbability);
	CheckSum(iSum, m_iDisappearanceProbability);
	CheckSum(iSum, m_iGrowthProbability);
	CheckSum(iSum, m_iDefenseModifier);
	CheckSum(iSum, m_iAdvancedStartRemoveCost);
	CheckSum(iSum, m_iTurnDamage);
	CheckSum(iSum, m_iWarmingDefense);
	CheckSum(iSum, m_iPopDestroys);

	CheckSum(iSum, m_bNoCoast);
	CheckSum(iSum, m_bNoRiver);
	CheckSum(iSum, m_bNoAdjacent);
	CheckSum(iSum, m_bRequiresFlatlands);
	CheckSum(iSum, m_bRequiresRiver);
	CheckSum(iSum, m_bCoastalOnly);
	CheckSum(iSum, m_bAddsFreshWater);
	CheckSum(iSum, m_bImpassable);
	CheckSum(iSum, m_bNoCity);
	CheckSum(iSum, m_bNoImprovement);
	CheckSum(iSum, m_bNoBonus);
	CheckSum(iSum, m_bVisibleAlways);
	CheckSum(iSum, m_bNukeImmune);
	CheckSum(iSum, m_bCountsAsPeak);
	CheckSumC(iSum, m_szOnUnitChangeTo);
	CheckSumC(iSum, m_aeMapCategoryTypes);
	CheckSumC(iSum, m_aiCategories);

	// Arrays

	CheckSum(iSum, m_piYieldChange, NUM_YIELD_TYPES);
	CheckSum(iSum, m_piRiverYieldChange, NUM_YIELD_TYPES);

	CheckSum(iSum, m_pbTerrain, GC.getNumTerrainInfos());

	m_PropertyManipulators.getCheckSum(iSum);
}

