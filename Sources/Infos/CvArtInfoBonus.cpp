//------------------------------------------------------------------------------------------------
//  FILE:    CvArtInfoBonus.cpp
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
#include "CvArtInfoBonus.h"


/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoBonus
/////////////////////////////////////////////////////////////////////////////////////////////

bool CvArtInfoBonus::read(CvXMLLoadUtility* pXML)
{
	if (!CvArtInfoScalableAsset::read(pXML))
	{
		return false;
	}
	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"SHADERNIF");
	setShaderNIF(szTextVal);

	pXML->GetChildXmlValByName(&m_iFontButtonIndex, L"FontButtonIndex");

	return true;
}


void CvArtInfoBonus::copyNonDefaults(const CvArtInfoBonus* pClassInfo)
{
	// Empty, for Art files we stick to FULL XML defintions
}


CvArtInfoBonus::CvArtInfoBonus()
{
	m_iFontButtonIndex = 0;
}


int CvArtInfoBonus::getFontButtonIndex() const
{
	return m_iFontButtonIndex;
}


const char* CvArtInfoBonus::getShaderNIF() const
{
	return m_szShaderNIF;
}

void CvArtInfoBonus::setShaderNIF(const char* szDesc)
{
	m_szShaderNIF = szDesc;
}

