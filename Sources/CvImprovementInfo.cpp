#include "CvArtFileMgr.h"
#include "CvGlobals.h"
#include "CvImprovementInfo.h"
#include "CvXMLLoadUtility.h"
#include "CheckSum.h"

CvImprovementInfo::CvImprovementInfo() :
	m_iAdvancedStartCost(100),
	m_iTilesPerGoody(0),
	m_iGoodyUniqueRange(0),
	m_iFeatureGrowthProbability(0),
	m_iUpgradeTime(0),
	m_iAirBombDefense(0),
	m_iDefenseModifier(0),
	m_iHappiness(0),
	m_iPillageGold(0),
	m_iImprovementPillage(NO_IMPROVEMENT),
	m_iImprovementUpgrade(NO_IMPROVEMENT),
	// Super Forts begin *XML*
	m_iCulture(0),
	m_iCultureRange(0),
	m_iVisibilityChange(0),
	m_iSeeFrom(0),
	m_iUniqueRange(0),
	m_bBombardable(false),
	m_bUpgradeRequiresFortify(false),
	// Super Forts end
	// Super forts C2C adaptation
	m_bIsUniversalTradeBonusProvider(false),
	m_bIsZOCSource(false),
	// Super forts C2C adaptation end
	m_bActsAsCity(false),
	m_bHillsMakesValid(false),
	m_bFreshWaterMakesValid(false),
	m_bRiverSideMakesValid(false),
	m_bNoFreshWater(false),
	m_bRequiresFlatlands(false),
	m_bRequiresRiverSide(false),
	m_bRequiresIrrigation(false),
	m_bCarriesIrrigation(false),
	m_bRequiresFeature(false),
	m_bPeakImprovement(false),
	m_bWaterImprovement(false),
	m_bGoody(false),
	m_bPermanent(false),
	m_bOutsideBorders(false),
	m_iWorldSoundscapeScriptId(0),
	m_piPrereqNatureYield(NULL),
	m_piYieldChange(NULL),
	m_piRiverSideYieldChange(NULL),
	m_piIrrigatedChange(NULL),
	m_pbTerrainMakesValid(NULL),
	m_pbFeatureMakesValid(NULL),
	m_ppiTechYieldChanges(NULL),
	m_ppiRouteYieldChanges(NULL),
	m_paImprovementBonus(NULL)

	, m_bPeakMakesValid(false)
	, m_iHealthPercent(0)
	, m_iPrereqTech(NO_TECH)
	//,m_ppiTraitYieldChanges(NULL)

	, m_PropertyManipulators()
	//TB Improvements
	, m_bCanMoveSeaUnits(true)
	, m_bChangeRemove(false)
	, m_bNotOnAnyBonus(false)
	, m_bNational(false)
	, m_bGlobal(false)
	//,m_iHighestCost(0)
	, m_iBonusChange(NO_BONUS)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvImprovementInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvImprovementInfo::~CvImprovementInfo()
{
	SAFE_DELETE_ARRAY(m_piPrereqNatureYield);
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piRiverSideYieldChange);
	SAFE_DELETE_ARRAY(m_piIrrigatedChange);
	SAFE_DELETE_ARRAY(m_pbTerrainMakesValid);
	SAFE_DELETE_ARRAY(m_pbFeatureMakesValid);
	SAFE_DELETE_ARRAY(m_paImprovementBonus); // XXX make sure this isn't leaking memory...
	SAFE_DELETE_ARRAY2(m_ppiTechYieldChanges, GC.getNumTechInfos());
	SAFE_DELETE_ARRAY2(m_ppiRouteYieldChanges, GC.getNumRouteInfos());
	//	SAFE_DELETE_ARRAY2(m_ppiTraitYieldChanges, GC.getNumTraitInfos());
	GC.removeDelayedResolution((int*)&m_iImprovementPillage);
	GC.removeDelayedResolution((int*)&m_iImprovementUpgrade);
	GC.removeDelayedResolution((int*)&m_iBonusChange);
	GC.removeDelayedResolutionVector(m_aiAlternativeImprovementUpgradeTypes);
	GC.removeDelayedResolutionVector(m_aiFeatureChangeTypes);
}

int CvImprovementInfo::getAdvancedStartCost() const
{
	return m_iAdvancedStartCost;
}

int CvImprovementInfo::getTilesPerGoody() const
{
	return m_iTilesPerGoody;
}

int CvImprovementInfo::getGoodyUniqueRange() const
{
	return m_iGoodyUniqueRange;
}

int CvImprovementInfo::getFeatureGrowthProbability() const
{
	return m_iFeatureGrowthProbability;
}

int CvImprovementInfo::getUpgradeTime() const
{
	return m_iUpgradeTime;
}

int CvImprovementInfo::getAirBombDefense() const
{
	return m_iAirBombDefense;
}

int CvImprovementInfo::getDefenseModifier() const
{
	return m_iDefenseModifier;
}

int CvImprovementInfo::getHappiness() const
{
	return m_iHappiness;
}

int CvImprovementInfo::getPillageGold() const
{
	return m_iPillageGold;
}

bool CvImprovementInfo::isOutsideBorders() const
{
	return m_bOutsideBorders;
}

// Super Forts begin *XML*
int CvImprovementInfo::getCulture() const
{
	return m_iCulture;
}

int CvImprovementInfo::getCultureRange() const
{
	return m_iCultureRange;
}

int CvImprovementInfo::getVisibilityChange() const
{
	return m_iVisibilityChange;
}

int CvImprovementInfo::getSeeFrom() const
{
	return m_iSeeFrom;
}

int CvImprovementInfo::getUniqueRange() const
{
	return m_iUniqueRange;
}

bool CvImprovementInfo::isBombardable() const
{
	return m_bBombardable;
}

bool CvImprovementInfo::isUpgradeRequiresFortify() const
{
	return m_bUpgradeRequiresFortify;
}
// Super Forts end
// Super forts C2C adaptation
bool CvImprovementInfo::isUniversalTradeBonusProvider() const
{
	return m_bIsUniversalTradeBonusProvider;
}

bool CvImprovementInfo::isZOCSource() const
{
	return m_bIsZOCSource;
}

// Super forts C2C adaptation end

bool CvImprovementInfo::isActsAsCity() const
{
	return m_bActsAsCity;
}

bool CvImprovementInfo::isHillsMakesValid() const
{
	return m_bHillsMakesValid;
}

bool CvImprovementInfo::isFreshWaterMakesValid() const
{
	return m_bFreshWaterMakesValid;
}

bool CvImprovementInfo::isRiverSideMakesValid() const
{
	return m_bRiverSideMakesValid;
}

bool CvImprovementInfo::isNoFreshWater() const
{
	return m_bNoFreshWater;
}

bool CvImprovementInfo::isRequiresFlatlands() const
{
	return m_bRequiresFlatlands;
}

bool CvImprovementInfo::isRequiresRiverSide() const
{
	return m_bRequiresRiverSide;
}

bool CvImprovementInfo::isRequiresIrrigation() const
{
	return m_bRequiresIrrigation;
}

bool CvImprovementInfo::isCarriesIrrigation() const
{
	return m_bCarriesIrrigation;
}

bool CvImprovementInfo::isRequiresFeature() const
{
	return m_bRequiresFeature;
}

bool CvImprovementInfo::isPeakImprovement() const
{
	return m_bPeakImprovement;
}

bool CvImprovementInfo::isWaterImprovement() const
{
	return m_bWaterImprovement;
}

bool CvImprovementInfo::isGoody() const
{
	return m_bGoody;
}

bool CvImprovementInfo::isPermanent() const
{
	return m_bPermanent;
}

const char* CvImprovementInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}

int CvImprovementInfo::getWorldSoundscapeScriptId() const
{
	return m_iWorldSoundscapeScriptId;
}

// Arrays

int CvImprovementInfo::getPrereqNatureYield(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piPrereqNatureYield ? m_piPrereqNatureYield[i] : 0;
}

int CvImprovementInfo::getYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}

int* CvImprovementInfo::getYieldChangeArray() const
{
	return m_piYieldChange;
}

int CvImprovementInfo::getRiverSideYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piRiverSideYieldChange ? m_piRiverSideYieldChange[i] : 0;
}

int* CvImprovementInfo::getRiverSideYieldChangeArray() const
{
	return m_piRiverSideYieldChange;
}

int CvImprovementInfo::getIrrigatedYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piIrrigatedChange ? m_piIrrigatedChange[i] : 0;
}

int* CvImprovementInfo::getIrrigatedYieldChangeArray() const
{
	return m_piIrrigatedChange;
}

bool CvImprovementInfo::getTerrainMakesValid(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), i);
	return m_pbTerrainMakesValid ? m_pbTerrainMakesValid[i] : false;
}

bool CvImprovementInfo::getFeatureMakesValid(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), i);
	return m_pbFeatureMakesValid ? m_pbFeatureMakesValid[i] : false;
}

int CvImprovementInfo::getTechYieldChanges(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), i);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, j);
	return (m_ppiTechYieldChanges && m_ppiTechYieldChanges[i]) ? m_ppiTechYieldChanges[i][j] : 0;
}

int* CvImprovementInfo::getTechYieldChangesArray(int i) const
{
	return m_ppiTechYieldChanges ? m_ppiTechYieldChanges[i] : NULL;
}

int CvImprovementInfo::getRouteYieldChanges(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumRouteInfos(), i);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, j);
	return (m_ppiRouteYieldChanges && m_ppiRouteYieldChanges[i]) ? m_ppiRouteYieldChanges[i][j] : 0;
}

int* CvImprovementInfo::getRouteYieldChangesArray(int i) const
{
	return m_ppiRouteYieldChanges ? m_ppiRouteYieldChanges[i] : NULL;
}

int CvImprovementInfo::getImprovementBonusYield(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, j);
	return m_paImprovementBonus[i].m_piYieldChange ? m_paImprovementBonus[i].getYieldChange(j) : 0;
}

bool CvImprovementInfo::isImprovementBonusMakesValid(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i);
	return m_paImprovementBonus[i].m_bBonusMakesValid;
}

bool CvImprovementInfo::isImprovementObsoleteBonusMakesValid(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i);
	return m_paImprovementBonus[i].m_bObsoleteBonusMakesValid;
}

bool CvImprovementInfo::isImprovementBonusTrade(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i);
	// Super forts C2C adaptation
	return m_bIsUniversalTradeBonusProvider || m_paImprovementBonus[i].m_bBonusTrade;
	// Super forts C2C adaptation end
}

int CvImprovementInfo::getImprovementBonusDiscoverRand(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i);
	return m_paImprovementBonus[i].m_iDiscoverRand;
}

const char* CvImprovementInfo::getButton() const
{
	/************************************************************************************************/
	/* XMLCOPY								 10/25/07								MRGENIE	  */
	/*																							  */
	/* Catch non-existing tag																	   */
	/************************************************************************************************/
	const CvString cDefault = CvString::format("").GetCString();
	if (getArtDefineTag() == cDefault)
	{
		return NULL;
	}
	const CvArtInfoImprovement* pImprovementArtInfo = getArtInfo();
	if (pImprovementArtInfo != NULL)
	{
		return pImprovementArtInfo->getButton();
	}
	return NULL;
}

const CvArtInfoImprovement* CvImprovementInfo::getArtInfo() const
{
	return ARTFILEMGR.getImprovementArtInfo(getArtDefineTag());
}

const char* CvArtInfoImprovement::getShaderNIF() const
{
	return m_szShaderNIF;
}

void CvArtInfoImprovement::setShaderNIF(const char* szDesc)
{
	m_szShaderNIF = szDesc;
}


bool CvImprovementInfo::isPeakMakesValid() const
{
	return m_bPeakMakesValid;
}

int CvImprovementInfo::getHealthPercent() const
{
	return m_iHealthPercent;
}

int CvImprovementInfo::getImprovementBonusDepletionRand(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i);
	return m_paImprovementBonus[i].m_iDepletionRand;
}

//int CvImprovementInfo::getTraitYieldChanges(int i, int j) const
//{
//	FASSERT_BOUNDS(0, GC.getNumTraitInfos(), i);
//	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, j);
//	return (m_ppiTraitYieldChanges && m_ppiTraitYieldChanges[i]) ? m_ppiTraitYieldChanges[i][j] : 0;
//}

//int* CvImprovementInfo::getTraitYieldChangesArray(int i) const
//{
//	return m_ppiTraitYieldChanges ? m_ppiTraitYieldChanges[i] : NULL;
//}

bool CvImprovementInfo::isCanMoveSeaUnits() const
{
	return m_bCanMoveSeaUnits;
}

bool CvImprovementInfo::isChangeRemove() const
{
	return m_bChangeRemove;
}

bool CvImprovementInfo::isNotOnAnyBonus() const
{
	return m_bNotOnAnyBonus;
}

bool CvImprovementInfo::isNational() const
{
	return m_bNational;
}

bool CvImprovementInfo::isGlobal() const
{
	return m_bGlobal;
}

int CvImprovementInfo::getAlternativeImprovementUpgradeType(int i) const
{
	return m_aiAlternativeImprovementUpgradeTypes[i];
}

int CvImprovementInfo::getNumAlternativeImprovementUpgradeTypes() const
{
	return (int)m_aiAlternativeImprovementUpgradeTypes.size();
}

bool CvImprovementInfo::isAlternativeImprovementUpgradeType(int i) const
{
	return algo::any_of_equal(m_aiAlternativeImprovementUpgradeTypes, i);
}

int CvImprovementInfo::getFeatureChangeType(int i) const
{
	return m_aiFeatureChangeTypes[i];
}

int CvImprovementInfo::getNumFeatureChangeTypes() const
{
	return (int)m_aiFeatureChangeTypes.size();
}

bool CvImprovementInfo::isFeatureChangeType(int i) const
{
	return algo::any_of_equal(m_aiFeatureChangeTypes, i);
}

//Post Load functions
//void CvImprovementInfo::setHighestCost()
//{
//	BuildTypes eHighestCostBuild = NO_BUILD;
//	int iHighestCost = 0;
//	for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
//	{
//		if (GC.getBuildInfo((BuildTypes)iI).getImprovement() == GC.getInfoTypeForString(m_szType))
//		{
//			if (GC.getBuildInfo((BuildTypes)iI).getCost() > iHighestCost)
//			{
//				eHighestCostBuild = (BuildTypes)iI;
//				iHighestCost = GC.getBuildInfo((BuildTypes)iI).getCost();
//			}
//		}
//	}
//	m_iHighestCost = iHighestCost;
//}

//int CvImprovementInfo::getHighestCost() const
//{
//	return m_iHighestCost;
//}

void CvImprovementInfo::doPostLoadCaching(uint32_t eThis)
{
	for (int i = 0, num = GC.getNumBuildInfos(); i < num; i++)
	{
		if (GC.getBuildInfo((BuildTypes)i).getImprovement() == eThis)
		{
			m_improvementBuildTypes.push_back((BuildTypes)i);
		}
	}
}

void CvImprovementInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iAdvancedStartCost);

	CheckSum(iSum, m_iTilesPerGoody);
	CheckSum(iSum, m_iGoodyUniqueRange);
	CheckSum(iSum, m_iFeatureGrowthProbability);
	CheckSum(iSum, m_iUpgradeTime);
	CheckSum(iSum, m_iAirBombDefense);
	CheckSum(iSum, m_iDefenseModifier);
	CheckSum(iSum, m_iHappiness);
	CheckSum(iSum, m_iPillageGold);
	CheckSum(iSum, m_iImprovementPillage);
	CheckSum(iSum, m_iImprovementUpgrade);

	// Super Forts begin *XML*
	CheckSum(iSum, m_iCulture);
	CheckSum(iSum, m_iCultureRange);
	CheckSum(iSum, m_iVisibilityChange);
	CheckSum(iSum, m_iSeeFrom);
	CheckSum(iSum, m_iUniqueRange);
	CheckSum(iSum, m_bBombardable);
	CheckSum(iSum, m_bUpgradeRequiresFortify);
	// Super Forts end
	// Super forts C2C adaptation
	CheckSum(iSum, m_bIsUniversalTradeBonusProvider);
	CheckSum(iSum, m_bIsZOCSource);
	// Super forts C2C adaptation end

	CheckSum(iSum, m_bActsAsCity);
	CheckSum(iSum, m_bHillsMakesValid);
	CheckSum(iSum, m_bFreshWaterMakesValid);
	CheckSum(iSum, m_bRiverSideMakesValid);
	CheckSum(iSum, m_bNoFreshWater);
	CheckSum(iSum, m_bRequiresFlatlands);
	CheckSum(iSum, m_bRequiresRiverSide);
	CheckSum(iSum, m_bRequiresIrrigation);
	CheckSum(iSum, m_bCarriesIrrigation);
	CheckSum(iSum, m_bRequiresFeature);
	CheckSum(iSum, m_bPeakImprovement);
	CheckSum(iSum, m_bWaterImprovement);
	CheckSum(iSum, m_bGoody);
	CheckSum(iSum, m_bPermanent);
	CheckSum(iSum, m_bOutsideBorders);
	CheckSumC(iSum, m_aeMapCategoryTypes);

	// Arrays

	CheckSumI(iSum, NUM_YIELD_TYPES, m_piPrereqNatureYield);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piYieldChange);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piRiverSideYieldChange);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piIrrigatedChange);
	CheckSumI(iSum, GC.getNumTerrainInfos(), m_pbTerrainMakesValid);
	CheckSumI(iSum, GC.getNumFeatureInfos(), m_pbFeatureMakesValid);

	 int i;
	if (m_paImprovementBonus)
		for (i = 0; i < GC.getNumBonusInfos(); i++)
		{
			m_paImprovementBonus[i].getCheckSum(iSum);
		}

	if (m_ppiTechYieldChanges)
		for (i = 0; i < GC.getNumTechInfos(); i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppiTechYieldChanges[i]);
		}

	if (m_ppiRouteYieldChanges)
		for (i = 0; i < GC.getNumRouteInfos(); i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppiRouteYieldChanges[i]);
		}

	CheckSum(iSum, m_iHealthPercent);
	CheckSum(iSum, m_bPeakMakesValid);
	CheckSum(iSum, m_iDepletionRand);
	CheckSum(iSum, m_iPrereqTech);

	//if (m_ppiTraitYieldChanges)
	//	for(i=0;i<GC.getNumTraitInfos();i++)
	//	{
	//		CheckSumI(iSum, NUM_YIELD_TYPES, m_ppiTraitYieldChanges[i]);
	//	}

	m_PropertyManipulators.getCheckSum(iSum);
	//TB Improvements
	//Object Indexes
	CheckSum(iSum, m_iBonusChange);
	//Booleans
	CheckSum(iSum, m_bCanMoveSeaUnits);
	CheckSum(iSum, m_bChangeRemove);
	CheckSum(iSum, m_bNotOnAnyBonus);
	CheckSum(iSum, m_bNational);
	CheckSum(iSum, m_bGlobal);
	CheckSumC(iSum, m_aiAlternativeImprovementUpgradeTypes);
	CheckSumC(iSum, m_aiFeatureChangeTypes);
}

bool CvImprovementInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	int iIndex, j, iNumSibs;

	pXML->GetOptionalChildXmlValByName(m_szArtDefineTag, L"ArtDefineTag");

	if (pXML->TryMoveToXmlFirstChild(L"PrereqNatureYields"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piPrereqNatureYield);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piPrereqNatureYield);
	}

	if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piYieldChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldChange);
	}

	if (pXML->TryMoveToXmlFirstChild(L"RiverSideYieldChange"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piRiverSideYieldChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piRiverSideYieldChange);
	}

	if (pXML->TryMoveToXmlFirstChild(L"IrrigatedYieldChange"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piIrrigatedChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piIrrigatedChange);
	}

	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartCost, L"iAdvancedStartCost", 100);
	pXML->GetOptionalChildXmlValByName(&m_bActsAsCity, L"bActsAsCity");
	pXML->GetOptionalChildXmlValByName(&m_bHillsMakesValid, L"bHillsMakesValid");
	pXML->GetOptionalChildXmlValByName(&m_bFreshWaterMakesValid, L"bFreshWaterMakesValid");
	pXML->GetOptionalChildXmlValByName(&m_bRiverSideMakesValid, L"bRiverSideMakesValid");
	pXML->GetOptionalChildXmlValByName(&m_bNoFreshWater, L"bNoFreshWater");
	pXML->GetOptionalChildXmlValByName(&m_bRequiresFlatlands, L"bRequiresFlatlands");
	pXML->GetOptionalChildXmlValByName(&m_bRequiresRiverSide, L"bRequiresRiverSide");
	pXML->GetOptionalChildXmlValByName(&m_bRequiresIrrigation, L"bRequiresIrrigation");
	pXML->GetOptionalChildXmlValByName(&m_bCarriesIrrigation, L"bCarriesIrrigation");
	pXML->GetOptionalChildXmlValByName(&m_bRequiresFeature, L"bRequiresFeature");
	pXML->GetOptionalChildXmlValByName(&m_bPeakImprovement, L"bPeakImprovement");
	pXML->GetOptionalChildXmlValByName(&m_bWaterImprovement, L"bWaterImprovement");
	pXML->GetOptionalChildXmlValByName(&m_bGoody, L"bGoody");
	pXML->GetOptionalChildXmlValByName(&m_bPermanent, L"bPermanent");
	pXML->GetOptionalChildXmlValByName(&m_iTilesPerGoody, L"iTilesPerGoody");
	pXML->GetOptionalChildXmlValByName(&m_iGoodyUniqueRange, L"iGoodyRange");
	pXML->GetOptionalChildXmlValByName(&m_iFeatureGrowthProbability, L"iFeatureGrowth");
	pXML->GetOptionalChildXmlValByName(&m_iUpgradeTime, L"iUpgradeTime");
	pXML->GetOptionalChildXmlValByName(&m_iAirBombDefense, L"iAirBombDefense");
	pXML->GetOptionalChildXmlValByName(&m_iDefenseModifier, L"iDefenseModifier");
	pXML->GetOptionalChildXmlValByName(&m_iHappiness, L"iHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iPillageGold, L"iPillageGold");
	pXML->GetOptionalChildXmlValByName(&m_bOutsideBorders, L"bOutsideBorders");
	// Super Forts begin *XML*
	pXML->GetOptionalChildXmlValByName(&m_iCulture, L"iCulture");
	pXML->GetOptionalChildXmlValByName(&m_iCultureRange, L"iCultureRange");
	pXML->GetOptionalChildXmlValByName(&m_iVisibilityChange, L"iVisibilityChange");
	pXML->GetOptionalChildXmlValByName(&m_iSeeFrom, L"iSeeFrom");
	pXML->GetOptionalChildXmlValByName(&m_iUniqueRange, L"iUniqueRange");
	pXML->GetOptionalChildXmlValByName(&m_bBombardable, L"bBombardable");
	pXML->GetOptionalChildXmlValByName(&m_bUpgradeRequiresFortify, L"bUpgradeRequiresFortify");
	// Super Forts end
	// Super forts C2C adaptation
	pXML->GetOptionalChildXmlValByName(&m_bIsUniversalTradeBonusProvider, L"bIsUniversalTradeBonusProvider");
	pXML->GetOptionalChildXmlValByName(&m_bIsZOCSource, L"bIsZOCSource");
	// Super forts C2C adaptation end

	pXML->SetVariableListTagPair(&m_pbTerrainMakesValid, L"TerrainMakesValids", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_pbFeatureMakesValid, L"FeatureMakesValids", GC.getNumFeatureInfos());

	pXML->SetOptionalVector(&m_aeMapCategoryTypes, L"MapCategoryTypes");
	if (pXML->TryMoveToXmlFirstChild(L"BonusTypeStructs"))
	{
		// call the function that sets the bonus booleans
		pXML->SetImprovementBonuses(&m_paImprovementBonus);
		pXML->MoveToXmlParent();
	}
	else
	{
		// initialize the boolean list to the correct size and all the booleans to false
		pXML->InitImprovementBonusList(&m_paImprovementBonus, GC.getNumBonusInfos());
	}

	// initialize the boolean list to the correct size and all the booleans to false
	FAssertMsg(GC.getNumTechInfos() > 0, "the number of tech infos is zero or less");
	if (pXML->TryMoveToXmlFirstChild(L"TechYieldChanges"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DList(&m_ppiTechYieldChanges, GC.getNumTechInfos(), NUM_YIELD_TYPES);
				for (j = 0; j < iNumSibs; j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"PrereqTech");
					iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppiTechYieldChanges[iIndex]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"TechYields"))
						{
							// call the function that sets the yield change variable
							pXML->SetYields(&m_ppiTechYieldChanges[iIndex]);
							pXML->MoveToXmlParent();
						}
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
			}

			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}

	// initialize the boolean list to the correct size and all the booleans to false
	FAssertMsg(GC.getNumRouteInfos() > 0, "the number of route infos is zero or less");
	if (pXML->TryMoveToXmlFirstChild(L"RouteYieldChanges"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DList(&m_ppiRouteYieldChanges, GC.getNumRouteInfos(), NUM_YIELD_TYPES);
				for (j = 0; j < iNumSibs; j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"RouteType");
					iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppiRouteYieldChanges[iIndex]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"RouteYields"))
						{
							// call the function that sets the yield change variable
							pXML->SetYields(&m_ppiRouteYieldChanges[iIndex]);
							pXML->MoveToXmlParent();
						}
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
			}

			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"WorldSoundscapeAudioScript");
	if (szTextVal.GetLength() > 0)
		m_iWorldSoundscapeScriptId = gDLL->getAudioTagIndex(szTextVal.GetCString(), AUDIOTAG_SOUNDSCAPE);
	else
		m_iWorldSoundscapeScriptId = -1;

	int iHealth = 0;
	pXML->GetOptionalChildXmlValByName(&iHealth, L"iHealth");
	pXML->GetOptionalChildXmlValByName(&m_iHealthPercent, L"iHealthPercent");
	m_iHealthPercent += iHealth * 100;
	pXML->GetOptionalChildXmlValByName(&m_bPeakMakesValid, L"bPeakMakesValid");
	pXML->GetOptionalChildXmlValByName(&m_iDepletionRand, L"iDepletionRand");

	pXML->GetOptionalTypeEnum(m_iPrereqTech, L"PrereqTech");

	// initialize the boolean list to the correct size and all the booleans to false
	//FAssertMsg((GC.getNumTraitInfos() > 0) && (NUM_YIELD_TYPES > 0),"either the number of trait infos is zero or less or the number of yield types is zero or less");
	//if (pXML->TryMoveToXmlFirstChild(L"TraitYieldChanges"))
	//{
	//	if (pXML->SkipToNextVal())
	//	{
	//		iNumSibs = pXML->GetXmlChildrenNumber();
	//		if (pXML->TryMoveToXmlFirstChild())
	//		{
	//			if (0 < iNumSibs)
	//			{
	//				pXML->Init2DList(&m_ppiTraitYieldChanges, GC.getNumTraitInfos(), NUM_YIELD_TYPES);
	//				for (j=0;j<iNumSibs;j++)
	//				{
	//					pXML->GetChildXmlValByName(szTextVal, L"TraitType");
	//					iIndex = pXML->GetInfoClass(szTextVal);

	//					if (iIndex > -1)
	//					{
	//						// delete the array since it will be reallocated
	//						SAFE_DELETE_ARRAY(m_ppiTraitYieldChanges[iIndex]);
	//						// if we can set the current xml node to it's next sibling
	//						if (pXML->TryMoveToXmlFirstChild(L"TraitYields"))
	//						{
	//							// call the function that sets the yield change variable
	//							pXML->SetYields(&m_ppiTraitYieldChanges[iIndex]);
	//							pXML->MoveToXmlParent();
	//						}
	//					}

	//					if (!pXML->TryMoveToXmlNextSibling())
	//					{
	//						break;
	//					}
	//				}
	//			}

	//			pXML->MoveToXmlParent();
	//		}
	//	}

	//	pXML->MoveToXmlParent();
	//}

	m_PropertyManipulators.read(pXML);

	pXML->GetOptionalChildXmlValByName(&m_bCanMoveSeaUnits, L"bCanMoveSeaUnits");
	pXML->GetOptionalChildXmlValByName(&m_bChangeRemove, L"bChangeRemove");
	pXML->GetOptionalChildXmlValByName(&m_bNotOnAnyBonus, L"bNotOnAnyBonus");
	pXML->GetOptionalChildXmlValByName(&m_bNational, L"bNational");
	pXML->GetOptionalChildXmlValByName(&m_bGlobal, L"bGlobal");
	pXML->GetOptionalTypeEnumWithDelayedResolution(m_iImprovementPillage, L"ImprovementPillage");
	pXML->GetOptionalTypeEnumWithDelayedResolution(m_iImprovementUpgrade, L"ImprovementUpgrade");
	pXML->GetOptionalTypeEnumWithDelayedResolution(m_iBonusChange, L"BonusChange");
	pXML->SetOptionalVectorWithDelayedResolution(m_aiAlternativeImprovementUpgradeTypes, L"AlternativeImprovementUpgradeTypes");
	pXML->SetOptionalVector(&m_aiFeatureChangeTypes, L"FeatureChangeTypes");

	return true;
}

void CvImprovementInfo::copyNonDefaults(const CvImprovementInfo* pClassInfo)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	if (getArtDefineTag() == cDefault) m_szArtDefineTag = pClassInfo->getArtDefineTag();

	CvInfoBase::copyNonDefaults(pClassInfo);

	for (int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		if (getPrereqNatureYield(i) == iDefault && pClassInfo->getPrereqNatureYield(i) != iDefault)
		{
			if (NULL == m_piPrereqNatureYield)
			{
				CvXMLLoadUtility::InitList(&m_piPrereqNatureYield, NUM_YIELD_TYPES, iDefault);
			}
			m_piPrereqNatureYield[i] = pClassInfo->getPrereqNatureYield(i);
		}
		if (getYieldChange(i) == iDefault && pClassInfo->getYieldChange(i) != iDefault)
		{
			if (NULL == m_piYieldChange)
			{
				CvXMLLoadUtility::InitList(&m_piYieldChange, NUM_YIELD_TYPES, iDefault);
			}
			m_piYieldChange[i] = pClassInfo->getYieldChange(i);
		}
		if (getRiverSideYieldChange(i) == iDefault && pClassInfo->getRiverSideYieldChange(i) != iDefault)
		{
			if (NULL == m_piRiverSideYieldChange)
			{
				CvXMLLoadUtility::InitList(&m_piRiverSideYieldChange, NUM_YIELD_TYPES, iDefault);
			}
			m_piRiverSideYieldChange[i] = pClassInfo->getRiverSideYieldChange(i);
		}
		if (getIrrigatedYieldChange(i) == iDefault && pClassInfo->getIrrigatedYieldChange(i) != iDefault)
		{
			if (NULL == m_piIrrigatedChange)
			{
				CvXMLLoadUtility::InitList(&m_piIrrigatedChange, NUM_YIELD_TYPES, iDefault);
			}
			m_piIrrigatedChange[i] = pClassInfo->getIrrigatedYieldChange(i);
		}
	}

	if (getAdvancedStartCost() == 100) m_iAdvancedStartCost = pClassInfo->getAdvancedStartCost();
	if (getTilesPerGoody() == iDefault) m_iTilesPerGoody = pClassInfo->getTilesPerGoody();
	if (getGoodyUniqueRange() == iDefault) m_iGoodyUniqueRange = pClassInfo->getGoodyUniqueRange();
	if (getFeatureGrowthProbability() == iDefault) m_iFeatureGrowthProbability = pClassInfo->getFeatureGrowthProbability();
	if (getUpgradeTime() == iDefault) m_iUpgradeTime = pClassInfo->getUpgradeTime();
	if (getAirBombDefense() == iDefault) m_iAirBombDefense = pClassInfo->getAirBombDefense();
	if (getDefenseModifier() == iDefault) m_iDefenseModifier = pClassInfo->getDefenseModifier();
	if (getHappiness() == iDefault) m_iHappiness = pClassInfo->getHappiness();
	if (getPillageGold() == iDefault) m_iPillageGold = pClassInfo->getPillageGold();
	// Super forts C2C adaptation
	if (getCulture() == iDefault) m_iCulture = pClassInfo->getCulture();
	if (getCultureRange() == iDefault) m_iCultureRange = pClassInfo->getCultureRange();
	if (getVisibilityChange() == iDefault) m_iVisibilityChange = pClassInfo->getVisibilityChange();
	if (getSeeFrom() == iDefault) m_iSeeFrom = pClassInfo->getSeeFrom();
	if (getUniqueRange() == iDefault) m_iUniqueRange = pClassInfo->getUniqueRange();
	if (isBombardable() == bDefault) m_bBombardable = pClassInfo->isBombardable();
	if (isUpgradeRequiresFortify() == bDefault) m_bUpgradeRequiresFortify = pClassInfo->isUpgradeRequiresFortify();
	if (isUniversalTradeBonusProvider() == bDefault) m_bIsUniversalTradeBonusProvider = pClassInfo->isUniversalTradeBonusProvider();
	if (isZOCSource() == bDefault) m_bIsZOCSource = pClassInfo->isZOCSource();
	// Super forts C2C adaptation end
	if (m_bActsAsCity == bDefault) m_bActsAsCity = pClassInfo->isActsAsCity();
	if (isHillsMakesValid() == bDefault) m_bHillsMakesValid = pClassInfo->isHillsMakesValid();
	if (isFreshWaterMakesValid() == bDefault) m_bFreshWaterMakesValid = pClassInfo->isFreshWaterMakesValid();
	if (isRiverSideMakesValid() == bDefault) m_bRiverSideMakesValid = pClassInfo->isRiverSideMakesValid();
	if (isNoFreshWater() == bDefault) m_bNoFreshWater = pClassInfo->isNoFreshWater();
	if (isRequiresFlatlands() == bDefault) m_bRequiresFlatlands = pClassInfo->isRequiresFlatlands();
	if (isRequiresRiverSide() == bDefault) m_bRequiresRiverSide = pClassInfo->isRequiresRiverSide();
	if (isRequiresIrrigation() == bDefault) m_bRequiresIrrigation = pClassInfo->isRequiresIrrigation();
	if (isCarriesIrrigation() == bDefault) m_bCarriesIrrigation = pClassInfo->isCarriesIrrigation();
	if (isRequiresFeature() == bDefault) m_bRequiresFeature = pClassInfo->isRequiresFeature();
	if (isPeakImprovement() == bDefault) m_bPeakImprovement = pClassInfo->isPeakImprovement();
	if (isWaterImprovement() == bDefault) m_bWaterImprovement = pClassInfo->isWaterImprovement();
	if (isGoody() == bDefault) m_bGoody = pClassInfo->isGoody();
	if (isPermanent() == bDefault) m_bPermanent = pClassInfo->isPermanent();
	if (isOutsideBorders() == bDefault) m_bOutsideBorders = pClassInfo->isOutsideBorders();

	for (int i = 0; i < GC.getNumTerrainInfos(); i++)
	{
		if (getTerrainMakesValid(i) == bDefault && pClassInfo->getTerrainMakesValid(i) != bDefault)
		{
			if (NULL == m_pbTerrainMakesValid)
			{
				CvXMLLoadUtility::InitList(&m_pbTerrainMakesValid, GC.getNumTerrainInfos(), bDefault);
			}
			m_pbTerrainMakesValid[i] = pClassInfo->getTerrainMakesValid(i);
		}
	}
	for (int i = 0; i < GC.getNumFeatureInfos(); i++)
	{
		if (getFeatureMakesValid(i) == bDefault && pClassInfo->getFeatureMakesValid(i) != bDefault)
		{
			if (NULL == m_pbFeatureMakesValid)
			{
				CvXMLLoadUtility::InitList(&m_pbFeatureMakesValid, GC.getNumFeatureInfos(), bDefault);
			}
			m_pbFeatureMakesValid[i] = pClassInfo->getFeatureMakesValid(i);
		}
	}
	for (int i = 0; i < GC.getNumBonusInfos(); i++)
	{
		if (m_paImprovementBonus[i].m_bBonusMakesValid == bDefault)
		{
			m_paImprovementBonus[i].m_bBonusMakesValid = pClassInfo->isImprovementBonusMakesValid(i);
		}
		if (m_paImprovementBonus[i].m_bObsoleteBonusMakesValid == bDefault)
		{
			m_paImprovementBonus[i].m_bObsoleteBonusMakesValid = pClassInfo->isImprovementObsoleteBonusMakesValid(i);
		}
		if (m_paImprovementBonus[i].m_bBonusTrade == bDefault)
		{
			m_paImprovementBonus[i].m_bBonusTrade = pClassInfo->isImprovementBonusTrade(i);
		}
		if (m_paImprovementBonus[i].m_iDiscoverRand == iDefault)
		{
			m_paImprovementBonus[i].m_iDiscoverRand = pClassInfo->getImprovementBonusDiscoverRand(i);
		}
		if (m_paImprovementBonus[i].m_iDepletionRand == 0)
		{
			m_paImprovementBonus[i].m_iDepletionRand = pClassInfo->getImprovementBonusDepletionRand(i);
		}

		for (int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if (m_paImprovementBonus[i].m_piYieldChange[j] == iDefault)
			{
				m_paImprovementBonus[i].m_piYieldChange[j] = pClassInfo->getImprovementBonusYield(i, j);
			}
		}
	}
	for (int i = 0; i < GC.getNumTechInfos(); i++)
	{
		for (int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if (getTechYieldChanges(i, j) == iDefault && pClassInfo->getTechYieldChanges(i, j) != iDefault)
			{
				if (NULL == m_ppiTechYieldChanges)
				{
					CvXMLLoadUtility::Init2DList(&m_ppiTechYieldChanges, GC.getNumTechInfos(), NUM_YIELD_TYPES);
				}
				else if (NULL == m_ppiTechYieldChanges[i])
				{
					CvXMLLoadUtility::InitList(&m_ppiTechYieldChanges[i], NUM_YIELD_TYPES, iDefault);
				}
				m_ppiTechYieldChanges[i][j] = pClassInfo->getTechYieldChanges(i, j);
			}
		}
	}
	for (int i = 0; i < GC.getNumRouteInfos(); i++)
	{
		for (int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if (getRouteYieldChanges(i, j) == iDefault && pClassInfo->getRouteYieldChanges(i, j) != iDefault)
			{
				if (NULL == m_ppiRouteYieldChanges)
				{
					CvXMLLoadUtility::Init2DList(&m_ppiRouteYieldChanges, GC.getNumRouteInfos(), NUM_YIELD_TYPES);
				}
				else if (NULL == m_ppiRouteYieldChanges[i])
				{
					CvXMLLoadUtility::InitList(&m_ppiRouteYieldChanges[i], NUM_YIELD_TYPES, iDefault);
				}
				m_ppiRouteYieldChanges[i][j] = pClassInfo->getRouteYieldChanges(i, j);
			}
		}
	}

	if (m_iWorldSoundscapeScriptId == iTextDefault) m_iWorldSoundscapeScriptId = pClassInfo->getWorldSoundscapeScriptId();

	if (isPeakMakesValid() == bDefault) m_bPeakMakesValid = pClassInfo->isPeakMakesValid();
	if (getHealthPercent() == iDefault) m_iHealthPercent = pClassInfo->getHealthPercent();
	if (getPrereqTech() == NO_TECH) m_iPrereqTech = pClassInfo->getPrereqTech();

	//for ( int i = 0; i < GC.getNumTraitInfos(); i++)
	//{
	//	for ( int j = 0; j < NUM_YIELD_TYPES; j++)
	//	{
	//		if ( getTraitYieldChanges(i, j) == iDefault && pClassInfo->getTraitYieldChanges(i, j) != iDefault)
	//		{
	//			if ( NULL == m_ppiTraitYieldChanges )
	//			{
	//				pXML->Init2DList(&m_ppiTraitYieldChanges, GC.getNumTraitInfos(), NUM_YIELD_TYPES);
	//			}
	//			else if ( NULL == m_ppiTraitYieldChanges[i] )
	//			{
	//				CvXMLLoadUtility::InitList(&m_ppiTraitYieldChanges[i],NUM_YIELD_TYPES,iDefault);
	//			}
	//			m_ppiTraitYieldChanges[i][j] = pClassInfo->getTraitYieldChanges(i, j);
	//		}
	//	}
	//}

	//TB Improvements
	//Object Indexes
	if (m_iBonusChange == iTextDefault) m_iBonusChange = pClassInfo->getBonusChange();
	//booleans
	if (isCanMoveSeaUnits() == bDefault) m_bCanMoveSeaUnits = pClassInfo->isCanMoveSeaUnits();
	if (isChangeRemove() == bDefault) m_bChangeRemove = pClassInfo->isChangeRemove();
	if (isNotOnAnyBonus() == bDefault) m_bNotOnAnyBonus = pClassInfo->isNotOnAnyBonus();
	if (isNational() == bDefault) m_bNational = pClassInfo->isNational();
	if (isGlobal() == bDefault) m_bGlobal = pClassInfo->isGlobal();

	if (getNumAlternativeImprovementUpgradeTypes() == 0)
	{
		const int iNum = pClassInfo->getNumAlternativeImprovementUpgradeTypes();
		m_aiAlternativeImprovementUpgradeTypes.resize(iNum);
		for (int i = 0; i < iNum; i++)
		{
			GC.copyNonDefaultDelayedResolution((int*)&(m_aiAlternativeImprovementUpgradeTypes[i]), (int*)&(pClassInfo->m_aiAlternativeImprovementUpgradeTypes[i]));
		}
	}

	GC.copyNonDefaultDelayedResolution((int*)&m_iImprovementPillage, (int*)&pClassInfo->m_iImprovementPillage);
	GC.copyNonDefaultDelayedResolution((int*)&m_iImprovementUpgrade, (int*)&pClassInfo->m_iImprovementUpgrade);
	GC.copyNonDefaultDelayedResolution((int*)&m_iBonusChange, (int*)&pClassInfo->m_iBonusChange);

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiFeatureChangeTypes, pClassInfo->m_aiFeatureChangeTypes);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aeMapCategoryTypes, pClassInfo->getMapCategories());
}
