//------------------------------------------------------------------------------------------------
//  FILE:    CvArtInfoMovie.cpp
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
#include "CvArtInfoMovie.h"


/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoMovie
/////////////////////////////////////////////////////////////////////////////////////////////

bool CvArtInfoMovie::read(CvXMLLoadUtility* pXML)
{
	return CvArtInfoAsset::read(pXML);
}


void CvArtInfoMovie::copyNonDefaults(const CvArtInfoMovie* pClassInfo)
{
	// Empty, for Art files we stick to FULL XML defintions
}

