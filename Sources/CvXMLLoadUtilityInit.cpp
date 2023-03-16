//
// init/cleanup XML functions
//

#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvXMLLoadUtility.h"

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   InitImprovementBonusList(CvImprovementBonusInfo** ppImprovementBonus, int iListLen)
//
//  PURPOSE :   allocate a improvement bonus struct list of iListLen size and initialize it's members
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::InitImprovementBonusList(CvImprovementBonusInfo** ppImprovementBonus, int iListLen)
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(*ppImprovementBonus == NULL,"memory leak?");
	FAssertMsg((0 < iListLen),"list size to allocate is less than 1");
	// allocate memory for the bonus type pointer based on the list length parameter
	*ppImprovementBonus = new CvImprovementBonusInfo[iListLen];
	// set the local pointer to the memory we just allocated
	CvImprovementBonusInfo* paImprovementBonus = *ppImprovementBonus;

	// loop through all the bonus structs
	for (int i = 0; i < iListLen; i++)
	{
		paImprovementBonus[i].m_bBonusMakesValid = false;
		paImprovementBonus[i].m_bBonusTrade = false;

		FAssertMsg(paImprovementBonus[i].m_piYieldChange==NULL, "mem leak");
		paImprovementBonus[i].m_piYieldChange = new int[NUM_YIELD_TYPES];
		for (int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			paImprovementBonus[i].m_piYieldChange[j] = 0;
		}

		paImprovementBonus[i].m_iDiscoverRand = 0;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CleanUpGlobals()
//
//  PURPOSE :   free the variables that are in globals.cpp/h
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::CleanUpGlobalVariables()
{
	GC.deleteInfoArrays();
}
