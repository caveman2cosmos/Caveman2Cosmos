#pragma once

#ifndef CVSTRUCTS_H
#define CVSTRUCTS_H

// structs.h

#include "CvString.h"

class BoolExpr;
class CvPlot;

// XXX these should not be in the DLL per se (if the user changes them, we are screwed...)

//TB Combat Mod
typedef std::vector< std::pair<UnitCombatTypes, int> > UnitCombatModifierArray;
typedef std::vector< std::pair<TechTypes, int> > TechModifierArray;
typedef std::vector< std::pair<TerrainTypes, int> > TerrainModifierArray;
typedef std::vector< std::pair<FeatureTypes, int> > FeatureModifierArray;
typedef std::vector< std::pair<BuildTypes, int> > BuildModifierArray;
typedef std::vector< std::pair<BonusTypes, int> > BonusModifierArray;
//typedef std::vector< std::pair<PromotionTypes, int> > PromotionModifierArray;
typedef std::vector< std::pair<PromotionLineTypes, int> > PromotionLineModifierArray;
//typedef std::vector< std::pair<ImprovementTypes, int> > ImprovementModifierArray;
typedef std::vector< std::pair<InvisibleTypes, int> > InvisibilityArray;
typedef std::vector< std::pair<EraTypes, int> > EraArray;
typedef std::vector< std::pair<PropertyTypes, int> > AidArray;

struct plotInfo
{
	plotInfo();
	std::string ToJSON();

	int index;
	bool worked;
	bool owned;
	bool bonusImproved;
	int yieldValue;
	short yields[NUM_YIELD_TYPES];
	BonusTypes currentBonus;
	ImprovementTypes currentImprovement;
	FeatureTypes currentFeature;
	BuildTypes currentBuild;
};

struct AidStruct
{
	PropertyTypes eProperty;
	int iChange;
	operator int() const {return (int)eProperty;}
	bool operator< (const AidStruct& rhs) const {return (int)eProperty < (int)rhs.eProperty;}
};
struct PromotionModifier
{
	PromotionTypes ePromotion;
	int iModifier;
	operator int() const {return (int)ePromotion;}
	bool operator< (const PromotionModifier& rhs) const {return (int)ePromotion < (int)rhs.ePromotion;}
};
struct PromotionLineModifier
{
	PromotionLineTypes ePromotionLine;
	int iModifier;
	operator int() const {return (int)ePromotionLine;}
	bool operator< (const PromotionLineModifier& rhs) const {return (int)ePromotionLine < (int)rhs.ePromotionLine;}
};
struct AfflictOnAttackChange
{
	PromotionLineTypes eAfflictionLine;
	int iProbabilityChange;
	int iMelee;
	int iDistance;
	int iImmediate;
	operator int() const {return (int)eAfflictionLine;}
	bool operator< (const AfflictOnAttackChange& rhs) const {return (int)eAfflictionLine < (int)rhs.eAfflictionLine;}
};
struct PlotTeamVisibilityIntensity
{
	TeamTypes eTeam;
	InvisibleTypes eInvisibility;
	int iUnitID;
	int iIntensity;
	int iCount;
	operator int() const {return (int)eTeam;}
	bool operator< (const PlotTeamVisibilityIntensity& rhs) const {return (int)eTeam < (int)rhs.eTeam;}
};
struct FreePromoTypes
{
	PromotionTypes ePromotion;
	const BoolExpr* m_pExprFreePromotionCondition;
	operator int() const {return (int)ePromotion;}
	bool operator< (const FreePromoTypes& rhs) const {return (int)ePromotion < (int)rhs.ePromotion;}
};

struct AfflictOnAttack
{
	PromotionLineTypes eAfflictionLine;
	int iProbability;
	int iMelee;
	int iDistance;
	int iImmediate;
	operator int() const {return (int)eAfflictionLine;}
	bool operator< (const AfflictOnAttack& rhs) const {return (int)eAfflictionLine < (int)rhs.eAfflictionLine;}
};
struct TerrainModifier
{
	TerrainTypes eTerrain;
	int iModifier;
	operator int() const {return (int)eTerrain;}
	bool operator< (const TerrainModifier& rhs) const {return (int)eTerrain < (int)rhs.eTerrain;}
};
struct FeatureModifier
{
	FeatureTypes eFeature;
	int iModifier;
	operator int() const {return (int)eFeature;}
	bool operator< (const FeatureModifier& rhs) const {return (int)eFeature < (int)rhs.eFeature;}
};
struct BuildModifier
{
	BuildTypes eBuild;
	int iModifier;
	operator int() const {return (int)eBuild;}
	bool operator< (const BuildModifier& rhs) const {return (int)eBuild < (int)rhs.eBuild;}
};
struct UnitCombatModifier
{
	UnitCombatTypes eUnitCombat;
	int iModifier;
	operator int() const {return (int)eUnitCombat;}
	bool operator< (const UnitCombatModifier& rhs) const {return (int)eUnitCombat < (int)rhs.eUnitCombat;}
};
struct BonusModifier
{
	BonusTypes eBonus;
	int iModifier;
	operator int() const {return (int)eBonus;}
	bool operator< (const BonusModifier& rhs) const {return (int)eBonus < (int)rhs.eBonus;}
};
struct ImprovementModifier
{
	ImprovementTypes eImprovement;
	int iModifier;
	operator int() const {return (int)eImprovement;}
	bool operator< (const ImprovementModifier& rhs) const {return (int)eImprovement < (int)rhs.eImprovement;}
};
struct DisallowedTraitType
{
	TraitTypes eTrait;
	operator int() const {return (int)eTrait;}
	bool operator< (const DisallowedTraitType& rhs) const {return (int)eTrait < (int)rhs.eTrait;}
};
struct DomainModifier
{
	DomainTypes eDomain;
	int iModifier;
	operator int() const {return (int)eDomain;}
	bool operator< (const DomainModifier& rhs) const {return (int)eDomain < (int)rhs.eDomain;}
};
struct TechModifier2
{
	TechTypes eTech;
	int iModifier;
	operator int() const {return (int)eTech;}
	bool operator< (const TechModifier2& rhs) const {return (int)eTech < (int)rhs.eTech;}
};
struct BuildingModifier
{
	BuildingTypes eBuilding;
	int iModifier;
	int* operator&() {return reinterpret_cast<int*>(&eBuilding);}
	operator int() const {return (int)eBuilding;}
	bool operator< (const BuildingModifier& rhs) const {return (int)eBuilding < (int)rhs.eBuilding;}
};
struct SpecialBuildingModifier
{
	SpecialBuildingTypes eSpecialBuilding;
	int iModifier;
	operator int() const {return (int)eSpecialBuilding;}
	bool operator< (const SpecialBuildingModifier& rhs) const {return (int)eSpecialBuilding < (int)rhs.eSpecialBuilding;}
};
struct UnitModifier
{
	UnitTypes eUnit;
	int iModifier;
	operator int() const {return (int)eUnit;}
	bool operator< (const UnitModifier& rhs) const {return (int)eUnit < (int)rhs.eUnit;}
};
struct SpecialUnitModifier
{
	SpecialUnitTypes eSpecialUnit;
	int iModifier;
	operator int() const {return (int)eSpecialUnit;}
	bool operator< (const SpecialUnitModifier& rhs) const {return (int)eSpecialUnit < (int)rhs.eSpecialUnit;}
};
struct ImprovementYieldModifier
{
	ImprovementTypes eImprovement;
	YieldTypes eYield;
	int iModifier;
	operator int() const {return (int)eImprovement;}
	bool operator< (const ImprovementYieldModifier& rhs) const {return (int)eImprovement < (int)rhs.eImprovement;}
};
struct CivicOptionTypeBool
{
	CivicOptionTypes eCivicOption;
	bool bBool;
	operator int() const {return (int)eCivicOption;}
	bool operator< (const CivicOptionTypeBool& rhs) const {return (int)eCivicOption < (int)rhs.eCivicOption;}
};
struct HealUnitCombat
{
	UnitCombatTypes eUnitCombat;
	int iHeal;
	int iAdjacentHeal;
	operator int() const {return (int)eUnitCombat;}
	bool operator< (const HealUnitCombat& rhs) const {return (int)eUnitCombat < (int)rhs.eUnitCombat;}
};
struct GroupSpawnUnitCombat
{
	UnitCombatTypes eUnitCombat;
	int iChance;
	CvWString m_szTitle;
	operator int() const {return (int)eUnitCombat;}
	bool operator< (const GroupSpawnUnitCombat& rhs) const {return (int)eUnitCombat < (int)rhs.eUnitCombat;}
};
struct ImprovementBuildTypes
{
	BuildTypes eBuildType;
	operator int() const { return eBuildType; }
	bool operator< (const ImprovementBuildTypes& rhs) const { return (int)eBuildType < (int)rhs.eBuildType; }
};

struct InvisibleTerrainChanges
{
	InvisibleTypes eInvisible;
	TerrainTypes eTerrain;
	int iIntensity;
	operator int() const {return (int)eInvisible;}
	bool operator< (const InvisibleTerrainChanges& rhs) const {return (int)eInvisible < (int)rhs.eInvisible;}
};
struct InvisibleFeatureChanges
{
	InvisibleTypes eInvisible;
	FeatureTypes eFeature;
	int iIntensity;
	operator int() const {return (int)eInvisible;}
	bool operator< (const InvisibleFeatureChanges& rhs) const {return (int)eInvisible < (int)rhs.eInvisible;}
};
struct InvisibleImprovementChanges
{
	InvisibleTypes eInvisible;
	ImprovementTypes eImprovement;
	int iIntensity;
	operator int() const {return (int)eInvisible;}
	bool operator< (const InvisibleImprovementChanges& rhs) const {return (int)eInvisible < (int)rhs.eInvisible;}
};
struct EnabledCivilizations
{
	CivilizationTypes eCivilization;
	operator int() const {return (int)eCivilization;}
	bool operator< (const EnabledCivilizations& rhs) const {return (int)eCivilization < (int)rhs.eCivilization;}
};
struct AfflictionLineChanges
{
	PromotionLineTypes eAfflictionLine;
	int iChange;
	operator int() const {return (int)eAfflictionLine;}
	bool operator< (const AfflictionLineChanges& rhs) const {return (int)eAfflictionLine < (int)rhs.eAfflictionLine;}
};
struct BonusAidModifiers
{
	BonusTypes eBonusType;
	PropertyTypes ePropertyType;
	int iModifier;
	operator int() const {return (int)eBonusType;}
	bool operator< (const BonusAidModifiers& rhs) const {return (int)eBonusType < (int)rhs.eBonusType;}
};
struct AidRateChanges
{
	PropertyTypes ePropertyType;
	int iChange;
	operator int() const {return (int)ePropertyType;}
	bool operator< (const AidRateChanges& rhs) const {return (int)ePropertyType < (int)rhs.ePropertyType;}
};
struct PrereqBuilding
{
	BuildingTypes eBuilding;
	int iMinimumRequired;
	operator int() const {return (int)eBuilding;}
	bool operator< (const PrereqBuilding& rhs) const {return (int)eBuilding < (int)rhs.eBuilding;}
};
struct TerrainStructs
{
	TerrainTypes eTerrain;
	TechTypes ePrereqTech;
	int iTime;
	operator int() const {return (int)eTerrain;}
	bool operator< (const TerrainStructs& rhs) const {return (int)eTerrain < (int)rhs.eTerrain;}
};
struct PlaceBonusTypes
{
	BonusTypes eBonus;
	int iProbability;
	bool bRequiresAccess;
	TechTypes ePrereqTech;
	TerrainTypes ePrereqTerrain;
	FeatureTypes ePrereqFeature;
	MapCategoryTypes ePrereqMapCategory;
	operator int() const {return (int)eBonus;}
	bool operator< (const PlaceBonusTypes& rhs) const {return (int)eBonus < (int)rhs.eBonus;}
};
struct PromotionLineAfflictionModifier
{
	PromotionLineTypes ePromotionLine;
	int iModifier;
	bool bWorkedTile;
	bool bVicinity;
	bool bAccessVolume;
	operator int() const {return (int)ePromotionLine;}
	bool operator< (const PromotionLineAfflictionModifier& rhs) const {return (int)ePromotionLine < (int)rhs.ePromotionLine;}
};
//TB Combat Mod end

struct XYCoords
{
	XYCoords(int x = INVALID_PLOT_COORD, int y = INVALID_PLOT_COORD);
	XYCoords(const CvPlot& plot);

	CvPlot* plot() const;

	bool operator<  (const XYCoords xy) const { return ((iY < xy.iY) || (iY == xy.iY && iX < xy.iX)); }
	bool operator<= (const XYCoords xy) const { return ((iY < xy.iY) || (iY == xy.iY && iX <= xy.iX)); }
	bool operator!= (const XYCoords xy) const { return (iY != xy.iY || iX != xy.iX); }
	bool operator== (const XYCoords xy) const { return (!(iY != xy.iY || iX != xy.iX)); }
	bool operator>= (const XYCoords xy) const { return ((iY > xy.iY) || (iY == xy.iY && iX >= xy.iX)); }
	bool operator>  (const XYCoords xy) const { return ((iY > xy.iY) || (iY == xy.iY && iX > xy.iX)); }

	int iX;
	int iY;
};

struct IDInfo
{

	IDInfo(PlayerTypes eOwner=NO_PLAYER, int iID=FFreeList::INVALID_INDEX) : eOwner(eOwner), iID(iID) {}
	PlayerTypes eOwner;
	int iID;

	bool operator== (const IDInfo& info) const
	{
		return (eOwner == info.eOwner && iID == info.iID);
	}

	void reset()
	{
		eOwner = NO_PLAYER;
		iID = FFreeList::INVALID_INDEX;
	}
};

struct GameTurnInfo
{
	int iMonthIncrement;
	int iNumGameTurnsPerIncrement;

	GameTurnInfo() : iMonthIncrement(0), iNumGameTurnsPerIncrement(0) {}
};

//	Koshling - need to cram some extra info into some orders.  Existing usage
//	means iData1 and iData2 can actually be shorts, so we divide them into two 16 bit fields
//	iData1 gets the unitType in the external data (previous usage), and the location the unit
//	is to head once built (if any)
//	iData2 gets the UnitAI being built against in the external (previous usage) and the contract-requested
//	UnitAI in the auxiliary (which can be NO_UNITAI)
#define	EXTERNAL_ORDER_IDATA(iData)				(short)((iData) & 0xFFFF)
#define	INTERNAL_AUXILIARY_ORDER_IDATA(iData)	(short)(((iData) & 0xffff0000) >> 16)
#define	PACK_INTERNAL_ORDER_IDATA(iBase, iAux)	(((unsigned int)(iBase) & 0xFFFF) | (((unsigned int)(iAux)) << 16))

struct OrderData
{
	OrderTypes eOrderType;
	union {
		struct {
			int iData1;
			int iData2;
		};
		struct { // Untyped
			uint16_t iData1_external;
			uint16_t iData1_aux;
			uint16_t iData2_external;
			uint16_t iData2_aux;
		} raw;
		struct { // For units
			/*UnitTypes*/uint16_t type;
			uint16_t plotIndex; // aux, 0xFFFF for none

			/*UnitAITypes*/uint16_t AIType;
			/*UnitAITypes*/uint8_t contractedAIType; // aux, 0xFF for none
			uint8_t contractFlags; // aux
		} unit;
		struct {
			/*BuildingTypes*/int type;
			int padding;
		} building;
		struct {
			/*ProjectTypes*/int type;
			int padding;
		} project;
		struct {
			/*ProcessTypes*/int type;
			int padding;
		} process;
		struct {
			int id;
			int padding;
		} orderlist;
	};
	bool bSave;

	OrderData(OrderTypes eOrderType = NO_ORDER, int iData1 = 0, int iData2 = 0, bool bSave = false)
		: eOrderType(eOrderType), iData1(iData1), iData2(iData2), bSave(bSave) {}
	OrderData(const OrderData& other)
		: eOrderType(other.eOrderType), iData1(other.iData1), iData2(other.iData2), bSave(other.bSave) {}

	bool operator==(const OrderData& other) const
	{
		return eOrderType == other.eOrderType && iData1 == other.iData1 && iData2 == other.iData2 && bSave == other.bSave;
	}

	static OrderData createUnitOrder(UnitTypes type, UnitAITypes AIType, uint16_t plotIndex, uint8_t contractFlags, UnitAITypes contractedAIType, bool bSave)
	{
		OrderData order(ORDER_TRAIN);
		order.unit.type = static_cast<uint16_t>(type);
		order.unit.AIType = static_cast<uint16_t>(AIType);
		order.unit.plotIndex = plotIndex;
		order.unit.contractFlags = contractFlags;
		order.unit.contractedAIType = static_cast<uint8_t>(contractedAIType);
		order.bSave = bSave;
		return order;
	}
	static OrderData createBuildingOrder(BuildingTypes type, bool bSave)
	{
		OrderData order(ORDER_CONSTRUCT);
		order.building.type = static_cast<uint16_t>(type);
		order.bSave = bSave;
		return order;
	}
	static OrderData createProjectOrder(ProjectTypes type, bool bSave)
	{
		OrderData order(ORDER_CREATE);
		order.project.type = static_cast<uint16_t>(type);
		order.bSave = bSave;
		return order;
	}
	static OrderData createProcessOrder(ProcessTypes type, bool bSave)
	{
		OrderData order(ORDER_MAINTAIN);
		order.process.type = static_cast<uint16_t>(type);
		order.bSave = bSave;
		return order;
	}
	// Convert to external view for the exe
	OrderData to_external() const
	{
		return OrderData(eOrderType, EXTERNAL_ORDER_IDATA(iData1), EXTERNAL_ORDER_IDATA(iData2), bSave);
	}

	OrderTypes getOrderType() const { return eOrderType; }

	UnitTypes getUnitType() const {
		FAssert(eOrderType == ORDER_TRAIN);
		return static_cast<UnitTypes>(unit.type);
	}
	void setUnitType(UnitTypes newUnitType) {
		FAssert(eOrderType == ORDER_TRAIN);
		unit.type = static_cast<uint16_t>(newUnitType);
	}

	UnitAITypes getUnitAIType() const {
		FAssert(eOrderType == ORDER_TRAIN);
		return static_cast<UnitAITypes>(unit.AIType);
	}
	void setUnitAIType(UnitAITypes newUnitAIType) {
		FAssert(eOrderType == ORDER_TRAIN);
		unit.AIType = static_cast<uint16_t>(newUnitAIType);
	}

	uint16_t getUnitPlotIndex() const {
		FAssert(eOrderType == ORDER_TRAIN);
		return unit.plotIndex;
	}
	uint8_t getUnitContractFlags() const {
		FAssert(eOrderType == ORDER_TRAIN);
		return unit.contractedAIType;
	}
	UnitAITypes getUnitContractedAIType() const {
		FAssert(eOrderType == ORDER_TRAIN);
		return static_cast<UnitAITypes>(unit.contractedAIType);
	}
	BuildingTypes getBuildingType() const {
		FAssert(eOrderType == ORDER_CONSTRUCT);
		return static_cast<BuildingTypes>(building.type);
	}
	void setBuildingType(BuildingTypes newBuildingType) {
		FAssert(eOrderType == ORDER_CONSTRUCT);
		building.type = static_cast<int>(newBuildingType);
	}
	ProjectTypes getProjectType() const {
		FAssert(eOrderType == ORDER_CREATE);
		return static_cast<ProjectTypes>(project.type);
	}
	void setProjectType(ProjectTypes newProjectType) {
		FAssert(eOrderType == ORDER_CREATE);
		project.type = static_cast<int>(newProjectType);
	}
	ProcessTypes getProcessType() const {
		FAssert(eOrderType == ORDER_MAINTAIN);
		return static_cast<ProcessTypes>(process.type);
	}
	void setProcessType(ProcessTypes newProcessType) {
		FAssert(eOrderType == ORDER_MAINTAIN);
		building.type = static_cast<int>(newProcessType);
	}
	int getOrderListID() const {
		FAssert(eOrderType == ORDER_LIST);
		return orderlist.id;
	}

	static const OrderData InvalidOrder;
};

struct _oldOrderData { OrderTypes eOrderType; int iData1; int iData2; bool bSave; };
STATIC_ASSERT(offsetof(OrderData, eOrderType) == offsetof(_oldOrderData, eOrderType), eOrderType_offset_wrong);
STATIC_ASSERT(offsetof(OrderData, iData1) == offsetof(_oldOrderData, iData1), iData1_offset_wrong);
STATIC_ASSERT(offsetof(OrderData, iData2) == offsetof(_oldOrderData, iData2), iData2_offset_wrong);
STATIC_ASSERT(offsetof(OrderData, bSave) == offsetof(_oldOrderData, bSave), bSave_offset_wrong);
STATIC_ASSERT(sizeof(OrderData) == sizeof(_oldOrderData), OrderData_struct_size_incorrect_check_alignment);

//	Contract auxiliary flags
#define	AUX_CONTRACT_FLAG_IS_UNIT_CONTRACT	0x01

struct MissionData
{
	MissionTypes eMissionType;
	int iData1;
	int iData2;
	int iFlags;
	int iPushTurn;

	MissionData()
		: eMissionType(NO_MISSION)
		, iData1(0)
		, iData2(0)
		, iFlags(0)
		, iPushTurn(0)
	{}
};

struct TradeData
{
	TradeableItems m_eItemType; //What type of item is this
	int m_iData; //Any additional data?
	bool m_bOffering; // Is this item up for grabs?
	bool m_bHidden; // Are we hidden?

	TradeData()
		: m_eItemType(TRADE_ITEM_NONE)
		, m_iData(0)
		, m_bOffering(0)
		, m_bHidden(0)
	{}
};

struct EventTriggeredData
{
	int m_iId;
	EventTriggerTypes m_eTrigger;
	int m_iTurn;
	PlayerTypes m_ePlayer;
	int m_iCityId;
	int m_iPlotX;
	int m_iPlotY;
	int m_iUnitId;
	PlayerTypes m_eOtherPlayer;
	int m_iOtherPlayerCityId;
	ReligionTypes m_eReligion;
	CorporationTypes m_eCorporation;
	BuildingTypes m_eBuilding;
	CvWString m_szText;
	CvWString m_szGlobalText;
	bool m_bExpired;

	EventTriggeredData()
		: m_iId(0)
		, m_eTrigger(NO_EVENTTRIGGER)
		, m_iTurn(0)
		, m_ePlayer(NO_PLAYER)
		, m_iCityId(0)
		, m_iPlotX(0)
		, m_iPlotY(0)
		, m_iUnitId(0)
		, m_eOtherPlayer(NO_PLAYER)
		, m_iOtherPlayerCityId(0)
		, m_eReligion(NO_RELIGION)
		, m_eCorporation(NO_CORPORATION)
		, m_eBuilding(NO_BUILDING)
		, m_bExpired(false)
	{}

	int getID() const;
	void setID(int iID);
	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
};

struct VoteSelectionSubData
{
	VoteTypes eVote;
	PlayerTypes ePlayer;
	int iCityId;
	PlayerTypes eOtherPlayer;
	CvWString szText;

	VoteSelectionSubData()
		: eVote(NO_VOTE)
		, ePlayer(NO_PLAYER)
		, iCityId(0)
		, eOtherPlayer(NO_PLAYER)
	{}
};

struct VoteSelectionData
{
	int iId;
	VoteSourceTypes eVoteSource;
	std::vector<VoteSelectionSubData> aVoteOptions;

	VoteSelectionData()
		: iId(0)
		, eVoteSource(NO_VOTESOURCE)
	{}

	int getID() const;
	void setID(int iID);
	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
};

struct VoteTriggeredData
{
	int iId;
	VoteSourceTypes eVoteSource;
	VoteSelectionSubData kVoteOption;

	VoteTriggeredData()
		: iId(0)
		, eVoteSource(NO_VOTESOURCE)
	{}

	int getID() const;
	void setID(int iID);
	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
};

struct EventMessage
{
	CvWString szDescription;
	int iExpirationTurn;
	int iX;
	int iY;

	EventMessage()
		: iExpirationTurn(0)
		, iX(0)
		, iY(0)
	{}

	// python friendly accessors
	std::wstring getDescription() const { return szDescription;	}
};

struct PlotExtraYield
{
	int m_iX;
	int m_iY;
	std::vector<int> m_aeExtraYield;

	PlotExtraYield()
		: m_iX(0)
		, m_iY(0)
	{}

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
};

struct PlotExtraCost
{
	int m_iX;
	int m_iY;
	int m_iCost;

	PlotExtraCost()
		: m_iX(0)
		, m_iY(0)
		, m_iCost(0)
	{}

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
};

typedef std::vector< std::pair<BuildingTypes, int> > BuildingChangeArray;

struct BuildingYieldChange
{
	BuildingTypes eBuilding;
	YieldTypes eYield;
	int iChange;

	BuildingYieldChange()
		: eBuilding(NO_BUILDING)
		, eYield(NO_YIELD)
		, iChange(0)
	{}

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
};

struct BuildingCommerceChange
{
	BuildingTypes eBuilding;
	CommerceTypes eCommerce;
	int iChange;

	BuildingCommerceChange()
		: eBuilding(NO_BUILDING)
		, eCommerce(NO_COMMERCE)
		, iChange(0)
	{}

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
};

struct PropertySpawns
{
	PropertyTypes eProperty;
	UnitTypes eUnit;

	PropertySpawns()
		: eProperty(NO_PROPERTY)
		, eUnit(NO_UNIT)
	{}

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
};
struct BuildingYieldModifier
{
	BuildingTypes eBuilding;
	YieldTypes eYield;
	int iChange;

	BuildingYieldModifier()
		: eBuilding(NO_BUILDING)
		, eYield(NO_YIELD)
		, iChange(0)
	{}

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
};

struct BuildingCommerceModifier
{
	BuildingTypes eBuilding;
	CommerceTypes eCommerce;
	int iChange;

	BuildingCommerceModifier()
		: eBuilding(NO_BUILDING)
		, eCommerce(NO_COMMERCE)
		, iChange(0)
	{}

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);
};

const struct TechCommerceChange
{
	TechCommerceChange(TechTypes eTech, CommerceTypes eCommerce, int iChange)
		: eTech(eTech)
		, eCommerce(eCommerce)
		, iChange(iChange)
	{}

	const TechTypes eTech;
	const CommerceTypes eCommerce;
	const int iChange;
};

struct DllExport PBGameSetupData
{
	PBGameSetupData();

	int iSize;
	int iClimate;
	int iSeaLevel;
	int iSpeed;
	int iEra;

	int iMaxTurns;
	int iCityElimination;
	int iAdvancedStartPoints;
	int iTurnTime;

	int iNumCustomMapOptions;
	int * aiCustomMapOptions;
	int getCustomMapOption(int iOption) {return aiCustomMapOptions[iOption];}

	int iNumVictories;
	bool * abVictories;
	bool getVictory(int iVictory) {return abVictories[iVictory];}

	std::wstring szMapName;
	std::wstring getMapName() {return szMapName;}

	std::vector<bool> abOptions;
	bool getOptionAt(int iOption) {return abOptions[iOption];}

	std::vector<bool> abMPOptions;
	bool getMPOptionAt(int iOption) {return abMPOptions[iOption];}
};

struct DllExport PBPlayerSetupData
{
	int iWho;
	int iCiv;
	int iLeader;
	int iTeam;
	int iDifficulty;

	PBPlayerSetupData()
		: iWho(0)
		, iCiv(0)
		, iLeader(0)
		, iTeam(0)
		, iDifficulty(0)
	{}

	std::wstring szStatusText;
	std::wstring getStatusText() {return szStatusText;}
};

struct DllExport PBPlayerAdminData
{
	std::wstring szName;
	std::wstring getName() {return szName;}
	std::wstring szPing;
	std::wstring getPing() {return szPing;}
	std::wstring szScore;
	std::wstring getScore() {return szScore;}
	bool bHuman;
	bool bClaimed;
	bool bTurnActive;

	PBPlayerAdminData()
		: bHuman(false)
		, bClaimed(false)
		, bTurnActive(false)
	{}
};

class CvUnit;
class CvPlot;

//! An enumeration for indexing units within a CvBattleDefinition
enum BattleUnitTypes
{
	BATTLE_UNIT_ATTACKER,	//!< Index referencing the attacking unit
	BATTLE_UNIT_DEFENDER,	//!< Index referencing the defending unit
	BATTLE_UNIT_COUNT		//!< The number of unit index references
};

//!< An enumeration for indexing times within the CvBattleDefinition
enum BattleTimeTypes
{
	BATTLE_TIME_BEGIN,
	BATTLE_TIME_RANGED,
	BATTLE_TIME_END,
	BATTLE_TIME_COUNT
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  STRUCT:      CvBattleRound
//!  \brief		Represents a single round within a battle.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBattleRound
{
public:
	CvBattleRound();
	bool isValid() const;

	DllExport bool isRangedRound() const;
	void setRangedRound(bool value);

	DllExport int getWaveSize() const;
	DllExport void setWaveSize(int size);

	DllExport int getNumKilled(BattleUnitTypes unitType) const;
	DllExport void setNumKilled(BattleUnitTypes unitType, int value);
	void addNumKilled(BattleUnitTypes unitType, int increment);

	DllExport int getNumAlive(BattleUnitTypes unitType) const;
	DllExport void setNumAlive(BattleUnitTypes unitType, int value);

private:
	int		m_aNumKilled[BATTLE_UNIT_COUNT];		//!< The number of units killed during this round for both sides
	int		m_aNumAlive[BATTLE_UNIT_COUNT];		//!< The number of units alive at the end of this round for both sides
	int		m_iWaveSize;				//!< The number of units that can perform exchanges
	bool	m_bRangedRound;				//!< true if this round is ranged, false otherwise
};

//------------------------------------------------------------------------------------------------

typedef std::vector<CvBattleRound> CvBattleRoundVector;		//!< Type declaration for a collection of battle round definitions

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvMissionDefinition
//!  \brief		Base mission definition struct
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMissionDefinition
{
public:
	CvMissionDefinition(MissionTypes type = NO_MISSION, CvPlot* plot = NULL, CvUnit* attacker = NULL, CvUnit* defender = NULL, float time = 0.f);

	DllExport MissionTypes getMissionType() const;
	void setMissionType(MissionTypes missionType);

	DllExport float getMissionTime() const;
	void setMissionTime(float time);

	DllExport CvUnit *getUnit(BattleUnitTypes unitType) const;
	void setUnit(BattleUnitTypes unitType, CvUnit *unit);

	DllExport const CvPlot *getPlot() const;
	void setPlot(const CvPlot *plot);

	// Check that this mission is valid to send to the exe for displaying
	bool isValid() const;
protected:
	MissionTypes		m_eMissionType;			//!< The type of event
	CvUnit *			m_aUnits[BATTLE_UNIT_COUNT];		//!< The units involved
	float				m_fMissionTime;			//!< The amount of time that the event will take
	const CvPlot *		m_pPlot;					//!< The plot associated with the event
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  STRUCT:     CvBattleDefinition
//!  \brief		A definition passed to CvBattleManager to start a battle between units
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBattleDefinition : public CvMissionDefinition
{
public:
	CvBattleDefinition(CvPlot * plot, CvUnit * attacker, CvUnit * defender);
	DllExport CvBattleDefinition( const CvBattleDefinition & kCopy );
	DllExport ~CvBattleDefinition();

	DllExport int getDamage(BattleUnitTypes unitType, BattleTimeTypes timeType) const;
	void setDamage(BattleUnitTypes unitType, BattleTimeTypes timeType, int damage);
	void addDamage(BattleUnitTypes unitType, BattleTimeTypes timeType, int increment);

	DllExport int getFirstStrikes(BattleUnitTypes unitType) const;
	void setFirstStrikes(BattleUnitTypes unitType, int firstStrikes);
	void addFirstStrikes(BattleUnitTypes unitType, int increment);

	DllExport bool isAdvanceSquare() const;
	void setAdvanceSquare(bool advanceSquare);

	int getNumRangedRounds() const;
	void setNumRangedRounds(int count);
	void addNumRangedRounds(int increment);

	int getNumMeleeRounds() const;
	void setNumMeleeRounds(int count);
	void addNumMeleeRounds(int increment);

	DllExport int getNumBattleRounds() const;
	DllExport void clearBattleRounds();
	DllExport CvBattleRound &getBattleRound(int index);
	DllExport const CvBattleRound &getBattleRound(int index) const;
	DllExport void addBattleRound(const CvBattleRound &round);
	void setBattleRound(int index, const CvBattleRound &round);

private:
	int					m_aDamage[BATTLE_UNIT_COUNT][BATTLE_TIME_COUNT];	//!< The beginning damage of the units
	int					m_aFirstStrikes[BATTLE_UNIT_COUNT];		//!< The number of ranged first strikes the units made
	int					m_iNumRangedRounds;				//!< The number of ranged rounds
	int					m_iNumMeleeRounds;				//!< The number of melee rounds
	bool				m_bAdvanceSquare;					//!< true if the attacking unit should move into the new square
	CvBattleRoundVector	m_aBattleRounds;					//!< The rounds that define the battle plan
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CLASS:      CvAirMissionDefinition
//!  \brief		A definition passed to CvAirMissionManager to start an air mission
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAirMissionDefinition : public CvMissionDefinition
{
public:
	CvAirMissionDefinition(MissionTypes type = MISSION_AIRPATROL, CvPlot * plot = NULL, CvUnit * attacker = NULL, CvUnit * defender = NULL, float time = -1.f);
	DllExport CvAirMissionDefinition( const CvAirMissionDefinition & kCopy );

	DllExport int getDamage(BattleUnitTypes unitType) const;
	void setDamage(BattleUnitTypes unitType, int damage);
	DllExport bool isDead(BattleUnitTypes unitType) const;

private:
	int					m_aDamage[BATTLE_UNIT_COUNT];		//!< The ending damage of the units
};

struct CvWidgetDataStruct
{
	int m_iData1;										//	The first bit of data
	int m_iData2;										//	The second piece of data

	bool m_bOption;									//	A boolean piece of data

	WidgetTypes m_eWidgetType;			//	What the 'type' of this widget is (for parsing help and executing actions)

	CvWidgetDataStruct()
		: m_iData1(0)
		, m_iData2(0)
		, m_bOption(false)
		, m_eWidgetType(WIDGET_PLOT_LIST)
	{}
};

struct DllExport CvPlotIndicatorData
{
	CvPlotIndicatorData()
		: m_kColor()
		, m_eVisibility(PLOT_INDICATOR_VISIBLE_ALWAYS)
		, m_bFlashing(false)
		, m_pUnit(NULL)
		, m_bTestEnemyVisibility(false)
		, m_bVisibleOnlyIfSelected(false)
		, m_bPersistentRotation(false)
	{}

	CvString m_strIcon;
	CvString m_strLabel;
	NiColor m_kColor;
	CvWString m_strHelpText;
	PlotIndicatorVisibilityFlags m_eVisibility;
	bool m_bFlashing;
	NiPoint2 m_Target;
	const CvUnit* m_pUnit;
	bool m_bTestEnemyVisibility;
	bool m_bVisibleOnlyIfSelected;
	bool m_bPersistentRotation;
};

struct DllExport CvGlobeLayerData
{
	explicit CvGlobeLayerData(GlobeLayerTypes eType) : m_eType(eType), m_bGlobeViewRequired(true), m_bShouldCitiesZoom(false), m_iNumOptions(0) { }
	GlobeLayerTypes m_eType;
	CvString m_strName;
	CvString m_strButtonHelpTag;
	CvString m_strButtonStyle;
	bool m_bGlobeViewRequired;
	bool m_bShouldCitiesZoom;
	int m_iNumOptions;
};

struct DllExport CvFlyoutMenuData
{
	CvFlyoutMenuData(FlyoutTypes eType, int iId, int iX, int iY, const wchar_t* strTitle) : m_eFlyout(eType), m_iID(iId), m_iX(iX), m_iY(iY), m_strTitle(strTitle) { }
	FlyoutTypes m_eFlyout;
	int m_iID;
	int m_iX;
	int m_iY;
	CvWString m_strTitle;
};

struct CvStatBase
{
	explicit CvStatBase(const char* strKey) : m_strKey(strKey) { }
	virtual ~CvStatBase() { }
	CvString m_strKey;
};

struct CvStatInt : public CvStatBase
{
	CvStatInt(const char* strKey, int iValue) : CvStatBase(strKey), m_iValue(iValue) { }
	int m_iValue;
};

struct CvStatString : public CvStatBase
{
	CvStatString(const char* strKey, const char* strValue) : CvStatBase(strKey), m_strValue(strValue) { }
	CvString m_strValue;
};

struct CvStatFloat : public CvStatBase
{
	CvStatFloat(const char* strKey, float fValue) : CvStatBase(strKey), m_fValue(fValue) { }
	float m_fValue;
};

struct DllExport CvWBData
{
	CvWBData(int iId, const wchar_t* strHelp, const char* strButton) : m_iId(iId), m_strHelp(strHelp), m_strButton(strButton) { }
	int m_iId;
	CvWString m_strHelp;
	CvString m_strButton;
};


#endif	// CVSTRUCTS_H
