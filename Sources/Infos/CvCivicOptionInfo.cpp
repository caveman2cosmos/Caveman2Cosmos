//------------------------------------------------------------------------------------------------
//  FILE:    CvCivicOptionInfo.cpp
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
#include "CvCivicOptionInfo.h"


//======================================================================================================
//					CvCivicOptionInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCivicOptionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCivicOptionInfo::CvCivicOptionInfo() :
	m_bPolicy(false),
	m_pabTraitNoUpkeep(NULL)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCivicOptionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCivicOptionInfo::~CvCivicOptionInfo()
{
	//SAFE_DELETE_ARRAY(m_pabTraitNoUpkeep);
}


bool CvCivicOptionInfo::isPolicy() const
{
	return m_bPolicy;
}

//bool CvCivicOptionInfo::getTraitNoUpkeep(int i) const
//{
//	FASSERT_BOUNDS(0, GC.getNumTraitInfos(), i);
//	return m_pabTraitNoUpkeep ? m_pabTraitNoUpkeep[i] : false;
//}

bool CvCivicOptionInfo::read(CvXMLLoadUtility* pXML)
{

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_bPolicy, L"bPolicy");
	//pXML->SetVariableListTagPair(&m_pabTraitNoUpkeep, L"TraitNoUpkeeps", GC.getNumTraitInfos());

	return true;
}


void CvCivicOptionInfo::copyNonDefaults(const CvCivicOptionInfo* pClassInfo)
{
	CvInfoBase::copyNonDefaults(pClassInfo);

	if (!isPolicy()) m_bPolicy = pClassInfo->isPolicy();
}

