//------------------------------------------------------------------------------------------------
//  FILE:    CvColorInfo.cpp
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
#include "CvColorInfo.h"


//======================================================================================================
//					CvColorInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvColorInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvColorInfo::CvColorInfo()
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvColorInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvColorInfo::~CvColorInfo()
{
}


const NiColorA& CvColorInfo::getColor() const
{
	return m_Color;
}


bool CvColorInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	float afColorVals[4];	// array to hold the 4 color values, red, green, blue, and alpha

	pXML->GetChildXmlValByName(&afColorVals[0], L"fRed");
	pXML->GetChildXmlValByName(&afColorVals[1], L"fGreen");
	pXML->GetChildXmlValByName(&afColorVals[2], L"fBlue");
	pXML->GetChildXmlValByName(&afColorVals[3], L"fAlpha");

	m_Color = NiColorA(afColorVals[0], afColorVals[1], afColorVals[2], afColorVals[3]);

	return true;
}


void CvColorInfo::copyNonDefaults(const CvColorInfo* pClassInfo)
{
	//const float fDefault = 0.0f;

	CvInfoBase::copyNonDefaults(pClassInfo);

//	if (afColorVals[0] == fDefault && afColorVals[1] == fDefault && afColorVals[2] == fDefault
//		 && afColorVals[3] == fDefault) m_Color = pClassInfo->getColor();
}

