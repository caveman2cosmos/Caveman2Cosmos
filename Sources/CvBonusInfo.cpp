
#include "FProfiler.h"

#include "CvBonusInfo.h"
#include "CvDefines.h"
#include "CvImprovementInfo.h"
#include "CvInfoUtil.h"
#include "CvXMLLoadUtility.h"
#include "CheckSum.h"

CvBonusInfo::CvBonusInfo() :
	m_iBonusClassType(NO_BONUSCLASS)
	, m_iChar(0)
	, m_iTechReveal(NO_TECH)
	, m_iTechCityTrade(NO_TECH)
	, m_iTechObsolete(NO_TECH)
	, m_iAITradeModifier(0)
	, m_iAIObjective(0)
	, m_iHealth(0)
	, m_iHappiness(0)
	, m_iMinAreaSize(0)
	, m_iMinLatitude(0)
	, m_iMaxLatitude(90)
	, m_iPlacementOrder(-1)
	, m_iConstAppearance(0)
	, m_iRandAppearance1(0)
	, m_iRandAppearance2(0)
	, m_iRandAppearance3(0)
	, m_iRandAppearance4(0)
	, m_iPercentPerPlayer(0)
	, m_iTilesPer(0)
	, m_iMinLandPercent(0)
	, m_iUniqueRange(0)
	, m_iGroupRange(0)
	, m_iGroupRand(0)
	, m_bOneArea(false)
	, m_bHills(false)
	, m_bFlatlands(false)
	, m_bBonusCoastalOnly(false)
	, m_bNoRiverSide(false)
	, m_bNormalize(false)
	, m_piYieldChange(NULL)
	, m_piImprovementChange(NULL)
	, m_pbTerrain(NULL)
	, m_pbFeature(NULL)
	, m_pbFeatureTerrain(NULL)
	, m_bPeaks(false)
	, m_PropertyManipulators()
	, m_tradeProvidingImprovements(NULL)
{ }

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvBonusInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvBonusInfo::~CvBonusInfo()
{
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piImprovementChange);
	SAFE_DELETE_ARRAY(m_pbTerrain);
	SAFE_DELETE_ARRAY(m_pbFeature);
	SAFE_DELETE_ARRAY(m_pbFeatureTerrain);	// free memory - MT
}

int CvBonusInfo::getBonusClassType() const
{
	return m_iBonusClassType;
}

int CvBonusInfo::getChar() const
{
	return m_iChar;
}

void CvBonusInfo::setChar(int i)
{
	m_iChar = i;
}

int CvBonusInfo::getTechReveal() const
{
	return m_iTechReveal;
}

int CvBonusInfo::getTechCityTrade() const
{
	return m_iTechCityTrade;
}

int CvBonusInfo::getTechObsolete() const
{
	return m_iTechObsolete;
}

int CvBonusInfo::getAITradeModifier() const
{
	return m_iAITradeModifier;
}

int CvBonusInfo::getAIObjective() const
{
	return m_iAIObjective;
}

int CvBonusInfo::getHealth() const
{
	return m_iHealth;
}

int CvBonusInfo::getHappiness() const
{
	return m_iHappiness;
}

int CvBonusInfo::getMinAreaSize() const
{
	return m_iMinAreaSize;
}

int CvBonusInfo::getMinLatitude() const
{
	return m_iMinLatitude;
}

int CvBonusInfo::getMaxLatitude() const
{
	return m_iMaxLatitude;
}

int CvBonusInfo::getPlacementOrder() const
{
	return m_iPlacementOrder;
}

int CvBonusInfo::getConstAppearance() const
{
	return m_iConstAppearance;
}

int CvBonusInfo::getRandAppearance1() const
{
	return m_iRandAppearance1;
}

int CvBonusInfo::getRandAppearance2() const
{
	return m_iRandAppearance2;
}

int CvBonusInfo::getRandAppearance3() const
{
	return m_iRandAppearance3;
}

int CvBonusInfo::getRandAppearance4() const
{
	return m_iRandAppearance4;
}

int CvBonusInfo::getPercentPerPlayer() const
{
	return m_iPercentPerPlayer;
}

int CvBonusInfo::getTilesPer() const
{
	return m_iTilesPer;
}

int CvBonusInfo::getMinLandPercent() const
{
	return m_iMinLandPercent;
}

int CvBonusInfo::getUniqueRange() const
{
	return m_iUniqueRange;
}

int CvBonusInfo::getGroupRange() const
{
	return m_iGroupRange;
}

int CvBonusInfo::getGroupRand() const
{
	return m_iGroupRand;
}

bool CvBonusInfo::isOneArea() const
{
	return m_bOneArea;
}

bool CvBonusInfo::isHills() const
{
	return m_bHills;
}

bool CvBonusInfo::isFlatlands() const
{
	return m_bFlatlands;
}

bool CvBonusInfo::isBonusCoastalOnly() const
{
	return m_bBonusCoastalOnly;
}

bool CvBonusInfo::isNoRiverSide() const
{
	return m_bNoRiverSide;
}

bool CvBonusInfo::isNormalize() const
{
	return m_bNormalize;
}

const char* CvBonusInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}

// Arrays

int CvBonusInfo::getYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}

int* CvBonusInfo::getYieldChangeArray() const
{
	return m_piYieldChange;
}

int CvBonusInfo::getImprovementChange(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), i);
	return m_piImprovementChange ? m_piImprovementChange[i] : 0;
}

bool CvBonusInfo::isTerrain(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), i);
	return m_pbTerrain ? m_pbTerrain[i] : false;
}

bool CvBonusInfo::isFeature(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), i);
	return m_pbFeature ? m_pbFeature[i] : false;
}

bool CvBonusInfo::isFeatureTerrain(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), i);
	return m_pbFeatureTerrain ? m_pbFeatureTerrain[i] : false;
}

int CvBonusInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}

int CvBonusInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}

bool CvBonusInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}

#ifdef OUTBREAKS_AND_AFFLICTIONS
int CvBonusInfo::getNumAfflictionCommunicabilityTypes() const
{
	return (int)m_aAfflictionCommunicabilityTypes.size();
}

PromotionLineAfflictionModifier CvBonusInfo::getAfflictionCommunicabilityType(int iPromotionLine, bool bWorkedTile, bool bVicinity, bool bAccessVolume)
{
	FASSERT_BOUNDS(0, getNumAfflictionCommunicabilityTypes(), iPromotionLine);

	if ((bWorkedTile && !m_aAfflictionCommunicabilityTypes[iPromotionLine].bWorkedTile) ||
		(bVicinity && !m_aAfflictionCommunicabilityTypes[iPromotionLine].bVicinity) ||
		(bAccessVolume && !m_aAfflictionCommunicabilityTypes[iPromotionLine].bAccessVolume))
	{
		PromotionLineAfflictionModifier kMod = m_aAfflictionCommunicabilityTypes[iPromotionLine];
		kMod.iModifier = 0;
		return kMod;
	}
	return m_aAfflictionCommunicabilityTypes[iPromotionLine];
}
#endif // OUTBREAKS_AND_AFFLICTIONS

const char* CvBonusInfo::getButton() const
{
	const CvArtInfoBonus* pBonusArtInfo = getArtInfo();
	return pBonusArtInfo ? pBonusArtInfo->getButton() : NULL;
}

bool CvBonusInfo::isPeaks() const
{
	return m_bPeaks;
}


ImprovementTypes CvBonusInfo::getProvidedByImprovementType(const int i) const
{
	return m_providedByImprovementTypes[i];
}

int CvBonusInfo::getNumProvidedByImprovementTypes() const
{
	return (int)m_providedByImprovementTypes.size();
}

bool CvBonusInfo::isProvidedByImprovementType(const ImprovementTypes i) const
{
	return algo::any_of_equal(m_providedByImprovementTypes, i);
}

void CvBonusInfo::setProvidedByImprovementTypes(const ImprovementTypes eType)
{
	m_providedByImprovementTypes.push_back(eType);
}


void CvBonusInfo::getCheckSum(uint32_t& iSum) const
{
	PROFILE_EXTRA_FUNC();
	CheckSum(iSum, m_iBonusClassType);
	CheckSum(iSum, m_iTechReveal);
	CheckSum(iSum, m_iTechCityTrade);
	CheckSum(iSum, m_iTechObsolete);
	CheckSum(iSum, m_iAITradeModifier);
	CheckSum(iSum, m_iAIObjective);
	CheckSum(iSum, m_iHealth);
	CheckSum(iSum, m_iHappiness);
	CheckSum(iSum, m_iMinAreaSize);
	CheckSum(iSum, m_iMinLatitude);
	CheckSum(iSum, m_iMaxLatitude);
	CheckSum(iSum, m_iPlacementOrder);
	CheckSum(iSum, m_iConstAppearance);
	CheckSum(iSum, m_iRandAppearance1);
	CheckSum(iSum, m_iRandAppearance2);
	CheckSum(iSum, m_iRandAppearance3);
	CheckSum(iSum, m_iRandAppearance4);
	CheckSum(iSum, m_iPercentPerPlayer);
	CheckSum(iSum, m_iTilesPer);
	CheckSum(iSum, m_iMinLandPercent);
	CheckSum(iSum, m_iUniqueRange);
	CheckSum(iSum, m_iGroupRange);
	CheckSum(iSum, m_iGroupRand);

	CheckSum(iSum, m_bOneArea);
	CheckSum(iSum, m_bHills);
	CheckSum(iSum, m_bFlatlands);
	CheckSum(iSum, m_bBonusCoastalOnly);
	CheckSum(iSum, m_bNoRiverSide);
	CheckSum(iSum, m_bNormalize);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piYieldChange);
	CheckSumI(iSum, GC.getNumImprovementInfos(), m_piImprovementChange);
	CheckSumI(iSum, GC.getNumTerrainInfos(), m_pbTerrain);
	CheckSumI(iSum, GC.getNumFeatureInfos(), m_pbFeature);
	CheckSumI(iSum, GC.getNumTerrainInfos(), m_pbFeatureTerrain);
	CheckSumC(iSum, m_aeMapCategoryTypes);
	CheckSum(iSum, m_bPeaks);

	const int iNumElements = m_aAfflictionCommunicabilityTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aAfflictionCommunicabilityTypes[i].ePromotionLine);
		CheckSum(iSum, m_aAfflictionCommunicabilityTypes[i].iModifier);
	}

	CheckSumC(iSum, m_aiCategories);

	m_PropertyManipulators.getCheckSum(iSum);
}

bool CvBonusInfo::read(CvXMLLoadUtility* pXML)
{

	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"BonusClassType");
	m_iBonusClassType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(m_szArtDefineTag, L"ArtDefineTag");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TechReveal");
	m_iTechReveal = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TechCityTrade");
	m_iTechCityTrade = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TechObsolete");
	m_iTechObsolete = pXML->GetInfoClass(szTextVal);

	if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
	{
		pXML->SetYields(&m_piYieldChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldChange);
	}

	pXML->GetOptionalChildXmlValByName(&m_iAITradeModifier, L"iAITradeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iAIObjective, L"iAIObjective");
	pXML->GetOptionalChildXmlValByName(&m_iHealth, L"iHealth");
	pXML->GetOptionalChildXmlValByName(&m_iHappiness, L"iHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iMinAreaSize, L"iMinAreaSize");
	pXML->GetOptionalChildXmlValByName(&m_iMinLatitude, L"iMinLatitude");
	pXML->GetOptionalChildXmlValByName(&m_iMaxLatitude, L"iMaxLatitude", 90);
	pXML->GetOptionalChildXmlValByName(&m_iPlacementOrder, L"iPlacementOrder", -1);
	pXML->GetOptionalChildXmlValByName(&m_iConstAppearance, L"iConstAppearance");

	// if we can set the current xml node to it's next sibling
	if (pXML->TryMoveToXmlFirstChild(L"Rands"))
	{
		pXML->GetOptionalChildXmlValByName(&m_iRandAppearance1, L"iRandApp1");
		pXML->GetOptionalChildXmlValByName(&m_iRandAppearance2, L"iRandApp2");
		pXML->GetOptionalChildXmlValByName(&m_iRandAppearance3, L"iRandApp3");
		pXML->GetOptionalChildXmlValByName(&m_iRandAppearance4, L"iRandApp4");

		// set the current xml node to it's parent node
		pXML->MoveToXmlParent();
	}

	pXML->GetOptionalChildXmlValByName(&m_iPercentPerPlayer, L"iPlayer");
	pXML->GetOptionalChildXmlValByName(&m_iTilesPer, L"iTilesPer");
	pXML->GetOptionalChildXmlValByName(&m_iMinLandPercent, L"iMinLandPercent");
	pXML->GetOptionalChildXmlValByName(&m_iUniqueRange, L"iUniqueRange");
	pXML->GetOptionalChildXmlValByName(&m_iGroupRange, L"iGroupRange");
	pXML->GetOptionalChildXmlValByName(&m_iGroupRand, L"iGroupRand");
	pXML->GetOptionalChildXmlValByName(&m_bOneArea, L"bArea");
	pXML->GetOptionalChildXmlValByName(&m_bHills, L"bHills");
	pXML->GetOptionalChildXmlValByName(&m_bFlatlands, L"bFlatlands");
	pXML->GetOptionalChildXmlValByName(&m_bBonusCoastalOnly, L"bBonusCoastalOnly");
	pXML->GetOptionalChildXmlValByName(&m_bNoRiverSide, L"bNoRiverSide");
	pXML->GetOptionalChildXmlValByName(&m_bNormalize, L"bNormalize");
	pXML->GetOptionalChildXmlValByName(&m_bPeaks, L"bPeaks");

	pXML->SetVariableListTagPair(&m_pbTerrain, L"TerrainBooleans", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_pbFeature, L"FeatureBooleans", GC.getNumFeatureInfos());
	pXML->SetVariableListTagPair(&m_pbFeatureTerrain, L"FeatureTerrainBooleans", GC.getNumTerrainInfos());
	pXML->SetOptionalVector(&m_aeMapCategoryTypes, L"MapCategoryTypes");
	pXML->SetOptionalVector(&m_aiCategories, L"Categories");

	if (pXML->TryMoveToXmlFirstChild(L"AfflictionCommunicabilityTypes"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"AfflictionCommunicabilityType");
		m_aAfflictionCommunicabilityTypes.resize(iNum);
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"AfflictionCommunicabilityType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"PromotionLineType");
					m_aAfflictionCommunicabilityTypes[i].ePromotionLine = (PromotionLineTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].iModifier), L"iCommunicability");
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].bWorkedTile), L"bWorkedTile");
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].bVicinity), L"bVicinity");
					pXML->GetChildXmlValByName(&(m_aAfflictionCommunicabilityTypes[i].bAccessVolume), L"bAccessVolume");
					i++;
				} while (pXML->TryMoveToXmlNextSibling(L"PromotionLineType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	m_PropertyManipulators.read(pXML);

	return true;
}
void CvBonusInfo::copyNonDefaults(const CvBonusInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();

	//this must always be in advance to the Hotkeyinfo initialization
	if (getArtDefineTag() == cDefault) m_szArtDefineTag = pClassInfo->getArtDefineTag();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getBonusClassType() == iTextDefault) m_iBonusClassType = pClassInfo->getBonusClassType();
	if (getTechReveal() == iTextDefault) m_iTechReveal = pClassInfo->getTechReveal();
	if (getTechCityTrade() == iTextDefault) m_iTechCityTrade = pClassInfo->getTechCityTrade();
	if (getTechObsolete() == iTextDefault) m_iTechObsolete = pClassInfo->getTechObsolete();

	for (int i = 0; i < NUM_YIELD_TYPES; i++)
	{
		if (getYieldChange(i) == iDefault && pClassInfo->getYieldChange(i) != iDefault)
		{
			if (m_piYieldChange == NULL)
			{
				CvXMLLoadUtility::InitList(&m_piYieldChange, NUM_YIELD_TYPES);
			}

			m_piYieldChange[i] = pClassInfo->getYieldChange(i);
		}
	}
	if (getAITradeModifier() == iDefault) m_iAITradeModifier = pClassInfo->getAITradeModifier();
	if (getAIObjective() == iDefault) m_iAIObjective = pClassInfo->getAIObjective();
	if (getHealth() == iDefault) m_iHealth = pClassInfo->getHealth();
	if (getHappiness() == iDefault) m_iHappiness = pClassInfo->getHappiness();
	if (getMinAreaSize() == iDefault) m_iMinAreaSize = pClassInfo->getMinAreaSize();
	if (getMinLatitude() == iDefault) m_iMinLatitude = pClassInfo->getMinLatitude();
	if (getMaxLatitude() == 90) m_iMaxLatitude = pClassInfo->getMaxLatitude();
	if (getPlacementOrder() == -1) m_iPlacementOrder = pClassInfo->getPlacementOrder();
	if (getConstAppearance() == iDefault) m_iConstAppearance = pClassInfo->getConstAppearance();
	if (getRandAppearance1() == iDefault) m_iRandAppearance1 = pClassInfo->getRandAppearance1();
	if (getRandAppearance2() == iDefault) m_iRandAppearance2 = pClassInfo->getRandAppearance2();
	if (getRandAppearance3() == iDefault) m_iRandAppearance3 = pClassInfo->getRandAppearance3();
	if (getRandAppearance4() == iDefault) m_iRandAppearance4 = pClassInfo->getRandAppearance4();
	if (getPercentPerPlayer() == iDefault) m_iPercentPerPlayer = pClassInfo->getPercentPerPlayer();
	if (getTilesPer() == iDefault) m_iTilesPer = pClassInfo->getTilesPer();
	if (getMinLandPercent() == iDefault) m_iMinLandPercent = pClassInfo->getMinLandPercent();
	if (getUniqueRange() == iDefault) m_iUniqueRange = pClassInfo->getUniqueRange();
	if (getGroupRange() == iDefault) m_iGroupRange = pClassInfo->getGroupRange();
	if (getGroupRand() == iDefault) m_iGroupRand = pClassInfo->getGroupRand();
	if (isOneArea() == bDefault) m_bOneArea = pClassInfo->isOneArea();
	if (isHills() == bDefault) m_bHills = pClassInfo->isHills();
	if (m_bFlatlands == bDefault) m_bFlatlands = pClassInfo->isFlatlands();
	if (m_bBonusCoastalOnly == bDefault) m_bBonusCoastalOnly = pClassInfo->isBonusCoastalOnly();
	if (isNoRiverSide() == bDefault) m_bNoRiverSide = pClassInfo->isNoRiverSide();
	if (isNormalize() == bDefault) m_bNormalize = pClassInfo->isNormalize();

	for (int i = 0; i < GC.getNumTerrainInfos(); i++)
	{
		if (isTerrain(i) == bDefault && pClassInfo->isTerrain(i) != bDefault)
		{
			if (NULL == m_pbTerrain)
			{
				CvXMLLoadUtility::InitList(&m_pbTerrain, GC.getNumTerrainInfos(), bDefault);
			}
			m_pbTerrain[i] = pClassInfo->isTerrain(i);
		}
		if (isFeatureTerrain(i) == bDefault && pClassInfo->isFeatureTerrain(i) != bDefault)
		{
			if (NULL == m_pbFeatureTerrain)
			{
				CvXMLLoadUtility::InitList(&m_pbFeatureTerrain, GC.getNumTerrainInfos(), bDefault);
			}
			m_pbFeatureTerrain[i] = pClassInfo->isFeatureTerrain(i);
		}
	}
	for (int i = 0; i < GC.getNumFeatureInfos(); i++)
	{
		if (isFeature(i) == bDefault && pClassInfo->isFeature(i) != bDefault)
		{
			if (NULL == m_pbFeature)
			{
				CvXMLLoadUtility::InitList(&m_pbFeature, GC.getNumFeatureInfos(), bDefault);
			}
			m_pbFeature[i] = pClassInfo->isFeature(i);
		}
	}
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aeMapCategoryTypes, pClassInfo->getMapCategories());
	if (isPeaks() == bDefault) m_bPeaks = pClassInfo->isPeaks();

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);

	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);
}

const std::vector<std::pair<ImprovementTypes, BuildTypes> >* CvBonusInfo::getTradeProvidingImprovements()
{
	PROFILE_EXTRA_FUNC();
	if (m_tradeProvidingImprovements == NULL)
	{

		if (m_tradeProvidingImprovements == NULL)
		{
			const int eBonus = GC.getInfoTypeForString(m_szType);
			std::vector<std::pair<ImprovementTypes, BuildTypes> >* tradeProvidingImprovements = new std::vector<std::pair<ImprovementTypes, BuildTypes> >();

			for (int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
			{
				const BuildTypes eBuild = static_cast<BuildTypes>(iJ);
				const ImprovementTypes eImp = GC.getBuildInfo(eBuild).getImprovement();

				if (eImp != NO_IMPROVEMENT && GC.getImprovementInfo(eImp).isImprovementBonusTrade(eBonus))
				{
					tradeProvidingImprovements->push_back(std::make_pair(eImp, eBuild));
				}
			}

			m_tradeProvidingImprovements = (volatile std::vector<std::pair<ImprovementTypes, BuildTypes> >*)tradeProvidingImprovements;
		}
	}

	return (const std::vector<std::pair<ImprovementTypes, BuildTypes> >*)m_tradeProvidingImprovements;
}