#pragma once

#ifndef CV_TECH_INFO_H
#define CV_TECH_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvTechInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTechInfo
	: public CvInfoBase
	, private bst::noncopyable
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvTechInfo();
	virtual ~CvTechInfo();

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
	inline bool isLanguage() const { return m_bLanguage;}

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

	int getCategory(int i) const;
	int getNumCategories() const;
	bool isCategory(int i) const;

	bool isGlobal() const;

	bool getDCMAirBombTech1() const;
	bool getDCMAirBombTech2() const;


	std::wstring getQuote() const;
	const char* getQuoteKey() const;
	const char* getSound() const;
	const char* getSoundMP() const;

	int getDomainExtraMoves(int i) const;
	int getFlavorValue(int i) const;

	const std::vector<TechTypes>& getPrereqOrTechs() const;
	const std::vector<TechTypes>& getPrereqAndTechs() const;

	bool isCommerceFlexible(int i) const;
	bool isTerrainTrade(int i) const;

	int getCommerceModifier(int i) const;
	int* getCommerceModifierArray() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvTechInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	void doPostLoadCaching(uint32_t iThis);
	const std::set<TechTypes>& getLeadsToTechs() const { return m_leadsTo; }

protected:

	int m_iAdvisorType;
	int m_iAIWeight;
	int m_iAITradeModifier;
	int m_iResearchCost;
	int m_iEra;
	int m_iTradeRoutes;
	int m_iFeatureProductionModifier;
	int m_iWorkerSpeedModifier;

	int m_iMaintenanceModifier;
	int m_iDistanceMaintenanceModifier;
	int m_iNumCitiesMaintenanceModifier;
	int m_iCoastalDistanceMaintenanceModifier;

	int m_iFirstFreeUnit;
	int m_iFirstFreeProphet;
	int m_iHealth;
	int m_iHappiness;
	int m_iFirstFreeTechs;
	int m_iAssetValue;
	int m_iPowerValue;

	int m_iGridX;
	int m_iGridY;

	int m_iInflationModifier;
	int m_iGlobalTradeModifier;
	int m_iGlobalForeignTradeModifier;
	int m_iTradeMissionModifier;
	int m_iCorporationRevenueModifier;
	int m_iCorporationMaintenanceModifier;
	int m_iPrereqGameOption;

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
	bool m_bLanguage;
	bool m_bDCMAirBombTech1;
	bool m_bDCMAirBombTech2;
	bool m_bGlobal;
	bool m_bCanPassPeaks;
	bool m_bMoveFastPeaks;
	bool m_bCanFoundOnPeaks;
	bool m_bEmbassyTrading;
	bool m_bEnableDarkAges;
	bool m_bRebaseAnywhere;
	bool m_bEnablesDesertFarming;

	CvString m_szQuoteKey;
	CvString m_szSound;
	CvString m_szSoundMP;


	int* m_piDomainExtraMoves;
	int* m_piFlavorValue;
	int* m_piCommerceModifier;
	int* m_piFreeSpecialistCount;

	bool* m_pbCommerceFlexible;
	bool* m_pbTerrainTrade;

	std::vector<int> m_aiCategories;

	std::vector<TechTypes> m_piPrereqOrTechs;
	std::vector<TechTypes> m_piPrereqAndTechs;

	std::vector<PrereqBuilding> m_aPrereqBuilding;
	std::vector<PrereqBuilding> m_aPrereqOrBuilding;

private:
	std::set<TechTypes> m_leadsTo;
	void setLeadsTo(const TechTypes eTech);
public:
	int getNumLeadsToTechs() const { return m_leadsTo.size(); }
	int getLeadsToTech(const int iCount) const
	{
		PROFILE_EXTRA_FUNC();
		std::set<TechTypes>::const_iterator itr = m_leadsTo.begin();
		for (int i = 0; i < iCount; i++) itr++;
		return *itr;
	}
};

#endif // CV_TECH_INFO_H
