#include "CvGameCoreDLL.h"
#include "CvUnitCombatInfo.h"
#include "CvGameAI.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvUnitCombatInfo
//
//  DESC:   Contains unit combat types
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CvUnitCombatInfo::CvUnitCombatInfo()
	//Textual References
	: m_eReligion(NO_RELIGION)
	, m_eCulture(NO_BONUS)
	, m_eEra(NO_ERA)
	//Integers
	, m_iIgnoreTerrainDamage(0)
	, m_iVisibilityChange(0)
	, m_iMovesChange(0)
	, m_iMoveDiscountChange(0)
	, m_iAirRangeChange(0)
	, m_iInterceptChange(0)
	, m_iEvasionChange(0)
	, m_iWithdrawalChange(0)
	, m_iCargoChange(0)
	, m_iSMCargoChange(0)
	, m_iSMCargoVolumeChange(0)
	, m_iSMCargoVolumeModifierChange(0)
	, m_iCollateralDamageChange(0)
	, m_iBombardRateChange(0)
	, m_iFirstStrikesChange(0)
	, m_iChanceFirstStrikesChange(0)
	, m_iEnemyHealChange(0)
	, m_iNeutralHealChange(0)
	, m_iFriendlyHealChange(0)
	, m_iSameTileHealChange(0)
	, m_iAdjacentTileHealChange(0)
	, m_iCombatPercent(0)
	, m_iCityAttackPercent(0)
	, m_iCityDefensePercent(0)
	, m_iHillsAttackPercent(0)
	, m_iHillsDefensePercent(0)
	, m_iHillsWorkPercent(0)
	, m_iPeaksWorkPercent(0)
	, m_iWorkRatePercent(0)
	, m_iRevoltProtection(0)
	, m_iCollateralDamageProtection(0)
	, m_iPillageChange(0)
	, m_iUpgradeDiscount(0)
	, m_iExperiencePercent(0)
	, m_iKamikazePercent(0)
	, m_iAirCombatLimitChange(0)
	, m_iCelebrityHappy(0)
	, m_iCollateralDamageLimitChange(0)
	, m_iCollateralDamageMaxUnitsChange(0)
	, m_iCombatLimitChange(0)
	, m_iExtraDropRange(0)
	, m_iSurvivorChance(0)
	, m_iVictoryAdjacentHeal(0)
	, m_iVictoryHeal(0)
	, m_iVictoryStackHeal(0)
	, m_iAttackCombatModifierChange(0)
	, m_iDefenseCombatModifierChange(0)
	, m_iPursuitChange(0)
	, m_iEarlyWithdrawChange(0)
	, m_iVSBarbsChange(0)
	, m_iArmorChange(0)
	, m_iPunctureChange(0)
	, m_iOverrunChange(0)
	, m_iRepelChange(0)
	, m_iFortRepelChange(0)
	, m_iRepelRetriesChange(0)
	, m_iUnyieldingChange(0)
	, m_iKnockbackChange(0)
	, m_iKnockbackRetriesChange(0)
#ifdef BATTLEWORN
	, m_iStrAdjperAttChange(0)
	, m_iStrAdjperDefChange(0)
	, m_iWithdrawAdjperAttChange(0)
#endif // BATTLEWORN
	, m_iUnnerveChange(0)
	, m_iEncloseChange(0)
	, m_iLungeChange(0)
	, m_iDynamicDefenseChange(0)
	, m_iStrengthChange(0)
	, m_iFortitudeChange(0)
#ifdef STRENGTH_IN_NUMBERS
	, m_iFrontSupportPercentChange(0)
	, m_iShortRangeSupportPercentChange(0)
	, m_iMediumRangeSupportPercentChange(0)
	, m_iLongRangeSupportPercentChange(0)
	, m_iFlankSupportPercentChange(0)
#endif
	, m_iDodgeModifierChange(0)
	, m_iPrecisionModifierChange(0)
	, m_iPowerShotsChange(0)
	, m_iPowerShotCombatModifierChange(0)
	, m_iPowerShotPunctureModifierChange(0)
	, m_iPowerShotPrecisionModifierChange(0)
	, m_iPowerShotCriticalModifierChange(0)
	, m_iCriticalModifierChange(0)
	, m_iEnduranceChange(0)
	, m_iRoundStunProbChange(0)
	, m_iPoisonProbabilityModifierChange(0)
	, m_iCaptureProbabilityModifierChange(0)
	, m_iCaptureResistanceModifierChange(0)
	, m_iBreakdownChanceChange(0)
	, m_iBreakdownDamageChange(0)
	, m_iTauntChange(0)
	, m_iMaxHPChange(0)
	, m_iStrengthModifier(0)
	, m_iQualityBase(-10)
	, m_iGroupBase(-10)
	, m_iSizeBase(-10)
	, m_iDamageModifierChange(0)
	, m_iUpkeepModifier(0)
	, m_iExtraUpkeep100(0)
	, m_iRBombardDamageBase(0)
	, m_iRBombardDamageLimitBase(0)
	, m_iRBombardDamageMaxUnitsBase(0)
	, m_iDCMBombRangeBase(0)
	, m_iDCMBombAccuracyBase(0)
	, m_iCombatModifierPerSizeMoreChange(0)
	, m_iCombatModifierPerSizeLessChange(0)
	, m_iCombatModifierPerVolumeMoreChange(0)
	, m_iCombatModifierPerVolumeLessChange(0)
	, m_iSelfHealModifier(0)
	, m_iNumHealSupport(0)
	, m_iExcileChange(0)
	, m_iPassageChange(0)
	, m_iNoNonOwnedCityEntryChange(0)
	, m_iBarbCoExistChange(0)
	, m_iBlendIntoCityChange(0)
	, m_iInsidiousnessChange(0)
	, m_iInvestigationChange(0)
	, m_iStealthStrikesChange(0)
	, m_iStealthCombatModifierChange(0)
	, m_iStealthDefenseChange(0)
	, m_iDefenseOnlyChange(0)
	, m_iNoInvisibilityChange(0)
	, m_iNoCaptureChange(0)
	, m_iAnimalIgnoresBordersChange(0)
	, m_iNoDefensiveBonusChange(0)
	, m_iGatherHerdChange(0)
	, m_iReligiousCombatModifierChange(0)
	//Booleans
	, m_bDefensiveVictoryMove(false)
	, m_bFreeDrop(false)
	, m_bOffensiveVictoryMove(false)
	, m_bOneUp(false)
	, m_bPillageEspionage(false)
	, m_bPillageMarauder(false)
	, m_bPillageOnMove(false)
	, m_bPillageOnVictory(false)
	, m_bPillageResearch(false)
	, m_bBlitz(false)
	, m_bAmphib(false)
	, m_bRiver(false)
	, m_bEnemyRoute(false)
	, m_bAlwaysHeal(false)
	, m_bHillsDoubleMove(false)
	, m_bImmuneToFirstStrikes(false)
	, m_bStampedeChange(false)
	, m_bRemoveStampede(false)
	, m_bOnslaughtChange(false)
	, m_bMakesDamageCold(false)
	, m_bMakesDamageNotCold(false)
	, m_bAddsColdImmunity(false)
	, m_bRemovesColdImmunity(false)
	, m_bAttackOnlyCitiesAdd(false)
	, m_bAttackOnlyCitiesSubtract(false)
	, m_bIgnoreNoEntryLevelAdd(false)
	, m_bIgnoreNoEntryLevelSubtract(false)
	, m_bIgnoreZoneofControlAdd(false)
	, m_bIgnoreZoneofControlSubtract(false)
	, m_bFliesToMoveAdd(false)
	, m_bFliesToMoveSubtract(false)
	, m_bCanMovePeaks(false)
	, m_bCanLeadThroughPeaks(false)
	, m_bZoneOfControl(false)
	, m_bSpy(false)
	, m_bCannotMergeSplit(false)
	, m_bRBombardDirect(false)
	, m_bRBombardForceAbility(false)
	, m_bInvisible(false)
	, m_bForMilitary(false)
	, m_bForNavalMilitary(false)
	, m_bHealsAs(false)
	, m_bNoSelfHeal(false)
	, m_piDomainModifierPercent(NULL)
	, m_PropertyManipulators()
{
	m_zobristValue = GC.getGame().getSorenRand().getInt();
}

CvUnitCombatInfo::~CvUnitCombatInfo()
{
	PROFILE_EXTRA_FUNC();
	SAFE_DELETE_ARRAY(m_piDomainModifierPercent);

	foreach_(const CvOutcomeMission* outcomeMission, m_aOutcomeMissions)
	{
		SAFE_DELETE(outcomeMission);
	}

#ifdef OUTBREAKS_AND_AFFLICTIONS
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
#endif
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

#ifdef OUTBREAKS_AND_AFFLICTIONS
	for (int i=0; i<(int)m_aAfflictOnAttackChangeTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aAfflictOnAttackChangeTypes[i]));
	}
	for (int i=0; i<(int)m_aDistanceAttackCommunicabilityTypeChanges.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aDistanceAttackCommunicabilityTypeChanges[i]));
	}
#endif
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

int CvUnitCombatInfo::getPeaksWorkPercent() const
{
	return m_iPeaksWorkPercent;
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

int CvUnitCombatInfo::getPursuitChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_FIGHT_OR_FLIGHT))
	{
		return 0;
	}
	return m_iPursuitChange;
}

int CvUnitCombatInfo::getEarlyWithdrawChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_FIGHT_OR_FLIGHT))
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

int CvUnitCombatInfo::getOverrunChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iOverrunChange;
}

int CvUnitCombatInfo::getRepelChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iRepelChange;
}

int CvUnitCombatInfo::getFortRepelChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iFortRepelChange;
}

int CvUnitCombatInfo::getRepelRetriesChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iRepelRetriesChange;
}

int CvUnitCombatInfo::getUnyieldingChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iUnyieldingChange;
}

int CvUnitCombatInfo::getKnockbackChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iKnockbackChange;
}

int CvUnitCombatInfo::getKnockbackRetriesChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iKnockbackRetriesChange;
}

#ifdef BATTLEWORN
int CvUnitCombatInfo::getStrAdjperAttChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_BATTLEWORN))
	{
		return 0;
	}
	return m_iStrAdjperAttChange;
}

int CvUnitCombatInfo::getStrAdjperDefChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_BATTLEWORN))
	{
		return 0;
	}
	return m_iStrAdjperDefChange;
}

int CvUnitCombatInfo::getWithdrawAdjperAttChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_BATTLEWORN))
	{
		return 0;
	}
	return m_iWithdrawAdjperAttChange;
}
#endif

int CvUnitCombatInfo::getUnnerveChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SURROUND_DESTROY))
	{
		return 0;
	}
	return m_iUnnerveChange;
}

int CvUnitCombatInfo::getEncloseChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SURROUND_DESTROY))
	{
		return 0;
	}
	return m_iEncloseChange;
}

int CvUnitCombatInfo::getLungeChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SURROUND_DESTROY))
	{
		return 0;
	}
	return m_iLungeChange;
}

int CvUnitCombatInfo::getDynamicDefenseChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SURROUND_DESTROY))
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

#ifdef STRENGTH_IN_NUMBERS
int CvUnitCombatInfo::getFrontSupportPercentChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_iFrontSupportPercentChange;
}

int CvUnitCombatInfo::getShortRangeSupportPercentChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_iShortRangeSupportPercentChange;
}

int CvUnitCombatInfo::getMediumRangeSupportPercentChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_iMediumRangeSupportPercentChange;
}

int CvUnitCombatInfo::getLongRangeSupportPercentChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_iLongRangeSupportPercentChange;
}

int CvUnitCombatInfo::getFlankSupportPercentChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_STRENGTH_IN_NUMBERS))
	{
		return 0;
	}
	return m_iFlankSupportPercentChange;
}
#endif

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

int CvUnitCombatInfo::getMaxHPChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
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

int CvUnitCombatInfo::getUpkeepModifier() const
{
	return m_iUpkeepModifier;
}

int CvUnitCombatInfo::getExtraUpkeep100() const
{
	return m_iExtraUpkeep100;
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

int CvUnitCombatInfo::getCombatModifierPerSizeMoreChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerSizeMoreChange;
}

int CvUnitCombatInfo::getCombatModifierPerSizeLessChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerSizeLessChange;
}

int CvUnitCombatInfo::getCombatModifierPerVolumeMoreChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
	{
		return 0;
	}
	return m_iCombatModifierPerVolumeMoreChange;
}

int CvUnitCombatInfo::getCombatModifierPerVolumeLessChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
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

int CvUnitCombatInfo::getStealthStrikesChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_WITHOUT_WARNING))
	{
		return 0;
	}
	return m_iStealthStrikesChange;
}

int CvUnitCombatInfo::getStealthCombatModifierChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_WITHOUT_WARNING))
	{
		return 0;
	}
	return m_iStealthCombatModifierChange;
}

int CvUnitCombatInfo::getStealthDefenseChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_WITHOUT_WARNING))
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
	FASSERT_BOUNDS(0, NUM_DOMAIN_TYPES, i);
	return m_piDomainModifierPercent ? m_piDomainModifierPercent[i] : 0;
}

bool CvUnitCombatInfo::isAnyDomainModifierPercent() const
{
	return m_piDomainModifierPercent != NULL;
}

#ifdef OUTBREAKS_AND_AFFLICTIONS
int CvUnitCombatInfo::getCureAfflictionChangeType(int i) const
{
	return m_aiCureAfflictionChangeTypes[i];
}

int CvUnitCombatInfo::getNumCureAfflictionChangeTypes() const
{
	return (int)m_aiCureAfflictionChangeTypes.size();
}

bool CvUnitCombatInfo::isCureAfflictionChangeType(int i) const
{
	return algo::any_of_equal(m_aiCureAfflictionChangeTypes, i);
}
#endif // OUTBREAKS_AND_AFFLICTIONS

int CvUnitCombatInfo::getTerrainIgnoreDamageChangeType(int i) const
{
	return m_aiTerrainIgnoreDamageChangeTypes[i];
}

int CvUnitCombatInfo::getNumTerrainIgnoreDamageChangeTypes() const
{
	return (int)m_aiTerrainIgnoreDamageChangeTypes.size();
}

bool CvUnitCombatInfo::isTerrainIgnoreDamageChangeType(int i) const
{
	return algo::any_of_equal(m_aiTerrainIgnoreDamageChangeTypes, i);
}

int CvUnitCombatInfo::getTerrainDoubleMoveChangeType(int i) const
{
	return m_aiTerrainDoubleMoveChangeTypes[i];
}

int CvUnitCombatInfo::getNumTerrainDoubleMoveChangeTypes() const
{
	return (int)m_aiTerrainDoubleMoveChangeTypes.size();
}

bool CvUnitCombatInfo::isTerrainDoubleMoveChangeType(int i) const
{
	return algo::any_of_equal(m_aiTerrainDoubleMoveChangeTypes, i);
}

int CvUnitCombatInfo::getFeatureDoubleMoveChangeType(int i) const
{
	return m_aiFeatureDoubleMoveChangeTypes[i];
}

int CvUnitCombatInfo::getNumFeatureDoubleMoveChangeTypes() const
{
	return (int)m_aiFeatureDoubleMoveChangeTypes.size();
}

bool CvUnitCombatInfo::isFeatureDoubleMoveChangeType(int i) const
{
	return algo::any_of_equal(m_aiFeatureDoubleMoveChangeTypes, i);
}
int CvUnitCombatInfo::getOnGameOption(int i) const
{
	return m_aiOnGameOptions[i];
}

int CvUnitCombatInfo::getNumOnGameOptions() const
{
	return (int)m_aiOnGameOptions.size();
}

bool CvUnitCombatInfo::isOnGameOption(int i) const
{
	return algo::any_of_equal(m_aiOnGameOptions, i);
}

int CvUnitCombatInfo::getNotOnGameOption(int i) const
{
	return m_aiNotOnGameOptions[i];
}

int CvUnitCombatInfo::getNumNotOnGameOptions() const
{
	return (int)m_aiNotOnGameOptions.size();
}

bool CvUnitCombatInfo::isNotOnGameOption(int i) const
{
	return algo::any_of_equal(m_aiNotOnGameOptions, i);
}

int CvUnitCombatInfo::getGGptsforUnitType(int i) const
{
	return m_aiGGptsforUnitTypes[i];
}

int CvUnitCombatInfo::getNumGGptsforUnitTypes() const
{
	return (int)m_aiGGptsforUnitTypes.size();
}

bool CvUnitCombatInfo::isGGptsforUnitType(int i) const
{
	return algo::any_of_equal(m_aiGGptsforUnitTypes, i);
}

int CvUnitCombatInfo::getDefaultStatusType(int i) const
{
	return m_aiDefaultStatusTypes[i];
}

int CvUnitCombatInfo::getNumDefaultStatusTypes() const
{
	return (int)m_aiDefaultStatusTypes.size();
}

bool CvUnitCombatInfo::isDefaultStatusType(int i) const
{
	return algo::any_of_equal(m_aiDefaultStatusTypes, i);
}

int CvUnitCombatInfo::getTrapImmunityUnitCombatType(int i) const
{
	return m_aiTrapImmunityUnitCombatTypes[i];
}

int CvUnitCombatInfo::getNumTrapImmunityUnitCombatTypes() const
{
	return (int)m_aiTrapImmunityUnitCombatTypes.size();
}

bool CvUnitCombatInfo::isTrapImmunityUnitCombatType(int i) const
{
	return algo::any_of_equal(m_aiTrapImmunityUnitCombatTypes, i);
}

int CvUnitCombatInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}

int CvUnitCombatInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}

bool CvUnitCombatInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}

// int vector utilizing pairing without delayed resolution

int CvUnitCombatInfo::getNumWithdrawOnTerrainTypeChanges() const
{
	return m_aWithdrawOnTerrainTypesChange.size();
}

int CvUnitCombatInfo::getWithdrawOnTerrainTypeChange(int iTerrain) const
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

int CvUnitCombatInfo::getNumWithdrawOnFeatureTypeChanges() const
{
	return m_aWithdrawOnFeatureTypesChange.size();
}

int CvUnitCombatInfo::getWithdrawOnFeatureTypeChange(int iFeature) const
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

int CvUnitCombatInfo::getNumVisibilityIntensityChangeTypes() const
{
	return m_aVisibilityIntensityChangeTypes.size();
}

int CvUnitCombatInfo::getVisibilityIntensityChangeType(int iInvisibility) const
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

bool CvUnitCombatInfo::isVisibilityIntensityChangeType(int iInvisibility) const
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

int CvUnitCombatInfo::getNumInvisibilityIntensityChangeTypes() const
{
	return m_aInvisibilityIntensityChangeTypes.size();
}

int CvUnitCombatInfo::getInvisibilityIntensityChangeType(int iInvisibility) const
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

bool CvUnitCombatInfo::isInvisibilityIntensityChangeType(int iInvisibility) const
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

int CvUnitCombatInfo::getNumVisibilityIntensityRangeChangeTypes() const
{
	return m_aVisibilityIntensityRangeChangeTypes.size();
}

int CvUnitCombatInfo::getVisibilityIntensityRangeChangeType(int iInvisibility) const
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

bool CvUnitCombatInfo::isVisibilityIntensityRangeChangeType(int iInvisibility) const
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

int CvUnitCombatInfo::getNumVisibilityIntensitySameTileChangeTypes() const
{
	return m_aVisibilityIntensitySameTileChangeTypes.size();
}

int CvUnitCombatInfo::getVisibilityIntensitySameTileChangeType(int iInvisibility) const
{
	PROFILE_EXTRA_FUNC();
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensitySameTileChangeTypes.begin(); it != m_aVisibilityIntensitySameTileChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitCombatInfo::isVisibilityIntensitySameTileChangeType(int iInvisibility) const
{
	PROFILE_EXTRA_FUNC();
	for (InvisibilityArray::const_iterator it = m_aVisibilityIntensitySameTileChangeTypes.begin(); it != m_aVisibilityIntensitySameTileChangeTypes.end(); ++it)
	{
		if ((*it).first == (InvisibleTypes)iInvisibility)
		{
			return true;
		}
	}
	return false;
}

#ifdef OUTBREAKS_AND_AFFLICTIONS
int CvUnitCombatInfo::getNumAidChanges() const
{
	return m_aAidChanges.size();
}

int CvUnitCombatInfo::getAidChange(int iProperty) const
{
	PROFILE_EXTRA_FUNC();
	for (AidArray::const_iterator it = m_aAidChanges.begin(); it != m_aAidChanges.end(); ++it)
	{
		if ((*it).first == (PropertyTypes)iProperty)
		{
			return (*it).second;
		}
	}
	return 0;
}

bool CvUnitCombatInfo::isAidChange(int iProperty) const
{
	PROFILE_EXTRA_FUNC();
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

const PromotionLineModifier& CvUnitCombatInfo::getAfflictionFortitudeChangeModifier(int iAfflictionLine) const
{
	FASSERT_BOUNDS(0, getNumAfflictionFortitudeChangeModifiers(), iAfflictionLine);
	return m_aAfflictionFortitudeChangeModifiers[iAfflictionLine];
}
#endif

int CvUnitCombatInfo::getNumTerrainAttackChangeModifiers() const
{
	return (int)m_aTerrainAttackChangeModifiers.size();
}

const TerrainModifier& CvUnitCombatInfo::getTerrainAttackChangeModifier(int iTerrain) const
{
	FASSERT_BOUNDS(0, getNumTerrainAttackChangeModifiers(), iTerrain);
	return m_aTerrainAttackChangeModifiers[iTerrain];
}

int CvUnitCombatInfo::getNumTerrainDefenseChangeModifiers() const
{
	return (int)m_aTerrainDefenseChangeModifiers.size();
}

const TerrainModifier& CvUnitCombatInfo::getTerrainDefenseChangeModifier(int iTerrain) const
{
	FASSERT_BOUNDS(0, getNumTerrainDefenseChangeModifiers(), iTerrain);
	return m_aTerrainDefenseChangeModifiers[iTerrain];
}

int CvUnitCombatInfo::getNumTerrainWorkChangeModifiers() const
{
	return (int)m_aTerrainWorkChangeModifiers.size();
}

const TerrainModifier& CvUnitCombatInfo::getTerrainWorkChangeModifier(int iTerrain) const
{
	FASSERT_BOUNDS(0, getNumTerrainWorkChangeModifiers(), iTerrain);
	return m_aTerrainWorkChangeModifiers[iTerrain];
}

int CvUnitCombatInfo::getNumBuildWorkChangeModifiers() const
{
	return (int)m_aBuildWorkChangeModifiers.size();
}

const BuildModifier& CvUnitCombatInfo::getBuildWorkChangeModifier(int iBuild) const
{
	FASSERT_BOUNDS(0, getNumBuildWorkChangeModifiers(), iBuild);
	return m_aBuildWorkChangeModifiers[iBuild];
}

int CvUnitCombatInfo::getNumFeatureAttackChangeModifiers() const
{
	return (int)m_aFeatureAttackChangeModifiers.size();
}

const FeatureModifier& CvUnitCombatInfo::getFeatureAttackChangeModifier(int iFeature) const
{
	FASSERT_BOUNDS(0, getNumFeatureAttackChangeModifiers(), iFeature);
	return m_aFeatureAttackChangeModifiers[iFeature];
}

int CvUnitCombatInfo::getNumFeatureDefenseChangeModifiers() const
{
	return (int)m_aFeatureDefenseChangeModifiers.size();
}

const FeatureModifier& CvUnitCombatInfo::getFeatureDefenseChangeModifier(int iFeature) const
{
	FASSERT_BOUNDS(0, getNumFeatureDefenseChangeModifiers(), iFeature);
	return m_aFeatureDefenseChangeModifiers[iFeature];
}

int CvUnitCombatInfo::getNumFeatureWorkChangeModifiers() const
{
	return (int)m_aFeatureWorkChangeModifiers.size();
}

const FeatureModifier& CvUnitCombatInfo::getFeatureWorkChangeModifier(int iFeature) const
{
	FASSERT_BOUNDS(0, getNumFeatureWorkChangeModifiers(), iFeature);
	return m_aFeatureWorkChangeModifiers[iFeature];
}

int CvUnitCombatInfo::getNumUnitCombatChangeModifiers() const
{
	return (int)m_aUnitCombatChangeModifiers.size();
}

const UnitCombatModifier& CvUnitCombatInfo::getUnitCombatChangeModifier(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, getNumUnitCombatChangeModifiers(), iUnitCombat);
	return m_aUnitCombatChangeModifiers[iUnitCombat];
}

int CvUnitCombatInfo::getNumFlankingStrengthbyUnitCombatTypesChange() const
{
	return (int)m_aFlankingStrengthbyUnitCombatTypeChange.size();
}

const UnitCombatModifier& CvUnitCombatInfo::getFlankingStrengthbyUnitCombatTypeChange(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, getNumFlankingStrengthbyUnitCombatTypesChange(), iUnitCombat);
	return m_aFlankingStrengthbyUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumWithdrawVSUnitCombatTypesChange() const
{
	return (int)m_aWithdrawVSUnitCombatTypeChange.size();
}

const UnitCombatModifier& CvUnitCombatInfo::getWithdrawVSUnitCombatTypeChange(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, getNumWithdrawVSUnitCombatTypesChange(), iUnitCombat);
	return m_aWithdrawVSUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumPursuitVSUnitCombatTypesChange() const
{
	return (int)m_aPursuitVSUnitCombatTypeChange.size();
}

const UnitCombatModifier& CvUnitCombatInfo::getPursuitVSUnitCombatTypeChange(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, getNumPursuitVSUnitCombatTypesChange(), iUnitCombat);
	//Note: iUnitCombat is just an index.  It is NOT the UnitCombat ID#.
	return m_aPursuitVSUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumRepelVSUnitCombatTypesChange() const
{
	return (int)m_aRepelVSUnitCombatTypeChange.size();
}

const UnitCombatModifier& CvUnitCombatInfo::getRepelVSUnitCombatTypeChange(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, getNumRepelVSUnitCombatTypesChange(), iUnitCombat);
	return m_aRepelVSUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumKnockbackVSUnitCombatTypesChange() const
{
	return (int)m_aKnockbackVSUnitCombatTypeChange.size();
}

const UnitCombatModifier& CvUnitCombatInfo::getKnockbackVSUnitCombatTypeChange(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, getNumKnockbackVSUnitCombatTypesChange(), iUnitCombat);
	return m_aKnockbackVSUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumPunctureVSUnitCombatTypesChange() const
{
	return (int)m_aPunctureVSUnitCombatTypeChange.size();
}

const UnitCombatModifier& CvUnitCombatInfo::getPunctureVSUnitCombatTypeChange(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, getNumPunctureVSUnitCombatTypesChange(), iUnitCombat);
	return m_aPunctureVSUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumArmorVSUnitCombatTypesChange() const
{
	return (int)m_aArmorVSUnitCombatTypeChange.size();
}

const UnitCombatModifier& CvUnitCombatInfo::getArmorVSUnitCombatTypeChange(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, getNumArmorVSUnitCombatTypesChange(), iUnitCombat);
	return m_aArmorVSUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumDodgeVSUnitCombatTypesChange() const
{
	return (int)m_aDodgeVSUnitCombatTypeChange.size();
}

const UnitCombatModifier& CvUnitCombatInfo::getDodgeVSUnitCombatTypeChange(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, getNumDodgeVSUnitCombatTypesChange(), iUnitCombat);
	return m_aDodgeVSUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumPrecisionVSUnitCombatTypesChange() const
{
	return (int)m_aPrecisionVSUnitCombatTypeChange.size();
}

const UnitCombatModifier& CvUnitCombatInfo::getPrecisionVSUnitCombatTypeChange(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, getNumPrecisionVSUnitCombatTypesChange(), iUnitCombat);
	return m_aPrecisionVSUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumCriticalVSUnitCombatTypesChange() const
{
	return (int)m_aCriticalVSUnitCombatTypeChange.size();
}

const UnitCombatModifier& CvUnitCombatInfo::getCriticalVSUnitCombatTypeChange(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, getNumCriticalVSUnitCombatTypesChange(), iUnitCombat);
	return m_aCriticalVSUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumRoundStunVSUnitCombatTypesChange() const
{
	return (int)m_aRoundStunVSUnitCombatTypeChange.size();
}

const UnitCombatModifier& CvUnitCombatInfo::getRoundStunVSUnitCombatTypeChange(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, getNumRoundStunVSUnitCombatTypesChange(), iUnitCombat);
	return m_aRoundStunVSUnitCombatTypeChange[iUnitCombat];
}

int CvUnitCombatInfo::getNumTrapAvoidanceUnitCombatTypes() const
{
	return (int)m_aTrapAvoidanceUnitCombatTypes.size();
}

const UnitCombatModifier& CvUnitCombatInfo::getTrapAvoidanceUnitCombatType(int iIndex) const
{
	return m_aTrapAvoidanceUnitCombatTypes[iIndex];
}

#ifdef OUTBREAKS_AND_AFFLICTIONS
int CvUnitCombatInfo::getNumAfflictOnAttackChangeTypes() const
{
	return (int)m_aAfflictOnAttackChangeTypes.size();
}

const AfflictOnAttackChange& CvUnitCombatInfo::getAfflictOnAttackChangeType(int iAfflictionLine) const
{
	FASSERT_BOUNDS(0, getNumAfflictOnAttackChangeTypes(), iAfflictionLine);
	return m_aAfflictOnAttackChangeTypes[iAfflictionLine];
}
#endif

int CvUnitCombatInfo::getNumInvisibleTerrainChanges() const
{
	return (int)m_aInvisibleTerrainChanges.size();
}

const InvisibleTerrainChanges& CvUnitCombatInfo::getInvisibleTerrainChange(int iIndex) const
{
	return m_aInvisibleTerrainChanges[iIndex];
}

int CvUnitCombatInfo::getNumInvisibleFeatureChanges() const
{
	return (int)m_aInvisibleFeatureChanges.size();
}

const InvisibleFeatureChanges& CvUnitCombatInfo::getInvisibleFeatureChange(int iIndex) const
{
	return m_aInvisibleFeatureChanges[iIndex];
}

int CvUnitCombatInfo::getNumInvisibleImprovementChanges() const
{
	return (int)m_aInvisibleImprovementChanges.size();
}

const InvisibleImprovementChanges& CvUnitCombatInfo::getInvisibleImprovementChange(int iIndex) const
{
	return m_aInvisibleImprovementChanges[iIndex];
}

int CvUnitCombatInfo::getNumVisibleTerrainChanges() const
{
	return (int)m_aVisibleTerrainChanges.size();
}

const InvisibleTerrainChanges& CvUnitCombatInfo::getVisibleTerrainChange(int iIndex) const
{
	return m_aVisibleTerrainChanges[iIndex];
}

int CvUnitCombatInfo::getNumVisibleFeatureChanges() const
{
	return (int)m_aVisibleFeatureChanges.size();
}

const InvisibleFeatureChanges& CvUnitCombatInfo::getVisibleFeatureChange(int iIndex) const
{
	return m_aVisibleFeatureChanges[iIndex];
}

int CvUnitCombatInfo::getNumVisibleImprovementChanges() const
{
	return (int)m_aVisibleImprovementChanges.size();
}

const InvisibleImprovementChanges& CvUnitCombatInfo::getVisibleImprovementChange(int iIndex) const
{
	return m_aVisibleImprovementChanges[iIndex];
}

int CvUnitCombatInfo::getNumVisibleTerrainRangeChanges() const
{
	return (int)m_aVisibleTerrainRangeChanges.size();
}

const InvisibleTerrainChanges& CvUnitCombatInfo::getVisibleTerrainRangeChange(int iIndex) const
{
	return m_aVisibleTerrainRangeChanges[iIndex];
}

int CvUnitCombatInfo::getNumVisibleFeatureRangeChanges() const
{
	return (int)m_aVisibleFeatureRangeChanges.size();
}

const InvisibleFeatureChanges& CvUnitCombatInfo::getVisibleFeatureRangeChange(int iIndex) const
{
	return m_aVisibleFeatureRangeChanges[iIndex];
}

int CvUnitCombatInfo::getNumVisibleImprovementRangeChanges() const
{
	return (int)m_aVisibleImprovementRangeChanges.size();
}

const InvisibleImprovementChanges& CvUnitCombatInfo::getVisibleImprovementRangeChange(int iIndex) const
{
	return m_aVisibleImprovementRangeChanges[iIndex];
}
#ifdef OUTBREAKS_AND_AFFLICTIONS
int CvUnitCombatInfo::getNumDistanceAttackCommunicabilityTypeChanges() const
{
	return (int)m_aDistanceAttackCommunicabilityTypeChanges.size();
}

const AfflictionLineChanges& CvUnitCombatInfo::getDistanceAttackCommunicabilityTypeChange(int iIndex) const
{
	return m_aDistanceAttackCommunicabilityTypeChanges[iIndex];
}
#endif

bool CvUnitCombatInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	CvString szTextVal2;

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	m_KillOutcomeList.read(pXML, L"KillOutcomes");

	if(pXML->TryMoveToXmlFirstChild(L"Actions"))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"Action"))
			{
				do
				{
					CvOutcomeMission* pOutcomeMission = new CvOutcomeMission();
					pOutcomeMission->read(pXML);
					m_aOutcomeMissions.push_back(pOutcomeMission);
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
#ifdef BATTLEWORN
	pXML->GetOptionalChildXmlValByName(&m_iStrAdjperAttChange, L"iStrAdjperAttChange");
	pXML->GetOptionalChildXmlValByName(&m_iStrAdjperDefChange, L"iStrAdjperDefChange");
	pXML->GetOptionalChildXmlValByName(&m_iWithdrawAdjperAttChange, L"iWithdrawAdjperAttChange");
#endif
	pXML->GetOptionalChildXmlValByName(&m_iUnnerveChange, L"iUnnerveChange");
	pXML->GetOptionalChildXmlValByName(&m_iEncloseChange, L"iEncloseChange");
	pXML->GetOptionalChildXmlValByName(&m_iLungeChange, L"iLungeChange");
	pXML->GetOptionalChildXmlValByName(&m_iDynamicDefenseChange, L"iDynamicDefenseChange");
	pXML->GetOptionalChildXmlValByName(&m_iStrengthChange, L"iStrengthChange");
	pXML->GetOptionalChildXmlValByName(&m_iFortitudeChange, L"iFortitudeChange");
#ifdef STRENGTH_IN_NUMBERS
	pXML->GetOptionalChildXmlValByName(&m_iFrontSupportPercentChange, L"iFrontSupportPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iShortRangeSupportPercentChange, L"iShortRangeSupportPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iMediumRangeSupportPercentChange, L"iMediumRangeSupportPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iLongRangeSupportPercentChange, L"iLongRangeSupportPercentChange");
	pXML->GetOptionalChildXmlValByName(&m_iFlankSupportPercentChange, L"iFlankSupportPercentChange");
#endif
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
	pXML->GetOptionalChildXmlValByName(&m_iBreakdownChanceChange, L"iBreakdownChanceChange");
	pXML->GetOptionalChildXmlValByName(&m_iBreakdownDamageChange, L"iBreakdownDamageChange");
	pXML->GetOptionalChildXmlValByName(&m_iTauntChange, L"iTauntChange");
	pXML->GetOptionalChildXmlValByName(&m_iMaxHPChange, L"iMaxHPChange");
	pXML->GetOptionalChildXmlValByName(&m_iStrengthModifier, L"iStrengthModifier");
	pXML->GetOptionalChildXmlValByName(&m_iQualityBase, L"iQualityBase", -10);
	pXML->GetOptionalChildXmlValByName(&m_iGroupBase, L"iGroupBase", -10);
	pXML->GetOptionalChildXmlValByName(&m_iSizeBase, L"iSizeBase", -10);
	pXML->GetOptionalChildXmlValByName(&m_iDamageModifierChange, L"iDamageModifierChange");

	pXML->GetOptionalChildXmlValByName(&m_iUpkeepModifier, L"iUpkeepModifier");
	pXML->GetOptionalChildXmlValByName(&m_iExtraUpkeep100, L"iExtraUpkeep100");

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
#ifdef OUTBREAKS_AND_AFFLICTIONS
	pXML->SetOptionalVectorWithDelayedResolution(m_aiCureAfflictionChangeTypes, L"CureAfflictionChangeTypes");
#endif
	pXML->SetOptionalVector(&m_aiTerrainIgnoreDamageChangeTypes, L"TerrainIgnoreDamageChangeTypes");
	pXML->SetOptionalVector(&m_aiTerrainDoubleMoveChangeTypes, L"TerrainDoubleMoveChangeTypes");
	pXML->SetOptionalVector(&m_aiFeatureDoubleMoveChangeTypes, L"FeatureDoubleMoveChangeTypes");
	pXML->SetOptionalVector(&m_aiOnGameOptions, L"OnGameOptions");
	pXML->SetOptionalVector(&m_aiNotOnGameOptions, L"NotOnGameOptions");
	pXML->SetOptionalVectorWithDelayedResolution(m_aiGGptsforUnitTypes, L"GGptsforUnitTypes");
	pXML->SetOptionalVectorWithDelayedResolution(m_aiDefaultStatusTypes, L"DefaultStatusTypes");
	pXML->SetOptionalVector(&m_aiTrapImmunityUnitCombatTypes, L"TrapImmunityUnitCombatTypes");
	pXML->SetOptionalVector(&m_aiCategories, L"Categories");

	// int vector utilizing pairing without delayed resolution
	m_aWithdrawOnTerrainTypesChange.clear();
	if (pXML->TryMoveToXmlFirstChild(L"WithdrawModifierOnTerrainTypeChanges"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						int iModifier;
						pXML->GetNextXmlVal(&iModifier);
						m_aWithdrawOnTerrainTypesChange.push_back(std::make_pair((TerrainTypes)pXML->GetInfoClass(szTextVal), iModifier));

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
		const int iNumSibs = pXML->GetXmlChildrenNumber();

		if (0 < iNumSibs)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumSibs; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						int iModifier;
						pXML->GetNextXmlVal(&iModifier);
						m_aWithdrawOnFeatureTypesChange.push_back(std::make_pair((FeatureTypes)pXML->GetInfoClass(szTextVal), iModifier));

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
		const int iNumSibs = pXML->GetXmlChildrenNumber();

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
		const int iNumSibs = pXML->GetXmlChildrenNumber();

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
		const int iNumSibs = pXML->GetXmlChildrenNumber();

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
		const int iNumSibs = pXML->GetXmlChildrenNumber();

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

#ifdef OUTBREAKS_AND_AFFLICTIONS
	pXML->SetOptionalPairVector<AidArray, PropertyTypes, int>(&m_aAidChanges, L"AidChanges");

	// int vector utilizing struct with delayed resolution
	if(pXML->TryMoveToXmlFirstChild(L"AfflictionFortitudeChangeModifiers"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"AfflictionFortitudeChangeModifier" );
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
#endif
	if(pXML->TryMoveToXmlFirstChild(L"TerrainAttackChangeModifiers"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"TerrainAttackChangeModifier" );
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
		const int iNum = pXML->GetXmlChildrenNumber(L"TerrainDefenseChangeModifier" );
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
		const int iNum = pXML->GetXmlChildrenNumber(L"TerrainWorkChangeModifier" );
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
		const int iNum = pXML->GetXmlChildrenNumber(L"BuildWorkChangeModifier" );
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
		const int iNum = pXML->GetXmlChildrenNumber(L"FeatureAttackChangeModifier" );
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
		const int iNum = pXML->GetXmlChildrenNumber(L"FeatureDefenseChangeModifier" );
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
		const int iNum = pXML->GetXmlChildrenNumber(L"FeatureWorkChangeModifier" );
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
		const int iNum = pXML->GetXmlChildrenNumber(L"UnitCombatChangeModifier" );
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
		const int iNum = pXML->GetXmlChildrenNumber(L"FlankingStrengthbyUnitCombatTypesChange" );
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
		const int iNum = pXML->GetXmlChildrenNumber(L"WithdrawVSUnitCombatTypesChange" );
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
		const int iNum = pXML->GetXmlChildrenNumber(L"PursuitVSUnitCombatTypesChange" );
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
		const int iNum = pXML->GetXmlChildrenNumber(L"RepelVSUnitCombatTypesChange" );
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
		const int iNum = pXML->GetXmlChildrenNumber(L"KnockbackVSUnitCombatTypesChange" );
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
		const int iNum = pXML->GetXmlChildrenNumber(L"PunctureVSUnitCombatTypesChange" );
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
		const int iNum = pXML->GetXmlChildrenNumber(L"ArmorVSUnitCombatTypesChange" );
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
		const int iNum = pXML->GetXmlChildrenNumber(L"DodgeVSUnitCombatTypesChange" );
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
		const int iNum = pXML->GetXmlChildrenNumber(L"PrecisionVSUnitCombatTypesChange" );
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
		const int iNum = pXML->GetXmlChildrenNumber(L"CriticalVSUnitCombatTypesChange" );
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
		const int iNum = pXML->GetXmlChildrenNumber(L"RoundStunVSUnitCombatTypesChange" );
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
		const int iNum = pXML->GetXmlChildrenNumber(L"TrapAvoidanceUnitCombatType" );
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

#ifdef OUTBREAKS_AND_AFFLICTIONS
	if(pXML->TryMoveToXmlFirstChild(L"AfflictOnAttackChangeTypes"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"AfflictOnAttackChangeType" );
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
#endif // OUTBREAKS_AND_AFFLICTIONS

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
#ifdef OUTBREAKS_AND_AFFLICTIONS
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
#endif
	m_PropertyManipulators.read(pXML);

	return true;
}
void CvUnitCombatInfo::copyNonDefaults(CvUnitCombatInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const bool bDefault = false;
	const int iDefault = 0;
	const CvString cDefault = CvString::format("").GetCString();
	const CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	m_KillOutcomeList.copyNonDefaults(&pClassInfo->m_KillOutcomeList);

	if (m_aOutcomeMissions.empty())
	{
		const int num = (int) pClassInfo->getNumActionOutcomes();
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
	if (m_iPursuitChange == iDefault) m_iPursuitChange = pClassInfo->m_iPursuitChange;
	if (m_iEarlyWithdrawChange == iDefault) m_iEarlyWithdrawChange = pClassInfo->m_iEarlyWithdrawChange;
	if (getVSBarbsChange() == iDefault) m_iVSBarbsChange = pClassInfo->getVSBarbsChange();
	if (getArmorChange() == iDefault) m_iArmorChange = pClassInfo->getArmorChange();
	if (getPunctureChange() == iDefault) m_iPunctureChange = pClassInfo->getPunctureChange();
	if (m_iOverrunChange == iDefault) m_iOverrunChange = pClassInfo->m_iOverrunChange;
	if (m_iRepelChange == iDefault) m_iRepelChange = pClassInfo->m_iRepelChange;
	if (m_iFortRepelChange == iDefault) m_iFortRepelChange = pClassInfo->m_iFortRepelChange;
	if (m_iRepelRetriesChange == iDefault) m_iRepelRetriesChange = pClassInfo->m_iRepelRetriesChange;
	if (m_iUnyieldingChange == iDefault) m_iUnyieldingChange = pClassInfo->m_iUnyieldingChange;
	if (m_iKnockbackChange == iDefault) m_iKnockbackChange = pClassInfo->m_iKnockbackChange;
	if (m_iKnockbackRetriesChange == iDefault) m_iKnockbackRetriesChange = pClassInfo->m_iKnockbackRetriesChange;
#ifdef BATTLEWORN
	if (m_iStrAdjperAttChange == iDefault) m_iStrAdjperAttChange = pClassInfo->m_iStrAdjperAttChange;
	if (m_iStrAdjperDefChange == iDefault) m_iStrAdjperDefChange = pClassInfo->m_iStrAdjperDefChange;
	if (m_iWithdrawAdjperAttChange == iDefault) m_iWithdrawAdjperAttChange = pClassInfo->m_iWithdrawAdjperAttChange;
#endif // BATTLEWORN
	if (m_iUnnerveChange == iDefault) m_iUnnerveChange = pClassInfo->m_iUnnerveChange;
	if (m_iEncloseChange == iDefault) m_iEncloseChange = pClassInfo->m_iEncloseChange;
	if (m_iLungeChange == iDefault) m_iLungeChange = pClassInfo->m_iLungeChange;
	if (m_iDynamicDefenseChange == iDefault) m_iDynamicDefenseChange = pClassInfo->m_iDynamicDefenseChange;
	if (getStrengthChange() == iDefault) m_iStrengthChange = pClassInfo->getStrengthChange();
	if (getFortitudeChange() == iDefault) m_iFortitudeChange = pClassInfo->getFortitudeChange();
#ifdef STRENGTH_IN_NUMBERS
	if (m_iFrontSupportPercentChange == iDefault) m_iFrontSupportPercentChange = pClassInfo->m_iFrontSupportPercentChange;
	if (m_iShortRangeSupportPercentChange == iDefault) m_iShortRangeSupportPercentChange = pClassInfo->m_iShortRangeSupportPercentChange;
	if (m_iMediumRangeSupportPercentChange == iDefault) m_iMediumRangeSupportPercentChange = pClassInfo->m_iMediumRangeSupportPercentChange;
	if (m_iLongRangeSupportPercentChange == iDefault) m_iLongRangeSupportPercentChange = pClassInfo->m_iLongRangeSupportPercentChange;
	if (m_iFlankSupportPercentChange == iDefault) m_iFlankSupportPercentChange = pClassInfo->m_iFlankSupportPercentChange;
#endif
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
	if (getBreakdownChanceChange() == iDefault) m_iBreakdownChanceChange = pClassInfo->getBreakdownChanceChange();
	if (getBreakdownDamageChange() == iDefault) m_iBreakdownDamageChange = pClassInfo->getBreakdownDamageChange();
	if (getTauntChange() == iDefault) m_iTauntChange = pClassInfo->getTauntChange();
	if (m_iMaxHPChange == iDefault) m_iMaxHPChange = pClassInfo->m_iMaxHPChange;
	if (getStrengthModifier() == iDefault) m_iStrengthModifier = pClassInfo->getStrengthModifier();
	if (getQualityBase() == -10) m_iQualityBase = pClassInfo->getQualityBase();
	if (getGroupBase() == -10) m_iGroupBase = pClassInfo->getGroupBase();
	if (getSizeBase() == -10) m_iSizeBase = pClassInfo->getSizeBase();
	if (getDamageModifierChange() == iDefault) m_iDamageModifierChange = pClassInfo->getDamageModifierChange();

	if (m_iUpkeepModifier == iDefault) m_iUpkeepModifier = pClassInfo->getUpkeepModifier();
	if (m_iExtraUpkeep100 == iDefault) m_iExtraUpkeep100 = pClassInfo->getExtraUpkeep100();

	if (getRBombardDamageBase() == iDefault) m_iRBombardDamageBase = pClassInfo->getRBombardDamageBase();
	if (getRBombardDamageLimitBase() == iDefault) m_iRBombardDamageLimitBase = pClassInfo->getRBombardDamageLimitBase();
	if (getRBombardDamageMaxUnitsBase() == iDefault) m_iRBombardDamageMaxUnitsBase = pClassInfo->getRBombardDamageMaxUnitsBase();
	if (getDCMBombRangeBase() == iDefault) m_iDCMBombRangeBase = pClassInfo->getDCMBombRangeBase();
	if (getDCMBombAccuracyBase() == iDefault) m_iDCMBombAccuracyBase = pClassInfo->getDCMBombAccuracyBase();
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
	if (getInsidiousnessChange() == iDefault) m_iInsidiousnessChange = pClassInfo->getInsidiousnessChange();
	if (getInvestigationChange() == iDefault) m_iInvestigationChange = pClassInfo->getInvestigationChange();
	if (m_iStealthStrikesChange == iDefault) m_iStealthStrikesChange = pClassInfo->m_iStealthStrikesChange;
	if (m_iStealthCombatModifierChange == iDefault) m_iStealthCombatModifierChange = pClassInfo->m_iStealthCombatModifierChange;
	if (m_iStealthDefenseChange == iDefault) m_iStealthDefenseChange = pClassInfo->m_iStealthDefenseChange;
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

#ifdef OUTBREAKS_AND_AFFLICTIONS
	// bool vector with delayed resolution
	if (getNumCureAfflictionChangeTypes() == 0)
	{
		const int iNum = pClassInfo->getNumCureAfflictionChangeTypes();
		m_aiCureAfflictionChangeTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			GC.copyNonDefaultDelayedResolution((int*)&(m_aiCureAfflictionChangeTypes[i]), (int*)&(pClassInfo->m_aiCureAfflictionChangeTypes[i]));
		}
	}
#endif
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
		const int iNum = pClassInfo->getNumGGptsforUnitTypes();
		m_aiGGptsforUnitTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			GC.copyNonDefaultDelayedResolution((int*)&(m_aiGGptsforUnitTypes[i]), (int*)&(pClassInfo->m_aiGGptsforUnitTypes[i]));
		}
	}

	if (getNumDefaultStatusTypes() == 0)
	{
		const int iNum = pClassInfo->getNumDefaultStatusTypes();
		m_aiDefaultStatusTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			GC.copyNonDefaultDelayedResolution((int*)&(m_aiDefaultStatusTypes[i]), (int*)&(pClassInfo->m_aiDefaultStatusTypes[i]));
		}
	}

	if (getNumTrapImmunityUnitCombatTypes() == 0)
	{
		const int iNum = pClassInfo->getNumTrapImmunityUnitCombatTypes();
		m_aiTrapImmunityUnitCombatTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			GC.copyNonDefaultDelayedResolution((int*)&(m_aiTrapImmunityUnitCombatTypes[i]), (int*)&(pClassInfo->m_aiTrapImmunityUnitCombatTypes[i]));
		}
	}

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);

	// int vectors utilizing pairing without delayed resolution

	if (getNumWithdrawOnTerrainTypeChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumWithdrawOnTerrainTypeChanges(); i++)
		{
			const TerrainTypes eTerrain = ((TerrainTypes)i);
			const int iChange = pClassInfo->getWithdrawOnTerrainTypeChange(i);
			m_aWithdrawOnTerrainTypesChange.push_back(std::make_pair(eTerrain, iChange));
		}
	}

	if (getNumWithdrawOnFeatureTypeChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumWithdrawOnFeatureTypeChanges(); i++)
		{
			const FeatureTypes eFeature = ((FeatureTypes)i);
			const int iChange = pClassInfo->getWithdrawOnFeatureTypeChange(i);
			m_aWithdrawOnFeatureTypesChange.push_back(std::make_pair(eFeature, iChange));
		}
	}

	if (getNumVisibilityIntensityChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumVisibilityIntensityChangeTypes(); i++)
		{
			const InvisibleTypes eInvisible = ((InvisibleTypes)i);
			const int iChange = pClassInfo->getVisibilityIntensityChangeType(i);
			m_aVisibilityIntensityChangeTypes.push_back(std::make_pair(eInvisible, iChange));
		}
	}

	if (getNumInvisibilityIntensityChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumInvisibilityIntensityChangeTypes(); i++)
		{
			const InvisibleTypes eInvisible = ((InvisibleTypes)i);
			const int iChange = pClassInfo->getInvisibilityIntensityChangeType(i);
			m_aInvisibilityIntensityChangeTypes.push_back(std::make_pair(eInvisible, iChange));
		}
	}

	if (getNumVisibilityIntensityRangeChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumVisibilityIntensityRangeChangeTypes(); i++)
		{
			const InvisibleTypes eInvisible = ((InvisibleTypes)i);
			const int iChange = pClassInfo->getVisibilityIntensityRangeChangeType(i);
			m_aVisibilityIntensityRangeChangeTypes.push_back(std::make_pair(eInvisible, iChange));
		}
	}

	if (getNumVisibilityIntensitySameTileChangeTypes()==0)
	{
		for (int i=0; i < pClassInfo->getNumVisibilityIntensitySameTileChangeTypes(); i++)
		{
			const InvisibleTypes eInvisible = ((InvisibleTypes)i);
			const int iChange = pClassInfo->getVisibilityIntensitySameTileChangeType(i);
			m_aVisibilityIntensitySameTileChangeTypes.push_back(std::make_pair(eInvisible, iChange));
		}
	}

#ifdef OUTBREAKS_AND_AFFLICTIONS
	if (getNumAidChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumAidChanges(); i++)
		{
			const PropertyTypes eProperty = ((PropertyTypes)i);
			const int iChange = pClassInfo->getAidChange(i);
			m_aAidChanges.push_back(std::make_pair(eProperty, iChange));
		}
	}

	// int vectors utilizing struct with delayed resolution
	if (getNumAfflictionFortitudeChangeModifiers() == 0)
	{
		const int iNum = pClassInfo->getNumAfflictionFortitudeChangeModifiers();
		m_aAfflictionFortitudeChangeModifiers.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aAfflictionFortitudeChangeModifiers[i].iModifier = pClassInfo->m_aAfflictionFortitudeChangeModifiers[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aAfflictionFortitudeChangeModifiers[i].ePromotionLine), (int*)&(pClassInfo->m_aAfflictionFortitudeChangeModifiers[i].ePromotionLine));
		}
	}
#endif
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
		const int iNum = pClassInfo->getNumBuildWorkChangeModifiers();
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
		const int iNum = pClassInfo->getNumUnitCombatChangeModifiers();
		m_aUnitCombatChangeModifiers.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aUnitCombatChangeModifiers[i].iModifier = pClassInfo->m_aUnitCombatChangeModifiers[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aUnitCombatChangeModifiers[i].eUnitCombat), (int*)&(pClassInfo->m_aUnitCombatChangeModifiers[i].eUnitCombat));
		}
	}

	if (getNumFlankingStrengthbyUnitCombatTypesChange() == 0)
	{
		const int iNum = pClassInfo->getNumFlankingStrengthbyUnitCombatTypesChange();
		m_aFlankingStrengthbyUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aFlankingStrengthbyUnitCombatTypeChange[i].iModifier = pClassInfo->m_aFlankingStrengthbyUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aFlankingStrengthbyUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aFlankingStrengthbyUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumWithdrawVSUnitCombatTypesChange() == 0)
	{
		const int iNum = pClassInfo->getNumWithdrawVSUnitCombatTypesChange();
		m_aWithdrawVSUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aWithdrawVSUnitCombatTypeChange[i].iModifier = pClassInfo->m_aWithdrawVSUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aWithdrawVSUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aWithdrawVSUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumPursuitVSUnitCombatTypesChange() == 0)
	{
		const int iNum = pClassInfo->getNumPursuitVSUnitCombatTypesChange();
		m_aPursuitVSUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aPursuitVSUnitCombatTypeChange[i].iModifier = pClassInfo->m_aPursuitVSUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aPursuitVSUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aPursuitVSUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumRepelVSUnitCombatTypesChange() == 0)
	{
		const int iNum = pClassInfo->getNumRepelVSUnitCombatTypesChange();
		m_aRepelVSUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aRepelVSUnitCombatTypeChange[i].iModifier = pClassInfo->m_aRepelVSUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aRepelVSUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aRepelVSUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumKnockbackVSUnitCombatTypesChange() == 0)
	{
		const int iNum = pClassInfo->getNumKnockbackVSUnitCombatTypesChange();
		m_aKnockbackVSUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aKnockbackVSUnitCombatTypeChange[i].iModifier = pClassInfo->m_aKnockbackVSUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aKnockbackVSUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aKnockbackVSUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumPunctureVSUnitCombatTypesChange() == 0)
	{
		const int iNum = pClassInfo->getNumPunctureVSUnitCombatTypesChange();
		m_aPunctureVSUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aPunctureVSUnitCombatTypeChange[i].iModifier = pClassInfo->m_aPunctureVSUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aPunctureVSUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aPunctureVSUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumArmorVSUnitCombatTypesChange() == 0)
	{
		const int iNum = pClassInfo->getNumArmorVSUnitCombatTypesChange();
		m_aArmorVSUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aArmorVSUnitCombatTypeChange[i].iModifier = pClassInfo->m_aArmorVSUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aArmorVSUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aArmorVSUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumDodgeVSUnitCombatTypesChange() == 0)
	{
		const int iNum = pClassInfo->getNumDodgeVSUnitCombatTypesChange();
		m_aDodgeVSUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aDodgeVSUnitCombatTypeChange[i].iModifier = pClassInfo->m_aDodgeVSUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aDodgeVSUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aDodgeVSUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumPrecisionVSUnitCombatTypesChange() == 0)
	{
		const int iNum = pClassInfo->getNumPrecisionVSUnitCombatTypesChange();
		m_aPrecisionVSUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aPrecisionVSUnitCombatTypeChange[i].iModifier = pClassInfo->m_aPrecisionVSUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aPrecisionVSUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aPrecisionVSUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumCriticalVSUnitCombatTypesChange() == 0)
	{
		const int iNum = pClassInfo->getNumCriticalVSUnitCombatTypesChange();
		m_aCriticalVSUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aCriticalVSUnitCombatTypeChange[i].iModifier = pClassInfo->m_aCriticalVSUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aCriticalVSUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aCriticalVSUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumRoundStunVSUnitCombatTypesChange() == 0)
	{
		const int iNum = pClassInfo->getNumRoundStunVSUnitCombatTypesChange();
		m_aRoundStunVSUnitCombatTypeChange.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aRoundStunVSUnitCombatTypeChange[i].iModifier = pClassInfo->m_aRoundStunVSUnitCombatTypeChange[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aRoundStunVSUnitCombatTypeChange[i].eUnitCombat), (int*)&(pClassInfo->m_aRoundStunVSUnitCombatTypeChange[i].eUnitCombat));
		}
	}

	if (getNumTrapAvoidanceUnitCombatTypes() == 0)
	{
		const int iNum = pClassInfo->getNumTrapAvoidanceUnitCombatTypes();
		m_aTrapAvoidanceUnitCombatTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aTrapAvoidanceUnitCombatTypes[i].iModifier = pClassInfo->m_aTrapAvoidanceUnitCombatTypes[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aTrapAvoidanceUnitCombatTypes[i].eUnitCombat), (int*)&(pClassInfo->m_aTrapAvoidanceUnitCombatTypes[i].eUnitCombat));
		}
	}

#ifdef OUTBREAKS_AND_AFFLICTIONS
	if (getNumAfflictOnAttackChangeTypes() == 0)
	{
		const int iNum = pClassInfo->getNumAfflictOnAttackChangeTypes();
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
#endif
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
#ifdef OUTBREAKS_AND_AFFLICTIONS
	if (getNumDistanceAttackCommunicabilityTypeChanges() == 0)
	{
		const int iNum = pClassInfo->getNumDistanceAttackCommunicabilityTypeChanges();
		m_aDistanceAttackCommunicabilityTypeChanges.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aDistanceAttackCommunicabilityTypeChanges[i].iChange = pClassInfo->m_aDistanceAttackCommunicabilityTypeChanges[i].iChange;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aDistanceAttackCommunicabilityTypeChanges[i].eAfflictionLine), (int*)&(pClassInfo->m_aDistanceAttackCommunicabilityTypeChanges[i].eAfflictionLine));
		}
	}
#endif
	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);
}

void CvUnitCombatInfo::getCheckSum(uint32_t& iSum) const
{
	PROFILE_EXTRA_FUNC();
	m_KillOutcomeList.getCheckSum(iSum);

	foreach_(const CvOutcomeMission* outcomeMission, m_aOutcomeMissions)
	{
		outcomeMission->getCheckSum(iSum);
	}

	// Textual References
	CheckSum(iSum, m_eReligion);
	CheckSum(iSum, m_eCulture);
	CheckSum(iSum, m_eEra);
	//Integers
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
#ifdef BATTLEWORN
	CheckSum(iSum, m_iStrAdjperAttChange);
	CheckSum(iSum, m_iStrAdjperDefChange);
	CheckSum(iSum, m_iWithdrawAdjperAttChange);
#endif // BATTLEWORN
	CheckSum(iSum, m_iUnnerveChange);
	CheckSum(iSum, m_iEncloseChange);
	CheckSum(iSum, m_iLungeChange);
	CheckSum(iSum, m_iDynamicDefenseChange);
	CheckSum(iSum, m_iStrengthChange);
	CheckSum(iSum, m_iFortitudeChange);
#ifdef STRENGTH_IN_NUMBERS
	CheckSum(iSum, m_iFrontSupportPercentChange);
	CheckSum(iSum, m_iShortRangeSupportPercentChange);
	CheckSum(iSum, m_iMediumRangeSupportPercentChange);
	CheckSum(iSum, m_iLongRangeSupportPercentChange);
	CheckSum(iSum, m_iFlankSupportPercentChange);
#endif
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
	CheckSum(iSum, m_iBreakdownChanceChange);
	CheckSum(iSum, m_iBreakdownDamageChange);
	CheckSum(iSum, m_iTauntChange);
	CheckSum(iSum, m_iMaxHPChange);
	CheckSum(iSum, m_iStrengthModifier);
	CheckSum(iSum, m_iQualityBase);
	CheckSum(iSum, m_iGroupBase);
	CheckSum(iSum, m_iSizeBase);
	CheckSum(iSum, m_iDamageModifierChange);

	CheckSum(iSum, m_iUpkeepModifier);
	CheckSum(iSum, m_iExtraUpkeep100);

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

	// Arrays
	CheckSum(iSum, m_piDomainModifierPercent, NUM_DOMAIN_TYPES);

	// int vectors utilizing pairing without delayed resolution
	CheckSumC(iSum, m_aWithdrawOnTerrainTypesChange);
	CheckSumC(iSum, m_aWithdrawOnFeatureTypesChange);
	CheckSumC(iSum, m_aVisibilityIntensityChangeTypes);
	CheckSumC(iSum, m_aInvisibilityIntensityChangeTypes);
	CheckSumC(iSum, m_aVisibilityIntensityRangeChangeTypes);
	CheckSumC(iSum, m_aVisibilityIntensitySameTileChangeTypes);
#ifdef OUTBREAKS_AND_AFFLICTIONS
	CheckSumC(iSum, m_aAidChanges);
	CheckSumC(iSum, m_aiCureAfflictionChangeTypes);
#endif
	CheckSumC(iSum, m_aiTerrainIgnoreDamageChangeTypes);
	CheckSumC(iSum, m_aiTerrainDoubleMoveChangeTypes);
	CheckSumC(iSum, m_aiFeatureDoubleMoveChangeTypes);
	CheckSumC(iSum, m_aiOnGameOptions);
	CheckSumC(iSum, m_aiNotOnGameOptions);
	CheckSumC(iSum, m_aiGGptsforUnitTypes);
	CheckSumC(iSum, m_aiDefaultStatusTypes);
	CheckSumC(iSum, m_aiTrapImmunityUnitCombatTypes);
	CheckSumC(iSum, m_aiCategories);

	//int vectors utilizing struct with delayed resolution
	int iNumElements;
#ifdef OUTBREAKS_AND_AFFLICTIONS
	iNumElements = m_aAfflictionFortitudeChangeModifiers.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aAfflictionFortitudeChangeModifiers[i].ePromotionLine);
		CheckSum(iSum, m_aAfflictionFortitudeChangeModifiers[i].iModifier);
	}
#endif
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

#ifdef OUTBREAKS_AND_AFFLICTIONS
	iNumElements = m_aAfflictOnAttackChangeTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aAfflictOnAttackChangeTypes[i].eAfflictionLine);
		CheckSum(iSum, m_aAfflictOnAttackChangeTypes[i].iProbabilityChange);
		CheckSum(iSum, m_aAfflictOnAttackChangeTypes[i].iMelee);
		CheckSum(iSum, m_aAfflictOnAttackChangeTypes[i].iDistance);
		CheckSum(iSum, m_aAfflictOnAttackChangeTypes[i].iImmediate);
	}
#endif
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
#ifdef OUTBREAKS_AND_AFFLICTIONS
	iNumElements = m_aDistanceAttackCommunicabilityTypeChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aDistanceAttackCommunicabilityTypeChanges[i].eAfflictionLine);
		CheckSum(iSum, m_aDistanceAttackCommunicabilityTypeChanges[i].iChange);
	}
#endif

	m_PropertyManipulators.getCheckSum(iSum);

}

const CvOutcomeList* CvUnitCombatInfo::getKillOutcomeList() const
{
	return &m_KillOutcomeList;
}

int CvUnitCombatInfo::getNumActionOutcomes() const
{
	return m_aOutcomeMissions.size();
}

MissionTypes CvUnitCombatInfo::getActionOutcomeMission(int index) const
{
	return m_aOutcomeMissions[index]->getMission();
}

const CvOutcomeList* CvUnitCombatInfo::getActionOutcomeList(int index) const
{
	return m_aOutcomeMissions[index]->getOutcomeList();
}

const CvOutcomeList* CvUnitCombatInfo::getActionOutcomeListByMission(MissionTypes eMission) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const CvOutcomeMission * outcomeMission, m_aOutcomeMissions)
	{
		if (outcomeMission->getMission() == eMission)
		{
			return outcomeMission->getOutcomeList();
		}
	}
	return NULL;
}

const CvOutcomeMission* CvUnitCombatInfo::getOutcomeMission(int index) const
{
	return m_aOutcomeMissions[index];
}

const CvOutcomeMission* CvUnitCombatInfo::getOutcomeMissionByMission(MissionTypes eMission) const
{
	return algo::find_if(m_aOutcomeMissions, bind(CvOutcomeMission::getMission, _1) == eMission).get_value_or(NULL);
}