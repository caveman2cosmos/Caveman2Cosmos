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

#include "CvXMLLoadUtilityModTools.h"
#include "CvArtFileMgr.h"
#include "CvProperties.h"
#include "CvPropertySource.h"
#include "CvPropertyInteraction.h"
#include "CvPropertyPropagator.h"
#include "CvPropertyManipulators.h"
#include "CvOutcomeList.h"
#include "CvOutcomeMission.h"
#include "CvDate.h"
#include "BoolExpr.h"
#include "IntExpr.h"
#include "IDValueMap.h"
#include "CheckSum.h"

#pragma warning( disable: 4251 )		// needs to have dll-interface to be used by clients of class
#pragma warning( disable: 4127 )


extern bool shouldHaveType;

class CvXMLLoadUtility;

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

	bool isGraphicalOnly() const; // Exposed to Python

	DllExport const TCHAR* getType() const; // Exposed to Python
	virtual const TCHAR* getButton() const; // Exposed to Python

	const wchar* getCivilopediaKey() const;
	const wchar* getHelpKey() const;
	const wchar* getStrategyKey() const;
	//const wchar* getGenericCategoryKey() const;

	// for python wide string handling
	std::wstring pyGetTextKey() const		{ return getTextKeyWide(); } // Exposed to Python
	std::wstring pyGetDescription() const	{ return getDescription(0); } // Exposed to Python
	std::wstring pyGetDescriptionForm(uint uiForm) const { return getDescription(uiForm); } // Exposed to Python
	std::wstring pyGetText() const			{ return getText(); } // Exposed to Python
	std::wstring pyGetCivilopedia() const	{ return getCivilopedia(); } // Exposed to Python
	std::wstring pyGetHelp() const			{ return getHelp(); } // Exposed to Python
	std::wstring pyGetStrategy() const		{ return getStrategy(); } // Exposed to Python
//	std::wstring pyGetGenericCategory()		{ return getGenericCategory(); } // Exposed to Python

	DllExport const wchar* getTextKeyWide() const;
	DllExport const wchar* getDescription(uint uiForm = 0) const;
	DllExport const wchar* getText() const;
	const wchar* getCivilopedia() const;
	DllExport const wchar* getHelp() const;
	const wchar* getStrategy() const;
/*	const wchar* getGenericCategory() const;*/

	bool isMatchForLink(std::wstring szLink, bool bKeysOnly) const;

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	virtual bool read(CvXMLLoadUtility* pXML);
	virtual bool readPass2(CvXMLLoadUtility* /*pXML*/) { FAssertMsg(false, "Override this"); return false; }
	virtual bool readPass3() { FAssertMsg(false, "Override this"); return false; }
	virtual void copyNonDefaults(CvInfoBase* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL );
	virtual void copyNonDefaultsReadPass2(CvInfoBase* pClassInfo = NULL , CvXMLLoadUtility* pXML = NULL, bool bOver = false)
	{ /* AIAndy: Default implementation for full copy of info without knowledge of one/twopass */ }
	virtual void getCheckSum(unsigned int& iSum) { };
	virtual const wchar* getExtraHoverText() const { return L""; };

protected:

	bool doneReadingXML(CvXMLLoadUtility* pXML);

	bool m_bGraphicalOnly;
	CvString m_szType;
	CvString m_szButton;	// Used for Infos that don't require an ArtAssetInfo
	CvWString m_szTextKey;
	CvWString m_szCivilopediaKey;
	CvWString m_szHelpKey;
	CvWString m_szStrategyKey;/*
	CvWString m_szGenericCategoryKey;*/


	// translated text
	std::vector<CvString> m_aszExtraXMLforPass3;
	mutable std::vector<CvWString> m_aCachedDescriptions;
	mutable CvWString m_szCachedText;
	mutable CvWString m_szCachedHelp;
	mutable CvWString m_szCachedStrategy;
	mutable CvWString m_szCachedCivilopedia;/*
	mutable CvWString m_szCachedGenericCategory;*/
};

//
// holds the scale for scalable objects
//
class CvScalableInfo
{
public:

	CvScalableInfo() : m_fScale(1.0f), m_fInterfaceScale(1.0f) { }

	DllExport float getScale() const;
	void setScale(float fScale);

	DllExport float getInterfaceScale() const;
	void setInterfaceScale(float fInterfaceScale);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvScalableInfo* pClassInfo, CvXMLLoadUtility* pXML);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

protected:

	float m_fScale; // Exposed to Python
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
	void copyNonDefaults(CvHotkeyInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	int getActionInfoIndex() const;
	void setActionInfoIndex(int i);

	int getHotKeyVal() const;
	void setHotKeyVal(int i);
	int getHotKeyPriority() const;
	void setHotKeyPriority(int i);
	int getHotKeyValAlt() const;
	void setHotKeyValAlt(int i);
	int getHotKeyPriorityAlt() const;
	void setHotKeyPriorityAlt(int i);
	int getOrderPriority() const;
	void setOrderPriority(int i);

	bool isAltDown() const;
	void setAltDown(bool b);
	bool isShiftDown() const;
	void setShiftDown(bool b);
	bool isCtrlDown() const;
	void setCtrlDown(bool b);
	bool isAltDownAlt() const;
	void setAltDownAlt(bool b);
	bool isShiftDownAlt() const;
	void setShiftDownAlt(bool b);
	bool isCtrlDownAlt() const;
	void setCtrlDownAlt(bool b);
	const TCHAR* getHotKey() const; // Exposed to Python
	void setHotKey(const TCHAR* szVal);
	const WCHAR* getHotKeyDescriptionKey() const;
	const WCHAR* getHotKeyAltDescriptionKey() const;
	const WCHAR* getHotKeyString() const;

	std::wstring getHotKeyDescription() const;
	void setHotKeyDescription(const wchar* szHotKeyDescKey, const wchar* szHotKeyAltDescKey, const wchar* szHotKeyString);

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
	void setNumDiplomacyText(int i);

	bool getCivilizationTypes(const int i) const;
	bool* getCivilizationTypes() const;
	void setCivilizationTypes(int i, bool bVal);

	bool getLeaderHeadTypes(const int i) const;
	bool* getLeaderHeadTypes() const;
	void setLeaderHeadTypes(int i, bool bVal);

	bool getAttitudeTypes(int i) const;
	bool* getAttitudeTypes() const;
	void setAttitudeTypes(int i, bool bVal);

	bool getDiplomacyPowerTypes(const int i) const;
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

	int getGreatPeopleUnitType() const; // Exposed to Python
	int getGreatPeopleRateChange() const; // Exposed to Python
	int getMissionType() const; // Exposed to Python
	void setMissionType(int iNewType);
	// Afforess 03/26/10
	int getHealthPercent() const;
	int getHappinessPercent() const;
	bool isSlave() const;						//Exposed to Python

	int getExperience() const; // Exposed to Python

	CvPropertyManipulators* getPropertyManipulators();

	bool isVisible() const; // Exposed to Python

	// Arrays

	int getYieldChange(int i) const; // Exposed to Python
	const int* getYieldChangeArray() const; // Exposed to Python - For Moose - CvWidgetData
	int getCommerceChange(int i) const; // Exposed to Python
	int getFlavorValue(int i) const; // Exposed to Python

	const TCHAR* getTexture() const; // Exposed to Python
	void setTexture(const TCHAR* szVal);

	//Team Project (1)
	//TB Specialist Tags
	// int
	int getInsidiousness() const;
	int getInvestigation() const;
	// int vector utilizing struct with delayed resolution
	int getNumTechHappinessTypes() const;
	const TechModifier& getTechHappinessType(int iTech) const;

	int getNumTechHealthTypes() const;
	const TechModifier& getTechHealthType(int iTech) const;

	int getNumUnitCombatExperienceTypes() const;
	const UnitCombatModifier& getUnitCombatExperienceType(int iUnitCombat, bool bForLoad = true) const;

	//TB Specialist Tags end

	void read(FDataStreamBase* ) {}
	void write(FDataStreamBase* ) {}

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvSpecialistInfo* pClassInfo, CvXMLLoadUtility* pXML);

	bool readPass3();

	void getCheckSum(unsigned int& iSum);
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
	std::vector<TechModifier> m_aTechHappinessTypes;
	std::vector<TechModifier> m_aTechHealthTypes;
	std::vector<UnitCombatModifier> m_aUnitCombatExperienceTypes;
	std::vector<UnitCombatModifier> m_aUnitCombatExperienceTypesNull;

	CvPropertyManipulators m_PropertyManipulators;
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

friend class CvXMLLoadUtility;

//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvTechInfo();
	virtual ~CvTechInfo();

	//	Validate tech info da after load (debug only)
	static void validate();

	int getAdvisorType() const; // Exposed to Python
	int getAIWeight() const; // Exposed to Python
	int getAITradeModifier() const; // Exposed to Python
	int getResearchCost() const; // Exposed to Python
	int getAdvancedStartCost() const; // Exposed to Python
	int getAdvancedStartCostIncrease() const; // Exposed to Python
	int getEra() const; // Exposed to Python
	int getTradeRoutes() const; // Exposed to Python
	int getFeatureProductionModifier() const; // Exposed to Python
	int getWorkerSpeedModifier() const; // Exposed to Python
	//DPII < Maintenance Modifier >
	int getMaintenanceModifier() const;
	int getDistanceMaintenanceModifier() const;
	int getNumCitiesMaintenanceModifier() const;
	int getCoastalDistanceMaintenanceModifier() const;
	//DPII < Maintenance Modifier >
	int getFirstFreeUnit() const; // Exposed to Python
	int getFirstFreeProphet() const; // Exposed to Python
	int getHealth() const; // Exposed to Python
	int getHappiness() const; // Exposed to Python
	int getFirstFreeTechs() const; // Exposed to Python
	int getAssetValue(bool bForLoad = false) const; // Exposed to Python
	int getPowerValue(bool bForLoad = false) const; // Exposed to Python

	int getGridX() const; // Exposed to Python
	int getGridY() const; // Exposed to Python

	bool isRepeat() const; // Exposed to Python
	bool isTrade() const; // Exposed to Python
	bool isDisable() const; // Exposed to Python
	bool isGoodyTech() const; // Exposed to Python
	bool isExtraWaterSeeFrom() const; // Exposed to Python
	bool isMapCentering() const; // Exposed to Python
	bool isMapVisible() const; // Exposed to Python
	bool isMapTrading() const; // Exposed to Python
	bool isTechTrading() const; // Exposed to Python
	bool isGoldTrading() const; // Exposed to Python
	bool isOpenBordersTrading() const; // Exposed to Python
	bool isDefensivePactTrading() const; // Exposed to Python
	bool isPermanentAllianceTrading() const; // Exposed to Python
	bool isVassalStateTrading() const; // Exposed to Python
	bool isBridgeBuilding() const; // Exposed to Python
	bool isIrrigation() const; // Exposed to Python
	bool isIgnoreIrrigation() const; // Exposed to Python
	bool isWaterWork() const; // Exposed to Python
	bool isRiverTrade() const; // Exposed to Python

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

	bool readPass3();

	void setPrereqOrTech(int i, int iTech);
	void setPrereqAndTech(int i, int iTech);
	void setGridX(int i);

	void setOriginalPrereqOrTech(int i, int iTech);
	void setOriginalPrereqAndTech(int i, int iTech);
	int getOriginalPrereqOrTechs(int i) const;
	int getOriginalPrereqAndTechs(int i) const;
	int getUnitStrengthChange(int iUnit, bool bForLoad = false) const;
	//TB Tech Tags
	bool isGlobal() const;
	//TB Tech Tags end

	// Dale - AB: Bombing START
	bool getDCMAirBombTech1() const;
	bool getDCMAirBombTech2() const;
protected:
	bool m_bDCMAirBombTech1;
	bool m_bDCMAirBombTech2;
public:
	// Dale - AB: Bombing END

	std::wstring getQuote() const; // Exposed to Python
	void setQuoteKey(const TCHAR* szVal);
	const TCHAR* getQuoteKey() const;
	const TCHAR* getSound() const; // Exposed to Python
	void setSound(const TCHAR* szVal);
	const TCHAR* getSoundMP() const; // Exposed to Python
	void setSoundMP(const TCHAR* szVal);

	// Arrays

	int getDomainExtraMoves(int i) const; // Exposed to Python
	int getFlavorValue(int i) const; // Exposed to Python
	int getPrereqOrTechs(int i) const; // Exposed to Python
	int getPrereqAndTechs(int i) const; // Exposed to Python

	bool isCommerceFlexible(int i) const; // Exposed to Python
	bool isTerrainTrade(int i) const; // Exposed to Python
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

	int* m_piPrereqOrTechs;
	int* m_piPrereqAndTechs;

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

	int* m_piOriginalPrereqOrTechs;
	int* m_piOriginalPrereqAndTechs;

	int* m_piUnitStrengthChange;

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
	int getPrereqPromotion() const; // Exposed to Python
	void setPrereqPromotion(int i); // Exposed to Python
	int getPrereqOrPromotion1() const; // Exposed to Python
	void setPrereqOrPromotion1(int i); // Exposed to Python
	int getPrereqOrPromotion2() const; // Exposed to Python
	void setPrereqOrPromotion2(int i); // Exposed to Python

	int getTechPrereq() const; // Exposed to Python
	int getMinEraType() const;
	int getMaxEraType() const;
	int getStateReligionPrereq() const; // Exposed to Python
	int getVisibilityChange() const; // Exposed to Python
	int getMovesChange() const; // Exposed to Python
	int getMoveDiscountChange() const; // Exposed to Python
	int getAirRangeChange() const; // Exposed to Python
	int getInterceptChange() const; // Exposed to Python
	int getEvasionChange() const; // Exposed to Python
	int getWithdrawalChange() const; // Exposed to Python
	int getCargoChange() const; // Exposed to Python
	int getSMCargoChange() const;
	int getSMCargoVolumeChange() const;
	int getSMCargoVolumeModifierChange() const;
	int getCollateralDamageChange() const; // Exposed to Python
	int getBombardRateChange() const; // Exposed to Python
	int getFirstStrikesChange() const; // Exposed to Python
	int getChanceFirstStrikesChange() const; // Exposed to Python
	int getEnemyHealChange() const; // Exposed to Python
	int getNeutralHealChange() const; // Exposed to Python
	int getFriendlyHealChange() const; // Exposed to Python
	int getSameTileHealChange() const; // Exposed to Python
	int getAdjacentTileHealChange() const; // Exposed to Python
	int getCombatPercent() const; // Exposed to Python
	int getCityAttackPercent() const; // Exposed to Python
	int getCityDefensePercent() const; // Exposed to Python
	int getHillsAttackPercent() const; // Exposed to Python
	int getHillsDefensePercent() const; // Exposed to Python
	int getHillsWorkPercent() const;
	//ls612: Work rate modifiers
	int getWorkRatePercent() const;
	int getCommandType() const; // Exposed to Python
	void setCommandType(int iNewType);

	int getRevoltProtection() const; // Exposed to Python
	int getCollateralDamageProtection() const; // Exposed to Python
	int getPillageChange() const; // Exposed to Python
	int getUpgradeDiscount() const; // Exposed to Python
	int getExperiencePercent() const; // Exposed to Python
	int getKamikazePercent() const; // Exposed to Python
/*****************************************************************************************************/
/**  Author: TheLadiesOgre																		  **/
/**  Date: 16.09.2009																			   **/
/**  ModComp: TLOTags																			   **/
/**  Reason Added: New Tag Definition															   **/
/**  Notes:																						 **/
/*****************************************************************************************************/
	int getAirCombatLimitChange() const; // Exposed to Python
	int getCelebrityHappy() const; // Exposed to Python
	int getCollateralDamageLimitChange() const; // Exposed to Python
	int getCollateralDamageMaxUnitsChange() const; // Exposed to Python
	int getCombatLimitChange() const; // Exposed to Python
	int getExtraDropRange() const; // Exposed to Python
	//Team Project (2)
	int getSurvivorChance() const;
	int getVictoryAdjacentHeal() const;
	//
	int getVictoryHeal() const; // Exposed to Python
	//Team Project (2)
	int getVictoryStackHeal() const;
	//
	bool isDefensiveVictoryMove() const; // Exposed to Python
	bool isFreeDrop() const; // Exposed to Python
	bool isOffensiveVictoryMove() const; // Exposed to Python
	//Team Project (2)
	bool isOneUp() const;
	//
	bool isPillageCulture() const; // Exposed to Python
	bool isPillageEspionage() const; // Exposed to Python
	bool isPillageMarauder() const; // Exposed to Python
	bool isPillageOnMove() const; // Exposed to Python
	bool isPillageOnVictory() const; // Exposed to Python
	bool isPillageResearch() const; // Exposed to Python
/*****************************************************************************************************/
/**  TheLadiesOgre; 16.09.2009; TLOTags															 **/
/*****************************************************************************************************/

	bool isLeader() const; // Exposed to Python
	bool isBlitz() const; // Exposed to Python
	bool isAmphib() const; // Exposed to Python
	bool isRiver() const; // Exposed to Python
	bool isEnemyRoute() const; // Exposed to Python
	bool isAlwaysHeal() const; // Exposed to Python
	bool isHillsDoubleMove() const; // Exposed to Python
	bool isImmuneToFirstStrikes() const; // Exposed to Python

	const TCHAR* getSound() const; // Exposed to Python
	void setSound(const TCHAR* szVal);

	bool changesMoveThroughPlots() const;
	//	This really belongs on CvInfoBase but you can't change the size of that
	//	object without crashing the core engine :-(
	inline int	getZobristValue() const { return m_zobristValue; }

	// Arrays

	int getTerrainAttackPercent(int i) const; // Exposed to Python
	bool isAnyTerrainAttackPercent() const;
	int getTerrainDefensePercent(int i) const; // Exposed to Python
	bool isAnyTerrainDefensePercent() const;
	int getFeatureAttackPercent(int i) const; // Exposed to Python
	bool isAnyFeatureAttackPercent() const;
	int getFeatureDefensePercent(int i) const; // Exposed to Python
	bool isAnyFeatureDefensePercent() const;
	int getUnitCombatModifierPercent(int i) const; // Exposed to Python
	bool isAnyUnitCombatModifierPercent() const;
	int getDomainModifierPercent(int i) const; // Exposed to Python
	bool isAnyDomainModifierPercent() const;
	//ls612: Terrain Work Modifiers
	int getTerrainWorkPercent(int i) const;
	int getFeatureWorkPercent(int i) const;

	bool getTerrainDoubleMove(int i) const; // Exposed to Python
	bool getFeatureDoubleMove(int i) const; // Exposed to Python
	bool getUnitCombat(int i) const; // Exposed to Python


	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}

/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
	bool isCanMovePeaks() const;
	//	Koshling - enhanced mountaineering mode to differentiate between ability to move through
	//	mountains, and ability to lead a stack through mountains
	bool isCanLeadThroughPeaks() const;
	int getNumPromotionOverwrites() const;
	PromotionTypes getPromotionOverwrites(int iI) const;
	int getObsoleteTech() const;
	int getControlPoints() const;
	int getCommandRange() const;
	bool isZoneOfControl() const;
	int getAssetMultiplier() const;
	int getPowerMultiplier() const;
	int getIgnoreTerrainDamage() const;

	void getCheckSum(unsigned int& iSum);

	CvPropertyManipulators* getPropertyManipulators();

	//TB Combat Mods Begin  TB SubCombat Mod begin
	//Text Strings
	const wchar* getRenamesUnitTo() const;
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
	int getPursuitChange(bool bForLoad = false) const;
	int getEarlyWithdrawChange(bool bForLoad = false) const;
	int getTauntChange() const;
	//
	int getVSBarbsChange() const;
	int getArmorChange() const;
	int getPunctureChange() const;
	// Heart of War
	int getOverrunChange(bool bForLoad = false) const;
	int getRepelChange(bool bForLoad = false) const;
	int getFortRepelChange(bool bForLoad = false) const;
	int getRepelRetriesChange(bool bForLoad = false) const;
	int getUnyieldingChange(bool bForLoad = false) const;
	int getKnockbackChange(bool bForLoad = false) const;
	int getKnockbackRetriesChange(bool bForLoad = false) const;
	//
	// Battleworn
	int getStrAdjperRndChange(bool bForLoad = false) const;
	int getStrAdjperAttChange(bool bForLoad = false) const;
	int getStrAdjperDefChange(bool bForLoad = false) const;
	int getWithdrawAdjperAttChange(bool bForLoad = false) const;
	//
	// S&D Extended
	int getUnnerveChange(bool bForLoad = false) const;
	int getEncloseChange(bool bForLoad = false) const;
	int getLungeChange(bool bForLoad = false) const;
	int getDynamicDefenseChange(bool bForLoad = false) const;
	//
	int getStrengthChange() const;
	int getLinePriority() const;
	int getFortitudeChange() const;
	int getDamageperTurn() const;
	int getStrAdjperTurn() const;
	int getWeakenperTurn() const;
	// Strength in Numbers
	int getFrontSupportPercentChange(bool bForLoad = false) const;
	int getShortRangeSupportPercentChange(bool bForLoad = false) const;
	int getMediumRangeSupportPercentChange(bool bForLoad = false) const;
	int getLongRangeSupportPercentChange(bool bForLoad = false) const;
	int getFlankSupportPercentChange(bool bForLoad = false) const;
	//
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
	//Team Project (3)
	int getCaptureProbabilityModifierChange() const;
	int getCaptureResistanceModifierChange() const;
	//Team Project (4)
	//WorkRateMod
	int getHillsWorkModifierChange() const;
	int getPeaksWorkModifierChange() const;
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
	int getCostModifierChange() const;
	int getRBombardDamageChange() const;
	int getRBombardDamageLimitChange() const;
	int getRBombardDamageMaxUnitsChange() const;
	int getDCMBombRangeChange() const;
	int getDCMBombAccuracyChange() const;
	int getCombatModifierPerSizeMoreChange(bool bForLoad = false) const;
	int getCombatModifierPerSizeLessChange(bool bForLoad = false) const;
	int getCombatModifierPerVolumeMoreChange(bool bForLoad = false) const;
	int getCombatModifierPerVolumeLessChange(bool bForLoad = false) const;
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
	int getStealthStrikesChange(bool bForLoad = false) const;
	int getStealthCombatModifierChange(bool bForLoad = false) const;
	int getStealthDefenseChange(bool bForLoad = false) const;
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
	bool isAffliction() const;
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
	bool isOnGameOption(int i)const;

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

	int getMapCategoryType(int i) const;
	int getNumMapCategoryTypes() const;
	bool isMapCategoryType(int i) const;

	// bool vector with delayed resolution
	int getCureAfflictionChangeType(int i) const;
	int getNumCureAfflictionChangeTypes() const;
	bool isCureAfflictionChangeType(int i) const;

	int getPrereqBonusType(int i) const;
	int getNumPrereqBonusTypes() const;
	bool isPrereqBonusType(int i) const;

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

	int getNumAidChanges() const;
	int getAidChange(int iProperty) const;
	bool isAidChange(int iProperty) const;
	//Team Project (4)
	//WorkRateMod
	int getNumTerrainWorkRateModifierChangeTypes() const;
	int getTerrainWorkRateModifierChangeType(int iTerrain) const;
	bool isTerrainWorkRateModifierChangeType(int iTerrain) const;

	int getNumFeatureWorkRateModifierChangeTypes() const;
	int getFeatureWorkRateModifierChangeType(int iFeature) const;
	bool isFeatureWorkRateModifierChangeType(int iFeature) const;

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

	int getNumAfflictOnAttackChangeTypes() const;
	const AfflictOnAttackChange& getAfflictOnAttackChangeType(int iAfflictionLine) const;

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

	bool hasNegativeEffects() const;

protected:
	bool m_bCanMovePeaks;
	//	Koshling - enhanced mountaineering mode to differentiate between ability to move through
	//	mountains, and ability to lead a stack through mountains
	bool m_bCanLeadThroughPeaks;
	int m_iNumPromotionOverwrites;
	int* m_piPromotionOverwrites;
	int m_iObsoleteTech;
	int m_iControlPoints;
	int m_iCommandRange;
	bool m_bZoneOfControl;
	int m_iAssetMultiplier;
	int m_iPowerMultiplier;
	int m_iIgnoreTerrainDamage;
	int m_zobristValue;
public:
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvPromotionInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void copyNonDefaultsReadPass2(CvPromotionInfo* pClassInfo = NULL , CvXMLLoadUtility* pXML = NULL, bool bOver = false);

//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iLayerAnimationPath;
	int m_iPrereqPromotion;
	int m_iPrereqOrPromotion1;
	int m_iPrereqOrPromotion2;

	int m_iTechPrereq;
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
	//ls612: Work rate modifiers
	int m_iWorkRatePercent;
	int m_iCommandType;
	int m_iRevoltProtection;
	int m_iCollateralDamageProtection;
	int m_iPillageChange;
	int m_iUpgradeDiscount;
	int m_iExperiencePercent;
	int m_iKamikazePercent;
/*****************************************************************************************************/
/**  Author: TheLadiesOgre																		  **/
/**  Date: 16.09.2009																			   **/
/**  ModComp: TLOTags																			   **/
/**  Reason Added: New Tag Definition															   **/
/**  Notes:																						 **/
/*****************************************************************************************************/
	int m_iAirCombatLimitChange;
	int m_iCelebrityHappy;
	int m_iCollateralDamageLimitChange;
	int m_iCollateralDamageMaxUnitsChange;
	int m_iCombatLimitChange;
	int m_iExtraDropRange;
	//Team Project (2)
	int m_iSurvivorChance;
	int m_iVictoryAdjacentHeal;
	int m_iVictoryHeal;
	//Team Project (2)
	int m_iVictoryStackHeal;
	bool m_bDefensiveVictoryMove;
	bool m_bFreeDrop;
	bool m_bOffensiveVictoryMove;
	//Team Project (2)
	bool m_bOneUp;
	bool m_bPillageCulture;
	bool m_bPillageEspionage;
	bool m_bPillageMarauder;
	bool m_bPillageOnMove;
	bool m_bPillageOnVictory;
	bool m_bPillageResearch;
/*****************************************************************************************************/
/**  TheLadiesOgre; 16.09.2009; TLOTags															 **/
/*****************************************************************************************************/

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
	bool m_bAnyTerrainAttackPercent;
	bool m_bAnyTerrainDefensePercent;
	bool m_bAnyFeatureAttackPercent;
	bool m_bAnyFeatureDefensePercent;
	bool m_bAnyUnitCombatModifierPercent;
	bool m_bAnyDomainModifierPercent;
	//bool m_bAnyAIWeightbyUnitCombatType;
	//ls612: Terrain Work Modifiers
	int* m_piTerrainWorkPercent;
	int* m_piFeatureWorkPercent;

	bool* m_pbTerrainDoubleMove;
	bool* m_pbFeatureDoubleMove;
	bool* m_pbUnitCombat;

	CvPropertyManipulators m_PropertyManipulators;

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
	int m_iStrAdjperRndChange;
	int m_iStrAdjperAttChange;
	int m_iStrAdjperDefChange;
	int m_iWithdrawAdjperAttChange;
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
	int m_iFrontSupportPercentChange;
	int m_iShortRangeSupportPercentChange;
	int m_iMediumRangeSupportPercentChange;
	int m_iLongRangeSupportPercentChange;
	int m_iFlankSupportPercentChange;
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
	//Team Project (3)
	int m_iCaptureProbabilityModifierChange;
	int m_iCaptureResistanceModifierChange;
	//Team Project (4)
	//WorkRateMod
	int m_iHillsWorkModifierChange;
	int m_iPeaksWorkModifierChange;

	int m_iBreakdownChanceChange;
	int m_iBreakdownDamageChange;
	int m_iTauntChange;
	int m_iMaxHPChange;
	int m_iStrengthModifier;
	int m_iQualityChange;
	int m_iGroupChange;
	int m_iLevelPrereq;
	int m_iDamageModifierChange;
	int m_iCostModifierChange;
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
	bool m_bAffliction;
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
	std::vector<int> m_aiMapCategoryTypes;
	// bool vector with delayed resolution
	std::vector<int> m_aiCureAfflictionChangeTypes;
	std::vector<int> m_aiPrereqBonusTypes;
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
	AidArray m_aAidChanges;
	//Team Project (4)
		//WorkRateMod
	TerrainModifierArray m_aTerrainWorkRateModifierChangeTypes;
	FeatureModifierArray m_aFeatureWorkRateModifierChangeTypes;
	BuildModifierArray m_aBuildWorkRateModifierChangeTypes;
	InvisibilityArray m_aVisibilityIntensityChangeTypes;
	InvisibilityArray m_aInvisibilityIntensityChangeTypes;
	InvisibilityArray m_aVisibilityIntensityRangeChangeTypes;
	// int vector utilizing struct with delayed resolution
	std::vector<UnitCombatModifier> m_aAIWeightbyUnitCombatTypes;
	std::vector<PromotionLineModifier> m_aAfflictionFortitudeChangeModifiers;
	std::vector<AfflictOnAttackChange> m_aAfflictOnAttackChangeTypes;
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

	DllExport int getTime() const; // Exposed to Python

	bool isSound() const; // Exposed to Python
	DllExport bool isTarget() const; // Exposed to Python
	bool isBuild() const; // Exposed to Python
	bool getVisible() const; // Exposed to Python
	DllExport EntityEventTypes getEntityEvent() const;

	const TCHAR* getWaypoint() const;		// effect type, Exposed to Python

	bool read(CvXMLLoadUtility* pXML);

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvMissionInfo* pClassInfo, CvXMLLoadUtility* pXML);

	// Should not be needed to checksum this as this will likely not get changed without DLL changes
	//void getCheckSum(unsigned int& iSum);

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
	void copyNonDefaults(CvControlInfo* pClassInfo, CvXMLLoadUtility* pXML);

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
	void setAutomate(int i);

	bool getConfirmCommand() const;
	bool getVisible() const;
	bool getAll() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvCommandInfo* pClassInfo, CvXMLLoadUtility* pXML);

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
	void setCommand(int i);
	int getAutomate() const;
	void setAutomate(int i);

	bool getConfirmCommand() const;
	void setConfirmCommand(bool bVal);
	bool getVisible() const;
	void setVisible(bool bVal);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvAutomateInfo* pClassInfo, CvXMLLoadUtility* pXML);

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

	int getMissionData() const; // Exposed to Python
	int getCommandData() const; // Exposed to Python

	int getAutomateType() const;
	int getInterfaceModeType() const; // Exposed to Python
	int getMissionType() const; // Exposed to Python
	int getCommandType() const; // Exposed to Python
	int getControlType() const; // Exposed to Python
	int getOriginalIndex() const;
	void setOriginalIndex(int i);

	bool isConfirmCommand() const; // Exposed to Python
	DllExport bool isVisible() const; // Exposed to Python
	DllExport ActionSubTypes getSubType() const;
	void setSubType(ActionSubTypes eSubType);

	// functions to replace the CvInfoBase calls
	const TCHAR* getType() const;
	const wchar* getDescription() const;
	const wchar* getCivilopedia() const;
	const wchar* getHelp() const;
	const wchar* getStrategy() const;/*
	const wchar* getGenericCategory() const;*/
	virtual const TCHAR* getButton() const;
	const wchar* getTextKeyWide() const;

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

	const TCHAR* getHotKey() const; // Exposed to Python

	std::wstring getHotKeyDescription() const; // Exposed to Python

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

	int getMaxGlobalInstances() const; // Exposed to Python
	int getMaxPlayerInstances() const; // Exposed to Python
	bool isUnlimitedException() const; // Exposed to Python
	bool isCivilizationUnit(const PlayerTypes ePlayer=NO_PLAYER) const;

	int getInstanceCostModifier() const; // Exposed to Python
	int getAIWeight() const; // Exposed to Python
	int getProductionCost() const; // Exposed to Python
	int getHurryCostModifier() const; // Exposed to Python
	int getAdvancedStartCost() const; // Exposed to Python
	int getAdvancedStartCostIncrease() const; // Exposed to Python
	int getMinAreaSize() const; // Exposed to Python
	int getMoves() const; // Exposed to Python
	int getAirRange() const; // Exposed to Python
	int getAirUnitCap() const; // Exposed to Python
	int getDropRange() const; // Exposed to Python
	int getNukeRange() const; // Exposed to Python
	int getWorkRate() const; // Exposed to Python
	int getBaseDiscover() const; // Exposed to Python
	int getDiscoverMultiplier() const; // Exposed to Python
	int getBaseHurry() const; // Exposed to Python
	int getHurryMultiplier() const; // Exposed to Python
	int getBaseTrade() const; // Exposed to Python
	int getTradeMultiplier() const; // Exposed to Python
	int getGreatWorkCulture() const; // Exposed to Python
	int getEspionagePoints() const; // Exposed to Python
	int getCombat() const; // Exposed to Python

	void setCombat(int iNum); // Exposed to Python
	int getCombatLimit() const; // Exposed to Python
	//TB Tweaked for Size Matters
	int getAirCombat() const; // Exposed to Python

	int getAirCombatLimit() const; // Exposed to Python
	int getXPValueAttack() const; // Exposed to Python
	int getXPValueDefense() const; // Exposed to Python
	int getFirstStrikes() const; // Exposed to Python
	int getChanceFirstStrikes() const; // Exposed to Python
	int getInterceptionProbability() const; // Exposed to Python
	//ls612: Advanced Nuke Interception
	//int getNukeInterceptionProbability() const;
	//int getNukeInterceptionRange() const;
	int getEvasionProbability() const; // Exposed to Python
	int getWithdrawalProbability(bool bForLoad = false) const; // Exposed to Python
	int getCollateralDamage() const; // Exposed to Python
	int getCollateralDamageLimit() const; // Exposed to Python
	int getCollateralDamageMaxUnits() const; // Exposed to Python
	int getCityAttackModifier() const; // Exposed to Python
	int getCityDefenseModifier() const; // Exposed to Python
	int getAnimalCombatModifier() const; // Exposed to Python
	int getHillsAttackModifier() const; // Exposed to Python
	int getHillsDefenseModifier() const; // Exposed to Python
	int getBombRate() const; // Exposed to Python
	int getBombardRate() const; // Exposed to Python
	int getSpecialCargo(bool bForLoad = false) const; // Exposed to Python
	int getSMSpecialCargo() const;
	int getSMNotSpecialCargo() const;
	int getDomainCargo() const; // Exposed to Python
	int getCargoSpace() const; // Exposed to Python
	int getSMCargoSpace() const;
	int getSMCargoVolume() const;
	int getConscriptionValue() const; // Exposed to Python
	int getCultureGarrisonValue() const; // Exposed to Python
	int getExtraCost() const; // Exposed to Python
	int getAssetValue(bool bForLoad = false) const; // Exposed to Python
	int getPowerValue(bool bForLoad = false) const; // Exposed to Python
	int getSpecialUnitType() const; // Exposed to Python
	int getUnitCaptureType() const; // Exposed to Python
	int getUnitCombatType() const; // Exposed to Python
	int getDomainType() const; // Exposed to Python
	int getDefaultUnitAIType() const; // Exposed to Python
	int getInvisibleType() const; // Exposed to Python
	int getSeeInvisibleType(int i) const; // Exposed to Python
	int getNumSeeInvisibleTypes() const; // Exposed to Python
	int getAdvisorType() const; // Exposed to Python
	int getMaxStartEra() const; // Exposed to Python
	int getForceObsoleteTech() const; // Exposed to Python
	bool isStateReligion() const; // Exposed to Python
	int getPrereqGameOption() const; // Exposed to Python
	int getNotGameOption() const; // Exposed to Python
	int getHolyCity() const; // Exposed to Python
	int getReligionType() const; // Exposed to Python
	int getStateReligion() const; // Exposed to Python
	int getPrereqReligion() const; // Exposed to Python
	int getPrereqCorporation() const; // Exposed to Python
	int getPrereqOrBuildingsNum() const; // Exposed to Python
	BuildingTypes getPrereqOrBuilding(int i) const; // Exposed to Python
	int getPrereqAndTech() const; // Exposed to Python
	int getPrereqAndBonus() const; // Exposed to Python
	// the initial number of individuals in the unit group
	int getGroupSize() const; // Exposed to Python
	// the number of UnitMeshGroups for this unit
	int getGroupDefinitions() const; // Exposed to Python
	int getMeleeWaveSize() const; // Exposed to Python
	int getRangedWaveSize() const; // Exposed to Python
	int getNumUnitNames() const; // Exposed to Python
	int getCommandType() const; // Exposed to Python
	void setCommandType(int iNewType);
	//	This really belongs on CvInfoBase but you can't change the size of that
	//	object without crashing the core engine :-(
	inline int	getZobristValue() const { return m_zobristValue; }

	bool isFoodProduction() const; // Exposed to Python
	bool isNoBadGoodies() const; // Exposed to Python
	bool isOnlyDefensive() const; // Exposed to Python
	bool isNoCapture() const; // Exposed to Python
	bool isQuickCombat() const; // Exposed to Python
	bool isRivalTerritory() const; // Exposed to Python
	bool isMilitaryHappiness() const; // Exposed to Python
	bool isMilitarySupport() const; // Exposed to Python
	bool isMilitaryProduction() const; // Exposed to Python
	bool isPillage() const; // Exposed to Python
	bool isSpy() const; // Exposed to Python
	bool isSabotage() const; // Exposed to Python
	bool isDestroy() const; // Exposed to Python
	bool isStealPlans() const; // Exposed to Python
	bool isInvestigate() const; // Exposed to Python
	bool isCounterSpy() const; // Exposed to Python
	bool isFound() const; // Exposed to Python
	bool isGoldenAge() const; // Exposed to Python
	bool isInvisible() const; // Exposed to Python
	void setInvisible(bool bEnable) ; // Exposed to Python
	bool isFirstStrikeImmune() const; // Exposed to Python
	bool isNoDefensiveBonus() const; // Exposed to Python
	bool isIgnoreBuildingDefense() const; // Exposed to Python
	bool isCanMoveImpassable() const; // Exposed to Python
	bool isCanMoveAllTerrain() const; // Exposed to Python
	bool isFlatMovementCost() const; // Exposed to Python
	bool isIgnoreTerrainCost() const; // Exposed to Python
	bool isNukeImmune() const; // Exposed to Python
	bool isPrereqBonuses() const; // Exposed to Python
	bool isMechUnit() const; // Exposed to Python
	bool isRenderBelowWater() const; // Exposed to Python
	bool isRenderAlways() const; // Exposed to Python
	bool isSuicide() const; // Exposed to Python
	bool isLineOfSight() const; // Exposed to Python
	bool isHiddenNationality() const; // Exposed to Python
	bool isAlwaysHostile() const; // Exposed to Python
	bool isFreeDrop() const; // Exposed to Python
	bool isNoRevealMap() const; // Exposed to Python
	bool isInquisitor() const; // Exposed to Python
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
	int getPrereqOrVicinityBonuses(int i) const;
	bool getPassableRouteNeeded(int i) const;
	int getBaseFoodChange() const;
	int getControlPoints() const;
	int getCommandRange() const;
	const TCHAR* getClassicalArtDefineTag(int i, UnitArtStyleTypes eStyle) const; // Exposed to Python
	void setClassicalArtDefineTag(int i, const TCHAR* szVal);
	const TCHAR* getRennArtDefineTag(int i, UnitArtStyleTypes eStyle) const; // Exposed to Python
	void setRennArtDefineTag(int i, const TCHAR* szVal);
	const TCHAR* getIndustrialArtDefineTag(int i, UnitArtStyleTypes eStyle) const; // Exposed to Python
	void setIndustrialArtDefineTag(int i, const TCHAR* szVal);
	const TCHAR* getFutureArtDefineTag(int i, UnitArtStyleTypes eStyle) const; // Exposed to Python
	void setFutureArtDefineTag(int i, const TCHAR* szVal);

	CvWString getCivilizationName(int i) const;
	int getCivilizationNamesVectorSize() const;
	CvWString getCivilizationNamesNamesVectorElement(const int i) const;
	CvWString getCivilizationNamesValuesVectorElement(const int i) const;

	//TB Combat Mod Begin  TB SubCombat Mod begin
	//Functions
	void setReligionSubCombat();
	void setCultureSubCombat();
	int getEraInfo() const;
	void setEraSubCombat();
	//integers
	int getAttackCombatModifier() const;
	int getDefenseCombatModifier() const;
	int getPursuit(bool bForLoad = false) const;
	int getEarlyWithdraw(bool bForLoad = false) const;
	int getVSBarbs() const;
	int getArmor() const;
	int getPuncture() const;
	int getOverrun(bool bForLoad = false) const;
	int getRepel(bool bForLoad = false) const;
	int getFortRepel(bool bForLoad = false) const;
	int getRepelRetries(bool bForLoad = false) const;
	int getUnyielding(bool bForLoad = false) const;
	int getKnockback(bool bForLoad = false) const;
	int getKnockbackRetries(bool bForLoad = false) const;
	int getStrAdjperRnd(bool bForLoad = false) const;
	int getStrAdjperAtt(bool bForLoad = false) const;
	int getStrAdjperDef(bool bForLoad = false) const;
	int getWithdrawAdjperAtt(bool bForLoad = false) const;
	int getUnnerve(bool bForLoad = false) const;
	int getEnclose(bool bForLoad = false) const;
	int getLunge(bool bForLoad = false) const;
	int getDynamicDefense(bool bForLoad = false) const;
	int getFortitude(bool bForLoad = false) const;
	int getFrontSupportPercent(bool bForLoad = false) const;
	int getShortRangeSupportPercent(bool bForLoad = false) const;
	int getMediumRangeSupportPercent(bool bForLoad = false) const;
	int getLongRangeSupportPercent(bool bForLoad = false) const;
	int getFlankSupportPercent(bool bForLoad = false) const;
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
	//Team Project (3)
	int getCaptureProbabilityModifier() const;
	int getCaptureResistanceModifier() const;
	//Team Project (4)
	//WorkRateMod
	int getHillsWorkModifier() const;
	int getPeaksWorkModifier() const;

	int getBreakdownChance() const;
	int getBreakdownDamage() const;
	int getTaunt(bool bForLoad = false) const;
	int getMaxHP(bool bForLoad = false) const;
	int getDamageModifier() const;
	int getCostModifier() const;
	int getRBombardDamage() const;
	int getRBombardDamageLimit() const;
	int getRBombardDamageMaxUnits() const;
	int getCombatModifierPerSizeMore(bool bForLoad = false) const;
	int getCombatModifierPerSizeLess(bool bForLoad = false) const;
	int getCombatModifierPerVolumeMore(bool bForLoad = false) const;
	int getCombatModifierPerVolumeLess(bool bForLoad = false) const;
	int getSelfHealModifier() const;
	int getNumHealSupport() const;
	int getInsidiousness() const;
	int getInvestigation() const;
	int getStealthStrikes(bool bForLoad = false) const;
	int getStealthCombatModifier(bool bForLoad = false) const;
	int getTrapDamageMax() const;
	int getTrapDamageMin() const;
	int getTrapComplexity() const;
	int getNumTriggers() const;
	int getAggression() const;
	int getAnimalIgnoresBorders() const;
	int getReligiousCombatModifier() const;
	// booleans
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
	bool isStealthDefense(bool bForLoad = false) const;
	bool isNoInvisibility() const;
	bool isTriggerBeforeAttack() const;
	bool isAnimal() const;
	bool isWildAnimal() const;
	bool canAnimalIgnoresBorders() const;
	bool canAnimalIgnoresImprovements() const;
	bool canAnimalIgnoresCities() const;
	bool isNoNonTypeProdMods() const;
	bool isGatherHerd() const;

	// boolean vectors without delayed resolution
	int getSubCombatType(int i) const;
	int getNumSubCombatTypes() const;
	bool isSubCombatType(int i) const;

	int getCureAfflictionType(int i) const;
	int getNumCureAfflictionTypes() const;
	bool isCureAfflictionType(int i) const;

	int getHealAsType(int i) const;
	int getNumHealAsTypes() const;
	bool isHealAsType(int i) const;
	void setHealAsTypes();

	int getTerrainImpassableType(int i) const;
	int getNumTerrainImpassableTypes() const;
	bool isTerrainImpassableType(int i) const;

	int getFeatureImpassableType(int i) const;
	int getNumFeatureImpassableTypes() const;
	bool isFeatureImpassableType(int i) const;

	int getMapCategoryType(int i) const;
	int getNumMapCategoryTypes() const;
	bool isMapCategoryType(int i);

	int getTrapSetWithPromotionType(int i) const;
	int getNumTrapSetWithPromotionTypes() const;
	bool isTrapSetWithPromotionType(int i) const;

	int getTrapImmunityUnitCombatType(int i) const;
	int getNumTrapImmunityUnitCombatTypes() const;
	bool isTrapImmunityUnitCombatType(int i)const;

	int getNumAfflictionFortitudeModifiers() const;
	const PromotionLineModifier& getAfflictionFortitudeModifier(int iAffliction) const;

	int getNumAfflictOnAttackTypes() const;
	const AfflictOnAttack& getAfflictOnAttackType(int iAfflictionLine) const;

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

	//Bool vector utilizing delayed resolution
	int getNumEnabledCivilizationTypes() const;
	const EnabledCivilizations& getEnabledCivilizationType(int iIndex) const;

	// int vector utilizing pairing without delayed resolution
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

	//Team Project (4)
		//WorkRateMod
	int getNumTerrainWorkRateModifierTypes() const;
	int getTerrainWorkRateModifierType(int iTerrain) const;
	bool isTerrainWorkRateModifierType(int iTerrain) const;

	int getNumFeatureWorkRateModifierTypes() const;
	int getFeatureWorkRateModifierType(int iFeature) const;
	bool isFeatureWorkRateModifierType(int iFeature) const;

	int getNumBuildWorkRateModifierTypes() const;
	int getBuildWorkRateModifierType(int iBuild) const;
	bool isBuildWorkRateModifierType(int iBuild) const;

	int getNumAidChanges() const;
	int getAidChange(int iProperty) const;
	bool isAidChange(int iProperty) const;


	// TB Combat Mod End TB SubCombat Mod end
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
	int getQualifiedPromotionType(int i) const;
	int getNumQualifiedPromotionTypes() const;
	bool isQualifiedPromotionType(int i) const;
	void setQualifiedPromotionTypes();
	void setCanAnimalIgnores();

	virtual const wchar* getExtraHoverText() const;

	void getCheckSum(unsigned int& iSum);

	CvPropertyManipulators* getPropertyManipulators();

	BoolExpr* getTrainCondition();

protected:
	int m_iMaxGlobalInstances;
	int m_iMaxPlayerInstances;
	bool m_bUnlimitedException;
	int m_iInstanceCostModifier;
	bool* m_pbPassableRouteNeeded;
	int* m_piPrereqOrVicinityBonuses;
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

	BoolExpr* m_pExprTrainCondition;

public:
/************************************************************************************************/
/* DCM									 04/19/09								Johny Smith  */
/************************************************************************************************/
	// Dale - RB: Field Bombard START
	int getDCMBombRange() const;
	int getDCMBombAccuracy() const;
protected:
	int m_iDCMBombRange;
	int m_iDCMBombAccuracy;
public:
	// Dale - RB: Field Bombard END
	// Dale - AB: Bombing START
	bool getDCMAirBomb1() const;
	bool getDCMAirBomb2() const;
	bool getDCMAirBomb3() const;
	bool getDCMAirBomb4() const;
	bool getDCMAirBomb5() const;
protected:
	bool m_bDCMAirBomb1;
	bool m_bDCMAirBomb2;
	bool m_bDCMAirBomb3;
	bool m_bDCMAirBomb4;
	bool m_bDCMAirBomb5;
public:
	// Dale - AB: Bombing END
	// Dale - FE: Fighters START
	bool getDCMFighterEngage() const;
protected:
	bool m_bDCMFighterEngage;
public:
	// Dale - FE: Fighters END
/************************************************************************************************/
/* DCM									 END												  */
/************************************************************************************************/
/************************************************************************************************/
/* REVOLUTION_MOD								 01/01/08						DPII		  */
/*																							  */
/* CoreComponent																				*/
/************************************************************************************************/
	bool isCanBeRebel() const;
	bool isCanRebelCapture() const;
	bool isCannotDefect() const;
	bool isCanQuellRebellion() const;
/************************************************************************************************/
/* REVOLUTION_MOD						  END												  */
/************************************************************************************************/

	float getUnitMaxSpeed() const; // Exposed to Python
	float getUnitPadTime() const; // Exposed to Python

// BUG - Unit Experience - start
	bool canAcquireExperience() const; // Exposed to Python
// BUG - Unit Experience - end

	// Arrays

	int getPrereqAndTechs(int i) const; // Exposed to Python
	int getPrereqOrBonuses(int i) const; // Exposed to Python
	int getFlavorValue(int i) const; // Exposed to Python
	int getTerrainAttackModifier(int i) const; // Exposed to Python
	int getTerrainDefenseModifier(int i) const; // Exposed to Python
	int getFeatureAttackModifier(int i) const; // Exposed to Python
	int getFeatureDefenseModifier(int i) const; // Exposed to Python
	int getUnitAttackModifier(int i) const; // Exposed to Python
	int getUnitDefenseModifier(int i) const; // Exposed to Python
	int getUnitCombatModifier(int i) const; // Exposed to Python
	int getUnitCombatCollateralImmune(int i) const; // Exposed to Python
	int getDomainModifier(int i) const; // Exposed to Python
	int getBonusProductionModifier(int i) const; // Exposed to Python
	int getUnitGroupRequired(int i) const; // Exposed to Python
	int getReligionSpreads(int i) const; // Exposed to Python
	int getCorporationSpreads(int i) const; // Exposed to Python
	int getTerrainPassableTech(int i) const; // Exposed to Python
	int getFeaturePassableTech(int i) const; // Exposed to Python
	int getFlankingStrikeUnit(int i) const; // Exposed to Python

	bool isPrereqOrCivics(int i) const; // Exposed to Python

	int CvUnitInfo::getPrereqAndBuilding(int i) const;
	int CvUnitInfo::getNumPrereqAndBuildings() const;
	bool CvUnitInfo::isPrereqAndBuilding(int i) const;

	int getTargetUnit(int i) const;
	int getNumTargetUnits() const;
	bool isTargetUnit(int i) const;		//Exposed to Python

	int getDefendAgainstUnit(int i) const;
	int getNumDefendAgainstUnits() const;
	bool isDefendAgainstUnit(int i) const;		//Exposed to Python

	int getSupersedingUnit(int i) const;
	int getNumSupersedingUnits() const;
	bool isSupersedingUnit(int i) const;		//Exposed to Python

	int getUnitUpgrade(int i) const;			//Exposed to Python
	int getNumUnitUpgrades() const;				//Exposed to Python
	bool isUnitUpgrade(int i) const;			//Exposed to Python

	std::vector<int> getUnitUpgradeChain() const;
	void CvUnitInfo::addUnitToUpgradeChain(int i);

	bool getTargetUnitCombat(int i) const; // Exposed to Python
	bool getDefenderUnitCombat(int i) const; // Exposed to Python
	bool getUnitAIType(int i) const; // Exposed to Python
	bool getNotUnitAIType(int i) const; // Exposed to Python
	bool getBuilds(int i) const; // Exposed to Python
	bool getGreatPeoples(int i) const; // Exposed to Python

	int getBuildings(int i) const; // Exposed to Python
	bool getHasBuilding(int i) const; // Exposed to Python
	int getNumBuildings() const;

	//bool getTerrainImpassable(int i) const; // Exposed to Python
	//bool getFeatureImpassable(int i) const; // Exposed to Python
	bool getTerrainNative(int i) const; // Exposed to Python
	bool getFeatureNative(int i) const; // Exposed to Python
	bool getFreePromotions(int i) const; // Exposed to Python
	int getLeaderPromotion() const; // Exposed to Python
	int getLeaderExperience() const; // Exposed to Python

	CvOutcomeList* getKillOutcomeList();
	int getNumActionOutcomes() const;
	CvOutcomeList* getActionOutcomeList(const int index) const;
	MissionTypes getActionOutcomeMission(const int index) const;
	CvOutcomeList* getActionOutcomeListByMission(const MissionTypes eMission) const;
	CvOutcomeMission* getOutcomeMission(const int index) const;
	CvOutcomeMission* getOutcomeMissionByMission(const MissionTypes eMission) const;

	const TCHAR* getEarlyArtDefineTag(int i, UnitArtStyleTypes eStyle) const; // Exposed to Python
	void setEarlyArtDefineTag(int i, const TCHAR* szVal);
	const TCHAR* getLateArtDefineTag(int i, UnitArtStyleTypes eStyle) const; // Exposed to Python
	void setLateArtDefineTag(int i, const TCHAR* szVal);
	const TCHAR* getMiddleArtDefineTag(int i, UnitArtStyleTypes eStyle) const; // Exposed to Python
	void setMiddleArtDefineTag(int i, const TCHAR* szVal);
	const TCHAR* getUnitNames(int i) const;
	const TCHAR* getFormationType() const;

	const TCHAR* getButton() const;
	void updateArtDefineButton();

	const CvArtInfoUnit* getArtInfo(int i, EraTypes eEra, UnitArtStyleTypes eStyle) const;

	void read(FDataStreamBase* ) {}
	void write(FDataStreamBase* ) {}

	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);
	bool readPass3();
	void copyNonDefaults(CvUnitInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void copyNonDefaultsReadPass2(CvUnitInfo* pClassInfo = NULL , CvXMLLoadUtility* pXML = NULL, bool bOver = false);

	std::vector<int>& getSubCombatTypes();
	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iAIWeight;
	int m_iProductionCost;
	int m_iHurryCostModifier;
	int m_iAdvancedStartCost;
	int m_iAdvancedStartCostIncrease;
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
	int m_iExtraCost;
	int m_iAssetValue;
	int m_iPowerValue;
	int m_iSpecialUnitType;
	int m_iUnitCaptureType;
	int m_iUnitCombatType;
	int m_iDomainType;
	int m_iDefaultUnitAIType;
	int m_iInvisibleType;
	int m_iAdvisorType;
	int m_iMaxStartEra;
	int m_iForceObsoleteTech;
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
	bool m_bPrereqBonuses;
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
	bool m_bCanBeRebel;
	bool m_bCanRebelCapture;
	bool m_bCannotDefect;
	bool m_bCanQuellRebellion;

	//ls612: Can't enter non-Owned cities
	bool m_bNoNonOwnedEntry;

	int m_iLeaderPromotion;

	float m_fUnitMaxSpeed;
	float m_fUnitPadTime;

	// Arrays

	int* m_piPrereqAndTechs;
	int* m_piPrereqOrBonuses;
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
	bool* m_pbBuilds;
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

	CvPropertyManipulators m_PropertyManipulators;

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
	int m_iStrAdjperRnd;
	int m_iStrAdjperAtt;
	int m_iStrAdjperDef;
	int m_iWithdrawAdjperAtt;
	int m_iUnnerve;
	int m_iEnclose;
	int m_iLunge;
	int m_iDynamicDefense;
	int m_iFortitude;
	int m_iFrontSupportPercent;
	int m_iShortRangeSupportPercent;
	int m_iMediumRangeSupportPercent;
	int m_iLongRangeSupportPercent;
	int m_iFlankSupportPercent;
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
	int m_iCostModifier;
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
	//boolean vectors without delayed resolution
	std::vector<int> m_aiSubCombatTypes;
	std::vector<int> m_aiCureAfflictionTypes;
	std::vector<int> m_aiHealAsTypes;
	std::vector<int> m_aiTerrainImpassableTypes;
	std::vector<int> m_aiFeatureImpassableTypes;
	std::vector<int> m_aiMapCategoryTypes;
	std::vector<int> m_aiTrapSetWithPromotionTypes;
	std::vector<int> m_aiTrapImmunityUnitCombatTypes;
	// int vectors utilizing struct with delayed resolution
	std::vector<PromotionLineModifier> m_aAfflictionFortitudeModifiers;
	std::vector<AfflictOnAttack> m_aAfflictOnAttackTypes;
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
	std::vector<AfflictionLineChanges> m_aDistanceAttackCommunicabilityTypeChanges;
	std::vector<EnabledCivilizations> m_aEnabledCivilizationTypes;
	// int vectors utilizing pairing without delayed resolution
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
	//Team Project (4)
		//WorkRateMod
	TerrainModifierArray m_aTerrainWorkRateModifierTypes;
	FeatureModifierArray m_aFeatureWorkRateModifierTypes;
	BuildModifierArray m_aBuildWorkRateModifierTypes;
	AidArray m_aAidChanges;
	//TB Combat Mods End  TB SubCombat Mod end
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

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	virtual bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvSpawnInfo* pClassInfo = NULL , CvXMLLoadUtility* pXML = NULL);

	int	getNumBonuses() const;
	int	getNumTerrains() const;
	int	getNumFeatures() const;
	int	getNumFeatureTerrains() const;
	int	getNumSpawnGroup() const;
	PlayerTypes getPlayer() const;
	BonusTypes getBonus(const int index) const;
	TerrainTypes getTerrain(const int index) const;
	FeatureTypes getFeature(const int index) const;
	TerrainTypes getFeatureTerrain(const int index) const;
	UnitTypes getSpawnGroup(const int index) const;
	int getTurnRate() const;
	int getGlobalTurnRate() const;
	int getMaxLocalDensity() const;
	int getMaxAreaTotalDensity() const;
	int getMaxAreaUnitDensity() const;
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
	TechTypes getPrereqTechType() const;
	TechTypes getObsoleteTechType() const;

	BoolExpr* getSpawnCondition() const;

	void getCheckSum(unsigned int& iSum);

private:
	UnitTypes				m_eUnitType;
	TechTypes				m_ePrereqTechType;
	TechTypes				m_eObsoleteTechType;
	int						m_iPlayerType;
	int						m_iTurns;
	int						m_iGlobalTurns;
	int						m_iMaxLocalDensity;
	int						m_iMaxAreaTotalDensity;
	int						m_iMaxAreaUnitDensity;
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

	BoolExpr*				m_pExprSpawnCondition;
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

	DllExport const TCHAR* getFormationType() const;
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
	void copyNonDefaults(CvUnitFormationInfo* pClassInfo, CvXMLLoadUtility* pXML);

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

	bool isCarrierUnitAIType(int i) const; // Exposed to Python

	int getCombatPercent() const;
	int getWithdrawalChange() const;
	int getPursuitChange(bool bForLoad = false) const;

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvSpecialUnitInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

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

//	bool getTraitNoUpkeep(int i) const; // Exposed to Python
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

	std::wstring pyGetWeLoveTheKing() const { return getWeLoveTheKing(); } // Exposed to Python
	const wchar* getWeLoveTheKing() const;
	void setWeLoveTheKingKey(const TCHAR* szVal);
	const wchar* getWeLoveTheKingKey() const;

	int getCivicOptionType() const; // Exposed to Python
	int getAnarchyLength() const; // Exposed to Python
	int getUpkeep() const; // Exposed to Python
	int getAIWeight() const; // Exposed to Python
	int getGreatPeopleRateModifier() const; // Exposed to Python
	int getGreatGeneralRateModifier() const; // Exposed to Python
	int getDomesticGreatGeneralRateModifier() const; // Exposed to Python
	int getStateReligionGreatPeopleRateModifier() const; // Exposed to Python
	int getDistanceMaintenanceModifier() const; // Exposed to Python
	int getNumCitiesMaintenanceModifier() const; // Exposed to Python
	int getHomeAreaMaintenanceModifier() const;
	int getOtherAreaMaintenanceModifier() const;
	int getCorporationMaintenanceModifier() const; // Exposed to Python
	int getExtraHealth() const; // Exposed to Python
	int getFreeExperience() const; // Exposed to Python
	int getWorkerSpeedModifier() const; // Exposed to Python
	int getImprovementUpgradeRateModifier() const; // Exposed to Python
	int getMilitaryProductionModifier() const; // Exposed to Python
	int getBaseFreeUnits() const; // Exposed to Python
	int getBaseFreeMilitaryUnits() const; // Exposed to Python
	int getFreeUnitsPopulationPercent() const; // Exposed to Python
	int getFreeMilitaryUnitsPopulationPercent() const; // Exposed to Python
	int getGoldPerUnit() const; // Exposed to Python
	int getGoldPerMilitaryUnit() const; // Exposed to Python
	int getHappyPerMilitaryUnit() const; // Exposed to Python
	int getLargestCityHappiness() const; // Exposed to Python
	int getWarWearinessModifier() const; // Exposed to Python
	int getFreeSpecialist() const; // Exposed to Python
	int getTradeRoutes() const; // Exposed to Python
	int getTechPrereq() const; // Exposed to Python
	int getCivicPercentAnger() const; // Exposed to Python
	int getMaxConscript() const; // Exposed to Python
	int getStateReligionHappiness() const; // Exposed to Python
	int getNonStateReligionHappiness() const; // Exposed to Python
	int getStateReligionUnitProductionModifier() const; // Exposed to Python
	int getStateReligionBuildingProductionModifier() const; // Exposed to Python
	int getStateReligionFreeExperience() const; // Exposed to Python
	int getExpInBorderModifier() const; // Exposed to Python
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
	bool isMilitaryFoodProduction() const; // Exposed to Python
	bool isNoUnhealthyPopulation() const; // Exposed to Python
	bool isBuildingOnlyHealthy() const; // Exposed to Python
	bool isNoForeignTrade() const; // Exposed to Python
	bool isNoCorporations() const; // Exposed to Python
	bool isNoForeignCorporations() const; // Exposed to Python
	bool isStateReligion() const; // Exposed to Python
	bool isNoNonStateReligionSpread() const; // Exposed to Python
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

	int getYieldModifier(int i) const; // Exposed to Python
	int* getYieldModifierArray() const;
	int getCapitalYieldModifier(int i) const; // Exposed to Python
	int* getCapitalYieldModifierArray() const;
	int getTradeYieldModifier(int i) const; // Exposed to Python
	int* getTradeYieldModifierArray() const;
	int getCommerceModifier(int i) const; // Exposed to Python
	int* getCommerceModifierArray() const;
	int getCapitalCommerceModifier(int i) const; // Exposed to Python
	int* getCapitalCommerceModifierArray() const;
	int getSpecialistExtraCommerce(int i) const; // Exposed to Python
	int* getSpecialistExtraCommerceArray() const;
	int getCivicAttitudeChange(int i) const;
	int* getCivicAttitudeChanges() const;
	int getLandmarkYieldChanges(int i) const;
	int* getLandmarkYieldChangesArray() const;

	int getBonusCommerceModifier(int i, int j) const;
	int* getBonusCommerceModifierArray(int i) const;

	int getBuildingHappinessChanges(int i) const; // Exposed to Python
	int getBuildingHealthChanges(int i) const; // Exposed to Python
	int getFeatureHappinessChanges(int i) const;
	int getBonusMintedPercent(int i) const;
	int getFreeSpecialistCount(int i) const; // Exposed to Python
	int getFlavorValue(int i) const;
	int getUnitCombatProductionModifier(int i) const;
	int getBuildingProductionModifier(int i) const;
	int getUnitProductionModifier(int i) const;
	int getImprovementHappinessChanges(int i) const;
	int getImprovementHealthPercentChanges(int i) const;

	bool isHurry(int i) const; // Exposed to Python
	bool isSpecialBuildingNotRequired(int i) const; // Exposed to Python
	bool isSpecialistValid(int i) const; // Exposed to Python

	int getImprovementYieldChanges(int i, int j) const; // Exposed to Python
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
	bool isAnySpecialistCommerceChanges() const;
	bool isAnySpecialistYieldChanges() const;

	CvString getCivicAttitudeReason(const int i) const;
	int getCivicAttitudeVectorSize() const;
	CvString getCivicAttitudeNamesVectorElement(const int i) const;
	int getCivicAttitudeValuesVectorElement(const int i) const;
	int getCivicAttitudeReasonVectorSize() const;
	CvString getCivicAttitudeReasonNamesVectorElement(const int i) const;
	CvString getCivicAttitudeReasonValuesVectorElement(const int i) const;

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
	int* m_paiBuildingProductionModifier;
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

	std::vector<CvString> m_aszUnitProdModforPass3;
	std::vector<int> m_aiUnitProdModforPass3;

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

	const CvDiplomacyResponse& getResponse(int iNum) const; // Exposed to Python
	int getNumResponses() const; // Exposed to Python

	bool getCivilizationTypes(int i, int j) const; // Exposed to Python
	bool getLeaderHeadTypes(int i, int j) const; // Exposed to Python
	bool getAttitudeTypes(int i, int j) const; // Exposed to Python
	bool getDiplomacyPowerTypes(int i, int j) const; // Exposed to Python

	int getNumDiplomacyText(int i) const; // Exposed to Python

	const TCHAR* getDiplomacyText(int i, int j) const; // Exposed to Python

	void read(FDataStreamBase* stream){}
	void write(FDataStreamBase* stream){}
	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvXMLLoadUtility* pXML);
	bool FindResponseIndex(const CvDiplomacyResponse* pNewResponse, int iCase, int* iIndex) const;

private:
	std::vector<CvDiplomacyResponse*> m_pResponses;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvBuildingInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoBuilding;
class CvArtInfoMovie;
class CvBuildingInfo : public CvHotkeyInfo
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvBuildingInfo();
	virtual ~CvBuildingInfo();

	int getMaxGlobalInstances() const   { return m_iMaxGlobalInstances; } // Exposed to Python
	int getMaxTeamInstances() const     { return m_iMaxTeamInstances; } // Exposed to Python
	int getMaxPlayerInstances() const   { return m_iMaxPlayerInstances; } // Exposed to Python
	int getExtraPlayerInstances() const { return m_iExtraPlayerInstances; } // Exposed to Python
	bool isNoLimit() const; // Exposed to Python

	int getVictoryPrereq() const; // Exposed to Python
	int getFreeStartEra() const; // Exposed to Python
	int getMaxStartEra() const; // Exposed to Python
	int getObsoleteTech() const; // Exposed to Python
	int getPrereqAndTech() const; // Exposed to Python
	int getNoBonus() const; // Exposed to Python
	int getPowerBonus() const; // Exposed to Python
	int getFreeBonus() const; // Exposed to Python
	int getNumFreeBonuses() const; // Exposed to Python
	int getNumExtraFreeBonuses() const;
	BonusTypes getExtraFreeBonus(int i) const;
	int getExtraFreeBonusNum(int i) const;
	bool hasExtraFreeBonus(BonusTypes eBonus) const;
	int getFreeBuilding() const; // Exposed to Python
	int getFreeAreaBuilding() const;
	int getFreePromotion() const; // Exposed to Python
	int getCivicOption() const; // Exposed to Python
	int getAIWeight() const; // Exposed to Python
	int getProductionCost() const; // Exposed to Python
	int getHurryCostModifier() const; // Exposed to Python
	int getHurryAngerModifier() const; // Exposed to Python
	int getAdvancedStartCost() const; // Exposed to Python
	int getAdvancedStartCostIncrease() const; // Exposed to Python
	int getMinAreaSize() const; // Exposed to Python
	int getNumCitiesPrereq() const; // Exposed to Python
	int getNumTeamsPrereq() const; // Exposed to Python
	int getUnitLevelPrereq() const; // Exposed to Python
	int getMinLatitude() const; // Exposed to Python
	int getMaxLatitude() const; // Exposed to Python
	int getGreatPeopleRateModifier() const; // Exposed to Python
	int getGreatGeneralRateModifier() const; // Exposed to Python
	int getDomesticGreatGeneralRateModifier() const; // Exposed to Python
	int getGlobalGreatPeopleRateModifier() const; // Exposed to Python
	int getAnarchyModifier() const; // Exposed to Python
	int getGoldenAgeModifier() const; // Exposed to Python
	int getGlobalHurryModifier() const; // Exposed to Python
	int getFreeExperience() const; // Exposed to Python
	int getGlobalFreeExperience() const; // Exposed to Python
	int getFoodKept() const; // Exposed to Python
	int getAirlift() const; // Exposed to Python
	int getAirModifier() const; // Exposed to Python
	int getAirUnitCapacity() const; // Exposed to Python
	int getNukeModifier() const; // Exposed to Python
	int getNukeExplosionRand() const; // Exposed to Python
	int getFreeSpecialist() const; // Exposed to Python
	int getAreaFreeSpecialist() const; // Exposed to Python
	int getGlobalFreeSpecialist() const; // Exposed to Python
	int getHappiness() const; // Exposed to Python
	int getAreaHappiness() const; // Exposed to Python
	int getGlobalHappiness() const; // Exposed to Python
	int getStateReligionHappiness() const; // Exposed to Python
	int getWorkerSpeedModifier() const; // Exposed to Python
	int getMilitaryProductionModifier() const; // Exposed to Python
	int getSpaceProductionModifier() const; // Exposed to Python
	int getGlobalSpaceProductionModifier() const; // Exposed to Python
	int getTradeRoutes() const; // Exposed to Python
	int getCoastalTradeRoutes() const; // Exposed to Python
	int getGlobalTradeRoutes() const; // Exposed to Python
	int getTradeRouteModifier() const; // Exposed to Python
	int getForeignTradeRouteModifier() const; // Exposed to Python
	int getAssetValue(bool bForLoad = false) const; // Exposed to Python
	int getPowerValue(bool bForLoad = false) const; // Exposed to Python
	int getSpecialBuildingType() const; // Exposed to Python
	int getAdvisorType() const; // Exposed to Python

	// REVDCM CanConstruct 2/16/10 by phungus420
	int getPrereqGameOption() const; // Exposed to Python
	int getNotGameOption() const; // Exposed to Python

	int getHolyCity() const; // Exposed to Python
	int getReligionType() const; // Exposed to Python
	int getStateReligion() const; // Exposed to Python
	int getPrereqReligion() const; // Exposed to Python
	int getPrereqCorporation() const; // Exposed to Python
	int getFoundsCorporation() const; // Exposed to Python
	int getGlobalReligionCommerce() const; // Exposed to Python
	int getGlobalCorporationCommerce() const; // Exposed to Python
	int getPrereqAndBonus() const; // Exposed to Python
	int getGreatPeopleUnitType() const; // Exposed to Python
	int getGreatPeopleRateChange() const; // Exposed to Python
	int getConquestProbability() const; // Exposed to Python
	int getMaintenanceModifier() const; // Exposed to Python
	//DPII < Maintenance Modifiers >
	int getGlobalMaintenanceModifier() const;
	int getAreaMaintenanceModifier() const;
	int getOtherAreaMaintenanceModifier() const;
	int getDistanceMaintenanceModifier() const;
	int getNumCitiesMaintenanceModifier() const;
	int getCoastalDistanceMaintenanceModifier() const;
	int getConnectedCityMaintenanceModifier() const;
	//DPII < Maintenance Modifiers >
	int getWarWearinessModifier() const; // Exposed to Python
	int getGlobalWarWearinessModifier() const; // Exposed to Python
	int getEnemyWarWearinessModifier() const; // Exposed to Python
	int getHealRateChange() const; // Exposed to Python
	int getHealth() const; // Exposed to Python
	int getAreaHealth() const; // Exposed to Python
	int getGlobalHealth() const; // Exposed to Python
	int getGlobalPopulationChange() const; // Exposed to Python
	int getFreeTechs() const; // Exposed to Python
	TechTypes getFreeSpecialTech() const;
	int getDefenseModifier() const; // Exposed to Python
	int getBombardDefenseModifier() const; // Exposed to Python
	int getAllCityDefenseModifier() const; // Exposed to Python
	int getEspionageDefenseModifier() const; // Exposed to Python

	// REVDCM Building Effects 4/09/10 by phungus420
	int getUnitUpgradePriceModifier() const; // Exposed to Python
	int getRevIdxLocal() const; // Exposed to Python
	int getRevIdxNational() const; // Exposed to Python
	int getRevIdxDistanceModifier() const; // Exposed to Python

	int getMissionType() const; // Exposed to Python
	void setMissionType(int iNewType);
	int getVoteSourceType() const; // Exposed to Python

	float getVisibilityPriority() const;

	bool isTeamShare() const; // Exposed to Python
	bool isAutoBuild() const;
	bool isWater() const; // Exposed to Python
	bool isRiver() const; // Exposed to Python
	bool isFreshWater() const; // Exposed to Python
	bool isPower() const; // Exposed to Python
	bool isDirtyPower() const; // Exposed to Python
	bool isAreaCleanPower() const; // Exposed to Python
	bool isOrbital() const;
	bool isOrbitalInfrastructure() const;
	bool isNoHolyCity() const;
	bool isAreaBorderObstacle() const; // Exposed to Python
	bool isForceTeamVoteEligible() const; // Exposed to Python
	bool isCapital() const; // Exposed to Python
	// DCM 04/19/09 Johny Smith
	// Dale - AB: Bombing START
	int getDCMAirbombMission() const;
protected:
public:
	// Dale - AB: Bombing END
	bool isGovernmentCenter() const; // Exposed to Python
	bool isGoldenAge() const; // Exposed to Python
	bool isMapCentering() const; // Exposed to Python
	bool isNoUnhappiness() const; // Exposed to Python
	bool isNoUnhealthyPopulation() const; // Exposed to Python
	bool isBuildingOnlyHealthy() const; // Exposed to Python
	bool isNeverCapture() const; // Exposed to Python
	bool isNukeImmune() const; // Exposed to Python
	bool isCenterInCity() const; // Exposed to Python
	bool isStateReligion() const; // Exposed to Python
	bool isAllowsNukes() const; // Exposed to Python

	const TCHAR* getConstructSound() const; // Exposed to Python
	void setConstructSound(const TCHAR* szVal);
	const TCHAR* getArtDefineTag() const; // Exposed to Python
	void setArtDefineTag(const TCHAR* szVal);
	const TCHAR* getMovieDefineTag() const; // Exposed to Python
	void setMovieDefineTag(const TCHAR* szVal);

	// Arrays

	int getYieldChange(int i) const; // Exposed to Python
	int* getYieldChangeArray() const;
	int getYieldPerPopChange(int i) const; // Exposed to Python
	int* getYieldPerPopChangeArray() const;
	int getYieldModifier(int i) const; // Exposed to Python
	int* getYieldModifierArray() const;
	int getPowerYieldModifier(int i) const; // Exposed to Python
	int* getPowerYieldModifierArray() const;
	int getAreaYieldModifier(int i) const; // Exposed to Python
	int* getAreaYieldModifierArray() const;
	int getGlobalYieldModifier(int i) const; // Exposed to Python
	int* getGlobalYieldModifierArray() const;
	int getSeaPlotYieldChange(int i) const; // Exposed to Python
	int* getSeaPlotYieldChangeArray() const;
	int getRiverPlotYieldChange(int i) const; // Exposed to Python
	int* getRiverPlotYieldChangeArray() const;
	int getGlobalSeaPlotYieldChange(int i) const; // Exposed to Python
	int* getGlobalSeaPlotYieldChangeArray() const;

	int getCommerceChange(int i) const; // Exposed to Python
	int* getCommerceChangeArray() const;
	int getCommercePerPopChange(int i) const;
	int* getCommercePerPopChangeArray() const;
	int getObsoleteSafeCommerceChange(int i) const; // Exposed to Python
	int* getObsoleteSafeCommerceChangeArray() const;
	int getCommerceChangeDoubleTime(int i) const; // Exposed to Python
	int getCommerceModifier(int i) const; // Exposed to Python
	int* getCommerceModifierArray() const;
	int getGlobalCommerceModifier(int i) const; // Exposed to Python
	int* getGlobalCommerceModifierArray() const;
	int getSpecialistExtraCommerce(int i) const; // Exposed to Python
	int* getSpecialistExtraCommerceArray() const;
	int getStateReligionCommerce(int i) const; // Exposed to Python
	int* getStateReligionCommerceArray() const;
	int getCommerceHappiness(int i) const; // Exposed to Python
	int getReligionChange(int i) const; // Exposed to Python
	int getSpecialistCount(int i) const; // Exposed to Python
	int getFreeSpecialistCount(int i) const; // Exposed to Python
	int getBonusHealthChanges(int i) const; // Exposed to Python
	int getBonusHappinessChanges(int i) const; // Exposed to Python
	int getBonusProductionModifier(int i) const; // Exposed to Python
	int getUnitCombatFreeExperience(int i) const; // Exposed to Python
	int getDomainFreeExperience(int i) const; // Exposed to Python
	bool isAnyUnitCombatFreeExperience() const;
	bool isAnyDomainFreeExperience() const;
	int getDomainProductionModifier(int i) const; // Exposed to Python
	int getPrereqAndTechs(int i) const; // Exposed to Python
	int getBuildingHappinessChanges(int i) const; // Exposed to Python
	int getPrereqNumOfBuilding(int i) const; // Exposed to Python

	int getFlavorValue(int i) const; // Exposed to Python
	int getImprovementFreeSpecialist(int i) const; // Exposed to Python

	bool isCommerceFlexible(int i) const; // Exposed to Python
	bool isCommerceChangeOriginalOwner(int i) const; // Exposed to Python

	int getPrereqInCityBuilding(int i) const;
	int getNumPrereqInCityBuildings() const;
	bool isPrereqInCityBuilding(int i) const; // Exposed to Python


	int getSpecialistYieldChange(int i, int j) const; // Exposed to Python
	int* getSpecialistYieldChangeArray(int i) const;

	int getSpecialistCommerceChange(int i, int j) const; // Exposed to Python
	int* getSpecialistCommerceChangeArray(int i) const;

	int getBonusYieldModifier(int i, int j) const; // Exposed to Python
	int* getBonusYieldModifierArray(int i) const;

	int getGlobalBuildingCommerceChange(int iBuilding, int iCommerce) const;
	int getNumGlobalBuildingCommerceChanges() const;

	// Other

	const CvArtInfoBuilding* getArtInfo() const;
	const CvArtInfoMovie* getMovieInfo() const;
	const TCHAR* getButton() const;
	const TCHAR* getMovie() const;

	// serialization
	void read(FDataStreamBase*) {}
	void write(FDataStreamBase*) {}
	bool read(CvXMLLoadUtility* pXML);

	// Afforess 12/9/09
	int getFreePromotion_2() const;
	int getFreePromotion_3() const;
	int getPrereqVicinityBonus() const;
	int getPrereqRawVicinityBonus() const;
	int getGlobalPopulationgrowthratepercentage() const;
	int getPopulationgrowthratepercentage() const;
	bool isBuildOnlyOnPeaks() const;
	bool isPrereqPower() const;
	bool isApplyFreePromotionOnMove() const;
	bool isNoEnemyPillagingIncome() const;
	bool isProvidesFreshWater() const;
	bool isForceAllTradeRoutes() const;
	bool isForceNoPrereqScaling() const;
	bool isPrereqWar() const;
	bool isRequiresActiveCivics() const;
	bool isZoneOfControl() const;
	bool isProtectedCulture() const;
	int getPillageGoldModifier() const;
	int getWorldTradeRoutes() const;
	int getLineOfSight() const;
	int getInflationModifier() const;
	int getInvasionChance() const;
	int getAdjacentDamagePercent() const;
	int getPrereqPopulation() const;
	int getProductionContinueBuilding() const;
	int getPrereqCultureLevel() const;
	int getWorkableRadius() const;
	int getPrereqAnyoneBuilding() const;
	int getExtendsBuilding() const;
	int getOccupationTimeModifier() const;
	int getNoEntryDefenseLevel(bool bForLoad = false) const;
	int getNumUnitFullHeal() const;
	int getNumPopulationEmployed() const;
	int getHappinessPercentPerPopulation() const;
	int getHealthPercentPerPopulation() const;

	int getUnitProductionModifier(int i) const;
	int getPrereqOrVicinityBonuses(int i) const;
	int getPrereqOrRawVicinityBonuses(int i) const;

	bool isPrereqOrBuilding(int i) const;

	bool isPrereqOrGameSpeed(int i) const;

	bool isPrereqOrCivics(int iCivic) const;
	bool isPrereqAndCivics(int iCivic) const;

	bool isPrereqOrTerrain(int i) const;		//Exposed to Python
	bool isPrereqAndTerrain(int i) const;		//Exposed to Python
	bool isPrereqOrImprovement(int i) const;	//Exposed to Python
	bool isPrereqOrFeature(int i) const;		//Exposed to Python
	bool isPrereqNotBuilding(int i) const;
	int  getBuildingProductionModifier(int i) const;
	int  getGlobalBuildingProductionModifier(int i) const;
	int  getGlobalBuildingCostModifier(int i) const;

	int  getBonusDefenseChanges(int i) const;

	std::vector<CvString> m_aszPrereqOrCivicsforPass3;
	std::vector<bool> m_abPrereqOrCivicsforPass3;

	int isPrereqOrCivicsVectorSize() const;
	CvString isPrereqOrCivicsNamesVectorElement(const int i) const;
	int isPrereqOrCivicsValuesVectorElement(const int i) const;

	int isPrereqAndCivicsVectorSize() const;
	CvString isPrereqAndCivicsNamesVectorElement(const int i) const;
	int isPrereqAndCivicsValuesVectorElement(const int i) const;

	std::vector<CvString> m_aszPrereqAndCivicsforPass3;
	std::vector<bool> m_abPrereqAndCivicsforPass3;

	std::vector<CvString> m_aszUnitProdModforPass3;
	std::vector<int> m_aiUnitProdModforPass3;

	bool isReplaceBuilding(int i) const;

	int getImprovementYieldChanges(int i, int j) const;

	int getBonusCommerceModifier(int i, int j) const;
	int* getBonusCommerceModifierArray(int i) const;

	int getBonusYieldChanges(int i, int j) const;
	int* getBonusYieldChangesArray(int i) const;

	int getVicinityBonusYieldChanges(int i, int j) const;
	int* getVicinityBonusYieldChangesArray(int i) const;

	int getBonusCommercePercentChanges(int i, int j) const;
	int* getBonusCommercePercentChangesArray(int i) const;

	int getTechHappinessChanges(int i) const;
	int* m_piTechHappinessChanges;

	int getTechHealthChanges(int i) const;		//Exposed to Python
	int* m_piTechHealthChanges;

	int getTechCommerceChange(int i, int j) const;
	int* getTechCommerceChangeArray(int i) const;

	int getTechYieldChange(int i, int j) const;
	int* getTechYieldChangeArray(int i) const;

	int getTechSpecialistChange(int i, int j) const;
	int* getTechSpecialistChangeArray(int i) const;

	int getTechCommerceModifier(int i, int j) const;
	int* getTechCommerceModifierArray(int i) const;

	int getTechYieldModifier(int i, int j) const;
	int* getTechYieldModifierArray(int i) const;

	int getUnitCombatExtraStrength(int i) const;

	int getCommerceAttacks(int i) const;
	int* getCommerceAttacksArray() const;

	CvProperties* getProperties();
	CvProperties* getPropertiesAllCities();
	CvProperties* getPrereqMinProperties();
	CvProperties* getPrereqMaxProperties();
	CvProperties* getPrereqPlayerMinProperties();
	CvProperties* getPrereqPlayerMaxProperties();

	//TB Combat Mods (Buildings) begin
	UnitTypes getPropertySpawnUnit() const;
	PropertyTypes getPropertySpawnProperty() const;
	PromotionLineTypes getPromotionLineType() const;
	//TechTypes getFreeSpecialTech() const;
	//integers
	int getLinePriority() const;
	int getOutbreakBase() const;
	int getOvercomeBase() const;
	int getTradeCommunicability() const;
	int getFrontSupportPercentModifier() const;
	int getShortRangeSupportPercentModifier() const;
	int getMediumRangeSupportPercentModifier() const;
	int getLongRangeSupportPercentModifier() const;
	int getFlankSupportPercentModifier() const;
	//Team Project (3)
	int getNationalCaptureProbabilityModifier() const;
	int getNationalCaptureResistanceModifier() const;
	int getLocalCaptureProbabilityModifier() const;
	int getLocalCaptureResistanceModifier() const;
	int getLocalDynamicDefense() const;
	int getRiverDefensePenalty() const;
	int getLocalRepel(bool bForLoad = false) const;
	int getMinDefense(bool bForLoad = false) const;
	int getBuildingDefenseRecoverySpeedModifier() const;
	int getCityDefenseRecoverySpeedModifier() const;
	int getDamageAttackerChance() const;
	int getDamageToAttacker() const;
	int getMaxPopulationAllowed(bool bForLoad = false) const;
	int getMaxPopulationChange(bool bForLoad = false) const;
	int getInsidiousness() const;
	int getInvestigation() const;
	int getPopulationChange() const;
	int getOneTimePopulationPercentLoss() const;
	//Booleans
	bool isDamageToAttackerIgnoresArmor() const;
	bool isDamageAllAttackers() const;
	bool isDamageAttackerCapable() const;
	bool isQuarantine() const;

	//boolean vectors without delayed resolution
	//int getFreePromoType(int i) const;
	//int getNumFreePromoTypes() const;
	//bool isFreePromoType(int i);

	int getUnitCombatRetrainType(int i) const;
	int getNumUnitCombatRetrainTypes() const;
	bool isUnitCombatRetrainType(int i) const;

	int getMayDamageAttackingUnitCombatType(int i) const;
	int getNumMayDamageAttackingUnitCombatTypes() const;
	bool isMayDamageAttackingUnitCombatType(int i) const;

	int getMapCategoryType(int i) const;
	int getNumMapCategoryTypes() const;
	bool isMapCategoryType(int i);

	//integer vectors with pairing method without delayed resolution
	int getNumUnitCombatRepelModifiers() const;
	int getUnitCombatRepelModifier(int iUnitCombat, bool bForLoad = false) const;

	int getNumUnitCombatRepelAgainstModifiers() const;
	int getUnitCombatRepelAgainstModifier(int iUnitCombat, bool bForLoad = false) const;

	int getNumUnitCombatDefenseAgainstModifiers() const;
	int getUnitCombatDefenseAgainstModifier(int iUnitCombat) const;

	int getNumUnitCombatProdModifiers() const;
	int getUnitCombatProdModifier(int iUnitCombat) const;

	int getNumUnitCombatOngoingTrainingDurations() const;
	int getUnitCombatOngoingTrainingDuration(int iUnitCombat, bool bForLoad = false) const;

	int getNumAfflictionOutbreakLevelChanges() const;
	int getAfflictionOutbreakLevelChange(int iAfflictionLine) const;

	int getNumTechOutbreakLevelChanges() const;
	int getTechOutbreakLevelChange(int iTech) const;

	//Team Project (1)
	int getNumTechHappinessTypes() const;
	int getTechHappinessType(int iTech) const;
	const TechModifierArray& getTechHappinessTypeArray() const { return m_aTechHappinessTypes; }

	int getNumTechHealthTypes() const;
	int getTechHealthType(int iTech) const;
	const TechModifierArray& getTechHealthTypeArray() const { return m_aTechHealthTypes; }

	//Arrays
	int getLocalSpecialistYieldChange(int i, int j) const;
	int* getLocalSpecialistYieldChangeArray(int i) const;

	int getLocalSpecialistCommerceChange(int i, int j) const;
	int* getLocalSpecialistCommerceChangeArray(int i) const;

	bool isAnySpecialistCommerceChanges() const;
	bool isAnyLocalSpecialistCommerceChanges() const;
	bool isHurry(int i) const;

	//ls612 Begin:
	int getMaxPopAllowed() const;
	int getMaxCultureLevelAllowed() const;
	//ls612 End:

	//BoolExpr* getFreePromotionCondition();
	//Struct
	int getNumFreePromoTypes() const;
	//FreePromoTypes& getFreePromoType(int iPromotion);
	const FreePromoTypes& getFreePromoType(int iPromotion) const;

	int getNumFreeTraitTypes() const;
	int getFreeTraitType(int iIndex) const;

	int getNumHealUnitCombatTypes() const;
	const HealUnitCombat& getHealUnitCombatType(int iUnitCombat) const;

	int getNumBonusAidModifiers() const;
	const BonusAidModifiers& getBonusAidModifier(int iIndex) const;

	int getNumAidRateChanges() const;
	const AidRateChanges& getAidRateChange(int iIndex) const;

	//Bool vector utilizing delayed resolution
	int getNumEnabledCivilizationTypes() const;
	const EnabledCivilizations& getEnabledCivilizationType(int iIndex) const;
	//TB Combat Mods (Buildings) end

	bool EnablesOtherBuildings() const;
	bool isFreeBonusOfBuilding(BonusTypes eBonus) const;
	bool EnablesUnits() const;

	bool readPass3();

	void getCheckSum(unsigned int& iSum);

	const CvPropertyManipulators* getPropertyManipulators() const;
	CvPropertyManipulators* getPropertyManipulators();

	bool isNewCityFree(CvGameObject* pObject);

	BoolExpr* getConstructCondition() const;

	int getVictoryThreshold(int i) const;

	bool getNotShowInCity() const;

	bool m_bNoLimit;
	bool m_bNotShowInCity;
	bool m_bAnySpecialistYieldChanges;
	bool m_bAnySpecialistCommerceChanges;
	bool m_bAnyBonusYieldModifiers;
	bool m_bAnyTechCommerceChanges;
	bool m_bAnyTechYieldChanges;
	bool m_bAnyTechSpecialistChanges;
	bool m_bAnyTechCommerceModifiers;
	bool m_bAnyTechYieldModifiers;
	bool m_bAnyBonusCommerceModifiers;
	bool m_bAnyBonusYieldChanges;
	bool m_bAnyVicinityBonusYieldChanges;
	bool m_bAnyBonusCommercePercentChanges;

protected:
	void setNotShowInCity();

	int m_iFreePromotion_2;
	int m_iFreePromotion_3;
	int m_iPrereqVicinityBonus;
	int m_iPrereqRawVicinityBonus;
	int m_iGlobalPopulationgrowthratepercentage;
	int m_iPopulationgrowthratepercentage;
	int m_iWorldTradeRoutes;
	int m_iLineOfSight;
	int m_iInflationModifier;
	int m_iPillageGoldModifier;
	int m_iInvasionChance;
	int m_iAdjacentDamagePercent;
	int m_iPrereqPopulation;
	int m_iPrereqCultureLevel;
	int m_iWorkableRadius;
	int m_iPrereqAnyoneBuilding;
	int m_iExtendsBuilding;
	int m_iOccupationTimeModifier;
	int m_iNoEntryDefenseLevel;
	int m_iNumUnitFullHeal;
	int m_iProductionContinueBuilding;
	int m_iNumPopulationEmployed;
	int m_iHappinessPercentPerPopulation;
	int m_iHealthPercentPerPopulation;
	bool m_bBuildOnlyOnPeaks;
	bool m_bPrereqPower;
	bool m_bApplyFreePromotionOnMove;
	bool m_bNoEnemyPillagingIncome;
	bool m_bProvidesFreshWater;
	bool m_bForceAllTradeRoutes;
	bool m_bForceNoPrereqScaling;
	bool m_bPrereqWar;
	bool m_bRequiresActiveCivics;
	bool m_bZoneOfControl;
	bool m_bProtectedCulture;

	int* m_piPrereqOrVicinityBonuses;
	int* m_piPrereqOrRawVicinityBonuses;
	int* m_piUnitProductionModifier;
	bool* m_pbPrereqOrCivics;
	bool* m_pbPrereqAndCivics;

	int* m_piCommerceAttacks;

	bool* m_pbPrereqOrGameSpeed;
	bool* m_pbPrereqOrTerrain;
	bool* m_pbPrereqAndTerrain;
	bool* m_pbPrereqOrImprovement;
	bool* m_pbPrereqOrFeature;
	int* m_piBonusDefenseChanges;
	int** m_ppaiBonusCommerceModifier;
	int* m_piUnitCombatExtraStrength;
	int** m_ppaiTechCommerceChange;
	int** m_ppaiTechYieldChange;
	int** m_ppaiTechSpecialistChange;
	int** m_ppaiTechCommerceModifier;
	int** m_ppaiTechYieldModifier;
	int** m_ppiImprovementYieldChanges;
	int** m_ppaiBonusYieldChanges;
	int** m_ppaiBonusCommercePercentChanges;
	int** m_ppaiVicinityBonusYieldChanges;

private:
	int* m_piBuildingProductionModifier;
	int* m_piGlobalBuildingProductionModifier;
	int* m_piGlobalBuildingCostModifier;
	int* m_piBuildingHappinessChanges;
	int* m_piPrereqNumOfBuilding;
	bool* m_pbPrereqNotBuilding;
	bool* m_pbReplaceBuilding;
	bool* m_pbPrereqOrBuilding;

public:
	bool readPass2(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvBuildingInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void copyNonDefaultsReadPass2(CvBuildingInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL, bool bOver = false);
	//Alberts2 PrereqBonuses
	int getNumPrereqOrBonuses() const; // Exposed to Python
	BonusTypes getPrereqOrBonuses(int i) const; // Exposed to Python

	//---------------------------------------PUBLIC MEMBER VARIABLES---------------------------------
protected:
	int m_iVictoryPrereq;
	int m_iFreeStartEra;
	int m_iMaxStartEra;
	int m_iObsoleteTech;
	int m_iPrereqAndTech;
	int m_iNoBonus;
	int m_iPowerBonus;
	int m_iFreeBonus;
	int m_iNumFreeBonuses;
	std::vector<std::pair<BonusTypes,int> > m_aExtraFreeBonuses;

	int m_iFreeBuilding;
	int m_iFreeAreaBuilding;
	int m_iFreePromotion;
	int m_iCivicOption;
	int m_iAIWeight;
	int m_iProductionCost;
	int m_iHurryCostModifier;
	int m_iHurryAngerModifier;
	int m_iAdvancedStartCost;
	int m_iAdvancedStartCostIncrease;
	int m_iMinAreaSize;
	int m_iNumCitiesPrereq;
	int m_iNumTeamsPrereq;
	int m_iUnitLevelPrereq;
	int m_iMinLatitude;
	int m_iMaxLatitude;
	int m_iGreatPeopleRateModifier;
	int m_iGreatGeneralRateModifier;
	int m_iDomesticGreatGeneralRateModifier;
	int m_iGlobalGreatPeopleRateModifier;
	int m_iAnarchyModifier;
	int m_iGoldenAgeModifier;
	int m_iGlobalHurryModifier;
	int m_iFreeExperience;
	int m_iGlobalFreeExperience;
	int m_iFoodKept;
	int m_iAirlift;
	int m_iAirModifier;
	int m_iAirUnitCapacity;
	int m_iNukeModifier;
	int m_iNukeExplosionRand;
	int m_iFreeSpecialist;
	int m_iAreaFreeSpecialist;
	int m_iGlobalFreeSpecialist;
	int m_iHappiness;
	int m_iAreaHappiness;
	int m_iGlobalHappiness;
	int m_iStateReligionHappiness;
	int m_iWorkerSpeedModifier;
	int m_iMilitaryProductionModifier;
	int m_iSpaceProductionModifier;
	int m_iGlobalSpaceProductionModifier;
	int m_iTradeRoutes;
	int m_iCoastalTradeRoutes;
	int m_iGlobalTradeRoutes;
	int m_iTradeRouteModifier;
	int m_iForeignTradeRouteModifier;
	int m_iAssetValue;
	int m_iPowerValue;
	int m_iSpecialBuildingType;
	int m_iAdvisorType;

	int m_iPrereqGameOption;
	int m_iNotGameOption;

	int m_iHolyCity;
	int m_iReligionType;
	int m_iStateReligion;
	int m_iPrereqReligion;
	int m_iPrereqCorporation;
	int m_iFoundsCorporation;
	int m_iGlobalReligionCommerce;
	int m_iGlobalCorporationCommerce;
	int m_iPrereqAndBonus;
	int m_iGreatPeopleUnitType;
	int m_iGreatPeopleRateChange;
	int m_iConquestProbability;
	int m_iMaintenanceModifier;
	//DPII < Maintenance Modifier >
	int m_iGlobalMaintenanceModifier;
	int m_iAreaMaintenanceModifier;
	int m_iOtherAreaMaintenanceModifier;
	int m_iDistanceMaintenanceModifier;
	int m_iNumCitiesMaintenanceModifier;
	int m_iCoastalDistanceMaintenanceModifier;
	int m_iConnectedCityMaintenanceModifier;
	//DPII < Maintenance Modifier >
	int m_iWarWearinessModifier;
	int m_iGlobalWarWearinessModifier;
	int m_iEnemyWarWearinessModifier;
	int m_iHealRateChange;
	int m_iHealth;
	int m_iAreaHealth;
	int m_iGlobalHealth;
	int m_iGlobalPopulationChange;
	int m_iFreeTechs;
	TechTypes m_eFreeSpecialTech;
	int m_iDefenseModifier;
	int m_iBombardDefenseModifier;
	int m_iAllCityDefenseModifier;
	int m_iEspionageDefenseModifier;

	int m_iUnitUpgradePriceModifier;
	int m_iRevIdxLocal;
	int m_iRevIdxNational;
	int m_iRevIdxDistanceModifier;

	int m_iMissionType;
	int m_iVoteSourceType;
	int m_iDCMAirbombMission;
	int m_iMaxGlobalInstances;
	int m_iMaxTeamInstances;
	int m_iMaxPlayerInstances;
	int m_iExtraPlayerInstances;

	float m_fVisibilityPriority;

	bool m_bTeamShare;
	bool m_bWater;
	bool m_bRiver;
	bool m_bPower;
	bool m_bFreshWater;
	bool m_bDirtyPower;
	bool m_bAreaCleanPower;
	bool m_bOrbital;
	bool m_bOrbitalInfrastructure;
	bool m_bNoHolyCity;
	bool m_bAreaBorderObstacle;
	bool m_bForceTeamVoteEligible;
	bool m_bCapital;
	bool m_bGovernmentCenter;
	bool m_bGoldenAge;
	bool m_bMapCentering;
	bool m_bNoUnhappiness;
	bool m_bNoUnhealthyPopulation;
	bool m_bBuildingOnlyHealthy;
	bool m_bNeverCapture;
	bool m_bNukeImmune;
	bool m_bCenterInCity;
	bool m_bStateReligion;
	bool m_bAllowsNukes;

	CvString m_szConstructSound;
	CvString m_szArtDefineTag;
	CvString m_szMovieDefineTag;

	// Arrays

	int* m_piPrereqAndTechs;
	int* m_piSeaPlotYieldChange;
	int* m_piRiverPlotYieldChange;
	int* m_piGlobalSeaPlotYieldChange;
	int* m_piYieldChange;
	int* m_piYieldPerPopChange;
	int* m_piYieldModifier;
	int* m_piPowerYieldModifier;
	int* m_piAreaYieldModifier;
	int* m_piGlobalYieldModifier;
	int* m_piCommerceChange;
	int* m_piCommercePerPopChange;
	int* m_piObsoleteSafeCommerceChange;
	int* m_piCommerceChangeDoubleTime;
	int* m_piCommerceModifier;
	int* m_piGlobalCommerceModifier;
	int* m_piSpecialistExtraCommerce;
	int* m_piStateReligionCommerce;
	int* m_piCommerceHappiness;
	int* m_piReligionChange;
	int* m_piSpecialistCount;
	int* m_piFreeSpecialistCount;
	int* m_piBonusHealthChanges;
	int* m_piBonusHappinessChanges;
	int* m_piBonusProductionModifier;
	int* m_piUnitCombatFreeExperience;
	int* m_piDomainFreeExperience;
	bool m_bAnyUnitCombatFreeExperience;
	bool m_bAnyDomainFreeExperience;
	int* m_piDomainProductionModifier;
	std::vector<int> m_aiPrereqInCityBuildings;
	int* m_piFlavorValue;
	int* m_piImprovementFreeSpecialist;
	int* m_piVictoryThreshold;

	bool* m_pbCommerceFlexible;
	bool* m_pbCommerceChangeOriginalOwner;

	int** m_ppaiSpecialistYieldChange;
	int** m_ppaiSpecialistCommerceChange;
	int** m_ppaiBonusYieldModifier;

	CvProperties m_Properties;
	CvProperties m_PropertiesAllCities;
	CvProperties m_PrereqMinProperties;
	CvProperties m_PrereqMaxProperties;
	CvProperties m_PrereqPlayerMinProperties;
	CvProperties m_PrereqPlayerMaxProperties;

	CvPropertyManipulators m_PropertyManipulators;

	BoolExpr* m_pExprNewCityFree;
	BoolExpr* m_pExprConstructCondition;

	std::vector<BuildingCommerceChange> m_aGlobalBuildingCommerceChanges;

	//TB Combat Mods (Buildings) begin
	UnitTypes m_ePropertySpawnUnit;
	PropertyTypes m_ePropertySpawnProperty;
	PromotionLineTypes m_ePromotionLineType;
	//TechTypes m_eFreeSpecialTech;
	//integers
	int m_iLinePriority;
	int m_iOutbreakBase;
	int m_iOvercomeBase;
	int m_iTradeCommunicability;
	int m_iFrontSupportPercentModifier;
	int m_iShortRangeSupportPercentModifier;
	int m_iMediumRangeSupportPercentModifier;
	int m_iLongRangeSupportPercentModifier;
	int m_iFlankSupportPercentModifier;
	//Team Project (3)
	int m_iNationalCaptureProbabilityModifier;
	int m_iNationalCaptureResistanceModifier;
	int m_iLocalCaptureProbabilityModifier;
	int m_iLocalCaptureResistanceModifier;
	int m_iLocalDynamicDefense;
	int m_iRiverDefensePenalty;
	int m_iLocalRepel;
	int m_iMinDefense;
	int m_iBuildingDefenseRecoverySpeedModifier;
	int m_iCityDefenseRecoverySpeedModifier;
	int m_iDamageAttackerChance;
	int m_iDamageToAttacker;
	int m_iMaxPopulationAllowed;
	int m_iMaxPopulationChange;
	int m_iInsidiousness;
	int m_iInvestigation;
	int m_iPopulationChange;
	int m_iOneTimePopulationPercentLoss;
	//Booleans
	bool m_bDamageToAttackerIgnoresArmor;
	bool m_bDamageAllAttackers;
	bool m_bDamageAttackerCapable;
	bool m_bQuarantine;
	//boolean vectors without delayed resolution
	//std::vector<int> m_aiFreePromoTypes;
	std::vector<int> m_aiUnitCombatRetrainTypes;
	std::vector<int> m_aiMayDamageAttackingUnitCombatTypes;
	std::vector<int> m_aiMapCategoryTypes;
	//integer vectors with pairing without delayed resolution
	UnitCombatModifierArray m_aUnitCombatRepelModifiers;
	UnitCombatModifierArray m_aUnitCombatRepelAgainstModifiers;
	UnitCombatModifierArray m_aUnitCombatDefenseAgainstModifiers;
	UnitCombatModifierArray m_aUnitCombatProdModifiers;
	UnitCombatModifierArray m_aUnitCombatOngoingTrainingDurations;
	PromotionLineModifierArray m_aAfflictionOutbreakLevelChanges;
	TechModifierArray m_aTechOutbreakLevelChanges;
	//Team Project (1)
	TechModifierArray m_aTechHappinessTypes;
	TechModifierArray m_aTechHealthTypes;
	//arrays
	int** m_ppaiLocalSpecialistYieldChange;
	int** m_ppaiLocalSpecialistCommerceChange;
	//Alberts2 PrereqBonuses
	std::vector<int> m_aePrereqOrBonuses;
public:
	bool m_bAnyLocalSpecialistYieldChanges;
	bool m_bAnyLocalSpecialistCommerceChanges;

protected:
	bool* m_pabHurry;

	//ls612 Begin:
	int m_iMaxPopAllowed;
	int m_iMaxCultureLevelAllowed;
	//ls612 End:

	/*
	BoolExpr* m_pExprFreePromotionCondition;*/
	//Structs
	std::vector<FreePromoTypes> m_aFreePromoTypes;
	std::vector<int> m_aiFreeTraitTypes;
	std::vector<HealUnitCombat> m_aHealUnitCombatTypes;
	std::vector<EnabledCivilizations> m_aEnabledCivilizationTypes;
	std::vector<BonusAidModifiers> m_aBonusAidModifiers;
	std::vector<AidRateChanges> m_aAidRateChanges;
	//TB Combat Mods (Buildings) end

	bool	m_bAutoBuild;

	mutable bool m_bEnablesOtherBuildingsCalculated;
	mutable bool m_bEnablesOtherBuildingsValue;
	mutable bool m_bEnablesUnitsCalculated;
	mutable bool m_bEnablesUnits;
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

	int getObsoleteTech( void ) const; // Exposed to Python
	int getTechPrereq( void ) const; // Exposed to Python
	int getTechPrereqAnyone( void ) const; // Exposed to Python
	int getMaxPlayerInstances() const;

	bool isValid( void ) const; // Exposed to Python

	// Arrays

	bool read(CvXMLLoadUtility* pXML);

	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}

	void copyNonDefaults(CvSpecialBuildingInfo* pClassInfo = NULL , CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iObsoleteTech;
	int m_iTechPrereq;
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

	DllExport const TCHAR* getModelFile() const; // Exposed to Python
	void setModelFile(const TCHAR* szVal);				// The model filename
	DllExport const TCHAR* getBorderFile() const; // Exposed to Python
	void setBorderFile(const TCHAR* szVal);				// The model filename

	DllExport int getTextureIndex() const;
	DllExport const TCHAR* getDeltaString() const;		//Exposed to Python
	DllExport const TCHAR* getConnectString() const; // Exposed to Python
	DllExport const TCHAR* getRotateString() const; // Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvRiverModelInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	CvString m_szModelFile;					// The model filename
	CvString m_szBorderFile;				// The border filename

	TCHAR		m_szDeltaString[32];		// The delta type
	TCHAR		m_szConnectString[32];		// The connections this cell makes ( N S E W NE NW SE SW )
	TCHAR		m_szRotateString[32];		// The possible rotations for this cell ( 0 90 180 270 )
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

	DllExport RouteTypes getRouteType() const;			// The route type

	DllExport const TCHAR* getModelFile() const; // Exposed to Python
	void setModelFile(const TCHAR* szVal);				// The model filename
	DllExport const TCHAR* getLateModelFile() const; // Exposed to Python
	void setLateModelFile(const TCHAR* szVal);			// The model filename
	const TCHAR* getModelFileKey() const; // Exposed to Python
	void setModelFileKey(const TCHAR* szVal);			// The model filename Key

	DllExport bool isAnimated() const;

	DllExport const TCHAR* getConnectString() const; // Exposed to Python
	DllExport const TCHAR* getModelConnectString() const; // Exposed to Python
	DllExport const TCHAR* getRotateString() const; // Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvRouteModelInfo* pClassInfo = NULL , CvXMLLoadUtility* pXML = NULL);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	RouteTypes	m_eRouteType;			// The route type

	CvString m_szModelFile;				// The model filename
	CvString m_szLateModelFile;			// The model filename
	CvString m_szModelFileKey;			// The model file key reference
	bool m_bAnimated;

	TCHAR	m_szConnectString[32];		// The connections this cell makes ( N S E W NE NW SE SW )
	TCHAR	m_szModelConnectString[32];	// The connections this model makes ( N S E W NE NW SE SW )
	TCHAR	m_szRotateString[32];		// The possible rotations for this cell ( 0 90 180 270 )
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

	DllExport int getDefaultPlayerColor() const; // Exposed to Python
	int getArtStyleType() const; // Exposed to Python
	int getUnitArtStyleType() const; // Exposed to Python
	int getNumCityNames() const; // Exposed to Python
	// the number of leaders the Civ has, this is needed so that random leaders can be generated easily
	int getNumLeaders() const; // Exposed to Python
	int getSelectionSoundScriptId() const; // Exposed to Python
	int getActionSoundScriptId() const; // Exposed to Python

	DllExport bool isAIPlayable() const; // Exposed to Python
	DllExport bool isPlayable() const; // Exposed to Python

	std::wstring pyGetShortDescription(uint uiForm) { return getShortDescription(uiForm); } // Exposed to Python
	DllExport const wchar* getShortDescription(uint uiForm = 0);
	const wchar* getShortDescriptionKey() const; // Exposed to Python
	std::wstring pyGetShortDescriptionKey() { return getShortDescriptionKey(); } // Exposed to Python

	std::wstring pyGetAdjective(uint uiForm) { return getAdjective(uiForm); } // Exposed to Python
	DllExport const wchar* getAdjective(uint uiForm = 0);
	const wchar* getAdjectiveKey() const; // Exposed to Python
	std::wstring pyGetAdjectiveKey() { return getAdjectiveKey(); } // Exposed to Python

	DllExport const TCHAR* getFlagTexture() const;
	const TCHAR* getArtDefineTag() const;
	void setArtDefineTag(const TCHAR* szVal);
	// Arrays

	int getCivilizationFreeUnits(int i) const; // Exposed to Python
	int getCivilizationInitialCivics(int i) const; // Exposed to Python
	// Afforess 04/05/10
	void setCivilizationInitialCivics(int iCivicOption, int iCivic);

	DllExport bool isLeaders(int i) const; // Exposed to Python

	bool isCivilizationFreeTechs(int i) const; // Exposed to Python
	bool isCivilizationDisableTechs(int i) const; // Exposed to Python

	int getNumCivilizationBuildings() const;
	int getCivilizationBuilding(int i) const;
	bool isCivilizationBuilding(int i) const; // Exposed to Python

	std::string getCityNames(int i) const; // Exposed to Python

	const CvArtInfoCivilization* getArtInfo() const;
	const TCHAR* getButton() const;

	int getDerivativeCiv() const; // Exposed to Python
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
	int m_iUnitArtStyleType; // FlavorUnits by Impaler[WrG]
	int m_iNumCityNames;
	int m_iNumLeaders;
	int m_iSelectionSoundScriptId;
	int m_iActionSoundScriptId;
	int m_iDerivativeCiv;
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

	int getPopulationPercentLead() const; // Exposed to Python
	int getLandPercent() const; // Exposed to Python
	int getMinLandPercent() const; // Exposed to Python
	int getReligionPercent() const; // Exposed to Python
	int getCityCulture() const; // Exposed to Python
	int getNumCultureCities() const; // Exposed to Python
	int getTotalCultureRatio() const; // Exposed to Python
	//Sevo VCM
	bool isTotalVictory() const;
	int getVictoryDelayTurns() const; // Exposed to Python

	bool isTargetScore() const; // Exposed to Python
	bool isEndScore() const; // Exposed to Python
	bool isConquest() const; // Exposed to Python
	bool isDiploVote() const; // Exposed to Python
	DllExport bool isPermanent() const; // Exposed to Python

	const char* getMovie() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvVictoryInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

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
	//Sevo VCM
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

		int getGoldPerProduction() const; // Exposed to Python
		int getProductionPerPopulation() const; // Exposed to Python

		bool isAnger() const; // Exposed to Python

		void read(FDataStreamBase* stream) {}
		void write(FDataStreamBase* stream) {}

		bool read(CvXMLLoadUtility* pXML);
		void copyNonDefaults(CvHurryInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
		void getCheckSum(unsigned int& iSum);

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

	int getFreeWinsVsBarbs() const; // Exposed to Python
	int getAnimalAttackProb() const; // Exposed to Python
	int getStartingLocationPercent() const; // Exposed to Python
	int getAdvancedStartPointsMod() const; // Exposed to Python
	int getStartingGold() const; // Exposed to Python
	int getFreeUnits() const; // Exposed to Python
	int getUnitCostPercent() const; // Exposed to Python
	int getTrainPercent() const;
	int getConstructPercent() const;
	int getResearchPercent() const; // Exposed to Python
	int getDistanceMaintenancePercent() const; // Exposed to Python
	int getNumCitiesMaintenancePercent() const; // Exposed to Python
	int getColonyMaintenancePercent() const; // Exposed to Python
	int getMaxColonyMaintenance() const; // Exposed to Python
	int getCorporationMaintenancePercent() const; // Exposed to Python
	int getCivicUpkeepPercent() const; // Exposed to Python
	int getInflationPercent() const; // Exposed to Python
	/*** REVOLUTION 09/25/09 by DPII ***/
	int getRevolutionIndexPercent() const;
	/***********************************/
	int getHealthBonus() const; // Exposed to Python
	int getHappyBonus() const; // Exposed to Python
	int getAttitudeChange() const; // Exposed to Python
	int getNoTechTradeModifier() const; // Exposed to Python
	int getTechTradeKnownModifier() const; // Exposed to Python
	int getUnownedTilesPerGameAnimal() const; // Exposed to Python
	int getUnownedTilesPerBarbarianUnit() const; // Exposed to Python
	int getUnownedWaterTilesPerBarbarianUnit() const; // Exposed to Python
	int getUnownedTilesPerBarbarianCity() const; // Exposed to Python
	int getBarbarianCityCreationTurnsElapsed() const; // Exposed to Python
	int getBarbarianCityCreationProb() const; // Exposed to Python
	int getAnimalCombatModifier() const; // Exposed to Python
	int getBarbarianCombatModifier() const; // Exposed to Python
	int getAIAnimalCombatModifier() const; // Exposed to Python
	int getAIBarbarianCombatModifier() const; // Exposed to Python

	int getStartingDefenseUnits() const; // Exposed to Python
	int getStartingWorkerUnits() const; // Exposed to Python
	int getStartingExploreUnits() const; // Exposed to Python
	int getAIStartingUnitMultiplier() const; // Exposed to Python
	int getAIStartingDefenseUnits() const; // Exposed to Python
	int getAIStartingWorkerUnits() const; // Exposed to Python
	int getAIStartingExploreUnits() const; // Exposed to Python
	int getBarbarianInitialDefenders() const; // Exposed to Python
	int getAIDeclareWarProb() const; // Exposed to Python
	int getAIWorkRateModifier() const; // Exposed to Python
	int getAIGrowthPercent() const; // Exposed to Python
	int getAITrainPercent() const; // Exposed to Python
	int getAIWorldTrainPercent() const; // Exposed to Python
	int getAIConstructPercent() const; // Exposed to Python
	int getAIWorldConstructPercent() const; // Exposed to Python
	int getAICreatePercent() const; // Exposed to Python
	int getAIResearchPercent() const;
	int getAIWorldCreatePercent() const; // Exposed to Python
	int getAICivicUpkeepPercent() const; // Exposed to Python
	int getAIUnitCostPercent() const; // Exposed to Python
	int getAIUnitSupplyPercent() const; // Exposed to Python
	int getAIUnitUpgradePercent() const; // Exposed to Python
	int getAIInflationPercent() const; // Exposed to Python
	int getAIWarWearinessPercent() const; // Exposed to Python
	int getAIPerEraModifier() const; // Exposed to Python
	int getAIAdvancedStartPercent() const; // Exposed to Python
	int getNumGoodies() const; // Exposed to Python

	CvPropertyManipulators* getPropertyManipulators();

	// Arrays

	int getGoodies(int i) const; // Exposed to Python
	int isFreeTechs(int i) const; // Exposed to Python
	int isAIFreeTechs(int i) const; // Exposed to Python

	int getPercent(int iID) const;

	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvHandicapInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iFreeWinsVsBarbs;
	int m_iAnimalAttackProb;
	int m_iStartingLocationPercent;
	int m_iAdvancedStartPointsMod;
	int m_iStartingGold;
	int m_iFreeUnits;
	int m_iUnitCostPercent;
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
	/*** REVOLUTION 09/25/09 by DPII ***/
	int m_iRevolutionIndexPercent;
	/***********************************/
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
	int m_iAIUnitCostPercent;
	int m_iAIUnitSupplyPercent;
	int m_iAIUnitUpgradePercent;
	int m_iAIInflationPercent;
	int m_iAIWarWearinessPercent;
	int m_iAIPerEraModifier;
	int m_iAIAdvancedStartPercent;
	int m_iNumGoodies;

	CvPropertyManipulators m_PropertyManipulators;

	CvString m_szHandicapName;

	IDValueMapPercent m_Percent;

	// Arrays

	int* m_piGoodies;

	bool* m_pbFreeTechs;
	bool* m_pbAIFreeTechs;

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

	int getGrowthPercent() const; // Exposed to Python
	int getTrainPercent() const; // Exposed to Python
	int getConstructPercent() const; // Exposed to Python
	int getCreatePercent() const; // Exposed to Python
	int getResearchPercent() const; // Exposed to Python
	int getBuildPercent() const; // Exposed to Python
	int getImprovementPercent() const; // Exposed to Python
	int getGreatPeoplePercent() const; // Exposed to Python
	int getAnarchyPercent() const; // Exposed to Python
	int getBarbPercent() const; // Exposed to Python
	int getFeatureProductionPercent() const; // Exposed to Python
	int getUnitDiscoverPercent() const; // Exposed to Python
	int getUnitHurryPercent() const; // Exposed to Python
	int getUnitTradePercent() const; // Exposed to Python
	int getUnitGreatWorkPercent() const; // Exposed to Python
	int getGoldenAgePercent() const; // Exposed to Python
	int getHurryPercent() const; // Exposed to Python
	int getHurryConscriptAngerPercent() const; // Exposed to Python
	int getInflationOffset() const; // Exposed to Python
	int getInflationPercent() const; // Exposed to Python
	int getOccupationTimePopulationPercent() const;
	int getVictoryDelayPercent() const; // Exposed to Python
	int getNumTurnIncrements() const; // Exposed to Python
	int getUnitMovementPercent() const;

	GameTurnInfo& getGameTurnInfo(int iIndex) const; // Exposed to Python
	CvDateIncrement& getDateIncrement(int iIndex);
	std::vector<CvDateIncrement>& getIncrements();
	bool getEndDatesCalculated() const;
	void setEndDatesCalculated(bool bCalculated);

	void allocateGameTurnInfos(const int iSize);

	int getPercent(int iID) const;

	//TB GameSpeed begin
	int getTraitGainPercent() const;
	//TB GameSpeed end

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvGameSpeedInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iGrowthPercent;
	int m_iTrainPercent;
	int m_iConstructPercent;
	int m_iCreatePercent;
	int m_iResearchPercent;
	int m_iBuildPercent;
	int m_iImprovementPercent;
	int m_iGreatPeoplePercent;
	int m_iAnarchyPercent;
	int m_iBarbPercent;
	int m_iFeatureProductionPercent;
	int m_iUnitDiscoverPercent;
	int m_iUnitHurryPercent;
	int m_iUnitTradePercent;
	int m_iUnitGreatWorkPercent;
	int m_iGoldenAgePercent;
	int m_iHurryPercent;
	int m_iHurryConscriptAngerPercent;
	int m_iInflationOffset;
	int m_iInflationPercent;
	int m_iOccupationTimePopulationPercent;
	int m_iVictoryDelayPercent;
	int m_iNumTurnIncrements;
	// Afforess 12/13/09
	int m_iUnitMovementPercent;

	CvString m_szGameSpeedName;
	GameTurnInfo* m_pGameTurnInfo;
	std::vector<CvDateIncrement> m_aIncrements;
	bool m_bEndDatesCalculated;
	//TB GameSpeed begin
	int m_iTraitGainPercent;
	//TB GameSpeed end

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

	int getBaseTime() const; // Exposed to Python
	int getCityBonus() const; // Exposed to Python
	int getUnitBonus() const; // Exposed to Python
	int getFirstTurnMultiplier() const; // Exposed to Python

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvTurnTimerInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

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

	int getTime() const; // Exposed to Python
	int getCost() const; // Exposed to Python
	int getTechPrereq() const; // Exposed to Python
	int getImprovement() const; // Exposed to Python
	int getRoute() const; // Exposed to Python
/************************************************************************************************/
/* Afforess					  Start		 05/25/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	int getTerrainChange() const; // Exposed to Python
	int getFeatureChange() const; // Exposed to Python
	int getObsoleteTech() const;
	bool isMine() const;
	bool isNoTechCanRemoveWithNoProductionGain(int i) const;
	bool isDisabled() const;
	bool isHideObsoleteExempt() const;
	void setDisabled(bool bNewVal);
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	DllExport int getEntityEvent() const; // Exposed to Python
	DllExport int getMissionType() const; // Exposed to Python
	void setMissionType(int iNewType);

	bool isKill() const; // Exposed to Python

	// Arrays

	int getFeatureTech(int i) const; // Exposed to Python
	int getFeatureTime(int i) const; // Exposed to Python
	int getFeatureProduction(int i) const; // Exposed to Python

	bool isFeatureRemove(int i) const; // Exposed to Python

	// Vectors
	int getPrereqBonusType(int i) const;
	int getNumPrereqBonusTypes() const;
	bool isPrereqBonusType(int i) const;

	int getMapCategoryType(int i) const;
	int getNumMapCategoryTypes() const;
	bool isMapCategoryType(int i);

	//Struct Vector with delayed resolution
	int getNumTerrainStructs() const;
	TerrainStructs& getTerrainStruct(int iIndex);

	int getNumPlaceBonusTypes() const;
	PlaceBonusTypes& getPlaceBonusType(int iIndex);

	bool read(CvXMLLoadUtility* pXML);
	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}
	void copyNonDefaults(CvBuildInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iTime;
	int m_iCost;
	int m_iTechPrereq;
	int m_iImprovement;
	int m_iRoute;
/************************************************************************************************/
/* Afforess					  Start		 05/25/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	int m_iTerrainChange;
	int m_iFeatureChange;
	int m_iObsoleteTech;
	bool m_bMine;
	bool m_bDisabled;
	bool m_bHideObsoleteExempt;
	bool* m_pabNoTechCanRemoveWithNoProductionGain;
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	int m_iEntityEvent;
	int m_iMissionType;

	bool m_bKill;

	// Arrays

	int* m_paiFeatureTech;
	int* m_paiFeatureTime;
	int* m_paiFeatureProduction;

	bool* m_pabFeatureRemove;

	std::vector<int> m_aiPrereqBonusTypes;
	std::vector<int> m_aiMapCategoryTypes;

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

	int getGold() const; // Exposed to Python
	int getGoldRand1() const; // Exposed to Python
	int getGoldRand2() const; // Exposed to Python
	int getMapOffset() const; // Exposed to Python
	int getMapRange() const; // Exposed to Python
	int getMapProb() const; // Exposed to Python
	int getExperience() const; // Exposed to Python
	int getHealing() const; // Exposed to Python
	int getDamagePrereq() const; // Exposed to Python
	int getBarbarianUnitProb() const; // Exposed to Python
	int getMinBarbarians() const; // Exposed to Python
	int getGoodyUnit() const; // Exposed to Python
	int getBarbarianUnit() const; // Exposed to Python
	int getEraType() const;
	int getNotEraType() const;
	int getResearch() const;

	bool isTech() const; // Exposed to Python
	bool isBad() const; // Exposed to Python
	bool isNaval() const;

	//Vectors
	int getMapCategoryType(int i) const;
	int getNumMapCategoryTypes() const;
	bool isMapCategoryType(int i);

	const TCHAR* getSound() const; // Exposed to Python
	void setSound(const TCHAR* szVal);

	bool read(CvXMLLoadUtility* pXML);
	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}

	void copyNonDefaults(CvGoodyInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

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

	std::vector<int> m_aiMapCategoryTypes;

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

	int getAdvancedStartCost() const; // Exposed to Python
	int getAdvancedStartCostIncrease() const; // Exposed to Python

	int getValue() const; // Exposed to Python
	int getMovementCost() const; // Exposed to Python
	int getFlatMovementCost() const; // Exposed to Python
	int getPrereqBonus() const; // Exposed to Python

	// JOOYO_ADDON, Added by Jooyo, 07/07/09
	bool isSeaTunnel() const;

	// Arrays
	int getYieldChange(int i) const; // Exposed to Python
	int* getYieldChangeArray() const;
	int getTechMovementChange(int i) const; // Exposed to Python
	int getPrereqOrBonus(int i) const; // Exposed to Python
	bool isAnyPrereqOrBonus() const;

	CvPropertyManipulators* getPropertyManipulators();
	//	This really belongs on CvInfoBase but you can't change the size of that
	//	object without crashing the core engine :-(
	inline int	getZobristValue() const { return m_zobristValue; }

	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}
	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvRouteInfo* pClassInfo = NULL , CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iAdvancedStartCost;
	int m_iAdvancedStartCostIncrease;

	int m_iValue;
	int m_iMovementCost;
	int m_iFlatMovementCost;
	int m_iPrereqBonus;
	
	int m_zobristValue;

	// JOOYO_ADDON, Added by Jooyo, 07/07/09
	bool m_bSeaTunnel;

	bool m_bAnyPrereqOrBonus;

	// Arrays
	int* m_piYieldChange;
	int* m_piTechMovementChange;
	int* m_piPrereqOrBonuses;

	CvPropertyManipulators m_PropertyManipulators;
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

	int getDiscoverRand() const; // Exposed to Python
	// Afforess 01/20/10
	int getDepletionRand() const;

	bool isBonusMakesValid() const; // Exposed to Python
	bool isObsoleteBonusMakesValid() const;
	bool isBonusTrade() const; // Exposed to Python

	int getYieldChange(int i) const; // Exposed to Python

	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iDiscoverRand;
	// Afforess 01/20/10
	int m_iDepletionRand;

	bool m_bBonusMakesValid;
	bool m_bObsoleteBonusMakesValid;
	bool m_bBonusTrade;

	// Arrays
	int* m_piYieldChange;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvImprovementInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoImprovement;
class CvImprovementInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvImprovementInfo();
	virtual ~CvImprovementInfo();

	int getAdvancedStartCost() const; // Exposed to Python
	int getAdvancedStartCostIncrease() const; // Exposed to Python

	int getTilesPerGoody() const; // Exposed to Python
	int getGoodyUniqueRange() const; // Exposed to Python
	int getFeatureGrowthProbability() const; // Exposed to Python
	int getUpgradeTime() const; // Exposed to Python
	int getAirBombDefense() const; // Exposed to Python
	int getDefenseModifier() const; // Exposed to Python
	int getHappiness() const; // Exposed to Python
	int getPillageGold() const; // Exposed to Python
	int getImprovementPillage() const; // Exposed to Python
	void setImprovementPillage(int i);
	int getImprovementUpgrade() const; // Exposed to Python
	void setImprovementUpgrade(int i);
	// Super Forts begin *XML*
	int getCulture() const;
	int getCultureRange() const;
	int getVisibilityChange() const;
	int getSeeFrom() const;
	int getUniqueRange() const;
	bool isBombardable() const;
	bool isUpgradeRequiresFortify() const;
	// Super Forts end
	// Super forts C2C adaptation
	bool isUniversalTradeBonusProvider() const;
	// Super forts C2C adaptation end

	bool isZOCSource() const; // Exposed to Python
	bool isActsAsCity() const; // Exposed to Python
	bool isHillsMakesValid() const; // Exposed to Python
	bool isFreshWaterMakesValid() const; // Exposed to Python
	bool isRiverSideMakesValid() const; // Exposed to Python
	bool isNoFreshWater() const; // Exposed to Python
	bool isRequiresFlatlands() const; // Exposed to Python
	DllExport bool isRequiresRiverSide() const; // Exposed to Python
	bool isRequiresIrrigation() const; // Exposed to Python
	bool isCarriesIrrigation() const; // Exposed to Python
	bool isRequiresFeature() const; // Exposed to Python
	bool isPeakImprovement() const; // Exposed to Python
	bool isWaterImprovement() const; // Exposed to Python
	DllExport bool isGoody() const; // Exposed to Python
	bool isPermanent() const; // Exposed to Python
	bool isOutsideBorders() const; // Exposed to Python

	const TCHAR* getArtDefineTag() const;
	void setArtDefineTag(const TCHAR* szVal);

	int getWorldSoundscapeScriptId() const;

	// Arrays

	int getPrereqNatureYield(int i) const; // Exposed to Python
	int* getPrereqNatureYieldArray() const;
	int getYieldChange(int i) const; // Exposed to Python
	int* getYieldChangeArray() const;
	int getRiverSideYieldChange(int i) const; // Exposed to Python
	int* getRiverSideYieldChangeArray() const;
	int getHillsYieldChange(int i) const; // Exposed to Python
	int* getHillsYieldChangeArray() const;
	int getIrrigatedYieldChange(int i) const; // Exposed to Python
	int* getIrrigatedYieldChangeArray() const;		// For Moose - CvWidgetData XXX

	bool getTerrainMakesValid(int i) const; // Exposed to Python
	bool getFeatureMakesValid(int i) const; // Exposed to Python

	int getTechYieldChanges(int i, int j) const; // Exposed to Python
	int* getTechYieldChangesArray(int i) const;
	int getRouteYieldChanges(int i, int j) const; // Exposed to Python
	int* getRouteYieldChangesArray(int i) const;		// For Moose - CvWidgetData XXX

	int getImprovementBonusYield(int i, int j) const; // Exposed to Python
	bool isImprovementBonusMakesValid(int i) const; // Exposed to Python
	bool isImprovementObsoleteBonusMakesValid(int i) const; // Exposed to Python
	bool isImprovementBonusTrade(int i) const; // Exposed to Python
	int getImprovementBonusDiscoverRand(int i) const; // Exposed to Python

	// Other
	int getMapCategoryType(int i) const;
	int getNumMapCategoryTypes() const;
	bool isMapCategoryType(int i);

	const TCHAR* getButton() const;
	DllExport const CvArtInfoImprovement* getArtInfo() const;

	// Afforess 12/9/09
	int getHealthPercent() const; // Exposed to Python
	bool isPeakMakesValid() const; // Exposed to Python
	int getImprovementBonusDepletionRand(int i) const;
	int getPrereqTech() const; // Exposed to Python
	//int getTraitYieldChanges(int i, int j) const;
	//int* getTraitYieldChangesArray(int i) const;

	//TB Improvements
	//Object Indexes
	int getBonusChange() const;
	//booleans
	bool isCanMoveSeaUnits() const;
	bool isChangeRemove() const;
	bool isNotOnAnyBonus() const;
	bool isNational() const;
	bool isGlobal() const;
	// bool vector with delayed resolution
	int getAlternativeImprovementUpgradeType(int i) const;
	int getNumAlternativeImprovementUpgradeTypes() const;
	bool isAlternativeImprovementUpgradeType(int i) const;

	int getFeatureChangeType(int i) const;
	int getNumFeatureChangeTypes() const;
	bool isFeatureChangeType(int i) const;

	//Post Load Functions
	void setHighestCost();
	int getHighestCostBuild() const;
	int getHighestCost() const;

protected:
	bool m_bPeakMakesValid;
	int m_iHealthPercent;
	bool m_bDepletedMine;
	int m_iDepletionRand;
	int m_iPrereqTech;
	//int** m_ppiTraitYieldChanges;

	std::vector<int> m_aiMapCategoryTypes;
public:
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}
	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvImprovementInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void copyNonDefaultsReadPass2(CvImprovementInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL, bool bOver = false);

	void getCheckSum(unsigned int& iSum);

	CvPropertyManipulators* getPropertyManipulators();

	static ImprovementTypes getImprovementCity();
	static void setImprovementCity(ImprovementTypes eIndex);

	static ImprovementTypes getImprovementRuins();
	static void setImprovementRuins(ImprovementTypes eIndex);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iAdvancedStartCost;
	int m_iAdvancedStartCostIncrease;

	int m_iTilesPerGoody;
	int m_iGoodyUniqueRange;
	int m_iFeatureGrowthProbability;
	int m_iUpgradeTime;
	int m_iAirBombDefense;
	int m_iDefenseModifier;
	int m_iHappiness;
	int m_iPillageGold;
	int m_iImprovementPillage;
	int m_iImprovementUpgrade;
	// Super Forts begin *XML*
	int m_iCulture;
	int m_iCultureRange;
	int m_iVisibilityChange;
	int m_iSeeFrom;
	int m_iUniqueRange;
	bool m_bBombardable;
	bool m_bUpgradeRequiresFortify;
	// Super Forts end
	// Super forts C2C adaptation
	bool m_bIsUniversalTradeBonusProvider;
	bool m_bIsZOCSource;
	// Super forts C2C adaptation end

	bool m_bActsAsCity;
	bool m_bHillsMakesValid;
	bool m_bFreshWaterMakesValid;
	bool m_bRiverSideMakesValid;
	bool m_bNoFreshWater;
	bool m_bRequiresFlatlands;
	bool m_bRequiresRiverSide;
	bool m_bRequiresIrrigation;
	bool m_bCarriesIrrigation;
	bool m_bRequiresFeature;
	bool m_bPeakImprovement;
	bool m_bWaterImprovement;
	bool m_bGoody;
	bool m_bPermanent;
	bool m_bOutsideBorders;

	CvString m_szArtDefineTag;

	int m_iWorldSoundscapeScriptId;

	// Arrays

	int* m_piPrereqNatureYield;
	int* m_piYieldChange;
	int* m_piRiverSideYieldChange;
	int* m_piHillsYieldChange;
	int* m_piIrrigatedChange;

	bool* m_pbTerrainMakesValid;
	bool* m_pbFeatureMakesValid;

	int** m_ppiTechYieldChanges;
	int** m_ppiRouteYieldChanges;

	CvImprovementBonusInfo* m_paImprovementBonus;

	CvPropertyManipulators m_PropertyManipulators;

	//TB Improvements
	//Object Indexes
	int m_iBonusChange;
	//booleans
	bool m_bCanMoveSeaUnits;
	bool m_bChangeRemove;
	bool m_bNotOnAnyBonus;
	bool m_bNational;
	bool m_bGlobal;
	// bool vector with delayed resolution
	std::vector<int> m_aiAlternativeImprovementUpgradeTypes;
	std::vector<int> m_aiFeatureChangeTypes;

	//Post Load Functions
	int m_eHighestCostBuild;
	int m_iHighestCost;
	static ImprovementTypes m_ImprovementCity;
	static ImprovementTypes m_ImprovementRuins;
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

	int getUniqueRange() const; // Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvBonusClassInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

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

	int getBonusClassType() const; // Exposed to Python
	int getChar() const; // Exposed to Python
	void setChar(int i); // Exposed to Python
	int getTechReveal() const; // Exposed to Python
	int getTechCityTrade() const; // Exposed to Python
	int getTechObsolete() const; // Exposed to Python
	int getAITradeModifier() const; // Exposed to Python
	int getAIObjective() const; // Exposed to Python
	int getHealth() const; // Exposed to Python
	int getHappiness() const; // Exposed to Python
	int getMinAreaSize() const; // Exposed to Python
	int getMinLatitude() const; // Exposed to Python
	int getMaxLatitude() const; // Exposed to Python
	int getPlacementOrder() const; // Exposed to Python
	int getConstAppearance() const; // Exposed to Python
	int getRandAppearance1() const; // Exposed to Python
	int getRandAppearance2() const; // Exposed to Python
	int getRandAppearance3() const; // Exposed to Python
	int getRandAppearance4() const; // Exposed to Python
	int getPercentPerPlayer() const; // Exposed to Python
	int getTilesPer() const; // Exposed to Python
	int getMinLandPercent() const; // Exposed to Python
	int getUniqueRange() const; // Exposed to Python
	int getGroupRange() const; // Exposed to Python
	int getGroupRand() const; // Exposed to Python

	bool isOneArea() const; // Exposed to Python
	bool isHills() const; // Exposed to Python
	bool isPeaks() const; // Exposed to Python
	bool isFlatlands() const; // Exposed to Python
	bool isBonusCoastalOnly() const; // Exposed to Python
	bool isNoRiverSide() const; // Exposed to Python
	bool isNormalize() const; // Exposed to Python

	const TCHAR* getArtDefineTag() const;
	void setArtDefineTag(const TCHAR* szVal);

	// Arrays
	int getYieldChange(int i) const; // Exposed to Python
	int* getYieldChangeArray() const; // Exposed to Python
	int getImprovementChange(int i) const;

	bool isTerrain(int i) const; // Exposed to Python
	bool isFeature(int i) const; // Exposed to Python
	bool isFeatureTerrain(int i) const; // Exposed to Python

	//Vectors
	int getMapCategoryType(int i) const;
	int getNumMapCategoryTypes() const;
	bool isMapCategoryType(int i);


	int getNumAfflictionCommunicabilityTypes() const;
	PromotionLineAfflictionModifier getAfflictionCommunicabilityType(int iPromotionLine, bool bWorkedTile = false, bool bVicinity = false, bool bAccessVolume = false);

	// Other
	const TCHAR* getButton() const; // Exposed to Python
	DllExport const CvArtInfoBonus* getArtInfo() const; // Exposed to Python
	CvPropertyManipulators* getPropertyManipulators();

	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvBonusInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

	const std::vector<std::pair<ImprovementTypes,BuildTypes> >*	getTradeProvidingImprovements();

	//---------------------------------------PUBLIC MEMBER VARIABLES---------------------------------

protected:

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

	std::vector<int> m_aiMapCategoryTypes;

	std::vector<PromotionLineAfflictionModifier> m_aAfflictionCommunicabilityTypes;

	CvPropertyManipulators m_PropertyManipulators;

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

	int getMovementCost() const; // Exposed to Python
	int getSeeThroughChange() const; // Exposed to Python
	int getHealthPercent() const; // Exposed to Python
	int getAppearanceProbability() const; // Exposed to Python
	int getDisappearanceProbability() const; // Exposed to Python
	int getGrowthProbability() const; // Exposed to Python
	int getDefenseModifier() const; // Exposed to Python
	int getAdvancedStartRemoveCost() const; // Exposed to Python
	int getTurnDamage() const; // Exposed to Python
	int getWarmingDefense() const; //GWMod new xml field M.A.
	int getPopDestroys() const;

	bool isNoCoast() const; // Exposed to Python
	bool isNoRiver() const; // Exposed to Python
	bool isNoAdjacent() const; // Exposed to Python
	bool isRequiresFlatlands() const; // Exposed to Python
	bool isRequiresRiver() const; // Exposed to Python
	bool isAddsFreshWater() const; // Exposed to Python
	bool isImpassable() const; // Exposed to Python
	bool isNoCity() const; // Exposed to Python
	bool isNoImprovement() const; // Exposed to Python
	bool isVisibleAlways() const; // Exposed to Python
	bool isNukeImmune() const; // Exposed to Python
// BUG - City Plot Status - start
	bool isOnlyBad() const; // Exposed to Python
	bool isCountsAsPeak() const;
// BUG - City Plot Status - end
	const TCHAR* getOnUnitChangeTo() const;

	const TCHAR* getArtDefineTag() const;
	void setArtDefineTag(const TCHAR* szTag);

	int getWorldSoundscapeScriptId() const;

	const TCHAR* getEffectType() const;
	int getEffectProbability() const;

	// Arrays

	int getYieldChange(int i) const; // Exposed to Python
	int getRiverYieldChange(int i) const; // Exposed to Python
	int getHillsYieldChange(int i) const; // Exposed to Python
	int get3DAudioScriptFootstepIndex(int i) const;

	bool isTerrain(int i) const; // Exposed to Python
	int getNumVarieties() const;

	bool canBeSecondary() const;

	// Other
	int getMapCategoryType(int i) const;
	int getNumMapCategoryTypes() const;
	bool isMapCategoryType(int i);

	int getNumAfflictionCommunicabilityTypes() const;
	PromotionLineAfflictionModifier getAfflictionCommunicabilityType(int iPromotionLine, bool bWorkedTile = false, bool bVicinity = false, bool bAccessVolume = false);

	DllExport const CvArtInfoFeature* getArtInfo() const;
	const TCHAR* getButton() const;

	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}

	bool read(CvXMLLoadUtility* pXML);

/************************************************************************************************/
/* Afforess					  Start		 01/31/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	const TCHAR* getGrowthSound() const;
	void setGrowthSound(const TCHAR* szVal);
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

	std::vector<int> m_aiMapCategoryTypes;
public:
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	void copyNonDefaults(CvFeatureInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

	CvPropertyManipulators* getPropertyManipulators();

	std::vector<PromotionLineAfflictionModifier> m_aAfflictionCommunicabilityTypes;
	//	This really belongs on CvInfoBase but you can't change the size of that
	//	object without crashing the core engine :-(
	inline int	getZobristValue() const { return m_zobristValue; }

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

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
	int* m_piHillsYieldChange;
	int* m_pi3DAudioScriptFootstepIndex;

	bool* m_pbTerrain;

	CvPropertyManipulators m_PropertyManipulators;

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

	int getChar() const; // Exposed to Python
	void setChar(int i);
	int getInitialPercent() const; // Exposed to Python
	int getInitialHappiness() const; // Exposed to Python
	int getAIWeightPercent() const; // Exposed to Python

	bool isFlexiblePercent() const; // Exposed to Python

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvCommerceInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

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

	int getChar() const; // Exposed to Python
	void setChar(int i);
	int getHillsChange() const; // Exposed to Python
	int getPeakChange() const; // Exposed to Python
	int getLakeChange() const; // Exposed to Python
	int getCityChange() const; // Exposed to Python
	int getPopulationChangeOffset() const; // Exposed to Python
	int getPopulationChangeDivisor() const; // Exposed to Python
	int getMinCity() const; // Exposed to Python
	int getTradeModifier() const; // Exposed to Python
	int getGoldenAgeYield() const; // Exposed to Python
	int getGoldenAgeYieldThreshold() const; // Exposed to Python
	int getAIWeightPercent() const; // Exposed to Python
	int getColorType() const; // Exposed to Python

	// Arrays

	const TCHAR* getSymbolPath(int i) const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvYieldInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iChar;
	int m_iHillsChange;
	int m_iPeakChange;
	int m_iLakeChange;
	int m_iCityChange;
	int m_iPopulationChangeOffset;
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

	int getMovementCost() const; // Exposed to Python
	int getSeeFromLevel() const; // Exposed to Python
	int getSeeThroughLevel() const; // Exposed to Python
	int getBuildModifier() const; // Exposed to Python
	int getDefenseModifier() const; // Exposed to Python

	bool isWaterTerrain() const; // Exposed to Python
	bool isImpassable() const; // Exposed to Python
	bool isFound() const; // Exposed to Python
	bool isFoundCoast() const; // Exposed to Python
	bool isFoundFreshWater() const; // Exposed to Python

	DllExport const TCHAR* getArtDefineTag() const;
	void setArtDefineTag(const TCHAR* szTag);

	int getWorldSoundscapeScriptId() const;

	// Arrays

	int getYield(int i) const; // Exposed to Python
	int getRiverYieldChange(int i) const; // Exposed to Python
	int getHillsYieldChange(int i) const; // Exposed to Python
	int get3DAudioScriptFootstepIndex(int i) const;

	// Other
	int getMapCategoryType(int i) const;
	int getNumMapCategoryTypes() const;
	bool isMapCategoryType(int i);

	int getNumAfflictionCommunicabilityTypes() const;
	PromotionLineAfflictionModifier getAfflictionCommunicabilityType(int iPromotionLine, bool bWorkedTile = false, bool bVicinity = false, bool bAccessVolume = false);

	const CvArtInfoTerrain* getArtInfo() const;
	const TCHAR* getButton() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvTerrainInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

	CvPropertyManipulators* getPropertyManipulators();

	static TerrainTypes getTerrainPeak()	{ return m_TerrainPeak; }
	static void setTerrainPeak(TerrainTypes eIndex)	{ m_TerrainPeak = eIndex; }
	static TerrainTypes getTerrainHill()	{ return m_TerrainHill; }
	static void setTerrainHill(TerrainTypes eIndex) { m_TerrainHill = eIndex; }

	//	This really belongs on CvInfoBase but you can't change the size of that
	//	object without crashing the core engine :-(
	inline int getZobristValue() const { return m_zobristValue; }

	int getCultureDistance() const;
	int getHealthPercent() const;

	//TB Combat Mod begin
	bool isColdDamage() const;
	//TB Combat Mod end

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	bool m_bWaterTerrain;
	bool m_bImpassable;
	bool m_bFound;
	bool m_bFoundCoast;
	bool m_bFoundFreshWater;
	bool m_bColdDamage; // TB Combat Mods

	int m_iMovementCost;
	int m_iSeeFromLevel;
	int m_iSeeThroughLevel;
	int m_iBuildModifier;
	int m_iDefenseModifier;
	int m_iWorldSoundscapeScriptId;
	int m_iCultureDistance;
	int m_iHealthPercent;

	// Arrays
	int* m_piYields;
	int* m_piRiverYieldChange;
	int* m_piHillsYieldChange;
	int* m_pi3DAudioScriptFootstepIndex;

	std::vector<int> m_aiMapCategoryTypes;

	std::vector<PromotionLineAfflictionModifier> m_aAfflictionCommunicabilityTypes;

	CvPropertyManipulators m_PropertyManipulators;

	static TerrainTypes m_TerrainPeak;
	static TerrainTypes m_TerrainHill;
private:

	CvString m_szArtDefineTag;
	int	m_zobristValue;
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
	void copyNonDefaults(CvInterfaceModeInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

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

	const TCHAR* getTexture() const; // Exposed to Python
	void setTexture(const TCHAR* szVal);
	int getNumCodes() const;
	int getEnableCode(uint uiCode) const;
	int getDisableCode(uint uiCode) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvAdvisorInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

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
	int getWonderConstructRand() const; // Exposed to Python
	int getBaseAttitude() const; // Exposed to Python
	int getBasePeaceWeight() const; // Exposed to Python
	int getPeaceWeightRand() const; // Exposed to Python
	int getWarmongerRespect() const; // Exposed to Python
	int getEspionageWeight() const; // Exposed to Python
	int getRefuseToTalkWarThreshold() const; // Exposed to Python
	int getNoTechTradeThreshold() const; // Exposed to Python
	int getTechTradeKnownPercent() const; // Exposed to Python
	int getMaxGoldTradePercent() const; // Exposed to Python
	int getMaxGoldPerTurnTradePercent() const; // Exposed to Python
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
	int getMaxWarRand() const; // Exposed to Python
	int getMaxWarNearbyPowerRatio() const; // Exposed to Python
	int getMaxWarDistantPowerRatio() const; // Exposed to Python
	int getMaxWarMinAdjacentLandPercent() const; // Exposed to Python
	int getLimitedWarRand() const; // Exposed to Python
	int getLimitedWarPowerRatio() const; // Exposed to Python
	int getDogpileWarRand() const; // Exposed to Python
	int getMakePeaceRand() const; // Exposed to Python
	int getDeclareWarTradeRand() const; // Exposed to Python
	int getDemandRebukedSneakProb() const; // Exposed to Python
	int getDemandRebukedWarProb() const; // Exposed to Python
	int getRazeCityProb() const; // Exposed to Python
	int getBuildUnitProb() const; // Exposed to Python
	int getBaseAttackOddsChange() const; // Exposed to Python
	int getAttackOddsChangeRand() const; // Exposed to Python
	int getWorseRankDifferenceAttitudeChange() const; // Exposed to Python
	int getBetterRankDifferenceAttitudeChange() const; // Exposed to Python
	int getCloseBordersAttitudeChange() const; // Exposed to Python
	int getLostWarAttitudeChange() const; // Exposed to Python
	int getAtWarAttitudeDivisor() const; // Exposed to Python
	int getAtWarAttitudeChangeLimit() const; // Exposed to Python
	int getAtPeaceAttitudeDivisor() const; // Exposed to Python
	int getAtPeaceAttitudeChangeLimit() const; // Exposed to Python
	int getSameReligionAttitudeChange() const; // Exposed to Python
	int getSameReligionAttitudeDivisor() const; // Exposed to Python
	int getSameReligionAttitudeChangeLimit() const; // Exposed to Python
	int getDifferentReligionAttitudeChange() const; // Exposed to Python
	int getDifferentReligionAttitudeDivisor() const; // Exposed to Python
	int getDifferentReligionAttitudeChangeLimit() const; // Exposed to Python
	int getBonusTradeAttitudeDivisor() const; // Exposed to Python
	int getBonusTradeAttitudeChangeLimit() const; // Exposed to Python
	int getOpenBordersAttitudeDivisor() const; // Exposed to Python
	int getOpenBordersAttitudeChangeLimit() const; // Exposed to Python
	int getDefensivePactAttitudeDivisor() const; // Exposed to Python
	int getDefensivePactAttitudeChangeLimit() const; // Exposed to Python
	int getShareWarAttitudeChange() const; // Exposed to Python
	int getShareWarAttitudeDivisor() const; // Exposed to Python
	int getShareWarAttitudeChangeLimit() const; // Exposed to Python
	int getFavoriteCivicAttitudeChange() const; // Exposed to Python
	int getFavoriteCivicAttitudeDivisor() const; // Exposed to Python
	int getFavoriteCivicAttitudeChangeLimit() const; // Exposed to Python
	int getDemandTributeAttitudeThreshold() const; // Exposed to Python
	int getNoGiveHelpAttitudeThreshold() const; // Exposed to Python
	int getTechRefuseAttitudeThreshold() const; // Exposed to Python
	int getStrategicBonusRefuseAttitudeThreshold() const; // Exposed to Python
	int getHappinessBonusRefuseAttitudeThreshold() const; // Exposed to Python
	int getHealthBonusRefuseAttitudeThreshold() const; // Exposed to Python
	int getMapRefuseAttitudeThreshold() const; // Exposed to Python
	int getDeclareWarRefuseAttitudeThreshold() const; // Exposed to Python
	int getDeclareWarThemRefuseAttitudeThreshold() const; // Exposed to Python
	int getStopTradingRefuseAttitudeThreshold() const; // Exposed to Python
	int getStopTradingThemRefuseAttitudeThreshold() const; // Exposed to Python
	int getAdoptCivicRefuseAttitudeThreshold() const; // Exposed to Python
	int getConvertReligionRefuseAttitudeThreshold() const; // Exposed to Python
	int getOpenBordersRefuseAttitudeThreshold() const; // Exposed to Python
	int getDefensivePactRefuseAttitudeThreshold() const; // Exposed to Python
	int getPermanentAllianceRefuseAttitudeThreshold() const; // Exposed to Python
	int getVassalRefuseAttitudeThreshold() const; // Exposed to Python
	int getVassalPowerModifier() const; // Exposed to Python
	int getFavoriteCivic() const; // Exposed to Python
	int getFavoriteReligion() const; // Exposed to Python
	int getFreedomAppreciation() const; // Exposed to Python

	const TCHAR* getArtDefineTag() const; // Exposed to Python
	void setArtDefineTag(const TCHAR* szVal);

	// Arrays

	bool hasTrait(int i) const; // Exposed to Python

	int getFlavorValue(int i) const; // Exposed to Python
	int getContactRand(int i) const; // Exposed to Python
	int getContactDelay(int i) const; // Exposed to Python
	int getMemoryDecayRand(int i) const; // Exposed to Python
	int getMemoryAttitudePercent(int i) const; // Exposed to Python
	int getNoWarAttitudeProb(int i) const; // Exposed to Python
	int getUnitAIWeightModifier(int i) const; // Exposed to Python
	int getImprovementWeightModifier(int i) const; // Exposed to Python
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

	DllExport int getDefaultPlayers() const; // Exposed to Python
	int getUnitNameModifier() const; // Exposed to Python
	int getTargetNumCities() const; // Exposed to Python
	int getNumFreeBuildingBonuses() const; // Exposed to Python
	int getBuildingPrereqModifier() const; // Exposed to Python
	int getMaxConscriptModifier() const; // Exposed to Python
	int getWarWearinessModifier() const; // Exposed to Python
	int getGridWidth() const; // Exposed to Python
	int getGridHeight() const; // Exposed to Python
	int getTerrainGrainChange() const; // Exposed to Python
	int getFeatureGrainChange() const; // Exposed to Python
	int getResearchPercent() const; // Exposed to Python
	int getTradeProfitPercent() const; // Exposed to Python
	int getDistanceMaintenancePercent() const; // Exposed to Python
	int getNumCitiesMaintenancePercent() const; // Exposed to Python
	int getColonyMaintenancePercent() const; // Exposed to Python
	int getCorporationMaintenancePercent() const; // Exposed to Python
	int getNumCitiesAnarchyPercent() const; // Exposed to Python
	int getAdvancedStartPointsMod() const; // Exposed to Python
	// Afforess 02/27/10
	int getCommandersLevelThresholdsPercent() const;

	bool read(CvXMLLoadUtility* pXML);

	int getPercent(int iID) const;

	void copyNonDefaults(CvWorldInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

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
	// Afforess 02/27/10
	int m_iCommandersLevelThresholdsPercent;

	IDValueMapPercent m_Percent;

};
/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvMapInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CvMapInfo : public CvInfoBase
{
public:
	CvMapInfo();
	virtual ~CvMapInfo();

	int getGridWidth() const;
	int getGridHeight() const;
	int getWrapX() const;
	int getWrapY() const;
	const CvString getInitialWBMap() const;
	const CvString getMapScript() const;
	/*bool isTechShareMap(MapTypes eMap) const;*/
	const TCHAR* getStartingEvent() const;
	int getSwitchType() const;

	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);
	bool readPass3();

protected:
	int m_iGridWidth;
	int m_iGridHeight;
	int m_iWrapX;
	int m_iWrapY;
	CvString m_szInitialWBMap;
	CvString m_szMapScript;
	/*bool* m_pabTechShareMaps;*/
	CvString m_szStartingEvent;
	int m_iSwitchType;
};

class CvMapSwitchInfo : public CvInfoBase
{
public:
	CvMapSwitchInfo();
	virtual ~CvMapSwitchInfo();

	int getMap() const;
	const TCHAR* getEvent() const;
	bool isManual() const;

	bool read(CvXMLLoadUtility* pXML);

protected:
	int m_iMap;
	CvString m_szEvent;
	bool m_bManual;
};

/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvClimateInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvClimateInfo :	public CvInfoBase
{
public:

	CvClimateInfo();
	virtual ~CvClimateInfo();

	int getDesertPercentChange() const; // Exposed to Python
	int getJungleLatitude() const; // Exposed to Python
	int getHillRange() const; // Exposed to Python
	int getPeakPercent() const; // Exposed to Python

	float getSnowLatitudeChange() const; // Exposed to Python
	float getTundraLatitudeChange() const; // Exposed to Python
	float getGrassLatitudeChange() const; // Exposed to Python
	float getDesertBottomLatitudeChange() const; // Exposed to Python
	float getDesertTopLatitudeChange() const; // Exposed to Python
	float getIceLatitude() const; // Exposed to Python
	float getRandIceLatitude() const; // Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvClimateInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

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

	int getSeaLevelChange() const; // Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvSeaLevelInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

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

	int getTechPrereq() const; // Exposed to Python

	// Arrays

	int getProductionToCommerceModifier(int i) const; // Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvProcessInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iTechPrereq;

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

	int getPopulationThreshold() const; // Exposed to Python
	int getStateReligionVotePercent() const; // Exposed to Python
	int getTradeRoutes() const; // Exposed to Python
	int getMinVoters() const; // Exposed to Python

	bool isSecretaryGeneral() const; // Exposed to Python
	bool isVictory() const; // Exposed to Python
	bool isFreeTrade() const; // Exposed to Python
	bool isNoNukes() const; // Exposed to Python
	bool isCityVoting() const; // Exposed to Python
	bool isCivVoting() const; // Exposed to Python
	bool isDefensivePact() const; // Exposed to Python
	bool isOpenBorders() const; // Exposed to Python
	bool isForcePeace() const; // Exposed to Python
	bool isForceNoTrade() const; // Exposed to Python
	bool isForceWar() const; // Exposed to Python
	bool isAssignCity() const; // Exposed to Python

	// Arrays

	bool isForceCivic(int i) const; // Exposed to Python
	bool isVoteSourceType(int i) const; // Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvVoteInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

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

	int getVictoryPrereq() const; // Exposed to Python
	int getTechPrereq() const; // Exposed to Python
	int getAnyoneProjectPrereq() const; // Exposed to Python
	void setAnyoneProjectPrereq(int i);
	int getMaxGlobalInstances() const; // Exposed to Python
	int getMaxTeamInstances() const; // Exposed to Python
	int getProductionCost() const; // Exposed to Python
	int getNukeInterception() const; // Exposed to Python
	int getTechShare() const; // Exposed to Python
	//DPII < Maintenance Modifier >
	int getGlobalMaintenanceModifier() const;
	int getDistanceMaintenanceModifier() const;
	int getNumCitiesMaintenanceModifier() const;
	int getConnectedCityMaintenanceModifier() const;
	//DPII < Maintenance Modifier >
	int getEveryoneSpecialUnit() const; // Exposed to Python
	int getEveryoneSpecialBuilding() const; // Exposed to Python
	int getVictoryDelayPercent() const; // Exposed to Python
	int getSuccessRate() const; // Exposed to Python

	bool isSpaceship() const; // Exposed to Python
	bool isAllowsNukes() const; // Exposed to Python
	const char* getMovieArtDef() const; // Exposed to Python

	const TCHAR* getCreateSound() const; // Exposed to Python
	void setCreateSound(const TCHAR* szVal);

	// Arrays

	int getBonusProductionModifier(int i) const; // Exposed to Python
	int getVictoryThreshold(int i) const; // Exposed to Python
	int getVictoryMinThreshold(int i) const; // Exposed to Python
	int getProjectsNeeded(int i) const; // Exposed to Python

	// Vectors
	int getMapCategoryType(int i) const;
	int getNumMapCategoryTypes() const;
	bool isMapCategoryType(int i);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvProjectInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

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
	bool m_bTechShareWithHalfCivs;
	int* m_piCommerceModifier;
	std::vector<CvString> m_aszProjectsNeededforPass3;
	std::vector<int> m_aiProjectsNeededforPass3;


	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iVictoryPrereq;
	int m_iTechPrereq;
	int m_iAnyoneProjectPrereq;
	int m_iMaxGlobalInstances;
	int m_iMaxTeamInstances;
	int m_iProductionCost;
	int m_iNukeInterception;
	int m_iTechShare;
	//DPII < Maintenance Modifiers >
	int m_iGlobalMaintenanceModifier;
	int m_iDistanceMaintenanceModifier;
	int m_iNumCitiesMaintenanceModifier;
	int m_iConnectedCityMaintenanceModifier;
	//DPII < Maintenance Modifiers >
	int m_iEveryoneSpecialUnit;
	int m_iEveryoneSpecialBuilding;
	int m_iVictoryDelayPercent;
	int m_iSuccessRate;

	bool m_bSpaceship;
	bool m_bAllowsNukes;

	CvString m_szCreateSound;
	CvString m_szMovieArtDef;

	// Arrays

	int* m_piBonusProductionModifier;
	int* m_piVictoryThreshold;
	int* m_piVictoryMinThreshold;
	int* m_piProjectsNeeded;

	// Vectors
	std::vector<int> m_aiMapCategoryTypes;
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

	int getChar() const; // Exposed to Python
	// TGA_INDEXATION 01/21/08 MRGENIE
	int getTGAIndex() const;
	void setTGAIndex(int i);

	void setChar(int i);
	int getHolyCityChar() const; // Exposed to Python
	void setHolyCityChar(int i);
	int getTechPrereq() const; // Exposed to Python
	int getFreeUnit() const; // Exposed to Python
	int getNumFreeUnits() const; // Exposed to Python
	int getSpreadFactor() const; // Exposed to Python
	int getMissionType() const; // Exposed to Python
	void setMissionType(int iNewType);

	const TCHAR* getTechButton() const; // Exposed to Python
	void setTechButton(const TCHAR* szVal);
	const TCHAR* getGenericTechButton() const; // Exposed to Python
	void setGenericTechButton(const TCHAR* szVal);
	const TCHAR* getMovieFile() const; // Exposed to Python
	void setMovieFile(const TCHAR* szVal);
	const TCHAR* getMovieSound() const; // Exposed to Python
	void setMovieSound(const TCHAR* szVal);
	const TCHAR* getSound() const; // Exposed to Python
	void setSound(const TCHAR* szVal);

	const TCHAR* getButtonDisabled() const;		//	Exposed to Python

	void setAdjectiveKey(const TCHAR* szVal);
	const wchar* getAdjectiveKey() const; // Exposed to Python
	std::wstring pyGetAdjectiveKey() const { return getAdjectiveKey(); } // Exposed to Python

	// Arrays

	int getGlobalReligionCommerce(int i) const; // Exposed to Python
	int* getGlobalReligionCommerceArray() const;
	int getHolyCityCommerce(int i) const; // Exposed to Python
	int* getHolyCityCommerceArray() const;
	int getStateReligionCommerce(int i) const; // Exposed to Python
	int* getStateReligionCommerceArray() const;
	int getFlavorValue(int i) const; // Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	bool readPass3();

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

	int getChar() const; // Exposed to Python
	// TGA_INDEXATION 01/21/08 MRGENIE
	int getTGAIndex() const;
	void setTGAIndex(int i);

	void setChar(int i);
	int getHeadquarterChar() const; // Exposed to Python
	void setHeadquarterChar(int i);
	int getTechPrereq() const; // Exposed to Python
	int getFreeUnit() const; // Exposed to Python
	int getSpreadFactor() const; // Exposed to Python
	int getSpreadCost() const; // Exposed to Python
	int getMaintenance() const; // Exposed to Python
	int getMissionType() const; // Exposed to Python
	void setMissionType(int iNewType);

	int getBonusProduced() const; // Exposed to Python

	const TCHAR* getMovieFile() const; // Exposed to Python
	void setMovieFile(const TCHAR* szVal);
	const TCHAR* getMovieSound() const; // Exposed to Python
	void setMovieSound(const TCHAR* szVal);
	const TCHAR* getSound() const; // Exposed to Python
	void setSound(const TCHAR* szVal);

	// Arrays

	int getPrereqBonus(int i) const; // Exposed to Python
	int getHeadquarterCommerce(int i) const; // Exposed to Python
	int* getHeadquarterCommerceArray() const;
	int getCommerceProduced(int i) const; // Exposed to Python
	int* getCommerceProducedArray() const;
	int getYieldProduced(int i) const; // Exposed to Python
	int* getYieldProducedArray() const;

	bool read(CvXMLLoadUtility* pXML);

	int getObsoleteTech() const;
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
	int m_iFreeUnit;
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

	int* m_paiPrereqBuilding;
	std::vector<CvString> m_aszPrereqBuildingforPass3;
	std::vector<int> m_aiPrereqBuildingforPass3;

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

	int getHealth() const; // Exposed to Python
	int getHappiness() const; // Exposed to Python
	int getMaxAnarchy() const; // Exposed to Python
	int getUpkeepModifier() const; // Exposed to Python
	int getLevelExperienceModifier() const; // Exposed to Python
	int getGreatPeopleRateModifier() const; // Exposed to Python
	int getGreatGeneralRateModifier() const; // Exposed to Python
	int getDomesticGreatGeneralRateModifier() const; // Exposed to Python
	int getMaxGlobalBuildingProductionModifier() const; // Exposed to Python
	int getMaxTeamBuildingProductionModifier() const; // Exposed to Python
	int getMaxPlayerBuildingProductionModifier() const; // Exposed to Python

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		RevTraitEffects															*/
/********************************************************************************/
	int getRevIdxLocal() const; // Exposed to Python
	int getRevIdxNational() const; // Exposed to Python
	int getRevIdxDistanceModifier() const; // Exposed to Python
	int getRevIdxHolyCityGood() const; // Exposed to Python
	int getRevIdxHolyCityBad() const; // Exposed to Python
	float getRevIdxNationalityMod() const; // Exposed to Python
	float getRevIdxBadReligionMod() const; // Exposed to Python
	float getRevIdxGoodReligionMod() const; // Exposed to Python
	bool isNonStateReligionCommerce() const; // Exposed to Python
	bool isUpgradeAnywhere() const; // Exposed to Python
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

	const TCHAR* getShortDescription() const; // Exposed to Python
	void setShortDescription(const TCHAR* szVal);

	// Arrays

	int getExtraYieldThreshold(int i) const; // Exposed to Python
	int getTradeYieldModifier(int i) const; // Exposed to Python
	int getCommerceChange(int i) const; // Exposed to Python
	int getCommerceModifier(int i) const; // Exposed to Python
/*
	int isFreePromotion(int i) const; // Exposed to Python
	int isFreePromotionUnitCombat(int i) const;
*/

	//TB Traits Mods begin
	//Textual References
	int getPrereqTrait() const;
	int getPrereqOrTrait1() const;
	int getPrereqOrTrait2() const;
	PromotionLineTypes getPromotionLine() const;
	int getGreatPeopleUnitType() const;
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
	bool isValidTrait(bool bGameStart=false) const;

	// bool vector without delayed resolution
	int getNotOnGameOption(int i) const;
	int getNumNotOnGameOptions() const;
	bool isNotOnGameOption(int i) const;

	int getOnGameOption(int i) const;
	int getNumOnGameOptions() const;
	bool isOnGameOption(int i) const;

	//Arrays
	int getSpecialistYieldChange(int i, int j) const; // Exposed to Python
	int* getSpecialistYieldChangeArray(int i) const;
	bool m_bAnySpecialistYieldChanges;

	int getYieldModifier(int i) const; // Exposed to Python
	int* getYieldModifierArray() const;

	int getCapitalYieldModifier(int i) const; // Exposed to Python
	int* getCapitalYieldModifierArray() const;

	int getCapitalCommerceModifier(int i) const; // Exposed to Python
	int* getCapitalCommerceModifierArray() const;

	int getSpecialistExtraCommerce(int i) const; // Exposed to Python
	int* getSpecialistExtraCommerceArray() const;

	int getSpecialistExtraYield(int i) const; // Exposed to Python
	int* getSpecialistExtraYieldArray() const;

	int getYieldChange(int i) const; // Exposed to Python

	int getSpecialistCommerceChange(int i, int j) const; // Exposed to Python
	int* getSpecialistCommerceChangeArray(int i) const;
	bool m_bAnySpecialistCommerceChanges;

	int getFlavorValue(int i) const;
	int getLessYieldThreshold(int i) const;

	int getSeaPlotYieldChanges(int i) const;
	int* getSeaPlotYieldChangesArray() const;

	int getImprovementYieldChange(int i, int j) const; // Exposed to Python
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
	int m_iGreatPeopleUnitType;
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

	DllExport const TCHAR* getPath(); // Exposed to Python
	void setPath(const TCHAR* szVal);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvCursorInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

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

	DllExport const TCHAR* getFileName();
	void setFileName(const TCHAR* szVal);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvThroneRoomCamera* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

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

	DllExport const TCHAR* getEvent();
	void setEvent(const TCHAR* szVal);
	DllExport const TCHAR* getNodeName();
	void setNodeName(const TCHAR* szVal);
	DllExport int getFromState();
	void setFromState(int iVal);
	DllExport int getToState();
	void setToState(int iVal);
	DllExport int getAnimation();
	void setAnimation(int iVal);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvThroneRoomInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

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

	DllExport const TCHAR* getArtStyleType();
	void setArtStyleType(const TCHAR* szVal);
	DllExport const TCHAR* getEraType();
	void setEraType(const TCHAR* szVal);
	DllExport const TCHAR* getFileName();
	void setFileName(const TCHAR* szVal);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvThroneRoomStyleInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

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

	DllExport const TCHAR* getPath();
	void setPath(const TCHAR* szVal);
	DllExport const TCHAR* getTransitionType();
	void setTransitionType(const TCHAR* szVal);
	DllExport float getStartTime();
	void setStartTime(float fVal);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvSlideShowInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

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

	DllExport const TCHAR* getPath();
	void setPath(const TCHAR* szVal);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvSlideShowRandomInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

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

	DllExport const TCHAR* getMapName();
	void setMapName(const TCHAR* szVal);
	DllExport const TCHAR* getModelFile();
	void setModelFile(const TCHAR* szVal);
	DllExport int getNumSizes();
	DllExport float getSize(int index);
	DllExport int getNumClimates();
	DllExport const TCHAR* getClimatePath(int index);
	DllExport int getNumWaterLevelDecals();
	DllExport const TCHAR* getWaterLevelDecalPath(int index);
	DllExport int getNumWaterLevelGloss();
	DllExport const TCHAR* getWaterLevelGlossPath(int index);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvWorldPickerInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

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

	DllExport const TCHAR* getNodeName();
	void setNodeName(const TCHAR* szVal);
	DllExport const TCHAR* getProjectName();
	void setProjectName(const TCHAR* szVal);
	DllExport ProjectTypes getProjectType();
	DllExport AxisTypes getCameraUpAxis();
	DllExport SpaceShipInfoTypes getSpaceShipInfoType();
	DllExport int getPartNumber();
	DllExport int getArtType();
	DllExport int getEventCode();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvSpaceShipInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

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
		DllExport CvAnimationPathDefinition * getPath( );
		DllExport bool isMissionPath() const;

		bool read(CvXMLLoadUtility* pXML);
		void copyNonDefaults(CvAnimationPathInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

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
		void copyNonDefaults(CvAnimationCategoryInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

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

		// serialization
		void read(FDataStreamBase* pStream) {}
		void write(FDataStreamBase* pStream) {}

		bool read(CvXMLLoadUtility* pXML);
		void copyNonDefaults(CvEntityEventInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

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

	const TCHAR* getTag() const; // Exposed to Python
	void setTag(const TCHAR* szDesc); // Exposed to Python

	DllExport const TCHAR* getPath() const; // Exposed to Python
	void setPath(const TCHAR* szDesc); // Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvAssetInfoBase* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szPath;

};

class CvArtInfoAsset : 	public CvAssetInfoBase
{
public:

	CvArtInfoAsset() {}
	virtual ~CvArtInfoAsset() {}

	DllExport const TCHAR* getNIF() const; // Exposed to Python
	DllExport const TCHAR* getKFM() const; // Exposed to Python

	void setNIF(const TCHAR* szDesc); // Exposed to Python
	void setKFM(const TCHAR* szDesc); // Exposed to Python

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvArtInfoAsset* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

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

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvArtInfoScalableAsset* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

};

// todoJS: Remove empty classes if additional items are not added

class CvArtInfoInterface : 	public CvArtInfoAsset
{
public:

	CvArtInfoInterface() {}
	virtual ~CvArtInfoInterface() {}

};

class CvArtInfoMisc : 	public CvArtInfoScalableAsset
{
public:

	CvArtInfoMisc() {}
	virtual ~CvArtInfoMisc() {}

};

class CvArtInfoMovie : 	public CvArtInfoAsset
{
public:

	CvArtInfoMovie() {}
	virtual ~CvArtInfoMovie() {}

	bool read(CvXMLLoadUtility* pXML);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvArtInfoMovie* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
};

class CvArtInfoUnit : public CvArtInfoScalableAsset
{
public:

	CvArtInfoUnit();
	virtual ~CvArtInfoUnit();

	DllExport bool getActAsRanged() const;
	DllExport bool getActAsLand() const;
	DllExport bool getActAsAir() const;

	DllExport const TCHAR* getShaderNIF() const;
	void setShaderNIF(const TCHAR* szDesc);

	DllExport const TCHAR* getShadowNIF() const;
	DllExport float getShadowScale() const;
	DllExport const TCHAR* getShadowAttachNode() const;
	DllExport int getDamageStates() const;

	DllExport const TCHAR* getTrailTexture() const;
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

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvArtInfoUnit* pClassInfo, CvXMLLoadUtility* pXML);

	const TCHAR* getTrainSound() const;
	void setTrainSound(const TCHAR* szVal);
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

	bool isAnimated() const; // Exposed to Python
	DllExport const TCHAR* getLSystemName() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvArtInfoBuilding* pClassInfo, CvXMLLoadUtility* pXML);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

protected:

	bool m_bAnimated;
	CvString m_szLSystemName;

};

class CvArtInfoCivilization : public CvArtInfoAsset
{
public:

	CvArtInfoCivilization();
	virtual ~CvArtInfoCivilization();

	bool isWhiteFlag() const; // Exposed to Python

	bool read(CvXMLLoadUtility* pXML);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvArtInfoCivilization* pClassInfo, CvXMLLoadUtility* pXML);

protected:

	bool m_bWhiteFlag;

};

class CvArtInfoLeaderhead : public CvArtInfoAsset
{
public:

	CvArtInfoLeaderhead() {}
	virtual ~CvArtInfoLeaderhead() {}

	DllExport const TCHAR* getNoShaderNIF() const;
	void setNoShaderNIF(const TCHAR* szNIF);
	DllExport const TCHAR* getBackgroundKFM() const;
	void setBackgroundKFM( const TCHAR* szKFM);

	bool read(CvXMLLoadUtility* pXML);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvArtInfoLeaderhead* pClassInfo, CvXMLLoadUtility* pXML);

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

	DllExport const TCHAR* getShaderNIF() const;
	void setShaderNIF(const TCHAR* szDesc);

	bool read(CvXMLLoadUtility* pXML);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvArtInfoBonus* pClassInfo, CvXMLLoadUtility* pXML);

protected:
	CvString m_szShaderNIF;		//!< The NIF used if the graphics card supports shaders
	int m_iFontButtonIndex;
};

class CvArtInfoImprovement : public CvArtInfoScalableAsset
{
public:

	CvArtInfoImprovement();
	virtual ~CvArtInfoImprovement();

	DllExport const TCHAR* getShaderNIF() const;
	void setShaderNIF(const TCHAR* szDesc);

	bool isExtraAnimations() const; // Exposed to Python

	bool read(CvXMLLoadUtility* pXML);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvArtInfoImprovement* pClassInfo, CvXMLLoadUtility* pXML);

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

	DllExport const TCHAR* getBaseTexture();
	void setBaseTexture(const TCHAR* szTmp);
	DllExport const TCHAR* getGridTexture();
	void setGridTexture(const TCHAR* szTmp);
	DllExport const TCHAR* getDetailTexture();
	void setDetailTexture(const TCHAR* szTmp);
	DllExport int getLayerOrder();
	DllExport bool useAlphaShader();
	DllExport CvTextureBlendSlotList& getBlendList(int blendMask);

	bool read(CvXMLLoadUtility* pXML);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvArtInfoTerrain* pClassInfo, CvXMLLoadUtility* pXML);

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

	DllExport bool isAnimated() const; // Exposed to Python
	DllExport bool isRiverArt() const; // Exposed to Python
	DllExport TileArtTypes getTileArtType() const;
	DllExport LightTypes getLightType() const;

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	void dump();

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvArtInfoFeature* pClassInfo, CvXMLLoadUtility* pXML);

	class FeatureArtModel
	{
	public:
		FeatureArtModel(const CvString &modelFile, RotationTypes rotation)
			: m_szModelFile(modelFile)
			, m_eRotation(rotation)
		{}

		const CvString &getModelFile() const
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

		const FeatureArtModel &getArtModel(int index) const
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

		const CvString &getVarietyButton() const
		{
			return m_szVarietyButton;
		}

		const FeatureArtPiece &getFeatureArtPiece(int index) const
		{
			FAssertMsg((index >= 0) && (index < (int) m_aFeatureArtPieces.size()), "[Jason] Invalid feature art index.");
			return m_aFeatureArtPieces[index];
		}

		const FeatureArtPiece &getFeatureArtPieceFromConnectionMask(int connectionMask) const
		{
			for(int i=0;i<(int)m_aFeatureArtPieces.size();i++)
				if(m_aFeatureArtPieces[i].getConnectionMask() == connectionMask)
					return m_aFeatureArtPieces[i];

			FAssertMsg(false, "[Jason] Failed to find feature art piece with valid connection mask.");
			return m_aFeatureArtPieces[0];
		}

		const CvString getFeatureDummyNodeName(const CvString &tagName) const
		{
			for(int i=0;i<(int)m_aFeatureDummyNodes.size();i++)
			{
				if(m_aFeatureDummyNodes[i].getTagName().CompareNoCase(tagName) == 0)
					return m_aFeatureDummyNodes[i].getNodeName();
			}

			FAssertMsg(false, "[Jason] Failed to find dummy tag name.");
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

		FeatureArtPiece &createFeatureArtPieceFromConnectionMask(int connectionMask)
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
	std::string getFeatureDummyNodeName(int variety, std::string tagName);

protected:

	int getConnectionMaskFromString(const CvString &connectionString);
	int getRotatedConnectionMask(int connectionMask, RotationTypes rotation);

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

	bool isAvoidGrowth() const; // Exposed to Python
	bool isGreatPeople() const; // Exposed to Python
	// Afforess 02/10/10
	bool isAvoidAngryCitizens() const; // Exposed to Python
	bool isAvoidUnhealthyCitizens() const; // Exposed to Python

	// Arrays
	int getYieldChange(int i) const; // Exposed to Python
	int getCommerceChange(int i) const; // Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvEmphasizeInfo* pClassInfo, CvXMLLoadUtility* pXML);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	bool m_bAvoidGrowth;
	bool m_bGreatPeople;
	// Afforess 02/10/10
	bool m_bAvoidAngryCitizens;
	bool m_bAvoidUnhealthyCitizens;

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

	int getPopulationPercent() const;		//	Exposed to Python
	int getCityPercent() const;				//	Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvUpkeepInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

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

	int getCityDefenseModifier() const; // Exposed to Python

	// JOOYO_ADDON, Added by Jooyo, 06/17/09
	int getCityRadius() const; // Exposed to Python
	int getMaxWorldWonders() const;
	int getMaxTeamWonders() const;
	int getMaxNationalWonders() const;
	int getMaxNationalWondersOCC() const;
	int getPrereqGameOption() const;

	int getSpeedThreshold(int i) const; // Exposed to Python

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvCultureLevelInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

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

	int getStartingUnitMultiplier() const; // Exposed to Python
	int getStartingDefenseUnits() const; // Exposed to Python
	int getStartingWorkerUnits() const; // Exposed to Python
	int getStartingExploreUnits() const; // Exposed to Python
	int getAdvancedStartPoints() const; // Exposed to Python
	int getStartingGold() const; // Exposed to Python
	int getFreePopulation() const; // Exposed to Python
	int getStartPercent() const; // Exposed to Python
	int getGrowthPercent() const; // Exposed to Python
	int getTrainPercent() const; // Exposed to Python
	int getConstructPercent() const; // Exposed to Python
	int getCreatePercent() const; // Exposed to Python
	int getResearchPercent() const; // Exposed to Python
	int getBuildPercent() const; // Exposed to Python
	int getImprovementPercent() const; // Exposed to Python
	int getGreatPeoplePercent() const; // Exposed to Python
	int getAnarchyPercent() const; // Exposed to Python
	int getEventChancePerTurn() const; // Exposed to Python
	int getSoundtrackSpace() const; // Exposed to Python
	int getNumSoundtracks() const; // Exposed to Python
	int getBeelineStingsTechCostModifier() const; // Exposed to Python
	// Afforess 09/12/10
	int getInitialCityMaintenancePercent() const;

	const TCHAR* getAudioUnitVictoryScript() const; // Exposed to Python
	const TCHAR* getAudioUnitDefeatScript() const; // Exposed to Python

	bool isNoGoodies() const; // Exposed to Python
	bool isNoAnimals() const; // Exposed to Python
	bool isNoBarbUnits() const; // Exposed to Python
	bool isNoBarbCities() const; // Exposed to Python
	bool isFirstSoundtrackFirst() const; // Exposed to Python

	// Arrays

	int getSoundtracks(int i) const;
	int getCitySoundscapeSciptId(int i) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvEraInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

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
	// Afforess 09/12/10
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

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvColorInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

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

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvPlayerColorInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

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

		const TCHAR* getSkyArt();
		void setSkyArt(const TCHAR* szPath);
		const TCHAR* getHeightMap();
		void setHeightMap(const TCHAR* szPath);
		const TCHAR* getTerrainMap();
		void setTerrainMap(const TCHAR* szPath);
		const TCHAR* getNormalMap();
		void setNormalMap(const TCHAR* szPath);
		const TCHAR* getBlendMap();
		void setBlendMap(const TCHAR* szPath);

		void read(FDataStreamBase* stream) {}
		void write(FDataStreamBase* stream) {}

		bool read(CvXMLLoadUtility* pXML);
		void copyNonDefaults(CvLandscapeInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

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

	const wchar* getText() const;
	void setText(const wchar* szText);

	// for Python
	std::wstring pyGetText() const { return getText(); }

	void setGender(const wchar* szGender) { m_szGender = szGender;	}
	const wchar* getGender() const { return m_szGender; }

	void setPlural(const wchar* szPlural) { m_szPlural = szPlural; }
	const wchar* getPlural() const { return m_szPlural; }

	DllExport int getNumLanguages() const; // not static for Python access
	DllExport void setNumLanguages(int iNum); // not static for Python access

	bool read(CvXMLLoadUtility* pXML);

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

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

		void read(FDataStreamBase* stream) {}
		void write(FDataStreamBase* stream) {}

		int m_iNumDiplomacyText;
		bool* m_pbCivilizationTypes;
		bool* m_pbLeaderHeadTypes;
		bool* m_pbAttitudeTypes;
		bool* m_pbDiplomacyPowerTypes;
		CvString* m_paszDiplomacyText;	// needs to be public for xml load assignment
	};

	CvDiplomacyTextInfo();
	virtual ~CvDiplomacyTextInfo() { uninit(); }	// free memory - MT

	// note - Response member vars allocated by CvXmlLoadUtility
	void init(int iNum);
	void uninit();

	const Response& getResponse(int iNum) const { return m_pResponses[iNum]; } // Exposed to Python
	int getNumResponses() const; // Exposed to Python

	bool getCivilizationTypes(int i, int j) const; // Exposed to Python
	bool getLeaderHeadTypes(int i, int j) const; // Exposed to Python
	bool getAttitudeTypes(int i, int j) const; // Exposed to Python
	bool getDiplomacyPowerTypes(int i, int j) const; // Exposed to Python

	int getNumDiplomacyText(int i) const; // Exposed to Python

	const TCHAR* getDiplomacyText(int i, int j) const; // Exposed to Python

	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}
	bool read(CvXMLLoadUtility* pXML);

private:

	int m_iNumResponses;			// set by init
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

	DllExport const TCHAR* getPath() const { return m_szPath; }
	void setPath(const TCHAR* szVal) { m_szPath = szVal; }
	float getUpdateRate( ) const { return m_fUpdateRate; };
	void setUpdateRate( float fUpdateRate ) { m_fUpdateRate = fUpdateRate; }
	bool isProjectile() const { return m_bProjectile; };
	float getProjectileSpeed() const { return m_fProjectileSpeed; };
	float getProjectileArc() const { return m_fProjectileArc; };
	bool isSticky() const { return m_bSticky; };
	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvEffectInfo* pClassInfo, CvXMLLoadUtility* pXML);

/************************************************************************************************/
/* RevolutionDCM					  Start		 05/31/10						Afforess	   */
/*																							  */
/* Battle Effects																			   */
/************************************************************************************************/
	bool isBattleEffect() const;
/************************************************************************************************/
/* RevolutionDCM				 Battle Effects END											 */
/************************************************************************************************/
	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}

private:
	CvString m_szPath;
	float m_fUpdateRate;
	bool m_bProjectile;
	bool m_bSticky;
	float m_fProjectileSpeed;
	float m_fProjectileArc;

/************************************************************************************************/
/* RevolutionDCM					  Start		 05/31/10						Afforess	   */
/*																							  */
/* Battle Effects																			   */
/************************************************************************************************/
	bool m_bBattleEffect;
/************************************************************************************************/
/* RevolutionDCM				 Battle Effects END											 */
/************************************************************************************************/
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

	DllExport const TCHAR* getPath() const { return m_szPath; }
	void setPath(const TCHAR* szVal) { m_szPath = szVal; }

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvAttachableInfo* pClassInfo, CvXMLLoadUtility* pXML);

private:
	CvString m_szPath;
	float m_fUpdateRate;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvQuestInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvQuestInfo :
	public CvInfoBase
{
public:
	struct QuestLink
	{
		// Stores the QuestLinks Type and Name
		QuestLink() :
		m_szQuestLinkType("No Type"),
		m_szQuestLinkName("No Name")
		{
		}

	CvString m_szQuestLinkType;
	CvString m_szQuestLinkName;
	};

	CvQuestInfo();
	virtual ~CvQuestInfo();

	void reset();
	bool initQuestLinks(int iNum);

	int getNumQuestMessages() const;
	int getNumQuestLinks() const;
	int getNumQuestSounds() const;
	const TCHAR* getQuestObjective() const;
	const TCHAR* getQuestBodyText() const;
	const TCHAR* getQuestMessages(int iIndex) const;
	const TCHAR* getQuestLinkType(int iIndex) const;
	const TCHAR* getQuestLinkName(int iIndex) const;
	const TCHAR* getQuestSounds(int iIndex) const;
	const TCHAR* getQuestScript() const;

	void setNumQuestMessages(int iNum);
	void setNumQuestSounds(int iNum);
	void setQuestObjective(const TCHAR* szText);
	void setQuestBodyText(const TCHAR* szText);
	void setQuestMessages(int iIndex, const TCHAR* szText);
	void setQuestSounds(int iIndex, const TCHAR* szText);
	void setQuestScript(const TCHAR* szText);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvQuestInfo* pClassInfo, CvXMLLoadUtility* pXML);

private:
	int m_iNumQuestMessages;
	int m_iNumQuestLinks;
	int m_iNumQuestSounds;

	CvString m_szQuestObjective;
	CvString m_szQuestBodyText;
	CvString m_szQuestScript;

	CvString* m_paszQuestMessages;
	QuestLink* m_pQuestLinks;
	CvString* m_paszQuestSounds;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvTutorialInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTutorialMessage
{
public:
	CvTutorialMessage();
	virtual ~CvTutorialMessage();

	const TCHAR* getText() const;
	const TCHAR* getImage() const;
	const TCHAR* getSound() const;

	void setText(const TCHAR* szText);
	void setImage(const TCHAR* szText);
	void setSound(const TCHAR* szText);
	void setScript(int iIndex, const TCHAR* szText);

	int getNumTutorialScripts() const;
	const TCHAR* getTutorialScriptByIndex(int i) const;
	bool read(CvXMLLoadUtility* pXML);

private:
	int m_iNumTutorialScripts;
	CvString m_szTutorialMessageText;
	CvString m_szTutorialMessageImage;
	CvString m_szTutorialMessageSound;
	CvString* m_paszTutorialScripts;
};

class CvTutorialMessage;
class CvTutorialInfo :
	public CvInfoBase
{
public:
	CvTutorialInfo();
	virtual ~CvTutorialInfo();

	const TCHAR* getNextTutorialInfoType() const;
	void setNextTutorialInfoType(const TCHAR* szVal);

	bool initTutorialMessages(int iNum);
	void resetMessages();

	int getNumTutorialMessages() const;
	const CvTutorialMessage* getTutorialMessage(int iIndex) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvTutorialInfo* pClassInfo, CvXMLLoadUtility* pXML);

private:
	CvString m_szNextTutorialInfoType;
	int m_iNumTutorialMessages;
	CvTutorialMessage* m_paTutorialMessages;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvGameOptionInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameOptionInfo :
	public CvInfoBase
{
public:
	CvGameOptionInfo();
	virtual ~CvGameOptionInfo();

	DllExport bool getDefault() const;
	DllExport bool getVisible() const;

	//TB Tags
	// int vector utilizing struct with delayed resolution
	int getNumEnforcesGameOptionOnTypes() const;
	GameOptionTypeBool& isEnforcesGameOptionOnType(int iOption);

	int getNumEnforcesGameOptionOffTypes() const;
	GameOptionTypeBool& isEnforcesGameOptionOffType(int iOption);

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvGameOptionInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

private:
	bool m_bDefault;
	bool m_bVisible;
	//TB Tags
	// int vector utilizing struct with delayed resolution
	std::vector<GameOptionTypeBool> m_aEnforcesGameOptionOnTypes;
	std::vector<GameOptionTypeBool> m_aEnforcesGameOptionOffTypes;
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

	void copyNonDefaults(CvMPOptionInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

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

	void copyNonDefaults(CvForceControlInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

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

	void copyNonDefaults(CvPlayerOptionInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

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

	void copyNonDefaults(CvGraphicOptionInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

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

	int getPercentGamesActive() const; // Exposed to Python
	int getProbability() const; // Exposed to Python
	int getNumUnits() const; // Exposed to Python
	int getNumBuildings() const; // Exposed to Python
	int getNumUnitsGlobal() const; // Exposed to Python
	int getNumBuildingsGlobal() const; // Exposed to Python
	int getNumPlotsRequired() const; // Exposed to Python
	int getPlotType() const; // Exposed to Python
	int getNumReligions() const; // Exposed to Python
	int getNumCorporations() const; // Exposed to Python
	int getOtherPlayerShareBorders() const; // Exposed to Python
	int getOtherPlayerHasTech() const; // Exposed to Python
	int getCityFoodWeight() const; // Exposed to Python
	int getCivic() const; // Exposed to Python
	int getMinPopulation() const; // Exposed to Python
	int getMaxPopulation() const; // Exposed to Python
	int getMinMapLandmass() const; // Exposed to Python
	int getMinOurLandmass() const; // Exposed to Python
	int getMaxOurLandmass() const; // Exposed to Python
	int getMinDifficulty() const; // Exposed to Python
	int getAngry() const; // Exposed to Python
	int getUnhealthy() const; // Exposed to Python
	int getUnitDamagedWeight() const; // Exposed to Python
	int getUnitDistanceWeight() const; // Exposed to Python
	int getUnitExperienceWeight() const; // Exposed to Python
	int getMinTreasury() const; // Exposed to Python

	int getBuildingRequired(int i) const; // Exposed to Python
	int getNumBuildingsRequired() const; // Exposed to Python
	int getUnitRequired(int i) const; // Exposed to Python
	int getNumUnitsRequired() const; // Exposed to Python
	int getPrereqOrTechs(int i) const; // Exposed to Python
	int getNumPrereqOrTechs() const; // Exposed to Python
	int getPrereqAndTechs(int i) const; // Exposed to Python
	int getNumPrereqAndTechs() const; // Exposed to Python
	int getObsoleteTech(int i) const; // Exposed to Python
	int getNumObsoleteTechs() const; // Exposed to Python
	int getEvent(int i) const; // Exposed to Python
	int getNumEvents() const; // Exposed to Python
	int getPrereqEvent(int i) const; // Exposed to Python
	int getNumPrereqEvents() const; // Exposed to Python
	int getFeatureRequired(int i) const; // Exposed to Python
	int getNumFeaturesRequired() const; // Exposed to Python
	int getTerrainRequired(int i) const; // Exposed to Python
	int getNumTerrainsRequired() const; // Exposed to Python
	int getImprovementRequired(int i) const; // Exposed to Python
	int getNumImprovementsRequired() const; // Exposed to Python
	int getBonusRequired(int i) const; // Exposed to Python
	int getNumBonusesRequired() const; // Exposed to Python
	int getRouteRequired(int i) const; // Exposed to Python
	int getNumRoutesRequired() const; // Exposed to Python
	int getReligionRequired(int i) const; // Exposed to Python
	int getNumReligionsRequired() const; // Exposed to Python
	int getCorporationRequired(int i) const; // Exposed to Python
	int getNumCorporationsRequired() const; // Exposed to Python

	//Property prereqs
	CvProperties* getPrereqMinProperties();
	CvProperties* getPrereqMaxProperties();
	CvProperties* getPrereqPlayerMinProperties();
	CvProperties* getPrereqPlayerMaxProperties();

	const CvWString& getText(int i) const;
	int getTextEra(int i) const;
	int getNumTexts() const;
	const CvWString& getWorldNews(int i) const;
	int getNumWorldNews() const;

// Start EmperorFool: Events with Images
	const TCHAR* getEventArt() const; // Exposed to Python
// End EmperorFool: Events with Images

	bool isSinglePlayer() const; // Exposed to Python
	bool isTeam() const; // Exposed to Python
	bool isRecurring() const; // Exposed to Python
	bool isGlobal() const; // Exposed to Python
	bool isPickPlayer() const; // Exposed to Python
	bool isOtherPlayerWar() const; // Exposed to Python
	bool isOtherPlayerHasReligion() const; // Exposed to Python
	bool isOtherPlayerHasOtherReligion() const; // Exposed to Python
	bool isOtherPlayerAI() const; // Exposed to Python
	bool isPickCity() const; // Exposed to Python
	bool isPickOtherPlayerCity() const; // Exposed to Python
	bool isShowPlot() const; // Exposed to Python
	bool isUnitsOnPlot() const; // Exposed to Python
	bool isOwnPlot() const; // Exposed to Python
	bool isPickReligion() const; // Exposed to Python
	bool isStateReligion() const; // Exposed to Python
	bool isHolyCity() const; // Exposed to Python
	bool isPickCorporation() const; // Exposed to Python
	bool isHeadquarters() const; // Exposed to Python
	bool isProbabilityUnitMultiply() const; // Exposed to Python
	bool isProbabilityBuildingMultiply() const; // Exposed to Python
	bool isPrereqEventCity() const; // Exposed to Python

	const char* getPythonCallback() const;
	const char* getPythonCanDo() const;
	const char* getPythonCanDoCity() const;
	const char* getPythonCanDoUnit() const;

	// bool vector without delayed resolution
	int getNotOnGameOption(int i) const;
	int getNumNotOnGameOptions() const;
	bool isNotOnGameOption(int i);

	int getOnGameOption(int i) const;
	int getNumOnGameOptions() const;
	bool isOnGameOption(int i);

	void read(FDataStreamBase* ) {}
	void write(FDataStreamBase* ) {}

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvEventTriggerInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

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

	// bool vector without delayed resolution
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

	bool isQuest() const; // Exposed to Python
	bool isGlobal() const; // Exposed to Python
	bool isTeam() const; // Exposed to Python
	bool isCityEffect() const; // Exposed to Python
	bool isOtherPlayerCityEffect() const; // Exposed to Python
	bool isGoldToPlayer() const; // Exposed to Python
	bool isGoldenAge() const; // Exposed to Python
	bool isDeclareWar() const; // Exposed to Python
	bool isDisbandUnit() const; // Exposed to Python

	int getGold() const; // Exposed to Python
	int getRandomGold() const; // Exposed to Python
	int getEspionagePoints() const; // Exposed to Python
	int getCulture() const; // Exposed to Python
	int getTech() const; // Exposed to Python
	int getTechPercent() const; // Exposed to Python
	int getTechCostPercent() const; // Exposed to Python
	int getTechMinTurnsLeft() const; // Exposed to Python
	int getPrereqTech() const; // Exposed to Python
	int getFreeUnit() const; // Exposed to Python
	int getNumUnits() const; // Exposed to Python
	int getBuilding() const; // Exposed to Python
	int getBuildingChange() const; // Exposed to Python
	int getHappy() const; // Exposed to Python
	int getHealth() const; // Exposed to Python
	int getHurryAnger() const; // Exposed to Python
	int getHappyTurns() const; // Exposed to Python
	int getFood() const; // Exposed to Python
	int getFoodPercent() const; // Exposed to Python
	int getFeature() const; // Exposed to Python
	int getFeatureChange() const; // Exposed to Python
	int getImprovement() const; // Exposed to Python
	int getImprovementChange() const; // Exposed to Python
	int getCommerceModifier(int i) const;
	int getYieldModifier(int i) const;
	int getBonus() const; // Exposed to Python
	int getBonusChange() const; // Exposed to Python
	int getRoute() const; // Exposed to Python
	int getRouteChange() const; // Exposed to Python
	int getBonusRevealed() const; // Exposed to Python
	int getBonusGift() const; // Exposed to Python
	int getUnitExperience() const; // Exposed to Python
	int getUnitImmobileTurns() const; // Exposed to Python
	int getConvertOwnCities() const; // Exposed to Python
	int getConvertOtherCities() const; // Exposed to Python
	int getMaxNumReligions() const; // Exposed to Python
	int getOurAttitudeModifier() const; // Exposed to Python
	int getAttitudeModifier() const; // Exposed to Python
	int getTheirEnemyAttitudeModifier() const; // Exposed to Python
	int getPopulationChange() const; // Exposed to Python
	int getRevoltTurns() const; // Exposed to Python
	int getMinPillage() const; // Exposed to Python
	int getMaxPillage() const; // Exposed to Python
	int getFreeUnitSupport() const; // Exposed to Python
	int getInflationModifier() const; // Exposed to Python
	int getSpaceProductionModifier() const; // Exposed to Python
	int getAIValue() const; // Exposed to Python

	int getAdditionalEventChance(int i) const; // Exposed to Python
	int getAdditionalEventTime(int i) const; // Exposed to Python
	int getClearEventChance(int i) const; // Exposed to Python
	int getTechFlavorValue(int i) const; // Exposed to Python
	int getPlotExtraYield(int i) const; // Exposed to Python
	int getFreeSpecialistCount(int i) const; // Exposed to Python
	int getUnitCombatPromotion(int i) const; // Exposed to Python
	int getUnitPromotion(int i) const; // Exposed to Python
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

	CvProperties* getProperties();
	CvProperties* getPropertiesAllCities();

	const char* getPythonCallback() const;
	const char* getPythonExpireCheck() const;
	const char* getPythonCanDo() const;
	const char* getPythonHelp() const;
	const wchar* getUnitNameKey() const;
	const wchar* getQuestFailTextKey() const;
	const wchar* getOtherPlayerPopup() const;
	const wchar* getLocalInfoTextKey() const;

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

	void read(FDataStreamBase* ) {}
	void write(FDataStreamBase* ) {}

	bool read(CvXMLLoadUtility* pXML);
//	bool readPass2(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvEventInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

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
	int m_iPrereqTech;
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
	int m_iImprovement;
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

	int getTechPrereq() const;
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

/************************************************************************************************/
/* Afforess					  Start		 01/31/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	bool isNuke() const;
	bool isRevolt() const;
	bool isDisablePower() const;
	int getPrereqGameOption() const;
	int getWarWearinessCounter() const;
	int getSabatogeResearchCostFactor() const;
	int getRemoveReligionsCostFactor() const;
	int getRemoveCorporationsCostFactor() const;
protected:
	bool m_bNuke;
	bool m_bRevolt;
	bool m_bDisablePower;
	int m_iPrereqGameOption;
	int m_iWarWearinessCounter;
	int m_iSabatogeResearchCostFactor;
	int m_iRemoveReligionsCostFactor;
	int m_iRemoveCorporationsCostFactor;
public:
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvEspionageMissionInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iCost;
	bool m_bIsPassive;
	bool m_bIsTwoPhases;
	bool m_bTargetsCity;
	bool m_bSelectPlot;

	int m_iTechPrereq;
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

	const TCHAR* getEarlyArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setEarlyArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal);
	const TCHAR* getLateArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setLateArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal);
	const TCHAR* getMiddleArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setMiddleArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal);

/************************************************************************************************/
/* Afforess					  Start		 03/10/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	const TCHAR* getClassicalArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setClassicalArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal);
	const TCHAR* getRennArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setRennArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal);
	const TCHAR* getIndustrialArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setIndustrialArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal);
	const TCHAR* getFutureArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const;
	void setFutureArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal);
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvUnitArtStyleTypeInfo* pClassInfo, CvXMLLoadUtility* pXML);

protected:

	struct ArtDefneTag
	{
		int iMeshIndex;
		int iUnitType;
		CvString szTag;
	};
	typedef std::vector<ArtDefneTag> ArtDefineArray;
/************************************************************************************************/
/* Afforess					  Start		 03/10/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	ArtDefineArray m_azEarlyArtDefineTags;
	ArtDefineArray m_azClassicalArtDefineTags;
	ArtDefineArray m_azMiddleArtDefineTags;
	ArtDefineArray m_azRennArtDefineTags;
	ArtDefineArray m_azIndustrialArtDefineTags;
	ArtDefineArray m_azLateArtDefineTags;
	ArtDefineArray m_azFutureArtDefineTags;
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
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

	int getVoteInterval() const; // Exposed to Python
	int getFreeSpecialist() const; // Exposed to Python
	int getCivic() const; // Exposed to Python
	const CvWString getPopupText() const;
	const CvWString getSecretaryGeneralText() const;
	const CvString& getCopyPopupText() const;
	const CvString& getCopySecretaryGeneralText() const;

	std::wstring pyGetSecretaryGeneralText() { return getSecretaryGeneralText(); } // Exposed to Python

	int getReligionYield(int i) const; // Exposed to Python
	int getReligionCommerce(int i) const; // Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	bool readPass3();

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvVoteSourceInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

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
	void copyNonDefaults(CvMainMenuInfo* pClassInfo, CvXMLLoadUtility* pXML);

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
	std::string getModuleFolder(int i) const;
	std::string getParentFolder() const;
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
	const PropertyPromotion& getPropertyPromotion(int index) const;
	int getNumPropertyPromotions() const;

	int getChangePropagator(const GameObjectTypes eFrom, const GameObjectTypes eTo) const;

	int getNumTargetLevelbyEraTypes() const;
	int getTargetLevelbyEraType(int iIndex) const;
	bool isTargetLevelbyEraType(int iIndex) const;

	int getChar() const;
	void setChar(int i);
	int getFontButtonIndex() const;

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvPropertyInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

	CvPropertyManipulators* getPropertyManipulators();

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

	CvPropertyManipulators m_PropertyManipulators;
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
	TechTypes getPrereqTech() const;
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
	int getNumPrereqBuildings() const;
	BuildingTypes getPrereqBuilding(int i) const;
	int getNumExtraChancePromotions() const;
	PromotionTypes getExtraChancePromotion(int i) const;
	int getExtraChancePromotionChance(int i) const;
	int getNumReplaceOutcomes() const;
	OutcomeTypes getReplaceOutcome(int i) const;

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvOutcomeInfo* pClassInfo, CvXMLLoadUtility* pXML);
	void copyNonDefaultsReadPass2(CvOutcomeInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL, bool bOver = false);

	void getCheckSum(unsigned int& iSum);

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

	TechTypes getPrereqTech() const;
	TechTypes getObsoleteTech() const;
	PropertyTypes getPropertyType() const;
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
	bool isAffliction() const;
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
	bool isUnitCombatPrereqType(int i) const;

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

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvPromotionLineInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

protected:

	TechTypes m_ePrereqTech;
	TechTypes m_eObsoleteTech;
	PropertyTypes m_ePropertyType;
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
	void copyNonDefaults(CvUnitCombatInfo* pClassInfo, CvXMLLoadUtility* pXML);
	void getCheckSum(unsigned int& iSum);

	//bool readPass2(CvXMLLoadUtility* pXML);
	//void copyNonDefaultsReadPass2(CvUnitCombatInfo* pClassInfo = NULL , CvXMLLoadUtility* pXML = NULL);

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	static UnitCombatTypes getUnitCombatAnimal()	{ return m_UnitCombatAnimal; }
	static void setUnitCombatAnimal(UnitCombatTypes eIndex) { m_UnitCombatAnimal = eIndex; }
	static UnitCombatTypes getUnitCombatWildAnimal()	{ return m_UnitCombatWildAnimal; }
	static void setUnitCombatWildAnimal(UnitCombatTypes eIndex) { m_UnitCombatWildAnimal = eIndex; }

	CvOutcomeList* getKillOutcomeList();
	int getNumActionOutcomes() const;
	CvOutcomeList* getActionOutcomeList(int index) const;
	MissionTypes getActionOutcomeMission(int index) const;
	CvOutcomeList* getActionOutcomeListByMission(MissionTypes eMission) const;
	CvOutcomeMission* getOutcomeMission(int index) const;
	CvOutcomeMission* getOutcomeMissionByMission(MissionTypes eMission) const;

protected:

	static UnitCombatTypes m_UnitCombatAnimal;
	static UnitCombatTypes m_UnitCombatWildAnimal;

	CvOutcomeList m_KillOutcomeList;
	std::vector<CvOutcomeMission*> m_aOutcomeMissions;

public:
	// Textual References
	ReligionTypes getReligion() const;
	BonusTypes getCulture() const;
	EraTypes getEra() const;
	// integers
	int getAssetMultiplier() const;
	int getPowerMultiplier() const;
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
	int getPursuitChange(bool bForLoad = false) const;
	int getEarlyWithdrawChange(bool bForLoad = false) const;
	int getVSBarbsChange() const;
	int getArmorChange() const;
	int getPunctureChange() const;
	int getOverrunChange(bool bForLoad = false) const;
	int getRepelChange(bool bForLoad = false) const;
	int getFortRepelChange(bool bForLoad = false) const;
	int getRepelRetriesChange(bool bForLoad = false) const;
	int getUnyieldingChange(bool bForLoad = false) const;
	int getKnockbackChange(bool bForLoad = false) const;
	int getKnockbackRetriesChange(bool bForLoad = false) const;
	int getStrAdjperAttChange(bool bForLoad = false) const;
	int getStrAdjperDefChange(bool bForLoad = false) const;
	int getWithdrawAdjperAttChange(bool bForLoad = false) const;
	int getUnnerveChange(bool bForLoad = false) const;
	int getEncloseChange(bool bForLoad = false) const;
	int getLungeChange(bool bForLoad = false) const;
	int getDynamicDefenseChange(bool bForLoad = false) const;
	int getStrengthChange() const;
	int getFortitudeChange() const;

	int getFrontSupportPercentChange(bool bForLoad = false) const;
	int getShortRangeSupportPercentChange(bool bForLoad = false) const;
	int getMediumRangeSupportPercentChange(bool bForLoad = false) const;
	int getLongRangeSupportPercentChange(bool bForLoad = false) const;
	int getFlankSupportPercentChange(bool bForLoad = false) const;

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
	int getHillsWorkModifierChange() const;
	int getPeaksWorkModifierChange() const;
	int getBreakdownChanceChange() const;
	int getBreakdownDamageChange() const;
	int getTauntChange() const;
	int getMaxHPChange(bool bForLoad = false) const;
	int getStrengthModifier() const;
	int getQualityBase() const;
	int getGroupBase() const;
	int getSizeBase() const;
	int getDamageModifierChange() const;
	int getCostModifierChange() const;
	int getRBombardDamageBase() const;
	int getRBombardDamageLimitBase() const;
	int getRBombardDamageMaxUnitsBase() const;
	int getDCMBombRangeBase() const;
	int getDCMBombAccuracyBase() const;
	int getCombatModifierPerSizeMoreChange(bool bForLoad = false) const;
	int getCombatModifierPerSizeLessChange(bool bForLoad = false) const;
	int getCombatModifierPerVolumeMoreChange(bool bForLoad = false) const;
	int getCombatModifierPerVolumeLessChange(bool bForLoad = false) const;
	int getSelfHealModifier() const;
	int getNumHealSupport() const;
	int getExcileChange() const;
	int getPassageChange() const;
	int getNoNonOwnedCityEntryChange() const;
	int getBarbCoExistChange() const;
	int getBlendIntoCityChange() const;
	int getInsidiousnessChange() const;
	int getInvestigationChange() const;
	int getStealthStrikesChange(bool bForLoad = false) const;
	int getStealthCombatModifierChange(bool bForLoad = false) const;
	int getStealthDefenseChange(bool bForLoad = false) const;
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
	bool isPillageCulture() const;
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
	inline int	getZobristValue() const { return m_zobristValue; }

	// Arrays
	int getDomainModifierPercent(int i) const; // Exposed to Python
	bool isAnyDomainModifierPercent() const;

	// bool vector with delayed resolution
	int getCureAfflictionChangeType(int i) const;
	int getNumCureAfflictionChangeTypes() const;
	bool isCureAfflictionChangeType(int i) const;

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

	int getNumAidChanges() const;
	int getAidChange(int iProperty) const;
	bool isAidChange(int iProperty) const;

	// int vector utilizing struct with delayed resolution
	int getNumAfflictionFortitudeChangeModifiers() const;
	const PromotionLineModifier& getAfflictionFortitudeChangeModifier(int iAfflictionLine) const;

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

	int getNumAfflictOnAttackChangeTypes() const;
	const AfflictOnAttackChange& getAfflictOnAttackChangeType(int iAfflictionLine) const;

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

	//Propery Manipulators
	CvPropertyManipulators* getPropertyManipulators();

protected:
	// Textual References
	ReligionTypes m_eReligion;
	BonusTypes m_eCulture;
	EraTypes m_eEra;
	//Integers
	int m_iAssetMultiplier;
	int m_iPowerMultiplier;
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
	int m_iStrAdjperAttChange;
	int m_iStrAdjperDefChange;
	int m_iWithdrawAdjperAttChange;
	int m_iUnnerveChange;
	int m_iEncloseChange;
	int m_iLungeChange;
	int m_iDynamicDefenseChange;
	int m_iStrengthChange;
	int m_iFortitudeChange;
	int m_iFrontSupportPercentChange;
	int m_iShortRangeSupportPercentChange;
	int m_iMediumRangeSupportPercentChange;
	int m_iLongRangeSupportPercentChange;
	int m_iFlankSupportPercentChange;
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
	int m_iHillsWorkModifierChange;
	int m_iPeaksWorkModifierChange;
	int m_iBreakdownChanceChange;
	int m_iBreakdownDamageChange;
	int m_iTauntChange;
	int m_iMaxHPChange;
	int m_iStrengthModifier;
	int m_iQualityBase;
	int m_iGroupBase;
	int m_iSizeBase;
	int m_iDamageModifierChange;
	int m_iCostModifierChange;
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
	bool m_bPillageCulture;
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
	bool m_bAnyDomainModifierPercent;

	// bool vectors without delayed resolution
	// bool vector with delayed resolution
	std::vector<int> m_aiCureAfflictionChangeTypes;
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
	AidArray m_aAidChanges;
	// int vector utilizing struct with delayed resolution
	std::vector<PromotionLineModifier> m_aAfflictionFortitudeChangeModifiers;
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
	std::vector<AfflictOnAttackChange> m_aAfflictOnAttackChangeTypes;
	std::vector<InvisibleTerrainChanges> m_aInvisibleTerrainChanges;
	std::vector<InvisibleFeatureChanges> m_aInvisibleFeatureChanges;
	std::vector<InvisibleImprovementChanges> m_aInvisibleImprovementChanges;
	std::vector<InvisibleFeatureChanges> m_aVisibleFeatureChanges;
	std::vector<InvisibleImprovementChanges> m_aVisibleImprovementChanges;
	std::vector<InvisibleTerrainChanges> m_aVisibleTerrainChanges;
	std::vector<InvisibleFeatureChanges> m_aVisibleFeatureRangeChanges;
	std::vector<InvisibleImprovementChanges> m_aVisibleImprovementRangeChanges;
	std::vector<AfflictionLineChanges> m_aDistanceAttackCommunicabilityTypeChanges;
	std::vector<InvisibleTerrainChanges> m_aVisibleTerrainRangeChanges;

	CvPropertyManipulators m_PropertyManipulators;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvMapCategoryInfo
//
//  DESC:   Contains info about Map Category Types
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMapCategoryInfo :
	public CvInfoBase
{
public:

	CvMapCategoryInfo();
	virtual ~CvMapCategoryInfo();

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvMapCategoryInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

	//bools
	bool isInitialized();

protected:
	//bools
	bool m_bInitialized;
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

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvIdeaClassInfo* pClassInfo, CvXMLLoadUtility* pXML);
	void copyNonDefaultsReadPass2(CvIdeaClassInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL, bool bOver = false);

	void getCheckSum(unsigned int& iSum);

	//bools
	bool isInitialized();

protected:
	//bools
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

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvIdeaInfo* pClassInfo, CvXMLLoadUtility* pXML);
	void copyNonDefaultsReadPass2(CvIdeaInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL, bool bOver = false);

	void getCheckSum(unsigned int& iSum);

	//References
	IdeaClassTypes getIdeaClass() const;
	//bools
	/*bool isInitialized();*/

protected:
	//References
	IdeaClassTypes m_eIdeaClass;

	//bools
	/*bool m_bInitialized;*/
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

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvInvisibleInfo* pClassInfo, CvXMLLoadUtility* pXML);
	void getCheckSum(unsigned int& iSum);

	//References
	//bools
	//integers

	//Processes
	int getChar() const;
	void setChar(int i);
	int getFontButtonIndex() const;

protected:
	//References
	//bools
	//Processes
	int m_iChar;
	int m_iFontButtonIndex;
};

#endif
