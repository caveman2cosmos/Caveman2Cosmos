//
// Python wrapper class for global vars and fxns
// Author - Mustafa Thamer
//

#include "CvGameCoreDLL.h"
#include "CvArtFileMgr.h"
#include "CvBuildingInfo.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvMap.h"
#include "CvPlayerAI.h"
#include "CvTeamAI.h"
#include "CyGame.h"
#include "CyGlobalContext.h"
#include "CyMap.h"
#include "CyPlayer.h"
#include "CyTeam.h"

CyGlobalContext::CyGlobalContext()
{
}

CyGlobalContext::~CyGlobalContext()
{
}

CyGlobalContext& CyGlobalContext::getInstance()
{
	static CyGlobalContext globalContext;
	return globalContext;
}

bool CyGlobalContext::isDebugBuild() const
{
#ifdef _DEBUG
	return true;
#else
	return false;
#endif
}

CyGame* CyGlobalContext::getCyGame() const
{
	static CyGame cyGame(GC.getGame());
	return &cyGame;
}


CyMap* CyGlobalContext::getCyMap() const
{
	static CyMap cyMap(&GC.getMap());
	return &cyMap;
}

void CyGlobalContext::switchMap(int iMap)
{
	GC.switchMap((MapTypes)iMap);
}

CyMap* CyGlobalContext::getMapByIndex(int iIndex)
{
	static CyMap cyMap;
	cyMap = GC.getMapByIndex((MapTypes)iIndex);
	return &cyMap;
}

CyPlayer* CyGlobalContext::getCyPlayer(int idx) const
{
	static CyPlayer cyPlayers[MAX_PLAYERS];
	static bool bInit = false;

	if (!bInit)
	{
		for (int i = 0; i < MAX_PLAYERS; i++)
			cyPlayers[i] = CyPlayer(&GET_PLAYER((PlayerTypes)i));
		bInit = true;
	}

	if (idx >= 0 && idx < MAX_PLAYERS)
	{
		return &cyPlayers[idx];
	}

	FErrorMsg("Player index requested isn't valid");
	return NULL;
}


CyPlayer* CyGlobalContext::getCyActivePlayer() const
{
	const PlayerTypes pt = GC.getGame().getActivePlayer();
	return pt != NO_PLAYER ? getCyPlayer(pt) : NULL;
}


CvRandom& CyGlobalContext::getCyASyncRand() const
{
	return GC.getASyncRand();
}

CyTeam* CyGlobalContext::getCyTeam(int i) const
{
	static CyTeam cyTeams[MAX_TEAMS];
	static bool bInit=false;

	if (!bInit)
	{
		int j;
		for(j=0;j<MAX_TEAMS;j++)
		{
			cyTeams[j]=CyTeam(&GET_TEAM((TeamTypes)j));
		}
		bInit = true;
	}

	return i<MAX_TEAMS ? &cyTeams[i] : NULL;
}

int CyGlobalContext::getInfoTypeForString(const char* szInfoType) const
{
	return GC.getInfoTypeForString(szInfoType);
}

int CyGlobalContext::getInfoTypeForStringWithHiddenAssert(const char* szInfoType) const
{
	return GC.getInfoTypeForString(szInfoType, true);
}

int CyGlobalContext::getTypesEnum(const char* szType) const
{
	return GC.getTypesEnum(szType);
}

const CvMapInfo& CyGlobalContext::getMapInfo(int i) const
{
	return GC.getMapInfo((MapTypes)i);
}

const CvEffectInfo* CyGlobalContext::getEffectInfo(int /*EffectTypes*/ i) const
{
	return (i>=0 && i<GC.getNumEffectInfos()) ? &GC.getEffectInfo((EffectTypes) i) : NULL;
}

const CvTerrainInfo* CyGlobalContext::getTerrainInfo(int /*TerrainTypes*/ i) const
{
	return (i>=0 && i<GC.getNumTerrainInfos()) ? &GC.getTerrainInfo((TerrainTypes) i) : NULL;
}

const CvBonusClassInfo* CyGlobalContext::getBonusClassInfo(int /*BonusClassTypes*/ i) const
{
	return (i > 0 && i < GC.getNumBonusClassInfos() ? &GC.getBonusClassInfo((BonusClassTypes) i) : NULL);
}


const CvBonusInfo* CyGlobalContext::getBonusInfo(int /*BonusTypes*/ i) const
{
	return (i>=0 && i<GC.getNumBonusInfos()) ? &GC.getBonusInfo((BonusTypes) i) : NULL;
}

const CvFeatureInfo* CyGlobalContext::getFeatureInfo(int i) const
{
	return (i>=0 && i<GC.getNumFeatureInfos()) ? &GC.getFeatureInfo((FeatureTypes) i) : NULL;
}

const CvCivilizationInfo* CyGlobalContext::getCivilizationInfo(int i) const
{
	return (i>=0 && i<GC.getNumCivilizationInfos()) ? &GC.getCivilizationInfo((CivilizationTypes) i) : NULL;
}


const CvLeaderHeadInfo* CyGlobalContext::getLeaderHeadInfo(int i) const
{
	return (i>=0 && i<GC.getNumLeaderHeadInfos()) ? &GC.getLeaderHeadInfo((LeaderHeadTypes) i) : NULL;
}


const CvTraitInfo* CyGlobalContext::getTraitInfo(int i) const
{
	return (i>=0 && i<GC.getNumTraitInfos()) ? &GC.getTraitInfo((TraitTypes) i) : NULL;
}


const CvUnitInfo* CyGlobalContext::getUnitInfo(int i) const
{
	return (i>=0 && i<GC.getNumUnitInfos()) ? &GC.getUnitInfo((UnitTypes) i) : NULL;
}

const CvSpecialUnitInfo* CyGlobalContext::getSpecialUnitInfo(int i) const
{
	return (i>=0 && i<GC.getNumSpecialUnitInfos()) ? &GC.getSpecialUnitInfo((SpecialUnitTypes) i) : NULL;
}

const CvYieldInfo* CyGlobalContext::getYieldInfo(int i) const
{
	return (i>=0 && i<NUM_YIELD_TYPES) ? &GC.getYieldInfo((YieldTypes) i) : NULL;
}


const CvCommerceInfo* CyGlobalContext::getCommerceInfo(int i) const
{
	return (i>=0 && i<NUM_COMMERCE_TYPES) ? &GC.getCommerceInfo((CommerceTypes) i) : NULL;
}


const CvRouteInfo* CyGlobalContext::getRouteInfo(int i) const
{
	return (i>=0 && i<GC.getNumRouteInfos()) ? &GC.getRouteInfo((RouteTypes) i) : NULL;
}


const CvImprovementInfo* CyGlobalContext::getImprovementInfo(int i) const
{
	return (i>=0 && i<GC.getNumImprovementInfos()) ? &GC.getImprovementInfo((ImprovementTypes) i) : NULL;
}


const CvGoodyInfo* CyGlobalContext::getGoodyInfo(int i) const
{
	return (i>=0 && i<GC.getNumGoodyInfos()) ? &GC.getGoodyInfo((GoodyTypes) i) : NULL;
}


const CvBuildInfo* CyGlobalContext::getBuildInfo(int i) const
{
	return (i>=0 && i<GC.getNumBuildInfos()) ? &GC.getBuildInfo((BuildTypes) i) : NULL;
}


const CvHandicapInfo* CyGlobalContext::getHandicapInfo(int i) const
{
	return (i>=0 && i<GC.getNumHandicapInfos()) ? &GC.getHandicapInfo((HandicapTypes) i) : NULL;
}


const CvBuildingInfo* CyGlobalContext::getBuildingInfo(int i) const
{
	return (i>=0 && i<GC.getNumBuildingInfos()) ? &GC.getBuildingInfo((BuildingTypes) i) : NULL;
}


const CvInfoBase* CyGlobalContext::getUnitCombatInfo(int i) const
{
	return (i>=0 && i<GC.getNumUnitCombatInfos()) ? &GC.getUnitCombatInfo((UnitCombatTypes)i) : NULL;
}

const CvPromotionLineInfo* CyGlobalContext::getPromotionLineInfo(int i) const
{
	return (i>=0 && i<GC.getNumPromotionLineInfos()) ? &GC.getPromotionLineInfo((PromotionLineTypes)i) : NULL;
}

const CvInfoBase* CyGlobalContext::getDomainInfo(int i) const
{
	return (i>=0 && i<NUM_DOMAIN_TYPES) ? &GC.getDomainInfo((DomainTypes)i) : NULL;
}


const CvActionInfo* CyGlobalContext::getActionInfo(int i) const
{
	return (i>=0 && i<GC.getNumActionInfos()) ? &GC.getActionInfo(i) : NULL;
}

const CvCommandInfo* CyGlobalContext::getCommandInfo(int i) const
{
	return (i>=0 && i<NUM_COMMAND_TYPES) ? &GC.getCommandInfo((CommandTypes)i) : NULL;
}

const CvControlInfo* CyGlobalContext::getControlInfo(int i) const
{
	return (i>=0 && i<NUM_CONTROL_TYPES) ? &GC.getControlInfo((ControlTypes)i) : NULL;
}

const CvMissionInfo* CyGlobalContext::getMissionInfo(int i) const
{
	return (i>=0 && i<GC.getNumMissionInfos()) ? &GC.getMissionInfo((MissionTypes) i) : NULL;
}

const CvPromotionInfo* CyGlobalContext::getPromotionInfo(int i) const
{
	return (i>=0 && i<GC.getNumPromotionInfos()) ? &GC.getPromotionInfo((PromotionTypes) i) : NULL;
}


const CvTechInfo* CyGlobalContext::getTechInfo(int i) const
{
	return (i>=0 && i<GC.getNumTechInfos()) ? &GC.getTechInfo((TechTypes) i) : NULL;
}


const CvSpecialBuildingInfo* CyGlobalContext::getSpecialBuildingInfo(int i) const
{
	return (i>=0 && i<GC.getNumSpecialBuildingInfos()) ? &GC.getSpecialBuildingInfo((SpecialBuildingTypes) i) : NULL;
}


const CvReligionInfo* CyGlobalContext::getReligionInfo(int i) const
{
	return (i>=0 && i<GC.getNumReligionInfos()) ? &GC.getReligionInfo((ReligionTypes) i) : NULL;
}


const CvCorporationInfo* CyGlobalContext::getCorporationInfo(int i) const
{
	return (i>=0 && i<GC.getNumCorporationInfos()) ? &GC.getCorporationInfo((CorporationTypes) i) : NULL;
}


const CvSpecialistInfo* CyGlobalContext::getSpecialistInfo(int i) const
{
	return (i>=0 && i<GC.getNumSpecialistInfos()) ? &GC.getSpecialistInfo((SpecialistTypes) i) : NULL;
}


const CvCivicOptionInfo* CyGlobalContext::getCivicOptionInfo(int i) const
{
	return (i>=0 && i<GC.getNumCivicOptionInfos()) ? &GC.getCivicOptionInfo((CivicOptionTypes) i) : NULL;
}


const CvCivicInfo* CyGlobalContext::getCivicInfo(int i) const
{
	return (i>=0 && i<GC.getNumCivicInfos()) ? &GC.getCivicInfo((CivicTypes) i) : NULL;
}

const CvDiplomacyInfo* CyGlobalContext::getDiplomacyInfo(int i) const
{
	return (i>=0 && i<GC.getNumDiplomacyInfos()) ? &GC.getDiplomacyInfo(i) : NULL;
}

const CvHurryInfo* CyGlobalContext::getHurryInfo(int i) const
{
	return (i>=0 && i<GC.getNumHurryInfos()) ? &GC.getHurryInfo((HurryTypes) i) : NULL;
}


const CvProjectInfo* CyGlobalContext::getProjectInfo(int i) const
{
	return (i>=0 && i<GC.getNumProjectInfos()) ? &GC.getProjectInfo((ProjectTypes) i) : NULL;
}


const CvVoteInfo* CyGlobalContext::getVoteInfo(int i) const
{
	return (i>=0 && i<GC.getNumVoteInfos()) ? &GC.getVoteInfo((VoteTypes) i) : NULL;
}


const CvProcessInfo* CyGlobalContext::getProcessInfo(int i) const
{
	return (i>=0 && i<GC.getNumProcessInfos()) ? &GC.getProcessInfo((ProcessTypes) i) : NULL;
}


const CvEmphasizeInfo* CyGlobalContext::getEmphasizeInfo(int i) const
{
	return (i>=0 && i<GC.getNumEmphasizeInfos()) ? &GC.getEmphasizeInfo((EmphasizeTypes) i) : NULL;
}


const CvCultureLevelInfo* CyGlobalContext::getCultureLevelInfo(int i) const
{
	return (i>=0 && i<GC.getNumCultureLevelInfos()) ? &GC.getCultureLevelInfo((CultureLevelTypes) i) : NULL;
}


const CvUpkeepInfo* CyGlobalContext::getUpkeepInfo(int i) const
{
	return (i>=0 && i<GC.getNumUpkeepInfos()) ? &GC.getUpkeepInfo((UpkeepTypes) i) : NULL;
}


const CvVictoryInfo* CyGlobalContext::getVictoryInfo(int i) const
{
	return (i>=0 && i<GC.getNumVictoryInfos()) ? &GC.getVictoryInfo((VictoryTypes) i) : NULL;
}


const CvEraInfo* CyGlobalContext::getEraInfo(int i) const
{
	return (i>=0 && i<GC.getNumEraInfos()) ? &GC.getEraInfo((EraTypes) i) : NULL;
}


const CvWorldInfo* CyGlobalContext::getWorldInfo(int i) const
{
	return (i>=0 && i<GC.getNumWorldInfos()) ? &GC.getWorldInfo((WorldSizeTypes) i) : NULL;
}


const CvClimateInfo* CyGlobalContext::getClimateInfo(int i) const
{
	return (i>=0 && i<GC.getNumClimateInfos()) ? &GC.getClimateInfo((ClimateTypes) i) : NULL;
}


const CvSeaLevelInfo* CyGlobalContext::getSeaLevelInfo(int i) const
{
	return (i>=0 && i<GC.getNumSeaLevelInfos()) ? &GC.getSeaLevelInfo((SeaLevelTypes) i) : NULL;
}


const CvInfoBase* CyGlobalContext::getUnitAIInfo(int i) const
{
	return (i>=0 && i<NUM_UNITAI_TYPES) ? &GC.getUnitAIInfo((UnitAITypes)i) : NULL;
}


const CvColorInfo* CyGlobalContext::getColorInfo(int i) const
{
	return (i>=0 && i<GC.getNumColorInfos()) ? &GC.getColorInfo((ColorTypes)i) : NULL;
}

const CvPropertyInfo* CyGlobalContext::getPropertyInfo(int i) const
{
	return (i>=0 && i<GC.getNumPropertyInfos()) ? &GC.getPropertyInfo((PropertyTypes)i) : NULL;
}

const CvPlayerColorInfo* CyGlobalContext::getPlayerColorInfo(int i) const
{
	return (i>=0 && i<GC.getNumPlayerColorInfos()) ? &GC.getPlayerColorInfo((PlayerColorTypes)i) : NULL;
}


const CvMainMenuInfo* CyGlobalContext::getMainMenus(int i) const
{
	return ((i >= 0 && i < GC.getNumMainMenus()) ? &GC.getMainMenus(i) : NULL);
}


const CvVoteSourceInfo* CyGlobalContext::getVoteSourceInfo(int i) const
{
	return ((i >= 0 && i < GC.getNumVoteSourceInfos()) ? &GC.getVoteSourceInfo((VoteSourceTypes)i) : NULL);
}


const CvInfoBase* CyGlobalContext::getAttitudeInfo(int i) const
{
	return (i>=0 && i<NUM_ATTITUDE_TYPES) ? &GC.getAttitudeInfo((AttitudeTypes)i) : NULL;
}


const CvInfoBase* CyGlobalContext::getMemoryInfo(int i) const
{
	return (i>=0 && i<NUM_MEMORY_TYPES) ? &GC.getMemoryInfo((MemoryTypes)i) : NULL;
}


const CvPlayerOptionInfo& CyGlobalContext::getPlayerOptionInfo(int i) const
{
	return GC.getPlayerOptionInfo((PlayerOptionTypes)i);
}


const CvGraphicOptionInfo& CyGlobalContext::getGraphicOptionInfo(int i) const
{
	return GC.getGraphicOptionInfo((GraphicOptionTypes)i);
}


const CvInfoBase* CyGlobalContext::getConceptInfo(int i) const
{
	return (i>=0 && i<GC.getNumConceptInfos()) ? &GC.getConceptInfo((ConceptTypes)i) : NULL;
}


const CvInfoBase* CyGlobalContext::getNewConceptInfo(int i) const
{
	return (i>=0 && i<GC.getNumNewConceptInfos()) ? &GC.getNewConceptInfo((NewConceptTypes)i) : NULL;
}


const CvInfoBase* CyGlobalContext::getCalendarInfo(int i) const
{
	return (i>=0 && i<GC.getNumCalendarInfos()) ? &GC.getCalendarInfo((CalendarTypes)i) : NULL;
}


const CvInfoBase* CyGlobalContext::getGameOptionInfo(int i) const
{
	return (i>=0 && i<GC.getNumGameOptionInfos()) ? &GC.getGameOptionInfo((GameOptionTypes)i) : NULL;
}


const CvInfoBase* CyGlobalContext::getMPOptionInfo(int i) const
{
	return (i>=0 && i<GC.getNumMPOptionInfos()) ? &GC.getMPOptionInfo((MultiplayerOptionTypes)i) : NULL;
}


const CvInfoBase* CyGlobalContext::getForceControlInfo(int i) const
{
	return (i>=0 && i<GC.getNumForceControlInfos()) ? &GC.getForceControlInfo((ForceControlTypes)i) : NULL;
}


const CvInfoBase* CyGlobalContext::getSeasonInfo(int i) const
{
	return (i>=0 && i<GC.getNumSeasonInfos()) ? &GC.getSeasonInfo((SeasonTypes)i) : NULL;
}


const CvInfoBase* CyGlobalContext::getDenialInfo(int i) const
{
	return (i>=0 && i<GC.getNumDenialInfos()) ? &GC.getDenialInfo((DenialTypes)i) : NULL;
}

const CvEventTriggerInfo* CyGlobalContext::getEventTriggerInfo(int i) const
{
	return (i>=0 && i<GC.getNumEventTriggerInfos()) ? &GC.getEventTriggerInfo((EventTriggerTypes)i) : NULL;
}


const CvEventInfo* CyGlobalContext::getEventInfo(int i) const
{
	return (i>=0 && i<GC.getNumEventInfos()) ? &GC.getEventInfo((EventTypes)i) : NULL;
}


const CvEspionageMissionInfo* CyGlobalContext::getEspionageMissionInfo(int i) const
{
	return (i>=0 && i<GC.getNumEspionageMissionInfos()) ? &GC.getEspionageMissionInfo((EspionageMissionTypes)i) : NULL;
}


const CvUnitArtStyleTypeInfo* CyGlobalContext::getUnitArtStyleTypeInfo(int i) const
{
	return (i>=0 && i<GC.getNumUnitArtStyleTypeInfos()) ? &GC.getUnitArtStyleTypeInfo((UnitArtStyleTypes)i) : NULL;
}


const CvArtInfoInterface* CyGlobalContext::getInterfaceArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumInterfaceArtInfos()) ? &ARTFILEMGR.getInterfaceArtInfo(i) : NULL;
}


const CvArtInfoMovie* CyGlobalContext::getMovieArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumMovieArtInfos()) ? &ARTFILEMGR.getMovieArtInfo(i) : NULL;
}


const CvArtInfoMisc* CyGlobalContext::getMiscArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumMiscArtInfos()) ? &ARTFILEMGR.getMiscArtInfo(i) : NULL;
}


const CvArtInfoUnit* CyGlobalContext::getUnitArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumUnitArtInfos()) ? &ARTFILEMGR.getUnitArtInfo(i) : NULL;
}


const CvArtInfoBuilding* CyGlobalContext::getBuildingArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumBuildingArtInfos()) ? &ARTFILEMGR.getBuildingArtInfo(i) : NULL;
}


const CvArtInfoCivilization* CyGlobalContext::getCivilizationArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumCivilizationArtInfos()) ? &ARTFILEMGR.getCivilizationArtInfo(i) : NULL;
}


const CvArtInfoBonus* CyGlobalContext::getBonusArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumBonusArtInfos()) ? &ARTFILEMGR.getBonusArtInfo(i) : NULL;
}


const CvArtInfoImprovement* CyGlobalContext::getImprovementArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumImprovementArtInfos()) ? &ARTFILEMGR.getImprovementArtInfo(i) : NULL;
}


const CvGameSpeedInfo& CyGlobalContext::getGameSpeedInfo(int i) const
{
	return GC.getGameSpeedInfo((GameSpeedTypes) i);
}
