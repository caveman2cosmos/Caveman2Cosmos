//------------------------------------------------------------------------------------------------
//  FILE:    CvEntityEventInfo.cpp
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
#include "CvEntityEventInfo.h"


/////////////////////////////////////////////////////////////////////////////////////////////
// CvEntityEventInfo
/////////////////////////////////////////////////////////////////////////////////////////////

CvEntityEventInfo::CvEntityEventInfo() :
m_bUpdateFormation(true)
{
}


CvEntityEventInfo::~CvEntityEventInfo()
{
}


bool CvEntityEventInfo::read(CvXMLLoadUtility* pXML)
{

	PROFILE_EXTRA_FUNC();
	CvString szTmp, szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	int iNumSibs, i;

	if (pXML->TryMoveToXmlFirstChild(L"AnimationPathTypes"))
	{
		// get the total number of children the current xml node has
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (iNumSibs > 0)
		{
			// if the call to the function that sets the current xml node to it's first non-comment
			// child and sets the parameter with the new node's value succeeds
			if (pXML->GetChildXmlVal(szTmp))
			{
				AnimationPathTypes eAnimationPath = (AnimationPathTypes)CvXMLLoadUtility::GetInfoClass( szTmp);
				if ( eAnimationPath > ANIMATIONPATH_NONE )
					m_vctAnimationPathType.push_back( eAnimationPath );

				// loop through all the siblings, we start at 1 since we already have the first value
				for (i=1;i<iNumSibs;i++)
				{
					if (!pXML->GetNextXmlVal(szTmp))
					{
						break;
					}
					AnimationPathTypes eAnimationPath = (AnimationPathTypes)CvXMLLoadUtility::GetInfoClass( szTmp);
					if ( eAnimationPath > ANIMATIONPATH_NONE )
						m_vctAnimationPathType.push_back( eAnimationPath );

				}
				pXML->MoveToXmlParent();
			}
		}
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"EffectTypes"))
	{
		// get the total number of children the current xml node has
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (iNumSibs > 0)
		{
			// if the call to the function that sets the current xml node to it's first non-comment
			// child and sets the parameter with the new node's value succeeds
			if (pXML->GetChildXmlVal(szTmp))
			{
				EffectTypes eEffectType = (EffectTypes)CvXMLLoadUtility::GetInfoClass( szTmp);
				if ( eEffectType > NO_EFFECT )
					m_vctEffectTypes.push_back( eEffectType );

				// loop through all the siblings, we start at 1 since we already have the first value
				for (i=1;i<iNumSibs;i++)
				{
					if (!pXML->GetNextXmlVal(szTmp))
					{
						break;
					}
					EffectTypes eEffectType = (EffectTypes)CvXMLLoadUtility::GetInfoClass( szTmp);
					if ( eEffectType > NO_EFFECT )
						m_vctEffectTypes.push_back( eEffectType );
				}
				pXML->MoveToXmlParent();
			}
		}
		pXML->MoveToXmlParent();
	}

	pXML->GetChildXmlValByName( &m_bUpdateFormation, L"bUpdateFormation" );

	return true;
}


void CvEntityEventInfo::copyNonDefaults(const CvEntityEventInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	for ( int i = 0; i < pClassInfo->getAnimationPathCount(); i++ )
	{
		bool bPushBack = true;
		for ( int j = 0; j < getAnimationPathCount(); j++)
		{
			if ( getAnimationPathType(j) == pClassInfo->getAnimationPathType(i) )
			{
				bPushBack = false;
			}
		}
		if ( bPushBack )
		{
			m_vctAnimationPathType.push_back( pClassInfo->getAnimationPathType(i));
		}
	}

	for ( int i = 0; i < pClassInfo->getEffectTypeCount(); i++ )
	{
		bool bPushBack = true;
		for ( int j = 0; j < getEffectTypeCount(); j++)
		{
			if ( getEffectType(j) == pClassInfo->getEffectType(i) )
			{
				bPushBack = false;
			}
		}
		if ( bPushBack )
		{
			m_vctEffectTypes.push_back( pClassInfo->getEffectType(i));
		}
	}

	if (getUpdateFormation() == bDefault) m_bUpdateFormation = pClassInfo->getUpdateFormation();
}


AnimationPathTypes CvEntityEventInfo::getAnimationPathType(int iIndex) const
{
	return iIndex >= (int)m_vctAnimationPathType.size() ? ANIMATIONPATH_NONE : m_vctAnimationPathType[iIndex];
}


EffectTypes CvEntityEventInfo::getEffectType(int iIndex) const
{
	return iIndex >= (int)m_vctEffectTypes.size() ? NO_EFFECT : m_vctEffectTypes[iIndex];
}


int CvEntityEventInfo::getAnimationPathCount() const
{
	return m_vctAnimationPathType.size();
}


int CvEntityEventInfo::getEffectTypeCount() const
{
	return m_vctEffectTypes.size();
}


bool CvEntityEventInfo::getUpdateFormation() const
{
	return m_bUpdateFormation;
}

