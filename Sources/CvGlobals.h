#pragma once

// cvGlobals.h
//ReplacementStep: search down here for 'CvInfoReplacements'

#ifndef CIV4_GLOBALS_H
#define CIV4_GLOBALS_H

//
// 'global' vars for Civ IV.  singleton class.
// All globals and global types should be contained in this class
//

class FProfiler;
class CvDLLUtilityIFaceBase;
class CvRandom;
class CvGameAI;
class CMessageControl;
class CvDropMgr;
class CMessageQueue;
class CvSetupData;
class CvInitCore;
class CvMessageCodeTranslator;
class CvPortal;
class CvStatsReporter;
class CvDLLInterfaceIFaceBase;
class CvPlayerAI;
class CvDiplomacyScreen;
class CvCivicsScreen;
class CvWBUnitEditScreen;
class CvWBCityEditScreen;
class CMPDiplomacyScreen;
class FMPIManager;
class FAStar;
class CvInterface;
class CMainMenu;
class CvEngine;
class CvArtFileMgr;
class FVariableSystem;
class CvMap;
class CvMapExternal;
class CvViewport;
class CvPlayerAI;
class CvTeamAI;
class CvInterfaceModeInfo;
class CvWorldInfo;
class CvClimateInfo;
class CvSeaLevelInfo;
class CvColorInfo;
class CvPlayerColorInfo;
class CvAdvisorInfo;
class CvRouteModelInfo;
class CvRiverInfo;
class CvRiverModelInfo;
class CvWaterPlaneInfo;
class CvTerrainPlaneInfo;
class CvCameraOverlayInfo;
class CvAnimationPathInfo;
class CvAnimationCategoryInfo;
class CvEntityEventInfo;
class CvEffectInfo;
class CvAttachableInfo;
class CvCameraInfo;
class CvUnitFormationInfo;
class CvGameText;
class CvLandscapeInfo;
class CvTerrainInfo;
class CvBonusClassInfo;
class CvBonusInfo;
class CvFeatureInfo;
class CvCivilizationInfo;
class CvLeaderHeadInfo;
class CvTraitInfo;
class CvCursorInfo;
class CvThroneRoomCamera;
class CvThroneRoomInfo;
class CvThroneRoomStyleInfo;
class CvSlideShowInfo;
class CvSlideShowRandomInfo;
class CvWorldPickerInfo;
class CvSpaceShipInfo;
class CvUnitInfo;
class CvSpawnInfo;
class CvSpecialUnitInfo;
class CvInfoBase;
class CvYieldInfo;
class CvCommerceInfo;
class CvRouteInfo;
class CvImprovementInfo;
class CvGoodyInfo;
class CvBuildInfo;
class CvHandicapInfo;
class CvGameSpeedInfo;
class CvTurnTimerInfo;
class CvProcessInfo;
class CvVoteInfo;
class CvProjectInfo;
class CvBuildingInfo;
class CvSpecialBuildingInfo;
class CvActionInfo;
class CvMissionInfo;
class CvControlInfo;
class CvCommandInfo;
class CvAutomateInfo;
class CvPromotionInfo;
class CvTechInfo;
class CvReligionInfo;
class CvCorporationInfo;
class CvSpecialistInfo;
class CvCivicOptionInfo;
class CvCivicInfo;
class CvDiplomacyInfo;
class CvEraInfo;
class CvHurryInfo;
class CvEmphasizeInfo;
class CvUpkeepInfo;
class CvCultureLevelInfo;
class CvVictoryInfo;
class CvQuestInfo;
class CvGameOptionInfo;
class CvMPOptionInfo;
class CvForceControlInfo;
class CvPlayerOptionInfo;
class CvGraphicOptionInfo;
class CvTutorialInfo;
class CvEventTriggerInfo;
class CvEventInfo;
class CvEspionageMissionInfo;
class CvUnitArtStyleTypeInfo;
class CvVoteSourceInfo;
class CvMainMenuInfo;
class CvPropertyInfo;
class CvOutcomeInfo;
class CvCivNameInfo;
class CvUnitCombatInfo;
//TB Promotion Line Mod begin
class CvPromotionLineInfo;
//TB Promotion Line Mod end
class CvMapCategoryInfo;
class CvIdeaClassInfo;
class CvIdeaInfo;
class CvInvisibleInfo;
//class CvTraitOptionEditsInfo;
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 10/24/07                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
// MLF loading
class CvModLoadControlInfo;
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
class CvMapInfo;
class CvMapSwitchInfo;
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

#include "CvInfoReplacements.h"
#include <stack>

extern CvDLLUtilityIFaceBase* g_DLL;

class cvInternalGlobals
{
//	friend class CvDLLUtilityIFace;
	friend class CvXMLLoadUtility;
public:

	// singleton accessor
	inline static cvInternalGlobals& getInstance();

	cvInternalGlobals();
	virtual ~cvInternalGlobals();

	void init();
	void uninit();
	void clearTypesMap();

	CvDiplomacyScreen* getDiplomacyScreen() const;
	CMPDiplomacyScreen* getMPDiplomacyScreen() const;

	FMPIManager*& getFMPMgrPtr();
	CvPortal& getPortal();
	CvSetupData& getSetupData();
	CvInitCore& getInitCore();
	CvInitCore& getLoadedInitCore();
	CvInitCore& getIniInitCore();
	CvMessageCodeTranslator& getMessageCodes();
	CvStatsReporter& getStatsReporter();
	CvStatsReporter* getStatsReporterPtr() const;
	CvInterface& getInterface();
	CvInterface* getInterfacePtr() const;
	int getMaxCivPlayers() const;
	int getMaxPCPlayers() const;

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	inline CvMap& getMap() const;
	CvViewport* getCurrentViewport();
	int	getViewportSizeX() const;
	int	getViewportSizeY() const;
	int getViewportSelectionBorder() const;
	int getViewportCenteringBorder() const;
	CvMapExternal& getMapExternal();

	bool bugInitCalled() const;
	bool multiMapsEnabled() const;
	bool viewportsEnabled() const;
	bool getReprocessGreatWallDynamically() const;
	int getNumMapInfos() const;
	int getNumMapSwitchInfos() const;
	std::vector<CvMapInfo*>& getMapInfos();
	std::vector<CvMapSwitchInfo*>& getMapSwitchInfos();
	CvMapInfo& getMapInfo(const MapTypes eMap) const;
	CvMapSwitchInfo& getMapSwitchInfo(const MapSwitchTypes eMapSwitch) const;
	
	void switchMap(MapTypes eMap);
	CvMap& getMapByIndex(MapTypes eIndex);
	void updateMaps();
	const std::vector<CvMap*>& getMaps() const;
	void initializeMap(MapTypes eMap);
	bool mapInitialized(MapTypes eMap) const;
	void clearSigns();
	void reprocessSigns();
	void setResourceLayer(bool bOn);
	bool getResourceLayer() const;
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/
	inline CvGameAI& getGame() const { return *m_game; }
	CvGameAI* getGamePointer();
	CvRandom& getASyncRand();
	CMessageQueue& getMessageQueue();
	CMessageQueue& getHotMessageQueue();
	CMessageControl& getMessageControl();
	CvDropMgr& getDropMgr();
	FAStar& getPathFinder();
	FAStar& getInterfacePathFinder();
	FAStar& getStepFinder();
	FAStar& getRouteFinder();
	FAStar& getBorderFinder();
	FAStar& getAreaFinder();
	FAStar& getPlotGroupFinder();
	NiPoint3& getPt3Origin();

	std::vector<CvInterfaceModeInfo*>& getInterfaceModeInfos();
	CvInterfaceModeInfo& getInterfaceModeInfo(InterfaceModeTypes e);

	NiPoint3& getPt3CameraDir();

	bool& getLogging();
	bool& getRandLogging();
	bool& getSynchLogging();
	bool& overwriteLogs();

	inline bool	getIsInPedia() const { return m_bIsInPedia; }
	inline void	setIsInPedia(bool bNewValue) { m_bIsInPedia = bNewValue; }

	int* getPlotDirectionX() const;
	int* getPlotDirectionY() const;
	int* getPlotCardinalDirectionX() const;
	int* getPlotCardinalDirectionY() const;
	int* getCityPlotX() const;
	int* getCityPlotY() const;
	int* getCityPlotPriority() const;
	int getXYCityPlot(const int i, const int j) const;
	DirectionTypes* getTurnLeftDirection() const;
	DirectionTypes getTurnLeftDirection(const int i) const;
	DirectionTypes* getTurnRightDirection() const;
	DirectionTypes getTurnRightDirection(int i) const;
	DirectionTypes getXYDirection(const int i, const int j) const;

/************************************************************************************************/
/* SORT_ALPHABET                           11/19/07                                MRGENIE      */
/*                                                                                              */
/* Method for alphabetically order tags                                                         */
/************************************************************************************************/
	//
	// Global Infos
	// All info type strings are upper case and are kept in this hash map for fast lookup
	//

	// returns the infos index, use this when searching for an info type string
	int getInfoTypeForString(const char* szType, bool hideAssert = false) const;

	void LogMissingType( const char* szType ) const;

	bool hasInfoTypeForString(const char* szType, bool hideAssert = false) const;
/*
	DllExport void setInfoTypeFromString(const char* szType, int idx);
*/
	void setInfoTypeFromString(const char* szType, int idx);
	void logInfoTypeMap(const char* tagMsg = "");
/************************************************************************************************/
/* SORT_ALPHABET                           END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 11/30/07                                MRGENIE      */
/*                                                                                              */
/* Savegame compatibility                                                                       */
/************************************************************************************************/
	void infoTypeFromStringReset();
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
	void addToInfosVectors(void *infoVector);
	void infosReset();
	int getOrCreateInfoTypeForString(const char* szType);

	void addDelayedResolution(int* pType, CvString szString);
	CvString* getDelayedResolution(int* pType);
	void removeDelayedResolution(int* pType);
	void copyNonDefaultDelayedResolution(int* pTypeSelf, int* pTypeOther);
	template<class T>
	void copyNonDefaultDelayedResolutionVector(std::vector<T>& aTarget, std::vector<T>& aSource)
	{
		std::stack<CvString> aszTemp;
		std::vector<T>::iterator it = aTarget.end(), it2 = aSource.begin();
		while (it > aTarget.begin())
		{
			it--;
			aszTemp.push(*getDelayedResolution(&*it));
			removeDelayedResolution(&*it);
		}
		aTarget.insert(aTarget.end(), aSource.begin(), aSource.end());
		it = aTarget.begin(); // because insert() invalidates the previous iterator
		while (aszTemp.size())
		{
			addDelayedResolution(&*it++, aszTemp.top());
			aszTemp.pop();
		}
		while (it2 != aSource.end())
		{
			addDelayedResolution(&*it++, *getDelayedResolution(&*it2++));
		}
	}
	void resolveDelayedResolution();

	int getNumWorldInfos() const;
	std::vector<CvWorldInfo*>& getWorldInfos();
	CvWorldInfo& getWorldInfo(WorldSizeTypes e) const;
	CvInfoReplacements<CvWorldInfo>* getWorldInfoReplacements();

	int getNumClimateInfos() const;
	std::vector<CvClimateInfo*>& getClimateInfos();
	CvClimateInfo& getClimateInfo(ClimateTypes e) const;

	int getNumSeaLevelInfos() const;
	std::vector<CvSeaLevelInfo*>& getSeaLevelInfos();
	CvSeaLevelInfo& getSeaLevelInfo(SeaLevelTypes e) const;

	int getNumColorInfos() const;
	std::vector<CvColorInfo*>& getColorInfos();
	CvColorInfo& getColorInfo(ColorTypes e) const;

	int getNumPlayerColorInfos() const;
	std::vector<CvPlayerColorInfo*>& getPlayerColorInfos();
	CvPlayerColorInfo& getPlayerColorInfo(PlayerColorTypes e) const;

	int getNumAdvisorInfos() const;
	std::vector<CvAdvisorInfo*>& getAdvisorInfos();
	CvAdvisorInfo& getAdvisorInfo(AdvisorTypes e) const;

	int getNumHints() const;
	std::vector<CvInfoBase*>& getHints();
	CvInfoBase& getHints(int i) const;

	int getNumMainMenus() const;
	std::vector<CvMainMenuInfo*>& getMainMenus();
	CvMainMenuInfo& getMainMenus(int i) const;
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 10/30/07                            MRGENIE          */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	// MLF loading
	void resetModLoadControlVector();
	int getModLoadControlVectorSize() const;
	void setModLoadControlVector(const char* szModule);
	CvString getModLoadControlVector(int i) const;

	int getTotalNumModules() const;
	void setTotalNumModules();
	int getNumModLoadControlInfos() const;
	std::vector<CvModLoadControlInfo*>& getModLoadControlInfos();
	CvModLoadControlInfo& getModLoadControlInfos(int i) const;
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING                 10/26/07                            MRGENIE          */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	void setModDir(const char* szModDir);
	std::string getModDir() const;
	std::string m_cszModDir;
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING                 END                                                  */
/************************************************************************************************/

	int getNumRouteModelInfos() const;
	std::vector<CvRouteModelInfo*>& getRouteModelInfos();
	CvRouteModelInfo& getRouteModelInfo(int i) const;

	int getNumRiverInfos() const;
	std::vector<CvRiverInfo*>& getRiverInfos();
	CvRiverInfo& getRiverInfo(RiverTypes e) const;

	int getNumRiverModelInfos() const;
	std::vector<CvRiverModelInfo*>& getRiverModelInfos();
	CvRiverModelInfo& getRiverModelInfo(int i) const;

	int getNumWaterPlaneInfos() const;
	std::vector<CvWaterPlaneInfo*>& getWaterPlaneInfos();
	CvWaterPlaneInfo& getWaterPlaneInfo(int i) const;

	int getNumTerrainPlaneInfos() const;
	std::vector<CvTerrainPlaneInfo*>& getTerrainPlaneInfos();
	CvTerrainPlaneInfo& getTerrainPlaneInfo(int i) const;

	int getNumCameraOverlayInfos() const;
	std::vector<CvCameraOverlayInfo*>& getCameraOverlayInfos();
	CvCameraOverlayInfo& getCameraOverlayInfo(int i) const;

	int getNumAnimationPathInfos() const;
	std::vector<CvAnimationPathInfo*>& getAnimationPathInfos();
	CvAnimationPathInfo& getAnimationPathInfo(AnimationPathTypes e) const;

	int getNumAnimationCategoryInfos() const;
	std::vector<CvAnimationCategoryInfo*>& getAnimationCategoryInfos();
	CvAnimationCategoryInfo& getAnimationCategoryInfo(AnimationCategoryTypes e) const;

	int getNumEntityEventInfos() const;
	std::vector<CvEntityEventInfo*>& getEntityEventInfos();
	CvEntityEventInfo& getEntityEventInfo(EntityEventTypes e) const;

	int getNumEffectInfos() const;
	std::vector<CvEffectInfo*>& getEffectInfos();
	CvEffectInfo& getEffectInfo(int i) const;

	int getNumAttachableInfos() const;
	std::vector<CvAttachableInfo*>& getAttachableInfos();
	CvAttachableInfo& getAttachableInfo(int i) const;

	int getNumCameraInfos() const;
	std::vector<CvCameraInfo*>& getCameraInfos();
	CvCameraInfo& getCameraInfo(CameraAnimationTypes eCameraAnimationNum) const;

	int getNumUnitFormationInfos() const;
	std::vector<CvUnitFormationInfo*>& getUnitFormationInfos();
	CvUnitFormationInfo& getUnitFormationInfo(int i) const;

	int getNumGameTextXML() const;
	std::vector<CvGameText*>& getGameTextXMLs();

	int getNumLandscapeInfos() const;
	std::vector<CvLandscapeInfo*>& getLandscapeInfos();
	CvLandscapeInfo& getLandscapeInfo(int iIndex) const;
	int getActiveLandscapeID() const;
	void setActiveLandscapeID(int iLandscapeID);

	int getNumTerrainInfos() const;
	std::vector<CvTerrainInfo*>& getTerrainInfos();
	CvTerrainInfo& getTerrainInfo(TerrainTypes eTerrainNum) const;
	CvInfoReplacements<CvTerrainInfo>* getTerrainInfoReplacements();

	int getNumBonusClassInfos() const;
	std::vector<CvBonusClassInfo*>& getBonusClassInfos();
	CvBonusClassInfo& getBonusClassInfo(BonusClassTypes eBonusNum) const;
	CvInfoReplacements<CvBonusClassInfo>* getBonusClassInfoReplacements();

	int getNumBonusInfos() const;
	std::vector<CvBonusInfo*>& getBonusInfos();
	CvBonusInfo& getBonusInfo(BonusTypes eBonusNum) const;
	CvInfoReplacements<CvBonusInfo>* getBonusInfoReplacements();

	int getNumFeatureInfos() const;
	std::vector<CvFeatureInfo*>& getFeatureInfos();
	CvFeatureInfo& getFeatureInfo(FeatureTypes eFeatureNum) const;
	CvInfoReplacements<CvFeatureInfo>* getFeatureInfoReplacements();

	int& getNumPlayableCivilizationInfos();
	int& getNumAIPlayableCivilizationInfos();
	int getNumCivilizationInfos() const;
	std::vector<CvCivilizationInfo*>& getCivilizationInfos();
	CvCivilizationInfo& getCivilizationInfo(CivilizationTypes eCivilizationNum) const;
	CvInfoReplacements<CvCivilizationInfo>* getCivilizationInfoReplacements();

	int getNumLeaderHeadInfos() const;
	std::vector<CvLeaderHeadInfo*>& getLeaderHeadInfos();
	CvLeaderHeadInfo& getLeaderHeadInfo(LeaderHeadTypes eLeaderHeadNum) const;
	CvInfoReplacements<CvLeaderHeadInfo>* getLeaderHeadInfoReplacements();

	int getNumTraitInfos() const;
	std::vector<CvTraitInfo*>& getTraitInfos();
	CvTraitInfo& getTraitInfo(TraitTypes eTraitNum) const;
	CvInfoReplacements<CvTraitInfo>* getTraitInfoReplacements();

	int getNumCursorInfos() const;
	std::vector<CvCursorInfo*>& getCursorInfos();
	CvCursorInfo& getCursorInfo(CursorTypes eCursorNum) const;

	int getNumThroneRoomCameras() const;
	std::vector<CvThroneRoomCamera*>& getThroneRoomCameras();
	CvThroneRoomCamera& getThroneRoomCamera(int iIndex) const;

	int getNumThroneRoomInfos() const;
	std::vector<CvThroneRoomInfo*>& getThroneRoomInfos();
	CvThroneRoomInfo& getThroneRoomInfo(int iIndex) const;

	int getNumThroneRoomStyleInfos() const;
	std::vector<CvThroneRoomStyleInfo*>& getThroneRoomStyleInfos();
	CvThroneRoomStyleInfo& getThroneRoomStyleInfo(int iIndex) const;

	int getNumSlideShowInfos() const;
	std::vector<CvSlideShowInfo*>& getSlideShowInfos();
	CvSlideShowInfo& getSlideShowInfo(int iIndex) const;

	int getNumSlideShowRandomInfos() const;
	std::vector<CvSlideShowRandomInfo*>& getSlideShowRandomInfos();
	CvSlideShowRandomInfo& getSlideShowRandomInfo(int iIndex) const;

	int getNumWorldPickerInfos() const;
	std::vector<CvWorldPickerInfo*>& getWorldPickerInfos();
	CvWorldPickerInfo& getWorldPickerInfo(int iIndex) const;

	int getNumSpaceShipInfos() const;
	std::vector<CvSpaceShipInfo*>& getSpaceShipInfos();
	CvSpaceShipInfo& getSpaceShipInfo(int iIndex) const;

	int getNumUnitInfos() const;
	std::vector<CvUnitInfo*>& getUnitInfos();
	CvUnitInfo& getUnitInfo(UnitTypes eUnitNum) const;
	CvInfoReplacements<CvUnitInfo>* getUnitInfoReplacements();

	int getNumSpawnInfos() const;
	std::vector<CvSpawnInfo*>& getSpawnInfos();
	CvSpawnInfo& getSpawnInfo(SpawnTypes eSpawnNum) const;
	CvInfoReplacements<CvSpawnInfo>* getSpawnInfoReplacements();

	int getNumSpecialUnitInfos() const;
	std::vector<CvSpecialUnitInfo*>& getSpecialUnitInfos();
	CvSpecialUnitInfo& getSpecialUnitInfo(SpecialUnitTypes eSpecialUnitNum) const;

	int getNumConceptInfos() const;
	std::vector<CvInfoBase*>& getConceptInfos();
	CvInfoBase& getConceptInfo(ConceptTypes e) const;

	int getNumNewConceptInfos() const;
	std::vector<CvInfoBase*>& getNewConceptInfos();
	CvInfoBase& getNewConceptInfo(NewConceptTypes e) const;

	int getNumPropertyInfos() const;
	std::vector<CvPropertyInfo*>& getPropertyInfos();
	CvPropertyInfo& getPropertyInfo(PropertyTypes ePropertyNum) const;

	int getNumOutcomeInfos() const;
	std::vector<CvOutcomeInfo*>& getOutcomeInfos();
	CvOutcomeInfo& getOutcomeInfo(OutcomeTypes eOutcomeNum) const;

/************************************************************************************************/
/*Afforess                                     12/21/09                                         */
/************************************************************************************************/
	int getPEAK_EXTRA_DEFENSE() const;
	int getPEAK_EXTRA_MOVEMENT() const;
	
	int iStuckUnitID;
	int iStuckUnitCount;

	bool isLoadedPlayerOptions() const;
	void setLoadedPlayerOptions(bool bNewVal);
	
	bool isXMLLogging() const;
	void setXMLLogging(bool bNewVal);

	void updateReplacements();
	
	int getSCORE_FREE_PERCENT() const;
	int getSCORE_POPULATION_FACTOR() const;
	int getSCORE_LAND_FACTOR() const;
	int getSCORE_TECH_FACTOR() const;
	int getSCORE_WONDER_FACTOR() const;
	
	int getUSE_CAN_CREATE_PROJECT_CALLBACK() const;
	int getUSE_CANNOT_CREATE_PROJECT_CALLBACK() const;
	int getUSE_CAN_DO_MELTDOWN_CALLBACK() const;
	int getUSE_CAN_MAINTAIN_PROCESS_CALLBACK() const;
	int getUSE_CANNOT_MAINTAIN_PROCESS_CALLBACK() const;
	int getUSE_CAN_DO_GROWTH_CALLBACK() const;
	int getUSE_CAN_DO_CULTURE_CALLBACK() const;
	int getUSE_CAN_DO_PLOT_CULTURE_CALLBACK() const;
	int getUSE_CAN_DO_PRODUCTION_CALLBACK() const;
	int getUSE_CAN_DO_RELIGION_CALLBACK() const;
	int getUSE_CAN_DO_GREATPEOPLE_CALLBACK() const;
	int getUSE_CAN_RAZE_CITY_CALLBACK() const;
	int getUSE_CAN_DO_GOLD_CALLBACK() const;
	int getUSE_CAN_DO_RESEARCH_CALLBACK() const;
	int getUSE_UPGRADE_UNIT_PRICE_CALLBACK() const;
	int getUSE_IS_VICTORY_CALLBACK() const;
	int getUSE_AI_UPDATE_UNIT_CALLBACK() const;
	int getUSE_AI_CHOOSE_PRODUCTION_CALLBACK() const;
	int getUSE_EXTRA_PLAYER_COSTS_CALLBACK() const;
	int getUSE_AI_DO_DIPLO_CALLBACK() const;
	int getUSE_AI_BESTTECH_CALLBACK() const;
	int getUSE_CAN_DO_COMBAT_CALLBACK() const;
	int getUSE_AI_CAN_DO_WARPLANS_CALLBACK() const;
	int getLAND_UNITS_CAN_ATTACK_WATER_CITIES() const;
	int getBASE_UNIT_UPGRADE_COST() const;
	int getUPGRADE_ROUND_LIMIT() const;
	int getCITY_BARBARIAN_DEFENSE_MODIFIER() const;
	int getUNIT_VISIBILITY_RANGE() const;
	int getMAX_UNIT_VISIBILITY_RANGE() const;
	int getGREATER_COMMERCE_SWITCH_POINT() const;
	int getWORKER_TRADE_VALUE_PERCENT_ADJUSTMENT() const;
	int getTRADE_MISSION_END_TOTAL_PERCENT_ADJUSTMENT() const;
	int getINFILTRATE_MISSION_END_TOTAL_PERCENT_ADJUSTMENT() const;
	int getESPIONAGE_MISSION_COST_END_TOTAL_PERCENT_ADJUSTMENT() const;
	int getWATER_POTENTIAL_CITY_WORK_FOR_AREA() const;
	int getSAD_MAX_MODIFIER() const;
	int getUPSCALED_RESEARCH_COST_MODIFIER() const;
	int getENABLE_DYNAMIC_UNIT_ENTITIES() const;
	float getSAD_FACTOR_1() const;
	float getSAD_FACTOR_2() const;
	float getSAD_FACTOR_3() const;
	float getSAD_FACTOR_4() const;
/************************************************************************************************/
/* Afforess                                END                                                  */
/************************************************************************************************/

	int getNumCityTabInfos() const;
	std::vector<CvInfoBase*>& getCityTabInfos();
	CvInfoBase& getCityTabInfo(CityTabTypes e) const;

	int getNumCalendarInfos() const;
	std::vector<CvInfoBase*>& getCalendarInfos();
	CvInfoBase& getCalendarInfo(CalendarTypes e) const;

	int getNumSeasonInfos() const;
	std::vector<CvInfoBase*>& getSeasonInfos();
	CvInfoBase& getSeasonInfo(SeasonTypes e) const;

	int getNumMonthInfos() const;
	std::vector<CvInfoBase*>& getMonthInfos();
	CvInfoBase& getMonthInfo(MonthTypes e) const;

	int getNumDenialInfos() const;
	std::vector<CvInfoBase*>& getDenialInfos();
	CvInfoBase& getDenialInfo(DenialTypes e) const;

	int getNumInvisibleInfos() const;
	std::vector<CvInvisibleInfo*>& getInvisibleInfos();
	CvInvisibleInfo& getInvisibleInfo(InvisibleTypes e) const;

	int getNumVoteSourceInfos() const;
	std::vector<CvVoteSourceInfo*>& getVoteSourceInfos();
	CvVoteSourceInfo& getVoteSourceInfo(VoteSourceTypes e) const;

	int getNumUnitCombatInfos() const;
	std::vector<CvUnitCombatInfo*>& getUnitCombatInfos();
	CvUnitCombatInfo& getUnitCombatInfo(UnitCombatTypes e) const;

	std::vector<CvInfoBase*>& getDomainInfos();
	CvInfoBase& getDomainInfo(DomainTypes e) const;

	//TB Promotion Line Mod begin
	int getNumPromotionLineInfos() const;
	std::vector<CvPromotionLineInfo*>& getPromotionLineInfos();
	CvPromotionLineInfo& getPromotionLineInfo(PromotionLineTypes e) const;
	//TB Promotion Line Mod end

	int getNumMapCategoryInfos() const;
	std::vector<CvMapCategoryInfo*>& getMapCategoryInfos();
	CvMapCategoryInfo& getMapCategoryInfo(MapCategoryTypes e) const;

	int getNumIdeaClassInfos() const;
	std::vector<CvIdeaClassInfo*>& getIdeaClassInfos();
	CvIdeaClassInfo& getIdeaClassInfo(IdeaClassTypes e) const;

	int getNumIdeaInfos() const;
	std::vector<CvIdeaInfo*>& getIdeaInfos();
	CvIdeaInfo& getIdeaInfo(IdeaTypes e) const;

	std::vector<CvInfoBase*>& getUnitAIInfos();
	CvInfoBase& getUnitAIInfo(UnitAITypes eUnitAINum) const;

	//	Koshling - added internal registration of supported UnitAI types, not reliant
	//	on external definition in XML
private:
	void registerUnitAI(const char* szType, int enumVal);
	void registerMission(const char* szType, int enumVal);
public:
	void registerUnitAIs();
	void registerAIScales();
	void registerGameObjects();
	void registerGOMs();
	void registerRelations();
	void registerAttributes();
	void registerTags();
	void registerPropertySources();
	void registerPropertyInteractions();
	void registerPropertyPropagators();
	void registerMissions();

	std::vector<CvInfoBase*>& getAttitudeInfos();
	CvInfoBase& getAttitudeInfo(AttitudeTypes eAttitudeNum) const;

	std::vector<CvInfoBase*>& getMemoryInfos();
	CvInfoBase& getMemoryInfo(MemoryTypes eMemoryNum) const;

	int getNumGameOptionInfos() const;
	std::vector<CvGameOptionInfo*>& getGameOptionInfos();
	CvGameOptionInfo& getGameOptionInfo(GameOptionTypes eGameOptionNum) const;

	int getNumMPOptionInfos() const;
	std::vector<CvMPOptionInfo*>& getMPOptionInfos();
	CvMPOptionInfo& getMPOptionInfo(MultiplayerOptionTypes eMPOptionNum) const;

	int getNumForceControlInfos() const;
	std::vector<CvForceControlInfo*>& getForceControlInfos();
	CvForceControlInfo& getForceControlInfo(ForceControlTypes eForceControlNum) const;

	std::vector<CvPlayerOptionInfo*>& getPlayerOptionInfos();
	CvPlayerOptionInfo& getPlayerOptionInfo(PlayerOptionTypes ePlayerOptionNum) const;

	std::vector<CvGraphicOptionInfo*>& getGraphicOptionInfos();
	CvGraphicOptionInfo& getGraphicOptionInfo(GraphicOptionTypes eGraphicOptionNum) const;

	std::vector<CvYieldInfo*>& getYieldInfos();
	CvYieldInfo& getYieldInfo(YieldTypes eYieldNum) const;

	std::vector<CvCommerceInfo*>& getCommerceInfos();
	CvCommerceInfo& getCommerceInfo(CommerceTypes eCommerceNum) const;

	int getNumRouteInfos() const;
	std::vector<CvRouteInfo*>& getRouteInfos();
	CvRouteInfo& getRouteInfo(RouteTypes eRouteNum) const;
	CvInfoReplacements<CvRouteInfo>* getRouteInfoReplacements();

	int getNumImprovementInfos() const;
	std::vector<CvImprovementInfo*>& getImprovementInfos();
	CvImprovementInfo& getImprovementInfo(ImprovementTypes eImprovementNum) const;
	CvInfoReplacements<CvImprovementInfo>* getImprovementInfoReplacements();

	int getNumGoodyInfos() const;
	std::vector<CvGoodyInfo*>& getGoodyInfos();
	CvGoodyInfo& getGoodyInfo(GoodyTypes eGoodyNum) const;

	int getNumBuildInfos() const;
	std::vector<CvBuildInfo*>& getBuildInfos();
	CvBuildInfo& getBuildInfo(BuildTypes eBuildNum) const;
	CvInfoReplacements<CvBuildInfo>* getBuildInfoReplacements();

	int getNumHandicapInfos() const;
	std::vector<CvHandicapInfo*>& getHandicapInfos();
	CvHandicapInfo& getHandicapInfo(HandicapTypes eHandicapNum) const;
	CvInfoReplacements<CvHandicapInfo>* getHandicapInfoReplacements();

	int getNumGameSpeedInfos() const;
	std::vector<CvGameSpeedInfo*>& getGameSpeedInfos();
	CvGameSpeedInfo& getGameSpeedInfo(GameSpeedTypes eGameSpeedNum) const;
	CvInfoReplacements<CvGameSpeedInfo>* getGameSpeedInfoReplacements();

	int getNumTurnTimerInfos() const;
	std::vector<CvTurnTimerInfo*>& getTurnTimerInfos();
	CvTurnTimerInfo& getTurnTimerInfo(TurnTimerTypes eTurnTimerNum) const;

	int getNumProcessInfos() const;
	std::vector<CvProcessInfo*>& getProcessInfos();
	CvProcessInfo& getProcessInfo(ProcessTypes e) const;
	CvInfoReplacements<CvProcessInfo>* getProcessInfoReplacements();

	int getNumVoteInfos() const;
	std::vector<CvVoteInfo*>& getVoteInfos();
	CvVoteInfo& getVoteInfo(VoteTypes e) const;

	int getNumProjectInfos() const;
	std::vector<CvProjectInfo*>& getProjectInfos();
	CvProjectInfo& getProjectInfo(ProjectTypes e) const;
	CvInfoReplacements<CvProjectInfo>* getProjectInfoReplacements();

	int getNumBuildingInfos() const;
	std::vector<CvBuildingInfo*>& getBuildingInfos();
	CvBuildingInfo& getBuildingInfo(BuildingTypes eBuildingNum) const;
	CvInfoReplacements<CvBuildingInfo>* getBuildingInfoReplacements();

	int getNumSpecialBuildingInfos() const;
	std::vector<CvSpecialBuildingInfo*>& getSpecialBuildingInfos();
	CvSpecialBuildingInfo& getSpecialBuildingInfo(SpecialBuildingTypes eSpecialBuildingNum) const;
	CvInfoReplacements<CvSpecialBuildingInfo>* getSpecialBuildingInfoReplacements();

	int getNumActionInfos() const;
	std::vector<CvActionInfo*>& getActionInfos();
	CvActionInfo& getActionInfo(int i) const;

	std::vector<CvMissionInfo*>& getMissionInfos();
	CvMissionInfo& getMissionInfo(MissionTypes eMissionNum) const;

	std::vector<CvControlInfo*>& getControlInfos();
	CvControlInfo& getControlInfo(ControlTypes eControlNum) const;

	std::vector<CvCommandInfo*>& getCommandInfos();
	CvCommandInfo& getCommandInfo(CommandTypes eCommandNum) const;

	int getNumAutomateInfos() const;
	std::vector<CvAutomateInfo*>& getAutomateInfos();
	CvAutomateInfo& getAutomateInfo(int iAutomateNum) const;

	int getNumPromotionInfos() const;
	std::vector<CvPromotionInfo*>& getPromotionInfos();
	CvPromotionInfo& getPromotionInfo(PromotionTypes ePromotionNum) const;
	CvInfoReplacements<CvPromotionInfo>* getPromotionInfoReplacements();
	typedef bst::function<bool(const CvPromotionInfo*, PromotionTypes)> PromotionPredicateFn;
	PromotionTypes findPromotion(PromotionPredicateFn predicateFn) const;

	int getNumTechInfos() const;
	std::vector<CvTechInfo*>& getTechInfos();
	CvTechInfo& getTechInfo(TechTypes eTechNum) const;
	CvInfoReplacements<CvTechInfo>* getTechInfoReplacements();

	int getNumReligionInfos() const;
	std::vector<CvReligionInfo*>& getReligionInfos();
	CvReligionInfo& getReligionInfo(ReligionTypes eReligionNum) const;
	CvInfoReplacements<CvReligionInfo>* getReligionInfoReplacements();

	int getNumCorporationInfos() const;
	std::vector<CvCorporationInfo*>& getCorporationInfos();
	CvCorporationInfo& getCorporationInfo(CorporationTypes eCorporationNum) const;
	CvInfoReplacements<CvCorporationInfo>* getCorporationInfoReplacements();

	int getNumSpecialistInfos() const;
	std::vector<CvSpecialistInfo*>& getSpecialistInfos();
	CvSpecialistInfo& getSpecialistInfo(SpecialistTypes eSpecialistNum) const;
	CvInfoReplacements<CvSpecialistInfo>* getSpecialistInfoReplacements();

	int getNumCivicOptionInfos() const;
	std::vector<CvCivicOptionInfo*>& getCivicOptionInfos();
	CvCivicOptionInfo& getCivicOptionInfo(CivicOptionTypes eCivicOptionNum) const;

	int getNumCivicInfos() const;
	std::vector<CvCivicInfo*>& getCivicInfos();
	CvCivicInfo& getCivicInfo(CivicTypes eCivicNum) const;
	CvInfoReplacements<CvCivicInfo>* getCivicInfoReplacements();

	int getNumDiplomacyInfos() const;
	std::vector<CvDiplomacyInfo*>& getDiplomacyInfos();
	CvDiplomacyInfo& getDiplomacyInfo(int iDiplomacyNum) const;

	int getNumEraInfos() const;
	std::vector<CvEraInfo*>& getEraInfos();
	CvEraInfo& getEraInfo(EraTypes eEraNum) const;
	CvInfoReplacements<CvEraInfo>* getEraInfoReplacements();

	int getNumHurryInfos() const;
	std::vector<CvHurryInfo*>& getHurryInfos();
	CvHurryInfo& getHurryInfo(HurryTypes eHurryNum) const;

	int getNumEmphasizeInfos() const;
	std::vector<CvEmphasizeInfo*>& getEmphasizeInfos();
	CvEmphasizeInfo& getEmphasizeInfo(EmphasizeTypes eEmphasizeNum) const;

	int getNumUpkeepInfos() const;
	std::vector<CvUpkeepInfo*>& getUpkeepInfos();
	CvUpkeepInfo& getUpkeepInfo(UpkeepTypes eUpkeepNum) const;

	int getNumCultureLevelInfos() const;
	std::vector<CvCultureLevelInfo*>& getCultureLevelInfos();
	CvCultureLevelInfo& getCultureLevelInfo(CultureLevelTypes eCultureLevelNum) const;
	CvInfoReplacements<CvCultureLevelInfo>* getCultureLevelInfoReplacements();

	int getNumVictoryInfos() const;
	std::vector<CvVictoryInfo*>& getVictoryInfos();
	CvVictoryInfo& getVictoryInfo(VictoryTypes eVictoryNum) const;

	int getNumQuestInfos() const;
	std::vector<CvQuestInfo*>& getQuestInfos();
	CvQuestInfo& getQuestInfo(int iIndex) const;

	int getNumTutorialInfos() const;
	std::vector<CvTutorialInfo*>& getTutorialInfos();
	CvTutorialInfo& getTutorialInfo(int i) const;

	int getNumEventTriggerInfos() const;
	std::vector<CvEventTriggerInfo*>& getEventTriggerInfos();
	CvEventTriggerInfo& getEventTriggerInfo(EventTriggerTypes eEventTrigger) const;
	CvInfoReplacements<CvEventTriggerInfo>* getEventTriggerInfoReplacements();

	int getNumEventInfos() const;
	std::vector<CvEventInfo*>& getEventInfos();
	CvEventInfo& getEventInfo(EventTypes eEvent) const;
	CvInfoReplacements<CvEventInfo>* getEventInfoReplacements();

	int getNumEspionageMissionInfos() const;
	std::vector<CvEspionageMissionInfo*>& getEspionageMissionInfos();
	CvEspionageMissionInfo& getEspionageMissionInfo(EspionageMissionTypes eEspionageMissionNum) const;

	int getNumUnitArtStyleTypeInfos() const;
	std::vector<CvUnitArtStyleTypeInfo*>& getUnitArtStyleTypeInfos();
	CvUnitArtStyleTypeInfo& getUnitArtStyleTypeInfo(UnitArtStyleTypes eUnitArtStyleTypeNum) const;

	//
	// Global Types
	// All type strings are upper case and are kept in this hash map for fast lookup
	// The other functions are kept for convenience when enumerating, but most are not used
	//
	int getTypesEnum(const char* szType) const;				// use this when searching for a type
	void setTypesEnum(const char* szType, int iEnum);

	int getNUM_ENGINE_DIRTY_BITS() const;
	int getNUM_INTERFACE_DIRTY_BITS() const;
	int getNUM_YIELD_TYPES() const;
	int getNUM_COMMERCE_TYPES() const;
	int getNUM_FORCECONTROL_TYPES() const;
	int getNUM_INFOBAR_TYPES() const;
	int getNUM_HEALTHBAR_TYPES() const;
	int getNUM_CONTROL_TYPES() const;
	int getNUM_LEADERANIM_TYPES() const;

	int& getNumEntityEventTypes();
	CvString*& getEntityEventTypes();
	CvString& getEntityEventTypes(EntityEventTypes e);

	int& getNumAnimationOperatorTypes();
	CvString*& getAnimationOperatorTypes();
	CvString& getAnimationOperatorTypes(AnimationOperatorTypes e);

	CvString*& getFunctionTypes();
	CvString& getFunctionTypes(FunctionTypes e);

	int& getNumFlavorTypes();
	CvString*& getFlavorTypes();
	CvString& getFlavorTypes(FlavorTypes e);

	int& getNumArtStyleTypes();
	CvString*& getArtStyleTypes();
	CvString& getArtStyleTypes(ArtStyleTypes e);

	int& getNumCitySizeTypes();
	CvString*& getCitySizeTypes();
	CvString& getCitySizeTypes(int i);

	CvString*& getContactTypes();
	CvString& getContactTypes(ContactTypes e);

	CvString*& getDiplomacyPowerTypes();
	CvString& getDiplomacyPowerTypes(DiplomacyPowerTypes e);

	CvString*& getAutomateTypes();
	CvString& getAutomateTypes(AutomateTypes e);

	CvString*& getDirectionTypes();
	CvString& getDirectionTypes(AutomateTypes e);

	int& getNumFootstepAudioTypes();
	CvString*& getFootstepAudioTypes();
	CvString& getFootstepAudioTypes(int i);
	int getFootstepAudioTypeByTag(CvString strTag);

	CvString*& getFootstepAudioTags();
	CvString& getFootstepAudioTags(int i);

	CvString& getCurrentXMLFile();
	void setCurrentXMLFile(const TCHAR* szFileName);

	//
	///////////////// BEGIN global defines
	// THESE ARE READ-ONLY
	//

	FVariableSystem* getDefinesVarSystem() const;
	void cacheGlobals();

	// ***** EXPOSED TO PYTHON *****
/************************************************************************************************/
/* MOD_COMPONENT_CONTROL                   08/02/07                            MRGENIE          */
/*                                                                                              */
/* Return true/false from                                                                       */
/************************************************************************************************/
	bool getDefineBOOL( const char * szName ) const;
/************************************************************************************************/
/* MOD_COMPONENT_CONTROL                   END                                                  */
/************************************************************************************************/

/************************************************************************************************/
/* Mod Globals    Start                          09/13/10                           phungus420  */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	bool isDCM_BATTLE_EFFECTS() const;
	int getBATTLE_EFFECT_LESS_FOOD() const;
	int getBATTLE_EFFECT_LESS_PRODUCTION() const;
	int getBATTLE_EFFECT_LESS_COMMERCE() const;
	int getBATTLE_EFFECTS_MINIMUM_TURN_INCREMENTS() const;
	int getMAX_BATTLE_TURNS() const;

	bool isDCM_AIR_BOMBING() const;
	bool isDCM_RANGE_BOMBARD() const;
	int getDCM_RB_CITY_INACCURACY() const;
	int getDCM_RB_CITYBOMBARD_CHANCE() const;
	bool isDCM_ATTACK_SUPPORT() const;
	bool isDCM_OPP_FIRE() const;
	bool isDCM_ACTIVE_DEFENSE() const;
	bool isDCM_FIGHTER_ENGAGE() const;

	bool isDYNAMIC_CIV_NAMES() const;

	bool isLIMITED_RELIGIONS_EXCEPTIONS() const;
	bool isOC_RESPAWN_HOLY_CITIES() const;

	bool isIDW_ENABLED() const;
	float getIDW_BASE_COMBAT_INFLUENCE() const;
	float getIDW_NO_CITY_DEFENDER_MULTIPLIER() const;
	float getIDW_FORT_CAPTURE_MULTIPLIER() const;
	float getIDW_EXPERIENCE_FACTOR() const;
	float getIDW_WARLORD_MULTIPLIER() const;
	int getIDW_INFLUENCE_RADIUS() const;
	float getIDW_PLOT_DISTANCE_FACTOR() const;
	float getIDW_WINNER_PLOT_MULTIPLIER() const;
	float getIDW_LOSER_PLOT_MULTIPLIER() const;
	bool isIDW_EMERGENCY_DRAFT_ENABLED() const;
	int getIDW_EMERGENCY_DRAFT_MIN_POPULATION() const;
	float getIDW_EMERGENCY_DRAFT_STRENGTH() const;
	float getIDW_EMERGENCY_DRAFT_ANGER_MULTIPLIER() const;
	bool isIDW_NO_BARBARIAN_INFLUENCE() const;
	bool isIDW_NO_NAVAL_INFLUENCE() const;
	bool isIDW_PILLAGE_INFLUENCE_ENABLED() const;
	float getIDW_BASE_PILLAGE_INFLUENCE() const;
	float getIDW_CITY_TILE_MULTIPLIER() const;

	bool isSS_ENABLED() const;
	bool isSS_BRIBE() const;
	bool isSS_ASSASSINATE() const;
/************************************************************************************************/
/* Mod Globals                        END                                           phungus420  */
/************************************************************************************************/
	void setGraphicalDetailPagingEnabled(bool bEnabled);
	bool getGraphicalDetailPagingEnabled() const;
	int getGraphicalDetailPageInRange();

	int getDefineINT( const char * szName ) const;
	float getDefineFLOAT( const char * szName ) const;
	const char * getDefineSTRING( const char * szName ) const;
/************************************************************************************************/
/* Afforess	                  Start		 08/18/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	void setDefineINT( const char * szName, int iValue, bool bUpdate = true);
	void setDefineFLOAT( const char * szName, float fValue, bool bUpdate = true );
	void setDefineSTRING( const char * szName, const char * szValue, bool bUpdate = true );
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/


	int getMOVE_DENOMINATOR() const;
	int getNUM_UNIT_PREREQ_OR_BONUSES() const;
	int getNUM_BUILDING_PREREQ_OR_BONUSES() const;
	int getFOOD_CONSUMPTION_PER_POPULATION() const;
	int getMAX_HIT_POINTS() const;
	int getPATH_DAMAGE_WEIGHT() const;
	int getHILLS_EXTRA_DEFENSE() const;
	int getRIVER_ATTACK_MODIFIER() const;
	int getAMPHIB_ATTACK_MODIFIER() const;
	int getHILLS_EXTRA_MOVEMENT() const;
	int getRIVER_EXTRA_MOVEMENT() const;
	int getMAX_PLOT_LIST_ROWS() const;
	int getUNIT_MULTISELECT_MAX() const;
	int getPERCENT_ANGER_DIVISOR() const;
	int getEVENT_MESSAGE_TIME() const;
	int getROUTE_FEATURE_GROWTH_MODIFIER() const;
	int getFEATURE_GROWTH_MODIFIER() const;
	int getMIN_CITY_RANGE() const;
	int getCITY_MAX_NUM_BUILDINGS() const;
	int getNUM_UNIT_AND_TECH_PREREQS() const;
	int getNUM_AND_TECH_PREREQS() const;
	int getNUM_OR_TECH_PREREQS() const;
	int getLAKE_MAX_AREA_SIZE() const;
	int getNUM_ROUTE_PREREQ_OR_BONUSES() const;
	int getNUM_BUILDING_AND_TECH_PREREQS() const;
	int getMIN_WATER_SIZE_FOR_OCEAN() const;
	int getFORTIFY_MODIFIER_PER_TURN() const;
	int getESTABLISH_MODIFIER_PER_TURN() const;
	int getESCAPE_MODIFIER_PER_TURN() const;
	int getMAX_CITY_DEFENSE_DAMAGE() const;
	int getNUM_CORPORATION_PREREQ_BONUSES() const;
	int getPEAK_SEE_THROUGH_CHANGE() const;
	int getHILLS_SEE_THROUGH_CHANGE() const;
	int getSEAWATER_SEE_FROM_CHANGE() const;
	int getPEAK_SEE_FROM_CHANGE() const;
	int getHILLS_SEE_FROM_CHANGE() const;
	int getUSE_SPIES_NO_ENTER_BORDERS() const;

	float getCAMERA_MIN_YAW() const;
	float getCAMERA_MAX_YAW() const;
	float getCAMERA_FAR_CLIP_Z_HEIGHT() const;
	float getCAMERA_MAX_TRAVEL_DISTANCE() const;
	float getCAMERA_START_DISTANCE() const;
	float getAIR_BOMB_HEIGHT() const;
	float getPLOT_SIZE() const;
	float getCAMERA_SPECIAL_PITCH() const;
	float getCAMERA_MAX_TURN_OFFSET() const;
	float getCAMERA_MIN_DISTANCE() const;
	float getCAMERA_UPPER_PITCH() const;
	float getCAMERA_LOWER_PITCH() const;
	float getFIELD_OF_VIEW() const;
	float getSHADOW_SCALE() const;
	float getUNIT_MULTISELECT_DISTANCE() const;

	int getUSE_CANNOT_FOUND_CITY_CALLBACK() const;
	int getUSE_CAN_FOUND_CITIES_ON_WATER_CALLBACK() const;
	int getUSE_IS_PLAYER_RESEARCH_CALLBACK() const;
	int getUSE_CAN_RESEARCH_CALLBACK() const;
	int getUSE_CANNOT_DO_CIVIC_CALLBACK() const;
	int getUSE_CAN_DO_CIVIC_CALLBACK() const;
	int getUSE_CANNOT_CONSTRUCT_CALLBACK() const;
	int getUSE_CAN_CONSTRUCT_CALLBACK() const;
	int getUSE_CAN_DECLARE_WAR_CALLBACK() const;
	int getUSE_CANNOT_RESEARCH_CALLBACK() const;
	int getUSE_GET_UNIT_COST_MOD_CALLBACK() const;
	int getUSE_GET_BUILDING_COST_MOD_CALLBACK() const;
	int getUSE_GET_CITY_FOUND_VALUE_CALLBACK() const;
	int getUSE_CANNOT_HANDLE_ACTION_CALLBACK() const;
	int getUSE_CAN_TRAIN_CALLBACK() const;
	int getUSE_CANNOT_TRAIN_CALLBACK() const;
	int getUSE_CAN_BUILD_CALLBACK() const;
	int getUSE_CAN_TRAIN_CALLBACK(UnitTypes eUnit) const;
	int getUSE_CANNOT_TRAIN_CALLBACK(UnitTypes eUnit) const;
	int getUSE_CAN_BUILD_CALLBACK(BuildTypes eBuild) const;
	int getUSE_UNIT_CANNOT_MOVE_INTO_CALLBACK() const;
	int getUSE_USE_CANNOT_SPREAD_RELIGION_CALLBACK() const;
	int getUSE_FINISH_TEXT_CALLBACK() const;
	int getUSE_ON_UNIT_SET_XY_CALLBACK() const;
	int getUSE_ON_UNIT_SELECTED_CALLBACK() const;
	int getUSE_ON_UPDATE_CALLBACK() const;
	int getUSE_ON_UNIT_CREATED_CALLBACK() const;
	int getUSE_ON_UNIT_LOST_CALLBACK() const;
/************************************************************************************************/
/* MODULES                                 11/13/07                            MRGENIE          */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int getTGA_RELIGIONS() const;								// GAMEFONT
	int getTGA_CORPORATIONS() const;
/************************************************************************************************/
/* MODULES                                 END                                                  */
/************************************************************************************************/
	int getMAX_CIV_PLAYERS() const;
	int getMAX_PC_PLAYERS() const;
	int getMAX_PLAYERS() const;
	int getMAX_PC_TEAMS() const;
	int getMAX_TEAMS() const;
	int getBARBARIAN_PLAYER() const;
	int getBARBARIAN_TEAM() const;
	int getNEANDERTHAL_PLAYER() const;
	int getNEANDERTHAL_TEAM() const;
	int getBEAST_PLAYER() const;
	int getBEAST_TEAM() const;
	int getPREDATOR_PLAYER() const;
	int getPREDATOR_TEAM() const;
	int getPREY_PLAYER() const;
	int getPREY_TEAM() const;
	int getINSECT_PLAYER() const;
	int getINSECT_TEAM() const;
	int getNPC4_PLAYER() const;
	int getNPC4_TEAM() const;
	int getNPC3_PLAYER() const;
	int getNPC3_TEAM() const;
	int getNPC2_PLAYER() const;
	int getNPC2_TEAM() const;
	int getNPC1_PLAYER() const;
	int getNPC1_TEAM() const;
	int getNPC0_PLAYER() const;
	int getNPC0_TEAM() const;
	int getINVALID_PLOT_COORD() const;
	int getNUM_CITY_PLOTS() const;
	int getCITY_HOME_PLOT() const;

	// ***** END EXPOSED TO PYTHON *****

	////////////// END DEFINES //////////////////

#ifdef _USRDLL
	CvDLLUtilityIFaceBase* getDLLIFace() { return g_DLL; }		// inlined for perf reasons, do not use outside of dll
#endif
	CvDLLUtilityIFaceBase* getDLLIFaceNonInl();
	void setDLLProfiler(FProfiler* prof);
	FProfiler* getDLLProfiler() const;
	void enableDLLProfiler(bool bEnable);
	bool isDLLProfilerEnabled() const;
	const char* alternateProfileSampleName() const;

	bool IsGraphicsInitialized() const;
	void SetGraphicsInitialized(bool bVal);

	// for caching
	bool readBuildingInfoArray(FDataStreamBase* pStream);
	void writeBuildingInfoArray(FDataStreamBase* pStream);

	bool readTechInfoArray(FDataStreamBase* pStream);
	void writeTechInfoArray(FDataStreamBase* pStream);

	bool readUnitInfoArray(FDataStreamBase* pStream);
	void writeUnitInfoArray(FDataStreamBase* pStream);

	bool readLeaderHeadInfoArray(FDataStreamBase* pStream);
	void writeLeaderHeadInfoArray(FDataStreamBase* pStream);

	bool readCivilizationInfoArray(FDataStreamBase* pStream);
	void writeCivilizationInfoArray(FDataStreamBase* pStream);

	bool readPromotionInfoArray(FDataStreamBase* pStream);
	void writePromotionInfoArray(FDataStreamBase* pStream);

	bool readDiplomacyInfoArray(FDataStreamBase* pStream);
	void writeDiplomacyInfoArray(FDataStreamBase* pStream);

	bool readCivicInfoArray(FDataStreamBase* pStream);
	void writeCivicInfoArray(FDataStreamBase* pStream);

	bool readHandicapInfoArray(FDataStreamBase* pStream);
	void writeHandicapInfoArray(FDataStreamBase* pStream);

	bool readBonusInfoArray(FDataStreamBase* pStream);
	void writeBonusInfoArray(FDataStreamBase* pStream);

	bool readImprovementInfoArray(FDataStreamBase* pStream);
	void writeImprovementInfoArray(FDataStreamBase* pStream);

	bool readEventInfoArray(FDataStreamBase* pStream);
	void writeEventInfoArray(FDataStreamBase* pStream);

	bool readEventTriggerInfoArray(FDataStreamBase* pStream);
	void writeEventTriggerInfoArray(FDataStreamBase* pStream);

	//
	// additional accessors for initting globals
	//

	void setInterface(CvInterface* pVal);
	 void setDiplomacyScreen(CvDiplomacyScreen* pVal);
	 void setMPDiplomacyScreen(CMPDiplomacyScreen* pVal);
	 void setMessageQueue(CMessageQueue* pVal);
	 void setHotJoinMessageQueue(CMessageQueue* pVal);
	 void setMessageControl(CMessageControl* pVal);
	 void setSetupData(CvSetupData* pVal);
	 void setMessageCodeTranslator(CvMessageCodeTranslator* pVal);
	 void setDropMgr(CvDropMgr* pVal);
	 void setPortal(CvPortal* pVal);
	 void setStatsReport(CvStatsReporter* pVal);
	 void setPathFinder(FAStar* pVal);
	 void setInterfacePathFinder(FAStar* pVal);
	 void setStepFinder(FAStar* pVal);
	 void setRouteFinder(FAStar* pVal);
	 void setBorderFinder(FAStar* pVal);
	 void setAreaFinder(FAStar* pVal);
	 void setPlotGroupFinder(FAStar* pVal);

	// So that CvEnums are moddable in the DLL
	 int getNumDirections() const;
	 int getNumGameOptions() const;
	 int getNumMPOptions() const;
	 int getNumSpecialOptions() const;
	 int getNumGraphicOptions() const;
	 int getNumTradeableItems() const;
	 int getNumBasicItems() const;
	 int getNumTradeableHeadings() const;
	 int getNumCommandInfos() const;
	 int getNumControlInfos() const;
	 int getNumMissionInfos() const;
	 int getNumPlayerOptionInfos() const;
	 int getMaxNumSymbols() const;
	 int getNumGraphicLevels() const;
	 int getNumGlobeLayers() const;

// BUG - BUG Info - start
	void setIsBug(bool bIsBug);
// BUG - BUG Info - end

	unsigned int getAssetCheckSum();

	void deleteInfoArrays();
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 11/30/07                                MRGENIE      */
/*                                                                                              */
/* Savegame compatibility                                                                       */
/************************************************************************************************/
	void doResetInfoClasses(int iNumSaveGameVector, std::vector<CvString> m_aszSaveGameVector);
	void StoreExeSettings();
	void LoadExeSettings();
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/

protected:

	bool m_bGraphicsInitialized;
	bool m_bDLLProfiler;
	bool m_bLogging;
	bool m_bRandLogging;
	bool m_bSynchLogging;
	bool m_bOverwriteLogs;
	NiPoint3  m_pt3CameraDir;
	int m_iNewPlayers;

	CMainMenu* m_pkMainMenu;

	bool m_bZoomOut;
	bool m_bZoomIn;
	bool m_bLoadGameFromFile;

	FMPIManager * m_pFMPMgr;

	CvRandom* m_asyncRand;

	CvGameAI* m_game;

	CMessageQueue* m_messageQueue;
	CMessageQueue* m_hotJoinMsgQueue;
	CMessageControl* m_messageControl;
	CvSetupData* m_setupData;
	CvInitCore* m_iniInitCore;
	CvInitCore* m_loadedInitCore;
	CvInitCore* m_initCore;
	CvMessageCodeTranslator * m_messageCodes;
	CvDropMgr* m_dropMgr;
	CvPortal* m_portal;
	CvStatsReporter * m_statsReporter;
	CvInterface* m_interface;

//	CvArtFileMgr* m_pArtFileMgr; (unused)

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	std::vector<CvMap*> m_maps;
	bool	m_bResourceLayerOn;
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

	CvDiplomacyScreen* m_diplomacyScreen;
	CMPDiplomacyScreen* m_mpDiplomacyScreen;

	FAStar* m_pathFinder;
	FAStar* m_interfacePathFinder;
	FAStar* m_stepFinder;
	FAStar* m_routeFinder;
	FAStar* m_borderFinder;
	FAStar* m_areaFinder;
	FAStar* m_plotGroupFinder;

	NiPoint3 m_pt3Origin;

	int* m_aiPlotDirectionX;	// [NUM_DIRECTION_TYPES];
	int* m_aiPlotDirectionY;	// [NUM_DIRECTION_TYPES];
	int* m_aiPlotCardinalDirectionX;	// [NUM_CARDINALDIRECTION_TYPES];
	int* m_aiPlotCardinalDirectionY;	// [NUM_CARDINALDIRECTION_TYPES];
	int* m_aiCityPlotX;	// [NUM_CITY_PLOTS];
	int* m_aiCityPlotY;	// [NUM_CITY_PLOTS];
	int* m_aiCityPlotPriority;	// [NUM_CITY_PLOTS];
	int m_aaiXYCityPlot[CITY_PLOTS_DIAMETER][CITY_PLOTS_DIAMETER];

	DirectionTypes* m_aeTurnLeftDirection;	// [NUM_DIRECTION_TYPES];
	DirectionTypes* m_aeTurnRightDirection;	// [NUM_DIRECTION_TYPES];
	DirectionTypes m_aaeXYDirection[DIRECTION_DIAMETER][DIRECTION_DIAMETER];

	//InterfaceModeInfo m_aInterfaceModeInfo[NUM_INTERFACEMODE_TYPES] =
	std::vector<CvInterfaceModeInfo*> m_paInterfaceModeInfo;

	/***********************************************************************************************************************
	Globals loaded from XML
	************************************************************************************************************************/

	// Hash function for zero terminated strings.
	struct SZStringHash {
		enum { // parameters for hash table
			bucket_size = 4, // 0 < bucket_size
			min_buckets = 4096,
		};
		typedef const char* Key;

		// Hashing from https://create.stephan-brumme.com/fnv-hash/
		static const size_t Prime = 0x01000193; //   16777619
		static const size_t Seed = 0x811C9DC5;  // 2166136261

		// hash a single byte
		static inline size_t fnv1a(unsigned char oneByte, size_t hash = Seed)
		{
			return (oneByte ^ hash) * Prime;
		}

		// hash a block of memory
		static inline size_t fnv1a(const void* data, size_t numBytes, size_t hash = Seed)
		{
			assert(data);
			const unsigned char* ptr = (const unsigned char*)data;
			while (numBytes--)
				hash = fnv1a(*ptr++, hash);
			return hash;
		}

		size_t operator()(const Key& key) const
		{
			return fnv1a(key, strlen(key));
		}

		bool operator()(const Key& key1, const Key& key2) const
		{
			return strcmp(key1, key2) > 0;
		}
	};

	// all type strings are upper case and are kept in this hash map for fast lookup, Moose
	typedef stdext::hash_map<const char* /* type */, int /* info index */, SZStringHash> InfosMap;
	InfosMap m_infosMap;
	std::vector<std::vector<CvInfoBase *> *> m_aInfoVectors;

	int m_iLastTypeID; // last generic type ID assigned (for type strings that do not have an assigned info class)

	// AIAndy: Delayed resolution of type strings
	typedef std::map<int*,std::pair<CvString,CvString> > DelayedResolutionMap;
	DelayedResolutionMap m_delayedResolutionMap;


	std::vector<CvColorInfo*> m_paColorInfo;
	std::vector<CvPlayerColorInfo*> m_paPlayerColorInfo;
	std::vector<CvAdvisorInfo*> m_paAdvisorInfo;
	std::vector<CvInfoBase*> m_paHints;
	std::vector<CvMainMenuInfo*> m_paMainMenus;
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 10/30/07                            MRGENIE          */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	// MLF loading
	std::vector<CvString> m_paModLoadControlVector;
	std::vector<CvModLoadControlInfo*> m_paModLoadControls;
	int m_iTotalNumModules;

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
	std::vector<CvTerrainInfo*> m_paTerrainInfo;
	CvInfoReplacements<CvTerrainInfo> m_TerrainInfoReplacements;
	std::vector<CvLandscapeInfo*> m_paLandscapeInfo;
	int m_iActiveLandscapeID;
	std::vector<CvWorldInfo*> m_paWorldInfo;
	CvInfoReplacements<CvWorldInfo> m_WorldInfoReplacements;
	std::vector<CvClimateInfo*> m_paClimateInfo;
	std::vector<CvSeaLevelInfo*> m_paSeaLevelInfo;
	std::vector<CvYieldInfo*> m_paYieldInfo;
	std::vector<CvCommerceInfo*> m_paCommerceInfo;
	std::vector<CvRouteInfo*> m_paRouteInfo;
	CvInfoReplacements<CvRouteInfo> m_RouteInfoReplacements;
	std::vector<CvFeatureInfo*> m_paFeatureInfo;
	CvInfoReplacements<CvFeatureInfo> m_FeatureInfoReplacements;
	std::vector<CvBonusClassInfo*> m_paBonusClassInfo;
	CvInfoReplacements<CvBonusClassInfo> m_BonusClassInfoReplacements;
	std::vector<CvBonusInfo*> m_paBonusInfo;
	CvInfoReplacements<CvBonusInfo> m_BonusInfoReplacements;
	std::vector<CvImprovementInfo*> m_paImprovementInfo;
	CvInfoReplacements<CvImprovementInfo> m_ImprovementInfoReplacements;
	std::vector<CvGoodyInfo*> m_paGoodyInfo;
	std::vector<CvBuildInfo*> m_paBuildInfo;
	CvInfoReplacements<CvBuildInfo> m_BuildInfoReplacements;
	std::vector<CvHandicapInfo*> m_paHandicapInfo;
	CvInfoReplacements<CvHandicapInfo> m_HandicapInfoReplacements;
	std::vector<CvGameSpeedInfo*> m_paGameSpeedInfo;
	CvInfoReplacements<CvGameSpeedInfo> m_GameSpeedInfoReplacements;
	std::vector<CvTurnTimerInfo*> m_paTurnTimerInfo;
	std::vector<CvCivilizationInfo*> m_paCivilizationInfo;
	CvInfoReplacements<CvCivilizationInfo> m_CivilizationInfoReplacements;
	int m_iNumPlayableCivilizationInfos;
	int m_iNumAIPlayableCivilizationInfos;
	std::vector<CvLeaderHeadInfo*> m_paLeaderHeadInfo;
	CvInfoReplacements<CvLeaderHeadInfo> m_LeaderHeadInfoReplacements;
	std::vector<CvTraitInfo*> m_paTraitInfo;
	CvInfoReplacements<CvTraitInfo> m_TraitInfoReplacements;
	std::vector<CvCursorInfo*> m_paCursorInfo;
	std::vector<CvThroneRoomCamera*> m_paThroneRoomCamera;
	std::vector<CvThroneRoomInfo*> m_paThroneRoomInfo;
	std::vector<CvThroneRoomStyleInfo*> m_paThroneRoomStyleInfo;
	std::vector<CvSlideShowInfo*> m_paSlideShowInfo;
	std::vector<CvSlideShowRandomInfo*> m_paSlideShowRandomInfo;
	std::vector<CvWorldPickerInfo*> m_paWorldPickerInfo;
	std::vector<CvSpaceShipInfo*> m_paSpaceShipInfo;
	std::vector<CvProcessInfo*> m_paProcessInfo;
	CvInfoReplacements<CvProcessInfo> m_ProcessInfoReplacements;
	std::vector<CvVoteInfo*> m_paVoteInfo;
	std::vector<CvProjectInfo*> m_paProjectInfo;
	CvInfoReplacements<CvProjectInfo> m_ProjectInfoReplacements;
	std::vector<CvBuildingInfo*> m_paBuildingInfo;
	CvInfoReplacements<CvBuildingInfo> m_BuildingInfoReplacements;
	std::vector<CvSpecialBuildingInfo*> m_paSpecialBuildingInfo;
	CvInfoReplacements<CvSpecialBuildingInfo> m_SpecialBuildingInfoReplacements;
	std::vector<CvUnitInfo*> m_paUnitInfo;
	CvInfoReplacements<CvUnitInfo> m_UnitInfoReplacements;
	std::vector<CvSpawnInfo*> m_paSpawnInfo;
	CvInfoReplacements<CvSpawnInfo> m_SpawnInfoReplacements;
	std::vector<CvSpecialUnitInfo*> m_paSpecialUnitInfo;
	std::vector<CvInfoBase*> m_paConceptInfo;
	std::vector<CvInfoBase*> m_paNewConceptInfo;
	std::vector<CvInfoBase*> m_paCityTabInfo;
	std::vector<CvInfoBase*> m_paCalendarInfo;
	std::vector<CvInfoBase*> m_paSeasonInfo;
	std::vector<CvInfoBase*> m_paMonthInfo;
	std::vector<CvInfoBase*> m_paDenialInfo;
	std::vector<CvInvisibleInfo*> m_paInvisibleInfo;
	std::vector<CvVoteSourceInfo*> m_paVoteSourceInfo;
	std::vector<CvUnitCombatInfo*> m_paUnitCombatInfo;
	//TB Promotion Line Mod begin
	std::vector<CvPromotionLineInfo*> m_paPromotionLineInfo;
	//TB Promotion Line Mod end
	std::vector<CvMapCategoryInfo*> m_paMapCategoryInfo;
	std::vector<CvIdeaClassInfo*> m_paIdeaClassInfo;
	std::vector<CvIdeaInfo*> m_paIdeaInfo;
	std::vector<CvInfoBase*> m_paDomainInfo;
	std::vector<CvInfoBase*> m_paUnitAIInfos;
	std::vector<CvInfoBase*> m_paAttitudeInfos;
	std::vector<CvInfoBase*> m_paMemoryInfos;
	std::vector<CvInfoBase*> m_paFeatInfos;
	std::vector<CvGameOptionInfo*> m_paGameOptionInfos;
	std::vector<CvMPOptionInfo*> m_paMPOptionInfos;
	std::vector<CvForceControlInfo*> m_paForceControlInfos;
	std::vector<CvPlayerOptionInfo*> m_paPlayerOptionInfos;
	std::vector<CvGraphicOptionInfo*> m_paGraphicOptionInfos;
	std::vector<CvSpecialistInfo*> m_paSpecialistInfo;
	CvInfoReplacements<CvSpecialistInfo> m_SpecialistInfoReplacements;
	std::vector<CvEmphasizeInfo*> m_paEmphasizeInfo;
	std::vector<CvUpkeepInfo*> m_paUpkeepInfo;
	std::vector<CvCultureLevelInfo*> m_paCultureLevelInfo;
	CvInfoReplacements<CvCultureLevelInfo> m_CultureLevelInfoReplacements;
	std::vector<CvReligionInfo*> m_paReligionInfo;
	CvInfoReplacements<CvReligionInfo> m_ReligionInfoReplacements;
	std::vector<CvCorporationInfo*> m_paCorporationInfo;
	CvInfoReplacements<CvCorporationInfo> m_CorporationInfoReplacements;
	std::vector<CvActionInfo*> m_paActionInfo;
	std::vector<CvMissionInfo*> m_paMissionInfo;
	std::vector<CvControlInfo*> m_paControlInfo;
	std::vector<CvCommandInfo*> m_paCommandInfo;
	std::vector<CvAutomateInfo*> m_paAutomateInfo;
	std::vector<CvPromotionInfo*> m_paPromotionInfo;
	CvInfoReplacements<CvPromotionInfo> m_PromotionInfoReplacements;
	std::vector<CvTechInfo*> m_paTechInfo;
	CvInfoReplacements<CvTechInfo> m_TechInfoReplacements;
	std::vector<CvCivicOptionInfo*> m_paCivicOptionInfo;
	std::vector<CvCivicInfo*> m_paCivicInfo;
	CvInfoReplacements<CvCivicInfo> m_CivicInfoReplacements;
	std::vector<CvDiplomacyInfo*> m_paDiplomacyInfo;
	std::vector<CvEraInfo*> m_aEraInfo;	// [NUM_ERA_TYPES];
	CvInfoReplacements<CvEraInfo> m_EraInfoReplacements;
	std::vector<CvHurryInfo*> m_paHurryInfo;
	std::vector<CvVictoryInfo*> m_paVictoryInfo;
	std::vector<CvRouteModelInfo*> m_paRouteModelInfo;
	std::vector<CvRiverInfo*> m_paRiverInfo;
	std::vector<CvRiverModelInfo*> m_paRiverModelInfo;
	std::vector<CvWaterPlaneInfo*> m_paWaterPlaneInfo;
	std::vector<CvTerrainPlaneInfo*> m_paTerrainPlaneInfo;
	std::vector<CvCameraOverlayInfo*> m_paCameraOverlayInfo;
	std::vector<CvAnimationPathInfo*> m_paAnimationPathInfo;
	std::vector<CvAnimationCategoryInfo*> m_paAnimationCategoryInfo;
	std::vector<CvEntityEventInfo*> m_paEntityEventInfo;
	std::vector<CvUnitFormationInfo*> m_paUnitFormationInfo;
	std::vector<CvEffectInfo*> m_paEffectInfo;
	std::vector<CvAttachableInfo*> m_paAttachableInfo;
	std::vector<CvCameraInfo*> m_paCameraInfo;
	std::vector<CvQuestInfo*> m_paQuestInfo;
	std::vector<CvTutorialInfo*> m_paTutorialInfo;
	std::vector<CvEventTriggerInfo*> m_paEventTriggerInfo;
	CvInfoReplacements<CvEventTriggerInfo> m_EventTriggerInfoReplacements;
	std::vector<CvEventInfo*> m_paEventInfo;
	CvInfoReplacements<CvEventInfo> m_EventInfoReplacements;
	std::vector<CvEspionageMissionInfo*> m_paEspionageMissionInfo;
	std::vector<CvUnitArtStyleTypeInfo*> m_paUnitArtStyleTypeInfo;
	std::vector<CvPropertyInfo*> m_paPropertyInfo;
	std::vector<CvOutcomeInfo*> m_paOutcomeInfo;
	std::vector<CvCivNameInfo*> m_paCivNameInfo;

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	std::vector<CvMapInfo*> m_paMapInfo;
	std::vector<CvMapSwitchInfo*> m_paMapSwitchInfo;
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

	// Game Text
	std::vector<CvGameText*> m_paGameTextXML;

	//////////////////////////////////////////////////////////////////////////
	// GLOBAL TYPES
	//////////////////////////////////////////////////////////////////////////

	// all type strings are upper case and are kept in this hash map for fast lookup, Moose
	typedef stdext::hash_map<std::string /* type string */, int /*enum value */> TypesMap;
	TypesMap m_typesMap;

	// XXX These are duplicates and are kept for enumeration convenience - most could be removed, Moose
	CvString *m_paszEntityEventTypes2;
	CvString *m_paszEntityEventTypes;
	int m_iNumEntityEventTypes;

	CvString *m_paszAnimationOperatorTypes;
	int m_iNumAnimationOperatorTypes;

	CvString* m_paszFunctionTypes;

	CvString* m_paszFlavorTypes;
	int m_iNumFlavorTypes;

	CvString *m_paszArtStyleTypes;
	int m_iNumArtStyleTypes;

	CvString *m_paszCitySizeTypes;
	int m_iNumCitySizeTypes;

	CvString *m_paszContactTypes;

	CvString *m_paszDiplomacyPowerTypes;

	CvString *m_paszAutomateTypes;

	CvString *m_paszDirectionTypes;

	CvString *m_paszFootstepAudioTypes;
	int m_iNumFootstepAudioTypes;

	CvString *m_paszFootstepAudioTags;
//	int m_iNumFootstepAudioTags; (unused)

	CvString m_szCurrentXMLFile;
	//////////////////////////////////////////////////////////////////////////
	// Formerly Global Defines
	//////////////////////////////////////////////////////////////////////////

	FVariableSystem* m_VarSystem;

/************************************************************************************************/
/* Mod Globals    Start                          09/13/10                           phungus420  */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	bool m_bDCM_BATTLE_EFFECTS;
	int m_iBATTLE_EFFECT_LESS_FOOD;
	int m_iBATTLE_EFFECT_LESS_PRODUCTION;
	int m_iBATTLE_EFFECT_LESS_COMMERCE;
	int m_iBATTLE_EFFECTS_MINIMUM_TURN_INCREMENTS;
	int m_iMAX_BATTLE_TURNS;

	bool m_bDCM_AIR_BOMBING;
	bool m_bDCM_RANGE_BOMBARD;
	int m_iDCM_RB_CITY_INACCURACY;
	int m_iDCM_RB_CITYBOMBARD_CHANCE;
	bool m_bDCM_ATTACK_SUPPORT;
	bool m_bDCM_OPP_FIRE;
	bool m_bDCM_ACTIVE_DEFENSE;
	bool m_bDCM_FIGHTER_ENGAGE;

	bool m_bDYNAMIC_CIV_NAMES;

	bool m_bLIMITED_RELIGIONS_EXCEPTIONS;
	bool m_bOC_RESPAWN_HOLY_CITIES;

	bool m_bIDW_ENABLED;
	float m_fIDW_BASE_COMBAT_INFLUENCE;
	float m_fIDW_NO_CITY_DEFENDER_MULTIPLIER;
	float m_fIDW_FORT_CAPTURE_MULTIPLIER;
	float m_fIDW_EXPERIENCE_FACTOR;
	float m_fIDW_WARLORD_MULTIPLIER;
	int m_iIDW_INFLUENCE_RADIUS;
	float m_fIDW_PLOT_DISTANCE_FACTOR;
	float m_fIDW_WINNER_PLOT_MULTIPLIER;
	float m_fIDW_LOSER_PLOT_MULTIPLIER;
	bool m_bIDW_EMERGENCY_DRAFT_ENABLED;
	int m_iIDW_EMERGENCY_DRAFT_MIN_POPULATION;
	float m_fIDW_EMERGENCY_DRAFT_STRENGTH;
	float m_fIDW_EMERGENCY_DRAFT_ANGER_MULTIPLIER;
	bool m_bIDW_NO_BARBARIAN_INFLUENCE;
	bool m_bIDW_NO_NAVAL_INFLUENCE;
	bool m_bIDW_PILLAGE_INFLUENCE_ENABLED;
	float m_fIDW_BASE_PILLAGE_INFLUENCE;
	float m_fIDW_CITY_TILE_MULTIPLIER;

	bool m_bSS_ENABLED;
	bool m_bSS_BRIBE;
	bool m_bSS_ASSASSINATE;
/************************************************************************************************/
/* Mod Globals                        END                                           phungus420  */
/************************************************************************************************/
	int m_iMOVE_DENOMINATOR;
	int m_iNUM_UNIT_PREREQ_OR_BONUSES;
	int m_iNUM_BUILDING_PREREQ_OR_BONUSES;
	int m_iFOOD_CONSUMPTION_PER_POPULATION;
	int m_iMAX_HIT_POINTS;
	int m_iPATH_DAMAGE_WEIGHT;
	int m_iHILLS_EXTRA_DEFENSE;
/************************************************************************************************/
/* Afforess	                		 12/21/09                                                   */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int m_iPEAK_EXTRA_DEFENSE;
	int m_iPEAK_EXTRA_MOVEMENT;
	bool m_bXMLLogging;
	bool m_bLoadedPlayerOptions;

	int m_iSCORE_FREE_PERCENT;
	int m_iSCORE_POPULATION_FACTOR;
	int m_iSCORE_LAND_FACTOR;
	int m_iSCORE_TECH_FACTOR;
	int m_iSCORE_WONDER_FACTOR;

	int m_iUSE_CAN_CREATE_PROJECT_CALLBACK;
	int m_iUSE_CANNOT_CREATE_PROJECT_CALLBACK;
	int m_iUSE_CAN_DO_MELTDOWN_CALLBACK;
	int m_iUSE_CAN_MAINTAIN_PROCESS_CALLBACK;
	int m_iUSE_CANNOT_MAINTAIN_PROCESS_CALLBACK;
	int m_iUSE_CAN_DO_GROWTH_CALLBACK;
	int m_iUSE_CAN_DO_CULTURE_CALLBACK;
	int m_iUSE_CAN_DO_PLOT_CULTURE_CALLBACK;
	int m_iUSE_CAN_DO_PRODUCTION_CALLBACK;
	int m_iUSE_CAN_DO_RELIGION_CALLBACK;
	int m_iUSE_CAN_DO_GREATPEOPLE_CALLBACK;
	int m_iUSE_CAN_RAZE_CITY_CALLBACK;
	int m_iUSE_CAN_DO_GOLD_CALLBACK;
	int m_iUSE_CAN_DO_RESEARCH_CALLBACK;
	int m_iUSE_UPGRADE_UNIT_PRICE_CALLBACK;
	int m_iUSE_IS_VICTORY_CALLBACK;
	int m_iUSE_AI_UPDATE_UNIT_CALLBACK;
	int m_iUSE_AI_CHOOSE_PRODUCTION_CALLBACK;
	int m_iUSE_EXTRA_PLAYER_COSTS_CALLBACK;
	int m_iUSE_AI_DO_DIPLO_CALLBACK;
	int m_iUSE_AI_BESTTECH_CALLBACK;
	int m_iUSE_CAN_DO_COMBAT_CALLBACK;
	int m_iUSE_AI_CAN_DO_WARPLANS_CALLBACK;
/************************************************************************************************/
/* Afforess	Mountains End       END        		                                             */
/************************************************************************************************/
	int m_iRIVER_ATTACK_MODIFIER;
	int m_iAMPHIB_ATTACK_MODIFIER;
	int m_iHILLS_EXTRA_MOVEMENT;
	int m_iRIVER_EXTRA_MOVEMENT;
	int m_iMAX_PLOT_LIST_ROWS;
	int m_iUNIT_MULTISELECT_MAX;
	int m_iPERCENT_ANGER_DIVISOR;
	int m_iEVENT_MESSAGE_TIME;
	int m_iROUTE_FEATURE_GROWTH_MODIFIER;
	int m_iFEATURE_GROWTH_MODIFIER;
	int m_iMIN_CITY_RANGE;
	int m_iCITY_MAX_NUM_BUILDINGS;
	int m_iNUM_UNIT_AND_TECH_PREREQS;
	int m_iNUM_AND_TECH_PREREQS;
	int m_iNUM_OR_TECH_PREREQS;
	int m_iLAKE_MAX_AREA_SIZE;
	int m_iNUM_ROUTE_PREREQ_OR_BONUSES;
	int m_iNUM_BUILDING_AND_TECH_PREREQS;
	int m_iMIN_WATER_SIZE_FOR_OCEAN;
	int m_iFORTIFY_MODIFIER_PER_TURN;
	int m_iESTABLISH_MODIFIER_PER_TURN;
	int m_iESCAPE_MODIFIER_PER_TURN;
	int m_iMAX_CITY_DEFENSE_DAMAGE;
	int m_iNUM_CORPORATION_PREREQ_BONUSES;
	int m_iPEAK_SEE_THROUGH_CHANGE;
	int m_iHILLS_SEE_THROUGH_CHANGE;
	int m_iSEAWATER_SEE_FROM_CHANGE;
	int m_iPEAK_SEE_FROM_CHANGE;
	int m_iHILLS_SEE_FROM_CHANGE;
	int m_iUSE_SPIES_NO_ENTER_BORDERS;

	float m_fCAMERA_MIN_YAW;
	float m_fCAMERA_MAX_YAW;
	float m_fCAMERA_FAR_CLIP_Z_HEIGHT;
	float m_fCAMERA_MAX_TRAVEL_DISTANCE;
	float m_fCAMERA_START_DISTANCE;
	float m_fAIR_BOMB_HEIGHT;
	float m_fPLOT_SIZE;
	float m_fCAMERA_SPECIAL_PITCH;
	float m_fCAMERA_MAX_TURN_OFFSET;
	float m_fCAMERA_MIN_DISTANCE;
	float m_fCAMERA_UPPER_PITCH;
	float m_fCAMERA_LOWER_PITCH;
	float m_fFIELD_OF_VIEW;
	float m_fSHADOW_SCALE;
	float m_fUNIT_MULTISELECT_DISTANCE;
	float m_fSAD_FACTOR_1;
	float m_fSAD_FACTOR_2;
	float m_fSAD_FACTOR_3;
	float m_fSAD_FACTOR_4;

	int m_iUSE_CANNOT_FOUND_CITY_CALLBACK;
	int m_iUSE_CAN_FOUND_CITIES_ON_WATER_CALLBACK;
	int m_iUSE_IS_PLAYER_RESEARCH_CALLBACK;
	int m_iUSE_CAN_RESEARCH_CALLBACK;
	int m_iUSE_CANNOT_DO_CIVIC_CALLBACK;
	int m_iUSE_CAN_DO_CIVIC_CALLBACK;
	int m_iUSE_CANNOT_CONSTRUCT_CALLBACK;
	int m_iUSE_CAN_CONSTRUCT_CALLBACK;
	int m_iUSE_CAN_DECLARE_WAR_CALLBACK;
	int m_iUSE_CANNOT_RESEARCH_CALLBACK;
	int m_iUSE_GET_UNIT_COST_MOD_CALLBACK;
	int m_iUSE_GET_BUILDING_COST_MOD_CALLBACK;
	int m_iUSE_GET_CITY_FOUND_VALUE_CALLBACK;
	int m_iUSE_CANNOT_HANDLE_ACTION_CALLBACK;
	int m_iUSE_CAN_BUILD_CALLBACK;
	int m_iUSE_CANNOT_TRAIN_CALLBACK;
	int m_iUSE_CAN_TRAIN_CALLBACK;
	int m_iUSE_UNIT_CANNOT_MOVE_INTO_CALLBACK;
	int m_iUSE_USE_CANNOT_SPREAD_RELIGION_CALLBACK;
	int m_iUSE_FINISH_TEXT_CALLBACK;
	int m_iUSE_ON_UNIT_SET_XY_CALLBACK;
	int m_iUSE_ON_UNIT_SELECTED_CALLBACK;
	int m_iUSE_ON_UPDATE_CALLBACK;
	int m_iUSE_ON_UNIT_CREATED_CALLBACK;
	int m_iUSE_ON_UNIT_LOST_CALLBACK;
	int m_iLAND_UNITS_CAN_ATTACK_WATER_CITIES;
	int m_iBASE_UNIT_UPGRADE_COST;
	int m_iUPGRADE_ROUND_LIMIT;
	int m_iCITY_BARBARIAN_DEFENSE_MODIFIER;
	int m_iUNIT_VISIBILITY_RANGE;
	int m_iMAX_UNIT_VISIBILITY_RANGE;
	int m_iGREATER_COMMERCE_SWITCH_POINT;
	int m_iWORKER_TRADE_VALUE_PERCENT_ADJUSTMENT;
	int m_iTRADE_MISSION_END_TOTAL_PERCENT_ADJUSTMENT;
	int m_iINFILTRATE_MISSION_END_TOTAL_PERCENT_ADJUSTMENT;
	int m_iESPIONAGE_MISSION_COST_END_TOTAL_PERCENT_ADJUSTMENT;
	int m_iWATER_POTENTIAL_CITY_WORK_FOR_AREA;
	int m_iSAD_MAX_MODIFIER;
	int m_iUPSCALED_RESEARCH_COST_MODIFIER;
	int m_iENABLE_DYNAMIC_UNIT_ENTITIES;
/************************************************************************************************/
/* MODULES                                 11/13/07                            MRGENIE          */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int m_iTGA_RELIGIONS;
	int m_iTGA_CORPORATIONS;

/************************************************************************************************/
/* MODULES                                 END                                                  */
/************************************************************************************************/
	bool m_bMultimapsEnabled;
	bool m_bViewportsEnabled;
	int	m_iViewportFocusBorder;
	int m_iViewportSizeX;
	int m_iViewportSizeY;
	int m_iViewportCenterOnSelectionCenterBorder;
	bool m_bGraphicalDetailPagingEnabled;

	const char* m_szAlternateProfilSampleName;
	FProfiler* m_Profiler;		// profiler
	CvString m_szDllProfileText;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      02/21/10                                jdog5000      */
/*                                                                                              */
/* Efficiency, Options                                                                          */
/************************************************************************************************/
public:
	int getDefineINT( const char * szName, const int iDefault ) const;

// BBAI Options
public:
	bool getBBAI_AIR_COMBAT() const;
	bool getBBAI_HUMAN_VASSAL_WAR_BUILD() const;
	int getBBAI_DEFENSIVE_PACT_BEHAVIOR() const;
	bool getBBAI_HUMAN_AS_VASSAL_OPTION() const;

protected:
	bool m_bBBAI_AIR_COMBAT;
	bool m_bBBAI_HUMAN_VASSAL_WAR_BUILD;
	int m_iBBAI_DEFENSIVE_PACT_BEHAVIOR;
	bool m_bBBAI_HUMAN_AS_VASSAL_OPTION;

// BBAI AI Variables
public:
	int getWAR_SUCCESS_CITY_CAPTURING() const;
	int getBBAI_ATTACK_CITY_STACK_RATIO() const;
	int getBBAI_SKIP_BOMBARD_BEST_ATTACK_ODDS() const;
	int getBBAI_SKIP_BOMBARD_BASE_STACK_RATIO() const;
	int getBBAI_SKIP_BOMBARD_MIN_STACK_RATIO() const;

protected:
	int m_iWAR_SUCCESS_CITY_CAPTURING;
	int m_iBBAI_ATTACK_CITY_STACK_RATIO;
	int m_iBBAI_SKIP_BOMBARD_BEST_ATTACK_ODDS;
	int m_iBBAI_SKIP_BOMBARD_BASE_STACK_RATIO;
	int m_iBBAI_SKIP_BOMBARD_MIN_STACK_RATIO;

// Tech Diffusion
public:
	bool getTECH_DIFFUSION_ENABLE() const;
	int getTECH_DIFFUSION_KNOWN_TEAM_MODIFIER() const;
	int getTECH_DIFFUSION_WELFARE_THRESHOLD() const;
	int getTECH_DIFFUSION_WELFARE_MODIFIER() const;
	int getTECH_COST_FIRST_KNOWN_PREREQ_MODIFIER() const;
	int getTECH_COST_KNOWN_PREREQ_MODIFIER() const;
	int getTECH_COST_MODIFIER() const;
	int getUNIT_PRODUCTION_PERCENT_SM() const;
	int getUNIT_PRODUCTION_PERCENT() const;
	int getBUILDING_PRODUCTION_PERCENT() const;

protected:
	bool m_bTECH_DIFFUSION_ENABLE;
	int m_iTECH_DIFFUSION_KNOWN_TEAM_MODIFIER;
	int m_iTECH_DIFFUSION_WELFARE_THRESHOLD;
	int m_iTECH_DIFFUSION_WELFARE_MODIFIER;
	int m_iTECH_COST_FIRST_KNOWN_PREREQ_MODIFIER;
	int m_iTECH_COST_KNOWN_PREREQ_MODIFIER;
	int m_iTECH_COST_MODIFIER;
	int m_iUNIT_PRODUCTION_PERCENT_SM;
	int m_iUNIT_PRODUCTION_PERCENT;
	int m_iBUILDING_PRODUCTION_PERCENT;

public:
	int getCOMBAT_DIE_SIDES() const;
	int getCOMBAT_DAMAGE() const;
protected:
	int m_iCOMBAT_DIE_SIDES;
	int m_iCOMBAT_DAMAGE;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 11/30/07                                MRGENIE      */
/*                                                                                              */
/* Savegame compatibility                                                                       */
/************************************************************************************************/
	int* m_iStoreExeSettingsCommerceInfo;
	int* m_iStoreExeSettingsYieldInfo;
	int* m_iStoreExeSettingsReligionInfo;
	int* m_iStoreExeSettingsCorporationInfo;
	int* m_iStoreExeSettingsBonusInfo;
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/

	bool m_bSignsCleared;
	bool m_bIsInPedia;
};

extern cvInternalGlobals* gGlobals;	// for debugging

class CvGlobals;

#ifdef _DEBUG
extern int inDLL;
extern const char* fnName;

class ProxyTracker
{
public:
	ProxyTracker(const CvGlobals* proxy, const char* name);
	~ProxyTracker();
};

#define PROXY_TRACK(x)	ProxyTracker tracker(this,x);

#else
#define	PROXY_TRACK(x)	;
#endif

//	cvGlobals is a proxy class with the same virtual interface as cvInternalGlobals
//	which just passes all requests onto the global internal instance.  This allows it to
//	be statically constructed so that IT's GetInstance() (as called by the Civ core engine
//	to retrieve a pointer to the DLLs virtual function table) can have a value to return prior
//	to any allocations takign pace (whicgh must be deferred until after SetDLLIFace() has been called
//	by the core engine to establish memory allocators)
class CvGlobals
{
//	friend class CvDLLUtilityIFace;
	friend class CvXMLLoadUtility;
	friend class ProxyTracker;
protected:
	void CheckProxy(const char* fnName) const;

public:

	// singleton accessor
	DllExport inline static CvGlobals& getInstance();

	CvGlobals()
	{
	}
	virtual ~CvGlobals()
	{
	}

	DllExport void init()
	{
		PROXY_TRACK("init");
		gGlobals->init();
	}
	DllExport void uninit()
	{
		PROXY_TRACK("uninit");
		gGlobals->uninit();
	}

	void clearTypesMap()
	{
		PROXY_TRACK("clearTypesMap");
		gGlobals->clearTypesMap();
	}

	DllExport CvDiplomacyScreen* getDiplomacyScreen()
	{
		PROXY_TRACK("getDiplomacyScreen");
		return gGlobals->getDiplomacyScreen();
	}

	DllExport CMPDiplomacyScreen* getMPDiplomacyScreen()
	{
		PROXY_TRACK("getMPDiplomacyScreen");
		return gGlobals->getMPDiplomacyScreen();
	}

	DllExport FMPIManager*& getFMPMgrPtr()
	{
		PROXY_TRACK("getFMPMgrPtr");
		return gGlobals->getFMPMgrPtr();
	}
	DllExport CvPortal& getPortal()
	{
		PROXY_TRACK("getPortal");
		return gGlobals->getPortal();
	}
	DllExport CvSetupData& getSetupData()
	{
		PROXY_TRACK("getSetupData");
		return gGlobals->getSetupData();
	}
	DllExport CvInitCore& getInitCore()
	{
		PROXY_TRACK("getInitCore");
		return gGlobals->getInitCore();
	}
	DllExport CvInitCore& getLoadedInitCore()
	{
		PROXY_TRACK("getLoadedInitCore");
		return gGlobals->getLoadedInitCore();
	}
	DllExport CvInitCore& getIniInitCore()
	{
		PROXY_TRACK("getIniInitCore");
		return gGlobals->getIniInitCore();
	}
	DllExport CvMessageCodeTranslator& getMessageCodes()
	{
		PROXY_TRACK("getMessageCodes");
		return gGlobals->getMessageCodes();
	}
	DllExport CvStatsReporter& getStatsReporter()
	{
		PROXY_TRACK("getStatsReporter");
		return gGlobals->getStatsReporter();
	}
	CvStatsReporter* getStatsReporterPtr()
	{
		PROXY_TRACK("getStatsReporterPtr");
		return gGlobals->getStatsReporterPtr();
	}
	DllExport CvInterface& getInterface()
	{
		PROXY_TRACK("getInterface");
		return gGlobals->getInterface();
	}
	DllExport CvInterface* getInterfacePtr()
	{
		PROXY_TRACK("getInterfacePtr");
		return gGlobals->getInterfacePtr();
	}
	DllExport int getMaxCivPlayers() const
	{
		PROXY_TRACK("getMaxCivPlayers");
		return gGlobals->getMaxCivPlayers();
	}
	DllExport CvMapExternal& getMap()
	{
		PROXY_TRACK("getMap");
		return gGlobals->getMapExternal();
	}
	DllExport CvGameAI& getGame()
	{
		PROXY_TRACK("getGame");
		return gGlobals->getGame();
	}
	DllExport CvGameAI *getGamePointer()
	{
		PROXY_TRACK("getGamePointer");
		return gGlobals->getGamePointer();
	}
	DllExport CvRandom& getASyncRand()
	{
		PROXY_TRACK("getASyncRand");
		return gGlobals->getASyncRand();
	}
	DllExport CMessageQueue& getMessageQueue()
	{
		PROXY_TRACK("getMessageQueue");
		return gGlobals->getMessageQueue();
	}
	DllExport CMessageQueue& getHotMessageQueue()
	{
		PROXY_TRACK("getHotMessageQueue");
		return gGlobals->getHotMessageQueue();
	}
	DllExport CMessageControl& getMessageControl()
	{
		PROXY_TRACK("getMessageControl");
		return gGlobals->getMessageControl();
	}
	DllExport CvDropMgr& getDropMgr()
	{
		PROXY_TRACK("getDropMgr");
		return gGlobals->getDropMgr();
	}
	DllExport FAStar& getPathFinder()
	{
		PROXY_TRACK("getPathFinder");
		return gGlobals->getPathFinder();
	}
	DllExport FAStar& getInterfacePathFinder()
	{
		PROXY_TRACK("getInterfacePathFinder");
		return gGlobals->getInterfacePathFinder();
	}
	DllExport FAStar& getStepFinder()
	{
		PROXY_TRACK("getStepFinder");
		return gGlobals->getStepFinder();
	}
	DllExport FAStar& getRouteFinder()
	{
		PROXY_TRACK("getRouteFinder");
		return gGlobals->getRouteFinder();
	}
	DllExport FAStar& getBorderFinder()
	{
		PROXY_TRACK("getBorderFinder");
		return gGlobals->getBorderFinder();
	}
	DllExport FAStar& getAreaFinder()
	{
		PROXY_TRACK("getAreaFinder");
		return gGlobals->getAreaFinder();
	}
	DllExport FAStar& getPlotGroupFinder()
	{
		PROXY_TRACK("getPlotGroupFinder");
		return gGlobals->getPlotGroupFinder();
	}
	NiPoint3& getPt3Origin()
	{
		PROXY_TRACK("getPt3Origin");
		return gGlobals->getPt3Origin();
	}

	DllExport std::vector<CvInterfaceModeInfo*>& getInterfaceModeInfo()
	{
		PROXY_TRACK("getInterfaceModeInfo");
		return gGlobals->getInterfaceModeInfos();
	}
	DllExport CvInterfaceModeInfo& getInterfaceModeInfo(InterfaceModeTypes e)
	{
		PROXY_TRACK("getInterfaceModeInfo");
		return gGlobals->getInterfaceModeInfo(e);
	}

	NiPoint3& getPt3CameraDir()
	{
		PROXY_TRACK("getPt3CameraDir");
		return gGlobals->getPt3CameraDir();
	}

	DllExport bool& getLogging()
	{
		PROXY_TRACK("getLogging");
		return gGlobals->getLogging();
	}
	DllExport bool& getRandLogging()
	{
		PROXY_TRACK("getRandLogging");
		return gGlobals->getRandLogging();
	}
	DllExport bool& getSynchLogging()
	{
		PROXY_TRACK("getSynchLogging");
		return gGlobals->getSynchLogging();
	}
	DllExport bool& overwriteLogs()
	{
		PROXY_TRACK("overwriteLogs");
		return gGlobals->overwriteLogs();
	}

	DllExport int* getPlotDirectionX()
	{
		PROXY_TRACK("getPlotDirectionX");
		return gGlobals->getPlotDirectionX();
	}
	DllExport int* getPlotDirectionY()
	{
		PROXY_TRACK("getPlotDirectionY");
		return gGlobals->getPlotDirectionY();
	}
	DllExport int* getPlotCardinalDirectionX()
	{
		PROXY_TRACK("getPlotCardinalDirectionX");
		return gGlobals->getPlotCardinalDirectionX();
	}
	DllExport int* getPlotCardinalDirectionY()
	{
		PROXY_TRACK("getPlotCardinalDirectionY");
		return gGlobals->getPlotCardinalDirectionY();
	}
	int* getCityPlotX()
	{
		PROXY_TRACK("getCityPlotX");
		return gGlobals->getCityPlotX();
	}
	int* getCityPlotY()
	{
		PROXY_TRACK("getCityPlotY");
		return gGlobals->getCityPlotY();
	}
	int* getCityPlotPriority()
	{
		PROXY_TRACK("getCityPlotPriority");
		return gGlobals->getCityPlotPriority();
	}
	int getXYCityPlot(int i, int j)
	{
		PROXY_TRACK("getXYCityPlot");
		return gGlobals->getXYCityPlot(i,j);
	}
	DirectionTypes* getTurnLeftDirection()
	{
		PROXY_TRACK("getTurnLeftDirection");
		return gGlobals->getTurnLeftDirection();
	}
	DirectionTypes getTurnLeftDirection(int i)
	{
		PROXY_TRACK("getTurnLeftDirection(i)");
		return gGlobals->getTurnLeftDirection(i);
	}
	DirectionTypes* getTurnRightDirection()
	{
		PROXY_TRACK("getTurnRightDirection");
		return gGlobals->getTurnRightDirection();
	}
	DirectionTypes getTurnRightDirection(int i)
	{
		PROXY_TRACK("getTurnRightDirection(i)");
		return gGlobals->getTurnRightDirection(i);
	}
	DllExport DirectionTypes getXYDirection(int i, int j)
	{
		PROXY_TRACK("getXYDirection");
		return gGlobals->getXYDirection(i,j);
	}
	//
	// Global Infos
	// All info type strings are upper case and are kept in this hash map for fast lookup
	//
	DllExport int getInfoTypeForString(const char* szType, bool hideAssert = false) const			// returns the infos index, use this when searching for an info type string
	{
		PROXY_TRACK("getInfoTypeForString");
		return gGlobals->getInfoTypeForString(szType, hideAssert);
	}
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 11/30/07                                MRGENIE      */
/*                                                                                              */
/* Savegame compatibility                                                                       */
/************************************************************************************************/
	DllExport void infoTypeFromStringReset()
	{
		PROXY_TRACK("infoTypeFromStringReset");
		gGlobals->infoTypeFromStringReset();
	}
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
	void addToInfosVectors(void *infoVector)
	{
		PROXY_TRACK("addToInfosVectors");
		gGlobals->addToInfosVectors(infoVector);
	}
	DllExport void infosReset()
	{
		PROXY_TRACK("infosReset");
		gGlobals->infosReset();
	}

	DllExport int getNumWorldInfos()
	{
		PROXY_TRACK("getNumWorldInfos");
		return gGlobals->getNumWorldInfos();
	}
	DllExport CvWorldInfo& getWorldInfo(WorldSizeTypes e)
	{
		PROXY_TRACK("getWorldInfo");
		return gGlobals->getWorldInfo(e);
	}

	DllExport int getNumClimateInfos()
	{
		PROXY_TRACK("getNumClimateInfos");
		return gGlobals->getNumClimateInfos();
	}
	DllExport CvClimateInfo& getClimateInfo(ClimateTypes e)
	{
		PROXY_TRACK("getClimateInfo");
		return gGlobals->getClimateInfo(e);
	}

	DllExport int getNumSeaLevelInfos()
	{
		PROXY_TRACK("getNumSeaLevelInfos");
		return gGlobals->getNumSeaLevelInfos();
	}
	DllExport CvSeaLevelInfo& getSeaLevelInfo(SeaLevelTypes e)
	{
		PROXY_TRACK("getSeaLevelInfo");
		return gGlobals->getSeaLevelInfo(e);
	}

	int getNumColorInfos()
	{
		PROXY_TRACK("getNumColorInfos");
		return gGlobals->getNumColorInfos();
	}
	DllExport CvColorInfo& getColorInfo(ColorTypes e)
	{
		PROXY_TRACK("getColorInfo");
		return gGlobals->getColorInfo(e);
	}

	DllExport int getNumPlayerColorInfos()
	{
		PROXY_TRACK("getNumPlayerColorInfos");
		return gGlobals->getNumPlayerColorInfos();
	}
	DllExport CvPlayerColorInfo& getPlayerColorInfo(PlayerColorTypes e)
	{
		PROXY_TRACK("getPlayerColorInfo");
		return gGlobals->getPlayerColorInfo(e);
	}

	DllExport  int getNumHints()
	{
		PROXY_TRACK("getNumHints");
		return gGlobals->getNumHints();
	}
	DllExport CvInfoBase& getHints(int i)
	{
		PROXY_TRACK("getHints");
		return gGlobals->getHints(i);
	}

	int getNumMainMenus()
	{
		PROXY_TRACK("getNumMainMenus");
		return gGlobals->getNumMainMenus();
	}
	DllExport CvMainMenuInfo& getMainMenus(int i)
	{
		PROXY_TRACK("getMainMenus");
		return gGlobals->getMainMenus(i);
	}
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 10/30/07                            MRGENIE          */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	CvModLoadControlInfo& getModLoadControlInfos(int i)
	{
		PROXY_TRACK("getModLoadControlInfos");
		return gGlobals->getModLoadControlInfos(i);
	}
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/

	DllExport int getNumRouteModelInfos()
	{
		PROXY_TRACK("getNumRouteModelInfos");
		return gGlobals->getNumRouteModelInfos();
	}
	DllExport CvRouteModelInfo& getRouteModelInfo(int i)
	{
		PROXY_TRACK("getRouteModelInfo");
		return gGlobals->getRouteModelInfo(i);
	}

	int getNumRiverInfos()
	{
		PROXY_TRACK("getNumRiverInfos");
		return gGlobals->getNumRiverInfos();
	}
	CvRiverInfo& getRiverInfo(RiverTypes e)
	{
		PROXY_TRACK("getRiverInfo");
		return gGlobals->getRiverInfo(e);
	}

	DllExport int getNumRiverModelInfos()
	{
		PROXY_TRACK("getNumRiverModelInfos");
		return gGlobals->getNumRiverModelInfos();
	}
	DllExport CvRiverModelInfo& getRiverModelInfo(int i)
	{
		PROXY_TRACK("getRiverModelInfo");
		return gGlobals->getRiverModelInfo(i);
	}

	int getNumWaterPlaneInfos()
	{
		PROXY_TRACK("getNumWaterPlaneInfos");
		return gGlobals->getNumWaterPlaneInfos();
	}
	DllExport CvWaterPlaneInfo& getWaterPlaneInfo(int i)
	{
		PROXY_TRACK("getWaterPlaneInfo");
		return gGlobals->getWaterPlaneInfo(i);
	}

	DllExport int getNumTerrainPlaneInfos()
	{
		PROXY_TRACK("getNumTerrainPlaneInfos");
		return gGlobals->getNumTerrainPlaneInfos();
	}
	DllExport CvTerrainPlaneInfo& getTerrainPlaneInfo(int i)
	{
		PROXY_TRACK("getTerrainPlaneInfo");
		return gGlobals->getTerrainPlaneInfo(i);
	}

	DllExport int getNumCameraOverlayInfos()
	{
		PROXY_TRACK("getNumCameraOverlayInfos");
		return gGlobals->getNumCameraOverlayInfos();
	}
	DllExport CvCameraOverlayInfo& getCameraOverlayInfo(int i)
	{
		PROXY_TRACK("getCameraOverlayInfo");
		return gGlobals->getCameraOverlayInfo(i);
	}

	int getNumAnimationPathInfos()
	{
		PROXY_TRACK("getNumAnimationPathInfos");
		return gGlobals->getNumAnimationPathInfos();
	}
	DllExport CvAnimationPathInfo& getAnimationPathInfo(AnimationPathTypes e)
	{
		PROXY_TRACK("getAnimationPathInfo");
		return gGlobals->getAnimationPathInfo(e);
	}

	int getNumAnimationCategoryInfos()
	{
		PROXY_TRACK("getNumAnimationCategoryInfos");
		return gGlobals->getNumAnimationCategoryInfos();
	}
	DllExport CvAnimationCategoryInfo& getAnimationCategoryInfo(AnimationCategoryTypes e)
	{
		PROXY_TRACK("getAnimationCategoryInfo");
		return gGlobals->getAnimationCategoryInfo(e);
	}

	int getNumEntityEventInfos()
	{
		PROXY_TRACK("getNumEntityEventInfos");
		return gGlobals->getNumEntityEventInfos();
	}
	DllExport CvEntityEventInfo& getEntityEventInfo(EntityEventTypes e)
	{
		PROXY_TRACK("getEntityEventInfo");
		return gGlobals->getEntityEventInfo(e);
	}

	int getNumEffectInfos()
	{
		PROXY_TRACK("getNumEffectInfos");
		return gGlobals->getNumEffectInfos();
	}
	DllExport CvEffectInfo& getEffectInfo(int i)
	{
		PROXY_TRACK("getEffectInfo");
		return gGlobals->getEffectInfo(i);
	}

	int getNumAttachableInfos()
	{
		PROXY_TRACK("getNumAttachableInfos");
		return gGlobals->getNumAttachableInfos();
	}
	DllExport CvAttachableInfo& getAttachableInfo(int i)
	{
		PROXY_TRACK("getAttachableInfo");
		return gGlobals->getAttachableInfo(i);
	}

	int getNumCameraInfos()
	{
		PROXY_TRACK("getNumCameraInfos");
		return gGlobals->getNumCameraInfos();
	}
	CvCameraInfo& getCameraInfo(CameraAnimationTypes eCameraAnimationNum)
	{
		PROXY_TRACK("getCameraInfo");
		return gGlobals->getCameraInfo(eCameraAnimationNum);
	}

	DllExport int getNumUnitFormationInfos()
	{
		PROXY_TRACK("getNumUnitFormationInfos");
		return gGlobals->getNumUnitFormationInfos();
	}
	DllExport CvUnitFormationInfo& getUnitFormationInfo(int i)
	{
		PROXY_TRACK("getUnitFormationInfo");
		return gGlobals->getUnitFormationInfo(i);
	}

	int getNumLandscapeInfos()
	{
		PROXY_TRACK("getNumLandscapeInfos");
		return gGlobals->getNumLandscapeInfos();
	}
	DllExport CvLandscapeInfo& getLandscapeInfo(int iIndex)
	{
		PROXY_TRACK("getLandscapeInfo");
		return gGlobals->getLandscapeInfo(iIndex);
	}
	DllExport int getActiveLandscapeID()
	{
		PROXY_TRACK("getActiveLandscapeID");
		return gGlobals->getActiveLandscapeID();
	}
	DllExport void setActiveLandscapeID(int iLandscapeID)
	{
		PROXY_TRACK("setActiveLandscapeID");
		return gGlobals->setActiveLandscapeID(iLandscapeID);
	}

	DllExport int getNumTerrainInfos()
	{
		PROXY_TRACK("getNumTerrainInfos");
		return gGlobals->getNumTerrainInfos();
	}
	DllExport CvTerrainInfo& getTerrainInfo(TerrainTypes eTerrainNum)
	{
		PROXY_TRACK("getTerrainInfo");
		return gGlobals->getTerrainInfo(eTerrainNum);
	}

	DllExport int getNumBonusInfos()
	{
		PROXY_TRACK("getNumBonusInfos");
		return gGlobals->getNumBonusInfos();
	}
	DllExport CvBonusInfo& getBonusInfo(BonusTypes eBonusNum)
	{
		PROXY_TRACK("getBonusInfo");
		return gGlobals->getBonusInfo(eBonusNum);
	}

	DllExport int getNumFeatureInfos()
	{
		PROXY_TRACK("getNumFeatureInfos");
		return gGlobals->getNumFeatureInfos();
	}
	DllExport CvFeatureInfo& getFeatureInfo(FeatureTypes eFeatureNum)
	{
		PROXY_TRACK("getFeatureInfo");
		return gGlobals->getFeatureInfo(eFeatureNum);
	}

	DllExport int& getNumPlayableCivilizationInfos()
	{
		PROXY_TRACK("getNumPlayableCivilizationInfos");
		return gGlobals->getNumPlayableCivilizationInfos();
	}
	DllExport int& getNumAIPlayableCivilizationInfos()
	{
		PROXY_TRACK("getNumAIPlayableCivilizationInfos");
		return gGlobals->getNumAIPlayableCivilizationInfos();
	}
	DllExport int getNumCivilizationInfos()
	{
		PROXY_TRACK("getNumCivilizationInfos");
		return gGlobals->getNumCivilizationInfos();
	}
	DllExport CvCivilizationInfo& getCivilizationInfo(CivilizationTypes eCivilizationNum)
	{
		PROXY_TRACK("getCivilizationInfo");
		return gGlobals->getCivilizationInfo(eCivilizationNum);
	}

	DllExport int getNumLeaderHeadInfos()
	{
		PROXY_TRACK("getNumLeaderHeadInfos");
		return gGlobals->getNumLeaderHeadInfos();
	}
	DllExport CvLeaderHeadInfo& getLeaderHeadInfo(LeaderHeadTypes eLeaderHeadNum)
	{
		PROXY_TRACK("getLeaderHeadInfo");
		return gGlobals->getLeaderHeadInfo(eLeaderHeadNum);
	}

	DllExport int getNumCursorInfos()
	{
		PROXY_TRACK("getNumCursorInfos");
		return gGlobals->getNumCursorInfos();
	}
	DllExport	CvCursorInfo& getCursorInfo(CursorTypes eCursorNum)
	{
		PROXY_TRACK("getCursorInfo");
		return gGlobals->getCursorInfo(eCursorNum);
	}

	int getNumThroneRoomCameras()
	{
		PROXY_TRACK("getNumThroneRoomCameras");
		return gGlobals->getNumThroneRoomCameras();
	}
	DllExport	CvThroneRoomCamera& getThroneRoomCamera(int iIndex)
	{
		PROXY_TRACK("getThroneRoomCamera");
		return gGlobals->getThroneRoomCamera(iIndex);
	}

	DllExport int getNumThroneRoomInfos()
	{
		PROXY_TRACK("getNumThroneRoomInfos");
		return gGlobals->getNumThroneRoomInfos();
	}
	DllExport	CvThroneRoomInfo& getThroneRoomInfo(int iIndex)
	{
		PROXY_TRACK("getThroneRoomInfo");
		return gGlobals->getThroneRoomInfo(iIndex);
	}

	DllExport int getNumThroneRoomStyleInfos()
	{
		PROXY_TRACK("getNumThroneRoomStyleInfos");
		return gGlobals->getNumThroneRoomStyleInfos();
	}
	std::vector<CvThroneRoomStyleInfo*>& getThroneRoomStyleInfo()
	{
		PROXY_TRACK("getThroneRoomStyleInfo");
		return gGlobals->getThroneRoomStyleInfos();
	}
	DllExport	CvThroneRoomStyleInfo& getThroneRoomStyleInfo(int iIndex)
	{
		PROXY_TRACK("getThroneRoomStyleInfo");
		return gGlobals->getThroneRoomStyleInfo(iIndex);
	}

	DllExport int getNumSlideShowInfos()
	{
		PROXY_TRACK("getNumSlideShowInfos");
		return gGlobals->getNumSlideShowInfos();
	}
	DllExport	CvSlideShowInfo& getSlideShowInfo(int iIndex)
	{
		PROXY_TRACK("getSlideShowInfo");
		return gGlobals->getSlideShowInfo(iIndex);
	}

	DllExport int getNumSlideShowRandomInfos()
	{
		PROXY_TRACK("getNumSlideShowRandomInfos");
		return gGlobals->getNumSlideShowRandomInfos();
	}
	DllExport	CvSlideShowRandomInfo& getSlideShowRandomInfo(int iIndex)
	{
		PROXY_TRACK("getSlideShowRandomInfo");
		return gGlobals->getSlideShowRandomInfo(iIndex);
	}

	DllExport int getNumWorldPickerInfos()
	{
		PROXY_TRACK("getNumWorldPickerInfos");
		return gGlobals->getNumWorldPickerInfos();
	}
	DllExport	CvWorldPickerInfo& getWorldPickerInfo(int iIndex)
	{
		PROXY_TRACK("getWorldPickerInfo");
		return gGlobals->getWorldPickerInfo(iIndex);
	}

	DllExport int getNumSpaceShipInfos()
	{
		PROXY_TRACK("getNumSpaceShipInfos");
		return gGlobals->getNumSpaceShipInfos();
	}
	DllExport	CvSpaceShipInfo& getSpaceShipInfo(int iIndex)
	{
		PROXY_TRACK("getSpaceShipInfo");
		return gGlobals->getSpaceShipInfo(iIndex);
	}

	int getNumGameOptionInfos()
	{
		PROXY_TRACK("getNumGameOptionInfos");
		return gGlobals->getNumGameOptionInfos();
	}
	DllExport	CvGameOptionInfo& getGameOptionInfo(GameOptionTypes eGameOptionNum)
	{
		PROXY_TRACK("getGameOptionInfo");
		return gGlobals->getGameOptionInfo(eGameOptionNum);
	}

	int getNumMPOptionInfos()
	{
		PROXY_TRACK("getNumMPOptionInfos");
		return gGlobals->getNumMPOptionInfos();
	}
	DllExport	CvMPOptionInfo& getMPOptionInfo(MultiplayerOptionTypes eMPOptionNum)
	{
		PROXY_TRACK("getMPOptionInfo");
		return gGlobals->getMPOptionInfo(eMPOptionNum);
	}

	int getNumForceControlInfos()
	{
		PROXY_TRACK("getNumForceControlInfos");
		return gGlobals->getNumForceControlInfos();
	}
	CvForceControlInfo& getForceControlInfo(ForceControlTypes eForceControlNum)
	{
		PROXY_TRACK("getForceControlInfo");
		return gGlobals->getForceControlInfo(eForceControlNum);
	}

	DllExport	CvPlayerOptionInfo& getPlayerOptionInfo(PlayerOptionTypes ePlayerOptionNum)
	{
		PROXY_TRACK("getPlayerOptionInfo");
		return gGlobals->getPlayerOptionInfo(ePlayerOptionNum);
	}

	DllExport	CvGraphicOptionInfo& getGraphicOptionInfo(GraphicOptionTypes eGraphicOptionNum)
	{
		PROXY_TRACK("getGraphicOptionInfo");
		return gGlobals->getGraphicOptionInfo(eGraphicOptionNum);
	}

	DllExport int getNumRouteInfos()
	{
		PROXY_TRACK("getNumRouteInfos");
		return gGlobals->getNumRouteInfos();
	}
	CvRouteInfo& getRouteInfo(RouteTypes eRouteNum)
	{
		PROXY_TRACK("getRouteInfo");
		return gGlobals->getRouteInfo(eRouteNum);
	}

	DllExport int getNumImprovementInfos()
	{
		PROXY_TRACK("getNumImprovementInfos");
		return gGlobals->getNumImprovementInfos();
	}
	DllExport CvImprovementInfo& getImprovementInfo(ImprovementTypes eImprovementNum)
	{
		PROXY_TRACK("getImprovementInfo");
		return gGlobals->getImprovementInfo(eImprovementNum);
	}

	int getNumGoodyInfos()
	{
		PROXY_TRACK("getNumGoodyInfos");
		return gGlobals->getNumGoodyInfos();
	}
	CvGoodyInfo& getGoodyInfo(GoodyTypes eGoodyNum)
	{
		PROXY_TRACK("getGoodyInfo");
		return gGlobals->getGoodyInfo(eGoodyNum);
	}

	int getNumBuildInfos()
	{
		PROXY_TRACK("getNumBuildInfos");
		return gGlobals->getNumBuildInfos();
	}
	DllExport CvBuildInfo& getBuildInfo(BuildTypes eBuildNum)
	{
		PROXY_TRACK("getBuildInfo");
		return gGlobals->getBuildInfo(eBuildNum);
	}

	DllExport int getNumHandicapInfos()
	{
		PROXY_TRACK("getNumHandicapInfos");
		return gGlobals->getNumHandicapInfos();
	}
	DllExport CvHandicapInfo& getHandicapInfo(HandicapTypes eHandicapNum)
	{
		PROXY_TRACK("getHandicapInfo");
		return gGlobals->getHandicapInfo(eHandicapNum);
	}

	DllExport int getNumGameSpeedInfos()
	{
		PROXY_TRACK("getNumGameSpeedInfos");
		return gGlobals->getNumGameSpeedInfos();
	}
	DllExport CvGameSpeedInfo& getGameSpeedInfo(GameSpeedTypes eGameSpeedNum)
	{
		PROXY_TRACK("getGameSpeedInfo");
		return gGlobals->getGameSpeedInfo(eGameSpeedNum);
	}

	DllExport int getNumTurnTimerInfos()
	{
		PROXY_TRACK("getNumTurnTimerInfos");
		return gGlobals->getNumTurnTimerInfos();
	}
	DllExport CvTurnTimerInfo& getTurnTimerInfo(TurnTimerTypes eTurnTimerNum)
	{
		PROXY_TRACK("getTurnTimerInfo");
		return gGlobals->getTurnTimerInfo(eTurnTimerNum);
	}

	DllExport int getNumActionInfos()
	{
		PROXY_TRACK("getNumActionInfos");
		return gGlobals->getNumActionInfos();
	}
	DllExport CvActionInfo& getActionInfo(int i)
	{
		PROXY_TRACK("getActionInfo");
		return gGlobals->getActionInfo(i);
	}

	DllExport CvMissionInfo& getMissionInfo(MissionTypes eMissionNum)
	{
		PROXY_TRACK("getMissionInfo");
		return gGlobals->getMissionInfo(eMissionNum);
	}

	CvControlInfo& getControlInfo(ControlTypes eControlNum)
	{
		PROXY_TRACK("getControlInfo");
		return gGlobals->getControlInfo(eControlNum);
	}

	CvCommandInfo& getCommandInfo(CommandTypes eCommandNum)
	{
		PROXY_TRACK("getCommandInfo");
		return gGlobals->getCommandInfo(eCommandNum);
	}

	int getNumAutomateInfos()
	{
		PROXY_TRACK("getNumAutomateInfos");
		return gGlobals->getNumAutomateInfos();
	}
	CvAutomateInfo& getAutomateInfo(int iAutomateNum)
	{
		PROXY_TRACK("getAutomateInfo");
		return gGlobals->getAutomateInfo(iAutomateNum);
	}

	DllExport int getNumEraInfos()
	{
		PROXY_TRACK("getNumEraInfos");
		return gGlobals->getNumEraInfos();
	}
	DllExport CvEraInfo& getEraInfo(EraTypes eEraNum)
	{
		PROXY_TRACK("getEraInfo");
		return gGlobals->getEraInfo(eEraNum);
	}

	DllExport int getNumVictoryInfos()
	{
		PROXY_TRACK("getNumVictoryInfos");
		return gGlobals->getNumVictoryInfos();
	}
	DllExport CvVictoryInfo& getVictoryInfo(VictoryTypes eVictoryNum)
	{
		PROXY_TRACK("getVictoryInfo");
		return gGlobals->getVictoryInfo(eVictoryNum);
	}

	//
	// Global Types
	// All type strings are upper case and are kept in this hash map for fast lookup
	// The other functions are kept for convenience when enumerating, but most are not used
	//
	DllExport int getTypesEnum(const char* szType) const				// use this when searching for a type
	{
		PROXY_TRACK("getTypesEnum");
		return gGlobals->getTypesEnum(szType);
	}
	void setTypesEnum(const char* szType, int iEnum)
	{
		PROXY_TRACK("setTypesEnum");
		gGlobals->setTypesEnum(szType, iEnum);
	}

	DllExport int getNUM_ENGINE_DIRTY_BITS() const
	{
		PROXY_TRACK("getNUM_ENGINE_DIRTY_BITS");
		return gGlobals->getNUM_ENGINE_DIRTY_BITS();
	}
	DllExport int getNUM_INTERFACE_DIRTY_BITS() const
	{
		PROXY_TRACK("getNUM_INTERFACE_DIRTY_BITS");
		return gGlobals->getNUM_INTERFACE_DIRTY_BITS();
	}
	DllExport int getNUM_YIELD_TYPES() const
	{
		PROXY_TRACK("getNUM_YIELD_TYPES");
		return gGlobals->getNUM_YIELD_TYPES();
	}
	int getNUM_COMMERCE_TYPES() const
	{
		PROXY_TRACK("getNUM_COMMERCE_TYPES");
		return gGlobals->getNUM_COMMERCE_TYPES();
	}
	DllExport int getNUM_FORCECONTROL_TYPES() const
	{
		PROXY_TRACK("getNUM_FORCECONTROL_TYPES");
		return gGlobals->getNUM_FORCECONTROL_TYPES();
	}
	DllExport int getNUM_INFOBAR_TYPES() const
	{
		PROXY_TRACK("getNUM_INFOBAR_TYPES");
		return gGlobals->getNUM_INFOBAR_TYPES();
	}
	DllExport int getNUM_HEALTHBAR_TYPES() const
	{
		PROXY_TRACK("getNUM_HEALTHBAR_TYPES");
		return gGlobals->getNUM_HEALTHBAR_TYPES();
	}
	int getNUM_CONTROL_TYPES() const
	{
		PROXY_TRACK("getNUM_CONTROL_TYPES");
		return gGlobals->getNUM_CONTROL_TYPES();
	}
	DllExport int getNUM_LEADERANIM_TYPES() const
	{
		PROXY_TRACK("getNUM_LEADERANIM_TYPES");
		return gGlobals->getNUM_LEADERANIM_TYPES();
	}

	int& getNumEntityEventTypes()
	{
		PROXY_TRACK("getNumEntityEventTypes");
		return gGlobals->getNumEntityEventTypes();
	}
	CvString& getEntityEventTypes(EntityEventTypes e)
	{
		PROXY_TRACK("getEntityEventTypes");
		return gGlobals->getEntityEventTypes(e);
	}

	int& getNumAnimationOperatorTypes()
	{
		PROXY_TRACK("getNumAnimationOperatorTypes");
		return gGlobals->getNumAnimationOperatorTypes();
	}
	CvString& getAnimationOperatorTypes(AnimationOperatorTypes e)
	{
		PROXY_TRACK("getAnimationOperatorTypes");
		return gGlobals->getAnimationOperatorTypes(e);
	}

	CvString& getFunctionTypes(FunctionTypes e)
	{
		PROXY_TRACK("getFunctionTypes");
		return gGlobals->getFunctionTypes(e);
	}

	DllExport int& getNumArtStyleTypes()
	{
		PROXY_TRACK("getNumArtStyleTypes");
		return gGlobals->getNumArtStyleTypes();
	}
	DllExport CvString& getArtStyleTypes(ArtStyleTypes e)
	{
		PROXY_TRACK("getArtStyleTypes");
		return gGlobals->getArtStyleTypes(e);
	}

	CvString& getDirectionTypes(AutomateTypes e)
	{
		PROXY_TRACK("getDirectionTypes");
		return gGlobals->getDirectionTypes(e);
	}

	DllExport int& getNumFootstepAudioTypes()
	{
		PROXY_TRACK("getNumFootstepAudioTypes");
		return gGlobals->getNumFootstepAudioTypes();
	}
	CvString& getFootstepAudioTypes(int i)
	{
		PROXY_TRACK("getFootstepAudioTypes");
		return gGlobals->getFootstepAudioTypes(i);
	}
	int getFootstepAudioTypeByTag(CvString strTag)
	{
		PROXY_TRACK("getFootstepAudioTypeByTag");
		return gGlobals->getFootstepAudioTypeByTag(strTag);
	}

	DllExport CvString& getFootstepAudioTags(int i)
	{
		PROXY_TRACK("getFootstepAudioTags");
		return gGlobals->getFootstepAudioTags(i);
	}

	//
	///////////////// BEGIN global defines
	// THESE ARE READ-ONLY
	//

	DllExport FVariableSystem* getDefinesVarSystem()
	{
		PROXY_TRACK("getDefinesVarSystem");
		return gGlobals->getDefinesVarSystem();
	}
	void cacheGlobals()
	{
		PROXY_TRACK("cacheGlobals");
		gGlobals->cacheGlobals();
	}

	// ***** EXPOSED TO PYTHON *****
/************************************************************************************************/
/* MOD_COMPONENT_CONTROL                   08/02/07                            MRGENIE          */
/*                                                                                              */
/* Return true/false from                                                                       */
/************************************************************************************************/
	bool getDefineBOOL( const char * szName ) const
	{
		PROXY_TRACK("getDefineBOOL");
		return gGlobals->getDefineBOOL(szName);
	}
/************************************************************************************************/
/* MOD_COMPONENT_CONTROL                   END                                                  */
/************************************************************************************************/

	DllExport int getDefineINT( const char * szName ) const
	{
		PROXY_TRACK("getDefineINT");
		return gGlobals->getDefineINT(szName);
	}
	DllExport float getDefineFLOAT( const char * szName ) const
	{
		PROXY_TRACK("getDefineFLOAT");
		return gGlobals->getDefineFLOAT(szName);
	}
	DllExport const char * getDefineSTRING( const char * szName ) const
	{
		PROXY_TRACK("getDefineSTRING");
		return gGlobals->getDefineSTRING(szName);
	}
/************************************************************************************************/
/* Afforess	                  Start		 08/18/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	void setDefineINT( const char * szName, int iValue, bool bUpdate = true)
	{
		PROXY_TRACK("setDefineINT");
		gGlobals->setDefineINT(szName, iValue, bUpdate);
	}
	void setDefineFLOAT( const char * szName, float fValue, bool bUpdate = true )
	{
		PROXY_TRACK("setDefineFLOAT");
		gGlobals->setDefineFLOAT(szName, fValue, bUpdate);
	}
	void setDefineSTRING( const char * szName, const char * szValue, bool bUpdate = true )
	{
		PROXY_TRACK("setDefineSTRING");
		gGlobals->setDefineSTRING(szName, szValue, bUpdate);
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	DllExport int getMAX_PLOT_LIST_ROWS()
	{
		PROXY_TRACK("getMAX_PLOT_LIST_ROWS");
		return gGlobals->getMAX_PLOT_LIST_ROWS();
	}
	DllExport int getUNIT_MULTISELECT_MAX()
	{
		PROXY_TRACK("getUNIT_MULTISELECT_MAX");
		return gGlobals->getUNIT_MULTISELECT_MAX();
	}
	DllExport int getEVENT_MESSAGE_TIME()
	{
		PROXY_TRACK("getEVENT_MESSAGE_TIME");
		return gGlobals->getEVENT_MESSAGE_TIME();
	}

	DllExport float getCAMERA_MIN_YAW()
	{
		PROXY_TRACK("getCAMERA_MIN_YAW");
		return gGlobals->getCAMERA_MIN_YAW();
	}
	DllExport float getCAMERA_MAX_YAW()
	{
		PROXY_TRACK("getCAMERA_MAX_YAW");
		return gGlobals->getCAMERA_MAX_YAW();
	}
	DllExport float getCAMERA_FAR_CLIP_Z_HEIGHT()
	{
		PROXY_TRACK("getCAMERA_FAR_CLIP_Z_HEIGHT");
		return gGlobals->getCAMERA_FAR_CLIP_Z_HEIGHT();
	}
	DllExport float getCAMERA_MAX_TRAVEL_DISTANCE()
	{
		PROXY_TRACK("getCAMERA_MAX_TRAVEL_DISTANCE");
		return gGlobals->getCAMERA_MAX_TRAVEL_DISTANCE();
	}
	DllExport float getCAMERA_START_DISTANCE()
	{
		PROXY_TRACK("getCAMERA_START_DISTANCE");
		return gGlobals->getCAMERA_START_DISTANCE();
	}
	DllExport float getAIR_BOMB_HEIGHT()
	{
		PROXY_TRACK("getAIR_BOMB_HEIGHT");
		return gGlobals->getAIR_BOMB_HEIGHT();
	}
	DllExport float getPLOT_SIZE()
	{
		PROXY_TRACK("getPLOT_SIZE");
		return gGlobals->getPLOT_SIZE();
	}
	DllExport float getCAMERA_SPECIAL_PITCH()
	{
		PROXY_TRACK("getCAMERA_SPECIAL_PITCH");
		return gGlobals->getCAMERA_SPECIAL_PITCH();
	}
	DllExport float getCAMERA_MAX_TURN_OFFSET()
	{
		PROXY_TRACK("getCAMERA_MAX_TURN_OFFSET");
		return gGlobals->getCAMERA_MAX_TURN_OFFSET();
	}
	DllExport float getCAMERA_MIN_DISTANCE()
	{
		PROXY_TRACK("getCAMERA_MIN_DISTANCE");
		return gGlobals->getCAMERA_MIN_DISTANCE();
	}
	DllExport float getCAMERA_UPPER_PITCH()
	{
		PROXY_TRACK("getCAMERA_UPPER_PITCH");
		return gGlobals->getCAMERA_UPPER_PITCH();
	}
	DllExport float getCAMERA_LOWER_PITCH()
	{
		PROXY_TRACK("getCAMERA_LOWER_PITCH");
		return gGlobals->getCAMERA_LOWER_PITCH();
	}
	DllExport float getFIELD_OF_VIEW()
	{
		PROXY_TRACK("getFIELD_OF_VIEW");
		return gGlobals->getFIELD_OF_VIEW();
	}
	DllExport float getSHADOW_SCALE()
	{
		PROXY_TRACK("getSHADOW_SCALE");
		return gGlobals->getSHADOW_SCALE();
	}
	DllExport float getUNIT_MULTISELECT_DISTANCE()
	{
		PROXY_TRACK("getUNIT_MULTISELECT_DISTANCE");
		return gGlobals->getUNIT_MULTISELECT_DISTANCE();
	}

	DllExport int getUSE_FINISH_TEXT_CALLBACK()
	{
		PROXY_TRACK("getUSE_FINISH_TEXT_CALLBACK");
		return gGlobals->getUSE_FINISH_TEXT_CALLBACK();
	}

	DllExport int getMAX_CIV_PLAYERS()
	{
		PROXY_TRACK("getMAX_CIV_PLAYERS");
		return gGlobals->getMAX_CIV_PLAYERS();
	}

	int getMAX_PC_PLAYERS()
	{
		PROXY_TRACK("getMAX_PC_PLAYERS");
		return gGlobals->getMAX_PC_PLAYERS();
	}

	int getMAX_PLAYERS()
	{
		PROXY_TRACK("getMAX_PLAYERS");
		return gGlobals->getMAX_PLAYERS();
	}
	int getMAX_PC_TEAMS()
	{
		PROXY_TRACK("getMAX_PC_TEAMS");
		return gGlobals->getMAX_PC_TEAMS();
	}
	int getMAX_TEAMS()
	{
		PROXY_TRACK("getMAX_TEAMS");
		return gGlobals->getMAX_TEAMS();
	}

	int getBARBARIAN_PLAYER()
	{
		PROXY_TRACK("getBARBARIAN_PLAYER");
		return gGlobals->getBARBARIAN_PLAYER();
	}
	int getBARBARIAN_TEAM()
	{
		PROXY_TRACK("getBARBARIAN_TEAM");
		return gGlobals->getBARBARIAN_TEAM();
	}

	int getNEANDERTHAL_PLAYER()
	{
		PROXY_TRACK("getNEANDERTHAL_PLAYER");
		return gGlobals->getNEANDERTHAL_PLAYER();
	}
	int getNEANDERTHAL_TEAM()
	{
		PROXY_TRACK("getNEANDERTHAL_TEAM");
		return gGlobals->getNEANDERTHAL_TEAM();
	}

	int getBEAST_PLAYER()
	{
		PROXY_TRACK("getBEAST_PLAYER");
		return gGlobals->getBEAST_PLAYER();
	}
	int getBEAST_TEAM()
	{
		PROXY_TRACK("getBEAST_TEAM");
		return gGlobals->getBEAST_TEAM();
	}

	int getPREDATOR_PLAYER()
	{
		PROXY_TRACK("getPREDATOR_PLAYER");
		return gGlobals->getPREDATOR_PLAYER();
	}
	int getPREDATOR_TEAM()
	{
		PROXY_TRACK("getPREDATOR_TEAM");
		return gGlobals->getPREDATOR_TEAM();
	}

	int getPREY_PLAYER()
	{
		PROXY_TRACK("getPREY_PLAYER");
		return gGlobals->getPREY_PLAYER();
	}
	int getPREY_TEAM()
	{
		PROXY_TRACK("getPREY_TEAM");
		return gGlobals->getPREY_TEAM();
	}

	int getINSECT_PLAYER()
	{
		PROXY_TRACK("getINSECT_PLAYER");
		return gGlobals->getINSECT_PLAYER();
	}
	int getINSECT_TEAM()
	{
		PROXY_TRACK("getINSECT_TEAM");
		return gGlobals->getINSECT_TEAM();
	}

	int getNPC4_PLAYER()
	{
		PROXY_TRACK("getNPC4_PLAYER");
		return gGlobals->getNPC4_PLAYER();
	}
	int getNPC4_TEAM()
	{
		PROXY_TRACK("getNPC4_TEAM");
		return gGlobals->getNPC4_TEAM();
	}

	int getNPC3_PLAYER()
	{
		PROXY_TRACK("getNPC3_PLAYER");
		return gGlobals->getNPC3_PLAYER();
	}
	int getNPC3_TEAM()
	{
		PROXY_TRACK("getNPC3_TEAM");
		return gGlobals->getNPC3_TEAM();
	}

	int getNPC2_PLAYER()
	{
		PROXY_TRACK("getNPC2_PLAYER");
		return gGlobals->getNPC2_PLAYER();
	}
	int getNPC2_TEAM()
	{
		PROXY_TRACK("getNPC2_TEAM");
		return gGlobals->getNPC2_TEAM();
	}

	int getNPC1_PLAYER()
	{
		PROXY_TRACK("getNPC1_PLAYER");
		return gGlobals->getNPC1_PLAYER();
	}
	int getNPC1_TEAM()
	{
		PROXY_TRACK("getNPC1_TEAM");
		return gGlobals->getNPC1_TEAM();
	}

	int getNPC0_PLAYER()
	{
		PROXY_TRACK("getNPC0_PLAYER");
		return gGlobals->getNPC0_PLAYER();
	}
	int getNPC0_TEAM()
	{
		PROXY_TRACK("getNPC0_TEAM");
		return gGlobals->getNPC0_TEAM();
	}

	int getINVALID_PLOT_COORD()
	{
		PROXY_TRACK("getINVALID_PLOT_COORD");
		return gGlobals->getINVALID_PLOT_COORD();
	}
	int getNUM_CITY_PLOTS()
	{
		PROXY_TRACK("getNUM_CITY_PLOTS");
		return gGlobals->getNUM_CITY_PLOTS();
	}
	int getCITY_HOME_PLOT()
	{
		PROXY_TRACK("getCITY_HOME_PLOT");
		return gGlobals->getCITY_HOME_PLOT();
	}

	// ***** END EXPOSED TO PYTHON *****

	////////////// END DEFINES //////////////////

	DllExport void setDLLIFace(CvDLLUtilityIFaceBase* pDll)
	{
		if ( pDll != NULL )
		{
			FAssertMsg(g_DLL == NULL, "Dll interface already set?");

			OutputDebugString("setDLLIFace()\n");
			if (gGlobals == NULL)
			{
				OutputDebugString("Constructing internal globals\n");
				gGlobals = new cvInternalGlobals();
			}

			g_DLL = pDll;
		}
		else
		{
			FAssertMsg(g_DLL != NULL, "Dll interface not set?");
			g_DLL = NULL;
			delete gGlobals;
		}
	}

	DllExport CvDLLUtilityIFaceBase* getDLLIFaceNonInl()
	{
		//PROXY_TRACK("getDLLIFaceNonInl");
		return gGlobals->getDLLIFaceNonInl();
	}
	DllExport void setDLLProfiler(FProfiler* prof)
	{
		PROXY_TRACK("setDLLProfiler");
		gGlobals->setDLLProfiler(prof);
	}
	DllExport void enableDLLProfiler(bool bEnable)
	{
		PROXY_TRACK("enableDLLProfiler");
		gGlobals->enableDLLProfiler(bEnable);
	}

	DllExport bool IsGraphicsInitialized() const
	{
		PROXY_TRACK("IsGraphicsInitialized");
		return gGlobals->IsGraphicsInitialized();
	}
	DllExport void SetGraphicsInitialized(bool bVal)
	{
		PROXY_TRACK("SetGraphicsInitialized");
		gGlobals->SetGraphicsInitialized(bVal);
	}

	// for caching
	DllExport bool readBuildingInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("readBuildingInfoArray");
		return gGlobals->readBuildingInfoArray(pStream);
	}
	DllExport void writeBuildingInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("writeBuildingInfoArray");
		gGlobals->writeBuildingInfoArray(pStream);
	}

	DllExport bool readTechInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("readTechInfoArray");
		return gGlobals->readTechInfoArray(pStream);
	}
	DllExport void writeTechInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("writeTechInfoArray");
		gGlobals->writeTechInfoArray(pStream);
	}

	DllExport bool readUnitInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("readUnitInfoArray");
		return gGlobals->readUnitInfoArray(pStream);
	}
	DllExport void writeUnitInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("writeUnitInfoArray");
		gGlobals->writeUnitInfoArray(pStream);
	}

	DllExport bool readLeaderHeadInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("readLeaderHeadInfoArray");
		return gGlobals->readLeaderHeadInfoArray(pStream);
	}
	DllExport void writeLeaderHeadInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("writeLeaderHeadInfoArray");
		gGlobals->writeLeaderHeadInfoArray(pStream);
	}

	DllExport bool readCivilizationInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("readCivilizationInfoArray");
		return gGlobals->readCivilizationInfoArray(pStream);
	}
	DllExport void writeCivilizationInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("writeCivilizationInfoArray");
		gGlobals->writeCivilizationInfoArray(pStream);
	}

	DllExport bool readPromotionInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("readPromotionInfoArray");
		return gGlobals->readPromotionInfoArray(pStream);
	}
	DllExport void writePromotionInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("writePromotionInfoArray");
		gGlobals->writePromotionInfoArray(pStream);
	}

	DllExport bool readDiplomacyInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("readDiplomacyInfoArray");
		return gGlobals->readDiplomacyInfoArray(pStream);
	}
	DllExport void writeDiplomacyInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("writeDiplomacyInfoArray");
		gGlobals->writeDiplomacyInfoArray(pStream);
	}

	DllExport bool readCivicInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("readCivicInfoArray");
		return gGlobals->readCivicInfoArray(pStream);
	}
	DllExport void writeCivicInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("writeCivicInfoArray");
		gGlobals->writeCivicInfoArray(pStream);
	}

	DllExport bool readHandicapInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("readHandicapInfoArray");
		return gGlobals->readHandicapInfoArray(pStream);
	}
	DllExport void writeHandicapInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("writeHandicapInfoArray");
		gGlobals->writeHandicapInfoArray(pStream);
	}

	DllExport bool readBonusInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("readBonusInfoArray");
		return gGlobals->readBonusInfoArray(pStream);
	}
	DllExport void writeBonusInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("writeBonusInfoArray");
		gGlobals->writeBonusInfoArray(pStream);
	}

	DllExport bool readImprovementInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("readImprovementInfoArray");
		return gGlobals->readImprovementInfoArray(pStream);
	}
	DllExport void writeImprovementInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("writeImprovementInfoArray");
		gGlobals->writeImprovementInfoArray(pStream);
	}

	DllExport bool readEventInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("readEventInfoArray");
		return gGlobals->readEventInfoArray(pStream);
	}
	DllExport void writeEventInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("writeEventInfoArray");
		gGlobals->writeEventInfoArray(pStream);
	}

	DllExport bool readEventTriggerInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("readEventTriggerInfoArray");
		return gGlobals->readEventTriggerInfoArray(pStream);
	}
	DllExport void writeEventTriggerInfoArray(FDataStreamBase* pStream)
	{
		PROXY_TRACK("writeEventTriggerInfoArray");
		gGlobals->writeEventTriggerInfoArray(pStream);
	}

	//
	// additional accessors for initting globals
	//

	DllExport void setInterface(CvInterface* pVal)
	{
		PROXY_TRACK("setInterface");
		gGlobals->setInterface(pVal);
	}
	DllExport void setDiplomacyScreen(CvDiplomacyScreen* pVal)
	{
		PROXY_TRACK("setDiplomacyScreen");
		gGlobals->setDiplomacyScreen(pVal);
	}
	DllExport void setMPDiplomacyScreen(CMPDiplomacyScreen* pVal)
	{
		PROXY_TRACK("setMPDiplomacyScreen");
		gGlobals->setMPDiplomacyScreen(pVal);
	}
	DllExport void setMessageQueue(CMessageQueue* pVal)
	{
		PROXY_TRACK("setMessageQueue");
		gGlobals->setMessageQueue(pVal);
	}
	DllExport void setHotJoinMessageQueue(CMessageQueue* pVal)
	{
		PROXY_TRACK("setHotJoinMessageQueue");
		gGlobals->setHotJoinMessageQueue(pVal);
	}
	DllExport void setMessageControl(CMessageControl* pVal)
	{
		PROXY_TRACK("setMessageControl");
		gGlobals->setMessageControl(pVal);
	}
	DllExport void setSetupData(CvSetupData* pVal)
	{
		PROXY_TRACK("setSetupData");
		gGlobals->setSetupData(pVal);
	}
	DllExport void setMessageCodeTranslator(CvMessageCodeTranslator* pVal)
	{
		PROXY_TRACK("setMessageCodeTranslator");
		gGlobals->setMessageCodeTranslator(pVal);
	}
	DllExport void setDropMgr(CvDropMgr* pVal)
	{
		PROXY_TRACK("setDropMgr");
		gGlobals->setDropMgr(pVal);
	}
	DllExport void setPortal(CvPortal* pVal)
	{
		PROXY_TRACK("setPortal");
		gGlobals->setPortal(pVal);
	}
	DllExport void setStatsReport(CvStatsReporter* pVal)
	{
		PROXY_TRACK("setStatsReport");
		gGlobals->setStatsReport(pVal);
	}
	DllExport void setPathFinder(FAStar* pVal)
	{
		PROXY_TRACK("setPathFinder");
		gGlobals->setPathFinder(pVal);
	}
	DllExport void setInterfacePathFinder(FAStar* pVal)
	{
		PROXY_TRACK("setInterfacePathFinder");
		gGlobals->setInterfacePathFinder(pVal);
	}
	DllExport void setStepFinder(FAStar* pVal)
	{
		PROXY_TRACK("setStepFinder");
		gGlobals->setStepFinder(pVal);
	}
	DllExport void setRouteFinder(FAStar* pVal)
	{
		PROXY_TRACK("setRouteFinder");
		gGlobals->setRouteFinder(pVal);
	}
	DllExport void setBorderFinder(FAStar* pVal)
	{
		PROXY_TRACK("setBorderFinder");
		gGlobals->setBorderFinder(pVal);
	}
	DllExport void setAreaFinder(FAStar* pVal)
	{
		PROXY_TRACK("setAreaFinder");
		gGlobals->setAreaFinder(pVal);
	}
	DllExport void setPlotGroupFinder(FAStar* pVal)
	{
		PROXY_TRACK("setPlotGroupFinder");
		gGlobals->setPlotGroupFinder(pVal);
	}

	// So that CvEnums are moddable in the DLL
	DllExport int getNumDirections() const
	{
		PROXY_TRACK("getNumDirections");
		return gGlobals->getNumDirections();
	}
	DllExport int getNumGameOptions() const
	{
		PROXY_TRACK("getNumGameOptions");
		return gGlobals->getNumGameOptions();
	}
	DllExport int getNumMPOptions() const
	{
		PROXY_TRACK("getNumMPOptions");
		return gGlobals->getNumMPOptions();
	}
	DllExport int getNumSpecialOptions() const
	{
		PROXY_TRACK("getNumSpecialOptions");
		return gGlobals->getNumSpecialOptions();
	}
	DllExport int getNumGraphicOptions() const
	{
		PROXY_TRACK("getNumGraphicOptions");
		return gGlobals->getNumGraphicOptions();
	}
	DllExport int getNumTradeableItems() const
	{
		PROXY_TRACK("getNumTradeableItems");
		return gGlobals->getNumTradeableItems();
	}
	DllExport int getNumBasicItems() const
	{
		PROXY_TRACK("getNumBasicItems");
		return gGlobals->getNumBasicItems();
	}
	DllExport int getNumTradeableHeadings() const
	{
		PROXY_TRACK("getNumTradeableHeadings");
		return gGlobals->getNumTradeableHeadings();
	}
	int getNumCommandInfos() const
	{
		PROXY_TRACK("getNumCommandInfos");
		return gGlobals->getNumCommandInfos();
	}
	int getNumControlInfos() const
	{
		PROXY_TRACK("getNumControlInfos");
		return gGlobals->getNumControlInfos();
	}
	int getNumMissionInfos() const
	{
		PROXY_TRACK("getNumMissionInfos");
		return gGlobals->getNumMissionInfos();
	}
	DllExport int getNumPlayerOptionInfos() const
	{
		PROXY_TRACK("getNumPlayerOptionInfos");
		return gGlobals->getNumPlayerOptionInfos();
	}
	DllExport int getMaxNumSymbols() const
	{
		PROXY_TRACK("getMaxNumSymbols");
		return gGlobals->getMaxNumSymbols();
	}
	DllExport int getNumGraphicLevels() const
	{
		PROXY_TRACK("getNumGraphicLevels");
		return gGlobals->getNumGraphicLevels();
	}
	int getNumGlobeLayers() const
	{
		PROXY_TRACK("getNumGlobeLayers");
		return gGlobals->getNumGlobeLayers();
	}
};

extern CvGlobals gGlobalsProxy;	// for debugging

//
// inlines
//
inline cvInternalGlobals& cvInternalGlobals::getInstance()
{
	// AIAndy: This function is called so often, it is not worth it to do this safety check on something that will not happen with the code changes we do nowadays
	//if ( gGlobals == NULL )
	//{
	//	::MessageBoxA(NULL, "cvInternalGlobals::getInstance() called prior to instantiation\n","CvGameCore",MB_OK);
	//}
	return *gGlobals;
}

inline CvGlobals& CvGlobals::getInstance()
{
	return gGlobalsProxy;
}

//
// helpers
//
#define GC cvInternalGlobals::getInstance()
#define gDLL g_DLL

#ifndef _USRDLL
#define NUM_DIRECTION_TYPES (GC.getNumDirections())
#define NUM_GAMEOPTION_TYPES (GC.getNumGameOptions())
#define NUM_MPOPTION_TYPES (GC.getNumMPOptions())
#define NUM_SPECIALOPTION_TYPES (GC.getNumSpecialOptions())
#define NUM_GRAPHICOPTION_TYPES (GC.getNumGraphicOptions())
#define NUM_TRADEABLE_ITEMS (GC.getNumTradeableItems())
#define NUM_BASIC_ITEMS (GC.getNumBasicItems())
#define NUM_TRADEABLE_HEADINGS (GC.getNumTradeableHeadings())
#define NUM_COMMAND_TYPES (GC.getNumCommandInfos())
#define NUM_CONTROL_TYPES (GC.getNumControlInfos())
#define NUM_PLAYEROPTION_TYPES (GC.getNumPlayerOptionInfos())
#define MAX_NUM_SYMBOLS (GC.getMaxNumSymbols())
#define NUM_GRAPHICLEVELS (GC.getNumGraphicLevels())
#define NUM_GLOBE_LAYER_TYPES (GC.getNumGlobeLayers())
#endif

#ifndef FIXED_MISSION_NUMBER
#define NUM_MISSION_TYPES (GC.getNumMissionInfos())
#endif

#endif

/**********************************************************************

File:		BugMod.h
Author:		EmperorFool
Created:	2009-01-22

Defines common constants and functions for use throughout the BUG Mod.

		Copyright (c) 2009 The BUG Mod. All rights reserved.

**********************************************************************/

#pragma once

#ifndef BUG_MOD_H
#define BUG_MOD_H

// name of the Python module where all the BUG functions that the DLL calls must live
// MUST BE A BUILT-IN MODULE IN THE ENTRYPOINTS FOLDER
// currently CvAppInterface
#define PYBugModule				PYCivModule

// Increment this by 1 each time you commit new/changed functions/constants in the Python API.
#define BUG_DLL_API_VERSION		6

// Used to signal the BULL saved game format is used
#define BUG_DLL_SAVE_FORMAT		64

// These are display-only values, and the version should be changed for each release.
#define BUG_DLL_NAME			L"BULL"
#define BUG_DLL_VERSION			L"1.3"
#define BUG_DLL_BUILD			L"219"

#endif