#pragma once

#ifndef CyCity_h__
#define CyCity_h__

//
// Python wrapper class for CvCity
//

class CityOutputHistory;
class CvCity;
//class CvProperties;
class CyPlot;
class CyArea;
class CyUnit;
struct OrderData;

class CyCity
{
public:
	DllExport explicit CyCity(CvCity* pCity);		// Call from C++

	CvCity* getCity() const { return m_pCity; }	// Call from C++

	void kill();

	int getRevolutionIndex() const;
	void setRevolutionIndex(int iNewValue);
	void changeRevolutionIndex(int iChange);

	int getLocalRevIndex() const;
	void setLocalRevIndex(int iNewValue);

	int getRevIndexAverage() const;
	void setRevIndexAverage(int iNewValue);
	void updateRevIndexAverage();

	int getRevIndexDistanceMod() const;

	int getRevolutionCounter() const;
	void setRevolutionCounter(int iNewValue);
	void changeRevolutionCounter(int iChange);

	int getReinforcementCounter() const;
	void setReinforcementCounter(int iNewValue);
	void changeReinforcementCounter(int iChange);

	CyPlot* getCityIndexPlot(int iIndex) const;
	bool canWork(const CyPlot* pPlot) const;
	int countNumImprovedPlots() const;
	int countNumWaterPlots() const;

	int findBaseYieldRateRank(YieldTypes eYield) const;
	int findYieldRateRank(YieldTypes eYield) const;
	int findCommerceRateRank(CommerceTypes eCommerce) const;

	int getMaxNumWorldWonders() const;
	int getMaxNumNationalWonders() const;

	bool canTrain(UnitTypes eUnit, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreUpgrades) const;
	bool canConstruct(BuildingTypes eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost) const;
	bool canCreate(ProjectTypes eProject, bool bContinue, bool bTestVisible) const;
	bool canMaintain(ProcessTypes eProcess) const;
	int getFoodTurnsLeft() const;
	bool isProduction() const;
	bool isProductionUnit() const;
	bool isProductionBuilding() const;
	bool isProductionProject() const;
	bool isProductionProcess() const;

	int getProductionExperience(UnitTypes eUnit) const;
	void addProductionExperience(const CyUnit& kUnit, bool bConscript);

	UnitTypes getProductionUnit() const;
	BuildingTypes getProductionBuilding() const;
	ProjectTypes getProductionProject() const;
	ProcessTypes getProductionProcess() const;
	std::wstring getProductionName() const;
	std::wstring getProductionNameKey() const;
	int getGeneralProductionTurnsLeft() const;
	bool isFoodProduction() const;
	int getFirstUnitOrder(int /*UnitTypes*/ eUnit) const;
	int getNumTrainUnitAI(int /*UnitAITypes*/ eUnitAI) const;
	int getFirstBuildingOrder(int /*BuildingTypes*/ eBuilding) const;
	int getProductionProgress() const;
	int getProductionNeeded() const;
	int getProductionTurnsLeft() const;
	int getUnitProductionTurnsLeft(int /*UnitTypes*/ iUnit, int iNum) const;
	int getBuildingProductionTurnsLeft(int /*BuildingTypes*/ iBuilding, int iNum) const;
	int getProjectProductionTurnsLeft(int /*ProjectTypes*/ eProject, int iNum) const;
	void setProductionProgress(int iNewValue);
	void changeProduction(int iChange);
	int getCurrentProductionDifference(bool bIgnoreFood, bool bOverflow) const;

	bool canHurry(int /*HurryTypes*/ iHurry, bool bTestVisible) const;
	int /*UnitTypes*/ getConscriptUnit() const;
	int flatConscriptAngerLength() const;
	bool canConscript() const;
	int getBonusHealth(int /*BonusTypes*/ iBonus) const;
	int getBonusHappiness(int /*BonusTypes*/ iBonus) const;
	int getBonusPower(int /*BonusTypes*/ eBonus) const;
	int getBonusYieldRateModifier(int /*YieldTypes*/ eIndex, int /*BonusTypes*/ eBonus) const;
	int /* HandicapTypes */ getHandicapType() const;
	int /* CivilizationTypes */ getCivilizationType() const;
	int /*LeaderHeadTypes*/ getPersonalityType() const;
	int /*ArtStyleTypes*/ getArtStyleType() const;

	bool hasTrait(int /*TraitTypes*/ iTrait) const;
	bool isNPC() const;
	bool isHominid() const;
	bool isHuman() const;
	bool isVisible(int /*TeamTypes*/ eTeam, bool bDebug) const;

	bool isCapital() const;
	bool isCoastal(int iMinWaterSize) const;
	bool isDisorder() const;
	bool isHolyCityByType(int /*ReligionTypes*/ iIndex) const;
	bool isHolyCity() const;
	bool isHeadquartersByType(int /*CorporationTypes*/ iIndex) const;
	int getNoMilitaryPercentAnger() const;
	int getWarWearinessPercentAnger() const;

	int getRevIndexPercentAnger() const;

	int unhappyLevel(int iExtra) const;
	int happyLevel() const;
	int angryPopulation(int iExtra) const;
	int totalFreeSpecialists() const;
	int goodHealth() const;
	int badHealth(bool bNoAngry) const;
	int healthRate(bool bNoAngry, int iExtra) const;
	int foodConsumption(bool bNoAngry, int iExtra) const;
	int foodDifference(bool bBottom) const;
	int growthThreshold() const;
	int productionLeft() const;
	int64_t getHurryGold(int /*HurryTypes*/ iHurry) const;
	int hurryPopulation(int /*HurryTypes*/ iHurry) const;
	int hurryProduction(int /*HurryTypes*/ iHurry) const;
	int flatHurryAngerLength() const;

	void changeHasBuilding(int /*BuildingTypes*/ iIndex, bool bNewValue);
	int hasBuilding(int /*BuildingTypes*/ iIndex) const;
	bool isActiveBuilding(int /*BuildingTypes*/ iIndex) const;
	bool isFreeBuilding(int /*BuildingTypes*/ iIndex) const;
	int getID() const;
	int getX() const;
	int getY() const;
	CyPlot* plot() const;
	bool isConnectedTo(const CyCity& kCity) const;
	bool isConnectedToCapital(int /*PlayerTypes*/ ePlayer) const;
	CyArea* area() const;
	CyArea* waterArea() const;

	int getGameTurnFounded() const;
	int getGameDateFounded() const;
	int getGameTurnAcquired() const;
	int getPopulation() const;
	void setPopulation(int iNewValue);
	void changePopulation(int iChange);
	int64_t getRealPopulation() const;

	int getHighestPopulation() const;
	void setHighestPopulation(int iNewValue);
	int getWorkingPopulation() const;
	int getSpecialistPopulation() const;
	int getNumGreatPeople() const;
	int getBaseGreatPeopleRate() const;
	int getGreatPeopleRate() const;
	void changeBaseGreatPeopleRate(int iChange);
	int getGreatPeopleRateModifier() const;
	int getGreatPeopleProgress() const;
	void changeGreatPeopleProgress(int iChange);
	int getNumWorldWonders() const;
	int getNumNationalWonders() const;
	int getNumBuildings() const;
	bool isGovernmentCenter() const;

	int getMaintenance() const;
	int getMaintenanceTimes100() const;
	int calculateDistanceMaintenance() const;
	int calculateNumCitiesMaintenance() const;
	int calculateColonyMaintenanceTimes100() const;
	int getMaintenanceModifier() const;
	int getWarWearinessModifier() const;

	int getEspionageHealthCounter() const;
	void changeEspionageHealthCounter(int iChange);
	int getEspionageHappinessCounter() const;
	void changeEspionageHappinessCounter(int iChange);

	int getBuildingHealth(int iBuilding) const;

	int getMilitaryHappinessUnits() const;
	int getBuildingHappiness(int iBuilding) const;
	int getReligionHappiness(int iReligion) const;
	int getExtraHappiness() const;
	int getExtraHealth() const;
	void changeExtraHappiness(int iChange);
	void changeExtraHealth(int iChange);
	int getHurryAngerTimer() const;
	void changeHurryAngerTimer(int iChange);

	int getRevRequestAngerTimer() const;
	void changeRevRequestAngerTimer(int iChange);
	void changeRevSuccessTimer(int iChange);

	int getConscriptAngerTimer() const;
	void changeConscriptAngerTimer(int iChange);
	int getDefyResolutionAngerTimer() const;
	void changeDefyResolutionAngerTimer(int iChange);
	int flatDefyResolutionAngerLength() const;
	int getHappinessTimer() const;
	void changeHappinessTimer(int iChange);
	bool isNoUnhappiness() const;

	bool isBuildingOnlyHealthy() const;

	int getFood() const;
	void setFood(int iNewValue);
	void changeFood(int iChange);
	int getFoodKept() const;
	int getMaxProductionOverflow() const;
	int getOverflowProduction() const;
	void setOverflowProduction(int iNewValue);
	int getFeatureProduction() const;
	void setFeatureProduction(int iNewValue);
	int getMilitaryProductionModifier() const;
	int getSpaceProductionModifier() const;
	int getExtraTradeRoutes() const;
	int getMaxTradeRoutes() const;
	void changeExtraTradeRoutes(int iChange);
	int getTradeRouteModifier() const;
	int getForeignTradeRouteModifier() const;
	int getBuildingDefense() const;
	int getFreeExperience() const;
	int getMaxAirlift() const;
	int getAirModifier() const;
	int getNukeModifier() const;
	bool isPower() const;
	int getDefenseDamage() const;
	void changeDefenseDamage(int iChange);
	int getNaturalDefense() const;
	int getTotalDefense(bool bIgnoreBuilding) const;
	int getDefenseModifier(bool bIgnoreBuilding) const;

	int getOccupationTimer() const;
	bool isOccupation() const;
	void setOccupationTimer(int iNewValue);
	void changeOccupationTimer(int iChange);
	bool isNeverLost() const;
	void setNeverLost(bool bNewValue);

	bool isBombarded() const;
	void setBombarded(bool bNewValue);
	bool isDrafted() const;
	void setDrafted(bool bNewValue);
	bool isAirliftTargeted() const;
	void setAirliftTargeted(bool bNewValue);
	bool isCitizensAutomated() const;
	void setCitizensAutomated(bool bNewValue);
	bool isProductionAutomated() const;
	void setProductionAutomated(bool bNewValue);
	bool isWallOverride() const;
	void setWallOverride(bool bOverride);
	bool isPlundered() const;
	void setPlundered(bool bNewValue);
	int /*PlayerTypes*/getOwner() const;
	int /*TeamTypes*/getTeam() const;
	int /*PlayerTypes*/getPreviousOwner() const;
	int /*PlayerTypes*/getOriginalOwner() const;
	void setOriginalOwner(int /*PlayerTypes*/ iPlayer);
	int /*CultureLevelTypes*/ getCultureLevel() const;
	int getCultureThreshold() const;

	int getPlotYield(int /*YieldTypes*/ eIndex) const;

	int getBaseYieldRateModifier(int /*YieldTypes*/ eIndex, int iExtra) const;
	int getYieldRate(int /*YieldTypes*/ eIndex) const;
	int getYieldRateModifier(int /*YieldTypes*/ eIndex) const;

	int getTradeYield(int /*YieldTypes*/ eIndex) const;

// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
	int calculateTradeProfitTimes100(const CyCity& kCity) const;
#endif
// BUG - Fractional Trade Routes - end
	int calculateTradeProfit(const CyCity& kCity) const;
	int calculateTradeYield(int /*YieldTypes*/ eIndex, int iTradeProfit) const;

	int getCommerceRate(int /*CommerceTypes*/ eIndex) const;
	int getCommerceRateTimes100(int /*CommerceTypes*/ eIndex) const;
	int getBaseCommerceRate(int /*CommerceTypes*/ eIndex) const;
	int getProductionToCommerceModifier(int /*CommerceTypes*/ eIndex) const;
	int getBuildingCommerceByBuilding(int /*CommerceTypes*/ eIndex, int /*BuildingTypes*/ iBuilding) const;
	int getSpecialistCommerce(int /*CommerceTypes*/ eIndex) const;
	void changeSpecialistCommerce(int /*CommerceTypes*/ eIndex, int iChange);
	int getReligionCommerceByReligion(int /*CommerceTypes*/ eIndex, int /*ReligionTypes*/ iReligion) const;
	int getCorporationCommerce(int /*CommerceTypes*/ eIndex) const;
	int getCorporationCommerceByCorporation(int /*CommerceTypes*/ eIndex, int /*CorporationTypes*/ iCorporation) const;
	int getCorporationYield(int /*YieldTypes*/ eIndex) const;
	int getCorporationYieldByCorporation(int /*YieldTypes*/ eIndex, int /*CorporationTypes*/ iCorporation) const;
	int getCommerceRateModifier(int /*CommerceTypes*/ eIndex) const;
	int getCommerceHappinessByType(int /*CommerceTypes*/ eIndex) const;
	int getDomainFreeExperience(int /*DomainTypes*/ eIndex) const;
	int getDomainProductionModifier(int /*DomainTypes*/ eIndex) const;

	int getBonusCommerceRateModifier(int /*CommerceTypes*/ eIndex) const;

	int getArea() const;

	bool isWeLoveTheKingDay() const;
	void setWeLoveTheKingDay(bool bWeLoveTheKingDay);
	int64_t calcCorporateMaintenance() const;
	void changePowerCount(int iChange);

	void changeEventAnger(int iChange);

	int getNumPopulationEmployed() const;

	int getBonusCommercePercentChanges(int eIndex, int eBuilding) const;
	int getBaseYieldRateFromBuilding100(int iYield, int iBuilding) const;

	bool isAutomatedCanBuild(int /*BuildTypes*/ eIndex) const;
	void setAutomatedCanBuild(int /*BuildTypes*/ eIndex, bool bNewValue);

	int getCulture(int /*PlayerTypes*/ eIndex) const;
	int getCultureTimes100(int /*PlayerTypes*/ eIndex) const;
	int countTotalCultureTimes100() const;
	PlayerTypes findHighestCulture() const;
	int calculateCulturePercent(int eIndex) const;
	void setCulture(int /*PlayerTypes*/ eIndex, int iNewValue, bool bPlots);
	void setCultureTimes100(int /*PlayerTypes*/ eIndex, int iNewValue, bool bPlots);
	void changeCulture(int /*PlayerTypes*/ eIndex, int iChange, bool bPlots);

	int getNumRevolts(int playerIdx) const;
	void changeNumRevolts(int playerIdx, int iChange);

	bool isRevealed(int /*TeamTypes*/ eIndex, bool bDebug) const;
	void setRevealed(int /*TeamTypes*/ eIndex, bool bNewValue);
	bool getEspionageVisibility(int /*TeamTypes*/ eIndex) const;
	std::wstring getName() const;
	std::wstring getNameForm(int iForm) const;
	std::wstring getNameKey() const;
	void setName(std::wstring szNewValue, bool bFound);
	int getFreeBonus(int /*BonusTypes*/ eIndex) const;
	void changeFreeBonus(int /*BonusTypes*/ eIndex, int iChange);
	int getNumBonuses(int /*BonusTypes*/ iBonus) const;
	bool hasBonus(int /*BonusTypes */ iBonus) const;

	int getProgressOnBuilding(int /*BuildingTypes*/ iIndex) const;
	void setProgressOnBuilding(int /*BuildingTypes*/ iIndex, int iNewValue);
	int getDelayOnBuilding(int /*BuildingTypes*/ eIndex) const;
	bool isBuildingProductionDecay(int /*BuildingTypes*/ eIndex) const;
	int getBuildingProductionDecayTurns(int /*BuildingTypes*/ eIndex) const;

	int getBuildingOriginalOwner(int /*BuildingTypes*/ iIndex) const;
	int getBuildingOriginalTime(int /*BuildingTypes*/ iIndex) const;

	int getProgressOnUnit(int iIndex) const;
	void setProgressOnUnit(int iIndex, int iNewValue);
	int getDelayOnUnit(int /*UnitTypes*/ eIndex) const;
	bool isUnitProductionDecay(int /*UnitTypes*/ eIndex) const;
	int getUnitProductionDecayTurns(int /*UnitTypes*/ eIndex) const;

	int getProjectProduction(int /*ProjectTypes*/ iIndex) const;

	int getGreatPeopleUnitProgress(int /*UnitTypes*/ iIndex) const;
	void setGreatPeopleUnitProgress(int /*UnitTypes*/ iIndex, int iNewValue);
	void changeGreatPeopleUnitProgress(int /*UnitTypes*/ iIndex, int iChange);
	int getSpecialistCount(int /*SpecialistTypes*/ eIndex) const;
	int getMaxSpecialistCount(int /*SpecialistTypes*/ eIndex) const;
	bool isSpecialistValid(int /*SpecialistTypes*/ eIndex, int iExtra) const;
	int getForceSpecialistCount(int /*SpecialistTypes*/ eIndex) const;
	void setForceSpecialistCount(int /*SpecialistTypes*/ eIndex, int iNewValue);
	int getFreeSpecialistCount(int /*SpecialistTypes*/ eIndex) const;
	//	Koshling - removed direct set of free specialist count - it's HIGHLY unsafe - use changeFreeSpecialistCount
	//void setFreeSpecialistCount(int /*SpecialistTypes*/ eIndex, int iNewValue);
	void changeFreeSpecialistCount(int /*SpecialistTypes*/ eIndex, int iChange);
	int getAddedFreeSpecialistCount(int /*SpecialistTypes*/ eIndex) const;
	int getImprovementFreeSpecialists(int /*ImprovementTypes*/ iIndex) const;
	void changeImprovementFreeSpecialists(int /*ImprovementTypes*/ iIndex, int iChange);
	int getReligionInfluence(int /*ReligionTypes*/ iIndex) const;
	void changeReligionInfluence(int /*ReligionTypes*/ iIndex, int iChange);

	int getStateReligionHappiness(int /*ReligionTypes*/ eIndex) const;
	void changeStateReligionHappiness(int /*ReligionTypes*/ eIndex, int iChange);

	int getSpecialistFreeExperience() const;
	int getEspionageDefenseModifier() const;

	bool isWorkingPlot(const CyPlot& kPlot) const;
	bool isHasReligion(int /*ReligionTypes*/ iIndex) const;
	void setHasReligion(int /*ReligionTypes*/ iIndex, bool bNewValue, bool bAnnounce, bool bArrows);
	bool isHasCorporation(int /*CorporationTypes*/ iIndex) const;
	void setHasCorporation(int /*CorporationTypes*/ iIndex, bool bNewValue, bool bAnnounce, bool bArrows);
	bool isActiveCorporation(int /*CorporationTypes*/ eCorporation) const;
	CyCity* getTradeCity(int iIndex) const;
	int getTradeRoutes() const;

	void clearOrderQueue();
	void pushOrder(OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bForce);
	void popOrder(int iNum, bool bFinish, bool bChoose);
	int getOrderQueueLength() const;
	OrderData getOrderFromQueue(int iIndex) const;

	int getBuildingYieldChange(int /*BuildingTypes*/ eBuilding, int /*YieldTypes*/ eYield) const;
	void setBuildingYieldChange(int /*BuildingTypes*/ eBuilding, int /*YieldTypes*/ eYield, int iChange);
	int getBuildingCommerceChange(int /*BuildingTypes*/ eBuilding, int /*CommerceTypes*/ eCommerce) const;
	void setBuildingCommerceChange(int /*BuildingTypes*/ eBuilding, int /*CommerceTypes*/ eCommerce, int iChange);
	int getBuildingHappyChange(int /*BuildingTypes*/ eBuilding) const;
	void setBuildingHappyChange(int /*BuildingTypes*/ eBuilding, int iChange);
	int getBuildingHealthChange(int /*BuildingTypes*/ eBuilding) const;
	void setBuildingHealthChange(int /*BuildingTypes*/ eBuilding, int iChange);

	int getLiberationPlayer(bool bConquest) const;

	bool AI_isEmphasizeSpecialist(int /*SpecialistTypes*/ iIndex) const;
	bool AI_isEmphasize(int iEmphasizeType) const;
	int AI_countBestBuilds(const CyArea& kArea) const;
	int AI_cityValue() const;

	CvProperties* getProperties() const;
	const CityOutputHistory* getCityOutputHistory() const;

	bool getBuildingListFilterActive(int /*BuildingFilterTypes*/ eFilter);
	void setBuildingListFilterActive(int /*BuildingFilterTypes*/ eFilter, bool bActive);
	int /*BuildingGroupingTypes*/ getBuildingListGrouping();
	void setBuildingListGrouping(int /*BuildingGroupingTypes*/ eGrouping);
	int /*BuildingSortTypes*/ getBuildingListSorting();
	void setBuildingListSorting(int /*BuildingSortTypes*/ eSorting);
	int getBuildingListGroupNum();
	int getBuildingListNumInGroup(int iGroup);
	int /*BuildingTypes*/ getBuildingListType(int iGroup, int iPos);

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

	bool isEventOccured(int /*EventTypes*/ eEvent) const;

	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);

	int AI_bestUnit() const;
	int AI_bestUnitAI(UnitAITypes eUnitAITypes) const;

private:
	CvCity* m_pCity;
};

DECLARE_PY_WRAPPER(CyCity, CvCity*);

#endif // CyCity_h__