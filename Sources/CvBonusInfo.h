#pragma once

#include "CvInfos.h"
#include <vector>

#ifndef CV_BONUSINFO_H
#define CV_BONUSINFO_H

class CvArtInfoBonus;

class CvBonusInfo
	: public CvInfoBase
	, private bst::noncopyable
{
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

	const std::vector<std::pair<ImprovementTypes, BuildTypes> >* getTradeProvidingImprovements();

	const std::vector<ImprovementTypes>& getProvidedByImprovementTypes() const { return m_providedByImprovementTypes; };
	ImprovementTypes getProvidedByImprovementType(const int i) const;
	int getNumProvidedByImprovementTypes() const;
	bool isProvidedByImprovementType(const ImprovementTypes i) const;
	void setProvidedByImprovementTypes(const ImprovementTypes eType);

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
	volatile std::vector<std::pair<ImprovementTypes, BuildTypes> >* m_tradeProvidingImprovements;

	std::vector<ImprovementTypes> m_providedByImprovementTypes;
};


#endif