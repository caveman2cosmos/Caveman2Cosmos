#pragma once
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
namespace CvXMLLoadUtilityModTools
{
	void StringArrayExtend( CvString **ppszListNew, int* iNumNewListElements,
							CvString **ppszListOld, int iNumOldListElements,
							CvString szDefaultListVal = CvString());

	bool isModularArt(const char* szLocationName);
}

#endif	// XML_LOAD_UTILITY_H
