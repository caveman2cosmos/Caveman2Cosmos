//------------------------------------------------------------------------------------------------
//  FILE:    CvRouteModelInfo.cpp
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
#include "CvRouteModelInfo.h"


//======================================================================================================
//					CvRouteModelInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvRouteModelInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvRouteModelInfo::CvRouteModelInfo() :
m_eRouteType(NO_ROUTE),
m_bAnimated(false)
{
	m_szConnectString[0] = '\0';
	m_szModelConnectString[0] = '\0';
	m_szRotateString[0] = '\0';
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvRouteModelInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvRouteModelInfo::~CvRouteModelInfo()
{
}


RouteTypes CvRouteModelInfo::getRouteType() const		// The route type
{
	return m_eRouteType;
}


const char* CvRouteModelInfo::getModelFile() const
{
	return m_szModelFile;
}


const char* CvRouteModelInfo::getLateModelFile() const
{
	return m_szLateModelFile;
}


const char* CvRouteModelInfo::getModelFileKey() const
{
	return m_szModelFileKey;
}


bool CvRouteModelInfo::isAnimated() const
{
	return m_bAnimated;
}


const char* CvRouteModelInfo::getConnectString() const
{
	return m_szConnectString;
}


const char* CvRouteModelInfo::getModelConnectString() const
{
	return m_szModelConnectString;
}


const char* CvRouteModelInfo::getRotateString() const
{
	return m_szRotateString;
}


bool CvRouteModelInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(m_szModelFile, L"ModelFile");
	pXML->GetChildXmlValByName(m_szLateModelFile, L"LateModelFile");
	pXML->GetChildXmlValByName(m_szModelFileKey, L"ModelFileKey");
	pXML->GetChildXmlValByName(&m_bAnimated, L"Animated");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"RouteType");
	m_eRouteType = (RouteTypes)(pXML->GetInfoClass(szTextVal));
	pXML->GetOptionalChildXmlValByName(m_szConnectString, L"Connections");
	pXML->GetOptionalChildXmlValByName(m_szModelConnectString, L"ModelConnections");
	pXML->GetOptionalChildXmlValByName(m_szRotateString, L"Rotations");

	return true;
}


void CvRouteModelInfo::copyNonDefaults(const CvRouteModelInfo* pClassInfo)
{
	const bool bDefault = false;
	const int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getModelFile() == cDefault) m_szModelFile = pClassInfo->getModelFile();
	if (getLateModelFile() == cDefault) m_szLateModelFile = pClassInfo->getLateModelFile();
	if (getModelFileKey() == cDefault) m_szModelFileKey = pClassInfo->getModelFileKey();

	if (isAnimated() == bDefault) m_bAnimated = pClassInfo->isAnimated();

	if (getRouteType() == iTextDefault) m_eRouteType = pClassInfo->getRouteType();

	if (getConnectString() == cDefault) strcpy(m_szConnectString, (const char*) CvString::format("%s", pClassInfo->getConnectString()));
	if (getModelConnectString() == cDefault) strcpy(m_szModelConnectString, (const char*) CvString::format("%s", pClassInfo->getModelConnectString()));
	if (getRotateString() == cDefault) strcpy(m_szRotateString, (const char*) CvString::format("%s", pClassInfo->getRotateString()));
}

