#include "CvGameCoreDLL.h"
#include "CvPlayerAI.h"
#include "CvTeam.h"
#include "CyArea.h"
#include "CyTeam.h"

//
// Python wrapper class for CvTeam
//

CyTeam::CyTeam() : m_pTeam(NULL) {}

CyTeam::CyTeam(CvTeam* pTeam) : m_pTeam(pTeam) {}


bool CyTeam::isRebel() const
{
	return m_pTeam->isRebel();
}

bool CyTeam::isSingleCityTeam() const
{
	return m_pTeam->isSingleCityTeam();
}

bool CyTeam::isRebelAgainst(int iTeam) const
{
	return m_pTeam->isRebelAgainst((TeamTypes)iTeam);
}

void CyTeam::setRebelAgainst(int iTeam, bool bNewValue)
{
	m_pTeam->setRebelAgainst((TeamTypes)iTeam, bNewValue);
}

int CyTeam::countRebelAgainst() const
{
	return m_pTeam->countRebelAgainst();
}

void CyTeam::addTeam(int /*TeamTypes*/ eTeam)
{
	m_pTeam->addTeam((TeamTypes)eTeam);
}

bool CyTeam::canChangeWarPeace(int /*TeamTypes*/ eTeam) const
{
	return m_pTeam->canChangeWarPeace((TeamTypes)eTeam);
}

bool CyTeam::canDeclareWar(int /*TeamTypes*/ eTeam) const
{
	return m_pTeam->canDeclareWar((TeamTypes)eTeam);
}

void CyTeam::declareWar(int /*TeamTypes*/ eTeam, bool bNewDiplo, int /*WarPlanTypes*/ eWarPlan)
{
	m_pTeam->declareWar((TeamTypes)eTeam, bNewDiplo, (WarPlanTypes)eWarPlan);
}

void CyTeam::makePeace(int /*TeamTypes*/ eTeam)
{
	m_pTeam->makePeace((TeamTypes)eTeam);
}

bool CyTeam::canContact(int /*TeamTypes*/ eTeam) const
{
	return m_pTeam->canContact((TeamTypes)eTeam);
}

void CyTeam::meet(int /*TeamTypes*/ eTeam, bool bNewDiplo)
{
	m_pTeam->meet((TeamTypes)eTeam, bNewDiplo);
}

void CyTeam::signOpenBorders(int /*TeamTypes*/ eTeam)
{
	m_pTeam->signOpenBorders((TeamTypes)eTeam);
}

void CyTeam::signDefensivePact(int /*TeamTypes*/ eTeam)
{
	m_pTeam->signDefensivePact((TeamTypes)eTeam);
}

int CyTeam::getAssets() const
{
	return m_pTeam->getAssets();
}

int CyTeam::getPower(bool bIncludeVassals) const
{
	return m_pTeam->getPower(bIncludeVassals);
}

int64_t CyTeam::getTotalVictoryScore() const
{
	return m_pTeam->getTotalVictoryScore();
}

int CyTeam::getAtWarCount(bool bIgnoreMinors) const
{
	return m_pTeam->getAtWarCount(bIgnoreMinors);
}

int CyTeam::getHasMetCivCount(bool bIgnoreMinors) const
{
	return m_pTeam->getHasMetCivCount(bIgnoreMinors);
}

bool CyTeam::isAVassal() const
{
	return m_pTeam->isAVassal();
}

int CyTeam::getUnitMaking(int /*UnitTypes*/ eUnit) const
{
	return m_pTeam->getUnitMaking((UnitTypes)eUnit);
}

int CyTeam::getUnitCountPlusMaking(int /*UnitTypes*/ eUnit) const
{
	return m_pTeam->getUnitCountPlusMaking((UnitTypes)eUnit);
}

int CyTeam::getBuildingCountPlusMaking(int /*BuildingTypes*/ eBuilding) const
{
	return m_pTeam->getBuildingCountPlusMaking((BuildingTypes)eBuilding);
}

int CyTeam::getHasReligionCount(int /*ReligionTypes*/ eReligion) const
{
	return m_pTeam->getHasReligionCount((ReligionTypes)eReligion);
}

int CyTeam::getHasCorporationCount(int /*CorporationTypes*/ eReligion) const
{
	return m_pTeam->getHasCorporationCount((CorporationTypes)eReligion);
}

int64_t CyTeam::countTotalCulture() const
{
	return m_pTeam->countTotalCulture();
}

int CyTeam::countNumCitiesByArea(const CyArea* pArea) const
{
	return m_pTeam->countNumCitiesByArea(pArea->getArea());
}

int CyTeam::getResearchCost(int /*TechTypes*/ eTech) const
{
	return m_pTeam->getResearchCost((TechTypes)eTech);
}

bool CyTeam::hasHolyCity(int /*ReligionTypes*/ eReligion) const
{
	return m_pTeam->hasHolyCity((ReligionTypes)eReligion);
}

bool CyTeam::hasHeadquarters(int /*CorporationTypes*/ eCorporation) const
{
	return m_pTeam->hasHeadquarters((CorporationTypes)eCorporation);
}

bool CyTeam::isHuman() const
{
	return m_pTeam->isHuman();
}

bool CyTeam::isBarbarian() const
{
	return m_pTeam->isBarbarian();
}

bool CyTeam::isNPC() const
{
	return m_pTeam->isNPC();
}

bool CyTeam::isHominid() const
{
	return m_pTeam->isHominid();
}

bool CyTeam::isMinorCiv() const
{
	return m_pTeam->isMinorCiv();
}

void CyTeam::setIsMinorCiv(bool bNewValue, bool bDoBarbCivCheck)
{
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == m_pTeam->getID())
		{
			if (GET_PLAYER((PlayerTypes)iI).getCivilizationType() < 0)
			{
				FErrorMsg("GET_PLAYER((PlayerTypes)iI) of m_pTeam should have a civilizationType");
#ifdef _DEBUG
				throw new std::exception();
#endif
			}
		}

	}
	m_pTeam->setIsMinorCiv(bNewValue, bDoBarbCivCheck);
}

int /*PlayerTypes*/ CyTeam::getLeaderID() const
{
	return m_pTeam->getLeaderID();
}

int /*HandicapTypes*/ CyTeam::getHandicapType() const
{
	return m_pTeam->getHandicapType();
}

std::wstring CyTeam::getName() const
{
	return m_pTeam->getName();
}

int CyTeam::getNumMembers() const
{
	return m_pTeam->getNumMembers();
}

bool CyTeam::isAlive() const
{
	return m_pTeam->isAlive();
}

bool CyTeam::isEverAlive() const
{
	return m_pTeam->isEverAlive();
}

int CyTeam::getNumCities() const
{
	return m_pTeam->getNumCities();
}

int CyTeam::getTotalPopulation() const
{
	return m_pTeam->getTotalPopulation();
}

int CyTeam::getTotalLand() const
{
	return m_pTeam->getTotalLand();
}

int CyTeam::getNukeInterception() const
{
	return m_pTeam->getNukeInterception();
}

void CyTeam::changeNukeInterception(int iChange)
{
	m_pTeam->changeNukeInterception(iChange);
}

int CyTeam::getForceTeamVoteEligibilityCount(int /*VoteSourceTypes*/ eVoteSource) const
{
	return m_pTeam->getForceTeamVoteEligibilityCount((VoteSourceTypes)eVoteSource);
}

bool CyTeam::isForceTeamVoteEligible(int /*VoteSourceTypes*/ eVoteSource) const
{
	return m_pTeam->isForceTeamVoteEligible((VoteSourceTypes)eVoteSource);
}

void CyTeam::changeForceTeamVoteEligibilityCount(int /*VoteSourceTypes*/ eVoteSource, int iChange)
{
	m_pTeam->changeForceTeamVoteEligibilityCount((VoteSourceTypes)eVoteSource, iChange);
}

int CyTeam::getExtraWaterSeeFromCount() const
{
	return m_pTeam->getExtraWaterSeeFromCount();
}

bool CyTeam::isExtraWaterSeeFrom() const
{
	return m_pTeam->isExtraWaterSeeFrom();
}

void CyTeam::changeExtraWaterSeeFromCount(int iChange)
{
	m_pTeam->changeExtraWaterSeeFromCount(iChange);
}

int CyTeam::getMapTradingCount() const
{
	return m_pTeam->getMapTradingCount();
}

bool CyTeam::isMapTrading() const
{
	return m_pTeam->isMapTrading();
}

void CyTeam::changeMapTradingCount(int iChange)
{
	m_pTeam->changeMapTradingCount(iChange);
}

int CyTeam::getTechTradingCount() const
{
	return m_pTeam->getTechTradingCount();
}

bool CyTeam::isTechTrading() const
{
	return m_pTeam->isTechTrading();
}

void CyTeam::changeTechTradingCount(int iChange)
{
	m_pTeam->changeTechTradingCount(iChange);
}

int CyTeam::getGoldTradingCount() const
{
	return m_pTeam->getGoldTradingCount();
}

bool CyTeam::isGoldTrading() const
{
	return m_pTeam->isGoldTrading();
}

void CyTeam::changeGoldTradingCount(int iChange)
{
	m_pTeam->changeGoldTradingCount(iChange);
}

int CyTeam::getOpenBordersTradingCount() const
{
	return m_pTeam->getOpenBordersTradingCount();
}

bool CyTeam::isOpenBordersTrading() const
{
	return m_pTeam->isOpenBordersTrading();
}

void CyTeam::changeOpenBordersTradingCount(int iChange)
{
	m_pTeam->changeOpenBordersTradingCount(iChange);
}

int CyTeam::getDefensivePactTradingCount() const
{
	return m_pTeam->getDefensivePactTradingCount();
}

bool CyTeam::isDefensivePactTrading() const
{
	return m_pTeam->isDefensivePactTrading();
}

void CyTeam::changeDefensivePactTradingCount(int iChange)
{
	m_pTeam->changeDefensivePactTradingCount(iChange);
}

int CyTeam::getPermanentAllianceTradingCount() const
{
	return m_pTeam->getPermanentAllianceTradingCount();
}

bool CyTeam::isPermanentAllianceTrading() const
{
	return m_pTeam->isPermanentAllianceTrading();
}

void CyTeam::changePermanentAllianceTradingCount(int iChange)
{
	m_pTeam->changePermanentAllianceTradingCount(iChange);
}

int CyTeam::getVassalTradingCount() const
{
	return m_pTeam->getVassalTradingCount();
}

bool CyTeam::isVassalStateTrading() const
{
	return m_pTeam->isVassalStateTrading();
}

void CyTeam::changeVassalTradingCount(int iChange)
{
	m_pTeam->changeVassalTradingCount(iChange);
}

int CyTeam::getBridgeBuildingCount() const
{
	return m_pTeam->getBridgeBuildingCount();
}

bool CyTeam::isBridgeBuilding() const
{
	return m_pTeam->isBridgeBuilding();
}

void CyTeam::changeBridgeBuildingCount(int iChange)
{
	m_pTeam->changeBridgeBuildingCount(iChange);
}

int CyTeam::getIrrigationCount() const
{
	return m_pTeam->getIrrigationCount();
}

bool CyTeam::isIrrigation() const
{
	return m_pTeam->isIrrigation();
}

void CyTeam::changeIrrigationCount(int iChange)
{
	m_pTeam->changeIrrigationCount(iChange);
}

int CyTeam::getIgnoreIrrigationCount() const
{
	return m_pTeam->getIgnoreIrrigationCount();
}

bool CyTeam::isIgnoreIrrigation() const
{
	return m_pTeam->isIgnoreIrrigation();
}

void CyTeam::changeIgnoreIrrigationCount(int iChange)
{
	m_pTeam->changeIgnoreIrrigationCount(iChange);
}

int CyTeam::getWaterWorkCount() const
{
	return m_pTeam->getWaterWorkCount();
}

bool CyTeam::isWaterWork() const
{
	return m_pTeam->isWaterWork();
}

void CyTeam::changeWaterWorkCount(int iChange)
{
	m_pTeam->changeWaterWorkCount(iChange);
}

int CyTeam::getVassalPower() const
{
	return m_pTeam->getVassalPower();
}

void CyTeam::setVassalPower(int iPower)
{
	m_pTeam->setVassalPower(iPower);
}

int CyTeam::getMasterPower() const
{
	return m_pTeam->getMasterPower();
}

void CyTeam::setMasterPower(int iPower)
{
	m_pTeam->setMasterPower(iPower);
}

int CyTeam::getEnemyWarWearinessModifier() const
{
	return m_pTeam->getEnemyWarWearinessModifier();
}

void CyTeam::changeEnemyWarWearinessModifier(int iChange)
{
	m_pTeam->changeEnemyWarWearinessModifier(iChange);
}

bool CyTeam::isMapCentering() const
{
	return m_pTeam->isMapCentering();
}

void CyTeam::setMapCentering(bool bNewValue)
{
	m_pTeam->setMapCentering(bNewValue);
}

int CyTeam::getID() const
{
	return m_pTeam->getID();
}

int CyTeam::getWarWeariness(int /*TeamTypes*/ eIndex) const
{
	return m_pTeam->getWarWeariness((TeamTypes)eIndex);
}

void CyTeam::changeWarWeariness(int /*TeamTypes*/ eIndex, int iChange)
{
	m_pTeam->changeWarWeariness((TeamTypes)eIndex, iChange);
}

int CyTeam::getCommerceFlexibleCount(int /*CommerceTypes*/ eIndex) const
{
	return m_pTeam->getCommerceFlexibleCount((CommerceTypes)eIndex);
}

bool CyTeam::isCommerceFlexible(int /*CommerceTypes*/ eIndex) const
{
	return m_pTeam->isCommerceFlexible((CommerceTypes)eIndex);
}

void CyTeam::changeCommerceFlexibleCount(int /*CommerceTypes*/ eIndex, int iChange)
{
	m_pTeam->changeCommerceFlexibleCount((CommerceTypes)eIndex, iChange);
}

int CyTeam::getExtraMoves(int /*DomainTypes*/ eIndex) const
{
	return m_pTeam->getExtraMoves((DomainTypes)eIndex);
}

void CyTeam::changeExtraMoves(int /*DomainTypes*/ eIndex, int iChange)
{
	m_pTeam->changeExtraMoves((DomainTypes)eIndex, iChange);
}

bool CyTeam::isHasMet(int /*TeamTypes*/ eIndex) const
{
	//Fuyu: Catching Civ4lerts mess-ups
	FASSERT_BOUNDS(0, MAX_TEAMS, eIndex);
	if (eIndex < 0 || eIndex >= MAX_TEAMS)
	{
#ifdef _DEBUG
		throw new std::exception();
#endif
        return false;
	}

	return m_pTeam->isHasMet((TeamTypes)eIndex);
}

bool CyTeam::isAtWar(int /*TeamTypes*/ iIndex) const
{
	return iIndex != NO_TEAM ? m_pTeam->isAtWar((TeamTypes)iIndex) : false;
}

bool CyTeam::isPermanentWarPeace(int /*TeamTypes*/ eIndex) const
{
	return m_pTeam->isPermanentWarPeace((TeamTypes)eIndex);
}

void CyTeam::setPermanentWarPeace(int /*TeamTypes*/ eIndex, bool bNewValue)
{
	m_pTeam->setPermanentWarPeace((TeamTypes)eIndex, bNewValue);
}

bool CyTeam::isOpenBorders(int /*TeamTypes*/ eIndex) const
{
	return m_pTeam->isOpenBorders((TeamTypes)eIndex);
}

bool CyTeam::isForcePeace(int /*TeamTypes*/ eIndex) const
{
	return m_pTeam->isForcePeace((TeamTypes)eIndex);
}

bool CyTeam::isVassal(int /*TeamTypes*/ eIndex) const
{
	return m_pTeam->isVassal((TeamTypes)eIndex);
}

void CyTeam::assignVassal(int /*TeamTypes*/ eIndex, bool bSurrender)
{
	m_pTeam->assignVassal((TeamTypes)eIndex, bSurrender);
}

void CyTeam::freeVassal(int /*TeamTypes*/ eIndex)
{
	m_pTeam->freeVassal((TeamTypes)eIndex);
}

bool CyTeam::isDefensivePact(int /*TeamTypes*/ eIndex) const
{
	return m_pTeam->isDefensivePact((TeamTypes)eIndex);
}

int CyTeam::getRouteChange(int /*RouteTypes*/ eIndex) const
{
	return m_pTeam->getRouteChange((RouteTypes)eIndex);
}

void CyTeam::changeRouteChange(int /*RouteTypes*/ eIndex, int iChange)
{
	m_pTeam->changeRouteChange((RouteTypes)eIndex, iChange);
}

int CyTeam::getProjectCount(int /*ProjectTypes*/ eIndex) const
{
	return m_pTeam->getProjectCount((ProjectTypes)eIndex);
}

int CyTeam::getProjectDefaultArtType(int /*ProjectTypes*/ eIndex) const
{
	return m_pTeam->getProjectDefaultArtType((ProjectTypes)eIndex);
}

void CyTeam::setProjectArtType(int /*ProjectTypes*/ eIndex, int number, int value)
{
	m_pTeam->setProjectArtType((ProjectTypes)eIndex, number, value);
}

void CyTeam::changeProjectCount(int /*ProjectTypes*/ eIndex, int iChange)
{
	m_pTeam->changeProjectCount((ProjectTypes)eIndex, iChange);
}

int CyTeam::getProjectMaking(int /*ProjectTypes*/ eIndex) const
{
	return m_pTeam->getProjectMaking((ProjectTypes)eIndex);
}

int CyTeam::getUnitCount(int /*UnitTypes*/ eIndex) const
{
	return m_pTeam->getUnitCount((UnitTypes)eIndex);
}

bool CyTeam::isUnitMaxedOut(int /*UnitTypes*/ eIndex, int iExtra) const
{
	return m_pTeam->isUnitMaxedOut((UnitTypes)eIndex, iExtra);
}

int CyTeam::getBuildingCount(int /*BuildingTypes*/ eIndex) const
{
	return m_pTeam->getBuildingCount((BuildingTypes)eIndex);
}

bool CyTeam::isBuildingMaxedOut(int /*BuildingTypes*/ eIndex, int iExtra) const
{
	return m_pTeam->isBuildingMaxedOut((BuildingTypes)eIndex, iExtra);
}

bool CyTeam::isObsoleteBuilding(int /*BuildingTypes*/ eIndex) const
{
	return m_pTeam->isObsoleteBuilding((BuildingTypes)eIndex);
}

int CyTeam::getResearchProgress(int /*TechTypes*/ eIndex) const
{
	return m_pTeam->getResearchProgress((TechTypes)eIndex);
}

void CyTeam::setResearchProgress(int /*TechTypes*/ eIndex, int iNewValue, int /*PlayerTypes*/ ePlayer)
{
	if (eIndex != NO_TECH)
		m_pTeam->setResearchProgress((TechTypes)eIndex, iNewValue, (PlayerTypes)ePlayer);
}

void CyTeam::changeResearchProgress(int /*TechTypes*/ eIndex, int iChange, int /*PlayerTypes*/ ePlayer)
{
	if (eIndex != NO_TECH)
		m_pTeam->changeResearchProgress((TechTypes)eIndex, iChange, (PlayerTypes)ePlayer);
}

int CyTeam::getTechCount(int /*TechTypes*/ eIndex) const
{
	return m_pTeam->getTechCount((TechTypes)eIndex);
}

bool CyTeam::isTerrainTrade(int /*TerrainTypes*/ eIndex) const
{
	return m_pTeam->isTerrainTrade((TerrainTypes)eIndex);
}

bool CyTeam::isRiverTrade() const
{
	return m_pTeam->isRiverTrade();
}

bool CyTeam::isHasTech(int /*TechTypes*/ iIndex) const
{
	return m_pTeam->isHasTech((TechTypes)iIndex);
}

void CyTeam::setHasTech(int /*TechTypes*/ eIndex, bool bNewValue, int /*PlayerTypes*/ ePlayer, bool bFirst, bool bAnnounce)
{
	m_pTeam->setHasTech((TechTypes)eIndex, bNewValue, (PlayerTypes)ePlayer, bFirst, bAnnounce);
}

bool CyTeam::isNoTradeTech(int /*TechTypes*/ iIndex) const
{
	return m_pTeam->isNoTradeTech((TechTypes)iIndex);
}

int CyTeam::getImprovementYieldChange(int /*ImprovementTypes*/ eIndex1, int /*YieldTypes*/ eIndex2) const
{
	return m_pTeam->getImprovementYieldChange((ImprovementTypes)eIndex1, (YieldTypes)eIndex2);
}

void CyTeam::changeImprovementYieldChange(int /*ImprovementTypes*/ eIndex1, int /*YieldTypes*/ eIndex2, int iChange)
{
	m_pTeam->changeImprovementYieldChange((ImprovementTypes)eIndex1, (YieldTypes)eIndex2, iChange);
}

int CyTeam::getBuildingCommerceTechChange(int eIndex, int eBuilding) const
{
	return m_pTeam->getBuildingCommerceTechChange((CommerceTypes)eIndex, (BuildingTypes)eBuilding);
}

int CyTeam::getVictoryCountdown(int /*VictoryTypes*/ eVictory) const
{
	return m_pTeam->getVictoryCountdown((VictoryTypes)eVictory);
}

int CyTeam::getVictoryDelay(int /*VictoryTypes*/ eVictory) const
{
	return m_pTeam->getVictoryDelay((VictoryTypes)eVictory);
}

bool CyTeam::canLaunch(int /*VictoryTypes*/ eVictory) const
{
	return m_pTeam->canLaunch((VictoryTypes)eVictory);
}

int CyTeam::getLaunchSuccessRate(int /*VictoryTypes*/ eVictory) const
{
	return m_pTeam->getLaunchSuccessRate((VictoryTypes)eVictory);
}

int CyTeam::getEspionagePointsAgainstTeam(int /*TeamTypes*/ eIndex) const
{
	return m_pTeam->getEspionagePointsAgainstTeam((TeamTypes) eIndex);
}

void CyTeam::setEspionagePointsAgainstTeam(int /*TeamTypes*/ eIndex, int iValue)
{
	m_pTeam->setEspionagePointsAgainstTeam((TeamTypes) eIndex, iValue);
}

void CyTeam::changeEspionagePointsAgainstTeam(int /*TeamTypes*/ eIndex, int iChange)
{
	m_pTeam->changeEspionagePointsAgainstTeam((TeamTypes) eIndex, iChange);
}

int CyTeam::getEspionagePointsEver() const
{
	return m_pTeam->getEspionagePointsEver();
}

void CyTeam::setEspionagePointsEver(int iValue)
{
	m_pTeam->setEspionagePointsEver(iValue);
}

void CyTeam::changeEspionagePointsEver(int iChange)
{
	m_pTeam->changeEspionagePointsEver(iChange);
}

int CyTeam::getCounterespionageTurnsLeftAgainstTeam(int /*TeamTypes*/ eIndex) const
{
	return m_pTeam->getCounterespionageTurnsLeftAgainstTeam((TeamTypes) eIndex);
}

void CyTeam::changeCounterespionageTurnsLeftAgainstTeam(int /*TeamTypes*/ eIndex, int iChange)
{
	m_pTeam->changeCounterespionageTurnsLeftAgainstTeam((TeamTypes) eIndex, iChange);
}

int CyTeam::getCounterespionageModAgainstTeam(int /*TeamTypes*/ eIndex) const
{
	return m_pTeam->getCounterespionageModAgainstTeam((TeamTypes) eIndex);
}

void CyTeam::changeCounterespionageModAgainstTeam(int /*TeamTypes*/ eIndex, int iChange)
{
	m_pTeam->changeCounterespionageModAgainstTeam((TeamTypes) eIndex, iChange);
}

bool CyTeam::AI_shareWar(int /*TeamTypes*/ eTeam) const
{
	return m_pTeam->AI_shareWar((TeamTypes)eTeam);
}

int CyTeam::AI_getAtWarCounter(int /*TeamTypes*/ eTeam) const
{
	return m_pTeam->AI_getAtWarCounter((TeamTypes)eTeam);
}

int CyTeam::AI_getAtPeaceCounter(int /*TeamTypes*/ eTeam) const
{
	return m_pTeam->AI_getAtPeaceCounter((TeamTypes)eTeam);
}

int CyTeam::AI_getWarSuccess(int /*TeamTypes*/ eIndex) const
{
	return m_pTeam->AI_getWarSuccess((TeamTypes)eIndex);
}

bool CyTeam::isHasEmbassy(int /*TeamTypes*/ eTeam) const
{
	return m_pTeam->isHasEmbassy((TeamTypes)eTeam);
}
