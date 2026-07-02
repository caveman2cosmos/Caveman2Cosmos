#pragma once

#ifndef CV_EVENT_INFO_H
#define CV_EVENT_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEventInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEventInfo
	: public CvInfoBase
	, private bst::noncopyable
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
	inline bool isGameSpeedScale() const { return m_bGameSpeedScale; }

	int getGold() const;
	int getRandomGold() const;
	int getEspionagePoints() const;
	int getCulture() const;
	int getTech() const;
	int getTechPercent() const;
	int getTechCostPercent() const;
	int getTechMinTurnsLeft() const;
	inline TechTypes getPrereqTech() const { return m_iPrereqTech; }
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
	inline ImprovementTypes getImprovement() const { return m_iImprovement; }
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

	// ---------------------------------------------------------------------
    // Direct sparse-iteration accessors over the per-building modifier
    // vectors below. Storage in CvEventInfo is already sparse (only buildings
    // with a non-zero change are stored), but the dense getXxx(iBuilding, ...)
    // accessors above do a linear search per call. Callers that want to apply
    // every change should iterate these vectors directly:
    //
    //     foreach_(const BuildingYieldChange& yc, kEvent.getBuildingYieldChanges())
    //         pCity->changeBuildingYieldChange(yc.eBuilding, yc.eYield, yc.iChange);
    //
    // This is the structural counterpart to the lazy sparse cache in
    // CvCivicInfo: there, the dense storage stays canonical and a sparse view
    // is derived. Here, the storage is already sparse so no caching is needed
    // — only public exposure of the existing vectors.
    // ---------------------------------------------------------------------
    const BuildingChangeArray& getBuildingHappyChanges() const;
    const BuildingChangeArray& getBuildingHealthChanges() const;
    const std::vector<BuildingYieldChange>& getBuildingYieldChanges() const;
    const std::vector<BuildingCommerceChange>& getBuildingCommerceChanges() const;
    const std::vector<BuildingCommerceChange>& getBuildingCommerceModifiers() const;

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
	bool m_bGameSpeedScale;

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

#endif // CV_EVENT_INFO_H
