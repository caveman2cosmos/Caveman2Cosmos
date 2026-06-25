//------------------------------------------------------------------------------------------------
//  FILE:    CvMissionInfo.cpp
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
#include "CvMissionInfo.h"


//======================================================================================================
//					CvMissionInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvMissionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvMissionInfo::CvMissionInfo() :
m_iTime(0),
m_bSound(false),
m_bTarget(false),
m_bBuild(false),
m_bVisible(false),
m_eEntityEvent(ENTITY_EVENT_NONE)
{
}


CvMissionInfo::CvMissionInfo(const char * szType) : CvHotkeyInfo(),
m_iTime(0),
m_bSound(false),
m_bTarget(false),
m_bBuild(false),
m_bVisible(false),
m_eEntityEvent(ENTITY_EVENT_NONE)
{
	m_szType = szType;
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvMissionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvMissionInfo::~CvMissionInfo()
{
}


int CvMissionInfo::getTime() const
{
	return m_iTime;
}


bool CvMissionInfo::isSound() const
{
	return m_bSound;
}


bool CvMissionInfo::isTarget() const
{
	return m_bTarget;
}


bool CvMissionInfo::isBuild() const
{
	return m_bBuild;
}


bool CvMissionInfo::getVisible() const
{
	return m_bVisible;
}


const char* CvMissionInfo::getWaypoint() const
{
	return m_szWaypoint;
}


EntityEventTypes CvMissionInfo::getEntityEvent() const
{
	return m_eEntityEvent;
}


bool CvMissionInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTmp;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szWaypoint, L"Waypoint");
	pXML->GetOptionalChildXmlValByName(&m_iTime, L"iTime");
	pXML->GetOptionalChildXmlValByName(&m_bSound, L"bSound");
	pXML->GetOptionalChildXmlValByName(&m_bTarget, L"bTarget");
	pXML->GetOptionalChildXmlValByName(&m_bBuild, L"bBuild");
	pXML->GetOptionalChildXmlValByName(&m_bVisible, L"bVisible");

	if ( pXML->GetOptionalChildXmlValByName(szTmp, L"EntityEventType"))
		m_eEntityEvent = (EntityEventTypes)pXML->GetInfoClass(szTmp);

	return true;
}


void CvMissionInfo::copyNonDefaults(const CvMissionInfo* pClassInfo)
{
	const bool bDefault = false;
	const int iDefault = 0;
	const CvString cDefault = CvString::format("").GetCString();

	CvHotkeyInfo::copyNonDefaults(pClassInfo);

	if (getWaypoint() == cDefault) m_szWaypoint = pClassInfo->getWaypoint();
	if (getTime() == iDefault) m_iTime = pClassInfo->getTime();

	if (isSound() == bDefault) m_bSound = pClassInfo->isSound();
	if (isTarget() == bDefault) m_bTarget = pClassInfo->isTarget();
	if (isBuild() == bDefault) m_bBuild = pClassInfo->isBuild();
	if (getVisible() == bDefault) m_bVisible = pClassInfo->getVisible();

	if ( getEntityEvent() == ENTITY_EVENT_NONE ) m_eEntityEvent = pClassInfo->getEntityEvent();
}

