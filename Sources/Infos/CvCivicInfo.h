#pragma once

#ifndef CV_CIVIC_INFO_H
#define CV_CIVIC_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCivicInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCivicInfo
	: public CvInfoBase
	, private bst::noncopyable
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvCivicInfo();
	virtual ~CvCivicInfo();

	std::wstring pyGetWeLoveTheKing() const { return getWeLoveTheKing(); }
	const wchar_t* getWeLoveTheKing() const;
	const wchar_t* getWeLoveTheKingKey() const;

	int getCivicOptionType() const;
	int getAnarchyLength() const;
	int getUpkeep() const;
	int getAIWeight() const;
	int getGreatPeopleRateModifier() const;
	int getGreatGeneralRateModifier() const;
	int getDomesticGreatGeneralRateModifier() const;
	int getStateReligionGreatPeopleRateModifier() const;
	int getDistanceMaintenanceModifier() const;
	int getNumCitiesMaintenanceModifier() const;
	int getHomeAreaMaintenanceModifier() const;
	int getOtherAreaMaintenanceModifier() const;
	int getCorporationMaintenanceModifier() const;
	int getExtraHealth() const;
	int getFreeExperience() const;
	int getWorkerSpeedModifier() const;
	int getImprovementUpgradeRateModifier() const;
	int getMilitaryProductionModifier() const;
	int getFreeUnitUpkeepCivilian() const;
	int getFreeUnitUpkeepMilitary() const;
	int getFreeUnitUpkeepCivilianPopPercent() const;
	int getFreeUnitUpkeepMilitaryPopPercent() const;
	int getCivilianUnitUpkeepMod() const;
	int getMilitaryUnitUpkeepMod() const;
	int getHappyPerMilitaryUnit() const;
	int getLargestCityHappiness() const;
	int getWarWearinessModifier() const;
	int getFreeSpecialist() const;
	int getTradeRoutes() const;
	TechTypes getTechPrereq() const { return m_iTechPrereq; }
	int getCivicPercentAnger() const;
	int getMaxConscript() const;
	int getStateReligionHappiness() const;
	int getNonStateReligionHappiness() const;
	int getStateReligionUnitProductionModifier() const;
	int getStateReligionBuildingProductionModifier() const;
	int getStateReligionFreeExperience() const;
	int getExpInBorderModifier() const;
	int getRevIdxLocal() const;
	int getRevIdxNational() const;
	int getRevIdxDistanceModifier() const;
	int getRevIdxHolyCityGood() const;
	int getRevIdxHolyCityBad() const;
	int getRevIdxSwitchTo() const;
	int getRevReligiousFreedom() const;
	int getRevLaborFreedom() const;
	int getRevEnvironmentalProtection() const;
	int getRevDemocracyLevel() const;
	int getAttitudeShareMod() const;
	int getEnslavementChance() const;
	int getForeignerUnhappyPercent() const;
	int getCityOverLimitUnhappy() const;
	int getLandmarkHappiness() const;
	int getForeignTradeRouteModifier() const;
	int getTaxRateUnhappiness() const;
	int getPopulationgrowthratepercentage() const;
	int getReligionSpreadRate() const;
	int getCivicHappiness() const;
	int getDistantUnitSupportCostModifier() const;
	int getExtraCityDefense() const;
	int getNationalCaptureProbabilityModifier() const;
	int getNationalCaptureResistanceModifier() const;
	int getInflationModifier() const;
	int getHurryInflationModifier() const;
	int getHurryCostModifier() const;
	int getSharedCivicTradeRouteModifier() const;
	int getCorporationSpreadRate() const;
	int getFreedomFighterChange() const;

	float getRevIdxNationalityMod() const;
	float getRevIdxBadReligionMod() const;
	float getRevIdxGoodReligionMod() const;
	float getRevViolentMod() const;

	bool isUpgradeAnywhere() const;
	bool isAllowInquisitions() const;
	bool isDisallowInquisitions() const;
	bool isCommunism() const;
	bool isFreeSpeech() const;
	bool isCanDoElection() const;
	bool isMilitaryFoodProduction() const;
	bool isNoUnhealthyPopulation() const;
	bool isBuildingOnlyHealthy() const;
	bool isNoForeignTrade() const;
	bool isNoCorporations() const;
	bool isNoForeignCorporations() const;
	bool isStateReligion() const;
	bool isNoNonStateReligionSpread() const;
	bool IsFixedBorders() const;
	bool isNoCapitalUnhappiness() const;
	bool isNoLandmarkAnger() const;
	bool isAllReligionsActive() const;
	bool isBansNonStateReligions() const;
	bool isFreedomFighter() const;
	bool isPolicy() const;

	// Arrays

	int getYieldModifier(int i) const;
	int* getYieldModifierArray() const;
	int getCapitalYieldModifier(int i) const;
	int* getCapitalYieldModifierArray() const;
	int getTradeYieldModifier(int i) const;
	int* getTradeYieldModifierArray() const;
	int getCommerceModifier(int i) const;
	int* getCommerceModifierArray() const;
	int getCapitalCommerceModifier(int i) const;
	int* getCapitalCommerceModifierArray() const;
	int getSpecialistExtraCommerce(int i) const;
	int* getSpecialistExtraCommerceArray() const;
	int getCivicAttitudeChange(int i) const;
	int* getCivicAttitudeChanges() const;
	int getLandmarkYieldChanges(int i) const;
	int* getLandmarkYieldChangesArray() const;

	int getBonusCommerceModifier(int i, int j) const;
	int* getBonusCommerceModifierArray(int i) const;

	int getBuildingProductionModifier(BuildingTypes e) const;
	const IDValueMap<BuildingTypes, int>& getBuildingProductionModifiers() const { return m_aBuildingProductionModifier; }
	const boost::python::list cyGetBuildingProductionModifiers() const { return m_aBuildingProductionModifier.makeList(); }

	int getBuildingHappinessChanges(int i) const;
	int getBuildingHealthChanges(int i) const;


	// ---------------------------------------------------------------------
    	// Sparse views over the per-building modifier arrays.
    	//
    	// The dense int* arrays (m_paiBuildingHappinessChanges /
    	// m_paiBuildingHealthChanges) remain the canonical storage. They preserve
    	// the XML schema, the save-load layout, and the existing per-index
    	// accessors getBuildingHappinessChanges(i) / getBuildingHealthChanges(i).
    	//
    	// The sparse views below exist purely as a query-time optimization for
    	// callers that previously did a full sweep over every building to apply a
    	// civic's effect, e.g.:
    	//
    	//     for (i = 0; i < GC.getNumBuildingInfos(); i++)
    	//         changeExtraBuildingHappiness(
    	//             (BuildingTypes)i,
    	//             kCivic.getBuildingHappinessChanges(i) * iChange, ...);
    	//
    	// In a typical civic only a handful of buildings have a non-zero modifier,
    	// so that loop spends almost all of its iterations calling change*() with
    	// a zero delta (and relying on the early-return inside change*() to make
    	// it a no-op). Migrated callers iterate only the non-zero entries:
    	//
    	//     foreach_(const BuildingModifier2& m,
    	//              kCivic.getBuildingHappinessChangesSparse())
    	//         changeExtraBuildingHappiness(m.first, m.second * iChange, ...);
    	//
    	// Cache lifecycle: filled lazily on first access from the dense arrays,
    	// invalidated by copyNonDefaults() when the dense storage is mutated.
    	// Same template applies to the other per-X dense arrays on this class
    	// (FeatureHappinessChanges, BuildingCommerceChange/Modifier,
    	// ImprovementYieldChanges, ...) once the pattern lands here.
    	// ---------------------------------------------------------------------
    	const std::vector<BuildingModifier2>& getBuildingHappinessChangesSparse() const;
    	const std::vector<BuildingModifier2>& getBuildingHealthChangesSparse() const;
    	// Same pattern, indexed by FeatureTypes instead of BuildingTypes.
    	const std::vector<std::pair<FeatureTypes, int> >& getFeatureHappinessChangesSparse() const;
    	void invalidateSparseLists();


	int getFeatureHappinessChanges(int i) const;
	int getBonusMintedPercent(int i) const;
	int getFreeSpecialistCount(int i) const;
	int getFlavorValue(int i) const;
	int getUnitCombatProductionModifier(int i) const;
	int getUnitProductionModifier(int i) const;
	int getImprovementHappinessChanges(int i) const;
	int getImprovementHealthPercentChanges(int i) const;

	bool isHurry(int i) const;
	bool isSpecialBuildingNotRequired(int i) const;
	bool isSpecialistValid(int i) const;

	int getImprovementYieldChanges(int i, int j) const;
	int getTerrainYieldChanges(int i, int j) const;
	int getBuildingCommerceModifier(int i, int j) const;
	int getBuildingCommerceChange(int i, int j) const;
	int getSpecialistYieldPercentChanges(int i, int j) const;
	int getSpecialistCommercePercentChanges(int i, int j) const;

	int getCityLimit(PlayerTypes ePlayer) const;

	bool isAnyBuildingHappinessChange() const;
	bool isAnyBuildingHealthChange() const;
	bool isAnyFeatureHappinessChange() const;
	bool isAnySpecialistValid() const;
	bool isAnyImprovementYieldChange() const;
	//bool isAnySpecialistCommerceChanges() const { return m_ppaiSpecialistCommerceChange != NULL; }
	//bool isAnySpecialistYieldChanges() const { return m_ppaiSpecialistYieldChange != NULL; }

	CvString getCivicAttitudeReason(int i) const;
	int getCivicAttitudeVectorSize() const;
	CvString getCivicAttitudeNamesVectorElement(int i) const;
	int getCivicAttitudeValuesVectorElement(int i) const;
	int getCivicAttitudeReasonVectorSize() const;
	CvString getCivicAttitudeReasonNamesVectorElement(int i) const;
	CvString getCivicAttitudeReasonValuesVectorElement(int i) const;

	int getCategory(int i) const;
	int getNumCategories() const;
	bool isCategory(int i) const;

	bool readPass3();
	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvCivicInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;


	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

private:
	CvPropertyManipulators m_PropertyManipulators;

protected:
	CvString m_szGlobalDefine;
	CvString* m_pszCivicAttitudeReason;
	CvWString m_szWeLoveTheKingKey;

	int m_iPopulationgrowthratepercentage;
	int m_iReligionSpreadRate;
	int m_iCivicHappiness;
	int m_iDistantUnitSupportCostModifier;
	int m_iExtraCityDefense;
	int m_iAttitudeShareMod;
	int m_iEnslavementChance;
	int m_iForeignTradeRouteModifier;
	int m_iTaxRateUnhappiness;
	int m_iInflationModifier;
	int m_iHurryInflationModifier;
	int m_iHurryCostModifier;
	int m_iSharedCivicTradeRouteModifier;
	int m_iLandmarkHappiness;
	int m_iCorporationSpreadRate;
	int m_iCityLimit;
	int m_iCityOverLimitUnhappy;
	int m_iForeignerUnhappyPercent;
	int m_iCivicOptionType;
	int m_iAnarchyLength;
	int m_iUpkeep;
	int m_iAIWeight;
	int m_iGreatPeopleRateModifier;
	int m_iGreatGeneralRateModifier;
	int m_iDomesticGreatGeneralRateModifier;
	int m_iStateReligionGreatPeopleRateModifier;
	int m_iDistanceMaintenanceModifier;
	int m_iNumCitiesMaintenanceModifier;
	int m_iHomeAreaMaintenanceModifier;
	int m_iOtherAreaMaintenanceModifier;
	int m_iCorporationMaintenanceModifier;
	int m_iExtraHealth;
	int m_iFreeExperience;
	int m_iWorkerSpeedModifier;
	int m_iImprovementUpgradeRateModifier;
	int m_iMilitaryProductionModifier;
	int m_iFreeUnitUpkeepCivilian;
	int m_iFreeUnitUpkeepMilitary;
	int m_iFreeUnitUpkeepCivilianPopPercent;
	int m_iFreeUnitUpkeepMilitaryPopPercent;
	int m_iCivilianUnitUpkeepMod;
	int m_iMilitaryUnitUpkeepMod;
	int m_iHappyPerMilitaryUnit;
	int m_iLargestCityHappiness;
	int m_iWarWearinessModifier;
	int m_iFreeSpecialist;
	int m_iTradeRoutes;
	TechTypes m_iTechPrereq;
	int m_iCivicPercentAnger;
	int m_iMaxConscript;
	int m_iStateReligionHappiness;
	int m_iNonStateReligionHappiness;
	int m_iStateReligionUnitProductionModifier;
	int m_iStateReligionBuildingProductionModifier;
	int m_iStateReligionFreeExperience;
	int m_iExpInBorderModifier;
	int m_iRevIdxLocal;
	int m_iRevIdxNational;
	int m_iRevIdxDistanceModifier;
	int m_iRevIdxHolyCityGood;
	int m_iRevIdxHolyCityBad;
	int m_iRevIdxSwitchTo;
	int m_iRevReligiousFreedom;
	int m_iRevLaborFreedom;
	int m_iRevEnvironmentalProtection;
	int m_iRevDemocracyLevel;
	int m_iNationalCaptureProbabilityModifier;
	int m_iNationalCaptureResistanceModifier;
	int m_iFreedomFighterChange;

	float m_fRevIdxNationalityMod;
	float m_fRevIdxGoodReligionMod;
	float m_fRevIdxBadReligionMod;
	float m_fRevViolentMod;

	bool m_bFixedBorders;
	bool m_bNoCapitalUnhappiness;
	bool m_bNoLandmarkAnger;
	bool m_bUpgradeAnywhere;
	bool m_bAllowInquisitions;
	bool m_bDisallowInquisitions;
	bool m_bCommunism;
	bool m_bFreeSpeech;
	bool m_bCanDoElection;
	bool m_bMilitaryFoodProduction;
	bool m_bNoUnhealthyPopulation;
	bool m_bBuildingOnlyHealthy;
	bool m_bNoForeignTrade;
	bool m_bNoCorporations;
	bool m_bNoForeignCorporations;
	bool m_bStateReligion;
	bool m_bNoNonStateReligionSpread;
	bool m_bAnyImprovementYieldChange;
	bool m_bAllReligionsActive;
	bool m_bBansNonStateReligions;
	bool m_bFreedomFighter;
	bool m_bPolicy;

	int* m_piBonusMintedPercent;
	int* m_piImprovementHappinessChanges;
	int* m_piImprovementHealthPercentChanges;
	int* m_piLandmarkYieldChanges;
	int* m_piFreeSpecialistCount;
	int* m_piUnitProductionModifier;
	int* m_piFlavorValue;
	int* m_piCivicAttitudeChanges;
	int* m_piYieldModifier;
	int* m_piCapitalYieldModifier;
	int* m_piTradeYieldModifier;
	int* m_piCommerceModifier;
	int* m_piCapitalCommerceModifier;
	int* m_piSpecialistExtraCommerce;

	int* m_paiUnitCombatProductionModifier;
	int* m_paiBuildingHappinessChanges;
	int* m_paiBuildingHealthChanges;
	int* m_paiFeatureHappinessChanges;

	bool* m_pabHurry;
	bool* m_pabSpecialBuildingNotRequired;
	bool* m_pabSpecialistValid;

	int** m_ppiSpecialistYieldPercentChanges;
	int** m_ppiSpecialistCommercePercentChanges;
	int** m_ppiTerrainYieldChanges;
	int** m_ppiBuildingCommerceChange;
	int** m_ppiBuildingCommerceModifier;
	int** m_ppiBonusCommerceModifier;
	int** m_ppiImprovementYieldChanges;

	std::vector<int> m_aiCategories;

	IDValueMap<BuildingTypes, int> m_aBuildingProductionModifier;

	// Sparse-view caches paired with the dense arrays above. See the public
    // accessor block for the design rationale and migration template.
    // Fields are mutable so const accessors can populate the cache on demand.
    void cacheSparseListsIfNeeded() const;
    mutable bool m_bSparseListsCached;
    mutable std::vector<BuildingModifier2> m_vBuildingHappinessChangesSparse;
    mutable std::vector<BuildingModifier2> m_vBuildingHealthChangesSparse;
    mutable std::vector<std::pair<FeatureTypes, int> > m_vFeatureHappinessChangesSparse;

	std::vector<CvString> m_aszUnitProdModforPass3;
	std::vector<int> m_aiUnitProdModforPass3;

	std::vector<CvString> m_aszCivicAttitudeforPass3;
	std::vector<int> m_aiCivicAttitudeforPass3;
	std::vector<CvString> m_aszCivicAttitudeReasonforPass3;
	std::vector<CvString> m_aszCivicAttitudeReasonValueforPass3;
};

#endif // CV_CIVIC_INFO_H
