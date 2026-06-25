#pragma once

#ifndef CV_HANDICAP_INFO_H
#define CV_HANDICAP_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvHandicapInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvHandicapInfo
	: public CvInfoBase
	, private bst::noncopyable
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
	int getUnownedWaterTilesPerBarbarianUnit() const;
	int getUnownedTilesPerBarbarianCity() const;
	int getBarbarianCityCreationTurnsElapsed() const;
	int getBarbarianCityCreationProb() const;
	int getAnimalCombatModifier() const;
	int getBarbarianCombatModifier() const;
	int getAIAnimalCombatModifier() const;
	inline int getSubdueAnimalBonusAI() const { return m_iSubdueAnimalBonusAI; }
	int getAIBarbarianCombatModifier() const;

	int getStartingDefenseUnits() const;
	int getStartingWorkerUnits() const;
	int getStartingExploreUnits() const;
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
	int m_iUnownedWaterTilesPerBarbarianUnit;
	int m_iUnownedTilesPerBarbarianCity;
	int m_iBarbarianCityCreationTurnsElapsed;
	int m_iBarbarianCityCreationProb;
	int m_iAnimalCombatModifier;
	int m_iBarbarianCombatModifier;
	int m_iAIAnimalCombatModifier;
	int m_iSubdueAnimalBonusAI;
	int m_iAIBarbarianCombatModifier;

	int m_iStartingDefenseUnits;
	int m_iStartingWorkerUnits;
	int m_iStartingExploreUnits;
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

#endif // CV_HANDICAP_INFO_H
