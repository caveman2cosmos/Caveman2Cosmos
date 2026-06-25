//------------------------------------------------------------------------------------------------
//  FILE:    CvIdeaInfo.cpp
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
#include "CvIdeaInfo.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvIdeaInfo
//
//  DESC:   Contains info about Ideas
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CvIdeaInfo::CvIdeaInfo() :
m_eIdeaClass(NO_IDEACLASS)
{
}


CvIdeaInfo::~CvIdeaInfo()
{
}


IdeaClassTypes CvIdeaInfo::getIdeaClass() const
{
	return m_eIdeaClass;
}


bool CvIdeaInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	CvString szTextVal;

	if ( pXML->GetOptionalChildXmlValByName(szTextVal, L"IdeaClassType") )
		m_eIdeaClass = (IdeaClassTypes)pXML->GetInfoClass(szTextVal);
	else
		m_eIdeaClass = NO_IDEACLASS;

	return true;
}


void CvIdeaInfo::copyNonDefaults(const CvIdeaInfo* pClassInfo)
{
	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getIdeaClass() == NO_IDEACLASS) m_eIdeaClass = pClassInfo->getIdeaClass();
}


void CvIdeaInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_eIdeaClass);
}

