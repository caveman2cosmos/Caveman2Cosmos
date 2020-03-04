#include "CvGameCoreDLL.h"

//# include <boost/python/manage_new_object.hpp>
//# include <boost/python/return_value_policy.hpp>

//
// published python interface for CyUnit
//

void CyUnitPythonInterface1(python::class_<CyUnit>& x)
{
	OutputDebugString("Python Extension Module - CyUnitPythonInterface1\n");

	x
		// < M.A.D. Nukes Start >
		.def("isMADEnabled", &CyUnit::isMADEnabled, "bool ()")
		.def("getMADTargetPlot", &CyUnit::getMADTargetPlot, python::return_value_policy<python::manage_new_object>(), "CyPlot* ()")
		.def("getMADTargetPlotX", &CyUnit::getMADTargetPlotX, "int ()")
		.def("getMADTargetPlotY", &CyUnit::getMADTargetPlotY, "int ()")
		.def("getMADTargetPlotOwner", &CyUnit::getMADTargetPlotOwner, "int ()")
		// < M.A.D. Nukes End   >
		.def("isNone", &CyUnit::isNone, "bool () - Is this a valid unit instance?")
		.def("convert", &CyUnit::convert, "void (CyUnit* pUnit)")
		.def("kill", &CyUnit::kill, "void (bool bDelay, int /*PlayerTypes*/ ePlayer)")
		.def("NotifyEntity", &CyUnit::NotifyEntity, "void (int EntityEventType)")
		
		.def("isActionRecommended", &CyUnit::isActionRecommended, "int (int i)")
		.def("isBetterDefenderThan", &CyUnit::isBetterDefenderThan, "bool (CyUnit* pDefender, CyUnit* pAttacker)")

		.def("canDoCommand", &CyUnit::canDoCommand, "bool (eCommand, iData1, iData2, bTestVisible = False) - can the unit perform eCommand?")
		.def("doCommand", &CyUnit::doCommand, "void (eCommand, iData1, iData2) - force the unit to perform eCommand")

		.def("getPathEndTurnPlot", &CyUnit::getPathEndTurnPlot, python::return_value_policy<python::manage_new_object>(), "CyPlot* ()")
		.def("generatePath", &CyUnit::generatePath, "bool (CyPlot* pToPlot, int iFlags = 0, bool bReuse = false, int* piPathTurns = NULL)")

		.def("canEnterTerritory", &CyUnit::canEnterTerritory, "bool (int (TeamTypes) eTeam, bool bIgnoreRightOfPassage)")
		.def("canEnterArea", &CyUnit::canEnterArea, "bool (int (TeamTypes) eTeam, CyArea* pArea, bool bIgnoreRightOfPassage)")
		.def("getDeclareWarMove", &CyUnit::getDeclareWarMove, "TeamTypes (CyPlot* pPlot)")
		.def("canMoveInto", &CyUnit::canMoveInto, "bool (CyPlot* pPlot, bool bAttack, bool bDeclareWar, bool bIgnoreLoad)")
		.def("canMoveOrAttackInto", &CyUnit::canMoveOrAttackInto, "bool (CyPlot* pPlot, bool bDeclareWar)")
		.def("canMoveThrough", &CyUnit::canMoveThrough, "bool (CyPlot* pPlot)")
		.def("jumpToNearestValidPlot", &CyUnit::jumpToNearestValidPlot, "bool ()")

		.def("canAutomate", &CyUnit::canAutomate, "bool (int (AutomateTypes) eAutomate)")
		.def("canScrap", &CyUnit::canScrap, "bool ()")
		.def("canGift", &CyUnit::canGift, "bool (bool bTestVisible)")
		/********************************************************************************/
		/**		REVOLUTION_MOD							1/1/08				jdog5000	*/
		/**																				*/
		/**		 																		*/
		/********************************************************************************/
		.def("gift", &CyUnit::gift, "void ()")
		/********************************************************************************/
		/**		REVOLUTION_MOD							END								*/
		/********************************************************************************/
		.def("canLoadUnit", &CyUnit::canLoadUnit, "bool (CyUnit* pUnit, CyPlot* pPlot)")
		.def("canLoad", &CyUnit::canLoad, "bool ()")
		.def("canUnload", &CyUnit::canUnload, "bool ()")
		.def("canUnloadAll", &CyUnit::canUnloadAll, "bool ()")
		.def("canHold", &CyUnit::canHold, "bool (CyPlot* pPlot)")
		.def("canSleep", &CyUnit::canSleep, "bool (CyPlot* pPlot)")
		.def("canFortify", &CyUnit::canFortify, "bool (CyPlot* pPlot)")
		.def("canPlunder", &CyUnit::canPlunder, "bool (CyPlot* pPlot)")
		.def("canAirPatrol", &CyUnit::canAirPatrol, "bool (CyPlot* pPlot)")
		.def("canSeaPatrol", &CyUnit::canSeaPatrol, "bool (CyPlot* pPlot)")
		.def("canHeal", &CyUnit::canHeal, "bool (CyPlot* pPlot)")
		.def("canSentry", &CyUnit::canSentry, "bool (CyPlot* pPlot)")

		.def("canAirlift", &CyUnit::canAirlift, "bool (CyPlot* pPlot)")
		.def("canAirliftAt", &CyUnit::canAirliftAt, "bool (CyPlot* pPlot, int iX, int iY)")

		.def("isNukeVictim", &CyUnit::isNukeVictim, "bool (CyPlot* pPlot, int eTeam)")
		.def("canNuke", &CyUnit::canNuke, "bool ()")
		.def("canNukeAt", &CyUnit::canNukeAt, "bool (CyPlot* pPlot, int iX, int iY)")

		.def("canRecon", &CyUnit::canRecon, "bool ()")
		.def("canReconAt", &CyUnit::canReconAt, "bool (CyPlot* pPlot, int iX, int iY)")

		.def("canParadrop", &CyUnit::canParadrop, "bool ()")
		.def("canParadropAt", &CyUnit::canParadropAt, "bool (CyPlot* pPlot, int iX, int iY)")

		.def("canAirBomb", &CyUnit::canAirBomb, "bool ()")
		.def("canAirBombAt", &CyUnit::canAirBombAt, "bool (CyPlot* pPlot, int iX, int iY)")

		.def("bombardTarget", &CyUnit::bombardTarget, python::return_value_policy<python::manage_new_object>(), "CyCity* (CyPlot* pPlot)")
		.def("canBombard", &CyUnit::canBombard, "bool (CyPlot* pPlot)")

		.def("canPillage", &CyUnit::canPillage, "bool (CyPlot* pPlot)")
		//TSHEEP Assassin Mission
		.def("canAssassin", &CyUnit::canAssassin, "bool (CyPlot* pPlot, bool bTestVisible)")
		//.def("assassinCost", &CyUnit::assassinCost, "int (CyUnit* pUnit)")
		//.def("assassinProb", &CyUnit::assassinProb, "int (CyUnit* pUnit)")
		//RevolutionDCM start
		.def("canBribe", &CyUnit::canBribe, "bool (CyPlot* pPlot, bool bTestVisible)")
		//RevolutionDCM end
		//TSHEEP End
		.def("sabotageCost", &CyUnit::sabotageCost, "int (CyPlot* pPlot)")
		.def("sabotageProb", &CyUnit::sabotageProb, "int (CyPlot* pPlot, int /*ProbabilityTypes*/ eProbStyle)")
		.def("canSabotage", &CyUnit::canSabotage, "bool (CyPlot* pPlot, bool bTestVisible)")
		.def("destroyCost", &CyUnit::destroyCost, "int (CyPlot* pPlot)")
		.def("destroyProb", &CyUnit::destroyProb, "int (CyPlot* pPlot, int /*ProbabilityTypes*/ eProbStyle)")
		.def("canDestroy", &CyUnit::canDestroy, "bool (CyPlot* pPlot, bool bTestVisible)")
		.def("stealPlansCost", &CyUnit::stealPlansCost, "int (CyPlot* pPlot)")
		.def("stealPlansProb", &CyUnit::stealPlansProb, "int (CyPlot* pPlot, int /*ProbabilityTypes*/ eProbStyle)")
		.def("canStealPlans", &CyUnit::canStealPlans, "bool (CyPlot* pPlot, bool bTestVisible)")

		.def("canFound", &CyUnit::canFound, "bool (CyPlot* pPlot, bool bTestVisible)")
		.def("canSpread", &CyUnit::canSpread, "bool (CyPlot* pPlot, int /*ReligionTypes*/ eReligion, bool bTestVisible)")
		.def("canJoin", &CyUnit::canJoin, "bool (CyPlot* pPlot, int (SpecialistTypes) eSpecialist)")
		.def("canConstruct", &CyUnit::canConstruct, "bool (CyPlot* pPlot, int (BuildingTypes) eBuilding)")

		.def("getDiscoveryTech", &CyUnit::getDiscoveryTech, "int /*TechTypes*/ ()")
		.def("getDiscoverResearch", &CyUnit::getDiscoverResearch, "int (int /*TechTypes*/ eTech)")
		.def("canDiscover", &CyUnit::canDiscover, "bool ()")
		.def("getMaxHurryProduction", &CyUnit::getMaxHurryProduction, "int (CyCity* pCity)")
		.def("getHurryProduction", &CyUnit::getHurryProduction, "int (CyPlot* pPlot)")
		.def("canHurry", &CyUnit::canHurry, "bool (CyPlot* pPlot, bool bTestVisible)")
		.def("getTradeGold", &CyUnit::getTradeGold, "int (CyPlot* pPlot)")
		.def("canTrade", &CyUnit::canTrade, "bool (CyPlot* pPlot, bool bTestVisible)")
		.def("getGreatWorkCulture", &CyUnit::getGreatWorkCulture, "int (CyPlot* pPlot)")
		.def("canGreatWork", &CyUnit::canGreatWork, "bool (CyPlot* pPlot)")
		.def("getEspionagePoints", &CyUnit::getEspionagePoints, "int (CyPlot* pPlot)")
		.def("canInfiltrate", &CyUnit::canInfiltrate, "bool (CyPlot* pPlot, bool bTestVisible)")
		.def("canEspionage", &CyUnit::canEspionage, "bool (CyPlot* pPlot)")

		.def("canGoldenAge", &CyUnit::canGoldenAge, "bool (CyPlot* pPlot, bool bTestVisible)")
		.def("canBuild", &CyUnit::canBuild, "bool (CyPlot* pPlot, int (BuildTypes) eBuild, bool bTestVisible)")
		.def("canLead", &CyUnit::canLead, "int (CyPlot* pPlot, int)")
		.def("lead", &CyUnit::lead, "bool (int)")
		.def("canGiveExperience", &CyUnit::canGiveExperience, "int (CyPlot* pPlot)")
		.def("giveExperience", &CyUnit::giveExperience, "bool ()")

		.def("canPromote", &CyUnit::canPromote, "bool (ePromotion, iLeaderUnitId)")
		.def("promote", &CyUnit::promote, "bool (ePromotion)")

		.def("upgradePrice", &CyUnit::upgradePrice, "int (UnitTypes eUnit)")
		.def("upgradeAvailable", &CyUnit::upgradeAvailable, "bool (int /*UnitTypes*/ eFromUnit, int /*UnitTypes*/ eToUnit, int iCount)")
		.def("canUpgrade", &CyUnit::canUpgrade, "bool (int /*UnitTypes*/ eUnit, bool bTestVisible)")
		.def("hasUpgrade", &CyUnit::hasUpgrade, "bool (bool bSearch)")

		.def("getHandicapType", &CyUnit::getHandicapType, "int ()")
		.def("getCivilizationType", &CyUnit::getCivilizationType, "int ()")
		.def("getSpecialUnitType", &CyUnit::getSpecialUnitType, "int ()")
		.def("getCaptureUnitType", &CyUnit::getCaptureUnitType, "int ()")
		.def("getUnitCombatType", &CyUnit::getUnitCombatType, "int ()")
		.def("getDomainType", &CyUnit::getDomainType, "int ()")
		.def("getInvisibleType", &CyUnit::getInvisibleType, "int (InvisibleTypes) ()")
		.def("getNumSeeInvisibleTypes", &CyUnit::getNumSeeInvisibleTypes, "int ()")
		.def("getSeeInvisibleType", &CyUnit::getSeeInvisibleType, "int (InvisibleTypes) (int)")

		.def("flavorValue", &CyUnit::flavorValue, "int (FlavorTypes eFlavor)")
		.def("isBarbarian", &CyUnit::isBarbarian, "bool ()")
		.def("isNPC", &CyUnit::isNPC, "bool ()")
		.def("isHominid", &CyUnit::isHominid, "bool ()")
		.def("isHuman", &CyUnit::isHuman, "bool ()")
		.def("visibilityRange", &CyUnit::visibilityRange, "int ()")
		.def("baseMoves", &CyUnit::baseMoves, "int ()")
		.def("movesLeft", &CyUnit::movesLeft, "int ()")

		.def("maxMoves", &CyUnit::maxMoves, "int ()")
		.def("canMove", &CyUnit::canMove, "bool ()")
		.def("hasMoved", &CyUnit::hasMoved, "bool ()")
		.def("airRange", &CyUnit::airRange, "int ()")
		.def("nukeRange", &CyUnit::nukeRange, "int ()")

		.def("canBuildRoute", &CyUnit::canBuildRoute, "bool ()")
		.def("getBuildType", &CyUnit::getBuildType, "int (BuildTypes) ()")
		.def("workRate", &CyUnit::workRate, "int (bool bMax)")
		.def("isAnimal", &CyUnit::isAnimal, "bool ()")
		.def("isNoBadGoodies", &CyUnit::isNoBadGoodies, "bool ()")
		.def("isOnlyDefensive", &CyUnit::isOnlyDefensive, "bool ()")
		.def("isNoCapture", &CyUnit::isNoCapture, "bool ()")
		.def("isRivalTerritory", &CyUnit::isRivalTerritory, "bool ()")
		.def("isMilitaryHappiness", &CyUnit::isMilitaryHappiness, "bool ()")
		.def("isInvestigate", &CyUnit::isInvestigate, "bool ()")
		.def("isCounterSpy", &CyUnit::isCounterSpy, "bool ()")
		.def("isFound", &CyUnit::isFound, "bool ()")
		/********************************************************************************/
		/**		REVOLUTION_MOD							1/1/08				DPII		*/
		/**																				*/
		/**		 																		*/
		/********************************************************************************/
		/*
		.def("isCanBeRebel", &CyUnit::isCanBeRebel, "bool ()")
		.def("isCanRebelCapture", &CyUnit::isCanRebelCapture, "bool ()")
		.def("isCannotDefect", &CyUnit::isCannotDefect, "bool ()")
		.def("isCanQuellRebellion", &CyUnit::isCanQuellRebellion, "bool ()")
		*/
		/********************************************************************************/
		/**		REVOLUTION_MOD							END								*/
		/********************************************************************************/
		.def("isGoldenAge", &CyUnit::isGoldenAge, "bool ()")
		.def("canCoexistWithEnemyUnit", &CyUnit::canCoexistWithEnemyUnit, "bool (int)")

		.def("isFighting", &CyUnit::isFighting, "bool ()")
		.def("isAttacking", &CyUnit::isAttacking, "bool ()")
		.def("isDefending", &CyUnit::isDefending, "bool ()")
		.def("isCombat", &CyUnit::isCombat, "bool ()")
		.def("maxHitPoints", &CyUnit::maxHitPoints, "int ()")
		.def("currHitPoints", &CyUnit::currHitPoints, "int ()")
		.def("isHurt", &CyUnit::isHurt, "bool ()")
		.def("isDead", &CyUnit::isDead, "bool ()")
		.def("setBaseCombatStr", &CyUnit::setBaseCombatStr, "void (int)")
		.def("baseCombatStr", &CyUnit::baseCombatStr, "int ()")
		.def("fbaseCombatStr", &CyUnit::fbaseCombatStr, "float ()")
		.def("maxCombatStr", &CyUnit::maxCombatStr, "int (CyPlot* pPlot, CyUnit* pAttacker)")
		.def("currCombatStr", &CyUnit::currCombatStr, "int (CyPlot* pPlot, CyUnit* pAttacker)")
		.def("currFirepower", &CyUnit::currFirepower, "int (CyPlot* pPlot, CyUnit* pAttacker)")
		.def("maxCombatStrFloat", &CyUnit::maxCombatStrFloat, "float (CyPlot* pPlot, CyUnit* pAttacker)")
		.def("currCombatStrFloat", &CyUnit::currCombatStrFloat, "float (CyPlot* pPlot, CyUnit* pAttacker)")

		.def("canFight", &CyUnit::canFight, "bool ()")
		.def("canAttack", &CyUnit::canAttack, "bool ()")
		.def("canDefend", &CyUnit::canDefend, "bool (CyPlot* pPlot)")
		.def("canSiege", &CyUnit::canSiege, "bool (int /*TeamTypes*/ eTeam)")

		.def("airBaseCombatStr", &CyUnit::airBaseCombatStr, "int ()")
		.def("fairBaseCombatStr", &CyUnit::fairBaseCombatStr, "float ()")
		.def("airMaxCombatStr", &CyUnit::airMaxCombatStr, "int (CyUnit* pOther)")
		.def("airCurrCombatStr", &CyUnit::airCurrCombatStr, "int (CyUnit* pOther)")
		.def("airMaxCombatStrFloat", &CyUnit::airMaxCombatStrFloat, "float (CyUnit* pOther)")
		.def("airCurrCombatStrFloat", &CyUnit::airCurrCombatStrFloat, "float (CyUnit* pOther)")
		.def("combatLimit", &CyUnit::combatLimit, "int ()")
		.def("airCombatLimit", &CyUnit::airCombatLimit, "int ()")
		.def("canAirAttack", &CyUnit::canAirAttack, "bool ()")
		.def("canAirDefend", &CyUnit::canAirDefend, "bool (CyPlot*)")
		.def("airCombatDamage", &CyUnit::airCombatDamage, "int (CyUnit* pDefender)")
		.def("bestInterceptor", &CyUnit::bestInterceptor, python::return_value_policy<python::manage_new_object>(), "CyUnit* (CyPlot*)")

		.def("isAutomated", &CyUnit::isAutomated, "bool ()")
/************************************************************************************************/
/* RevDCM	                  Start		 21/12/2010                                             */
/*                                                                                              */
/* Advanced Automations                                                                         */
/************************************************************************************************/
		.def("isAutoPromoting", &CyUnit::isAutoPromoting, "bool ()")
		.def("isAutoUpgrading", &CyUnit::isAutoUpgrading, "bool ()")
/************************************************************************************************/
/* Advanced Automations		end                                                                 */
/************************************************************************************************/
		.def("isWaiting", &CyUnit::isWaiting, "bool ()")
		.def("isFortifyable", &CyUnit::isFortifyable, "bool ()")
		.def("fortifyModifier", &CyUnit::fortifyModifier, "int ()")
		//TB Combat Mods begin
		.def("fortifyRepelModifier", &CyUnit::fortifyRepelModifier, "int ()")
		//TB Combat Mods end
		.def("experienceNeeded", &CyUnit::experienceNeeded, "int ()") 
		.def("attackXPValue", &CyUnit::attackXPValue, "int ()") 
		.def("defenseXPValue", &CyUnit::defenseXPValue, "int ()")
		.def("firstStrikes", &CyUnit::firstStrikes, "int ()")
		.def("chanceFirstStrikes", &CyUnit::chanceFirstStrikes, "int ()")
		.def("maxFirstStrikes", &CyUnit::maxFirstStrikes, "int ()")
		.def("isRanged", &CyUnit::isRanged, "bool ()")
		.def("alwaysInvisible", &CyUnit::alwaysInvisible, "bool ()")

		.def("immuneToFirstStrikes", &CyUnit::immuneToFirstStrikes, "bool ()")
		.def("noDefensiveBonus", &CyUnit::noDefensiveBonus, "bool ()")
		.def("ignoreBuildingDefense", &CyUnit::ignoreBuildingDefense, "bool ()")
		.def("canMoveImpassable", &CyUnit::canMoveImpassable, "bool ()")
		.def("canMoveAllTerrain", &CyUnit::canMoveAllTerrain, "bool ()")
		.def("flatMovementCost", &CyUnit::flatMovementCost, "bool ()")
		.def("ignoreTerrainCost", &CyUnit::ignoreTerrainCost, "bool ()")
		.def("isNeverInvisible", &CyUnit::isNeverInvisible, "bool ()")
		.def("isInvisible", &CyUnit::isInvisible, "bool (int (TeamTypes) eTeam, bool bDebug)")
		.def("isNukeImmune", &CyUnit::isNukeImmune, "bool ()")
/************************************************************************************************/
/* REVDCM_OC                              02/16/10                                phungus420    */
/*                                                                                              */
/* Inquisitions                                                                                 */
/************************************************************************************************/
		.def("isInquisitor", &CyUnit::isInquisitor, "bool ()")
/************************************************************************************************/
/* REVDCM_OC                               END                                                  */
/************************************************************************************************/

		.def("maxInterceptionProbability", &CyUnit::maxInterceptionProbability, "int ()")
		.def("currInterceptionProbability", &CyUnit::currInterceptionProbability, "int ()")
		.def("evasionProbability", &CyUnit::evasionProbability, "int ()")
		.def("withdrawalProbability", &CyUnit::withdrawalProbability, "int ()")
		//TB Combat Mods Begin
		.def("attackCombatModifierTotal()", &CyUnit::attackCombatModifierTotal, "int ()")
		.def("defenseCombatModifierTotal()", &CyUnit::defenseCombatModifierTotal, "int ()")
		.def("pursuitProbability()", &CyUnit::pursuitProbability, "int ()")
		.def("earlyWithdrawTotal()", &CyUnit::earlyWithdrawTotal, "int ()")
		.def("vsBarbsModifier()", &CyUnit::vsBarbsModifier, "int ()")
		.def("armorTotal()", &CyUnit::armorTotal, "int ()")
		.def("punctureTotal()", &CyUnit::punctureTotal, "int ()")
		.def("damageModifierTotal()", &CyUnit::damageModifierTotal, "int ()")
		.def("overrunTotal()", &CyUnit::overrunTotal, "int ()")
		.def("repelTotal()", &CyUnit::repelTotal, "int ()")
		.def("fortRepelTotal()", &CyUnit::fortRepelTotal, "int ()")
		.def("repelRetriesTotal()", &CyUnit::repelRetriesTotal, "int ()")
		.def("unyieldingTotal()", &CyUnit::unyieldingTotal, "int ()")
		.def("knockbackTotal()", &CyUnit::knockbackTotal, "int ()")
		.def("knockbackRetriesTotal()", &CyUnit::knockbackRetriesTotal, "int ()")
		.def("canStampede()", &CyUnit::canStampede, "bool ()")
		.def("strAdjperRndTotal()", &CyUnit::strAdjperRndTotal, "int ()")
		.def("strAdjperAttTotal()", &CyUnit::strAdjperAttTotal, "int ()")
		.def("strAdjperDefTotal()", &CyUnit::strAdjperDefTotal, "int ()")
		.def("withdrawAdjperAttTotal()", &CyUnit::withdrawAdjperAttTotal, "int ()")
		.def("currentStrAdjperRndTotal()", &CyUnit::currentStrAdjperRndTotal, "int ()")
		.def("currentStrAdjperAttTotal()", &CyUnit::currentStrAdjperAttTotal, "int ()")
		.def("currentStrAdjperDefTotal()", &CyUnit::currentStrAdjperDefTotal, "int ()")
		.def("currentWithdrawAdjperAttTotal()", &CyUnit::currentWithdrawAdjperAttTotal, "int ()")
		.def("unnerveTotal()", &CyUnit::unnerveTotal, "int ()")
		.def("encloseTotal()", &CyUnit::encloseTotal, "int ()")
		.def("lungeTotal()", &CyUnit::lungeTotal, "int ()")
		.def("dynamicDefenseTotal()", &CyUnit::dynamicDefenseTotal, "int ()")
		.def("canAnimalIgnoresBorders()", &CyUnit::canAnimalIgnoresBorders, "bool ()")
		.def("canOnslaught()", &CyUnit::canOnslaught, "bool ()")
		.def("hasSubCombatType()", &CyUnit::hasSubCombatType, "bool ()")
		.def("hasCureAfflictionType()", &CyUnit::hasCureAfflictionType, "bool ()")
		.def("fortitudeTotal()", &CyUnit::fortitudeTotal, "int ()")
		//.def("aidTotal()", &CyUnit::aidTotal, "int ()")
		//TB Combat Mods End
		.def("collateralDamage", &CyUnit::collateralDamage, "int ()")
		.def("collateralDamageLimit", &CyUnit::collateralDamageLimit, "int ()")
		.def("collateralDamageMaxUnits", &CyUnit::collateralDamageMaxUnits, "int ()")

		.def("cityAttackModifier", &CyUnit::cityAttackModifier, "int ()")
		.def("cityDefenseModifier", &CyUnit::cityDefenseModifier, "int ()")
		.def("animalCombatModifier", &CyUnit::animalCombatModifier, "int ()")
		.def("hillsAttackModifier", &CyUnit::hillsAttackModifier, "int ()")
		.def("hillsDefenseModifier", &CyUnit::hillsDefenseModifier, "int ()")
		.def("terrainAttackModifier", &CyUnit::terrainAttackModifier, "int (int /*TerrainTypes*/ eTerrain)")
		.def("terrainDefenseModifier", &CyUnit::terrainDefenseModifier, "int (int /*TerrainTypes*/ eTerrain)")
		.def("featureAttackModifier", &CyUnit::featureAttackModifier, "int (int /*FeatureTypes*/ eFeature)")
		.def("featureDefenseModifier", &CyUnit::featureDefenseModifier, "int (int /*FeatureTypes*/ eFeature)")
		.def("unitAttackModifier", &CyUnit::unitAttackModifier, "int (int /*UnitTypes*/ eUnit)")
		.def("unitDefenseModifier", &CyUnit::unitDefenseModifier, "int (int /*UnitTypes*/ eUnit)")
		.def("unitCombatModifier", &CyUnit::unitCombatModifier, "int (int /*UnitCombatTypes*/ eUnitCombat)")
		.def("domainModifier", &CyUnit::domainModifier, "int (int /*DomainTypes*/ eDomain)") 

		.def("bombardRate", &CyUnit::bombardRate, "int ()") 
		.def("airBombBaseRate", &CyUnit::airBombBaseRate, "int ()")
		.def("airBombCurrRate", &CyUnit::airBombCurrRate, "int ()")

		.def("specialCargo", &CyUnit::specialCargo, "int ()") 
		.def("domainCargo", &CyUnit::domainCargo, "int ()") 
		.def("cargoSpace", &CyUnit::cargoSpace, "int ()")
		.def("changeCargoSpace", &CyUnit::changeCargoSpace, "void (int)")
		.def("isFull", &CyUnit::isFull, "bool ()")
		.def("cargoSpaceAvailable", &CyUnit::cargoSpaceAvailable, "int ()")
		.def("hasCargo", &CyUnit::hasCargo, "bool ()")
		.def("canCargoAllMove", &CyUnit::canCargoAllMove, "bool ()")
		.def("getUnitAICargo", &CyUnit::getUnitAICargo, "int (int (UnitAITypes) eUnitAI)")
		.def("getID", &CyUnit::getID, "int ()")

		.def("getGroupID", &CyUnit::getGroupID, "int ()")
		.def("isInGroup", &CyUnit::isInGroup, "bool ()")
		.def("isGroupHead", &CyUnit::isGroupHead, "bool ()")
		.def("getGroup", &CyUnit::getGroup, python::return_value_policy<python::manage_new_object>(), "CySelectionGroup* ()")

		.def("getHotKeyNumber", &CyUnit::getHotKeyNumber, "int () - returns the HotKey number for this unit")
		.def("setHotKeyNumber", &CyUnit::setHotKeyNumber, "void (int iNewValue)")

		.def("getX", &CyUnit::getX, "int ()")
		.def("getY", &CyUnit::getY, "int ()")
		.def("setXY", &CyUnit::setXY, "int (int iX, int iY)")
		.def("at", &CyUnit::at, "bool (int iX, int iY)")
		.def("atPlot", &CyUnit::atPlot, "bool (CyPlot* pPlot)")
		.def("plot", &CyUnit::plot, python::return_value_policy<python::manage_new_object>(), "CyPlot* ()")
		.def("area", &CyUnit::area, python::return_value_policy<python::manage_new_object>(), "CyArea* ()")
		.def("getReconPlot", &CyUnit::getReconPlot, python::return_value_policy<python::manage_new_object>(), "CyPlot* ()")
		.def("setReconPlot", &CyUnit::setReconPlot, "void (CyPlot)")				 

		.def("getGameTurnCreated", &CyUnit::getGameTurnCreated, "int ()")

		.def("getDamage", &CyUnit::getDamage, "int ()")
		.def("setDamage", &CyUnit::setDamage, "void (int iNewValue, int /*PlayerTypes*/ ePlayer)")
		.def("changeDamage", &CyUnit::changeDamage, "void (int iChange, int /*PlayerTypes*/ ePlayer)")
		.def("getMoves", &CyUnit::getMoves, "int ()")
		.def("setMoves", &CyUnit::setMoves, "void (int iNewValue)")
		.def("changeMoves", &CyUnit::changeMoves, "void (int iChange)")
		.def("finishMoves", &CyUnit::finishMoves, "void ()")
		.def("getExperience", &CyUnit::getExperience, "int ()")
		.def("setExperience", &CyUnit::setExperience, "void (int iNewValue)")
		.def("changeExperience", &CyUnit::changeExperience, "void (int iChange, int iMax, bool bFromCombat, bool bInBorders, bool bUpdateGlobal)")
		.def("getLevel", &CyUnit::getLevel, "int ()")
		.def("setLevel", &CyUnit::setLevel)
		.def("changeLevel", &CyUnit::changeLevel)
		.def("getFacingDirection", &CyUnit::getFacingDirection, "int ()")
		.def("rotateFacingDirectionClockwise", &CyUnit::rotateFacingDirectionClockwise, "void ()")
		.def("rotateFacingDirectionCounterClockwise", &CyUnit::rotateFacingDirectionCounterClockwise, "void ()")
		.def("getCargo", &CyUnit::getCargo, "int ()")
		.def("getFortifyTurns", &CyUnit::getFortifyTurns, "int ()")
		/********************************************************************************/
		/**		REVOLUTION_MOD							5/27/08				jdog5000	*/
		/**																				*/
		/**																				*/
		/********************************************************************************/
		.def("setFortifyTurns", &CyUnit::setFortifyTurns, "void (int iNewValue)")
		/********************************************************************************/
		/**		REVOLUTION_MOD							END								*/
		/********************************************************************************/
		.def("getBlitzCount", &CyUnit::getBlitzCount, "int ()")
		.def("isBlitz", &CyUnit::isBlitz, "bool ()")
		.def("getAmphibCount", &CyUnit::getAmphibCount, "int ()")
		.def("isAmphib", &CyUnit::isAmphib, "bool ()")
		.def("getRiverCount", &CyUnit::getRiverCount, "int ()")
		.def("isRiver", &CyUnit::isRiver, "bool ()")
		.def("isEnemyRoute", &CyUnit::isEnemyRoute, "bool ()")
		.def("isAlwaysHeal", &CyUnit::isAlwaysHeal, "bool ()")
		.def("isHillsDoubleMove", &CyUnit::isHillsDoubleMove, "bool ()")

		.def("getExtraVisibilityRange", &CyUnit::getExtraVisibilityRange, "int ()")
		.def("getExtraMoves", &CyUnit::getExtraMoves, "int ()")
		.def("getExtraMoveDiscount", &CyUnit::getExtraMoveDiscount, "int ()")
		.def("getExtraAirRange", &CyUnit::getExtraAirRange, "int ()")
		.def("getExtraIntercept", &CyUnit::getExtraIntercept, "int ()")
		.def("getExtraEvasion", &CyUnit::getExtraEvasion, "int ()")
		.def("getExtraFirstStrikes", &CyUnit::getExtraFirstStrikes, "int ()")
		.def("getExtraChanceFirstStrikes", &CyUnit::getExtraChanceFirstStrikes, "int ()")
		.def("getExtraWithdrawal", &CyUnit::getExtraWithdrawal, "int ()")
		//TB Combat Mods begin
		.def("getExtraPursuit", &CyUnit::getExtraPursuit, "int ()")
		.def("getExtraEarlyWithdraw", &CyUnit::getExtraEarlyWithdraw, "int ()")
		.def("getExtraVSBarbs", &CyUnit::getExtraVSBarbs, "int ()")
		.def("getExtraArmor", &CyUnit::getExtraArmor, "int ()")
		.def("getExtraPuncture", &CyUnit::getExtraPuncture, "int ()")
		.def("getExtraDamageModifier", &CyUnit::getExtraDamageModifier, "int ()")
		.def("getExtraOverrun", &CyUnit::getExtraOverrun, "int ()")
		.def("getExtraRepel", &CyUnit::getExtraRepel, "int ()")
		.def("getExtraFortRepel", &CyUnit::getExtraFortRepel, "int ()")
		.def("getExtraRepelRetries", &CyUnit::getExtraRepelRetries, "int ()")
		.def("getExtraUnyielding", &CyUnit::getExtraUnyielding, "int ()")
		.def("getExtraKnockback", &CyUnit::getExtraKnockback, "int ()")
		.def("getExtraKnockbackRetries", &CyUnit::getExtraKnockbackRetries, "int ()")
		.def("mayStampede", &CyUnit::mayStampede, "bool ()")
		.def("getExtraStrAdjperRnd", &CyUnit::getExtraStrAdjperRnd, "int ()")
		.def("getExtraStrAdjperAtt", &CyUnit::getExtraStrAdjperAtt, "int ()")
		.def("getExtraStrAdjperDef", &CyUnit::getExtraStrAdjperDef, "int ()")
		.def("getExtraWithdrawAdjperAtt", &CyUnit::getExtraWithdrawAdjperAtt, "int ()")
		.def("getExtraUnnerve", &CyUnit::getExtraUnnerve, "int ()")
		.def("getExtraEnclose", &CyUnit::getExtraEnclose, "int ()")
		.def("getExtraLunge", &CyUnit::getExtraLunge, "int ()")
		.def("getExtraDynamicDefense", &CyUnit::getExtraDynamicDefense, "int ()")
		.def("getExtraStrength", &CyUnit::getExtraStrength, "int ()")
		.def("mayAnimalIgnoresBorders", &CyUnit::mayAnimalIgnoresBorders, "bool ()")
		.def("mayOnslaught", &CyUnit::mayOnslaught, "bool ()")
		.def("hasExtraSubCombatType", &CyUnit::hasExtraSubCombatType, "bool (eCombatType)")
		.def("hasRemovesUnitCombatType", &CyUnit::hasRemovesUnitCombatType, "bool (eCombatType)")
		.def("hasExtraCureAffliction", &CyUnit::hasExtraCureAffliction, "bool (ePromotionLineType)")
		.def("getExtraFortitude", &CyUnit::getExtraFortitude, "int ()")
		//TB Combat Mods end
		.def("getExtraCollateralDamage", &CyUnit::getExtraCollateralDamage, "int ()")
		.def("getExtraEnemyHeal", &CyUnit::getExtraEnemyHeal, "int ()")
		.def("getExtraNeutralHeal", &CyUnit::getExtraNeutralHeal, "int ()")
		.def("getExtraFriendlyHeal", &CyUnit::getExtraFriendlyHeal, "int ()")

		.def("getSameTileHeal", &CyUnit::getSameTileHeal, "int ()")
		.def("getAdjacentTileHeal", &CyUnit::getAdjacentTileHeal, "int ()")

		.def("getExtraCombatPercent", &CyUnit::getExtraCombatPercent, "int ()")
		.def("getExtraCityAttackPercent", &CyUnit::getExtraCityAttackPercent, "int ()")
		.def("getExtraCityDefensePercent", &CyUnit::getExtraCityDefensePercent, "int ()")
		.def("getExtraHillsAttackPercent", &CyUnit::getExtraHillsAttackPercent, "int ()")
		.def("getExtraHillsDefensePercent", &CyUnit::getExtraHillsDefensePercent, "int ()")
		.def("getRevoltProtection", &CyUnit::getRevoltProtection, "int ()")
		.def("getCollateralDamageProtection", &CyUnit::getCollateralDamageProtection, "int ()")
		.def("getPillageChange", &CyUnit::getPillageChange, "int ()")
		.def("getUpgradeDiscount", &CyUnit::getUpgradeDiscount, "int ()")
		.def("getExperiencePercent", &CyUnit::getExperiencePercent, "int ()")
		.def("getKamikazePercent", &CyUnit::getKamikazePercent, "int ()")
		.def("getImmobileTimer", &CyUnit::getImmobileTimer, "int ()")
		.def("setImmobileTimer", &CyUnit::setImmobileTimer, "void (int)")

		.def("isMadeAttack", &CyUnit::isMadeAttack, "bool ()")
		.def("setMadeAttack", &CyUnit::setMadeAttack, "void (int iNewValue)")
		//TB Combat Mod begin
		.def("getRoundCount", &CyUnit::getRoundCount, "int ()")
		.def("changeRoundCount", &CyUnit::changeRoundCount, "void (int iChange)")
		.def("getAttackCount", &CyUnit::getAttackCount, "int ()")
		.def("changeAttackCount", &CyUnit::changeRoundCount, "void (int iChange)")
		.def("getDefenseCount", &CyUnit::getDefenseCount, "int ()")
		.def("changeDefenseCount", &CyUnit::changeDefenseCount, "void (int iChange)")
		//TB Combat Mod end
		.def("isMadeInterception", &CyUnit::isMadeInterception, "bool ()")
		.def("setMadeInterception", &CyUnit::setMadeInterception, "void (int iNewValue)")

		.def("isPromotionReady", &CyUnit::isPromotionReady, "bool ()")
		.def("setPromotionReady", &CyUnit::setPromotionReady, "void (int iNewValue)")
		.def("getOwner", &CyUnit::getOwner, "int ()")
		.def("getVisualOwner", &CyUnit::getVisualOwner, "int ()")
		.def("getCombatOwner", &CyUnit::getCombatOwner, "int (int)")
		.def("getTeam", &CyUnit::getTeam, "int ()")

		.def("getUnitType", &CyUnit::getUnitType, "int ()")
		.def("getLeaderUnitType", &CyUnit::getLeaderUnitType, "int ()")
		.def("setLeaderUnitType", &CyUnit::setLeaderUnitType, "void (int iNewValue)")

		.def("getTransportUnit", &CyUnit::getTransportUnit, python::return_value_policy<python::manage_new_object>(), "CyUnit* ()")
		.def("isCargo", &CyUnit::isCargo, "bool ()")
		.def("setTransportUnit", &CyUnit::setTransportUnit, "void (CyUnit* pTransportUnit)")

		.def("getExtraDomainModifier", &CyUnit::getExtraDomainModifier, "int ()")

		.def("getName", &CyUnit::getName, "str () - Returns the name of a unit along with its type description in parens if using a custom name")
		.def("getNameForm", &CyUnit::getNameForm, "str (int iForm)")
		.def("getNameKey", &CyUnit::getNameKey, "str ()")
		.def("getNameNoDesc", &CyUnit::getNameNoDesc, "str () - Returns the name of a unit without any description afterwards")
		.def("setName", &CyUnit::setName, "void (str)")
		.def("getScriptData", &CyUnit::getScriptData, "str ()")
		.def("setScriptData", &CyUnit::setScriptData, "void (str)")

		.def("isTerrainDoubleMove", &CyUnit::isTerrainDoubleMove, "bool (TerrainType)")
		.def("isFeatureDoubleMove", &CyUnit::isFeatureDoubleMove, "bool (FeatureType)")

		.def("getExtraTerrainAttackPercent", &CyUnit::getExtraTerrainAttackPercent, "int ()")
		.def("getExtraTerrainDefensePercent", &CyUnit::getExtraTerrainDefensePercent, "int ()")
		.def("getExtraFeatureAttackPercent", &CyUnit::getExtraFeatureAttackPercent, "int ()")
		.def("getExtraFeatureDefensePercent", &CyUnit::getExtraFeatureDefensePercent, "int ()")
		.def("getExtraUnitCombatModifier", &CyUnit::getExtraUnitCombatModifier, "int ()")

		.def("canAcquirePromotion", &CyUnit::canAcquirePromotion, "bool (int /*PromotionTypes*/ ePromotion)")
		.def("canAcquirePromotionAny", &CyUnit::canAcquirePromotionAny, "bool ()")
		.def("isPromotionValid", &CyUnit::isPromotionValid, "bool (int /*PromotionTypes*/ ePromotion)")
		.def("isPromotionOverriden", &CyUnit::isPromotionOverriden, "bool (int /*PromotionTypes*/ ePromotion)")
		.def("isHasPromotion", &CyUnit::isHasPromotion, "bool (int /*PromotionTypes*/ ePromotion)")
		.def("isHasUnitCombat", &CyUnit::isHasUnitCombat, "bool (int /*UnitCombatTypes*/ eUnitCombat)")
		.def("setHasPromotion", &CyUnit::setHasPromotion, "void (int (PromotionTypes) eIndex, bool bNewValue)")
		.def("IsSelected", &CyUnit::IsSelected)

		.def("getUnitAIType", &CyUnit::getUnitAIType, "int UnitAIType () - returns the int value of the UnitAIType")
		.def("setUnitAIType", &CyUnit::setUnitAIType, "void UnitAIType (int iUnitAIType) - sets the unit's UnitAIType")

		// Python Helper Functions
		.def("centerCamera", &CyUnit::centerCamera, "void () - Centers the Camera on the unit")
		.def("attackForDamage", &CyUnit::attackForDamage, "void attackForDamage(CyUnit *defender, int attakerDamageChange, int defenderDamageChange)")
		.def("rangeStrike", &CyUnit::rangeStrike, "void rangeStrike(int iX, int iY)")

		.def("getArtInfo", &CyUnit::getArtInfo,  python::return_value_policy<python::reference_existing_object>(), "CvArtInfoUnit* (int i, eEra)")
		.def("getButton", &CyUnit::getButton, "std::string ()")
/************************************************************************************************/
/* Afforess	                  Start		 03/1/10                       Coded By: KillMePlease   */
/*                                                                                              */
/* Great Commanders                                                                             */
/************************************************************************************************/
		.def("isCommander", &CyUnit::isCommander, "bool ()")
		.def("controlPointsLeft", &CyUnit::controlPointsLeft, "int ()")
		.def("controlPoints", &CyUnit::controlPoints, "int ()")
		.def("commandRange", &CyUnit::commandRange, "int ()")
		.def("getExperience100", &CyUnit::getExperience100, "float ()")
		.def("getRealExperience", &CyUnit::getRealExperience, "float ()")
		.def("setExperience100", &CyUnit::setExperience100, "void (int iNewValue)")
		.def("changeExperience100", &CyUnit::changeExperience100, "void (int iChange, int iMax, bool bFromCombat, bool bInBorders, bool bUpdateGlobal)")
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		.def("captureProbabilityTotal", &CyUnit::captureProbabilityTotal, "int ()")
		.def("captureResistanceTotal", &CyUnit::captureResistanceTotal, "int ()")
		.def("isUnitAtBaseGroup", &CyUnit::isUnitAtBaseGroup, "bool ()")
		.def("isUnitAboveBaseGroup", &CyUnit::isUnitAboveBaseGroup, "bool ()")
		.def("isUnitBelowBaseGroup", &CyUnit::isUnitAboveBaseGroup, "bool ()")
		.def("isHiddenNationality", &CyUnit::isHiddenNationality, "bool ()")
		.def("doHNCapture", &CyUnit::doHNCapture, "void ()")

		.def("getCapturingUnit", &CyUnit::getCapturingUnit, python::return_value_policy<python::manage_new_object>(), "CyUnit* ()")
		.def("setCapturingUnit", &CyUnit::setCapturingUnit, "void (CyUnit* pCapturingUnit)")
		;
}
