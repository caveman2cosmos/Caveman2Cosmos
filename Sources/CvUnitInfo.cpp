//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvUnitInfo.cpp
//
//  PURPOSE: Info class for units
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2003 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"

//======================================================================================================
//					CvUnitInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvUnitInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvUnitInfo::CvUnitInfo() :
m_iDCMBombRange(0),
m_iDCMBombAccuracy(0),
m_bDCMAirBomb1(0),
m_bDCMAirBomb2(0),
m_bDCMAirBomb3(0),
m_bDCMAirBomb4(0),
m_bDCMAirBomb5(0),
m_bDCMFighterEngage(0),
m_iAIWeight(0),
m_iProductionCost(0),
m_iHurryCostModifier(0),
m_iAdvancedStartCost(100),
m_iAdvancedStartCostIncrease(0),
m_iMinAreaSize(0),
m_iMoves(0),
m_iAirRange(0),
m_iAirUnitCap(0),
m_iDropRange(0),
m_iNukeRange(-1),
m_iWorkRate(0),
m_iBaseDiscover(0),
m_iDiscoverMultiplier(0),
m_iBaseHurry(0),
m_iHurryMultiplier(0),
m_iBaseTrade(0),
m_iTradeMultiplier(0),
m_iGreatWorkCulture(0),
m_iEspionagePoints(0),
m_iCombat(0),
m_iCombatLimit(100),
m_iAirCombat(0),
m_iAirCombatLimit(0),
m_iXPValueAttack(0),
m_iXPValueDefense(0),
m_iFirstStrikes(0),
m_iChanceFirstStrikes(0),
m_iInterceptionProbability(0),
//ls612: Advanced Nuke Interception
//m_iNukeInterceptionProbability(0),
//m_iNukeInterceptionRange(0),
m_iEvasionProbability(0),
m_iWithdrawalProbability(0),
m_iCollateralDamage(0),
m_iCollateralDamageLimit(0),
m_iCollateralDamageMaxUnits(0),
m_iCityAttackModifier(0),
m_iCityDefenseModifier(0),
m_iAnimalCombatModifier(0),
m_iHillsAttackModifier(0),
m_iHillsDefenseModifier(0),
m_iBombRate(0),
m_iBombardRate(0),
m_iSpecialCargo(NO_SPECIALUNIT),
m_iSMSpecialCargo(NO_SPECIALUNIT),
m_iSMNotSpecialCargo(NO_SPECIALUNIT),
m_iDomainCargo(NO_DOMAIN),
m_iCargoSpace(0),
m_iSMCargoSpace(0),
m_iSMCargoVolume(0),
m_iConscriptionValue(0),
m_iCultureGarrisonValue(0),
m_iExtraCost(0),
m_iAssetValue(0),
m_iPowerValue(0),
m_iUnitClassType(NO_UNITCLASS),
m_iSpecialUnitType(NO_SPECIALUNIT),
m_iUnitCaptureClassType(NO_UNITCLASS),
m_iUnitCombatType(NO_UNITCOMBAT),
m_iDomainType(NO_DOMAIN),
m_iDefaultUnitAIType(NO_UNITAI),
m_iInvisibleType(NO_INVISIBLE),
m_iAdvisorType(NO_ADVISOR),

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain																*/
/********************************************************************************/
m_iMaxStartEra(NO_ERA),
m_iForceObsoleteTech(NO_TECH),
m_bStateReligion(false),
m_iPrereqGameOption(NO_GAMEOPTION),
m_iNotGameOption(NO_GAMEOPTION),
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

m_iHolyCity(NO_RELIGION),
m_iReligionType(NO_RELIGION),
m_iStateReligion(NO_RELIGION),
m_iPrereqReligion(NO_RELIGION),
m_iPrereqCorporation(NO_CORPORATION),
m_iPrereqBuilding(NO_BUILDING),
m_iPrereqAndTech(NO_TECH),
m_iPrereqAndBonus(NO_BONUS),
m_iGroupSize(0),
m_iGroupDefinitions(0),
m_iUnitMeleeWaveSize(0),
m_iUnitRangedWaveSize(0),
m_iNumUnitNames(0),
m_iCommandType(NO_COMMAND),
m_iLeaderExperience(0),
m_bFoodProduction(false),
m_bNoBadGoodies(false),
m_bOnlyDefensive(false),
m_bNoCapture(false),
m_bQuickCombat(false),
m_bRivalTerritory(false),
m_bMilitaryHappiness(false),
m_bMilitarySupport(false),
m_bMilitaryProduction(false),
m_bPillage(false),
m_bSpy(false),
m_bSabotage(false),
m_bDestroy(false),
m_bStealPlans(false),
m_bInvestigate(false),
m_bCounterSpy(false),
m_bFound(false),
m_bGoldenAge(false),
m_bInvisible(false),
m_bFirstStrikeImmune(false),
m_bNoDefensiveBonus(false),
m_bIgnoreBuildingDefense(false),
m_bCanMoveImpassable(false),
m_bCanMoveAllTerrain(false),
m_bFlatMovementCost(false),
m_bIgnoreTerrainCost(false),
m_bNukeImmune(false),
m_bPrereqBonuses(false),
m_bPrereqReligion(false),
m_bMechanized(false),
m_bRenderBelowWater(false),
m_bRenderAlways(false),
m_bSuicide(false),
m_bLineOfSight(false),
m_bHiddenNationality(false),
m_bAlwaysHostile(false),
/*****************************************************************************************************/
/**  Author: TheLadiesOgre																		  **/
/**  Date: 21.09.2009																			   **/
/**  ModComp: TLOTags																			   **/
/**  Reason Added: New Tag Definition															   **/
/**  Notes:																						 **/
/*****************************************************************************************************/
m_bFreeDrop(false),
/*****************************************************************************************************/
/**  TheLadiesOgre; 21.09.2009; TLOTags															 **/
/*****************************************************************************************************/
m_bNoRevealMap(false),
/************************************************************************************************/
/* REVOLUTION_MOD								 01/01/08						DPII		  */
/*																							  */
/* CoreComponent																				*/
/************************************************************************************************/
m_bInquisitor(false),
m_bCanBeRebel(false),
m_bCanRebelCapture(false),
m_bCannotDefect(false),
m_bCanQuellRebellion(false),
/************************************************************************************************/
/* REVOLUTION_MOD						  END												  */
/************************************************************************************************/

//ls612: Can't Enter non-Owned cities
m_bNoNonOwnedEntry(false),
m_iLeaderPromotion(NO_PROMOTION),
m_fUnitMaxSpeed(0.0f),
m_fUnitPadTime(0.0f),

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain																*/
/********************************************************************************/
m_pbPrereqOrCivics(NULL),
m_pbPrereqBuildingClass(NULL),
m_piPrereqBuildingClassOverrideTech(NULL),
m_piPrereqBuildingClassOverrideEra(NULL),
m_pbForceObsoleteUnitClass(NULL),
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

m_pbUpgradeUnitClass(NULL),
m_pbTargetUnitClass(NULL),
m_pbTargetUnitCombat(NULL),
m_pbDefenderUnitClass(NULL),
m_pbDefenderUnitCombat(NULL),
m_piFlankingStrikeUnitClass(NULL),
m_pbUnitAIType(NULL),
m_pbNotUnitAIType(NULL),
m_pbBuilds(NULL),
m_piReligionSpreads(NULL),
m_piCorporationSpreads(NULL),
m_piTerrainPassableTech(NULL),
m_piFeaturePassableTech(NULL),
m_pbGreatPeoples(NULL),
//m_pbTerrainImpassable(NULL),
//m_pbFeatureImpassable(NULL),
m_piPrereqAndTechs(NULL),
m_piPrereqOrBonuses(NULL),
//m_piProductionTraits(NULL),
m_piFlavorValue(NULL),
m_piTerrainAttackModifier(NULL),
m_piTerrainDefenseModifier(NULL),
m_piFeatureAttackModifier(NULL),
m_piFeatureDefenseModifier(NULL),
m_piUnitClassAttackModifier(NULL),
m_piUnitClassDefenseModifier(NULL),
m_piUnitCombatModifier(NULL),
m_piUnitCombatCollateralImmune(NULL),
m_piDomainModifier(NULL),
m_piBonusProductionModifier(NULL),
m_piUnitGroupRequired(NULL),
m_pbTerrainNative(NULL),
m_pbFeatureNative(NULL),
m_pbFreePromotions(NULL),
m_paszEarlyArtDefineTags(NULL),
m_paszLateArtDefineTags(NULL),
m_paszMiddleArtDefineTags(NULL),
m_paszUnitNames(NULL)
/************************************************************************************************/
/* Afforess					  Start		 Last Update: 3/8/10									*/
/*																							  */
/*																							  */
/************************************************************************************************/
,m_iPrereqVicinityBonus(NO_BONUS)
,m_iBaseFoodChange(0)
,m_iControlPoints(0)
,m_iCommandRange(0)
,m_bWorkerTrade(false)
,m_bMilitaryTrade(false)
,m_bForceUpgrade(false)
,m_bGreatGeneral(false)
,m_bSlave(false)
,m_bRequiresStateReligionInCity(false)
,m_abHasCombatType(NULL)
,m_piPrereqOrVicinityBonuses(NULL)
,m_pbPassableRouteNeeded(NULL)
,m_paszClassicalArtDefineTags(NULL)
,m_paszRennArtDefineTags(NULL)
,m_paszIndustrialArtDefineTags(NULL)
,m_paszFutureArtDefineTags(NULL)
,m_paszCivilizationNames(NULL)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
,m_PropertyManipulators()
,m_pExprTrainCondition(NULL)

//TB Combat Mods begin
//integers
,m_iAttackCombatModifier(0),
m_iDefenseCombatModifier(0),
m_iPursuit(0),
m_iEarlyWithdraw(0),
m_iVSBarbs(0),
m_iArmor(0),
m_iPuncture(0),
m_iOverrun(0),
m_iRepel(0),
m_iFortRepel(0),
m_iRepelRetries(0),
m_iUnyielding(0),
m_iKnockback(0),
m_iKnockbackRetries(0),
m_iStrAdjperRnd(0),
m_iStrAdjperAtt(0),
m_iStrAdjperDef(0),
m_iWithdrawAdjperAtt(0),
m_iUnnerve(0),
m_iEnclose(0),
m_iLunge(0),
m_iDynamicDefense(0),
m_iFortitude(0),
m_iDodgeModifier(0),
m_iPrecisionModifier(0),
m_iPowerShots(0),
m_iPowerShotCombatModifier(0),
m_iPowerShotPunctureModifier(0),
m_iPowerShotPrecisionModifier(0),
m_iPowerShotCriticalModifier(0),
m_iCriticalModifier(0),
m_iEndurance(0),
m_iRoundStunProb(0),
m_iPoisonProbabilityModifier(0),
//Team Project (3)
m_iCaptureProbabilityModifier(0),
m_iCaptureResistanceModifier(0),
//Team Project (4)
//WorkRateMod
m_iHillsWorkModifier(0),
m_iPeaksWorkModifier(0),
m_iBreakdownChance(0),
m_iBreakdownDamage(0),
m_iTaunt(0),
m_iMaxHP(100),
m_iDamageModifier(0),
m_iCostModifier(0),
m_iTotalCombatStrengthModifierBase(0),
m_iTotalCombatStrengthChangeBase(0),
m_iBaseCargoVolume(0),
m_iRBombardDamage(0),
m_iRBombardDamageLimit(0),
m_iRBombardDamageMaxUnits(0),
m_iBaseGroupRank(0),
m_iCombatModifierPerSizeMore(0),
m_iCombatModifierPerSizeLess(0),
m_iCombatModifierPerVolumeMore(0),
m_iCombatModifierPerVolumeLess(0),
m_iBaseSMRankTotal(0),
m_iBaseSMVolumetricRankTotal(0),
m_iSelfHealModifier(0),
m_iNumHealSupport(0),
m_iInsidiousness(0),
m_iInvestigation(0),
m_iStealthStrikes(0),
m_iStealthCombatModifier(0),
m_iTrapDamageMax(0),
m_iTrapDamageMin(0),
m_iTrapComplexity(0),
m_iNumTriggers(0),
m_iAnimalIgnoresBorders(0),
m_iAggression(5),
m_iReligiousCombatModifier(0),
//booleans
m_bStampede(false),
m_bOnslaught(false),
m_bDealsColdDamage(false),
m_bColdImmune(false),
m_bAttackOnlyCities(false),
m_bIgnoreNoEntryLevel(false),
m_bIgnoreZoneofControl(false),
m_bFliesToMove(false),
m_bRBombardForceAbility(false),
m_bNoSelfHeal(false),
m_bExcile(false),
m_bPassage(false),
m_bNoNonOwnedCityEntry(false),
m_bBarbCoExist(false),
m_bBlendIntoCity(false),
m_bUpgradeAnywhere(false),
m_bAssassin(false),
m_bStealthDefense(false),
m_bNoInvisibility(false),
m_bTriggerBeforeAttack(false),
m_bCanAnimalIgnoresBorders(false),
m_bCanAnimalIgnoresImprovements(false),
m_bCanAnimalIgnoresCities(false),
m_bNoNonTypeProdMods(false),
m_bGatherHerd(false)
//TB Combat Mods end
{
	m_zobristValue = GC.getGameINLINE().getSorenRand().getInt();
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvUnitInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvUnitInfo::~CvUnitInfo()
{

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain																*/
/********************************************************************************/
	SAFE_DELETE_ARRAY(m_pbPrereqOrCivics);
	SAFE_DELETE_ARRAY(m_pbPrereqBuildingClass);
	SAFE_DELETE_ARRAY(m_piPrereqBuildingClassOverrideTech);
	SAFE_DELETE_ARRAY(m_piPrereqBuildingClassOverrideEra);
	SAFE_DELETE_ARRAY(m_pbForceObsoleteUnitClass);
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

	SAFE_DELETE_ARRAY(m_pbUpgradeUnitClass);
	SAFE_DELETE_ARRAY(m_pbTargetUnitClass);
	SAFE_DELETE_ARRAY(m_pbTargetUnitCombat);
	SAFE_DELETE_ARRAY(m_pbDefenderUnitClass);
	SAFE_DELETE_ARRAY(m_pbDefenderUnitCombat);
	SAFE_DELETE_ARRAY(m_piFlankingStrikeUnitClass);
	SAFE_DELETE_ARRAY(m_pbUnitAIType);
	SAFE_DELETE_ARRAY(m_pbNotUnitAIType);
	SAFE_DELETE_ARRAY(m_pbBuilds);
	SAFE_DELETE_ARRAY(m_piReligionSpreads);
	SAFE_DELETE_ARRAY(m_piCorporationSpreads);
	SAFE_DELETE_ARRAY(m_piTerrainPassableTech);
	SAFE_DELETE_ARRAY(m_piFeaturePassableTech);
	SAFE_DELETE_ARRAY(m_pbGreatPeoples);
	//SAFE_DELETE_ARRAY(m_pbTerrainImpassable);
	//SAFE_DELETE_ARRAY(m_pbFeatureImpassable);
	SAFE_DELETE_ARRAY(m_piPrereqAndTechs);
	SAFE_DELETE_ARRAY(m_piPrereqOrBonuses);
	//SAFE_DELETE_ARRAY(m_piProductionTraits);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piTerrainAttackModifier);
	SAFE_DELETE_ARRAY(m_piTerrainDefenseModifier);
	SAFE_DELETE_ARRAY(m_piFeatureAttackModifier);
	SAFE_DELETE_ARRAY(m_piFeatureDefenseModifier);
	SAFE_DELETE_ARRAY(m_piUnitClassAttackModifier);
	SAFE_DELETE_ARRAY(m_piUnitClassDefenseModifier);
	SAFE_DELETE_ARRAY(m_piUnitCombatModifier);
	SAFE_DELETE_ARRAY(m_piUnitCombatCollateralImmune);
	SAFE_DELETE_ARRAY(m_piDomainModifier);
	SAFE_DELETE_ARRAY(m_piBonusProductionModifier);
	SAFE_DELETE_ARRAY(m_piUnitGroupRequired);
	SAFE_DELETE_ARRAY(m_pbTerrainNative);
	SAFE_DELETE_ARRAY(m_pbFeatureNative);
	SAFE_DELETE_ARRAY(m_pbFreePromotions);
	SAFE_DELETE_ARRAY(m_paszEarlyArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszLateArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszMiddleArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszUnitNames);
/************************************************************************************************/
/* Afforess					  Start		 Last Update: 3/8/10									*/
/*																							  */
/*																							  */
/************************************************************************************************/
	SAFE_DELETE_ARRAY(m_pbPrereqBuildingClass);
	SAFE_DELETE_ARRAY(m_piPrereqOrVicinityBonuses);
	SAFE_DELETE_ARRAY(m_pbPassableRouteNeeded);
	SAFE_DELETE_ARRAY(m_paszClassicalArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszRennArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszIndustrialArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszFutureArtDefineTags);
	SAFE_DELETE_ARRAY(m_paszCivilizationNames);
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	SAFE_DELETE(m_pExprTrainCondition);
	SAFE_DELETE(m_aiSupportPercent);

	for (int i=0; i<(int)m_aOutcomeMissions.size(); i++)
	{
		SAFE_DELETE(m_aOutcomeMissions[i]);
	}
//TB Combat Mod begin

	//Struct Vector
	for (int i=0; i<(int)m_aEnabledCivilizationTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aEnabledCivilizationTypes[i]));
	}
	//TB Combat Mod end
}

const wchar* CvUnitInfo::getExtraHoverText() const
{
	if (!m_szExtraHoverTextKey.empty())
	{
		return gDLL->getText(m_szExtraHoverTextKey);
	}
	else
	{
		return L"";
	}
}

/************************************************************************************************/
/* DCM									 04/19/09								Johny Smith  */
/************************************************************************************************/
// Dale - RB: Field Bombard START
int CvUnitInfo::getDCMBombRange() const
{
	return m_iDCMBombRange;
}

int CvUnitInfo::getDCMBombAccuracy() const
{
	return m_iDCMBombAccuracy;
}
// Dale - RB: Field Bombard END

// Dale - AB: Bombing START
bool CvUnitInfo::getDCMAirBomb1() const
{
	return m_bDCMAirBomb1;
}

bool CvUnitInfo::getDCMAirBomb2() const
{
	return m_bDCMAirBomb2;
}

bool CvUnitInfo::getDCMAirBomb3() const
{
	return m_bDCMAirBomb3;
}

bool CvUnitInfo::getDCMAirBomb4() const
{
	return m_bDCMAirBomb4;
}

bool CvUnitInfo::getDCMAirBomb5() const
{
	return m_bDCMAirBomb5;
}
// Dale - AB: Bombing END
// Dale - FE: Fighters START
bool CvUnitInfo::getDCMFighterEngage() const
{
	return m_bDCMFighterEngage;
}
// Dale - FE: Fighters END
/************************************************************************************************/
/* DCM									 END												  */
/************************************************************************************************/

int CvUnitInfo::getAIWeight() const
{
	return m_iAIWeight;
}

int CvUnitInfo::getProductionCost() const
{
	return m_iProductionCost;
}

int CvUnitInfo::getHurryCostModifier() const
{
	return m_iHurryCostModifier;
}

int CvUnitInfo::getAdvancedStartCost() const
{
	return m_iAdvancedStartCost;
}

int CvUnitInfo::getAdvancedStartCostIncrease() const
{
	return m_iAdvancedStartCostIncrease;
}

int CvUnitInfo::getMinAreaSize() const
{
	return m_iMinAreaSize;
}

int CvUnitInfo::getMoves() const
{
	return m_iMoves;
}

int CvUnitInfo::getAirRange() const
{
	return m_iAirRange;
}

int CvUnitInfo::getAirUnitCap() const
{
	return m_iAirUnitCap;
}

int CvUnitInfo::getDropRange() const
{
	return m_iDropRange;
}

int CvUnitInfo::getNukeRange() const
{
	return m_iNukeRange;
}

int CvUnitInfo::getWorkRate() const
{
	return m_iWorkRate;
}

int CvUnitInfo::getBaseDiscover() const
{
	return m_iBaseDiscover;
}

int CvUnitInfo::getDiscoverMultiplier() const
{
	return m_iDiscoverMultiplier;
}

int CvUnitInfo::getBaseHurry() const
{
	return m_iBaseHurry;
}

int CvUnitInfo::getHurryMultiplier() const
{
	return m_iHurryMultiplier;
}

int CvUnitInfo::getBaseTrade() const
{
	return m_iBaseTrade;
}

int CvUnitInfo::getTradeMultiplier() const
{
	return m_iTradeMultiplier;
}

int CvUnitInfo::getGreatWorkCulture() const
{
	return m_iGreatWorkCulture;
}

int CvUnitInfo::getEspionagePoints() const
{
	return m_iEspionagePoints;
}

int CvUnitInfo::getCombat() const
{
	return m_iCombat;
}

void CvUnitInfo::setCombat(int iNum)
{
	m_iCombat = iNum;
}

int CvUnitInfo::getCombatLimit() const
{
	return m_iCombatLimit;
}

int CvUnitInfo::getAirCombat() const
{
	return m_iAirCombat;
}

int CvUnitInfo::getAirCombatLimit() const
{
	return m_iAirCombatLimit;
}

int CvUnitInfo::getXPValueAttack() const
{
	return m_iXPValueAttack;
}

int CvUnitInfo::getXPValueDefense() const
{
	return m_iXPValueDefense;
}

int CvUnitInfo::getFirstStrikes() const
{
	return m_iFirstStrikes;
}

int CvUnitInfo::getChanceFirstStrikes() const
{
	return m_iChanceFirstStrikes;
}

int CvUnitInfo::getInterceptionProbability() const
{
	return m_iInterceptionProbability;
}

//ls612: Advanced Nuke Interception
//int CvUnitInfo::getNukeInterceptionProbability() const
//{
//	return m_iNukeInterceptionProbability;
//}
//
//int CvUnitInfo::getNukeInterceptionRange() const
//{
//	return m_iNukeInterceptionRange;
//}

int CvUnitInfo::getEvasionProbability() const
{
	return m_iEvasionProbability;
}

int CvUnitInfo::getWithdrawalProbability(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_FIGHT_OR_FLIGHT) && m_iWithdrawalProbability > 0)
	{
		if (hasUnitCombat((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_WILD")))
		{
			return 0;
		}
		return (m_iWithdrawalProbability / 2);
	}

	return m_iWithdrawalProbability;
}

int CvUnitInfo::getCollateralDamage() const
{
	return m_iCollateralDamage;
}

int CvUnitInfo::getCollateralDamageLimit() const
{
	return m_iCollateralDamageLimit;
}

int CvUnitInfo::getCollateralDamageMaxUnits() const
{
	return m_iCollateralDamageMaxUnits;
}

int CvUnitInfo::getCityAttackModifier() const
{
	return m_iCityAttackModifier;
}

int CvUnitInfo::getCityDefenseModifier() const
{
	return m_iCityDefenseModifier;
}

int CvUnitInfo::getAnimalCombatModifier() const
{
	return m_iAnimalCombatModifier;
}

int CvUnitInfo::getHillsAttackModifier() const
{
	return m_iHillsAttackModifier;
}

int CvUnitInfo::getHillsDefenseModifier() const
{
	return m_iHillsDefenseModifier;
}

int CvUnitInfo::getBombRate() const
{
	return m_iBombRate;
}

int CvUnitInfo::getBombardRate() const
{
	return m_iBombardRate;
}

int CvUnitInfo::getSpecialCargo(bool bForLoad) const
{
	if (!bForLoad && GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		return m_iSMSpecialCargo;
	}

	return m_iSpecialCargo;
}

int CvUnitInfo::getSMSpecialCargo() const
{
	return m_iSMSpecialCargo;
}

int CvUnitInfo::getSMNotSpecialCargo() const
{
	return m_iSMNotSpecialCargo;
}

int CvUnitInfo::getDomainCargo() const
{
	return m_iDomainCargo;
}

int CvUnitInfo::getCargoSpace() const
{
	return m_iCargoSpace;
}

int CvUnitInfo::getSMCargoSpace() const
{
	return m_iSMCargoSpace;
}

int CvUnitInfo::getSMCargoVolume() const
{
	return m_iSMCargoVolume;
}

int CvUnitInfo::getConscriptionValue() const
{
	return m_iConscriptionValue;
}

int CvUnitInfo::getCultureGarrisonValue() const
{
	return m_iCultureGarrisonValue;
}

int CvUnitInfo::getExtraCost() const
{
	return m_iExtraCost;
}

int CvUnitInfo::getAssetValue(bool bForLoad) const
{
	if (!bForLoad)
	{
		return m_iAssetValue * 100;
	}
	return m_iAssetValue;
}

int CvUnitInfo::getPowerValue(bool bForLoad) const
{
	if (!bForLoad)
	{
		return m_iPowerValue * 100;
	}
	return m_iPowerValue;
}

int CvUnitInfo::getUnitClassType() const
{
	return m_iUnitClassType;
}

int CvUnitInfo::getSpecialUnitType() const
{
	return m_iSpecialUnitType;
}

int CvUnitInfo::getUnitCaptureClassType() const
{
	return m_iUnitCaptureClassType;
}

int CvUnitInfo::getUnitCombatType() const
{
	return m_iUnitCombatType;
}

int CvUnitInfo::getDomainType() const
{
	return m_iDomainType;
}

int CvUnitInfo::getDefaultUnitAIType() const
{
	return m_iDefaultUnitAIType;
}

int CvUnitInfo::getInvisibleType() const
{
	return m_iInvisibleType;
}

int CvUnitInfo::getSeeInvisibleType(int i) const
{
	FAssert(i < (int)m_aiSeeInvisibleTypes.size());

	return m_aiSeeInvisibleTypes[i];
}

int CvUnitInfo::getNumSeeInvisibleTypes() const
{
	return (int)m_aiSeeInvisibleTypes.size();
}

int CvUnitInfo::getAdvisorType() const
{
	return m_iAdvisorType;
}

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain																*/
/********************************************************************************/
int CvUnitInfo::getMaxStartEra() const
{
	return m_iMaxStartEra;
}

int CvUnitInfo::getForceObsoleteTech() const
{
	return m_iForceObsoleteTech;
}

bool CvUnitInfo::isStateReligion() const
{
	return m_bStateReligion;
}

int CvUnitInfo::getPrereqGameOption() const
{
	return m_iPrereqGameOption;
}

int CvUnitInfo::getNotGameOption() const
{
	return m_iNotGameOption;
}
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

int CvUnitInfo::getHolyCity() const
{
	return m_iHolyCity;
}

int CvUnitInfo::getReligionType() const
{
	return m_iReligionType;
}

int CvUnitInfo::getStateReligion() const
{
	return m_iStateReligion;
}

int CvUnitInfo::getPrereqReligion() const
{
	return m_iPrereqReligion;
}

int CvUnitInfo::getPrereqCorporation() const
{
	return m_iPrereqCorporation;
}

int CvUnitInfo::getPrereqBuilding() const
{
	return m_iPrereqBuilding;
}

int CvUnitInfo::getPrereqOrBuildingsNum() const
{
	return m_aePrereqOrBuildings.size();
}

BuildingTypes CvUnitInfo::getPrereqOrBuilding(int i) const
{
	FAssertMsg(i < getPrereqOrBuildingsNum(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return (BuildingTypes)m_aePrereqOrBuildings[i];
}

int CvUnitInfo::getPrereqAndTech() const
{
	return m_iPrereqAndTech;
}

int CvUnitInfo::getPrereqAndBonus() const
{
	return m_iPrereqAndBonus;
}

int CvUnitInfo::getGroupSize() const// the initial number of individuals in the unit group
{
	return m_iGroupSize;
}

int CvUnitInfo::getGroupDefinitions() const// the number of UnitMeshGroups for this unit
{
	return m_iGroupDefinitions;
}

int CvUnitInfo::getMeleeWaveSize() const
{
	return m_iUnitMeleeWaveSize;
}

int CvUnitInfo::getRangedWaveSize() const
{
	return m_iUnitRangedWaveSize;
}

int CvUnitInfo::getNumUnitNames() const
{
	return m_iNumUnitNames;
}

bool CvUnitInfo::isFoodProduction() const
{
	return m_bFoodProduction;
}

bool CvUnitInfo::isNoBadGoodies() const
{
	return m_bNoBadGoodies;
}

bool CvUnitInfo::isOnlyDefensive() const
{
	return m_bOnlyDefensive;
}

bool CvUnitInfo::isNoCapture() const
{
	return (m_bNoCapture);
}

bool CvUnitInfo::isQuickCombat() const
{
	return m_bQuickCombat;
}

bool CvUnitInfo::isRivalTerritory() const
{
	return m_bRivalTerritory;
}

bool CvUnitInfo::isMilitaryHappiness() const
{
	return m_bMilitaryHappiness;
}

bool CvUnitInfo::isMilitarySupport() const
{
	return m_bMilitarySupport;
}

bool CvUnitInfo::isMilitaryProduction() const
{
	return m_bMilitaryProduction;
}

bool CvUnitInfo::isPillage() const
{
	return m_bPillage;
}

bool CvUnitInfo::isSpy() const
{
	return m_bSpy;
}

bool CvUnitInfo::isSabotage() const
{
	return m_bSabotage;
}

bool CvUnitInfo::isDestroy() const
{
	return m_bDestroy;
}

bool CvUnitInfo::isStealPlans() const
{
	return m_bStealPlans;
}

bool CvUnitInfo::isInvestigate() const
{
	return m_bInvestigate;
}

bool CvUnitInfo::isCounterSpy() const
{
	return m_bCounterSpy;
}

bool CvUnitInfo::isFound() const
{
	return m_bFound;
}

bool CvUnitInfo::isGoldenAge() const
{
	return m_bGoldenAge;
}

bool CvUnitInfo::isInvisible() const
{
	return m_bInvisible;
}

void CvUnitInfo::setInvisible(bool bEnable)
{
	m_bInvisible = bEnable;
}

bool CvUnitInfo::isFirstStrikeImmune() const
{
	return m_bFirstStrikeImmune;
}

bool CvUnitInfo::isNoDefensiveBonus() const
{
	return m_bNoDefensiveBonus;
}

bool CvUnitInfo::isIgnoreBuildingDefense() const
{
	return m_bIgnoreBuildingDefense;
}

bool CvUnitInfo::isCanMoveImpassable() const
{
	return m_bCanMoveImpassable;
}

bool CvUnitInfo::isCanMoveAllTerrain() const
{
	return m_bCanMoveAllTerrain;
}

bool CvUnitInfo::isFlatMovementCost() const
{
	return m_bFlatMovementCost;
}

bool CvUnitInfo::isIgnoreTerrainCost() const
{
	return m_bIgnoreTerrainCost;
}

bool CvUnitInfo::isNukeImmune() const
{
	return m_bNukeImmune;
}

bool CvUnitInfo::isPrereqBonuses() const
{
	return m_bPrereqBonuses;
}

bool CvUnitInfo::isPrereqReligion() const
{
	return m_bPrereqReligion;
}

bool CvUnitInfo::isMechUnit() const
{
	return m_bMechanized;
}

bool CvUnitInfo::isRenderBelowWater() const
{
	return m_bRenderBelowWater;
}

bool CvUnitInfo::isRenderAlways() const
{
	return m_bRenderAlways;
}

bool CvUnitInfo::isSuicide() const
{
	return m_bSuicide;
}

bool CvUnitInfo::isLineOfSight() const
{
	return m_bLineOfSight;
}

bool CvUnitInfo::isHiddenNationality() const
{
	return m_bHiddenNationality;
}

bool CvUnitInfo::isAlwaysHostile() const
{
	return m_bAlwaysHostile;
}

/*****************************************************************************************************/
/**  Author: TheLadiesOgre																		  **/
/**  Date: 21.09.2009																			   **/
/**  ModComp: TLOTags																			   **/
/**  Reason Added: New Tag Definition															   **/
/**  Notes:																						 **/
/*****************************************************************************************************/
bool CvUnitInfo::isFreeDrop() const
{
	return m_bFreeDrop;
}

/*****************************************************************************************************/
/**  TheLadiesOgre; 21.09.2009; TLOTags															 **/
/*****************************************************************************************************/

bool CvUnitInfo::isNoRevealMap() const
{
	return m_bNoRevealMap;
}

/************************************************************************************************/
/* REVOLUTION_MOD								 01/01/08						DPII		  */
/*																							  */
/* CoreComponent																				*/
/************************************************************************************************/
bool CvUnitInfo::isInquisitor() const
{
	return m_bInquisitor;
}

bool CvUnitInfo::isCanBeRebel() const
{
	return m_bCanBeRebel;
}

bool CvUnitInfo::isCanRebelCapture() const
{
	return m_bCanRebelCapture;
}

bool CvUnitInfo::isCannotDefect() const
{
	return m_bCannotDefect;
}

bool CvUnitInfo::isCanQuellRebellion() const
{
	return m_bCanQuellRebellion;
}
/************************************************************************************************/
/* REVOLUTION_MOD						  END												  */
/************************************************************************************************/

//ls612: Can't enter non-Owned cities
bool CvUnitInfo::isNoNonOwnedEntry() const
{
	return m_bNoNonOwnedEntry;
}

float CvUnitInfo::getUnitMaxSpeed() const
{
	return m_fUnitMaxSpeed;
}

float CvUnitInfo::getUnitPadTime() const
{
	return m_fUnitPadTime;
}

int CvUnitInfo::getCommandType() const
{
	return m_iCommandType;
}

void CvUnitInfo::setCommandType(int iNewType)
{
	m_iCommandType = iNewType;
}

BoolExpr* CvUnitInfo::getTrainCondition()
{
	return m_pExprTrainCondition;
}

// BUG - Unit Experience - start
/*
 * Returns true if this unit type is eligible to receive experience points.
 */
bool CvUnitInfo::canAcquireExperience() const
{
	if (m_iUnitCombatType != NO_UNITCOMBAT)
	{
		for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
		{
			if (GC.getPromotionInfo((PromotionTypes)iI).getUnitCombat(m_iUnitCombatType))
			{
				return true;
			}
		}
	}

	return false;
}
// BUG - Unit Experience - end


// Arrays

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain																*/
/********************************************************************************/
bool CvUnitInfo::isPrereqOrCivics(int i) const
{
	FAssertMsg(i < GC.getNumCivicInfos(), "Index out of bounds");
	FAssertMsg(i >= -1, "Index out of bounds");
	if (i == NO_CIVIC)
	{
		return m_pbPrereqOrCivics != NULL;
	}
	return m_pbPrereqOrCivics ? m_pbPrereqOrCivics[i] : false;
}

bool CvUnitInfo::isPrereqBuildingClass(int i) const
{
	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	FAssertMsg(i >= -1, "Index out of bounds");
	if(i == NO_BUILDINGCLASS)
	{
		return m_pbPrereqBuildingClass != NULL;
	}
	return m_pbPrereqBuildingClass ? m_pbPrereqBuildingClass[i] : false;
}

int CvUnitInfo::getPrereqBuildingClassOverrideTech(int i) const
{
	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piPrereqBuildingClassOverrideTech ? m_piPrereqBuildingClassOverrideTech[i] : -1;
}

int CvUnitInfo::getPrereqBuildingClassOverrideEra(int i) const
{
	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piPrereqBuildingClassOverrideEra ? m_piPrereqBuildingClassOverrideEra[i] : -1;
}

bool CvUnitInfo::getForceObsoleteUnitClass(int i) const
{
	FAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	FAssertMsg(i >= -1, "Index out of bounds");
	if (i == NO_UNITCLASS)
	{
		return m_pbForceObsoleteUnitClass != NULL;
	}
	return m_pbForceObsoleteUnitClass ? m_pbForceObsoleteUnitClass[i] : false;
}
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

int CvUnitInfo::getPrereqAndTechs(int i) const
{
	FAssertMsg(i < GC.getNUM_UNIT_AND_TECH_PREREQS(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piPrereqAndTechs ? m_piPrereqAndTechs[i] : -1;
}

int CvUnitInfo::getPrereqOrBonuses(int i) const
{
	FAssertMsg(i < GC.getNUM_UNIT_PREREQ_OR_BONUSES(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piPrereqOrBonuses ? m_piPrereqOrBonuses[i] : -1;
}

//int CvUnitInfo::getProductionTraits(int i) const
//{
//	FAssertMsg(i < GC.getNumTraitInfos(), "Index out of bounds");
//	FAssertMsg(i > -1, "Index out of bounds");
//	return m_piProductionTraits ? m_piProductionTraits[i] : 0;
//}

int CvUnitInfo::getFlavorValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}

int CvUnitInfo::getTerrainAttackModifier(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piTerrainAttackModifier ? m_piTerrainAttackModifier[i] : 0;
}

int CvUnitInfo::getTerrainDefenseModifier(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piTerrainDefenseModifier ? m_piTerrainDefenseModifier[i] : 0;
}

int CvUnitInfo::getFeatureAttackModifier(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFeatureAttackModifier ? m_piFeatureAttackModifier[i] : 0;
}

int CvUnitInfo::getFeatureDefenseModifier(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFeatureDefenseModifier ? m_piFeatureDefenseModifier[i] : 0;
}

int CvUnitInfo::getUnitClassAttackModifier(int i) const
{
	FAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piUnitClassAttackModifier ? m_piUnitClassAttackModifier[i] : 0;
}

int CvUnitInfo::getUnitClassDefenseModifier(int i) const
{
	FAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piUnitClassDefenseModifier ? m_piUnitClassDefenseModifier[i] : 0;
}

int CvUnitInfo::getUnitCombatModifier(int i) const
{
	FAssertMsg(i < GC.getNumUnitCombatInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piUnitCombatModifier ? m_piUnitCombatModifier[i] : 0;
}

int CvUnitInfo::getUnitCombatCollateralImmune(int i) const
{
	FAssertMsg(i < GC.getNumUnitCombatInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piUnitCombatCollateralImmune ? m_piUnitCombatCollateralImmune[i] : 0;
}

int CvUnitInfo::getDomainModifier(int i) const
{
	FAssertMsg(i < NUM_DOMAIN_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piDomainModifier ? m_piDomainModifier[i] : 0;
}

int CvUnitInfo::getBonusProductionModifier(int i) const
{
	FAssertMsg(i < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piBonusProductionModifier ? m_piBonusProductionModifier[i] : 0;
}

int CvUnitInfo::getUnitGroupRequired(int i) const
{
	FAssertMsg(i < getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piUnitGroupRequired ? m_piUnitGroupRequired[i] : NULL;
}

bool CvUnitInfo::getUpgradeUnitClass(int i) const
{
	FAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbUpgradeUnitClass ? m_pbUpgradeUnitClass[i] : false;
}

bool CvUnitInfo::getTargetUnitClass(int i) const
{
	FAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbTargetUnitClass ? m_pbTargetUnitClass[i] : false;
}

bool CvUnitInfo::getTargetUnitCombat(int i) const
{
	FAssertMsg(i < GC.getNumUnitCombatInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbTargetUnitCombat ? m_pbTargetUnitCombat[i] : false;
}

bool CvUnitInfo::getDefenderUnitClass(int i) const
{
	FAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbDefenderUnitClass ? m_pbDefenderUnitClass[i] : false;
}

bool CvUnitInfo::getDefenderUnitCombat(int i) const
{
	FAssertMsg(i < GC.getNumUnitCombatInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbDefenderUnitCombat ? m_pbDefenderUnitCombat[i] : false;
}

int CvUnitInfo::getFlankingStrikeUnitClass(int i) const
{
	FAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFlankingStrikeUnitClass ? m_piFlankingStrikeUnitClass[i] : -1;
}

bool CvUnitInfo::getUnitAIType(int i) const
{
	FAssertMsg(i < NUM_UNITAI_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbUnitAIType ? m_pbUnitAIType[i] : false;
}

bool CvUnitInfo::getNotUnitAIType(int i) const
{
	FAssertMsg(i < NUM_UNITAI_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbNotUnitAIType ? m_pbNotUnitAIType[i] : false;
}

bool CvUnitInfo::getBuilds(int i) const
{
	FAssertMsg(i < GC.getNumBuildInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbBuilds ? m_pbBuilds[i] : false;
}

int CvUnitInfo::getReligionSpreads(int i) const
{
	FAssertMsg(i < GC.getNumReligionInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piReligionSpreads ? m_piReligionSpreads[i] : -1;
}

int CvUnitInfo::getCorporationSpreads(int i) const
{
	FAssertMsg(i < GC.getNumCorporationInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piCorporationSpreads ? m_piCorporationSpreads[i] : -1;
}

int CvUnitInfo::getTerrainPassableTech(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piTerrainPassableTech ? m_piTerrainPassableTech[i] : -1;
}

int CvUnitInfo::getFeaturePassableTech(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFeaturePassableTech ? m_piFeaturePassableTech[i] : -1;
}

bool CvUnitInfo::getGreatPeoples(int i) const
{
	FAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbGreatPeoples ? m_pbGreatPeoples[i] : false;
}

int CvUnitInfo::getBuildings(int i) const
{
	FAssertMsg(i < GC.getNumBuildingInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbBuildings[i];
}

bool CvUnitInfo::getHasBuilding(int i) const
{
	FAssertMsg(i < GC.getNumBuildingInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return std::find(m_pbBuildings.begin(), m_pbBuildings.end(), i) != m_pbBuildings.end();
}

int CvUnitInfo::getNumBuildings() const
{
	return m_pbBuildings.size();
}

//
//bool CvUnitInfo::getTerrainImpassable(int i) const
//{
//	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
//	FAssertMsg(i > -1, "Index out of bounds");
//	return m_pbTerrainImpassable ? m_pbTerrainImpassable[i] : false;
//}
//
//bool CvUnitInfo::getFeatureImpassable(int i) const
//{
//	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
//	FAssertMsg(i > -1, "Index out of bounds");
//	return m_pbFeatureImpassable ? m_pbFeatureImpassable[i] : false;
//}

bool CvUnitInfo::getTerrainNative(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbTerrainNative ? m_pbTerrainNative[i] : false;
}

bool CvUnitInfo::getFeatureNative(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbFeatureNative ? m_pbFeatureNative[i] : false;
}

bool CvUnitInfo::getFreePromotions(int i) const
{
	FAssertMsg(i < GC.getNumPromotionInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbFreePromotions ? m_pbFreePromotions[i] : false;
}

int CvUnitInfo::getLeaderPromotion() const
{
	return m_iLeaderPromotion;
}

int CvUnitInfo::getLeaderExperience() const
{
	return m_iLeaderExperience;
}

CvOutcomeList* CvUnitInfo::getKillOutcomeList()
{
	return &m_KillOutcomeList;
}

int CvUnitInfo::getNumActionOutcomes()
{
	return m_aOutcomeMissions.size();
}

MissionTypes CvUnitInfo::getActionOutcomeMission(int index)
{
	return m_aOutcomeMissions[index]->getMission();
}

CvOutcomeList* CvUnitInfo::getActionOutcomeList(int index)
{
	return m_aOutcomeMissions[index]->getOutcomeList();
}

CvOutcomeList* CvUnitInfo::getActionOutcomeListByMission(MissionTypes eMission)
{
	for (int i = 0; i < (int) m_aOutcomeMissions.size(); i++)
	{
		if (m_aOutcomeMissions[i]->getMission() == eMission)
		{
			return m_aOutcomeMissions[i]->getOutcomeList();
		}
	}
	return NULL;
}

CvOutcomeMission* CvUnitInfo::getOutcomeMission(int index)
{
	return m_aOutcomeMissions[index];
}

CvOutcomeMission* CvUnitInfo::getOutcomeMissionByMission(MissionTypes eMission)
{
	for (int i = 0; i < (int) m_aOutcomeMissions.size(); i++)
	{
		if (m_aOutcomeMissions[i]->getMission() == eMission)
		{
			return m_aOutcomeMissions[i];
		}
	}
	return NULL;
}

const TCHAR* CvUnitInfo::getEarlyArtDefineTag(int i, UnitArtStyleTypes eStyle) const
{
	FAssertMsg(i < getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if (NO_UNIT_ARTSTYLE != eStyle)
	{
		int iIndex = GC.getInfoTypeForString(getType());
		if (-1 != iIndex)
		{
			const TCHAR* pcTag = GC.getUnitArtStyleTypeInfo(eStyle).getEarlyArtDefineTag(i, iIndex);
			if (NULL != pcTag)
			{
				return pcTag;
			}
		}
	}

	return (m_paszEarlyArtDefineTags) ? m_paszEarlyArtDefineTags[i] : NULL;
}

void CvUnitInfo::setEarlyArtDefineTag(int i, const TCHAR* szVal)
{
	FAssertMsg(i < getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	m_paszEarlyArtDefineTags[i] = szVal;
}

const TCHAR* CvUnitInfo::getLateArtDefineTag(int i, UnitArtStyleTypes eStyle) const
{
	FAssertMsg(i < getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if (NO_UNIT_ARTSTYLE != eStyle)
	{
		const int iIndex = GC.getInfoTypeForString(getType());
		if (-1 != iIndex)
		{
			const TCHAR* pcTag = GC.getUnitArtStyleTypeInfo(eStyle).getLateArtDefineTag(i, iIndex);
			if (NULL != pcTag)
			{
				return pcTag;
			}
		}
	}

	return (m_paszLateArtDefineTags) ? m_paszLateArtDefineTags[i] : NULL;
}

void CvUnitInfo::setLateArtDefineTag(int i, const TCHAR* szVal)
{
	FAssertMsg(i < getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	m_paszLateArtDefineTags[i] = szVal;
}

const TCHAR* CvUnitInfo::getMiddleArtDefineTag(int i, UnitArtStyleTypes eStyle) const
{
	FAssertMsg(i < getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if (NO_UNIT_ARTSTYLE != eStyle)
	{
		const int iIndex = GC.getInfoTypeForString(getType());
		if (-1 != iIndex)
		{
			const TCHAR* pcTag = GC.getUnitArtStyleTypeInfo(eStyle).getMiddleArtDefineTag(i, iIndex);
			if (NULL != pcTag)
			{
				return pcTag;
			}
		}
	}

	return (m_paszMiddleArtDefineTags) ? m_paszMiddleArtDefineTags[i] : NULL;
}

void CvUnitInfo::setMiddleArtDefineTag(int i, const TCHAR* szVal)
{
	FAssertMsg(i < getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	m_paszMiddleArtDefineTags[i] = szVal;
}

const TCHAR* CvUnitInfo::getUnitNames(int i) const
{
	FAssertMsg(i < getNumUnitNames(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return (m_paszUnitNames) ? m_paszUnitNames[i] : NULL;
}

const TCHAR* CvUnitInfo::getFormationType() const
{
	return m_szFormationType;
}

const TCHAR* CvUnitInfo::getButton() const
{
	return m_szArtDefineButton;
}

void CvUnitInfo::updateArtDefineButton()
{
	m_szArtDefineButton = getArtInfo(0, NO_ERA, NO_UNIT_ARTSTYLE)->getButton();
}

/************************************************************************************************/
/* Afforess					  Start		 Last Update: 3/10/10								   */
/*																							  */
/*																							  */
/************************************************************************************************/
const TCHAR* CvUnitInfo::getClassicalArtDefineTag(int i, UnitArtStyleTypes eStyle) const
{
	FAssertMsg(i < getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if (NO_UNIT_ARTSTYLE != eStyle)
	{
		int iIndex = GC.getInfoTypeForString(getType());
		if (-1 != iIndex)
		{
			const TCHAR* pcTag = GC.getUnitArtStyleTypeInfo(eStyle).getClassicalArtDefineTag(i, iIndex);
			if (NULL != pcTag)
			{
				return pcTag;
			}
		}
	}

	return (m_paszClassicalArtDefineTags) ? m_paszClassicalArtDefineTags[i] : NULL;
}

void CvUnitInfo::setClassicalArtDefineTag(int i, const TCHAR* szVal)
{
	FAssertMsg(i < getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	m_paszClassicalArtDefineTags[i] = szVal;
}

const TCHAR* CvUnitInfo::getRennArtDefineTag(int i, UnitArtStyleTypes eStyle) const
{
	FAssertMsg(i < getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if (NO_UNIT_ARTSTYLE != eStyle)
	{
		const int iIndex = GC.getInfoTypeForString(getType());
		if (-1 != iIndex)
		{
			const TCHAR* pcTag = GC.getUnitArtStyleTypeInfo(eStyle).getRennArtDefineTag(i, iIndex);
			if (NULL != pcTag)
			{
				return pcTag;
			}
		}
	}

	return (m_paszRennArtDefineTags) ? m_paszRennArtDefineTags[i] : NULL;
}

void CvUnitInfo::setRennArtDefineTag(int i, const TCHAR* szVal)
{
	FAssertMsg(i < getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	m_paszRennArtDefineTags[i] = szVal;
}

const TCHAR* CvUnitInfo::getIndustrialArtDefineTag(int i, UnitArtStyleTypes eStyle) const
{
	FAssertMsg(i < getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if (NO_UNIT_ARTSTYLE != eStyle)
	{
		const int iIndex = GC.getInfoTypeForString(getType());
		if (-1 != iIndex)
		{
			const TCHAR* pcTag = GC.getUnitArtStyleTypeInfo(eStyle).getIndustrialArtDefineTag(i, iIndex);
			if (NULL != pcTag)
			{
				return pcTag;
			}
		}
	}

	return (m_paszIndustrialArtDefineTags) ? m_paszIndustrialArtDefineTags[i] : NULL;
}

void CvUnitInfo::setIndustrialArtDefineTag(int i, const TCHAR* szVal)
{
	FAssertMsg(i < getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	m_paszIndustrialArtDefineTags[i] = szVal;
}

const TCHAR* CvUnitInfo::getFutureArtDefineTag(int i, UnitArtStyleTypes eStyle) const
{
	FAssertMsg(i < getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if (NO_UNIT_ARTSTYLE != eStyle)
	{
		const int iIndex = GC.getInfoTypeForString(getType());
		if (-1 != iIndex)
		{
			const TCHAR* pcTag = GC.getUnitArtStyleTypeInfo(eStyle).getFutureArtDefineTag(i, iIndex);
			if (NULL != pcTag)
			{
				return pcTag;
			}
		}
	}

	return (m_paszFutureArtDefineTags) ? m_paszFutureArtDefineTags[i] : NULL;
}

void CvUnitInfo::setFutureArtDefineTag(int i, const TCHAR* szVal)
{
	FAssertMsg(i < getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	m_paszFutureArtDefineTags[i] = szVal;
}

const CvArtInfoUnit* CvUnitInfo::getArtInfo(int i, EraTypes eEra, UnitArtStyleTypes eStyle) const
{
	if ((eEra > 8) && (getFutureArtDefineTag(i, eStyle) != NULL) && !CvString(getFutureArtDefineTag(i, eStyle)).empty())
	{
		return ARTFILEMGR.getUnitArtInfo(getFutureArtDefineTag(i, eStyle));
	}
	else if ((eEra > 5) && (getLateArtDefineTag(i, eStyle) != NULL) && !CvString(getLateArtDefineTag(i, eStyle)).empty())
	{
		return ARTFILEMGR.getUnitArtInfo(getLateArtDefineTag(i, eStyle));
	}
	else if ((eEra > 4) && (getIndustrialArtDefineTag(i, eStyle) != NULL) && !CvString(getIndustrialArtDefineTag(i, eStyle)).empty())
	{
		return ARTFILEMGR.getUnitArtInfo(getIndustrialArtDefineTag(i, eStyle));
	}
	else if ((eEra > 3) && (getRennArtDefineTag(i, eStyle) != NULL) && !CvString(getRennArtDefineTag(i, eStyle)).empty())
	{
		return ARTFILEMGR.getUnitArtInfo(getRennArtDefineTag(i, eStyle));
	}
	else if ((eEra > 2) && (getMiddleArtDefineTag(i, eStyle) != NULL) && !CvString(getMiddleArtDefineTag(i, eStyle)).empty())
	{
		return ARTFILEMGR.getUnitArtInfo(getMiddleArtDefineTag(i, eStyle));
	}
	else if ((eEra > 1) && (getClassicalArtDefineTag(i, eStyle) != NULL) && !CvString(getClassicalArtDefineTag(i, eStyle)).empty())
	{
		return ARTFILEMGR.getUnitArtInfo(getClassicalArtDefineTag(i, eStyle));
	}
	else
	{
		return ARTFILEMGR.getUnitArtInfo(getEarlyArtDefineTag(i, eStyle));
	}
}

void CvUnitInfo::setPowerValue(int iNewValue)
{
	m_iPowerValue = iNewValue;
}

CvPropertyManipulators* CvUnitInfo::getPropertyManipulators()
{
	return &m_PropertyManipulators;
}

int CvUnitInfo::getPrereqVicinityBonus() const
{
	return m_iPrereqVicinityBonus;
}

int CvUnitInfo::getBaseFoodChange() const
{
	return m_iBaseFoodChange;
}

bool CvUnitInfo::isWorkerTrade() const
{
	return m_bWorkerTrade;
}

bool CvUnitInfo::isMilitaryTrade() const
{
	return m_bMilitaryTrade;
}

bool CvUnitInfo::isForceUpgrade() const
{
	return m_bForceUpgrade;
}

bool CvUnitInfo::isGreatGeneral() const
{
	return m_bGreatGeneral;
}

bool CvUnitInfo::isSlave() const
{
	return m_bSlave;
}

int CvUnitInfo::getControlPoints() const
{
	return m_iControlPoints;
}

int CvUnitInfo::getCommandRange() const
{
	return m_iCommandRange;
}

bool CvUnitInfo::isRequiresStateReligionInCity() const
{
	return m_bRequiresStateReligionInCity;
}

bool CvUnitInfo::getPassableRouteNeeded(int i) const
{
	FAssertMsg(i < GC.getNumRouteInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbPassableRouteNeeded ? m_pbPassableRouteNeeded[i] : false;
}

int CvUnitInfo::getPrereqOrVicinityBonuses(int i) const
{
	FAssertMsg(i < GC.getNUM_UNIT_PREREQ_OR_BONUSES(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piPrereqOrVicinityBonuses ? m_piPrereqOrVicinityBonuses[i] : -1;
}

CvWString CvUnitInfo::getCivilizationName(int i) const
{
	FAssertMsg(i < GC.getNumCivilizationInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paszCivilizationNames[i];
}

int CvUnitInfo::getCivilizationNamesVectorSize()					{return m_aszCivilizationNamesforPass3.size();}
CvWString CvUnitInfo::getCivilizationNamesNamesVectorElement(int i)	{return m_aszCivilizationNamesforPass3[i];}
CvWString CvUnitInfo::getCivilizationNamesValuesVectorElement(int i)		{return m_aszCivilizationNamesValueforPass3[i];}

// Sanguo Mod Performance start, added by poyuzhe 07.27.09
std::vector<int> CvUnitInfo::getUpgradeUnitClassTypes() const
{
	return m_aiUpgradeUnitClassTypes;
}

void CvUnitInfo::addUpgradeUnitClassTypes(int i)
{
	FAssert (i > -1 && i < GC.getNumUnitClassInfos());
	if (find(m_aiUpgradeUnitClassTypes.begin(), m_aiUpgradeUnitClassTypes.end(), i) == m_aiUpgradeUnitClassTypes.end())
	{
		m_aiUpgradeUnitClassTypes.push_back(i);
	}
}

bool CvUnitInfo::isUpgradeUnitClassTypes(int i)
{
	FAssert (i > -1 && i < GC.getNumUnitClassInfos());
	if (find(m_aiUpgradeUnitClassTypes.begin(), m_aiUpgradeUnitClassTypes.end(), i) == m_aiUpgradeUnitClassTypes.end())
	{
		return false;
	}
	return true;
}
// Sanguo Mod Performance, end
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

//TB Combat Mods Start  TB SubCombat Mod begin
//Functions

void CvUnitInfo::setReligionSubCombat()
{
	int iI;
	for (iI=0; iI < getNumSubCombatTypes(); iI++)
	{
		if (GC.getUnitCombatInfo((UnitCombatTypes)getSubCombatType(iI)).getReligion() != NO_RELIGION)
		{
			return;
		}
	}
	if (getReligionType() != NO_RELIGION)
	{
		for (iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
		{
			if (GC.getUnitCombatInfo((UnitCombatTypes)iI).getReligion() == (ReligionTypes)getReligionType())
			{
				m_aiSubCombatTypes.push_back(iI);
				break;
			}
		}
	}
	if (getStateReligion() != NO_RELIGION)
	{
		for (iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
		{
			if (GC.getUnitCombatInfo((UnitCombatTypes)iI).getReligion() == (ReligionTypes)getStateReligion())
			{
				m_aiSubCombatTypes.push_back(iI);
				break;
			}
		}
	}
	if (getPrereqReligion() != NO_RELIGION)
	{
		for (iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
		{
			if (GC.getUnitCombatInfo((UnitCombatTypes)iI).getReligion() == (ReligionTypes)getPrereqReligion())
			{
				m_aiSubCombatTypes.push_back(iI);
				break;
			}
		}
	}
}
void CvUnitInfo::setCultureSubCombat()
{
	int iI;
	for (iI=0; iI < getNumSubCombatTypes(); iI++)
	{
		if (GC.getUnitCombatInfo((UnitCombatTypes)getSubCombatType(iI)).getCulture() != NO_BONUS)
		{
			return;
		}
	}
	if (getPrereqAndBonus() != NO_BONUS && GC.getBonusInfo((BonusTypes)getPrereqAndBonus()).getBonusClassType() == GC.getInfoTypeForString("BONUSCLASS_CULTURE"))
	{
		for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
		{
			if (GC.getUnitCombatInfo((UnitCombatTypes)iI).getCulture() == (BonusTypes)getPrereqAndBonus())
			{
				m_aiSubCombatTypes.push_back(iI);
				break;
			}
		}
	}
}
int CvUnitInfo::getEraInfo() const
{
	if (getPrereqAndTech() != NO_TECH)
	{
		return GC.getTechInfo((TechTypes)getPrereqAndTech()).getEra();
	}
	else
	{
		TechTypes eHighestTech = NO_TECH;
		for ( int i = 0; i < GC.getNUM_UNIT_AND_TECH_PREREQS(); i++)
		{
			if ( getPrereqAndTechs(i) != NO_TECH )
			{
				if (eHighestTech == NO_TECH)
				{
					eHighestTech = (TechTypes)getPrereqAndTechs(i);
				}
				else if ((int)GC.getTechInfo((TechTypes)getPrereqAndTechs(i)).getEra() > (int)GC.getTechInfo(eHighestTech).getEra())
				{
					eHighestTech = (TechTypes)getPrereqAndTechs(i);
				}
			}
		}
		if (eHighestTech != NO_TECH)
		{
			return GC.getTechInfo(eHighestTech).getEra();
		}
	}
	return NO_ERA;
}
void CvUnitInfo::setEraSubCombat()
{
	if (getEraInfo() == NO_ERA)
	{
		return;
	}
	for (int iI = 0; iI < getNumSubCombatTypes(); iI++)
	{
		if (GC.getUnitCombatInfo((UnitCombatTypes)getSubCombatType(iI)).getEra() != NO_ERA)
		{
			return;
		}
	}
	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		if (GC.getUnitCombatInfo((UnitCombatTypes)iI).getEra() == getEraInfo())
		{
			m_aiSubCombatTypes.push_back(iI);
			break;
		}
	}
}
//integers
int CvUnitInfo::getAttackCombatModifier() const
{
	return m_iAttackCombatModifier;
}

int CvUnitInfo::getDefenseCombatModifier() const
{
	return m_iDefenseCombatModifier;
}

int CvUnitInfo::getPursuit(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_FIGHT_OR_FLIGHT))
	{
		return 0;
	}
	return m_iPursuit;
}

int CvUnitInfo::getEarlyWithdraw(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_FIGHT_OR_FLIGHT))
	{
		return 0;
	}
	return m_iEarlyWithdraw;
}

int CvUnitInfo::getVSBarbs() const
{
	return m_iVSBarbs;
}

int CvUnitInfo::getArmor() const
{
	return m_iArmor;
}

int CvUnitInfo::getPuncture() const
{
	return m_iPuncture;
}

int CvUnitInfo::getOverrun(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iOverrun;
}

int CvUnitInfo::getRepel(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iRepel;
}

int CvUnitInfo::getFortRepel(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iFortRepel;
}

int CvUnitInfo::getRepelRetries(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iRepelRetries;
}

int CvUnitInfo::getUnyielding(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iUnyielding;
}

int CvUnitInfo::getKnockback(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iKnockback;
}

int CvUnitInfo::getKnockbackRetries(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iKnockbackRetries;
}

int CvUnitInfo::getStrAdjperRnd(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_BATTLEWORN))
	{
		return 0;
	}
	return m_iStrAdjperRnd;
}

int CvUnitInfo::getStrAdjperAtt(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_BATTLEWORN))
	{
		return 0;
	}
	return m_iStrAdjperAtt;
}

int CvUnitInfo::getStrAdjperDef(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_BATTLEWORN))
	{
		return 0;
	}
	return m_iStrAdjperDef;
}

int CvUnitInfo::getWithdrawAdjperAtt(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_BATTLEWORN))
	{
		return 0;
	}
	return m_iWithdrawAdjperAtt;
}

int CvUnitInfo::getUnnerve(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SAD))
	{
		return 0;
	}
	return m_iUnnerve;
}

int CvUnitInfo::getEnclose(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SAD))
	{
		return 0;
	}
	return m_iEnclose;
}

int CvUnitInfo::getLunge(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SAD))
	{
		return 0;
	}
	return m_iLunge;
}

int CvUnitInfo::getDynamicDefense(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SAD))
	{
		return 0;
	}
	return m_iDynamicDefense;
}

int CvUnitInfo::getFortitude(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_OUTBREAKS_AND_AFFLICTIONS))
	{
		return 0;
	}
	return m_iFortitude;
}

int CvUnitInfo::getSupportPercent(SupportPositionTypes position, bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_aiSupportPercent[position];
}

int CvUnitInfo::getDodgeModifier() const
{
	return m_iDodgeModifier;
}

int CvUnitInfo::getPrecisionModifier() const
{
	return m_iPrecisionModifier;
}

int CvUnitInfo::getPowerShots() const
{
	return m_iPowerShots;
}

int CvUnitInfo::getPowerShotCombatModifier() const
{
	return m_iPowerShotCombatModifier;
}

int CvUnitInfo::getPowerShotPunctureModifier() const
{
	return m_iPowerShotPunctureModifier;
}

int CvUnitInfo::getPowerShotPrecisionModifier() const
{
	return m_iPowerShotPrecisionModifier;
}

int CvUnitInfo::getPowerShotCriticalModifier() const
{
	return m_iPowerShotCriticalModifier;
}

int CvUnitInfo::getCriticalModifier() const
{
	return m_iCriticalModifier;
}

int CvUnitInfo::getEndurance() const
{
	return m_iEndurance;
}

int CvUnitInfo::getRoundStunProb() const
{
	return m_iRoundStunProb;
}

int CvUnitInfo::getPoisonProbabilityModifier() const
{
	return m_iPoisonProbabilityModifier;
}

//Team Project (3)
int CvUnitInfo::getCaptureProbabilityModifier() const
{
	return m_iCaptureProbabilityModifier;
}

int CvUnitInfo::getCaptureResistanceModifier() const
{
	return m_iCaptureResistanceModifier;
}
//Team Project (4)
//WorkRateMod
int CvUnitInfo::getHillsWorkModifier() const
{
	return m_iHillsWorkModifier;
}

int CvUnitInfo::getPeaksWorkModifier() const
{
	return m_iPeaksWorkModifier;
}

int CvUnitInfo::getBreakdownChance() const
{
	return m_iBreakdownChance;
}

int CvUnitInfo::getBreakdownDamage() const
{
	return m_iBreakdownDamage;
}

int CvUnitInfo::getTaunt(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_FIGHT_OR_FLIGHT))
	{
		return 0;
	}
	return m_iTaunt;
}

int CvUnitInfo::getMaxHP(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		return 100;
	}
	if (m_iMaxHP == 0)
	{
		return 100;
	}
	return m_iMaxHP;
}

int CvUnitInfo::getDamageModifier() const
{
	return m_iDamageModifier;
}

int CvUnitInfo::getCostModifier() const
{
	return m_iCostModifier;
}

int CvUnitInfo::getRBombardDamage() const
{
	return m_iRBombardDamage;
}

int CvUnitInfo::getRBombardDamageLimit() const
{
	return m_iRBombardDamageLimit;
}

int CvUnitInfo::getRBombardDamageMaxUnits() const
{
	return m_iRBombardDamageMaxUnits;
}

int CvUnitInfo::getCombatModifierPerSizeMore(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerSizeMore;
}

int CvUnitInfo::getCombatModifierPerSizeLess(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerSizeLess;
}

int CvUnitInfo::getCombatModifierPerVolumeMore(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerVolumeMore;
}

int CvUnitInfo::getCombatModifierPerVolumeLess(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerVolumeLess;
}

int CvUnitInfo::getSelfHealModifier() const
{
	return m_iSelfHealModifier;
}

int CvUnitInfo::getNumHealSupport() const
{
	return m_iNumHealSupport;
}

int CvUnitInfo::getInsidiousness() const
{
	return m_iInsidiousness;
}

int CvUnitInfo::getInvestigation() const
{
	return m_iInvestigation;
}

int CvUnitInfo::getStealthStrikes(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_WITHOUT_WARNING))
	{
		return 0;
	}
	return m_iStealthStrikes;
}

int CvUnitInfo::getStealthCombatModifier(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_WITHOUT_WARNING))
	{
		return 0;
	}
	return m_iStealthCombatModifier;
}

int CvUnitInfo::getTrapDamageMax() const
{
	return m_iTrapDamageMax;
}

int CvUnitInfo::getTrapDamageMin() const
{
	return m_iTrapDamageMin;
}

int CvUnitInfo::getTrapComplexity() const
{
	return m_iTrapComplexity;
}

int CvUnitInfo::getNumTriggers() const
{
	return m_iNumTriggers;
}

int CvUnitInfo::getAggression() const
{
	return m_iAggression;
}

int CvUnitInfo::getAnimalIgnoresBorders() const
{
	return m_iAnimalIgnoresBorders;
}

int CvUnitInfo::getReligiousCombatModifier() const
{
	return m_iReligiousCombatModifier;
}

//booleans
bool CvUnitInfo::isStampede() const
{
	return m_bStampede;
}

bool CvUnitInfo::isOnslaught() const
{
	return m_bOnslaught;
}

bool CvUnitInfo::isDealsColdDamage() const
{
	return m_bDealsColdDamage;
}

bool CvUnitInfo::isColdImmune() const
{
	return m_bColdImmune;
}

bool CvUnitInfo::isAttackOnlyCities() const
{
	return m_bAttackOnlyCities;
}

bool CvUnitInfo::isIgnoreNoEntryLevel() const
{
	return m_bIgnoreNoEntryLevel;
}

bool CvUnitInfo::isIgnoreZoneofControl() const
{
	return m_bIgnoreZoneofControl;
}

bool CvUnitInfo::isFliesToMove() const
{
	return m_bFliesToMove;
}

bool CvUnitInfo::isRBombardForceAbility() const
{
	return m_bRBombardForceAbility;
}

bool CvUnitInfo::isNoSelfHeal() const
{
	return m_bNoSelfHeal;
}

bool CvUnitInfo::isExcile() const
{
	return m_bExcile;
}

bool CvUnitInfo::isPassage() const
{
	return m_bPassage;
}

bool CvUnitInfo::isNoNonOwnedCityEntry() const
{
	return m_bNoNonOwnedCityEntry;
}

bool CvUnitInfo::isBarbCoExist() const
{
	return m_bBarbCoExist;
}

bool CvUnitInfo::isBlendIntoCity() const
{
	return m_bBlendIntoCity;
}

bool CvUnitInfo::isUpgradeAnywhere() const
{
	return m_bUpgradeAnywhere;
}

bool CvUnitInfo::isAssassin() const
{
	return m_bAssassin;
}

bool CvUnitInfo::isStealthDefense(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_WITHOUT_WARNING))
	{
		return 0;
	}
	return m_bStealthDefense;
}

bool CvUnitInfo::isNoInvisibility() const
{
	return m_bNoInvisibility;
}

bool CvUnitInfo::isTriggerBeforeAttack() const
{
	return m_bTriggerBeforeAttack;
}

bool CvUnitInfo::isAnimal() const
{
	return hasUnitCombat(CvUnitCombatInfo::getUnitCombatAnimal());
}

bool CvUnitInfo::isWildAnimal() const
{
	return hasUnitCombat(CvUnitCombatInfo::getUnitCombatWildAnimal());
}

bool CvUnitInfo::canAnimalIgnoresBorders() const
{
	return m_bCanAnimalIgnoresBorders;
}

bool CvUnitInfo::canAnimalIgnoresImprovements() const
{
	return m_bCanAnimalIgnoresImprovements;
}

bool CvUnitInfo::canAnimalIgnoresCities() const
{
	return m_bCanAnimalIgnoresCities;
}

bool CvUnitInfo::isNoNonTypeProdMods() const
{
	return m_bNoNonTypeProdMods;
}

bool CvUnitInfo::isGatherHerd() const
{
	return m_bGatherHerd;
}


//boolean vectors without delayed resolution
int CvUnitInfo::getSubCombatType(int i) const
{
	return m_aiSubCombatTypes[i];
}

int CvUnitInfo::getNumSubCombatTypes() const
{
	return (int)m_aiSubCombatTypes.size();
}

bool CvUnitInfo::isSubCombatType(int i)
{
	FAssert (i > -1 && i < GC.getNumUnitCombatInfos());
	return find(m_aiSubCombatTypes.begin(), m_aiSubCombatTypes.end(), i) != m_aiSubCombatTypes.end();
}

std::vector<int>& CvUnitInfo::getSubCombatTypes()
{
	return m_aiSubCombatTypes;
}

int CvUnitInfo::getCureAfflictionType(int i) const
{
	return m_aiCureAfflictionTypes[i];
}

int CvUnitInfo::getNumCureAfflictionTypes() const
{
	return (int)m_aiCureAfflictionTypes.size();
}

bool CvUnitInfo::isCureAfflictionType(int i)
{
	FAssert (i > -1 && i < GC.getNumPromotionLineInfos());
	if (find(m_aiCureAfflictionTypes.begin(), m_aiCureAfflictionTypes.end(), i) == m_aiCureAfflictionTypes.end())
	{
		return false;
	}
	return true;
}

int CvUnitInfo::getHealAsType(int i) const
{
	return m_aiHealAsTypes[i];
}

int CvUnitInfo::getNumHealAsTypes() const
{
	return (int)m_aiHealAsTypes.size();
}

bool CvUnitInfo::isHealAsType(int i)
{
	FAssert (i > -1 && i < GC.getNumUnitCombatInfos());
	if (find(m_aiHealAsTypes.begin(), m_aiHealAsTypes.end(), i) == m_aiHealAsTypes.end())
	{
		return false;
	}
	return true;
}

void CvUnitInfo::setHealAsTypes()
{
	m_aiHealAsTypes.clear();
	for ( int i = 0; i < GC.getNumUnitCombatInfos(); i++)
	{
		if ((getUnitCombatType() == i || isSubCombatType(i)) && GC.getUnitCombatInfo((UnitCombatTypes)i).isHealsAs())
		{
			m_aiHealAsTypes.push_back((UnitCombatTypes)i);
		}
	}
}

int CvUnitInfo::getTerrainImpassableType(int i) const
{
	return m_aiTerrainImpassableTypes[i];
}

int CvUnitInfo::getNumTerrainImpassableTypes() const
{
	return (int)m_aiTerrainImpassableTypes.size();
}

bool CvUnitInfo::isTerrainImpassableType(int i)
{
	FAssert (i > -1 && i < GC.getNumTerrainInfos());
	if (find(m_aiTerrainImpassableTypes.begin(), m_aiTerrainImpassableTypes.end(), i) == m_aiTerrainImpassableTypes.end())
	{
		return false;
	}
	return true;
}

int CvUnitInfo::getFeatureImpassableType(int i) const
{
	return m_aiFeatureImpassableTypes[i];
}

int CvUnitInfo::getNumFeatureImpassableTypes() const
{
	return (int)m_aiFeatureImpassableTypes.size();
}

bool CvUnitInfo::isFeatureImpassableType(int i)
{
	FAssert (i > -1 && i < GC.getNumPromotionLineInfos());
	if (find(m_aiFeatureImpassableTypes.begin(), m_aiFeatureImpassableTypes.end(), i) == m_aiFeatureImpassableTypes.end())
	{
		return false;
	}
	return true;
}

int CvUnitInfo::getMapCategoryType(int i) const
{
	return m_aiMapCategoryTypes[i];
}

int CvUnitInfo::getNumMapCategoryTypes() const
{
	return (int)m_aiMapCategoryTypes.size();
}

bool CvUnitInfo::isMapCategoryType(int i)
{
	FAssert (i > -1 && i < GC.getNumMapCategoryInfos()); // do not include this line if for delayed resolution
	if (find(m_aiMapCategoryTypes.begin(), m_aiMapCategoryTypes.end(), i) == m_aiMapCategoryTypes.end())
	{
		return false;
	}
	return true;
}

int CvUnitInfo::getTrapSetWithPromotionType(int i) const
{
	return m_aiTrapSetWithPromotionTypes[i];
}

int CvUnitInfo::getNumTrapSetWithPromotionTypes() const
{
	return (int)m_aiTrapSetWithPromotionTypes.size();
}

bool CvUnitInfo::isTrapSetWithPromotionType(int i)
{
	FAssert (i > -1 && i < GC.getNumPromotionInfos()); // do not include this line if for delayed resolution
	if (find(m_aiTrapSetWithPromotionTypes.begin(), m_aiTrapSetWithPromotionTypes.end(), i) == m_aiTrapSetWithPromotionTypes.end())
	{
		return false;
	}
	return true;
}

int CvUnitInfo::getTrapImmunityUnitCombatType(int i) const
{
	return m_aiTrapImmunityUnitCombatTypes[i];
}

int CvUnitInfo::getNumTrapImmunityUnitCombatTypes() const
{
	return (int)m_aiTrapImmunityUnitCombatTypes.size();
}

bool CvUnitInfo::isTrapImmunityUnitCombatType(int i)
{
	FAssert (i > -1 && i < GC.getNumUnitCombatInfos()); // do not include this line if for delayed resolution
	if (find(m_aiTrapImmunityUnitCombatTypes.begin(), m_aiTrapImmunityUnitCombatTypes.end(), i) == m_aiTrapImmunityUnitCombatTypes.end())
	{
		return false;
	}
	return true;
}
//struct vectors
int CvUnitInfo::getNumAfflictionFortitudeModifiers() const
{
	return (int)m_aAfflictionFortitudeModifiers.size();
}

PromotionLineModifier& CvUnitInfo::getAfflictionFortitudeModifier(int iAffliction)
{
	FAssertMsg(iAffliction < (int)m_aAfflictionFortitudeModifiers.size(), "Index out of bounds");
	FAssertMsg(iAffliction > -1, "Index out of bounds");
	return m_aAfflictionFortitudeModifiers[iAffliction];
}

int CvUnitInfo::getNumAfflictOnAttackTypes() const
{
	return (int)m_aAfflictOnAttackTypes.size();
}

AfflictOnAttack& CvUnitInfo::getAfflictOnAttackType(int iAfflictionLine)
{
	FAssertMsg(iAfflictionLine < (int)m_aAfflictOnAttackTypes.size(), "Index out of bounds");
	FAssertMsg(iAfflictionLine > -1, "Index out of bounds");
	return m_aAfflictOnAttackTypes[iAfflictionLine];
}

int CvUnitInfo::getNumHealUnitCombatTypes() const
{
	return (int)m_aHealUnitCombatTypes.size();
}

HealUnitCombat& CvUnitInfo::getHealUnitCombatType(int iUnitCombat)
{
	FAssertMsg(iUnitCombat < (int)m_aHealUnitCombatTypes.size(), "Index out of bounds");
	FAssertMsg(iUnitCombat > -1, "Index out of bounds");
	return m_aHealUnitCombatTypes[iUnitCombat];
}

int CvUnitInfo::getNumGroupSpawnUnitCombatTypes() const
{
	return (int)m_aGroupSpawnUnitCombatTypes.size();
}

GroupSpawnUnitCombat& CvUnitInfo::getGroupSpawnUnitCombatType(int iIndex)
{
	return m_aGroupSpawnUnitCombatTypes[iIndex];
}

int CvUnitInfo::getNumInvisibleTerrainChanges() const
{
	return (int)m_aInvisibleTerrainChanges.size();
}

InvisibleTerrainChanges& CvUnitInfo::getInvisibleTerrainChange(int iIndex)
{
	return m_aInvisibleTerrainChanges[iIndex];
}

int CvUnitInfo::getNumInvisibleFeatureChanges() const
{
	return (int)m_aInvisibleFeatureChanges.size();
}

InvisibleFeatureChanges& CvUnitInfo::getInvisibleFeatureChange(int iIndex)
{
	return m_aInvisibleFeatureChanges[iIndex];
}

int CvUnitInfo::getNumInvisibleImprovementChanges() const
{
	return (int)m_aInvisibleImprovementChanges.size();
}

InvisibleImprovementChanges& CvUnitInfo::getInvisibleImprovementChange(int iIndex)
{
	return m_aInvisibleImprovementChanges[iIndex];
}

int CvUnitInfo::getNumVisibleTerrainChanges() const
{
	return (int)m_aVisibleTerrainChanges.size();
}

InvisibleTerrainChanges& CvUnitInfo::getVisibleTerrainChange(int iIndex)
{
	return m_aVisibleTerrainChanges[iIndex];
}

int CvUnitInfo::getNumVisibleFeatureChanges() const
{
	return (int)m_aVisibleFeatureChanges.size();
}

InvisibleFeatureChanges& CvUnitInfo::getVisibleFeatureChange(int iIndex)
{
	return m_aVisibleFeatureChanges[iIndex];
}

int CvUnitInfo::getNumVisibleImprovementChanges() const
{
	return (int)m_aVisibleImprovementChanges.size();
}

InvisibleImprovementChanges& CvUnitInfo::getVisibleImprovementChange(int iIndex)
{
	return m_aVisibleImprovementChanges[iIndex];
}

int CvUnitInfo::getNumVisibleTerrainRangeChanges() const
{
	return (int)m_aVisibleTerrainRangeChanges.size();
}

InvisibleTerrainChanges& CvUnitInfo::getVisibleTerrainRangeChange(int iIndex)
{
	return m_aVisibleTerrainRangeChanges[iIndex];
}

int CvUnitInfo::getNumVisibleFeatureRangeChanges() const
{
	return (int)m_aVisibleFeatureRangeChanges.size();
}

InvisibleFeatureChanges& CvUnitInfo::getVisibleFeatureRangeChange(int iIndex)
{
	return m_aVisibleFeatureRangeChanges[iIndex];
}

int CvUnitInfo::getNumVisibleImprovementRangeChanges() const
{
	return (int)m_aVisibleImprovementRangeChanges.size();
}

InvisibleImprovementChanges& CvUnitInfo::getVisibleImprovementRangeChange(int iIndex)
{
	return m_aVisibleImprovementRangeChanges[iIndex];
}

int CvUnitInfo::getNumDistanceAttackCommunicabilityTypeChanges() const
{
	return (int)m_aDistanceAttackCommunicabilityTypeChanges.size();
}

AfflictionLineChanges& CvUnitInfo::getDistanceAttackCommunicabilityTypeChange(int iIndex)
{
	return m_aDistanceAttackCommunicabilityTypeChanges[iIndex];
}

// bool vector utilizing delayed resolution
int CvUnitInfo::getNumEnabledCivilizationTypes() const
{
	return (int)m_aEnabledCivilizationTypes.size();
}

EnabledCivilizations& CvUnitInfo::getEnabledCivilizationType(int iIndex)
{
	return m_aEnabledCivilizationTypes[iIndex];
}

// int vectors utilizing pairing without delayed resolution
int CvUnitInfo::getNumFlankingStrikesbyUnitCombatTypes() const
{
	return m_aFlankingStrengthbyUnitCombatType.size();
}

int CvUnitInfo::getFlankingStrengthbyUnitCombatType(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aFlankingStrengthbyUnitCombatType.begin(); it != m_aFlankingStrengthbyUnitCombatType.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isFlankingStrikebyUnitCombatType(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aFlankingStrengthbyUnitCombatType.begin(); it != m_aFlankingStrengthbyUnitCombatType.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvUnitInfo::getNumWithdrawOnTerrainTypes() const
{
	return m_aWithdrawOnTerrainTypes.size();
}

int CvUnitInfo::getWithdrawOnTerrainType(int iTerrain) const
{
	for (TerrainModifierArray::const_iterator it = m_aWithdrawOnTerrainTypes.begin(); it != m_aWithdrawOnTerrainTypes.end(); ++it)
	{
		if ((*it).first == (TerrainTypes)iTerrain)
		{
			return (*it).second;
		}
	}
	return 0;
}

int CvUnitInfo::getNumWithdrawOnFeatureTypes() const
{
	return m_aWithdrawOnFeatureTypes.size();
}

int CvUnitInfo::getWithdrawOnFeatureType(int iFeature) const
{
	for (FeatureModifierArray::const_iterator it = m_aWithdrawOnFeatureTypes.begin(); it != m_aWithdrawOnFeatureTypes.end(); ++it)
	{
		if ((*it).first == (FeatureTypes)iFeature)
		{
			return (*it).second;
		}
	}
	return 0;
}

int CvUnitInfo::getNumWithdrawVSUnitCombatTypes() const
{
	return m_aWithdrawVSUnitCombatTypes.size();
}

int CvUnitInfo::getWithdrawVSUnitCombatType(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aWithdrawVSUnitCombatTypes.begin(); it != m_aWithdrawVSUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isWithdrawVSUnitCombatType(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aWithdrawVSUnitCombatTypes.begin(); it != m_aWithdrawVSUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

UnitCombatModifierArray& CvUnitInfo::getWithdrawVSUnitCombatTypes()
{
	return m_aWithdrawVSUnitCombatTypes;
}

int CvUnitInfo::getNumPursuitVSUnitCombatTypes() const
{
	return m_aPursuitVSUnitCombatTypes.size();
}

int CvUnitInfo::getPursuitVSUnitCombatType(int iUnitCombat, bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_FIGHT_OR_FLIGHT))
	{
		return 0;
	}
	for (UnitCombatModifierArray::const_iterator it = m_aPursuitVSUnitCombatTypes.begin(); it != m_aPursuitVSUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isPursuitVSUnitCombatType(int iUnitCombat, bool bForLoad)
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_FIGHT_OR_FLIGHT))
	{
		return false;
	}
	for (UnitCombatModifierArray::const_iterator it = m_aPursuitVSUnitCombatTypes.begin(); it != m_aPursuitVSUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

UnitCombatModifierArray& CvUnitInfo::getPursuitVSUnitCombatTypes()
{
	return m_aPursuitVSUnitCombatTypes;
}

int CvUnitInfo::getNumRepelVSUnitCombatTypes() const
{
	return m_aRepelVSUnitCombatTypes.size();
}

int CvUnitInfo::getRepelVSUnitCombatType(int iUnitCombat, bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	for (UnitCombatModifierArray::const_iterator it = m_aRepelVSUnitCombatTypes.begin(); it != m_aRepelVSUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isRepelVSUnitCombatType(int iUnitCombat, bool bForLoad)
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return false;
	}
	for (UnitCombatModifierArray::const_iterator it = m_aRepelVSUnitCombatTypes.begin(); it != m_aRepelVSUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

UnitCombatModifierArray& CvUnitInfo::getRepelVSUnitCombatTypes()
{
	return m_aRepelVSUnitCombatTypes;
}

int CvUnitInfo::getNumKnockbackVSUnitCombatTypes() const
{
	return m_aKnockbackVSUnitCombatTypes.size();
}

int CvUnitInfo::getKnockbackVSUnitCombatType(int iUnitCombat, bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	for (UnitCombatModifierArray::const_iterator it = m_aKnockbackVSUnitCombatTypes.begin(); it != m_aKnockbackVSUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isKnockbackVSUnitCombatType(int iUnitCombat, bool bForLoad)
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return false;
	}
	for (UnitCombatModifierArray::const_iterator it = m_aKnockbackVSUnitCombatTypes.begin(); it != m_aKnockbackVSUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

UnitCombatModifierArray& CvUnitInfo::getKnockbackVSUnitCombatTypes()
{
	return m_aKnockbackVSUnitCombatTypes;
}

int CvUnitInfo::getNumPunctureVSUnitCombatTypes() const
{
	return m_aPunctureVSUnitCombatTypes.size();
}

int CvUnitInfo::getPunctureVSUnitCombatType(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aPunctureVSUnitCombatTypes.begin(); it != m_aPunctureVSUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isPunctureVSUnitCombatType(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aPunctureVSUnitCombatTypes.begin(); it != m_aPunctureVSUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

UnitCombatModifierArray& CvUnitInfo::getPunctureVSUnitCombatTypes()
{
	return m_aPunctureVSUnitCombatTypes;
}

int CvUnitInfo::getNumArmorVSUnitCombatTypes() const
{
	return m_aArmorVSUnitCombatTypes.size();
}

int CvUnitInfo::getArmorVSUnitCombatType(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aArmorVSUnitCombatTypes.begin(); it != m_aArmorVSUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isArmorVSUnitCombatType(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aArmorVSUnitCombatTypes.begin(); it != m_aArmorVSUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

UnitCombatModifierArray& CvUnitInfo::getArmorVSUnitCombatTypes()
{
	return m_aArmorVSUnitCombatTypes;
}

int CvUnitInfo::getNumDodgeVSUnitCombatTypes() const
{
	return m_aDodgeVSUnitCombatTypes.size();
}

int CvUnitInfo::getDodgeVSUnitCombatType(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aDodgeVSUnitCombatTypes.begin(); it != m_aDodgeVSUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isDodgeVSUnitCombatType(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aDodgeVSUnitCombatTypes.begin(); it != m_aDodgeVSUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

UnitCombatModifierArray& CvUnitInfo::getDodgeVSUnitCombatTypes()
{
	return m_aDodgeVSUnitCombatTypes;
}

int CvUnitInfo::getNumPrecisionVSUnitCombatTypes() const
{
	return m_aPrecisionVSUnitCombatTypes.size();
}

int CvUnitInfo::getPrecisionVSUnitCombatType(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aPrecisionVSUnitCombatTypes.begin(); it != m_aPrecisionVSUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isPrecisionVSUnitCombatType(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aPrecisionVSUnitCombatTypes.begin(); it != m_aPrecisionVSUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

UnitCombatModifierArray& CvUnitInfo::getPrecisionVSUnitCombatTypes()
{
	return m_aPrecisionVSUnitCombatTypes;
}

int CvUnitInfo::getNumCriticalVSUnitCombatTypes() const
{
	return m_aCriticalVSUnitCombatTypes.size();
}

int CvUnitInfo::getCriticalVSUnitCombatType(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aCriticalVSUnitCombatTypes.begin(); it != m_aCriticalVSUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isCriticalVSUnitCombatType(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aCriticalVSUnitCombatTypes.begin(); it != m_aCriticalVSUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

UnitCombatModifierArray& CvUnitInfo::getCriticalVSUnitCombatTypes()
{
	return m_aCriticalVSUnitCombatTypes;
}

int CvUnitInfo::getNumRoundStunVSUnitCombatTypes() const
{
	return m_aRoundStunVSUnitCombatTypes.size();
}

int CvUnitInfo::getRoundStunVSUnitCombatType(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aRoundStunVSUnitCombatTypes.begin(); it != m_aRoundStunVSUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isRoundStunVSUnitCombatType(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aRoundStunVSUnitCombatTypes.begin(); it != m_aRoundStunVSUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

UnitCombatModifierArray& CvUnitInfo::getRoundStunVSUnitCombatTypes()
{
	return m_aRoundStunVSUnitCombatTypes;
}

int CvUnitInfo::getNumTrapDisableUnitCombatTypes() const
{
	return m_aTrapDisableUnitCombatTypes.size();
}

int CvUnitInfo::getTrapDisableUnitCombatType(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aTrapDisableUnitCombatTypes.begin(); it != m_aTrapDisableUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isTrapDisableUnitCombatType(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aTrapDisableUnitCombatTypes.begin(); it != m_aTrapDisableUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

UnitCombatModifierArray& CvUnitInfo::getTrapDisableUnitCombatTypes()
{
	return m_aTrapDisableUnitCombatTypes;
}

int CvUnitInfo::getNumTrapAvoidanceUnitCombatTypes() const
{
	return m_aTrapAvoidanceUnitCombatTypes.size();
}

int CvUnitInfo::getTrapAvoidanceUnitCombatType(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aTrapAvoidanceUnitCombatTypes.begin(); it != m_aTrapAvoidanceUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isTrapAvoidanceUnitCombatType(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aTrapAvoidanceUnitCombatTypes.begin(); it != m_aTrapAvoidanceUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

UnitCombatModifierArray& CvUnitInfo::getTrapAvoidanceUnitCombatTypes()
{
	return m_aTrapAvoidanceUnitCombatTypes;
}

int CvUnitInfo::getNumTrapTriggerUnitCombatTypes() const
{
	return m_aTrapTriggerUnitCombatTypes.size();
}

int CvUnitInfo::getTrapTriggerUnitCombatType(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aTrapTriggerUnitCombatTypes.begin(); it != m_aTrapTriggerUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isTrapTriggerUnitCombatType(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aTrapTriggerUnitCombatTypes.begin(); it != m_aTrapTriggerUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

UnitCombatModifierArray& CvUnitInfo::getTrapTriggerUnitCombatTypes()
{
	return m_aTrapTriggerUnitCombatTypes;
}

int CvUnitInfo::getNumVisibilityIntensityTypes() const
{
	return m_aVisibilityIntensityTypes.size();
}

int CvUnitInfo::getVisibilityIntensityType(int iInvisibility) const
{
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensityTypes.begin(); it != m_aVisibilityIntensityTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isVisibilityIntensityType(int iInvisibility)
{
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensityTypes.begin(); it != m_aVisibilityIntensityTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return true;
		}
	}
	return false;
}

InvisibilityArray& CvUnitInfo::getVisibilityIntensityTypes()
{
	return m_aVisibilityIntensityTypes;
}

int CvUnitInfo::getNumInvisibilityIntensityTypes() const
{
	return m_aInvisibilityIntensityTypes.size();
}

int CvUnitInfo::getInvisibilityIntensityType(int iInvisibility) const
{
	for (InvisibilityArray::const_iterator it = m_aInvisibilityIntensityTypes.begin(); it != m_aInvisibilityIntensityTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isInvisibilityIntensityType(int iInvisibility)
{
	for (InvisibilityArray::const_iterator it = m_aInvisibilityIntensityTypes.begin(); it != m_aInvisibilityIntensityTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return true;
		}
	}
	return false;
}

InvisibilityArray& CvUnitInfo::getInvisibilityIntensityTypes()
{
	return m_aInvisibilityIntensityTypes;
}

int CvUnitInfo::getNumVisibilityIntensityRangeTypes() const
{
	return m_aVisibilityIntensityRangeTypes.size();
}

int CvUnitInfo::getVisibilityIntensityRangeType(int iInvisibility) const
{
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensityRangeTypes.begin(); it != m_aVisibilityIntensityRangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isVisibilityIntensityRangeType(int iInvisibility)
{
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensityRangeTypes.begin(); it != m_aVisibilityIntensityRangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return true;
		}
	}
	return false;
}

InvisibilityArray& CvUnitInfo::getVisibilityIntensityRangeTypes()
{
	return m_aVisibilityIntensityRangeTypes;
}

//Team Project (4)
	//WorkRateMod
int CvUnitInfo::getNumTerrainWorkRateModifierTypes() const
{
	return m_aTerrainWorkRateModifierTypes.size();
}

int CvUnitInfo::getTerrainWorkRateModifierType(int iTerrain) const
{
	for (TerrainModifierArray::const_iterator it = m_aTerrainWorkRateModifierTypes.begin(); it != m_aTerrainWorkRateModifierTypes.end(); ++it)
	{
		if ((*it).first == (TerrainTypes)iTerrain)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isTerrainWorkRateModifierType(int iTerrain)
{
	for (TerrainModifierArray::const_iterator it = m_aTerrainWorkRateModifierTypes.begin(); it != m_aTerrainWorkRateModifierTypes.end(); ++it)
	{
		if ((*it).first == (TerrainTypes)iTerrain)
		{
			return true;
		}
	}
	return false;
}

int CvUnitInfo::getNumFeatureWorkRateModifierTypes() const
{
	return m_aFeatureWorkRateModifierTypes.size();
}

int CvUnitInfo::getFeatureWorkRateModifierType(int iFeature) const
{
	for (FeatureModifierArray::const_iterator it = m_aFeatureWorkRateModifierTypes.begin(); it != m_aFeatureWorkRateModifierTypes.end(); ++it)
	{
		if ((*it).first == (FeatureTypes)iFeature)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isFeatureWorkRateModifierType(int iFeature)
{
	for (FeatureModifierArray::const_iterator it = m_aFeatureWorkRateModifierTypes.begin(); it != m_aFeatureWorkRateModifierTypes.end(); ++it)
	{
		if ((*it).first == (FeatureTypes)iFeature)
		{
			return true;
		}
	}
	return false;
}

int CvUnitInfo::getNumBuildWorkRateModifierTypes() const
{
	return m_aBuildWorkRateModifierTypes.size();
}

int CvUnitInfo::getBuildWorkRateModifierType(int iBuild) const
{
	for (BuildModifierArray::const_iterator it = m_aBuildWorkRateModifierTypes.begin(); it != m_aBuildWorkRateModifierTypes.end(); ++it)
	{
		if ((*it).first == (BuildTypes)iBuild)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isBuildWorkRateModifierType(int iBuild)
{
	for (BuildModifierArray::const_iterator it = m_aBuildWorkRateModifierTypes.begin(); it != m_aBuildWorkRateModifierTypes.end(); ++it)
	{
		if ((*it).first == (BuildTypes)iBuild)
		{
			return true;
		}
	}
	return false;
}

int CvUnitInfo::getNumAidChanges() const
{
	return m_aAidChanges.size();
}

int CvUnitInfo::getAidChange(int iProperty) const
{
	for (AidArray::const_iterator it = m_aAidChanges.begin(); it != m_aAidChanges.end(); ++it)
	{
		if ((*it).first == (PropertyTypes)iProperty)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitInfo::isAidChange(int iProperty)
{
	for (AidArray::const_iterator it = m_aAidChanges.begin(); it != m_aAidChanges.end(); ++it)
	{
		if ((*it).first == (PropertyTypes)iProperty)
		{
			return true;
		}
	}
	return false;
}
//TB Combat Mods End  TB SubCombat Mod end

void CvUnitInfo::getCheckSum(unsigned int &iSum)
{
	CheckSum(iSum, m_iDCMBombRange);
	CheckSum(iSum, m_iDCMBombAccuracy);
	CheckSum(iSum, m_bDCMAirBomb1);
	CheckSum(iSum, m_bDCMAirBomb2);
	CheckSum(iSum, m_bDCMAirBomb3);
	CheckSum(iSum, m_bDCMAirBomb4);
	CheckSum(iSum, m_bDCMAirBomb5);
	CheckSum(iSum, m_bDCMFighterEngage);

	CheckSum(iSum, m_iAIWeight);
	CheckSum(iSum, m_iProductionCost);
	CheckSum(iSum, m_iHurryCostModifier);
	CheckSum(iSum, m_iAdvancedStartCost);
	CheckSum(iSum, m_iAdvancedStartCostIncrease);
	CheckSum(iSum, m_iMinAreaSize);
	CheckSum(iSum, m_iMoves);
	CheckSum(iSum, m_iAirRange);
	CheckSum(iSum, m_iAirUnitCap);
	CheckSum(iSum, m_iDropRange);
	CheckSum(iSum, m_iNukeRange);
	CheckSum(iSum, m_iWorkRate);
	CheckSum(iSum, m_iBaseDiscover);
	CheckSum(iSum, m_iDiscoverMultiplier);
	CheckSum(iSum, m_iBaseHurry);
	CheckSum(iSum, m_iHurryMultiplier);
	CheckSum(iSum, m_iBaseTrade);
	CheckSum(iSum, m_iTradeMultiplier);
	CheckSum(iSum, m_iGreatWorkCulture);
	CheckSum(iSum, m_iEspionagePoints);
	CheckSum(iSum, m_iCombat);
	CheckSum(iSum, m_iCombatLimit);
	CheckSum(iSum, m_iAirCombat);
	CheckSum(iSum, m_iAirCombatLimit);
	CheckSum(iSum, m_iXPValueAttack);
	CheckSum(iSum, m_iXPValueDefense);
	CheckSum(iSum, m_iFirstStrikes);
	CheckSum(iSum, m_iChanceFirstStrikes);
	CheckSum(iSum, m_iInterceptionProbability);
	//ls612: Advanced Nuke Interception
	//CheckSum(iSum, m_iNukeInterceptionProbability);
	//CheckSum(iSum, m_iNukeInterceptionRange);
	CheckSum(iSum, m_iEvasionProbability);
	CheckSum(iSum, m_iWithdrawalProbability);
	CheckSum(iSum, m_iCollateralDamage);
	CheckSum(iSum, m_iCollateralDamageLimit);
	CheckSum(iSum, m_iCollateralDamageMaxUnits);
	CheckSum(iSum, m_iCityAttackModifier);
	CheckSum(iSum, m_iCityDefenseModifier);
	CheckSum(iSum, m_iAnimalCombatModifier);
	CheckSum(iSum, m_iHillsAttackModifier);
	CheckSum(iSum, m_iHillsDefenseModifier);
	CheckSum(iSum, m_iBombRate);
	CheckSum(iSum, m_iBombardRate);
	CheckSum(iSum, m_iSpecialCargo);
	CheckSum(iSum, m_iSMSpecialCargo);
	CheckSum(iSum, m_iSMNotSpecialCargo);
	CheckSum(iSum, m_iDomainCargo);
	CheckSum(iSum, m_iCargoSpace);
	CheckSum(iSum, m_iSMCargoSpace);
	CheckSum(iSum, m_iSMCargoVolume);
	CheckSum(iSum, m_iConscriptionValue);
	CheckSum(iSum, m_iCultureGarrisonValue);
	CheckSum(iSum, m_iExtraCost);
	CheckSum(iSum, m_iAssetValue);
	CheckSum(iSum, m_iPowerValue);
	CheckSum(iSum, m_iUnitClassType);
	CheckSum(iSum, m_iSpecialUnitType);
	CheckSum(iSum, m_iUnitCaptureClassType);
	CheckSum(iSum, m_iUnitCombatType);
	CheckSum(iSum, m_iDomainType);
	CheckSum(iSum, m_iDefaultUnitAIType);
	CheckSum(iSum, m_iInvisibleType);

	CheckSumC(iSum, m_aiSeeInvisibleTypes);

	CheckSum(iSum, m_iAdvisorType);

	CheckSum(iSum, m_iMaxStartEra);
	CheckSum(iSum, m_iForceObsoleteTech);
	CheckSum(iSum, m_bStateReligion);
	CheckSum(iSum, m_iPrereqGameOption);
	CheckSum(iSum, m_iNotGameOption);

	CheckSum(iSum, m_iHolyCity);
	CheckSum(iSum, m_iReligionType);
	CheckSum(iSum, m_iStateReligion);
	CheckSum(iSum, m_iPrereqReligion);
	CheckSum(iSum, m_iPrereqCorporation);
	CheckSum(iSum, m_iPrereqBuilding);

	CheckSumC(iSum, m_aePrereqOrBuildings);

	CheckSum(iSum, m_iPrereqAndTech);
	CheckSum(iSum, m_iPrereqAndBonus);
	CheckSum(iSum, m_iGroupSize);
	CheckSum(iSum, m_iGroupDefinitions);
	CheckSum(iSum, m_iUnitMeleeWaveSize);
	CheckSum(iSum, m_iUnitRangedWaveSize);
	CheckSum(iSum, m_iNumUnitNames);
	CheckSum(iSum, m_iCommandType);

	CheckSum(iSum, m_bFoodProduction);
	CheckSum(iSum, m_bNoBadGoodies);
	CheckSum(iSum, m_bOnlyDefensive);
	CheckSum(iSum, m_bNoCapture);
	CheckSum(iSum, m_bQuickCombat);
	CheckSum(iSum, m_bRivalTerritory);
	CheckSum(iSum, m_bMilitaryHappiness);
	CheckSum(iSum, m_bMilitarySupport);
	CheckSum(iSum, m_bMilitaryProduction);
	CheckSum(iSum, m_bPillage);
	CheckSum(iSum, m_bSpy);
	CheckSum(iSum, m_bSabotage);
	CheckSum(iSum, m_bDestroy);
	CheckSum(iSum, m_bStealPlans);
	CheckSum(iSum, m_bInvestigate);
	CheckSum(iSum, m_bCounterSpy);
	CheckSum(iSum, m_bFound);
	CheckSum(iSum, m_bGoldenAge);
	CheckSum(iSum, m_bInvisible);
	CheckSum(iSum, m_bFirstStrikeImmune);
	CheckSum(iSum, m_bNoDefensiveBonus);
	CheckSum(iSum, m_bIgnoreBuildingDefense);
	CheckSum(iSum, m_bCanMoveImpassable);
	CheckSum(iSum, m_bCanMoveAllTerrain);
	CheckSum(iSum, m_bFlatMovementCost);
	CheckSum(iSum, m_bIgnoreTerrainCost);
	CheckSum(iSum, m_bNukeImmune);
	CheckSum(iSum, m_bPrereqBonuses);
	CheckSum(iSum, m_bPrereqReligion);
	CheckSum(iSum, m_bMechanized);
	CheckSum(iSum, m_bRenderBelowWater);
	CheckSum(iSum, m_bRenderAlways);
	CheckSum(iSum, m_bSuicide);
	CheckSum(iSum, m_bLineOfSight);
	CheckSum(iSum, m_bHiddenNationality);
	CheckSum(iSum, m_bAlwaysHostile);
/*****************************************************************************************************/
/**  Author: TheLadiesOgre																		  **/
/**  Date: 21.09.2009																			   **/
/**  ModComp: TLOTags																			   **/
/**  Reason Added: New Tag Definition															   **/
/**  Notes:																						 **/
/*****************************************************************************************************/
	CheckSum(iSum, m_bFreeDrop);
/*****************************************************************************************************/
/**  TheLadiesOgre; 21.09.2009; TLOTags															 **/
/*****************************************************************************************************/
	CheckSum(iSum, m_bNoRevealMap);

	CheckSum(iSum, m_bInquisitor);
	CheckSum(iSum, m_bCanBeRebel);
	CheckSum(iSum, m_bCanRebelCapture);
	CheckSum(iSum, m_bCannotDefect);
	CheckSum(iSum, m_bCanQuellRebellion);

	CheckSum(iSum, m_bNoNonOwnedEntry);


	//CheckSum(iSum, m_fUnitMaxSpeed);
	//CheckSum(iSum, m_fUnitPadTime);

	CheckSumI(iSum, GC.getNUM_UNIT_AND_TECH_PREREQS(), m_piPrereqAndTechs);
	CheckSumI(iSum, GC.getNUM_UNIT_PREREQ_OR_BONUSES(), m_piPrereqOrBonuses);
	//CheckSumI(iSum, GC.getNumTraitInfos(), m_piProductionTraits);
	CheckSumI(iSum, GC.getNumFlavorTypes(), m_piFlavorValue);
	CheckSumI(iSum, GC.getNumTerrainInfos(), m_piTerrainAttackModifier);
	CheckSumI(iSum, GC.getNumTerrainInfos(), m_piTerrainDefenseModifier);
	CheckSumI(iSum, GC.getNumFeatureInfos(), m_piFeatureAttackModifier);
	CheckSumI(iSum, GC.getNumFeatureInfos(), m_piFeatureDefenseModifier);
	CheckSumI(iSum, GC.getNumUnitClassInfos(), m_piUnitClassAttackModifier);
	CheckSumI(iSum, GC.getNumUnitClassInfos(), m_piUnitClassDefenseModifier);
	CheckSumI(iSum, GC.getNumUnitCombatInfos(), m_piUnitCombatModifier);
	CheckSumI(iSum, GC.getNumUnitCombatInfos(), m_piUnitCombatCollateralImmune);
	CheckSumI(iSum, NUM_DOMAIN_TYPES, m_piDomainModifier);
	CheckSumI(iSum, GC.getNumBonusInfos(), m_piBonusProductionModifier);
	//CheckSumI(iSum, m_iGroupDefinitions, m_piUnitGroupRequired);

	CheckSumI(iSum, GC.getNumCivicInfos(), m_pbPrereqOrCivics);
	CheckSumI(iSum, GC.getNumBuildingClassInfos(), m_pbPrereqBuildingClass);
	CheckSumI(iSum, GC.getNumBuildingClassInfos(), m_piPrereqBuildingClassOverrideTech);
	CheckSumI(iSum, GC.getNumBuildingClassInfos(), m_piPrereqBuildingClassOverrideEra);
	CheckSumI(iSum, GC.getNumUnitClassInfos(), m_pbForceObsoleteUnitClass);
	CheckSumI(iSum, GC.getNumUnitClassInfos(), m_pbUpgradeUnitClass);
	CheckSumI(iSum, GC.getNumUnitClassInfos(), m_pbTargetUnitClass);
	CheckSumI(iSum, GC.getNumUnitCombatInfos(), m_pbTargetUnitCombat);
	CheckSumI(iSum, GC.getNumUnitClassInfos(), m_pbDefenderUnitClass);
	CheckSumI(iSum, GC.getNumUnitCombatInfos(), m_pbDefenderUnitCombat);
	CheckSumI(iSum, GC.getNumUnitClassInfos(), m_piFlankingStrikeUnitClass);
	CheckSumI(iSum, NUM_UNITAI_TYPES, m_pbUnitAIType);
	CheckSumI(iSum, NUM_UNITAI_TYPES, m_pbNotUnitAIType);
	CheckSumI(iSum, GC.getNumBuildInfos(), m_pbBuilds);
	CheckSumI(iSum, GC.getNumReligionInfos(), m_piReligionSpreads);
	CheckSumI(iSum, GC.getNumCorporationInfos(), m_piCorporationSpreads);
	CheckSumI(iSum, GC.getNumTerrainInfos(), m_piTerrainPassableTech);
	CheckSumI(iSum, GC.getNumFeatureInfos(), m_piFeaturePassableTech);
	CheckSumI(iSum, GC.getNumSpecialistInfos(), m_pbGreatPeoples);
	CheckSumC(iSum, m_pbBuildings);
	CheckSumI(iSum, GC.getNumTerrainInfos(), m_pbTerrainNative);
	CheckSumI(iSum, GC.getNumFeatureInfos(), m_pbFeatureNative);
	//CheckSumI(iSum, GC.getNumTerrainInfos(), m_pbTerrainImpassable);
	//CheckSumI(iSum, GC.getNumFeatureInfos(), m_pbFeatureImpassable);
	CheckSumI(iSum, GC.getNumPromotionInfos(), m_pbFreePromotions);

	CheckSum(iSum, m_iLeaderPromotion);
	CheckSum(iSum, m_iLeaderExperience);

	CheckSum(iSum, m_bRequiresStateReligionInCity);
	CheckSum(iSum, m_bWorkerTrade);
	CheckSum(iSum, m_bMilitaryTrade);
	CheckSum(iSum, m_bForceUpgrade);
	CheckSum(iSum, m_bGreatGeneral);
	CheckSum(iSum, m_bSlave);
	CheckSum(iSum, m_iPrereqVicinityBonus);
	CheckSum(iSum, m_iBaseFoodChange);
	CheckSum(iSum, m_iControlPoints);
	CheckSum(iSum, m_iCommandRange);

	CheckSumI(iSum, GC.getNUM_UNIT_PREREQ_OR_BONUSES(), m_piPrereqOrVicinityBonuses);
	CheckSumI(iSum, GC.getNumBuildingClassInfos(), m_pbPrereqBuildingClass);
	CheckSumI(iSum, GC.getNumRouteInfos(), m_pbPassableRouteNeeded);

	CheckSumC(iSum, m_aiUpgradeUnitClassTypes);

	getKillOutcomeList()->getCheckSum(iSum);

	for (int i=0; i<(int)m_aOutcomeMissions.size(); i++)
	{
		m_aOutcomeMissions[i]->getCheckSum(iSum);
	}

	m_PropertyManipulators.getCheckSum(iSum);

	if (m_pExprTrainCondition)
	{
		m_pExprTrainCondition->getCheckSum(iSum);
	}
	//TB Combat Mods Begin  TB SubCombat Mod begin
	//integers
	CheckSum(iSum, m_iAttackCombatModifier);
	CheckSum(iSum, m_iDefenseCombatModifier);
	CheckSum(iSum, m_iPursuit);
	CheckSum(iSum, m_iEarlyWithdraw);
	CheckSum(iSum, m_iVSBarbs);
	CheckSum(iSum, m_iArmor);
	CheckSum(iSum, m_iPuncture);
	CheckSum(iSum, m_iOverrun);
	CheckSum(iSum, m_iRepel);
	CheckSum(iSum, m_iFortRepel);
	CheckSum(iSum, m_iRepelRetries);
	CheckSum(iSum, m_iUnyielding);
	CheckSum(iSum, m_iKnockback);
	CheckSum(iSum, m_iKnockbackRetries);
	CheckSum(iSum, m_iStrAdjperRnd);
	CheckSum(iSum, m_iStrAdjperAtt);
	CheckSum(iSum, m_iStrAdjperDef);
	CheckSum(iSum, m_iWithdrawAdjperAtt);
	CheckSum(iSum, m_iUnnerve);
	CheckSum(iSum, m_iEnclose);
	CheckSum(iSum, m_iLunge);
	CheckSum(iSum, m_iDynamicDefense);
	CheckSum(iSum, m_iFortitude);
	CheckSumI(iSum, NUM_SUPPORT_POSITION_TYPES, m_aiSupportPercent);

	CheckSum(iSum, m_iFrontSupportPercent);
	CheckSum(iSum, m_iShortRangeSupportPercent);
	CheckSum(iSum, m_iMediumRangeSupportPercent);
	CheckSum(iSum, m_iLongRangeSupportPercent);
	CheckSum(iSum, m_iFlankSupportPercent);

	CheckSum(iSum, m_iDodgeModifier);
	CheckSum(iSum, m_iPrecisionModifier);
	CheckSum(iSum, m_iPowerShots);
	CheckSum(iSum, m_iPowerShotCombatModifier);
	CheckSum(iSum, m_iPowerShotPunctureModifier);
	CheckSum(iSum, m_iPowerShotPrecisionModifier);
	CheckSum(iSum, m_iPowerShotCriticalModifier);
	CheckSum(iSum, m_iCriticalModifier);
	CheckSum(iSum, m_iEndurance);
	CheckSum(iSum, m_iRoundStunProb);
	CheckSum(iSum, m_iPoisonProbabilityModifier);
//Team Project (3)
	CheckSum(iSum, m_iCaptureProbabilityModifier);
	CheckSum(iSum, m_iCaptureResistanceModifier);
//Team Project (4)
//WorkRateMod
	CheckSum(iSum, m_iHillsWorkModifier);
	CheckSum(iSum, m_iPeaksWorkModifier);

	CheckSum(iSum, m_iBreakdownChance);
	CheckSum(iSum, m_iBreakdownDamage);
	CheckSum(iSum, m_iTaunt);
	CheckSum(iSum, m_iMaxHP);
	CheckSum(iSum, m_iDamageModifier);
	CheckSum(iSum, m_iCostModifier);
	CheckSum(iSum, m_iTotalCombatStrengthModifierBase);
	CheckSum(iSum, m_iTotalCombatStrengthChangeBase);
	CheckSum(iSum, m_iBaseCargoVolume);
	CheckSum(iSum, m_iRBombardDamage);
	CheckSum(iSum, m_iRBombardDamageLimit);
	CheckSum(iSum, m_iRBombardDamageMaxUnits);
	CheckSum(iSum, m_iBaseGroupRank);
	CheckSum(iSum, m_iCombatModifierPerSizeMore);
	CheckSum(iSum, m_iCombatModifierPerSizeLess);
	CheckSum(iSum, m_iCombatModifierPerVolumeMore);
	CheckSum(iSum, m_iCombatModifierPerVolumeLess);
	CheckSum(iSum, m_iBaseSMRankTotal);
	CheckSum(iSum, m_iBaseSMVolumetricRankTotal);
	CheckSum(iSum, m_iSelfHealModifier);
	CheckSum(iSum, m_iNumHealSupport);
	CheckSum(iSum, m_iInsidiousness);
	CheckSum(iSum, m_iInvestigation);
	CheckSum(iSum, m_iStealthStrikes);
	CheckSum(iSum, m_iStealthCombatModifier);
	CheckSum(iSum, m_iTrapDamageMax);
	CheckSum(iSum, m_iTrapDamageMin);
	CheckSum(iSum, m_iTrapComplexity);
	CheckSum(iSum, m_iNumTriggers);
	CheckSum(iSum, m_iAggression);
	CheckSum(iSum, m_iAnimalIgnoresBorders);
	CheckSum(iSum, m_iReligiousCombatModifier);
	//booleans
	CheckSum(iSum, m_bStampede);
	CheckSum(iSum, m_bOnslaught);
	CheckSum(iSum, m_bDealsColdDamage);
	CheckSum(iSum, m_bColdImmune);
	CheckSum(iSum, m_bAttackOnlyCities);
	CheckSum(iSum, m_bIgnoreNoEntryLevel);
	CheckSum(iSum, m_bIgnoreZoneofControl);
	CheckSum(iSum, m_bFliesToMove);
	CheckSum(iSum, m_bRBombardForceAbility);
	CheckSum(iSum, m_bNoSelfHeal);
	CheckSum(iSum, m_bExcile);
	CheckSum(iSum, m_bPassage);
	CheckSum(iSum, m_bNoNonOwnedCityEntry);
	CheckSum(iSum, m_bBarbCoExist);
	CheckSum(iSum, m_bBlendIntoCity);
	CheckSum(iSum, m_bUpgradeAnywhere);
	CheckSum(iSum, m_bAssassin);
	CheckSum(iSum, m_bStealthDefense);
	CheckSum(iSum, m_bNoInvisibility);
	CheckSum(iSum, m_bTriggerBeforeAttack);
	CheckSum(iSum, m_bNoNonTypeProdMods);
	CheckSum(iSum, m_bGatherHerd);
	//boolean vectors without delayed resolution
	CheckSumC(iSum, m_aiSubCombatTypes);
	CheckSumC(iSum, m_aiCureAfflictionTypes);
	CheckSumC(iSum, m_aiHealAsTypes);
	CheckSumC(iSum, m_aiTerrainImpassableTypes);
	CheckSumC(iSum, m_aiFeatureImpassableTypes);
	CheckSumC(iSum, m_aiMapCategoryTypes);
	CheckSumC(iSum, m_aiTrapSetWithPromotionTypes);
	CheckSumC(iSum, m_aiTrapImmunityUnitCombatTypes);
	// int vectors utilizing struct with delayed resolution
	int iNumElements;

	iNumElements = m_aAfflictionFortitudeModifiers.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aAfflictionFortitudeModifiers[i].ePromotionLine);
		CheckSum(iSum, m_aAfflictionFortitudeModifiers[i].iModifier);
	}

	iNumElements = m_aAfflictOnAttackTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aAfflictOnAttackTypes[i].eAfflictionLine);
		CheckSum(iSum, m_aAfflictOnAttackTypes[i].iProbability);
		CheckSum(iSum, m_aAfflictOnAttackTypes[i].iMelee);
		CheckSum(iSum, m_aAfflictOnAttackTypes[i].iDistance);
		CheckSum(iSum, m_aAfflictOnAttackTypes[i].iImmediate);
	}

	iNumElements = m_aHealUnitCombatTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aHealUnitCombatTypes[i].eUnitCombat);
		CheckSum(iSum, m_aHealUnitCombatTypes[i].iHeal);
	}

	iNumElements = m_aGroupSpawnUnitCombatTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aGroupSpawnUnitCombatTypes[i].eUnitCombat);
		CheckSum(iSum, m_aGroupSpawnUnitCombatTypes[i].iChance);
	}

	iNumElements = m_aInvisibleTerrainChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aInvisibleTerrainChanges[i].eInvisible);
		CheckSum(iSum, m_aInvisibleTerrainChanges[i].eTerrain);
		CheckSum(iSum, m_aInvisibleTerrainChanges[i].iIntensity);
	}

	iNumElements = m_aInvisibleFeatureChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aInvisibleFeatureChanges[i].eInvisible);
		CheckSum(iSum, m_aInvisibleFeatureChanges[i].eFeature);
		CheckSum(iSum, m_aInvisibleFeatureChanges[i].iIntensity);
	}

	iNumElements = m_aInvisibleImprovementChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aInvisibleImprovementChanges[i].eInvisible);
		CheckSum(iSum, m_aInvisibleImprovementChanges[i].eImprovement);
		CheckSum(iSum, m_aInvisibleImprovementChanges[i].iIntensity);
	}

	iNumElements = m_aVisibleTerrainChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aVisibleTerrainChanges[i].eInvisible);
		CheckSum(iSum, m_aVisibleTerrainChanges[i].eTerrain);
		CheckSum(iSum, m_aVisibleTerrainChanges[i].iIntensity);
	}

	iNumElements = m_aVisibleFeatureChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aVisibleFeatureChanges[i].eInvisible);
		CheckSum(iSum, m_aVisibleFeatureChanges[i].eFeature);
		CheckSum(iSum, m_aVisibleFeatureChanges[i].iIntensity);
	}

	iNumElements = m_aVisibleImprovementChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aVisibleImprovementChanges[i].eInvisible);
		CheckSum(iSum, m_aVisibleImprovementChanges[i].eImprovement);
		CheckSum(iSum, m_aVisibleImprovementChanges[i].iIntensity);
	}

	iNumElements = m_aVisibleTerrainRangeChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aVisibleTerrainRangeChanges[i].eInvisible);
		CheckSum(iSum, m_aVisibleTerrainRangeChanges[i].eTerrain);
		CheckSum(iSum, m_aVisibleTerrainRangeChanges[i].iIntensity);
	}

	iNumElements = m_aVisibleFeatureRangeChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aVisibleFeatureRangeChanges[i].eInvisible);
		CheckSum(iSum, m_aVisibleFeatureRangeChanges[i].eFeature);
		CheckSum(iSum, m_aVisibleFeatureRangeChanges[i].iIntensity);
	}

	iNumElements = m_aVisibleImprovementRangeChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aVisibleImprovementRangeChanges[i].eInvisible);
		CheckSum(iSum, m_aVisibleImprovementRangeChanges[i].eImprovement);
		CheckSum(iSum, m_aVisibleImprovementRangeChanges[i].iIntensity);
	}

	iNumElements = m_aDistanceAttackCommunicabilityTypeChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aDistanceAttackCommunicabilityTypeChanges[i].eAfflictionLine);
		CheckSum(iSum, m_aDistanceAttackCommunicabilityTypeChanges[i].iChange);
	}

	iNumElements = m_aEnabledCivilizationTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aEnabledCivilizationTypes[i].eCivilization);
	}
	// int vectors utilizing pairing without delayed resolution
	CheckSumC(iSum, m_aWithdrawOnTerrainTypes);
	CheckSumC(iSum, m_aWithdrawOnFeatureTypes);
	CheckSumC(iSum, m_aFlankingStrengthbyUnitCombatType);
	CheckSumC(iSum, m_aWithdrawVSUnitCombatTypes);
	CheckSumC(iSum, m_aPursuitVSUnitCombatTypes);
	CheckSumC(iSum, m_aRepelVSUnitCombatTypes);
	CheckSumC(iSum, m_aKnockbackVSUnitCombatTypes);
	CheckSumC(iSum, m_aPunctureVSUnitCombatTypes);
	CheckSumC(iSum, m_aArmorVSUnitCombatTypes);
	CheckSumC(iSum, m_aDodgeVSUnitCombatTypes);
	CheckSumC(iSum, m_aPrecisionVSUnitCombatTypes);
	CheckSumC(iSum, m_aCriticalVSUnitCombatTypes);
	CheckSumC(iSum, m_aRoundStunVSUnitCombatTypes);
	CheckSumC(iSum, m_aTrapDisableUnitCombatTypes);
	CheckSumC(iSum, m_aTrapAvoidanceUnitCombatTypes);
	CheckSumC(iSum, m_aTrapTriggerUnitCombatTypes);
	CheckSumC(iSum, m_aVisibilityIntensityTypes);
	CheckSumC(iSum, m_aInvisibilityIntensityTypes);
//Team Project (4)
		//WorkRateMod
	CheckSumC(iSum, m_aTerrainWorkRateModifierTypes);
	CheckSumC(iSum, m_aFeatureWorkRateModifierTypes);
	CheckSumC(iSum, m_aBuildWorkRateModifierTypes);
	CheckSumC(iSum, m_aAidChanges);
	//TB Combat Mods End  TB SubCombat Mod end

	CheckSum(iSum, m_szExtraHoverTextKey);
}

//
// read from xml
//
bool CvUnitInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	CvString szTextVal2;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	int j=0;				//loop counter
	int k=0;				//loop counter
	int iNumSibs=0;				// the number of siblings the current xml node has
	int iNumChildren;
	int iIndexVal;

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Class");
	m_iUnitClassType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Special");
	m_iSpecialUnitType = pXML->GetInfoClass(szTextVal);

	// EXTRA HOVER TEXT
	pXML->GetOptionalChildXmlValByName(m_szExtraHoverTextKey, L"ExtraHoverText");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Capture");
	m_iUnitCaptureClassType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Combat");
	m_iUnitCombatType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Domain");
	m_iDomainType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"DefaultUnitAI", "UNITAI_UNKNOWN");
	m_iDefaultUnitAIType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Invisible");
		m_iInvisibleType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SeeInvisible");
	std::vector<CvString> tokens;
	szTextVal.getTokens(",", tokens);
	for(int i=0;i<(int)tokens.size();i++)
	{
		int iInvisibleType = pXML->GetInfoClass(tokens[i]);
		if(iInvisibleType != NO_INVISIBLE)
		{
			m_aiSeeInvisibleTypes.push_back(iInvisibleType);
		}
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Advisor");
	m_iAdvisorType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_bFoodProduction, L"bFood");
	pXML->GetOptionalChildXmlValByName(&m_bNoBadGoodies, L"bNoBadGoodies");
	pXML->GetOptionalChildXmlValByName(&m_bOnlyDefensive, L"bOnlyDefensive");
	pXML->GetOptionalChildXmlValByName(&m_bNoCapture, L"bNoCapture");
	pXML->GetOptionalChildXmlValByName(&m_bQuickCombat, L"bQuickCombat");
	pXML->GetOptionalChildXmlValByName(&m_bRivalTerritory, L"bRivalTerritory");
	pXML->GetOptionalChildXmlValByName(&m_bMilitaryHappiness, L"bMilitaryHappiness");
	pXML->GetOptionalChildXmlValByName(&m_bMilitarySupport, L"bMilitarySupport");
	pXML->GetOptionalChildXmlValByName(&m_bMilitaryProduction, L"bMilitaryProduction");
	pXML->GetOptionalChildXmlValByName(&m_bPillage, L"bPillage");
	pXML->GetOptionalChildXmlValByName(&m_bSpy, L"bSpy");
	pXML->GetOptionalChildXmlValByName(&m_bSabotage, L"bSabotage");
	pXML->GetOptionalChildXmlValByName(&m_bDestroy, L"bDestroy");
	pXML->GetOptionalChildXmlValByName(&m_bStealPlans, L"bStealPlans");
	pXML->GetOptionalChildXmlValByName(&m_bInvestigate, L"bInvestigate");
	pXML->GetOptionalChildXmlValByName(&m_bCounterSpy, L"bCounterSpy");
	pXML->GetOptionalChildXmlValByName(&m_bFound, L"bFound");
	pXML->GetOptionalChildXmlValByName(&m_bGoldenAge, L"bGoldenAge");
	pXML->GetOptionalChildXmlValByName(&m_bInvisible, L"bInvisible");
	pXML->GetOptionalChildXmlValByName(&m_bFirstStrikeImmune, L"bFirstStrikeImmune");
	pXML->GetOptionalChildXmlValByName(&m_bNoDefensiveBonus, L"bNoDefensiveBonus");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreBuildingDefense, L"bIgnoreBuildingDefense");
	pXML->GetOptionalChildXmlValByName(&m_bCanMoveImpassable, L"bCanMoveImpassable");
	pXML->GetOptionalChildXmlValByName(&m_bCanMoveAllTerrain, L"bCanMoveAllTerrain");
	pXML->GetOptionalChildXmlValByName(&m_bFlatMovementCost, L"bFlatMovementCost");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreTerrainCost, L"bIgnoreTerrainCost");
	pXML->GetOptionalChildXmlValByName(&m_bNukeImmune, L"bNukeImmune");
	pXML->GetOptionalChildXmlValByName(&m_bPrereqBonuses, L"bPrereqBonuses");
	pXML->GetOptionalChildXmlValByName(&m_bPrereqReligion, L"bPrereqReligion");
	pXML->GetOptionalChildXmlValByName(&m_bMechanized, L"bMechanized");
	pXML->GetOptionalChildXmlValByName(&m_bRenderBelowWater, L"bRenderBelowWater");
	pXML->GetOptionalChildXmlValByName(&m_bRenderAlways, L"bRenderAlways");
	pXML->GetOptionalChildXmlValByName(&m_bSuicide, L"bSuicide");
	pXML->GetOptionalChildXmlValByName(&m_bLineOfSight, L"bLineOfSight");
	pXML->GetOptionalChildXmlValByName(&m_bHiddenNationality, L"bHiddenNationality");
	pXML->GetOptionalChildXmlValByName(&m_bAlwaysHostile, L"bAlwaysHostile");
/*****************************************************************************************************/
/**  Author: TheLadiesOgre																		  **/
/**  Date: 21.09.2009																			   **/
/**  ModComp: TLOTags																			   **/
/**  Reason Added: New Tag Definition															   **/
/**  Notes:																						 **/
/*****************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_bFreeDrop, L"bFreeDrop");
/*****************************************************************************************************/
/**  TheLadiesOgre; 21.09.2009; TLOTags															 **/
/*****************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_bNoRevealMap, L"bNoRevealMap");
/************************************************************************************************/
/* REVOLUTION_MOD								 01/01/08						DPII		  */
/*																							  */
/* CoreComponent																				*/
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_bInquisitor, L"bInquisitor");
	pXML->GetOptionalChildXmlValByName(&m_bCanBeRebel, L"bCanBeRebel");
	pXML->GetOptionalChildXmlValByName(&m_bCanRebelCapture, L"bCanRebelCapture");
	pXML->GetOptionalChildXmlValByName(&m_bCannotDefect, L"bCannotDefect");
	pXML->GetOptionalChildXmlValByName(&m_bCanQuellRebellion, L"bCanQuellRebellion");
/************************************************************************************************/
/* REVOLUTION_MOD						  END												  */
/************************************************************************************************/
	//ls612: Can't enter non-Owned cities
	pXML->GetOptionalChildXmlValByName(&m_bNoNonOwnedEntry, L"bOnlyFriendlyEntry");

	pXML->SetVariableListTagPair(&m_pbUpgradeUnitClass, L"UnitClassUpgrades", GC.getNumUnitClassInfos());
	pXML->SetVariableListTagPair(&m_pbTargetUnitClass, L"UnitClassTargets", GC.getNumUnitClassInfos());
	pXML->SetVariableListTagPair(&m_pbTargetUnitCombat, L"UnitCombatTargets", GC.getNumUnitCombatInfos());
	pXML->SetVariableListTagPair(&m_pbDefenderUnitClass, L"UnitClassDefenders", GC.getNumUnitClassInfos());
	pXML->SetVariableListTagPair(&m_pbDefenderUnitCombat, L"UnitCombatDefenders", GC.getNumUnitCombatInfos());
	pXML->SetVariableListTagPair(&m_piFlankingStrikeUnitClass, L"FlankingStrikes", GC.getNumUnitClassInfos(), -1);
	pXML->SetVariableListTagPair(&m_pbUnitAIType, L"UnitAIs", NUM_UNITAI_TYPES);
	pXML->SetVariableListTagPair(&m_pbNotUnitAIType, L"NotUnitAIs", NUM_UNITAI_TYPES);

	pXML->SetVariableListTagPair(&m_pbBuilds, L"Builds", GC.getNumBuildInfos());

	pXML->SetVariableListTagPair(&m_piReligionSpreads, L"ReligionSpreads", GC.getNumReligionInfos(),-1);
	pXML->SetVariableListTagPair(&m_piCorporationSpreads, L"CorporationSpreads", GC.getNumCorporationInfos(), -1);

	CvString* pszTemp = NULL;
	pXML->SetVariableListTagPair(&pszTemp, L"TerrainPassableTechs", GC.getNumTerrainInfos());
	if ( pszTemp != NULL )
	{
		m_piTerrainPassableTech = new int[GC.getNumTerrainInfos()];
		for (int i = 0; i < GC.getNumTerrainInfos(); ++i)
		{
			m_piTerrainPassableTech[i] = pszTemp[i].IsEmpty() ? NO_TECH : pXML->GetInfoClass(pszTemp[i]);
		}
		SAFE_DELETE_ARRAY(pszTemp);
	}
	else
	{
		m_piTerrainPassableTech = NULL;
	}

	pXML->SetVariableListTagPair(&pszTemp, L"FeaturePassableTechs", GC.getNumFeatureInfos());
	if ( pszTemp != NULL )
	{
		m_piFeaturePassableTech = new int[GC.getNumFeatureInfos()];
		for (int i = 0; i < GC.getNumFeatureInfos(); ++i)
		{
			m_piFeaturePassableTech[i] = pszTemp[i].IsEmpty() ? NO_TECH : pXML->GetInfoClass(pszTemp[i]);
		}
		SAFE_DELETE_ARRAY(pszTemp);
	}
	else
	{
		m_piFeaturePassableTech = NULL;
	}

	pXML->SetVariableListTagPair(&m_pbGreatPeoples, L"GreatPeoples", GC.getNumSpecialistInfos());

	pXML->SetOptionalIntVector(&m_pbBuildings, L"Buildings");
/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain																*/
/********************************************************************************/
	pXML->GetOptionalChildXmlValByName(szTextVal, L"MaxStartEra");
	m_iMaxStartEra = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ForceObsoleteTech");
	m_iForceObsoleteTech = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_bStateReligion, L"bStateReligion");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqGameOption");
	m_iPrereqGameOption = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"NotGameOption");
	m_iNotGameOption = pXML->GetInfoClass(szTextVal);

	pXML->SetVariableListTagPair(&m_pbPrereqOrCivics, L"PrereqOrCivics", GC.getNumCivicInfos());

	pXML->SetVariableListTagPair(&m_pbPrereqBuildingClass, L"PrereqBuildingClasses", GC.getNumBuildingClassInfos());

	pXML->SetVariableListTagPair(&m_piPrereqBuildingClassOverrideTech, L"PrereqBuildingClasses", GC.getNumBuildingClassInfos(), L"TechOverride", GC.getNumTechInfos());

	pXML->SetVariableListTagPair(&m_piPrereqBuildingClassOverrideEra, L"PrereqBuildingClasses", GC.getNumBuildingClassInfos(), L"EraOverride", GC.getNumEraInfos());

	pXML->SetVariableListTagPair(&m_pbForceObsoleteUnitClass, L"ForceObsoleteUnitClasses", GC.getNumUnitClassInfos());
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

	pXML->GetOptionalChildXmlValByName(szTextVal, L"HolyCity");
	m_iHolyCity = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ReligionType");
	m_iReligionType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"StateReligion");
	m_iStateReligion = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqReligion");
	m_iPrereqReligion = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqCorporation");
	m_iPrereqCorporation = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqBuilding");
	m_iPrereqBuilding = pXML->GetInfoClass(szTextVal);

	pXML->SetOptionalIntVector(&m_aePrereqOrBuildings, L"PrereqOrBuildings");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqTech");
	m_iPrereqAndTech = pXML->GetInfoClass(szTextVal);

	if (pXML->TryMoveToXmlFirstChild(L"TechTypes"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		FAssertMsg((0 < GC.getNUM_UNIT_AND_TECH_PREREQS()) ,"Allocating zero or less memory in SetGlobalUnitInfo");

		if (0 < iNumSibs)
		{
			pXML->CvXMLLoadUtility::InitList(&m_piPrereqAndTechs, GC.getNUM_UNIT_AND_TECH_PREREQS(), -1);
			if (pXML->GetChildXmlVal(szTextVal))
			{
				FAssertMsg((iNumSibs <= GC.getNUM_UNIT_AND_TECH_PREREQS()) ,"There are more siblings than memory allocated for them in SetGlobalUnitInfo");
				for (j=0;j<iNumSibs;j++)
				{
					m_piPrereqAndTechs[j] = pXML->GetInfoClass(szTextVal);
					if (!pXML->GetNextXmlVal(szTextVal))
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}
		else
		{
			SAFE_DELETE_ARRAY(m_piPrereqAndTechs);
		}

		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piPrereqAndTechs);
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"BonusType");
	m_iPrereqAndBonus = pXML->GetInfoClass(szTextVal);

	if (pXML->TryMoveToXmlFirstChild(L"PrereqBonuses"))
	{
		iNumChildren = pXML->GetXmlChildrenNumber();
		FAssertMsg((0 < GC.getNUM_UNIT_PREREQ_OR_BONUSES()),"Allocating zero or less memory in SetGlobalUnitInfo");

		if (0 < iNumChildren)
		{
			pXML->CvXMLLoadUtility::InitList(&m_piPrereqOrBonuses, GC.getNUM_UNIT_PREREQ_OR_BONUSES(), -1);
			if (pXML->GetChildXmlVal(szTextVal))
			{
				FAssertMsg((iNumChildren <= GC.getNUM_UNIT_PREREQ_OR_BONUSES()) , "There are more siblings than memory allocated for them in SetGlobalUnitInfo");
				for (j=0;j<iNumChildren;j++)
				{
					m_piPrereqOrBonuses[j] = pXML->GetInfoClass(szTextVal);
					if (!pXML->GetNextXmlVal(szTextVal))
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}
		else
		{
			SAFE_DELETE_ARRAY(m_piPrereqOrBonuses);
		}

		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piPrereqOrBonuses);
	}

	//pXML->SetVariableListTagPair(&m_piProductionTraits, L"ProductionTraits", GC.getNumTraitInfos());

	pXML->SetVariableListTagPair(&m_piFlavorValue, L"Flavors", GC.getFlavorTypes(), GC.getNumFlavorTypes());

	pXML->GetOptionalChildXmlValByName(&m_iAIWeight, L"iAIWeight");
	pXML->GetOptionalChildXmlValByName(&m_iProductionCost, L"iCost");
	pXML->GetOptionalChildXmlValByName(&m_iHurryCostModifier, L"iHurryCostModifier");
	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartCost, L"iAdvancedStartCost", 100);
	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartCostIncrease, L"iAdvancedStartCostIncrease");
	pXML->GetOptionalChildXmlValByName(&m_iMinAreaSize, L"iMinAreaSize");
	pXML->GetOptionalChildXmlValByName(&m_iMoves, L"iMoves");
	pXML->GetOptionalChildXmlValByName(&m_iAirRange, L"iAirRange");
	pXML->GetOptionalChildXmlValByName(&m_iAirUnitCap, L"iAirUnitCap");
	pXML->GetOptionalChildXmlValByName(&m_iDropRange, L"iDropRange");
	pXML->GetOptionalChildXmlValByName(&m_iNukeRange, L"iNukeRange", -1);
	pXML->GetOptionalChildXmlValByName(&m_iWorkRate, L"iWorkRate");
	pXML->GetOptionalChildXmlValByName(&m_iBaseDiscover, L"iBaseDiscover");
	pXML->GetOptionalChildXmlValByName(&m_iDiscoverMultiplier, L"iDiscoverMultiplier");
	pXML->GetOptionalChildXmlValByName(&m_iBaseHurry, L"iBaseHurry");
	pXML->GetOptionalChildXmlValByName(&m_iHurryMultiplier, L"iHurryMultiplier");
	pXML->GetOptionalChildXmlValByName(&m_iBaseTrade, L"iBaseTrade");
	pXML->GetOptionalChildXmlValByName(&m_iTradeMultiplier, L"iTradeMultiplier");
	pXML->GetOptionalChildXmlValByName(&m_iGreatWorkCulture, L"iGreatWorkCulture");
	pXML->GetOptionalChildXmlValByName(&m_iEspionagePoints, L"iEspionagePoints");

	//pXML->SetVariableListTagPair(&m_pbTerrainImpassable, L"TerrainImpassables", GC.getNumTerrainInfos(), false);
	//pXML->SetVariableListTagPair(&m_pbFeatureImpassable, L"FeatureImpassables", GC.getNumFeatureInfos(), false);

	pXML->GetOptionalChildXmlValByName(&m_iCombat, L"iCombat");
	pXML->GetOptionalChildXmlValByName(&m_iCombatLimit, L"iCombatLimit", 100);
	pXML->GetOptionalChildXmlValByName(&m_iAirCombat, L"iAirCombat");
	pXML->GetOptionalChildXmlValByName(&m_iAirCombatLimit, L"iAirCombatLimit");
	pXML->GetOptionalChildXmlValByName(&m_iXPValueAttack, L"iXPValueAttack");
	pXML->GetOptionalChildXmlValByName(&m_iXPValueDefense, L"iXPValueDefense");
	pXML->GetOptionalChildXmlValByName(&m_iFirstStrikes, L"iFirstStrikes");
	pXML->GetOptionalChildXmlValByName(&m_iChanceFirstStrikes, L"iChanceFirstStrikes");
	pXML->GetOptionalChildXmlValByName(&m_iInterceptionProbability, L"iInterceptionProbability");
	//ls612: Advanced Nuke Interception
	//pXML->GetChildXmlValByName(&m_iNukeInterceptionProbability, L"iNukeInterceptionProbability");
	//pXML->GetChildXmlValByName(&m_iNukeInterceptionRange, L"iNukeInterceptionRange");
	pXML->GetOptionalChildXmlValByName(&m_iEvasionProbability, L"iEvasionProbability");
	pXML->GetOptionalChildXmlValByName(&m_iWithdrawalProbability, L"iWithdrawalProb");
	pXML->GetOptionalChildXmlValByName(&m_iCollateralDamage, L"iCollateralDamage");
	pXML->GetOptionalChildXmlValByName(&m_iCollateralDamageLimit, L"iCollateralDamageLimit");
	pXML->GetOptionalChildXmlValByName(&m_iCollateralDamageMaxUnits, L"iCollateralDamageMaxUnits");
	pXML->GetOptionalChildXmlValByName(&m_iCityAttackModifier, L"iCityAttack");
	pXML->GetOptionalChildXmlValByName(&m_iCityDefenseModifier, L"iCityDefense");
	pXML->GetOptionalChildXmlValByName(&m_iAnimalCombatModifier, L"iAnimalCombat");
	pXML->GetOptionalChildXmlValByName(&m_iHillsAttackModifier, L"iHillsAttack");
	pXML->GetOptionalChildXmlValByName(&m_iHillsDefenseModifier, L"iHillsDefense");

	pXML->SetVariableListTagPair(&m_pbTerrainNative, L"TerrainNatives", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_pbFeatureNative, L"FeatureNatives", GC.getNumFeatureInfos());

	pXML->SetVariableListTagPair(&m_piTerrainAttackModifier, L"TerrainAttacks", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_piTerrainDefenseModifier, L"TerrainDefenses", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_piFeatureAttackModifier, L"FeatureAttacks", GC.getNumFeatureInfos());
	pXML->SetVariableListTagPair(&m_piFeatureDefenseModifier, L"FeatureDefenses", GC.getNumFeatureInfos());

	pXML->SetVariableListTagPair(&m_piUnitClassAttackModifier, L"UnitClassAttackMods", GC.getNumUnitClassInfos());
	pXML->SetVariableListTagPair(&m_piUnitClassDefenseModifier, L"UnitClassDefenseMods", GC.getNumUnitClassInfos());

	pXML->SetVariableListTagPair(&m_piUnitCombatModifier, L"UnitCombatMods", GC.getNumUnitCombatInfos());
	pXML->SetVariableListTagPair(&m_piUnitCombatCollateralImmune, L"UnitCombatCollateralImmunes", GC.getNumUnitCombatInfos());
	pXML->SetVariableListTagPair(&m_piDomainModifier, L"DomainMods", NUM_DOMAIN_TYPES);

	pXML->SetVariableListTagPair(&m_piBonusProductionModifier, L"BonusProductionModifiers", GC.getNumBonusInfos());

	pXML->GetOptionalChildXmlValByName(&m_iBombRate, L"iBombRate");
	pXML->GetOptionalChildXmlValByName(&m_iBombardRate, L"iBombardRate");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SpecialCargo");
	m_iSpecialCargo = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SMSpecialCargo");
	m_iSMSpecialCargo = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SMNotSpecialCargo");
	m_iSMNotSpecialCargo = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"DomainCargo");
	m_iDomainCargo = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iCargoSpace, L"iCargo");
	pXML->GetOptionalChildXmlValByName(&m_iSMCargoSpace, L"iSMCargo");
	pXML->GetOptionalChildXmlValByName(&m_iSMCargoVolume, L"iSMCargoVolume");
	pXML->GetOptionalChildXmlValByName(&m_iConscriptionValue, L"iConscription");
	pXML->GetOptionalChildXmlValByName(&m_iCultureGarrisonValue, L"iCultureGarrison");
	pXML->GetOptionalChildXmlValByName(&m_iExtraCost, L"iExtraCost");
	pXML->GetOptionalChildXmlValByName(&m_iAssetValue, L"iAsset");
	pXML->GetOptionalChildXmlValByName(&m_iPowerValue, L"iPower");

	// Read the mesh groups elements
	if ( pXML->TryMoveToXmlFirstChild(L"UnitMeshGroups") )
	{
		pXML->GetChildXmlValByName( &m_iGroupSize, L"iGroupSize");
		m_iGroupDefinitions = iIndexVal = pXML->GetXmlChildrenNumber(L"UnitMeshGroup");
		m_piUnitGroupRequired = new int[ iIndexVal ];
		pXML->GetChildXmlValByName( &m_iUnitMeleeWaveSize, L"iMeleeWaveSize" );
		pXML->GetChildXmlValByName( &m_iUnitRangedWaveSize, L"iRangedWaveSize" );
		pXML->GetChildXmlValByName( &m_fUnitMaxSpeed, L"fMaxSpeed");
		pXML->GetChildXmlValByName( &m_fUnitPadTime, L"fPadTime");
/************************************************************************************************/
/* Afforess					  Start		 03/10/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
		m_paszEarlyArtDefineTags = new CvString[ iIndexVal ];
		m_paszClassicalArtDefineTags = new CvString[ iIndexVal ];
		m_paszMiddleArtDefineTags = new CvString[ iIndexVal ];
		m_paszRennArtDefineTags = new CvString[ iIndexVal ];
		m_paszIndustrialArtDefineTags = new CvString[ iIndexVal ];
		m_paszLateArtDefineTags = new CvString[ iIndexVal ];
		m_paszFutureArtDefineTags = new CvString[ iIndexVal ];
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

		if (pXML->TryMoveToXmlFirstChild(L"UnitMeshGroup"))
		{
			for ( k = 0; k < iIndexVal; k++ )
			{
/************************************************************************************************/
/* Afforess					  Start		 03/10/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
				pXML->GetChildXmlValByName( &m_piUnitGroupRequired[k], L"iRequired");
				if (pXML->GetOptionalChildXmlValByName(szTextVal, L"EarlyArtDefineTag"))
					GC.hasInfoTypeForString(szTextVal);
				setEarlyArtDefineTag(k, szTextVal);
				if (pXML->GetOptionalChildXmlValByName(szTextVal, L"ClassicalArtDefineTag"))
					GC.hasInfoTypeForString(szTextVal);
				setClassicalArtDefineTag(k, szTextVal);
				if (pXML->GetOptionalChildXmlValByName(szTextVal, L"MiddleArtDefineTag"))
					GC.hasInfoTypeForString(szTextVal);
				setMiddleArtDefineTag(k, szTextVal);
				if (pXML->GetOptionalChildXmlValByName(szTextVal, L"RennArtDefineTag"))
					GC.hasInfoTypeForString(szTextVal);
				setRennArtDefineTag(k, szTextVal);
				if (pXML->GetOptionalChildXmlValByName(szTextVal, L"IndustrialArtDefineTag"))
					GC.hasInfoTypeForString(szTextVal);
				setIndustrialArtDefineTag(k, szTextVal);
				if (pXML->GetOptionalChildXmlValByName(szTextVal, L"LateArtDefineTag"))
					GC.hasInfoTypeForString(szTextVal);
				setLateArtDefineTag(k, szTextVal);
				if (pXML->GetOptionalChildXmlValByName(szTextVal, L"FutureArtDefineTag"))
					GC.hasInfoTypeForString(szTextVal);
				setFutureArtDefineTag(k, szTextVal);
				pXML->TryMoveToXmlNextSibling();
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	pXML->GetOptionalChildXmlValByName(m_szFormationType, L"FormationType");

	if (pXML->TryMoveToXmlFirstChild(L"UniqueNames"))
	{
		pXML->SetStringList(&m_paszUnitNames, &m_iNumUnitNames);
		pXML->MoveToXmlParent();
	}

	pXML->SetVariableListTagPair(&m_pbFreePromotions, L"FreePromotions", GC.getNumPromotionInfos());

	pXML->GetOptionalChildXmlValByName(szTextVal, L"LeaderPromotion");
	m_iLeaderPromotion = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iLeaderExperience, L"iLeaderExperience");

	// Dale - RB: Field Bombard START
	pXML->GetOptionalChildXmlValByName(&m_iDCMBombRange, L"iDCMBombRange");
	pXML->GetOptionalChildXmlValByName(&m_iDCMBombAccuracy, L"iDCMBombAccuracy");
	// Dale - RB: Field Bombard END
	// Dale - AB: Bombing START
	pXML->GetOptionalChildXmlValByName(&m_bDCMAirBomb1, L"bDCMAirBomb1");
	pXML->GetOptionalChildXmlValByName(&m_bDCMAirBomb2, L"bDCMAirBomb2");
	pXML->GetOptionalChildXmlValByName(&m_bDCMAirBomb3, L"bDCMAirBomb3");
	pXML->GetOptionalChildXmlValByName(&m_bDCMAirBomb4, L"bDCMAirBomb4");
	pXML->GetOptionalChildXmlValByName(&m_bDCMAirBomb5, L"bDCMAirBomb5");
	// Dale - AB: Bombing END
	// Dale - FE: Fighters START
	pXML->GetOptionalChildXmlValByName(&m_bDCMFighterEngage, L"bDCMFighterEngage");
	// Dale - FE: Fighters END

/************************************************************************************************/
/* Afforess					  Start		 Last Update: 3/8/10									*/
/*																							  */
/*																							  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_bWorkerTrade, L"bWorkerTrade");
	pXML->GetOptionalChildXmlValByName(&m_bMilitaryTrade, L"bMilitaryTrade");
	pXML->GetOptionalChildXmlValByName(&m_bForceUpgrade, L"bForceUpgrade");
	pXML->GetOptionalChildXmlValByName(&m_bGreatGeneral, L"bGreatGeneral");
	pXML->GetOptionalChildXmlValByName(&m_bSlave, L"bSlave");
	pXML->GetOptionalChildXmlValByName(&m_bRequiresStateReligionInCity, L"bRequiresStateReligionInCity");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"VicinityBonusType");
	m_iPrereqVicinityBonus = pXML->GetInfoClass(szTextVal);
	pXML->SetVariableListTagPair(&m_pbPassableRouteNeeded, L"PassableRouteNeededs", GC.getNumRouteInfos(), false);
	pXML->SetVariableListTagPair(&m_pbPrereqBuildingClass, L"PrereqBuildingClasses", GC.getNumBuildingClassInfos());
	pXML->GetOptionalChildXmlValByName(&m_iBaseFoodChange, L"iBaseFoodChange");
	pXML->GetOptionalChildXmlValByName(&m_iControlPoints, L"iControlPoints");
	pXML->GetOptionalChildXmlValByName(&m_iCommandRange, L"iCommandRange");

//	pXML->SetVariableListTagPair(&m_paszCivilizationNames, L"", sizeof(GC.getCivilizationInfo((CivilizationTypes)0)), GC.getNumCivilizationInfos(), L"");


	if (pXML->TryMoveToXmlFirstChild(L"CivilizationNames"))
	{
		CvString szTemp;
		int iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (iNumSibs > 0)
			{
				for (int i=0;i<iNumSibs;i++)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						m_aszCivilizationNamesforPass3.push_back(szTextVal);
						pXML->GetNextXmlVal(szTemp);
						m_aszCivilizationNamesValueforPass3.push_back(szTemp);

						pXML->MoveToXmlParent();
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"PrereqVicinityBonuses"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		FAssertMsg((0 < GC.getNUM_UNIT_PREREQ_OR_BONUSES()),"Allocating zero or less memory in SetGlobalUnitInfo");

		if (0 < iNumSibs)
		{
			pXML->CvXMLLoadUtility::InitList(&m_piPrereqOrVicinityBonuses, GC.getNUM_UNIT_PREREQ_OR_BONUSES(), -1);
			if (pXML->GetChildXmlVal(szTextVal))
			{
				FAssertMsg((iNumSibs <= GC.getNUM_UNIT_PREREQ_OR_BONUSES()) , "There are more siblings than memory allocated for them in SetGlobalUnitInfo");
				for (j=0;j<iNumSibs;j++)
				{
					m_piPrereqOrVicinityBonuses[j] = pXML->GetInfoClass(szTextVal);
					if (!pXML->GetNextXmlVal(szTextVal))
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}
		else
		{
			SAFE_DELETE_ARRAY(m_piPrereqOrVicinityBonuses);
		}

		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piPrereqOrVicinityBonuses);
	}

/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	m_PropertyManipulators.read(pXML);

	if (pXML->TryMoveToXmlFirstChild(L"TrainCondition"))
	{
		m_pExprTrainCondition = BoolExpr::read(pXML);
		pXML->MoveToXmlParent();
	}
	//TB Combat Mods Begin  TB SubCombat Mod begin
	//integers
	pXML->GetOptionalChildXmlValByName(&m_iAttackCombatModifier, L"iAttackCombatModifier");
	pXML->GetOptionalChildXmlValByName(&m_iDefenseCombatModifier, L"iDefenseCombatModifier");
	pXML->GetOptionalChildXmlValByName(&m_iPursuit, L"iPursuit");
	pXML->GetOptionalChildXmlValByName(&m_iEarlyWithdraw, L"iEarlyWithdraw");
	pXML->GetOptionalChildXmlValByName(&m_iVSBarbs, L"iVSBarbs");
	pXML->GetOptionalChildXmlValByName(&m_iArmor, L"iArmor");
	pXML->GetOptionalChildXmlValByName(&m_iPuncture, L"iPuncture");
	pXML->GetOptionalChildXmlValByName(&m_iOverrun, L"iOverrun");
	pXML->GetOptionalChildXmlValByName(&m_iRepel, L"iRepel");
	pXML->GetOptionalChildXmlValByName(&m_iFortRepel, L"iFortRepel");
	pXML->GetOptionalChildXmlValByName(&m_iRepelRetries, L"iRepelRetries");
	pXML->GetOptionalChildXmlValByName(&m_iUnyielding, L"iUnyielding");
	pXML->GetOptionalChildXmlValByName(&m_iKnockback, L"iKnockback");
	pXML->GetOptionalChildXmlValByName(&m_iKnockbackRetries, L"iKnockbackRetries");
	pXML->GetOptionalChildXmlValByName(&m_iStrAdjperRnd, L"iStrAdjperRnd");
	pXML->GetOptionalChildXmlValByName(&m_iStrAdjperAtt, L"iStrAdjperAtt");
	pXML->GetOptionalChildXmlValByName(&m_iStrAdjperDef, L"iStrAdjperDef");
	pXML->GetOptionalChildXmlValByName(&m_iWithdrawAdjperAtt, L"iWithdrawAdjperAtt");
	pXML->GetOptionalChildXmlValByName(&m_iUnnerve, L"iUnnerve");
	pXML->GetOptionalChildXmlValByName(&m_iEnclose, L"iEnclose");
	pXML->GetOptionalChildXmlValByName(&m_iLunge, L"iLunge");
	pXML->GetOptionalChildXmlValByName(&m_iDynamicDefense, L"iDynamicDefense");
	pXML->GetOptionalChildXmlValByName(&m_iFortitude, L"iFortitude");

	for (int iI = 0; iI < NUM_SUPPORT_POSITION_TYPES; iI++)
	{
		m_aiSupportPercent.pushback(0);
	}
	pXML->GetOptionalChildXmlValByName(&m_aiSupportPercent[SUPPORT_POSITION_FRONT], L"iFrontSupportPercent");
	pXML->GetOptionalChildXmlValByName(&m_aiSupportPercent[SUPPORT_POSITION_SHORT_RANGE], L"iShortRangeSupportPercent");
	pXML->GetOptionalChildXmlValByName(&m_aiSupportPercent[SUPPORT_POSITION_MEDIUM_RANGE], L"iMediumRangeSupportPercent");
	pXML->GetOptionalChildXmlValByName(&m_aiSupportPercent[SUPPORT_POSITION_LONG_RANGE], L"iLongRangeSupportPercent");
	pXML->GetOptionalChildXmlValByName(&m_aiSupportPercent[SUPPORT_POSITION_FLANK], L"iFlankSupportPercent");

	pXML->GetOptionalChildXmlValByName(&m_iDodgeModifier, L"iDodgeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iPrecisionModifier, L"iPrecisionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iPowerShots, L"iPowerShots");
	pXML->GetOptionalChildXmlValByName(&m_iPowerShotCombatModifier, L"iPowerShotCombatModifier");
	pXML->GetOptionalChildXmlValByName(&m_iPowerShotPunctureModifier, L"iPowerShotPunctureModifier");
	pXML->GetOptionalChildXmlValByName(&m_iPowerShotPrecisionModifier, L"iPowerShotPrecisionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iPowerShotCriticalModifier, L"iPowerShotCriticalModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCriticalModifier, L"iCriticalModifier");
	pXML->GetOptionalChildXmlValByName(&m_iEndurance, L"iEndurance");
	pXML->GetOptionalChildXmlValByName(&m_iRoundStunProb, L"iRoundStunProb");
	pXML->GetOptionalChildXmlValByName(&m_iPoisonProbabilityModifier, L"iPoisonProbabilityModifier");
//Team Project (3)
	pXML->GetOptionalChildXmlValByName(&m_iCaptureProbabilityModifier, L"iCaptureProbabilityModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCaptureResistanceModifier, L"iCaptureResistanceModifier");
//Team Project (4)
//WorkRateMod
	pXML->GetOptionalChildXmlValByName(&m_iHillsWorkModifier, L"iHillsWorkModifier");
	pXML->GetOptionalChildXmlValByName(&m_iPeaksWorkModifier, L"iPeaksWorkModifier");

	pXML->GetOptionalChildXmlValByName(&m_iBreakdownChance, L"iBreakdownChance");
	pXML->GetOptionalChildXmlValByName(&m_iBreakdownDamage, L"iBreakdownDamage");
	pXML->GetOptionalChildXmlValByName(&m_iTaunt, L"iTaunt");
	pXML->GetOptionalChildXmlValByName(&m_iMaxHP, L"iMaxHP", 100);
	pXML->GetOptionalChildXmlValByName(&m_iDamageModifier, L"iDamageModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCostModifier, L"iCostModifier");
	pXML->GetOptionalChildXmlValByName(&m_iRBombardDamage, L"iRBombardDamage");
	pXML->GetOptionalChildXmlValByName(&m_iRBombardDamageLimit, L"iRBombardDamageLimit");
	pXML->GetOptionalChildXmlValByName(&m_iRBombardDamageMaxUnits, L"iRBombardDamageMaxUnits");
	pXML->GetOptionalChildXmlValByName(&m_iCombatModifierPerSizeMore, L"iCombatModifierPerSizeMore");
	pXML->GetOptionalChildXmlValByName(&m_iCombatModifierPerSizeLess, L"iCombatModifierPerSizeLess");
	pXML->GetOptionalChildXmlValByName(&m_iCombatModifierPerVolumeMore, L"iCombatModifierPerVolumeMore");
	pXML->GetOptionalChildXmlValByName(&m_iCombatModifierPerVolumeLess, L"iCombatModifierPerVolumeLess");
	pXML->GetOptionalChildXmlValByName(&m_iSelfHealModifier, L"iSelfHealModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNumHealSupport, L"iNumHealSupport");
	pXML->GetOptionalChildXmlValByName(&m_iInsidiousness, L"iInsidiousness");
	pXML->GetOptionalChildXmlValByName(&m_iInvestigation, L"iInvestigation");
	pXML->GetOptionalChildXmlValByName(&m_iStealthStrikes, L"iStealthStrikes");
	pXML->GetOptionalChildXmlValByName(&m_iStealthCombatModifier, L"iStealthCombatModifier");
	pXML->GetOptionalChildXmlValByName(&m_iTrapDamageMax, L"iTrapDamageMax");
	pXML->GetOptionalChildXmlValByName(&m_iTrapDamageMin, L"iTrapDamageMin");
	pXML->GetOptionalChildXmlValByName(&m_iTrapComplexity, L"iTrapComplexity");
	pXML->GetOptionalChildXmlValByName(&m_iNumTriggers, L"iNumTriggers");
	pXML->GetOptionalChildXmlValByName(&m_iAggression, L"iAggression", 5);
	pXML->GetOptionalChildXmlValByName(&m_iAnimalIgnoresBorders, L"iAnimalIgnoresBorders");
	pXML->GetOptionalChildXmlValByName(&m_iReligiousCombatModifier, L"iReligiousCombatModifier");
	//booleans
	pXML->GetOptionalChildXmlValByName(&m_bStampede, L"bStampede");
	pXML->GetOptionalChildXmlValByName(&m_bOnslaught, L"bOnslaught");
	pXML->GetOptionalChildXmlValByName(&m_bDealsColdDamage, L"bDealsColdDamage");
	pXML->GetOptionalChildXmlValByName(&m_bColdImmune, L"bColdImmune");
	pXML->GetOptionalChildXmlValByName(&m_bAttackOnlyCities, L"bAttackOnlyCities");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreNoEntryLevel, L"bIgnoreNoEntryLevel");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreZoneofControl, L"bIgnoreZoneofControl");
	pXML->GetOptionalChildXmlValByName(&m_bFliesToMove, L"bFliesToMove");
	pXML->GetOptionalChildXmlValByName(&m_bRBombardForceAbility, L"bRBombardForceAbility");
	pXML->GetOptionalChildXmlValByName(&m_bNoSelfHeal, L"bNoSelfHeal");
	pXML->GetOptionalChildXmlValByName(&m_bExcile, L"bExcile");
	pXML->GetOptionalChildXmlValByName(&m_bPassage, L"bPassage");
	pXML->GetOptionalChildXmlValByName(&m_bNoNonOwnedCityEntry, L"bNoNonOwnedCityEntry");
	pXML->GetOptionalChildXmlValByName(&m_bBarbCoExist, L"bBarbCoExist");
	pXML->GetOptionalChildXmlValByName(&m_bBlendIntoCity, L"bBlendIntoCity");
	pXML->GetOptionalChildXmlValByName(&m_bUpgradeAnywhere, L"bUpgradeAnywhere");
	pXML->GetOptionalChildXmlValByName(&m_bAssassin, L"bAssassin");
	pXML->GetOptionalChildXmlValByName(&m_bStealthDefense, L"bStealthDefense");
	pXML->GetOptionalChildXmlValByName(&m_bNoInvisibility, L"bNoInvisibility");
	pXML->GetOptionalChildXmlValByName(&m_bTriggerBeforeAttack, L"bTriggerBeforeAttack");
	pXML->GetOptionalChildXmlValByName(&m_bNoNonTypeProdMods, L"bNoNonTypeProdMods");
	pXML->GetOptionalChildXmlValByName(&m_bGatherHerd, L"bGatherHerd");

	//boolean vectors without delayed resolution
	pXML->SetOptionalIntVector(&m_aiSubCombatTypes, L"SubCombatTypes");

	pXML->SetOptionalIntVector(&m_aiCureAfflictionTypes, L"CureAfflictionTypes");

	pXML->SetOptionalIntVector(&m_aiTerrainImpassableTypes, L"TerrainImpassableTypes");
	pXML->SetOptionalIntVector(&m_aiFeatureImpassableTypes, L"FeatureImpassableTypes");

	pXML->SetOptionalIntVector(&m_aiMapCategoryTypes, L"MapCategoryTypes");

	pXML->SetOptionalIntVector(&m_aiTrapSetWithPromotionTypes, L"TrapSetWithPromotionTypes");

	pXML->SetOptionalIntVector(&m_aiTrapImmunityUnitCombatTypes, L"TrapImmunityUnitCombatTypes");

	// int vectors utilizing struct with delayed resolution
	if(pXML->TryMoveToXmlFirstChild(L"AfflictionFortitudeModifiers"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"AfflictionFortitudeModifier" );
		m_aAfflictionFortitudeModifiers.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"AfflictionFortitudeModifier"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"PromotionLineType");
					m_aAfflictionFortitudeModifiers[i].ePromotionLine = (PromotionLineTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aAfflictionFortitudeModifiers[i].iModifier), L"iModifier");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"AfflictionFortitudeModifier"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"AfflictOnAttackTypes"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"AfflictOnAttackType" );
		m_aAfflictOnAttackTypes.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"AfflictOnAttackType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"PromotionLineType");
					m_aAfflictOnAttackTypes[i].eAfflictionLine = (PromotionLineTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aAfflictOnAttackTypes[i].iProbability), L"iProbability");
					pXML->GetChildXmlValByName(&(m_aAfflictOnAttackTypes[i].iMelee), L"iMelee");
					pXML->GetChildXmlValByName(&(m_aAfflictOnAttackTypes[i].iDistance), L"iDistance");
					pXML->GetChildXmlValByName(&(m_aAfflictOnAttackTypes[i].iImmediate), L"iImmediate");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"AfflictOnAttackType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"HealUnitCombatTypes"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"HealUnitCombatType" );
		m_aHealUnitCombatTypes.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"HealUnitCombatType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					m_aHealUnitCombatTypes[i].eUnitCombat = (UnitCombatTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aHealUnitCombatTypes[i].iHeal), L"iHeal");
					pXML->GetChildXmlValByName(&(m_aHealUnitCombatTypes[i].iAdjacentHeal), L"iAdjacentHeal");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"HealUnitCombatType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"GroupSpawnUnitCombatTypes"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"GroupSpawnUnitCombatType" );
		m_aGroupSpawnUnitCombatTypes.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"GroupSpawnUnitCombatType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					m_aGroupSpawnUnitCombatTypes[i].eUnitCombat = (UnitCombatTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aGroupSpawnUnitCombatTypes[i].iChance), L"iChance");
					pXML->GetOptionalChildXmlValByName(m_aGroupSpawnUnitCombatTypes[i].m_szTitle, L"Title");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"GroupSpawnUnitCombatType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"InvisibleTerrainChanges"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"InvisibleTerrainChange" );
		m_aInvisibleTerrainChanges.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"InvisibleTerrainChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"InvisibleType");
					m_aInvisibleTerrainChanges[i].eInvisible = (InvisibleTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"TerrainType");
					m_aInvisibleTerrainChanges[i].eTerrain = (TerrainTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aInvisibleTerrainChanges[i].iIntensity), L"iIntensity");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"InvisibleTerrainChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"InvisibleFeatureChanges"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"InvisibleFeatureChange" );
		m_aInvisibleFeatureChanges.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"InvisibleFeatureChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"InvisibleType");
					m_aInvisibleFeatureChanges[i].eInvisible = (InvisibleTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"FeatureType");
					m_aInvisibleFeatureChanges[i].eFeature = (FeatureTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aInvisibleFeatureChanges[i].iIntensity), L"iIntensity");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"InvisibleFeatureChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"InvisibleImprovementChanges"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"InvisibleImprovementChange" );
		m_aInvisibleImprovementChanges.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"InvisibleImprovementChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"InvisibleType");
					m_aInvisibleImprovementChanges[i].eInvisible = (InvisibleTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"ImprovementType");
					m_aInvisibleImprovementChanges[i].eImprovement = (ImprovementTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aInvisibleImprovementChanges[i].iIntensity), L"iIntensity");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"InvisibleImprovementChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"VisibleTerrainChanges"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"VisibleTerrainChange" );
		m_aVisibleTerrainChanges.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"VisibleTerrainChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"InvisibleType");
					m_aVisibleTerrainChanges[i].eInvisible = (InvisibleTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"TerrainType");
					m_aVisibleTerrainChanges[i].eTerrain = (TerrainTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aVisibleTerrainChanges[i].iIntensity), L"iIntensity");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"VisibleTerrainChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"VisibleFeatureChanges"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"VisibleFeatureChange" );
		m_aVisibleFeatureChanges.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"VisibleFeatureChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"InvisibleType");
					m_aVisibleFeatureChanges[i].eInvisible = (InvisibleTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"FeatureType");
					m_aVisibleFeatureChanges[i].eFeature = (FeatureTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aVisibleFeatureChanges[i].iIntensity), L"iIntensity");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"VisibleFeatureChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"VisibleImprovementChanges"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"VisibleImprovementChange" );
		m_aVisibleImprovementChanges.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"VisibleImprovementChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"InvisibleType");
					m_aVisibleImprovementChanges[i].eInvisible = (InvisibleTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"ImprovementType");
					m_aVisibleImprovementChanges[i].eImprovement = (ImprovementTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aVisibleImprovementChanges[i].iIntensity), L"iIntensity");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"VisibleImprovementChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"VisibleTerrainRangeChanges"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"VisibleTerrainRangeChange" );
		m_aVisibleTerrainRangeChanges.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"VisibleTerrainRangeChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"InvisibleType");
					pXML->GetChildXmlValByName(szTextVal2, L"TerrainType");
					m_aVisibleTerrainRangeChanges[i].eTerrain = (TerrainTypes)pXML->GetInfoClass(szTextVal2);
					pXML->GetChildXmlValByName(&(m_aVisibleTerrainRangeChanges[i].iIntensity), L"iIntensity");
					GC.addDelayedResolution((int*)&(m_aVisibleTerrainRangeChanges[i].eInvisible), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"VisibleTerrainRangeChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"VisibleFeatureRangeChanges"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"VisibleFeatureRangeChange" );
		m_aVisibleFeatureRangeChanges.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"VisibleFeatureRangeChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"InvisibleType");
					m_aVisibleFeatureRangeChanges[i].eInvisible = (InvisibleTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"FeatureType");
					m_aVisibleFeatureRangeChanges[i].eFeature = (FeatureTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aVisibleFeatureRangeChanges[i].iIntensity), L"iIntensity");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"VisibleFeatureRangeChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"VisibleImprovementRangeChanges"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"VisibleImprovementRangeChange" );
		m_aVisibleImprovementRangeChanges.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"VisibleImprovementRangeChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"InvisibleType");
					m_aVisibleImprovementRangeChanges[i].eInvisible = (InvisibleTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"ImprovementType");
					m_aVisibleImprovementRangeChanges[i].eImprovement = (ImprovementTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aVisibleImprovementRangeChanges[i].iIntensity), L"iIntensity");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"VisibleImprovementRangeChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"DistanceAttackCommunicabilityTypeChanges"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"DistanceAttackCommunicabilityTypeChange" );
		m_aDistanceAttackCommunicabilityTypeChanges.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"DistanceAttackCommunicabilityTypeChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"PromotionLineType");
					m_aDistanceAttackCommunicabilityTypeChanges[i].eAfflictionLine = (PromotionLineTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aDistanceAttackCommunicabilityTypeChanges[i].iChange), L"iChange");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"DistanceAttackCommunicabilityTypeChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"EnabledCivilizationTypes"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"EnabledCivilizationType" );
		m_aEnabledCivilizationTypes.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"EnabledCivilizationType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"CivilizationType");
					GC.addDelayedResolution((int*)&(m_aEnabledCivilizationTypes[i].eCivilization), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"EnabledCivilizationType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	// int vector utilizing pairing without delayed resolution
	pXML->SetOptionalPairVector<TerrainModifierArray, TerrainTypes, int>(&m_aWithdrawOnTerrainTypes, L"WithdrawModifierOnTerrainTypes");

	pXML->SetOptionalPairVector<FeatureModifierArray, FeatureTypes, int>(&m_aWithdrawOnFeatureTypes, L"WithdrawModifierOnFeatureTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aFlankingStrengthbyUnitCombatType, L"FlankingStrikesbyUnitCombat");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aWithdrawVSUnitCombatTypes, L"WithdrawVSUnitCombatTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aPursuitVSUnitCombatTypes, L"PursuitVSUnitCombatTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aRepelVSUnitCombatTypes, L"RepelVSUnitCombatTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aKnockbackVSUnitCombatTypes, L"KnockbackVSUnitCombatTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aPunctureVSUnitCombatTypes, L"PunctureVSUnitCombatTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aArmorVSUnitCombatTypes, L"ArmorVSUnitCombatTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aDodgeVSUnitCombatTypes, L"DodgeVSUnitCombatTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aPrecisionVSUnitCombatTypes, L"PrecisionVSUnitCombatTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aCriticalVSUnitCombatTypes, L"CriticalVSUnitCombatTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aRoundStunVSUnitCombatTypes, L"RoundStunVSUnitCombatTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aTrapDisableUnitCombatTypes, L"TrapDisableUnitCombatTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aTrapAvoidanceUnitCombatTypes, L"TrapAvoidanceUnitCombatTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aTrapTriggerUnitCombatTypes, L"TrapTriggerUnitCombatTypes");

	pXML->SetOptionalPairVector<InvisibilityArray, InvisibleTypes, int>(&m_aVisibilityIntensityTypes, L"VisibilityIntensityTypes");

	pXML->SetOptionalPairVector<InvisibilityArray, InvisibleTypes, int>(&m_aInvisibilityIntensityTypes, L"InvisibilityIntensityTypes");

//Team Project (4)
		//WorkRateMod
	pXML->SetOptionalPairVector<TerrainModifierArray, TerrainTypes, int>(&m_aTerrainWorkRateModifierTypes, L"TerrainWorkRateModifierTypes");

	pXML->SetOptionalPairVector<FeatureModifierArray, FeatureTypes, int>(&m_aFeatureWorkRateModifierTypes, L"FeatureWorkRateModifierTypes");

	pXML->SetOptionalPairVector<BuildModifierArray, BuildTypes, int>(&m_aBuildWorkRateModifierTypes, L"BuildWorkRateModifierTypes");

	pXML->SetOptionalPairVector<AidArray, PropertyTypes, int>(&m_aAidChanges, L"AidChanges");

	//TB Combat Mods End  TB SubCombat Mod end

	getKillOutcomeList()->read(pXML, L"KillOutcomes");

	if(pXML->TryMoveToXmlFirstChild(L"Actions"))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"Action"))
			{
				int i = 0;
				do
				{
					m_aOutcomeMissions.push_back(new CvOutcomeMission());
					m_aOutcomeMissions[i]->read(pXML);
					i++;
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

/************************************************************************************************/
/* XMLCOPY								 10/10/07								MRGENIE	  */
/*																							  */
/* if we're using a module, assuming the meshgroup isn't set, we update first after			 */
/* the copyNonDefaults method																   */
/************************************************************************************************/
/*
	updateArtDefineButton();
*/
	if (m_iGroupSize != 0)
	{
		updateArtDefineButton();
	}

	//	Auto-correct units that are animals but don't have the naimal AI set as their default
	//	(this situation currently confuses the AI so disallow it)
	//TB Animal Search note: THIS will have to be unwritted to allow for multiple animal AI types!!!  leaving alone for now but WILL be removed eventually
	// Alberts2 Commented this out to allow Animals with a different AI
	//if ( isAnimal() && getDefaultUnitAIType() != UNITAI_ANIMAL )
	//{
	//	FAssertMsg(false, CvString::format("Auto-corrected: UnitAI not set to UNITAI_ANIMAL for animal unit %s", m_szType.c_str()).c_str());
	//	m_iDefaultUnitAIType = UNITAI_ANIMAL;
	//}

	return true;
}

void CvUnitInfo::copyNonDefaults(CvUnitInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	CvHotkeyInfo::copyNonDefaults(pClassInfo, pXML);

	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;
	CvString cDefault = CvString::format("").GetCString();

	if ( m_iUnitClassType == iTextDefault )	m_iUnitClassType = pClassInfo->getUnitClassType();
	if ( m_iSpecialUnitType == iTextDefault )	m_iSpecialUnitType = pClassInfo->getSpecialUnitType();
	if ( m_iUnitCaptureClassType == iTextDefault )	m_iUnitCaptureClassType = pClassInfo->getUnitCaptureClassType();
	if ( m_iUnitCombatType == iTextDefault )	m_iUnitCombatType = pClassInfo->getUnitCombatType();
	if ( m_iDomainType == iTextDefault )	m_iDomainType = pClassInfo->getDomainType();
	if ( m_iDefaultUnitAIType == UNITAI_UNKNOWN )	m_iDefaultUnitAIType = pClassInfo->getDefaultUnitAIType();
	if ( m_iInvisibleType == iTextDefault )	m_iInvisibleType = pClassInfo->getInvisibleType();

	if ( getNumSeeInvisibleTypes() < 1 )
	{
		for(int i=0; i < pClassInfo->getNumSeeInvisibleTypes(); i++)
		{
			int iSeeInvisibleType = pClassInfo->getSeeInvisibleType(i);
			if(iSeeInvisibleType != NO_INVISIBLE)
			{
				m_aiSeeInvisibleTypes.push_back(iSeeInvisibleType);
			}
		}
	}
	if ( m_iAdvisorType == iTextDefault )	m_iAdvisorType = pClassInfo->getAdvisorType();

	if ( m_bFoodProduction == bDefault )	m_bFoodProduction = pClassInfo->isFoodProduction();
	if ( m_bNoBadGoodies == bDefault )	m_bNoBadGoodies = pClassInfo->isNoBadGoodies();
	if ( m_bOnlyDefensive == bDefault )	m_bOnlyDefensive = pClassInfo->isOnlyDefensive();
	if ( m_bNoCapture == bDefault )	m_bNoCapture = pClassInfo->isNoCapture();
	if ( m_bQuickCombat == bDefault )	m_bQuickCombat = pClassInfo->isQuickCombat();
	if ( m_bRivalTerritory == bDefault )	m_bRivalTerritory = pClassInfo->isRivalTerritory();
	if ( m_bMilitaryHappiness == bDefault )	m_bMilitaryHappiness = pClassInfo->isMilitaryHappiness();
	if ( m_bMilitarySupport == bDefault )	m_bMilitarySupport = pClassInfo->isMilitarySupport();
	if ( m_bMilitaryProduction == bDefault )	m_bMilitaryProduction = pClassInfo->isMilitaryProduction();
	if ( m_bPillage == bDefault )	m_bPillage = pClassInfo->isPillage();
	if ( m_bSpy == bDefault )	m_bSpy = pClassInfo->isSpy();
	if ( m_bSabotage == bDefault )	m_bSabotage = pClassInfo->isSabotage();
	if ( m_bDestroy == bDefault )	m_bDestroy = pClassInfo->isDestroy();
	if ( m_bStealPlans == bDefault )	m_bStealPlans = pClassInfo->isStealPlans();
	if ( m_bInvestigate == bDefault )	m_bInvestigate = pClassInfo->isInvestigate();
	if ( m_bCounterSpy == bDefault )	m_bCounterSpy = pClassInfo->isCounterSpy();
	if ( m_bFound == bDefault )	m_bFound = pClassInfo->isFound();
	if ( m_bGoldenAge == bDefault )	m_bGoldenAge = pClassInfo->isGoldenAge();
	if ( m_bInvisible == bDefault )	m_bInvisible = pClassInfo->isInvisible();
	if ( m_bFirstStrikeImmune == bDefault )	m_bFirstStrikeImmune = pClassInfo->isFirstStrikeImmune();
	if ( m_bNoDefensiveBonus == bDefault )	m_bNoDefensiveBonus = pClassInfo->isNoDefensiveBonus();
	if ( m_bIgnoreBuildingDefense == bDefault )	m_bIgnoreBuildingDefense = pClassInfo->isIgnoreBuildingDefense();
	if ( m_bCanMoveImpassable == bDefault )	m_bCanMoveImpassable = pClassInfo->isCanMoveImpassable();
	if ( m_bCanMoveAllTerrain == bDefault )	m_bCanMoveAllTerrain = pClassInfo->isCanMoveAllTerrain();
	if ( m_bFlatMovementCost == bDefault )	m_bFlatMovementCost = pClassInfo->isFlatMovementCost();
	if ( m_bIgnoreTerrainCost == bDefault )	m_bIgnoreTerrainCost = pClassInfo->isIgnoreTerrainCost();
	if ( m_bNukeImmune == bDefault )	m_bNukeImmune = pClassInfo->isNukeImmune();
	if ( m_bPrereqBonuses == bDefault )	m_bPrereqBonuses = pClassInfo->isPrereqBonuses();
	if ( m_bPrereqReligion == bDefault )	m_bPrereqReligion = pClassInfo->isPrereqReligion();
	if ( m_bMechanized == bDefault )	m_bMechanized = pClassInfo->isMechUnit();
	if ( m_bRenderBelowWater == bDefault )	m_bRenderBelowWater = pClassInfo->isRenderBelowWater();
	if ( m_bRenderAlways == bDefault )	m_bRenderAlways = pClassInfo->isRenderAlways();
	if ( m_bSuicide == bDefault )	m_bSuicide = pClassInfo->isSuicide();
	if ( m_bLineOfSight == bDefault )	m_bLineOfSight = pClassInfo->isLineOfSight();
	if ( m_bHiddenNationality == bDefault )	m_bHiddenNationality = pClassInfo->isHiddenNationality();
	if ( m_bAlwaysHostile == bDefault )	m_bAlwaysHostile = pClassInfo->isAlwaysHostile();
	if ( m_bNoRevealMap == bDefault )	m_bNoRevealMap = pClassInfo->isNoRevealMap();

/********************************************************************************/
/**		REVDCM_OC								2/16/10				phungus420	*/
/**																				*/
/**		 																		*/
/********************************************************************************/
	if ( m_bInquisitor == bDefault )	m_bInquisitor = pClassInfo->isInquisitor();
/********************************************************************************/
/**		REVDCM_OC								END								*/
/********************************************************************************/
	//ls612: Can't enter non-Owned cities
	if ( m_bNoNonOwnedEntry == bDefault ) m_bNoNonOwnedEntry = pClassInfo->isNoNonOwnedEntry();

	for ( int i = 0; i < GC.getNumUnitClassInfos(); i++)
	{
		if ( getUpgradeUnitClass(i) == bDefault && pClassInfo->getUpgradeUnitClass(i) != bDefault)
		{
			if ( m_pbUpgradeUnitClass == NULL )
			{
				CvXMLLoadUtility::InitList(&m_pbUpgradeUnitClass,GC.getNumUnitClassInfos(),bDefault);
			}
			m_pbUpgradeUnitClass[i] = pClassInfo->getUpgradeUnitClass(i);
		}
		if ( getTargetUnitClass(i) == bDefault && pClassInfo->getTargetUnitClass(i) != bDefault)
		{
			if ( m_pbTargetUnitClass == NULL )
			{
				CvXMLLoadUtility::InitList(&m_pbTargetUnitClass,GC.getNumUnitClassInfos(),bDefault);
			}
			m_pbTargetUnitClass[i] = pClassInfo->getTargetUnitClass(i);
		}
		if ( getDefenderUnitClass(i) == bDefault && pClassInfo->getDefenderUnitClass(i) != bDefault)
		{
			if ( m_pbDefenderUnitClass == NULL )
			{
				CvXMLLoadUtility::InitList(&m_pbDefenderUnitClass,GC.getNumUnitClassInfos(),bDefault);
			}
			m_pbDefenderUnitClass[i] = pClassInfo->getDefenderUnitClass(i);
		}
		if ( getFlankingStrikeUnitClass(i) == -1 && pClassInfo->getFlankingStrikeUnitClass(i) != -1)
		{
			if ( m_piFlankingStrikeUnitClass == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piFlankingStrikeUnitClass,GC.getNumUnitClassInfos(),-1);
			}
			m_piFlankingStrikeUnitClass[i] = pClassInfo->getFlankingStrikeUnitClass(i);
		}
		if ( getUnitClassAttackModifier(i) == iDefault && pClassInfo->getUnitClassAttackModifier(i) != iDefault)
		{
			if ( m_piUnitClassAttackModifier == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piUnitClassAttackModifier,GC.getNumUnitClassInfos(),iDefault);
			}
			m_piUnitClassAttackModifier[i] = pClassInfo->getUnitClassAttackModifier(i);
		}
		if ( getUnitClassDefenseModifier(i) == iDefault && pClassInfo->getUnitClassDefenseModifier(i) != iDefault)
		{
			if ( m_piUnitClassDefenseModifier == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piUnitClassDefenseModifier,GC.getNumUnitClassInfos(),iDefault);
			}
			m_piUnitClassDefenseModifier[i] = pClassInfo->getUnitClassDefenseModifier(i);
		}

	}

	for ( int i = 0; i < GC.getNumUnitCombatInfos(); i++)
	{
		if ( getTargetUnitCombat(i) == bDefault && pClassInfo->getTargetUnitCombat(i) != bDefault)
		{
			if ( NULL == m_pbTargetUnitCombat )
			{
				CvXMLLoadUtility::InitList(&m_pbTargetUnitCombat,GC.getNumUnitCombatInfos(),bDefault);
			}
			m_pbTargetUnitCombat[i] = pClassInfo->getTargetUnitCombat(i);
		}
		if ( getDefenderUnitCombat(i) == bDefault && pClassInfo->getDefenderUnitCombat(i) != bDefault)
		{
			if ( NULL == m_pbDefenderUnitCombat )
			{
				CvXMLLoadUtility::InitList(&m_pbDefenderUnitCombat,GC.getNumUnitCombatInfos(),bDefault);
			}
			m_pbDefenderUnitCombat[i] = pClassInfo->getDefenderUnitCombat(i);
		}
		if ( getUnitCombatModifier(i) == iDefault && pClassInfo->getUnitCombatModifier(i) != iDefault )
		{
			if ( NULL == m_piUnitCombatModifier )
			{
				CvXMLLoadUtility::InitList(&m_piUnitCombatModifier,GC.getNumUnitCombatInfos(),iDefault);
			}
			m_piUnitCombatModifier[i] = pClassInfo->getUnitCombatModifier(i);
		}
		if ( getUnitCombatCollateralImmune(i) == iDefault && pClassInfo->getUnitCombatCollateralImmune(i) != iDefault)
		{
			if ( NULL == m_piUnitCombatCollateralImmune )
			{
				CvXMLLoadUtility::InitList(&m_piUnitCombatCollateralImmune,GC.getNumUnitCombatInfos(),iDefault);
			}
			m_piUnitCombatCollateralImmune[i] = pClassInfo->getUnitCombatCollateralImmune(i);
		}
	}

	for ( int i = 0; i < NUM_UNITAI_TYPES; i++)
	{
		if ( getUnitAIType(i) == bDefault && pClassInfo->getUnitAIType(i) != bDefault)
		{
			if ( NULL == m_pbUnitAIType )
			{
				CvXMLLoadUtility::InitList(&m_pbUnitAIType,NUM_UNITAI_TYPES,bDefault);
			}
			m_pbUnitAIType[i] = pClassInfo->getUnitAIType(i);
		}
		if ( getNotUnitAIType(i) == bDefault && pClassInfo->getNotUnitAIType(i) != bDefault)
		{
			if ( NULL == m_pbNotUnitAIType )
			{
				CvXMLLoadUtility::InitList(&m_pbNotUnitAIType,NUM_UNITAI_TYPES,bDefault);
			}
			m_pbNotUnitAIType[i] = pClassInfo->getNotUnitAIType(i);
		}
	}

	for ( int i = 0; i < GC.getNumBuildInfos(); i++)
	{
		if ( getBuilds(i) == bDefault && pClassInfo->getBuilds(i) != bDefault)
		{
			if ( NULL == m_pbBuilds )
			{
				CvXMLLoadUtility::InitList(&m_pbBuilds,GC.getNumBuildInfos(),bDefault);
			}
			m_pbBuilds[i] = pClassInfo->getBuilds(i);
		}
	}

	CvXMLLoadUtility::CopyNonDefaultsFromIntVector(m_pbBuildings, pClassInfo->m_pbBuildings);

	for ( int i = 0; i < GC.getNumReligionInfos(); i++)
	{
		if ( getReligionSpreads(i) == -1 && pClassInfo->getReligionSpreads(i) != -1)
		{
			if ( NULL == m_piReligionSpreads )
			{
				CvXMLLoadUtility::InitList(&m_piReligionSpreads,GC.getNumReligionInfos(),-1);
			}
			m_piReligionSpreads[i] = pClassInfo->getReligionSpreads(i);
		}
	}

	for ( int i = 0; i < GC.getNumCorporationInfos(); i++)
	{
		if ( getCorporationSpreads(i) == -1 && pClassInfo->getCorporationSpreads(i) != -1 )
		{
			if ( NULL == m_piCorporationSpreads)
			{
				CvXMLLoadUtility::InitList(&m_piCorporationSpreads,GC.getNumCorporationInfos(),-1);
			}
			m_piCorporationSpreads[i] = pClassInfo->getCorporationSpreads(i);
		}
	}

	//for ( int i = 0; i < GC.getNumTraitInfos(); i++)
	//{
	//	if ( getProductionTraits(i) == iDefault && pClassInfo->getProductionTraits(i) != iDefault)
	//	{
	//		if ( NULL == m_piProductionTraits )
	//		{
	//			CvXMLLoadUtility::InitList(&m_piProductionTraits,GC.getNumTraitInfos(),iDefault);
	//		}
	//		m_piProductionTraits[i] = pClassInfo->getProductionTraits(i);
	//	}
	//}

	for ( int i = 0; i < GC.getNumFlavorTypes(); i++)
	{
		if ( getFlavorValue(i) == iDefault && pClassInfo->getFlavorValue(i) != iDefault)
		{
			if ( NULL == m_piFlavorValue )
			{
				CvXMLLoadUtility::InitList(&m_piFlavorValue,GC.getNumFlavorTypes(),iDefault);
			}
			m_piFlavorValue[i] = pClassInfo->getFlavorValue(i);
		}
	}

	for ( int i = 0; i < GC.getNumSpecialistInfos(); i++)
	{
		if ( getGreatPeoples(i) == bDefault && pClassInfo->getGreatPeoples(i) != bDefault)
		{
			if ( NULL == m_pbGreatPeoples )
			{
				CvXMLLoadUtility::InitList(&m_pbGreatPeoples,GC.getNumSpecialistInfos(),bDefault);
			}
			m_pbGreatPeoples[i] = pClassInfo->getGreatPeoples(i);
		}
	}

	for ( int i = 0; i < GC.getNumTerrainInfos(); i++)
	{
/*		if ( getTerrainImpassable(i) == bDefault && pClassInfo->getTerrainImpassable(i) != bDefault)
		{
			if ( NULL == m_pbTerrainImpassable )
			{
				CvXMLLoadUtility::InitList(&m_pbTerrainImpassable,GC.getNumTerrainInfos(),bDefault);
			}
			m_pbTerrainImpassable[i] = pClassInfo->getTerrainImpassable(i);
		}*/
		if ( getTerrainNative(i) == bDefault && pClassInfo->getTerrainNative(i) != bDefault)
		{
			if ( NULL == m_pbTerrainNative )
			{
				CvXMLLoadUtility::InitList(&m_pbTerrainNative,GC.getNumTerrainInfos(),bDefault);
			}
			m_pbTerrainNative[i] = pClassInfo->getTerrainNative(i);
		}
		if ( getTerrainAttackModifier(i) == iDefault && pClassInfo->getTerrainAttackModifier(i) != iDefault)
		{
			if ( NULL == m_piTerrainAttackModifier )
			{
				CvXMLLoadUtility::InitList(&m_piTerrainAttackModifier,GC.getNumTerrainInfos(),iDefault);
			}
			m_piTerrainAttackModifier[i] = pClassInfo->getTerrainAttackModifier(i);
		}
		if ( getTerrainDefenseModifier(i) == iDefault && pClassInfo->getTerrainDefenseModifier(i) != iDefault)
		{
			if ( NULL == m_piTerrainDefenseModifier )
			{
				CvXMLLoadUtility::InitList(&m_piTerrainDefenseModifier,GC.getNumTerrainInfos(),iDefault);
			}
			m_piTerrainDefenseModifier[i] = pClassInfo->getTerrainDefenseModifier(i);
		}
		if ( getTerrainPassableTech(i) == NO_TECH && pClassInfo->getTerrainPassableTech(i) != NO_TECH )
		{
			if ( NULL == m_piTerrainPassableTech )
			{
				CvXMLLoadUtility::InitList(&m_piTerrainPassableTech,GC.getNumTerrainInfos(),(int)NO_TECH);
			}
			m_piTerrainPassableTech[i] = pClassInfo->getTerrainPassableTech(i);
		}
	}

	for ( int i = 0; i < GC.getNumFeatureInfos(); i++)
	{
		//if ( getFeatureImpassable(i) == bDefault && pClassInfo->getFeatureImpassable(i) != bDefault)
		//{
		//	if (m_pbFeatureImpassable == NULL)
		//	{
		//		CvXMLLoadUtility::InitList(&m_pbFeatureImpassable,GC.getNumFeatureInfos(),bDefault);
		//	}
		//	m_pbFeatureImpassable[i] = pClassInfo->getFeatureImpassable(i);
		//}
		if ( getFeatureNative(i) == bDefault && pClassInfo->getFeatureNative(i) != bDefault)
		{
			if (m_pbFeatureNative == NULL)
			{
				CvXMLLoadUtility::InitList(&m_pbFeatureNative,GC.getNumFeatureInfos(),bDefault);
			}
			m_pbFeatureNative[i] = pClassInfo->getFeatureNative(i);
		}
		if ( getFeatureAttackModifier(i) == iDefault && pClassInfo->getFeatureAttackModifier(i) != iDefault)
		{
			if (m_piFeatureAttackModifier == NULL)
			{
				CvXMLLoadUtility::InitList(&m_piFeatureAttackModifier,GC.getNumFeatureInfos(),iDefault);
			}
			m_piFeatureAttackModifier[i] = pClassInfo->getFeatureAttackModifier(i);
		}
		if ( getFeatureDefenseModifier(i) == iDefault && pClassInfo->getFeatureDefenseModifier(i) != iDefault)
		{
			if (m_piFeatureDefenseModifier == NULL)
			{
				CvXMLLoadUtility::InitList(&m_piFeatureDefenseModifier,GC.getNumFeatureInfos(),iDefault);
			}
			m_piFeatureDefenseModifier[i] = pClassInfo->getFeatureDefenseModifier(i);
		}
		if ( getFeaturePassableTech(i) == NO_TECH && pClassInfo->getFeaturePassableTech(i) != NO_TECH )
		{
			if ( NULL == m_piFeaturePassableTech )
			{
				CvXMLLoadUtility::InitList(&m_piFeaturePassableTech,GC.getNumFeatureInfos(),(int)NO_TECH);
			}
			m_piFeaturePassableTech[i] = pClassInfo->getFeaturePassableTech(i);
		}
	}

	for ( int i = 0; i < NUM_DOMAIN_TYPES; i++)
	{
		if ( getDomainModifier(i) == iDefault && pClassInfo->getDomainModifier(i) != iDefault)
		{
			if ( NULL == m_piDomainModifier )
			{
				CvXMLLoadUtility::InitList(&m_piDomainModifier,NUM_DOMAIN_TYPES,iDefault);
			}
			m_piDomainModifier[i] = pClassInfo->getDomainModifier(i);
		}
	}

	for ( int i = 0; i < GC.getNumBonusInfos(); i++)
	{
		if ( getBonusProductionModifier(i) == iDefault && pClassInfo->getBonusProductionModifier(i) != iDefault)
		{
			if ( NULL == m_piBonusProductionModifier )
			{
				CvXMLLoadUtility::InitList(&m_piBonusProductionModifier,GC.getNumBonusInfos(),iDefault);
			}
			m_piBonusProductionModifier[i] = pClassInfo->getBonusProductionModifier(i);
		}
	}

	for ( int i = 0; i < GC.getNumPromotionInfos(); i++)
	{
		if ( getFreePromotions(i) == bDefault && pClassInfo->getFreePromotions(i) != bDefault)
		{
			if ( NULL == m_pbFreePromotions )
			{
				CvXMLLoadUtility::InitList(&m_pbFreePromotions,GC.getNumPromotionInfos(),bDefault);
			}
			m_pbFreePromotions[i] = pClassInfo->getFreePromotions(i);
		}
	}

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain																*/
/********************************************************************************/
	for ( int i = 0; i < GC.getNumCivicInfos(); i++)
	{
		if ( isPrereqOrCivics(i) == bDefault && pClassInfo->isPrereqOrCivics(i) != bDefault)
		{
			if ( NULL == m_pbPrereqOrCivics )
			{
				CvXMLLoadUtility::InitList(&m_pbPrereqOrCivics,GC.getNumCivicInfos(),bDefault);
			}
			m_pbPrereqOrCivics[i] = pClassInfo->isPrereqOrCivics(i);
		}
	}

	for ( int i = 0; i < GC.getNumBuildingClassInfos(); i++)
	{
		if ( isPrereqBuildingClass(i) == bDefault && pClassInfo->isPrereqBuildingClass(i) != bDefault)
		{
			if ( NULL == m_pbPrereqBuildingClass )
			{
				CvXMLLoadUtility::InitList(&m_pbPrereqBuildingClass,GC.getNumBuildingClassInfos(),bDefault);
			}
			m_pbPrereqBuildingClass[i] = pClassInfo->isPrereqBuildingClass(i);
		}
	}

	for ( int i = 0; i < GC.getNumBuildingClassInfos(); i++)
	{
		if ( getPrereqBuildingClassOverrideTech(i) == -1 && pClassInfo->getPrereqBuildingClassOverrideTech(i) != -1 )
		{
			if ( NULL == m_piPrereqBuildingClassOverrideTech )
			{
				CvXMLLoadUtility::InitList(&m_piPrereqBuildingClassOverrideTech,GC.getNumBuildingClassInfos(),-1);
			}
			m_piPrereqBuildingClassOverrideTech[i] = pClassInfo->getPrereqBuildingClassOverrideTech(i);
		}
	}

	for ( int i = 0; i < GC.getNumBuildingClassInfos(); i++)
	{
		if ( getPrereqBuildingClassOverrideEra(i) == -1 && pClassInfo->getPrereqBuildingClassOverrideEra(i) != -1 )
		{
			if ( NULL == m_piPrereqBuildingClassOverrideEra )
			{
				CvXMLLoadUtility::InitList(&m_piPrereqBuildingClassOverrideEra,GC.getNumBuildingClassInfos(),-1);
			}
			m_piPrereqBuildingClassOverrideEra[i] = pClassInfo->getPrereqBuildingClassOverrideEra(i);
		}
	}

	for ( int i = 0; i < GC.getNumUnitClassInfos(); i++)
	{
		if ( getForceObsoleteUnitClass(i) == bDefault && pClassInfo->getForceObsoleteUnitClass(i) != bDefault)
		{
			if ( NULL == m_pbForceObsoleteUnitClass )
			{
				CvXMLLoadUtility::InitList(&m_pbForceObsoleteUnitClass,GC.getNumUnitClassInfos(),bDefault);
			}
			m_pbForceObsoleteUnitClass[i] = pClassInfo->getForceObsoleteUnitClass(i);
		}
	}
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

	for ( int i = 0; i < GC.getNUM_UNIT_AND_TECH_PREREQS(); i++)
	{
		if ( getPrereqAndTechs(i) == NO_TECH && pClassInfo->getPrereqAndTechs(i) != NO_TECH)
		{
			if ( NULL == m_piPrereqAndTechs )
			{
				CvXMLLoadUtility::InitList(&m_piPrereqAndTechs,GC.getNUM_UNIT_AND_TECH_PREREQS(),(int)NO_TECH);
			}
			m_piPrereqAndTechs[i] = pClassInfo->getPrereqAndTechs(i);
		}
	}

	for ( int i = 0; i < GC.getNUM_UNIT_PREREQ_OR_BONUSES(); i++)
	{
		if ( getPrereqOrBonuses(i) == NO_BONUS && pClassInfo->getPrereqOrBonuses(i) != NO_BONUS)
		{
			if ( NULL == m_piPrereqOrBonuses )
			{
				CvXMLLoadUtility::InitList(&m_piPrereqOrBonuses,GC.getNUM_UNIT_PREREQ_OR_BONUSES(),(int)NO_BONUS);
			}
			m_piPrereqOrBonuses[i] = pClassInfo->getPrereqOrBonuses(i);
		}
	}
	/********************************************************************************/
	/*		REVOLUTION_MOD							2/12/09				Afforess	*/
	/*																				*/
	/*		 																		*/
	/********************************************************************************/
	if ( m_iDCMBombRange == iDefault ) m_iDCMBombRange = pClassInfo->getDCMBombRange();
	if ( m_iDCMBombAccuracy == iDefault ) m_iDCMBombAccuracy = pClassInfo->getDCMBombAccuracy();

	if ( m_bDCMAirBomb1 == bDefault ) m_bDCMAirBomb1 = pClassInfo->getDCMAirBomb1();
	if ( m_bDCMAirBomb2 == bDefault ) m_bDCMAirBomb2 = pClassInfo->getDCMAirBomb2();
	if ( m_bDCMAirBomb3 == bDefault ) m_bDCMAirBomb3 = pClassInfo->getDCMAirBomb3();
	if ( m_bDCMAirBomb4 == bDefault ) m_bDCMAirBomb4 = pClassInfo->getDCMAirBomb4();
	if ( m_bDCMAirBomb5 == bDefault ) m_bDCMAirBomb5 = pClassInfo->getDCMAirBomb5();

	if ( m_bDCMFighterEngage == bDefault ) m_bDCMFighterEngage = pClassInfo->getDCMFighterEngage();
	/********************************************************************************/
	/*		REVOLUTION_MOD							END								*/
	/********************************************************************************/

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain																*/
/********************************************************************************/
	if ( m_iMaxStartEra == iTextDefault) m_iMaxStartEra = pClassInfo->getMaxStartEra();
	if ( m_iForceObsoleteTech == iTextDefault ) m_iForceObsoleteTech = pClassInfo->getForceObsoleteTech();
	if ( m_bStateReligion == bDefault )	m_bStateReligion = pClassInfo->isStateReligion();
	if ( m_iPrereqGameOption == iTextDefault ) m_iPrereqGameOption = pClassInfo->getPrereqGameOption();
	if ( m_iNotGameOption == iTextDefault ) m_iNotGameOption = pClassInfo->getNotGameOption();
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

	if ( m_iHolyCity == iTextDefault ) m_iHolyCity = pClassInfo->getHolyCity();
	if ( m_iReligionType == iTextDefault ) m_iReligionType = pClassInfo->getReligionType();
	if ( m_iStateReligion == iTextDefault ) m_iStateReligion = pClassInfo->getStateReligion();
	if ( m_iPrereqReligion == iTextDefault ) m_iPrereqReligion = pClassInfo->getPrereqReligion();
	if ( m_iPrereqCorporation == iTextDefault ) m_iPrereqCorporation = pClassInfo->getPrereqCorporation();
	if ( m_iPrereqBuilding == iTextDefault ) m_iPrereqBuilding = pClassInfo->getPrereqBuilding();
	CvXMLLoadUtility::CopyNonDefaultsFromIntVector(m_aePrereqOrBuildings, pClassInfo->m_aePrereqOrBuildings);
	if ( m_iPrereqAndTech == iTextDefault ) m_iPrereqAndTech = pClassInfo->getPrereqAndTech();
	if ( m_iPrereqAndBonus == iTextDefault ) m_iPrereqAndBonus = pClassInfo->getPrereqAndBonus();

	if ( m_iAIWeight == iDefault ) m_iAIWeight = pClassInfo->getAIWeight();
	if ( m_iProductionCost == iDefault ) m_iProductionCost = pClassInfo->getProductionCost();
	if ( m_iHurryCostModifier == iDefault ) m_iHurryCostModifier = pClassInfo->getHurryCostModifier();
	if ( m_iAdvancedStartCost == 100 ) m_iAdvancedStartCost = pClassInfo->getAdvancedStartCost();
	if ( m_iAdvancedStartCostIncrease == iDefault ) m_iAdvancedStartCostIncrease = pClassInfo->getAdvancedStartCostIncrease();
	if ( m_iMinAreaSize == iDefault ) m_iMinAreaSize = pClassInfo->getMinAreaSize();
	if ( m_iMoves == iDefault ) m_iMoves = pClassInfo->getMoves();
	if ( m_iAirRange == iDefault ) m_iAirRange = pClassInfo->getAirRange();
	if ( m_iAirUnitCap == iDefault ) m_iAirUnitCap = pClassInfo->getAirUnitCap();
	if ( m_iDropRange == iDefault ) m_iDropRange = pClassInfo->getDropRange();
	if ( m_iNukeRange == -1 ) m_iNukeRange = pClassInfo->getNukeRange();
	if ( m_iWorkRate == iDefault ) m_iWorkRate = pClassInfo->getWorkRate();
	if ( m_iBaseDiscover == iDefault ) m_iBaseDiscover = pClassInfo->getBaseDiscover();
	if ( m_iDiscoverMultiplier == iDefault ) m_iDiscoverMultiplier = pClassInfo->getDiscoverMultiplier();
	if ( m_iBaseHurry == iDefault ) m_iBaseHurry = pClassInfo->getBaseHurry();
	if ( m_iHurryMultiplier == iDefault ) m_iHurryMultiplier = pClassInfo->getHurryMultiplier();
	if ( m_iBaseTrade == iDefault ) m_iBaseTrade = pClassInfo->getBaseTrade();
	if ( m_iTradeMultiplier == iDefault ) m_iTradeMultiplier = pClassInfo->getTradeMultiplier();
	if ( m_iGreatWorkCulture == iDefault ) m_iGreatWorkCulture = pClassInfo->getGreatWorkCulture();
	if ( m_iEspionagePoints == iDefault ) m_iEspionagePoints = pClassInfo->getEspionagePoints();
	if ( m_iCombat == iDefault ) m_iCombat = pClassInfo->getCombat();
	if ( m_iCombatLimit == 100 ) m_iCombatLimit = pClassInfo->getCombatLimit();
	if ( m_iAirCombat == iDefault ) m_iAirCombat = pClassInfo->getAirCombat();
	if ( m_iAirCombatLimit == iDefault ) m_iAirCombatLimit = pClassInfo->getAirCombatLimit();
	if ( m_iXPValueAttack == iDefault ) m_iXPValueAttack = pClassInfo->getXPValueAttack();
	if ( m_iXPValueDefense == iDefault ) m_iXPValueDefense = pClassInfo->getXPValueDefense();
	if ( m_iFirstStrikes == iDefault ) m_iFirstStrikes = pClassInfo->getFirstStrikes();
	if ( m_iChanceFirstStrikes == iDefault ) m_iChanceFirstStrikes = pClassInfo->getChanceFirstStrikes();
	if ( m_iInterceptionProbability == iDefault ) m_iInterceptionProbability = pClassInfo->getInterceptionProbability();
	//ls612: Advanced Nuke Interception
	//if ( m_iNukeInterceptionProbability == iDefault ) m_iNukeInterceptionProbability = pClassInfo->getNukeInterceptionProbability();
	//if ( m_iNukeInterceptionRange == iDefault ) m_iNukeInterceptionRange = pClassInfo->getNukeInterceptionRange();
	if ( m_iEvasionProbability == iDefault ) m_iEvasionProbability = pClassInfo->getEvasionProbability();
	if ( m_iWithdrawalProbability == iDefault ) m_iWithdrawalProbability = pClassInfo->getWithdrawalProbability(true);
	if ( m_iCollateralDamage == iDefault ) m_iCollateralDamage = pClassInfo->getCollateralDamage();
	if ( m_iCollateralDamageLimit == iDefault ) m_iCollateralDamageLimit = pClassInfo->getCollateralDamageLimit();
	if ( m_iCollateralDamageMaxUnits == iDefault ) m_iCollateralDamageMaxUnits = pClassInfo->getCollateralDamageMaxUnits();
	if ( m_iCityAttackModifier == iDefault ) m_iCityAttackModifier = pClassInfo->getCityAttackModifier();
	if ( m_iCityDefenseModifier == iDefault ) m_iCityDefenseModifier = pClassInfo->getCityDefenseModifier();
	if ( m_iAnimalCombatModifier == iDefault ) m_iAnimalCombatModifier = pClassInfo->getAnimalCombatModifier();
	if ( m_iHillsAttackModifier == iDefault ) m_iHillsAttackModifier = pClassInfo->getHillsAttackModifier();
	if ( m_iHillsDefenseModifier == iDefault ) m_iHillsDefenseModifier = pClassInfo->getHillsDefenseModifier();

	if ( m_iBombRate == iDefault ) m_iBombRate = pClassInfo->getBombRate();
	if ( m_iBombardRate == iDefault ) m_iBombardRate = pClassInfo->getBombardRate();

	if ( m_iSpecialCargo == iTextDefault ) m_iSpecialCargo = pClassInfo->getSpecialCargo(true);
	if ( m_iSMSpecialCargo == iTextDefault ) m_iSMSpecialCargo = pClassInfo->getSMSpecialCargo();
	if ( m_iSMNotSpecialCargo == iTextDefault ) m_iSMNotSpecialCargo = pClassInfo->getSMNotSpecialCargo();
	if ( m_iDomainCargo == iTextDefault ) m_iDomainCargo = pClassInfo->getDomainCargo();

	if ( m_iCargoSpace == iDefault ) m_iCargoSpace = pClassInfo->getCargoSpace();
	if ( m_iSMCargoSpace == iDefault ) m_iSMCargoSpace = pClassInfo->getSMCargoSpace();
	if ( m_iSMCargoVolume == iDefault ) m_iSMCargoVolume = pClassInfo->getSMCargoVolume();
	if ( m_iConscriptionValue == iDefault ) m_iConscriptionValue = pClassInfo->getConscriptionValue();
	if ( m_iCultureGarrisonValue == iDefault ) m_iCultureGarrisonValue = pClassInfo->getCultureGarrisonValue();
	if ( m_iExtraCost == iDefault ) m_iExtraCost = pClassInfo->getExtraCost();
	if ( m_iAssetValue == iDefault ) m_iAssetValue = pClassInfo->getAssetValue(true);
	if ( m_iPowerValue == iDefault ) m_iPowerValue = pClassInfo->getPowerValue(true);

	if ( m_iLeaderPromotion == iTextDefault ) m_iLeaderPromotion = pClassInfo->getLeaderPromotion();
	if ( m_iLeaderExperience == iDefault ) m_iLeaderExperience = pClassInfo->getLeaderExperience();

/************************************************************************************************/
/* Afforess					  Start		 02/03/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	if (getPrereqVicinityBonus() == iTextDefault) m_iPrereqVicinityBonus = pClassInfo->getPrereqVicinityBonus();
	if ( m_bRequiresStateReligionInCity == bDefault )	m_bRequiresStateReligionInCity = pClassInfo->isRequiresStateReligionInCity();
	if ( m_bWorkerTrade == bDefault )	m_bWorkerTrade = pClassInfo->isWorkerTrade();
	if ( m_bMilitaryTrade == bDefault )	m_bMilitaryTrade = pClassInfo->isMilitaryTrade();
	if ( m_bForceUpgrade == bDefault )	m_bForceUpgrade = pClassInfo->isForceUpgrade();
	if ( m_bGreatGeneral == bDefault )	m_bGreatGeneral = pClassInfo->isGreatGeneral();
	if ( m_bSlave == bDefault )	m_bSlave = pClassInfo->isSlave();
	if ( m_iBaseFoodChange == iDefault )	m_iBaseFoodChange = pClassInfo->getBaseFoodChange();
	if ( m_iControlPoints == iDefault )	m_iControlPoints = pClassInfo->getControlPoints();
	if ( m_iCommandRange == iDefault )	m_iCommandRange = pClassInfo->getCommandRange();

	for ( int i = 0; i < GC.getNUM_UNIT_PREREQ_OR_BONUSES(); i++)
	{
		if ( getPrereqOrVicinityBonuses(i) == NO_BONUS && pClassInfo->getPrereqOrVicinityBonuses(i) != NO_BONUS)
		{
			if ( NULL == m_piPrereqOrVicinityBonuses )
			{
				CvXMLLoadUtility::InitList(&m_piPrereqOrVicinityBonuses,GC.getNUM_UNIT_PREREQ_OR_BONUSES(),(int)NO_BONUS);
			}
			m_piPrereqOrVicinityBonuses[i] = pClassInfo->getPrereqOrVicinityBonuses(i);
		}
	}

	for (int i = 0; i < GC.getNumRouteInfos(); i++)
	{
		if (!getPassableRouteNeeded(i) && pClassInfo->getPassableRouteNeeded(i))
		{
			if ( NULL == m_pbPassableRouteNeeded )
			{
				CvXMLLoadUtility::InitList(&m_pbPassableRouteNeeded,GC.getNumRouteInfos(),false);
			}
			m_pbPassableRouteNeeded[i] = pClassInfo->getPassableRouteNeeded(i);
		}
	}

	m_PropertyManipulators.copyNonDefaults(pClassInfo->getPropertyManipulators(),pXML);

	if (!m_pExprTrainCondition)
	{
		m_pExprTrainCondition = pClassInfo->m_pExprTrainCondition;
		pClassInfo->m_pExprTrainCondition = NULL;
	}
/*****************************************************************************************************/
/**  Author: TheLadiesOgre																		  **/
/**  Date: 21.09.2009																			   **/
/**  ModComp: TLOTags																			   **/
/**  Reason Added: New Tag Definition															   **/
/**  Notes: Adapted to C2C by Thunderbrd															**/
/*****************************************************************************************************/
	if ( m_bFreeDrop == bDefault ) m_bFreeDrop = pClassInfo->isFreeDrop();
/*****************************************************************************************************/
/**  TheLadiesOgre; 21.09.2009; TLOTags															 **/
/*****************************************************************************************************/

	//TB Combat Mods Begin  TB SubCombat Mod begin
	if ( m_iPursuit == iDefault ) m_iPursuit = pClassInfo->getPursuit(true);
	if ( m_iEarlyWithdraw == iDefault ) m_iEarlyWithdraw = pClassInfo->getEarlyWithdraw(true);
	if ( m_iVSBarbs == iDefault ) m_iVSBarbs = pClassInfo->getVSBarbs();
	if ( m_iArmor == iDefault ) m_iArmor = pClassInfo->getArmor();
	if ( m_iPuncture == iDefault ) m_iPuncture = pClassInfo->getPuncture();
	if ( m_iOverrun == iDefault ) m_iOverrun = pClassInfo->getOverrun(true);
	if ( m_iRepel == iDefault ) m_iRepel = pClassInfo->getRepel(true);
	if ( m_iFortRepel == iDefault ) m_iFortRepel = pClassInfo->getFortRepel(true);
	if ( m_iRepelRetries == iDefault ) m_iRepelRetries = pClassInfo->getRepelRetries(true);
	if ( m_iUnyielding == iDefault ) m_iUnyielding = pClassInfo->getUnyielding(true);
	if ( m_iKnockback == iDefault ) m_iKnockback = pClassInfo->getKnockback(true);
	if ( m_iKnockbackRetries == iDefault ) m_iKnockbackRetries = pClassInfo->getKnockbackRetries(true);
	if ( m_iStrAdjperRnd == iDefault ) m_iStrAdjperRnd = pClassInfo->getStrAdjperRnd(true);
	if ( m_iStrAdjperAtt == iDefault ) m_iStrAdjperAtt = pClassInfo->getStrAdjperAtt(true);
	if ( m_iStrAdjperDef == iDefault ) m_iStrAdjperDef = pClassInfo->getStrAdjperDef(true);
	if ( m_iWithdrawAdjperAtt == iDefault ) m_iWithdrawAdjperAtt = pClassInfo->getWithdrawAdjperAtt(true);
	if ( m_iUnnerve == iDefault ) m_iUnnerve = pClassInfo->getUnnerve(true);
	if ( m_iEnclose == iDefault ) m_iEnclose = pClassInfo->getEnclose(true);
	if ( m_iLunge == iDefault ) m_iLunge = pClassInfo->getLunge(true);
	if ( m_iDynamicDefense == iDefault ) m_iDynamicDefense = pClassInfo->getDynamicDefense(true);
	if ( m_iFortitude == iDefault ) m_iFortitude = pClassInfo->getFortitude(true);

	for (int iI = 0; iI < NUM_SUPPORT_POSITION_TYPES; iI++)
	{
		if ( m_aiSupportPercent[iI] == iDefault ) m_aiSupportPercent[iI] = pClassInfo->getSupportPercent((SupportPositionTypes)iI, true);
	}
	if ( m_iFrontSupportPercent == iDefault ) m_iFrontSupportPercent = pClassInfo->getFrontSupportPercent(true);
	if ( m_iShortRangeSupportPercent == iDefault ) m_iShortRangeSupportPercent = pClassInfo->getShortRangeSupportPercent(true);
	if ( m_iMediumRangeSupportPercent == iDefault ) m_iMediumRangeSupportPercent = pClassInfo->getMediumRangeSupportPercent(true);
	if ( m_iLongRangeSupportPercent == iDefault ) m_iLongRangeSupportPercent = pClassInfo->getLongRangeSupportPercent(true);
	if ( m_iFlankSupportPercent == iDefault ) m_iFlankSupportPercent = pClassInfo->getFlankSupportPercent(true);

	if ( m_iDodgeModifier == iDefault ) m_iDodgeModifier = pClassInfo->getDodgeModifier();
	if ( m_iPrecisionModifier == iDefault ) m_iPrecisionModifier = pClassInfo->getPrecisionModifier();
	if ( m_iPowerShots == iDefault ) m_iPowerShots = pClassInfo->getPowerShots();
	if ( m_iPowerShotCombatModifier == iDefault ) m_iPowerShotCombatModifier = pClassInfo->getPowerShotCombatModifier();
	if ( m_iPowerShotPunctureModifier == iDefault ) m_iPowerShotPunctureModifier = pClassInfo->getPowerShotPunctureModifier();
	if ( m_iPowerShotPrecisionModifier == iDefault ) m_iPowerShotPrecisionModifier = pClassInfo->getPowerShotPrecisionModifier();
	if ( m_iPowerShotCriticalModifier == iDefault ) m_iPowerShotCriticalModifier = pClassInfo->getPowerShotCriticalModifier();
	if ( m_iCriticalModifier == iDefault ) m_iCriticalModifier = pClassInfo->getCriticalModifier();
	if ( m_iEndurance == iDefault ) m_iEndurance = pClassInfo->getEndurance();
	if ( m_iRoundStunProb == iDefault ) m_iRoundStunProb = pClassInfo->getRoundStunProb();
	if ( m_iPoisonProbabilityModifier == iDefault ) m_iPoisonProbabilityModifier = pClassInfo->getPoisonProbabilityModifier();
	//Team Project (3)
	if ( m_iCaptureProbabilityModifier == iDefault ) m_iCaptureProbabilityModifier = pClassInfo->getCaptureProbabilityModifier();
	if ( m_iCaptureResistanceModifier == iDefault ) m_iCaptureResistanceModifier = pClassInfo->getCaptureResistanceModifier();
//Team Project (4)
//WorkRateMod
	if ( m_iHillsWorkModifier == iDefault ) m_iHillsWorkModifier = pClassInfo->getHillsWorkModifier();
	if ( m_iPeaksWorkModifier == iDefault ) m_iPeaksWorkModifier = pClassInfo->getPeaksWorkModifier();

	if ( m_iBreakdownChance == iDefault ) m_iBreakdownChance = pClassInfo->getBreakdownChance();
	if ( m_iBreakdownDamage == iDefault ) m_iBreakdownDamage = pClassInfo->getBreakdownDamage();
	if ( m_iTaunt == iDefault ) m_iTaunt = pClassInfo->getTaunt(true);
	if ( m_iMaxHP == 100 ) m_iMaxHP = pClassInfo->getMaxHP(true);
	if ( m_iDamageModifier == iDefault ) m_iDamageModifier = pClassInfo->getDamageModifier();
	if ( m_iCostModifier == iDefault ) m_iCostModifier = pClassInfo->getCostModifier();
	if ( m_iRBombardDamage == iDefault ) m_iRBombardDamage = pClassInfo->getRBombardDamage();
	if ( m_iRBombardDamageLimit == iDefault ) m_iRBombardDamageLimit = pClassInfo->getRBombardDamageLimit();
	if ( m_iRBombardDamageMaxUnits == iDefault ) m_iRBombardDamageMaxUnits = pClassInfo->getRBombardDamageMaxUnits();
	if ( m_iCombatModifierPerSizeMore == iDefault ) m_iCombatModifierPerSizeMore = pClassInfo->getCombatModifierPerSizeMore(true);
	if ( m_iCombatModifierPerSizeLess == iDefault ) m_iCombatModifierPerSizeLess = pClassInfo->getCombatModifierPerSizeLess(true);
	if ( m_iCombatModifierPerVolumeMore == iDefault ) m_iCombatModifierPerVolumeMore = pClassInfo->getCombatModifierPerVolumeMore(true);
	if ( m_iCombatModifierPerVolumeLess == iDefault ) m_iCombatModifierPerVolumeLess = pClassInfo->getCombatModifierPerVolumeLess(true);
	if ( m_iSelfHealModifier == iDefault ) m_iSelfHealModifier = pClassInfo->getSelfHealModifier();
	if ( m_iNumHealSupport == iDefault ) m_iNumHealSupport = pClassInfo->getNumHealSupport();
	if ( m_iInsidiousness == iDefault ) m_iInsidiousness = pClassInfo->getInsidiousness();
	if ( m_iInvestigation == iDefault ) m_iInvestigation = pClassInfo->getInvestigation();
	if ( getStealthStrikes(true) == iDefault ) m_iStealthStrikes = pClassInfo->getStealthStrikes(true);
	if ( getStealthCombatModifier(true) == iDefault ) m_iStealthCombatModifier = pClassInfo->getStealthCombatModifier(true);
	if ( m_iTrapDamageMax == iDefault ) m_iTrapDamageMax = pClassInfo->getTrapDamageMax();
	if ( m_iTrapDamageMin == iDefault ) m_iTrapDamageMin = pClassInfo->getTrapDamageMin();
	if ( m_iTrapComplexity == iDefault ) m_iTrapComplexity = pClassInfo->getTrapComplexity();
	if ( m_iNumTriggers == iDefault ) m_iNumTriggers = pClassInfo->getNumTriggers();
	if ( m_iAggression == iDefault ) m_iAggression = pClassInfo->getAggression();
	if ( m_iAnimalIgnoresBorders == iDefault ) m_iAnimalIgnoresBorders = pClassInfo->getAnimalIgnoresBorders();
	if ( m_iReligiousCombatModifier == iDefault ) m_iReligiousCombatModifier = pClassInfo->getReligiousCombatModifier();
	//booleans
	if ( m_bStampede == bDefault ) m_bStampede = pClassInfo->isStampede();
	if ( m_bOnslaught == bDefault ) m_bOnslaught = pClassInfo->isOnslaught();
	if ( m_bDealsColdDamage == bDefault ) m_bDealsColdDamage = pClassInfo->isDealsColdDamage();
	if ( m_bColdImmune == bDefault ) m_bColdImmune = pClassInfo->isColdImmune();
	if ( m_bAttackOnlyCities == bDefault ) m_bAttackOnlyCities = pClassInfo->isAttackOnlyCities();
	if ( m_bIgnoreNoEntryLevel == bDefault ) m_bIgnoreNoEntryLevel = pClassInfo->isIgnoreNoEntryLevel();
	if ( m_bIgnoreZoneofControl == bDefault ) m_bIgnoreZoneofControl = pClassInfo->isIgnoreZoneofControl();
	if ( m_bFliesToMove == bDefault ) m_bFliesToMove = pClassInfo->isFliesToMove();
	if ( m_bRBombardForceAbility == bDefault ) m_bRBombardForceAbility = pClassInfo->isRBombardForceAbility();
	if ( m_bNoSelfHeal == bDefault ) m_bNoSelfHeal = pClassInfo->isNoSelfHeal();
	if ( m_bExcile == bDefault ) m_bExcile = pClassInfo->isExcile();
	if ( m_bPassage == bDefault ) m_bPassage = pClassInfo->isPassage();
	if ( m_bNoNonOwnedCityEntry == bDefault ) m_bNoNonOwnedCityEntry = pClassInfo->isNoNonOwnedCityEntry();
	if ( m_bBarbCoExist == bDefault ) m_bBarbCoExist = pClassInfo->isBarbCoExist();
	if ( m_bBlendIntoCity == bDefault ) m_bBlendIntoCity = pClassInfo->isBlendIntoCity();
	if ( m_bUpgradeAnywhere == bDefault ) m_bUpgradeAnywhere = pClassInfo->isUpgradeAnywhere();
	if ( m_bAssassin == bDefault ) m_bAssassin = pClassInfo->isAssassin();
	if ( isStealthDefense(true) == bDefault ) m_bStealthDefense = pClassInfo->isStealthDefense(true);
	if ( m_bNoInvisibility == bDefault ) m_bNoInvisibility = pClassInfo->isNoInvisibility();
	if ( m_bTriggerBeforeAttack == bDefault ) m_bTriggerBeforeAttack = pClassInfo->isTriggerBeforeAttack();
	if ( m_bNoNonTypeProdMods == bDefault ) m_bNoNonTypeProdMods = pClassInfo->isNoNonTypeProdMods();
	if ( m_bGatherHerd == bDefault ) m_bGatherHerd = pClassInfo->isGatherHerd();
	//boolean vectors without delayed resolution
	if (getNumSubCombatTypes() == 0)
	{
		m_aiSubCombatTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumSubCombatTypes(); i++)
		{
			m_aiSubCombatTypes.push_back(pClassInfo->getSubCombatType(i));
		}
	}

	if (getNumCureAfflictionTypes() == 0)
	{
		m_aiCureAfflictionTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumCureAfflictionTypes(); i++)
		{
			m_aiCureAfflictionTypes.push_back(pClassInfo->getCureAfflictionType(i));
		}
	}

	if (getNumTerrainImpassableTypes() == 0)
	{
		m_aiTerrainImpassableTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumTerrainImpassableTypes(); i++)
		{
			m_aiTerrainImpassableTypes.push_back(pClassInfo->getTerrainImpassableType(i));
		}
	}

	if (getNumFeatureImpassableTypes() == 0)
	{
		m_aiFeatureImpassableTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumFeatureImpassableTypes(); i++)
		{
			m_aiFeatureImpassableTypes.push_back(pClassInfo->getFeatureImpassableType(i));
		}
	}

	if (getNumMapCategoryTypes() == 0)
	{
		m_aiMapCategoryTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumMapCategoryTypes(); i++)
		{
			m_aiMapCategoryTypes.push_back(pClassInfo->getMapCategoryType(i));
		}
	}

	if (getNumTrapSetWithPromotionTypes() == 0)
	{
		m_aiTrapSetWithPromotionTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumTrapSetWithPromotionTypes(); i++)
		{
			m_aiTrapSetWithPromotionTypes.push_back(pClassInfo->getTrapSetWithPromotionType(i));
		}
	}

	if (getNumTrapImmunityUnitCombatTypes() == 0)
	{
		m_aiTrapImmunityUnitCombatTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumTrapImmunityUnitCombatTypes(); i++)
		{
			m_aiTrapImmunityUnitCombatTypes.push_back(pClassInfo->getTrapImmunityUnitCombatType(i));
		}
	}
	// int vectors utilizing struct with delayed resolution
	if (getNumAfflictionFortitudeModifiers() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aAfflictionFortitudeModifiers, pClassInfo->m_aAfflictionFortitudeModifiers);
	}

	if (getNumAfflictOnAttackTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aAfflictOnAttackTypes, pClassInfo->m_aAfflictOnAttackTypes);
	}

	if (getNumHealUnitCombatTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aHealUnitCombatTypes, pClassInfo->m_aHealUnitCombatTypes);
	}

	if (getNumGroupSpawnUnitCombatTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aGroupSpawnUnitCombatTypes, pClassInfo->m_aGroupSpawnUnitCombatTypes);
	}

	if (getNumInvisibleTerrainChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aInvisibleTerrainChanges, pClassInfo->m_aInvisibleTerrainChanges);
	}

	if (getNumInvisibleFeatureChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aInvisibleFeatureChanges, pClassInfo->m_aInvisibleFeatureChanges);
	}

	if (getNumInvisibleImprovementChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aInvisibleImprovementChanges, pClassInfo->m_aInvisibleImprovementChanges);
	}

	if (getNumVisibleTerrainChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aVisibleTerrainChanges, pClassInfo->m_aVisibleTerrainChanges);
	}

	if (getNumVisibleFeatureChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aVisibleFeatureChanges, pClassInfo->m_aVisibleFeatureChanges);
	}

	if (getNumVisibleImprovementChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aVisibleImprovementChanges, pClassInfo->m_aVisibleImprovementChanges);
	}

	if (getNumVisibleTerrainRangeChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aVisibleTerrainRangeChanges, pClassInfo->m_aVisibleTerrainRangeChanges);
	}

	if (getNumVisibleFeatureRangeChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aVisibleFeatureRangeChanges, pClassInfo->m_aVisibleFeatureRangeChanges);
	}

	if (getNumVisibleImprovementRangeChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aVisibleImprovementRangeChanges, pClassInfo->m_aVisibleImprovementRangeChanges);
	}

	if (getNumDistanceAttackCommunicabilityTypeChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aDistanceAttackCommunicabilityTypeChanges, pClassInfo->m_aDistanceAttackCommunicabilityTypeChanges);
	}

	if (getNumEnabledCivilizationTypes() == 0)
	{
		int iNum = pClassInfo->getNumEnabledCivilizationTypes();
		m_aEnabledCivilizationTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			GC.copyNonDefaultDelayedResolution((int*)&(m_aEnabledCivilizationTypes[i].eCivilization), (int*)&(pClassInfo->m_aEnabledCivilizationTypes[i].eCivilization));
		}
	}
	// int vectors utilizing pairing without delayed resolution
	if (getNumWithdrawOnTerrainTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumWithdrawOnTerrainTypes(); i++)
		{
			TerrainTypes eTerrain = ((TerrainTypes)i);
			int iChange = pClassInfo->getWithdrawOnTerrainType(i);
			m_aWithdrawOnTerrainTypes.push_back(std::make_pair(eTerrain, iChange));
		}
	}

	if (getNumWithdrawOnFeatureTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumWithdrawOnFeatureTypes(); i++)
		{
			FeatureTypes eFeature = ((FeatureTypes)i);
			int iChange = pClassInfo->getWithdrawOnFeatureType(i);
			m_aWithdrawOnFeatureTypes.push_back(std::make_pair(eFeature, iChange));
		}
	}

	if (getNumFlankingStrikesbyUnitCombatTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumFlankingStrikesbyUnitCombatTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getFlankingStrengthbyUnitCombatType(i);
			m_aFlankingStrengthbyUnitCombatType.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumWithdrawVSUnitCombatTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumWithdrawVSUnitCombatTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getWithdrawVSUnitCombatType(i);
			m_aWithdrawVSUnitCombatTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumPursuitVSUnitCombatTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumPursuitVSUnitCombatTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getPursuitVSUnitCombatType(i, true);
			m_aPursuitVSUnitCombatTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumRepelVSUnitCombatTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumRepelVSUnitCombatTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getRepelVSUnitCombatType(i, true);
			m_aRepelVSUnitCombatTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumKnockbackVSUnitCombatTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumKnockbackVSUnitCombatTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getKnockbackVSUnitCombatType(i, true);
			m_aKnockbackVSUnitCombatTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumPunctureVSUnitCombatTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumPunctureVSUnitCombatTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getPunctureVSUnitCombatType(i);
			m_aPunctureVSUnitCombatTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumArmorVSUnitCombatTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumArmorVSUnitCombatTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getArmorVSUnitCombatType(i);
			m_aArmorVSUnitCombatTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumDodgeVSUnitCombatTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumDodgeVSUnitCombatTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getDodgeVSUnitCombatType(i);
			m_aDodgeVSUnitCombatTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumPrecisionVSUnitCombatTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumPrecisionVSUnitCombatTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getPrecisionVSUnitCombatType(i);
			m_aPrecisionVSUnitCombatTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumCriticalVSUnitCombatTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumCriticalVSUnitCombatTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getCriticalVSUnitCombatType(i);
			m_aCriticalVSUnitCombatTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumRoundStunVSUnitCombatTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumRoundStunVSUnitCombatTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getRoundStunVSUnitCombatType(i);
			m_aRoundStunVSUnitCombatTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumTrapDisableUnitCombatTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumTrapDisableUnitCombatTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getTrapDisableUnitCombatType(i);
			m_aTrapDisableUnitCombatTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumTrapAvoidanceUnitCombatTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumTrapAvoidanceUnitCombatTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getTrapAvoidanceUnitCombatType(i);
			m_aTrapAvoidanceUnitCombatTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumTrapTriggerUnitCombatTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumTrapTriggerUnitCombatTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getTrapTriggerUnitCombatType(i);
			m_aTrapTriggerUnitCombatTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumVisibilityIntensityTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumVisibilityIntensityTypes(); i++)
		{
			InvisibleTypes eInvisible = ((InvisibleTypes)i);
			int iChange = pClassInfo->getVisibilityIntensityType(i);
			m_aVisibilityIntensityTypes.push_back(std::make_pair(eInvisible, iChange));
		}
	}

	if (getNumInvisibilityIntensityTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumInvisibilityIntensityTypes(); i++)
		{
			InvisibleTypes eInvisible = ((InvisibleTypes)i);
			int iChange = pClassInfo->getInvisibilityIntensityType(i);
			m_aInvisibilityIntensityTypes.push_back(std::make_pair(eInvisible, iChange));
		}
	}

	//Team Project (4)
		//WorkRateMod
	if (getNumTerrainWorkRateModifierTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumTerrainWorkRateModifierTypes(); i++)
		{
			TerrainTypes eTerrain = ((TerrainTypes)i);
			int iChange = pClassInfo->getTerrainWorkRateModifierType(i);
			m_aTerrainWorkRateModifierTypes.push_back(std::make_pair(eTerrain, iChange));
		}
	}

	if (getNumFeatureWorkRateModifierTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumFeatureWorkRateModifierTypes(); i++)
		{
			FeatureTypes eFeature = ((FeatureTypes)i);
			int iChange = pClassInfo->getFeatureWorkRateModifierType(i);
			m_aFeatureWorkRateModifierTypes.push_back(std::make_pair(eFeature, iChange));
		}
	}

	if (getNumBuildWorkRateModifierTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumBuildWorkRateModifierTypes(); i++)
		{
			BuildTypes eBuild = ((BuildTypes)i);
			int iChange = pClassInfo->getBuildWorkRateModifierType(i);
			m_aBuildWorkRateModifierTypes.push_back(std::make_pair(eBuild, iChange));
		}
	}

	if (getNumAidChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumAidChanges(); i++)
		{
			PropertyTypes eProperty = ((PropertyTypes)i);
			int iChange = pClassInfo->getAidChange(i);
			m_aAidChanges.push_back(std::make_pair(eProperty, iChange));
		}
	}
	//TB Combat Mods End  TB SubCombat Mod end
	//setTotalModifiedCombatStrengthDetails();

	getKillOutcomeList()->copyNonDefaults(pClassInfo->getKillOutcomeList(), pXML);

	if (m_aOutcomeMissions.empty())
	{
		int num = (int) pClassInfo->getNumActionOutcomes();
		for (int index = 0; index < num; index++)
		{
			m_aOutcomeMissions.push_back(pClassInfo->m_aOutcomeMissions[index]);
			pClassInfo->m_aOutcomeMissions[index] = NULL;
		}
	}

	// For the Meshgroups I assume the XML holding the largest GroupSize is the most completed(most fancy)
	// and we want to keep that one
	if ( m_iGroupSize < pClassInfo->getGroupSize() )
	{
		m_iGroupSize = pClassInfo->getGroupSize();
		m_iGroupDefinitions = pClassInfo->getGroupDefinitions();

		m_iUnitMeleeWaveSize = pClassInfo->getMeleeWaveSize();
		m_iUnitRangedWaveSize = pClassInfo->getRangedWaveSize();
		m_fUnitMaxSpeed = pClassInfo->getUnitMaxSpeed();
		m_fUnitPadTime = pClassInfo->getUnitPadTime();

		//Delete old Arrays for initializing a new one
		SAFE_DELETE_ARRAY(m_piUnitGroupRequired)
		SAFE_DELETE_ARRAY(m_paszEarlyArtDefineTags)
		SAFE_DELETE_ARRAY(m_paszLateArtDefineTags)
		SAFE_DELETE_ARRAY(m_paszMiddleArtDefineTags)
		SAFE_DELETE_ARRAY(m_paszClassicalArtDefineTags)
		SAFE_DELETE_ARRAY(m_paszRennArtDefineTags)
		SAFE_DELETE_ARRAY(m_paszIndustrialArtDefineTags)
		SAFE_DELETE_ARRAY(m_paszFutureArtDefineTags)

		m_piUnitGroupRequired = new int[ m_iGroupDefinitions ];
		m_paszEarlyArtDefineTags = new CvString[ m_iGroupDefinitions ];
		m_paszLateArtDefineTags = new CvString[ m_iGroupDefinitions ];
		m_paszMiddleArtDefineTags = new CvString[ m_iGroupDefinitions ];
		m_paszClassicalArtDefineTags = new CvString[ m_iGroupDefinitions ];
		m_paszRennArtDefineTags = new CvString[ m_iGroupDefinitions ];
		m_paszIndustrialArtDefineTags = new CvString[ m_iGroupDefinitions ];
		m_paszFutureArtDefineTags = new CvString[ m_iGroupDefinitions ];

		for ( int i = 0; i < m_iGroupDefinitions; i++ )
		{
			m_piUnitGroupRequired[i] = pClassInfo->getUnitGroupRequired(i);
			setEarlyArtDefineTag(i, pClassInfo->getEarlyArtDefineTag(i, NO_UNIT_ARTSTYLE));
			setLateArtDefineTag(i, pClassInfo->getLateArtDefineTag(i, NO_UNIT_ARTSTYLE));
			setClassicalArtDefineTag(i, pClassInfo->getClassicalArtDefineTag(i, NO_UNIT_ARTSTYLE));
			setMiddleArtDefineTag(i, pClassInfo->getMiddleArtDefineTag(i, NO_UNIT_ARTSTYLE));
			setRennArtDefineTag(i, pClassInfo->getRennArtDefineTag(i, NO_UNIT_ARTSTYLE));
			setIndustrialArtDefineTag(i, pClassInfo->getIndustrialArtDefineTag(i, NO_UNIT_ARTSTYLE));
			setFutureArtDefineTag(i, pClassInfo->getFutureArtDefineTag(i, NO_UNIT_ARTSTYLE));
		}
	}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	if ( m_szFormationType == "FORMATION_TYPE_DEFAULT" ) m_szFormationType = pClassInfo->getFormationType();

	// First we check if there are different Unique Unitnames in the Modules(we want to keep all of them)
	// So we have to set the Arraysize properly, knowing the amount of Unique Unitnames
	if ( pClassInfo->getNumUnitNames() != 0 )
	{
		CvString* m_paszNewNames = new CvString[pClassInfo->getNumUnitNames()];
		for ( int i = 0; i < pClassInfo->getNumUnitNames(); i++)
		{
			m_paszNewNames[i] = pClassInfo->getUnitNames(i);
		}

		CvXMLLoadUtilityModTools* pCurrentUnitClass = new CvXMLLoadUtilityModTools;
		pCurrentUnitClass->StringArrayExtend(&m_paszUnitNames, &m_iNumUnitNames,
										 &m_paszNewNames, pClassInfo->getNumUnitNames());
		delete pCurrentUnitClass;
		SAFE_DELETE_ARRAY(m_paszNewNames)
	}

	if (m_iGroupSize != 0)
	{
		updateArtDefineButton();
	}
}

bool CvUnitInfo::readPass3()
{
	m_paszCivilizationNames = new CvWString[GC.getNumCivilizationInfos()];
	for (int iI = 0; iI < GC.getNumCivilizationInfos(); iI++)
	{
		m_paszCivilizationNames[iI] = "";
	}

	if (!m_aszCivilizationNamesValueforPass3.empty() && !m_aszCivilizationNamesforPass3.empty())
	{
		int iNumLoad = m_aszCivilizationNamesValueforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			int iTempIndex = GC.getInfoTypeForString(m_aszCivilizationNamesforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumCivilizationInfos())
				m_paszCivilizationNames[iTempIndex] = m_aszCivilizationNamesValueforPass3[iI];
		}
		m_aszCivilizationNamesforPass3.clear();
		m_aszCivilizationNamesValueforPass3.clear();
	}

	return true;
}

bool CvUnitInfo::hasUnitCombat(UnitCombatTypes eUnitCombat) const
{
	FAssert(0 <= eUnitCombat && eUnitCombat < GC.getNumUnitCombatInfos());

	if ( m_abHasCombatType == NULL )
	{
		m_abHasCombatType = new bool[GC.getNumUnitCombatInfos()];
		memset(m_abHasCombatType, 0, GC.getNumUnitCombatInfos());

		for(int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
		{
			if (getUnitCombatType() == iI)
			{
				m_abHasCombatType[iI] = true;
				continue;
			}
			//TB SubCombat Mod Begin
			for (int iJ = 0; iJ < getNumSubCombatTypes(); iJ++)
			{
				if (getSubCombatType(iJ) == iI)
				{
					m_abHasCombatType[iI] = true;
					break;
				}
			}
		}
	}

	return m_abHasCombatType[eUnitCombat];
}

void CvUnitInfo::setTotalModifiedCombatStrengthDetails()
{
	m_iTotalCombatStrengthChangeBase = 0;
	m_iTotalCombatStrengthModifierBase = 0;
	for(int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		if (hasUnitCombat((UnitCombatTypes)iI))
		{
			const UnitCombatTypes eUnitCombat = ((UnitCombatTypes)iI);
			m_iTotalCombatStrengthChangeBase += GC.getUnitCombatInfo(eUnitCombat).getStrengthChange();
			if (!GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS_UNCUT))
			{
				if (GC.getUnitCombatInfo(eUnitCombat).getQualityBase() > -10)
				{
						m_iTotalCombatStrengthModifierBase += (GC.getUnitCombatInfo(eUnitCombat).getQualityBase() - 5);
				}
				if (GC.getUnitCombatInfo(eUnitCombat).getSizeBase() > -10)
				{
						m_iTotalCombatStrengthModifierBase += (GC.getUnitCombatInfo(eUnitCombat).getSizeBase() - 5);
				}
				if (GC.getUnitCombatInfo(eUnitCombat).getGroupBase() > -10)
				{
						m_iTotalCombatStrengthModifierBase += (GC.getUnitCombatInfo(eUnitCombat).getGroupBase() - 5);
				}
			}
		}
	}
}

int CvUnitInfo::getCombatStrengthModifier() const
{
	return m_iTotalCombatStrengthModifierBase;
}
int CvUnitInfo::getTotalModifiedCombatStrength100() const
{
	int iStr = m_iCombat + m_iTotalCombatStrengthChangeBase;

	if (iStr < 0)
	{
		iStr = 0;
	}
	if (iStr !=0)
	{
		iStr *= 100;

		const int iSMMultiplier = GC.getDefineINT("SIZE_MATTERS_MOST_MULTIPLIER");
		const bool bPositive = (m_iTotalCombatStrengthModifierBase > 0);
		const int iIterator = ((bPositive) ? m_iTotalCombatStrengthModifierBase : (-1 * m_iTotalCombatStrengthModifierBase));

		for (int iI = 0; iI < iIterator; iI++)
		{
			if (bPositive)
			{
				iStr *= iSMMultiplier;
				iStr /= 100;
			}
			else
			{
				iStr *= 100;
				iStr /= iSMMultiplier;
			}
		}
	}
	return iStr;
}
int CvUnitInfo::getTotalModifiedAirCombatStrength100() const
{
	int iStr = m_iAirCombat + m_iTotalCombatStrengthChangeBase;
	if (iStr < 0)
	{
		iStr = 0;
	}
	if (iStr !=0)
	{
		iStr *= 100;
		int iStrEval = iStr;
		iStrEval *= m_iTotalCombatStrengthModifierBase;
		iStrEval /= 100;
		iStrEval += iStr;
		iStrEval = getApproaching0Return(iStrEval);
		iStr = std::max(1, iStrEval);
	}
	return iStr;
}
int CvUnitInfo::getApproaching0Return(int i) const
{
	if (i > 10)
	{
		return i;
	}
	else if (i > 0)
	{
		return 9;
	}
	else if (i > -10)
	{
		return 8;
	}
	else if (i > -20)
	{
		return 7;
	}
	else if (i > -40)
	{
		return 6;
	}
	else if (i > -80)
	{
		return 5;
	}
	else if (i > -160)
	{
		return 4;
	}
	else if (i > -320)
	{
		return 3;
	}
	else if (i > -640)
	{
		return 2;
	}
	else
	{
		return 1;
	}
	return 1;
}

int CvUnitInfo::getBaseGroupRank() const
{
	return m_iBaseGroupRank;
}

void CvUnitInfo::setBaseCargoVolume()
{
	int iOffset = 0;
	int iBaseGroup = 0;
	for(int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		if (hasUnitCombat((UnitCombatTypes)iI))
		{
			if (GC.getUnitCombatInfo((UnitCombatTypes)iI).getGroupBase() > -10)
			{
				iOffset += GC.getUnitCombatInfo((UnitCombatTypes)iI).getGroupBase();
				iBaseGroup += GC.getUnitCombatInfo((UnitCombatTypes)iI).getGroupBase();
			}
			if (GC.getUnitCombatInfo((UnitCombatTypes)iI).getSizeBase() > -10)
			{
				iOffset += GC.getUnitCombatInfo((UnitCombatTypes)iI).getSizeBase();
			}
		}
	}
	m_iBaseGroupRank = iBaseGroup;

	int iBase = 100;
	const int iSMMultiplier = GC.getDefineINT("SIZE_MATTERS_MOST_VOLUMETRIC_MULTIPLIER");
	bool bPositive = ((iOffset > 0) ? true : false);
	iBase *= 100;
	if (bPositive)
	{
		for (int iI = 0; iI < iOffset; iI++)
		{
			iBase *= iSMMultiplier;
			iBase /= 100;
		}
	}
	else if (!bPositive)
	{
		for (int iI = 0; iI < -iOffset; iI++)
		{
			iBase *= 100;
			iBase /= iSMMultiplier;
		}
	}
	iBase /= 100;
	m_iBaseCargoVolume = std::max(1, iBase);
}

int CvUnitInfo::getBaseCargoVolume() const
{
	return m_iBaseCargoVolume;
}

void CvUnitInfo::setBaseSizeMattersZeroPoints()
{
	int iFullTotal = 0;
	int iVolumetricTotal = 0;
	for(int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		if (hasUnitCombat((UnitCombatTypes)iI))
		{
			if (GC.getUnitCombatInfo((UnitCombatTypes)iI).getGroupBase() > -10)
			{
				iFullTotal += GC.getUnitCombatInfo((UnitCombatTypes)iI).getGroupBase();
				iVolumetricTotal += GC.getUnitCombatInfo((UnitCombatTypes)iI).getGroupBase();
			}
			if (GC.getUnitCombatInfo((UnitCombatTypes)iI).getSizeBase() > -10)
			{
				iFullTotal += GC.getUnitCombatInfo((UnitCombatTypes)iI).getSizeBase();
				iVolumetricTotal += GC.getUnitCombatInfo((UnitCombatTypes)iI).getSizeBase();
			}
			if (GC.getUnitCombatInfo((UnitCombatTypes)iI).getQualityBase() > -10)
			{
				iFullTotal += GC.getUnitCombatInfo((UnitCombatTypes)iI).getQualityBase();
			}
		}
	}
	m_iBaseSMRankTotal = iFullTotal;
	m_iBaseSMVolumetricRankTotal = iVolumetricTotal;
}

int CvUnitInfo::getSMRankTotal() const
{
	return m_iBaseSMRankTotal;
}

int CvUnitInfo::getSMVolumetricRankTotal() const
{
	return m_iBaseSMVolumetricRankTotal;
}


void CvUnitInfo::setSM()
{
	if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		setTotalModifiedCombatStrengthDetails();
		setBaseCargoVolume();
		setBaseSizeMattersZeroPoints();
	}
}

int CvUnitInfo::getQualifiedPromotionType(int i) const
{
	return m_aiQualifiedPromotionTypes[i];
}

int CvUnitInfo::getNumQualifiedPromotionTypes() const
{
	return (int)m_aiQualifiedPromotionTypes.size();
}

bool CvUnitInfo::isQualifiedPromotionType(int i)
{
	FAssert (i > -1 && i < GC.getNumPromotionInfos());
	if (find(m_aiQualifiedPromotionTypes.begin(), m_aiQualifiedPromotionTypes.end(), i) == m_aiQualifiedPromotionTypes.end())
	{
		return false;
	}
	return true;
}

void CvUnitInfo::setQualifiedPromotionTypes()
{
	bool bQualified = false;
	m_aiQualifiedPromotionTypes.clear();
	for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		for (int iJ = 0; iJ < GC.getPromotionInfo((PromotionTypes)iI).getNumQualifiedUnitCombatTypes(); iJ++)
		{
			if (hasUnitCombat((UnitCombatTypes)GC.getPromotionInfo((PromotionTypes)iI).getQualifiedUnitCombatType(iJ)))
			{
				bQualified = true;
				PromotionLineTypes ePromotionline = GC.getPromotionInfo((PromotionTypes)iI).getPromotionLine();
				if (ePromotionline != NO_PROMOTIONLINE)
				{
					for (int iK = 0; iK < GC.getPromotionLineInfo(ePromotionline).getNumNotOnDomainTypes(); iK++)
					{
						DomainTypes eDomain = (DomainTypes)GC.getPromotionLineInfo(ePromotionline).getNotOnDomainType(iK);
						if (getDomainType() == eDomain)
						{
							bQualified = false;
						}
					}
				}
				for (int iK = 0; iK < GC.getPromotionInfo((PromotionTypes)iI).getNumNotOnDomainTypes(); iK++)
				{
					DomainTypes eDomain = (DomainTypes)GC.getPromotionInfo((PromotionTypes)iI).getNotOnDomainType(iK);
					if (getDomainType() == eDomain)
					{
						bQualified = false;
					}
				}
				if (bQualified)
				{
					m_aiQualifiedPromotionTypes.push_back(iI);
				}
			}
		}
	}
}

void CvUnitInfo::setCanAnimalIgnores()
{
	int iCount = getAnimalIgnoresBorders();
	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		if (hasUnitCombat((UnitCombatTypes)iI) && GC.getUnitCombatInfo((UnitCombatTypes)iI).getAnimalIgnoresBordersChange() != 0)
		{
			iCount += GC.getUnitCombatInfo((UnitCombatTypes)iI).getAnimalIgnoresBordersChange();
		}
	}
	m_bCanAnimalIgnoresBorders = (iCount > 0);
	m_bCanAnimalIgnoresImprovements = (iCount > 1);
	m_bCanAnimalIgnoresCities = (iCount > 2);
}

