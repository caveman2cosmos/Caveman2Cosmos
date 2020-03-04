#pragma once

#ifndef CyCity_h__
#define CyCity_h__

#include <string>
#ifndef __INTELLISENSE__
#include <boost/python/tuple.hpp>
namespace python = boost::python;
#endif

struct OrderData;
class CvCity;
class CvProperties;
class CyPlot;
class CyArea;
class CyUnit;

//
// Python wrapper class for CvCity
//
class CyCity
{
public:
	CyCity();
	DllExport explicit CyCity(CvCity* pCity);		// Call from C++

	CvCity* getCity() { return m_pCity;	}	// Call from C++
	bool isNone() { return (m_pCity==NULL); }
	void kill();

/************************************************************************************************/
/* REVOLUTION_MOD                         03/29/09                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int getRevolutionIndex();
	void setRevolutionIndex( int iNewValue );
	void changeRevolutionIndex( int iChange );

	int getLocalRevIndex();
	void setLocalRevIndex( int iNewValue );
	void changeLocalRevIndex( int iChange );

	int getRevIndexAverage() const;
	void setRevIndexAverage( int iNewValue );
	void updateRevIndexAverage( );

	int getRevolutionCounter();
	void setRevolutionCounter( int iNewValue );
	void changeRevolutionCounter( int iChange );

	int getReinforcementCounter();
	void setReinforcementCounter( int iNewValue );
	void changeReinforcementCounter( int iChange );

	bool isRecentlyAcquired();
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
	void createGreatPeople(int /*UnitTypes*/ eGreatPersonUnit, bool bIncrementThreshold, bool bIncrementExperience);

	void doTask(int /*TaskTypes*/ eTask, int iData1, int iData2, bool bOption);
	void chooseProduction(int /*UnitTypes*/ eTrainUnit, int /*BuildingTypes*/ eConstructBuilding, int /*ProjectTypes*/ eCreateProject, bool bFinish, bool bFront);
	int getCityPlotIndex(CyPlot* pPlot);
	CyPlot* getCityIndexPlot(int iIndex);
	bool canWork(CyPlot* pPlot);
	void clearWorkingOverride(int iIndex);
	int countNumImprovedPlots();
	int countNumWaterPlots();
	int countNumRiverPlots();

	int findPopulationRank();
	int findBaseYieldRateRank(int /*YieldTypes*/ eYield);
	int findYieldRateRank(int /*YieldTypes*/ eYield);
	int findCommerceRateRank(int /*CommerceTypes*/ eCommerce);

	bool isSupersedingUnitAvailable(int /*UnitTypes*/ eUnit);
	bool isPlotTrainable(int /*UnitTypes*/ eUnit, bool bContinue, bool bTestVisible);

	int /*UnitTypes*/ allUpgradesAvailable(int /*UnitTypes*/ eUnit, int iUpgradeCount);
	bool isWorldWondersMaxed();
	bool isTeamWondersMaxed();
	bool isNationalWondersMaxed();
	bool isBuildingsMaxed();
	int getMaxNumWorldWonders();
	int getMaxNumTeamWonders();
	int getMaxNumNationalWonders();

	bool canTrain( int iUnit, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreUpgrades );
	bool canConstruct( int iBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost);
	bool canCreate( int iProject, bool bContinue, bool bTestVisible );
	bool canMaintain( int iProcess, bool bContinue );
	bool canJoin();
	int getFoodTurnsLeft();
	bool isProduction();
	bool isProductionLimited();
	bool isProductionUnit();
	bool isProductionBuilding();
	bool isProductionProject();
	bool isProductionProcess();

	bool canContinueProduction(OrderData order);
	int getProductionExperience(int /*UnitTypes*/ eUnit);
	void addProductionExperience(CyUnit* pUnit, bool bConscript);

	int /*UnitTypes*/ getProductionUnit();
	int /*UnitAITypes*/ getProductionUnitAI();
	int /*BuildingTypes*/ getProductionBuilding();
	int /*ProjectTypes*/ getProductionProject();
	int /*ProcessTypes*/ getProductionProcess();
	std::wstring getProductionName();
	std::wstring getProductionNameKey();
	int getGeneralProductionTurnsLeft();
	bool isFoodProduction();
	int getFirstUnitOrder(int /*UnitTypes*/ eUnit);
	int getFirstProjectOrder(int /*ProjectTypes*/ eProject);
	int getNumTrainUnitAI(int /*UnitAITypes*/ eUnitAI);
	int getFirstBuildingOrder(int /*BuildingTypes*/ eBuilding);
	bool isUnitFoodProduction(int /*UnitTypes*/ iUnit);
	int getProduction();
	int getProductionNeeded();
	int getProductionTurnsLeft();
	int getUnitProductionTurnsLeft(int /*UnitTypes*/ iUnit, int iNum);
	int getBuildingProductionTurnsLeft(int /*BuildingTypes*/ iBuilding, int iNum);
	int getProjectProductionTurnsLeft(int /*ProjectTypes*/ eProject, int iNum);
	void setProduction(int iNewValue);
	void changeProduction(int iChange);
	int getProductionModifier();
	int getCurrentProductionDifference(bool bIgnoreFood, bool bOverflow);
	int getBuildingProductionModifier(int /*BuildingTypes*/ iBuilding);
	int getProjectProductionModifier(int /*ProjectTypes*/ eProject);

	int getExtraProductionDifference(int iExtra);

	bool canHurry(int /*HurryTypes*/ iHurry, bool bTestVisible);
	void hurry(int /*HurryTypes*/ iHurry);
	int /*UnitTypes*/ getConscriptUnit();
	int getConscriptPopulation();
	int conscriptMinCityPopulation();
	int flatConscriptAngerLength();
	bool canConscript();
	void conscript();
	int getBonusHealth(int /*BonusTypes*/ iBonus);
	int getBonusHappiness(int /*BonusTypes*/ iBonus);
	int getBonusPower(int /*BonusTypes*/ eBonus, bool bDirty);
	int getBonusYieldRateModifier(int /*YieldTypes*/ eIndex, int /*BonusTypes*/ eBonus);
	int /* HandicapTypes */ getHandicapType();
	int /* CivilizationTypes */ getCivilizationType();
	int /*LeaderHeadTypes*/ getPersonalityType();
	int /*ArtStyleTypes*/ getArtStyleType();
	int /*CitySizeTypes*/ getCitySizeType();

	bool hasTrait(int /*TraitTypes*/ iTrait);
	bool isBarbarian();
	bool isNPC();
	bool isHominid();
	bool isHuman();
	bool isVisible(int /*TeamTypes*/ eTeam, bool bDebug);

	bool isCapital();
	bool isCoastal(int iMinWaterSize);
	bool isDisorder();
	bool isHolyCityByType(int /*ReligionTypes*/ iIndex);
	bool isHolyCity();
	bool isHeadquartersByType(int /*CorporationTypes*/ iIndex);
	bool isHeadquarters();
	int getOvercrowdingPercentAnger(int iExtra);
	int getNoMilitaryPercentAnger();
	int getCulturePercentAnger();
	int getReligionPercentAnger();
	int getWarWearinessPercentAnger();
/************************************************************************************************/
/* REVOLUTION_MOD                         04/19/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int getRevRequestPercentAnger();
	int getRevIndexPercentAnger();
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
	int getLargestCityHappiness();
	int unhappyLevel(int iExtra);
	int happyLevel();
	int angryPopulation(int iExtra);
	int totalFreeSpecialists();
	int extraPopulation();
	int extraSpecialists();
	int extraFreeSpecialists();
	int unhealthyPopulation(bool bNoAngry, int iExtra);
	int totalGoodBuildingHealth();
	int totalBadBuildingHealth();
	int goodHealth();
	int badHealth(bool bNoAngry);
	int healthRate(bool bNoAngry, int iExtra);
	int foodConsumption(bool bNoAngry, int iExtra);
	int foodDifference(bool bBottom);
	int growthThreshold();
	int productionLeft();
	int hurryCost(bool bExtra);
	int hurryGold(int /*HurryTypes*/ iHurry);
	int hurryPopulation(int /*HurryTypes*/ iHurry);
	int hurryProduction(int /*HurryTypes*/ iHurry);
	int flatHurryAngerLength();
	int hurryAngerLength(int /*HurryTypes*/ iHurry);
	int maxHurryPopulation();

	int cultureDistance(int iDX, int iDY);
	int cultureStrength(int /*PlayerTypes*/ ePlayer);
	int cultureGarrison(int /*PlayerTypes*/ ePlayer);
	int getNumBuilding(int /*BuildingTypes*/ iIndex);
	bool isHasBuilding(int /*BuildingTypes*/ iIndex);		// This is a function to help modders out, since it was replaced with getNumBuildings() in the C++
	int getNumActiveBuilding(int /*BuildingTypes*/ iIndex);
	int getID();
	int getX();
	int getY();
	bool at(int iX, int iY);
	bool atPlot(CyPlot* pPlot);
	CyPlot* plot();
	bool isConnectedTo(CyCity* pCity);
	bool isConnectedToCapital(int /*PlayerTypes*/ ePlayer);
	CyArea* area();
	CyArea* waterArea();
	CyPlot* getRallyPlot();

	int getGameTurnFounded();
	int getGameTurnAcquired();
	int getPopulation();
	void setPopulation(int iNewValue);
	void changePopulation(int iChange);
	long getRealPopulation();

	int getHighestPopulation();
	void setHighestPopulation(int iNewValue);
	int getWorkingPopulation();
	int getSpecialistPopulation();
	int getNumGreatPeople();
	int getBaseGreatPeopleRate();
	int getGreatPeopleRate();
	int getTotalGreatPeopleRateModifier();
	void changeBaseGreatPeopleRate(int iChange);
	int getGreatPeopleRateModifier();
// BUG - Building Additional Great People - start
	int getAdditionalGreatPeopleRateByBuilding(int /*BuildingTypes*/ iBuilding);
	int getAdditionalBaseGreatPeopleRateByBuilding(int /*BuildingTypes*/ iBuilding);
	int getAdditionalGreatPeopleRateModifierByBuilding(int /*BuildingTypes*/ iBuilding);
// BUG - Building Additional Great People - end
	int getGreatPeopleProgress();
	void changeGreatPeopleProgress(int iChange);
	int getNumWorldWonders();
	int getNumTeamWonders();
	int getNumNationalWonders();
	int getNumBuildings();
	bool isGovernmentCenter();
// BUG - Building Saved Maintenance - start
	int getSavedMaintenanceByBuilding(int /*BuildingTypes*/ iBuilding) const;
	int getSavedMaintenanceTimes100ByBuilding(int /*BuildingTypes*/ iBuilding) const;
// BUG - Building Saved Maintenance - end
	int getMaintenance() const;
	int getMaintenanceTimes100() const;
	int calculateDistanceMaintenance() const;
	int calculateDistanceMaintenanceTimes100() const;
	int calculateNumCitiesMaintenance() const;
	int calculateNumCitiesMaintenanceTimes100() const;
	int calculateColonyMaintenanceTimes100() const;
	int calculateColonyMaintenance() const;
	int calculateCorporationMaintenanceTimes100() const;
	int calculateCorporationMaintenance() const;
	int getMaintenanceModifier();
	int getWarWearinessModifier();
	int getHurryAngerModifier();
	void changeHealRate(int iChange);

	int getEspionageHealthCounter();
	void changeEspionageHealthCounter(int iChange);
	int getEspionageHappinessCounter();
	void changeEspionageHappinessCounter(int iChange);

	int getFreshWaterGoodHealth();
	int getFreshWaterBadHealth();
	int getBuildingGoodHealth();
	int getBuildingBadHealth();
	int getFeatureGoodHealth();
	int getFeatureBadHealth();
// BUG - Feature Health - start
	int getAdditionalHealthByFeature(int /*FeatureTypes*/ eFeature, int iChange);
	int getAdditionalGoodHealthByFeature(int /*FeatureTypes*/ eFeature, int iChange);
	int getAdditionalBadHealthByFeature(int /*FeatureTypes*/ eFeature, int iChange);
// BUG - Feature Health - end
	int getBuildingHealth(int iBuilding);
// BUG - Building Additional Health - start
	int getAdditionalHealthByBuilding(int /*BuildingTypes*/ eBuilding);
	int getAdditionalGoodHealthByBuilding(int /*BuildingTypes*/ eBuilding);
	int getAdditionalBadHealthByBuilding(int /*BuildingTypes*/ eBuilding);
	int getAdditionalSpoiledFoodByBuilding(int /*BuildingTypes*/ eBuilding);
	int getAdditionalStarvationByBuilding(int /*BuildingTypes*/ eBuilding);
// BUG - Building Additional Health - end
	int getPowerGoodHealth();
	int getPowerBadHealth();
	int getBonusGoodHealth();
	int getBonusBadHealth();
	int getMilitaryHappiness();
	int getMilitaryHappinessUnits();
	int getBuildingGoodHappiness();
	int getBuildingBadHappiness();
	int getBuildingHappiness(int iBuilding);
// BUG - Building Additional Happiness - start
	int getAdditionalHappinessByBuilding(int /*BuildingTypes*/ eBuilding);
	int getAdditionalGoodHappinessByBuilding(int /*BuildingTypes*/ eBuilding);
	int getAdditionalBadHappinessByBuilding(int /*BuildingTypes*/ eBuilding);
	int getAdditionalAngryPopulationByBuilding(int /*BuildingTypes*/ eBuilding);
// BUG - Building Additional Happiness - end
	int getExtraBuildingGoodHappiness();
	int getExtraBuildingBadHappiness();
	int getFeatureGoodHappiness();
	int getFeatureBadHappiness();
	int getBonusGoodHappiness();
	int getBonusBadHappiness();
	int getReligionGoodHappiness();
	int getReligionBadHappiness();
	int getReligionHappiness(int iReligion);
	int getExtraHappiness();
	int getExtraHealth();
	void changeExtraHappiness(int iChange);
	void changeExtraHealth(int iChange);
	int getHurryAngerTimer();
	void changeHurryAngerTimer(int iChange);
/************************************************************************************************/
/* REVOLUTION_MOD                         04/28/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int getRevRequestAngerTimer();
	void changeRevRequestAngerTimer(int iChange);
	int getRevSuccessTimer();
	void changeRevSuccessTimer(int iChange);
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
	int getConscriptAngerTimer();
	void changeConscriptAngerTimer(int iChange);
	int getDefyResolutionAngerTimer();
	void changeDefyResolutionAngerTimer(int iChange);
	int flatDefyResolutionAngerLength();
	int getHappinessTimer();
	void changeHappinessTimer(int iChange);
	bool isNoUnhappiness();
	bool isNoUnhealthyPopulation();

	bool isBuildingOnlyHealthy();

	int getFood();
	void setFood(int iNewValue);
	void changeFood(int iChange);
	int getFoodKept();
	int getMaxFoodKeptPercent();
	int getOverflowProduction();
	void setOverflowProduction(int iNewValue);
	int getFeatureProduction();
	void setFeatureProduction(int iNewValue);
	int getMilitaryProductionModifier();
	int getSpaceProductionModifier();
	int getExtraTradeRoutes();
	void changeExtraTradeRoutes(int iChange);
	int getTradeRouteModifier();
	int getForeignTradeRouteModifier();
	int getBuildingDefense();
// BUG - Building Additional Defense - start
	int getAdditionalDefenseByBuilding(int /*BuildingTypes*/ eBuilding);
// BUG - Building Additional Defense - end
	int getBuildingBombardDefense();
	int getFreeExperience();
	int getCurrAirlift();
	int getMaxAirlift();
	int getAirModifier();
	int getAirUnitCapacity(int /*TeamTypes*/ eTeam);
	int getNukeModifier();
	int getFreeSpecialist();
	bool isPower();
	bool isAreaCleanPower();
	bool isDirtyPower();
	int getDefenseDamage();
	void changeDefenseDamage(int iChange);
	bool isBombardable(CyUnit* pUnit);
	int getNaturalDefense();
	int getTotalDefense(bool bIgnoreBuilding);
	int getDefenseModifier(bool bIgnoreBuilding);

	int visiblePopulation();

	int getOccupationTimer();
	bool isOccupation();
	void setOccupationTimer(int iNewValue);
	void changeOccupationTimer(int iChange);
	int getCultureUpdateTimer();
	void changeCultureUpdateTimer(int iChange);
	bool isNeverLost();
	void setNeverLost(int iNewValue);
	// < M.A.D. Nukes Start >
	int getMADIncoming();
	// < M.A.D. Nukes End   >
	bool isBombarded();
	void setBombarded(int iNewValue);
	bool isDrafted();
	void setDrafted(int iNewValue);
	bool isAirliftTargeted();
	void setAirliftTargeted(int iNewValue);
	bool isCitizensAutomated();
	void setCitizensAutomated(bool bNewValue);
	bool isProductionAutomated();
	void setProductionAutomated(bool bNewValue);
	bool isWallOverride() const;
	void setWallOverride(bool bOverride);
	void setCitySizeBoost(int iBoost);
	bool isPlundered();
	void setPlundered(bool bNewValue);
	int /*PlayerTypes*/getOwner();
	int /*TeamTypes*/getTeam();
	int /*PlayerTypes*/getPreviousOwner();
	int /*PlayerTypes*/getOriginalOwner();
	int /*CultureLevelTypes*/ getCultureLevel();
	int getCultureThreshold();
	int getSeaPlotYield(int /*YieldTypes*/ eIndex);
	int getRiverPlotYield(int /*YieldTypes*/ eIndex);

	int getBaseYieldRate(int /*YieldTypes*/ eIndex);
	void setBaseYieldRate(int /*YieldTypes*/ eIndex, int iNewValue);
	void changeBaseYieldRate(int /*YieldTypes*/ eIndex, int iNewValue);

	int getBaseYieldRateModifier(int /*YieldTypes*/ eIndex, int iExtra);
	int getYieldRate(int /*YieldTypes*/ eIndex);
	int getYieldRateModifier(int /*YieldTypes*/ eIndex);

// BUG - Building Additional Yield - start
	int getAdditionalYieldByBuilding(int /*YieldTypes*/ eIndex, int /*BuildingTypes*/ iBuilding);
	int getAdditionalBaseYieldRateByBuilding(int /*YieldTypes*/ eIndex, int /*BuildingTypes*/ iBuilding);
	int getAdditionalYieldRateModifierByBuilding(int /*YieldTypes*/ eIndex, int /*BuildingTypes*/ iBuilding);
// BUG - Building Additional Yield - end

	int getTradeYield(int /*YieldTypes*/ eIndex);
	int totalTradeModifier();

// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
	int calculateTradeProfitTimes100(CyCity* pCity);
#endif
// BUG - Fractional Trade Routes - end
	int calculateTradeProfit(CyCity* pCity);
	int calculateTradeYield(int /*YieldTypes*/ eIndex, int iTradeProfit);

	int getExtraSpecialistYield(int /*YieldTypes*/ eIndex);
	int getExtraSpecialistYieldOfType(int /*YieldTypes*/ eIndex, int /*SpecialistTypes*/ eSpecialist);

	int getCommerceRate(int /*CommerceTypes*/ eIndex);
	int getCommerceRateTimes100(int /*CommerceTypes*/ eIndex);
	int getCommerceFromPercent(int /*CommerceTypes*/ eIndex, int iYieldRate);
	int getBaseCommerceRate(int /*CommerceTypes*/ eIndex);
	int getBaseCommerceRateTimes100(int /*CommerceTypes*/ eIndex);
	int getTotalCommerceRateModifier(int /*CommerceTypes*/ eIndex);
	int getProductionToCommerceModifier(int /*CommerceTypes*/ eIndex);
	int getBuildingCommerce(int /*CommerceTypes*/ eIndex);
	int getBuildingCommerceByBuilding(int /*CommerceTypes*/ eIndex, int /*BuildingTypes*/ iBuilding);
// BUG - Building Additional Commerce - start
	int getAdditionalCommerceByBuilding(int /*CommerceTypes*/ eIndex, int /*BuildingTypes*/ iBuilding);
	int getAdditionalCommerceTimes100ByBuilding(int /*CommerceTypes*/ eIndex, int /*BuildingTypes*/ iBuilding);
	int getAdditionalBaseCommerceRateByBuilding(int /*CommerceTypes*/ eIndex, int /*BuildingTypes*/ iBuilding);
	int getAdditionalCommerceRateModifierByBuilding(int /*CommerceTypes*/ eIndex, int /*BuildingTypes*/ iBuilding);
// BUG - Building Additional Commerce - end
	int getSpecialistCommerce(int /*CommerceTypes*/ eIndex);
	void changeSpecialistCommerce(int /*CommerceTypes*/ eIndex, int iChange);
	int getReligionCommerce(int /*CommerceTypes*/ eIndex);
	int getReligionCommerceByReligion(int /*CommerceTypes*/ eIndex, int /*ReligionTypes*/ iReligion);
	int getCorporationCommerce(int /*CommerceTypes*/ eIndex);
	int getCorporationCommerceByCorporation(int /*CommerceTypes*/ eIndex, int /*CorporationTypes*/ iCorporation);
	int getCorporationYield(int /*YieldTypes*/ eIndex);
	int getCorporationYieldByCorporation(int /*YieldTypes*/ eIndex, int /*CorporationTypes*/ iCorporation);
	int getCommerceRateModifier(int /*CommerceTypes*/ eIndex);
	int getCommerceHappinessPer(int /*CommerceTypes*/ eIndex);
	int getCommerceHappinessByType(int /*CommerceTypes*/ eIndex);
	int getCommerceHappiness();
	int getDomainFreeExperience(int /*DomainTypes*/ eIndex);
	int getDomainProductionModifier(int /*DomainTypes*/ eIndex);

	int getCivicHappiness() const;
	int getNumCityPlots() const;
	int getPopulationgrowthratepercentage() const;
	void changePopulationgrowthratepercentage(int iChange);
	bool isValidTerrainForBuildings(int /*BuildingTypes*/ eIndex) const;
	bool hasFreshWater() const;
	bool canUpgradeUnit(int /*UnitTypes*/ eUnit) const;
	int getUnitProductionModifier(int /*UnitTypes*/ eIndex) const; //Exposed to Python
	void changeUnitProductionModifier(int /*UnitTypes*/ eIndex, int iChange);
	int getBuildingClassProductionModifier(int /*BuildingClassTypes*/ eIndex) const;
	void changeBuildingClassProductionModifier(int /*BuildingClassTypes*/ eIndex, int iChange);
	int getBonusDefenseChanges(int /*CommerceTypes*/ eIndex) const;
	void changeBonusDefenseChanges(int /*CommerceTypes*/ eIndex, int iChange);
	int getBonusCommerceRateModifier(int /*CommerceTypes*/ eIndex) const;
	void changeBonusCommerceRateModifier(int /*CommerceTypes*/ eIndex, int iChange);

	int getArea() const;

	//AI
	bool AI_isDefended(int iExtra);

	bool isWeLoveTheKingDay() const;
	void setWeLoveTheKingDay(bool bWeLoveTheKingDay);
	int calculateCorporateTaxes() const;
	void changePowerCount(int iChange, bool bDirty);

	int getEventAnger();
	void changeEventAnger(int iChange);

	int getNumPopulationEmployed();

	int getBonusCommercePercentChanges(int eIndex, int eBuilding);

	bool isAutomatedCanBuild(int /*BuildTypes*/ eIndex);
	void setAutomatedCanBuild(int /*BuildTypes*/ eIndex, bool bNewValue);

	int getCulture(int /*PlayerTypes*/ eIndex);
	int getCultureTimes100(int /*PlayerTypes*/ eIndex);
	int countTotalCultureTimes100();
	PlayerTypes findHighestCulture();
	int calculateCulturePercent(int eIndex);
	int calculateTeamCulturePercent(int /*TeamTypes*/ eIndex);
	void setCulture(int /*PlayerTypes*/ eIndex, int iNewValue, bool bPlots);
	void setCultureTimes100(int /*PlayerTypes*/ eIndex, int iNewValue, bool bPlots);
	void changeCulture(int /*PlayerTypes*/ eIndex, int iChange, bool bPlots);
	void changeCultureTimes100(int /*PlayerTypes*/ eIndex, int iChange, bool bPlots);

	int getNumRevolts(int playerIdx);
	void changeNumRevolts(int playerIdx, int iChange);

	bool isTradeRoute(int /*PlayerTypes*/ eIndex);
	bool isEverOwned(int /*PlayerTypes*/ eIndex);

	bool isRevealed(int /*TeamTypes*/ eIndex, bool bDebug);
	void setRevealed(int /*TeamTypes*/ eIndex, bool bNewValue);
	bool getEspionageVisibility(int /*TeamTypes*/ eIndex);
	std::wstring getName();
	std::wstring getNameForm(int iForm);
	std::wstring getNameKey();
	void setName(std::wstring szNewValue, bool bFound);
	void changeNoBonusCount(int /*BonusTypes*/ eBonus, int iChange);
	bool isNoBonus(int /*BonusTypes*/ eBonus);
	int getFreeBonus(int /*BonusTypes*/ eIndex);
	void changeFreeBonus(int /*BonusTypes*/ eIndex, int iChange);
	int getNumBonuses(int /*BonusTypes*/ iBonus);
	bool hasBonus(int /*BonusTypes */ iBonus);
	bool hasVicinityBonus(int /*BonusTypes */ iBonus);
	int getBuildingProduction(int /*BuildingTypes*/ iIndex);
	void setBuildingProduction(int /*BuildingTypes*/ iIndex, int iNewValue);
	void changeBuildingProduction(int /*BuildingTypes*/ iIndex, int iChange);
	int getBuildingProductionTime(int /*BuildingTypes*/ eIndex);
	void setBuildingProductionTime(int /*BuildingTypes*/ eIndex, int iNewValue);
	void changeBuildingProductionTime(int /*BuildingTypes*/ eIndex, int iChange);
// BUG - Production Decay - start
	bool isBuildingProductionDecay(int /*BuildingTypes*/ eIndex);
	int getBuildingProductionDecay(int /*BuildingTypes*/ eIndex);
	int getBuildingProductionDecayTurns(int /*BuildingTypes*/ eIndex);
// BUG - Production Decay - end
	int getBuildingOriginalOwner(int /*BuildingTypes*/ iIndex);
	int getBuildingOriginalTime(int /*BuildingTypes*/ iIndex);
	int getUnitProduction(int iIndex);
	void setUnitProduction(int iIndex, int iNewValue);
	void changeUnitProduction(int /*UnitTypes*/ iIndex, int iChange);
// BUG - Production Decay - start
	int getUnitProductionTime(int /*UnitTypes*/ eIndex);
	void setUnitProductionTime(int /*UnitTypes*/ eIndex, int iNewValue);
	void changeUnitProductionTime(int /*UnitTypes*/ eIndex, int iChange);
	bool isUnitProductionDecay(int /*UnitTypes*/ eIndex);
	int getUnitProductionDecay(int /*UnitTypes*/ eIndex);
	int getUnitProductionDecayTurns(int /*UnitTypes*/ eIndex);
// BUG - Production Decay - end
// BUG - Project Production - start
	int getProjectProduction(int /*ProjectTypes*/ iIndex);
	void setProjectProduction(int /*ProjectTypes*/ iIndex, int iNewValue);
	void changeProjectProduction(int /*ProjectTypes*/ iIndex, int iChange);
// BUG - Project Production - end
	int getGreatPeopleUnitRate(int /*UnitTypes*/ iIndex);
	int getGreatPeopleUnitProgress(int /*UnitTypes*/ iIndex);
	void setGreatPeopleUnitProgress(int /*UnitTypes*/ iIndex, int iNewValue);
	void changeGreatPeopleUnitProgress(int /*UnitTypes*/ iIndex, int iChange);
	int getSpecialistCount(int /*SpecialistTypes*/ eIndex);
	void alterSpecialistCount(int /*SpecialistTypes*/ eIndex, int iChange);
	int getMaxSpecialistCount(int /*SpecialistTypes*/ eIndex);
	bool isSpecialistValid(int /*SpecialistTypes*/ eIndex, int iExtra);
	int getForceSpecialistCount(int /*SpecialistTypes*/ eIndex);
	bool isSpecialistForced();
	void setForceSpecialistCount(int /*SpecialistTypes*/ eIndex, int iNewValue);
	void changeForceSpecialistCount(int /*SpecialistTypes*/ eIndex, int iChange);
	int getFreeSpecialistCount(int /*SpecialistTypes*/ eIndex);
	//	Koshling - removed direct set of free specialist count - it's HIGHLY unsafe - use changeFreeSpecialistCount
	//void setFreeSpecialistCount(int /*SpecialistTypes*/ eIndex, int iNewValue);
	void changeFreeSpecialistCount(int /*SpecialistTypes*/ eIndex, int iChange);
	int getAddedFreeSpecialistCount(int /*SpecialistTypes*/ eIndex);
	int getImprovementFreeSpecialists(int /*ImprovementTypes*/ iIndex);
	void changeImprovementFreeSpecialists(int /*ImprovementTypes*/ iIndex, int iChange);
	int getReligionInfluence(int /*ReligionTypes*/ iIndex);
	void changeReligionInfluence(int /*ReligionTypes*/ iIndex, int iChange);

	int getCurrentStateReligionHappiness();
	int getStateReligionHappiness(int /*ReligionTypes*/ eIndex);
	void changeStateReligionHappiness(int /*ReligionTypes*/ eIndex, int iChange);

	int getUnitCombatFreeExperience(int /*UnitCombatTypes*/ eIndex);
	int getFreePromotionCount(int /*PromotionTypes*/ eIndex);
	bool isFreePromotion(int /*PromotionTypes*/ eIndex);
	int getSpecialistFreeExperience() const;
	int getEspionageDefenseModifier() const;

	bool isWorkingPlotByIndex(int iIndex);
	bool isWorkingPlot(CyPlot* pPlot);
	void alterWorkingPlot(int iIndex);
	int getNumRealBuilding(int /*BuildingTypes*/ iIndex);
	void setNumRealBuilding(int /*BuildingTypes*/ iIndex, int iNewValue);
	int getNumFreeBuilding(int /*BuildingTypes*/ iIndex);
	bool isHasReligion(int /*ReligionTypes*/ iIndex);
	void setHasReligion(int /*ReligionTypes*/ iIndex, bool bNewValue, bool bAnnounce, bool bArrows);
	bool isHasCorporation(int /*CorporationTypes*/ iIndex);
	void setHasCorporation(int /*CorporationTypes*/ iIndex, bool bNewValue, bool bAnnounce, bool bArrows);
	bool isActiveCorporation(int /*CorporationTypes*/ eCorporation);
	CyCity* getTradeCity(int iIndex);
	int getTradeRoutes();

	void clearOrderQueue();
	void pushOrder(OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bForce);
	void popOrder(int iNum, bool bFinish, bool bChoose);
	int getOrderQueueLength();
	OrderData getOrderFromQueue(int iIndex);

	void setWallOverridePoints(const python::tuple& kPoints); /* points are given in world space ... i.e. PlotXToPointX, etc */
	python::tuple getWallOverridePoints() const;

	int getBuildingYieldChange(int /*BuildingClassTypes*/ eBuildingClass, int /*YieldTypes*/ eYield) const;
	void setBuildingYieldChange(int /*BuildingClassTypes*/ eBuildingClass, int /*YieldTypes*/ eYield, int iChange);
	int getBuildingCommerceChange(int /*BuildingClassTypes*/ eBuildingClass, int /*CommerceTypes*/ eCommerce) const;
	void setBuildingCommerceChange(int /*BuildingClassTypes*/ eBuildingClass, int /*CommerceTypes*/ eCommerce, int iChange);
	int getBuildingHappyChange(int /*BuildingClassTypes*/ eBuildingClass) const;
	void setBuildingHappyChange(int /*BuildingClassTypes*/ eBuildingClass, int iChange);
	int getBuildingHealthChange(int /*BuildingClassTypes*/ eBuildingClass) const;
	void setBuildingHealthChange(int /*BuildingClassTypes*/ eBuildingClass, int iChange);

	int getLiberationPlayer(bool bConquest);
	void liberate(bool bConquest);

	bool AI_avoidGrowth();
	bool AI_isEmphasize(int iEmphasizeType);
	int AI_countBestBuilds(CyArea* pArea);
	int AI_cityValue();
/************************************************************************************************/
/* REVOLUTION_MOD                         02/01/09                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int /*UnitTypes*/ AI_bestUnit();
	int /*UnitTypes*/ AI_bestUnitAI(int iUnitAIType);
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

	CvProperties* getProperties();

	void setBuildingListInvalid();
	bool getBuildingListFilterActive(int /*BuildingFilterTypes*/ eFilter);
	void setBuildingListFilterActive(int /*BuildingFilterTypes*/ eFilter, bool bActive);
	int /*BuildingGroupingTypes*/ getBuildingListGrouping();
	void setBuildingListGrouping(int /*BuildingGroupingTypes*/ eGrouping);
	int /*BuildingSortTypes*/ getBuildingListSorting();
	void setBuildingListSorting(int /*BuildingSortTypes*/ eSorting);
	int getBuildingListGroupNum();
	int getBuildingListNumInGroup(int iGroup);
	int /*BuildingTypes*/ getBuildingListType(int iGroup, int iPos);
	int getBuildingListSelectedBuildingRow();
	int getBuildingListSelectedWonderRow();
	void setBuildingListSelectedBuilding(int /*BuildingTypes*/ eBuilding);
	void setBuildingListSelectedWonder(int /*BuildingTypes*/ eWonder);
	int /*BuildingTypes*/ getBuildingListSelectedBuilding();
	int /*BuildingTypes*/ getBuildingListSelectedWonder();

	void setUnitListInvalid();
	bool getUnitListFilterActive(int /*UnitFilterTypes*/ eFilter);
	void setUnitListFilterActive(int /*UnitFilterTypes*/ eFilter, bool bActive);
	int /*UnitGroupingTypes*/ getUnitListGrouping();
	void setUnitListGrouping(int /*UnitGroupingTypes*/ eGrouping);
	int /*UnitSortTypes*/ getUnitListSorting();
	void setUnitListSorting(int /*UnitSortTypes*/ eSorting);
	int getUnitListGroupNum();
	int getUnitListNumInGroup(int iGroup);
	int /*UnitTypes*/ getUnitListType(int iGroup, int iPos);
	int getUnitListSelectedRow();
	void setUnitListSelected(int /*UnitTypes*/ eUnit);
	int /*UnitTypes*/ getUnitListSelected();

	int localCitizenCaptureResistance() const;
	int getPrioritorizedSpecialist() const;
	void setPrioritorizedSpecialist(int /*SpecialistTypes*/ eSpecialist);

	bool isSpecialistBanned(int /*SpecialistTypes*/ eSpecialist);
	void banSpecialist(int /*SpecialistTypes*/ eSpecialist);
	void removeSpecialistBan(int /*SpecialistTypes*/ eSpecialist);
	bool isEventOccured(int /*EventTypes*/ eEvent) const;

	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);

private:
	CvCity* m_pCity;
};

DECLARE_PY_WRAPPER(CyCity, CvCity*);

#endif // CyCity_h__