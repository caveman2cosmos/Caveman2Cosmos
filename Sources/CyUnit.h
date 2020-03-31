#pragma once

#ifndef CyUnit_h__
#define CyUnit_h__

class CyArea;
class CyPlot;
class CyCity;
class CvUnit;
class CySelectionGroup;
class CvArtInfoUnit;

//
// Python wrapper class for CvUnit
//
class CyUnit
{
public:
	CyUnit();
	DllExport explicit CyUnit(CvUnit* pUnit);		// Call from C++

	CyPlot* getMADTargetPlot();

	CvUnit* getUnit() { return m_pUnit;	};	// Call from C++
	const CvUnit* getUnit() const { return m_pUnit;	};	// Call from C++
	bool isNone() { return (m_pUnit==NULL); }
	void convert(CyUnit* pUnit);
	void kill(bool bDelay, int /*PlayerTypes*/ ePlayer);

	void NotifyEntity(int /*MissionTypes*/ eMission);

	bool isActionRecommended(int i);

	void doCommand(CommandTypes eCommand, int iData1, int iData2);

	bool canMoveInto(CyPlot* pPlot, bool bAttack, bool bDeclareWar, bool bIgnoreLoad);

	bool isAutoUpgrading();
	bool isAutoPromoting();

	bool canHeal(CyPlot* pPlot);

	bool IsSelected();

	bool canFound(CyPlot* pPlot, bool bTestVisible);
	bool canConstruct(CyPlot* pPlot, int /*BuildingTypes*/ eBuilding);

	bool canHurry(CyPlot* pPlot, bool bTestVisible);
	bool canTrade(CyPlot* pPlot, bool bTestVisible);
	int getEspionagePoints(CyPlot* pPlot);

	bool canBuild(CyPlot* pPlot, int /*BuildTypes*/ eBuild, bool bTestVisible);

	bool canUpgrade(int /*UnitTypes*/ eUnit, bool bTestVisible);

	int /*HandicapTypes*/ getHandicapType();
	int /*CivilizationTypes*/ getCivilizationType();
	int /*SpecialUnitTypes*/ getSpecialUnitType();
	int /*UnitTypes*/ getCaptureUnitType();
	int /*UnitCombatTypes*/ getUnitCombatType();
	int /*DomainTypes*/ getDomainType();

	bool isBarbarian();
	bool isNPC();
	bool isHominid();
	bool isHuman();
	int baseMoves();

	int maxMoves();
	int movesLeft();

	bool canMove();
	bool hasMoved();
	int nukeRange();

	bool isAnimal();
	bool isOnlyDefensive();
	bool isFound();
	bool isGoldenAge();
	bool isFighting();

	int maxHitPoints();
	int currHitPoints();
	bool isHurt();
	bool isDead();
	void setBaseCombatStr(int iCombat);
	int baseCombatStr();

	bool canFight();

	int airBaseCombatStr();
	bool canAirAttack();

	bool isAutomated();
	bool isWaiting();
	bool isFortifyable();
	bool isEstablishable();
	bool isEscapable();
	int experienceNeeded();
	bool isInvisible(int /*TeamTypes*/ eTeam, bool bDebug);
	bool isNukeImmune();

	int bombardRate();

	int cargoSpace();
	void changeCargoSpace(int iChange);
	int getID();

	CySelectionGroup* getGroup();

	int getHotKeyNumber();

	int getX();
	int getY();
	void setXY(int iX, int iY, bool bGroup, bool bUpdate, bool bShow);
	CyPlot* plot();
	CyArea* area();

	int getDamage();
	void setDamage(int iNewValue, int /*PlayerTypes*/ ePlayer);
	void changeDamage(int iChange, int /*PlayerTypes*/ ePlayer);
	int getMoves();
	void setMoves(int iNewValue);
	void finishMoves();
	int getExperience();
	void setExperience(int iNewValue, int iMax);
	void changeExperience(int iChange, int iMax, bool bFromCombat, bool bInBorders, bool bUpdateGlobal);
	int getLevel();
	void setLevel(int iNewLevel);
	int getFacingDirection();
	void rotateFacingDirectionClockwise();
	void rotateFacingDirectionCounterClockwise();
	int getFortifyTurns();
	void setFortifyTurns(int iNewValue);
	bool isRiver();

	int getExtraMoves();
	int getRevoltProtection() const;
	int getPillageChange() const;
	int getUpgradeDiscount() const;

	int getImmobileTimer() const;
	void setImmobileTimer(int iNewValue);

	bool isMadeAttack();
	void setMadeAttack(bool bNewValue);

	bool isMadeInterception();
	void setMadeInterception(bool bNewValue);

	bool isPromotionReady();
	void setPromotionReady(bool bNewValue);
	int getOwner();
	int getVisualOwner();
	int getTeam();

	int /*UnitTypes*/ getUnitType();
	int /*UnitTypes*/ getLeaderUnitType();
	void setLeaderUnitType(int /*UnitTypes*/ leaderUnitType);

	std::wstring getName();
	std::wstring getNameForm(int iForm);
	std::wstring getNameKey();
	std::wstring getNameNoDesc();
	void setName(std::wstring szNewValue);
	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);
	bool isTerrainDoubleMove(int /*TerrainTypes*/ eIndex);
	bool isFeatureDoubleMove(int /*FeatureTypes*/ eIndex);

	int getExtraTerrainAttackPercent(int /*TerrainTypes*/ eIndex);
	int getExtraTerrainDefensePercent(int /*TerrainTypes*/ eIndex);
	int getExtraFeatureAttackPercent(int /*FeatureTypes*/ eIndex);
	int getExtraFeatureDefensePercent(int /*FeatureTypes*/ eIndex);
	int getExtraUnitCombatModifier(int /*UnitCombatTypes*/ eIndex);
	bool canAcquirePromotion(int /*PromotionTypes*/ ePromotion);
	bool isPromotionValid(int /*PromotionTypes*/ ePromotion);
	bool isPromotionOverriden(int /*PromotionTypes*/ ePromotion);
	bool isHasPromotion(int /*PromotionTypes*/ ePromotion);
	bool isHasUnitCombat(int /*UnitCombatTypes*/ eUnitCombat);
	void setHasUnitCombat(int /*UnitCombatTypes*/ eIndex, bool bNewValue);
	void setHasPromotion(int /*PromotionTypes*/ eIndex, bool bNewValue);

	int captureProbabilityTotal();
	int captureResistanceTotal();
	bool isHiddenNationality();
	void doHNCapture();
	CyUnit* getCapturingUnit() const;
	void setCapturingUnit(CyUnit* pCapturingUnit);

	int /*UnitAITypes*/ getUnitAIType();
	void setUnitAIType(int /*UnitAITypes*/ iNewValue);

	std::string getButton() const;

	bool isCommander() const;
	int controlPointsLeft() const;
	int controlPoints() const;
	float getRealExperience();

protected:
	CvUnit* m_pUnit;
};

DECLARE_PY_WRAPPER(CyUnit, CvUnit*);

#endif // CyUnit_h__
