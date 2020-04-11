#pragma once

// player.h

#ifndef CIV4_PLAYER_H
#define CIV4_PLAYER_H

#include "CvCityAI.h"
#include "CvUnitAI.h"
#include "CvSelectionGroupAI.h"
#include "CvPlotGroup.h"
#include "LinkedList.h"
#include "CvTalkingHeadMessage.h"
#include "CvContractBroker.h"
#include "CvGameObject.h"
#include "CvBuildLists.h"

class CvDiploParameters;
class CvPopupInfo;
class CvEventTriggerInfo;

#define	UNIT_BIRTHMARK_TEMP_UNIT	20000

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
typedef stdext::hash_map<int, int> CvTurnScoreMap;
typedef stdext::hash_map<EventTypes, EventTriggeredData> CvEventMap;
typedef std::vector< std::pair<UnitCombatTypes, PromotionTypes> > UnitCombatPromotionArray;
typedef std::vector< std::pair<UnitTypes, PromotionTypes> > UnitPromotionArray;
typedef std::vector< std::pair<CivilizationTypes, LeaderHeadTypes> > CivLeaderArray;

//	Forward declaration
class CvUpgradeCache;

typedef std::vector<TechTypes> techPath;

class CvPlayer
{
public:
	CvPlayer();
	virtual ~CvPlayer();

	CvGameObjectPlayer* getGameObject() { return &m_GameObject; };
	const CvGameObjectPlayer* getGameObject() const { return &m_GameObject; };

protected:
	CvGameObjectPlayer m_GameObject;

public:
	// M.A.D. Nukes
	int getMADDeterrent() const;
	void setMADDeterrent(int iValue);
	void changeMADDeterrent(int iValue);
	int getMADIncoming() const;
	void setMADIncoming(int iValue);
	void changeMADIncoming(int iValue);
	int getMADOutgoing() const;
	void setMADOutgoing(int iValue);
	void changeMADOutgoing(int iValue);
	bool getMADTrigger(int iValue) const;
	void setMADTrigger(int iValue, bool bValue);
	int getMADNukesCount() const;
	bool isEnabledMAD() const;
	void changeMADNukesCount(int iChange);
protected:
	int m_iMADDeterrent;
	int m_iMADIncoming;
	int m_iMADOutgoing;
	int m_iMADNukesCount;
	bool m_bMADTrigger[MAX_PLAYERS];

public:


	DllExport void init(PlayerTypes eID);
	DllExport void setupGraphical();
	DllExport void reset(PlayerTypes eID = NO_PLAYER, bool bConstructorCall = false);

	void resetPlotAndCityData();

	void logMsg(char* format, ...);
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

	//protected:
	void uninit();

public:

	void initFreeState();
	void initFreeUnits();

	UnitTypes getBestUnitType(UnitAITypes eUnitAI) const; // Exposed to Python

	int getBestUnitTypeCargoVolume(UnitAITypes eUnitAI) const;
	bool addStartUnitAI(const UnitAITypes eUnitAI, const int iCount);

	int startingPlotRange() const; // Exposed to Python
	bool startingPlotWithinRange(CvPlot* pPlot, PlayerTypes ePlayer, int iRange, int iPass) const; // Exposed to Python
	int startingPlotDistanceFactor(const CvPlot* pPlot, PlayerTypes ePlayer, int iRange) const;
	int findStartingArea() const;
	CvPlot* findStartingPlot(bool bRandomize = false); // Exposed to Python

	CvPlotGroup* initPlotGroup(CvPlot* pPlot, bool bRecalculateBonuses);

	CvCity* initCity(int iX, int iY, bool bBumpUnits, bool bUpdatePlotGroups); // Exposed to Python
	void acquireCity(CvCity* pCity, bool bConquest, bool bTrade, bool bUpdatePlotGroups); // Exposed to Python
	void killCities(); // Exposed to Python
	CvWString getNewCityName() const; // Exposed to Python
	void getCivilizationCityName(CvWString& szBuffer, CivilizationTypes eCivilization) const;
	bool isCityNameValid(CvWString& szName, bool bTestDestroyed = true) const;

	CvUnit* getTempUnit(UnitTypes eUnit, int iX, int iY);
	void releaseTempUnit();
	CvUnit* initUnit(UnitTypes eUnit, int iX, int iY, UnitAITypes eUnitAI, DirectionTypes eFacingDirection, int iBirthmark); // Exposed to Python
	void disbandUnit(bool bAnnounce); // Exposed to Python
	void killUnits(); // Exposed to Python

	CvSelectionGroup* cycleSelectionGroups(const CvUnit* pUnit, bool bForward, bool bWorkers, bool* pbWrap, bool bAllowViewportSwitch);

	bool hasTrait(TraitTypes eTrait) const; // Exposed to Python

	void setHumanDisabled(bool newVal);
	bool isHumanDisabled() const;

	DllExport bool isHuman() const; // Exposed to Python
	DllExport void updateHuman();
	DllExport bool isBarbarian() const; // Exposed to Python
	bool isNPC() const;
	bool isHominid() const;
	bool isAnimal() const;
	bool isInvasionCapablePlayer() const;

	DllExport const wchar* getName(uint uiForm = 0) const; // Exposed to Python

	void setName(std::wstring szNewValue); // Exposed to Python
	void setCivName(std::wstring szNewDesc, std::wstring szNewShort, std::wstring szNewAdj); // Exposed to Python

	DllExport const wchar* getNameKey() const; // Exposed to Python
	DllExport const wchar* getCivilizationDescription(uint uiForm = 0) const; // Exposed to Python
	const wchar* getCivilizationDescriptionKey() const; // Exposed to Python
	const wchar* getCivilizationShortDescription(uint uiForm = 0) const; // Exposed to Python
	const wchar* getCivilizationShortDescriptionKey() const; // Exposed to Python
	const wchar* getCivilizationAdjective(uint uiForm = 0) const; // Exposed to Python
	const wchar* getCivilizationAdjectiveKey() const; // Exposed to Python
	DllExport CvWString getFlagDecal() const; // Exposed to Python
	DllExport bool isWhiteFlag() const; // Exposed to Python
	const wchar* getStateReligionName(uint uiForm = 0) const; // Exposed to Python
	const wchar* getStateReligionKey() const; // Exposed to Python
	const CvWString getBestAttackUnitName(uint uiForm = 0) const; // Exposed to Python
	const CvWString getWorstEnemyName() const; // Exposed to Python
	const wchar* getBestAttackUnitKey() const; // Exposed to Python
	DllExport ArtStyleTypes getArtStyleType() const; // Exposed to Python
	const TCHAR* getUnitButton(UnitTypes eUnit) const; // Exposed to Python

	void doTurn();
	void doTurnUnits();
	void doInflation(bool pReinit);
	int getCurrentInflationCostModifier() const; //	Exposed to Python
	int getEquilibriumInflationCostModifier() const; //	Exposed to Python

	//	Dump stats to BBAI log
	void dumpStats() const;
	void NoteAnimalSubdued();
	void NoteUnitConstructed(BuildingTypes eBuilding);
	void NoteCivicsSwitched(int iNumChanges);

	void verifyCivics();

	void inhibitPlotGroupCalcsUntilFullRebuild(); //	Ignore updates until an update with reInitialize set
	void updatePlotGroups(CvArea* possibleNewInAreaOnly = NULL, bool reInitialize = false);

	void updateYield();
	void updateMaintenance() const;
	inline void setMaintenanceDirty(bool bDirty) const { m_bMaintenanceDirty = bDirty; }
	void updatePowerHealth();

	void updateExtraBuildingHappiness(bool bLimited = false);
	void updateExtraBuildingHealth(bool bLimited = false);
	void updateFeatureHappiness(bool bLimited = false);
	void updateReligionHappiness(bool bLimited = false);

	void updateExtraSpecialistYield();
	void setCommerceDirty(CommerceTypes eIndex = NO_COMMERCE, bool bPlayerOnly = false);
	void updateCommerce(CommerceTypes eCommerce = NO_COMMERCE, bool bForce = true) const;
	void updateBuildingCommerce();
	void updateReligionCommerce();
	void updateCorporation();
	void updateCityPlotYield();
	void updateCitySight(bool bIncrement, bool bUpdatePlotGroups);
	void updateTradeRoutes();
	void updatePlunder(int iChange, bool bUpdatePlotGroups);
	void validateCommerce() const;
	void updateTimers();
	CvCity* findClosestCity(const CvPlot* pPlot) const;

	bool hasReadyUnautomatedUnit(bool bAny = false) const;
	bool hasReadyUnit(bool bAny = false) const;
	bool hasAutoUnit() const;
	DllExport bool hasBusyUnit() const;

	bool isChoosingFreeTech() const;
	void setChoosingFreeTech(bool bValue);
	void chooseTech(int iDiscover = 0, CvWString szText = CvWString(), bool bFront = false); // Exposed to Python

	int calculateScore(bool bFinal = false, bool bVictory = false) const;

	int findBestFoundValue() const; // Exposed to Python

	int upgradeAllPrice(UnitTypes eUpgradeUnit, UnitTypes eFromUnit);

	int countReligionSpreadUnits(const CvArea* pArea, ReligionTypes eReligion, bool bIncludeTraining = false) const; // Exposed to Python
	int countCorporationSpreadUnits(const CvArea* pArea, CorporationTypes eCorporation, bool bIncludeTraining = false) const; // Exposed to Python

	int countNumCoastalCities() const; // Exposed to Python
	int countNumCoastalCitiesByArea(const CvArea* pArea) const; // Exposed to Python
	int countNumCitiesWithOrbitalInfrastructure() const;
	inline void noteOrbitalInfrastructureCountDirty() { m_orbitalInfrastructureCountDirty = true; }
	unsigned long long countTotalCulture() const;
	void doCountTotalCulture(); // Exposed to Python
	int countOwnedBonuses(BonusTypes eBonus) const; // Exposed to Python
	int countUnimprovedBonuses(const CvArea* pArea, const CvPlot* pFromPlot = NULL) const; // Exposed to Python
	int countCityFeatures(FeatureTypes eFeature) const; // Exposed to Python
	int countNumBuildings(BuildingTypes eBuilding) const; // Exposed to Python
	int countNumCitiesConnectedToCapital() const; // Exposed to Python
	int countPotentialForeignTradeCities(const CvArea* pIgnoreArea = NULL) const; // Exposed to Python
	int countPotentialForeignTradeCitiesConnected() const; // Exposed to Python

	DllExport bool canContact(PlayerTypes ePlayer) const; // Exposed to Python
	void contact(PlayerTypes ePlayer); // Exposed to Python
	DllExport void handleDiploEvent(DiploEventTypes eDiploEvent, PlayerTypes ePlayer, int iData1, int iData2);
	bool canTradeWith(PlayerTypes eWhoTo) const; // Exposed to Python
	DllExport bool canTradeItem(PlayerTypes eWhoTo, TradeData item, bool bTestDenial = false) const; // Exposed to Python
	DllExport DenialTypes getTradeDenial(PlayerTypes eWhoTo, TradeData item) const; // Exposed to Python
	bool canTradeNetworkWith(PlayerTypes ePlayer) const; // Exposed to Python
	int getNumAvailableBonuses(BonusTypes eBonus) const; // Exposed to Python
	int getNumTradeableBonuses(BonusTypes eBonus) const; // Exposed to Python
	int getNumTradeBonusImports(PlayerTypes ePlayer) const; // Exposed to Python
	bool hasBonus(BonusTypes eBonus) const; // Exposed to Python

	bool isTradingWithTeam(TeamTypes eTeam, bool bIncludeCancelable) const;
	bool canStopTradingWithTeam(TeamTypes eTeam, bool bContinueNotTrading = false) const; // Exposed to Python
	void stopTradingWithTeam(TeamTypes eTeam); // Exposed to Python
	void killAllDeals(); // Exposed to Python

	void findNewCapital(); // Exposed to Python
	int getNumGovernmentCenters() const; // Exposed to Python

	bool canRaze(CvCity* pCity) const; // Exposed to Python
	void raze(CvCity* pCity); // Exposed to Python
	void disband(CvCity* pCity); // Exposed to Python

	bool canReceiveGoody(CvPlot* pPlot, GoodyTypes eGoody, CvUnit* pUnit) const; // Exposed to Python
	void receiveGoody(CvPlot* pPlot, GoodyTypes eGoody, CvUnit* pUnit); // Exposed to Python
	void doGoody(CvPlot* pPlot, CvUnit* pUnit); // Exposed to Python

	DllExport bool canFound(int iX, int iY, bool bTestVisible = false) const; // Exposed to Python
	void found(int iX, int iY, CvUnit* pUnit = NULL); // Exposed to Python

	bool canTrain(UnitTypes eUnit, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, bool bPropertySpawn = false) const; // Exposed to Python
	bool canConstruct(BuildingTypes eBuilding, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, TechTypes eIgnoreTechReq = NO_TECH, int* probabilityEverConstructable = NULL, bool bAffliction = false, bool bExposed = false) const; // Exposed to Python
	bool canConstructInternal(BuildingTypes eBuilding, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, TechTypes eIgnoreTechReq = NO_TECH, int* probabilityEverConstructable = NULL, bool bAffliction = false, bool bExposed = false) const;
	bool canCreate(ProjectTypes eProject, bool bContinue = false, bool bTestVisible = false) const; // Exposed to Python
	bool canMaintain(ProcessTypes eProcess, bool bContinue = false) const; // Exposed to Python
	bool isProductionMaxedBuilding(BuildingTypes building, bool bAcquireCity = false) const; // Exposed to Python
	bool isProductionMaxedUnit(UnitTypes eUnit) const; // Exposed to Python
	bool isProductionMaxedProject(ProjectTypes eProject) const; // Exposed to Python
	int getProductionNeeded(UnitTypes eUnit) const; // Exposed to Python
	int getProductionNeeded(BuildingTypes eBuilding) const; // Exposed to Python
	int getProductionNeeded(ProjectTypes eProject) const; // Exposed to Python
	int getProductionModifier(UnitTypes eUnit) const;
	int getProductionModifier(BuildingTypes eBuilding) const;
	int getProductionModifier(ProjectTypes eProject) const;

	int getBuildingPrereqBuilding(BuildingTypes eBuilding, BuildingTypes ePrereqBuilding, int iExtra = 0) const; // Exposed to Python
	void removeBuilding(BuildingTypes building);

	void processBuilding(BuildingTypes eBuilding, int iChange, CvArea* pArea, bool bReligiouslyDisabling = false);

	int getBuildCost(const CvPlot* pPlot, BuildTypes eBuild) const;
	bool canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestEra = false, bool bTestVisible = false, bool bIncludePythonOverrides = true) const; // Exposed to Python

	RouteTypes getBestRoute(CvPlot* pPlot = NULL, bool bConnect = true, CvUnit* pBuilder = NULL) const; // Exposed to Python

	int getImprovementUpgradeRateTimes100(ImprovementTypes eImprovement) const; // Exposed to Python

	int calculateTotalYield(YieldTypes eYield) const; // Exposed to Python
	int calculateTotalExports(YieldTypes eYield) const; // Exposed to Python
	int calculateTotalImports(YieldTypes eYield) const; // Exposed to Python

	int calculateTotalCityHappiness() const; // Exposed to Python
	int calculateTotalCityUnhappiness() const; // Exposed to Python

	int calculateTotalCityHealthiness() const; // Exposed to Python
	int calculateTotalCityUnhealthiness() const; // Exposed to Python

	int calculateUnitCost(int& iFreeUnits, int& iFreeMilitaryUnits, int& iPaidUnits, int& iPaidMilitaryUnits, int& iBaseUnitCost, int& iMilitaryCost, int& iExtraCost) const;
	int calculateUnitCost() const; // Exposed to Python
	int calculateUnitSupply(int& iPaidUnits, int& iBaseSupplyCost) const; // Exposed to Python
	int calculateUnitSupply() const; // Exposed to Python
	int calculatePreInflatedCosts() const; // Exposed to Python
	int calculateInflationRate() const; // Exposed to Python
	int calculateInflatedCosts() const; // Exposed to Python
	int getCurrentInflationPerTurnTimes10000() const;

	int getFreeUnitCountdown() const;
	void setFreeUnitCountdown(int iValue);

	int calculateBaseNetGold() const;
	int calculateBaseNetResearch(TechTypes eTech = NO_TECH) const; // Exposed to Python
	int calculateResearchModifier(TechTypes eTech) const; // Exposed to Python
	int calculateGoldRate() const; // Exposed to Python
	int calculateResearchRate(TechTypes eTech = NO_TECH) const; // Exposed to Python
	int calculateTotalCommerce() const;

	bool isResearch() const; // Exposed to Python
	bool canEverResearch(TechTypes eTech) const; // Exposed to Python
	bool canResearch(TechTypes eTech) const; // Exposed to Python
	TechTypes getCurrentResearch() const; // Exposed to Python
	bool isCurrentResearchRepeat() const; // Exposed to Python
	bool isNoResearchAvailable() const; // Exposed to Python
	int getResearchTurnsLeft(TechTypes eTech, bool bOverflow) const; // Exposed to Python

	bool isCivic(CivicTypes eCivic) const; // Exposed to Python
	bool canDoCivics(CivicTypes eCivic) const; // Exposed to Python
	bool canRevolution(CivicTypes* paeNewCivics) const; // Exposed to Python
	void revolution(CivicTypes* paeNewCivics, bool bForce = false); // Exposed to Python
	int getCivicPercentAnger(CivicTypes eCivic) const; // Exposed to Python

	bool canDoReligion(ReligionTypes eReligion) const; // Exposed to Python
	bool canChangeReligion() const; // Exposed to Python
	bool canConvert(ReligionTypes eReligion) const; // Exposed to Python
	void convert(ReligionTypes eReligion); // Exposed to Python
	bool hasHolyCity(ReligionTypes eReligion) const; // Exposed to Python
	bool hasHolyCity() const; // Exposed to Python
	bool hasStateReligionHolyCity() const; // Exposed to Python
	bool hasStateReligionShrine() const; // Exposed to Python

	int countHolyCities() const; // Exposed to Python
	void foundReligion(ReligionTypes eReligion, ReligionTypes eSlotReligion, bool bAward); // Exposed to Python

	bool hasHeadquarters(CorporationTypes eCorporation) const; // Exposed to Python
	int countHeadquarters() const; // Exposed to Python
	int countCorporations(CorporationTypes eCorporation) const; // Exposed to Python
	void foundCorporation(CorporationTypes eCorporation); // Exposed to Python

	int getCivicAnarchyLength(CivicTypes* paeNewCivics) const; // Exposed to Python
	int getReligionAnarchyLength() const; // Exposed to Python

	int unitsRequiredForGoldenAge() const; // Exposed to Python
	int unitsGoldenAgeCapable() const; // Exposed to Python
	int unitsGoldenAgeReady() const; // Exposed to Python
	void killGoldenAgeUnits(CvUnit* pUnitAlive);

	int greatPeopleThresholdMilitary() const; // Exposed to Python
	int greatPeopleThresholdNonMilitary() const; // Exposed to Python

	int specialistYield(SpecialistTypes eSpecialist, YieldTypes eYield) const; // Exposed to Python
	int specialistCommerceTimes100(SpecialistTypes eSpecialist, CommerceTypes eCommerce) const;
	int specialistCommerce(SpecialistTypes eSpecialist, CommerceTypes eCommerce) const; // Exposed to Python

	CvPlot* getStartingPlot() const; // Exposed to Python
	void setStartingPlot(CvPlot* pNewValue, bool bUpdateStartDist); // Exposed to Python

	int getTotalPopulation() const; // Exposed to Python
	int getAveragePopulation() const; // Exposed to Python
	void changeTotalPopulation(int iChange);
	long getRealPopulation() const; // Exposed to Python
	int getReligionPopulation(ReligionTypes eReligion) const;

	int getTotalLand() const; // Exposed to Python
	void changeTotalLand(int iChange);

	int getTotalLandScored() const; // Exposed to Python
	void changeTotalLandScored(int iChange);

	int getEffectiveGold() const;

	int getGold() const; // Exposed to Python
	DllExport void setGold(int iNewValue); // Exposed to Python
	DllExport void changeGold(int iChange);

	int getGreaterGold() const; // Exposed to Python
	void setGreaterGold(int iNewValue); // Exposed to Python
	void changeGreaterGold(int iChange); // Exposed to Python

	int getGoldPerTurn() const; // Exposed to Python

	int getAdvancedStartPoints() const; // Exposed to Python
	void setAdvancedStartPoints(int iNewValue); // Exposed to Python
	void changeAdvancedStartPoints(int iChange); // Exposed to Python

	void doEspionageOneOffPoints(int iChange);
	int getEspionageSpending(TeamTypes eAgainstTeam, int iTotal = -1) const; // Exposed to Python
	bool canDoEspionageMission(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvPlot* pPlot, int iExtraData, const CvUnit* pUnit) const; // Exposed to Python
	int getEspionageMissionBaseCost(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvPlot* pPlot, int iExtraData, const CvUnit* pSpyUnit) const;
	int getEspionageMissionCost(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvPlot* pPlot = NULL, int iExtraData = -1, const CvUnit* pSpyUnit = NULL) const; // Exposed to Python
	int getEspionageMissionCostModifier(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvPlot* pPlot = NULL, int iExtraData = -1, const CvUnit* pSpyUnit = NULL) const;
	bool doEspionageMission(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, CvPlot* pPlot, int iExtraData, CvUnit* pUnit, bool bCaught = false);
	int getEspionageSpendingWeightAgainstTeam(TeamTypes eIndex) const; // Exposed to Python
	void setEspionageSpendingWeightAgainstTeam(TeamTypes eIndex, int iValue); // Exposed to Python
	void changeEspionageSpendingWeightAgainstTeam(TeamTypes eIndex, int iChange); // Exposed to Python

	bool canStealTech(PlayerTypes eTarget, TechTypes eTech) const;
	bool canForceCivics(PlayerTypes eTarget, CivicTypes eCivic) const;
	bool canForceReligion(PlayerTypes eTarget, ReligionTypes eReligion) const;
	bool canSpyDestroyUnit(PlayerTypes eTarget, const CvUnit& kUnit) const;
	bool canSpyBribeUnit(PlayerTypes eTarget, const CvUnit& kUnit) const;
	bool canSpyDestroyBuilding(PlayerTypes eTarget, BuildingTypes eBuilding) const;
	bool canSpyDestroyProject(PlayerTypes eTarget, ProjectTypes eProject) const;

	void doAdvancedStartAction(AdvancedStartActionTypes eAction, int iX, int iY, int iData, bool bAdd);
	int getAdvancedStartUnitCost(UnitTypes eUnit, bool bAdd, const CvPlot* pPlot = NULL) const; // Exposed to Python 
	int getAdvancedStartCityCost(bool bAdd, const CvPlot* pPlot = NULL) const; // Exposed to Python 
	int getAdvancedStartPopCost(bool bAdd, const CvCity* pCity = NULL) const; // Exposed to Python 
	int getAdvancedStartCultureCost(bool bAdd, const CvCity* pCity = NULL) const; // Exposed to Python 
	int getAdvancedStartBuildingCost(BuildingTypes eBuilding, bool bAdd, const CvCity* pCity = NULL) const; // Exposed to Python 
	int getAdvancedStartImprovementCost(ImprovementTypes eImprovement, bool bAdd, const CvPlot* pPlot = NULL) const; // Exposed to Python 
	int getAdvancedStartRouteCost(RouteTypes eRoute, bool bAdd, const CvPlot* pPlot = NULL) const; // Exposed to Python 
	int getAdvancedStartTechCost(TechTypes eTech, bool bAdd) const; // Exposed to Python 
	int getAdvancedStartVisibilityCost(bool bAdd, const CvPlot* pPlot = NULL) const; // Exposed to Python 

	int getGoldenAgeTurns() const;// Exposed to Python  
	bool isGoldenAge() const; // Exposed to Python
	void changeGoldenAgeTurns(int iChange); // Exposed to Python 
	int getGoldenAgeLength() const;

	int getNumUnitGoldenAges() const; // Exposed to Python
	void changeNumUnitGoldenAges(int iChange); // Exposed to Python

	int getAnarchyTurns() const; // Exposed to Python
	bool isAnarchy() const; // Exposed to Python

	void changeAnarchyTurns(int iChange, bool bHideMessages = false); // Exposed to Python

	int getStrikeTurns() const; // Exposed to Python
	void changeStrikeTurns(int iChange);

	int getMaxAnarchyTurns() const; // Exposed to Python
	void updateMaxAnarchyTurns();

	int getMinAnarchyTurns() const; // Exposed to Python
	void updateMinAnarchyTurns();

	int getAnarchyModifier() const; // Exposed to Python
	void changeAnarchyModifier(int iChange);

	int getGoldenAgeModifier() const; // Exposed to Python
	void changeGoldenAgeModifier(int iChange);

	int getHurryModifier() const; // Exposed to Python
	void changeHurryModifier(int iChange);

	void createGreatPeople(UnitTypes eGreatPersonUnit, bool bIncrementThreshold, bool bIncrementExperience, int iX, int iY);

	int getGreatPeopleCreated() const; // Exposed to Python
	void incrementGreatPeopleCreated();

	int getGreatGeneralsCreated() const; // Exposed to Python
	void incrementGreatGeneralsCreated();

	int getGreatPeopleThresholdModifier() const; // Exposed to Python
	void changeGreatPeopleThresholdModifier(int iChange);

	int getGreatGeneralsThresholdModifier() const; // Exposed to Python
	void changeGreatGeneralsThresholdModifier(int iChange);

	int getGreatPeopleRateModifier() const; // Exposed to Python
	void changeGreatPeopleRateModifier(int iChange);

	int getGreatGeneralRateModifier() const; // Exposed to Python
	void changeGreatGeneralRateModifier(int iChange);

	int getDomesticGreatGeneralRateModifier() const; // Exposed to Python
	void changeDomesticGreatGeneralRateModifier(int iChange);

	int getStateReligionGreatPeopleRateModifier() const; // Exposed to Python
	void changeStateReligionGreatPeopleRateModifier(int iChange);

	int getMaxGlobalBuildingProductionModifier() const; // Exposed to Python
	void changeMaxGlobalBuildingProductionModifier(int iChange);

	int getMaxTeamBuildingProductionModifier() const; // Exposed to Python
	void changeMaxTeamBuildingProductionModifier(int iChange);

	int getMaxPlayerBuildingProductionModifier() const; // Exposed to Python
	void changeMaxPlayerBuildingProductionModifier(int iChange);

	int getFreeExperience() const; // Exposed to Python
	void changeFreeExperience(int iChange);

	int getFeatureProductionModifier() const; // Exposed to Python
	void changeFeatureProductionModifier(int iChange);

	int getWorkerSpeedModifier() const; // Exposed to Python
	void changeWorkerSpeedModifier(int iChange);
	int getWorkRate(BuildTypes eBuild) const;
	int getImprovementUpgradeRateModifier() const; // Exposed to Python
	void changeImprovementUpgradeRateModifier(int iChange);

	int getMilitaryProductionModifier() const; // Exposed to Python
	void changeMilitaryProductionModifier(int iChange);

	int getSpaceProductionModifier() const; // Exposed to Python
	void changeSpaceProductionModifier(int iChange);

	int getCityDefenseModifier() const; // Exposed to Python
	void changeCityDefenseModifier(int iChange);

	bool isNonStateReligionCommerce() const;
	void changeNonStateReligionCommerce(int iNewValue);

	bool isUpgradeAnywhere() const;
	void changeUpgradeAnywhere(int iNewValue);

	int getRevIdxLocal() const; // Exposed to Python
	void changeRevIdxLocal(int iChange);

	int getRevIdxNational() const; // Exposed to Python
	void changeRevIdxNational(int iChange);

	int getRevIdxDistanceModifier() const; // Exposed to Python
	void changeRevIdxDistanceModifier(int iChange);

	int getRevIdxHolyCityGood() const; // Exposed to Python
	void changeRevIdxHolyCityGood(int iChange);

	int getRevIdxHolyCityBad() const; // Exposed to Python
	void changeRevIdxHolyCityBad(int iChange);

	float getRevIdxNationalityMod() const; // Exposed to Python
	void changeRevIdxNationalityMod(float fChange);

	float getRevIdxBadReligionMod() const; // Exposed to Python
	void changeRevIdxBadReligionMod(float fChange);

	float getRevIdxGoodReligionMod() const; // Exposed to Python
	void changeRevIdxGoodReligionMod(float fChange);

	bool isInquisitionConditions() const; // Exposed to Python
	void setInquisitionConditions();

	int getUnitUpgradePriceModifier() const; // Exposed to Python
	void changeUnitUpgradePriceModifier(int iChange);

	bool canFoundReligion() const; // Exposed to Python

	int getNumNukeUnits() const; // Exposed to Python
	void changeNumNukeUnits(int iChange);

	int getNumOutsideUnits() const; // Exposed to Python
	void changeNumOutsideUnits(int iChange);

	int getBaseFreeUnits() const; // Exposed to Python
	void changeBaseFreeUnits(int iChange);

	int getBaseFreeMilitaryUnits() const; // Exposed to Python
	void changeBaseFreeMilitaryUnits(int iChange);

	int getFreeUnitsPopulationPercent() const; // Exposed to Python
	void changeFreeUnitsPopulationPercent(int iChange);

	int getFreeMilitaryUnitsPopulationPercent() const; // Exposed to Python
	void changeFreeMilitaryUnitsPopulationPercent(int iChange);

	int getTypicalUnitValue(UnitAITypes eUnitAI) const;

	int getGoldPerUnit() const; // Exposed to Python
	void changeGoldPerUnit(int iChange);

	int getGoldPerMilitaryUnit() const; // Exposed to Python
	void changeGoldPerMilitaryUnit(int iChange);

	int getExtraUnitCost() const; // Exposed to Python
	void changeExtraUnitCost(int iChange);

	int getNumMilitaryUnits() const; // Exposed to Python
	void changeNumMilitaryUnits(int iChange);

	int getUnitPercentCountForCostAdjustment() const; // Exposed to Python
	void changeUnitPercentCountForCostAdjustment(int iChange);
	int getUnitCountForCostAdjustmentTotal() const;

	int getHappyPerMilitaryUnit() const; // Exposed to Python

	void changeHappyPerMilitaryUnit(int iChange, bool bLimited = false);

	int getMilitaryFoodProductionCount() const;
	bool isMilitaryFoodProduction() const; // Exposed to Python

	void changeMilitaryFoodProductionCount(int iChange, bool bLimited = false);

	int getHighestUnitLevel() const; // Exposed to Python
	void setHighestUnitLevel(int iNewValue);

	int getConscriptCount() const; // Exposed to Python
	void setConscriptCount(int iNewValue); // Exposed to Python
	void changeConscriptCount(int iChange); // Exposed to Python

	int getMaxConscript() const; // Exposed to Python
	void changeMaxConscript(int iChange);

	int getOverflowResearch() const; // Exposed to Python
	void setOverflowResearch(int iNewValue); // Exposed to Python
	void changeOverflowResearch(int iChange); // Exposed to Python

	int getNoUnhealthyPopulationCount() const;
	bool isNoUnhealthyPopulation() const; // Exposed to Python

	void changeNoUnhealthyPopulationCount(int iChange, bool bLimited = false);

	int getExpInBorderModifier() const;
	void changeExpInBorderModifier(int iChange);

	int getBuildingOnlyHealthyCount() const;
	bool isBuildingOnlyHealthy() const; // Exposed to Python


	int getMaintenanceModifier();
	void changeMaintenanceModifier(int iChange);
	int getCoastalDistanceMaintenanceModifier() const;
	void changeCoastalDistanceMaintenanceModifier(int iChange);
	int getConnectedCityMaintenanceModifier();
	void changeConnectedCityMaintenanceModifier(int iChange);

	void changeBuildingOnlyHealthyCount(int iChange, bool bLimited = false);

	int getDistanceMaintenanceModifier() const; // Exposed to Python
	void changeDistanceMaintenanceModifier(int iChange);

	int getNumCitiesMaintenanceModifier() const; // Exposed to Python
	void changeNumCitiesMaintenanceModifier(int iChange);

	int getCorporationMaintenanceModifier() const; // Exposed to Python
	void changeCorporationMaintenanceModifier(int iChange, bool bLimited = false);

	int getTotalMaintenance() const; // Exposed to Python

	int getUpkeepModifier() const; // Exposed to Python
	void changeUpkeepModifier(int iChange);

	int getLevelExperienceModifier() const; // Exposed to Python
	void changeLevelExperienceModifier(int iChange);

	int getExtraHealth() const; // Exposed to Python
	void changeExtraHealth(int iChange, bool bLimited = false);

	int getCivicHealth() const; // Included in getExtraHealth() but split off to aid hover text displays

	int getBuildingGoodHealth() const; // Exposed to Python
	void changeBuildingGoodHealth(int iChange);

	int getBuildingBadHealth() const; // Exposed to Python
	void changeBuildingBadHealth(int iChange);

	int getExtraHappiness() const; // Exposed to Python
	void changeExtraHappiness(int iChange, bool bUnattributed = false);

	int getBuildingHappiness() const; // Exposed to Python
	void changeBuildingHappiness(int iChange);

	int getLargestCityHappiness() const; // Exposed to Python
	void changeLargestCityHappiness(int iChange, bool bLimited = false);

	int getWarWearinessPercentAnger() const; // Exposed to Python
	void updateWarWearinessPercentAnger();
	int getModifiedWarWearinessPercentAnger(int iWarWearinessPercentAnger) const;

	int getWarWearinessModifier() const; // Exposed to Python
	void changeWarWearinessModifier(int iChange, bool bLimited = false);

	int getFreeSpecialist() const; // Exposed to Python
	void changeFreeSpecialist(int iChange);

	int getNoForeignTradeCount() const;
	bool isNoForeignTrade() const; // Exposed to Python
	void changeNoForeignTradeCount(int iChange, bool bLimited = false);

	int getNoCorporationsCount() const;
	bool isNoCorporations() const; // Exposed to Python
	void changeNoCorporationsCount(int iChange, bool bLimited = false);

	int getNoForeignCorporationsCount() const;
	bool isNoForeignCorporations() const; // Exposed to Python
	void changeNoForeignCorporationsCount(int iChange, bool bLimited = false);

	int getCoastalTradeRoutes() const; // Exposed to Python
	void changeCoastalTradeRoutes(int iChange); // Exposed to Python

	int getTradeRoutes() const; // Exposed to Python
	void changeTradeRoutes(int iChange); // Exposed to Python

	int getRevolutionTimer() const; // Exposed to Python
	void setRevolutionTimer(int iNewValue);
	void changeRevolutionTimer(int iChange);

	int getConversionTimer() const; // Exposed to Python
	void setConversionTimer(int iNewValue);
	void changeConversionTimer(int iChange);

	int getStateReligionCount() const;
	bool isStateReligion() const; // Exposed to Python
	void changeStateReligionCount(int iChange, bool bLimited = false);

	int getNoNonStateReligionSpreadCount() const;
	bool isNoNonStateReligionSpread() const; // Exposed to Python
	void changeNoNonStateReligionSpreadCount(int iChange);

	int getStateReligionHappiness() const; // Exposed to Python
	void changeStateReligionHappiness(int iChange, bool bLimited = false);
	int getNonStateReligionHappiness() const; // Exposed to Python
	void changeNonStateReligionHappiness(int iChange, bool bLimited = false);

	int getStateReligionUnitProductionModifier() const; // Exposed to Python
	void changeStateReligionUnitProductionModifier(int iChange);

	int getStateReligionBuildingProductionModifier() const; // Exposed to Python
	void changeStateReligionBuildingProductionModifier(int iChange); // Exposed to Python

	int getStateReligionFreeExperience() const; // Exposed to Python
	void changeStateReligionFreeExperience(int iChange);

	DllExport CvCity* getCapitalCity() const; // Exposed to Python
	void setCapitalCity(CvCity* pNewCapitalCity);

	int getCitiesLost() const; // Exposed to Python
	void changeCitiesLost(int iChange);

	int getWinsVsBarbs() const; // Exposed to Python
	void changeWinsVsBarbs(int iChange);

	int getAssets() const; // Exposed to Python
	void changeAssets(int iChange); // Exposed to Python

	int getPower() const; // Exposed to Python
	void changePower(int iChange);

	int getTechPower() const; // Exposed to Python
	void changeTechPower(int iChange);
	int getUnitPower() const; // Exposed to Python
	void changeUnitPower(int iChange);

	int getPopScore(bool bCheckVassal = true) const; // Exposed to Python
	void changePopScore(int iChange); // Exposed to Python
	int getLandScore(bool bCheckVassal = true) const; // Exposed to Python
	void changeLandScore(int iChange); // Exposed to Python
	int getTechScore() const; // Exposed to Python
	void changeTechScore(int iChange); // Exposed to Python
	int getWondersScore() const; // Exposed to Python
	void changeWondersScore(int iChange); // Exposed to Python

	int getCombatExperience() const; // Exposed to Python
	void setCombatExperience(int iExperience, UnitTypes eGGType = NO_UNIT); // Exposed to Python
	void changeCombatExperience(int iChange, UnitTypes eGGType = NO_UNIT); // Exposed to Python

	bool isConnected() const;
	DllExport int getNetID() const;
	DllExport void setNetID(int iNetID);
	void sendReminder();

	uint getStartTime() const;
	DllExport void setStartTime(uint uiStartTime);
	uint getTotalTimePlayed() const; // Exposed to Python

	bool isMinorCiv() const; // Exposed to Python

	DllExport bool isAlive() const; // Exposed to Python
	bool isEverAlive() const; // Exposed to Python
	void setAlive(bool bNewValue);
	void setNewPlayerAlive(bool bNewValue);
	void verifyAlive();

	DllExport bool isTurnActive() const;
	DllExport void setTurnActive(bool bNewValue, bool bDoTurn = true);

	bool isAutoMoves() const;
	void setAutoMoves(bool bNewValue);
	DllExport void setTurnActiveForPbem(bool bActive);

	DllExport bool isPbemNewTurn() const;
	DllExport void setPbemNewTurn(bool bNew);

	bool isEndTurn() const;
	DllExport void setEndTurn(bool bNewValue);

	DllExport bool isTurnDone() const;

	bool isExtendedGame() const; // Exposed to Python
	void makeExtendedGame();

	bool isFoundedFirstCity() const; // Exposed to Python
	void setFoundedFirstCity(bool bNewValue);

	bool isStrike() const; // Exposed to Python
	void setStrike(bool bNewValue);

	DllExport PlayerTypes getID() const; // Exposed to Python

	DllExport HandicapTypes getHandicapType() const; // Exposed to Python

	DllExport CivilizationTypes getCivilizationType() const; // Exposed to Python

	DllExport LeaderHeadTypes getLeaderType() const; // Exposed to Python

	LeaderHeadTypes getPersonalityType() const; // Exposed to Python
	void setPersonalityType(LeaderHeadTypes eNewValue); // Exposed to Python

	DllExport EraTypes getCurrentEra() const; // Exposed to Python
	void setCurrentEra(EraTypes eNewValue);

	int getCulture() const;
	void setCulture(int iNewValue);
	void changeCulture(int iAddValue);


	int getGreaterCulture() const;
	void setGreaterCulture(int iNewValue);
	void changeGreaterCulture(int iAddValue);

	ReligionTypes getLastStateReligion() const;
	ReligionTypes getStateReligion() const; // Exposed to Python
	void setLastStateReligion(ReligionTypes eNewValue); // Exposed to Python

	PlayerTypes getParent() const;
	void setParent(PlayerTypes eParent);

	DllExport TeamTypes getTeam() const; // Exposed to Python
	void setTeam(TeamTypes eTeam);
	void updateTeamType();

	void setDoNotBotherStatus(PlayerTypes playerID);
	bool isDoNotBotherStatus(PlayerTypes playerID) const;

	DllExport PlayerColorTypes getPlayerColor() const; // Exposed to Python
	DllExport int getPlayerTextColorR() const; // Exposed to Python
	DllExport int getPlayerTextColorG() const; // Exposed to Python
	DllExport int getPlayerTextColorB() const; // Exposed to Python
	int getPlayerTextColorA() const;

	int getSeaPlotYield(YieldTypes eIndex) const;
	void changeSeaPlotYield(YieldTypes eIndex, int iChange);

	int getGoldenAgeYield(YieldTypes eIndex) const;
	void changeGoldenAgeYield(YieldTypes eIndex, int iChange);

	int getGoldenAgeCommerce(CommerceTypes eIndex) const;
	void changeGoldenAgeCommerce(CommerceTypes eIndex, int iChange);

	int getYieldRateModifier(YieldTypes eIndex) const; // Exposed to Python
	void changeYieldRateModifier(YieldTypes eIndex, int iChange);

	int getCapitalYieldRateModifier(YieldTypes eIndex) const; // Exposed to Python
	void changeCapitalYieldRateModifier(YieldTypes eIndex, int iChange);

	int getExtraYieldThreshold(YieldTypes eIndex) const; // Exposed to Python
	void updateExtraYieldThreshold(YieldTypes eIndex);

	int getLessYieldThreshold(YieldTypes eIndex) const; // Exposed to Python
	void updateLessYieldThreshold(YieldTypes eIndex);

	int getTradeYieldModifier(YieldTypes eIndex) const; // Exposed to Python
	void changeTradeYieldModifier(YieldTypes eIndex, int iChange);

	int getFreeCityCommerce(CommerceTypes eIndex) const; // Exposed to Python
	void changeFreeCityCommerce(CommerceTypes eIndex, int iChange);

	int getCommercePercent(CommerceTypes eIndex) const; // Exposed to Python
	void setCommercePercent(CommerceTypes eIndex, int iNewValue); // Exposed to Python
	void changeCommercePercent(CommerceTypes eIndex, int iChange); // Exposed to Python

	int getTotalCityBaseCommerceRate(CommerceTypes eIndex) const;
	int getCommerceRate(CommerceTypes eIndex) const; // Exposed to Python
	void changeCommerceRate(CommerceTypes eIndex, int iChange);

	int getCommerceRateModifier(CommerceTypes eIndex) const; // Exposed to Python
	void changeCommerceRateModifier(CommerceTypes eIndex, int iChange);

	int getCommerceRateModifierfromEvents(CommerceTypes eIndex) const;
	void changeCommerceRateModifierfromEvents(CommerceTypes eIndex, int iChange);

	int getCommerceRateModifierfromBuildings(CommerceTypes eIndex) const;
	void changeCommerceRateModifierfromBuildings(CommerceTypes eIndex, int iChange);

	int getCapitalCommerceRateModifier(CommerceTypes eIndex) const; // Exposed to Python
	void changeCapitalCommerceRateModifier(CommerceTypes eIndex, int iChange);

	int getStateReligionBuildingCommerce(CommerceTypes eIndex) const; // Exposed to Python
	void changeStateReligionBuildingCommerce(CommerceTypes eIndex, int iChange);

	int getSpecialistExtraCommerce(CommerceTypes eIndex) const; // Exposed to Python
	void changeSpecialistExtraCommerce(CommerceTypes eIndex, int iChange);

	int getCommerceFlexibleCount(CommerceTypes eIndex) const;
	bool isCommerceFlexible(CommerceTypes eIndex) const; // Exposed to Python
	void changeCommerceFlexibleCount(CommerceTypes eIndex, int iChange);

	int getGoldPerTurnByPlayer(PlayerTypes eIndex) const; // Exposed to Python
	void changeGoldPerTurnByPlayer(PlayerTypes eIndex, int iChange);

	bool isFeatAccomplished(FeatTypes eIndex) const; // Exposed to Python
	void setFeatAccomplished(FeatTypes eIndex, bool bNewValue); // Exposed to Python

	DllExport bool isOption(PlayerOptionTypes eIndex) const; // Exposed to Python
	DllExport void setOption(PlayerOptionTypes eIndex, bool bNewValue); // Exposed to Python

	bool isLoyalMember(VoteSourceTypes eVoteSource) const; // Exposed to Python
	void setLoyalMember(VoteSourceTypes eVoteSource, bool bNewValue); // Exposed to Python

	bool isPlayable() const;
	void setPlayable(bool bNewValue);

	int getBonusExport(BonusTypes eIndex) const; // Exposed to Python
	void changeBonusExport(BonusTypes eIndex, int iChange);

	int getBonusImport(BonusTypes eIndex) const; // Exposed to Python
	void changeBonusImport(BonusTypes eIndex, int iChange);

	int getImprovementCount(ImprovementTypes eIndex) const; // Exposed to Python
	void changeImprovementCount(ImprovementTypes eIndex, int iChange);

	int getFreeBuildingCount(BuildingTypes eIndex) const;
	int getFreeAreaBuildingCount(BuildingTypes eIndex, const CvArea* area) const;
	bool isBuildingFree(BuildingTypes eIndex, const CvArea* area = NULL) const; // Exposed to Python
	void changeFreeBuildingCount(BuildingTypes eIndex, int iChange);
	void changeFreeAreaBuildingCount(BuildingTypes eIndex, const CvArea* area, int iChange);

	int getExtraBuildingHappiness(BuildingTypes eIndex) const; // Exposed to Python
	void changeExtraBuildingHappiness(BuildingTypes eIndex, int iChange, bool bLimited = false);

	int getExtraBuildingHealth(BuildingTypes eIndex) const; // Exposed to Python
	void changeExtraBuildingHealth(BuildingTypes eIndex, int iChange, bool bLimited = false);

	int getFeatureHappiness(FeatureTypes eIndex) const; // Exposed to Python
	void changeFeatureHappiness(FeatureTypes eIndex, int iChange, bool bLimited = false);

	int getUnitCount(UnitTypes eIndex) const; // Exposed to Python
	bool isUnitMaxedOut(const UnitTypes eIndex, const int iExtra = 0) const; // Exposed to Python
	void changeUnitCount(UnitTypes eIndex, int iChange);
	int getUnitMaking(UnitTypes eIndex) const; // Exposed to Python
	void changeUnitMaking(UnitTypes eIndex, int iChange);
	int getUnitCountPlusMaking(UnitTypes eIndex) const; // Exposed to Python

	int getBuildingCount(BuildingTypes eIndex) const;
	int getBuildingGroupCount(SpecialBuildingTypes eIndex) const; // Exposed to Python
	bool isBuildingMaxedOut(BuildingTypes eIndex, int iExtra = 0) const; // Exposed to Python
	bool isBuildingGroupMaxedOut(SpecialBuildingTypes eIndex, int iExtra = 0) const; // Exposed to Python
	void changeBuildingCount(BuildingTypes eIndex, int iChange); // Exposed to Python
	void changeBuildingGroupCount(SpecialBuildingTypes eIndex, int iChange);
	int getBuildingMaking(BuildingTypes eIndex) const;
	int getBuildingGroupMaking(SpecialBuildingTypes eIndex) const; // Exposed to Python
	void changeBuildingMaking(BuildingTypes eIndex, int iChange); // Exposed to Python
	void changeBuildingGroupMaking(SpecialBuildingTypes eIndex, int iChange);
	int getBuildingCountPlusMaking(BuildingTypes eIndex) const;
	int getBuildingGroupCountPlusMaking(SpecialBuildingTypes eIndex) const; // Exposed to Python

	int getHurryCount(HurryTypes eIndex) const; // Exposed to Python
	bool canHurry(HurryTypes eIndex) const; // Exposed to Python
	bool canPopRush() const;
	void changeHurryCount(HurryTypes eIndex, int iChange);

	int getSpecialBuildingNotRequiredCount(SpecialBuildingTypes eIndex) const; // Exposed to Python
	bool isSpecialBuildingNotRequired(SpecialBuildingTypes eIndex) const; // Exposed to Python
	void changeSpecialBuildingNotRequiredCount(SpecialBuildingTypes eIndex, int iChange);

	int getHasCivicOptionCount(CivicOptionTypes eIndex) const;
	bool isHasCivicOption(CivicOptionTypes eIndex) const; // Exposed to Python
	void changeHasCivicOptionCount(CivicOptionTypes eIndex, int iChange);

	int getNoCivicUpkeepCount(CivicOptionTypes eIndex) const;
	bool isNoCivicUpkeep(CivicOptionTypes eIndex) const; // Exposed to Python
	void changeNoCivicUpkeepCount(CivicOptionTypes eIndex, int iChange);

	int getHasReligionCount(ReligionTypes eIndex) const; // Exposed to Python
	int countTotalHasReligion() const; // Exposed to Python
	int findHighestHasReligionCount() const; // Exposed to Python
	void changeHasReligionCount(ReligionTypes eIndex, int iChange);

	int getHasCorporationCount(CorporationTypes eIndex) const; // Exposed to Python
	int countTotalHasCorporation() const; // Exposed to Python
	void changeHasCorporationCount(CorporationTypes eIndex, int iChange);
	bool isActiveCorporation(CorporationTypes eIndex) const;

	int getUpkeepCount(UpkeepTypes eIndex) const; // Exposed to Python
	void changeUpkeepCount(UpkeepTypes eIndex, int iChange);

	int getSpecialistValidCount(SpecialistTypes eIndex) const;
	bool isSpecialistValid(SpecialistTypes eIndex) const; // Exposed to Python

	CvProperties* getProperties();
	const CvProperties* getPropertiesConst() const;

	void changeSpecialistValidCount(SpecialistTypes eIndex, int iChange, bool bLimited = false);

	bool isResearchingTech(TechTypes eIndex) const; // Exposed to Python
	void setResearchingTech(TechTypes eIndex, bool bNewValue);

	CivicTypes getCivics(CivicOptionTypes eIndex) const; // Exposed to Python
	int getSingleCivicUpkeep(CivicTypes eCivic, bool bIgnoreAnarchy = false) const; // Exposed to Python
	int getCivicUpkeep(CivicTypes* paeCivics = NULL, bool bIgnoreAnarchy = false) const; // Exposed to Python
	void setCivics(CivicOptionTypes eIndex, CivicTypes eNewValue); // Exposed to Python

	int getExtraSpecialistYield(SpecialistTypes eIndex1, YieldTypes eIndex2) const; // Exposed to Python
	void changeExtraSpecialistYield(SpecialistTypes eIndex1, YieldTypes eIndex2, int iChange);

	int getImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const; // Exposed to Python
	void changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange);

	void updateGroupCycle(CvUnit* pUnit, bool bFarMove);
	CLLNode<int>* removeGroupCycle(int iID);
	CLLNode<int>* deleteGroupCycleNode(CLLNode<int>* pNode);
	CLLNode<int>* nextGroupCycleNode(CLLNode<int>* pNode) const;
	CLLNode<int>* previousGroupCycleNode(CLLNode<int>* pNode) const;
	CLLNode<int>* headGroupCycleNode() const;
	CLLNode<int>* tailGroupCycleNode() const;

	int findPathLength(TechTypes eTech, bool bCost = true) const; // Exposed to Python
	int getQueuePosition(TechTypes eTech) const; // Exposed to Python
	void clearResearchQueue(); // Exposed to Python
	bool pushResearch(TechTypes eTech, bool bClear = false); // Exposed to Python
	void popResearch(TechTypes eTech); // Exposed to Python
	int getLengthResearchQueue() const; // Exposed to Python
	CLLNode<TechTypes>* nextResearchQueueNode(CLLNode<TechTypes>* pNode) const;
	CLLNode<TechTypes>* headResearchQueueNode() const;
	CLLNode<TechTypes>* tailResearchQueueNode() const;

	void addCityName(const CvWString& szName); // Exposed to Python
	int getNumCityNames() const; // Exposed to Python
	CvWString getCityName(int iIndex) const; // Exposed to Python
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

	// deprecated, use city_iterator
	CvCity* firstCity(int* pIterIdx, bool bRev = false) const; // Exposed to Python
	// deprecated, use city_iterator
	CvCity* nextCity(int* pIterIdx, bool bRev = false) const; // Exposed to Python

	DllExport CvCity* firstCityExternal(int* pIterIdx, bool bRev = false) const; // Exposed to Python
	DllExport CvCity* nextCityExternal(int* pIterIdx, bool bRev = false) const; // Exposed to Python
	DllExport int getNumCities() const; // Exposed to Python
	DllExport CvCity* getCity(int iID) const; // Exposed to Python
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
	CvUnit* firstUnit(int* pIterIdx, bool bRev = false) const; // Exposed to Python
	// deprecated, use unit_range
	CvUnit* nextUnit(int* pIterIdx, bool bRev = false) const; // Exposed to Python
	DllExport CvUnit* firstUnitExternal(int* pIterIdx, bool bRev = false) const; // Exposed to Python
	DllExport CvUnit* nextUnitExternal(int* pIterIdx, bool bRev = false) const; // Exposed to Python
	DllExport int getNumUnits() const; // Exposed to Python
	CvUnit* getUnit(int iID) const; // Exposed to Python
	CvUnit* addUnit();
	void deleteUnit(int iID);

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
	CvSelectionGroup* firstSelectionGroup(int* pIterIdx, bool bRev = false) const; // Exposed to Python
	// deprecated, use group_range
	CvSelectionGroup* nextSelectionGroup(int* pIterIdx, bool bRev = false) const; // Exposed to Python
	CvSelectionGroup* firstSelectionGroupNonEmpty(int* pIterIdx, bool bRev = false) const; // Exposed to Python
	CvSelectionGroup* nextSelectionGroupNonEmpty(int* pIterIdx, bool bRev = false) const; // Exposed to Python

	int getNumSelectionGroups() const; // Exposed to Python

	CvSelectionGroup* getSelectionGroup(int iID) const; // Exposed to Python
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
	EventTriggeredData* getEventTriggered(int iID) const; // Exposed to Python
	EventTriggeredData* addEventTriggered();
	void deleteEventTriggered(int iID);
	EventTriggeredData* initTriggeredData(EventTriggerTypes eEventTrigger, bool bFire = false, int iCityId = -1, int iPlotX = INVALID_PLOT_COORD, int iPlotY = INVALID_PLOT_COORD, PlayerTypes eOtherPlayer = NO_PLAYER, int iOtherPlayerCityId = -1, ReligionTypes eReligion = NO_RELIGION, CorporationTypes eCorporation = NO_CORPORATION, int iUnitId = -1, BuildingTypes eBuilding = NO_BUILDING); // Exposed to Python
	int getEventTriggerWeight(EventTriggerTypes eTrigger) const; // Exposed to python
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

	int getScoreHistory(int iTurn) const; // Exposed to Python
	void updateScoreHistory(int iTurn, int iBestScore);

	int getEconomyHistory(int iTurn) const; // Exposed to Python
	void updateEconomyHistory(int iTurn, int iBestEconomy);
	int getIndustryHistory(int iTurn) const; // Exposed to Python
	void updateIndustryHistory(int iTurn, int iBestIndustry);
	int getAgricultureHistory(int iTurn) const; // Exposed to Python
	void updateAgricultureHistory(int iTurn, int iBestAgriculture);
	int getPowerHistory(int iTurn) const; // Exposed to Python
	void updatePowerHistory(int iTurn, int iBestPower);
	int getCultureHistory(int iTurn) const; // Exposed to Python
	void updateCultureHistory(int iTurn, int iBestCulture);
	int getEspionageHistory(int iTurn) const; // Exposed to Python
	void updateEspionageHistory(int iTurn, int iBestEspionage);

	int getRevolutionStabilityHistory(int iTurn) const; // Exposed to Python
	void updateRevolutionStabilityHistory(int iTurn, int m_iStabilityIndexAverage);

	// Script data needs to be a narrow string for pickling in Python
	std::string getScriptData() const; // Exposed to Python
	void setScriptData(std::string szNewValue); // Exposed to Python

	DllExport const CvString getPbemEmailAddress() const;
	DllExport void setPbemEmailAddress(const char* szAddress);
	DllExport const CvString getSmtpHost() const;
	void setSmtpHost(const char* szHost);

	const EventTriggeredData* getEventOccured(EventTypes eEvent, bool bIncludeExpiredEvents = false) const; // Exposed to python
	bool isTriggerFired(EventTriggerTypes eEventTrigger) const;
	void setEventOccured(EventTypes eEvent, const EventTriggeredData& kEventTriggered, bool bOthers = true);
	void resetEventOccured(EventTypes eEvent, bool bAnnounce = true); // Exposed to Python
	void setTriggerFired(const EventTriggeredData& kTriggeredData, bool bOthers = true, bool bAnnounce = true);
	void resetTriggerFired(EventTriggerTypes eEventTrigger);
	void trigger(EventTriggerTypes eEventTrigger); // Exposed to Python
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
	bool assimilatePlayer(PlayerTypes ePlayer); // Exposed to Python

	void launch(VictoryTypes victoryType);

	bool hasShrine(ReligionTypes eReligion);
	int getVotes(VoteTypes eVote, VoteSourceTypes eVoteSource) const; // Exposed to Python
	void processVoteSourceBonus(VoteSourceTypes eVoteSource, bool bActive);
	bool canDoResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData) const;
	bool canDefyResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData) const;
	void setDefiedResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData);
	void setEndorsedResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData);
	bool isFullMember(VoteSourceTypes eVoteSource) const; // Exposed to Python
	bool isVotingMember(VoteSourceTypes eVoteSource) const; // Exposed to Python

	void invalidatePopulationRankCache();
	void invalidateYieldRankCache(YieldTypes eYield = NO_YIELD);
	void invalidateCommerceRankCache(CommerceTypes eCommerce = NO_COMMERCE);

	PlayerTypes pickConqueredCityOwner(const CvCity& kCity) const;
	bool canHaveTradeRoutesWith(PlayerTypes ePlayer) const;

	void forcePeace(PlayerTypes ePlayer); // exposed to Python

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

	int getWorldTradeRoutes() const;
	void changeWorldTradeRoutes(int iChange);

	int getProjectHealth() const;
	void changeProjectHealth(int iChange);

	int getNoCapitalUnhappiness() const;
	void changeNoCapitalUnhappiness(int iChange);

	int getTaxationAnger() const;
	void changeTaxationAnger(int iChange);

	int getLastTurnTaxRate() const;
	void changeLastTurnTaxRate(int iChange);

	int getCivilizationHealth() const;
	void changeCivilizationHealth(int iChange);

	int getTaxRateUnhappiness() const;
	void changeTaxRateUnhappiness(int iChange);
	int calculateTaxRateUnhappiness() const;

	int getReligionSpreadRate() const;
	void setReligionSpreadRate(int iNewValue);
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

	int getCityLimit() const;
	void changeCityLimit(int iChange);

	int getCityOverLimitUnhappy() const;
	void changeCityOverLimitUnhappy(int iChange);

	int getForeignUnhappyPercent() const;
	void changeForeignUnhappyPercent(int iChange);

	bool isShowLandmarks() const; //Exposed to Python
	void setShowLandmarks(bool bNewVal); //Exposed to Python

	int getTerrainYieldChange(TerrainTypes eIndex1, YieldTypes eIndex2) const;
	void changeTerrainYieldChange(TerrainTypes eIndex1, YieldTypes eIndex2, int iChange);

	bool hasValidCivics(BuildingTypes eBuilding) const;

	int getSevoWondersScore(int mode);

	int getForeignTradeRouteModifier() const;
	void changeForeignTradeRouteModifier(int iChange);
	int getCivicHappiness() const;
	void changeCivicHappiness(int iChange);

	/*bool hasFixedBorders();*/
	void doCheckForTaxationAnger();
	bool hasEnemyDefenderUnit(const CvPlot* pPlot) const;

	CvCity* getBestHQCity(CorporationTypes eCorporation) const;
	PlayerVoteTypes getPledgedVote() const;
	void setPledgedVote(PlayerVoteTypes eIndex);
	TeamTypes getPledgedSecretaryGeneralVote() const;
	void setPledgedSecretaryGeneralVote(TeamTypes eIndex);
	int getUnitCombatProductionModifier(UnitCombatTypes eIndex) const;
	void changeUnitCombatProductionModifier(UnitCombatTypes eIndex, int iChange);

	int getUnitCombatFreeExperience(UnitCombatTypes eIndex) const;
	void changeUnitCombatFreeExperience(UnitCombatTypes eIndex, int iChange);

	int getBuildingProductionModifier(BuildingTypes eIndex) const;
	void changeBuildingProductionModifier(BuildingTypes eIndex, int iChange);
	int getBuildingCostModifier(BuildingTypes eIndex) const;
	void changeBuildingCostModifier(BuildingTypes eIndex, int iChange);

	int getUnitProductionModifier(UnitTypes eIndex) const;
	void changeUnitProductionModifier(UnitTypes eIndex, int iChange);

	bool isAutomatedCanBuild(BuildTypes eBuild) const; //Exposed to Python
	void setAutomatedCanBuild(BuildTypes eBuild, bool bNewValue); //Exposed to Python

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

	int processedNationalCulture() const;

	void acquireFort(CvPlot* pPlot);

	int getResourceConsumption(BonusTypes eBonus) const;
	void recalculateResourceConsumption(BonusTypes eBonus);
	void recalculateAllResourceConsumption();

	std::vector<CvUnit*> Commanders;

	int getFreeSpecialistCount(SpecialistTypes eIndex) const;
	void setFreeSpecialistCount(SpecialistTypes eIndex, int iNewValue);
	void changeFreeSpecialistCount(SpecialistTypes eIndex, int iChange);

	bool hasValidBuildings(TechTypes eTech) const; //Exposed to Python

	void checkAIStrategy();

	void setCityCommerceModifierDirty(CommerceTypes eCommerce);

	int getBuildingCommerceModifier(BuildingTypes eIndex1, CommerceTypes eIndex2) const;
	void changeBuildingCommerceModifier(BuildingTypes eIndex1, CommerceTypes eIndex2, int iChange);

	int getBuildingCommerceChange(BuildingTypes building, CommerceTypes CommerceType) const;
	void changeBuildingCommerceChange(BuildingTypes building, CommerceTypes CommerceType, int iChange);

	int getBonusCommerceModifier(BonusTypes eIndex1, CommerceTypes eIndex2) const; //Exposed to Python
	void changeBonusCommerceModifier(BonusTypes eIndex1, CommerceTypes eIndex2, int iChange);

	int getLandmarkYield(YieldTypes eIndex) const;
	void changeLandmarkYield(YieldTypes eIndex, int iChange);

	int getNoLandmarkAngerCount() const;
	bool isNoLandmarkAnger() const;
	void changeNoLandmarkAngerCount(int iChange);

	int getLandmarkHappiness() const;
	void changeLandmarkHappiness(int iChange);

	int getBuildingCountWithUpgrades(BuildingTypes eBuilding) const;

	void setColor(PlayerColorTypes eColor);

	void setHandicap(int iNewVal);

	bool canBuild(const CvPlot* pPlot, ImprovementTypes eImprovement, bool bTestEra, bool bTestVisible) const;

	int getModderOption(ModderOptionTypes eIndex) const; // Exposed to Python
	bool isModderOption(ModderOptionTypes eIndex) const; // Exposed to Python
	void setModderOption(ModderOptionTypes eIndex, bool bNewValue); // Exposed to Python
	void setModderOption(ModderOptionTypes eIndex, int iNewValue); // Exposed to Python

	int getCorporationSpreadModifier() const;
	void changeCorporationSpreadModifier(int iChange);

	int getCorporateTaxIncome() const;
	void changeCorporateTaxIncome(int iChange);

	int getCorporationInfluence(CorporationTypes eIndex) const;
	int getEnvironmentalProtection() const;
	int getLaborFreedom() const;

	void doTaxes();

	bool m_bChoosingReligion;

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

	void updateCache();

	void clearTileCulture();
	void clearCityCulture();

	int getBonusMintedPercent(BonusTypes eIndex) const;
	void changeBonusMintedPercent(BonusTypes eIndex, int iChange);

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
	inline int	getZobristValue() const { return m_zobristValue; }

	inline bool getTurnHadUIInteraction() const { return m_turnHadUIInteraction; }
	inline void setTurnHadUIInteraction(bool newVal) { m_turnHadUIInteraction = newVal; }

protected:
	int** m_ppiSpecialistYieldPercentChanges;
	int** m_ppiSpecialistCommercePercentChanges;
	int** m_ppaaiTerrainYieldChange;
	int** m_ppiBuildingCommerceModifier;
	int** m_ppiBuildingCommerceChange;
	int** m_ppiBonusCommerceModifier;
	int* m_paiUnitCombatProductionModifier;
	int* m_paiBuildingProductionModifier;
	int* m_paiBuildingCostModifier;
	int* m_paiUnitProductionModifier;
	int* m_paiBonusMintedPercent;
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
	int m_iLastTurnTaxRate;
	int m_iTaxationAnger;
	int m_iNoCapitalUnhappiness;
	int m_iProjectHealth;
	int m_iWorldTradeRoutes;
	int m_iForceAllTradeRoutes;
	int m_iProjectHappiness;
	int m_iWorldHealth;
	int m_iWorldHappiness;
	float m_fPopulationgrowthratepercentageLog;
	int m_iCorporationSpreadModifier;
	int m_iCorporateTaxIncome;
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
	int m_iTraitDisplayCount;
	int m_iNationalEspionageDefense;
	int m_iInquisitionCount;
	int m_iCompatCheckCount;
	int* m_paiNationalGreatPeopleUnitRate;
	int* m_paiUnitCombatClassDisplayCount;
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
	int* m_paiGoldenAgeOnBirthOfGreatPersonCount;
	int* m_paiGreatGeneralPointsForType;
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

	int* m_paiUnitCombatFreeExperience;

	int m_iExtraNationalCaptureProbabilityModifier;
	int m_iExtraNationalCaptureResistanceModifier;

	int m_iExtraStateReligionSpreadModifier;
	int m_iExtraNonStateReligionSpreadModifier;
	//TB Traits
	int m_iBaseMergeSelection;
	int m_iFirstMergeSelection;
	int m_iSecondMergeSelection;
	int m_iSplittingUnit;
	int m_iArrestingUnit;
	int m_iArrestedUnit;
	int m_iAmbushingUnit;
	bool m_bAssassinate;

	int m_iBuildingInflation;
	int m_iProjectInflation;
	int m_iTechInflation;
	int m_iCivicInflation;
	int m_iHurryCostModifier;
	int m_iHurryInflationModifier;
	int m_iHurryCount;
	mutable int m_accruedCostRatioTimes10000; //	Inflation effects accrue to this ratio to start-of-game cost

	int m_iFractionalCombatExperience;

	TeamTypes m_eDemandWarAgainstTeam;

	CvProperties m_Properties;
	CvBuildingList m_BuildingList;
	CvUnitList m_UnitList;

	int	m_zobristValue;

	bool m_turnHadUIInteraction;

public:
	void verifyUnitStacksValid();
	UnitTypes getTechFreeUnit(TechTypes eTech) const;
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

	virtual void AI_conquerCity(CvCity* pCity) = 0;
	virtual int AI_foundValue(int iX, int iY, int iMinUnitRange = -1, bool bStartingLoc = false) const = 0; // Exposed to Python
	virtual bool AI_isCommercePlot(const CvPlot* pPlot) const = 0;
	virtual int AI_getPlotDanger(const CvPlot* pPlot, int iRange = -1, bool bTestMoves = true) const = 0;
	virtual bool AI_isFinancialTrouble() const = 0; // Exposed to Python
	virtual TechTypes AI_bestTech(int iMaxPathLength = 1, bool bIgnoreCost = false, bool bAsync = false, TechTypes eIgnoreTech = NO_TECH, AdvisorTypes eIgnoreAdvisor = NO_ADVISOR) const = 0;
	virtual void AI_chooseFreeTech() = 0;
	virtual void AI_chooseResearch() = 0;
	virtual bool AI_isWillingToTalk(PlayerTypes ePlayer) const = 0;
	virtual bool AI_demandRebukedSneak(PlayerTypes ePlayer) const = 0;
	virtual bool AI_demandRebukedWar(PlayerTypes ePlayer) const = 0; // Exposed to Python
	virtual AttitudeTypes AI_getAttitude(PlayerTypes ePlayer, bool bForced = true) const = 0; // Exposed to Python
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

	virtual int AI_unitValue(UnitTypes eUnit, UnitAITypes eUnitAI, const CvArea* pArea, CvUnitSelectionCriteria* criteria = NULL) const = 0; // Exposed to Python
	virtual int AI_totalUnitAIs(UnitAITypes eUnitAI) const = 0; // Exposed to Python
	virtual int AI_totalAreaUnitAIs(const CvArea* pArea, UnitAITypes eUnitAI) const = 0; // Exposed to Python
	virtual int AI_totalWaterAreaUnitAIs(const CvArea* pArea, UnitAITypes eUnitAI) const = 0; // Exposed to Python
	virtual int AI_plotTargetMissionAIs(CvPlot* pPlot, MissionAITypes eMissionAI, const CvSelectionGroup* pSkipSelectionGroup = NULL, int iRange = 0, int* piClosest = NULL) const = 0;
	virtual int AI_unitTargetMissionAIs(const CvUnit* pUnit, MissionAITypes eMissionAI, const CvSelectionGroup* pSkipSelectionGroup = NULL) const = 0;

	virtual int AI_civicValue(CivicTypes eCivic, bool bCivicOptionVacuum = false, CivicTypes* paeSelectedCivics = NULL) const = 0; // Exposed to Python

	virtual int AI_getNumAIUnits(UnitAITypes eIndex) const = 0; // Exposed to Python
	virtual void AI_changePeacetimeTradeValue(PlayerTypes eIndex, int iChange) = 0;
	virtual void AI_changePeacetimeGrantValue(PlayerTypes eIndex, int iChange) = 0;
	virtual int AI_getAttitudeExtra(PlayerTypes eIndex) const = 0; // Exposed to Python
	virtual void AI_setAttitudeExtra(PlayerTypes eIndex, int iNewValue) = 0; // Exposed to Python
	virtual void AI_changeAttitudeExtra(PlayerTypes eIndex, int iChange) = 0; // Exposed to Python
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

	int m_iStartingX;
	int m_iStartingY;
	int m_iTotalPopulation;
	int m_iTotalLand;
	int m_iTotalLandScored;
	int m_iGold;
	int m_iGreaterGold;
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
	bool m_bPopBad;
	int m_iUnitUpgradePriceModifier;
	int m_iNationalGreatPeopleRate;

	int m_iNumNukeUnits;
	int m_iNumOutsideUnits;
	int m_iBaseFreeUnits;
	int m_iBaseFreeMilitaryUnits;
	int m_iFreeUnitsPopulationPercent;
	int m_iFreeMilitaryUnitsPopulationPercent;
	int m_iGoldPerUnit;
	int m_iGoldPerMilitaryUnit;
	int m_iExtraUnitCost;
	int m_iNumMilitaryUnits;
	int m_iNumUnitPercentCountForCostAdjustment;
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
	mutable int m_iTotalMaintenance;
	int m_iUpkeepModifier;
	int m_iLevelExperienceModifier;
	int m_iExtraHealth;
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

	int m_iFreeUnitCountdown;

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

	bool m_bChoosingFreeTech;

	PlayerTypes m_eID;
	LeaderHeadTypes m_ePersonalityType;
	EraTypes m_eCurrentEra;
	ReligionTypes m_eLastStateReligion;
	PlayerTypes m_eParent;
	TeamTypes m_eTeamType;

	// Members for culture accumulation
	int m_iCulture;
	int m_iGreaterCulture;

	int m_iUpgradeRoundCount;
	int m_iSelectionRegroup;

	int* m_aiSeaPlotYield;
	int* m_aiYieldRateModifier;
	int* m_aiCapitalYieldRateModifier;
	int* m_aiExtraYieldThreshold;
	int* m_aiTradeYieldModifier;
	int* m_aiFreeCityCommerce;
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

	int* m_paiBonusExport;
	int* m_paiBonusImport;
	int* m_paiImprovementCount;
	int* m_paiFreeBuildingCount;
	int* m_paiExtraBuildingHappiness;
	int* m_paiExtraBuildingHealth;
	int** m_paiExtraBuildingYield;
	int** m_paiExtraBuildingCommerce;
	int* m_paiFeatureHappiness;
	int* m_paiBuildingCount;
	int* m_paiUnitCount;
	int* m_paiUnitMaking;
	int* m_paiBuildingGroupCount;
	int* m_paiBuildingMaking;
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

	CLinkList<int> m_groupCycle;

	CLinkList<TechTypes> m_researchQueue;

	CLinkList<CvWString> m_cityNames;

	FFreeListTrashArray<CvPlotGroup> m_plotGroups;

	FFreeListTrashArray<CvCityAI> m_cities;

	FFreeListTrashArray<CvUnitAI> m_units;

	FFreeListTrashArray<CvSelectionGroupAI> m_selectionGroups;

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
	bool isValidTriggerReligion(const CvEventTriggerInfo& kTrigger, CvCity* pCity, ReligionTypes eReligion) const;
	bool isValidTriggerCorporation(const CvEventTriggerInfo& kTrigger, CvCity* pCity, CorporationTypes eCorporation) const;
	CvCity* pickTriggerCity(EventTriggerTypes eTrigger) const;
	CvUnit* pickTriggerUnit(EventTriggerTypes eTrigger, CvPlot* pPlot, bool bPickPlot) const;
	bool isValidEventTech(TechTypes eTech, EventTypes eEvent, PlayerTypes eOtherPlayer) const;
	void recalculatePopulationgrowthratepercentage();

	RouteTypes getBestRouteInternal(CvPlot* pPlot, bool bConnect, CvUnit* pBuilder, BuildTypes* eBestRouteBuild = NULL) const; // Exposed to Python
	bool isRouteValid(RouteTypes eRoute, BuildTypes eRouteBuild, CvPlot* pPlot, CvUnit* pBuilder) const;

	void verifyGoldCommercePercent();

	void processCivics(CivicTypes eCivic, int iChange, bool bLimited = false);

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
	void getDebugLayerColors(GlobeLayerResourceOptionTypes eOption, std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const; // used by Globeview resource layer

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

	int getSpecialistExtraYield(YieldTypes eIndex) const; // Exposed to Python
	void changeSpecialistExtraYield(YieldTypes eIndex, int iChange);

	int getFreeCityYield(YieldTypes eIndex) const;
	void changeFreeCityYield(YieldTypes eIndex, int iChange);

	int getTraitExtraCityDefense() const;
	void setTraitExtraCityDefense(int iNewValue);
	void changeTraitExtraCityDefense(int iChange);

	void setHasTrait(TraitTypes eIndex, bool bNewValue);
	bool canLearnTrait(TraitTypes eIndex, bool isSelectingNegative = false);
	bool canUnlearnTrait(TraitTypes eTrait, bool bPositive);

	int getLeaderHeadLevel() const;
	void setLeaderHeadLevel(int iValue);
	void changeLeaderHeadLevel(int iChange);

	int getLeaderLevelupNextCultureTotal(int& iGreaterCultureReq);
	int getLeaderLevelupCultureToEarn(int& iGreaterCultureReq);

	bool canLeaderPromote();
	void doPromoteLeader();
	void clearLeaderTraits();

	int getTraitDisplayCount() const;
	void setTraitDisplayCount(int iNewValue);
	void changeTraitDisplayCount(int iChange);

	int getUnitCombatClassDisplayCount(UnitTypes eIndex) const;
	void setUnitCombatClassDisplayCount(UnitTypes eIndex, int iValue);
	void changeUnitCombatClassDisplayCount(UnitTypes eIndex, int iChange);

	int getNationalEspionageDefense() const;
	void setNationalEspionageDefense(int iNewValue);
	void changeNationalEspionageDefense(int iChange);

	int getInquisitionCount() const;
	void setInquisitionCount(int iNewValue);
	void changeInquisitionCount(int iChange);

	int getNationalGreatPeopleUnitRate(UnitTypes eIndex) const;
	void setNationalGreatPeopleUnitRate(UnitTypes eIndex, int iNewValue);
	void changeNationalGreatPeopleUnitRate(UnitTypes eIndex, int iChange);

	int getNationalGreatPeopleRate() const;
	void setNationalGreatPeopleRate(int iNewValue);
	void changeNationalGreatPeopleRate(int iChange);

	int getMaxTradeRoutesAdjustment() const;
	void setMaxTradeRoutesAdjustment(int iNewValue);
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

	int getCoastalAIInfluence();

	int getEraAdvanceFreeSpecialistCount(SpecialistTypes eIndex) const;
	void setEraAdvanceFreeSpecialistCount(SpecialistTypes eIndex, int iValue);
	void changeEraAdvanceFreeSpecialistCount(SpecialistTypes eIndex, int iChange);

	int getGoldenAgeOnBirthOfGreatPersonCount(UnitTypes eIndex) const;
	void setGoldenAgeOnBirthOfGreatPersonCount(UnitTypes eIndex, int iValue);
	void changeGoldenAgeOnBirthOfGreatPersonCount(UnitTypes eIndex, int iChange);

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

	void updateTechHappinessandHealth();
	void checkReligiousDisablingAllBuildings();
	bool isBuildingtoDisplayReligiouslyDisabled(BuildingTypes eBuilding);

	void doGoldenAgebyPercentage(int iPercent);
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

	int getArrestingUnit() const;
	void setArrestingUnit(int iNewValue);
	int getArrestedUnit() const;
	void setArrestedUnit(int iNewValue);

	int getAmbushingUnit() const;
	bool isAssassinate() const;
	void setAmbushingUnit(int iNewValue, bool bAssassinate = false);

	int getGreatGeneralPointsForType(UnitTypes eIndex) const;
	void setGreatGeneralPointsForType(UnitTypes eIndex, int iValue);
	void changeGreatGeneralPointsForType(UnitTypes eIndex, int iChange);

	void setGreatGeneralTypetoAssign();
	UnitTypes getGreatGeneralTypetoAssign() const;
	void setSMValues();

	void upgradePlotPopup(ImprovementTypes eImprovement, int iX, int iY);
	void upgradePlot(int iX, int iY, ImprovementTypes eImprovement, bool bConfirm);

	void setFocusPlots(int iX, int iY);
	int getFocusPlotX() const;
	int getFocusPlotY() const;

public:
	void RecalculatePlotGroupHashes();
	CvContractBroker& getContractBroker();

	void addPlotDangerSource(CvPlot* pPlot, int iStrength);

	void clearModifierTotals();
	void recalculateModifiers();

	void addPropertiesAllCities(CvProperties* pProp);
	void subtractPropertiesAllCities(CvProperties* pProp);

	bool canHaveBuilder(BuildTypes eBuild) const;
	//TB Nukefix
	bool isNukesValid() const; // Exposed to Python
	void makeNukesValid(bool bValid = true); // Exposed to Python

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

#ifdef _DEBUG
	void ValidatePlotGroup(CvPlot* plot, CvPlotGroup* group);
#endif

private:
	int m_iNumAnimalsSubdued;
	std::map<BuildingTypes, int> m_unitConstructionCounts;
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

	static CRITICAL_SECTION	c_canConstructCacheSection;
	static CRITICAL_SECTION	c_allCitiesPropertySection;
	static CRITICAL_SECTION	c_buildingProcessingSection;
	static CRITICAL_SECTION	c_GroupCycleSection;
	static bool m_staticsInitialized;

	bool m_bUpdatesDeferred;
	bool m_bGoldenAgeStarted; // Used to defer reporting in update-deferred sections

	void reportGoldenAgeStart();
	void deferUpdates();
	void resumeUpdates();

protected:
	void constructTechPathSet(TechTypes eTech, std::vector<techPath*>& pathSet, techPath& rootPath) const;
	void clearCanConstructCache(BuildingTypes building, bool bIncludeCities = false) const;
	void clearCanConstructCacheForGroup(SpecialBuildingTypes eSpecialBuilding, bool bIncludeCities = false) const;

public:
	int getPlayerWideAfflictionCount(PromotionLineTypes ePromotionLineType) const;
	void changePlayerWideAfflictionCount(PromotionLineTypes ePromotionLineType, int iChange);
	void setPlayerWideAfflictionCount(PromotionLineTypes ePromotionLineType, int iChange);
	int countAfflictedUnits(PromotionLineTypes eAfflictionLine);
	void recalculateAfflictedUnitCount();
};

#endif
