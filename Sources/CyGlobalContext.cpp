//
// Python wrapper class for global vars and fxns
// Author - Mustafa Thamer
//

#include "CvBuildingInfo.h"
#include "CvGameCoreDLL.h"
#include "CvGameAI.h"
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

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
bool CyGlobalContext::enableMultiMaps()
{
#ifdef PARALLEL_MAPS
	GC.enableMultiMaps();
	return true;
#else
	return false;
#endif
}

bool CyGlobalContext::multiMapsEnabled() const
{
	return GC.multiMapsEnabled();
}

void CyGlobalContext::switchMap(int iMap)
{
	GC.switchMap((MapTypes)iMap);
}

int CyGlobalContext::getNumMapInfos() const
{
	return GC.getNumMapInfos();
}

CvMapInfo* CyGlobalContext::getMapInfo(int iMap) const
{
	return &(GC.getMapInfo((MapTypes)iMap));
}

CyMap* CyGlobalContext::getMapByIndex(int iIndex)
{
	static CyMap cyMap;
	cyMap = GC.getMapByIndex((MapTypes)iIndex);
	return &cyMap;
}

int CyGlobalContext::getNumMaps() const
{
	return GC.getNumMaps();
}

void CyGlobalContext::updateMaps()
{
	GC.updateMaps();
}

void CyGlobalContext::initializeMap(int iMap)
{
	GC.initializeMap((MapTypes)iMap);
}

bool CyGlobalContext::mapInitialized(int iMap) const
{
	return GC.mapInitialized((MapTypes)iMap);
}

/*******************************/
/***** Parallel Maps - End *****/
/*******************************/	

void CyGlobalContext::setIsInPedia(bool isInPedia)
{
	GC.setIsInPedia(isInPedia);
}

CyPlayer* CyGlobalContext::getCyPlayer(int idx) const
{
	static CyPlayer cyPlayers[MAX_PLAYERS];
	static bool bInit=false;

	if (!bInit)
	{
		int i;
		for(i=0;i<MAX_PLAYERS;i++)
			cyPlayers[i]=CyPlayer(&GET_PLAYER((PlayerTypes)i));
		bInit=true;
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


CvEffectInfo* CyGlobalContext::getEffectInfo(int /*EffectTypes*/ i) const
{
	return (i>=0 && i<GC.getNumEffectInfos()) ? &GC.getEffectInfo((EffectTypes) i) : NULL;
}

CvTerrainInfo* CyGlobalContext::getTerrainInfo(int /*TerrainTypes*/ i) const
{
	return (i>=0 && i<GC.getNumTerrainInfos()) ? &GC.getTerrainInfo((TerrainTypes) i) : NULL;
}

CvBonusClassInfo* CyGlobalContext::getBonusClassInfo(int /*BonusClassTypes*/ i) const
{
	return (i > 0 && i < GC.getNumBonusClassInfos() ? &GC.getBonusClassInfo((BonusClassTypes) i) : NULL);
}


CvBonusInfo* CyGlobalContext::getBonusInfo(int /*(BonusTypes)*/ i) const
{
	return (i>=0 && i<GC.getNumBonusInfos()) ? &GC.getBonusInfo((BonusTypes) i) : NULL;
}

CvFeatureInfo* CyGlobalContext::getFeatureInfo(int i) const
{
	return (i>=0 && i<GC.getNumFeatureInfos()) ? &GC.getFeatureInfo((FeatureTypes) i) : NULL;
}

CvCivilizationInfo* CyGlobalContext::getCivilizationInfo(int i) const
{
	return (i>=0 && i<GC.getNumCivilizationInfos()) ? &GC.getCivilizationInfo((CivilizationTypes) i) : NULL;
}


CvLeaderHeadInfo* CyGlobalContext::getLeaderHeadInfo(int i) const
{
	return (i>=0 && i<GC.getNumLeaderHeadInfos()) ? &GC.getLeaderHeadInfo((LeaderHeadTypes) i) : NULL;
}


CvTraitInfo* CyGlobalContext::getTraitInfo(int i) const
{
	return (i>=0 && i<GC.getNumTraitInfos()) ? &GC.getTraitInfo((TraitTypes) i) : NULL;
}


CvUnitInfo* CyGlobalContext::getUnitInfo(int i) const
{
	return (i>=0 && i<GC.getNumUnitInfos()) ? &GC.getUnitInfo((UnitTypes) i) : NULL;
}

CvSpecialUnitInfo* CyGlobalContext::getSpecialUnitInfo(int i) const
{
	return (i>=0 && i<GC.getNumSpecialUnitInfos()) ? &GC.getSpecialUnitInfo((SpecialUnitTypes) i) : NULL;
}

CvYieldInfo* CyGlobalContext::getYieldInfo(int i) const
{
	return (i>=0 && i<NUM_YIELD_TYPES) ? &GC.getYieldInfo((YieldTypes) i) : NULL;
}


CvCommerceInfo* CyGlobalContext::getCommerceInfo(int i) const
{
	return (i>=0 && i<NUM_COMMERCE_TYPES) ? &GC.getCommerceInfo((CommerceTypes) i) : NULL;
}


CvRouteInfo* CyGlobalContext::getRouteInfo(int i) const
{
	return (i>=0 && i<GC.getNumRouteInfos()) ? &GC.getRouteInfo((RouteTypes) i) : NULL;
}


CvImprovementInfo* CyGlobalContext::getImprovementInfo(int i) const
{
	return (i>=0 && i<GC.getNumImprovementInfos()) ? &GC.getImprovementInfo((ImprovementTypes) i) : NULL;
}


CvGoodyInfo* CyGlobalContext::getGoodyInfo(int i) const
{
	return (i>=0 && i<GC.getNumGoodyInfos()) ? &GC.getGoodyInfo((GoodyTypes) i) : NULL;
}


CvBuildInfo* CyGlobalContext::getBuildInfo(int i) const
{
	return (i>=0 && i<GC.getNumBuildInfos()) ? &GC.getBuildInfo((BuildTypes) i) : NULL;
}


CvHandicapInfo* CyGlobalContext::getHandicapInfo(int i) const
{
	return (i>=0 && i<GC.getNumHandicapInfos()) ? &GC.getHandicapInfo((HandicapTypes) i) : NULL;
}


CvBuildingInfo* CyGlobalContext::getBuildingInfo(int i) const
{
	return (i>=0 && i<GC.getNumBuildingInfos()) ? &GC.getBuildingInfo((BuildingTypes) i) : NULL;
}


CvInfoBase* CyGlobalContext::getUnitCombatInfo(int i) const
{
	return (i>=0 && i<GC.getNumUnitCombatInfos()) ? &GC.getUnitCombatInfo((UnitCombatTypes)i) : NULL;
}

//TB Promotion Line Mod begin
CvPromotionLineInfo* CyGlobalContext::getPromotionLineInfo(int i) const
{
	return (i>=0 && i<GC.getNumPromotionLineInfos()) ? &GC.getPromotionLineInfo((PromotionLineTypes)i) : NULL;
}
//TB Promotion Line Mod end

//CvTraitOptionEditsInfo* CyGlobalContext::getTraitOptionEditsInfo(int i) const
//{
//	return (i>=0 && i<GC.getNumTraitOptionEditsInfos()) ? &GC.getTraitOptionEditsInfo((TraitOptionEditsTypes)i) : NULL;
//}

CvMapCategoryInfo* CyGlobalContext::getMapCategoryInfo(int i) const
{
	return (i>=0 && i<GC.getNumMapCategoryInfos()) ? &GC.getMapCategoryInfo((MapCategoryTypes)i) : NULL;
}

CvIdeaClassInfo* CyGlobalContext::getIdeaClassInfo(int i) const
{
	return (i>=0 && i<GC.getNumIdeaClassInfos()) ? &GC.getIdeaClassInfo((IdeaClassTypes)i) : NULL;
}

CvIdeaInfo* CyGlobalContext::getIdeaInfo(int i) const
{
	return (i>=0 && i<GC.getNumIdeaInfos()) ? &GC.getIdeaInfo((IdeaTypes)i) : NULL;
}

CvInfoBase* CyGlobalContext::getDomainInfo(int i) const
{
	return (i>=0 && i<NUM_DOMAIN_TYPES) ? &GC.getDomainInfo((DomainTypes)i) : NULL;
}


CvActionInfo* CyGlobalContext::getActionInfo(int i) const
{
	return (i>=0 && i<GC.getNumActionInfos()) ? &GC.getActionInfo(i) : NULL;
}

CvAutomateInfo* CyGlobalContext::getAutomateInfo(int i) const
{
	return (i>=0 && i<GC.getNumAutomateInfos()) ? &GC.getAutomateInfo(i) : NULL;
}

CvCommandInfo* CyGlobalContext::getCommandInfo(int i) const
{
	return (i>=0 && i<NUM_COMMAND_TYPES) ? &GC.getCommandInfo((CommandTypes)i) : NULL;
}

CvControlInfo* CyGlobalContext::getControlInfo(int i) const
{
	return (i>=0 && i<NUM_CONTROL_TYPES) ? &GC.getControlInfo((ControlTypes)i) : NULL;
}

CvMissionInfo* CyGlobalContext::getMissionInfo(int i) const
{
	return (i>=0 && i<GC.getNumMissionInfos()) ? &GC.getMissionInfo((MissionTypes) i) : NULL;
}

CvPromotionInfo* CyGlobalContext::getPromotionInfo(int i) const
{
	return (i>=0 && i<GC.getNumPromotionInfos()) ? &GC.getPromotionInfo((PromotionTypes) i) : NULL;
}


CvTechInfo* CyGlobalContext::getTechInfo(int i) const
{
	return (i>=0 && i<GC.getNumTechInfos()) ? &GC.getTechInfo((TechTypes) i) : NULL;
}


CvSpecialBuildingInfo* CyGlobalContext::getSpecialBuildingInfo(int i) const
{
	return (i>=0 && i<GC.getNumSpecialBuildingInfos()) ? &GC.getSpecialBuildingInfo((SpecialBuildingTypes) i) : NULL;
}


CvReligionInfo* CyGlobalContext::getReligionInfo(int i) const
{
	return (i>=0 && i<GC.getNumReligionInfos()) ? &GC.getReligionInfo((ReligionTypes) i) : NULL;
}


CvCorporationInfo* CyGlobalContext::getCorporationInfo(int i) const
{
	return (i>=0 && i<GC.getNumCorporationInfos()) ? &GC.getCorporationInfo((CorporationTypes) i) : NULL;
}


CvSpecialistInfo* CyGlobalContext::getSpecialistInfo(int i) const
{
	return (i>=0 && i<GC.getNumSpecialistInfos()) ? &GC.getSpecialistInfo((SpecialistTypes) i) : NULL;
}


CvCivicOptionInfo* CyGlobalContext::getCivicOptionInfo(int i) const
{
	return (i>=0 && i<GC.getNumCivicOptionInfos()) ? &GC.getCivicOptionInfo((CivicOptionTypes) i) : NULL;
}


CvCivicInfo* CyGlobalContext::getCivicInfo(int i) const
{
	return (i>=0 && i<GC.getNumCivicInfos()) ? &GC.getCivicInfo((CivicTypes) i) : NULL;
}

CvDiplomacyInfo* CyGlobalContext::getDiplomacyInfo(int i) const
{
	return (i>=0 && i<GC.getNumDiplomacyInfos()) ? &GC.getDiplomacyInfo(i) : NULL;
}

CvHurryInfo* CyGlobalContext::getHurryInfo(int i) const
{
	return (i>=0 && i<GC.getNumHurryInfos()) ? &GC.getHurryInfo((HurryTypes) i) : NULL;
}


CvProjectInfo* CyGlobalContext::getProjectInfo(int i) const
{
	return (i>=0 && i<GC.getNumProjectInfos()) ? &GC.getProjectInfo((ProjectTypes) i) : NULL;
}


CvVoteInfo* CyGlobalContext::getVoteInfo(int i) const
{
	return (i>=0 && i<GC.getNumVoteInfos()) ? &GC.getVoteInfo((VoteTypes) i) : NULL;
}


CvProcessInfo* CyGlobalContext::getProcessInfo(int i) const
{
	return (i>=0 && i<GC.getNumProcessInfos()) ? &GC.getProcessInfo((ProcessTypes) i) : NULL;
}

CvAnimationPathInfo* CyGlobalContext::getAnimationPathInfo(int i) const
{
	return (i>=0 && i<GC.getNumAnimationPathInfos()) ? &GC.getAnimationPathInfo((AnimationPathTypes)i) : NULL;
}


CvEmphasizeInfo* CyGlobalContext::getEmphasizeInfo(int i) const
{
	return (i>=0 && i<GC.getNumEmphasizeInfos()) ? &GC.getEmphasizeInfo((EmphasizeTypes) i) : NULL;
}


CvCultureLevelInfo* CyGlobalContext::getCultureLevelInfo(int i) const
{
	return (i>=0 && i<GC.getNumCultureLevelInfos()) ? &GC.getCultureLevelInfo((CultureLevelTypes) i) : NULL;
}


CvUpkeepInfo* CyGlobalContext::getUpkeepInfo(int i) const
{
	return (i>=0 && i<GC.getNumUpkeepInfos()) ? &GC.getUpkeepInfo((UpkeepTypes) i) : NULL;
}


CvVictoryInfo* CyGlobalContext::getVictoryInfo(int i) const
{
	return (i>=0 && i<GC.getNumVictoryInfos()) ? &GC.getVictoryInfo((VictoryTypes) i) : NULL;
}


CvEraInfo* CyGlobalContext::getEraInfo(int i) const
{
	return (i>=0 && i<GC.getNumEraInfos()) ? &GC.getEraInfo((EraTypes) i) : NULL;
}


CvWorldInfo* CyGlobalContext::getWorldInfo(int i) const
{
	return (i>=0 && i<GC.getNumWorldInfos()) ? &GC.getWorldInfo((WorldSizeTypes) i) : NULL;
}


CvClimateInfo* CyGlobalContext::getClimateInfo(int i) const
{
	return (i>=0 && i<GC.getNumClimateInfos()) ? &GC.getClimateInfo((ClimateTypes) i) : NULL;
}


CvSeaLevelInfo* CyGlobalContext::getSeaLevelInfo(int i) const
{
	return (i>=0 && i<GC.getNumSeaLevelInfos()) ? &GC.getSeaLevelInfo((SeaLevelTypes) i) : NULL;
}


CvInfoBase* CyGlobalContext::getUnitAIInfo(int i) const
{
	return (i>=0 && i<NUM_UNITAI_TYPES) ? &GC.getUnitAIInfo((UnitAITypes)i) : NULL;
}


CvColorInfo* CyGlobalContext::getColorInfo(int i) const
{
	return (i>=0 && i<GC.getNumColorInfos()) ? &GC.getColorInfo((ColorTypes)i) : NULL;
}

CvPropertyInfo* CyGlobalContext::getPropertyInfo(int i) const
{
	return (i>=0 && i<GC.getNumPropertyInfos()) ? &GC.getPropertyInfo((PropertyTypes)i) : NULL;
}

int CyGlobalContext::getInfoTypeForString(const char* szInfoType) const
{
	return GC.getInfoTypeForString(szInfoType);
}
/************************************************************************************************/
/* Afforess	                  Start		 03/18/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
int CyGlobalContext::getInfoTypeForStringWithHiddenAssert(const char* szInfoType) const
{
	return GC.getInfoTypeForString(szInfoType, true);
}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

int CyGlobalContext::getTypesEnum(const char* szType) const
{
	return GC.getTypesEnum(szType);
}


CvPlayerColorInfo* CyGlobalContext::getPlayerColorInfo(int i) const
{
	return (i>=0 && i<GC.getNumPlayerColorInfos()) ? &GC.getPlayerColorInfo((PlayerColorTypes)i) : NULL;
}


CvInfoBase* CyGlobalContext::getHints(int i) const
{
	return ((i >= 0 && i < GC.getNumHints()) ? &GC.getHints(i) : NULL);
}


CvMainMenuInfo* CyGlobalContext::getMainMenus(int i) const
{
	return ((i >= 0 && i < GC.getNumMainMenus()) ? &GC.getMainMenus(i) : NULL);
}


CvVoteSourceInfo* CyGlobalContext::getVoteSourceInfo(int i) const
{
	return ((i >= 0 && i < GC.getNumVoteSourceInfos()) ? &GC.getVoteSourceInfo((VoteSourceTypes)i) : NULL);
}


CvInvisibleInfo* CyGlobalContext::getInvisibleInfo(int i) const
{
	return ((i >= 0 && i < GC.getNumInvisibleInfos()) ? &GC.getInvisibleInfo((InvisibleTypes)i) : NULL);
}


CvInfoBase* CyGlobalContext::getAttitudeInfo(int i) const
{
	return (i>=0 && i<NUM_ATTITUDE_TYPES) ? &GC.getAttitudeInfo((AttitudeTypes)i) : NULL;
}


CvInfoBase* CyGlobalContext::getMemoryInfo(int i) const
{
	return (i>=0 && i<NUM_MEMORY_TYPES) ? &GC.getMemoryInfo((MemoryTypes)i) : NULL;
}


CvPlayerOptionInfo* CyGlobalContext::getPlayerOptionInfo(int i) const
{
	return &GC.getPlayerOptionInfo((PlayerOptionTypes)i);
}


CvGraphicOptionInfo* CyGlobalContext::getGraphicOptionInfo(int i) const
{
	return &GC.getGraphicOptionInfo((GraphicOptionTypes)i);
}


CvInfoBase* CyGlobalContext::getConceptInfo(int i) const
{
	return (i>=0 && i<GC.getNumConceptInfos()) ? &GC.getConceptInfo((ConceptTypes)i) : NULL;
}


CvInfoBase* CyGlobalContext::getNewConceptInfo(int i) const
{
	return (i>=0 && i<GC.getNumNewConceptInfos()) ? &GC.getNewConceptInfo((NewConceptTypes)i) : NULL;
}


CvInfoBase* CyGlobalContext::getCityTabInfo(int i) const
{
	return (i>=0 && i<GC.getNumCityTabInfos()) ? &GC.getCityTabInfo((CityTabTypes)i) : NULL;
}


CvInfoBase* CyGlobalContext::getCalendarInfo(int i) const
{
	return (i>=0 && i<GC.getNumCalendarInfos()) ? &GC.getCalendarInfo((CalendarTypes)i) : NULL;
}


CvInfoBase* CyGlobalContext::getGameOptionInfo(int i) const
{
	return (i>=0 && i<GC.getNumGameOptionInfos()) ? &GC.getGameOptionInfo((GameOptionTypes)i) : NULL;
}


CvInfoBase* CyGlobalContext::getMPOptionInfo(int i) const
{
	return (i>=0 && i<GC.getNumMPOptionInfos()) ? &GC.getMPOptionInfo((MultiplayerOptionTypes)i) : NULL;
}


CvInfoBase* CyGlobalContext::getForceControlInfo(int i) const
{
	return (i>=0 && i<GC.getNumForceControlInfos()) ? &GC.getForceControlInfo((ForceControlTypes)i) : NULL;
}


CvInfoBase* CyGlobalContext::getSeasonInfo(int i) const
{
	return (i>=0 && i<GC.getNumSeasonInfos()) ? &GC.getSeasonInfo((SeasonTypes)i) : NULL;
}


CvInfoBase* CyGlobalContext::getMonthInfo(int i) const
{
	return (i>=0 && i<GC.getNumMonthInfos()) ? &GC.getMonthInfo((MonthTypes)i) : NULL;
}


CvInfoBase* CyGlobalContext::getDenialInfo(int i) const
{
	return (i>=0 && i<GC.getNumDenialInfos()) ? &GC.getDenialInfo((DenialTypes)i) : NULL;
}


CvQuestInfo* CyGlobalContext::getQuestInfo(int i) const
{
	return (i>=0 && i<GC.getNumQuestInfos()) ? &GC.getQuestInfo(i) : NULL;
}


CvTutorialInfo* CyGlobalContext::getTutorialInfo(int i) const
{
	return (i>=0 && i<GC.getNumTutorialInfos()) ? &GC.getTutorialInfo(i) : NULL;
}


CvEventTriggerInfo* CyGlobalContext::getEventTriggerInfo(int i) const
{
	return (i>=0 && i<GC.getNumEventTriggerInfos()) ? &GC.getEventTriggerInfo((EventTriggerTypes)i) : NULL;
}


CvEventInfo* CyGlobalContext::getEventInfo(int i) const
{
	return (i>=0 && i<GC.getNumEventInfos()) ? &GC.getEventInfo((EventTypes)i) : NULL;
}


CvEspionageMissionInfo* CyGlobalContext::getEspionageMissionInfo(int i) const
{
	return (i>=0 && i<GC.getNumEspionageMissionInfos()) ? &GC.getEspionageMissionInfo((EspionageMissionTypes)i) : NULL;
}


CvUnitArtStyleTypeInfo* CyGlobalContext::getUnitArtStyleTypeInfo(int i) const
{
	return (i>=0 && i<GC.getNumUnitArtStyleTypeInfos()) ? &GC.getUnitArtStyleTypeInfo((UnitArtStyleTypes)i) : NULL;
}


CvArtInfoInterface* CyGlobalContext::getInterfaceArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumInterfaceArtInfos()) ? &ARTFILEMGR.getInterfaceArtInfo(i) : NULL;
}


CvArtInfoMovie* CyGlobalContext::getMovieArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumMovieArtInfos()) ? &ARTFILEMGR.getMovieArtInfo(i) : NULL;
}


CvArtInfoMisc* CyGlobalContext::getMiscArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumMiscArtInfos()) ? &ARTFILEMGR.getMiscArtInfo(i) : NULL;
}


CvArtInfoUnit* CyGlobalContext::getUnitArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumUnitArtInfos()) ? &ARTFILEMGR.getUnitArtInfo(i) : NULL;
}


CvArtInfoBuilding* CyGlobalContext::getBuildingArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumBuildingArtInfos()) ? &ARTFILEMGR.getBuildingArtInfo(i) : NULL;
}


CvArtInfoCivilization* CyGlobalContext::getCivilizationArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumCivilizationArtInfos()) ? &ARTFILEMGR.getCivilizationArtInfo(i) : NULL;
}


CvArtInfoLeaderhead* CyGlobalContext::getLeaderheadArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumLeaderheadArtInfos()) ? &ARTFILEMGR.getLeaderheadArtInfo(i) : NULL;
}


CvArtInfoBonus* CyGlobalContext::getBonusArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumBonusArtInfos()) ? &ARTFILEMGR.getBonusArtInfo(i) : NULL;
}


CvArtInfoImprovement* CyGlobalContext::getImprovementArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumImprovementArtInfos()) ? &ARTFILEMGR.getImprovementArtInfo(i) : NULL;
}


CvArtInfoTerrain* CyGlobalContext::getTerrainArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumTerrainArtInfos()) ? &ARTFILEMGR.getTerrainArtInfo(i) : NULL;
}


CvArtInfoFeature* CyGlobalContext::getFeatureArtInfo(int i) const
{
	return (i>=0 && i<ARTFILEMGR.getNumFeatureArtInfos()) ? &ARTFILEMGR.getFeatureArtInfo(i) : NULL;
}


CvGameSpeedInfo* CyGlobalContext::getGameSpeedInfo(int i) const
{
	return &(GC.getGameSpeedInfo((GameSpeedTypes) i));
}

CvTurnTimerInfo* CyGlobalContext::getTurnTimerInfo(int i) const
{
	return &(GC.getTurnTimerInfo((TurnTimerTypes) i));
}

bool CyGlobalContext::isShiftDown() const {
	return gDLL->shiftKey();
}
bool CyGlobalContext::isAltDown() const {
	return gDLL->altKey();
}
bool CyGlobalContext::isCtrlDown() const {
	return gDLL->ctrlKey();
}
