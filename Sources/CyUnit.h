#pragma once

#ifndef CyUnit_h__
#define CyUnit_h__

//
// Python wrapper class for CvUnit
//

class CvUnit;
class CyArea;
class CyPlot;
class CySelectionGroup;

class CyUnit
{
public:
	DllExport explicit CyUnit(CvUnit* pUnit);		// Call from C++

	CvUnit* getUnit() const { return m_pUnit; };	// Call from C++
	const CvUnit* getUnitConst() const { return m_pUnit; };	// Call from C++

	void convert(const CyUnit& kUnit, bool bKillOriginal);
	void kill(bool bDelay, int /*PlayerTypes*/ ePlayer);
	void NotifyEntity(int /*MissionTypes*/ eMission);
	void doCommand(CommandTypes eCommand, int iData1, int iData2);
	bool isActionRecommended(int i) const;

	bool canEnterPlot(const CyPlot& kPlot, bool bAttack, bool bDeclareWar, bool bIgnoreLoad) const;
	bool isAutoUpgrading() const;
	bool isAutoPromoting() const;
	bool canHeal(const CyPlot& kPlot) const;
	bool IsSelected() const;
	bool canFound(const CyPlot& kPlot, bool bTestVisible) const;
	bool canConstruct(const CyPlot& kPlot, int /*BuildingTypes*/ eBuilding) const;

	int /*TechTypes*/ getDiscoveryTech() const;
	int getDiscoverResearch(int /*TechTypes*/ eTech) const;
	int getHurryProduction(const CyPlot& kPlot) const;
	bool canHurry(const CyPlot& kPlot, bool bTestVisible) const;
	bool canTrade(const CyPlot& kPlot, bool bTestVisible) const;
	int getGreatWorkCulture(const CyPlot& kPlot) const;
	int getEspionagePoints(const CyPlot& kPlot) const;

	bool canUpgrade(int /*UnitTypes*/ eUnit, bool bTestVisible) const;

	int /*HandicapTypes*/ getHandicapType() const;
	int /*CivilizationTypes*/ getCivilizationType() const;
	int /*SpecialUnitTypes*/ getSpecialUnitType() const;
	int /*UnitTypes*/ getCaptureUnitType() const;
	int /*UnitCombatTypes*/ getUnitCombatType() const;
	DomainTypes getDomainType() const;

	bool isBarbarian() const;
	bool isNPC() const;
	bool isHominid() const;
	bool isHuman() const;

	int baseMoves() const;
	int maxMoves() const;
	int movesLeft() const;
	bool canMove() const;
	bool hasMoved() const;
	int nukeRange() const;

	bool isAnimal() const;
	bool isOnlyDefensive() const;

	bool isFound() const;
	bool isGoldenAge() const;

	bool isFighting() const;
	int getMaxHP() const;
	int getHP() const;
	bool isHurt() const;
	bool isDead() const;
	void setBaseCombatStr(int iCombat);
	int baseCombatStr() const;

	bool canFight() const;

	int airBaseCombatStr() const;
	bool canAirAttack() const;

	bool isAutomated() const;
	bool isWaiting() const;
	bool isFortifyable() const;
	int experienceNeeded() const;
	bool isInvisible(int /*TeamTypes*/ eTeam, bool bDebug) const;
	bool isNukeImmune() const;

	int bombardRate() const;

	int /*SpecialUnitTypes*/ getSpecialCargo() const;
	int /*DomainTypes*/ getDomainCargo() const;
	int cargoSpace() const;
	void changeCargoSpace(int iChange);
	bool isFull() const;

	int getID() const;

	int getGroupID() const;
	bool isInGroup() const;
	bool isGroupHead() const;
	CySelectionGroup* getGroup() const;

	int getHotKeyNumber() const;

	int getX() const;
	int getY() const;
	void setXY(int iX, int iY, bool bGroup, bool bUpdate, bool bShow);
	CyPlot* plot() const;
	CyArea* area() const;

	int getDamage() const;
	void setDamage(int iNewValue, int /*PlayerTypes*/ ePlayer);
	void changeDamage(int iChange, int /*PlayerTypes*/ ePlayer);
	int getMoves() const;
	void setMoves(int iNewValue);
	void changeMoves(int iChange);
	void finishMoves();
	int getExperience() const;
	void setExperience(int iNewValue);
	void changeExperience(int iChange, int iMax, bool bFromCombat, bool bInBorders, bool bUpdateGlobal);
	int getLevel() const;
	void setLevel(int iNewLevel);
	int getFacingDirection() const;
	void rotateFacingDirectionClockwise();
	void rotateFacingDirectionCounterClockwise();
	int getCargo() const;
	int getFortifyTurns() const;
	void setFortifyTurns(int iNewValue);

	bool isRiver() const;

	int getExtraMoves() const;

	int getRevoltProtection() const;
	int getPillageChange() const;
	int getUpgradeDiscount() const;

	int getImmobileTimer() const;
	void setImmobileTimer(int iNewValue);

	bool isMadeAttack() const;
	void setMadeAttack(bool bNewValue);

	bool isMadeInterception() const;
	void setMadeInterception(bool bNewValue);

	bool isPromotionReady() const;
	void setPromotionReady(bool bNewValue);
	int getOwner() const;
	int getVisualOwner() const;
	int getTeam() const;

	int /*UnitTypes*/ getUnitType() const;
	int /*UnitTypes*/ getLeaderUnitType() const;
	void setLeaderUnitType(int /*UnitTypes*/ leaderUnitType);

	CyUnit* getTransportUnit() const;
	bool isCargo() const;
	void setTransportUnit(const CyUnit& kTransportUnit, const bool bLoad);

	std::wstring getName() const;
	std::wstring getNameForm(int iForm) const;
	std::wstring getNameKey() const;
	std::wstring getNameNoDesc() const;
	void setName(std::wstring szNewValue);
	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);

	bool canAcquirePromotion(int /*PromotionTypes*/ ePromotion) const;
	bool canAcquirePromotionAny() const;
	bool isPromotionValid(int /*PromotionTypes*/ ePromotion) const;
	bool isPromotionOverriden(int /*PromotionTypes*/ ePromotion) const;
	bool isHasPromotion(int /*PromotionTypes*/ ePromotion) const;
	bool isHasUnitCombat(int /*UnitCombatTypes*/ eUnitCombat) const;
	void setHasPromotion(int /*PromotionTypes*/ eIndex, bool bNewValue);

	int captureProbabilityTotal() const;
	int captureResistanceTotal() const;
	bool isHiddenNationality() const;
	void doHNCapture();

	int /*UnitAITypes*/ getUnitAIType() const;
	void setUnitAIType(int /*UnitAITypes*/ iNewValue);

	std::string getButton() const;

	void setCommander(bool bNewValue);
	bool isCommander() const;
	int controlPointsLeft() const;
	int controlPoints() const;
	float getRealExperience() const;

protected:
	CvUnit* m_pUnit;
};

DECLARE_PY_WRAPPER(CyUnit, CvUnit*);

#endif // CyUnit_h__
