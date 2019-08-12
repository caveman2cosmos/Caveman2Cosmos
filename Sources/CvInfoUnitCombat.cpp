#include "CvGameCoreDLL.h"

//======================================================================================================
//					CvPromotionInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvPromotionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvPromotionInfo::CvPromotionInfo() :
m_iLayerAnimationPath(ANIMATIONPATH_NONE),
m_iPrereqPromotion(NO_PROMOTION),
m_iPrereqOrPromotion1(NO_PROMOTION),
m_iPrereqOrPromotion2(NO_PROMOTION),
m_iTechPrereq(NO_TECH),
m_iMinEraType(NO_ERA),
m_iMaxEraType(NO_ERA),
m_iStateReligionPrereq(NO_RELIGION),
m_iVisibilityChange(0),
m_iMovesChange(0),
m_iMoveDiscountChange(0),
m_iAirRangeChange(0),
m_iInterceptChange(0),
m_iEvasionChange(0),
m_iWithdrawalChange(0),
m_iCargoChange(0),
m_iSMCargoChange(0),
m_iSMCargoVolumeChange(0),
m_iSMCargoVolumeModifierChange(0),
m_iCollateralDamageChange(0),
m_iBombardRateChange(0),
m_iFirstStrikesChange(0),
m_iChanceFirstStrikesChange(0),
m_iEnemyHealChange(0),
m_iNeutralHealChange(0),
m_iFriendlyHealChange(0),
m_iSameTileHealChange(0),
m_iAdjacentTileHealChange(0),
m_iCombatPercent(0),
m_iCityAttackPercent(0),
m_iCityDefensePercent(0),
m_iHillsAttackPercent(0),
m_iHillsDefensePercent(0),
m_iHillsWorkPercent(0),
//ls612: Work rate modifiers
m_iWorkRatePercent(0),
m_iCommandType(NO_COMMAND),
m_iRevoltProtection(0),
m_iCollateralDamageProtection(0),
m_iPillageChange(0),
m_iUpgradeDiscount(0),
m_iExperiencePercent(0),
m_iKamikazePercent(0),
/*****************************************************************************************************/
/**  Author: TheLadiesOgre																		  **/
/**  Date: 16.09.2009																			   **/
/**  ModComp: TLOTags																			   **/
/**  Reason Added: New Tag Definition															   **/
/**  Notes:																						 **/
/*****************************************************************************************************/
m_iAirCombatLimitChange(0),
m_iCelebrityHappy(0),
m_iCollateralDamageLimitChange(0),
m_iCollateralDamageMaxUnitsChange(0),
m_iCombatLimitChange(0),
m_iExtraDropRange(0),
//Team Project (2)
m_iSurvivorChance(0),
m_iVictoryAdjacentHeal(0),
m_iVictoryHeal(0),
//Team Project (2)
m_iVictoryStackHeal(0),
m_bDefensiveVictoryMove(false),
m_bFreeDrop(false),
m_bOffensiveVictoryMove(false),
//Team Project (2)
m_bOneUp(false),
m_bPillageCulture(false),
m_bPillageEspionage(false),
m_bPillageMarauder(false),
m_bPillageOnMove(false),
m_bPillageOnVictory(false),
m_bPillageResearch(false),
/*****************************************************************************************************/
/**  TheLadiesOgre; 16.09.2009; TLOTags															 **/
/*****************************************************************************************************/
m_bLeader(false),
m_bBlitz(false),
m_bAmphib(false),
m_bRiver(false),
m_bEnemyRoute(false),
m_bAlwaysHeal(false),
m_bHillsDoubleMove(false),
m_bImmuneToFirstStrikes(false),
m_piTerrainAttackPercent(NULL),
m_piTerrainDefensePercent(NULL),
m_piFeatureAttackPercent(NULL),
m_piFeatureDefensePercent(NULL),
m_piUnitCombatModifierPercent(NULL),
m_piDomainModifierPercent(NULL),
//m_piAIWeightbyUnitCombatTypes(NULL),
m_pbTerrainDoubleMove(NULL),
m_pbFeatureDoubleMove(NULL),
m_pbUnitCombat(NULL),
//ls612: Terrain Work modifiers
m_piTerrainWorkPercent(NULL),
m_piFeatureWorkPercent(NULL),
/************************************************************************************************/
/* Afforess					  Start		 Last Update: 3/8/10									*/
/*																							  */
/*																							  */
/************************************************************************************************/
m_bCanMovePeaks(false)
,m_bCanLeadThroughPeaks(false)
,m_iNumPromotionOverwrites(0)
,m_piPromotionOverwrites(NULL)
,m_iObsoleteTech(NO_TECH)
,m_iControlPoints(0)
,m_iCommandRange(0)
,m_iAssetMultiplier(0)
,m_iPowerMultiplier(0)
,m_iIgnoreTerrainDamage(NO_TERRAIN)
,m_bZoneOfControl(false)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
,m_PropertyManipulators()
//TB Combat Mods Begin
//Textual References
,m_ePromotionLine(NO_PROMOTIONLINE),
m_eReplacesUnitCombat(NO_UNITCOMBAT),
m_eDomainCargoChange(NO_DOMAIN),
m_eSpecialCargoChange(NO_SPECIALUNIT),
m_eSpecialCargoPrereq(NO_SPECIALUNIT),
m_eSMSpecialCargoChange(NO_SPECIALUNIT),
m_eSMSpecialCargoPrereq(NO_SPECIALUNIT),
m_eSMNotSpecialCargoChange(NO_SPECIALUNIT),
m_eSMNotSpecialCargoPrereq(NO_SPECIALUNIT),
m_eSetSpecialUnit(NO_SPECIALUNIT),
//integers
m_iAttackCombatModifierChange(0),
m_iDefenseCombatModifierChange(0),
m_iPursuitChange(0),
m_iEarlyWithdrawChange(0),
m_iVSBarbsChange(0),
m_iArmorChange(0),
m_iPunctureChange(0),
m_iOverrunChange(0),
m_iRepelChange(0),
m_iFortRepelChange(0),
m_iRepelRetriesChange(0),
m_iUnyieldingChange(0),
m_iKnockbackChange(0),
m_iKnockbackRetriesChange(0),
m_iStrAdjperRndChange(0),
m_iStrAdjperAttChange(0),
m_iStrAdjperDefChange(0),
m_iWithdrawAdjperAttChange(0),
m_iUnnerveChange(0),
m_iEncloseChange(0),
m_iLungeChange(0),
m_iDynamicDefenseChange(0),
m_iStrengthChange(0),
m_iLinePriority(0),
m_iFortitudeChange(0),
m_iDamageperTurn(0),
m_iStrAdjperTurn(0),
m_iWeakenperTurn(0),
m_iFrontSupportPercentChange(0),
m_iShortRangeSupportPercentChange(0),
m_iMediumRangeSupportPercentChange(0),
m_iLongRangeSupportPercentChange(0),
m_iFlankSupportPercentChange(0),
m_iDodgeModifierChange(0),
m_iPrecisionModifierChange(0),
m_iPowerShotsChange(0),
m_iPowerShotCombatModifierChange(0),
m_iPowerShotPunctureModifierChange(0),
m_iPowerShotPrecisionModifierChange(0),
m_iPowerShotCriticalModifierChange(0),
m_iCriticalModifierChange(0),
m_iEnduranceChange(0),
m_iRoundStunProbChange(0),
m_iPoisonProbabilityModifierChange(0),
//Team Project (3)
m_iCaptureProbabilityModifierChange(0),
m_iCaptureResistanceModifierChange(0),
//Team Project (4)
//WorkRateMod
m_iHillsWorkModifierChange(0),
m_iPeaksWorkModifierChange(0),
m_iBreakdownChanceChange(0),
m_iBreakdownDamageChange(0),
m_iTauntChange(0),
m_iMaxHPChange(0),
m_iStrengthModifier(0),
m_iQualityChange(0),
m_iGroupChange(0),
m_iLevelPrereq(0),
m_iDamageModifierChange(0),
m_iCostModifierChange(0),
m_iRBombardDamageChange(0),
m_iRBombardDamageLimitChange(0),
m_iRBombardDamageMaxUnitsChange(0),
m_iDCMBombRangeChange(0),
m_iDCMBombAccuracyChange(0),
m_iCombatModifierPerSizeMoreChange(0),
m_iCombatModifierPerSizeLessChange(0),
m_iCombatModifierPerVolumeMoreChange(0),
m_iCombatModifierPerVolumeLessChange(0),
m_iSelfHealModifier(0),
m_iNumHealSupport(0),
m_iExcileChange(0),
m_iPassageChange(0),
m_iNoNonOwnedCityEntryChange(0),
m_iBarbCoExistChange(0),
m_iBlendIntoCityChange(0),
m_iUpgradeAnywhereChange(0),
m_iInsidiousnessChange(0),
m_iInvestigationChange(0),
m_iAssassinChange(0),
m_iStealthStrikesChange(0),
m_iStealthCombatModifierChange(0),
m_iStealthDefenseChange(0),
m_iDefenseOnlyChange(0),
m_iNoInvisibilityChange(0),
m_iTrapDamageMax(0),
m_iTrapDamageMin(0),
m_iTrapComplexity(0),
m_iNumTriggers(0),
m_iTriggerBeforeAttackChange(0),
m_iHiddenNationalityChange(0),
m_iAnimalIgnoresBordersChange(0),
m_iNoDefensiveBonusChange(0),
m_iGatherHerdChange(0),
m_iReligiousCombatModifierChange(0),
//booleans
m_bStampedeChange(false),
m_bRemoveStampede(false),
m_bOnslaughtChange(false),
m_bEquipment(false),
m_bAffliction(false),
m_bParalyze(false),
m_bMakesDamageCold(false),
m_bMakesDamageNotCold(false),
m_bAddsColdImmunity(false),
m_bRemovesColdImmunity(false),
m_bCritical(false),
m_bAttackOnlyCitiesAdd(false),
m_bAttackOnlyCitiesSubtract(false),
m_bIgnoreNoEntryLevelAdd(false),
m_bIgnoreNoEntryLevelSubtract(false),
m_bIgnoreZoneofControlAdd(false),
m_bIgnoreZoneofControlSubtract(false),
m_bFliesToMoveAdd(false),
m_bFliesToMoveSubtract(false),
m_bAnyTerrainAttackPercent(false),
m_bAnyTerrainDefensePercent(false),
m_bAnyFeatureAttackPercent(false),
m_bAnyFeatureDefensePercent(false),
m_bAnyUnitCombatModifierPercent(false),
m_bAnyDomainModifierPercent(false),
//m_bAnyAIWeightbyUnitCombat(false),
m_bZeroesXP(false),
m_bForOffset(false),
m_bCargoPrereq(false),
m_bRBombardPrereq(false),
m_bNoSelfHeal(false),
m_bSetOnHNCapture(false),
m_bSetOnInvestigated(false),
m_bStatus(false),
m_bPrereqNormInvisible(false),
m_bPlotPrereqsKeepAfter(false),
m_bRemoveAfterSet(false),
m_bQuick(false)
//TB Combat Mods End
{
	m_zobristValue = GC.getGameINLINE().getSorenRand().getInt();
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvPromotionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------

CvPromotionInfo::~CvPromotionInfo()
{
	SAFE_DELETE_ARRAY(m_piTerrainAttackPercent);
	SAFE_DELETE_ARRAY(m_piTerrainDefensePercent);
	SAFE_DELETE_ARRAY(m_piFeatureAttackPercent);
	SAFE_DELETE_ARRAY(m_piFeatureDefensePercent);
	SAFE_DELETE_ARRAY(m_piUnitCombatModifierPercent);
	SAFE_DELETE_ARRAY(m_piDomainModifierPercent);
	SAFE_DELETE_ARRAY(m_pbTerrainDoubleMove);
	SAFE_DELETE_ARRAY(m_pbFeatureDoubleMove);
	SAFE_DELETE_ARRAY(m_pbUnitCombat);
	//ls612: Terrain Work Modifiers
	SAFE_DELETE_ARRAY(m_piTerrainWorkPercent);
	SAFE_DELETE_ARRAY(m_piFeatureWorkPercent);
	//TB Combat Mods begin
	//Delayed Resolution vectors

	for (int i=0; i<(int)m_aiAddsBuildTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aiAddsBuildTypes[i]));
	}

	for (int i=0; i<(int)m_aiPrereqLocalBuildingClassTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aiPrereqLocalBuildingClassTypes[i]));
	}

	for (int i=0; i<(int)m_aiTrapSetWithPromotionTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aiTrapSetWithPromotionTypes[i]));
	}

	//TB Combat Mod end
}

int CvPromotionInfo::getLayerAnimationPath() const
{
	return m_iLayerAnimationPath;
}

int CvPromotionInfo::getPrereqPromotion() const
{
	return m_iPrereqPromotion;
}

void CvPromotionInfo::setPrereqPromotion(int i)
{
	m_iPrereqPromotion = i;
}

int CvPromotionInfo::getPrereqOrPromotion1() const
{
	return m_iPrereqOrPromotion1;
}

void CvPromotionInfo::setPrereqOrPromotion1(int i)
{
	m_iPrereqOrPromotion1 = i;
}

int CvPromotionInfo::getPrereqOrPromotion2() const
{
	return m_iPrereqOrPromotion2;
}

void CvPromotionInfo::setPrereqOrPromotion2(int i)
{
	m_iPrereqOrPromotion2 = i;
}

int CvPromotionInfo::getTechPrereq() const
{
	if (m_iTechPrereq == NO_TECH)
	{
		if (getPromotionLine() == NO_PROMOTIONLINE)
		{
			return m_iTechPrereq;
		}
		else
		{
			return GC.getPromotionLineInfo(getPromotionLine()).getPrereqTech();
		}
	}
	else
	{   //Sets up the Tech Prereq on the Promotion Definition as a potential override to the Line Tech Prereq
		return m_iTechPrereq;
	}
}

int CvPromotionInfo::getStateReligionPrereq() const
{
	return m_iStateReligionPrereq;
}

int CvPromotionInfo::getMinEraType() const
{
	return m_iMinEraType;
}

int CvPromotionInfo::getMaxEraType() const
{
	return m_iMaxEraType;
}

int CvPromotionInfo::getVisibilityChange() const
{
	return m_iVisibilityChange;
}

int CvPromotionInfo::getMovesChange() const
{
	return m_iMovesChange;
}

int CvPromotionInfo::getMoveDiscountChange() const
{
	return m_iMoveDiscountChange;
}

int CvPromotionInfo::getAirRangeChange() const
{
	return m_iAirRangeChange;
}

int CvPromotionInfo::getInterceptChange() const
{
	return m_iInterceptChange;
}

int CvPromotionInfo::getEvasionChange() const
{
	return m_iEvasionChange;
}

int CvPromotionInfo::getWithdrawalChange() const
{
	return m_iWithdrawalChange;
}

int CvPromotionInfo::getCargoChange() const
{
	return m_iCargoChange;
}

int CvPromotionInfo::getSMCargoChange() const
{
	return m_iSMCargoChange;
}

int CvPromotionInfo::getSMCargoVolumeChange() const
{
	return m_iSMCargoVolumeChange;
}

int CvPromotionInfo::getSMCargoVolumeModifierChange() const
{
	return m_iSMCargoVolumeModifierChange;
}

int CvPromotionInfo::getCollateralDamageChange() const
{
	return m_iCollateralDamageChange;
}

int CvPromotionInfo::getBombardRateChange() const
{
	return m_iBombardRateChange;
}

int CvPromotionInfo::getFirstStrikesChange() const
{
	return m_iFirstStrikesChange;
}

int CvPromotionInfo::getChanceFirstStrikesChange() const
{
	return m_iChanceFirstStrikesChange;
}

int CvPromotionInfo::getEnemyHealChange() const
{
	return m_iEnemyHealChange;
}

int CvPromotionInfo::getNeutralHealChange() const
{
	return m_iNeutralHealChange;
}

int CvPromotionInfo::getFriendlyHealChange() const
{
	return m_iFriendlyHealChange;
}

int CvPromotionInfo::getSameTileHealChange() const
{
	return m_iSameTileHealChange;
}

int CvPromotionInfo::getAdjacentTileHealChange() const
{
	return m_iAdjacentTileHealChange;
}

int CvPromotionInfo::getCombatPercent() const
{
	return m_iCombatPercent;
}

int CvPromotionInfo::getCityAttackPercent() const
{
	return m_iCityAttackPercent;
}

int CvPromotionInfo::getCityDefensePercent() const
{
	return m_iCityDefensePercent;
}

int CvPromotionInfo::getHillsAttackPercent() const
{
	return m_iHillsAttackPercent;
}

int CvPromotionInfo::getHillsDefensePercent() const
{
	return m_iHillsDefensePercent;
}

int CvPromotionInfo::getHillsWorkPercent() const
{
	return m_iHillsWorkPercent;
}
//ls612: Work rate modifiers

int CvPromotionInfo::getWorkRatePercent() const
{
	return m_iWorkRatePercent;
}

int CvPromotionInfo::getCommandType() const
{
	return m_iCommandType;
}

void CvPromotionInfo::setCommandType(int iNewType)
{
	m_iCommandType = iNewType;
}

int CvPromotionInfo::getRevoltProtection() const
{
	return m_iRevoltProtection;
}

int CvPromotionInfo::getCollateralDamageProtection() const
{
	return m_iCollateralDamageProtection;
}

int CvPromotionInfo::getPillageChange() const
{
	return m_iPillageChange;
}

int CvPromotionInfo::getUpgradeDiscount() const
{
	return m_iUpgradeDiscount;
}

int CvPromotionInfo::getExperiencePercent() const
{
	return m_iExperiencePercent;
}

int CvPromotionInfo::getKamikazePercent() const
{
	return m_iKamikazePercent;
}

/*****************************************************************************************************/
/**  Author: TheLadiesOgre																		  **/
/**  Date: 16.09.2009																			   **/
/**  ModComp: TLOTags																			   **/
/**  Reason Added: New Tag Definition															   **/
/**  Notes:																						 **/
/*****************************************************************************************************/
int CvPromotionInfo::getAirCombatLimitChange() const
{
	return m_iAirCombatLimitChange;
}

int CvPromotionInfo::getCelebrityHappy() const
{
	return m_iCelebrityHappy;
}

int CvPromotionInfo::getCollateralDamageLimitChange() const
{
	return m_iCollateralDamageLimitChange;
}

int CvPromotionInfo::getCollateralDamageMaxUnitsChange() const
{
	return m_iCollateralDamageMaxUnitsChange;
}

int CvPromotionInfo::getCombatLimitChange() const
{
	return m_iCombatLimitChange;
}

int CvPromotionInfo::getExtraDropRange() const
{
	return m_iExtraDropRange;
}

//Team Project (2)
int CvPromotionInfo::getSurvivorChance() const
{
	return m_iSurvivorChance;
}

int CvPromotionInfo::getVictoryAdjacentHeal() const
{
	return m_iVictoryAdjacentHeal;
}

int CvPromotionInfo::getVictoryHeal() const
{
	return m_iVictoryHeal;
}

//Team Project (2)
int CvPromotionInfo::getVictoryStackHeal() const
{
	return m_iVictoryStackHeal;
}

bool CvPromotionInfo::isDefensiveVictoryMove() const
{
	return m_bDefensiveVictoryMove;
}

bool CvPromotionInfo::isFreeDrop() const
{
	return m_bFreeDrop;
}

bool CvPromotionInfo::isOffensiveVictoryMove() const
{
	return m_bOffensiveVictoryMove;
}

//Team Project (2)
bool CvPromotionInfo::isOneUp() const
{
	return m_bOneUp;
}

bool CvPromotionInfo::isPillageCulture() const
{
	return m_bPillageCulture;
}

bool CvPromotionInfo::isPillageEspionage() const
{
	return m_bPillageEspionage;
}

bool CvPromotionInfo::isPillageMarauder() const
{
	return m_bPillageMarauder;
}

bool CvPromotionInfo::isPillageOnMove() const
{
	return m_bPillageOnMove;
}

bool CvPromotionInfo::isPillageOnVictory() const
{
	return m_bPillageOnVictory;
}

bool CvPromotionInfo::isPillageResearch() const
{
	return m_bPillageResearch;
}

/*****************************************************************************************************/
/**  TheLadiesOgre; 16.09.2009; TLOTags															 **/
/*****************************************************************************************************/

bool CvPromotionInfo::isLeader() const
{
	return m_bLeader;
}

bool CvPromotionInfo::isBlitz() const
{
	return m_bBlitz;
}

bool CvPromotionInfo::isAmphib() const
{
	return m_bAmphib;
}

bool CvPromotionInfo::isRiver() const
{
	return m_bRiver;
}

bool CvPromotionInfo::isEnemyRoute() const
{
	return m_bEnemyRoute;
}

bool CvPromotionInfo::isAlwaysHeal() const
{
	return m_bAlwaysHeal;
}

bool CvPromotionInfo::isHillsDoubleMove() const
{
	return m_bHillsDoubleMove;
}

bool CvPromotionInfo::isImmuneToFirstStrikes() const
{
	return m_bImmuneToFirstStrikes;
}

const TCHAR* CvPromotionInfo::getSound() const
{
	return m_szSound;
}

void CvPromotionInfo::setSound(const TCHAR* szVal)
{
	m_szSound = szVal;
}


bool CvPromotionInfo::changesMoveThroughPlots() const
{
	return (isAmphib() ||
			isCanMovePeaks() ||
			isCanLeadThroughPeaks() ||
			m_pbTerrainDoubleMove != NULL ||
			m_pbFeatureDoubleMove != NULL ||
			m_bHillsDoubleMove);
}

// Arrays

int CvPromotionInfo::getTerrainAttackPercent(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piTerrainAttackPercent ? m_piTerrainAttackPercent[i] : 0;
}

bool CvPromotionInfo::isAnyTerrainAttackPercent() const
{
	return m_bAnyTerrainAttackPercent;
}

int CvPromotionInfo::getTerrainDefensePercent(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piTerrainDefensePercent ? m_piTerrainDefensePercent[i] : 0;
}

bool CvPromotionInfo::isAnyTerrainDefensePercent() const
{
	return m_bAnyTerrainDefensePercent;
}

int CvPromotionInfo::getFeatureAttackPercent(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFeatureAttackPercent ? m_piFeatureAttackPercent[i] : 0;
}

bool CvPromotionInfo::isAnyFeatureAttackPercent() const
{
	return m_bAnyFeatureAttackPercent;
}

int CvPromotionInfo::getFeatureDefensePercent(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFeatureDefensePercent ? m_piFeatureDefensePercent[i] : 0;
}

bool CvPromotionInfo::isAnyFeatureDefensePercent() const
{
	return m_bAnyFeatureDefensePercent;
}

int CvPromotionInfo::getUnitCombatModifierPercent(int i) const
{
	FAssertMsg(i < GC.getNumUnitCombatInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piUnitCombatModifierPercent ? m_piUnitCombatModifierPercent[i] : 0;
}

bool CvPromotionInfo::isAnyUnitCombatModifierPercent() const
{
	return m_bAnyUnitCombatModifierPercent;
}

int CvPromotionInfo::getDomainModifierPercent(int i) const
{
	FAssertMsg(i < NUM_DOMAIN_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piDomainModifierPercent ? m_piDomainModifierPercent[i] : 0;
}

bool CvPromotionInfo::isAnyDomainModifierPercent() const
{
	return m_bAnyDomainModifierPercent;
}

bool CvPromotionInfo::getTerrainDoubleMove(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbTerrainDoubleMove ? m_pbTerrainDoubleMove[i] : false;
}

bool CvPromotionInfo::getFeatureDoubleMove(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbFeatureDoubleMove ? m_pbFeatureDoubleMove[i] : false;
}

bool CvPromotionInfo::getUnitCombat(int i) const
{
	FAssertMsg(i < GC.getNumUnitCombatInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbUnitCombat ? m_pbUnitCombat[i] : false;
}
//ls612: Terrain Work Modifiers
int CvPromotionInfo::getTerrainWorkPercent(int i) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piTerrainWorkPercent ? m_piTerrainWorkPercent[i] : 0;
}

int CvPromotionInfo::getFeatureWorkPercent(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFeatureWorkPercent ? m_piFeatureWorkPercent[i] : 0;
}
/************************************************************************************************/
/* Afforess					  Start		 Last Update: 3/8/10									*/
/*																							  */
/*																							  */
/************************************************************************************************/
bool CvPromotionInfo::isCanMovePeaks() const
{
	return m_bCanMovePeaks;
}

//	Koshling - enhanced mountaineering mode to differentiate between ability to move through
//	mountains, and ability to lead a stack through mountains
bool CvPromotionInfo::isCanLeadThroughPeaks() const
{
	return m_bCanLeadThroughPeaks;
}

PromotionTypes CvPromotionInfo::getPromotionOverwrites(int iI) const
{
	return (getNumPromotionOverwrites() > iI)	? (PromotionTypes)m_piPromotionOverwrites[iI]	: NO_PROMOTION;
}

int CvPromotionInfo::getNumPromotionOverwrites() const
{
	return m_iNumPromotionOverwrites;
}

int CvPromotionInfo::getObsoleteTech() const
{
	if (m_iObsoleteTech == NO_TECH)
	{
		if (getPromotionLine() == NO_PROMOTIONLINE)
		{
			return m_iObsoleteTech;
		}
		else
		{
			return GC.getPromotionLineInfo(getPromotionLine()).getObsoleteTech();
		}
	}
	else
	{   //Sets up the Tech Prereq on the Promotion Definition as a potential override to the Line Tech Obsoletion
		return m_iObsoleteTech;
	}
}

int CvPromotionInfo::getControlPoints() const
{
	return m_iControlPoints;
}

int CvPromotionInfo::getCommandRange() const
{
	return m_iCommandRange;
}

int CvPromotionInfo::getAssetMultiplier() const
{
	return m_iAssetMultiplier;
}

int CvPromotionInfo::getPowerMultiplier() const
{
	return m_iPowerMultiplier;
}

int CvPromotionInfo::getIgnoreTerrainDamage() const
{
	return m_iIgnoreTerrainDamage;
}

bool CvPromotionInfo::isZoneOfControl() const
{
	return m_bZoneOfControl;
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
//TB Combat Mods Begin  TB SubCombat Mod begin
const wchar* CvPromotionInfo::getRenamesUnitTo() const
{
	return m_szRenamesUnitTo;
}

//Textual References
PromotionLineTypes CvPromotionInfo::getPromotionLine() const
{
	return m_ePromotionLine;
}

UnitCombatTypes CvPromotionInfo::getReplacesUnitCombat() const
{
	return m_eReplacesUnitCombat;
}

DomainTypes CvPromotionInfo::getDomainCargoChange() const
{
	return m_eDomainCargoChange;
}

SpecialUnitTypes CvPromotionInfo::getSpecialCargoChange() const
{
	return m_eSpecialCargoChange;
}

SpecialUnitTypes CvPromotionInfo::getSpecialCargoPrereq() const
{
	return m_eSpecialCargoPrereq;
}

SpecialUnitTypes CvPromotionInfo::getSMSpecialCargoChange() const
{
	return m_eSMSpecialCargoChange;
}

SpecialUnitTypes CvPromotionInfo::getSMSpecialCargoPrereq() const
{
	return m_eSMSpecialCargoPrereq;
}

SpecialUnitTypes CvPromotionInfo::getSMNotSpecialCargoChange() const
{
	return m_eSMNotSpecialCargoChange;
}

SpecialUnitTypes CvPromotionInfo::getSMNotSpecialCargoPrereq() const
{
	return m_eSMNotSpecialCargoPrereq;
}

SpecialUnitTypes CvPromotionInfo::setSpecialUnit() const
{
	return m_eSetSpecialUnit;
}

// integers
int CvPromotionInfo::getAttackCombatModifierChange() const
{
	return m_iAttackCombatModifierChange;
}

int CvPromotionInfo::getDefenseCombatModifierChange() const
{
	return m_iDefenseCombatModifierChange;
}

int CvPromotionInfo::getPursuitChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_FIGHT_OR_FLIGHT))
	{
		return 0;
	}
	return m_iPursuitChange;
}

int CvPromotionInfo::getEarlyWithdrawChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_FIGHT_OR_FLIGHT))
	{
		return 0;
	}
	return m_iEarlyWithdrawChange;
}

int CvPromotionInfo::getVSBarbsChange() const
{
	return m_iVSBarbsChange;
}

int CvPromotionInfo::getArmorChange() const
{
	return m_iArmorChange;
}

int CvPromotionInfo::getPunctureChange() const
{
	return m_iPunctureChange;
}

//Heart of War
int CvPromotionInfo::getOverrunChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iOverrunChange;
}

int CvPromotionInfo::getRepelChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iRepelChange;
}

int CvPromotionInfo::getFortRepelChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iFortRepelChange;
}

int CvPromotionInfo::getRepelRetriesChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iRepelRetriesChange;
}

int CvPromotionInfo::getUnyieldingChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iUnyieldingChange;
}

int CvPromotionInfo::getKnockbackChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iKnockbackChange;
}

int CvPromotionInfo::getKnockbackRetriesChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iKnockbackRetriesChange;
}
//
//Battleworn
int CvPromotionInfo::getStrAdjperRndChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_BATTLEWORN))
	{
		return 0;
	}
	return m_iStrAdjperRndChange;
}

int CvPromotionInfo::getStrAdjperAttChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_BATTLEWORN))
	{
		return 0;
	}
	return m_iStrAdjperAttChange;
}

int CvPromotionInfo::getStrAdjperDefChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_BATTLEWORN))
	{
		return 0;
	}
	return m_iStrAdjperDefChange;
}

int CvPromotionInfo::getWithdrawAdjperAttChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_BATTLEWORN))
	{
		return 0;
	}
	return m_iWithdrawAdjperAttChange;
}
//
//S&D extended
int CvPromotionInfo::getUnnerveChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SAD))
	{
		return 0;
	}
	return m_iUnnerveChange;
}

int CvPromotionInfo::getEncloseChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SAD))
	{
		return 0;
	}
	return m_iEncloseChange;
}

int CvPromotionInfo::getLungeChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SAD))
	{
		return 0;
	}
	return m_iLungeChange;
}

int CvPromotionInfo::getDynamicDefenseChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SAD))
	{
		return 0;
	}
	return m_iDynamicDefenseChange;
}
//

int CvPromotionInfo::getStrengthChange() const
{
	return m_iStrengthChange;
}

int CvPromotionInfo::getLinePriority() const
{
	return m_iLinePriority;
}

//Realm of the Serpent
int CvPromotionInfo::getFortitudeChange() const
{
	return m_iFortitudeChange;
}

int CvPromotionInfo::getDamageperTurn() const
{
	return m_iDamageperTurn;
}

int CvPromotionInfo::getStrAdjperTurn() const
{
	return m_iStrAdjperTurn;
}

int CvPromotionInfo::getWeakenperTurn() const
{
	return m_iWeakenperTurn;
}
//
//Strength in Numbers
int CvPromotionInfo::getFrontSupportPercentChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_iFrontSupportPercentChange;
}

int CvPromotionInfo::getShortRangeSupportPercentChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_iShortRangeSupportPercentChange;
}

int CvPromotionInfo::getMediumRangeSupportPercentChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_iMediumRangeSupportPercentChange;
}

int CvPromotionInfo::getLongRangeSupportPercentChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_iLongRangeSupportPercentChange;
}

int CvPromotionInfo::getFlankSupportPercentChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_iFlankSupportPercentChange;
}
//

int CvPromotionInfo::getDodgeModifierChange() const
{
	return m_iDodgeModifierChange;
}

int CvPromotionInfo::getPrecisionModifierChange() const
{
	return m_iPrecisionModifierChange;
}

int CvPromotionInfo::getPowerShotsChange() const
{
	return m_iPowerShotsChange;
}

int CvPromotionInfo::getPowerShotCombatModifierChange() const
{
	return m_iPowerShotCombatModifierChange;
}

int CvPromotionInfo::getPowerShotPunctureModifierChange() const
{
	return m_iPowerShotPunctureModifierChange;
}

int CvPromotionInfo::getPowerShotPrecisionModifierChange() const
{
	return m_iPowerShotPrecisionModifierChange;
}

int CvPromotionInfo::getPowerShotCriticalModifierChange() const
{
	return m_iPowerShotCriticalModifierChange;
}

int CvPromotionInfo::getCriticalModifierChange() const
{
	return m_iCriticalModifierChange;
}

int CvPromotionInfo::getEnduranceChange() const
{
	return m_iEnduranceChange;
}

int CvPromotionInfo::getRoundStunProbChange() const
{
	return m_iRoundStunProbChange;
}

int CvPromotionInfo::getPoisonProbabilityModifierChange() const
{
	return m_iPoisonProbabilityModifierChange;
}

//Team Project (3)
int CvPromotionInfo::getCaptureProbabilityModifierChange() const
{
	return m_iCaptureProbabilityModifierChange;
}

int CvPromotionInfo::getCaptureResistanceModifierChange() const
{
	return m_iCaptureResistanceModifierChange;
}

//Team Project (4)
//WorkRateMod
int CvPromotionInfo::getHillsWorkModifierChange() const
{
	return m_iHillsWorkModifierChange;
}

int CvPromotionInfo::getPeaksWorkModifierChange() const
{
	return m_iPeaksWorkModifierChange;
}

int CvPromotionInfo::getBreakdownChanceChange() const
{
	return m_iBreakdownChanceChange;
}

int CvPromotionInfo::getBreakdownDamageChange() const
{
	return m_iBreakdownDamageChange;
}

int CvPromotionInfo::getTauntChange() const
{
	return m_iTauntChange;
}

int CvPromotionInfo::getMaxHPChange() const
{
	return m_iMaxHPChange;
}

int CvPromotionInfo::getStrengthModifier() const
{
	return m_iStrengthModifier;
}

int CvPromotionInfo::getQualityChange() const
{
	return m_iQualityChange;
}

int CvPromotionInfo::getGroupChange() const
{
	return m_iGroupChange;
}

int CvPromotionInfo::getLevelPrereq() const
{
	return m_iLevelPrereq;
}

int CvPromotionInfo::getDamageModifierChange() const
{
	return m_iDamageModifierChange;
}

int CvPromotionInfo::getCostModifierChange() const
{
	return m_iCostModifierChange;
}

int CvPromotionInfo::getRBombardDamageChange() const
{
	return m_iRBombardDamageChange;
}

int CvPromotionInfo::getRBombardDamageLimitChange() const
{
	return m_iRBombardDamageLimitChange;
}

int CvPromotionInfo::getRBombardDamageMaxUnitsChange() const
{
	return m_iRBombardDamageMaxUnitsChange;
}

int CvPromotionInfo::getDCMBombRangeChange() const
{
	return m_iDCMBombRangeChange;
}

int CvPromotionInfo::getDCMBombAccuracyChange() const
{
	return m_iDCMBombAccuracyChange;
}

int CvPromotionInfo::getCombatModifierPerSizeMoreChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerSizeMoreChange;
}

int CvPromotionInfo::getCombatModifierPerSizeLessChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerSizeLessChange;
}

int CvPromotionInfo::getCombatModifierPerVolumeMoreChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerVolumeMoreChange;
}

int CvPromotionInfo::getCombatModifierPerVolumeLessChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerVolumeLessChange;
}

int CvPromotionInfo::getSelfHealModifier() const
{
	return m_iSelfHealModifier;
}

int CvPromotionInfo::getNumHealSupport() const
{
	return m_iNumHealSupport;
}

int CvPromotionInfo::getExcileChange() const
{
	return m_iExcileChange;
}

int CvPromotionInfo::getPassageChange() const
{
	return m_iPassageChange;
}

int CvPromotionInfo::getNoNonOwnedCityEntryChange() const
{
	return m_iNoNonOwnedCityEntryChange;
}

int CvPromotionInfo::getBarbCoExistChange() const
{
	return m_iBarbCoExistChange;
}

int CvPromotionInfo::getBlendIntoCityChange() const
{
	return m_iBlendIntoCityChange;
}

int CvPromotionInfo::getUpgradeAnywhereChange() const
{
	return m_iUpgradeAnywhereChange;
}

int CvPromotionInfo::getInsidiousnessChange() const
{
	return m_iInsidiousnessChange;
}

int CvPromotionInfo::getInvestigationChange() const
{
	return m_iInvestigationChange;
}

int CvPromotionInfo::getAssassinChange() const
{
	return m_iAssassinChange;
}

int CvPromotionInfo::getStealthStrikesChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_WITHOUT_WARNING))
	{
		return 0;
	}
	return m_iStealthStrikesChange;
}

int CvPromotionInfo::getStealthCombatModifierChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_WITHOUT_WARNING))
	{
		return 0;
	}
	return m_iStealthCombatModifierChange;
}

int CvPromotionInfo::getStealthDefenseChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_WITHOUT_WARNING))
	{
		return 0;
	}
	return m_iStealthDefenseChange;
}

int CvPromotionInfo::getDefenseOnlyChange() const
{
	return m_iDefenseOnlyChange;
}

int CvPromotionInfo::getNoInvisibilityChange() const
{
	return m_iNoInvisibilityChange;
}

int CvPromotionInfo::getTrapDamageMax() const
{
	return m_iTrapDamageMax;
}

int CvPromotionInfo::getTrapDamageMin() const
{
	return m_iTrapDamageMin;
}

int CvPromotionInfo::getTrapComplexity() const
{
	return m_iTrapComplexity;
}

int CvPromotionInfo::getNumTriggers() const
{
	return m_iNumTriggers;
}

int CvPromotionInfo::getTriggerBeforeAttackChange() const
{
	return m_iTriggerBeforeAttackChange;
}

int CvPromotionInfo::getHiddenNationalityChange() const
{
	return m_iHiddenNationalityChange;
}

int CvPromotionInfo::getAnimalIgnoresBordersChange() const
{
	return m_iAnimalIgnoresBordersChange;
}

int CvPromotionInfo::getNoDefensiveBonusChange() const
{
	return m_iNoDefensiveBonusChange;
}

int CvPromotionInfo::getGatherHerdChange() const
{
	return m_iGatherHerdChange;
}

int CvPromotionInfo::getReligiousCombatModifierChange() const
{
	return m_iReligiousCombatModifierChange;
}
// booleans
bool CvPromotionInfo::isStampedeChange() const
{
	return m_bStampedeChange;
}

bool CvPromotionInfo::isRemoveStampede() const
{
	return m_bRemoveStampede;
}

bool CvPromotionInfo::isOnslaughtChange() const
{
	return m_bOnslaughtChange;
}

bool CvPromotionInfo::isEquipment() const
{
	if ( getPromotionLine() == NO_PROMOTIONLINE)
	{
		return false; // require afflictions to always be parts of lines
	}

	// return the value from the line
	return GC.getPromotionLineInfo(getPromotionLine()).isEquipment();
}

bool CvPromotionInfo::isAffliction() const
{
	if ( getPromotionLine() == NO_PROMOTIONLINE)
	{
		return false; // require afflictions to always be parts of lines
	}

	// return the value from the line
	return GC.getPromotionLineInfo(getPromotionLine()).isAffliction();
}

bool CvPromotionInfo::isParalyze() const
{
	return m_bParalyze;
}

bool CvPromotionInfo::isMakesDamageCold() const
{
	return m_bMakesDamageCold;
}

bool CvPromotionInfo::isMakesDamageNotCold() const
{
	return m_bMakesDamageNotCold;
}

bool CvPromotionInfo::isAddsColdImmunity() const
{
	return m_bAddsColdImmunity;
}

bool CvPromotionInfo::isRemovesColdImmunity() const
{
	return m_bRemovesColdImmunity;
}

bool CvPromotionInfo::isCritical() const
{
	if ( getPromotionLine() == NO_PROMOTIONLINE)
	{
		return false; // require afflictions to always be parts of lines
	}

	// return the value from the line
	return GC.getPromotionLineInfo(getPromotionLine()).isCritical();
}

bool CvPromotionInfo::isAttackOnlyCitiesAdd() const
{
	return m_bAttackOnlyCitiesAdd;
}

bool CvPromotionInfo::isAttackOnlyCitiesSubtract() const
{
	return m_bAttackOnlyCitiesSubtract;
}

bool CvPromotionInfo::isIgnoreNoEntryLevelAdd() const
{
	return m_bIgnoreNoEntryLevelAdd;
}

bool CvPromotionInfo::isIgnoreNoEntryLevelSubtract() const
{
	return m_bIgnoreNoEntryLevelSubtract;
}

bool CvPromotionInfo::isIgnoreZoneofControlAdd() const
{
	return m_bIgnoreZoneofControlAdd;
}

bool CvPromotionInfo::isIgnoreZoneofControlSubtract() const
{
	return m_bIgnoreZoneofControlSubtract;
}

bool CvPromotionInfo::isFliesToMoveAdd() const
{
	return m_bFliesToMoveAdd;
}

bool CvPromotionInfo::isFliesToMoveSubtract() const
{
	return m_bFliesToMoveSubtract;
}

bool CvPromotionInfo::isZeroesXP() const
{
	return m_bZeroesXP;
}

bool CvPromotionInfo::isForOffset() const
{
	return m_bForOffset;
}

bool CvPromotionInfo::isCargoPrereq() const
{
	return m_bCargoPrereq;
}

bool CvPromotionInfo::isRBombardPrereq() const
{
	return m_bRBombardPrereq;
}

bool CvPromotionInfo::isNoSelfHeal() const
{
	return m_bNoSelfHeal;
}

bool CvPromotionInfo::isSetOnHNCapture() const
{
	return m_bSetOnHNCapture;
}

bool CvPromotionInfo::isSetOnInvestigated() const
{
	return m_bSetOnInvestigated;
}

bool CvPromotionInfo::isStatus() const
{
	return m_bStatus;
}

bool CvPromotionInfo::isPrereqNormInvisible() const
{
	return m_bPrereqNormInvisible;
}

bool CvPromotionInfo::isPlotPrereqsKeepAfter() const
{
	return m_bPlotPrereqsKeepAfter;
}

bool CvPromotionInfo::isRemoveAfterSet() const
{
	return m_bRemoveAfterSet;
}

bool CvPromotionInfo::isQuick() const
{
	return m_bQuick;
}

// bool vectors without delayed resolution
int CvPromotionInfo::getSubCombatChangeType(int i) const
{
	return m_aiSubCombatChangeTypes[i];
}

int CvPromotionInfo::getNumSubCombatChangeTypes() const
{
	return (int)m_aiSubCombatChangeTypes.size();
}

bool CvPromotionInfo::isSubCombatChangeType(int i)
{
	FAssert (i > -1 && i < GC.getNumUnitCombatInfos());
	if (find(m_aiSubCombatChangeTypes.begin(), m_aiSubCombatChangeTypes.end(), i) == m_aiSubCombatChangeTypes.end())
	{
		return false;
	}
	return true;
}

int CvPromotionInfo::getRemovesUnitCombatType(int i) const
{
	return m_aiRemovesUnitCombatTypes[i];
}

int CvPromotionInfo::getNumRemovesUnitCombatTypes() const
{
	return (int)m_aiRemovesUnitCombatTypes.size();
}

bool CvPromotionInfo::isRemovesUnitCombatType(int i)
{
	FAssert (i > -1 && i < GC.getNumUnitCombatInfos());
	if (find(m_aiRemovesUnitCombatTypes.begin(), m_aiRemovesUnitCombatTypes.end(), i) == m_aiRemovesUnitCombatTypes.end())
	{
		return false;
	}
	return true;
}

int CvPromotionInfo::getOnGameOption(int i) const
{
	return m_aiOnGameOptions[i];
}

int CvPromotionInfo::getNumOnGameOptions() const
{
	return (int)m_aiOnGameOptions.size();
}

bool CvPromotionInfo::isOnGameOption(int i)
{
	FAssert (i > -1 && i < GC.getNumGameOptions());
	if (find(m_aiOnGameOptions.begin(), m_aiOnGameOptions.end(), i) == m_aiOnGameOptions.end())
	{
		return false;
	}
	return true;
}

int CvPromotionInfo::getNotOnGameOption(int i) const
{
	return m_aiNotOnGameOptions[i];
}

int CvPromotionInfo::getNumNotOnGameOptions() const
{
	return (int)m_aiNotOnGameOptions.size();
}

bool CvPromotionInfo::isNotOnGameOption(int i)
{
	FAssert (i > -1 && i < GC.getNumGameOptions());
	if (find(m_aiNotOnGameOptions.begin(), m_aiNotOnGameOptions.end(), i) == m_aiNotOnGameOptions.end())
	{
		return false;
	}
	return true;
}

int CvPromotionInfo::getFreetoUnitCombat(int i) const
{
	return m_aiFreetoUnitCombats[i];
}

int CvPromotionInfo::getNumFreetoUnitCombats() const
{
	return (int)m_aiFreetoUnitCombats.size();
}

bool CvPromotionInfo::isFreetoUnitCombat(int i)
{
	FAssert (i > -1 && i < GC.getNumUnitCombatInfos());
	if (find(m_aiFreetoUnitCombats.begin(), m_aiFreetoUnitCombats.end(), i) == m_aiFreetoUnitCombats.end())
	{
		return false;
	}
	return true;
}

int CvPromotionInfo::getNotOnUnitCombatType(int i) const
{
	return m_aiNotOnUnitCombatTypes[i];
}

int CvPromotionInfo::getNumNotOnUnitCombatTypes() const
{
	return (int)m_aiNotOnUnitCombatTypes.size();
}

bool CvPromotionInfo::isNotOnUnitCombatType(int i)
{
	FAssert (i > -1 && i < GC.getNumUnitCombatInfos());
	if (find(m_aiNotOnUnitCombatTypes.begin(), m_aiNotOnUnitCombatTypes.end(), i) == m_aiNotOnUnitCombatTypes.end())
	{
		return false;
	}
	return true;
}

int CvPromotionInfo::getNotOnDomainType(int i) const
{
	return m_aiNotOnDomainTypes[i];
}

int CvPromotionInfo::getNumNotOnDomainTypes() const
{
	return (int)m_aiNotOnDomainTypes.size();
}

bool CvPromotionInfo::isNotOnDomainType(int i)
{
	FAssert (i > -1 && i < NUM_DOMAIN_TYPES);
	if (find(m_aiNotOnDomainTypes.begin(), m_aiNotOnDomainTypes.end(), i) == m_aiNotOnDomainTypes.end())
	{
		return false;
	}
	return true;
}

int CvPromotionInfo::getNoAutoEquiptoCombatClassType(int i) const
{
	return m_aiNoAutoEquiptoCombatClassTypes[i];
}

int CvPromotionInfo::getNumNoAutoEquiptoCombatClassTypes() const
{
	return (int)m_aiNoAutoEquiptoCombatClassTypes.size();
}

bool CvPromotionInfo::isNoAutoEquiptoCombatClassType(int i)
{
	FAssert (i > -1 && i < GC.getNumUnitCombatInfos());
	if (find(m_aiNoAutoEquiptoCombatClassTypes.begin(), m_aiNoAutoEquiptoCombatClassTypes.end(), i) == m_aiNoAutoEquiptoCombatClassTypes.end())
	{
		return false;
	}
	return true;
}

int CvPromotionInfo::getMapCategoryType(int i) const
{
	return m_aiMapCategoryTypes[i];
}

int CvPromotionInfo::getNumMapCategoryTypes() const
{
	return (int)m_aiMapCategoryTypes.size();
}

bool CvPromotionInfo::isMapCategoryType(int i)
{
	FAssert (i > -1 && i < GC.getNumMapCategoryInfos()); // do not include this line if for delayed resolution
	if (find(m_aiMapCategoryTypes.begin(), m_aiMapCategoryTypes.end(), i) == m_aiMapCategoryTypes.end())
	{
		return false;
	}
	return true;
}
//Arrays

//int CvPromotionInfo::getAIWeightbyUnitCombatType(int i) const
//{
//	FAssertMsg(i < GC.getNumUnitCombatInfos(), "Index out of bounds");
//	FAssertMsg(i > -1, "Index out of bounds");
//	return m_piAIWeightbyUnitCombatTypes ? m_piAIWeightbyUnitCombatTypes[i] : 0;
//}

//bool CvPromotionInfo::isAnyAIWeightbyUnitCombatType() const
//{
//	return m_bAnyAIWeightbyUnitCombat;
//}

// bool vector with delayed resolution
int CvPromotionInfo::getCureAfflictionChangeType(int i) const
{
	return m_aiCureAfflictionChangeTypes[i];
}

int CvPromotionInfo::getNumCureAfflictionChangeTypes() const
{
	return (int)m_aiCureAfflictionChangeTypes.size();
}

bool CvPromotionInfo::isCureAfflictionChangeType(int i)
{
	if (find(m_aiCureAfflictionChangeTypes.begin(), m_aiCureAfflictionChangeTypes.end(), i) == m_aiCureAfflictionChangeTypes.end())
	{
		return false;
	}
	return true;
}

int CvPromotionInfo::getPrereqBonusType(int i) const
{
	return m_aiPrereqBonusTypes[i];
}

int CvPromotionInfo::getNumPrereqBonusTypes() const
{
	return (int)m_aiPrereqBonusTypes.size();
}

bool CvPromotionInfo::isPrereqBonusType(int i)
{
	if (find(m_aiPrereqBonusTypes.begin(), m_aiPrereqBonusTypes.end(), i) == m_aiPrereqBonusTypes.end())
	{
		return false;
	}
	return true;
}

int CvPromotionInfo::getAddsBuildType(int i) const
{
	return m_aiAddsBuildTypes[i];
}

int CvPromotionInfo::getNumAddsBuildTypes() const
{
	return (int)m_aiAddsBuildTypes.size();
}

bool CvPromotionInfo::isAddsBuildType(int i)
{
	if (find(m_aiAddsBuildTypes.begin(), m_aiAddsBuildTypes.end(), i) == m_aiAddsBuildTypes.end())
	{
		return false;
	}
	return true;
}

int CvPromotionInfo::getNegatesInvisibilityType(int i) const
{
	return m_aiNegatesInvisibilityTypes[i];
}

int CvPromotionInfo::getNumNegatesInvisibilityTypes() const
{
	return (int)m_aiNegatesInvisibilityTypes.size();
}

bool CvPromotionInfo::isNegatesInvisibilityType(int i)
{
	if (find(m_aiNegatesInvisibilityTypes.begin(), m_aiNegatesInvisibilityTypes.end(), i) == m_aiNegatesInvisibilityTypes.end())
	{
		return false;
	}
	return true;
}

int CvPromotionInfo::getPrereqTerrainType(int i) const
{
	return m_aiPrereqTerrainTypes[i];
}

int CvPromotionInfo::getNumPrereqTerrainTypes() const
{
	return (int)m_aiPrereqTerrainTypes.size();
}

bool CvPromotionInfo::isPrereqTerrainType(int i)
{
	if (find(m_aiPrereqTerrainTypes.begin(), m_aiPrereqTerrainTypes.end(), i) == m_aiPrereqTerrainTypes.end())
	{
		return false;
	}
	return true;
}

int CvPromotionInfo::getPrereqFeatureType(int i) const
{
	return m_aiPrereqFeatureTypes[i];
}

int CvPromotionInfo::getNumPrereqFeatureTypes() const
{
	return (int)m_aiPrereqFeatureTypes.size();
}

bool CvPromotionInfo::isPrereqFeatureType(int i)
{
	if (find(m_aiPrereqFeatureTypes.begin(), m_aiPrereqFeatureTypes.end(), i) == m_aiPrereqFeatureTypes.end())
	{
		return false;
	}
	return true;
}

int CvPromotionInfo::getPrereqImprovementType(int i) const
{
	return m_aiPrereqImprovementTypes[i];
}

int CvPromotionInfo::getNumPrereqImprovementTypes() const
{
	return (int)m_aiPrereqImprovementTypes.size();
}

bool CvPromotionInfo::isPrereqImprovementType(int i)
{
	if (find(m_aiPrereqImprovementTypes.begin(), m_aiPrereqImprovementTypes.end(), i) == m_aiPrereqImprovementTypes.end())
	{
		return false;
	}
	return true;
}

int CvPromotionInfo::getPrereqPlotBonusType(int i) const
{
	return m_aiPrereqPlotBonusTypes[i];
}

int CvPromotionInfo::getNumPrereqPlotBonusTypes() const
{
	return (int)m_aiPrereqPlotBonusTypes.size();
}

bool CvPromotionInfo::isPrereqPlotBonusType(int i)
{
	if (find(m_aiPrereqPlotBonusTypes.begin(), m_aiPrereqPlotBonusTypes.end(), i) == m_aiPrereqPlotBonusTypes.end())
	{
		return false;
	}
	return true;
}

int CvPromotionInfo::getPrereqLocalBuildingClassType(int i) const
{
	return m_aiPrereqLocalBuildingClassTypes[i];
}

int CvPromotionInfo::getNumPrereqLocalBuildingClassTypes() const
{
	return (int)m_aiPrereqLocalBuildingClassTypes.size();
}

bool CvPromotionInfo::isPrereqLocalBuildingClassType(int i)
{
	if (find(m_aiPrereqLocalBuildingClassTypes.begin(), m_aiPrereqLocalBuildingClassTypes.end(), i) == m_aiPrereqLocalBuildingClassTypes.end())
	{
		return false;
	}
	return true;
}

int CvPromotionInfo::getTrapSetWithPromotionType(int i) const
{
	return m_aiTrapSetWithPromotionTypes[i];
}

int CvPromotionInfo::getNumTrapSetWithPromotionTypes() const
{
	return (int)m_aiTrapSetWithPromotionTypes.size();
}

bool CvPromotionInfo::isTrapSetWithPromotionType(int i)
{
	if (find(m_aiTrapSetWithPromotionTypes.begin(), m_aiTrapSetWithPromotionTypes.end(), i) == m_aiTrapSetWithPromotionTypes.end())
	{
		return false;
	}
	return true;
}

int CvPromotionInfo::getTrapImmunityUnitCombatType(int i) const
{
	return m_aiTrapImmunityUnitCombatTypes[i];
}

int CvPromotionInfo::getNumTrapImmunityUnitCombatTypes() const
{
	return (int)m_aiTrapImmunityUnitCombatTypes.size();
}

bool CvPromotionInfo::isTrapImmunityUnitCombatType(int i)
{
	if (find(m_aiTrapImmunityUnitCombatTypes.begin(), m_aiTrapImmunityUnitCombatTypes.end(), i) == m_aiTrapImmunityUnitCombatTypes.end())
	{
		return false;
	}
	return true;
}

int CvPromotionInfo::getTargetUnitCombatType(int i) const
{
	return m_aiTargetUnitCombatTypes[i];
}

int CvPromotionInfo::getNumTargetUnitCombatTypes() const
{
	return (int)m_aiTargetUnitCombatTypes.size();
}

bool CvPromotionInfo::isTargetUnitCombatType(int i)
{
	if (find(m_aiTargetUnitCombatTypes.begin(), m_aiTargetUnitCombatTypes.end(), i) == m_aiTargetUnitCombatTypes.end())
	{
		return false;
	}
	return true;
}

// int vectors utilizing pairing without delayed resolution
int CvPromotionInfo::getNumFlankingStrikesbyUnitCombatTypesChange() const
{
	return m_aFlankingStrengthbyUnitCombatTypeChange.size();
}

int CvPromotionInfo::getFlankingStrengthbyUnitCombatTypeChange(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aFlankingStrengthbyUnitCombatTypeChange.begin(); it != m_aFlankingStrengthbyUnitCombatTypeChange.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isFlankingStrikebyUnitCombatTypeChange(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aFlankingStrengthbyUnitCombatTypeChange.begin(); it != m_aFlankingStrengthbyUnitCombatTypeChange.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumWithdrawOnTerrainTypeChanges() const
{
	return m_aWithdrawOnTerrainTypesChange.size();
}

int CvPromotionInfo::getWithdrawOnTerrainTypeChange(int iTerrain) const
{
	for (TerrainModifierArray::const_iterator it = m_aWithdrawOnTerrainTypesChange.begin(); it != m_aWithdrawOnTerrainTypesChange.end(); ++it)
	{
		if ((*it).first == (TerrainTypes)iTerrain)
		{
			return (*it).second;
		}
	}
	return 0;
}

int CvPromotionInfo::getNumWithdrawOnFeatureTypeChanges() const
{
	return m_aWithdrawOnFeatureTypesChange.size();
}

int CvPromotionInfo::getWithdrawOnFeatureTypeChange(int iFeature) const
{
	for (FeatureModifierArray::const_iterator it = m_aWithdrawOnFeatureTypesChange.begin(); it != m_aWithdrawOnFeatureTypesChange.end(); ++it)
	{
		if ((*it).first == (FeatureTypes)iFeature)
		{
			return (*it).second;
		}
	}
	return 0;
}

int CvPromotionInfo::getNumWithdrawVSUnitCombatChangeTypes() const
{
	return m_aWithdrawVSUnitCombatChangeTypes.size();
}

int CvPromotionInfo::getWithdrawVSUnitCombatChangeType(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aWithdrawVSUnitCombatChangeTypes.begin(); it != m_aWithdrawVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isWithdrawVSUnitCombatChangeType(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aWithdrawVSUnitCombatChangeTypes.begin(); it != m_aWithdrawVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumPursuitVSUnitCombatChangeTypes() const
{
	return m_aPursuitVSUnitCombatChangeTypes.size();
}

int CvPromotionInfo::getPursuitVSUnitCombatChangeType(int iUnitCombat, bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_FIGHT_OR_FLIGHT))
	{
		return 0;
	}
	for (UnitCombatModifierArray::const_iterator it = m_aPursuitVSUnitCombatChangeTypes.begin(); it != m_aPursuitVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isPursuitVSUnitCombatChangeType(int iUnitCombat, bool bForLoad)
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_FIGHT_OR_FLIGHT))
	{
		return false;
	}

	for (UnitCombatModifierArray::const_iterator it = m_aPursuitVSUnitCombatChangeTypes.begin(); it != m_aPursuitVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumRepelVSUnitCombatChangeTypes() const
{
	return m_aRepelVSUnitCombatChangeTypes.size();
}

int CvPromotionInfo::getRepelVSUnitCombatChangeType(int iUnitCombat, bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	for (UnitCombatModifierArray::const_iterator it = m_aRepelVSUnitCombatChangeTypes.begin(); it != m_aRepelVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isRepelVSUnitCombatChangeType(int iUnitCombat, bool bForLoad)
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return false;
	}
	for (UnitCombatModifierArray::const_iterator it = m_aRepelVSUnitCombatChangeTypes.begin(); it != m_aRepelVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumKnockbackVSUnitCombatChangeTypes() const
{
	return m_aKnockbackVSUnitCombatChangeTypes.size();
}

int CvPromotionInfo::getKnockbackVSUnitCombatChangeType(int iUnitCombat, bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	for (UnitCombatModifierArray::const_iterator it = m_aKnockbackVSUnitCombatChangeTypes.begin(); it != m_aKnockbackVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isKnockbackVSUnitCombatChangeType(int iUnitCombat, bool bForLoad)
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return false;
	}
	for (UnitCombatModifierArray::const_iterator it = m_aKnockbackVSUnitCombatChangeTypes.begin(); it != m_aKnockbackVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumPunctureVSUnitCombatChangeTypes() const
{
	return m_aPunctureVSUnitCombatChangeTypes.size();
}

int CvPromotionInfo::getPunctureVSUnitCombatChangeType(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aPunctureVSUnitCombatChangeTypes.begin(); it != m_aPunctureVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isPunctureVSUnitCombatChangeType(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aPunctureVSUnitCombatChangeTypes.begin(); it != m_aPunctureVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumArmorVSUnitCombatChangeTypes() const
{
	return m_aArmorVSUnitCombatChangeTypes.size();
}

int CvPromotionInfo::getArmorVSUnitCombatChangeType(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aArmorVSUnitCombatChangeTypes.begin(); it != m_aArmorVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isArmorVSUnitCombatChangeType(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aArmorVSUnitCombatChangeTypes.begin(); it != m_aArmorVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumDodgeVSUnitCombatChangeTypes() const
{
	return m_aDodgeVSUnitCombatChangeTypes.size();
}

int CvPromotionInfo::getDodgeVSUnitCombatChangeType(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aDodgeVSUnitCombatChangeTypes.begin(); it != m_aDodgeVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isDodgeVSUnitCombatChangeType(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aDodgeVSUnitCombatChangeTypes.begin(); it != m_aDodgeVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumPrecisionVSUnitCombatChangeTypes() const
{
	return m_aPrecisionVSUnitCombatChangeTypes.size();
}

int CvPromotionInfo::getPrecisionVSUnitCombatChangeType(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aPrecisionVSUnitCombatChangeTypes.begin(); it != m_aPrecisionVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isPrecisionVSUnitCombatChangeType(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aPrecisionVSUnitCombatChangeTypes.begin(); it != m_aPrecisionVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumCriticalVSUnitCombatChangeTypes() const
{
	return m_aCriticalVSUnitCombatChangeTypes.size();
}

int CvPromotionInfo::getCriticalVSUnitCombatChangeType(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aCriticalVSUnitCombatChangeTypes.begin(); it != m_aCriticalVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isCriticalVSUnitCombatChangeType(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aCriticalVSUnitCombatChangeTypes.begin(); it != m_aCriticalVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumRoundStunVSUnitCombatChangeTypes() const
{
	return m_aRoundStunVSUnitCombatChangeTypes.size();
}

int CvPromotionInfo::getRoundStunVSUnitCombatChangeType(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aRoundStunVSUnitCombatChangeTypes.begin(); it != m_aRoundStunVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isRoundStunVSUnitCombatChangeType(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aRoundStunVSUnitCombatChangeTypes.begin(); it != m_aRoundStunVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumTrapDisableUnitCombatTypes() const
{
	return m_aTrapDisableUnitCombatTypes.size();
}

int CvPromotionInfo::getTrapDisableUnitCombatType(int iUnitCombat) const
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

bool CvPromotionInfo::isTrapDisableUnitCombatType(int iUnitCombat)
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

int CvPromotionInfo::getNumTrapAvoidanceUnitCombatTypes() const
{
	return m_aTrapAvoidanceUnitCombatTypes.size();
}

int CvPromotionInfo::getTrapAvoidanceUnitCombatType(int iUnitCombat) const
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

bool CvPromotionInfo::isTrapAvoidanceUnitCombatType(int iUnitCombat)
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

int CvPromotionInfo::getNumTrapTriggerUnitCombatTypes() const
{
	return m_aTrapTriggerUnitCombatTypes.size();
}

int CvPromotionInfo::getTrapTriggerUnitCombatType(int iUnitCombat) const
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

bool CvPromotionInfo::isTrapTriggerUnitCombatType(int iUnitCombat)
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

int CvPromotionInfo::getNumAidChanges() const
{
	return m_aAidChanges.size();
}

int CvPromotionInfo::getAidChange(int iProperty) const
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

bool CvPromotionInfo::isAidChange(int iProperty)
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

//Team Project (4)
	//WorkRateMod
int CvPromotionInfo::getNumTerrainWorkRateModifierChangeTypes() const
{
	return m_aTerrainWorkRateModifierChangeTypes.size();
}

int CvPromotionInfo::getTerrainWorkRateModifierChangeType(int iTerrain) const
{
	for (TerrainModifierArray::const_iterator it = m_aTerrainWorkRateModifierChangeTypes.begin(); it != m_aTerrainWorkRateModifierChangeTypes.end(); ++it)
	{
		if ((*it).first == (TerrainTypes)iTerrain)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isTerrainWorkRateModifierChangeType(int iTerrain)
{
	for (TerrainModifierArray::const_iterator it = m_aTerrainWorkRateModifierChangeTypes.begin(); it != m_aTerrainWorkRateModifierChangeTypes.end(); ++it)
	{
		if ((*it).first == (TerrainTypes)iTerrain)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumFeatureWorkRateModifierChangeTypes() const
{
	return m_aFeatureWorkRateModifierChangeTypes.size();
}

int CvPromotionInfo::getFeatureWorkRateModifierChangeType(int iFeature) const
{
	for (FeatureModifierArray::const_iterator it = m_aFeatureWorkRateModifierChangeTypes.begin(); it != m_aFeatureWorkRateModifierChangeTypes.end(); ++it)
	{
		if ((*it).first == (FeatureTypes)iFeature)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isFeatureWorkRateModifierChangeType(int iFeature)
{
	for (FeatureModifierArray::const_iterator it = m_aFeatureWorkRateModifierChangeTypes.begin(); it != m_aFeatureWorkRateModifierChangeTypes.end(); ++it)
	{
		if ((*it).first == (FeatureTypes)iFeature)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumBuildWorkRateModifierChangeTypes() const
{
	return m_aBuildWorkRateModifierChangeTypes.size();
}

int CvPromotionInfo::getBuildWorkRateModifierChangeType(int iBuild) const
{
	for (BuildModifierArray::const_iterator it = m_aBuildWorkRateModifierChangeTypes.begin(); it != m_aBuildWorkRateModifierChangeTypes.end(); ++it)
	{
		if ((*it).first == (BuildTypes)iBuild)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isBuildWorkRateModifierChangeType(int iBuild)
{
	for (BuildModifierArray::const_iterator it = m_aBuildWorkRateModifierChangeTypes.begin(); it != m_aBuildWorkRateModifierChangeTypes.end(); ++it)
	{
		if ((*it).first == (BuildTypes)iBuild)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumVisibilityIntensityChangeTypes() const
{
	return m_aVisibilityIntensityChangeTypes.size();
}

int CvPromotionInfo::getVisibilityIntensityChangeType(int iInvisibility) const
{
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensityChangeTypes.begin(); it != m_aVisibilityIntensityChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isVisibilityIntensityChangeType(int iInvisibility)
{
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensityChangeTypes.begin(); it != m_aVisibilityIntensityChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumInvisibilityIntensityChangeTypes() const
{
	return m_aInvisibilityIntensityChangeTypes.size();
}

int CvPromotionInfo::getInvisibilityIntensityChangeType(int iInvisibility) const
{
	for (InvisibilityArray::const_iterator it = m_aInvisibilityIntensityChangeTypes.begin(); it != m_aInvisibilityIntensityChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isInvisibilityIntensityChangeType(int iInvisibility)
{
	for (InvisibilityArray::const_iterator it = m_aInvisibilityIntensityChangeTypes.begin(); it != m_aInvisibilityIntensityChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionInfo::getNumVisibilityIntensityRangeChangeTypes() const
{
	return m_aVisibilityIntensityRangeChangeTypes.size();
}

int CvPromotionInfo::getVisibilityIntensityRangeChangeType(int iInvisibility) const
{
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensityRangeChangeTypes.begin(); it != m_aVisibilityIntensityRangeChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionInfo::isVisibilityIntensityRangeChangeType(int iInvisibility)
{
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensityRangeChangeTypes.begin(); it != m_aVisibilityIntensityRangeChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return true;
		}
	}
	return false;
}

// int vector utilizing struct with delayed resolution
int CvPromotionInfo::getNumAIWeightbyUnitCombatTypes() const
{
	return (int)m_aAIWeightbyUnitCombatTypes.size();
}

UnitCombatModifier& CvPromotionInfo::getAIWeightbyUnitCombatType(int iUnitCombat)
{
	FAssertMsg(iUnitCombat < (int)m_aAIWeightbyUnitCombatTypes.size(), "Index out of bounds");
	FAssertMsg(iUnitCombat > -1, "Index out of bounds");
	return m_aAIWeightbyUnitCombatTypes[iUnitCombat];
}

int CvPromotionInfo::getNumAfflictionFortitudeChangeModifiers() const
{
	return (int)m_aAfflictionFortitudeChangeModifiers.size();
}

PromotionLineModifier& CvPromotionInfo::getAfflictionFortitudeChangeModifier(int iAfflictionLine)
{
	FAssertMsg(iAfflictionLine < (int)m_aAfflictionFortitudeChangeModifiers.size(), "Index out of bounds");
	FAssertMsg(iAfflictionLine > -1, "Index out of bounds");
	return m_aAfflictionFortitudeChangeModifiers[iAfflictionLine];
}

int CvPromotionInfo::getNumAfflictOnAttackChangeTypes() const
{
	return (int)m_aAfflictOnAttackChangeTypes.size();
}

AfflictOnAttackChange& CvPromotionInfo::getAfflictOnAttackChangeType(int iAfflictionLine)
{
	FAssertMsg(iAfflictionLine < (int)m_aAfflictOnAttackChangeTypes.size(), "Index out of bounds");
	FAssertMsg(iAfflictionLine > -1, "Index out of bounds");
	return m_aAfflictOnAttackChangeTypes[iAfflictionLine];
}

int CvPromotionInfo::getNumInvisibleTerrainChanges() const
{
	return (int)m_aInvisibleTerrainChanges.size();
}

InvisibleTerrainChanges& CvPromotionInfo::getInvisibleTerrainChange(int iIndex)
{
	return m_aInvisibleTerrainChanges[iIndex];
}

int CvPromotionInfo::getNumInvisibleFeatureChanges() const
{
	return (int)m_aInvisibleFeatureChanges.size();
}

InvisibleFeatureChanges& CvPromotionInfo::getInvisibleFeatureChange(int iIndex)
{
	return m_aInvisibleFeatureChanges[iIndex];
}

int CvPromotionInfo::getNumInvisibleImprovementChanges() const
{
	return (int)m_aInvisibleImprovementChanges.size();
}

InvisibleImprovementChanges& CvPromotionInfo::getInvisibleImprovementChange(int iIndex)
{
	return m_aInvisibleImprovementChanges[iIndex];
}

int CvPromotionInfo::getNumVisibleTerrainChanges() const
{
	return (int)m_aVisibleTerrainChanges.size();
}

InvisibleTerrainChanges& CvPromotionInfo::getVisibleTerrainChange(int iIndex)
{
	return m_aVisibleTerrainChanges[iIndex];
}

int CvPromotionInfo::getNumVisibleFeatureChanges() const
{
	return (int)m_aVisibleFeatureChanges.size();
}

InvisibleFeatureChanges& CvPromotionInfo::getVisibleFeatureChange(int iIndex)
{
	return m_aVisibleFeatureChanges[iIndex];
}

int CvPromotionInfo::getNumVisibleImprovementChanges() const
{
	return (int)m_aVisibleImprovementChanges.size();
}

InvisibleImprovementChanges& CvPromotionInfo::getVisibleImprovementChange(int iIndex)
{
	return m_aVisibleImprovementChanges[iIndex];
}

int CvPromotionInfo::getNumVisibleTerrainRangeChanges() const
{
	return (int)m_aVisibleTerrainRangeChanges.size();
}

InvisibleTerrainChanges& CvPromotionInfo::getVisibleTerrainRangeChange(int iIndex)
{
	return m_aVisibleTerrainRangeChanges[iIndex];
}

int CvPromotionInfo::getNumVisibleFeatureRangeChanges() const
{
	return (int)m_aVisibleFeatureRangeChanges.size();
}

InvisibleFeatureChanges& CvPromotionInfo::getVisibleFeatureRangeChange(int iIndex)
{
	return m_aVisibleFeatureRangeChanges[iIndex];
}

int CvPromotionInfo::getNumVisibleImprovementRangeChanges() const
{
	return (int)m_aVisibleImprovementRangeChanges.size();
}

InvisibleImprovementChanges& CvPromotionInfo::getVisibleImprovementRangeChange(int iIndex)
{
	return m_aVisibleImprovementRangeChanges[iIndex];
}

int CvPromotionInfo::getNumDistanceAttackCommunicabilityTypeChanges() const
{
	return (int)m_aDistanceAttackCommunicabilityTypeChanges.size();
}

AfflictionLineChanges& CvPromotionInfo::getDistanceAttackCommunicabilityTypeChange(int iIndex)
{
	return m_aDistanceAttackCommunicabilityTypeChanges[iIndex];
}

int CvPromotionInfo::getNumHealUnitCombatChangeTypes() const
{
	return (int)m_aHealUnitCombatChangeTypes.size();
}

HealUnitCombat& CvPromotionInfo::getHealUnitCombatChangeType(int iUnitCombat)
{
	FAssertMsg(iUnitCombat < (int)m_aHealUnitCombatChangeTypes.size(), "Index out of bounds");
	FAssertMsg(iUnitCombat > -1, "Index out of bounds");
	return m_aHealUnitCombatChangeTypes[iUnitCombat];
}

int CvPromotionInfo::getQualifiedUnitCombatType(int i) const
{
	return m_aiQualifiedUnitCombatTypes[i];
}

int CvPromotionInfo::getNumQualifiedUnitCombatTypes() const
{
	return (int)m_aiQualifiedUnitCombatTypes.size();
}

bool CvPromotionInfo::isQualifiedUnitCombatType(int i)
{
	FAssert (i > -1 && i < GC.getNumUnitCombatInfos());
	if (find(m_aiQualifiedUnitCombatTypes.begin(), m_aiQualifiedUnitCombatTypes.end(), i) == m_aiQualifiedUnitCombatTypes.end())
	{
		return false;
	}
	return true;
}

void CvPromotionInfo::setQualifiedUnitCombatTypes()
{
	m_aiQualifiedUnitCombatTypes.clear();
	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		if (getUnitCombat(iI))
		{
			m_aiQualifiedUnitCombatTypes.push_back(iI);
		}
	}
	for (int iI = 0; iI < GC.getNumPromotionLineInfos(); iI++)
	{
		if (getPromotionLine() == (PromotionLineTypes)iI)
		{
			PromotionLineTypes ePromotionLine = (PromotionLineTypes)iI;
			for (int iJ = 0; iJ < GC.getPromotionLineInfo(ePromotionLine).getNumUnitCombatPrereqTypes(); iJ++)
			{
				int iUnitCombat = GC.getPromotionLineInfo(ePromotionLine).getUnitCombatPrereqType(iJ);
				if (!isQualifiedUnitCombatType(iUnitCombat))
				{
					m_aiQualifiedUnitCombatTypes.push_back(iUnitCombat);
				}
			}
		}
	}
}
//TB Combat Mods End  TB SubCombat Mod end

bool CvPromotionInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();
	OutputDebugString("Reading PromotionInfo");

	CvString szTextVal;
	CvString szTextVal2;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Sound");
	setSound(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"LayerAnimationPath");
	m_iLayerAnimationPath = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TechPrereq");
	m_iTechPrereq = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"StateReligionPrereq");
	m_iStateReligionPrereq = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"MinEraType");
	m_iMinEraType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"MaxEraType");
	m_iMaxEraType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_bLeader, L"bLeader");
	if (m_bLeader)
	{
		m_bGraphicalOnly = true;  // don't show in Civilopedia list of promotions
	}
	pXML->GetOptionalChildXmlValByName(&m_bBlitz, L"bBlitz");
	pXML->GetOptionalChildXmlValByName(&m_bAmphib, L"bAmphib");
	pXML->GetOptionalChildXmlValByName(&m_bRiver, L"bRiver");
	pXML->GetOptionalChildXmlValByName(&m_bEnemyRoute, L"bEnemyRoute");
	pXML->GetOptionalChildXmlValByName(&m_bAlwaysHeal, L"bAlwaysHeal");
	pXML->GetOptionalChildXmlValByName(&m_bHillsDoubleMove, L"bHillsDoubleMove");
	pXML->GetOptionalChildXmlValByName(&m_bImmuneToFirstStrikes, L"bImmuneToFirstStrikes");
	pXML->GetOptionalChildXmlValByName(&m_iVisibilityChange, L"iVisibilityChange");
	pXML->GetOptionalChildXmlValByName(&m_iMovesChange, L"iMovesChange");
	pXML->GetOptionalChildXmlValByName(&m_iMoveDiscountChange, L"iMoveDiscountChange");
	pXML->GetOptionalChildXmlValByName(&m_iAirRangeChange, L"iAirRangeChange");
	pXML->GetOptionalChildXmlValByName(&m_iInterceptChange, L"iInterceptChange");
	pXML->GetOptionalChildXmlValByName(&m_iEvasionChange, L"iEvasionChange");
	pXML->GetOptionalChildXmlValByName(&m_iWithdrawalChange, L"iWithdrawalChange");
	pXML->GetOptionalChildXmlValByName(&m_iCargoChange, L"iCargoChange");
	pXML->GetOptionalChildXmlValByName(&m_iSMCargoChange, L"iSMCargoChange");
	pXML->GetOptionalChildXmlValByName(&m_iSMCargoVolumeChange, L"iSMCargoVolumeChange");
	pXML->GetOptionalChildXmlValByName(&m_iSMCargoVolumeModifierChange, L"iSMCargoVolumeModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iCollateralDamageChange, L"iCollateralDamageChange");
	pXML->GetOptionalChildXmlValByName(&m_iBombardRateChange, L"iBombardRateChange");
	pXML->GetOptionalChildXmlValByName(&m_iFirstStrikesChange, L"iFirstStrikesChange");
	pXML->GetOptionalChildXmlValByName(&m_iChanceFirstStrikesChange, L"iChanceFirstStrikesChange");
	pXML->GetOptionalChildXmlValByName(&m_iEnemyHealChange, L"iEnemyHealChange");
	pXML->GetOptionalChildXmlValByName(&m_iNeutralHealChange, L"iNeutralHealChange");
	pXML->GetOptionalChildXmlValByName(&m_iFriendlyHealChange, L"iFriendlyHealChange");
	pXML->GetOptionalChildXmlValByName(&m_iSameTileHealChange, L"iSameTileHealChange");
	pXML->GetOptionalChildXmlValByName(&m_iAdjacentTileHealChange, L"iAdjacentTileHealChange");
	pXML->GetOptionalChildXmlValByName(&m_iCombatPercent, L"iCombatPercent");
	pXML->GetOptionalChildXmlValByName(&m_iCityAttackPercent, L"iCityAttack");
	pXML->GetOptionalChildXmlValByName(&m_iCityDefensePercent, L"iCityDefense");
	pXML->GetOptionalChildXmlValByName(&m_iHillsAttackPercent, L"iHillsAttack");
	pXML->GetOptionalChildXmlValByName(&m_iHillsDefensePercent, L"iHillsDefense");
	pXML->GetOptionalChildXmlValByName(&m_iHillsWorkPercent, L"iHillsWork");
	//ls612: Work rate modifiers
	pXML->GetOptionalChildXmlValByName(&m_iWorkRatePercent, L"iWorkRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iRevoltProtection, L"iRevoltProtection");
	pXML->GetOptionalChildXmlValByName(&m_iCollateralDamageProtection, L"iCollateralDamageProtection");
	pXML->GetOptionalChildXmlValByName(&m_iPillageChange, L"iPillageChange");
	pXML->GetOptionalChildXmlValByName(&m_iUpgradeDiscount, L"iUpgradeDiscount");
	pXML->GetOptionalChildXmlValByName(&m_iExperiencePercent, L"iExperiencePercent");
	pXML->GetOptionalChildXmlValByName(&m_iKamikazePercent, L"iKamikazePercent");
/*****************************************************************************************************/
/**  Author: TheLadiesOgre																		  **/
/**  Date: 16.09.2009																			   **/
/**  ModComp: TLOTags																			   **/
/**  Reason Added: New Tag Definition															   **/
/**  Notes:																						 **/
/*****************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_bDefensiveVictoryMove, L"bDefensiveVictoryMove");
	pXML->GetOptionalChildXmlValByName(&m_bFreeDrop, L"bFreeDrop");
	pXML->GetOptionalChildXmlValByName(&m_bOffensiveVictoryMove, L"bOffensiveVictoryMove");
//Team Project (2)
	pXML->GetOptionalChildXmlValByName(&m_bOneUp, L"bOneUp");
	pXML->GetOptionalChildXmlValByName(&m_bPillageCulture, L"bPillageCulture");
	pXML->GetOptionalChildXmlValByName(&m_bPillageEspionage, L"bPillageEspionage");
	pXML->GetOptionalChildXmlValByName(&m_bPillageMarauder, L"bPillageMarauder");
	pXML->GetOptionalChildXmlValByName(&m_bPillageOnMove, L"bPillageOnMove");
	pXML->GetOptionalChildXmlValByName(&m_bPillageOnVictory, L"bPillageOnVictory");
	pXML->GetOptionalChildXmlValByName(&m_bPillageResearch, L"bPillageResearch");
	pXML->GetOptionalChildXmlValByName(&m_iAirCombatLimitChange, L"iAirCombatLimitChange");
	pXML->GetOptionalChildXmlValByName(&m_iCelebrityHappy, L"iCelebrityHappy");
	pXML->GetOptionalChildXmlValByName(&m_iCollateralDamageLimitChange, L"iCollateralDamageLimitChange");
	pXML->GetOptionalChildXmlValByName(&m_iCollateralDamageMaxUnitsChange, L"iCollateralDamageMaxUnitsChange");
	pXML->GetOptionalChildXmlValByName(&m_iCombatLimitChange, L"iCombatLimitChange");
	pXML->GetOptionalChildXmlValByName(&m_iExtraDropRange, L"iExtraDropRange");
//Team Project (2)
	pXML->GetOptionalChildXmlValByName(&m_iSurvivorChance, L"iSurvivorChance");
	pXML->GetOptionalChildXmlValByName(&m_iVictoryAdjacentHeal, L"iVictoryAdjacentHeal");
	pXML->GetOptionalChildXmlValByName(&m_iVictoryHeal, L"iVictoryHeal");
//Team Project (2)
	pXML->GetOptionalChildXmlValByName(&m_iVictoryStackHeal, L"iVictoryStackHeal");
/*****************************************************************************************************/
/**  TheLadiesOgre; 16.09.2009; TLOTags															 **/
/*****************************************************************************************************/

	pXML->SetVariableListTagPair(&m_piTerrainAttackPercent, L"TerrainAttacks", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_piTerrainDefensePercent, L"TerrainDefenses", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_piFeatureAttackPercent, L"FeatureAttacks", GC.getNumFeatureInfos());
	pXML->SetVariableListTagPair(&m_piFeatureDefensePercent, L"FeatureDefenses", GC.getNumFeatureInfos());
	pXML->SetVariableListTagPair(&m_piUnitCombatModifierPercent, L"UnitCombatMods", GC.getNumUnitCombatInfos());
	pXML->SetVariableListTagPair(&m_piDomainModifierPercent, L"DomainMods", NUM_DOMAIN_TYPES);

	pXML->SetVariableListTagPair(&m_pbTerrainDoubleMove, L"TerrainDoubleMoves", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_pbFeatureDoubleMove, L"FeatureDoubleMoves", GC.getNumFeatureInfos());
	pXML->SetVariableListTagPair(&m_pbUnitCombat, L"UnitCombats", GC.getNumUnitCombatInfos());
	//ls612: Terrain Work Modifiers
	pXML->SetVariableListTagPair(&m_piTerrainWorkPercent, L"TerrainWorks", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_piFeatureWorkPercent, L"FeatureWorks", GC.getNumFeatureInfos());

/************************************************************************************************/
/* Afforess					  Start		 Last Update: 3/8/10									*/
/*																							  */
/*																							  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_bCanMovePeaks, L"bCanMovePeaks");
	//	Koshling - enhanced mountaineering mode to differentiate between ability to move through
	//	mountains, and ability to lead a stack through mountains
	pXML->GetOptionalChildXmlValByName(&m_bCanLeadThroughPeaks, L"bCanLeadThroughPeaks");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"ObsoleteTech");
	m_iObsoleteTech = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iControlPoints, L"iControlPoints");
	pXML->GetOptionalChildXmlValByName(&m_iCommandRange, L"iCommandRange");
	pXML->GetOptionalChildXmlValByName(&m_bZoneOfControl, L"bZoneOfControl");
	pXML->GetOptionalChildXmlValByName(&m_iAssetMultiplier, L"iAssetMultiplierPercent");
	pXML->GetOptionalChildXmlValByName(&m_iPowerMultiplier, L"iPowerMultiplierPercent");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"IgnoreTerrainDamage");
	m_iIgnoreTerrainDamage = pXML->GetInfoClass(szTextVal);
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	m_PropertyManipulators.read(pXML);

	//TB Combat Mods Begin  TB SubCombat Mod begin
	//Text Strings
	pXML->GetOptionalChildXmlValByName(m_szRenamesUnitTo, L"RenamesUnitTo");

	//Textual References
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PromotionLine");
	m_ePromotionLine = (PromotionLineTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ReplacesUnitCombat");
	m_eReplacesUnitCombat = (UnitCombatTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"DomainCargoChange");
	m_eDomainCargoChange = (DomainTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SpecialCargoChange");
	m_eSpecialCargoChange = (SpecialUnitTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SpecialCargoPrereq");
	m_eSpecialCargoPrereq = (SpecialUnitTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SMSpecialCargoChange");
	m_eSMSpecialCargoChange = (SpecialUnitTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SMSpecialCargoPrereq");
	m_eSMSpecialCargoPrereq = (SpecialUnitTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SMNotSpecialCargoChange");
	m_eSMNotSpecialCargoChange = (SpecialUnitTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SMNotSpecialCargoPrereq");
	m_eSMNotSpecialCargoPrereq = (SpecialUnitTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SetSpecialUnit");
	m_eSetSpecialUnit = (SpecialUnitTypes) pXML->GetInfoClass(szTextVal);
	//integers
	pXML->GetOptionalChildXmlValByName(&m_iAttackCombatModifierChange, L"iAttackCombatModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iDefenseCombatModifierChange, L"iDefenseCombatModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iPursuitChange, L"iPursuitChange");
	pXML->GetOptionalChildXmlValByName(&m_iEarlyWithdrawChange, L"iEarlyWithdrawChange");
	pXML->GetOptionalChildXmlValByName(&m_iVSBarbsChange, L"iVSBarbsChange");
	pXML->GetOptionalChildXmlValByName(&m_iArmorChange, L"iArmorChange");
	pXML->GetOptionalChildXmlValByName(&m_iPunctureChange, L"iPunctureChange");
	pXML->GetOptionalChildXmlValByName(&m_iOverrunChange, L"iOverrunChange");
	pXML->GetOptionalChildXmlValByName(&m_iRepelChange, L"iRepelChange");
	pXML->GetOptionalChildXmlValByName(&m_iFortRepelChange, L"iFortRepelChange");
	pXML->GetOptionalChildXmlValByName(&m_iRepelRetriesChange, L"iRepelRetriesChange");
	pXML->GetOptionalChildXmlValByName(&m_iUnyieldingChange, L"iUnyieldingChange");
	pXML->GetOptionalChildXmlValByName(&m_iKnockbackChange, L"iKnockbackChange");
	pXML->GetOptionalChildXmlValByName(&m_iKnockbackRetriesChange, L"iKnockbackRetriesChange");
	pXML->GetOptionalChildXmlValByName(&m_iStrAdjperRndChange, L"iStrAdjperRndChange");
	pXML->GetOptionalChildXmlValByName(&m_iStrAdjperAttChange, L"iStrAdjperAttChange");
	pXML->GetOptionalChildXmlValByName(&m_iStrAdjperDefChange, L"iStrAdjperDefChange");
	pXML->GetOptionalChildXmlValByName(&m_iWithdrawAdjperAttChange, L"iWithdrawAdjperAttChange");
	pXML->GetOptionalChildXmlValByName(&m_iUnnerveChange, L"iUnnerveChange");
	pXML->GetOptionalChildXmlValByName(&m_iEncloseChange, L"iEncloseChange");
	pXML->GetOptionalChildXmlValByName(&m_iLungeChange, L"iLungeChange");
	pXML->GetOptionalChildXmlValByName(&m_iDynamicDefenseChange, L"iDynamicDefenseChange");
	pXML->GetOptionalChildXmlValByName(&m_iStrengthChange, L"iStrengthChange");
	pXML->GetOptionalChildXmlValByName(&m_iLinePriority, L"iLinePriority");
	pXML->GetOptionalChildXmlValByName(&m_iFortitudeChange, L"iFortitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iDamageperTurn, L"iDamageperTurn");
	pXML->GetOptionalChildXmlValByName(&m_iStrAdjperTurn, L"iStrAdjperTurn");
	pXML->GetOptionalChildXmlValByName(&m_iWeakenperTurn, L"iWeakenperTurn");
	pXML->GetOptionalChildXmlValByName(&m_iFrontSupportPercentChange, L"iFrontSupportPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iShortRangeSupportPercentChange, L"iShortRangeSupportPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iMediumRangeSupportPercentChange, L"iMediumRangeSupportPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iLongRangeSupportPercentChange, L"iLongRangeSupportPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iFlankSupportPercentChange, L"iFlankSupportPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iDodgeModifierChange, L"iDodgeModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iPrecisionModifierChange, L"iPrecisionModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iPowerShotsChange, L"iPowerShotsChange");
	pXML->GetOptionalChildXmlValByName(&m_iPowerShotCombatModifierChange, L"iPowerShotCombatModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iPowerShotPunctureModifierChange, L"iPowerShotPunctureModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iPowerShotPrecisionModifierChange, L"iPowerShotPrecisionModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iPowerShotCriticalModifierChange, L"iPowerShotCriticalModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iCriticalModifierChange, L"iCriticalModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iEnduranceChange, L"iEnduranceChange");
	pXML->GetOptionalChildXmlValByName(&m_iRoundStunProbChange, L"iRoundStunProbChange");
	pXML->GetOptionalChildXmlValByName(&m_iPoisonProbabilityModifierChange, L"iPoisonProbabilityModifierChange");
//Team Project (3)
	pXML->GetOptionalChildXmlValByName(&m_iCaptureProbabilityModifierChange, L"iCaptureProbabilityModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iCaptureResistanceModifierChange, L"iCaptureResistanceModifierChange");
//Team Project (4)
//WorkRateMod
	pXML->GetOptionalChildXmlValByName(&m_iHillsWorkModifierChange, L"iHillsWorkModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iPeaksWorkModifierChange, L"iPeaksWorkModifierChange");

	pXML->GetOptionalChildXmlValByName(&m_iBreakdownChanceChange, L"iBreakdownChanceChange");
	pXML->GetOptionalChildXmlValByName(&m_iBreakdownDamageChange, L"iBreakdownDamageChange");
	pXML->GetOptionalChildXmlValByName(&m_iTauntChange, L"iTauntChange");
	pXML->GetOptionalChildXmlValByName(&m_iMaxHPChange, L"iMaxHPChange");
	pXML->GetOptionalChildXmlValByName(&m_iStrengthModifier, L"iStrengthModifier");
	pXML->GetOptionalChildXmlValByName(&m_iQualityChange, L"iQualityChange");
	pXML->GetOptionalChildXmlValByName(&m_iGroupChange, L"iGroupChange");
	pXML->GetOptionalChildXmlValByName(&m_iLevelPrereq, L"iLevelPrereq");
	pXML->GetOptionalChildXmlValByName(&m_iDamageModifierChange, L"iDamageModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iCostModifierChange, L"iCostModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iRBombardDamageChange, L"iRBombardDamageChange");
	pXML->GetOptionalChildXmlValByName(&m_iRBombardDamageLimitChange, L"iRBombardDamageLimitChange");
	pXML->GetOptionalChildXmlValByName(&m_iRBombardDamageMaxUnitsChange, L"iRBombardDamageMaxUnitsChange");
	pXML->GetOptionalChildXmlValByName(&m_iDCMBombRangeChange, L"iDCMBombRangeChange");
	pXML->GetOptionalChildXmlValByName(&m_iDCMBombAccuracyChange, L"iDCMBombAccuracyChange");
	pXML->GetOptionalChildXmlValByName(&m_iCombatModifierPerSizeMoreChange, L"iCombatModifierPerSizeMoreChange");
	pXML->GetOptionalChildXmlValByName(&m_iCombatModifierPerSizeLessChange, L"iCombatModifierPerSizeLessChange");
	pXML->GetOptionalChildXmlValByName(&m_iCombatModifierPerVolumeMoreChange, L"iCombatModifierPerVolumeMoreChange");
	pXML->GetOptionalChildXmlValByName(&m_iCombatModifierPerVolumeLessChange, L"iCombatModifierPerVolumeLessChange");
	pXML->GetOptionalChildXmlValByName(&m_iSelfHealModifier, L"iSelfHealModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNumHealSupport, L"iNumHealSupport");
	pXML->GetOptionalChildXmlValByName(&m_iExcileChange, L"iExcileChange");
	pXML->GetOptionalChildXmlValByName(&m_iPassageChange, L"iPassageChange");
	pXML->GetOptionalChildXmlValByName(&m_iNoNonOwnedCityEntryChange, L"iNoNonOwnedCityEntryChange");
	pXML->GetOptionalChildXmlValByName(&m_iBarbCoExistChange, L"iBarbCoExistChange");
	pXML->GetOptionalChildXmlValByName(&m_iBlendIntoCityChange, L"iBlendIntoCityChange");
	pXML->GetOptionalChildXmlValByName(&m_iUpgradeAnywhereChange, L"iUpgradeAnywhereChange");
	pXML->GetOptionalChildXmlValByName(&m_iInsidiousnessChange, L"iInsidiousnessChange");
	pXML->GetOptionalChildXmlValByName(&m_iInvestigationChange, L"iInvestigationChange");
	pXML->GetOptionalChildXmlValByName(&m_iAssassinChange, L"iAssassinChange");
	pXML->GetOptionalChildXmlValByName(&m_iStealthStrikesChange, L"iStealthStrikesChange");
	pXML->GetOptionalChildXmlValByName(&m_iStealthCombatModifierChange, L"iStealthCombatModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iStealthDefenseChange, L"iStealthDefenseChange");
	pXML->GetOptionalChildXmlValByName(&m_iDefenseOnlyChange, L"iDefenseOnlyChange");
	pXML->GetOptionalChildXmlValByName(&m_iNoInvisibilityChange, L"iNoInvisibilityChange");
	pXML->GetOptionalChildXmlValByName(&m_iTrapDamageMax, L"iTrapDamageMax");
	pXML->GetOptionalChildXmlValByName(&m_iTrapDamageMin, L"iTrapDamageMin");
	pXML->GetOptionalChildXmlValByName(&m_iTrapComplexity, L"iTrapComplexity");
	pXML->GetOptionalChildXmlValByName(&m_iNumTriggers, L"iNumTriggers");
	pXML->GetOptionalChildXmlValByName(&m_iTriggerBeforeAttackChange, L"iTriggerBeforeAttackChange");
	pXML->GetOptionalChildXmlValByName(&m_iHiddenNationalityChange, L"iHiddenNationalityChange");
	pXML->GetOptionalChildXmlValByName(&m_iAnimalIgnoresBordersChange, L"iAnimalIgnoresBordersChange");
	pXML->GetOptionalChildXmlValByName(&m_iNoDefensiveBonusChange, L"iNoDefensiveBonusChange");
	pXML->GetOptionalChildXmlValByName(&m_iGatherHerdChange, L"iGatherHerdChange");
	pXML->GetOptionalChildXmlValByName(&m_iReligiousCombatModifierChange, L"iReligiousCombatModifierChange");
	//booleans
	pXML->GetOptionalChildXmlValByName(&m_bStampedeChange, L"bStampedeChange");
	pXML->GetOptionalChildXmlValByName(&m_bRemoveStampede, L"bRemoveStampede");
	pXML->GetOptionalChildXmlValByName(&m_bOnslaughtChange, L"bOnslaughtChange");
	pXML->GetOptionalChildXmlValByName(&m_bEquipment, L"bEquipment");
	pXML->GetOptionalChildXmlValByName(&m_bAffliction, L"bAffliction");
	pXML->GetOptionalChildXmlValByName(&m_bParalyze, L"bParalyze");
	pXML->GetOptionalChildXmlValByName(&m_bMakesDamageCold, L"bMakesDamageCold");
	pXML->GetOptionalChildXmlValByName(&m_bMakesDamageNotCold, L"bMakesDamageNotCold");
	pXML->GetOptionalChildXmlValByName(&m_bAddsColdImmunity, L"bAddsColdImmunity");
	pXML->GetOptionalChildXmlValByName(&m_bRemovesColdImmunity, L"bRemovesColdImmunity");
	pXML->GetOptionalChildXmlValByName(&m_bCritical, L"bCritical");
	pXML->GetOptionalChildXmlValByName(&m_bAttackOnlyCitiesAdd, L"bAttackOnlyCitiesAdd");
	pXML->GetOptionalChildXmlValByName(&m_bAttackOnlyCitiesSubtract, L"bAttackOnlyCitiesSubtract");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreNoEntryLevelAdd, L"bIgnoreNoEntryLevelAdd");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreNoEntryLevelSubtract, L"bIgnoreNoEntryLevelSubtract");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreZoneofControlAdd, L"bIgnoreZoneofControlAdd");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreZoneofControlSubtract, L"bIgnoreZoneofControlSubtract");
	pXML->GetOptionalChildXmlValByName(&m_bFliesToMoveAdd, L"bFliesToMoveAdd");
	pXML->GetOptionalChildXmlValByName(&m_bFliesToMoveSubtract, L"bFliesToMoveSubtract");
	pXML->GetOptionalChildXmlValByName(&m_bZeroesXP, L"bZeroesXP");
	pXML->GetOptionalChildXmlValByName(&m_bForOffset, L"bForOffset");
	pXML->GetOptionalChildXmlValByName(&m_bCargoPrereq, L"bCargoPrereq");
	pXML->GetOptionalChildXmlValByName(&m_bRBombardPrereq, L"bRBombardPrereq");
	pXML->GetOptionalChildXmlValByName(&m_bNoSelfHeal, L"bNoSelfHeal");
	pXML->GetOptionalChildXmlValByName(&m_bSetOnHNCapture, L"bSetOnHNCapture");
	pXML->GetOptionalChildXmlValByName(&m_bSetOnInvestigated, L"bSetOnInvestigated");
	pXML->GetOptionalChildXmlValByName(&m_bStatus, L"bStatus");
	pXML->GetOptionalChildXmlValByName(&m_bPrereqNormInvisible, L"bPrereqNormInvisible");
	pXML->GetOptionalChildXmlValByName(&m_bPlotPrereqsKeepAfter, L"bPlotPrereqsKeepAfter");
	pXML->GetOptionalChildXmlValByName(&m_bRemoveAfterSet, L"bRemoveAfterSet");
	pXML->GetOptionalChildXmlValByName(&m_bQuick, L"bQuick");

	//Arrays
	//pXML->SetVariableListTagPair(&m_piAIWeightbyUnitCombatTypes, L"AIWeightbyUnitCombatTypes", GC.getNumUnitCombatInfos());
	// bool vector without delayed resolution
	CvString* pszTemp = NULL;
	pXML->SetOptionalIntVector(&m_aiSubCombatChangeTypes, L"SubCombatChangeTypes");
	pXML->SetOptionalIntVector(&m_aiRemovesUnitCombatTypes, L"RemovesUnitCombatTypes");
	pXML->SetOptionalIntVector(&m_aiOnGameOptions, L"OnGameOptions");
	pXML->SetOptionalIntVector(&m_aiNotOnGameOptions, L"NotOnGameOptions");
	pXML->SetOptionalIntVector(&m_aiFreetoUnitCombats, L"FreetoUnitCombats");
	pXML->SetOptionalIntVector(&m_aiNotOnUnitCombatTypes, L"NotOnUnitCombatTypes");
	pXML->SetOptionalIntVector(&m_aiNotOnDomainTypes, L"NotOnDomainTypes");
	pXML->SetOptionalIntVector(&m_aiNoAutoEquiptoCombatClassTypes, L"NoAutoEquiptoCombatClassTypes");
	pXML->SetOptionalIntVector(&m_aiMapCategoryTypes, L"MapCategoryTypes");

	// bool vector with delayed resolution
	pXML->SetOptionalIntVector(&m_aiCureAfflictionChangeTypes, L"CureAfflictionChangeTypes");
	pXML->SetOptionalIntVector(&m_aiPrereqBonusTypes, L"PrereqBonusTypes");
	pXML->SetOptionalIntVectorWithDelayedResolution(m_aiAddsBuildTypes, L"AddsBuildTypes");
	pXML->SetOptionalIntVector(&m_aiNegatesInvisibilityTypes, L"NegatesInvisibilityTypes");
	pXML->SetOptionalIntVector(&m_aiPrereqTerrainTypes, L"PrereqTerrainTypes");
	pXML->SetOptionalIntVector(&m_aiPrereqFeatureTypes, L"PrereqFeatureTypes");
	pXML->SetOptionalIntVector(&m_aiPrereqImprovementTypes, L"PrereqImprovementTypes");
	pXML->SetOptionalIntVector(&m_aiPrereqPlotBonusTypes, L"PrereqPlotBonusTypes");
	pXML->SetOptionalIntVectorWithDelayedResolution(m_aiPrereqLocalBuildingClassTypes, L"PrereqLocalBuildingClassTypes");
	pXML->SetOptionalIntVectorWithDelayedResolution(m_aiTrapSetWithPromotionTypes, L"TrapSetWithPromotionTypes");
	pXML->SetOptionalIntVector(&m_aiTrapImmunityUnitCombatTypes, L"TrapImmunityUnitCombatTypes");
	pXML->SetOptionalIntVector(&m_aiTargetUnitCombatTypes, L"TargetUnitCombatTypes");

	// int vector utilizing pairing without delayed resolution
	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aFlankingStrengthbyUnitCombatTypeChange, L"FlankingStrikesbyUnitCombatChange");

	pXML->SetOptionalPairVector<TerrainModifierArray, TerrainTypes, int>(&m_aWithdrawOnTerrainTypesChange, L"WithdrawModifierOnTerrainTypeChanges");

	pXML->SetOptionalPairVector<FeatureModifierArray, FeatureTypes, int>(&m_aWithdrawOnFeatureTypesChange, L"WithdrawModifierOnFeatureTypeChanges");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aWithdrawVSUnitCombatChangeTypes, L"WithdrawVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aPursuitVSUnitCombatChangeTypes, L"PursuitVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aRepelVSUnitCombatChangeTypes, L"RepelVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aKnockbackVSUnitCombatChangeTypes, L"KnockbackVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aPunctureVSUnitCombatChangeTypes, L"PunctureVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aArmorVSUnitCombatChangeTypes, L"ArmorVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aDodgeVSUnitCombatChangeTypes, L"DodgeVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aPrecisionVSUnitCombatChangeTypes, L"PrecisionVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aCriticalVSUnitCombatChangeTypes, L"CriticalVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aRoundStunVSUnitCombatChangeTypes, L"RoundStunVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aTrapDisableUnitCombatTypes, L"TrapDisableUnitCombatTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aTrapAvoidanceUnitCombatTypes, L"TrapAvoidanceUnitCombatTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aTrapTriggerUnitCombatTypes, L"TrapTriggerUnitCombatTypes");

	pXML->SetOptionalPairVector<AidArray, PropertyTypes, int>(&m_aAidChanges, L"AidChanges");

	//pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(m_aAIWeightbyUnitCombatTypes, L"AIWeightbyUnitCombatTypes");

//Team Project (4)
		//WorkRateMod
	pXML->SetOptionalPairVector<TerrainModifierArray, TerrainTypes, int>(&m_aTerrainWorkRateModifierChangeTypes, L"TerrainWorkRateModifierChangeTypes");

	pXML->SetOptionalPairVector<FeatureModifierArray, FeatureTypes, int>(&m_aFeatureWorkRateModifierChangeTypes, L"FeatureWorkRateModifierChangeTypes");

	pXML->SetOptionalPairVector<BuildModifierArray, BuildTypes, int>(&m_aBuildWorkRateModifierChangeTypes, L"BuildWorkRateModifierChangeTypes");

	pXML->SetOptionalPairVector<InvisibilityArray, InvisibleTypes, int>(&m_aVisibilityIntensityChangeTypes, L"VisibilityIntensityChangeTypes");

	pXML->SetOptionalPairVector<InvisibilityArray, InvisibleTypes, int>(&m_aInvisibilityIntensityChangeTypes, L"InvisibilityIntensityChangeTypes");

	pXML->SetOptionalPairVector<InvisibilityArray, InvisibleTypes, int>(&m_aVisibilityIntensityRangeChangeTypes, L"VisibilityIntensityRangeChangeTypes");

	// int vector utilizing struct with delayed resolution
	if(pXML->TryMoveToXmlFirstChild(L"AIWeightbyUnitCombatTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"AIWeightbyUnitCombatType" );
		m_aAIWeightbyUnitCombatTypes.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"AIWeightbyUnitCombatType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					m_aAIWeightbyUnitCombatTypes[i].eUnitCombat = (UnitCombatTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aAIWeightbyUnitCombatTypes[i].iModifier), L"iAIWeight");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"AIWeightbyUnitCombatType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"AfflictionFortitudeChangeModifiers"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"AfflictionFortitudeChangeModifier" );
		m_aAfflictionFortitudeChangeModifiers.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"AfflictionFortitudeChangeModifier"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"PromotionLineType");
					m_aAfflictionFortitudeChangeModifiers[i].ePromotionLine = (PromotionLineTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aAfflictionFortitudeChangeModifiers[i].iModifier), L"iModifier");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"AfflictionFortitudeChangeModifier"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"AfflictOnAttackChangeTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"AfflictOnAttackChangeType" );
		m_aAfflictOnAttackChangeTypes.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"AfflictOnAttackChangeType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"PromotionLineType");
					m_aAfflictOnAttackChangeTypes[i].eAfflictionLine = (PromotionLineTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aAfflictOnAttackChangeTypes[i].iProbabilityChange), L"iProbability");
					pXML->GetChildXmlValByName(&(m_aAfflictOnAttackChangeTypes[i].iMelee), L"iMelee");
					pXML->GetChildXmlValByName(&(m_aAfflictOnAttackChangeTypes[i].iDistance), L"iDistance");
					pXML->GetChildXmlValByName(&(m_aAfflictOnAttackChangeTypes[i].iImmediate), L"iImmediate");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"AfflictOnAttackChangeType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"HealUnitCombatChangeTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"HealUnitCombatChangeType" );
		m_aHealUnitCombatChangeTypes.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"HealUnitCombatChangeType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					m_aHealUnitCombatChangeTypes[i].eUnitCombat = (UnitCombatTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aHealUnitCombatChangeTypes[i].iHeal), L"iHeal");
					pXML->GetChildXmlValByName(&(m_aHealUnitCombatChangeTypes[i].iAdjacentHeal), L"iAdjacentHeal");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"HealUnitCombatChangeType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"InvisibleTerrainChanges"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"InvisibleTerrainChange" );
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
		int iNum = pXML->GetXmlChildrenNumber(L"InvisibleFeatureChange" );
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
		int iNum = pXML->GetXmlChildrenNumber(L"InvisibleImprovementChange" );
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
		int iNum = pXML->GetXmlChildrenNumber(L"VisibleTerrainChange" );
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
		int iNum = pXML->GetXmlChildrenNumber(L"VisibleFeatureChange" );
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
		int iNum = pXML->GetXmlChildrenNumber(L"VisibleImprovementChange" );
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
		int iNum = pXML->GetXmlChildrenNumber(L"VisibleTerrainRangeChange" );
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
		int iNum = pXML->GetXmlChildrenNumber(L"VisibleFeatureRangeChange" );
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
		int iNum = pXML->GetXmlChildrenNumber(L"VisibleImprovementRangeChange" );
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
		int iNum = pXML->GetXmlChildrenNumber(L"DistanceAttackCommunicabilityTypeChange" );
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
		//TB Combat Mod End
	return true;
}

bool CvPromotionInfo::readPass2(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;

/************************************************************************************************/
/* XMLCOPY								 10/13/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PromotionPrereq");
	m_iPrereqPromotion = GC.getInfoTypeForString(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PromotionPrereqOr1");
	m_iPrereqOrPromotion1 = GC.getInfoTypeForString(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PromotionPrereqOr2");
	m_iPrereqOrPromotion2 = GC.getInfoTypeForString(szTextVal);

/************************************************************************************************/
/* Afforess					  Start		 Last Update: 3/8/10									*/
/*																							  */
/*																							  */
/************************************************************************************************/
	if (pXML->TryMoveToXmlFirstChild(L"PromotionOverwrites"))
		pXML->SetIntWithChildList(&m_iNumPromotionOverwrites, &m_piPromotionOverwrites);

/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	return true;
}
/************************************************************************************************/
/* XMLCOPY								 10/14/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvPromotionInfo::copyNonDefaults(CvPromotionInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	CvHotkeyInfo::copyNonDefaults(pClassInfo, pXML);

	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	if (getSound() == cDefault) setSound(pClassInfo->getSound());

	if (getLayerAnimationPath() == ANIMATIONPATH_NONE) m_iLayerAnimationPath = pClassInfo->getLayerAnimationPath();
	if (getTechPrereq() == NO_TECH) m_iTechPrereq = pClassInfo->getTechPrereq();
	if (getStateReligionPrereq() == NO_RELIGION) m_iStateReligionPrereq = pClassInfo->getStateReligionPrereq();
	if (getMinEraType() == NO_ERA) m_iMinEraType = pClassInfo->getMinEraType();
	if (getMaxEraType() == NO_ERA) m_iMaxEraType = pClassInfo->getMaxEraType();

	if (isLeader() == bDefault) m_bLeader = pClassInfo->isLeader();
	if (m_bLeader)
	{
		m_bGraphicalOnly = true;  // don't show in Civilopedia list of promotions
	}
	if (isBlitz() == bDefault) m_bBlitz = pClassInfo->isBlitz();
	if (isAmphib() == bDefault) m_bAmphib = pClassInfo->isAmphib();
	if (isRiver() == bDefault) m_bRiver = pClassInfo->isRiver();
	if (isEnemyRoute() == bDefault) m_bEnemyRoute = pClassInfo->isEnemyRoute();
	if (isAlwaysHeal() == bDefault) m_bAlwaysHeal = pClassInfo->isAlwaysHeal();
	if (isHillsDoubleMove() == bDefault) m_bHillsDoubleMove = pClassInfo->isHillsDoubleMove();
	if (isImmuneToFirstStrikes() == bDefault) m_bImmuneToFirstStrikes = pClassInfo->isImmuneToFirstStrikes();
	if (getVisibilityChange() == iDefault) m_iVisibilityChange = pClassInfo->getVisibilityChange();
	if (getMovesChange() == iDefault) m_iMovesChange = pClassInfo->getMovesChange();
	if (getMoveDiscountChange() == iDefault) m_iMoveDiscountChange = pClassInfo->getMoveDiscountChange();
	if (getAirRangeChange() == iDefault) m_iAirRangeChange = pClassInfo->getAirRangeChange();
	if (getInterceptChange() == iDefault) m_iInterceptChange = pClassInfo->getInterceptChange();
	if (getEvasionChange() == iDefault) m_iEvasionChange = pClassInfo->getEvasionChange();
	if (getWithdrawalChange() == iDefault) m_iWithdrawalChange = pClassInfo->getWithdrawalChange();
	if (getCargoChange() == iDefault) m_iCargoChange = pClassInfo->getCargoChange();
	if (getSMCargoChange() == iDefault) m_iSMCargoChange = pClassInfo->getSMCargoChange();
	if (getSMCargoVolumeChange() == iDefault) m_iSMCargoVolumeChange = pClassInfo->getSMCargoVolumeChange();
	if (getSMCargoVolumeModifierChange() == iDefault) m_iSMCargoVolumeModifierChange = pClassInfo->getSMCargoVolumeModifierChange();
	if (getCollateralDamageChange() == iDefault) m_iCollateralDamageChange = pClassInfo->getCollateralDamageChange();
	if (getBombardRateChange() == iDefault) m_iBombardRateChange = pClassInfo->getBombardRateChange();
	if (getFirstStrikesChange() == iDefault) m_iFirstStrikesChange = pClassInfo->getFirstStrikesChange();
	if (getChanceFirstStrikesChange() == iDefault) m_iChanceFirstStrikesChange = pClassInfo->getChanceFirstStrikesChange();
	if (getEnemyHealChange() == iDefault) m_iEnemyHealChange = pClassInfo->getEnemyHealChange();
	if (getNeutralHealChange() == iDefault) m_iNeutralHealChange = pClassInfo->getNeutralHealChange();
	if (getFriendlyHealChange() == iDefault) m_iFriendlyHealChange = pClassInfo->getFriendlyHealChange();
	if (getSameTileHealChange() == iDefault) m_iSameTileHealChange = pClassInfo->getSameTileHealChange();
	if (getAdjacentTileHealChange() == iDefault) m_iAdjacentTileHealChange = pClassInfo->getAdjacentTileHealChange();
	if (getCombatPercent() == iDefault) m_iCombatPercent = pClassInfo->getCombatPercent();
	if (getCityAttackPercent() == iDefault) m_iCityAttackPercent = pClassInfo->getCityAttackPercent();
	if (getCityDefensePercent() == iDefault) m_iCityDefensePercent = pClassInfo->getCityDefensePercent();
	if (getHillsAttackPercent() == iDefault) m_iHillsAttackPercent = pClassInfo->getHillsAttackPercent();
	if (getHillsDefensePercent() == iDefault) m_iHillsDefensePercent = pClassInfo->getHillsDefensePercent();
	if (getHillsWorkPercent() == iDefault) m_iHillsWorkPercent = pClassInfo->getHillsWorkPercent();
	//ls612: Work rate modifiers
	if (getWorkRatePercent() == iDefault) m_iWorkRatePercent = pClassInfo->getWorkRatePercent();
	if (getRevoltProtection() == iDefault) m_iRevoltProtection = pClassInfo->getRevoltProtection();
	if (getCollateralDamageProtection() == iDefault) m_iCollateralDamageProtection = pClassInfo->getCollateralDamageProtection();
	if (getPillageChange() == iDefault) m_iPillageChange = pClassInfo->getPillageChange();
	if (getUpgradeDiscount() == iDefault) m_iUpgradeDiscount = pClassInfo->getUpgradeDiscount();
	if (getExperiencePercent() == iDefault) m_iExperiencePercent = pClassInfo->getExperiencePercent();
	if (getKamikazePercent() == iDefault) m_iKamikazePercent = pClassInfo->getKamikazePercent();

	for (int j = 0; j < GC.getNumTerrainInfos(); j++)
	{
		if ((m_piTerrainAttackPercent == NULL || m_piTerrainAttackPercent[j] == iDefault) &&
			pClassInfo->getTerrainAttackPercent(j) != iDefault)
		{
			if ( m_piTerrainAttackPercent == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piTerrainAttackPercent,GC.getNumTerrainInfos(),iDefault);
			}
			m_piTerrainAttackPercent[j] = pClassInfo->getTerrainAttackPercent(j);
		}

		if ((m_piTerrainDefensePercent == NULL ||m_piTerrainDefensePercent[j] == iDefault) &&
			pClassInfo->getTerrainDefensePercent(j) != iDefault)
		{
			if ( m_piTerrainDefensePercent == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piTerrainDefensePercent,GC.getNumTerrainInfos(),iDefault);
			}
			m_piTerrainDefensePercent[j] = pClassInfo->getTerrainDefensePercent(j);
		}

		if ((m_pbTerrainDoubleMove == NULL ||m_pbTerrainDoubleMove[j] == bDefault) &&
			pClassInfo->getTerrainDoubleMove(j) != bDefault)
		{
			if ( m_pbTerrainDoubleMove == NULL )
			{
				CvXMLLoadUtility::InitList(&m_pbTerrainDoubleMove,GC.getNumTerrainInfos(),bDefault);
			}
			m_pbTerrainDoubleMove[j] = pClassInfo->getTerrainDoubleMove(j);
		}

		//ls612: Terrain Work Modifiers
		if ((m_piTerrainWorkPercent == NULL || m_piTerrainWorkPercent[j] == iDefault) &&
			pClassInfo->getTerrainWorkPercent(j) != iDefault)
		{
			if ( m_piTerrainWorkPercent == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piTerrainWorkPercent,GC.getNumTerrainInfos(),iDefault);
			}
			m_piTerrainWorkPercent[j] = pClassInfo->getTerrainWorkPercent(j);
		}
	}
	if (m_piTerrainAttackPercent != NULL)
	{
		m_bAnyTerrainAttackPercent = true;
	}
	if (m_piTerrainDefensePercent != NULL)
	{
		m_bAnyTerrainDefensePercent = true;
	}

	for (int j = 0; j < GC.getNumFeatureInfos(); j++)
	{
		if ((m_piFeatureAttackPercent == NULL || m_piFeatureAttackPercent[j] == iDefault) &&
			pClassInfo->getFeatureAttackPercent(j) != iDefault)
		{
			if ( m_piFeatureAttackPercent == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piFeatureAttackPercent,GC.getNumFeatureInfos(),iDefault);
			}
			m_piFeatureAttackPercent[j] = pClassInfo->getFeatureAttackPercent(j);
		}

		if ((m_piFeatureDefensePercent == NULL ||m_piFeatureDefensePercent[j] == iDefault) &&
			pClassInfo->getFeatureDefensePercent(j) != iDefault)
		{
			if ( m_piFeatureDefensePercent == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piFeatureDefensePercent,GC.getNumFeatureInfos(),iDefault);
			}
			m_piFeatureDefensePercent[j] = pClassInfo->getFeatureDefensePercent(j);
		}

		if ((m_pbFeatureDoubleMove == NULL ||m_pbFeatureDoubleMove[j] == bDefault) &&
			pClassInfo->getFeatureDoubleMove(j) != bDefault)
		{
			if ( m_pbFeatureDoubleMove == NULL )
			{
				CvXMLLoadUtility::InitList(&m_pbFeatureDoubleMove,GC.getNumFeatureInfos(),bDefault);
			}
			m_pbFeatureDoubleMove[j] = pClassInfo->getFeatureDoubleMove(j);
		}
		//ls612: Terrain Work Modifiers
		if ((m_piFeatureWorkPercent == NULL || m_piFeatureWorkPercent[j] == iDefault) &&
			pClassInfo->getFeatureWorkPercent(j) != iDefault)
		{
			if ( m_piFeatureWorkPercent == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piFeatureWorkPercent,GC.getNumFeatureInfos(),iDefault);
			}
			m_piFeatureWorkPercent[j] = pClassInfo->getFeatureWorkPercent(j);
		}
	}
	if (m_piFeatureAttackPercent != NULL)
	{
		m_bAnyFeatureAttackPercent = true;
	}
	if (m_piFeatureDefensePercent != NULL)
	{
		m_bAnyFeatureDefensePercent = true;
	}
	for (int j = 0; j < GC.getNumUnitCombatInfos(); j++)
	{
		if ((m_piUnitCombatModifierPercent == NULL || m_piUnitCombatModifierPercent[j] == iDefault) &&
			pClassInfo->getUnitCombatModifierPercent(j) != iDefault)
		{
			if ( m_piUnitCombatModifierPercent == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piUnitCombatModifierPercent,GC.getNumUnitCombatInfos(),iDefault);
			}
			m_piUnitCombatModifierPercent[j] = pClassInfo->getUnitCombatModifierPercent(j);
		}

		if ((m_pbUnitCombat == NULL || m_pbUnitCombat[j] == bDefault) &&
			pClassInfo->getUnitCombat(j) != bDefault)
		{
			if ( m_pbUnitCombat == NULL )
			{
				CvXMLLoadUtility::InitList(&m_pbUnitCombat,GC.getNumUnitCombatInfos(),bDefault);
			}
			m_pbUnitCombat[j] = pClassInfo->getUnitCombat(j);
		}

		//if ((m_piAIWeightbyUnitCombatTypes == NULL || m_piAIWeightbyUnitCombatTypes[j] == iDefault) &&
		//	pClassInfo->getAIWeightbyUnitCombatType(j) != iDefault)
		//{
		//	if ( m_piAIWeightbyUnitCombatTypes == NULL )
		//	{
		//		CvXMLLoadUtility::InitList(&m_piAIWeightbyUnitCombatTypes,GC.getNumUnitCombatInfos(),iDefault);
		//	}
		//	m_piAIWeightbyUnitCombatTypes[j] = pClassInfo->getAIWeightbyUnitCombatType(j);
		//}
	}
	if (m_piUnitCombatModifierPercent != NULL)
	{
		m_bAnyUnitCombatModifierPercent = true;
	}
	//if (m_piAIWeightbyUnitCombatTypes != NULL)
	//{
	//	m_bAnyAIWeightbyUnitCombatType = true;
	//}
	for (int j = 0; j < NUM_DOMAIN_TYPES; j++)
	{
		if ((m_piDomainModifierPercent == NULL || m_piDomainModifierPercent[j] == iDefault) &&
			pClassInfo->getDomainModifierPercent(j) != iDefault)
		{
			if ( m_piDomainModifierPercent == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piDomainModifierPercent,NUM_DOMAIN_TYPES,iDefault);
			}
			m_piDomainModifierPercent[j] = pClassInfo->getDomainModifierPercent(j);
		}
	}
	if (m_piDomainModifierPercent != NULL)
	{
		m_bAnyDomainModifierPercent = true;
	}

/*****************************************************************************************************/
/**  Author: TheLadiesOgre																		  **/
/**  Date: 16.09.2009																			   **/
/**  ModComp: TLOTags																			   **/
/**  Reason Added: New Tag Definition															   **/
/**  Notes: Adapted to C2C by Thunderbrd															**/
/*****************************************************************************************************/
	if (isDefensiveVictoryMove() == bDefault) m_bDefensiveVictoryMove = pClassInfo->isDefensiveVictoryMove();
	if (isFreeDrop() == bDefault) m_bFreeDrop = pClassInfo->isFreeDrop();
	if (isOffensiveVictoryMove() == bDefault) m_bOffensiveVictoryMove = pClassInfo->isOffensiveVictoryMove();
//Team Project (2)
	if (isOneUp() == bDefault) m_bOneUp = pClassInfo->isOneUp();
	if (isPillageCulture() == bDefault) m_bPillageCulture = pClassInfo->isPillageCulture();
	if (isPillageEspionage() == bDefault) m_bPillageEspionage = pClassInfo->isPillageEspionage();
	if (isPillageMarauder() == bDefault) m_bPillageMarauder = pClassInfo->isPillageMarauder();
	if (isPillageOnMove() == bDefault) m_bPillageOnMove = pClassInfo->isPillageOnMove();
	if (isPillageOnVictory() == bDefault) m_bPillageOnVictory = pClassInfo->isPillageOnVictory();
	if (isPillageResearch() == bDefault) m_bPillageResearch = pClassInfo->isPillageResearch();
	if (getAirCombatLimitChange() == iDefault) m_iAirCombatLimitChange = pClassInfo->getAirCombatLimitChange();
	if (getCelebrityHappy() == iDefault) m_iCelebrityHappy = pClassInfo->getCelebrityHappy();
	if (getCollateralDamageLimitChange() == iDefault) m_iCollateralDamageLimitChange = pClassInfo->getCollateralDamageLimitChange();
	if (getCollateralDamageMaxUnitsChange() == iDefault) m_iCollateralDamageMaxUnitsChange = pClassInfo->getCollateralDamageMaxUnitsChange();
	if (getCombatLimitChange() == iDefault) m_iCombatLimitChange = pClassInfo->getCombatLimitChange();
	if (getExtraDropRange() == iDefault) m_iExtraDropRange = pClassInfo->getExtraDropRange();
//Team Project (2)
	if (getSurvivorChance() == iDefault) m_iSurvivorChance = pClassInfo->getSurvivorChance();
	if (getVictoryAdjacentHeal() == iDefault) m_iVictoryAdjacentHeal = pClassInfo->getVictoryAdjacentHeal();
	if (getVictoryHeal() == iDefault) m_iVictoryHeal = pClassInfo->getVictoryHeal();
//Team Project (2)
	if (getVictoryStackHeal() == iDefault) m_iVictoryStackHeal = pClassInfo->getVictoryStackHeal();
/*****************************************************************************************************/
/**  TheLadiesOgre; 16.09.2009; TLOTags															 **/
/*****************************************************************************************************/
	//TB Combat Mods Begin  TB SubCombat Mods Begin
	// Text Strings
	if ( getRenamesUnitTo() == NULL || getRenamesUnitTo() == wDefault )
	{
		m_szRenamesUnitTo = pClassInfo->getRenamesUnitTo();
	}
	// Textual References
	if (getPromotionLine() == NO_PROMOTIONLINE) m_ePromotionLine = pClassInfo->getPromotionLine();
	if (getReplacesUnitCombat() == NO_UNITCOMBAT) m_eReplacesUnitCombat = pClassInfo->getReplacesUnitCombat();
	if (getDomainCargoChange() == NO_DOMAIN) m_eDomainCargoChange = pClassInfo->getDomainCargoChange();
	if (getSpecialCargoChange() == NO_SPECIALUNIT) m_eSpecialCargoChange = pClassInfo->getSpecialCargoChange();
	if (getSpecialCargoPrereq() == NO_SPECIALUNIT) m_eSpecialCargoPrereq = pClassInfo->getSpecialCargoPrereq();
	if (getSMSpecialCargoChange() == NO_SPECIALUNIT) m_eSMSpecialCargoChange = pClassInfo->getSMSpecialCargoChange();
	if (getSMSpecialCargoPrereq() == NO_SPECIALUNIT) m_eSMSpecialCargoPrereq = pClassInfo->getSMSpecialCargoPrereq();
	if (getSMNotSpecialCargoChange() == NO_SPECIALUNIT) m_eSMNotSpecialCargoChange = pClassInfo->getSMNotSpecialCargoChange();
	if (getSMNotSpecialCargoPrereq() == NO_SPECIALUNIT) m_eSMNotSpecialCargoPrereq = pClassInfo->getSMNotSpecialCargoPrereq();
	if (setSpecialUnit() == NO_SPECIALUNIT) m_eSetSpecialUnit = pClassInfo->setSpecialUnit();
	//integers
	if (getAttackCombatModifierChange() == iDefault) m_iAttackCombatModifierChange = pClassInfo->getAttackCombatModifierChange();
	if (getDefenseCombatModifierChange() == iDefault) m_iDefenseCombatModifierChange = pClassInfo->getDefenseCombatModifierChange();
	if (getPursuitChange(true) == iDefault) m_iPursuitChange = pClassInfo->getPursuitChange(true);
	if (getEarlyWithdrawChange(true) == iDefault) m_iEarlyWithdrawChange = pClassInfo->getEarlyWithdrawChange(true);
	if (getVSBarbsChange() == iDefault) m_iVSBarbsChange = pClassInfo->getVSBarbsChange();
	if (getArmorChange() == iDefault) m_iArmorChange = pClassInfo->getArmorChange();
	if (getPunctureChange() == iDefault) m_iPunctureChange = pClassInfo->getPunctureChange();
	if (getOverrunChange(true) == iDefault) m_iOverrunChange = pClassInfo->getOverrunChange(true);
	if (getRepelChange(true) == iDefault) m_iRepelChange = pClassInfo->getRepelChange(true);
	if (getFortRepelChange(true) == iDefault) m_iFortRepelChange = pClassInfo->getFortRepelChange(true);
	if (getRepelRetriesChange(true) == iDefault) m_iRepelRetriesChange = pClassInfo->getRepelRetriesChange(true);
	if (getUnyieldingChange(true) == iDefault) m_iUnyieldingChange = pClassInfo->getUnyieldingChange(true);
	if (getKnockbackChange(true) == iDefault) m_iKnockbackChange = pClassInfo->getKnockbackChange(true);
	if (getKnockbackRetriesChange(true) == iDefault) m_iKnockbackRetriesChange = pClassInfo->getKnockbackRetriesChange(true);
	if (getStrAdjperRndChange(true) == iDefault) m_iStrAdjperRndChange = pClassInfo->getStrAdjperRndChange(true);
	if (getStrAdjperAttChange(true) == iDefault) m_iStrAdjperAttChange = pClassInfo->getStrAdjperAttChange(true);
	if (getStrAdjperDefChange(true) == iDefault) m_iStrAdjperDefChange = pClassInfo->getStrAdjperDefChange(true);
	if (getWithdrawAdjperAttChange(true) == iDefault) m_iWithdrawAdjperAttChange = pClassInfo->getWithdrawAdjperAttChange(true);
	if (getUnnerveChange(true) == iDefault) m_iUnnerveChange = pClassInfo->getUnnerveChange(true);
	if (getEncloseChange(true) == iDefault) m_iEncloseChange = pClassInfo->getEncloseChange(true);
	if (getLungeChange(true) == iDefault) m_iLungeChange = pClassInfo->getLungeChange(true);
	if (getDynamicDefenseChange(true) == iDefault) m_iDynamicDefenseChange = pClassInfo->getDynamicDefenseChange(true);
	if (getStrengthChange() == iDefault) m_iStrengthChange = pClassInfo->getStrengthChange();
	if (getLinePriority() == iDefault) m_iLinePriority = pClassInfo->getLinePriority();
	if (getFortitudeChange() == iDefault) m_iFortitudeChange = pClassInfo->getFortitudeChange();
	if (getDamageperTurn() == iDefault) m_iDamageperTurn = pClassInfo->getDamageperTurn();
	if (getStrAdjperTurn() == iDefault) m_iStrAdjperTurn = pClassInfo->getStrAdjperTurn();
	if (getWeakenperTurn() == iDefault) m_iWeakenperTurn = pClassInfo->getWeakenperTurn();
	if (getFrontSupportPercentChange(true) == iDefault) m_iFrontSupportPercentChange = pClassInfo->getFrontSupportPercentChange(true);
	if (getShortRangeSupportPercentChange(true) == iDefault) m_iShortRangeSupportPercentChange = pClassInfo->getShortRangeSupportPercentChange(true);
	if (getMediumRangeSupportPercentChange(true) == iDefault) m_iMediumRangeSupportPercentChange = pClassInfo->getMediumRangeSupportPercentChange(true);
	if (getLongRangeSupportPercentChange(true) == iDefault) m_iLongRangeSupportPercentChange = pClassInfo->getLongRangeSupportPercentChange(true);
	if (getFlankSupportPercentChange(true) == iDefault) m_iFlankSupportPercentChange = pClassInfo->getFlankSupportPercentChange(true);
	if (getDodgeModifierChange() == iDefault) m_iDodgeModifierChange = pClassInfo->getDodgeModifierChange();
	if (getPrecisionModifierChange() == iDefault) m_iPrecisionModifierChange = pClassInfo->getPrecisionModifierChange();
	if (getPowerShotsChange() == iDefault) m_iPowerShotsChange = pClassInfo->getPowerShotsChange();
	if (getPowerShotCombatModifierChange() == iDefault) m_iPowerShotCombatModifierChange = pClassInfo->getPowerShotCombatModifierChange();
	if (getPowerShotPunctureModifierChange() == iDefault) m_iPowerShotPunctureModifierChange = pClassInfo->getPowerShotPunctureModifierChange();
	if (getPowerShotPrecisionModifierChange() == iDefault) m_iPowerShotPrecisionModifierChange = pClassInfo->getPowerShotPrecisionModifierChange();
	if (getPowerShotCriticalModifierChange() == iDefault) m_iPowerShotCriticalModifierChange = pClassInfo->getPowerShotCriticalModifierChange();
	if (getCriticalModifierChange() == iDefault) m_iCriticalModifierChange = pClassInfo->getCriticalModifierChange();
	if (getEnduranceChange() == iDefault) m_iEnduranceChange = pClassInfo->getEnduranceChange();
	if (getRoundStunProbChange() == iDefault) m_iRoundStunProbChange = pClassInfo->getRoundStunProbChange();
	if (getPoisonProbabilityModifierChange() == iDefault) m_iPoisonProbabilityModifierChange = pClassInfo->getPoisonProbabilityModifierChange();
//Team Project (3)
	if (getCaptureProbabilityModifierChange() == iDefault) m_iCaptureProbabilityModifierChange = pClassInfo->getCaptureProbabilityModifierChange();
	if (getCaptureResistanceModifierChange() == iDefault) m_iCaptureResistanceModifierChange = pClassInfo->getCaptureResistanceModifierChange();
//Team Project (4)
//WorkRateMod
	if (getHillsWorkModifierChange() == iDefault) m_iHillsWorkModifierChange = pClassInfo->getHillsWorkModifierChange();
	if (getPeaksWorkModifierChange() == iDefault) m_iPeaksWorkModifierChange = pClassInfo->getPeaksWorkModifierChange();

	if (getBreakdownChanceChange() == iDefault) m_iBreakdownChanceChange = pClassInfo->getBreakdownChanceChange();
	if (getBreakdownDamageChange() == iDefault) m_iBreakdownDamageChange = pClassInfo->getBreakdownDamageChange();
	if (getTauntChange() == iDefault) m_iTauntChange = pClassInfo->getTauntChange();
	if (getMaxHPChange() == iDefault) m_iMaxHPChange = pClassInfo->getMaxHPChange();
	if (getStrengthModifier() == iDefault) m_iStrengthModifier = pClassInfo->getStrengthModifier();
	if (getQualityChange() == iDefault) m_iQualityChange = pClassInfo->getQualityChange();
	if (getGroupChange() == iDefault) m_iGroupChange = pClassInfo->getGroupChange();
	if (getLevelPrereq() == iDefault) m_iLevelPrereq = pClassInfo->getLevelPrereq();
	if (getDamageModifierChange() == iDefault) m_iDamageModifierChange = pClassInfo->getDamageModifierChange();
	if (getCostModifierChange() == iDefault) m_iCostModifierChange = pClassInfo->getCostModifierChange();
	if (getRBombardDamageChange() == iDefault) m_iRBombardDamageChange = pClassInfo->getRBombardDamageChange();
	if (getRBombardDamageLimitChange() == iDefault) m_iRBombardDamageLimitChange = pClassInfo->getRBombardDamageLimitChange();
	if (getRBombardDamageMaxUnitsChange() == iDefault) m_iRBombardDamageMaxUnitsChange = pClassInfo->getRBombardDamageMaxUnitsChange();
	if (getDCMBombRangeChange() == iDefault) m_iDCMBombRangeChange = pClassInfo->getDCMBombRangeChange();
	if (getDCMBombAccuracyChange() == iDefault) m_iDCMBombAccuracyChange = pClassInfo->getDCMBombAccuracyChange();
	if (getCombatModifierPerSizeMoreChange(true) == iDefault) m_iCombatModifierPerSizeMoreChange = pClassInfo->getCombatModifierPerSizeMoreChange(true);
	if (getCombatModifierPerSizeLessChange(true) == iDefault) m_iCombatModifierPerSizeLessChange = pClassInfo->getCombatModifierPerSizeLessChange(true);
	if (getCombatModifierPerVolumeMoreChange(true) == iDefault) m_iCombatModifierPerVolumeMoreChange = pClassInfo->getCombatModifierPerVolumeMoreChange(true);
	if (getCombatModifierPerVolumeLessChange(true) == iDefault) m_iCombatModifierPerVolumeLessChange = pClassInfo->getCombatModifierPerVolumeLessChange(true);
	if (getSelfHealModifier() == iDefault) m_iSelfHealModifier = pClassInfo->getSelfHealModifier();
	if (getNumHealSupport() == iDefault) m_iNumHealSupport = pClassInfo->getNumHealSupport();
	if (getExcileChange() == iDefault) m_iExcileChange = pClassInfo->getExcileChange();
	if (getPassageChange() == iDefault) m_iPassageChange = pClassInfo->getPassageChange();
	if (getNoNonOwnedCityEntryChange() == iDefault) m_iNoNonOwnedCityEntryChange = pClassInfo->getNoNonOwnedCityEntryChange();
	if (getBarbCoExistChange() == iDefault) m_iBarbCoExistChange = pClassInfo->getBarbCoExistChange();
	if (getBlendIntoCityChange() == iDefault) m_iBlendIntoCityChange = pClassInfo->getBlendIntoCityChange();
	if (getUpgradeAnywhereChange() == iDefault) m_iUpgradeAnywhereChange = pClassInfo->getUpgradeAnywhereChange();
	if (getInsidiousnessChange() == iDefault) m_iInsidiousnessChange = pClassInfo->getInsidiousnessChange();
	if (getInvestigationChange() == iDefault) m_iInvestigationChange = pClassInfo->getInvestigationChange();
	if (getAssassinChange() == iDefault) m_iAssassinChange = pClassInfo->getAssassinChange();
	if (getStealthStrikesChange(true) == iDefault) m_iStealthStrikesChange = pClassInfo->getStealthStrikesChange(true);
	if (getStealthCombatModifierChange(true) == iDefault) m_iStealthCombatModifierChange = pClassInfo->getStealthCombatModifierChange(true);
	if (getStealthDefenseChange(true) == iDefault) m_iStealthDefenseChange = pClassInfo->getStealthDefenseChange(true);
	if (getDefenseOnlyChange() == iDefault) m_iDefenseOnlyChange = pClassInfo->getDefenseOnlyChange();
	if (getNoInvisibilityChange() == iDefault) m_iNoInvisibilityChange = pClassInfo->getNoInvisibilityChange();
	if (getTrapDamageMax() == iDefault) m_iTrapDamageMax = pClassInfo->getTrapDamageMax();
	if (getTrapDamageMin() == iDefault) m_iTrapDamageMin = pClassInfo->getTrapDamageMin();
	if (getTrapComplexity() == iDefault) m_iTrapComplexity = pClassInfo->getTrapComplexity();
	if (getNumTriggers() == iDefault) m_iNumTriggers = pClassInfo->getNumTriggers();
	if (getTriggerBeforeAttackChange() == iDefault) m_iTriggerBeforeAttackChange = pClassInfo->getTriggerBeforeAttackChange();
	if (getHiddenNationalityChange() == iDefault) m_iHiddenNationalityChange = pClassInfo->getHiddenNationalityChange();
	if (getAnimalIgnoresBordersChange() == iDefault) m_iAnimalIgnoresBordersChange = pClassInfo->getAnimalIgnoresBordersChange();
	if (getNoDefensiveBonusChange() == iDefault) m_iNoDefensiveBonusChange = pClassInfo->getNoDefensiveBonusChange();
	if (getGatherHerdChange() == iDefault) m_iGatherHerdChange = pClassInfo->getGatherHerdChange();
	if (getReligiousCombatModifierChange() == iDefault) m_iReligiousCombatModifierChange = pClassInfo->getReligiousCombatModifierChange();
	// booleans
	if (isStampedeChange() == bDefault) m_bStampedeChange = pClassInfo->isStampedeChange();
	if (isRemoveStampede() == bDefault) m_bRemoveStampede = pClassInfo->isRemoveStampede();
	if (isOnslaughtChange() == bDefault) m_bOnslaughtChange = pClassInfo->isOnslaughtChange();
	if (isEquipment() == bDefault) m_bEquipment = pClassInfo->isEquipment();
	if (isAffliction() == bDefault) m_bAffliction = pClassInfo->isAffliction();
	if (isParalyze() == bDefault) m_bParalyze = pClassInfo->isParalyze();
	if (isMakesDamageCold() == bDefault) m_bMakesDamageCold = pClassInfo->isMakesDamageCold();
	if (isMakesDamageNotCold() == bDefault) m_bMakesDamageNotCold = pClassInfo->isMakesDamageNotCold();
	if (isAddsColdImmunity() == bDefault) m_bAddsColdImmunity = pClassInfo->isAddsColdImmunity();
	if (isRemovesColdImmunity() == bDefault) m_bRemovesColdImmunity = pClassInfo->isRemovesColdImmunity();
	if (isCritical() == bDefault) m_bCritical = pClassInfo->isCritical();
	if (isAttackOnlyCitiesAdd() == bDefault) m_bAttackOnlyCitiesAdd = pClassInfo->isAttackOnlyCitiesAdd();
	if (isAttackOnlyCitiesSubtract() == bDefault) m_bAttackOnlyCitiesSubtract = pClassInfo->isAttackOnlyCitiesSubtract();
	if (isIgnoreNoEntryLevelAdd() == bDefault) m_bIgnoreNoEntryLevelAdd = pClassInfo->isIgnoreNoEntryLevelAdd();
	if (isIgnoreNoEntryLevelSubtract() == bDefault) m_bIgnoreNoEntryLevelSubtract = pClassInfo->isIgnoreNoEntryLevelSubtract();
	if (isIgnoreZoneofControlAdd() == bDefault) m_bIgnoreZoneofControlAdd = pClassInfo->isIgnoreZoneofControlAdd();
	if (isIgnoreZoneofControlSubtract() == bDefault) m_bIgnoreZoneofControlSubtract = pClassInfo->isIgnoreZoneofControlSubtract();
	if (isFliesToMoveAdd() == bDefault) m_bFliesToMoveAdd = pClassInfo->isFliesToMoveAdd();
	if (isFliesToMoveSubtract() == bDefault) m_bFliesToMoveSubtract = pClassInfo->isFliesToMoveSubtract();
	if (isZeroesXP() == bDefault) m_bZeroesXP = pClassInfo->isZeroesXP();
	if (isForOffset() == bDefault) m_bForOffset = pClassInfo->isForOffset();
	if (isCargoPrereq() == bDefault) m_bCargoPrereq = pClassInfo->isCargoPrereq();
	if (isRBombardPrereq() == bDefault) m_bRBombardPrereq = pClassInfo->isRBombardPrereq();
	if (isNoSelfHeal() == bDefault) m_bNoSelfHeal = pClassInfo->isNoSelfHeal();
	if (isSetOnHNCapture() == bDefault) m_bSetOnHNCapture = pClassInfo->isSetOnHNCapture();
	if (isSetOnInvestigated() == bDefault) m_bSetOnInvestigated = pClassInfo->isSetOnInvestigated();
	if (isStatus() == bDefault) m_bStatus = pClassInfo->isStatus();
	if (isPrereqNormInvisible() == bDefault) m_bPrereqNormInvisible = pClassInfo->isPrereqNormInvisible();
	if (isPlotPrereqsKeepAfter() == bDefault) m_bPlotPrereqsKeepAfter = pClassInfo->isPlotPrereqsKeepAfter();
	if (isRemoveAfterSet() == bDefault) m_bRemoveAfterSet = pClassInfo->isRemoveAfterSet();
	if (isQuick() == bDefault) m_bQuick = pClassInfo->isQuick();
	// bool vectors without delayed resolution
	if (getNumSubCombatChangeTypes() == 0)
	{
		m_aiSubCombatChangeTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumSubCombatChangeTypes(); i++)
		{
			m_aiSubCombatChangeTypes.push_back(pClassInfo->getSubCombatChangeType(i));
		}
	}

	if (getNumRemovesUnitCombatTypes() == 0)
	{
		m_aiRemovesUnitCombatTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumRemovesUnitCombatTypes(); i++)
		{
			m_aiRemovesUnitCombatTypes.push_back(pClassInfo->getRemovesUnitCombatType(i));
		}
	}

	if (getNumOnGameOptions() == 0)
	{
		m_aiOnGameOptions.clear();
		for ( int i = 0; i < pClassInfo->getNumOnGameOptions(); i++)
		{
			m_aiOnGameOptions.push_back(pClassInfo->getOnGameOption(i));
		}
	}

	if (getNumNotOnGameOptions() == 0)
	{
		m_aiNotOnGameOptions.clear();
		for ( int i = 0; i < pClassInfo->getNumNotOnGameOptions(); i++)
		{
			m_aiNotOnGameOptions.push_back(pClassInfo->getNotOnGameOption(i));
		}
	}

	if (getNumFreetoUnitCombats() == 0)
	{
		m_aiFreetoUnitCombats.clear();
		for ( int i = 0; i < pClassInfo->getNumFreetoUnitCombats(); i++)
		{
			m_aiFreetoUnitCombats.push_back(pClassInfo->getFreetoUnitCombat(i));
		}
	}

	if (getNumNotOnUnitCombatTypes() == 0)
	{
		m_aiNotOnUnitCombatTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumNotOnUnitCombatTypes(); i++)
		{
			m_aiNotOnUnitCombatTypes.push_back(pClassInfo->getNotOnUnitCombatType(i));
		}
	}

	if (getNumNotOnDomainTypes() == 0)
	{
		m_aiNotOnDomainTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumNotOnDomainTypes(); i++)
		{
			m_aiNotOnDomainTypes.push_back(pClassInfo->getNotOnDomainType(i));
		}
	}

	if (getNumNoAutoEquiptoCombatClassTypes() == 0)
	{
		m_aiNoAutoEquiptoCombatClassTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumNoAutoEquiptoCombatClassTypes(); i++)
		{
			m_aiNoAutoEquiptoCombatClassTypes.push_back(pClassInfo->getNoAutoEquiptoCombatClassType(i));
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

	// bool vector with delayed resolution
	if (getNumCureAfflictionChangeTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromIntVector(m_aiCureAfflictionChangeTypes, pClassInfo->m_aiCureAfflictionChangeTypes);
	}

	if (getNumPrereqBonusTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromIntVector(m_aiPrereqBonusTypes, pClassInfo->m_aiPrereqBonusTypes);
	}

	if (getNumAddsBuildTypes() == 0)
	{
		int iNum = pClassInfo->getNumAddsBuildTypes();
		m_aiAddsBuildTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			GC.copyNonDefaultDelayedResolution((int*)&(m_aiAddsBuildTypes[i]), (int*)&(pClassInfo->m_aiAddsBuildTypes[i]));
		}
	}

	if (getNumNegatesInvisibilityTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromIntVector(m_aiNegatesInvisibilityTypes, pClassInfo->m_aiNegatesInvisibilityTypes);
	}

	if (getNumPrereqTerrainTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromIntVector(m_aiPrereqTerrainTypes, pClassInfo->m_aiPrereqTerrainTypes);
	}

	if (getNumPrereqFeatureTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromIntVector(m_aiPrereqFeatureTypes, pClassInfo->m_aiPrereqFeatureTypes);
	}

	if (getNumPrereqImprovementTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromIntVector(m_aiPrereqImprovementTypes, pClassInfo->m_aiPrereqImprovementTypes);
	}

	if (getNumPrereqPlotBonusTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromIntVector(m_aiPrereqPlotBonusTypes, pClassInfo->m_aiPrereqPlotBonusTypes);
	}

	if (getNumPrereqLocalBuildingClassTypes() == 0)
	{
		int iNum = pClassInfo->getNumPrereqLocalBuildingClassTypes();
		m_aiPrereqLocalBuildingClassTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			GC.copyNonDefaultDelayedResolution((int*)&(m_aiPrereqLocalBuildingClassTypes[i]), (int*)&(pClassInfo->m_aiPrereqLocalBuildingClassTypes[i]));
		}
	}

	if (getNumTrapSetWithPromotionTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromIntVector(m_aiTrapSetWithPromotionTypes, pClassInfo->m_aiTrapSetWithPromotionTypes);
	}

	if (getNumTrapImmunityUnitCombatTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromIntVector(m_aiTrapImmunityUnitCombatTypes, pClassInfo->m_aiTrapImmunityUnitCombatTypes);
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

	if (getNumTargetUnitCombatTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromIntVector(m_aiTargetUnitCombatTypes, pClassInfo->m_aiTargetUnitCombatTypes);
	}

	// int vectors utilizing pairing without delayed resolution
	if (getNumFlankingStrikesbyUnitCombatTypesChange()==0)
	{
		for (int i=0; i < pClassInfo->getNumFlankingStrikesbyUnitCombatTypesChange(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getFlankingStrengthbyUnitCombatTypeChange(i);
			m_aFlankingStrengthbyUnitCombatTypeChange.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumWithdrawOnTerrainTypeChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumWithdrawOnTerrainTypeChanges(); i++)
		{
			TerrainTypes eTerrain = ((TerrainTypes)i);
			int iChange = pClassInfo->getWithdrawOnTerrainTypeChange(i);
			m_aWithdrawOnTerrainTypesChange.push_back(std::make_pair(eTerrain, iChange));
		}
	}

	if (getNumWithdrawOnFeatureTypeChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumWithdrawOnFeatureTypeChanges(); i++)
		{
			FeatureTypes eFeature = ((FeatureTypes)i);
			int iChange = pClassInfo->getWithdrawOnFeatureTypeChange(i);
			m_aWithdrawOnFeatureTypesChange.push_back(std::make_pair(eFeature, iChange));
		}
	}

	if (getNumWithdrawVSUnitCombatChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumWithdrawVSUnitCombatChangeTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getWithdrawVSUnitCombatChangeType(i);
			m_aWithdrawVSUnitCombatChangeTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumPursuitVSUnitCombatChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumPursuitVSUnitCombatChangeTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getPursuitVSUnitCombatChangeType(i, true);
			m_aPursuitVSUnitCombatChangeTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumRepelVSUnitCombatChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumRepelVSUnitCombatChangeTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getRepelVSUnitCombatChangeType(i, true);
			m_aRepelVSUnitCombatChangeTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumKnockbackVSUnitCombatChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumKnockbackVSUnitCombatChangeTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getKnockbackVSUnitCombatChangeType(i, true);
			m_aKnockbackVSUnitCombatChangeTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumPunctureVSUnitCombatChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumPunctureVSUnitCombatChangeTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getPunctureVSUnitCombatChangeType(i);
			m_aPunctureVSUnitCombatChangeTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumArmorVSUnitCombatChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumArmorVSUnitCombatChangeTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getArmorVSUnitCombatChangeType(i);
			m_aArmorVSUnitCombatChangeTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumDodgeVSUnitCombatChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumDodgeVSUnitCombatChangeTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getDodgeVSUnitCombatChangeType(i);
			m_aDodgeVSUnitCombatChangeTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumPrecisionVSUnitCombatChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumPrecisionVSUnitCombatChangeTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getPrecisionVSUnitCombatChangeType(i);
			m_aPrecisionVSUnitCombatChangeTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumCriticalVSUnitCombatChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumCriticalVSUnitCombatChangeTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getCriticalVSUnitCombatChangeType(i);
			m_aCriticalVSUnitCombatChangeTypes.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumRoundStunVSUnitCombatChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumRoundStunVSUnitCombatChangeTypes(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getRoundStunVSUnitCombatChangeType(i);
			m_aRoundStunVSUnitCombatChangeTypes.push_back(std::make_pair(eUnitCombat, iChange));
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


//Team Project (4)
		//WorkRateMod
	if (getNumTerrainWorkRateModifierChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumTerrainWorkRateModifierChangeTypes(); i++)
		{
			TerrainTypes eTerrain = ((TerrainTypes)i);
			int iChange = pClassInfo->getTerrainWorkRateModifierChangeType(i);
			m_aTerrainWorkRateModifierChangeTypes.push_back(std::make_pair(eTerrain, iChange));
		}
	}

	if (getNumFeatureWorkRateModifierChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumFeatureWorkRateModifierChangeTypes(); i++)
		{
			FeatureTypes eFeature = ((FeatureTypes)i);
			int iChange = pClassInfo->getFeatureWorkRateModifierChangeType(i);
			m_aFeatureWorkRateModifierChangeTypes.push_back(std::make_pair(eFeature, iChange));
		}
	}

	if (getNumBuildWorkRateModifierChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumBuildWorkRateModifierChangeTypes(); i++)
		{
			BuildTypes eBuild = ((BuildTypes)i);
			int iChange = pClassInfo->getBuildWorkRateModifierChangeType(i);
			m_aBuildWorkRateModifierChangeTypes.push_back(std::make_pair(eBuild, iChange));
		}
	}

	if (getNumVisibilityIntensityChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumVisibilityIntensityChangeTypes(); i++)
		{
			InvisibleTypes eInvisible = ((InvisibleTypes)i);
			int iChange = pClassInfo->getVisibilityIntensityChangeType(i);
			m_aVisibilityIntensityChangeTypes.push_back(std::make_pair(eInvisible, iChange));
		}
	}

	if (getNumInvisibilityIntensityChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumInvisibilityIntensityChangeTypes(); i++)
		{
			InvisibleTypes eInvisible = ((InvisibleTypes)i);
			int iChange = pClassInfo->getInvisibilityIntensityChangeType(i);
			m_aInvisibilityIntensityChangeTypes.push_back(std::make_pair(eInvisible, iChange));
		}
	}

	if (getNumVisibilityIntensityRangeChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumVisibilityIntensityRangeChangeTypes(); i++)
		{
			InvisibleTypes eInvisible = ((InvisibleTypes)i);
			int iChange = pClassInfo->getVisibilityIntensityRangeChangeType(i);
			m_aVisibilityIntensityRangeChangeTypes.push_back(std::make_pair(eInvisible, iChange));
		}
	}

	// int vectors utilizing struct with delayed resolution
	if (getNumAIWeightbyUnitCombatTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aAIWeightbyUnitCombatTypes, pClassInfo->m_aAIWeightbyUnitCombatTypes);
	}

	if (getNumAfflictionFortitudeChangeModifiers() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aAfflictionFortitudeChangeModifiers, pClassInfo->m_aAfflictionFortitudeChangeModifiers);
	}

	if (getNumAfflictOnAttackChangeTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aAfflictOnAttackChangeTypes, pClassInfo->m_aAfflictOnAttackChangeTypes);
	}

	if (getNumHealUnitCombatChangeTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aHealUnitCombatChangeTypes, pClassInfo->m_aHealUnitCombatChangeTypes);
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
	//TB Combat Mods End  TB SubCombat Mods end

	// Readpass2 stuff
	if (getPrereqPromotion() == iTextDefault) m_iPrereqPromotion = pClassInfo->getPrereqPromotion();
	if (getPrereqOrPromotion1() == iTextDefault) m_iPrereqOrPromotion1 = pClassInfo->getPrereqOrPromotion1();
	if (getPrereqOrPromotion2() == iTextDefault) m_iPrereqOrPromotion2 = pClassInfo->getPrereqOrPromotion2();

/************************************************************************************************/
/* Afforess					  Start		 Last Update: 3/8/10									*/
/*																							  */
/*																							  */
/************************************************************************************************/
	if (isCanMovePeaks() == bDefault) m_bCanMovePeaks = pClassInfo->isCanMovePeaks();
	//	Koshling - enhanced mountaineering mode to differentiate between ability to move through
	//	mountains, and ability to lead a stack through mountains
	if (isCanLeadThroughPeaks() == bDefault) m_bCanLeadThroughPeaks = pClassInfo->isCanLeadThroughPeaks();

	if(pClassInfo->getNumPromotionOverwrites() > 0)
	{
		int* tempArray = new int[getNumPromotionOverwrites() + pClassInfo->getNumPromotionOverwrites()];
		for(int i = 0; i< getNumPromotionOverwrites(); ++i)
		{
			tempArray[i] = getPromotionOverwrites(i);
		}
		int iNewItems = 0;
		for(int i = 0; i < pClassInfo->getNumPromotionOverwrites(); ++i)
		{
			bool bLoad = true;
			for(int j=0;j<getNumPromotionOverwrites();++j)
			{
				if(pClassInfo->getPromotionOverwrites(i) == getPromotionOverwrites(j))
				{
					bLoad = false;
					break;
				}
			}
			if(bLoad)
			{
				tempArray[iNewItems+getNumPromotionOverwrites()] = pClassInfo->getPromotionOverwrites(i);
				iNewItems++;
			}
		}
		SAFE_DELETE_ARRAY(m_piPromotionOverwrites);
		int iGoalSize = getNumPromotionOverwrites() + iNewItems;
		m_piPromotionOverwrites = new int[iGoalSize];
		for(int i = 0; i < iGoalSize; ++i)
		{
			m_piPromotionOverwrites[i] = tempArray[i];
			FAssertMsg(m_piPromotionOverwrites[i] < GC.getNumPromotionInfos() ,"Out of Bounds Array Melding");
			FAssertMsg(m_piPromotionOverwrites[i] > -1 ,"Out of Bounds Array Melding");
		}
		m_iNumPromotionOverwrites = iGoalSize;
		SAFE_DELETE_ARRAY(tempArray);
	}
	if (getObsoleteTech() == NO_TECH) m_iObsoleteTech = pClassInfo->getObsoleteTech();
	if (getControlPoints() == iDefault) m_iControlPoints = pClassInfo->getControlPoints();
	if (getCommandRange() == iDefault) m_iCommandRange = pClassInfo->getCommandRange();
	if (isZoneOfControl() == bDefault) m_bZoneOfControl = pClassInfo->isZoneOfControl();
	if (getAssetMultiplier() == iDefault) m_iAssetMultiplier = pClassInfo->getAssetMultiplier();
	if (getPowerMultiplier() == iDefault) m_iPowerMultiplier = pClassInfo->getPowerMultiplier();
	if (getIgnoreTerrainDamage() == NO_TERRAIN) m_iIgnoreTerrainDamage = pClassInfo->getIgnoreTerrainDamage();
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	m_PropertyManipulators.copyNonDefaults(pClassInfo->getPropertyManipulators(), pXML);
}

void CvPromotionInfo::copyNonDefaultsReadPass2(CvPromotionInfo* pClassInfo, CvXMLLoadUtility* pXML, bool bOver)
{
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default

	if (bOver || getPrereqPromotion() == iTextDefault) m_iPrereqPromotion = pClassInfo->getPrereqPromotion();
	if (bOver || getPrereqOrPromotion1() == iTextDefault) m_iPrereqOrPromotion1 = pClassInfo->getPrereqOrPromotion1();
	if (bOver || getPrereqOrPromotion2() == iTextDefault) m_iPrereqOrPromotion2 = pClassInfo->getPrereqOrPromotion2();
/************************************************************************************************/
/* Afforess					  Start		 Last Update: 3/8/10									*/
/*																							  */
/*																							  */
/************************************************************************************************/
	if(pClassInfo->getNumPromotionOverwrites() > 0)
	{
		int iGoalSize = bOver ? pClassInfo->getNumPromotionOverwrites() : getNumPromotionOverwrites() + pClassInfo->getNumPromotionOverwrites();
		int* tempArray = new int[iGoalSize];
		for(int i = 0; i < pClassInfo->getNumPromotionOverwrites(); ++i)
		{
			tempArray[i] = pClassInfo->getPromotionOverwrites(i);
		}
		int iOffset = pClassInfo->getNumPromotionOverwrites();
		for(int i = 0; i< getNumPromotionOverwrites(); ++i)
		{
			tempArray[i+iOffset] = getPromotionOverwrites(i);
		}
		SAFE_DELETE_ARRAY(m_piPromotionOverwrites);
		m_piPromotionOverwrites = new int[iGoalSize];
		for(int i = 0; i < iGoalSize; ++i)
		{
			m_piPromotionOverwrites[i] = tempArray[i];
			FAssertMsg(m_piPromotionOverwrites[i] < GC.getNumPromotionInfos() ,"Out of Bounds Array Melding");
			FAssertMsg(m_piPromotionOverwrites[i] >= 0 ,"Out of Bounds Array Melding");
		}
		m_iNumPromotionOverwrites = iGoalSize;
		SAFE_DELETE_ARRAY(tempArray);
	}

/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
}

void CvPromotionInfo::getCheckSum(unsigned int &iSum)
{//TBPROMO

	CheckSum(iSum, m_iPrereqPromotion);
	CheckSum(iSum, m_iPrereqOrPromotion1);
	CheckSum(iSum, m_iPrereqOrPromotion2);

	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iStateReligionPrereq);
	CheckSum(iSum, m_iMinEraType);
	CheckSum(iSum, m_iMaxEraType);
	CheckSum(iSum, m_iVisibilityChange);
	CheckSum(iSum, m_iMovesChange);
	CheckSum(iSum, m_iMoveDiscountChange);
	CheckSum(iSum, m_iAirRangeChange);
	CheckSum(iSum, m_iInterceptChange);
	CheckSum(iSum, m_iEvasionChange);
	CheckSum(iSum, m_iWithdrawalChange);
	CheckSum(iSum, m_iCargoChange);
	CheckSum(iSum, m_iSMCargoChange);
	CheckSum(iSum, m_iSMCargoVolumeChange);
	CheckSum(iSum, m_iSMCargoVolumeModifierChange);
	CheckSum(iSum, m_iCollateralDamageChange);
	CheckSum(iSum, m_iBombardRateChange);
	CheckSum(iSum, m_iFirstStrikesChange);
	CheckSum(iSum, m_iChanceFirstStrikesChange);
	CheckSum(iSum, m_iEnemyHealChange);
	CheckSum(iSum, m_iNeutralHealChange);
	CheckSum(iSum, m_iFriendlyHealChange);
	CheckSum(iSum, m_iSameTileHealChange);
	CheckSum(iSum, m_iAdjacentTileHealChange);
	CheckSum(iSum, m_iCombatPercent);
	CheckSum(iSum, m_iCityAttackPercent);
	CheckSum(iSum, m_iCityDefensePercent);
	CheckSum(iSum, m_iHillsAttackPercent);
	CheckSum(iSum, m_iHillsDefensePercent);
	CheckSum(iSum, m_iHillsWorkPercent);
	//ls612: Work rate modifiers
	CheckSum(iSum, m_iWorkRatePercent);
	CheckSum(iSum, m_iCommandType);
	CheckSum(iSum, m_iRevoltProtection);
	CheckSum(iSum, m_iCollateralDamageProtection);
	CheckSum(iSum, m_iPillageChange);
	CheckSum(iSum, m_iUpgradeDiscount);
	CheckSum(iSum, m_iExperiencePercent);
	CheckSum(iSum, m_iKamikazePercent);

	CheckSum(iSum, m_bLeader);
	CheckSum(iSum, m_bBlitz);
	CheckSum(iSum, m_bAmphib);
	CheckSum(iSum, m_bRiver);
	CheckSum(iSum, m_bEnemyRoute);
	CheckSum(iSum, m_bAlwaysHeal);
	CheckSum(iSum, m_bHillsDoubleMove);
	CheckSum(iSum, m_bImmuneToFirstStrikes);

	// Arrays

	CheckSum(iSum, m_piTerrainAttackPercent, GC.getNumTerrainInfos());
	CheckSum(iSum, m_piTerrainDefensePercent, GC.getNumTerrainInfos());
	CheckSum(iSum, m_piFeatureAttackPercent, GC.getNumFeatureInfos());
	CheckSum(iSum, m_piFeatureDefensePercent, GC.getNumFeatureInfos());
	CheckSum(iSum, m_piUnitCombatModifierPercent, GC.getNumUnitCombatInfos());
	CheckSum(iSum, m_piDomainModifierPercent, NUM_DOMAIN_TYPES);
	CheckSum(iSum, m_pbTerrainDoubleMove, GC.getNumTerrainInfos());
	CheckSum(iSum, m_pbFeatureDoubleMove, GC.getNumFeatureInfos());
	CheckSum(iSum, m_pbUnitCombat, GC.getNumUnitCombatInfos());
	//ls612: Terrain Work Modifiers
	CheckSum(iSum, m_piTerrainWorkPercent, GC.getNumTerrainInfos());
	CheckSum(iSum, m_piFeatureWorkPercent, GC.getNumFeatureInfos());

	CheckSum(iSum, m_bCanMovePeaks);
	CheckSum(iSum, m_bCanLeadThroughPeaks);
	//CheckSum(iSum, m_iNumPromotionOverwrites);
	CheckSum(iSum, m_piPromotionOverwrites, m_iNumPromotionOverwrites);

	CheckSum(iSum, m_iObsoleteTech);
	CheckSum(iSum, m_iControlPoints);
	CheckSum(iSum, m_iCommandRange);
	CheckSum(iSum, m_iAssetMultiplier);
	CheckSum(iSum, m_iPowerMultiplier);
	CheckSum(iSum, m_iIgnoreTerrainDamage);
	CheckSum(iSum, m_bZoneOfControl);
	m_PropertyManipulators.getCheckSum(iSum);

/*****************************************************************************************************/
/**  Author: TheLadiesOgre																		  **/
/**  Date: 16.09.2009																			   **/
/**  ModComp: TLOTags																			   **/
/**  Reason Added: New Tag Definition															   **/
/**  Notes: Adapted to C2C by Thunderbrd															**/
/*****************************************************************************************************/
	CheckSum(iSum, m_bDefensiveVictoryMove);
	CheckSum(iSum, m_bFreeDrop);
	CheckSum(iSum, m_bOffensiveVictoryMove);
//Team Project (2)
	CheckSum(iSum, m_bOneUp);
	CheckSum(iSum, m_bPillageCulture);
	CheckSum(iSum, m_bPillageEspionage);
	CheckSum(iSum, m_bPillageMarauder);
	CheckSum(iSum, m_bPillageOnMove);
	CheckSum(iSum, m_bPillageOnVictory);
	CheckSum(iSum, m_bPillageResearch);
	CheckSum(iSum, m_iAirCombatLimitChange);
	CheckSum(iSum, m_iCelebrityHappy);
	CheckSum(iSum, m_iCollateralDamageLimitChange);
	CheckSum(iSum, m_iCollateralDamageMaxUnitsChange);
	CheckSum(iSum, m_iCombatLimitChange);
	CheckSum(iSum, m_iExtraDropRange);
//Team Project (2)
	CheckSum(iSum, m_iSurvivorChance);
	CheckSum(iSum, m_iVictoryAdjacentHeal);
	CheckSum(iSum, m_iVictoryHeal);
//Team Project (2)
	CheckSum(iSum, m_iVictoryStackHeal);
/*****************************************************************************************************/
/**  TheLadiesOgre; 16.09.2009; TLOTags															 **/
/*****************************************************************************************************/

	//TB Combat Mods Begin  TB SubCombat Mod begin
	// Textual References
	CheckSum(iSum, m_ePromotionLine);
	CheckSum(iSum, m_eReplacesUnitCombat);
	CheckSum(iSum, m_eDomainCargoChange);
	CheckSum(iSum, m_eSpecialCargoChange);
	CheckSum(iSum, m_eSpecialCargoPrereq);
	CheckSum(iSum, m_eSMSpecialCargoChange);
	CheckSum(iSum, m_eSMSpecialCargoPrereq);
	CheckSum(iSum, m_eSMNotSpecialCargoChange);
	CheckSum(iSum, m_eSMNotSpecialCargoPrereq);
	CheckSum(iSum, m_eSetSpecialUnit);
	//integers
	CheckSum(iSum, m_iAttackCombatModifierChange);
	CheckSum(iSum, m_iDefenseCombatModifierChange);
	CheckSum(iSum, m_iPursuitChange);
	CheckSum(iSum, m_iEarlyWithdrawChange);
	CheckSum(iSum, m_iVSBarbsChange);
	CheckSum(iSum, m_iArmorChange);
	CheckSum(iSum, m_iPunctureChange);
	CheckSum(iSum, m_iOverrunChange);
	CheckSum(iSum, m_iRepelChange);
	CheckSum(iSum, m_iFortRepelChange);
	CheckSum(iSum, m_iRepelRetriesChange);
	CheckSum(iSum, m_iUnyieldingChange);
	CheckSum(iSum, m_iKnockbackChange);
	CheckSum(iSum, m_iKnockbackRetriesChange);
	CheckSum(iSum, m_iStrAdjperRndChange);
	CheckSum(iSum, m_iStrAdjperAttChange);
	CheckSum(iSum, m_iStrAdjperDefChange);
	CheckSum(iSum, m_iWithdrawAdjperAttChange);
	CheckSum(iSum, m_iUnnerveChange);
	CheckSum(iSum, m_iEncloseChange);
	CheckSum(iSum, m_iLungeChange);
	CheckSum(iSum, m_iDynamicDefenseChange);
	CheckSum(iSum, m_iStrengthChange);
	CheckSum(iSum, m_iLinePriority);
	CheckSum(iSum, m_iFortitudeChange);
	CheckSum(iSum, m_iDamageperTurn);
	CheckSum(iSum, m_iStrAdjperTurn);
	CheckSum(iSum, m_iWeakenperTurn);
	CheckSum(iSum, m_iFrontSupportPercentChange);
	CheckSum(iSum, m_iShortRangeSupportPercentChange);
	CheckSum(iSum, m_iMediumRangeSupportPercentChange);
	CheckSum(iSum, m_iLongRangeSupportPercentChange);
	CheckSum(iSum, m_iFlankSupportPercentChange);
	CheckSum(iSum, m_iDodgeModifierChange);
	CheckSum(iSum, m_iPrecisionModifierChange);
	CheckSum(iSum, m_iPowerShotsChange);
	CheckSum(iSum, m_iPowerShotCombatModifierChange);
	CheckSum(iSum, m_iPowerShotPunctureModifierChange);
	CheckSum(iSum, m_iPowerShotPrecisionModifierChange);
	CheckSum(iSum, m_iPowerShotCriticalModifierChange);
	CheckSum(iSum, m_iCriticalModifierChange);
	CheckSum(iSum, m_iEnduranceChange);
	CheckSum(iSum, m_iRoundStunProbChange);
	CheckSum(iSum, m_iPoisonProbabilityModifierChange);
//Team Project (3)
	CheckSum(iSum, m_iCaptureProbabilityModifierChange);
	CheckSum(iSum, m_iCaptureResistanceModifierChange);
//Team Project (4)
//WorkRateMod
	CheckSum(iSum, m_iHillsWorkModifierChange);
	CheckSum(iSum, m_iPeaksWorkModifierChange);

	CheckSum(iSum, m_iBreakdownChanceChange);
	CheckSum(iSum, m_iBreakdownDamageChange);
	CheckSum(iSum, m_iTauntChange);
	CheckSum(iSum, m_iMaxHPChange);
	CheckSum(iSum, m_iStrengthModifier);
	CheckSum(iSum, m_iQualityChange);
	CheckSum(iSum, m_iGroupChange);
	CheckSum(iSum, m_iLevelPrereq);
	CheckSum(iSum, m_iDamageModifierChange);
	CheckSum(iSum, m_iCostModifierChange);
	CheckSum(iSum, m_iRBombardDamageChange);
	CheckSum(iSum, m_iRBombardDamageLimitChange);
	CheckSum(iSum, m_iRBombardDamageMaxUnitsChange);
	CheckSum(iSum, m_iDCMBombRangeChange);
	CheckSum(iSum, m_iDCMBombAccuracyChange);
	CheckSum(iSum, m_iCombatModifierPerSizeMoreChange);
	CheckSum(iSum, m_iCombatModifierPerSizeLessChange);
	CheckSum(iSum, m_iCombatModifierPerVolumeMoreChange);
	CheckSum(iSum, m_iCombatModifierPerVolumeLessChange);
	CheckSum(iSum, m_iSelfHealModifier);
	CheckSum(iSum, m_iNumHealSupport);
	CheckSum(iSum, m_iExcileChange);
	CheckSum(iSum, m_iPassageChange);
	CheckSum(iSum, m_iNoNonOwnedCityEntryChange);
	CheckSum(iSum, m_iBarbCoExistChange);
	CheckSum(iSum, m_iBlendIntoCityChange);
	CheckSum(iSum, m_iUpgradeAnywhereChange);
	CheckSum(iSum, m_iInsidiousnessChange);
	CheckSum(iSum, m_iInvestigationChange);
	CheckSum(iSum, m_iAssassinChange);
	CheckSum(iSum, m_iStealthStrikesChange);
	CheckSum(iSum, m_iStealthCombatModifierChange);
	CheckSum(iSum, m_iStealthDefenseChange);
	CheckSum(iSum, m_iDefenseOnlyChange);
	CheckSum(iSum, m_iNoInvisibilityChange);
	CheckSum(iSum, m_iTrapDamageMax);
	CheckSum(iSum, m_iTrapDamageMin);
	CheckSum(iSum, m_iTrapComplexity);
	CheckSum(iSum, m_iNumTriggers);
	CheckSum(iSum, m_iTriggerBeforeAttackChange);
	CheckSum(iSum, m_iHiddenNationalityChange);
	CheckSum(iSum, m_iAnimalIgnoresBordersChange);
	CheckSum(iSum, m_iNoDefensiveBonusChange);
	CheckSum(iSum, m_iGatherHerdChange);
	CheckSum(iSum, m_iReligiousCombatModifierChange);
	//booleans
	CheckSum(iSum, m_bStampedeChange);
	CheckSum(iSum, m_bRemoveStampede);
	CheckSum(iSum, m_bOnslaughtChange);
	CheckSum(iSum, m_bEquipment);
	CheckSum(iSum, m_bAffliction);
	CheckSum(iSum, m_bParalyze);
	CheckSum(iSum, m_bMakesDamageCold);
	CheckSum(iSum, m_bMakesDamageNotCold);
	CheckSum(iSum, m_bAddsColdImmunity);
	CheckSum(iSum, m_bRemovesColdImmunity);
	CheckSum(iSum, m_bCritical);
	CheckSum(iSum, m_bAttackOnlyCitiesAdd);
	CheckSum(iSum, m_bAttackOnlyCitiesSubtract);
	CheckSum(iSum, m_bIgnoreNoEntryLevelAdd);
	CheckSum(iSum, m_bIgnoreNoEntryLevelSubtract);
	CheckSum(iSum, m_bIgnoreZoneofControlAdd);
	CheckSum(iSum, m_bIgnoreZoneofControlSubtract);
	CheckSum(iSum, m_bFliesToMoveAdd);
	CheckSum(iSum, m_bFliesToMoveSubtract);

	CheckSum(iSum, m_bAnyTerrainAttackPercent);
	CheckSum(iSum, m_bAnyTerrainDefensePercent);
	CheckSum(iSum, m_bAnyFeatureAttackPercent);
	CheckSum(iSum, m_bAnyFeatureDefensePercent);
	CheckSum(iSum, m_bAnyUnitCombatModifierPercent);
	CheckSum(iSum, m_bAnyDomainModifierPercent);
	CheckSum(iSum, m_bNoSelfHeal);
	CheckSum(iSum, m_bSetOnHNCapture);
	CheckSum(iSum, m_bSetOnInvestigated);
	CheckSum(iSum, m_bStatus);
	CheckSum(iSum, m_bPrereqNormInvisible);
	CheckSum(iSum, m_bPlotPrereqsKeepAfter);
	CheckSum(iSum, m_bRemoveAfterSet);
	CheckSum(iSum, m_bQuick);
	//CheckSum(iSum, m_bAnyAIWeightbyUnitCombatType);
	CheckSum(iSum, m_bZeroesXP);
	CheckSum(iSum, m_bForOffset);
	CheckSum(iSum, m_bCargoPrereq);
	CheckSum(iSum, m_bRBombardPrereq);
	//Arrays
	//CheckSum(iSum, m_piAIWeightbyUnitCombatTypes, GC.getNumUnitCombatInfos());
	// bool vectors without delayed resolution
	CheckSumC(iSum, m_aiSubCombatChangeTypes);
	CheckSumC(iSum, m_aiRemovesUnitCombatTypes);
	CheckSumC(iSum, m_aiOnGameOptions);
	CheckSumC(iSum, m_aiNotOnGameOptions);
	CheckSumC(iSum, m_aiFreetoUnitCombats);
	CheckSumC(iSum, m_aiNotOnUnitCombatTypes);
	CheckSumC(iSum, m_aiNotOnDomainTypes);
	CheckSumC(iSum, m_aiNoAutoEquiptoCombatClassTypes);
	CheckSumC(iSum, m_aiMapCategoryTypes);
	// bool vector with delayed resolution
	CheckSumC(iSum, m_aiCureAfflictionChangeTypes);
	CheckSumC(iSum, m_aiPrereqBonusTypes);
	CheckSumC(iSum, m_aiAddsBuildTypes);
	CheckSumC(iSum, m_aiNegatesInvisibilityTypes);
	CheckSumC(iSum, m_aiPrereqTerrainTypes);
	CheckSumC(iSum, m_aiPrereqFeatureTypes);
	CheckSumC(iSum, m_aiPrereqImprovementTypes);
	CheckSumC(iSum, m_aiPrereqPlotBonusTypes);
	CheckSumC(iSum, m_aiPrereqLocalBuildingClassTypes);
	CheckSumC(iSum, m_aiTrapSetWithPromotionTypes);
	CheckSumC(iSum, m_aiTrapImmunityUnitCombatTypes);
	CheckSumC(iSum, m_aiTargetUnitCombatTypes);
	// int vectors utilizing pairing without delayed resolution
	CheckSumC(iSum, m_aFlankingStrengthbyUnitCombatTypeChange);
	CheckSumC(iSum, m_aWithdrawOnTerrainTypesChange);
	CheckSumC(iSum, m_aWithdrawOnFeatureTypesChange);
	CheckSumC(iSum, m_aWithdrawVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aPursuitVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aRepelVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aKnockbackVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aPunctureVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aArmorVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aDodgeVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aPrecisionVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aCriticalVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aRoundStunVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aTrapDisableUnitCombatTypes);
	CheckSumC(iSum, m_aTrapAvoidanceUnitCombatTypes);
	CheckSumC(iSum, m_aTrapTriggerUnitCombatTypes);
	CheckSumC(iSum, m_aAidChanges);
//Team Project (4)
		//WorkRateMod
	CheckSumC(iSum, m_aTerrainWorkRateModifierChangeTypes);
	CheckSumC(iSum, m_aFeatureWorkRateModifierChangeTypes);
	CheckSumC(iSum, m_aBuildWorkRateModifierChangeTypes);
	CheckSumC(iSum, m_aVisibilityIntensityChangeTypes);
	CheckSumC(iSum, m_aInvisibilityIntensityChangeTypes);
	CheckSumC(iSum, m_aVisibilityIntensityRangeChangeTypes);
	// int vectors utilizing struct with delayed resolution
	int iNumElements;

	iNumElements = m_aAIWeightbyUnitCombatTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aAIWeightbyUnitCombatTypes[i].eUnitCombat);
		CheckSum(iSum, m_aAIWeightbyUnitCombatTypes[i].iModifier);
	}

	iNumElements = m_aAfflictionFortitudeChangeModifiers.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aAfflictionFortitudeChangeModifiers[i].ePromotionLine);
		CheckSum(iSum, m_aAfflictionFortitudeChangeModifiers[i].iModifier);
	}

	iNumElements = m_aAfflictOnAttackChangeTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aAfflictOnAttackChangeTypes[i].eAfflictionLine);
		CheckSum(iSum, m_aAfflictOnAttackChangeTypes[i].iProbabilityChange);
		CheckSum(iSum, m_aAfflictOnAttackChangeTypes[i].iMelee);
		CheckSum(iSum, m_aAfflictOnAttackChangeTypes[i].iDistance);
		CheckSum(iSum, m_aAfflictOnAttackChangeTypes[i].iImmediate);
	}

	iNumElements = m_aHealUnitCombatChangeTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aHealUnitCombatChangeTypes[i].eUnitCombat);
		CheckSum(iSum, m_aHealUnitCombatChangeTypes[i].iHeal);
		CheckSum(iSum, m_aHealUnitCombatChangeTypes[i].iAdjacentHeal);
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
	//TB Combat Mods End  TB SubCombat Mod end
}

CvPropertyManipulators* CvPromotionInfo::getPropertyManipulators()
{
	return &m_PropertyManipulators;
}

//======================================================================================================
//					CvMissionInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvMissionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvMissionInfo::CvMissionInfo() :
m_iTime(0),
m_bSound(false),
m_bTarget(false),
m_bBuild(false),
m_bVisible(false),
m_eEntityEvent(ENTITY_EVENT_NONE)
{
}

CvMissionInfo::CvMissionInfo(const char * szType) : CvHotkeyInfo(),
m_iTime(0),
m_bSound(false),
m_bTarget(false),
m_bBuild(false),
m_bVisible(false),
m_eEntityEvent(ENTITY_EVENT_NONE)
{
	m_szType = szType;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvMissionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvMissionInfo::~CvMissionInfo()
{
}

int CvMissionInfo::getTime() const
{
	return m_iTime;
}

bool CvMissionInfo::isSound() const
{
	return m_bSound;
}

bool CvMissionInfo::isTarget() const
{
	return m_bTarget;
}

bool CvMissionInfo::isBuild() const
{
	return m_bBuild;
}

bool CvMissionInfo::getVisible() const
{
	return m_bVisible;
}

const TCHAR* CvMissionInfo::getWaypoint() const
{
	return m_szWaypoint;
}

EntityEventTypes CvMissionInfo::getEntityEvent() const
{
	return m_eEntityEvent;
}

bool CvMissionInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTmp;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szWaypoint, L"Waypoint");
	pXML->GetOptionalChildXmlValByName(&m_iTime, L"iTime");
	pXML->GetOptionalChildXmlValByName(&m_bSound, L"bSound");
	pXML->GetOptionalChildXmlValByName(&m_bTarget, L"bTarget");
	pXML->GetOptionalChildXmlValByName(&m_bBuild, L"bBuild");
	pXML->GetOptionalChildXmlValByName(&m_bVisible, L"bVisible");

	if ( pXML->GetOptionalChildXmlValByName(szTmp, L"EntityEventType"))
		m_eEntityEvent = (EntityEventTypes)pXML->GetInfoClass(szTmp);

	return true;
}

void CvMissionInfo::copyNonDefaults(CvMissionInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvHotkeyInfo::copyNonDefaults(pClassInfo, pXML);

	if (getWaypoint() == cDefault) m_szWaypoint = pClassInfo->getWaypoint();
	if (getTime() == iDefault) m_iTime = pClassInfo->getTime();

	if (isSound() == bDefault) m_bSound = pClassInfo->isSound();
	if (isTarget() == bDefault) m_bTarget = pClassInfo->isTarget();
	if (isBuild() == bDefault) m_bBuild = pClassInfo->isBuild();
	if (getVisible() == bDefault) m_bVisible = pClassInfo->getVisible();

	if ( getEntityEvent() == ENTITY_EVENT_NONE ) m_eEntityEvent = pClassInfo->getEntityEvent();
}

//======================================================================================================
//					CvCommandInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCommandInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCommandInfo::CvCommandInfo() :
m_iAutomate(NO_AUTOMATE),
m_bConfirmCommand(false),
m_bVisible(false),
m_bAll(false)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCommandInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCommandInfo::~CvCommandInfo()
{
}

int CvCommandInfo::getAutomate() const
{
	return m_iAutomate;
}

void CvCommandInfo::setAutomate(int i)
{
	m_iAutomate = i;
}

bool CvCommandInfo::getConfirmCommand() const
{
	return m_bConfirmCommand;
}

bool CvCommandInfo::getVisible() const
{
	return m_bVisible;
}

bool CvCommandInfo::getAll() const
{
	return m_bAll;
}

bool CvCommandInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Automate");
	setAutomate(GC.getTypesEnum(szTextVal));

	pXML->GetOptionalChildXmlValByName(&m_bConfirmCommand, L"bConfirmCommand");
	pXML->GetOptionalChildXmlValByName(&m_bVisible, L"bVisible");
	pXML->GetOptionalChildXmlValByName(&m_bAll, L"bAll");

	return true;
}
void CvCommandInfo::copyNonDefaults(CvCommandInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvHotkeyInfo::copyNonDefaults(pClassInfo, pXML);

	if (getAutomate() == iTextDefault) setAutomate(pClassInfo->getAutomate());
	if (getConfirmCommand() == bDefault) m_bConfirmCommand = pClassInfo->getConfirmCommand();
	if (getVisible() == bDefault) m_bVisible = pClassInfo->getVisible();
	if (getAll() == bDefault) m_bAll = pClassInfo->getAll();
}

//======================================================================================================
//					CvAutomateInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvAutomateInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvAutomateInfo::CvAutomateInfo() :
m_iCommand(NO_COMMAND),
m_iAutomate(NO_AUTOMATE),
m_bConfirmCommand(false),
m_bVisible(false)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvAutomateInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvAutomateInfo::~CvAutomateInfo()
{
}

int CvAutomateInfo::getCommand() const
{
	return m_iCommand;
}

void CvAutomateInfo::setCommand(int i)
{
	m_iCommand = i;
}

int CvAutomateInfo::getAutomate() const
{
	return m_iAutomate;
}

void CvAutomateInfo::setAutomate(int i)
{
	m_iAutomate = i;
}

bool CvAutomateInfo::getConfirmCommand() const
{
	return m_bConfirmCommand;
}

bool CvAutomateInfo::getVisible() const
{
	return m_bVisible;
}

bool CvAutomateInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Command");
	setCommand(pXML->GetInfoClass(szTextVal));

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Automate");
	setAutomate(GC.getTypesEnum(szTextVal));

	pXML->GetOptionalChildXmlValByName(&m_bConfirmCommand, L"bConfirmCommand");
	pXML->GetOptionalChildXmlValByName(&m_bVisible, L"bVisible");

	return true;
}
void CvAutomateInfo::copyNonDefaults(CvAutomateInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvHotkeyInfo::copyNonDefaults(pClassInfo, pXML);

	if (getCommand() == iTextDefault) setCommand(pClassInfo->getCommand());
	if (getAutomate() == iTextDefault) setAutomate(pClassInfo->getAutomate());

	if (getConfirmCommand() == bDefault) m_bConfirmCommand = pClassInfo->getConfirmCommand();
	if (getVisible() == bDefault) m_bVisible = pClassInfo->getVisible();
}

//======================================================================================================
//					CvSpawnInfo
//======================================================================================================
CvSpawnInfo::CvSpawnInfo() :	m_eUnitType(NO_UNIT),
								m_ePrereqTechType(NO_TECH),
								m_eObsoleteTechType(NO_TECH),
								m_iPlayerType(-1),
								m_iTurns(-1),
								m_iGlobalTurns(-1),
								m_iMaxLocalDensity(0),
								m_iMaxAreaTotalDensity(0),
								m_iMaxAreaUnitDensity(0),
								m_iStartDate(-200000),
								m_iEndDate(50000),
								m_bTreatAsBarbarian(false),
								m_bNeutralOnly(true),
								m_bNotInView(false),
								m_bNoSpeedNormalization(false),
								m_iRateOverride(100),
								m_bHills(false),
								m_bFlatlands(false),
								m_bPeaks(false),
								m_bFreshWaterOnly(false),
								m_bLatitudeAbs(true),
								m_iMinLatitude(-90),
								m_iMaxLatitude(90),
								m_iMinLongitude(-180),
								m_iMaxLongitude(180),
								m_pExprSpawnCondition(NULL)
{
}

CvSpawnInfo::~CvSpawnInfo()
{
	SAFE_DELETE(m_pExprSpawnCondition);
}

bool CvSpawnInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	CvString szTextVal2;

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"UnitType");
	m_eUnitType = (UnitTypes)pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqTech");
	m_ePrereqTechType = (TechTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ObsoleteTech");
	m_eObsoleteTechType = (TechTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iPlayerType, L"PlayerType");

	pXML->GetOptionalChildXmlValByName(&m_iTurns, L"iTurns");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalTurns, L"iGlobalTurns", -1);
	pXML->GetOptionalChildXmlValByName(&m_iMaxLocalDensity, L"iMaxLocalDensity");
	pXML->GetOptionalChildXmlValByName(&m_iMaxAreaTotalDensity, L"iMaxAreaTotalDensity");
	pXML->GetOptionalChildXmlValByName(&m_iMaxAreaUnitDensity, L"iMaxAreaUnitDensity");
	pXML->GetOptionalChildXmlValByName(&m_iStartDate, L"iStartDate", -200000);
	pXML->GetOptionalChildXmlValByName(&m_iEndDate, L"iEndDate", 50000);
	pXML->GetOptionalChildXmlValByName(&m_bTreatAsBarbarian, L"bTreatAsBarbarian", true);
	pXML->GetOptionalChildXmlValByName(&m_bNeutralOnly, L"bNeutralOnly", true);
	pXML->GetOptionalChildXmlValByName(&m_bNoSpeedNormalization, L"bNoSpeedNormalization");
	pXML->GetOptionalChildXmlValByName(&m_bNotInView, L"bNotInView");
	pXML->GetOptionalChildXmlValByName(&m_bHills, L"bHills");
	pXML->GetOptionalChildXmlValByName(&m_bFlatlands, L"bFlatlands", true);
	pXML->GetOptionalChildXmlValByName(&m_bPeaks, L"bPeaks");
	pXML->GetOptionalChildXmlValByName(&m_bFreshWaterOnly, L"bFreshWaterOnly");
	pXML->GetOptionalChildXmlValByName(&m_bLatitudeAbs, L"bLatitudeAbs", true);
	pXML->GetOptionalChildXmlValByName(&m_iMinLatitude, L"iMinLatitude", -90);
	pXML->GetOptionalChildXmlValByName(&m_iMaxLatitude, L"iMaxLatitude", 90);
	pXML->GetOptionalChildXmlValByName(&m_iMinLongitude, L"iMinLongitude", -180);
	pXML->GetOptionalChildXmlValByName(&m_iMaxLongitude, L"iMaxLongitude", 180);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"rateOverrideDefineName");
	if ( szTextVal.GetLength() != 0 )
	{
		m_iRateOverride = std::max(GC.getDefineINT(szTextVal, 100),0);
	}

	if(pXML->TryMoveToXmlFirstChild(L"BonusTypes"))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"BonusType"))
			{
				do
				{
					int iBonusType;

					pXML->GetXmlVal(szTextVal);
					iBonusType = pXML->GetInfoClass(szTextVal);

					m_bonusTypes.push_back((BonusTypes)iBonusType);
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"TerrainTypes"))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"TerrainType"))
			{
				do
				{
					int iTerrainType;

					pXML->GetXmlVal(szTextVal);
					iTerrainType = pXML->GetInfoClass(szTextVal);

					m_terrainTypes.push_back((TerrainTypes)iTerrainType);
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"FeatureTypes"))
	{
		if (pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"FeatureType"))
			{
				do
				{
					int iFeatureType;

					pXML->GetXmlVal(szTextVal);
					iFeatureType = pXML->GetInfoClass(szTextVal);

					m_featureTypes.push_back((FeatureTypes)iFeatureType);
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"FeatureTerrainTypes"))
	{
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"TerrainType"))
			{
				do
				{
					int iTerrainType;

					pXML->GetXmlVal(szTextVal);
					iTerrainType = pXML->GetInfoClass(szTextVal);

					m_featureTerrainTypes.push_back((TerrainTypes)iTerrainType);
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"SpawnGroup"))
	{
		if (pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"UnitType"))
			{
				do
				{
					int iUnitType;

					pXML->GetXmlVal(szTextVal);
					iUnitType = pXML->GetInfoClass(szTextVal);

					m_spawnGroup.push_back((UnitTypes)iUnitType);
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpawnCondition"))
	{
		m_pExprSpawnCondition = BoolExpr::read(pXML);
		pXML->MoveToXmlParent();
	}

	return true;
}

void CvSpawnInfo::copyNonDefaults(CvSpawnInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (!m_pExprSpawnCondition)
	{
		m_pExprSpawnCondition = pClassInfo->m_pExprSpawnCondition;
		pClassInfo->m_pExprSpawnCondition = NULL;
	}

	// not yet implemented
}

BoolExpr* CvSpawnInfo::getSpawnCondition()
{
	return m_pExprSpawnCondition;
}

int	CvSpawnInfo::getNumBonuses()
{
	return m_bonusTypes.size();
}

int	CvSpawnInfo::getNumTerrains()
{
	return m_terrainTypes.size();
}

int	CvSpawnInfo::getNumFeatures()
{
	return m_featureTypes.size();
}

int	CvSpawnInfo::getNumFeatureTerrains()
{
	return m_featureTerrainTypes.size();
}

int	CvSpawnInfo::getNumSpawnGroup()
{
	return m_spawnGroup.size();
}

BonusTypes	CvSpawnInfo::getBonus(int index)
{
	FAssert(0 <= index);
	FAssert(index < (int)m_bonusTypes.size());
	return m_bonusTypes[index];
}

TerrainTypes CvSpawnInfo::getTerrain(int index)
{
	FAssert(0 <= index);
	FAssert(index < (int)m_terrainTypes.size());
	return m_terrainTypes[index];
}

FeatureTypes CvSpawnInfo::getFeature(int index)
{
	FAssert(0 <= index);
	FAssert(index < (int)m_featureTypes.size());
	return m_featureTypes[index];
}

TerrainTypes CvSpawnInfo::getFeatureTerrain(int index)
{
	FAssert(0 <= index);
	FAssert(index < (int)m_featureTerrainTypes.size());
	return m_featureTerrainTypes[index];
}

UnitTypes CvSpawnInfo::getSpawnGroup(int index)
{
	FAssert(0 <= index);
	FAssert(index < (int)m_spawnGroup.size());
	return m_spawnGroup[index];
}

int CvSpawnInfo::getTurnRate()
{
	return m_iTurns;
}

int CvSpawnInfo::getGlobalTurnRate()
{
	return m_iGlobalTurns;
}

int CvSpawnInfo::getMaxLocalDensity()
{
	return m_iMaxLocalDensity;
}

int CvSpawnInfo::getMaxAreaTotalDensity()
{
	return m_iMaxAreaTotalDensity;
}

int CvSpawnInfo::getMaxAreaUnitDensity()
{
	return m_iMaxAreaUnitDensity;
}

int CvSpawnInfo::getStartDate()
{
	return m_iStartDate;
}

int	CvSpawnInfo::getEndDate()
{
	return m_iEndDate;
}

UnitTypes	CvSpawnInfo::getUnitType()
{
	return m_eUnitType;
}

TechTypes	CvSpawnInfo::getPrereqTechType()
{
	return m_ePrereqTechType;
}

TechTypes	CvSpawnInfo::getObsoleteTechType()
{
	return m_eObsoleteTechType;
}

PlayerTypes	CvSpawnInfo::getPlayer()
{
	return (PlayerTypes)m_iPlayerType;
}

bool CvSpawnInfo::getTreatAsBarbarian()
{
	return m_bTreatAsBarbarian;
}

bool CvSpawnInfo::getNeutralOnly()
{
	if (GC.getUnitInfo(getUnitType()).isWildAnimal() && GC.getGameINLINE().isOption(GAMEOPTION_DANGEROUS_WILDLIFE))
	{
		return false;
	}
	return m_bNeutralOnly;
}

bool CvSpawnInfo::getNoSpeedNormalization()
{
	return m_bNoSpeedNormalization;
}

bool CvSpawnInfo::getNotInView()
{
	return m_bNotInView;
}

bool CvSpawnInfo::getHills()
{
	return m_bHills;
}

bool CvSpawnInfo::getFlatlands()
{
	return m_bFlatlands;
}

bool CvSpawnInfo::getPeaks()
{
	return m_bPeaks;
}

bool CvSpawnInfo::getFreshWaterOnly()
{
	return m_bFreshWaterOnly;
}

bool CvSpawnInfo::getLatitudeAbs()
{
	return m_bLatitudeAbs;
}

int CvSpawnInfo::getMinLatitude()
{
	return m_iMinLatitude;
}

int CvSpawnInfo::getMaxLatitude()
{
	return m_iMaxLatitude;
}

int CvSpawnInfo::getMinLongitude()
{
	return m_iMinLongitude;
}

int CvSpawnInfo::getMaxLongitude()
{
	return m_iMaxLongitude;
}

int CvSpawnInfo::getRateOverride()
{
	return m_iRateOverride;
}

void CvSpawnInfo::getCheckSum(unsigned int &iSum)
{
	CheckSum(iSum, m_eUnitType);
	CheckSum(iSum, m_ePrereqTechType);
	CheckSum(iSum, m_eObsoleteTechType);
	CheckSum(iSum, m_iPlayerType);
	CheckSum(iSum, m_iTurns);
	CheckSum(iSum, m_iGlobalTurns);
	CheckSum(iSum, m_iMaxLocalDensity);
	CheckSum(iSum, m_iMaxAreaTotalDensity);
	CheckSum(iSum, m_iMaxAreaUnitDensity);
	CheckSum(iSum, m_iStartDate);
	CheckSum(iSum, m_iEndDate);
	CheckSum(iSum, m_bTreatAsBarbarian);
	CheckSum(iSum, m_bNeutralOnly);
	CheckSum(iSum, m_bNotInView);
	CheckSum(iSum, m_bNoSpeedNormalization);
	CheckSum(iSum, m_bHills);
	CheckSum(iSum, m_bFlatlands);
	CheckSum(iSum, m_bPeaks);
	CheckSum(iSum, m_bFreshWaterOnly);

	CheckSum(iSum, m_bLatitudeAbs);
	CheckSum(iSum, m_iMinLatitude);
	CheckSum(iSum, m_iMaxLatitude);
	CheckSum(iSum, m_iMinLongitude);
	CheckSum(iSum, m_iMaxLongitude);

	CheckSum(iSum, m_iRateOverride);
	CheckSumC(iSum, m_bonusTypes);
	CheckSumC(iSum, m_terrainTypes);
	CheckSumC(iSum, m_featureTypes);
	CheckSumC(iSum, m_featureTerrainTypes);
	CheckSumC(iSum, m_spawnGroup);

	if (m_pExprSpawnCondition)
	{
		m_pExprSpawnCondition->getCheckSum(iSum);
	}
}


//======================================================================================================
//					CvUnitFormationInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvUnitFormationInfo()
//
//  \brief		Default Constructor
//
//------------------------------------------------------------------------------------------------------
CvUnitFormationInfo::CvUnitFormationInfo()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvUnitFormationInfo()
//
//  \brief		Destructor
//
//------------------------------------------------------------------------------------------------------
CvUnitFormationInfo::~CvUnitFormationInfo()
{
}

const TCHAR* CvUnitFormationInfo::getFormationType() const
{
	return m_szFormationType;
}

int CvUnitFormationInfo::getNumEventTypes() const
{
	return m_vctEventTypes.size();
}

const EntityEventTypes &CvUnitFormationInfo::getEventType(int index) const
{
	return m_vctEventTypes[index];
}
const std::vector<EntityEventTypes> & CvUnitFormationInfo::getEventTypes() const
{
	return m_vctEventTypes;
}

int CvUnitFormationInfo::getNumUnitEntries() const
{
	return m_vctUnitEntries.size();
}

const CvUnitEntry &CvUnitFormationInfo::getUnitEntry(int index) const
{
	return m_vctUnitEntries[index];
}

void CvUnitFormationInfo::addUnitEntry(const CvUnitEntry &unitEntry)
{
	m_vctUnitEntries.push_back(unitEntry);
}

int CvUnitFormationInfo::getNumGreatUnitEntries() const
{
	return m_vctGreatUnitEntries.size();
}

const CvUnitEntry &CvUnitFormationInfo::getGreatUnitEntry(int index) const
{
	return m_vctGreatUnitEntries[index];
}

int CvUnitFormationInfo::getNumSiegeUnitEntries() const
{
	return m_vctSiegeUnitEntries.size();
}

const CvUnitEntry &CvUnitFormationInfo::getSiegeUnitEntry(int index) const
{
	return m_vctSiegeUnitEntries[index];
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvUnitFormationInfo()
//
//  \brief		Reads from XML
//
//------------------------------------------------------------------------------------------------------
bool CvUnitFormationInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	int iIndex;
	bool bNextSibling;

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(m_szFormationType, L"FormationType");

	if (pXML->TryMoveToXmlFirstChild(L"EventMaskList" ))
	{
		if ( pXML->TryMoveToXmlFirstChild() )
		{
			pXML->GetXmlVal( szTextVal );
			do
			{
				iIndex = pXML->GetInfoClass(szTextVal);
				if ( iIndex != -1 )
					m_vctEventTypes.push_back( (EntityEventTypes)iIndex );
				bNextSibling = pXML->GetNextXmlVal( szTextVal );
			}
			while( bNextSibling );
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	// Read the entries
	if (pXML->TryMoveToXmlFirstChild(L"UnitEntry" ) )
	{
		do
		{
			CvUnitEntry unitEntry;
			pXML->GetChildXmlValByName(szTextVal, L"UnitEntryType");
			if (pXML->TryMoveToXmlFirstChild(L"Position" ) )
			{
				pXML->GetChildXmlValByName( &unitEntry.m_position.x, L"x");
				pXML->GetChildXmlValByName( &unitEntry.m_position.y, L"y");
				pXML->MoveToXmlParent();
			}
			pXML->GetChildXmlValByName( &unitEntry.m_fRadius, L"PositionRadius");
			pXML->GetChildXmlValByName( &unitEntry.m_fFacingDirection, L"Direction");
			pXML->GetChildXmlValByName( &unitEntry.m_fFacingVariance, L"DirVariation");

			if(szTextVal.CompareNoCase("Unit") == 0)
				m_vctUnitEntries.push_back(unitEntry);
			else if(szTextVal.CompareNoCase("General") == 0)
				m_vctGreatUnitEntries.push_back(unitEntry);
			else if(szTextVal.CompareNoCase("Siege") == 0)
				m_vctSiegeUnitEntries.push_back(unitEntry);
			else
			{
				//FAssertMsg(false, "[Jason] Unknown unit formation entry type.");
			}
		}
		while ( pXML->TryMoveToXmlNextSibling(L"UnitEntry"));
		pXML->MoveToXmlParent();
	}

//	FAssertMsg(m_vctGreatUnitEntries.size() > 0, "[Jason] Formation missing great general entry.");
//	FAssertMsg(m_vctSiegeUnitEntries.size() > 0, "[Jason] Formation missing siege tower entry.");

	return true;
}
void CvUnitFormationInfo::copyNonDefaults(CvUnitFormationInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getFormationType() == cDefault) m_szFormationType = pClassInfo->getFormationType();

	for ( int i = 0; i < pClassInfo->getNumEventTypes(); i++ )
	{
		m_vctEventTypes.push_back( pClassInfo->getEventType(i) );
	}

	for ( int i = 0; i < pClassInfo->getNumUnitEntries(); i++ )
	{
		m_vctUnitEntries.push_back( pClassInfo->getUnitEntry(i) );
	}

	for ( int i = 0; i < pClassInfo->getNumGreatUnitEntries(); i++ )
	{
		m_vctGreatUnitEntries.push_back( pClassInfo->getGreatUnitEntry(i) );
	}

	for ( int i = 0; i < pClassInfo->getNumSiegeUnitEntries(); i++ )
	{
		m_vctSiegeUnitEntries.push_back( pClassInfo->getSiegeUnitEntry(i) );
	}
}


//======================================================================================================
//					CvSpecialUnitInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSpecialUnitInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialUnitInfo::CvSpecialUnitInfo() :
m_bValid(false),
m_bCityLoad(false),
m_bSMLoadSame(false),
m_iCombatPercent(0),
m_iWithdrawalChange(0),
m_iPursuitChange(0),
m_pbCarrierUnitAITypes(NULL)/*,*/
//m_piProductionTraits(NULL)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSpecialUnitInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialUnitInfo::~CvSpecialUnitInfo()
{
	SAFE_DELETE_ARRAY(m_pbCarrierUnitAITypes);
	//SAFE_DELETE_ARRAY(m_piProductionTraits);
}

bool CvSpecialUnitInfo::isValid() const
{
	return m_bValid;
}

bool CvSpecialUnitInfo::isCityLoad() const
{
	return m_bCityLoad;
}

bool CvSpecialUnitInfo::isSMLoadSame() const
{
	return m_bSMLoadSame;
}

int CvSpecialUnitInfo::getCombatPercent() const
{
	return m_iCombatPercent;
}

int CvSpecialUnitInfo::getWithdrawalChange() const
{
	return m_iWithdrawalChange;
}

int CvSpecialUnitInfo::getPursuitChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_FIGHT_OR_FLIGHT))
	{
		return 0;
	}
	return m_iPursuitChange;
}

// Arrays

bool CvSpecialUnitInfo::isCarrierUnitAIType(int i) const
{
	FAssertMsg(i < NUM_UNITAI_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbCarrierUnitAITypes ? m_pbCarrierUnitAITypes[i] : false;
}

//int CvSpecialUnitInfo::getProductionTraits(int i) const
//{
//	FAssertMsg(i < GC.getNumTraitInfos(), "Index out of bounds");
//	FAssertMsg(i > -1, "Index out of bounds");
//	return m_piProductionTraits ? m_piProductionTraits[i] : 0;
//}

bool CvSpecialUnitInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_bValid, L"bValid");
	pXML->GetOptionalChildXmlValByName(&m_bCityLoad, L"bCityLoad");
	pXML->GetOptionalChildXmlValByName(&m_bSMLoadSame, L"bSMLoadSame");
	pXML->GetOptionalChildXmlValByName(&m_iCombatPercent, L"iCombatPercent");
	pXML->GetOptionalChildXmlValByName(&m_iWithdrawalChange, L"iWithdrawalChange");
	pXML->GetOptionalChildXmlValByName(&m_iPursuitChange, L"iPursuitChange");

	pXML->SetVariableListTagPair(&m_pbCarrierUnitAITypes, L"CarrierUnitAITypes", NUM_UNITAI_TYPES);

	//pXML->SetVariableListTagPair(&m_piProductionTraits, L"ProductionTraits", GC.getNumTraitInfos());

	return true;
}

void CvSpecialUnitInfo::copyNonDefaults(CvSpecialUnitInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (isValid() == bDefault) m_bValid = pClassInfo->isValid();
	if (isCityLoad() == bDefault) m_bCityLoad = pClassInfo->isCityLoad();
	if (isSMLoadSame() == bDefault) m_bSMLoadSame = pClassInfo->isSMLoadSame();
	if (getCombatPercent() == iDefault) m_iCombatPercent = pClassInfo->getCombatPercent();
	if (getWithdrawalChange() == iDefault) m_iWithdrawalChange = pClassInfo->getWithdrawalChange();
	if (getPursuitChange(true) == iDefault) m_iPursuitChange = pClassInfo->getPursuitChange(true);

	for ( int i = 0; i < NUM_UNITAI_TYPES; i++ )
	{
		if ( isCarrierUnitAIType(i) == bDefault && pClassInfo->isCarrierUnitAIType(i) != bDefault)
		{
			if ( NULL == m_pbCarrierUnitAITypes )
			{
				CvXMLLoadUtility::InitList(&m_pbCarrierUnitAITypes,NUM_UNITAI_TYPES,bDefault);
			}
			m_pbCarrierUnitAITypes[i] = pClassInfo->isCarrierUnitAIType(i);
		}
	}

	//for ( int i = 0; i < GC.getNumTraitInfos(); i++ )
	//{
	//	if ( (m_piProductionTraits == NULL || m_piProductionTraits[i] == iDefault) &&
	//		 pClassInfo->getProductionTraits(i) != iDefault)
	//	{
	//		if ( NULL == m_piProductionTraits )
	//		{
	//			CvXMLLoadUtility::InitList(&m_piProductionTraits,GC.getNumTraitInfos(),iDefault);
	//		}
	//		m_piProductionTraits[i] = pClassInfo->getProductionTraits(i);
	//	}
	//}
}

void CvSpecialUnitInfo::getCheckSum(unsigned int &iSum)
{
	CheckSum(iSum, m_bValid);
	CheckSum(iSum, m_bSMLoadSame);
	CheckSum(iSum, m_iCombatPercent);
	CheckSum(iSum, m_iWithdrawalChange);
	CheckSum(iSum, m_iPursuitChange);

	CheckSum(iSum, m_pbCarrierUnitAITypes, NUM_UNITAI_TYPES);
	//CheckSum(iSum, m_piProductionTraits, GC.getNumTraitInfos());
}

//======================================================================================================
//					CvUnitClassInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvUnitClassInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvUnitClassInfo::CvUnitClassInfo() :
m_iMaxGlobalInstances(0),
m_iMaxTeamInstances(0),
m_iMaxPlayerInstances(0),
//TB Unlimited National Units Mod
m_bUnlimitedException(false),
//TB Unlimited National Units End
m_iInstanceCostModifier(0),
m_iDefaultUnitIndex(NO_UNIT)
/************************************************************************************************/
/* Afforess					  Start		 12/23/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
,m_bUnique(false)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvUnitClassInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvUnitClassInfo::~CvUnitClassInfo()
{
}

int CvUnitClassInfo::getMaxGlobalInstances() const
{
	return m_iMaxGlobalInstances;
}

int CvUnitClassInfo::getMaxTeamInstances() const
{
	return m_iMaxTeamInstances;
}

int CvUnitClassInfo::getMaxPlayerInstances() const
{
	return m_iMaxPlayerInstances;
}

//TB Unlimited National Units Mod
bool CvUnitClassInfo::isUnlimitedException() const
{
	return m_bUnlimitedException;
}
//TB Unlimited National Units End

int CvUnitClassInfo::getInstanceCostModifier() const
{
	return m_iInstanceCostModifier;
}

int CvUnitClassInfo::getDefaultUnitIndex() const
{
	return m_iDefaultUnitIndex;
}

void CvUnitClassInfo::setDefaultUnitIndex(int i)
{
	m_iDefaultUnitIndex = i;
}
/************************************************************************************************/
/* Afforess					  Start		 12/23/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
bool CvUnitClassInfo::isUnique() const
{
	return m_bUnique;
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

int CvUnitClassInfo::getDefaultUnitIndexVector()
{
	return m_aszExtraXMLforPass3.size();
}
CvString CvUnitClassInfo::getDefaultUnitIndexVectorElement(int i)
{
	return m_aszExtraXMLforPass3[i];
}

bool CvUnitClassInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iMaxGlobalInstances, L"iMaxGlobalInstances", -1);
	pXML->GetOptionalChildXmlValByName(&m_iMaxTeamInstances, L"iMaxTeamInstances", -1);
	pXML->GetOptionalChildXmlValByName(&m_iMaxPlayerInstances, L"iMaxPlayerInstances", -1);
	pXML->GetOptionalChildXmlValByName(&m_bUnlimitedException, L"bUnlimitedException");
	pXML->GetOptionalChildXmlValByName(&m_iInstanceCostModifier, L"iInstanceCostModifier");
/************************************************************************************************/
/* Afforess					  Start		 12/23/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_bUnique, L"bUnique");
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"DefaultUnit");
	m_aszExtraXMLforPass3.push_back(szTextVal);

	return true;
}

void CvUnitClassInfo::copyNonDefaults(CvUnitClassInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getMaxGlobalInstances() == -1) m_iMaxGlobalInstances = pClassInfo->getMaxGlobalInstances();
	if (getMaxTeamInstances() == -1) m_iMaxTeamInstances = pClassInfo->getMaxTeamInstances();
	if (getMaxPlayerInstances() == -1) m_iMaxPlayerInstances = pClassInfo->getMaxPlayerInstances();
	if (isUnlimitedException() == bDefault) m_bUnlimitedException = pClassInfo->isUnlimitedException();
	if (getInstanceCostModifier() == iDefault) m_iInstanceCostModifier = pClassInfo->getInstanceCostModifier();
/************************************************************************************************/
/* Afforess					  Start		 12/23/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
	if (isUnique() == bDefault) m_bUnique = pClassInfo->isUnique();
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	for ( int i = 0; i < pClassInfo->getDefaultUnitIndexVector(); i++ )
	{
		m_aszExtraXMLforPass3.push_back(pClassInfo->getDefaultUnitIndexVectorElement(i));
	}
}

bool CvUnitClassInfo::readPass3()
{
	if (m_aszExtraXMLforPass3.size() < 1)
	{
		FAssert(false);
		return false;
	}

/************************************************************************************************/
/* XMLCOPY								 10/12/07								MRGENIE	  */
/*																							  */
/* Assuming the modder purposly added an entry to this tag, we want to take the last enty set   */
/* by the modder and not the first as set by firaxis											*/
/************************************************************************************************/
/*
	m_iDefaultUnitIndex = GC.getInfoTypeForString(m_aszExtraXMLforPass3[0]);
*/
	int iSize = m_aszExtraXMLforPass3.size();
	int iTextDefault = -1;
	for ( int i = 0; i < iSize; i++ )
	{
		if ( GC.getInfoTypeForString(m_aszExtraXMLforPass3[i], true) != iTextDefault)
		{
			m_iDefaultUnitIndex = GC.getInfoTypeForString(m_aszExtraXMLforPass3[i]);
			break;
		}
	}
	m_aszExtraXMLforPass3.clear();

	return true;
}

void CvUnitClassInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iMaxGlobalInstances);
	CheckSum(iSum, m_iMaxTeamInstances);
	CheckSum(iSum, m_iMaxPlayerInstances);
	CheckSum(iSum, m_bUnlimitedException);
	CheckSum(iSum, m_iInstanceCostModifier);
	CheckSum(iSum, m_iDefaultUnitIndex);

	CheckSum(iSum, m_bUnique);
}

//======================================================================================================
//					CvBuildInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvBuildInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvBuildInfo::CvBuildInfo() :
m_iTime(0),
m_iCost(0),
m_iTechPrereq(NO_TECH),
m_iImprovement(NO_IMPROVEMENT),
m_iRoute(NO_ROUTE),
m_iEntityEvent(ENTITY_EVENT_NONE),
m_iMissionType(NO_MISSION),
m_bKill(false),
m_paiFeatureTech(NULL),
m_paiFeatureTime(NULL),
m_paiFeatureProduction(NULL),
m_pabFeatureRemove(NULL)
/************************************************************************************************/
/* Afforess					  Start		 05/25/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
,m_pabNoTechCanRemoveWithNoProductionGain(NULL)
,m_iTerrainChange(NO_TERRAIN)
,m_iFeatureChange(NO_FEATURE)
,m_iObsoleteTech(NO_TECH)
,m_bMine(false)
,m_bDisabled(false)
,m_bHideObsoleteExempt(false)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvBuildInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvBuildInfo::~CvBuildInfo()
{
	SAFE_DELETE_ARRAY(m_paiFeatureTech);
	SAFE_DELETE_ARRAY(m_paiFeatureTime);
	SAFE_DELETE_ARRAY(m_paiFeatureProduction);
	SAFE_DELETE_ARRAY(m_pabFeatureRemove);
/************************************************************************************************/
/* Afforess					  Start		 05/25/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	SAFE_DELETE_ARRAY(m_pabNoTechCanRemoveWithNoProductionGain);
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	for (int i=0; i<(int)m_aiPrereqBonusTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aiPrereqBonusTypes[i]));
	}

	for (int i=0; i<(int)m_aTerrainStructs.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aTerrainStructs[i]));
	}

	for (int i=0; i<(int)m_aPlaceBonusTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aPlaceBonusTypes[i]));
	}
}

int CvBuildInfo::getTime() const
{
	return m_iTime;
}

int CvBuildInfo::getCost() const
{
	return m_iCost;
}

int CvBuildInfo::getTechPrereq() const
{
	return m_iTechPrereq;
}

int CvBuildInfo::getImprovement() const
{
	return m_iImprovement;
}

int CvBuildInfo::getRoute() const
{
	return m_iRoute;
}

/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/13/09														*/
/*																							  */
/*																							  */
/************************************************************************************************/
int CvBuildInfo::getTerrainChange() const
{
	return m_iTerrainChange;
}

int CvBuildInfo::getFeatureChange() const
{
	return m_iFeatureChange;
}

int CvBuildInfo::getObsoleteTech() const
{
	return m_iObsoleteTech;
}

bool CvBuildInfo::isMine() const
{
	return m_bMine;
}

bool CvBuildInfo::isDisabled() const
{
	return m_bDisabled;
}

bool CvBuildInfo::isHideObsoleteExempt() const
{
	return m_bHideObsoleteExempt;
}

void CvBuildInfo::setDisabled(bool bNewVal)
{
	m_bDisabled = bNewVal;
}

bool CvBuildInfo::isNoTechCanRemoveWithNoProductionGain(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pabNoTechCanRemoveWithNoProductionGain ? m_pabNoTechCanRemoveWithNoProductionGain[i] : false;
}
/************************************************************************************************/
/* JOOYO_ADDON						  END													 */
/************************************************************************************************/

int CvBuildInfo::getEntityEvent() const
{
	return m_iEntityEvent;
}

int CvBuildInfo::getMissionType() const
{
	return m_iMissionType;
}

void CvBuildInfo::setMissionType(int iNewType)
{
	m_iMissionType = iNewType;
}

bool CvBuildInfo::isKill() const
{
	return m_bKill;
}

// Arrays

int CvBuildInfo::getFeatureTech(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiFeatureTech ? m_paiFeatureTech[i] : -1;
}

int CvBuildInfo::getFeatureTime(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiFeatureTime ? m_paiFeatureTime[i] : 0;
}

int CvBuildInfo::getFeatureProduction(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiFeatureProduction ? m_paiFeatureProduction[i] : 0;
}

bool CvBuildInfo::isFeatureRemove(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pabFeatureRemove ? m_pabFeatureRemove[i] : false;
}

int CvBuildInfo::getPrereqBonusType(int i) const
{
	return m_aiPrereqBonusTypes[i];
}

int CvBuildInfo::getNumPrereqBonusTypes() const
{
	return (int)m_aiPrereqBonusTypes.size();
}

bool CvBuildInfo::isPrereqBonusType(int i)
{
	if (find(m_aiPrereqBonusTypes.begin(), m_aiPrereqBonusTypes.end(), i) == m_aiPrereqBonusTypes.end())
	{
		return false;
	}
	return true;
}

int CvBuildInfo::getMapCategoryType(int i) const
{
	return m_aiMapCategoryTypes[i];
}

int CvBuildInfo::getNumMapCategoryTypes() const
{
	return (int)m_aiMapCategoryTypes.size();
}

bool CvBuildInfo::isMapCategoryType(int i)
{
	FAssert (i > -1 && i < GC.getNumMapCategoryInfos()); // do not include this line if for delayed resolution
	if (find(m_aiMapCategoryTypes.begin(), m_aiMapCategoryTypes.end(), i) == m_aiMapCategoryTypes.end())
	{
		return false;
	}
	return true;
}

int CvBuildInfo::getNumTerrainStructs() const
{
	return (int)m_aTerrainStructs.size();
}

TerrainStructs& CvBuildInfo::getTerrainStruct(int iIndex)
{
	FAssertMsg(iIndex < (int)m_aTerrainStructs.size(), "Index out of bounds");
	FAssertMsg(iIndex > -1, "Index out of bounds");
	return m_aTerrainStructs[iIndex];
}

int CvBuildInfo::getNumPlaceBonusTypes() const
{
	return (int)m_aPlaceBonusTypes.size();
}

PlaceBonusTypes& CvBuildInfo::getPlaceBonusType(int iIndex)
{
	return m_aPlaceBonusTypes[iIndex];
}

bool CvBuildInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	CvString szTextVal2;
	CvString szTextVal3;
	CvString szTextVal4;
	CvString szTextVal5;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqTech");
	m_iTechPrereq = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iTime, L"iTime");
	pXML->GetOptionalChildXmlValByName(&m_iCost, L"iCost");
	pXML->GetOptionalChildXmlValByName(&m_bKill, L"bKill");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ObsoleteTech");
	m_iObsoleteTech = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ImprovementType");
	m_iImprovement = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"RouteType");
	m_iRoute = pXML->GetInfoClass(szTextVal);
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/13/09														*/
/*																							  */
/*																							  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(szTextVal, L"TerrainChange");
	m_iTerrainChange = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"FeatureChange");
	m_iFeatureChange = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_bMine, L"bMine");
	pXML->GetOptionalChildXmlValByName(&m_bHideObsoleteExempt, L"bHideObsoleteExempt");
/************************************************************************************************/
/* JOOYO_ADDON						  END													 */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(szTextVal, L"EntityEvent");
	m_iEntityEvent = pXML->GetInfoClass(szTextVal);
/************************************************************************************************/
/* Afforess					  Start		 05/25/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
/*
	pXML->SetFeatureStruct(&m_paiFeatureTech, &m_paiFeatureTime, &m_paiFeatureProduction, &m_pabFeatureRemove);
*/
	pXML->SetFeatureStruct(&m_paiFeatureTech, &m_paiFeatureTime, &m_paiFeatureProduction, &m_pabFeatureRemove, &m_pabNoTechCanRemoveWithNoProductionGain);
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	pXML->SetOptionalIntVectorWithDelayedResolution(m_aiPrereqBonusTypes, L"PrereqBonusTypes");
	pXML->SetOptionalIntVector(&m_aiMapCategoryTypes, L"MapCategoryTypes");

	if(pXML->TryMoveToXmlFirstChild(L"TerrainStructs"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"TerrainStruct" );
		m_aTerrainStructs.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"TerrainStruct"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"TerrainType");
					m_aTerrainStructs[i].eTerrain = (TerrainTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"PrereqTech");
					m_aTerrainStructs[i].ePrereqTech = (TechTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aTerrainStructs[i].iTime), L"iTime");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"TerrainStruct"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"PlaceBonusTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"PlaceBonusType" );
		m_aPlaceBonusTypes.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"PlaceBonusType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"BonusType");
					m_aPlaceBonusTypes[i].eBonus = (BonusTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aPlaceBonusTypes[i].iProbability), L"iProbability");
					pXML->GetChildXmlValByName(&(m_aPlaceBonusTypes[i].bRequiresAccess), L"bRequiresAccess");
					pXML->GetChildXmlValByName(szTextVal, L"TerrainType");
					m_aPlaceBonusTypes[i].ePrereqTerrain = (TerrainTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"FeatureType");
					m_aPlaceBonusTypes[i].ePrereqFeature = (FeatureTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"MapCategoryType");
					m_aPlaceBonusTypes[i].ePrereqMapCategory = (MapCategoryTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"TechType");
					m_aPlaceBonusTypes[i].ePrereqTech = (TechTypes)pXML->GetInfoClass(szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"PlaceBonusType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	return true;
}

void CvBuildInfo::copyNonDefaults(CvBuildInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvHotkeyInfo::copyNonDefaults(pClassInfo, pXML);

	if (getTechPrereq() == iTextDefault) m_iTechPrereq = pClassInfo->getTechPrereq();

	if (getTime() == iDefault) m_iTime = pClassInfo->getTime();
	if (getCost() == iDefault) m_iCost = pClassInfo->getCost();
	if (isKill() == bDefault) m_bKill = pClassInfo->isKill();

	if (getImprovement() == iTextDefault) m_iImprovement = pClassInfo->getImprovement();
	if (getRoute() == iTextDefault) m_iRoute = pClassInfo->getRoute();
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/13/09														*/
/*																							  */
/*																							  */
/************************************************************************************************/
	if (getTerrainChange() == iTextDefault) m_iTerrainChange = pClassInfo->getTerrainChange();
	if (getFeatureChange() == iTextDefault) m_iFeatureChange = pClassInfo->getFeatureChange();
	//TB Team Project
	if (getObsoleteTech() == iTextDefault) m_iObsoleteTech = pClassInfo->getObsoleteTech();

	if (isMine() == bDefault) m_bMine = pClassInfo->isMine();
	if (isHideObsoleteExempt() == bDefault) m_bHideObsoleteExempt = pClassInfo->isHideObsoleteExempt();
/************************************************************************************************/
/* JOOYO_ADDON						  END													 */
/************************************************************************************************/
	if (getEntityEvent() == iTextDefault) m_iEntityEvent = pClassInfo->getEntityEvent();

	for ( int i = 0; i < GC.getNumFeatureInfos(); i++)
	{
		if ( m_paiFeatureTech[i] == iTextDefault )
		{
			m_paiFeatureTech[i] = pClassInfo->getFeatureTech(i);
			m_paiFeatureTime[i] = pClassInfo->getFeatureTime(i);
			m_paiFeatureProduction[i] = pClassInfo->getFeatureProduction(i);
			m_pabFeatureRemove[i] = pClassInfo->isFeatureRemove(i);
			m_pabNoTechCanRemoveWithNoProductionGain[i] = pClassInfo->isNoTechCanRemoveWithNoProductionGain(i);
		}
	}

	if (getNumPrereqBonusTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiPrereqBonusTypes, pClassInfo->m_aiPrereqBonusTypes);
	}

	if (getNumMapCategoryTypes() == 0)
	{
		m_aiMapCategoryTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumMapCategoryTypes(); i++)
		{
			m_aiMapCategoryTypes.push_back(pClassInfo->getMapCategoryType(i));
		}
	}

	if (getNumTerrainStructs() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aTerrainStructs, pClassInfo->m_aTerrainStructs);
	}

	if (getNumPlaceBonusTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aPlaceBonusTypes, pClassInfo->m_aPlaceBonusTypes);
	}
}

void CvBuildInfo::getCheckSum(unsigned int &iSum)
{
	CheckSum(iSum, m_iTime);
	CheckSum(iSum, m_iCost);
	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iImprovement);
	CheckSum(iSum, m_iRoute);

	CheckSum(iSum, m_iTerrainChange);
	CheckSum(iSum, m_iFeatureChange);
	CheckSum(iSum, m_iObsoleteTech);
	CheckSum(iSum, m_bMine);
	CheckSum(iSum, m_bDisabled);
	CheckSum(iSum, m_bHideObsoleteExempt);

	CheckSum(iSum, m_iEntityEvent);
	CheckSum(iSum, m_iMissionType);

	CheckSum(iSum, m_bKill);

	// Arrays

	CheckSum(iSum, m_paiFeatureTech,  GC.getNumFeatureInfos());
	CheckSum(iSum, m_paiFeatureTime,  GC.getNumFeatureInfos());
	CheckSum(iSum, m_paiFeatureProduction,  GC.getNumFeatureInfos());
	CheckSum(iSum, m_pabFeatureRemove,  GC.getNumFeatureInfos());
	CheckSum(iSum, m_pabNoTechCanRemoveWithNoProductionGain,  GC.getNumFeatureInfos());

	//Vectors

	CheckSumC(iSum, m_aiPrereqBonusTypes);
	CheckSumC(iSum, m_aiMapCategoryTypes);

	int iNumElements = m_aTerrainStructs.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aTerrainStructs[i].eTerrain);
		CheckSum(iSum, m_aTerrainStructs[i].ePrereqTech);
		CheckSum(iSum, m_aTerrainStructs[i].iTime);
	}

	iNumElements = m_aPlaceBonusTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aPlaceBonusTypes[i].eBonus);
		CheckSum(iSum, m_aPlaceBonusTypes[i].iProbability);
		CheckSum(iSum, m_aPlaceBonusTypes[i].bRequiresAccess);
		CheckSum(iSum, m_aPlaceBonusTypes[i].ePrereqTerrain);
		CheckSum(iSum, m_aPlaceBonusTypes[i].ePrereqFeature);
		CheckSum(iSum, m_aPlaceBonusTypes[i].ePrereqMapCategory);
		CheckSum(iSum, m_aPlaceBonusTypes[i].ePrereqTech);
	}
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvOutcomeInfo
//
//  DESC:   Contains info about outcome types which can be the result of a kill or of actions
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvOutcomeInfo::CvOutcomeInfo() :
									m_ePrereqTech(NO_TECH),
									m_eObsoleteTech(NO_TECH),
									m_bToCoastalCity(false),
									m_bFriendlyTerritory(true),
									m_bNeutralTerritory(true),
									m_bHostileTerritory(true),
									m_bBarbarianTerritory(false),
									m_bCity(false),
									m_bNotCity(false),
									m_bCapture(false),
									m_ePrereqCivic(NO_CIVIC)
{
}

CvOutcomeInfo::~CvOutcomeInfo()
{
}

bool CvOutcomeInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}


	pXML->GetOptionalChildXmlValByName(m_szMessageText, L"Message");
	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqTech");
	m_ePrereqTech = (TechTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ObsoleteTech");
	m_eObsoleteTech = (TechTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqCivic");
	m_ePrereqCivic = (CivicTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_bToCoastalCity, L"bToCoastalCity");
	pXML->GetOptionalChildXmlValByName(&m_bFriendlyTerritory, L"bFriendlyTerritory", true);
	pXML->GetOptionalChildXmlValByName(&m_bNeutralTerritory, L"bNeutralTerritory", true);
	pXML->GetOptionalChildXmlValByName(&m_bHostileTerritory, L"bHostileTerritory", true);
	pXML->GetOptionalChildXmlValByName(&m_bBarbarianTerritory, L"bBarbarianTerritory");
	pXML->GetOptionalChildXmlValByName(&m_bCity, L"bCity");
	pXML->GetOptionalChildXmlValByName(&m_bNotCity, L"bNotCity");
	pXML->GetOptionalChildXmlValByName(&m_bCapture, L"bCapture");

	if(pXML->TryMoveToXmlFirstChild(L"ExtraChancePromotions"))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"ExtraChancePromotion"))
			{
				do
				{
					int iExtraChance;
					pXML->GetChildXmlValByName(szTextVal, L"PromotionType");
					PromotionTypes ePromotion = (PromotionTypes) pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&iExtraChance, L"iExtraChance");
					m_aeiExtraChancePromotions.push_back(std::pair<PromotionTypes,int>(ePromotion, iExtraChance));
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"PrereqBuildings"))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"BuildingType"))
			{
				do
				{
					pXML->GetXmlVal(szTextVal);
					BuildingTypes eBuilding = (BuildingTypes) pXML->GetInfoClass(szTextVal);
					m_aePrereqBuildings.push_back(eBuilding);
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	return true;
}

bool CvOutcomeInfo::readPass2(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;


	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	if(pXML->TryMoveToXmlFirstChild(L"ReplaceOutcomes"))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"OutcomeType"))
			{
				do
				{
					pXML->GetXmlVal(szTextVal);
					m_aeReplaceOutcomes.push_back((OutcomeTypes)pXML->GetInfoClass(szTextVal));
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	return true;
}

void CvOutcomeInfo::copyNonDefaults(CvOutcomeInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getMessageText() == wDefault) m_szMessageText = pClassInfo->getMessageText();
	if (getPrereqTech() == NO_TECH) m_ePrereqTech = pClassInfo->getPrereqTech();
	if (getObsoleteTech() == NO_TECH) m_eObsoleteTech = pClassInfo->getObsoleteTech();
	if (!getToCoastalCity()) m_bToCoastalCity = pClassInfo->getToCoastalCity();
	if (getFriendlyTerritory()) m_bFriendlyTerritory = pClassInfo->getFriendlyTerritory();
	if (getNeutralTerritory()) m_bNeutralTerritory = pClassInfo->getNeutralTerritory();
	if (getHostileTerritory()) m_bHostileTerritory = pClassInfo->getHostileTerritory();
	if (!getBarbarianTerritory()) m_bBarbarianTerritory = pClassInfo->getBarbarianTerritory();
	if (!getCity()) m_bCity = pClassInfo->getCity();
	if (!getNotCity()) m_bNotCity = pClassInfo->getNotCity();
	if (!isCapture()) m_bCapture = pClassInfo->isCapture();
	if (getNumPrereqBuildings() == 0) m_aePrereqBuildings = pClassInfo->m_aePrereqBuildings;
	if (getNumExtraChancePromotions() == 0) m_aeiExtraChancePromotions = pClassInfo->m_aeiExtraChancePromotions;
	if (getPrereqCivic() == NO_CIVIC) m_ePrereqCivic = pClassInfo->getPrereqCivic();
}

void CvOutcomeInfo::copyNonDefaultsReadPass2(CvOutcomeInfo* pClassInfo, CvXMLLoadUtility* pXML, bool bOver)
{
	for ( int i = 0; i < pClassInfo->getNumReplaceOutcomes(); i++ )
	{
		m_aeReplaceOutcomes.push_back(pClassInfo->getReplaceOutcome(i));
	}
}

void CvOutcomeInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_ePrereqTech);
	CheckSum(iSum, m_eObsoleteTech);
	CheckSumC(iSum, m_aeiExtraChancePromotions);
	CheckSumC(iSum, m_aePrereqBuildings);
	CheckSum(iSum, m_bToCoastalCity);
	CheckSum(iSum, m_bFriendlyTerritory);
	CheckSum(iSum, m_bNeutralTerritory);
	CheckSum(iSum, m_bHostileTerritory);
	CheckSum(iSum, m_bBarbarianTerritory);
	CheckSum(iSum, m_bCity);
	CheckSum(iSum, m_bNotCity);
	CheckSum(iSum, m_bCapture);
	CheckSumC(iSum, m_aeReplaceOutcomes);
	CheckSum(iSum, m_ePrereqCivic);
}

CvWString CvOutcomeInfo::getMessageText() const
{
	return m_szMessageText;
}

bool CvOutcomeInfo::getToCoastalCity() const
{
	return m_bToCoastalCity;
}

bool CvOutcomeInfo::getFriendlyTerritory() const
{
	return m_bFriendlyTerritory;
}

bool CvOutcomeInfo::getNeutralTerritory() const
{
	return m_bNeutralTerritory;
}

bool CvOutcomeInfo::getHostileTerritory() const
{
	return m_bHostileTerritory;
}

bool CvOutcomeInfo::getBarbarianTerritory() const
{
	return m_bBarbarianTerritory;
}

bool CvOutcomeInfo::getCity() const
{
	return m_bCity;
}

bool CvOutcomeInfo::getNotCity() const
{
	return m_bNotCity;
}

bool CvOutcomeInfo::isCapture() const
{
	return m_bCapture;
}

TechTypes CvOutcomeInfo::getPrereqTech() const
{
	return m_ePrereqTech;
}

TechTypes CvOutcomeInfo::getObsoleteTech() const
{
	return m_eObsoleteTech;
}

CivicTypes CvOutcomeInfo::getPrereqCivic() const
{
	return m_ePrereqCivic;
}

int CvOutcomeInfo::getNumPrereqBuildings() const
{
	return m_aePrereqBuildings.size();
}

BuildingTypes CvOutcomeInfo::getPrereqBuilding(int i) const
{
	FAssertMsg(i < (int) m_aePrereqBuildings.size(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aePrereqBuildings[i];
}

int CvOutcomeInfo::getNumExtraChancePromotions() const
{
	return m_aeiExtraChancePromotions.size();
}

PromotionTypes CvOutcomeInfo::getExtraChancePromotion(int i) const
{
	FAssertMsg(i < (int) m_aeiExtraChancePromotions.size(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aeiExtraChancePromotions[i].first;
}

int CvOutcomeInfo::getExtraChancePromotionChance(int i) const
{
	FAssertMsg(i < (int) m_aeiExtraChancePromotions.size(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aeiExtraChancePromotions[i].second;
}

int CvOutcomeInfo::getNumReplaceOutcomes() const
{
	return m_aeReplaceOutcomes.size();
}

OutcomeTypes CvOutcomeInfo::getReplaceOutcome(int i) const
{
	FAssertMsg(i < (int) m_aeReplaceOutcomes.size(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_aeReplaceOutcomes[i];
}

//TB Promotion Line Info
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPromotionLineInfo
//
//  DESC:   Contains info about Promotion Lines
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvPromotionLineInfo::CvPromotionLineInfo() :
									m_ePrereqTech(NO_TECH),
									m_eObsoleteTech(NO_TECH),
									m_ePropertyType(NO_PROPERTY),
									m_iToleranceBuildup(0),
									m_iToleranceDecay(0),
									m_iCommunicability(0),
									m_iWorseningProbabilityIncrementModifier(0),
									m_iWorsenedCommunicabilityIncrementModifier(0),
									m_iWorsenedOvercomeIncrementModifier(0),
									m_iOvercomeProbability(0),
									m_iOvercomeAdjperTurn(0),
									m_iOutbreakModifier(100),
									m_iOvercomeModifier(100),
									m_bAffliction(false),
									m_bEquipment(false),
									m_bCritical(false),
									m_bNoSpreadonBattle(false),
									m_bNoSpreadUnitProximity(false),
									m_bNoSpreadUnittoCity(false),
									m_bNoSpreadCitytoUnit(false),
									m_bBuildUp(false),
									m_bPoison(false)
{
}

CvPromotionLineInfo::~CvPromotionLineInfo()
{
}

bool CvPromotionLineInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqTech");
	m_ePrereqTech = (TechTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ObsoleteTech");
	m_eObsoleteTech = (TechTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PropertyType");
	m_ePropertyType = (PropertyTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iToleranceBuildup, L"iToleranceBuildup");
	pXML->GetOptionalChildXmlValByName(&m_iToleranceDecay, L"iToleranceDecay");
	pXML->GetOptionalChildXmlValByName(&m_iCommunicability, L"iCommunicability");
	pXML->GetOptionalChildXmlValByName(&m_iWorseningProbabilityIncrementModifier, L"iWorseningProbabilityIncrementModifier");
	pXML->GetOptionalChildXmlValByName(&m_iWorsenedCommunicabilityIncrementModifier, L"iWorsenedCommunicabilityIncrementModifier");
	pXML->GetOptionalChildXmlValByName(&m_iWorsenedOvercomeIncrementModifier, L"iWorsenedOvercomeIncrementModifier");
	pXML->GetOptionalChildXmlValByName(&m_iOvercomeProbability, L"iOvercomeProbability");
	pXML->GetOptionalChildXmlValByName(&m_iOvercomeAdjperTurn, L"iOvercomeAdjperTurn");
	pXML->GetOptionalChildXmlValByName(&m_iOutbreakModifier, L"iOutbreakModifier", 100);
	pXML->GetOptionalChildXmlValByName(&m_iOvercomeModifier, L"iOvercomeModifier", 100);
	pXML->GetOptionalChildXmlValByName(&m_bAffliction, L"bAffliction");
	pXML->GetOptionalChildXmlValByName(&m_bEquipment, L"bEquipment");
	pXML->GetOptionalChildXmlValByName(&m_bCritical, L"bCritical");
	pXML->GetOptionalChildXmlValByName(&m_bNoSpreadonBattle, L"bNoSpreadonBattle");
	pXML->GetOptionalChildXmlValByName(&m_bNoSpreadUnitProximity, L"bNoSpreadUnitProximity");
	pXML->GetOptionalChildXmlValByName(&m_bNoSpreadUnittoCity, L"bNoSpreadUnittoCity");
	pXML->GetOptionalChildXmlValByName(&m_bNoSpreadCitytoUnit, L"bNoSpreadCitytoUnit");
	pXML->GetOptionalChildXmlValByName(&m_bBuildUp, L"bBuildUp");
	pXML->GetOptionalChildXmlValByName(&m_bPoison, L"bPoison");

	// bool vector without delayed resolution
	pXML->SetOptionalIntVector(&m_aiUnitCombatPrereqTypes, L"UnitCombatPrereqTypes");
	pXML->SetOptionalIntVector(&m_aiNotOnUnitCombatTypes, L"NotOnUnitCombatTypes");
	pXML->SetOptionalIntVector(&m_aiNotOnDomainTypes, L"NotOnDomainTypes");
	pXML->SetOptionalIntVector(&m_aiOnGameOptions, L"OnGameOptions");
	pXML->SetOptionalIntVector(&m_aiNotOnGameOptions, L"NotOnGameOptions");
	pXML->SetOptionalIntVector(&m_aiCriticalOriginCombatClassTypes, L"CriticalOriginCombatClassTypes");

	// int vector utilizing pairing without delayed resolution
	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aUnitCombatContractChanceChanges, L"UnitCombatContractChanceChanges");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aUnitCombatOvercomeChanges, L"UnitCombatOvercomeChanges");

	pXML->SetOptionalPairVector<TechModifierArray, TechTypes, int>(&m_aTechContractChanceChanges, L"TechContractChanceChanges");

	pXML->SetOptionalPairVector<TechModifierArray, TechTypes, int>(&m_aTechOvercomeChanges, L"TechOvercomeChanges");

	return true;
}

bool CvPromotionLineInfo::readPass2(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;


	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	return true;
}

void CvPromotionLineInfo::copyNonDefaults(CvPromotionLineInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getPrereqTech() == NO_TECH) m_ePrereqTech = pClassInfo->getPrereqTech();
	if (getObsoleteTech() == NO_TECH) m_eObsoleteTech = pClassInfo->getObsoleteTech();

	if (getToleranceBuildup() == iDefault) m_iToleranceBuildup = pClassInfo->getToleranceBuildup();
	if (getToleranceDecay() == iDefault) m_iToleranceDecay = pClassInfo->getToleranceDecay();
	if (getCommunicability() == iDefault) m_iCommunicability = pClassInfo->getCommunicability();
	if (getWorseningProbabilityIncrementModifier() == iDefault) m_iWorseningProbabilityIncrementModifier = pClassInfo->getWorseningProbabilityIncrementModifier();
	if (getWorsenedCommunicabilityIncrementModifier() == iDefault) m_iWorsenedCommunicabilityIncrementModifier = pClassInfo->getWorsenedCommunicabilityIncrementModifier();
	if (getWorsenedOvercomeIncrementModifier() == iDefault) m_iWorsenedOvercomeIncrementModifier = pClassInfo->getWorsenedOvercomeIncrementModifier();
	if (getOvercomeProbability() == iDefault) m_iOvercomeProbability = pClassInfo->getOvercomeProbability();
	if (getOvercomeAdjperTurn() == iDefault) m_iOvercomeAdjperTurn = pClassInfo->getOvercomeAdjperTurn();
	if (getOutbreakModifier() == iDefault) m_iOutbreakModifier = pClassInfo->getOutbreakModifier();
	if (getOvercomeModifier() == iDefault) m_iOvercomeModifier = pClassInfo->getOvercomeModifier();
	if (isAffliction() == bDefault) m_bAffliction = pClassInfo->isAffliction();
	if (isEquipment() == bDefault) m_bEquipment = pClassInfo->isEquipment();
	if (isCritical() == bDefault) m_bCritical = pClassInfo->isCritical();
	if (isNoSpreadonBattle() == bDefault) m_bNoSpreadonBattle = pClassInfo->isNoSpreadonBattle();
	if (isNoSpreadUnitProximity() == bDefault) m_bNoSpreadUnitProximity = pClassInfo->isNoSpreadUnitProximity();
	if (isNoSpreadUnittoCity() == bDefault) m_bNoSpreadUnittoCity = pClassInfo->isNoSpreadUnittoCity();
	if (isNoSpreadCitytoUnit() == bDefault) m_bNoSpreadCitytoUnit = pClassInfo->isNoSpreadCitytoUnit();
	if (isBuildUp() == bDefault) m_bBuildUp = pClassInfo->isBuildUp();
	if (isPoison() == bDefault) m_bPoison = pClassInfo->isPoison();

	if (getPropertyType() != NO_PROPERTY) m_ePropertyType = pClassInfo->getPropertyType();

	// bool vectors without delayed resolution
	if (getNumUnitCombatPrereqTypes() == 0)
	{
		m_aiUnitCombatPrereqTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumUnitCombatPrereqTypes(); i++)
		{
			m_aiUnitCombatPrereqTypes.push_back(pClassInfo->getUnitCombatPrereqType(i));
		}
	}

	if (getNumNotOnUnitCombatTypes() == 0)
	{
		m_aiNotOnUnitCombatTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumNotOnUnitCombatTypes(); i++)
		{
			m_aiNotOnUnitCombatTypes.push_back(pClassInfo->getNotOnUnitCombatType(i));
		}
	}

	if (getNumNotOnDomainTypes() == 0)
	{
		m_aiNotOnDomainTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumNotOnDomainTypes(); i++)
		{
			m_aiNotOnDomainTypes.push_back(pClassInfo->getNotOnDomainType(i));
		}
	}

	if (getNumOnGameOptions() == 0)
	{
		m_aiOnGameOptions.clear();
		for ( int i = 0; i < pClassInfo->getNumOnGameOptions(); i++)
		{
			m_aiOnGameOptions.push_back(pClassInfo->getOnGameOption(i));
		}
	}

	if (getNumNotOnGameOptions() == 0)
	{
		m_aiNotOnGameOptions.clear();
		for ( int i = 0; i < pClassInfo->getNumNotOnGameOptions(); i++)
		{
			m_aiNotOnGameOptions.push_back(pClassInfo->getNotOnGameOption(i));
		}
	}

	if (getNumCriticalOriginCombatClassTypes() == 0)
	{
		m_aiCriticalOriginCombatClassTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumCriticalOriginCombatClassTypes(); i++)
		{
			m_aiCriticalOriginCombatClassTypes.push_back(pClassInfo->getCriticalOriginCombatClassType(i));
		}
	}

	// int vector utilizing pairing without delayed resolution
	if (getNumUnitCombatContractChanceChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumUnitCombatContractChanceChanges(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getUnitCombatContractChanceChange(i);
			m_aUnitCombatContractChanceChanges.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumUnitCombatOvercomeChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumUnitCombatOvercomeChanges(); i++)
		{
			UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			int iChange = pClassInfo->getUnitCombatOvercomeChange(i);
			m_aUnitCombatOvercomeChanges.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumTechContractChanceChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumTechContractChanceChanges(); i++)
		{
			TechTypes eTech = ((TechTypes)i);
			int iChange = pClassInfo->getTechContractChanceChange(i);
			m_aTechContractChanceChanges.push_back(std::make_pair(eTech, iChange));
		}
	}

	if (getNumTechOvercomeChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumTechOvercomeChanges(); i++)
		{
			TechTypes eTech = ((TechTypes)i);
			int iChange = pClassInfo->getTechOvercomeChange(i);
			m_aTechOvercomeChanges.push_back(std::make_pair(eTech, iChange));
		}
	}
}

void CvPromotionLineInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_ePrereqTech);
	CheckSum(iSum, m_eObsoleteTech);
	//integers
	CheckSum(iSum, m_iToleranceBuildup);
	CheckSum(iSum, m_iToleranceDecay);
	CheckSum(iSum, m_iCommunicability);
	CheckSum(iSum, m_iWorseningProbabilityIncrementModifier);
	CheckSum(iSum, m_iWorsenedCommunicabilityIncrementModifier);
	CheckSum(iSum, m_iWorsenedOvercomeIncrementModifier);
	CheckSum(iSum, m_iOvercomeProbability);
	CheckSum(iSum, m_iOvercomeAdjperTurn);
	CheckSum(iSum, m_iOutbreakModifier);
	CheckSum(iSum, m_iOvercomeModifier);
	//booleans
	CheckSum(iSum, m_bAffliction);
	CheckSum(iSum, m_bEquipment);
	CheckSum(iSum, m_bCritical);
	CheckSum(iSum, m_bNoSpreadonBattle);
	CheckSum(iSum, m_bNoSpreadUnitProximity);
	CheckSum(iSum, m_bNoSpreadUnittoCity);
	CheckSum(iSum, m_bNoSpreadCitytoUnit);
	CheckSum(iSum, m_bBuildUp);
	CheckSum(iSum, m_bPoison);

	CheckSum(iSum, (int)m_ePropertyType);
	// bool vectors without delayed resolution
	CheckSumC(iSum, m_aiUnitCombatPrereqTypes);
	CheckSumC(iSum, m_aiNotOnUnitCombatTypes);
	CheckSumC(iSum, m_aiNotOnDomainTypes);
	CheckSumC(iSum, m_aiOnGameOptions);
	CheckSumC(iSum, m_aiNotOnGameOptions);
	CheckSumC(iSum, m_aiCriticalOriginCombatClassTypes);
	// int vector utilizing pairing without delayed resolution
	CheckSumC(iSum, m_aUnitCombatContractChanceChanges);
	CheckSumC(iSum, m_aUnitCombatOvercomeChanges);
	CheckSumC(iSum, m_aTechContractChanceChanges);
	CheckSumC(iSum, m_aTechOvercomeChanges);
}

TechTypes CvPromotionLineInfo::getPrereqTech() const
{
	return m_ePrereqTech;
}

TechTypes CvPromotionLineInfo::getObsoleteTech() const
{
	return m_eObsoleteTech;
}

int CvPromotionLineInfo::getToleranceBuildup() const
{
	return m_iToleranceBuildup;
}

int CvPromotionLineInfo::getToleranceDecay() const
{
	return m_iToleranceDecay;
}

int CvPromotionLineInfo::getCommunicability() const
{
	return m_iCommunicability;
}

int CvPromotionLineInfo::getWorseningProbabilityIncrementModifier() const
{
	return m_iWorseningProbabilityIncrementModifier;
}

int CvPromotionLineInfo::getWorsenedCommunicabilityIncrementModifier() const
{
	return m_iWorsenedCommunicabilityIncrementModifier;
}

int CvPromotionLineInfo::getWorsenedOvercomeIncrementModifier() const
{
	return m_iWorsenedOvercomeIncrementModifier;
}

int CvPromotionLineInfo::getOvercomeProbability() const
{
	return m_iOvercomeProbability;
}

int CvPromotionLineInfo::getOvercomeAdjperTurn() const
{
	return m_iOvercomeAdjperTurn;
}

int CvPromotionLineInfo::getOutbreakModifier() const
{
	return m_iOutbreakModifier;
}

int CvPromotionLineInfo::getOvercomeModifier() const
{
	return m_iOvercomeModifier;
}

bool CvPromotionLineInfo::isAffliction() const
{
	return m_bAffliction;
}

bool CvPromotionLineInfo::isEquipment() const
{
	return m_bEquipment;
}

bool CvPromotionLineInfo::isCritical() const
{
	return m_bCritical;
}

bool CvPromotionLineInfo::isNoSpreadonBattle() const
{
	return m_bNoSpreadonBattle;
}

bool CvPromotionLineInfo::isNoSpreadUnitProximity() const
{
	return m_bNoSpreadUnitProximity;
}

bool CvPromotionLineInfo::isNoSpreadUnittoCity() const
{
	return m_bNoSpreadUnittoCity;
}

bool CvPromotionLineInfo::isNoSpreadCitytoUnit() const
{
	return m_bNoSpreadCitytoUnit;
}

bool CvPromotionLineInfo::isBuildUp() const
{
	return m_bBuildUp;
}

bool CvPromotionLineInfo::isPoison() const
{
	return m_bPoison;
}

PropertyTypes CvPromotionLineInfo::getPropertyType() const
{
	return m_ePropertyType;
}

int CvPromotionLineInfo::getUnitCombatPrereqType(int i) const
{
	return m_aiUnitCombatPrereqTypes[i];
}

int CvPromotionLineInfo::getNumUnitCombatPrereqTypes() const
{
	return (int)m_aiUnitCombatPrereqTypes.size();
}

bool CvPromotionLineInfo::isUnitCombatPrereqType(int i)
{
	FAssert (i > -1 && i < GC.getNumUnitCombatInfos());
	if (find(m_aiUnitCombatPrereqTypes.begin(), m_aiUnitCombatPrereqTypes.end(), i) == m_aiUnitCombatPrereqTypes.end())
	{
		return false;
	}
	return true;
}

int CvPromotionLineInfo::getNotOnUnitCombatType(int i) const
{
	return m_aiNotOnUnitCombatTypes[i];
}

int CvPromotionLineInfo::getNumNotOnUnitCombatTypes() const
{
	return (int)m_aiNotOnUnitCombatTypes.size();
}

bool CvPromotionLineInfo::isNotOnUnitCombatType(int i)
{
	FAssert (i > -1 && i < GC.getNumUnitCombatInfos());
	if (find(m_aiNotOnUnitCombatTypes.begin(), m_aiNotOnUnitCombatTypes.end(), i) == m_aiNotOnUnitCombatTypes.end())
	{
		return false;
	}
	return true;
}

int CvPromotionLineInfo::getNotOnDomainType(int i) const
{
	return m_aiNotOnDomainTypes[i];
}

int CvPromotionLineInfo::getNumNotOnDomainTypes() const
{
	return (int)m_aiNotOnDomainTypes.size();
}

bool CvPromotionLineInfo::isNotOnDomainType(int i)
{
	FAssert (i > -1 && i < NUM_DOMAIN_TYPES);
	if (find(m_aiNotOnDomainTypes.begin(), m_aiNotOnDomainTypes.end(), i) == m_aiNotOnDomainTypes.end())
	{
		return false;
	}
	return true;
}

int CvPromotionLineInfo::getOnGameOption(int i) const
{
	return m_aiOnGameOptions[i];
}

int CvPromotionLineInfo::getNumOnGameOptions() const
{
	return (int)m_aiOnGameOptions.size();
}

bool CvPromotionLineInfo::isOnGameOption(int i)
{
	FAssert (i > -1 && i < GC.getNumGameOptions());
	if (find(m_aiOnGameOptions.begin(), m_aiOnGameOptions.end(), i) == m_aiOnGameOptions.end())
	{
		return false;
	}
	return true;
}

int CvPromotionLineInfo::getNotOnGameOption(int i) const
{
	return m_aiNotOnGameOptions[i];
}

int CvPromotionLineInfo::getNumNotOnGameOptions() const
{
	return (int)m_aiNotOnGameOptions.size();
}

bool CvPromotionLineInfo::isNotOnGameOption(int i)
{
	FAssert (i > -1 && i < GC.getNumGameOptions());
	if (find(m_aiNotOnGameOptions.begin(), m_aiNotOnGameOptions.end(), i) == m_aiNotOnGameOptions.end())
	{
		return false;
	}
	return true;
}

int CvPromotionLineInfo::getCriticalOriginCombatClassType(int i) const
{
	return m_aiCriticalOriginCombatClassTypes[i];
}

int CvPromotionLineInfo::getNumCriticalOriginCombatClassTypes() const
{
	return (int)m_aiCriticalOriginCombatClassTypes.size();
}

bool CvPromotionLineInfo::isCriticalOriginCombatClassType(int i)
{
	FAssert (i > -1 && i < GC.getNumUnitCombatInfos());
	if (find(m_aiCriticalOriginCombatClassTypes.begin(), m_aiCriticalOriginCombatClassTypes.end(), i) == m_aiCriticalOriginCombatClassTypes.end())
	{
		return false;
	}
	return true;
}

// int vector utilizing pairing without delayed resolution
int CvPromotionLineInfo::getNumUnitCombatContractChanceChanges() const
{
	return m_aUnitCombatContractChanceChanges.size();
}

int CvPromotionLineInfo::getUnitCombatContractChanceChange(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aUnitCombatContractChanceChanges.begin(); it != m_aUnitCombatContractChanceChanges.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionLineInfo::isUnitCombatContractChanceChange(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aUnitCombatContractChanceChanges.begin(); it != m_aUnitCombatContractChanceChanges.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionLineInfo::getNumUnitCombatOvercomeChanges() const
{
	return m_aUnitCombatOvercomeChanges.size();
}

int CvPromotionLineInfo::getUnitCombatOvercomeChange(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aUnitCombatOvercomeChanges.begin(); it != m_aUnitCombatOvercomeChanges.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionLineInfo::isUnitCombatOvercomeChange(int iUnitCombat)
{
	for (UnitCombatModifierArray::const_iterator it = m_aUnitCombatOvercomeChanges.begin(); it != m_aUnitCombatOvercomeChanges.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionLineInfo::getNumTechContractChanceChanges() const
{
	return m_aTechContractChanceChanges.size();
}

int CvPromotionLineInfo::getTechContractChanceChange(int iTech) const
{
	for (TechModifierArray::const_iterator it = m_aTechContractChanceChanges.begin(); it != m_aTechContractChanceChanges.end(); ++it)
	{
		if ((*it).first == (TechTypes)iTech)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionLineInfo::isTechContractChanceChange(int iTech)
{
	for (TechModifierArray::const_iterator it = m_aTechContractChanceChanges.begin(); it != m_aTechContractChanceChanges.end(); ++it)
	{
		if ((*it).first == (TechTypes)iTech)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionLineInfo::getNumTechOvercomeChanges() const
{
	return m_aTechOvercomeChanges.size();
}

int CvPromotionLineInfo::getTechOvercomeChange(int iTech) const
{
	for (TechModifierArray::const_iterator it = m_aTechOvercomeChanges.begin(); it != m_aTechOvercomeChanges.end(); ++it)
	{
		if ((*it).first == (TechTypes)iTech)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvPromotionLineInfo::isTechOvercomeChange(int iTech)
{
	for (TechModifierArray::const_iterator it = m_aTechOvercomeChanges.begin(); it != m_aTechOvercomeChanges.end(); ++it)
	{
		if ((*it).first == (TechTypes)iTech)
		{
			return true;
		}
	}
	return false;
}

int CvPromotionLineInfo::getPromotion(int i) const
{
	return m_aiPromotions[i];
}

int CvPromotionLineInfo::getNumPromotions() const
{
	return (int)m_aiPromotions.size();
}

bool CvPromotionLineInfo::isPromotion(int i)
{
	FAssert (i > -1 && i < GC.getNumPromotionInfos());
	if (find(m_aiPromotions.begin(), m_aiPromotions.end(), i) == m_aiPromotions.end())
	{
		return false;
	}
	return true;
}
void CvPromotionLineInfo::setPromotions()
{
	m_aiPromotions.clear();
	int iIndex = GC.getInfoTypeForString(getType());
	for ( int i = 0; i < GC.getNumPromotionInfos(); i++)
	{
		if (GC.getPromotionInfo((PromotionTypes)i).getPromotionLine() == iIndex)
		{
			m_aiPromotions.push_back(i);
		}
	}
}

int CvPromotionLineInfo::getBuilding(int i) const
{
	return m_aiBuildings[i];
}

int CvPromotionLineInfo::getNumBuildings() const
{
	return (int)m_aiBuildings.size();
}

bool CvPromotionLineInfo::isBuilding(int i)
{
	FAssert (i > -1 && i < GC.getNumBuildingInfos());
	if (find(m_aiBuildings.begin(), m_aiBuildings.end(), i) == m_aiBuildings.end())
	{
		return false;
	}
	return true;
}
void CvPromotionLineInfo::setBuildings()
{
	m_aiBuildings.clear();
	int iIndex = GC.getInfoTypeForString(getType());
	for ( int i = 0; i < GC.getNumBuildingInfos(); i++)
	{
		if (GC.getBuildingInfo((BuildingTypes)i).getPromotionLineType() == iIndex)
		{
			m_aiBuildings.push_back(i);
		}
	}
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvUnitCombatInfo
//
//  DESC:   Contains unit combat types
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
UnitCombatTypes CvUnitCombatInfo::m_UnitCombatAnimal = NO_UNITCOMBAT;
UnitCombatTypes CvUnitCombatInfo::m_UnitCombatWildAnimal = NO_UNITCOMBAT;

CvUnitCombatInfo::CvUnitCombatInfo() :
//Textual References
m_eReligion(NO_RELIGION)
,m_eCulture(NO_BONUS)
,m_eEra(NO_ERA)
//Integers
,m_iAssetMultiplier(0)
,m_iPowerMultiplier(0)
,m_iVisibilityChange(0)
,m_iMovesChange(0)
,m_iMoveDiscountChange(0)
,m_iAirRangeChange(0)
,m_iInterceptChange(0)
,m_iEvasionChange(0)
,m_iWithdrawalChange(0)
,m_iCargoChange(0)
,m_iSMCargoChange(0)
,m_iSMCargoVolumeChange(0)
,m_iSMCargoVolumeModifierChange(0)
,m_iCollateralDamageChange(0)
,m_iBombardRateChange(0)
,m_iFirstStrikesChange(0)
,m_iChanceFirstStrikesChange(0)
,m_iEnemyHealChange(0)
,m_iNeutralHealChange(0)
,m_iFriendlyHealChange(0)
,m_iSameTileHealChange(0)
,m_iAdjacentTileHealChange(0)
,m_iCombatPercent(0)
,m_iCityAttackPercent(0)
,m_iCityDefensePercent(0)
,m_iHillsAttackPercent(0)
,m_iHillsDefensePercent(0)
,m_iHillsWorkPercent(0)
,m_iWorkRatePercent(0)
,m_iRevoltProtection(0)
,m_iCollateralDamageProtection(0)
,m_iPillageChange(0)
,m_iUpgradeDiscount(0)
,m_iExperiencePercent(0)
,m_iKamikazePercent(0)
,m_iAirCombatLimitChange(0)
,m_iCelebrityHappy(0)
,m_iCollateralDamageLimitChange(0)
,m_iCollateralDamageMaxUnitsChange(0)
,m_iCombatLimitChange(0)
,m_iExtraDropRange(0)
,m_iSurvivorChance(0)
,m_iVictoryAdjacentHeal(0)
,m_iVictoryHeal(0)
,m_iVictoryStackHeal(0)
,m_iAttackCombatModifierChange(0)
,m_iDefenseCombatModifierChange(0)
,m_iPursuitChange(0)
,m_iEarlyWithdrawChange(0)
,m_iVSBarbsChange(0)
,m_iArmorChange(0)
,m_iPunctureChange(0)
,m_iOverrunChange(0)
,m_iRepelChange(0)
,m_iFortRepelChange(0)
,m_iRepelRetriesChange(0)
,m_iUnyieldingChange(0)
,m_iKnockbackChange(0)
,m_iKnockbackRetriesChange(0)
,m_iStrAdjperAttChange(0)
,m_iStrAdjperDefChange(0)
,m_iWithdrawAdjperAttChange(0)
,m_iUnnerveChange(0)
,m_iEncloseChange(0)
,m_iLungeChange(0)
,m_iDynamicDefenseChange(0)
,m_iStrengthChange(0)
,m_iFortitudeChange(0)
,m_iFrontSupportPercentChange(0)
,m_iShortRangeSupportPercentChange(0)
,m_iMediumRangeSupportPercentChange(0)
,m_iLongRangeSupportPercentChange(0)
,m_iFlankSupportPercentChange(0)
,m_iDodgeModifierChange(0)
,m_iPrecisionModifierChange(0)
,m_iPowerShotsChange(0)
,m_iPowerShotCombatModifierChange(0)
,m_iPowerShotPunctureModifierChange(0)
,m_iPowerShotPrecisionModifierChange(0)
,m_iPowerShotCriticalModifierChange(0)
,m_iCriticalModifierChange(0)
,m_iEnduranceChange(0)
,m_iRoundStunProbChange(0)
,m_iPoisonProbabilityModifierChange(0)
,m_iCaptureProbabilityModifierChange(0)
,m_iCaptureResistanceModifierChange(0)
,m_iHillsWorkModifierChange(0)
,m_iPeaksWorkModifierChange(0)
,m_iBreakdownChanceChange(0)
,m_iBreakdownDamageChange(0)
,m_iTauntChange(0)
,m_iMaxHPChange(0)
,m_iStrengthModifier(0)
,m_iQualityBase(-10)
,m_iGroupBase(-10)
,m_iSizeBase(-10)
,m_iDamageModifierChange(0)
,m_iCostModifierChange(0)
,m_iRBombardDamageBase(0)
,m_iRBombardDamageLimitBase(0)
,m_iRBombardDamageMaxUnitsBase(0)
,m_iDCMBombRangeBase(0)
,m_iDCMBombAccuracyBase(0)
,m_iCombatModifierPerSizeMoreChange(0)
,m_iCombatModifierPerSizeLessChange(0)
,m_iCombatModifierPerVolumeMoreChange(0)
,m_iCombatModifierPerVolumeLessChange(0)
,m_iSelfHealModifier(0)
,m_iNumHealSupport(0)
,m_iExcileChange(0)
,m_iPassageChange(0)
,m_iNoNonOwnedCityEntryChange(0)
,m_iBarbCoExistChange(0)
,m_iBlendIntoCityChange(0)
,m_iInsidiousnessChange(0)
,m_iInvestigationChange(0)
,m_iStealthStrikesChange(0)
,m_iStealthCombatModifierChange(0)
,m_iStealthDefenseChange(0)
,m_iDefenseOnlyChange(0)
,m_iNoInvisibilityChange(0)
,m_iNoCaptureChange(0)
,m_iAnimalIgnoresBordersChange(0)
,m_iNoDefensiveBonusChange(0)
,m_iGatherHerdChange(0)
,m_iReligiousCombatModifierChange(0)
//Booleans
,m_bDefensiveVictoryMove(false)
,m_bFreeDrop(false)
,m_bOffensiveVictoryMove(false)
,m_bOneUp(false)
,m_bPillageCulture(false)
,m_bPillageEspionage(false)
,m_bPillageMarauder(false)
,m_bPillageOnMove(false)
,m_bPillageOnVictory(false)
,m_bPillageResearch(false)
,m_bBlitz(false)
,m_bAmphib(false)
,m_bRiver(false)
,m_bEnemyRoute(false)
,m_bAlwaysHeal(false)
,m_bHillsDoubleMove(false)
,m_bImmuneToFirstStrikes(false)
,m_bStampedeChange(false)
,m_bRemoveStampede(false)
,m_bOnslaughtChange(false)
,m_bMakesDamageCold(false)
,m_bMakesDamageNotCold(false)
,m_bAddsColdImmunity(false)
,m_bRemovesColdImmunity(false)
,m_bAttackOnlyCitiesAdd(false)
,m_bAttackOnlyCitiesSubtract(false)
,m_bIgnoreNoEntryLevelAdd(false)
,m_bIgnoreNoEntryLevelSubtract(false)
,m_bIgnoreZoneofControlAdd(false)
,m_bIgnoreZoneofControlSubtract(false)
,m_bFliesToMoveAdd(false)
,m_bFliesToMoveSubtract(false)
,m_bCanMovePeaks(false)
,m_bCanLeadThroughPeaks(false)
,m_bZoneOfControl(false)
,m_bSpy(false)
,m_bCannotMergeSplit(false)
,m_bRBombardDirect(false)
,m_bRBombardForceAbility(false)
,m_bInvisible(false)
,m_bForMilitary(false)
,m_bForNavalMilitary(false)
,m_bHealsAs(false)
,m_bNoSelfHeal(false)
//Array Derived
,m_bAnyDomainModifierPercent(false)
//Arrays
,m_piDomainModifierPercent(NULL)
//PropertyManipulators
,m_PropertyManipulators()
{
	m_zobristValue = GC.getGameINLINE().getSorenRand().getInt();
}

CvUnitCombatInfo::~CvUnitCombatInfo()
{

	SAFE_DELETE_ARRAY(m_piDomainModifierPercent);

	for (int i=0; i<(int)m_aOutcomeMissions.size(); i++)
	{
		SAFE_DELETE(m_aOutcomeMissions[i]);
	}

	// bool vector with delayed resolution
	for (int i=0; i<(int)m_aiCureAfflictionChangeTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aiCureAfflictionChangeTypes[i]));
	}

	// int vector utilizing struct with delayed resolution
	for (int i=0; i<(int)m_aAfflictionFortitudeChangeModifiers.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aAfflictionFortitudeChangeModifiers[i]));
	}

	for (int i=0; i<(int)m_aBuildWorkChangeModifiers.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aBuildWorkChangeModifiers[i]));
	}

	for (int i=0; i<(int)m_aUnitCombatChangeModifiers.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aUnitCombatChangeModifiers[i]));
	}

	for (int i=0; i<(int)m_aFlankingStrengthbyUnitCombatTypeChange.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aFlankingStrengthbyUnitCombatTypeChange[i]));
	}

	for (int i=0; i<(int)m_aWithdrawVSUnitCombatTypeChange.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aWithdrawVSUnitCombatTypeChange[i]));
	}

	for (int i=0; i<(int)m_aPursuitVSUnitCombatTypeChange.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aPursuitVSUnitCombatTypeChange[i]));
	}

	for (int i=0; i<(int)m_aRepelVSUnitCombatTypeChange.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aRepelVSUnitCombatTypeChange[i]));
	}

	for (int i=0; i<(int)m_aKnockbackVSUnitCombatTypeChange.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aKnockbackVSUnitCombatTypeChange[i]));
	}

	for (int i=0; i<(int)m_aPunctureVSUnitCombatTypeChange.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aPunctureVSUnitCombatTypeChange[i]));
	}

	for (int i=0; i<(int)m_aArmorVSUnitCombatTypeChange.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aArmorVSUnitCombatTypeChange[i]));
	}

	for (int i=0; i<(int)m_aDodgeVSUnitCombatTypeChange.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aDodgeVSUnitCombatTypeChange[i]));
	}

	for (int i=0; i<(int)m_aPrecisionVSUnitCombatTypeChange.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aPrecisionVSUnitCombatTypeChange[i]));
	}

	for (int i=0; i<(int)m_aCriticalVSUnitCombatTypeChange.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aCriticalVSUnitCombatTypeChange[i]));
	}

	for (int i=0; i<(int)m_aRoundStunVSUnitCombatTypeChange.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aRoundStunVSUnitCombatTypeChange[i]));
	}

	for (int i=0; i<(int)m_aTrapAvoidanceUnitCombatTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aTrapAvoidanceUnitCombatTypes[i]));
	}

	for (int i=0; i<(int)m_aAfflictOnAttackChangeTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aAfflictOnAttackChangeTypes[i]));
	}

	for (int i=0; i<(int)m_aDistanceAttackCommunicabilityTypeChanges.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aDistanceAttackCommunicabilityTypeChanges[i]));
	}
}

// Textual References
ReligionTypes CvUnitCombatInfo::getReligion() const
{
	return m_eReligion;
}

BonusTypes CvUnitCombatInfo::getCulture() const
{
	return m_eCulture;
}

EraTypes CvUnitCombatInfo::getEra() const
{
	return m_eEra;
}
//Integers
int CvUnitCombatInfo::getAssetMultiplier() const
{
	return m_iAssetMultiplier;
}

int CvUnitCombatInfo::getPowerMultiplier() const
{
	return m_iPowerMultiplier;
}

int CvUnitCombatInfo::getVisibilityChange() const
{
	return m_iVisibilityChange;
}

int CvUnitCombatInfo::getMovesChange() const
{
	return m_iMovesChange;
}

int CvUnitCombatInfo::getMoveDiscountChange() const
{
	return m_iMoveDiscountChange;
}

int CvUnitCombatInfo::getAirRangeChange() const
{
	return m_iAirRangeChange;
}

int CvUnitCombatInfo::getInterceptChange() const
{
	return m_iInterceptChange;
}

int CvUnitCombatInfo::getEvasionChange() const
{
	return m_iEvasionChange;
}

int CvUnitCombatInfo::getWithdrawalChange() const
{
	return m_iWithdrawalChange;
}

int CvUnitCombatInfo::getCargoChange() const
{
	return m_iCargoChange;
}

int CvUnitCombatInfo::getSMCargoChange() const
{
	return m_iSMCargoChange;
}

int CvUnitCombatInfo::getSMCargoVolumeChange() const
{
	return m_iSMCargoVolumeChange;
}

int CvUnitCombatInfo::getSMCargoVolumeModifierChange() const
{
	return m_iSMCargoVolumeModifierChange;
}

int CvUnitCombatInfo::getCollateralDamageChange() const
{
	return m_iCollateralDamageChange;
}

int CvUnitCombatInfo::getBombardRateChange() const
{
	return m_iBombardRateChange;
}

int CvUnitCombatInfo::getFirstStrikesChange() const
{
	return m_iFirstStrikesChange;
}

int CvUnitCombatInfo::getChanceFirstStrikesChange() const
{
	return m_iChanceFirstStrikesChange;
}

int CvUnitCombatInfo::getEnemyHealChange() const
{
	return m_iEnemyHealChange;
}

int CvUnitCombatInfo::getNeutralHealChange() const
{
	return m_iNeutralHealChange;
}

int CvUnitCombatInfo::getFriendlyHealChange() const
{
	return m_iFriendlyHealChange;
}

int CvUnitCombatInfo::getSameTileHealChange() const
{
	return m_iSameTileHealChange;
}

int CvUnitCombatInfo::getAdjacentTileHealChange() const
{
	return m_iAdjacentTileHealChange;
}

int CvUnitCombatInfo::getCombatPercent() const
{
	return m_iCombatPercent;
}

int CvUnitCombatInfo::getCityAttackPercent() const
{
	return m_iCityAttackPercent;
}

int CvUnitCombatInfo::getCityDefensePercent() const
{
	return m_iCityDefensePercent;
}

int CvUnitCombatInfo::getHillsAttackPercent() const
{
	return m_iHillsAttackPercent;
}

int CvUnitCombatInfo::getHillsDefensePercent() const
{
	return m_iHillsDefensePercent;
}

int CvUnitCombatInfo::getHillsWorkPercent() const
{
	return m_iHillsWorkPercent;
}

int CvUnitCombatInfo::getWorkRatePercent() const
{
	return m_iWorkRatePercent;
}

int CvUnitCombatInfo::getRevoltProtection() const
{
	return m_iRevoltProtection;
}

int CvUnitCombatInfo::getCollateralDamageProtection() const
{
	return m_iCollateralDamageProtection;
}

int CvUnitCombatInfo::getPillageChange() const
{
	return m_iPillageChange;
}

int CvUnitCombatInfo::getUpgradeDiscount() const
{
	return m_iUpgradeDiscount;
}

int CvUnitCombatInfo::getExperiencePercent() const
{
	return m_iExperiencePercent;
}

int CvUnitCombatInfo::getKamikazePercent() const
{
	return m_iKamikazePercent;
}

int CvUnitCombatInfo::getAirCombatLimitChange() const
{
	return m_iAirCombatLimitChange;
}

int CvUnitCombatInfo::getCelebrityHappy() const
{
	return m_iCelebrityHappy;
}

int CvUnitCombatInfo::getCollateralDamageLimitChange() const
{
	return m_iCollateralDamageLimitChange;
}

int CvUnitCombatInfo::getCollateralDamageMaxUnitsChange() const
{
	return m_iCollateralDamageMaxUnitsChange;
}

int CvUnitCombatInfo::getCombatLimitChange() const
{
	return m_iCombatLimitChange;
}

int CvUnitCombatInfo::getExtraDropRange() const
{
	return m_iExtraDropRange;
}

int CvUnitCombatInfo::getSurvivorChance() const
{
	return m_iSurvivorChance;
}

int CvUnitCombatInfo::getVictoryAdjacentHeal() const
{
	return m_iVictoryAdjacentHeal;
}

int CvUnitCombatInfo::getVictoryHeal() const
{
	return m_iVictoryHeal;
}

int CvUnitCombatInfo::getVictoryStackHeal() const
{
	return m_iVictoryStackHeal;
}

int CvUnitCombatInfo::getAttackCombatModifierChange() const
{
	return m_iAttackCombatModifierChange;
}

int CvUnitCombatInfo::getDefenseCombatModifierChange() const
{
	return m_iDefenseCombatModifierChange;
}

int CvUnitCombatInfo::getPursuitChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_FIGHT_OR_FLIGHT))
	{
		return 0;
	}
	return m_iPursuitChange;
}

int CvUnitCombatInfo::getEarlyWithdrawChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_FIGHT_OR_FLIGHT))
	{
		return 0;
	}
	return m_iEarlyWithdrawChange;
}

int CvUnitCombatInfo::getVSBarbsChange() const
{
	return m_iVSBarbsChange;
}

int CvUnitCombatInfo::getArmorChange() const
{
	return m_iArmorChange;
}

int CvUnitCombatInfo::getPunctureChange() const
{
	return m_iPunctureChange;
}

int CvUnitCombatInfo::getOverrunChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iOverrunChange;
}

int CvUnitCombatInfo::getRepelChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iRepelChange;
}

int CvUnitCombatInfo::getFortRepelChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iFortRepelChange;
}

int CvUnitCombatInfo::getRepelRetriesChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iRepelRetriesChange;
}

int CvUnitCombatInfo::getUnyieldingChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iUnyieldingChange;
}

int CvUnitCombatInfo::getKnockbackChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iKnockbackChange;
}

int CvUnitCombatInfo::getKnockbackRetriesChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iKnockbackRetriesChange;
}

int CvUnitCombatInfo::getStrAdjperAttChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_BATTLEWORN))
	{
		return 0;
	}
	return m_iStrAdjperAttChange;
}

int CvUnitCombatInfo::getStrAdjperDefChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_BATTLEWORN))
	{
		return 0;
	}
	return m_iStrAdjperDefChange;
}

int CvUnitCombatInfo::getWithdrawAdjperAttChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_BATTLEWORN))
	{
		return 0;
	}
	return m_iWithdrawAdjperAttChange;
}

int CvUnitCombatInfo::getUnnerveChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SAD))
	{
		return 0;
	}
	return m_iUnnerveChange;
}

int CvUnitCombatInfo::getEncloseChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SAD))
	{
		return 0;
	}
	return m_iEncloseChange;
}

int CvUnitCombatInfo::getLungeChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SAD))
	{
		return 0;
	}
	return m_iLungeChange;
}

int CvUnitCombatInfo::getDynamicDefenseChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SAD))
	{
		return 0;
	}
	return m_iDynamicDefenseChange;
}

int CvUnitCombatInfo::getStrengthChange() const
{
	return m_iStrengthChange;
}

int CvUnitCombatInfo::getFortitudeChange() const
{
	return m_iFortitudeChange;
}

int CvUnitCombatInfo::getFrontSupportPercentChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_iFrontSupportPercentChange;
}

int CvUnitCombatInfo::getShortRangeSupportPercentChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_iShortRangeSupportPercentChange;
}

int CvUnitCombatInfo::getMediumRangeSupportPercentChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_iMediumRangeSupportPercentChange;
}

int CvUnitCombatInfo::getLongRangeSupportPercentChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_iLongRangeSupportPercentChange;
}

int CvUnitCombatInfo::getFlankSupportPercentChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_iFlankSupportPercentChange;
}

int CvUnitCombatInfo::getDodgeModifierChange() const
{
	return m_iDodgeModifierChange;
}

int CvUnitCombatInfo::getPrecisionModifierChange() const
{
	return m_iPrecisionModifierChange;
}

int CvUnitCombatInfo::getPowerShotsChange() const
{
	return m_iPowerShotsChange;
}

int CvUnitCombatInfo::getPowerShotCombatModifierChange() const
{
	return m_iPowerShotCombatModifierChange;
}

int CvUnitCombatInfo::getPowerShotPunctureModifierChange() const
{
	return m_iPowerShotPunctureModifierChange;
}

int CvUnitCombatInfo::getPowerShotPrecisionModifierChange() const
{
	return m_iPowerShotPrecisionModifierChange;
}

int CvUnitCombatInfo::getPowerShotCriticalModifierChange() const
{
	return m_iPowerShotCriticalModifierChange;
}

int CvUnitCombatInfo::getCriticalModifierChange() const
{
	return m_iCriticalModifierChange;
}

int CvUnitCombatInfo::getEnduranceChange() const
{
	return m_iEnduranceChange;
}

int CvUnitCombatInfo::getRoundStunProbChange() const
{
	return m_iRoundStunProbChange;
}

int CvUnitCombatInfo::getPoisonProbabilityModifierChange() const
{
	return m_iPoisonProbabilityModifierChange;
}

int CvUnitCombatInfo::getCaptureProbabilityModifierChange() const
{
	return m_iCaptureProbabilityModifierChange;
}

int CvUnitCombatInfo::getCaptureResistanceModifierChange() const
{
	return m_iCaptureResistanceModifierChange;
}

int CvUnitCombatInfo::getHillsWorkModifierChange() const
{
	return m_iHillsWorkModifierChange;
}

int CvUnitCombatInfo::getPeaksWorkModifierChange() const
{
	return m_iPeaksWorkModifierChange;
}

int CvUnitCombatInfo::getBreakdownChanceChange() const
{
	return m_iBreakdownChanceChange;
}

int CvUnitCombatInfo::getBreakdownDamageChange() const
{
	return m_iBreakdownDamageChange;
}

int CvUnitCombatInfo::getTauntChange() const
{
	return m_iTauntChange;
}

int CvUnitCombatInfo::getMaxHPChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iMaxHPChange;
}

int CvUnitCombatInfo::getStrengthModifier() const
{
	return m_iStrengthModifier;
}

int CvUnitCombatInfo::getQualityBase() const
{
	return m_iQualityBase;
}

int CvUnitCombatInfo::getGroupBase() const
{
	return m_iGroupBase;
}

int CvUnitCombatInfo::getSizeBase() const
{
	return m_iSizeBase;
}

int CvUnitCombatInfo::getDamageModifierChange() const
{
	return m_iDamageModifierChange;
}

int CvUnitCombatInfo::getCostModifierChange() const
{
	return m_iCostModifierChange;
}

int CvUnitCombatInfo::getRBombardDamageBase() const
{
	return m_iRBombardDamageBase;
}

int CvUnitCombatInfo::getRBombardDamageLimitBase() const
{
	return m_iRBombardDamageLimitBase;
}

int CvUnitCombatInfo::getRBombardDamageMaxUnitsBase() const
{
	return m_iRBombardDamageMaxUnitsBase;
}

int CvUnitCombatInfo::getDCMBombRangeBase() const
{
	return m_iDCMBombRangeBase;
}

int CvUnitCombatInfo::getDCMBombAccuracyBase() const
{
	return m_iDCMBombAccuracyBase;
}

int CvUnitCombatInfo::getCombatModifierPerSizeMoreChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerSizeMoreChange;
}

int CvUnitCombatInfo::getCombatModifierPerSizeLessChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerSizeLessChange;
}

int CvUnitCombatInfo::getCombatModifierPerVolumeMoreChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerVolumeMoreChange;
}

int CvUnitCombatInfo::getCombatModifierPerVolumeLessChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerVolumeLessChange;
}

int CvUnitCombatInfo::getSelfHealModifier() const
{
	return m_iSelfHealModifier;
}

int CvUnitCombatInfo::getNumHealSupport() const
{
	return m_iNumHealSupport;
}

int CvUnitCombatInfo::getExcileChange() const
{
	return m_iExcileChange;
}

int CvUnitCombatInfo::getPassageChange() const
{
	return m_iPassageChange;
}

int CvUnitCombatInfo::getNoNonOwnedCityEntryChange() const
{
	return m_iNoNonOwnedCityEntryChange;
}

int CvUnitCombatInfo::getBarbCoExistChange() const
{
	return m_iBarbCoExistChange;
}

int CvUnitCombatInfo::getBlendIntoCityChange() const
{
	return m_iBlendIntoCityChange;
}

int CvUnitCombatInfo::getInsidiousnessChange() const
{
	return m_iInsidiousnessChange;
}

int CvUnitCombatInfo::getInvestigationChange() const
{
	return m_iInvestigationChange;
}

int CvUnitCombatInfo::getStealthStrikesChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_WITHOUT_WARNING))
	{
		return 0;
	}
	return m_iStealthStrikesChange;
}

int CvUnitCombatInfo::getStealthCombatModifierChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_WITHOUT_WARNING))
	{
		return 0;
	}
	return m_iStealthCombatModifierChange;
}

int CvUnitCombatInfo::getStealthDefenseChange(bool bForLoad) const
{
	if (!bForLoad && !GC.getGameINLINE().isOption(GAMEOPTION_WITHOUT_WARNING))
	{
		return 0;
	}
	return m_iStealthDefenseChange;
}

int CvUnitCombatInfo::getDefenseOnlyChange() const
{
	return m_iDefenseOnlyChange;
}

int CvUnitCombatInfo::getNoInvisibilityChange() const
{
	return m_iNoInvisibilityChange;
}

int CvUnitCombatInfo::getNoCaptureChange() const
{
	return m_iNoCaptureChange;
}

int CvUnitCombatInfo::getAnimalIgnoresBordersChange() const
{
	return m_iAnimalIgnoresBordersChange;
}

int CvUnitCombatInfo::getNoDefensiveBonusChange() const
{
	return m_iNoDefensiveBonusChange;
}

int CvUnitCombatInfo::getGatherHerdChange() const
{
	return m_iGatherHerdChange;
}

int CvUnitCombatInfo::getReligiousCombatModifierChange() const
{
	return m_iReligiousCombatModifierChange;
}


//Booleans
bool CvUnitCombatInfo::isDefensiveVictoryMove() const
{
	return m_bDefensiveVictoryMove;
}

bool CvUnitCombatInfo::isFreeDrop() const
{
	return m_bFreeDrop;
}

bool CvUnitCombatInfo::isOffensiveVictoryMove() const
{
	return m_bOffensiveVictoryMove;
}

bool CvUnitCombatInfo::isOneUp() const
{
	return m_bOneUp;
}

bool CvUnitCombatInfo::isPillageCulture() const
{
	return m_bPillageCulture;
}

bool CvUnitCombatInfo::isPillageEspionage() const
{
	return m_bPillageEspionage;
}

bool CvUnitCombatInfo::isPillageMarauder() const
{
	return m_bPillageMarauder;
}

bool CvUnitCombatInfo::isPillageOnMove() const
{
	return m_bPillageOnMove;
}

bool CvUnitCombatInfo::isPillageOnVictory() const
{
	return m_bPillageOnVictory;
}

bool CvUnitCombatInfo::isPillageResearch() const
{
	return m_bPillageResearch;
}

bool CvUnitCombatInfo::isBlitz() const
{
	return m_bBlitz;
}

bool CvUnitCombatInfo::isAmphib() const
{
	return m_bAmphib;
}

bool CvUnitCombatInfo::isRiver() const
{
	return m_bRiver;
}

bool CvUnitCombatInfo::isEnemyRoute() const
{
	return m_bEnemyRoute;
}

bool CvUnitCombatInfo::isAlwaysHeal() const
{
	return m_bAlwaysHeal;
}

bool CvUnitCombatInfo::isHillsDoubleMove() const
{
	return m_bHillsDoubleMove;
}

bool CvUnitCombatInfo::isImmuneToFirstStrikes() const
{
	return m_bImmuneToFirstStrikes;
}

bool CvUnitCombatInfo::isStampedeChange() const
{
	return m_bStampedeChange;
}

bool CvUnitCombatInfo::isRemoveStampede() const
{
	return m_bRemoveStampede;
}
bool CvUnitCombatInfo::isOnslaughtChange() const
{
	return m_bOnslaughtChange;
}

bool CvUnitCombatInfo::isMakesDamageCold() const
{
	return m_bMakesDamageCold;
}

bool CvUnitCombatInfo::isMakesDamageNotCold() const
{
	return m_bMakesDamageNotCold;
}

bool CvUnitCombatInfo::isAddsColdImmunity() const
{
	return m_bAddsColdImmunity;
}

bool CvUnitCombatInfo::isRemovesColdImmunity() const
{
	return m_bRemovesColdImmunity;
}

bool CvUnitCombatInfo::isAttackOnlyCitiesAdd() const
{
	return m_bAttackOnlyCitiesAdd;
}

bool CvUnitCombatInfo::isAttackOnlyCitiesSubtract() const
{
	return m_bAttackOnlyCitiesSubtract;
}

bool CvUnitCombatInfo::isIgnoreNoEntryLevelAdd() const
{
	return m_bIgnoreNoEntryLevelAdd;
}

bool CvUnitCombatInfo::isIgnoreNoEntryLevelSubtract() const
{
	return m_bIgnoreNoEntryLevelSubtract;
}

bool CvUnitCombatInfo::isIgnoreZoneofControlAdd() const
{
	return m_bIgnoreZoneofControlAdd;
}

bool CvUnitCombatInfo::isIgnoreZoneofControlSubtract() const
{
	return m_bIgnoreZoneofControlSubtract;
}

bool CvUnitCombatInfo::isFliesToMoveAdd() const
{
	return m_bFliesToMoveAdd;
}

bool CvUnitCombatInfo::isFliesToMoveSubtract() const
{
	return m_bFliesToMoveSubtract;
}

bool CvUnitCombatInfo::isCannotMergeSplit() const
{
	return m_bCannotMergeSplit;
}

bool CvUnitCombatInfo::isRBombardDirect() const
{
	return m_bRBombardDirect;
}

bool CvUnitCombatInfo::isRBombardForceAbility() const
{
	return m_bRBombardForceAbility;
}

bool CvUnitCombatInfo::changesMoveThroughPlots() const
{
	return (isAmphib() ||
			isCanMovePeaks() ||
			isCanLeadThroughPeaks() ||
			(int)m_aiTerrainDoubleMoveChangeTypes.size() > 0 ||
			(int)m_aiFeatureDoubleMoveChangeTypes.size() > 0 ||
			m_bHillsDoubleMove);
}

bool CvUnitCombatInfo::isCanMovePeaks() const
{
	return m_bCanMovePeaks;
}

bool CvUnitCombatInfo::isCanLeadThroughPeaks() const
{
	return m_bCanLeadThroughPeaks;
}

bool CvUnitCombatInfo::isZoneOfControl() const
{
	return m_bZoneOfControl;
}

bool CvUnitCombatInfo::isSpy() const
{
	return m_bSpy;
}

bool CvUnitCombatInfo::isAlwaysInvisible() const
{
	return m_bInvisible;
}

bool CvUnitCombatInfo::isForMilitary() const
{
	return m_bForMilitary;
}

bool CvUnitCombatInfo::isForNavalMilitary() const
{
	return m_bForNavalMilitary;
}

bool CvUnitCombatInfo::isHealsAs() const
{
	return m_bHealsAs;
}

bool CvUnitCombatInfo::isNoSelfHeal() const
{
	return m_bNoSelfHeal;
}
//Arrays
int CvUnitCombatInfo::getDomainModifierPercent(int i) const
{
	FAssertMsg(i < NUM_DOMAIN_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piDomainModifierPercent ? m_piDomainModifierPercent[i] : 0;
}

bool CvUnitCombatInfo::isAnyDomainModifierPercent() const
{
	return m_bAnyDomainModifierPercent;
}

// bool vector with delayed resolution
int CvUnitCombatInfo::getCureAfflictionChangeType(int i) const
{
	return m_aiCureAfflictionChangeTypes[i];
}

int CvUnitCombatInfo::getNumCureAfflictionChangeTypes() const
{
	return (int)m_aiCureAfflictionChangeTypes.size();
}

bool CvUnitCombatInfo::isCureAfflictionChangeType(int i)
{
	if (find(m_aiCureAfflictionChangeTypes.begin(), m_aiCureAfflictionChangeTypes.end(), i) == m_aiCureAfflictionChangeTypes.end())
	{
		return false;
	}
	return true;
}

int CvUnitCombatInfo::getTerrainIgnoreDamageChangeType(int i) const
{
	return m_aiTerrainIgnoreDamageChangeTypes[i];
}

int CvUnitCombatInfo::getNumTerrainIgnoreDamageChangeTypes() const
{
	return (int)m_aiTerrainIgnoreDamageChangeTypes.size();
}

bool CvUnitCombatInfo::isTerrainIgnoreDamageChangeType(int i)
{
	if (find(m_aiTerrainIgnoreDamageChangeTypes.begin(), m_aiTerrainIgnoreDamageChangeTypes.end(), i) == m_aiTerrainIgnoreDamageChangeTypes.end())
	{
		return false;
	}
	return true;
}

int CvUnitCombatInfo::getTerrainDoubleMoveChangeType(int i) const
{
	return m_aiTerrainDoubleMoveChangeTypes[i];
}

int CvUnitCombatInfo::getNumTerrainDoubleMoveChangeTypes() const
{
	return (int)m_aiTerrainDoubleMoveChangeTypes.size();
}

bool CvUnitCombatInfo::isTerrainDoubleMoveChangeType(int i)
{
	if (find(m_aiTerrainDoubleMoveChangeTypes.begin(), m_aiTerrainDoubleMoveChangeTypes.end(), i) == m_aiTerrainDoubleMoveChangeTypes.end())
	{
		return false;
	}
	return true;
}

int CvUnitCombatInfo::getFeatureDoubleMoveChangeType(int i) const
{
	return m_aiFeatureDoubleMoveChangeTypes[i];
}

int CvUnitCombatInfo::getNumFeatureDoubleMoveChangeTypes() const
{
	return (int)m_aiFeatureDoubleMoveChangeTypes.size();
}

bool CvUnitCombatInfo::isFeatureDoubleMoveChangeType(int i)
{
	if (find(m_aiFeatureDoubleMoveChangeTypes.begin(), m_aiFeatureDoubleMoveChangeTypes.end(), i) == m_aiFeatureDoubleMoveChangeTypes.end())
	{
		return false;
	}
	return true;
}
int CvUnitCombatInfo::getOnGameOption(int i) const
{
	return m_aiOnGameOptions[i];
}

int CvUnitCombatInfo::getNumOnGameOptions() const
{
	return (int)m_aiOnGameOptions.size();
}

bool CvUnitCombatInfo::isOnGameOption(int i)
{
	if (find(m_aiOnGameOptions.begin(), m_aiOnGameOptions.end(), i) == m_aiOnGameOptions.end())
	{
		return false;
	}
	return true;
}

int CvUnitCombatInfo::getNotOnGameOption(int i) const
{
	return m_aiNotOnGameOptions[i];
}

int CvUnitCombatInfo::getNumNotOnGameOptions() const
{
	return (int)m_aiNotOnGameOptions.size();
}

bool CvUnitCombatInfo::isNotOnGameOption(int i)
{
	if (find(m_aiNotOnGameOptions.begin(), m_aiNotOnGameOptions.end(), i) == m_aiNotOnGameOptions.end())
	{
		return false;
	}
	return true;
}

int CvUnitCombatInfo::getGGptsforUnitType(int i) const
{
	return m_aiGGptsforUnitTypes[i];
}

int CvUnitCombatInfo::getNumGGptsforUnitTypes() const
{
	return (int)m_aiGGptsforUnitTypes.size();
}

bool CvUnitCombatInfo::isGGptsforUnitType(int i)
{
	if (find(m_aiGGptsforUnitTypes.begin(), m_aiGGptsforUnitTypes.end(), i) == m_aiGGptsforUnitTypes.end())
	{
		return false;
	}
	return true;
}

int CvUnitCombatInfo::getDefaultStatusType(int i) const
{
	return m_aiDefaultStatusTypes[i];
}

int CvUnitCombatInfo::getNumDefaultStatusTypes() const
{
	return (int)m_aiDefaultStatusTypes.size();
}

bool CvUnitCombatInfo::isDefaultStatusType(int i)
{
	if (find(m_aiDefaultStatusTypes.begin(), m_aiDefaultStatusTypes.end(), i) == m_aiDefaultStatusTypes.end())
	{
		return false;
	}
	return true;
}

int CvUnitCombatInfo::getTrapImmunityUnitCombatType(int i) const
{
	return m_aiTrapImmunityUnitCombatTypes[i];
}

int CvUnitCombatInfo::getNumTrapImmunityUnitCombatTypes() const
{
	return (int)m_aiTrapImmunityUnitCombatTypes.size();
}

bool CvUnitCombatInfo::isTrapImmunityUnitCombatType(int i)
{
	if (find(m_aiTrapImmunityUnitCombatTypes.begin(), m_aiTrapImmunityUnitCombatTypes.end(), i) == m_aiTrapImmunityUnitCombatTypes.end())
	{
		return false;
	}
	return true;
}

// int vector utilizing pairing without delayed resolution

int CvUnitCombatInfo::getNumWithdrawOnTerrainTypeChanges() const
{
	return m_aWithdrawOnTerrainTypesChange.size();
}

int CvUnitCombatInfo::getWithdrawOnTerrainTypeChange(int iTerrain) const
{
	for (TerrainModifierArray::const_iterator it = m_aWithdrawOnTerrainTypesChange.begin(); it != m_aWithdrawOnTerrainTypesChange.end(); ++it)
	{
		if ((*it).first == (TerrainTypes)iTerrain)
		{
			return (*it).second;
		}
	}
	return 0;
}

int CvUnitCombatInfo::getNumWithdrawOnFeatureTypeChanges() const
{
	return m_aWithdrawOnFeatureTypesChange.size();
}

int CvUnitCombatInfo::getWithdrawOnFeatureTypeChange(int iFeature) const
{
	for (FeatureModifierArray::const_iterator it = m_aWithdrawOnFeatureTypesChange.begin(); it != m_aWithdrawOnFeatureTypesChange.end(); ++it)
	{
		if ((*it).first == (FeatureTypes)iFeature)
		{
			return (*it).second;
		}
	}
	return 0;
}

int CvUnitCombatInfo::getNumVisibilityIntensityChangeTypes() const
{
	return m_aVisibilityIntensityChangeTypes.size();
}

int CvUnitCombatInfo::getVisibilityIntensityChangeType(int iInvisibility) const
{
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensityChangeTypes.begin(); it != m_aVisibilityIntensityChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitCombatInfo::isVisibilityIntensityChangeType(int iInvisibility)
{
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensityChangeTypes.begin(); it != m_aVisibilityIntensityChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return true;
		}
	}
	return false;
}

int CvUnitCombatInfo::getNumInvisibilityIntensityChangeTypes() const
{
	return m_aInvisibilityIntensityChangeTypes.size();
}

int CvUnitCombatInfo::getInvisibilityIntensityChangeType(int iInvisibility) const
{
	for (InvisibilityArray::const_iterator it = m_aInvisibilityIntensityChangeTypes.begin(); it != m_aInvisibilityIntensityChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitCombatInfo::isInvisibilityIntensityChangeType(int iInvisibility)
{
	for (InvisibilityArray::const_iterator it = m_aInvisibilityIntensityChangeTypes.begin(); it != m_aInvisibilityIntensityChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return true;
		}
	}
	return false;
}

int CvUnitCombatInfo::getNumVisibilityIntensityRangeChangeTypes() const
{
	return m_aVisibilityIntensityRangeChangeTypes.size();
}

int CvUnitCombatInfo::getVisibilityIntensityRangeChangeType(int iInvisibility) const
{
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensityRangeChangeTypes.begin(); it != m_aVisibilityIntensityRangeChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitCombatInfo::isVisibilityIntensityRangeChangeType(int iInvisibility)
{
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensityRangeChangeTypes.begin(); it != m_aVisibilityIntensityRangeChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return true;
		}
	}
	return false;
}

int CvUnitCombatInfo::getNumVisibilityIntensitySameTileChangeTypes() const
{
	return m_aVisibilityIntensitySameTileChangeTypes.size();
}

int CvUnitCombatInfo::getVisibilityIntensitySameTileChangeType(int iInvisibility) const
{
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensitySameTileChangeTypes.begin(); it != m_aVisibilityIntensitySameTileChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitCombatInfo::isVisibilityIntensitySameTileChangeType(int iInvisibility)
{
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensitySameTileChangeTypes.begin(); it != m_aVisibilityIntensitySameTileChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return true;
		}
	}
	return false;
}


int CvUnitCombatInfo::getNumAidChanges() const
{
	return m_aAidChanges.size();
}

int CvUnitCombatInfo::getAidChange(int iProperty) const
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

bool CvUnitCombatInfo::isAidChange(int iProperty)
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
// int vector utilizing struct with delayed resolution
int CvUnitCombatInfo::getNumAfflictionFortitudeChangeModifiers() const
{
	return (int)m_aAfflictionFortitudeChangeModifiers.size();
}

PromotionLineModifier& CvUnitCombatInfo::getAfflictionFortitudeChangeModifier(int iAfflictionLine)
{
	FAssertMsg(iAfflictionLine < (int)m_aAfflictionFortitudeChangeModifiers.size(), "Index out of bounds");
	FAssertMsg(iAfflictionLine > -1, "Index out of bounds");
	return m_aAfflictionFortitudeChangeModifiers[iAfflictionLine];
}

int CvUnitCombatInfo::getNumTerrainAttackChangeModifiers() const
{
	return (int)m_aTerrainAttackChangeModifiers.size();
}

TerrainModifier& CvUnitCombatInfo::getTerrainAttackChangeModifier(int iTerrain)
{
	FAssertMsg(iTerrain < (int)m_aTerrainAttackChangeModifiers.size(), "Index out of bounds");
	FAssertMsg(iTerrain > -1, "Index out of bounds");
	return m_aTerrainAttackChangeModifiers[iTerrain];
}

int CvUnitCombatInfo::getNumTerrainDefenseChangeModifiers() const
{
	return (int)m_aTerrainDefenseChangeModifiers.size();
}

TerrainModifier& CvUnitCombatInfo::getTerrainDefenseChangeModifier(int iTerrain)
{
	FAssertMsg(iTerrain < (int)m_aTerrainDefenseChangeModifiers.size(), "Index out of bounds");
	FAssertMsg(iTerrain > -1, "Index out of bounds");
	return m_aTerrainDefenseChangeModifiers[iTerrain];
}

int CvUnitCombatInfo::getNumTerrainWorkChangeModifiers() const
{
	return (int)m_aTerrainWorkChangeModifiers.size();
}

TerrainModifier& CvUnitCombatInfo::getTerrainWorkChangeModifier(int iTerrain)
{
	FAssertMsg(iTerrain < (int)m_aTerrainWorkChangeModifiers.size(), "Index out of bounds");
	FAssertMsg(iTerrain > -1, "Index out of bounds");
	return m_aTerrainWorkChangeModifiers[iTerrain];
}

int CvUnitCombatInfo::getNumBuildWorkChangeModifiers() const
{
	return (int)m_aBuildWorkChangeModifiers.size();
}

BuildModifier& CvUnitCombatInfo::getBuildWorkChangeModifier(int iBuild)
{
	FAssertMsg(iBuild < (int)m_aBuildWorkChangeModifiers.size(), "Index out of bounds");
	FAssertMsg(iBuild > -1, "Index out of bounds");
	return m_aBuildWorkChangeModifiers[iBuild];
}

int CvUnitCombatInfo::getNumFeatureAttackChangeModifiers() const
{
	return (int)m_aFeatureAttackChangeModifiers.size();
}

FeatureModifier& CvUnitCombatInfo::getFeatureAttackChangeModifier(int iFeature)
{
	FAssertMsg(iFeature < (int)m_aFeatureAttackChangeModifiers.size(), "Index out of bounds");
	FAssertMsg(iFeature > -1, "Index out of bounds");
	return m_aFeatureAttackChangeModifiers[iFeature];
}

int CvUnitCombatInfo::getNumFeatureDefenseChangeModifiers() const
{
	return (int)m_aFeatureDefenseChangeModifiers.size();
}

FeatureModifier& CvUnitCombatInfo::getFeatureDefenseChangeModifier(int iFeature)
{
	FAssertMsg(iFeature < (int)m_aFeatureDefenseChangeModifiers.size(), "Index out of bounds");
	FAssertMsg(iFeature > -1, "Index out of bounds");
	return m_aFeatureDefenseChangeModifiers[iFeature];
}

int CvUnitCombatInfo::getNumFeatureWorkChangeModifiers() const
{
	return (int)m_aFeatureWorkChangeModifiers.size();
}

FeatureModifier& CvUnitCombatInfo::getFeatureWorkChangeModifier(int iFeature)
{
	FAssertMsg(iFeature < (int)m_aFeatureWorkChangeModifiers.size(), "Index out of bounds");
	FAssertMsg(iFeature > -1, "Index out of bounds");
	return m_aFeatureWorkChangeModifiers[iFeature];
}

int CvUnitCombatInfo::getNumUnitCombatChangeModifiers() const
{
	return (int)m_aUnitCombatChangeModifiers.size();
}

UnitCombatModifier& CvUnitCombatInfo::getUnitCombatChangeModifier(int iUnitCombat)
{
	FAssertMsg(iUnitCombat < (int)m_aUnitCombatChangeModifiers.size(), "Index out of bounds");
	FAssertMsg(iUnitCombat > -1, "Index out of bounds");
	return m_aUnitCombatChangeModifiers[iUnitCombat];
}

int CvUnitCombatInfo::getNumFlankingStrengthbyUnitCombatTypesChange() const
{
	return (int)m_aFlankingStrengthbyUnitCombatTypeChange.size();
}

UnitCombatModifier& CvUnitCombatInfo::getFlankingStrengthbyUnitCombatTypeChange(int iUnitCombat)
{
	FAssertMsg(iUnitCombat < (int)m_aFlankingStrengthbyUnitCombatTypeChange.size(), "Index out of bounds");
	FAssertMsg(iUnitCombat > -1, "Index out of bounds");
	return m_aFlankingStrengthbyUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumWithdrawVSUnitCombatTypesChange() const
{
	return (int)m_aWithdrawVSUnitCombatTypeChange.size();
}

UnitCombatModifier& CvUnitCombatInfo::getWithdrawVSUnitCombatTypeChange(int iUnitCombat)
{
	FAssertMsg(iUnitCombat < (int)m_aWithdrawVSUnitCombatTypeChange.size(), "Index out of bounds");
	FAssertMsg(iUnitCombat > -1, "Index out of bounds");
	return m_aWithdrawVSUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumPursuitVSUnitCombatTypesChange() const
{
	return (int)m_aPursuitVSUnitCombatTypeChange.size();
}

UnitCombatModifier& CvUnitCombatInfo::getPursuitVSUnitCombatTypeChange(int iUnitCombat)
{
	FAssertMsg(iUnitCombat < (int)m_aPursuitVSUnitCombatTypeChange.size(), "Index out of bounds");
	FAssertMsg(iUnitCombat > -1, "Index out of bounds");
	//Note: iUnitCombat is just an index.  It is NOT the UnitCombat ID#.
	return m_aPursuitVSUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumRepelVSUnitCombatTypesChange() const
{
	return (int)m_aRepelVSUnitCombatTypeChange.size();
}

UnitCombatModifier& CvUnitCombatInfo::getRepelVSUnitCombatTypeChange(int iUnitCombat)
{
	FAssertMsg(iUnitCombat < (int)m_aRepelVSUnitCombatTypeChange.size(), "Index out of bounds");
	FAssertMsg(iUnitCombat > -1, "Index out of bounds");
	return m_aRepelVSUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumKnockbackVSUnitCombatTypesChange() const
{
	return (int)m_aKnockbackVSUnitCombatTypeChange.size();
}

UnitCombatModifier& CvUnitCombatInfo::getKnockbackVSUnitCombatTypeChange(int iUnitCombat)
{
	FAssertMsg(iUnitCombat < (int)m_aKnockbackVSUnitCombatTypeChange.size(), "Index out of bounds");
	FAssertMsg(iUnitCombat > -1, "Index out of bounds");
	return m_aKnockbackVSUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumPunctureVSUnitCombatTypesChange() const
{
	return (int)m_aPunctureVSUnitCombatTypeChange.size();
}

UnitCombatModifier& CvUnitCombatInfo::getPunctureVSUnitCombatTypeChange(int iUnitCombat)
{
	FAssertMsg(iUnitCombat < (int)m_aPunctureVSUnitCombatTypeChange.size(), "Index out of bounds");
	FAssertMsg(iUnitCombat > -1, "Index out of bounds");
	return m_aPunctureVSUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumArmorVSUnitCombatTypesChange() const
{
	return (int)m_aArmorVSUnitCombatTypeChange.size();
}

UnitCombatModifier& CvUnitCombatInfo::getArmorVSUnitCombatTypeChange(int iUnitCombat)
{
	FAssertMsg(iUnitCombat < (int)m_aArmorVSUnitCombatTypeChange.size(), "Index out of bounds");
	FAssertMsg(iUnitCombat > -1, "Index out of bounds");
	return m_aArmorVSUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumDodgeVSUnitCombatTypesChange() const
{
	return (int)m_aDodgeVSUnitCombatTypeChange.size();
}

UnitCombatModifier& CvUnitCombatInfo::getDodgeVSUnitCombatTypeChange(int iUnitCombat)
{
	FAssertMsg(iUnitCombat < (int)m_aDodgeVSUnitCombatTypeChange.size(), "Index out of bounds");
	FAssertMsg(iUnitCombat > -1, "Index out of bounds");
	return m_aDodgeVSUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumPrecisionVSUnitCombatTypesChange() const
{
	return (int)m_aPrecisionVSUnitCombatTypeChange.size();
}

UnitCombatModifier& CvUnitCombatInfo::getPrecisionVSUnitCombatTypeChange(int iUnitCombat)
{
	FAssertMsg(iUnitCombat < (int)m_aPrecisionVSUnitCombatTypeChange.size(), "Index out of bounds");
	FAssertMsg(iUnitCombat > -1, "Index out of bounds");
	return m_aPrecisionVSUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumCriticalVSUnitCombatTypesChange() const
{
	return (int)m_aCriticalVSUnitCombatTypeChange.size();
}

UnitCombatModifier& CvUnitCombatInfo::getCriticalVSUnitCombatTypeChange(int iUnitCombat)
{
	FAssertMsg(iUnitCombat < (int)m_aCriticalVSUnitCombatTypeChange.size(), "Index out of bounds");
	FAssertMsg(iUnitCombat > -1, "Index out of bounds");
	return m_aCriticalVSUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumRoundStunVSUnitCombatTypesChange() const
{
	return (int)m_aRoundStunVSUnitCombatTypeChange.size();
}

UnitCombatModifier& CvUnitCombatInfo::getRoundStunVSUnitCombatTypeChange(int iUnitCombat)
{
	FAssertMsg(iUnitCombat < (int)m_aRoundStunVSUnitCombatTypeChange.size(), "Index out of bounds");
	FAssertMsg(iUnitCombat > -1, "Index out of bounds");
	return m_aRoundStunVSUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumTrapAvoidanceUnitCombatTypes() const
{
	return (int)m_aTrapAvoidanceUnitCombatTypes.size();
}

UnitCombatModifier& CvUnitCombatInfo::getTrapAvoidanceUnitCombatType(int iIndex)
{
	return m_aTrapAvoidanceUnitCombatTypes[iIndex];
}

int CvUnitCombatInfo::getNumAfflictOnAttackChangeTypes() const
{
	return (int)m_aAfflictOnAttackChangeTypes.size();
}

AfflictOnAttackChange& CvUnitCombatInfo::getAfflictOnAttackChangeType(int iAfflictionLine)
{
	FAssertMsg(iAfflictionLine < (int)m_aAfflictOnAttackChangeTypes.size(), "Index out of bounds");
	FAssertMsg(iAfflictionLine > -1, "Index out of bounds");
	return m_aAfflictOnAttackChangeTypes[iAfflictionLine];
}

int CvUnitCombatInfo::getNumInvisibleTerrainChanges() const
{
	return (int)m_aInvisibleTerrainChanges.size();
}

InvisibleTerrainChanges& CvUnitCombatInfo::getInvisibleTerrainChange(int iIndex)
{
	return m_aInvisibleTerrainChanges[iIndex];
}

int CvUnitCombatInfo::getNumInvisibleFeatureChanges() const
{
	return (int)m_aInvisibleFeatureChanges.size();
}

InvisibleFeatureChanges& CvUnitCombatInfo::getInvisibleFeatureChange(int iIndex)
{
	return m_aInvisibleFeatureChanges[iIndex];
}

int CvUnitCombatInfo::getNumInvisibleImprovementChanges() const
{
	return (int)m_aInvisibleImprovementChanges.size();
}

InvisibleImprovementChanges& CvUnitCombatInfo::getInvisibleImprovementChange(int iIndex)
{
	return m_aInvisibleImprovementChanges[iIndex];
}

int CvUnitCombatInfo::getNumVisibleTerrainChanges() const
{
	return (int)m_aVisibleTerrainChanges.size();
}

InvisibleTerrainChanges& CvUnitCombatInfo::getVisibleTerrainChange(int iIndex)
{
	return m_aVisibleTerrainChanges[iIndex];
}

int CvUnitCombatInfo::getNumVisibleFeatureChanges() const
{
	return (int)m_aVisibleFeatureChanges.size();
}

InvisibleFeatureChanges& CvUnitCombatInfo::getVisibleFeatureChange(int iIndex)
{
	return m_aVisibleFeatureChanges[iIndex];
}

int CvUnitCombatInfo::getNumVisibleImprovementChanges() const
{
	return (int)m_aVisibleImprovementChanges.size();
}

InvisibleImprovementChanges& CvUnitCombatInfo::getVisibleImprovementChange(int iIndex)
{
	return m_aVisibleImprovementChanges[iIndex];
}

int CvUnitCombatInfo::getNumVisibleTerrainRangeChanges() const
{
	return (int)m_aVisibleTerrainRangeChanges.size();
}

InvisibleTerrainChanges& CvUnitCombatInfo::getVisibleTerrainRangeChange(int iIndex)
{
	return m_aVisibleTerrainRangeChanges[iIndex];
}

int CvUnitCombatInfo::getNumVisibleFeatureRangeChanges() const
{
	return (int)m_aVisibleFeatureRangeChanges.size();
}

InvisibleFeatureChanges& CvUnitCombatInfo::getVisibleFeatureRangeChange(int iIndex)
{
	return m_aVisibleFeatureRangeChanges[iIndex];
}

int CvUnitCombatInfo::getNumVisibleImprovementRangeChanges() const
{
	return (int)m_aVisibleImprovementRangeChanges.size();
}

InvisibleImprovementChanges& CvUnitCombatInfo::getVisibleImprovementRangeChange(int iIndex)
{
	return m_aVisibleImprovementRangeChanges[iIndex];
}

int CvUnitCombatInfo::getNumDistanceAttackCommunicabilityTypeChanges() const
{
	return (int)m_aDistanceAttackCommunicabilityTypeChanges.size();
}

AfflictionLineChanges& CvUnitCombatInfo::getDistanceAttackCommunicabilityTypeChange(int iIndex)
{
	return m_aDistanceAttackCommunicabilityTypeChanges[iIndex];
}

bool CvUnitCombatInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	CvString szTextVal2;

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

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

	// Textual References

	if ( pXML->GetOptionalChildXmlValByName(szTextVal, L"ReligionType") )
		m_eReligion = (ReligionTypes)pXML->GetInfoClass(szTextVal);
	else
		m_eReligion = NO_RELIGION;

	if ( pXML->GetOptionalChildXmlValByName(szTextVal, L"CultureType") )
		m_eCulture = (BonusTypes)pXML->GetInfoClass(szTextVal);
	else
		m_eCulture = NO_BONUS;

	if ( pXML->GetOptionalChildXmlValByName(szTextVal, L"EraType") )
		m_eEra = (EraTypes)pXML->GetInfoClass(szTextVal);
	else
		m_eEra = NO_ERA;

	//Integers
	pXML->GetOptionalChildXmlValByName(&m_iAssetMultiplier, L"iAssetMultiplierPercent");
	pXML->GetOptionalChildXmlValByName(&m_iPowerMultiplier, L"iPowerMultiplierPercent");
	pXML->GetOptionalChildXmlValByName(&m_iVisibilityChange, L"iVisibilityChange");
	pXML->GetOptionalChildXmlValByName(&m_iMovesChange, L"iMovesChange");
	pXML->GetOptionalChildXmlValByName(&m_iMoveDiscountChange, L"iMoveDiscountChange");
	pXML->GetOptionalChildXmlValByName(&m_iAirRangeChange, L"iAirRangeChange");
	pXML->GetOptionalChildXmlValByName(&m_iInterceptChange, L"iInterceptChange");
	pXML->GetOptionalChildXmlValByName(&m_iEvasionChange, L"iEvasionChange");
	pXML->GetOptionalChildXmlValByName(&m_iWithdrawalChange, L"iWithdrawalChange");
	pXML->GetOptionalChildXmlValByName(&m_iCargoChange, L"iCargoChange");
	pXML->GetOptionalChildXmlValByName(&m_iSMCargoChange, L"iSMCargoChange");
	pXML->GetOptionalChildXmlValByName(&m_iSMCargoVolumeChange, L"iSMCargoVolumeChange");
	pXML->GetOptionalChildXmlValByName(&m_iSMCargoVolumeModifierChange, L"iSMCargoVolumeModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iCollateralDamageChange, L"iCollateralDamageChange");
	pXML->GetOptionalChildXmlValByName(&m_iBombardRateChange, L"iBombardRateChange");
	pXML->GetOptionalChildXmlValByName(&m_iFirstStrikesChange, L"iFirstStrikesChange");
	pXML->GetOptionalChildXmlValByName(&m_iChanceFirstStrikesChange, L"iChanceFirstStrikesChange");
	pXML->GetOptionalChildXmlValByName(&m_iEnemyHealChange, L"iEnemyHealChange");
	pXML->GetOptionalChildXmlValByName(&m_iNeutralHealChange, L"iNeutralHealChange");
	pXML->GetOptionalChildXmlValByName(&m_iFriendlyHealChange, L"iFriendlyHealChange");
	pXML->GetOptionalChildXmlValByName(&m_iSameTileHealChange, L"iSameTileHealChange");
	pXML->GetOptionalChildXmlValByName(&m_iAdjacentTileHealChange, L"iAdjacentTileHealChange");
	pXML->GetOptionalChildXmlValByName(&m_iCombatPercent, L"iCombatPercent");
	pXML->GetOptionalChildXmlValByName(&m_iCityAttackPercent, L"iCityAttack");
	pXML->GetOptionalChildXmlValByName(&m_iCityDefensePercent, L"iCityDefense");
	pXML->GetOptionalChildXmlValByName(&m_iHillsAttackPercent, L"iHillsAttack");
	pXML->GetOptionalChildXmlValByName(&m_iHillsDefensePercent, L"iHillsDefense");
	pXML->GetOptionalChildXmlValByName(&m_iHillsWorkPercent, L"iHillsWork");
	pXML->GetOptionalChildXmlValByName(&m_iWorkRatePercent, L"iWorkRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iRevoltProtection, L"iRevoltProtection");
	pXML->GetOptionalChildXmlValByName(&m_iCollateralDamageProtection, L"iCollateralDamageProtection");
	pXML->GetOptionalChildXmlValByName(&m_iPillageChange, L"iPillageChange");
	pXML->GetOptionalChildXmlValByName(&m_iUpgradeDiscount, L"iUpgradeDiscount");
	pXML->GetOptionalChildXmlValByName(&m_iExperiencePercent, L"iExperiencePercent");
	pXML->GetOptionalChildXmlValByName(&m_iKamikazePercent, L"iKamikazePercent");
	pXML->GetOptionalChildXmlValByName(&m_iAirCombatLimitChange, L"iAirCombatLimitChange");
	pXML->GetOptionalChildXmlValByName(&m_iCelebrityHappy, L"iCelebrityHappy");
	pXML->GetOptionalChildXmlValByName(&m_iCollateralDamageLimitChange, L"iCollateralDamageLimitChange");
	pXML->GetOptionalChildXmlValByName(&m_iCollateralDamageMaxUnitsChange, L"iCollateralDamageMaxUnitsChange");
	pXML->GetOptionalChildXmlValByName(&m_iCombatLimitChange, L"iCombatLimitChange");
	pXML->GetOptionalChildXmlValByName(&m_iExtraDropRange, L"iExtraDropRange");
	pXML->GetOptionalChildXmlValByName(&m_iSurvivorChance, L"iSurvivorChance");
	pXML->GetOptionalChildXmlValByName(&m_iVictoryAdjacentHeal, L"iVictoryAdjacentHeal");
	pXML->GetOptionalChildXmlValByName(&m_iVictoryHeal, L"iVictoryHeal");
	pXML->GetOptionalChildXmlValByName(&m_iVictoryStackHeal, L"iVictoryStackHeal");
	pXML->GetOptionalChildXmlValByName(&m_iAttackCombatModifierChange, L"iAttackCombatModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iDefenseCombatModifierChange, L"iDefenseCombatModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iPursuitChange, L"iPursuitChange");
	pXML->GetOptionalChildXmlValByName(&m_iEarlyWithdrawChange, L"iEarlyWithdrawChange");
	pXML->GetOptionalChildXmlValByName(&m_iVSBarbsChange, L"iVSBarbsChange");
	pXML->GetOptionalChildXmlValByName(&m_iArmorChange, L"iArmorChange");
	pXML->GetOptionalChildXmlValByName(&m_iPunctureChange, L"iPunctureChange");
	pXML->GetOptionalChildXmlValByName(&m_iOverrunChange, L"iOverrunChange");
	pXML->GetOptionalChildXmlValByName(&m_iRepelChange, L"iRepelChange");
	pXML->GetOptionalChildXmlValByName(&m_iFortRepelChange, L"iFortRepelChange");
	pXML->GetOptionalChildXmlValByName(&m_iRepelRetriesChange, L"iRepelRetriesChange");
	pXML->GetOptionalChildXmlValByName(&m_iUnyieldingChange, L"iUnyieldingChange");
	pXML->GetOptionalChildXmlValByName(&m_iKnockbackChange, L"iKnockbackChange");
	pXML->GetOptionalChildXmlValByName(&m_iKnockbackRetriesChange, L"iKnockbackRetriesChange");
	pXML->GetOptionalChildXmlValByName(&m_iStrAdjperAttChange, L"iStrAdjperAttChange");
	pXML->GetOptionalChildXmlValByName(&m_iStrAdjperDefChange, L"iStrAdjperDefChange");
	pXML->GetOptionalChildXmlValByName(&m_iWithdrawAdjperAttChange, L"iWithdrawAdjperAttChange");
	pXML->GetOptionalChildXmlValByName(&m_iUnnerveChange, L"iUnnerveChange");
	pXML->GetOptionalChildXmlValByName(&m_iEncloseChange, L"iEncloseChange");
	pXML->GetOptionalChildXmlValByName(&m_iLungeChange, L"iLungeChange");
	pXML->GetOptionalChildXmlValByName(&m_iDynamicDefenseChange, L"iDynamicDefenseChange");
	pXML->GetOptionalChildXmlValByName(&m_iStrengthChange, L"iStrengthChange");
	pXML->GetOptionalChildXmlValByName(&m_iFortitudeChange, L"iFortitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iFrontSupportPercentChange, L"iFrontSupportPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iShortRangeSupportPercentChange, L"iShortRangeSupportPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iMediumRangeSupportPercentChange, L"iMediumRangeSupportPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iLongRangeSupportPercentChange, L"iLongRangeSupportPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iFlankSupportPercentChange, L"iFlankSupportPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iDodgeModifierChange, L"iDodgeModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iPrecisionModifierChange, L"iPrecisionModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iPowerShotsChange, L"iPowerShotsChange");
	pXML->GetOptionalChildXmlValByName(&m_iPowerShotCombatModifierChange, L"iPowerShotCombatModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iPowerShotPunctureModifierChange, L"iPowerShotPunctureModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iPowerShotPrecisionModifierChange, L"iPowerShotPrecisionModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iPowerShotCriticalModifierChange, L"iPowerShotCriticalModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iCriticalModifierChange, L"iCriticalModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iEnduranceChange, L"iEnduranceChange");
	pXML->GetOptionalChildXmlValByName(&m_iRoundStunProbChange, L"iRoundStunProbChange");
	pXML->GetOptionalChildXmlValByName(&m_iPoisonProbabilityModifierChange, L"iPoisonProbabilityModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iCaptureProbabilityModifierChange, L"iCaptureProbabilityModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iCaptureResistanceModifierChange, L"iCaptureResistanceModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iHillsWorkModifierChange, L"iHillsWorkModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iPeaksWorkModifierChange, L"iPeaksWorkModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iBreakdownChanceChange, L"iBreakdownChanceChange");
	pXML->GetOptionalChildXmlValByName(&m_iBreakdownDamageChange, L"iBreakdownDamageChange");
	pXML->GetOptionalChildXmlValByName(&m_iTauntChange, L"iTauntChange");
	pXML->GetOptionalChildXmlValByName(&m_iMaxHPChange, L"iMaxHPChange");
	pXML->GetOptionalChildXmlValByName(&m_iStrengthModifier, L"iStrengthModifier");
	pXML->GetOptionalChildXmlValByName(&m_iQualityBase, L"iQualityBase", -10);
	pXML->GetOptionalChildXmlValByName(&m_iGroupBase, L"iGroupBase", -10);
	pXML->GetOptionalChildXmlValByName(&m_iSizeBase, L"iSizeBase", -10);
	pXML->GetOptionalChildXmlValByName(&m_iDamageModifierChange, L"iDamageModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iCostModifierChange, L"iCostModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iRBombardDamageBase, L"iRBombardDamageBase");
	pXML->GetOptionalChildXmlValByName(&m_iRBombardDamageLimitBase, L"iRBombardDamageLimitBase");
	pXML->GetOptionalChildXmlValByName(&m_iRBombardDamageMaxUnitsBase, L"iRBombardDamageMaxUnitsBase");
	pXML->GetOptionalChildXmlValByName(&m_iDCMBombRangeBase, L"iDCMBombRangeBase");
	pXML->GetOptionalChildXmlValByName(&m_iDCMBombAccuracyBase, L"iDCMBombAccuracyBase");
	pXML->GetOptionalChildXmlValByName(&m_iCombatModifierPerSizeMoreChange, L"iCombatModifierPerSizeMoreChange");
	pXML->GetOptionalChildXmlValByName(&m_iCombatModifierPerSizeLessChange, L"iCombatModifierPerSizeLessChange");
	pXML->GetOptionalChildXmlValByName(&m_iCombatModifierPerVolumeMoreChange, L"iCombatModifierPerVolumeMoreChange");
	pXML->GetOptionalChildXmlValByName(&m_iCombatModifierPerVolumeLessChange, L"iCombatModifierPerVolumeLessChange");
	pXML->GetOptionalChildXmlValByName(&m_iSelfHealModifier, L"iSelfHealModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNumHealSupport, L"iNumHealSupport");
	pXML->GetOptionalChildXmlValByName(&m_iExcileChange, L"iExcileChange");
	pXML->GetOptionalChildXmlValByName(&m_iPassageChange, L"iPassageChange");
	pXML->GetOptionalChildXmlValByName(&m_iNoNonOwnedCityEntryChange, L"iNoNonOwnedCityEntryChange");
	pXML->GetOptionalChildXmlValByName(&m_iBarbCoExistChange, L"iBarbCoExistChange");
	pXML->GetOptionalChildXmlValByName(&m_iBlendIntoCityChange, L"iBlendIntoCityChange");
	pXML->GetOptionalChildXmlValByName(&m_iInsidiousnessChange, L"iInsidiousnessChange");
	pXML->GetOptionalChildXmlValByName(&m_iInvestigationChange, L"iInvestigationChange");
	pXML->GetOptionalChildXmlValByName(&m_iStealthStrikesChange, L"iStealthStrikesChange");
	pXML->GetOptionalChildXmlValByName(&m_iStealthCombatModifierChange, L"iStealthCombatModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iStealthDefenseChange, L"iStealthDefenseChange");
	pXML->GetOptionalChildXmlValByName(&m_iDefenseOnlyChange, L"iDefenseOnlyChange");
	pXML->GetOptionalChildXmlValByName(&m_iNoInvisibilityChange, L"iNoInvisibilityChange");
	pXML->GetOptionalChildXmlValByName(&m_iNoCaptureChange, L"iNoCaptureChange");
	pXML->GetOptionalChildXmlValByName(&m_iAnimalIgnoresBordersChange, L"iAnimalIgnoresBordersChange");
	pXML->GetOptionalChildXmlValByName(&m_iNoDefensiveBonusChange, L"iNoDefensiveBonusChange");
	pXML->GetOptionalChildXmlValByName(&m_iGatherHerdChange, L"iGatherHerdChange");
	pXML->GetOptionalChildXmlValByName(&m_iReligiousCombatModifierChange, L"iReligiousCombatModifierChange");

	//Booleans
	pXML->GetOptionalChildXmlValByName(&m_bDefensiveVictoryMove, L"bDefensiveVictoryMove");
	pXML->GetOptionalChildXmlValByName(&m_bFreeDrop, L"bFreeDrop");
	pXML->GetOptionalChildXmlValByName(&m_bOffensiveVictoryMove, L"bOffensiveVictoryMove");
	pXML->GetOptionalChildXmlValByName(&m_bOneUp, L"bOneUp");
	pXML->GetOptionalChildXmlValByName(&m_bPillageCulture, L"bPillageCulture");
	pXML->GetOptionalChildXmlValByName(&m_bPillageEspionage, L"bPillageEspionage");
	pXML->GetOptionalChildXmlValByName(&m_bPillageMarauder, L"bPillageMarauder");
	pXML->GetOptionalChildXmlValByName(&m_bPillageOnMove, L"bPillageOnMove");
	pXML->GetOptionalChildXmlValByName(&m_bPillageOnVictory, L"bPillageOnVictory");
	pXML->GetOptionalChildXmlValByName(&m_bPillageResearch, L"bPillageResearch");
	pXML->GetOptionalChildXmlValByName(&m_bBlitz, L"bBlitz");
	pXML->GetOptionalChildXmlValByName(&m_bAmphib, L"bAmphib");
	pXML->GetOptionalChildXmlValByName(&m_bRiver, L"bRiver");
	pXML->GetOptionalChildXmlValByName(&m_bEnemyRoute, L"bEnemyRoute");
	pXML->GetOptionalChildXmlValByName(&m_bAlwaysHeal, L"bAlwaysHeal");
	pXML->GetOptionalChildXmlValByName(&m_bHillsDoubleMove, L"bHillsDoubleMove");
	pXML->GetOptionalChildXmlValByName(&m_bImmuneToFirstStrikes, L"bImmuneToFirstStrikes");
	pXML->GetOptionalChildXmlValByName(&m_bStampedeChange, L"bStampedeChange");
	pXML->GetOptionalChildXmlValByName(&m_bRemoveStampede, L"bRemoveStampede");
	pXML->GetOptionalChildXmlValByName(&m_bOnslaughtChange, L"bOnslaughtChange");
	pXML->GetOptionalChildXmlValByName(&m_bMakesDamageCold, L"bMakesDamageCold");
	pXML->GetOptionalChildXmlValByName(&m_bMakesDamageNotCold, L"bMakesDamageNotCold");
	pXML->GetOptionalChildXmlValByName(&m_bAddsColdImmunity, L"bAddsColdImmunity");
	pXML->GetOptionalChildXmlValByName(&m_bRemovesColdImmunity, L"bRemovesColdImmunity");
	pXML->GetOptionalChildXmlValByName(&m_bAttackOnlyCitiesAdd, L"bAttackOnlyCitiesAdd");
	pXML->GetOptionalChildXmlValByName(&m_bAttackOnlyCitiesSubtract, L"bAttackOnlyCitiesSubtract");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreNoEntryLevelAdd, L"bIgnoreNoEntryLevelAdd");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreNoEntryLevelSubtract, L"bIgnoreNoEntryLevelSubtract");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreZoneofControlAdd, L"bIgnoreZoneofControlAdd");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreZoneofControlSubtract, L"bIgnoreZoneofControlSubtract");
	pXML->GetOptionalChildXmlValByName(&m_bFliesToMoveAdd, L"bFliesToMoveAdd");
	pXML->GetOptionalChildXmlValByName(&m_bFliesToMoveSubtract, L"bFliesToMoveSubtract");
	pXML->GetOptionalChildXmlValByName(&m_bCanMovePeaks, L"bCanMovePeaks");
	pXML->GetOptionalChildXmlValByName(&m_bCanLeadThroughPeaks, L"bCanLeadThroughPeaks");
	pXML->GetOptionalChildXmlValByName(&m_bZoneOfControl, L"bZoneOfControl");
	pXML->GetOptionalChildXmlValByName(&m_bSpy, L"bSpy");
	pXML->GetOptionalChildXmlValByName(&m_bInvisible, L"bInvisible");
	pXML->GetOptionalChildXmlValByName(&m_bCannotMergeSplit, L"bCannotMergeSplit");
	pXML->GetOptionalChildXmlValByName(&m_bRBombardDirect, L"bRBombardDirect");
	pXML->GetOptionalChildXmlValByName(&m_bRBombardForceAbility, L"bRBombardForceAbility");
	pXML->GetOptionalChildXmlValByName(&m_bInvisible, L"bInvisible");
	pXML->GetOptionalChildXmlValByName(&m_bForMilitary, L"bForMilitary");
	pXML->GetOptionalChildXmlValByName(&m_bForNavalMilitary, L"bForNavalMilitary");
	pXML->GetOptionalChildXmlValByName(&m_bHealsAs, L"bHealsAs");
	pXML->GetOptionalChildXmlValByName(&m_bNoSelfHeal, L"bNoSelfHeal");

	//Arrays
	pXML->SetVariableListTagPair(&m_piDomainModifierPercent, L"DomainMods", NUM_DOMAIN_TYPES);

	// bool vector with delayed resolution
	pXML->SetOptionalIntVectorWithDelayedResolution(m_aiCureAfflictionChangeTypes, L"CureAfflictionChangeTypes");
	pXML->SetOptionalIntVector(&m_aiTerrainIgnoreDamageChangeTypes, L"TerrainIgnoreDamageChangeTypes");
	pXML->SetOptionalIntVector(&m_aiTerrainDoubleMoveChangeTypes, L"TerrainDoubleMoveChangeTypes");
	pXML->SetOptionalIntVector(&m_aiFeatureDoubleMoveChangeTypes, L"FeatureDoubleMoveChangeTypes");
	pXML->SetOptionalIntVector(&m_aiOnGameOptions, L"OnGameOptions");
	pXML->SetOptionalIntVector(&m_aiNotOnGameOptions, L"NotOnGameOptions");
	pXML->SetOptionalIntVectorWithDelayedResolution(m_aiGGptsforUnitTypes, L"GGptsforUnitTypes");
	pXML->SetOptionalIntVectorWithDelayedResolution(m_aiDefaultStatusTypes, L"DefaultStatusTypes");
	pXML->SetOptionalIntVector(&m_aiTrapImmunityUnitCombatTypes, L"TrapImmunityUnitCombatTypes");

	// int vector utilizing pairing without delayed resolution
	m_aWithdrawOnTerrainTypesChange.clear();
	if (pXML->TryMoveToXmlFirstChild(L"WithdrawModifierOnTerrainTypeChanges"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						TerrainTypes eTerrain = (TerrainTypes)pXML->GetInfoClass(szTextVal);
						int iModifier;
						pXML->GetNextXmlVal(&iModifier);
						m_aWithdrawOnTerrainTypesChange.push_back(std::make_pair(eTerrain, iModifier));

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

	m_aWithdrawOnFeatureTypesChange.clear();
	if (pXML->TryMoveToXmlFirstChild(L"WithdrawModifierOnFeatureTypeChanges"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						FeatureTypes eFeature = (FeatureTypes)pXML->GetInfoClass(szTextVal);
						int iModifier;
						pXML->GetNextXmlVal(&iModifier);
						m_aWithdrawOnFeatureTypesChange.push_back(std::make_pair(eFeature, iModifier));

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

	m_aVisibilityIntensityChangeTypes.clear();
	if (pXML->TryMoveToXmlFirstChild(L"VisibilityIntensityChangeTypes"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						InvisibleTypes eInvisible = (InvisibleTypes)pXML->GetInfoClass(szTextVal);
						int iChange;
						pXML->GetNextXmlVal(&iChange);
						m_aVisibilityIntensityChangeTypes.push_back(std::make_pair(eInvisible, iChange));

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

	m_aInvisibilityIntensityChangeTypes.clear();
	if (pXML->TryMoveToXmlFirstChild(L"InvisibilityIntensityChangeTypes"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						InvisibleTypes eInvisible = (InvisibleTypes)pXML->GetInfoClass(szTextVal);
						int iChange;
						pXML->GetNextXmlVal(&iChange);
						m_aInvisibilityIntensityChangeTypes.push_back(std::make_pair(eInvisible, iChange));

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

	m_aVisibilityIntensityRangeChangeTypes.clear();
	if (pXML->TryMoveToXmlFirstChild(L"VisibilityIntensityRangeChangeTypes"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						InvisibleTypes eInvisible = (InvisibleTypes)pXML->GetInfoClass(szTextVal);
						int iChange;
						pXML->GetNextXmlVal(&iChange);
						m_aVisibilityIntensityRangeChangeTypes.push_back(std::make_pair(eInvisible, iChange));

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

	m_aVisibilityIntensitySameTileChangeTypes.clear();
	if (pXML->TryMoveToXmlFirstChild(L"VisibilityIntensitySameTileChangeTypes"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						InvisibleTypes eInvisible = (InvisibleTypes)pXML->GetInfoClass(szTextVal);
						int iChange;
						pXML->GetNextXmlVal(&iChange);
						m_aVisibilityIntensitySameTileChangeTypes.push_back(std::make_pair(eInvisible, iChange));

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

	pXML->SetOptionalPairVector<AidArray, PropertyTypes, int>(&m_aAidChanges, L"AidChanges");

	// int vector utilizing struct with delayed resolution
	if(pXML->TryMoveToXmlFirstChild(L"AfflictionFortitudeChangeModifiers"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"AfflictionFortitudeChangeModifier" );
		m_aAfflictionFortitudeChangeModifiers.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"AfflictionFortitudeChangeModifier"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"PromotionLineType");
					pXML->GetChildXmlValByName(&(m_aAfflictionFortitudeChangeModifiers[i].iModifier), L"iModifier");
					GC.addDelayedResolution((int*)&(m_aAfflictionFortitudeChangeModifiers[i].ePromotionLine), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"AfflictionFortitudeChangeModifier"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"TerrainAttackChangeModifiers"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"TerrainAttackChangeModifier" );
		m_aTerrainAttackChangeModifiers.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"TerrainAttackChangeModifier"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"TerrainType");
					m_aTerrainAttackChangeModifiers[i].eTerrain = (TerrainTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aTerrainAttackChangeModifiers[i].iModifier), L"iModifier");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"TerrainAttackChangeModifier"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"TerrainDefenseChangeModifiers"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"TerrainDefenseChangeModifier" );
		m_aTerrainDefenseChangeModifiers.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"TerrainDefenseChangeModifier"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"TerrainType");
					m_aTerrainDefenseChangeModifiers[i].eTerrain = (TerrainTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aTerrainDefenseChangeModifiers[i].iModifier), L"iModifier");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"TerrainDefenseChangeModifier"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"TerrainWorkChangeModifiers"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"TerrainWorkChangeModifier" );
		m_aTerrainWorkChangeModifiers.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"TerrainWorkChangeModifier"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"TerrainType");
					m_aTerrainWorkChangeModifiers[i].eTerrain = (TerrainTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aTerrainWorkChangeModifiers[i].iModifier), L"iModifier");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"TerrainWorkChangeModifier"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"BuildWorkChangeModifiers"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"BuildWorkChangeModifier" );
		m_aBuildWorkChangeModifiers.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"BuildWorkChangeModifier"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"BuildType");
					pXML->GetChildXmlValByName(&(m_aBuildWorkChangeModifiers[i].iModifier), L"iModifier");
					GC.addDelayedResolution((int*)&(m_aBuildWorkChangeModifiers[i].eBuild), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"BuildWorkChangeModifier"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"FeatureAttackChangeModifiers"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"FeatureAttackChangeModifier" );
		m_aFeatureAttackChangeModifiers.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"FeatureAttackChangeModifier"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"FeatureType");
					m_aFeatureAttackChangeModifiers[i].eFeature = (FeatureTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aFeatureAttackChangeModifiers[i].iModifier), L"iModifier");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"FeatureAttackChangeModifier"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"FeatureDefenseChangeModifiers"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"FeatureDefenseChangeModifier" );
		m_aFeatureDefenseChangeModifiers.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"FeatureDefenseChangeModifier"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"FeatureType");
					m_aFeatureDefenseChangeModifiers[i].eFeature = (FeatureTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aFeatureDefenseChangeModifiers[i].iModifier), L"iModifier");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"FeatureDefenseChangeModifier"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"FeatureWorkChangeModifiers"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"FeatureWorkChangeModifier" );
		m_aFeatureWorkChangeModifiers.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"FeatureWorkChangeModifier"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"FeatureType");
					m_aFeatureWorkChangeModifiers[i].eFeature = (FeatureTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aFeatureWorkChangeModifiers[i].iModifier), L"iModifier");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"FeatureWorkChangeModifier"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"UnitCombatChangeModifiers"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"UnitCombatChangeModifier" );
		m_aUnitCombatChangeModifiers.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"UnitCombatChangeModifier"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					pXML->GetChildXmlValByName(&(m_aUnitCombatChangeModifiers[i].iModifier), L"iModifier");
					GC.addDelayedResolution((int*)&(m_aUnitCombatChangeModifiers[i].eUnitCombat), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"UnitCombatChangeModifier"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"FlankingStrengthbyUnitCombatTypesChanges"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"FlankingStrengthbyUnitCombatTypesChange" );
		m_aFlankingStrengthbyUnitCombatTypeChange.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"FlankingStrengthbyUnitCombatTypesChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					pXML->GetChildXmlValByName(&(m_aFlankingStrengthbyUnitCombatTypeChange[i].iModifier), L"iModifier");
					GC.addDelayedResolution((int*)&(m_aFlankingStrengthbyUnitCombatTypeChange[i].eUnitCombat), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"FlankingStrengthbyUnitCombatTypesChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"WithdrawVSUnitCombatTypesChanges"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"WithdrawVSUnitCombatTypesChange" );
		m_aWithdrawVSUnitCombatTypeChange.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"WithdrawVSUnitCombatTypesChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					pXML->GetChildXmlValByName(&(m_aWithdrawVSUnitCombatTypeChange[i].iModifier), L"iModifier");
					GC.addDelayedResolution((int*)&(m_aWithdrawVSUnitCombatTypeChange[i].eUnitCombat), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"WithdrawVSUnitCombatTypesChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"PursuitVSUnitCombatTypesChanges"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"PursuitVSUnitCombatTypesChange" );
		m_aPursuitVSUnitCombatTypeChange.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"PursuitVSUnitCombatTypesChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					pXML->GetChildXmlValByName(&(m_aPursuitVSUnitCombatTypeChange[i].iModifier), L"iModifier");
					GC.addDelayedResolution((int*)&(m_aPursuitVSUnitCombatTypeChange[i].eUnitCombat), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"PursuitVSUnitCombatTypesChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"RepelVSUnitCombatTypesChanges"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"RepelVSUnitCombatTypesChange" );
		m_aRepelVSUnitCombatTypeChange.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"RepelVSUnitCombatTypesChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					pXML->GetChildXmlValByName(&(m_aRepelVSUnitCombatTypeChange[i].iModifier), L"iModifier");
					GC.addDelayedResolution((int*)&(m_aRepelVSUnitCombatTypeChange[i].eUnitCombat), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"RepelVSUnitCombatTypesChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"KnockbackVSUnitCombatTypesChanges"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"KnockbackVSUnitCombatTypesChange" );
		m_aKnockbackVSUnitCombatTypeChange.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"KnockbackVSUnitCombatTypesChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					pXML->GetChildXmlValByName(&(m_aKnockbackVSUnitCombatTypeChange[i].iModifier), L"iModifier");
					GC.addDelayedResolution((int*)&(m_aKnockbackVSUnitCombatTypeChange[i].eUnitCombat), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"KnockbackVSUnitCombatTypesChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"PunctureVSUnitCombatTypesChanges"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"PunctureVSUnitCombatTypesChange" );
		m_aPunctureVSUnitCombatTypeChange.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"PunctureVSUnitCombatTypesChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					pXML->GetChildXmlValByName(&(m_aPunctureVSUnitCombatTypeChange[i].iModifier), L"iModifier");
					GC.addDelayedResolution((int*)&(m_aPunctureVSUnitCombatTypeChange[i].eUnitCombat), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"PunctureVSUnitCombatTypesChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"ArmorVSUnitCombatTypesChanges"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"ArmorVSUnitCombatTypesChange" );
		m_aArmorVSUnitCombatTypeChange.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"ArmorVSUnitCombatTypesChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					pXML->GetChildXmlValByName(&(m_aArmorVSUnitCombatTypeChange[i].iModifier), L"iModifier");
					GC.addDelayedResolution((int*)&(m_aArmorVSUnitCombatTypeChange[i].eUnitCombat), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"ArmorVSUnitCombatTypesChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"DodgeVSUnitCombatTypesChanges"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"DodgeVSUnitCombatTypesChange" );
		m_aDodgeVSUnitCombatTypeChange.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"DodgeVSUnitCombatTypesChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					pXML->GetChildXmlValByName(&(m_aDodgeVSUnitCombatTypeChange[i].iModifier), L"iModifier");
					GC.addDelayedResolution((int*)&(m_aDodgeVSUnitCombatTypeChange[i].eUnitCombat), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"DodgeVSUnitCombatTypesChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"PrecisionVSUnitCombatTypesChanges"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"PrecisionVSUnitCombatTypesChange" );
		m_aPrecisionVSUnitCombatTypeChange.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"PrecisionVSUnitCombatTypesChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					pXML->GetChildXmlValByName(&(m_aPrecisionVSUnitCombatTypeChange[i].iModifier), L"iModifier");
					GC.addDelayedResolution((int*)&(m_aPrecisionVSUnitCombatTypeChange[i].eUnitCombat), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"PrecisionVSUnitCombatTypesChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"CriticalVSUnitCombatTypesChanges"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"CriticalVSUnitCombatTypesChange" );
		m_aCriticalVSUnitCombatTypeChange.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"CriticalVSUnitCombatTypesChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					pXML->GetChildXmlValByName(&(m_aCriticalVSUnitCombatTypeChange[i].iModifier), L"iModifier");
					GC.addDelayedResolution((int*)&(m_aCriticalVSUnitCombatTypeChange[i].eUnitCombat), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"CriticalVSUnitCombatTypesChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"RoundStunVSUnitCombatTypesChanges"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"RoundStunVSUnitCombatTypesChange" );
		m_aRoundStunVSUnitCombatTypeChange.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"RoundStunVSUnitCombatTypesChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					pXML->GetChildXmlValByName(&(m_aRoundStunVSUnitCombatTypeChange[i].iModifier), L"iModifier");
					GC.addDelayedResolution((int*)&(m_aRoundStunVSUnitCombatTypeChange[i].eUnitCombat), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"RoundStunVSUnitCombatTypesChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"TrapAvoidanceUnitCombatTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"TrapAvoidanceUnitCombatType" );
		m_aTrapAvoidanceUnitCombatTypes.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"TrapAvoidanceUnitCombatType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					pXML->GetChildXmlValByName(&(m_aTrapAvoidanceUnitCombatTypes[i].iModifier), L"iIntensity");
					GC.addDelayedResolution((int*)&(m_aTrapAvoidanceUnitCombatTypes[i].eUnitCombat), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"TrapAvoidanceUnitCombatType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"AfflictOnAttackChangeTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"AfflictOnAttackChangeType" );
		m_aAfflictOnAttackChangeTypes.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"AfflictOnAttackChangeType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"PromotionLineType");
					pXML->GetChildXmlValByName(&(m_aAfflictOnAttackChangeTypes[i].iProbabilityChange), L"iProbability");
					pXML->GetChildXmlValByName(&(m_aAfflictOnAttackChangeTypes[i].iMelee), L"iMelee");
					pXML->GetChildXmlValByName(&(m_aAfflictOnAttackChangeTypes[i].iDistance), L"iDistance");
					pXML->GetChildXmlValByName(&(m_aAfflictOnAttackChangeTypes[i].iImmediate), L"iImmediate");
					GC.addDelayedResolution((int*)&(m_aAfflictOnAttackChangeTypes[i].eAfflictionLine), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"AfflictOnAttackChangeType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"InvisibleTerrainChanges"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"InvisibleTerrainChange" );
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
		int iNum = pXML->GetXmlChildrenNumber(L"InvisibleFeatureChange" );
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
		int iNum = pXML->GetXmlChildrenNumber(L"InvisibleImprovementChange" );
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
		int iNum = pXML->GetXmlChildrenNumber(L"VisibleTerrainChange" );
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
		int iNum = pXML->GetXmlChildrenNumber(L"VisibleFeatureChange" );
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
		int iNum = pXML->GetXmlChildrenNumber(L"VisibleImprovementChange" );
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
		int iNum = pXML->GetXmlChildrenNumber(L"VisibleTerrainRangeChange" );
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
		int iNum = pXML->GetXmlChildrenNumber(L"VisibleFeatureRangeChange" );
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
		int iNum = pXML->GetXmlChildrenNumber(L"VisibleImprovementRangeChange" );
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
		int iNum = pXML->GetXmlChildrenNumber(L"DistanceAttackCommunicabilityTypeChange" );
		m_aDistanceAttackCommunicabilityTypeChanges.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"DistanceAttackCommunicabilityTypeChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"PromotionLineType");
					pXML->GetChildXmlValByName(&(m_aDistanceAttackCommunicabilityTypeChanges[i].iChange), L"iChange");
					GC.addDelayedResolution((int*)&(m_aDistanceAttackCommunicabilityTypeChanges[i].eAfflictionLine), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"DistanceAttackCommunicabilityTypeChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	m_PropertyManipulators.read(pXML);

	return true;
}

void CvUnitCombatInfo::copyNonDefaults(CvUnitCombatInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

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

	// Textual References
	if (getReligion() == NO_RELIGION) m_eReligion = pClassInfo->getReligion();
	if (getCulture() == NO_BONUS) m_eCulture = pClassInfo->getCulture();
	if (getEra() == NO_ERA) m_eEra = pClassInfo->getEra();
	//Integers
	if (getAssetMultiplier() == iDefault) m_iAssetMultiplier = pClassInfo->getAssetMultiplier();
	if (getPowerMultiplier() == iDefault) m_iPowerMultiplier = pClassInfo->getPowerMultiplier();
	if (getVisibilityChange() == iDefault) m_iVisibilityChange = pClassInfo->getVisibilityChange();
	if (getMovesChange() == iDefault) m_iMovesChange = pClassInfo->getMovesChange();
	if (getMoveDiscountChange() == iDefault) m_iMoveDiscountChange = pClassInfo->getMoveDiscountChange();
	if (getAirRangeChange() == iDefault) m_iAirRangeChange = pClassInfo->getAirRangeChange();
	if (getInterceptChange() == iDefault) m_iInterceptChange = pClassInfo->getInterceptChange();
	if (getEvasionChange() == iDefault) m_iEvasionChange = pClassInfo->getEvasionChange();
	if (getWithdrawalChange() == iDefault) m_iWithdrawalChange = pClassInfo->getWithdrawalChange();
	if (getCargoChange() == iDefault) m_iCargoChange = pClassInfo->getCargoChange();
	if (getSMCargoChange() == iDefault) m_iSMCargoChange = pClassInfo->getSMCargoChange();
	if (getSMCargoVolumeChange() == iDefault) m_iSMCargoVolumeChange = pClassInfo->getSMCargoVolumeChange();
	if (getSMCargoVolumeModifierChange() == iDefault) m_iSMCargoVolumeModifierChange = pClassInfo->getSMCargoVolumeModifierChange();
	if (getCollateralDamageChange() == iDefault) m_iCollateralDamageChange = pClassInfo->getCollateralDamageChange();
	if (getBombardRateChange() == iDefault) m_iBombardRateChange = pClassInfo->getBombardRateChange();
	if (getFirstStrikesChange() == iDefault) m_iFirstStrikesChange = pClassInfo->getFirstStrikesChange();
	if (getChanceFirstStrikesChange() == iDefault) m_iChanceFirstStrikesChange = pClassInfo->getChanceFirstStrikesChange();
	if (getEnemyHealChange() == iDefault) m_iEnemyHealChange = pClassInfo->getEnemyHealChange();
	if (getNeutralHealChange() == iDefault) m_iNeutralHealChange = pClassInfo->getNeutralHealChange();
	if (getFriendlyHealChange() == iDefault) m_iFriendlyHealChange = pClassInfo->getFriendlyHealChange();
	if (getSameTileHealChange() == iDefault) m_iSameTileHealChange = pClassInfo->getSameTileHealChange();
	if (getAdjacentTileHealChange() == iDefault) m_iAdjacentTileHealChange = pClassInfo->getAdjacentTileHealChange();
	if (getCombatPercent() == iDefault) m_iCombatPercent = pClassInfo->getCombatPercent();
	if (getCityAttackPercent() == iDefault) m_iCityAttackPercent = pClassInfo->getCityAttackPercent();
	if (getCityDefensePercent() == iDefault) m_iCityDefensePercent = pClassInfo->getCityDefensePercent();
	if (getHillsAttackPercent() == iDefault) m_iHillsAttackPercent = pClassInfo->getHillsAttackPercent();
	if (getHillsDefensePercent() == iDefault) m_iHillsDefensePercent = pClassInfo->getHillsDefensePercent();
	if (getHillsWorkPercent() == iDefault) m_iHillsWorkPercent = pClassInfo->getHillsWorkPercent();
	if (getWorkRatePercent() == iDefault) m_iWorkRatePercent = pClassInfo->getWorkRatePercent();
	if (getRevoltProtection() == iDefault) m_iRevoltProtection = pClassInfo->getRevoltProtection();
	if (getCollateralDamageProtection() == iDefault) m_iCollateralDamageProtection = pClassInfo->getCollateralDamageProtection();
	if (getPillageChange() == iDefault) m_iPillageChange = pClassInfo->getPillageChange();
	if (getUpgradeDiscount() == iDefault) m_iUpgradeDiscount = pClassInfo->getUpgradeDiscount();
	if (getExperiencePercent() == iDefault) m_iExperiencePercent = pClassInfo->getExperiencePercent();
	if (getKamikazePercent() == iDefault) m_iKamikazePercent = pClassInfo->getKamikazePercent();
	if (getAirCombatLimitChange() == iDefault) m_iAirCombatLimitChange = pClassInfo->getAirCombatLimitChange();
	if (getCelebrityHappy() == iDefault) m_iCelebrityHappy = pClassInfo->getCelebrityHappy();
	if (getCollateralDamageLimitChange() == iDefault) m_iCollateralDamageLimitChange = pClassInfo->getCollateralDamageLimitChange();
	if (getCollateralDamageMaxUnitsChange() == iDefault) m_iCollateralDamageMaxUnitsChange = pClassInfo->getCollateralDamageMaxUnitsChange();
	if (getCombatLimitChange() == iDefault) m_iCombatLimitChange = pClassInfo->getCombatLimitChange();
	if (getExtraDropRange() == iDefault) m_iExtraDropRange = pClassInfo->getExtraDropRange();
	if (getSurvivorChance() == iDefault) m_iSurvivorChance = pClassInfo->getSurvivorChance();
	if (getVictoryAdjacentHeal() == iDefault) m_iVictoryAdjacentHeal = pClassInfo->getVictoryAdjacentHeal();
	if (getVictoryHeal() == iDefault) m_iVictoryHeal = pClassInfo->getVictoryHeal();
	if (getVictoryStackHeal() == iDefault) m_iVictoryStackHeal = pClassInfo->getVictoryStackHeal();
	if (getAttackCombatModifierChange() == iDefault) m_iAttackCombatModifierChange = pClassInfo->getAttackCombatModifierChange();
	if (getDefenseCombatModifierChange() == iDefault) m_iDefenseCombatModifierChange = pClassInfo->getDefenseCombatModifierChange();
	if (getPursuitChange(true) == iDefault) m_iPursuitChange = pClassInfo->getPursuitChange(true);
	if (getEarlyWithdrawChange(true) == iDefault) m_iEarlyWithdrawChange = pClassInfo->getEarlyWithdrawChange(true);
	if (getVSBarbsChange() == iDefault) m_iVSBarbsChange = pClassInfo->getVSBarbsChange();
	if (getArmorChange() == iDefault) m_iArmorChange = pClassInfo->getArmorChange();
	if (getPunctureChange() == iDefault) m_iPunctureChange = pClassInfo->getPunctureChange();
	if (getOverrunChange(true) == iDefault) m_iOverrunChange = pClassInfo->getOverrunChange(true);
	if (getRepelChange(true) == iDefault) m_iRepelChange = pClassInfo->getRepelChange(true);
	if (getFortRepelChange(true) == iDefault) m_iFortRepelChange = pClassInfo->getFortRepelChange(true);
	if (getRepelRetriesChange(true) == iDefault) m_iRepelRetriesChange = pClassInfo->getRepelRetriesChange(true);
	if (getUnyieldingChange(true) == iDefault) m_iUnyieldingChange = pClassInfo->getUnyieldingChange(true);
	if (getKnockbackChange(true) == iDefault) m_iKnockbackChange = pClassInfo->getKnockbackChange(true);
	if (getKnockbackRetriesChange(true) == iDefault) m_iKnockbackRetriesChange = pClassInfo->getKnockbackRetriesChange(true);
	if (getStrAdjperAttChange(true) == iDefault) m_iStrAdjperAttChange = pClassInfo->getStrAdjperAttChange(true);
	if (getStrAdjperDefChange(true) == iDefault) m_iStrAdjperDefChange = pClassInfo->getStrAdjperDefChange(true);
	if (getWithdrawAdjperAttChange(true) == iDefault) m_iWithdrawAdjperAttChange = pClassInfo->getWithdrawAdjperAttChange(true);
	if (getUnnerveChange(true) == iDefault) m_iUnnerveChange = pClassInfo->getUnnerveChange(true);
	if (getEncloseChange(true) == iDefault) m_iEncloseChange = pClassInfo->getEncloseChange(true);
	if (getLungeChange(true) == iDefault) m_iLungeChange = pClassInfo->getLungeChange(true);
	if (getDynamicDefenseChange(true) == iDefault) m_iDynamicDefenseChange = pClassInfo->getDynamicDefenseChange(true);
	if (getStrengthChange() == iDefault) m_iStrengthChange = pClassInfo->getStrengthChange();
	if (getFortitudeChange() == iDefault) m_iFortitudeChange = pClassInfo->getFortitudeChange();
	if (getFrontSupportPercentChange(true) == iDefault) m_iFrontSupportPercentChange = pClassInfo->getFrontSupportPercentChange(true);
	if (getShortRangeSupportPercentChange(true) == iDefault) m_iShortRangeSupportPercentChange = pClassInfo->getShortRangeSupportPercentChange(true);
	if (getMediumRangeSupportPercentChange(true) == iDefault) m_iMediumRangeSupportPercentChange = pClassInfo->getMediumRangeSupportPercentChange(true);
	if (getLongRangeSupportPercentChange(true) == iDefault) m_iLongRangeSupportPercentChange = pClassInfo->getLongRangeSupportPercentChange(true);
	if (getFlankSupportPercentChange(true) == iDefault) m_iFlankSupportPercentChange = pClassInfo->getFlankSupportPercentChange(true);
	if (getDodgeModifierChange() == iDefault) m_iDodgeModifierChange = pClassInfo->getDodgeModifierChange();
	if (getPrecisionModifierChange() == iDefault) m_iPrecisionModifierChange = pClassInfo->getPrecisionModifierChange();
	if (getPowerShotsChange() == iDefault) m_iPowerShotsChange = pClassInfo->getPowerShotsChange();
	if (getPowerShotCombatModifierChange() == iDefault) m_iPowerShotCombatModifierChange = pClassInfo->getPowerShotCombatModifierChange();
	if (getPowerShotPunctureModifierChange() == iDefault) m_iPowerShotPunctureModifierChange = pClassInfo->getPowerShotPunctureModifierChange();
	if (getPowerShotPrecisionModifierChange() == iDefault) m_iPowerShotPrecisionModifierChange = pClassInfo->getPowerShotPrecisionModifierChange();
	if (getPowerShotCriticalModifierChange() == iDefault) m_iPowerShotCriticalModifierChange = pClassInfo->getPowerShotCriticalModifierChange();
	if (getCriticalModifierChange() == iDefault) m_iCriticalModifierChange = pClassInfo->getCriticalModifierChange();
	if (getEnduranceChange() == iDefault) m_iEnduranceChange = pClassInfo->getEnduranceChange();
	if (getRoundStunProbChange() == iDefault) m_iRoundStunProbChange = pClassInfo->getRoundStunProbChange();
	if (getPoisonProbabilityModifierChange() == iDefault) m_iPoisonProbabilityModifierChange = pClassInfo->getPoisonProbabilityModifierChange();
	if (getCaptureProbabilityModifierChange() == iDefault) m_iCaptureProbabilityModifierChange = pClassInfo->getCaptureProbabilityModifierChange();
	if (getCaptureResistanceModifierChange() == iDefault) m_iCaptureResistanceModifierChange = pClassInfo->getCaptureResistanceModifierChange();
	if (getHillsWorkModifierChange() == iDefault) m_iHillsWorkModifierChange = pClassInfo->getHillsWorkModifierChange();
	if (getPeaksWorkModifierChange() == iDefault) m_iPeaksWorkModifierChange = pClassInfo->getPeaksWorkModifierChange();
	if (getBreakdownChanceChange() == iDefault) m_iBreakdownChanceChange = pClassInfo->getBreakdownChanceChange();
	if (getBreakdownDamageChange() == iDefault) m_iBreakdownDamageChange = pClassInfo->getBreakdownDamageChange();
	if (getTauntChange() == iDefault) m_iTauntChange = pClassInfo->getTauntChange();
	if (getMaxHPChange(true) == iDefault) m_iMaxHPChange = pClassInfo->getMaxHPChange(true);
	if (getStrengthModifier() == iDefault) m_iStrengthModifier = pClassInfo->getStrengthModifier();
	if (getQualityBase() == -10) m_iQualityBase = pClassInfo->getQualityBase();
	if (getGroupBase() == -10) m_iGroupBase = pClassInfo->getGroupBase();
	if (getSizeBase() == -10) m_iSizeBase = pClassInfo->getSizeBase();
	if (getDamageModifierChange() == iDefault) m_iDamageModifierChange = pClassInfo->getDamageModifierChange();
	if (getCostModifierChange() == iDefault) m_iCostModifierChange = pClassInfo->getCostModifierChange();
	if (getRBombardDamageBase() == iDefault) m_iRBombardDamageBase = pClassInfo->getRBombardDamageBase();
	if (getRBombardDamageLimitBase() == iDefault) m_iRBombardDamageLimitBase = pClassInfo->getRBombardDamageLimitBase();
	if (getRBombardDamageMaxUnitsBase() == iDefault) m_iRBombardDamageMaxUnitsBase = pClassInfo->getRBombardDamageMaxUnitsBase();
	if (getDCMBombRangeBase() == iDefault) m_iDCMBombRangeBase = pClassInfo->getDCMBombRangeBase();
	if (getDCMBombAccuracyBase() == iDefault) m_iDCMBombAccuracyBase = pClassInfo->getDCMBombAccuracyBase();
	if (getCombatModifierPerSizeMoreChange(true) == iDefault) m_iCombatModifierPerSizeMoreChange = pClassInfo->getCombatModifierPerSizeMoreChange(true);
	if (getCombatModifierPerSizeLessChange(true) == iDefault) m_iCombatModifierPerSizeLessChange = pClassInfo->getCombatModifierPerSizeLessChange(true);
	if (getCombatModifierPerVolumeMoreChange(true) == iDefault) m_iCombatModifierPerVolumeMoreChange = pClassInfo->getCombatModifierPerVolumeMoreChange(true);
	if (getCombatModifierPerVolumeLessChange(true) == iDefault) m_iCombatModifierPerVolumeLessChange = pClassInfo->getCombatModifierPerVolumeLessChange(true);
	if (getSelfHealModifier() == iDefault) m_iSelfHealModifier = pClassInfo->getSelfHealModifier();
	if (getNumHealSupport() == iDefault) m_iNumHealSupport = pClassInfo->getNumHealSupport();
	if (getExcileChange() == iDefault) m_iExcileChange = pClassInfo->getExcileChange();
	if (getPassageChange() == iDefault) m_iPassageChange = pClassInfo->getPassageChange();
	if (getNoNonOwnedCityEntryChange() == iDefault) m_iNoNonOwnedCityEntryChange = pClassInfo->getNoNonOwnedCityEntryChange();
	if (getBarbCoExistChange() == iDefault) m_iBarbCoExistChange = pClassInfo->getBarbCoExistChange();
	if (getBlendIntoCityChange() == iDefault) m_iBlendIntoCityChange = pClassInfo->getBlendIntoCityChange();
	if (getInsidiousnessChange() == iDefault) m_iInsidiousnessChange = pClassInfo->getInsidiousnessChange();
	if (getInvestigationChange() == iDefault) m_iInvestigationChange = pClassInfo->getInvestigationChange();
	if (getStealthStrikesChange(true) == iDefault) m_iStealthStrikesChange = pClassInfo->getStealthStrikesChange(true);
	if (getStealthCombatModifierChange(true) == iDefault) m_iStealthCombatModifierChange = pClassInfo->getStealthCombatModifierChange(true);
	if (getStealthDefenseChange(true) == iDefault) m_iStealthDefenseChange = pClassInfo->getStealthDefenseChange(true);
	if (getDefenseOnlyChange() == iDefault) m_iDefenseOnlyChange = pClassInfo->getDefenseOnlyChange();
	if (getNoInvisibilityChange() == iDefault) m_iNoInvisibilityChange = pClassInfo->getNoInvisibilityChange();
	if (getNoCaptureChange() == iDefault) m_iNoCaptureChange = pClassInfo->getNoCaptureChange();
	if (getAnimalIgnoresBordersChange() == iDefault) m_iAnimalIgnoresBordersChange = pClassInfo->getAnimalIgnoresBordersChange();
	if (getNoDefensiveBonusChange() == iDefault) m_iNoDefensiveBonusChange = pClassInfo->getNoDefensiveBonusChange();
	if (getGatherHerdChange() == iDefault) m_iGatherHerdChange = pClassInfo->getGatherHerdChange();
	if (getReligiousCombatModifierChange() == iDefault) m_iReligiousCombatModifierChange = pClassInfo->getReligiousCombatModifierChange();
	//Booleans
	if (isDefensiveVictoryMove() == bDefault) m_bDefensiveVictoryMove = pClassInfo->isDefensiveVictoryMove();
	if (isFreeDrop() == bDefault) m_bFreeDrop = pClassInfo->isFreeDrop();
	if (isOffensiveVictoryMove() == bDefault) m_bOffensiveVictoryMove = pClassInfo->isOffensiveVictoryMove();
	if (isOneUp() == bDefault) m_bOneUp = pClassInfo->isOneUp();
	if (isPillageCulture() == bDefault) m_bPillageCulture = pClassInfo->isPillageCulture();
	if (isPillageEspionage() == bDefault) m_bPillageEspionage = pClassInfo->isPillageEspionage();
	if (isPillageMarauder() == bDefault) m_bPillageMarauder = pClassInfo->isPillageMarauder();
	if (isPillageOnMove() == bDefault) m_bPillageOnMove = pClassInfo->isPillageOnMove();
	if (isPillageOnVictory() == bDefault) m_bPillageOnVictory = pClassInfo->isPillageOnVictory();
	if (isPillageResearch() == bDefault) m_bPillageResearch = pClassInfo->isPillageResearch();
	if (isBlitz() == bDefault) m_bBlitz = pClassInfo->isBlitz();
	if (isAmphib() == bDefault) m_bAmphib = pClassInfo->isAmphib();
	if (isRiver() == bDefault) m_bRiver = pClassInfo->isRiver();
	if (isEnemyRoute() == bDefault) m_bEnemyRoute = pClassInfo->isEnemyRoute();
	if (isAlwaysHeal() == bDefault) m_bAlwaysHeal = pClassInfo->isAlwaysHeal();
	if (isHillsDoubleMove() == bDefault) m_bHillsDoubleMove = pClassInfo->isHillsDoubleMove();
	if (isImmuneToFirstStrikes() == bDefault) m_bImmuneToFirstStrikes = pClassInfo->isImmuneToFirstStrikes();
	if (isStampedeChange() == bDefault) m_bStampedeChange = pClassInfo->isStampedeChange();
	if (isRemoveStampede() == bDefault) m_bRemoveStampede = pClassInfo->isRemoveStampede();
	if (isOnslaughtChange() == bDefault) m_bOnslaughtChange = pClassInfo->isOnslaughtChange();
	if (isMakesDamageCold() == bDefault) m_bMakesDamageCold = pClassInfo->isMakesDamageCold();
	if (isMakesDamageNotCold() == bDefault) m_bMakesDamageNotCold = pClassInfo->isMakesDamageNotCold();
	if (isAddsColdImmunity() == bDefault) m_bAddsColdImmunity = pClassInfo->isAddsColdImmunity();
	if (isRemovesColdImmunity() == bDefault) m_bRemovesColdImmunity = pClassInfo->isRemovesColdImmunity();
	if (isAttackOnlyCitiesAdd() == bDefault) m_bAttackOnlyCitiesAdd = pClassInfo->isAttackOnlyCitiesAdd();
	if (isAttackOnlyCitiesSubtract() == bDefault) m_bAttackOnlyCitiesSubtract = pClassInfo->isAttackOnlyCitiesSubtract();
	if (isIgnoreNoEntryLevelAdd() == bDefault) m_bIgnoreNoEntryLevelAdd = pClassInfo->isIgnoreNoEntryLevelAdd();
	if (isIgnoreNoEntryLevelSubtract() == bDefault) m_bIgnoreNoEntryLevelSubtract = pClassInfo->isIgnoreNoEntryLevelSubtract();
	if (isIgnoreZoneofControlAdd() == bDefault) m_bIgnoreZoneofControlAdd = pClassInfo->isIgnoreZoneofControlAdd();
	if (isIgnoreZoneofControlSubtract() == bDefault) m_bIgnoreZoneofControlSubtract = pClassInfo->isIgnoreZoneofControlSubtract();
	if (isFliesToMoveAdd() == bDefault) m_bFliesToMoveAdd = pClassInfo->isFliesToMoveAdd();
	if (isFliesToMoveSubtract() == bDefault) m_bFliesToMoveSubtract = pClassInfo->isFliesToMoveSubtract();
	if (isCanMovePeaks() == bDefault) m_bCanMovePeaks = pClassInfo->isCanMovePeaks();
	if (isCanLeadThroughPeaks() == bDefault) m_bCanLeadThroughPeaks = pClassInfo->isCanLeadThroughPeaks();
	if (isZoneOfControl() == bDefault) m_bZoneOfControl = pClassInfo->isZoneOfControl();
	if (isSpy() == bDefault) m_bSpy = pClassInfo->isSpy();
	if (isCannotMergeSplit() == bDefault) m_bCannotMergeSplit = pClassInfo->isCannotMergeSplit();
	if (isRBombardDirect() == bDefault) m_bRBombardDirect = pClassInfo->isRBombardDirect();
	if (isRBombardForceAbility() == bDefault) m_bRBombardForceAbility = pClassInfo->isRBombardForceAbility();
	if (isAlwaysInvisible() == bDefault) m_bInvisible = pClassInfo->isAlwaysInvisible();
	if (isForMilitary() == bDefault) m_bForMilitary = pClassInfo->isForMilitary();
	if (isForNavalMilitary() == bDefault) m_bForNavalMilitary = pClassInfo->isForNavalMilitary();
	if (isHealsAs() == bDefault) m_bHealsAs = pClassInfo->isHealsAs();
	if (isNoSelfHeal() == bDefault) m_bNoSelfHeal = pClassInfo->isNoSelfHeal();

	//Arrays
	for (int j = 0; j < NUM_DOMAIN_TYPES; j++)
	{
		if ((m_piDomainModifierPercent == NULL || m_piDomainModifierPercent[j] == iDefault) &&
			pClassInfo->getDomainModifierPercent(j) != iDefault)
		{
			if ( m_piDomainModifierPercent == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piDomainModifierPercent,NUM_DOMAIN_TYPES,iDefault);
			}
			m_piDomainModifierPercent[j] = pClassInfo->getDomainModifierPercent(j);
		}
	}
	if (m_piDomainModifierPercent != NULL)
	{
		m_bAnyDomainModifierPercent = true;
	}

	// bool vector with delayed resolution
	if (getNumCureAfflictionChangeTypes() == 0)
	{
		int iNum = pClassInfo->getNumCureAfflictionChangeTypes();
		m_aiCureAfflictionChangeTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			GC.copyNonDefaultDelayedResolution((int*)&(m_aiCureAfflictionChangeTypes[i]), (int*)&(pClassInfo->m_aiCureAfflictionChangeTypes[i]));
		}
	}

	if (getNumTerrainIgnoreDamageChangeTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiTerrainIgnoreDamageChangeTypes, pClassInfo->m_aiTerrainIgnoreDamageChangeTypes);
	}

	if (getNumTerrainDoubleMoveChangeTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiTerrainDoubleMoveChangeTypes, pClassInfo->m_aiTerrainDoubleMoveChangeTypes);
	}

	if (getNumFeatureDoubleMoveChangeTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiFeatureDoubleMoveChangeTypes, pClassInfo->m_aiFeatureDoubleMoveChangeTypes);
	}

	if (getNumOnGameOptions() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiOnGameOptions, pClassInfo->m_aiOnGameOptions);
	}

	if (getNumNotOnGameOptions() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiNotOnGameOptions, pClassInfo->m_aiNotOnGameOptions);
	}

	if (getNumGGptsforUnitTypes() == 0)
	{
		int iNum = pClassInfo->getNumGGptsforUnitTypes();
		m_aiGGptsforUnitTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			GC.copyNonDefaultDelayedResolution((int*)&(m_aiGGptsforUnitTypes[i]), (int*)&(pClassInfo->m_aiGGptsforUnitTypes[i]));
		}
	}

	if (getNumDefaultStatusTypes() == 0)
	{
		int iNum = pClassInfo->getNumDefaultStatusTypes();
		m_aiDefaultStatusTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			GC.copyNonDefaultDelayedResolution((int*)&(m_aiDefaultStatusTypes[i]), (int*)&(pClassInfo->m_aiDefaultStatusTypes[i]));
		}
	}

	if (getNumTrapImmunityUnitCombatTypes() == 0)
	{
		int iNum = pClassInfo->getNumTrapImmunityUnitCombatTypes();
		m_aiTrapImmunityUnitCombatTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			GC.copyNonDefaultDelayedResolution((int*)&(m_aiTrapImmunityUnitCombatTypes[i]), (int*)&(pClassInfo->m_aiTrapImmunityUnitCombatTypes[i]));
		}
	}

	// int vectors utilizing pairing without delayed resolution

	if (getNumWithdrawOnTerrainTypeChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumWithdrawOnTerrainTypeChanges(); i++)
		{
			TerrainTypes eTerrain = ((TerrainTypes)i);
			int iChange = pClassInfo->getWithdrawOnTerrainTypeChange(i);
			m_aWithdrawOnTerrainTypesChange.push_back(std::make_pair(eTerrain, iChange));
		}
	}

	if (getNumWithdrawOnFeatureTypeChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumWithdrawOnFeatureTypeChanges(); i++)
		{
			FeatureTypes eFeature = ((FeatureTypes)i);
			int iChange = pClassInfo->getWithdrawOnFeatureTypeChange(i);
			m_aWithdrawOnFeatureTypesChange.push_back(std::make_pair(eFeature, iChange));
		}
	}

	if (getNumVisibilityIntensityChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumVisibilityIntensityChangeTypes(); i++)
		{
			InvisibleTypes eInvisible = ((InvisibleTypes)i);
			int iChange = pClassInfo->getVisibilityIntensityChangeType(i);
			m_aVisibilityIntensityChangeTypes.push_back(std::make_pair(eInvisible, iChange));
		}
	}

	if (getNumInvisibilityIntensityChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumInvisibilityIntensityChangeTypes(); i++)
		{
			InvisibleTypes eInvisible = ((InvisibleTypes)i);
			int iChange = pClassInfo->getInvisibilityIntensityChangeType(i);
			m_aInvisibilityIntensityChangeTypes.push_back(std::make_pair(eInvisible, iChange));
		}
	}

	if (getNumVisibilityIntensityRangeChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumVisibilityIntensityRangeChangeTypes(); i++)
		{
			InvisibleTypes eInvisible = ((InvisibleTypes)i);
			int iChange = pClassInfo->getVisibilityIntensityRangeChangeType(i);
			m_aVisibilityIntensityRangeChangeTypes.push_back(std::make_pair(eInvisible, iChange));
		}
	}

	if (getNumVisibilityIntensitySameTileChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumVisibilityIntensitySameTileChangeTypes(); i++)
		{
			InvisibleTypes eInvisible = ((InvisibleTypes)i);
			int iChange = pClassInfo->getVisibilityIntensitySameTileChangeType(i);
			m_aVisibilityIntensitySameTileChangeTypes.push_back(std::make_pair(eInvisible, iChange));
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

	// int vectors utilizing struct with delayed resolution
	if (getNumAfflictionFortitudeChangeModifiers() == 0)
	{
		int iNum = pClassInfo->getNumAfflictionFortitudeChangeModifiers();
		m_aAfflictionFortitudeChangeModifiers.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aAfflictionFortitudeChangeModifiers[i].iModifier = pClassInfo->m_aAfflictionFortitudeChangeModifiers[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aAfflictionFortitudeChangeModifiers[i].ePromotionLine), (int*)&(pClassInfo->m_aAfflictionFortitudeChangeModifiers[i].ePromotionLine));
		}
	}

	if (getNumTerrainAttackChangeModifiers() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aTerrainAttackChangeModifiers, pClassInfo->m_aTerrainAttackChangeModifiers);
	}

	if (getNumTerrainDefenseChangeModifiers() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aTerrainDefenseChangeModifiers, pClassInfo->m_aTerrainDefenseChangeModifiers);
	}

	if (getNumTerrainWorkChangeModifiers() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aTerrainWorkChangeModifiers, pClassInfo->m_aTerrainWorkChangeModifiers);
	}

	if (getNumBuildWorkChangeModifiers() == 0)
	{
		int iNum = pClassInfo->getNumBuildWorkChangeModifiers();
		m_aBuildWorkChangeModifiers.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aBuildWorkChangeModifiers[i].iModifier = pClassInfo->m_aBuildWorkChangeModifiers[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aBuildWorkChangeModifiers[i].eBuild), (int*)&(pClassInfo->m_aBuildWorkChangeModifiers[i].eBuild));
		}
	}

	if (getNumFeatureAttackChangeModifiers() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aFeatureAttackChangeModifiers, pClassInfo->m_aFeatureAttackChangeModifiers);
	}

	if (getNumFeatureDefenseChangeModifiers() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aFeatureDefenseChangeModifiers, pClassInfo->m_aFeatureDefenseChangeModifiers);
	}

	if (getNumFeatureWorkChangeModifiers() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aFeatureWorkChangeModifiers, pClassInfo->m_aFeatureWorkChangeModifiers);
	}

	if (getNumUnitCombatChangeModifiers() == 0)
	{
		int iNum = pClassInfo->getNumUnitCombatChangeModifiers();
		m_aUnitCombatChangeModifiers.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aUnitCombatChangeModifiers[i].iModifier = pClassInfo->m_aUnitCombatChangeModifiers[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aUnitCombatChangeModifiers[i].eUnitCombat), (int*)&(pClassInfo->m_aUnitCombatChangeModifiers[i].eUnitCombat));
		}
	}

	if (getNumFlankingStrengthbyUnitCombatTypesChange() == 0)
	{
		int iNum = pClassInfo->getNumFlankingStrengthbyUnitCombatTypesChange();
		m_aFlankingStrengthbyUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aFlankingStrengthbyUnitCombatTypeChange[i].iModifier = pClassInfo->m_aFlankingStrengthbyUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aFlankingStrengthbyUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aFlankingStrengthbyUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumWithdrawVSUnitCombatTypesChange() == 0)
	{
		int iNum = pClassInfo->getNumWithdrawVSUnitCombatTypesChange();
		m_aWithdrawVSUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aWithdrawVSUnitCombatTypeChange[i].iModifier = pClassInfo->m_aWithdrawVSUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aWithdrawVSUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aWithdrawVSUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumPursuitVSUnitCombatTypesChange() == 0)
	{
		int iNum = pClassInfo->getNumPursuitVSUnitCombatTypesChange();
		m_aPursuitVSUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aPursuitVSUnitCombatTypeChange[i].iModifier = pClassInfo->m_aPursuitVSUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aPursuitVSUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aPursuitVSUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumRepelVSUnitCombatTypesChange() == 0)
	{
		int iNum = pClassInfo->getNumRepelVSUnitCombatTypesChange();
		m_aRepelVSUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aRepelVSUnitCombatTypeChange[i].iModifier = pClassInfo->m_aRepelVSUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aRepelVSUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aRepelVSUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumKnockbackVSUnitCombatTypesChange() == 0)
	{
		int iNum = pClassInfo->getNumKnockbackVSUnitCombatTypesChange();
		m_aKnockbackVSUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aKnockbackVSUnitCombatTypeChange[i].iModifier = pClassInfo->m_aKnockbackVSUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aKnockbackVSUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aKnockbackVSUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumPunctureVSUnitCombatTypesChange() == 0)
	{
		int iNum = pClassInfo->getNumPunctureVSUnitCombatTypesChange();
		m_aPunctureVSUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aPunctureVSUnitCombatTypeChange[i].iModifier = pClassInfo->m_aPunctureVSUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aPunctureVSUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aPunctureVSUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumArmorVSUnitCombatTypesChange() == 0)
	{
		int iNum = pClassInfo->getNumArmorVSUnitCombatTypesChange();
		m_aArmorVSUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aArmorVSUnitCombatTypeChange[i].iModifier = pClassInfo->m_aArmorVSUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aArmorVSUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aArmorVSUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumDodgeVSUnitCombatTypesChange() == 0)
	{
		int iNum = pClassInfo->getNumDodgeVSUnitCombatTypesChange();
		m_aDodgeVSUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aDodgeVSUnitCombatTypeChange[i].iModifier = pClassInfo->m_aDodgeVSUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aDodgeVSUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aDodgeVSUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumPrecisionVSUnitCombatTypesChange() == 0)
	{
		int iNum = pClassInfo->getNumPrecisionVSUnitCombatTypesChange();
		m_aPrecisionVSUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aPrecisionVSUnitCombatTypeChange[i].iModifier = pClassInfo->m_aPrecisionVSUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aPrecisionVSUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aPrecisionVSUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumCriticalVSUnitCombatTypesChange() == 0)
	{
		int iNum = pClassInfo->getNumCriticalVSUnitCombatTypesChange();
		m_aCriticalVSUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aCriticalVSUnitCombatTypeChange[i].iModifier = pClassInfo->m_aCriticalVSUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aCriticalVSUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aCriticalVSUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumRoundStunVSUnitCombatTypesChange() == 0)
	{
		int iNum = pClassInfo->getNumRoundStunVSUnitCombatTypesChange();
		m_aRoundStunVSUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aRoundStunVSUnitCombatTypeChange[i].iModifier = pClassInfo->m_aRoundStunVSUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aRoundStunVSUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aRoundStunVSUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumTrapAvoidanceUnitCombatTypes() == 0)
	{
		int iNum = pClassInfo->getNumTrapAvoidanceUnitCombatTypes();
		m_aTrapAvoidanceUnitCombatTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aTrapAvoidanceUnitCombatTypes[i].iModifier = pClassInfo->m_aTrapAvoidanceUnitCombatTypes[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aTrapAvoidanceUnitCombatTypes[i].eUnitCombat), (int*)&(pClassInfo->m_aTrapAvoidanceUnitCombatTypes[i].eUnitCombat));
		}
	}

	if (getNumAfflictOnAttackChangeTypes() == 0)
	{
		int iNum = pClassInfo->getNumAfflictOnAttackChangeTypes();
		m_aAfflictOnAttackChangeTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aAfflictOnAttackChangeTypes[i].iProbabilityChange = pClassInfo->m_aAfflictOnAttackChangeTypes[i].iProbabilityChange;
			m_aAfflictOnAttackChangeTypes[i].iMelee = pClassInfo->m_aAfflictOnAttackChangeTypes[i].iMelee;
			m_aAfflictOnAttackChangeTypes[i].iDistance = pClassInfo->m_aAfflictOnAttackChangeTypes[i].iDistance;
			m_aAfflictOnAttackChangeTypes[i].iImmediate = pClassInfo->m_aAfflictOnAttackChangeTypes[i].iImmediate;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aAfflictOnAttackChangeTypes[i].eAfflictionLine), (int*)&(pClassInfo->m_aAfflictOnAttackChangeTypes[i].eAfflictionLine));
		}
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
		int iNum = pClassInfo->getNumDistanceAttackCommunicabilityTypeChanges();
		m_aDistanceAttackCommunicabilityTypeChanges.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aDistanceAttackCommunicabilityTypeChanges[i].iChange = pClassInfo->m_aDistanceAttackCommunicabilityTypeChanges[i].iChange;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aDistanceAttackCommunicabilityTypeChanges[i].eAfflictionLine), (int*)&(pClassInfo->m_aDistanceAttackCommunicabilityTypeChanges[i].eAfflictionLine));
		}
	}

	m_PropertyManipulators.copyNonDefaults(pClassInfo->getPropertyManipulators(), pXML);
}

void CvUnitCombatInfo::getCheckSum(unsigned int& iSum)
{
	getKillOutcomeList()->getCheckSum(iSum);

	for (int i=0; i<(int)m_aOutcomeMissions.size(); i++)
	{
		m_aOutcomeMissions[i]->getCheckSum(iSum);
	}

	// Textual References
	CheckSum(iSum, m_eReligion);
	CheckSum(iSum, m_eCulture);
	CheckSum(iSum, m_eEra);
	//Integers
	CheckSum(iSum, m_iAssetMultiplier);
	CheckSum(iSum, m_iPowerMultiplier);
	CheckSum(iSum, m_iVisibilityChange);
	CheckSum(iSum, m_iMovesChange);
	CheckSum(iSum, m_iMoveDiscountChange);
	CheckSum(iSum, m_iAirRangeChange);
	CheckSum(iSum, m_iInterceptChange);
	CheckSum(iSum, m_iEvasionChange);
	CheckSum(iSum, m_iWithdrawalChange);
	CheckSum(iSum, m_iCargoChange);
	CheckSum(iSum, m_iSMCargoChange);
	CheckSum(iSum, m_iSMCargoVolumeChange);
	CheckSum(iSum, m_iSMCargoVolumeModifierChange);
	CheckSum(iSum, m_iCollateralDamageChange);
	CheckSum(iSum, m_iBombardRateChange);
	CheckSum(iSum, m_iFirstStrikesChange);
	CheckSum(iSum, m_iChanceFirstStrikesChange);
	CheckSum(iSum, m_iEnemyHealChange);
	CheckSum(iSum, m_iNeutralHealChange);
	CheckSum(iSum, m_iFriendlyHealChange);
	CheckSum(iSum, m_iSameTileHealChange);
	CheckSum(iSum, m_iAdjacentTileHealChange);
	CheckSum(iSum, m_iCombatPercent);
	CheckSum(iSum, m_iCityAttackPercent);
	CheckSum(iSum, m_iCityDefensePercent);
	CheckSum(iSum, m_iHillsAttackPercent);
	CheckSum(iSum, m_iHillsDefensePercent);
	CheckSum(iSum, m_iHillsWorkPercent);
	CheckSum(iSum, m_iWorkRatePercent);
	CheckSum(iSum, m_iRevoltProtection);
	CheckSum(iSum, m_iCollateralDamageProtection);
	CheckSum(iSum, m_iPillageChange);
	CheckSum(iSum, m_iUpgradeDiscount);
	CheckSum(iSum, m_iExperiencePercent);
	CheckSum(iSum, m_iKamikazePercent);
	CheckSum(iSum, m_iAirCombatLimitChange);
	CheckSum(iSum, m_iCelebrityHappy);
	CheckSum(iSum, m_iCollateralDamageLimitChange);
	CheckSum(iSum, m_iCollateralDamageMaxUnitsChange);
	CheckSum(iSum, m_iCombatLimitChange);
	CheckSum(iSum, m_iExtraDropRange);
	CheckSum(iSum, m_iSurvivorChance);
	CheckSum(iSum, m_iVictoryAdjacentHeal);
	CheckSum(iSum, m_iVictoryHeal);
	CheckSum(iSum, m_iVictoryStackHeal);
	CheckSum(iSum, m_iAttackCombatModifierChange);
	CheckSum(iSum, m_iDefenseCombatModifierChange);
	CheckSum(iSum, m_iPursuitChange);
	CheckSum(iSum, m_iEarlyWithdrawChange);
	CheckSum(iSum, m_iVSBarbsChange);
	CheckSum(iSum, m_iArmorChange);
	CheckSum(iSum, m_iPunctureChange);
	CheckSum(iSum, m_iOverrunChange);
	CheckSum(iSum, m_iRepelChange);
	CheckSum(iSum, m_iFortRepelChange);
	CheckSum(iSum, m_iRepelRetriesChange);
	CheckSum(iSum, m_iUnyieldingChange);
	CheckSum(iSum, m_iKnockbackChange);
	CheckSum(iSum, m_iKnockbackRetriesChange);
	CheckSum(iSum, m_iStrAdjperAttChange);
	CheckSum(iSum, m_iStrAdjperDefChange);
	CheckSum(iSum, m_iWithdrawAdjperAttChange);
	CheckSum(iSum, m_iUnnerveChange);
	CheckSum(iSum, m_iEncloseChange);
	CheckSum(iSum, m_iLungeChange);
	CheckSum(iSum, m_iDynamicDefenseChange);
	CheckSum(iSum, m_iStrengthChange);
	CheckSum(iSum, m_iFortitudeChange);
	CheckSum(iSum, m_iFrontSupportPercentChange);
	CheckSum(iSum, m_iShortRangeSupportPercentChange);
	CheckSum(iSum, m_iMediumRangeSupportPercentChange);
	CheckSum(iSum, m_iLongRangeSupportPercentChange);
	CheckSum(iSum, m_iFlankSupportPercentChange);
	CheckSum(iSum, m_iDodgeModifierChange);
	CheckSum(iSum, m_iPrecisionModifierChange);
	CheckSum(iSum, m_iPowerShotsChange);
	CheckSum(iSum, m_iPowerShotCombatModifierChange);
	CheckSum(iSum, m_iPowerShotPunctureModifierChange);
	CheckSum(iSum, m_iPowerShotPrecisionModifierChange);
	CheckSum(iSum, m_iPowerShotCriticalModifierChange);
	CheckSum(iSum, m_iCriticalModifierChange);
	CheckSum(iSum, m_iEnduranceChange);
	CheckSum(iSum, m_iRoundStunProbChange);
	CheckSum(iSum, m_iPoisonProbabilityModifierChange);
	CheckSum(iSum, m_iCaptureProbabilityModifierChange);
	CheckSum(iSum, m_iCaptureResistanceModifierChange);
	CheckSum(iSum, m_iHillsWorkModifierChange);
	CheckSum(iSum, m_iPeaksWorkModifierChange);
	CheckSum(iSum, m_iBreakdownChanceChange);
	CheckSum(iSum, m_iBreakdownDamageChange);
	CheckSum(iSum, m_iTauntChange);
	CheckSum(iSum, m_iMaxHPChange);
	CheckSum(iSum, m_iStrengthModifier);
	CheckSum(iSum, m_iQualityBase);
	CheckSum(iSum, m_iGroupBase);
	CheckSum(iSum, m_iSizeBase);
	CheckSum(iSum, m_iDamageModifierChange);
	CheckSum(iSum, m_iCostModifierChange);
	CheckSum(iSum, m_iRBombardDamageBase);
	CheckSum(iSum, m_iRBombardDamageLimitBase);
	CheckSum(iSum, m_iRBombardDamageMaxUnitsBase);
	CheckSum(iSum, m_iDCMBombRangeBase);
	CheckSum(iSum, m_iDCMBombAccuracyBase);
	CheckSum(iSum, m_iCombatModifierPerSizeMoreChange);
	CheckSum(iSum, m_iCombatModifierPerSizeLessChange);
	CheckSum(iSum, m_iCombatModifierPerVolumeMoreChange);
	CheckSum(iSum, m_iCombatModifierPerVolumeLessChange);
	CheckSum(iSum, m_iSelfHealModifier);
	CheckSum(iSum, m_iNumHealSupport);
	CheckSum(iSum, m_iExcileChange);
	CheckSum(iSum, m_iPassageChange);
	CheckSum(iSum, m_iNoNonOwnedCityEntryChange);
	CheckSum(iSum, m_iBarbCoExistChange);
	CheckSum(iSum, m_iBlendIntoCityChange);
	CheckSum(iSum, m_iInsidiousnessChange);
	CheckSum(iSum, m_iInvestigationChange);
	CheckSum(iSum, m_iStealthStrikesChange);
	CheckSum(iSum, m_iStealthCombatModifierChange);
	CheckSum(iSum, m_iStealthDefenseChange);
	CheckSum(iSum, m_iDefenseOnlyChange);
	CheckSum(iSum, m_iNoInvisibilityChange);
	CheckSum(iSum, m_iNoCaptureChange);
	CheckSum(iSum, m_iAnimalIgnoresBordersChange);
	CheckSum(iSum, m_iNoDefensiveBonusChange);
	CheckSum(iSum, m_iGatherHerdChange);
	CheckSum(iSum, m_iReligiousCombatModifierChange);
	//Boolean
	CheckSum(iSum, m_bDefensiveVictoryMove);
	CheckSum(iSum, m_bFreeDrop);
	CheckSum(iSum, m_bOffensiveVictoryMove);
	CheckSum(iSum, m_bOneUp);
	CheckSum(iSum, m_bPillageCulture);
	CheckSum(iSum, m_bPillageEspionage);
	CheckSum(iSum, m_bPillageMarauder);
	CheckSum(iSum, m_bPillageOnMove);
	CheckSum(iSum, m_bPillageOnVictory);
	CheckSum(iSum, m_bPillageResearch);
	CheckSum(iSum, m_bBlitz);
	CheckSum(iSum, m_bAmphib);
	CheckSum(iSum, m_bRiver);
	CheckSum(iSum, m_bEnemyRoute);
	CheckSum(iSum, m_bAlwaysHeal);
	CheckSum(iSum, m_bHillsDoubleMove);
	CheckSum(iSum, m_bImmuneToFirstStrikes);
	CheckSum(iSum, m_bStampedeChange);
	CheckSum(iSum, m_bRemoveStampede);
	CheckSum(iSum, m_bOnslaughtChange);
	CheckSum(iSum, m_bMakesDamageCold);
	CheckSum(iSum, m_bMakesDamageNotCold);
	CheckSum(iSum, m_bAddsColdImmunity);
	CheckSum(iSum, m_bRemovesColdImmunity);
	CheckSum(iSum, m_bAttackOnlyCitiesAdd);
	CheckSum(iSum, m_bAttackOnlyCitiesSubtract);
	CheckSum(iSum, m_bIgnoreNoEntryLevelAdd);
	CheckSum(iSum, m_bIgnoreNoEntryLevelSubtract);
	CheckSum(iSum, m_bIgnoreZoneofControlAdd);
	CheckSum(iSum, m_bIgnoreZoneofControlSubtract);
	CheckSum(iSum, m_bFliesToMoveAdd);
	CheckSum(iSum, m_bFliesToMoveSubtract);

	CheckSum(iSum, m_bCanMovePeaks);
	CheckSum(iSum, m_bCanLeadThroughPeaks);
	CheckSum(iSum, m_bSpy);
	CheckSum(iSum, m_bCannotMergeSplit);
	CheckSum(iSum, m_bRBombardDirect);
	CheckSum(iSum, m_bRBombardForceAbility);
	CheckSum(iSum, m_bInvisible);
	CheckSum(iSum, m_bForMilitary);
	CheckSum(iSum, m_bForNavalMilitary);
	CheckSum(iSum, m_bHealsAs);
	CheckSum(iSum, m_bNoSelfHeal);
	//Array Derived
	CheckSum(iSum, m_bAnyDomainModifierPercent);

	// Arrays
	CheckSum(iSum, m_piDomainModifierPercent, NUM_DOMAIN_TYPES);

	// int vectors utilizing pairing without delayed resolution
	CheckSumC(iSum, m_aWithdrawOnTerrainTypesChange);
	CheckSumC(iSum, m_aWithdrawOnFeatureTypesChange);
	CheckSumC(iSum, m_aVisibilityIntensityChangeTypes);
	CheckSumC(iSum, m_aInvisibilityIntensityChangeTypes);
	CheckSumC(iSum, m_aVisibilityIntensityRangeChangeTypes);
	CheckSumC(iSum, m_aVisibilityIntensitySameTileChangeTypes);
	CheckSumC(iSum, m_aAidChanges);

	//bool vector with delayed resolution
	CheckSumC(iSum, m_aiCureAfflictionChangeTypes);
	CheckSumC(iSum, m_aiTerrainIgnoreDamageChangeTypes);
	CheckSumC(iSum, m_aiTerrainDoubleMoveChangeTypes);
	CheckSumC(iSum, m_aiFeatureDoubleMoveChangeTypes);
	CheckSumC(iSum, m_aiOnGameOptions);
	CheckSumC(iSum, m_aiNotOnGameOptions);
	CheckSumC(iSum, m_aiGGptsforUnitTypes);
	CheckSumC(iSum, m_aiDefaultStatusTypes);
	CheckSumC(iSum, m_aiTrapImmunityUnitCombatTypes);

	//int vectors utilizing struct with delayed resolution
	int iNumElements;
	iNumElements = m_aAfflictionFortitudeChangeModifiers.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aAfflictionFortitudeChangeModifiers[i].ePromotionLine);
		CheckSum(iSum, m_aAfflictionFortitudeChangeModifiers[i].iModifier);
	}

	iNumElements = m_aTerrainAttackChangeModifiers.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aTerrainAttackChangeModifiers[i].eTerrain);
		CheckSum(iSum, m_aTerrainAttackChangeModifiers[i].iModifier);
	}

	iNumElements = m_aTerrainDefenseChangeModifiers.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aTerrainDefenseChangeModifiers[i].eTerrain);
		CheckSum(iSum, m_aTerrainDefenseChangeModifiers[i].iModifier);
	}

	iNumElements = m_aTerrainWorkChangeModifiers.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aTerrainWorkChangeModifiers[i].eTerrain);
		CheckSum(iSum, m_aTerrainWorkChangeModifiers[i].iModifier);
	}

	iNumElements = m_aBuildWorkChangeModifiers.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aBuildWorkChangeModifiers[i].eBuild);
		CheckSum(iSum, m_aBuildWorkChangeModifiers[i].iModifier);
	}

	iNumElements = m_aFeatureAttackChangeModifiers.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aFeatureAttackChangeModifiers[i].eFeature);
		CheckSum(iSum, m_aFeatureAttackChangeModifiers[i].iModifier);
	}

	iNumElements = m_aFeatureDefenseChangeModifiers.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aFeatureDefenseChangeModifiers[i].eFeature);
		CheckSum(iSum, m_aFeatureDefenseChangeModifiers[i].iModifier);
	}

	iNumElements = m_aFeatureWorkChangeModifiers.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aFeatureWorkChangeModifiers[i].eFeature);
		CheckSum(iSum, m_aFeatureWorkChangeModifiers[i].iModifier);
	}

	iNumElements = m_aUnitCombatChangeModifiers.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aUnitCombatChangeModifiers[i].eUnitCombat);
		CheckSum(iSum, m_aUnitCombatChangeModifiers[i].iModifier);
	}

	iNumElements = m_aFlankingStrengthbyUnitCombatTypeChange.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aFlankingStrengthbyUnitCombatTypeChange[i].eUnitCombat);
		CheckSum(iSum, m_aFlankingStrengthbyUnitCombatTypeChange[i].iModifier);
	}

	iNumElements = m_aWithdrawVSUnitCombatTypeChange.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aWithdrawVSUnitCombatTypeChange[i].eUnitCombat);
		CheckSum(iSum, m_aWithdrawVSUnitCombatTypeChange[i].iModifier);
	}

	iNumElements = m_aPursuitVSUnitCombatTypeChange.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aPursuitVSUnitCombatTypeChange[i].eUnitCombat);
		CheckSum(iSum, m_aPursuitVSUnitCombatTypeChange[i].iModifier);
	}

	iNumElements = m_aRepelVSUnitCombatTypeChange.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aRepelVSUnitCombatTypeChange[i].eUnitCombat);
		CheckSum(iSum, m_aRepelVSUnitCombatTypeChange[i].iModifier);
	}

	iNumElements = m_aKnockbackVSUnitCombatTypeChange.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aKnockbackVSUnitCombatTypeChange[i].eUnitCombat);
		CheckSum(iSum, m_aKnockbackVSUnitCombatTypeChange[i].iModifier);
	}

	iNumElements = m_aPunctureVSUnitCombatTypeChange.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aPunctureVSUnitCombatTypeChange[i].eUnitCombat);
		CheckSum(iSum, m_aPunctureVSUnitCombatTypeChange[i].iModifier);
	}

	iNumElements = m_aArmorVSUnitCombatTypeChange.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aArmorVSUnitCombatTypeChange[i].eUnitCombat);
		CheckSum(iSum, m_aArmorVSUnitCombatTypeChange[i].iModifier);
	}

	iNumElements = m_aDodgeVSUnitCombatTypeChange.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aDodgeVSUnitCombatTypeChange[i].eUnitCombat);
		CheckSum(iSum, m_aDodgeVSUnitCombatTypeChange[i].iModifier);
	}

	iNumElements = m_aPrecisionVSUnitCombatTypeChange.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aPrecisionVSUnitCombatTypeChange[i].eUnitCombat);
		CheckSum(iSum, m_aPrecisionVSUnitCombatTypeChange[i].iModifier);
	}

	iNumElements = m_aCriticalVSUnitCombatTypeChange.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aCriticalVSUnitCombatTypeChange[i].eUnitCombat);
		CheckSum(iSum, m_aCriticalVSUnitCombatTypeChange[i].iModifier);
	}

	iNumElements = m_aRoundStunVSUnitCombatTypeChange.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aRoundStunVSUnitCombatTypeChange[i].eUnitCombat);
		CheckSum(iSum, m_aRoundStunVSUnitCombatTypeChange[i].iModifier);
	}

	iNumElements = m_aTrapAvoidanceUnitCombatTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aTrapAvoidanceUnitCombatTypes[i].eUnitCombat);
		CheckSum(iSum, m_aTrapAvoidanceUnitCombatTypes[i].iModifier);
	}

	iNumElements = m_aAfflictOnAttackChangeTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aAfflictOnAttackChangeTypes[i].eAfflictionLine);
		CheckSum(iSum, m_aAfflictOnAttackChangeTypes[i].iProbabilityChange);
		CheckSum(iSum, m_aAfflictOnAttackChangeTypes[i].iMelee);
		CheckSum(iSum, m_aAfflictOnAttackChangeTypes[i].iDistance);
		CheckSum(iSum, m_aAfflictOnAttackChangeTypes[i].iImmediate);
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

	m_PropertyManipulators.getCheckSum(iSum);

}

CvPropertyManipulators* CvUnitCombatInfo::getPropertyManipulators()
{
	return &m_PropertyManipulators;
}

CvOutcomeList* CvUnitCombatInfo::getKillOutcomeList()
{
	return &m_KillOutcomeList;
}

int CvUnitCombatInfo::getNumActionOutcomes()
{
	return m_aOutcomeMissions.size();
}

MissionTypes CvUnitCombatInfo::getActionOutcomeMission(int index)
{
	return m_aOutcomeMissions[index]->getMission();
}

CvOutcomeList* CvUnitCombatInfo::getActionOutcomeList(int index)
{
	return m_aOutcomeMissions[index]->getOutcomeList();
}

CvOutcomeList* CvUnitCombatInfo::getActionOutcomeListByMission(MissionTypes eMission)
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

CvOutcomeMission* CvUnitCombatInfo::getOutcomeMission(int index)
{
	return m_aOutcomeMissions[index];
}

CvOutcomeMission* CvUnitCombatInfo::getOutcomeMissionByMission(MissionTypes eMission)
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

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvInvisibleInfo
//
//  DESC:   Contains info about Invisibles
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvInvisibleInfo::CvInvisibleInfo() :
m_iChar(0),
m_iFontButtonIndex(0)
{
}

CvInvisibleInfo::~CvInvisibleInfo()
{
}

bool CvInvisibleInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	CvString szTextVal;

	pXML->GetOptionalChildXmlValByName(&m_iFontButtonIndex, L"FontButtonIndex");

	return true;
}

bool CvInvisibleInfo::readPass2(CvXMLLoadUtility* pXML)
{
	//Uncomment when needed
	//CvString szTextVal;


	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	return true;
}

void CvInvisibleInfo::copyNonDefaults(CvInvisibleInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	//Uncomment all of the following as needed
	//bool bDefault = false;
	int iDefault = 0;
	//int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	//int iAudioDefault = -1;  //all audio is default -1
	//float fDefault = 0.0f;
	//CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getFontButtonIndex() == iDefault) m_iFontButtonIndex = pClassInfo->getFontButtonIndex();

}

void CvInvisibleInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iChar);
	CheckSum(iSum, m_iFontButtonIndex);
}

int CvInvisibleInfo::getChar() const
{
	return m_iChar;
}

void CvInvisibleInfo::setChar(int i)
{
	m_iChar = i;
}

int CvInvisibleInfo::getFontButtonIndex() const
{
	return m_iFontButtonIndex;
}

