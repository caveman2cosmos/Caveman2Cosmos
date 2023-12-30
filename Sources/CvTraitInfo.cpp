#include "CvGameCoreDLL.h"
#include "CvTraitInfo.h"
#include "CvGlobals.h"
#include "CvInfoUtil.h"
#include "CvGameAI.h"

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvTraitInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvTraitInfo::CvTraitInfo()
	: m_iHealth(0)
	, m_iHappiness(0)
	, m_iMaxAnarchy(-1)
	, m_iUpkeepModifier(0)
	, m_iLevelExperienceModifier(0)
	, m_iGreatPeopleRateModifier(0)
	, m_iGreatGeneralRateModifier(0)
	, m_iDomesticGreatGeneralRateModifier(0)
	, m_iMaxGlobalBuildingProductionModifier(0)
	, m_iMaxTeamBuildingProductionModifier(0)
	, m_iMaxPlayerBuildingProductionModifier(0)

	, m_iRevIdxLocal(0)
	, m_iRevIdxNational(0)
	, m_iRevIdxDistanceModifier(0)
	, m_iRevIdxHolyCityGood(0)
	, m_iRevIdxHolyCityBad(0)
	, m_fRevIdxNationalityMod(0)
	, m_fRevIdxGoodReligionMod(0)
	, m_fRevIdxBadReligionMod(0)
	, m_bNonStateReligionCommerce(false)
	, m_bUpgradeAnywhere(false)

	, m_paiExtraYieldThreshold(NULL)
	, m_paiTradeYieldModifier(NULL)
	, m_paiCommerceChange(NULL)
	, m_paiCommerceModifier(NULL)
	, m_ppbFreePromotionUnitCombats(NULL)

	, m_PropertyManipulators()
	//TB Traits Mods begin
	, m_PropertyManipulatorsNull()
	//Textual References
	, m_iPrereqTrait(NO_TRAIT)
	, m_iPrereqOrTrait1(NO_TRAIT)
	, m_iPrereqOrTrait2(NO_TRAIT)
	, m_ePromotionLine(NO_PROMOTIONLINE)
	, m_iGreatPeopleUnitType(NO_UNIT)
	, m_ePrereqTech(NO_TECH)

	, m_eEraAdvanceFreeSpecialistType(NO_SPECIALIST)
	, m_iGoldenAgeonBirthofGreatPeopleType(NO_UNIT)
	//integers
	, m_iWarWearinessAccumulationModifier(0)
	, m_iCivicAnarchyTimeModifier(0)
	, m_iReligiousAnarchyTimeModifier(0)
	, m_iImprovementUpgradeRateModifier(0)
	, m_iWorkerSpeedModifier(0)
	, m_iMaxConscript(0)
	, m_iDistanceMaintenanceModifier(0)
	, m_iNumCitiesMaintenanceModifier(0)
	, m_iCorporationMaintenanceModifier(0)
	, m_iStateReligionGreatPeopleRateModifier(0)
	, m_iFreeExperience(0)
	, m_iFreeUnitUpkeepCivilian(0)
	, m_iFreeUnitUpkeepMilitary(0)
	, m_iFreeUnitUpkeepCivilianPopPercent(0)
	, m_iFreeUnitUpkeepMilitaryPopPercent(0)
	, m_iCivilianUnitUpkeepMod(0)
	, m_iMilitaryUnitUpkeepMod(0)
	, m_iHappyPerMilitaryUnit(0)
	, m_iLargestCityHappiness(0)
	, m_iFreeSpecialist(0)
	, m_iTradeRoutes(0)
	, m_iStateReligionHappiness(0)
	, m_iNonStateReligionHappiness(0)
	, m_iStateReligionUnitProductionModifier(0)
	, m_iStateReligionBuildingProductionModifier(0)
	, m_iStateReligionFreeExperience(0)
	, m_iExpInBorderModifier(0)
	, m_iCityDefenseBonus(0)
	, m_iMilitaryProductionModifier(0)
	, m_iAttitudeModifier(0)
	, m_iLinePriority(0)
	, m_iEspionageDefense(0)
	, m_iMinAnarchy(0)
	, m_iMaxTradeRoutesChange(0)
	, m_iGoldenAgeDurationModifier(0)
	, m_iGreatPeopleRateChange(0)
	, m_iHurryAngerModifier(0)
	, m_iHurryCostModifier(0)
	, m_iEnemyWarWearinessModifier(0)
	, m_iForeignTradeRouteModifier(0)
	, m_iBombardDefense(0)
	, m_iUnitUpgradePriceModifier(0)
	, m_iCoastalTradeRoutes(0)

	, m_iGlobalPopulationgrowthratepercentage(0)
	, m_iCityStartCulture(0)
	, m_iGlobalAirUnitCapacity(0)
	, m_iCapitalXPModifier(0)
	, m_iHolyCityofStateReligionXPModifier(0)
	, m_iHolyCityofNonStateReligionXPModifier(0)
	, m_iBonusPopulationinNewCities(0)
	, m_iMissileRange(0)
	, m_iFlightOperationRange(0)
	, m_iNavalCargoSpace(0)
	, m_iMissileCargoSpace(0)
	, m_iNationalCaptureProbabilityModifier(0)
	, m_iNationalCaptureResistanceModifier(0)
	, m_iStateReligionSpreadProbabilityModifier(0)
	, m_iNonStateReligionSpreadProbabilityModifier(0)
	, m_iFreedomFighterChange(0)
	//booleans
	, m_bMilitaryFoodProduction(false)
	, m_bNegativeTrait(false)
	, m_bImpurePropertyManipulators(false)
	, m_bImpurePromotions(false)
	, m_bCivilizationTrait(false)
	, m_bAllowsInquisitions(false)
	, m_bCoastalAIInfluence(false)
	, m_bBarbarianSelectionOnly(false)
	, m_bCitiesStartwithStateReligion(false)
	, m_bDraftsOnCityCapture(false)
	, m_bFreeSpecialistperWorldWonder(false)
	, m_bFreeSpecialistperNationalWonder(false)
	, m_bFreeSpecialistperTeamProject(false)
	, m_bExtraGoody(false)
	, m_bAllReligionsActive(false)
	, m_bBansNonStateReligions(false)
	, m_bFreedomFighter(false)
	//arrays
	, m_ppaiSpecialistYieldChange(NULL)
	, m_piYieldModifier(NULL)
	, m_piCapitalYieldModifier(NULL)
	, m_piCapitalCommerceModifier(NULL)
	, m_piSpecialistExtraCommerce(NULL)
	, m_piSpecialistExtraYield(NULL)
	, m_piYieldChange(NULL)
	, m_ppaiSpecialistCommerceChange(NULL)
	, m_piFlavorValue(NULL)
	, m_paiLessYieldThreshold(NULL)
	, m_piSeaPlotYieldChanges(NULL)
	, m_ppaiImprovementYieldChange(NULL)
	, m_piGoldenAgeYieldChanges(NULL)
	, m_piGoldenAgeCommerceChanges(NULL)
	//For Pure Traits
	, m_paiExtraYieldThresholdFiltered(NULL)
	, m_paiTradeYieldModifierFiltered(NULL)
	, m_paiCommerceChangeFiltered(NULL)
	, m_paiCommerceModifierFiltered(NULL)
	, m_ppaiSpecialistYieldChangeFiltered(NULL)
	, m_piYieldModifierFiltered(NULL)
	, m_piCapitalYieldModifierFiltered(NULL)
	, m_piCapitalCommerceModifierFiltered(NULL)
	, m_piSpecialistExtraCommerceFiltered(NULL)
	, m_piSpecialistExtraYieldFiltered(NULL)
	, m_piYieldChangeFiltered(NULL)
	, m_ppaiSpecialistCommerceChangeFiltered(NULL)
	, m_paiLessYieldThresholdFiltered(NULL)
	, m_piSeaPlotYieldChangesFiltered(NULL)
	, m_ppaiImprovementYieldChangeFiltered(NULL)

	, m_piGoldenAgeYieldChangesFiltered(NULL)
	, m_piGoldenAgeCommerceChangesFiltered(NULL)
	//TB Traits Mods end
	, m_piBonusHappinessChangesFiltered(NULL)
{
	CvInfoUtil(this).initDataMembers();
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvTraitInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvTraitInfo::~CvTraitInfo()
{
	PROFILE_EXTRA_FUNC();
	SAFE_DELETE_ARRAY(m_paiExtraYieldThreshold);
	SAFE_DELETE_ARRAY(m_paiTradeYieldModifier);
	SAFE_DELETE_ARRAY(m_paiCommerceChange);
	SAFE_DELETE_ARRAY(m_paiCommerceModifier);
	SAFE_DELETE_ARRAY2(m_ppbFreePromotionUnitCombats, GC.getNumPromotionInfos());
	SAFE_DELETE_ARRAY2(m_ppaiSpecialistYieldChange, GC.getNumSpecialistInfos());
	SAFE_DELETE_ARRAY(m_piYieldModifier);
	SAFE_DELETE_ARRAY(m_piCapitalYieldModifier);
	SAFE_DELETE_ARRAY(m_piCapitalCommerceModifier);
	SAFE_DELETE_ARRAY(m_piSpecialistExtraCommerce);
	SAFE_DELETE_ARRAY(m_piSpecialistExtraYield);
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_paiLessYieldThreshold);
	SAFE_DELETE_ARRAY(m_piSeaPlotYieldChanges);
	SAFE_DELETE_ARRAY(m_piGoldenAgeYieldChanges);
	SAFE_DELETE_ARRAY(m_piGoldenAgeCommerceChanges);
	SAFE_DELETE_ARRAY2(m_ppaiSpecialistCommerceChange, GC.getNumSpecialistInfos());
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY2(m_ppaiImprovementYieldChange, GC.getNumImprovementInfos());

//	 int vector utilizing struct with delayed resolution
	for (int i=0; i<(int)m_aDisallowedTraitTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aDisallowedTraitTypes[i]));
	}

	for (int i=0; i<(int)m_aBuildingProductionModifiers.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aBuildingProductionModifiers[i]));
	}

	for (int i=0; i<(int)m_aUnitProductionModifiers.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aUnitProductionModifiers[i]));
	}

	for (int i=0; i<(int)m_aCivicOptionNoUpkeepTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aCivicOptionNoUpkeepTypes[i]));
	}


	SAFE_DELETE_ARRAY(m_paiExtraYieldThresholdFiltered);
	SAFE_DELETE_ARRAY(m_paiTradeYieldModifierFiltered);
	SAFE_DELETE_ARRAY(m_paiCommerceChangeFiltered);
	SAFE_DELETE_ARRAY(m_paiCommerceModifierFiltered);
	SAFE_DELETE_ARRAY(m_piSeaPlotYieldChangesFiltered);
	SAFE_DELETE_ARRAY(m_piGoldenAgeYieldChangesFiltered);
	SAFE_DELETE_ARRAY(m_piGoldenAgeCommerceChangesFiltered);
	SAFE_DELETE_ARRAY(m_piYieldModifierFiltered);
	SAFE_DELETE_ARRAY(m_piCapitalYieldModifierFiltered);
	SAFE_DELETE_ARRAY(m_piCapitalCommerceModifierFiltered);
	SAFE_DELETE_ARRAY(m_piSpecialistExtraCommerceFiltered);
	SAFE_DELETE_ARRAY(m_piSpecialistExtraYieldFiltered);
	SAFE_DELETE_ARRAY(m_piYieldChangeFiltered);
	SAFE_DELETE_ARRAY(m_paiLessYieldThresholdFiltered);
	SAFE_DELETE_ARRAY2(m_ppaiSpecialistYieldChangeFiltered, GC.getNumSpecialistInfos());
	SAFE_DELETE_ARRAY2(m_ppaiSpecialistCommerceChangeFiltered, GC.getNumSpecialistInfos());
	SAFE_DELETE_ARRAY2(m_ppaiImprovementYieldChangeFiltered, GC.getNumImprovementInfos());

	GC.removeDelayedResolution((int*)&m_iPrereqTrait);
	GC.removeDelayedResolution((int*)&m_iPrereqOrTrait1);
	GC.removeDelayedResolution((int*)&m_iPrereqOrTrait2);

	CvInfoUtil(this).uninitDataMembers();
}

int CvTraitInfo::getHealth() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iHealth > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iHealth < 0)
		{
			return 0;
		}
	}
	return m_iHealth;
}

int CvTraitInfo::getHappiness() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iHappiness > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iHappiness < 0)
		{
			return 0;
		}
	}
	return m_iHappiness;
}

int CvTraitInfo::getMaxAnarchy() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iMaxAnarchy > -1)
		{
			return -1;
		}
	}
	return m_iMaxAnarchy;
}

int CvTraitInfo::getUpkeepModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iUpkeepModifier < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iUpkeepModifier > 0)
		{
			return 0;
		}
	}
	return m_iUpkeepModifier;
}

int CvTraitInfo::getLevelExperienceModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iLevelExperienceModifier < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iLevelExperienceModifier > 0)
		{
			return 0;
		}
	}
	return m_iLevelExperienceModifier;
}

int CvTraitInfo::getGreatPeopleRateModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iGreatPeopleRateModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iGreatPeopleRateModifier < 0)
		{
			return 0;
		}
	}
	return m_iGreatPeopleRateModifier;
}

int CvTraitInfo::getGreatGeneralRateModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iGreatGeneralRateModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iGreatGeneralRateModifier < 0)
		{
			return 0;
		}
	}
	return m_iGreatGeneralRateModifier;
}

int CvTraitInfo::getDomesticGreatGeneralRateModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iDomesticGreatGeneralRateModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iDomesticGreatGeneralRateModifier < 0)
		{
			return 0;
		}
	}
	return m_iDomesticGreatGeneralRateModifier;
}

int CvTraitInfo::getMaxGlobalBuildingProductionModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iMaxGlobalBuildingProductionModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iMaxGlobalBuildingProductionModifier < 0)
		{
			return 0;
		}
	}
	return m_iMaxGlobalBuildingProductionModifier;
}

int CvTraitInfo::getMaxTeamBuildingProductionModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iMaxTeamBuildingProductionModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iMaxTeamBuildingProductionModifier < 0)
		{
			return 0;
		}
	}
	return m_iMaxTeamBuildingProductionModifier;
}

int CvTraitInfo::getMaxPlayerBuildingProductionModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iMaxPlayerBuildingProductionModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iMaxPlayerBuildingProductionModifier < 0)
		{
			return 0;
		}
	}
	return m_iMaxPlayerBuildingProductionModifier;
}

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		RevTrait Effects														*/
/********************************************************************************/

int CvTraitInfo::getRevIdxLocal() const
//Note: Positive is penalty
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iRevIdxLocal < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iRevIdxLocal > 0)
		{
			return 0;
		}
	}
	return m_iRevIdxLocal;
}

int CvTraitInfo::getRevIdxNational() const
{
//Note: Positive is Penalty
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iRevIdxNational < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iRevIdxNational > 0)
		{
			return 0;
		}
	}
	return m_iRevIdxNational;
}

int CvTraitInfo::getRevIdxDistanceModifier() const
{
//Note: Positive is penalty
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iRevIdxDistanceModifier < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iRevIdxDistanceModifier > 0)
		{
			return 0;
		}
	}
	return m_iRevIdxDistanceModifier;
}

int CvTraitInfo::getRevIdxHolyCityGood() const
{
//Note: Positive is bonus
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iRevIdxHolyCityGood > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iRevIdxHolyCityGood < 0)
		{
			return 0;
		}
	}
	return m_iRevIdxHolyCityGood;
}

int CvTraitInfo::getRevIdxHolyCityBad() const
{
//Note: Positive is Penalty
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iRevIdxHolyCityBad < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iRevIdxHolyCityBad > 0)
		{
			return 0;
		}
	}
	return m_iRevIdxHolyCityBad;
}

float CvTraitInfo::getRevIdxNationalityMod() const
{
//Note: Positive is Penalty
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_fRevIdxNationalityMod < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_fRevIdxNationalityMod > 0)
		{
			return 0;
		}
	}
	return m_fRevIdxNationalityMod;
}

float CvTraitInfo::getRevIdxBadReligionMod() const
{
//Note: Positive is Penalty
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_fRevIdxBadReligionMod < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_fRevIdxBadReligionMod > 0)
		{
			return 0;
		}
	}
	return m_fRevIdxBadReligionMod;
}

float CvTraitInfo::getRevIdxGoodReligionMod() const
{
//Note: Positive is Bonus
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_fRevIdxGoodReligionMod > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_fRevIdxGoodReligionMod < 0)
		{
			return 0;
		}
	}
	return m_fRevIdxGoodReligionMod;
}

bool CvTraitInfo::isNonStateReligionCommerce() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_bNonStateReligionCommerce)
		{
				return false;
		}
	}
	return m_bNonStateReligionCommerce;
}

bool CvTraitInfo::isUpgradeAnywhere() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_bUpgradeAnywhere)
		{
				return false;
		}
	}
	return m_bUpgradeAnywhere;
}
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

const char* CvTraitInfo::getShortDescription() const
{
	return m_szShortDescription;
}

void CvTraitInfo::setShortDescription(const char* szVal)
{
	m_szShortDescription = szVal;
}

// Arrays

int CvTraitInfo::getExtraYieldThreshold(int i) const
{
	if (!m_paiExtraYieldThreshold)
		return -1;

	if (m_paiExtraYieldThresholdFiltered && GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_paiExtraYieldThreshold[i] > -1)
		{
			m_paiExtraYieldThresholdFiltered[i] = -1;
		}
		else
		{
			m_paiExtraYieldThresholdFiltered[i] = m_paiExtraYieldThreshold[i];
		}
		return m_paiExtraYieldThresholdFiltered[i];
	}
	else
	{
		return m_paiExtraYieldThreshold[i];
	}
}

int CvTraitInfo::getTradeYieldModifier(int i) const
{
	if (m_paiTradeYieldModifier)
	{
		if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS) && m_paiTradeYieldModifierFiltered)
		{
			if (isNegativeTrait() && m_paiTradeYieldModifier[i] > 0)
			{
				m_paiTradeYieldModifierFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_paiTradeYieldModifier[i] < 0)
			{
				m_paiTradeYieldModifierFiltered[i] = 0;
			}
			else
			{
				m_paiTradeYieldModifierFiltered[i] = m_paiTradeYieldModifier[i];
			}
			return m_paiTradeYieldModifierFiltered ? m_paiTradeYieldModifierFiltered[i] : 0;
		}
	}
	return m_paiTradeYieldModifier ? m_paiTradeYieldModifier[i] : 0;
}

int CvTraitInfo::getCommerceChange(int i) const
{
	if (m_paiCommerceChange)
	{
		if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS) && m_paiCommerceChangeFiltered)
		{
			if (isNegativeTrait() && m_paiCommerceChange[i] > 0)
			{
				m_paiCommerceChangeFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_paiCommerceChange[i] < 0)
			{
				m_paiCommerceChangeFiltered[i] = 0;
			}
			else
			{
				m_paiCommerceChangeFiltered[i] = m_paiCommerceChange[i];
			}
			return m_paiCommerceChangeFiltered ? m_paiCommerceChangeFiltered[i] : 0;
		}
	}
	return m_paiCommerceChange ? m_paiCommerceChange[i] : 0;
}

int CvTraitInfo::getCommerceModifier(int i) const
{
	if (m_paiCommerceModifier)
	{
		if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS) && m_paiCommerceModifierFiltered)
		{
			if (isNegativeTrait() && m_paiCommerceModifier[i] > 0)
			{
				m_paiCommerceModifierFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_paiCommerceModifier[i] < 0)
			{
				m_paiCommerceModifierFiltered[i] = 0;
			}
			else
			{
				m_paiCommerceModifierFiltered[i] = m_paiCommerceModifier[i];
			}
			return m_paiCommerceModifierFiltered ? m_paiCommerceModifierFiltered[i] : 0;
		}
	}
	return m_paiCommerceModifier ? m_paiCommerceModifier[i] : 0;
}

bool CvTraitInfo::isFreePromotionUnitCombats(int i, int j) const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS) && isImpurePromotions())
	{
		return false;
	}
	return (m_ppbFreePromotionUnitCombats && m_ppbFreePromotionUnitCombats[i]) ? m_ppbFreePromotionUnitCombats[i][j] : false;
}

//TB Traits Mods begin

PromotionLineTypes CvTraitInfo::getPromotionLine() const
{
	return m_ePromotionLine;
}

int CvTraitInfo::getGreatPeopleUnitType() const
{
	return m_iGreatPeopleUnitType;
}

SpecialistTypes CvTraitInfo::getEraAdvanceFreeSpecialistType() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_eEraAdvanceFreeSpecialistType != NO_SPECIALIST)
		{
				return NO_SPECIALIST;
		}
	}
	return m_eEraAdvanceFreeSpecialistType;
}

int CvTraitInfo::getGoldenAgeonBirthofGreatPeopleType() const
{
	if (isNegativeTrait() && GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		return NO_UNIT;
	}
	return m_iGoldenAgeonBirthofGreatPeopleType;
}

int CvTraitInfo::getWarWearinessAccumulationModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iWarWearinessAccumulationModifier < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iWarWearinessAccumulationModifier > 0)
		{
			return 0;
		}
	}
	return m_iWarWearinessAccumulationModifier;
}

int CvTraitInfo::getCivicAnarchyTimeModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iCivicAnarchyTimeModifier < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iCivicAnarchyTimeModifier > 0)
		{
			return 0;
		}
	}
	return m_iCivicAnarchyTimeModifier;
}

int CvTraitInfo::getReligiousAnarchyTimeModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iReligiousAnarchyTimeModifier < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iReligiousAnarchyTimeModifier > 0)
		{
			return 0;
		}
	}
	return m_iReligiousAnarchyTimeModifier;
}

int CvTraitInfo::getImprovementUpgradeRateModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iImprovementUpgradeRateModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iImprovementUpgradeRateModifier < 0)
		{
			return 0;
		}
	}
	return m_iImprovementUpgradeRateModifier;
}

int CvTraitInfo::getWorkerSpeedModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iWorkerSpeedModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iWorkerSpeedModifier < 0)
		{
			return 0;
		}
	}
	return m_iWorkerSpeedModifier;
}

int CvTraitInfo::getMaxConscript() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iMaxConscript > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iMaxConscript < 0)
		{
			return 0;
		}
	}
	return m_iMaxConscript;
}

int CvTraitInfo::getDistanceMaintenanceModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iDistanceMaintenanceModifier < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iDistanceMaintenanceModifier > 0)
		{
			return 0;
		}
	}
	return m_iDistanceMaintenanceModifier;
}

int CvTraitInfo::getNumCitiesMaintenanceModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iNumCitiesMaintenanceModifier < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iNumCitiesMaintenanceModifier > 0)
		{
			return 0;
		}
	}
	return m_iNumCitiesMaintenanceModifier;
}

int CvTraitInfo::getCorporationMaintenanceModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iCorporationMaintenanceModifier < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iCorporationMaintenanceModifier > 0)
		{
			return 0;
		}
	}
	return m_iCorporationMaintenanceModifier;
}

int CvTraitInfo::getStateReligionGreatPeopleRateModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iStateReligionGreatPeopleRateModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iStateReligionGreatPeopleRateModifier < 0)
		{
			return 0;
		}
	}
	return m_iStateReligionGreatPeopleRateModifier;
}

int CvTraitInfo::getFreeExperience() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iFreeExperience > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iFreeExperience < 0)
		{
			return 0;
		}
	}
	return m_iFreeExperience;
}

int CvTraitInfo::getFreeUnitUpkeepCivilian() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait())
		{
			if (m_iFreeUnitUpkeepCivilian > 0)
			{
				return 0;
			}
		}
		else if (m_iFreeUnitUpkeepCivilian < 0)
		{
			return 0;
		}
	}
	return m_iFreeUnitUpkeepCivilian;
}

int CvTraitInfo::getFreeUnitUpkeepMilitary() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait())
		{
			if (m_iFreeUnitUpkeepMilitary > 0)
			{
				return 0;
			}
		}
		else if (m_iFreeUnitUpkeepMilitary < 0)
		{
			return 0;
		}
	}
	return m_iFreeUnitUpkeepMilitary;
}

int CvTraitInfo::getFreeUnitUpkeepCivilianPopPercent() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait())
		{
			if (m_iFreeUnitUpkeepCivilianPopPercent > 0)
			{
				return 0;
			}
		}
		else if (m_iFreeUnitUpkeepCivilianPopPercent < 0)
		{
			return 0;
		}
	}
	return m_iFreeUnitUpkeepCivilianPopPercent;
}

int CvTraitInfo::getFreeUnitUpkeepMilitaryPopPercent() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait())
		{
			if (m_iFreeUnitUpkeepMilitaryPopPercent > 0)
			{
				return 0;
			}
		}
		else if (m_iFreeUnitUpkeepMilitaryPopPercent < 0)
		{
			return 0;
		}
	}
	return m_iFreeUnitUpkeepMilitaryPopPercent;
}

int CvTraitInfo::getCivilianUnitUpkeepMod() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait())
		{
			if (m_iCivilianUnitUpkeepMod < 0)
			{
				return 0;
			}
		}
		else if (m_iCivilianUnitUpkeepMod > 0)
		{
			return 0;
		}
	}
	return m_iCivilianUnitUpkeepMod;
}

int CvTraitInfo::getMilitaryUnitUpkeepMod() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait())
		{
			if (m_iMilitaryUnitUpkeepMod < 0)
			{
				return 0;
			}
		}
		else if (m_iMilitaryUnitUpkeepMod > 0)
		{
			return 0;
		}
	}
	return m_iMilitaryUnitUpkeepMod;
}

int CvTraitInfo::getHappyPerMilitaryUnit() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iHappyPerMilitaryUnit > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iHappyPerMilitaryUnit < 0)
		{
			return 0;
		}
	}
	return m_iHappyPerMilitaryUnit;
}

int CvTraitInfo::getLargestCityHappiness() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iLargestCityHappiness > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iLargestCityHappiness < 0)
		{
			return 0;
		}
	}
	return m_iLargestCityHappiness;
}

int CvTraitInfo::getFreeSpecialist() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait())
		{
			if (m_iFreeSpecialist > 0)
			{
				return 0;
			}
		}
		else if (m_iFreeSpecialist < 0)
		{
			return 0;
		}
	}
	return m_iFreeSpecialist;
}

int CvTraitInfo::getTradeRoutes() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait())
		{
			if (m_iTradeRoutes > 0)
			{
				return 0;
			}
		}
		else if (m_iTradeRoutes < 0)
		{
			return 0;
		}
	}
	return m_iTradeRoutes;
}

int CvTraitInfo::getStateReligionHappiness() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait())
		{
			if (m_iStateReligionHappiness > 0)
			{
				return 0;
			}
		}
		else if (m_iStateReligionHappiness < 0)
		{
			return 0;
		}
	}
	return m_iStateReligionHappiness;
}

int CvTraitInfo::getNonStateReligionHappiness() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iNonStateReligionHappiness > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iNonStateReligionHappiness < 0)
		{
			return 0;
		}
	}
	return m_iNonStateReligionHappiness;
}

int CvTraitInfo::getStateReligionUnitProductionModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iStateReligionUnitProductionModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iStateReligionUnitProductionModifier < 0)
		{
			return 0;
		}
	}
	return m_iStateReligionUnitProductionModifier;
}

int CvTraitInfo::getStateReligionBuildingProductionModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iStateReligionBuildingProductionModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iStateReligionBuildingProductionModifier < 0)
		{
			return 0;
		}
	}
	return m_iStateReligionBuildingProductionModifier;
}

int CvTraitInfo::getStateReligionFreeExperience() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iStateReligionFreeExperience > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iStateReligionFreeExperience < 0)
		{
			return 0;
		}
	}
	return m_iStateReligionFreeExperience;
}

int CvTraitInfo::getExpInBorderModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iExpInBorderModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iExpInBorderModifier < 0)
		{
			return 0;
		}
	}
	return m_iExpInBorderModifier;
}

int CvTraitInfo::getCityDefenseBonus() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iCityDefenseBonus > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iCityDefenseBonus < 0)
		{
			return 0;
		}
	}
	return m_iCityDefenseBonus;
}

int CvTraitInfo::getMilitaryProductionModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iMilitaryProductionModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iMilitaryProductionModifier < 0)
		{
			return 0;
		}
	}
	return m_iMilitaryProductionModifier;
}

int CvTraitInfo::getAttitudeModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iAttitudeModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iAttitudeModifier < 0)
		{
			return 0;
		}
	}
	return m_iAttitudeModifier;
}

int CvTraitInfo::getLinePriority() const
{
	return m_iLinePriority;
}

int CvTraitInfo::getEspionageDefense() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iEspionageDefense > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iEspionageDefense < 0)
		{
			return 0;
		}
	}
	return m_iEspionageDefense;
}

int CvTraitInfo::getMinAnarchy() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iMinAnarchy < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iMinAnarchy > 0)
		{
			return 0;
		}
	}
	return m_iMinAnarchy;
}

int CvTraitInfo::getMaxTradeRoutesChange() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iMaxTradeRoutesChange > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iMaxTradeRoutesChange < 0)
		{
			return 0;
		}
	}
	return m_iMaxTradeRoutesChange;
}

int CvTraitInfo::getGoldenAgeDurationModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iGoldenAgeDurationModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iGoldenAgeDurationModifier < 0)
		{
			return 0;
		}
	}
	return m_iGoldenAgeDurationModifier;
}

int CvTraitInfo::getGreatPeopleRateChange() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iGreatPeopleRateChange > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iGreatPeopleRateChange < 0)
		{
			return 0;
		}
	}
	return m_iGreatPeopleRateChange;
}

int CvTraitInfo::getHurryAngerModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iHurryAngerModifier < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iHurryAngerModifier > 0)
		{
			return 0;
		}
	}
	return m_iHurryAngerModifier;
}

int CvTraitInfo::getHurryCostModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iHurryCostModifier < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iHurryCostModifier > 0)
		{
			return 0;
		}
	}
	return m_iHurryCostModifier;
}

int CvTraitInfo::getEnemyWarWearinessModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iEnemyWarWearinessModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iEnemyWarWearinessModifier < 0)
		{
			return 0;
		}
	}
	return m_iEnemyWarWearinessModifier;
}

int CvTraitInfo::getForeignTradeRouteModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iForeignTradeRouteModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iForeignTradeRouteModifier < 0)
		{
			return 0;
		}
	}
	return m_iForeignTradeRouteModifier;
}

int CvTraitInfo::getBombardDefense() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iBombardDefense > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iBombardDefense < 0)
		{
			return 0;
		}
	}
	return m_iBombardDefense;
}

int CvTraitInfo::getUnitUpgradePriceModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iUnitUpgradePriceModifier < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iUnitUpgradePriceModifier > 0)
		{
			return 0;
		}
	}
	return m_iUnitUpgradePriceModifier;
}

int CvTraitInfo::getCoastalTradeRoutes() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iCoastalTradeRoutes > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iCoastalTradeRoutes < 0)
		{
			return 0;
		}
	}
	return m_iCoastalTradeRoutes;
}

int CvTraitInfo::getGlobalPopulationgrowthratepercentage() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iGlobalPopulationgrowthratepercentage < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iGlobalPopulationgrowthratepercentage > 0)
		{
			return 0;
		}
	}
	return m_iGlobalPopulationgrowthratepercentage;
}

int CvTraitInfo::getCityStartCulture(bool bForLoad) const
{
	if (!bForLoad && GC.getGame().isOption(GAMEOPTION_CULTURE_1_CITY_TILE_FOUNDING))
	{
		return 0;
	}
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iCityStartCulture > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iCityStartCulture < 0)
		{
			return 0;
		}
	}
	return m_iCityStartCulture;
}

int CvTraitInfo::getGlobalAirUnitCapacity() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iGlobalAirUnitCapacity > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iGlobalAirUnitCapacity < 0)
		{
			return 0;
		}
	}
	return m_iGlobalAirUnitCapacity;
}

int CvTraitInfo::getCapitalXPModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iCapitalXPModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iCapitalXPModifier < 0)
		{
			return 0;
		}
	}
	return m_iCapitalXPModifier;
}

int CvTraitInfo::getHolyCityofStateReligionXPModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iHolyCityofStateReligionXPModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iHolyCityofStateReligionXPModifier < 0)
		{
			return 0;
		}
	}
	return m_iHolyCityofStateReligionXPModifier;
}

int CvTraitInfo::getHolyCityofNonStateReligionXPModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iHolyCityofNonStateReligionXPModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iHolyCityofNonStateReligionXPModifier < 0)
		{
			return 0;
		}
	}
	return m_iHolyCityofNonStateReligionXPModifier;
}

int CvTraitInfo::getBonusPopulationinNewCities() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iBonusPopulationinNewCities > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iBonusPopulationinNewCities < 0)
		{
			return 0;
		}
	}
	return m_iBonusPopulationinNewCities;
}

int CvTraitInfo::getMissileRange() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iMissileRange > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iMissileRange < 0)
		{
			return 0;
		}
	}
	return m_iMissileRange;
}

int CvTraitInfo::getFlightOperationRange() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iFlightOperationRange > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iFlightOperationRange < 0)
		{
			return 0;
		}
	}
	return m_iFlightOperationRange;
}

int CvTraitInfo::getNavalCargoSpace() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iNavalCargoSpace > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iNavalCargoSpace < 0)
		{
			return 0;
		}
	}
	return m_iNavalCargoSpace;
}

int CvTraitInfo::getMissileCargoSpace() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iMissileCargoSpace > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iMissileCargoSpace < 0)
		{
			return 0;
		}
	}
	return m_iMissileCargoSpace;
}

int CvTraitInfo::getNationalCaptureProbabilityModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iNationalCaptureProbabilityModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iNationalCaptureProbabilityModifier < 0)
		{
			return 0;
		}
	}
	return m_iNationalCaptureProbabilityModifier;
}

int CvTraitInfo::getNationalCaptureResistanceModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iNationalCaptureResistanceModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iNationalCaptureResistanceModifier < 0)
		{
			return 0;
		}
	}
	return m_iNationalCaptureResistanceModifier;
}

int CvTraitInfo::getStateReligionSpreadProbabilityModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iStateReligionSpreadProbabilityModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iStateReligionSpreadProbabilityModifier < 0)
		{
			return 0;
		}
	}
	return m_iStateReligionSpreadProbabilityModifier;
}

int CvTraitInfo::getNonStateReligionSpreadProbabilityModifier() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iNonStateReligionSpreadProbabilityModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iNonStateReligionSpreadProbabilityModifier < 0)
		{
			return 0;
		}
	}
	return m_iNonStateReligionSpreadProbabilityModifier;
}

int CvTraitInfo::getFreedomFighterChange() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iFreedomFighterChange > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iFreedomFighterChange < 0)
		{
			return 0;
		}
	}
	return m_iFreedomFighterChange;
}

bool CvTraitInfo::isMilitaryFoodProduction() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_bMilitaryFoodProduction)
		{
				return false;
		}
	}
	return m_bMilitaryFoodProduction;
}

bool CvTraitInfo::isImpurePropertyManipulators() const
{
	return m_bImpurePropertyManipulators;
}

bool CvTraitInfo::isImpurePromotions() const
{
	return m_bImpurePromotions;
}

bool CvTraitInfo::isCivilizationTrait() const
{
	return m_bCivilizationTrait;
}

bool CvTraitInfo::isAllowsInquisitions() const
{
	return m_bAllowsInquisitions && (!GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS) || !isNegativeTrait());
}

bool CvTraitInfo::isCoastalAIInfluence() const
{
	return m_bCoastalAIInfluence;
}

bool CvTraitInfo::isBarbarianSelectionOnly() const
{
	return m_bBarbarianSelectionOnly;
}

bool CvTraitInfo::isCitiesStartwithStateReligion() const
{
	return m_bCitiesStartwithStateReligion && (!GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS) || !isNegativeTrait());
}

bool CvTraitInfo::isDraftsOnCityCapture() const
{
	return m_bDraftsOnCityCapture && (!GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS) || !isNegativeTrait());
}

bool CvTraitInfo::isFreeSpecialistperWorldWonder() const
{
	return m_bFreeSpecialistperWorldWonder && (!GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS) || !isNegativeTrait());
}

bool CvTraitInfo::isFreeSpecialistperNationalWonder() const
{
	return m_bFreeSpecialistperNationalWonder && (!GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS) || !isNegativeTrait());
}

bool CvTraitInfo::isFreeSpecialistperTeamProject() const
{
	return m_bFreeSpecialistperTeamProject && (!GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS) || !isNegativeTrait());
}

bool CvTraitInfo::isExtraGoody() const
{
	return m_bExtraGoody && (!GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS) || !isNegativeTrait());
}

bool CvTraitInfo::isAllReligionsActive() const
{
	return m_bAllReligionsActive && (!GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS) || !isNegativeTrait());
}

bool CvTraitInfo::isBansNonStateReligions() const
{
	return m_bBansNonStateReligions && (!GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS) || isNegativeTrait());
}

bool CvTraitInfo::isFreedomFighter() const
{
	return m_bFreedomFighter && (!GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS) || !isNegativeTrait());
}

// bool vector without delayed resolution
int CvTraitInfo::getNotOnGameOption(int i) const
{
	return m_aiNotOnGameOptions[i];
}

int CvTraitInfo::getNumNotOnGameOptions() const
{
	return (int)m_aiNotOnGameOptions.size();
}

bool CvTraitInfo::isNotOnGameOption(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumGameOptionInfos(), i);
	return algo::any_of_equal(m_aiNotOnGameOptions, i);
}

int CvTraitInfo::getOnGameOption(int i) const
{
	return m_aiOnGameOptions[i];
}

int CvTraitInfo::getNumOnGameOptions() const
{
	return (int)m_aiOnGameOptions.size();
}

bool CvTraitInfo::isOnGameOption(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumGameOptionInfos(), i);
	return algo::any_of_equal(m_aiOnGameOptions, i);
}

int CvTraitInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}

int CvTraitInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}

bool CvTraitInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}

bool CvTraitInfo::isValidTrait(bool bGameStart) const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < GC.getNumGameOptionInfos(); iI++)
	{
		if (GC.getGame().isOption((GameOptionTypes)iI))
		{
			if (isNotOnGameOption(iI)) return false;
		}
		else if (isOnGameOption(iI)) return false;
	}

	if (bGameStart && isBarbarianSelectionOnly())
	{
		return true;
	}

	if (isNegativeTrait())
	{
		if (GC.getGame().isOption(GAMEOPTION_LEADER_NO_NEGATIVE_TRAITS)
		|| bGameStart && GC.getGame().isOption(GAMEOPTION_LEADER_START_NO_POSITIVE_TRAITS) && GC.getGame().isOption(GAMEOPTION_LEADER_DEVELOPING))
		{
			return false;
		}
	}
	else if (bGameStart && GC.getGame().isOption(GAMEOPTION_LEADER_START_NO_POSITIVE_TRAITS))
	{
		return false;
	}

	if (isCivilizationTrait()) return true;

	if (GC.getGame().isOption(GAMEOPTION_LEADER_DEVELOPING))
	{
		if (getLinePriority() == 0)
		{
			return false;
		}
	}
	else if (getLinePriority() != 0)
	{
		return false;
	}
	return true;
}

//Arrays
int CvTraitInfo::getSpecialistYieldChange(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, j);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (m_ppaiSpecialistYieldChange && m_ppaiSpecialistYieldChange[i] && m_ppaiSpecialistYieldChangeFiltered && m_ppaiSpecialistYieldChangeFiltered[i])
		{
			if (isNegativeTrait() && m_ppaiSpecialistYieldChange[i][j] > 0)
			{
				m_ppaiSpecialistYieldChangeFiltered[i][j] = 0;
			}
			else if (!isNegativeTrait() && m_ppaiSpecialistYieldChange[i][j] < 0)
			{
				m_ppaiSpecialistYieldChangeFiltered[i][j] = 0;
			}
			else
			{
				m_ppaiSpecialistYieldChangeFiltered[i][j] = (m_ppaiSpecialistYieldChange && m_ppaiSpecialistYieldChange[i]) ? m_ppaiSpecialistYieldChange[i][j] : 0;
			}
			return (m_ppaiSpecialistYieldChangeFiltered && m_ppaiSpecialistYieldChangeFiltered[i]) ? m_ppaiSpecialistYieldChangeFiltered[i][j] : 0;
		}
	}
	return (m_ppaiSpecialistYieldChange && m_ppaiSpecialistYieldChange[i]) ? m_ppaiSpecialistYieldChange[i][j] : 0;
}

int* CvTraitInfo::getSpecialistYieldChangeArray(int i) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		for (int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if (m_ppaiSpecialistYieldChange && m_ppaiSpecialistYieldChange[i] && m_ppaiSpecialistYieldChangeFiltered)
			{
				if (isNegativeTrait() && m_ppaiSpecialistYieldChange[i][j] > 0)
				{
					m_ppaiSpecialistYieldChangeFiltered[i][j] = 0;
				}
				else if (!isNegativeTrait() && m_ppaiSpecialistYieldChange[i][j] < 0)
				{
					m_ppaiSpecialistYieldChangeFiltered[i][j] = 0;
				}
				else
				{
					m_ppaiSpecialistYieldChangeFiltered[i][j] = (m_ppaiSpecialistYieldChange && m_ppaiSpecialistYieldChange[i]) ? m_ppaiSpecialistYieldChange[i][j] : 0;
				}
			}
		}
		return (m_ppaiSpecialistYieldChangeFiltered) ? m_ppaiSpecialistYieldChangeFiltered[i] : 0;
	}
	return (m_ppaiSpecialistYieldChange) ? m_ppaiSpecialistYieldChange[i] : 0;
}

int CvTraitInfo::getYieldModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (m_piYieldModifier && m_piYieldModifierFiltered)
		{
			if (isNegativeTrait() && m_piYieldModifier[i] > 0)
			{
				m_piYieldModifierFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_piYieldModifier[i] < 0)
			{
				m_piYieldModifierFiltered[i] = 0;
			}
			else
			{
				m_piYieldModifierFiltered[i] = m_piYieldModifier ? m_piYieldModifier[i] : 0;
			}
			return m_piYieldModifierFiltered ? m_piYieldModifierFiltered[i] : 0;
		}
	}
	return m_piYieldModifier ? m_piYieldModifier[i] : 0;
}

int* CvTraitInfo::getYieldModifierArray() const
{
	PROFILE_EXTRA_FUNC();
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			if (m_piYieldModifier && m_piYieldModifier[i])
			{
				if (isNegativeTrait() && m_piYieldModifier[i] > 0)
				{
					m_piYieldModifierFiltered[i] = 0;
				}
				else if (!isNegativeTrait() && m_piYieldModifier[i] < 0)
				{
					m_piYieldModifierFiltered[i] = 0;
				}
				else
				{
					m_piYieldModifierFiltered[i] = m_piYieldModifier ? m_piYieldModifier[i] : 0;
				}
			}
		}
		return m_piYieldModifierFiltered;
	}
	return m_piYieldModifier;
}

int CvTraitInfo::getCapitalYieldModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (m_piCapitalYieldModifier && m_piCapitalYieldModifierFiltered)
		{
			if (isNegativeTrait() && m_piCapitalYieldModifier[i] > 0)
			{
				m_piCapitalYieldModifierFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_piCapitalYieldModifier[i] < 0)
			{
				m_piCapitalYieldModifierFiltered[i] = 0;
			}
			else
			{
				m_piCapitalYieldModifierFiltered[i] = m_piCapitalYieldModifier ? m_piCapitalYieldModifier[i] : 0;
			}
			return m_piCapitalYieldModifierFiltered ? m_piCapitalYieldModifierFiltered[i] : 0;
		}
	}
	return m_piCapitalYieldModifier ? m_piCapitalYieldModifier[i] : 0;
}

int* CvTraitInfo::getCapitalYieldModifierArray() const
{
	PROFILE_EXTRA_FUNC();
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			if (m_piCapitalYieldModifier && m_piCapitalYieldModifier[i])
			{
				if (isNegativeTrait() && m_piCapitalYieldModifier[i] > 0)
				{
					m_piCapitalYieldModifierFiltered[i] = 0;
				}
				else if (!isNegativeTrait() && m_piCapitalYieldModifier[i] < 0)
				{
					m_piCapitalYieldModifierFiltered[i] = 0;
				}
				else
				{
					m_piCapitalYieldModifierFiltered[i] = m_piCapitalYieldModifier ? m_piCapitalYieldModifier[i] : 0;
				}
			}
		}
		return m_piCapitalYieldModifierFiltered;
	}
	return m_piCapitalYieldModifier;
}

int CvTraitInfo::getCapitalCommerceModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (m_piCapitalCommerceModifier && m_piCapitalCommerceModifierFiltered)
		{
			if (isNegativeTrait() && m_piCapitalCommerceModifier[i] > 0)
			{
				m_piCapitalCommerceModifierFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_piCapitalCommerceModifier[i] < 0)
			{
				m_piCapitalCommerceModifierFiltered[i] = 0;
			}
			else
			{
				m_piCapitalCommerceModifierFiltered[i] = m_piCapitalCommerceModifier ? m_piCapitalCommerceModifier[i] : 0;
			}
			return m_piCapitalCommerceModifierFiltered ? m_piCapitalCommerceModifierFiltered[i] : 0;
		}
	}
	return m_piCapitalCommerceModifier ? m_piCapitalCommerceModifier[i] : 0;
}

int* CvTraitInfo::getCapitalCommerceModifierArray() const
{
	PROFILE_EXTRA_FUNC();
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		for (int i = 0; i < NUM_COMMERCE_TYPES; i++)
		{
			if (m_piCapitalCommerceModifier && m_piCapitalCommerceModifier[i])
			{
				if (isNegativeTrait() && m_piCapitalCommerceModifier[i] > 0)
				{
					m_piCapitalCommerceModifierFiltered[i] = 0;
				}
				else if (!isNegativeTrait() && m_piCapitalCommerceModifier[i] < 0)
				{
					m_piCapitalCommerceModifierFiltered[i] = 0;
				}
				else
				{
					m_piCapitalCommerceModifierFiltered[i] = m_piCapitalCommerceModifier ? m_piCapitalCommerceModifier[i] : 0;
				}
			}
		}
		return m_piCapitalCommerceModifierFiltered;
	}
	return m_piCapitalCommerceModifier;
}

int CvTraitInfo::getSpecialistExtraCommerce(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (m_piSpecialistExtraCommerce && m_piSpecialistExtraCommerceFiltered)
		{
			if (isNegativeTrait() && m_piSpecialistExtraCommerce[i] > 0)
			{
				m_piSpecialistExtraCommerceFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_piSpecialistExtraCommerce[i] < 0)
			{
				m_piSpecialistExtraCommerceFiltered[i] = 0;
			}
			else
			{
				m_piSpecialistExtraCommerceFiltered[i] = m_piSpecialistExtraCommerce ? m_piSpecialistExtraCommerce[i] : 0;
			}
			return m_piSpecialistExtraCommerceFiltered ? m_piSpecialistExtraCommerceFiltered[i] : 0;
		}
	}
	return m_piSpecialistExtraCommerce ? m_piSpecialistExtraCommerce[i] : 0;
}

int* CvTraitInfo::getSpecialistExtraCommerceArray() const
{
	PROFILE_EXTRA_FUNC();
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		for (int i = 0; i < NUM_COMMERCE_TYPES; i++)
		{
			if (m_piSpecialistExtraCommerce && m_piSpecialistExtraCommerce[i])
			{
				if (isNegativeTrait() && m_piSpecialistExtraCommerce[i] > 0)
				{
					m_piSpecialistExtraCommerceFiltered[i] = 0;
				}
				else if (!isNegativeTrait() && m_piSpecialistExtraCommerce[i] < 0)
				{
					m_piSpecialistExtraCommerceFiltered[i] = 0;
				}
				else
				{
					m_piSpecialistExtraCommerceFiltered[i] = m_piSpecialistExtraCommerce ? m_piSpecialistExtraCommerce[i] : 0;
				}
			}
		}
		return m_piSpecialistExtraCommerceFiltered;
	}
	return m_piSpecialistExtraCommerce;
}

int CvTraitInfo::getSpecialistExtraYield(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (m_piSpecialistExtraYield && m_piSpecialistExtraYieldFiltered)
		{
			if (isNegativeTrait() && m_piSpecialistExtraYield[i] > 0)
			{
				m_piSpecialistExtraYieldFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_piSpecialistExtraYield[i] < 0)
			{
				m_piSpecialistExtraYieldFiltered[i] = 0;
			}
			else
			{
				m_piSpecialistExtraYieldFiltered[i] = m_piSpecialistExtraYield ? m_piSpecialistExtraYield[i] : 0;
			}
			return m_piSpecialistExtraYieldFiltered ? m_piSpecialistExtraYieldFiltered[i] : 0;
		}
	}
	return m_piSpecialistExtraYield ? m_piSpecialistExtraYield[i] : 0;
}

int* CvTraitInfo::getSpecialistExtraYieldArray() const
{
	PROFILE_EXTRA_FUNC();
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			if (m_piSpecialistExtraYield && m_piSpecialistExtraYield[i])
			{
				if (isNegativeTrait() && m_piSpecialistExtraYield[i] > 0)
				{
					m_piSpecialistExtraYieldFiltered[i] = 0;
				}
				else if (!isNegativeTrait() && m_piSpecialistExtraYield[i] < 0)
				{
					m_piSpecialistExtraYieldFiltered[i] = 0;
				}
				else
				{
					m_piSpecialistExtraYieldFiltered[i] = m_piSpecialistExtraYield ? m_piSpecialistExtraYield[i] : 0;
				}
			}
		}
		return m_piSpecialistExtraYieldFiltered;
	}
	return m_piSpecialistExtraYield;
}

int CvTraitInfo::getFlavorValue(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFlavorTypes(), i);

	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}

int CvTraitInfo::getYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (m_piYieldChange && m_piYieldChangeFiltered)
		{
			if (isNegativeTrait() && m_piYieldChange[i] > 0)
			{
				m_piYieldChangeFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_piYieldChange[i] < 0)
			{
				m_piYieldChangeFiltered[i] = 0;
			}
			else
			{
				m_piYieldChangeFiltered[i] = m_piYieldChange ? m_piYieldChange[i] : 0;
			}
			return m_piYieldChangeFiltered ? m_piYieldChangeFiltered[i] : 0;
		}
	}
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}

int CvTraitInfo::getSpecialistCommerceChange(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, j);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (m_ppaiSpecialistCommerceChange && m_ppaiSpecialistCommerceChange[i] && m_ppaiSpecialistCommerceChangeFiltered && m_ppaiSpecialistCommerceChangeFiltered[i])
		{
			if (isNegativeTrait() && m_ppaiSpecialistCommerceChange[i][j] > 0)
			{
				m_ppaiSpecialistCommerceChangeFiltered[i][j] = 0;
			}
			else if (!isNegativeTrait() && m_ppaiSpecialistCommerceChange[i][j] < 0)
			{
				m_ppaiSpecialistCommerceChangeFiltered[i][j] = 0;
			}
			else
			{
				m_ppaiSpecialistCommerceChangeFiltered[i][j] = (m_ppaiSpecialistCommerceChange && m_ppaiSpecialistCommerceChange[i]) ? m_ppaiSpecialistCommerceChange[i][j] : 0;
			}
			return (m_ppaiSpecialistCommerceChangeFiltered && m_ppaiSpecialistCommerceChangeFiltered[i]) ? m_ppaiSpecialistCommerceChangeFiltered[i][j] : 0;
		}
	}
	return (m_ppaiSpecialistCommerceChange && m_ppaiSpecialistCommerceChange[i]) ? m_ppaiSpecialistCommerceChange[i][j] : 0;
}

int* CvTraitInfo::getSpecialistCommerceChangeArray(int i) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		for (int j = 0; j < NUM_COMMERCE_TYPES; j++)
		{
			if (m_ppaiSpecialistCommerceChange && m_ppaiSpecialistCommerceChange[i])
			{
				if (isNegativeTrait() && m_ppaiSpecialistCommerceChange[i][j] > 0)
				{
					m_ppaiSpecialistCommerceChangeFiltered[i][j] = 0;
				}
				else if (!isNegativeTrait() && m_ppaiSpecialistCommerceChange[i][j] < 0)
				{
					m_ppaiSpecialistCommerceChangeFiltered[i][j] = 0;
				}
				else
				{
					m_ppaiSpecialistCommerceChangeFiltered[i][j] = (m_ppaiSpecialistCommerceChange && m_ppaiSpecialistCommerceChange[i]) ? m_ppaiSpecialistCommerceChange[i][j] : 0;
				}
			}
		}
		return m_ppaiSpecialistCommerceChangeFiltered[i];
	}
	return m_ppaiSpecialistCommerceChange[i];
}

int CvTraitInfo::getLessYieldThreshold(int i) const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (m_paiLessYieldThreshold && m_paiLessYieldThresholdFiltered)
		{
			// AIAndy: There is something wrong with this logic, both branches are identical
			// TB: Thanks for pointing that out there!
			if (!isNegativeTrait() && m_paiLessYieldThreshold[i] > 0)
			{
				m_paiLessYieldThresholdFiltered[i] = -1;
			}
			else
			{
				m_paiLessYieldThresholdFiltered[i] = m_paiLessYieldThreshold ? m_paiLessYieldThreshold[i] : 0;
			}
			return m_paiLessYieldThresholdFiltered ? m_paiLessYieldThresholdFiltered[i] : 0;
		}
	}
	return m_paiLessYieldThreshold ? m_paiLessYieldThreshold[i] : 0;
}

int CvTraitInfo::getSeaPlotYieldChanges(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (m_piSeaPlotYieldChanges && m_piSeaPlotYieldChangesFiltered)
		{
			if (isNegativeTrait() && m_piSeaPlotYieldChanges[i] > 0)
			{
				m_piSeaPlotYieldChangesFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_piSeaPlotYieldChanges[i] < 0)
			{
				m_piSeaPlotYieldChangesFiltered[i] = 0;
			}
			else
			{
				m_piSeaPlotYieldChangesFiltered[i] = m_piSeaPlotYieldChanges ? m_piSeaPlotYieldChanges[i] : 0;
			}
			return m_piSeaPlotYieldChangesFiltered ? m_piSeaPlotYieldChangesFiltered[i] : 0;
		}
	}
	return m_piSeaPlotYieldChanges ? m_piSeaPlotYieldChanges[i] : 0;
}

int* CvTraitInfo::getSeaPlotYieldChangesArray() const
{
	PROFILE_EXTRA_FUNC();
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			if (m_piSeaPlotYieldChanges && m_piSeaPlotYieldChanges[i])
			{
				if (isNegativeTrait() && m_piSeaPlotYieldChanges[i] > 0)
				{
					m_piSeaPlotYieldChangesFiltered[i] = 0;
				}
				else if (!isNegativeTrait() && m_piSeaPlotYieldChanges[i] < 0)
				{
					m_piSeaPlotYieldChangesFiltered[i] = 0;
				}
				else
				{
					m_piSeaPlotYieldChangesFiltered[i] = m_piSeaPlotYieldChanges ? m_piSeaPlotYieldChanges[i] : 0;
				}
			}
		}
		return m_piSeaPlotYieldChangesFiltered;
	}
	return m_piSeaPlotYieldChanges;
}

int CvTraitInfo::getImprovementYieldChange(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), i);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, j);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (m_ppaiImprovementYieldChange && m_ppaiImprovementYieldChange[i] && m_ppaiImprovementYieldChangeFiltered && m_ppaiImprovementYieldChangeFiltered[i])
		{
			if (isNegativeTrait() && m_ppaiImprovementYieldChange[i][j] > 0)
			{
				m_ppaiImprovementYieldChangeFiltered[i][j] = 0;
			}
			else if (!isNegativeTrait() && m_ppaiImprovementYieldChange[i][j] < 0)
			{
				m_ppaiImprovementYieldChangeFiltered[i][j] = 0;
			}
			else
			{
				m_ppaiImprovementYieldChangeFiltered[i][j] = (m_ppaiImprovementYieldChange && m_ppaiImprovementYieldChange[i]) ? m_ppaiImprovementYieldChange[i][j] : 0;
			}
			return (m_ppaiImprovementYieldChangeFiltered && m_ppaiImprovementYieldChangeFiltered[i]) ? m_ppaiImprovementYieldChangeFiltered[i][j] : 0;
		}
	}
	return (m_ppaiImprovementYieldChange && m_ppaiImprovementYieldChange[i]) ? m_ppaiImprovementYieldChange[i][j] : 0;
}

int* CvTraitInfo::getImprovementYieldChangeArray(int i) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), i);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		for (int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if (m_ppaiImprovementYieldChange && m_ppaiImprovementYieldChange[i] && m_ppaiImprovementYieldChangeFiltered)
			{
				if (isNegativeTrait() && m_ppaiImprovementYieldChange[i][j] > 0)
				{
					m_ppaiImprovementYieldChangeFiltered[i][j] = 0;
				}
				else if (!isNegativeTrait() && m_ppaiImprovementYieldChange[i][j] < 0)
				{
					m_ppaiImprovementYieldChangeFiltered[i][j] = 0;
				}
				else
				{
					m_ppaiImprovementYieldChangeFiltered[i][j] = (m_ppaiImprovementYieldChange && m_ppaiImprovementYieldChange[i]) ? m_ppaiImprovementYieldChange[i][j] : 0;
				}
			}
		}
		return (m_ppaiImprovementYieldChangeFiltered) ? m_ppaiImprovementYieldChangeFiltered[i] : 0;
	}
	return (m_ppaiImprovementYieldChange) ? m_ppaiImprovementYieldChange[i] : 0;
}

int CvTraitInfo::getGoldenAgeYieldChanges(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (m_piGoldenAgeYieldChanges && m_piGoldenAgeYieldChangesFiltered)
		{
			if (isNegativeTrait() && m_piGoldenAgeYieldChanges[i] > 0)
			{
				m_piGoldenAgeYieldChangesFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_piGoldenAgeYieldChanges[i] < 0)
			{
				m_piGoldenAgeYieldChangesFiltered[i] = 0;
			}
			else
			{
				m_piGoldenAgeYieldChangesFiltered[i] = m_piGoldenAgeYieldChanges ? m_piGoldenAgeYieldChanges[i] : 0;
			}
			return m_piGoldenAgeYieldChangesFiltered ? m_piGoldenAgeYieldChangesFiltered[i] : 0;
		}
	}
	return m_piGoldenAgeYieldChanges ? m_piGoldenAgeYieldChanges[i] : 0;
}

int* CvTraitInfo::getGoldenAgeYieldChangesArray() const
{
	PROFILE_EXTRA_FUNC();
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			if (m_piGoldenAgeYieldChanges && m_piGoldenAgeYieldChanges[i])
			{
				if (isNegativeTrait() && m_piGoldenAgeYieldChanges[i] > 0)
				{
					m_piGoldenAgeYieldChangesFiltered[i] = 0;
				}
				else if (!isNegativeTrait() && m_piGoldenAgeYieldChanges[i] < 0)
				{
					m_piGoldenAgeYieldChangesFiltered[i] = 0;
				}
				else
				{
					m_piGoldenAgeYieldChangesFiltered[i] = m_piGoldenAgeYieldChanges ? m_piGoldenAgeYieldChanges[i] : 0;
				}
			}
		}
		return m_piGoldenAgeYieldChangesFiltered;
	}
	return m_piGoldenAgeYieldChanges;
}

int CvTraitInfo::getGoldenAgeCommerceChanges(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		if (m_piGoldenAgeCommerceChanges && m_piGoldenAgeCommerceChangesFiltered)
		{
			if (isNegativeTrait() && m_piGoldenAgeCommerceChanges[i] > 0)
			{
				m_piGoldenAgeCommerceChangesFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_piGoldenAgeCommerceChanges[i] < 0)
			{
				m_piGoldenAgeCommerceChangesFiltered[i] = 0;
			}
			else
			{
				m_piGoldenAgeCommerceChangesFiltered[i] = m_piGoldenAgeCommerceChanges ? m_piGoldenAgeCommerceChanges[i] : 0;
			}
			return m_piGoldenAgeCommerceChangesFiltered ? m_piGoldenAgeCommerceChangesFiltered[i] : 0;
		}
	}
	return m_piGoldenAgeCommerceChanges ? m_piGoldenAgeCommerceChanges[i] : 0;
}

int* CvTraitInfo::getGoldenAgeCommerceChangesArray() const
{
	PROFILE_EXTRA_FUNC();
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		for (int i = 0; i < NUM_COMMERCE_TYPES; i++)
		{
			if (m_piGoldenAgeCommerceChanges && m_piGoldenAgeCommerceChanges[i])
			{
				if (isNegativeTrait() && m_piGoldenAgeCommerceChanges[i] > 0)
				{
					m_piGoldenAgeCommerceChangesFiltered[i] = 0;
				}
				else if (!isNegativeTrait() && m_piGoldenAgeCommerceChanges[i] < 0)
				{
					m_piGoldenAgeCommerceChangesFiltered[i] = 0;
				}
				else
				{
					m_piGoldenAgeCommerceChangesFiltered[i] = m_piGoldenAgeCommerceChanges ? m_piGoldenAgeCommerceChanges[i] : 0;
				}
			}
		}
		return m_piGoldenAgeCommerceChangesFiltered;
	}
	return m_piGoldenAgeCommerceChanges;
}

namespace PureTraits
{
	namespace
	{
		template <typename T1, typename T2>
		bool anyValue(const std::pair<T1, T2>&)
		{
			return true;
		}
		template <typename T1, typename T2>
		bool isPositiveValue(const std::pair<T1, T2>& pair)
		{
			return pair.second > 0;
		}
		template <typename T1, typename T2>
		bool isNegativeValue(const std::pair<T1, T2>& pair)
		{
			return pair.second < 0;
		}
	}

	template <typename T1, typename T2>
	bst::function<bool(const std::pair<T1, T2>&)> getPredicate(bool bNegativeTrait)
	{
		if (!GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
			return bind(anyValue<T1, T2>, _1);

		if (bNegativeTrait)
			return bind(isNegativeValue<T1, T2>, _1);
		else
			return bind(isPositiveValue<T1, T2>, _1);
	}
};

const IDValueMap<ImprovementTypes, int>::filtered CvTraitInfo::getImprovementUpgradeModifiers() const
{
	return filter(m_aImprovementUpgradeModifierTypes, PureTraits::getPredicate<ImprovementTypes, int>(m_bNegativeTrait));
}

const IDValueMap<BuildTypes, int>::filtered CvTraitInfo::getBuildWorkerSpeedModifiers() const
{
	return filter(m_aBuildWorkerSpeedModifierTypes, PureTraits::getPredicate<BuildTypes, int>(m_bNegativeTrait));
}

int CvTraitInfo::getNumDisallowedTraitTypes() const
{
	return (int)m_aDisallowedTraitTypes.size();
}

DisallowedTraitType CvTraitInfo::isDisallowedTraitType(int iTrait) const
{
	FASSERT_BOUNDS(0, getNumDisallowedTraitTypes(), iTrait);

	return m_aDisallowedTraitTypes[iTrait];
}

const IDValueMap<DomainTypes, int>::filtered CvTraitInfo::getDomainFreeExperience() const
{
	return filter(m_aDomainFreeExperiences, PureTraits::getPredicate<DomainTypes, int>(m_bNegativeTrait));
}

const IDValueMap<DomainTypes, int>::filtered CvTraitInfo::getDomainProductionModifiers() const
{
	return filter(m_aDomainProductionModifiers, PureTraits::getPredicate<DomainTypes, int>(m_bNegativeTrait));
}

int CvTraitInfo::getNumBuildingProductionModifiers() const
{
	return (int)m_aBuildingProductionModifiers.size();
}

BuildingModifier CvTraitInfo::getBuildingProductionModifier(int iBuilding) const
{
	FASSERT_BOUNDS(0, getNumBuildingProductionModifiers(), iBuilding);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		BuildingModifier kMod = m_aBuildingProductionModifiers[iBuilding];
		if (isNegativeTrait() && kMod.iModifier > 0)
		{
			kMod.iModifier = 0;
		}
		else if (!isNegativeTrait() && kMod.iModifier < 0)
		{
			kMod.iModifier = 0;
		}
		return kMod;
	}
	return m_aBuildingProductionModifiers[iBuilding];
}

const IDValueMap<TechTypes, int>::filtered CvTraitInfo::getTechResearchModifiers() const
{
	return filter(m_aTechResearchModifiers, PureTraits::getPredicate<TechTypes, int>(m_bNegativeTrait));
}

const IDValueMap<SpecialBuildingTypes, int>::filtered CvTraitInfo::getSpecialBuildingProductionModifiers() const
{
	return filter(m_aSpecialBuildingProductionModifiers, PureTraits::getPredicate<SpecialBuildingTypes, int>(m_bNegativeTrait));
}

const IDValueMap<BuildingTypes, int>::filtered CvTraitInfo::getBuildingHappinessModifiersFiltered() const
{
	return filter(m_aBuildingHappinessModifiers, PureTraits::getPredicate<BuildingTypes, int>(m_bNegativeTrait));
}

int CvTraitInfo::getNumUnitProductionModifiers() const
{
	return (int)m_aUnitProductionModifiers.size();
}

UnitModifier CvTraitInfo::getUnitProductionModifier(int iUnit) const
{
	FASSERT_BOUNDS(0, getNumUnitProductionModifiers(), iUnit);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		UnitModifier kMod = m_aUnitProductionModifiers[iUnit];
		if (isNegativeTrait() && kMod.iModifier > 0)
		{
			kMod.iModifier = 0;
		}
		else if (!isNegativeTrait() && kMod.iModifier < 0)
		{
			kMod.iModifier = 0;
		}
		return kMod;
	}
	return m_aUnitProductionModifiers[iUnit];
}

int CvTraitInfo::getNumSpecialUnitProductionModifiers() const
{
	return (int)m_aSpecialUnitProductionModifiers.size();
}

SpecialUnitModifier CvTraitInfo::getSpecialUnitProductionModifier(int iSpecialUnit) const
{
	FASSERT_BOUNDS(0, getNumSpecialUnitProductionModifiers(), iSpecialUnit);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		SpecialUnitModifier kMod = m_aSpecialUnitProductionModifiers[iSpecialUnit];
		if (isNegativeTrait() && kMod.iModifier > 0)
		{
			kMod.iModifier = 0;
		}
		else if (!isNegativeTrait() && kMod.iModifier < 0)
		{
			kMod.iModifier = 0;
		}
		return kMod;
	}
	return m_aSpecialUnitProductionModifiers[iSpecialUnit];
}

int CvTraitInfo::getNumCivicOptionNoUpkeepTypes() const
{
	return (int)m_aCivicOptionNoUpkeepTypes.size();
}

CivicOptionTypeBool CvTraitInfo::isCivicOptionNoUpkeepType(int iCivicOption) const
{
	FASSERT_BOUNDS(0, getNumCivicOptionNoUpkeepTypes(), iCivicOption);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		CivicOptionTypeBool kMod = m_aCivicOptionNoUpkeepTypes[iCivicOption];
		if (isNegativeTrait() && kMod.bBool)
		{
			kMod.bBool = 0;
		}
		else if (!isNegativeTrait() && !kMod.bBool)
		{
			kMod.bBool = 0;
		}
		return kMod;
	}
	return m_aCivicOptionNoUpkeepTypes[iCivicOption];
}

//Team Project (8)
int CvTraitInfo::getNumUnitCombatFreeExperiences() const
{
	return (int)m_aUnitCombatFreeExperiences.size();
}

UnitCombatModifier CvTraitInfo::getUnitCombatFreeExperience(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, getNumUnitCombatFreeExperiences(), iUnitCombat);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		UnitCombatModifier kMod = m_aUnitCombatFreeExperiences[iUnitCombat];
		if (isNegativeTrait() && kMod.iModifier > 0)
		{
			kMod.iModifier = 0;
		}
		else if (!isNegativeTrait() && kMod.iModifier < 0)
		{
			kMod.iModifier = 0;
		}
		return kMod;
	}
	return m_aUnitCombatFreeExperiences[iUnitCombat];
}

int CvTraitInfo::getNumUnitCombatProductionModifiers() const
{
	return (int)m_aUnitCombatProductionModifiers.size();
}

UnitCombatModifier CvTraitInfo::getUnitCombatProductionModifier(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, getNumUnitCombatProductionModifiers(), iUnitCombat);

	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS))
	{
		UnitCombatModifier kMod = m_aUnitCombatProductionModifiers[iUnitCombat];
		if (isNegativeTrait() && kMod.iModifier > 0)
		{
			kMod.iModifier = 0;
		}
		else if (!isNegativeTrait() && kMod.iModifier < 0)
		{
			kMod.iModifier = 0;
		}
		return kMod;
	}
	return m_aUnitCombatProductionModifiers[iUnitCombat];
}

const IDValueMap<BonusTypes, int>::filtered CvTraitInfo::getBonusHappinessChanges() const
{
	return filter(m_aBonusHappinessChanges, PureTraits::getPredicate<BonusTypes, int>(m_bNegativeTrait));
}

void CvTraitInfo::getDataMembers(CvInfoUtil& util)
{
	util
		.add(m_aSpecialBuildingProductionModifiers, L"SpecialBuildingProductionModifierTypes")
		.add(m_aBuildWorkerSpeedModifierTypes, L"BuildWorkerSpeedModifierTypes")
		.add(m_aBuildingHappinessModifiers, L"BuildingHappinessModifierTypes")
		.add(m_aTechResearchModifiers, L"TechResearchModifiers")
		.add(m_aBonusHappinessChanges, L"BonusHappinessChanges")
		.add(m_aImprovementUpgradeModifierTypes, L"ImprovementUpgradeModifierTypes")
		.add(m_aDomainFreeExperiences, L"DomainFreeExperiences")
		.add(m_aDomainProductionModifiers, L"DomainProductionModifiers")
	;
}

bool CvTraitInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	CvString szTextVal2;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ShortDescription");
	setShortDescription(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iHealth, L"iHealth");
	pXML->GetOptionalChildXmlValByName(&m_iHappiness, L"iHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iMaxAnarchy, L"iMaxAnarchy",-1);
	pXML->GetOptionalChildXmlValByName(&m_iUpkeepModifier, L"iUpkeepModifier");
	pXML->GetOptionalChildXmlValByName(&m_iLevelExperienceModifier, L"iLevelExperienceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGreatPeopleRateModifier, L"iGreatPeopleRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGreatGeneralRateModifier, L"iGreatGeneralRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iDomesticGreatGeneralRateModifier, L"iDomesticGreatGeneralRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iMaxGlobalBuildingProductionModifier, L"iMaxGlobalBuildingProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iMaxTeamBuildingProductionModifier, L"iMaxTeamBuildingProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iMaxPlayerBuildingProductionModifier, L"iMaxPlayerBuildingProductionModifier");

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		RevTrait Effects														*/
/********************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxLocal, L"iRevIdxLocal");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxNational, L"iRevIdxNational");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxDistanceModifier, L"iRevIdxDistanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxHolyCityGood, L"iRevIdxHolyCityGood");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxHolyCityBad, L"iRevIdxHolyCityBad");
	pXML->GetOptionalChildXmlValByName(&m_fRevIdxNationalityMod, L"fRevIdxNationalityMod");
	pXML->GetOptionalChildXmlValByName(&m_fRevIdxBadReligionMod, L"fRevIdxBadReligionMod");
	pXML->GetOptionalChildXmlValByName(&m_fRevIdxGoodReligionMod, L"fRevIdxGoodReligionMod");
	pXML->GetOptionalChildXmlValByName(&m_bNonStateReligionCommerce, L"bNonStateReligionCommerce");
	pXML->GetOptionalChildXmlValByName(&m_bUpgradeAnywhere, L"bUpgradeAnywhere");
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

	if (pXML->TryMoveToXmlFirstChild(L"ExtraYieldThresholds"))
	{
		pXML->SetYields(&m_paiExtraYieldThreshold);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiExtraYieldThreshold);
	}

	if (pXML->TryMoveToXmlFirstChild(L"TradeYieldModifiers"))
	{
		pXML->SetYields(&m_paiTradeYieldModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiTradeYieldModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CommerceChanges"))
	{
		pXML->SetCommerce(&m_paiCommerceChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiCommerceChange);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
	{
		pXML->SetCommerce(&m_paiCommerceModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiCommerceModifier);
	}

	FAssertMsg((GC.getNumPromotionInfos() > 0) && (GC.getNumUnitCombatInfos()) > 0,"either the number of promotion infos is zero or less or the number of unit combat types is zero or less");
	if (pXML->TryMoveToXmlFirstChild(L"FreePromotionUnitCombatTypes"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				CvXMLLoadUtility::InitPointerList(&m_ppbFreePromotionUnitCombats, GC.getNumPromotionInfos());
				for (int j=0;j<iNumSibs;j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"PromotionType");
					int iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppbFreePromotionUnitCombats[iIndex]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"UnitCombatTypes"))
						{
							// get the total number of children the current xml node has
							int iNumChildren = pXML->GetXmlChildrenNumber();

							if (0 < iNumChildren)
							{
								CvXMLLoadUtility::InitList(&m_ppbFreePromotionUnitCombats[iIndex], GC.getNumUnitCombatInfos());

								// if the call to the function that sets the current xml node to it's first non-comment
								// child and sets the parameter with the new node's value succeeds
								CvString szTag;
								if (pXML->GetChildXmlVal(szTag))
								{
									int iIndex2 = pXML->GetInfoClass(szTag);
									if (iIndex2 > -1)
									{
										m_ppbFreePromotionUnitCombats[iIndex][iIndex2] = true;
									}
									if(!(iNumChildren <= GC.getNumUnitCombatInfos()))
									{
										char	szMessage[1024];
										sprintf( szMessage, "For loop iterator is greater than array size \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
										gDLL->MessageBox(szMessage, "XML Error");
									}
									// loop through all the siblings, we start at 1 since we already have the first value
									for (int i=1;i<iNumChildren;i++)
									{
										// if the call to the function that sets the current xml node to it's first non-comment
										// sibling and sets the parameter with the new node's value does not succeed
										// we will break out of this for loop

										if (!pXML->GetNextXmlVal(szTag))
										{
											break;
										}
										iIndex2 = pXML->GetInfoClass(szTag);
										if (iIndex2 > -1)
										{
											m_ppbFreePromotionUnitCombats[iIndex][iIndex2] = true;
										}

									}
									// set the current xml node to it's parent node
									pXML->MoveToXmlParent();
								}
							}
							else
							{
								SAFE_DELETE_ARRAY(m_ppbFreePromotionUnitCombats[iIndex]);
							}

							pXML->MoveToXmlParent();
						}
						else
						{
							SAFE_DELETE_ARRAY(m_ppbFreePromotionUnitCombats[iIndex]);
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
	m_PropertyManipulators.read(pXML);

	//TB Traits Mods begin
	//Textual References
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PromotionLine");
	m_ePromotionLine = (PromotionLineTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"GreatPeopleUnitType");
	m_iGreatPeopleUnitType = (UnitTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"GoldenAgeonBirthofGreatPersonType");
	m_iGoldenAgeonBirthofGreatPeopleType = (UnitTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqTech");
	m_ePrereqTech = (TechTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"EraAdvanceFreeSpecialistType");
	m_eEraAdvanceFreeSpecialistType = (SpecialistTypes) pXML->GetInfoClass(szTextVal);

	//integers
	pXML->GetOptionalChildXmlValByName(&m_iWarWearinessAccumulationModifier, L"iWarWearinessAccumulationModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCivicAnarchyTimeModifier, L"iCivicAnarchyTimeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iReligiousAnarchyTimeModifier, L"iReligiousAnarchyTimeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iImprovementUpgradeRateModifier, L"iImprovementUpgradeRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iWorkerSpeedModifier, L"iWorkerSpeedModifier");
	pXML->GetOptionalChildXmlValByName(&m_iMaxConscript, L"iMaxConscript");
	pXML->GetOptionalChildXmlValByName(&m_iDistanceMaintenanceModifier, L"iDistanceMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNumCitiesMaintenanceModifier, L"iNumCitiesMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCorporationMaintenanceModifier, L"iCorporationMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionGreatPeopleRateModifier, L"iStateReligionGreatPeopleRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iFreeExperience, L"iFreeExperience");
	pXML->GetOptionalChildXmlValByName(&m_iFreeUnitUpkeepCivilian, L"iFreeUnitUpkeepCivilian");
	pXML->GetOptionalChildXmlValByName(&m_iFreeUnitUpkeepMilitary, L"iFreeUnitUpkeepMilitary");
	pXML->GetOptionalChildXmlValByName(&m_iFreeUnitUpkeepCivilianPopPercent, L"iFreeUnitUpkeepCivilianPopPercent");
	pXML->GetOptionalChildXmlValByName(&m_iFreeUnitUpkeepMilitaryPopPercent, L"iFreeUnitUpkeepMilitaryPopPercent");
	pXML->GetOptionalChildXmlValByName(&m_iCivilianUnitUpkeepMod, L"iCivilianUnitUpkeepMod");
	pXML->GetOptionalChildXmlValByName(&m_iMilitaryUnitUpkeepMod, L"iMilitaryUnitUpkeepMod");
	pXML->GetOptionalChildXmlValByName(&m_iHappyPerMilitaryUnit, L"iHappyPerMilitaryUnit");
	pXML->GetOptionalChildXmlValByName(&m_iLargestCityHappiness, L"iLargestCityHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iFreeSpecialist, L"iFreeSpecialist");
	pXML->GetOptionalChildXmlValByName(&m_iTradeRoutes, L"iTradeRoutes");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionHappiness, L"iStateReligionHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iNonStateReligionHappiness, L"iNonStateReligionHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionUnitProductionModifier, L"iStateReligionUnitProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionBuildingProductionModifier, L"iStateReligionBuildingProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionFreeExperience, L"iStateReligionFreeExperience");
	pXML->GetOptionalChildXmlValByName(&m_iExpInBorderModifier, L"iExpInBorderModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCityDefenseBonus, L"iCityDefenseBonus");
	pXML->GetOptionalChildXmlValByName(&m_iMilitaryProductionModifier, L"iMilitaryProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iAttitudeModifier, L"iAttitudeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iLinePriority, L"iLinePriority");
	pXML->GetOptionalChildXmlValByName(&m_iEspionageDefense, L"iEspionageDefense");
	pXML->GetOptionalChildXmlValByName(&m_iMinAnarchy, L"iMinAnarchy");
	pXML->GetOptionalChildXmlValByName(&m_iMaxTradeRoutesChange, L"iMaxTradeRoutesChange");
	pXML->GetOptionalChildXmlValByName(&m_iGoldenAgeDurationModifier, L"iGoldenAgeDurationModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGreatPeopleRateChange, L"iGreatPeopleRateChange");
	pXML->GetOptionalChildXmlValByName(&m_iHurryAngerModifier, L"iHurryAngerModifier");
	pXML->GetOptionalChildXmlValByName(&m_iHurryCostModifier, L"iHurryCostModifier");
	pXML->GetOptionalChildXmlValByName(&m_iEnemyWarWearinessModifier, L"iEnemyWarWearinessModifier");
	pXML->GetOptionalChildXmlValByName(&m_iForeignTradeRouteModifier, L"iForeignTradeRouteModifier");
	pXML->GetOptionalChildXmlValByName(&m_iBombardDefense, L"iBombardDefense");
	pXML->GetOptionalChildXmlValByName(&m_iUnitUpgradePriceModifier, L"iUnitUpgradePriceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCoastalTradeRoutes, L"iCoastalTradeRoutes");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalPopulationgrowthratepercentage, L"iGlobalPopulationgrowthratepercentage");
	pXML->GetOptionalChildXmlValByName(&m_iCityStartCulture, L"iCityStartCulture");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalAirUnitCapacity, L"iGlobalAirUnitCapacity");
	pXML->GetOptionalChildXmlValByName(&m_iCapitalXPModifier, L"iCapitalXPModifier");
	pXML->GetOptionalChildXmlValByName(&m_iHolyCityofStateReligionXPModifier, L"iHolyCityofStateReligionXPModifier");
	pXML->GetOptionalChildXmlValByName(&m_iHolyCityofNonStateReligionXPModifier, L"iHolyCityofNonStateReligionXPModifier");
	pXML->GetOptionalChildXmlValByName(&m_iBonusPopulationinNewCities, L"iBonusPopulationinNewCities");
	pXML->GetOptionalChildXmlValByName(&m_iMissileRange, L"iMissileRange");
	pXML->GetOptionalChildXmlValByName(&m_iFlightOperationRange, L"iFlightOperationRange");
	pXML->GetOptionalChildXmlValByName(&m_iNavalCargoSpace, L"iNavalCargoSpace");
	pXML->GetOptionalChildXmlValByName(&m_iMissileCargoSpace, L"iMissileCargoSpace");
	pXML->GetOptionalChildXmlValByName(&m_iNationalCaptureProbabilityModifier, L"iNationalCaptureProbabilityModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNationalCaptureResistanceModifier, L"iNationalCaptureResistanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionSpreadProbabilityModifier, L"iStateReligionSpreadProbabilityModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNonStateReligionSpreadProbabilityModifier, L"iNonStateReligionSpreadProbabilityModifier");
	pXML->GetOptionalChildXmlValByName(&m_iFreedomFighterChange, L"iFreedomFighterChange");
	//booleans
	pXML->GetOptionalChildXmlValByName(&m_bMilitaryFoodProduction, L"bMilitaryFoodProduction");
	pXML->GetOptionalChildXmlValByName(&m_bNegativeTrait, L"bNegativeTrait");
	pXML->GetOptionalChildXmlValByName(&m_bImpurePropertyManipulators, L"bImpurePropertyManipulators");
	pXML->GetOptionalChildXmlValByName(&m_bImpurePromotions, L"bImpurePromotions");
	pXML->GetOptionalChildXmlValByName(&m_bCivilizationTrait, L"bCivilizationTrait");
	pXML->GetOptionalChildXmlValByName(&m_bAllowsInquisitions, L"bAllowsInquisitions");
	pXML->GetOptionalChildXmlValByName(&m_bCoastalAIInfluence, L"bCoastalAIInfluence");
	pXML->GetOptionalChildXmlValByName(&m_bBarbarianSelectionOnly, L"bBarbarianSelectionOnly");
	pXML->GetOptionalChildXmlValByName(&m_bCitiesStartwithStateReligion, L"bCitiesStartwithStateReligion");
	pXML->GetOptionalChildXmlValByName(&m_bDraftsOnCityCapture, L"bDraftsOnCityCapture");
	pXML->GetOptionalChildXmlValByName(&m_bFreeSpecialistperWorldWonder, L"bFreeSpecialistperWorldWonder");
	pXML->GetOptionalChildXmlValByName(&m_bFreeSpecialistperNationalWonder, L"bFreeSpecialistperNationalWonder");
	pXML->GetOptionalChildXmlValByName(&m_bFreeSpecialistperTeamProject, L"bFreeSpecialistperTeamProject");
	pXML->GetOptionalChildXmlValByName(&m_bExtraGoody, L"bExtraGoody");
	pXML->GetOptionalChildXmlValByName(&m_bAllReligionsActive, L"bAllReligionsActive");
	pXML->GetOptionalChildXmlValByName(&m_bBansNonStateReligions, L"bBansNonStateReligions");
	pXML->GetOptionalChildXmlValByName(&m_bFreedomFighter, L"bFreedomFighter");

	// bool vector without delayed resolution
	pXML->SetOptionalVector(&m_aiOnGameOptions, L"OnGameOptions");
	pXML->SetOptionalVector(&m_aiNotOnGameOptions, L"NotOnGameOptions");
	pXML->SetOptionalVector(&m_aiCategories, L"Categories");

	//arrays
	int j;
	int k;
	int iNumChildren;
	if (pXML->TryMoveToXmlFirstChild(L"SpecialistYieldChanges"))
	{
		iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"SpecialistYieldChange"))
		{
			for(j=0;j<iNumChildren;j++)
			{
				pXML->GetChildXmlValByName(szTextVal, L"SpecialistType");
				k = pXML->GetInfoClass(szTextVal);
				if (k > -1)
				{
					if ( m_ppaiSpecialistYieldChange == NULL )
					{
						m_ppaiSpecialistYieldChange = new int*[GC.getNumSpecialistInfos()];

						for(int i = 0; i < GC.getNumSpecialistInfos(); i++)
						{
							m_ppaiSpecialistYieldChange[i] = NULL;
						}
					}
					if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
					{
						// call the function that sets the yield change variable
						pXML->SetYields(&m_ppaiSpecialistYieldChange[k]);
						pXML->MoveToXmlParent();
					}
				}

				if (!pXML->TryMoveToXmlNextSibling())
				{
					break;
				}
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"YieldModifiers"))
	{
		pXML->SetYields(&m_piYieldModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CapitalYieldModifiers"))
	{
		pXML->SetYields(&m_piCapitalYieldModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCapitalYieldModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CapitalCommerceModifiers"))
	{
		pXML->SetCommerce(&m_piCapitalCommerceModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCapitalCommerceModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistExtraCommerces"))
	{
		pXML->SetCommerce(&m_piSpecialistExtraCommerce);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piSpecialistExtraCommerce);
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistExtraYields"))
	{
		pXML->SetYields(&m_piSpecialistExtraYield);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piSpecialistExtraYield);
	}

	if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
	{
		pXML->SetYields(&m_piYieldChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldChange);
	}

	if (pXML->TryMoveToXmlFirstChild(L"LessYieldThresholds"))
	{
		pXML->SetYields(&m_paiLessYieldThreshold);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiLessYieldThreshold);
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistCommerceChanges"))
	{
		iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"SpecialistCommerceChange"))
		{
			for(j=0;j<iNumChildren;j++)
			{
				pXML->GetChildXmlValByName(szTextVal, L"SpecialistType");
				k = pXML->GetInfoClass(szTextVal);
				if (k > -1)
				{
					if ( m_ppaiSpecialistCommerceChange == NULL )
					{
						m_ppaiSpecialistCommerceChange = new int*[GC.getNumSpecialistInfos()];

						for(int i = 0; i < GC.getNumSpecialistInfos(); i++)
						{
							m_ppaiSpecialistCommerceChange[i] = NULL;
						}
					}
					if (pXML->TryMoveToXmlFirstChild(L"CommerceChanges"))
					{
						// call the function that sets the yield change variable
						pXML->SetCommerce(&m_ppaiSpecialistCommerceChange[k]);
						pXML->MoveToXmlParent();
					}
				}

				if (!pXML->TryMoveToXmlNextSibling())
				{
					break;
				}
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}
	pXML->SetVariableListTagPair(&m_piFlavorValue, L"Flavors", GC.getNumFlavorTypes());

	if (pXML->TryMoveToXmlFirstChild(L"SeaPlotYieldChanges"))
	{
		pXML->SetYields(&m_piSeaPlotYieldChanges);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piSeaPlotYieldChanges);
	}

	if (pXML->TryMoveToXmlFirstChild(L"ImprovementYieldChanges"))
	{
		iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"ImprovementYieldChange"))
		{
			for(j=0;j<iNumChildren;j++)
			{
				pXML->GetChildXmlValByName(szTextVal, L"ImprovementType");
				k = pXML->GetInfoClass(szTextVal);
				if (k > -1)
				{
					if ( m_ppaiImprovementYieldChange == NULL )
					{
						m_ppaiImprovementYieldChange = new int*[GC.getNumImprovementInfos()];

						for(int i = 0; i < GC.getNumImprovementInfos(); i++)
						{
							m_ppaiImprovementYieldChange[i] = NULL;
						}
					}
					if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
					{
						// call the function that sets the yield change variable
						pXML->SetYields(&m_ppaiImprovementYieldChange[k]);
						pXML->MoveToXmlParent();
					}
				}

				if (!pXML->TryMoveToXmlNextSibling())
				{
					break;
				}
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"GoldenAgeYieldChanges"))
	{
		pXML->SetYields(&m_piGoldenAgeYieldChanges);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piGoldenAgeYieldChanges);
	}

	if (pXML->TryMoveToXmlFirstChild(L"GoldenAgeCommerceChanges"))
	{
		pXML->SetCommerce(&m_piGoldenAgeCommerceChanges);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piGoldenAgeCommerceChanges);
	}

	if(pXML->TryMoveToXmlFirstChild(L"DisallowedTraitTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"DisallowedTraitType" );
		m_aDisallowedTraitTypes.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"DisallowedTraitType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"TraitType");
					GC.addDelayedResolution((int*)&(m_aDisallowedTraitTypes[i].eTrait), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"DisallowedTraitType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"BuildingProductionModifierTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"BuildingProductionModifierType" );
		m_aBuildingProductionModifiers.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"BuildingProductionModifierType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"BuildingType");
					pXML->GetChildXmlValByName(&(m_aBuildingProductionModifiers[i].iModifier), L"iBuildingProductionModifier");
					GC.addDelayedResolution((int*)&(m_aBuildingProductionModifiers[i].eBuilding), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"BuildingProductionModifierType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"UnitProductionModifierTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"UnitProductionModifierType" );
		m_aUnitProductionModifiers.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"UnitProductionModifierType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitType");
					pXML->GetChildXmlValByName(&(m_aUnitProductionModifiers[i].iModifier), L"iUnitProductionModifier");
					GC.addDelayedResolution((int*)&(m_aUnitProductionModifiers[i].eUnit), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"UnitProductionModifierType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"SpecialUnitProductionModifierTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"SpecialUnitProductionModifierType" );
		m_aSpecialUnitProductionModifiers.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"SpecialUnitProductionModifierType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"SpecialUnitType");
					m_aSpecialUnitProductionModifiers[i].eSpecialUnit = (SpecialUnitTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aSpecialUnitProductionModifiers[i].iModifier), L"iSpecialUnitProductionModifier");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"SpecialUnitProductionModifierType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"CivicOptionNoUpkeepTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"CivicOptionNoUpkeepType" );
		m_aCivicOptionNoUpkeepTypes.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"CivicOptionNoUpkeepType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"CivicOptionType");
					pXML->GetChildXmlValByName(&(m_aCivicOptionNoUpkeepTypes[i].bBool), L"bCivicOptionNoUpkeep");
					GC.addDelayedResolution((int*)&(m_aCivicOptionNoUpkeepTypes[i].eCivicOption), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"CivicOptionNoUpkeepType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"UnitCombatFreeExperiences"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"UnitCombatFreeExperience" );
		m_aUnitCombatFreeExperiences.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"UnitCombatFreeExperience"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					m_aUnitCombatFreeExperiences[i].eUnitCombat = (UnitCombatTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aUnitCombatFreeExperiences[i].iModifier), L"iModifier");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"UnitCombatFreeExperience"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"UnitCombatProductionModifiers"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"UnitCombatProductionModifier" );
		m_aUnitCombatProductionModifiers.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"UnitCombatProductionModifier"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					m_aUnitCombatProductionModifiers[i].eUnitCombat = (UnitCombatTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aUnitCombatProductionModifiers[i].iModifier), L"iModifier");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"UnitCombatProductionModifier"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	//For Pure Traits

	if (pXML->TryMoveToXmlFirstChild(L"ExtraYieldThresholds"))
	{
		pXML->SetYields(&m_paiExtraYieldThresholdFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiExtraYieldThresholdFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"TradeYieldModifiers"))
	{
		pXML->SetYields(&m_paiTradeYieldModifierFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiTradeYieldModifierFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CommerceChanges"))
	{
		pXML->SetCommerce(&m_paiCommerceChangeFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiCommerceChangeFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
	{
		pXML->SetCommerce(&m_paiCommerceModifierFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiCommerceModifierFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistYieldChanges"))
	{
		iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"SpecialistYieldChange"))
		{
			for(j=0;j<iNumChildren;j++)
			{
				pXML->GetChildXmlValByName(szTextVal, L"SpecialistType");
				k = pXML->GetInfoClass(szTextVal);
				if (k > -1)
				{
					if ( m_ppaiSpecialistYieldChangeFiltered == NULL )
					{
						m_ppaiSpecialistYieldChangeFiltered = new int*[GC.getNumSpecialistInfos()];

						for(int i = 0; i < GC.getNumSpecialistInfos(); i++)
						{
							m_ppaiSpecialistYieldChangeFiltered[i] = NULL;
						}
					}
					if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
					{
						// call the function that sets the yield change variable
						pXML->SetYields(&m_ppaiSpecialistYieldChangeFiltered[k]);
						pXML->MoveToXmlParent();
					}
				}

				if (!pXML->TryMoveToXmlNextSibling())
				{
					break;
				}
			}

			// set the current xml node to it's parent node
			pXML->MoveToXmlParent();
		}

		// set the current xml node to it's parent node
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"YieldModifiers"))
	{
		pXML->SetYields(&m_piYieldModifierFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldModifierFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CapitalYieldModifiers"))
	{
		pXML->SetYields(&m_piCapitalYieldModifierFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCapitalYieldModifierFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CapitalCommerceModifiers"))
	{
		pXML->SetCommerce(&m_piCapitalCommerceModifierFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCapitalCommerceModifierFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistExtraCommerces"))
	{
		pXML->SetCommerce(&m_piSpecialistExtraCommerceFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piSpecialistExtraCommerceFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistExtraYields"))
	{
		pXML->SetYields(&m_piSpecialistExtraYieldFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piSpecialistExtraYieldFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
	{
		pXML->SetYields(&m_piYieldChangeFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldChangeFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistCommerceChanges"))
	{
		iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"SpecialistCommerceChange"))
		{
			for(j=0;j<iNumChildren;j++)
			{
				pXML->GetChildXmlValByName(szTextVal, L"SpecialistType");
				k = pXML->GetInfoClass(szTextVal);
				if (k > -1)
				{
					if ( m_ppaiSpecialistCommerceChangeFiltered == NULL )
					{
						m_ppaiSpecialistCommerceChangeFiltered = new int*[GC.getNumSpecialistInfos()];

						for(int i = 0; i < GC.getNumSpecialistInfos(); i++)
						{
							m_ppaiSpecialistCommerceChangeFiltered[i] = NULL;
						}
					}
					if (pXML->TryMoveToXmlFirstChild(L"CommerceChanges"))
					{
						// call the function that sets the yield change variable
						pXML->SetCommerce(&m_ppaiSpecialistCommerceChangeFiltered[k]);
						pXML->MoveToXmlParent();
					}
				}

				if (!pXML->TryMoveToXmlNextSibling())
				{
					break;
				}
			}

			// set the current xml node to it's parent node
			pXML->MoveToXmlParent();
		}

		// set the current xml node to it's parent node
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"LessYieldThresholds"))
	{
		pXML->SetYields(&m_paiLessYieldThresholdFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiLessYieldThresholdFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"SeaPlotYieldChanges"))
	{
		pXML->SetYields(&m_piSeaPlotYieldChangesFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piSeaPlotYieldChangesFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"ImprovementYieldChanges"))
	{
		iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"ImprovementYieldChange"))
		{
			for(j=0;j<iNumChildren;j++)
			{
				pXML->GetChildXmlValByName(szTextVal, L"ImprovementType");
				k = pXML->GetInfoClass(szTextVal);
				if (k > -1)
				{
					if ( m_ppaiImprovementYieldChangeFiltered == NULL )
					{
						m_ppaiImprovementYieldChangeFiltered = new int*[GC.getNumImprovementInfos()];

						for(int i = 0; i < GC.getNumImprovementInfos(); i++)
						{
							m_ppaiImprovementYieldChangeFiltered[i] = NULL;
						}
					}
					if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
					{
						// call the function that sets the yield change variable
						pXML->SetYields(&m_ppaiImprovementYieldChangeFiltered[k]);
						pXML->MoveToXmlParent();
					}
				}

				if (!pXML->TryMoveToXmlNextSibling())
				{
					break;
				}
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"GoldenAgeYieldChanges"))
	{
		pXML->SetYields(&m_piGoldenAgeYieldChangesFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piGoldenAgeYieldChangesFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"GoldenAgeCommerceChanges"))
	{
		pXML->SetCommerce(&m_piGoldenAgeCommerceChangesFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piGoldenAgeCommerceChangesFiltered);
	}

	pXML->GetOptionalTypeEnumWithDelayedResolution(m_iPrereqTrait, L"TraitPrereq");
	pXML->GetOptionalTypeEnumWithDelayedResolution(m_iPrereqOrTrait1, L"TraitPrereqOr1");
	pXML->GetOptionalTypeEnumWithDelayedResolution(m_iPrereqOrTrait2, L"TraitPrereqOr2");

	CvInfoUtil(this).readXml(pXML);

	return true;
}

void CvTraitInfo::copyNonDefaults(CvTraitInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	if (getShortDescription() == cDefault) setShortDescription(pClassInfo->getShortDescription());

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getHealth() == iDefault) m_iHealth = pClassInfo->getHealth();
	if (getHappiness() == iDefault) m_iHappiness = pClassInfo->getHappiness();
	if (getMaxAnarchy() == iDefault) m_iMaxAnarchy = pClassInfo->getMaxAnarchy();
	if (getUpkeepModifier() == iDefault) m_iUpkeepModifier = pClassInfo->getUpkeepModifier();
	if (getLevelExperienceModifier() == iDefault) m_iLevelExperienceModifier = pClassInfo->getLevelExperienceModifier();
	if (getGreatPeopleRateModifier() == iDefault) m_iGreatPeopleRateModifier = pClassInfo->getGreatPeopleRateModifier();
	if (getGreatGeneralRateModifier() == iDefault) m_iGreatGeneralRateModifier = pClassInfo->getGreatGeneralRateModifier();
	if (getDomesticGreatGeneralRateModifier() == iDefault) m_iDomesticGreatGeneralRateModifier = pClassInfo->getDomesticGreatGeneralRateModifier();
	if (getMaxGlobalBuildingProductionModifier() == iDefault) m_iMaxGlobalBuildingProductionModifier = pClassInfo->getMaxGlobalBuildingProductionModifier();
	if (getMaxTeamBuildingProductionModifier() == iDefault) m_iMaxTeamBuildingProductionModifier = pClassInfo->getMaxTeamBuildingProductionModifier();
	if (getMaxPlayerBuildingProductionModifier() == iDefault) m_iMaxPlayerBuildingProductionModifier = pClassInfo->getMaxPlayerBuildingProductionModifier();

	if (!m_paiExtraYieldThreshold) CvXMLLoadUtility::InitList(&m_paiExtraYieldThreshold, NUM_YIELD_TYPES);
	if (!m_paiTradeYieldModifier) CvXMLLoadUtility::InitList(&m_paiTradeYieldModifier, NUM_YIELD_TYPES);
	if (!m_paiExtraYieldThresholdFiltered) CvXMLLoadUtility::InitList(&m_paiExtraYieldThresholdFiltered, NUM_YIELD_TYPES);
	if (!m_paiTradeYieldModifierFiltered) CvXMLLoadUtility::InitList(&m_paiTradeYieldModifierFiltered, NUM_YIELD_TYPES);
	if (!m_paiLessYieldThreshold) CvXMLLoadUtility::InitList(&m_paiLessYieldThreshold, NUM_YIELD_TYPES);
	if (!m_paiLessYieldThresholdFiltered) CvXMLLoadUtility::InitList(&m_paiLessYieldThresholdFiltered, NUM_YIELD_TYPES);
	for ( int j = 0; j < NUM_YIELD_TYPES; j++ )
	{
		if (m_paiExtraYieldThreshold[j] == iDefault)
		{
			m_paiExtraYieldThreshold[j] = pClassInfo->getExtraYieldThreshold(j);
		}
		if (m_paiTradeYieldModifier[j] == iDefault)
		{
			m_paiTradeYieldModifier[j] = pClassInfo->getTradeYieldModifier(j);
		}
		if (m_paiExtraYieldThresholdFiltered[j] == iDefault)
		{
			m_paiExtraYieldThresholdFiltered[j] = pClassInfo->getExtraYieldThreshold(j);
		}
		if (m_paiTradeYieldModifierFiltered[j] == iDefault)
		{
			m_paiTradeYieldModifierFiltered[j] = pClassInfo->getTradeYieldModifier(j);
		}
		if (m_paiLessYieldThreshold[j] == iDefault)
		{
			m_paiLessYieldThreshold[j] = pClassInfo->getLessYieldThreshold(j);
		}
		if (m_paiLessYieldThresholdFiltered[j] == iDefault)
		{
			m_paiLessYieldThresholdFiltered[j] = pClassInfo->getLessYieldThreshold(j);
		}
	}

	if (!m_paiCommerceChange) CvXMLLoadUtility::InitList(&m_paiCommerceChange, NUM_COMMERCE_TYPES);
	if (!m_paiCommerceModifier) CvXMLLoadUtility::InitList(&m_paiCommerceModifier, NUM_COMMERCE_TYPES);
	if (!m_paiCommerceChangeFiltered) CvXMLLoadUtility::InitList(&m_paiCommerceChangeFiltered, NUM_COMMERCE_TYPES);
	if (!m_paiCommerceModifierFiltered) CvXMLLoadUtility::InitList(&m_paiCommerceModifierFiltered, NUM_COMMERCE_TYPES);
	for ( int j = 0; j < NUM_COMMERCE_TYPES; j++ )
	{
		if (m_paiCommerceChange[j] == iDefault)
		{
			m_paiCommerceChange[j] = pClassInfo->getCommerceChange(j);
		}
		if (m_paiCommerceModifier[j] == iDefault)
		{
			m_paiCommerceModifier[j] = pClassInfo->getCommerceModifier(j);
		}
		if (m_paiCommerceChangeFiltered[j] == iDefault)
		{
			m_paiCommerceChangeFiltered[j] = pClassInfo->getCommerceChange(j);
		}
		if (m_paiCommerceModifierFiltered[j] == iDefault)
		{
			m_paiCommerceModifierFiltered[j] = pClassInfo->getCommerceModifier(j);
		}
	}

	for ( int j = 0; j < GC.getNumPromotionInfos(); j++ )
	{
		for ( int i = 0; i < GC.getNumUnitCombatInfos(); i++ )
		{
			if (!isFreePromotionUnitCombats(j, i) && pClassInfo->isFreePromotionUnitCombats(j, i))
			{
				if ( NULL == m_ppbFreePromotionUnitCombats )
				{
					CvXMLLoadUtility::Init2DList(&m_ppbFreePromotionUnitCombats, GC.getNumPromotionInfos(), GC.getNumUnitCombatInfos());
				}
				else if ( NULL == m_ppbFreePromotionUnitCombats[i] )
				{
					CvXMLLoadUtility::InitList(&m_ppbFreePromotionUnitCombats[i],GC.getNumUnitCombatInfos());
				}
				m_ppbFreePromotionUnitCombats[j][i] = pClassInfo->isFreePromotionUnitCombats(j, i);
			}
		}
	}

	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);

	//TB Traits Mods begin
	//Textual References
	if (getPromotionLine() == NO_PROMOTIONLINE) m_ePromotionLine = pClassInfo->getPromotionLine();
	if (m_iGreatPeopleUnitType == NO_UNIT) m_iGreatPeopleUnitType = pClassInfo->m_iGreatPeopleUnitType;
	if (m_iGoldenAgeonBirthofGreatPeopleType == NO_UNIT) m_iGoldenAgeonBirthofGreatPeopleType = pClassInfo->m_iGoldenAgeonBirthofGreatPeopleType;
	if (getPrereqTech() == NO_TECH) m_ePrereqTech = pClassInfo->getPrereqTech();
	if (getEraAdvanceFreeSpecialistType() == NO_SPECIALIST) m_eEraAdvanceFreeSpecialistType = pClassInfo->getEraAdvanceFreeSpecialistType();
	if (getWarWearinessAccumulationModifier() == iDefault) m_iWarWearinessAccumulationModifier = pClassInfo->getWarWearinessAccumulationModifier();
	if (getCivicAnarchyTimeModifier() == iDefault) m_iCivicAnarchyTimeModifier = pClassInfo->getCivicAnarchyTimeModifier();
	if (getReligiousAnarchyTimeModifier() == iDefault) m_iReligiousAnarchyTimeModifier = pClassInfo->getReligiousAnarchyTimeModifier();
	if (getImprovementUpgradeRateModifier() == iDefault) m_iImprovementUpgradeRateModifier = pClassInfo->getImprovementUpgradeRateModifier();
	if (getWorkerSpeedModifier() == iDefault) m_iWorkerSpeedModifier = pClassInfo->getWorkerSpeedModifier();
	if (getMaxConscript() == iDefault) m_iMaxConscript = pClassInfo->getMaxConscript();
	if (getDistanceMaintenanceModifier() == iDefault) m_iDistanceMaintenanceModifier = pClassInfo->getDistanceMaintenanceModifier();
	if (getNumCitiesMaintenanceModifier() == iDefault) m_iNumCitiesMaintenanceModifier = pClassInfo->getNumCitiesMaintenanceModifier();
	if (getCorporationMaintenanceModifier() == iDefault) m_iCorporationMaintenanceModifier = pClassInfo->getCorporationMaintenanceModifier();
	if (getStateReligionGreatPeopleRateModifier() == iDefault) m_iStateReligionGreatPeopleRateModifier = pClassInfo->getStateReligionGreatPeopleRateModifier();
	if (getFreeExperience() == iDefault) m_iFreeExperience = pClassInfo->getFreeExperience();
	if (m_iFreeUnitUpkeepCivilian == iDefault) m_iFreeUnitUpkeepCivilian = pClassInfo->getFreeUnitUpkeepCivilian();
	if (m_iFreeUnitUpkeepMilitary == iDefault) m_iFreeUnitUpkeepMilitary = pClassInfo->getFreeUnitUpkeepMilitary();
	if (m_iFreeUnitUpkeepCivilianPopPercent == iDefault) m_iFreeUnitUpkeepCivilianPopPercent = pClassInfo->getFreeUnitUpkeepCivilianPopPercent();
	if (m_iFreeUnitUpkeepMilitaryPopPercent == iDefault) m_iFreeUnitUpkeepMilitaryPopPercent = pClassInfo->getFreeUnitUpkeepMilitaryPopPercent();
	if (m_iCivilianUnitUpkeepMod == iDefault) m_iCivilianUnitUpkeepMod = pClassInfo->getCivilianUnitUpkeepMod();
	if (m_iMilitaryUnitUpkeepMod == iDefault) m_iMilitaryUnitUpkeepMod = pClassInfo->getMilitaryUnitUpkeepMod();
	if (getHappyPerMilitaryUnit() == iDefault) m_iHappyPerMilitaryUnit = pClassInfo->getHappyPerMilitaryUnit();
	if (getLargestCityHappiness() == iDefault) m_iLargestCityHappiness = pClassInfo->getLargestCityHappiness();
	if (getFreeSpecialist() == iDefault) m_iFreeSpecialist = pClassInfo->getFreeSpecialist();
	if (getTradeRoutes() == iDefault) m_iTradeRoutes = pClassInfo->getTradeRoutes();
	if (getStateReligionHappiness() == iDefault) m_iStateReligionHappiness = pClassInfo->getStateReligionHappiness();
	if (getNonStateReligionHappiness() == iDefault) m_iNonStateReligionHappiness = pClassInfo->getNonStateReligionHappiness();
	if (getStateReligionUnitProductionModifier() == iDefault) m_iStateReligionUnitProductionModifier = pClassInfo->getStateReligionUnitProductionModifier();
	if (getStateReligionBuildingProductionModifier() == iDefault) m_iStateReligionBuildingProductionModifier = pClassInfo->getStateReligionBuildingProductionModifier();
	if (getStateReligionFreeExperience() == iDefault) m_iStateReligionFreeExperience = pClassInfo->getStateReligionFreeExperience();
	if (getExpInBorderModifier() == iDefault) m_iExpInBorderModifier = pClassInfo->getExpInBorderModifier();
	if (getCityDefenseBonus() == iDefault) m_iCityDefenseBonus = pClassInfo->getCityDefenseBonus();
	if (getMilitaryProductionModifier() == iDefault) m_iMilitaryProductionModifier = pClassInfo->getMilitaryProductionModifier();
	if (getAttitudeModifier() == iDefault) m_iAttitudeModifier = pClassInfo->getAttitudeModifier();
	if (getLinePriority() == iDefault) m_iLinePriority = pClassInfo->getLinePriority();
	if (getEspionageDefense() == iDefault) m_iEspionageDefense = pClassInfo->getEspionageDefense();
	if (getMinAnarchy() == iDefault) m_iMinAnarchy = pClassInfo->getMinAnarchy();
	if (getMaxTradeRoutesChange() == iDefault) m_iMaxTradeRoutesChange = pClassInfo->getMaxTradeRoutesChange();
	if (getGoldenAgeDurationModifier() == iDefault) m_iGoldenAgeDurationModifier = pClassInfo->getGoldenAgeDurationModifier();
	if (getGreatPeopleRateChange() == iDefault) m_iGreatPeopleRateChange = pClassInfo->getGreatPeopleRateChange();
	if (getHurryAngerModifier() == iDefault) m_iHurryAngerModifier = pClassInfo->getHurryAngerModifier();
	if (getHurryCostModifier() == iDefault) m_iHurryCostModifier = pClassInfo->getHurryCostModifier();
	if (getEnemyWarWearinessModifier() == iDefault) m_iEnemyWarWearinessModifier = pClassInfo->getEnemyWarWearinessModifier();
	if (getForeignTradeRouteModifier() == iDefault) m_iForeignTradeRouteModifier = pClassInfo->getForeignTradeRouteModifier();
	if (getBombardDefense() == iDefault) m_iBombardDefense = pClassInfo->getBombardDefense();
	if (getUnitUpgradePriceModifier() == iDefault) m_iUnitUpgradePriceModifier = pClassInfo->getUnitUpgradePriceModifier();
	if (getCoastalTradeRoutes() == iDefault) m_iCoastalTradeRoutes = pClassInfo->getCoastalTradeRoutes();
	if (getGlobalPopulationgrowthratepercentage() == iDefault) m_iGlobalPopulationgrowthratepercentage = pClassInfo->getGlobalPopulationgrowthratepercentage();
	if (getCityStartCulture(true) == iDefault) m_iCityStartCulture = pClassInfo->getCityStartCulture(true);
	if (getGlobalAirUnitCapacity() == iDefault) m_iGlobalAirUnitCapacity = pClassInfo->getGlobalAirUnitCapacity();
	if (getCapitalXPModifier() == iDefault) m_iCapitalXPModifier = pClassInfo->getCapitalXPModifier();
	if (getHolyCityofStateReligionXPModifier() == iDefault) m_iHolyCityofStateReligionXPModifier = pClassInfo->getHolyCityofStateReligionXPModifier();
	if (getHolyCityofNonStateReligionXPModifier() == iDefault) m_iHolyCityofNonStateReligionXPModifier = pClassInfo->getHolyCityofNonStateReligionXPModifier();
	if (getBonusPopulationinNewCities() == iDefault) m_iBonusPopulationinNewCities = pClassInfo->getBonusPopulationinNewCities();
	if (getMissileRange() == iDefault) m_iMissileRange = pClassInfo->getMissileRange();
	if (getFlightOperationRange() == iDefault) m_iFlightOperationRange = pClassInfo->getFlightOperationRange();
	if (getNavalCargoSpace() == iDefault) m_iNavalCargoSpace = pClassInfo->getNavalCargoSpace();
	if (getMissileCargoSpace() == iDefault) m_iMissileCargoSpace = pClassInfo->getMissileCargoSpace();
	if (getNationalCaptureProbabilityModifier() == iDefault) m_iNationalCaptureProbabilityModifier = pClassInfo->getNationalCaptureProbabilityModifier();
	if (getNationalCaptureResistanceModifier() == iDefault) m_iNationalCaptureResistanceModifier = pClassInfo->getNationalCaptureResistanceModifier();
	if (getStateReligionSpreadProbabilityModifier() == iDefault) m_iStateReligionSpreadProbabilityModifier = pClassInfo->getStateReligionSpreadProbabilityModifier();
	if (getNonStateReligionSpreadProbabilityModifier() == iDefault) m_iNonStateReligionSpreadProbabilityModifier = pClassInfo->getNonStateReligionSpreadProbabilityModifier();
	if (getFreedomFighterChange() == iDefault) m_iFreedomFighterChange = pClassInfo->getFreedomFighterChange();
	//booleans
	if (isMilitaryFoodProduction() == bDefault) m_bMilitaryFoodProduction = pClassInfo->isMilitaryFoodProduction();
	if (isNegativeTrait() == bDefault) m_bNegativeTrait = pClassInfo->isNegativeTrait();
	if (isImpurePropertyManipulators() == bDefault) m_bImpurePropertyManipulators = pClassInfo->isImpurePropertyManipulators();
	if (isImpurePromotions() == bDefault) m_bImpurePromotions = pClassInfo->isImpurePromotions();
	if (isCivilizationTrait() == bDefault) m_bCivilizationTrait = pClassInfo->isCivilizationTrait();
	if (isAllowsInquisitions() == bDefault) m_bAllowsInquisitions = pClassInfo->isAllowsInquisitions();
	if (isCoastalAIInfluence() == bDefault) m_bCoastalAIInfluence = pClassInfo->isCoastalAIInfluence();
	if (isBarbarianSelectionOnly() == bDefault) m_bBarbarianSelectionOnly = pClassInfo->isBarbarianSelectionOnly();
	if (isCitiesStartwithStateReligion() == bDefault) m_bCitiesStartwithStateReligion = pClassInfo->isCitiesStartwithStateReligion();
	if (isDraftsOnCityCapture() == bDefault) m_bDraftsOnCityCapture = pClassInfo->isDraftsOnCityCapture();
	if (isFreeSpecialistperWorldWonder() == bDefault) m_bFreeSpecialistperWorldWonder = pClassInfo->isFreeSpecialistperWorldWonder();
	if (isFreeSpecialistperNationalWonder() == bDefault) m_bFreeSpecialistperNationalWonder = pClassInfo->isFreeSpecialistperNationalWonder();
	if (isFreeSpecialistperTeamProject() == bDefault) m_bFreeSpecialistperTeamProject = pClassInfo->isFreeSpecialistperTeamProject();
	if (isExtraGoody() == bDefault) m_bExtraGoody = pClassInfo->isExtraGoody();
	if (isAllReligionsActive() == bDefault) m_bAllReligionsActive = pClassInfo->isAllReligionsActive();
	if (isBansNonStateReligions() == bDefault) m_bBansNonStateReligions = pClassInfo->isBansNonStateReligions();
	if (isFreedomFighter() == bDefault) m_bFreedomFighter = pClassInfo->isFreedomFighter();

	// bool vector without delayed resolution
	if (getNumNotOnGameOptions() == 0)
	{
		m_aiNotOnGameOptions.clear();
		for ( int i = 0; i < pClassInfo->getNumNotOnGameOptions(); i++)
		{
			m_aiNotOnGameOptions.push_back(pClassInfo->getNotOnGameOption(i));
		}
	}
	if (getNumOnGameOptions() == 0)
	{
		m_aiOnGameOptions.clear();
		for ( int i = 0; i < pClassInfo->getNumOnGameOptions(); i++)
		{
			m_aiOnGameOptions.push_back(pClassInfo->getOnGameOption(i));
		}
	}

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);

	//Arrays
	for ( int i = 0; i < GC.getNumSpecialistInfos(); i++)
	{
		for ( int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if ( getSpecialistYieldChange(i,j) == iDefault )
			{
				int iChange = pClassInfo->getSpecialistYieldChange(i, j);

				if ( iChange != iDefault )
				{
					if ( m_ppaiSpecialistYieldChange == NULL )
					{
						m_ppaiSpecialistYieldChange = new int*[GC.getNumSpecialistInfos()];
						for(int k = 0; k < GC.getNumSpecialistInfos(); k++)
						{
							m_ppaiSpecialistYieldChange[k] = NULL;
						}
					}
					if ( m_ppaiSpecialistYieldChange[i] == NULL )
					{
						CvXMLLoadUtility::InitList(&m_ppaiSpecialistYieldChange[i], NUM_YIELD_TYPES);
					}

					if ( m_ppaiSpecialistYieldChangeFiltered == NULL )
					{
						m_ppaiSpecialistYieldChangeFiltered = new int*[GC.getNumSpecialistInfos()];
						for(int k = 0; k < GC.getNumSpecialistInfos(); k++)
						{
							m_ppaiSpecialistYieldChangeFiltered[k] = NULL;
						}
					}
					if ( m_ppaiSpecialistYieldChangeFiltered[i] == NULL )
					{
						CvXMLLoadUtility::InitList(&m_ppaiSpecialistYieldChangeFiltered[i], NUM_YIELD_TYPES);
					}

					m_ppaiSpecialistYieldChange[i][j] = iChange;
					m_ppaiSpecialistYieldChangeFiltered[i][j] = iChange;
				}
			}
		}
	}

	for ( int i = 0; i < GC.getNumImprovementInfos(); i++)
	{
		for ( int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if ( getImprovementYieldChange(i,j) == iDefault )
			{
				int iChange = pClassInfo->getImprovementYieldChange(i, j);

				if ( iChange != iDefault )
				{
					if ( m_ppaiImprovementYieldChange == NULL )
					{
						m_ppaiImprovementYieldChange = new int*[GC.getNumImprovementInfos()];
						for(int k = 0; k < GC.getNumImprovementInfos(); k++)
						{
							m_ppaiImprovementYieldChange[k] = NULL;
						}
					}
					if ( m_ppaiImprovementYieldChange[i] == NULL )
					{
						CvXMLLoadUtility::InitList(&m_ppaiImprovementYieldChange[i], NUM_YIELD_TYPES);
					}

					if ( m_ppaiImprovementYieldChangeFiltered == NULL )
					{
						m_ppaiImprovementYieldChangeFiltered = new int*[GC.getNumImprovementInfos()];
						for(int k = 0; k < GC.getNumImprovementInfos(); k++)
						{
							m_ppaiImprovementYieldChangeFiltered[k] = NULL;
						}
					}
					if ( m_ppaiImprovementYieldChangeFiltered[i] == NULL )
					{
						CvXMLLoadUtility::InitList(&m_ppaiImprovementYieldChangeFiltered[i], NUM_YIELD_TYPES);
					}

					m_ppaiImprovementYieldChange[i][j] = iChange;
					m_ppaiImprovementYieldChangeFiltered[i][j] = iChange;
				}
			}
		}
	}

	if (!m_piYieldModifier) CvXMLLoadUtility::InitList(&m_piYieldModifier, NUM_YIELD_TYPES);
	if (!m_piYieldModifierFiltered) CvXMLLoadUtility::InitList(&m_piYieldModifierFiltered, NUM_YIELD_TYPES);
	if (!m_piCapitalYieldModifier) CvXMLLoadUtility::InitList(&m_piCapitalYieldModifier, NUM_YIELD_TYPES);
	if (!m_piCapitalYieldModifierFiltered) CvXMLLoadUtility::InitList(&m_piCapitalYieldModifierFiltered, NUM_YIELD_TYPES);
	if (!m_piSpecialistExtraYield) CvXMLLoadUtility::InitList(&m_piSpecialistExtraYield, NUM_YIELD_TYPES);
	if (!m_piSpecialistExtraYieldFiltered) CvXMLLoadUtility::InitList(&m_piSpecialistExtraYieldFiltered, NUM_YIELD_TYPES);
	if (!m_piYieldChange) CvXMLLoadUtility::InitList(&m_piYieldChange, NUM_YIELD_TYPES);
	if (!m_piYieldChangeFiltered) CvXMLLoadUtility::InitList(&m_piYieldChangeFiltered, NUM_YIELD_TYPES);
	if (!m_piSeaPlotYieldChanges) CvXMLLoadUtility::InitList(&m_piSeaPlotYieldChanges, NUM_YIELD_TYPES);
	if (!m_piSeaPlotYieldChangesFiltered) CvXMLLoadUtility::InitList(&m_piSeaPlotYieldChangesFiltered, NUM_YIELD_TYPES);
	if (!m_piGoldenAgeYieldChanges) CvXMLLoadUtility::InitList(&m_piGoldenAgeYieldChanges, NUM_YIELD_TYPES);
	if (!m_piGoldenAgeYieldChangesFiltered) CvXMLLoadUtility::InitList(&m_piGoldenAgeYieldChangesFiltered, NUM_YIELD_TYPES);
	for ( int i = 0; i < NUM_YIELD_TYPES; i++ )
	{
		if ( m_piYieldModifier[i] == iDefault )
		{
			m_piYieldModifier[i] = pClassInfo->getYieldModifier(i);
		}
		if ( m_piYieldModifierFiltered[i] == iDefault )
		{
			m_piYieldModifierFiltered[i] = pClassInfo->getYieldModifier(i);
		}

		if ( m_piCapitalYieldModifier[i] == iDefault )
		{
			m_piCapitalYieldModifier[i] = pClassInfo->getCapitalYieldModifier(i);
		}
		if ( m_piCapitalYieldModifierFiltered[i] == iDefault )
		{
			m_piCapitalYieldModifierFiltered[i] = pClassInfo->getCapitalYieldModifier(i);
		}

		if ( m_piSpecialistExtraYield[i] == iDefault )
		{
			m_piSpecialistExtraYield[i] = pClassInfo->getSpecialistExtraYield(i);
		}
		if ( m_piSpecialistExtraYieldFiltered[i] == iDefault )
		{
			m_piSpecialistExtraYieldFiltered[i] = pClassInfo->getSpecialistExtraYield(i);
		}

		if ( m_piYieldChange[i] == iDefault )
		{
			m_piYieldChange[i] = pClassInfo->getYieldChange(i);
		}
		if ( m_piYieldChangeFiltered[i] == iDefault )
		{
			m_piYieldChangeFiltered[i] = pClassInfo->getYieldChange(i);
		}

		if ( m_piSeaPlotYieldChanges[i] == iDefault )
		{
			m_piSeaPlotYieldChanges[i] = pClassInfo->getSeaPlotYieldChanges(i);
		}
		if ( m_piSeaPlotYieldChangesFiltered[i] == iDefault )
		{
			m_piSeaPlotYieldChangesFiltered[i] = pClassInfo->getSeaPlotYieldChanges(i);
		}

//Team Project (7)
		if ( m_piGoldenAgeYieldChanges[i] == iDefault )
		{
			m_piGoldenAgeYieldChanges[i] = pClassInfo->getGoldenAgeYieldChanges(i);
		}
		if ( m_piGoldenAgeYieldChangesFiltered[i] == iDefault )
		{
			m_piGoldenAgeYieldChangesFiltered[i] = pClassInfo->getGoldenAgeYieldChanges(i);
		}
	}

	if (!m_piCapitalCommerceModifier) CvXMLLoadUtility::InitList(&m_piCapitalCommerceModifier, NUM_COMMERCE_TYPES);
	if (!m_piCapitalCommerceModifierFiltered) CvXMLLoadUtility::InitList(&m_piCapitalCommerceModifierFiltered, NUM_COMMERCE_TYPES);
	if (!m_piSpecialistExtraCommerce) CvXMLLoadUtility::InitList(&m_piSpecialistExtraCommerce, NUM_COMMERCE_TYPES);
	if (!m_piSpecialistExtraCommerceFiltered) CvXMLLoadUtility::InitList(&m_piSpecialistExtraCommerceFiltered, NUM_COMMERCE_TYPES);
	if (!m_piGoldenAgeCommerceChanges) CvXMLLoadUtility::InitList(&m_piGoldenAgeCommerceChanges, NUM_COMMERCE_TYPES);
	if (!m_piGoldenAgeCommerceChangesFiltered) CvXMLLoadUtility::InitList(&m_piGoldenAgeCommerceChangesFiltered, NUM_COMMERCE_TYPES);
	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++ )
	{
		if ( m_piCapitalCommerceModifier[i] == iDefault )
		{
			m_piCapitalCommerceModifier[i] = pClassInfo->getCapitalCommerceModifier(i);
		}
		if ( m_piCapitalCommerceModifierFiltered[i] == iDefault )
		{
			m_piCapitalCommerceModifierFiltered[i] = pClassInfo->getCapitalCommerceModifier(i);
		}

		if ( m_piSpecialistExtraCommerce[i] == iDefault )
		{
			m_piSpecialistExtraCommerce[i] = pClassInfo->getSpecialistExtraCommerce(i);
		}
		if ( m_piSpecialistExtraCommerceFiltered[i] == iDefault )
		{
			m_piSpecialistExtraCommerceFiltered[i] = pClassInfo->getSpecialistExtraCommerce(i);
		}

		if ( m_piGoldenAgeCommerceChanges[i] == iDefault )
		{
			m_piGoldenAgeCommerceChanges[i] = pClassInfo->getGoldenAgeCommerceChanges(i);
		}
		if ( m_piGoldenAgeCommerceChangesFiltered[i] == iDefault )
		{
			m_piGoldenAgeCommerceChangesFiltered[i] = pClassInfo->getGoldenAgeCommerceChanges(i);
		}
	}

	for ( int i = 0; i < GC.getNumSpecialistInfos(); i++)
	{
		for ( int j = 0; j < NUM_COMMERCE_TYPES; j++)
		{
			if ( getSpecialistCommerceChange(i,j) == iDefault )
			{
				int iChange = pClassInfo->getSpecialistCommerceChange(i, j);

				if ( iChange != iDefault )
				{
					if ( m_ppaiSpecialistCommerceChange == NULL )
					{
						m_ppaiSpecialistCommerceChange = new int*[GC.getNumSpecialistInfos()];
						for(int k = 0; k < GC.getNumSpecialistInfos(); k++)
						{
							m_ppaiSpecialistCommerceChange[k] = NULL;
						}
					}
					if ( m_ppaiSpecialistCommerceChange[i] == NULL )
					{
						CvXMLLoadUtility::InitList(&m_ppaiSpecialistCommerceChange[i], NUM_COMMERCE_TYPES);
					}

					if ( m_ppaiSpecialistCommerceChangeFiltered == NULL )
					{
						m_ppaiSpecialistCommerceChangeFiltered = new int*[GC.getNumSpecialistInfos()];
						for(int k = 0; k < GC.getNumSpecialistInfos(); k++)
						{
							m_ppaiSpecialistCommerceChangeFiltered[k] = NULL;
						}
					}
					if ( m_ppaiSpecialistCommerceChangeFiltered[i] == NULL )
					{
						CvXMLLoadUtility::InitList(&m_ppaiSpecialistCommerceChangeFiltered[i], NUM_COMMERCE_TYPES);
					}

					m_ppaiSpecialistCommerceChange[i][j] = iChange;
					m_ppaiSpecialistCommerceChangeFiltered[i][j] = iChange;
				}
			}
		}
	}

	if (getNumDisallowedTraitTypes() == 0)
	{
		int iNum = pClassInfo->getNumDisallowedTraitTypes();
		m_aDisallowedTraitTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			GC.copyNonDefaultDelayedResolution((int*)&(m_aDisallowedTraitTypes[i].eTrait), (int*)&(pClassInfo->m_aDisallowedTraitTypes[i].eTrait));
		}
	}

	if (getNumCivicOptionNoUpkeepTypes() == 0)
	{
		int iNum = pClassInfo->getNumCivicOptionNoUpkeepTypes();
		m_aCivicOptionNoUpkeepTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aCivicOptionNoUpkeepTypes[i].bBool = pClassInfo->m_aCivicOptionNoUpkeepTypes[i].bBool;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aCivicOptionNoUpkeepTypes[i].eCivicOption), (int*)&(pClassInfo->m_aCivicOptionNoUpkeepTypes[i].eCivicOption));
		}
	}

	for ( int i = 0; i < GC.getNumFlavorTypes(); i++ )
	{
		if ( getFlavorValue(i) == iDefault && pClassInfo->getFlavorValue(i) != iDefault)
		{
			if ( NULL == m_piFlavorValue )
			{
				CvXMLLoadUtility::InitList(&m_piFlavorValue,GC.getNumFlavorTypes(),iDefault);
			}
			m_piFlavorValue[i] = pClassInfo->getFlavorValue(i);
		}
	}

	if (getNumBuildingProductionModifiers() == 0)
	{
		int iNum = pClassInfo->getNumBuildingProductionModifiers();
		m_aBuildingProductionModifiers.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aBuildingProductionModifiers[i].iModifier = pClassInfo->m_aBuildingProductionModifiers[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aBuildingProductionModifiers[i].eBuilding), (int*)&(pClassInfo->m_aBuildingProductionModifiers[i].eBuilding));
		}
	}

	if (getNumUnitProductionModifiers() == 0)
	{
		int iNum = pClassInfo->getNumUnitProductionModifiers();
		m_aUnitProductionModifiers.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aUnitProductionModifiers[i].iModifier = pClassInfo->m_aUnitProductionModifiers[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aUnitProductionModifiers[i].eUnit), (int*)&(pClassInfo->m_aUnitProductionModifiers[i].eUnit));
		}
	}

	if (getNumSpecialUnitProductionModifiers() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aSpecialUnitProductionModifiers, pClassInfo->m_aSpecialUnitProductionModifiers);
	}

//Team Project (8)
	if (getNumUnitCombatFreeExperiences() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aUnitCombatFreeExperiences, pClassInfo->m_aUnitCombatFreeExperiences);
	}

	if (getNumUnitCombatProductionModifiers() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aUnitCombatProductionModifiers, pClassInfo->m_aUnitCombatProductionModifiers);
	}

	GC.copyNonDefaultDelayedResolution((int*)&m_iPrereqTrait, (int*)&pClassInfo->m_iPrereqTrait);
	GC.copyNonDefaultDelayedResolution((int*)&m_iPrereqOrTrait1, (int*)&pClassInfo->m_iPrereqOrTrait1);
	GC.copyNonDefaultDelayedResolution((int*)&m_iPrereqOrTrait2, (int*)&pClassInfo->m_iPrereqOrTrait2);

	CvInfoUtil(this).copyNonDefaults(pClassInfo);
}

void CvTraitInfo::getCheckSum(uint32_t& iSum) const
{
	PROFILE_EXTRA_FUNC();
	for ( int j = 0; j < GC.getNumPromotionInfos(); j++ )
	{
		for ( int i = 0; i < GC.getNumUnitCombatInfos(); i++ )
		{
			CheckSum(iSum, isFreePromotionUnitCombats(j,i));
		}
	}

	CheckSum(iSum, m_iHealth);
	CheckSum(iSum, m_iHappiness);
	CheckSum(iSum, m_iMaxAnarchy);
	CheckSum(iSum, m_iUpkeepModifier);
	CheckSum(iSum, m_iLevelExperienceModifier);
	CheckSum(iSum, m_iGreatPeopleRateModifier);
	CheckSum(iSum, m_iGreatGeneralRateModifier);
	CheckSum(iSum, m_iDomesticGreatGeneralRateModifier);
	CheckSum(iSum, m_iMaxGlobalBuildingProductionModifier);
	CheckSum(iSum, m_iMaxTeamBuildingProductionModifier);
	CheckSum(iSum, m_iMaxPlayerBuildingProductionModifier);

	CheckSum(iSum, m_iRevIdxLocal);
	CheckSum(iSum, m_iRevIdxNational);
	CheckSum(iSum, m_iRevIdxDistanceModifier);
	CheckSum(iSum, m_iRevIdxHolyCityGood);
	CheckSum(iSum, m_iRevIdxHolyCityBad);
	CheckSum(iSum, m_fRevIdxNationalityMod);
	CheckSum(iSum, m_fRevIdxGoodReligionMod);
	CheckSum(iSum, m_fRevIdxBadReligionMod);
	CheckSum(iSum, m_bNonStateReligionCommerce);
	CheckSum(iSum, m_bUpgradeAnywhere);

	// Arrays

	CheckSum(iSum, m_paiExtraYieldThreshold, NUM_YIELD_TYPES);
	CheckSum(iSum, m_paiTradeYieldModifier, NUM_YIELD_TYPES);
	CheckSum(iSum, m_paiCommerceChange, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_paiCommerceModifier, NUM_COMMERCE_TYPES);

	m_PropertyManipulators.getCheckSum(iSum);

	//TB Traits Mods begin
	//Textual References
	CheckSum(iSum, m_iPrereqTrait);
	CheckSum(iSum, m_iPrereqOrTrait1);
	CheckSum(iSum, m_iPrereqOrTrait2);
	CheckSum(iSum, m_ePromotionLine);
	CheckSum(iSum, m_iGreatPeopleUnitType);
	CheckSum(iSum, m_iGoldenAgeonBirthofGreatPeopleType);
	CheckSum(iSum, m_ePrereqTech);
	CheckSum(iSum, m_eEraAdvanceFreeSpecialistType);
	//integers
	CheckSum(iSum, m_iWarWearinessAccumulationModifier);
	CheckSum(iSum, m_iCivicAnarchyTimeModifier);
	CheckSum(iSum, m_iReligiousAnarchyTimeModifier);
	CheckSum(iSum, m_iImprovementUpgradeRateModifier);
	CheckSum(iSum, m_iWorkerSpeedModifier);
	CheckSum(iSum, m_iMaxConscript);
	CheckSum(iSum, m_iDistanceMaintenanceModifier);
	CheckSum(iSum, m_iNumCitiesMaintenanceModifier);
	CheckSum(iSum, m_iCorporationMaintenanceModifier);
	CheckSum(iSum, m_iStateReligionGreatPeopleRateModifier);
	CheckSum(iSum, m_iFreeExperience);
	CheckSum(iSum, m_iFreeUnitUpkeepCivilian);
	CheckSum(iSum, m_iFreeUnitUpkeepMilitary);
	CheckSum(iSum, m_iFreeUnitUpkeepCivilianPopPercent);
	CheckSum(iSum, m_iFreeUnitUpkeepMilitaryPopPercent);
	CheckSum(iSum, m_iCivilianUnitUpkeepMod);
	CheckSum(iSum, m_iMilitaryUnitUpkeepMod);
	CheckSum(iSum, m_iHappyPerMilitaryUnit);
	CheckSum(iSum, m_iLargestCityHappiness);
	CheckSum(iSum, m_iFreeSpecialist);
	CheckSum(iSum, m_iTradeRoutes);
	CheckSum(iSum, m_iStateReligionHappiness);
	CheckSum(iSum, m_iNonStateReligionHappiness);
	CheckSum(iSum, m_iStateReligionUnitProductionModifier);
	CheckSum(iSum, m_iStateReligionBuildingProductionModifier);
	CheckSum(iSum, m_iStateReligionFreeExperience);
	CheckSum(iSum, m_iExpInBorderModifier);
	CheckSum(iSum, m_iCityDefenseBonus);
	CheckSum(iSum, m_iMilitaryProductionModifier);
	CheckSum(iSum, m_iAttitudeModifier);
	CheckSum(iSum, m_iLinePriority);
	CheckSum(iSum, m_iEspionageDefense);
	CheckSum(iSum, m_iMinAnarchy);
	CheckSum(iSum, m_iMaxTradeRoutesChange);
	CheckSum(iSum, m_iGoldenAgeDurationModifier);
	CheckSum(iSum, m_iGreatPeopleRateChange);
	CheckSum(iSum, m_iHurryAngerModifier);
	CheckSum(iSum, m_iHurryCostModifier);
	CheckSum(iSum, m_iEnemyWarWearinessModifier);
	CheckSum(iSum, m_iForeignTradeRouteModifier);
	CheckSum(iSum, m_iBombardDefense);
	CheckSum(iSum, m_iUnitUpgradePriceModifier);
	CheckSum(iSum, m_iCoastalTradeRoutes);
	CheckSum(iSum, m_iGlobalPopulationgrowthratepercentage);
	CheckSum(iSum, m_iCityStartCulture);
	CheckSum(iSum, m_iGlobalAirUnitCapacity);
	CheckSum(iSum, m_iCapitalXPModifier);
	CheckSum(iSum, m_iHolyCityofStateReligionXPModifier);
	CheckSum(iSum, m_iHolyCityofNonStateReligionXPModifier);
	CheckSum(iSum, m_iBonusPopulationinNewCities);
	CheckSum(iSum, m_iMissileRange);
	CheckSum(iSum, m_iFlightOperationRange);
	CheckSum(iSum, m_iNavalCargoSpace);
	CheckSum(iSum, m_iMissileCargoSpace);
	CheckSum(iSum, m_iNationalCaptureProbabilityModifier);
	CheckSum(iSum, m_iNationalCaptureResistanceModifier);
	CheckSum(iSum, m_iStateReligionSpreadProbabilityModifier);
	CheckSum(iSum, m_iNonStateReligionSpreadProbabilityModifier);
	CheckSum(iSum, m_iFreedomFighterChange);
	//booleans
	CheckSum(iSum, m_bMilitaryFoodProduction);
	CheckSum(iSum, m_bNegativeTrait);
	CheckSum(iSum, m_bImpurePropertyManipulators);
	CheckSum(iSum, m_bImpurePromotions);
	CheckSum(iSum, m_bCivilizationTrait);
	CheckSum(iSum, m_bAllowsInquisitions);
	CheckSum(iSum, m_bCoastalAIInfluence);
	CheckSum(iSum, m_bBarbarianSelectionOnly);
	CheckSum(iSum, m_bCitiesStartwithStateReligion);
	CheckSum(iSum, m_bDraftsOnCityCapture);
	CheckSum(iSum, m_bFreeSpecialistperWorldWonder);
	CheckSum(iSum, m_bFreeSpecialistperNationalWonder);
	CheckSum(iSum, m_bFreeSpecialistperTeamProject);
	CheckSum(iSum, m_bExtraGoody);
	CheckSum(iSum, m_bAllReligionsActive);
	CheckSum(iSum, m_bBansNonStateReligions);
	CheckSum(iSum, m_bFreedomFighter);
	// bool vector without delayed resolution
	CheckSumC(iSum, m_aiNotOnGameOptions);
	CheckSumC(iSum, m_aiOnGameOptions);
	CheckSumC(iSum, m_aiCategories);
	//Arrays
	if (m_ppaiSpecialistYieldChange)
	{
		for(int i=0;i<GC.getNumSpecialistInfos();i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppaiSpecialistYieldChange[i]);
		}
	}

	CheckSumI(iSum, NUM_YIELD_TYPES, m_piYieldModifier);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piCapitalYieldModifier);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piCapitalCommerceModifier);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piSpecialistExtraCommerce);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piSpecialistExtraYield);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piYieldChange);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piSeaPlotYieldChanges);
//Team Project (7)
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piGoldenAgeYieldChanges);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piGoldenAgeCommerceChanges);

	if (m_ppaiSpecialistCommerceChange)
	{
		for(int i=0;i<GC.getNumSpecialistInfos();i++)
		{
			CheckSumI(iSum, NUM_COMMERCE_TYPES, m_ppaiSpecialistCommerceChange[i]);
		}
	}

	CheckSum(iSum, m_paiLessYieldThreshold, NUM_YIELD_TYPES);
	if (m_ppaiImprovementYieldChange)
	{
		for(int i=0;i<GC.getNumImprovementInfos();i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppaiImprovementYieldChange[i]);
		}
	}
	// int vectors utilizing struct with delayed resolution
	int iNumElements;
	int i;

	iNumElements = m_aDisallowedTraitTypes.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aDisallowedTraitTypes[i].eTrait);
	}

	CheckSum(iSum, m_piFlavorValue, GC.getNumFlavorTypes());

	iNumElements = m_aBuildingProductionModifiers.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aBuildingProductionModifiers[i].eBuilding);
		CheckSum(iSum, m_aBuildingProductionModifiers[i].iModifier);
	}

	iNumElements = m_aUnitProductionModifiers.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aUnitProductionModifiers[i].eUnit);
		CheckSum(iSum, m_aUnitProductionModifiers[i].iModifier);
	}

	iNumElements = m_aSpecialUnitProductionModifiers.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aSpecialUnitProductionModifiers[i].eSpecialUnit);
		CheckSum(iSum, m_aSpecialUnitProductionModifiers[i].iModifier);
	}

	iNumElements = m_aCivicOptionNoUpkeepTypes.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aCivicOptionNoUpkeepTypes[i].eCivicOption);
		CheckSum(iSum, m_aCivicOptionNoUpkeepTypes[i].bBool);
	}

//Team Project (8)
	iNumElements = m_aUnitCombatFreeExperiences.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aUnitCombatFreeExperiences[i].eUnitCombat);
		CheckSum(iSum, m_aUnitCombatFreeExperiences[i].iModifier);
	}

	iNumElements = m_aUnitCombatProductionModifiers.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aUnitCombatProductionModifiers[i].eUnitCombat);
		CheckSum(iSum, m_aUnitCombatProductionModifiers[i].iModifier);
	}

	//For Pure Traits
	CheckSum(iSum, m_paiExtraYieldThresholdFiltered, NUM_YIELD_TYPES);
	CheckSum(iSum, m_paiTradeYieldModifierFiltered, NUM_YIELD_TYPES);
	CheckSum(iSum, m_paiCommerceChangeFiltered, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_paiCommerceModifierFiltered, NUM_COMMERCE_TYPES);
	if (m_ppaiSpecialistYieldChangeFiltered)
	{
		for(i=0;i<GC.getNumSpecialistInfos();i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppaiSpecialistYieldChangeFiltered[i]);
		}
	}

	CheckSumI(iSum, NUM_YIELD_TYPES, m_piYieldModifierFiltered);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piCapitalYieldModifierFiltered);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piCapitalCommerceModifierFiltered);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piSpecialistExtraCommerceFiltered);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piSpecialistExtraYieldFiltered);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piYieldChangeFiltered);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piSeaPlotYieldChangesFiltered);
//Team Project (7)
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piGoldenAgeYieldChangesFiltered);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piGoldenAgeCommerceChangesFiltered);
	CheckSum(iSum, m_paiLessYieldThresholdFiltered, NUM_YIELD_TYPES);

	if (m_ppaiSpecialistCommerceChangeFiltered)
	{
		for(i=0;i<GC.getNumSpecialistInfos();i++)
		{
			CheckSumI(iSum, NUM_COMMERCE_TYPES, m_ppaiSpecialistCommerceChangeFiltered[i]);
		}
	}

	if (m_ppaiImprovementYieldChangeFiltered)
	{
		for(i=0;i<GC.getNumImprovementInfos();i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppaiImprovementYieldChangeFiltered[i]);
		}
	}

	//TB Traits Mods end

	CvInfoUtil(const_cast<CvTraitInfo*>(this)).checkSum(iSum);
}


const CvPropertyManipulators* CvTraitInfo::getPropertyManipulators() const
{
	if (GC.getGame().isOption(GAMEOPTION_LEADER_PURE_TRAITS) && isImpurePropertyManipulators())
	{
		return &m_PropertyManipulatorsNull;
	}

	return &m_PropertyManipulators;
}