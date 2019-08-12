#ifndef CV_INFO_PLAYER_H
#define CV_INFO_PLAYER_H

class CvDiplomacyInfo;
class CvDiplomacyResponse
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvDiplomacyResponse();
	virtual ~CvDiplomacyResponse();

	int getNumDiplomacyText();
	void setNumDiplomacyText(int i);

	bool getCivilizationTypes(int i);
	bool* getCivilizationTypes() const;
	void setCivilizationTypes(int i, bool bVal);

	bool getLeaderHeadTypes(int i);
	bool* getLeaderHeadTypes() const;
	void setLeaderHeadTypes(int i, bool bVal);

	bool getAttitudeTypes(int i) const;
	bool* getAttitudeTypes() const;
	void setAttitudeTypes(int i, bool bVal);

	bool getDiplomacyPowerTypes(int i);
	bool* getDiplomacyPowerTypes() const;
	void setDiplomacyPowerTypes(int i, bool bVal);

	const TCHAR* getDiplomacyText(int i) const;
	const CvString* getDiplomacyText() const;
	void setDiplomacyText(int i, CvString szText);

	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}
	bool read(CvXMLLoadUtility* pXML);
	void UpdateDiplomacies(CvDiplomacyInfo* pDiplomacyInfo, int iIndex);

protected:
	int m_iNumDiplomacyText;
	bool* m_pbCivilizationTypes;
	bool* m_pbLeaderHeadTypes;
	bool* m_pbAttitudeTypes;
	bool* m_pbDiplomacyPowerTypes;
	CvString* m_paszDiplomacyText;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvTechInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTechInfo : public CvInfoBase
{

friend class CvXMLLoadUtility;

//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvTechInfo();
	virtual ~CvTechInfo();

	//	Validate tech info da after load (debug only)
	static void validate();

	int getAdvisorType() const;					// Exposed to Python
	int getAIWeight() const;					// Exposed to Python
	int getAITradeModifier() const;				// Exposed to Python
	int getResearchCost() const;				// Exposed to Python
	int getAdvancedStartCost() const;			// Exposed to Python
	int getAdvancedStartCostIncrease() const;	// Exposed to Python
	int getEra() const;							// Exposed to Python
	int getTradeRoutes() const;					// Exposed to Python
	int getFeatureProductionModifier() const;	// Exposed to Python
	int getWorkerSpeedModifier() const;			// Exposed to Python
	//DPII < Maintenance Modifier >
	int getMaintenanceModifier() const;
	int getDistanceMaintenanceModifier() const;
	int getNumCitiesMaintenanceModifier() const;
	int getCoastalDistanceMaintenanceModifier() const;
	//DPII < Maintenance Modifier >
	int getFirstFreeUnitClass() const;			// Exposed to Python
	int getFirstFreeProphet() const;			// Exposed to Python
	int getHealth() const;						// Exposed to Python
	int getHappiness() const;					// Exposed to Python
	int getFirstFreeTechs() const;				// Exposed to Python
	int getAssetValue(bool bForLoad = false) const;	// Exposed to Python
	int getPowerValue(bool bForLoad = false) const;	// Exposed to Python

	int getGridX() const;						// Exposed to Python
	int getGridY() const;						// Exposed to Python

	bool isRepeat() const;						// Exposed to Python
	bool isTrade() const;						// Exposed to Python
	bool isDisable() const;						// Exposed to Python
	bool isGoodyTech() const;					// Exposed to Python
	bool isExtraWaterSeeFrom() const;			// Exposed to Python
	bool isMapCentering() const;				// Exposed to Python
	bool isMapVisible() const;					// Exposed to Python
	bool isMapTrading() const;					// Exposed to Python
	bool isTechTrading() const;					// Exposed to Python
	bool isGoldTrading() const;					// Exposed to Python
	bool isOpenBordersTrading() const;			// Exposed to Python
	bool isDefensivePactTrading() const;		// Exposed to Python
	bool isPermanentAllianceTrading() const;	// Exposed to Python
	bool isVassalStateTrading() const;			// Exposed to Python
	bool isBridgeBuilding() const;				// Exposed to Python
	bool isIrrigation() const;					// Exposed to Python
	bool isIgnoreIrrigation() const;			// Exposed to Python
	bool isWaterWork() const;					// Exposed to Python
	bool isRiverTrade() const;					// Exposed to Python

/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
	bool isCanPassPeaks() const;
	bool isMoveFastPeaks() const;
	bool isCanFoundOnPeaks() const;
	bool isEmbassyTrading() const;
	bool isEnableDarkAges() const;
	bool isRebaseAnywhere() const;
	bool isEnablesDesertFarming() const;
	int getInflationModifier() const;
	int getGlobalTradeModifier() const;
	int getGlobalForeignTradeModifier() const;
	int getTradeMissionModifier() const;
	int getCorporationRevenueModifier() const;
	int getCorporationMaintenanceModifier() const;
	int getFreeSpecialistCount(int i) const;
	int getPrereqGameOption() const;

	int getNumPrereqBuildingClasses() const;
	PrereqBuildingClass& getPrereqBuildingClass(int iIndex);
	int getPrereqBuildingClassType(int iIndex);
	int getPrereqBuildingClassMinimumRequired(int iIndex);

	int getNumPrereqOrBuildingClasses() const;
	PrereqBuildingClass& getPrereqOrBuildingClass(int iIndex);
	int getPrereqOrBuildingClassType(int iIndex);
	int getPrereqOrBuildingClassMinimumRequired(int iIndex);

	//int getPrereqBuildingClass(int i) const;
	//int getPrereqBuildingVectorSize();
	//CvString getPrereqBuildingNamesVectorElement(int i);
	//int getPrereqBuildingClassValuesVectorElement(int i);
	//
	//int getPrereqOrBuildingClass(int i) const;
	//int getPrereqOrBuildingVectorSize();
	//CvString getPrereqOrBuildingNamesVectorElement(int i);
	//int getPrereqOrBuildingClassValuesVectorElement(int i);

	bool readPass3();

	void setPrereqOrTech(int i, int iTech);
	void setPrereqAndTech(int i, int iTech);
	void setGridX(int i);

	void setOriginalPrereqOrTech(int i, int iTech);
	void setOriginalPrereqAndTech(int i, int iTech);
	int getOriginalPrereqOrTechs(int i) const;
	int getOriginalPrereqAndTechs(int i) const;
	int getUnitClassStrengthChange(int iUnit, bool bForLoad = false) const;
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	//TB Tech Tags
	bool isGlobal() const;
	//TB Tech Tags end


/************************************************************************************************/
/* DCM									 04/19/09								Johny Smith  */
/************************************************************************************************/
	// Dale - AB: Bombing START
	bool getDCMAirBombTech1() const;
	bool getDCMAirBombTech2() const;
protected:
	bool m_bDCMAirBombTech1;
	bool m_bDCMAirBombTech2;
public:
	// Dale - AB: Bombing END
/************************************************************************************************/
/* DCM									 END												  */
/************************************************************************************************/
	std::wstring getQuote() const;				// Exposed to Python
	void setQuoteKey(const TCHAR* szVal);
	const TCHAR* getQuoteKey();
	const TCHAR* getSound() const;				// Exposed to Python
	void setSound(const TCHAR* szVal);
	const TCHAR* getSoundMP() const;			// Exposed to Python
	void setSoundMP(const TCHAR* szVal);

	// Arrays

	int getDomainExtraMoves(int i) const;		// Exposed to Python
	int getFlavorValue(int i) const;			// Exposed to Python
	int getPrereqOrTechs(int i) const;			// Exposed to Python
	int getPrereqAndTechs(int i) const;			// Exposed to Python

	bool isCommerceFlexible(int i) const;		// Exposed to Python
	bool isTerrainTrade(int i) const;			// Exposed to Python
	//ls612: Tech Commerce Modifiers
	int getCommerceModifier(int i) const;
	int* getCommerceModifierArray() const;

	void read(FDataStreamBase* ) {}
	void write(FDataStreamBase* ) {}

	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvTechInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void copyNonDefaultsReadPass2(CvTechInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL, bool bOver = false);

	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iAdvisorType;
	int m_iAIWeight;
	int m_iAITradeModifier;
	int m_iResearchCost;
	int m_iAdvancedStartCost;
	int m_iAdvancedStartCostIncrease;
	int m_iEra;
	int m_iTradeRoutes;
	int m_iFeatureProductionModifier;
	int m_iWorkerSpeedModifier;
	//DPII < Maintenance Modifier >
	int m_iMaintenanceModifier;
	int m_iDistanceMaintenanceModifier;
	int m_iNumCitiesMaintenanceModifier;
	int m_iCoastalDistanceMaintenanceModifier;
	//DPII < Maintenance Modifier >
	int m_iFirstFreeUnitClass;
	int m_iFirstFreeProphet;
	int m_iHealth;
	int m_iHappiness;
	int m_iFirstFreeTechs;
	int m_iAssetValue;
	int m_iPowerValue;

	int m_iGridX;
	int m_iGridY;

	bool m_bRepeat;
	bool m_bTrade;
	bool m_bDisable;
	bool m_bGoodyTech;
	bool m_bExtraWaterSeeFrom;
	bool m_bMapCentering;
	bool m_bMapVisible;
	bool m_bMapTrading;
	bool m_bTechTrading;
	bool m_bGoldTrading;
	bool m_bOpenBordersTrading;
	bool m_bDefensivePactTrading;
	bool m_bPermanentAllianceTrading;
	bool m_bVassalStateTrading;
	bool m_bBridgeBuilding;
	bool m_bIrrigation;
	bool m_bIgnoreIrrigation;
	bool m_bWaterWork;
	bool m_bRiverTrade;

	CvString m_szQuoteKey;
	CvString m_szSound;
	CvString m_szSoundMP;

	// Arrays

	int* m_piDomainExtraMoves;
	int* m_piFlavorValue;

	int* m_piPrereqOrTechs;
	int* m_piPrereqAndTechs;

	bool* m_pbCommerceFlexible;
	bool* m_pbTerrainTrade;
	//ls612: Tech Commerce Modifiers
	int* m_piCommerceModifier;

/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
	//TB Tech Tags
	bool m_bGlobal;
	//TB Tech Tags end
	bool m_bCanPassPeaks;
	bool m_bMoveFastPeaks;
	bool m_bCanFoundOnPeaks;
	bool m_bEmbassyTrading;
	bool m_bEnableDarkAges;
	bool m_bRebaseAnywhere;
	bool m_bEnablesDesertFarming;

	int m_iInflationModifier;
	int m_iGlobalTradeModifier;
	int m_iGlobalForeignTradeModifier;
	int m_iTradeMissionModifier;
	int m_iCorporationRevenueModifier;
	int m_iCorporationMaintenanceModifier;
	int m_iPrereqGameOption;
	int* m_piFreeSpecialistCount;

	int* m_piOriginalPrereqOrTechs;
	int* m_piOriginalPrereqAndTechs;

	int* m_piUnitClassStrengthChange;

	std::vector<PrereqBuildingClass> m_aPrereqBuildingClass;
	std::vector<PrereqBuildingClass> m_aPrereqOrBuildingClass;

	//int* m_paiPrereqBuildingClass;
	//std::vector<CvString> m_aszPrereqBuildingClassforPass3;
	//std::vector<int> m_aiPrereqBuildingClassforPass3;
	//
	//int* m_paiPrereqOrBuildingClass;
	//std::vector<CvString> m_aszPrereqOrBuildingClassforPass3;
	//std::vector<int> m_aiPrereqOrBuildingClassforPass3;
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCivicOptionInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCivicOptionInfo :
	public CvInfoBase
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvCivicOptionInfo();
	virtual ~CvCivicOptionInfo();

/*	bool getTraitNoUpkeep(int i) const;	*/			// Exposed to Python
	bool isPolicy() const;

	bool read(CvXMLLoadUtility* pXML);
/************************************************************************************************/
/* XMLCOPY								 10/22/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	void copyNonDefaults(CvCivicOptionInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	bool* m_pabTraitNoUpkeep;
	bool m_bPolicy;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCivicInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCivicInfo :
	public CvInfoBase
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvCivicInfo();
	virtual ~CvCivicInfo();

	std::wstring pyGetWeLoveTheKing() { return getWeLoveTheKing(); }// Exposed to Python
	const wchar* getWeLoveTheKing();
	void setWeLoveTheKingKey(const TCHAR* szVal);
	const wchar* getWeLoveTheKingKey();

	int getCivicOptionType() const;						// Exposed to Python
	int getAnarchyLength() const;						// Exposed to Python
	int getUpkeep() const;								// Exposed to Python
	int getAIWeight() const;							// Exposed to Python
	int getGreatPeopleRateModifier() const;				// Exposed to Python
	int getGreatGeneralRateModifier() const;			// Exposed to Python
	int getDomesticGreatGeneralRateModifier() const;	// Exposed to Python
	int getStateReligionGreatPeopleRateModifier() const;// Exposed to Python
	int getDistanceMaintenanceModifier() const;			// Exposed to Python
	int getNumCitiesMaintenanceModifier() const;		// Exposed to Python
		int getHomeAreaMaintenanceModifier() const;
		int getOtherAreaMaintenanceModifier() const;
	int getCorporationMaintenanceModifier() const;		// Exposed to Python
	int getExtraHealth() const;							// Exposed to Python
	int getFreeExperience() const;						// Exposed to Python
	int getWorkerSpeedModifier() const;					// Exposed to Python
	int getImprovementUpgradeRateModifier() const;		// Exposed to Python
	int getMilitaryProductionModifier() const;			// Exposed to Python
	int getBaseFreeUnits() const;						// Exposed to Python
	int getBaseFreeMilitaryUnits() const;				// Exposed to Python
	int getFreeUnitsPopulationPercent() const;			// Exposed to Python
	int getFreeMilitaryUnitsPopulationPercent() const;	// Exposed to Python
	int getGoldPerUnit() const;							// Exposed to Python
	int getGoldPerMilitaryUnit() const;					// Exposed to Python
	int getHappyPerMilitaryUnit() const;				// Exposed to Python
	int getLargestCityHappiness() const;				// Exposed to Python
	int getWarWearinessModifier() const;				// Exposed to Python
	int getFreeSpecialist() const;						// Exposed to Python
	int getTradeRoutes() const;							// Exposed to Python
	int getTechPrereq() const;							// Exposed to Python
	int getCivicPercentAnger() const;					// Exposed to Python
	int getMaxConscript() const;						// Exposed to Python
	int getStateReligionHappiness() const;				// Exposed to Python
	int getNonStateReligionHappiness() const;			// Exposed to Python
	int getStateReligionUnitProductionModifier() const;	// Exposed to Python
	int getStateReligionBuildingProductionModifier() const;	// Exposed to Python
	int getStateReligionFreeExperience() const;			// Exposed to Python
	int getExpInBorderModifier() const;					// Exposed to Python
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
	int getRealCorporationMaintenanceModifier() const;
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
	bool isMilitaryFoodProduction() const;			// Exposed to Python
	bool isNoUnhealthyPopulation() const;			// Exposed to Python
	bool isBuildingOnlyHealthy() const;				// Exposed to Python
	bool isNoForeignTrade() const;					// Exposed to Python
	bool isNoCorporations() const;					// Exposed to Python
	bool isNoForeignCorporations() const;			// Exposed to Python
	bool isStateReligion() const;					// Exposed to Python
	bool isNoNonStateReligionSpread() const;		// Exposed to Python
	bool IsFixedBorders() const;
	bool isNoCapitalUnhappiness() const;
	bool isTaxationAnger() const;
	bool isNoLandmarkAnger() const;
	bool isEnablesMAD() const;
	bool isAllReligionsActive() const;
	bool isBansNonStateReligions() const;
	bool isFreedomFighter() const;
	bool isPolicy() const;

	// Arrays

	int getYieldModifier(int i) const;				// Exposed to Python
	int* getYieldModifierArray() const;
	int getCapitalYieldModifier(int i) const;		// Exposed to Python
	int* getCapitalYieldModifierArray() const;
	int getTradeYieldModifier(int i) const;			// Exposed to Python
	int* getTradeYieldModifierArray() const;
	int getCommerceModifier(int i) const;			// Exposed to Python
	int* getCommerceModifierArray() const;
	int getCapitalCommerceModifier(int i) const;	// Exposed to Python
	int* getCapitalCommerceModifierArray() const;
	int getSpecialistExtraCommerce(int i) const;	// Exposed to Python
	int* getSpecialistExtraCommerceArray() const;
	int getCivicAttitudeChange(int i) const;
	int* getCivicAttitudeChanges() const;
	int getLandmarkYieldChanges(int i) const;
	int* getLandmarkYieldChangesArray() const;

	int getBonusCommerceModifier(int i, int j) const;
	int* getBonusCommerceModifierArray(int i) const;

	int getBuildingHappinessChanges(int i) const;	// Exposed to Python
	int getBuildingHealthChanges(int i) const;		// Exposed to Python
	int getFeatureHappinessChanges(int i) const;
	int getBonusMintedPercent(int i) const;
	int getFreeSpecialistCount(int i) const;		// Exposed to Python
	int getFlavorValue(int i) const;
	int getUnitCombatProductionModifier(int i) const;
	int getUnitClassProductionModifier(int i) const;
	int getBuildingClassProductionModifier(int i) const;
	int getImprovementHappinessChanges(int i) const;
	int getImprovementHealthPercentChanges(int i) const;

	bool isHurry(int i) const;						// Exposed to Python
	bool isSpecialBuildingNotRequired(int i) const;	// Exposed to Python
	bool isSpecialistValid(int i) const;			// Exposed to Python

	int getImprovementYieldChanges(int i, int j) const;	// Exposed to Python
	int getTerrainYieldChanges(int i, int j) const;
	int getBuildingCommerceModifier(int i, int j) const;
	int getBuildingClassCommerceChange(int i, int j) const;
	int getSpecialistYieldPercentChanges(int i, int j) const;
	int getSpecialistCommercePercentChanges(int i, int j) const;

	int getCityLimit(PlayerTypes ePlayer) const;

	bool isAnyBuildingHappinessChange() const;
	bool isAnyBuildingHealthChange() const;
	bool isAnyFeatureHappinessChange() const;
	bool isAnySpecialistValid() const;
	bool isAnyImprovementYieldChange() const;
	bool isAnySpecialistCommerceChanges() const;
	bool isAnySpecialistYieldChanges() const;

	CvString getCivicAttitudeReason(int i) const;
	int getCivicAttitudeVectorSize();
	CvString getCivicAttitudeNamesVectorElement(int i);
	int getCivicAttitudeValuesVectorElement(int i);
	int getCivicAttitudeReasonVectorSize();
	CvString getCivicAttitudeReasonNamesVectorElement(int i);
	CvString getCivicAttitudeReasonValuesVectorElement(int i);

	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}
	bool readPass3();
	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvCivicInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);
	CvPropertyManipulators* getPropertyManipulators();

//----------------------PROTECTED MEMBER VARIABLES----------------------------
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
	int m_iRealCorporationMaintenanceModifier;
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
	int m_iBaseFreeUnits;
	int m_iBaseFreeMilitaryUnits;
	int m_iFreeUnitsPopulationPercent;
	int m_iFreeMilitaryUnitsPopulationPercent;
	int m_iGoldPerUnit;
	int m_iGoldPerMilitaryUnit;
	int m_iHappyPerMilitaryUnit;
	int m_iLargestCityHappiness;
	int m_iWarWearinessModifier;
	int m_iFreeSpecialist;
	int m_iTradeRoutes;
	int m_iTechPrereq;
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
	bool m_bTaxationAnger;
	bool m_bNoLandmarkAnger;
	bool m_bEnablesMAD;
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
	bool m_bAnySpecialistCommerceChanges;
	bool m_bAnySpecialistYieldChanges;
	bool m_bAllReligionsActive;
	bool m_bBansNonStateReligions;
	bool m_bFreedomFighter;
	bool m_bPolicy;

	int* m_piBonusMintedPercent;
	int* m_piImprovementHappinessChanges;
	int* m_piImprovementHealthPercentChanges;
	int* m_piLandmarkYieldChanges;
	int* m_piFreeSpecialistCount;
	int* m_piUnitClassProductionModifier;
	int* m_piFlavorValue;
	int* m_piCivicAttitudeChanges;
	int* m_piYieldModifier;
	int* m_piCapitalYieldModifier;
	int* m_piTradeYieldModifier;
	int* m_piCommerceModifier;
	int* m_piCapitalCommerceModifier;
	int* m_piSpecialistExtraCommerce;

	int* m_paiUnitCombatProductionModifier;
	int* m_paiBuildingClassProductionModifier;
	int* m_paiBuildingHappinessChanges;
	int* m_paiBuildingHealthChanges;
	int* m_paiFeatureHappinessChanges;

	bool* m_pabHurry;
	bool* m_pabSpecialBuildingNotRequired;
	bool* m_pabSpecialistValid;

	int** m_ppiSpecialistYieldPercentChanges;
	int** m_ppiSpecialistCommercePercentChanges;
	int** m_ppiTerrainYieldChanges;
	int** m_ppiBuildingClassCommerceChange;
	int** m_ppiBuildingCommerceModifier;
	int** m_ppiBonusCommerceModifier;
	int** m_ppiImprovementYieldChanges;

	std::vector<CvString> m_aszCivicAttitudeforPass3;
	std::vector<int> m_aiCivicAttitudeforPass3;
	std::vector<CvString> m_aszCivicAttitudeReasonforPass3;
	std::vector<CvString> m_aszCivicAttitudeReasonValueforPass3;

	CvPropertyManipulators m_PropertyManipulators;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvDiplomacyInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvDiplomacyInfo :
	public CvInfoBase
{

	friend class CvXMLLoadUtility;		// so it can access private vars to initialize the class
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvDiplomacyInfo();
	virtual ~CvDiplomacyInfo();

	void uninit();

	const CvDiplomacyResponse& getResponse(int iNum) const;	// Exposed to Python
	int getNumResponses() const;							// Exposed to Python

	bool getCivilizationTypes(int i, int j) const;			// Exposed to Python
	bool getLeaderHeadTypes(int i, int j) const;			// Exposed to Python
	bool getAttitudeTypes(int i, int j) const;				// Exposed to Python
	bool getDiplomacyPowerTypes(int i, int j) const;		// Exposed to Python

	int getNumDiplomacyText(int i) const;					// Exposed to Python

	const TCHAR* getDiplomacyText(int i, int j) const;		// Exposed to Python

	void read(FDataStreamBase* stream){}
	void write(FDataStreamBase* stream){}
	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvXMLLoadUtility* pXML);
	bool FindResponseIndex(CvDiplomacyResponse* pNewResponse, int iCase, int* iIndex);

private:
	std::vector<CvDiplomacyResponse*> m_pResponses;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCivilizationInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoCivilization;
class CvCivilizationInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvCivilizationInfo();
	virtual ~CvCivilizationInfo();
	virtual void reset();

	DllExport int getDefaultPlayerColor() const;		// Exposed to Python
	int getArtStyleType() const;						// Exposed to Python
	int getUnitArtStyleType() const;					// Exposed to Python
	int getNumCityNames() const;						// Exposed to Python
	// the number of leaders the Civ has, this is needed so that random leaders can be generated easily
	int getNumLeaders() const;							// Exposed to Python
	int getSelectionSoundScriptId() const;				// Exposed to Python
	int getActionSoundScriptId() const;					// Exposed to Python

	DllExport bool isAIPlayable() const;				// Exposed to Python
	DllExport bool isPlayable() const;					// Exposed to Python

	std::wstring pyGetShortDescription(uint uiForm) { return getShortDescription(uiForm); }	// Exposed to Python
	DllExport const wchar* getShortDescription(uint uiForm = 0);
	const wchar* getShortDescriptionKey() const;		// Exposed to Python
	std::wstring pyGetShortDescriptionKey() { return getShortDescriptionKey(); }// Exposed to Python

	std::wstring pyGetAdjective(uint uiForm) { return getAdjective(uiForm);  }	// Exposed to Python
	DllExport const wchar* getAdjective(uint uiForm = 0);
	const wchar* getAdjectiveKey() const;				// Exposed to Python
	std::wstring pyGetAdjectiveKey() { return getAdjectiveKey(); }				// Exposed to Python

	DllExport const TCHAR* getFlagTexture() const;
	const TCHAR* getArtDefineTag() const;
	void setArtDefineTag(const TCHAR* szVal);
	// Arrays

	int getCivilizationBuildings(int i) const;			// Exposed to Python
	int getCivilizationUnits(int i) const;				// Exposed to Python
	int getCivilizationFreeUnitsClass(int i) const;		// Exposed to Python
	int getCivilizationInitialCivics(int i) const;		// Exposed to Python
	// Afforess 04/05/10
	void setCivilizationInitialCivics(int iCivicOption, int iCivic);

	DllExport bool isLeaders(int i) const;				// Exposed to Python
	bool isCivilizationFreeBuildingClass(int i) const;	// Exposed to Python
	bool isCivilizationFreeTechs(int i) const;			// Exposed to Python
	bool isCivilizationDisableTechs(int i) const;		// Exposed to Python

	std::string getCityNames(int i) const;				// Exposed to Python

	const CvArtInfoCivilization* getArtInfo() const;
	const TCHAR* getButton() const;

	int getDerivativeCiv() const;						// Exposed to Python
	void setDerivativeCiv(int iCiv);

	//TB Tags
	//int
	int getSpawnRateModifier() const;
	int getSpawnRateNPCPeaceModifier() const;
	// Means that the civilization cannot build or train anything which is not specified
	// as allowed in Unit or Building Info by the EnabledCivilization tag.  Generally used for NPC players.
	bool isStronglyRestricted() const;

	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvCivilizationInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void copyNonDefaultsReadPass2(CvCivilizationInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL, bool bOver = false);
	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}

	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iDefaultPlayerColor;
	int m_iArtStyleType;
	// FlavorUnits by Impaler[WrG]
	int m_iUnitArtStyleType;

	int m_iNumCityNames;
	int m_iNumLeaders;
	int m_iSelectionSoundScriptId;
	int m_iActionSoundScriptId;
	int m_iDerivativeCiv;

	bool m_bAIPlayable;
	bool m_bPlayable;

	CvString m_szArtDefineTag;
	CvWString m_szShortDescriptionKey;
	CvWString m_szAdjectiveKey;
	// Arrays

	int* m_piCivilizationBuildings;
	int* m_piCivilizationUnits;
	int* m_piCivilizationFreeUnitsClass;
	int* m_piCivilizationInitialCivics;

	bool* m_pbLeaders;
	bool* m_pbCivilizationFreeBuildingClass;
	bool* m_pbCivilizationFreeTechs;
	bool* m_pbCivilizationDisableTechs;

	CvString* m_paszCityNames;

	mutable std::vector<CvWString> m_aszShortDescription;
	mutable std::vector<CvWString> m_aszAdjective;

	//TB Tags
	//int
	int m_iSpawnRateModifier;
	int m_iSpawnRateNPCPeaceModifier;
	//bool
	bool m_bStronglyRestricted;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvLeaderHeadInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoLeaderhead;
class CvLeaderHeadInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvLeaderHeadInfo();
	virtual ~CvLeaderHeadInfo();

	bool isNPC() const;
	int getWonderConstructRand() const;			// Exposed to Python
	int getBaseAttitude() const;				// Exposed to Python
	int getBasePeaceWeight() const;				// Exposed to Python
	int getPeaceWeightRand() const;				// Exposed to Python
	int getWarmongerRespect() const;			// Exposed to Python
	int getEspionageWeight() const;				// Exposed to Python
	int getRefuseToTalkWarThreshold() const;	// Exposed to Python
	int getNoTechTradeThreshold() const;		// Exposed to Python
	int getTechTradeKnownPercent() const;		// Exposed to Python
	int getMaxGoldTradePercent() const;			// Exposed to Python
	int getMaxGoldPerTurnTradePercent() const;	// Exposed to Python
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					  03/21/10								jdog5000	  */
/*																							  */
/* Victory Strategy AI																		  */
/************************************************************************************************/
	int getCultureVictoryWeight() const;
	int getSpaceVictoryWeight() const;
	int getConquestVictoryWeight() const;
	int getDominationVictoryWeight() const;
	int getDiplomacyVictoryWeight() const;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					   END												  */
/************************************************************************************************/
	int getMaxWarRand() const;						// Exposed to Python
	int getMaxWarNearbyPowerRatio() const;			// Exposed to Python
	int getMaxWarDistantPowerRatio() const;			// Exposed to Python
	int getMaxWarMinAdjacentLandPercent() const;	// Exposed to Python
	int getLimitedWarRand() const;					// Exposed to Python
	int getLimitedWarPowerRatio() const;			// Exposed to Python
	int getDogpileWarRand() const;					// Exposed to Python
	int getMakePeaceRand() const;					// Exposed to Python
	int getDeclareWarTradeRand() const;				// Exposed to Python
	int getDemandRebukedSneakProb() const;			// Exposed to Python
	int getDemandRebukedWarProb() const;			// Exposed to Python
	int getRazeCityProb() const;					// Exposed to Python
	int getBuildUnitProb() const;					// Exposed to Python
	int getBaseAttackOddsChange() const;			// Exposed to Python
	int getAttackOddsChangeRand() const;			// Exposed to Python
	int getWorseRankDifferenceAttitudeChange() const;	// Exposed to Python
	int getBetterRankDifferenceAttitudeChange() const;	// Exposed to Python
	int getCloseBordersAttitudeChange() const;		// Exposed to Python
	int getLostWarAttitudeChange() const;			// Exposed to Python
	int getAtWarAttitudeDivisor() const;			// Exposed to Python
	int getAtWarAttitudeChangeLimit() const;		// Exposed to Python
	int getAtPeaceAttitudeDivisor() const;			// Exposed to Python
	int getAtPeaceAttitudeChangeLimit() const;		// Exposed to Python
	int getSameReligionAttitudeChange() const;		// Exposed to Python
	int getSameReligionAttitudeDivisor() const;		// Exposed to Python
	int getSameReligionAttitudeChangeLimit() const;		// Exposed to Python
	int getDifferentReligionAttitudeChange() const;		// Exposed to Python
	int getDifferentReligionAttitudeDivisor() const;	// Exposed to Python
	int getDifferentReligionAttitudeChangeLimit() const;// Exposed to Python
	int getBonusTradeAttitudeDivisor() const;			// Exposed to Python
	int getBonusTradeAttitudeChangeLimit() const;		// Exposed to Python
	int getOpenBordersAttitudeDivisor() const;			// Exposed to Python
	int getOpenBordersAttitudeChangeLimit() const;		// Exposed to Python
	int getDefensivePactAttitudeDivisor() const;		// Exposed to Python
	int getDefensivePactAttitudeChangeLimit() const;	// Exposed to Python
	int getShareWarAttitudeChange() const;				// Exposed to Python
	int getShareWarAttitudeDivisor() const;				// Exposed to Python
	int getShareWarAttitudeChangeLimit() const;			// Exposed to Python
	int getFavoriteCivicAttitudeChange() const;			// Exposed to Python
	int getFavoriteCivicAttitudeDivisor() const;		// Exposed to Python
	int getFavoriteCivicAttitudeChangeLimit() const;	// Exposed to Python
	int getDemandTributeAttitudeThreshold() const;		// Exposed to Python
	int getNoGiveHelpAttitudeThreshold() const;			// Exposed to Python
	int getTechRefuseAttitudeThreshold() const;			// Exposed to Python
	int getStrategicBonusRefuseAttitudeThreshold() const;	// Exposed to Python
	int getHappinessBonusRefuseAttitudeThreshold() const;	// Exposed to Python
	int getHealthBonusRefuseAttitudeThreshold() const;		// Exposed to Python
	int getMapRefuseAttitudeThreshold() const;				// Exposed to Python
	int getDeclareWarRefuseAttitudeThreshold() const;		// Exposed to Python
	int getDeclareWarThemRefuseAttitudeThreshold() const;	// Exposed to Python
	int getStopTradingRefuseAttitudeThreshold() const;		// Exposed to Python
	int getStopTradingThemRefuseAttitudeThreshold() const;	// Exposed to Python
	int getAdoptCivicRefuseAttitudeThreshold() const;		// Exposed to Python
	int getConvertReligionRefuseAttitudeThreshold() const;	// Exposed to Python
	int getOpenBordersRefuseAttitudeThreshold() const;		// Exposed to Python
	int getDefensivePactRefuseAttitudeThreshold() const;	// Exposed to Python
	int getPermanentAllianceRefuseAttitudeThreshold() const;// Exposed to Python
	int getVassalRefuseAttitudeThreshold() const;			// Exposed to Python
	int getVassalPowerModifier() const;				// Exposed to Python
	int getFavoriteCivic() const;					// Exposed to Python
	int getFavoriteReligion() const;				// Exposed to Python
	int getFreedomAppreciation() const;				// Exposed to Python

	const TCHAR* getArtDefineTag() const;			// Exposed to Python
	void setArtDefineTag(const TCHAR* szVal);

	// Arrays

	bool hasTrait(int i) const;						// Exposed to Python

	int getFlavorValue(int i) const;				// Exposed to Python
	int getContactRand(int i) const;				// Exposed to Python
	int getContactDelay(int i) const;				// Exposed to Python
	int getMemoryDecayRand(int i) const;			// Exposed to Python
	int getMemoryAttitudePercent(int i) const;		// Exposed to Python
	int getNoWarAttitudeProb(int i) const;			// Exposed to Python
	int getUnitAIWeightModifier(int i) const;		// Exposed to Python
	int getImprovementWeightModifier(int i) const;	// Exposed to Python
	int getDiploPeaceIntroMusicScriptIds(int i) const;
	int getDiploPeaceMusicScriptIds(int i) const;
	int getDiploWarIntroMusicScriptIds(int i) const;
	int getDiploWarMusicScriptIds(int i) const;

	// Other

/************************************************************************************************/
/* Afforess					  Start		 07/29/10											   */
/*																							  */
/* Advanced Diplomacy																		   */
/************************************************************************************************/
	int getWorkerRefuseAttitudeThreshold() const;
	int getMilitaryUnitRefuseAttitudeThreshold() const;
	int getCorporationRefuseAttitudeThreshold() const;
	int getSecretaryGeneralVoteRefuseAttitudeThreshold() const;
	void setCultureVictoryWeight(int i);
	void setSpaceVictoryWeight(int i);
	void setConquestVictoryWeight(int i);
	void setDominationVictoryWeight(int i);
	void setDiplomacyVictoryWeight(int i);
protected:
	int m_iMilitaryUnitRefuseAttitudeThreshold;
	int m_iWorkerRefuseAttitudeThreshold;
	int m_iCorporationRefuseAttitudeThreshold;
	int m_iSecretaryGeneralVoteRefuseAttitudeThreshold;
	void setDefaultMemoryInfo();
	void setDefaultContactInfo();
public:
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	DllExport const CvArtInfoLeaderhead* getArtInfo() const;
	const TCHAR* getLeaderHead() const;
	const TCHAR* getButton() const;

	void write(FDataStreamBase* stream) {}
	void read(FDataStreamBase* stream) {}
	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvLeaderHeadInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	bool m_bNPC;

	int m_iWonderConstructRand;
	int m_iBaseAttitude;
	int m_iBasePeaceWeight;
	int m_iPeaceWeightRand;
	int m_iWarmongerRespect;
	int m_iEspionageWeight;
	int m_iRefuseToTalkWarThreshold;
	int m_iNoTechTradeThreshold;
	int m_iTechTradeKnownPercent;
	int m_iMaxGoldTradePercent;
	int m_iMaxGoldPerTurnTradePercent;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					  03/21/10								jdog5000	  */
/*																							  */
/* Victory Strategy AI																		  */
/************************************************************************************************/
	int m_iCultureVictoryWeight;
	int m_iSpaceVictoryWeight;
	int m_iConquestVictoryWeight;
	int m_iDominationVictoryWeight;
	int m_iDiplomacyVictoryWeight;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					   END												  */
/************************************************************************************************/
	int m_iMaxWarRand;
	int m_iMaxWarNearbyPowerRatio;
	int m_iMaxWarDistantPowerRatio;
	int m_iMaxWarMinAdjacentLandPercent;
	int m_iLimitedWarRand;
	int m_iLimitedWarPowerRatio;
	int m_iDogpileWarRand;
	int m_iMakePeaceRand;
	int m_iDeclareWarTradeRand;
	int m_iDemandRebukedSneakProb;
	int m_iDemandRebukedWarProb;
	int m_iRazeCityProb;
	int m_iBuildUnitProb;
	int m_iBaseAttackOddsChange;
	int m_iAttackOddsChangeRand;
	int m_iWorseRankDifferenceAttitudeChange;
	int m_iBetterRankDifferenceAttitudeChange;
	int m_iCloseBordersAttitudeChange;
	int m_iLostWarAttitudeChange;
	int m_iAtWarAttitudeDivisor;
	int m_iAtWarAttitudeChangeLimit;
	int m_iAtPeaceAttitudeDivisor;
	int m_iAtPeaceAttitudeChangeLimit;
	int m_iSameReligionAttitudeChange;
	int m_iSameReligionAttitudeDivisor;
	int m_iSameReligionAttitudeChangeLimit;
	int m_iDifferentReligionAttitudeChange;
	int m_iDifferentReligionAttitudeDivisor;
	int m_iDifferentReligionAttitudeChangeLimit;
	int m_iBonusTradeAttitudeDivisor;
	int m_iBonusTradeAttitudeChangeLimit;
	int m_iOpenBordersAttitudeDivisor;
	int m_iOpenBordersAttitudeChangeLimit;
	int m_iDefensivePactAttitudeDivisor;
	int m_iDefensivePactAttitudeChangeLimit;
	int m_iShareWarAttitudeChange;
	int m_iShareWarAttitudeDivisor;
	int m_iShareWarAttitudeChangeLimit;
	int m_iFavoriteCivicAttitudeChange;
	int m_iFavoriteCivicAttitudeDivisor;
	int m_iFavoriteCivicAttitudeChangeLimit;
	int m_iDemandTributeAttitudeThreshold;
	int m_iNoGiveHelpAttitudeThreshold;
	int m_iTechRefuseAttitudeThreshold;
	int m_iStrategicBonusRefuseAttitudeThreshold;
	int m_iHappinessBonusRefuseAttitudeThreshold;
	int m_iHealthBonusRefuseAttitudeThreshold;
	int m_iMapRefuseAttitudeThreshold;
	int m_iDeclareWarRefuseAttitudeThreshold;
	int m_iDeclareWarThemRefuseAttitudeThreshold;
	int m_iStopTradingRefuseAttitudeThreshold;
	int m_iStopTradingThemRefuseAttitudeThreshold;
	int m_iAdoptCivicRefuseAttitudeThreshold;
	int m_iConvertReligionRefuseAttitudeThreshold;
	int m_iOpenBordersRefuseAttitudeThreshold;
	int m_iDefensivePactRefuseAttitudeThreshold;
	int m_iPermanentAllianceRefuseAttitudeThreshold;
	int m_iVassalRefuseAttitudeThreshold;
	int m_iVassalPowerModifier;
	int m_iFreedomAppreciation;
	int m_iFavoriteCivic;
	int m_iFavoriteReligion;

	CvString m_szArtDefineTag;

	// Arrays

	bool* m_pbTraits;

	int* m_piFlavorValue;
	int* m_piContactRand;
	int* m_piContactDelay;
	int* m_piMemoryDecayRand;
	int* m_piMemoryAttitudePercent;
	int* m_piNoWarAttitudeProb;
	int* m_piUnitAIWeightModifier;
	int* m_piImprovementWeightModifier;
	int* m_piDiploPeaceIntroMusicScriptIds;
	int* m_piDiploPeaceMusicScriptIds;
	int* m_piDiploWarIntroMusicScriptIds;
	int* m_piDiploWarMusicScriptIds;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvReligionInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvReligionInfo : public CvHotkeyInfo
{
	//---------------------------PUBLIC INTERFACE----------------------------------------
public:

	CvReligionInfo();
	virtual ~CvReligionInfo();

	int getChar() const;						// Exposed to Python
	// TGA_INDEXATION 01/21/08 MRGENIE
	int getTGAIndex() const;
	void setTGAIndex(int i);

	void setChar(int i);
	int getHolyCityChar() const;				// Exposed to Python
	void setHolyCityChar(int i);
	int getTechPrereq() const;					// Exposed to Python
	int getFreeUnitClass() const;				// Exposed to Python
	int getNumFreeUnits() const;				// Exposed to Python
	int getSpreadFactor() const;				// Exposed to Python
	int getMissionType() const;					// Exposed to Python
	void setMissionType(int iNewType);

	const TCHAR* getTechButton() const;			// Exposed to Python
	void setTechButton(const TCHAR* szVal);
	const TCHAR* getGenericTechButton() const;	// Exposed to Python
	void setGenericTechButton(const TCHAR* szVal);
	const TCHAR* getMovieFile() const;			// Exposed to Python
	void setMovieFile(const TCHAR* szVal);
	const TCHAR* getMovieSound() const;			// Exposed to Python
	void setMovieSound(const TCHAR* szVal);
	const TCHAR* getSound() const;				// Exposed to Python
	void setSound(const TCHAR* szVal);

	const TCHAR* getButtonDisabled() const;		//	Exposed to Python

	void setAdjectiveKey(const TCHAR* szVal);
	const wchar* getAdjectiveKey() const;			// Exposed to Python
	std::wstring pyGetAdjectiveKey() { return getAdjectiveKey(); }	// Exposed to Python

	// Arrays

	int getGlobalReligionCommerce(int i) const;		// Exposed to Python
	int* getGlobalReligionCommerceArray() const;
	int getHolyCityCommerce(int i) const;			// Exposed to Python
	int* getHolyCityCommerceArray() const;
	int getStateReligionCommerce(int i) const;		// Exposed to Python
	int* getStateReligionCommerceArray() const;
	int getFlavorValue(int i) const;				// Exposed to Python

	bool read(CvXMLLoadUtility* pXML);

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvReligionInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

	CvPropertyManipulators* getPropertyManipulators();

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	// TGA_INDEXATION 01/21/08 MRGENIE
	int m_iTGAIndex;

	int m_iChar;
	int m_iHolyCityChar;
	int m_iTechPrereq;
	int m_iFreeUnitClass;
	int m_iNumFreeUnits;
	int m_iSpreadFactor;
	int m_iMissionType;

	CvString m_szTechButton;
	CvString m_szGenericTechButton;
	CvString m_szMovieFile;
	CvString m_szMovieSound;
	CvString m_szSound;
	CvWString m_szAdjectiveKey;

	// Arrays

	int* m_paiGlobalReligionCommerce;
	int* m_paiHolyCityCommerce;
	int* m_paiStateReligionCommerce;
	int* m_piFlavorValue;

	CvPropertyManipulators m_PropertyManipulators;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCorporationInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCorporationInfo : public CvHotkeyInfo
{
	//---------------------------PUBLIC INTERFACE----------------------------------------
public:

	CvCorporationInfo();
	virtual ~CvCorporationInfo();

	int getChar() const;					// Exposed to Python
	// TGA_INDEXATION 01/21/08 MRGENIE
	int getTGAIndex() const;
	void setTGAIndex(int i);

	void setChar(int i);
	int getHeadquarterChar() const;			// Exposed to Python
	void setHeadquarterChar(int i);
	int getTechPrereq() const;				// Exposed to Python
	int getFreeUnitClass() const;			// Exposed to Python
	int getSpreadFactor() const;			// Exposed to Python
	int getSpreadCost() const;				// Exposed to Python
	int getMaintenance() const;				// Exposed to Python
	int getMissionType() const;				// Exposed to Python
	void setMissionType(int iNewType);

	int getBonusProduced() const;			// Exposed to Python

	const TCHAR* getMovieFile() const;		// Exposed to Python
	void setMovieFile(const TCHAR* szVal);
	const TCHAR* getMovieSound() const;		// Exposed to Python
	void setMovieSound(const TCHAR* szVal);
	const TCHAR* getSound() const;			// Exposed to Python
	void setSound(const TCHAR* szVal);

	// Arrays

	int getPrereqBonus(int i) const;			// Exposed to Python
	int getHeadquarterCommerce(int i) const;	// Exposed to Python
	int* getHeadquarterCommerceArray() const;
	int getCommerceProduced(int i) const;		// Exposed to Python
	int* getCommerceProducedArray() const;
	int getYieldProduced(int i) const;			// Exposed to Python
	int* getYieldProducedArray() const;

	bool read(CvXMLLoadUtility* pXML);
/************************************************************************************************/
/* Afforess					  Start		 01/14/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	int getObsoleteTech() const;
	int getSpread() const;
	int getHealth() const;
	int getHappiness() const;
	int getMilitaryProductionModifier() const;
	int getFreeXP() const;
	int getPrereqGameOption() const;

	int getPrereqBuildingClass(int i) const;
	int getPrereqBuildingVectorSize();
	CvString getPrereqBuildingNamesVectorElement(int i);
	int getPrereqBuildingClassValuesVectorElement(int i);

	bool isCompetingCorporation(int i) const;
	int getCompetingCorporationVectorSize();
	CvString getCompetingCorporationNamesVectorElement(int i);
	bool getCompetingCorporationValuesVectorElement(int i);

	bool readPass3();
	int getYieldChange(int i) const;
	int* getYieldChangeArray() const;
	int getCommerceChange(int i) const;
	int* getCommerceChangeArray() const;
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}

	void copyNonDefaults(CvCorporationInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

	CvPropertyManipulators* getPropertyManipulators();

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	// TGA_INDEXATION 01/21/08 MRGENIE
	int m_iTGAIndex;

	int m_iChar;
	int m_iHeadquarterChar;
	int m_iTechPrereq;
	int m_iFreeUnitClass;
	int m_iSpreadFactor;
	int m_iSpreadCost;
	int m_iMaintenance;
	int m_iMissionType;
	int m_iBonusProduced;
/************************************************************************************************/
/* Afforess					  Start		 01/14/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	int m_iObsoleteTech;
	int m_iSpread;
	int m_iHealth;
	int m_iHappiness;
	int m_iFreeXP;
	int m_iMilitaryProductionModifier;
	int m_iPrereqGameOption;

	int* m_paiPrereqBuildingClass;
	std::vector<CvString> m_aszPrereqBuildingClassforPass3;
	std::vector<int> m_aiPrereqBuildingClassforPass3;

	bool* m_pabCompetingCorporation;
	std::vector<CvString> m_aszCompetingCorporationforPass3;
	std::vector<bool> m_abCompetingCorporationforPass3;

	int* m_piYieldChange;
	int* m_piCommerceChange;
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	CvString m_szMovieFile;
	CvString m_szMovieSound;
	CvString m_szSound;

	// Arrays

	int* m_paiPrereqBonuses;
	int* m_paiHeadquarterCommerce;
	int* m_paiCommerceProduced;
	int* m_paiYieldProduced;

	CvPropertyManipulators m_PropertyManipulators;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvTraitInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTraitInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvTraitInfo();
	virtual ~CvTraitInfo();

	int getHealth() const;								// Exposed to Python
	int getHappiness() const;							// Exposed to Python
	int getMaxAnarchy() const;							// Exposed to Python
	int getUpkeepModifier() const;						// Exposed to Python
	int getLevelExperienceModifier() const;				// Exposed to Python
	int getGreatPeopleRateModifier() const;				// Exposed to Python
	int getGreatGeneralRateModifier() const;			// Exposed to Python
	int getDomesticGreatGeneralRateModifier() const;	// Exposed to Python
	int getMaxGlobalBuildingProductionModifier() const;	// Exposed to Python
	int getMaxTeamBuildingProductionModifier() const;	// Exposed to Python
	int getMaxPlayerBuildingProductionModifier() const;	// Exposed to Python

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		RevTraitEffects															*/
/********************************************************************************/
	int getRevIdxLocal() const;							// Exposed to Python
	int getRevIdxNational() const;						// Exposed to Python
	int getRevIdxDistanceModifier() const;				// Exposed to Python
	int getRevIdxHolyCityGood() const;					// Exposed to Python
	int getRevIdxHolyCityBad() const;					// Exposed to Python
	float getRevIdxNationalityMod() const;				// Exposed to Python
	float getRevIdxBadReligionMod() const;				// Exposed to Python
	float getRevIdxGoodReligionMod() const;				// Exposed to Python
	bool isNonStateReligionCommerce() const;			// Exposed to Python
	bool isUpgradeAnywhere() const;						// Exposed to Python
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

	const TCHAR* getShortDescription() const;			// Exposed to Python
	void setShortDescription(const TCHAR* szVal);

	// Arrays

	int getExtraYieldThreshold(int i) const;			// Exposed to Python
	int getTradeYieldModifier(int i) const;				// Exposed to Python
	int getCommerceChange(int i) const;					// Exposed to Python
	int getCommerceModifier(int i) const;				// Exposed to Python
/*
	int isFreePromotion(int i) const;					// Exposed to Python
	int isFreePromotionUnitCombat(int i) const;
*/

	//TB Traits Mods begin
	//Textual References
	int getPrereqTrait() const;
	int getPrereqOrTrait1() const;
	int getPrereqOrTrait2() const;
	PromotionLineTypes getPromotionLine() const;
	int getGreatPeopleUnitClass() const;
	TechTypes getPrereqTech() const;
	//Team Project (6)
	SpecialistTypes getEraAdvanceFreeSpecialistType() const;
	int getGoldenAgeonBirthofGreatPeopleType() const;
	//integers
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
	int getBaseFreeUnits() const;
	int getBaseFreeMilitaryUnits() const;
	int getFreeUnitsPopulationPercent() const;
	int getFreeMilitaryUnitsPopulationPercent() const;
	int getGoldPerUnit() const;
	int getGoldPerMilitaryUnit() const;
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
	bool isNegativeTrait() const;
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

	// bool vector without delayed resolution
	int getNotOnGameOption(int i) const;
	int getNumNotOnGameOptions() const;
	bool isNotOnGameOption(int i);

	int getOnGameOption(int i) const;
	int getNumOnGameOptions() const;
	bool isOnGameOption(int i);

	//Arrays
	int getSpecialistYieldChange(int i, int j) const;	// Exposed to Python
	int* getSpecialistYieldChangeArray(int i) const;
	bool m_bAnySpecialistYieldChanges;

	int getYieldModifier(int i) const;					// Exposed to Python
	int* getYieldModifierArray() const;

	int getCapitalYieldModifier(int i) const;			// Exposed to Python
	int* getCapitalYieldModifierArray() const;

	int getCapitalCommerceModifier(int i) const;		// Exposed to Python
	int* getCapitalCommerceModifierArray() const;

	int getSpecialistExtraCommerce(int i) const;		// Exposed to Python
	int* getSpecialistExtraCommerceArray() const;

	int getSpecialistExtraYield(int i) const;			// Exposed to Python
	int* getSpecialistExtraYieldArray() const;

	int getYieldChange(int i) const;					// Exposed to Python

	int getSpecialistCommerceChange(int i, int j) const;// Exposed to Python
	int* getSpecialistCommerceChangeArray(int i) const;
	bool m_bAnySpecialistCommerceChanges;

	int getFlavorValue(int i) const;
	int getLessYieldThreshold(int i) const;

	int getSeaPlotYieldChanges(int i) const;
	int* getSeaPlotYieldChangesArray() const;

	int getImprovementYieldChange(int i, int j) const;	// Exposed to Python
	int* getImprovementYieldChangeArray(int i) const;
	bool m_bAnyImprovementYieldChanges;
	//Team Project (7)
	int getGoldenAgeYieldChanges(int i) const;
	int* getGoldenAgeYieldChangesArray() const;

	int getGoldenAgeCommerceChanges(int i) const;
	int* getGoldenAgeCommerceChangesArray() const;

	// int vector utilizing struct with delayed resolution

	int getNumImprovementUpgradeModifierTypes() const;
	ImprovementModifier getImprovementUpgradeModifier(int iImprovement);

	int getNumBuildWorkerSpeedModifierTypes() const;
	BuildModifier getBuildWorkerSpeedModifier(int iBuild);

	int getNumDisallowedTraitTypes() const;
	DisallowedTraitType isDisallowedTraitType(int iTrait);

	int getNumDomainFreeExperiences() const;
	DomainModifier getDomainFreeExperience(int iDomain);

	int getNumDomainProductionModifiers() const;
	DomainModifier getDomainProductionModifier(int iDomain);

	int getNumTechResearchModifiers() const;
	TechModifier getTechResearchModifier(int iTech);

	int getNumBuildingProductionModifiers() const;
	BuildingModifier getBuildingProductionModifier(int iBuilding);

	int getNumSpecialBuildingProductionModifiers() const;
	SpecialBuildingModifier getSpecialBuildingProductionModifier(int iSpecialBuilding);

	int getNumBuildingHappinessModifiers() const;
	BuildingModifier getBuildingHappinessModifier(int iBuilding);

	int getNumUnitProductionModifiers() const;
	UnitModifier getUnitProductionModifier(int iUnit);

	int getNumSpecialUnitProductionModifiers() const;
	SpecialUnitModifier getSpecialUnitProductionModifier(int iSpecialUnit);

	int getNumCivicOptionNoUpkeepTypes() const;
	CivicOptionTypeBool isCivicOptionNoUpkeepType(int iCivicOption);
	//Team Project (8)
	int getNumUnitCombatFreeExperiences() const;
	UnitCombatModifier getUnitCombatFreeExperience(int iUnitCombat);

	int getNumUnitCombatProductionModifiers() const;
	UnitCombatModifier getUnitCombatProductionModifier(int iUnitCombat);

	int getNumBonusHappinessChanges() const;
	BonusModifier getBonusHappinessChange(int iBonus);

	CvPropertyManipulators* getPropertyManipulators();

	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);
	bool readPass3();
	void copyNonDefaults(CvTraitInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL );
	void copyNonDefaultsReadPass2(CvTraitInfo* pClassInfo = NULL , CvXMLLoadUtility* pXML = NULL, bool bOver = false);
	void getCheckSum(unsigned int& iSum);

	bool isFreePromotionUnitCombats(int i, int j) const;
//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:
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

	CvPropertyManipulators m_PropertyManipulators;
	CvPropertyManipulators m_PropertyManipulatorsNull;

	//TB Traits Mods begin
	//Textual References
	int m_iPrereqTrait;
	int m_iPrereqOrTrait1;
	int m_iPrereqOrTrait2;
	TechTypes m_ePrereqTech;
	PromotionLineTypes m_ePromotionLine;
	int m_iGreatPeopleUnitClass;
	//Team Project (6)
	SpecialistTypes m_eEraAdvanceFreeSpecialistType;
	int m_iGoldenAgeonBirthofGreatPeopleType;
	//integers
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
	int m_iBaseFreeUnits;
	int m_iBaseFreeMilitaryUnits;
	int m_iFreeUnitsPopulationPercent;
	int m_iFreeMilitaryUnitsPopulationPercent;
	int m_iGoldPerUnit;
	int m_iGoldPerMilitaryUnit;
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
	//Team Project (7)
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
	//Team Project (7)
	int* m_piGoldenAgeYieldChangesFiltered;
	int* m_piGoldenAgeCommerceChangesFiltered;
	// bool vector without delayed resolution
	// int vector utilizing struct with delayed resolution
	std::vector<ImprovementModifier> m_aImprovementUpgradeModifierTypes;
	std::vector<BuildModifier> m_aBuildWorkerSpeedModifierTypes;
	std::vector<DisallowedTraitType> m_aDisallowedTraitTypes;
	std::vector<DomainModifier> m_aDomainFreeExperiences;
	std::vector<DomainModifier> m_aDomainProductionModifiers;
	std::vector<TechModifier> m_aTechResearchModifiers;
	std::vector<BuildingModifier> m_aBuildingProductionModifiers;
	std::vector<SpecialBuildingModifier> m_aSpecialBuildingProductionModifiers;
	std::vector<BuildingModifier> m_aBuildingHappinessModifiers;
	std::vector<UnitModifier> m_aUnitProductionModifiers;
	std::vector<SpecialUnitModifier> m_aSpecialUnitProductionModifiers;
	std::vector<CivicOptionTypeBool> m_aCivicOptionNoUpkeepTypes;
	//Team Project (8)
	std::vector<UnitCombatModifier> m_aUnitCombatFreeExperiences;
	std::vector<UnitCombatModifier> m_aUnitCombatProductionModifiers;
	std::vector<BonusModifier> m_aBonusHappinessChanges;
	//TB Traits mods end
};

#endif /* CV_INFO_PLAYER_H */
