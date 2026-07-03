#pragma once

// player.h

#ifndef CIV4_PLAYER_H
#define CIV4_PLAYER_H

#include "copy_iterator.h"
#include "CvBuildLists.h"
#include "CvCityAI.h"
#include "CvContractBroker.h"
#include "CvGameObject.h"
#include "CvBuildLists.h"
#include "CvPlotGroup.h"
#include "CvProperties.h"
#include "CvSelectionGroupAI.h"
#include "CvTalkingHeadMessage.h"
#include "CvUnitList.h"
#include "CvUnitAI.h"
#include "index_iterator_base.h"
#include "LinkedList.h"
#ifdef CVARMY_BREAKSAVE
	#include "CvArmy.h"
#endif

class CvArea;
class CvDiploParameters;
class CvEventTriggerInfo;
class CvPlot;
class CvPopupInfo;
class CvUnitSelectionCriteria;
class CvUpgradeCache;
class CvPlayerAI;

#define	UNIT_BIRTHMARK_TEMP_UNIT	20000

#define MAX_RESEARCH_RATE_VALUE 1999999999
#define MAX_COMMERCE_RATE_VALUE 1999999999
#define MAX_COMMERCE_VALUE 900000000
#define MAX_RESEARCH_RATE_VALUE100 1999999999
#define MAX_COMMERCE_RATE_VALUE100 1999999999
#define MAX_COMMERCE_RATE_MODIFIER_VALUE 1999999999
#define MAX_GOLD_VALUE 9999999999999999
#define MAX_GOLD_PER_TURN_VALUE 1999999999
#define MIN_TOL_FALSE_ACCUMULATE -9999
#define MIN_TOL_FALSE_RESEARCH -9999

//	Struct used to hold civic switch history
typedef struct civcSwitchInstance
{
	int	iTurn;
	int	eFromCivic;
	int	eToCivic;
	bool bNoAnarchy;
} civcSwitchInstance;

typedef std::list<CvTalkingHeadMessage> CvMessageQueue;
typedef std::list<CvPopupInfo*> CvPopupQueue;
typedef std::list<CvDiploParameters*> CvDiploQueue;
typedef stdext::hash_map<int, int64_t> CvTurnScoreMap;
typedef stdext::hash_map<EventTypes, EventTriggeredData> CvEventMap;
typedef std::vector< std::pair<UnitCombatTypes, PromotionTypes> > UnitCombatPromotionArray;
typedef std::vector< std::pair<UnitTypes, PromotionTypes> > UnitPromotionArray;
typedef std::vector< std::pair<CivilizationTypes, LeaderHeadTypes> > CivLeaderArray;
typedef std::vector<TechTypes> techPath;

class CvPlayer
	: private bst::noncopyable
{
public:
	CvPlayer();
	virtual ~CvPlayer();

	CvGameObjectPlayer* getGameObject() { return &m_GameObject; };
	const CvGameObjectPlayer* getGameObject() const { return &m_GameObject; };

	void setIdleCity(const int iCityID, const bool bNewValue);
	bool hasIdleCity() const;
	CvCity* getIdleCity() const;
	bool isIdleCity(const int iCityID) const;
	void resetIdleCities();

	void processTech(const TechTypes eTech, const int iChange);

	bool hasHeritage(const HeritageTypes eType) const;
	bool canAddHeritage(const HeritageTypes eType, const bool bTestVisible = false) const;
	void setHeritage(const HeritageTypes eType, const bool bNewValue);
	int getHeritageCommerceEraChange(const CommerceTypes eType, const EraTypes eEra) const;
	std::vector<HeritageTypes> getHeritage() const { return m_myHeritage; }

protected:
	CvGameObjectPlayer m_GameObject;
	void baseInit(PlayerTypes eID);
	void initMore(PlayerTypes eID, LeaderHeadTypes ePersonality, bool bSetAlive = true);

	std::vector<int> m_idleCities;
	std::vector<CvUnit*> m_commanders;
	std::vector<CvPlot*> m_commandFieldPlots;
	std::vector<CvUnit*> m_commodores;
	std::vector<CvPlot*> m_commodoreFieldPlots;

	void processHeritage(const HeritageTypes eType, const int iChange);
	std::vector<HeritageTypes> m_myHeritage;

public:

	DllExport void init(PlayerTypes eID);
	DllExport void setupGraphical();
	DllExport void reset(PlayerTypes eID = NO_PLAYER, bool bConstructorCall = false);

	void resetPlotAndCityData();

	void changePersonalityType();
	void resetCivTypeEffects();
	void changeLeader(LeaderHeadTypes eNewLeader);
	void changeCiv(CivilizationTypes eNewCiv);
	void setIsHuman(bool bNewValue);

	void initInGame(PlayerTypes eID, bool bSetAlive);
	void setIsRebel(bool bNewValue);
	bool isRebel() const;
	int getStabilityIndex() const;
	void setStabilityIndex(int iNewValue);
	void changeStabilityIndex(int iChange);
	int getStabilityIndexAverage() const;
	void setStabilityIndexAverage(int iNewValue);
	void updateStabilityIndexAverage();

	bool haveSettlerUnit() const;
	bool isAliveAndTeam(const TeamTypes eTeam, const bool bSameTeam = true, const TeamTypes eTeamAlt = NO_TEAM) const;

	void uninit();

public:

	void initFreeState();
	void initFreeUnits();

	UnitTypes getBestUnitType(UnitAITypes eUnitAI) const;

	int getBestUnitTypeCargoVolume(UnitAITypes eUnitAI) const;
	bool addStartUnitAI(const UnitAITypes eUnitAI, const int iCount);

	int startingPlotRange() const;
	bool startingPlotWithinRange(CvPlot* pPlot, PlayerTypes ePlayer, int iRange, int iPass) const;
	int startingPlotDistanceFactor(const CvPlot* pPlot, PlayerTypes ePlayer, int iRange) const;
	int findStartingArea() const;
	CvPlot* findStartingPlot(bool bRandomize = false);

	CvPlotGroup* initPlotGroup(CvPlot* pPlot, bool bRecalculateBonuses);

	CvCity* initCity(int iX, int iY, bool bBumpUnits, bool bUpdatePlotGroups);
	void acquireCity(CvCity* pCity, bool bConquest, bool bTrade, bool bUpdatePlotGroups);
	void killCities();
	CvWString getNewCityName() const;
	void getCivilizationCityName(CvWString& szBuffer, CivilizationTypes eCivilization) const;
	bool isCityNameValid(CvWString& szName, bool bTestDestroyed = true) const;

	CvUnit* getTempUnit(UnitTypes eUnit, int iX, int iY);
	void releaseTempUnit();
	CvUnit* initUnit(UnitTypes eUnit, int iX, int iY, UnitAITypes eUnitAI, DirectionTypes eFacingDirection, int iBirthmark);
	void disbandUnit();
	void killUnits();

	CvSelectionGroup* cycleSelectionGroups(const CvUnit* pUnit, bool bForward, bool bWorkers, bool* pbWrap, bool bAllowViewportSwitch);

	bool hasTrait(TraitTypes eTrait) const;

	void setHumanDisabled(bool newVal);
	inline bool isHumanDisabled() const { return m_bDisableHuman; }
	inline bool isHumanPlayer(const bool bCountDisabled = false) const { return m_bHuman || bCountDisabled && m_bDisableHuman; }
	bool isNormalAI() const;

	DllExport bool isHuman() const;
	DllExport void updateHuman();
	DllExport bool isBarbarian() const;
	bool isNPC() const;
	bool isHominid() const;
	bool isAnimal() const;

	DllExport const wchar_t* getName(uint uiForm = 0) const;

	void setName(std::wstring szNewValue);
	void setCivName(std::wstring szNewDesc, std::wstring szNewShort, std::wstring szNewAdj);

	DllExport const wchar_t* getNameKey() const;
	DllExport const wchar_t* getCivilizationDescription(uint uiForm = 0) const;
	const wchar_t* getCivilizationDescriptionKey() const;
	const wchar_t* getCivilizationShortDescription(uint uiForm = 0) const;
	const wchar_t* getCivilizationShortDescriptionKey() const;
	const wchar_t* getCivilizationAdjective(uint uiForm = 0) const;
	const wchar_t* getCivilizationAdjectiveKey() const;
	DllExport CvWString getFlagDecal() const;
	DllExport bool isWhiteFlag() const;
	const wchar_t* getStateReligionName(uint uiForm = 0) const;
	const wchar_t* getStateReligionKey() const;
	const CvWString getBestAttackUnitName(uint uiForm = 0) const;
	const CvWString getWorstEnemyName() const;
	const wchar_t* getBestAttackUnitKey() const;
	DllExport ArtStyleTypes getArtStyleType() const;
	const char* getUnitButton(UnitTypes eUnit) const;

	void doTurn();
	void doMultiMapTurn();
	void doTurnUnits();

	void recordHistory();

	//	Dump stats to BBAI log
	void dumpStats() const;
	void NoteAnimalSubdued();
	void NoteUnitConstructed(BuildingTypes eBuilding);
	void NoteCivicsSwitched(int iNumChanges);

	void verifyCivics();

	void inhibitPlotGroupCalcsUntilFullRebuild(); //	Ignore updates until an update with reInitialize set
	void updatePlotGroups(const CvArea* possibleNewInAreaOnly = NULL, bool reInitialize = false);

	void updateYield();
	void updateMaintenance() const;
	void setMaintenanceDirty(const bool bDirty, const bool bCities = true) const;

	void updateFeatureHappiness(bool bLimited = false);
	void updateReligionHappiness(bool bLimited = false);

	void updateExtraSpecialistYield();
	void setCommerceDirty(CommerceTypes eIndex = NO_COMMERCE, bool bPlayerOnly = false);
	void updateCommerce(CommerceTypes eCommerce = NO_COMMERCE, bool bForce = true) const;
	void updateBuildingCommerce();
	void updateReligionCommerce();
	void updateCorporation();
	void updateCitySight(bool bIncrement, bool bUpdatePlotGroups);
	void updateTradeRoutes();
	void updatePlunder(int iChange, bool bUpdatePlotGroups);
	void updateTimers();
	CvCity* findClosestCity(const CvPlot* pPlot) const;

	bool hasReadyUnautomatedUnit(bool bAny = false) const;
	bool hasReadyUnit(bool bAny = false) const;
	bool hasAutoUnit() const;
	DllExport bool hasBusyUnit() const;

	bool isChoosingFreeTech() const;
	void startChoosingFreeTech();
	void endChoosingFreeTech();
	void chooseTech(int iDiscover = 0, CvWString szText = CvWString(), bool bFront = false);

	int calculateScore(bool bFinal = false, bool bVictory = false) const;

	int findBestFoundValue() const;

	int upgradeAllPrice(UnitTypes eUpgradeUnit, UnitTypes eFromUnit) const;

	int countReligionSpreadUnits(const CvArea* pArea, ReligionTypes eReligion, bool bIncludeTraining = false) const;
	int countCorporationSpreadUnits(const CvArea* pArea, CorporationTypes eCorporation, bool bIncludeTraining = false) const;

	int countNumCoastalCities() const;
	int countNumCoastalCitiesByArea(const CvArea* pArea) const;
	int countNumCitiesWithOrbitalInfrastructure() const;
	inline void noteOrbitalInfrastructureCountDirty() { m_orbitalInfrastructureCountDirty = true; }

	int countOwnedBonuses(BonusTypes eBonus) const;
	int countUnimprovedBonuses(const CvArea* pArea, const CvPlot* pFromPlot = NULL) const;
	int countCityFeatures(FeatureTypes eFeature) const;
	int countNumBuildings(BuildingTypes eBuilding) const;
	bool hasBuilding(const BuildingTypes eBuilding) const;
	int countNumCitiesConnectedToCapital() const;
	int countPotentialForeignTradeCities(const CvArea* pIgnoreArea = NULL) const;
	int countPotentialForeignTradeCitiesConnected() const;

	DllExport bool canContact(PlayerTypes ePlayer) const;
	void contact(PlayerTypes ePlayer);
	DllExport void handleDiploEvent(DiploEventTypes eDiploEvent, PlayerTypes ePlayer, int iData1, int iData2);
	bool canTradeWith(PlayerTypes eWhoTo) const;
	DllExport bool canTradeItem(PlayerTypes eWhoTo, TradeData item, bool bTestDenial = false) const;
	DllExport DenialTypes getTradeDenial(PlayerTypes eWhoTo, TradeData item) const;
	bool canTradeNetworkWith(PlayerTypes ePlayer) const;
	int getNumAvailableBonuses(BonusTypes eBonus) const;
	int getNumTradeableBonuses(BonusTypes eBonus) const;
	int getNumTradeBonusImports(PlayerTypes ePlayer) const;
	bool hasBonus(BonusTypes eBonus) const;

	bool isTradingWithTeam(TeamTypes eTeam, bool bIncludeCancelable) const;
	bool canStopTradingWithTeam(TeamTypes eTeam, bool bContinueNotTrading = false) const;
	void stopTradingWithTeam(TeamTypes eTeam);
	void killAllDeals();

	void findNewCapital();
	int getNumGovernmentCenters() const;

	void raze(CvCity* pCity);
	void disband(CvCity* pCity);

	bool canReceiveGoody(const CvPlot* pPlot, GoodyTypes eGoody, const CvUnit* pUnit) const;
	void receiveGoody(CvPlot* pPlot, GoodyTypes eGoody, CvUnit* pUnit);
	void doGoody(CvPlot* pPlot, CvUnit* pUnit);

	DllExport bool canFound(int iX, int iY, bool bTestVisible = false) const;
	void found(int iX, int iY, CvUnit* pUnit = NULL);

	bool canTrain(UnitTypes eUnit, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, bool bPropertySpawn = false) const;
	bool canConstruct(BuildingTypes eBuilding, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, TechTypes eIgnoreTechReq = NO_TECH, int* probabilityEverConstructable = NULL, bool bAffliction = false, bool bExposed = false) const;
	bool canConstructInternal(BuildingTypes eBuilding, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, TechTypes eIgnoreTechReq = NO_TECH, int* probabilityEverConstructable = NULL, bool bExposed = false) const;
	bool canCreate(ProjectTypes eProject, bool bContinue = false, bool bTestVisible = false) const;
	bool canMaintain(ProcessTypes eProcess) const;
	bool isProductionMaxedBuilding(BuildingTypes building, bool bAcquireCity = false) const;
	bool isProductionMaxedUnit(UnitTypes eUnit) const;
	bool isProductionMaxedProject(ProjectTypes eProject) const;
	int getProductionNeeded(UnitTypes eUnit) const;
	int getProductionNeeded(BuildingTypes eBuilding) const;
	int getProductionNeeded(ProjectTypes eProject) const;
	int getProductionModifier(UnitTypes eUnit) const;
	int getProductionModifier(BuildingTypes eBuilding) const;
	int getProductionModifier(ProjectTypes eProject) const;

	int64_t getBaseUnitCost100(const UnitTypes eUnit) const;

	int getBuildingPrereqBuilding(BuildingTypes eBuilding, BuildingTypes ePrereqBuilding, int iExtra = 0) const;
	void removeBuilding(BuildingTypes building);

	void processBuilding(BuildingTypes eBuilding, int iChange, CvArea* pArea, bool bReligiouslyDisabling = false);

	int getBuildCost(const CvPlot* pPlot, BuildTypes eBuild) const;
	bool canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestVisible = false, bool bIncludePythonOverrides = true) const;

	RouteTypes getBestRoute(const CvPlot* pPlot = NULL, bool bConnect = true, const CvUnit* pBuilder = NULL) const;

	int getImprovementUpgradeProgressRate(const ImprovementTypes eImprovement) const;

	int calculateTotalYield(YieldTypes eYield) const;
	int calculateTotalExports(YieldTypes eYield) const;
	int calculateTotalImports(YieldTypes eYield) const;

	int calculateTotalCityHappiness() const;
	int calculateTotalCityUnhappiness() const;

	int calculateTotalCityHealthiness() const;
	int calculateTotalCityUnhealthiness() const;

	int calculateUnitCost(int& iFreeUnits, int& iFreeMilitaryUnits, int& iPaidUnits, int& iPaidMilitaryUnits, int& iBaseUnitCost, int& iMilitaryCost, int& iExtraCost) const;

	int calculateUnitSupply(int& iPaidUnits, int& iBaseSupplyCost) const;
	int calculateUnitSupply() const;
	int64_t calculatePreInflatedCosts() const;
	int getInflationMod10000() const;
	int64_t getInflationCost() const;
	int64_t getFinalExpense() const;
	short getProfitMargin(int64_t &iNetExpenses, int iExtraExpense=0, int iExtraExpenseMod=0) const;
	short getProfitMargin(int iExtraExpense=0, int iExtraExpenseMod=0) const;
	void cacheKeyFinanceNumbers();
	int64_t getMinTaxIncome() const;
	int64_t getMaxTaxIncome() const;

	int64_t calculateBaseNetGold() const;
	int calculateBaseNetResearch(TechTypes eTech = NO_TECH) const;
	int calculateResearchModifier(TechTypes eTech) const;
	int calculateGoldRate() const;
	int calculateResearchRate(TechTypes eTech = NO_TECH) const;
	int calculateTotalCommerce() const;

	bool canEverResearch(TechTypes eTech) const;
	bool canResearch(const TechTypes eTech, const bool bRightNow = true, const bool bSpecialRequirements = true) const;
	TechTypes getCurrentResearch() const;
	bool isCurrentResearchRepeat() const;
	bool isNoResearchAvailable() const;
	int getResearchTurnsLeft(TechTypes eTech, bool bOverflow) const;

	bool isCivic(CivicTypes eCivic) const;
	bool canDoCivics(CivicTypes eCivic) const;
	bool canRevolution(CivicTypes* paeNewCivics) const;
	void revolution(CivicTypes* paeNewCivics, bool bForce = false);
	int getCivicPercentAnger(CivicTypes eCivic) const;

	bool canDoReligion(ReligionTypes eReligion) const;
	bool canChangeReligion() const;
	bool canConvert(ReligionTypes eReligion) const;
	void convert(ReligionTypes eReligion);
	bool hasHolyCity(ReligionTypes eReligion) const;
	bool hasHolyCity() const;
	bool hasStateReligionHolyCity() const;
	bool hasStateReligionShrine() const;

	int countHolyCities() const;
	void foundReligion(ReligionTypes eReligion, ReligionTypes eSlotReligion, bool bAward);

	bool hasHeadquarters(CorporationTypes eCorporation) const;
	int countHeadquarters() const;
	int countCorporations(CorporationTypes eCorporation) const;
	void foundCorporation(CorporationTypes eCorporation);

	int getCivicAnarchyLength(CivicTypes* paeNewCivics) const;
	int getReligionAnarchyLength() const;

	int unitsRequiredForGoldenAge() const;
	int unitsGoldenAgeCapable() const;
	int unitsGoldenAgeReady() const;
	void killGoldenAgeUnits(CvUnit* pUnitAlive);

	int greatPeopleThresholdMilitary() const;
	int greatPeopleThresholdNonMilitary() const;

	int specialistYield(SpecialistTypes eSpecialist, YieldTypes eYield) const;
	int specialistCommerceTimes100(SpecialistTypes eSpecialist, CommerceTypes eCommerce) const;
	int specialistCommerce(SpecialistTypes eSpecialist, CommerceTypes eCommerce) const;

	CvPlot* getStartingPlot() const;
	void setStartingPlot(CvPlot* pNewValue, const bool bUpdateStartDist);

	int getTotalPopulation() const;
	int getAveragePopulation() const;
	void changeTotalPopulation(int iChange);
	int64_t getRealPopulation() const;
	int getReligionPopulation(ReligionTypes eReligion) const;

	int getTotalLand() const;
	void changeTotalLand(int iChange);

	int getTotalLandScored() const;
	void changeTotalLandScored(int iChange);

	int64_t getGold() const;
	void setGold(int64_t iNewValue);
	DllExport void setGold(int iNewValue) { setGold(static_cast<int64_t>(iNewValue)); }
	void changeGold(int64_t iChange);
	DllExport void changeGold(int iChange) { setGold(static_cast<int64_t>(getGold() + iChange)); }

	int getGoldPerTurn() const;

	int getAdvancedStartPoints() const;
	void setAdvancedStartPoints(int iNewValue);
	void changeAdvancedStartPoints(int iChange);

	void doEspionageOneOffPoints(int iChange);
	int getEspionageSpending(TeamTypes eAgainstTeam, int iTotal = -1) const;
	bool canDoEspionageMission(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvPlot* pPlot, int iExtraData, const CvUnit* pUnit) const;
	int64_t getEspionageMissionBaseCost(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvPlot* pPlot, int iExtraData, const CvUnit* pSpyUnit) const;
	int getEspionageMissionCost(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvPlot* pPlot = NULL, int iExtraData = -1, const CvUnit* pSpyUnit = NULL) const;
	int getEspionageMissionCostModifier(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvPlot* pPlot, const CvUnit* pSpyUnit) const;
	bool doEspionageMission(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, CvPlot* pPlot, int iExtraData, CvUnit* pUnit, bool bCaught = false);
	int getEspionageSpendingWeightAgainstTeam(TeamTypes eIndex) const;
	void setEspionageSpendingWeightAgainstTeam(TeamTypes eIndex, int iValue);
	void changeEspionageSpendingWeightAgainstTeam(TeamTypes eIndex, int iChange);

	bool canStealTech(PlayerTypes eTarget, TechTypes eTech) const;
	bool canForceCivics(PlayerTypes eTarget, CivicTypes eCivic) const;
	bool canForceReligion(PlayerTypes eTarget, ReligionTypes eReligion) const;
	bool canSpyDestroyUnit(PlayerTypes eTarget, const CvUnit& kUnit) const;
	bool canSpyBribeUnit(PlayerTypes eTarget, const CvUnit& kUnit) const;
	bool canSpyDestroyBuilding(PlayerTypes eTarget, BuildingTypes eBuilding) const;
	bool canSpyDestroyProject(PlayerTypes eTarget, ProjectTypes eProject) const;

	void doAdvancedStartAction(AdvancedStartActionTypes eAction, int iX, int iY, int iData, bool bAdd);
	int getAdvancedStartUnitCost(UnitTypes eUnit, bool bAdd, const CvPlot* pPlot = NULL) const;
	int getAdvancedStartCityCost(bool bAdd, const CvPlot* pPlot = NULL) const;
	int getAdvancedStartPopCost(bool bAdd, const CvCity* pCity = NULL) const;
	int getAdvancedStartCultureCost(bool bAdd, const CvCity* pCity = NULL) const;
	int getAdvancedStartBuildingCost(BuildingTypes eBuilding, bool bAdd, const CvCity* pCity = NULL) const;
	int getAdvancedStartImprovementCost(ImprovementTypes eImprovement, bool bAdd, const CvPlot* pPlot = NULL) const;
	int getAdvancedStartRouteCost(RouteTypes eRoute, bool bAdd, const CvPlot* pPlot = NULL) const;
	int getAdvancedStartTechCost(TechTypes eTech, bool bAdd) const;
	int getAdvancedStartVisibilityCost(const CvPlot* pPlot = NULL) const;

	int getGoldenAgeTurns() const;
	bool isGoldenAge() const;
	void changeGoldenAgeTurns(int iChange);
	int getGoldenAgeLength() const;

	int getNumUnitGoldenAges() const;
	void changeNumUnitGoldenAges(int iChange);

	int getAnarchyTurns() const;
	bool isAnarchy() const;

	void changeAnarchyTurns(int iChange, bool bHideMessages = false);

	int getStrikeTurns() const;
	void changeStrikeTurns(int iChange);

	int getMaxAnarchyTurns() const;
	void updateMaxAnarchyTurns();

	int getMinAnarchyTurns() const;
	void updateMinAnarchyTurns();

	int getAnarchyModifier() const;
	void changeAnarchyModifier(int iChange);

	int getGoldenAgeModifier() const;
	void changeGoldenAgeModifier(int iChange);

	int getHurryModifier() const;
	void changeHurryModifier(int iChange);

	void createGreatPeople(UnitTypes eGreatPersonUnit, bool bIncrementThreshold, bool bIncrementExperience, int iX, int iY);

	int getGreatPeopleCreated() const;
	void incrementGreatPeopleCreated();

	int getGreatGeneralsCreated() const;
	void incrementGreatGeneralsCreated();

	int getGreatPeopleThresholdModifier() const;
	void changeGreatPeopleThresholdModifier(int iChange);

	int getGreatGeneralsThresholdModifier() const;
	void changeGreatGeneralsThresholdModifier(int iChange);

	int getGreatPeopleRateModifier() const;
	void changeGreatPeopleRateModifier(int iChange);

	int getGreatGeneralRateModifier() const;
	void changeGreatGeneralRateModifier(int iChange);

	int getDomesticGreatGeneralRateModifier() const;
	void changeDomesticGreatGeneralRateModifier(int iChange);

	int getStateReligionGreatPeopleRateModifier() const;
	void changeStateReligionGreatPeopleRateModifier(int iChange);

	int getMaxGlobalBuildingProductionModifier() const;
	void changeMaxGlobalBuildingProductionModifier(int iChange);

	int getMaxTeamBuildingProductionModifier() const;
	void changeMaxTeamBuildingProductionModifier(int iChange);

	int getMaxPlayerBuildingProductionModifier() const;
	void changeMaxPlayerBuildingProductionModifier(int iChange);

	int getFreeExperience() const;
	void changeFreeExperience(int iChange);

	int getFeatureProductionModifier() const;
	void changeFeatureProductionModifier(int iChange);

	int getWorkerSpeedModifier() const;
	void changeWorkerSpeedModifier(int iChange);
	int getWorkRate(BuildTypes eBuild) const;
	int getImprovementUpgradeRateModifier() const;
	void changeImprovementUpgradeRateModifier(int iChange);

	int getMilitaryProductionModifier() const;
	void changeMilitaryProductionModifier(int iChange);

	int getSpaceProductionModifier() const;
	void changeSpaceProductionModifier(int iChange);

	int getCityDefenseModifier() const;
	void changeCityDefenseModifier(int iChange);

	bool isNonStateReligionCommerce() const;
	void changeNonStateReligionCommerce(int iNewValue);

	bool isUpgradeAnywhere() const;
	void changeUpgradeAnywhere(int iNewValue);

	int getRevIdxLocal() const;
	void changeRevIdxLocal(int iChange);

	int getRevIdxNational() const;
	void changeRevIdxNational(int iChange);

	int getRevIdxDistanceModifier() const;
	void changeRevIdxDistanceModifier(int iChange);

	int getRevIdxHolyCityGood() const;
	void changeRevIdxHolyCityGood(int iChange);

	int getRevIdxHolyCityBad() const;
	void changeRevIdxHolyCityBad(int iChange);

	float getRevIdxNationalityMod() const;
	void changeRevIdxNationalityMod(float fChange);

	float getRevIdxBadReligionMod() const;
	void changeRevIdxBadReligionMod(float fChange);

	float getRevIdxGoodReligionMod() const;
	void changeRevIdxGoodReligionMod(float fChange);

	bool isInquisitionConditions() const;
	void setInquisitionConditions();

	int getUnitUpgradePriceModifier() const;
	void changeUnitUpgradePriceModifier(int iChange);

	bool canFoundReligion() const;

	int getNumNukeUnits() const;
	void changeNumNukeUnits(int iChange);

	int getNumOutsideUnits() const;
	void changeNumOutsideUnits(int iChange);

	int getBaseFreeUnitUpkeepCivilian() const;
	int getBaseFreeUnitUpkeepMilitary() const;
	void changeBaseFreeUnitUpkeepCivilian(const int iChange);
	void changeBaseFreeUnitUpkeepMilitary(const int iChange);
	int getFreeUnitUpkeepCivilianPopPercent() const;
	int getFreeUnitUpkeepMilitaryPopPercent() const;
	void changeFreeUnitUpkeepCivilianPopPercent(const int iChange);
	void changeFreeUnitUpkeepMilitaryPopPercent(const int iChange);
	int getFreeUnitUpkeepCivilian() const;
	int getFreeUnitUpkeepMilitary() const;

	int getTypicalUnitValue(UnitAITypes eUnitAI) const;

	// Toffer - Unit Upkeep
	int getCivilianUnitUpkeepMod() const;
	int getMilitaryUnitUpkeepMod() const;
	void changeCivilianUnitUpkeepMod(const int iChange);
	void changeMilitaryUnitUpkeepMod(const int iChange);
	void changeUnitUpkeep(const int iChange, const bool bMilitary);
	void setUnitUpkeepDirty() const;

	int64_t getUnitUpkeepCivilian100() const;
	int64_t getUnitUpkeepCivilian() const;
	int64_t getUnitUpkeepCivilianNet() const;
	int64_t getUnitUpkeepMilitary100() const;
	int64_t getUnitUpkeepMilitary() const;
	int64_t getUnitUpkeepMilitaryNet() const;
	int64_t getUnitUpkeepNet(const bool bMilitary, const int iUnitUpkeep = MAX_INT) const;
	int64_t calcFinalUnitUpkeep(const bool bReal=true) const;
	int64_t getFinalUnitUpkeep() const;
	int getFinalUnitUpkeepChange(const int iExtra, const bool bMilitary);
	// ! Unit Upkeep

	int getNumMilitaryUnits() const;
	void changeNumMilitaryUnits(int iChange);

	int getHappyPerMilitaryUnit() const;

	void changeHappyPerMilitaryUnit(int iChange, bool bLimited = false);

	int getMilitaryFoodProductionCount() const;
	bool isMilitaryFoodProduction() const;

	void changeMilitaryFoodProductionCount(int iChange, bool bLimited = false);

	int getHighestUnitLevel() const;
	void setHighestUnitLevel(int iNewValue);

	int getConscriptCount() const;
	void setConscriptCount(int iNewValue);
	void changeConscriptCount(int iChange);

	int getMaxConscript() const;
	void changeMaxConscript(int iChange);

	int getOverflowResearch() const;
	void changeOverflowResearch(int iChange);

	int getNoUnhealthyPopulationCount() const;
	bool isNoUnhealthyPopulation() const;

	void changeNoUnhealthyPopulationCount(int iChange, bool bLimited = false);

	int getExpInBorderModifier() const;
	void changeExpInBorderModifier(int iChange);

	int getBuildingOnlyHealthyCount() const;
	bool isBuildingOnlyHealthy() const;


	int getMaintenanceModifier();
	void changeMaintenanceModifier(int iChange);
	int getCoastalDistanceMaintenanceModifier() const;
	void changeCoastalDistanceMaintenanceModifier(int iChange);
	int getConnectedCityMaintenanceModifier();
	void changeConnectedCityMaintenanceModifier(int iChange);

	void changeBuildingOnlyHealthyCount(int iChange, bool bLimited = false);

	inline int getDistanceMaintenanceModifier() const { return m_iDistanceMaintenanceModifier; }
	void changeDistanceMaintenanceModifier(const int iChange);

	inline int getNumCitiesMaintenanceModifier() const { return m_iNumCitiesMaintenanceModifier; }
	void changeNumCitiesMaintenanceModifier(const int iChange);

	inline int getCorporationMaintenanceModifier() const { return m_iCorporationMaintenanceModifier; }
	void changeCorporationMaintenanceModifier(const int iChange, const bool bLimited = false);

	inline int getHomeAreaMaintenanceModifier() const { return m_iHomeAreaMaintenanceModifier; }
	void changeHomeAreaMaintenanceModifier(const int iChange);

	inline int getOtherAreaMaintenanceModifier() const { return m_iOtherAreaMaintenanceModifier; }
	void changeOtherAreaMaintenanceModifier(const int iChange);

	int getTotalMaintenance() const;

	int getUpkeepModifier() const;
	void changeUpkeepModifier(int iChange);

	int getLevelExperienceModifier() const;
	void changeLevelExperienceModifier(int iChange);

	int getExtraHealth() const;
	void changeExtraHealth(int iChange);
	void changeCivicHealth(const int iChange, const bool bLimited = false);

	int getCivicHealth() const; // Included in getExtraHealth() but split off to aid hover text displays

	int getBuildingGoodHealth() const;
	void changeBuildingGoodHealth(int iChange);

	int getBuildingBadHealth() const;
	void changeBuildingBadHealth(int iChange);

	int getExtraHappiness() const;
	void changeExtraHappiness(int iChange, bool bUnattributed = false);

	int getBuildingHappiness() const;
	void changeBuildingHappiness(int iChange);

	int getLargestCityHappiness() const;
	void changeLargestCityHappiness(int iChange, bool bLimited = false);

	int getWarWearinessPercentAnger() const;
	void updateWarWearinessPercentAnger();
	int getModifiedWarWearinessPercentAnger(int iWarWearinessPercentAnger) const;

	int getWarWearinessModifier() const;
	void changeWarWearinessModifier(int iChange, bool bLimited = false);

	int getFreeSpecialist() const;
	void changeFreeSpecialist(int iChange);

	int getNoForeignTradeCount() const;
	bool isNoForeignTrade() const;
	void changeNoForeignTradeCount(int iChange, bool bLimited = false);

	int getNoCorporationsCount() const;
	bool isNoCorporations() const;
	void changeNoCorporationsCount(int iChange, bool bLimited = false);

	int getNoForeignCorporationsCount() const;
	bool isNoForeignCorporations() const;
	void changeNoForeignCorporationsCount(int iChange, bool bLimited = false);

	int getCoastalTradeRoutes() const;
	void changeCoastalTradeRoutes(int iChange);

	int getTradeRoutes() const;
	void changeTradeRoutes(int iChange);

	int getRevolutionTimer() const;
	void setRevolutionTimer(int iNewValue);
	void changeRevolutionTimer(int iChange);

	int getConversionTimer() const;
	void setConversionTimer(int iNewValue);
	void changeConversionTimer(int iChange);

	int getStateReligionCount() const;
	bool isStateReligion() const;
	void changeStateReligionCount(int iChange, bool bLimited = false);

	int getNoNonStateReligionSpreadCount() const;
	bool isNoNonStateReligionSpread() const;
	void changeNoNonStateReligionSpreadCount(int iChange);

	int getStateReligionHappiness() const;
	void changeStateReligionHappiness(int iChange, bool bLimited = false);
	int getNonStateReligionHappiness() const;
	void changeNonStateReligionHappiness(int iChange, bool bLimited = false);

	int getStateReligionUnitProductionModifier() const;
	void changeStateReligionUnitProductionModifier(int iChange);

	int getStateReligionBuildingProductionModifier() const;
	void changeStateReligionBuildingProductionModifier(int iChange);

	int getStateReligionFreeExperience() const;
	void changeStateReligionFreeExperience(int iChange);

	DllExport CvCity* getCapitalCity() const;
	void setCapitalCity(CvCity* pNewCapitalCity);

	int getCitiesLost() const;
	void changeCitiesLost(int iChange);

	int getWinsVsBarbs() const;
	void changeWinsVsBarbs(int iChange);

	int getAssets() const;
	void changeAssets(int iChange);

	int getPower() const;
	void changePower(int iChange);

	int getTechPower() const;
	void changeTechPower(int iChange);
	int getUnitPower() const;
	void changeUnitPower(int iChange);

	int getPopScore(bool bCheckVassal = true) const;
	void changePopScore(int iChange);
	int getLandScore(bool bCheckVassal = true) const;
	void changeLandScore(int iChange);
	int getTechScore() const;
	void changeTechScore(int iChange);
	int getWondersScore() const;
	void changeWondersScore(int iChange);

	int getCombatExperience() const;
	void setCombatExperience(int iExperience, UnitTypes eGGType = NO_UNIT);
	void changeCombatExperience(int iChange, UnitTypes eGGType = NO_UNIT);

	bool isConnected() const;
	DllExport int getNetID() const;
	DllExport void setNetID(int iNetID);
	void sendReminder();

	uint getStartTime() const;
	DllExport void setStartTime(uint uiStartTime);
	uint getTotalTimePlayed() const;

	bool isMinorCiv() const;

	DllExport bool isAlive() const;
	bool isEverAlive() const;
	void setAlive(bool bNewValue, bool bActivateTurn = true);
	void verifyAlive();

	DllExport bool isTurnActive() const;
	DllExport void setTurnActive(bool bNewValue, bool bDoTurn = true);

	bool isAutoMoves() const;
	void setAutoMoves(bool bNewValue);
	DllExport void setTurnActiveForPbem(bool bActive);

	DllExport bool isPbemNewTurn() const;
	DllExport void setPbemNewTurn(bool bNew);

	bool isEndTurn() const { return m_bEndTurn; }
	DllExport void setEndTurn(bool bNewValue);
	void changeEndTurn(const bool bNewValue, const bool bForce = false);

	bool isForcedCityCycle() const;
	void setForcedCityCycle(const bool bNewValue) { m_bForcedCityCycle = bNewValue; }

	DllExport bool isTurnDone() const;

	bool isExtendedGame() const;
	void makeExtendedGame();

	bool isFoundedFirstCity() const;
	void setFoundedFirstCity(bool bNewValue);

	bool isStrike() const;
	void setStrike(bool bNewValue);

	DllExport PlayerTypes getID() const;

	DllExport HandicapTypes getHandicapType() const;

	DllExport CivilizationTypes getCivilizationType() const;

	DllExport LeaderHeadTypes getLeaderType() const;

	LeaderHeadTypes getPersonalityType() const;
	void setPersonalityType(LeaderHeadTypes eNewValue);

	DllExport EraTypes getCurrentEra() const;
	void setCurrentEra(EraTypes eNewValue);

	int64_t getCulture() const;
	void changeCulture(int64_t iAddValue);

	ReligionTypes getLastStateReligion() const;
	ReligionTypes getStateReligion() const;
	void setLastStateReligion(const ReligionTypes eNewReligion);

	PlayerTypes getParent() const;
	void setParent(PlayerTypes eParent);

	DllExport TeamTypes getTeam() const;
	void setTeam(TeamTypes eTeam);
	void updateTeamType();

	bool isDoNotBotherStatus(PlayerTypes playerID) const;

	DllExport PlayerColorTypes getPlayerColor() const;
	DllExport int getPlayerTextColorR() const;
	DllExport int getPlayerTextColorG() const;
	DllExport int getPlayerTextColorB() const;
	int getPlayerTextColorA() const;

	int getSeaPlotYield(YieldTypes eIndex) const;
	void changeSeaPlotYield(YieldTypes eIndex, int iChange);

	int getGoldenAgeYield(YieldTypes eIndex) const;
	void changeGoldenAgeYield(YieldTypes eIndex, int iChange);

	int getGoldenAgeCommerce(CommerceTypes eIndex) const;
	void changeGoldenAgeCommerce(CommerceTypes eIndex, int iChange);

	int getYieldRateModifier(YieldTypes eIndex) const;
	void changeYieldRateModifier(YieldTypes eIndex, int iChange);

	int getCapitalYieldRateModifier(YieldTypes eIndex) const;
	void changeCapitalYieldRateModifier(YieldTypes eIndex, int iChange);

	int getExtraYieldThreshold(YieldTypes eIndex) const;
	void updateExtraYieldThreshold(YieldTypes eIndex);

	int getLessYieldThreshold(YieldTypes eIndex) const;
	void updateLessYieldThreshold(YieldTypes eIndex);

	int getTradeYieldModifier(YieldTypes eIndex) const;
	void changeTradeYieldModifier(YieldTypes eIndex, int iChange);

	int getExtraCommerce100(const CommerceTypes eIndex) const;
	void changeExtraCommerce100(const CommerceTypes eIndex, const int iChange);

	int getCommercePercent(CommerceTypes eIndex) const;
	void setCommercePercent(CommerceTypes eIndex, int iNewValue);
	void changeCommercePercent(CommerceTypes eIndex, int iChange);

	int getTotalCityBaseCommerceRate(CommerceTypes eIndex) const;
	int getCommerceRate(CommerceTypes eIndex) const;
	void changeCommerceRate(CommerceTypes eIndex, int iChange);

	int getCommerceRateModifier(CommerceTypes eIndex) const;
	void changeCommerceRateModifier(CommerceTypes eIndex, int iChange);

	int getCommerceRateModifierfromEvents(CommerceTypes eIndex) const;
	void changeCommerceRateModifierfromEvents(CommerceTypes eIndex, int iChange);

	int getCommerceRateModifierfromBuildings(CommerceTypes eIndex) const;
	void changeCommerceRateModifierfromBuildings(CommerceTypes eIndex, int iChange);

	int getCapitalCommerceRateModifier(CommerceTypes eIndex) const;
	void changeCapitalCommerceRateModifier(CommerceTypes eIndex, int iChange);

	int getStateReligionBuildingCommerce(CommerceTypes eIndex) const;
	void changeStateReligionBuildingCommerce(CommerceTypes eIndex, int iChange);

	int getSpecialistExtraCommerce(CommerceTypes eIndex) const;
	void changeSpecialistExtraCommerce(CommerceTypes eIndex, int iChange);

	int getCommerceFlexibleCount(CommerceTypes eIndex) const;
	bool isCommerceFlexible(CommerceTypes eIndex) const;
	void changeCommerceFlexibleCount(CommerceTypes eIndex, int iChange);

	int getGoldPerTurnByPlayer(PlayerTypes eIndex) const;
	void changeGoldPerTurnByPlayer(PlayerTypes eIndex, int iChange);

	bool isFeatAccomplished(FeatTypes eIndex) const;
	void setFeatAccomplished(FeatTypes eIndex, bool bNewValue);

	DllExport bool isOption(PlayerOptionTypes eIndex) const;
	DllExport void setOption(PlayerOptionTypes eIndex, bool bNewValue);

	bool isLoyalMember(VoteSourceTypes eVoteSource) const;
	void setLoyalMember(VoteSourceTypes eVoteSource, bool bNewValue);

	bool isPlayable() const;
	void setPlayable(bool bNewValue);

	int getBonusExport(const BonusTypes eBonus) const;
	int getBonusImport(const BonusTypes eBonus) const;
	void changeBonusExport(const BonusTypes eBonus, const int iChange);
	void changeBonusImport(const BonusTypes eBonus, const int iChange);

	int getImprovementCount(ImprovementTypes eIndex) const;
	void changeImprovementCount(ImprovementTypes eIndex, int iChange);

	uint16_t getFreeBuildingCount(const BuildingTypes eIndex) const;
	void checkFreeBuildings(CvCity* city);
	uint16_t getFreeAreaBuildingCount(BuildingTypes eIndex, const CvArea* area) const;
	bool isBuildingFree(BuildingTypes eIndex, const CvArea* area = NULL) const;
	void changeFreeBuildingCount(const BuildingTypes eIndex, const int iChange);

	int getExtraBuildingHappiness(const BuildingTypes eIndex) const;
	void changeExtraBuildingHappiness(const BuildingTypes eIndex, const int iChange, const bool bLimited = false);
	int getExtraBuildingHealth(const BuildingTypes eIndex) const;
	void changeExtraBuildingHealth(const BuildingTypes eIndex, const int iChange, const bool bLimited = false);

	int getFeatureHappiness(FeatureTypes eIndex) const;
	void changeFeatureHappiness(FeatureTypes eIndex, int iChange, bool bLimited = false);

	int getUnitCount(const UnitTypes eUnit) const;
	void changeUnitCount(const UnitTypes eUnit, const int iChange);
	int getUnitCountSM(const UnitTypes eUnit) const;
	void changeUnitCountSM(const UnitTypes eUnit, const int iChange);
	bool isUnitMaxedOut(const UnitTypes eUnit, const int iExtra = 0) const;
	int getUnitMaking(const UnitTypes eUnit) const;
	void changeUnitMaking(const UnitTypes eUnit, int iChange);
	int getUnitCountPlusMaking(const UnitTypes eUnit) const;

	int getBuildingCount(BuildingTypes eIndex) const;
	int getBuildingGroupCount(SpecialBuildingTypes eIndex) const;
	bool isBuildingMaxedOut(BuildingTypes eIndex, int iExtra = 0) const;
	bool isBuildingGroupMaxedOut(SpecialBuildingTypes eIndex, int iExtra = 0) const;
	void changeBuildingCount(BuildingTypes eIndex, int iChange);
	void changeBuildingGroupCount(SpecialBuildingTypes eIndex, int iChange);

	int getBuildingMaking(const BuildingTypes eIndex) const;
	void changeBuildingMaking(const BuildingTypes eIndex, int iChange);

	int getBuildingGroupMaking(SpecialBuildingTypes eIndex) const;
	void changeBuildingGroupMaking(SpecialBuildingTypes eIndex, int iChange);

	int getBuildingCountPlusMaking(BuildingTypes eIndex) const;
	int getBuildingGroupCountPlusMaking(SpecialBuildingTypes eIndex) const;

	int getHurryCount(HurryTypes eIndex) const;
	bool canHurry(HurryTypes eIndex) const;
	bool canPopRush() const;
	void changeHurryCount(HurryTypes eIndex, int iChange);

	int getSpecialBuildingNotRequiredCount(SpecialBuildingTypes eIndex) const;
	bool isSpecialBuildingNotRequired(SpecialBuildingTypes eIndex) const;
	void changeSpecialBuildingNotRequiredCount(SpecialBuildingTypes eIndex, int iChange);

	int getHasCivicOptionCount(CivicOptionTypes eIndex) const;
	bool isHasCivicOption(CivicOptionTypes eIndex) const;
	void changeHasCivicOptionCount(CivicOptionTypes eIndex, int iChange);

	int getNoCivicUpkeepCount(CivicOptionTypes eIndex) const;
	bool isNoCivicUpkeep(CivicOptionTypes eIndex) const;
	void changeNoCivicUpkeepCount(CivicOptionTypes eIndex, int iChange);

	int getHasReligionCount(ReligionTypes eIndex) const;
	int countTotalHasReligion() const;
	int findHighestHasReligionCount() const;
	void changeHasReligionCount(ReligionTypes eIndex, int iChange);

	int getHasCorporationCount(CorporationTypes eIndex) const;
	int countTotalHasCorporation() const;
	void changeHasCorporationCount(CorporationTypes eIndex, int iChange);
	bool isActiveCorporation(CorporationTypes eIndex) const;

	int getUpkeepCount(UpkeepTypes eIndex) const;
	void changeUpkeepCount(UpkeepTypes eIndex, int iChange);

	int getSpecialistValidCount(SpecialistTypes eIndex) const;
	bool isSpecialistValid(SpecialistTypes eIndex) const;

	CvProperties* getProperties();
	const CvProperties* getPropertiesConst() const;

	void changeSpecialistValidCount(SpecialistTypes eIndex, int iChange, bool bLimited = false);

	bool isResearchingTech(TechTypes eIndex) const;
	void setResearchingTech(TechTypes eIndex, bool bNewValue);

	CivicTypes getCivics(CivicOptionTypes eIndex) const;
	int getSingleCivicUpkeep(CivicTypes eCivic, bool bIgnoreAnarchy = false) const;
	int getCivicUpkeep(bool bIgnoreAnarchy = false) const;
	void setCivics(CivicOptionTypes eIndex, CivicTypes eNewValue);

	int64_t getTreasuryUpkeep() const;

	int getExtraSpecialistYield(SpecialistTypes eIndex1, YieldTypes eIndex2) const;
	void changeExtraSpecialistYield(SpecialistTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const;
	void changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange);

	void updateGroupCycle(CvUnit* pUnit, bool bFarMove);
	CLLNode<int>* removeGroupCycle(int iID);
	CLLNode<int>* deleteGroupCycleNode(CLLNode<int>* pNode);
	CLLNode<int>* nextGroupCycleNode(CLLNode<int>* pNode) const;
	CLLNode<int>* previousGroupCycleNode(CLLNode<int>* pNode) const;
	CLLNode<int>* headGroupCycleNode() const;
	CLLNode<int>* tailGroupCycleNode() const;

	int findPathLength(TechTypes eTech, bool bCost = true) const;
	int getQueuePosition(TechTypes eTech) const;
	void clearResearchQueue();
	bool pushResearch(TechTypes eTech, bool bClear = false);
	void popResearch(TechTypes eTech);
	int getLengthResearchQueue() const;
	CLLNode<TechTypes>* nextResearchQueueNode(CLLNode<TechTypes>* pNode) const;
	CLLNode<TechTypes>* headResearchQueueNode() const;
	CLLNode<TechTypes>* tailResearchQueueNode() const;

	void addCityName(const CvWString& szName);
	int getNumCityNames() const;
	CvWString getCityName(int iIndex) const;
	CLLNode<CvWString>* nextCityNameNode(CLLNode<CvWString>* pNode) const;
	CLLNode<CvWString>* headCityNameNode() const;

	// plot groups iteration
	DECLARE_INDEX_ITERATOR(const CvPlayer, CvPlotGroup, plot_group_iterator, firstPlotGroup, nextPlotGroup);
	plot_group_iterator beginPlotGroups() const { return plot_group_iterator(this); }
	plot_group_iterator endPlotGroups() const { return plot_group_iterator(); }
	typedef bst::iterator_range<plot_group_iterator> plot_group_range;
	plot_group_range plot_groups() const { return plot_group_range(beginPlotGroups(), endPlotGroups()); }

	// deprecated, use plot_group_iterator
	CvPlotGroup* firstPlotGroup(int* pIterIdx, bool bRev = false) const;
	// deprecated, use plot_group_iterator
	CvPlotGroup* nextPlotGroup(int* pIterIdx, bool bRev = false) const;

	int getNumPlotGroups() const;
	CvPlotGroup* getPlotGroup(int iID) const;
	CvPlotGroup* addPlotGroup();
	void deletePlotGroup(int iID);

	// city iteration
	DECLARE_INDEX_ITERATOR(const CvPlayer, CvCity, city_iterator, firstCity, nextCity);
	city_iterator beginCities() const { return city_iterator(this); }
	city_iterator endCities() const { return city_iterator(); }
	typedef bst::iterator_range<city_iterator> city_range;
	city_range cities() const { return city_range(beginCities(), endCities()); }

	safe_city_iterator beginCitiesSafe() const { return safe_city_iterator(beginCities(), endCities()); }
	safe_city_iterator endCitiesSafe() const { return safe_city_iterator(); }
	typedef bst::iterator_range<safe_city_iterator> safe_city_range;
	safe_city_range cities_safe() const { return safe_city_range(beginCitiesSafe(), endCitiesSafe()); }

	// deprecated, use city_iterator
	CvCity* firstCity(int* pIterIdx, bool bRev = false) const;
	// deprecated, use city_iterator
	CvCity* nextCity(int* pIterIdx, bool bRev = false) const;

	DllExport CvCity* firstCityExternal(int* pIterIdx, bool bRev = false) const;
	DllExport CvCity* nextCityExternal(int* pIterIdx, bool bRev = false) const;
	DllExport int getNumCities() const;
	DllExport CvCity* getCity(int iID) const;
	CvCity* addCity();
	void deleteCity(int iID);

	DECLARE_INDEX_ITERATOR(const CvPlayer, CvUnit, unit_iterator, firstUnit, nextUnit);

	unit_iterator beginUnits() const { return unit_iterator(this); }
	unit_iterator endUnits() const { return unit_iterator(); }
	typedef bst::iterator_range<unit_iterator> unit_range;
	unit_range units() const { return unit_range(beginUnits(), endUnits()); }

	safe_unit_iterator beginUnitsSafe() const { return safe_unit_iterator(beginUnits(), endUnits()); }
	safe_unit_iterator endUnitsSafe() const { return safe_unit_iterator(); }
	typedef bst::iterator_range<safe_unit_iterator> safe_unit_range;
	safe_unit_range units_safe() const { return safe_unit_range(beginUnitsSafe(), endUnitsSafe()); }

	// deprecated, use unit_range
	CvUnit* firstUnit(int* pIterIdx, bool bRev = false) const;
	// deprecated, use unit_range
	CvUnit* nextUnit(int* pIterIdx, bool bRev = false) const;
	DllExport CvUnit* firstUnitExternal(int* pIterIdx, bool bRev = false) const;
	DllExport CvUnit* nextUnitExternal(int* pIterIdx, bool bRev = false) const;
	DllExport int getNumUnits() const;
	CvUnit* getUnit(int iID) const;
	CvUnit* addUnit();
	void deleteUnit(int iID);

#ifdef CVARMY_BREAKSAVE
	CvArmy* getArmy(int iArmyID) const;

	void deleteArmy(int iArmyID);

#endif

	// selection groups iteration
	DECLARE_INDEX_ITERATOR(const CvPlayer, CvSelectionGroup, group_iterator, firstSelectionGroup, nextSelectionGroup);
	group_iterator beginGroups() const { return group_iterator(this); }
	group_iterator endGroups() const { return group_iterator(); }
	typedef bst::iterator_range<group_iterator> group_range;
	group_range groups() const { return group_range(beginGroups(), endGroups()); }

	// non-empty selection groups iteration
	DECLARE_INDEX_ITERATOR(const CvPlayer, CvSelectionGroup, group_non_empty_iterator, firstSelectionGroupNonEmpty, nextSelectionGroupNonEmpty);
	group_non_empty_iterator beginGroupsNonEmpty() const { return group_non_empty_iterator(this); }
	group_non_empty_iterator endGroupsNonEmpty() const { return group_non_empty_iterator(); }
	typedef bst::iterator_range<group_non_empty_iterator> group_non_empty_range;
	group_non_empty_range groups_non_empty() const { return group_non_empty_range(beginGroupsNonEmpty(), endGroupsNonEmpty()); }

	// deprecated, use group_range
	CvSelectionGroup* firstSelectionGroup(int* pIterIdx, bool bRev = false) const;
	// deprecated, use group_range
	CvSelectionGroup* nextSelectionGroup(int* pIterIdx, bool bRev = false) const;
	CvSelectionGroup* firstSelectionGroupNonEmpty(int* pIterIdx, bool bRev = false) const;
	CvSelectionGroup* nextSelectionGroupNonEmpty(int* pIterIdx, bool bRev = false) const;

	int getNumSelectionGroups() const;

	CvSelectionGroup* getSelectionGroup(int iID) const;
	CvSelectionGroup* addSelectionGroup();
	void deleteSelectionGroup(int iID);

	// triggered events iteration
	DECLARE_INDEX_ITERATOR(const CvPlayer, EventTriggeredData, event_iterator, firstEventTriggered, nextEventTriggered);
	event_iterator beginEvents() const { return event_iterator(this); }
	event_iterator endEvents() const { return event_iterator(); }
	typedef bst::iterator_range<event_iterator> event_range;
	event_range events() const { return event_range(beginEvents(), endEvents()); }

	// deprecated, use event_iterator
	EventTriggeredData* firstEventTriggered(int* pIterIdx, bool bRev = false) const;
	// deprecated, use event_iterator
	EventTriggeredData* nextEventTriggered(int* pIterIdx, bool bRev = false) const;

	int getNumEventsTriggered() const;
	EventTriggeredData* getEventTriggered(int iID) const;
	EventTriggeredData* addEventTriggered();
	void deleteEventTriggered(int iID);
	EventTriggeredData* initTriggeredData(EventTriggerTypes eEventTrigger, bool bFire = false, int iCityId = -1, int iPlotX = INVALID_PLOT_COORD, int iPlotY = INVALID_PLOT_COORD, PlayerTypes eOtherPlayer = NO_PLAYER, int iOtherPlayerCityId = -1, ReligionTypes eReligion = NO_RELIGION, CorporationTypes eCorporation = NO_CORPORATION, int iUnitId = -1, BuildingTypes eBuilding = NO_BUILDING);
	int getEventTriggerWeight(EventTriggerTypes eTrigger) const;
	bool isEventTriggerPossible(EventTriggerTypes eTrigger, bool bIgnoreActive = false) const;

	DllExport void addMessage(const CvTalkingHeadMessage& message);
	void showMissedMessages();
	void clearMessages();
	DllExport const CvMessageQueue& getGameMessages() const;
	void expireMessages();
	DllExport void addPopup(CvPopupInfo* pInfo, bool bFront = false);
	void clearPopups();
	DllExport CvPopupInfo* popFrontPopup();
	DllExport const CvPopupQueue& getPopups() const;
	DllExport void addDiplomacy(CvDiploParameters* pDiplo);
	void clearDiplomacy();
	DllExport const CvDiploQueue& getDiplomacy() const;
	DllExport CvDiploParameters* popFrontDiplomacy();
	DllExport void showSpaceShip();
	DllExport void clearSpaceShipPopups();

	int64_t getScoreHistory(int iTurn) const;
	void updateScoreHistory(int iTurn, int64_t iBestScore);

	int64_t getEconomyHistory(int iTurn) const;
	int64_t getIndustryHistory(int iTurn) const;
	int64_t getAgricultureHistory(int iTurn) const;
	int64_t getPowerHistory(int iTurn) const;
	int64_t getCultureHistory(int iTurn) const;
	int64_t getEspionageHistory(int iTurn) const;
	int64_t getRevolutionStabilityHistory(int iTurn) const;

	// Script data needs to be a narrow string for pickling in Python
	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);

	DllExport const CvString getPbemEmailAddress() const;
	DllExport void setPbemEmailAddress(const char* szAddress);
	DllExport const CvString getSmtpHost() const;
	void setSmtpHost(const char* szHost);

	const EventTriggeredData* getEventOccured(EventTypes eEvent, bool bIncludeExpiredEvents = false) const;
	bool isTriggerFired(EventTriggerTypes eEventTrigger) const;
	void setEventOccured(EventTypes eEvent, const EventTriggeredData& kEventTriggered, bool bOthers = true);
	void resetEventOccured(EventTypes eEvent, bool bAnnounce = true);
	void setTriggerFired(const EventTriggeredData& kTriggeredData, bool bOthers = true, bool bAnnounce = true);
	void resetTriggerFired(EventTriggerTypes eEventTrigger);
	void trigger(EventTriggerTypes eEventTrigger);
	void trigger(const EventTriggeredData& kData);
	void applyEvent(EventTypes eEvent, int iTriggeredId, bool bUpdateTrigger = true);
	bool canDoEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const;
	TechTypes getBestEventTech(EventTypes eEvent, PlayerTypes eOtherPlayer) const;
	int getEventCost(EventTypes eEvent, PlayerTypes eOtherPlayer, bool bRandom) const;
	bool canTrigger(EventTriggerTypes eTrigger, PlayerTypes ePlayer, ReligionTypes eReligion) const;
	const EventTriggeredData* getEventCountdown(EventTypes eEvent) const;
	void setEventCountdown(EventTypes eEvent, const EventTriggeredData& kEventTriggered);
	void resetEventCountdown(EventTypes eEvent);

	bool isFreePromotion(UnitCombatTypes eUnitCombat, PromotionTypes ePromotion) const;
	void setFreePromotion(UnitCombatTypes eUnitCombat, PromotionTypes ePromotion, bool bFree);
	bool isFreePromotion(UnitTypes eUnit, PromotionTypes ePromotion) const;
	void setFreePromotion(UnitTypes eUnit, PromotionTypes ePromotion, bool bFree);

	PlayerVoteTypes getVote(int iId) const;
	void setVote(int iId, PlayerVoteTypes ePlayerVote);

	int getUnitExtraCost(UnitTypes eUnit) const;
	void setUnitExtraCost(UnitTypes eUnit, int iCost);

	bool splitEmpire(int iAreaId);
	bool canSplitEmpire() const;
	bool canSplitArea(int iAreaId) const;
	PlayerTypes getSplitEmpirePlayer(int iAreaId) const;
	bool getSplitEmpireLeaders(CivLeaderArray& aLeaders) const;
	bool assimilatePlayer(PlayerTypes ePlayer);

	void launch(VictoryTypes victoryType);

	bool hasShrine(ReligionTypes eReligion) const;
	int getVotes(VoteTypes eVote, VoteSourceTypes eVoteSource) const;
	void processVoteSourceBonus(VoteSourceTypes eVoteSource, bool bActive);
	bool canDoResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData) const;
	bool canDefyResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData) const;
	void setDefiedResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData);
	void setEndorsedResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData);
	bool isFullMember(VoteSourceTypes eVoteSource) const;
	bool isVotingMember(VoteSourceTypes eVoteSource) const;

	void invalidatePopulationRankCache();
	void invalidateYieldRankCache(YieldTypes eYield = NO_YIELD);
	void invalidateCommerceRankCache(CommerceTypes eCommerce = NO_COMMERCE);

	PlayerTypes pickConqueredCityOwner(const CvCity& kCity) const;
	bool canHaveTradeRoutesWith(PlayerTypes ePlayer) const;

	void forcePeace(PlayerTypes ePlayer);

	bool canSpiesEnterBorders(PlayerTypes ePlayer) const;
	int getNewCityProductionValue() const;

	int getGrowthThreshold(int iPopulation) const;
	int getPopulationgrowthratepercentage() const;
	void setPopulationgrowthratepercentage(int iNewValue);
	void changePopulationgrowthratepercentage(int iChange, bool bAdd);

	int getWorldHappiness() const;
	void changeWorldHappiness(int iChange);

	int getWorldHealth() const;
	void changeWorldHealth(int iChange);

	int getProjectHappiness() const;
	void changeProjectHappiness(int iChange);

	int getForceAllTradeRoutes() const;
	void changeForceAllTradeRoutes(int iChange);

	int getProjectHealth() const;
	void changeProjectHealth(int iChange);

	inline bool isNoCapitalUnhappiness() const { return m_iNoCapitalUnhappiness > 0; }
	void changeNoCapitalUnhappiness(int iChange);

	int getCivilizationHealth() const;
	void changeCivilizationHealth(int iChange);

	int getTaxRateUnhappiness() const;
	void changeTaxRateUnhappiness(int iChange);
	int calculateTaxRateUnhappiness() const;

	int getReligionSpreadRate() const;
	void changeReligionSpreadRate(int iChange);

	int getDistantUnitSupportCostModifier() const;
	void setDistantUnitSupportCostModifier(int iNewValue);
	void changeDistantUnitSupportCostModifier(int iChange);

	int getExtraCityDefense() const;
	void setExtraCityDefense(int iNewValue);
	void changeExtraCityDefense(int iChange);

	int getBuildingInflation() const;
	void changeBuildingInflation(int iChange);

	int getProjectInflation() const;
	void changeProjectInflation(int iChange);

	int getTechInflation() const;
	void changeTechInflation(int iChange);

	int getCivicInflation() const;
	void changeCivicInflation(int iChange);

	int getHurryCostModifier() const;
	void changeHurryCostModifier(int iChange);

	int getHurryInflationModifier() const;
	void changeHurryInflationModifier(int iChange);

	int getCityLimit() const { return m_iCityLimit; }
	void changeCityLimit(int iChange);

	int getCityOverLimitUnhappy() const;
	void changeCityOverLimitUnhappy(int iChange);

	int getForeignUnhappyPercent() const;
	void changeForeignUnhappyPercent(int iChange);

	bool isShowLandmarks() const;
	void setShowLandmarks(bool bNewVal);

	int getTerrainYieldChange(TerrainTypes eIndex1, YieldTypes eIndex2) const;
	void changeTerrainYieldChange(TerrainTypes eIndex1, YieldTypes eIndex2, int iChange);

	bool hasValidCivics(BuildingTypes eBuilding) const;

	int getSevoWondersScore(int mode);

	int getForeignTradeRouteModifier() const;
	void changeForeignTradeRouteModifier(int iChange);
	int getCivicHappiness() const;
	void changeCivicHappiness(int iChange);

	bool hasEnemyDefenderUnit(const CvPlot* pPlot) const;

	CvCity* getBestHQCity(CorporationTypes eCorporation) const;
	PlayerVoteTypes getPledgedVote() const;
	void setPledgedVote(PlayerVoteTypes eIndex);
	TeamTypes getPledgedSecretaryGeneralVote() const;
	void setPledgedSecretaryGeneralVote(TeamTypes eIndex);

	int getUnitCombatFreeExperience(const UnitCombatTypes eIndex) const;
	void changeUnitCombatFreeExperience(const UnitCombatTypes eIndex, const int iChange);

	int getBuildingProductionModifier(const BuildingTypes eIndex) const;
	void changeBuildingProductionModifier(const BuildingTypes eIndex, const int iChange);
	int getBuildingCostModifier(const BuildingTypes eIndex) const;
	void changeBuildingCostModifier(const BuildingTypes eIndex, const int iChange);

	int getUnitProductionModifier(const UnitTypes eUnit) const;
	void changeUnitProductionModifier(const UnitTypes eUnit, const int iChange);
	int getUnitCombatProductionModifier(const UnitCombatTypes eIndex) const;
	void changeUnitCombatProductionModifier(const UnitCombatTypes eIndex, const int iChange);

	bool isAutomatedCanBuild(BuildTypes eBuild) const;
	void setAutomatedCanBuild(BuildTypes eBuild, bool bNewValue);

	int getNumTradeImportsByBonus(PlayerTypes ePlayer, BonusTypes eBonus) const;
	bool isTradingMilitaryBonus(PlayerTypes ePlayer) const;

	DenialTypes AI_workerTrade(const CvUnit* pUnit, PlayerTypes ePlayer) const;

	DenialTypes AI_militaryUnitTrade(const CvUnit* pUnit, PlayerTypes ePlayer) const;
	DenialTypes AI_corporationTrade(CorporationTypes eCorporation, PlayerTypes ePlayer) const;
	DenialTypes AI_pledgeVoteTrade(VoteTriggeredData* kData, PlayerVoteTypes ePlayerVote, PlayerTypes ePlayer) const;
	DenialTypes AI_secretaryGeneralTrade(VoteSourceTypes eVoteSource, PlayerTypes ePlayer) const;
	int getEnslavementChance() const;
	void changeEnslavementChance(int iChange);

	int doMultipleResearch(int iOverflow);

	void acquireFort(CvPlot* pPlot);

	int getResourceConsumption(BonusTypes eBonus) const;
	void recalculateResourceConsumption(BonusTypes eBonus);
	void recalculateAllResourceConsumption();

	void listCommander(bool bAdd, CvUnit* unit);
	std::vector<CvUnit*> getCommanders() const { return m_commanders; }

	void listCommodore(bool bAdd, CvUnit* unit);
    std::vector<CvUnit*> getCommodores() const { return m_commodores; }

	void setCommandFieldPlot(bool bNewValue, CvPlot* aPlot);
	std::vector<CvPlot*> getCommandFieldPlots() const { return m_commandFieldPlots; }

	void setCommodoreFieldPlot(bool bNewValue, CvPlot* aPlot);
	std::vector<CvPlot*> getCommodoreFieldPlots() const { return m_commodoreFieldPlots; }

	int getFreeSpecialistCount(SpecialistTypes eIndex) const;
	void setFreeSpecialistCount(SpecialistTypes eIndex, int iNewValue);
	void changeFreeSpecialistCount(SpecialistTypes eIndex, int iChange);

	bool hasValidBuildings(TechTypes eTech) const;

	void checkAIStrategy();

	void setCityCommerceModifierDirty(CommerceTypes eCommerce);

	int getBuildingCommerceModifier(BuildingTypes eBonus, CommerceTypes eIndex) const;
	void changeBuildingCommerceModifier(BuildingTypes eBonus, CommerceTypes eIndex, int iChange);

	int getBuildingCommerceChange(BuildingTypes building, CommerceTypes CommerceType) const;
	void changeBuildingCommerceChange(BuildingTypes building, CommerceTypes CommerceType, int iChange);

	int getBonusCommerceModifier(BonusTypes eBonus, CommerceTypes eIndex) const;
	void changeBonusCommerceModifier(BonusTypes eBonus, CommerceTypes eIndex, int iChange);

	int getLandmarkYield(YieldTypes eIndex) const;
	void changeLandmarkYield(YieldTypes eIndex, int iChange);

	int getNoLandmarkAngerCount() const;
	bool isNoLandmarkAnger() const;
	void changeNoLandmarkAngerCount(int iChange);

	int getLandmarkHappiness() const;
	void changeLandmarkHappiness(int iChange);

	int getBuildingCountWithUpgrades(BuildingTypes eBuilding) const;

	void setColor(PlayerColorTypes eColor);

	void setHandicap(int iNewVal, bool bAdjustGameHandicap = false);

	bool canBuild(const CvPlot* pPlot, ImprovementTypes eImprovement, bool bTestVisible) const;

	int getModderOption(ModderOptionTypes eIndex) const;
	bool isModderOption(ModderOptionTypes eIndex) const;
	void setModderOption(ModderOptionTypes eIndex, bool bNewValue);
	void setModderOption(ModderOptionTypes eIndex, int iNewValue);

	int getCorporationSpreadModifier() const;
	void changeCorporationSpreadModifier(int iChange);

	int64_t getCorporateMaintenance() const;
	void updateCorporateMaintenance();

	int getCorporationInfluence(CorporationTypes eIndex) const;
	int getEnvironmentalProtection() const;
	int getLaborFreedom() const;


	bool m_bChoosingReligion;
	bool m_bHasLanguage;

	int getBuildingCount(BuildingTypes eBuilding, bool bUpgrades) const;

	int getScoreComponent(int iRawScore, int iInitial, int iMax, int iFactor, bool bExponential, bool bFinal, bool bVictory) const;

	int getHurriedCount() const;
	void changeHurriedCount(int iChange);
	void doAdvancedEconomy();

	int getWonderConstructRand() const;

	int getSpecialistCommercePercentChanges(SpecialistTypes eIndex1, CommerceTypes eIndex2) const;
	void changeSpecialistCommercePercentChanges(SpecialistTypes eIndex1, CommerceTypes eIndex2, int iChange);

	int getSpecialistYieldPercentChanges(SpecialistTypes eIndex1, YieldTypes eIndex2) const;
	void changeSpecialistYieldPercentChanges(SpecialistTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getFractionalCombatExperience() const;
	void changeFractionalCombatExperience(int iChange, UnitTypes eGGType = NO_UNIT);

	void clearTileCulture();
	void clearCityCulture();

	int getBonusMintedPercent(const BonusTypes eBonus) const;
	void changeBonusMintedPercent(const BonusTypes eBonus, const int iChange);

	//	Moved from unit to player to allow for caching
	bool upgradeAvailable(UnitTypes eFromUnit, UnitTypes eToUnit) const;

	// Building list for filtering, grouping and sorting
	void setBuildingListInvalid();
	bool getBuildingListFilterActive(BuildingFilterTypes eFilter);
	void setBuildingListFilterActive(BuildingFilterTypes eFilter, bool bActive);
	BuildingGroupingTypes getBuildingListGrouping();
	void setBuildingListGrouping(BuildingGroupingTypes eGrouping);
	BuildingSortTypes getBuildingListSorting();
	void setBuildingListSorting(BuildingSortTypes eSorting);
	int getBuildingListGroupNum();
	int getBuildingListNumInGroup(int iGroup);
	BuildingTypes getBuildingListType(int iGroup, int iPos);
	int getBuildingListSelectedBuildingRow();
	int getBuildingListSelectedWonderRow();
	void setBuildingListSelectedBuilding(BuildingTypes eBuilding);
	void setBuildingListSelectedWonder(BuildingTypes eWonder);
	BuildingTypes getBuildingListSelectedBuilding();
	BuildingTypes getBuildingListSelectedWonder();

	// Unit list for filtering, grouping and sorting
	void setUnitListInvalid();
	bool getUnitListFilterActive(UnitFilterTypes eFilter);
	void setUnitListFilterActive(UnitFilterTypes eFilter, bool bActive);
	UnitGroupingTypes getUnitListGrouping();
	void setUnitListGrouping(UnitGroupingTypes eGrouping);
	UnitSortTypes getUnitListSorting();
	void setUnitListSorting(UnitSortTypes eSorting);
	int getUnitListGroupNum();
	int getUnitListNumInGroup(int iGroup);
	UnitTypes getUnitListType(int iGroup, int iPos);
	int getUnitListSelectedRow();
	void setUnitListSelected(UnitTypes eUnit);
	UnitTypes getUnitListSelected();
	void processNewRoutes();
	inline int getZobristValue() const { return m_zobristValue; }

	inline bool getTurnHadUIInteraction() const { return m_turnHadUIInteraction; }
	inline void setTurnHadUIInteraction(bool newVal) { m_turnHadUIInteraction = newVal; }

protected:
	int** m_ppiSpecialistYieldPercentChanges;
	int** m_ppiSpecialistCommercePercentChanges;
	int** m_ppaaiTerrainYieldChange;
	int** m_ppiBuildingCommerceModifier;
	int** m_ppiBuildingCommerceChange;
	int** m_ppiBonusCommerceModifier;
	int* m_paiPlayerWideAfflictionCount;
	bool* m_pabAutomatedCanBuild;
	int* m_paiResourceConsumption;
	int* m_paiFreeSpecialistCount;
	int* m_aiLandmarkYield;
	int* m_aiModderOptions;
	int m_iEnslavementChance;
	PlayerVoteTypes m_ePledgedVote;
	TeamTypes m_eSecretaryGeneralVote;
	UnitTypes m_eGreatGeneralTypetoAssign;
	int m_iForeignTradeRouteModifier;
	int m_iCivicHappiness;
	int m_iNoLandmarkAngerCount;
	int m_iLandmarkHappiness;
	int m_iExtraCityDefense;
	int m_iDistantUnitSupportCostModifier;
	int m_iReligionSpreadRate;
	int m_iTaxRateUnhappiness;
	int m_iCivilizationHealth;
	int m_iNoCapitalUnhappiness;
	int m_iProjectHealth;
	int m_iForceAllTradeRoutes;
	int m_iProjectHappiness;
	int m_iWorldHealth;
	int m_iWorldHappiness;
	float m_fPopulationgrowthratepercentageLog;
	int m_iCorporationSpreadModifier;
	// @SAVEBREAK - delete
	int m_iCorporateTaxIncome;
	// !SAVEBREAK
	int64_t m_iCorporateMaintenance;
	bool m_bShowLandmarks;
	int m_iCityLimit;
	int m_iCityOverLimitUnhappy;
	int m_iForeignUnhappyPercent;
	//TB Traits
	int* m_paiImprovementUpgradeRateModifierSpecific;
	int* m_paiBuildWorkerSpeedModifierSpecific;
	int m_iTraitExtraCityDefense;
	bool* m_pabHasTrait;
	int m_iLeaderHeadLevel;
	int m_iNationalEspionageDefense;
	int m_iInquisitionCount;
	int m_iCompatCheckCount;
	int m_iMaxTradeRoutesAdjustment;
	int m_iNationalHurryAngerModifier;
	int m_iNationalEnemyWarWearinessModifier;
	int m_iNationalBombardDefenseModifier;
	int* m_paiNationalDomainFreeExperience;
	int* m_paiNationalDomainProductionModifier;
	int* m_paiNationalTechResearchModifier;
	int m_iFixedBordersCount;
	int m_iFreedomFighterCount;
	int m_iExtraFreedomFighters;

	int* m_paiEraAdvanceFreeSpecialistCount;
	int m_iNationalCityStartCulture;
	int m_iNationalAirUnitCapacity;
	int m_iCapitalXPModifier;
	int m_iStateReligionHolyCityXPModifier;
	int m_iNonStateReligionHolyCityXPModifier;
	int m_iNationalCityStartBonusPopulation;
	int m_iNationalMissileRangeChange;
	int m_iNationalFlightOperationRangeChange;
	int m_iNationalNavalCargoSpaceChange;
	int m_iNationalMissileCargoSpaceChange;
	int m_iCitiesStartwithStateReligionCount;
	int m_iDraftsOnCityCaptureCount;
	int m_iFreeSpecialistperWorldWonderCount;
	int m_iFreeSpecialistperNationalWonderCount;
	int m_iFreeSpecialistperTeamProjectCount;
	int m_iExtraGoodyCount;

	int m_iAllReligionsActiveCount;

	int m_iExtraNationalCaptureProbabilityModifier;
	int m_iExtraNationalCaptureResistanceModifier;

	int m_iExtraStateReligionSpreadModifier;
	int m_iExtraNonStateReligionSpreadModifier;
	//TB Traits
	int m_iBaseMergeSelection;
	int m_iFirstMergeSelection;
	int m_iSecondMergeSelection;
	int m_iSplittingUnit;
	int m_iAmbushingUnit;
	int m_iAmbushingTargetUnit;
	bool m_bAssassinate;

	int m_iBuildingInflation;
	int m_iProjectInflation;
	int m_iTechInflation;
	int m_iCivicInflation;
	int m_iHurryCostModifier;
	int m_iHurryInflationModifier;
	int m_iHurryCount;

	int m_iFractionalCombatExperience;

	TeamTypes m_eDemandWarAgainstTeam;

	CvProperties m_Properties;
	CvBuildingList m_BuildingList;
	CvUnitList m_UnitList;

	int	m_zobristValue;

	bool m_turnHadUIInteraction;


public:
	void verifyUnitStacksValid();
	UnitTypes getTechFreeProphet(TechTypes eTech) const;

	// BUG - Trade Totals - start
	void calculateTradeTotals(YieldTypes eIndex, int& iDomesticYield, int& iDomesticRoutes, int& iForeignYield, int& iForeignRoutes, PlayerTypes eWithPlayer = NO_PLAYER, bool bRound = false, bool bBase = false) const;
	int calculateTotalTradeYield(YieldTypes eIndex, PlayerTypes eWithPlayer = NO_PLAYER, bool bRound = false, bool bBase = false) const;
	// BUG - Trade Totals - end

	DllExport void buildTradeTable(PlayerTypes eOtherPlayer, CLinkList<TradeData>& ourList) const;
	DllExport bool getHeadingTradeString(PlayerTypes eOtherPlayer, TradeableItems eItem, CvWString& szString, CvString& szIcon) const;
	DllExport bool getItemTradeString(PlayerTypes eOtherPlayer, bool bOffer, bool bShowingCurrent, const TradeData& zTradeData, CvWString& szString, CvString& szIcon) const;
	DllExport void updateTradeList(PlayerTypes eOtherPlayer, CLinkList<TradeData>& ourInventory, const CLinkList<TradeData>& ourOffer, const CLinkList<TradeData>& theirOffer) const;
	DllExport int getIntroMusicScriptId(PlayerTypes eForPlayer) const;
	DllExport int getMusicScriptId(PlayerTypes eForPlayer) const;
	DllExport void getGlobeLayerColors(GlobeLayerTypes eGlobeLayerType, int iOption, std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const;
	DllExport void cheat(bool bCtrl, bool bAlt, bool bShift);

	DllExport const CvArtInfoUnit* getUnitArtInfo(UnitTypes eUnit, int iMeshGroup = 0) const;
	DllExport bool hasSpaceshipArrived() const;

	// BUG - Reminder Mod - start
	void addReminder(int iGameTurn, CvWString szMessage) const;
	// BUG - Reminder Mod - end

	virtual void AI_init() = 0;
	virtual void AI_reset(bool bConstructor) = 0;
	virtual void AI_doTurnPre() = 0;
	virtual void AI_doTurnPost() = 0;
	virtual void AI_doTurnUnitsPre() = 0;
	virtual void AI_doTurnUnitsPost() = 0;
	virtual void AI_updateFoundValues(bool bClear = false, const CvArea* area = NULL) const = 0;
	virtual void AI_unitUpdate() = 0;
	virtual void AI_makeAssignWorkDirty() = 0;
	virtual void AI_assignWorkingPlots() = 0;
	virtual void AI_updateAssignWork() = 0;
	virtual void AI_makeProductionDirty() = 0;

	//virtual void AI_doCentralizedProduction() = 0;

	virtual void AI_conquerCity(PlayerTypes eOldOwner, CvCity* pCity, bool bConquest, bool bTrade) = 0;
	virtual int AI_foundValue(int iX, int iY, int iMinUnitRange = -1, bool bStartingLoc = false) const = 0;
	virtual bool AI_isCommercePlot(const CvPlot* pPlot) const = 0;
	virtual int AI_getPlotDanger(const CvPlot* pPlot, int iRange = -1, bool bTestMoves = true) const = 0;
	virtual bool AI_isFinancialTrouble() const = 0;
	virtual TechTypes AI_bestTech(int iMaxPathLength = 1, bool bIgnoreCost = false, bool bAsync = false, TechTypes eIgnoreTech = NO_TECH, AdvisorTypes eIgnoreAdvisor = NO_ADVISOR) = 0;
	virtual void AI_chooseFreeTech() = 0;
	virtual void AI_chooseResearch() = 0;
	virtual bool AI_isWillingToTalk(PlayerTypes ePlayer) const = 0;
	virtual bool AI_demandRebukedSneak(PlayerTypes ePlayer) const = 0;
	virtual bool AI_demandRebukedWar(PlayerTypes ePlayer) const = 0;
	virtual AttitudeTypes AI_getAttitude(PlayerTypes ePlayer, bool bForced = true) const = 0;
	virtual PlayerVoteTypes AI_diploVote(const VoteSelectionSubData& kVoteData, VoteSourceTypes eVoteSource, bool bPropose) = 0;
	virtual int AI_dealVal(PlayerTypes ePlayer, const CLinkList<TradeData>* pList, bool bIgnoreAnnual = false, int iExtra = 0) const = 0;
	virtual bool AI_considerOffer(PlayerTypes ePlayer, const CLinkList<TradeData>* pTheirList, const CLinkList<TradeData>* pOurList, int iChange = 1) const = 0;
	virtual bool AI_counterPropose(PlayerTypes ePlayer, const CLinkList<TradeData>* pTheirList, const CLinkList<TradeData>* pOurList, CLinkList<TradeData>* pTheirInventory, CLinkList<TradeData>* pOurInventory, CLinkList<TradeData>* pTheirCounter, CLinkList<TradeData>* pOurCounter) const = 0;
	virtual int AI_bonusVal(BonusTypes eBonus, int iChange = 0, bool bForTrade = false) const = 0;
	virtual int AI_bonusTradeVal(BonusTypes eBonus, PlayerTypes ePlayer, int iChange = 0) const = 0;
	virtual DenialTypes AI_bonusTrade(BonusTypes eBonus, PlayerTypes ePlayer) const = 0;
	virtual int AI_cityTradeVal(CvCity* pCity) const = 0;
	virtual DenialTypes AI_cityTrade(CvCity* pCity, PlayerTypes ePlayer) const = 0;
	virtual DenialTypes AI_stopTradingTrade(TeamTypes eTradeTeam, PlayerTypes ePlayer) const = 0;
	virtual DenialTypes AI_civicTrade(CivicTypes eCivic, PlayerTypes ePlayer) const = 0;
	virtual DenialTypes AI_religionTrade(ReligionTypes eReligion, PlayerTypes ePlayer) const = 0;

	virtual int AI_unitValue(UnitTypes eUnit, UnitAITypes eUnitAI, const CvArea* pArea, const CvUnitSelectionCriteria* criteria = NULL) const = 0;
	virtual int AI_totalUnitAIs(UnitAITypes eUnitAI) const = 0;
	virtual int AI_totalAreaUnitAIs(const CvArea* pArea, UnitAITypes eUnitAI) const = 0;
	virtual int AI_totalWaterAreaUnitAIs(const CvArea* pArea, UnitAITypes eUnitAI) const = 0;
	virtual int AI_plotTargetMissionAIs(const CvPlot* pPlot, MissionAITypes eMissionAI, const CvSelectionGroup* pSkipSelectionGroup = NULL, int iRange = 0, int* piClosest = NULL) const = 0;
	virtual int AI_unitTargetMissionAIs(const CvUnit* pUnit, MissionAITypes eMissionAI, const CvSelectionGroup* pSkipSelectionGroup = NULL) const = 0;

	virtual int AI_civicValue(CivicTypes eCivic, bool bCivicOptionVacuum = false, CivicTypes* paeSelectedCivics = NULL) const = 0;

	virtual int AI_getNumAIUnits(UnitAITypes eIndex) const = 0;
	virtual void AI_changePeacetimeTradeValue(PlayerTypes eIndex, int iChange) = 0;
	virtual void AI_changePeacetimeGrantValue(PlayerTypes eIndex, int iChange) = 0;
	virtual int AI_getAttitudeExtra(const PlayerTypes ePlayer) const = 0;
	virtual void AI_setAttitudeExtra(const PlayerTypes ePlayer, const int iNewValue) = 0;
	virtual void AI_changeAttitudeExtra(const PlayerTypes ePlayer, const int iChange) = 0;
	virtual void AI_setFirstContact(PlayerTypes eIndex, bool bNewValue) = 0;
	virtual int AI_getMemoryCount(PlayerTypes eIndex1, MemoryTypes eIndex2) const = 0;
	virtual void AI_changeMemoryCount(PlayerTypes eIndex1, MemoryTypes eIndex2, int iChange) = 0;
	virtual void AI_doCommerce() = 0;
	virtual EventTypes AI_chooseEvent(int iTriggeredId, int* pValue = NULL) const = 0;
	virtual void AI_launch(VictoryTypes eVictory) = 0;
	virtual void AI_doAdvancedStart(bool bNoExit = false) = 0;
	virtual void AI_updateBonusValue() = 0;
	virtual void AI_updateBonusValue(BonusTypes eBonus) = 0;
	virtual ReligionTypes AI_chooseReligion() = 0;
	virtual int AI_getExtraGoldTarget() const = 0;
	virtual void AI_setExtraGoldTarget(int iNewValue) = 0;
	virtual int AI_maxGoldPerTurnTrade(PlayerTypes ePlayer) const = 0;
	virtual int AI_maxGoldTrade(PlayerTypes ePlayer) const = 0;

protected:
	bst::array<XYCoords, NUM_MAPS> m_startingCoords;
	int m_iTotalPopulation;
	int m_iTotalLand;
	int m_iTotalLandScored;
	int64_t m_iGold;
	int m_iGoldPerTurn;
	int m_iAdvancedStartPoints;
	int m_iGoldenAgeTurns;
	int m_iNumUnitGoldenAges;
	int m_iStrikeTurns;
	int m_iAnarchyTurns;
	int m_iMaxAnarchyTurns;
	int m_iMinAnarchyTurns;
	int m_iAnarchyModifier;
	int m_iGoldenAgeModifier;
	int m_iGlobalHurryModifier;
	int m_iGreatPeopleCreated;
	int m_iGreatGeneralsCreated;
	int m_iGreatPeopleThresholdModifier;
	int m_iGreatGeneralsThresholdModifier;
	int m_iGreatPeopleRateModifier;
	int m_iGreatGeneralRateModifier;
	int m_iDomesticGreatGeneralRateModifier;
	int m_iStateReligionGreatPeopleRateModifier;
	int m_iMaxGlobalBuildingProductionModifier;
	int m_iMaxTeamBuildingProductionModifier;
	int m_iMaxPlayerBuildingProductionModifier;
	int m_iFreeExperience;
	int m_iFeatureProductionModifier;
	int m_iWorkerSpeedModifier;
	int m_iImprovementUpgradeRateModifier;
	int m_iMilitaryProductionModifier;
	int m_iSpaceProductionModifier;
	int m_iCityDefenseModifier;

	int m_iNonStateReligionCommerceCount;
	int m_iUpgradeAnywhereCount;
	int m_iRevIdxLocal;
	int m_iRevIdxNational;
	int m_iRevIdxDistanceModifier;
	int m_iRevIdxHolyCityGood;
	int m_iRevIdxHolyCityBad;
	float m_fRevIdxNationalityMod;
	float m_fRevIdxBadReligionMod;
	float m_fRevIdxGoodReligionMod;
	bool m_bInquisitionConditions;

	int m_iUnitUpgradePriceModifier;
	int m_iNationalGreatPeopleRate;

	int m_iNumNukeUnits;
	int m_iNumOutsideUnits;
	int m_iBaseFreeUnitUpkeepCivilian;
	int m_iBaseFreeUnitUpkeepMilitary;
	int m_iFreeUnitUpkeepCivilianPopPercent;
	int m_iFreeUnitUpkeepMilitaryPopPercent;
	int m_iCivilianUnitUpkeepMod;
	int m_iMilitaryUnitUpkeepMod;

	int64_t m_iUnitUpkeepCivilian100;
	int64_t m_iUnitUpkeepMilitary100;
	mutable int64_t m_iFinalUnitUpkeep;
	mutable bool m_bUnitUpkeepDirty;

	int m_iNumMilitaryUnits;
	int m_iHappyPerMilitaryUnit;
	int m_iMilitaryFoodProductionCount;
	int m_iConscriptCount;
	int m_iMaxConscript;
	int m_iHighestUnitLevel;
	int m_iOverflowResearch;
	int m_iNoUnhealthyPopulationCount;
	int m_iExpInBorderModifier;
	int m_iBuildingOnlyHealthyCount;

	int m_iMaintenanceModifier;
	int m_iCoastalDistanceMaintenanceModifier;
	int m_iConnectedCityMaintenanceModifier;
	int m_iDistanceMaintenanceModifier;
	int m_iNumCitiesMaintenanceModifier;
	int m_iCorporationMaintenanceModifier;
	int m_iHomeAreaMaintenanceModifier;
	int m_iOtherAreaMaintenanceModifier;
	mutable int m_iTotalMaintenance;

	int m_iUpkeepModifier;
	int m_iLevelExperienceModifier;
	int m_iExtraHealth;
	int m_iCivicHealth;
	int m_iBuildingGoodHealth;
	int m_iBuildingBadHealth;
	int m_iExtraHappiness;
	int m_iExtraHappinessUnattributed;
	int m_iBuildingHappiness;
	int m_iLargestCityHappiness;
	int m_iWarWearinessPercentAnger;
	int m_iWarWearinessModifier;
	int m_iFreeSpecialist;
	int m_iNoForeignTradeCount;
	int m_iNoCorporationsCount;
	int m_iNoForeignCorporationsCount;
	int m_iCoastalTradeRoutes;
	int m_iTradeRoutes;
	int m_iRevolutionTimer;
	int m_iConversionTimer;
	int m_iStateReligionCount;
	int m_iNoNonStateReligionSpreadCount;
	int m_iStateReligionHappiness;
	int m_iNonStateReligionHappiness;
	int m_iStateReligionUnitProductionModifier;
	int m_iStateReligionBuildingProductionModifier;
	int m_iStateReligionFreeExperience;
	int m_iCapitalCityID;
	int m_iCitiesLost;
	int m_iWinsVsBarbs;
	int m_iAssets;
	int m_iPower;
	int m_iTechPower;
	int	m_iUnitPower;
	int m_iPopulationScore;
	int m_iLandScore;
	int m_iTechScore;
	int m_iWondersScore;
	int m_iCombatExperience;
	int m_iPopRushHurryCount;
	int m_iInflationModifier;

	uint m_uiStartTime; // XXX save these?

	bool m_bAlive;
	bool m_bEverAlive;
	bool m_bTurnActive;
	bool m_bAutoMoves;
	bool m_bEndTurn;
	bool m_bPbemNewTurn;
	bool m_bExtendedGame;
	bool m_bFoundedFirstCity;
	bool m_bStrike;
	//TB Nukefix
	bool m_bNukesValid;
	bool m_bHuman;
	bool m_bDisableHuman; // Set to true to disable isHuman() check
	bool m_bForcedCityCycle;

	int m_iStabilityIndex;
	int m_iStabilityIndexAverage;

	bool m_bRebel;
	int m_iMotherPlayer;

	//TB Traits begin
	int m_iCivicAnarchyModifier;
	int m_iReligiousAnarchyModifier;
	int m_iAIAttitudeModifier;
	int m_iFocusPlotX;
	int m_iFocusPlotY;
	int* m_aiFreeCityYield;
	int* m_aiLessYieldThreshold;

	int* m_aiGoldenAgeYield;
	int* m_aiGoldenAgeCommerce;
	//TB Traits end

	// Used for DynamicCivNames
	CvWString m_szName;
	CvWString m_szCivDesc;
	CvWString m_szCivShort;
	CvWString m_szCivAdj;

	int m_bDoNotBotherStatus;

	int m_iChoosingFreeTech;

	PlayerTypes m_eID;
	LeaderHeadTypes m_ePersonalityType;
	EraTypes m_eCurrentEra;
	ReligionTypes m_eLastStateReligion;
	PlayerTypes m_eParent;
	TeamTypes m_eTeamType;

	int64_t m_iCulture;

	int m_iUpgradeRoundCount;
	int m_iSelectionRegroup;

	int* m_aiSeaPlotYield;
	int* m_aiYieldRateModifier;
	int* m_aiCapitalYieldRateModifier;
	int* m_aiExtraYieldThreshold;
	int* m_aiTradeYieldModifier;
	int* m_aiFreeCityCommerce; // @SAVEBREAK remove as it is unused.
	int* m_extraCommerce;
	int* m_aiCommercePercent;
	int* m_aiCommerceRate;
	bool* m_abCommerceDirty;
	int* m_aiCommerceRateModifier;
	int* m_aiCommerceRateModifierfromEvents;
	int* m_aiCommerceRateModifierfromBuildings;
	int* m_aiCapitalCommerceRateModifier;
	int* m_aiStateReligionBuildingCommerce;
	int* m_aiSpecialistExtraCommerce;
	int* m_aiSpecialistExtraYield;
	int* m_aiCommerceFlexibleCount;
	int* m_aiGoldPerTurnByPlayer;
	int* m_aiEspionageSpendingWeightAgainstTeam;

	bool* m_abFeatAccomplished;
	bool* m_abOptions;

	CvString m_szScriptData;

	int* m_paiImprovementCount;
	int** m_paiExtraBuildingYield;
	int** m_paiExtraBuildingCommerce;
	int* m_paiFeatureHappiness;
	int* m_paiBuildingCount;
	int* m_paiBuildingGroupCount;
	int* m_paiBuildingGroupMaking;
	int* m_paiHurryCount;
	int* m_paiSpecialBuildingNotRequiredCount;
	int* m_paiHasCivicOptionCount;
	int* m_paiNoCivicUpkeepCount;
	int* m_paiHasReligionCount;
	int* m_paiHasCorporationCount;
	int* m_paiUpkeepCount;
	int* m_paiSpecialistValidCount;

	bool* m_pabResearchingTech;
	bool* m_pabLoyalMember;

	std::vector<EventTriggerTypes> m_triggersFired;

	CivicTypes* m_paeCivics;

	int** m_ppaaiSpecialistExtraYield;
	int** m_ppaaiImprovementYieldChange;
	int** m_ppaaiSpecialistExtraCommerce;

	CLinkList<TechTypes> m_researchQueue;

	CLinkList<CvWString> m_cityNames;

	std::vector<CLinkList<int>*>						  m_groupCycles;
	std::vector<FFreeListTrashArray<CvPlotGroup>*>		  m_plotGroups;
	std::vector<FFreeListTrashArray<CvCityAI>*>			  m_cities;
	std::vector<FFreeListTrashArray<CvUnitAI>*>			  m_units;
	std::vector<FFreeListTrashArray<CvSelectionGroupAI>*> m_selectionGroups;

#ifdef CVARMY_BREAKSAVE
	FFreeListTrashArray<CvArmy>							  m_armies;
#endif 

	FFreeListTrashArray<EventTriggeredData> m_eventsTriggered;
	CvEventMap m_mapEventsOccured;
	CvEventMap m_mapEventCountdown;
	UnitCombatPromotionArray m_aFreeUnitCombatPromotions;
	UnitPromotionArray m_aFreeUnitPromotions;

	std::vector< std::pair<int, PlayerVoteTypes> > m_aVote;
	std::vector< std::pair<UnitTypes, int> > m_aUnitExtraCosts;

	CvMessageQueue m_listGameMessages;
	CvPopupQueue m_listPopups;
	CvDiploQueue m_listDiplomacy;

	CvTurnScoreMap m_mapScoreHistory;
	CvTurnScoreMap m_mapEconomyHistory;
	CvTurnScoreMap m_mapIndustryHistory;
	CvTurnScoreMap m_mapAgricultureHistory;
	CvTurnScoreMap m_mapPowerHistory;
	CvTurnScoreMap m_mapCultureHistory;
	CvTurnScoreMap m_mapEspionageHistory;

	CvTurnScoreMap m_mapRevolutionStabilityHistory;


	// KOSHLING - add pre-calculated lists of plots meeting criteria that
	// otherwise get re-calculated many times during unit mission setting
	std::map<int, BonusTypes>	m_guardableResourcePlots;

	// Temp unit which is used to generate paths for hypothetical units.
	// Kept around rather than created each usage to avoid chewing through the ID space.
	CvUnit* m_pTempUnit;
public:
	inline bool isTempUnit(const CvUnit* pUnit) const
	{
		return (pUnit == m_pTempUnit || pUnit->AI_getBirthmark() == UNIT_BIRTHMARK_TEMP_UNIT);
	}
	inline const std::map<int, BonusTypes>& getGuardableResourcePlots() const
	{
		return m_guardableResourcePlots;
	}

protected:
	void doGold();
	void doResearch();
	void doEspionagePoints();
	void doWarnings();
	void doEvents();

	bool checkExpireEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const;
	void expireEvent(EventTypes eEvent, EventTriggeredData& kTriggeredData, bool bFail);
	bool isValidTriggerReligion(const CvEventTriggerInfo& kTrigger, const CvCity* pCity, ReligionTypes eReligion) const;
	bool isValidTriggerCorporation(const CvEventTriggerInfo& kTrigger, const CvCity* pCity, CorporationTypes eCorporation) const;
	CvCity* pickTriggerCity(EventTriggerTypes eTrigger) const;
	CvUnit* pickTriggerUnit(EventTriggerTypes eTrigger, const CvPlot* pPlot, bool bCheckPlot) const;
	bool isValidEventTech(TechTypes eTech, EventTypes eEvent, PlayerTypes eOtherPlayer) const;
	void recalculatePopulationgrowthratepercentage();

	int calculatePlotRouteYieldDifference(const CvPlot* pPlot, const RouteTypes eRoute, YieldTypes eYield) const;
	RouteTypes getBestRouteInternal(const CvPlot* pPlot, bool bConnect, const CvUnit* pBuilder, BuildTypes* eBestRouteBuild = NULL) const;
	bool isRouteValid(RouteTypes eRoute, BuildTypes eRouteBuild, const CvPlot* pPlot, const CvUnit* pBuilder) const;

	void verifyGoldCommercePercent();

	void processCivics(const CivicTypes eCivic, const int iChange, const bool bLimited = false);

	// for serialization
	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);

	void doUpdateCacheOnTurn();
	int getResearchTurnsLeftTimes100(TechTypes eTech, bool bOverflow) const;

	void getTradeLayerColors(std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const; // used by Globeview trade layer
	void getUnitLayerColors(GlobeLayerUnitOptionTypes eOption, std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const; // used by Globeview unit layer
	void getResourceLayerColors(GlobeLayerResourceOptionTypes eOption, std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const; // used by Globeview resource layer
	void getReligionLayerColors(ReligionTypes eSelectedReligion, std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const; // used by Globeview religion layer
	void getCultureLayerColors(std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const; // used by Globeview culture layer

	void processTrait(TraitTypes eTrait, int iChange);
	void recalculateUnitCounts();

	//TB Traits begin
public:
	int getCivicAnarchyModifier() const;
	void setCivicAnarchyModifier(int iNewValue);
	void changeCivicAnarchyModifier(int iChange);

	int getReligiousAnarchyModifier() const;
	void setReligiousAnarchyModifier(int iNewValue);
	void changeReligiousAnarchyModifier(int iChange);

	int getImprovementUpgradeRateModifierSpecific(ImprovementTypes eImprovement) const;
	void changeImprovementUpgradeRateModifierSpecific(ImprovementTypes eImprovement, int iChange);

	int getBuildWorkerSpeedModifierSpecific(BuildTypes eBuild) const;
	void changeBuildWorkerSpeedModifierSpecific(BuildTypes eBuild, int iChange);

	int getAIAttitudeModifier() const;
	void setAIAttitudeModifier(int iNewValue);
	void changeAIAttitudeModifier(int iChange);

	int getExtraSpecialistCommerce(SpecialistTypes eIndex1, CommerceTypes eIndex2) const;
	void changeExtraSpecialistCommerce(SpecialistTypes eIndex1, CommerceTypes eIndex2, int iChange);
	void updateExtraSpecialistCommerce();

	int getSpecialistExtraYield(YieldTypes eIndex) const;
	void changeSpecialistExtraYield(YieldTypes eIndex, int iChange);

	int getFreeCityYield(YieldTypes eIndex) const;
	void changeFreeCityYield(YieldTypes eIndex, int iChange);

	int getTraitExtraCityDefense() const;
	void setTraitExtraCityDefense(int iNewValue);
	void changeTraitExtraCityDefense(int iChange);

	void setHasTrait(TraitTypes eIndex, bool bNewValue);
	bool canLearnTrait(TraitTypes eIndex, bool isSelectingNegative = false) const;
	bool canUnlearnTrait(TraitTypes eTrait, bool bPositive) const;

	int getLeaderHeadLevel() const;
	void setLeaderHeadLevel(int iValue);
	void changeLeaderHeadLevel(int iChange);

	uint64_t getLeaderLevelupNextCultureTotal() const;
	uint64_t getLeaderLevelupCultureToEarn() const;

	bool canLeaderPromote() const;
	void doPromoteLeader();
	void clearLeaderTraits();

	int getNationalEspionageDefense() const;
	void setNationalEspionageDefense(int iNewValue);
	void changeNationalEspionageDefense(int iChange);

	int getInquisitionCount() const;
	void changeInquisitionCount(int iChange);

	int getNationalGreatPeopleRate() const;
	int getNationalGreatPeopleUnitRate(const UnitTypes eIndex) const;
	void changeNationalGreatPeopleUnitRate(const UnitTypes eIndex, const int iChange);

	int getMaxTradeRoutesAdjustment() const;
	void changeMaxTradeRoutesAdjustment(int iChange);

	int getNationalHurryAngerModifier() const;
	void setNationalHurryAngerModifier(int iNewValue);
	void changeNationalHurryAngerModifier(int iChange);

	int getNationalEnemyWarWearinessModifier() const;
	void setNationalEnemyWarWearinessModifier(int iNewValue);
	void changeNationalEnemyWarWearinessModifier(int iChange);

	int getNationalBombardDefenseModifier() const;
	void setNationalBombardDefenseModifier(int iNewValue);
	void changeNationalBombardDefenseModifier(int iChange);

	int getNationalDomainFreeExperience(DomainTypes eIndex) const;
	void setNationalDomainFreeExperience(DomainTypes eIndex, int iNewValue);
	void changeNationalDomainFreeExperience(DomainTypes eIndex, int iChange);

	int getNationalDomainProductionModifier(DomainTypes eIndex) const;
	void setNationalDomainProductionModifier(DomainTypes eIndex, int iNewValue);
	void changeNationalDomainProductionModifier(DomainTypes eIndex, int iChange);

	int getNationalTechResearchModifier(TechTypes eIndex) const;
	void setNationalTechResearchModifier(TechTypes eIndex, int iNewValue);
	void changeNationalTechResearchModifier(TechTypes eIndex, int iChange);

	int getCoastalAIInfluence() const;

	int getEraAdvanceFreeSpecialistCount(SpecialistTypes eIndex) const;
	void setEraAdvanceFreeSpecialistCount(SpecialistTypes eIndex, int iValue);
	void changeEraAdvanceFreeSpecialistCount(SpecialistTypes eIndex, int iChange);

	int getGoldenAgeOnBirthOfGreatPersonCount(const UnitTypes eIndex) const;
	void changeGoldenAgeOnBirthOfGreatPersonCount(const UnitTypes eIndex, const char iChange);

	int getNationalCityStartCulture() const;
	void setNationalCityStartCulture(int iValue);
	void changeNationalCityStartCulture(int iChange);

	int getNationalAirUnitCapacity() const;
	void setNationalAirUnitCapacity(int iValue);
	void changeNationalAirUnitCapacity(int iChange);

	int getCapitalXPModifier() const;
	void setCapitalXPModifier(int iValue);
	void changeCapitalXPModifier(int iChange);

	int getStateReligionHolyCityXPModifier() const;
	void setStateReligionHolyCityXPModifier(int iValue);
	void changeStateReligionHolyCityXPModifier(int iChange);

	int getNonStateReligionHolyCityXPModifier() const;
	void setNonStateReligionHolyCityXPModifier(int iValue);
	void changeNonStateReligionHolyCityXPModifier(int iChange);

	int getNationalCityStartBonusPopulation() const;
	void setNationalCityStartBonusPopulation(int iValue);
	void changeNationalCityStartBonusPopulation(int iChange);

	int getNationalMissileRangeChange() const;
	void setNationalMissileRangeChange(int iValue);
	void changeNationalMissileRangeChange(int iChange);

	int getNationalFlightOperationRangeChange() const;
	void setNationalFlightOperationRangeChange(int iValue);
	void changeNationalFlightOperationRangeChange(int iChange);

	int getNationalNavalCargoSpaceChange() const;
	void setNationalNavalCargoSpaceChange(int iValue);
	void changeNationalNavalCargoSpaceChange(int iChange);

	int getNationalMissileCargoSpaceChange() const;
	void setNationalMissileCargoSpaceChange(int iValue);
	void changeNationalMissileCargoSpaceChange(int iChange);

	bool hasCitiesStartwithStateReligion() const;
	void setCitiesStartwithStateReligionCount(int iValue);
	void changeCitiesStartwithStateReligionCount(int iChange);

	bool hasDraftsOnCityCapture() const;
	void setDraftsOnCityCaptureCount(int iValue);
	void changeDraftsOnCityCaptureCount(int iChange);

	bool hasFreeSpecialistperWorldWonder() const;
	void setFreeSpecialistperWorldWonderCount(int iValue);
	void changeFreeSpecialistperWorldWonderCount(int iChange);

	bool hasFreeSpecialistperNationalWonder() const;
	void setFreeSpecialistperNationalWonderCount(int iValue);
	void changeFreeSpecialistperNationalWonderCount(int iChange);

	bool hasFreeSpecialistperTeamProject() const;
	void setFreeSpecialistperTeamProjectCount(int iValue);
	void changeFreeSpecialistperTeamProjectCount(int iChange);

	bool hasExtraGoody() const;
	void setExtraGoodyCount(int iValue);
	void changeExtraGoodyCount(int iChange);

	bool hasBannedNonStateReligions() const;
	bool hasAllReligionsActive() const;
	int getAllReligionsActiveCount() const;
	void setAllReligionsActiveCount(int iValue);
	void changeAllReligionsActiveCount(int iChange);

	int getExtraNationalCaptureProbabilityModifier() const;
	void setExtraNationalCaptureProbabilityModifier(int iValue);
	void changeExtraNationalCaptureProbabilityModifier(int iChange);

	int getExtraNationalCaptureResistanceModifier() const;
	void setExtraNationalCaptureResistanceModifier(int iValue);
	void changeExtraNationalCaptureResistanceModifier(int iChange);

	int getExtraStateReligionSpreadModifier() const;
	void setExtraStateReligionSpreadModifier(int iValue);
	void changeExtraStateReligionSpreadModifier(int iChange);

	int getExtraNonStateReligionSpreadModifier() const;
	void setExtraNonStateReligionSpreadModifier(int iValue);
	void changeExtraNonStateReligionSpreadModifier(int iChange);

	void checkReligiousDisablingAllBuildings();
	//TB Traits end

	void startDeferredPlotGroupBonusCalculation();
	void endDeferredPlotGroupBonusCalculation();

	bool hasFixedBorders() const;
	void setFixedBordersCount(int iValue);
	void changeFixedBordersCount(int iChange);

	int getBaseMergeSelectionUnit() const;
	void setBaseMergeSelectionUnit(int iNewValue);

	int getFirstMergeSelectionUnit() const;
	void setFirstMergeSelectionUnit(int iNewValue);

	int getSecondMergeSelectionUnit() const;
	void setSecondMergeSelectionUnit(int iNewValue);

	int getSplittingUnit() const;
	void setSplittingUnit(int iNewValue);

	int getAmbushingUnit() const;
	int getAmbushingTargetUnit() const;
	bool isAssassinate() const;
	void setAmbushingUnit(int iNewValue, bool bAssassinate = false);
	void setAmbushingTargetUnit(int iNewValue, bool bAssassinate = true);

	int getGreatGeneralPointsForType(const UnitTypes eUnit) const;
	void setGreatGeneralPointsForType(const UnitTypes eUnit, const int iValue);
	void changeGreatGeneralPointsForType(const UnitTypes eUnit, const int iChange);

	UnitTypes getGreatGeneralTypetoAssign() const;
	void setSMValues();

	void upgradePlotPopup(ImprovementTypes eImprovement, int iX, int iY);
	void upgradePlot(int iX, int iY, ImprovementTypes eImprovement, bool bConfirm);

	void setFocusPlots(int iX, int iY);
	int getFocusPlotX() const;
	int getFocusPlotY() const;

	void RecalculatePlotGroupHashes();
	CvContractBroker& getContractBroker();

	void addPlotDangerSource(const CvPlot* pPlot, int iStrength);

	void clearModifierTotals();
	void recalculateModifiers();

	void addPropertiesAllCities(const CvProperties* pProp);
	void subtractPropertiesAllCities(const CvProperties* pProp);

	bool canHaveBuilder(BuildTypes eBuild) const;
	//TB Nukefix
	bool isNukesValid() const;
	void makeNukesValid(bool bValid = true);

	int getUpgradeRoundCount() const;
	void changeUpgradeRoundCount(int iChange);
	void resetUpgradeRoundCount();

	int getSelectionRegroup() const;
	void setSelectionRegroup(int iGroupID);

	bool hasFreedomFighter() const;
	void changeFreedomFighterCount(int iChange);
	int getExtraFreedomFighters() const;
	void changeExtraFreedomFighters(int iChange);

	CvBuildLists* m_pBuildLists;

private:
	int m_iNumAnimalsSubdued;
	std::map<BuildingTypes, int> m_unitConstructionCounts;
	std::map<short, uint32_t> m_unitCount;
	std::map<short, uint32_t> m_unitCountSM;
	std::map<short, uint32_t> m_unitMaking;
	std::map<short, uint32_t> m_buildingMaking;
	std::map<short, uint16_t> m_freeBuildingCount;
	std::map<short, uint32_t> m_bonusExport;
	std::map<short, uint32_t> m_bonusImport;
	std::map<short, uint32_t> m_greatGeneralPointsType;
	std::map<short, int> m_bonusMintedPercent;
	std::map<short, int> m_extraBuildingHappiness;
	std::map<short, int> m_extraBuildingHealth;
	std::map<short, int> m_buildingProductionMod;
	std::map<short, int> m_buildingCostMod;
	std::map<short, int> m_unitProductionMod;
	std::map<short, int> m_unitCombatProductionMod;
	std::map<short, int> m_greatPeopleRateforUnit;
	std::map<short, char> m_goldenAgeOnBirthOfGreatPersonCount;
	std::map<short, short> m_unitCombatFreeXP;

	int m_iNumAnarchyTurns;
	int m_iNumCivicSwitches;
	int m_iNumCivicsSwitched;
	mutable int* m_aiPathLengthCache;
	mutable int* m_aiCostPathLengthCache;
	mutable bool* m_bCanConstruct;
	mutable bool* m_bCanConstructCached;
	mutable bool* m_bCanConstructDefaultParam;
	mutable bool* m_bCanConstructCachedDefaultParam;
	mutable RouteTypes m_eBestRoute;
	mutable BuildTypes m_eBestRouteBuild;
	mutable std::map<int, bool>	m_canHaveBuilder;

	CvContractBroker m_contractBroker;

	mutable bst::scoped_ptr<CvUpgradeCache> m_upgradeCache;

	bool m_bInhibitPlotGroupRecalc;
	mutable bool m_bMaintenanceDirty;
	mutable bool m_orbitalInfrastructureCountDirty;
	mutable int m_orbitalInfrastructureCount;
	mutable int* m_cachedBonusCount;
	mutable int	 m_cachedBonusCountGameTurn;

	mutable int m_cachedTotalCityBaseCommerceRate[NUM_COMMERCE_TYPES];

	std::vector<civcSwitchInstance> m_civicSwitchHistory;

	static bool m_staticsInitialized;

	int64_t m_iMinTaxIncome;
	int64_t m_iMaxTaxIncome;

protected:
	void constructTechPathSet(TechTypes eTech, std::vector<techPath*>& pathSet, techPath& rootPath) const;
	void clearCanConstructCache(BuildingTypes building, bool bIncludeCities = false) const;
	void clearCanConstructCacheForGroup(SpecialBuildingTypes eSpecialBuilding, bool bIncludeCities = false) const;

public:
#ifdef OUTBREAKS_AND_AFFLICTIONS
	int getPlayerWideAfflictionCount(PromotionLineTypes ePromotionLineType) const;
	void changePlayerWideAfflictionCount(PromotionLineTypes ePromotionLineType, int iChange);
	void setPlayerWideAfflictionCount(PromotionLineTypes ePromotionLineType, int iChange);
	int countAfflictedUnits(PromotionLineTypes eAfflictionLine);
	void recalculateAfflictedUnitCount();
#endif
	virtual void AI_invalidateAttitudeCache(PlayerTypes ePlayer) = 0;
	virtual void AI_setHasInquisitionTarget() = 0;
};

#endif
