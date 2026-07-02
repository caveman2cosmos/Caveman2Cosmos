#pragma once

#ifndef CV_LEADER_HEAD_INFO_H
#define CV_LEADER_HEAD_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvLeaderHeadInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoLeaderhead;
class CvLeaderHeadInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvLeaderHeadInfo();
	virtual ~CvLeaderHeadInfo();

	bool isNPC() const;
	int getWonderConstructRand() const;
	int getBaseAttitude() const;
	int getBasePeaceWeight() const;
	int getPeaceWeightRand() const;
	int getWarmongerRespect() const;
	int getEspionageWeight() const;
	int getRefuseToTalkWarThreshold() const;
	int getNoTechTradeThreshold() const;
	int getTechTradeKnownPercent() const;
	int getMaxGoldTradePercent() const;
	int getMaxGoldPerTurnTradePercent() const;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					  03/21/10								jdog5000	  */
/*																							  */
/* Victory Strategy AI																		  */
/************************************************************************************************/
	int getCultureVictoryWeight() const;
	int getSpaceVictoryWeight() const;
	int getConquestVictoryWeight() const;
	int getDominationVictoryWeight() const;
	int getDiplomacyVictoryWeight() const;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					   END												  */
/************************************************************************************************/
	int getMaxWarRand() const;
	int getMaxWarNearbyPowerRatio() const;
	int getMaxWarDistantPowerRatio() const;
	int getMaxWarMinAdjacentLandPercent() const;
	int getLimitedWarRand() const;
	int getLimitedWarPowerRatio() const;
	int getDogpileWarRand() const;
	int getMakePeaceRand() const;
	int getDeclareWarTradeRand() const;
	int getDemandRebukedSneakProb() const;
	int getDemandRebukedWarProb() const;
	int getRazeCityProb() const;
	int getBuildUnitProb() const;
	int getBaseAttackOddsChange() const;
	int getAttackOddsChangeRand() const;
	int getWorseRankDifferenceAttitudeChange() const;
	int getBetterRankDifferenceAttitudeChange() const;
	int getCloseBordersAttitudeChange() const;
	int getLostWarAttitudeChange() const;
	int getAtWarAttitudeDivisor() const;
	int getAtWarAttitudeChangeLimit() const;
	int getAtPeaceAttitudeDivisor() const;
	int getAtPeaceAttitudeChangeLimit() const;
	int getSameReligionAttitudeChange() const;
	int getSameReligionAttitudeDivisor() const;
	int getSameReligionAttitudeChangeLimit() const;
	int getDifferentReligionAttitudeChange() const;
	int getDifferentReligionAttitudeDivisor() const;
	int getDifferentReligionAttitudeChangeLimit() const;
	int getBonusTradeAttitudeDivisor() const;
	int getBonusTradeAttitudeChangeLimit() const;
	int getOpenBordersAttitudeDivisor() const;
	int getOpenBordersAttitudeChangeLimit() const;
	int getDefensivePactAttitudeDivisor() const;
	int getDefensivePactAttitudeChangeLimit() const;
	int getShareWarAttitudeChange() const;
	int getShareWarAttitudeDivisor() const;
	int getShareWarAttitudeChangeLimit() const;
	int getFavoriteCivicAttitudeChange() const;
	int getFavoriteCivicAttitudeDivisor() const;
	int getFavoriteCivicAttitudeChangeLimit() const;
	int getDemandTributeAttitudeThreshold() const;
	int getNoGiveHelpAttitudeThreshold() const;
	int getTechRefuseAttitudeThreshold() const;
	int getStrategicBonusRefuseAttitudeThreshold() const;
	int getHappinessBonusRefuseAttitudeThreshold() const;
	int getHealthBonusRefuseAttitudeThreshold() const;
	int getMapRefuseAttitudeThreshold() const;
	int getDeclareWarRefuseAttitudeThreshold() const;
	int getDeclareWarThemRefuseAttitudeThreshold() const;
	int getStopTradingRefuseAttitudeThreshold() const;
	int getStopTradingThemRefuseAttitudeThreshold() const;
	int getAdoptCivicRefuseAttitudeThreshold() const;
	int getConvertReligionRefuseAttitudeThreshold() const;
	int getOpenBordersRefuseAttitudeThreshold() const;
	int getDefensivePactRefuseAttitudeThreshold() const;
	int getPermanentAllianceRefuseAttitudeThreshold() const;
	int getVassalRefuseAttitudeThreshold() const;
	int getVassalPowerModifier() const;
	int getFavoriteCivic() const;
	int getFavoriteReligion() const;
	int getFreedomAppreciation() const;

	const char* getArtDefineTag() const;

	// Arrays
	bool hasTrait(int i) const;

	int getFlavorValue(int i) const;
	int getContactRand(int i) const;
	int getContactDelay(int i) const;
	int getMemoryDecayRand(int i) const;
	int getMemoryAttitudePercent(int i) const;
	int getNoWarAttitudeProb(int i) const;
	int getUnitAIWeightModifier(int i) const;
	int getImprovementWeightModifier(int i) const;
	int getDiploPeaceIntroMusicScriptIds(int i) const;
	int getDiploPeaceMusicScriptIds(int i) const;
	int getDiploWarIntroMusicScriptIds(int i) const;
	int getDiploWarMusicScriptIds(int i) const;

	// Other
	int getWorkerRefuseAttitudeThreshold() const;
	int getMilitaryUnitRefuseAttitudeThreshold() const;
	int getCorporationRefuseAttitudeThreshold() const;
	int getSecretaryGeneralVoteRefuseAttitudeThreshold() const;
	void setCultureVictoryWeight(int i);
	void setSpaceVictoryWeight(int i);
	void setConquestVictoryWeight(int i);
	void setDominationVictoryWeight(int i);
	void setDiplomacyVictoryWeight(int i);


	//Int list Vector without delayed resolution

	int getDefaultTrait(int i) const;
	int getNumDefaultTraits() const;
	bool isDefaultTrait(int i) const;

	int getDefaultComplexTrait(int i) const;
	int getNumDefaultComplexTraits() const;
	bool isDefaultComplexTrait(int i) const;

protected:
	int m_iMilitaryUnitRefuseAttitudeThreshold;
	int m_iWorkerRefuseAttitudeThreshold;
	int m_iCorporationRefuseAttitudeThreshold;
	int m_iSecretaryGeneralVoteRefuseAttitudeThreshold;
	void setDefaultMemoryInfo();
	void setDefaultContactInfo();
public:
	DllExport const CvArtInfoLeaderhead* getArtInfo() const;
	const char* getLeaderHead() const;
	const char* getButton() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvLeaderHeadInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	bool m_bNPC;

	int m_iWonderConstructRand;
	int m_iBaseAttitude;
	int m_iBasePeaceWeight;
	int m_iPeaceWeightRand;
	int m_iWarmongerRespect;
	int m_iEspionageWeight;
	int m_iRefuseToTalkWarThreshold;
	int m_iNoTechTradeThreshold;
	int m_iTechTradeKnownPercent;
	int m_iMaxGoldTradePercent;
	int m_iMaxGoldPerTurnTradePercent;

	int m_iCultureVictoryWeight;
	int m_iSpaceVictoryWeight;
	int m_iConquestVictoryWeight;
	int m_iDominationVictoryWeight;
	int m_iDiplomacyVictoryWeight;

	int m_iMaxWarRand;
	int m_iMaxWarNearbyPowerRatio;
	int m_iMaxWarDistantPowerRatio;
	int m_iMaxWarMinAdjacentLandPercent;
	int m_iLimitedWarRand;
	int m_iLimitedWarPowerRatio;
	int m_iDogpileWarRand;
	int m_iMakePeaceRand;
	int m_iDeclareWarTradeRand;
	int m_iDemandRebukedSneakProb;
	int m_iDemandRebukedWarProb;
	int m_iRazeCityProb;
	int m_iBuildUnitProb;
	int m_iBaseAttackOddsChange;
	int m_iAttackOddsChangeRand;
	int m_iWorseRankDifferenceAttitudeChange;
	int m_iBetterRankDifferenceAttitudeChange;
	int m_iCloseBordersAttitudeChange;
	int m_iLostWarAttitudeChange;
	int m_iAtWarAttitudeDivisor;
	int m_iAtWarAttitudeChangeLimit;
	int m_iAtPeaceAttitudeDivisor;
	int m_iAtPeaceAttitudeChangeLimit;
	int m_iSameReligionAttitudeChange;
	int m_iSameReligionAttitudeDivisor;
	int m_iSameReligionAttitudeChangeLimit;
	int m_iDifferentReligionAttitudeChange;
	int m_iDifferentReligionAttitudeDivisor;
	int m_iDifferentReligionAttitudeChangeLimit;
	int m_iBonusTradeAttitudeDivisor;
	int m_iBonusTradeAttitudeChangeLimit;
	int m_iOpenBordersAttitudeDivisor;
	int m_iOpenBordersAttitudeChangeLimit;
	int m_iDefensivePactAttitudeDivisor;
	int m_iDefensivePactAttitudeChangeLimit;
	int m_iShareWarAttitudeChange;
	int m_iShareWarAttitudeDivisor;
	int m_iShareWarAttitudeChangeLimit;
	int m_iFavoriteCivicAttitudeChange;
	int m_iFavoriteCivicAttitudeDivisor;
	int m_iFavoriteCivicAttitudeChangeLimit;
	int m_iDemandTributeAttitudeThreshold;
	int m_iNoGiveHelpAttitudeThreshold;
	int m_iTechRefuseAttitudeThreshold;
	int m_iStrategicBonusRefuseAttitudeThreshold;
	int m_iHappinessBonusRefuseAttitudeThreshold;
	int m_iHealthBonusRefuseAttitudeThreshold;
	int m_iMapRefuseAttitudeThreshold;
	int m_iDeclareWarRefuseAttitudeThreshold;
	int m_iDeclareWarThemRefuseAttitudeThreshold;
	int m_iStopTradingRefuseAttitudeThreshold;
	int m_iStopTradingThemRefuseAttitudeThreshold;
	int m_iAdoptCivicRefuseAttitudeThreshold;
	int m_iConvertReligionRefuseAttitudeThreshold;
	int m_iOpenBordersRefuseAttitudeThreshold;
	int m_iDefensivePactRefuseAttitudeThreshold;
	int m_iPermanentAllianceRefuseAttitudeThreshold;
	int m_iVassalRefuseAttitudeThreshold;
	int m_iVassalPowerModifier;
	int m_iFreedomAppreciation;
	int m_iFavoriteCivic;
	int m_iFavoriteReligion;

	//Int list Vector without delayed resolution
	std::vector<int> m_aiDefaultTraits;
	std::vector<int> m_aiDefaultComplexTraits;


	CvString m_szArtDefineTag;

	// Arrays

	bool* m_pbTraits;

	int* m_piFlavorValue;
	int* m_piContactRand;
	int* m_piContactDelay;
	int* m_piMemoryDecayRand;
	int* m_piMemoryAttitudePercent;
	int* m_piNoWarAttitudeProb;
	int* m_piUnitAIWeightModifier;
	int* m_piImprovementWeightModifier;
	int* m_piDiploPeaceIntroMusicScriptIds;
	int* m_piDiploPeaceMusicScriptIds;
	int* m_piDiploWarIntroMusicScriptIds;
	int* m_piDiploWarMusicScriptIds;

};

#endif // CV_LEADER_HEAD_INFO_H
