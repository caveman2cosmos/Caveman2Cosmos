#pragma once
//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvXMLLoadUtilitySetMod.h
//
//  AUTHOR:  Vincent Veldman  --  11/2007
//
//  PURPOSE: Group of functions for Vectorized Enumeration and MLF for Civilization 4 BtS
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2003 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef XML_LOAD_UTILITY_SET_MOD_H
#define XML_LOAD_UTILITY_SET_MOD_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvXMLLoadUtilitySetMod
//
//  DESC:   Group of functions for Vectorized Enumeration and MLF for Civilization 4 BtS
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvXMLLoadUtilitySetMod
{
	//---------------------------------------PUBLIC INTERFACE---------------------------------
public:

    void setModLoadControlDirArray(bool bSetModControl = true);

	void MLFEnumerateFiles(		std::vector<CvString>&	aszFiles,
								const CvString&			refcstrRootDirectory,
								const CvString&			refcstrModularDirectory,
								const CvString&			refcstrExtension,
								bool					bSearchSubdirectories = true);
	bool isValidModule(			bool&							bValid,
								const CvString&					refcstrRootDirectory,
								const CvString&					refcstrModularDirectory,
								const CvString&					refcstrExtension,
								bool							bSearchSubdirectories = false);

	void loadModControlArray(std::vector<CvString>&	aszFiles, const char* szFileRoot);
};

#endif	// XML_LOAD_UTILITY_SET_MOD_H