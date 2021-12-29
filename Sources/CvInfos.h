#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvInfos.h
//
//  PURPOSE: All Civ4 info classes and the base class for them
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2003 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef CV_INFO_H
#define CV_INFO_H

#include "CvProperties.h"
#include "CvPropertySource.h"
#include "CvPropertyInteraction.h"
#include "CvPropertyPropagator.h"
#include "CvPropertyManipulators.h"
#include "CvOutcomeList.h"
#include "CvOutcomeMission.h"
#include "CvDate.h"
#include "BoolExpr.h"
//#include "IntExpr.h"
#include "IDValueMap.h"

extern bool shouldHaveType;

class CvXMLLoadUtility;
class IntExpr;
struct CvInfoUtil;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvInfoBase
//
//  DESC:   The base class for all info classes to inherit from.  This gives us
//			the base description and type strings
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvInfoBase
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:
	CvInfoBase();
	explicit CvInfoBase(const char* szType);
	DllExport virtual ~CvInfoBase();

	virtual void reset();

	bool isGraphicalOnly() const;

	DllExport const char* getType() const;
	virtual const char* getButton() const;

	const wchar_t* getCivilopediaKey() const;
	const wchar_t* getHelpKey() const;
	const wchar_t* getStrategyKey() const;

	// for python wide string handling
	std::wstring pyGetTextKey() const		{ return getTextKeyWide(); }
	std::wstring pyGetDescription() const	{ return getDescription(0); }
	std::wstring pyGetDescriptionForm(uint uiForm) const { return getDescription(uiForm); }
	std::wstring pyGetText() const			{ return getText(); }
	std::wstring pyGetCivilopedia() const	{ return getCivilopedia(); }
	std::wstring pyGetHelp() const			{ return getHelp(); }
	std::wstring pyGetStrategy() const		{ return getStrategy(); }

	DllExport const wchar_t* getTextKeyWide() const;
	DllExport const wchar_t* getDescription(uint uiForm = 0) const;
	DllExport const wchar_t* getText() const;
	const wchar_t* getCivilopedia() const;
	DllExport const wchar_t* getHelp() const;
	const wchar_t* getStrategy() const;

	virtual void read(FDataStreamBase*) {}
	virtual void write(FDataStreamBase*) {}

	virtual void getDataMembers(CvInfoUtil&) { FErrorMsg("Override this"); }
	virtual bool read(CvXMLLoadUtility* pXML);
	virtual bool readPass2(CvXMLLoadUtility*) { FErrorMsg("Override this"); return false; }
	virtual bool readPass3() { FErrorMsg("Override this"); return false; }
	virtual void copyNonDefaults(const CvInfoBase* pClassInfo);
	virtual void copyNonDefaultsReadPass2(CvInfoBase*, CvXMLLoadUtility*, bool = false)
	{ /* AIAndy: Default implementation for full copy of info without knowledge of one/twopass */ }
	virtual void getCheckSum(uint32_t&) const { }
	virtual void doPostLoadCaching(uint32_t) { }
	virtual const wchar_t* getExtraHoverText() const { return L""; }

protected:

	bool doneReadingXML(CvXMLLoadUtility* pXML);

	bool m_bGraphicalOnly;
	CvString m_szType;
	CvString m_szButton; // Used for Infos that don't require an ArtAssetInfo
	CvWString m_szTextKey;
	CvWString m_szCivilopediaKey;
	CvWString m_szHelpKey;
	CvWString m_szStrategyKey;

	// translated text
	std::vector<CvString> m_aszExtraXMLforPass3;
	mutable std::vector<CvWString> m_aCachedDescriptions;
	mutable CvWString m_szCachedText;
	mutable CvWString m_szCachedHelp;
	mutable CvWString m_szCachedStrategy;
	mutable CvWString m_szCachedCivilopedia;
};

//
// holds the scale for scalable objects
//
class CvScalableInfo
{
public:

	CvScalableInfo() : m_fScale(1.0f), m_fInterfaceScale(1.0f) { }

	DllExport float getScale() const;
	DllExport float getInterfaceScale() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvScalableInfo* pClassInfo);

protected:

	float m_fScale;
	float m_fInterfaceScale;	//!< the scale of the unit appearing in the interface screens
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:	  CvHotkeyInfo
//!  \brief			holds the hotkey info for an info class
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvHotkeyInfo : public CvInfoBase
{
public:
	//constructor
	CvHotkeyInfo();
	//destructor
	virtual ~CvHotkeyInfo();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvHotkeyInfo* pClassInfo);

	int getActionInfoIndex() const;
	void setActionInfoIndex(int i);

	int getHotKeyVal() const;
	int getHotKeyPriority() const;
	int getHotKeyValAlt() const;
	int getHotKeyPriorityAlt() const;
	int getOrderPriority() const;

	bool isAltDown() const;
	bool isShiftDown() const;
	bool isCtrlDown() const;
	bool isAltDownAlt() const;
	bool isShiftDownAlt() const;
	bool isCtrlDownAlt() const;
	const char* getHotKey() const;
	const wchar_t* getHotKeyDescriptionKey() const;
	const wchar_t* getHotKeyAltDescriptionKey() const;
	const wchar_t* getHotKeyString() const;

	std::wstring getHotKeyDescription() const;
	void setHotKeyDescription(const wchar_t* szHotKeyDescKey, const wchar_t* szHotKeyAltDescKey, const wchar_t* szHotKeyString);

protected:

	int m_iActionInfoIndex;

	int m_iHotKeyVal;
	int m_iHotKeyPriority;
	int m_iHotKeyValAlt;
	int m_iHotKeyPriorityAlt;
	int m_iOrderPriority;

	bool m_bAltDown;
	bool m_bShiftDown;
	bool m_bCtrlDown;
	bool m_bAltDownAlt;
	bool m_bShiftDownAlt;
	bool m_bCtrlDownAlt;

	CvString m_szHotKey;
	CvWString m_szHotKeyDescriptionKey;
	CvWString m_szHotKeyAltDescriptionKey;
	CvWString m_szHotKeyString;
};

class CvDiplomacyInfo;
class CvDiplomacyResponse
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvDiplomacyResponse();
	virtual ~CvDiplomacyResponse();

	int getNumDiplomacyText() const;

	bool getCivilizationTypes(const int i) const;
	bool* getCivilizationTypes() const;

	bool getLeaderHeadTypes(const int i) const;
	bool* getLeaderHeadTypes() const;

	bool getAttitudeTypes(int i) const;
	bool* getAttitudeTypes() const;

	bool getDiplomacyPowerTypes(const int i) const;
	bool* getDiplomacyPowerTypes() const;

	const char* getDiplomacyText(int i) const;
	const CvString* getDiplomacyText() const;

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
//  class : CvSpecialistInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpecialistInfo : public CvHotkeyInfo
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvSpecialistInfo();
	virtual ~CvSpecialistInfo();

	int getGreatPeopleUnitType() const;
	int getGreatPeopleRateChange() const;
	int getMissionType() const;
	void setMissionType(int iNewType);
	// Afforess 03/26/10
	int getHealthPercent() const;
	int getHappinessPercent() const;

	bool isSlave() const;

	int getExperience() const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

	bool isVisible() const;

	// Arrays

	int getYieldChange(int i) const;
	const int* getYieldChangeArray() const;
	int getCommerceChange(int i) const;
	int getFlavorValue(int i) const;

	const char* getTexture() const;

	//Team Project (1)
	//TB Specialist Tags
	// int
	int getInsidiousness() const;
	int getInvestigation() const;

	//int getNumTechHappinessTypes() const;
	int getTechHappiness(TechTypes eTech) const;
	const IDValueMap<TechTypes, int>& getTechHappinessTypes() const { return m_aTechHappinessTypes; }

	//int getNumTechHealthTypes() const;
	int getTechHealth(TechTypes eTech) const;
	const IDValueMap<TechTypes, int>& getTechHealthTypes() const { return m_aTechHealthTypes; }

	int getNumUnitCombatExperienceTypes() const;
	const UnitCombatModifier& getUnitCombatExperienceType(int iUnitCombat) const;

	//TB Specialist Tags end

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvSpecialistInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

private:
	CvPropertyManipulators m_PropertyManipulators;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iGreatPeopleUnitType;
	int m_iGreatPeopleRateChange;
	int m_iMissionType;
	// Afforess 03/26/10
	int m_iHealthPercent;
	int m_iHappinessPercent;

	int m_iExperience;
	//TB Specialist Tags
	int m_iInsidiousness;
	int m_iInvestigation;

	// Afforess 03/26/10
	bool m_bSlave;

	bool m_bVisible;

	// Arrays
	int* m_piYieldChange;
	int* m_piCommerceChange;
	int* m_piFlavorValue;

	//Team Project (1)
	//TB Specialist Tags
	// int vector utilizing struct with delayed resolution
	IDValueMap<TechTypes, int> m_aTechHappinessTypes;
	IDValueMap<TechTypes, int> m_aTechHealthTypes;
	std::vector<UnitCombatModifier> m_aUnitCombatExperienceTypes;
	std::vector<UnitCombatModifier> m_aUnitCombatExperienceTypesNull;

	CvString m_szTexture;
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
//friend class CvXMLLoadUtility;

//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvTechInfo();
	virtual ~CvTechInfo();

	// Validate tech info da after load (debug only)
	static void validate();

	int getAdvisorType() const;
	int getAIWeight() const;
	int getAITradeModifier() const;
	int getResearchCost() const;
	int getEra() const;
	int getTradeRoutes() const;
	int getFeatureProductionModifier() const;
	int getWorkerSpeedModifier() const;

	int getMaintenanceModifier() const;
	int getDistanceMaintenanceModifier() const;
	int getNumCitiesMaintenanceModifier() const;
	int getCoastalDistanceMaintenanceModifier() const;

	int getFirstFreeUnit() const;
	int getFirstFreeProphet() const;
	int getHealth() const;
	int getHappiness() const;
	int getFirstFreeTechs() const;
	int getAssetValue() const;
	int getPowerValue() const;

	int getGridX() const;
	int getGridY() const;

	bool isRepeat() const;
	bool isTrade() const;
	bool isDisable() const;
	bool isGoodyTech() const;
	bool isExtraWaterSeeFrom() const;
	bool isMapCentering() const;
	bool isMapVisible() const;
	bool isMapTrading() const;
	bool isTechTrading() const;
	bool isGoldTrading() const;
	bool isOpenBordersTrading() const;
	bool isDefensivePactTrading() const;
	bool isPermanentAllianceTrading() const;
	bool isVassalStateTrading() const;
	bool isBridgeBuilding() const;
	bool isIrrigation() const;
	bool isIgnoreIrrigation() const;
	bool isWaterWork() const;
	bool isRiverTrade() const;

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

	int getNumPrereqBuildings() const;
	const PrereqBuilding& getPrereqBuilding(int iIndex) const;
	int getPrereqBuildingType(int iIndex) const;
	int getPrereqBuildingMinimumRequired(int iIndex) const;

	int getNumPrereqOrBuildings() const;
	const PrereqBuilding& getPrereqOrBuilding(int iIndex) const;
	int getPrereqOrBuildingType(int iIndex) const;
	int getPrereqOrBuildingMinimumRequired(int iIndex) const;

	bool isGlobal() const;

	// Dale - AB: Bombing START
	bool getDCMAirBombTech1() const;
	bool getDCMAirBombTech2() const;
protected:
	bool m_bDCMAirBombTech1;
	bool m_bDCMAirBombTech2;
public:
	// Dale - AB: Bombing END

	std::wstring getQuote() const;
	const char* getQuoteKey() const;
	const char* getSound() const;
	const char* getSoundMP() const;

	// Arrays

	int getDomainExtraMoves(int i) const;
	int getFlavorValue(int i) const;

	const std::vector<TechTypes>& getPrereqOrTechs() const;
	const std::vector<TechTypes>& getPrereqAndTechs() const;

	bool isCommerceFlexible(int i) const;
	bool isTerrainTrade(int i) const;
	//ls612: Tech Commerce Modifiers
	int getCommerceModifier(int i) const;
	int* getCommerceModifierArray() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvTechInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iAdvisorType;
	int m_iAIWeight;
	int m_iAITradeModifier;
	int m_iResearchCost;
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
	int m_iFirstFreeUnit;
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

	std::vector<TechTypes> m_piPrereqOrTechs;
	std::vector<TechTypes> m_piPrereqAndTechs;

	bool* m_pbCommerceFlexible;
	bool* m_pbTerrainTrade;
	//ls612: Tech Commerce Modifiers
	int* m_piCommerceModifier;

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

	std::vector<PrereqBuilding> m_aPrereqBuilding;
	std::vector<PrereqBuilding> m_aPrereqOrBuilding;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPromotionInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPromotionInfo :	public CvHotkeyInfo
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:
	CvPromotionInfo();
	virtual ~CvPromotionInfo();

	int getLayerAnimationPath() const;
	int getPrereqPromotion() const;
	int getPrereqOrPromotion1() const;
	int getPrereqOrPromotion2() const;

	TechTypes getTechPrereq() const;
	int getMinEraType() const;
	int getMaxEraType() const;
	int getStateReligionPrereq() const;
	int getVisibilityChange() const;
	int getMovesChange() const;
	int getMoveDiscountChange() const;
	int getAirRangeChange() const;
	int getInterceptChange() const;
	int getEvasionChange() const;
	int getWithdrawalChange() const;
	int getCargoChange() const;
	int getSMCargoChange() const;
	int getSMCargoVolumeChange() const;
	int getSMCargoVolumeModifierChange() const;
	int getCollateralDamageChange() const;
	int getBombardRateChange() const;
	int getFirstStrikesChange() const;
	int getChanceFirstStrikesChange() const;
	int getEnemyHealChange() const;
	int getNeutralHealChange() const;
	int getFriendlyHealChange() const;
	int getSameTileHealChange() const;
	int getAdjacentTileHealChange() const;
	int getCombatPercent() const;
	int getCityAttackPercent() const;
	int getCityDefensePercent() const;
	int getHillsAttackPercent() const;
	int getHillsDefensePercent() const;
	int getHillsWorkPercent() const;
	//ls612: Work rate modifiers
	int getWorkRatePercent() const;
	int getCommandType() const;
	void setCommandType(int iNewType);

	int getRevoltProtection() const;
	int getCollateralDamageProtection() const;
	int getPillageChange() const;
	int getUpgradeDiscount() const;
	int getExperiencePercent() const;
	int getKamikazePercent() const;

	int getAirCombatLimitChange() const;
	int getCelebrityHappy() const;
	int getCollateralDamageLimitChange() const;
	int getCollateralDamageMaxUnitsChange() const;
	int getCombatLimitChange() const;
	int getExtraDropRange() const;

	int getSurvivorChance() const;
	int getVictoryAdjacentHeal() const;
	int getVictoryHeal() const;
	int getVictoryStackHeal() const;
	bool isDefensiveVictoryMove() const;
	bool isFreeDrop() const;
	bool isOffensiveVictoryMove() const;
	bool isOneUp() const;

	bool isPillageEspionage() const;
	bool isPillageMarauder() const;
	bool isPillageOnMove() const;
	bool isPillageOnVictory() const;
	bool isPillageResearch() const;

	bool isLeader() const;
	bool isBlitz() const;
	bool isAmphib() const;
	bool isRiver() const;
	bool isEnemyRoute() const;
	bool isAlwaysHeal() const;
	bool isHillsDoubleMove() const;
	bool isImmuneToFirstStrikes() const;

	const char* getSound() const;

	bool changesMoveThroughPlots() const;
	//	This really belongs on CvInfoBase but you can't change the size of that
	//	object without crashing the core engine :-(
	inline int	getZobristValue() const { return m_zobristValue; }

	// Arrays

	int getTerrainAttackPercent(int i) const;
	bool isAnyTerrainAttackPercent() const;
	int getTerrainDefensePercent(int i) const;
	bool isAnyTerrainDefensePercent() const;
	int getFeatureAttackPercent(int i) const;
	bool isAnyFeatureAttackPercent() const;
	int getFeatureDefensePercent(int i) const;
	bool isAnyFeatureDefensePercent() const;
	int getUnitCombatModifierPercent(int i) const;
	bool isAnyUnitCombatModifierPercent() const;
	int getDomainModifierPercent(int i) const;
	bool isAnyDomainModifierPercent() const;
	//ls612: Terrain Work Modifiers
	int getTerrainWorkPercent(int i) const;
	int getFeatureWorkPercent(int i) const;

	bool getTerrainDoubleMove(int i) const;
	bool getFeatureDoubleMove(int i) const;
	bool getUnitCombat(int i) const;
	bool isCanMovePeaks() const;
	//	Koshling - enhanced mountaineering mode to differentiate between ability to move through
	//	mountains, and ability to lead a stack through mountains
	bool isCanLeadThroughPeaks() const;
	//const std::vector<PromotionTypes>& getPromotionOverwrites() const { return m_vPromotionOverwrites; }
	TechTypes getObsoleteTech() const;
	int getControlPoints() const;
	int getCommandRange() const;
	bool isZoneOfControl() const;
	int getIgnoreTerrainDamage() const;

	void getCheckSum(uint32_t& iSum) const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

	//TB Combat Mods Begin  TB SubCombat Mod begin
	//Text Strings
	const wchar_t* getRenamesUnitTo() const;
	// Textual References
	PromotionLineTypes getPromotionLine() const;
	UnitCombatTypes getReplacesUnitCombat() const;
	DomainTypes getDomainCargoChange() const;
	SpecialUnitTypes getSpecialCargoChange() const;
	SpecialUnitTypes getSpecialCargoPrereq() const;
	SpecialUnitTypes getSMSpecialCargoChange() const;
	SpecialUnitTypes getSMSpecialCargoPrereq() const;
	SpecialUnitTypes getSMNotSpecialCargoChange() const;
	SpecialUnitTypes getSMNotSpecialCargoPrereq() const;
	SpecialUnitTypes setSpecialUnit() const;
	// integers
	int getAttackCombatModifierChange() const;
	int getDefenseCombatModifierChange() const;
	// Fight or Flight
	int getPursuitChange() const;
	int getEarlyWithdrawChange() const;
	int getTauntChange() const;
	//
	int getVSBarbsChange() const;
	int getArmorChange() const;
	int getPunctureChange() const;
	// Heart of War
	int getOverrunChange() const;
	int getRepelChange() const;
	int getFortRepelChange() const;
	int getRepelRetriesChange() const;
	int getUnyieldingChange() const;
	int getKnockbackChange() const;
	int getKnockbackRetriesChange() const;
#ifdef BATTLEWORN
	int getStrAdjperRndChange() const;
	int getStrAdjperAttChange() const;
	int getStrAdjperDefChange() const;
	int getWithdrawAdjperAttChange() const;
#endif // BATTLEWORN
	// S&D Extended
	int getUnnerveChange() const;
	int getEncloseChange() const;
	int getLungeChange() const;
	int getDynamicDefenseChange() const;
	//
	int getStrengthChange() const;
	int getLinePriority() const;
	int getFortitudeChange() const;
	int getDamageperTurn() const;
	int getStrAdjperTurn() const;
	int getWeakenperTurn() const;
#ifdef STRENGTH_IN_NUMBERS
	int getFrontSupportPercentChange() const;
	int getShortRangeSupportPercentChange() const;
	int getMediumRangeSupportPercentChange() const;
	int getLongRangeSupportPercentChange() const;
	int getFlankSupportPercentChange() const;
#endif
	int getDodgeModifierChange() const;
	int getPrecisionModifierChange() const;
	int getPowerShotsChange() const;
	int getPowerShotCombatModifierChange() const;
	int getPowerShotPunctureModifierChange() const;
	int getPowerShotPrecisionModifierChange() const;
	int getPowerShotCriticalModifierChange() const;
	int getCriticalModifierChange() const;
	int getEnduranceChange() const;
	int getRoundStunProbChange() const;
	int getPoisonProbabilityModifierChange() const;

	int getCaptureProbabilityModifierChange() const;
	int getCaptureResistanceModifierChange() const;

	//WorkRateMod
	int getPeaksWorkPercent() const;
	//
	int getBreakdownChanceChange() const;
	int getBreakdownDamageChange() const;
	// Size Matters
	int getMaxHPChange() const;
	int getStrengthModifier() const;
	//
	int getQualityChange() const;
	int getGroupChange() const;
	int getLevelPrereq() const;
	int getDamageModifierChange() const;
	int getUpkeepModifier() const;
	int getExtraUpkeep100() const;
	int getRBombardDamageChange() const;
	int getRBombardDamageLimitChange() const;
	int getRBombardDamageMaxUnitsChange() const;
	int getDCMBombRangeChange() const;
	int getDCMBombAccuracyChange() const;
	int getCombatModifierPerSizeMoreChange() const;
	int getCombatModifierPerSizeLessChange() const;
	int getCombatModifierPerVolumeMoreChange() const;
	int getCombatModifierPerVolumeLessChange() const;
	int getSelfHealModifier() const;
	int getNumHealSupport() const;
	int getExcileChange() const;
	int getPassageChange() const;
	int getNoNonOwnedCityEntryChange() const;
	int getBarbCoExistChange() const;
	int getBlendIntoCityChange() const;
	int getUpgradeAnywhereChange() const;
	int getInsidiousnessChange() const;
	int getInvestigationChange() const;
	int getAssassinChange() const;
	int getStealthStrikesChange() const;
	int getStealthCombatModifierChange() const;
	int getStealthDefenseChange() const;
	int getDefenseOnlyChange() const;
	int getNoInvisibilityChange() const;
	int getTrapDamageMax() const;
	int getTrapDamageMin() const;
	int getTrapComplexity() const;
	int getNumTriggers() const;
	int getTriggerBeforeAttackChange() const;
	int getHiddenNationalityChange() const;
	int getAnimalIgnoresBordersChange() const;
	int getNoDefensiveBonusChange() const;
	int getGatherHerdChange() const;
	int getReligiousCombatModifierChange() const;
	//booleans
	bool isStampedeChange() const;
	bool isRemoveStampede() const;
	bool isOnslaughtChange() const;
	bool isEquipment() const;
#ifdef OUTBREAKS_AND_AFFLICTIONS
	bool isAffliction() const;
#endif
	bool isParalyze() const;
	bool isMakesDamageCold() const;
	bool isMakesDamageNotCold() const;
	bool isAddsColdImmunity() const;
	bool isRemovesColdImmunity() const;
	bool isCritical() const;
	bool isAttackOnlyCitiesAdd() const;
	bool isAttackOnlyCitiesSubtract() const;
	bool isIgnoreNoEntryLevelAdd() const;
	bool isIgnoreNoEntryLevelSubtract() const;
	bool isIgnoreZoneofControlAdd() const;
	bool isIgnoreZoneofControlSubtract() const;
	bool isFliesToMoveAdd() const;
	bool isFliesToMoveSubtract() const;
	bool isZeroesXP() const;
	bool isForOffset() const;
	bool isCargoPrereq() const;
	bool isRBombardPrereq() const;
	bool isNoSelfHeal() const;
	bool isSetOnHNCapture() const;
	bool isSetOnInvestigated() const;
	bool isStatus() const;
	bool isPrereqNormInvisible() const;
	bool isPlotPrereqsKeepAfter() const;
	bool isRemoveAfterSet() const;
	bool isQuick() const;
	//Arrays
	//int getAIWeightbyUnitCombatType(int i) const;
	//bool isAnyAIWeightbyUnitCombatType() const;

	// bool vector without delayed resolution
	int getSubCombatChangeType(int i) const;
	int getNumSubCombatChangeTypes() const;
	bool isSubCombatChangeType(int i) const;

	int getRemovesUnitCombatType(int i) const;
	int getNumRemovesUnitCombatTypes() const;
	bool isRemovesUnitCombatType(int i) const;

	int getOnGameOption(int i) const;
	int getNumOnGameOptions() const;
	bool isOnGameOption(int i) const;

	int getNotOnGameOption(int i) const;
	int getNumNotOnGameOptions() const;
	bool isNotOnGameOption(int i) const;

	int getFreetoUnitCombat(int i) const;
	int getNumFreetoUnitCombats() const;
	bool isFreetoUnitCombat(int i) const;

	int getNotOnUnitCombatType(int i) const;
	int getNumNotOnUnitCombatTypes() const;
	bool isNotOnUnitCombatType(int i) const;

	int getNotOnDomainType(int i) const;
	int getNumNotOnDomainTypes() const;
	bool isNotOnDomainType(int i) const;

	int getNoAutoEquiptoCombatClassType(int i) const;
	int getNumNoAutoEquiptoCombatClassTypes() const;
	bool isNoAutoEquiptoCombatClassType(int i) const;

	const std::vector<MapCategoryTypes>& getMapCategories() const { return m_aeMapCategoryTypes; }

#ifdef OUTBREAKS_AND_AFFLICTIONS
	int getCureAfflictionChangeType(int i) const;
	int getNumCureAfflictionChangeTypes() const;
	bool isCureAfflictionChangeType(int i) const;
#endif // OUTBREAKS_AND_AFFLICTIONS

	const std::vector<BonusTypes>& getPrereqBonuses() const { return m_aiPrereqBonusTypes; }

	int getAddsBuildType(int i) const;
	int getNumAddsBuildTypes() const;
	bool isAddsBuildType(int i) const;

	int getNegatesInvisibilityType(int i) const;
	int getNumNegatesInvisibilityTypes() const;
	bool isNegatesInvisibilityType(int i) const;

	int getPrereqTerrainType(int i) const;
	int getNumPrereqTerrainTypes() const;
	bool isPrereqTerrainType(int i) const;

	int getPrereqFeatureType(int i) const;
	int getNumPrereqFeatureTypes() const;
	bool isPrereqFeatureType(int i) const;

	int getPrereqImprovementType(int i) const;
	int getNumPrereqImprovementTypes() const;
	bool isPrereqImprovementType(int i) const;

	int getPrereqPlotBonusType(int i) const;
	int getNumPrereqPlotBonusTypes() const;
	bool isPrereqPlotBonusType(int i) const;

	int getPrereqLocalBuildingType(int i) const;
	int getNumPrereqLocalBuildingTypes() const;
	bool isPrereqLocalBuildingType(int i) const;

	int getTrapSetWithPromotionType(int i) const;
	int getNumTrapSetWithPromotionTypes() const;
	bool isTrapSetWithPromotionType(int i) const;

	int getTrapImmunityUnitCombatType(int i) const;
	int getNumTrapImmunityUnitCombatTypes() const;
	bool isTrapImmunityUnitCombatType(int i) const;

	int getTargetUnitCombatType(int i) const;
	int getNumTargetUnitCombatTypes() const;
	bool isTargetUnitCombatType(int i) const;

	// int vector utilizing pairing without delayed resolution

	int getNumFlankingStrikesbyUnitCombatTypesChange() const;
	int getFlankingStrengthbyUnitCombatTypeChange(int iUnitCombat) const;
	bool isFlankingStrikebyUnitCombatTypeChange(int iUnitCombat) const;

	int getNumWithdrawOnTerrainTypeChanges() const;
	int getWithdrawOnTerrainTypeChange(int iTerrain) const;

	int getNumWithdrawOnFeatureTypeChanges() const;
	int getWithdrawOnFeatureTypeChange(int iFeature) const;

	int getNumWithdrawVSUnitCombatChangeTypes() const;
	int getWithdrawVSUnitCombatChangeType(int iUnitCombat) const;
	bool isWithdrawVSUnitCombatChangeType(int iUnitCombat) const;

	int getNumPursuitVSUnitCombatChangeTypes() const;
	int getPursuitVSUnitCombatChangeType(int iUnitCombat, bool bForLoad = false) const;
	bool isPursuitVSUnitCombatChangeType(int iUnitCombat, bool bForLoad = false) const;

	int getNumRepelVSUnitCombatChangeTypes() const;
	int getRepelVSUnitCombatChangeType(int iUnitCombat, bool bForLoad = false) const;
	bool isRepelVSUnitCombatChangeType(int iUnitCombat, bool bForLoad = false) const;

	int getNumKnockbackVSUnitCombatChangeTypes() const;
	int getKnockbackVSUnitCombatChangeType(int iUnitCombat, bool bForLoad = false) const;
	bool isKnockbackVSUnitCombatChangeType(int iUnitCombat, bool bForLoad = false) const;

	int getNumPunctureVSUnitCombatChangeTypes() const;
	int getPunctureVSUnitCombatChangeType(int iUnitCombat) const;
	bool isPunctureVSUnitCombatChangeType(int iUnitCombat) const;

	int getNumArmorVSUnitCombatChangeTypes() const;
	int getArmorVSUnitCombatChangeType(int iUnitCombat) const;
	bool isArmorVSUnitCombatChangeType(int iUnitCombat) const;

	int getNumDodgeVSUnitCombatChangeTypes() const;
	int getDodgeVSUnitCombatChangeType(int iUnitCombat) const;
	bool isDodgeVSUnitCombatChangeType(int iUnitCombat) const;

	int getNumPrecisionVSUnitCombatChangeTypes() const;
	int getPrecisionVSUnitCombatChangeType(int iUnitCombat) const;
	bool isPrecisionVSUnitCombatChangeType(int iUnitCombat) const;

	int getNumCriticalVSUnitCombatChangeTypes() const;
	int getCriticalVSUnitCombatChangeType(int iUnitCombat) const;
	bool isCriticalVSUnitCombatChangeType(int iUnitCombat) const;

	int getNumRoundStunVSUnitCombatChangeTypes() const;
	int getRoundStunVSUnitCombatChangeType(int iUnitCombat) const;
	bool isRoundStunVSUnitCombatChangeType(int iUnitCombat) const;

	int getNumTrapDisableUnitCombatTypes() const;
	int getTrapDisableUnitCombatType(int iUnitCombat) const;
	bool isTrapDisableUnitCombatType(int iUnitCombat) const;

	int getNumTrapAvoidanceUnitCombatTypes() const;
	int getTrapAvoidanceUnitCombatType(int iUnitCombat) const;
	bool isTrapAvoidanceUnitCombatType(int iUnitCombat) const;

	int getNumTrapTriggerUnitCombatTypes() const;
	int getTrapTriggerUnitCombatType(int iUnitCombat) const;
	bool isTrapTriggerUnitCombatType(int iUnitCombat) const;

#ifdef OUTBREAKS_AND_AFFLICTIONS
	int getNumAidChanges() const;
	int getAidChange(int iProperty) const;
	bool isAidChange(int iProperty) const;
#endif

	int getNumBuildWorkRateModifierChangeTypes() const;
	int getBuildWorkRateModifierChangeType(int iBuild) const;
	bool isBuildWorkRateModifierChangeType(int iBuild) const;

	int getNumVisibilityIntensityChangeTypes() const;
	int getVisibilityIntensityChangeType(int iInvisibility) const;
	bool isVisibilityIntensityChangeType(int iInvisibility) const;

	int getNumInvisibilityIntensityChangeTypes() const;
	int getInvisibilityIntensityChangeType(int iInvisibility) const;
	bool isInvisibilityIntensityChangeType(int iInvisibility) const;

	int getNumVisibilityIntensityRangeChangeTypes() const;
	int getVisibilityIntensityRangeChangeType(int iInvisibility) const;
	bool isVisibilityIntensityRangeChangeType(int iInvisibility) const;

	int getNumAIWeightbyUnitCombatTypes() const;
	const UnitCombatModifier& getAIWeightbyUnitCombatType(int iUnitCombat) const;

	int getNumAfflictionFortitudeChangeModifiers() const;
	const PromotionLineModifier& getAfflictionFortitudeChangeModifier(int iAfflictionLine) const;

#ifdef OUTBREAKS_AND_AFFLICTIONS
	int getNumAfflictOnAttackChangeTypes() const;
	const AfflictOnAttackChange& getAfflictOnAttackChangeType(int iAfflictionLine) const;
#endif
	int getNumHealUnitCombatChangeTypes() const;
	const HealUnitCombat& getHealUnitCombatChangeType(int iUnitCombat) const;

	int getNumInvisibleTerrainChanges() const;
	const InvisibleTerrainChanges& getInvisibleTerrainChange(int iIndex) const;

	int getNumInvisibleFeatureChanges() const;
	const InvisibleFeatureChanges& getInvisibleFeatureChange(int iIndex) const;

	int getNumInvisibleImprovementChanges() const;
	const InvisibleImprovementChanges& getInvisibleImprovementChange(int iIndex) const;

	int getNumVisibleTerrainChanges() const;
	const InvisibleTerrainChanges& getVisibleTerrainChange(int iIndex) const;

	int getNumVisibleFeatureChanges() const;
	const InvisibleFeatureChanges& getVisibleFeatureChange(int iIndex) const;

	int getNumVisibleImprovementChanges() const;
	const InvisibleImprovementChanges& getVisibleImprovementChange(int iIndex) const;

	int getNumVisibleTerrainRangeChanges() const;
	const InvisibleTerrainChanges& getVisibleTerrainRangeChange(int iIndex) const;

	int getNumVisibleFeatureRangeChanges() const;
	const InvisibleFeatureChanges& getVisibleFeatureRangeChange(int iIndex) const;

	int getNumVisibleImprovementRangeChanges() const;
	const InvisibleImprovementChanges& getVisibleImprovementRangeChange(int iIndex) const;

	int getNumDistanceAttackCommunicabilityTypeChanges() const;
	const AfflictionLineChanges& getDistanceAttackCommunicabilityTypeChange(int iIndex) const;

	// TB Combat Mods End  TB SubCombat Mod end

	//Pediahelp
	int getQualifiedUnitCombatType(int i) const;
	int getNumQualifiedUnitCombatTypes() const;
	bool isQualifiedUnitCombatType(int i) const;
	void setQualifiedUnitCombatTypes();

	int getDisqualifiedUnitCombatType(int i) const;
	int getNumDisqualifiedUnitCombatTypes() const;
	void setDisqualifiedUnitCombatTypes();

	bool hasNegativeEffects() const;

protected:
	bool m_bCanMovePeaks;
	//	Koshling - enhanced mountaineering mode to differentiate between ability to move through
	//	mountains, and ability to lead a stack through mountains
	bool m_bCanLeadThroughPeaks;
	int m_iNumPromotionOverwrites;
	//std::vector<PromotionTypes> m_vPromotionOverwrites;
	TechTypes m_iObsoleteTech;
	int m_iControlPoints;
	int m_iCommandRange;
	bool m_bZoneOfControl;
	int m_iIgnoreTerrainDamage;
	int m_zobristValue;
public:
	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvPromotionInfo* pClassInfo);
	void copyNonDefaultsReadPass2(CvPromotionInfo* pClassInfo, CvXMLLoadUtility* pXML, bool bOver = false);

private:
	CvPropertyManipulators m_PropertyManipulators;

//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:
	int m_iLayerAnimationPath;
	int m_iPrereqPromotion;
	int m_iPrereqOrPromotion1;
	int m_iPrereqOrPromotion2;

	TechTypes m_iTechPrereq;
	int m_iStateReligionPrereq;
	int m_iMinEraType;
	int m_iMaxEraType;
	int m_iVisibilityChange;
	int m_iMovesChange;
	int m_iMoveDiscountChange;
	int m_iAirRangeChange;
	int m_iInterceptChange;
	int m_iEvasionChange;
	int m_iWithdrawalChange;
	int m_iCargoChange;
	int m_iSMCargoChange;
	int m_iSMCargoVolumeChange;
	int m_iSMCargoVolumeModifierChange;
	int m_iCollateralDamageChange;
	int m_iBombardRateChange;
	int m_iFirstStrikesChange;
	int m_iChanceFirstStrikesChange;
	int m_iEnemyHealChange;
	int m_iNeutralHealChange;
	int m_iFriendlyHealChange;
	int m_iSameTileHealChange;
	int m_iAdjacentTileHealChange;
	int m_iCombatPercent;
	int m_iCityAttackPercent;
	int m_iCityDefensePercent;
	int m_iHillsAttackPercent;
	int m_iHillsDefensePercent;
	int m_iHillsWorkPercent;
	int m_iWorkRatePercent;
	int m_iCommandType;
	int m_iRevoltProtection;
	int m_iCollateralDamageProtection;
	int m_iPillageChange;
	int m_iUpgradeDiscount;
	int m_iExperiencePercent;
	int m_iKamikazePercent;

	int m_iAirCombatLimitChange;
	int m_iCelebrityHappy;
	int m_iCollateralDamageLimitChange;
	int m_iCollateralDamageMaxUnitsChange;
	int m_iCombatLimitChange;
	int m_iExtraDropRange;

	int m_iSurvivorChance;
	int m_iVictoryAdjacentHeal;
	int m_iVictoryHeal;
	int m_iVictoryStackHeal;
	bool m_bDefensiveVictoryMove;
	bool m_bFreeDrop;
	bool m_bOffensiveVictoryMove;
	bool m_bOneUp;

	bool m_bPillageEspionage;
	bool m_bPillageMarauder;
	bool m_bPillageOnMove;
	bool m_bPillageOnVictory;
	bool m_bPillageResearch;

	bool m_bLeader;
	bool m_bBlitz;
	bool m_bAmphib;
	bool m_bRiver;
	bool m_bEnemyRoute;
	bool m_bAlwaysHeal;
	bool m_bHillsDoubleMove;
	bool m_bImmuneToFirstStrikes;

	CvString m_szSound;

	// Arrays
	int* m_piTerrainAttackPercent;
	int* m_piTerrainDefensePercent;
	int* m_piFeatureAttackPercent;
	int* m_piFeatureDefensePercent;
	int* m_piUnitCombatModifierPercent;
	int* m_piDomainModifierPercent;
	//ls612: Terrain Work Modifiers
	int* m_piTerrainWorkPercent;
	int* m_piFeatureWorkPercent;

	bool* m_pbTerrainDoubleMove;
	bool* m_pbFeatureDoubleMove;
	bool* m_pbUnitCombat;

	//TB Combat Mods Begin TB SubCombat Mod begin
	//Text Strings
	CvWString m_szRenamesUnitTo;
	//Textual References
	PromotionLineTypes m_ePromotionLine;
	UnitCombatTypes m_eReplacesUnitCombat;
	DomainTypes m_eDomainCargoChange;
	SpecialUnitTypes m_eSpecialCargoChange;
	SpecialUnitTypes m_eSpecialCargoPrereq;
	SpecialUnitTypes m_eSMSpecialCargoChange;
	SpecialUnitTypes m_eSMSpecialCargoPrereq;
	SpecialUnitTypes m_eSMNotSpecialCargoChange;
	SpecialUnitTypes m_eSMNotSpecialCargoPrereq;
	SpecialUnitTypes m_eSetSpecialUnit;
	//integers
	int m_iAttackCombatModifierChange;
	int m_iDefenseCombatModifierChange;
	int m_iPursuitChange;
	int m_iEarlyWithdrawChange;
	int m_iVSBarbsChange;
	int m_iArmorChange;
	int m_iPunctureChange;
	int m_iOverrunChange;
	int m_iRepelChange;
	int m_iFortRepelChange;
	int m_iRepelRetriesChange;
	int m_iUnyieldingChange;
	int m_iKnockbackChange;
	int m_iKnockbackRetriesChange;
#ifdef BATTLEWORN
	int m_iStrAdjperRndChange;
	int m_iStrAdjperAttChange;
	int m_iStrAdjperDefChange;
	int m_iWithdrawAdjperAttChange;
#endif // BATTLEWORN
	int m_iUnnerveChange;
	int m_iEncloseChange;
	int m_iLungeChange;
	int m_iDynamicDefenseChange;
	int m_iStrengthChange;
	int m_iLinePriority;
	int m_iFortitudeChange;
	int m_iDamageperTurn;
	int m_iStrAdjperTurn;
	int m_iWeakenperTurn;
#ifdef STRENGTH_IN_NUMBERS
	int m_iFrontSupportPercentChange;
	int m_iShortRangeSupportPercentChange;
	int m_iMediumRangeSupportPercentChange;
	int m_iLongRangeSupportPercentChange;
	int m_iFlankSupportPercentChange;
#endif
	int m_iDodgeModifierChange;
	int m_iPrecisionModifierChange;
	int m_iPowerShotsChange;
	int m_iPowerShotCombatModifierChange;
	int m_iPowerShotPunctureModifierChange;
	int m_iPowerShotPrecisionModifierChange;
	int m_iPowerShotCriticalModifierChange;
	int m_iCriticalModifierChange;
	int m_iEnduranceChange;
	int m_iRoundStunProbChange;
	int m_iPoisonProbabilityModifierChange;

	int m_iCaptureProbabilityModifierChange;
	int m_iCaptureResistanceModifierChange;

	//WorkRateMod
	int m_iPeaksWorkPercent;

	int m_iBreakdownChanceChange;
	int m_iBreakdownDamageChange;
	int m_iTauntChange;
	int m_iMaxHPChange;
	int m_iStrengthModifier;
	int m_iQualityChange;
	int m_iGroupChange;
	int m_iLevelPrereq;
	int m_iDamageModifierChange;

	int m_iUpkeepModifier;
	int m_iExtraUpkeep100;

	int m_iRBombardDamageChange;
	int m_iRBombardDamageLimitChange;
	int m_iRBombardDamageMaxUnitsChange;
	int m_iDCMBombRangeChange;
	int m_iDCMBombAccuracyChange;
	int m_iCombatModifierPerSizeMoreChange;
	int m_iCombatModifierPerSizeLessChange;
	int m_iCombatModifierPerVolumeMoreChange;
	int m_iCombatModifierPerVolumeLessChange;
	int m_iSelfHealModifier;
	int m_iNumHealSupport;
	int m_iExcileChange;
	int m_iPassageChange;
	int m_iNoNonOwnedCityEntryChange;
	int m_iBarbCoExistChange;
	int m_iBlendIntoCityChange;
	int m_iUpgradeAnywhereChange;
	int m_iInsidiousnessChange;
	int m_iInvestigationChange;
	int m_iAssassinChange;
	int m_iStealthStrikesChange;
	int m_iStealthCombatModifierChange;
	int m_iStealthDefenseChange;
	int m_iDefenseOnlyChange;
	int m_iNoInvisibilityChange;
	int m_iTrapDamageMax;
	int m_iTrapDamageMin;
	int m_iTrapComplexity;
	int m_iNumTriggers;
	int m_iTriggerBeforeAttackChange;
	int m_iHiddenNationalityChange;
	int m_iAnimalIgnoresBordersChange;
	int m_iNoDefensiveBonusChange;
	int m_iGatherHerdChange;
	int m_iReligiousCombatModifierChange;
	//booleans
	bool m_bStampedeChange;
	bool m_bRemoveStampede;
	bool m_bOnslaughtChange;
	bool m_bEquipment;
#ifdef OUTBREAKS_AND_AFFLICTIONS
	bool m_bAffliction;
#endif
	bool m_bParalyze;
	bool m_bMakesDamageCold;
	bool m_bMakesDamageNotCold;
	bool m_bAddsColdImmunity;
	bool m_bRemovesColdImmunity;
	bool m_bCritical;
	bool m_bAttackOnlyCitiesAdd;
	bool m_bAttackOnlyCitiesSubtract;
	bool m_bIgnoreNoEntryLevelAdd;
	bool m_bIgnoreNoEntryLevelSubtract;
	bool m_bIgnoreZoneofControlAdd;
	bool m_bIgnoreZoneofControlSubtract;
	bool m_bFliesToMoveAdd;
	bool m_bFliesToMoveSubtract;
	bool m_bZeroesXP;
	bool m_bForOffset;
	bool m_bCargoPrereq;
	bool m_bRBombardPrereq;
	bool m_bNoSelfHeal;
	bool m_bSetOnHNCapture;
	bool m_bSetOnInvestigated;
	bool m_bStatus;
	bool m_bPrereqNormInvisible;
	bool m_bPlotPrereqsKeepAfter;
	bool m_bRemoveAfterSet;
	bool m_bQuick;
	//bool m_bAnyAIWeightbyUnitCombat;
	//Arrays
	//int* m_piAIWeightbyUnitCombatTypes;
	// bool vectors without delayed resolution
	std::vector<int> m_aiSubCombatChangeTypes;
	std::vector<int> m_aiRemovesUnitCombatTypes;
	std::vector<int> m_aiOnGameOptions;
	std::vector<int> m_aiNotOnGameOptions;
	std::vector<int> m_aiFreetoUnitCombats;
	std::vector<int> m_aiNotOnUnitCombatTypes;
	std::vector<int> m_aiNotOnDomainTypes;
	std::vector<int> m_aiNoAutoEquiptoCombatClassTypes;
	std::vector<MapCategoryTypes> m_aeMapCategoryTypes;
	std::vector<BonusTypes> m_aiPrereqBonusTypes;
	std::vector<int> m_aiAddsBuildTypes;
	std::vector<int> m_aiNegatesInvisibilityTypes;
	std::vector<int> m_aiPrereqTerrainTypes;
	std::vector<int> m_aiPrereqFeatureTypes;
	std::vector<int> m_aiPrereqImprovementTypes;
	std::vector<int> m_aiPrereqPlotBonusTypes;
	std::vector<int> m_aiPrereqLocalBuildingTypes;
	std::vector<int> m_aiTrapSetWithPromotionTypes;
	std::vector<int> m_aiTrapImmunityUnitCombatTypes;
	std::vector<int> m_aiTargetUnitCombatTypes;
	// int vectors utilizing pairing without delayed resolution
	UnitCombatModifierArray m_aFlankingStrengthbyUnitCombatTypeChange;
	TerrainModifierArray m_aWithdrawOnTerrainTypesChange;
	FeatureModifierArray m_aWithdrawOnFeatureTypesChange;
	UnitCombatModifierArray m_aWithdrawVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aPursuitVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aRepelVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aKnockbackVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aPunctureVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aArmorVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aDodgeVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aPrecisionVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aCriticalVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aRoundStunVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aTrapDisableUnitCombatTypes;
	UnitCombatModifierArray m_aTrapAvoidanceUnitCombatTypes;
	UnitCombatModifierArray m_aTrapTriggerUnitCombatTypes;
#ifdef OUTBREAKS_AND_AFFLICTIONS
	AidArray m_aAidChanges;
#endif

	BuildModifierArray m_aBuildWorkRateModifierChangeTypes;
	InvisibilityArray m_aVisibilityIntensityChangeTypes;
	InvisibilityArray m_aInvisibilityIntensityChangeTypes;
	InvisibilityArray m_aVisibilityIntensityRangeChangeTypes;
	// int vector utilizing struct with delayed resolution
	std::vector<UnitCombatModifier> m_aAIWeightbyUnitCombatTypes;
#ifdef OUTBREAKS_AND_AFFLICTIONS
	std::vector<int> m_aiCureAfflictionChangeTypes;
	std::vector<PromotionLineModifier> m_aAfflictionFortitudeChangeModifiers;
	std::vector<AfflictOnAttackChange> m_aAfflictOnAttackChangeTypes;
#endif
	std::vector<HealUnitCombat> m_aHealUnitCombatChangeTypes;
	std::vector<InvisibleTerrainChanges> m_aInvisibleTerrainChanges;
	std::vector<InvisibleFeatureChanges> m_aInvisibleFeatureChanges;
	std::vector<InvisibleImprovementChanges> m_aInvisibleImprovementChanges;
	std::vector<InvisibleFeatureChanges> m_aVisibleFeatureChanges;
	std::vector<InvisibleImprovementChanges> m_aVisibleImprovementChanges;
	std::vector<InvisibleTerrainChanges> m_aVisibleTerrainChanges;
	std::vector<InvisibleFeatureChanges> m_aVisibleFeatureRangeChanges;
	std::vector<InvisibleImprovementChanges> m_aVisibleImprovementRangeChanges;
	std::vector<InvisibleTerrainChanges> m_aVisibleTerrainRangeChanges;
	std::vector<AfflictionLineChanges> m_aDistanceAttackCommunicabilityTypeChanges;
	//TB Combat Mods End  TB SubCombat Mod end

	//Pediahelp
	std::vector<int> m_aiQualifiedUnitCombatTypes;
	std::vector<int> m_disqualifiedUnitCombatTypes;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvMissionInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMissionInfo : public CvHotkeyInfo
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvMissionInfo();
	explicit CvMissionInfo(const char* szType);
	virtual ~CvMissionInfo();

	DllExport int getTime() const;

	bool isSound() const;
	DllExport bool isTarget() const;
	bool isBuild() const;
	bool getVisible() const;
	DllExport EntityEventTypes getEntityEvent() const;

	const char* getWaypoint() const;		// effect type, Exposed to Python

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvMissionInfo* pClassInfo);

	// Should not be needed to checksum this as this will likely not get changed without DLL changes
	//void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iTime;

	bool m_bSound;
	bool m_bTarget;
	bool m_bBuild;
	bool m_bVisible;
	EntityEventTypes m_eEntityEvent;

	CvString m_szWaypoint;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvControlInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvControlInfo : public CvHotkeyInfo
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvControlInfo();
	virtual ~CvControlInfo();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvControlInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCommandInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCommandInfo : public CvHotkeyInfo
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvCommandInfo();
	virtual ~CvCommandInfo();

	int getAutomate() const;

	bool getConfirmCommand() const;
	bool getVisible() const;
	bool getAll() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvCommandInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:
	int m_iAutomate;

	bool m_bConfirmCommand;
	bool m_bVisible;
	bool m_bAll;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvAutomateInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAutomateInfo : public CvHotkeyInfo
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvAutomateInfo();
	virtual ~CvAutomateInfo();

	int getCommand() const;
	int getAutomate() const;

	bool getConfirmCommand() const;
	bool getVisible() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvAutomateInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:
	int m_iCommand;
	int m_iAutomate;

	bool m_bConfirmCommand;
	bool m_bVisible;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvActionInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvActionInfo
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvActionInfo();
	virtual ~CvActionInfo();

	int getMissionData() const;
	int getCommandData() const;

	int getAutomateType() const;
	int getInterfaceModeType() const;
	int getMissionType() const;
	int getCommandType() const;
	int getControlType() const;
	int getOriginalIndex() const;
	void setOriginalIndex(int i) { m_iOriginalIndex = i; }

	bool isConfirmCommand() const;
	DllExport bool isVisible() const;
	DllExport ActionSubTypes getSubType() const;
	void setSubType(ActionSubTypes eSubType) { m_eSubType = eSubType; }

	// functions to replace the CvInfoBase calls
	const char* getType() const;
	const wchar_t* getDescription() const;
	const wchar_t* getCivilopedia() const;
	const wchar_t* getHelp() const;
	const wchar_t* getStrategy() const;

	virtual const char* getButton() const;
	const wchar_t* getTextKeyWide() const;

	// functions to replace the CvHotkey calls
	int getActionInfoIndex() const;
	DllExport int getHotKeyVal() const;
	DllExport int getHotKeyPriority() const;
	DllExport int getHotKeyValAlt() const;
	DllExport int getHotKeyPriorityAlt() const;
	int getOrderPriority() const;

	DllExport bool isAltDown() const;
	DllExport bool isShiftDown() const;
	DllExport bool isCtrlDown() const;
	DllExport bool isAltDownAlt() const;
	DllExport bool isShiftDownAlt() const;
	DllExport bool isCtrlDownAlt() const;

	const char* getHotKey() const;

	std::wstring getHotKeyDescription() const;

//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iOriginalIndex;
	ActionSubTypes m_eSubType;

private:
	CvHotkeyInfo* getHotkeyInfo() const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvUnitInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoUnit;
class CvUnitInfo : public CvHotkeyInfo
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:
	CvUnitInfo();
	virtual ~CvUnitInfo();

	int getMaxGlobalInstances() const;
	int getMaxPlayerInstances() const;
	bool isUnlimitedException() const;
	bool isCivilizationUnit(const PlayerTypes ePlayer=NO_PLAYER) const;

	int getInstanceCostModifier() const;
	int getAIWeight() const;
	int getProductionCost() const;
	int getHurryCostModifier() const;
	int getAdvancedStartCost() const;
	int getMinAreaSize() const;
	int getMoves() const;
	int getAirRange() const;
	int getAirUnitCap() const;
	int getDropRange() const;
	int getNukeRange() const;
	int getWorkRate() const;
	int getBaseDiscover() const;
	int getDiscoverMultiplier() const;
	int getBaseHurry() const;
	int getHurryMultiplier() const;
	int getBaseTrade() const;
	int getTradeMultiplier() const;
	int getGreatWorkCulture() const;
	int getEspionagePoints() const;
	int getCombat() const;

	void setCombat(int iNum);
	int getCombatLimit() const;
	//TB Tweaked for Size Matters
	int getAirCombat() const;

	int getAirCombatLimit() const;
	int getXPValueAttack() const;
	int getXPValueDefense() const;
	int getFirstStrikes() const;
	int getChanceFirstStrikes() const;
	int getInterceptionProbability() const;
	//ls612: Advanced Nuke Interception
	//int getNukeInterceptionProbability() const;
	//int getNukeInterceptionRange() const;
	int getEvasionProbability() const;
	int getWithdrawalProbability() const;
	int getCollateralDamage() const;
	int getCollateralDamageLimit() const;
	int getCollateralDamageMaxUnits() const;
	int getCityAttackModifier() const;
	int getCityDefenseModifier() const;
	int getAnimalCombatModifier() const;
	int getHillsAttackModifier() const;
	int getHillsDefenseModifier() const;
	int getBombRate() const;
	int getBombardRate() const;
	int getSpecialCargo() const;
	int getSMSpecialCargo() const;
	int getSMNotSpecialCargo() const;
	int getDomainCargo() const;
	int getCargoSpace() const;
	int getSMCargoSpace() const;
	int getSMCargoVolume() const;
	int getConscriptionValue() const;
	int getCultureGarrisonValue() const;
	int getBaseUpkeep() const;
	int getAssetValue() const;
	int getPowerValue() const;
	int getSpecialUnitType() const;
	int getUnitCaptureType() const;
	int getUnitCombatType() const;
	DomainTypes getDomainType() const;
	UnitAITypes getDefaultUnitAIType() const;
	int getInvisibleType() const;
	int getSeeInvisibleType(int i) const;
	int getNumSeeInvisibleTypes() const;
	int getAdvisorType() const;
	int getMaxStartEra() const;
	int getObsoleteTech() const;
	bool isStateReligion() const;
	int getPrereqGameOption() const;
	int getNotGameOption() const;
	int getHolyCity() const;
	int getReligionType() const;
	int getStateReligion() const;
	int getPrereqReligion() const;
	int getPrereqCorporation() const;
	int getPrereqOrBuildingsNum() const;
	BuildingTypes getPrereqOrBuilding(int i) const;
	int getPrereqAndTech() const;
	int getPrereqAndBonus() const;
	// the initial number of individuals in the unit group
	int getGroupSize() const;
	// the number of UnitMeshGroups for this unit
	int getGroupDefinitions() const;
	int getMeleeWaveSize() const;
	int getRangedWaveSize() const;
	int getNumUnitNames() const;
	int getCommandType() const;
	void setCommandType(int iNewType);
	//	This really belongs on CvInfoBase but you can't change the size of that
	//	object without crashing the core engine :-(
	inline int	getZobristValue() const { return m_zobristValue; }

	bool isFoodProduction() const;
	bool isNoBadGoodies() const;
	bool isOnlyDefensive() const;
	bool isNoCapture() const;
	bool isQuickCombat() const;
	bool isRivalTerritory() const;
	bool isMilitaryHappiness() const;
	bool isMilitarySupport() const;
	bool isMilitaryProduction() const;
	bool isPillage() const;
	bool isSpy() const;
	bool isSabotage() const;
	bool isDestroy() const;
	bool isStealPlans() const;
	bool isInvestigate() const;
	bool isCounterSpy() const;
	bool isFound() const;
	bool isGoldenAge() const;
	bool isInvisible() const;
	void setInvisible(bool bEnable) ;
	bool isFirstStrikeImmune() const;
	bool isNoDefensiveBonus() const;
	bool isIgnoreBuildingDefense() const;
	bool isCanMoveImpassable() const;
	bool isCanMoveAllTerrain() const;
	bool isFlatMovementCost() const;
	bool isIgnoreTerrainCost() const;
	bool isNukeImmune() const;
	bool isMechUnit() const;
	bool isRenderBelowWater() const;
	bool isRenderAlways() const;
	bool isSuicide() const;
	bool isLineOfSight() const;
	bool isHiddenNationality() const;
	bool isAlwaysHostile() const;
	bool isFreeDrop() const;
	bool isNoRevealMap() const;
	bool isInquisitor() const;
	//ls612: Can't enter non-Owned cities
	bool isNoNonOwnedEntry() const;

	void setPowerValue(int iNewValue);
	int getPrereqVicinityBonus() const;
	bool isRequiresStateReligionInCity() const;
	bool isWorkerTrade() const;
	bool isMilitaryTrade() const;
	bool isForceUpgrade() const;
	bool isGreatGeneral() const;
	bool isSlave() const;
	bool getPassableRouteNeeded(int i) const;
	int getBaseFoodChange() const;
	int getControlPoints() const;
	int getCommandRange() const;
	const char* getClassicalArtDefineTag(int i, UnitArtStyleTypes eStyle) const;
	void setClassicalArtDefineTag(int i, const char* szVal);
	const char* getRennArtDefineTag(int i, UnitArtStyleTypes eStyle) const;
	void setRennArtDefineTag(int i, const char* szVal);
	const char* getIndustrialArtDefineTag(int i, UnitArtStyleTypes eStyle) const;
	void setIndustrialArtDefineTag(int i, const char* szVal);
	const char* getFutureArtDefineTag(int i, UnitArtStyleTypes eStyle) const;
	void setFutureArtDefineTag(int i, const char* szVal);

	CvWString getCivilizationName(int i) const;
	int getCivilizationNamesVectorSize() const;
	CvWString getCivilizationNamesNamesVectorElement(int i) const;
	CvWString getCivilizationNamesValuesVectorElement(int i) const;

	//TB Combat Mod Begin  TB SubCombat Mod begin
	//Functions
	void setReligionSubCombat();
	void setCultureSubCombat();
	int getEraInfo() const;
	void setEraSubCombat();

	int getAttackCombatModifier() const;
	int getDefenseCombatModifier() const;
	int getPursuit() const;
	int getEarlyWithdraw() const;
	int getVSBarbs() const;
	int getArmor() const;
	int getPuncture() const;
	int getOverrun() const;
	int getRepel() const;
	int getFortRepel() const;
	int getRepelRetries() const;
	int getUnyielding() const;
	int getKnockback() const;
	int getKnockbackRetries() const;
#ifdef BATTLEWORN
	int getStrAdjperRnd() const;
	int getStrAdjperAtt() const;
	int getStrAdjperDef() const;
	int getWithdrawAdjperAtt() const;
#endif // BATTLEWORN
	int getUnnerve() const;
	int getEnclose() const;
	int getLunge() const;
	int getDynamicDefense() const;
	int getFortitude() const;
#ifdef STRENGTH_IN_NUMBERS
	int getFrontSupportPercent() const;
	int getShortRangeSupportPercent() const;
	int getMediumRangeSupportPercent() const;
	int getLongRangeSupportPercent() const;
	int getFlankSupportPercent() const;
#endif // STRENGTH_IN_NUMBERS
	int getDodgeModifier() const;
	int getPrecisionModifier() const;
	int getPowerShots() const;
	int getPowerShotCombatModifier() const;
	int getPowerShotPunctureModifier() const;
	int getPowerShotPrecisionModifier() const;
	int getPowerShotCriticalModifier() const;
	int getCriticalModifier() const;
	int getEndurance() const;
	int getRoundStunProb() const;
	int getPoisonProbabilityModifier() const;
	int getCaptureProbabilityModifier() const;
	int getCaptureResistanceModifier() const;
	int getHillsWorkModifier() const;
	int getPeaksWorkModifier() const;
	int getBreakdownChance() const;
	int getBreakdownDamage() const;
	int getTaunt() const;
	int getMaxHP(bool bForLoad = false) const;
	int getDamageModifier() const;
	int getRBombardDamage() const;
	int getRBombardDamageLimit() const;
	int getRBombardDamageMaxUnits() const;
	int getCombatModifierPerSizeMore() const;
	int getCombatModifierPerSizeLess() const;
	int getCombatModifierPerVolumeMore() const;
	int getCombatModifierPerVolumeLess() const;
	int getSelfHealModifier() const;
	int getNumHealSupport() const;
	int getInsidiousness() const;
	int getInvestigation() const;
	int getStealthStrikes() const;
	int getStealthCombatModifier() const;
	int getTrapDamageMax() const;
	int getTrapDamageMin() const;
	int getTrapComplexity() const;
	int getNumTriggers() const;
	int getAggression() const;
	int getAnimalIgnoresBorders() const;
	int getReligiousCombatModifier() const;

	bool isStampede() const;
	bool isOnslaught() const;
	bool isDealsColdDamage() const;
	bool isColdImmune() const;
	bool isAttackOnlyCities() const;
	bool isIgnoreNoEntryLevel() const;
	bool isIgnoreZoneofControl() const;
	bool isFliesToMove() const;
	bool isRBombardForceAbility() const;
	bool isNoSelfHeal() const;
	bool isExcile() const;
	bool isPassage() const;
	bool isNoNonOwnedCityEntry() const;
	bool isBarbCoExist() const;
	bool isBlendIntoCity() const;
	bool isUpgradeAnywhere() const;
	bool isAssassin() const;
	bool isStealthDefense() const;
	bool isNoInvisibility() const;
	bool isTriggerBeforeAttack() const;
	bool isAnimal() const;
	bool isWildAnimal() const;
	bool canAnimalIgnoresBorders() const;
	bool canAnimalIgnoresImprovements() const;
	bool canAnimalIgnoresCities() const;
	bool isNoNonTypeProdMods() const;
	bool isGatherHerd() const;

	UnitCombatTypes getSubCombatType(int i) const;
	int getNumSubCombatTypes() const;
	bool isSubCombatType(UnitCombatTypes e) const;
	const std::vector<UnitCombatTypes>& getSubCombatTypes() const;

	int getCureAfflictionType(int i) const;
	int getNumCureAfflictionTypes() const;
	bool isCureAfflictionType(int i) const;

	int getHealAsType(int i) const;
	int getNumHealAsTypes() const;
	bool isHealAsType(int i) const;
	void setHealAsTypes();

	bool isTerrainImpassableType(TerrainTypes e) const;
	const std::vector<TerrainTypes>& getImpassableTerrains() const { return m_vTerrainImpassableTypes; }

	bool isFeatureImpassableType(FeatureTypes e) const;
	const std::vector<FeatureTypes>& getImpassableFeatures() const { return m_vFeatureImpassableTypes; }

	const std::vector<MapCategoryTypes>& getMapCategories() const { return m_aeMapCategoryTypes; }

	int getTrapSetWithPromotionType(int i) const;
	int getNumTrapSetWithPromotionTypes() const;
	bool isTrapSetWithPromotionType(int i) const;

	int getTrapImmunityUnitCombatType(int i) const;
	int getNumTrapImmunityUnitCombatTypes() const;
	bool isTrapImmunityUnitCombatType(int i) const;

#ifdef OUTBREAKS_AND_AFFLICTIONS
	int getNumAfflictionFortitudeModifiers() const;
	const PromotionLineModifier& getAfflictionFortitudeModifier(int iAffliction) const;

	int getNumAfflictOnAttackTypes() const;
	const AfflictOnAttack& getAfflictOnAttackType(int iAfflictionLine) const;

	int getNumAidChanges() const;
	int getAidChange(int iProperty) const;
	bool isAidChange(int iProperty) const;
#endif // OUTBREAKS_AND_AFFLICTIONS

	int getNumHealUnitCombatTypes() const;
	const HealUnitCombat& getHealUnitCombatType(int iUnitCombat) const;

	int getNumGroupSpawnUnitCombatTypes() const;
	const GroupSpawnUnitCombat& getGroupSpawnUnitCombatType(int iIndex) const;

	int getNumInvisibleTerrainChanges() const;
	const InvisibleTerrainChanges& getInvisibleTerrainChange(int iIndex) const;

	int getNumInvisibleFeatureChanges() const;
	const InvisibleFeatureChanges& getInvisibleFeatureChange(int iIndex) const;

	int getNumInvisibleImprovementChanges() const;
	const InvisibleImprovementChanges& getInvisibleImprovementChange(int iIndex) const;

	int getNumVisibleTerrainChanges() const;
	const InvisibleTerrainChanges& getVisibleTerrainChange(int iIndex) const;

	int getNumVisibleFeatureChanges() const;
	const InvisibleFeatureChanges& getVisibleFeatureChange(int iIndex) const;

	int getNumVisibleImprovementChanges() const;
	const InvisibleImprovementChanges& getVisibleImprovementChange(int iIndex) const;

	int getNumVisibleTerrainRangeChanges() const;
	const InvisibleTerrainChanges& getVisibleTerrainRangeChange(int iIndex) const;

	int getNumVisibleFeatureRangeChanges() const;
	const InvisibleFeatureChanges& getVisibleFeatureRangeChange(int iIndex) const;

	int getNumDistanceAttackCommunicabilityTypeChanges() const;
	const InvisibleImprovementChanges& getVisibleImprovementRangeChange(int iIndex) const;

	int getNumVisibleImprovementRangeChanges() const;
	const AfflictionLineChanges& getDistanceAttackCommunicabilityTypeChange(int iIndex) const;

	int getNumEnabledCivilizationTypes() const;
	const EnabledCivilizations& getEnabledCivilizationType(int iIndex) const;

	int getNumFlankingStrikesbyUnitCombatTypes() const;
	int getFlankingStrengthbyUnitCombatType(int iUnitCombat) const;
	bool isFlankingStrikebyUnitCombatType(int iUnitCombat) const;

	int getNumWithdrawOnTerrainTypes() const;
	int getWithdrawOnTerrainType(int iTerrain) const;

	int getNumWithdrawOnFeatureTypes() const;
	int getWithdrawOnFeatureType(int iFeature) const;

	int getNumWithdrawVSUnitCombatTypes() const;
	int getWithdrawVSUnitCombatType(int iUnitCombat) const;
	bool isWithdrawVSUnitCombatType(int iUnitCombat) const;
	const UnitCombatModifierArray& getWithdrawVSUnitCombatTypes() const;

	int getNumPursuitVSUnitCombatTypes() const;
	int getPursuitVSUnitCombatType(int iUnitCombat, bool bForLoad = false) const;
	bool isPursuitVSUnitCombatType(int iUnitCombat, bool bForLoad = false) const;
	const UnitCombatModifierArray& getPursuitVSUnitCombatTypes() const;

	int getNumRepelVSUnitCombatTypes() const;
	int getRepelVSUnitCombatType(int iUnitCombat, bool bForLoad = false) const;
	bool isRepelVSUnitCombatType(int iUnitCombat, bool bForLoad = false) const;
	const UnitCombatModifierArray& getRepelVSUnitCombatTypes() const;

	int getNumKnockbackVSUnitCombatTypes() const;
	int getKnockbackVSUnitCombatType(int iUnitCombat, bool bForLoad = false) const;
	bool isKnockbackVSUnitCombatType(int iUnitCombat, bool bForLoad = false) const;
	const UnitCombatModifierArray& getKnockbackVSUnitCombatTypes() const;

	int getNumPunctureVSUnitCombatTypes() const;
	int getPunctureVSUnitCombatType(int iUnitCombat) const;
	bool isPunctureVSUnitCombatType(int iUnitCombat) const;
	const UnitCombatModifierArray& getPunctureVSUnitCombatTypes() const;

	int getNumArmorVSUnitCombatTypes() const;
	int getArmorVSUnitCombatType(int iUnitCombat) const;
	bool isArmorVSUnitCombatType(int iUnitCombat) const;
	const UnitCombatModifierArray& getArmorVSUnitCombatTypes() const;

	int getNumDodgeVSUnitCombatTypes() const;
	int getDodgeVSUnitCombatType(int iUnitCombat) const;
	bool isDodgeVSUnitCombatType(int iUnitCombat) const;
	const UnitCombatModifierArray& getDodgeVSUnitCombatTypes() const;

	int getNumPrecisionVSUnitCombatTypes() const;
	int getPrecisionVSUnitCombatType(int iUnitCombat) const;
	bool isPrecisionVSUnitCombatType(int iUnitCombat) const;
	const UnitCombatModifierArray& getPrecisionVSUnitCombatTypes() const;

	int getNumCriticalVSUnitCombatTypes() const;
	int getCriticalVSUnitCombatType(int iUnitCombat) const;
	bool isCriticalVSUnitCombatType(int iUnitCombat) const;
	const UnitCombatModifierArray& getCriticalVSUnitCombatTypes() const;

	int getNumRoundStunVSUnitCombatTypes() const;
	int getRoundStunVSUnitCombatType(int iUnitCombat) const;
	bool isRoundStunVSUnitCombatType(int iUnitCombat) const;
	const UnitCombatModifierArray& getRoundStunVSUnitCombatTypes() const;

	int getNumTrapDisableUnitCombatTypes() const;
	int getTrapDisableUnitCombatType(int iUnitCombat) const;
	bool isTrapDisableUnitCombatType(int iUnitCombat) const;
	const UnitCombatModifierArray& getTrapDisableUnitCombatTypes() const;

	int getNumTrapAvoidanceUnitCombatTypes() const;
	int getTrapAvoidanceUnitCombatType(int iUnitCombat) const;
	bool isTrapAvoidanceUnitCombatType(int iUnitCombat) const;
	const UnitCombatModifierArray& getTrapAvoidanceUnitCombatTypes() const;

	int getNumTrapTriggerUnitCombatTypes() const;
	int getTrapTriggerUnitCombatType(int iUnitCombat) const;
	bool isTrapTriggerUnitCombatType(int iUnitCombat) const;
	const UnitCombatModifierArray& getTrapTriggerUnitCombatTypes() const;

	int getNumVisibilityIntensityTypes() const;
	int getVisibilityIntensityType(int iInvisibility) const;
	bool isVisibilityIntensityType(int iInvisibility) const;
	const InvisibilityArray& getVisibilityIntensityTypes() const;

	int getNumInvisibilityIntensityTypes() const;
	int getInvisibilityIntensityType(int iInvisibility) const;
	bool isInvisibilityIntensityType(int iInvisibility) const;
	const InvisibilityArray& getInvisibilityIntensityTypes() const;

	int getNumVisibilityIntensityRangeTypes() const;
	int getVisibilityIntensityRangeType(int iInvisibility) const;
	bool isVisibilityIntensityRangeType(int iInvisibility) const;
	const InvisibilityArray& getVisibilityIntensityRangeTypes() const;

	int getNumTerrainWorkRateModifierTypes() const;
	int getTerrainWorkRateModifierType(int iTerrain) const;
	bool isTerrainWorkRateModifierType(int iTerrain) const;

	int getNumFeatureWorkRateModifierTypes() const;
	int getFeatureWorkRateModifierType(int iFeature) const;
	bool isFeatureWorkRateModifierType(int iFeature) const;

	int getNumBuildWorkRateModifierTypes() const;
	int getBuildWorkRateModifierType(int iBuild) const;
	bool isBuildWorkRateModifierType(int iBuild) const;

	bool hasUnitCombat(UnitCombatTypes eUnitCombat) const;
	void setTotalModifiedCombatStrengthDetails();
	int getCombatStrengthModifier() const;
	int getTotalModifiedCombatStrength100() const;
	int getTotalModifiedAirCombatStrength100() const;
	int getApproaching0Return(int i) const;
	int getBaseGroupRank() const;
	void setBaseCargoVolume();
	int getBaseCargoVolume() const;
	void setBaseSizeMattersZeroPoints();
	int getSMRankTotal() const;
	int getSMVolumetricRankTotal() const;

	void setSM();
	bool isQualifiedPromotionType(int i) const;
	bool setQualifiedPromotionType(const int iPromo, std::vector<int>& checklist);
	void setQualifiedPromotionTypes();
	void setCanAnimalIgnores();

	virtual const wchar_t* getExtraHoverText() const;

	void getCheckSum(uint32_t& iSum) const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

	const BoolExpr* getTrainCondition() const;

protected:
	int m_iMaxGlobalInstances;
	int m_iMaxPlayerInstances;
	bool m_bUnlimitedException;
	int m_iInstanceCostModifier;
	bool* m_pbPassableRouteNeeded;
	std::vector<BonusTypes> m_piPrereqOrVicinityBonuses;
	bool m_bWorkerTrade;
	bool m_bMilitaryTrade;
	bool m_bForceUpgrade;
	bool m_bGreatGeneral;
	bool m_bSlave;
	bool m_bRequiresStateReligionInCity;
	mutable bool* m_abHasCombatType;
	int m_iPrereqVicinityBonus;
	int m_iBaseFoodChange;
	int m_iControlPoints;
	int m_iCommandRange;
	int m_zobristValue;
	CvString* m_paszClassicalArtDefineTags;
	CvString* m_paszRennArtDefineTags;
	CvString* m_paszIndustrialArtDefineTags;
	CvString* m_paszFutureArtDefineTags;
	CvWString* m_paszCivilizationNames;
	std::vector<CvString> m_aszCivilizationNamesforPass3;
	std::vector<CvWString> m_aszCivilizationNamesValueforPass3;

	const BoolExpr* m_pExprTrainCondition;

public:
	int getDCMBombRange() const;
	int getDCMBombAccuracy() const;
	bool getDCMAirBomb1() const;
	bool getDCMAirBomb2() const;
	bool getDCMAirBomb3() const;
	bool getDCMAirBomb4() const;
	bool getDCMAirBomb5() const;
	bool getDCMFighterEngage() const;

	float getUnitMaxSpeed() const;
	float getUnitPadTime() const;

	bool canAcquireExperience() const;

	const std::vector<BonusTypes>& getPrereqOrBonuses() const;
	const std::vector<BonusTypes>& getPrereqOrVicinityBonuses() const;

	const std::vector<TechTypes>& getPrereqAndTechs() const;

	// Arrays
	int getFlavorValue(int i) const;
	int getTerrainAttackModifier(int i) const;
	int getTerrainDefenseModifier(int i) const;
	int getFeatureAttackModifier(int i) const;
	int getFeatureDefenseModifier(int i) const;
	int getUnitAttackModifier(int i) const;
	int getUnitDefenseModifier(int i) const;
	int getUnitCombatModifier(int i) const;
	int getUnitCombatCollateralImmune(int i) const;
	int getDomainModifier(int i) const;
	int getBonusProductionModifier(int i) const;
	int getUnitGroupRequired(int i) const;
	int getReligionSpreads(int i) const;
	int getCorporationSpreads(int i) const;
	int getTerrainPassableTech(int i) const;
	int getFeaturePassableTech(int i) const;
	int getFlankingStrikeUnit(int i) const;

	bool isPrereqOrCivics(int i) const;

	const std::vector<BuildTypes>& getBuilds() const { return m_workerBuilds; }
	BuildTypes getBuild(int i) const;
	short getNumBuilds() const;
	bool hasBuild(BuildTypes e) const;

	int getPrereqAndBuilding(int i) const;
	int getNumPrereqAndBuildings() const;
	bool isPrereqAndBuilding(int i) const;
	bool isPrereqOrBuilding(int i) const;

	int getTargetUnit(int i) const;
	int getNumTargetUnits() const;
	bool isTargetUnit(int i) const;

	int getDefendAgainstUnit(int i) const;
	int getNumDefendAgainstUnits() const;
	bool isDefendAgainstUnit(int i) const;

	int getSupersedingUnit(int i) const;
	short getNumSupersedingUnits() const;
	bool isSupersedingUnit(int i) const;

	int getUnitUpgrade(int i) const;
	int getNumUnitUpgrades() const;
	bool isUnitUpgrade(int i) const;

	std::vector<int> getUnitUpgradeChain() const;
	void addUnitToUpgradeChain(int i);

	bool getTargetUnitCombat(int i) const;
	bool getDefenderUnitCombat(int i) const;
	bool getUnitAIType(int i) const;
	bool getNotUnitAIType(int i) const;
	bool getGreatPeoples(int i) const;

	int getBuildings(int i) const;
	bool getHasBuilding(int i) const;
	int getNumBuildings() const;

	bool getTerrainNative(int i) const;
	bool getFeatureNative(int i) const;
	bool getFreePromotions(int i) const;
	int getLeaderPromotion() const;
	int getLeaderExperience() const;

	const CvOutcomeList* getKillOutcomeList() const;
	int getNumActionOutcomes() const;
	const CvOutcomeList* getActionOutcomeList(int index) const;
	MissionTypes getActionOutcomeMission(int index) const;
	const CvOutcomeList* getActionOutcomeListByMission(MissionTypes eMission) const;
	const CvOutcomeMission* getOutcomeMission(int index) const;
	CvOutcomeMission* getOutcomeMissionByMission(MissionTypes eMission) const;

	const char* getEarlyArtDefineTag(int i, UnitArtStyleTypes eStyle) const;
	void setEarlyArtDefineTag(int i, const char* szVal);
	const char* getLateArtDefineTag(int i, UnitArtStyleTypes eStyle) const;
	void setLateArtDefineTag(int i, const char* szVal);
	const char* getMiddleArtDefineTag(int i, UnitArtStyleTypes eStyle) const;
	void setMiddleArtDefineTag(int i, const char* szVal);
	const char* getUnitNames(int i) const;
	const char* getFormationType() const;

	const char* getButton() const;
	void updateArtDefineButton();

	const CvArtInfoUnit* getArtInfo(int i, EraTypes eEra, UnitArtStyleTypes eStyle) const;

	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);
	bool readPass3();
	void copyNonDefaults(CvUnitInfo* pClassInfo);
	void copyNonDefaultsReadPass2(CvUnitInfo* pClassInfo, CvXMLLoadUtility* pXML, bool bOver = false);

private:
	CvPropertyManipulators m_PropertyManipulators;

protected:
	int m_iDCMBombRange;
	int m_iDCMBombAccuracy;
	bool m_bDCMFighterEngage;
	bool m_bDCMAirBomb1;
	bool m_bDCMAirBomb2;
	bool m_bDCMAirBomb3;
	bool m_bDCMAirBomb4;
	bool m_bDCMAirBomb5;

	int m_iAIWeight;
	int m_iProductionCost;
	int m_iHurryCostModifier;
	int m_iAdvancedStartCost;
	int m_iMinAreaSize;
	int m_iMoves;
	int m_iAirRange;
	int m_iAirUnitCap;
	int m_iDropRange;
	int m_iNukeRange;
	int m_iWorkRate;
	int m_iBaseDiscover;
	int m_iDiscoverMultiplier;
	int m_iBaseHurry;
	int m_iHurryMultiplier;
	int m_iBaseTrade;
	int m_iTradeMultiplier;
	int m_iGreatWorkCulture;
	int m_iEspionagePoints;
	int m_iCombat;
	int m_iCombatLimit;
	int m_iAirCombat;
	int m_iAirCombatLimit;
	int m_iXPValueAttack;
	int m_iXPValueDefense;
	int m_iFirstStrikes;
	int m_iChanceFirstStrikes;
	int m_iInterceptionProbability;
	//ls612: Advanced Nuke Interception
	//int m_iNukeInterceptionProbability;
	//int m_iNukeInterceptionRange;
	int m_iEvasionProbability;
	int m_iWithdrawalProbability;
	int m_iCollateralDamage;
	int m_iCollateralDamageLimit;
	int m_iCollateralDamageMaxUnits;
	int m_iCityAttackModifier;
	int m_iCityDefenseModifier;
	int m_iAnimalCombatModifier;
	int m_iHillsAttackModifier;
	int m_iHillsDefenseModifier;
	int m_iBombRate;
	int m_iBombardRate;
	int m_iSpecialCargo;
	int m_iSMSpecialCargo;
	int m_iSMNotSpecialCargo;
	int m_iDomainCargo;
	int m_iCargoSpace;
	int m_iSMCargoSpace;
	int m_iSMCargoVolume;
	int m_iConscriptionValue;
	int m_iCultureGarrisonValue;
	int m_iBaseUpkeep;
	int m_iAssetValue;
	int m_iPowerValue;
	int m_iSpecialUnitType;
	int m_iUnitCaptureType;
	int m_iUnitCombatType;
	DomainTypes m_iDomainType;
	UnitAITypes m_iDefaultUnitAIType;
	int m_iInvisibleType;
	int m_iAdvisorType;
	int m_iMaxStartEra;
	int m_iObsoleteTech;
	bool m_bStateReligion;
	int m_iPrereqGameOption;
	int m_iNotGameOption;
	int m_iHolyCity;
	int m_iReligionType;
	int m_iStateReligion;
	int m_iPrereqReligion;
	int m_iPrereqCorporation;
	int m_iPrereqAndTech;
	int m_iPrereqAndBonus;

	std::vector<BuildTypes> m_workerBuilds;
	std::vector<int> m_aiPrereqAndBuildings;
	std::vector<int> m_aiPrereqOrBuildings;
	std::vector<int> m_aiTargetUnit;
	std::vector<int> m_aiDefendAgainstUnit;
	std::vector<int> m_aiSupersedingUnits;
	std::vector<int> m_aiUnitUpgrades;
	std::vector<int> m_aiUnitUpgradeChain;

	int m_iGroupSize;
	int m_iGroupDefinitions;
	int m_iUnitMeleeWaveSize;
	int m_iUnitRangedWaveSize;
	int m_iNumUnitNames;
	int m_iCommandType;
	int m_iLeaderExperience;

	bool m_bFoodProduction;
	bool m_bNoBadGoodies;
	bool m_bOnlyDefensive;
	bool m_bNoCapture;
	bool m_bQuickCombat;
	bool m_bRivalTerritory;
	bool m_bMilitaryHappiness;
	bool m_bMilitarySupport;
	bool m_bMilitaryProduction;
	bool m_bPillage;
	bool m_bSpy;
	bool m_bSabotage;
	bool m_bDestroy;
	bool m_bStealPlans;
	bool m_bInvestigate;
	bool m_bCounterSpy;
	bool m_bFound;
	bool m_bGoldenAge;
	bool m_bInvisible;
	bool m_bFirstStrikeImmune;
	bool m_bNoDefensiveBonus;
	bool m_bIgnoreBuildingDefense;
	bool m_bCanMoveImpassable;
	bool m_bCanMoveAllTerrain;
	bool m_bFlatMovementCost;
	bool m_bIgnoreTerrainCost;
	bool m_bNukeImmune;
	bool m_bMechanized;
	bool m_bRenderBelowWater;
	bool m_bRenderAlways;
	bool m_bSuicide;
	bool m_bLineOfSight;
	bool m_bHiddenNationality;
	bool m_bAlwaysHostile;
	bool m_bFreeDrop;
	bool m_bNoRevealMap;
	bool m_bInquisitor;

	//ls612: Can't enter non-Owned cities
	bool m_bNoNonOwnedEntry;

	int m_iLeaderPromotion;

	float m_fUnitMaxSpeed;
	float m_fUnitPadTime;

	// Arrays

	std::vector<TechTypes> m_piPrereqAndTechs;
	std::vector<BonusTypes> m_piPrereqOrBonuses;
	int* m_piFlavorValue;
	int* m_piTerrainAttackModifier;
	int* m_piTerrainDefenseModifier;
	int* m_piFeatureAttackModifier;
	int* m_piFeatureDefenseModifier;
	int* m_piUnitAttackModifier;
	int* m_piUnitDefenseModifier;
	int* m_piUnitCombatModifier;
	int* m_piUnitCombatCollateralImmune;
	int* m_piDomainModifier;
	int* m_piBonusProductionModifier;
	int* m_piUnitGroupRequired;
	int* m_piReligionSpreads;
	int* m_piCorporationSpreads;
	int* m_piTerrainPassableTech;
	int* m_piFeaturePassableTech;
	int* m_piFlankingStrikeUnit;

	bool* m_pbPrereqOrCivics;

	bool* m_pbTargetUnitCombat;
	bool* m_pbDefenderUnitCombat;
	bool* m_pbUnitAIType;
	bool* m_pbNotUnitAIType;
	bool* m_pbGreatPeoples;
	std::vector<int> m_pbBuildings;
	bool* m_pbTerrainNative;
	bool* m_pbFeatureNative;
	//bool* m_pbTerrainImpassable;
	//bool* m_pbFeatureImpassable;
	bool* m_pbFreePromotions;

	CvString* m_paszEarlyArtDefineTags;
	CvString* m_paszLateArtDefineTags;
	CvString* m_paszMiddleArtDefineTags;
	CvString* m_paszUnitNames;
	CvString m_szFormationType;
	CvString m_szArtDefineButton;
	CvWString m_szExtraHoverTextKey;

	std::vector<int> m_aiSeeInvisibleTypes;

	CvOutcomeList m_KillOutcomeList;
	std::vector<CvOutcomeMission*> m_aOutcomeMissions;

	//TB Combat Mods Start  TB SubCombat Mod begin
	//integers
	int m_iAttackCombatModifier;
	int m_iDefenseCombatModifier;
	int m_iPursuit;
	int m_iEarlyWithdraw;
	int m_iVSBarbs;
	int m_iArmor;
	int m_iPuncture;
	int m_iOverrun;
	int m_iRepel;
	int m_iFortRepel;
	int m_iRepelRetries;
	int m_iUnyielding;
	int m_iKnockback;
	int m_iKnockbackRetries;
#ifdef BATTLEWORN
	int m_iStrAdjperRnd;
	int m_iStrAdjperAtt;
	int m_iStrAdjperDef;
	int m_iWithdrawAdjperAtt;
#endif // BATTLEWORN
	int m_iUnnerve;
	int m_iEnclose;
	int m_iLunge;
	int m_iDynamicDefense;
	int m_iFortitude;
#ifdef STRENGTH_IN_NUMBERS
	int m_iFrontSupportPercent;
	int m_iShortRangeSupportPercent;
	int m_iMediumRangeSupportPercent;
	int m_iLongRangeSupportPercent;
	int m_iFlankSupportPercent;
#endif
	int m_iDodgeModifier;
	int m_iPrecisionModifier;
	int m_iPowerShots;
	int m_iPowerShotCombatModifier;
	int m_iPowerShotPunctureModifier;
	int m_iPowerShotPrecisionModifier;
	int m_iPowerShotCriticalModifier;
	int m_iCriticalModifier;
	int m_iEndurance;
	int m_iRoundStunProb;
	int m_iPoisonProbabilityModifier;
	//Team Project (3)
	int m_iCaptureProbabilityModifier;
	int m_iCaptureResistanceModifier;
	//Team Project (4)
	//WorkRateMod
	int m_iHillsWorkModifier;
	int m_iPeaksWorkModifier;

	int m_iBreakdownChance;
	int m_iBreakdownDamage;
	int m_iTaunt;
	int m_iMaxHP;
	int m_iDamageModifier;
	int m_iTotalCombatStrengthModifierBase;
	int m_iTotalCombatStrengthChangeBase;
	int m_iBaseCargoVolume;
	int m_iRBombardDamage;
	int m_iRBombardDamageLimit;
	int m_iRBombardDamageMaxUnits;
	int m_iBaseGroupRank;
	int m_iCombatModifierPerSizeMore;
	int m_iCombatModifierPerSizeLess;
	int m_iCombatModifierPerVolumeMore;
	int m_iCombatModifierPerVolumeLess;
	int m_iBaseSMRankTotal;
	int m_iBaseSMVolumetricRankTotal;
	int m_iSelfHealModifier;
	int m_iNumHealSupport;
	int m_iInsidiousness;
	int m_iInvestigation;
	int m_iStealthStrikes;
	int m_iStealthCombatModifier;
	int m_iTrapDamageMax;
	int m_iTrapDamageMin;
	int m_iTrapComplexity;
	int m_iNumTriggers;
	int m_iAggression;
	int m_iAnimalIgnoresBorders;
	int m_iReligiousCombatModifier;

	//booleans
	bool m_bStampede;
	bool m_bOnslaught;
	bool m_bDealsColdDamage;
	bool m_bColdImmune;
	bool m_bAttackOnlyCities;
	bool m_bIgnoreNoEntryLevel;
	bool m_bIgnoreZoneofControl;
	bool m_bFliesToMove;
	bool m_bRBombardForceAbility;
	bool m_bNoSelfHeal;
	bool m_bExcile;
	bool m_bPassage;
	bool m_bNoNonOwnedCityEntry;
	bool m_bBarbCoExist;
	bool m_bBlendIntoCity;
	bool m_bUpgradeAnywhere;
	bool m_bAssassin;
	bool m_bStealthDefense;
	bool m_bNoInvisibility;
	bool m_bTriggerBeforeAttack;
	bool m_bCanAnimalIgnoresBorders;
	bool m_bCanAnimalIgnoresImprovements;
	bool m_bCanAnimalIgnoresCities;
	bool m_bNoNonTypeProdMods;
	bool m_bGatherHerd;

	std::vector<UnitCombatTypes> m_aiSubCombatTypes;
	std::vector<int> m_aiCureAfflictionTypes;
	std::vector<int> m_aiHealAsTypes;
	std::vector<TerrainTypes> m_vTerrainImpassableTypes;
	std::vector<FeatureTypes> m_vFeatureImpassableTypes;
	std::vector<MapCategoryTypes> m_aeMapCategoryTypes;
	std::vector<int> m_aiTrapSetWithPromotionTypes;
	std::vector<int> m_aiTrapImmunityUnitCombatTypes;

	std::vector<HealUnitCombat> m_aHealUnitCombatTypes;
	std::vector<GroupSpawnUnitCombat> m_aGroupSpawnUnitCombatTypes;
	std::vector<InvisibleTerrainChanges> m_aInvisibleTerrainChanges;
	std::vector<InvisibleFeatureChanges> m_aInvisibleFeatureChanges;
	std::vector<InvisibleImprovementChanges> m_aInvisibleImprovementChanges;
	std::vector<InvisibleTerrainChanges> m_aVisibleTerrainChanges;
	std::vector<InvisibleFeatureChanges> m_aVisibleFeatureChanges;
	std::vector<InvisibleImprovementChanges> m_aVisibleImprovementChanges;
	std::vector<InvisibleTerrainChanges> m_aVisibleTerrainRangeChanges;
	std::vector<InvisibleFeatureChanges> m_aVisibleFeatureRangeChanges;
	std::vector<InvisibleImprovementChanges> m_aVisibleImprovementRangeChanges;
	std::vector<EnabledCivilizations> m_aEnabledCivilizationTypes;
	UnitCombatModifierArray m_aFlankingStrengthbyUnitCombatType;
	TerrainModifierArray m_aWithdrawOnTerrainTypes;
	FeatureModifierArray m_aWithdrawOnFeatureTypes;
	UnitCombatModifierArray m_aWithdrawVSUnitCombatTypes;
	UnitCombatModifierArray m_aPursuitVSUnitCombatTypes;
	UnitCombatModifierArray m_aRepelVSUnitCombatTypes;
	UnitCombatModifierArray m_aKnockbackVSUnitCombatTypes;
	UnitCombatModifierArray m_aPunctureVSUnitCombatTypes;
	UnitCombatModifierArray m_aArmorVSUnitCombatTypes;
	UnitCombatModifierArray m_aDodgeVSUnitCombatTypes;
	UnitCombatModifierArray m_aPrecisionVSUnitCombatTypes;
	UnitCombatModifierArray m_aCriticalVSUnitCombatTypes;
	UnitCombatModifierArray m_aRoundStunVSUnitCombatTypes;
	UnitCombatModifierArray m_aTrapDisableUnitCombatTypes;
	UnitCombatModifierArray m_aTrapAvoidanceUnitCombatTypes;
	UnitCombatModifierArray m_aTrapTriggerUnitCombatTypes;
	InvisibilityArray m_aVisibilityIntensityTypes;
	InvisibilityArray m_aInvisibilityIntensityTypes;
	InvisibilityArray m_aVisibilityIntensityRangeTypes;
	TerrainModifierArray m_aTerrainWorkRateModifierTypes;
	FeatureModifierArray m_aFeatureWorkRateModifierTypes;
	BuildModifierArray m_aBuildWorkRateModifierTypes;
#ifdef OUTBREAKS_AND_AFFLICTIONS
	std::vector<AfflictionLineChanges> m_aDistanceAttackCommunicabilityTypeChanges;
	std::vector<PromotionLineModifier> m_aAfflictionFortitudeModifiers;
	std::vector<AfflictOnAttack> m_aAfflictOnAttackTypes;
	AidArray m_aAidChanges;
#endif // OUTBREAKS_AND_AFFLICTIONS

	//Pediahelp
	std::vector<int> m_aiQualifiedPromotionTypes;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// class	: CvSpawnInfo
//
// \brief	: Holds information relating to auto unit spawns
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CvSpawnInfo : public CvInfoBase
{
public:
	CvSpawnInfo();
	virtual ~CvSpawnInfo();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvSpawnInfo* pClassInfo);

	const std::vector<BonusTypes>&   getBonuses() const        { return m_bonusTypes; }
	const std::vector<TerrainTypes>& getTerrain() const        { return m_terrainTypes; }
	const std::vector<FeatureTypes>& getFeatures() const       { return m_featureTypes; }
	const std::vector<TerrainTypes>& getFeatureTerrain() const { return m_featureTerrainTypes; }
	const std::vector<UnitTypes>&    getSpawnGroups() const    { return m_spawnGroup; }

	PlayerTypes getPlayer() const;
	int getTurnRate() const;
	int getGlobalTurnRate() const;
	int getMaxLocalDensity() const;
	int getMinAreaPlotsPerPlayerUnit() const;
	int getMinAreaPlotsPerUnitType() const;
	int getStartDate() const;
	int	getEndDate() const;
	int	getMinLatitude() const;
	int	getMaxLatitude() const;
	bool getLatitudeAbs() const;
	int	getMinLongitude() const;
	int	getMaxLongitude() const;
	bool getTreatAsBarbarian() const;
	bool getNeutralOnly() const;
	bool getNoSpeedNormalization() const;
	bool getNotInView() const;
	bool getHills() const;
	bool getFlatlands() const;
	bool getPeaks() const;
	bool getFreshWaterOnly() const;
	int getRateOverride() const;
	UnitTypes getUnitType() const;
	TechTypes getPrereqTech() const			{ return m_ePrereqTech; }
	TechTypes getObsoleteTech() const		{ return m_eObsoleteTechType; }

	const BoolExpr* getSpawnCondition() const;

	void getCheckSum(uint32_t& iSum) const;

private:
	UnitTypes				m_eUnitType;
	TechTypes				m_ePrereqTech;
	TechTypes				m_eObsoleteTechType;
	int						m_iPlayerType;
	int						m_iTurns;
	int						m_iGlobalTurns;
	int						m_iMaxLocalDensity;
	int						m_iMinAreaPlotsPerPlayerUnit;
	int						m_iMinAreaPlotsPerUnitType;
	int						m_iStartDate;
	int						m_iEndDate;
	bool					m_bTreatAsBarbarian;
	bool					m_bNeutralOnly;
	bool					m_bNotInView;
	bool					m_bNoSpeedNormalization;
	bool					m_bHills;
	bool					m_bFlatlands;
	bool					m_bPeaks;
	bool					m_bFreshWaterOnly;

	bool					m_bLatitudeAbs;
	int						m_iMinLatitude;
	int						m_iMaxLatitude;
	int						m_iMinLongitude;
	int						m_iMaxLongitude;

	int						m_iRateOverride;
	std::vector<BonusTypes>	m_bonusTypes;
	std::vector<TerrainTypes>	m_terrainTypes;
	std::vector<FeatureTypes>	m_featureTypes;
	std::vector<TerrainTypes>	m_featureTerrainTypes;
	std::vector<UnitTypes>		m_spawnGroup;

	const BoolExpr*				m_pExprSpawnCondition;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// class	: CvUnitFormationInfo
//
// \brief	: Holds information relating to the formation of sub-units within a unit
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CvUnitEntry
{
public:
	CvUnitEntry()
		: m_position()
		, m_fRadius(0)
		, m_fFacingDirection(0)
		, m_fFacingVariance(0)
	{
	}

	CvUnitEntry(const NiPoint2 &position, float radius, float facingDirection, float facingVariance)
		: m_position(position)
		, m_fRadius(radius)
		, m_fFacingDirection(facingDirection)
		, m_fFacingVariance(facingVariance)
	{
	}

	NiPoint2 m_position;
	float m_fRadius;
	float m_fFacingDirection;
	float m_fFacingVariance;
};

class CvUnitFormationInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	DllExport CvUnitFormationInfo();
	virtual ~CvUnitFormationInfo();

	DllExport const char* getFormationType() const;
	int getNumEventTypes() const;
	const EntityEventTypes &getEventType(int index) const;
	DllExport const std::vector<EntityEventTypes> & getEventTypes() const;

	DllExport int getNumUnitEntries() const;
	DllExport const CvUnitEntry &getUnitEntry(int index) const;
	DllExport void addUnitEntry(const CvUnitEntry &unitEntry);
	int getNumGreatUnitEntries() const;
	DllExport const CvUnitEntry &getGreatUnitEntry(int index) const;
	int getNumSiegeUnitEntries() const;
	DllExport const CvUnitEntry &getSiegeUnitEntry(int index) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvUnitFormationInfo* pClassInfo);

	//---------------------------------------PUBLIC MEMBER VARIABLES---------------------------------

protected:

	CvString m_szFormationType;
	std::vector<EntityEventTypes>	m_vctEventTypes;		//!< The list of EntityEventTypes that this formation is intended for

	std::vector<CvUnitEntry> m_vctUnitEntries;
	std::vector<CvUnitEntry> m_vctGreatUnitEntries;
	std::vector<CvUnitEntry> m_vctSiegeUnitEntries;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSpecialUnitInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpecialUnitInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvSpecialUnitInfo();
	virtual ~CvSpecialUnitInfo();

	bool isValid() const;
	bool isCityLoad() const;
	bool isSMLoadSame() const;

	// Arrays

	bool isCarrierUnitAIType(int i) const;

	int getCombatPercent() const;
	int getWithdrawalChange() const;
	int getPursuitChange() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvSpecialUnitInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	bool m_bValid;
	bool m_bCityLoad;
	bool m_bSMLoadSame;

	int m_iCombatPercent;
	int m_iWithdrawalChange;
	int m_iPursuitChange;

	// Arrays
	bool* m_pbCarrierUnitAITypes;
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

//	bool getTraitNoUpkeep(int i) const;
	bool isPolicy() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvCivicOptionInfo* pClassInfo);

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
	const python::list cyGetBuildingProductionModifiers() const { return m_aBuildingProductionModifier.makeList(); }

	int getBuildingHappinessChanges(int i) const;
	int getBuildingHealthChanges(int i) const;
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

	IDValueMap<BuildingTypes, int> m_aBuildingProductionModifier;

	std::vector<CvString> m_aszUnitProdModforPass3;
	std::vector<int> m_aiUnitProdModforPass3;

	std::vector<CvString> m_aszCivicAttitudeforPass3;
	std::vector<int> m_aiCivicAttitudeforPass3;
	std::vector<CvString> m_aszCivicAttitudeReasonforPass3;
	std::vector<CvString> m_aszCivicAttitudeReasonValueforPass3;
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

	const CvDiplomacyResponse& getResponse(int iNum) const;
	int getNumResponses() const;

	bool getCivilizationTypes(int i, int j) const;
	bool getLeaderHeadTypes(int i, int j) const;
	bool getAttitudeTypes(int i, int j) const;
	bool getDiplomacyPowerTypes(int i, int j) const;

	int getNumDiplomacyText(int i) const;

	const char* getDiplomacyText(int i, int j) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvXMLLoadUtility* pXML);
	bool FindResponseIndex(const CvDiplomacyResponse* pNewResponse, int iCase, int* iIndex) const;

private:
	std::vector<CvDiplomacyResponse*> m_pResponses;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSpecialBuildingInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpecialBuildingInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvSpecialBuildingInfo();
	virtual ~CvSpecialBuildingInfo();

	TechTypes getObsoleteTech() const;
	TechTypes getTechPrereq() const { return m_iTechPrereq; }
	int getTechPrereqAnyone() const;
	int getMaxPlayerInstances() const;

	bool isValid() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvSpecialBuildingInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	TechTypes m_iObsoleteTech;
	TechTypes m_iTechPrereq;
	int m_iTechPrereqAnyone;
	int m_iMaxPlayerInstances;

	bool m_bValid;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvRiverModelInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvRiverModelInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvRiverModelInfo();
	virtual ~CvRiverModelInfo();

	DllExport const char* getModelFile() const;
	DllExport const char* getBorderFile() const;

	DllExport int getTextureIndex() const;
	DllExport const char* getDeltaString() const;
	DllExport const char* getConnectString() const;
	DllExport const char* getRotateString() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvRiverModelInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	CvString m_szModelFile;					// The model filename
	CvString m_szBorderFile;				// The border filename

	char		m_szDeltaString[32];		// The delta type
	char		m_szConnectString[32];		// The connections this cell makes ( N S E W NE NW SE SW )
	char		m_szRotateString[32];		// The possible rotations for this cell ( 0 90 180 270 )
	int			m_iTextureIndex;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvRouteModelInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvRouteModelInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvRouteModelInfo();
	virtual ~CvRouteModelInfo();

	DllExport RouteTypes getRouteType() const;

	DllExport const char* getModelFile() const;
	DllExport const char* getLateModelFile() const;
	const char* getModelFileKey() const;

	DllExport bool isAnimated() const;

	DllExport const char* getConnectString() const;
	DllExport const char* getModelConnectString() const;
	DllExport const char* getRotateString() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvRouteModelInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	RouteTypes	m_eRouteType;			// The route type

	CvString m_szModelFile;				// The model filename
	CvString m_szLateModelFile;			// The model filename
	CvString m_szModelFileKey;			// The model file key reference
	bool m_bAnimated;

	char m_szConnectString[32];		// The connections this cell makes ( N S E W NE NW SE SW )
	char m_szModelConnectString[32];	// The connections this model makes ( N S E W NE NW SE SW )
	char m_szRotateString[32];		// The possible rotations for this cell ( 0 90 180 270 )
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

	DllExport int getDefaultPlayerColor() const;
	int getArtStyleType() const;
	int getUnitArtStyleType() const;
	int getNumCityNames() const;
	// the number of leaders the Civ has, this is needed so that random leaders can be generated easily
	int getNumLeaders() const;
	int getSelectionSoundScriptId() const;
	int getActionSoundScriptId() const;

	DllExport bool isAIPlayable() const;
	DllExport bool isPlayable() const;

	std::wstring pyGetShortDescription(uint uiForm) { return getShortDescription(uiForm); }
	DllExport const wchar_t* getShortDescription(uint uiForm = 0);
	const wchar_t* getShortDescriptionKey() const;
	const std::wstring pyGetShortDescriptionKey() const { return getShortDescriptionKey(); }

	std::wstring pyGetAdjective(uint uiForm) { return getAdjective(uiForm); }
	DllExport const wchar_t* getAdjective(uint uiForm = 0);
	const wchar_t* getAdjectiveKey() const;
	const std::wstring pyGetAdjectiveKey() const { return getAdjectiveKey(); }

	DllExport const char* getFlagTexture() const;
	const char* getArtDefineTag() const;

	// Arrays

	int getCivilizationFreeUnits(int i) const;
	int getCivilizationInitialCivics(int i) const;
	// Afforess 04/05/10
	void setCivilizationInitialCivics(int iCivicOption, int iCivic);

	DllExport bool isLeaders(int i) const;

	bool isCivilizationFreeTechs(int i) const;
	bool isCivilizationDisableTechs(int i) const;

	int getNumCivilizationBuildings() const;
	int getCivilizationBuilding(int i) const;
	bool isCivilizationBuilding(int i) const;

	std::string getCityNames(int i) const;

	const CvArtInfoCivilization* getArtInfo() const;
	const char* getButton() const;

	CivilizationTypes getDerivativeCiv() const { return m_iDerivativeCiv; }

	//TB Tags
	//int
	int getSpawnRateModifier() const;
	int getSpawnRateNPCPeaceModifier() const;
	// Means that the civilization cannot build or train anything which is not specified
	// as allowed in Unit or Building Info by the EnabledCivilization tag.  Generally used for NPC players.
	bool isStronglyRestricted() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvCivilizationInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:
	int m_iDefaultPlayerColor;
	int m_iArtStyleType;
	int m_iUnitArtStyleType;
	int m_iNumCityNames;
	int m_iNumLeaders;
	int m_iSelectionSoundScriptId;
	int m_iActionSoundScriptId;
	CivilizationTypes m_iDerivativeCiv;
	// TB Tags
	int m_iSpawnRateModifier;
	int m_iSpawnRateNPCPeaceModifier;

	bool m_bStronglyRestricted;
	// ! TB Tags
	bool m_bAIPlayable;
	bool m_bPlayable;

	CvString m_szArtDefineTag;
	CvWString m_szShortDescriptionKey;
	CvWString m_szAdjectiveKey;

	int* m_piCivilizationFreeUnits;
	int* m_piCivilizationInitialCivics;

	bool* m_pbLeaders;
	bool* m_pbCivilizationFreeTechs;
	bool* m_pbCivilizationDisableTechs;

	CvString* m_paszCityNames;

	std::vector<int> m_aiCivilizationBuildings;

	mutable std::vector<CvWString> m_aszShortDescription;
	mutable std::vector<CvWString> m_aszAdjective;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvVictoryInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvVictoryInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvVictoryInfo();
	virtual ~CvVictoryInfo();

	int getPopulationPercentLead() const;
	int getLandPercent() const;
	int getMinLandPercent() const;
	int getReligionPercent() const;
	int getCityCulture() const;
	int getNumCultureCities() const;
	int getTotalCultureRatio() const;
	int getVictoryDelayTurns() const;

	bool isTotalVictory() const;
	bool isTargetScore() const;
	bool isEndScore() const;
	bool isConquest() const;
	bool isDiploVote() const;
	DllExport bool isPermanent() const;

	const char* getMovie() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvVictoryInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iPopulationPercentLead;
	int m_iLandPercent;
	int m_iMinLandPercent;
	int m_iReligionPercent;
	int m_iCityCulture;
	int m_iNumCultureCities;
	int m_iTotalCultureRatio;
	int m_iVictoryDelayTurns;

	bool m_bTargetScore;
	bool m_bEndScore;
	bool m_bConquest;
	bool m_bDiploVote;
	bool m_bPermanent;
	bool m_bTotalVictory;

	CvString m_szMovie;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvHurryInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvHurryInfo :
	public CvInfoBase
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvHurryInfo();
	virtual ~CvHurryInfo();

	int getGoldPerProduction() const;
	int getProductionPerPopulation() const;

	bool isAnger() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvHurryInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

//---------------------------------------PUBLIC MEMBER VARIABLES---------------------------------
protected:

	int m_iGoldPerProduction;
	int m_iProductionPerPopulation;

	bool m_bAnger;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvHandicapInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvHandicapInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvHandicapInfo();
	virtual ~CvHandicapInfo();

	int getFreeWinsVsBarbs() const;
	int getAnimalAttackProb() const;
	int getAdvancedStartPointsMod() const;
	int getStartingGold() const;
	int getUnitUpkeepPercent() const;
	int getTrainPercent() const;
	int getConstructPercent() const;
	int getResearchPercent() const;
	int getDistanceMaintenancePercent() const;
	int getNumCitiesMaintenancePercent() const;
	int getColonyMaintenancePercent() const;
	int getMaxColonyMaintenance() const;
	int getCorporationMaintenancePercent() const;
	int getCivicUpkeepPercent() const;
	int getInflationPercent() const;
	/*** REVOLUTION 09/25/09 by DPII ***/
	int getRevolutionIndexPercent() const;
	/***********************************/
	int getHealthBonus() const;
	int getHappyBonus() const;
	int getAttitudeChange() const;
	int getNoTechTradeModifier() const;
	int getTechTradeKnownModifier() const;
	int getUnownedTilesPerGameAnimal() const;
	int getUnownedTilesPerBarbarianUnit() const;
	int getUnownedWaterTilesPerBarbarianUnit() const;
	int getUnownedTilesPerBarbarianCity() const;
	int getBarbarianCityCreationTurnsElapsed() const;
	int getBarbarianCityCreationProb() const;
	int getAnimalCombatModifier() const;
	int getBarbarianCombatModifier() const;
	int getAIAnimalCombatModifier() const;
	int getAIBarbarianCombatModifier() const;

	int getStartingDefenseUnits() const;
	int getStartingWorkerUnits() const;
	int getStartingExploreUnits() const;
	int getAIStartingUnitMultiplier() const;
	int getAIStartingDefenseUnits() const;
	int getAIStartingWorkerUnits() const;
	int getAIStartingExploreUnits() const;
	int getBarbarianInitialDefenders() const;
	int getAIDeclareWarProb() const;
	int getAIWorkRateModifier() const;
	int getAIGrowthPercent() const;
	int getAITrainPercent() const;
	int getAIWorldTrainPercent() const;
	int getAIConstructPercent() const;
	int getAIWorldConstructPercent() const;
	int getAICreatePercent() const;
	int getAIResearchPercent() const;
	int getAIWorldCreatePercent() const;
	int getAICivicUpkeepPercent() const;
	int getAIUnitUpkeepPercent() const;
	int getAIUnitSupplyPercent() const;
	int getAIUnitUpgradePercent() const;
	int getAIInflationPercent() const;
	int getAIWarWearinessPercent() const;
	int getAIPerEraModifier() const;
	int getAIAdvancedStartPercent() const;
	int getNumGoodies() const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

	// Arrays

	int getGoodies(int i) const;

	int getPercent(int iID) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvHandicapInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

private:
	CvPropertyManipulators m_PropertyManipulators;

	int m_iFreeWinsVsBarbs;
	int m_iAnimalAttackProb;
	int m_iAdvancedStartPointsMod;
	int m_iStartingGold;
	int m_iUnitUpkeepPercent;
	int m_iTrainPercent;
	int m_iConstructPercent;
	int m_iResearchPercent;
	int m_iDistanceMaintenancePercent;
	int m_iNumCitiesMaintenancePercent;
	int m_iColonyMaintenancePercent;
	int m_iMaxColonyMaintenance;
	int m_iCorporationMaintenancePercent;
	int m_iCivicUpkeepPercent;
	int m_iInflationPercent;
	int m_iRevolutionIndexPercent;
	int m_iHealthBonus;
	int m_iHappyBonus;
	int m_iAttitudeChange;
	int m_iNoTechTradeModifier;
	int m_iTechTradeKnownModifier;
	int m_iUnownedTilesPerGameAnimal;
	int m_iUnownedTilesPerBarbarianUnit;
	int m_iUnownedWaterTilesPerBarbarianUnit;
	int m_iUnownedTilesPerBarbarianCity;
	int m_iBarbarianCityCreationTurnsElapsed;
	int m_iBarbarianCityCreationProb;
	int m_iAnimalCombatModifier;
	int m_iBarbarianCombatModifier;
	int m_iAIAnimalCombatModifier;
	int m_iAIBarbarianCombatModifier;

	int m_iStartingDefenseUnits;
	int m_iStartingWorkerUnits;
	int m_iStartingExploreUnits;
	int m_iAIStartingUnitMultiplier;
	int m_iAIStartingDefenseUnits;
	int m_iAIStartingWorkerUnits;
	int m_iAIStartingExploreUnits;
	int m_iBarbarianInitialDefenders;
	int m_iAIDeclareWarProb;
	int m_iAIWorkRateModifier;
	int m_iAIGrowthPercent;
	int m_iAITrainPercent;
	int m_iAIWorldTrainPercent;
	int m_iAIConstructPercent;
	int m_iAIWorldConstructPercent;
	int m_iAICreatePercent;
	int m_iAIResearchPercent;
	int m_iAIWorldCreatePercent;
	int m_iAICivicUpkeepPercent;
	int m_iAIUnitUpkeepPercent;
	int m_iAIUnitSupplyPercent;
	int m_iAIUnitUpgradePercent;
	int m_iAIInflationPercent;
	int m_iAIWarWearinessPercent;
	int m_iAIPerEraModifier;
	int m_iAIAdvancedStartPercent;

	CvString m_szHandicapName;

	IDValueMapPercent m_Percent;

	std::vector<int> m_piGoodies;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvGameSpeedInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameSpeedInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvGameSpeedInfo();
	virtual ~CvGameSpeedInfo();

	int getSpeedPercent() const;
	int getHammerCostPercent() const;
	int getNumTurnIncrements() const;

	const GameTurnInfo& getGameTurnInfo(int iIndex) const;
	const CvDateIncrement& getDateIncrement(int iIndex) const;
	std::vector<CvDateIncrement>& getIncrements();
	bool getEndDatesCalculated() const;
	void setEndDatesCalculated(bool bCalculated);

	void allocateGameTurnInfos(int iSize);

	int getPercent(int iID) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvGameSpeedInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iSpeedPercent;
	int m_iNumTurnIncrements;

	CvString m_szGameSpeedName;
	GameTurnInfo* m_pGameTurnInfo;
	std::vector<CvDateIncrement> m_aIncrements;
	bool m_bEndDatesCalculated;

	IDValueMapPercent m_Percent;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvTurnTimerInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTurnTimerInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvTurnTimerInfo();
	virtual ~CvTurnTimerInfo();

	int getBaseTime() const;
	int getCityBonus() const;
	int getUnitBonus() const;
	int getFirstTurnMultiplier() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvTurnTimerInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iBaseTime;
	int m_iCityBonus;
	int m_iUnitBonus;
	int m_iFirstTurnMultiplier;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvBuildInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBuildInfo : public CvHotkeyInfo
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvBuildInfo();
	virtual ~CvBuildInfo();

	int getTime() const;
	int getCost() const;
	TechTypes getTechPrereq() const			{ return m_iTechPrereq; }
	ImprovementTypes getImprovement() const	{ return m_iImprovement; }
	int getRoute() const;
	int getTerrainChange() const;
	int getFeatureChange() const;
	TechTypes getObsoleteTech() const;
	bool isNoTechCanRemoveWithNoProductionGain(int i) const;
	bool isDisabled() const;
	void setDisabled(bool bNewVal);
	DllExport int getEntityEvent() const;
	DllExport int getMissionType() const;
	void setMissionType(int iNewType);

	bool isKill() const;

	// Arrays

	TechTypes getFeatureTech(FeatureTypes e) const;
	int getFeatureTime(FeatureTypes e) const;
	int getFeatureProduction(FeatureTypes e) const;
	bool isFeatureRemove(FeatureTypes e) const;

	const std::vector<BonusTypes>& getPrereqBonuses() const			{ return m_aiPrereqBonusTypes; }
	const std::vector<MapCategoryTypes>& getMapCategories() const	{ return m_aeMapCategoryTypes; }
	const std::vector<TerrainStructs>& getTerrainStructs() const	{ return m_aTerrainStructs; }
	const std::vector<PlaceBonusTypes>& getPlaceBonusTypes() const	{ return m_aPlaceBonusTypes; }

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvBuildInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:
	bool m_bDisabled;
	bool m_bKill;

	int m_iTime;
	int m_iCost;
	TechTypes m_iTechPrereq;
	ImprovementTypes m_iImprovement;
	int m_iRoute;
	int m_iTerrainChange;
	int m_iFeatureChange;
	TechTypes m_iObsoleteTech;
	int m_iEntityEvent;
	int m_iMissionType;

	// Arrays

	int* m_paiFeatureTech;
	int* m_paiFeatureTime;
	int* m_paiFeatureProduction;

	bool* m_pabFeatureRemove;
	bool* m_pabNoTechCanRemoveWithNoProductionGain;

	std::vector<BonusTypes> m_aiPrereqBonusTypes;
	std::vector<MapCategoryTypes> m_aeMapCategoryTypes;

	std::vector<TerrainStructs> m_aTerrainStructs;
	std::vector<PlaceBonusTypes> m_aPlaceBonusTypes;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvGoodyInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGoodyInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvGoodyInfo();
	virtual ~CvGoodyInfo();

	int getGold() const;
	int getGoldRand1() const;
	int getGoldRand2() const;
	int getMapOffset() const;
	int getMapRange() const;
	int getMapProb() const;
	int getExperience() const;
	int getHealing() const;
	int getDamagePrereq() const;
	int getBarbarianUnitProb() const;
	int getMinBarbarians() const;
	int getGoodyUnit() const;
	int getBarbarianUnit() const;
	int getEraType() const;
	int getNotEraType() const;
	int getResearch() const;

	bool isTech() const;
	bool isBad() const;
	bool isNaval() const;

	const std::vector<MapCategoryTypes>& getMapCategories() const { return m_aeMapCategoryTypes; }

	const char* getSound() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvGoodyInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iGold;
	int m_iGoldRand1;
	int m_iGoldRand2;
	int m_iMapOffset;
	int m_iMapRange;
	int m_iMapProb;
	int m_iExperience;
	int m_iHealing;
	int m_iDamagePrereq;
	int m_iBarbarianUnitProb;
	int m_iMinBarbarians;
	int m_iGoodyUnit;
	int m_iBarbarianUnit;
	int m_iEraType;
	int m_iNotEraType;
	int m_iResearch;

	bool m_bTech;
	bool m_bBad;
	bool m_bNaval;

	std::vector<MapCategoryTypes> m_aeMapCategoryTypes;

	CvString m_szSound;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvRouteInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvRouteInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvRouteInfo();
	virtual ~CvRouteInfo();

	int getAdvancedStartCost() const;

	int getValue() const;
	int getMovementCost() const;
	int getFlatMovementCost() const;
	int getPrereqBonus() const;

	bool isSeaTunnel() const;

	// Arrays
	int getYieldChange(int i) const;
	int* getYieldChangeArray() const;
	int getTechMovementChange(int i) const;

	const std::vector<BonusTypes>& getPrereqOrBonuses() const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }
	//	This really belongs on CvInfoBase but you can't change the size of that
	//	object without crashing the core engine :-(
	inline int getZobristValue() const { return m_zobristValue; }

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvRouteInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

private:
	CvPropertyManipulators m_PropertyManipulators;

	int m_iAdvancedStartCost;
	int m_iValue;
	int m_iMovementCost;
	int m_iFlatMovementCost;
	int m_iPrereqBonus;
	int m_zobristValue;

	bool m_bSeaTunnel;

	// Arrays
	int* m_piYieldChange;
	int* m_piTechMovementChange;
	std::vector<BonusTypes> m_piPrereqOrBonuses;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvImprovementBonusInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvImprovementBonusInfo :
	public CvInfoBase
{
	friend class CvImprovementInfo;
	friend class CvXMLLoadUtility;

	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvImprovementBonusInfo();
	virtual ~CvImprovementBonusInfo();

	int getDiscoverRand() const;
	int getDepletionRand() const;

	bool isBonusMakesValid() const;
	bool isObsoleteBonusMakesValid() const;
	bool isBonusTrade() const;

	int getYieldChange(int i) const;

	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iDiscoverRand;
	int m_iDepletionRand;

	bool m_bBonusMakesValid;
	bool m_bObsoleteBonusMakesValid;
	bool m_bBonusTrade;

	// Arrays
	int* m_piYieldChange;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvBonusClassInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBonusClassInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvBonusClassInfo();
	virtual ~CvBonusClassInfo();

	int getUniqueRange() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvBonusClassInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:
	int m_iUniqueRange;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvBonusInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoBonus;
class CvBonusInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvBonusInfo();
	virtual ~CvBonusInfo();

	int getBonusClassType() const;
	int getChar() const;
	void setChar(int i);
	int getTechReveal() const;
	int getTechCityTrade() const;
	int getTechObsolete() const;
	int getAITradeModifier() const;
	int getAIObjective() const;
	int getHealth() const;
	int getHappiness() const;
	int getMinAreaSize() const;
	int getMinLatitude() const;
	int getMaxLatitude() const;
	int getPlacementOrder() const;
	int getConstAppearance() const;
	int getRandAppearance1() const;
	int getRandAppearance2() const;
	int getRandAppearance3() const;
	int getRandAppearance4() const;
	int getPercentPerPlayer() const;
	int getTilesPer() const;
	int getMinLandPercent() const;
	int getUniqueRange() const;
	int getGroupRange() const;
	int getGroupRand() const;

	bool isOneArea() const;
	bool isHills() const;
	bool isPeaks() const;
	bool isFlatlands() const;
	bool isBonusCoastalOnly() const;
	bool isNoRiverSide() const;
	bool isNormalize() const;

	const char* getArtDefineTag() const;

	// Arrays
	int getYieldChange(int i) const;
	int* getYieldChangeArray() const;
	int getImprovementChange(int i) const;

	bool isTerrain(int i) const;
	bool isFeature(int i) const;
	bool isFeatureTerrain(int i) const;

	const std::vector<MapCategoryTypes>& getMapCategories() const { return m_aeMapCategoryTypes; }

#ifdef OUTBREAKS_AND_AFFLICTIONS
	int getNumAfflictionCommunicabilityTypes() const;
	PromotionLineAfflictionModifier getAfflictionCommunicabilityType(int iPromotionLine, bool bWorkedTile = false, bool bVicinity = false, bool bAccessVolume = false);
#endif // OUTBREAKS_AND_AFFLICTIONS

	const char* getButton() const;
	DllExport const CvArtInfoBonus* getArtInfo() const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvBonusInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	const std::vector<std::pair<ImprovementTypes,BuildTypes> >*	getTradeProvidingImprovements();

private:
	CvPropertyManipulators m_PropertyManipulators;

	int m_iBonusClassType;
	int m_iChar;
	int m_iTechReveal;
	int m_iTechCityTrade;
	int m_iTechObsolete;
	int m_iAITradeModifier;
	int m_iAIObjective;
	int m_iHealth;
	int m_iHappiness;
	int m_iMinAreaSize;
	int m_iMinLatitude;
	int m_iMaxLatitude;
	int m_iPlacementOrder;
	int m_iConstAppearance;
	int m_iRandAppearance1;
	int m_iRandAppearance2;
	int m_iRandAppearance3;
	int m_iRandAppearance4;
	int m_iPercentPerPlayer;
	int m_iTilesPer;
	int m_iMinLandPercent;
	int m_iUniqueRange;
	int m_iGroupRange;
	int m_iGroupRand;

	bool m_bOneArea;
	bool m_bHills;
	bool m_bPeaks;
	bool m_bFlatlands;
	bool m_bBonusCoastalOnly;
	bool m_bNoRiverSide;
	bool m_bNormalize;

	CvString m_szArtDefineTag;

	// Arrays

	int* m_piYieldChange;
	int* m_piImprovementChange;

	bool* m_pbTerrain;
	bool* m_pbFeature;
	bool* m_pbFeatureTerrain;

	std::vector<MapCategoryTypes> m_aeMapCategoryTypes;
#ifdef OUTBREAKS_AND_AFFLICTIONS
	std::vector<PromotionLineAfflictionModifier> m_aAfflictionCommunicabilityTypes;
#endif // OUTBREAKS_AND_AFFLICTIONS
	volatile std::vector<std::pair<ImprovementTypes,BuildTypes> >* m_tradeProvidingImprovements;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvFeatureInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoFeature;
class CvFeatureInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvFeatureInfo();
	virtual ~CvFeatureInfo();

	int getMovementCost() const;
	int getSeeThroughChange() const;
	int getHealthPercent() const;
	int getAppearanceProbability() const;
	int getDisappearanceProbability() const;
	int getGrowthProbability() const;
	int getDefenseModifier() const;
	int getAdvancedStartRemoveCost() const;
	int getTurnDamage() const;
	int getWarmingDefense() const; //GWMod new xml field M.A.
	int getPopDestroys() const;

	bool isNoCoast() const;
	bool isNoRiver() const;
	bool isNoAdjacent() const;
	bool isRequiresFlatlands() const;
	bool isRequiresRiver() const;
	bool isAddsFreshWater() const;
	bool isImpassable() const;
	bool isNoCity() const;
	bool isNoImprovement() const;
	bool isVisibleAlways() const;
	bool isNukeImmune() const;
// BUG - City Plot Status - start
	bool isOnlyBad() const;
	bool isCountsAsPeak() const;
// BUG - City Plot Status - end
	const char* getOnUnitChangeTo() const;

	const char* getArtDefineTag() const;

	int getWorldSoundscapeScriptId() const;

	const char* getEffectType() const;
	int getEffectProbability() const;

	// Arrays

	int getYieldChange(int i) const;
	int getRiverYieldChange(int i) const;
	int get3DAudioScriptFootstepIndex(int i) const;

	bool isTerrain(int i) const;
	int getNumVarieties() const;

	bool canBeSecondary() const;

	const std::vector<MapCategoryTypes>& getMapCategories() const { return m_aeMapCategoryTypes; }

#ifdef OUTBREAKS_AND_AFFLICTIONS
	int getNumAfflictionCommunicabilityTypes() const;
	PromotionLineAfflictionModifier getAfflictionCommunicabilityType(int iPromotionLine, bool bWorkedTile = false, bool bVicinity = false, bool bAccessVolume = false);
#endif // OUTBREAKS_AND_AFFLICTIONS

	DllExport const CvArtInfoFeature* getArtInfo() const;
	const char* getButton() const;

	bool read(CvXMLLoadUtility* pXML);

	const char* getGrowthSound() const;
	int getSpreadProbability() const;
	int getCultureDistance() const;
	bool isIgnoreTerrainCulture() const;
	bool isCanGrowAnywhere() const;
protected:
	CvString m_szGrowthSound;
	int m_iSpreadProbability;
	int m_iCultureDistance;
	bool m_bIgnoreTerrainCulture;
	bool m_bCanGrowAnywhere;

	std::vector<MapCategoryTypes> m_aeMapCategoryTypes;
public:

	void copyNonDefaults(const CvFeatureInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

	//	This really belongs on CvInfoBase but you can't change the size of that
	//	object without crashing the core engine :-(
	inline int	getZobristValue() const { return m_zobristValue; }

private:
	CvPropertyManipulators m_PropertyManipulators;

	int m_iMovementCost;
	int m_iSeeThroughChange;
	int m_iHealthPercent;
	int m_iAppearanceProbability;
	int m_iDisappearanceProbability;
	int m_iGrowthProbability;
	int m_iDefenseModifier;
	int m_iAdvancedStartRemoveCost;
	int m_iTurnDamage;
	int m_iWarmingDefense; //GWMod new xml field M.A.
	int m_iPopDestroys;

	bool m_bNoCoast;
	bool m_bNoRiver;
	bool m_bNoAdjacent;
	bool m_bRequiresFlatlands;
	bool m_bRequiresRiver;
	bool m_bAddsFreshWater;
	bool m_bImpassable;
	bool m_bNoCity;
	bool m_bNoImprovement;
	bool m_bVisibleAlways;
	bool m_bNukeImmune;
	bool m_bCountsAsPeak;
	CvString m_szOnUnitChangeTo;

	int m_iWorldSoundscapeScriptId;

	CvString m_szEffectType;
	int m_iEffectProbability;

	// Arrays

	int* m_piYieldChange;
	int* m_piRiverYieldChange;
	int* m_pi3DAudioScriptFootstepIndex;

	bool* m_pbTerrain;

#ifdef OUTBREAKS_AND_AFFLICTIONS
	std::vector<PromotionLineAfflictionModifier> m_aAfflictionCommunicabilityTypes;
#endif // OUTBREAKS_AND_AFFLICTIONS

private:
	CvString m_szArtDefineTag;
	int m_zobristValue;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCommerceInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCommerceInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvCommerceInfo();
	virtual ~CvCommerceInfo();

	int getChar() const;
	void setChar(int i);
	int getInitialPercent() const;
	int getInitialHappiness() const;
	int getAIWeightPercent() const;

	bool isFlexiblePercent() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvCommerceInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iChar;
	int m_iInitialPercent;
	int m_iInitialHappiness;
	int m_iAIWeightPercent;

	bool m_bFlexiblePercent;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvYieldInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvYieldInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvYieldInfo();
	virtual ~CvYieldInfo();

	int getChar() const;
	void setChar(int i);
	int getHillsChange() const;
	int getPeakChange() const;
	int getRiverChange() const;
	int getCityChange() const;
	int getPopulationChangeDivisor() const;
	int getMinCity() const;
	int getTradeModifier() const;
	int getGoldenAgeYield() const;
	int getGoldenAgeYieldThreshold() const;
	int getAIWeightPercent() const;
	int getColorType() const;

	// Arrays

	const char* getSymbolPath(int i) const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvYieldInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iChar;
	int m_iHillsChange;
	int m_iPeakChange;
	int m_iRiverChange;
	int m_iCityChange;
	int m_iPopulationChangeDivisor;
	int m_iMinCity;
	int m_iTradeModifier;
	int m_iGoldenAgeYield;
	int m_iGoldenAgeYieldThreshold;
	int m_iAIWeightPercent;
	int m_iColorType;

	CvString* m_paszSymbolPath;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvTerrainInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoTerrain;
class CvTerrainInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvTerrainInfo();
	virtual ~CvTerrainInfo();

	int getMovementCost() const;
	int getBuildModifier() const;
	int getDefenseModifier() const;

	bool isWaterTerrain() const;
	bool isImpassable() const;
	bool isFound() const;
	bool isFoundCoast() const;
	bool isFoundFreshWater() const;
	bool isFreshWaterTerrain() const;

	DllExport const char* getArtDefineTag() const;

	int getYield(int i) const;
	int getWorldSoundscapeScriptId() const;
	int get3DAudioScriptFootstepIndex(int i) const;

	const std::vector<MapCategoryTypes>& getMapCategories() const { return m_aeMapCategoryTypes; }

#ifdef OUTBREAKS_AND_AFFLICTIONS
	int getNumAfflictionCommunicabilityTypes() const;
	PromotionLineAfflictionModifier getAfflictionCommunicabilityType(int iPromotionLine, bool bWorkedTile = false, bool bVicinity = false, bool bAccessVolume = false);
#endif // OUTBREAKS_AND_AFFLICTIONS

	const CvArtInfoTerrain* getArtInfo() const;
	const char* getButton() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvTerrainInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

	//	This really belongs on CvInfoBase but you can't change the size of that
	//	object without crashing the core engine :-(
	inline int getZobristValue() const { return m_zobristValue; }

	int getCultureDistance() const;
	int getHealthPercent() const;

	//TB Combat Mod begin
	bool isColdDamage() const;
	//TB Combat Mod end

private:
	CvPropertyManipulators m_PropertyManipulators;

	CvString m_szArtDefineTag;
	int	m_zobristValue;

	bool m_bWaterTerrain;
	bool m_bImpassable;
	bool m_bFound;
	bool m_bFoundCoast;
	bool m_bFoundFreshWater;
	bool m_bFreshWaterTerrain;
	bool m_bColdDamage; // TB Combat Mods

	int m_iMovementCost;
	int m_iBuildModifier;
	int m_iDefenseModifier;
	int m_iWorldSoundscapeScriptId;
	int m_iCultureDistance;
	int m_iHealthPercent;

	// Arrays
	int* m_piYields;
	int* m_pi3DAudioScriptFootstepIndex;

	std::vector<MapCategoryTypes> m_aeMapCategoryTypes;
#ifdef OUTBREAKS_AND_AFFLICTIONS
	std::vector<PromotionLineAfflictionModifier> m_aAfflictionCommunicabilityTypes;
#endif // OUTBREAKS_AND_AFFLICTIONS
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvInterfaceModeInfo (ADD to Python)
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvInterfaceModeInfo :
	public CvHotkeyInfo
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvInterfaceModeInfo();
	virtual ~CvInterfaceModeInfo();

	DllExport int getCursorIndex() const;
	DllExport int getMissionType() const;

	bool getVisible() const;
	DllExport bool getGotoPlot() const;
	bool getHighlightPlot() const;
	bool getSelectType() const;
	bool getSelectAll() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvInterfaceModeInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iCursorIndex;
	int m_iMissionType;

	bool m_bVisible;
	bool m_bGotoPlot;
	bool m_bHighlightPlot;
	bool m_bSelectType;
	bool m_bSelectAll;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvAdvisorInfo (ADD to Python)
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAdvisorInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvAdvisorInfo();
	virtual ~CvAdvisorInfo();

	const char* getTexture() const;
	int getNumCodes() const;
	int getEnableCode(uint32_t uiCode) const;
	int getDisableCode(uint32_t uiCode) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvAdvisorInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:
	CvString m_szTexture;
	std::vector< std::pair< int, int > > m_vctEnableDisableCodes;
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
	int getWonderConstructRand() const;
	int getBaseAttitude() const;
	int getBasePeaceWeight() const;
	int getPeaceWeightRand() const;
	int getWarmongerRespect() const;
	int getEspionageWeight() const;
	int getRefuseToTalkWarThreshold() const;
	int getNoTechTradeThreshold() const;
	int getTechTradeKnownPercent() const;
	int getMaxGoldTradePercent() const;
	int getMaxGoldPerTurnTradePercent() const;
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
	int getMaxWarRand() const;
	int getMaxWarNearbyPowerRatio() const;
	int getMaxWarDistantPowerRatio() const;
	int getMaxWarMinAdjacentLandPercent() const;
	int getLimitedWarRand() const;
	int getLimitedWarPowerRatio() const;
	int getDogpileWarRand() const;
	int getMakePeaceRand() const;
	int getDeclareWarTradeRand() const;
	int getDemandRebukedSneakProb() const;
	int getDemandRebukedWarProb() const;
	int getRazeCityProb() const;
	int getBuildUnitProb() const;
	int getBaseAttackOddsChange() const;
	int getAttackOddsChangeRand() const;
	int getWorseRankDifferenceAttitudeChange() const;
	int getBetterRankDifferenceAttitudeChange() const;
	int getCloseBordersAttitudeChange() const;
	int getLostWarAttitudeChange() const;
	int getAtWarAttitudeDivisor() const;
	int getAtWarAttitudeChangeLimit() const;
	int getAtPeaceAttitudeDivisor() const;
	int getAtPeaceAttitudeChangeLimit() const;
	int getSameReligionAttitudeChange() const;
	int getSameReligionAttitudeDivisor() const;
	int getSameReligionAttitudeChangeLimit() const;
	int getDifferentReligionAttitudeChange() const;
	int getDifferentReligionAttitudeDivisor() const;
	int getDifferentReligionAttitudeChangeLimit() const;
	int getBonusTradeAttitudeDivisor() const;
	int getBonusTradeAttitudeChangeLimit() const;
	int getOpenBordersAttitudeDivisor() const;
	int getOpenBordersAttitudeChangeLimit() const;
	int getDefensivePactAttitudeDivisor() const;
	int getDefensivePactAttitudeChangeLimit() const;
	int getShareWarAttitudeChange() const;
	int getShareWarAttitudeDivisor() const;
	int getShareWarAttitudeChangeLimit() const;
	int getFavoriteCivicAttitudeChange() const;
	int getFavoriteCivicAttitudeDivisor() const;
	int getFavoriteCivicAttitudeChangeLimit() const;
	int getDemandTributeAttitudeThreshold() const;
	int getNoGiveHelpAttitudeThreshold() const;
	int getTechRefuseAttitudeThreshold() const;
	int getStrategicBonusRefuseAttitudeThreshold() const;
	int getHappinessBonusRefuseAttitudeThreshold() const;
	int getHealthBonusRefuseAttitudeThreshold() const;
	int getMapRefuseAttitudeThreshold() const;
	int getDeclareWarRefuseAttitudeThreshold() const;
	int getDeclareWarThemRefuseAttitudeThreshold() const;
	int getStopTradingRefuseAttitudeThreshold() const;
	int getStopTradingThemRefuseAttitudeThreshold() const;
	int getAdoptCivicRefuseAttitudeThreshold() const;
	int getConvertReligionRefuseAttitudeThreshold() const;
	int getOpenBordersRefuseAttitudeThreshold() const;
	int getDefensivePactRefuseAttitudeThreshold() const;
	int getPermanentAllianceRefuseAttitudeThreshold() const;
	int getVassalRefuseAttitudeThreshold() const;
	int getVassalPowerModifier() const;
	int getFavoriteCivic() const;
	int getFavoriteReligion() const;
	int getFreedomAppreciation() const;

	const char* getArtDefineTag() const;

	// Arrays
	bool hasTrait(int i) const;

	int getFlavorValue(int i) const;
	int getContactRand(int i) const;
	int getContactDelay(int i) const;
	int getMemoryDecayRand(int i) const;
	int getMemoryAttitudePercent(int i) const;
	int getNoWarAttitudeProb(int i) const;
	int getUnitAIWeightModifier(int i) const;
	int getImprovementWeightModifier(int i) const;
	int getDiploPeaceIntroMusicScriptIds(int i) const;
	int getDiploPeaceMusicScriptIds(int i) const;
	int getDiploWarIntroMusicScriptIds(int i) const;
	int getDiploWarMusicScriptIds(int i) const;

	// Other
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
	DllExport const CvArtInfoLeaderhead* getArtInfo() const;
	const char* getLeaderHead() const;
	const char* getButton() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvLeaderHeadInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

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

	int m_iCultureVictoryWeight;
	int m_iSpaceVictoryWeight;
	int m_iConquestVictoryWeight;
	int m_iDominationVictoryWeight;
	int m_iDiplomacyVictoryWeight;

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
//  class : CvWorldInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvWorldInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvWorldInfo();
	virtual ~CvWorldInfo();

	DllExport int getDefaultPlayers() const;
	int getUnitNameModifier() const;
	int getTargetNumCities() const;
	int getNumFreeBuildingBonuses() const;
	int getBuildingPrereqModifier() const;
	int getMaxConscriptModifier() const;
	int getWarWearinessModifier() const;
	int getGridWidth() const;
	int getGridHeight() const;
	int getTerrainGrainChange() const;
	int getFeatureGrainChange() const;
	int getResearchPercent() const;
	int getTradeProfitPercent() const;
	int getDistanceMaintenancePercent() const;
	int getNumCitiesMaintenancePercent() const;
	int getColonyMaintenancePercent() const;
	int getCorporationMaintenancePercent() const;
	int getNumCitiesAnarchyPercent() const;
	int getAdvancedStartPointsMod() const;
	int getCommandersLevelThresholdsPercent() const;
	int getOceanMinAreaSize() const;

	bool read(CvXMLLoadUtility* pXML);

	int getPercent(int iID) const;

	void copyNonDefaults(const CvWorldInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iDefaultPlayers;
	int m_iUnitNameModifier;
	int m_iTargetNumCities;
	int m_iNumFreeBuildingBonuses;
	int m_iBuildingPrereqModifier;
	int m_iMaxConscriptModifier;
	int m_iWarWearinessModifier;
	int m_iGridWidth;
	int m_iGridHeight;
	int m_iTerrainGrainChange;
	int m_iFeatureGrainChange;
	int m_iResearchPercent;
	int m_iTradeProfitPercent;
	int m_iDistanceMaintenancePercent;
	int m_iNumCitiesMaintenancePercent;
	int m_iColonyMaintenancePercent;
	int m_iCorporationMaintenancePercent;
	int m_iNumCitiesAnarchyPercent;
	int m_iAdvancedStartPointsMod;
	int m_iCommandersLevelThresholdsPercent;
	int m_iOceanMinAreaSize;

	IDValueMapPercent m_Percent;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvMapInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CvMapInfo : public CvInfoBase
{
public:
	CvMapInfo();
	virtual ~CvMapInfo();

	int getGridWidth() const				{ return m_iGridWidth; }
	int getGridHeight() const				{ return m_iGridHeight; }

	int getWrapX() const					{ return m_iWrapX; }
	int getWrapY() const					{ return m_iWrapY; }

	const CvString getInitialWBMap() const	{ return m_szInitialWBMap; }
	const CvString getMapScript() const		{ return m_szMapScript; }

	bool read(CvXMLLoadUtility* pXML);

private:
	int m_iGridWidth;
	int m_iGridHeight;

	int m_iWrapX;
	int m_iWrapY;

	CvString m_szInitialWBMap;
	CvString m_szMapScript;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvClimateInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvClimateInfo : public CvInfoBase
{
public:

	CvClimateInfo();
	virtual ~CvClimateInfo();

	int getDesertPercentChange() const;
	int getJungleLatitude() const;
	int getHillRange() const;
	int getPeakPercent() const;

	float getSnowLatitudeChange() const;
	float getTundraLatitudeChange() const;
	float getGrassLatitudeChange() const;
	float getDesertBottomLatitudeChange() const;
	float getDesertTopLatitudeChange() const;
	float getIceLatitude() const;
	float getRandIceLatitude() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvClimateInfo* pClassInfo);

protected:

	int m_iDesertPercentChange;
	int m_iJungleLatitude;
	int m_iHillRange;
	int m_iPeakPercent;

	float m_fSnowLatitudeChange;
	float m_fTundraLatitudeChange;
	float m_fGrassLatitudeChange;
	float m_fDesertBottomLatitudeChange;
	float m_fDesertTopLatitudeChange;
	float m_fIceLatitude;
	float m_fRandIceLatitude;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvSeaLevelInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSeaLevelInfo :	public CvInfoBase
{
public:

	CvSeaLevelInfo();
	virtual ~CvSeaLevelInfo();

	int getSeaLevelChange() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvSeaLevelInfo* pClassInfo);

protected:

	int m_iSeaLevelChange;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvProcessInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvProcessInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvProcessInfo();
	virtual ~CvProcessInfo();

	TechTypes getTechPrereq() const { return m_iTechPrereq; }

	// Arrays

	int getProductionToCommerceModifier(int i) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvProcessInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	TechTypes m_iTechPrereq;

	// Arrays
	int* m_paiProductionToCommerceModifier;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvVoteInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvVoteInfo :	public CvInfoBase
{
public:
	CvVoteInfo();
	virtual ~CvVoteInfo();

	int getPopulationThreshold() const;
	int getStateReligionVotePercent() const;
	int getTradeRoutes() const;
	int getMinVoters() const;

	bool isSecretaryGeneral() const;
	bool isVictory() const;
	bool isFreeTrade() const;
	bool isNoNukes() const;
	bool isCityVoting() const;
	bool isCivVoting() const;
	bool isDefensivePact() const;
	bool isOpenBorders() const;
	bool isForcePeace() const;
	bool isForceNoTrade() const;
	bool isForceWar() const;
	bool isAssignCity() const;

	// Arrays

	bool isForceCivic(int i) const;
	bool isVoteSourceType(int i) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvVoteInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

protected:

	int m_iPopulationThreshold;
	int m_iStateReligionVotePercent;
	int m_iTradeRoutes;
	int m_iMinVoters;

	bool m_bSecretaryGeneral;
	bool m_bVictory;
	bool m_bFreeTrade;
	bool m_bNoNukes;
	bool m_bCityVoting;
	bool m_bCivVoting;
	bool m_bDefensivePact;
	bool m_bOpenBorders;
	bool m_bForcePeace;
	bool m_bForceNoTrade;
	bool m_bForceWar;
	bool m_bAssignCity;

	// Arrays

	bool* m_pbForceCivic;
	bool* m_abVoteSourceTypes;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvProjectInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvProjectInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvProjectInfo();
	virtual ~CvProjectInfo();

	int getVictoryPrereq() const;
	TechTypes getTechPrereq() const { return m_iTechPrereq; }
	int getAnyoneProjectPrereq() const;
	void setAnyoneProjectPrereq(int i);
	int getMaxGlobalInstances() const;
	int getMaxTeamInstances() const;
	int getProductionCost() const;
	int getNukeInterception() const;
	int getTechShare() const;
	int getGlobalMaintenanceModifier() const;
	int getDistanceMaintenanceModifier() const;
	int getNumCitiesMaintenanceModifier() const;
	int getConnectedCityMaintenanceModifier() const;
	int getEveryoneSpecialUnit() const;
	int getEveryoneSpecialBuilding() const;
	int getVictoryDelayPercent() const;
	int getSuccessRate() const;

	bool isSpaceship() const;
	bool isAllowsNukes() const;
	const char* getMovieArtDef() const;

	const char* getCreateSound() const;
	void setCreateSound(const char* szVal);

	// Arrays

	int getBonusProductionModifier(int i) const;
	int getVictoryThreshold(int i) const;
	int getVictoryMinThreshold(int i) const;
	int getProjectsNeeded(int i) const;

	const std::vector<MapCategoryTypes>& getMapCategories() const { return m_aeMapCategoryTypes; }

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvProjectInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	int getWorldHappiness() const;
	int getGlobalHappiness() const;
	int getWorldHealth() const;
	int getGlobalHealth() const;
	int getWorldTradeRoutes() const;
	int getInflationModifier() const;
	bool isTechShareWithHalfCivs() const;
	int getCommerceModifier(int i) const;
	int* getCommerceModifierArray() const;
	int getProjectsNeededVectorSize() const;
	CvString getProjectsNeededNamesVectorElement(int i) const;
	int getProjectsNeededValuesVectorElement(int i) const;

	bool readPass3();
protected:
	int m_iWorldHappiness;
	int m_iGlobalHappiness;
	int m_iWorldHealth;
	int m_iGlobalHealth;
	int m_iWorldTradeRoutes;
	int m_iInflationModifier;
	std::vector<CvString> m_aszProjectsNeededforPass3;
	std::vector<int> m_aiProjectsNeededforPass3;

	int m_iVictoryPrereq;
	TechTypes m_iTechPrereq;
	int m_iAnyoneProjectPrereq;
	int m_iMaxGlobalInstances;
	int m_iMaxTeamInstances;
	int m_iProductionCost;
	int m_iNukeInterception;
	int m_iTechShare;
	int m_iGlobalMaintenanceModifier;
	int m_iDistanceMaintenanceModifier;
	int m_iNumCitiesMaintenanceModifier;
	int m_iConnectedCityMaintenanceModifier;
	int m_iEveryoneSpecialUnit;
	int m_iEveryoneSpecialBuilding;
	int m_iVictoryDelayPercent;
	int m_iSuccessRate;

	bool m_bTechShareWithHalfCivs;
	bool m_bSpaceship;
	bool m_bAllowsNukes;

	CvString m_szCreateSound;
	CvString m_szMovieArtDef;

	int* m_piCommerceModifier;
	int* m_piBonusProductionModifier;
	int* m_piVictoryThreshold;
	int* m_piVictoryMinThreshold;
	int* m_piProjectsNeeded;

	std::vector<MapCategoryTypes> m_aeMapCategoryTypes;
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

	int getChar() const;
	// TGA_INDEXATION 01/21/08 MRGENIE
	int getTGAIndex() const;
	//void setTGAIndex(int i);

	void setChar(int i);
	int getHolyCityChar() const;
	void setHolyCityChar(int i);
	TechTypes getTechPrereq() const { return m_iTechPrereq; }
	int getFreeUnit() const;
	int getNumFreeUnits() const;
	int getSpreadFactor() const;
	int getMissionType() const;
	void setMissionType(int iNewType);

	const char* getTechButton() const;
	const char* getGenericTechButton() const;
	const char* getMovieFile() const;
	const char* getMovieSound() const;
	const char* getSound() const;
	const char* getButtonDisabled() const;

	void setAdjectiveKey(const char* szVal);
	const wchar_t* getAdjectiveKey() const;
	std::wstring pyGetAdjectiveKey() const { return getAdjectiveKey(); }

	// Arrays

	int getGlobalReligionCommerce(int i) const;
	int* getGlobalReligionCommerceArray() const;
	int getHolyCityCommerce(int i) const;
	int* getHolyCityCommerceArray() const;
	int getStateReligionCommerce(int i) const;
	int* getStateReligionCommerceArray() const;
	int getFlavorValue(int i) const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvReligionInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

private:
	CvPropertyManipulators m_PropertyManipulators;

protected:
	// TGA_INDEXATION 01/21/08 MRGENIE
	int m_iTGAIndex;

	int m_iChar;
	int m_iHolyCityChar;
	TechTypes m_iTechPrereq;
	int m_iFreeUnit;
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

	int getChar() const;
	// TGA_INDEXATION 01/21/08 MRGENIE
	int getTGAIndex() const;

	void setChar(int i);
	int getHeadquarterChar() const;
	void setHeadquarterChar(int i);
	TechTypes getTechPrereq() const { return m_iTechPrereq; }
	int getFreeUnit() const;
	int getSpreadFactor() const;
	int getSpreadCost() const;
	int getMaintenance() const;
	int getMissionType() const;
	void setMissionType(int iNewType);

	int getBonusProduced() const;

	const char* getMovieFile() const;
	const char* getMovieSound() const;
	const char* getSound() const;

	const std::vector<BonusTypes>& getPrereqBonuses() const;

	// Arrays
	int getHeadquarterCommerce(int i) const;
	int* getHeadquarterCommerceArray() const;
	int getCommerceProduced(int i) const;
	int* getCommerceProducedArray() const;
	int getYieldProduced(int i) const;
	int* getYieldProducedArray() const;

	bool read(CvXMLLoadUtility* pXML);

	TechTypes getObsoleteTech() const;
	int getSpread() const;
	int getHealth() const;
	int getHappiness() const;
	int getMilitaryProductionModifier() const;
	int getFreeXP() const;
	int getPrereqGameOption() const;

	int getPrereqBuilding(int i) const;
	int getPrereqBuildingVectorSize() const;
	CvString getPrereqBuildingNamesVectorElement(const int i) const;
	int getPrereqBuildingValuesVectorElement(const int i) const;

	bool isCompetingCorporation(int i) const;
	int getCompetingCorporationVectorSize() const;
	CvString getCompetingCorporationNamesVectorElement(const int i) const;
	bool getCompetingCorporationValuesVectorElement(const int i) const;

	bool readPass3();
	int getYieldChange(int i) const;
	int* getYieldChangeArray() const;
	int getCommerceChange(int i) const;
	int* getCommerceChangeArray() const;

	void copyNonDefaults(const CvCorporationInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

private:
	CvPropertyManipulators m_PropertyManipulators;

protected:
	// TGA_INDEXATION 01/21/08 MRGENIE
	int m_iTGAIndex;

	int m_iChar;
	int m_iHeadquarterChar;
	TechTypes m_iTechPrereq;
	int m_iFreeUnit;
	int m_iSpreadFactor;
	int m_iSpreadCost;
	int m_iMaintenance;
	int m_iMissionType;
	int m_iBonusProduced;

	TechTypes m_iObsoleteTech;
	int m_iSpread;
	int m_iHealth;
	int m_iHappiness;
	int m_iFreeXP;
	int m_iMilitaryProductionModifier;
	int m_iPrereqGameOption;

	int* m_paiPrereqBuilding;
	std::vector<CvString> m_aszPrereqBuildingforPass3;
	std::vector<int> m_aiPrereqBuildingforPass3;

	bool* m_pabCompetingCorporation;
	std::vector<CvString> m_aszCompetingCorporationforPass3;
	std::vector<bool> m_abCompetingCorporationforPass3;

	int* m_piYieldChange;
	int* m_piCommerceChange;

	CvString m_szMovieFile;
	CvString m_szMovieSound;
	CvString m_szSound;

	std::vector<BonusTypes> m_vPrereqBonuses;

	// Arrays
	int* m_paiHeadquarterCommerce;
	int* m_paiCommerceProduced;
	int* m_paiYieldProduced;
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
	bool isValidTrait(bool bGameStart=false) const;

	// bool vector without delayed resolution
	int getNotOnGameOption(int i) const;
	int getNumNotOnGameOptions() const;
	bool isNotOnGameOption(int i) const;

	int getOnGameOption(int i) const;
	int getNumOnGameOptions() const;
	bool isOnGameOption(int i) const;

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

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCursorInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCursorInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvCursorInfo();
	virtual ~CvCursorInfo();

	DllExport const char* getPath();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvCursorInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szPath;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvThroneRoomCamera
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvThroneRoomCamera : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvThroneRoomCamera();
	virtual ~CvThroneRoomCamera();

	DllExport const char* getFileName();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvThroneRoomCamera* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szFileName;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvThroneRoomInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvThroneRoomInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvThroneRoomInfo();
	virtual ~CvThroneRoomInfo();

	DllExport const char* getEvent();
	DllExport const char* getNodeName();
	DllExport int getFromState();
	DllExport int getToState();
	DllExport int getAnimation();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvThroneRoomInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iFromState;
	int m_iToState;
	int m_iAnimation;
	CvString m_szEvent;
	CvString m_szNodeName;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvThroneRoomStyleInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvThroneRoomStyleInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvThroneRoomStyleInfo();
	virtual ~CvThroneRoomStyleInfo();

	DllExport const char* getArtStyleType()	{ return m_szArtStyleType; }
	DllExport const char* getEraType()		{ return m_szEraType; }
	DllExport const char* getFileName()		{ return m_szFileName; }

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvThroneRoomStyleInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szArtStyleType;
	CvString m_szEraType;
	CvString m_szFileName;
	std::vector<CvString> m_aNodeNames;
	std::vector<CvString> m_aTextureNames;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSlideShowInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSlideShowInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvSlideShowInfo();
	virtual ~CvSlideShowInfo();

	DllExport const char* getPath()				{ return m_szPath; }
	DllExport const char* getTransitionType()	{ return m_szTransitionType; }
	DllExport float getStartTime()				{ return m_fStartTime; }

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvSlideShowInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	float m_fStartTime;
	CvString m_szPath;
	CvString m_szTransitionType;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSlideShowRandomInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSlideShowRandomInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvSlideShowRandomInfo();
	virtual ~CvSlideShowRandomInfo();

	DllExport const char* getPath();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvSlideShowRandomInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szPath;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvWorldPickerInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvWorldPickerInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvWorldPickerInfo();
	virtual ~CvWorldPickerInfo();

	DllExport const char* getMapName();
	DllExport const char* getModelFile();
	DllExport int getNumSizes();
	DllExport float getSize(int index);
	DllExport int getNumClimates();
	DllExport const char* getClimatePath(int index);
	DllExport int getNumWaterLevelDecals();
	DllExport const char* getWaterLevelDecalPath(int index);
	DllExport int getNumWaterLevelGloss();
	DllExport const char* getWaterLevelGlossPath(int index);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvWorldPickerInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szMapName;
	CvString m_szModelFile;
	std::vector<float> m_aSizes;
	std::vector<CvString> m_aClimates;
	std::vector<CvString> m_aWaterLevelDecals;
	std::vector<CvString> m_aWaterLevelGloss;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSpaceShipInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpaceShipInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvSpaceShipInfo();
	virtual ~CvSpaceShipInfo();

	DllExport const char* getNodeName();
	DllExport const char* getProjectName();
	void setProjectName(const char* szVal);
	DllExport ProjectTypes getProjectType();
	DllExport AxisTypes getCameraUpAxis();
	DllExport SpaceShipInfoTypes getSpaceShipInfoType();
	DllExport int getPartNumber();
	DllExport int getArtType();
	DllExport int getEventCode();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvSpaceShipInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szNodeName;
	CvString m_szProjectName;
	ProjectTypes m_eProjectType;
	AxisTypes m_eCameraUpAxis;
	int m_iPartNumber;
	int m_iArtType;
	int m_iEventCode;
	SpaceShipInfoTypes m_eSpaceShipInfoType;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvAnimationInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef std::vector<std::pair<int,float> > CvAnimationPathDefinition;
typedef std::pair<int,int >			CvAnimationCategoryDefinition;

class CvAnimationPathInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
	public:

		CvAnimationPathInfo();
		virtual ~CvAnimationPathInfo();

		DllExport int getPathCategory( int i );
		float getPathParameter( int i );
		DllExport int getNumPathDefinitions();
		DllExport CvAnimationPathDefinition* getPath();
		DllExport bool isMissionPath() const;

		bool read(CvXMLLoadUtility* pXML);
		void copyNonDefaults(CvAnimationPathInfo* pClassInfo);

	//---------------------------------------PRIVATE MEMBER VARIABLES---------------------------------
	private:

		CvAnimationPathDefinition 	m_vctPathDefinition;	//!< Animation path definitions, pair(category,param).
		bool						m_bMissionPath;			//!< True if this animation is used in missions
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvAnimationInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAnimationCategoryInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
	public:

		CvAnimationCategoryInfo();
		virtual ~CvAnimationCategoryInfo();

		DllExport int getCategoryBaseID( );
		DllExport int getCategoryDefaultTo( );

		bool read(CvXMLLoadUtility* pXML);
		void copyNonDefaults(const CvAnimationCategoryInfo* pClassInfo);

	//---------------------------------------PRIVATE MEMBER VARIABLES---------------------------------
	private:

		CvAnimationCategoryDefinition	m_kCategory;		//!< The pair(base IDs, default categories) defining the animation categories
		CvString						m_szDefaultTo;		//!< Holds the default to parameter, until all categories are read
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEntityEventInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEntityEventInfo : public CvInfoBase
{
		//---------------------------PUBLIC INTERFACE---------------------------------
	public:

		CvEntityEventInfo();
		virtual ~CvEntityEventInfo();

		bool read(CvXMLLoadUtility* pXML);
		void copyNonDefaults(const CvEntityEventInfo* pClassInfo);

		DllExport AnimationPathTypes getAnimationPathType(int iIndex = 0) const;
		DllExport EffectTypes getEffectType(int iIndex = 0) const;
		int getAnimationPathCount() const;
		int getEffectTypeCount() const;

		bool getUpdateFormation() const;

		//---------------------------------------PRIVATE MEMBER VARIABLES---------------------------------
	private:

		std::vector<AnimationPathTypes>	m_vctAnimationPathType;
		std::vector<EffectTypes>		m_vctEffectTypes;
		bool							m_bUpdateFormation;
};

// The below classes are for the ArtFile Management
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  classes : CvArtInfos
//
// This is also an abstract BASE class
//
//  DESC:  Used to store data from Art\Civ4ArtDefines.xml
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAssetInfoBase : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvAssetInfoBase()  {}
	virtual ~CvAssetInfoBase() {}

	const char* getTag() const;
	void setTag(const char* szDesc);

	DllExport const char* getPath() const;
	void setPath(const char* szDesc);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvAssetInfoBase* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szPath;
};

class CvArtInfoAsset : public CvAssetInfoBase
{
public:

	CvArtInfoAsset() {}
	virtual ~CvArtInfoAsset() {}

	DllExport const char* getNIF() const;
	DllExport const char* getKFM() const;

	void setNIF(const char* szDesc);
	void setKFM(const char* szDesc);

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvArtInfoAsset* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szKFM;
	CvString m_szNIF;
};

//
//////////////////////////////////////////////////////////////////////////
// Another base class
//////////////////////////////////////////////////////////////////////////

class CvArtInfoScalableAsset :
	public CvArtInfoAsset,
	public CvScalableInfo
{
public:
	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvArtInfoScalableAsset* pClassInfo);
};

// todoJS: Remove empty classes if additional items are not added

class CvArtInfoInterface : public CvArtInfoAsset
{
public:

	CvArtInfoInterface() {}
	virtual ~CvArtInfoInterface() {}
};

class CvArtInfoMisc : public CvArtInfoScalableAsset
{
public:

	CvArtInfoMisc() {}
	virtual ~CvArtInfoMisc() {}
};

class CvArtInfoMovie : public CvArtInfoAsset
{
public:

	CvArtInfoMovie() {}
	virtual ~CvArtInfoMovie() {}

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvArtInfoMovie* pClassInfo);
};

class CvArtInfoUnit : public CvArtInfoScalableAsset
{
public:

	CvArtInfoUnit();
	virtual ~CvArtInfoUnit();

	DllExport bool getActAsRanged() const;
	DllExport bool getActAsLand() const;
	DllExport bool getActAsAir() const;

	DllExport const char* getShaderNIF() const;
	void setShaderNIF(const char* szDesc);

	DllExport const char* getShadowNIF() const;
	DllExport float getShadowScale() const;
	DllExport const char* getShadowAttachNode() const;
	DllExport int getDamageStates() const;

	DllExport const char* getTrailTexture() const;
	DllExport float getTrailWidth() const;
	DllExport float getTrailLength() const;
	DllExport float getTrailTaper() const;
	DllExport float getTrailFadeStarTime() const;
	DllExport float getTrailFadeFalloff() const;

	DllExport float getBattleDistance() const;
	DllExport float getRangedDeathTime() const;
	DllExport float getExchangeAngle() const;
	DllExport bool getCombatExempt() const;
	bool getSmoothMove() const;
	float getAngleInterpRate() const;
	DllExport float getBankRate() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvArtInfoUnit* pClassInfo);

	const char* getTrainSound() const;
	void setTrainSound(const char* szVal);
	DllExport int getRunLoopSoundTag() const;
	DllExport int getRunEndSoundTag() const;
	DllExport int getPatrolSoundTag() const;
	int getSelectionSoundScriptId() const;
	int getActionSoundScriptId() const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:
	CvString m_szShaderNIF;		//!< The NIF used if the graphics card supports shaders
	CvString m_szShadowNIF;		//!< The shadow blob NIF to use for the unit
	CvString m_szShadowAttach;	//!< The name of the node to which the shadow takes its x,y position

	float m_fShadowScale;		//!< the scale of the unit's shadow.

	int m_iDamageStates;		//!< The maximum number of damage states this unit type supports
	bool m_bActAsRanged;		//!< true if the unit acts as a ranged unit in combat (but may or may not be actually a ranged unit)
	bool m_bActAsLand;
	bool m_bActAsAir;
	bool m_bCombatExempt;		//!< true if the unit is 'exempt' from combat - ie. it just flees instead of dying
	bool m_bSmoothMove;			//!< true if the unit should do non-linear interpolation for moves

	CvString m_szTrailTexture;	//!< The trail texture of the unit
	float m_fTrailWidth;		//!< The width of the trail
	float m_fTrailLength;		//!< The length of the trail
	float m_fTrailTaper;		//!< Tapering of the trail
	float m_fTrailFadeStartTime;//!< Time after which the trail starts to fade
	float m_fTrailFadeFalloff;	//!< Speed at which the fade happens

	float m_fBattleDistance;	//!< The preferred attack distance of this unit (1.0 == plot size)
	float m_fRangedDeathTime;	//!< The offset from firing in which an opponent should die
	float m_fExchangeAngle;		//!< The angle at which the unit does combat.
	float m_fAngleInterRate;	//!< The rate at which the units' angle interpolates
	float m_fBankRate;

	CvString m_szTrainSound;
	int m_iRunLoopSoundTag;
	int m_iRunEndSoundTag;
	int m_iPatrolSoundTag;
	int m_iSelectionSoundScriptId;
	int m_iActionSoundScriptId;
};

class CvArtInfoBuilding : public CvArtInfoScalableAsset
{
public:

	CvArtInfoBuilding();
	virtual ~CvArtInfoBuilding();

	bool isAnimated() const;
	DllExport const char* getLSystemName() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvArtInfoBuilding* pClassInfo);

protected:

	bool m_bAnimated;
	CvString m_szLSystemName;
};

class CvArtInfoCivilization : public CvArtInfoAsset
{
public:

	CvArtInfoCivilization();
	virtual ~CvArtInfoCivilization();

	bool isWhiteFlag() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvArtInfoCivilization* pClassInfo);

protected:

	bool m_bWhiteFlag;
};

class CvArtInfoLeaderhead : public CvArtInfoAsset
{
public:

	CvArtInfoLeaderhead() {}
	virtual ~CvArtInfoLeaderhead() {}

	DllExport const char* getNoShaderNIF() const;
	void setNoShaderNIF(const char* szNIF);
	DllExport const char* getBackgroundKFM() const;
	void setBackgroundKFM( const char* szKFM);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvArtInfoLeaderhead* pClassInfo);

protected:

	CvString m_szNoShaderNIF;
	CvString m_szBackgroundKFM;
};

class CvArtInfoBonus : public CvArtInfoScalableAsset
{
public:
	CvArtInfoBonus();
	virtual ~CvArtInfoBonus() {}

	int getFontButtonIndex() const;

	DllExport const char* getShaderNIF() const;
	void setShaderNIF(const char* szDesc);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvArtInfoBonus* pClassInfo);

protected:
	CvString m_szShaderNIF;		//!< The NIF used if the graphics card supports shaders
	int m_iFontButtonIndex;
};

class CvArtInfoImprovement : public CvArtInfoScalableAsset
{
public:

	CvArtInfoImprovement();
	virtual ~CvArtInfoImprovement();

	DllExport const char* getShaderNIF() const;
	void setShaderNIF(const char* szDesc);

	bool isExtraAnimations() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvArtInfoImprovement* pClassInfo);

protected:
	CvString m_szShaderNIF;		//!< The NIF used if the graphics card supports shaders

	bool m_bExtraAnimations;
};

typedef std::vector<std::pair<int, int> > CvTextureBlendSlotList;

class CvArtInfoTerrain : public CvArtInfoAsset
{
public:
	//!< number to blend textures.
	static const int NUM_TEXTURE_BLENDS = 16;

	CvArtInfoTerrain();

	DllExport const char* getBaseTexture();
	void setBaseTexture(const char* szTmp);
	DllExport const char* getGridTexture();
	void setGridTexture(const char* szTmp);
	DllExport const char* getDetailTexture();
	void setDetailTexture(const char* szTmp);
	DllExport int getLayerOrder();
	DllExport bool useAlphaShader();
	DllExport CvTextureBlendSlotList& getBlendList(int blendMask);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvArtInfoTerrain* pClassInfo);

protected:

	//!< Detail texture associated with the Terrain base texture
	CvString m_szDetailTexture;
	CvString m_szGridTexture;

	//!< Layering order of texture
	int m_iLayerOrder;
	bool m_bAlphaShader;

	//!< Array of Textureslots per blend tile
	CvTextureBlendSlotList m_pTextureSlots[NUM_TEXTURE_BLENDS];
	CvString m_pSlotNames[NUM_TEXTURE_BLENDS];
};

class CvArtInfoFeature : public CvArtInfoScalableAsset
{
public:

	CvArtInfoFeature();
	virtual ~CvArtInfoFeature();

	DllExport bool isAnimated() const;
	DllExport bool isRiverArt() const;
	DllExport TileArtTypes getTileArtType() const;
	DllExport LightTypes getLightType() const;

	void dump();

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvArtInfoFeature* pClassInfo);

	class FeatureArtModel
	{
	public:
		FeatureArtModel(const CvString &modelFile, RotationTypes rotation)
			: m_szModelFile(modelFile)
			, m_eRotation(rotation)
		{}

		const CvString& getModelFile() const
		{
			return m_szModelFile;
		}

		RotationTypes getRotation() const
		{
			return m_eRotation;
		}

	private:
		CvString m_szModelFile;
		RotationTypes m_eRotation;
	};

	class FeatureArtPiece
	{
	public:
		explicit FeatureArtPiece(int connectionMask)
		{
			m_iConnectionMask = connectionMask;
		}

		int getConnectionMask() const
		{
			return m_iConnectionMask;
		}

		int getNumArtModels() const
		{
			return m_aArtModels.size();
		}

		const FeatureArtModel& getArtModel(int index) const
		{
			FAssertMsg((index >= 0) && (index < (int) m_aArtModels.size()), "[Jason] Invalid feature model file index.");
			return m_aArtModels[index];
		}

	private:
		std::vector<FeatureArtModel> m_aArtModels;
		int m_iConnectionMask;

		friend CvArtInfoFeature;
	};

	class FeatureDummyNode
	{
	public:
		FeatureDummyNode(const CvString &tagName, const CvString &nodeName)
			: m_szTag(tagName)
			, m_szName(nodeName)
		{}

		const CvString getTagName() const
		{
			return m_szTag;
		}

		const CvString getNodeName() const
		{
			return m_szName;
		}

	private:
		CvString m_szTag;
		CvString m_szName;
	};

	class FeatureVariety
	{
	public:
		FeatureVariety()
		{
		}

		const CvString& getVarietyButton() const
		{
			return m_szVarietyButton;
		}

		const FeatureArtPiece& getFeatureArtPiece(int index) const
		{
			FAssertMsg((index >= 0) && (index < (int) m_aFeatureArtPieces.size()), "[Jason] Invalid feature art index.");
			return m_aFeatureArtPieces[index];
		}

		const FeatureArtPiece& getFeatureArtPieceFromConnectionMask(int connectionMask) const
		{
			for(int i=0;i<(int)m_aFeatureArtPieces.size();i++)
				if(m_aFeatureArtPieces[i].getConnectionMask() == connectionMask)
					return m_aFeatureArtPieces[i];

			FErrorMsg("[Jason] Failed to find feature art piece with valid connection mask.");
			return m_aFeatureArtPieces[0];
		}

		const CvString getFeatureDummyNodeName(const CvString &tagName) const
		{
			for(int i=0;i<(int)m_aFeatureDummyNodes.size();i++)
			{
				if(m_aFeatureDummyNodes[i].getTagName().CompareNoCase(tagName) == 0)
					return m_aFeatureDummyNodes[i].getNodeName();
			}

			FErrorMsg("[Jason] Failed to find dummy tag name.");
			return "";
		}

		const CvString getFeatureDummyTag(const CvString &nodeName) const
		{
			for(int i=0;i<(int)m_aFeatureDummyNodes.size();i++)
			{
				if(m_aFeatureDummyNodes[i].getNodeName().CompareNoCase(nodeName) == 0)
					return m_aFeatureDummyNodes[i].getTagName();
			}

			return "";
		}

		FeatureArtPiece& createFeatureArtPieceFromConnectionMask(int connectionMask)
		{
			for(int i=0;i<(int)m_aFeatureArtPieces.size();i++)
				if(m_aFeatureArtPieces[i].getConnectionMask() == connectionMask)
					return m_aFeatureArtPieces[i];

			m_aFeatureArtPieces.push_back(FeatureArtPiece(connectionMask));
			return m_aFeatureArtPieces.back();
		}

		void createFeatureDummyNode(const CvString &tagName, const CvString &nodeName)
		{
			m_aFeatureDummyNodes.push_back(FeatureDummyNode(tagName, nodeName));
		}

	protected:
		std::vector<FeatureArtPiece> m_aFeatureArtPieces;
		std::vector<FeatureDummyNode> m_aFeatureDummyNodes;
		CvString m_szVarietyButton;

		friend CvArtInfoFeature;
	};

	DllExport const FeatureVariety &getVariety(int index) const;
	DllExport int getNumVarieties() const;
	const std::string getFeatureDummyNodeName(int variety, const std::string tagName) const;

protected:

	int getConnectionMaskFromString(const CvString& connectionString) const;
	int getRotatedConnectionMask(int connectionMask, RotationTypes rotation) const;

	bool m_bAnimated;
	bool m_bRiverArt;
	TileArtTypes m_eTileArtType;
	LightTypes m_eLightType;
	std::vector<FeatureVariety> m_aFeatureVarieties;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEmphasizeInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEmphasizeInfo :
	public CvInfoBase
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvEmphasizeInfo();
	virtual ~CvEmphasizeInfo();

	bool isAvoidGrowth() const;
	bool isGreatPeople() const;

	// Afforess 02/10/10
	bool isAvoidAngryCitizens() const;
	bool isAvoidUnhealthyCitizens() const;
	// ! Afforess

	// Arrays
	int getYieldChange(int i) const;
	int getCommerceChange(int i) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvEmphasizeInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:
	bool m_bAvoidGrowth;
	bool m_bGreatPeople;

	// Afforess 02/10/10
	bool m_bAvoidAngryCitizens;
	bool m_bAvoidUnhealthyCitizens;
	// ! Afforess

	// Arrays
	int* m_piYieldModifiers;
	int* m_piCommerceModifiers;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvUpkeepInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUpkeepInfo :
	public CvInfoBase
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvUpkeepInfo();
	virtual ~CvUpkeepInfo();

	int getPopulationPercent() const;
	int getCityPercent() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvUpkeepInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iPopulationPercent;
	int m_iCityPercent;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCultureLevelInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCultureLevelInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvCultureLevelInfo();
	virtual ~CvCultureLevelInfo();

	int getCityDefenseModifier() const;

	int getCityRadius() const;
	int getMaxWorldWonders() const;
	int getMaxTeamWonders() const;
	int getMaxNationalWonders() const;
	int getMaxNationalWondersOCC() const;
	int getPrereqGameOption() const;

	int getSpeedThreshold(int i) const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvCultureLevelInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iCityDefenseModifier;
	int m_iCityRadius;
	int m_iMaxWorldWonders;
	int m_iMaxTeamWonders;
	int m_iMaxNationalWonders;
	int m_iMaxNationalWondersOCC;
	int m_iPrereqGameOption;

	int* m_paiSpeedThreshold;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEraInfo
//
//  DESC:   Used to manage different types of Art Styles
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEraInfo :
	public CvInfoBase
{
public:

	CvEraInfo();
	virtual ~CvEraInfo();

	int getStartingUnitMultiplier() const;
	int getStartingDefenseUnits() const;
	int getStartingWorkerUnits() const;
	int getStartingExploreUnits() const;
	int getAdvancedStartPoints() const;
	int getStartingGold() const;
	int getFreePopulation() const;
	int getStartPercent() const;
	int getGrowthPercent() const;
	int getTrainPercent() const;
	int getConstructPercent() const;
	int getCreatePercent() const;
	int getResearchPercent() const;
	int getBuildPercent() const;
	int getImprovementPercent() const;
	int getGreatPeoplePercent() const;
	int getAnarchyPercent() const;
	int getEventChancePerTurn() const;
	int getSoundtrackSpace() const;
	int getNumSoundtracks() const;
	int getBeelineStingsTechCostModifier() const;
	int getInitialCityMaintenancePercent() const;

	const char* getAudioUnitVictoryScript() const;
	const char* getAudioUnitDefeatScript() const;

	bool isNoGoodies() const;
	bool isNoAnimals() const;
	bool isNoBarbUnits() const;
	bool isNoBarbCities() const;
	bool isFirstSoundtrackFirst() const;

	// Arrays

	int getSoundtracks(int i) const;
	int getCitySoundscapeSciptId(int i) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvEraInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

protected:

	int m_iStartingUnitMultiplier;
	int m_iStartingDefenseUnits;
	int m_iStartingWorkerUnits;
	int m_iStartingExploreUnits;
	int m_iAdvancedStartPoints;
	int m_iStartingGold;
	int m_iFreePopulation;
	int m_iStartPercent;
	int m_iGrowthPercent;
	int m_iTrainPercent;
	int m_iConstructPercent;
	int m_iCreatePercent;
	int m_iResearchPercent;
	int m_iBuildPercent;
	int m_iImprovementPercent;
	int m_iGreatPeoplePercent;
	int m_iAnarchyPercent;
	int m_iEventChancePerTurn;
	int m_iSoundtrackSpace;
	int m_iNumSoundtracks;
	int m_iBeelineStingsTechCostModifier;
	int m_iInitialCityMaintenancePercent;

	CvString m_szAudioUnitVictoryScript;
	CvString m_szAudioUnitDefeatScript;

	bool m_bNoGoodies;
	bool m_bNoAnimals;
	bool m_bNoBarbUnits;
	bool m_bNoBarbCities;
	bool m_bFirstSoundtrackFirst;

	// Arrays

	int* m_paiSoundtracks;
	int* m_paiCitySoundscapeSciptIds;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvColorInfo
//
//  DESC:   Used to manage different types of Art Styles
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvColorInfo :
	public CvInfoBase
{
public:

	CvColorInfo();
	virtual ~CvColorInfo();

	DllExport const NiColorA& getColor() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvColorInfo* pClassInfo);

protected:
	NiColorA m_Color;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPlayerColorInfo (ADD to Python)
//
//  DESC:   Used to manage different types of Art Styles
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPlayerColorInfo :
	public CvInfoBase
{
public:

	CvPlayerColorInfo();
	virtual ~CvPlayerColorInfo();

	DllExport int getColorTypePrimary() const;
	DllExport int getColorTypeSecondary() const;
	int getTextColorType() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvPlayerColorInfo* pClassInfo);

protected:

	int m_iColorTypePrimary;
	int m_iColorTypeSecondary;
	int m_iTextColorType;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvLandscapeInfo
//
//  Purpose:	This info acts as the Civ4Terrain.ini and is initialize in CvXmlLoadUtility with the infos in
//					XML/Terrain/TerrainSettings.xml
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvLandscapeInfo :
	public CvInfoBase
{
	public:

		CvLandscapeInfo();
		virtual ~CvLandscapeInfo() {}

		int getFogR() const;
		int getFogG() const;
		int getFogB() const;
		DllExport int getHorizontalGameCell() const;
		DllExport int getVerticalGameCell() const;
		DllExport int getPlotsPerCellX() const;
		DllExport int getPlotsPerCellY() const;
		DllExport int getHorizontalVertCnt() const;
		DllExport int getVerticalVertCnt() const;
		DllExport int getWaterHeight() const;

		float getTextureScaleX() const;
		float getTextureScaleY() const;
		DllExport float getZScale() const;

		bool isUseTerrainShader() const;
		bool isUseLightmap() const;
		bool isRandomMap() const;
		DllExport float getPeakScale() const;
		DllExport float getHillScale() const;

		const char* getSkyArt() const		{ return m_szSkyArt; }
		const char* getHeightMap() const	{ return m_szHeightMap; }
		const char* getTerrainMap() const	{ return m_szTerrainMap; }
		const char* getNormalMap() const	{ return m_szNormalMap; }
		const char* getBlendMap() const		{ return m_szBlendMap; }

		bool read(CvXMLLoadUtility* pXML);
		void copyNonDefaults(const CvLandscapeInfo* pClassInfo);

	protected:

		int m_iFogR;
		int m_iFogG;
		int m_iFogB;
		int m_iHorizontalGameCell;
		int m_iVerticalGameCell;
		int m_iPlotsPerCellX;
		int m_iPlotsPerCellY;
		int m_iHorizontalVertCnt;
		int m_iVerticalVertCnt;
		int m_iWaterHeight;

		float m_fTextureScaleX;
		float m_fTextureScaleY;
		float m_fZScale;
		float m_fPeakScale;
		float m_fHillScale;

		bool m_bUseTerrainShader;
		bool m_bUseLightmap;
		bool m_bRandomMap;

		CvString m_szSkyArt;
		CvString m_szHeightMap;
		CvString m_szTerrainMap;
		CvString m_szNormalMap;
		CvString m_szBlendMap;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvGameText
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameText : public CvInfoBase
{
public:
	DllExport CvGameText();

	const wchar_t* getText() const;
	void setText(const wchar_t* szText);

	// for Python
	std::wstring pyGetText() const { return getText(); }

	void setGender(const wchar_t* szGender) { m_szGender = szGender;	}
	const wchar_t* getGender() const { return m_szGender; }

	void setPlural(const wchar_t* szPlural) { m_szPlural = szPlural; }
	const wchar_t* getPlural() const { return m_szPlural; }

	DllExport int getNumLanguages() const; // not static for Python access
	DllExport void setNumLanguages(int iNum); // not static for Python access

	bool read(CvXMLLoadUtility* pXML);

	virtual void read(FDataStreamBase*) {}
	virtual void write(FDataStreamBase*) {}

	static void setLanguage(const CvWString& szLanguage) { m_szLanguage = szLanguage; }

protected:

	static CvWString m_szLanguage;

	CvWString m_szText;
	CvWString m_szGender;
	CvWString m_szPlural;

	static int NUM_LANGUAGES;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvDiplomacyTextInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvDiplomacyTextInfo :	public CvInfoBase
{
	friend class CvXMLLoadUtility;		// so it can access private vars to initialize the class
public:
	struct Response
	{
		Response() :
			m_iNumDiplomacyText(0),
			m_pbCivilizationTypes(NULL),
			m_pbLeaderHeadTypes(NULL),
			m_pbAttitudeTypes(NULL),
			m_pbDiplomacyPowerTypes(NULL),
			m_paszDiplomacyText(NULL)
		{
		}

		virtual ~Response ()
		{
			SAFE_DELETE_ARRAY(m_pbCivilizationTypes);
			SAFE_DELETE_ARRAY(m_pbLeaderHeadTypes);
			SAFE_DELETE_ARRAY(m_pbAttitudeTypes);
			SAFE_DELETE_ARRAY(m_pbDiplomacyPowerTypes);
			SAFE_DELETE_ARRAY(m_paszDiplomacyText);
		}

		int m_iNumDiplomacyText;
		bool* m_pbCivilizationTypes;
		bool* m_pbLeaderHeadTypes;
		bool* m_pbAttitudeTypes;
		bool* m_pbDiplomacyPowerTypes;
		CvString* m_paszDiplomacyText;	// needs to be public for xml load assignment
	};

	CvDiplomacyTextInfo();
	virtual ~CvDiplomacyTextInfo() { uninit(); }

	// note - Response member vars allocated by CvXmlLoadUtility
	void init(int iNum);
	void uninit();

	const Response& getResponse(int iNum) const { return m_pResponses[iNum]; }
	int getNumResponses() const;

	bool getCivilizationTypes(int i, int j) const;
	bool getLeaderHeadTypes(int i, int j) const;
	bool getAttitudeTypes(int i, int j) const;
	bool getDiplomacyPowerTypes(int i, int j) const;

	int getNumDiplomacyText(int i) const;

	const char* getDiplomacyText(int i, int j) const;

	bool read(CvXMLLoadUtility* pXML);

private:

	int m_iNumResponses;
	Response* m_pResponses;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEffectInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEffectInfo :
	public CvInfoBase,
	public CvScalableInfo
{
public:

	CvEffectInfo();
	virtual ~CvEffectInfo();

	DllExport const char* getPath() const { return m_szPath; }
	void setPath(const char* szVal) { m_szPath = szVal; }
	float getUpdateRate( ) const { return m_fUpdateRate; };
	void setUpdateRate( float fUpdateRate ) { m_fUpdateRate = fUpdateRate; }
	bool isProjectile() const { return m_bProjectile; };
	float getProjectileSpeed() const { return m_fProjectileSpeed; };
	float getProjectileArc() const { return m_fProjectileArc; };
	bool isSticky() const { return m_bSticky; };

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvEffectInfo* pClassInfo);

private:
	CvString m_szPath;
	float m_fUpdateRate;
	bool m_bProjectile;
	bool m_bSticky;
	float m_fProjectileSpeed;
	float m_fProjectileArc;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvAttachableInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAttachableInfo :
	public CvInfoBase,
	public CvScalableInfo
{
public:

	CvAttachableInfo();
	virtual ~CvAttachableInfo();

	DllExport const char* getPath() const { return m_szPath; }
	void setPath(const char* szVal) { m_szPath = szVal; }

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvAttachableInfo* pClassInfo);

private:
	CvString m_szPath;
	float m_fUpdateRate;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvGameOptionInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameOptionInfo : public CvInfoBase
{
public:
	CvGameOptionInfo();
	virtual ~CvGameOptionInfo();

	DllExport bool getDefault() const { return m_bDefault; }
	DllExport bool getVisible() const { return m_bVisible; }

	//TB Tags
	const std::vector<GameOptionTypes>& getEnforcesGameOptionOnTypes() const  { return m_aEnforcesGameOptionOnTypes; }
	const std::vector<GameOptionTypes>& getEnforcesGameOptionOffTypes() const { return m_aEnforcesGameOptionOffTypes; }

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvGameOptionInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

private:
	bool m_bDefault;
	bool m_bVisible;
	//TB Tags
	std::vector<GameOptionTypes> m_aEnforcesGameOptionOnTypes;
	std::vector<GameOptionTypes> m_aEnforcesGameOptionOffTypes;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvMPOptionInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMPOptionInfo :
	public CvInfoBase
{
public:
	CvMPOptionInfo();
	virtual ~CvMPOptionInfo();

	bool getDefault() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvMPOptionInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

private:
	bool m_bDefault;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvForceControlInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvForceControlInfo :
	public CvInfoBase
{
public:
	CvForceControlInfo();
	virtual ~CvForceControlInfo();

	bool getDefault() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvForceControlInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

private:
	bool m_bDefault;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPlayerOptionInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPlayerOptionInfo :
	public CvInfoBase
{
public:
	CvPlayerOptionInfo();
	virtual ~CvPlayerOptionInfo();

	DllExport bool getDefault() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvPlayerOptionInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

private:
	bool m_bDefault;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvGraphicOptionInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGraphicOptionInfo :
	public CvInfoBase
{
public:
	CvGraphicOptionInfo();
	virtual ~CvGraphicOptionInfo();

	DllExport bool getDefault() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvGraphicOptionInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

private:
	bool m_bDefault;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEventTriggerInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEventTriggerInfo : public CvInfoBase
{
	friend class CvXMLLoadUtility;

public:
	CvEventTriggerInfo();
	virtual ~CvEventTriggerInfo();

	int getPercentGamesActive() const;
	int getProbability() const;
	int getNumUnits() const;
	int getNumBuildings() const;
	int getNumUnitsGlobal() const;
	int getNumBuildingsGlobal() const;
	int getNumPlotsRequired() const;
	int getPlotType() const;
	int getNumReligions() const;
	int getNumCorporations() const;
	int getOtherPlayerShareBorders() const;
	int getOtherPlayerHasTech() const;
	int getCityFoodWeight() const;
	int getCivic() const;
	int getMinPopulation() const;
	int getMaxPopulation() const;
	int getMinMapLandmass() const;
	int getMinOurLandmass() const;
	int getMaxOurLandmass() const;
	int getMinDifficulty() const;
	int getAngry() const;
	int getUnhealthy() const;
	int getUnitDamagedWeight() const;
	int getUnitDistanceWeight() const;
	int getUnitExperienceWeight() const;
	int getMinTreasury() const;

	int getBuildingRequired(int i) const;
	int getNumBuildingsRequired() const;
	int getUnitRequired(int i) const;
	int getNumUnitsRequired() const;
	int getPrereqOrTechs(int i) const;
	int getNumPrereqOrTechs() const;
	int getPrereqAndTechs(int i) const;
	int getNumPrereqAndTechs() const;
	int getObsoleteTech(int i) const;
	int getNumObsoleteTechs() const;
	int getEvent(int i) const;
	int getNumEvents() const;
	int getPrereqEvent(int i) const;
	int getNumPrereqEvents() const;
	int getFeatureRequired(int i) const;
	int getNumFeaturesRequired() const;
	int getTerrainRequired(int i) const;
	int getNumTerrainsRequired() const;
	int getImprovementRequired(int i) const;
	int getNumImprovementsRequired() const;
	int getBonusRequired(int i) const;
	int getNumBonusesRequired() const;
	int getRouteRequired(int i) const;
	int getNumRoutesRequired() const;
	int getReligionRequired(int i) const;
	int getNumReligionsRequired() const;
	int getCorporationRequired(int i) const;
	int getNumCorporationsRequired() const;

	//Property prereqs
	const CvProperties* getPrereqMinProperties() const;
	const CvProperties* getPrereqMaxProperties() const;
	const CvProperties* getPrereqPlayerMinProperties() const;
	const CvProperties* getPrereqPlayerMaxProperties() const;

	const CvWString& getText(int i) const;
	int getTextEra(int i) const;
	int getNumTexts() const;
	const CvWString& getWorldNews(int i) const;
	int getNumWorldNews() const;

// Start EmperorFool: Events with Images
	const char* getEventArt() const;
// End EmperorFool: Events with Images

	bool isSinglePlayer() const;
	bool isTeam() const;
	bool isRecurring() const;
	bool isGlobal() const;
	bool isPickPlayer() const;
	bool isOtherPlayerWar() const;
	bool isOtherPlayerHasReligion() const;
	bool isOtherPlayerHasOtherReligion() const;
	bool isOtherPlayerAI() const;
	bool isPickCity() const;
	bool isPickOtherPlayerCity() const;
	bool isShowPlot() const;
	bool isUnitsOnPlot() const;
	bool isOwnPlot() const;
	bool isPickReligion() const;
	bool isStateReligion() const;
	bool isHolyCity() const;
	bool isPickCorporation() const;
	bool isHeadquarters() const;
	bool isProbabilityUnitMultiply() const;
	bool isProbabilityBuildingMultiply() const;
	bool isPrereqEventCity() const;

	const char* getPythonCallback() const;
	const char* getPythonCanDo() const;
	const char* getPythonCanDoCity() const;
	const char* getPythonCanDoUnit() const;

	// bool vector without delayed resolution
	int getNotOnGameOption(int i) const;
	int getNumNotOnGameOptions() const;
	bool isNotOnGameOption(int i) const;

	int getOnGameOption(int i) const;
	int getNumOnGameOptions() const;
	bool isOnGameOption(int i) const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvEventTriggerInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

private:
	int m_iPercentGamesActive;
	int m_iProbability;
	int m_iNumUnits;
	int m_iNumBuildings;
	int m_iNumUnitsGlobal;
	int m_iNumBuildingsGlobal;
	int m_iNumPlotsRequired;
	int m_iPlotType;
	int m_iNumReligions;
	int m_iNumCorporations;
	int m_iOtherPlayerShareBorders;
	int m_iOtherPlayerHasTech;
	int m_iCityFoodWeight;
	int m_iCivic;
	int m_iMinPopulation;
	int m_iMaxPopulation;
	int m_iMinMapLandmass;
	int m_iMinOurLandmass;
	int m_iMaxOurLandmass;
	int m_iMinDifficulty;
	int m_iAngry;
	int m_iUnhealthy;
	int m_iUnitDamagedWeight;
	int m_iUnitDistanceWeight;
	int m_iUnitExperienceWeight;
	int m_iMinTreasury;

	CvProperties m_PrereqMinProperties;
	CvProperties m_PrereqMaxProperties;
	CvProperties m_PrereqPlayerMinProperties;
	CvProperties m_PrereqPlayerMaxProperties;

	std::vector<int> m_aiUnitsRequired;
	std::vector<int> m_aiBuildingsRequired;
	std::vector<int> m_aiPrereqOrTechs;
	std::vector<int> m_aiPrereqAndTechs;
	std::vector<int> m_aiObsoleteTechs;
	std::vector<int> m_aiEvents;
	std::vector<int> m_aiPrereqEvents;
	std::vector<int> m_aiFeaturesRequired;
	std::vector<int> m_aiTerrainsRequired;
	std::vector<int> m_aiImprovementsRequired;
	std::vector<int> m_aiBonusesRequired;
	std::vector<int> m_aiRoutesRequired;
	std::vector<int> m_aiReligionsRequired;
	std::vector<int> m_aiCorporationsRequired;

	std::vector<int> m_aiTextEra;
	std::vector<CvWString> m_aszText;
	std::vector<CvWString> m_aszWorldNews;

// Start EmperorFool: Events with Images
	CvString m_szEventArt;
// End EmperorFool: Events with Images
	bool m_bSinglePlayer;
	bool m_bTeam;
	bool m_bRecurring;
	bool m_bGlobal;
	bool m_bPickPlayer;
	bool m_bOtherPlayerWar;
	bool m_bOtherPlayerHasReligion;
	bool m_bOtherPlayerHasOtherReligion;
	bool m_bOtherPlayerAI;
	bool m_bPickCity;
	bool m_bPickOtherPlayerCity;
	bool m_bShowPlot;
	bool m_bUnitsOnPlot;
	bool m_bOwnPlot;
	bool m_bPickReligion;
	bool m_bStateReligion;
	bool m_bHolyCity;
	bool m_bPickCorporation;
	bool m_bHeadquarters;
	bool m_bProbabilityUnitMultiply;
	bool m_bProbabilityBuildingMultiply;
	bool m_bPrereqEventCity;

	CvString m_szPythonCallback;
	CvString m_szPythonCanDo;
	CvString m_szPythonCanDoCity;
	CvString m_szPythonCanDoUnit;

	std::vector<int> m_aiNotOnGameOptions;
	std::vector<int> m_aiOnGameOptions;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEventInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEventInfo : public CvInfoBase
{
	friend class CvXMLLoadUtility;

public:
	CvEventInfo();
	virtual ~CvEventInfo();

	bool isQuest() const;
	bool isGlobal() const;
	bool isTeam() const;
	bool isCityEffect() const;
	bool isOtherPlayerCityEffect() const;
	bool isGoldToPlayer() const;
	bool isGoldenAge() const;
	bool isDeclareWar() const;
	bool isDisbandUnit() const;

	int getGold() const;
	int getRandomGold() const;
	int getEspionagePoints() const;
	int getCulture() const;
	int getTech() const;
	int getTechPercent() const;
	int getTechCostPercent() const;
	int getTechMinTurnsLeft() const;
	TechTypes getPrereqTech() const	{ return m_iPrereqTech; }
	int getFreeUnit() const;
	int getNumUnits() const;
	int getBuilding() const;
	int getBuildingChange() const;
	int getHappy() const;
	int getHealth() const;
	int getHurryAnger() const;
	int getHappyTurns() const;
	int getFood() const;
	int getFoodPercent() const;
	int getFeature() const;
	int getFeatureChange() const;
	ImprovementTypes getImprovement() const	{ return m_iImprovement; }
	int getImprovementChange() const;
	int getCommerceModifier(int i) const;
	int getYieldModifier(int i) const;
	int getBonus() const;
	int getBonusChange() const;
	int getRoute() const;
	int getRouteChange() const;
	int getBonusRevealed() const;
	int getBonusGift() const;
	int getUnitExperience() const;
	int getUnitImmobileTurns() const;
	int getConvertOwnCities() const;
	int getConvertOtherCities() const;
	int getMaxNumReligions() const;
	int getOurAttitudeModifier() const;
	int getAttitudeModifier() const;
	int getTheirEnemyAttitudeModifier() const;
	int getPopulationChange() const;
	int getRevoltTurns() const;
	int getMinPillage() const;
	int getMaxPillage() const;
	int getFreeUnitSupport() const;
	int getInflationModifier() const;
	int getSpaceProductionModifier() const;
	int getAIValue() const;

	int getAdditionalEventChance(int i) const;
	int getAdditionalEventTime(int i) const;
	int getClearEventChance(int i) const;
	int getTechFlavorValue(int i) const;
	int getPlotExtraYield(int i) const;
	int getFreeSpecialistCount(int i) const;
	int getUnitCombatPromotion(int i) const;
	int getUnitPromotion(int i) const;
	const CvWString& getWorldNews(int i) const;
	int getNumWorldNews() const;

	int getBuildingYieldChange(int iBuilding, int iYield) const;
	int getNumBuildingYieldChanges() const;
	int getBuildingCommerceChange(int iBuilding, int iCommerce) const;
	int getNumBuildingCommerceChanges() const;
	int getBuildingYieldModifier(int iBuilding, int iYield) const;
	int getNumBuildingYieldModifiers() const;
	int getBuildingCommerceModifier(int iBuilding, int iCommerce) const;
	int getNumBuildingCommerceModifiers() const;
	int getBuildingHappyChange(int iBuilding) const;
	int getNumBuildingHappyChanges() const;
	int getBuildingHealthChange(int iBuilding) const;
	int getNumBuildingHealthChanges() const;

	const CvProperties* getProperties() const;
	const CvProperties* getPropertiesAllCities() const;

	const char* getPythonCallback() const;
	const char* getPythonExpireCheck() const;
	const char* getPythonCanDo() const;
	const char* getPythonHelp() const;
	const wchar_t* getUnitNameKey() const;
	const wchar_t* getQuestFailTextKey() const;
	const wchar_t* getOtherPlayerPopup() const;
	const wchar_t* getLocalInfoTextKey() const;

	int getPrereqGameOption() const;
	int getRevolutionIndexChange() const;
	int getAdditionalEventChanceVectorSize() const;
	CvString getAdditionalEventChanceNamesVectorElement(int i) const;
	int getAdditionalEventChanceValuesVectorElement(int i) const;

	int getAdditionalEventTimeVectorSize() const;
	CvString getAdditionalEventTimeNamesVectorElement(int i) const;
	int getAdditionalEventTimeValuesVectorElement(int i) const;

	int getClearEventChanceVectorSize() const;
	CvString getClearEventChanceNamesVectorElement(int i) const;
	int getClearEventChanceValuesVectorElement(int i) const;

	bool readPass3();

private:
	std::vector<CvString> m_aszAdditionalEventChanceforPass3;
	std::vector<int> m_aiAdditionalEventChanceforPass3;
	std::vector<CvString> m_aszAdditionalEventTimeforPass3;
	std::vector<int> m_aiAdditionalEventTimeforPass3;
	std::vector<CvString> m_aszClearEventChanceforPass3;
	std::vector<int> m_aiClearEventChanceforPass3;
public:

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvEventInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

private:
	int m_iPrereqGameOption;
	int m_iRevolutionIndexChange;

	bool m_bQuest;
	bool m_bGlobal;
	bool m_bTeam;
	bool m_bCityEffect;
	bool m_bOtherPlayerCityEffect;
	bool m_bGoldToPlayer;
	bool m_bGoldenAge;
	bool m_bDeclareWar;
	bool m_bDisbandUnit;

	int m_iGold;
	int m_iRandomGold;
	int m_iCulture;
	int m_iEspionagePoints;
	int m_iTech;
	int m_iTechPercent;
	int m_iTechCostPercent;
	int m_iTechMinTurnsLeft;
	TechTypes m_iPrereqTech;
	int m_iFreeUnit;
	int m_iNumUnits;
	int m_iBuilding;
	int m_iBuildingChange;
	int m_iHappy;
	int m_iHealth;
	int m_iHurryAnger;
	int m_iHappyTurns;
	int m_iFood;
	int m_iFoodPercent;
	int m_iFeature;
	int m_iFeatureChange;
	ImprovementTypes m_iImprovement;
	int m_iImprovementChange;
	int m_iBonus;
	int m_iBonusChange;
	int m_iRoute;
	int m_iRouteChange;
	int m_iBonusRevealed;
	int m_iBonusGift;
	int m_iUnitExperience;
	int m_iUnitImmobileTurns;
	int m_iConvertOwnCities;
	int m_iConvertOtherCities;
	int m_iMaxNumReligions;
	int m_iOurAttitudeModifier;
	int m_iAttitudeModifier;
	int m_iTheirEnemyAttitudeModifier;
	int m_iPopulationChange;
	int m_iRevoltTurns;
	int m_iMinPillage;
	int m_iMaxPillage;
	int m_iUnitPromotion;
	int m_iFreeUnitSupport;
	int m_iInflationModifier;
	int m_iSpaceProductionModifier;
	int m_iAIValue;

	CvProperties m_Properties;
	CvProperties m_PropertiesAllCities;

	int* m_piTechFlavorValue;
	int* m_piPlotExtraYields;
	int* m_piFreeSpecialistCount;
	int* m_piAdditionalEventChance;
	int* m_piAdditionalEventTime;
	int* m_piClearEventChance;
	int* m_piUnitCombatPromotions;
	int* m_piUnitPromotions;
	int* m_piCommerceModifier;
	int* m_piYieldModifier;

	std::vector<BuildingYieldChange> m_aBuildingYieldChanges;
	std::vector<BuildingCommerceChange> m_aBuildingCommerceChanges;
	std::vector<BuildingCommerceChange> m_aBuildingCommerceModifiers;
	BuildingChangeArray m_aBuildingHappyChanges;
	BuildingChangeArray m_aBuildingHealthChanges;

	CvString m_szPythonCallback;
	CvString m_szPythonExpireCheck;
	CvString m_szPythonCanDo;
	CvString m_szPythonHelp;
	CvWString m_szUnitName;
	CvWString m_szOtherPlayerPopup;
	CvWString m_szQuestFailText;
	CvWString m_szLocalInfoText;
	std::vector<CvWString> m_aszWorldNews;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEspionageMissionInfo
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEspionageMissionInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvEspionageMissionInfo();
	virtual ~CvEspionageMissionInfo();

	int getCost() const;
	bool isPassive() const;
	bool isTwoPhases() const;
	bool isTargetsCity() const;
	bool isSelectPlot() const;

	TechTypes getTechPrereq() const { return m_iTechPrereq; }
	int getVisibilityLevel() const;
	bool isInvestigateCity() const;
	bool isSeeDemographics() const;
	bool isNoActiveMissions() const;
	bool isSeeResearch() const;

	bool isDestroyImprovement() const;
	int getDestroyBuildingCostFactor() const;
	int getDestroyUnitCostFactor() const;
	int getDestroyProjectCostFactor() const;
	int getDestroyProductionCostFactor() const;
	int getBuyUnitCostFactor() const;
	int getBuyCityCostFactor() const;
	int getStealTreasuryTypes() const;
	int getCityInsertCultureAmountFactor() const;
	int getCityInsertCultureCostFactor() const;
	int getCityPoisonWaterCounter() const;
	int getCityUnhappinessCounter() const;
	int getCityRevoltCounter() const;
	int getBuyTechCostFactor() const;
	int getSwitchCivicCostFactor() const;
	int getSwitchReligionCostFactor() const;
	int getPlayerAnarchyCounter() const;
	int getCounterespionageNumTurns() const;
	int getCounterespionageMod() const;
	int getDifficultyMod() const;

	bool isNuke() const;
	bool isRevolt() const;
	bool isDisablePower() const;
	int getPrereqGameOption() const;
	int getWarWearinessCounter() const;
	int getSabatogeResearchCostFactor() const;
	int getRemoveReligionsCostFactor() const;
	int getRemoveCorporationsCostFactor() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvEspionageMissionInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iCost;
	bool m_bIsPassive;
	bool m_bIsTwoPhases;
	bool m_bTargetsCity;
	bool m_bSelectPlot;

	TechTypes m_iTechPrereq;
	int m_iVisibilityLevel;
	bool m_bInvestigateCity;
	bool m_bSeeDemographics;
	bool m_bNoActiveMissions;
	bool m_bSeeResearch;

	bool m_bDestroyImprovement;
	int m_iDestroyBuildingCostFactor;
	int m_iDestroyUnitCostFactor;
	int m_iDestroyProjectCostFactor;
	int m_iDestroyProductionCostFactor;
	int m_iBuyUnitCostFactor;
	int m_iBuyCityCostFactor;
	int m_iStealTreasuryTypes;
	int m_iCityInsertCultureAmountFactor;
	int m_iCityInsertCultureCostFactor;
	int m_iCityPoisonWaterCounter;
	int m_iCityUnhappinessCounter;
	int m_iCityRevoltCounter;
	int m_iBuyTechCostFactor;
	int m_iSwitchCivicCostFactor;
	int m_iSwitchReligionCostFactor;
	int m_iPlayerAnarchyCounter;
	int m_iCounterespionageNumTurns;
	int m_iCounterespionageMod;
	int m_iDifficultyMod;

	bool m_bNuke;
	bool m_bRevolt;
	bool m_bDisablePower;
	int m_iPrereqGameOption;
	int m_iWarWearinessCounter;
	int m_iSabatogeResearchCostFactor;
	int m_iRemoveReligionsCostFactor;
	int m_iRemoveCorporationsCostFactor;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvUnitArtStyleTypeInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitArtStyleTypeInfo : public CvInfoBase
{
public:

	CvUnitArtStyleTypeInfo();
	virtual ~CvUnitArtStyleTypeInfo();

	const char* getEarlyArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setEarlyArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal);
	const char* getLateArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setLateArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal);
	const char* getMiddleArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setMiddleArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal);

	const char* getClassicalArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setClassicalArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal);
	const char* getRennArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setRennArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal);
	const char* getIndustrialArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setIndustrialArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal);
	const char* getFutureArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setFutureArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvUnitArtStyleTypeInfo* pClassInfo);

protected:

	struct ArtDefneTag
	{
		int iMeshIndex;
		int iUnitType;
		CvString szTag;
	};
	typedef std::vector<ArtDefneTag> ArtDefineArray;

	ArtDefineArray m_azEarlyArtDefineTags;
	ArtDefineArray m_azClassicalArtDefineTags;
	ArtDefineArray m_azMiddleArtDefineTags;
	ArtDefineArray m_azRennArtDefineTags;
	ArtDefineArray m_azIndustrialArtDefineTags;
	ArtDefineArray m_azLateArtDefineTags;
	ArtDefineArray m_azFutureArtDefineTags;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvVoteSourceInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvVoteSourceInfo : public CvInfoBase
{
public:

	CvVoteSourceInfo();
	virtual ~CvVoteSourceInfo();

	int getVoteInterval() const;
	int getFreeSpecialist() const;
	int getCivic() const;
	const CvWString getPopupText() const;
	const CvWString getSecretaryGeneralText() const;

	std::wstring pyGetSecretaryGeneralText() const { return getSecretaryGeneralText(); }

	int getReligionYield(int i) const;
	int getReligionCommerce(int i) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvVoteSourceInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

protected:
	int m_iVoteInterval;
	int m_iFreeSpecialist;
	int m_iCivic;

	int* m_aiReligionYields;
	int* m_aiReligionCommerces;

	CvString m_szPopupText;
	CvString m_szSecretaryGeneralText;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvMainMenuInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMainMenuInfo : public CvInfoBase
{
public:

	CvMainMenuInfo();
	virtual ~CvMainMenuInfo();

	DllExport std::string getScene() const;
	DllExport std::string getSceneNoShader() const;
	DllExport std::string getSoundtrack() const;
	DllExport std::string getLoading() const;
	DllExport std::string getLoadingSlideshow() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvMainMenuInfo* pClassInfo);

protected:
	std::string m_szScene;
	std::string m_szSceneNoShader;
	std::string m_szSoundtrack;
	std::string m_szLoading;
	std::string m_szLoadingSlideshow;
};
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL				 10/24/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
// MLF loading
class CvModLoadControlInfo : public CvInfoBase
{
public:

	CvModLoadControlInfo();
	virtual ~CvModLoadControlInfo();

	bool isLoad(int i) const;
	void setLoad(int i, bool bLoad = true);
	const std::string getModuleFolder(int i) const;
	const std::string getParentFolder() const;
	int getNumModules() const;
	int getDirDepth() const;
	bool read(CvXMLLoadUtility* pXML, CvString szDirDepth, int iDirDepth);

protected:
	bool* m_bLoad;
	int m_iNumModules;
	int m_iDirDepth;
	std::string* m_paszModuleFolder;
	std::string m_paszParentFolder;
};
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL				 END												  */
/************************************************************************************************/

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPropertyInfo
//
//  DESC:   Contains info about generic properties which can be added to buildings
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPropertyInfo :
	public CvInfoBase
{
public:

	CvPropertyInfo();
	virtual ~CvPropertyInfo();

	int getAIWeight() const;
	AIScaleTypes getAIScaleType() const;
	int getOperationalRangeMin() const;
	int getOperationalRangeMax() const;
	int getTargetLevel() const;
	int getTrainReluctance() const;
	bool isSourceDrain() const;
	bool isOAType() const;
	CvWString getValueDisplayText() const;
	CvWString getChangeDisplayText() const;
	CvWString getChangeAllCitiesDisplayText() const;
	CvWString getPrereqMinDisplayText() const;
	CvWString getPrereqMaxDisplayText() const;

	const PropertyBuilding& getPropertyBuilding(int index) const;
	int getNumPropertyBuildings() const;
	const std::vector<PropertyBuilding>& getPropertyBuildings() const { return m_aPropertyBuildings; }

	const PropertyPromotion& getPropertyPromotion(int index) const;
	int getNumPropertyPromotions() const;
	const std::vector<PropertyPromotion>& getPropertyPromotions() const { return m_aPropertyPromotions; }

	int getChangePropagator(const GameObjectTypes eFrom, const GameObjectTypes eTo) const;

	int getNumTargetLevelbyEraTypes() const;
	int getTargetLevelbyEraType(int iIndex) const;
	bool isTargetLevelbyEraType(int iIndex) const;

	int getChar() const;
	void setChar(int i);
	int getFontButtonIndex() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvPropertyInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

private:
	CvPropertyManipulators m_PropertyManipulators;

protected:
	int m_iChar;
	int m_iFontButtonIndex;

	int m_iAIWeight;
	AIScaleTypes m_eAIScaleType;
	int m_iOperationalRangeMin;
	int m_iOperationalRangeMax;
	int m_iTargetLevel;
	int m_iTrainReluctance;
	bool m_bSourceDrain;
	bool m_bOAType;
	CvWString m_szValueDisplayText;
	CvWString m_szChangeDisplayText;
	CvWString m_szChangeAllCitiesDisplayText;
	CvWString m_szPrereqMinDisplayText;
	CvWString m_szPrereqMaxDisplayText;

	int m_aaiChangePropagator[NUM_GAMEOBJECTS][NUM_GAMEOBJECTS]; // from/to, in percent

	std::vector<PropertyBuilding> m_aPropertyBuildings;
	std::vector<PropertyPromotion> m_aPropertyPromotions;
	EraArray m_aTargetLevelbyEraTypes;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvOutcomeInfo
//
//  DESC:   Contains info about outcome types which can be the result of a kill or of actions
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvOutcomeInfo :
	public CvInfoBase
{
public:

	CvOutcomeInfo();
	virtual ~CvOutcomeInfo();

	CvWString getMessageText() const;
	TechTypes getPrereqTech() const	{ return m_ePrereqTech; }
	TechTypes getObsoleteTech() const;
	CivicTypes getPrereqCivic() const;
	bool getToCoastalCity() const;
	bool getFriendlyTerritory() const;
	bool getNeutralTerritory() const;
	bool getHostileTerritory() const;
	bool getBarbarianTerritory() const;
	bool getCity() const;
	bool getNotCity() const;
	bool isCapture() const;
	const std::vector<BuildingTypes>& getPrereqBuildings() const { return m_aePrereqBuildings; }
	int getNumExtraChancePromotions() const;
	PromotionTypes getExtraChancePromotion(int i) const;
	int getExtraChancePromotionChance(int i) const;
	const std::vector<OutcomeTypes>& getReplaceOutcomes() const { return m_aeReplaceOutcomes; }

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvOutcomeInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

protected:

	TechTypes m_ePrereqTech;
	TechTypes m_eObsoleteTech;
	CivicTypes m_ePrereqCivic;
	bool m_bToCoastalCity;
	bool m_bFriendlyTerritory;
	bool m_bNeutralTerritory;
	bool m_bHostileTerritory;
	bool m_bBarbarianTerritory;
	bool m_bCity;
	bool m_bNotCity;
	bool m_bCapture;
	std::vector<BuildingTypes> m_aePrereqBuildings;
	std::vector<std::pair<PromotionTypes, int> > m_aeiExtraChancePromotions;
	std::vector<OutcomeTypes> m_aeReplaceOutcomes;
	CvWString m_szMessageText;
};

//TB Promotion Line Mod begin
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPromotionLineInfo
//
//  DESC:   Contains info about Promotion Lines, definitions of promotion upgrade chains
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPromotionLineInfo :
	public CvInfoBase
{
public:

	CvPromotionLineInfo();
	virtual ~CvPromotionLineInfo();

	TechTypes getPrereqTech() const	{ return m_ePrereqTech; }
	TechTypes getObsoleteTech() const;
	PropertyTypes getPropertyType() const;
#ifdef OUTBREAKS_AND_AFFLICTIONS
	int getToleranceBuildup() const;
	int getToleranceDecay() const;
	int getCommunicability() const;
	int getWorseningProbabilityIncrementModifier() const;
	int getWorsenedCommunicabilityIncrementModifier() const;
	int getWorsenedOvercomeIncrementModifier() const;
	int getOvercomeProbability() const;
	int getOvercomeAdjperTurn() const;
	int getOutbreakModifier() const;
	int getOvercomeModifier() const;
	bool isAffliction() const { return m_bAffliction; }
#endif // OUTBREAKS_AND_AFFLICTIONS
	bool isEquipment() const;
	bool isCritical() const;
	bool isNoSpreadonBattle() const;
	bool isNoSpreadUnitProximity() const;
	bool isNoSpreadUnittoCity() const;
	bool isNoSpreadCitytoUnit() const;
	bool isBuildUp() const;
	bool isPoison() const;

	// bool vectors without delayed resolution
	int getUnitCombatPrereqType(int i) const;
	int getNumUnitCombatPrereqTypes() const;

	int getNotOnUnitCombatType(int i) const;
	int getNumNotOnUnitCombatTypes() const;
	bool isNotOnUnitCombatType(int i) const;

	int getNotOnDomainType(int i) const;
	int getNumNotOnDomainTypes() const;
	bool isNotOnDomainType(int i) const;

	int getOnGameOption(int i) const;
	int getNumOnGameOptions() const;
	bool isOnGameOption(int i) const;

	int getNotOnGameOption(int i) const;
	int getNumNotOnGameOptions() const;
	bool isNotOnGameOption(int i) const;

	int getCriticalOriginCombatClassType(int i) const;
	int getNumCriticalOriginCombatClassTypes() const;
	bool isCriticalOriginCombatClassType(int i) const;

	// int vector utilizing pairing without delayed resolution
	int getNumUnitCombatContractChanceChanges() const;
	int getUnitCombatContractChanceChange(int iUnitCombat) const;
	bool isUnitCombatContractChanceChange(int iUnitCombat) const;

	int getNumUnitCombatOvercomeChanges() const;
	int getUnitCombatOvercomeChange(int iUnitCombat) const;
	bool isUnitCombatOvercomeChange(int iUnitCombat) const;

	int getNumTechContractChanceChanges() const;
	int getTechContractChanceChange(int iTech) const;
	bool isTechContractChanceChange(int iTech) const;

	int getNumTechOvercomeChanges() const;
	int getTechOvercomeChange(int iTech) const;
	bool isTechOvercomeChange(int iTech) const;

	//Post Load

	int getPromotion(int i) const;
	int getNumPromotions() const;
	bool isPromotion(int i) const;
	void setPromotions();

	int getBuilding(int i) const;
	int getNumBuildings() const;
	bool isBuilding(int i) const;
	void setBuildings();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvPromotionLineInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

protected:
	TechTypes m_ePrereqTech;
	TechTypes m_eObsoleteTech;
	PropertyTypes m_ePropertyType;
#ifdef OUTBREAKS_AND_AFFLICTIONS
	int m_iToleranceBuildup;
	int m_iToleranceDecay;
	int m_iCommunicability;
	int m_iWorseningProbabilityIncrementModifier;
	int m_iWorsenedCommunicabilityIncrementModifier;
	int m_iWorsenedOvercomeIncrementModifier;
	int m_iOvercomeProbability;
	int m_iOvercomeAdjperTurn;
	int m_iOutbreakModifier;
	int m_iOvercomeModifier;
	bool m_bAffliction;
#endif // OUTBREAKS_AND_AFFLICTIONS
	bool m_bEquipment;
	bool m_bCritical;
	bool m_bNoSpreadonBattle;
	bool m_bNoSpreadUnitProximity;
	bool m_bNoSpreadUnittoCity;
	bool m_bNoSpreadCitytoUnit;
	bool m_bBuildUp;
	bool m_bPoison;


	// bool vectors without delayed resolution
	std::vector<int> m_aiUnitCombatPrereqTypes;
	std::vector<int> m_aiNotOnUnitCombatTypes;
	std::vector<int> m_aiNotOnDomainTypes;
	std::vector<int> m_aiOnGameOptions;
	std::vector<int> m_aiNotOnGameOptions;
	std::vector<int> m_aiCriticalOriginCombatClassTypes;
	std::vector<int> m_aiPromotions;
	std::vector<int> m_aiBuildings;

	// int vector utilizing pairing without delayed resolution
	UnitCombatModifierArray m_aUnitCombatContractChanceChanges;
	UnitCombatModifierArray m_aUnitCombatOvercomeChanges;
	TechModifierArray m_aTechContractChanceChanges;
	TechModifierArray m_aTechOvercomeChanges;
};
//TB Promotion Line Mod end

// AIAndy: A proper info class for unit combat types
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvUnitCombatInfo
//
//  DESC:   Contains unit combat types
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitCombatInfo :
	public CvInfoBase
{
public:

	CvUnitCombatInfo();
	virtual ~CvUnitCombatInfo();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvUnitCombatInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	const CvOutcomeList* getKillOutcomeList() const;
	int getNumActionOutcomes() const;
	const CvOutcomeList* getActionOutcomeList(int index) const;
	MissionTypes getActionOutcomeMission(int index) const;
	const CvOutcomeList* getActionOutcomeListByMission(MissionTypes eMission) const;
	const CvOutcomeMission* getOutcomeMission(int index) const;
	CvOutcomeMission* getOutcomeMissionByMission(MissionTypes eMission) const;

protected:
	CvOutcomeList m_KillOutcomeList;
	std::vector<CvOutcomeMission*> m_aOutcomeMissions;

public:
	// Textual References
	ReligionTypes getReligion() const;
	BonusTypes getCulture() const;
	EraTypes getEra() const;
	// integers
	int getVisibilityChange() const;
	int getMovesChange() const;
	int getMoveDiscountChange() const;
	int getAirRangeChange() const;
	int getInterceptChange() const;
	int getEvasionChange() const;
	int getWithdrawalChange() const;
	int getCargoChange() const;
	int getSMCargoChange() const;
	int getSMCargoVolumeChange() const;
	int getSMCargoVolumeModifierChange() const;
	int getCollateralDamageChange() const;
	int getBombardRateChange() const;
	int getFirstStrikesChange() const;
	int getChanceFirstStrikesChange() const;
	int getEnemyHealChange() const;
	int getNeutralHealChange() const;
	int getFriendlyHealChange() const;
	int getSameTileHealChange() const;
	int getAdjacentTileHealChange() const;
	int getCombatPercent() const;
	int getCityAttackPercent() const;
	int getCityDefensePercent() const;
	int getHillsAttackPercent() const;
	int getHillsDefensePercent() const;
	int getHillsWorkPercent() const;
	int getWorkRatePercent() const;
	int getRevoltProtection() const;
	int getCollateralDamageProtection() const;
	int getPillageChange() const;
	int getUpgradeDiscount() const;
	int getExperiencePercent() const;
	int getKamikazePercent() const;
	int getAirCombatLimitChange() const;
	int getCelebrityHappy() const;
	int getCollateralDamageLimitChange() const;
	int getCollateralDamageMaxUnitsChange() const;
	int getCombatLimitChange() const;
	int getExtraDropRange() const;
	int getSurvivorChance() const;
	int getVictoryAdjacentHeal() const;
	int getVictoryHeal() const;
	int getVictoryStackHeal() const;
	int getAttackCombatModifierChange() const;
	int getDefenseCombatModifierChange() const;
	int getPursuitChange() const;
	int getEarlyWithdrawChange() const;
	int getVSBarbsChange() const;
	int getArmorChange() const;
	int getPunctureChange() const;
	int getOverrunChange() const;
	int getRepelChange() const;
	int getFortRepelChange() const;
	int getRepelRetriesChange() const;
	int getUnyieldingChange() const;
	int getKnockbackChange() const;
	int getKnockbackRetriesChange() const;
#ifdef BATTLEWORN
	int getStrAdjperAttChange() const;
	int getStrAdjperDefChange() const;
	int getWithdrawAdjperAttChange() const;
#endif // BATTLEWORN
	int getUnnerveChange() const;
	int getEncloseChange() const;
	int getLungeChange() const;
	int getDynamicDefenseChange() const;
	int getStrengthChange() const;
	int getFortitudeChange() const;
#ifdef STRENGTH_IN_NUMBERS
	int getFrontSupportPercentChange() const;
	int getShortRangeSupportPercentChange() const;
	int getMediumRangeSupportPercentChange() const;
	int getLongRangeSupportPercentChange() const;
	int getFlankSupportPercentChange() const;
#endif // STRENGTH_IN_NUMBERS
	int getDodgeModifierChange() const;
	int getPrecisionModifierChange() const;
	int getPowerShotsChange() const;
	int getPowerShotCombatModifierChange() const;
	int getPowerShotPunctureModifierChange() const;
	int getPowerShotPrecisionModifierChange() const;
	int getPowerShotCriticalModifierChange() const;
	int getCriticalModifierChange() const;
	int getEnduranceChange() const;
	int getRoundStunProbChange() const;
	int getPoisonProbabilityModifierChange() const;
	int getCaptureProbabilityModifierChange() const;
	int getCaptureResistanceModifierChange() const;
	int getPeaksWorkPercent() const;
	int getBreakdownChanceChange() const;
	int getBreakdownDamageChange() const;
	int getTauntChange() const;
	int getMaxHPChange() const;
	int getStrengthModifier() const;
	int getQualityBase() const;
	int getGroupBase() const;
	int getSizeBase() const;
	int getDamageModifierChange() const;
	int getUpkeepModifier() const;
	int getExtraUpkeep100() const;
	int getRBombardDamageBase() const;
	int getRBombardDamageLimitBase() const;
	int getRBombardDamageMaxUnitsBase() const;
	int getDCMBombRangeBase() const;
	int getDCMBombAccuracyBase() const;
	int getCombatModifierPerSizeMoreChange() const;
	int getCombatModifierPerSizeLessChange() const;
	int getCombatModifierPerVolumeMoreChange() const;
	int getCombatModifierPerVolumeLessChange() const;
	int getSelfHealModifier() const;
	int getNumHealSupport() const;
	int getExcileChange() const;
	int getPassageChange() const;
	int getNoNonOwnedCityEntryChange() const;
	int getBarbCoExistChange() const;
	int getBlendIntoCityChange() const;
	int getInsidiousnessChange() const;
	int getInvestigationChange() const;
	int getStealthStrikesChange() const;
	int getStealthCombatModifierChange() const;
	int getStealthDefenseChange() const;
	int getDefenseOnlyChange() const;
	int getNoInvisibilityChange() const;
	int getNoCaptureChange() const;
	int getAnimalIgnoresBordersChange() const;
	int getNoDefensiveBonusChange() const;
	int getGatherHerdChange() const;
	int getReligiousCombatModifierChange() const;

	//booleans
	bool isDefensiveVictoryMove() const;
	bool isFreeDrop() const;
	bool isOffensiveVictoryMove() const;
	bool isOneUp() const;
	bool isPillageEspionage() const;
	bool isPillageMarauder() const;
	bool isPillageOnMove() const;
	bool isPillageOnVictory() const;
	bool isPillageResearch() const;
	bool isBlitz() const;
	bool isAmphib() const;
	bool isRiver() const;
	bool isEnemyRoute() const;
	bool isAlwaysHeal() const;
	bool isHillsDoubleMove() const;
	bool isImmuneToFirstStrikes() const;
	bool isStampedeChange() const;
	bool isRemoveStampede() const;
	bool isOnslaughtChange() const;
	bool isMakesDamageCold() const;
	bool isMakesDamageNotCold() const;
	bool isAddsColdImmunity() const;
	bool isRemovesColdImmunity() const;
	bool isAttackOnlyCitiesAdd() const;
	bool isAttackOnlyCitiesSubtract() const;
	bool isIgnoreNoEntryLevelAdd() const;
	bool isIgnoreNoEntryLevelSubtract() const;
	bool isIgnoreZoneofControlAdd() const;
	bool isIgnoreZoneofControlSubtract() const;
	bool changesMoveThroughPlots() const;
	bool isCanMovePeaks() const;
	bool isCanLeadThroughPeaks() const;
	bool isZoneOfControl() const;
	bool isSpy() const;
	bool isFliesToMoveAdd() const;
	bool isFliesToMoveSubtract() const;
	bool isCannotMergeSplit() const;
	bool isRBombardDirect() const;
	bool isRBombardForceAbility() const;
	bool isAlwaysInvisible() const;
	bool isForMilitary() const;
	bool isForNavalMilitary() const;
	bool isHealsAs() const;
	bool isNoSelfHeal() const;
	// Inline Int
	inline int getZobristValue() const { return m_zobristValue; }

	// Arrays
	int getDomainModifierPercent(int i) const;
	bool isAnyDomainModifierPercent() const;

	int getTerrainIgnoreDamageChangeType(int i) const;
	int getNumTerrainIgnoreDamageChangeTypes() const;
	bool isTerrainIgnoreDamageChangeType(int i) const;

	int getTerrainDoubleMoveChangeType(int i) const;
	int getNumTerrainDoubleMoveChangeTypes() const;
	bool isTerrainDoubleMoveChangeType(int i) const;

	int getFeatureDoubleMoveChangeType(int i) const;
	int getNumFeatureDoubleMoveChangeTypes() const;
	bool isFeatureDoubleMoveChangeType(int i) const;

	int getOnGameOption(int i) const;
	int getNumOnGameOptions() const;
	bool isOnGameOption(int i) const;

	int getNotOnGameOption(int i) const;
	int getNumNotOnGameOptions() const;
	bool isNotOnGameOption(int i) const;

	int getGGptsforUnitType(int i) const;
	int getNumGGptsforUnitTypes() const;
	bool isGGptsforUnitType(int i) const;

	int getDefaultStatusType(int i) const;
	int getNumDefaultStatusTypes() const;
	bool isDefaultStatusType(int i) const;

	int getTrapImmunityUnitCombatType(int i) const;
	int getNumTrapImmunityUnitCombatTypes() const;
	bool isTrapImmunityUnitCombatType(int i) const;

	// int vector utilizing pairing without delayed resolution
	int getNumWithdrawOnTerrainTypeChanges() const;
	int getWithdrawOnTerrainTypeChange(int iTerrain) const;

	int getNumWithdrawOnFeatureTypeChanges() const;
	int getWithdrawOnFeatureTypeChange(int iFeature) const;

	int getNumVisibilityIntensityChangeTypes() const;
	int getVisibilityIntensityChangeType(int iInvisibility) const;
	bool isVisibilityIntensityChangeType(int iInvisibility) const;

	int getNumInvisibilityIntensityChangeTypes() const;
	int getInvisibilityIntensityChangeType(int iInvisibility) const;
	bool isInvisibilityIntensityChangeType(int iInvisibility) const;

	int getNumVisibilityIntensityRangeChangeTypes() const;
	int getVisibilityIntensityRangeChangeType(int iInvisibility) const;
	bool isVisibilityIntensityRangeChangeType(int iInvisibility) const;

	int getNumVisibilityIntensitySameTileChangeTypes() const;
	int getVisibilityIntensitySameTileChangeType(int iInvisibility) const;
	bool isVisibilityIntensitySameTileChangeType(int iInvisibility) const;

#ifdef OUTBREAKS_AND_AFFLICTIONS
	int getNumAidChanges() const;
	int getAidChange(int iProperty) const;
	bool isAidChange(int iProperty) const;

	int getCureAfflictionChangeType(int i) const;
	int getNumCureAfflictionChangeTypes() const;
	bool isCureAfflictionChangeType(int i) const;

	int getNumAfflictionFortitudeChangeModifiers() const;
	const PromotionLineModifier& getAfflictionFortitudeChangeModifier(int iAfflictionLine) const;

	int getNumAfflictOnAttackChangeTypes() const;
	const AfflictOnAttackChange& getAfflictOnAttackChangeType(int iAfflictionLine) const;
#endif // OUTBREAKS_AND_AFFLICTIONS

	int getNumTerrainAttackChangeModifiers() const;
	const TerrainModifier& getTerrainAttackChangeModifier(int iTerrain) const;

	int getNumTerrainDefenseChangeModifiers() const;
	const TerrainModifier& getTerrainDefenseChangeModifier(int iTerrain) const;

	int getNumTerrainWorkChangeModifiers() const;
	const TerrainModifier& getTerrainWorkChangeModifier(int iTerrain) const;

	int getNumBuildWorkChangeModifiers() const;
	const BuildModifier& getBuildWorkChangeModifier(int iBuild) const;

	int getNumFeatureAttackChangeModifiers() const;
	const FeatureModifier& getFeatureAttackChangeModifier(int iFeature) const;

	int getNumFeatureDefenseChangeModifiers() const;
	const FeatureModifier& getFeatureDefenseChangeModifier(int iFeature) const;

	int getNumFeatureWorkChangeModifiers() const;
	const FeatureModifier& getFeatureWorkChangeModifier(int iFeature) const;

	int getNumUnitCombatChangeModifiers() const;
	const UnitCombatModifier& getUnitCombatChangeModifier(int iUnitCombat) const;

	int getNumFlankingStrengthbyUnitCombatTypesChange() const;
	const UnitCombatModifier& getFlankingStrengthbyUnitCombatTypeChange(int iUnitCombat) const;

	int getNumWithdrawVSUnitCombatTypesChange() const;
	const UnitCombatModifier& getWithdrawVSUnitCombatTypeChange(int iUnitCombat) const;

	//Fight or Flight only
	int getNumPursuitVSUnitCombatTypesChange() const;
	const UnitCombatModifier& getPursuitVSUnitCombatTypeChange(int iUnitCombat) const;

	//Heart of War only
	int getNumRepelVSUnitCombatTypesChange() const;
	const UnitCombatModifier& getRepelVSUnitCombatTypeChange(int iUnitCombat) const;

	//Heart of War only
	int getNumKnockbackVSUnitCombatTypesChange() const;
	const UnitCombatModifier& getKnockbackVSUnitCombatTypeChange(int iUnitCombat) const;

	int getNumPunctureVSUnitCombatTypesChange() const;
	const UnitCombatModifier& getPunctureVSUnitCombatTypeChange(int iUnitCombat) const;

	int getNumArmorVSUnitCombatTypesChange() const;
	const UnitCombatModifier& getArmorVSUnitCombatTypeChange(int iUnitCombat) const;

	int getNumDodgeVSUnitCombatTypesChange() const;
	const UnitCombatModifier& getDodgeVSUnitCombatTypeChange(int iUnitCombat) const;

	int getNumPrecisionVSUnitCombatTypesChange() const;
	const UnitCombatModifier& getPrecisionVSUnitCombatTypeChange(int iUnitCombat) const;

	int getNumCriticalVSUnitCombatTypesChange() const;
	const UnitCombatModifier& getCriticalVSUnitCombatTypeChange(int iUnitCombat) const;

	int getNumRoundStunVSUnitCombatTypesChange() const;
	const UnitCombatModifier& getRoundStunVSUnitCombatTypeChange(int iUnitCombat) const;

	int getNumTrapAvoidanceUnitCombatTypes() const;
	const UnitCombatModifier& getTrapAvoidanceUnitCombatType(int iIndex) const;

	int getNumInvisibleTerrainChanges() const;
	const InvisibleTerrainChanges& getInvisibleTerrainChange(int iIndex) const;

	int getNumInvisibleFeatureChanges() const;
	const InvisibleFeatureChanges& getInvisibleFeatureChange(int iIndex) const;

	int getNumInvisibleImprovementChanges() const;
	const InvisibleImprovementChanges& getInvisibleImprovementChange(int iIndex) const;

	int getNumVisibleTerrainChanges() const;
	const InvisibleTerrainChanges& getVisibleTerrainChange(int iIndex) const;

	int getNumVisibleFeatureChanges() const;
	const InvisibleFeatureChanges& getVisibleFeatureChange(int iIndex) const;

	int getNumVisibleImprovementChanges() const;
	const InvisibleImprovementChanges& getVisibleImprovementChange(int iIndex) const;

	int getNumVisibleTerrainRangeChanges() const;
	const InvisibleTerrainChanges& getVisibleTerrainRangeChange(int iIndex) const;

	int getNumVisibleFeatureRangeChanges() const;
	const InvisibleFeatureChanges& getVisibleFeatureRangeChange(int iIndex) const;

	int getNumVisibleImprovementRangeChanges() const;
	const InvisibleImprovementChanges& getVisibleImprovementRangeChange(int iIndex) const;

	int getNumDistanceAttackCommunicabilityTypeChanges() const;
	const AfflictionLineChanges& getDistanceAttackCommunicabilityTypeChange(int iIndex) const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

private:
	CvPropertyManipulators m_PropertyManipulators;

protected:
	// Textual References
	ReligionTypes m_eReligion;
	BonusTypes m_eCulture;
	EraTypes m_eEra;
	//Integers
	int m_iIgnoreTerrainDamage;
	int m_zobristValue;
	int m_iVisibilityChange;
	int m_iMovesChange;
	int m_iMoveDiscountChange;
	int m_iAirRangeChange;
	int m_iInterceptChange;
	int m_iEvasionChange;
	int m_iWithdrawalChange;
	int m_iCargoChange;
	int m_iSMCargoChange;
	int m_iSMCargoVolumeChange;
	int m_iSMCargoVolumeModifierChange;
	int m_iCollateralDamageChange;
	int m_iBombardRateChange;
	int m_iFirstStrikesChange;
	int m_iChanceFirstStrikesChange;
	int m_iEnemyHealChange;
	int m_iNeutralHealChange;
	int m_iFriendlyHealChange;
	int m_iSameTileHealChange;
	int m_iAdjacentTileHealChange;
	int m_iCombatPercent;
	int m_iCityAttackPercent;
	int m_iCityDefensePercent;
	int m_iHillsAttackPercent;
	int m_iHillsDefensePercent;
	int m_iHillsWorkPercent;
	int m_iWorkRatePercent;
	int m_iRevoltProtection;
	int m_iCollateralDamageProtection;
	int m_iPillageChange;
	int m_iUpgradeDiscount;
	int m_iExperiencePercent;
	int m_iKamikazePercent;
	int m_iAirCombatLimitChange;
	int m_iCelebrityHappy;
	int m_iCollateralDamageLimitChange;
	int m_iCollateralDamageMaxUnitsChange;
	int m_iCombatLimitChange;
	int m_iExtraDropRange;
	int m_iSurvivorChance;
	int m_iVictoryAdjacentHeal;
	int m_iVictoryHeal;
	int m_iVictoryStackHeal;
	int m_iAttackCombatModifierChange;
	int m_iDefenseCombatModifierChange;
	int m_iPursuitChange;
	int m_iEarlyWithdrawChange;
	int m_iVSBarbsChange;
	int m_iArmorChange;
	int m_iPunctureChange;
	int m_iOverrunChange;
	int m_iRepelChange;
	int m_iFortRepelChange;
	int m_iRepelRetriesChange;
	int m_iUnyieldingChange;
	int m_iKnockbackChange;
	int m_iKnockbackRetriesChange;
#ifdef BATTLEWORN
	int m_iStrAdjperAttChange;
	int m_iStrAdjperDefChange;
	int m_iWithdrawAdjperAttChange;
#endif // BATTLEWORN
	int m_iUnnerveChange;
	int m_iEncloseChange;
	int m_iLungeChange;
	int m_iDynamicDefenseChange;
	int m_iStrengthChange;
	int m_iFortitudeChange;
#ifdef STRENGTH_IN_NUMBERS
	int m_iFrontSupportPercentChange;
	int m_iShortRangeSupportPercentChange;
	int m_iMediumRangeSupportPercentChange;
	int m_iLongRangeSupportPercentChange;
	int m_iFlankSupportPercentChange;
#endif // STRENGTH_IN_NUMBERS
	int m_iDodgeModifierChange;
	int m_iPrecisionModifierChange;
	int m_iPowerShotsChange;
	int m_iPowerShotCombatModifierChange;
	int m_iPowerShotPunctureModifierChange;
	int m_iPowerShotPrecisionModifierChange;
	int m_iPowerShotCriticalModifierChange;
	int m_iCriticalModifierChange;
	int m_iEnduranceChange;
	int m_iRoundStunProbChange;
	int m_iPoisonProbabilityModifierChange;
	int m_iCaptureProbabilityModifierChange;
	int m_iCaptureResistanceModifierChange;
	int m_iPeaksWorkPercent;
	int m_iBreakdownChanceChange;
	int m_iBreakdownDamageChange;
	int m_iTauntChange;
	int m_iMaxHPChange;
	int m_iStrengthModifier;
	int m_iQualityBase;
	int m_iGroupBase;
	int m_iSizeBase;
	int m_iDamageModifierChange;

	int m_iUpkeepModifier;
	int m_iExtraUpkeep100;

	int m_iRBombardDamageBase;
	int m_iRBombardDamageLimitBase;
	int m_iRBombardDamageMaxUnitsBase;
	int m_iDCMBombRangeBase;
	int m_iDCMBombAccuracyBase;
	int m_iCombatModifierPerSizeMoreChange;
	int m_iCombatModifierPerSizeLessChange;
	int m_iCombatModifierPerVolumeMoreChange;
	int m_iCombatModifierPerVolumeLessChange;
	int m_iSelfHealModifier;
	int m_iNumHealSupport;
	int m_iExcileChange;
	int m_iPassageChange;
	int m_iNoNonOwnedCityEntryChange;
	int m_iBarbCoExistChange;
	int m_iBlendIntoCityChange;
	int m_iInsidiousnessChange;
	int m_iInvestigationChange;
	int m_iStealthStrikesChange;
	int m_iStealthCombatModifierChange;
	int m_iStealthDefenseChange;
	int m_iDefenseOnlyChange;
	int m_iNoInvisibilityChange;
	int m_iNoCaptureChange;
	int m_iAnimalIgnoresBordersChange;
	int m_iNoDefensiveBonusChange;
	int m_iGatherHerdChange;
	int m_iReligiousCombatModifierChange;

	//Booleans
	bool m_bCanMovePeaks;
	bool m_bCanLeadThroughPeaks;
	bool m_bZoneOfControl;
	bool m_bDefensiveVictoryMove;
	bool m_bFreeDrop;
	bool m_bOffensiveVictoryMove;
	bool m_bOneUp;
	bool m_bPillageEspionage;
	bool m_bPillageMarauder;
	bool m_bPillageOnMove;
	bool m_bPillageOnVictory;
	bool m_bPillageResearch;
	bool m_bBlitz;
	bool m_bAmphib;
	bool m_bRiver;
	bool m_bEnemyRoute;
	bool m_bAlwaysHeal;
	bool m_bHillsDoubleMove;
	bool m_bImmuneToFirstStrikes;
	bool m_bStampedeChange;
	bool m_bRemoveStampede;
	bool m_bOnslaughtChange;
	bool m_bMakesDamageCold;
	bool m_bMakesDamageNotCold;
	bool m_bAddsColdImmunity;
	bool m_bRemovesColdImmunity;
	bool m_bAttackOnlyCitiesAdd;
	bool m_bAttackOnlyCitiesSubtract;
	bool m_bIgnoreNoEntryLevelAdd;
	bool m_bIgnoreNoEntryLevelSubtract;
	bool m_bIgnoreZoneofControlAdd;
	bool m_bIgnoreZoneofControlSubtract;
	bool m_bSpy;
	bool m_bFliesToMoveAdd;
	bool m_bFliesToMoveSubtract;
	bool m_bCannotMergeSplit;
	bool m_bRBombardDirect;
	bool m_bRBombardForceAbility;
	bool m_bInvisible;
	bool m_bForMilitary;
	bool m_bForNavalMilitary;
	bool m_bHealsAs;
	bool m_bNoSelfHeal;

	//Arrays
	int* m_piDomainModifierPercent;

	// bool vectors without delayed resolution
	// bool vector with delayed resolution
	std::vector<int> m_aiTerrainIgnoreDamageChangeTypes;
	std::vector<int> m_aiTerrainDoubleMoveChangeTypes;
	std::vector<int> m_aiFeatureDoubleMoveChangeTypes;
	std::vector<int> m_aiOnGameOptions;
	std::vector<int> m_aiNotOnGameOptions;
	std::vector<int> m_aiGGptsforUnitTypes;
	std::vector<int> m_aiDefaultStatusTypes;
	std::vector<int> m_aiTrapImmunityUnitCombatTypes;
	// int vectors utilizing pairing without delayed resolution
	TerrainModifierArray m_aWithdrawOnTerrainTypesChange;
	FeatureModifierArray m_aWithdrawOnFeatureTypesChange;
	InvisibilityArray m_aVisibilityIntensityChangeTypes;
	InvisibilityArray m_aInvisibilityIntensityChangeTypes;
	InvisibilityArray m_aVisibilityIntensityRangeChangeTypes;
	InvisibilityArray m_aVisibilityIntensitySameTileChangeTypes;
#ifdef OUTBREAKS_AND_AFFLICTIONS
	AidArray m_aAidChanges;
#endif
	// int vector utilizing struct with delayed resolution
	std::vector<TerrainModifier> m_aTerrainAttackChangeModifiers;
	std::vector<TerrainModifier> m_aTerrainDefenseChangeModifiers;
	std::vector<TerrainModifier> m_aTerrainWorkChangeModifiers;
	std::vector<BuildModifier> m_aBuildWorkChangeModifiers;
	std::vector<FeatureModifier> m_aFeatureAttackChangeModifiers;
	std::vector<FeatureModifier> m_aFeatureDefenseChangeModifiers;
	std::vector<FeatureModifier> m_aFeatureWorkChangeModifiers;
	std::vector<UnitCombatModifier> m_aUnitCombatChangeModifiers;
	std::vector<UnitCombatModifier> m_aFlankingStrengthbyUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aWithdrawVSUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aPursuitVSUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aRepelVSUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aKnockbackVSUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aPunctureVSUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aArmorVSUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aDodgeVSUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aPrecisionVSUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aCriticalVSUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aRoundStunVSUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aTrapAvoidanceUnitCombatTypes;
#ifdef OUTBREAKS_AND_AFFLICTIONS
	std::vector<int> m_aiCureAfflictionChangeTypes;
	std::vector<PromotionLineModifier> m_aAfflictionFortitudeChangeModifiers;
	std::vector<AfflictOnAttackChange> m_aAfflictOnAttackChangeTypes;
	std::vector<AfflictionLineChanges> m_aDistanceAttackCommunicabilityTypeChanges;
#endif // OUTBREAKS_AND_AFFLICTIONS
	std::vector<InvisibleTerrainChanges> m_aInvisibleTerrainChanges;
	std::vector<InvisibleFeatureChanges> m_aInvisibleFeatureChanges;
	std::vector<InvisibleImprovementChanges> m_aInvisibleImprovementChanges;
	std::vector<InvisibleFeatureChanges> m_aVisibleFeatureChanges;
	std::vector<InvisibleImprovementChanges> m_aVisibleImprovementChanges;
	std::vector<InvisibleTerrainChanges> m_aVisibleTerrainChanges;
	std::vector<InvisibleFeatureChanges> m_aVisibleFeatureRangeChanges;
	std::vector<InvisibleImprovementChanges> m_aVisibleImprovementRangeChanges;
	std::vector<InvisibleTerrainChanges> m_aVisibleTerrainRangeChanges;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvMapCategoryInfo
//
//  DESC:   Contains info about Map Category Types
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMapCategoryInfo : public CvInfoBase
{
public:
	CvMapCategoryInfo();
	virtual ~CvMapCategoryInfo();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvMapCategoryInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvIdeaClassInfo
//
//  DESC:   Contains info about Idea Class Types
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvIdeaClassInfo :
	public CvInfoBase
{
public:

	CvIdeaClassInfo();
	virtual ~CvIdeaClassInfo();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvIdeaClassInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	bool isInitialized();

protected:
	bool m_bInitialized;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvIdeaInfo
//
//  DESC:   Contains info about Idea Types
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvIdeaInfo :
	public CvInfoBase
{
public:

	CvIdeaInfo();
	virtual ~CvIdeaInfo();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvIdeaInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	IdeaClassTypes getIdeaClass() const;

protected:
	IdeaClassTypes m_eIdeaClass;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvInvisibleInfo
//
//  DESC:   Contains info about Invisible Types
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvInvisibleInfo :
	public CvInfoBase
{
public:

	CvInvisibleInfo();
	virtual ~CvInvisibleInfo();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvInvisibleInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	void setChar(int i);
	int getChar() const { return m_iChar; }

	int getFontButtonIndex() const { return m_iFontButtonIndex; }

	bool isIntrinsic() const { return m_bIntrinsic; }

private:
	int m_iChar;
	int m_iFontButtonIndex;
	bool m_bIntrinsic;
};

class CvUnitAIInfo : public CvInfoBase { };
class CvDomainInfo : public CvInfoBase { };
class CvConceptInfo : public CvInfoBase { };
class CvNewConceptInfo : public CvInfoBase { };
class CvCityTabInfo : public CvInfoBase { };
class CvCalendarInfo : public CvInfoBase { };
class CvSeasonInfo : public CvInfoBase { };
class CvMonthInfo : public CvInfoBase { };
class CvDenialInfo : public CvInfoBase { };
class CvAttitudeInfo : public CvInfoBase { };
class CvMemoryInfo : public CvInfoBase { };
class CvHintInfo : public CvInfoBase { };

#endif
