#pragma once

#ifndef CV_SPAWN_INFO_H
#define CV_SPAWN_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// class	: CvSpawnInfo
//
// \brief	: Holds information relating to auto unit spawns
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CvSpawnInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:
	CvSpawnInfo();
	virtual ~CvSpawnInfo();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvSpawnInfo* pClassInfo);

	const std::vector<BonusTypes>& getBonuses() const { return m_bonusTypes; }
	const std::vector<UnitTypes>& getSpawnGroups() const { return m_spawnGroup; }
	const std::vector<TerrainTypes>& getTerrain() const { return m_terrainTypes; }
	const std::vector<FeatureTypes>& getFeatures() const { return m_featureTypes; }
	const std::vector<TerrainTypes>& getFeatureTerrain() const { return m_featureTerrainTypes; }

	PlayerTypes getPlayer() const;
	int getTurnRate() const;
	int getGlobalTurnRate() const;
	int getMaxLocalDensity() const;
	int getMinAreaPlotsPerPlayerUnit() const;
	int getMinAreaPlotsPerUnitType() const;
	int getStartDate() const;
	int	getEndDate() const;
	int	getMinLatitude() const;
	int	getMaxLatitude() const;
	bool getLatitudeAbs() const;
	int	getMinLongitude() const;
	int	getMaxLongitude() const;
	bool getTreatAsBarbarian() const;
	bool getNeutralOnly() const;
	bool getNoSpeedNormalization() const;
	bool getNotInView() const;
	bool getHills() const;
	bool getFlatlands() const;
	bool getPeaks() const;
	bool getFreshWaterOnly() const;
	int getRateOverride() const;
	UnitTypes getUnitType() const;
	TechTypes getPrereqTech() const			{ return m_ePrereqTech; }
	TechTypes getObsoleteTech() const		{ return m_eObsoleteTechType; }

	const BoolExpr* getSpawnCondition() const;

	void getCheckSum(uint32_t& iSum) const;

private:
	UnitTypes				m_eUnitType;
	TechTypes				m_ePrereqTech;
	TechTypes				m_eObsoleteTechType;
	int						m_iPlayerType;
	int						m_iTurns;
	int						m_iGlobalTurns;
	int						m_iMaxLocalDensity;
	int						m_iMinAreaPlotsPerPlayerUnit;
	int						m_iMinAreaPlotsPerUnitType;
	int						m_iStartDate;
	int						m_iEndDate;
	bool					m_bTreatAsBarbarian;
	bool					m_bNeutralOnly;
	bool					m_bNotInView;
	bool					m_bNoSpeedNormalization;
	bool					m_bHills;
	bool					m_bFlatlands;
	bool					m_bPeaks;
	bool					m_bFreshWaterOnly;

	bool					m_bLatitudeAbs;
	int						m_iMinLatitude;
	int						m_iMaxLatitude;
	int						m_iMinLongitude;
	int						m_iMaxLongitude;

	int						m_iRateOverride;
	std::vector<BonusTypes>	m_bonusTypes;
	std::vector<TerrainTypes>	m_terrainTypes;
	std::vector<FeatureTypes>	m_featureTypes;
	std::vector<TerrainTypes>	m_featureTerrainTypes;
	std::vector<UnitTypes>		m_spawnGroup;

	const BoolExpr*				m_pExprSpawnCondition;
};

#endif // CV_SPAWN_INFO_H
