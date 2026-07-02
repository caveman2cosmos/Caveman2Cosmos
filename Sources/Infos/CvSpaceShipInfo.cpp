//------------------------------------------------------------------------------------------------
//  FILE:    CvSpaceShipInfo.cpp
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
#include "CvSpaceShipInfo.h"


//======================================================================================================
//					CvSpaceShipInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSpaceShipInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSpaceShipInfo::CvSpaceShipInfo() :
m_eSpaceShipInfoType(SPACE_SHIP_INFO_TYPE_NONE),
m_iPartNumber(-1),
m_iArtType(-1),
m_iEventCode(-1),
m_eProjectType(NO_PROJECT),
m_eCameraUpAxis(AXIS_X)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSpaceShipInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSpaceShipInfo::~CvSpaceShipInfo()
{
}


const char* CvSpaceShipInfo::getNodeName()
{
	return m_szNodeName;
}


const char* CvSpaceShipInfo::getProjectName()
{
	return m_szProjectName;
}


void CvSpaceShipInfo::setProjectName(const char* szVal)
{
	m_szProjectName = szVal;
	m_eProjectType = (ProjectTypes) GC.getInfoTypeForString(m_szProjectName, true);
}


ProjectTypes CvSpaceShipInfo::getProjectType()
{
	return m_eProjectType;
}


AxisTypes CvSpaceShipInfo::getCameraUpAxis()
{
	return m_eCameraUpAxis;
}


SpaceShipInfoTypes CvSpaceShipInfo::getSpaceShipInfoType()
{
	return m_eSpaceShipInfoType;
}


int CvSpaceShipInfo::getPartNumber()
{
	return m_iPartNumber;
}


int CvSpaceShipInfo::getArtType()
{
	return m_iArtType;
}


int CvSpaceShipInfo::getEventCode()
{
	return m_iEventCode;
}


bool CvSpaceShipInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	CvString szTextVal;
	pXML->GetChildXmlValByName(m_szNodeName, L"NodeName");
	pXML->GetChildXmlValByName(szTextVal, L"ProjectName");
	setProjectName(szTextVal);

	//up axis
	pXML->GetChildXmlValByName(szTextVal, L"CameraUpAxis");
	if(szTextVal.CompareNoCase("AXIS_X") == 0)
		m_eCameraUpAxis = AXIS_X;
	else if(szTextVal.CompareNoCase("AXIS_Y") == 0)
		m_eCameraUpAxis = AXIS_Y;
	else if(szTextVal.CompareNoCase("AXIS_Z") == 0)
		m_eCameraUpAxis = AXIS_Z;
	else
	{
		FErrorMsg("[Jason] Unknown Axis Type.");
	}

	pXML->GetOptionalChildXmlValByName(&m_iPartNumber, L"PartNumber");
	pXML->GetOptionalChildXmlValByName(&m_iArtType, L"ArtType");
	pXML->GetOptionalChildXmlValByName(&m_iEventCode, L"EventCode");

	//type
	pXML->GetChildXmlValByName(szTextVal, L"InfoType");
	if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_FILENAME") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_FILENAME;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_ALPHA_CENTAURI") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_ALPHA_CENTAURI;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_LAUNCH") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_LAUNCH;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_LAUNCHED") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_LAUNCHED;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_ZOOM_IN") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_ZOOM_IN;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_ZOOM_MOVE") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_ZOOM_MOVE;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_COMPONENT_OFF") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_COMPONENT_OFF;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_COMPONENT_APPEAR") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_COMPONENT_APPEAR;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_COMPONENT_PREVIEW") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_COMPONENT_PREVIEW;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_COMPONENT_ON") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_COMPONENT_ON;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_LIGHT_OFF") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_LIGHT_OFF;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_GANTRY_SMOKE_ON") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_GANTRY_SMOKE_ON;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_IN_SPACE_SMOKE_ON") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_IN_SPACE_SMOKE_ON;
	else if(szTextVal.CompareNoCase("SPACE_SHIP_INFO_TYPE_IN_GAME_SMOKE_ON") == 0)
		m_eSpaceShipInfoType = SPACE_SHIP_INFO_TYPE_IN_GAME_SMOKE_ON;
	else
	{
		FErrorMsg("[Jason] Unknown SpaceShipInfoType.");
	}

	return true;
}


void CvSpaceShipInfo::copyNonDefaults(CvSpaceShipInfo* pClassInfo)
{
	const int iDefault = 0;
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getNodeName() == cDefault) m_szNodeName = pClassInfo->m_szNodeName;
	if (getProjectName() == cDefault) setProjectName(pClassInfo->getProjectName());

	if (getPartNumber() == iDefault) m_iPartNumber = pClassInfo->m_iPartNumber;
	if (getArtType() == iDefault) m_iArtType = pClassInfo->m_iArtType;
	if (getEventCode() == iDefault) m_iEventCode = pClassInfo->m_iEventCode;

//	if (getCameraUpAxis() == cDefault) m_eCameraUpAxis = pClassInfo->getCameraUpAxis();
//	if (getSpaceShipInfoType() == cDefault) m_eSpaceShipInfoType = pClassInfo->getSpaceShipInfoType();
}

