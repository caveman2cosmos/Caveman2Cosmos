//------------------------------------------------------------------------------------------------
//  FILE:    CvArtInfoAsset.cpp
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
#include "CvArtInfoAsset.h"


/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoAsset
/////////////////////////////////////////////////////////////////////////////////////////////

const char* CvArtInfoAsset::getNIF() const
{
	return m_szNIF;
}


const char* CvArtInfoAsset::getKFM() const
{
	return m_szKFM;
}


void CvArtInfoAsset::setNIF(const char* szDesc)
{
	m_szNIF = szDesc;
}


void CvArtInfoAsset::setKFM(const char* szDesc)
{
	m_szKFM = szDesc;
}


bool CvArtInfoAsset::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvAssetInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"NIF"); // TO DO : maybe we should do something different if there is no NIF tag?
	setNIF(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"KFM");// TO DO : maybe we should do something different if there is no KFM tag?
	setKFM(szTextVal);


	return true;
}

void CvArtInfoAsset::copyNonDefaults(const CvArtInfoAsset* pClassInfo)
{
	// Empty, for Art files we stick to FULL XML defintions
}

