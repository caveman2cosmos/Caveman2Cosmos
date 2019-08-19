//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvXMLLoadUtilityModTools.h
//
//  AUTHOR:  Vincent Veldman  --  10/2007
//
//  PURPOSE: Group of functions to enable true Modular loading for Civilization 4 BtS
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2003 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef XML_LOAD_UTILITY_MOD_TOOLS_H
#define XML_LOAD_UTILITY_MOD_TOOLS_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvXMLLoadUtilityModTools
//
//  DESC:   Group of functions to enable true Modular loading for Civilization 4 BtS
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvXMLLoadUtilityModTools
{
//---------------------------------------PUBLIC INTERFACE---------------------------------
public:
	
	void StringArrayExtend( CvString **ppszListNew, int* iNumNewListElements,
							CvString **ppszListOld, int iNumOldListElements,
							CvString szDefaultListVal = "") const;

	bool isDuplicate(int iNumValueNew, int *ppiListNew, int iValueOld) const;
	bool isDuplicate(int iNumValueNew, CvWString *ppiListNew, CvWString wValueOld) const;
	
	void setLocationName( CvString *pszTextVal, const char* szDirName);
	bool isExcludedFile(const char* szLocationName);
	bool isCommaFile(CvString *pszTextVal, const char* szDirName);
	bool isModularArt(const char* szLocationName);
	CvString GetProgramDir();
	CvString deleteFileName(const char* szDirName, const char szLocateChar);

	void writeThm(const char* szTextVal);

};

#endif	// XML_LOAD_UTILITY_H
