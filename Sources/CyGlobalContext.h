#pragma once

#ifndef CyGlobalContext_h
#define CyGlobalContext_h

//
// Python wrapper class for global vars and fxns
// Passed to Python
//

#include "CvGlobals.h"

class CvArtFileMgr;
class CyGame;
class CyMap;
class CyPlayer;
class CvRandom;
class CyTeam;

class CyGlobalContext
{
public:
	CyGlobalContext();
	virtual ~CyGlobalContext();

	static CyGlobalContext& getInstance();		// singleton accessor

	bool isDebugBuild() const;
	CyGame* getCyGame() const;
	CyMap* getCyMap() const;

	void switchMap(int iMap);
	CyMap* getMapByIndex(int iIndex);

	CyPlayer* getCyPlayer(int idx) const;
	CyPlayer* getCyActivePlayer() const;
	CvRandom& getCyASyncRand() const;
	CyTeam* getCyTeam(int i) const;

	int getInfoTypeForString(const char* szInfoType) const;
	int getInfoTypeForStringWithHiddenAssert(const char* szInfoType) const;
	int getTypesEnum(const char* szType) const;

	const CvMapInfo& getMapInfo(int i) const;
	const CvEffectInfo* getEffectInfo(int i) const;
	const CvTerrainInfo* getTerrainInfo(int i) const;
	const CvBonusClassInfo* getBonusClassInfo(int i) const;
	const CvBonusInfo* getBonusInfo(int i) const;
	const CvFeatureInfo* getFeatureInfo(int i) const;
	const CvCivilizationInfo* getCivilizationInfo(int idx) const;
	const CvLeaderHeadInfo* getLeaderHeadInfo(int i) const;
	const CvTraitInfo* getTraitInfo(int i) const;
	const CvUnitInfo* getUnitInfo(int i) const;
	const CvSpecialUnitInfo* getSpecialUnitInfo(int i) const;
	const CvYieldInfo* getYieldInfo(int i) const;
	const CvCommerceInfo* getCommerceInfo(int i) const;
	const CvRouteInfo* getRouteInfo(int i) const;
	const CvImprovementInfo* getImprovementInfo(int i) const;
	const CvGoodyInfo* getGoodyInfo(int i) const;
	const CvBuildInfo* getBuildInfo(int i) const;
	const CvHandicapInfo* getHandicapInfo(int i) const;
	const CvGameSpeedInfo& getGameSpeedInfo(int i) const;
	const CvMissionInfo* getMissionInfo(int i) const;
	const CvCommandInfo* getCommandInfo(int i) const;
	const CvActionInfo* getActionInfo(int i) const;
	const CvInfoBase* getUnitCombatInfo(int i) const;
	const CvPromotionLineInfo* getPromotionLineInfo(int i) const;
	const CvInfoBase* getDomainInfo(int i) const;
	const CvBuildingInfo* getBuildingInfo(int i) const;
	const CvCivicOptionInfo* getCivicOptionInfo(int i) const;
	const CvCivicInfo* getCivicInfo(int i) const;
	const CvDiplomacyInfo* getDiplomacyInfo(int i) const;
	const CvProjectInfo* getProjectInfo(int i) const;
	const CvVoteInfo* getVoteInfo(int i) const;
	const CvProcessInfo* getProcessInfo(int i) const;
	const CvSpecialistInfo* getSpecialistInfo(int i) const;
	const CvReligionInfo* getReligionInfo(int i) const;
	const CvCorporationInfo* getCorporationInfo(int i) const;
	const CvControlInfo* getControlInfo(int i) const;
	const CvTechInfo* getTechInfo(int i) const;
	const CvSpecialBuildingInfo* getSpecialBuildingInfo(int i) const;
	const CvPromotionInfo* getPromotionInfo(int i) const;
	const CvEmphasizeInfo * getEmphasizeInfo(int i) const;
	const CvUpkeepInfo * getUpkeepInfo(int i) const;
	const CvCultureLevelInfo * getCultureLevelInfo(int i) const;
	const CvEraInfo * getEraInfo(int i) const;
	const CvVictoryInfo * getVictoryInfo(int i) const;
	const CvWorldInfo * getWorldInfo(int i) const;
	const CvClimateInfo * getClimateInfo(int i) const;
	const CvSeaLevelInfo * getSeaLevelInfo(int i) const;
	const CvInfoBase * getUnitAIInfo(int i) const;
	const CvColorInfo* getColorInfo(int i) const;
	const CvUnitArtStyleTypeInfo* getUnitArtStyleTypeInfo(int i) const;
	const CvPropertyInfo* getPropertyInfo(int i) const;
	const CvPlayerColorInfo* getPlayerColorInfo(int i) const;
	const CvMainMenuInfo* getMainMenus(int i) const;
	const CvVoteSourceInfo* getVoteSourceInfo(int i) const;
	const CvInfoBase* getAttitudeInfo(int i) const;
	const CvInfoBase* getMemoryInfo(int i) const;
	const CvInfoBase* getConceptInfo(int i) const;
	const CvInfoBase* getNewConceptInfo(int i) const;
	const CvInfoBase* getCalendarInfo(int i) const;
	const CvInfoBase* getGameOptionInfo(int i) const;
	const CvInfoBase* getMPOptionInfo(int i) const;
	const CvInfoBase* getForceControlInfo(int i) const;
	const CvInfoBase* getSeasonInfo(int i) const;
	const CvInfoBase* getDenialInfo(int i) const;
	const CvEventTriggerInfo* getEventTriggerInfo(int i) const;
	const CvEventInfo* getEventInfo(int i) const;
	const CvEspionageMissionInfo* getEspionageMissionInfo(int i) const;
	const CvHurryInfo* getHurryInfo(int i) const;
	const CvPlayerOptionInfo& getPlayerOptionInfo(int i) const;
	const CvGraphicOptionInfo& getGraphicOptionInfo(int i) const;

	// ArtInfos
	const CvArtInfoInterface* getInterfaceArtInfo(int i) const;
	const CvArtInfoMovie* getMovieArtInfo(int i) const;
	const CvArtInfoMisc* getMiscArtInfo(int i) const;
	const CvArtInfoUnit* getUnitArtInfo(int i) const;
	const CvArtInfoBuilding* getBuildingArtInfo(int i) const;
	const CvArtInfoCivilization* getCivilizationArtInfo(int i) const;
	const CvArtInfoBonus* getBonusArtInfo(int i) const;
	const CvArtInfoImprovement* getImprovementArtInfo(int i) const;

	const char* getArtStyleTypes(int i) const { return GC.getArtStyleTypes((ArtStyleTypes) i); }
	const char* getFlavorTypes(int i) const { return GC.getFlavorTypes((FlavorTypes) i); }
	const char* getDiplomacyPowerTypes(int i) const { return GC.getDiplomacyPowerTypes((DiplomacyPowerTypes) i); }

	int getNumEffectInfos() const { return GC.getNumEffectInfos(); }
	int getNumTerrainInfos() const { return GC.getNumTerrainInfos(); }
	int getNumSpecialBuildingInfos() const { return GC.getNumSpecialBuildingInfos(); }
	int getNumBonusInfos() const { return GC.getNumBonusInfos(); };
	int getNumPlayableCivilizationInfos() const { return GC.getNumPlayableCivilizationInfos(); }
	int getNumCivilizatonInfos() const { return GC.getNumCivilizationInfos(); }
	int getNumLeaderHeadInfos() const { return GC.getNumLeaderHeadInfos(); }
	int getNumTraitInfos() const { return GC.getNumTraitInfos(); }
	int getNumUnitInfos() const { return GC.getNumUnitInfos(); }
	int getNumSpecialUnitInfos() const { return GC.getNumSpecialUnitInfos(); }
	int getNumRouteInfos() const { return GC.getNumRouteInfos(); }
	int getNumFeatureInfos() const { return GC.getNumFeatureInfos(); }
	int getNumImprovementInfos() const { return GC.getNumImprovementInfos(); }
	int getNumGoodyInfos() const { return GC.getNumGoodyInfos(); }
	int getNumBuildInfos() const { return GC.getNumBuildInfos(); }
	int getNumHandicapInfos() const { return GC.getNumHandicapInfos(); }
	int getNumGameSpeedInfos() const { return GC.getNumGameSpeedInfos(); }
	int getNumBuildingInfos() const { return GC.getNumBuildingInfos(); }
	int getNumUnitCombatInfos() const { return GC.getNumUnitCombatInfos(); }
	int getNumPromotionLineInfos() const { return GC.getNumPromotionLineInfos(); }
	int getNumCommandInfos() const { return NUM_COMMAND_TYPES; }
	int getNumControlInfos() const { return NUM_CONTROL_TYPES; }
	int getNumMissionInfos() const { return GC.getNumMissionInfos(); }
	int getNumActionInfos() const { return GC.getNumActionInfos(); }
	int getNumPromotionInfos() const { return GC.getNumPromotionInfos(); }
	int getNumTechInfos() const { return GC.getNumTechInfos(); }
	int getNumReligionInfos() const { return GC.getNumReligionInfos(); }
	int getNumCorporationInfos() const { return GC.getNumCorporationInfos(); }
	int getNumSpecialistInfos() const { return GC.getNumSpecialistInfos(); }
	int getNumCivicInfos() const { return GC.getNumCivicInfos(); }
	int getNumDiplomacyInfos() const { return GC.getNumDiplomacyInfos(); }
	int getNumCivicOptionInfos() const { return GC.getNumCivicOptionInfos(); }
	int getNumProjectInfos() const { return GC.getNumProjectInfos(); }
	int getNumVoteInfos() const { return GC.getNumVoteInfos(); }
	int getNumProcessInfos() const { return GC.getNumProcessInfos(); }
	int getNumEmphasizeInfos() const { return GC.getNumEmphasizeInfos(); }
	int getNumHurryInfos() const { return GC.getNumHurryInfos(); }
	int getNumUpkeepInfos() const { return GC.getNumUpkeepInfos(); }
	int getNumCultureLevelInfos() const { return GC.getNumCultureLevelInfos(); }
	int getNumEraInfos() const { return GC.getNumEraInfos(); }
	int getNumVictoryInfos() const { return GC.getNumVictoryInfos(); }
	int getNumWorldInfos() const { return GC.getNumWorldInfos(); }
	int getNumSeaLevelInfos() const { return GC.getNumSeaLevelInfos(); }
	int getNumClimateInfos() const { return GC.getNumClimateInfos(); }
	int getNumConceptInfos() const { return GC.getNumConceptInfos(); }
	int getNumNewConceptInfos() const { return GC.getNumNewConceptInfos(); }
	int getNumCalendarInfos() const { return GC.getNumCalendarInfos(); }
	int getNumGameOptionInfos() const { return GC.getNumGameOptionInfos(); }
	int getNumMPOptionInfos() const { return GC.getNumMPOptionInfos(); }
	int getNumForceControlInfos() const { return GC.getNumForceControlInfos(); }
	int getNumSeasonInfos() const { return GC.getNumSeasonInfos(); }
	int getNumDenialInfos() const { return GC.getNumDenialInfos(); }
	int getNumEventTriggerInfos() const { return GC.getNumEventTriggerInfos(); }
	int getNumEventInfos() const { return GC.getNumEventInfos(); }
	int getNumEspionageMissionInfos() const { return GC.getNumEspionageMissionInfos(); }
	int getNumMainMenus() const { return GC.getNumMainMenus(); }
	int getNumVoteSourceInfos() const { return GC.getNumVoteSourceInfos(); }
	int getNumPropertyInfos() const { return GC.getNumPropertyInfos(); }
	int getNumPlayerColorInfos() const { return GC.getNumPlayerColorInfos(); }

	int getNumAnimationOperatorTypes() const { return GC.getNumAnimationOperatorTypes(); }
	int getNumArtStyleTypes() const { return GC.getNumArtStyleTypes(); }
	int getNumFlavorTypes() const { return GC.getNumFlavorTypes(); }
	int getNumFootstepAudioTypes() const { return GC.getNumFootstepAudioTypes(); }

	//////////////////////
	// Globals Defines
	//////////////////////

	int getDefineINT( const char * szName ) const { return GC.getDefineINT( szName ); }
	float getDefineFLOAT( const char * szName ) const { return GC.getDefineFLOAT( szName ); }

	void setDefineINT( const char * szName, int iValue ) { return GC.setDefineINT( szName, iValue ); }
	void setDefineFLOAT( const char * szName, float fValue ) { return GC.setDefineFLOAT( szName, fValue ); }

	bool isDCM_AIR_BOMBING() const { return GC.isDCM_AIR_BOMBING(); }
	bool isDCM_RANGE_BOMBARD() const { return GC.isDCM_RANGE_BOMBARD(); }
	bool isDCM_ATTACK_SUPPORT() const { return GC.isDCM_ATTACK_SUPPORT(); }
	bool isDCM_OPP_FIRE() const { return GC.isDCM_OPP_FIRE(); }
	bool isDCM_ACTIVE_DEFENSE() const { return GC.isDCM_ACTIVE_DEFENSE(); }
	bool isDCM_FIGHTER_ENGAGE() const { return GC.isDCM_FIGHTER_ENGAGE(); }

	bool isIDW_ENABLED() const { return GC.isIDW_ENABLED(); }
	bool isIDW_EMERGENCY_DRAFT_ENABLED() const { return GC.isIDW_EMERGENCY_DRAFT_ENABLED(); }
	bool isIDW_NO_BARBARIAN_INFLUENCE() const { return GC.isIDW_NO_BARBARIAN_INFLUENCE(); }
	bool isIDW_NO_NAVAL_INFLUENCE() const { return GC.isIDW_NO_NAVAL_INFLUENCE(); }
	bool isIDW_PILLAGE_INFLUENCE_ENABLED() const { return GC.isIDW_PILLAGE_INFLUENCE_ENABLED(); }

	bool isSS_ENABLED() const { return GC.isSS_ENABLED(); }
	bool isSS_BRIBE() const { return GC.isSS_BRIBE(); }
	bool isSS_ASSASSINATE() const { return GC.isSS_ASSASSINATE(); }

	int getMAX_PC_PLAYERS() const { return GC.getMAX_PC_PLAYERS(); }
	int getMAX_PLAYERS() const { return GC.getMAX_PLAYERS(); }
	int getMAX_PC_TEAMS() const { return GC.getMAX_PC_TEAMS(); }
	int getMAX_TEAMS() const { return GC.getMAX_TEAMS(); }
	int getBARBARIAN_PLAYER() const { return GC.getBARBARIAN_PLAYER(); }
	int getBARBARIAN_TEAM() const { return GC.getBARBARIAN_TEAM(); }

	int getNUM_CITY_PLOTS() const { return NUM_CITY_PLOTS; }

	void setIsBug() { GC.setIsBug(); }

	void setNoUpdateDefineFLOAT( const char * szName, float fValue ) { return GC.setDefineFLOAT( szName, fValue, false ); }

#define DECLARE_CY_GET_METHOD(dataType, VAR) \
	int get##VAR() const { return (int)GC.get##VAR(); }

	DO_FOR_EACH_EXPOSED_INT_GLOBAL_DEFINE(DECLARE_CY_GET_METHOD)
	DO_FOR_EACH_EXPOSED_INFO_TYPE(DECLARE_CY_GET_METHOD)
};

#endif	// CyGlobalContext_h
