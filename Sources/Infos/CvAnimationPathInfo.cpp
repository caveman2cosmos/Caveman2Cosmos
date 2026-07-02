//------------------------------------------------------------------------------------------------
//  FILE:    CvAnimationPathInfo.cpp
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
#include "CvAnimationPathInfo.h"


//======================================================================================================
//					CvAnimationPathInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvAnimationPathInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvAnimationPathInfo::CvAnimationPathInfo() :
	m_bMissionPath(false)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvAnimationPathInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvAnimationPathInfo::~CvAnimationPathInfo()
{
}


int CvAnimationPathInfo::getPathCategory( int i )
{
	return (int)m_vctPathDefinition.size() > i ? m_vctPathDefinition[i].first : -1;
}


float CvAnimationPathInfo::getPathParameter( int i )
{
	return (int)m_vctPathDefinition.size() > i ? m_vctPathDefinition[i].second : -1;
}


int CvAnimationPathInfo::getNumPathDefinitions()
{
	return m_vctPathDefinition.size();
}


CvAnimationPathDefinition * CvAnimationPathInfo::getPath( )
{
	return &m_vctPathDefinition;
}


bool CvAnimationPathInfo::isMissionPath() const
{
	return m_bMissionPath;
}


//------------------------------------------------------------------------------------------------
// FUNCTION:	CvAnimationPathInfo::read
//! \brief	  Reads in a CvAnimationPathInfo definition from XML
//! \param	  pXML Pointer to the XML loading object
//! \retval	 true if the definition was read successfully, false otherwise
//------------------------------------------------------------------------------------------------
bool CvAnimationPathInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_bMissionPath, L"bMissionPath");

	CvString animPathName;
	if (!pXML->GetChildXmlValByName(animPathName, L"Type"))
	{
		FErrorMsg("No animation path name found");
	}
	if (pXML->TryMoveToXmlFirstChild(L"PathEntry"))
	{
		do
		{
			int		iCurrentCategory;	// The current category information we are building
			float	fParameter;			// Temporary
			std::string	szTempString;

			if (pXML->GetOptionalChildXmlValByName(szTempString, L"Category"))
			{
				iCurrentCategory = pXML->GetInfoClass(szTempString.c_str());
				fParameter = 0.0f;
			}
			else
			{
				pXML->GetChildXmlValByName(szTempString, L"Operator");
				iCurrentCategory = GC.getInfoTypeForString(szTempString.c_str());
				iCurrentCategory += (int)ANIMOP_FIRST;
				if (!pXML->GetChildXmlValByName(&fParameter, L"Parameter"))
				{
					fParameter = 0.0f;
				}
			}
			m_vctPathDefinition.push_back(std::make_pair(iCurrentCategory, fParameter));
		} while (pXML->TryMoveToXmlNextSibling());
	}
	else
	{
		FErrorMsg(CvString::format("Animation path %s XML structure is invalid", animPathName).c_str());
		return false;
	}
	pXML->MoveToXmlParent();

	return true;
}


void CvAnimationPathInfo::copyNonDefaults(CvAnimationPathInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const bool bDefault = false;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (isMissionPath() == bDefault) m_bMissionPath = pClassInfo->isMissionPath();

	int		iCurrentCategory;
	float	fParameter;

	if ( m_vctPathDefinition.empty())
	{
		for ( int i = 0; i < (int)pClassInfo->m_vctPathDefinition.size(); i++ )
		{
			iCurrentCategory = pClassInfo->getPathCategory(i);
			fParameter = pClassInfo->getPathParameter(i);
			m_vctPathDefinition.push_back( std::make_pair(iCurrentCategory, fParameter ));
		}
	}
}

