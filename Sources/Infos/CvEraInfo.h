#pragma once

#ifndef CV_ERA_INFO_H
#define CV_ERA_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEraInfo
//
//  DESC:   Used to manage different types of Art Styles
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEraInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:

	CvEraInfo();
	virtual ~CvEraInfo();

	int getStartingUnitMultiplier() const;
	int getStartingDefenseUnits() const;
	int getStartingWorkerUnits() const;
	int getStartingExploreUnits() const;
	int getAdvancedStartPoints() const;
	int getStartingGold() const;
	int getFreePopulation() const;
	int getStartPercent() const;
	int getGrowthPercent() const;
	int getTrainPercent() const;
	int getConstructPercent() const;
	int getCreatePercent() const;
	int getResearchPercent() const;
	int getBuildPercent() const;
	int getImprovementPercent() const;
	int getGreatPeoplePercent() const;
	int getAnarchyPercent() const;
	int getEventChancePerTurn() const;
	int getSoundtrackSpace() const;
	int getNumSoundtracks() const;
	int getCuttingEdgeCutsTechCostModifier() const;
	int getInitialCityMaintenancePercent() const;

	const char* getAudioUnitVictoryScript() const;
	const char* getAudioUnitDefeatScript() const;

	bool isNoGoodies() const;
	bool isNoAnimals() const;
	bool isNoBarbUnits() const;
	bool isNoBarbCities() const;
	bool isFirstSoundtrackFirst() const;

	// Arrays

	int getSoundtracks(int i) const;
	int getCitySoundscapeSciptId(int i) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvEraInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

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
	int m_iCuttingEdgeCutsTechCostModifier;
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

#endif // CV_ERA_INFO_H
