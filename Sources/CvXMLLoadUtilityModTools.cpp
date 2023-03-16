//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvXMLLoadUtilityModTools.cpp
//
//  AUTHOR:  Vincent Veldman  --  10/2007
//
//  PURPOSE: Group of functions to enable true Modular loading for Civilization 4 BtS
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2003 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvInitCore.h"
#include "CvXMLLoadUtility.h"
#include "CvXMLLoadUtilityModTools.h"

using namespace std;

/************************************************************************************************/
/* DEBUG_IS_MODULAR_ART                    05/12/08                                Faichele     */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
#define DEBUG_IS_MODULAR_ART 0
/************************************************************************************************/
/* DEBUG_IS_MODULAR_ART                    END                                                  */
/************************************************************************************************/

namespace CvXMLLoadUtilityModTools
{
	//function to expand string arrays.
	//i.e. original XML holds 60citynames for america
	//you add 5new in a module, this one will append those 5
	//to the array of 60(technically, it's the other way around,but who cares)
	void StringArrayExtend(	CvString **ppszListNew, int* pNumNewListElements,
							CvString **ppszListOld, int iNumOldListElements,
							CvString szDefaultListVal)
	{
		CvString* pszListNew;
		CvString* pszListOld;
		pszListNew = *ppszListNew;
		pszListOld = *ppszListOld;
		int m_iAddNumNames = 0;
		int iCount;
		int iNumNewListElements = *pNumNewListElements;

		for ( int j = 0; j < iNumNewListElements; j++ )
		{
			iCount = 0;
			for ( int i = 0; i < iNumOldListElements; i++ )
			{
				if ( pszListNew[j] == pszListOld[i] )
				{
					break;
				}
				else
				{
					iCount++;
				}
			}
			if ( iCount == iNumOldListElements ) //it's not a dupe, we want to add it to the array
			{
				m_iAddNumNames++;		// Nr of Elements to add to the new array
			}
			else  //set empty, don't want a dupe!
			{
				pszListNew[j] = szDefaultListVal;
			}
		}

		// Make a temp Array of the new size
		CvString* m_paszTempList = new CvString[iNumOldListElements + m_iAddNumNames];
		iCount = 0;

		//read values from previous infoclass object, we want them to be first...
		for ( int i = 0; i < iNumOldListElements; i++ )
		{
			m_paszTempList[i] = pszListOld[i];
			iCount++;
		}

		//read values from new infoclass object
		for ( int i = 0; i < iNumNewListElements; i++ )
		{
			if ( pszListNew[i] != szDefaultListVal)
			{
				m_paszTempList[iCount] = pszListNew[i];
				iCount++;
			}
		}

		// set Number of array elements need for the additional list plus old list
		*pNumNewListElements = iNumOldListElements + m_iAddNumNames;
		// delete infoclass array ppszListNew
		if ( NULL != *ppszListNew )
		{
			SAFE_DELETE_ARRAY(*ppszListNew);
		}
		//new instance of infoclass array with new number of elements
		*ppszListNew = new CvString[*pNumNewListElements];
		pszListNew = *ppszListNew;

		// copy values from Temp array
		for ( int i = 0; i < *pNumNewListElements; i++ )
		{
			pszListNew[i] = m_paszTempList[i];
		}
		//Delete the temp array
		SAFE_DELETE_ARRAY(m_paszTempList);
	}


	bool isModularArt(const char* szLocationName)
	{
	/************************************************************************************************/
	/* DEBUG_IS_MODULAR_ART                    05/12/08                                Faichele     */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/
	#if (DEBUG_IS_MODULAR_ART == 1)
		CvString szDebugBuffer;
		szDebugBuffer.Format("=== isModularArt BEGIN ===");
		gDLL->logMsg("CvXMLLoadUtilityModTools_isModularArt.log", szDebugBuffer.c_str());

		szDebugBuffer.Format(" Location name: '%s'", szLocationName);
		gDLL->logMsg("CvXMLLoadUtilityModTools_isModularArt.log", szDebugBuffer.c_str());
	#endif

		//the passed is crap, we don't want to continue anything with it
		if (szLocationName == NULL || strcmp(szLocationName, "") == 0 || strcmp(szLocationName, "None") == 0 || strcmp(szLocationName, "NONE") == 0 )
		{
	#if (DEBUG_IS_MODULAR_ART == 1)
			szDebugBuffer.Format(" EMPTY location, aborting.");
			gDLL->logMsg("CvXMLLoadUtilityModTools_isModularArt.log", szDebugBuffer.c_str());
	#endif
			return false;
		}

		// Dir where the Civ4BeyondSword.exe is started from
	/************************************************************************************************/
	/* Afforess	                  Start		 06/15/10                                               */
	/*                                                                                              */
	/*                                                                                              */
	/************************************************************************************************/
		CvString m_szFolderPath = GC.getInitCore().getDLLPath() + "\\";
	/************************************************************************************************/
	/* Afforess	                     END                                                            */
	/************************************************************************************************/

		#if (DEBUG_IS_MODULAR_ART == 1)
			szDebugBuffer.Format(" Complete path (with Assets dir): %s", m_szFolderPath.c_str());
			gDLL->logMsg("CvXMLLoadUtilityModTools_isModularArt.log", szDebugBuffer.c_str());
		#endif

		m_szFolderPath += szLocationName;		// where the tag points to, usually "Art\filename.xxx"

	#if (DEBUG_IS_MODULAR_ART == 1)
		szDebugBuffer.Format("  Path WITH given location: '%s'", m_szFolderPath.c_str());
		gDLL->logMsg("CvXMLLoadUtilityModTools_isModularArt.log", szDebugBuffer.c_str());
	#endif

		int iReplaced = m_szFolderPath.Replace('/','\\');
	#if (DEBUG_IS_MODULAR_ART == 1)

		szDebugBuffer.Format("  Replaced %i occurrences of '/' with '\\'.", iReplaced);
		gDLL->logMsg("CvXMLLoadUtilityModTools_isModularArt.log", szDebugBuffer.c_str());

	#endif

		iReplaced = m_szFolderPath.Replace(CvString("\\\\"),CvString("\\"));
	#if (DEBUG_IS_MODULAR_ART == 1)
		szDebugBuffer.Format("  Replaced %i occurrences of '\\\\' with '\\'.", iReplaced);
		gDLL->logMsg("CvXMLLoadUtilityModTools_isModularArt.log", szDebugBuffer.c_str());

		szDebugBuffer.Format("  Path WITH given location: '%s'", m_szFolderPath.c_str());
		gDLL->logMsg("CvXMLLoadUtilityModTools_isModularArt.log", szDebugBuffer.c_str());
	#endif
		FILE *file;
		DWORD dwAttr = GetFileAttributes(m_szFolderPath.c_str());
		if(dwAttr != 0xffffffff && (dwAttr & FILE_ATTRIBUTE_DIRECTORY))
		{
	#if (DEBUG_IS_MODULAR_ART == 1)
			szDebugBuffer.Format(" '%s' is a DIRECTORY, no modular art?", m_szFolderPath.c_str());
			gDLL->logMsg("CvXMLLoadUtilityModTools_isModularArt.log", szDebugBuffer.c_str());
	#endif
			return false;
		}
		else
		{
			file = fopen(m_szFolderPath.c_str(), "rb");
		}

	/************************************************************************************************/
	/* DEBUG_IS_MODULAR_ART                    END                                                  */
	/************************************************************************************************/

		if (file == NULL)
		{
			return false;
		}
		fclose(file);

	#if (DEBUG_IS_MODULAR_ART == 1)
		szDebugBuffer.Format("=== isModularArt END ===");
		gDLL->logMsg("CvXMLLoadUtilityModTools_isModularArt.log", szDebugBuffer.c_str());
	#endif

		return true;
	}
}