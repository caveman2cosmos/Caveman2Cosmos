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
		.def("getMADTargetPlot", &CyUnit::getMADTargetPlot, python::return_value_policy<python::manage_new_object>(), "CyPlot* ()")
		// < M.A.D. Nukes End   >
		.def("isNone", &CyUnit::isNone, "bool () - Is this a valid unit instance?")
		.def("convert", &CyUnit::convert, "void (CyUnit* pUnit)")
		.def("kill", &CyUnit::kill, "void (bool bDelay, int /*PlayerTypes*/ ePlayer)")
		.def("NotifyEntity", &CyUnit::NotifyEntity, "void (int EntityEventType)")
		
		.def("isActionRecommended", &CyUnit::isActionRecommended, "int (int i)")

		.def("doCommand", &CyUnit::doCommand, "void (eCommand, iData1, iData2) - force the unit to perform eCommand")


		.def("canMoveInto", &CyUnit::canMoveInto, "bool (CyPlot* pPlot, bool bAttack, bool bDeclareWar, bool bIgnoreLoad)")

		/********************************************************************************/
		/**		REVOLUTION_MOD							1/1/08				jdog5000	*/
		/**																				*/
		/**		 																		*/
		/********************************************************************************/
		/********************************************************************************/
		/**		REVOLUTION_MOD							END								*/
		/********************************************************************************/
		.def("canHeal", &CyUnit::canHeal, "bool (CyPlot* pPlot)")







		//TSHEEP Assassin Mission
		//.def("assassinCost", &CyUnit::assassinCost, "int (CyUnit* pUnit)")
		//.def("assassinProb", &CyUnit::assassinProb, "int (CyUnit* pUnit)")
		//RevolutionDCM start
		//RevolutionDCM end
		//TSHEEP End

		.def("canFound", &CyUnit::canFound, "bool (CyPlot* pPlot, bool bTestVisible)")
		.def("canConstruct", &CyUnit::canConstruct, "bool (CyPlot* pPlot, int (BuildingTypes) eBuilding)")

		.def("canHurry", &CyUnit::canHurry, "bool (CyPlot* pPlot, bool bTestVisible)")
		.def("canTrade", &CyUnit::canTrade, "bool (CyPlot* pPlot, bool bTestVisible)")
		.def("getEspionagePoints", &CyUnit::getEspionagePoints, "int (CyPlot* pPlot)")

		.def("canBuild", &CyUnit::canBuild, "bool (CyPlot* pPlot, int (BuildTypes) eBuild, bool bTestVisible)")


		.def("canUpgrade", &CyUnit::canUpgrade, "bool (int /*UnitTypes*/ eUnit, bool bTestVisible)")

		.def("getHandicapType", &CyUnit::getHandicapType, "int ()")
		.def("getCivilizationType", &CyUnit::getCivilizationType, "int ()")
		.def("getSpecialUnitType", &CyUnit::getSpecialUnitType, "int ()")
		.def("getCaptureUnitType", &CyUnit::getCaptureUnitType, "int ()")
		.def("getUnitCombatType", &CyUnit::getUnitCombatType, "int ()")
		.def("getDomainType", &CyUnit::getDomainType, "int ()")

		.def("isBarbarian", &CyUnit::isBarbarian, "bool ()")
		.def("isNPC", &CyUnit::isNPC, "bool ()")
		.def("isHominid", &CyUnit::isHominid, "bool ()")
		.def("isHuman", &CyUnit::isHuman, "bool ()")
		.def("baseMoves", &CyUnit::baseMoves, "int ()")
		.def("movesLeft", &CyUnit::movesLeft, "int ()")

		.def("maxMoves", &CyUnit::maxMoves, "int ()")
		.def("canMove", &CyUnit::canMove, "bool ()")
		.def("hasMoved", &CyUnit::hasMoved, "bool ()")
		.def("nukeRange", &CyUnit::nukeRange, "int ()")

		.def("isAnimal", &CyUnit::isAnimal, "bool ()")
		.def("isOnlyDefensive", &CyUnit::isOnlyDefensive, "bool ()")
		.def("isFound", &CyUnit::isFound, "bool ()")
		/********************************************************************************/
		/**		REVOLUTION_MOD							1/1/08				DPII		*/
		/**																				*/
		/**		 																		*/
		/********************************************************************************/
		/*
		*/
		/********************************************************************************/
		/**		REVOLUTION_MOD							END								*/
		/********************************************************************************/
		.def("isGoldenAge", &CyUnit::isGoldenAge, "bool ()")

		.def("isFighting", &CyUnit::isFighting, "bool ()")
		.def("maxHitPoints", &CyUnit::maxHitPoints, "int ()")
		.def("currHitPoints", &CyUnit::currHitPoints, "int ()")
		.def("isHurt", &CyUnit::isHurt, "bool ()")
		.def("isDead", &CyUnit::isDead, "bool ()")
		.def("setBaseCombatStr", &CyUnit::setBaseCombatStr, "void (int)")
		.def("baseCombatStr", &CyUnit::baseCombatStr, "int ()")

		.def("canFight", &CyUnit::canFight, "bool ()")

		.def("airBaseCombatStr", &CyUnit::airBaseCombatStr, "int ()")
		.def("canAirAttack", &CyUnit::canAirAttack, "bool ()")

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
		//TB Combat Mods begin
		//TB Combat Mods end
		.def("experienceNeeded", &CyUnit::experienceNeeded, "int ()") 

		.def("isInvisible", &CyUnit::isInvisible, "bool (int (TeamTypes) eTeam, bool bDebug)")
		.def("isNukeImmune", &CyUnit::isNukeImmune, "bool ()")
/************************************************************************************************/
/* REVDCM_OC                              02/16/10                                phungus420    */
/*                                                                                              */
/* Inquisitions                                                                                 */
/************************************************************************************************/
/************************************************************************************************/
/* REVDCM_OC                               END                                                  */
/************************************************************************************************/

		//TB Combat Mods Begin
		//.def("aidTotal()", &CyUnit::aidTotal, "int ()")
		//TB Combat Mods End


		.def("bombardRate", &CyUnit::bombardRate, "int ()") 

		.def("cargoSpace", &CyUnit::cargoSpace, "int ()")
		.def("changeCargoSpace", &CyUnit::changeCargoSpace, "void (int)")
		.def("getID", &CyUnit::getID, "int ()")

		.def("getGroup", &CyUnit::getGroup, python::return_value_policy<python::manage_new_object>(), "CySelectionGroup* ()")

		.def("getHotKeyNumber", &CyUnit::getHotKeyNumber, "int () - returns the HotKey number for this unit")

		.def("getX", &CyUnit::getX, "int ()")
		.def("getY", &CyUnit::getY, "int ()")
		.def("setXY", &CyUnit::setXY, "int (int iX, int iY)")
		.def("plot", &CyUnit::plot, python::return_value_policy<python::manage_new_object>(), "CyPlot* ()")
		.def("area", &CyUnit::area, python::return_value_policy<python::manage_new_object>(), "CyArea* ()")


		.def("getDamage", &CyUnit::getDamage, "int ()")
		.def("setDamage", &CyUnit::setDamage, "void (int iNewValue, int /*PlayerTypes*/ ePlayer)")
		.def("changeDamage", &CyUnit::changeDamage, "void (int iChange, int /*PlayerTypes*/ ePlayer)")
		.def("getMoves", &CyUnit::getMoves, "int ()")
		.def("setMoves", &CyUnit::setMoves, "void (int iNewValue)")
		.def("finishMoves", &CyUnit::finishMoves, "void ()")
		.def("getExperience", &CyUnit::getExperience, "int ()")
		.def("setExperience", &CyUnit::setExperience, "void (int iNewValue)")
		.def("changeExperience", &CyUnit::changeExperience, "void (int iChange, int iMax, bool bFromCombat, bool bInBorders, bool bUpdateGlobal)")
		.def("getLevel", &CyUnit::getLevel, "int ()")
		.def("setLevel", &CyUnit::setLevel)
		.def("getFacingDirection", &CyUnit::getFacingDirection, "int ()")
		.def("rotateFacingDirectionClockwise", &CyUnit::rotateFacingDirectionClockwise, "void ()")
		.def("rotateFacingDirectionCounterClockwise", &CyUnit::rotateFacingDirectionCounterClockwise, "void ()")
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
		.def("isRiver", &CyUnit::isRiver, "bool ()")

		.def("getExtraMoves", &CyUnit::getExtraMoves, "int ()")
		//TB Combat Mods begin
		//TB Combat Mods end


		.def("getRevoltProtection", &CyUnit::getRevoltProtection, "int ()")
		.def("getPillageChange", &CyUnit::getPillageChange, "int ()")
		.def("getUpgradeDiscount", &CyUnit::getUpgradeDiscount, "int ()")
		.def("getImmobileTimer", &CyUnit::getImmobileTimer, "int ()")
		.def("setImmobileTimer", &CyUnit::setImmobileTimer, "void (int)")

		.def("isMadeAttack", &CyUnit::isMadeAttack, "bool ()")
		.def("setMadeAttack", &CyUnit::setMadeAttack, "void (int iNewValue)")
		//TB Combat Mod begin
		//TB Combat Mod end
		.def("isMadeInterception", &CyUnit::isMadeInterception, "bool ()")
		.def("setMadeInterception", &CyUnit::setMadeInterception, "void (int iNewValue)")

		.def("isPromotionReady", &CyUnit::isPromotionReady, "bool ()")
		.def("setPromotionReady", &CyUnit::setPromotionReady, "void (int iNewValue)")
		.def("getOwner", &CyUnit::getOwner, "int ()")
		.def("getVisualOwner", &CyUnit::getVisualOwner, "int ()")
		.def("getTeam", &CyUnit::getTeam, "int ()")

		.def("getUnitType", &CyUnit::getUnitType, "int ()")
		.def("getLeaderUnitType", &CyUnit::getLeaderUnitType, "int ()")
		.def("setLeaderUnitType", &CyUnit::setLeaderUnitType, "void (int iNewValue)")



		.def("getName", &CyUnit::getName, "str () - Returns the name of a unit along with its type description in parens if using a custom name")
		.def("getNameForm", &CyUnit::getNameForm, "str (int iForm)")
		.def("getNameKey", &CyUnit::getNameKey, "str ()")
		.def("getNameNoDesc", &CyUnit::getNameNoDesc, "str () - Returns the name of a unit without any description afterwards")
		.def("setName", &CyUnit::setName, "void (str)")
		.def("getScriptData", &CyUnit::getScriptData, "str ()")
		.def("setScriptData", &CyUnit::setScriptData, "void (str)")



		.def("canAcquirePromotion", &CyUnit::canAcquirePromotion, "bool (int /*PromotionTypes*/ ePromotion)")
		.def("isPromotionValid", &CyUnit::isPromotionValid, "bool (int /*PromotionTypes*/ ePromotion)")
		.def("isPromotionOverriden", &CyUnit::isPromotionOverriden, "bool (int /*PromotionTypes*/ ePromotion)")
		.def("isHasPromotion", &CyUnit::isHasPromotion, "bool (int /*PromotionTypes*/ ePromotion)")
		.def("isHasUnitCombat", &CyUnit::isHasUnitCombat, "bool (int /*UnitCombatTypes*/ eUnitCombat)")
		.def("setHasPromotion", &CyUnit::setHasPromotion, "void (int (PromotionTypes) eIndex, bool bNewValue)")
		.def("IsSelected", &CyUnit::IsSelected)

		.def("getUnitAIType", &CyUnit::getUnitAIType, "int UnitAIType () - returns the int value of the UnitAIType")
		.def("setUnitAIType", &CyUnit::setUnitAIType, "void UnitAIType (int iUnitAIType) - sets the unit's UnitAIType")

		// Python Helper Functions

		.def("getButton", &CyUnit::getButton, "std::string ()")
/************************************************************************************************/
/* Afforess	                  Start		 03/1/10                       Coded By: KillMePlease   */
/*                                                                                              */
/* Great Commanders                                                                             */
/************************************************************************************************/
		.def("isCommander", &CyUnit::isCommander, "bool ()")
		.def("controlPointsLeft", &CyUnit::controlPointsLeft, "int ()")
		.def("controlPoints", &CyUnit::controlPoints, "int ()")
		.def("getRealExperience", &CyUnit::getRealExperience, "float ()")
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		.def("captureProbabilityTotal", &CyUnit::captureProbabilityTotal, "int ()")
		.def("captureResistanceTotal", &CyUnit::captureResistanceTotal, "int ()")
		.def("isHiddenNationality", &CyUnit::isHiddenNationality, "bool ()")
		.def("doHNCapture", &CyUnit::doHNCapture, "void ()")

		;
}
