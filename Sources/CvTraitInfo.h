#pragma once

#ifndef Cv_Trait_Info
#define Cv_Trait_Info

#include "CvInfos.h"

class CvTraitInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvTraitInfo();
	virtual ~CvTraitInfo();

	int getHealth() const;
	int getHappiness() const;
	int getMaxAnarchy() const;
	int getUpkeepModifier() const;
	int getLevelExperienceModifier() const;
	int getGreatPeopleRateModifier() const;
	int getGreatGeneralRateModifier() const;
	int getDomesticGreatGeneralRateModifier() const;
	int getMaxGlobalBuildingProductionModifier() const;
	int getMaxTeamBuildingProductionModifier() const;
	int getMaxPlayerBuildingProductionModifier() const;

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		RevTraitEffects															*/
/********************************************************************************/
	int getRevIdxLocal() const;
	int getRevIdxNational() const;
	int getRevIdxDistanceModifier() const;
	int getRevIdxHolyCityGood() const;
	int getRevIdxHolyCityBad() const;
	float getRevIdxNationalityMod() const;
	float getRevIdxBadReligionMod() const;
	float getRevIdxGoodReligionMod() const;
	bool isNonStateReligionCommerce() const;
	bool isUpgradeAnywhere() const;
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

	const char* getShortDescription() const;
	void setShortDescription(const char* szVal);

	// Arrays

	int getExtraYieldThreshold(int i) const;
	int getTradeYieldModifier(int i) const;
	int getCommerceChange(int i) const;
	int getCommerceModifier(int i) const;
/*
	int isFreePromotion(int i) const;
	int isFreePromotionUnitCombat(int i) const;
*/

	//TB Traits Mods begin
	TraitTypes getPrereqTrait() const						{ return m_iPrereqTrait; }
	TraitTypes getPrereqOrTrait1() const					{ return m_iPrereqOrTrait1; }
	TraitTypes getPrereqOrTrait2() const					{ return m_iPrereqOrTrait2; }
	PromotionLineTypes getPromotionLine() const;
	int getGreatPeopleUnitType() const;
	TechTypes getPrereqTech() const							{ return m_ePrereqTech; }
	SpecialistTypes getEraAdvanceFreeSpecialistType() const;
	int getGoldenAgeonBirthofGreatPeopleType() const;
	int getWarWearinessAccumulationModifier() const;
	int getCivicAnarchyTimeModifier() const;
	int getReligiousAnarchyTimeModifier() const;
	int getImprovementUpgradeRateModifier() const;
	int getWorkerSpeedModifier() const;
	int getMaxConscript() const;
	int getDistanceMaintenanceModifier() const;
	int getNumCitiesMaintenanceModifier() const;
	int getCorporationMaintenanceModifier() const;
	int getStateReligionGreatPeopleRateModifier() const;
	int getFreeExperience() const;
	int getFreeUnitUpkeepCivilian() const;
	int getFreeUnitUpkeepMilitary() const;
	int getFreeUnitUpkeepCivilianPopPercent() const;
	int getFreeUnitUpkeepMilitaryPopPercent() const;
	int getCivilianUnitUpkeepMod() const;
	int getMilitaryUnitUpkeepMod() const;
	int getLargestCityHappiness() const;
	int getHappyPerMilitaryUnit() const;
	int getFreeSpecialist() const;
	int getTradeRoutes() const;
	int getStateReligionHappiness() const;
	int getNonStateReligionHappiness() const;
	int getStateReligionUnitProductionModifier() const;
	int getStateReligionBuildingProductionModifier() const;
	int getStateReligionFreeExperience() const;
	int getExpInBorderModifier() const;
	int getCityDefenseBonus() const;
	int getMilitaryProductionModifier() const;
	int getAttitudeModifier() const;
	int getLinePriority() const;
	int getEspionageDefense() const;
	int getMinAnarchy() const;
	int getMaxTradeRoutesChange() const;
	int getGoldenAgeDurationModifier() const;
	int getGreatPeopleRateChange() const;
	int getHurryAngerModifier() const;
	int getHurryCostModifier() const;
	int getEnemyWarWearinessModifier() const;
	int getForeignTradeRouteModifier() const;
	int getBombardDefense() const;
	int getUnitUpgradePriceModifier() const;
	int getCoastalTradeRoutes() const;
	int getGlobalPopulationgrowthratepercentage() const;
	int getCityStartCulture(bool bForLoad = false) const;
	int getGlobalAirUnitCapacity() const;
	int getCapitalXPModifier() const;
	int getHolyCityofStateReligionXPModifier() const;
	int getHolyCityofNonStateReligionXPModifier() const;
	int getBonusPopulationinNewCities() const;
	int getMissileRange() const;
	int getFlightOperationRange() const;
	int getNavalCargoSpace() const;
	int getMissileCargoSpace() const;
	int getNationalCaptureProbabilityModifier() const;
	int getNationalCaptureResistanceModifier() const;
	int getStateReligionSpreadProbabilityModifier() const;
	int getNonStateReligionSpreadProbabilityModifier() const;
	int getFreedomFighterChange() const;

	//Booleans
	bool isMilitaryFoodProduction() const;
	inline bool isNegativeTrait() const { return m_bNegativeTrait; }
	bool isImpurePropertyManipulators() const;
	bool isImpurePromotions() const;
	bool isCivilizationTrait() const;
	bool isAllowsInquisitions() const;
	bool isCoastalAIInfluence() const;
	bool isBarbarianSelectionOnly() const;
	bool isCitiesStartwithStateReligion() const;
	bool isDraftsOnCityCapture() const;
	bool isFreeSpecialistperWorldWonder() const;
	bool isFreeSpecialistperNationalWonder() const;
	bool isFreeSpecialistperTeamProject() const;
	bool isExtraGoody() const;
	bool isAllReligionsActive() const;
	bool isBansNonStateReligions() const;
	bool isFreedomFighter() const;
	bool isValidTrait(bool bGameStart=false) const;

	// bool vector without delayed resolution
	int getNotOnGameOption(int i) const;
	int getNumNotOnGameOptions() const;
	bool isNotOnGameOption(int i) const;

	int getOnGameOption(int i) const;
	int getNumOnGameOptions() const;
	bool isOnGameOption(int i) const;

	int getCategory(int i) const;
	int getNumCategories() const;
	bool isCategory(int i) const;

	//Arrays
	int getSpecialistYieldChange(int i, int j) const;
	int* getSpecialistYieldChangeArray(int i) const;
	bool isAnySpecialistYieldChanges() const { return m_ppaiSpecialistYieldChange != NULL; }

	int getYieldModifier(int i) const;
	int* getYieldModifierArray() const;

	int getCapitalYieldModifier(int i) const;
	int* getCapitalYieldModifierArray() const;

	int getCapitalCommerceModifier(int i) const;
	int* getCapitalCommerceModifierArray() const;

	int getSpecialistExtraCommerce(int i) const;
	int* getSpecialistExtraCommerceArray() const;

	int getSpecialistExtraYield(int i) const;
	int* getSpecialistExtraYieldArray() const;

	int getYieldChange(int i) const;

	int getSpecialistCommerceChange(int i, int j) const;
	int* getSpecialistCommerceChangeArray(int i) const;
	bool isAnySpecialistCommerceChanges() const { return m_ppaiSpecialistCommerceChange != NULL; }

	int getFlavorValue(int i) const;
	int getLessYieldThreshold(int i) const;

	int getSeaPlotYieldChanges(int i) const;
	int* getSeaPlotYieldChangesArray() const;

	int getImprovementYieldChange(int i, int j) const;
	int* getImprovementYieldChangeArray(int i) const;

	int getGoldenAgeYieldChanges(int i) const;
	int* getGoldenAgeYieldChangesArray() const;

	int getGoldenAgeCommerceChanges(int i) const;
	int* getGoldenAgeCommerceChangesArray() const;

	// int vector utilizing struct with delayed resolution

	const IDValueMap<ImprovementTypes, int>::filtered getImprovementUpgradeModifiers() const;
	const IDValueMap<BuildTypes, int>::filtered getBuildWorkerSpeedModifiers() const;

	int getNumDisallowedTraitTypes() const;
	DisallowedTraitType isDisallowedTraitType(int iTrait) const;

	const IDValueMap<DomainTypes, int>::filtered getDomainFreeExperience() const;
	const IDValueMap<DomainTypes, int>::filtered getDomainProductionModifiers() const;
	const IDValueMap<TechTypes, int>::filtered getTechResearchModifiers() const;

	int getNumBuildingProductionModifiers() const;
	BuildingModifier getBuildingProductionModifier(int iBuilding) const;

	const IDValueMap<SpecialBuildingTypes, int>::filtered getSpecialBuildingProductionModifiers() const;

	const IDValueMap<BuildingTypes, int>&			getBuildingHappinessModifiers() const { return m_aBuildingHappinessModifiers; }
	const IDValueMap<BuildingTypes, int>::filtered	getBuildingHappinessModifiersFiltered() const;

	int getNumUnitProductionModifiers() const;
	UnitModifier getUnitProductionModifier(int iUnit) const;

	int getNumSpecialUnitProductionModifiers() const;
	SpecialUnitModifier getSpecialUnitProductionModifier(int iSpecialUnit) const;

	int getNumCivicOptionNoUpkeepTypes() const;
	CivicOptionTypeBool isCivicOptionNoUpkeepType(int iCivicOption) const;

	int getNumUnitCombatFreeExperiences() const;
	UnitCombatModifier getUnitCombatFreeExperience(int iUnitCombat) const;

	int getNumUnitCombatProductionModifiers() const;
	UnitCombatModifier getUnitCombatProductionModifier(int iUnitCombat) const;

	const IDValueMap<BonusTypes, int>::filtered getBonusHappinessChanges() const;

	const CvPropertyManipulators* getPropertyManipulators() const;

	bool isFreePromotionUnitCombats(int i, int j) const;

	void getDataMembers(CvInfoUtil& util);
	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvTraitInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

private:
	CvPropertyManipulators m_PropertyManipulators;

	bool** m_ppbFreePromotionUnitCombats;

	int m_iHealth;
	int m_iHappiness;
	int m_iMaxAnarchy;
	int m_iUpkeepModifier;
	int m_iLevelExperienceModifier;
	int m_iGreatPeopleRateModifier;
	int m_iGreatGeneralRateModifier;
	int m_iDomesticGreatGeneralRateModifier;
	int m_iMaxGlobalBuildingProductionModifier;
	int m_iMaxTeamBuildingProductionModifier;
	int m_iMaxPlayerBuildingProductionModifier;

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		RevTraitEffects															*/
/********************************************************************************/
	int m_iRevIdxLocal;
	int m_iRevIdxNational;
	int m_iRevIdxDistanceModifier;
	int m_iRevIdxHolyCityGood;
	int m_iRevIdxHolyCityBad;
	float m_fRevIdxNationalityMod;
	float m_fRevIdxGoodReligionMod;
	float m_fRevIdxBadReligionMod;
	bool m_bNonStateReligionCommerce;
	bool m_bUpgradeAnywhere;
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

	CvString m_szShortDescription;

	// Arrays

	int* m_paiExtraYieldThreshold;
	int* m_paiTradeYieldModifier;
	int* m_paiCommerceChange;
	int* m_paiCommerceModifier;

//	bool* m_pabFreePromotion;
//	bool* m_pabFreePromotionUnitCombat;

	CvPropertyManipulators m_PropertyManipulatorsNull;

	//TB Traits Mods begin
	//Textual References
	TraitTypes m_iPrereqTrait;
	TraitTypes m_iPrereqOrTrait1;
	TraitTypes m_iPrereqOrTrait2;
	TechTypes m_ePrereqTech;
	PromotionLineTypes m_ePromotionLine;
	int m_iGreatPeopleUnitType;
	SpecialistTypes m_eEraAdvanceFreeSpecialistType;
	int m_iGoldenAgeonBirthofGreatPeopleType;
	int m_iWarWearinessAccumulationModifier;
	int m_iCivicAnarchyTimeModifier;
	int m_iReligiousAnarchyTimeModifier;
	int m_iImprovementUpgradeRateModifier;
	int m_iWorkerSpeedModifier;
	int m_iMaxConscript;
	int m_iDistanceMaintenanceModifier;
	int m_iNumCitiesMaintenanceModifier;
	int m_iCorporationMaintenanceModifier;
	int m_iStateReligionGreatPeopleRateModifier;
	int m_iFreeExperience;
	int m_iFreeUnitUpkeepCivilian;
	int m_iFreeUnitUpkeepMilitary;
	int m_iFreeUnitUpkeepCivilianPopPercent;
	int m_iFreeUnitUpkeepMilitaryPopPercent;
	int m_iCivilianUnitUpkeepMod;
	int m_iMilitaryUnitUpkeepMod;
	int m_iHappyPerMilitaryUnit;
	int m_iLargestCityHappiness;
	int m_iFreeSpecialist;
	int m_iTradeRoutes;
	int m_iStateReligionHappiness;
	int m_iNonStateReligionHappiness;
	int m_iStateReligionUnitProductionModifier;
	int m_iStateReligionBuildingProductionModifier;
	int m_iStateReligionFreeExperience;
	int m_iExpInBorderModifier;
	int m_iCityDefenseBonus;
	int m_iMilitaryProductionModifier;
	int m_iAttitudeModifier;
	int m_iLinePriority;
	int m_iEspionageDefense;
	int m_iMinAnarchy;
	int m_iMaxTradeRoutesChange;
	int m_iGoldenAgeDurationModifier;
	int m_iGreatPeopleRateChange;
	int m_iHurryAngerModifier;
	int m_iHurryCostModifier;
	int m_iEnemyWarWearinessModifier;
	int m_iForeignTradeRouteModifier;
	int m_iBombardDefense;
	int m_iUnitUpgradePriceModifier;
	int m_iCoastalTradeRoutes;
	int m_iGlobalPopulationgrowthratepercentage;
	int m_iCityStartCulture;
	int m_iGlobalAirUnitCapacity;
	int m_iCapitalXPModifier;
	int m_iHolyCityofStateReligionXPModifier;
	int m_iHolyCityofNonStateReligionXPModifier;
	int m_iBonusPopulationinNewCities;
	int m_iMissileRange;
	int m_iFlightOperationRange;
	int m_iNavalCargoSpace;
	int m_iMissileCargoSpace;
	int m_iNationalCaptureProbabilityModifier;
	int m_iNationalCaptureResistanceModifier;
	int m_iStateReligionSpreadProbabilityModifier;
	int m_iNonStateReligionSpreadProbabilityModifier;
	int m_iFreedomFighterChange;
	//booleans
	bool m_bMilitaryFoodProduction;
	bool m_bNegativeTrait;
	bool m_bImpurePropertyManipulators;
	bool m_bImpurePromotions;
	bool m_bCivilizationTrait;
	bool m_bAllowsInquisitions;
	bool m_bCoastalAIInfluence;
	bool m_bBarbarianSelectionOnly;
	bool m_bCitiesStartwithStateReligion;
	bool m_bDraftsOnCityCapture;
	bool m_bFreeSpecialistperWorldWonder;
	bool m_bFreeSpecialistperNationalWonder;
	bool m_bFreeSpecialistperTeamProject;
	bool m_bExtraGoody;
	bool m_bAllReligionsActive;
	bool m_bBansNonStateReligions;
	bool m_bFreedomFighter;
	// bool vector without delayed resolution
	std::vector<int> m_aiNotOnGameOptions;
	std::vector<int> m_aiOnGameOptions;
	//Arrays
	int** m_ppaiSpecialistYieldChange;
	int* m_piYieldModifier;
	int* m_piCapitalYieldModifier;
	int* m_piCapitalCommerceModifier;
	int* m_piSpecialistExtraCommerce;
	int* m_piSpecialistExtraYield;
	int* m_piYieldChange;
	int** m_ppaiSpecialistCommerceChange;
	int* m_piFlavorValue;
	int* m_paiLessYieldThreshold;
	int* m_piSeaPlotYieldChanges;
	int** m_ppaiImprovementYieldChange;
	int* m_piGoldenAgeYieldChanges;
	int* m_piGoldenAgeCommerceChanges;
	//Arrays for Pure Traits
	int* m_piBonusHappinessChangesFiltered;
	int* m_paiExtraYieldThresholdFiltered;
	int* m_paiTradeYieldModifierFiltered;
	int* m_paiCommerceChangeFiltered;
	int* m_paiCommerceModifierFiltered;
	int** m_ppaiSpecialistYieldChangeFiltered;
	int* m_piYieldModifierFiltered;
	int* m_piCapitalYieldModifierFiltered;
	int* m_piCapitalCommerceModifierFiltered;
	int* m_piSpecialistExtraCommerceFiltered;
	int* m_piSpecialistExtraYieldFiltered;
	int* m_piYieldChangeFiltered;
	int** m_ppaiSpecialistCommerceChangeFiltered;
	int* m_paiLessYieldThresholdFiltered;
	int* m_piSeaPlotYieldChangesFiltered;
	int** m_ppaiImprovementYieldChangeFiltered;
	int* m_piGoldenAgeYieldChangesFiltered;
	int* m_piGoldenAgeCommerceChangesFiltered;
	// bool vector without delayed resolution
	std::vector<int> m_aiCategories;
	// int vector utilizing struct with delayed resolution
	IDValueMap<ImprovementTypes, int> m_aImprovementUpgradeModifierTypes;
	IDValueMap<BuildTypes, int> m_aBuildWorkerSpeedModifierTypes;
	std::vector<DisallowedTraitType> m_aDisallowedTraitTypes;
	IDValueMap<DomainTypes, int> m_aDomainFreeExperiences;
	IDValueMap<DomainTypes, int> m_aDomainProductionModifiers;
	IDValueMap<TechTypes, int> m_aTechResearchModifiers;
	std::vector<BuildingModifier> m_aBuildingProductionModifiers;
	IDValueMap<SpecialBuildingTypes, int> m_aSpecialBuildingProductionModifiers;
	IDValueMap<BuildingTypes, int> m_aBuildingHappinessModifiers;
	std::vector<UnitModifier> m_aUnitProductionModifiers;
	std::vector<SpecialUnitModifier> m_aSpecialUnitProductionModifiers;
	std::vector<CivicOptionTypeBool> m_aCivicOptionNoUpkeepTypes;
	std::vector<UnitCombatModifier> m_aUnitCombatFreeExperiences;
	std::vector<UnitCombatModifier> m_aUnitCombatProductionModifiers;
	IDValueMap<BonusTypes, int> m_aBonusHappinessChanges;
};

#endif