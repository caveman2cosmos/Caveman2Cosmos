#ifndef CV_INFO_TERRAIN_H
#define CV_INFO_TERRAIN_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvRiverInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvRiverInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvRiverInfo();
	virtual ~CvRiverInfo();

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvRiverModelInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvRiverModelInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvRiverModelInfo();
	virtual ~CvRiverModelInfo();

	DllExport const TCHAR* getModelFile() const;		// Exposed to Python
	void setModelFile(const TCHAR* szVal);				// The model filename
	DllExport const TCHAR* getBorderFile() const;		// Exposed to Python
	void setBorderFile(const TCHAR* szVal);				// The model filename

	DllExport int getTextureIndex() const;
	DllExport const TCHAR* getDeltaString() const;		//Exposed to Python
	DllExport const TCHAR* getConnectString() const;	// Exposed to Python
	DllExport const TCHAR* getRotateString() const;		// Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvRiverModelInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	CvString m_szModelFile;					// The model filename
	CvString m_szBorderFile;				// The border filename

	TCHAR		m_szDeltaString[32];		// The delta type
	TCHAR		m_szConnectString[32];		// The connections this cell makes ( N S E W NE NW SE SW )
	TCHAR		m_szRotateString[32];		// The possible rotations for this cell ( 0 90 180 270 )
	int			m_iTextureIndex;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvRouteModelInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvRouteModelInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvRouteModelInfo();
	virtual ~CvRouteModelInfo();

	DllExport RouteTypes getRouteType() const;			// The route type

	DllExport const TCHAR* getModelFile() const;		// Exposed to Python
	void setModelFile(const TCHAR* szVal);				// The model filename
	DllExport const TCHAR* getLateModelFile() const;	// Exposed to Python
	void setLateModelFile(const TCHAR* szVal);			// The model filename
	const TCHAR* getModelFileKey() const;				// Exposed to Python
	void setModelFileKey(const TCHAR* szVal);			// The model filename Key

	DllExport bool isAnimated() const;

	DllExport const TCHAR* getConnectString() const;		// Exposed to Python
	DllExport const TCHAR* getModelConnectString() const;	// Exposed to Python
	DllExport const TCHAR* getRotateString() const;			// Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvRouteModelInfo* pClassInfo = NULL , CvXMLLoadUtility* pXML = NULL);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	RouteTypes	m_eRouteType;			// The route type

	CvString m_szModelFile;				// The model filename
	CvString m_szLateModelFile;			// The model filename
	CvString m_szModelFileKey;			// The model file key reference
	bool m_bAnimated;

	TCHAR	m_szConnectString[32];		// The connections this cell makes ( N S E W NE NW SE SW )
	TCHAR	m_szModelConnectString[32];	// The connections this model makes ( N S E W NE NW SE SW )
	TCHAR	m_szRotateString[32];		// The possible rotations for this cell ( 0 90 180 270 )
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvGoodyInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvGoodyInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvGoodyInfo();
	virtual ~CvGoodyInfo();

	int getGold() const;				// Exposed to Python
	int getGoldRand1() const;			// Exposed to Python
	int getGoldRand2() const;			// Exposed to Python
	int getMapOffset() const;			// Exposed to Python
	int getMapRange() const;			// Exposed to Python
	int getMapProb() const;				// Exposed to Python
	int getExperience() const;			// Exposed to Python
	int getHealing() const;				// Exposed to Python
	int getDamagePrereq() const;		// Exposed to Python
	int getBarbarianUnitProb() const;	// Exposed to Python
	int getMinBarbarians() const;		// Exposed to Python
	int getUnitClassType() const;		// Exposed to Python
	int getBarbarianUnitClass() const;	// Exposed to Python
	int getEraType() const;
	int getNotEraType() const;
	int getResearch() const;

	bool isTech() const;				// Exposed to Python
	bool isBad() const;					// Exposed to Python
	bool isNaval() const;

	//Vectors
	int getMapCategoryType(int i) const;
	int getNumMapCategoryTypes() const;
	bool isMapCategoryType(int i);

	const TCHAR* getSound() const;		// Exposed to Python
	void setSound(const TCHAR* szVal);

	bool read(CvXMLLoadUtility* pXML);
	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}

	void copyNonDefaults(CvGoodyInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iGold;
	int m_iGoldRand1;
	int m_iGoldRand2;
	int m_iMapOffset;
	int m_iMapRange;
	int m_iMapProb;
	int m_iExperience;
	int m_iHealing;
	int m_iDamagePrereq;
	int m_iBarbarianUnitProb;
	int m_iMinBarbarians;
	int m_iUnitClassType;
	int m_iBarbarianUnitClass;
	int m_iEraType;
	int m_iNotEraType;
	int m_iResearch;

	bool m_bTech;
	bool m_bBad;
	bool m_bNaval;

	std::vector<int> m_aiMapCategoryTypes;

	CvString m_szSound;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvRouteInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvRouteInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvRouteInfo();
	virtual ~CvRouteInfo();

	int getAdvancedStartCost() const;			// Exposed to Python
	int getAdvancedStartCostIncrease() const;	// Exposed to Python

	int getValue() const;						// Exposed to Python
	int getMovementCost() const;				// Exposed to Python
	int getFlatMovementCost() const;			// Exposed to Python
	int getPrereqBonus() const;					// Exposed to Python

	// JOOYO_ADDON, Added by Jooyo, 07/07/09
	bool isSeaTunnel() const;

	// Arrays
	int getYieldChange(int i) const;			// Exposed to Python
	int* getYieldChangeArray();
	int getTechMovementChange(int i) const;		// Exposed to Python
	int getPrereqOrBonus(int i) const;			// Exposed to Python
	bool isAnyPrereqOrBonus() const;

	CvPropertyManipulators* getPropertyManipulators();
	//	This really belongs on CvInfoBase but you can't change the size of that
	//	object without crashing the core engine :-(
	inline int	getZobristValue() const { return m_zobristValue; }

	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}
	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvRouteInfo* pClassInfo = NULL , CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iAdvancedStartCost;
	int m_iAdvancedStartCostIncrease;

	int m_iValue;
	int m_iMovementCost;
	int m_iFlatMovementCost;
	int m_iPrereqBonus;
	
	int m_zobristValue;

	// JOOYO_ADDON, Added by Jooyo, 07/07/09
	bool m_bSeaTunnel;

	bool m_bAnyPrereqOrBonus;

	// Arrays
	int* m_piYieldChange;
	int* m_piTechMovementChange;
	int* m_piPrereqOrBonuses;

	CvPropertyManipulators m_PropertyManipulators;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvImprovementBonusInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvImprovementBonusInfo :
	public CvInfoBase
{

friend class CvImprovementInfo;
friend class CvXMLLoadUtility;

	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvImprovementBonusInfo();
	virtual ~CvImprovementBonusInfo();

	int getDiscoverRand() const;		// Exposed to Python
	// Afforess 01/20/10
	int getDepletionRand() const;

	bool isBonusMakesValid() const;		// Exposed to Python
	bool isObsoleteBonusMakesValid() const;
	bool isBonusTrade() const;			// Exposed to Python

	int getYieldChange(int i) const;	// Exposed to Python

	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iDiscoverRand;
	// Afforess 01/20/10
	int m_iDepletionRand;

	bool m_bBonusMakesValid;
	bool m_bObsoleteBonusMakesValid;
	bool m_bBonusTrade;

	// Arrays
	int* m_piYieldChange;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvImprovementInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoImprovement;
class CvImprovementInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvImprovementInfo();
	virtual ~CvImprovementInfo();

	int getAdvancedStartCost() const;			// Exposed to Python
	int getAdvancedStartCostIncrease() const;	// Exposed to Python

	int getTilesPerGoody() const;				// Exposed to Python
	int getGoodyUniqueRange() const;			// Exposed to Python
	int getFeatureGrowthProbability() const;	// Exposed to Python
	int getUpgradeTime() const;					// Exposed to Python
	int getAirBombDefense() const;				// Exposed to Python
	int getDefenseModifier() const;				// Exposed to Python
	int getHappiness() const;					// Exposed to Python
	int getPillageGold() const;					// Exposed to Python
	int getImprovementPillage() const;			// Exposed to Python
	void setImprovementPillage(int i);
	int getImprovementUpgrade() const;			// Exposed to Python
	void setImprovementUpgrade(int i);
	// Super Forts begin *XML*
	int getCulture() const;
	int getCultureRange() const;
	int getVisibilityChange() const;
	int getSeeFrom() const;
	int getUniqueRange() const;
	bool isBombardable() const;
	bool isUpgradeRequiresFortify() const;
	// Super Forts end
	// Super forts C2C adaptation
	bool isUniversalTradeBonusProvider() const;
	// Super forts C2C adaptation end

	bool isZOCSource() const;					// Exposed to Python
	bool isActsAsCity() const;					// Exposed to Python
	bool isHillsMakesValid() const;				// Exposed to Python
	bool isFreshWaterMakesValid() const;		// Exposed to Python
	bool isRiverSideMakesValid() const;			// Exposed to Python
	bool isNoFreshWater() const;				// Exposed to Python
	bool isRequiresFlatlands() const;			// Exposed to Python
	DllExport bool isRequiresRiverSide() const;	// Exposed to Python
	bool isRequiresPeak() const;
	bool isRequiresIrrigation() const;			// Exposed to Python
	bool isCarriesIrrigation() const;			// Exposed to Python
	bool isRequiresFeature() const;				// Exposed to Python
	bool isWater() const;						// Exposed to Python
	DllExport bool isGoody() const;				// Exposed to Python
	bool isPermanent() const;					// Exposed to Python
	bool isOutsideBorders() const;				// Exposed to Python

	const TCHAR* getArtDefineTag() const;
	void setArtDefineTag(const TCHAR* szVal);

	int getWorldSoundscapeScriptId() const;

	// Arrays

	int getPrereqNatureYield(int i) const;		// Exposed to Python
	int* getPrereqNatureYieldArray();
	int getYieldChange(int i) const;			// Exposed to Python
	int* getYieldChangeArray();
	int getRiverSideYieldChange(int i) const;	// Exposed to Python
	int* getRiverSideYieldChangeArray();
	int getHillsYieldChange(int i) const;		// Exposed to Python
	int* getHillsYieldChangeArray();
	int getIrrigatedYieldChange(int i) const;	// Exposed to Python
	int* getIrrigatedYieldChangeArray();		// For Moose - CvWidgetData XXX

	bool getTerrainMakesValid(int i) const;		// Exposed to Python
	bool getFeatureMakesValid(int i) const;		// Exposed to Python

	int getTechYieldChanges(int i, int j) const;	// Exposed to Python
	int* getTechYieldChangesArray(int i);
	int getRouteYieldChanges(int i, int j) const;	// Exposed to Python
	int* getRouteYieldChangesArray(int i);		// For Moose - CvWidgetData XXX

	int getImprovementBonusYield(int i, int j) const;	// Exposed to Python
	bool isImprovementBonusMakesValid(int i) const;		// Exposed to Python
	bool isImprovementObsoleteBonusMakesValid(int i) const;	// Exposed to Python
	bool isImprovementBonusTrade(int i) const;			// Exposed to Python
	int getImprovementBonusDiscoverRand(int i) const;	// Exposed to Python

	// Other
	int getMapCategoryType(int i) const;
	int getNumMapCategoryTypes() const;
	bool isMapCategoryType(int i);

	const TCHAR* getButton() const;
	DllExport const CvArtInfoImprovement* getArtInfo() const;

	// Afforess 12/9/09
	int getHealthPercent() const;		// Exposed to Python
	bool isPeakMakesValid() const;		// Exposed to Python
	bool isDepletedMine() const;
	int getDepletionRand() const;
	int getImprovementBonusDepletionRand(int i) const;
	int getPrereqTech() const;			// Exposed to Python
	//int getTraitYieldChanges(int i, int j) const;
	//int* getTraitYieldChangesArray(int i);

	//TB Improvements
	//Object Indexes
	int getBonusChange() const;
	//booleans
	bool isCanMoveSeaUnits() const;
	bool isChangeRemove() const;
	bool isNotOnAnyBonus() const;
	bool isNational() const;
	bool isGlobal() const;
	// bool vector with delayed resolution
	int getAlternativeImprovementUpgradeType(int i) const;
	int getNumAlternativeImprovementUpgradeTypes() const;
	bool isAlternativeImprovementUpgradeType(int i);

	int getFeatureChangeType(int i) const;
	int getNumFeatureChangeTypes() const;
	bool isFeatureChangeType(int i);

	//Post Load Functions
	void setHighestCost();
	int getHighestCostBuild() const;
	int getHighestCost() const;

protected:
	bool m_bPeakMakesValid;
	int m_iHealthPercent;
	bool m_bDepletedMine;
	int m_iDepletionRand;
	int m_iPrereqTech;
	//int** m_ppiTraitYieldChanges;

	std::vector<int> m_aiMapCategoryTypes;
public:
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}
	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvImprovementInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void copyNonDefaultsReadPass2(CvImprovementInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL, bool bOver = false);

	void getCheckSum(unsigned int& iSum);

	CvPropertyManipulators* getPropertyManipulators();

	static ImprovementTypes getImprovementCity();
	static void setImprovementCity(ImprovementTypes eIndex);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iAdvancedStartCost;
	int m_iAdvancedStartCostIncrease;

	int m_iTilesPerGoody;
	int m_iGoodyUniqueRange;
	int m_iFeatureGrowthProbability;
	int m_iUpgradeTime;
	int m_iAirBombDefense;
	int m_iDefenseModifier;
	int m_iHappiness;
	int m_iPillageGold;
	int m_iImprovementPillage;
	int m_iImprovementUpgrade;
	// Super Forts begin *XML*
	int m_iCulture;
	int m_iCultureRange;
	int m_iVisibilityChange;
	int m_iSeeFrom;
	int m_iUniqueRange;
	bool m_bBombardable;
	bool m_bUpgradeRequiresFortify;
	// Super Forts end
	// Super forts C2C adaptation
	bool m_bIsUniversalTradeBonusProvider;
	bool m_bIsZOCSource;
	// Super forts C2C adaptation end

	bool m_bActsAsCity;
	bool m_bHillsMakesValid;
	bool m_bFreshWaterMakesValid;
	bool m_bRiverSideMakesValid;
	bool m_bNoFreshWater;
	bool m_bRequiresFlatlands;
	bool m_bRequiresRiverSide;
	bool m_bRequiresPeak;
	bool m_bRequiresIrrigation;
	bool m_bCarriesIrrigation;
	bool m_bRequiresFeature;
	bool m_bWater;
	bool m_bGoody;
	bool m_bPermanent;
	bool m_bOutsideBorders;

	CvString m_szArtDefineTag;

	int m_iWorldSoundscapeScriptId;

	// Arrays

	int* m_piPrereqNatureYield;
	int* m_piYieldChange;
	int* m_piRiverSideYieldChange;
	int* m_piHillsYieldChange;
	int* m_piIrrigatedChange;

	bool* m_pbTerrainMakesValid;
	bool* m_pbFeatureMakesValid;

	int** m_ppiTechYieldChanges;
	int** m_ppiRouteYieldChanges;

	CvImprovementBonusInfo* m_paImprovementBonus;

	CvPropertyManipulators m_PropertyManipulators;

	//TB Improvements
	//Object Indexes
	int m_iBonusChange;
	//booleans
	bool m_bCanMoveSeaUnits;
	bool m_bChangeRemove;
	bool m_bNotOnAnyBonus;
	bool m_bNational;
	bool m_bGlobal;
	// bool vector with delayed resolution
	std::vector<int> m_aiAlternativeImprovementUpgradeTypes;
	std::vector<int> m_aiFeatureChangeTypes;

	//Post Load Functions
	int m_eHighestCostBuild;
	int m_iHighestCost;
	static ImprovementTypes m_ImprovementCity;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvBonusClassInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBonusClassInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvBonusClassInfo();
	virtual ~CvBonusClassInfo();

	int getUniqueRange() const;				// Exposed to Python

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvBonusClassInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iUniqueRange;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvBonusInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoBonus;
class CvBonusInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvBonusInfo();
	virtual ~CvBonusInfo();

	int getBonusClassType() const;		// Exposed to Python
	int getChar() const;				// Exposed to Python
	void setChar(int i);				// Exposed to Python
	int getTechReveal() const;			// Exposed to Python
	int getTechCityTrade() const;		// Exposed to Python
	int getTechObsolete() const;		// Exposed to Python
	int getAITradeModifier() const;		// Exposed to Python
	int getAIObjective() const;			// Exposed to Python
	int getHealth() const;				// Exposed to Python
	int getHappiness() const;			// Exposed to Python
	int getMinAreaSize() const;			// Exposed to Python
	int getMinLatitude() const;			// Exposed to Python
	int getMaxLatitude() const;			// Exposed to Python
	int getPlacementOrder() const;		// Exposed to Python
	int getConstAppearance() const;		// Exposed to Python
	int getRandAppearance1() const;		// Exposed to Python
	int getRandAppearance2() const;		// Exposed to Python
	int getRandAppearance3() const;		// Exposed to Python
	int getRandAppearance4() const;		// Exposed to Python
	int getPercentPerPlayer() const;	// Exposed to Python
	int getTilesPer() const;			// Exposed to Python
	int getMinLandPercent() const;		// Exposed to Python
	int getUniqueRange() const;			// Exposed to Python
	int getGroupRange() const;			// Exposed to Python
	int getGroupRand() const;			// Exposed to Python

	bool isOneArea() const;				// Exposed to Python
	bool isHills() const;				// Exposed to Python

	// Afforess	Mountains 08/03/09
	bool isPeaks() const;				// Exposed to Python
	bool isCurrency() const;

	bool isFlatlands() const;			// Exposed to Python
	bool isNoRiverSide() const;			// Exposed to Python
	bool isNormalize() const;			// Exposed to Python

	const TCHAR* getArtDefineTag() const;
	void setArtDefineTag(const TCHAR* szVal);

	// Arrays
	int getYieldChange(int i) const;	// Exposed to Python
	int* getYieldChangeArray();			// Exposed to Python
	int getImprovementChange(int i) const;

	bool isTerrain(int i) const;		// Exposed to Python
	bool isFeature(int i) const;		// Exposed to Python
	bool isFeatureTerrain(int i) const;	// Exposed to Python

	//Vectors
	int getMapCategoryType(int i) const;
	int getNumMapCategoryTypes() const;
	bool isMapCategoryType(int i);


	int getNumAfflictionCommunicabilityTypes() const;
	PromotionLineAfflictionModifier getAfflictionCommunicabilityType(int iPromotionLine, bool bWorkedTile = false, bool bVicinity = false, bool bAccessVolume = false);

	// Other
	const TCHAR* getButton() const;		// Exposed to Python
	DllExport const CvArtInfoBonus* getArtInfo() const;	// Exposed to Python
	CvPropertyManipulators* getPropertyManipulators();

	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvBonusInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void getCheckSum(unsigned int& iSum);

	const std::vector<std::pair<ImprovementTypes,BuildTypes> >*	getTradeProvidingImprovements();

	//---------------------------------------PUBLIC MEMBER VARIABLES---------------------------------

protected:

	int m_iBonusClassType;
	int m_iChar;
	int m_iTechReveal;
	int m_iTechCityTrade;
	int m_iTechObsolete;
	int m_iAITradeModifier;
	int m_iAIObjective;
	int m_iHealth;
	int m_iHappiness;
	int m_iMinAreaSize;
	int m_iMinLatitude;
	int m_iMaxLatitude;
	int m_iPlacementOrder;
	int m_iConstAppearance;
	int m_iRandAppearance1;
	int m_iRandAppearance2;
	int m_iRandAppearance3;
	int m_iRandAppearance4;
	int m_iPercentPerPlayer;
	int m_iTilesPer;
	int m_iMinLandPercent;
	int m_iUniqueRange;
	int m_iGroupRange;
	int m_iGroupRand;

	bool m_bOneArea;
	bool m_bHills;
/************************************************************************************************/
/* Afforess	Mountains Start		 08/03/09										   		 */
/*																							  */
/*																							  */
/************************************************************************************************/
	bool m_bPeaks;
	bool m_bCurrency;
/************************************************************************************************/
/* Afforess	Mountains End	   END															 */
/************************************************************************************************/
	bool m_bFlatlands;
	bool m_bNoRiverSide;
	bool m_bNormalize;

	CvString m_szArtDefineTag;

	// Arrays

	int* m_piYieldChange;
	int* m_piImprovementChange;

	bool* m_pbTerrain;
	bool* m_pbFeature;
	bool* m_pbFeatureTerrain;

	std::vector<int> m_aiMapCategoryTypes;

	std::vector<PromotionLineAfflictionModifier> m_aAfflictionCommunicabilityTypes;

	CvPropertyManipulators m_PropertyManipulators;

	volatile std::vector<std::pair<ImprovementTypes,BuildTypes> >* m_tradeProvidingImprovements;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvFeatureInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoFeature;
class CvFeatureInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvFeatureInfo();
	virtual ~CvFeatureInfo();

	int getMovementCost() const;							// Exposed to Python
	int getSeeThroughChange() const;					// Exposed to Python
	int getHealthPercent() const;							// Exposed to Python
	int getAppearanceProbability() const;			// Exposed to Python
	int getDisappearanceProbability() const;	// Exposed to Python
	int getGrowthProbability() const;					// Exposed to Python
	int getDefenseModifier() const;						// Exposed to Python
	int getAdvancedStartRemoveCost() const;						// Exposed to Python
	int getTurnDamage() const;						// Exposed to Python
	int getWarmingDefense() const; //GWMod new xml field M.A.
	int getPopDestroys() const;

	bool isNoCoast() const;						// Exposed to Python
	bool isNoRiver() const;						// Exposed to Python
	bool isNoAdjacent() const;				// Exposed to Python
	bool isRequiresFlatlands() const;	// Exposed to Python
	bool isRequiresRiver() const;			// Exposed to Python
	bool isAddsFreshWater() const;		// Exposed to Python
	bool isImpassable() const;				// Exposed to Python
	bool isNoCity() const;						// Exposed to Python
	bool isNoImprovement() const;			// Exposed to Python
	bool isVisibleAlways() const;			// Exposed to Python
	bool isNukeImmune() const;			// Exposed to Python
// BUG - City Plot Status - start
	bool isOnlyBad() const;				// Exposed to Python
	bool isCountsAsPeak() const;
// BUG - City Plot Status - end
	const TCHAR* getOnUnitChangeTo() const;

	const TCHAR* getArtDefineTag() const;
	void setArtDefineTag(const TCHAR* szTag);

	int getWorldSoundscapeScriptId() const;

	const TCHAR* getEffectType() const;
	int getEffectProbability() const;

	// Arrays

	int getYieldChange(int i) const;						// Exposed to Python
	int getRiverYieldChange(int i) const;				// Exposed to Python
	int getHillsYieldChange(int i) const;				// Exposed to Python
	int get3DAudioScriptFootstepIndex(int i) const;

	bool isTerrain(int i) const;								// Exposed to Python
	int getNumVarieties() const;

	bool canBeSecondary() const;

	// Other
	int getMapCategoryType(int i) const;
	int getNumMapCategoryTypes() const;
	bool isMapCategoryType(int i);

	int getNumAfflictionCommunicabilityTypes() const;
	PromotionLineAfflictionModifier getAfflictionCommunicabilityType(int iPromotionLine, bool bWorkedTile = false, bool bVicinity = false, bool bAccessVolume = false);

	DllExport const CvArtInfoFeature* getArtInfo() const;
	const TCHAR* getButton() const;

	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}

	bool read(CvXMLLoadUtility* pXML);

/************************************************************************************************/
/* Afforess					  Start		 01/31/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	const TCHAR* getGrowthSound() const;
	void setGrowthSound(const TCHAR* szVal);
	int getSpreadProbability() const;
	int getCultureDistance() const;
	bool isIgnoreTerrainCulture() const;
	bool isCanGrowAnywhere() const;
protected:
	CvString m_szGrowthSound;
	int m_iSpreadProbability;
	int m_iCultureDistance;
	bool m_bIgnoreTerrainCulture;
	bool m_bCanGrowAnywhere;

	std::vector<int> m_aiMapCategoryTypes;
public:
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	void copyNonDefaults(CvFeatureInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

	CvPropertyManipulators* getPropertyManipulators();

	std::vector<PromotionLineAfflictionModifier> m_aAfflictionCommunicabilityTypes;
	//	This really belongs on CvInfoBase but you can't change the size of that
	//	object without crashing the core engine :-(
	inline int	getZobristValue() const { return m_zobristValue; }

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iMovementCost;
	int m_iSeeThroughChange;
	int m_iHealthPercent;
	int m_iAppearanceProbability;
	int m_iDisappearanceProbability;
	int m_iGrowthProbability;
	int m_iDefenseModifier;
	int m_iAdvancedStartRemoveCost;
	int m_iTurnDamage;
	int m_iWarmingDefense; //GWMod new xml field M.A.
	int m_iPopDestroys;

	bool m_bNoCoast;
	bool m_bNoRiver;
	bool m_bNoAdjacent;
	bool m_bRequiresFlatlands;
	bool m_bRequiresRiver;
	bool m_bAddsFreshWater;
	bool m_bImpassable;
	bool m_bNoCity;
	bool m_bNoImprovement;
	bool m_bVisibleAlways;
	bool m_bNukeImmune;
	bool m_bCountsAsPeak;
	CvString m_szOnUnitChangeTo;

	int m_iWorldSoundscapeScriptId;

	CvString m_szEffectType;
	int m_iEffectProbability;

	// Arrays

	int* m_piYieldChange;
	int* m_piRiverYieldChange;
	int* m_piHillsYieldChange;
	int* m_pi3DAudioScriptFootstepIndex;

	bool* m_pbTerrain;

	CvPropertyManipulators m_PropertyManipulators;

private:

	CvString m_szArtDefineTag;
	int m_zobristValue;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCommerceInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCommerceInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvCommerceInfo();
	virtual ~CvCommerceInfo();

	int getChar() const;				// Exposed to Python
	void setChar(int i);
	int getInitialPercent() const;				// Exposed to Python
	int getInitialHappiness() const;				// Exposed to Python
	int getAIWeightPercent() const;				// Exposed to Python

	bool isFlexiblePercent() const;				// Exposed to Python

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvCommerceInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iChar;
	int m_iInitialPercent;
	int m_iInitialHappiness;
	int m_iAIWeightPercent;

	bool m_bFlexiblePercent;

};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvYieldInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvYieldInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvYieldInfo();
	virtual ~CvYieldInfo();

	int getChar() const;				// Exposed to Python
	void setChar(int i);
	int getHillsChange() const;				// Exposed to Python
	int getPeakChange() const;				// Exposed to Python
	int getLakeChange() const;				// Exposed to Python
	int getCityChange() const;				// Exposed to Python
	int getPopulationChangeOffset() const;				// Exposed to Python
	int getPopulationChangeDivisor() const;				// Exposed to Python
	int getMinCity() const;				// Exposed to Python
	int getTradeModifier() const;				// Exposed to Python
	int getGoldenAgeYield() const;				// Exposed to Python
	int getGoldenAgeYieldThreshold() const;				// Exposed to Python
	int getAIWeightPercent() const;				// Exposed to Python
	int getColorType() const;				// Exposed to Python

	// Arrays

	const TCHAR* getSymbolPath(int i) const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvYieldInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iChar;
	int m_iHillsChange;
	int m_iPeakChange;
	int m_iLakeChange;
	int m_iCityChange;
	int m_iPopulationChangeOffset;
	int m_iPopulationChangeDivisor;
	int m_iMinCity;
	int m_iTradeModifier;
	int m_iGoldenAgeYield;
	int m_iGoldenAgeYieldThreshold;
	int m_iAIWeightPercent;
	int m_iColorType;

	CvString* m_paszSymbolPath;

};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvTerrainInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoTerrain;
class CvTerrainInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvTerrainInfo();
	virtual ~CvTerrainInfo();

	int getMovementCost() const;				// Exposed to Python
	int getSeeFromLevel() const;				// Exposed to Python
	int getSeeThroughLevel() const;			// Exposed to Python
	int getBuildModifier() const;				// Exposed to Python
	int getDefenseModifier() const;			// Exposed to Python

	bool isWater() const;								// Exposed to Python
	bool isImpassable() const;					// Exposed to Python
	bool isFound() const;								// Exposed to Python
	bool isFoundCoast() const;					// Exposed to Python
	bool isFoundFreshWater() const;			// Exposed to Python

	DllExport const TCHAR* getArtDefineTag() const;
	void setArtDefineTag(const TCHAR* szTag);

	int getWorldSoundscapeScriptId() const;

	// Arrays

	int getYield(int i) const;				// Exposed to Python
	int getRiverYieldChange(int i) const;				// Exposed to Python
	int getHillsYieldChange(int i) const;				// Exposed to Python
	int get3DAudioScriptFootstepIndex(int i) const;

	// Other
	int getMapCategoryType(int i) const;
	int getNumMapCategoryTypes() const;
	bool isMapCategoryType(int i);

	int getNumAfflictionCommunicabilityTypes() const;
	PromotionLineAfflictionModifier getAfflictionCommunicabilityType(int iPromotionLine, bool bWorkedTile = false, bool bVicinity = false, bool bAccessVolume = false);

	const CvArtInfoTerrain* getArtInfo() const;
	const TCHAR* getButton() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvTerrainInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

	CvPropertyManipulators* getPropertyManipulators();

	static TerrainTypes getTerrainPeak()	{ return m_TerrainPeak; }
	static void setTerrainPeak(TerrainTypes eIndex)	{ m_TerrainPeak = eIndex; }
	static TerrainTypes getTerrainHill()	{ return m_TerrainHill; }
	static void setTerrainHill(TerrainTypes eIndex) { m_TerrainHill = eIndex; }

	//	This really belongs on CvInfoBase but you can't change the size of that
	//	object without crashing the core engine :-(
	inline int	getZobristValue() const { return m_zobristValue; }

/************************************************************************************************/
/* Afforess					  Start		 04/21/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	int getCultureDistance() const;
	int getHealthPercent() const;
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	//TB Combat Mod begin
	bool isColdDamage() const;
	//TB Combat Mod end

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iMovementCost;
	int m_iSeeFromLevel;
	int m_iSeeThroughLevel;
	int m_iBuildModifier;
	int m_iDefenseModifier;

	bool m_bWater;
	bool m_bImpassable;
	bool m_bFound;
	bool m_bFoundCoast;
	bool m_bFoundFreshWater;

	int m_iWorldSoundscapeScriptId;

	// Arrays

	int* m_piYields;
	int* m_piRiverYieldChange;
	int* m_piHillsYieldChange;
	int* m_pi3DAudioScriptFootstepIndex;
/************************************************************************************************/
/* Afforess					  Start		 04/21/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	int m_iCultureDistance;
	int m_iHealthPercent;
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	std::vector<int> m_aiMapCategoryTypes;

	std::vector<PromotionLineAfflictionModifier> m_aAfflictionCommunicabilityTypes;

	CvPropertyManipulators m_PropertyManipulators;
	//TB Combat Mods begin
	bool m_bColdDamage;
	//TB Combat Mods end

	static TerrainTypes m_TerrainPeak;
	static TerrainTypes m_TerrainHill;
private:

	CvString m_szArtDefineTag;
	int	m_zobristValue;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvMapCategoryInfo
//
//  DESC:   Contains info about Map Category Types
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMapCategoryInfo :
	public CvInfoBase
{
public:

	CvMapCategoryInfo();
	virtual ~CvMapCategoryInfo();

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvMapCategoryInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

	//bools
	bool isInitialized();

protected:
	//bools
	bool m_bInitialized;
};

#endif /* CV_INFO_TERRAIN_H */
