#ifndef CV_INFO_GAME_H
#define CV_INFO_GAME_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvVictoryInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvVictoryInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvVictoryInfo();
	virtual ~CvVictoryInfo();

	int getPopulationPercentLead() const;	// Exposed to Python
	int getLandPercent() const;				// Exposed to Python
	int getMinLandPercent() const;			// Exposed to Python
	int getReligionPercent() const;			// Exposed to Python
	int getCityCulture() const;				// Exposed to Python
	int getNumCultureCities() const;		// Exposed to Python
	int getTotalCultureRatio() const;		// Exposed to Python
#ifdef QC_MASTERY_VICTORY
	//Sevo VCM
	bool isTotalVictory() const;
#endif
	int getVictoryDelayTurns() const;		// Exposed to Python

	bool isTargetScore() const;				// Exposed to Python
	bool isEndScore() const;				// Exposed to Python
	bool isConquest() const;				// Exposed to Python
	bool isDiploVote() const;				// Exposed to Python
	DllExport bool isPermanent() const;		// Exposed to Python

	const char* getMovie() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvVictoryInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iPopulationPercentLead;
	int m_iLandPercent;
	int m_iMinLandPercent;
	int m_iReligionPercent;
	int m_iCityCulture;
	int m_iNumCultureCities;
	int m_iTotalCultureRatio;
	int m_iVictoryDelayTurns;

	bool m_bTargetScore;
	bool m_bEndScore;
	bool m_bConquest;
	bool m_bDiploVote;
	bool m_bPermanent;
#ifdef QC_MASTERY_VICTORY
	//Sevo VCM
	bool m_bTotalVictory;
#endif
	CvString m_szMovie;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvHandicapInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvHandicapInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvHandicapInfo();
	virtual ~CvHandicapInfo();

	int getFreeWinsVsBarbs() const;					// Exposed to Python
	int getAnimalAttackProb() const;				// Exposed to Python
	int getStartingLocationPercent() const;			// Exposed to Python
	int getAdvancedStartPointsMod() const;			// Exposed to Python
	int getStartingGold() const;					// Exposed to Python
	int getFreeUnits() const;						// Exposed to Python
	int getUnitCostPercent() const;					// Exposed to Python
	int getTrainPercent() const;
	int getConstructPercent() const;
	int getResearchPercent() const;					// Exposed to Python
	int getDistanceMaintenancePercent() const;		// Exposed to Python
	int getNumCitiesMaintenancePercent() const;		// Exposed to Python
	int getMaxNumCitiesMaintenance() const;			// Exposed to Python
	int getColonyMaintenancePercent() const;		// Exposed to Python
	int getMaxColonyMaintenance() const;			// Exposed to Python
	int getCorporationMaintenancePercent() const;	// Exposed to Python
	int getCivicUpkeepPercent() const;				// Exposed to Python
	int getInflationPercent() const;				// Exposed to Python
	/*** REVOLUTION 09/25/09 by DPII ***/
	int getRevolutionIndexPercent() const;
	/***********************************/
	int getHealthBonus() const;						// Exposed to Python
	int getHappyBonus() const;						// Exposed to Python
	int getAttitudeChange() const;					// Exposed to Python
	int getNoTechTradeModifier() const;				// Exposed to Python
	int getTechTradeKnownModifier() const;			// Exposed to Python
	int getUnownedTilesPerGameAnimal() const;		// Exposed to Python
	int getUnownedTilesPerBarbarianUnit() const;	// Exposed to Python
	int getUnownedWaterTilesPerBarbarianUnit() const;	// Exposed to Python
	int getUnownedTilesPerBarbarianCity() const;		// Exposed to Python
	int getBarbarianCreationTurnsElapsed() const;		// Exposed to Python
	int getBarbarianCityCreationTurnsElapsed() const;	// Exposed to Python
	int getBarbarianCityCreationProb() const;		// Exposed to Python
	int getAnimalCombatModifier() const;			// Exposed to Python
	int getBarbarianCombatModifier() const;			// Exposed to Python
	int getAIAnimalCombatModifier() const;			// Exposed to Python
	int getAIBarbarianCombatModifier() const;		// Exposed to Python

	int getStartingDefenseUnits() const;		// Exposed to Python
	int getStartingWorkerUnits() const;			// Exposed to Python
	int getStartingExploreUnits() const;		// Exposed to Python
	int getAIStartingUnitMultiplier() const;	// Exposed to Python
	int getAIStartingDefenseUnits() const;		// Exposed to Python
	int getAIStartingWorkerUnits() const;		// Exposed to Python
	int getAIStartingExploreUnits() const;		// Exposed to Python
	int getBarbarianInitialDefenders() const;	// Exposed to Python
	int getAIDeclareWarProb() const;			// Exposed to Python
	int getAIWorkRateModifier() const;			// Exposed to Python
	int getAIGrowthPercent() const;				// Exposed to Python
	int getAITrainPercent() const;				// Exposed to Python
	int getAIWorldTrainPercent() const;			// Exposed to Python
	int getAIConstructPercent() const;			// Exposed to Python
	int getAIWorldConstructPercent() const;		// Exposed to Python
	int getAICreatePercent() const;				// Exposed to Python
	int getAIResearchPercent() const;
	int getAIWorldCreatePercent() const;		// Exposed to Python
	int getAICivicUpkeepPercent() const;		// Exposed to Python
	int getAIUnitCostPercent() const;			// Exposed to Python
	int getAIUnitSupplyPercent() const;			// Exposed to Python
	int getAIUnitUpgradePercent() const;		// Exposed to Python
	int getAIInflationPercent() const;			// Exposed to Python
	int getAIWarWearinessPercent() const;		// Exposed to Python
	int getAIPerEraModifier() const;			// Exposed to Python
	int getAIAdvancedStartPercent() const;		// Exposed to Python
	int getNumGoodies() const;					// Exposed to Python

	CvPropertyManipulators* getPropertyManipulators();

	// Arrays

	int getGoodies(int i) const;				// Exposed to Python
	int isFreeTechs(int i) const;				// Exposed to Python
	int isAIFreeTechs(int i) const;				// Exposed to Python

	int getPercent(int iID) const;

	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvHandicapInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iFreeWinsVsBarbs;
	int m_iAnimalAttackProb;
	int m_iStartingLocationPercent;
	int m_iAdvancedStartPointsMod;
	int m_iStartingGold;
	int m_iFreeUnits;
	int m_iUnitCostPercent;
	int m_iTrainPercent;
	int m_iConstructPercent;
	int m_iResearchPercent;
	int m_iDistanceMaintenancePercent;
	int m_iNumCitiesMaintenancePercent;
	int m_iMaxNumCitiesMaintenance;
	int m_iColonyMaintenancePercent;
	int m_iMaxColonyMaintenance;
	int m_iCorporationMaintenancePercent;
	int m_iCivicUpkeepPercent;
	int m_iInflationPercent;
	/*** REVOLUTION 09/25/09 by DPII ***/
	int m_iRevolutionIndexPercent;
	/***********************************/
	int m_iHealthBonus;
	int m_iHappyBonus;
	int m_iAttitudeChange;
	int m_iNoTechTradeModifier;
	int m_iTechTradeKnownModifier;
	int m_iUnownedTilesPerGameAnimal;
	int m_iUnownedTilesPerBarbarianUnit;
	int m_iUnownedWaterTilesPerBarbarianUnit;
	int m_iUnownedTilesPerBarbarianCity;
	int m_iBarbarianCreationTurnsElapsed;
	int m_iBarbarianCityCreationTurnsElapsed;
	int m_iBarbarianCityCreationProb;
	int m_iAnimalCombatModifier;
	int m_iBarbarianCombatModifier;
	int m_iAIAnimalCombatModifier;
	int m_iAIBarbarianCombatModifier;

	int m_iStartingDefenseUnits;
	int m_iStartingWorkerUnits;
	int m_iStartingExploreUnits;
	int m_iAIStartingUnitMultiplier;
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
	int m_iAIUnitCostPercent;
	int m_iAIUnitSupplyPercent;
	int m_iAIUnitUpgradePercent;
	int m_iAIInflationPercent;
	int m_iAIWarWearinessPercent;
	int m_iAIPerEraModifier;
	int m_iAIAdvancedStartPercent;
	int m_iNumGoodies;

	CvPropertyManipulators m_PropertyManipulators;

	CvString m_szHandicapName;

	IDValueMapPercent m_Percent;

	// Arrays

	int* m_piGoodies;

	bool* m_pbFreeTechs;
	bool* m_pbAIFreeTechs;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvGameSpeedInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameSpeedInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvGameSpeedInfo();
	virtual ~CvGameSpeedInfo();

	int getGrowthPercent() const;				// Exposed to Python
	int getTrainPercent() const;				// Exposed to Python
	int getConstructPercent() const;			// Exposed to Python
	int getCreatePercent() const;				// Exposed to Python
	int getResearchPercent() const;				// Exposed to Python
	int getBuildPercent() const;				// Exposed to Python
	int getImprovementPercent() const;			// Exposed to Python
	int getGreatPeoplePercent() const;			// Exposed to Python
	int getAnarchyPercent() const;				// Exposed to Python
	int getBarbPercent() const;					// Exposed to Python
	int getFeatureProductionPercent() const;	// Exposed to Python
	int getUnitDiscoverPercent() const;			// Exposed to Python
	int getUnitHurryPercent() const;			// Exposed to Python
	int getUnitTradePercent() const;			// Exposed to Python
	int getUnitGreatWorkPercent() const;		// Exposed to Python
	int getGoldenAgePercent() const;			// Exposed to Python
	int getHurryPercent() const;				// Exposed to Python
	int getHurryConscriptAngerPercent() const;	// Exposed to Python
	int getInflationOffset() const;				// Exposed to Python
	int getInflationPercent() const;			// Exposed to Python
	//ls612: Begin
	int getGoldModifier() const;
	int getOccupationTimePopulationPercent() const;
	//ls612: End
	int getVictoryDelayPercent() const;			// Exposed to Python
	int getNumTurnIncrements() const;			// Exposed to Python
	// Afforess 12/13/09
	int getUnitMovementPercent() const;

	GameTurnInfo& getGameTurnInfo(int iIndex) const;	// Exposed to Python
	CvDateIncrement& getDateIncrement(int iIndex);
	std::vector<CvDateIncrement>& getIncrements();
	bool getEndDatesCalculated();
	void setEndDatesCalculated(bool bCalculated);

	void allocateGameTurnInfos(const int iSize);

	int getPercent(int iID) const;

	//TB GameSpeed begin
	int getTraitGainPercent() const;
	//TB GameSpeed end

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvGameSpeedInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iGrowthPercent;
	int m_iTrainPercent;
	int m_iConstructPercent;
	int m_iCreatePercent;
	int m_iResearchPercent;
	int m_iBuildPercent;
	int m_iImprovementPercent;
	int m_iGreatPeoplePercent;
	int m_iAnarchyPercent;
	int m_iBarbPercent;
	int m_iFeatureProductionPercent;
	int m_iUnitDiscoverPercent;
	int m_iUnitHurryPercent;
	int m_iUnitTradePercent;
	int m_iUnitGreatWorkPercent;
	int m_iGoldenAgePercent;
	int m_iHurryPercent;
	int m_iHurryConscriptAngerPercent;
	int m_iInflationOffset;
	int m_iInflationPercent;
	//ls612: Begin
	int m_iGoldModifier;
	int m_iOccupationTimePopulationPercent;
	int m_iVictoryDelayPercent;
	int m_iNumTurnIncrements;
	// Afforess 12/13/09
	int m_iUnitMovementPercent;

	CvString m_szGameSpeedName;
	GameTurnInfo* m_pGameTurnInfo;
	std::vector<CvDateIncrement> m_aIncrements;
	bool m_bEndDatesCalculated;
	//TB GameSpeed begin
	int m_iTraitGainPercent;
	//TB GameSpeed end

	IDValueMapPercent m_Percent;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvTurnTimerInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTurnTimerInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvTurnTimerInfo();
	virtual ~CvTurnTimerInfo();

	int getBaseTime() const;				// Exposed to Python
	int getCityBonus() const;				// Exposed to Python
	int getUnitBonus() const;				// Exposed to Python
	int getFirstTurnMultiplier() const;		// Exposed to Python

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvTurnTimerInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iBaseTime;
	int m_iCityBonus;
	int m_iUnitBonus;
	int m_iFirstTurnMultiplier;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvWorldInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvWorldInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvWorldInfo();
	virtual ~CvWorldInfo();

	DllExport int getDefaultPlayers() const;		// Exposed to Python
	int getUnitNameModifier() const;				// Exposed to Python
	int getTargetNumCities() const;					// Exposed to Python
	int getNumFreeBuildingBonuses() const;			// Exposed to Python
	int getBuildingClassPrereqModifier() const;		// Exposed to Python
	int getMaxConscriptModifier() const;			// Exposed to Python
	int getWarWearinessModifier() const;			// Exposed to Python
	int getGridWidth() const;						// Exposed to Python
	int getGridHeight() const;						// Exposed to Python
	int getTerrainGrainChange() const;				// Exposed to Python
	int getFeatureGrainChange() const;				// Exposed to Python
	int getResearchPercent() const;					// Exposed to Python
	int getTradeProfitPercent() const;				// Exposed to Python
	int getDistanceMaintenancePercent() const;		// Exposed to Python
	int getNumCitiesMaintenancePercent() const;		// Exposed to Python
	int getColonyMaintenancePercent() const;		// Exposed to Python
	int getCorporationMaintenancePercent() const;	// Exposed to Python
	int getNumCitiesAnarchyPercent() const;			// Exposed to Python
	int getAdvancedStartPointsMod() const;			// Exposed to Python
	// Afforess 02/27/10
	int getCommandersLevelThresholdsPercent() const;

	bool read(CvXMLLoadUtility* pXML);

	int getPercent(int iID) const;

	void copyNonDefaults(CvWorldInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iDefaultPlayers;
	int m_iUnitNameModifier;
	int m_iTargetNumCities;
	int m_iNumFreeBuildingBonuses;
	int m_iBuildingClassPrereqModifier;
	int m_iMaxConscriptModifier;
	int m_iWarWearinessModifier;
	int m_iGridWidth;
	int m_iGridHeight;
	int m_iTerrainGrainChange;
	int m_iFeatureGrainChange;
	int m_iResearchPercent;
	int m_iTradeProfitPercent;
	int m_iDistanceMaintenancePercent;
	int m_iNumCitiesMaintenancePercent;
	int m_iColonyMaintenancePercent;
	int m_iCorporationMaintenancePercent;
	int m_iNumCitiesAnarchyPercent;
	int m_iAdvancedStartPointsMod;
	// Afforess 02/27/10
	int m_iCommandersLevelThresholdsPercent;

	IDValueMapPercent m_Percent;

};
/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvMapInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CvMapInfo : public CvInfoBase
{
public:
	CvMapInfo();
	virtual ~CvMapInfo();

	int getGridWidth() const;
	int getGridHeight() const;
	int getWrapX() const;
	int getWrapY() const;
	const CvString getInitialWBMap() const;
	const CvString getMapScript() const;
	/*bool isTechShareMap(MapTypes eMap) const;*/
	const TCHAR* getStartingEvent() const;
	int getSwitchType() const;

	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);
	bool readPass3();

protected:
	int m_iGridWidth;
	int m_iGridHeight;
	int m_iWrapX;
	int m_iWrapY;
	CvString m_szInitialWBMap;
	CvString m_szMapScript;
	/*bool* m_pabTechShareMaps;*/
	CvString m_szStartingEvent;
	int m_iSwitchType;
};

class CvMapSwitchInfo : public CvInfoBase
{
public:
	CvMapSwitchInfo();
	virtual ~CvMapSwitchInfo();

	int getMap() const;
	const TCHAR* getEvent() const;
	bool isManual() const;

	bool read(CvXMLLoadUtility* pXML);

protected:
	int m_iMap;
	CvString m_szEvent;
	bool m_bManual;
};

/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvClimateInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvClimateInfo :	public CvInfoBase
{
public:

	CvClimateInfo();
	virtual ~CvClimateInfo();

	int getDesertPercentChange() const;				// Exposed to Python
	int getJungleLatitude() const;					// Exposed to Python
	int getHillRange() const;						// Exposed to Python
	int getPeakPercent() const;						// Exposed to Python

	float getSnowLatitudeChange() const;			// Exposed to Python
	float getTundraLatitudeChange() const;			// Exposed to Python
	float getGrassLatitudeChange() const;			// Exposed to Python
	float getDesertBottomLatitudeChange() const;	// Exposed to Python
	float getDesertTopLatitudeChange() const;		// Exposed to Python
	float getIceLatitude() const;					// Exposed to Python
	float getRandIceLatitude() const;				// Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvClimateInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

protected:

	int m_iDesertPercentChange;
	int m_iJungleLatitude;
	int m_iHillRange;
	int m_iPeakPercent;

	float m_fSnowLatitudeChange;
	float m_fTundraLatitudeChange;
	float m_fGrassLatitudeChange;
	float m_fDesertBottomLatitudeChange;
	float m_fDesertTopLatitudeChange;
	float m_fIceLatitude;
	float m_fRandIceLatitude;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvSeaLevelInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSeaLevelInfo :	public CvInfoBase
{
public:

	CvSeaLevelInfo();
	virtual ~CvSeaLevelInfo();

	int getSeaLevelChange() const;		// Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvSeaLevelInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

protected:

	int m_iSeaLevelChange;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvVoteInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvVoteInfo :	public CvInfoBase
{
public:
	CvVoteInfo();
	virtual ~CvVoteInfo();

	int getPopulationThreshold() const;		// Exposed to Python
	int getStateReligionVotePercent() const;// Exposed to Python
	int getTradeRoutes() const;				// Exposed to Python
	int getMinVoters() const;				// Exposed to Python

	bool isSecretaryGeneral() const;		// Exposed to Python
	bool isVictory() const;					// Exposed to Python
	bool isFreeTrade() const;				// Exposed to Python
	bool isNoNukes() const;					// Exposed to Python
	bool isCityVoting() const;				// Exposed to Python
	bool isCivVoting() const;				// Exposed to Python
	bool isDefensivePact() const;			// Exposed to Python
	bool isOpenBorders() const;				// Exposed to Python
	bool isForcePeace() const;				// Exposed to Python
	bool isForceNoTrade() const;			// Exposed to Python
	bool isForceWar() const;				// Exposed to Python
	bool isAssignCity() const;				// Exposed to Python

	// Arrays

	bool isForceCivic(int i) const;			// Exposed to Python
	bool isVoteSourceType(int i) const;		// Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvVoteInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

protected:

	int m_iPopulationThreshold;
	int m_iStateReligionVotePercent;
	int m_iTradeRoutes;
	int m_iMinVoters;

	bool m_bSecretaryGeneral;
	bool m_bVictory;
	bool m_bFreeTrade;
	bool m_bNoNukes;
	bool m_bCityVoting;
	bool m_bCivVoting;
	bool m_bDefensivePact;
	bool m_bOpenBorders;
	bool m_bForcePeace;
	bool m_bForceNoTrade;
	bool m_bForceWar;
	bool m_bAssignCity;

	// Arrays

	bool* m_pbForceCivic;
	bool* m_abVoteSourceTypes;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvProjectInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvProjectInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvProjectInfo();
	virtual ~CvProjectInfo();

	int getVictoryPrereq() const;				// Exposed to Python
	int getTechPrereq() const;					// Exposed to Python
	int getAnyoneProjectPrereq() const;			// Exposed to Python
	void setAnyoneProjectPrereq(int i);
	int getMaxGlobalInstances() const;			// Exposed to Python
	int getMaxTeamInstances() const;			// Exposed to Python
	int getProductionCost() const;				// Exposed to Python
	int getNukeInterception() const;			// Exposed to Python
	int getTechShare() const;					// Exposed to Python
	//DPII < Maintenance Modifier >
	int getGlobalMaintenanceModifier() const;
	int getDistanceMaintenanceModifier() const;
	int getNumCitiesMaintenanceModifier() const;
	int getConnectedCityMaintenanceModifier() const;
	//DPII < Maintenance Modifier >
	int getEveryoneSpecialUnit() const;			// Exposed to Python
	int getEveryoneSpecialBuilding() const;		// Exposed to Python
	int getVictoryDelayPercent() const;			// Exposed to Python
	int getSuccessRate() const;					// Exposed to Python

	bool isSpaceship() const;					// Exposed to Python
	bool isAllowsNukes() const;					// Exposed to Python
	const char* getMovieArtDef() const;			// Exposed to Python

	const TCHAR* getCreateSound() const;		// Exposed to Python
	void setCreateSound(const TCHAR* szVal);

	// Arrays

	int getBonusProductionModifier(int i) const;// Exposed to Python
	int getVictoryThreshold(int i) const;		// Exposed to Python
	int getVictoryMinThreshold(int i) const;	// Exposed to Python
	int getProjectsNeeded(int i) const;			// Exposed to Python

	// Vectors
	int getMapCategoryType(int i) const;
	int getNumMapCategoryTypes() const;
	bool isMapCategoryType(int i);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvProjectInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

/************************************************************************************************/
/* Afforess					  Start		 07/30/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	int getWorldHappiness() const;
	int getGlobalHappiness() const;
	int getWorldHealth() const;
	int getGlobalHealth() const;
	int getWorldTradeRoutes() const;
	int getInflationModifier() const;
	bool isTechShareWithHalfCivs() const;
	int getCommerceModifier(int i) const;
	int* getCommerceModifierArray() const;
	int getProjectsNeededVectorSize();
	CvString getProjectsNeededNamesVectorElement(int i);
	int getProjectsNeededValuesVectorElement(int i);

	bool readPass3();
protected:
	int m_iWorldHappiness;
	int m_iGlobalHappiness;
	int m_iWorldHealth;
	int m_iGlobalHealth;
	int m_iWorldTradeRoutes;
	int m_iInflationModifier;
	bool m_bTechShareWithHalfCivs;
	int* m_piCommerceModifier;
	std::vector<CvString> m_aszProjectsNeededforPass3;
	std::vector<int> m_aiProjectsNeededforPass3;
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/


	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iVictoryPrereq;
	int m_iTechPrereq;
	int m_iAnyoneProjectPrereq;
	int m_iMaxGlobalInstances;
	int m_iMaxTeamInstances;
	int m_iProductionCost;
	int m_iNukeInterception;
	int m_iTechShare;
	//DPII < Maintenance Modifiers >
	int m_iGlobalMaintenanceModifier;
	int m_iDistanceMaintenanceModifier;
	int m_iNumCitiesMaintenanceModifier;
	int m_iConnectedCityMaintenanceModifier;
	//DPII < Maintenance Modifiers >
	int m_iEveryoneSpecialUnit;
	int m_iEveryoneSpecialBuilding;
	int m_iVictoryDelayPercent;
	int m_iSuccessRate;

	bool m_bSpaceship;
	bool m_bAllowsNukes;

	CvString m_szCreateSound;
	CvString m_szMovieArtDef;

	// Arrays

	int* m_piBonusProductionModifier;
	int* m_piVictoryThreshold;
	int* m_piVictoryMinThreshold;
	int* m_piProjectsNeeded;

	// Vectors
	std::vector<int> m_aiMapCategoryTypes;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSpaceShipInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpaceShipInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvSpaceShipInfo();
	virtual ~CvSpaceShipInfo();

	DllExport const TCHAR* getNodeName();
	void setNodeName(const TCHAR* szVal);
	DllExport const TCHAR* getProjectName();
	void setProjectName(const TCHAR* szVal);
	DllExport ProjectTypes getProjectType();
	DllExport AxisTypes getCameraUpAxis();
	DllExport SpaceShipInfoTypes getSpaceShipInfoType();
	DllExport int getPartNumber();
	DllExport int getArtType();
	DllExport int getEventCode();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvSpaceShipInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szNodeName;
	CvString m_szProjectName;
	ProjectTypes m_eProjectType;
	AxisTypes m_eCameraUpAxis;
	int m_iPartNumber;
	int m_iArtType;
	int m_iEventCode;
	SpaceShipInfoTypes m_eSpaceShipInfoType;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCultureLevelInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCultureLevelInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvCultureLevelInfo();
	virtual ~CvCultureLevelInfo();

	int getCityDefenseModifier() const;		// Exposed to Python

	// JOOYO_ADDON, Added by Jooyo, 06/17/09
	int getCityRadius() const;				// Exposed to python
	int getMaxWorldWonders() const;
	int getMaxTeamWonders() const;
	int getMaxNationalWonders() const;
	int getMaxNationalWondersOCC() const;
	int getPrereqGameOption() const;

	int getSpeedThreshold(int i) const;		// Exposed to Python

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvCultureLevelInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iCityDefenseModifier;
	int m_iCityRadius;
	int m_iMaxWorldWonders;
	int m_iMaxTeamWonders;
	int m_iMaxNationalWonders;
	int m_iMaxNationalWondersOCC;
	int m_iPrereqGameOption;

	int* m_paiSpeedThreshold;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEraInfo
//
//  DESC:   Used to manage different types of Art Styles
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEraInfo :
	public CvInfoBase
{
public:

	CvEraInfo();
	virtual ~CvEraInfo();

	int getStartingUnitMultiplier() const;		// Exposed to Python
	int getStartingDefenseUnits() const;		// Exposed to Python
	int getStartingWorkerUnits() const;			// Exposed to Python
	int getStartingExploreUnits() const;		// Exposed to Python
	int getAdvancedStartPoints() const;			// Exposed to Python
	int getStartingGold() const;				// Exposed to Python
	int getFreePopulation() const;				// Exposed to Python
	int getStartPercent() const;				// Exposed to Python
	int getGrowthPercent() const;				// Exposed to Python
	int getTrainPercent() const;				// Exposed to Python
	int getConstructPercent() const;			// Exposed to Python
	int getCreatePercent() const;				// Exposed to Python
	int getResearchPercent() const;				// Exposed to Python
	int getBuildPercent() const;				// Exposed to Python
	int getImprovementPercent() const;			// Exposed to Python
	int getGreatPeoplePercent() const;			// Exposed to Python
	int getAnarchyPercent() const;				// Exposed to Python
	int getEventChancePerTurn() const;			// Exposed to Python
	int getSoundtrackSpace() const;				// Exposed to Python
	int getNumSoundtracks() const;				// Exposed to Python
	int getBeelineStingsTechCostModifier() const;	// Exposed to Python
	// Afforess 09/12/10
	int getInitialCityMaintenancePercent() const;

	const TCHAR* getAudioUnitVictoryScript() const;	// Exposed to Python
	const TCHAR* getAudioUnitDefeatScript() const;	// Exposed to Python

	bool isNoGoodies() const;					// Exposed to Python
	bool isNoAnimals() const;					// Exposed to Python
	bool isNoBarbUnits() const;					// Exposed to Python
	bool isNoBarbCities() const;				// Exposed to Python
	bool isFirstSoundtrackFirst() const;		// Exposed to Python

	// Arrays

	int getSoundtracks(int i) const;
	int getCitySoundscapeSciptId(int i) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvEraInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

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
	int m_iBeelineStingsTechCostModifier;
	// Afforess 09/12/10
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

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvGameText
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameText : public CvInfoBase
{
public:
	DllExport CvGameText();

	const wchar* getText() const;
	void setText(const wchar* szText);

	// for Python
	std::wstring pyGetText() const { return getText(); }

	void setGender(const wchar* szGender) { m_szGender = szGender;	}
	const wchar* getGender() const { return m_szGender; }

	void setPlural(const wchar* szPlural) { m_szPlural = szPlural; }
	const wchar* getPlural() const { return m_szPlural; }

	DllExport int getNumLanguages() const; // not static for Python access
	DllExport void setNumLanguages(int iNum); // not static for Python access

	bool read(CvXMLLoadUtility* pXML);

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	static void setLanguage(const CvWString& szLanguage) { m_szLanguage = szLanguage; }

protected:

	static CvWString m_szLanguage;

	CvWString m_szText;
	CvWString m_szGender;
	CvWString m_szPlural;

	static int NUM_LANGUAGES;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvDiplomacyTextInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvDiplomacyTextInfo :	public CvInfoBase
{
	friend class CvXMLLoadUtility;		// so it can access private vars to initialize the class
public:
	struct Response
	{
		Response() :
			m_iNumDiplomacyText(0),
			m_pbCivilizationTypes(NULL),
			m_pbLeaderHeadTypes(NULL),
			m_pbAttitudeTypes(NULL),
			m_pbDiplomacyPowerTypes(NULL),
			m_paszDiplomacyText(NULL)
		{
		}

		virtual ~Response ()
		{
			SAFE_DELETE_ARRAY(m_pbCivilizationTypes);
			SAFE_DELETE_ARRAY(m_pbLeaderHeadTypes);
			SAFE_DELETE_ARRAY(m_pbAttitudeTypes);
			SAFE_DELETE_ARRAY(m_pbDiplomacyPowerTypes);
			SAFE_DELETE_ARRAY(m_paszDiplomacyText);
		}

		void read(FDataStreamBase* stream) {}
		void write(FDataStreamBase* stream) {}

		int m_iNumDiplomacyText;
		bool* m_pbCivilizationTypes;
		bool* m_pbLeaderHeadTypes;
		bool* m_pbAttitudeTypes;
		bool* m_pbDiplomacyPowerTypes;
		CvString* m_paszDiplomacyText;	// needs to be public for xml load assignment
	};

	CvDiplomacyTextInfo();
	virtual ~CvDiplomacyTextInfo() { uninit(); }	// free memory - MT

	// note - Response member vars allocated by CvXmlLoadUtility
	void init(int iNum);
	void uninit();

	const Response& getResponse(int iNum) const { return m_pResponses[iNum]; }	// Exposed to Python
	int getNumResponses() const;						// Exposed to Python

	bool getCivilizationTypes(int i, int j) const;		// Exposed to Python
	bool getLeaderHeadTypes(int i, int j) const;		// Exposed to Python
	bool getAttitudeTypes(int i, int j) const;			// Exposed to Python
	bool getDiplomacyPowerTypes(int i, int j) const;	// Exposed to Python

	int getNumDiplomacyText(int i) const;				// Exposed to Python

	const TCHAR* getDiplomacyText(int i, int j) const;	// Exposed to Python

	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}
	bool read(CvXMLLoadUtility* pXML);

private:

	int m_iNumResponses;			// set by init
	Response* m_pResponses;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvQuestInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvQuestInfo :
	public CvInfoBase
{
public:
	struct QuestLink
	{
		// Stores the QuestLinks Type and Name
		QuestLink() :
		m_szQuestLinkType("No Type"),
		m_szQuestLinkName("No Name")
		{
		}

	CvString m_szQuestLinkType;
	CvString m_szQuestLinkName;
	};

	CvQuestInfo();
	virtual ~CvQuestInfo();

	void reset();
	bool initQuestLinks(int iNum);

	int getNumQuestMessages() const;
	int getNumQuestLinks() const;
	int getNumQuestSounds() const;
	const TCHAR* getQuestObjective() const;
	const TCHAR* getQuestBodyText() const;
	const TCHAR* getQuestMessages(int iIndex) const;
	const TCHAR* getQuestLinkType(int iIndex) const;
	const TCHAR* getQuestLinkName(int iIndex) const;
	const TCHAR* getQuestSounds(int iIndex) const;
	const TCHAR* getQuestScript() const;

	void setNumQuestMessages(int iNum);
	void setNumQuestSounds(int iNum);
	void setQuestObjective(const TCHAR* szText);
	void setQuestBodyText(const TCHAR* szText);
	void setQuestMessages(int iIndex, const TCHAR* szText);
	void setQuestSounds(int iIndex, const TCHAR* szText);
	void setQuestScript(const TCHAR* szText);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvQuestInfo* pClassInfo, CvXMLLoadUtility* pXML);

private:
	int m_iNumQuestMessages;
	int m_iNumQuestLinks;
	int m_iNumQuestSounds;

	CvString m_szQuestObjective;
	CvString m_szQuestBodyText;
	CvString m_szQuestScript;

	CvString* m_paszQuestMessages;
	QuestLink* m_pQuestLinks;
	CvString* m_paszQuestSounds;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvTutorialInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTutorialMessage
{
public:
	CvTutorialMessage();
	virtual ~CvTutorialMessage();

	const TCHAR* getText() const;
	const TCHAR* getImage() const;
	const TCHAR* getSound() const;

	void setText(const TCHAR* szText);
	void setImage(const TCHAR* szText);
	void setSound(const TCHAR* szText);
	void setScript(int iIndex, const TCHAR* szText);

	int getNumTutorialScripts() const;
	const TCHAR* getTutorialScriptByIndex(int i) const;
	bool read(CvXMLLoadUtility* pXML);

private:
	int m_iNumTutorialScripts;
	CvString m_szTutorialMessageText;
	CvString m_szTutorialMessageImage;
	CvString m_szTutorialMessageSound;
	CvString* m_paszTutorialScripts;
};

class CvTutorialMessage;
class CvTutorialInfo :
	public CvInfoBase
{
public:
	CvTutorialInfo();
	virtual ~CvTutorialInfo();

	const TCHAR* getNextTutorialInfoType();
	void setNextTutorialInfoType(const TCHAR* szVal);

	bool initTutorialMessages(int iNum);
	void resetMessages();

	int getNumTutorialMessages() const;
	const CvTutorialMessage* getTutorialMessage(int iIndex) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvTutorialInfo* pClassInfo, CvXMLLoadUtility* pXML);

private:
	CvString m_szNextTutorialInfoType;
	int m_iNumTutorialMessages;
	CvTutorialMessage* m_paTutorialMessages;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvGameOptionInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGameOptionInfo :
	public CvInfoBase
{
public:
	CvGameOptionInfo();
	virtual ~CvGameOptionInfo();

	DllExport bool getDefault() const;
	DllExport bool getVisible() const;

	//TB Tags
	// int vector utilizing struct with delayed resolution
	int getNumEnforcesGameOptionOnTypes() const;
	GameOptionTypeBool& isEnforcesGameOptionOnType(int iOption);

	int getNumEnforcesGameOptionOffTypes() const;
	GameOptionTypeBool& isEnforcesGameOptionOffType(int iOption);

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvGameOptionInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

private:
	bool m_bDefault;
	bool m_bVisible;
	//TB Tags
	// int vector utilizing struct with delayed resolution
	std::vector<GameOptionTypeBool> m_aEnforcesGameOptionOnTypes;
	std::vector<GameOptionTypeBool> m_aEnforcesGameOptionOffTypes;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvMPOptionInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMPOptionInfo :
	public CvInfoBase
{
public:
	CvMPOptionInfo();
	virtual ~CvMPOptionInfo();

	bool getDefault() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvMPOptionInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

private:
	bool m_bDefault;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvForceControlInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvForceControlInfo :
	public CvInfoBase
{
public:
	CvForceControlInfo();
	virtual ~CvForceControlInfo();

	bool getDefault() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvForceControlInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

private:
	bool m_bDefault;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPlayerOptionInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPlayerOptionInfo :
	public CvInfoBase
{
public:
	CvPlayerOptionInfo();
	virtual ~CvPlayerOptionInfo();

	DllExport bool getDefault() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvPlayerOptionInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

private:
	bool m_bDefault;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvGraphicOptionInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGraphicOptionInfo :
	public CvInfoBase
{
public:
	CvGraphicOptionInfo();
	virtual ~CvGraphicOptionInfo();

	DllExport bool getDefault() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvGraphicOptionInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

private:
	bool m_bDefault;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEventTriggerInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEventTriggerInfo : public CvInfoBase
{
	friend class CvXMLLoadUtility;

public:
	CvEventTriggerInfo();
	virtual ~CvEventTriggerInfo();

	int getPercentGamesActive() const;			// Exposed to Python
	int getProbability() const;					// Exposed to Python
	int getNumUnits() const;					// Exposed to Python
	int getNumBuildings() const;				// Exposed to Python
	int getNumUnitsGlobal() const;				// Exposed to Python
	int getNumBuildingsGlobal() const;			// Exposed to Python
	int getNumPlotsRequired() const;			// Exposed to Python
	int getPlotType() const;					// Exposed to Python
	int getNumReligions() const;				// Exposed to Python
	int getNumCorporations() const;				// Exposed to Python
	int getOtherPlayerShareBorders() const;		// Exposed to Python
	int getOtherPlayerHasTech() const;			// Exposed to Python
	int getCityFoodWeight() const;				// Exposed to Python
	int getCivic() const;						// Exposed to Python
	int getMinPopulation() const;				// Exposed to Python
	int getMaxPopulation() const;				// Exposed to Python
	int getMinMapLandmass() const;				// Exposed to Python
	int getMinOurLandmass() const;				// Exposed to Python
	int getMaxOurLandmass() const;				// Exposed to Python
	int getMinDifficulty() const;				// Exposed to Python
	int getAngry() const;						// Exposed to Python
	int getUnhealthy() const;					// Exposed to Python
	int getUnitDamagedWeight() const;			// Exposed to Python
	int getUnitDistanceWeight() const;			// Exposed to Python
	int getUnitExperienceWeight() const;		// Exposed to Python
	int getMinTreasury() const;					// Exposed to Python

	int getBuildingRequired(int i) const;		// Exposed to Python
	int getNumBuildingsRequired() const;		// Exposed to Python
	int getUnitRequired(int i) const;			// Exposed to Python
	int getNumUnitsRequired() const;			// Exposed to Python
	int getPrereqOrTechs(int i) const;			// Exposed to Python
	int getNumPrereqOrTechs() const;			// Exposed to Python
	int getPrereqAndTechs(int i) const;			// Exposed to Python
	int getNumPrereqAndTechs() const;			// Exposed to Python
	int getObsoleteTech(int i) const;			// Exposed to Python
	int getNumObsoleteTechs() const;			// Exposed to Python
	int getEvent(int i) const;					// Exposed to Python
	int getNumEvents() const;					// Exposed to Python
	int getPrereqEvent(int i) const;			// Exposed to Python
	int getNumPrereqEvents() const;				// Exposed to Python
	int getFeatureRequired(int i) const;		// Exposed to Python
	int getNumFeaturesRequired() const;			// Exposed to Python
	int getTerrainRequired(int i) const;		// Exposed to Python
	int getNumTerrainsRequired() const;			// Exposed to Python
	int getImprovementRequired(int i) const;	// Exposed to Python
	int getNumImprovementsRequired() const;		// Exposed to Python
	int getBonusRequired(int i) const;			// Exposed to Python
	int getNumBonusesRequired() const;			// Exposed to Python
	int getRouteRequired(int i) const;			// Exposed to Python
	int getNumRoutesRequired() const;			// Exposed to Python
	int getReligionRequired(int i) const;		// Exposed to Python
	int getNumReligionsRequired() const;		// Exposed to Python
	int getCorporationRequired(int i) const;	// Exposed to Python
	int getNumCorporationsRequired() const;		// Exposed to Python

	//Property prereqs
	CvProperties* getPrereqMinProperties();
	CvProperties* getPrereqMaxProperties();
	CvProperties* getPrereqPlayerMinProperties();
	CvProperties* getPrereqPlayerMaxProperties();

	const CvWString& getText(int i) const;
	int getTextEra(int i) const;
	int getNumTexts() const;
	const CvWString& getWorldNews(int i) const;
	int getNumWorldNews() const;

// Start EmperorFool: Events with Images
	const TCHAR* getEventArt() const;			// Exposed to Python
// End EmperorFool: Events with Images

	bool isSinglePlayer() const;				// Exposed to Python
	bool isTeam() const;						// Exposed to Python
	bool isRecurring() const;					// Exposed to Python
	bool isGlobal() const;						// Exposed to Python
	bool isPickPlayer() const;					// Exposed to Python
	bool isOtherPlayerWar() const;				// Exposed to Python
	bool isOtherPlayerHasReligion() const;		// Exposed to Python
	bool isOtherPlayerHasOtherReligion() const;	// Exposed to Python
	bool isOtherPlayerAI() const;				// Exposed to Python
	bool isPickCity() const;					// Exposed to Python
	bool isPickOtherPlayerCity() const;			// Exposed to Python
	bool isShowPlot() const;					// Exposed to Python
	bool isUnitsOnPlot() const;					// Exposed to Python
	bool isOwnPlot() const;						// Exposed to Python
	bool isPickReligion() const;				// Exposed to Python
	bool isStateReligion() const;				// Exposed to Python
	bool isHolyCity() const;					// Exposed to Python
	bool isPickCorporation() const;				// Exposed to Python
	bool isHeadquarters() const;				// Exposed to Python
	bool isProbabilityUnitMultiply() const;		// Exposed to Python
	bool isProbabilityBuildingMultiply() const;	// Exposed to Python
	bool isPrereqEventCity() const;				// Exposed to Python

	const char* getPythonCallback() const;
	const char* getPythonCanDo() const;
	const char* getPythonCanDoCity() const;
	const char* getPythonCanDoUnit() const;

	// bool vector without delayed resolution
	int getNotOnGameOption(int i) const;
	int getNumNotOnGameOptions() const;
	bool isNotOnGameOption(int i);

	int getOnGameOption(int i) const;
	int getNumOnGameOptions() const;
	bool isOnGameOption(int i);

	void read(FDataStreamBase* ) {}
	void write(FDataStreamBase* ) {}

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvEventTriggerInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

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

	// bool vector without delayed resolution
	std::vector<int> m_aiNotOnGameOptions;
	std::vector<int> m_aiOnGameOptions;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEventInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEventInfo : public CvInfoBase
{
	friend class CvXMLLoadUtility;

public:
	CvEventInfo();
	virtual ~CvEventInfo();

	bool isQuest() const;						// Exposed to Python
	bool isGlobal() const;						// Exposed to Python
	bool isTeam() const;						// Exposed to Python
	bool isCityEffect() const;					// Exposed to Python
	bool isOtherPlayerCityEffect() const;		// Exposed to Python
	bool isGoldToPlayer() const;				// Exposed to Python
	bool isGoldenAge() const;					// Exposed to Python
	bool isDeclareWar() const;					// Exposed to Python
	bool isDisbandUnit() const;					// Exposed to Python

	int getGold() const;						// Exposed to Python
	int getRandomGold() const;					// Exposed to Python
	int getEspionagePoints() const;				// Exposed to Python
	int getCulture() const;						// Exposed to Python
	int getTech() const;						// Exposed to Python
	int getTechPercent() const;					// Exposed to Python
	int getTechCostPercent() const;				// Exposed to Python
	int getTechMinTurnsLeft() const;			// Exposed to Python
	int getPrereqTech() const;					// Exposed to Python
	int getUnitClass() const;					// Exposed to Python
	int getNumUnits() const;					// Exposed to Python
	int getBuildingClass() const;				// Exposed to Python
	int getBuildingChange() const;				// Exposed to Python
	int getHappy() const;						// Exposed to Python
	int getHealth() const;						// Exposed to Python
	int getHurryAnger() const;					// Exposed to Python
	int getHappyTurns() const;					// Exposed to Python
	int getFood() const;						// Exposed to Python
	int getFoodPercent() const;					// Exposed to Python
	int getFeature() const;						// Exposed to Python
	int getFeatureChange() const;				// Exposed to Python
	int getImprovement() const;					// Exposed to Python
	int getImprovementChange() const;			// Exposed to Python
	int getCommerceModifier(int i) const;
	int getYieldModifier(int i) const;
	int getBonus() const;						// Exposed to Python
	int getBonusChange() const;					// Exposed to Python
	int getRoute() const;						// Exposed to Python
	int getRouteChange() const;					// Exposed to Python
	int getBonusRevealed() const;				// Exposed to Python
	int getBonusGift() const;					// Exposed to Python
	int getUnitExperience() const;				// Exposed to Python
	int getUnitImmobileTurns() const;			// Exposed to Python
	int getConvertOwnCities() const;			// Exposed to Python
	int getConvertOtherCities() const;			// Exposed to Python
	int getMaxNumReligions() const;				// Exposed to Python
	int getOurAttitudeModifier() const;			// Exposed to Python
	int getAttitudeModifier() const;			// Exposed to Python
	int getTheirEnemyAttitudeModifier() const;	// Exposed to Python
	int getPopulationChange() const;			// Exposed to Python
	int getRevoltTurns() const;					// Exposed to Python
	int getMinPillage() const;					// Exposed to Python
	int getMaxPillage() const;					// Exposed to Python
	int getUnitPromotion() const;				// Exposed to Python
	int getFreeUnitSupport() const;				// Exposed to Python
	int getInflationModifier() const;			// Exposed to Python
	int getSpaceProductionModifier() const;		// Exposed to Python
	int getAIValue() const;	// Exposed to Python

	int getAdditionalEventChance(int i) const;	// Exposed to Python
	int getAdditionalEventTime(int i) const;	// Exposed to Python
	int getClearEventChance(int i) const;		// Exposed to Python
	int getTechFlavorValue(int i) const;		// Exposed to Python
	int getPlotExtraYield(int i) const;			// Exposed to Python
	int getFreeSpecialistCount(int i) const;	// Exposed to Python
	int getUnitCombatPromotion(int i) const;	// Exposed to Python
	int getUnitClassPromotion(int i) const;		// Exposed to Python
	const CvWString& getWorldNews(int i) const;
	int getNumWorldNews() const;

	int getBuildingYieldChange(int iBuildingClass, int iYield) const;
	int getNumBuildingYieldChanges() const;
	int getBuildingCommerceChange(int iBuildingClass, int iCommerce) const;
	int getNumBuildingCommerceChanges() const;
	int getBuildingYieldModifier(int iBuildingClass, int iYield) const;
	int getNumBuildingYieldModifiers() const;
	int getBuildingCommerceModifier(int iBuildingClass, int iCommerce) const;
	int getNumBuildingCommerceModifiers() const;
	int getBuildingHappyChange(int iBuildingClass) const;
	int getNumBuildingHappyChanges() const;
	int getBuildingHealthChange(int iBuildingClass) const;
	int getNumBuildingHealthChanges() const;

	CvProperties* getProperties();
	CvProperties* getPropertiesAllCities();

	const char* getPythonCallback() const;
	const char* getPythonExpireCheck() const;
	const char* getPythonCanDo() const;
	const char* getPythonHelp() const;
	const wchar* getUnitNameKey() const;
	const wchar* getQuestFailTextKey() const;
	const wchar* getOtherPlayerPopup() const;
	const wchar* getLocalInfoTextKey() const;

/************************************************************************************************/
/* Afforess					  Start		 07/30/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	int getPrereqGameOption() const;
	int getRevolutionIndexChange() const;
	int getAdditionalEventChanceVectorSize();
	CvString getAdditionalEventChanceNamesVectorElement(int i);
	int getAdditionalEventChanceValuesVectorElement(int i);

	int getAdditionalEventTimeVectorSize();
	CvString getAdditionalEventTimeNamesVectorElement(int i);
	int getAdditionalEventTimeValuesVectorElement(int i);

	int getClearEventChanceVectorSize();
	CvString getClearEventChanceNamesVectorElement(int i);
	int getClearEventChanceValuesVectorElement(int i);

	bool readPass3();

private:
	std::vector<CvString> m_aszAdditionalEventChanceforPass3;
	std::vector<int> m_aiAdditionalEventChanceforPass3;
	std::vector<CvString> m_aszAdditionalEventTimeforPass3;
	std::vector<int> m_aiAdditionalEventTimeforPass3;
	std::vector<CvString> m_aszClearEventChanceforPass3;
	std::vector<int> m_aiClearEventChanceforPass3;
public:
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/


	void read(FDataStreamBase* ) {}
	void write(FDataStreamBase* ) {}

	bool read(CvXMLLoadUtility* pXML);
//	bool readPass2(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvEventInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

private:
	int m_iPrereqGameOption;
	int m_iRevolutionIndexChange;

	bool m_bQuest;
	bool m_bGlobal;
	bool m_bTeam;
	bool m_bCityEffect;
	bool m_bOtherPlayerCityEffect;
	bool m_bGoldToPlayer;
	bool m_bGoldenAge;
	bool m_bDeclareWar;
	bool m_bDisbandUnit;

	int m_iGold;
	int m_iRandomGold;
	int m_iCulture;
	int m_iEspionagePoints;
	int m_iTech;
	int m_iTechPercent;
	int m_iTechCostPercent;
	int m_iTechMinTurnsLeft;
	int m_iPrereqTech;
	int m_iUnitClass;
	int m_iNumUnits;
	int m_iBuildingClass;
	int m_iBuildingChange;
	int m_iHappy;
	int m_iHealth;
	int m_iHurryAnger;
	int m_iHappyTurns;
	int m_iFood;
	int m_iFoodPercent;
	int m_iFeature;
	int m_iFeatureChange;
	int m_iImprovement;
	int m_iImprovementChange;
	int m_iBonus;
	int m_iBonusChange;
	int m_iRoute;
	int m_iRouteChange;
	int m_iBonusRevealed;
	int m_iBonusGift;
	int m_iUnitExperience;
	int m_iUnitImmobileTurns;
	int m_iConvertOwnCities;
	int m_iConvertOtherCities;
	int m_iMaxNumReligions;
	int m_iOurAttitudeModifier;
	int m_iAttitudeModifier;
	int m_iTheirEnemyAttitudeModifier;
	int m_iPopulationChange;
	int m_iRevoltTurns;
	int m_iMinPillage;
	int m_iMaxPillage;
	int m_iUnitPromotion;
	int m_iFreeUnitSupport;
	int m_iInflationModifier;
	int m_iSpaceProductionModifier;
	int m_iAIValue;

	CvProperties m_Properties;
	CvProperties m_PropertiesAllCities;

	int* m_piTechFlavorValue;
	int* m_piPlotExtraYields;
	int* m_piFreeSpecialistCount;
	int* m_piAdditionalEventChance;
	int* m_piAdditionalEventTime;
	int* m_piClearEventChance;
	int* m_piUnitCombatPromotions;
	int* m_piUnitClassPromotions;
	int* m_piCommerceModifier;
	int* m_piYieldModifier;

	std::vector<BuildingYieldChange> m_aBuildingYieldChanges;
	std::vector<BuildingCommerceChange> m_aBuildingCommerceChanges;
	std::vector<BuildingCommerceChange> m_aBuildingCommerceModifiers;
	BuildingChangeArray m_aBuildingHappyChanges;
	BuildingChangeArray m_aBuildingHealthChanges;

	CvString m_szPythonCallback;
	CvString m_szPythonExpireCheck;
	CvString m_szPythonCanDo;
	CvString m_szPythonHelp;
	CvWString m_szUnitName;
	CvWString m_szOtherPlayerPopup;
	CvWString m_szQuestFailText;
	CvWString m_szLocalInfoText;
	std::vector<CvWString> m_aszWorldNews;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvEspionageMissionInfo
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvEspionageMissionInfo : public CvInfoBase
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

	int getTechPrereq() const;
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

/************************************************************************************************/
/* Afforess					  Start		 01/31/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	bool isNuke() const;
	bool isRevolt() const;
	bool isDisablePower() const;
	int getPrereqGameOption() const;
	int getWarWearinessCounter() const;
	int getSabatogeResearchCostFactor() const;
	int getRemoveReligionsCostFactor() const;
	int getRemoveCorporationsCostFactor() const;
protected:
	bool m_bNuke;
	bool m_bRevolt;
	bool m_bDisablePower;
	int m_iPrereqGameOption;
	int m_iWarWearinessCounter;
	int m_iSabatogeResearchCostFactor;
	int m_iRemoveReligionsCostFactor;
	int m_iRemoveCorporationsCostFactor;
public:
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvEspionageMissionInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iCost;
	bool m_bIsPassive;
	bool m_bIsTwoPhases;
	bool m_bTargetsCity;
	bool m_bSelectPlot;

	int m_iTechPrereq;
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
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvVoteSourceInfo
//
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvVoteSourceInfo : public CvInfoBase
{
public:

	CvVoteSourceInfo();
	virtual ~CvVoteSourceInfo();

	int getVoteInterval() const;				// Exposed to Python
	int getFreeSpecialist() const;				// Exposed to Python
	int getCivic() const;						// Exposed to Python
	const CvWString getPopupText() const;
	const CvWString getSecretaryGeneralText() const;
	const CvString& getCopyPopupText() const;
	const CvString& getCopySecretaryGeneralText() const;

	std::wstring pyGetSecretaryGeneralText() { return getSecretaryGeneralText(); }	// Exposed to Python

	int getReligionYield(int i) const;			// Exposed to Python
	int getReligionCommerce(int i) const;		// Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	bool readPass3();

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvVoteSourceInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

protected:
	int m_iVoteInterval;
	int m_iFreeSpecialist;
	int m_iCivic;

	int* m_aiReligionYields;
	int* m_aiReligionCommerces;

	CvString m_szPopupText;
	CvString m_szSecretaryGeneralText;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPropertyInfo
//
//  DESC:   Contains info about generic properties which can be added to buildings
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPropertyInfo :
	public CvInfoBase
{
public:

	CvPropertyInfo();
	virtual ~CvPropertyInfo();

	int getAIWeight() const;
	AIScaleTypes getAIScaleType() const;
	int getOperationalRangeMin() const;
	int getOperationalRangeMax() const;
	int getTargetLevel() const;
	int getTrainReluctance() const;
	bool isSourceDrain() const;
	bool isOAType() const;
	CvWString getValueDisplayText() const;
	CvWString getChangeDisplayText() const;
	CvWString getChangeAllCitiesDisplayText() const;
	CvWString getPrereqMinDisplayText() const;
	CvWString getPrereqMaxDisplayText() const;
	PropertyBuilding& getPropertyBuilding(int index);
	int getNumPropertyBuildings() const;
	PropertyPromotion& getPropertyPromotion(int index);
	int getNumPropertyPromotions() const;

	int getChangePropagator(const GameObjectTypes eFrom, const GameObjectTypes eTo) const;

	int getNumTargetLevelbyEraTypes() const;
	int getTargetLevelbyEraType(int iIndex) const;
	bool isTargetLevelbyEraType(int iIndex);

	int getChar() const;
	void setChar(int i);
	int getFontButtonIndex() const;

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvPropertyInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

	CvPropertyManipulators* getPropertyManipulators();

protected:

	int m_iChar;
	int m_iFontButtonIndex;

	int m_iAIWeight;
	AIScaleTypes m_eAIScaleType;
	int m_iOperationalRangeMin;
	int m_iOperationalRangeMax;
	int m_iTargetLevel;
	int m_iTrainReluctance;
	bool m_bSourceDrain;
	bool m_bOAType;
	CvWString m_szValueDisplayText;
	CvWString m_szChangeDisplayText;
	CvWString m_szChangeAllCitiesDisplayText;
	CvWString m_szPrereqMinDisplayText;
	CvWString m_szPrereqMaxDisplayText;

	int m_aaiChangePropagator[NUM_GAMEOBJECTS][NUM_GAMEOBJECTS]; // from/to, in percent

	std::vector<PropertyBuilding> m_aPropertyBuildings;
	std::vector<PropertyPromotion> m_aPropertyPromotions;
	EraArray m_aTargetLevelbyEraTypes;

	CvPropertyManipulators m_PropertyManipulators;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvIdeaClassInfo
//
//  DESC:   Contains info about Idea Class Types
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvIdeaClassInfo :
	public CvInfoBase
{
public:

	CvIdeaClassInfo();
	virtual ~CvIdeaClassInfo();

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvIdeaClassInfo* pClassInfo, CvXMLLoadUtility* pXML);
	void copyNonDefaultsReadPass2(CvIdeaClassInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL, bool bOver = false);

	void getCheckSum(unsigned int& iSum);

	//bools
	bool isInitialized();

protected:
	//bools
	bool m_bInitialized;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvIdeaInfo
//
//  DESC:   Contains info about Idea Types
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvIdeaInfo :
	public CvInfoBase
{
public:

	CvIdeaInfo();
	virtual ~CvIdeaInfo();

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvIdeaInfo* pClassInfo, CvXMLLoadUtility* pXML);
	void copyNonDefaultsReadPass2(CvIdeaInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL, bool bOver = false);

	void getCheckSum(unsigned int& iSum);

	//References
	IdeaClassTypes getIdeaClass() const;
	//bools
	/*bool isInitialized();*/

protected:
	//References
	IdeaClassTypes m_eIdeaClass;

	//bools
	/*bool m_bInitialized;*/
};

#endif /* CV_INFO_GAME_H */
