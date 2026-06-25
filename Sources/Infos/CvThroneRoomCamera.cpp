//------------------------------------------------------------------------------------------------
//  FILE:    CvThroneRoomCamera.cpp
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
#include "CvThroneRoomCamera.h"


//======================================================================================================
//					CvThroneRoomCamera
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvThroneRoomCamera()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvThroneRoomCamera::CvThroneRoomCamera()
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvThroneRoomCamera()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvThroneRoomCamera::~CvThroneRoomCamera()
{
}


const char* CvThroneRoomCamera::getFileName()
{
	return m_szFileName;
}


bool CvThroneRoomCamera::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(m_szFileName, L"FileName");

	return true;
}


void CvThroneRoomCamera::copyNonDefaults(const CvThroneRoomCamera* pClassInfo)
{
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getFileName() == cDefault) m_szFileName = pClassInfo->m_szFileName;
}

