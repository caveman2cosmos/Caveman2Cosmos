//------------------------------------------------------------------------------------------------
//  FILE:    CvPlayerColorInfo.cpp
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
#include "CvPlayerColorInfo.h"


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvPlayerColorInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvPlayerColorInfo::CvPlayerColorInfo() :
m_iColorTypePrimary(NO_COLOR),
m_iColorTypeSecondary(NO_COLOR),
m_iTextColorType(NO_COLOR)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvPlayerColorInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvPlayerColorInfo::~CvPlayerColorInfo()
{
}


int CvPlayerColorInfo::getColorTypePrimary() const
{
	return m_iColorTypePrimary;
}


int CvPlayerColorInfo::getColorTypeSecondary() const
{
	return m_iColorTypeSecondary;
}


int CvPlayerColorInfo::getTextColorType() const
{
	return m_iTextColorType;
}


bool CvPlayerColorInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"ColorTypePrimary");
	m_iColorTypePrimary = pXML->GetInfoClass( szTextVal);

	pXML->GetChildXmlValByName(szTextVal, L"ColorTypeSecondary");
	m_iColorTypeSecondary = pXML->GetInfoClass( szTextVal);

	pXML->GetChildXmlValByName(szTextVal, L"TextColorType");
	m_iTextColorType = pXML->GetInfoClass( szTextVal);

	return true;
}


void CvPlayerColorInfo::copyNonDefaults(const CvPlayerColorInfo* pClassInfo)
{
	const int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getColorTypePrimary() == iTextDefault) m_iColorTypePrimary = pClassInfo->getColorTypePrimary();
	if (getColorTypeSecondary() == iTextDefault) m_iColorTypeSecondary = pClassInfo->getColorTypeSecondary();
	if (getTextColorType() == iTextDefault) m_iTextColorType = pClassInfo->getTextColorType();
}

