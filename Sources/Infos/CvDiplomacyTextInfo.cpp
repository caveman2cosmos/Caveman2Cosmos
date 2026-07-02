//------------------------------------------------------------------------------------------------
//  FILE:    CvDiplomacyTextInfo.cpp
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
#include "CvDiplomacyTextInfo.h"


//////////////////////////////////////////////////////////////////////////
//
//	CvDiplomacyTextInfo
//
//

CvDiplomacyTextInfo::CvDiplomacyTextInfo() :
m_iNumResponses(0),
m_pResponses(NULL)
{
}


// note - Response member vars allocated by CvXmlLoadUtility
void CvDiplomacyTextInfo::init(int iNum)
{
	uninit();
	m_pResponses = new Response[iNum];
	m_iNumResponses=iNum;
}


void CvDiplomacyTextInfo::uninit()
{
	SAFE_DELETE_ARRAY(m_pResponses);
}


int CvDiplomacyTextInfo::getNumResponses() const
{
	return m_iNumResponses;
}


bool CvDiplomacyTextInfo::getCivilizationTypes(int i, int j) const
{
	FASSERT_BOUNDS(0, getNumResponses(), i);
	FASSERT_BOUNDS(0, GC.getNumCivilizationInfos(), j);
	return m_pResponses[i].m_pbCivilizationTypes ? m_pResponses[i].m_pbCivilizationTypes[j] : false;
}


bool CvDiplomacyTextInfo::getLeaderHeadTypes(int i, int j) const
{
	FASSERT_BOUNDS(0, getNumResponses(), i);
	FASSERT_BOUNDS(0, GC.getNumLeaderHeadInfos(), j);
	return m_pResponses[i].m_pbLeaderHeadTypes ? m_pResponses[i].m_pbLeaderHeadTypes[j] : false;
}


bool CvDiplomacyTextInfo::getAttitudeTypes(int i, int j) const
{
	FASSERT_BOUNDS(0, getNumResponses(), i);
	FASSERT_BOUNDS(0, NUM_ATTITUDE_TYPES, j);
	return m_pResponses[i].m_pbAttitudeTypes ? m_pResponses[i].m_pbAttitudeTypes[j] : false;
}


bool CvDiplomacyTextInfo::getDiplomacyPowerTypes(int i, int j) const
{
	FASSERT_BOUNDS(0, getNumResponses(), i);
	FASSERT_BOUNDS(0, NUM_DIPLOMACYPOWER_TYPES, j);
	return m_pResponses[i].m_pbDiplomacyPowerTypes ? m_pResponses[i].m_pbDiplomacyPowerTypes[j] : false;
}


int CvDiplomacyTextInfo::getNumDiplomacyText(int i) const
{
	FASSERT_BOUNDS(0, getNumResponses(), i);
	return m_pResponses[i].m_iNumDiplomacyText;
}


const char* CvDiplomacyTextInfo::getDiplomacyText(int i, int j) const
{
	FASSERT_BOUNDS(0, getNumResponses(), i);
	FASSERT_BOUNDS(0, getNumDiplomacyText(i), j);
	return m_pResponses[i].m_paszDiplomacyText[j];
}


bool CvDiplomacyTextInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"Type");

	if ( pXML->TryMoveToXmlFirstChild(L"Responses") )
	{
		const int iIndexVal = pXML->GetXmlChildrenNumber(L"Response");
		init(iIndexVal);

		if (iIndexVal > 0 && pXML->TryMoveToXmlFirstChild())
		{
			for (int j = 0; j < iIndexVal; j++)
			{
				pXML->SetVariableListTagPair(&m_pResponses[j].m_pbCivilizationTypes, L"Civilizations", GC.getNumCivilizationInfos());
				pXML->SetVariableListTagPair(&m_pResponses[j].m_pbLeaderHeadTypes, L"Leaders", GC.getNumLeaderHeadInfos());
				pXML->SetVariableListTagPair(&m_pResponses[j].m_pbAttitudeTypes, L"Attitudes", NUM_ATTITUDE_TYPES);
				pXML->SetVariableListTagPair(&m_pResponses[j].m_pbDiplomacyPowerTypes, L"DiplomacyPowers", NUM_DIPLOMACYPOWER_TYPES);

				if (pXML->TryMoveToXmlFirstChild(L"DiplomacyText"))
				{
					pXML->SetStringList(&m_pResponses[j].m_paszDiplomacyText, &m_pResponses[j].m_iNumDiplomacyText);
					pXML->MoveToXmlParent();
				}

				if (!pXML->TryMoveToXmlNextSibling())
				{
					break;
				}
			}

			pXML->MoveToXmlParent();
		}
	}
	pXML->MoveToXmlParent();

	return true;
}

