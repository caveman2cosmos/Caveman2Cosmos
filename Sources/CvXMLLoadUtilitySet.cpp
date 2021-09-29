//
// XML Set functions
//

#include "CvGameCoreDLL.h"
#include "CvArtFileMgr.h"
#include "CvBuildingInfo.h"
#include "CvGameTextMgr.h"
#include "CvGlobals.h"
#include "CvXMLLoadUtility.h"
#include "CvXMLLoadUtilitySetMod.h"
#include "FVariableSystem.h"
#include "CvImprovementInfo.h"
#include <iostream>
#include "CvInitCore.h"

// Macro for Setting Global Art Defines
#define INIT_XML_GLOBAL_LOAD(xmlInfoPath, infoArray, numInfos)  SetGlobalClassInfo(infoArray, xmlInfoPath, numInfos);

bool CvXMLLoadUtility::ReadGlobalDefines(const TCHAR* szXMLFileName, CvCacheObject* cache)
{
	OutputDebugString("Reading Global Defines: Start");

	bool bLoaded = false;	// used to make sure that the xml file was loaded correctly

	if (!gDLL->cacheRead(cache, szXMLFileName))			// src data file name
	{
		// load normally
		if (!CreateFXml())
		{
			return false;
		}

		// load the new FXml variable with the szXMLFileName file
		bLoaded = LoadCivXml(NULL, szXMLFileName);
		if (!bLoaded)
		{
			char szMessage[1024];
			sprintf( szMessage, "LoadXML call failed for %s \n Current XML file is: %s", szXMLFileName, GC.getCurrentXMLFile().GetCString());
			gDLL->MessageBox(szMessage, "XML Load Error");
		}

		// if the load succeeded we will continue
		else //if (bLoaded)
		{
			// locate the first define tag in the xml
			if (TryMoveToXmlFirstMatchingElement(L"/Civ4Defines/Define"))
			{
				// loop through all the Define tags
				do
				{
					//char szNodeType[256];	// holds the type of the current node
					char szName[256];

					// call the function that sets the FXml pointer to the first non-comment child of
					// the current tag and gets the value of that new node
					if (TryMoveToXmlFirstChild())
					{
						GetXmlVal(szName);
						OutputDebugString(szName);
						// set the FXml pointer to the next sibling of the current tag``
						if (TryMoveToXmlNextSibling())
						{
						// if we successfuly get the node type for the current tag

							OutputDebugStringW(GetXmlTagName());

							switch(GetXmlTagName()[0])
							{
							case L'f':  // get the float value for the define
										float fVal;
										GetXmlVal(&fVal);
										GC.getDefinesVarSystem()->SetValue(szName, fVal);
										break;
							case L'i':  // get the int value for the define
										int iVal;
										GetXmlVal(&iVal);
										GC.getDefinesVarSystem()->SetValue(szName, iVal);
										break;
							case L'b':  // get the boolean value for the define
										bool bVal;
										GetXmlVal(&bVal);
										GC.getDefinesVarSystem()->SetValue(szName, bVal);
										break;
							default:
										char szVal[2048];
										// get the string/text value for the define
										GetXmlVal(szVal);
										GC.getDefinesVarSystem()->SetValue(szName, szVal);
							}

							// since we are looking at the children of a Define tag we will need to go up
							// one level so that we can go to the next Define tag.
							// Set the FXml pointer to the parent of the current tag
							MoveToXmlParent();
						}
					}
				}
				while(TryMoveToXmlNextSibling());


				// write global defines info to cache
				bool bOk = gDLL->cacheWrite(cache);
				if (!bOk)
				{
					char szMessage[1024];
					sprintf( szMessage, "Failed writing to global defines cache. \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
					gDLL->MessageBox(szMessage, "XML Caching Error");
				}
				else
				{
					logging::logMsg("xml.log", "Wrote GlobalDefines to cache\n");
				}
			}
		}

		// delete the pointer to the FXml variable
		//GETXML->DestroyFXml(NULL);
		DestroyFXml();
	}
	else
	{
		logging::logMsg("xml.log", "Read GobalDefines from cache\n");
	}

	OutputDebugString("Reading Global Defines: End\n");

	return true;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalDefines()
//
//  PURPOSE :   Initialize the variables located in globaldefines.cpp/h with the values in
//				GlobalDefines.xml
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::SetGlobalDefines()
{
	OutputDebugString("Setting Global Defines: Start\n");

	bool bLoaded = false;

	UpdateProgressCB("GlobalDefines");

	/////////////////////////////////
	//
	// use disk cache if possible.
	// if no cache or cache is older than xml file, use xml file like normal, else read from cache
	//
	if ( !bLoaded )
	{
		CvCacheObject* cache = gDLL->createGlobalDefinesCacheObject("GlobalDefines.dat");	// cache file name

		DEBUG_LOG("XmlCheckDoubleTypes.log", "\nEntering: GlobalDefines\n");

		if (!ReadGlobalDefines("xml\\GlobalDefines.xml", cache))
		{
			return false;
		}

		if (!ReadGlobalDefines("xml\\GlobalDefinesAlt.xml", cache))
		{
			return false;
		}

		//	Parallel maps
		if (!ReadGlobalDefines("xml\\ParallelMaps_GlobalDefines.xml", cache))
		{
			FErrorMsg("ParallelMaps_GlobalDefines Failed to load!");
		}

		//Affores
		if (!ReadGlobalDefines("xml\\A_New_Dawn_GlobalDefines.xml", cache))
		{
			FErrorMsg("A_New_Dawn_GlobalDefines Failed to load!");
		}
	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                      02/21/10                                jdog5000      */
	/*                                                                                              */
	/* XML Options                                                                                  */
	/************************************************************************************************/
		if (!ReadGlobalDefines("xml\\BBAI_Game_Options_GlobalDefines.xml", cache))
		{
			FErrorMsg("BBAI_Game_Options_GlobalDefines Failed to load!");
		}

		if (!ReadGlobalDefines("xml\\BBAI_AI_Variables_GlobalDefines.xml", cache))
		{
			FErrorMsg("BBAI_AI_Variables_GlobalDefines Failed to load!");
		}

		if (!ReadGlobalDefines("xml\\TechDiffusion_GlobalDefines.xml", cache))
		{
			FErrorMsg("TechDiffusion_GlobalDefines Failed to load!");
		}

	/************************************************************************************************/
	/* BETTER_BTS_AI_MOD                       END                                                  */
	/************************************************************************************************/
	/************************************************************************************************/
	/* TGA_INDEXATION                          02/19/08                                MRGENIE      */
	/*                                                                                              */
	/* reading the Defines to know the modded TGA icons                                             */
	/************************************************************************************************/
		if (!ReadGlobalDefines("res\\Fonts\\GameFont_GlobalDefines.xml", cache))
		{
			FErrorMsg("The \"GameFont_GlobalDefines.xml\" must reside in the \"Mods\\World of Civilization\\Assets\\res\\Fonts\" directory next to the 2 GameFont.tga files");
		}
	/************************************************************************************************/
	/* TGA_INDEXATION                          END                                                  */
	/************************************************************************************************/

		if (gDLL->isModularXMLLoading())
		{
			std::vector<CvString> aszFiles;
			gDLL->enumerateFiles(aszFiles, "modules\\*_GlobalDefines.xml");

			foreach_(const CvString& szFile, aszFiles)
			{
				if (!ReadGlobalDefines(szFile, cache))
				{
					OutputDebugString("Setting Global Defines: End\n");
					return false;
				}
			}
		}
	/************************************************************************************************/
	/* MODULAR_LOADING_CONTROL                 11/15/07                                MRGENIE      */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/
		else
		{
			std::vector<CvString> aszFiles;
			CvXMLLoadUtilitySetMod* pModEnumVector = new CvXMLLoadUtilitySetMod;

			pModEnumVector->loadModControlArray(aszFiles, "globaldefines");

			foreach_(const CvString& szFile, aszFiles)
			{
				if (!ReadGlobalDefines(szFile, cache))
				{
					SAFE_DELETE(pModEnumVector);
					OutputDebugString("Setting Global Defines: End\n");
					return false;
				}
			}
			SAFE_DELETE(pModEnumVector);
			aszFiles.clear();
		}
	/************************************************************************************************/
	/* MODULAR_LOADING_CONTROL                 END                                                  */
	/************************************************************************************************/

		gDLL->destroyCache(cache);
	}
	////////////////////////////////////////////////////////////////////////

	GC.cacheGlobals();

	OutputDebugString("Setting Global Defines: End\n");

	return true;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetPostGlobalsGlobalDefines()
//
//  PURPOSE :   This function assumes that the SetGlobalDefines function has already been called
//							it then loads the few global defines that needed to reference a global variable that
//							hadn't been loaded in prior to the SetGlobalDefines call
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::SetPostGlobalsGlobalDefines()
{
	OutputDebugString("Setting Post Global Defines: Start\n");

	const char* szVal=NULL;		// holds the string value from the define queue
	int idx;

	if (GC.getDefinesVarSystem()->GetSize() > 0)
	{
		SetGlobalDefine("LAND_TERRAIN", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("LAND_TERRAIN", idx);

		SetGlobalDefine("DEEP_WATER_TERRAIN", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DEEP_WATER_TERRAIN", idx);

		SetGlobalDefine("SHALLOW_WATER_TERRAIN", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("SHALLOW_WATER_TERRAIN", idx);
//GWMod Start M.A.
		SetGlobalDefine("FROZEN_TERRAIN", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("FROZEN_TERRAIN", idx);

		SetGlobalDefine("COLD_TERRAIN", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("COLD_TERRAIN", idx);

		SetGlobalDefine("TEMPERATE_TERRAIN", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("TEMPERATE_TERRAIN", idx);

		SetGlobalDefine("DRY_TERRAIN", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("DRY_TERRAIN", idx);

		SetGlobalDefine("BARREN_TERRAIN", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("BARREN_TERRAIN", idx);

		SetGlobalDefine("COLD_FEATURE", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("COLD_FEATURE", idx);

		SetGlobalDefine("TEMPERATE_FEATURE", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("TEMPERATE_FEATURE", idx);

		SetGlobalDefine("WARM_FEATURE", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("WARM_FEATURE", idx);

		SetGlobalDefine("MARSH_TERRAIN", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("MARSH_TERRAIN", idx);

//GWMod end M.A.
		SetGlobalDefine("LAND_IMPROVEMENT", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("LAND_IMPROVEMENT", idx);

		SetGlobalDefine("WATER_IMPROVEMENT", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("WATER_IMPROVEMENT", idx);

		SetGlobalDefine("NUKE_FEATURE", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("NUKE_FEATURE", idx);

		SetGlobalDefine("CAPITAL_BUILDING", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("CAPITAL_BUILDING", idx);

		SetGlobalDefine("INITIAL_CITY_ROUTE_TYPE", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("INITIAL_CITY_ROUTE_TYPE", idx);

		SetGlobalDefine("STANDARD_HANDICAP", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_HANDICAP", idx);

		SetGlobalDefine("STANDARD_HANDICAP_QUICK", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_HANDICAP_QUICK", idx);

		SetGlobalDefine("STANDARD_GAMESPEED", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_GAMESPEED", idx);

		SetGlobalDefine("STANDARD_TURNTIMER", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_TURNTIMER", idx);

		SetGlobalDefine("STANDARD_CLIMATE", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_CLIMATE", idx);

		SetGlobalDefine("STANDARD_SEALEVEL", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_SEALEVEL", idx);

		SetGlobalDefine("STANDARD_ERA", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_ERA", idx);

		SetGlobalDefine("STANDARD_CALENDAR", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("STANDARD_CALENDAR", idx);

		SetGlobalDefine("AI_HANDICAP", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("AI_HANDICAP", idx);

		SetGlobalDefine("BARBARIAN_HANDICAP", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("BARBARIAN_HANDICAP", idx);

		SetGlobalDefine("BARBARIAN_CIVILIZATION", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("BARBARIAN_CIVILIZATION", idx);

		SetGlobalDefine("BARBARIAN_LEADER", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("BARBARIAN_LEADER", idx);

		SetGlobalDefine("NEANDERTHAL_CIVILIZATION", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("NEANDERTHAL_CIVILIZATION", idx);

		SetGlobalDefine("NEANDERTHAL_LEADER", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("NEANDERTHAL_LEADER", idx);

		SetGlobalDefine("BEAST_CIVILIZATION", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("BEAST_CIVILIZATION", idx);

		SetGlobalDefine("PREDATOR_CIVILIZATION", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("PREDATOR_CIVILIZATION", idx);

		SetGlobalDefine("PREY_CIVILIZATION", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("PREY_CIVILIZATION", idx);

		SetGlobalDefine("ANIMAL_LEADER", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("ANIMAL_LEADER", idx);

		SetGlobalDefine("INSECT_CIVILIZATION", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("INSECT_CIVILIZATION", idx);

		SetGlobalDefine("INSECT_LEADER", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("INSECT_LEADER", idx);

		SetGlobalDefine("NPC4_CIVILIZATION", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("NPC4_CIVILIZATION", idx);

		SetGlobalDefine("NPC4_LEADER", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("NPC4_LEADER", idx);

		SetGlobalDefine("NPC3_CIVILIZATION", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("NPC3_CIVILIZATION", idx);

		SetGlobalDefine("NPC3_LEADER", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("NPC3_LEADER", idx);

		SetGlobalDefine("NPC2_CIVILIZATION", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("NPC2_CIVILIZATION", idx);

		SetGlobalDefine("NPC2_LEADER", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("NPC2_LEADER", idx);

		SetGlobalDefine("NPC1_CIVILIZATION", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("NPC1_CIVILIZATION", idx);

		SetGlobalDefine("NPC1_LEADER", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("NPC1_LEADER", idx);

		SetGlobalDefine("NPC0_CIVILIZATION", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("NPC0_CIVILIZATION", idx);

		SetGlobalDefine("NPC0_LEADER", szVal);
		idx = GetInfoClass(szVal);
		GC.getDefinesVarSystem()->SetValue("NPC0_LEADER", idx);

	#define SET_ENUM_GLOBAL_DEFINE(dataType, VAR) \
		SetGlobalDefine(#VAR, szVal); \
		GC.getDefinesVarSystem()->SetValue(#VAR, GetInfoClass(szVal));
		DO_FOR_EACH_ENUM_GLOBAL_DEFINE(SET_ENUM_GLOBAL_DEFINE)

		GC.cacheEnumGlobals();

		gDLL->ChangeINIKeyValue("CONFIG", "HideMinSpecWarning ", "1");
		gDLL->ChangeINIKeyValue("GAME", "ModularLoading  ", "0");

		return true;
	}

	char	szMessage[1024];
	sprintf( szMessage, "Size of Global Defines is not greater than 0. \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
	gDLL->MessageBox(szMessage, "XML Load Error");

	OutputDebugString("Setting Post Global Defines: End\n");

	return false;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalTypes()
//
//  PURPOSE :   Initialize the variables located in globaltypes.cpp/h with the values in
//				GlobalTypes.xml
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::SetGlobalTypes()
{
	OutputDebugString("Setting Global Types: Start\n");

	UpdateProgressCB("GlobalTypes");

	bool bLoaded = false;	// used to make sure that the xml file was loaded correctly

	DEBUG_LOG("XmlCheckDoubleTypes.log", "\nEntering: GlobalTypes\n");

	if (!CreateFXml())
	{
		return false;
	}

	// load the new FXml variable with the GlobalTypes.xml file
	bLoaded = LoadCivXml(NULL, "xml/GlobalTypes.xml");
	if (!bLoaded)
	{
		char	szMessage[1024];
		sprintf( szMessage, "LoadXML call failed for GlobalTypes.xml. \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Load Error");
	}

/************************************************************************************************/
/* XML_CHECK_DOUBLE_TYPE                   03/14/08                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	// Debugging dependencies problems - BEGIN
#ifdef _DEBUG
	GC.logInfoTypeMap("SetGlobalTypes PRE");
#endif
	// Debugging dependencies problems - END
/************************************************************************************************/
/* XML_CHECK_DOUBLE_TYPE                   END                                                  */
/************************************************************************************************/

	// if the load succeeded we will continue
	if (bLoaded)
	{
		SetGlobalStringArray(&GC.getAnimationOperatorTypes(), L"/Civ4Types/AnimationOperatorTypes/AnimationOperatorType", &GC.getNumAnimationOperatorTypes());
		int iEnumVal = NUM_FUNC_TYPES;
		SetGlobalStringArray(&GC.getFunctionTypes(), L"/Civ4Types/FunctionTypes/FunctionType", &iEnumVal, true);
		SetGlobalStringArray(&GC.getFlavorTypes(), L"/Civ4Types/FlavorTypes/FlavorType", &GC.getNumFlavorTypes());
		SetGlobalStringArray(&GC.getArtStyleTypes(), L"/Civ4Types/ArtStyleTypes/ArtStyleType", &GC.getNumArtStyleTypes());
		SetGlobalStringArray(&GC.getCitySizeTypes(), L"/Civ4Types/CitySizeTypes/CitySizeType", &GC.getNumCitySizeTypes());
		iEnumVal = NUM_CONTACT_TYPES;
		SetGlobalStringArray(&GC.getContactTypes(), L"/Civ4Types/ContactTypes/ContactType", &iEnumVal, true);
		iEnumVal = NUM_DIPLOMACYPOWER_TYPES;
		SetGlobalStringArray(&GC.getDiplomacyPowerTypes(), L"/Civ4Types/DiplomacyPowerTypes/DiplomacyPowerType", &iEnumVal, true);
		iEnumVal = NUM_AUTOMATE_TYPES;
		SetGlobalStringArray(&GC.getAutomateTypes(), L"/Civ4Types/AutomateTypes/AutomateType", &iEnumVal, true);
		iEnumVal = NUM_DIRECTION_TYPES;
		SetGlobalStringArray(&GC.getDirectionTypes(), L"/Civ4Types/DirectionTypes/DirectionType", &iEnumVal, true);
		SetGlobalStringArray(&GC.getFootstepAudioTypes(), L"/Civ4Types/FootstepAudioTypes/FootstepAudioType", &GC.getNumFootstepAudioTypes());

		MoveToXmlParent();
		MoveToXmlParent();
		SetVariableListTagPair(&GC.getFootstepAudioTags(), L"FootstepAudioTags", GC.getFootstepAudioTypes(), GC.getNumFootstepAudioTypes(), "");
	}

/************************************************************************************************/
/* XML_CHECK_DOUBLE_TYPE                   03/14/08                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	// Debugging dependencies problems - BEGIN
#ifdef _DEBUG
	GC.logInfoTypeMap("SetGlobalTypes POST");
#endif
	// Debugging dependencies problems - END
/************************************************************************************************/
/* XML_CHECK_DOUBLE_TYPE                   END                                                  */
/************************************************************************************************/
	// delete the pointer to the FXml variable
	DestroyFXml();

	OutputDebugString("Setting Global Types: End\n");

	return true;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetDiplomacyCommentTypes()
//
//  PURPOSE :   Creates a full list of Diplomacy Comments
//
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetDiplomacyCommentTypes(CvString** ppszString, int* iNumVals)
{
	FErrorMsg("should never get here");
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetupGlobalLandscapeInfos()
//
//  PURPOSE :   Initialize the appropriate variables located in globals.cpp/h with the values in
//				Terrain\Civ4TerrainSettings.xml
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::SetupGlobalLandscapeInfo()
{
	if (!CreateFXml())
	{
		return false;
	}

	LoadGlobalClassInfo(GC.m_paLandscapeInfo, "CIV4TerrainSettings", "Terrain", L"/Civ4TerrainSettings/LandscapeInfos/LandscapeInfo", false);

	// delete the pointer to the FXml variable
	DestroyFXml();

	return true;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalArtDefines()
//
//  PURPOSE :   Initialize the appropriate variables located in globals.cpp/h with the values in
//				Civ4ArtDefines.xml
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::SetGlobalArtDefines()
{
	if (!CreateFXml())
	{
		return false;
	}

	LoadGlobalClassInfo(ARTFILEMGR.getInterfaceArtInfo(), "CIV4ArtDefines_Interface", "Art", L"/Civ4ArtDefines/InterfaceArtInfos/InterfaceArtInfo", false);
	LoadGlobalClassInfo(ARTFILEMGR.getMovieArtInfo(), "CIV4ArtDefines_Movie", "Art", L"/Civ4ArtDefines/MovieArtInfos/MovieArtInfo", false);
	LoadGlobalClassInfo(ARTFILEMGR.getMiscArtInfo(), "CIV4ArtDefines_Misc", "Art", L"/Civ4ArtDefines/MiscArtInfos/MiscArtInfo", false);
	LoadGlobalClassInfo(ARTFILEMGR.getUnitArtInfo(), "CIV4ArtDefines_Unit", "Art", L"/Civ4ArtDefines/UnitArtInfos/UnitArtInfo", false);
	LoadGlobalClassInfo(ARTFILEMGR.getBuildingArtInfo(), "CIV4ArtDefines_Building", "Art", L"/Civ4ArtDefines/BuildingArtInfos/BuildingArtInfo", false);
	LoadGlobalClassInfo(ARTFILEMGR.getCivilizationArtInfo(), "CIV4ArtDefines_Civilization", "Art", L"/Civ4ArtDefines/CivilizationArtInfos/CivilizationArtInfo", false);
	LoadGlobalClassInfo(ARTFILEMGR.getLeaderheadArtInfo(), "CIV4ArtDefines_Leaderhead", "Art", L"/Civ4ArtDefines/LeaderheadArtInfos/LeaderheadArtInfo", false);
	LoadGlobalClassInfo(ARTFILEMGR.getBonusArtInfo(), "CIV4ArtDefines_Bonus", "Art", L"/Civ4ArtDefines/BonusArtInfos/BonusArtInfo", false);
	LoadGlobalClassInfo(ARTFILEMGR.getImprovementArtInfo(), "CIV4ArtDefines_Improvement", "Art", L"/Civ4ArtDefines/ImprovementArtInfos/ImprovementArtInfo", false);
	LoadGlobalClassInfo(ARTFILEMGR.getTerrainArtInfo(), "CIV4ArtDefines_Terrain", "Art", L"/Civ4ArtDefines/TerrainArtInfos/TerrainArtInfo", false);
	LoadGlobalClassInfo(ARTFILEMGR.getFeatureArtInfo(), "CIV4ArtDefines_Feature", "Art", L"/Civ4ArtDefines/FeatureArtInfos/FeatureArtInfo", false);

	DestroyFXml();

	return true;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalText()
//
//  PURPOSE :   Handles all Global Text Infos
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::LoadGlobalText()
{
	OutputDebugString("Begin load global text\n");

	// set language tag
	CvString szLanguage = GC.getDefineSTRING("LANGUAGE");
	if (szLanguage == "Default")
	{
		switch (GAMETEXT.getCurrentLanguage())
		{
			case 0:
				szLanguage = "English";
				break;

			case 1:
				szLanguage = "French";
				break;

			case 2:
				szLanguage = "German";
				break;

			case 3:
				szLanguage = "Italian";
				break;

			case 4:
				szLanguage = "Spanish";
				break;

			case 5:
				szLanguage = "Polish";
				break;

			case 6:
				szLanguage = "Russian";
				break;

			default:
				szLanguage = "English";
				break;
		}
	}
	CvGameText::setLanguage(szLanguage);
	logging::logMsg("xml.log", "\nXML language set to %s\n", szLanguage.c_str());
	DEBUG_LOG("XmlCheckDoubleTypes.log", "\nEntering: GameText\n");

	if (!CreateFXml())
	{
		return false;
	}

	//
	// load all files in the xml text directory
	//
	std::vector<CvString> aszFiles;
	std::vector<CvString> aszModfiles;

	gDLL->enumerateFiles(aszFiles, "xml\\GameText\\*.xml");

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 05/23/08                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/* original firaxis
	if (gDLL->isModularXMLLoading())
	{
		gDLL->enumerateFiles(aszModfiles, L"modules\\*_CIV4GameText.xml");
		aszFiles.insert(aszFiles.end(), aszModfiles.begin(), aszModfiles.end());
	}
*/
	gDLL->enumerateFiles(aszModfiles, "modules\\*_CIV4GameText*.xml");
	aszFiles.insert(aszFiles.end(), aszModfiles.begin(), aszModfiles.end());
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
	std::vector<CvGameText> texts;

	foreach_(const CvString& szFile, aszFiles)
	{
		bool bLoaded = LoadCivXml(NULL, szFile); // Load the XML
		if (!bLoaded)
		{
			char	szMessage[1024];
			sprintf( szMessage, "LoadXML call failed for %s. \n Current XML file is: %s", szFile.c_str(), GC.getCurrentXMLFile().GetCString());
			gDLL->MessageBox(szMessage, "XML Load Error");
		}
		if (bLoaded)
		{
			// if the xml is successfully validated
			SetGameText(L"/Civ4GameText", L"/Civ4GameText/TEXT", texts);
		}
	}

	DestroyFXml();

	OutputDebugString("End load global text\n");

	return true;
}

bool CvXMLLoadUtility::LoadBasicInfos()
{
	OutputDebugString("Loading Basic Infos: Start\n");

	if (!CreateFXml())
	{
		return false;
	}

	LoadGlobalClassInfo(GC.m_paConceptInfo, "CIV4BasicInfos", "BasicInfos", L"/Civ4BasicInfos/ConceptInfos/ConceptInfo", false);
	LoadGlobalClassInfo(GC.m_paNewConceptInfo, "CIV4NewConceptInfos", "BasicInfos", L"/Civ4NewConceptInfos/NewConceptInfos/NewConceptInfo", false);
	LoadGlobalClassInfo(GC.m_paCityTabInfo, "CIV4CityTabInfos", "BasicInfos", L"/Civ4CityTabInfos/CityTabInfos/CityTabInfo", false);
	LoadGlobalClassInfo(GC.m_paCalendarInfo, "CIV4CalendarInfos", "BasicInfos", L"/Civ4CalendarInfos/CalendarInfos/CalendarInfo", false);
	LoadGlobalClassInfo(GC.m_paSeasonInfo, "CIV4SeasonInfos", "BasicInfos", L"/Civ4SeasonInfos/SeasonInfos/SeasonInfo", false);
	LoadGlobalClassInfo(GC.m_paMonthInfo, "CIV4MonthInfos", "BasicInfos", L"/Civ4MonthInfos/MonthInfos/MonthInfo", false);
	LoadGlobalClassInfo(GC.m_paDenialInfo, "CIV4DenialInfos", "BasicInfos", L"/Civ4DenialInfos/DenialInfos/DenialInfo", false);
	LoadGlobalClassInfo(GC.m_paDomainInfo, "CIV4DomainInfos", "BasicInfos", L"/Civ4DomainInfos/DomainInfos/DomainInfo", false);
	//	Koshling - replaced XML-based registration of UNITAI types with internal registration.  Since they are a DLL-defined enum
	//	anyway this allows new UNITAIs to be defined freely without ordering issues (in the XML or DLL), which in turn makes it
	//	easier to merge mods with different UNITAI changes.  The XML is retained purely for documentary purposes
	GC.registerUnitAIs();
	GC.registerAIScales();
	GC.registerGameObjects();
	GC.registerGOMs();
	GC.registerRelations();
	GC.registerAttributes();
	GC.registerTags();
	GC.registerPropertySources();
	GC.registerPropertyInteractions();
	GC.registerPropertyPropagators();
	//LoadGlobalClassInfo(GC.m_paUnitAIInfo(), "CIV4UnitAIInfos", "BasicInfos", L"CIV4UnitAIInfos/UnitAIInfos/UnitAIInfo", false);
	LoadGlobalClassInfo(GC.m_paAttitudeInfos, "CIV4AttitudeInfos", "BasicInfos", L"/Civ4AttitudeInfos/AttitudeInfos/AttitudeInfo", false);
	LoadGlobalClassInfo(GC.m_paMemoryInfos, "CIV4MemoryInfos", "BasicInfos", L"/Civ4MemoryInfos/MemoryInfos/MemoryInfo", false);

	DestroyFXml();

	OutputDebugString("Loading Basic Infos: End\n");

	return true;
}

// comparator function for sorting ReligionInfos by TGA index
inline bool cmpReligionTGA(CvReligionInfo* lhs, CvReligionInfo* rhs) { return lhs->getTGAIndex() < rhs->getTGAIndex();}
// comparator function for sorting CorporationInfos by TGA index
inline bool cmpCorporationTGA(CvCorporationInfo* lhs, CvCorporationInfo* rhs) { return lhs->getTGAIndex() < rhs->getTGAIndex();}
// comparator function for sorting InfoBase by TextKey alphabetically
inline bool cmpInfoByAlphabet(CvInfoBase* lhs, CvInfoBase* rhs) { return CvWString::format(lhs->getTextKeyWide()) < CvWString::format(rhs->getTextKeyWide());}
//
// Globals which must be loaded before the main menus.
// Don't put anything in here unless it has to be loaded before the main menus,
// instead try to load things in LoadPostMenuGlobals()
//
bool CvXMLLoadUtility::LoadPreMenuGlobals()
{
	OutputDebugString("Loading PreMenu Infos: Start\n");

	//ReplacementStep: search down here for 'InfoReplacements()'
	if (!CreateFXml())
	{
		return false;
	}

	OutputDebugString("Begin load global infos\n");
	LoadGlobalClassInfo(GC.m_paInvisibleInfo, "CIV4InvisibleInfos", "Units", L"/Civ4InvisibleInfos/InvisibleInfos/InvisibleInfo", false);
	LoadGlobalClassInfo(GC.m_paMapCategoryInfo, "CIV4MapCategoryInfos", "Terrain", L"/Civ4MapCategoryInfos/MapCategoryInfos/MapCategoryInfo", false);
	LoadGlobalClassInfo(GC.m_paMapInfo, "CIV4MapInfo", "GameInfo", L"/Civ4MapInfos/MapInfos/MapInfo", false);
	LoadGlobalClassInfo(GC.m_paGameSpeedInfo, "CIV4GameSpeedInfo", "GameInfo", L"/Civ4GameSpeedInfo/GameSpeedInfos/GameSpeedInfo", false, &GC.m_GameSpeedInfoReplacements);
	LoadGlobalClassInfo(GC.m_paGameOptionInfos, "CIV4GameOptionInfos", "GameInfo", L"/Civ4GameOptionInfos/GameOptionInfos/GameOptionInfo", false);
	LoadGlobalClassInfo(GC.m_paColorInfo, "CIV4ColorVals", "Interface", L"/Civ4ColorVals/ColorVals/ColorVal", false);
	LoadGlobalClassInfo(GC.m_paTurnTimerInfo, "CIV4TurnTimerInfo", "GameInfo", L"/Civ4TurnTimerInfo/TurnTimerInfos/TurnTimerInfo", false);
	LoadGlobalClassInfo(GC.m_paWorldInfo, "CIV4WorldInfo", "GameInfo", L"/Civ4WorldInfo/WorldInfos/WorldInfo", false, &GC.m_WorldInfoReplacements);
	LoadGlobalClassInfo(GC.m_paClimateInfo, "CIV4ClimateInfo", "GameInfo", L"/Civ4ClimateInfo/ClimateInfos/ClimateInfo", false);
	LoadGlobalClassInfo(GC.m_paSeaLevelInfo, "CIV4SeaLevelInfo", "GameInfo", L"/Civ4SeaLevelInfo/SeaLevelInfos/SeaLevelInfo", false);
	LoadGlobalClassInfo(GC.m_paAdvisorInfo, "CIV4AdvisorInfos", "Interface", L"/Civ4AdvisorInfos/AdvisorInfos/AdvisorInfo", false);
	LoadGlobalClassInfo(GC.m_paIdeaClassInfo, "CIV4IdeaClassInfos", "GameInfo", L"/Civ4IdeaClassInfos/IdeaClassInfos/IdeaClassInfo", false);
	LoadGlobalClassInfo(GC.m_paIdeaInfo, "CIV4IdeaInfos", "GameInfo", L"/Civ4IdeaInfos/IdeaInfos/IdeaInfo", false);
	LoadGlobalClassInfo(GC.m_paTerrainInfo, "CIV4TerrainInfos", "Terrain", L"/Civ4TerrainInfos/TerrainInfos/TerrainInfo", false, &GC.m_TerrainInfoReplacements);
	LoadGlobalClassInfo(GC.m_paYieldInfo, "CIV4YieldInfos", "Terrain", L"/Civ4YieldInfos/YieldInfos/YieldInfo", false);
	LoadGlobalClassInfo(GC.m_paCommerceInfo, "CIV4CommerceInfo", "GameInfo", L"/Civ4CommerceInfo/CommerceInfos/CommerceInfo", false);
	LoadGlobalClassInfo(GC.m_aEraInfo, "CIV4EraInfos", "GameInfo", L"/Civ4EraInfos/EraInfos/EraInfo", false, &GC.m_EraInfoReplacements);
	LoadGlobalClassInfo(GC.m_paAnimationCategoryInfo, "CIV4AnimationInfos", "Units", L"/Civ4AnimationInfos/AnimationCategories/AnimationCategory", false);
	LoadGlobalClassInfo(GC.m_paAnimationPathInfo, "CIV4AnimationPathInfos", "Units", L"/Civ4AnimationPathInfos/AnimationPaths/AnimationPath", false);
	LoadGlobalClassInfo(GC.m_paCursorInfo, "CIV4CursorInfo", "GameInfo", L"/Civ4CursorInfo/CursorInfos/CursorInfo", false);
	LoadGlobalClassInfo(GC.m_paCivicOptionInfo, "CIV4CivicOptionInfos", "GameInfo", L"/Civ4CivicOptionInfos/CivicOptionInfos/CivicOptionInfo", false);
	LoadGlobalClassInfo(GC.m_paUpkeepInfo, "CIV4UpkeepInfo", "GameInfo", L"/Civ4UpkeepInfo/UpkeepInfos/UpkeepInfo", false);
	LoadGlobalClassInfo(GC.m_paCultureLevelInfo, "CIV4CultureLevelInfo", "GameInfo", L"/Civ4CultureLevelInfo/CultureLevelInfos/CultureLevelInfo", false, &GC.m_CultureLevelInfoReplacements);
	LoadGlobalClassInfo(GC.m_paBonusClassInfo, "CIV4BonusClassInfos", "Terrain", L"/Civ4BonusClassInfos/BonusClassInfos/BonusClassInfo", false, &GC.m_BonusClassInfoReplacements);
	LoadGlobalClassInfo(GC.m_paVictoryInfo, "CIV4VictoryInfo", "GameInfo", L"/Civ4VictoryInfo/VictoryInfos/VictoryInfo", false);
	LoadGlobalClassInfo(GC.m_paEffectInfo, "CIV4EffectInfos", "Misc", L"/Civ4EffectInfos/EffectInfos/EffectInfo", false);
	LoadGlobalClassInfo(GC.m_paEntityEventInfo, "CIV4EntityEventInfos", "Units", L"/Civ4EntityEventInfos/EntityEventInfos/EntityEventInfo", false);
	LoadGlobalClassInfo(GC.m_paPropertyInfo, "CIV4PropertyInfos", "GameInfo", L"/Civ4PropertyInfos/PropertyInfos/PropertyInfo", false);
	LoadGlobalClassInfo(GC.m_paSpecialistInfo, "CIV4SpecialistInfos", "GameInfo", L"/Civ4SpecialistInfos/SpecialistInfos/SpecialistInfo", false, &GC.m_SpecialistInfoReplacements);
	LoadGlobalClassInfo(GC.m_paVoteSourceInfo, "CIV4VoteSourceInfos", "GameInfo", L"/Civ4VoteSourceInfos/VoteSourceInfos/VoteSourceInfo", false);
	LoadGlobalClassInfo(GC.m_paTechInfo, "CIV4TechInfos", "Technologies", L"/Civ4TechInfos/TechInfos/TechInfo", false, &GC.m_TechInfoReplacements);
	LoadGlobalClassInfo(GC.m_paFeatureInfo, "CIV4FeatureInfos", "Terrain", L"/Civ4FeatureInfos/FeatureInfos/FeatureInfo", false, &GC.m_FeatureInfoReplacements);
	LoadGlobalClassInfo(GC.m_paReligionInfo, "CIV4ReligionInfo", "GameInfo", L"/Civ4ReligionInfo/ReligionInfos/ReligionInfo", false, &GC.m_ReligionInfoReplacements);
	// TGA indexation - important must do before anything else
	std::vector<CvReligionInfo*>& aReligionInfos = GC.m_paReligionInfo;
	std::sort(aReligionInfos.begin(), aReligionInfos.end(), cmpReligionTGA);
	for (int i = 0; i < (int)aReligionInfos.size(); i++)
	{
		GC.setInfoTypeFromString(aReligionInfos.at(i)->getType(), i);
	}

	LoadGlobalClassInfo(GC.m_paBonusInfo, "CIV4BonusInfos", "Terrain", L"/Civ4BonusInfos/BonusInfos/BonusInfo", false, &GC.m_BonusInfoReplacements);
	LoadGlobalClassInfo(GC.m_paSpecialUnitInfo, "CIV4SpecialUnitInfos", "Units", L"/Civ4SpecialUnitInfos/SpecialUnitInfos/SpecialUnitInfo", false);
	shouldHaveType = true;
	LoadGlobalClassInfo(GC.m_paRouteInfo, "CIV4RouteInfos", "Misc", L"/Civ4RouteInfos/RouteInfos/RouteInfo", false, &GC.m_RouteInfoReplacements);
	shouldHaveType = false;
	LoadGlobalClassInfo(GC.m_paImprovementInfo, "CIV4ImprovementInfos", "Terrain", L"/Civ4ImprovementInfos/ImprovementInfos/ImprovementInfo", false, &GC.m_ImprovementInfoReplacements);
	LoadGlobalClassInfo(GC.m_paUnitCombatInfo, "CIV4UnitCombatInfos", "Units", L"/Civ4UnitCombatInfos/UnitCombatInfos/UnitCombatInfo", false);
	//TB Promotion Line Mod begin
	LoadGlobalClassInfo(GC.m_paPromotionLineInfo, "CIV4PromotionLineInfos", "Units", L"/Civ4PromotionLineInfos/PromotionLineInfos/PromotionLineInfo", false);
	//TB Promotion Line Mod begin
	LoadGlobalClassInfo(GC.m_paPromotionInfo, "CIV4PromotionInfos", "Units", L"/Civ4PromotionInfos/PromotionInfos/PromotionInfo", true, &GC.m_PromotionInfoReplacements);
	LoadGlobalClassInfo(GC.m_paHurryInfo, "CIV4HurryInfo", "GameInfo", L"/Civ4HurryInfo/HurryInfos/HurryInfo", false);
	LoadGlobalClassInfo(GC.m_paCorporationInfo, "CIV4CorporationInfo", "GameInfo", L"/Civ4CorporationInfo/CorporationInfos/CorporationInfo", false, &GC.m_CorporationInfoReplacements);
	// TGA indexation - important must do before anything else
	std::vector<CvCorporationInfo*>& aCorporationInfos = GC.m_paCorporationInfo;
	std::sort(aCorporationInfos.begin(), aCorporationInfos.end(), cmpCorporationTGA);
	for (int i = 0; i < (int)aCorporationInfos.size(); i++)
	{
		GC.setInfoTypeFromString(aCorporationInfos.at(i)->getType(), i);
	}

	LoadGlobalClassInfo(GC.m_paSpecialBuildingInfo, "CIV4SpecialBuildingInfos", "Buildings", L"/Civ4SpecialBuildingInfos/SpecialBuildingInfos/SpecialBuildingInfo", false, &GC.m_SpecialBuildingInfoReplacements);
	LoadGlobalClassInfo(GC.m_paBuildingInfo, "CIV4BuildingInfos", "Buildings", L"/Civ4BuildingInfos/BuildingInfos/BuildingInfo", true, &GC.m_BuildingInfoReplacements);
	LoadGlobalClassInfo(GC.m_paCivicInfo, "CIV4CivicInfos", "GameInfo", L"/Civ4CivicInfos/CivicInfos/CivicInfo", false, &GC.m_CivicInfoReplacements);
	LoadGlobalClassInfo(GC.m_paPlayerColorInfo, "CIV4PlayerColorInfos", "Interface", L"/Civ4PlayerColorInfos/PlayerColorInfos/PlayerColorInfo", false);
	LoadGlobalClassInfo(GC.m_paBuildInfo, "CIV4BuildInfos", "Units", L"/Civ4BuildInfos/BuildInfos/BuildInfo", false, &GC.m_BuildInfoReplacements);
	LoadGlobalClassInfo(GC.m_paOutcomeInfo, "CIV4OutcomeInfos", "GameInfo", L"/Civ4OutcomeInfos/OutcomeInfos/OutcomeInfo", false);

	//	AlbertS2: Register mission types
	GC.registerMissions();
	LoadGlobalClassInfo(GC.m_paMissionInfo, "CIV4MissionInfos", "Units", L"/Civ4MissionInfos/MissionInfos/MissionInfo", false);
	LoadGlobalClassInfo(GC.m_paUnitInfo, "CIV4UnitInfos", "Units", L"/Civ4UnitInfos/UnitInfos/UnitInfo", true, &GC.m_UnitInfoReplacements);
	LoadGlobalClassInfo(GC.m_paTraitInfo, "CIV4TraitInfos", "Civilizations", L"/Civ4TraitInfos/TraitInfos/TraitInfo", false, &GC.m_TraitInfoReplacements);
	LoadGlobalClassInfo(GC.m_paLeaderHeadInfo, "CIV4LeaderHeadInfos", "Civilizations", L"/Civ4LeaderHeadInfos/LeaderHeadInfos/LeaderHeadInfo", false, &GC.m_LeaderHeadInfoReplacements);
/************************************************************************************************/
/* SORT_ALPHABET                           11/17/07                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	OutputDebugString("Pre leaderhead sort\n");
	std::sort(GC.m_paLeaderHeadInfo.begin(), GC.m_paLeaderHeadInfo.end(), cmpInfoByAlphabet);
	for (int i = 0; i < GC.getNumLeaderHeadInfos(); i++)
	{
		GC.setInfoTypeFromString(GC.getLeaderHeadInfo((LeaderHeadTypes)i).getType(), i);
	}
	OutputDebugString("Post leaderhead sort\n");
/************************************************************************************************/
/* SORT_ALPHABET                           END                                                  */
/************************************************************************************************/

	LoadGlobalClassInfo(GC.m_paUnitArtStyleTypeInfo, "CIV4UnitArtStyleTypeInfos", "Civilizations", L"/Civ4UnitArtStyleTypeInfos/UnitArtStyleTypeInfos/UnitArtStyleTypeInfo", false);
	LoadGlobalClassInfo(GC.m_paCivilizationInfo, "CIV4CivilizationInfos", "Civilizations", L"/Civ4CivilizationInfos/CivilizationInfos/CivilizationInfo", false, &GC.m_CivilizationInfoReplacements);

	LoadGlobalClassInfo(GC.m_paProjectInfo, "CIV4ProjectInfo", "GameInfo", L"/Civ4ProjectInfo/ProjectInfos/ProjectInfo", false, &GC.m_ProjectInfoReplacements);

/************************************************************************************************/
/* SORT_ALPHABET                           11/17/07                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	OutputDebugString("Pre civ sort\n");
	std::sort(GC.m_paCivilizationInfo.begin(), GC.m_paCivilizationInfo.end(), cmpInfoByAlphabet);
	for (int i = 0; i < GC.getNumCivilizationInfos(); i++)
	{
		GC.setInfoTypeFromString(GC.getCivilizationInfo((CivilizationTypes)i).getType(), i);
	}
	OutputDebugString("Post civ sort\n");
/************************************************************************************************/
/* SORT_ALPHABET                           END                                                  */
/************************************************************************************************/
	LoadGlobalClassInfo(GC.m_paHints, "CIV4Hints", "GameInfo", L"/Civ4Hints/HintInfos/HintInfo", false);
	LoadGlobalClassInfo(GC.m_paMainMenus, "CIV4MainMenus", "Art", L"/Civ4MainMenus/MainMenus/MainMenu", false);
	LoadGlobalClassInfo(GC.m_paSlideShowInfo, "CIV4SlideShowInfos", "Interface", L"/Civ4SlideShowInfos/SlideShowInfos/SlideShowInfo", false);
	LoadGlobalClassInfo(GC.m_paSlideShowRandomInfo, "CIV4SlideShowRandomInfos", "Interface", L"/Civ4SlideShowRandomInfos/SlideShowRandomInfos/SlideShowRandomInfo", false);
	LoadGlobalClassInfo(GC.m_paWorldPickerInfo, "CIV4WorldPickerInfos", "Interface", L"/Civ4WorldPickerInfos/WorldPickerInfos/WorldPickerInfo", false);
	LoadGlobalClassInfo(GC.m_paSpaceShipInfo, "CIV4SpaceShipInfos", "Interface", L"/Civ4SpaceShipInfos/SpaceShipInfos/SpaceShipInfo", false);

	shouldHaveType = true;
	LoadGlobalClassInfo(GC.m_paGoodyInfo, "CIV4GoodyInfo", "GameInfo", L"/Civ4GoodyInfo/GoodyInfos/GoodyInfo", false);
	LoadGlobalClassInfo(GC.m_paHandicapInfo, "CIV4HandicapInfo", "GameInfo", L"/Civ4HandicapInfo/HandicapInfos/HandicapInfo", false, &GC.m_HandicapInfoReplacements);
	shouldHaveType = false;

	LoadGlobalClassInfo(GC.m_paMPOptionInfos, "CIV4MPOptionInfos", "GameInfo", L"/Civ4MPOptionInfos/MPOptionInfos/MPOptionInfo", false);
	LoadGlobalClassInfo(GC.m_paForceControlInfos, "CIV4ForceControlInfos", "GameInfo", L"/Civ4ForceControlInfos/ForceControlInfos/ForceControlInfo", false);

	LoadGlobalClassInfo(GC.m_paEventInfo, "CIV4EventInfos", "Events", L"/Civ4EventInfos/EventInfos/EventInfo", false, &GC.m_EventInfoReplacements);
	LoadGlobalClassInfo(GC.m_paEventTriggerInfo, "CIV4EventTriggerInfos", "Events", L"/Civ4EventTriggerInfos/EventTriggerInfos/EventTriggerInfo", false, &GC.m_EventTriggerInfoReplacements);
	OutputDebugString("Base load complete\n");

	// AIAndy: Resolve delayed resolutions of type strings
	GC.resolveDelayedResolution();
	OutputDebugString("Delayed resolution resolved\n");

	for (int i=0; i < GC.getNumBuildingInfos(); ++i)
	{
		GC.getBuildingInfo((BuildingTypes)i).readPass3();
		GC.m_BuildingInfoReplacements.readPass3();
	}

	for (int i=0; i < GC.getNumCivicInfos(); ++i)
	{
		GC.getCivicInfo((CivicTypes)i).readPass3();
		GC.m_CivicInfoReplacements.readPass3();
	}

	for (int i=0; i < GC.getNumCorporationInfos(); ++i)
	{
		GC.getCorporationInfo((CorporationTypes)i).readPass3();
		GC.m_CorporationInfoReplacements.readPass3();
	}

	for (int i=0; i < GC.getNumEventInfos(); ++i)
	{
		GC.getEventInfo((EventTypes)i).readPass3();
		GC.m_EventInfoReplacements.readPass3();
	}

	for (int i=0; i < GC.getNumProjectInfos(); ++i)
	{
		GC.getProjectInfo((ProjectTypes)i).readPass3();
		GC.m_ProjectInfoReplacements.readPass3();
	}

	for (int i=0; i < GC.getNumUnitInfos(); ++i)
	{
		GC.getUnitInfo((UnitTypes)i).readPass3();
		GC.m_UnitInfoReplacements.readPass3();
	}
	OutputDebugString("Pass3 processing complete\n");

	// add types to global var system
	for (int i = 0; i < GC.getNumCursorInfos(); ++i)
	{
		int iVal;
		const CvString szType = GC.getCursorInfo((CursorTypes)i).getType();
		if (GC.getDefinesVarSystem()->GetValue(szType, iVal))
		{
			char szMessage[1024];
			char* tmp = xercesc::XMLString::transcode(m_pParser->getDocument()->getDocumentURI());
			sprintf(szMessage, "cursor type already set? \n Current XML file is: %s", tmp);
				// GC.getCurrentXMLFile().GetCString()); HERE
			xercesc::XMLString::release(&tmp);
			gDLL->MessageBox(szMessage, "XML Error");
		}
		GC.getDefinesVarSystem()->SetValue(szType, i);
	}

	// Check Playables
	for (int i=0; i < GC.getNumCivilizationInfos(); ++i)
	{
		// if the civilization is playable we will increment the playable var
		if (GC.getCivilizationInfo((CivilizationTypes) i).isPlayable())
		{
			GC.getNumPlayableCivilizationInfos() += 1;
		}

		// if the civilization is playable by AI increments num playable
		if (GC.getCivilizationInfo((CivilizationTypes) i).isAIPlayable())
		{
			GC.getNumAIPlayableCivilizationInfos() += 1;
		}
	}

	GC.getInitCore().checkInitialCivics();


	//Establish Promotion Pedia Help info
	for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
	{
		PromotionTypes ePromotion = (PromotionTypes)iI;
		GC.getPromotionInfo(ePromotion).setQualifiedUnitCombatTypes();
	}
	for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		UnitTypes eUnit = (UnitTypes)iI;
		GC.getUnitInfo(eUnit).setQualifiedPromotionTypes();
		GC.getUnitInfo(eUnit).setCanAnimalIgnores();
	}

	GC.doPostLoadCaching();

	// Add TGA space fillers
	CvReligionInfo* pReligionBogus = new CvReligionInfo();
	aReligionInfos.insert(aReligionInfos.end(), GC.getGAMEFONT_TGA_RELIGIONS() - aReligionInfos.size(), pReligionBogus);
	CvCorporationInfo* pCorporationBogus = new CvCorporationInfo();
	aCorporationInfos.insert(aCorporationInfos.end(), GC.getGAMEFONT_TGA_CORPORATIONS() - aCorporationInfos.size(), pCorporationBogus);

	OutputDebugString("Load globals complete\n");
	UpdateProgressCB("GlobalOther");

	DestroyFXml();

	OutputDebugString("Loading PreMenu Infos: End\n");

	return true;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   LoadPostMenuGlobals()
//
//  PURPOSE :   loads global xml data which isn't needed for the main menus
//		this data is loaded as a secodn stage, when the game is launched
//
//------------------------------------------------------------------------------------------------------
bool CvXMLLoadUtility::LoadPostMenuGlobals()
{
	PROFILE_FUNC();

	OutputDebugString("Loading PostMenu Infos: Start");

	if (!CreateFXml())
	{
		return false;
	}

	//throne room disabled
	UpdateProgressCB("Global Throne Room");

	LoadGlobalClassInfo(GC.m_paThroneRoomCamera, "CIV4ThroneRoomCameraInfos", "Interface", L"/Civ4ThroneRoomCameraInfos/ThroneRoomCameraInfos/ThroneRoomCamera", false);
	LoadGlobalClassInfo(GC.m_paThroneRoomInfo, "CIV4ThroneRoomInfos", "Interface", L"/Civ4ThroneRoomInfos/ThroneRoomInfos/ThroneRoomInfo", false);
	LoadGlobalClassInfo(GC.m_paThroneRoomStyleInfo, "CIV4ThroneRoomStyleInfos", "Interface", L"/Civ4ThroneRoomStyleInfos/ThroneRoomStyleInfos/ThroneRoomStyleInfo", false);

	UpdateProgressCB("Global Events");
	UpdateProgressCB("Global Routes");

	LoadGlobalClassInfo(GC.m_paRouteModelInfo, "CIV4RouteModelInfos", "Art", L"/Civ4RouteModelInfos/RouteModelInfos/RouteModelInfo", false);

	UpdateProgressCB("Global Rivers");

	LoadGlobalClassInfo(GC.m_paRiverModelInfo, "CIV4RiverModelInfos", "Art", L"/Civ4RiverModelInfos/RiverModelInfos/RiverModelInfo", false);

	UpdateProgressCB("Global Other");

	LoadGlobalClassInfo(GC.m_paWaterPlaneInfo, "CIV4WaterPlaneInfos", "Misc", L"/Civ4WaterPlaneInfos/WaterPlaneInfos/WaterPlaneInfo", false);
	LoadGlobalClassInfo(GC.m_paTerrainPlaneInfo, "CIV4TerrainPlaneInfos", "Misc", L"/Civ4TerrainPlaneInfos/TerrainPlaneInfos/TerrainPlaneInfo", false);
	LoadGlobalClassInfo(GC.m_paCameraOverlayInfo, "CIV4CameraOverlayInfos", "Misc", L"/Civ4CameraOverlayInfos/CameraOverlayInfos/CameraOverlayInfo", false);


	UpdateProgressCB("Global Process");

	LoadGlobalClassInfo(GC.m_paProcessInfo, "CIV4ProcessInfo", "GameInfo", L"/Civ4ProcessInfo/ProcessInfos/ProcessInfo", false, &GC.m_ProcessInfoReplacements);

	UpdateProgressCB("Global Emphasize");

	LoadGlobalClassInfo(GC.m_paEmphasizeInfo, "CIV4EmphasizeInfo", "GameInfo", L"/Civ4EmphasizeInfo/EmphasizeInfos/EmphasizeInfo", false);

	UpdateProgressCB("Global Other");

	LoadGlobalClassInfo(GC.m_paControlInfo, "CIV4ControlInfos", "Units", L"/Civ4ControlInfos/ControlInfos/ControlInfo", false);
	LoadGlobalClassInfo(GC.m_paCommandInfo, "CIV4CommandInfos", "Units", L"/Civ4CommandInfos/CommandInfos/CommandInfo", false);
	LoadGlobalClassInfo(GC.m_paAutomateInfo, "CIV4AutomateInfos", "Units", L"/Civ4AutomateInfos/AutomateInfos/AutomateInfo", false);

	UpdateProgressCB("Global Vote");

	LoadGlobalClassInfo(GC.m_paVoteInfo, "CIV4VoteInfo", "GameInfo", L"/Civ4VoteInfo/VoteInfos/VoteInfo", false);

	UpdateProgressCB("Global Interface");

	LoadGlobalClassInfo(GC.m_paInterfaceModeInfo, "CIV4InterfaceModeInfos", "Interface", L"/Civ4InterfaceModeInfos/InterfaceModeInfos/InterfaceModeInfo", false);

	SetGlobalActionInfo();

	// Load the formation info
	LoadGlobalClassInfo(GC.m_paUnitFormationInfo, "CIV4FormationInfos", "Units", L"/UnitFormations/UnitFormation", false);

	// Load the attachable infos
	LoadGlobalClassInfo(GC.m_paAttachableInfo, "CIV4AttachableInfos", "Misc", L"/Civ4AttachableInfos/AttachableInfos/AttachableInfo", false);

	// Special Case Diplomacy Info due to double vectored nature and appending of Responses
	LoadDiplomacyInfo(GC.m_paDiplomacyInfo, "CIV4DiplomacyInfos", "GameInfo", L"/Civ4DiplomacyInfos/DiplomacyInfos/DiplomacyInfo", true);

	LoadGlobalClassInfo(GC.m_paEspionageMissionInfo, "CIV4EspionageMissionInfo", "GameInfo", L"/Civ4EspionageMissionInfo/EspionageMissionInfos/EspionageMissionInfo", false);

	// TB: Moved to enable PlayerTypes to load with Spawn Infos.  Spawn infos aren't called by any other class anyhow.
	LoadGlobalClassInfo(GC.m_paSpawnInfo, "CIV4SpawnInfos", "Units", L"/Civ4SpawnInfos/SpawnInfos/SpawnInfo", false, &GC.m_SpawnInfoReplacements);
	DestroyFXml();

	GC.getInitCore().calculateAssetCheckSum();

	GC.cacheInfoTypes();

	OutputDebugString("Loading PostMenu Infos: End");

	return true;
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalStringArray(TCHAR (**ppszString)[256], char* szTagName, int* iNumVals)
//
//  PURPOSE :   takes the szTagName parameter and if it finds it in the currently selected XML element
//				then it loads the ppszString parameter with the string values under it and the
//				iNumVals with the total number of tags with the szTagName in the xml file
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetGlobalStringArray(CvString **ppszString, wchar_t* szTagName, int* iNumVals, bool bUseEnum)
{
	PROFILE_FUNC();
	logging::logMsgW("xml.log", L"SetGlobalStringArray %s\n", szTagName);

	int i=0;					//loop counter
	CvString *pszString;	// hold the local pointer to the newly allocated string memory
	pszString = NULL;			// null out the local string pointer so that it can be checked at the
	// end of the function in an FAssert

	// if we locate the szTagName, the current node is set to the first instance of the tag name in the xml file
	if (TryMoveToXmlFirstMatchingElement(szTagName))
	{
		if (!bUseEnum)
		{
			// get the total number of times this tag appears in the xml
			//*iNumVals = GETXML->NumOfElementsByTagName(NULL,szTagName); WTF?
			*iNumVals = GetXmlSiblingsNumber(GetXmlTagName());
		}
		// initialize the memory based on the total number of tags in the xml and the 256 character length we selected
		*ppszString = new CvString[*iNumVals];
		// set the local pointer to the memory just allocated
		pszString = *ppszString;

		// loop through each of the tags
		for (i=0;i<*iNumVals;i++)
		{
			// get the string value at the current node
			GetXmlVal(pszString[i]);
			GC.setTypesEnum(pszString[i], i);
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 05/29/08                                MRGENIE      */
/*                                                                                              */
/* adding to hash map - used for the dependencies                                               */
/************************************************************************************************/
			DEBUG_LOG("xml.log", " Adding info type %s with ID %i", pszString[i].c_str(), i);
			GC.setInfoTypeFromString(pszString[i], i);
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/

			// if can't set the current node to a sibling node we will break out of the for loop
			// otherwise we will keep looping
			if (!TryMoveToXmlNextSibling())
			{
				break;
			}
		}
	}

	// if the local string pointer is null then we weren't able to find the szTagName in the xml
	// so we will FAssert to let whoever know it
	if (!pszString)
	{
		char	szMessage[1024];
		sprintf( szMessage, "Error locating tag node in SetGlobalStringArray function \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
}




//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalActionInfo(CvActionInfo** ppActionInfo, int* iNumVals)
//
//  PURPOSE :   Takes the szTagName parameter and if it exists in the xml it loads the ppActionInfo
//				with the value under it and sets the value of the iNumVals parameter to the total number
//				of occurances of the szTagName tag in the xml.
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetGlobalActionInfo()
{
	PROFILE_FUNC();
	logging::logMsg("xml.log", "SetGlobalActionInfo\n");
	int i=0;					//loop counter

	if(!(NUM_INTERFACEMODE_TYPES > 0))
	{
		char	szMessage[1024];
		sprintf( szMessage, "NUM_INTERFACE_TYPES is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(GC.getNumBuildInfos() > 0))
	{
		char	szMessage[1024];
		sprintf( szMessage, "GC.getNumBuildInfos() is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(GC.getNumPromotionInfos() > 0))
	{
		char	szMessage[1024];
		sprintf( szMessage, "GC.getNumPromotionInfos() is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(GC.getNumSpecialistInfos() > 0) )
	{
		char	szMessage[1024];
		sprintf( szMessage, "GC.getNumSpecialistInfos() is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(GC.getNumBuildingInfos() > 0) )
	{
		char	szMessage[1024];
		sprintf( szMessage, "GC.getNumBuildingInfos() is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(NUM_CONTROL_TYPES > 0) )
	{
		char	szMessage[1024];
		sprintf( szMessage, "NUM_CONTROL_TYPES is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(GC.getNumAutomateInfos() > 0) )
	{
		char	szMessage[1024];
		sprintf( szMessage, "GC.getNumAutomateInfos() is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(NUM_COMMAND_TYPES > 0) )
	{
		char	szMessage[1024];
		sprintf( szMessage, "NUM_COMMAND_TYPES is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if(!(GC.getNumMissionInfos() > 0) )
	{
		char	szMessage[1024];
		sprintf( szMessage, "GC.getNumMissionInfos() is not greater than zero in CvXMLLoadUtility::SetGlobalActionInfo \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}

	int* piOrderedIndex=NULL;

	int iNumOrigVals = GC.getNumActionInfos();

	int iNumActionInfos = iNumOrigVals +
		NUM_INTERFACEMODE_TYPES +
		GC.getNumBuildInfos() +
		GC.getNumPromotionInfos() +
		GC.getNumReligionInfos() +
		GC.getNumCorporationInfos() +
		GC.getNumUnitInfos() +
		GC.getNumSpecialistInfos() +
		GC.getNumBuildingInfos() +
		NUM_CONTROL_TYPES +
		NUM_COMMAND_TYPES +
		GC.getNumAutomateInfos() +
		GC.getNumMissionInfos();

	int* piIndexList = new int[iNumActionInfos];
	int* piPriorityList = new int[iNumActionInfos];
	int* piActionInfoTypeList = new int[iNumActionInfos];

	int iTotalActionInfoCount = 0;

	// loop through control info
	for (i=0;i<NUM_COMMAND_TYPES;i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getCommandInfo((CommandTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_COMMAND;
		iTotalActionInfoCount++;
	}

	for (i=0;i<NUM_INTERFACEMODE_TYPES;i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getInterfaceModeInfo((InterfaceModeTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_INTERFACEMODE;
		iTotalActionInfoCount++;
	}

	for (i=0;i<GC.getNumBuildInfos();i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getBuildInfo((BuildTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_BUILD;
		iTotalActionInfoCount++;
	}

	for (i=0;i<GC.getNumPromotionInfos();i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getPromotionInfo((PromotionTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_PROMOTION;
		iTotalActionInfoCount++;
	}

	for (i=0;i<GC.getNumUnitInfos();i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getUnitInfo((UnitTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_UNIT;
		iTotalActionInfoCount++;
	}

	for (i=0;i<GC.getNumReligionInfos();i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getReligionInfo((ReligionTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_RELIGION;
		iTotalActionInfoCount++;
	}

	for (i=0;i<GC.getNumCorporationInfos();i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getCorporationInfo((CorporationTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_CORPORATION;
		iTotalActionInfoCount++;
	}

	for (i=0;i<GC.getNumSpecialistInfos();i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getSpecialistInfo((SpecialistTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_SPECIALIST;
		iTotalActionInfoCount++;
	}

	for (i=0;i<GC.getNumBuildingInfos();i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getBuildingInfo((BuildingTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_BUILDING;
		iTotalActionInfoCount++;
	}

	for (i=0;i<NUM_CONTROL_TYPES;i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getControlInfo((ControlTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_CONTROL;
		iTotalActionInfoCount++;
	}

	for (i=0;i<GC.getNumAutomateInfos();i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getAutomateInfo(i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_AUTOMATE;
		iTotalActionInfoCount++;
	}

	for (i=0;i<GC.getNumMissionInfos();i++)
	{
		piIndexList[iTotalActionInfoCount] = i;
		piPriorityList[iTotalActionInfoCount] = GC.getMissionInfo((MissionTypes)i).getOrderPriority();
		piActionInfoTypeList[iTotalActionInfoCount] = ACTIONSUBTYPE_MISSION;
		iTotalActionInfoCount++;
	}

	SAFE_DELETE_ARRAY(piOrderedIndex);
	piOrderedIndex = new int[iNumActionInfos];

	orderHotkeyInfo(&piOrderedIndex, piPriorityList, iNumActionInfos);
	for (i=0;i<iNumActionInfos;i++)
	{
		CvActionInfo* pActionInfo = new CvActionInfo;
		pActionInfo->setOriginalIndex(piIndexList[piOrderedIndex[i]]);
		pActionInfo->setSubType((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]]);
		if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_COMMAND)
		{
			GC.getCommandInfo((CommandTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_INTERFACEMODE)
		{
			GC.getInterfaceModeInfo((InterfaceModeTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_BUILD)
		{
			GC.getBuildInfo((BuildTypes)piIndexList[piOrderedIndex[i]]).setMissionType(GetInfoClass("MISSION_BUILD"));
			GC.getBuildInfo((BuildTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_PROMOTION)
		{
			if (GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).isEquipment())
			{
				GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).setCommandType(GetInfoClass("COMMAND_REEQUIP"));
				GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
				GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).setHotKeyDescription(GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).getTextKeyWide(), GC.getCommandInfo((CommandTypes)(GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).getCommandType())).getTextKeyWide(), CreateHotKeyFromDescription(GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).getHotKey(), GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).isShiftDown(), GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).isAltDown(), GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).isCtrlDown()));
			}
			else if(GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).isStatus())
			{
				GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).setCommandType(GetInfoClass("COMMAND_STATUS"));
				GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
				GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).setHotKeyDescription(GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).getTextKeyWide(), GC.getCommandInfo((CommandTypes)(GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).getCommandType())).getTextKeyWide(), CreateHotKeyFromDescription(GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).getHotKey(), GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).isShiftDown(), GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).isAltDown(), GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).isCtrlDown()));
			}
			else
			{
				GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).setCommandType(GetInfoClass("COMMAND_PROMOTION"));
				GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
				GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).setHotKeyDescription(GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).getTextKeyWide(), GC.getCommandInfo((CommandTypes)(GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).getCommandType())).getTextKeyWide(), CreateHotKeyFromDescription(GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).getHotKey(), GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).isShiftDown(), GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).isAltDown(), GC.getPromotionInfo((PromotionTypes)piIndexList[piOrderedIndex[i]]).isCtrlDown()));
			}
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_UNIT)
		{
			GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).setCommandType(GetInfoClass("COMMAND_UPGRADE"));
			GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
			GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).setHotKeyDescription(GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).getTextKeyWide(), GC.getCommandInfo((CommandTypes)(GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).getCommandType())).getTextKeyWide(), CreateHotKeyFromDescription(GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).getHotKey(), GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).isShiftDown(), GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).isAltDown(), GC.getUnitInfo((UnitTypes)piIndexList[piOrderedIndex[i]]).isCtrlDown()));
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_RELIGION)
		{
			GC.getReligionInfo((ReligionTypes)piIndexList[piOrderedIndex[i]]).setMissionType(GetInfoClass("MISSION_SPREAD"));
			GC.getReligionInfo((ReligionTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
			GC.getReligionInfo((ReligionTypes)piIndexList[piOrderedIndex[i]]).setHotKeyDescription(GC.getReligionInfo((ReligionTypes)piIndexList[piOrderedIndex[i]]).getTextKeyWide(), GC.getMissionInfo((MissionTypes)(GC.getReligionInfo((ReligionTypes)piIndexList[piOrderedIndex[i]]).getMissionType())).getTextKeyWide(), CreateHotKeyFromDescription(GC.getReligionInfo((ReligionTypes)piIndexList[piOrderedIndex[i]]).getHotKey(), GC.getReligionInfo((ReligionTypes)piIndexList[piOrderedIndex[i]]).isShiftDown(), GC.getReligionInfo((ReligionTypes)piIndexList[piOrderedIndex[i]]).isAltDown(), GC.getReligionInfo((ReligionTypes)piIndexList[piOrderedIndex[i]]).isCtrlDown()));
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_CORPORATION)
		{
			GC.getCorporationInfo((CorporationTypes)piIndexList[piOrderedIndex[i]]).setMissionType(GetInfoClass("MISSION_SPREAD_CORPORATION"));
			GC.getCorporationInfo((CorporationTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
			GC.getCorporationInfo((CorporationTypes)piIndexList[piOrderedIndex[i]]).setHotKeyDescription(GC.getCorporationInfo((CorporationTypes)piIndexList[piOrderedIndex[i]]).getTextKeyWide(), GC.getMissionInfo((MissionTypes)(GC.getCorporationInfo((CorporationTypes)piIndexList[piOrderedIndex[i]]).getMissionType())).getTextKeyWide(), CreateHotKeyFromDescription(GC.getCorporationInfo((CorporationTypes)piIndexList[piOrderedIndex[i]]).getHotKey(), GC.getCorporationInfo((CorporationTypes)piIndexList[piOrderedIndex[i]]).isShiftDown(), GC.getCorporationInfo((CorporationTypes)piIndexList[piOrderedIndex[i]]).isAltDown(), GC.getCorporationInfo((CorporationTypes)piIndexList[piOrderedIndex[i]]).isCtrlDown()));
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_SPECIALIST)
		{
			GC.getSpecialistInfo((SpecialistTypes)piIndexList[piOrderedIndex[i]]).setMissionType(GetInfoClass("MISSION_JOIN"));
			GC.getSpecialistInfo((SpecialistTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
			GC.getSpecialistInfo((SpecialistTypes)piIndexList[piOrderedIndex[i]]).setHotKeyDescription(GC.getSpecialistInfo((SpecialistTypes)piIndexList[piOrderedIndex[i]]).getTextKeyWide(), GC.getMissionInfo((MissionTypes)(GC.getSpecialistInfo((SpecialistTypes)piIndexList[piOrderedIndex[i]]).getMissionType())).getTextKeyWide(), CreateHotKeyFromDescription(GC.getSpecialistInfo((SpecialistTypes)piIndexList[piOrderedIndex[i]]).getHotKey(), GC.getSpecialistInfo((SpecialistTypes)piIndexList[piOrderedIndex[i]]).isShiftDown(), GC.getSpecialistInfo((SpecialistTypes)piIndexList[piOrderedIndex[i]]).isAltDown(), GC.getSpecialistInfo((SpecialistTypes)piIndexList[piOrderedIndex[i]]).isCtrlDown()));
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_BUILDING)
		{
			GC.getBuildingInfo((BuildingTypes)piIndexList[piOrderedIndex[i]]).setMissionType(GetInfoClass("MISSION_CONSTRUCT"));
			GC.getBuildingInfo((BuildingTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
			GC.getBuildingInfo((BuildingTypes)piIndexList[piOrderedIndex[i]]).setHotKeyDescription(GC.getBuildingInfo((BuildingTypes)piIndexList[piOrderedIndex[i]]).getTextKeyWide(), GC.getMissionInfo((MissionTypes)(GC.getBuildingInfo((BuildingTypes)piIndexList[piOrderedIndex[i]]).getMissionType())).getTextKeyWide(), CreateHotKeyFromDescription(GC.getBuildingInfo((BuildingTypes)piIndexList[piOrderedIndex[i]]).getHotKey(), GC.getBuildingInfo((BuildingTypes)piIndexList[piOrderedIndex[i]]).isShiftDown(), GC.getBuildingInfo((BuildingTypes)piIndexList[piOrderedIndex[i]]).isAltDown(), GC.getBuildingInfo((BuildingTypes)piIndexList[piOrderedIndex[i]]).isCtrlDown()));
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_CONTROL)
		{
			GC.getControlInfo((ControlTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_AUTOMATE)
		{
			GC.getAutomateInfo(piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i);
		}
		else if ((ActionSubTypes)piActionInfoTypeList[piOrderedIndex[i]] == ACTIONSUBTYPE_MISSION)
		{
			GC.getMissionInfo((MissionTypes)piIndexList[piOrderedIndex[i]]).setActionInfoIndex(i + iNumOrigVals);
		}

		GC.m_paActionInfo.push_back(pActionInfo);
	}

	SAFE_DELETE_ARRAY(piOrderedIndex);
	SAFE_DELETE_ARRAY(piIndexList);
	SAFE_DELETE_ARRAY(piPriorityList);
	SAFE_DELETE_ARRAY(piActionInfoTypeList);
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalAnimationPathInfo(CvAnimationPathInfo** ppAnimationPathInfo, char* szTagName, int* iNumVals)
//
//  PURPOSE :   Takes the szTagName parameter and if it exists in the xml it loads the ppAnimationPathInfo
//				with the value under it and sets the value of the iNumVals parameter to the total number
//				of occurances of the szTagName tag in the xml.
//
//------------------------------------------------------------------------------------------------------
/*void CvXMLLoadUtility::SetGlobalAnimationPathInfo(CvAnimationPathInfo** ppAnimationPathInfo, const wchar_t* szTagName, int* iNumVals)
{
	PROFILE_FUNC();
	logMsg( "SetGlobalAnimationPathInfo %s\n", szTagName );

	int		i;						// Loop counters
	CvAnimationPathInfo * pAnimPathInfo = NULL;	// local pointer to the domain info memory

	if ( TryMoveToXmlFirstMatchingElement(szTagName) )
	{
		// get the number of times the szTagName tag appears in the xml file
		*iNumVals = GETXML->NumOfElementsByTagName(NULL,szTagName);

		// allocate memory for the domain info based on the number above
		*ppAnimationPathInfo = new CvAnimationPathInfo[*iNumVals];
		pAnimPathInfo = *ppAnimationPathInfo;

		MoveToXmlParent();
		TryMoveToXmlFirstChild();
		TryMoveToXmlFirstChild();


		// Loop through each tag.
		for (i=0;i<*iNumVals;i++)
		{
			if (!pAnimPathInfo[i].read(this))
				break;
			GC.setInfoTypeFromString(pAnimPathInfo[i].getType(), i);	// add type to global info type hash map
			if (!TryMoveToXmlNextSibling())
			{
				break;
			}
		}
	}

	// if we didn't find the tag name in the xml then we never set the local pointer to the
	// newly allocated memory and there for we will FAssert to let people know this most
	// interesting fact
	if(!pAnimPathInfo )
	{
		char	szMessage[1024];
		sprintf( szMessage, "Error finding tag node in SetGlobalAnimationPathInfo function \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
}*/

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalUnitScales(float* pfLargeScale, float* pfSmallScale, char* szTagName)
//
//  PURPOSE :   Takes the szTagName parameter and if it exists in the xml it loads the ppPromotionInfo
//				with the value under it and sets the value of the iNumVals parameter to the total number
//				of occurances of the szTagName tag in the xml.
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetGlobalUnitScales(float* pfLargeScale, float* pfSmallScale, const wchar_t* szTagName)
{
	PROFILE_FUNC();
	logging::logMsgW("xml.log", L"SetGlobalUnitScales %s\n", szTagName);
	// if we successfully locate the szTagName node
	if (TryMoveToXmlFirstMatchingElement(szTagName))
	{
		// call the function that sets the FXml pointer to the first non-comment child of
		// the current tag and gets the value of that new node
		if (GetChildXmlVal(pfLargeScale))
		{
			// set the current xml node to it's next sibling and then
			// get the sibling's TCHAR value
			GetNextXmlVal(pfSmallScale);

			// set the current xml node to it's parent node
			MoveToXmlParent();
		}
	}
	else
	{
		// if we didn't find the tag name in the xml then we never set the local pointer to the
		// newly allocated memory and there for we will FAssert to let people know this most
		// interesting fact
		char	szMessage[1024];
		sprintf( szMessage, "Error finding tag node in SetGlobalUnitScales function \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGameText()
//
//  PURPOSE :   Reads game text info from XML and adds it to the translation manager
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetGameText(const wchar_t* szTextGroup, const wchar_t* szTagName, std::vector<CvGameText>& texts)
{
	PROFILE_FUNC();
	logging::logMsgW("xml.log", L"SetGameText: %s\n", szTagName);
	int startNumOfTexts = texts.size();

	if (TryMoveToXmlFirstMatchingElement(szTextGroup)) // Get the Text Group 1st
	{
		//int iNumVals = GetXmlChildrenNumber();	// Get the number of Children that the Text Group has
		if (TryMoveToXmlFirstMatchingElement(szTagName)) // Now switch to the TEXT Tag
		{
			/*MoveToXmlParent();
			TryMoveToXmlFirstChild();*/

			// loop through each tag
			do
			{
				CvGameText textInfo;
				textInfo.read(this);

				texts.push_back(textInfo);

				gDLL->addText(textInfo.getType() /*id*/, textInfo.getText(), textInfo.getGender(), textInfo.getPlural());
			}
			while(TryMoveToXmlNextSibling());
		}
	}
	logging::logMsg("xml.log", "Read %i text(s)\n", texts.size() - startNumOfTexts);
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetGlobalClassInfo - This is a template function that is USED FOR ALMOST ALL INFO CLASSES.
//		Each info class should have a read(CvXMLLoadUtility*) function that is responsible for initializing
//		the class from xml data.
//
//  PURPOSE :   takes the szTagName parameter and loads the ppszString with the text values
//				under the tags.  This will be the hints displayed during game initialization and load
//
//------------------------------------------------------------------------------------------------------
template <class T>
void CvXMLLoadUtility::SetGlobalClassInfo(std::vector<T*>& aInfos, const wchar_t* szTagName, bool bTwoPass, CvInfoReplacements<T>* pReplacements)
{
	char szLog[256];
	char* tmp = xercesc::XMLString::transcode(szTagName);
	sprintf(szLog, "SetGlobalClassInfo (%s)\n", tmp);
	xercesc::XMLString::release(&tmp);
	PROFILE(szLog);
	logging::logMsg("xml.log", szLog);

	// if we successfully locate the tag name in the xml file
	if (TryMoveToXmlFirstMatchingElement(szTagName))
	{
		// loop through each tag
		do
		{
// Anq: try to load TYPE, DEPEDENCY, FORCEOVERWRITE, REPLACEMENT info before actually calling read()
// Firstly, we read the type name, look it up in the global map whether one of the same type is loaded.
			CvString szTypeName, szTypeReplace;
			bool bHasType = true;
			if (!GetOptionalChildXmlValByName(szTypeName, L"Type") || szTypeName.empty())
			{
				bHasType = false;
				if (shouldHaveType)
				{
					OutputDebugString("Missing Element");
					OutputDebugStringW(GetCurrentXMLElement()->getNodeName());
					OutputDebugStringW(GetCurrentXMLElement()->getTextContent());
					continue; // skip the current object altogether
				}
			}
			if (!bHasType)
			{
				// (1) Let's deal with type-ignorant classes first, they are easier
				std::auto_ptr<T> pClassInfo(new T());
				if (pClassInfo->read(this))
				{
					aInfos.push_back(pClassInfo.release());
				}
			}
			else
			{
				// (2) Check dependencies. If not satisfied, skip altogether
				if (!CheckDependency())
					continue;

				// (3) Read off if a modder wants his work to completely replace the core definition
				bool bForceOverwrite = false;
				GetOptionalChildXmlValByName(&bForceOverwrite, L"bForceOverwrite");

				// (4) Read off the Replacement condition
				uint32_t uiReplacementID = 0;
				std::auto_ptr<const BoolExpr> pReplacementCondition;
				if (GetOptionalChildXmlValByName(szTypeReplace, L"ReplacementID") && szTypeReplace.size())
				{
					uiReplacementID = CvInfoReplacements<T>::getReplacementIDForString(szTypeReplace);
					if (TryMoveToXmlFirstChild(L"ReplacementCondition"))
					{
						// Replacement condition must be defined by the base object that
						// names the particular Replacement ID; otherwise it won't work!
						if (TryMoveToXmlFirstChild())
						{
							pReplacementCondition.reset(BoolExpr::read(this));
							MoveToXmlParent();
						}
						MoveToXmlParent();
					}
				}

				// (5) Now we can parse the object
				std::auto_ptr<T> pClassInfo(new T());
				if (pClassInfo->read(this))
				{
					// See if the type name is associated with any loaded object
					if (GC.getInfoTypeForString(szTypeName, true) == -1)
					{
						// (5-1) Does not exist
						const uint32_t uiAppendPosition = aInfos.size();
						if (szTypeReplace.empty())
						{
							aInfos.push_back(pClassInfo.release());
						}
						else if (pReplacementCondition.get())	// has szTypeReplace
						{
							// AIAndy: If the class is a replacement, add it to the replacements
							// but also add a dummy to the normal array to reserve an ID
							aInfos.push_back(new T());
							pReplacements->addReplacement(uiAppendPosition, uiReplacementID, pReplacementCondition.release(), pClassInfo.release());
						}
						else
						{
							FAssertMsg(pReplacementCondition.get(), CvString::format("No replacement condition for this Replacement ID %s.\r\n\tMake sure it's defined for the first replacement object in the load order.", szTypeReplace.c_str()));
						}
						GC.setInfoTypeFromString(szTypeName, uiAppendPosition);
					}
					else
					{
						// (5-2) Found at uiExistPosition
						const uint32_t uiExistPosition = GC.getInfoTypeForString(szTypeName);
						FAssertMsg(uiExistPosition != -1, "Couldn't find info type");
						if (szTypeReplace.empty())
						{
							if (!bForceOverwrite)
							{
								pClassInfo->copyNonDefaults(aInfos[uiExistPosition]);
							}
							SAFE_DELETE(aInfos[uiExistPosition])
							aInfos[uiExistPosition] = pClassInfo.release();
						}
						else
						{
							CvInfoReplacement<T>* pExisting = pReplacements->getReplacement(uiExistPosition, uiReplacementID);
							if (pExisting)
							{
								pClassInfo->copyNonDefaults(pExisting->getInfo());
								pExisting->setInfo(pClassInfo.release());
							}
							else if (pReplacementCondition.get())
							{
								pReplacements->addReplacement(uiExistPosition, uiReplacementID, pReplacementCondition.release(), pClassInfo.release());
							}
							else
							{
								FAssertMsg(pReplacementCondition.get(), CvString::format("No replacement condition for this Replacement ID %s.\r\n\tMake sure it's defined for the first replacement object in the load order.", szTypeReplace.c_str()));
							}
						}
					}
				}
			}
		} while (TryMoveToXmlNextSibling());
	}
}

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 05/17/08                                MRGENIE      */
/*                                                                                              */
/* This method is a replacement for the bTwoPass, if stuff that is depending on each other in   */
/* a loop, the bTwoPass would fail since it doesn't look first in the other Modules!            */
/************************************************************************************************/
template <class T>
void CvXMLLoadUtility::SetGlobalClassInfoTwoPassReplacement(std::vector<T*>& aInfos, const wchar_t* szTagName, CvInfoReplacements<T>* pReplacements)
{
	char szLog[256];
	char* tmp = xercesc::XMLString::transcode(szTagName);
	sprintf(szLog, "SetGlobalClassInfo (%s)\n", tmp);
	xercesc::XMLString::release(&tmp);
	PROFILE(szLog);
	logging::logMsg("xml.log", szLog);

	if (TryMoveToXmlFirstMatchingElement(szTagName))
	{
		// loop through each tag
		do
		{
			CvString szTypeName, szTypeReplace;
			if (!GetOptionalChildXmlValByName(szTypeName, L"Type") || szTypeName.empty())
			{
				char* tmp = xercesc::XMLString::transcode(GetCurrentXMLElement()->getNodeName());
				char* tmp2 = xercesc::XMLString::transcode(GetCurrentXMLElement()->getTextContent());
				const CvString errorMsg = CvString::format("Missing Element, %s, %s", tmp, tmp2);
				xercesc::XMLString::release(&tmp);
				xercesc::XMLString::release(&tmp2);
				FErrorMsg(errorMsg)
				continue;
			}
			// (1) Check if an object of the name exists already. If not, skip it.
			if (GC.getInfoTypeForString(szTypeName, true) == -1) {
				FErrorMsg(CvString::format("Item %s not found to do a ReadPass2", szTypeName.c_str()))
				continue;
			}
			// (2) Check dependencies. If not satisfied, skip altogether
			if (!CheckDependency())
				continue;
			// (3) Read off if a modder wants his work to completely replace the core definition
			bool bForceOverwrite = false;
			GetOptionalChildXmlValByName(&bForceOverwrite, L"bForceOverwrite");
			// (4) Read off the Replacement condition
			uint uiReplacementID = 0;
			bst::shared_ptr<const BoolExpr> pReplacementCondition;
			if (GetOptionalChildXmlValByName(szTypeReplace, L"ReplacementID") && szTypeReplace.size()) {
				uiReplacementID = CvInfoReplacements<T>::getReplacementIDForString(szTypeReplace);
				if (TryMoveToXmlFirstChild(L"ReplacementCondition")) {
					if (TryMoveToXmlFirstChild()) {
						pReplacementCondition.reset(BoolExpr::read(this));
						MoveToXmlParent();
					}
					MoveToXmlParent();
				}
			}
			T pClassInfo;
			if (pClassInfo.readPass2(this))
			{
				uint uiExistPosition = GC.getInfoTypeForString(szTypeName);
				FAssertMsg(uiExistPosition != -1, "Couldn't find info type");
				if (szTypeReplace.empty())
				{
					aInfos[uiExistPosition]->copyNonDefaultsReadPass2(&pClassInfo, this, bForceOverwrite);
				}
				else
				{
					CvInfoReplacement<T>* pExisting = pReplacements->getReplacement(uiExistPosition, uiReplacementID);
					FAssertMsg(pExisting != NULL, CvString::format("Must have an existing replacement object of ID %s on the list!", szTypeReplace.c_str()))
					pExisting->getInfo()->copyNonDefaultsReadPass2(&pClassInfo, this, bForceOverwrite);
				}
			}
		} while (TryMoveToXmlNextSibling());
	}
}
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/

void CvXMLLoadUtility::SetDiplomacyInfo(std::vector<CvDiplomacyInfo*>& DiploInfos, const wchar_t* szTagName)
{
	char szLog[256];
	char* tmp = xercesc::XMLString::transcode(szTagName);
	sprintf(szLog, "SetDiplomacyInfo (%s)\n", tmp);
	xercesc::XMLString::release(&tmp);
	PROFILE(szLog);
	logging::logMsg("xml.log", szLog);

	// if we successfully locate the tag name in the xml file
	if (TryMoveToXmlFirstMatchingElement(szTagName))
	{
		// loop through each tag
		do
		{
			CvString szType;
			GetChildXmlValByName(szType, L"Type");
			int iIndex = GC.getInfoTypeForString(szType, true);

			if (-1 == iIndex)
			{
				CvDiplomacyInfo* pClassInfo = new CvDiplomacyInfo;

				if (NULL == pClassInfo)
				{
					FAssert(false);
					break;
				}

				pClassInfo->read(this);
				if (NULL != pClassInfo->getType())
				{
					GC.setInfoTypeFromString(pClassInfo->getType(), (int)DiploInfos.size());	// add type to global info type hash map
				}
				DiploInfos.push_back(pClassInfo);
			}
			else
			{
/*************************************************************************************************/
/**	TrueModular								05/26/09	Written: Mr. Genie	Imported: Xienwolf	**/
/**																								**/
/**	Properly links Modular modifications to previous elements, and allows partial overwriting	**/
/*************************************************************************************************/
/**								---- Start Original Code ----									**
				DiploInfos[iIndex]->read(this);
/**								----  End Original Code  ----									**/
				bool bForceOverwrite = false;
				GetOptionalChildXmlValByName(&bForceOverwrite, L"bForceOverwrite");
				if(!bForceOverwrite)
					DiploInfos[iIndex]->copyNonDefaults(this);
/*************************************************************************************************/
/**	TrueModular								END													**/
/*************************************************************************************************/
			}

		} while (TryMoveToXmlNextSibling());
	}
}

template <class T>
void CvXMLLoadUtility::LoadGlobalClassInfo(std::vector<T*>& aInfos, const char* szFileRoot, const char* szFileDirectory, const wchar_t* szXmlPath, bool bTwoPass, CvInfoReplacements<T>* pReplacements)
{
	bool bLoaded = false;
	GC.addToInfosVectors(&aInfos);

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 05/17/08                                MRGENIE      */
/*                                                                                              */
/* This method is a replacement for the bTwoPass, if stuff that is depending on each other in   */
/* a loop, the bTwoPass would fail since it doesn't look first in the other Modules!            */
/************************************************************************************************/
	bool bTwoPassReplacement = true; // Use this if you wanna use the regular Firaxis bTwoPass. AIAndy: Firaxis two pass no more maintained now
	if ( bTwoPassReplacement )
	{
		if (!bTwoPass )
		{
			bTwoPassReplacement = false;
		}
		bTwoPass = false;
	}
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/

	DEBUG_LOG("XmlCheckDoubleTypes.log", "\nEntering: %s\n", szFileRoot);

	CvXMLLoadUtilitySetMod* pModEnumVector = new CvXMLLoadUtilitySetMod;
	CvXMLLoadUtilityModTools* p_szDirName = new CvXMLLoadUtilityModTools;

	if (!bLoaded)
	{
		std::vector<CvString> aszFiles;
		CvString szModDirectory = GC.getInitCore().getDLLPath() + "\\xml\\";
		pModEnumVector->MLFEnumerateFiles(aszFiles, (szModDirectory + szFileDirectory).c_str(), CvString::format("xml\\%s", szFileDirectory).c_str(), CvString::format("%s.xml", szFileRoot).c_str(), false);
		if(aszFiles.size() == 0)
			aszFiles.push_back(CvString::format("xml\\%s/%s.xml", szFileDirectory, szFileRoot));

		foreach_(const CvString& szFile, aszFiles)
		{
			bLoaded = LoadCivXml(NULL, szFile);

			if (!bLoaded)
			{
				char szMessage[1024];
				sprintf(szMessage, "LoadXML call failed for %s.", szFile.GetCString());
				gDLL->MessageBox(szMessage, "XML Load Error");
				break;
			}
			else
			{
				SetGlobalClassInfo(aInfos, szXmlPath, bTwoPass, pReplacements);
			}
		}

		if (bLoaded)
		{
			if (gDLL->isModularXMLLoading())
			{
				std::vector<CvString> aszFiles;
				gDLL->enumerateFiles(aszFiles, CvString::format("modules\\*_%s.xml", szFileRoot));  // search for the modular files

				foreach_(const CvString& szFile, aszFiles)
				{
					bLoaded = LoadCivXml(NULL, szFile);

					if (!bLoaded)
					{
						char szMessage[1024];
						sprintf(szMessage, "LoadXML call failed for %s.", szFile.GetCString());
						gDLL->MessageBox(szMessage, "XML Load Error");
					}
					else
					{
						SetGlobalClassInfo(aInfos, szXmlPath, bTwoPass, pReplacements);
					}
				}

				//AIAndy: Moved to this place so module stuff first pass is loaded before the second pass to the base XML
				if ( bTwoPassReplacement )
				{
					std::vector<CvString> aszFiles;
					CvString szModDirectory = GC.getInitCore().getDLLPath() + "\\xml\\";
					pModEnumVector->MLFEnumerateFiles(aszFiles, (szModDirectory + szFileDirectory).c_str(), CvString::format("xml\\%s", szFileDirectory).c_str(), CvString::format("%s.xml", szFileRoot).c_str(), false);
					if(aszFiles.size() == 0)
						aszFiles.push_back(CvString::format("xml\\%s/%s.xml", szFileDirectory, szFileRoot));

					foreach_(const CvString& szFile, aszFiles)
					{
						bLoaded = LoadCivXml(NULL, szFile);

						if (!bLoaded)
						{
							char szMessage[1024];
							sprintf(szMessage, "LoadXML call failed for %s.", szFile.GetCString());
							gDLL->MessageBox(szMessage, "XML Load Error");
							break;
						}
						else
						{
							SetGlobalClassInfoTwoPassReplacement(aInfos, szXmlPath, pReplacements);
						}
					}
				}

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 05/17/08                                MRGENIE      */
/*                                                                                              */
/* This method is a replacement for the bTwoPass, if stuff that is depending on each other in   */
/* a loop, the bTwoPass would fail since it doesn't look first in the other Modules!            */
/************************************************************************************************/
				if ( bTwoPassReplacement )	// reloop through the modules!
				{
					foreach_(const CvString& szFile, aszFiles)
					{
						bLoaded = LoadCivXml(NULL, szFile);

						if (!bLoaded)
						{
							char szMessage[1024];
							sprintf(szMessage, "LoadXML call failed for %s.", szFile.GetCString());
							gDLL->MessageBox(szMessage, "XML Load Error");
						}
						else
						{
							SetGlobalClassInfoTwoPassReplacement(aInfos, szXmlPath, pReplacements);
						}
					}
				}
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
			}

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 11/15/07                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
			else
			{
				std::vector<CvString> aszFiles;
				pModEnumVector->loadModControlArray(aszFiles, szFileRoot);

				foreach_(const CvString& szFile, aszFiles)
				{
					bLoaded = LoadCivXml(NULL, szFile);

					if (!bLoaded)
					{
						char szMessage[1024];
						sprintf(szMessage, "LoadXML call failed for %s.", szFile.GetCString());
						gDLL->MessageBox(szMessage, "XML Load Error");
					}
					else
					{
						SetGlobalClassInfo(aInfos, szXmlPath, bTwoPass, pReplacements);
					}
				}

				//AIAndy: Moved to this place so module stuff first pass is loaded before the second pass to the base XML
				if ( bTwoPassReplacement )
				{
					std::vector<CvString> aszFiles;
					CvString szModDirectory = GC.getInitCore().getDLLPath() + "\\xml\\";
					pModEnumVector->MLFEnumerateFiles(aszFiles, (szModDirectory + szFileDirectory).c_str(), CvString::format("xml\\%s", szFileDirectory).c_str(), CvString::format("%s.xml", szFileRoot).c_str(), false);
					if(aszFiles.size() == 0)
						aszFiles.push_back(CvString::format("xml\\%s/%s.xml", szFileDirectory, szFileRoot));

					foreach_(const CvString& szFile, aszFiles)
					{
						bLoaded = LoadCivXml(NULL, szFile);

						if (!bLoaded)
						{
							char szMessage[1024];
							sprintf(szMessage, "LoadXML call failed for %s.", szFile.GetCString());
							gDLL->MessageBox(szMessage, "XML Load Error");
							break;
						}
						else
						{
							SetGlobalClassInfoTwoPassReplacement(aInfos, szXmlPath, pReplacements);
						}
					}
				}

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 05/17/08                                MRGENIE      */
/*                                                                                              */
/* This method is a replacement for the bTwoPass, if stuff that is depending on each other in   */
/* a loop, the bTwoPass would fail since it doesn't look first in the other Modules!            */
/************************************************************************************************/
				if ( bTwoPassReplacement )	// reloop through the modules!
				{
					foreach_(const CvString& szFile, aszFiles)
					{
						bLoaded = LoadCivXml(NULL, szFile);

						if (!bLoaded)
						{
							char szMessage[1024];
							sprintf(szMessage, "LoadXML call failed for %s.", szFile.GetCString());
							gDLL->MessageBox(szMessage, "XML Load Error");
						}
						else
						{
							SetGlobalClassInfoTwoPassReplacement(aInfos, szXmlPath, pReplacements);
						}
					}
				}
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
			}

			m_pParser->resetDocumentPool();
		}
	}

/************************************************************************************************/
/* XML_MODULAR_ART_LOADING                 10/26/07                            MRGENIE          */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	SAFE_DELETE(pModEnumVector);
	SAFE_DELETE(p_szDirName);
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
}

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 05/13/08                                MRGENIE      */
/* we use this for xml's that should only be loaded as a module                                 */
/*                                                                                              */
/************************************************************************************************/
// AIAndy: Still unused, so no more maintained for now
/*template <class T>
void CvXMLLoadUtility::LoadGlobalClassInfoModular(std::vector<T*>& aInfos, const char* szFileRoot, const char* szFileDirectory, const char* szXmlPath, bool bTwoPass)
{
	// we don't wanna use cache for modular loading, could do that, but safer just to load modules from scratch..
	bool bLoaded = false;
	GC.addToInfosVectors(&aInfos);

#ifdef _DEBUG
	logXmlCheckDoubleTypes("\nEntering: %s\n", szFileRoot);
#endif
	CvXMLLoadUtilitySetMod* pModEnumVector = new CvXMLLoadUtilitySetMod;

	std::vector<CvString> aszFiles;
	//aszFiles.reserve(10000);
	unsigned long ulCheckSum = 0;
	pModEnumVector->loadModControlArray(aszFiles, szFileRoot, ulCheckSum);

	for (std::vector<CvString>::iterator it = aszFiles.begin(); it != aszFiles.end(); ++it)
	{

		bLoaded = LoadCivXml(NULL, *it);

		if (!bLoaded)
		{
			char szMessage[1024];
			sprintf(szMessage, "LoadXML call failed for %s.", (*it).GetCString());
			gDLL->MessageBox(szMessage, "XML Load Error");
		}
		else
		{
			SetGlobalClassInfo(aInfos, szXmlPath, bTwoPass);
		}
	}

	SAFE_DELETE(pModEnumVector);
	SAFE_DELETE(p_szDirName);
}*/
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/

void CvXMLLoadUtility::LoadDiplomacyInfo(std::vector<CvDiplomacyInfo*>& DiploInfos, const char* szFileRoot, const char* szFileDirectory, const wchar_t* szXmlPath, bool bUseCaching)
{
	bool bLoaded = false;

	if (!bLoaded)
	{
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING                 10/26/07                            MRGENIE          */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		CvXMLLoadUtilityModTools* p_szDirName = new CvXMLLoadUtilityModTools;
		CvXMLLoadUtilitySetMod* pModEnumVector = new CvXMLLoadUtilitySetMod;
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING                 END                                                  */
/************************************************************************************************/
		bLoaded = LoadCivXml(NULL, CvString::format("xml\\%s/%s.xml", szFileDirectory, szFileRoot));

		if (!bLoaded)
		{
			char szMessage[1024];
			sprintf(szMessage, "LoadXML call failed for %s.", CvString::format("%s/%s.xml", szFileDirectory, szFileRoot).GetCString());
			gDLL->MessageBox(szMessage, "XML Load Error");
		}
		else
		{
			SetDiplomacyInfo(DiploInfos, szXmlPath);

			if (gDLL->isModularXMLLoading())
			{
				std::vector<CvString> aszFiles;
				gDLL->enumerateFiles(aszFiles, CvString::format("modules\\*_%s.xml", szFileRoot));  // search for the modular files

				foreach_(const CvString& szFile, aszFiles)
				{
					bLoaded = LoadCivXml(NULL, szFile);

					if (!bLoaded)
					{
						char szMessage[1024];
						sprintf(szMessage, "LoadXML call failed for %s.", szFile.GetCString());
						gDLL->MessageBox(szMessage, "XML Load Error");
					}
					else
					{
						SetDiplomacyInfo(DiploInfos, szXmlPath);
					}
				}
			}

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 11/15/07                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
			else
			{
				std::vector<CvString> aszFiles;
				//aszFiles.reserve(10000);
				pModEnumVector->loadModControlArray(aszFiles, szFileRoot);

				foreach_(const CvString& szFile, aszFiles)
				{
					bLoaded = LoadCivXml(NULL, szFile);

					if (!bLoaded)
					{
						char szMessage[1024];
						sprintf(szMessage, "LoadXML call failed for %s.", szFile.GetCString());
						gDLL->MessageBox(szMessage, "XML Load Error");
					}
					else
					{
						SetDiplomacyInfo(DiploInfos, szXmlPath);
					}
				}
			}
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
		}
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 11/15/07                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		SAFE_DELETE(pModEnumVector);
		SAFE_DELETE(p_szDirName);
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
	}
}

//
// helper sort predicate
//

struct OrderIndex {int m_iPriority; int m_iIndex;};
bool sortHotkeyPriority(const OrderIndex& orderIndex1, const OrderIndex& orderIndex2)
{
	return (orderIndex1.m_iPriority > orderIndex2.m_iPriority);
}

void CvXMLLoadUtility::orderHotkeyInfo(int** ppiSortedIndex, int* pHotkeyIndex, int iLength)
{
	int* piSortedIndex;
	std::vector<OrderIndex> viOrderPriority;

	viOrderPriority.resize(iLength);
	piSortedIndex = *ppiSortedIndex;

	// set up vector
	for (int iI = 0; iI < iLength; iI++)
	{
		viOrderPriority[iI].m_iPriority = pHotkeyIndex[iI];
		viOrderPriority[iI].m_iIndex = iI;
	}

	// sort the array
	algo::sort(viOrderPriority, sortHotkeyPriority);

	// insert new order into the array to return
	for (int iI = 0; iI < iLength; iI++)
	{
		piSortedIndex[iI] = viOrderPriority[iI].m_iIndex;
	}
}

//
// helper sort predicate
//
/*
bool sortHotkeyPriorityOld(const CvHotkeyInfo* hotkey1, const CvHotkeyInfo* hotkey2)
{
return (hotkey1->getOrderPriority() < hotkey2->getOrderPriority());
}
*/

//------------------------------------------------------------------------------------------------
// FUNCTION:    void CvXMLLoadUtility::orderHotkeyInfoOld(T **ppHotkeyInfos, int iLength)
//! \brief      order a hotkey info derived class
//! \param      ppHotkeyInfos is a hotkey info derived class
//!							iLength is the length of the hotkey info derived class array
//! \retval
//------------------------------------------------------------------------------------------------
/*
template <class T>
void CvXMLLoadUtility::orderHotkeyInfoOld(T **ppHotkeyInfos, int iLength)
{
int iI;
std::vector<T*> vHotkeyInfo;
T* pHotkeyInfo;	// local pointer to the hotkey info memory

for (iI=0;iI<iLength;iI++)
{
pHotkeyInfo = new T;
*pHotkeyInfo = (*ppHotkeyInfos)[iI];
vHotkeyInfo.push_back(pHotkeyInfo);
}

std::sort(vHotkeyInfo.begin(), vHotkeyInfo.end(), sortHotkeyPriority);

for (iI=0;iI<iLength;iI++)
{
(*ppHotkeyInfos)[iI] = *vHotkeyInfo[iI];
}

for (iI=0;iI<(int)vHotkeyInfo.size();iI++)
{
vHotkeyInfo[iI]->reset();
SAFE_DELETE(vHotkeyInfo[iI]);
}
vHotkeyInfo.clear();
}
*/

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetYields(int** ppiYield)
//
//  PURPOSE :   Allocate memory for the yield parameter and set it to the values
//				in the xml file.  The current/last located node must be the first child of the
//				yield changes node
//
//------------------------------------------------------------------------------------------------------
int CvXMLLoadUtility::SetYields(int** ppiYield)
{
	int i=0;			//loop counter
	int iNumSibs=0;		// the number of siblings the current xml node has
	int *piYield;	// local pointer for the yield memory

	// get the total number of children the current xml node has
	iNumSibs = GetXmlChildrenNumber();

	InitList(ppiYield, NUM_YIELD_TYPES);

	// set the local pointer to the memory we just allocated
	piYield = *ppiYield;

	if (0 < iNumSibs)
	{
		// if the call to the function that sets the current xml node to it's first non-comment
		// child and sets the parameter with the new node's value succeeds
		if (GetChildXmlVal(&piYield[0]))
		{
			if(!(iNumSibs <= NUM_YIELD_TYPES))
			{
				char	szMessage[1024];
				sprintf( szMessage, "For loop iterator is greater than array size \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
				gDLL->MessageBox(szMessage, "XML Error");
			}
			// loop through all the siblings, we start at 1 since we already have the first value
			for (i=1;i<iNumSibs;i++)
			{
				// if the call to the function that sets the current xml node to it's first non-comment
				// sibling and sets the parameter with the new node's value does not succeed
				// we will break out of this for loop
				if (!GetNextXmlVal(&piYield[i]))
				{
					break;
				}
			}
			// set the current xml node to it's parent node
			MoveToXmlParent();
		}
	}

	return iNumSibs;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetFeatureStruct(int** ppiFeatureTech, int** ppiFeatureTime, int** ppiFeatureProduction, bool** ppbFeatureRemove, bool** ppbNoTechCanRemoveWithNoProductionGain)
//
//  PURPOSE :   allocate and set the feature struct variables for the CvBuildInfo class
//	Last Modified: Afforess, 5/25/10
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetFeatureStruct(int** ppiFeatureTech, int** ppiFeatureTime, int** ppiFeatureProduction, bool** ppbFeatureRemove, bool** ppbNoTechCanRemoveWithNoProductionGain)
{
	int i=0;				//loop counter
	int iNumChildren;		// the number of siblings the current xml node has
	int iFeatureIndex;
	TCHAR szTextVal[256];	// temporarily hold the text value of the current xml node
	int* paiFeatureTech = NULL;
	int* paiFeatureTime = NULL;
	int* paiFeatureProduction = NULL;
	bool* pabFeatureRemove = NULL;
	bool* pabNoTechCanRemoveWithNoProductionGain = NULL;

	if(GC.getNumFeatureInfos() < 1)
	{
		char	szMessage[1024];
		sprintf( szMessage, "no feature infos set yet! \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
		gDLL->MessageBox(szMessage, "XML Error");
	}
	InitList(ppiFeatureTech, GC.getNumFeatureInfos(), -1);
	InitList(ppiFeatureTime, GC.getNumFeatureInfos());
	InitList(ppiFeatureProduction, GC.getNumFeatureInfos());
	InitList(ppbFeatureRemove, GC.getNumFeatureInfos());
	InitList(ppbNoTechCanRemoveWithNoProductionGain, GC.getNumFeatureInfos());

	paiFeatureTech = *ppiFeatureTech;
	paiFeatureTime = *ppiFeatureTime;
	paiFeatureProduction = *ppiFeatureProduction;
	pabFeatureRemove = *ppbFeatureRemove;
	pabNoTechCanRemoveWithNoProductionGain = *ppbNoTechCanRemoveWithNoProductionGain;

	if (TryMoveToXmlFirstChild(L"FeatureStructs"))
	{
		iNumChildren = GetXmlChildrenNumber();

		if (0 < iNumChildren)
		{
			if (TryMoveToXmlFirstChild(L"FeatureStruct"))
			{
				if(!(iNumChildren <= GC.getNumFeatureInfos()))
				{
					char	szMessage[1024];
					sprintf( szMessage, "iNumSibs is greater than GC.getNumFeatureInfos in SetFeatureStruct function \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
					gDLL->MessageBox(szMessage, "XML Error");
				}
				for (i=0;i<iNumChildren;i++)
				{
					GetChildXmlValByName(szTextVal, L"FeatureType");
					iFeatureIndex = GetInfoClass(szTextVal);
					if(!(iFeatureIndex != -1))
					{
						char	szMessage[1024];
						sprintf( szMessage, "iFeatureIndex is -1 inside SetFeatureStruct function \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
						gDLL->MessageBox(szMessage, "XML Error");
					}
					GetOptionalChildXmlValByName(szTextVal, L"PrereqTech");
					paiFeatureTech[iFeatureIndex] = GetInfoClass(szTextVal);
					GetChildXmlValByName(&paiFeatureTime[iFeatureIndex], L"iTime");
					GetChildXmlValByName(&paiFeatureProduction[iFeatureIndex], L"iProduction");
					GetOptionalChildXmlValByName(&pabFeatureRemove[iFeatureIndex], L"bRemove");
					GetOptionalChildXmlValByName(&pabNoTechCanRemoveWithNoProductionGain[iFeatureIndex], L"bCanRemoveWithNoProductionGain");

					if (!TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				MoveToXmlParent();
			}
		}

		MoveToXmlParent();
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetImprovementBonuses(CvImprovementBonusInfo** ppImprovementBonus)
//
//  PURPOSE :   Allocate memory for the improvement bonus pointer and fill it based on the
//				values in the xml.
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetImprovementBonuses(CvImprovementBonusInfo** ppImprovementBonus)
{
	int i = 0;				//loop counter
	int iNumChildren;		// the number of siblings the current xml node has
	TCHAR szNodeVal[256];	// temporarily holds the string value of the current xml node
	CvImprovementBonusInfo* paImprovementBonus;	// local pointer to the bonus type struct in memory

	// initialize the boolean list to the correct size and all the booleans to false
	InitImprovementBonusList(ppImprovementBonus, GC.getNumBonusInfos());
	// set the local pointer to the memory we just allocated
	paImprovementBonus = *ppImprovementBonus;

	// get the total number of children the current xml node has
	iNumChildren = GetXmlChildrenNumber();
	// if we can set the current xml node to the child of the one it is at now
	if (TryMoveToXmlFirstChild(L"BonusTypeStruct"))
	{
		if (!(iNumChildren <= GC.getNumBonusInfos()))
		{
			char	szMessage[1024];
			sprintf(szMessage, "For loop iterator is greater than array size \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
			gDLL->MessageBox(szMessage, "XML Error");
		}
		// loop through all the siblings
		for (i = 0; i<iNumChildren; i++)
		{
			// call the function that sets the FXml pointer to the first non-comment child of
			// the current tag and gets the value of that new node

			if (GetChildXmlValByName(szNodeVal, L"BonusType"))
			{
				int iBonusIndex;	// index of the match in the bonus types list
				// call the find in list function to return either -1 if no value is found
				// or the index in the list the match is found at
				iBonusIndex = GetInfoClass(szNodeVal);
				// if we found a match we will get the next sibling's boolean value at that match's index
				if (iBonusIndex >= 0)
				{
					GetOptionalChildXmlValByName(&paImprovementBonus[iBonusIndex].m_bObsoleteBonusMakesValid, L"bObsoleteBonusMakesValid");
					GetOptionalChildXmlValByName(&paImprovementBonus[iBonusIndex].m_bBonusMakesValid, L"bBonusMakesValid");
					GetOptionalChildXmlValByName(&paImprovementBonus[iBonusIndex].m_bBonusTrade, L"bBonusTrade");
					GetOptionalChildXmlValByName(&paImprovementBonus[iBonusIndex].m_iDiscoverRand, L"iDiscoverRand");
					/************************************************************************************************/
					/* Afforess	                  Start		 07/27/10                                               */
					/*                                                                                              */
					/*                                                                                              */
					/************************************************************************************************/
					GetOptionalChildXmlValByName(&paImprovementBonus[iBonusIndex].m_iDepletionRand, L"iDepletionRand");
					/************************************************************************************************/
					/* Afforess	                     END                                                            */
					/************************************************************************************************/

					SAFE_DELETE_ARRAY(paImprovementBonus[iBonusIndex].m_piYieldChange);	// free memory - MT, since we are about to reallocate
					if (TryMoveToXmlFirstChild(L"YieldChanges"))
					{
						SetYields(&paImprovementBonus[iBonusIndex].m_piYieldChange);
						MoveToXmlParent();
					}
					else
					{
						InitList(&paImprovementBonus[iBonusIndex].m_piYieldChange, NUM_YIELD_TYPES);
					}
				}
				else
				{
					MoveToXmlParent();
				}

				// set the current xml node to it's parent node
			}

			// if we cannot set the current xml node to it's next sibling then we will break out of the for loop
			// otherwise we will continue looping
			if (!TryMoveToXmlNextSibling())
			{
				break;
			}
		}
		// set the current xml node to it's parent node
		MoveToXmlParent();
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetVariableListTagPair(	int **ppiList, const TCHAR* szRootTagName,
//										int iInfoBaseSize, int iInfoBaseLength, int iDefaultListVal)
//
//  PURPOSE :   allocate and initialize a list from a tag pair in the xml
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetVariableListTagPair(int **ppiList, const wchar_t* szRootTagName, int iInfoBaseLength, int iDefaultListVal)
{
	if (0 > iInfoBaseLength)
	{
		char	szMessage[1024];
		char* tmp = xercesc::XMLString::transcode(szRootTagName);
		sprintf( szMessage, "Allocating zero or less memory in CvXMLLoadUtility::SetVariableListTagPair (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
		xercesc::XMLString::release(&tmp);
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if (TryMoveToXmlFirstChild(szRootTagName))
	{
		int iNumChildren = GetXmlChildrenNumber();
		if (0 < iNumChildren)
		{
			InitList(ppiList, iInfoBaseLength, iDefaultListVal);
			if (iNumChildren > iInfoBaseLength)
			{
				char szMessage[1024];
				char* tmp = xercesc::XMLString::transcode(szRootTagName);
				sprintf( szMessage, "There are more siblings than memory allocated for them in CvXMLLoadUtility::SetVariableListTagPair (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
				xercesc::XMLString::release(&tmp);
				gDLL->MessageBox(szMessage, "XML Error");
			}
			if (TryMoveToXmlFirstChild())
			{
				int* piList = *ppiList;
				TCHAR szTextVal[256];
				for (int i = 0; i < iNumChildren; i++)
				{
					if (GetChildXmlVal(szTextVal))
					{
						int iIndexVal = GetInfoClass(szTextVal);
						if (iIndexVal != -1)
						{
							GetNextXmlVal(&piList[iIndexVal]);
						}
						MoveToXmlParent();
					}
					if (!TryMoveToXmlNextSibling())
					{
						break;
					}
				}
				MoveToXmlParent();
			}
		}
		MoveToXmlParent();
	}
}

/************************************************************************************************/
/* RevDCM  XMLloading                             05/05/10             phungus420               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
//allows loading of a variable pair into an array by tag name
void CvXMLLoadUtility::SetVariableListTagPair(int **ppiList, const wchar_t* szRootTagName,
		int iInfoBaseLength, const wchar_t* szValueTagName, int iValueInfoBaseLength, int iDefaultListVal)
{
	int i;
	int iIndexVal;
	int iNumSibs;
	int iValue;
	TCHAR szTextPosition[256];
	TCHAR szTextVal[256];
	int* piList = NULL;

	if (0 > iInfoBaseLength)
	{
		char	szMessage[1024];
		char* tmp = xercesc::XMLString::transcode(szRootTagName);
		sprintf( szMessage, "Allocating zero or less memory in CvXMLLoadUtility::SetVariableListTagPair (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
		xercesc::XMLString::release(&tmp);
		gDLL->MessageBox(szMessage, "XML Error");
	}

	if (TryMoveToXmlFirstChild(szRootTagName))
	{
		iNumSibs = GetXmlChildrenNumber();
		if (0 < iNumSibs)
		{
			InitList(ppiList, iInfoBaseLength, iDefaultListVal);
			piList = *ppiList;
			if(!(iNumSibs <= iInfoBaseLength))
			{
				char	szMessage[1024];
				char* tmp = xercesc::XMLString::transcode(szRootTagName);
				sprintf( szMessage, "There are more siblings than memory allocated for them in CvXMLLoadUtility::SetVariableListTagPair (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
				xercesc::XMLString::release(&tmp);
				gDLL->MessageBox(szMessage, "XML Error");
			}
			if (TryMoveToXmlFirstChild())
			{
				for (i=0;i<iNumSibs;i++)
				{
					if (GetChildXmlVal(szTextPosition))
					{
						iIndexVal = GetInfoClass(szTextPosition);

						if (iIndexVal != -1)
						{
							MoveToXmlParent();
							GetOptionalChildXmlValByName(szTextVal, szValueTagName);
							iValue = GetInfoClass(szTextVal);
							if( (iValue < -1) || (iValue >= iValueInfoBaseLength) )
							{
								char	szMessage[1024];
								char* tmp = xercesc::XMLString::transcode(szRootTagName);
								sprintf( szMessage, "A defined value for an array is outside of the accepted size of the infoclass! (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
								xercesc::XMLString::release(&tmp);
								gDLL->MessageBox(szMessage, "XML Error");
							} else
							{
								piList[iIndexVal] = iValue;
							}
						} else
						{
							MoveToXmlParent();
						}
					}

					if (!TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				MoveToXmlParent();
			}
		}

		MoveToXmlParent();
	}
}
/************************************************************************************************/
/* RevDCM	                                 END                                                */
/************************************************************************************************/

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetVariableListTagPair(	bool **ppbList, const TCHAR* szRootTagName,
//										int iInfoBaseSize, int iInfoBaseLength, bool bDefaultListVal)
//
//  PURPOSE :   allocate and initialize a list from a tag pair in the xml
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetVariableListTagPair(bool **ppbList, const wchar_t* szRootTagName, int iInfoBaseLength, bool bDefaultListVal)
{
	int i;
	int iIndexVal;
	int iNumSibs;
	TCHAR szTextVal[256];
	bool* pbList;

	if(!(0 < iInfoBaseLength))
	{
		char	szMessage[1024];
		char* tmp = xercesc::XMLString::transcode(szRootTagName);
		sprintf( szMessage, "Allocating zero or less memory in CvXMLLoadUtility::SetVariableListTagPair (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
		xercesc::XMLString::release(&tmp);
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if (TryMoveToXmlFirstChild(szRootTagName))
	{
		iNumSibs = GetXmlChildrenNumber();
		if (0 < iNumSibs)
		{
			InitList(ppbList, iInfoBaseLength, bDefaultListVal);
			pbList = *ppbList;
			if(!(iNumSibs <= iInfoBaseLength))
			{
				char	szMessage[1024];
				char* tmp = xercesc::XMLString::transcode(szRootTagName);
				sprintf( szMessage, "There are more siblings than memory allocated for them in CvXMLLoadUtility::SetVariableListTagPair (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
				xercesc::XMLString::release(&tmp);
				gDLL->MessageBox(szMessage, "XML Error");
			}
			if (TryMoveToXmlFirstChild())
			{
				for (i=0;i<iNumSibs;i++)
				{
					if (GetChildXmlVal(szTextVal))
					{
						iIndexVal = GetInfoClass(szTextVal);
						if (iIndexVal != -1)
						{
							GetNextXmlVal(&pbList[iIndexVal]);
						}

						MoveToXmlParent();
					}

					if (!TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				MoveToXmlParent();
			}
		}

		MoveToXmlParent();
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetVariableListTagPair(	float **ppfList, const TCHAR* szRootTagName,
//										int iInfoBaseSize, int iInfoBaseLength, float fDefaultListVal)
//
//  PURPOSE :   allocate and initialize a list from a tag pair in the xml
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetVariableListTagPair(float **ppfList, const wchar_t* szRootTagName, int iInfoBaseLength, float fDefaultListVal)
{
	int i;
	int iIndexVal;
	int iNumSibs;
	TCHAR szTextVal[256];
	float* pfList;

	if(!(0 < iInfoBaseLength))
	{
		char	szMessage[1024];
		char* tmp = xercesc::XMLString::transcode(szRootTagName);
		sprintf( szMessage, "Allocating zero or less memory in CvXMLLoadUtility::SetVariableListTagPair (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
		xercesc::XMLString::release(&tmp);
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if (TryMoveToXmlFirstChild(szRootTagName))
	{
		iNumSibs = GetXmlChildrenNumber();
		if (0 < iNumSibs)
		{
			InitList(ppfList, iInfoBaseLength, fDefaultListVal);
			pfList = *ppfList;
			if(!(iNumSibs <= iInfoBaseLength))
			{
				char	szMessage[1024];
				char* tmp = xercesc::XMLString::transcode(szRootTagName);
				sprintf( szMessage, "There are more siblings than memory allocated for them in CvXMLLoadUtility::SetVariableListTagPair (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
				xercesc::XMLString::release(&tmp);
				gDLL->MessageBox(szMessage, "XML Error");
			}
			if (TryMoveToXmlFirstChild())
			{
				for (i=0;i<iNumSibs;i++)
				{
					if (GetChildXmlVal(szTextVal))
					{
						iIndexVal = GetInfoClass(szTextVal);
						if (iIndexVal != -1)
						{
							GetNextXmlVal(&pfList[iIndexVal]);
						}

						MoveToXmlParent();
					}

					if (!TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				MoveToXmlParent();
			}
		}

		MoveToXmlParent();
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetVariableListTagPair(	CvString **ppfList, const TCHAR* szRootTagName,
//										int iInfoBaseSize, int iInfoBaseLength, CvString szDefaultListVal)
//
//  PURPOSE :   allocate and initialize a list from a tag pair in the xml
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetVariableListTagPair(CvString **ppszList, const wchar_t* szRootTagName, int iInfoBaseLength, CvString szDefaultListVal)
{
	int i;
	int iIndexVal;
	int iNumSibs;
	TCHAR szTextVal[256];
	CvString* pszList;

	if(!(0 < iInfoBaseLength))
	{
		char	szMessage[1024];
		char* tmp = xercesc::XMLString::transcode(szRootTagName);
		sprintf( szMessage, "Allocating zero or less memory in CvXMLLoadUtility::SetVariableListTagPair (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
		xercesc::XMLString::release(&tmp);
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if (TryMoveToXmlFirstChild(szRootTagName))
	{
		iNumSibs = GetXmlChildrenNumber();
		if (0 < iNumSibs)
		{
			InitList(ppszList, iInfoBaseLength, szDefaultListVal);
			pszList = *ppszList;
			if(!(iNumSibs <= iInfoBaseLength))
			{
				char	szMessage[1024];
				char* tmp = xercesc::XMLString::transcode(szRootTagName);
				sprintf( szMessage, "There are more siblings than memory allocated for them in CvXMLLoadUtility::SetVariableListTagPair (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
				xercesc::XMLString::release(&tmp);
				gDLL->MessageBox(szMessage, "XML Error");
			}
			if (TryMoveToXmlFirstChild())
			{
				for (i=0;i<iNumSibs;i++)
				{
					if (GetChildXmlVal(szTextVal))
					{
						iIndexVal = GetInfoClass(szTextVal);
						if (iIndexVal != -1)
						{
							GetNextXmlVal(pszList[iIndexVal]);
						}

						MoveToXmlParent();
					}

					if (!TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				MoveToXmlParent();
			}
		}

		MoveToXmlParent();
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetVariableListTagPair(int **ppiList, const TCHAR* szRootTagName,
//										CvString* m_paszTagList, int iTagListLength, int iDefaultListVal)
//
//  PURPOSE :   allocate and initialize a list from a tag pair in the xml
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetVariableListTagPair(int **ppiList, const wchar_t* szRootTagName,
											  CvString* m_paszTagList, int iTagListLength, int iDefaultListVal)
{
	int i;
	int iIndexVal;
	int iNumSibs;
	TCHAR szTextVal[256];
	int* piList;

	if(!(0 < iTagListLength))
	{
		char	szMessage[1024];
		char* tmp = xercesc::XMLString::transcode(szRootTagName);
		sprintf( szMessage, "Allocating zero or less memory in CvXMLLoadUtility::SetVariableListTagPair (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
		xercesc::XMLString::release(&tmp);
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if (TryMoveToXmlFirstChild(szRootTagName))
	{
		iNumSibs = GetXmlChildrenNumber();
		if (0 < iNumSibs)
		{
			InitList(ppiList, iTagListLength, iDefaultListVal);
			piList = *ppiList;
			if(!(iNumSibs <= iTagListLength))
			{
				char	szMessage[1024];
				char* tmp = xercesc::XMLString::transcode(szRootTagName);
				sprintf( szMessage, "There are more siblings than memory allocated for them in CvXMLLoadUtility::SetVariableListTagPair (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
				xercesc::XMLString::release(&tmp);
				gDLL->MessageBox(szMessage, "XML Error");
			}
			if (TryMoveToXmlFirstChild())
			{
				for (i=0;i<iNumSibs;i++)
				{
					if (GetChildXmlVal(szTextVal))
					{
						iIndexVal = GC.getTypesEnum(szTextVal);
						if (iIndexVal != -1)
						{
							GetNextXmlVal(&piList[iIndexVal]);
						}

						MoveToXmlParent();
					}

					if (!TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				MoveToXmlParent();
			}
		}

		MoveToXmlParent();
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetVariableListTagPair(int **ppiList, const TCHAR* szRootTagName,
//										CvString* m_paszTagList, int iTagListLength, int iDefaultListVal)
//
//  PURPOSE :   allocate and initialize a list from a tag pair in the xml for audio scripts
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetVariableListTagPairForAudioScripts(int **ppiList, const wchar_t* szRootTagName,
															 CvString* m_paszTagList, int iTagListLength, int iDefaultListVal)
{
	int i;
	int iIndexVal;
	int iNumSibs;
	TCHAR szTextVal[256];
	int* piList;
	CvString szTemp;

	if (TryMoveToXmlFirstChild(szRootTagName))
	{
		iNumSibs = GetXmlChildrenNumber();
		if(!(0 < iTagListLength))
		{
			char	szMessage[1024];
			char* tmp = xercesc::XMLString::transcode(szRootTagName);
			sprintf( szMessage, "Allocating zero or less memory in CvXMLLoadUtility::SetVariableListTagPairForAudio (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
			xercesc::XMLString::release(&tmp);
			gDLL->MessageBox(szMessage, "XML Error");
		}
		if (0 < iNumSibs)
		{
			InitList(ppiList, iTagListLength, iDefaultListVal);
			piList = *ppiList;
			if(!(iNumSibs <= iTagListLength))
			{
				char	szMessage[1024];
				char* tmp = xercesc::XMLString::transcode(szRootTagName);
				sprintf( szMessage, "There are more siblings than memory allocated for them in CvXMLLoadUtility::SetVariableListTagPairForAudio (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
				xercesc::XMLString::release(&tmp);
				gDLL->MessageBox(szMessage, "XML Error");
			}
			if (TryMoveToXmlFirstChild())
			{
				for (i=0;i<iNumSibs;i++)
				{
					if (GetChildXmlVal(szTextVal))
					{
						iIndexVal =	GC.getTypesEnum(szTextVal);
						if (iIndexVal != -1)
						{
							GetNextXmlVal(szTemp);
							if ( szTemp.GetLength() > 0 )
								piList[iIndexVal] = gDLL->getAudioTagIndex(szTemp);
							else
								piList[iIndexVal] = -1;
						}

						MoveToXmlParent();
					}

					if (!TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				MoveToXmlParent();
			}
		}

		MoveToXmlParent();
	}
/************************************************************************************************/
/* XMLCOPY                                 10/15/07                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	else
	{
		InitList(ppiList, iTagListLength, iDefaultListVal);
	}
/************************************************************************************************/
/* XMLCOPY                                 END                                                  */
/************************************************************************************************/
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetVariableListTagPairForAudioScripts(int **ppiList, const TCHAR* szRootTagName,
//										int iInfoBaseLength, int iDefaultListVal)
//
//  PURPOSE :   allocate and initialize a list from a tag pair in the xml for audio scripts
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetVariableListTagPairForAudioScripts(int **ppiList, const wchar_t* szRootTagName, int iInfoBaseLength, int iDefaultListVal)
{
	int i;
	int iIndexVal;
	int iNumSibs;
	TCHAR szTextVal[256];
	int* piList;
	CvString szTemp;

	if (TryMoveToXmlFirstChild(szRootTagName))
	{
		iNumSibs = GetXmlChildrenNumber();
		if(!(0 < iInfoBaseLength))
		{
			char	szMessage[1024];
			char* tmp = xercesc::XMLString::transcode(szRootTagName);
			sprintf( szMessage, "Allocating zero or less memory in CvXMLLoadUtility::SetVariableListTagPair (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
			xercesc::XMLString::release(&tmp);
			gDLL->MessageBox(szMessage, "XML Error");
		}
		if (0 < iNumSibs)
		{
			InitList(ppiList, iInfoBaseLength, iDefaultListVal);
			piList = *ppiList;
			if(!(iNumSibs <= iInfoBaseLength))
			{
				char	szMessage[1024];
				char* tmp = xercesc::XMLString::transcode(szRootTagName);
				sprintf( szMessage, "There are more siblings than memory allocated for them in CvXMLLoadUtility::SetVariableListTagPair (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
				xercesc::XMLString::release(&tmp);
				gDLL->MessageBox(szMessage, "XML Error");
			}
			if (TryMoveToXmlFirstChild())
			{
				for (i=0;i<iNumSibs;i++)
				{
					if (GetChildXmlVal(szTextVal))
					{
						iIndexVal = GetInfoClass(szTextVal);
						if (iIndexVal != -1)
						{
							GetNextXmlVal(szTemp);
							if ( szTemp.GetLength() > 0 )
								piList[iIndexVal] = gDLL->getAudioTagIndex(szTemp);
							else
								piList[iIndexVal] = -1;
						}

						MoveToXmlParent();
					}

					if (!TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				MoveToXmlParent();
			}
		}

		MoveToXmlParent();
	}
/************************************************************************************************/
/* XMLCOPY                                 10/12/07                                MRGENIE      */
/*                                                                                              */
/* Always create this array, for XML copy comparison                                            */
/************************************************************************************************/
	else
	{
		InitList(ppiList, iInfoBaseLength, iDefaultListVal);
	}
/************************************************************************************************/
/* XMLCOPY                                 END                                                  */
/************************************************************************************************/
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   SetVariableListTagPair(bool **ppbList, const TCHAR* szRootTagName,
//										CvString* m_paszTagList, int iTagListLength, int iDefaultListVal)
//
//  PURPOSE :   allocate and initialize a list from a tag pair in the xml
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetVariableListTagPair(bool **ppbList, const wchar_t* szRootTagName,
											  CvString* m_paszTagList, int iTagListLength, bool bDefaultListVal)
{
	int i;
	int iIndexVal;
	int iNumSibs;
	TCHAR szTextVal[256];
	bool* pbList;

	if(!(0 < iTagListLength))
	{
		char	szMessage[1024];
		char* tmp = xercesc::XMLString::transcode(szRootTagName);
		sprintf( szMessage, "Allocating zero or less memory in CvXMLLoadUtility::SetVariableListTagPair (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
		xercesc::XMLString::release(&tmp);
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if (TryMoveToXmlFirstChild(szRootTagName))
	{
			iNumSibs = GetXmlChildrenNumber();
			if (0 < iNumSibs)
			{
			InitList(ppbList, iTagListLength, bDefaultListVal);
			pbList = *ppbList;
			if(!(iNumSibs <= iTagListLength))
			{
				char	szMessage[1024];
				char* tmp = xercesc::XMLString::transcode(szRootTagName);
				sprintf( szMessage, "There are more siblings than memory allocated for them in CvXMLLoadUtility::SetVariableListTagPair (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
				xercesc::XMLString::release(&tmp);
				gDLL->MessageBox(szMessage, "XML Error");
			}
			if (TryMoveToXmlFirstChild())
			{
				for (i=0;i<iNumSibs;i++)
				{
					if (GetChildXmlVal(szTextVal))
					{
						iIndexVal =	GC.getTypesEnum(szTextVal);
						if (iIndexVal != -1)
						{
							GetNextXmlVal(&pbList[iIndexVal]);
						}

						MoveToXmlParent();
					}

					if (!TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				MoveToXmlParent();
			}
		}

		MoveToXmlParent();
	}
}

//------------------------------------------------------------------------------------------------------
//
//	FUNCTION:	SetVariableListTagPair(CvString **ppszList, const TCHAR* szRootTagName,
//							CvString* m_paszTagList, int iTagListLength, CvString szDefaultListVal = "")
//
//  PURPOSE :   allocate and initialize a list from a tag pair in the xml
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::SetVariableListTagPair(CvString **ppszList, const wchar_t* szRootTagName,
											  CvString* m_paszTagList, int iTagListLength, CvString szDefaultListVal)
{
	int i;
	int iIndexVal;
	int iNumSibs;
	TCHAR szTextVal[256];
	CvString* pszList;

	if(!(0 < iTagListLength))
	{
		char	szMessage[1024];
		char*   tmp = xercesc::XMLString::transcode(szRootTagName);
		sprintf( szMessage, "Allocating zero or less memory in CvXMLLoadUtility::SetVariableListTagPair (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
		xercesc::XMLString::release(&tmp);
		gDLL->MessageBox(szMessage, "XML Error");
	}
	if (TryMoveToXmlFirstChild(szRootTagName))
	{
		iNumSibs = GetXmlChildrenNumber();
		if (0 < iNumSibs)
		{
			InitList(ppszList, iTagListLength, szDefaultListVal);
			pszList = *ppszList;
			if(!(iNumSibs <= iTagListLength))
			{
				char	szMessage[1024];
				char* tmp = xercesc::XMLString::transcode(szRootTagName);
				sprintf( szMessage, "There are more siblings than memory allocated for them in CvXMLLoadUtility::SetVariableListTagPair (tag: %s)\n Current XML file is: %s", tmp, GC.getCurrentXMLFile().GetCString());
				xercesc::XMLString::release(&tmp);
				gDLL->MessageBox(szMessage, "XML Error");
			}
			if (TryMoveToXmlFirstChild())
			{
				for (i=0;i<iNumSibs;i++)
				{
					if (GetChildXmlVal(szTextVal))
					{
						iIndexVal =	GC.getTypesEnum(szTextVal);
						if (iIndexVal != -1)
						{
							GetNextXmlVal(pszList[iIndexVal]);
						}

						MoveToXmlParent();
					}

					if (!TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				MoveToXmlParent();
			}
		}

		MoveToXmlParent();
	}
}

DllExport bool CvXMLLoadUtility::LoadPlayerOptions()
{
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 10/30/07                            MRGENIE          */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	// MLF loading
	if (!gDLL->isModularXMLLoading())
	{
		if (!CreateFXml())
			return false;

		ModularLoadingControlXML();

		DestroyFXml();
	}
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/
	if (!CreateFXml())
		return false;

	LoadGlobalClassInfo(GC.m_paPlayerOptionInfos, "CIV4PlayerOptionInfos", "GameInfo", L"/Civ4PlayerOptionInfos/PlayerOptionInfos/PlayerOptionInfo", false);
	FAssert(static_cast<int>(GC.m_paPlayerOptionInfos.size()) == NUM_PLAYEROPTION_TYPES);

	DestroyFXml();
	return true;
}

DllExport bool CvXMLLoadUtility::LoadGraphicOptions()
{
	if (!CreateFXml())
		return false;

	LoadGlobalClassInfo(GC.m_paGraphicOptionInfos, "CIV4GraphicOptionInfos", "GameInfo", L"/Civ4GraphicOptionInfos/GraphicOptionInfos/GraphicOptionInfo", false);
	FAssert(static_cast<int>(GC.m_paGraphicOptionInfos.size()) == NUM_GRAPHICOPTION_TYPES);

	DestroyFXml();
	return true;
}

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 10/24/07                            MRGENIE          */
/*                                                                                              */
/* Initialize the list of Module Directories to be loaded                                       */
/************************************************************************************************/
// Main control of the MLF feature
void CvXMLLoadUtility::ModularLoadingControlXML()
{
	CvXMLLoadUtilitySetMod* pSetMod = new CvXMLLoadUtilitySetMod;
	pSetMod->setModLoadControlDirArray(LoadModLoadControlInfo(GC.m_paModLoadControls, "CIV4ModularLoadingControls", L"Type"));
	SAFE_DELETE(pSetMod);
}

// In the next 2 methods we load the MLF classes
template <class T>
bool CvXMLLoadUtility::LoadModLoadControlInfo(std::vector<T*>& aInfos, const char* szFileRoot, const wchar_t* szXmlPath)
{
	GC.addToInfosVectors(&aInfos);
	DEBUG_LOG("MLF.log", "Entering MLF");
	DEBUG_LOG("XmlCheckDoubleTypes.log", "\nEntering: MLF_CIV4ModularLoadingControls\n");

	bool bContinue = true;
	int m_iDirDepth = 0;

	CvXMLLoadUtilityModTools* pProgramDir = new CvXMLLoadUtilityModTools;

	std::string szDirDepth = "modules\\";
	std::string szModDirectory = "modules";
	std::string szConfigString;

	bool bLoaded = LoadCivXml(NULL, CvString::format("%s\\MLF_%s.xml", szModDirectory.c_str(), szFileRoot));

	if (!bLoaded)
	{
		DEBUG_LOG("MLF.log", "MLF not found, you will now load the modules without Modular Loading Control");
		SAFE_DELETE(pProgramDir);
		return false;
	}
	else
	{
		if ( TryMoveToXmlFirstMatchingElement(L"/Civ4ModularLoadControls/DefaultConfiguration"))
		{
			// call the function that sets the FXml pointer to the first non-comment child of
			// the current tag and gets the value of that new node

			GetXmlVal(szConfigString, "NONE");

			if (szConfigString == "NONE")
			{
				DEBUG_LOG("MLF.log", "The default configuration in \"%s\\MLF_%s.xml\" was set to \"NONE\", you will continue loading the regular Firaxian method", szModDirectory.c_str(), szFileRoot);
				SAFE_DELETE(pProgramDir);
				return false;   // abort without enumerating anything
			}
		}
		else
		{
			DEBUG_LOG("MLF.log", "The default configuration in \"%s\\MLF_%s.xml\" couldn't be found, you will continue loading using the regular Firaxian method", szModDirectory.c_str(), szFileRoot);
			SAFE_DELETE(pProgramDir);
			return false;
		}

		if (!SetModLoadControlInfo(aInfos, szXmlPath, szConfigString, szDirDepth, m_iDirDepth))
		{
			DEBUG_LOG("MLF.log", "The default configuration in \"%s\\MLF_%s.xml\" set by you could not be found, please check your XML settings!", szModDirectory.c_str(), szFileRoot);
			SAFE_DELETE(pProgramDir);
			return false;
		}

		//	We want a top to bottom control mechanism. If in any new level there wasn't found a
		//	new MLF we don't want to loop further downwards into the directory hyrarchy
		while (bContinue)
		{
			m_iDirDepth++;
			bContinue = false;	//we want to stop the while loop, unless a new MLF will be found
			//loop through all MLF's so far loaded
			for ( int iInfos = 0; iInfos < GC.getNumModLoadControlInfos(); iInfos++)
			{
				//only loop through files in the actual depth
				if (GC.getModLoadControlInfos(iInfos).getDirDepth() + 1 == m_iDirDepth)
				{
					//loop through the modules of each MLF
					for ( int i = 0; i < GC.getModLoadControlInfos(iInfos).getNumModules(); i++ )
					{
						if ( GC.getModLoadControlInfos(iInfos).isLoad(i) )
						{

							//each new loop we load the previous dir, and check if a MLF file exist on a lower level
							szModDirectory = GC.getModLoadControlInfos(iInfos).getModuleFolder(i);

							//Check if this Modulefolder is parent to a child MLF
							if ( pProgramDir->isModularArt(CvString::format("%s\\MLF_%s.xml", szModDirectory.c_str(), szFileRoot)))
							{
								bLoaded = LoadCivXml(NULL, CvString::format("%s\\MLF_%s.xml", szModDirectory.c_str(), szFileRoot));
							}
							else
							{
								bLoaded = false;
							}

							if (!bLoaded)
							{
								DEBUG_LOG("MLF.log", "Found module: \"%s\\\"", szModDirectory.c_str());
							}
							else
							{
								if ( TryMoveToXmlFirstMatchingElement(L"/Civ4ModularLoadControls/DefaultConfiguration"))
								{
									// call the function that sets the FXml pointer to the first non-comment child of
									// the current tag and gets the value of that new node

									GetXmlVal(szConfigString, "NONE");

									if (szConfigString == "NONE")
									{
										DEBUG_LOG("MLF.log", "The default configuration in \"%s\\MLF_%s.xml\" was set to \"NONE\", settings in this file will be disregarded", szModDirectory.c_str(), szFileRoot);
									}
									else
									{
										szDirDepth = CvString::format("%s\\", szModDirectory.c_str());
										SetModLoadControlInfo(aInfos, szXmlPath, szConfigString.c_str(), szDirDepth.c_str(), m_iDirDepth);
										bContinue = true; //found a new MLF in a subdir, continue the loop
									}
								}
							}
						}
					}
				}
			}
		}
	}
	SAFE_DELETE(pProgramDir);
	return true;
}

template <class T>
bool CvXMLLoadUtility::SetModLoadControlInfo(std::vector<T*>& aInfos, const wchar_t* szTagName, CvString szConfigString, CvString szDirDepth, int iDirDepth)
{
	OutputDebugString("Setting Mod Control Infos");

	std::string szCandidateConfig;

	if ( TryMoveToXmlFirstMatchingElement(L"/Civ4ModularLoadControls/ConfigurationInfos/ConfigurationInfo"))
	{
		// loop through each tag
		do
		{
			szCandidateConfig = "NONE";
			GetChildXmlValByName(szCandidateConfig, szTagName);
			if (szCandidateConfig == szConfigString)
			{
				std::auto_ptr<T> pClassInfo(new T);

				bool bSuccess = pClassInfo->read(this, szDirDepth, iDirDepth);
				if (!bSuccess)
				{
					FErrorMsg(CvString::format("Couldn't read %s dir %s", szConfigString.c_str(), szDirDepth.c_str()));
					break;
				}

				int iIndex = -1;
				if (NULL != pClassInfo->getType())
				{
					iIndex = GC.getInfoTypeForString(pClassInfo->getType(), true);
					if ( iIndex != -1 )
					{
						DEBUG_LOG("MLF.log", "Type \"%s\" is specified more than once", pClassInfo->getType());
						//Catch dupes here, we don't want the overwrite or copy method for the MLF
						FErrorMsg(CvString::format("The <type>%s</type> of the \"MLF_CIV4ModularLoadingControls.xml\" in directory: \"%s\" is already in use, please use an alternative <type> -name", pClassInfo->getType(), szDirDepth.c_str()));

						return false;
					}
					else
					{
						GC.setInfoTypeFromString(pClassInfo->getType(), (int)aInfos.size());	// add type to global info type hash map

						aInfos.push_back(pClassInfo.release());

						return true;
					}
				}
				aInfos.push_back(pClassInfo.release());
			}
		} while (TryMoveToXmlNextSibling());
	}
	return false;
}
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL                 END                                                  */
/************************************************************************************************/

/************************************************************************************************/
/* SORT_ALPHABET                           11/17/07                                MRGENIE      */
/*                                                                                              */
/* Method for alphabetically order tags                                                         */
/************************************************************************************************/
//template <class T>
//void CvXMLLoadUtility::SortAlphabetGlobalClassInfo(std::vector<T*>& aInfos)
//{
//	int iLenghtInfos = aInfos.size();	//Number of Elements
//	T* pTempClassInfo = new T;	// Temp to store while swapping positions
//
//	for ( int iI = 0; iI < iLenghtInfos; iI++ )
//	{
//		int minIndex = iI; //stores index of the min array value
//
//		for ( int iJ = iI + 1; iJ < iLenghtInfos; iJ++ )
//		{
//			if ( CvWString::format(aInfos[iJ]->getTextKeyWide()) < CvWString::format(aInfos[minIndex]->getTextKeyWide()) )
//			{
//				minIndex = iJ;
//			}
//		}
//
//		//swap the Info classes at positions iI and minIndex
//		pTempClassInfo = aInfos[minIndex];
//		aInfos[minIndex] = aInfos[iI];
//		aInfos[iI] = pTempClassInfo;		// This is the actual new order
//
//		// Set the InfoTypeFromString map properly according to the new alphabetical order
//		if (NULL != aInfos[iI]->getType())
//		{
//			// overwrite existing info maps with the proper id's
//			GC.setInfoTypeFromString(aInfos[iI]->getType(), iI);
//		}
//	}
//}
/************************************************************************************************/
/* SORT_ALPHABET                           END                                                  */
/************************************************************************************************/

void CvXMLLoadUtility::RemoveTGAFiller()
{
	std::vector<CvReligionInfo*>& aInfos1 = GC.m_paReligionInfo;
	std::vector<CvCorporationInfo*>& aInfos2 = GC.m_paCorporationInfo;
	if (aInfos1.size() && aInfos1.size() == GC.getGAMEFONT_TGA_RELIGIONS())
	{
		std::sort(aInfos1.begin(), aInfos1.end(), cmpReligionTGA);
		if (aInfos1.front()->getTGAIndex() == -1)
		{
			std::vector<CvReligionInfo*>::iterator it = aInfos1.begin();
			while (it != aInfos1.end() && (*it)->getTGAIndex() == -1) {it++;}
			SAFE_DELETE(aInfos1.front())
			aInfos1.erase(aInfos1.begin(), it);
		}
	}
	if (aInfos2.size() && aInfos2.size() == GC.getGAMEFONT_TGA_CORPORATIONS())
	{
		std::sort(aInfos2.begin(), aInfos2.end(), cmpCorporationTGA);
		if (aInfos2.front()->getTGAIndex() == -1)
		{
			std::vector<CvCorporationInfo*>::iterator it = aInfos2.begin();
			while (it != aInfos2.end() && (*it)->getTGAIndex() == -1) {it++;}
			SAFE_DELETE(aInfos2.front())
			aInfos2.erase(aInfos2.begin(), it);
		}
	}
}
