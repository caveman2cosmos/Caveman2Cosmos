//------------------------------------------------------------------------------------------------
//  FILE:    CvWorldPickerInfo.cpp
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvArtFileMgr.h"
#include "CvBuildingInfo.h"
#include "CvHeritageInfo.h"
#include "CvGameAI.h"
#include "CvGameTextMgr.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvInfoUtil.h"
#include "CvPlayerAI.h"
#include "CvPython.h"
#include "CvXMLLoadUtility.h"
#include "CvXMLLoadUtilityModTools.h"
#include "CheckSum.h"
#include "CvImprovementInfo.h"
#include "CvBonusInfo.h"
#include "CvWorldPickerInfo.h"


//======================================================================================================
//					CvWorldPickerInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvWorldPickerInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvWorldPickerInfo::CvWorldPickerInfo()
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvWorldPickerInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvWorldPickerInfo::~CvWorldPickerInfo()
{
}


const char* CvWorldPickerInfo::getMapName()
{
	return m_szMapName;
}


const char* CvWorldPickerInfo::getModelFile()
{
	return m_szModelFile;
}


int CvWorldPickerInfo::getNumSizes()
{
	return m_aSizes.size();
}


float CvWorldPickerInfo::getSize(int index)
{
	return m_aSizes[index];
}


int CvWorldPickerInfo::getNumClimates()
{
	return m_aClimates.size();
}


const char* CvWorldPickerInfo::getClimatePath(int index)
{
	return m_aClimates[index];
}


int CvWorldPickerInfo::getNumWaterLevelDecals()
{
	return m_aWaterLevelDecals.size();
}


const char* CvWorldPickerInfo::getWaterLevelDecalPath(int index)
{
	return m_aWaterLevelDecals[index];
}


int CvWorldPickerInfo::getNumWaterLevelGloss()
{
	return m_aWaterLevelGloss.size();
}


const char* CvWorldPickerInfo::getWaterLevelGlossPath(int index)
{
	return m_aWaterLevelGloss[index];
}


bool CvWorldPickerInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	float fVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(m_szMapName, L"MapName");
	pXML->GetChildXmlValByName(m_szModelFile, L"ModelFile");

	//sizes
	if(pXML->TryMoveToXmlFirstChild(L"Sizes"))
	{
		if(pXML->TryMoveToXmlFirstChild(L"Size"))
		{
			do
			{
				pXML->GetXmlVal(&fVal);
				m_aSizes.push_back(fVal);
			} while(pXML->TryMoveToXmlNextSibling(L"Size"));

			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	//climates
	if(pXML->TryMoveToXmlFirstChild(L"Climates"))
	{
		if(pXML->TryMoveToXmlFirstChild(L"ClimatePath"))
		{
			do
			{
				pXML->GetXmlVal(szTextVal);
				m_aClimates.push_back(szTextVal);
			} while(pXML->TryMoveToXmlNextSibling(L"ClimatePath"));

			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	//water level decals
	if(pXML->TryMoveToXmlFirstChild(L"WaterLevelDecals"))
	{
		if(pXML->TryMoveToXmlFirstChild(L"WaterLevelDecalPath"))
		{
			do
			{
				pXML->GetXmlVal(szTextVal);
				m_aWaterLevelDecals.push_back(szTextVal);
			} while(pXML->TryMoveToXmlNextSibling(L"WaterLevelDecalPath"));

			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	//water level gloss
	if(pXML->TryMoveToXmlFirstChild(L"WaterLevelGloss"))
	{
		if(pXML->TryMoveToXmlFirstChild(L"WaterLevelGlossPath"))
		{
			do
			{
				pXML->GetXmlVal(szTextVal);
				m_aWaterLevelGloss.push_back(szTextVal);
			} while(pXML->TryMoveToXmlNextSibling(L"WaterLevelGlossPath"));

			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	return true;
}

void CvWorldPickerInfo::copyNonDefaults(CvWorldPickerInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getMapName() == cDefault) m_szMapName = pClassInfo->m_szMapName;
	if (getModelFile() == cDefault) m_szModelFile = pClassInfo->getModelFile();

	if ( getNumSizes() == 0 )
	{
		for ( int i = 0; i < pClassInfo->getNumSizes(); i++ )
		{
			m_aSizes.push_back(pClassInfo->getSize(i));
		}
	}

	if ( getNumClimates() == 0 )
	{
		for ( int i = 0; i < pClassInfo->getNumClimates(); i++ )
		{
			m_aClimates.push_back(pClassInfo->getClimatePath(i));
		}
	}

	if ( getNumWaterLevelDecals() == 0 )
	{
		for ( int i = 0; i < pClassInfo->getNumWaterLevelDecals(); i++ )
		{
			m_aWaterLevelDecals.push_back(pClassInfo->getWaterLevelDecalPath(i));
		}
	}

	if ( getNumWaterLevelGloss() == 0 )
	{
		for ( int i = 0; i < pClassInfo->getNumWaterLevelGloss(); i++ )
		{
			m_aWaterLevelDecals.push_back(pClassInfo->getWaterLevelGlossPath(i));
		}
	}
}

