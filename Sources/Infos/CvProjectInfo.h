#pragma once

#ifndef CV_PROJECT_INFO_H
#define CV_PROJECT_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvProjectInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvProjectInfo
	: public CvInfoBase
	, private bst::noncopyable
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

	int getCategory(int i) const;
	int getNumCategories() const;
	bool isCategory(int i) const;

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
	std::vector<int> m_aiCategories;
};

#endif // CV_PROJECT_INFO_H
