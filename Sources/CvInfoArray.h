#pragma once

#ifndef CvInfoArray_h__
#define CvInfoArray_h__

#include "CvGlobals.h"
#include "CvInfoReplacements.h"
#include "CvTaggedSaveFormatWrapper.h"
//#include "CvXMLLoadUtility.h"

enum InfoClass
{
	CONCEPT_INFO,
	NEW_CONCEPT_INFO,
	CITY_TAB_INFO,
	CALENDAR_INFO,
	SEASON_INFO,
	MONTH_INFO,
	DENIAL_INFO,
	DOMAIN_INFO,
	ATTITUDE_INFO,
	MEMORY_INFO,
	INTERFACE_MODE_INFO,
	WORLD_INFO,
	CLIMATE_INFO,
	SEA_LEVEL_INFO,
	COLOR_INFO,
	PLAYER_COLOR_INFO,
	ADVISOR_INFO,
	ROUTE_MODEL_INFO,
	RIVER_INFO,
	RIVER_MODEL_INFO,
	WATER_PLANE_INFO,
	TERRAIN_PLANE_INFO,
	CAMERA_OVERLAY_INFO,
	ANIMATION_PATH_INFO,
	ANIMATION_CATEGORY_INFO,
	ENTITY_EVENT_INFO,
	EFFECT_INFO,
	ATTACHABLE_INFO,
	CAMERA_INFO,
	UNIT_FORMATION_INFO,
	LANDSCAPE_INFO,
	TERRAIN_INFO,
	BONUS_CLASS_INFO,
	BONUS_INFO,
	FEATURE_INFO,
	CIVILIZATION_INFO,
	LEADERHEAD_INFO,
	TRAIT_INFO,
	CURSOR_INFO,
	THRONE_ROOM_INFO,
	THRONE_ROOM_STYLE_INFO,
	THRONE_ROOM_CAMERA_INFO,
	SLIDE_SHOW_INFO,
	SLIDE_SHOW_RANDOM_INFO,
	WORLD_PICKER_INFO,
	SPACE_SHIP_INFO,
	UNIT_INFO,
	SPAWN_INFO,
	SPECIAL_UNIT_INFO,
	YIELD_INFO,
	COMMERCE_INFO,
	ROUTE_INFO,
	IMPROVEMENT_INFO,
	GOODY_INFO,
	BUILD_INFO,
	HANDICAP_INFO,
	GAME_SPEED_INFO,
	TURN_TIMER_INFO,
	PROCESS_INFO,
	VOTE_INFO,
	PROJECT_INFO,
	BUILDING_INFO,
	SPECIAL_BUILDING_INFO,
	ACTION_INFO,
	MISSION_INFO,
	CONTROL_INFOS,
	COMMAND_INFO,
	AUTOMATE_INFO,
	PROMOTION_INFO,
	TECH_INFO,
	RELIGION_INFO,
	CORPORATION_INFO,
	SPECIALIST_INFO,
	CIVIC_OPTION_INFO,
	CIVIC_INFO,
	DIPLOMACY_INFO,
	ERA_INFO,
	HURRY_INFO,
	EMPHASIZE_INFO,
	UPKEEP_INFO,
	CULTURE_LEVEL_INFO,
	VICTORY_INFO,
	QUEST_INFO,
	GAME_OPTION_INFO,
	MP_OPTION_INFO,
	FORCE_CONTROL_INFO,
	PLAYER_OPTION_INFO,
	GRAPHIC_OPTION_INFO,
	TUTORIAL_INFO,
	EVENT_TRIGGER_INFO,
	EVENT_INFO,
	ESPIONAGE_MISSION_INFO,
	UNIT_ART_STYLE_TYPE_INFO,
	VOTE_SOURCE_INFO,
	MAIN_MENU_INFO,
	PROPERTY_INFO,
	OUTCOME_INFO,
	UNIT_COMBAT_INFO,
	PROMOTION_LINE_INFO,
	IDEA_CLASS_INFO,
	IDEA_INFO,
	INVISIBLE_INFO,
	MOD_LOAD_CONTROL_INFO,
	MAP_INFO,
	MAP_SWITCH_INFO,
	HINT_INFO,

	//INTERFACE_ART_INFO,
	//MOVIE_ART_INFO,
	//MISC_ART_INFO,
	//UNIT_ART_INFO,
	//BUILDING_ART_INFO,
	//CIVILIZATION_ART_INFO,
	//LEADER_HEAD_ART_INFO,
	//BONUS_ART_INFO,
	//IMPROVEMENT_ART_INFO,
	//TERRAIN_ART_INFO,
	//FEATURE_ART_INFO,

	NUM_INFO_CLASSES
};

template <class CvInfoClass_T>
struct CvInfoArray : bst::noncopyable
{
	~CvInfoArray()
	{
		deleteInfos();
	}

	CvInfoClass_T* operator[](int i) const
	{
		FASSERT_BOUNDS(0, size(), i)
		return m_infos[i];
	}

	int size() const
	{
		return (int)m_infos.size();
	}

	typedef typename std::vector<CvInfoClass_T*>::iterator        iterator;
	typedef typename std::vector<CvInfoClass_T*>::const_iterator  const_iterator;

	iterator begin() { return m_infos.begin(); }
	iterator end()   { return m_infos.end(); }

	const_iterator begin() const { return m_infos.begin(); }
	const_iterator end() const   { return m_infos.end(); }

	void writeClassMappingTable(FDataStreamBase* pStream)
	{
		class_mapping_table_entry entry;
		entry.escapeId = SAVE_ELEMENT_ID_CLASS_MAP;
		entry.classType = classType;
		entry.numClasses = size();
		pStream->Write(sizeof(class_mapping_table_entry), static_cast<uint8_t*>(&entry));
		foreach_(const CvInfoClass_T* info, m_infos)
			pStream->WriteString(info->getType());
	}

private:
	friend class cvInternalGlobals;
	friend class CvXMLLoadUtility;

	void deleteInfos()
	{
		foreach_(const CvInfoClass_T* info, m_infos)
			delete info;
		m_infos.clear();
	}

	void push_back(CvInfoClass_T* info)
	{
		return m_infos.push_back(info);
	}

	void insert(iterator& it, int i, CvInfoClass_T* info)
	{
		return m_infos.insert(it, i, info);
	}

	void load(CvXMLLoadUtility* pXML, const char* szFile, const char* szFolder, const wchar_t* szXmlPath, bool bTwoPass = false)
	{
		//FAssert(m_infos.empty())
		GC.addToInfoArrays(this);
		pXML->LoadGlobalClassInfo(m_infos, szFile, szFolder, szXmlPath, bTwoPass, &m_infoReplacements);
	}

	void readPass3()
	{
		foreach_(CvInfoClass_T* info, m_infos)
		{
			info->readPass3();
			m_infoReplacements.readPass3();
		}
	}

	void updateReplacements()
	{
		m_infoReplacements.updateReplacements(m_infos);
	}

	std::vector<CvInfoClass_T*> m_infos;
	CvInfoReplacements<CvInfoClass_T> m_infoReplacements;
};

#endif