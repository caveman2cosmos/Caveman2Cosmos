#pragma once

#ifndef CV_FEATURE_INFO_H
#define CV_FEATURE_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvFeatureInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoFeature;
class CvFeatureInfo
	: public CvInfoBase
	, private bst::noncopyable
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
	inline bool isCoastalOnly() const { return m_bCoastalOnly; }
	bool isAddsFreshWater() const;
	bool isImpassable() const;
	bool isNoCity() const;
	bool isNoImprovement() const;
	bool isNoBonus() const { return m_bNoBonus; }
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

	int getCategory(int i) const;
	int getNumCategories() const;
	bool isCategory(int i) const;

	const std::vector<MapCategoryTypes>& getMapCategories() const { return m_aeMapCategoryTypes; }

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

	std::vector<int> m_aiCategories;
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
	bool m_bCoastalOnly;
	bool m_bAddsFreshWater;
	bool m_bImpassable;
	bool m_bNoCity;
	bool m_bNoImprovement;
	bool m_bNoBonus;
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

private:
	CvString m_szArtDefineTag;
	int m_zobristValue;
};

#endif // CV_FEATURE_INFO_H
