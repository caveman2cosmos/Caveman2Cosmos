//------------------------------------------------------------------------------------------------
//  FILE:    CvPromotionInfo.cpp
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvArtFileMgr.h"
#include "CvBuildingInfo.h"
#include "CvHeritageInfo.h"
#include "CvGameAI.h"
#include "CvGameTextMgr.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvInfoUtil.h"
#include "CvPlayerAI.h"
#include "CvPython.h"
#include "CvXMLLoadUtility.h"
#include "CvXMLLoadUtilityModTools.h"
#include "CheckSum.h"
#include "CvImprovementInfo.h"
#include "CvBonusInfo.h"
#include "CvPromotionInfo.h"


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
m_iPeaksWorkPercent(0),
//ls612: Work rate modifiers
m_iWorkRatePercent(0),
m_iCommandType(NO_COMMAND),
m_iRevoltProtection(0),
m_iCollateralDamageProtection(0),
m_iPillageChange(0),
m_iUpgradeDiscount(0),
m_iExperiencePercent(0),
m_iKamikazePercent(0),
m_iAirCombatLimitChange(0),
m_iCelebrityHappy(0),
m_iCollateralDamageLimitChange(0),
m_iCollateralDamageMaxUnitsChange(0),
m_iCombatLimitChange(0),
m_iExtraDropRange(0),
m_iSurvivorChance(0),
m_iVictoryAdjacentHeal(0),
m_iVictoryHeal(0),
m_iVictoryStackHeal(0),
m_bDefensiveVictoryMove(false),
m_bFreeDrop(false),
m_bOffensiveVictoryMove(false),
m_bOneUp(false),
m_bPillageEspionage(false),
m_bPillageMarauder(false),
m_bPillageOnMove(false),
m_bPillageOnVictory(false),
m_bPillageResearch(false),
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

m_bCanMovePeaks(false)
,m_bCanLeadThroughPeaks(false)
,m_iObsoleteTech(NO_TECH)
,m_iControlPoints(0)
,m_iCommandRange(0)
,m_iIgnoreTerrainDamage(NO_TERRAIN)
,m_bZoneOfControl(false)

,m_PropertyManipulators()
//TB Combat Mods Begin
//Textual References
,m_ePromotionLine(NO_PROMOTIONLINE),
m_eReplacesUnitCombat(NO_UNITCOMBAT),
m_eDomainCargoChange(NO_DOMAIN),
m_eSpecialCargoChange(NO_SPECIALUNIT),
m_eSpecialCargoPrereq(NO_SPECIALUNIT),
m_eSMNotSpecialCargoChange(NO_SPECIALUNIT),
m_eSMNotSpecialCargoPrereq(NO_SPECIALUNIT),
m_eSetSpecialUnit(NO_SPECIALUNIT),
//integers
m_iAttackCombatModifierChange(0),
m_iDefenseCombatModifierChange(0),
m_iPursuitChange(0),
m_iEarlyWithdrawChange(0),
m_iVSBarbsChange(0),
m_iUnnerveChange(0),
m_iEncloseChange(0),
m_iLungeChange(0),
m_iDynamicDefenseChange(0),
m_iStrengthChange(0),
m_iLinePriority(0),
m_iDamageperTurn(0),
m_iStrAdjperTurn(0),
m_iWeakenperTurn(0),
m_iEnduranceChange(0),
m_iPoisonProbabilityModifierChange(0),

m_iCaptureProbabilityModifierChange(0),
m_iCaptureResistanceModifierChange(0),

m_iBreakdownChanceChange(0),
m_iBreakdownDamageChange(0),
m_iTauntChange(0),
m_iMaxHPChange(0),
m_iStrengthModifier(0),
m_iQualityChange(0),
m_iGroupChange(0),
m_iLevelPrereq(0),
m_iDamageModifierChange(0),
m_iUpkeepModifier(0),
m_iExtraUpkeep100(0),
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
m_bParalyze(false),
m_bAttackOnlyCitiesAdd(false),
m_bAttackOnlyCitiesSubtract(false),
m_bIgnoreNoEntryLevelAdd(false),
m_bIgnoreNoEntryLevelSubtract(false),
m_bIgnoreZoneofControlAdd(false),
m_bIgnoreZoneofControlSubtract(false),
m_bFliesToMoveAdd(false),
m_bFliesToMoveSubtract(false),
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
m_bQuick(false),
m_bStarsign(false)
//TB Combat Mods End
{
	CvInfoUtil(this).initDataMembers();

	m_zobristValue = GC.getGame().getSorenRand().getInt();
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
	CvInfoUtil(this).uninitDataMembers();

	SAFE_DELETE_ARRAY(m_piTerrainAttackPercent);
	SAFE_DELETE_ARRAY(m_piTerrainDefensePercent);
	SAFE_DELETE_ARRAY(m_piFeatureAttackPercent);
	SAFE_DELETE_ARRAY(m_piFeatureDefensePercent);
	SAFE_DELETE_ARRAY(m_piUnitCombatModifierPercent);
	SAFE_DELETE_ARRAY(m_piDomainModifierPercent);
	SAFE_DELETE_ARRAY(m_pbTerrainDoubleMove);
	SAFE_DELETE_ARRAY(m_pbFeatureDoubleMove);
	SAFE_DELETE_ARRAY(m_pbUnitCombat);
	SAFE_DELETE_ARRAY(m_piTerrainWorkPercent);
	SAFE_DELETE_ARRAY(m_piFeatureWorkPercent);

	//GC.removeDelayedResolutionVector(m_vPromotionOverwrites);
	GC.removeDelayedResolutionVector(m_aiAddsBuildTypes);
	GC.removeDelayedResolutionVector(m_aiPrereqLocalBuildingTypes);
	GC.removeDelayedResolutionVector(m_aiTrapSetWithPromotionTypes);
}


int CvPromotionInfo::getLayerAnimationPath() const
{
	return m_iLayerAnimationPath;
}


TechTypes CvPromotionInfo::getTechPrereq() const
{
	if (m_iTechPrereq == NO_TECH)
	{
		if (getPromotionLine() == NO_PROMOTIONLINE)
		{
			return m_iTechPrereq;
		}
		return GC.getPromotionLineInfo(getPromotionLine()).getPrereqTech();
	}
	// Sets up the Tech Prereq on the Promotion Definition as a potential override to the Line Tech Prereq
	return m_iTechPrereq;
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


int CvPromotionInfo::getPeaksWorkPercent() const
{
	return m_iPeaksWorkPercent;
}


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


bool CvPromotionInfo::isOneUp() const
{
	return m_bOneUp;
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


const char* CvPromotionInfo::getSound() const
{
	return m_szSound;
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


int CvPromotionInfo::getTerrainAttackPercent(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), i);
	return m_piTerrainAttackPercent ? m_piTerrainAttackPercent[i] : 0;
}


bool CvPromotionInfo::isAnyTerrainAttackPercent() const
{
	return m_piTerrainAttackPercent != NULL;
}


int CvPromotionInfo::getTerrainDefensePercent(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), i);
	return m_piTerrainDefensePercent ? m_piTerrainDefensePercent[i] : 0;
}


bool CvPromotionInfo::isAnyTerrainDefensePercent() const
{
	return m_piTerrainDefensePercent != NULL;
}


int CvPromotionInfo::getFeatureAttackPercent(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), i);
	return m_piFeatureAttackPercent ? m_piFeatureAttackPercent[i] : 0;
}


bool CvPromotionInfo::isAnyFeatureAttackPercent() const
{
	return m_piFeatureAttackPercent != NULL;
}


int CvPromotionInfo::getFeatureDefensePercent(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), i);
	return m_piFeatureDefensePercent ? m_piFeatureDefensePercent[i] : 0;
}


bool CvPromotionInfo::isAnyFeatureDefensePercent() const
{
	return m_piFeatureDefensePercent != NULL;
}


int CvPromotionInfo::getUnitCombatModifierPercent(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return m_piUnitCombatModifierPercent ? m_piUnitCombatModifierPercent[i] : 0;
}


bool CvPromotionInfo::isAnyUnitCombatModifierPercent() const
{
	return m_piUnitCombatModifierPercent != NULL;
}


int CvPromotionInfo::getDomainModifierPercent(int i) const
{
	FASSERT_BOUNDS(0, NUM_DOMAIN_TYPES, i);
	return m_piDomainModifierPercent ? m_piDomainModifierPercent[i] : 0;
}


bool CvPromotionInfo::isAnyDomainModifierPercent() const
{
	return m_piDomainModifierPercent != NULL;
}


bool CvPromotionInfo::getTerrainDoubleMove(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), i);
	return m_pbTerrainDoubleMove ? m_pbTerrainDoubleMove[i] : false;
}


bool CvPromotionInfo::getFeatureDoubleMove(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), i);
	return m_pbFeatureDoubleMove ? m_pbFeatureDoubleMove[i] : false;
}


bool CvPromotionInfo::getUnitCombat(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return m_pbUnitCombat ? m_pbUnitCombat[i] : false;
}

//ls612: Terrain Work Modifiers
int CvPromotionInfo::getTerrainWorkPercent(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), i);
	return m_piTerrainWorkPercent ? m_piTerrainWorkPercent[i] : 0;
}


int CvPromotionInfo::getFeatureWorkPercent(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), i);
	return m_piFeatureWorkPercent ? m_piFeatureWorkPercent[i] : 0;
}


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


TechTypes CvPromotionInfo::getObsoleteTech() const
{
	if (m_iObsoleteTech == NO_TECH)
	{
		if (getPromotionLine() == NO_PROMOTIONLINE)
		{
			return m_iObsoleteTech;
		}
		return GC.getPromotionLineInfo(getPromotionLine()).getObsoleteTech();
	}
	//Sets up the Tech Prereq on the Promotion Definition as a potential override to the Line Tech Obsoletion
	return m_iObsoleteTech;
}


int CvPromotionInfo::getControlPoints() const
{
	return m_iControlPoints;
}


int CvPromotionInfo::getCommandRange() const
{
	return m_iCommandRange;
}


int CvPromotionInfo::getIgnoreTerrainDamage() const
{
	return m_iIgnoreTerrainDamage;
}


bool CvPromotionInfo::isZoneOfControl() const
{
	return m_bZoneOfControl;
}


//TB Combat Mods Begin  TB SubCombat Mod begin
const wchar_t* CvPromotionInfo::getRenamesUnitTo() const
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


int CvPromotionInfo::getPursuitChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_FIGHT_OR_FLIGHT))
	{
		return 0;
	}
	return m_iPursuitChange;
}


int CvPromotionInfo::getEarlyWithdrawChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_FIGHT_OR_FLIGHT))
	{
		return 0;
	}
	return m_iEarlyWithdrawChange;
}


int CvPromotionInfo::getVSBarbsChange() const
{
	return m_iVSBarbsChange;
}


//S&D extended
int CvPromotionInfo::getUnnerveChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SURROUND_DESTROY))
	{
		return 0;
	}
	return m_iUnnerveChange;
}


int CvPromotionInfo::getEncloseChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SURROUND_DESTROY))
	{
		return 0;
	}
	return m_iEncloseChange;
}


int CvPromotionInfo::getLungeChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SURROUND_DESTROY))
	{
		return 0;
	}
	return m_iLungeChange;
}


int CvPromotionInfo::getDynamicDefenseChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SURROUND_DESTROY))
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


int CvPromotionInfo::getEnduranceChange() const
{
	return m_iEnduranceChange;
}


int CvPromotionInfo::getPoisonProbabilityModifierChange() const
{
	return m_iPoisonProbabilityModifierChange;
}



int CvPromotionInfo::getCaptureProbabilityModifierChange() const
{
	return m_iCaptureProbabilityModifierChange;
}


int CvPromotionInfo::getCaptureResistanceModifierChange() const
{
	return m_iCaptureResistanceModifierChange;
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


int CvPromotionInfo::getUpkeepModifier() const
{
	return m_iUpkeepModifier;
}


int CvPromotionInfo::getExtraUpkeep100() const
{
	return m_iExtraUpkeep100;
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


int CvPromotionInfo::getCombatModifierPerSizeMoreChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerSizeMoreChange;
}


int CvPromotionInfo::getCombatModifierPerSizeLessChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerSizeLessChange;
}


int CvPromotionInfo::getCombatModifierPerVolumeMoreChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerVolumeMoreChange;
}


int CvPromotionInfo::getCombatModifierPerVolumeLessChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
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


int CvPromotionInfo::getStealthStrikesChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_WITHOUT_WARNING))
	{
		return 0;
	}
	return m_iStealthStrikesChange;
}


int CvPromotionInfo::getStealthCombatModifierChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_WITHOUT_WARNING))
	{
		return 0;
	}
	return m_iStealthCombatModifierChange;
}


int CvPromotionInfo::getStealthDefenseChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_WITHOUT_WARNING))
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


bool CvPromotionInfo::isParalyze() const
{
	return m_bParalyze;
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


bool CvPromotionInfo::isSubCombatChangeType(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return algo::any_of_equal(m_aiSubCombatChangeTypes, i);
}


int CvPromotionInfo::getRemovesUnitCombatType(int i) const
{
	return m_aiRemovesUnitCombatTypes[i];
}


int CvPromotionInfo::getNumRemovesUnitCombatTypes() const
{
	return (int)m_aiRemovesUnitCombatTypes.size();
}


bool CvPromotionInfo::isRemovesUnitCombatType(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return algo::any_of_equal(m_aiRemovesUnitCombatTypes, i);
}


int CvPromotionInfo::getOnGameOption(int i) const
{
	return m_aiOnGameOptions[i];
}


int CvPromotionInfo::getNumOnGameOptions() const
{
	return (int)m_aiOnGameOptions.size();
}


bool CvPromotionInfo::isOnGameOption(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumGameOptionInfos(), i);
	return algo::any_of_equal(m_aiOnGameOptions, i);
}


int CvPromotionInfo::getNotOnGameOption(int i) const
{
	return m_aiNotOnGameOptions[i];
}


int CvPromotionInfo::getNumNotOnGameOptions() const
{
	return (int)m_aiNotOnGameOptions.size();
}


bool CvPromotionInfo::isNotOnGameOption(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumGameOptionInfos(), i);
	return algo::any_of_equal(m_aiNotOnGameOptions, i);
}


int CvPromotionInfo::getFreetoUnitCombat(int i) const
{
	return m_aiFreetoUnitCombats[i];
}


int CvPromotionInfo::getNumFreetoUnitCombats() const
{
	return (int)m_aiFreetoUnitCombats.size();
}


bool CvPromotionInfo::isFreetoUnitCombat(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return algo::any_of_equal(m_aiFreetoUnitCombats, i);
}


int CvPromotionInfo::getNotOnDomainType(int i) const
{
	return m_aiNotOnDomainTypes[i];
}


int CvPromotionInfo::getNumNotOnDomainTypes() const
{
	return (int)m_aiNotOnDomainTypes.size();
}


bool CvPromotionInfo::isNotOnDomainType(int i) const
{
	FASSERT_BOUNDS(0, NUM_DOMAIN_TYPES, i);
	return algo::any_of_equal(m_aiNotOnDomainTypes, i);
}


int CvPromotionInfo::getAddsBuildType(int i) const
{
	return m_aiAddsBuildTypes[i];
}


int CvPromotionInfo::getNumAddsBuildTypes() const
{
	return (int)m_aiAddsBuildTypes.size();
}


bool CvPromotionInfo::isAddsBuildType(int i) const
{
	return algo::any_of_equal(m_aiAddsBuildTypes, i);
}


int CvPromotionInfo::getNegatesInvisibilityType(int i) const
{
	return m_aiNegatesInvisibilityTypes[i];
}


int CvPromotionInfo::getNumNegatesInvisibilityTypes() const
{
	return (int)m_aiNegatesInvisibilityTypes.size();
}


bool CvPromotionInfo::isNegatesInvisibilityType(int i) const
{
	return algo::any_of_equal(m_aiNegatesInvisibilityTypes, i);
}


int CvPromotionInfo::getPrereqTerrainType(int i) const
{
	return m_aiPrereqTerrainTypes[i];
}


int CvPromotionInfo::getNumPrereqTerrainTypes() const
{
	return (int)m_aiPrereqTerrainTypes.size();
}


bool CvPromotionInfo::isPrereqTerrainType(int i) const
{
	return algo::any_of_equal(m_aiPrereqTerrainTypes, i);
}


int CvPromotionInfo::getPrereqFeatureType(int i) const
{
	return m_aiPrereqFeatureTypes[i];
}


int CvPromotionInfo::getNumPrereqFeatureTypes() const
{
	return (int)m_aiPrereqFeatureTypes.size();
}


bool CvPromotionInfo::isPrereqFeatureType(int i) const
{
	return algo::any_of_equal(m_aiPrereqFeatureTypes, i);
}


int CvPromotionInfo::getPrereqImprovementType(int i) const
{
	return m_aiPrereqImprovementTypes[i];
}


int CvPromotionInfo::getNumPrereqImprovementTypes() const
{
	return (int)m_aiPrereqImprovementTypes.size();
}


bool CvPromotionInfo::isPrereqImprovementType(int i) const
{
	return algo::any_of_equal(m_aiPrereqImprovementTypes, i);
}


int CvPromotionInfo::getPrereqPlotBonusType(int i) const
{
	return m_aiPrereqPlotBonusTypes[i];
}


int CvPromotionInfo::getNumPrereqPlotBonusTypes() const
{
	return (int)m_aiPrereqPlotBonusTypes.size();
}


bool CvPromotionInfo::isPrereqPlotBonusType(int i) const
{
	return algo::any_of_equal(m_aiPrereqPlotBonusTypes, i);
}


int CvPromotionInfo::getPrereqLocalBuildingType(int i) const
{
	return m_aiPrereqLocalBuildingTypes[i];
}


int CvPromotionInfo::getNumPrereqLocalBuildingTypes() const
{
	return (int)m_aiPrereqLocalBuildingTypes.size();
}


bool CvPromotionInfo::isPrereqLocalBuildingType(int i) const
{
	return algo::any_of_equal(m_aiPrereqLocalBuildingTypes, i);
}


int CvPromotionInfo::getTrapSetWithPromotionType(int i) const
{
	return m_aiTrapSetWithPromotionTypes[i];
}


int CvPromotionInfo::getNumTrapSetWithPromotionTypes() const
{
	return (int)m_aiTrapSetWithPromotionTypes.size();
}


bool CvPromotionInfo::isTrapSetWithPromotionType(int i) const
{
	return algo::any_of_equal(m_aiTrapSetWithPromotionTypes, i);
}


int CvPromotionInfo::getTrapImmunityUnitCombatType(int i) const
{
	return m_aiTrapImmunityUnitCombatTypes[i];
}


int CvPromotionInfo::getNumTrapImmunityUnitCombatTypes() const
{
	return (int)m_aiTrapImmunityUnitCombatTypes.size();
}


bool CvPromotionInfo::isTrapImmunityUnitCombatType(int i) const
{
	return algo::any_of_equal(m_aiTrapImmunityUnitCombatTypes, i);
}


int CvPromotionInfo::getTargetUnitCombatType(int i) const
{
	return m_aiTargetUnitCombatTypes[i];
}


int CvPromotionInfo::getNumTargetUnitCombatTypes() const
{
	return (int)m_aiTargetUnitCombatTypes.size();
}


bool CvPromotionInfo::isTargetUnitCombatType(int i) const
{
	return algo::any_of_equal(m_aiTargetUnitCombatTypes, i);
}


int CvPromotionInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}


int CvPromotionInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}


bool CvPromotionInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}


// int vectors utilizing pairing without delayed resolution
int CvPromotionInfo::getNumFlankingStrikesbyUnitCombatTypesChange() const
{
	return m_aFlankingStrengthbyUnitCombatTypeChange.size();
}


int CvPromotionInfo::getFlankingStrengthbyUnitCombatTypeChange(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aFlankingStrengthbyUnitCombatTypeChange.begin(); it != m_aFlankingStrengthbyUnitCombatTypeChange.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}


bool CvPromotionInfo::isFlankingStrikebyUnitCombatTypeChange(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
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
	PROFILE_EXTRA_FUNC();
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
	PROFILE_EXTRA_FUNC();
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
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aWithdrawVSUnitCombatChangeTypes.begin(); it != m_aWithdrawVSUnitCombatChangeTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}


bool CvPromotionInfo::isWithdrawVSUnitCombatChangeType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
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
	PROFILE_EXTRA_FUNC();
	if (!bForLoad && !GC.getGame().isOption(GAMEOPTION_COMBAT_FIGHT_OR_FLIGHT))
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


bool CvPromotionInfo::isPursuitVSUnitCombatChangeType(int iUnitCombat, bool bForLoad) const
{
	PROFILE_EXTRA_FUNC();
	if (!bForLoad && !GC.getGame().isOption(GAMEOPTION_COMBAT_FIGHT_OR_FLIGHT))
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


int CvPromotionInfo::getNumTrapDisableUnitCombatTypes() const
{
	return m_aTrapDisableUnitCombatTypes.size();
}


int CvPromotionInfo::getTrapDisableUnitCombatType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aTrapDisableUnitCombatTypes.begin(); it != m_aTrapDisableUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}


bool CvPromotionInfo::isTrapDisableUnitCombatType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
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
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aTrapAvoidanceUnitCombatTypes.begin(); it != m_aTrapAvoidanceUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}


bool CvPromotionInfo::isTrapAvoidanceUnitCombatType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
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
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aTrapTriggerUnitCombatTypes.begin(); it != m_aTrapTriggerUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}
	return 0;
}


bool CvPromotionInfo::isTrapTriggerUnitCombatType(int iUnitCombat) const
{
	PROFILE_EXTRA_FUNC();
	for (UnitCombatModifierArray::const_iterator it = m_aTrapTriggerUnitCombatTypes.begin(); it != m_aTrapTriggerUnitCombatTypes.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
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
	PROFILE_EXTRA_FUNC();
	for (BuildModifierArray::const_iterator it = m_aBuildWorkRateModifierChangeTypes.begin(); it != m_aBuildWorkRateModifierChangeTypes.end(); ++it)
	{
		if ((*it).first == (BuildTypes)iBuild)
		{
			return (*it).second;
		}
	}
	return 0;
}


bool CvPromotionInfo::isBuildWorkRateModifierChangeType(int iBuild) const
{
	PROFILE_EXTRA_FUNC();
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
	PROFILE_EXTRA_FUNC();
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensityChangeTypes.begin(); it != m_aVisibilityIntensityChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return (*it).second;
		}
	}
	return 0;
}


bool CvPromotionInfo::isVisibilityIntensityChangeType(int iInvisibility) const
{
	PROFILE_EXTRA_FUNC();
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
	PROFILE_EXTRA_FUNC();
	for (InvisibilityArray::const_iterator it = m_aInvisibilityIntensityChangeTypes.begin(); it != m_aInvisibilityIntensityChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return (*it).second;
		}
	}
	return 0;
}


bool CvPromotionInfo::isInvisibilityIntensityChangeType(int iInvisibility) const
{
	PROFILE_EXTRA_FUNC();
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
	PROFILE_EXTRA_FUNC();
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensityRangeChangeTypes.begin(); it != m_aVisibilityIntensityRangeChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return (*it).second;
		}
	}
	return 0;
}


bool CvPromotionInfo::isVisibilityIntensityRangeChangeType(int iInvisibility) const
{
	PROFILE_EXTRA_FUNC();
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensityRangeChangeTypes.begin(); it != m_aVisibilityIntensityRangeChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return true;
		}
	}
	return false;
}


int CvPromotionInfo::getNumAIWeightbyUnitCombatTypes() const
{
	return (int)m_aAIWeightbyUnitCombatTypes.size();
}


const UnitCombatModifier& CvPromotionInfo::getAIWeightbyUnitCombatType(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, getNumAIWeightbyUnitCombatTypes(), iUnitCombat);
	return m_aAIWeightbyUnitCombatTypes[iUnitCombat];
}


int CvPromotionInfo::getNumInvisibleTerrainChanges() const
{
	return (int)m_aInvisibleTerrainChanges.size();
}


const InvisibleTerrainChanges& CvPromotionInfo::getInvisibleTerrainChange(int iIndex) const
{
	return m_aInvisibleTerrainChanges[iIndex];
}


int CvPromotionInfo::getNumInvisibleFeatureChanges() const
{
	return (int)m_aInvisibleFeatureChanges.size();
}


const InvisibleFeatureChanges& CvPromotionInfo::getInvisibleFeatureChange(int iIndex) const
{
	return m_aInvisibleFeatureChanges[iIndex];
}


int CvPromotionInfo::getNumInvisibleImprovementChanges() const
{
	return (int)m_aInvisibleImprovementChanges.size();
}


const InvisibleImprovementChanges& CvPromotionInfo::getInvisibleImprovementChange(int iIndex) const
{
	return m_aInvisibleImprovementChanges[iIndex];
}


int CvPromotionInfo::getNumVisibleTerrainChanges() const
{
	return (int)m_aVisibleTerrainChanges.size();
}


const InvisibleTerrainChanges& CvPromotionInfo::getVisibleTerrainChange(int iIndex) const
{
	return m_aVisibleTerrainChanges[iIndex];
}


int CvPromotionInfo::getNumVisibleFeatureChanges() const
{
	return (int)m_aVisibleFeatureChanges.size();
}


const InvisibleFeatureChanges& CvPromotionInfo::getVisibleFeatureChange(int iIndex) const
{
	return m_aVisibleFeatureChanges[iIndex];
}


int CvPromotionInfo::getNumVisibleImprovementChanges() const
{
	return (int)m_aVisibleImprovementChanges.size();
}


const InvisibleImprovementChanges& CvPromotionInfo::getVisibleImprovementChange(int iIndex) const
{
	return m_aVisibleImprovementChanges[iIndex];
}


int CvPromotionInfo::getNumVisibleTerrainRangeChanges() const
{
	return (int)m_aVisibleTerrainRangeChanges.size();
}


const InvisibleTerrainChanges& CvPromotionInfo::getVisibleTerrainRangeChange(int iIndex) const
{
	return m_aVisibleTerrainRangeChanges[iIndex];
}


int CvPromotionInfo::getNumVisibleFeatureRangeChanges() const
{
	return (int)m_aVisibleFeatureRangeChanges.size();
}


const InvisibleFeatureChanges& CvPromotionInfo::getVisibleFeatureRangeChange(int iIndex) const
{
	return m_aVisibleFeatureRangeChanges[iIndex];
}


int CvPromotionInfo::getNumVisibleImprovementRangeChanges() const
{
	return (int)m_aVisibleImprovementRangeChanges.size();
}


const InvisibleImprovementChanges& CvPromotionInfo::getVisibleImprovementRangeChange(int iIndex) const
{
	return m_aVisibleImprovementRangeChanges[iIndex];
}


int CvPromotionInfo::getNumHealUnitCombatChangeTypes() const
{
	return (int)m_aHealUnitCombatChangeTypes.size();
}


const HealUnitCombat& CvPromotionInfo::getHealUnitCombatChangeType(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, getNumHealUnitCombatChangeTypes(), iUnitCombat);
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


bool CvPromotionInfo::isQualifiedUnitCombatType(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return algo::any_of_equal(m_aiQualifiedUnitCombatTypes, i);
}


void CvPromotionInfo::setQualifiedUnitCombatTypes()
{
	PROFILE_EXTRA_FUNC();
	m_aiQualifiedUnitCombatTypes.clear();
	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		if (getUnitCombat(iI))
		{
			m_aiQualifiedUnitCombatTypes.push_back(iI);
		}
	}
	const PromotionLineTypes ePromotionLine = getPromotionLine();

	if (ePromotionLine > -1)
	{
		for (int iI = 0; iI < GC.getPromotionLineInfo(ePromotionLine).getNumUnitCombatPrereqTypes(); iI++)
		{
			const int iUnitCombat = GC.getPromotionLineInfo(ePromotionLine).getUnitCombatPrereqType(iI);

			if (!isQualifiedUnitCombatType(iUnitCombat))
			{
				m_aiQualifiedUnitCombatTypes.push_back(iUnitCombat);
			}
		}
	}
}


int CvPromotionInfo::getNotOnUnitCombatType(int i) const
{
	return m_aiNotOnUnitCombatTypes[i];
}


int CvPromotionInfo::getNumNotOnUnitCombatTypes() const
{
	return (int)m_aiNotOnUnitCombatTypes.size();
}


bool CvPromotionInfo::isNotOnUnitCombatType(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return algo::any_of_equal(m_aiNotOnUnitCombatTypes, i);
}


int CvPromotionInfo::getDisqualifiedUnitCombatType(int i) const
{
	return m_disqualifiedUnitCombatTypes[i];
}


int CvPromotionInfo::getNumDisqualifiedUnitCombatTypes() const
{
	return (int)m_disqualifiedUnitCombatTypes.size();
}


void CvPromotionInfo::setDisqualifiedUnitCombatTypes()
{
	PROFILE_EXTRA_FUNC();
	m_disqualifiedUnitCombatTypes.clear();
	for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
	{
		if (isNotOnUnitCombatType(iI))
		{
			if (isQualifiedUnitCombatType(iI))
			{
				std::vector<int>::iterator itr = find(m_aiQualifiedUnitCombatTypes.begin(), m_aiQualifiedUnitCombatTypes.end(), iI);
				if (itr != m_aiQualifiedUnitCombatTypes.end())
				{
					m_aiQualifiedUnitCombatTypes.erase(itr);
				}
			}
			m_disqualifiedUnitCombatTypes.push_back(iI);
		}
	}
	const PromotionLineTypes ePromotionLine = getPromotionLine();

	if (ePromotionLine > -1)
	{
		for (int iI = 0; iI < GC.getPromotionLineInfo(ePromotionLine).getNumNotOnUnitCombatTypes(); iI++)
		{
			const int iUnitCombat = GC.getPromotionLineInfo(ePromotionLine).getNotOnUnitCombatType(iI);

			if (!isNotOnUnitCombatType(iUnitCombat))
			{
				if (isQualifiedUnitCombatType(iUnitCombat))
				{
					std::vector<int>::iterator itr = find(m_aiQualifiedUnitCombatTypes.begin(), m_aiQualifiedUnitCombatTypes.end(), iI);
					if (itr != m_aiQualifiedUnitCombatTypes.end())
					{
						m_aiQualifiedUnitCombatTypes.erase(itr);
					}
				}
				m_disqualifiedUnitCombatTypes.push_back(iUnitCombat);
			}
		}
	}
}


//TB Combat Mods End  TB SubCombat Mod end

bool CvPromotionInfo::hasNegativeEffects() const
{
	return getPursuitChange() < 0 ||
		getLungeChange() < 0 ||
		getEnduranceChange() < 0 ||
		getFirstStrikesChange() < 0 ||
		getChanceFirstStrikesChange() < 0 ||
		getVSBarbsChange() < 0 ||
		getStrengthChange() < 0 ||
		getAttackCombatModifierChange() < 0 ||
		getCombatPercent() < 0 ||
		getDefenseOnlyChange() > 0 ||
		getNoInvisibilityChange() > 0 ||
		getHiddenNationalityChange() != 0;
}


void CvPromotionInfo::getDataMembers(CvInfoUtil& util)
{
	util
		.addEnum(m_iPrereqPromotion, L"PromotionPrereq")
		.addEnum(m_iPrereqOrPromotion1, L"PromotionPrereqOr1")
		.addEnum(m_iPrereqOrPromotion2, L"PromotionPrereqOr2")
	;
}


bool CvPromotionInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvInfoUtil(this).readXml(pXML);

	CvString szTextVal;
	CvString szTextVal2;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szSound, L"Sound");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"LayerAnimationPath");
	m_iLayerAnimationPath = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalTypeEnum(m_iTechPrereq, L"TechPrereq");

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
	pXML->GetOptionalChildXmlValByName(&m_iHillsWorkPercent, L"iHillsWorkModifier");
	pXML->GetOptionalChildXmlValByName(&m_iPeaksWorkPercent, L"iPeaksWorkModifier");
	pXML->GetOptionalChildXmlValByName(&m_iWorkRatePercent, L"iWorkRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iRevoltProtection, L"iRevoltProtection");
	pXML->GetOptionalChildXmlValByName(&m_iCollateralDamageProtection, L"iCollateralDamageProtection");
	pXML->GetOptionalChildXmlValByName(&m_iPillageChange, L"iPillageChange");
	pXML->GetOptionalChildXmlValByName(&m_iUpgradeDiscount, L"iUpgradeDiscount");
	pXML->GetOptionalChildXmlValByName(&m_iExperiencePercent, L"iExperiencePercent");
	pXML->GetOptionalChildXmlValByName(&m_iKamikazePercent, L"iKamikazePercent");
	pXML->GetOptionalChildXmlValByName(&m_bDefensiveVictoryMove, L"bDefensiveVictoryMove");
	pXML->GetOptionalChildXmlValByName(&m_bFreeDrop, L"bFreeDrop");
	pXML->GetOptionalChildXmlValByName(&m_bOffensiveVictoryMove, L"bOffensiveVictoryMove");
	pXML->GetOptionalChildXmlValByName(&m_bOneUp, L"bOneUp");
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
	pXML->GetOptionalChildXmlValByName(&m_iSurvivorChance, L"iSurvivorChance");
	pXML->GetOptionalChildXmlValByName(&m_iVictoryAdjacentHeal, L"iVictoryAdjacentHeal");
	pXML->GetOptionalChildXmlValByName(&m_iVictoryHeal, L"iVictoryHeal");
	pXML->GetOptionalChildXmlValByName(&m_iVictoryStackHeal, L"iVictoryStackHeal");

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

	pXML->GetOptionalChildXmlValByName(&m_bCanMovePeaks, L"bCanMovePeaks");
	//	Koshling - enhanced mountaineering mode to differentiate between ability to move through
	//	mountains, and ability to lead a stack through mountains
	pXML->GetOptionalChildXmlValByName(&m_bCanLeadThroughPeaks, L"bCanLeadThroughPeaks");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"ObsoleteTech");
	m_iObsoleteTech = static_cast<TechTypes>(pXML->GetInfoClass(szTextVal));
	pXML->GetOptionalChildXmlValByName(&m_iControlPoints, L"iControlPoints");
	pXML->GetOptionalChildXmlValByName(&m_iCommandRange, L"iCommandRange");
	pXML->GetOptionalChildXmlValByName(&m_bZoneOfControl, L"bZoneOfControl");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"IgnoreTerrainDamage");
	m_iIgnoreTerrainDamage = pXML->GetInfoClass(szTextVal);

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
	pXML->GetOptionalChildXmlValByName(&m_iUnnerveChange, L"iUnnerveChange");
	pXML->GetOptionalChildXmlValByName(&m_iEncloseChange, L"iEncloseChange");
	pXML->GetOptionalChildXmlValByName(&m_iLungeChange, L"iLungeChange");
	pXML->GetOptionalChildXmlValByName(&m_iDynamicDefenseChange, L"iDynamicDefenseChange");
	pXML->GetOptionalChildXmlValByName(&m_iStrengthChange, L"iStrengthChange");
	pXML->GetOptionalChildXmlValByName(&m_iLinePriority, L"iLinePriority");
	pXML->GetOptionalChildXmlValByName(&m_iDamageperTurn, L"iDamageperTurn");
	pXML->GetOptionalChildXmlValByName(&m_iStrAdjperTurn, L"iStrAdjperTurn");
	pXML->GetOptionalChildXmlValByName(&m_iWeakenperTurn, L"iWeakenperTurn");
	pXML->GetOptionalChildXmlValByName(&m_iEnduranceChange, L"iEnduranceChange");
	pXML->GetOptionalChildXmlValByName(&m_iPoisonProbabilityModifierChange, L"iPoisonProbabilityModifierChange");

	pXML->GetOptionalChildXmlValByName(&m_iCaptureProbabilityModifierChange, L"iCaptureProbabilityModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iCaptureResistanceModifierChange, L"iCaptureResistanceModifierChange");

	pXML->GetOptionalChildXmlValByName(&m_iBreakdownChanceChange, L"iBreakdownChanceChange");
	pXML->GetOptionalChildXmlValByName(&m_iBreakdownDamageChange, L"iBreakdownDamageChange");
	pXML->GetOptionalChildXmlValByName(&m_iTauntChange, L"iTauntChange");
	pXML->GetOptionalChildXmlValByName(&m_iMaxHPChange, L"iMaxHPChange");
	pXML->GetOptionalChildXmlValByName(&m_iStrengthModifier, L"iStrengthModifier");
	pXML->GetOptionalChildXmlValByName(&m_iQualityChange, L"iQualityChange");
	pXML->GetOptionalChildXmlValByName(&m_iGroupChange, L"iGroupChange");
	pXML->GetOptionalChildXmlValByName(&m_iLevelPrereq, L"iLevelPrereq");
	pXML->GetOptionalChildXmlValByName(&m_iDamageModifierChange, L"iDamageModifierChange");
	pXML->GetOptionalChildXmlValByName(&m_iUpkeepModifier, L"iUpkeepModifier");
	pXML->GetOptionalChildXmlValByName(&m_iExtraUpkeep100, L"iExtraUpkeep100");
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

	pXML->GetOptionalChildXmlValByName(&m_bParalyze, L"bParalyze");
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
	pXML->GetOptionalChildXmlValByName(&m_bStarsign, L"bStarsign");
	//pXML->SetVariableListTagPair(&m_piAIWeightbyUnitCombatTypes, L"AIWeightbyUnitCombatTypes", GC.getNumUnitCombatInfos());
	pXML->SetOptionalVector(&m_aiSubCombatChangeTypes, L"SubCombatChangeTypes");
	pXML->SetOptionalVector(&m_aiRemovesUnitCombatTypes, L"RemovesUnitCombatTypes");
	pXML->SetOptionalVector(&m_aiOnGameOptions, L"OnGameOptions");
	pXML->SetOptionalVector(&m_aiNotOnGameOptions, L"NotOnGameOptions");
	pXML->SetOptionalVector(&m_aiFreetoUnitCombats, L"FreetoUnitCombats");
	pXML->SetOptionalVector(&m_aiNotOnUnitCombatTypes, L"NotOnUnitCombatTypes");
	pXML->SetOptionalVector(&m_aiNotOnDomainTypes, L"NotOnDomainTypes");
	pXML->SetOptionalVector(&m_aeMapCategoryTypes, L"MapCategoryTypes");

	pXML->SetOptionalVector(&m_aiPrereqBonusTypes, L"PrereqBonusTypes");
	pXML->SetOptionalVectorWithDelayedResolution(m_aiAddsBuildTypes, L"AddsBuildTypes");
	pXML->SetOptionalVector(&m_aiNegatesInvisibilityTypes, L"NegatesInvisibilityTypes");
	pXML->SetOptionalVector(&m_aiPrereqTerrainTypes, L"PrereqTerrainTypes");
	pXML->SetOptionalVector(&m_aiPrereqFeatureTypes, L"PrereqFeatureTypes");
	pXML->SetOptionalVector(&m_aiPrereqImprovementTypes, L"PrereqImprovementTypes");
	pXML->SetOptionalVector(&m_aiPrereqPlotBonusTypes, L"PrereqPlotBonusTypes");
	pXML->SetOptionalVectorWithDelayedResolution(m_aiPrereqLocalBuildingTypes, L"PrereqLocalBuildingTypes");
	pXML->SetOptionalVectorWithDelayedResolution(m_aiTrapSetWithPromotionTypes, L"TrapSetWithPromotionTypes");
	pXML->SetOptionalVector(&m_aiTrapImmunityUnitCombatTypes, L"TrapImmunityUnitCombatTypes");
	pXML->SetOptionalVector(&m_aiTargetUnitCombatTypes, L"TargetUnitCombatTypes");
	pXML->SetOptionalVector(&m_aiCategories, L"Categories");

	// int vector utilizing pairing without delayed resolution
	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aFlankingStrengthbyUnitCombatTypeChange, L"FlankingStrikesbyUnitCombatChange");

	pXML->SetOptionalPairVector<TerrainModifierArray, TerrainTypes, int>(&m_aWithdrawOnTerrainTypesChange, L"WithdrawModifierOnTerrainTypeChanges");

	pXML->SetOptionalPairVector<FeatureModifierArray, FeatureTypes, int>(&m_aWithdrawOnFeatureTypesChange, L"WithdrawModifierOnFeatureTypeChanges");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aWithdrawVSUnitCombatChangeTypes, L"WithdrawVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aPursuitVSUnitCombatChangeTypes, L"PursuitVSUnitCombatChangeTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aTrapDisableUnitCombatTypes, L"TrapDisableUnitCombatTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aTrapAvoidanceUnitCombatTypes, L"TrapAvoidanceUnitCombatTypes");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aTrapTriggerUnitCombatTypes, L"TrapTriggerUnitCombatTypes");

	//pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(m_aAIWeightbyUnitCombatTypes, L"AIWeightbyUnitCombatTypes");

	pXML->SetOptionalPairVector<BuildModifierArray, BuildTypes, int>(&m_aBuildWorkRateModifierChangeTypes, L"BuildWorkRateModifierChangeTypes");

	pXML->SetOptionalPairVector<InvisibilityArray, InvisibleTypes, int>(&m_aVisibilityIntensityChangeTypes, L"VisibilityIntensityChangeTypes");

	pXML->SetOptionalPairVector<InvisibilityArray, InvisibleTypes, int>(&m_aInvisibilityIntensityChangeTypes, L"InvisibilityIntensityChangeTypes");

	pXML->SetOptionalPairVector<InvisibilityArray, InvisibleTypes, int>(&m_aVisibilityIntensityRangeChangeTypes, L"VisibilityIntensityRangeChangeTypes");

	// int vector utilizing struct with delayed resolution
	if(pXML->TryMoveToXmlFirstChild(L"AIWeightbyUnitCombatTypes"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"AIWeightbyUnitCombatType" );
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

	if(pXML->TryMoveToXmlFirstChild(L"HealUnitCombatChangeTypes"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"HealUnitCombatChangeType" );
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
		//TB Combat Mod End
	//pXML->SetOptionalVectorWithDelayedResolution(PromotionOverwrites, L"PromotionOverwrites");

	return true;
}


void CvPromotionInfo::copyNonDefaults(const CvPromotionInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	CvHotkeyInfo::copyNonDefaults(pClassInfo);

	CvInfoUtil(this).copyNonDefaults(pClassInfo);

	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	if (getSound() == cDefault) m_szSound = pClassInfo->getSound();

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
	if (getPeaksWorkPercent() == iDefault) m_iPeaksWorkPercent = pClassInfo->getPeaksWorkPercent();
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

	if (isDefensiveVictoryMove() == bDefault) m_bDefensiveVictoryMove = pClassInfo->isDefensiveVictoryMove();
	if (isFreeDrop() == bDefault) m_bFreeDrop = pClassInfo->isFreeDrop();
	if (isOffensiveVictoryMove() == bDefault) m_bOffensiveVictoryMove = pClassInfo->isOffensiveVictoryMove();
	if (isOneUp() == bDefault) m_bOneUp = pClassInfo->isOneUp();
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
	if (getSurvivorChance() == iDefault) m_iSurvivorChance = pClassInfo->getSurvivorChance();
	if (getVictoryAdjacentHeal() == iDefault) m_iVictoryAdjacentHeal = pClassInfo->getVictoryAdjacentHeal();
	if (getVictoryHeal() == iDefault) m_iVictoryHeal = pClassInfo->getVictoryHeal();
	if (getVictoryStackHeal() == iDefault) m_iVictoryStackHeal = pClassInfo->getVictoryStackHeal();

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
	if (getSMNotSpecialCargoChange() == NO_SPECIALUNIT) m_eSMNotSpecialCargoChange = pClassInfo->getSMNotSpecialCargoChange();
	if (getSMNotSpecialCargoPrereq() == NO_SPECIALUNIT) m_eSMNotSpecialCargoPrereq = pClassInfo->getSMNotSpecialCargoPrereq();
	if (setSpecialUnit() == NO_SPECIALUNIT) m_eSetSpecialUnit = pClassInfo->setSpecialUnit();
	//integers
	if (getAttackCombatModifierChange() == iDefault) m_iAttackCombatModifierChange = pClassInfo->getAttackCombatModifierChange();
	if (getDefenseCombatModifierChange() == iDefault) m_iDefenseCombatModifierChange = pClassInfo->getDefenseCombatModifierChange();
	if (m_iPursuitChange == iDefault) m_iPursuitChange = pClassInfo->m_iPursuitChange;
	if (m_iEarlyWithdrawChange == iDefault) m_iEarlyWithdrawChange = pClassInfo->m_iEarlyWithdrawChange;
	if (getVSBarbsChange() == iDefault) m_iVSBarbsChange = pClassInfo->getVSBarbsChange();
	if (m_iUnnerveChange == iDefault) m_iUnnerveChange = pClassInfo->m_iUnnerveChange;
	if (m_iEncloseChange == iDefault) m_iEncloseChange = pClassInfo->m_iEncloseChange;
	if (m_iLungeChange == iDefault) m_iLungeChange = pClassInfo->m_iLungeChange;
	if (m_iDynamicDefenseChange == iDefault) m_iDynamicDefenseChange = pClassInfo->m_iDynamicDefenseChange;
	if (getStrengthChange() == iDefault) m_iStrengthChange = pClassInfo->getStrengthChange();
	if (getLinePriority() == iDefault) m_iLinePriority = pClassInfo->getLinePriority();
	if (getDamageperTurn() == iDefault) m_iDamageperTurn = pClassInfo->getDamageperTurn();
	if (getStrAdjperTurn() == iDefault) m_iStrAdjperTurn = pClassInfo->getStrAdjperTurn();
	if (getWeakenperTurn() == iDefault) m_iWeakenperTurn = pClassInfo->getWeakenperTurn();
	if (getEnduranceChange() == iDefault) m_iEnduranceChange = pClassInfo->getEnduranceChange();
	if (getPoisonProbabilityModifierChange() == iDefault) m_iPoisonProbabilityModifierChange = pClassInfo->getPoisonProbabilityModifierChange();

	if (getCaptureProbabilityModifierChange() == iDefault) m_iCaptureProbabilityModifierChange = pClassInfo->getCaptureProbabilityModifierChange();
	if (getCaptureResistanceModifierChange() == iDefault) m_iCaptureResistanceModifierChange = pClassInfo->getCaptureResistanceModifierChange();

	if (getBreakdownChanceChange() == iDefault) m_iBreakdownChanceChange = pClassInfo->getBreakdownChanceChange();
	if (getBreakdownDamageChange() == iDefault) m_iBreakdownDamageChange = pClassInfo->getBreakdownDamageChange();
	if (getTauntChange() == iDefault) m_iTauntChange = pClassInfo->getTauntChange();
	if (getMaxHPChange() == iDefault) m_iMaxHPChange = pClassInfo->getMaxHPChange();
	if (getStrengthModifier() == iDefault) m_iStrengthModifier = pClassInfo->getStrengthModifier();
	if (getQualityChange() == iDefault) m_iQualityChange = pClassInfo->getQualityChange();
	if (getGroupChange() == iDefault) m_iGroupChange = pClassInfo->getGroupChange();
	if (getLevelPrereq() == iDefault) m_iLevelPrereq = pClassInfo->getLevelPrereq();
	if (getDamageModifierChange() == iDefault) m_iDamageModifierChange = pClassInfo->getDamageModifierChange();

	if (m_iUpkeepModifier == iDefault) m_iUpkeepModifier = pClassInfo->getUpkeepModifier();
	if (m_iExtraUpkeep100 == iDefault) m_iExtraUpkeep100 = pClassInfo->getExtraUpkeep100();

	if (getRBombardDamageChange() == iDefault) m_iRBombardDamageChange = pClassInfo->getRBombardDamageChange();
	if (getRBombardDamageLimitChange() == iDefault) m_iRBombardDamageLimitChange = pClassInfo->getRBombardDamageLimitChange();
	if (getRBombardDamageMaxUnitsChange() == iDefault) m_iRBombardDamageMaxUnitsChange = pClassInfo->getRBombardDamageMaxUnitsChange();
	if (getDCMBombRangeChange() == iDefault) m_iDCMBombRangeChange = pClassInfo->getDCMBombRangeChange();
	if (getDCMBombAccuracyChange() == iDefault) m_iDCMBombAccuracyChange = pClassInfo->getDCMBombAccuracyChange();
	if (m_iCombatModifierPerSizeMoreChange == iDefault) m_iCombatModifierPerSizeMoreChange = pClassInfo->m_iCombatModifierPerSizeMoreChange;
	if (m_iCombatModifierPerSizeLessChange == iDefault) m_iCombatModifierPerSizeLessChange = pClassInfo->m_iCombatModifierPerSizeLessChange;
	if (m_iCombatModifierPerVolumeMoreChange == iDefault) m_iCombatModifierPerVolumeMoreChange = pClassInfo->m_iCombatModifierPerVolumeMoreChange;
	if (m_iCombatModifierPerVolumeLessChange == iDefault) m_iCombatModifierPerVolumeLessChange = pClassInfo->m_iCombatModifierPerVolumeLessChange;
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
	if (m_iStealthStrikesChange == iDefault) m_iStealthStrikesChange = pClassInfo->m_iStealthStrikesChange;
	if (m_iStealthCombatModifierChange == iDefault) m_iStealthCombatModifierChange = pClassInfo->m_iStealthCombatModifierChange;
	if (m_iStealthDefenseChange == iDefault) m_iStealthDefenseChange = pClassInfo->m_iStealthDefenseChange;
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

	if (isParalyze() == bDefault) m_bParalyze = pClassInfo->isParalyze();
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
	if (m_bStarsign == false) m_bStarsign = pClassInfo->isStarsign();
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

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aeMapCategoryTypes, pClassInfo->getMapCategories());

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiPrereqBonusTypes, pClassInfo->m_aiPrereqBonusTypes);

	if (getNumAddsBuildTypes() == 0)
	{
		const int iNum = pClassInfo->getNumAddsBuildTypes();
		m_aiAddsBuildTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			GC.copyNonDefaultDelayedResolution((int*)&(m_aiAddsBuildTypes[i]), (int*)&(pClassInfo->m_aiAddsBuildTypes[i]));
		}
	}

	if (getNumNegatesInvisibilityTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiNegatesInvisibilityTypes, pClassInfo->m_aiNegatesInvisibilityTypes);
	}

	if (getNumPrereqTerrainTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiPrereqTerrainTypes, pClassInfo->m_aiPrereqTerrainTypes);
	}

	if (getNumPrereqFeatureTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiPrereqFeatureTypes, pClassInfo->m_aiPrereqFeatureTypes);
	}

	if (getNumPrereqImprovementTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiPrereqImprovementTypes, pClassInfo->m_aiPrereqImprovementTypes);
	}

	if (getNumPrereqPlotBonusTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiPrereqPlotBonusTypes, pClassInfo->m_aiPrereqPlotBonusTypes);
	}

	if (getNumPrereqLocalBuildingTypes() == 0)
	{
		const int iNum = pClassInfo->getNumPrereqLocalBuildingTypes();
		m_aiPrereqLocalBuildingTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			GC.copyNonDefaultDelayedResolution((int*)&(m_aiPrereqLocalBuildingTypes[i]), (int*)&(pClassInfo->m_aiPrereqLocalBuildingTypes[i]));
		}
	}

	if (getNumTrapSetWithPromotionTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiTrapSetWithPromotionTypes, pClassInfo->m_aiTrapSetWithPromotionTypes);
	}

	if (getNumTrapImmunityUnitCombatTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiTrapImmunityUnitCombatTypes, pClassInfo->m_aiTrapImmunityUnitCombatTypes);
	}


	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);

	if (getNumTargetUnitCombatTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiTargetUnitCombatTypes, pClassInfo->m_aiTargetUnitCombatTypes);
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

	//TB Combat Mods End  TB SubCombat Mods end

	if (isCanMovePeaks() == bDefault) m_bCanMovePeaks = pClassInfo->isCanMovePeaks();
	//	Koshling - enhanced mountaineering mode to differentiate between ability to move through
	//	mountains, and ability to lead a stack through mountains
	if (isCanLeadThroughPeaks() == bDefault) m_bCanLeadThroughPeaks = pClassInfo->isCanLeadThroughPeaks();

	if (getObsoleteTech() == NO_TECH) m_iObsoleteTech = pClassInfo->getObsoleteTech();
	if (getControlPoints() == iDefault) m_iControlPoints = pClassInfo->getControlPoints();
	if (getCommandRange() == iDefault) m_iCommandRange = pClassInfo->getCommandRange();
	if (isZoneOfControl() == bDefault) m_bZoneOfControl = pClassInfo->isZoneOfControl();
	if (getIgnoreTerrainDamage() == NO_TERRAIN) m_iIgnoreTerrainDamage = pClassInfo->getIgnoreTerrainDamage();

	//GC.copyNonDefaultDelayedResolutionVector(m_vPromotionOverwrites, pClassInfo->getPromotionOverwrites());

	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);
}


void CvPromotionInfo::getCheckSum(uint32_t& iSum) const
{
	PROFILE_EXTRA_FUNC();
	CvInfoUtil(this).checkSum(iSum);

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
	CheckSum(iSum, m_iPeaksWorkPercent);
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
	CheckSum(iSum, m_piTerrainWorkPercent, GC.getNumTerrainInfos());
	CheckSum(iSum, m_piFeatureWorkPercent, GC.getNumFeatureInfos());
	CheckSum(iSum, m_bCanMovePeaks);
	CheckSum(iSum, m_bCanLeadThroughPeaks);
	//CheckSumC(iSum, m_vPromotionOverwrites);
	CheckSum(iSum, m_iObsoleteTech);
	CheckSum(iSum, m_iControlPoints);
	CheckSum(iSum, m_iCommandRange);
	CheckSum(iSum, m_iIgnoreTerrainDamage);
	CheckSum(iSum, m_bZoneOfControl);
	m_PropertyManipulators.getCheckSum(iSum);
	CheckSum(iSum, m_bDefensiveVictoryMove);
	CheckSum(iSum, m_bFreeDrop);
	CheckSum(iSum, m_bOffensiveVictoryMove);
	CheckSum(iSum, m_bOneUp);
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
	CheckSum(iSum, m_iSurvivorChance);
	CheckSum(iSum, m_iVictoryAdjacentHeal);
	CheckSum(iSum, m_iVictoryHeal);
	CheckSum(iSum, m_iVictoryStackHeal);

	//TB Combat Mods Begin  TB SubCombat Mod begin
	// Textual References
	CheckSum(iSum, m_ePromotionLine);
	CheckSum(iSum, m_eReplacesUnitCombat);
	CheckSum(iSum, m_eDomainCargoChange);
	CheckSum(iSum, m_eSpecialCargoChange);
	CheckSum(iSum, m_eSpecialCargoPrereq);
	CheckSum(iSum, m_eSMNotSpecialCargoChange);
	CheckSum(iSum, m_eSMNotSpecialCargoPrereq);
	CheckSum(iSum, m_eSetSpecialUnit);
	//integers
	CheckSum(iSum, m_iAttackCombatModifierChange);
	CheckSum(iSum, m_iDefenseCombatModifierChange);
	CheckSum(iSum, m_iPursuitChange);
	CheckSum(iSum, m_iEarlyWithdrawChange);
	CheckSum(iSum, m_iVSBarbsChange);
	CheckSum(iSum, m_iUnnerveChange);
	CheckSum(iSum, m_iEncloseChange);
	CheckSum(iSum, m_iLungeChange);
	CheckSum(iSum, m_iDynamicDefenseChange);
	CheckSum(iSum, m_iStrengthChange);
	CheckSum(iSum, m_iLinePriority);
	CheckSum(iSum, m_iDamageperTurn);
	CheckSum(iSum, m_iStrAdjperTurn);
	CheckSum(iSum, m_iWeakenperTurn);
	CheckSum(iSum, m_iEnduranceChange);
	CheckSum(iSum, m_iPoisonProbabilityModifierChange);

	CheckSum(iSum, m_iCaptureProbabilityModifierChange);
	CheckSum(iSum, m_iCaptureResistanceModifierChange);

	CheckSum(iSum, m_iBreakdownChanceChange);
	CheckSum(iSum, m_iBreakdownDamageChange);
	CheckSum(iSum, m_iTauntChange);
	CheckSum(iSum, m_iMaxHPChange);
	CheckSum(iSum, m_iStrengthModifier);
	CheckSum(iSum, m_iQualityChange);
	CheckSum(iSum, m_iGroupChange);
	CheckSum(iSum, m_iLevelPrereq);
	CheckSum(iSum, m_iDamageModifierChange);

	CheckSum(iSum, m_iUpkeepModifier);
	CheckSum(iSum, m_iExtraUpkeep100);

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
	CheckSum(iSum, m_bParalyze);
	CheckSum(iSum, m_bAttackOnlyCitiesAdd);
	CheckSum(iSum, m_bAttackOnlyCitiesSubtract);
	CheckSum(iSum, m_bIgnoreNoEntryLevelAdd);
	CheckSum(iSum, m_bIgnoreNoEntryLevelSubtract);
	CheckSum(iSum, m_bIgnoreZoneofControlAdd);
	CheckSum(iSum, m_bIgnoreZoneofControlSubtract);
	CheckSum(iSum, m_bFliesToMoveAdd);
	CheckSum(iSum, m_bFliesToMoveSubtract);
	CheckSum(iSum, m_bNoSelfHeal);
	CheckSum(iSum, m_bSetOnHNCapture);
	CheckSum(iSum, m_bSetOnInvestigated);
	CheckSum(iSum, m_bStatus);
	CheckSum(iSum, m_bPrereqNormInvisible);
	CheckSum(iSum, m_bPlotPrereqsKeepAfter);
	CheckSum(iSum, m_bRemoveAfterSet);
	CheckSum(iSum, m_bQuick);
	CheckSum(iSum, m_bStarsign);
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
	CheckSumC(iSum, m_aeMapCategoryTypes);
	CheckSumC(iSum, m_aiPrereqBonusTypes);
	CheckSumC(iSum, m_aiAddsBuildTypes);
	CheckSumC(iSum, m_aiNegatesInvisibilityTypes);
	CheckSumC(iSum, m_aiPrereqTerrainTypes);
	CheckSumC(iSum, m_aiPrereqFeatureTypes);
	CheckSumC(iSum, m_aiPrereqImprovementTypes);
	CheckSumC(iSum, m_aiPrereqPlotBonusTypes);
	CheckSumC(iSum, m_aiPrereqLocalBuildingTypes);
	CheckSumC(iSum, m_aiTrapSetWithPromotionTypes);
	CheckSumC(iSum, m_aiTrapImmunityUnitCombatTypes);
	CheckSumC(iSum, m_aiTargetUnitCombatTypes);
	CheckSumC(iSum, m_aiCategories);
	// int vectors utilizing pairing without delayed resolution
	CheckSumC(iSum, m_aFlankingStrengthbyUnitCombatTypeChange);
	CheckSumC(iSum, m_aWithdrawOnTerrainTypesChange);
	CheckSumC(iSum, m_aWithdrawOnFeatureTypesChange);
	CheckSumC(iSum, m_aWithdrawVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aPursuitVSUnitCombatChangeTypes);
	CheckSumC(iSum, m_aTrapDisableUnitCombatTypes);
	CheckSumC(iSum, m_aTrapAvoidanceUnitCombatTypes);
	CheckSumC(iSum, m_aTrapTriggerUnitCombatTypes);
//Team Project (4)
		//WorkRateMod
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
	//TB Combat Mods End  TB SubCombat Mod end
}

