//------------------------------------------------------------------------------------------------
//  FILE:    CvArtInfoLeaderhead.cpp
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
#include "CvArtInfoLeaderhead.h"


/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoLeaderhead
/////////////////////////////////////////////////////////////////////////////////////////////

const char* CvArtInfoLeaderhead::getNoShaderNIF() const
{
	return m_szNoShaderNIF;
}


void CvArtInfoLeaderhead::setNoShaderNIF(const char* szNIF)
{
	m_szNoShaderNIF = szNIF;
}


const char* CvArtInfoLeaderhead::getBackgroundKFM() const
{
	return m_szBackgroundKFM;
}


void CvArtInfoLeaderhead::setBackgroundKFM(const char* szKFM)
{
	m_szBackgroundKFM = szKFM;
}


bool CvArtInfoLeaderhead::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvArtInfoAsset::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"NoShaderNIF");
	setNoShaderNIF(szTextVal);

	if (pXML->GetOptionalChildXmlValByName(szTextVal, L"BackgroundKFM"))
	{
		setBackgroundKFM(szTextVal);
	}
	else
		setBackgroundKFM("");

	return true;
}


void CvArtInfoLeaderhead::copyNonDefaults(const CvArtInfoLeaderhead* pClassInfo)
{
	// Empty, for Art files we stick to FULL XML defintions
}

