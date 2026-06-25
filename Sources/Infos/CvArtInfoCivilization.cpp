//------------------------------------------------------------------------------------------------
//  FILE:    CvArtInfoCivilization.cpp
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
#include "CvArtInfoCivilization.h"


/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoCivilization
/////////////////////////////////////////////////////////////////////////////////////////////

CvArtInfoCivilization::CvArtInfoCivilization() :
m_bWhiteFlag(false)
{
}


CvArtInfoCivilization::~CvArtInfoCivilization()
{
}


bool CvArtInfoCivilization::isWhiteFlag() const
{
	return m_bWhiteFlag;
}


bool CvArtInfoCivilization::read(CvXMLLoadUtility* pXML)
{
	if (!CvArtInfoAsset::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_bWhiteFlag, L"bWhiteFlag");
	return true;
}


void CvArtInfoCivilization::copyNonDefaults(const CvArtInfoCivilization* pClassInfo)
{
	// Empty, for Art files we stick to FULL XML defintions
}

