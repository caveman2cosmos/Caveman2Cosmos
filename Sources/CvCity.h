#pragma once

// city.h

#ifndef CIV4_CITY_H
#define CIV4_CITY_H

//#include "CvDLLEntity.h"
#include "LinkedList.h"
#include "CvBuildingList.h"
#include "CvUnitList.h"
#include "CvGameObject.h"

class CvArea;
class CvArtInfoBuilding;
class CvDLLEntity;
class CvPlot;
class CvPlotGroup;

// BUG - start
void addGoodOrBad(int iValue, int& iGood, int& iBad);
void subtractGoodOrBad(int iValue, int& iGood, int& iBad);
// BUG - end

//	Unit ordering criteria
class CvUnitSelectionCriteria
{
public:
	CvUnitSelectionCriteria()
	:
		m_eUnitAI(NO_UNITAI),
		m_eIgnoreAdvisor(NO_ADVISOR),
		m_eHealUnitCombat(NO_UNITCOMBAT),
		m_eProperty(NO_PROPERTY),
		m_eVisibility(NO_INVISIBLE),
		m_bIgnoreNotUnitAIs(false),
		m_bIgnoreGrowth(false),
		m_bPropertyBeneficial(true),
		m_bNoNegativeProperties(false),
		m_bIsHealer(false),
		m_bIsCommander(false)
	{ }

	CvString getDescription()
	{
		if (m_eProperty != NO_PROPERTY)
		{
			return (
				CvString::format(
					"Property %s for %S",
					m_bPropertyBeneficial ? "improver" : "worsener",
					GC.getPropertyInfo(m_eProperty).getDescription()
				)
			);
		}
		return CvString("");
	}

#define PROPERTY_MASK 0x0F
#define ADVISOR_MASK 0x07

	int getHash() const
	{
		int	iResult = (m_eUnitAI << 16);

		if ( m_eProperty != NO_PROPERTY )
		{
			iResult |= 0x8000;
			iResult |= (m_eProperty & PROPERTY_MASK) << 11;
			if ( !m_bPropertyBeneficial )
			{
				iResult |= 0x400;
			}
		}
		if ( m_bIgnoreNotUnitAIs )
		{
			iResult |= 0x200;
		}
		if ( m_bIgnoreGrowth )
		{
			iResult |= 0x100;
		}
		iResult |= (m_eIgnoreAdvisor & ADVISOR_MASK) << 5;
		if ( m_bIsHealer )
		{
			iResult |= 0x10;
		}
		if ( m_bIsCommander )
		{
			iResult |= 0x8;
		}

		return iResult;
	}

	UnitAITypes		m_eUnitAI;
	AdvisorTypes	m_eIgnoreAdvisor;
	UnitCombatTypes	m_eHealUnitCombat;
	PropertyTypes	m_eProperty;
	InvisibleTypes	m_eVisibility;
	bool			m_bIgnoreNotUnitAIs;
	bool			m_bIgnoreGrowth;
	bool			m_bPropertyBeneficial;
	bool			m_bNoNegativeProperties;
	bool			m_bIsHealer;
	bool			m_bIsCommander;

	CvUnitSelectionCriteria& UnitAI(UnitAITypes UnitAI) { m_eUnitAI = UnitAI; return *this; }
	CvUnitSelectionCriteria& IgnoreAdvisor(AdvisorTypes IgnoreAdvisor) { m_eIgnoreAdvisor = IgnoreAdvisor; return *this; }
	CvUnitSelectionCriteria& HealUnitCombat(UnitCombatTypes HealUnitCombat) { m_eHealUnitCombat = HealUnitCombat; return *this; }
	CvUnitSelectionCriteria& Property(PropertyTypes Property) { m_eProperty = Property; return *this; }
	CvUnitSelectionCriteria& Visibility(InvisibleTypes Visibility) { m_eVisibility = Visibility; return *this; }
	CvUnitSelectionCriteria& IgnoreNotUnitAIs(bool IgnoreNotUnitAIs) { m_bIgnoreNotUnitAIs = IgnoreNotUnitAIs; return *this; }
	CvUnitSelectionCriteria& IgnoreGrowth(bool IgnoreGrowth) { m_bIgnoreGrowth = IgnoreGrowth; return *this; }
	CvUnitSelectionCriteria& PropertyBeneficial(bool PropertyBeneficial) { m_bPropertyBeneficial = PropertyBeneficial; return *this; }
	CvUnitSelectionCriteria& NoNegativeProperties(bool NoNegativeProperties) { m_bNoNegativeProperties = NoNegativeProperties; return *this; }
	CvUnitSelectionCriteria& IsHealer(bool IsHealer) { m_bIsHealer = IsHealer; return *this; }
	CvUnitSelectionCriteria& IsCommander(bool IsCommander) { m_bIsCommander = IsCommander; return *this; }
};

struct ProductionCalc
{
	enum flags {
		None = 0,
		FoodProduction = 1 << 0,
		Overflow = 1 << 1,
		Yield = 1 << 2
	};
};

DECLARE_FLAGS(ProductionCalc::flags);


class CvCity : public CvDLLEntity
{
public:
	CvCity();
	virtual ~CvCity();


	void init(int iID, PlayerTypes eOwner, int iX, int iY, bool bBumpUnits, bool bUpdatePlotGroups);
	void uninit();
	void reset(int iID = 0, PlayerTypes eOwner = NO_PLAYER, int iX = 0, int iY = 0, bool bConstructorCall = false);
	void setupGraphical();

	CvGameObjectCity* getGameObject() { return &m_GameObject; }
	const CvGameObjectCity* getGameObject() const { return &m_GameObject; }

private:
	// disable copy: we have owned pointers so we can't use the default copy implementation
	CvCity(const CvCity&);
	CvCity& operator=(const CvCity&);
	bool canHurryInternal(const HurryTypes eHurry) const;

protected:
	CvGameObjectCity m_GameObject;

public:
	int getRevolutionIndex() const;
	void setRevolutionIndex( int iNewValue );
	void changeRevolutionIndex( int iChange );

	int getLocalRevIndex() const;
	void setLocalRevIndex( int iNewValue );
	void changeLocalRevIndex( int iChange );

	int getRevIndexAverage() const;
	void setRevIndexAverage( int iNewValue );
	void updateRevIndexAverage( );

	int getRevolutionCounter() const;
	void setRevolutionCounter( int iNewValue );
	void changeRevolutionCounter( int iChange );

	int getReinforcementCounter() const;
	void setReinforcementCounter( int iNewValue );
	void changeReinforcementCounter( int iChange );

	bool isRecentlyAcquired() const;

	void kill(bool bUpdatePlotGroups, bool bUpdateCulture = true);
	void killTestCheap(); // For testing, do not call in a game situation

	void doTurn();
	void doAutobuild();

	bool isCitySelected() const;
	DllExport bool canBeSelected() const;
	DllExport void updateSelectedCity(bool bTestProduction);

	void updateYield();

	void updateVisibility();
	bool isVisibilitySetup() const;

	void createGreatPeople(UnitTypes eGreatPersonUnit, bool bIncrementThreshold, bool bIncrementExperience);

	void doTask(TaskTypes eTask, int iData1 = -1, int iData2 = -1, bool bOption = false, bool bAlt = false, bool bShift = false, bool bCtrl = false);

	void chooseProduction(UnitTypes eTrainUnit = NO_UNIT, BuildingTypes eConstructBuilding = NO_BUILDING, ProjectTypes eCreateProject = NO_PROJECT, bool bFinish = false, bool bFront = false);

	// Base iterator type for iterating over city plots, returning valid ones only
	template < class Value_ >
	struct city_plot_iterator_base :
		public bst::iterator_facade<city_plot_iterator_base<Value_>, Value_*, bst::forward_traversal_tag, Value_*>
	{
		city_plot_iterator_base() : m_centerX(-1), m_centerY(-1), m_curr(nullptr), m_idx(0), m_numPlots(0) {}
		explicit city_plot_iterator_base(int centerX, int centerY, int numPlots, int idx) : m_centerX(centerX), m_centerY(centerY), m_numPlots(numPlots), m_curr(nullptr), m_idx(idx)
		{
			increment();
		}

	private:
		friend class bst::iterator_core_access;
		void increment()
		{
			m_curr = nullptr;
			while (m_curr == nullptr && m_idx < m_numPlots/*NUM_CITY_PLOTS*/)
			{
				m_curr = plotCity(m_centerX, m_centerY, m_idx);
				++m_idx;
			}
		}
		bool equal(city_plot_iterator_base const& other) const
		{
			return (this->m_centerX == other.m_centerX
				&& this->m_centerY == other.m_centerY
				&& this->m_idx == other.m_idx)
				|| (this->m_curr == nullptr && other.m_curr == nullptr);
		}

		Value_* dereference() const { return m_curr; }

		const int m_centerX;
		const int m_centerY;
		const int m_numPlots;
		Value_* m_curr;
		int m_idx;
	};
	typedef city_plot_iterator_base<CvPlot> city_plot_iterator;

	city_plot_iterator beginPlots(int numPlots, bool skipCityHomePlot) const { return city_plot_iterator(getX(), getY(), numPlots, skipCityHomePlot); }
	city_plot_iterator endPlots() const { return city_plot_iterator(); }

	typedef bst::iterator_range<city_plot_iterator> city_plot_range;
 	city_plot_range plots(int numPlots, bool skipCityHomePlot = false) const { return city_plot_range(beginPlots(numPlots, skipCityHomePlot), endPlots()); }
	city_plot_range plots(bool skipCityHomePlot = false) const { return city_plot_range(beginPlots(getNumCityPlots(), skipCityHomePlot), endPlots()); }

	int getCityPlotIndex(const CvPlot* pPlot) const;
	// Prefer to use plots() range instead of this for loops, searching etc.
	CvPlot* getCityIndexPlot(int iIndex) const;

	bool canWork(const CvPlot* pPlot) const;
	void verifyWorkingPlot(int iIndex);
	void verifyWorkingPlots();
	void clearWorkingOverride(int iIndex);
	int countNumImprovedPlots(ImprovementTypes eImprovement = NO_IMPROVEMENT, bool bPotential = false) const;
	int countNumWaterPlots() const;
	int countNumRiverPlots() const;

	int findPopulationRank() const;
	int findBaseYieldRateRank(YieldTypes eYield) const;
	int findYieldRateRank(YieldTypes eYield) const;
	int findCommerceRateRank(CommerceTypes eCommerce) const;

	bool isSupersedingUnitAvailable(UnitTypes eUnit) const;
	bool isPlotTrainable(UnitTypes eUnit, bool bContinue, bool bTestVisible) const;

	UnitTypes allUpgradesAvailable(UnitTypes eUnit, int iUpgradeCount = 0) const;
	bool isWorldWondersMaxed() const;
	bool isTeamWondersMaxed() const;
	bool isNationalWondersMaxed() const;
	int getMaxNumWorldWonders() const;
	int getMaxNumTeamWonders() const;
	int getMaxNumNationalWonders() const;

	bool canTrain(UnitTypes eUnit, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, bool bIgnoreUpgrades = false, bool bPropertySpawn = false) const;
	bool canTrainInternal(UnitTypes eUnit, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, bool bIgnoreUpgrades = false) const;
	bool canTrain(UnitCombatTypes eUnitCombat) const;

	bool canConstruct(BuildingTypes eBuilding, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, bool bIgnoreAmount = false, bool bIgnoreBuildings = false, TechTypes eIgnoreTechReq = NO_TECH, int* probabilityEverConstructable = NULL, bool bAffliction = false, bool bExposed = false) const;

	//	KOSHLING - cache can build results
	void FlushCanConstructCache(BuildingTypes eBuilding = NO_BUILDING);
	bool canConstructInternal(BuildingTypes eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreAmount, BuildingTypes withExtraBuilding = NO_BUILDING, bool bIgnoreBuildings = false, TechTypes eIgnoreTechReq = NO_TECH, int* probabilityEverConstructable = NULL, bool bAffliction = false, bool bExposed = false) const;
	void NoteBuildingNoLongerConstructable(BuildingTypes eBuilding) const;

	bool canCreate(ProjectTypes eProject, bool bContinue = false, bool bTestVisible = false) const;
	bool canMaintain(ProcessTypes eProcess) const;

	int getFoodTurnsLeft() const;
	bool isProduction() const;
	bool isProductionLimited() const;
	bool isProductionUnitCombat(int iIndex) const;
	bool isProductionUnit() const;
	bool isProductionBuilding() const;
	bool isProductionProject() const;
	bool isProductionProcess() const;

	bool canContinueProduction(const OrderData& order) const;
	int getProductionExperience(UnitTypes eUnit = NO_UNIT) const;
	void addProductionExperience(CvUnit* pUnit, bool bConscript = false);

	UnitTypes getProductionUnit() const;
	UnitAITypes getProductionUnitAI() const;
	BuildingTypes getProductionBuilding() const;
	ProjectTypes getProductionProject() const;
	ProcessTypes getProductionProcess() const;
	const wchar_t* getProductionName() const;
	const wchar_t* getProductionNameKey() const;
	int getGeneralProductionTurnsLeft() const;

	bool isFoodProduction() const;
	bool isFoodProduction(const OrderData& order) const;
	bool isFoodProduction(UnitTypes eUnit) const;

	int getFirstUnitOrder(UnitTypes eUnit) const;
	int getFirstBuildingOrder(BuildingTypes eBuilding) const;
	int getFirstProjectOrder(ProjectTypes eProject) const;
	int getNumTrainUnitAI(UnitAITypes eUnitAI) const;

	int getProduction() const;
	int getProductionNeeded() const;
	int getProductionNeeded(const OrderData& order) const;
	int getProductionNeeded(UnitTypes eUnit) const;
	int getProductionNeeded(BuildingTypes eBuilding) const;
	int getProductionNeeded(ProjectTypes eProject) const;
	int getOrderProductionTurnsLeft(const OrderData& order, int iIndex = 0) const;

	// For fractional production calculations:
	int getTotalProductionQueueTurnsLeft() const;
	int getProductionTurnsLeft() const;
	int getProductionTurnsLeft(UnitTypes eUnit, int iNum) const;
	int getProductionTurnsLeft(BuildingTypes eBuilding, int iNum) const;
	int getProductionTurnsLeft(ProjectTypes eProject, int iNum) const;
	int getProductionTurnsLeft(int iProductionNeeded, int iProduction, int iFirstProductionDifference, int iProductionDifference) const;

	void setProduction(int iNewValue);
	void changeProduction(int iChange);
	int numQueuedUnits(UnitAITypes contractedAIType, const CvPlot* contractedPlot) const;

	int getProductionModifier(const OrderData& order) const;
	int getProductionModifier() const;

	int getProductionModifier(UnitTypes eUnit) const;
	int getProductionModifier(BuildingTypes eBuilding) const;
	int getProductionModifier(ProjectTypes eProject) const;

	int getProductionPerTurn(ProductionCalc::flags flags) const;

	int getProductionDifference(const OrderData& orderData, ProductionCalc::flags flags) const;
	int getCurrentProductionDifference(ProductionCalc::flags flags) const;

	bool canHurry(const HurryTypes eHurry, const bool bTestVisible = false) const;
	int64_t getHurryGold(const HurryTypes eHurry, int iHurryCost = -1) const;
	void hurry(HurryTypes eHurry);
	bool hurryOverflow(HurryTypes eHurry, int* iProduction, int* iGold, bool bCountThisTurn = false) const;

	UnitTypes getConscriptUnit() const;
	CvUnit* initConscriptedUnit();
	int getConscriptPopulation() const;
	int conscriptMinCityPopulation() const;
	int flatConscriptAngerLength() const;
	bool canConscript(bool bOnCapture = false) const;
	void conscript(bool bOnCapture = false);
	void emergencyConscript();

	int getBonusHealth(BonusTypes eBonus) const;
	int getBonusHappiness(BonusTypes eBonus) const;
	int getBonusPower(BonusTypes eBonus) const;
	int getBonusYieldRateModifier(YieldTypes eIndex, BonusTypes eBonus) const;
	void processBonus(BonusTypes eBonus, int iChange);

	void processBuilding(const BuildingTypes eBuilding, const int iChange, const bool bReligiously = false, const bool bAlphaOmega = false);
	void processProcess(ProcessTypes eProcess, int iChange);
	void processSpecialist(SpecialistTypes eSpecialist, int iChange);

	HandicapTypes getHandicapType() const;
	CivilizationTypes getCivilizationType() const;
	LeaderHeadTypes getPersonalityType() const;
	DllExport ArtStyleTypes getArtStyleType() const;
	CitySizeTypes getCitySizeType() const;
	DllExport const CvArtInfoBuilding* getBuildingArtInfo(BuildingTypes eBuilding) const;
	DllExport float getBuildingVisibilityPriority(BuildingTypes eBuilding) const;

	bool hasTrait(TraitTypes eTrait) const;
	bool isBarbarian() const;
	bool isNPC() const;
	bool isHominid() const;
	bool isHuman() const;
	DllExport bool isVisible(TeamTypes eTeam, bool bDebug) const;

	bool isCapital() const;
	bool isCoastal(int iMinWaterSize) const;
	bool isDisorder() const;
	bool isHolyCity(ReligionTypes eIndex) const;
	bool isHolyCity() const;
	bool isHeadquarters(CorporationTypes eIndex) const;
	bool isHeadquarters() const;
	void setHeadquarters(CorporationTypes eIndex);

	int getOvercrowdingPercentAnger(int iExtra = 0) const;
	int getNoMilitaryPercentAnger() const;
	int getCulturePercentAnger() const;
	int getReligionPercentAnger() const;
	int getHurryPercentAnger(int iExtra = 0) const;
	int getConscriptPercentAnger(int iExtra = 0) const;
	int getDefyResolutionPercentAnger(int iExtra = 0) const;
	int getWarWearinessPercentAnger() const;
	int getAngerPercent(const int iExtra = 0) const;

	int getRevRequestPercentAnger(int iExtra = 0) const;
	int getRevIndexPercentAnger(int iExtra = 0) const;
	int getRevSuccessHappiness() const;

	int getLargestCityHappiness() const;
	int getVassalHappiness() const;
	int getVassalUnhappiness() const;

	int getCelebrityHappiness() const;

	int unhappyLevel(int iExtra = 0) const;
	int happyLevel() const;
	int angryPopulation(int iExtra = 0) const;

	int visiblePopulation() const;
	int totalFreeSpecialists() const;
	int extraPopulation() const;
	int extraSpecialists() const;
	int extraFreeSpecialists() const;

	int unhealthyPopulation(bool bNoAngry = false, int iExtra = 0) const;
	int totalGoodBuildingHealth() const;
	int totalBadBuildingHealth() const;
	int goodHealth() const;
	int badHealth(bool bNoAngry = false, int iExtra = 0) const;
	int healthRate(bool bNoAngry = false, int iExtra = 0) const;
	int getPopulationPlusProgress100(const int iExtra) const;
	int getFoodConsumedPerPopulation100(const int iExtra = 0) const;
	int getFoodConsumedByPopulation(const int iExtra = 0) const;
	int foodConsumption(const bool bNoAngry=false, const int iExtra=0, const bool bIncludeWastage=true) const;
	int foodDifference(const bool bBottom=true, const bool bIncludeWastage=true, const bool bIgnoreFoodBuildOrRev=false) const;
	float foodWastage(int surplass = -1) const;
	int growthThreshold(const int iPopChange = 0) const;

	int productionLeft() const;
	int hurryCost() const;
	int getHurryCostModifier() const;
	int hurryPopulation(HurryTypes eHurry) const;
	int hurryProduction(HurryTypes eHurry) const;
	int flatHurryAngerLength() const;
	int hurryAngerLength(HurryTypes eHurry) const;
	int maxHurryPopulation() const;

//	int cultureDistance(int iDX, int iDY) const;
	int cultureStrength(PlayerTypes ePlayer, int &iOriginal) const;
	int cultureGarrison(PlayerTypes ePlayer) const;

	//	Note arrival or leaving of a unit
	void noteUnitMoved(const CvUnit* pUnit) const;
	int getGlobalSourcedProperty(PropertyTypes eProperty) const;
	int getTotalBuildingSourcedProperty(PropertyTypes eProperty) const;
	int getTotalUnitSourcedProperty(PropertyTypes eProperty) const;
	int getNumActiveBuilding(BuildingTypes eIndex) const;
	bool hasActiveWorldWonder() const;

	int getNumActiveWorldWonders() const;

	bool processGreatWall(bool bIn, bool bForce = false, bool bSeeded = true);

	int getReligionCount() const;
	int getCorporationCount() const;

#ifdef OUTBREAKS_AND_AFFLICTIONS
	//TB Combat Mods (Buildings) begin
	int getUnitAidPresent(PropertyTypes eProperty) const;
	int getCityAidTotal(PromotionLineTypes ePromotionLineType) const;
	int getAidRate(PropertyTypes ePropertyType) const;
	void changeAidRate(PropertyTypes ePropertyType, int iChange);
	void setAidRate(PropertyTypes ePropertyType, int iChange);

	bool hasAfflictionType(PromotionLineTypes ePromotionLineType) const;
	void changeAfflictionTypeCount(PromotionLineTypes ePromotionLineType, int iChange);
	void setAfflictionTypeCount(PromotionLineTypes ePromotionLineType, int iChange);

	int getExtraBonusAidModifier(BonusTypes eBonusType, PropertyTypes ePropertyType) const;
	void changeExtraBonusAidModifier(BonusTypes eBonusType, PropertyTypes ePropertyType, int iChange);
	void setExtraBonusAidModifier(BonusTypes eBonusType, PropertyTypes ePropertyType, int iChange);

	int getExtraAfflictionOutbreakLevelChange(PromotionLineTypes ePromotionLine) const;
	void changeExtraAfflictionOutbreakLevelChange(PromotionLineTypes ePromotionLine, int iChange);
	void setExtraAfflictionOutbreakLevelChange(PromotionLineTypes ePromotionLine, int iChange);

	int getAfflictionToleranceChange(PromotionLineTypes ePromotionLine) const;
	void changeAfflictionToleranceChange(PromotionLineTypes ePromotionLine, int iChange);
	void setAfflictionToleranceChange(PromotionLineTypes ePromotionLine, int iChange);

	int getCurrentOvercomeChange(PromotionLineTypes ePromotionLine) const;
	void changeCurrentOvercomeChange(PromotionLineTypes ePromotionLine, int iChange);
	void setCurrentOvercomeChange(PromotionLineTypes ePromotionLine, int iChange);

	int getTotalTechOutbreakLevelChange(BuildingTypes eBuilding) const;
	int getOutbreakChangesTotal(BuildingTypes eAfflictionBuilding, PromotionLineTypes eAfflictionLine) const;
	int getOvercomeChangesTotal(BuildingTypes eAfflictionBuilding, PromotionLineTypes eAfflictionLine) const;
	int getUnitCommunicability(PromotionLineTypes eAfflictionLine) const;
	int getTradeCommunicabilityTotal(BuildingTypes eAfflictionBuilding, PromotionLineTypes eAfflictionLine) const;
	bool canAcquireAffliction(BuildingTypes eDisease, PromotionLineTypes eAfflictionLine) const;
	int getTotalCommunicableExposure(PromotionLineTypes eAfflictionLine) const;
	void doOutbreakCheck(PromotionLineTypes eAfflictionLine);
	void doOvercomeCheck(PromotionLineTypes eAfflictionLine);
	int getPromotionLineAfflictionAttackCommunicability(PromotionLineTypes eAffliction) const;
	void changePromotionLineAfflictionAttackCommunicability(PromotionLineTypes eAffliction, int iChange);
	void setPromotionLineAfflictionAttackCommunicability(PromotionLineTypes eAffliction, int iValue);
#endif // OUTBREAKS_AND_AFFLICTIONS

	int getPropertyValue(PropertyTypes eProperty);

	int getUnitCombatProductionModifier(UnitCombatTypes eIndex) const;
	void changeUnitCombatProductionModifier(UnitCombatTypes eIndex, int iChange);
	int getUnitCombatRepelModifierTotal(UnitCombatTypes eIndex) const;
	void changeUnitCombatRepelModifierTotal(UnitCombatTypes eIndex, int iChange);
	int getUnitCombatRepelAgainstModifierTotal(UnitCombatTypes eIndex) const;
	void changeUnitCombatRepelAgainstModifierTotal(UnitCombatTypes eIndex, int iChange);
	int getUnitCombatDefenseAgainstModifierTotal(UnitCombatTypes eIndex) const;
	void changeUnitCombatDefenseAgainstModifierTotal(UnitCombatTypes eIndex, int iChange);

#ifdef STRENGTH_IN_NUMBERS
	int getTotalFrontSupportPercentModifier() const;
	void changeTotalFrontSupportPercentModifier(int iChange);
	int getTotalShortRangeSupportPercentModifier() const;
	void changeTotalShortRangeSupportPercentModifier(int iChange);
	int getTotalMediumRangeSupportPercentModifier() const;
	void changeTotalMediumRangeSupportPercentModifier(int iChange);
	int getTotalLongRangeSupportPercentModifier() const;
	void changeTotalLongRangeSupportPercentModifier(int iChange);
	int getTotalFlankSupportPercentModifier() const;
	void changeTotalFlankSupportPercentModifier(int iChange);
#endif // STRENGTH_IN_NUMBERS

	int getUnitCombatOngoingTrainingTimeCount(UnitCombatTypes eIndex) const;
	void changeUnitCombatOngoingTrainingTimeCount(UnitCombatTypes eIndex, int iChange);
	int getUnitCombatOngoingTrainingTimeIncrement(UnitCombatTypes eIndex) const;
	void setUnitCombatOngoingTrainingTimeIncrement(UnitCombatTypes eIndex, int iChange);
	void updateOngoingTraining(UnitCombatTypes eCombat);
	void assignOngoingTraining(UnitCombatTypes eCombat, const CvPlot* pPlot);
	bool canEquip(const CvUnit* pUnit, PromotionTypes eEquipment) const;

	bool assignPromotionChecked(PromotionTypes ePromotion, CvUnit* pUnit) const;
	void assignPromotionsFromBuildingChecked(const CvBuildingInfo& kBuilding, CvUnit* pLoopUnit) const;

	//TB Combat Mods (Buildings) end
	//TB Traits begin
	int getBaseYieldRate(YieldTypes eIndex) const;
	//TB Traits end
	bool isQuarantined() const;
	int getQuarantinedCount() const;
	void changeQuarantinedCount(int iChange);
	void resetQuarantinedCount();

	void flushBestSeeInvisibleUnitCache();
	void setBestSeeInvisibleUnitCache(InvisibleTypes eInvisible, UnitTypes eUnitType, bool bSea = false);
	UnitTypes getCachedBestSeeInvisibleUnit(InvisibleTypes eInvisible, bool bSea = false) const;
	bool isCachedBestSeeInvisibleUnit(InvisibleTypes eInvisible, bool bSea = false) const;

	DllExport int getID() const;
	int getIndex() const;
	DllExport IDInfo getIDInfo() const;
	void setID(int iID);

	DllExport int getViewportX() const;
	inline int getX() const { return m_iX; }
	DllExport int getViewportY() const;
	inline int getY() const { return m_iY; }
	bool isInViewport() const;
	bool at(int iX, int iY) const;
	bool at(const CvPlot* pPlot) const;
	CvPlot* plot() const;
	DllExport CvPlot* plotExternal() const;
	CvPlotGroup* plotGroup(const PlayerTypes ePlayer) const;
	bool isConnectedTo(const CvCity* pCity) const;
	bool isConnectedToCapital(const PlayerTypes ePlayer = NO_PLAYER) const;
	int getArea() const;
	CvArea* area() const;
	CvArea* waterArea(const bool bNoImpassable = false) const;
	CvArea* secondWaterArea() const;
	CvArea* sharedWaterArea(const CvCity* pCity) const;
	bool isBlockaded() const;

	CvPlot* getRallyPlot() const;
	void setRallyPlot(const CvPlot* pPlot);

	int getGameTurnFounded() const;
	void setGameTurnFounded(int iNewValue);

	int getGameTurnAcquired() const;
	void setGameTurnAcquired(int iNewValue);

	int getPopulation() const;
	void setPopulation(int iNewValue);
	void changePopulation(int iChange);

	int getRealPopulation() const;

	int getHighestPopulation() const;
	void setHighestPopulation(int iNewValue);

	int getWorkingPopulation() const;
	void changeWorkingPopulation(int iChange);

	int getSpecialistPopulation() const;
	void changeSpecialistPopulation(int iChange);

	int getNumGreatPeople() const;
	void changeNumGreatPeople(int iChange);

	int getBaseGreatPeopleRate() const;
	int getGreatPeopleRate() const;
	int getTotalGreatPeopleRateModifier() const;
	void changeBaseGreatPeopleRate(int iChange);

	int getGreatPeopleRateModifier() const;
	void changeGreatPeopleRateModifier(int iChange);

	CvProperties* getProperties();
	const CvProperties* getPropertiesConst() const;

	int getAdditionalGreatPeopleRateByBuilding(BuildingTypes eBuilding) const;
	int getAdditionalBaseGreatPeopleRateByBuilding(BuildingTypes eBuilding) const;
	int getAdditionalGreatPeopleRateModifierByBuilding(BuildingTypes eBuilding) const;

	int getAdditionalGreatPeopleRateBySpecialist(SpecialistTypes eSpecialist, int iChange) const;
	int getAdditionalBaseGreatPeopleRateBySpecialist(SpecialistTypes eSpecialist, int iChange) const;

	int getGreatPeopleProgress() const;
	void changeGreatPeopleProgress(int iChange);

	int getNumWorldWonders() const;
	void changeNumWorldWonders(int iChange);

	int getNumTeamWonders() const;
	void changeNumTeamWonders(int iChange);

	int getNumNationalWonders() const;
	void changeNumNationalWonders(int iChange);

	int getNumBuildings() const;
	void changeNumBuildings(int iChange);

	int getGovernmentCenterCount() const;
	bool isGovernmentCenter() const;
	void changeGovernmentCenterCount(int iChange);

	int getSavedMaintenanceByBuilding(BuildingTypes eBuilding) const;
	int getSavedMaintenanceTimes100ByBuilding(BuildingTypes eBuilding) const;

	int getMaintenance() const;
	int getMaintenanceTimes100() const;
	int getEffectiveMaintenanceModifier() const;
	void updateMaintenance() const;
	void setMaintenanceDirty(bool bDirty) const;
	int calculateDistanceMaintenance() const;
	int calculateNumCitiesMaintenance() const;
	int calculateColonyMaintenance() const;
	int calculateCorporationMaintenance() const;
	int calculateDistanceMaintenanceTimes100(int iExtraDistanceModifier = 0, int iExtraCoastalDistanceModifier = 0) const;
	int calculateNumCitiesMaintenanceTimes100(int iExtraModifier = 0) const;
	int calculateColonyMaintenanceTimes100() const;
	int calculateCorporationMaintenanceTimes100(CorporationTypes eCorporation) const;
	int calculateCorporationMaintenanceTimes100() const;
	int calculateBaseMaintenanceTimes100() const;
	int calculateBuildingMaintenanceTimes100() const;
	int getMaintenanceModifier() const;
	void changeMaintenanceModifier(int iChange);

	int getWarWearinessModifier() const;
	void changeWarWearinessModifier(int iChange);

	int getHurryAngerModifier() const;
	void changeHurryAngerModifier(int iChange);

	int getHealRate() const;
	void changeHealRate(int iChange);

	int getHealUnitCombatTypeTotal(UnitCombatTypes eUnitCombat) const;
	void changeHealUnitCombatTypeVolume(UnitCombatTypes eUnitCombat, int iChange);
	void setHealUnitCombatTypeVolume(UnitCombatTypes eUnitCombat, int iChange);

	int getEspionageHealthCounter() const;
	void changeEspionageHealthCounter(int iChange);

	int getEspionageHappinessCounter() const;
	void changeEspionageHappinessCounter(int iChange);

	int getFreshWaterGoodHealth() const;
	void updateFreshWaterHealth();

	int getFeatureGoodHealth() const;
	int getFeatureBadHealth() const;
	void updateFeatureHealth();
// BUG - Feature Health - start
	void calculateFeatureHealthPercent(int& iGood, int& iBad) const;
	void calculateFeatureHealthPercentChange(int& iGood, int& iBad, CvPlot* pIgnorePlot = NULL) const;
	int getAdditionalHealthByFeature(FeatureTypes eFeature, int iChange) const;
	int getAdditionalHealthByFeature(FeatureTypes eFeature, int iChange, int& iGood, int& iBad) const;
	int getAdditionalHealth(int iGoodPercent, int iBadPercent, int& iGood, int& iBad) const;
// BUG - Feature Health - end

// BUG - Actual Effects - start
	int getAdditionalAngryPopuplation(int iGood, int iBad) const;
	int getAdditionalSpoiledFood(int iGood, int iBad, int iHealthAdjust = 0) const;
	int getAdditionalStarvation(int iSpoiledFood, int iFoodAdjust = 0) const;
// BUG - Actual Effects - end

	int getBuildingGoodHealth() const;
	int getBuildingBadHealth() const;
	int getBuildingHealth(BuildingTypes eBuilding) const;
	int getBuildingGoodHealth(BuildingTypes eBuilding) const;
	int getBuildingBadHealth(BuildingTypes eBuilding) const;
	void changeBuildingGoodHealth(int iChange);
	void changeBuildingBadHealth(int iChange);

	int getBonusGoodHealth() const;
	int getBonusBadHealth() const;
	void changeBonusGoodHealth(int iChange);
	void changeBonusBadHealth(int iChange);

	int getMilitaryHappiness() const;
	int getMilitaryHappinessUnits() const;
	void changeMilitaryHappinessUnits(int iChange);

	int getBuildingGoodHappiness() const;
	int getBuildingBadHappiness() const;
	int getBuildingHappiness(BuildingTypes eBuilding) const;
	void changeBuildingGoodHappiness(int iChange);
	void changeBuildingBadHappiness(int iChange);

	int getExtraBuildingGoodHappiness() const;
	int getExtraBuildingBadHappiness() const;
	void updateExtraBuildingHappiness(bool bLimited = false);

	int getAdditionalHappinessByCivic(CivicTypes eCivic, bool bDifferenceToCurrent = true, bool bCivicOptionVacuum = false, ReligionTypes eStateReligion = NO_RELIGION, int iExtraPop = 0, int iMilitaryHappinessUnits = -1) const;
	int getAdditionalHealthByCivic(CivicTypes eCivic, bool bDifferenceToCurrent = true) const;
	int getAdditionalHealthByCivic(CivicTypes eCivic, int& iGood, int& iBad, int& iBadBuilding, bool bDifferenceToCurrent = true, int iExtraPop = 0, bool bCivicOptionVacuum = false, int iIgnoreNoUnhealthyPopulationCount = 0, int iIgnoreBuildingOnlyHealthyCount = 0) const;
	int getAdditionalHealthByPlayerNoUnhealthyPopulation(int iExtraPop = 0, int iIgnoreNoUnhealthyPopulationCount = 0) const;
	int getAdditionalHealthByPlayerBuildingOnlyHealthy(int iIgnoreBuildingOnlyHealthyCount = 0) const;

	int getAdditionalHappinessByBuilding(BuildingTypes eBuilding) const;
	int getAdditionalHappinessByBuilding(BuildingTypes eBuilding, int& iGood, int& iBad, int& iAngryPop) const;

	int getExtraBuildingGoodHealth() const;
	int getExtraBuildingBadHealth() const;
	void updateExtraBuildingHealth(bool bLimited = false);

	int getAdditionalHealthByBuilding(BuildingTypes eBuilding) const;
	int getAdditionalHealthByBuilding(BuildingTypes eBuilding, int& iGood, int& iBad, int& iSpoiledFood, int& iStarvation) const;

	int getFeatureGoodHappiness() const;
	int getFeatureBadHappiness() const;
	void updateFeatureHappiness(bool bLimited = false);

	int getBonusGoodHappiness() const;
	int getBonusBadHappiness() const;
	void changeBonusGoodHappiness(int iChange);
	void changeBonusBadHappiness(int iChange);

	int getReligionGoodHappiness() const;
	int getReligionBadHappiness() const;
	int getReligionHappiness(ReligionTypes eReligion) const;
	void updateReligionHappiness(bool bLimited = false);

	int getExtraHappiness() const;
	void changeExtraHappiness(int iChange);

	int getExtraHealth() const;
	void changeExtraHealth(int iChange);

	int getHurryAngerTimer() const;
	void changeHurryAngerTimer(int iChange);

	int getRevRequestAngerTimer() const;
	void changeRevRequestAngerTimer(int iChange);

	int getRevSuccessTimer() const;
	void changeRevSuccessTimer(int iChange);

	int getConscriptAngerTimer() const;
	void changeConscriptAngerTimer(int iChange);

	int getDefyResolutionAngerTimer() const;
	void changeDefyResolutionAngerTimer(int iChange);
	int flatDefyResolutionAngerLength() const;

	int getHappinessTimer() const;
	void changeHappinessTimer(int iChange);

	int getNoUnhappinessCount() const;
	bool isNoUnhappiness() const;
	void changeNoUnhappinessCount(int iChange);

	int getNoUnhealthyPopulationCount() const;
	bool isNoUnhealthyPopulation() const;
	void changeNoUnhealthyPopulationCount(int iChange);

	int getBuildingOnlyHealthyCount() const;
	bool isBuildingOnlyHealthy() const;
	void changeBuildingOnlyHealthyCount(int iChange);

	int getFood() const;
	void setFood(int iNewValue);
	void changeFood(int iChange);

	int getFoodKept() const;
	void setFoodKept(int iNewValue);
	void changeFoodKept(int iChange);

	int getMaxFoodKeptPercent() const;
	void changeMaxFoodKeptPercent(int iChange, bool bAdd);

	int getMaxProductionOverflow() const;

	int getOverflowProduction() const;
	void setOverflowProduction(int iNewValue);
	void changeOverflowProduction(int iChange);

	int getFeatureProduction() const;
	void setFeatureProduction(int iNewValue);
	void changeFeatureProduction(int iChange);

	int getMilitaryProductionModifier() const;
	void changeMilitaryProductionModifier(int iChange);

	int getSpaceProductionModifier() const;
	void changeSpaceProductionModifier(int iChange);

	int getExtraTradeRoutes() const;
	void changeExtraTradeRoutes(int iChange);

	int getMaxTradeRoutes() const;

	int getTradeRouteModifier() const;
	void changeTradeRouteModifier(int iChange);

	int getForeignTradeRouteModifier() const;
	void changeForeignTradeRouteModifier(int iChange);

	int getBuildingDefense() const;
	void changeBuildingDefense(int iChange);

	int getBuildingBombardDefense() const;
	void changeBuildingBombardDefense(int iChange);
	int getAdditionalBombardDefenseByBuilding(BuildingTypes eBuilding) const;

	int getFreeExperience() const;
	void changeFreeExperience(int iChange);

	int getCurrAirlift() const;
	void setCurrAirlift(int iNewValue);
	void changeCurrAirlift(int iChange);

	int getMaxAirlift() const;
	void changeMaxAirlift(int iChange);

	int getAirModifier() const;
	void changeAirModifier(int iChange);

	int getSMAirUnitCapacity(TeamTypes eTeam) const;
	int getAirUnitCapacity(TeamTypes eTeam) const;
	void changeAirUnitCapacity(int iChange);

	int getNukeModifier() const;
	void changeNukeModifier(int iChange);

	int getFreeSpecialist() const;
	void changeFreeSpecialist(int iChange);

	int getPowerCount() const;
	bool isPower() const;
	bool isAreaCleanPower() const;
	void changePowerCount(int iChange);

	int getDefenseDamage() const;
	void changeDefenseDamage(int iChange);
	void changeDefenseModifier(int iChange);

	int getLastDefenseDamage() const;
	void setLastDefenseDamage(int iNewValue);

	bool isBombardable(const CvUnit* pUnit) const;
	int getNaturalDefense() const;
	int getTotalDefense(bool bIgnoreBuilding) const;
	int getDefenseModifier(bool bIgnoreBuilding) const;

	int getOccupationTimer() const;
	bool isOccupation() const;
	void setOccupationTimer(int iNewValue);
	void changeOccupationTimer(int iChange);

	int getCultureUpdateTimer() const;
	void setCultureUpdateTimer(int iNewValue);
	void changeCultureUpdateTimer(int iChange);

	int getCitySizeBoost() const;
	void setCitySizeBoost(int iBoost);

	bool isNeverLost() const;
	void setNeverLost(bool bNewValue);

	bool isBombarded() const;
	void setBombarded(bool bNewValue);

	bool isDrafted() const;
	void setDrafted(bool bNewValue);

	bool isAirliftTargeted() const;
	void setAirliftTargeted(bool bNewValue);

	bool isWeLoveTheKingDay() const;
	void setWeLoveTheKingDay(bool bNewValue);

	bool isCitizensAutomated() const;
	void setCitizensAutomated(bool bNewValue);

	bool isProductionAutomated() const;
	void setProductionAutomated(bool bNewValue, bool bClear);

	/* allows you to programatically specify a cities walls rather than having them be generated automagically */
	DllExport bool isWallOverride() const;
	void setWallOverride(bool bOverride);

	DllExport bool isInfoDirty() const;
	DllExport void setInfoDirty(bool bNewValue);

	DllExport bool isLayoutDirty() const;
	DllExport void setLayoutDirty(bool bNewValue);

	bool isPlundered() const;
	void setPlundered(bool bNewValue);

	DllExport inline PlayerTypes getOwner() const { return m_eOwner; }
	DllExport TeamTypes getTeam() const;

	PlayerTypes getPreviousOwner() const;
	void setPreviousOwner(PlayerTypes eNewValue);

	PlayerTypes getOriginalOwner() const;
	void setOriginalOwner(PlayerTypes eNewValue);

	CultureLevelTypes getCultureLevel() const;
	int getCultureThreshold() const;
	int getCultureThreshold(CultureLevelTypes eLevel) const;
	void setCultureLevel(CultureLevelTypes eNewValue, bool bUpdatePlotGroups);
	void updateCultureLevel(bool bUpdatePlotGroups);

	int getSeaPlotYield(YieldTypes eIndex) const;
	void changeSeaPlotYield(YieldTypes eIndex, int iChange);

	int getRiverPlotYield(YieldTypes eIndex) const;
	void changeRiverPlotYield(YieldTypes eIndex, int iChange);

	int getAdditionalYieldByBuilding(YieldTypes eIndex, BuildingTypes eBuilding, bool bFilter = false) const;
	int getAdditionalExtraYieldByBuilding(YieldTypes eIndex, BuildingTypes eBuilding) const;
	int getAdditionalBaseYieldByBuilding(YieldTypes eIndex, BuildingTypes eBuilding) const;
	int getAdditionalBaseYieldModifierByBuilding(YieldTypes eIndex, BuildingTypes eBuilding, bool bFilter = false) const;

	int getYieldBySpecialist(YieldTypes eIndex, SpecialistTypes eSpecialist) const;

	int getPlotYield(YieldTypes eIndex) const;
	int getBaseYieldRateModifier(YieldTypes eIndex, int iExtra = 0) const;
	int getYieldRate(YieldTypes eIndex) const;
	void changePlotYield(YieldTypes eIndex, int iChange);

	int getExtraYield(YieldTypes eIndex) const;
	void changeExtraYield(YieldTypes eIndex, int iChange);

	void onYieldChange();

	int popYield(YieldTypes eIndex) const;
	int getBaseYieldPerPopRate(YieldTypes eIndex) const;
	void setBaseYieldPerPopRate(YieldTypes eIndex, int iNewValue);
	void changeBaseYieldPerPopRate(YieldTypes eIndex, int iChange);

	int getYieldRateModifier(YieldTypes eIndex) const;
	void changeYieldRateModifier(YieldTypes eIndex, int iChange);

	int getPowerYieldRateModifier(YieldTypes eIndex) const;
	void changePowerYieldRateModifier(YieldTypes eIndex, int iChange);

	int getBonusYieldRateModifier(YieldTypes eIndex) const;
	void changeBonusYieldRateModifier(YieldTypes eIndex, int iChange);

	int getTradeYield(YieldTypes eIndex) const;
	int totalTradeModifier(const CvCity* pOtherCity = NULL) const;
	int getPopulationTradeModifier() const;
	int getPeaceTradeModifier(TeamTypes eTeam) const;
	int getBaseTradeProfit(const CvCity* pCity) const;
#ifdef _MOD_FRACTRADE
	int calculateTradeProfitTimes100(const CvCity* pCity) const;
#endif
	int calculateTradeProfit(const CvCity* pCity) const;
	int calculateTradeYield(YieldTypes eIndex, int iTradeProfit) const;
	void calculateTradeTotals(YieldTypes eIndex, int& iDomesticYield, int& iDomesticRoutes, int& iForeignYield, int& iForeignRoutes, PlayerTypes eWithPlayer = NO_PLAYER, bool bRound = false, bool bBase = false) const;
	int calculateTotalTradeYield(YieldTypes eIndex, PlayerTypes eWithPlayer = NO_PLAYER, bool bRound = false, bool bBase = false) const;
	void setTradeYield(YieldTypes eIndex, int iNewValue);

	int getExtraSpecialistYield(YieldTypes eIndex) const;
	int getExtraSpecialistYield(YieldTypes eIndex, SpecialistTypes eSpecialist) const;
	void updateExtraSpecialistYield(YieldTypes eYield);
	void updateExtraSpecialistYield();

	int getExtraSpecialistCommerceTotal(CommerceTypes eIndex) const;
	int getExtraSpecialistCommerce(CommerceTypes eIndex, SpecialistTypes eSpecialist) const;
	void updateExtraSpecialistCommerce(CommerceTypes eCommerce);
	void updateExtraSpecialistCommerce();

	int getCommerceRate(CommerceTypes eIndex) const;
	int getCommerceRateTimes100(CommerceTypes eIndex) const;
	int getCommerceFromPercent(CommerceTypes eIndex, int iYieldRate) const;
	int getBaseCommerceRate(CommerceTypes eIndex) const;
	int getBaseCommerceRateTimes100(CommerceTypes eIndex) const;
	int getTotalCommerceRateModifier(CommerceTypes eIndex) const;
	void setCommerceModifierDirty(CommerceTypes eCommerce);
	void setCommerceDirty(CommerceTypes eCommerce = NO_COMMERCE);
	void updateCommerce(CommerceTypes eIndex = NO_COMMERCE, bool bForce = true) const;

	int getProductionToCommerceModifier(CommerceTypes eIndex) const;
	void changeProductionToCommerceModifier(CommerceTypes eIndex, int iChange);

	int getBuildingCommerce(CommerceTypes eIndex) const;
	int getBuildingCommerceByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding) const;
	int getOrbitalBuildingCommerceByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding) const;
	int getAdditionalCommerceByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding) const;
	int getAdditionalCommerceTimes100ByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding) const;
	int getAdditionalBaseCommerceRateByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding) const;
	int getAdditionalBaseCommerceRateByBuildingTimes100(CommerceTypes eIndex, BuildingTypes eBuilding) const;
	int getAdditionalCommerceRateModifierByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding) const;
	void updateBuildingCommerce();

	int getSpecialistCommerce(CommerceTypes eIndex) const;
	void changeSpecialistCommerceTimes100(CommerceTypes eIndex, int iChange);
	int getAdditionalCommerceBySpecialist(CommerceTypes eIndex, SpecialistTypes eSpecialist, int iChange) const;
	int getAdditionalCommerceTimes100BySpecialist(CommerceTypes eIndex, SpecialistTypes eSpecialist, int iChange) const;
	int getAdditionalBaseCommerceRateBySpecialist(CommerceTypes eIndex, SpecialistTypes eSpecialist, int iChange) const;

	int getReligionCommerce(CommerceTypes eIndex) const;
	int getReligionCommerceByReligion(CommerceTypes eIndex, ReligionTypes eReligion) const;
	void updateReligionCommerce(CommerceTypes eIndex);
	void updateReligionCommerce();

	void setCorporationYield(YieldTypes eIndex, int iNewValue);
	int getCorporationCommerce(CommerceTypes eIndex) const;
	int getCorporationCommerceByCorporation(CommerceTypes eIndex, CorporationTypes eCorporation) const;
	int getCorporationYield(YieldTypes eIndex) const;
	int getCorporationYieldByCorporation(YieldTypes eIndex, CorporationTypes eCorporation) const;
	void updateCorporation();
	void updateCorporationCommerce(CommerceTypes eIndex);
	void updateCorporationYield(YieldTypes eIndex);
	void updateCorporationBonus();

	int getCommerceRateModifier(CommerceTypes eIndex) const;
	void changeCommerceRateModifier(CommerceTypes eIndex, int iChange);

	int getCommerceHappinessPer(CommerceTypes eIndex) const;
	int getCommerceHappinessByType(CommerceTypes eIndex) const;
	int getCommerceHappiness() const;
	void changeCommerceHappinessPer(CommerceTypes eIndex, int iChange);

	int getDomainFreeExperience(DomainTypes eIndex) const;
	void changeDomainFreeExperience(DomainTypes eIndex, int iChange);

	int getDomainProductionModifier(DomainTypes eIndex) const;
	void changeDomainProductionModifier(DomainTypes eIndex, int iChange);

	int getCulture(PlayerTypes eIndex) const;
	int getCultureTimes100(PlayerTypes eIndex) const;
	int countTotalCultureTimes100() const;
	PlayerTypes findHighestCulture() const;
	int calculateCulturePercent(PlayerTypes eIndex) const;
	int calculateTeamCulturePercent(TeamTypes eIndex) const;
	void setCulture(PlayerTypes eIndex, int iNewValue, bool bPlots, bool bUpdatePlotGroups, bool bNationalSet = false);
	void setCultureTimes100(PlayerTypes eIndex, int iNewValue, bool bPlots, bool bUpdatePlotGroups, bool bNationalSet = false);
	void changeCulture(PlayerTypes eIndex, int iChange, bool bPlots, bool bUpdatePlotGroups);
	void changeCultureTimes100(PlayerTypes eIndex, int iChange, bool bPlots, bool bUpdatePlotGroups);

	int getNumRevolts(PlayerTypes eIndex) const;
	void changeNumRevolts(PlayerTypes eIndex, int iChange);

	bool isTradeRoute(PlayerTypes eIndex) const;
	void setTradeRoute(PlayerTypes eIndex, bool bNewValue);

	bool isEverOwned(PlayerTypes eIndex) const;
	void setEverOwned(PlayerTypes eIndex, bool bNewValue);

	DllExport bool isRevealed(TeamTypes eIndex, bool bDebug) const;
	void setRevealed(TeamTypes eIndex, bool bNewValue);

	bool getEspionageVisibility(TeamTypes eTeam) const;
	void setEspionageVisibility(TeamTypes eTeam, bool bVisible, bool bUpdatePlotGroups);
	void updateEspionageVisibility(bool bUpdatePlotGroups);

	DllExport const CvWString getName(uint uiForm = 0) const;
	DllExport const wchar_t* getNameKey() const;
	void setName(const wchar_t* szNewValue, bool bFound = false);
	void doFoundMessage();

	// Script data needs to be a narrow string for pickling in Python
	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);

	int getFreeBonus(BonusTypes eIndex) const;
	void changeFreeBonus(BonusTypes eIndex, int iChange);

	void processNumBonusChange(BonusTypes eIndex, int iOldValue, int iNewValue);
	void endDeferredBonusProcessing();
	void startDeferredBonusProcessing();
	int getNumBonusesFromBase(BonusTypes eIndex, int iBaseNum) const;

	int getNumBonuses(BonusTypes eIndex) const;
	bool hasBonus(BonusTypes eIndex) const;
	void changeNumBonuses(BonusTypes eIndex, int iChange);

	int getNumCorpProducedBonuses(BonusTypes eIndex) const;
	bool isCorporationBonus(BonusTypes eBonus) const;
	bool isActiveCorporation(CorporationTypes eCorporation) const;

	// How many hammers already put into production of the building
	int getBuildingProduction(BuildingTypes eIndex) const;
	void setBuildingProduction(BuildingTypes eIndex, int iNewValue);
	void changeBuildingProduction(BuildingTypes eIndex, int iChange);

	int getBuildingProductionTime(BuildingTypes eIndex) const;
	void setBuildingProductionTime(BuildingTypes eIndex, int iNewValue);
	void changeBuildingProductionTime(BuildingTypes eIndex, int iChange);

	bool isBuildingProductionDecay(BuildingTypes eIndex) const;
	int getBuildingProductionDecay(BuildingTypes eIndex) const;
	int getBuildingProductionDecayTurns(BuildingTypes eIndex) const;

	int getProjectProduction(ProjectTypes eIndex) const;
	void setProjectProduction(ProjectTypes eIndex, int iNewValue);
	void changeProjectProduction(ProjectTypes eIndex, int iChange);

	int getBuildingOriginalOwner(BuildingTypes eIndex) const;
	int getBuildingOriginalTime(BuildingTypes eIndex) const;
	void setBuildingOriginalTime(BuildingTypes eIndex, int iNewValue);

	int getUnitProduction(UnitTypes eIndex) const;
	void setUnitProduction(UnitTypes eIndex, int iNewValue);
	void changeUnitProduction(UnitTypes eIndex, int iChange);

	int getUnitProductionTime(UnitTypes eIndex) const;
	void setUnitProductionTime(UnitTypes eIndex, int iNewValue);
	void changeUnitProductionTime(UnitTypes eIndex, int iChange);

	bool isUnitProductionDecay(UnitTypes eIndex) const;
	int getUnitProductionDecay(UnitTypes eIndex) const;
	int getUnitProductionDecayTurns(UnitTypes eIndex) const;

	int getGreatPeopleUnitRate(UnitTypes eIndex) const;
	void setGreatPeopleUnitRate(UnitTypes eIndex, int iNewValue);
	void changeGreatPeopleUnitRate(UnitTypes eIndex, int iChange);

	int getGreatPeopleUnitProgress(UnitTypes eIndex) const;
	void setGreatPeopleUnitProgress(UnitTypes eIndex, int iNewValue);
	void changeGreatPeopleUnitProgress(UnitTypes eIndex, int iChange);

	int getSpecialistCount(SpecialistTypes eIndex) const;
	void setSpecialistCount(SpecialistTypes eIndex, int iNewValue);
	void changeSpecialistCount(SpecialistTypes eIndex, int iChange);
	void alterSpecialistCount(SpecialistTypes eIndex, int iChange);

	int getMaxSpecialistCount() const;
	int getMaxSpecialistCount(SpecialistTypes eIndex) const;
	bool isSpecialistValid(SpecialistTypes eIndex, int iExtra = 0) const;
	void changeMaxSpecialistCount(SpecialistTypes eIndex, int iChange);

	int getForceSpecialistCount(SpecialistTypes eIndex) const;
	bool isSpecialistForced() const;
	void setForceSpecialistCount(SpecialistTypes eIndex, int iNewValue);

	int getFreeSpecialistCount(SpecialistTypes eIndex) const;
	void setFreeSpecialistCount(SpecialistTypes eIndex, int iNewValue);
	void changeFreeSpecialistCount(SpecialistTypes eIndex, int iChange, bool bUnattributed = false);
	int getAddedFreeSpecialistCount(SpecialistTypes eIndex) const;

	int getImprovementFreeSpecialists(ImprovementTypes eIndex) const;
	void changeImprovementFreeSpecialists(ImprovementTypes eIndex, int iChange);

	uint32_t getReligionInfluence(ReligionTypes eIndex) const;
	void changeReligionInfluence(ReligionTypes eIndex, int iChange);

	int getCurrentStateReligionHappiness() const;
	int getStateReligionHappiness(ReligionTypes eIndex) const;
	void changeStateReligionHappiness(ReligionTypes eIndex, int iChange);

	int getUnitCombatFreeExperience(UnitCombatTypes eIndex) const;
	void changeUnitCombatFreeExperience(UnitCombatTypes eIndex, int iChange);

	int getFreePromotionCount(PromotionTypes eIndex) const;
	bool isFreePromotion(PromotionTypes eIndex) const;
	void changeFreePromotionCount(PromotionTypes eIndex, int iChange);

	int getSpecialistFreeExperience() const;
	void changeSpecialistFreeExperience(int iChange);

	int getEspionageDefenseModifier() const;
	void changeEspionageDefenseModifier(int iChange);

	bool isWorkingPlot(int iIndex) const;
	bool isWorkingPlot(const CvPlot* pPlot) const;
	void setWorkingPlot(int iCityPlotIndex, bool bNewValue);
	void alterWorkingPlot(int iIndex);
	void processWorkingPlot(int iPlot, int iChange, bool yieldsOnly = false);

	bool hasFullyActiveBuilding(const BuildingTypes eIndex) const;
	int getNumRealBuilding(const BuildingTypes eIndex) const;
	void setNumRealBuilding(const BuildingTypes eIndex, const int iNewValue);
	void setNumRealBuildingTimed(const BuildingTypes eBuilding, const bool bNewValue, const PlayerTypes eOriginalOwner, const int iOriginalTime, const bool bFirst = true);
	void setupBuilding(const CvBuildingInfo& kBuilding, const BuildingTypes eBuilding, const bool bNewValue, const bool bFirst);
	void handleBuildingCounts(const BuildingTypes eBuilding, const int iChange, const bool bWonder);

	bool isValidBuildingLocation(BuildingTypes eIndex) const;

	uint16_t getFreeAreaBuildingCount(const short iIndex) const;
	void changeFreeAreaBuildingCount(const BuildingTypes eIndex, const int iChange);
	void setFreeBuilding(const BuildingTypes eIndex, const bool bNewValue);
	bool isFreeBuilding(short iIndex) const;
	void checkFreeBuildings();

	bool isHasReligion(ReligionTypes eIndex) const;
	void setHasReligion(ReligionTypes eIndex, bool bNewValue, bool bAnnounce, bool bArrows = true);

	void checkReligiousDisablingAllBuildings();
	void checkReligiousDisabling(const BuildingTypes eBuilding, const CvPlayer& player);
	void applyReligionModifiers(const ReligionTypes eIndex, const bool bValue);

	bool isHasCorporation(CorporationTypes eIndex) const;
	void setHasCorporation(CorporationTypes eIndex, bool bNewValue, bool bAnnounce, bool bArrows = true);
	void applyCorporationModifiers(CorporationTypes eIndex, bool bValue);

	CvCity* getTradeCity(int iIndex) const;
	int getTradeRoutes() const;
	void clearTradeRoutes();
	void updateTradeRoutes();
	void resizeTradeRouteVector();

	void clearOrderQueue();
	void pushOrder(OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bForce = false, CvPlot* deliveryDestination = NULL, UnitAITypes contractedAIType = NO_UNITAI, uint8_t contractFlags = 0);
	void popOrder(int iNum, bool bFinish = false, bool bChoose = false, bool bResolveList = true);
	void startHeadOrder();
	void stopHeadOrder();

	int getOrderQueueLength() const;
	bst::optional<OrderData> getHeadOrder() const { return m_orderQueue.empty() ? bst::optional<OrderData>() : m_orderQueue.front(); }
	bst::optional<OrderData> getTailOrder() const { return m_orderQueue.empty() ? bst::optional<OrderData>() : m_orderQueue.back(); }
	OrderData getOrderAt(int index) const { return m_orderQueue[index]; }

	//CLLNode<OrderData>* nextOrderQueueNode(CLLNode<OrderData>* pNode) const;
	//CLLNode<OrderData>* headOrderQueueNode() const;
	DllExport int getNumOrdersQueued() const { return m_orderQueue.size(); };
	DllExport OrderData getOrderData(int iIndex) const;
	bool pushFirstValidBuildListOrder(int iListID);

	// fill the kVisible array with buildings that you want shown in city, as well as the number of generics
	// This function is called whenever CvCity::setLayoutDirty() is called
	DllExport void getVisibleBuildings(std::list<BuildingTypes>& kVisible, int& iNumGenerics);

	// Fill the kEffectNames array with references to effects in the CIV4EffectInfos.xml to have a
	// city play a given set of effects. This is called whenever the interface updates the city billboard
	// or when the zoom level changes
	DllExport void getVisibleEffects(ZoomLevelTypes eCurrentZoom, std::vector<const TCHAR*>& kEffectNames);


	// Billboard appearance controls
	DllExport void getCityBillboardSizeIconColors(NiColorA& kDotColor, NiColorA& kTextColor) const;
	DllExport const TCHAR* getCityBillboardProductionIcon() const;
	DllExport bool getFoodBarPercentages(std::vector<float>& afPercentages) const;
	DllExport bool getProductionBarPercentages(std::vector<float>& afPercentages) const;
	DllExport NiColorA getBarBackgroundColor() const;
	DllExport bool isStarCity() const;


	void setWallOverridePoints(const std::vector< std::pair<float, float> >& kPoints); /* points are given in world space ... i.e. PlotXToPointX, etc */
	DllExport const std::vector< std::pair<float, float> >& getWallOverridePoints() const;

	bool isEventTriggerPossible(EventTriggerTypes eTrigger) const;
	int getTriggerValue(EventTriggerTypes eTrigger) const;
	bool canApplyEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const;
	void applyEvent(EventTypes eEvent, const EventTriggeredData* pTriggeredData);
	bool isEventOccured(EventTypes eEvent) const;
	void setEventOccured(EventTypes eEvent, bool bOccured);

	int getBuildingYieldChange(BuildingTypes eBuilding, YieldTypes eYield) const;
	void setBuildingYieldChange(BuildingTypes eBuilding, YieldTypes eYield, int iChange);
	void changeBuildingYieldChange(BuildingTypes eBuilding, YieldTypes eYield, int iChange);
	int getBuildingCommerceChange(BuildingTypes eBuilding, CommerceTypes eCommerce) const;
	void setBuildingCommerceChange(BuildingTypes eBuilding, CommerceTypes eCommerce, int iChange);
	void changeBuildingCommerceChange(BuildingTypes eBuilding, CommerceTypes eCommerce, int iChange);
	int getBuildingHappyChange(BuildingTypes eBuilding) const;
	void setBuildingHappyChange(BuildingTypes eBuilding, int iChange);
	int getBuildingHealthChange(BuildingTypes eBuilding) const;
	void setBuildingHealthChange(BuildingTypes eBuilding, int iChange);

	PlayerTypes getLiberationPlayer(bool bConquest) const;
	void liberate(bool bConquest);

	void changeNoBonusCount(BonusTypes eBonus, int iChange);
	int getNoBonusCount(BonusTypes eBonus) const;
	bool isNoBonus(BonusTypes eBonus) const;

	bool isAutoRaze() const;

	DllExport int getMusicScriptId() const;
	DllExport int getSoundscapeScriptId() const;
	DllExport void cheat(bool bCtrl, bool bAlt, bool bShift);

	DllExport void getBuildQueue(std::vector<std::string>& astrQueue) const;

	int getCivicHappiness() const;
	int getAdditionalDefenseByBuilding(BuildingTypes eBuilding) const;
	int getNumCityPlots() const;
	int getPopulationgrowthratepercentage() const;
	void changePopulationgrowthratepercentage(int iChange, bool bAdd);
	bool isValidTerrainForBuildings(BuildingTypes eIndex) const;

	void changeFreshWater(int iChange);
	bool hasFreshWater() const;

	bool canUpgradeUnit(UnitTypes eUnit) const;

	int getBuildingProductionModifier(const BuildingTypes eIndex) const;
	void changeBuildingProductionModifier(const BuildingTypes eIndex, const int iChange);

	int getUnitProductionModifier(const UnitTypes eIndex) const;
	void changeUnitProductionModifier(const UnitTypes eIndex, const int iChange);

	bool hadVicinityBonus(BonusTypes eIndex) const;
	bool hadRawVicinityBonus(BonusTypes eIndex) const;

	int getBonusDefenseChanges(const BonusTypes eIndex) const;
	void changeBonusDefenseChanges(const BonusTypes eIndex, const int iChange);

	int getBonusCommerceRateModifier(CommerceTypes eIndex) const;
	void changeBonusCommerceRateModifier(CommerceTypes eIndex, int iChange);
	bool isBuiltFoodProducedUnit() const;
	void setBuiltFoodProducedUnit(bool bNewValue);
	int getSpecialistGoodHealth() const;
	int getSpecialistBadHealth() const;
	int getSpecialistHappiness() const;
	int getSpecialistUnhappiness() const;
	void changeSpecialistGoodHealth(int iChange);
	void changeSpecialistBadHealth(int iChange);
	void changeSpecialistHappiness(int iChange);
	void changeSpecialistUnhappiness(int iChange);
	int getImprovementGoodHealth() const;
	int getImprovementBadHealth() const;
	void updateImprovementHealth();
	int getBonusCommerceRateModifier(CommerceTypes eIndex, BonusTypes eBonus) const;
	void clearLostProduction();
	bool isProductionWonder() const;
	void updateYieldRate(BuildingTypes eBuilding, YieldTypes eYield, int iChange);
	void updateMaxSpecialistCount(BuildingTypes eBuilding, SpecialistTypes eSpecialist, int iChange);
	void updateCommerceModifierByBuilding(BuildingTypes eBuilding, CommerceTypes eCommerce, int iChange);
	void updateCommerceRateByBuilding(BuildingTypes eBuilding, CommerceTypes eCommerce, int iChange);
	void updateYieldModifierByBuilding(BuildingTypes eBuilding, YieldTypes eYield, int iChange);
	//int getImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const;
	//void changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange);
	int calculateBonusDefense() const;

	void setCivilizationType(int iCiv);

	int getBuildingCommerceModifier(BuildingTypes eBuilding, CommerceTypes eCommerce) const;
	int getBuildingYieldModifier(BuildingTypes eBuilding, YieldTypes eYield) const;

	int calculateBuildingCommerceModifier(CommerceTypes eCommerce) const;
	int calculateBuildingYieldModifier(YieldTypes eYield) const;
	int getRevTrend() const;
	bool isInquisitionConditions() const;
	int calculateCorporationHealth() const;
	int calculateCorporationHappiness() const;

	BuildTypes findChopBuild(FeatureTypes eFeature) const;
	CultureLevelTypes getOccupationCultureLevel() const;
	void setOccupationCultureLevel(CultureLevelTypes eNewValue);
	CultureLevelTypes getMaxCultureLevelAmongPlayers() const;
	CultureLevelTypes getCultureLevel(PlayerTypes eIndex) const;
	CultureLevelTypes getCultureLevelForCulture(int iCulture) const;
	int getLineOfSight() const;
	void changeLineOfSight(int iChange);
	int calculateBonusCommerceRateModifier(CommerceTypes eIndex) const;
	int getLandmarkAngerTimer() const;
	void changeLandmarkAngerTimer(int iChange);
	int getLandmarkAnger() const;
	void clearVicinityBonusCache(BonusTypes eBonus);
	bool hasVicinityBonus(BonusTypes eBonus) const;
	void clearRawVicinityBonusCache(BonusTypes eBonus);
	bool hasRawVicinityBonus(BonusTypes eBonus) const;
	void checkBuildings(bool bAlertOwner = true);
	void doVicinityBonus();
	bool isDevelopingCity() const;

	int getMintedCommerceTimes100() const;

	int getUnitCombatExtraStrength(UnitCombatTypes eIndex) const;
	void changeUnitCombatExtraStrength(UnitCombatTypes eIndex, int iChange);

	int getInvasionChance() const;
	void changeInvasionChance(int iChange);

	int getInvasionTimer() const;
	void changeInvasionTimer(int iChange);
	bool isInvaded() const;

	void doInvasion();

	void setDisabledBuilding(const BuildingTypes eIndex, const bool bNewValue, const bool bProcess = true);
	bool isDisabledBuilding(const short iIndex) const;

	void setReligiouslyLimitedBuilding(BuildingTypes eIndex, bool bNewValue);
	bool isReligiouslyLimitedBuilding(BuildingTypes eIndex) const;

	bool isZoneOfControl() const;

	int getAdjacentDamagePercent() const;
	void changeAdjacentDamagePercent(int iChange);

	int getWorkableRadiusOverride() const;
	void setWorkableRadiusOverride(int iNewVal);

	int getProtectedCultureCount() const;
	bool isProtectedCulture() const;
	void changeProtectedCultureCount(int iChange);

	int getNumUnitFullHeal() const;
	void changeNumUnitFullHeal(int iChange);

	int getCommerceAttacks(CommerceTypes eIndex) const;
	void changeCommerceAttacks(CommerceTypes eIndex, int iChange);
	int getMaxCommerceAttacks(CommerceTypes eIndex) const;
	void changeMaxCommerceAttacks(CommerceTypes eIndex, int iChange);

	void doAttack();
	void doHeal();
	void decayCommerce();

	void doCorporation();
	int getCorporationInfluence(CorporationTypes eCorporation) const;
	int calculateCorporateTaxes() const;

	int getDisabledPowerTimer() const;
	void changeDisabledPowerTimer(int iChange);
	void doDisabledPower();

	int getWarWearinessTimer() const;
	void changeWarWearinessTimer(int iChange);
	void doWarWeariness();

	int getEventAnger() const;
	void changeEventAnger(int iChange);

	int getNonHolyReligionCount() const;

	void calculateExtraTradeRouteProfit(int iExtra, int* &aiTradeYields) const;

	int getMinimumDefenseLevel() const;
	void setMinimumDefenseLevel(int iNewValue);

	SpecialistTypes getBestSpecialist(int iExtra) const;

	int getNumPopulationEmployed() const;
	void setNumPopulationEmployed(int iNewValue);
	void changeNumPopulationEmployed(int iNewValue);

	void removeWorstCitizenActualEffects(int iNumCitizens, int& iGreatPeopleRate, int& iHappiness, int& iHealthiness, int*& aiYields, int*& aiCommerces) const;

	void changeHealthPercentPerPopulation(int iChange);
	int calculatePopulationHealth() const;
	void changeHappinessPercentPerPopulation(int iChange);
	int calculatePopulationHappiness() const;

	int getAssignedSpecialistCount() const;

	int getBonusCommercePercentChanges(CommerceTypes eIndex) const;
	int getBonusCommercePercentChanges(CommerceTypes eIndex, BonusTypes eBonus) const;
	int getBonusCommercePercentChanges(CommerceTypes eIndex, BuildingTypes eBuilding) const;
	void changeBonusCommercePercentChanges(CommerceTypes eIndex, int iChange);

	bool isAutomatedCanBuild(BuildTypes eBuild) const;
	void setAutomatedCanBuild(BuildTypes eBuild, bool bNewValue);

	virtual bool AI_isEmphasizeAvoidAngryCitizens() const = 0;
	virtual bool AI_isEmphasizeAvoidUnhealthyCitizens() const = 0;

	virtual int AI_plotValue(const CvPlot* pPlot, bool bAvoidGrowth, bool bRemove, bool bIgnoreFood = false, bool bIgnoreGrowth = false, bool bIgnoreStarvation = false) const = 0;

	virtual int AI_getMilitaryProductionRateRank() const = 0;
	virtual int AI_getNavalMilitaryProductionRateRank() const = 0;

	virtual bool AI_isMilitaryProductionCity() const = 0;
	virtual void AI_setMilitaryProductionCity(bool bNewVal) = 0;
	virtual bool AI_isNavalMilitaryProductionCity() const = 0;
	virtual void AI_setNavalMilitaryProductionCity(bool bNewVal) = 0;

	int getDistanceMaintenanceSavedTimes100ByCivic(CivicTypes eCivic) const;
	int getNumCitiesMaintenanceSavedTimes100ByCivic(CivicTypes eCivic) const;
	int getHomeAreaMaintenanceSavedTimes100ByCivic(CivicTypes eCivic) const;
	int getOtherAreaMaintenanceSavedTimes100ByCivic(CivicTypes eCivic) const;

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);

	virtual void AI_init() = 0;
	virtual void AI_reset() = 0;
	virtual void AI_doTurn() = 0;
	virtual void AI_assignWorkingPlots() = 0;
	virtual void AI_updateAssignWork() = 0;
	virtual bool AI_avoidGrowth() = 0;
	virtual int AI_specialistValue(SpecialistTypes eSpecialist, bool bAvoidGrowth, bool bRemove) const = 0;
	virtual void AI_chooseProduction() = 0;
	//	KOSHLING - initialisation called on every city prior to performing unit mission allocation logic
	//	This allows caches that will remian valid for the procesign of teh current turn's units to be cleared
	virtual void AI_preUnitTurn() = 0;
	virtual void AI_noteUnitEscortNeeded() = 0;
	virtual void AI_trained(UnitTypes eUnitType, UnitAITypes eUnitAIType) = 0;
	virtual UnitTypes AI_bestUnit(int& iBestValue, int iNumSelectableTypes = -1, UnitAITypes* pSelectableTypes = NULL, bool bAsync = false, UnitAITypes* peBestUnitAI = NULL, bool bNoRand = false, bool bNoWeighting = false, const CvUnitSelectionCriteria* criteria = NULL) = 0;
	virtual UnitTypes AI_bestUnitAI(UnitAITypes eUnitAI, int& iBestValue, bool bAsync = false, bool bNoRand = false, const CvUnitSelectionCriteria* criteria = NULL) = 0;

	virtual void AI_FlushBuildingValueCache(bool bRetainValues = false) = 0;

	// Represents a building with associated score as measured by the AI
	struct ScoredBuilding
	{
		ScoredBuilding(BuildingTypes building = NO_BUILDING, int score = -1) : building(building), score(score) {}
		bool operator<(const ScoredBuilding& other) const { return score < other.score; }

		BuildingTypes building;
		int score;

		// Get some interesting stats about a set of scored buildings
		static void averageMinMax(const std::vector<ScoredBuilding>& scores, float& averageScore, int& minScore, int& maxScore)
		{
			averageScore = 0;
			minScore = MAX_INT;
			maxScore = -MAX_INT;
			foreach_(const ScoredBuilding& itr, scores)
			{
				averageScore = averageScore + itr.score / scores.size();
				minScore = std::min(minScore, itr.score);
				maxScore = std::max(maxScore, itr.score);
			}
		}
	};

	// Evaluate a predefined list of buildings based on the specified criteria, returning a sorted list of the buildings and their scores
	virtual bool AI_scoreBuildingsFromListThreshold(std::vector<ScoredBuilding>& scoredBuildings, const std::vector<BuildingTypes>& possibleBuildings, int iFocusFlags = 0, int iMaxTurns = MAX_INT, int iMinThreshold = 0, bool bAsync = false, AdvisorTypes eIgnoreAdvisor = NO_ADVISOR, bool bMaximizeFlaggedValue = false, PropertyTypes eProperty = NO_PROPERTY) = 0;
	virtual int AI_buildingValue(BuildingTypes eBuilding, int iFocusFlags = 0, bool bForTech = false) = 0;
	virtual int AI_projectValue(ProjectTypes eProject) const = 0;
	virtual int AI_neededSeaWorkers() const = 0;
	virtual bool AI_isDefended(int iExtra = 0, bool bAllowAnyDefenders = true) = 0;

	virtual bool AI_isAirDefended(bool bCountLand = 0, int iExtra = 0) = 0;
	virtual bool AI_isAdequateHappinessMilitary(int iExtra = 0) const = 0;

	virtual bool AI_isDanger() const = 0;
	virtual int evaluateDanger() = 0;
	virtual int AI_neededDefenders() = 0;
	virtual int AI_neededAirDefenders() const = 0;
	virtual int AI_minDefenders() const = 0;
	virtual bool AI_isEmphasizeAvoidGrowth() const = 0;
	virtual bool AI_isAssignWorkDirty() const = 0;
	virtual CvCity* AI_getRouteToCity() const = 0;
	virtual void AI_setAssignWorkDirty(bool bNewValue) = 0;
	virtual bool AI_isChooseProductionDirty() const = 0;
	virtual void AI_setChooseProductionDirty(bool bNewValue) = 0;
	virtual bool AI_isEmphasize(EmphasizeTypes eIndex) const = 0;
	virtual void AI_setEmphasize(EmphasizeTypes eIndex, bool bNewValue) = 0;
	virtual bool AI_isEmphasizeSpecialist(SpecialistTypes eIndex) const = 0;
	virtual void AI_setEmphasizeSpecialist(SpecialistTypes eIndex, bool bNewValue) = 0;
	virtual int AI_getBestBuildValue(int iIndex) const = 0;
	virtual void AI_markBestBuildValuesStale() = 0;

	virtual int AI_getTargetSize() const = 0;
	virtual int AI_getGoodTileCount() const = 0;
	virtual int AI_getImprovementValue(const CvPlot* pPlot, ImprovementTypes eImprovement, int iFoodPriority, int iProductionPriority, int iCommercePriority, int iFoodChange) const = 0;
	virtual void AI_getYieldMultipliers(int &iFoodMultiplier, int &iProductionMultiplier, int &iCommerceMultiplier, int &iDesiredFoodChange) const = 0;

	virtual int AI_totalBestBuildValue(const CvArea* pArea) const = 0;
	virtual int AI_countBestBuilds(const CvArea* pArea) const = 0;
	virtual BuildTypes AI_getBestBuild(int iIndex) const = 0;
	virtual void AI_updateBestBuild() = 0;
	virtual int AI_cityValue() const = 0;
	virtual int AI_clearFeatureValue(int iIndex) const = 0;

	virtual int AI_calculateCulturePressure(bool bGreatWork = false) const = 0;
	virtual int AI_calculateWaterWorldPercent() const = 0;
	virtual int AI_countNumBonuses(BonusTypes eBonus, bool bIncludeOurs, bool bIncludeNeutral, int iOtherCultureThreshold, bool bLand = true, bool bWater = true) const = 0;
	virtual int AI_yieldMultiplier(YieldTypes eYield) const = 0;
	virtual int AI_playerCloseness(PlayerTypes eIndex, int iMaxDistance = 7) = 0;
	virtual int AI_cityThreat(TeamTypes eTargetTeam = NO_TEAM, int* piThreatModifier = NULL) = 0;
	virtual BuildingTypes AI_bestAdvancedStartBuilding(int iPass) = 0;

	virtual int AI_getWorkersHave() const = 0;
	virtual int AI_getWorkersNeeded() const = 0;
	virtual void AI_changeWorkersHave(int iChange) = 0;

	// Fuyu - Worker Counting - 03.08.2010
	virtual int AI_workingCityPlotTargetMissionAIs(PlayerTypes ePlayer, MissionAITypes eMissionAI, UnitAITypes eUnitAI = NO_UNITAI, bool bSameAreaOnly = false) const = 0;

	virtual int AI_getBuildPriority() const = 0;

	bool hasShrine(ReligionTypes eReligion) const;
	bool hasOrbitalInfrastructure() const;
	void processVoteSourceBonus(VoteSourceTypes eVoteSource, bool bActive);

	void invalidatePopulationRankCache();
	void invalidateYieldRankCache(YieldTypes eYield = NO_YIELD);
	void invalidateCommerceRankCache(CommerceTypes eCommerce = NO_COMMERCE);

	int getBestYieldAvailable(YieldTypes eYield) const;

/************************************************************************************************/
/* phunny_pharmer                Start		 05/03/10                                           */
/*   note: recalculateCultureDistance must be const as it is called from cultureDistance, a     */
/*     const function; this means that the actual cached structure must be mutable in order to  */
/*     be modified in the const method                                                          */
/************************************************************************************************/
	void recalculateCultureDistances(int iMaxDistance) const;
	int calculateCultureDistance(int iDX, int iDY, int iMaxDistance) const;
	void clearCultureDistanceCache();
	int cultureDistance(int iDX, int iDY, bool bForce = false) const;
/************************************************************************************************/
/* phunny_pharmer                End		 05/03/10                                           */
/************************************************************************************************/
	void clearModifierTotals();
	void recalculateModifiers();

	void setBuildingListInvalid();
	bool getBuildingListFilterActive(BuildingFilterTypes eFilter) const;
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

	void setUnitListInvalid();
	bool getUnitListFilterActive(UnitFilterTypes eFilter) const;
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

	bool isDirectAttackable() const;

protected:

	int m_iID;
	int m_iX;
	int m_iY;
	int m_iRallyX;
	int m_iRallyY;
	int m_iGameTurnFounded;
	int m_iGameTurnAcquired;
	int m_iPopulation;
	int m_iHighestPopulation;
	int m_iWorkingPopulation;
	int m_iSpecialistPopulation;
	int m_iNumGreatPeople;
	int m_iBaseGreatPeopleRate;
	int m_iGreatPeopleRateModifier;
	int m_iGreatPeopleProgress;
	int m_iNumWorldWonders;
	int m_iNumTeamWonders;
	int m_iNumNationalWonders;
	int m_iNumBuildings;
	int m_iGovernmentCenterCount;
	mutable int m_iMaintenance;
	int m_iMaintenanceModifier;
	int m_iWarWearinessModifier;
	int m_iHurryAngerModifier;
	int m_iHealRate;
	int m_iEspionageHealthCounter;
	int m_iEspionageHappinessCounter;
	int m_iFreshWaterGoodHealth;
	int m_iFeatureGoodHealth;
	int m_iFeatureBadHealth;
	int m_iBuildingGoodHealth;
	int m_iBuildingBadHealth;
	int m_iBonusGoodHealth;
	int m_iBonusBadHealth;
	int m_iHurryAngerTimer;
	int m_iRevRequestAngerTimer;
	int m_iRevSuccessTimer;
	int m_iConscriptAngerTimer;
	int m_iDefyResolutionAngerTimer;
	int m_iHappinessTimer;
	int m_iMilitaryHappinessUnits;
	int m_iBuildingGoodHappiness;
	int m_iBuildingBadHappiness;
	int m_iExtraBuildingGoodHappiness;
	int m_iExtraBuildingBadHappiness;
	int m_iExtraBuildingGoodHealth;
	int m_iExtraBuildingBadHealth;
	int m_iFeatureGoodHappiness;
	int m_iFeatureBadHappiness;
	int m_iBonusGoodHappiness;
	int m_iBonusBadHappiness;
	int m_iReligionGoodHappiness;
	int m_iReligionBadHappiness;
	int m_iExtraHappiness;
	int m_iExtraHealth;
	int m_iNoUnhappinessCount;
	int m_iNoUnhealthyPopulationCount;
	int m_iBuildingOnlyHealthyCount;
	int m_iFood;
	int m_iFoodKept;
	float m_fMaxFoodKeptMultiplierLog;
#define INVALID_STORED_FOOD_PERCENT_LOG (-1000000)	//	Used as a reserved value to trigger calculation on upgrade of save format
	int m_iOverflowProduction;
	int m_iFeatureProduction;

	int m_iSpecialistGoodHealth;
	int m_iSpecialistBadHealth;
	int m_iSpecialistHappiness;
	int m_iSpecialistUnhappiness;
	int m_iImprovementGoodHealth;
	int m_iImprovementBadHealth;
	int m_iLostProductionModified;
	int m_iGoldFromLostProduction;
	int m_iCiv;
	float m_fPopulationgrowthratepercentageLog;

	bool m_bBuiltFoodProducedUnit;
	bool m_bResetTechs;
	int m_iLineOfSight;
	int m_iLandmarkAngerTimer;
	int m_iInvasionChance;
	int m_iInvasionTimer;
	int m_iAdjacentDamagePercent;
	int m_iWorkableRadiusOverride;
	int m_iProtectedCultureCount;
	int m_iNumUnitFullHeal;
	int m_iDisabledPowerTimer;
	int m_iWarWearinessTimer;
	int m_iMinimumDefenseLevel;
	int m_iNumPopulationEmployed;
	int m_iHappinessPercentPerPopulation;
	int m_iHealthPercentPerPopulation;

	int m_iLostProduction;
	int m_iEventAnger;

	int m_iFreshWater;

	int* m_aiBonusCommerceRateModifier;
	int* m_aiBonusCommercePercentChanges;
	mutable int* m_cachedPropertyNeeds;
	bool* m_pabHadVicinityBonus;
	bool* m_pabHadRawVicinityBonus;
	mutable bool* m_pabHasVicinityBonusCached;
	mutable bool* m_pabHasRawVicinityBonusCached;
	mutable bool* m_pabHasVicinityBonus;
	mutable bool* m_pabHasRawVicinityBonus;

	bool* m_pabReligiouslyDisabledBuilding;
	int* m_paiUnitCombatExtraStrength;
	int* m_aiCommerceAttacks;
	int* m_aiMaxCommerceAttacks;
	bool* m_pabAutomatedCanBuild;

	std::vector<BuildingCommerceModifier> m_aBuildingCommerceModifier;
	std::vector<BuildingYieldModifier> m_aBuildingYieldModifier;

	std::vector<PropertySpawns> m_aPropertySpawns;

	std::vector<short> m_vFreeBuildings;
	std::vector<short> m_vDisabledBuildings;
	std::map<short, uint16_t> m_freeAreaBuildingCount;
	std::map<short, int> m_bonusDefenseChanges;
	std::map<short, int> m_buildingProductionMod;
	std::map<short, int> m_unitProductionMod;

	CultureLevelTypes m_eOccupationCultureLevel;

	int m_iMilitaryProductionModifier;
	int m_iSpaceProductionModifier;
	int m_iExtraTradeRoutes;
	int m_iTradeRouteModifier;
	int m_iForeignTradeRouteModifier;
	int m_iBuildingDefense;
	int m_iBuildingBombardDefense;
	int m_iFreeExperience;
	int m_iCurrAirlift;
	int m_iMaxAirlift;
	int m_iAirModifier;
	int m_iAirUnitCapacity;
	int m_iNukeModifier;
	int m_iFreeSpecialist;
	int m_iPowerCount;
	int m_iDefenseDamage;
	int m_iLastDefenseDamage;
	int m_iOccupationTimer;
	int m_iCultureUpdateTimer;
	int m_iCitySizeBoost;
	int m_iSpecialistFreeExperience;
	int m_iEspionageDefenseModifier;
	int m_iSpecialistInsidiousness;
	int m_iSpecialistInvestigation;
	// Mutable as its used in caching
	mutable int m_icachedPropertyNeedsTurn;

	int m_iQuarantinedCount;

	bool m_bNeverLost;
	bool m_bPropertyControlBuildingQueued;
	bool m_bBombarded;
	bool m_bDrafted;
	bool m_bAirliftTargeted;
	bool m_bWeLoveTheKingDay;
	bool m_bCitizensAutomated;
	bool m_bProductionAutomated;
	bool m_bWallOverride;
	bool m_bInfoDirty;
	bool m_bLayoutDirty;
	bool m_bPlundered;
	bool m_bPopProductionProcess;

	PlayerTypes m_eOwner;
	PlayerTypes m_ePreviousOwner;
	PlayerTypes m_eOriginalOwner;
	CultureLevelTypes m_eCultureLevel;

	int m_iRevolutionIndex;
	int m_iLocalRevIndex;
	int m_iRevIndexAverage;
	int m_iRevolutionCounter;
	int m_iReinforcementCounter;

	//TB Combat Mod (Buildings)
#ifdef STRENGTH_IN_NUMBERS
	int m_iTotalFrontSupportPercentModifier;
	int m_iTotalShortRangeSupportPercentModifier;
	int m_iTotalMediumRangeSupportPercentModifier;
	int m_iTotalLongRangeSupportPercentModifier;
	int m_iTotalFlankSupportPercentModifier;
#endif
	int* m_paiNewAfflictionTypeCount;
	int* m_paiAidRate;
	int** m_ppaaiExtraBonusAidModifier;
	int* m_paiNewExtraAfflictionOutbreakLevelChange;
	int* m_paiNewAfflictionToleranceChange;
	int* m_paiNewCurrentOvercomeChange;
	int* m_paiUnitCombatProductionModifier;
	int* m_paiUnitCombatRepelModifier;
	int* m_paiUnitCombatRepelAgainstModifier;
	int* m_paiUnitCombatDefenseAgainstModifier;
	int* m_paiPromotionLineAfflictionAttackCommunicability;
	int* m_paiUnitCombatOngoingTrainingTimeCount;
	int* m_paiUnitCombatOngoingTrainingTimeIncrement;
	//TB Building Tags
	int m_iExtraLocalCaptureProbabilityModifier;
	int m_iExtraLocalCaptureResistanceModifier;
	int m_iExtraLocalDynamicDefense;
	int m_iExtraRiverDefensePenalty;
	int m_iExtraLocalRepel;
	int m_iExtraMinDefense;
	int m_iExtraBuildingDefenseRecoverySpeedModifier;
	int m_iModifiedBuildingDefenseRecoverySpeedCap;
	int m_iExtraCityDefenseRecoverySpeedModifier;

	int** m_ppaaiTechSpecialistHappinessTypes;
	int* m_paiTechSpecialistHappiness;
	int* m_paiTechHappiness;
	int m_iExtraTechSpecialistHappiness;
	int m_iExtraTechHappiness;
	int** m_ppaaiTechSpecialistHealthTypes;
	int* m_paiTechSpecialistHealth;
	int* m_paiTechHealth;
	int m_iExtraTechSpecialistHealth;
	int m_iExtraTechHealth;
	int** m_ppaaiLocalSpecialistExtraYield;
	int** m_ppaaiLocalSpecialistExtraCommerce;
	int m_iPrioritySpecialist;
	int* m_paiSpecialistBannedCount;
	int* m_paiDamageAttackingUnitCombatCount;
	int* m_paiBuildingCostPopulationCount;
	int* m_paiHealUnitCombatTypeVolume;
	int m_iExtraInsidiousness;
	int m_iExtraInvestigation;

	int* m_aiSeaPlotYield;
	int* m_aiRiverPlotYield;
	int* m_aiBaseYieldRate;
	int* m_aiExtraYield;
	int* m_aiBaseYieldPerPopRate;
	int* m_aiYieldRateModifier;
	int* m_aiPowerYieldRateModifier;
	int* m_aiBonusYieldRateModifier;
	int* m_aiTradeYield;
	int* m_aiCorporationYield;
	int* m_aiExtraSpecialistYield;
	int* m_aiExtraSpecialistCommerce;
	mutable int* m_aiCommerceRate;
	mutable bool* m_abCommerceRateDirty;
	int* m_aiProductionToCommerceModifier;
	int* m_aiBuildingCommerce;
	int* m_aiSpecialistCommerce100;
	int* m_aiReligionCommerce;
	int* m_aiCorporationCommerce;
	int* m_aiCommerceRateModifier;
	int* m_aiCommerceHappinessPer;
	int* m_aiDomainFreeExperience;
	int* m_aiDomainProductionModifier;
	int* m_aiCulture;
	int* m_aiNumRevolts;

	bool* m_abEverOwned;
	bool* m_abTradeRoute;
	bool* m_abRevealed;
	bool* m_abEspionageVisibility;

	CvWString m_szName;
	CvString m_szScriptData;

	int* m_paiNoBonus;
	int* m_paiFreeBonus;
	int* m_paiNumBonuses;
	int* m_paiNumCorpProducedBonuses;
	int* m_paiProjectProduction;
	int* m_paiBuildingProduction;
	int* m_paiBuildingProductionTime;
	int* m_paiBuildingOriginalOwner;
	int* m_paiBuildingOriginalTime;
	int* m_paiUnitProduction;
	int* m_paiUnitProductionTime;
	int* m_paiGreatPeopleUnitRate;
	int* m_paiGreatPeopleUnitProgress;
	int* m_paiSpecialistCount;
	int* m_paiMaxSpecialistCount;
	int* m_paiForceSpecialistCount;
	int* m_paiFreeSpecialistCount;
	int* m_paiFreeSpecialistCountUnattributed;
	int* m_paiImprovementFreeSpecialists;
	int* m_paiReligionInfluence;
	int* m_paiStateReligionHappiness;
	int* m_paiUnitCombatFreeExperience;
	int* m_paiFreePromotionCount;
	int* m_paiNumRealBuilding;

	bool* m_pabWorkingPlot;
	bool* m_pabHasReligion;
	bool* m_pabHasCorporation;

	int	m_deferringBonusProcessingCount;
	int* m_paiStartDeferredSectionNumBonuses;

	CvProperties m_Properties;
	CvBuildingList m_BuildingList;
	CvUnitList m_UnitList;

	std::vector<IDInfo> m_paTradeCities;

	typedef std::vector<OrderData> OrderQueue;
	OrderQueue m_orderQueue;

	std::vector< std::pair < float, float> > m_kWallOverridePoints;

	std::vector<EventTypes> m_aEventsOccured;
	std::vector<BuildingYieldChange> m_aBuildingYieldChange;
	std::vector<BuildingCommerceChange> m_aBuildingCommerceChange;
	BuildingChangeArray m_aBuildingHappyChange;
	BuildingChangeArray m_aBuildingHealthChange;

	// CACHE: cache frequently used values
	int* m_aiCachedBestSeeInvisibleUnit;
	bool* m_abCachedBestSeeInvisibleUnit;
	int* m_aiCachedBestSeeInvisibleUnitSea;
	bool* m_abCachedBestSeeInvisibleUnitSea;
	mutable int	m_iPopulationRank;
	mutable bool m_bPopulationRankValid;
	int*	m_aiBaseYieldRank;
	bool*	m_abBaseYieldRankValid;
	int*	m_aiYieldRank;
	bool*	m_abYieldRankValid;
	int*	m_aiCommerceRank;
	bool*	m_abCommerceRankValid;

	mutable std::map<int,int> m_aCultureDistances;

	void doGrowth();
	void doCulture();
	void doPlotCulture(bool bUpdate, PlayerTypes ePlayer, int iCultureRate);
	void doProduction(bool bAllowNoProduction);
	void doDecay();
	void doReligion();
	void doGreatPeople();
	void doMeltdown();
	bool doCheckProduction();
	void doPromotion();

	int getHurryCostModifier(UnitTypes eUnit) const;
	int getHurryCostModifier(BuildingTypes eBuilding) const;
	int getHurryCostModifier(int iBaseModifier, int iExtraMod) const;
	int getHurryCost(UnitTypes eUnit) const;
	int getHurryCost(BuildingTypes eBuilding) const;
	int getHurryCost(int iProductionLeft, int iHurryModifier) const;
	int getHurryPopulation(HurryTypes eHurry, int iHurryCost) const;
	bool canHurryUnit(HurryTypes eHurry, UnitTypes eUnit) const;
	bool canHurryBuilding(HurryTypes eHurry, BuildingTypes eBuilding) const;
	void recalculateMaxFoodKeptPercent();
	void recalculatePopulationgrowthratepercentage();
	virtual bool AI_addBestCitizen(bool bWorkers, bool bSpecialists, int* piBestPlot = NULL, SpecialistTypes* peBestSpecialist = NULL) = 0;
	virtual bool AI_removeWorstCitizen(SpecialistTypes eIgnoreSpecialist = NO_SPECIALIST) = 0;

	//TB Building tags
	void setExtraLocalCaptureProbabilityModifier(int iValue);
	void changeExtraLocalCaptureProbabilityModifier(int iChange);

	void setExtraLocalCaptureResistanceModifier(int iValue);
	void changeExtraLocalCaptureResistanceModifier(int iChange);

	short m_iZoCCount;
	void changeZoCCount(short iChange);
public:
	int getTechSpecialistHappinessTypes(TechTypes eTech, SpecialistTypes eSpecialist) const;

	int localCitizenCaptureResistance() const;
	int getTechSpecialistHappiness(TechTypes eTech) const;
	int getExtraTechHappinessTotal() const;
	int getExtraTechUnHappinessTotal() const;
	int getTechSpecialistHealthTypes(TechTypes eTech, SpecialistTypes eSpecialist) const;
	int getTechSpecialistHealth(TechTypes eTech) const;
	int getExtraTechHealthTotal() const;
	int getExtraTechUnHealthTotal() const;
	int getLocalSpecialistExtraYield(SpecialistTypes eSpecialist, YieldTypes eYield) const;
	int getLocalSpecialistExtraCommerce(SpecialistTypes eSpecialist, CommerceTypes eCommerce) const;
private:
	void changeTechSpecialistHappinessTypes(TechTypes eTech, SpecialistTypes eSpecialist, int iChange);
	void changeTechSpecialistHappiness(TechTypes eTech, int iChange);
	void updateExtraTechSpecialistHappiness();
	int getExtraTechSpecialistHappiness() const;
	int getTechHappiness(TechTypes eTech) const;
	void changeTechHappiness(TechTypes eTech, int iChange);
	void updateExtraTechHappiness();
	int getExtraTechHappiness() const;
	void changeTechSpecialistHealthTypes(TechTypes eTech, SpecialistTypes eSpecialist, int iChange);
	void changeTechSpecialistHealth(TechTypes eTech, int iChange);
	void updateExtraTechSpecialistHealth();
	int getExtraTechSpecialistHealth() const;
	int getTechHealth(TechTypes eTech) const;
	void changeTechHealth(TechTypes eTech, int iChange);
	void updateExtraTechHealth();
	int getExtraTechHealth() const;
	void changeLocalSpecialistExtraYield(SpecialistTypes eSpecialist, YieldTypes eYield, int iChange);
	void changeLocalSpecialistExtraCommerce(SpecialistTypes eSpecialist, CommerceTypes eCommerce, int iChange);
public:
	int specialistCount(SpecialistTypes eSpecialist) const;
	int specialistYield(SpecialistTypes eSpecialist, YieldTypes eYield) const;
	int specialistCommerce(SpecialistTypes eSpecialist, CommerceTypes eCommerce) const;
	int specialistYieldTotal(SpecialistTypes eSpecialist, YieldTypes eYield) const;

	int getPrioritorizedSpecialist() const;
	void setPrioritorizedSpecialist(SpecialistTypes eSpecialist);

	bool isSpecialistBanned(SpecialistTypes eSpecialist);
	void banSpecialist(SpecialistTypes eSpecialist);
	void removeSpecialistBan(SpecialistTypes eSpecialist);


#ifdef YIELD_VALUE_CACHING
	virtual void AI_NoteWorkerChange() = 0;
	virtual void AI_NoteSpecialistChange() = 0;
public:
	virtual void ClearYieldValueCache() = 0;
#ifdef _DEBUG
	virtual void CheckYieldValueCache(char* label) = 0;
#define CHECK_YIELD_VALUE_CACHE(label) CheckYieldValueCache(label);
#else
#define CHECK_YIELD_VALUE_CACHE(label) ;
#endif
#endif

	//	Koshling - add cache of trainability of units which will be
	//	populated prior to calculating the city's build choices and
	//	then invalidated so it is only used within that scope
#ifdef CAN_TRAIN_CACHING
public:
//Team Project (3)
	int getExtraLocalCaptureProbabilityModifier() const;
	int getExtraLocalCaptureResistanceModifier() const;
//Team Project (1)
	void updateTechHappinessandHealth();

	int getExtraLocalDynamicDefense() const;
	void setExtraLocalDynamicDefense(int iValue);
	void changeExtraLocalDynamicDefense(int iChange);

	int getExtraRiverDefensePenalty() const;
	void setExtraRiverDefensePenalty(int iValue);
	void changeExtraRiverDefensePenalty(int iChange);

	int getExtraLocalRepel() const;
	void setExtraLocalRepel(int iValue);
	void changeExtraLocalRepel(int iChange);

	int getExtraMinDefense() const;
	void setExtraMinDefense(int iValue);
	void changeExtraMinDefense(int iChange);

	int getExtraBuildingDefenseRecoverySpeedModifier() const;
	void setExtraBuildingDefenseRecoverySpeedModifier(int iValue);
	void changeExtraBuildingDefenseRecoverySpeedModifier(int iChange);

	int getModifiedBuildingDefenseRecoverySpeedCap() const;
	void setModifiedBuildingDefenseRecoverySpeedCap(int iValue);
	void changeModifiedBuildingDefenseRecoverySpeedCap(int iChange);

	int getExtraCityDefenseRecoverySpeedModifier() const;
	void setExtraCityDefenseRecoverySpeedModifier(int iValue);
	void changeExtraCityDefenseRecoverySpeedModifier(int iChange);

	bool canDamageAttackingUnitCombat(UnitCombatTypes eUnitCombat) const;
	int getDamageAttackingUnitCombatCount(UnitCombatTypes eUnitCombat) const;
	void setDamageAttackingUnitCombatCount(UnitCombatTypes eUnitCombat, int iValue);
	void changeDamageAttackingUnitCombatCount(UnitCombatTypes eUnitCombat, int iChange);

	bool canBuildingCostPopulation(BuildingTypes eBuilding) const;
	int getBuildingCostPopulation(BuildingTypes eBuilding) const;
	void setBuildingCostPopulation(BuildingTypes eBuilding, int iValue);
	void changeBuildingCostPopulationCount(BuildingTypes eBuilding, int iChange);

	int cityDefenseRecoveryRate() const;
	int getInvestigationTotal(bool bActual = false) const;

	int getExtraInsidiousness() const;
	void setExtraInsidiousness(int iValue);
	void changeExtraInsidiousness(int iChange);

	int getExtraInvestigation() const;
	void setExtraInvestigation(int iValue);
	void changeExtraInvestigation(int iChange);

	int getSpecialistInsidiousness() const;
	void changeSpecialistInsidiousness(int iChange);
	int getSpecialistInvestigation() const;
	void changeSpecialistInvestigation(int iChange);

	int getPropertyNeed(PropertyTypes eProperty) const;
	int getNumPropertySpawns() const;
	PropertySpawns& getPropertySpawn(int iIndex);
	void changePropertySpawn(int iChange, PropertyTypes eProperty, UnitTypes eUnit);
	void doPropertyUnitSpawn();

	void AI_setPropertyControlBuildingQueued(bool bSet);
	bool AI_isPropertyControlBuildingQueued() const;
private:
	mutable stdext::hash_map<UnitTypes,bool> m_canTrainCacheUnits;
	mutable stdext::hash_map<UnitTypes,UnitTypes> m_eCachedAllUpgradesResults;
	mutable stdext::hash_map<UnitTypes,UnitTypes> m_eCachedAllUpgradesResultsRoot;
	mutable bool m_canTrainCachePopulated;
	mutable bool m_canTrainCacheDirty;
	mutable int m_cachedBuildingYieldModifers[NUM_YIELD_TYPES];
	bool m_bPlotWorkingMasked;
	mutable int m_totalCommerceRateModifier[NUM_COMMERCE_TYPES];

	mutable std::map<int,int> m_buildingSourcedPropertyCache;
	mutable std::map<int,int> m_unitSourcedPropertyCache;

	bool m_bIsGreatWallSeed;
	bool m_bVisibilitySetup;
	mutable bool m_bMaintenanceDirty;

	mutable std::map<int,bool>*	m_bCanConstruct;


public:
	void populateCanTrainCache(bool bUnconditional = true) const;
	void clearCanTrainCache() const;
	void clearUpgradeCache(UnitTypes eUnit) const;
protected:
	void invalidateCachedCanTrainForUnit(UnitTypes eUnit) const;
#endif

public:
	//
	// Algorithm/range helpers
	//
	struct fn {
		DECLARE_MAP_FUNCTOR(CvCity, void, startDeferredBonusProcessing);
		DECLARE_MAP_FUNCTOR(CvCity, void, endDeferredBonusProcessing);
		DECLARE_MAP_FUNCTOR(CvCity, void, doTurn);
		DECLARE_MAP_FUNCTOR(CvCity, void, clearCanTrainCache);
		DECLARE_MAP_FUNCTOR(CvCity, void, checkReligiousDisablingAllBuildings);
		DECLARE_MAP_FUNCTOR(CvCity, void, updateTechHappinessandHealth);
		DECLARE_MAP_FUNCTOR(CvCity, void, updateExtraSpecialistYield);
		DECLARE_MAP_FUNCTOR(CvCity, void, updateExtraSpecialistCommerce);
		DECLARE_MAP_FUNCTOR(CvCity, void, updateReligionCommerce);
		DECLARE_MAP_FUNCTOR(CvCity, void, updateBuildingCommerce);
		DECLARE_MAP_FUNCTOR(CvCity, void, updateCorporation);
		DECLARE_MAP_FUNCTOR(CvCity, void, updateYield);
		DECLARE_MAP_FUNCTOR(CvCity, void, onYieldChange);
		DECLARE_MAP_FUNCTOR(CvCity, void, clearTradeRoutes);
		DECLARE_MAP_FUNCTOR(CvCity, void, setBuildingListInvalid);
		DECLARE_MAP_FUNCTOR(CvCity, void, clearModifierTotals);
		DECLARE_MAP_FUNCTOR(CvCity, void, recalculateModifiers);
		DECLARE_MAP_FUNCTOR(CvCity, void, ClearYieldValueCache);
		DECLARE_MAP_FUNCTOR(CvCity, void, AI_preUnitTurn);
		DECLARE_MAP_FUNCTOR(CvCity, void, AI_assignWorkingPlots);
		DECLARE_MAP_FUNCTOR(CvCity, void, AI_updateAssignWork);
		DECLARE_MAP_FUNCTOR(CvCity, void, AI_markBestBuildValuesStale);
		DECLARE_MAP_FUNCTOR(CvCity, void, setupGraphical);
		DECLARE_MAP_FUNCTOR(CvCity, void, chooseProduction);
		DECLARE_MAP_FUNCTOR(CvCity, void, invalidatePopulationRankCache);
		DECLARE_MAP_FUNCTOR(CvCity, void, invalidateYieldRankCache);
		DECLARE_MAP_FUNCTOR(CvCity, void, invalidateCommerceRankCache);

		DECLARE_MAP_FUNCTOR_1(CvCity, void, updateExtraBuildingHappiness, bool);
		DECLARE_MAP_FUNCTOR_1(CvCity, void, updateExtraBuildingHealth, bool);
		DECLARE_MAP_FUNCTOR_1(CvCity, void, updateFeatureHappiness, bool);
		DECLARE_MAP_FUNCTOR_1(CvCity, void, updateReligionHappiness, bool);
		DECLARE_MAP_FUNCTOR_1(CvCity, void, setCommerceModifierDirty, CommerceTypes);
		DECLARE_MAP_FUNCTOR_1(CvCity, void, setCommerceDirty, CommerceTypes);
		DECLARE_MAP_FUNCTOR_1(CvCity, void, setLayoutDirty, bool);
		DECLARE_MAP_FUNCTOR_1(CvCity, void, FlushCanConstructCache, BuildingTypes);
		DECLARE_MAP_FUNCTOR_1(CvCity, void, AI_setAssignWorkDirty, bool);
		DECLARE_MAP_FUNCTOR_1(CvCity, void, AI_setChooseProductionDirty, bool);
		DECLARE_MAP_FUNCTOR_1(CvCity, void, AI_setMilitaryProductionCity, bool);
		DECLARE_MAP_FUNCTOR_1(CvCity, void, AI_setNavalMilitaryProductionCity, bool);
		DECLARE_MAP_FUNCTOR_1(CvCity, void, kill, bool);
		DECLARE_MAP_FUNCTOR_1(CvCity, void, setInfoDirty, bool);
		DECLARE_MAP_FUNCTOR_1(CvCity, void, changeFood, int);

		DECLARE_MAP_FUNCTOR_2(CvCity, void, setBuildingListFilterActive, BuildingFilterTypes, bool);
		DECLARE_MAP_FUNCTOR_2(CvCity, void, updateCommerce, CommerceTypes, bool);
		DECLARE_MAP_FUNCTOR_2(CvCity, void, setFreeBuilding, BuildingTypes, bool);
		DECLARE_MAP_FUNCTOR_2(CvCity, void, changeFreeAreaBuildingCount, BuildingTypes, int);
		DECLARE_MAP_FUNCTOR_2(CvCity, void, changeFreeSpecialistCount, SpecialistTypes, int);
		DECLARE_MAP_FUNCTOR_2(CvCity, void, processVoteSourceBonus, VoteSourceTypes, bool);
		DECLARE_MAP_FUNCTOR_2(CvCity, void, changeExtraYield, YieldTypes, int);

		DECLARE_MAP_FUNCTOR_CONST(CvCity, bool, isCapital);
		DECLARE_MAP_FUNCTOR_CONST(CvCity, bool, isNoUnhappiness);
		DECLARE_MAP_FUNCTOR_CONST(CvCity, bool, hasOrbitalInfrastructure);
		DECLARE_MAP_FUNCTOR_CONST(CvCity, bool, isConnectedToCapital);
		DECLARE_MAP_FUNCTOR_CONST(CvCity, bool, isGovernmentCenter);
		DECLARE_MAP_FUNCTOR_CONST(CvCity, bool, AI_isMilitaryProductionCity);
		DECLARE_MAP_FUNCTOR_CONST(CvCity, bool, AI_isNavalMilitaryProductionCity);
		DECLARE_MAP_FUNCTOR_CONST(CvCity, int, calculateCorporateTaxes);
		DECLARE_MAP_FUNCTOR_CONST(CvCity, int, getID);
		DECLARE_MAP_FUNCTOR_CONST(CvCity, int, getPopulation);
		DECLARE_MAP_FUNCTOR_CONST(CvCity, int, getRealPopulation);
		DECLARE_MAP_FUNCTOR_CONST(CvCity, int, goodHealth);
		DECLARE_MAP_FUNCTOR_CONST(CvCity, int, badHealth);
		DECLARE_MAP_FUNCTOR_CONST(CvCity, int, happyLevel);
		DECLARE_MAP_FUNCTOR_CONST(CvCity, int, unhappyLevel);
		DECLARE_MAP_FUNCTOR_CONST(CvCity, const CvWString, getName);
		DECLARE_MAP_FUNCTOR_CONST(CvCity, const CvArea*, area);
		DECLARE_MAP_FUNCTOR_CONST(CvCity, const CvPlot*, plot);

		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, bool, hasFullyActiveBuilding, BuildingTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, bool, canConstruct, BuildingTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, bool, canTrain, UnitTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, bool, isHasReligion, ReligionTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, bool, isHasCorporation, CorporationTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, bool, hasBonus, BonusTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, bool, isCoastal, int);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, int, getNumRealBuilding, BuildingTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, int, getNumActiveBuilding, BuildingTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, int, getCommerceRateTimes100, CommerceTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, int, getBaseCommerceRateTimes100, CommerceTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, int, getCultureTimes100, PlayerTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, int, getYieldRate, YieldTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, int, getBaseYieldRate, YieldTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, const CvPlotGroup*, plotGroup, PlayerTypes);

		DECLARE_MAP_FUNCTOR_CONST_2(CvCity, bool, isRevealed, TeamTypes, bool);
	};
};

#endif
