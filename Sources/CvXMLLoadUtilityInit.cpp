//
// init/cleanup XML functions
//
#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvXMLLoadUtility.h"

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   InitStringList(CvString **ppszList, int iListLen, CvString szString = "")
//
//  PURPOSE :   allocate and initialize a CvString pointer list of iListLen size to "", or
//				to szString if it is supplied
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::InitStringList(CvString **ppszList, int iListLen, CvString szString)
{
	PROFILE_FUNC();
	int i;	// loop counter
	CvString* pszList;

	FAssertMsg(*ppszList == NULL,"memory leak?");
	FAssertMsg((0 < iListLen),"list size to allocate is less than 1");
	// allocate memory for the boolean pointer based on the list length parameter
	*ppszList = new CvString[iListLen];
	// set the local pointer to the memory we just allocated
	pszList = *ppszList;

	// loop through all the booleans
	for (i=0;i<iListLen;i++)
	{
		// set the current boolean to false
		pszList[i] = szString;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   void Init2DFloatList(float*** pppfList, int iSizeX, int iSizeY)
//
//  PURPOSE :   allocate and initialize a 2d array of float data
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::Init2DFloatList(float*** pppfList, int iSizeX, int iSizeY)
{
	// SPEEDUP
	PROFILE_FUNC();
	int i,j;	// loop counters
	float** ppfList;

	FAssertMsg(*pppfList == NULL,"memory leak?");
	FAssertMsg(((0 < iSizeX) && (0 < iSizeY)), "list size to allocate is less than 1");
	// allocate the memory for the array of pointers to arrays of floats
	*pppfList = new float *[iSizeX];
	// set the local pointer to the newly allocated memory
	ppfList = *pppfList;

	// loop through each of the pointers
	for (i=0;i<iSizeX;i++)
	{
		// allocate a list of floats for the current pointer
		ppfList[i] = new float[iSizeY];
		// loop through all of the current pointer's floats
		for (j=0;j<iSizeY;j++)
		{
			// set the current float to zero
			ppfList[i][j] = 0.0f;
		}
	}
}
/************************************************************************************************/
/* Afforess	                  Start		 08/26/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   void Init2DBoolList(bool*** pppiList, int iSizeX, int iSizeY)
//
//  PURPOSE :   allocate and initialize a 2d array of bool data
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::Init2DBoolList(bool*** pppbList, int iSizeX, int iSizeY)
{
	// SPEEDUP
	PROFILE_FUNC();
	int i,j;	// loop counters
	bool** ppbList;

	FAssertMsg(*pppbList == NULL,"memory leak?");
	FAssertMsg(((0 < iSizeX)), "list size to allocate is less than 1");
	// allocate the memory for the array of pointers to arrays of ints
	*pppbList = new bool *[iSizeX];
	// set the local pointer to the newly allocated memory
	ppbList = *pppbList;

	// loop through each of the pointers
	for (i=0;i<iSizeX;i++)
	{
		if ( iSizeY == -1 )
		{
			ppbList[i] = NULL;
		}
		else
		{
			// allocate a list of bools for the current pointer
			ppbList[i] = new bool[iSizeY];
			// loop through all of the current pointer's bools
			for (j=0;j<iSizeY;j++)
			{
				// set the current bool to false
				ppbList[i][j] = false;
			}
		}
	}
}

/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/



//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   void Init2DIntList(int*** pppiList, int iSizeX, int iSizeY)
//
//  PURPOSE :   allocate and initialize a 2d array of int data
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::Init2DIntList(int*** pppiList, int iSizeX, int iSizeY)
{
	// SPEEDUP
	PROFILE_FUNC();
	int i,j;	// loop counters
	int** ppiList;

	FAssertMsg(*pppiList == NULL,"memory leak?");
	FAssertMsg(((0 < iSizeX)), "list size to allocate is less than 1");
	// allocate the memory for the array of pointers to arrays of ints
	*pppiList = new int *[iSizeX];
	// set the local pointer to the newly allocated memory
	ppiList = *pppiList;

	// loop through each of the pointers
	for (i=0;i<iSizeX;i++)
	{
		if ( iSizeY == -1 )
		{
			ppiList[i] = NULL;
		}
		else
		{
			// allocate a list of ints for the current pointer
			ppiList[i] = new int[iSizeY];
			// loop through all of the current pointer's ints
			for (j=0;j<iSizeY;j++)
			{
				// set the current int to zero
				ppiList[i][j] = 0;
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   void InitPointerFloatList(float*** pppfList, int iSizeX)
//
//  PURPOSE :   allocate and initialize a 2d array of float data
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::InitPointerFloatList(float*** pppfList, int iSizeX)
{
	// SPEEDUP
	PROFILE_FUNC();
	int i;	// loop counters
	float** ppfList;

	FAssertMsg(*pppfList == NULL,"memory leak?");
	FAssertMsg((0 < iSizeX), "list size to allocate is less than 1");
	// allocate the memory for the array of pointers to arrays of floats
	*pppfList = new float *[iSizeX];
	// set the local pointer to the newly allocated memory
	ppfList = *pppfList;

	// loop through each of the pointers
	for (i=0;i<iSizeX;i++)
	{
		// Null each pointer
		ppfList[i] = NULL;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   void InitPointerIntList(int*** pppiList, int iSizeX)
//
//  PURPOSE :   allocate and initialize a 2d array of int data
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::InitPointerIntList(int*** pppiList, int iSizeX)
{
	// SPEEDUP
	PROFILE_FUNC();
	int i;	// loop counters
	int** ppiList;

	FAssertMsg(*pppiList == NULL,"memory leak?");
	FAssertMsg((0 < iSizeX), "list size to allocate is less than 1");
	// allocate the memory for the array of pointers to arrays of ints
	*pppiList = new int *[iSizeX];
	// set the local pointer to the newly allocated memory
	ppiList = *pppiList;

	// loop through each of the pointers
	for (i=0;i<iSizeX;i++)
	{
		// Null each pointer
		ppiList[i] = NULL;
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   void Init2DDirectionTypesList(DirectionTypes*** pppiList, int iSizeX, int iSizeY)
//
//  PURPOSE :   allocate and initialize a 2d array of DirectionTypes data
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::Init2DDirectionTypesList(DirectionTypes*** pppiList, int iSizeX, int iSizeY)
{
	// SPEEDUP
	PROFILE_FUNC();
	int i,j;	// loop counters
	DirectionTypes** ppiList;

	FAssertMsg(*pppiList == NULL,"memory leak?");
	FAssertMsg(((0 < iSizeX) && (0 < iSizeY)), "list size to allocate is less than 1");
	// allocate the memory for the array of pointers to arrays of DirectionTypes
	*pppiList = new DirectionTypes *[iSizeX];
	// set the local pointer to the newly allocated memory
	ppiList = *pppiList;

	// loop through each of the pointers
	for (i=0;i<iSizeX;i++)
	{
		// allocate a list of DirectionTypes for the current pointer
		ppiList[i] = new DirectionTypes[iSizeY];
		// loop through all of the current pointer's DirectionTypes
		for (j=0;j<iSizeY;j++)
		{
			// set the current DirectionTypes to NO_DIRECTION
			ppiList[i][j] = NO_DIRECTION;
		}
	}
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   InitImprovementBonusList(CvImprovementBonusInfo** ppImprovementBonus, int iListLen)
//
//  PURPOSE :   allocate a improvement bonus struct list of iListLen size and initialize it's members
//
//------------------------------------------------------------------------------------------------------
void CvXMLLoadUtility::InitImprovementBonusList(CvImprovementBonusInfo** ppImprovementBonus, int iListLen)
{
	// SPEEDUP
	PROFILE_FUNC();
	int i;	// loop counter
	CvImprovementBonusInfo* paImprovementBonus;

	FAssertMsg(*ppImprovementBonus == NULL,"memory leak?");
	FAssertMsg((0 < iListLen),"list size to allocate is less than 1");
	// allocate memory for the bonus type pointer based on the list length parameter
	*ppImprovementBonus = new CvImprovementBonusInfo[iListLen];
	// set the local pointer to the memory we just allocated
	paImprovementBonus = *ppImprovementBonus;

	// loop through all the bonus structs
	for (i=0;i<iListLen;i++)
	{
		paImprovementBonus[i].m_bBonusMakesValid = false;
		paImprovementBonus[i].m_bBonusTrade = false;

#if 1
		FAssertMsg(paImprovementBonus[i].m_piYieldChange==NULL, "mem leak");
		paImprovementBonus[i].m_piYieldChange = new int[NUM_YIELD_TYPES];
		for (int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			paImprovementBonus[i].m_piYieldChange[j] = 0;
		}
#endif

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
