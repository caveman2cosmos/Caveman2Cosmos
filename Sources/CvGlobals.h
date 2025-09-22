#pragma once

// cvGlobals.h
//ReplacementStep: search down here for 'CvInfoReplacements'

#ifndef CIV4_GLOBALS_H
#define CIV4_GLOBALS_H

#include "FProfiler.h"

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
class CvDiplomacyScreen;
class CMPDiplomacyScreen;
class FMPIManager;
class FAStar;
class CvInterface;
class FVariableSystem;
class CvMap;
class CvMapExternal;
class CvViewport;
class CvInterfaceModeInfo;
class CvWorldInfo;
class CvClimateInfo;
class CvSeaLevelInfo;
class CvColorInfo;
class CvPlayerColorInfo;
class CvAdvisorInfo;
class CvRouteModelInfo;
class CvRiverModelInfo;
class CvWaterPlaneInfo;
class CvTerrainPlaneInfo;
class CvCameraOverlayInfo;
class CvAnimationPathInfo;
class CvAnimationCategoryInfo;
class CvEntityEventInfo;
class CvEffectInfo;
class CvAttachableInfo;
class CvUnitFormationInfo;
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
class CvGameOptionInfo;
class CvMPOptionInfo;
class CvForceControlInfo;
class CvPlayerOptionInfo;
class CvGraphicOptionInfo;
class CvEventTriggerInfo;
class CvEventInfo;
class CvEspionageMissionInfo;
class CvUnitArtStyleTypeInfo;
class CvVoteSourceInfo;
class CvMainMenuInfo;
class CvPropertyInfo;
class CvOutcomeInfo;
class CvUnitCombatInfo;
class CvPromotionLineInfo;
class CvMapCategoryInfo;
class CvIdeaClassInfo;
class CvIdeaInfo;
class CvInvisibleInfo;
class CvCategoryInfo;
class CvHeritageInfo;
//class CvTraitOptionEditsInfo;
class CvModLoadControlInfo;
class CvMapInfo;

#include "CvInfoClassTraits.h"
#include "CvInfoReplacements.h"
#include "GlobalDefines.h"
#include <stack>
#include <vector>

enum DelayedResolutionTypes
{
	NO_DELAYED_RESOLUTION,
	USE_DELAYED_RESOLUTION
};

extern CvDLLUtilityIFaceBase* gDLL;
extern bool gMiscLogging;

class cvInternalGlobals
	: private bst::noncopyable
{
	friend class CvXMLLoadUtility;
public:

	// singleton accessor
	inline static cvInternalGlobals& getInstance();

	cvInternalGlobals();
	virtual ~cvInternalGlobals();

	void init();
	void uninit();
	void clearTypesMap();

	CvDLLUtilityIFaceBase* getDLLIFace() const			{ return gDLL; }
	CvDiplomacyScreen* getDiplomacyScreen() const 		{ return m_diplomacyScreen; }
	CMPDiplomacyScreen* getMPDiplomacyScreen() const 	{ return m_mpDiplomacyScreen; }
	FMPIManager*& getFMPMgrPtr()	 					{ return m_pFMPMgr; }
	CvPortal& getPortal() const 						{ return *m_portal; }
	CvSetupData& getSetupData() const 					{ return *m_setupData; }
	CvInitCore& getInitCore() const 					{ return *m_initCore; }
	CvInitCore& getLoadedInitCore() const 				{ return *m_loadedInitCore; }
	CvInitCore& getIniInitCore() const 					{ return *m_iniInitCore; }
	CvMessageCodeTranslator& getMessageCodes() const 	{ return *m_messageCodes; }
	CvStatsReporter& getStatsReporter() const 			{ return *m_statsReporter; }
	CvStatsReporter* getStatsReporterPtr() const 		{ return m_statsReporter; }
	CvInterface& getInterface() const 					{ return *m_interface; }
	CvInterface* getInterfacePtr() const 				{ return m_interface; }

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	CvMap& getMap() const;
	const bst::array<CvMap*, NUM_MAPS>& getMaps() const { return m_maps; }

	CvViewport* getCurrentViewport() const;
	int getViewportCenteringBorder() const;
	CvMapExternal& getMapExternal() const;

	bool bugInitCalled() const;
	bool viewportsEnabled() const;
	int getNumMapInfos() const;
	CvMapInfo& getMapInfo(MapTypes eMap) const;

	void switchMap(MapTypes eMap);
	CvMap& getMapByIndex(MapTypes eIndex) const;
	void clearSigns();
	void reprocessSigns();
	void setResourceLayer(bool bOn);
	bool getResourceLayer() const;
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

	CvGameAI& getGame() const 					{ return *m_game; }
	CvGameAI* getGamePointer();
	CvRandom& getASyncRand() const 				{ return *m_asyncRand; }
	CMessageQueue& getMessageQueue() const 		{ return *m_messageQueue; }
	CMessageQueue& getHotMessageQueue() const 	{ return *m_hotJoinMsgQueue; }
	CMessageControl& getMessageControl() const 	{ return *m_messageControl; }
	CvDropMgr& getDropMgr() const 				{ return *m_dropMgr; }
	FAStar& getPathFinder(MapTypes map = NO_MAP) const;
	FAStar& getInterfacePathFinder(MapTypes map = NO_MAP) const;
	FAStar& getStepFinder(MapTypes map = NO_MAP) const;
	FAStar& getRouteFinder(MapTypes map = NO_MAP) const;
	FAStar& getBorderFinder(MapTypes map = NO_MAP) const;
	FAStar& getAreaFinder(MapTypes map = NO_MAP) const;
	FAStar& getPlotGroupFinder(MapTypes map = NO_MAP) const;

	std::vector<CvInterfaceModeInfo*>& getInterfaceModeInfos();
	CvInterfaceModeInfo& getInterfaceModeInfo(InterfaceModeTypes e) const;

	bool& getLogging() 							{ return m_bLogging; }
	bool& getRandLogging() 						{ return m_bRandLogging; }
	bool& getSynchLogging() 					{ return m_bSynchLogging; }
	bool& overwriteLogs() 						{ return m_bOverwriteLogs; }

	inline bool	getIsInPedia() const 			{ return m_bIsInPedia; }
	inline void	setIsInPedia(bool bNewValue) 	{ m_bIsInPedia = bNewValue; }

	int* getPlotDirectionX() const;
	int* getPlotDirectionY() const;
	int* getPlotCardinalDirectionX() const;
	int* getPlotCardinalDirectionY() const;
	int* getCityPlotX() const;
	int* getCityPlotY() const;
	int* getCityPlotPriority() const;
	int getXYCityPlot(int i, int j) const;
	DirectionTypes* getTurnLeftDirection() const;
	DirectionTypes getTurnLeftDirection(int i) const;
	DirectionTypes* getTurnRightDirection() const;
	DirectionTypes getTurnRightDirection(int i) const;
	DirectionTypes getXYDirection(int i, int j) const;

	//
	// Global Infos
	// All info type strings are upper case and are kept in this hash map for fast lookup
	//

	// returns the infos index, use this when searching for an info type string
	int getInfoTypeForString(const char* szType, bool hideAssert = false) const;
	void setInfoTypeFromString(const char* szType, int idx);
	void logInfoTypeMap(const char* tagMsg = "");
	void infoTypeFromStringReset();
	void addToInfosVectors(void* infoVector, InfoClassTypes eInfoClass);
	void infosReset();
	void cacheInfoTypes();
	int getOrCreateInfoTypeForString(const char* szType);

	bool isDelayedResolutionRequired(InfoClassTypes eLoadingClass, InfoClassTypes eRefClass) const;
	void addDelayedResolution(int* pType, CvString szString);
	CvString* getDelayedResolution(int* pType);
	void removeDelayedResolution(int* pType);
	template<class T>
	void removeDelayedResolutionVector(const std::vector<T>& vector)
	{
		PROFILE_EXTRA_FUNC();
		foreach_(const T& type, vector)
			removeDelayedResolution((int*)&type);
	}
	void copyNonDefaultDelayedResolution(int* pTypeSelf, int* pTypeOther);
	template<class T>
	void copyNonDefaultDelayedResolutionVector(std::vector<T>& aTarget, const std::vector<T>& aSource)
	{
		PROFILE_EXTRA_FUNC();
		std::stack<CvString> aszTemp;
		std::vector<T>::const_iterator it = aTarget.end(), it2 = aSource.begin();
		while (it > aTarget.begin())
		{
			it--;
			aszTemp.push(*getDelayedResolution((int*)&*it));
			removeDelayedResolution((int*)&*it);
		}
		aTarget.insert(aTarget.end(), aSource.begin(), aSource.end());
		it = aTarget.begin(); // because insert() invalidates the previous iterator
		while (aszTemp.size())
		{
			addDelayedResolution((int*)&*it++, aszTemp.top());
			aszTemp.pop();
		}
		while (it2 != aSource.end())
		{
			addDelayedResolution((int*)&*it++, *getDelayedResolution((int*)&*it2++));
		}
	}
	void resolveDelayedResolution();

	int getNumWorldInfos() const;
	CvWorldInfo& getWorldInfo(WorldSizeTypes e) const;

	int getNumClimateInfos() const;
	CvClimateInfo& getClimateInfo(ClimateTypes e) const;

	int getNumSeaLevelInfos() const;
	CvSeaLevelInfo& getSeaLevelInfo(SeaLevelTypes e) const;

	int getNumColorInfos() const;
	CvColorInfo& getColorInfo(ColorTypes e) const;

	int getNumPlayerColorInfos() const;
	CvPlayerColorInfo& getPlayerColorInfo(PlayerColorTypes e) const;

	int getNumAdvisorInfos() const;
	CvAdvisorInfo& getAdvisorInfo(AdvisorTypes e) const;

	int getNumHints() const;
	CvInfoBase& getHints(int i) const;

	int getNumMainMenus() const;
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
	CvModLoadControlInfo& getModLoadControlInfos(int i) const;
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/

	int getNumRouteModelInfos() const;
	CvRouteModelInfo& getRouteModelInfo(int i) const;

	int getNumRiverModelInfos() const;
	CvRiverModelInfo& getRiverModelInfo(int i) const;

	int getNumWaterPlaneInfos() const;
	CvWaterPlaneInfo& getWaterPlaneInfo(int i) const;

	int getNumTerrainPlaneInfos() const;
	CvTerrainPlaneInfo& getTerrainPlaneInfo(int i) const;

	int getNumCameraOverlayInfos() const;
	CvCameraOverlayInfo& getCameraOverlayInfo(int i) const;

	int getNumAnimationPathInfos() const;
	CvAnimationPathInfo& getAnimationPathInfo(AnimationPathTypes e) const;

	int getNumAnimationCategoryInfos() const;
	CvAnimationCategoryInfo& getAnimationCategoryInfo(AnimationCategoryTypes e) const;

	int getNumEntityEventInfos() const;
	CvEntityEventInfo& getEntityEventInfo(EntityEventTypes e) const;

	int getNumEffectInfos() const;
	CvEffectInfo& getEffectInfo(int i) const;

	int getNumAttachableInfos() const;
	CvAttachableInfo& getAttachableInfo(int i) const;

	int getNumUnitFormationInfos() const;
	CvUnitFormationInfo& getUnitFormationInfo(int i) const;

	int getNumLandscapeInfos() const;
	CvLandscapeInfo& getLandscapeInfo(int iIndex) const;
	int getActiveLandscapeID() const;
	void setActiveLandscapeID(int iLandscapeID);

	int getNumTerrainInfos() const;
	CvTerrainInfo& getTerrainInfo(TerrainTypes eTerrainNum) const;

	int getNumBonusClassInfos() const;
	CvBonusClassInfo& getBonusClassInfo(BonusClassTypes eBonusNum) const;

	int getNumBonusInfos() const;
	const std::vector<CvBonusInfo*>& getBonusInfos() const;
	CvBonusInfo& getBonusInfo(BonusTypes eBonusNum) const;

	int getNumMapBonuses() const;
	BonusTypes getMapBonus(const int i) const;

	PromotionTypes getStatusPromotion(int i) const;
	int getNumStatusPromotions() const;

	inline PromotionTypes getStarsign(int i) const { return m_starsigns[i]; }
	inline int getNumStarsigns() const { return (int)m_starsigns.size(); }

	int getNumFeatureInfos() const;
	CvFeatureInfo& getFeatureInfo(FeatureTypes eFeatureNum) const;

	int& getNumPlayableCivilizationInfos();
	int& getNumAIPlayableCivilizationInfos();
	int getNumCivilizationInfos() const;
	CvCivilizationInfo& getCivilizationInfo(CivilizationTypes eCivilizationNum) const;

	int getNumLeaderHeadInfos() const;
	CvLeaderHeadInfo& getLeaderHeadInfo(LeaderHeadTypes eLeaderHeadNum) const;

	int getNumTraitInfos() const;
	CvTraitInfo& getTraitInfo(TraitTypes eTraitNum) const;
	const std::vector<CvTraitInfo*>& getTraitInfos() const { return m_paTraitInfo; }

	int getNumCursorInfos() const;
	CvCursorInfo& getCursorInfo(CursorTypes eCursorNum) const;

	int getNumThroneRoomCameras() const;
	CvThroneRoomCamera& getThroneRoomCamera(int iIndex) const;

	int getNumThroneRoomInfos() const;
	CvThroneRoomInfo& getThroneRoomInfo(int iIndex) const;

	int getNumThroneRoomStyleInfos() const;
	CvThroneRoomStyleInfo& getThroneRoomStyleInfo(int iIndex) const;

	int getNumSlideShowInfos() const;
	CvSlideShowInfo& getSlideShowInfo(int iIndex) const;

	int getNumSlideShowRandomInfos() const;
	CvSlideShowRandomInfo& getSlideShowRandomInfo(int iIndex) const;

	int getNumWorldPickerInfos() const;
	CvWorldPickerInfo& getWorldPickerInfo(int iIndex) const;

	int getNumSpaceShipInfos() const;
	CvSpaceShipInfo& getSpaceShipInfo(int iIndex) const;

	int getNumUnitInfos() const;
	CvUnitInfo& getUnitInfo(UnitTypes eUnitNum) const;
	const std::vector<CvUnitInfo*>& getUnitInfos() const { return m_paUnitInfo; }

	int getNumSpawnInfos() const;
	CvSpawnInfo& getSpawnInfo(SpawnTypes eSpawnNum) const;

	int getNumSpecialUnitInfos() const;
	CvSpecialUnitInfo& getSpecialUnitInfo(SpecialUnitTypes eSpecialUnitNum) const;

	int getNumConceptInfos() const;
	CvInfoBase& getConceptInfo(ConceptTypes e) const;

	int getNumNewConceptInfos() const;
	CvInfoBase& getNewConceptInfo(NewConceptTypes e) const;

	int getNumPropertyInfos() const;
	CvPropertyInfo& getPropertyInfo(PropertyTypes ePropertyNum) const;

	int getNumOutcomeInfos() const;
	CvOutcomeInfo& getOutcomeInfo(OutcomeTypes eOutcomeNum) const;

	int iStuckUnitID;
	int iStuckUnitCount;

	void updateReplacements();

	int getNumCityTabInfos() const;
	CvInfoBase& getCityTabInfo(CityTabTypes e) const;

	int getNumCalendarInfos() const;
	CvInfoBase& getCalendarInfo(CalendarTypes e) const;

	int getNumSeasonInfos() const;
	CvInfoBase& getSeasonInfo(SeasonTypes e) const;

	int getNumMonthInfos() const;
	CvInfoBase& getMonthInfo(MonthTypes e) const;

	int getNumDenialInfos() const;
	CvInfoBase& getDenialInfo(DenialTypes e) const;

	int getNumInvisibleInfos() const;
	CvInvisibleInfo& getInvisibleInfo(InvisibleTypes e) const;

	int getNumCategoryInfos() const;
	CvCategoryInfo& getCategoryInfo(CategoryTypes e) const;

	int getNumHeritageInfos() const;
	CvHeritageInfo& getHeritageInfo(HeritageTypes e) const;

	int getNumVoteSourceInfos() const;
	CvVoteSourceInfo& getVoteSourceInfo(VoteSourceTypes e) const;

	int getNumUnitCombatInfos() const;
	CvUnitCombatInfo& getUnitCombatInfo(UnitCombatTypes e) const;

	CvInfoBase& getDomainInfo(DomainTypes e) const;

	int getNumPromotionLineInfos() const;
	CvPromotionLineInfo& getPromotionLineInfo(PromotionLineTypes e) const;

	int getNumMapCategoryInfos() const;
	CvMapCategoryInfo& getMapCategoryInfo(MapCategoryTypes e) const;

	int getNumIdeaClassInfos() const;
	CvIdeaClassInfo& getIdeaClassInfo(IdeaClassTypes e) const;

	int getNumIdeaInfos() const;
	CvIdeaInfo& getIdeaInfo(IdeaTypes e) const;

	CvInfoBase& getUnitAIInfo(UnitAITypes eUnitAINum) const;

	bool isGraphicalPaging() const { return m_bGraphicalPaging; }

	//	Koshling - added internal registration of supported UnitAI types, not reliant
	//	on external definition in XML
private:
	void registerUnitAI(const char* szType, int enumVal);
	void registerMission(const char* szType, int enumVal);

public:
	void registerPlotTypes();
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
	void registerNPCPlayers();
	void registerClimateZones();

	CvInfoBase& getAttitudeInfo(AttitudeTypes eAttitudeNum) const;

	CvInfoBase& getMemoryInfo(MemoryTypes eMemoryNum) const;

	int getNumGameOptionInfos() const;
	CvGameOptionInfo& getGameOptionInfo(GameOptionTypes eGameOptionNum) const;

	int getNumMPOptionInfos() const;
	CvMPOptionInfo& getMPOptionInfo(MultiplayerOptionTypes eMPOptionNum) const;

	int getNumForceControlInfos() const;
	CvForceControlInfo& getForceControlInfo(ForceControlTypes eForceControlNum) const;

	CvPlayerOptionInfo& getPlayerOptionInfo(PlayerOptionTypes ePlayerOptionNum) const;

	CvGraphicOptionInfo& getGraphicOptionInfo(GraphicOptionTypes eGraphicOptionNum) const;

	CvYieldInfo& getYieldInfo(YieldTypes eYieldNum) const;

	CvCommerceInfo& getCommerceInfo(CommerceTypes eCommerceNum) const;

	int getNumRouteInfos() const;
	CvRouteInfo& getRouteInfo(RouteTypes eRouteNum) const;

	int getNumImprovementInfos() const;
	CvImprovementInfo& getImprovementInfo(ImprovementTypes eImprovementNum) const;

	int getNumGoodyInfos() const;
	CvGoodyInfo& getGoodyInfo(GoodyTypes eGoodyNum) const;

	int getNumBuildInfos() const;
	CvBuildInfo& getBuildInfo(BuildTypes eBuildNum) const;
	const std::vector<CvBuildInfo*>& getBuildInfos() const { return m_paBuildInfo; }

	int getNumHandicapInfos() const;
	CvHandicapInfo& getHandicapInfo(HandicapTypes eHandicapNum) const;

	int getNumGameSpeedInfos() const;
	CvGameSpeedInfo& getGameSpeedInfo(GameSpeedTypes eGameSpeedNum) const;

	int getNumTurnTimerInfos() const;
	CvTurnTimerInfo& getTurnTimerInfo(TurnTimerTypes eTurnTimerNum) const;

	int getNumProcessInfos() const;
	CvProcessInfo& getProcessInfo(ProcessTypes e) const;

	int getNumVoteInfos() const;
	CvVoteInfo& getVoteInfo(VoteTypes e) const;

	int getNumProjectInfos() const;
	CvProjectInfo& getProjectInfo(ProjectTypes e) const;

	int getNumBuildingInfos() const;
	CvBuildingInfo& getBuildingInfo(BuildingTypes eBuildingNum) const;
	const std::vector<CvBuildingInfo*>& getBuildingInfos() const { return m_paBuildingInfo; }

	int getNumSpecialBuildingInfos() const;
	CvSpecialBuildingInfo& getSpecialBuildingInfo(SpecialBuildingTypes eSpecialBuildingNum) const;

	int getNumActionInfos() const;
	CvActionInfo& getActionInfo(int i) const;

	int getNumMissionInfos() const;
	CvMissionInfo& getMissionInfo(MissionTypes eMissionNum) const;

	CvControlInfo& getControlInfo(ControlTypes eControlNum) const;

	CvCommandInfo& getCommandInfo(CommandTypes eCommandNum) const;

	int getNumAutomateInfos() const;
	CvAutomateInfo& getAutomateInfo(int iAutomateNum) const;

	int getNumPromotionInfos() const;
	CvPromotionInfo& getPromotionInfo(PromotionTypes ePromotionNum) const;
	typedef bst::function<bool(const CvPromotionInfo*, PromotionTypes)> PromotionPredicateFn;
	PromotionTypes findPromotion(PromotionPredicateFn predicateFn) const;

	int getNumTechInfos() const;
	CvTechInfo& getTechInfo(TechTypes eTechNum) const;

	int getNumReligionInfos() const;
	CvReligionInfo& getReligionInfo(ReligionTypes eReligionNum) const;
	const std::vector<CvReligionInfo*>& getReligionInfos() const { return m_paReligionInfo; }

	int getNumCorporationInfos() const;
	CvCorporationInfo& getCorporationInfo(CorporationTypes eCorporationNum) const;

	int getNumSpecialistInfos() const;
	CvSpecialistInfo& getSpecialistInfo(SpecialistTypes eSpecialistNum) const;

	int getNumCivicOptionInfos() const;
	CvCivicOptionInfo& getCivicOptionInfo(CivicOptionTypes eCivicOptionNum) const;

	int getNumCivicInfos() const;
	CvCivicInfo& getCivicInfo(CivicTypes eCivicNum) const;

	int getNumDiplomacyInfos() const;
	CvDiplomacyInfo& getDiplomacyInfo(int iDiplomacyNum) const;

	int getNumEraInfos() const;
	CvEraInfo& getEraInfo(EraTypes eEraNum) const;

	int getNumHurryInfos() const;
	CvHurryInfo& getHurryInfo(HurryTypes eHurryNum) const;

	int getNumEmphasizeInfos() const;
	CvEmphasizeInfo& getEmphasizeInfo(EmphasizeTypes eEmphasizeNum) const;

	int getNumUpkeepInfos() const;
	CvUpkeepInfo& getUpkeepInfo(UpkeepTypes eUpkeepNum) const;

	int getNumCultureLevelInfos() const;
	CvCultureLevelInfo& getCultureLevelInfo(CultureLevelTypes eCultureLevelNum) const;
	const std::vector<CvCultureLevelInfo*>& getCultureLevelInfos() const { return m_paCultureLevelInfo; }

	int getNumVictoryInfos() const;
	CvVictoryInfo& getVictoryInfo(VictoryTypes eVictoryNum) const;

	int getNumEventTriggerInfos() const;
	CvEventTriggerInfo& getEventTriggerInfo(EventTriggerTypes eEventTrigger) const;

	int getNumEventInfos() const;
	CvEventInfo& getEventInfo(EventTypes eEvent) const;

	int getNumEspionageMissionInfos() const;
	CvEspionageMissionInfo& getEspionageMissionInfo(EspionageMissionTypes eEspionageMissionNum) const;

	int getNumUnitArtStyleTypeInfos() const;
	CvUnitArtStyleTypeInfo& getUnitArtStyleTypeInfo(UnitArtStyleTypes eUnitArtStyleTypeNum) const;

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
	int getFootstepAudioTypeByTag(const CvString strTag) const;

	CvString*& getFootstepAudioTags();
	CvString& getFootstepAudioTags(int i) const;

	const CvString& getCurrentXMLFile() const;
	void setCurrentXMLFile(const char* szFileName);

	//
	///////////////// BEGIN global defines
	// THESE ARE READ-ONLY
	//

	FVariableSystem* getDefinesVarSystem() const;
	void cacheEnumGlobals();
	void cacheGlobals();

#define DECLARE_GET_METHOD(dataType, VAR) \
	dataType get##VAR() const { return m_##VAR; }

	DO_FOR_EACH_INT_GLOBAL_DEFINE(DECLARE_GET_METHOD)
	DO_FOR_EACH_ENUM_GLOBAL_DEFINE(DECLARE_GET_METHOD)
	DO_FOR_EACH_FLOAT_GLOBAL_DEFINE(DECLARE_GET_METHOD)

#define DECLARE_GET_METHOD_WITH_FASSERT(dataType, VAR) \
	dataType get##VAR() const \
	{ \
		FAssert(m_##VAR != -1); \
		return m_##VAR; \
	}

	DO_FOR_EACH_INFO_TYPE(DECLARE_GET_METHOD_WITH_FASSERT)

#define DECLARE_IS_METHOD(dataType, VAR) \
	dataType is##VAR() const { return m_##VAR; }

	DO_FOR_EACH_BOOL_GLOBAL_DEFINE(DECLARE_IS_METHOD)

	// ***** EXPOSED TO PYTHON *****

	bool getDefineBOOL(const char* szName, bool bDefault = false) const;
	int getDefineINT(const char* szName, int iDefault = 0) const;
	float getDefineFLOAT(const char* szName, float fDefault = 0.0f) const;
	const char* getDefineSTRING(const char* szName, const char* szDefault = "") const;

	void setDefineINT(const char* szName, int iValue, bool bUpdate = true);
	void setDefineFLOAT(const char* szName, float fValue, bool bUpdate = true);
	void setDefineSTRING(const char* szName, const char* szValue, bool bUpdate = true);

	float getPLOT_SIZE() const;

	// ***** END EXPOSED TO PYTHON *****

	////////////// END DEFINES //////////////////

	void setDLLProfiler(FProfiler* prof);
	FProfiler* getDLLProfiler() const;
	void enableDLLProfiler(bool bEnable);
	bool isDLLProfilerEnabled() const;
	const char* alternateProfileSampleName() const;

	bool IsGraphicsInitialized() const;
	void SetGraphicsInitialized(bool bVal);

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
	void setIsBug();
	void refreshOptionsBUG();

	uint32_t getAssetCheckSum() const;

	void deleteInfoArrays();

	void checkInitialCivics();

	void cacheGameSpecificValues();

protected:
	void doPostLoadCaching();

	bool m_bGraphicsInitialized;
	bool m_bDLLProfiler;
	bool m_bLogging;
	bool m_bRandLogging;
	bool m_bSynchLogging;
	bool m_bOverwriteLogs;
	bool m_bGraphicalPaging;

	FMPIManager* m_pFMPMgr;

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
	CvDiplomacyScreen* m_diplomacyScreen;
	CMPDiplomacyScreen* m_mpDiplomacyScreen;

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	bst::array<CvMap*, NUM_MAPS> m_maps;
	bool m_bResourceLayerOn;
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

	bst::array<FAStar*, NUM_MAPS> m_pathFinders;
	bst::array<FAStar*, NUM_MAPS> m_interfacePathFinders;
	bst::array<FAStar*, NUM_MAPS> m_stepFinders;
	bst::array<FAStar*, NUM_MAPS> m_routeFinders;
	bst::array<FAStar*, NUM_MAPS> m_borderFinders;
	bst::array<FAStar*, NUM_MAPS> m_areaFinders;
	bst::array<FAStar*, NUM_MAPS> m_plotGroupFinders;

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
		static inline size_t fnv1a(uint8_t oneByte, size_t hash = Seed)
		{
			return (oneByte ^ hash) * Prime;
		}

		// hash a block of memory
		static inline size_t fnv1a(const void* data, size_t numBytes, size_t hash = Seed)
		{
			FAssert(data);
			const uint8_t* ptr = (const uint8_t*)data;
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
	bst::array<uint16_t, NUM_INFO_CLASSES> m_infoClassXmlLoadOrder;

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
	std::vector<CvCategoryInfo*> m_paCategoryInfo;
	std::vector<CvHeritageInfo*> m_heritageInfo;
	std::vector<CvVoteSourceInfo*> m_paVoteSourceInfo;
	std::vector<CvUnitCombatInfo*> m_paUnitCombatInfo;
	std::vector<CvPromotionLineInfo*> m_paPromotionLineInfo;
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
	std::vector<CvEventTriggerInfo*> m_paEventTriggerInfo;
	CvInfoReplacements<CvEventTriggerInfo> m_EventTriggerInfoReplacements;
	std::vector<CvEventInfo*> m_paEventInfo;
	CvInfoReplacements<CvEventInfo> m_EventInfoReplacements;
	std::vector<CvEspionageMissionInfo*> m_paEspionageMissionInfo;
	std::vector<CvUnitArtStyleTypeInfo*> m_paUnitArtStyleTypeInfo;
	std::vector<CvPropertyInfo*> m_paPropertyInfo;
	std::vector<CvOutcomeInfo*> m_paOutcomeInfo;
	std::vector<CvMapInfo*> m_paMapInfo;

	//////////////////////////////////////////////////////////////////////////
	// GLOBAL TYPES
	//////////////////////////////////////////////////////////////////////////

	// XXX These are duplicates and are kept for enumeration convenience - most could be removed, Moose
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

#define DECLARE_MEMBER_VAR(dataType, VAR) \
	dataType m_##VAR;

	DO_FOR_EACH_GLOBAL_DEFINE(DECLARE_MEMBER_VAR)
	DO_FOR_EACH_INFO_TYPE(DECLARE_MEMBER_VAR)

	float m_fPLOT_SIZE;

	int m_iViewportCenterOnSelectionCenterBorder;

	const char* m_szAlternateProfilSampleName;
	FProfiler* m_Profiler;
	CvString m_szDllProfileText;

	std::vector<BonusTypes> m_mapBonuses;
	std::vector<PromotionTypes> m_aiStatusPromotions;
	std::vector<PromotionTypes> m_starsigns;

// BBAI Options
public:
	bool getBBAI_AIR_COMBAT() const;
	bool getBBAI_HUMAN_VASSAL_WAR_BUILD() const;

protected:
	bool m_bBBAI_AIR_COMBAT;
	bool m_bBBAI_HUMAN_VASSAL_WAR_BUILD;

// Tech Diffusion
public:
	bool getTECH_DIFFUSION_ENABLE() const;

protected:
	bool m_bTECH_DIFFUSION_ENABLE;

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
	ProxyTracker(const char* name);
	~ProxyTracker();
};

#define PROXY_TRACK(x)	ProxyTracker tracker(x);

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
public:
	DllExport inline static CvGlobals& getInstance();

	CvGlobals() {}
	virtual ~CvGlobals() {}

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
	// This determines how many starting civs can be chosen in the custom game staging room screen.
	// Currently it allows you to add more civs than there are civ slots.
	// If this is reduced to the correct number, the city bar and city graphics for the neanderthal NPC will never be updated as the exe doesn't understand that their cities exist.
	DllExport int getMaxCivPlayers() const
	{
		PROXY_TRACK("getMaxCivPlayers");
		return MAX_PLAYERS-1;
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
	DllExport CvGameAI* getGamePointer()
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
	DllExport void infoTypeFromStringReset()
	{
		PROXY_TRACK("infoTypeFromStringReset");
		gGlobals->infoTypeFromStringReset();
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
	DllExport int getNumHints()
	{
		PROXY_TRACK("getNumHints");
		return gGlobals->getNumHints();
	}
	DllExport CvInfoBase& getHints(int i)
	{
		PROXY_TRACK("getHints");
		return gGlobals->getHints(i);
	}
	DllExport CvMainMenuInfo& getMainMenus(int i)
	{
		PROXY_TRACK("getMainMenus");
		return gGlobals->getMainMenus(i);
	}
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
	DllExport CvAnimationPathInfo& getAnimationPathInfo(AnimationPathTypes e)
	{
		PROXY_TRACK("getAnimationPathInfo");
		return gGlobals->getAnimationPathInfo(e);
	}
	DllExport CvAnimationCategoryInfo& getAnimationCategoryInfo(AnimationCategoryTypes e)
	{
		PROXY_TRACK("getAnimationCategoryInfo");
		return gGlobals->getAnimationCategoryInfo(e);
	}
	DllExport CvEntityEventInfo& getEntityEventInfo(EntityEventTypes e)
	{
		PROXY_TRACK("getEntityEventInfo");
		return gGlobals->getEntityEventInfo(e);
	}
	DllExport CvEffectInfo& getEffectInfo(int i)
	{
		PROXY_TRACK("getEffectInfo");
		return gGlobals->getEffectInfo(i);
	}
	DllExport CvAttachableInfo& getAttachableInfo(int i)
	{
		PROXY_TRACK("getAttachableInfo");
		return gGlobals->getAttachableInfo(i);
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
	DllExport CvCursorInfo& getCursorInfo(CursorTypes eCursorNum)
	{
		PROXY_TRACK("getCursorInfo");
		return gGlobals->getCursorInfo(eCursorNum);
	}
	DllExport CvThroneRoomCamera& getThroneRoomCamera(int iIndex)
	{
		PROXY_TRACK("getThroneRoomCamera");
		return gGlobals->getThroneRoomCamera(iIndex);
	}
	DllExport int getNumThroneRoomInfos()
	{
		PROXY_TRACK("getNumThroneRoomInfos");
		return gGlobals->getNumThroneRoomInfos();
	}
	DllExport CvThroneRoomInfo& getThroneRoomInfo(int iIndex)
	{
		PROXY_TRACK("getThroneRoomInfo");
		return gGlobals->getThroneRoomInfo(iIndex);
	}
	DllExport int getNumThroneRoomStyleInfos()
	{
		PROXY_TRACK("getNumThroneRoomStyleInfos");
		return gGlobals->getNumThroneRoomStyleInfos();
	}
	DllExport CvThroneRoomStyleInfo& getThroneRoomStyleInfo(int iIndex)
	{
		PROXY_TRACK("getThroneRoomStyleInfo");
		return gGlobals->getThroneRoomStyleInfo(iIndex);
	}
	DllExport int getNumSlideShowInfos()
	{
		PROXY_TRACK("getNumSlideShowInfos");
		return gGlobals->getNumSlideShowInfos();
	}
	DllExport CvSlideShowInfo& getSlideShowInfo(int iIndex)
	{
		PROXY_TRACK("getSlideShowInfo");
		return gGlobals->getSlideShowInfo(iIndex);
	}
	DllExport int getNumSlideShowRandomInfos()
	{
		PROXY_TRACK("getNumSlideShowRandomInfos");
		return gGlobals->getNumSlideShowRandomInfos();
	}
	DllExport CvSlideShowRandomInfo& getSlideShowRandomInfo(int iIndex)
	{
		PROXY_TRACK("getSlideShowRandomInfo");
		return gGlobals->getSlideShowRandomInfo(iIndex);
	}
	DllExport int getNumWorldPickerInfos()
	{
		PROXY_TRACK("getNumWorldPickerInfos");
		return gGlobals->getNumWorldPickerInfos();
	}
	DllExport CvWorldPickerInfo& getWorldPickerInfo(int iIndex)
	{
		PROXY_TRACK("getWorldPickerInfo");
		return gGlobals->getWorldPickerInfo(iIndex);
	}
	DllExport int getNumSpaceShipInfos()
	{
		PROXY_TRACK("getNumSpaceShipInfos");
		return gGlobals->getNumSpaceShipInfos();
	}
	DllExport CvSpaceShipInfo& getSpaceShipInfo(int iIndex)
	{
		PROXY_TRACK("getSpaceShipInfo");
		return gGlobals->getSpaceShipInfo(iIndex);
	}
	DllExport CvGameOptionInfo& getGameOptionInfo(GameOptionTypes eGameOptionNum)
	{
		PROXY_TRACK("getGameOptionInfo");
		return gGlobals->getGameOptionInfo(eGameOptionNum);
	}
	DllExport CvMPOptionInfo& getMPOptionInfo(MultiplayerOptionTypes eMPOptionNum)
	{
		PROXY_TRACK("getMPOptionInfo");
		return gGlobals->getMPOptionInfo(eMPOptionNum);
	}
	DllExport CvPlayerOptionInfo& getPlayerOptionInfo(PlayerOptionTypes ePlayerOptionNum)
	{
		PROXY_TRACK("getPlayerOptionInfo");
		return gGlobals->getPlayerOptionInfo(ePlayerOptionNum);
	}
	DllExport CvGraphicOptionInfo& getGraphicOptionInfo(GraphicOptionTypes eGraphicOptionNum)
	{
		PROXY_TRACK("getGraphicOptionInfo");
		return gGlobals->getGraphicOptionInfo(eGraphicOptionNum);
	}
	DllExport int getNumRouteInfos()
	{
		PROXY_TRACK("getNumRouteInfos");
		return gGlobals->getNumRouteInfos();
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
		return gGlobals->getInfoTypeForString(szType);
	}
	DllExport int getNUM_ENGINE_DIRTY_BITS() const
	{
		PROXY_TRACK("getNUM_ENGINE_DIRTY_BITS");
		return NUM_ENGINE_DIRTY_BITS;
	}
	DllExport int getNUM_INTERFACE_DIRTY_BITS() const
	{
		PROXY_TRACK("getNUM_INTERFACE_DIRTY_BITS");
		return NUM_INTERFACE_DIRTY_BITS;
	}
	DllExport int getNUM_YIELD_TYPES() const
	{
		PROXY_TRACK("getNUM_YIELD_TYPES");
		return NUM_YIELD_TYPES;
	}
	DllExport int getNUM_FORCECONTROL_TYPES() const
	{
		PROXY_TRACK("getNUM_FORCECONTROL_TYPES");
		return NUM_FORCECONTROL_TYPES;
	}
	DllExport int getNUM_INFOBAR_TYPES() const
	{
		PROXY_TRACK("getNUM_INFOBAR_TYPES");
		return NUM_INFOBAR_TYPES;
	}
	DllExport int getNUM_HEALTHBAR_TYPES() const
	{
		PROXY_TRACK("getNUM_HEALTHBAR_TYPES");
		return NUM_HEALTHBAR_TYPES;
	}
	DllExport int getNUM_LEADERANIM_TYPES() const
	{
		PROXY_TRACK("getNUM_LEADERANIM_TYPES");
		return NUM_LEADERANIM_TYPES;
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
	DllExport int& getNumFootstepAudioTypes()
	{
		PROXY_TRACK("getNumFootstepAudioTypes");
		return gGlobals->getNumFootstepAudioTypes();
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
		// Toffer - Change this to true to make exe call String finishText([String,]) within CvTranslator.py.
		//	text handled by the exe will be sent as input to the python function line by line
		//	and the string it gets in return will replace whatever it sent in.
		//	Not sure we can actually use this for anythin, I think it's mostly a weird way to alter exe tooltip text.
		//	Maybe there's some text hardcoded in the exe we can change through this, though I doubt that.
		return false;
	}
	DllExport int getMAX_CIV_PLAYERS()
	{
		PROXY_TRACK("getMAX_CIV_PLAYERS");
		return MAX_PC_PLAYERS;
	}
	// ***** END EXPOSED TO PYTHON *****

	////////////// END DEFINES //////////////////

	DllExport void setDLLIFace(CvDLLUtilityIFaceBase* pDll)
	{
		if (pDll != NULL)
		{
			FAssertMsg(gDLL == NULL, "Dll interface already set?");

			OutputDebugString("setDLLIFace()\n");
			if (gGlobals == NULL)
			{
				OutputDebugString("Constructing internal globals\n");
				gGlobals = new cvInternalGlobals();
			}

			gDLL = pDll;
		}
		else
		{
			FAssertMsg(gDLL != NULL, "Dll interface not set?");
			gDLL = NULL;
			delete gGlobals;
		}
	}
	DllExport CvDLLUtilityIFaceBase* getDLLIFaceNonInl()
	{
		//PROXY_TRACK("getDLLIFaceNonInl");
		return gDLL;
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
		return NUM_DIRECTION_TYPES;
	}
	DllExport int getNumGameOptions() const
	{
		PROXY_TRACK("getNumGameOptions");
		return gGlobals->getNumGameOptionInfos();
	}
	DllExport int getNumMPOptions() const
	{
		PROXY_TRACK("getNumMPOptions");
		return NUM_MPOPTION_TYPES;
	}
	DllExport int getNumSpecialOptions() const
	{
		PROXY_TRACK("getNumSpecialOptions");
		return NUM_SPECIALOPTION_TYPES;
	}
	DllExport int getNumGraphicOptions() const
	{
		PROXY_TRACK("getNumGraphicOptions");
		return NUM_GRAPHICOPTION_TYPES;
	}
	DllExport int getNumTradeableItems() const
	{
		PROXY_TRACK("getNumTradeableItems");
		return NUM_TRADEABLE_ITEMS;
	}
	DllExport int getNumBasicItems() const
	{
		PROXY_TRACK("getNumBasicItems");
		return NUM_BASIC_ITEMS;
	}
	DllExport int getNumTradeableHeadings() const
	{
		PROXY_TRACK("getNumTradeableHeadings");
		return NUM_TRADEABLE_HEADINGS;
	}
	DllExport int getNumPlayerOptionInfos() const
	{
		PROXY_TRACK("getNumPlayerOptionInfos");
		return NUM_PLAYEROPTION_TYPES;
	}
	DllExport int getMaxNumSymbols() const
	{
		PROXY_TRACK("getMaxNumSymbols");
		return MAX_NUM_SYMBOLS;
	}
	DllExport int getNumGraphicLevels() const
	{
		PROXY_TRACK("getNumGraphicLevels");
		return NUM_GRAPHICLEVELS;
	}
	// code to cache infos was deleted. https://github.com/caveman2cosmos/Caveman2Cosmos/issues/751
	DllExport bool readBuildingInfoArray(FDataStreamBase*)		{ return false; }
	DllExport bool readTechInfoArray(FDataStreamBase*)			{ return false; }
	DllExport bool readUnitInfoArray(FDataStreamBase*)			{ return false; }
	DllExport bool readLeaderHeadInfoArray(FDataStreamBase*)	{ return false; }
	DllExport bool readCivilizationInfoArray(FDataStreamBase*)	{ return false; }
	DllExport bool readPromotionInfoArray(FDataStreamBase*)		{ return false; }
	DllExport bool readDiplomacyInfoArray(FDataStreamBase*)		{ return false; }
	DllExport bool readCivicInfoArray(FDataStreamBase*)			{ return false; }
	DllExport bool readHandicapInfoArray(FDataStreamBase*)		{ return false; }
	DllExport bool readBonusInfoArray(FDataStreamBase*)			{ return false; }
	DllExport bool readImprovementInfoArray(FDataStreamBase*)	{ return false; }
	DllExport bool readEventInfoArray(FDataStreamBase*)			{ return false; }
	DllExport bool readEventTriggerInfoArray(FDataStreamBase*)	{ return false; }

	DllExport void writeBuildingInfoArray(FDataStreamBase*)		{ }
	DllExport void writeTechInfoArray(FDataStreamBase*)			{ }
	DllExport void writeUnitInfoArray(FDataStreamBase*)			{ }
	DllExport void writeLeaderHeadInfoArray(FDataStreamBase*)	{ }
	DllExport void writeCivilizationInfoArray(FDataStreamBase*)	{ }
	DllExport void writePromotionInfoArray(FDataStreamBase*)	{ }
	DllExport void writeDiplomacyInfoArray(FDataStreamBase*)	{ }
	DllExport void writeCivicInfoArray(FDataStreamBase*)		{ }
	DllExport void writeHandicapInfoArray(FDataStreamBase*)		{ }
	DllExport void writeBonusInfoArray(FDataStreamBase*)		{ }
	DllExport void writeImprovementInfoArray(FDataStreamBase*)	{ }
	DllExport void writeEventInfoArray(FDataStreamBase*)		{ }
	DllExport void writeEventTriggerInfoArray(FDataStreamBase*)	{ }
};

extern CvGlobals gGlobalsProxy;	// for debugging

//
// inlines
//
inline cvInternalGlobals& cvInternalGlobals::getInstance()
{
	return *gGlobals;
}

inline CvGlobals& CvGlobals::getInstance()
{
	return gGlobalsProxy;
}

#endif