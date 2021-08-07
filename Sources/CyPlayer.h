#pragma once

#ifndef CyPlayer_h
#define CyPlayer_h

//
// Python wrapper class for CvPlayer
//

class CyUnit;
class CvPlayer;
class CvProperties;
class CyCity;
class CyArea;
class CyPlot;
class CySelectionGroup;

class CyPlayer
{
public:
	CyPlayer();
	explicit CyPlayer(CvPlayer* pPlayer); // Call from C++

	//const CvPlayer* getPlayer() const { return m_pPlayer; } // Call from C++

	void changeLeader(int /*LeaderHeadTypes*/ eNewLeader);
	void changeCiv(int /*CivilizationTypes*/ eNewCiv);
	void setIsHuman(bool bNewValue);
	void setIsRebel(bool bNewValue);
	bool isRebel() const;

	int getStabilityIndex() const;
	void changeStabilityIndex(int iChange);
	int getStabilityIndexAverage() const;
	void updateStabilityIndexAverage();

	bool isDoNotBotherStatus(int /*PlayerTypes*/ playerID) const;

	int startingPlotRange() const;
	bool startingPlotWithinRange(const CyPlot* pPlot, int /*PlayerTypes*/ ePlayer, int iRange, int iPass);

	CyPlot* findStartingPlot(bool bRandomize) const;

	CyCity* initCity(int x, int y);
	void acquireCity(CyCity* pCity, bool bConquest, bool bTrade);
	void killCities();

	std::wstring getNewCityName() const;

	CyUnit* initUnit(int /*UnitTypes*/ iIndex, int iX, int iY, UnitAITypes eUnitAI, DirectionTypes eFacingDirection);

	void killUnits();
	bool hasTrait(int /*TraitTypes*/ iIndex) const;
	bool isHuman() const;
	bool isHumanDisabled() const;
	bool isBarbarian() const;
	bool isNPC() const;
	bool isHominid() const;
	std::wstring getName() const;

	void setName(std::wstring szNewValue);

	std::wstring getNameForm(int iForm) const;
	std::wstring getNameKey() const;
	std::wstring getCivilizationDescription(int iForm) const;

	void setCivName(const std::wstring szNewDesc, const std::wstring szNewShort, const std::wstring szNewAdj);

	std::wstring getCivilizationDescriptionKey() const;
	std::wstring getCivilizationShortDescription(int iForm) const;
	std::wstring getCivilizationShortDescriptionKey() const;
	std::wstring getCivilizationAdjective(int iForm) const;
	std::wstring getCivilizationAdjectiveKey() const;
	std::wstring getFlagDecal() const;
	bool isWhiteFlag() const;
	std::wstring getStateReligionName(int iForm) const;
	std::wstring getStateReligionKey() const;
	std::wstring getBestAttackUnitName(int iForm) const;
	std::wstring getWorstEnemyName() const;
	int /*ArtStyleTypes*/ getArtStyleType() const;
	std::string getUnitButton(int eUnit) const;

	int countReligionSpreadUnits(const CyArea* pArea, int /*ReligionTypes*/ eReligion) const;

	int countNumCoastalCities() const;

	int countOwnedBonuses(int /*BonusTypes*/ eBonus) const;
	int countNumBuildings(int /*BuildingTypes*/ eBuilding) const;
	bool hasBuilding(int /*BuildingTypes*/ eBuilding) const;
	int countNumCitiesConnectedToCapital() const;

	bool canContact(int /*PlayerTypes*/ ePlayer) const;
	void contact(int /*PlayerTypes*/ ePlayer);
	bool canTradeWith(int /*PlayerTypes*/ eWhoTo) const;
	bool canTradeItem(int /*PlayerTypes*/ eWhoTo, TradeData item, bool bTestDenial) const;
	DenialTypes getTradeDenial(int /*PlayerTypes*/ eWhoTo, TradeData item) const;
	bool canTradeNetworkWith(int /*PlayerTypes*/ iPlayer) const;
	int getNumAvailableBonuses(int /*BonusTypes*/ eBonus) const;
	int getNumTradeableBonuses(int /*BonusTypes*/ eBonus) const;
	bool hasBonus(int /*BonusTypes*/ eBonus) const;

	bool isTurnActive() const;

	void receiveGoody(const CyPlot* pPlot, int /*GoodyTypes*/ eGoody, const CyUnit* pUnit);
	void doGoody(const CyPlot* pPlot, const CyUnit* pUnit);
	bool canFound(int iX, int iY) const;
	void found(int iX, int iY);
	bool canTrain(int /*UnitTypes*/ eUnit, bool bContinue, bool bTestVisible) const;
	bool canConstruct(int /*BuildingTypes*/eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost) const;
	bool canCreate(int /*ProjectTypes*/ eProject, bool bContinue, bool bTestVisible) const;
	int getUnitProductionNeeded(int /*UnitTypes*/ iIndex) const;
	int getBuildingProductionNeeded(int /*BuildingTypes*/ iIndex) const;
	int getProjectProductionNeeded(int /*ProjectTypes*/ iIndex) const;

	bool canBuild(const CyPlot* pPlot, int /*BuildTypes*/ eBuild, bool bTestEra, bool bTestVisible) const;
	int /*RouteTypes*/ getBestRoute(const CyPlot* pPlot) const;

	int calculateTotalYield(int /*YieldTypes*/ eYield) const;
	int calculateTotalExports(int /*YieldTypes*/ eYield) const;
	int calculateTotalImports(int /*YieldTypes*/ eYield) const;

	int calculateTotalCityHappiness() const;
	int calculateTotalCityUnhappiness() const;

	int calculateTotalCityHealthiness() const;
	int calculateTotalCityUnhealthiness() const;

	int64_t getFinalUnitUpkeep() const;
	int calculateUnitSupply() const;
	int64_t calculatePreInflatedCosts() const;
	int64_t getFinalExpense() const;
	int64_t calculateInflatedCosts() const;

	int calculateGoldRate() const;
	int calculateTotalCommerce() const;
	int calculateResearchRate(int /*TechTypes*/ eTech) const;
	int calculateResearchModifier(int /*TechTypes*/ eTech) const;
	int calculateBaseNetResearch() const;
	bool canEverResearch(int /*TechTypes*/ eTech) const;
	bool canResearch(int /*TechTypes*/ eTech) const;
	int /* TechTypes */ getCurrentResearch() const;
	bool isCurrentResearchRepeat() const;
	int getResearchTurnsLeft(int /*TechTypes*/ eTech, bool bOverflow) const;

	bool isCivic(int /*CivicTypes*/ eCivic) const;
	bool canDoCivics(int /*CivicTypes*/ eCivic) const;
	bool canRevolution() const;

	bool canChangeReligion() const;
	bool canConvert(int /*ReligionTypes*/ iIndex) const;
	void convert(int /*ReligionTypes*/ iIndex);
	bool hasHolyCity(int /*ReligionTypes*/ eReligion) const;
	int countHolyCities() const;

	int getCivicAnarchyLength(boost::python::list& /*CivicTypes**/ paeNewCivics) const;
	int getReligionAnarchyLength() const;

	bool hasHeadquarters(int /*CorporationTypes*/ eCorporation) const;

	int unitsRequiredForGoldenAge() const;
	int greatPeopleThresholdMilitary() const;
	int greatPeopleThresholdNonMilitary() const;
	int getGreatGeneralsThresholdModifier() const;
	int specialistYield(int /*SpecialistTypes*/ eSpecialist, int /*YieldTypes*/ eCommerce) const;

	CyPlot* getStartingPlot() const;
	void setStartingPlot(const CyPlot* pPlot, bool bUpdateStartDist);
	int getTotalPopulation() const;
	int64_t getRealPopulation() const;

	int getTotalLand() const;

	int64_t getGold() const;
	void setGold(int64_t iNewValue);
	void changeGold(int64_t iChange);
	int getGoldPerTurn() const;

	int getAdvancedStartPoints() const;
	int getAdvancedStartUnitCost(int /*UnitTypes*/ eUnit, bool bAdd, const CyPlot* pPlot) const;
	int getAdvancedStartCityCost(bool bAdd, const CyPlot* pPlot) const;
	int getAdvancedStartPopCost(bool bAdd, const CyCity* pCity) const;
	int getAdvancedStartCultureCost(bool bAdd, const CyCity* pCity) const;
	int getAdvancedStartBuildingCost(int /*BuildingTypes*/ eBuilding, bool bAdd, const CyCity* pCity) const;
	int getAdvancedStartImprovementCost(int /*ImprovementTypes*/ eImprovement, bool bAdd, const CyPlot* pPlot) const;
	int getAdvancedStartRouteCost(int /*RouteTypes*/ eRoute, bool bAdd, const CyPlot* pPlot) const;
	int getAdvancedStartTechCost(int /*TechTypes*/ eTech, bool bAdd) const;
	int getAdvancedStartVisibilityCost(const CyPlot* pPlot) const;

	int getEspionageSpending(int /*PlayerTypes*/ ePlayer) const;
	bool canDoEspionageMission(int /*EspionageMissionTypes*/ eMission, int /*PlayerTypes*/ eTargetPlayer, const CyPlot* pPlot, int iExtraData) const;
	int getEspionageMissionCost(int /*EspionageMissionTypes*/ eMission, int /*PlayerTypes*/ eTargetPlayer, const CyPlot* pPlot, int iExtraData) const;

	int getEspionageSpendingWeightAgainstTeam(int /*TeamTypes*/ eIndex) const;
	void setEspionageSpendingWeightAgainstTeam(int /*TeamTypes*/ eIndex, int iValue);
	void changeEspionageSpendingWeightAgainstTeam(int /*TeamTypes*/ eIndex, int iChange);

	int getGoldenAgeTurns() const;
	int getGoldenAgeLength() const;
	bool isGoldenAge() const;
	void changeGoldenAgeTurns(int iChange);
	void changeNumUnitGoldenAges(int iChange);
	int getAnarchyTurns() const;
	bool isAnarchy() const;
	void changeAnarchyTurns(int iChange);
	int getAnarchyModifier() const;
	int getGoldenAgeModifier() const;
	void changeGreatGeneralsThresholdModifier(int iChange);
	int getGreatPeopleRateModifier() const;
	int getGreatGeneralRateModifier() const;

	int getFreeExperience() const;
	int getFeatureProductionModifier() const;
	int getWorkerSpeedModifier() const;
	int getMilitaryProductionModifier() const;
	int getSpaceProductionModifier() const;

	int getBestUnitType(int /*UnitAITypes*/ eUnitAI) const;

	bool isNonStateReligionCommerce() const;

	int getRevIdxNational() const;
	bool canFoundReligion() const;

	int getNumMilitaryUnits() const;

	int getOverflowResearch() const;
	bool isBuildingOnlyHealthy() const;

	int64_t getTreasuryUpkeep() const;
	int getTotalMaintenance() const;
	int getLevelExperienceModifier() const;

	int getExtraHealth() const;
	void changeExtraHealth(int iChange);

	int getExtraHappiness() const;
	void changeExtraHappiness(int iChange);

	int getBuildingHappiness() const;
	int getWarWearinessPercentAnger() const;
	int getWarWearinessModifier() const;
	int getCoastalTradeRoutes() const;
	void changeCoastalTradeRoutes(int iChange);
	int getTradeRoutes() const;
	int getRevolutionTimer() const;
	void changeRevolutionTimer(int addTime);

	bool isStateReligion() const;
	int getStateReligionHappiness() const;
	int getStateReligionUnitProductionModifier() const;
	void changeStateReligionUnitProductionModifier(int iChange);
	int getStateReligionBuildingProductionModifier() const;
	void changeStateReligionBuildingProductionModifier(int iChange);
	int getStateReligionFreeExperience() const;
	CyCity* getCapitalCity() const;
	int getCitiesLost() const;

	int getAssets() const;
	int getPower() const;
	int getUnitPower() const;
	int getPopScore() const;
	int getLandScore() const;

	int getSevoWondersScore(int mode) const;
	int getWondersScore() const;
	int getTechScore() const;
	bool isMinorCiv() const;
	bool isAlive() const;
	bool isEverAlive() const;
	bool isFoundedFirstCity() const;

	void setFoundedFirstCity(bool bNewValue);
	void setAlive(bool bNewValue);
	void setNewPlayerAlive(bool bNewValue);
	void changeTechScore(int iChange);

	bool isStrike() const;

	int getID() const;
	int /* HandicapTypes */ getHandicapType() const;
	int /* CivilizationTypes */ getCivilizationType() const;
	int /*LeaderHeadTypes*/ getLeaderType() const;
	int /*LeaderHeadTypes*/ getPersonalityType() const;
	void setPersonalityType(int /*LeaderHeadTypes*/ eNewValue);
	int /*ErasTypes*/ getCurrentEra() const;
	void setCurrentEra(int /*EraTypes*/ iNewValue);

	int /*ReligonTypes*/ getStateReligion() const;
	void setLastStateReligion(int /*ReligionTypes*/ iNewReligion);

	int getTeam() const;

	int /*PlayerColorTypes*/ getPlayerColor() const;
	int getPlayerTextColorR() const;
	int getPlayerTextColorG() const;
	int getPlayerTextColorB() const;
	int getPlayerTextColorA() const;

	int getSeaPlotYield(YieldTypes eIndex) const;
	int getYieldRateModifier(YieldTypes eIndex) const;
	int getCommercePercent(int /*CommerceTypes*/ eIndex) const;
	void changeCommercePercent(CommerceTypes eIndex, int iChange);
	int getCommerceRate(CommerceTypes eIndex) const;
	int getCommerceRateModifier(CommerceTypes eIndex) const;
	int getCapitalCommerceRateModifier(CommerceTypes eIndex) const;
	int getSpecialistExtraCommerce(CommerceTypes eIndex) const;

	bool isCommerceFlexible(int /*CommerceTypes*/ eIndex) const;

	bool isFeatAccomplished(int /*FeatTypes*/ eIndex) const;
	void setFeatAccomplished(int /*FeatTypes*/ eIndex, bool bNewValue);
	bool isOption(int /*PlayerOptionTypes*/ eIndex) const;
	void setOption(int /*PlayerOptionTypes*/ eIndex, bool bNewValue);
	int getVotes(int /*VoteTypes*/ eVote, int /*VoteSourceTypes*/ eVoteSource) const;
	bool isFullMember(int /*VoteSourceTypes*/ eVoteSource) const;
	bool isVotingMember(int /*VoteSourceTypes*/ eVoteSource) const;
	bool isPlayable() const;
	int getBonusExport(int /*BonusTypes*/ iIndex) const;
	int getBonusImport(int /*BonusTypes*/ iIndex) const;

	int getImprovementCount(int /*ImprovementTypes*/ iIndex) const;

	int getExtraBuildingHappiness(int /*BuildingTypes*/ iIndex) const;
	int getExtraBuildingHealth(int /*BuildingTypes*/ iIndex) const;

	int getUnitCount(int /*UnitTypes*/ eIndex) const;
	bool isUnitMaxedOut(int /*UnitTypes*/ eIndex, int iExtra) const;
	int getUnitMaking(int /*UnitTypes*/ eIndex) const;
	int getUnitCountPlusMaking(int /*UnitTypes*/ eIndex) const;

	int getBuildingCount(int /*BuildingTypes*/ iIndex) const;
	bool isBuildingMaxedOut(int /*BuildingTypes*/ iIndex, int iExtra) const;
	int getBuildingCountPlusMaking(int /*BuildingTypes*/ iIndex) const;
	bool canHurry(int /*HurryTypes*/ eIndex) const;

	int getHasReligionCount(int /*ReligionTypes*/ eIndex) const;
	int getHasCorporationCount(int /*CorporationTypes*/ eIndex) const;

	bool isSpecialistValid(int /*SpecialistTypes*/ iIndex) const;
	bool isResearchingTech(int /*TechTypes*/ iIndex) const;
	int /*CivicTypes*/ getCivics(int /*CivicOptionTypes*/ iIndex) const;
	int getSingleCivicUpkeep(int /*CivicTypes*/ eCivic, bool bIgnoreAnarchy) const;
	int getCivicUpkeep(bool bIgnoreAnarchy) const;
	void setCivics(int /*CivicOptionTypes*/ eIndex, int /*CivicTypes*/ eNewValue);

	int getCombatExperience() const;
	void changeCombatExperience(int iChange);
	void setCombatExperience(int iExperience);

	int getQueuePosition(int /*TechTypes*/ eTech) const;
	void clearResearchQueue();
	bool pushResearch(int /*TechTypes*/ iIndex, bool bClear);
	void popResearch(int /*TechTypes*/ eTech);
	int getLengthResearchQueue() const;

	void addCityName(const std::wstring szName);
	int getNumCityNames() const;
	std::wstring getCityName(int iIndex) const;

	python::list cities() const;
	python::tuple firstCity(bool bRev) const; // returns tuple of (CyCity, iterOut)
	python::tuple nextCity(int iterIn, bool bRev) const; // returns tuple of (CyCity, iterOut)
	int getNumCities() const;
	CyCity* getCity(int iID) const;

	python::list units() const;
	python::tuple firstUnit(bool bRev) const; // returns tuple of (CyUnit, iterOut)
	python::tuple nextUnit(int iterIn, bool bRev) const; // returns tuple of (CyUnit, iterOut)
	int getNumUnits() const;
	CyUnit* getUnit(int iID) const;

	python::list groups() const;
	int getNumSelectionGroups() const;
	CySelectionGroup* getSelectionGroup(int iID) const;

	void trigger(/*EventTriggerTypes*/int eEventTrigger);
	const EventTriggeredData* getEventOccured(int /*EventTypes*/ eEvent) const;
	void resetEventOccured(/*EventTypes*/ int eEvent);
	EventTriggeredData* getEventTriggered(int iID) const;
	EventTriggeredData* initTriggeredData(int eEventTrigger, bool bFire, int iCityId, int iPlotX, int iPlotY, int eOtherPlayer, int iOtherPlayerCityId, int eReligion, int eCorporation, int iUnitId, int eBuilding);

	int getEventTriggerWeight(int /*EventTriggerTypes*/ eTrigger) const;

	void AI_updateFoundValues(bool bStartingLoc);
	bool AI_isFinancialTrouble() const;
	bool AI_demandRebukedWar(int /*PlayerTypes*/ ePlayer);
	AttitudeTypes AI_getAttitude(int /*PlayerTypes*/ ePlayer) const;
	int AI_unitValue(int /*UnitTypes*/ eUnit, int /*UnitAITypes*/ eUnitAI, const CyArea* pArea) const;
	int AI_civicValue(int /*CivicTypes*/ eCivic) const;
	int AI_totalUnitAIs(int /*UnitAITypes*/ eUnitAI) const;
	int AI_totalAreaUnitAIs(const CyArea* pArea, int /*UnitAITypes*/ eUnitAI) const;
	int AI_getNumAIUnits(int /*UnitAITypes*/ eIndex) const;
	int AI_getAttitudeExtra(int /*PlayerTypes*/ eIndex) const;
	void AI_setAttitudeExtra(int /*PlayerTypes*/ eIndex, int iNewValue);
	void AI_changeAttitudeExtra(int /*PlayerTypes*/ eIndex, int iChange);
	int AI_getMemoryCount(int /*PlayerTypes*/ eIndex1, int /*MemoryTypes*/ eIndex2) const;
	void AI_changeMemoryCount(int /*PlayerTypes*/ eIndex1, int /*MemoryTypes*/ eIndex2, int iChange);

	bool AI_isWillingToTalk(int /*PlayerTypes*/ ePlayer) const;

	int64_t getScoreHistory(int iTurn) const;
	int64_t getEconomyHistory(int iTurn) const;
	int64_t getIndustryHistory(int iTurn) const;
	int64_t getAgricultureHistory(int iTurn) const;
	int64_t getPowerHistory(int iTurn) const;
	int64_t getCultureHistory(int iTurn) const;
	int64_t getEspionageHistory(int iTurn) const;
	int64_t getRevolutionStabilityHistory(int iTurn) const;

	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);

	int AI_maxGoldTrade(int iPlayer) const;
	int AI_maxGoldPerTurnTrade(int iPlayer) const;

	bool canSplitEmpire() const;
	bool canSplitArea(int iAreaId) const;
	bool assimilatePlayer(int iPlayer);

	bool canHaveTradeRoutesWith(int iPlayer) const;

	void forcePeace(int iPlayer);

	void addReminder(int iGameTurn, std::wstring szMessage) const;

	int getBuildingCountWithUpgrades(int /*BuildingTypes*/ iBuilding) const;
	void setColor(int /*PlayerColorTypes*/ iColor);
	void setHandicap(int iNewVal);

	void setModderOption(int /*ModderOptionTypes*/ eIndex, int iNewValue);

	void doRevolution(boost::python::list& /*CivicTypes**/ paeNewCivics, bool bForce);

	bool isAutomatedCanBuild(int /*BuildTypes*/ eIndex) const;
	void setAutomatedCanBuild(int /*BuildTypes*/ eIndex, bool bNewValue);

	int64_t getCulture() const;
	void setCulture(int64_t iNewValue);
	void changeCulture(int64_t iAddValue);

	CvProperties* getProperties() const;

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
	void makeNukesValid(bool bValid);

	// AIAndy: Build Lists
	int getBLNumLists();
	int getBLIndexByID(int iID);
	int getBLID(int index);
	std::wstring getBLListName(int index);
	int getBLListLength(int index);
	const OrderData* getBLOrder(int index, int iQIndex) const;
	void writeBLToFile();
	int getBLCurrentList();
	void addBLList();
	void renameBLList(int iID);
	void removeBLList(int iID);

private:
	CvPlayer* m_pPlayer;
};

#endif	// CyPlayer_h
