//------------------------------------------------------------------------------------------------
//  FILE:    CvLeaderHeadInfo.cpp
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
#include "CvLeaderHeadInfo.h"


//======================================================================================================
//					CvLeaderHeadInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvLeaderHeadInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvLeaderHeadInfo::CvLeaderHeadInfo() :
m_bNPC(false),
m_iWonderConstructRand(0),
m_iBaseAttitude(0),
m_iBasePeaceWeight(0),
m_iPeaceWeightRand(0),
m_iWarmongerRespect(0),
m_iEspionageWeight(0),
m_iRefuseToTalkWarThreshold(0),
m_iNoTechTradeThreshold(0),
m_iTechTradeKnownPercent(0),
m_iMaxGoldTradePercent(0),
m_iMaxGoldPerTurnTradePercent(0),
m_iCultureVictoryWeight(0),
m_iSpaceVictoryWeight(0),
m_iConquestVictoryWeight(0),
m_iDominationVictoryWeight(0),
m_iDiplomacyVictoryWeight(0),
m_iMaxWarRand(0),
m_iMaxWarNearbyPowerRatio(0),
m_iMaxWarDistantPowerRatio(0),
m_iMaxWarMinAdjacentLandPercent(0),
m_iLimitedWarRand(0),
m_iLimitedWarPowerRatio(0),
m_iDogpileWarRand(0),
m_iMakePeaceRand(0),
m_iDeclareWarTradeRand(0),
m_iDemandRebukedSneakProb(0),
m_iDemandRebukedWarProb(0),
m_iRazeCityProb(0),
m_iBuildUnitProb(0),
m_iBaseAttackOddsChange(0),
m_iAttackOddsChangeRand(0),
m_iWorseRankDifferenceAttitudeChange(0),
m_iBetterRankDifferenceAttitudeChange(0),
m_iCloseBordersAttitudeChange(0),
m_iLostWarAttitudeChange(0),
m_iAtWarAttitudeDivisor(0),
m_iAtWarAttitudeChangeLimit(0),
m_iAtPeaceAttitudeDivisor(0),
m_iAtPeaceAttitudeChangeLimit(0),
m_iSameReligionAttitudeChange(0),
m_iSameReligionAttitudeDivisor(0),
m_iSameReligionAttitudeChangeLimit(0),
m_iDifferentReligionAttitudeChange(0),
m_iDifferentReligionAttitudeDivisor(0),
m_iDifferentReligionAttitudeChangeLimit(0),
m_iBonusTradeAttitudeDivisor(0),
m_iBonusTradeAttitudeChangeLimit(0),
m_iOpenBordersAttitudeDivisor(0),
m_iOpenBordersAttitudeChangeLimit(0),
m_iDefensivePactAttitudeDivisor(0),
m_iDefensivePactAttitudeChangeLimit(0),
m_iShareWarAttitudeChange(0),
m_iShareWarAttitudeDivisor(0),
m_iShareWarAttitudeChangeLimit(0),
m_iFavoriteCivicAttitudeChange(0),
m_iFavoriteCivicAttitudeDivisor(0),
m_iFavoriteCivicAttitudeChangeLimit(0),
m_iDemandTributeAttitudeThreshold(NO_ATTITUDE),
m_iNoGiveHelpAttitudeThreshold(NO_ATTITUDE),
m_iTechRefuseAttitudeThreshold(NO_ATTITUDE),
m_iStrategicBonusRefuseAttitudeThreshold(NO_ATTITUDE),
m_iHappinessBonusRefuseAttitudeThreshold(NO_ATTITUDE),
m_iHealthBonusRefuseAttitudeThreshold(NO_ATTITUDE),
m_iMapRefuseAttitudeThreshold(NO_ATTITUDE),
m_iDeclareWarRefuseAttitudeThreshold(NO_ATTITUDE),
m_iDeclareWarThemRefuseAttitudeThreshold(NO_ATTITUDE),
m_iStopTradingRefuseAttitudeThreshold(NO_ATTITUDE),
m_iStopTradingThemRefuseAttitudeThreshold(NO_ATTITUDE),
m_iAdoptCivicRefuseAttitudeThreshold(NO_ATTITUDE),
m_iConvertReligionRefuseAttitudeThreshold(NO_ATTITUDE),
m_iOpenBordersRefuseAttitudeThreshold(NO_ATTITUDE),
m_iDefensivePactRefuseAttitudeThreshold(NO_ATTITUDE),
m_iPermanentAllianceRefuseAttitudeThreshold(NO_ATTITUDE),
m_iVassalRefuseAttitudeThreshold(NO_ATTITUDE),
m_iVassalPowerModifier(0),
m_iFreedomAppreciation(0),
m_iFavoriteCivic(NO_CIVIC),
m_iFavoriteReligion(NO_RELIGION),
m_pbTraits(NULL),
m_piFlavorValue(NULL),
m_piContactRand(NULL),
m_piContactDelay(NULL),
m_piMemoryDecayRand(NULL),
m_piMemoryAttitudePercent(NULL),
m_piNoWarAttitudeProb(NULL),
m_piUnitAIWeightModifier(NULL),
m_piImprovementWeightModifier(NULL),
m_piDiploPeaceIntroMusicScriptIds(NULL),
m_piDiploPeaceMusicScriptIds(NULL),
m_piDiploWarIntroMusicScriptIds(NULL),
m_piDiploWarMusicScriptIds(NULL)

,m_iMilitaryUnitRefuseAttitudeThreshold(ATTITUDE_ANNOYED)
,m_iWorkerRefuseAttitudeThreshold(ATTITUDE_ANNOYED)
,m_iCorporationRefuseAttitudeThreshold(ATTITUDE_CAUTIOUS)
,m_iSecretaryGeneralVoteRefuseAttitudeThreshold(ATTITUDE_ANNOYED)
{ }


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvLeaderHeadInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvLeaderHeadInfo::~CvLeaderHeadInfo()
{
	SAFE_DELETE_ARRAY(m_pbTraits);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piContactRand);
	SAFE_DELETE_ARRAY(m_piContactDelay);
	SAFE_DELETE_ARRAY(m_piMemoryDecayRand);
	SAFE_DELETE_ARRAY(m_piMemoryAttitudePercent);
	SAFE_DELETE_ARRAY(m_piNoWarAttitudeProb);
	SAFE_DELETE_ARRAY(m_piUnitAIWeightModifier);
	SAFE_DELETE_ARRAY(m_piImprovementWeightModifier);
	SAFE_DELETE_ARRAY(m_piDiploPeaceIntroMusicScriptIds);
	SAFE_DELETE_ARRAY(m_piDiploPeaceMusicScriptIds);
	SAFE_DELETE_ARRAY(m_piDiploWarIntroMusicScriptIds);
	SAFE_DELETE_ARRAY(m_piDiploWarMusicScriptIds);
}


const char* CvLeaderHeadInfo::getButton() const
{
	const CvArtInfoLeaderhead* pLeaderheadArtInfo = getArtInfo();
	return pLeaderheadArtInfo ? pLeaderheadArtInfo->getButton() : NULL;
}


bool CvLeaderHeadInfo::isNPC() const
{
	return m_bNPC;
}


int CvLeaderHeadInfo::getWonderConstructRand() const
{
	return m_iWonderConstructRand;
}


int CvLeaderHeadInfo::getBaseAttitude() const
{
	return m_iBaseAttitude;
}


int CvLeaderHeadInfo::getBasePeaceWeight() const
{
	return m_iBasePeaceWeight;
}


int CvLeaderHeadInfo::getPeaceWeightRand() const
{
	return m_iPeaceWeightRand;
}


int CvLeaderHeadInfo::getWarmongerRespect() const
{
	return m_iWarmongerRespect;
}


int CvLeaderHeadInfo::getEspionageWeight() const
{
	return m_iEspionageWeight;
}


int CvLeaderHeadInfo::getRefuseToTalkWarThreshold() const
{
	return m_iRefuseToTalkWarThreshold;
}


int CvLeaderHeadInfo::getNoTechTradeThreshold() const
{
	return m_iNoTechTradeThreshold;
}


int CvLeaderHeadInfo::getTechTradeKnownPercent() const
{
	return m_iTechTradeKnownPercent;
}


int CvLeaderHeadInfo::getMaxGoldTradePercent() const
{
	return m_iMaxGoldTradePercent;
}


int CvLeaderHeadInfo::getMaxGoldPerTurnTradePercent() const
{
	return m_iMaxGoldPerTurnTradePercent;
}


int CvLeaderHeadInfo::getCultureVictoryWeight() const
{
	return m_iCultureVictoryWeight;
}


int CvLeaderHeadInfo::getSpaceVictoryWeight() const
{
	return m_iSpaceVictoryWeight;
}


int CvLeaderHeadInfo::getConquestVictoryWeight() const
{
	return m_iConquestVictoryWeight;
}


int CvLeaderHeadInfo::getDominationVictoryWeight() const
{
	return m_iDominationVictoryWeight;
}


int CvLeaderHeadInfo::getDiplomacyVictoryWeight() const
{
	return m_iDiplomacyVictoryWeight;
}


int CvLeaderHeadInfo::getMaxWarRand() const
{
	return m_iMaxWarRand;
}


int CvLeaderHeadInfo::getMaxWarNearbyPowerRatio() const
{
	return m_iMaxWarNearbyPowerRatio;
}


int CvLeaderHeadInfo::getMaxWarDistantPowerRatio() const
{
	return m_iMaxWarDistantPowerRatio;
}


int CvLeaderHeadInfo::getMaxWarMinAdjacentLandPercent() const
{
	return m_iMaxWarMinAdjacentLandPercent;
}


int CvLeaderHeadInfo::getLimitedWarRand() const
{
	return m_iLimitedWarRand;
}


int CvLeaderHeadInfo::getLimitedWarPowerRatio() const
{
	return m_iLimitedWarPowerRatio;
}


int CvLeaderHeadInfo::getDogpileWarRand() const
{
	return m_iDogpileWarRand;
}


int CvLeaderHeadInfo::getMakePeaceRand() const
{
	return m_iMakePeaceRand;
}


int CvLeaderHeadInfo::getDeclareWarTradeRand() const
{
	return m_iDeclareWarTradeRand;
}


int CvLeaderHeadInfo::getDemandRebukedSneakProb() const
{
	return m_iDemandRebukedSneakProb;
}


int CvLeaderHeadInfo::getDemandRebukedWarProb() const
{
	return m_iDemandRebukedWarProb;
}


int CvLeaderHeadInfo::getRazeCityProb() const
{
	return m_iRazeCityProb;
}


int CvLeaderHeadInfo::getBuildUnitProb() const
{
	return m_iBuildUnitProb;
}


int CvLeaderHeadInfo::getBaseAttackOddsChange() const
{
	return m_iBaseAttackOddsChange;
}


int CvLeaderHeadInfo::getAttackOddsChangeRand() const
{
	return m_iAttackOddsChangeRand;
}


int CvLeaderHeadInfo::getWorseRankDifferenceAttitudeChange() const
{
	return m_iWorseRankDifferenceAttitudeChange;
}


int CvLeaderHeadInfo::getBetterRankDifferenceAttitudeChange() const
{
	return m_iBetterRankDifferenceAttitudeChange;
}


int CvLeaderHeadInfo::getCloseBordersAttitudeChange() const
{
	return m_iCloseBordersAttitudeChange;
}


int CvLeaderHeadInfo::getLostWarAttitudeChange() const
{
	return m_iLostWarAttitudeChange;
}


int CvLeaderHeadInfo::getAtWarAttitudeDivisor() const
{
	return m_iAtWarAttitudeDivisor;
}


int CvLeaderHeadInfo::getAtWarAttitudeChangeLimit() const
{
	return m_iAtWarAttitudeChangeLimit;
}


int CvLeaderHeadInfo::getAtPeaceAttitudeDivisor() const
{
	return m_iAtPeaceAttitudeDivisor;
}


int CvLeaderHeadInfo::getAtPeaceAttitudeChangeLimit() const
{
	return m_iAtPeaceAttitudeChangeLimit;
}


int CvLeaderHeadInfo::getSameReligionAttitudeChange() const
{
	return m_iSameReligionAttitudeChange;
}


int CvLeaderHeadInfo::getSameReligionAttitudeDivisor() const
{
	return m_iSameReligionAttitudeDivisor;
}


int CvLeaderHeadInfo::getSameReligionAttitudeChangeLimit() const
{
	return m_iSameReligionAttitudeChangeLimit;
}


int CvLeaderHeadInfo::getDifferentReligionAttitudeChange() const
{
	return m_iDifferentReligionAttitudeChange;
}


int CvLeaderHeadInfo::getDifferentReligionAttitudeDivisor() const
{
	return m_iDifferentReligionAttitudeDivisor;
}


int CvLeaderHeadInfo::getDifferentReligionAttitudeChangeLimit() const
{
	return m_iDifferentReligionAttitudeChangeLimit;
}


int CvLeaderHeadInfo::getBonusTradeAttitudeDivisor() const
{
	return m_iBonusTradeAttitudeDivisor;
}


int CvLeaderHeadInfo::getBonusTradeAttitudeChangeLimit() const
{
	return m_iBonusTradeAttitudeChangeLimit;
}


int CvLeaderHeadInfo::getOpenBordersAttitudeDivisor() const
{
	return m_iOpenBordersAttitudeDivisor;
}


int CvLeaderHeadInfo::getOpenBordersAttitudeChangeLimit() const
{
	return m_iOpenBordersAttitudeChangeLimit;
}


int CvLeaderHeadInfo::getDefensivePactAttitudeDivisor() const
{
	return m_iDefensivePactAttitudeDivisor;
}


int CvLeaderHeadInfo::getDefensivePactAttitudeChangeLimit() const
{
	return m_iDefensivePactAttitudeChangeLimit;
}


int CvLeaderHeadInfo::getShareWarAttitudeChange() const
{
	return m_iShareWarAttitudeChange;
}


int CvLeaderHeadInfo::getShareWarAttitudeDivisor() const
{
	return m_iShareWarAttitudeDivisor;
}


int CvLeaderHeadInfo::getShareWarAttitudeChangeLimit() const
{
	return m_iShareWarAttitudeChangeLimit;
}


int CvLeaderHeadInfo::getFavoriteCivicAttitudeChange() const
{
	return m_iFavoriteCivicAttitudeChange;
}


int CvLeaderHeadInfo::getFavoriteCivicAttitudeDivisor() const
{
	return m_iFavoriteCivicAttitudeDivisor;
}


int CvLeaderHeadInfo::getFavoriteCivicAttitudeChangeLimit() const
{
	return m_iFavoriteCivicAttitudeChangeLimit;
}


int CvLeaderHeadInfo::getDemandTributeAttitudeThreshold() const
{
	return m_iDemandTributeAttitudeThreshold;
}


int CvLeaderHeadInfo::getNoGiveHelpAttitudeThreshold() const
{
	return m_iNoGiveHelpAttitudeThreshold;
}


int CvLeaderHeadInfo::getTechRefuseAttitudeThreshold() const
{
	return m_iTechRefuseAttitudeThreshold;
}


int CvLeaderHeadInfo::getStrategicBonusRefuseAttitudeThreshold() const
{
	return m_iStrategicBonusRefuseAttitudeThreshold;
}


int CvLeaderHeadInfo::getHappinessBonusRefuseAttitudeThreshold() const
{
	return m_iHappinessBonusRefuseAttitudeThreshold;
}


int CvLeaderHeadInfo::getHealthBonusRefuseAttitudeThreshold() const
{
	return m_iHealthBonusRefuseAttitudeThreshold;
}


int CvLeaderHeadInfo::getMapRefuseAttitudeThreshold() const
{
	return m_iMapRefuseAttitudeThreshold;
}


int CvLeaderHeadInfo::getDeclareWarRefuseAttitudeThreshold() const
{
	return m_iDeclareWarRefuseAttitudeThreshold;
}


int CvLeaderHeadInfo::getDeclareWarThemRefuseAttitudeThreshold() const
{
	return m_iDeclareWarThemRefuseAttitudeThreshold;
}


int CvLeaderHeadInfo::getStopTradingRefuseAttitudeThreshold() const
{
	return m_iStopTradingRefuseAttitudeThreshold;
}


int CvLeaderHeadInfo::getStopTradingThemRefuseAttitudeThreshold() const
{
	return m_iStopTradingThemRefuseAttitudeThreshold;
}


int CvLeaderHeadInfo::getAdoptCivicRefuseAttitudeThreshold() const
{
	return m_iAdoptCivicRefuseAttitudeThreshold;
}


int CvLeaderHeadInfo::getConvertReligionRefuseAttitudeThreshold() const
{
	return m_iConvertReligionRefuseAttitudeThreshold;
}


int CvLeaderHeadInfo::getOpenBordersRefuseAttitudeThreshold() const
{
	return m_iOpenBordersRefuseAttitudeThreshold;
}


int CvLeaderHeadInfo::getDefensivePactRefuseAttitudeThreshold() const
{
	return m_iDefensivePactRefuseAttitudeThreshold;
}


int CvLeaderHeadInfo::getPermanentAllianceRefuseAttitudeThreshold() const
{
	return m_iPermanentAllianceRefuseAttitudeThreshold;
}


int CvLeaderHeadInfo::getVassalRefuseAttitudeThreshold() const
{
	return m_iVassalRefuseAttitudeThreshold;
}


int CvLeaderHeadInfo::getVassalPowerModifier() const
{
	return m_iVassalPowerModifier;
}


int CvLeaderHeadInfo::getFavoriteCivic() const
{
	return m_iFavoriteCivic;
}


int CvLeaderHeadInfo::getFavoriteReligion() const
{
	return m_iFavoriteReligion;
}


int CvLeaderHeadInfo::getFreedomAppreciation() const
{
	return m_iFreedomAppreciation;
}


const char* CvLeaderHeadInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}


// Arrays
//DEFTRAITORIG
bool CvLeaderHeadInfo::hasTrait(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTraitInfos(), i);
	return m_pbTraits ? m_pbTraits[i] : false;
}


int CvLeaderHeadInfo::getFlavorValue(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFlavorTypes(), i);
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}


int CvLeaderHeadInfo::getContactRand(int i) const
{
	FASSERT_BOUNDS(0, NUM_CONTACT_TYPES, i);
	return m_piContactRand[i];
}


int CvLeaderHeadInfo::getContactDelay(int i) const
{
	FASSERT_BOUNDS(0, NUM_CONTACT_TYPES, i);
	return m_piContactDelay[i];
}


int CvLeaderHeadInfo::getMemoryDecayRand(int i) const
{
	FASSERT_BOUNDS(0, NUM_MEMORY_TYPES, i);
	return m_piMemoryDecayRand[i];
}


int CvLeaderHeadInfo::getMemoryAttitudePercent(int i) const
{
	FASSERT_BOUNDS(0, NUM_MEMORY_TYPES, i);
	return m_piMemoryAttitudePercent[i];
}


int CvLeaderHeadInfo::getNoWarAttitudeProb(int i) const
{
	FASSERT_BOUNDS(0, NUM_ATTITUDE_TYPES, i);
	return m_piNoWarAttitudeProb ? m_piNoWarAttitudeProb[i] : 0;
}


int CvLeaderHeadInfo::getUnitAIWeightModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_UNITAI_TYPES, i);
	return m_piUnitAIWeightModifier ? m_piUnitAIWeightModifier[i] : 0;
}


int CvLeaderHeadInfo::getImprovementWeightModifier(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), i);
	return m_piImprovementWeightModifier ? m_piImprovementWeightModifier[i] : 0;
}


int CvLeaderHeadInfo::getDiploPeaceIntroMusicScriptIds(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumEraInfos(), i);
	return m_piDiploPeaceIntroMusicScriptIds ? m_piDiploPeaceIntroMusicScriptIds[i] : -1;
}


int CvLeaderHeadInfo::getDiploPeaceMusicScriptIds(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumEraInfos(), i);
	return m_piDiploPeaceMusicScriptIds ? m_piDiploPeaceMusicScriptIds[i] : -1;
}


int CvLeaderHeadInfo::getDiploWarIntroMusicScriptIds(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumEraInfos(), i);
	return m_piDiploWarIntroMusicScriptIds ? m_piDiploWarIntroMusicScriptIds[i] : -1;
}


int CvLeaderHeadInfo::getDiploWarMusicScriptIds(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumEraInfos(), i);
	return m_piDiploWarMusicScriptIds ? m_piDiploWarMusicScriptIds[i] : -1;
}


const char* CvLeaderHeadInfo::getLeaderHead() const
{
	return getArtInfo() ? getArtInfo()->getNIF() : NULL;
}


int CvLeaderHeadInfo::getMilitaryUnitRefuseAttitudeThreshold() const
{
	return m_iMilitaryUnitRefuseAttitudeThreshold;
}


int CvLeaderHeadInfo::getWorkerRefuseAttitudeThreshold() const
{
	return m_iWorkerRefuseAttitudeThreshold;
}


int CvLeaderHeadInfo::getCorporationRefuseAttitudeThreshold() const
{
	return m_iCorporationRefuseAttitudeThreshold;
}


int CvLeaderHeadInfo::getSecretaryGeneralVoteRefuseAttitudeThreshold() const
{
	return m_iSecretaryGeneralVoteRefuseAttitudeThreshold;
}


void CvLeaderHeadInfo::setCultureVictoryWeight(int i)
{
	m_iCultureVictoryWeight = i;
}


void CvLeaderHeadInfo::setSpaceVictoryWeight(int i)
{
	m_iSpaceVictoryWeight = i;
}


void CvLeaderHeadInfo::setConquestVictoryWeight(int i)
{
	m_iConquestVictoryWeight = i;
}


void CvLeaderHeadInfo::setDominationVictoryWeight(int i)
{
	m_iDominationVictoryWeight = i;
}


void CvLeaderHeadInfo::setDiplomacyVictoryWeight(int i)
{
	m_iDiplomacyVictoryWeight = i;
}


//Int list Vector without delayed resolution
int CvLeaderHeadInfo::getDefaultTrait(int i) const
{
	return m_aiDefaultTraits[i];
}


int CvLeaderHeadInfo::getNumDefaultTraits() const
{
	return (int)m_aiDefaultTraits.size();
}


bool CvLeaderHeadInfo::isDefaultTrait(int i) const
{
	return algo::any_of_equal(m_aiDefaultTraits, i);
}


int CvLeaderHeadInfo::getDefaultComplexTrait(int i) const
{
	return m_aiDefaultComplexTraits[i];
}


int CvLeaderHeadInfo::getNumDefaultComplexTraits() const
{
	return (int)m_aiDefaultComplexTraits.size();
}


bool CvLeaderHeadInfo::isDefaultComplexTrait(int i) const
{
	return algo::any_of_equal(m_aiDefaultComplexTraits, i);
}



void CvLeaderHeadInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_bNPC);
	CheckSum(iSum, m_iWonderConstructRand);
	CheckSum(iSum, m_iBaseAttitude);
	CheckSum(iSum, m_iBasePeaceWeight);
	CheckSum(iSum, m_iPeaceWeightRand);
	CheckSum(iSum, m_iWarmongerRespect);
	CheckSum(iSum, m_iEspionageWeight);
	CheckSum(iSum, m_iRefuseToTalkWarThreshold);
	CheckSum(iSum, m_iNoTechTradeThreshold);
	CheckSum(iSum, m_iTechTradeKnownPercent);
	CheckSum(iSum, m_iMaxGoldTradePercent);
	CheckSum(iSum, m_iMaxGoldPerTurnTradePercent);

	CheckSum(iSum, m_iCultureVictoryWeight);
	CheckSum(iSum, m_iSpaceVictoryWeight);
	CheckSum(iSum, m_iConquestVictoryWeight);
	CheckSum(iSum, m_iDominationVictoryWeight);
	CheckSum(iSum, m_iDiplomacyVictoryWeight);

	CheckSum(iSum, m_iMaxWarRand);
	CheckSum(iSum, m_iMaxWarNearbyPowerRatio);
	CheckSum(iSum, m_iMaxWarDistantPowerRatio);
	CheckSum(iSum, m_iMaxWarMinAdjacentLandPercent);
	CheckSum(iSum, m_iLimitedWarRand);
	CheckSum(iSum, m_iLimitedWarPowerRatio);
	CheckSum(iSum, m_iDogpileWarRand);
	CheckSum(iSum, m_iMakePeaceRand);
	CheckSum(iSum, m_iDeclareWarTradeRand);
	CheckSum(iSum, m_iDemandRebukedSneakProb);
	CheckSum(iSum, m_iDemandRebukedWarProb);
	CheckSum(iSum, m_iRazeCityProb);
	CheckSum(iSum, m_iBuildUnitProb);
	CheckSum(iSum, m_iBaseAttackOddsChange);
	CheckSum(iSum, m_iAttackOddsChangeRand);
	CheckSum(iSum, m_iWorseRankDifferenceAttitudeChange);
	CheckSum(iSum, m_iBetterRankDifferenceAttitudeChange);
	CheckSum(iSum, m_iCloseBordersAttitudeChange);
	CheckSum(iSum, m_iLostWarAttitudeChange);
	CheckSum(iSum, m_iAtWarAttitudeDivisor);
	CheckSum(iSum, m_iAtWarAttitudeChangeLimit);
	CheckSum(iSum, m_iAtPeaceAttitudeDivisor);
	CheckSum(iSum, m_iAtPeaceAttitudeChangeLimit);
	CheckSum(iSum, m_iSameReligionAttitudeChange);
	CheckSum(iSum, m_iSameReligionAttitudeDivisor);
	CheckSum(iSum, m_iSameReligionAttitudeChangeLimit);
	CheckSum(iSum, m_iDifferentReligionAttitudeChange);
	CheckSum(iSum, m_iDifferentReligionAttitudeDivisor);
	CheckSum(iSum, m_iDifferentReligionAttitudeChangeLimit);
	CheckSum(iSum, m_iBonusTradeAttitudeDivisor);
	CheckSum(iSum, m_iBonusTradeAttitudeChangeLimit);
	CheckSum(iSum, m_iOpenBordersAttitudeDivisor);
	CheckSum(iSum, m_iOpenBordersAttitudeChangeLimit);
	CheckSum(iSum, m_iDefensivePactAttitudeDivisor);
	CheckSum(iSum, m_iDefensivePactAttitudeChangeLimit);
	CheckSum(iSum, m_iShareWarAttitudeChange);
	CheckSum(iSum, m_iShareWarAttitudeDivisor);
	CheckSum(iSum, m_iShareWarAttitudeChangeLimit);
	CheckSum(iSum, m_iFavoriteCivicAttitudeChange);
	CheckSum(iSum, m_iFavoriteCivicAttitudeDivisor);
	CheckSum(iSum, m_iFavoriteCivicAttitudeChangeLimit);
	CheckSum(iSum, m_iDemandTributeAttitudeThreshold);
	CheckSum(iSum, m_iNoGiveHelpAttitudeThreshold);
	CheckSum(iSum, m_iTechRefuseAttitudeThreshold);
	CheckSum(iSum, m_iStrategicBonusRefuseAttitudeThreshold);
	CheckSum(iSum, m_iHappinessBonusRefuseAttitudeThreshold);
	CheckSum(iSum, m_iHealthBonusRefuseAttitudeThreshold);
	CheckSum(iSum, m_iMapRefuseAttitudeThreshold);
	CheckSum(iSum, m_iDeclareWarRefuseAttitudeThreshold);
	CheckSum(iSum, m_iDeclareWarThemRefuseAttitudeThreshold);
	CheckSum(iSum, m_iStopTradingRefuseAttitudeThreshold);
	CheckSum(iSum, m_iStopTradingThemRefuseAttitudeThreshold);
	CheckSum(iSum, m_iAdoptCivicRefuseAttitudeThreshold);
	CheckSum(iSum, m_iConvertReligionRefuseAttitudeThreshold);
	CheckSum(iSum, m_iOpenBordersRefuseAttitudeThreshold);
	CheckSum(iSum, m_iDefensivePactRefuseAttitudeThreshold);
	CheckSum(iSum, m_iPermanentAllianceRefuseAttitudeThreshold);
	CheckSum(iSum, m_iVassalRefuseAttitudeThreshold);
	CheckSum(iSum, m_iVassalPowerModifier);
	CheckSum(iSum, m_iFreedomAppreciation);
	CheckSum(iSum, m_iFavoriteCivic);
	CheckSum(iSum, m_iFavoriteReligion);

	// Arrays

	CheckSumI(iSum, GC.getNumTraitInfos(), m_pbTraits);

	CheckSumI(iSum, GC.getNumFlavorTypes(), m_piFlavorValue);
	CheckSumI(iSum, NUM_CONTACT_TYPES, m_piContactRand);
	CheckSumI(iSum, NUM_CONTACT_TYPES, m_piContactDelay);
	CheckSumI(iSum, NUM_MEMORY_TYPES, m_piMemoryDecayRand);
	CheckSumI(iSum, NUM_MEMORY_TYPES, m_piMemoryAttitudePercent);
	CheckSumI(iSum, NUM_ATTITUDE_TYPES, m_piNoWarAttitudeProb);
	CheckSumI(iSum, NUM_UNITAI_TYPES, m_piUnitAIWeightModifier);
	CheckSumI(iSum, GC.getNumImprovementInfos(), m_piImprovementWeightModifier);

	CheckSum(iSum, m_iMilitaryUnitRefuseAttitudeThreshold);
	CheckSum(iSum, m_iWorkerRefuseAttitudeThreshold);
	CheckSum(iSum, m_iCorporationRefuseAttitudeThreshold);
	CheckSum(iSum, m_iSecretaryGeneralVoteRefuseAttitudeThreshold);


	//Int list Vector without delayed resolution

	CheckSumC(iSum, m_aiDefaultTraits);
	CheckSumC(iSum, m_aiDefaultComplexTraits);


}


const CvArtInfoLeaderhead* CvLeaderHeadInfo::getArtInfo() const
{
	return ARTFILEMGR.getLeaderheadArtInfo( getArtDefineTag());
}


bool CvLeaderHeadInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	if (!m_piMemoryDecayRand)
	{
		CvXMLLoadUtility::InitList(&m_piMemoryDecayRand, NUM_MEMORY_TYPES, 0);
		CvXMLLoadUtility::InitList(&m_piMemoryAttitudePercent, NUM_MEMORY_TYPES, 0);
		CvXMLLoadUtility::InitList(&m_piContactRand, NUM_CONTACT_TYPES, 0);
		CvXMLLoadUtility::InitList(&m_piContactDelay, NUM_CONTACT_TYPES, 0);
	}
	CvString szTextVal;

	pXML->GetOptionalChildXmlValByName(m_szArtDefineTag, L"ArtDefineTag");
	pXML->GetOptionalChildXmlValByName(&m_bNPC, L"bNPC");
	pXML->GetOptionalChildXmlValByName(&m_iWonderConstructRand, L"iWonderConstructRand");
	pXML->GetOptionalChildXmlValByName(&m_iBaseAttitude, L"iBaseAttitude");
	pXML->GetOptionalChildXmlValByName(&m_iBasePeaceWeight, L"iBasePeaceWeight");
	pXML->GetOptionalChildXmlValByName(&m_iPeaceWeightRand, L"iPeaceWeightRand");
	pXML->GetOptionalChildXmlValByName(&m_iWarmongerRespect, L"iWarmongerRespect");
	pXML->GetOptionalChildXmlValByName(&m_iEspionageWeight, L"iEspionageWeight");
	pXML->GetOptionalChildXmlValByName(&m_iRefuseToTalkWarThreshold, L"iRefuseToTalkWarThreshold");
	pXML->GetOptionalChildXmlValByName(&m_iNoTechTradeThreshold, L"iNoTechTradeThreshold");
	pXML->GetOptionalChildXmlValByName(&m_iTechTradeKnownPercent, L"iTechTradeKnownPercent");
	pXML->GetOptionalChildXmlValByName(&m_iMaxGoldTradePercent, L"iMaxGoldTradePercent");
	pXML->GetOptionalChildXmlValByName(&m_iMaxGoldPerTurnTradePercent, L"iMaxGoldPerTurnTradePercent");
	pXML->GetOptionalChildXmlValByName(&m_iCultureVictoryWeight, L"iCultureVictoryWeight");
	pXML->GetOptionalChildXmlValByName(&m_iSpaceVictoryWeight, L"iSpaceVictoryWeight");
	pXML->GetOptionalChildXmlValByName(&m_iConquestVictoryWeight, L"iConquestVictoryWeight");
	pXML->GetOptionalChildXmlValByName(&m_iDominationVictoryWeight, L"iDominationVictoryWeight");
	pXML->GetOptionalChildXmlValByName(&m_iDiplomacyVictoryWeight, L"iDiplomacyVictoryWeight");
	pXML->GetOptionalChildXmlValByName(&m_iMaxWarRand, L"iMaxWarRand");
	pXML->GetOptionalChildXmlValByName(&m_iMaxWarNearbyPowerRatio, L"iMaxWarNearbyPowerRatio");
	pXML->GetOptionalChildXmlValByName(&m_iMaxWarDistantPowerRatio, L"iMaxWarDistantPowerRatio");
	pXML->GetOptionalChildXmlValByName(&m_iMaxWarMinAdjacentLandPercent, L"iMaxWarMinAdjacentLandPercent");
	pXML->GetOptionalChildXmlValByName(&m_iLimitedWarRand, L"iLimitedWarRand");
	pXML->GetOptionalChildXmlValByName(&m_iLimitedWarPowerRatio, L"iLimitedWarPowerRatio");
	pXML->GetOptionalChildXmlValByName(&m_iDogpileWarRand, L"iDogpileWarRand");
	pXML->GetOptionalChildXmlValByName(&m_iMakePeaceRand, L"iMakePeaceRand");
	pXML->GetOptionalChildXmlValByName(&m_iDeclareWarTradeRand, L"iDeclareWarTradeRand");
	pXML->GetOptionalChildXmlValByName(&m_iDemandRebukedSneakProb, L"iDemandRebukedSneakProb");
	pXML->GetOptionalChildXmlValByName(&m_iDemandRebukedWarProb, L"iDemandRebukedWarProb");
	pXML->GetOptionalChildXmlValByName(&m_iRazeCityProb, L"iRazeCityProb");
	pXML->GetOptionalChildXmlValByName(&m_iBuildUnitProb, L"iBuildUnitProb");
	pXML->GetOptionalChildXmlValByName(&m_iBaseAttackOddsChange, L"iBaseAttackOddsChange");
	pXML->GetOptionalChildXmlValByName(&m_iAttackOddsChangeRand, L"iAttackOddsChangeRand");
	pXML->GetOptionalChildXmlValByName(&m_iWorseRankDifferenceAttitudeChange, L"iWorseRankDifferenceAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iBetterRankDifferenceAttitudeChange, L"iBetterRankDifferenceAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iCloseBordersAttitudeChange, L"iCloseBordersAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iLostWarAttitudeChange, L"iLostWarAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iAtWarAttitudeDivisor, L"iAtWarAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iAtWarAttitudeChangeLimit, L"iAtWarAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iAtPeaceAttitudeDivisor, L"iAtPeaceAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iAtPeaceAttitudeChangeLimit, L"iAtPeaceAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iSameReligionAttitudeChange, L"iSameReligionAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iSameReligionAttitudeDivisor, L"iSameReligionAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iSameReligionAttitudeChangeLimit, L"iSameReligionAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iDifferentReligionAttitudeChange, L"iDifferentReligionAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iDifferentReligionAttitudeDivisor, L"iDifferentReligionAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iDifferentReligionAttitudeChangeLimit, L"iDifferentReligionAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iBonusTradeAttitudeDivisor, L"iBonusTradeAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iBonusTradeAttitudeChangeLimit, L"iBonusTradeAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iOpenBordersAttitudeDivisor, L"iOpenBordersAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iOpenBordersAttitudeChangeLimit, L"iOpenBordersAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iDefensivePactAttitudeDivisor, L"iDefensivePactAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iDefensivePactAttitudeChangeLimit, L"iDefensivePactAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iShareWarAttitudeChange, L"iShareWarAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iShareWarAttitudeDivisor, L"iShareWarAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iShareWarAttitudeChangeLimit, L"iShareWarAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iFavoriteCivicAttitudeChange, L"iFavoriteCivicAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iFavoriteCivicAttitudeDivisor, L"iFavoriteCivicAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iFavoriteCivicAttitudeChangeLimit, L"iFavoriteCivicAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iVassalPowerModifier, L"iVassalPowerModifier");
	pXML->GetOptionalChildXmlValByName(&m_iFreedomAppreciation, L"iFreedomAppreciation");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"DemandTributeAttitudeThreshold");
	m_iDemandTributeAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"NoGiveHelpAttitudeThreshold");
	m_iNoGiveHelpAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TechRefuseAttitudeThreshold");
	m_iTechRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"StrategicBonusRefuseAttitudeThreshold");
	m_iStrategicBonusRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"HappinessBonusRefuseAttitudeThreshold");
	m_iHappinessBonusRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"HealthBonusRefuseAttitudeThreshold");
	m_iHealthBonusRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"MapRefuseAttitudeThreshold");
	m_iMapRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"DeclareWarRefuseAttitudeThreshold");
	m_iDeclareWarRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"DeclareWarThemRefuseAttitudeThreshold");
	m_iDeclareWarThemRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"StopTradingRefuseAttitudeThreshold");
	m_iStopTradingRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"StopTradingThemRefuseAttitudeThreshold");
	m_iStopTradingThemRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"AdoptCivicRefuseAttitudeThreshold");
	m_iAdoptCivicRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ConvertReligionRefuseAttitudeThreshold");
	m_iConvertReligionRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"OpenBordersRefuseAttitudeThreshold");
	m_iOpenBordersRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"DefensivePactRefuseAttitudeThreshold");
	m_iDefensivePactRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PermanentAllianceRefuseAttitudeThreshold");
	m_iPermanentAllianceRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"VassalRefuseAttitudeThreshold");
	m_iVassalRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FavoriteCivic");
	m_iFavoriteCivic = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FavoriteReligion");
	m_iFavoriteReligion = pXML->GetInfoClass(szTextVal);

	pXML->SetVariableListTagPair(&m_pbTraits, L"Traits", GC.getNumTraitInfos());

	pXML->SetVariableListTagPair(&m_piFlavorValue, L"Flavors", GC.getNumFlavorTypes());
	pXML->SetVariableListTagPair(&m_piContactRand, L"ContactRands", NUM_CONTACT_TYPES);
	pXML->SetVariableListTagPair(&m_piContactDelay, L"ContactDelays", NUM_CONTACT_TYPES);
	pXML->SetVariableListTagPair(&m_piMemoryDecayRand, L"MemoryDecays", NUM_MEMORY_TYPES);
	pXML->SetVariableListTagPair(&m_piMemoryAttitudePercent, L"MemoryAttitudePercents", NUM_MEMORY_TYPES);
	pXML->SetVariableListTagPair(&m_piNoWarAttitudeProb, L"NoWarAttitudeProbs", NUM_ATTITUDE_TYPES);
	pXML->SetVariableListTagPair(&m_piUnitAIWeightModifier, L"UnitAIWeightModifiers", NUM_UNITAI_TYPES);
	pXML->SetVariableListTagPair(&m_piImprovementWeightModifier, L"ImprovementWeightModifiers", GC.getNumImprovementInfos());
	pXML->SetVariableListTagPairForAudioScripts(&m_piDiploPeaceIntroMusicScriptIds, L"DiplomacyIntroMusicPeace", GC.getNumEraInfos());
	pXML->SetVariableListTagPairForAudioScripts(&m_piDiploPeaceMusicScriptIds, L"DiplomacyMusicPeace", GC.getNumEraInfos());
	pXML->SetVariableListTagPairForAudioScripts(&m_piDiploWarIntroMusicScriptIds, L"DiplomacyIntroMusicWar", GC.getNumEraInfos());
	pXML->SetVariableListTagPairForAudioScripts(&m_piDiploWarMusicScriptIds, L"DiplomacyMusicWar", GC.getNumEraInfos());

	pXML->GetOptionalChildXmlValByName(szTextVal, L"MilitaryUnitRefuseAttitudeThreshold", "ATTITUDE_ANNOYED");
	m_iMilitaryUnitRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"WorkerRefuseAttitudeThreshold", "ATTITUDE_ANNOYED");
	m_iWorkerRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CorporationRefuseAttitudeThreshold", "ATTITUDE_CAUTIOUS");
	m_iCorporationRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SecretaryGeneralVoteRefuseAttitudeThreshold", "ATTITUDE_ANNOYED");
	m_iSecretaryGeneralVoteRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	//Int list Vector without delayed resolution
	pXML->SetOptionalVector(&m_aiDefaultTraits, L"DefaultTraits");
	pXML->SetOptionalVector(&m_aiDefaultComplexTraits, L"DefaultComplexTraits");

	setDefaultMemoryInfo();
	setDefaultContactInfo();

	return true;
}


void CvLeaderHeadInfo::copyNonDefaults(const CvLeaderHeadInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	CvString cDefault = CvString::format("").GetCString();

	//Art files must be reread first!
	if (getArtDefineTag() == cDefault) m_szArtDefineTag = pClassInfo->getArtDefineTag();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (!isNPC()) m_bNPC = pClassInfo->isNPC();
	if (getWonderConstructRand() == 0) m_iWonderConstructRand = pClassInfo->getWonderConstructRand();
	if (getBaseAttitude() == 0) m_iBaseAttitude = pClassInfo->getBaseAttitude();
	if (getBasePeaceWeight() == 0) m_iBasePeaceWeight = pClassInfo->getBasePeaceWeight();
	if (getPeaceWeightRand() == 0) m_iPeaceWeightRand = pClassInfo->getPeaceWeightRand();
	if (getWarmongerRespect() == 0) m_iWarmongerRespect = pClassInfo->getWarmongerRespect();
	if (getEspionageWeight() == 0) m_iEspionageWeight = pClassInfo->getEspionageWeight();
	if (getRefuseToTalkWarThreshold() == 0) m_iRefuseToTalkWarThreshold = pClassInfo->getRefuseToTalkWarThreshold();
	if (getNoTechTradeThreshold() == 0) m_iNoTechTradeThreshold = pClassInfo->getNoTechTradeThreshold();
	if (getTechTradeKnownPercent() == 0) m_iTechTradeKnownPercent = pClassInfo->getTechTradeKnownPercent();
	if (getMaxGoldTradePercent() == 0) m_iMaxGoldTradePercent = pClassInfo->getMaxGoldTradePercent();
	if (getMaxGoldPerTurnTradePercent() == 0) m_iMaxGoldPerTurnTradePercent = pClassInfo->getMaxGoldPerTurnTradePercent();
	if (getMaxWarRand() == 0) m_iMaxWarRand = pClassInfo->getMaxWarRand();
	if (getMaxWarNearbyPowerRatio() == 0) m_iMaxWarNearbyPowerRatio = pClassInfo->getMaxWarNearbyPowerRatio();
	if (getMaxWarDistantPowerRatio() == 0) m_iMaxWarDistantPowerRatio = pClassInfo->getMaxWarDistantPowerRatio();
	if (getMaxWarMinAdjacentLandPercent() == 0) m_iMaxWarMinAdjacentLandPercent = pClassInfo->getMaxWarMinAdjacentLandPercent();
	if (getLimitedWarRand() == 0) m_iLimitedWarRand = pClassInfo->getLimitedWarRand();
	if (getLimitedWarPowerRatio() == 0) m_iLimitedWarPowerRatio = pClassInfo->getLimitedWarPowerRatio();
	if (getDogpileWarRand() == 0) m_iDogpileWarRand = pClassInfo->getDogpileWarRand();
	if (getMakePeaceRand() == 0) m_iMakePeaceRand = pClassInfo->getMakePeaceRand();
	if (getDeclareWarTradeRand() == 0) m_iDeclareWarTradeRand = pClassInfo->getDeclareWarTradeRand();
	if (getDemandRebukedSneakProb() == 0) m_iDemandRebukedSneakProb = pClassInfo->getDemandRebukedSneakProb();
	if (getDemandRebukedWarProb() == 0) m_iDemandRebukedWarProb = pClassInfo->getDemandRebukedWarProb();
	if (getRazeCityProb() == 0) m_iRazeCityProb = pClassInfo->getRazeCityProb();
	if (getBuildUnitProb() == 0) m_iBuildUnitProb = pClassInfo->getBuildUnitProb();
	if (getBaseAttackOddsChange() == 0) m_iBaseAttackOddsChange = pClassInfo->getBaseAttackOddsChange();
	if (getAttackOddsChangeRand() == 0) m_iAttackOddsChangeRand = pClassInfo->getAttackOddsChangeRand();
	if (getWorseRankDifferenceAttitudeChange() == 0) m_iWorseRankDifferenceAttitudeChange = pClassInfo->getWorseRankDifferenceAttitudeChange();
	if (getBetterRankDifferenceAttitudeChange() == 0) m_iBetterRankDifferenceAttitudeChange = pClassInfo->getBetterRankDifferenceAttitudeChange();
	if (getCloseBordersAttitudeChange() == 0) m_iCloseBordersAttitudeChange = pClassInfo->getCloseBordersAttitudeChange();
	if (getLostWarAttitudeChange() == 0) m_iLostWarAttitudeChange = pClassInfo->getLostWarAttitudeChange();
	if (getAtWarAttitudeDivisor() == 0) m_iAtWarAttitudeDivisor = pClassInfo->getAtWarAttitudeDivisor();
	if (getAtWarAttitudeChangeLimit() == 0) m_iAtWarAttitudeChangeLimit = pClassInfo->getAtWarAttitudeChangeLimit();
	if (getAtPeaceAttitudeDivisor() == 0) m_iAtPeaceAttitudeDivisor = pClassInfo->getAtPeaceAttitudeDivisor();
	if (getAtPeaceAttitudeChangeLimit() == 0) m_iAtPeaceAttitudeChangeLimit = pClassInfo->getAtPeaceAttitudeChangeLimit();
	if (getSameReligionAttitudeChange() == 0) m_iSameReligionAttitudeChange = pClassInfo->getSameReligionAttitudeChange();
	if (getSameReligionAttitudeDivisor() == 0) m_iSameReligionAttitudeDivisor = pClassInfo->getSameReligionAttitudeDivisor();
	if (getSameReligionAttitudeChangeLimit() == 0) m_iSameReligionAttitudeChangeLimit = pClassInfo->getSameReligionAttitudeChangeLimit();
	if (getDifferentReligionAttitudeChange() == 0) m_iDifferentReligionAttitudeChange = pClassInfo->getDifferentReligionAttitudeChange();
	if (getDifferentReligionAttitudeDivisor() == 0) m_iDifferentReligionAttitudeDivisor = pClassInfo->getDifferentReligionAttitudeDivisor();
	if (getDifferentReligionAttitudeChangeLimit() == 0) m_iDifferentReligionAttitudeChangeLimit = pClassInfo->getDifferentReligionAttitudeChangeLimit();
	if (getBonusTradeAttitudeDivisor() == 0) m_iBonusTradeAttitudeDivisor = pClassInfo->getBonusTradeAttitudeDivisor();
	if (getBonusTradeAttitudeChangeLimit() == 0) m_iBonusTradeAttitudeChangeLimit = pClassInfo->getBonusTradeAttitudeChangeLimit();
	if (getOpenBordersAttitudeDivisor() == 0) m_iOpenBordersAttitudeDivisor = pClassInfo->getOpenBordersAttitudeDivisor();
	if (getOpenBordersAttitudeChangeLimit() == 0) m_iOpenBordersAttitudeChangeLimit = pClassInfo->getOpenBordersAttitudeChangeLimit();
	if (getDefensivePactAttitudeDivisor() == 0) m_iDefensivePactAttitudeDivisor = pClassInfo->getDefensivePactAttitudeDivisor();
	if (getDefensivePactAttitudeChangeLimit() == 0) m_iDefensivePactAttitudeChangeLimit = pClassInfo->getDefensivePactAttitudeChangeLimit();
	if (getShareWarAttitudeChange() == 0) m_iShareWarAttitudeChange = pClassInfo->getShareWarAttitudeChange();
	if (getShareWarAttitudeDivisor() == 0) m_iShareWarAttitudeDivisor = pClassInfo->getShareWarAttitudeDivisor();
	if (getShareWarAttitudeChangeLimit() == 0) m_iShareWarAttitudeChangeLimit = pClassInfo->getShareWarAttitudeChangeLimit();
	if (getFavoriteCivicAttitudeChange() == 0) m_iFavoriteCivicAttitudeChange = pClassInfo->getFavoriteCivicAttitudeChange();
	if (getFavoriteCivicAttitudeDivisor() == 0) m_iFavoriteCivicAttitudeDivisor = pClassInfo->getFavoriteCivicAttitudeDivisor();
	if (getFavoriteCivicAttitudeChangeLimit() == 0) m_iFavoriteCivicAttitudeChangeLimit = pClassInfo->getFavoriteCivicAttitudeChangeLimit();
	if (getVassalPowerModifier() == 0) m_iVassalPowerModifier = pClassInfo->getVassalPowerModifier();
	if (getFreedomAppreciation() == 0) m_iFreedomAppreciation = pClassInfo->getFreedomAppreciation();

	if (getDemandTributeAttitudeThreshold() == -1) m_iDemandTributeAttitudeThreshold = pClassInfo->getDemandTributeAttitudeThreshold();
	if (getNoGiveHelpAttitudeThreshold() == -1) m_iNoGiveHelpAttitudeThreshold = pClassInfo->getNoGiveHelpAttitudeThreshold();
	if (getTechRefuseAttitudeThreshold() == -1) m_iTechRefuseAttitudeThreshold = pClassInfo->getTechRefuseAttitudeThreshold();
	if (getStrategicBonusRefuseAttitudeThreshold() == -1) m_iStrategicBonusRefuseAttitudeThreshold = pClassInfo->getStrategicBonusRefuseAttitudeThreshold();
	if (getHappinessBonusRefuseAttitudeThreshold() == -1) m_iHappinessBonusRefuseAttitudeThreshold = pClassInfo->getHappinessBonusRefuseAttitudeThreshold();
	if (getHealthBonusRefuseAttitudeThreshold() == -1) m_iHealthBonusRefuseAttitudeThreshold = pClassInfo->getHealthBonusRefuseAttitudeThreshold();
	if (getMapRefuseAttitudeThreshold() == -1) m_iMapRefuseAttitudeThreshold = pClassInfo->getMapRefuseAttitudeThreshold();
	if (getDeclareWarRefuseAttitudeThreshold() == -1) m_iDeclareWarRefuseAttitudeThreshold = pClassInfo->getDeclareWarRefuseAttitudeThreshold();
	if (getDeclareWarThemRefuseAttitudeThreshold() == -1) m_iDeclareWarThemRefuseAttitudeThreshold = pClassInfo->getDeclareWarThemRefuseAttitudeThreshold();
	if (getStopTradingRefuseAttitudeThreshold() == -1) m_iStopTradingRefuseAttitudeThreshold = pClassInfo->getStopTradingRefuseAttitudeThreshold();
	if (getStopTradingThemRefuseAttitudeThreshold() == -1) m_iStopTradingThemRefuseAttitudeThreshold = pClassInfo->getStopTradingThemRefuseAttitudeThreshold();
	if (getAdoptCivicRefuseAttitudeThreshold() == -1) m_iAdoptCivicRefuseAttitudeThreshold = pClassInfo->getAdoptCivicRefuseAttitudeThreshold();
	if (getConvertReligionRefuseAttitudeThreshold() == -1) m_iConvertReligionRefuseAttitudeThreshold = pClassInfo->getConvertReligionRefuseAttitudeThreshold();
	if (getOpenBordersRefuseAttitudeThreshold() == -1) m_iOpenBordersRefuseAttitudeThreshold = pClassInfo->getOpenBordersRefuseAttitudeThreshold();
	if (getDefensivePactRefuseAttitudeThreshold() == -1) m_iDefensivePactRefuseAttitudeThreshold = pClassInfo->getDefensivePactRefuseAttitudeThreshold();
	if (getPermanentAllianceRefuseAttitudeThreshold() == -1) m_iPermanentAllianceRefuseAttitudeThreshold = pClassInfo->getPermanentAllianceRefuseAttitudeThreshold();
	if (getVassalRefuseAttitudeThreshold() == -1) m_iVassalRefuseAttitudeThreshold = pClassInfo->getVassalRefuseAttitudeThreshold();
	if (getFavoriteCivic() == -1) m_iFavoriteCivic = pClassInfo->getFavoriteCivic();
	if (getFavoriteReligion() == -1) m_iFavoriteReligion = pClassInfo->getFavoriteReligion();

	for (int j = 0; j < GC.getNumTraitInfos(); j++)
	{
		if (!hasTrait(j) && pClassInfo->hasTrait(j))
		{
			if (!m_pbTraits)
			{
				CvXMLLoadUtility::InitList(&m_pbTraits, GC.getNumTraitInfos(), false);
			}
			m_pbTraits[j] = pClassInfo->hasTrait(j);
		}
	}

	for (int j = 0; j < GC.getNumFlavorTypes(); j++)
	{
		if (getFlavorValue(j) == 0 && pClassInfo->getFlavorValue(j) != 0)
		{
			if (!m_piFlavorValue)
			{
				CvXMLLoadUtility::InitList(&m_piFlavorValue,GC.getNumFlavorTypes(),0);
			}
			m_piFlavorValue[j] = pClassInfo->getFlavorValue(j);
		}
	}
	for (int j = 0; j < NUM_CONTACT_TYPES; j++)
	{
		if (m_piContactRand[j] == 0 && pClassInfo->getContactRand(j) != 0)
		{
			m_piContactRand[j] = pClassInfo->getContactRand(j);
		}
		if (m_piContactDelay[j] == 0 && pClassInfo->getContactDelay(j) != 0)
		{
			m_piContactDelay[j] = pClassInfo->getContactDelay(j);
		}
	}
	for (int j = 0; j < NUM_MEMORY_TYPES; j++)
	{
		if (m_piMemoryDecayRand[j] == 0 && pClassInfo->getMemoryDecayRand(j) != 0)
		{
			m_piMemoryDecayRand[j] = pClassInfo->getMemoryDecayRand(j);
		}
		if (m_piMemoryAttitudePercent[j] == 0 && pClassInfo->getMemoryAttitudePercent(j) != 0)
		{
			m_piMemoryAttitudePercent[j] = pClassInfo->getMemoryAttitudePercent(j);
		}
	}
	for (int j = 0; j < NUM_ATTITUDE_TYPES; j++)
	{
		if (getNoWarAttitudeProb(j) == 0 && pClassInfo->getNoWarAttitudeProb(j) != 0)
		{
			if (!m_piNoWarAttitudeProb)
			{
				CvXMLLoadUtility::InitList(&m_piNoWarAttitudeProb,NUM_ATTITUDE_TYPES,0);
			}
			m_piNoWarAttitudeProb[j] = pClassInfo->getNoWarAttitudeProb(j);
		}
	}
	for (int j = 0; j < NUM_UNITAI_TYPES; j++)
	{
		if (getUnitAIWeightModifier(j) == 0 && pClassInfo->getUnitAIWeightModifier(j) != 0)
		{
			if (!m_piUnitAIWeightModifier)
			{
				CvXMLLoadUtility::InitList(&m_piUnitAIWeightModifier,NUM_UNITAI_TYPES,0);
			}
			m_piUnitAIWeightModifier[j] = pClassInfo->getUnitAIWeightModifier(j);
		}
	}
	for (int j = 0; j < GC.getNumImprovementInfos(); j++)
	{
		if (getImprovementWeightModifier(j) == 0 && pClassInfo->getImprovementWeightModifier(j) != 0)
		{
			if (!m_piImprovementWeightModifier)
			{
				CvXMLLoadUtility::InitList(&m_piImprovementWeightModifier, GC.getNumImprovementInfos(),0);
			}
			m_piImprovementWeightModifier[j] = pClassInfo->getImprovementWeightModifier(j);
		}
	}

	for (int j = 0; j < GC.getNumEraInfos(); j++)
	{
		if (getDiploPeaceIntroMusicScriptIds(j) == -1 && pClassInfo->getDiploPeaceIntroMusicScriptIds(j) != -1)
		{
			if (!m_piDiploPeaceIntroMusicScriptIds)
			{
				CvXMLLoadUtility::InitList(&m_piDiploPeaceIntroMusicScriptIds, GC.getNumEraInfos(), -1);
			}
			m_piDiploPeaceIntroMusicScriptIds[j] = pClassInfo->getDiploPeaceIntroMusicScriptIds(j);
		}
		if (getDiploPeaceMusicScriptIds(j) == -1 && pClassInfo->getDiploPeaceMusicScriptIds(j) != -1)
		{
			if (!m_piDiploPeaceMusicScriptIds)
			{
				CvXMLLoadUtility::InitList(&m_piDiploPeaceMusicScriptIds, GC.getNumEraInfos(), -1);
			}
			m_piDiploPeaceMusicScriptIds[j] = pClassInfo->getDiploPeaceMusicScriptIds(j);
		}
		if (getDiploWarIntroMusicScriptIds(j) == -1 && pClassInfo->getDiploWarIntroMusicScriptIds(j) != -1)
		{
			if (!m_piDiploWarIntroMusicScriptIds)
			{
				CvXMLLoadUtility::InitList(&m_piDiploWarIntroMusicScriptIds, GC.getNumEraInfos(), -1);
			}
			m_piDiploWarIntroMusicScriptIds[j] = pClassInfo->getDiploWarIntroMusicScriptIds(j);
		}
		if (getDiploWarMusicScriptIds(j) == -1 && pClassInfo->getDiploWarMusicScriptIds(j) != -1)
		{
			if (!m_piDiploWarMusicScriptIds)
			{
				CvXMLLoadUtility::InitList(&m_piDiploWarMusicScriptIds, GC.getNumEraInfos(), -1);
			}
			m_piDiploWarMusicScriptIds[j] = pClassInfo->getDiploWarMusicScriptIds(j);
		}
	}

	if (getMilitaryUnitRefuseAttitudeThreshold() == ATTITUDE_ANNOYED) m_iMilitaryUnitRefuseAttitudeThreshold = pClassInfo->getMilitaryUnitRefuseAttitudeThreshold();
	if (getWorkerRefuseAttitudeThreshold() == ATTITUDE_ANNOYED) m_iWorkerRefuseAttitudeThreshold = pClassInfo->getWorkerRefuseAttitudeThreshold();
	if (getCorporationRefuseAttitudeThreshold() == ATTITUDE_CAUTIOUS) m_iCorporationRefuseAttitudeThreshold = pClassInfo->getCorporationRefuseAttitudeThreshold();
	if (getSecretaryGeneralVoteRefuseAttitudeThreshold() == ATTITUDE_ANNOYED) m_iSecretaryGeneralVoteRefuseAttitudeThreshold = pClassInfo->getSecretaryGeneralVoteRefuseAttitudeThreshold();
	if (getCultureVictoryWeight() == 0) m_iCultureVictoryWeight = pClassInfo->getCultureVictoryWeight();
	if (getSpaceVictoryWeight() == 0) m_iSpaceVictoryWeight = pClassInfo->getSpaceVictoryWeight();
	if (getConquestVictoryWeight() == 0) m_iConquestVictoryWeight = pClassInfo->getConquestVictoryWeight();
	if (getDominationVictoryWeight() == 0) m_iDominationVictoryWeight = pClassInfo->getDominationVictoryWeight();
	if (getDiplomacyVictoryWeight() == 0) m_iDiplomacyVictoryWeight = pClassInfo->getDiplomacyVictoryWeight();

	//Int list Vector without delayed resolution
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiDefaultTraits, pClassInfo->m_aiDefaultTraits);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiDefaultComplexTraits, pClassInfo->m_aiDefaultComplexTraits);
}



//I'm lazy, so sue me. The XML still overrides this, so no worries.
void CvLeaderHeadInfo::setDefaultMemoryInfo()
{
	PROFILE_EXTRA_FUNC();

	for (int i = 0; i < NUM_MEMORY_TYPES; i++)
	{
		if (m_piMemoryDecayRand[i] == 0)
		{
			switch (i)
			{
				case MEMORY_WARMONGER:
				case MEMORY_MADE_PEACE:
				{
					m_piMemoryDecayRand[i] = 1;
					break;
				}
				case MEMORY_RECALLED_AMBASSADOR:
				{
					m_piMemoryDecayRand[i] = 25;
					break;
				}
				case MEMORY_INQUISITION:
				{
					m_piMemoryDecayRand[i] = 75;
					break;
				}
				case MEMORY_ENSLAVED_CITIZENS:
				{
					m_piMemoryDecayRand[i] = 100;
					break;
				}
				case MEMORY_SACKED_CITY:
				{
					m_piMemoryDecayRand[i] = 125;
					break;
				}
				case MEMORY_SACKED_HOLY_CITY:
				{
					m_piMemoryDecayRand[i] = 200;
					break;
				}
				case MEMORY_BACKSTAB:
				case MEMORY_BACKSTAB_FRIEND:
				{
					m_piMemoryDecayRand[i] = 250;
				}
			}
		}
		if (m_piMemoryAttitudePercent[i] == 0)
		{
			switch (i)
			{
				case MEMORY_INQUISITION:
				{
					m_piMemoryAttitudePercent[i] = -100;
					break;
				}
				case MEMORY_BACKSTAB_FRIEND:
				{
					m_piMemoryAttitudePercent[i] = -150;
					break;
				}
				case MEMORY_SACKED_CITY:
				case MEMORY_ENSLAVED_CITIZENS:
				{
					m_piMemoryAttitudePercent[i] = -200;
					break;
				}
				case MEMORY_SACKED_HOLY_CITY:
				case MEMORY_BACKSTAB:
				{
					m_piMemoryAttitudePercent[i] = -400;
				}
			}
		}
	}
}


void CvLeaderHeadInfo::setDefaultContactInfo()
{
	PROFILE_EXTRA_FUNC();

	for (int i = 0; i < NUM_CONTACT_TYPES; i++)
	{
		if (m_piContactRand[i] == 0)
		{
			switch (i)
			{
				case CONTACT_TRADE_JOIN_WAR:
				case CONTACT_TRADE_BUY_WAR:
				{
					m_piContactRand[i] = 10;
					break;
				}
				case CONTACT_TRADE_CONTACTS:
				{
					m_piContactRand[i] = 15;
					break;
				}
				case CONTACT_TRADE_STOP_TRADING:
				case CONTACT_TRADE_MILITARY_UNITS:
				{
					m_piContactRand[i] = 20;
					break;
				}
				case CONTACT_EMBASSY:
				case CONTACT_SECRETARY_GENERAL_VOTE:
				case CONTACT_TRADE_WORKERS:
				{
					m_piContactRand[i] = 25;
					break;
				}
				case CONTACT_TRADE_CORPORATION:
				{
					m_piContactRand[i] = 35;
					break;
				}
				case CONTACT_PEACE_PRESSURE:
				{
					m_piContactRand[i] = 50;
				}
			}
		}
		if (m_piContactDelay[i] == 0)
		{
			switch (i)
			{
				case CONTACT_TRADE_BUY_WAR:
				{
					m_piContactDelay[i] = 10;
					break;
				}
				case CONTACT_EMBASSY:
				case CONTACT_TRADE_JOIN_WAR:
				case CONTACT_TRADE_CONTACTS:
				case CONTACT_TRADE_STOP_TRADING:
				{
					m_piContactDelay[i] = 20;
					break;
				}
				case CONTACT_SECRETARY_GENERAL_VOTE:
				case CONTACT_TRADE_MILITARY_UNITS:
				{
					m_piContactDelay[i] = 25;
					break;
				}
				case CONTACT_PEACE_PRESSURE:
				case CONTACT_TRADE_WORKERS:
				{
					m_piContactDelay[i] = 30;
					break;
				}
				case CONTACT_TRADE_CORPORATION:
				{
					m_piContactDelay[i] = 50;
				}
			}
		}
	}
}

