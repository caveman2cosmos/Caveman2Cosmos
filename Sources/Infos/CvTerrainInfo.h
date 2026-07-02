#pragma once

#ifndef CV_TERRAIN_INFO_H
#define CV_TERRAIN_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvTerrainInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoTerrain;
class CvTerrainInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvTerrainInfo();
	virtual ~CvTerrainInfo();

	int getMovementCost() const;
	int getBuildModifier() const;
	int getDefenseModifier() const;

	inline ClimateZoneTypes getClimate() const { return m_eClimate; }
	inline int getDistanceToLand() const { return m_iDistanceToLand; }
	inline bool isWaterTerrain() const { return m_iDistanceToLand > 0; }
	bool isImpassable() const;
	bool isFound() const;
	bool isFoundCoast() const;
	bool isFoundFreshWater() const;
	bool isFreshWaterTerrain() const;

	DllExport const char* getArtDefineTag() const;

	int getYield(int i) const;
	int getWorldSoundscapeScriptId() const;
	int get3DAudioScriptFootstepIndex(int i) const;

	const std::vector<MapCategoryTypes>& getMapCategories() const { return m_aeMapCategoryTypes; }

	const CvArtInfoTerrain* getArtInfo() const;
	const char* getButton() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvTerrainInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

	//	This really belongs on CvInfoBase but you can't change the size of that
	//	object without crashing the core engine :-(
	inline int getZobristValue() const { return m_zobristValue; }

	int getCultureDistance() const;
	int getHealthPercent() const;

	int getCategory(int i) const;
	int getNumCategories() const;
	bool isCategory(int i) const;

private:
	CvPropertyManipulators m_PropertyManipulators;

	CvString m_szArtDefineTag;
	int	m_zobristValue;
	int m_iDistanceToLand;
	ClimateZoneTypes m_eClimate;

	bool m_bImpassable;
	bool m_bFound;
	bool m_bFoundCoast;
	bool m_bFoundFreshWater;
	bool m_bFreshWaterTerrain;

	int m_iMovementCost;
	int m_iBuildModifier;
	int m_iDefenseModifier;
	int m_iWorldSoundscapeScriptId;
	int m_iCultureDistance;
	int m_iHealthPercent;

	// Arrays
	int* m_piYields;
	int* m_pi3DAudioScriptFootstepIndex;

	std::vector<int> m_aiCategories;
	std::vector<MapCategoryTypes> m_aeMapCategoryTypes;
};

#endif // CV_TERRAIN_INFO_H
