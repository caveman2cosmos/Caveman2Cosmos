//------------------------------------------------------------------------------------------------
//  FILE:    CvUnitFormationInfo.cpp
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
#include "CvUnitFormationInfo.h"



//======================================================================================================
//					CvUnitFormationInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvUnitFormationInfo()
//
//  \brief		Default Constructor
//
//------------------------------------------------------------------------------------------------------
CvUnitFormationInfo::CvUnitFormationInfo()
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvUnitFormationInfo()
//
//  \brief		Destructor
//
//------------------------------------------------------------------------------------------------------
CvUnitFormationInfo::~CvUnitFormationInfo()
{
}


const char* CvUnitFormationInfo::getFormationType() const
{
	return m_szFormationType;
}


int CvUnitFormationInfo::getNumEventTypes() const
{
	return m_vctEventTypes.size();
}


const EntityEventTypes &CvUnitFormationInfo::getEventType(int index) const
{
	return m_vctEventTypes[index];
}

const std::vector<EntityEventTypes> & CvUnitFormationInfo::getEventTypes() const
{
	return m_vctEventTypes;
}


int CvUnitFormationInfo::getNumUnitEntries() const
{
	return m_vctUnitEntries.size();
}


const CvUnitEntry &CvUnitFormationInfo::getUnitEntry(int index) const
{
	return m_vctUnitEntries[index];
}


void CvUnitFormationInfo::addUnitEntry(const CvUnitEntry &unitEntry)
{
	m_vctUnitEntries.push_back(unitEntry);
}


int CvUnitFormationInfo::getNumGreatUnitEntries() const
{
	return m_vctGreatUnitEntries.size();
}


const CvUnitEntry &CvUnitFormationInfo::getGreatUnitEntry(int index) const
{
	return m_vctGreatUnitEntries[index];
}


int CvUnitFormationInfo::getNumSiegeUnitEntries() const
{
	return m_vctSiegeUnitEntries.size();
}


const CvUnitEntry &CvUnitFormationInfo::getSiegeUnitEntry(int index) const
{
	return m_vctSiegeUnitEntries[index];
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvUnitFormationInfo()
//
//  \brief		Reads from XML
//
//------------------------------------------------------------------------------------------------------
bool CvUnitFormationInfo::read(CvXMLLoadUtility* pXML)
{

	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	int iIndex;
	bool bNextSibling;

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(m_szFormationType, L"FormationType");

	if (pXML->TryMoveToXmlFirstChild(L"EventMaskList" ))
	{
		if ( pXML->TryMoveToXmlFirstChild() )
		{
			pXML->GetXmlVal( szTextVal );
			do
			{
				iIndex = pXML->GetInfoClass(szTextVal);
				if ( iIndex != -1 )
					m_vctEventTypes.push_back( (EntityEventTypes)iIndex );
				bNextSibling = pXML->GetNextXmlVal( szTextVal );
			}
			while( bNextSibling );
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	// Read the entries
	if (pXML->TryMoveToXmlFirstChild(L"UnitEntry" ) )
	{
		do
		{
			CvUnitEntry unitEntry;
			pXML->GetChildXmlValByName(szTextVal, L"UnitEntryType");
			if (pXML->TryMoveToXmlFirstChild(L"Position" ) )
			{
				pXML->GetChildXmlValByName( &unitEntry.m_position.x, L"x");
				pXML->GetChildXmlValByName( &unitEntry.m_position.y, L"y");
				pXML->MoveToXmlParent();
			}
			pXML->GetChildXmlValByName( &unitEntry.m_fRadius, L"PositionRadius");
			pXML->GetChildXmlValByName( &unitEntry.m_fFacingDirection, L"Direction");
			pXML->GetChildXmlValByName( &unitEntry.m_fFacingVariance, L"DirVariation");

			if(szTextVal.CompareNoCase("Unit") == 0)
				m_vctUnitEntries.push_back(unitEntry);
			else if(szTextVal.CompareNoCase("General") == 0)
				m_vctGreatUnitEntries.push_back(unitEntry);
			else if(szTextVal.CompareNoCase("Siege") == 0)
				m_vctSiegeUnitEntries.push_back(unitEntry);
			else
			{
				//FErrorMsg("[Jason] Unknown unit formation entry type.");
			}
		}
		while ( pXML->TryMoveToXmlNextSibling(L"UnitEntry"));
		pXML->MoveToXmlParent();
	}

//	FAssertMsg(m_vctGreatUnitEntries.size() > 0, "[Jason] Formation missing great general entry.");
//	FAssertMsg(m_vctSiegeUnitEntries.size() > 0, "[Jason] Formation missing siege tower entry.");

	return true;
}

void CvUnitFormationInfo::copyNonDefaults(const CvUnitFormationInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getFormationType() == cDefault) m_szFormationType = pClassInfo->getFormationType();

	for ( int i = 0; i < pClassInfo->getNumEventTypes(); i++ )
	{
		m_vctEventTypes.push_back( pClassInfo->getEventType(i) );
	}

	for ( int i = 0; i < pClassInfo->getNumUnitEntries(); i++ )
	{
		m_vctUnitEntries.push_back( pClassInfo->getUnitEntry(i) );
	}

	for ( int i = 0; i < pClassInfo->getNumGreatUnitEntries(); i++ )
	{
		m_vctGreatUnitEntries.push_back( pClassInfo->getGreatUnitEntry(i) );
	}

	for ( int i = 0; i < pClassInfo->getNumSiegeUnitEntries(); i++ )
	{
		m_vctSiegeUnitEntries.push_back( pClassInfo->getSiegeUnitEntry(i) );
	}
}

