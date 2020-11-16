#pragma once

// city.h

#ifndef CIV4_CITY_H
#define CIV4_CITY_H

#include "CvDLLEntity.h"
#include "LinkedList.h"
#include "CvInfos.h"
#include "CvBuildingList.h"
#include "CvUnitList.h"
#include "CvGameObject.h"

class CvPlot;
class CvPlotGroup;
class CvArea;
class CvArtInfoBuilding;

//	KOSHLING - Cache yield values where possible
#define YIELD_VALUE_CACHING

//	Enable canTrain results to be cached within a (caller)defined scope
#define CAN_TRAIN_CACHING

// BUG - start
void addGoodOrBad(int iValue, int& iGood, int& iBad);
void subtractGoodOrBad(int iValue, int& iGood, int& iBad);
// BUG - end

//	Unit ordering criteria
class CvUnitSelectionCriteria
{
public:
	CvUnitSelectionCriteria() : m_eUnitAI(NO_UNITAI),
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
	{
	}

	CvString	getDescription()
	{
		if ( m_eProperty != NO_PROPERTY )
		{
			return CvString::format("Property %s for %S",
									m_bPropertyBeneficial ? "improver" : "worsener",
									GC.getPropertyInfo(m_eProperty).getDescription());
		}
		else
		{
			return CvString("");
		}
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

protected:
	CvGameObjectCity m_GameObject;

public:
/************************************************************************************************/
/* REVOLUTION_MOD                         03/29/09                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
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
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/

	void kill(bool bUpdatePlotGroups, bool bUpdateCulture = true); // Exposed to Python
	void killTestCheap(); // For testing, do not call in a game situation

	void doTurn();
	void doAutobuild();

	bool isCitySelected() const;
	DllExport bool canBeSelected() const;
	DllExport void updateSelectedCity(bool bTestProduction);

	void updateYield();

	void updateVisibility();
	bool isVisibilitySetup() const;

	void createGreatPeople(UnitTypes eGreatPersonUnit, bool bIncrementThreshold, bool bIncrementExperience); // Exposed to Python

	void doTask(TaskTypes eTask, int iData1 = -1, int iData2 = -1, bool bOption = false, bool bAlt = false, bool bShift = false, bool bCtrl = false); // Exposed to Python

	void chooseProduction(UnitTypes eTrainUnit = NO_UNIT, BuildingTypes eConstructBuilding = NO_BUILDING, ProjectTypes eCreateProject = NO_PROJECT, bool bFinish = false, bool bFront = false); // Exposed to Python

	// Base iterator type for iterating over city plots, returning valid ones only
	template < class Value_ >
	struct city_plot_iterator_base :
		public bst::iterator_facade<city_plot_iterator_base<Value_>, Value_*, bst::forward_traversal_tag, Value_*>
	{
		city_plot_iterator_base() : m_centerX(-1), m_centerY(-1), m_curr(nullptr), m_idx(0) {}
		explicit city_plot_iterator_base(int centerX, int centerY) : m_centerX(centerX), m_centerY(centerY), m_curr(nullptr), m_idx(0)
		{
			increment();
		}

	private:
		friend class bst::iterator_core_access;
		void increment()
		{
			m_curr = nullptr;
			while (m_curr == nullptr && m_idx < NUM_CITY_PLOTS)
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

		int m_centerX;
		int m_centerY;
		Value_* m_curr;
		int m_idx;
	};
	typedef city_plot_iterator_base<CvPlot> city_plot_iterator;

	city_plot_iterator beginPlots() const { return city_plot_iterator(getX(), getY()); }
	city_plot_iterator endPlots() const { return city_plot_iterator(); }

	typedef bst::iterator_range<city_plot_iterator> city_plot_range;
	city_plot_range plots() const { return city_plot_range(beginPlots(), endPlots()); }

	int getCityPlotIndex(const CvPlot* pPlot) const; // Exposed to Python
	// Prefer to use plots() range instead of this for loops, searching etc.
	CvPlot* getCityIndexPlot(int iIndex) const; // Exposed to Python

	bool canWork(const CvPlot* pPlot) const; // Exposed to Python
	void verifyWorkingPlot(int iIndex);
	void verifyWorkingPlots();
	void clearWorkingOverride(int iIndex); // Exposed to Python
	int countNumImprovedPlots(ImprovementTypes eImprovement = NO_IMPROVEMENT, bool bPotential = false) const; // Exposed to Python
	int countNumWaterPlots() const; // Exposed to Python
	int countNumRiverPlots() const; // Exposed to Python

	int findPopulationRank() const; // Exposed to Python
	int findBaseYieldRateRank(YieldTypes eYield) const; // Exposed to Python
	int findYieldRateRank(YieldTypes eYield) const; // Exposed to Python
	int findCommerceRateRank(CommerceTypes eCommerce) const; // Exposed to Python

	bool isSupersedingUnitAvailable(UnitTypes eUnit) const; // Exposed to Python
	bool isPlotTrainable(UnitTypes eUnit, bool bContinue, bool bTestVisible) const; // Exposed to Python

	UnitTypes allUpgradesAvailable(UnitTypes eUnit, int iUpgradeCount = 0) const; // Exposed to Python
	bool isWorldWondersMaxed() const; // Exposed to Python
	bool isTeamWondersMaxed() const; // Exposed to Python
	bool isNationalWondersMaxed() const; // Exposed to Python
	int getMaxNumWorldWonders() const; // Exposed to Python
	int getMaxNumTeamWonders() const; // Exposed to Python
	int getMaxNumNationalWonders() const; // Exposed to Python

	bool canTrain(UnitTypes eUnit, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, bool bIgnoreUpgrades = false, bool bPropertySpawn = false) const; // Exposed to Python
	bool canTrainInternal(UnitTypes eUnit, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, bool bIgnoreUpgrades = false) const; // Exposed to Python
	bool canTrain(UnitCombatTypes eUnitCombat) const;

	bool canConstruct(BuildingTypes eBuilding, bool bContinue = false, bool bTestVisible = false, bool bIgnoreCost = false, bool bIgnoreAmount = false, bool bIgnoreBuildings = false, TechTypes eIgnoreTechReq = NO_TECH, int* probabilityEverConstructable = NULL, bool bAffliction = false, bool bExposed = false) const; // Exposed to Python

	//	KOSHLING - cache can build results
	void FlushCanConstructCache(BuildingTypes eBuilding = NO_BUILDING);
	bool canConstructInternal(BuildingTypes eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreAmount, BuildingTypes withExtraBuilding = NO_BUILDING, bool bIgnoreBuildings = false, TechTypes eIgnoreTechReq = NO_TECH, int* probabilityEverConstructable = NULL, bool bAffliction = false, bool bExposed = false) const;
	void NoteBuildingNoLongerConstructable(BuildingTypes eBuilding) const;

	bool canCreate(ProjectTypes eProject, bool bContinue = false, bool bTestVisible = false) const; // Exposed to Python
	bool canMaintain(ProcessTypes eProcess, bool bContinue = false) const; // Exposed to Python

	int getFoodTurnsLeft() const; // Exposed to Python
	bool isProduction() const; // Exposed to Python
	bool isProductionLimited() const;
	bool isProductionUnitCombat(int iIndex) const;
	bool isProductionUnit() const; // Exposed to Python
	bool isProductionBuilding() const; // Exposed to Python
	bool isProductionProject() const; // Exposed to Python
	bool isProductionProcess() const; // Exposed to Python

	bool canContinueProduction(const OrderData& order) const; // Exposed to Python
	int getProductionExperience(UnitTypes eUnit = NO_UNIT) const; // Exposed to Python
	void addProductionExperience(CvUnit* pUnit, bool bConscript = false); // Exposed to Python

	UnitTypes getProductionUnit() const; // Exposed to Python
	UnitAITypes getProductionUnitAI() const; // Exposed to Python
	BuildingTypes getProductionBuilding() const; // Exposed to Python
	ProjectTypes getProductionProject() const; // Exposed to Python
	ProcessTypes getProductionProcess() const; // Exposed to Python
	const wchar_t* getProductionName() const; // Exposed to Python
	const wchar_t* getProductionNameKey() const; // Exposed to Python
	int getGeneralProductionTurnsLeft() const; // Exposed to Python

	bool isFoodProduction() const; // Exposed to Python
	bool isFoodProduction(const OrderData& order) const;
	bool isFoodProduction(UnitTypes eUnit) const; // Exposed to Python

	int getFirstUnitOrder(UnitTypes eUnit) const; // Exposed to Python
	int getFirstBuildingOrder(BuildingTypes eBuilding) const; // Exposed to Python
	int getFirstProjectOrder(ProjectTypes eProject) const; // Exposed to Python
	int getNumTrainUnitAI(UnitAITypes eUnitAI) const; // Exposed to Python

	int getProduction() const; // Exposed to Python
	int getProductionNeeded() const; // Exposed to Python
	int getProductionNeeded(const OrderData& order) const;
	int getProductionNeeded(UnitTypes eUnit) const;
	int getProductionNeeded(BuildingTypes eBuilding) const;
	int getProductionNeeded(ProjectTypes eProject) const;
	int getOrderProductionTurnsLeft(const OrderData& order, int iIndex = 0) const;

	// For fractional production calculations:
	int getTotalProductionQueueTurnsLeft() const;
	int getProductionTurnsLeft() const; // Exposed to Python
	int getProductionTurnsLeft(UnitTypes eUnit, int iNum) const; // Exposed to Python
	int getProductionTurnsLeft(BuildingTypes eBuilding, int iNum) const; // Exposed to Python
	int getProductionTurnsLeft(ProjectTypes eProject, int iNum) const; // Exposed to Python
	int getProductionTurnsLeft(int iProductionNeeded, int iProduction, int iFirstProductionDifference, int iProductionDifference) const;

	void setProduction(int iNewValue); // Exposed to Python
	void changeProduction(int iChange); // Exposed to Python
	int numQueuedUnits(UnitAITypes contractedAIType, const CvPlot* contractedPlot) const;

	int getProductionModifier(const OrderData& order) const;
	int getProductionModifier() const; // Exposed to Python

	int getProductionModifier(UnitTypes eUnit) const; // Exposed to Python
	int getProductionModifier(BuildingTypes eBuilding) const; // Exposed to Python
	int getProductionModifier(ProjectTypes eProject) const; // Exposed to Python

//	int getOverflowProductionDifference(int iProductionNeeded, int iProduction, int iProductionModifier, int iDiff, int iModifiedProduction) const;
//	int getProductionDifference(int iProductionNeeded, int iProduction, int iProductionModifier, bool bFoodProduction, bool bOverflow) const;

	int getOverflowProductionDifference() const;
	// int getProductionDifference(int iProductionNeeded, int iProduction, int iProductionModifier, bool bFoodProduction, bool bOverflow, bool bYield = true) const;
	int getProductionPerTurn(int iProductionModifier, ProductionCalc::flags flags) const;

	int getProductionDifference(const OrderData& orderData, ProductionCalc::flags flags) const;
	int getCurrentProductionDifference(ProductionCalc::flags flags) const; // Exposed to Python
	int getExtraProductionDifference(int iExtra) const; // Exposed to Python

	bool canHurry(HurryTypes eHurry, bool bTestVisible = false) const; // Exposed to Python
	void hurry(HurryTypes eHurry); // Exposed to Python
// BUG - Hurry Assist - start
	bool hurryOverflow(HurryTypes eHurry, int* iProduction, int* iGold, bool bCountThisTurn = false) const;
// BUG - Hurry Assist - end

	UnitTypes getConscriptUnit() const; // Exposed to Python
	CvUnit* initConscriptedUnit();
	int getConscriptPopulation() const; // Exposed to Python
	int conscriptMinCityPopulation() const; // Exposed to Python
	int flatConscriptAngerLength() const;
//Team Project (6)																			// Exposed to Python
	bool canConscript(bool bOnCapture = false) const; // Exposed to Python
	void conscript(bool bOnCapture = false); // Exposed to Python
/************************************************************************************************/
/* INFLUENCE_DRIVEN_WAR                   04/16/09                                johnysmith    */
/*                                                                                              */
/* Original Author Moctezuma              Start                                                 */
/************************************************************************************************/
	// ------ BEGIN InfluenceDrivenWar -------------------------------
	void emergencyConscript();
	// ------ END InfluenceDrivenWar ---------------------------------
/************************************************************************************************/
/* INFLUENCE_DRIVEN_WAR                   04/16/09                                johnysmith    */
/*                                                                                              */
/* Original Author Moctezuma              End                                                   */
/************************************************************************************************/

	int getBonusHealth(BonusTypes eBonus) const; // Exposed to Python - getBonusHealth
	int getBonusHappiness(BonusTypes eBonus) const; // Exposed to Python - getBonusHappiness
	int getBonusPower(BonusTypes eBonus, bool bDirty) const; // Exposed to Python
	int getBonusYieldRateModifier(YieldTypes eIndex, BonusTypes eBonus) const; // Exposed to Python
	void processBonus(BonusTypes eBonus, int iChange);

//Team Project (5)
	void processBuilding(BuildingTypes eBuilding, int iChange, bool bObsolete = false, bool bReplacingNow = false, bool bReligiouslyDisabling = false);
	void processProcess(ProcessTypes eProcess, int iChange);
	void processSpecialist(SpecialistTypes eSpecialist, int iChange);

	HandicapTypes getHandicapType() const; // Exposed to Python
	CivilizationTypes getCivilizationType() const; // Exposed to Python
	LeaderHeadTypes getPersonalityType() const; // Exposed to Python
	DllExport ArtStyleTypes getArtStyleType() const; // Exposed to Python
	CitySizeTypes getCitySizeType() const; // Exposed to Python
	DllExport const CvArtInfoBuilding* getBuildingArtInfo(BuildingTypes eBuilding) const;
	DllExport float getBuildingVisibilityPriority(BuildingTypes eBuilding) const;

	bool hasTrait(TraitTypes eTrait) const; // Exposed to Python
	bool isBarbarian() const; // Exposed to Python
	bool isNPC() const; // Exposed to Python
	bool isHominid() const; // Exposed to Python
	bool isHuman() const; // Exposed to Python
	DllExport bool isVisible(TeamTypes eTeam, bool bDebug) const; // Exposed to Python

	bool isCapital() const; // Exposed to Python
	bool isCoastal(int iMinWaterSize) const; // Exposed to Python
	bool isDisorder() const; // Exposed to Python
	bool isHolyCity(ReligionTypes eIndex) const; // Exposed to Python
	bool isHolyCity() const; // Exposed to Python
	bool isHeadquarters(CorporationTypes eIndex) const; // Exposed to Python
	bool isHeadquarters() const; // Exposed to Python
	void setHeadquarters(CorporationTypes eIndex);

	int getOvercrowdingPercentAnger(int iExtra = 0) const; // Exposed to Python
	int getNoMilitaryPercentAnger() const; // Exposed to Python
	int getCulturePercentAnger() const; // Exposed to Python
	int getReligionPercentAnger() const; // Exposed to Python
	int getHurryPercentAnger(int iExtra = 0) const; // Exposed to Python
	int getConscriptPercentAnger(int iExtra = 0) const; // Exposed to Python
	int getDefyResolutionPercentAnger(int iExtra = 0) const;
	int getWarWearinessPercentAnger() const; // Exposed to Python

	int getRevRequestPercentAnger(int iExtra = 0) const; // Exposed to Python
	int getRevIndexPercentAnger(int iExtra = 0) const; // Exposed to Python
	int getRevSuccessHappiness() const; // Exposed to Python

	int getLargestCityHappiness() const; // Exposed to Python
	int getVassalHappiness() const; // Exposed to Python
	int getVassalUnhappiness() const; // Exposed to Python

	int getCelebrityHappiness() const; // Exposed to Python

	int unhappyLevel(int iExtra = 0) const; // Exposed to Python
	int happyLevel() const; // Exposed to Python
	int angryPopulation(int iExtra = 0) const; // Exposed to Python

	int visiblePopulation() const;
	int totalFreeSpecialists() const; // Exposed to Python
	int extraPopulation() const; // Exposed to Python
	int extraSpecialists() const; // Exposed to Python
	int extraFreeSpecialists() const; // Exposed to Python

	int unhealthyPopulation(bool bNoAngry = false, int iExtra = 0) const; // Exposed to Python
	int totalGoodBuildingHealth() const; // Exposed to Python
	int totalBadBuildingHealth() const; // Exposed to Python
	int goodHealth() const; // Exposed to Python
	int badHealth(bool bNoAngry = false, int iExtra = 0) const; // Exposed to Python
	int healthRate(bool bNoAngry = false, int iExtra = 0) const; // Exposed to Python
	int getPopulationPlusProgress100(const int iExtra) const;
	int getFoodConsumedPerPopulation100(const int iExtra = 0) const;
	int getFoodConsumedByPopulation(const int iExtra = 0) const;
	int foodConsumption(const bool bNoAngry=false, const int iExtra=0, const bool bIncludeWastage=true) const; // Exposed to Python
	int foodDifference(const bool bBottom=true, const bool bIncludeWastage=true, const bool bIgnoreFoodBuildOrRev=false) const; // Exposed to Python
	float foodWastage(int surplass = -1) const;
	int growthThreshold(const int iPopChange = 0) const; // Exposed to Python

	int productionLeft() const; // Exposed to Python
	int hurryCost(bool bExtra) const; // Exposed to Python
	int getHurryCostModifier(bool bIgnoreNew = false) const;
	int hurryGold(HurryTypes eHurry) const; // Exposed to Python
	int hurryPopulation(HurryTypes eHurry) const; // Exposed to Python
	int hurryProduction(HurryTypes eHurry) const; // Exposed to Python
	int flatHurryAngerLength() const; // Exposed to Python
	int hurryAngerLength(HurryTypes eHurry) const; // Exposed to Python
	int maxHurryPopulation() const; // Exposed to Python

//	int cultureDistance(int iDX, int iDY) const; // Exposed to Python
	int cultureStrength(PlayerTypes ePlayer, int &iOriginal) const; // Exposed to Python
	int cultureGarrison(PlayerTypes ePlayer) const; // Exposed to Python

	//	Note arrival or leaving of a unit
	void noteUnitMoved(const CvUnit* pUnit) const;
	int getGlobalSourcedProperty(PropertyTypes eProperty) const;
	int getTotalBuildingSourcedProperty(PropertyTypes eProperty) const;
	int getTotalUnitSourcedProperty(PropertyTypes eProperty) const;
	int getNumBuilding(BuildingTypes eIndex) const; // Exposed to Python
	int getNumActiveBuilding(BuildingTypes eIndex) const; // Exposed to Python
	bool hasActiveWorldWonder() const; // Exposed to Python

	int getNumActiveWorldWonders() const;


	bool processGreatWall(bool bIn, bool bForce = false, bool bSeeded = true);

	int getReligionCount() const; // Exposed to Python
	int getCorporationCount() const;
	//TB Combat Mods (Buildings) begin
	int getUnitAidPresent(PropertyTypes eProperty) const;
	int getCityAidTotal(PromotionLineTypes ePromotionLineType) const;
	int getAidRate(PropertyTypes ePropertyType) const;
	void changeAidRate(PropertyTypes ePropertyType, int iChange);
	void setAidRate(PropertyTypes ePropertyType, int iChange);

	bool hasAfflictionType(PromotionLineTypes ePromotionLineType) const;
	void changeAfflictionTypeCount(PromotionLineTypes ePromotionLineType, int iChange); // Exposed to Python
	void setAfflictionTypeCount(PromotionLineTypes ePromotionLineType, int iChange);

	int getExtraBonusAidModifier(BonusTypes eBonusType, PropertyTypes ePropertyType) const;
	void changeExtraBonusAidModifier(BonusTypes eBonusType, PropertyTypes ePropertyType, int iChange); // Exposed to Python
	void setExtraBonusAidModifier(BonusTypes eBonusType, PropertyTypes ePropertyType, int iChange);

	int getExtraAfflictionOutbreakLevelChange(PromotionLineTypes ePromotionLine) const;
	void changeExtraAfflictionOutbreakLevelChange(PromotionLineTypes ePromotionLine, int iChange); // Exposed to Python
	void setExtraAfflictionOutbreakLevelChange(PromotionLineTypes ePromotionLine, int iChange);

	int getAfflictionToleranceChange(PromotionLineTypes ePromotionLine) const;
	void changeAfflictionToleranceChange(PromotionLineTypes ePromotionLine, int iChange); // Exposed to Python
	void setAfflictionToleranceChange(PromotionLineTypes ePromotionLine, int iChange);

	int getCurrentOvercomeChange(PromotionLineTypes ePromotionLine) const;
	void changeCurrentOvercomeChange(PromotionLineTypes ePromotionLine, int iChange); // Exposed to Python
	void setCurrentOvercomeChange(PromotionLineTypes ePromotionLine, int iChange);

	int getTotalTechOutbreakLevelChange(BuildingTypes eBuilding) const;
	int getPropertyValue(PropertyTypes eProperty);
	int getOutbreakChangesTotal(BuildingTypes eAfflictionBuilding, PromotionLineTypes eAfflictionLine) const;
	int getOvercomeChangesTotal(BuildingTypes eAfflictionBuilding, PromotionLineTypes eAfflictionLine) const;
	int getUnitCommunicability(PromotionLineTypes eAfflictionLine) const;
	int getTradeCommunicabilityTotal(BuildingTypes eAfflictionBuilding, PromotionLineTypes eAfflictionLine) const;
	bool canAcquireAffliction(BuildingTypes eDisease, PromotionLineTypes eAfflictionLine) const;
	int getTotalCommunicableExposure(PromotionLineTypes eAfflictionLine) const;
	void doOutbreakCheck(PromotionLineTypes eAfflictionLine);
	void doOvercomeCheck(PromotionLineTypes eAfflictionLine);
	int getUnitCombatProductionModifier(UnitCombatTypes eIndex) const;
	void changeUnitCombatProductionModifier(UnitCombatTypes eIndex, int iChange);
	int getUnitCombatRepelModifierTotal(UnitCombatTypes eIndex) const;
	void changeUnitCombatRepelModifierTotal(UnitCombatTypes eIndex, int iChange);
	int getUnitCombatRepelAgainstModifierTotal(UnitCombatTypes eIndex) const;
	void changeUnitCombatRepelAgainstModifierTotal(UnitCombatTypes eIndex, int iChange);
	int getUnitCombatDefenseAgainstModifierTotal(UnitCombatTypes eIndex) const;
	void changeUnitCombatDefenseAgainstModifierTotal(UnitCombatTypes eIndex, int iChange);

	int getPromotionLineAfflictionAttackCommunicability(PromotionLineTypes eAffliction) const;
	void changePromotionLineAfflictionAttackCommunicability(PromotionLineTypes eAffliction, int iChange);
	void setPromotionLineAfflictionAttackCommunicability(PromotionLineTypes eAffliction, int iValue);

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
	int getModifiedBaseYieldRate(YieldTypes eIndex) const;
	//TB Traits end
	bool isQuarantined() const;
	int getQuarantinedCount() const;
	void changeQuarantinedCount(int iChange);
	void resetQuarantinedCount();

	void flushBestSeeInvisibleUnitCache();
	void setBestSeeInvisibleUnitCache(InvisibleTypes eInvisible, UnitTypes eUnitType, bool bSea = false);
	UnitTypes getCachedBestSeeInvisibleUnit(InvisibleTypes eInvisible, bool bSea = false) const;
	bool isCachedBestSeeInvisibleUnit(InvisibleTypes eInvisible, bool bSea = false) const;

	DllExport int getID() const; // Exposed to Python
	int getIndex() const;
	DllExport IDInfo getIDInfo() const;
	void setID(int iID);

	DllExport int getViewportX() const; // Exposed to Python
	inline int getX() const { return m_iX; } // Exposed to Python
	DllExport int getViewportY() const; // Exposed to Python
	inline int getY() const { return m_iY; } // Exposed to Python
	bool isInViewport() const;
	bool at(int iX, int iY) const; // Exposed to Python
	bool at(const CvPlot* pPlot) const; // Exposed to Python - atPlot
	CvPlot* plot() const; // Exposed to Python
	DllExport CvPlot* plotExternal() const; // Exposed to Python
	CvPlotGroup* plotGroup(const PlayerTypes ePlayer) const;
	bool isConnectedTo(const CvCity* pCity) const; // Exposed to Python
	bool isConnectedToCapital(const PlayerTypes ePlayer = NO_PLAYER) const; // Exposed to Python
	int getArea() const;
	CvArea* area() const; // Exposed to Python
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      01/02/09                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	CvArea* waterArea(const bool bNoImpassable = false) const; // Exposed to Python
	CvArea* secondWaterArea() const;
	CvArea* sharedWaterArea(const CvCity* pCity) const;
	bool isBlockaded() const;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	CvPlot* getRallyPlot() const; // Exposed to Python
	void setRallyPlot(const CvPlot* pPlot);

	int getGameTurnFounded() const; // Exposed to Python
	void setGameTurnFounded(int iNewValue);

	int getGameTurnAcquired() const; // Exposed to Python
	void setGameTurnAcquired(int iNewValue);

	int getPopulation() const; // Exposed to Python
	void setPopulation(int iNewValue); // Exposed to Python
	void changePopulation(int iChange); // Exposed to Python

	int getRealPopulation() const; // Exposed to Python

	int getHighestPopulation() const; // Exposed to Python
	void setHighestPopulation(int iNewValue);

	int getWorkingPopulation() const; // Exposed to Python
	void changeWorkingPopulation(int iChange);

	int getSpecialistPopulation() const; // Exposed to Python
	void changeSpecialistPopulation(int iChange);

	int getNumGreatPeople() const; // Exposed to Python
	void changeNumGreatPeople(int iChange);

	int getBaseGreatPeopleRate() const; // Exposed to Python
	int getGreatPeopleRate() const; // Exposed to Python
	int getTotalGreatPeopleRateModifier() const; // Exposed to Python
	void changeBaseGreatPeopleRate(int iChange); // Exposed to Python

	int getGreatPeopleRateModifier() const; // Exposed to Python
	void changeGreatPeopleRateModifier(int iChange);

	CvProperties* getProperties();
	const CvProperties* getPropertiesConst() const;

// BUG - Building Additional Great People - start
	int getAdditionalGreatPeopleRateByBuilding(BuildingTypes eBuilding); // Exposed to Python
	int getAdditionalBaseGreatPeopleRateByBuilding(BuildingTypes eBuilding); // Exposed to Python
	int getAdditionalGreatPeopleRateModifierByBuilding(BuildingTypes eBuilding); // Exposed to Python
// BUG - Building Additional Great People - end

// BUG - Specialist Additional Great People - start
	int getAdditionalGreatPeopleRateBySpecialist(SpecialistTypes eSpecialist, int iChange) const; // Exposed to Python
	int getAdditionalBaseGreatPeopleRateBySpecialist(SpecialistTypes eSpecialist, int iChange) const; // Exposed to Python
// BUG - Specialist Additional Great People - end

	int getGreatPeopleProgress() const; // Exposed to Python
	void changeGreatPeopleProgress(int iChange); // Exposed to Python

	int getNumWorldWonders() const; // Exposed to Python
	void changeNumWorldWonders(int iChange);

	int getNumTeamWonders() const; // Exposed to Python
	void changeNumTeamWonders(int iChange);

	int getNumNationalWonders() const; // Exposed to Python
	void changeNumNationalWonders(int iChange);

	int getNumBuildings() const; // Exposed to Python
	void changeNumBuildings(int iChange);

	int getGovernmentCenterCount() const;
	bool isGovernmentCenter() const; // Exposed to Python
	void changeGovernmentCenterCount(int iChange);

// BUG - Building Saved Maintenance - start
	int getSavedMaintenanceByBuilding(BuildingTypes eBuilding) const; // Exposed to Python
	int getSavedMaintenanceTimes100ByBuilding(BuildingTypes eBuilding) const; // Exposed to Python
// BUG - Building Saved Maintenance - end

	int getMaintenance() const; // Exposed to Python
	int getMaintenanceTimes100() const; // Exposed to Python
	int getEffectiveMaintenanceModifier() const;
	void updateMaintenance() const;
	void setMaintenanceDirty(bool bDirty) const;
	int calculateDistanceMaintenance() const; // Exposed to Python
	int calculateNumCitiesMaintenance() const; // Exposed to Python
	int calculateColonyMaintenance() const; // Exposed to Python
	int calculateCorporationMaintenance() const; // Exposed to Python
/************************************************************************************************/
/* Afforess	                  Start		 09/08/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int calculateDistanceMaintenanceTimes100(int iExtraDistanceModifier = 0, int iExtraCoastalDistanceModifier = 0) const; // Exposed to Python
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	//Afforess: added iExtraModifier
	int calculateNumCitiesMaintenanceTimes100(int iExtraModifier = 0) const; // Exposed to Python
	int calculateColonyMaintenanceTimes100() const; // Exposed to Python
	int calculateCorporationMaintenanceTimes100(CorporationTypes eCorporation) const; // Exposed to Python
	int calculateCorporationMaintenanceTimes100() const; // Exposed to Python
	int calculateBaseMaintenanceTimes100() const;
	int calculateBuildingMaintenanceTimes100() const;
	int getMaintenanceModifier() const; // Exposed to Python
	void changeMaintenanceModifier(int iChange);

	int getWarWearinessModifier() const; // Exposed to Python
	void changeWarWearinessModifier(int iChange);

	int getHurryAngerModifier() const; // Exposed to Python
	void changeHurryAngerModifier(int iChange);

	int getHealRate() const; // Exposed to Python
	void changeHealRate(int iChange);

	int getHealUnitCombatTypeTotal(UnitCombatTypes eUnitCombat) const;
	void changeHealUnitCombatTypeVolume(UnitCombatTypes eUnitCombat, int iChange);
	void setHealUnitCombatTypeVolume(UnitCombatTypes eUnitCombat, int iChange);

	int getEspionageHealthCounter() const; // Exposed to Python
	void changeEspionageHealthCounter(int iChange); // Exposed to Python

	int getEspionageHappinessCounter() const; // Exposed to Python
	void changeEspionageHappinessCounter(int iChange); // Exposed to Python

	int getFreshWaterGoodHealth() const; // Exposed to Python
	void updateFreshWaterHealth();

	int getFeatureGoodHealth() const; // Exposed to Python
	int getFeatureBadHealth() const; // Exposed to Python
	void updateFeatureHealth();
// BUG - Feature Health - start
	void calculateFeatureHealthPercent(int& iGood, int& iBad) const;
	void calculateFeatureHealthPercentChange(int& iGood, int& iBad, CvPlot* pIgnorePlot = NULL) const;
	int getAdditionalHealthByFeature(FeatureTypes eFeature, int iChange) const; // Exposed to Python
	int getAdditionalHealthByFeature(FeatureTypes eFeature, int iChange, int& iGood, int& iBad) const;
	int getAdditionalHealth(int iGoodPercent, int iBadPercent, int& iGood, int& iBad) const;
// BUG - Feature Health - end

// BUG - Actual Effects - start
	int getAdditionalAngryPopuplation(int iGood, int iBad) const;
	int getAdditionalSpoiledFood(int iGood, int iBad, int iHealthAdjust = 0) const;
	int getAdditionalStarvation(int iSpoiledFood, int iFoodAdjust = 0) const;
// BUG - Actual Effects - end

	int getBuildingGoodHealth() const; // Exposed to Python
	int getBuildingBadHealth() const; // Exposed to Python
	int getBuildingHealth(BuildingTypes eBuilding) const; // Exposed to Python
	int getBuildingGoodHealth(BuildingTypes eBuilding) const;
	int getBuildingBadHealth(BuildingTypes eBuilding) const;
	void changeBuildingGoodHealth(int iChange);
	void changeBuildingBadHealth(int iChange);

	int getPowerGoodHealth() const; // Exposed to Python
	int getPowerBadHealth() const; // Exposed to Python
	void updatePowerHealth();

	int getBonusGoodHealth() const; // Exposed to Python
	int getBonusBadHealth() const; // Exposed to Python
	void changeBonusGoodHealth(int iChange);
	void changeBonusBadHealth(int iChange);

	int getMilitaryHappiness() const; // Exposed to Python
	int getMilitaryHappinessUnits() const; // Exposed to Python
	void changeMilitaryHappinessUnits(int iChange);

	int getBuildingGoodHappiness() const; // Exposed to Python
	int getBuildingBadHappiness() const; // Exposed to Python
	int getBuildingHappiness(BuildingTypes eBuilding) const; // Exposed to Python
	void changeBuildingGoodHappiness(int iChange);
	void changeBuildingBadHappiness(int iChange);

	int getExtraBuildingGoodHappiness() const; // Exposed to Python
	int getExtraBuildingBadHappiness() const; // Exposed to Python
/********************************************************************************/
/* 	New Civic AI						19.08.2010				Fuyu			*/
/********************************************************************************/
//Fuyu bLimited
	void updateExtraBuildingHappiness(bool bLimited = false);

	int getAdditionalHappinessByCivic(CivicTypes eCivic, bool bDifferenceToCurrent = true, bool bCivicOptionVacuum = false, ReligionTypes eStateReligion = NO_RELIGION, int iExtraPop = 0, int iMilitaryHappinessUnits = -1) const;
	int getAdditionalHealthByCivic(CivicTypes eCivic, bool bDifferenceToCurrent = true) const;
	int getAdditionalHealthByCivic(CivicTypes eCivic, int& iGood, int& iBad, int& iBadBuilding, bool bDifferenceToCurrent = true, int iExtraPop = 0, bool bCivicOptionVacuum = false, int iIgnoreNoUnhealthyPopulationCount = 0, int iIgnoreBuildingOnlyHealthyCount = 0) const;
	int getAdditionalHealthByPlayerNoUnhealthyPopulation(int iExtraPop = 0, int iIgnoreNoUnhealthyPopulationCount = 0) const;
	int getAdditionalHealthByPlayerBuildingOnlyHealthy(int iIgnoreBuildingOnlyHealthyCount = 0) const;

// BUG - Building Additional Happiness - start
	int getAdditionalHappinessByBuilding(BuildingTypes eBuilding); // Exposed to Python
	int getAdditionalHappinessByBuilding(BuildingTypes eBuilding, int& iGood, int& iBad, int& iAngryPop);
// BUG - Building Additional Happiness - end

	int getExtraBuildingGoodHealth() const; // Exposed to Python
	int getExtraBuildingBadHealth() const; // Exposed to Python
//Fuyu bLimited
	void updateExtraBuildingHealth(bool bLimited = false);

// BUG - Building Additional Health - start
	int getAdditionalHealthByBuilding(BuildingTypes eBuilding); // Exposed to Python
	int getAdditionalHealthByBuilding(BuildingTypes eBuilding, int& iGood, int& iBad, int& iSpoiledFood, int& iStarvation);
// BUG - Building Additional Health - end

	int getFeatureGoodHappiness() const; // Exposed to Python
	int getFeatureBadHappiness() const; // Exposed to Python
//Fuyu bLimited
	void updateFeatureHappiness(bool bLimited = false);

	int getBonusGoodHappiness() const; // Exposed to Python
	int getBonusBadHappiness() const; // Exposed to Python
	void changeBonusGoodHappiness(int iChange);
	void changeBonusBadHappiness(int iChange);

	int getReligionGoodHappiness() const; // Exposed to Python
	int getReligionBadHappiness() const; // Exposed to Python
	int getReligionHappiness(ReligionTypes eReligion) const; // Exposed to Python
//Fuyu bLimited
	void updateReligionHappiness(bool bLimited = false);
/********************************************************************************/
/* 	New Civic AI												END 			*/
/********************************************************************************/

	int getExtraHappiness() const; // Exposed to Python
	void changeExtraHappiness(int iChange); // Exposed to Python

	int getExtraHealth() const; // Exposed to Python
	void changeExtraHealth(int iChange); // Exposed to Python

	int getHurryAngerTimer() const; // Exposed to Python
	void changeHurryAngerTimer(int iChange); // Exposed to Python

/************************************************************************************************/
/* REVOLUTION_MOD                         04/28/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int getRevRequestAngerTimer() const; // Exposed to Python
	void changeRevRequestAngerTimer(int iChange); // Exposed to Python

	int getRevSuccessTimer() const; // Exposed to Python
	void changeRevSuccessTimer(int iChange); // Exposed to Python
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
	int getConscriptAngerTimer() const; // Exposed to Python
	void changeConscriptAngerTimer(int iChange); // Exposed to Python

	int getDefyResolutionAngerTimer() const; // Exposed to Python
	void changeDefyResolutionAngerTimer(int iChange); // Exposed to Python
	int flatDefyResolutionAngerLength() const; // Exposed to Python

	int getHappinessTimer() const; // Exposed to Python
	void changeHappinessTimer(int iChange); // Exposed to Python

	int getNoUnhappinessCount() const;
	bool isNoUnhappiness() const; // Exposed to Python
	void changeNoUnhappinessCount(int iChange);

	int getNoUnhealthyPopulationCount() const;
	bool isNoUnhealthyPopulation() const; // Exposed to Python
	void changeNoUnhealthyPopulationCount(int iChange);

	int getBuildingOnlyHealthyCount() const;
	bool isBuildingOnlyHealthy() const; // Exposed to Python
	void changeBuildingOnlyHealthyCount(int iChange);

	int getFood() const; // Exposed to Python
	void setFood(int iNewValue); // Exposed to Python
	void changeFood(int iChange); // Exposed to Python

	int getFoodKept() const; // Exposed to Python
	void setFoodKept(int iNewValue);
	void changeFoodKept(int iChange);

	int getMaxFoodKeptPercent() const; // Exposed to Python
	void changeMaxFoodKeptPercent(int iChange, bool bAdd);

	int getOverflowProduction() const; // Exposed to Python
	void setOverflowProduction(int iNewValue); // Exposed to Python
	void changeOverflowProduction(int iChange, int iProductionModifier);

	int getFeatureProduction() const; // Exposed to Python
	void setFeatureProduction(int iNewValue); // Exposed to Python
	void changeFeatureProduction(int iChange);

	int getMilitaryProductionModifier() const; // Exposed to Python
	void changeMilitaryProductionModifier(int iChange);

	int getSpaceProductionModifier() const; // Exposed to Python
	void changeSpaceProductionModifier(int iChange);

	int getExtraTradeRoutes() const; // Exposed to Python
	void changeExtraTradeRoutes(int iChange); // Exposed to Python

	int getTradeRouteModifier() const; // Exposed to Python
	void changeTradeRouteModifier(int iChange);

	int getForeignTradeRouteModifier() const; // Exposed to Python
	void changeForeignTradeRouteModifier(int iChange);

	int getBuildingDefense() const; // Exposed to Python
	void changeBuildingDefense(int iChange);

	int getBuildingBombardDefense() const; // Exposed to Python
	void changeBuildingBombardDefense(int iChange);
// BUG - Building Additional Bombard Defense - start
	int getAdditionalBombardDefenseByBuilding(BuildingTypes eBuilding) const; // Exposed to Python
// BUG - Building Additional Bombard Defense - start

	int getFreeExperience() const; // Exposed to Python
	void changeFreeExperience(int iChange);

	int getCurrAirlift() const; // Exposed to Python
	void setCurrAirlift(int iNewValue);
	void changeCurrAirlift(int iChange);

	int getMaxAirlift() const; // Exposed to Python
	void changeMaxAirlift(int iChange);

	int getAirModifier() const; // Exposed to Python
	void changeAirModifier(int iChange);

	int getSMAirUnitCapacity(TeamTypes eTeam) const;
	int getAirUnitCapacity(TeamTypes eTeam) const; // Exposed to Python
	void changeAirUnitCapacity(int iChange); // Exposed to Python

	int getNukeModifier() const; // Exposed to Python
	void changeNukeModifier(int iChange);

	int getFreeSpecialist() const; // Exposed to Python
	void changeFreeSpecialist(int iChange);

	int getPowerCount() const;
	bool isPower() const; // Exposed to Python
	bool isAreaCleanPower() const; // Exposed to Python
	int getDirtyPowerCount() const;
	bool isDirtyPower() const; // Exposed to Python
	void changePowerCount(int iChange, bool bDirty);

	int getDefenseDamage() const; // Exposed to Python
	void changeDefenseDamage(int iChange); // Exposed to Python
	void changeDefenseModifier(int iChange); // Exposed to Python

	int getLastDefenseDamage() const; // Exposed to Python
	void setLastDefenseDamage(int iNewValue);

	bool isBombardable(const CvUnit* pUnit) const; // Exposed to Python
	int getNaturalDefense() const; // Exposed to Python
	int getTotalDefense(bool bIgnoreBuilding) const; // Exposed to Python
	int getDefenseModifier(bool bIgnoreBuilding) const; // Exposed to Python

	int getOccupationTimer() const; // Exposed to Python
	bool isOccupation() const; // Exposed to Python
	void setOccupationTimer(int iNewValue); // Exposed to Python
	void changeOccupationTimer(int iChange); // Exposed to Python

	int getCultureUpdateTimer() const; // Exposed to Python
	void setCultureUpdateTimer(int iNewValue);
	void changeCultureUpdateTimer(int iChange); // Exposed to Python

	int getCitySizeBoost() const;
	void setCitySizeBoost(int iBoost);

	// < M.A.D. Nukes Start >
	int getMADIncoming() const; // Exposed to Python
	void setMADIncoming(int iValue); // Exposed to Python
	void changeMADIncoming(int iValue); // Exposed to Python
	// < M.A.D. Nukes End   >

	bool isNeverLost() const; // Exposed to Python
	void setNeverLost(bool bNewValue); // Exposed to Python

	bool isBombarded() const; // Exposed to Python
	void setBombarded(bool bNewValue); // Exposed to Python

	bool isDrafted() const; // Exposed to Python
	void setDrafted(bool bNewValue); // Exposed to Python

	bool isAirliftTargeted() const; // Exposed to Python
	void setAirliftTargeted(bool bNewValue); // Exposed to Python

	bool isWeLoveTheKingDay() const; // Exposed to Python
	void setWeLoveTheKingDay(bool bNewValue);

	bool isCitizensAutomated() const; // Exposed to Python
	void setCitizensAutomated(bool bNewValue); // Exposed to Python

	bool isProductionAutomated() const; // Exposed to Python
	void setProductionAutomated(bool bNewValue, bool bClear); // Exposed to Python

	/* allows you to programatically specify a cities walls rather than having them be generated automagically */
	DllExport bool isWallOverride() const;
	void setWallOverride(bool bOverride);

	DllExport bool isInfoDirty() const;
	DllExport void setInfoDirty(bool bNewValue);

	DllExport bool isLayoutDirty() const;
	DllExport void setLayoutDirty(bool bNewValue);

	bool isPlundered() const; // Exposed to Python
	void setPlundered(bool bNewValue); // Exposed to Python

	DllExport inline PlayerTypes getOwner() const { return m_eOwner; } // Exposed to Python
	DllExport TeamTypes getTeam() const; // Exposed to Python

	PlayerTypes getPreviousOwner() const; // Exposed to Python
	void setPreviousOwner(PlayerTypes eNewValue);

	PlayerTypes getOriginalOwner() const; // Exposed to Python
	void setOriginalOwner(PlayerTypes eNewValue);

	CultureLevelTypes getCultureLevel() const; // Exposed to Python
	int getCultureThreshold() const; // Exposed to Python
	int getCultureThreshold(CultureLevelTypes eLevel) const;
	void setCultureLevel(CultureLevelTypes eNewValue, bool bUpdatePlotGroups);
	void updateCultureLevel(bool bUpdatePlotGroups);

	int getSeaPlotYield(YieldTypes eIndex) const; // Exposed to Python
	void changeSeaPlotYield(YieldTypes eIndex, int iChange);

	int getRiverPlotYield(YieldTypes eIndex) const; // Exposed to Python
	void changeRiverPlotYield(YieldTypes eIndex, int iChange);

// BUG - Building Additional Yield - start
	int getAdditionalYieldByBuilding(YieldTypes eIndex, BuildingTypes eBuilding, bool bFilter = false); // Exposed to Python
	int getAdditionalBaseYieldRateByBuilding(YieldTypes eIndex, BuildingTypes eBuilding); // Exposed to Python
	int getAdditionalYieldRateModifierByBuilding(YieldTypes eIndex, BuildingTypes eBuilding, bool bFilter = false) const; // Exposed to Python
// BUG - Building Additional Yield - end

// BUG - Specialist Additional Yield - start
	int getAdditionalYieldBySpecialist(YieldTypes eIndex, SpecialistTypes eSpecialist, int iChange) const; // Exposed to Python
	int getAdditionalBaseYieldRateBySpecialist(YieldTypes eIndex, SpecialistTypes eSpecialist, int iChange) const; // Exposed to Python
// BUG - Specialist Additional Yield - end

	int getBaseYieldRate(YieldTypes eIndex) const; // Exposed to Python
	int getBaseYieldRateModifier(YieldTypes eIndex, int iExtra = 0) const; // Exposed to Python
	int getYieldRate(YieldTypes eIndex) const; // Exposed to Python
	void setBaseYieldRate(YieldTypes eIndex, int iNewValue); // Exposed to Python
	void changeBaseYieldRate(YieldTypes eIndex, int iChange); // Exposed to Python

	int popYield(YieldTypes eIndex) const;
	int getBaseYieldPerPopRate(YieldTypes eIndex) const;
	void setBaseYieldPerPopRate(YieldTypes eIndex, int iNewValue);
	void changeBaseYieldPerPopRate(YieldTypes eIndex, int iChange);

	int getYieldRateModifier(YieldTypes eIndex) const; // Exposed to Python
	void changeYieldRateModifier(YieldTypes eIndex, int iChange);

	int getPowerYieldRateModifier(YieldTypes eIndex) const; // Exposed to Python
	void changePowerYieldRateModifier(YieldTypes eIndex, int iChange);

	int getBonusYieldRateModifier(YieldTypes eIndex) const; // Exposed to Python
	void changeBonusYieldRateModifier(YieldTypes eIndex, int iChange);

	int getTradeYield(YieldTypes eIndex) const; // Exposed to Python
	int totalTradeModifier(const CvCity* pOtherCity = NULL) const; // Exposed to Python
	int getPopulationTradeModifier() const;
	int getPeaceTradeModifier(TeamTypes eTeam) const;
	int getBaseTradeProfit(const CvCity* pCity) const;
// BUG - Fractional Trade Routes - start
#ifdef _MOD_FRACTRADE
	int calculateTradeProfitTimes100(const CvCity* pCity) const; // Exposed to Python
#endif
// BUG - Fractional Trade Routes - end
	int calculateTradeProfit(const CvCity* pCity) const; // Exposed to Python
	int calculateTradeYield(YieldTypes eIndex, int iTradeProfit) const; // Exposed to Python
// BUG - Trade Totals - start
	void calculateTradeTotals(YieldTypes eIndex, int& iDomesticYield, int& iDomesticRoutes, int& iForeignYield, int& iForeignRoutes, PlayerTypes eWithPlayer = NO_PLAYER, bool bRound = false, bool bBase = false) const;
	int calculateTotalTradeYield(YieldTypes eIndex, PlayerTypes eWithPlayer = NO_PLAYER, bool bRound = false, bool bBase = false) const;
// BUG - Trade Totals - end
	void setTradeYield(YieldTypes eIndex, int iNewValue);

	int getExtraSpecialistYield(YieldTypes eIndex) const; // Exposed to Python
	int getExtraSpecialistYield(YieldTypes eIndex, SpecialistTypes eSpecialist) const; // Exposed to Python
	void updateExtraSpecialistYield(YieldTypes eYield);
	void updateExtraSpecialistYield();

	int getExtraSpecialistCommerceTotal(CommerceTypes eIndex) const; // Exposed to Python
	int getExtraSpecialistCommerce(CommerceTypes eIndex, SpecialistTypes eSpecialist) const; // Exposed to Python
	void updateExtraSpecialistCommerce(CommerceTypes eCommerce);
	void updateExtraSpecialistCommerce();

	int getCommerceRate(CommerceTypes eIndex) const; // Exposed to Python
	int getCommerceRateTimes100(CommerceTypes eIndex) const; // Exposed to Python
	int getCommerceFromPercent(CommerceTypes eIndex, int iYieldRate) const; // Exposed to Python
	int getBaseCommerceRate(CommerceTypes eIndex) const; // Exposed to Python
	int getBaseCommerceRateTimes100(CommerceTypes eIndex) const; // Exposed to Python
	int getTotalCommerceRateModifier(CommerceTypes eIndex) const; // Exposed to Python
	void setCommerceModifierDirty(CommerceTypes eCommerce);
	void setCommerceDirty(CommerceTypes eCommerce = NO_COMMERCE);
	void updateCommerce(CommerceTypes eIndex = NO_COMMERCE, bool bForce = true) const;

	int getProductionToCommerceModifier(CommerceTypes eIndex) const; // Exposed to Python
	void changeProductionToCommerceModifier(CommerceTypes eIndex, int iChange);

	int getBuildingCommerce(CommerceTypes eIndex) const; // Exposed to Python
	int getBuildingCommerceByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding) const; // Exposed to Python
	int getOrbitalBuildingCommerceByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding) const;
// BUG - Building Additional Commerce - start
	int getAdditionalCommerceByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding); // Exposed to Python
	int getAdditionalCommerceTimes100ByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding); // Exposed to Python
	int getAdditionalBaseCommerceRateByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding); // Exposed to Python
	int getAdditionalBaseCommerceRateByBuildingTimes100(CommerceTypes eIndex, BuildingTypes eBuilding);
	int getAdditionalCommerceRateModifierByBuilding(CommerceTypes eIndex, BuildingTypes eBuilding); // Exposed to Python
// BUG - Building Additional Commerce - end
	void updateBuildingCommerce();

	int getSpecialistCommerce(CommerceTypes eIndex) const; // Exposed to Python
	void changeSpecialistCommerceTimes100(CommerceTypes eIndex, int iChange);
// BUG - Specialist Additional Commerce - start
	int getAdditionalCommerceBySpecialist(CommerceTypes eIndex, SpecialistTypes eSpecialist, int iChange) const; // Exposed to Python
	int getAdditionalCommerceTimes100BySpecialist(CommerceTypes eIndex, SpecialistTypes eSpecialist, int iChange) const; // Exposed to Python
	int getAdditionalBaseCommerceRateBySpecialist(CommerceTypes eIndex, SpecialistTypes eSpecialist, int iChange) const; // Exposed to Python
// BUG - Specialist Additional Commerce - end


	int getReligionCommerce(CommerceTypes eIndex) const; // Exposed to Python
	int getReligionCommerceByReligion(CommerceTypes eIndex, ReligionTypes eReligion) const; // Exposed to Python
	void updateReligionCommerce(CommerceTypes eIndex);
	void updateReligionCommerce();

	void setCorporationYield(YieldTypes eIndex, int iNewValue);
	int getCorporationCommerce(CommerceTypes eIndex) const; // Exposed to Python
	int getCorporationCommerceByCorporation(CommerceTypes eIndex, CorporationTypes eCorporation) const; // Exposed to Python
	int getCorporationYield(YieldTypes eIndex) const; // Exposed to Python
	int getCorporationYieldByCorporation(YieldTypes eIndex, CorporationTypes eCorporation) const; // Exposed to Python
	void updateCorporation();
	void updateCorporationCommerce(CommerceTypes eIndex);
	void updateCorporationYield(YieldTypes eIndex);
	void updateCorporationBonus();

	int getCommerceRateModifier(CommerceTypes eIndex) const; // Exposed to Python
	void changeCommerceRateModifier(CommerceTypes eIndex, int iChange);

	int getCommerceHappinessPer(CommerceTypes eIndex) const; // Exposed to Python
	int getCommerceHappinessByType(CommerceTypes eIndex) const; // Exposed to Python
	int getCommerceHappiness() const; // Exposed to Python
	void changeCommerceHappinessPer(CommerceTypes eIndex, int iChange);

	int getDomainFreeExperience(DomainTypes eIndex) const; // Exposed to Python
	void changeDomainFreeExperience(DomainTypes eIndex, int iChange);

	int getDomainProductionModifier(DomainTypes eIndex) const; // Exposed to Python
	void changeDomainProductionModifier(DomainTypes eIndex, int iChange);

	int getCulture(PlayerTypes eIndex) const; // Exposed to Python
	int getCultureTimes100(PlayerTypes eIndex) const; // Exposed to Python
	int countTotalCultureTimes100() const; // Exposed to Python
	PlayerTypes findHighestCulture() const; // Exposed to Python
	int calculateCulturePercent(PlayerTypes eIndex) const; // Exposed to Python
	int calculateTeamCulturePercent(TeamTypes eIndex) const; // Exposed to Python
	void setCulture(PlayerTypes eIndex, int iNewValue, bool bPlots, bool bUpdatePlotGroups, bool bNationalSet = false); // Exposed to Python
	void setCultureTimes100(PlayerTypes eIndex, int iNewValue, bool bPlots, bool bUpdatePlotGroups, bool bNationalSet = false); // Exposed to Python
	void changeCulture(PlayerTypes eIndex, int iChange, bool bPlots, bool bUpdatePlotGroups); // Exposed to Python
	void changeCultureTimes100(PlayerTypes eIndex, int iChange, bool bPlots, bool bUpdatePlotGroups); // Exposed to Python

	int getNumRevolts(PlayerTypes eIndex) const;
	void changeNumRevolts(PlayerTypes eIndex, int iChange);

	bool isTradeRoute(PlayerTypes eIndex) const; // Exposed to Python
	void setTradeRoute(PlayerTypes eIndex, bool bNewValue);

	bool isEverOwned(PlayerTypes eIndex) const; // Exposed to Python
	void setEverOwned(PlayerTypes eIndex, bool bNewValue);

	DllExport bool isRevealed(TeamTypes eIndex, bool bDebug) const; // Exposed to Python
	void setRevealed(TeamTypes eIndex, bool bNewValue); // Exposed to Python

	bool getEspionageVisibility(TeamTypes eTeam) const; // Exposed to Python
	void setEspionageVisibility(TeamTypes eTeam, bool bVisible, bool bUpdatePlotGroups);
	void updateEspionageVisibility(bool bUpdatePlotGroups);

	DllExport const CvWString getName(uint uiForm = 0) const; // Exposed to Python
	DllExport const wchar_t* getNameKey() const; // Exposed to Python
	void setName(const wchar_t* szNewValue, bool bFound = false); // Exposed to Python
	void doFoundMessage();

	// Script data needs to be a narrow string for pickling in Python
	std::string getScriptData() const; // Exposed to Python
	void setScriptData(std::string szNewValue); // Exposed to Python

	int getFreeBonus(BonusTypes eIndex) const; // Exposed to Python
	void changeFreeBonus(BonusTypes eIndex, int iChange); // Exposed to Python

	void processNumBonusChange(BonusTypes eIndex, int iOldValue, int iNewValue);
	void endDeferredBonusProcessing();
	void startDeferredBonusProcessing();
	int getNumBonusesFromBase(BonusTypes eIndex, int iBaseNum) const;

	int getNumBonuses(BonusTypes eIndex) const; // Exposed to Python
	bool hasBonus(BonusTypes eIndex) const; // Exposed to Python
	void changeNumBonuses(BonusTypes eIndex, int iChange);

	int getNumCorpProducedBonuses(BonusTypes eIndex) const;
	bool isCorporationBonus(BonusTypes eBonus) const;
	bool isActiveCorporation(CorporationTypes eCorporation) const;

	// How many hammers already put into production of the building
	int getBuildingProduction(BuildingTypes eIndex) const; // Exposed to Python
	void setBuildingProduction(BuildingTypes eIndex, int iNewValue); // Exposed to Python
	void changeBuildingProduction(BuildingTypes eIndex, int iChange); // Exposed to Python

	int getBuildingProductionTime(BuildingTypes eIndex) const; // Exposed to Python
	void setBuildingProductionTime(BuildingTypes eIndex, int iNewValue); // Exposed to Python
	void changeBuildingProductionTime(BuildingTypes eIndex, int iChange); // Exposed to Python
// BUG - Production Decay - start
	bool isBuildingProductionDecay(BuildingTypes eIndex) const; // Exposed to Python
	int getBuildingProductionDecay(BuildingTypes eIndex) const; // Exposed to Python
	int getBuildingProductionDecayTurns(BuildingTypes eIndex) const; // Exposed to Python
// BUG - Production Decay - end

	int getProjectProduction(ProjectTypes eIndex) const; // Exposed to Python
	void setProjectProduction(ProjectTypes eIndex, int iNewValue); // Exposed to Python
	void changeProjectProduction(ProjectTypes eIndex, int iChange); // Exposed to Python

	int getBuildingOriginalOwner(BuildingTypes eIndex) const; // Exposed to Python
	int getBuildingOriginalTime(BuildingTypes eIndex) const; // Exposed to Python
	void setBuildingOriginalTime(BuildingTypes eIndex, int iNewValue); // Exposed to Python

	int getUnitProduction(UnitTypes eIndex) const; // Exposed to Python
	void setUnitProduction(UnitTypes eIndex, int iNewValue); // Exposed to Python
	void changeUnitProduction(UnitTypes eIndex, int iChange); // Exposed to Python

	int getUnitProductionTime(UnitTypes eIndex) const; // Exposed to Python
	void setUnitProductionTime(UnitTypes eIndex, int iNewValue); // Exposed to Python
	void changeUnitProductionTime(UnitTypes eIndex, int iChange); // Exposed to Python
// BUG - Production Decay - start
	bool isUnitProductionDecay(UnitTypes eIndex) const; // Exposed to Python
	int getUnitProductionDecay(UnitTypes eIndex) const; // Exposed to Python
	int getUnitProductionDecayTurns(UnitTypes eIndex) const; // Exposed to Python
// BUG - Production Decay - end

	int getGreatPeopleUnitRate(UnitTypes eIndex) const; // Exposed to Python
	void setGreatPeopleUnitRate(UnitTypes eIndex, int iNewValue);
	void changeGreatPeopleUnitRate(UnitTypes eIndex, int iChange);

	int getGreatPeopleUnitProgress(UnitTypes eIndex) const; // Exposed to Python
	void setGreatPeopleUnitProgress(UnitTypes eIndex, int iNewValue); // Exposed to Python
	void changeGreatPeopleUnitProgress(UnitTypes eIndex, int iChange); // Exposed to Python

	int getSpecialistCount(SpecialistTypes eIndex) const; // Exposed to Python
	void setSpecialistCount(SpecialistTypes eIndex, int iNewValue);
	void changeSpecialistCount(SpecialistTypes eIndex, int iChange);
	void alterSpecialistCount(SpecialistTypes eIndex, int iChange); // Exposed to Python

	int getMaxSpecialistCount(SpecialistTypes eIndex) const; // Exposed to Python
	bool isSpecialistValid(SpecialistTypes eIndex, int iExtra = 0) const; // Exposed to Python
	void changeMaxSpecialistCount(SpecialistTypes eIndex, int iChange);

	int getForceSpecialistCount(SpecialistTypes eIndex) const; // Exposed to Python
	bool isSpecialistForced() const; // Exposed to Python
	void setForceSpecialistCount(SpecialistTypes eIndex, int iNewValue); // Exposed to Python
	void changeForceSpecialistCount(SpecialistTypes eIndex, int iChange); // Exposed to Python

	int getFreeSpecialistCount(SpecialistTypes eIndex) const; // Exposed to Python
	void setFreeSpecialistCount(SpecialistTypes eIndex, int iNewValue); // Exposed to Python
	void changeFreeSpecialistCount(SpecialistTypes eIndex, int iChange, bool bUnattributed = false); // Exposed to Python
	int getAddedFreeSpecialistCount(SpecialistTypes eIndex) const; // Exposed to Python

	int getImprovementFreeSpecialists(ImprovementTypes eIndex) const; // Exposed to Python
	void changeImprovementFreeSpecialists(ImprovementTypes eIndex, int iChange); // Exposed to Python

	uint32_t getReligionInfluence(ReligionTypes eIndex) const; // Exposed to Python
	void changeReligionInfluence(ReligionTypes eIndex, int iChange); // Exposed to Python

	int getCurrentStateReligionHappiness() const; // Exposed to Python
	int getStateReligionHappiness(ReligionTypes eIndex) const; // Exposed to Python
	void changeStateReligionHappiness(ReligionTypes eIndex, int iChange); // Exposed to Python

	int getUnitCombatFreeExperience(UnitCombatTypes eIndex) const; // Exposed to Python
	void changeUnitCombatFreeExperience(UnitCombatTypes eIndex, int iChange);

	int getFreePromotionCount(PromotionTypes eIndex) const; // Exposed to Python
	bool isFreePromotion(PromotionTypes eIndex) const; // Exposed to Python
	void changeFreePromotionCount(PromotionTypes eIndex, int iChange);

	int getSpecialistFreeExperience() const; // Exposed to Python
	void changeSpecialistFreeExperience(int iChange);

	int getEspionageDefenseModifier() const; // Exposed to Python
	void changeEspionageDefenseModifier(int iChange);

	bool isWorkingPlot(int iIndex) const; // Exposed to Python
	bool isWorkingPlot(const CvPlot* pPlot) const; // Exposed to Python
	void setWorkingPlot(int iIndex, bool bNewValue);
	void setWorkingPlot(CvPlot* pPlot, bool bNewValue);
	void alterWorkingPlot(int iIndex); // Exposed to Python
	void processWorkingPlot(int iPlot, int iChange, bool yieldsOnly = false);

	int getNumRealBuilding(BuildingTypes eIndex) const; // Exposed to Python
	void setNumRealBuilding(BuildingTypes eIndex, int iNewValue); // Exposed to Python
	void setNumRealBuildingTimed(BuildingTypes eIndex, int iNewValue, bool bFirst, PlayerTypes eOriginalOwner, int iOriginalTime);

	bool isValidBuildingLocation(BuildingTypes eIndex) const;

	uint16_t getFreeAreaBuildingCount(const short iIndex) const;
	void changeFreeAreaBuildingCount(const BuildingTypes eIndex, const int iChange);
	void setFreeBuilding(const BuildingTypes eIndex, const bool bNewValue);
	bool isFreeBuilding(short iIndex) const;
	void checkFreeBuildings();

	bool isHasReligion(ReligionTypes eIndex) const;
	void setHasReligion(ReligionTypes eIndex, bool bNewValue, bool bAnnounce, bool bArrows = true);
	//Team Project (5)
	void checkReligiousDisablingAllBuildings();
	void checkReligiousDisabling(const BuildingTypes eBuilding, const CvPlayer& player);
	void applyReligionModifiers(const ReligionTypes eIndex, const bool bValue);

	bool isHasCorporation(CorporationTypes eIndex) const;
	void setHasCorporation(CorporationTypes eIndex, bool bNewValue, bool bAnnounce, bool bArrows = true);
	void applyCorporationModifiers(CorporationTypes eIndex, bool bValue);

	CvCity* getTradeCity(int iIndex) const; // Exposed to Python
	int getTradeRoutes() const; // Exposed to Python
	void clearTradeRoutes();
	void updateTradeRoutes();

	void clearOrderQueue(); // Exposed to Python
	void pushOrder(OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend, bool bForce = false, CvPlot* deliveryDestination = NULL, UnitAITypes contractedAIType = NO_UNITAI, uint8_t contractFlags = 0); // Exposed to Python
	void popOrder(int iNum, bool bFinish = false, bool bChoose = false, bool bResolveList = true); // Exposed to Python
	void startHeadOrder();
	void stopHeadOrder();

	int getOrderQueueLength() const; // Exposed to Python
	bst::optional<OrderData> getHeadOrder() const { return m_orderQueue.empty() ? bst::optional<OrderData>() : m_orderQueue.front(); }
	bst::optional<OrderData> getTailOrder() const { return m_orderQueue.empty() ? bst::optional<OrderData>() : m_orderQueue.back(); }
	OrderData getOrderAt(int index) const { return m_orderQueue[index]; } // Exposed to Python

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

	// Exposed to Python
	void setWallOverridePoints(const std::vector< std::pair<float, float> >& kPoints); /* points are given in world space ... i.e. PlotXToPointX, etc */
	DllExport const std::vector< std::pair<float, float> >& getWallOverridePoints() const;

	bool isEventTriggerPossible(EventTriggerTypes eTrigger) const;
	int getTriggerValue(EventTriggerTypes eTrigger) const;
	bool canApplyEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const;
	void applyEvent(EventTypes eEvent, const EventTriggeredData* pTriggeredData);
	bool isEventOccured(EventTypes eEvent) const;
	void setEventOccured(EventTypes eEvent, bool bOccured);

	int getBuildingYieldChange(BuildingTypes eBuilding, YieldTypes eYield) const; // Exposed to Python
	void setBuildingYieldChange(BuildingTypes eBuilding, YieldTypes eYield, int iChange); // Exposed to Python
	void changeBuildingYieldChange(BuildingTypes eBuilding, YieldTypes eYield, int iChange);
	int getBuildingCommerceChange(BuildingTypes eBuilding, CommerceTypes eCommerce) const; // Exposed to Python
	void setBuildingCommerceChange(BuildingTypes eBuilding, CommerceTypes eCommerce, int iChange); // Exposed to Python
	void changeBuildingCommerceChange(BuildingTypes eBuilding, CommerceTypes eCommerce, int iChange);
	int getBuildingHappyChange(BuildingTypes eBuilding) const; // Exposed to Python
	void setBuildingHappyChange(BuildingTypes eBuilding, int iChange); // Exposed to Python
	int getBuildingHealthChange(BuildingTypes eBuilding) const; // Exposed to Python
	void setBuildingHealthChange(BuildingTypes eBuilding, int iChange); // Exposed to Python

	PlayerTypes getLiberationPlayer(bool bConquest) const; // Exposed to Python
	void liberate(bool bConquest); // Exposed to Python

	void changeNoBonusCount(BonusTypes eBonus, int iChange); // Exposed to Python
	int getNoBonusCount(BonusTypes eBonus) const;
	bool isNoBonus(BonusTypes eBonus) const; // Exposed to Python

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

	int getUnitProductionModifier(const UnitTypes eIndex) const; //Exposed to Python
	void changeUnitProductionModifier(const UnitTypes eIndex, const int iChange);

	bool hadVicinityBonus(BonusTypes eIndex) const;
	bool hadRawVicinityBonus(BonusTypes eIndex) const;

	int getBonusDefenseChanges(const BonusTypes eIndex) const;
	void changeBonusDefenseChanges(const BonusTypes eIndex, const int iChange);

	int getBonusCommerceRateModifier(CommerceTypes eIndex) const;
	void changeBonusCommerceRateModifier(CommerceTypes eIndex, int iChange);
	bool isBuiltFoodProducedUnit() const;
	void setBuiltFoodProducedUnit(bool bNewValue);
	int getSpecialistGoodHealth() const; // Exposed to Python
	int getSpecialistBadHealth() const; // Exposed to Python
	int getSpecialistHappiness() const; // Exposed to Python
	int getSpecialistUnhappiness() const; // Exposed to Python
	void changeSpecialistGoodHealth(int iChange);
	void changeSpecialistBadHealth(int iChange);
	void changeSpecialistHappiness(int iChange);
	void changeSpecialistUnhappiness(int iChange);
	int getImprovementGoodHealth() const; // Exposed to Python
	int getImprovementBadHealth() const; // Exposed to Python
	void updateImprovementHealth();
	int getBonusCommerceRateModifier(CommerceTypes eIndex, BonusTypes eBonus) const; // Exposed to Python
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
	int getExtraYieldTurns() const;
	void changeExtraYieldTurns (int iChange);
	void setExtraYieldTurns(int iNewVal);
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
	void checkBuildings(bool bBonus = true, bool bCivics = true, bool bWar = true, bool bPower = true, bool bPopulation = true, bool bAlertOwner = true);
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

	void setDisabledBuilding(const BuildingTypes eIndex, const bool bNewValue);
	bool isDisabledBuilding(const short iIndex) const;

	void setReligiouslyDisabledBuilding(BuildingTypes eIndex, bool bNewValue);
	bool isReligiouslyDisabledBuilding(BuildingTypes eIndex) const;

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

	SpecialistTypes getBestSpecialist(int iExtra);

	int getNumPopulationEmployed() const;
	void setNumPopulationEmployed(int iNewValue);
	void changeNumPopulationEmployed(int iNewValue);

	void removeWorstCitizenActualEffects(int iNumCitizens, int& iGreatPeopleRate, int& iHappiness, int& iHealthiness, int*& aiYields, int*& aiCommerces);

	void changeHealthPercentPerPopulation(int iChange);
	int calculatePopulationHealth() const;
	void changeHappinessPercentPerPopulation(int iChange);
	int calculatePopulationHappiness() const;

	int getAssignedSpecialistCount() const;

	int getBonusCommercePercentChanges(CommerceTypes eIndex) const;
	int getBonusCommercePercentChanges(CommerceTypes eIndex, BonusTypes eBonus) const;
	int getBonusCommercePercentChanges(CommerceTypes eIndex, BuildingTypes eBuilding) const;
	void changeBonusCommercePercentChanges(CommerceTypes eIndex, int iChange);

	bool isAutomatedCanBuild(BuildTypes eBuild) const; //Exposed to Python
	void setAutomatedCanBuild(BuildTypes eBuild, bool bNewValue); //Exposed to Python

	virtual bool AI_isEmphasizeAvoidAngryCitizens() const = 0;
	virtual bool AI_isEmphasizeAvoidUnhealthyCitizens() const = 0;

	virtual int AI_plotValue(const CvPlot* pPlot, bool bAvoidGrowth, bool bRemove, bool bIgnoreFood = false, bool bIgnoreGrowth = false, bool bIgnoreStarvation = false) = 0;

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
	virtual bool AI_avoidGrowth() = 0; // Exposed to Python
	virtual int AI_specialistValue(SpecialistTypes eSpecialist, bool bAvoidGrowth, bool bRemove) = 0;
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
			for (std::vector<ScoredBuilding>::const_iterator itr = scores.begin(); itr != scores.end(); ++itr)
			{
				averageScore = averageScore + itr->score / scores.size();
				minScore = std::min(minScore, itr->score);
				maxScore = std::max(maxScore, itr->score);
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
	virtual bool AI_isEmphasize(EmphasizeTypes eIndex) const = 0; // Exposed to Python
	virtual void AI_setEmphasize(EmphasizeTypes eIndex, bool bNewValue) = 0;
	virtual bool AI_isEmphasizeSpecialist(SpecialistTypes eIndex) const = 0;
	virtual void AI_setEmphasizeSpecialist(SpecialistTypes eIndex, bool bNewValue) = 0;
	virtual int AI_getBestBuildValue(int iIndex) const = 0;
	virtual void AI_markBestBuildValuesStale() = 0;

	virtual int AI_getTargetSize() = 0;
	virtual int AI_getGoodTileCount() const = 0;
	virtual int AI_getImprovementValue(CvPlot* pPlot, ImprovementTypes eImprovement, int iFoodPriority, int iProductionPriority, int iCommercePriority, int iFoodChange) = 0;
	virtual void AI_getYieldMultipliers( int &iFoodMultiplier, int &iProductionMultiplier, int &iCommerceMultiplier, int &iDesiredFoodChange ) = 0;

	virtual int AI_totalBestBuildValue(const CvArea* pArea) const = 0;
	virtual int AI_countBestBuilds(const CvArea* pArea) const = 0; // Exposed to Python
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
	int cultureDistance(int iDX, int iDY, bool bForce = false) const; // Exposed to Python
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
	int m_iPowerGoodHealth;
	int m_iPowerBadHealth;
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

	int m_iLostProductionBase;
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
#define INVALID_GROWTH_PERCENT_LOG ((float)-10000.0)	//	Used to detect old format saves when loading

	bool m_bBuiltFoodProducedUnit;
	bool m_bResetTechs;
	int m_iExtraYieldTurns;
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
	int m_iDirtyPowerCount;
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
	// < M.A.D. Nukes Start >
	int m_iMADIncoming;
	// < M.A.D. Nukes Start >
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
/************************************************************************************************/
/* UNOFFICIAL_PATCH                       12/07/09                         denev & jdog5000     */
/*                                                                                              */
/* Bugfix                                                                                       */
/************************************************************************************************/
	bool m_bPopProductionProcess;
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/

	PlayerTypes m_eOwner;
	PlayerTypes m_ePreviousOwner;
	PlayerTypes m_eOriginalOwner;
	CultureLevelTypes m_eCultureLevel;

/************************************************************************************************/
/* REVOLUTION_MOD                         06/10/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int m_iRevolutionIndex;
	int m_iLocalRevIndex;
	int m_iRevIndexAverage;
	int m_iRevolutionCounter;
	int m_iReinforcementCounter;
/************************************************************************************************/
/* REVOLUTION_MOD                          END                                                  */
/************************************************************************************************/
	//TB Combat Mod (Buildings) begin
	int m_iTotalFrontSupportPercentModifier;
	int m_iTotalShortRangeSupportPercentModifier;
	int m_iTotalMediumRangeSupportPercentModifier;
	int m_iTotalLongRangeSupportPercentModifier;
	int m_iTotalFlankSupportPercentModifier;
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
	//TB Combat Mod (Buildings) end
	//TB Building Tags
//Team Project (3)
	int m_iExtraLocalCaptureProbabilityModifier;
	int m_iExtraLocalCaptureResistanceModifier;
	int m_iExtraLocalDynamicDefense;
	int m_iExtraRiverDefensePenalty;
	int m_iExtraLocalRepel;
	int m_iExtraMinDefense;
	int m_iExtraBuildingDefenseRecoverySpeedModifier;
	int m_iModifiedBuildingDefenseRecoverySpeedCap;
	int m_iExtraCityDefenseRecoverySpeedModifier;
//Team Project (1)
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
	//
	int* m_aiSeaPlotYield;
	int* m_aiRiverPlotYield;
	int* m_aiBaseYieldRate;
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
	mutable int* m_paiBuildingReplaced;
	mutable bool m_bHasCalculatedBuildingReplacement;

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

/************************************************************************************************/
/* phunny_pharmer                Start		 04/21/10                                           */
/************************************************************************************************/
	mutable std::map<int,int> m_aCultureDistances;
/************************************************************************************************/
/* phunny_pharmer                End		 04/21/10                                           */
/************************************************************************************************/

	void doGrowth();
	void doCulture();
	void doPlotCulture(bool bUpdate, PlayerTypes ePlayer, int iCultureRate);
	void doProduction(bool bAllowNoProduction);
	void doDecay();
	void doReligion();
	void doGreatPeople();
	void doMeltdown();
	bool doCheckProduction();
/************************************************************************************************/
/* Afforess	Meteorology Modmod Start		 09/25/09                                           */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	void doPromotion();

/************************************************************************************************/
/* Afforess	                               END                                                  */
/************************************************************************************************/

	int getExtraProductionDifference(int iExtra, UnitTypes eUnit) const;
	int getExtraProductionDifference(int iExtra, BuildingTypes eBuilding) const;
	int getExtraProductionDifference(int iExtra, ProjectTypes eProject) const;
	int getExtraProductionDifference(int iExtra, int iModifier) const;
	int getHurryCostModifier(UnitTypes eUnit, bool bIgnoreNew) const;
	int getHurryCostModifier(BuildingTypes eBuilding, bool bIgnoreNew) const;
	int getHurryCostModifier(int iBaseModifier, int iProduction, bool bIgnoreNew) const;
	int getHurryCost(bool bExtra, UnitTypes eUnit, bool bIgnoreNew) const;
	int getHurryCost(bool bExtra, BuildingTypes eBuilding, bool bIgnoreNew) const;
	int getHurryCost(bool bExtra, int iProductionLeft, int iHurryModifier, int iModifier) const;
	int getHurryPopulation(HurryTypes eHurry, int iHurryCost) const;
	int getHurryGold(HurryTypes eHurry, int iHurryCost) const;
	bool canHurryUnit(HurryTypes eHurry, UnitTypes eUnit, bool bIgnoreNew) const;
	bool canHurryBuilding(HurryTypes eHurry, BuildingTypes eBuilding, bool bIgnoreNew) const;
	void recalculateMaxFoodKeptPercent();
	void recalculatePopulationgrowthratepercentage();
	virtual bool AI_addBestCitizen(bool bWorkers, bool bSpecialists, int* piBestPlot = NULL, SpecialistTypes* peBestSpecialist = NULL) = 0;
	virtual bool AI_removeWorstCitizen(SpecialistTypes eIgnoreSpecialist = NO_SPECIALIST) = 0;
	void calculateBuildingReplacements() const;
	void changeBuildingReplacementCount(BuildingTypes eBuilding, bool bAdd);

	//TB Building tags
//Team Project (3)
	void setExtraLocalCaptureProbabilityModifier(int iValue);
	void changeExtraLocalCaptureProbabilityModifier(int iChange);

	void setExtraLocalCaptureResistanceModifier(int iValue);
	void changeExtraLocalCaptureResistanceModifier(int iChange);

	short m_iZoCCount;
	void changeZoCCount(short iChange);
//Team Project (1)
public:
	int getTechSpecialistHappinessTypes(TechTypes eTech, SpecialistTypes eSpecialist) const;

	int localCitizenCaptureResistance() const;
private:
	void changeTechSpecialistHappinessTypes(TechTypes eTech, SpecialistTypes eSpecialist, int iChange);
public:
	int getTechSpecialistHappiness(TechTypes eTech) const;
private:
	void changeTechSpecialistHappiness(TechTypes eTech, int iChange);
	void updateExtraTechSpecialistHappiness();
	int getExtraTechSpecialistHappiness() const;
	int getTechHappiness(TechTypes eTech) const;
	void changeTechHappiness(TechTypes eTech, int iChange);
	void updateExtraTechHappiness();
	int getExtraTechHappiness() const;
public:
	int getExtraTechHappinessTotal() const;
	int getExtraTechUnHappinessTotal() const;
	int getTechSpecialistHealthTypes(TechTypes eTech, SpecialistTypes eSpecialist) const;
private:
	void changeTechSpecialistHealthTypes(TechTypes eTech, SpecialistTypes eSpecialist, int iChange);
public:
	int getTechSpecialistHealth(TechTypes eTech) const;
private:
	void changeTechSpecialistHealth(TechTypes eTech, int iChange);
	void updateExtraTechSpecialistHealth();
	int getExtraTechSpecialistHealth() const;
	int getTechHealth(TechTypes eTech) const;
	void changeTechHealth(TechTypes eTech, int iChange);
	void updateExtraTechHealth();
	int getExtraTechHealth() const;
public:
	int getExtraTechHealthTotal() const;
	int getExtraTechUnHealthTotal() const;
	int getLocalSpecialistExtraYield(SpecialistTypes eSpecialist, YieldTypes eYield) const;
	int getLocalSpecialistExtraCommerce(SpecialistTypes eSpecialist, CommerceTypes eCommerce) const;
private:
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
	int m_recalcBuilding;
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
		DECLARE_MAP_FUNCTOR(CvCity, void, updatePowerHealth);
		DECLARE_MAP_FUNCTOR(CvCity, void, updateExtraSpecialistYield);
		DECLARE_MAP_FUNCTOR(CvCity, void, updateExtraSpecialistCommerce);
		DECLARE_MAP_FUNCTOR(CvCity, void, updateReligionCommerce);
		DECLARE_MAP_FUNCTOR(CvCity, void, updateBuildingCommerce);
		DECLARE_MAP_FUNCTOR(CvCity, void, updateCorporation);
		DECLARE_MAP_FUNCTOR(CvCity, void, updateYield);
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
		DECLARE_MAP_FUNCTOR_2(CvCity, void, changeBaseYieldRate, YieldTypes, int);

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

		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, bool, isReligiouslyDisabledBuilding, BuildingTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, bool, canConstruct, BuildingTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, bool, canTrain, UnitTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, bool, isHasReligion, ReligionTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, bool, isHasCorporation, CorporationTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, bool, hasBonus, BonusTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, bool, isCoastal, int);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, int, getNumBuilding, BuildingTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, int, getNumRealBuilding, BuildingTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, int, getNumActiveBuilding, BuildingTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, int, getCommerceRateTimes100, CommerceTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, int, getBaseCommerceRateTimes100, CommerceTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, int, getCultureTimes100, PlayerTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, int, getYieldRate, YieldTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, int, getModifiedBaseYieldRate, YieldTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvCity, const CvPlotGroup*, plotGroup, PlayerTypes);

		DECLARE_MAP_FUNCTOR_CONST_2(CvCity, bool, isRevealed, TeamTypes, bool);
	};
};

#endif
