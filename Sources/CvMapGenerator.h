#pragma once

#ifndef CIV4_MAPGENERATOR_H
#define CIV4_MAPGENERATOR_H

//#include "CvEnums.h"

#pragma warning( disable: 4251 )		// needs to have dll-interface to be used by clients of class

class CvFractal;
class CvPlot;
class CvArea;

class CvMapGenerator
{
public:
	DllExport static CvMapGenerator& GetInstance();
	DllExport static void FreeInstance() { SAFE_DELETE(m_pInst); }
	CvMapGenerator();
	virtual ~CvMapGenerator();

	bool canPlaceBonusAt(BonusTypes eBonus, int iX, int iY, bool bIgnoreLatitude);
	bool canPlaceGoodyAt(ImprovementTypes eImprovement, int iX, int iY);

	// does all of the below "add..." functions:
	DllExport void addGameElements();

	void addLakes();
	DllExport void addRivers();
	void doRiver(CvPlot* pStartPlot, CardinalDirectionTypes eLastCardinalDirection=NO_CARDINALDIRECTION, CardinalDirectionTypes eOriginalCardinalDirection=NO_CARDINALDIRECTION, int iThisRiverID=-1);
	bool addRiver(CvPlot *pFreshWaterPlot);
	DllExport void addFeatures();
	DllExport void addBonuses();
	void addUniqueBonusType(BonusTypes eBonusType);
	void addNonUniqueBonusType(BonusTypes eBonusType);
	DllExport void addGoodies();

	DllExport void eraseRivers();
	DllExport void eraseFeatures();
	DllExport void eraseBonuses();
	DllExport void eraseGoodies();

	DllExport void generateRandomMap();

	void generatePlotTypes();
	void generateTerrain();

	void afterGeneration();

	void setPlotTypes(const std::vector<int>& plotTypes);

private:
	// Utility functions for roughenHeights()
	int getRiverValueAtPlot(CvPlot* pPlot);
	int calculateNumBonusesToAdd(BonusTypes eBonusType);

	static CvMapGenerator* m_pInst;

	void setBonusClusterValues(const CvBonusInfo& bonus, const int iWorldSize, int& iGroupRange, int& iGroupRand, int& iMaxCluster);
	void placeBonusWithCluster(const BonusTypes eBonus, const int iGroupRange, const int iGroupRand, const int iMaxCluster, const bool bIgnoreLatitude, int& iBonusCount, const CvArea *pBestArea=NULL);
};
#endif
