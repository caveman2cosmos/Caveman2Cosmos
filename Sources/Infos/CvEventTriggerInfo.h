#pragma once

#ifndef CV_EVENT_TRIGGER_INFO_H
#define CV_EVENT_TRIGGER_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEventTriggerInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEventTriggerInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	friend class CvXMLLoadUtility;

public:
	CvEventTriggerInfo();
	virtual ~CvEventTriggerInfo();

	int getPercentGamesActive() const;
	int getProbability() const;
	int getNumUnits() const;
	int getNumBuildings() const;
	int getNumUnitsGlobal() const;
	int getNumBuildingsGlobal() const;
	int getNumPlotsRequired() const;
	int getPlotType() const;
	int getNumReligions() const;
	int getNumCorporations() const;
	int getOtherPlayerShareBorders() const;
	int getOtherPlayerHasTech() const;
	int getCityFoodWeight() const;
	int getCivic() const;
	int getMinPopulation() const;
	int getMaxPopulation() const;
	int getMinMapLandmass() const;
	int getMinOurLandmass() const;
	int getMaxOurLandmass() const;
	int getMinDifficulty() const;
	int getAngry() const;
	int getUnhealthy() const;
	int getUnitDamagedWeight() const;
	int getUnitDistanceWeight() const;
	int getUnitExperienceWeight() const;
	int getMinTreasury() const;

	int getBuildingRequired(int i) const;
	int getNumBuildingsRequired() const;
	int getUnitRequired(int i) const;
	int getNumUnitsRequired() const;
	int getPrereqOrTechs(int i) const;
	int getNumPrereqOrTechs() const;
	int getPrereqAndTechs(int i) const;
	int getNumPrereqAndTechs() const;
	int getObsoleteTech(int i) const;
	int getNumObsoleteTechs() const;
	int getEvent(int i) const;
	int getNumEvents() const;
	int getPrereqEvent(int i) const;
	int getNumPrereqEvents() const;
	int getFeatureRequired(int i) const;
	int getNumFeaturesRequired() const;
	int getTerrainRequired(int i) const;
	int getNumTerrainsRequired() const;
	int getImprovementRequired(int i) const;
	int getNumImprovementsRequired() const;
	int getBonusRequired(int i) const;
	int getNumBonusesRequired() const;
	int getRouteRequired(int i) const;
	int getNumRoutesRequired() const;
	int getReligionRequired(int i) const;
	int getNumReligionsRequired() const;
	int getCorporationRequired(int i) const;
	int getNumCorporationsRequired() const;

	//Property prereqs
	const CvProperties* getPrereqMinProperties() const;
	const CvProperties* getPrereqMaxProperties() const;
	const CvProperties* getPrereqPlayerMinProperties() const;
	const CvProperties* getPrereqPlayerMaxProperties() const;

	const CvWString& getText(int i) const;
	int getTextEra(int i) const;
	int getNumTexts() const;
	const CvWString& getWorldNews(int i) const;
	int getNumWorldNews() const;

// Start EmperorFool: Events with Images
	const char* getEventArt() const;
// End EmperorFool: Events with Images

	bool isSinglePlayer() const;
	bool isTeam() const;
	bool isRecurring() const;
	bool isGlobal() const;
	bool isPickPlayer() const;
	bool isOtherPlayerWar() const;
	bool isOtherPlayerHasReligion() const;
	bool isOtherPlayerHasOtherReligion() const;
	bool isOtherPlayerAI() const;
	bool isPickCity() const;
	bool isPickOtherPlayerCity() const;
	bool isShowPlot() const;
	bool isUnitsOnPlot() const;
	bool isOwnPlot() const;
	bool isPickReligion() const;
	bool isStateReligion() const;
	bool isHolyCity() const;
	bool isPickCorporation() const;
	bool isHeadquarters() const;
	bool isProbabilityUnitMultiply() const;
	bool isProbabilityBuildingMultiply() const;
	bool isPrereqEventCity() const;

	const char* getPythonCallback() const;
	const char* getPythonCanDo() const;
	const char* getPythonCanDoCity() const;
	const char* getPythonCanDoUnit() const;

	// bool vector without delayed resolution
	int getNotOnGameOption(int i) const;
	int getNumNotOnGameOptions() const;
	bool isNotOnGameOption(int i) const;

	int getOnGameOption(int i) const;
	int getNumOnGameOptions() const;
	bool isOnGameOption(int i) const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvEventTriggerInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

private:
	int m_iPercentGamesActive;
	int m_iProbability;
	int m_iNumUnits;
	int m_iNumBuildings;
	int m_iNumUnitsGlobal;
	int m_iNumBuildingsGlobal;
	int m_iNumPlotsRequired;
	int m_iPlotType;
	int m_iNumReligions;
	int m_iNumCorporations;
	int m_iOtherPlayerShareBorders;
	int m_iOtherPlayerHasTech;
	int m_iCityFoodWeight;
	int m_iCivic;
	int m_iMinPopulation;
	int m_iMaxPopulation;
	int m_iMinMapLandmass;
	int m_iMinOurLandmass;
	int m_iMaxOurLandmass;
	int m_iMinDifficulty;
	int m_iAngry;
	int m_iUnhealthy;
	int m_iUnitDamagedWeight;
	int m_iUnitDistanceWeight;
	int m_iUnitExperienceWeight;
	int m_iMinTreasury;

	CvProperties m_PrereqMinProperties;
	CvProperties m_PrereqMaxProperties;
	CvProperties m_PrereqPlayerMinProperties;
	CvProperties m_PrereqPlayerMaxProperties;

	std::vector<int> m_aiUnitsRequired;
	std::vector<int> m_aiBuildingsRequired;
	std::vector<int> m_aiPrereqOrTechs;
	std::vector<int> m_aiPrereqAndTechs;
	std::vector<int> m_aiObsoleteTechs;
	std::vector<int> m_aiEvents;
	std::vector<int> m_aiPrereqEvents;
	std::vector<int> m_aiFeaturesRequired;
	std::vector<int> m_aiTerrainsRequired;
	std::vector<int> m_aiImprovementsRequired;
	std::vector<int> m_aiBonusesRequired;
	std::vector<int> m_aiRoutesRequired;
	std::vector<int> m_aiReligionsRequired;
	std::vector<int> m_aiCorporationsRequired;

	std::vector<int> m_aiTextEra;
	std::vector<CvWString> m_aszText;
	std::vector<CvWString> m_aszWorldNews;

// Start EmperorFool: Events with Images
	CvString m_szEventArt;
// End EmperorFool: Events with Images
	bool m_bSinglePlayer;
	bool m_bTeam;
	bool m_bRecurring;
	bool m_bGlobal;
	bool m_bPickPlayer;
	bool m_bOtherPlayerWar;
	bool m_bOtherPlayerHasReligion;
	bool m_bOtherPlayerHasOtherReligion;
	bool m_bOtherPlayerAI;
	bool m_bPickCity;
	bool m_bPickOtherPlayerCity;
	bool m_bShowPlot;
	bool m_bUnitsOnPlot;
	bool m_bOwnPlot;
	bool m_bPickReligion;
	bool m_bStateReligion;
	bool m_bHolyCity;
	bool m_bPickCorporation;
	bool m_bHeadquarters;
	bool m_bProbabilityUnitMultiply;
	bool m_bProbabilityBuildingMultiply;
	bool m_bPrereqEventCity;

	CvString m_szPythonCallback;
	CvString m_szPythonCanDo;
	CvString m_szPythonCanDoCity;
	CvString m_szPythonCanDoUnit;

	std::vector<int> m_aiNotOnGameOptions;
	std::vector<int> m_aiOnGameOptions;
};

#endif // CV_EVENT_TRIGGER_INFO_H
