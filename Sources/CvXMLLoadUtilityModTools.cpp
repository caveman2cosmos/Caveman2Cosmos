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
//function to expand string arrays.
//i.e. original XML holds 60citynames for america
//you add 5new in a module, this one will append those 5
//to the array of 60(technically, it's the other way around,but who cares)
void CvXMLLoadUtilityModTools::StringArrayExtend(CvString **ppszListNew, int* pNumNewListElements,
												 CvString **ppszListOld, int iNumOldListElements,
												 CvString szDefaultListVal) const
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

bool CvXMLLoadUtilityModTools::isDuplicate(int iNumValueNew, int *ppiListNew, int iValueOld) const
{	
	for ( int j = 0; j < iNumValueNew; j++ )
	{	
		if ( ppiListNew[j] == iValueOld)
		{
			return true; 			
		}
	}	
	return false;
}

bool CvXMLLoadUtilityModTools::isDuplicate(int iNumValueNew, CvWString *ppiListNew, CvWString wValueOld) const
{	
	for ( int j = 0; j < iNumValueNew; j++ )
	{	
		if ( ppiListNew[j] == wValueOld)
		{
			return true; 			
		}
	}	
	return false;
}

// WARNING WARNING WARNING WARNING
// the p_szLogWrite->XmlArtTagVerification can cause CTD because of memory overflow
// If too much is parsed into the XmlArtTagVerification method!!
void CvXMLLoadUtilityModTools::setLocationName( CvString *pszTextVal, const char* szDirName)
{
#if (DEBUG_IS_MODULAR_ART == 1)
	CvString szDebugBuffer;
	szDebugBuffer.Format("=== setLocationName BEGIN ===");
	gDLL->logMsg("CvXMLLoadUtilityModTools_setLocationName.log", szDebugBuffer.c_str());

	szDebugBuffer.Format(" Text value: '%s', Directory name: '%s'", pszTextVal->c_str(), szDirName);
	gDLL->logMsg("CvXMLLoadUtilityModTools_setLocationName.log", szDebugBuffer.c_str());
#endif

	std::string szFiraxisLoad = "NONE";
	if (szDirName == szFiraxisLoad) //non modular loading doesn't need this method
	{
//#if (DEBUG_IS_MODULAR_ART == 1)
//	szDebugBuffer.Format("DirName == NONE, exiting?");
//	gDLL->logMsg("CvXMLLoadUtilityModTools_setLocationName.log", szDebugBuffer.c_str());
//
//	szDebugBuffer.Format("=== setLocationName END ===");
//	gDLL->logMsg("CvXMLLoadUtilityModTools_setLocationName.log", szDebugBuffer.c_str());
//#endif

		return;
	}
	CvString szModular = szDirName;
	CvString szTextVal = *pszTextVal;
	szModular = szModular + szTextVal;

	CvXMLLoadUtility* p_szLogWrite = new CvXMLLoadUtility;

	if (isExcludedFile(szTextVal)) // These are special files that are relative to the Directory of Civ4BeyondSword.exe
	{
		if ( isModularArt(szModular))
		{			
			szTextVal = gDLL->getModName();
			szTextVal += "Assets\\";
			szTextVal += szModular;
			writeThm(szTextVal);
		}
		else
		{
#ifdef _DEBUG
		//the passed is crap, we don't want to continue anything with it
/*
		if ( szTextVal == "" )	// this shouldn't exist
		{
			DEBUG_LOG("XmlArtTagVerification.log", "CRASH WARNING, Your theme <Path> tag is found emtpy in: %s\\%s", szDirName.GetCString(), GC.getCurrentXMLFile().GetCString());
		}
		else if (szTextVal == "None")	// this shouldn't exist!
		{
			DEBUG_LOG("XmlArtTagVerification.log", "CRASH WARNING, Your theme <Path> tag is set to: %s in: %s\\%s", szTextVal.GetCString(), szDirName.GetCString(), GC.getCurrentXMLFile().GetCString());		
		}
		else
		{
		}
*/
			DEBUG_LOG("XmlArtTagVerification.log", "CRASH WARNING, Your theme <Path> %s, seems not to be relative to the Module path: %s", szTextVal.GetCString(), szDirName);		
#endif
		}
	}
	else if (isCommaFile(&szTextVal, szDirName)) // These are tags with a comma, mostly having 2button types even
	{
#ifdef _DEBUG
		//the passed is crap, we don't want to continue anything with it
		if ( szTextVal == "" )
		{
			DEBUG_LOG("XmlArtTagVerification.log", "One art tag is found emtpy in: %s", GC.getCurrentXMLFile().GetCString());
		}
		else if (szTextVal == "None")
		{
			DEBUG_LOG("XmlArtTagVerification.log", "One art tag is set to: %s in: %s", szTextVal.GetCString(), GC.getCurrentXMLFile().GetCString());		
		}
		else
		{		
			DEBUG_LOG("XmlArtTagVerification.log", "One art tag: %s, seems not to be relative to the module path in: %s", szTextVal.GetCString(), GC.getCurrentXMLFile().GetCString());		
		}
#endif
	}	
	else
	{
		if ( isModularArt(szModular))
		{
			szTextVal = szModular;
		}
		else
		{
#ifdef _DEBUG
		//the passed is crap, we don't want to continue anything with it
		if ( szTextVal == "" )
		{
			DEBUG_LOG("XmlArtTagVerification.log", "One art tag is found emtpy in: %s", GC.getCurrentXMLFile().GetCString());
		}
		else if (szTextVal == "None")
		{
			DEBUG_LOG("XmlArtTagVerification.log", "One art tag is set to: %s in: %s", szTextVal.GetCString(), GC.getCurrentXMLFile().GetCString());		
		}
		else
		{		
			DEBUG_LOG("XmlArtTagVerification.log", "One art tag: %s, seems not to be relative to the module path in: %s", szTextVal.GetCString(), GC.getCurrentXMLFile().GetCString());		
		}
#endif
		}
	}
	*pszTextVal = szTextVal;

#if (DEBUG_IS_MODULAR_ART == 1)
	szDebugBuffer.Format(" Assigned OUT text value: '%s'", pszTextVal->c_str());
	gDLL->logMsg("CvXMLLoadUtilityModTools_setLocationName.log", szDebugBuffer.c_str());

	szDebugBuffer.Format("=== setLocationName END ===");
	gDLL->logMsg("CvXMLLoadUtilityModTools_setLocationName.log", szDebugBuffer.c_str());
#endif

	SAFE_DELETE(p_szLogWrite);
}

bool CvXMLLoadUtilityModTools::isExcludedFile(const char* szLocationName)
{
	CvString szLocationNameStripDot = szLocationName;
	CvString szLocationNameStripTemp = szLocationName;	

	string::size_type posDot = szLocationNameStripDot.find_last_of('.');

	if(posDot != string::npos) 
	{		
		//force lowercase for comparison
		int length = szLocationNameStripDot.size();
		for (int i = 0; i < length; ++i)
		{
			szLocationNameStripTemp[length - (i + 1)] = tolower(szLocationNameStripDot[i]);
		}
		string::size_type posDot = szLocationNameStripTemp.find_last_of('.');
		//delete everything after first dir
		szLocationNameStripTemp.erase(posDot);
		//compare
		if ( szLocationNameStripTemp == "mht" )  //if its a thema file, force static linking
		{
			DEBUG_LOG("XmlArtTagVerification.log", "You are now loading an alternative theme: %s", szLocationName);
			return true;
		}
	}
	return false;
}

bool CvXMLLoadUtilityModTools::isCommaFile(CvString *pszTextVal, const char* szDirName)
{
#if (DEBUG_IS_MODULAR_ART == 1)
	CvString szDebugBuffer;
	szDebugBuffer.Format("=== isCommaFile BEGIN ===");
	gDLL->logMsg("CvXMLLoadUtilityModTools_isCommaFile.log", szDebugBuffer.c_str());

	szDebugBuffer.Format(" Text value: %s,  Directory name: '%s'", pszTextVal->c_str(), szDirName);
	gDLL->logMsg("CvXMLLoadUtilityModTools_isCommaFile.log", szDebugBuffer.c_str());
#endif

	string::size_type posComma = (*pszTextVal).find_first_of(',');

	if(posComma != string::npos) //if no comma found at all, return false
	{

#if (DEBUG_IS_MODULAR_ART == 1)
		szDebugBuffer.Format(" Found a ',' at position %i (total length %i)", (int)posComma, (*pszTextVal).GetLength());
		gDLL->logMsg("CvXMLLoadUtilityModTools_isCommaFile.log", szDebugBuffer.c_str());
#endif
		CvString szTempLocation;
		CvString szAppend = " ";
		int iCountComma = 0;
		CvString szLocationNameStripComma;
		// Check how many comma's we have in the string and how many Button Files		

#if (DEBUG_IS_MODULAR_ART == 1)
		std::vector<CvString> asTagParts;
		pszTextVal->getTokens(CvString(","), asTagParts);
		iCountComma = asTagParts.size();

		szDebugBuffer.Format(" Total number of ',' in pszTextVal: %i", iCountComma);
		gDLL->logMsg("CvXMLLoadUtilityModTools_isCommaFile.log", szDebugBuffer.c_str());
		for (std::vector<CvString>::iterator it = asTagParts.begin(); it != asTagParts.end(); it++)
		{
			szDebugBuffer.Format("  - Token %s", (*it).c_str());
			gDLL->logMsg("CvXMLLoadUtilityModTools_isCommaFile.log", szDebugBuffer.c_str());
		}
#else
		szLocationNameStripComma = *pszTextVal;
		for ( int i = 0; i < szLocationNameStripComma.GetLength(); i++)
		{
			if (szLocationNameStripComma[i] == 44) // "," = 44 (ASCII)
			{
			  iCountComma++;
			}
		}		
#endif

		

#if (DEBUG_IS_MODULAR_ART == 1)
		CvString szButtonsString;
		for (std::vector<CvString>::iterator it = asTagParts.begin(); it != asTagParts.end(); it++)
		{
			bool bDigitsOnly = true;
			for (int i = 0; i < (*it).GetLength(); i++)
			{
				if (!isdigit((*it)[i]))
					bDigitsOnly = false;
			}
			if (!bDigitsOnly)
				szButtonsString += ((*it) + ",");
		}
		// Eliminate comma at end of string, if there is one
		if (szButtonsString.GetLength() > 0 && szButtonsString[szButtonsString.GetLength()] == ',')
			szButtonsString = szButtonsString.substr(0, szButtonsString.GetLength() - 1);

		szDebugBuffer.Format(" Button art string after eliminating numerical indices: %s", szButtonsString.c_str());
		gDLL->logMsg("CvXMLLoadUtilityModTools_isCommaFile.log", szDebugBuffer.c_str());

#else
		// determine the append string at the end of the tag
		bool bContinue = true;
		szTempLocation = *pszTextVal;
		while ( bContinue)
		{
			posComma = szTempLocation.find_first_of(',');
			if(posComma != string::npos) //Prevent Null pointer deletion
			{
				szTempLocation = szTempLocation.substr(szTempLocation.find(",")+1);
				if (isdigit(szTempLocation[0]))  //We found the Append
				{					
					bContinue = false;
				}
			}
			else break;			
		}
		if (!bContinue )
		{
			szAppend = "," + szTempLocation;
		}
#endif

std::vector<CvString> vecButtonArtFile;
#if (DEBUG_IS_MODULAR_ART == 1)
	szButtonsString.getTokens(CvString(","), vecButtonArtFile);
	szDebugBuffer.Format(" Button art vector size after tokenizing by ',': %i", vecButtonArtFile.size());
	gDLL->logMsg("CvXMLLoadUtilityModTools_isCommaFile.log", szDebugBuffer.c_str());
#else
		// set Button Array
		// Array to hold the Button art files
		CvString szTempLocationSubstr;
		szTempLocation = *pszTextVal;
		while (true)
		{
			posComma = szTempLocation.find_first_of(',');
			if(posComma != string::npos) //Prevent Null pointer deletion
			{
				if (szTempLocation[0] == 44) // "," = 44 (ASCII)
				{
					szTempLocation = szTempLocation.substr(szTempLocation.find(",")+1);
				}
				szTempLocationSubstr = szTempLocation;
				posComma = szTempLocationSubstr.find_first_of(',');
				if(posComma != string::npos) //Prevent Null pointer deletion
				{
					szTempLocationSubstr.erase(posComma);
					if (!isdigit(szTempLocationSubstr[0]))
					{
						vecButtonArtFile.push_back(szTempLocationSubstr);
					}
					else break;
				}
				szTempLocation = szTempLocation.substr(szTempLocation.find(",")+1);
			}
			else if (szTempLocation.GetLength() >= 1)
			{
				if (!isdigit(szTempLocationSubstr[0]))
				{
					vecButtonArtFile.push_back(szTempLocationSubstr);
				}
				break;
			}
			else break;
		}		
#endif

		//Check if we need to modularize the files
		bool bNeedChange = false;
/************************************************************************************************/
/* Afforess	                  Start		 06/15/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		CvString m_szFolderPath = GC.getInitCore().getDLLPath() + "//";
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

		m_szFolderPath += szDirName;			// "Modules\Modules\ModuleXXX" 
		for ( unsigned int i = 0; i < vecButtonArtFile.size(); i++)
		{
			szTempLocation = m_szFolderPath;
			szTempLocation += vecButtonArtFile[i];
			//set the Slash properly
			for ( int j = 0; j < szTempLocation.GetLength(); j++)
			{
				if ( szTempLocation[j] == 47)  // 47 = "/"
				{
					szTempLocation[j] = 92;  //92 = "\\", actually 1 backslash of course
				}
			}

			FILE *file = fopen(szTempLocation , "rb");
			if (file != NULL)
			{
				vecButtonArtFile[i] = szDirName + vecButtonArtFile[i];
				fclose(file);
				bNeedChange = true;
			}
		}

		// Now set the new tag string properly
		if (bNeedChange)
		{
			if (szLocationNameStripComma[0] == 44)
			{
				szTempLocation = ",";
			}
			else
			{
				szTempLocation = 0;
			}
			for ( unsigned int i = 0; i < vecButtonArtFile.size(); i++)
			{
				if (i != 0) szTempLocation += ",";
				szTempLocation += vecButtonArtFile[i];
			}
			if (szAppend[0] == 44)   // "," = 44 (ASCII)
			{
				szTempLocation += szAppend;
			}
			*pszTextVal = szTempLocation;
			return true;
		}	
	} 
	return false;
}

bool CvXMLLoadUtilityModTools::isModularArt(const char* szLocationName)
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

CvString CvXMLLoadUtilityModTools::deleteFileName(const char* szDirName, const char szLocateChar)
{	
	CvString szDirNameStrip = szDirName;
	string::size_type pos = szDirNameStrip.find_last_of(szLocateChar);
	if(pos != string::npos) 
	{
		szDirNameStrip.erase(pos + 1);
	}
	else
	{
		FErrorMsg("Couldn't find the directory slash")
	}

	return szDirNameStrip;
}

void CvXMLLoadUtilityModTools::writeThm(const char* szTextVal)
{	
	CvString tszTextVal = szTextVal;
	int posCut;
	string::size_type posBackSlash = tszTextVal.find_last_of('\\');
	string::size_type posSlashForw = tszTextVal.find_last_of('/');

	if (posBackSlash > posSlashForw)
	{
		posCut = posBackSlash;
	}
	else
	{
		posCut = posSlashForw;
	}

	if(posCut != string::npos) 
	{
		tszTextVal.erase(posCut);
	}
	else
	{
		FErrorMsg("Something went wrong with the Theme file");
	}
	
	FILE *pFile = fopen(szTextVal, "w");

	if (pFile == NULL) 
	{
		return;
/*
#ifdef _DEBUG
		CvString szPrint = GetProgramDir();
		szPrint.append(szTextVal);
		DEBUG_LOG("XmlArtTagVerification.log", "Your Theme file: %s seems to be corrupted", szPrint.GetCString());
#endif
*/
	}
	
	//set the proper string to write to the Theme file	
	CvString szOutput = "resource_path\t";	
	szOutput.append("\"");
	szOutput.append(tszTextVal);
	szOutput.append("\";\n");
	szOutput.append("\n");
	szOutput.append("include\t");
	szOutput.append("\"");
	szOutput.append(tszTextVal);
	szOutput.append("\\Themes\\Civ4\\Civ4Theme.thm");	
	szOutput.append("\";\n");

	//write the file
	fputs(szOutput, pFile);
	fclose(pFile);
}
