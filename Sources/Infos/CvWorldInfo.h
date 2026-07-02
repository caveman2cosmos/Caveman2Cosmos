#pragma once

#ifndef CV_WORLD_INFO_H
#define CV_WORLD_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvWorldInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvWorldInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:
	CvWorldInfo();
	virtual ~CvWorldInfo();

	DllExport int getDefaultPlayers() const;
	int getUnitNameModifier() const;
	int getTargetNumCities() const;
	int getBuildingPrereqModifier() const;
	int getMaxConscriptModifier() const;
	int getWarWearinessModifier() const;
	int getGridWidth() const;
	int getGridHeight() const;
	int getTerrainGrainChange() const;
	int getFeatureGrainChange() const;
	int getTradeProfitPercent() const;
	int getDistanceMaintenancePercent() const;
	int getNumCitiesMaintenancePercent() const;
	int getColonyMaintenancePercent() const;
	int getCorporationMaintenancePercent() const;
	int getNumCitiesAnarchyPercent() const;
	int getAdvancedStartPointsMod() const;
	int getCommandersLevelThresholdsPercent() const;
	int getOceanMinAreaSize() const;

	int getPercent(int iID) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvWorldInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
private:
	int m_iDefaultPlayers;
	int m_iUnitNameModifier;
	int m_iTargetNumCities;
	int m_iBuildingPrereqModifier;
	int m_iMaxConscriptModifier;
	int m_iWarWearinessModifier;
	int m_iGridWidth;
	int m_iGridHeight;
	int m_iTerrainGrainChange;
	int m_iFeatureGrainChange;
	int m_iTradeProfitPercent;
	int m_iDistanceMaintenancePercent;
	int m_iNumCitiesMaintenancePercent;
	int m_iColonyMaintenancePercent;
	int m_iCorporationMaintenancePercent;
	int m_iNumCitiesAnarchyPercent;
	int m_iAdvancedStartPointsMod;
	int m_iCommandersLevelThresholdsPercent;
	int m_iOceanMinAreaSize;

	IDValueMapPercent m_Percent;
};

#endif // CV_WORLD_INFO_H
