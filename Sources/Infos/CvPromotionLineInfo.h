#pragma once

#ifndef CV_PROMOTION_LINE_INFO_H
#define CV_PROMOTION_LINE_INFO_H

#include "CvInfoBase.h"

//TB Promotion Line Mod begin
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPromotionLineInfo
//
//  DESC:   Contains info about Promotion Lines, definitions of promotion upgrade chains
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPromotionLineInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:

	CvPromotionLineInfo();
	virtual ~CvPromotionLineInfo();

	TechTypes getPrereqTech() const	{ return m_ePrereqTech; }
	TechTypes getObsoleteTech() const;
	PropertyTypes getPropertyType() const;
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

	int getCategory(int i) const;
	int getNumCategories() const;
	bool isCategory(int i) const;

	// int vector utilizing pairing without delayed resolution
	int getNumUnitCombatContractChanceChanges() const;
	int getUnitCombatContractChanceChange(int iUnitCombat) const;
	bool isUnitCombatContractChanceChange(int iUnitCombat) const;

	int getNumTechContractChanceChanges() const;
	int getTechContractChanceChange(int iTech) const;
	bool isTechContractChanceChange(int iTech) const;

	//Post Load

	int getPromotion(int i) const;
	int getNumPromotions() const;
	bool isPromotion(int i) const;

	int getBuilding(int i) const;
	int getNumBuildings() const;
	bool isBuilding(int i) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvPromotionLineInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;
	void doPostLoadCaching(uint32_t iThis);

protected:
	TechTypes m_ePrereqTech;
	TechTypes m_eObsoleteTech;
	PropertyTypes m_ePropertyType;
	bool m_bBuildUp;
	bool m_bPoison;


	// bool vectors without delayed resolution
	std::vector<int> m_aiUnitCombatPrereqTypes;
	std::vector<int> m_aiNotOnUnitCombatTypes;
	std::vector<int> m_aiNotOnDomainTypes;
	std::vector<int> m_aiOnGameOptions;
	std::vector<int> m_aiNotOnGameOptions;
	std::vector<int> m_aiPromotions;
	std::vector<int> m_aiBuildings;
	std::vector<int> m_aiCategories;

	// int vector utilizing pairing without delayed resolution
	UnitCombatModifierArray m_aUnitCombatContractChanceChanges;
	TechModifierArray m_aTechContractChanceChanges;
};

#endif // CV_PROMOTION_LINE_INFO_H
