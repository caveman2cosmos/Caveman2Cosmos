#pragma once

#ifndef CV_BUILD_INFO_H
#define CV_BUILD_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvBuildInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBuildInfo
	: public CvHotkeyInfo
	, private bst::noncopyable
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

	int getCategory(int i) const;
	int getNumCategories() const;
	bool isCategory(int i) const;

	const std::vector<BonusTypes>& getPrereqBonuses() const			{ return m_aiPrereqBonusTypes; }
	const std::vector<MapCategoryTypes>& getMapCategories() const	{ return m_aeMapCategoryTypes; }
	const std::vector<TerrainStructs>& getTerrainStructs() const	{ return m_aTerrainStructs; }
	const std::vector<PlaceBonusTypes>& getPlaceBonusTypes() const	{ return m_aPlaceBonusTypes; }

	void getDataMembers(CvInfoUtil& util);
	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvBuildInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;
	void doPostLoadCaching(uint32_t iThis);

	//----------------------PRIVATE MEMBER VARIABLES----------------------------

private:
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

	std::vector<int> m_aiCategories;
	std::vector<BonusTypes> m_aiPrereqBonusTypes;
	std::vector<MapCategoryTypes> m_aeMapCategoryTypes;

	std::vector<TerrainStructs> m_aTerrainStructs;
	std::vector<PlaceBonusTypes> m_aPlaceBonusTypes;
};

#endif // CV_BUILD_INFO_H
