#pragma once

#ifndef CV_ESPIONAGE_MISSION_INFO_H
#define CV_ESPIONAGE_MISSION_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEspionageMissionInfo
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEspionageMissionInfo
	: public CvInfoBase
	, private bst::noncopyable
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

#endif // CV_ESPIONAGE_MISSION_INFO_H
