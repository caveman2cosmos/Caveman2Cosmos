//------------------------------------------------------------------------------------------------
//  FILE:    CvAssetInfoBase.cpp
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
#include "CvAssetInfoBase.h"


/////////////////////////////////////////////////////////////////////////////////////////////
// CvAssetInfoBase
/////////////////////////////////////////////////////////////////////////////////////////////

const char* CvAssetInfoBase::getTag() const
{
	return getType();
}


void CvAssetInfoBase::setTag(const char* szDesc)
{
	m_szType = szDesc;
}


const char* CvAssetInfoBase::getPath() const
{
	return m_szPath;
}


void CvAssetInfoBase::setPath(const char* szDesc)
{
	m_szPath = szDesc;
}


bool CvAssetInfoBase::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))		// 'tag' is the same as 'type'
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Path");
	setPath(szTextVal);

	return true;
}

