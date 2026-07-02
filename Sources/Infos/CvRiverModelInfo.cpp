//------------------------------------------------------------------------------------------------
//  FILE:    CvRiverModelInfo.cpp
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
#include "CvRiverModelInfo.h"


//======================================================================================================
//					CvRiverModelInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvRiverModelInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvRiverModelInfo::CvRiverModelInfo() :
m_iTextureIndex(0)
{
	m_szDeltaString[0] = '\0';
	m_szConnectString[0] = '\0';
	m_szRotateString[0] = '\0';
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvRiverModelInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvRiverModelInfo::~CvRiverModelInfo()
{
}


const char* CvRiverModelInfo::getModelFile() const
{
	return m_szModelFile;
}


const char* CvRiverModelInfo::getBorderFile() const
{
	return m_szBorderFile;
}


int CvRiverModelInfo::getTextureIndex() const
{
	return m_iTextureIndex;
}


const char* CvRiverModelInfo::getDeltaString() const
{
	return m_szDeltaString;
}


const char* CvRiverModelInfo::getConnectString() const
{
	return m_szConnectString;
}


const char* CvRiverModelInfo::getRotateString() const
{
	return m_szRotateString;
}


bool CvRiverModelInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szModelFile, L"ModelFile");
	pXML->GetOptionalChildXmlValByName(m_szBorderFile, L"BorderFile");
	pXML->GetOptionalChildXmlValByName(&m_iTextureIndex, L"TextureIndex");
	pXML->GetOptionalChildXmlValByName(m_szDeltaString, L"DeltaType");
	pXML->GetOptionalChildXmlValByName(m_szConnectString, L"Connections");
	pXML->GetOptionalChildXmlValByName(m_szRotateString, L"Rotations");

	return true;
}

void CvRiverModelInfo::copyNonDefaults(const CvRiverModelInfo* pClassInfo)
{
	const int iDefault = 0;
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getModelFile() == cDefault) m_szModelFile = pClassInfo->getModelFile();
	if (getBorderFile() == cDefault) m_szBorderFile = pClassInfo->getBorderFile();

	if (getTextureIndex() == iDefault) m_iTextureIndex = pClassInfo->getTextureIndex();

	if (getDeltaString() == cDefault) strcpy(m_szDeltaString, (const char*) CvString::format("%s", pClassInfo->getDeltaString()));
	if (getConnectString() == cDefault) strcpy(m_szConnectString, (const char*) CvString::format("%s", pClassInfo->getConnectString()));
	if (getRotateString() == cDefault) strcpy(m_szRotateString, (const char*) CvString::format("%s", pClassInfo->getRotateString()));
}

