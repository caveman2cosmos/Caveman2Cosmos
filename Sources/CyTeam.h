#pragma once

#ifndef CyTeam_h
#define CyTeam_h

//
// Python wrapper class for CvTeam
//

class CvTeam;
class CyArea;

class CyTeam
{
public:
	CyTeam();
	explicit CyTeam(CvTeam* pTeam);		// Call from C++

	//const CvTeam* getTeam() const { return m_pTeam; }	// Call from C++

	bool isRebel() const;
	bool isSingleCityTeam() const;
	bool isRebelAgainst(int iTeam) const;
	void setRebelAgainst(int iTeam, bool bNewValue);
	int countRebelAgainst() const;

	void addTeam(int /*TeamTypes*/ eTeam);

	bool canChangeWarPeace(int /*TeamTypes*/ eTeam) const;
	bool canDeclareWar(int /*TeamTypes*/ eTeam) const;
	void declareWar(int /*TeamTypes*/ eTeam, bool bNewDiplo, int /*WarPlanTypes*/ eWarPlan);
	void makePeace(int /*TeamTypes*/ eTeam);
	bool canContact(int /*TeamTypes*/ eTeam) const;
	void meet(int /*TeamTypes*/ eTeam, bool bNewDiplo);
	void signOpenBorders(int /*TeamTypes*/ eTeam);
	void signDefensivePact(int /*TeamTypes*/ eTeam);

	int getAssets() const;
	int getPower(bool bIncludeVassals) const;
	int64_t getTotalVictoryScore() const;

	int getAtWarCount(bool bIgnoreMinors) const;
	int getHasMetCivCount(bool bIgnoreMinors) const;
	bool isAVassal() const;

	int getBuildingCountPlusMaking(int /*BuildingTypes*/ eBuilding) const;
	int getUnitMaking(int /*UnitTypes*/ eUnit) const;
	int getUnitCountPlusMaking(int /*UnitTypes*/ eUnit) const;
	int getHasReligionCount(int /*ReligionTypes*/ eReligion) const;
	int getHasCorporationCount(int /*CorporationTypes*/ eCorporation) const;

	int64_t countTotalCulture() const;

	int countNumCitiesByArea(const CyArea* pArea) const;

	int getResearchCost(int /*TechTypes*/ eTech) const;

	bool hasHolyCity(int /*ReligionTypes*/ eReligion) const;
	bool hasHeadquarters(int /*CorporationTypes*/ eCorporation) const;

	bool isHuman() const;
	bool isNPC() const;
	bool isHominid() const;
	bool isMinorCiv() const;

	void setIsMinorCiv(bool bNewValue, bool bDoBarbCivCheck);

	int /*PlayerTypes*/ getLeaderID() const;
	int /*HandicapTypes*/ getHandicapType() const;
	std::wstring getName() const;

	int getNumMembers() const;
	bool isAlive() const;
	bool isEverAlive() const;
	int getNumCities() const;
	int getTotalPopulation() const;
	int getTotalLand() const;
	int getNukeInterception() const;
	void changeNukeInterception(int iChange);

	int getForceTeamVoteEligibilityCount(int /*VoteSourceTypes*/ eVoteSource) const;
	bool isForceTeamVoteEligible(int /*VoteSourceTypes*/ eVoteSource) const;
	void changeForceTeamVoteEligibilityCount(int /*VoteSourceTypes*/ eVoteSource, int iChange);
	int getExtraWaterSeeFromCount() const;
	bool isExtraWaterSeeFrom() const;
	void changeExtraWaterSeeFromCount(int iChange);
	int getMapTradingCount() const;
	bool isMapTrading() const;
	void changeMapTradingCount(int iChange);
	int getTechTradingCount() const;
	bool isTechTrading() const;
	void changeTechTradingCount(int iChange);
	int getGoldTradingCount() const;
	bool isGoldTrading() const;
	void changeGoldTradingCount(int iChange);
	int getOpenBordersTradingCount() const;
	bool isOpenBordersTrading() const;
	void changeOpenBordersTradingCount(int iChange);
	int getDefensivePactTradingCount() const;
	bool isDefensivePactTrading() const;
	void changeDefensivePactTradingCount(int iChange);
	int getPermanentAllianceTradingCount() const;
	bool isPermanentAllianceTrading() const;
	void changePermanentAllianceTradingCount(int iChange);
	int getVassalTradingCount() const;
	bool isVassalStateTrading() const;
	void changeVassalTradingCount(int iChange);
	int getBridgeBuildingCount() const;
	bool isBridgeBuilding() const;
	void changeBridgeBuildingCount(int iChange);
	int getIrrigationCount() const;
	bool isIrrigation() const;
	void changeIrrigationCount(int iChange);
	int getIgnoreIrrigationCount() const;
	bool isIgnoreIrrigation() const;
	void changeIgnoreIrrigationCount(int iChange);
	int getWaterWorkCount() const;
	bool isWaterWork() const;
	void changeWaterWorkCount(int iChange);

	int getVassalPower() const;
	void setVassalPower(int iPower);
	int getMasterPower() const;
	void setMasterPower(int iPower);

	int getEnemyWarWearinessModifier() const;
	void changeEnemyWarWearinessModifier(int iChange);

	bool isMapCentering() const;
	void setMapCentering(bool bNewValue);

	int getID() const;

	int getWarWeariness(int /*TeamTypes*/ eIndex) const;
	void changeWarWeariness(int /*TeamTypes*/ eIndex, int iChange);

	int getCommerceFlexibleCount(int /*CommerceTypes*/ eIndex) const;
	bool isCommerceFlexible(int /*CommerceTypes*/ eIndex) const;
	void changeCommerceFlexibleCount(int /*CommerceTypes*/ eIndex, int iChange);

	int getExtraMoves(int /*DomainTypes*/ eIndex) const;
	void changeExtraMoves(int /*DomainTypes*/ eIndex, int iChange);

	bool isHasMet(int /*TeamTypes*/ eIndex) const;
	bool isAtWar(int /*TeamTypes*/ eIndex) const;
	bool isPermanentWarPeace(int /*TeamTypes*/ eIndex) const;
	void setPermanentWarPeace(int /*TeamTypes*/ eIndex, bool bNewValue);

	bool isOpenBorders(int /*TeamTypes*/ eIndex) const;
	bool isForcePeace(int /*TeamTypes*/ eIndex) const;
	bool isVassal(int /*TeamTypes*/ eIndex) const;
	void assignVassal(int /*TeamTypes*/ eIndex, bool bSurrender);
	void freeVassal(int /*TeamTypes*/ eIndex);
	bool isDefensivePact(int /*TeamTypes*/ eIndex) const;
	int getRouteChange(int /*RouteTypes*/ eIndex) const;
	void changeRouteChange(int /*RouteTypes*/ eIndex, int iChange);
	int getProjectCount(int /*ProjectTypes*/ eIndex) const;
	int getProjectDefaultArtType(int /*ProjectTypes*/ eIndex) const;
	void setProjectArtType(int /*ProjectTypes*/ eIndex, int number, int value);
	void changeProjectCount(int /*ProjectTypes*/ eIndex, int iChange);
	int getProjectMaking(int /*ProjectTypes*/ eIndex) const;
	int getBuildingCount(int /*BuildingTypes*/ eIndex) const;
	bool isBuildingMaxedOut(int /*BuildingTypes*/ eIndex, int iExtra) const;
	int getUnitCount(int /*UnitTypes*/ eIndex) const;
	bool isUnitMaxedOut(int /*UnitTypes*/ eIndex, int iExtra) const;
	bool isObsoleteBuilding(int /*BuildingTypes*/ eIndex) const;

	int getResearchProgress(int /*TechTypes*/ eIndex) const;
	void setResearchProgress(int /*TechTypes*/ eIndex, int iNewValue, int /*PlayerTypes*/ ePlayer);
	void changeResearchProgress(int /*TechTypes*/ eIndex, int iChange, int /*PlayerTypes*/ ePlayer);
	int getTechCount(int /*TechTypes*/ eIndex) const;

	bool isTerrainTrade(int /*TerrainTypes*/ eIndex) const;
	bool isRiverTrade() const;
	bool isHasTech(int /*TechTypes*/ iIndex) const;
	void setHasTech(int /*TechTypes*/ eIndex, bool bNewValue, int /*PlayerTypes*/ ePlayer, bool bFirst, bool bAnnounce);
	bool isNoTradeTech(int /*TechType */ iIndex) const;

	int getImprovementYieldChange(int /*ImprovementTypes*/ eIndex, int /*YieldTypes*/ eIndex2) const;
	void changeImprovementYieldChange(int /*ImprovementTypes*/ eIndex1, int /*YieldTypes*/ eIndex2, int iChange);

	int getBuildingCommerceTechChange(int eIndex, int eBuilding) const;

	int getVictoryCountdown(int /*VictoryTypes*/ eVictory) const;
	int getVictoryDelay(int /*VictoryTypes*/ eVictory) const;
	bool canLaunch(int /*VictoryTypes*/ eVictory) const;
	int getLaunchSuccessRate(int /*VictoryTypes*/ eVictory) const;

	int getEspionagePointsAgainstTeam(int /*TeamTypes*/ eIndex) const;
	void setEspionagePointsAgainstTeam(int /*TeamTypes*/ eIndex, int iValue);
	void changeEspionagePointsAgainstTeam(int /*TeamTypes*/ eIndex, int iChange);
	int getEspionagePointsEver() const;
	void setEspionagePointsEver(int iValue);
	void changeEspionagePointsEver(int iChange);
	int getCounterespionageTurnsLeftAgainstTeam(int /*TeamTypes*/ eIndex) const;
	void changeCounterespionageTurnsLeftAgainstTeam(int /*TeamTypes*/ eIndex, int iChange);
	int getCounterespionageModAgainstTeam(int /*TeamTypes*/ eIndex) const;
	void changeCounterespionageModAgainstTeam(int /*TeamTypes*/ eIndex, int iChange);

	bool AI_shareWar(int /*TeamTypes*/ eTeam) const;
	int AI_getAtWarCounter(int /*TeamTypes*/ eTeam) const;
	int AI_getAtPeaceCounter(int /*TeamTypes*/ eTeam) const;
	int AI_getWarSuccess(int /*TeamTypes*/ eIndex) const;

	bool isHasEmbassy(int /*TeamTypes*/ eIndex) const;

protected:
	CvTeam* m_pTeam;
};

#endif	// #ifndef CyTeam
