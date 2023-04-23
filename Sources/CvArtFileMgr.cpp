//---------------------------------------------------------------------------------------
//
//  *****************   Civilization IV   ********************
//
//  FILE:    CvArtFileMgr.cpp
//
//  AUTHOR:  Jesse Smith / Mustafa Thamer	9/2004
//
//  PURPOSE: Interfaces with Civ4ArtDefines.xml to manage the paths of art files
//
//---------------------------------------------------------------------------------------
//  Copyright (c) 2004 Firaxis Games, Inc. All rights reserved.
//---------------------------------------------------------------------------------------

#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvArtFileMgr.h"
#include "CvXMLLoadUtility.h"

// Macro for Building Art Info Maps
#define BUILD_INFO_MAP(map, infoArray, numInfos) \
{ \
	PROFILE_EXTRA_FUNC(); for (int iI = 0; iI < numInfos; iI++) \
	{ \
	(map)[infoArray(iI).getTag()] = &infoArray(iI); \
	} \
}

//
// creates a derived artItem class which automatically registers itself with the ARTFILEMGR upon contruction.
// creates a static var of that artItem type which constructs (and registers) at startup.
// creates a getFooArtInfo() function that searches the map based on the id provided and returns the artInfo struct or null.
//
#define ART_INFO_DEFN(name) \
\
class Cv##name##ArtInfoItem : public CvArtFileMgr::ArtInfoItem \
{ \
	void init() { ARTFILEMGR.m_map##name##ArtInfos = new CvArtFileMgr::ArtInfo##name##MapType; } \
	void deInit(); \
	void buildMap() { BUILD_INFO_MAP(*ARTFILEMGR.m_map##name##ArtInfos, ARTFILEMGR.get##name##ArtInfo, ARTFILEMGR.getNum##name##ArtInfos()); } \
}; \
\
static Cv##name##ArtInfoItem* g##name##ArtInfoItem; \
\
CvArtInfo##name * CvArtFileMgr::get##name##ArtInfo( const char *szArtDefineTag ) const \
{ \
	FAssertMsg(szArtDefineTag, "NULL string on art info lookup?"); \
	ArtInfo##name##MapType::const_iterator it = m_map##name##ArtInfos->find( szArtDefineTag );\
	if ( it == m_map##name##ArtInfos->end() ) \
	{\
		char msg[256]; \
		sprintf(msg, "ArtInfo: '%s' was not found", szArtDefineTag); \
		FErrorMsg(msg); \
		if ( 0 == strcmp(szArtDefineTag, "ERROR") ) \
		{ \
			return NULL; \
		} \
		else \
		{ \
			return get##name##ArtInfo( "ERROR" ); \
		} \
	} \
	return it->second; \
} \
void Cv##name##ArtInfoItem::deInit() \
{ \
	PROFILE_EXTRA_FUNC(); SAFE_DELETE(ARTFILEMGR.m_map##name##ArtInfos); \
	for (uint i = 0; i < ARTFILEMGR.m_pa##name##ArtInfo.size(); ++i) \
	{ \
		SAFE_DELETE(ARTFILEMGR.m_pa##name##ArtInfo[i]); \
	} \
	ARTFILEMGR.m_pa##name##ArtInfo.clear(); \
} \
CvArtInfo##name & CvArtFileMgr::get##name##ArtInfo(int i) { return *(m_pa##name##ArtInfo[i]); }

#define ART_INFO_INST(name) g##name##ArtInfoItem = new  Cv##name##ArtInfoItem();


// Macros the declaration of the art file info maps
ART_INFO_DEFN(Asset);
ART_INFO_DEFN(Misc);
ART_INFO_DEFN(Unit);
ART_INFO_DEFN(Building);
ART_INFO_DEFN(Civilization);
ART_INFO_DEFN(Leaderhead);
ART_INFO_DEFN(Bonus);
ART_INFO_DEFN(Improvement);
ART_INFO_DEFN(Terrain);
ART_INFO_DEFN(Feature);
ART_INFO_DEFN(Movie);
ART_INFO_DEFN(Interface);

//----------------------------------------------------------------------------
//
//	FUNCTION:	GetInstance()
//
//	PURPOSE:	Get the instance of this class.
//
//----------------------------------------------------------------------------
static CvArtFileMgr* gs_ArtFileMgr = NULL;

CvArtFileMgr& CvArtFileMgr::GetInstance()
{
	if (gs_ArtFileMgr == NULL)
	{
		gs_ArtFileMgr = new CvArtFileMgr();

		ART_INFO_INST(Asset);
		ART_INFO_INST(Misc);
		ART_INFO_INST(Unit);
		ART_INFO_INST(Building);
		ART_INFO_INST(Civilization);
		ART_INFO_INST(Leaderhead);
		ART_INFO_INST(Bonus);
		ART_INFO_INST(Improvement);
		ART_INFO_INST(Terrain);
		ART_INFO_INST(Feature);
		ART_INFO_INST(Movie);
		ART_INFO_INST(Interface);
	}

	return *gs_ArtFileMgr;
}

//----------------------------------------------------------------------------
//
//	FUNCTION:	Init()
//
//	PURPOSE:	Initializes the Maps
//
//----------------------------------------------------------------------------
void CvArtFileMgr::Init()
{
	PROFILE_EXTRA_FUNC();
	foreach_(ArtInfoItem* item, m_artInfoItems)
	{
		item->init();
	}
}

//----------------------------------------------------------------------------
//
//	FUNCTION:	DeInit()
//
//	PURPOSE:	Deletes the Maps
//
//----------------------------------------------------------------------------
void CvArtFileMgr::DeInit()
{
	PROFILE_EXTRA_FUNC();
	foreach_(ArtInfoItem* item, m_artInfoItems)
	{
		item->deInit();
	}
}

//----------------------------------------------------------------------------
//
//	FUNCTION:	Reset()
//
//	PURPOSE:	Reloads the XML & Rebuilds the Maps
//
//----------------------------------------------------------------------------
void CvArtFileMgr::Reset()
{
	DeInit();		// Cleans Art Defines
	CvXMLLoadUtility XMLLoadUtility;
	XMLLoadUtility.SetGlobalArtDefines();		// Reloads/allocs Art Defines
	Init();			// reallocs maps
	buildArtFileInfoMaps();
}

//----------------------------------------------------------------------------
//
//	FUNCTION:	buildArtFileInfoMaps()
//
//	PURPOSE:	Builds the Art File Maps
//
//----------------------------------------------------------------------------
void CvArtFileMgr::buildArtFileInfoMaps()
{
	PROFILE_EXTRA_FUNC();
	foreach_(ArtInfoItem* item, m_artInfoItems)
	{
		item->buildMap();
	}
}
