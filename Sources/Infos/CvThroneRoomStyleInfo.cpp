//------------------------------------------------------------------------------------------------
//  FILE:    CvThroneRoomStyleInfo.cpp
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
#include "CvThroneRoomStyleInfo.h"


//======================================================================================================
//					CvThroneRoomStyleInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvThroneRoomStyleInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvThroneRoomStyleInfo::CvThroneRoomStyleInfo()
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvThroneRoomStyleInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvThroneRoomStyleInfo::~CvThroneRoomStyleInfo()
{
}


bool CvThroneRoomStyleInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetOptionalChildXmlValByName(m_szArtStyleType, L"ArtStyleType");
	pXML->GetOptionalChildXmlValByName(m_szEraType, L"EraType");
	pXML->GetOptionalChildXmlValByName(m_szFileName, L"FileName");

	//node names
	if(pXML->TryMoveToXmlFirstChild())
	{
		while(pXML->TryMoveToXmlNextSibling(L"NodeName"))
		{
			pXML->GetXmlVal(szTextVal);
			m_aNodeNames.push_back(szTextVal);
		}
		pXML->MoveToXmlParent();
	}

	//texture names
	if(pXML->TryMoveToXmlFirstChild())
	{
		while(pXML->TryMoveToXmlNextSibling(L"TextureName"))
		{
			pXML->GetXmlVal(szTextVal);
			m_aTextureNames.push_back(szTextVal);
		}
		pXML->MoveToXmlParent();
	}

	return true;
}


void CvThroneRoomStyleInfo::copyNonDefaults(const CvThroneRoomStyleInfo* pClassInfo)
{
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getArtStyleType() == cDefault) m_szArtStyleType = pClassInfo->m_szArtStyleType;
	if (getEraType() == cDefault) m_szEraType = pClassInfo->m_szEraType;
	if (getFileName() == cDefault) m_szFileName = pClassInfo->m_szFileName;

	/*
	m_aNodeNames and m_aTextureNames don't seem to be used?
	since i hardly doubt anyone ever touches those XML's anyway, i just leave them out
	FOR NOW! */
}

