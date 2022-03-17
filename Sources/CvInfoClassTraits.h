#pragma once

#ifndef CIV4_INFO_CLASS_TRAITS_H
#define CIV4_INFO_CLASS_TRAITS_H


#define EXPAND_FOR_EACH_INFO_CLASS(MACRO_FUNC)	\
	MACRO_FUNC(CvBuildingInfo, BUILDING_INFO, BuildingTypes) \
	MACRO_FUNC(CvUnitInfo, UNIT_INFO, UnitTypes) \
	MACRO_FUNC(CvBonusInfo, BONUS_INFO, BonusTypes) \
	MACRO_FUNC(CvProjectInfo, PROJECT_INFO, ProjectTypes) \
	MACRO_FUNC(CvSpecialistInfo, SPECIALIST_INFO, SpecialistTypes) \
	MACRO_FUNC(CvImprovementInfo, IMPROVEMENT_INFO, ImprovementTypes) \
	MACRO_FUNC(CvReligionInfo, RELIGION_INFO, ReligionTypes) \
	MACRO_FUNC(CvPromotionInfo, PROMOTION_INFO, PromotionTypes) \
	MACRO_FUNC(CvCorporationInfo, CORPORATION_INFO, CorporationTypes) \
	MACRO_FUNC(CvTechInfo, TECH_INFO, TechTypes) \
	MACRO_FUNC(CvCivicInfo, CIVIC_INFO, CivicTypes) \
	MACRO_FUNC(CvVoteInfo, VOTE_INFO, VoteTypes) \
	MACRO_FUNC(CvVoteSourceInfo, VOTE_SOURCE_INFO, VoteSourceTypes) \
	MACRO_FUNC(CvSpecialUnitInfo, SPECIAL_UNIT_INFO, SpecialUnitTypes) \
	MACRO_FUNC(CvSpecialBuildingInfo, SPECIAL_BUILDING_INFO, SpecialBuildingTypes) \
	MACRO_FUNC(CvUpkeepInfo, UPKEEP_INFO, UpkeepTypes) \
	MACRO_FUNC(CvHurryInfo, HURRY_INFO, HurryTypes) \
	MACRO_FUNC(CvFeatureInfo, FEATURE_INFO, FeatureTypes) \
	MACRO_FUNC(CvCivicOptionInfo, CIVIC_OPTION_INFO, CivicOptionTypes) \
	MACRO_FUNC(CvBuildInfo, BUILD_INFO, BuildTypes) \
	MACRO_FUNC(CvTerrainInfo, TERRAIN_INFO, TerrainTypes) \
	MACRO_FUNC(CvRouteInfo, ROUTE_INFO, RouteTypes) \
	MACRO_FUNC(CvVictoryInfo, VICTORY_INFO, VictoryTypes) \
	MACRO_FUNC(CvLeaderHeadInfo, LEADERHEAD_INFO, LeaderHeadTypes) \
	MACRO_FUNC(CvCivilizationInfo, CIVILIZATION_INFO, CivilizationTypes) \
	MACRO_FUNC(CvGameOptionInfo, GAME_OPTION_INFO, GameOptionTypes) \
	MACRO_FUNC(CvMPOptionInfo, MP_OPTION_INFO, MultiplayerOptionTypes) \
	MACRO_FUNC(CvUnitAIInfo, UNIT_AI_INFO, UnitAITypes) \
	MACRO_FUNC(CvEventInfo, EVENT_INFO, EventTypes) \
	MACRO_FUNC(CvEventTriggerInfo, EVENT_TRIGGER_INFO, EventTriggerTypes) \
	MACRO_FUNC(CvGameSpeedInfo, GAME_SPEED_INFO, GameSpeedTypes) \
	MACRO_FUNC(CvPropertyInfo, PROPERTy_INFO, PropertyTypes) \
	MACRO_FUNC(CvPromotionLineInfo, PROMOTION_LINE_INFO, PromotionLineTypes) \
	MACRO_FUNC(CvTraitInfo, TRAIT_INFO, TraitTypes) \
	MACRO_FUNC(CvInvisibleInfo, INVISIBLE_INFO, InvisibleTypes) \
	MACRO_FUNC(CvMapCategoryInfo, MAPCATEGORy_INFO, MapCategoryTypes) \
	MACRO_FUNC(CvIdeaClassInfo, IDEACLASS_INFO, IdeaClassTypes) \
	MACRO_FUNC(CvIdeaInfo, IDEA_INFO, IdeaTypes) \
	MACRO_FUNC(CvMissionInfo, MISSION_INFO, MissionTypes) \
	MACRO_FUNC(CvYieldInfo, YIELD_INFO, YieldTypes) \
	MACRO_FUNC(CvCommerceInfo, COMMERCE_INFO, CommerceTypes) \
	MACRO_FUNC(CvDomainInfo, DOMAIN_INFO, DomainTypes) \
	MACRO_FUNC(CvMapInfo, MAP_INFO, MapTypes) \
	MACRO_FUNC(CvConceptInfo, CONCEPT_INFO, ConceptTypes) \
	MACRO_FUNC(CvNewConceptInfo, NEW_CONCEPT_INFO, NewConceptTypes) \
	MACRO_FUNC(CvCityTabInfo, CITY_TAB_INFO, CityTabTypes) \
	MACRO_FUNC(CvCalendarInfo, CALENDAR_INFO, CalendarTypes) \
	MACRO_FUNC(CvSeasonInfo, SEASON_INFO, SeasonTypes) \
	MACRO_FUNC(CvMonthInfo, MONTH_INFO, MonthTypes) \
	MACRO_FUNC(CvDenialInfo, DENIAL_INFO, DenialTypes) \
	MACRO_FUNC(CvAttitudeInfo, ATTITUDE_INFO, AttitudeTypes) \
	MACRO_FUNC(CvMemoryInfo, MEMORY_INFO, MemoryTypes) \
	MACRO_FUNC(CvInterfaceModeInfo, INTERFACE_MODE_INFO, InterfaceModeTypes) \
	MACRO_FUNC(CvWorldInfo, WORLD_INFO, WorldSizeTypes) \
	MACRO_FUNC(CvClimateInfo, CLIMATE_INFO, ClimateTypes) \
	MACRO_FUNC(CvSeaLevelInfo, SEA_LEVEL_INFO, SeaLevelTypes) \
	MACRO_FUNC(CvColorInfo, COLOR_INFO, ColorTypes) \
	MACRO_FUNC(CvPlayerColorInfo, PLAYER_COLOR_INFO, PlayerColorTypes) \
	MACRO_FUNC(CvAdvisorInfo, ADVISOR_INFO, AdvisorTypes) \
	MACRO_FUNC(CvRiverModelInfo, RIVER_MODEL_INFO, RiverModelTypes) \
	MACRO_FUNC(CvWaterPlaneInfo, WATER_PLANE_INFO, WaterPlaneTypes) \
	MACRO_FUNC(CvTerrainPlaneInfo, TERRAIN_PLANE_INFO, TerrainPlaneTypes) \
	MACRO_FUNC(CvCameraOverlayInfo, CAMERA_OVERLAY_INFO, CameraOverlayTypes) \
	MACRO_FUNC(CvAnimationPathInfo, ANIMATION_PATH_INFO, AnimationPathTypes) \
	MACRO_FUNC(CvAnimationCategoryInfo, ANIMATION_CATEGORY_INFO, AnimationCategoryTypes) \
	MACRO_FUNC(CvEntityEventInfo, ENTITY_EVENT_INFO, EntityEventTypes) \
	MACRO_FUNC(CvEffectInfo, EFFECT_INFO, EffectTypes) \
	MACRO_FUNC(CvAttachableInfo, ATTACHABLE_INFO, AttachableTypes) \
	MACRO_FUNC(CvUnitFormationInfo, UNIT_FORMATION_INFO, UnitFormationTypes) \
	MACRO_FUNC(CvLandscapeInfo, LANDSCAPE_INFO, LandscapeTypes) \
	MACRO_FUNC(CvBonusClassInfo, BONUS_CLASS_INFO, BonusClassTypes) \
	MACRO_FUNC(CvCursorInfo, CURSOR_INFO, CursorTypes) \
	MACRO_FUNC(CvThroneRoomInfo, THRONE_ROOM_INFO, ThroneRoomTypes) \
	MACRO_FUNC(CvThroneRoomStyleInfo, THRONE_ROOM_STYLE_INFO, ThroneRoomStyleTypes) \
	MACRO_FUNC(CvThroneRoomCamera, THRONE_ROOM_CAMERA_INFO, int) \
	MACRO_FUNC(CvSlideShowInfo, SLIDE_SHOW_INFO, SlideShowTypes) \
	MACRO_FUNC(CvSlideShowRandomInfo, SLIDE_SHOW_RANDOM_INFO, SlideShowRandomTypes) \
	MACRO_FUNC(CvWorldPickerInfo, WORLD_PICKER_INFO, WorldPickerTypes) \
	MACRO_FUNC(CvSpaceShipInfo, SPACE_SHIP_INFO, SpaceShipTypes) \
	MACRO_FUNC(CvSpawnInfo, SPAWN_INFO, SpawnTypes) \
	MACRO_FUNC(CvRouteModelInfo, ROUTE_MODEL_INFO, RouteModelTypes) \
	MACRO_FUNC(CvGoodyInfo, GOODY_INFO, GoodyTypes) \
	MACRO_FUNC(CvHandicapInfo, HANDICAP_INFO, HandicapTypes) \
	MACRO_FUNC(CvTurnTimerInfo, TURN_TIMER_INFO, TurnTimerTypes) \
	MACRO_FUNC(CvProcessInfo, PROCESS_INFO, ProcessTypes) \
	MACRO_FUNC(CvActionInfo, ACTION_INFO, ActionTypes) \
	MACRO_FUNC(CvControlInfo, CONTROL_INFOS, ControlTypes) \
	MACRO_FUNC(CvCommandInfo, COMMAND_INFO, CommandTypes) \
	MACRO_FUNC(CvAutomateInfo, AUTOMATE_INFO, AutomateTypes) \
	MACRO_FUNC(CvDiplomacyInfo, DIPLOMACY_INFO, DiplomacyTypes) \
	MACRO_FUNC(CvEraInfo, ERA_INFO, EraTypes) \
	MACRO_FUNC(CvEmphasizeInfo, EMPHASIZE_INFO, EmphasizeTypes) \
	MACRO_FUNC(CvCultureLevelInfo, CULTURE_LEVEL_INFO, CultureLevelTypes) \
	MACRO_FUNC(CvForceControlInfo, FORCE_CONTROL_INFO, ForceControlTypes) \
	MACRO_FUNC(CvPlayerOptionInfo, PLAYER_OPTION_INFO, PlayerOptionTypes) \
	MACRO_FUNC(CvGraphicOptionInfo, GRAPHIC_OPTION_INFO, GraphicOptionTypes) \
	MACRO_FUNC(CvEspionageMissionInfo, ESPIONAGE_MISSION_INFO, EspionageMissionTypes) \
	MACRO_FUNC(CvUnitArtStyleTypeInfo, UNIT_ART_STYLE_TYPE_INFO, UnitArtStyleTypes) \
	MACRO_FUNC(CvMainMenuInfo, MAIN_MENU_INFO, MainMenuTypes) \
	MACRO_FUNC(CvOutcomeInfo, OUTCOME_INFO, OutcomeTypes) \
	MACRO_FUNC(CvUnitCombatInfo, UNIT_COMBAT_INFO, UnitCombatTypes) \
	MACRO_FUNC(CvModLoadControlInfo, MOD_LOAD_CONTROL_INFO, ModLoadControlTypes) \
	MACRO_FUNC(CvHintInfo, HINT_INFO, HintTypes) \
	MACRO_FUNC(CvArtInfoAsset, ASSET_ART_INFO, AssetArtTypes) \
	MACRO_FUNC(CvArtInfoMisc, MISC_ART_INFO, MiscArtTypes) \
	MACRO_FUNC(CvArtInfoUnit, UNIT_ART_INFO, UnitArtTypes) \
	MACRO_FUNC(CvArtInfoBuilding, BUILDING_ART_INFO, BuildingArtTypes) \
	MACRO_FUNC(CvArtInfoCivilization, CIVILIZATION_ART_INFO, CivilizationArtTypes) \
	MACRO_FUNC(CvArtInfoLeaderhead, LEADER_HEAD_ART_INFO, LeaderHeadArtTypes) \
	MACRO_FUNC(CvArtInfoBonus, BONNUS_ART_INFO, BonusArtTypes) \
	MACRO_FUNC(CvArtInfoImprovement, IMPROVEMENT_ART_INFO, ImprovementArtTypes) \
	MACRO_FUNC(CvArtInfoTerrain, TERRAIN_ART_INFO, TerrainArtTypes) \
	MACRO_FUNC(CvArtInfoFeature, FEATURE_ART_INFO, FeatureArtTypes) \
	MACRO_FUNC(CvArtInfoMovie, MOVIE_ART_INFO, MovieArtTypes) \
	MACRO_FUNC(CvArtInfoInterface, INTERFACE_ART_INFO, InterfaceArtTypes)


#define LIST_INFO_CLASS_ENUM(_class, _classEnum, _indexType) \
	_classEnum,

enum InfoClassTypes
{
	NO_INFO_CLASS = -1,

	EXPAND_FOR_EACH_INFO_CLASS(LIST_INFO_CLASS_ENUM)

	NUM_INFO_CLASSES
};


class CvInfoBase;

template <class InfoClass_>
struct InfoClassTraits
{
	typedef int IndexType;
	typedef CvInfoBase InfoClass;

	static const InfoClassTypes InfoClassEnum = NO_INFO_CLASS;
};


#define DECLARE_INFO_CLASS_TRAITS(_class, _classEnum, _indexType)	\
																	\
class _class;														\
																	\
template <>															\
struct InfoClassTraits<_class>										\
{																	\
	typedef _indexType IndexType; 									\
	typedef _class InfoClass; 										\
																	\
	static const InfoClassTypes InfoClassEnum = _classEnum;			\
};																	\
																	\
template <>															\
struct InfoClassTraits<_indexType>									\
{																	\
	typedef _indexType IndexType; 									\
	typedef _class InfoClass; 										\
																	\
	static const InfoClassTypes InfoClassEnum = _classEnum;			\
};

EXPAND_FOR_EACH_INFO_CLASS(DECLARE_INFO_CLASS_TRAITS);


#undef LIST_INFO_CLASS_ENUM
#undef DECLARE_INFO_CLASS_TRAITS
#undef EXPAND_FOR_EACH_INFO_CLASS

#endif