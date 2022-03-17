#include "CvGameCoreDLL.h"
#include "CyTeam.h"

//
// published python interface for CyTeam
//

void CyTeamPythonInterface()
{
	OutputDebugString("Python Extension Module - CyTeamPythonInterface\n");

	python::class_<CyTeam>("CyTeam", python::no_init)
		.def("isRebelAgainst", &CyTeam::isRebelAgainst, "bool ( int iTeam )" )
		.def("setRebelAgainst", &CyTeam::setRebelAgainst, "void ( int iTeam, bool bNewValue )" )
		.def("countRebelAgainst", &CyTeam::countRebelAgainst, "int ( )" )

		.def("addTeam", &CyTeam::addTeam, "void (int /*TeamTypes*/ eTeam)")

		.def("canChangeWarPeace", &CyTeam::canChangeWarPeace, "bool (int /*TeamTypes*/ eTeam)")
		.def("canDeclareWar", &CyTeam::canDeclareWar, "bool (int /*TeamTypes*/ eTeam)")
		.def("declareWar", &CyTeam::declareWar, "void (int /*TeamTypes*/ eTeam, bool bNewDiplo, int /*WarPlanTypes*/ eWarPlan) - Forces your team to declare War on iTeam")
		.def("makePeace", &CyTeam::makePeace, "void (int /*TeamTypes*/ eTeam) - Forces peace between your team and iTeam")
		.def("canContact", &CyTeam::canContact, "bool (int /*TeamTypes*/ eTeam)")
		.def("meet", &CyTeam::meet, "void (int /*TeamTypes*/ eTeam, bool bNewDiplo) - forces team to meet iTeam")
		.def("signOpenBorders", &CyTeam::signOpenBorders, "void (int /*TeamTypes*/ eTeam)")
		.def("signDefensivePact", &CyTeam::signDefensivePact, "void (int /*TeamTypes*/ eTeam)")

		.def("getAssets", &CyTeam::getAssets, "int ()")
		.def("getPower", &CyTeam::getPower, "int (bool bIncludeVassals)")

		.def("getTotalVictoryScore", &CyTeam::getTotalVictoryScore, "int ()")

		.def("isAtWar", &CyTeam::isAtWar, "bool (bool bCountMinors)")
		.def("getAtWarCount", &CyTeam::getAtWarCount, "int (bool bIgnoreMinors)")
		.def("getHasMetCivCount", &CyTeam::getHasMetCivCount, "int (bool bIgnoreMinors)")
		.def("isAVassal", &CyTeam::isAVassal, "bool ()")

		.def("getBuildingCountPlusMaking", &CyTeam::getBuildingCountPlusMaking, "int (int (BuildingTypes) eBuilding)")
		.def("getUnitMaking", &CyTeam::getUnitMaking, "int (int (UnitTypes) eUnit)")
		.def("getUnitCountPlusMaking", &CyTeam::getUnitCountPlusMaking, "int (int (UnitTypes) eUnit)")
		.def("getHasReligionCount", &CyTeam::getHasReligionCount, "int (int (ReligionTypes) eReligion)")
		.def("getHasCorporationCount", &CyTeam::getHasCorporationCount, "int (int (CorporationTypes) eReligion)")

		.def("countTotalCulture", &CyTeam::countTotalCulture, "int ()")

		.def("countNumCitiesByArea", &CyTeam::countNumCitiesByArea, "int (CyArea* pArea)")

		.def("getResearchCost", &CyTeam::getResearchCost, "int (TechID) - total cost of Tech")

		.def("hasHolyCity", &CyTeam::hasHolyCity, "bool (int (ReligionTypes) eReligion) - does this team have eReligion's holy city?")
		.def("hasHeadquarters", &CyTeam::hasHeadquarters, "bool (int (CorporationTypes) eCorporation) - does this team have eCorporation's headquarters?")

		.def("isHuman", &CyTeam::isHuman, "bool () - is human team?")
		.def("isNPC", &CyTeam::isNPC, "bool () - is NPC team?")
		.def("isHominid", &CyTeam::isNPC, "bool () - is Hominid team?")
		.def("isMinorCiv", &CyTeam::isMinorCiv)
		.def("setIsMinorCiv", &CyTeam::setIsMinorCiv, "void( bool bNewValue, bool bDoBarbCivCheck )")

		.def("getLeaderID", &CyTeam::getLeaderID, "int (PlayerTypes) ()")
		.def("getHandicapType", &CyTeam::getHandicapType, "int (HandicapTypes) ()")
		.def("getName", &CyTeam::getName, "str ()")

		.def("getNumMembers", &CyTeam::getNumMembers, "int (); # of people on team")
		.def("isAlive", &CyTeam::isAlive, "bool ()")
		.def("isEverAlive", &CyTeam::isEverAlive, "bool ()")
		.def("getNumCities", &CyTeam::getNumCities, "int (); # of cities controlled by team")
		.def("getTotalPopulation", &CyTeam::getTotalPopulation, "int (); # of citizens controlled by team")
		.def("getTotalLand", &CyTeam::getTotalLand, "int ()")
		.def("getNukeInterception", &CyTeam::getNukeInterception, "int ()")
		.def("changeNukeInterception", &CyTeam::changeNukeInterception, "void (iChange)")

		.def("getForceTeamVoteEligibilityCount", &CyTeam::getForceTeamVoteEligibilityCount, "int (int /*VoteSourceTypes*/ eVoteSource)")
		.def("isForceTeamVoteEligible", &CyTeam::isForceTeamVoteEligible, "bool (int /*VoteSourceTypes*/ eVoteSource)")
		.def("changeForceTeamVoteEligibilityCount", &CyTeam::changeForceTeamVoteEligibilityCount, "void (int /*VoteSourceTypes*/ eVoteSource, iChange)")
		.def("getExtraWaterSeeFromCount", &CyTeam::getExtraWaterSeeFromCount, "int ()")
		.def("isExtraWaterSeeFrom", &CyTeam::isExtraWaterSeeFrom, "bool ()")
		.def("changeExtraWaterSeeFromCount", &CyTeam::changeExtraWaterSeeFromCount, "void (iChange)")
		.def("getMapTradingCount", &CyTeam::getMapTradingCount, "int ()")
		.def("isMapTrading", &CyTeam::isMapTrading, "bool () - map is ready")
		.def("changeMapTradingCount", &CyTeam::changeMapTradingCount, "void (iChange)")
		.def("getTechTradingCount", &CyTeam::getTechTradingCount, "int ()")
		.def("isTechTrading", &CyTeam::isTechTrading, "bool () - tech trading?")
		.def("changeTechTradingCount", &CyTeam::changeTechTradingCount, "void (iChange)")
		.def("getGoldTradingCount", &CyTeam::getGoldTradingCount, "int ()")
		.def("isGoldTrading", &CyTeam::isGoldTrading, "bool () - gold trading?")
		.def("changeGoldTradingCount", &CyTeam::changeGoldTradingCount, "void (iChange)")
		.def("getOpenBordersTradingCount", &CyTeam::getOpenBordersTradingCount, "int ()")
		.def("isOpenBordersTrading", &CyTeam::isOpenBordersTrading, "bool ()")
		.def("changeOpenBordersTradingCount", &CyTeam::changeOpenBordersTradingCount, "void (iChange)")
		.def("getDefensivePactTradingCount", &CyTeam::getDefensivePactTradingCount, "int ()")
		.def("isDefensivePactTrading", &CyTeam::isDefensivePactTrading, "bool ()")
		.def("changeDefensivePactTradingCount", &CyTeam::changeDefensivePactTradingCount, "void (iChange)")
		.def("getPermanentAllianceTradingCount", &CyTeam::getPermanentAllianceTradingCount, "int ()")
		.def("isPermanentAllianceTrading", &CyTeam::isPermanentAllianceTrading, "bool ()")
		.def("changePermanentAllianceTradingCount", &CyTeam::changePermanentAllianceTradingCount, "void (iChange)")
		.def("getVassalTradingCount", &CyTeam::getVassalTradingCount, "int ()")
		.def("isVassalStateTrading", &CyTeam::isVassalStateTrading, "bool ()")
		.def("changeVassalTradingCount", &CyTeam::changeVassalTradingCount, "void (iChange)")
		.def("getBridgeBuildingCount", &CyTeam::getBridgeBuildingCount, "int ()")
		.def("isBridgeBuilding", &CyTeam::isBridgeBuilding, "bool ()")
		.def("changeBridgeBuildingCount", &CyTeam::changeBridgeBuildingCount, "void (iChange)")
		.def("getIrrigationCount", &CyTeam::getIrrigationCount, "int ()")
		.def("isIrrigation", &CyTeam::isIrrigation, "bool ()")
		.def("changeIrrigationCount", &CyTeam::changeIrrigationCount, "void (iChange)")
		.def("getIgnoreIrrigationCount", &CyTeam::getIgnoreIrrigationCount, "int ()")
		.def("isIgnoreIrrigation", &CyTeam::isIgnoreIrrigation, "bool ()")
		.def("changeIgnoreIrrigationCount", &CyTeam::changeIgnoreIrrigationCount, "void (iChange)")
		.def("getWaterWorkCount", &CyTeam::getWaterWorkCount, "int ()")
		.def("isWaterWork", &CyTeam::isWaterWork, "bool ()")
		.def("changeWaterWorkCount", &CyTeam::changeWaterWorkCount, "void (iChange)")

		.def("getVassalPower", &CyTeam::getVassalPower, "int ()")
		.def("setVassalPower", &CyTeam::setVassalPower, "void (int)")
		.def("getMasterPower", &CyTeam::getMasterPower, "int ()")
		.def("setMasterPower", &CyTeam::setMasterPower, "void (int)")

		.def("isMapCentering", &CyTeam::isMapCentering, "bool () - map is centered")
		.def("setMapCentering", &CyTeam::setMapCentering, "void (bNewValue)")

		.def("getEnemyWarWearinessModifier", &CyTeam::getEnemyWarWearinessModifier, "int ()")
		.def("changeEnemyWarWearinessModifier", &CyTeam::changeEnemyWarWearinessModifier, "void (iChange)")

		.def("getID", &CyTeam::getID, "int () - team ID")

		.def("getWarWeariness", &CyTeam::getWarWeariness, "int (int /*TeamTypes*/ eIndex)")
		.def("changeWarWeariness", &CyTeam::changeWarWeariness, "void (int /*TeamTypes*/ eIndex, iChange)")
		.def("getCommerceFlexibleCount", &CyTeam::getCommerceFlexibleCount, "int (int /*CommerceTypes*/ eIndex)")
		.def("isCommerceFlexible", &CyTeam::isCommerceFlexible, "bool (int /*CommerceTypes*/ eIndex)")
		.def("changeCommerceFlexibleCount", &CyTeam::changeCommerceFlexibleCount, "void (int /*CommerceTypes*/ eIndex, iChange)")

		.def("getExtraMoves", &CyTeam::getExtraMoves, "int (int /*DomainTypes*/ eIndex)")
		.def("changeExtraMoves", &CyTeam::changeExtraMoves, "void (int /*DomainTypes*/ eIndex, int iChange)")

		.def("isHasMet", &CyTeam::isHasMet, "bool (int /*TeamTypes*/ eIndex)")
		.def("isAtWarWith", &CyTeam::isAtWarWith, "bool (int /*TeamTypes*/ eIndex)")
		.def("isPermanentWarPeace", &CyTeam::isPermanentWarPeace, "bool (int /*TeamTypes*/ eIndex)")
		.def("setPermanentWarPeace", &CyTeam::setPermanentWarPeace, "void (int /*TeamTypes*/ eIndex, bool bNewValue)")
		.def("isOpenBorders", &CyTeam::isOpenBorders, "bool (TeamTypes)")
		.def("isForcePeace", &CyTeam::isForcePeace, "bool (TeamTypes)")
		.def("isVassal", &CyTeam::isVassal, "bool (TeamTypes)")
		.def("assignVassal", &CyTeam::assignVassal, "void (TeamTypes, bool)")
		.def("freeVassal", &CyTeam::freeVassal, "void (TeamTypes)")
		.def("isDefensivePact", &CyTeam::isDefensivePact, "bool (TeamTypes)")
		.def("getRouteChange", &CyTeam::getRouteChange,	"int (RouteType) - Route Change caused by RouteType")
		.def("changeRouteChange", &CyTeam::changeRouteChange, "void (int /*RouteType*/ eIndex, int iChange)")
		.def("getProjectCount", &CyTeam::getProjectCount, "bool (int /*ProjectTypes*/ eIndex)")
		.def("getProjectDefaultArtType", &CyTeam::getProjectDefaultArtType, "bool (int /*ProjectTypes*/ eIndex)")
		.def("setProjectArtType", &CyTeam::setProjectArtType, "void (int /*ProjectTypes*/ eIndex, int number, int value)")
		.def("changeProjectCount", &CyTeam::changeProjectCount, "void (int /*ProjectTypes*/ eIndex, int iChange)")
		.def("getProjectMaking", &CyTeam::getProjectMaking, "bool (int /*ProjectTypes*/ eIndex)")
		.def("getBuildingCount", &CyTeam::getBuildingCount, "bool (int /*BuildingTypes*/ eIndex)")
		.def("isBuildingMaxedOut", &CyTeam::isBuildingMaxedOut, "bool (BuildingTypes, iExtra)")
		.def("getUnitCount", &CyTeam::getUnitCount, "int (int (UnitTypes) eIndex)")
		.def("isUnitMaxedOut", &CyTeam::isUnitMaxedOut, "bool (int (UnitTypes) eIndex, int iExtra)")
		.def("isObsoleteBuilding", &CyTeam::isObsoleteBuilding, "bool (BuildingID - is BuildingID obsolete?")

		.def("getResearchProgress", &CyTeam::getResearchProgress, "int (TechID) - progress towards finishing research on TechID")
		.def("setResearchProgress", &CyTeam::setResearchProgress, "void (TechID, iNewValue, iPlayer ) - sets progress towards TechID")
		.def("changeResearchProgress", &CyTeam::changeResearchProgress, "void (TechID, iChange, iPlayer ) - edits progress towards TechID")
		.def("isTerrainTrade", &CyTeam::isTerrainTrade, "bool (int iTerrainType) - will let us know if this terrain type allows trade")
		.def("isRiverTrade", &CyTeam::isRiverTrade, "bool () - will let us know if rivers allow trade")
		.def("getTechCount", &CyTeam::getTechCount, "int (TechID)")

		.def("isHasTech", &CyTeam::isHasTech, "bool (TechID) - has the team researched techID")
		.def("setHasTech", &CyTeam::setHasTech, "void (TechID, bNewValue, iPlayer, bFirst, bAnnounce)")
		.def("isNoTradeTech", &CyTeam::isNoTradeTech, "bool (TechID)")

		.def("getImprovementYieldChange", &CyTeam::getImprovementYieldChange,	"int (int /*ImprovementTypes*/ eIndex1, int /*YieldTypes*/ eIndex2)")
		.def("changeImprovementYieldChange", &CyTeam::changeImprovementYieldChange, "void (int /*ImprovementTypes*/ eIndex1, int /*YieldTypes*/ eIndex2, iChange)")

		.def("getBuildingCommerceTechChange", &CyTeam::getBuildingCommerceTechChange, "int (eCommerce, eBuilding)")

		.def("isHasEmbassy", &CyTeam::isHasEmbassy, "bool (TeamTypes)")

		.def("getVictoryCountdown", &CyTeam::getVictoryCountdown, "int (int /*VictoryTypes*/)")
		.def("getVictoryDelay", &CyTeam::getVictoryDelay, "int (int /*VictoryTypes*/)")
		.def("canLaunch", &CyTeam::canLaunch, "bool ()")
		.def("getLaunchSuccessRate", &CyTeam::getLaunchSuccessRate, "int (int /*VictoryTypes*/)")

		.def("getEspionagePointsAgainstTeam", &CyTeam::getEspionagePointsAgainstTeam, "int (TeamTypes eIndex)")
		.def("setEspionagePointsAgainstTeam", &CyTeam::setEspionagePointsAgainstTeam, "void (TeamTypes eIndex, int iValue)")
		.def("changeEspionagePointsAgainstTeam", &CyTeam::changeEspionagePointsAgainstTeam, "void (TeamTypes eIndex, int iChange)")
		.def("getEspionagePointsEver", &CyTeam::getEspionagePointsEver, "int ()")
		.def("setEspionagePointsEver", &CyTeam::setEspionagePointsEver, "void (int iValue)")
		.def("changeEspionagePointsEver", &CyTeam::changeEspionagePointsEver, "void (int iChange)")
		.def("getCounterespionageTurnsLeftAgainstTeam", &CyTeam::getCounterespionageTurnsLeftAgainstTeam, "int (TeamTypes eIndex)")
		.def("changeCounterespionageTurnsLeftAgainstTeam", &CyTeam::changeCounterespionageTurnsLeftAgainstTeam, "void (TeamTypes eIndex, int iChange)")
		.def("getCounterespionageModAgainstTeam", &CyTeam::getCounterespionageModAgainstTeam, "int (TeamTypes eIndex)")
		.def("changeCounterespionageModAgainstTeam", &CyTeam::changeCounterespionageModAgainstTeam, "void (TeamTypes eIndex, int iChange)")
		.def("AI_shareWar", &CyTeam::AI_shareWar, "bool (TeamTypes)")
		.def("AI_getAtWarCounter", &CyTeam::AI_getAtWarCounter, "int (TeamTypes)")
		.def("AI_getAtPeaceCounter", &CyTeam::AI_getAtPeaceCounter, "int (TeamTypes)")
		.def("AI_getWarSuccess", &CyTeam::AI_getWarSuccess, "int (TeamTypes)")
	;
}
