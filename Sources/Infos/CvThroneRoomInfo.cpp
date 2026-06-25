//------------------------------------------------------------------------------------------------
//  FILE:    CvThroneRoomInfo.cpp
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
#include "CvThroneRoomInfo.h"


//======================================================================================================
//					CvThroneRoomInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvThroneRoomInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvThroneRoomInfo::CvThroneRoomInfo() :
m_iFromState(0),
m_iToState(0),
m_iAnimation(0)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvThroneRoomInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvThroneRoomInfo::~CvThroneRoomInfo()
{
}


const char* CvThroneRoomInfo::getEvent()
{
	return m_szEvent;
}


const char* CvThroneRoomInfo::getNodeName()
{
	return m_szNodeName;
}


int CvThroneRoomInfo::getFromState()
{
	return m_iFromState;
}


int CvThroneRoomInfo::getToState()
{
	return m_iToState;
}


int CvThroneRoomInfo::getAnimation()
{
	return m_iAnimation;
}


bool CvThroneRoomInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szEvent, L"Event");
	pXML->GetOptionalChildXmlValByName(&m_iFromState, L"iFromState" );
	pXML->GetOptionalChildXmlValByName(&m_iToState, L"iToState" );
	pXML->GetOptionalChildXmlValByName(m_szNodeName, L"NodeName");
	pXML->GetOptionalChildXmlValByName(&m_iAnimation, L"iAnimation" );

	return true;
}


void CvThroneRoomInfo::copyNonDefaults(CvThroneRoomInfo* pClassInfo)
{
	const int iDefault = 0;
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getEvent() == cDefault) m_szEvent = pClassInfo->getEvent();
	if (getFromState() == iDefault) m_iFromState = pClassInfo->getFromState();
	if (getToState() == iDefault) m_iToState = pClassInfo->getToState();
	if (getNodeName() == cDefault) m_szNodeName = pClassInfo->getNodeName();
	if (getAnimation() == iDefault) m_iAnimation = pClassInfo->getAnimation();
}

