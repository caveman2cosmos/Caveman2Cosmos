//------------------------------------------------------------------------------------------------
//  FILE:    CvInfoBase.cpp
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
#include "CvInfoBase.h"


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CInfoBase()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvInfoBase::CvInfoBase() :
m_bGraphicalOnly(false)
{
}


CvInfoBase::CvInfoBase(const char* szType) :
m_bGraphicalOnly(false),
m_szType(szType)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CInfoBase()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvInfoBase::~CvInfoBase()
{
}


void CvInfoBase::reset()
{
	//clear cache
	m_aCachedDescriptions.clear();
	m_szCachedText.clear();
	m_szCachedHelp.clear();
	m_szCachedStrategy.clear();
	m_szCachedCivilopedia.clear();
}


bool CvInfoBase::isGraphicalOnly() const
{
	return m_bGraphicalOnly;
}


const char* CvInfoBase::getType() const
{
	// AIAndy: Comparing a CvString to NULL makes no real sense
	//if (m_szType == NULL)
	//	return NULL;

	if (m_szType.empty())
	{
		return NULL;
	}

	return m_szType;
}


const char* CvInfoBase::getButton() const
{
	if (m_szButton.empty())
	{
		return NULL;
	}

	return m_szButton;
}


const wchar_t* CvInfoBase::getCivilopediaKey() const
{
	return m_szCivilopediaKey;
}


const wchar_t* CvInfoBase::getHelpKey() const
{
	return m_szHelpKey;
}


const wchar_t* CvInfoBase::getStrategyKey() const
{
	return m_szStrategyKey;
}


const wchar_t* CvInfoBase::getTextKeyWide() const
{
	return m_szTextKey;
}


const wchar_t* CvInfoBase::getDescription(uint uiForm) const
{
	PROFILE_EXTRA_FUNC();
	while(m_aCachedDescriptions.size() <= uiForm)
	{
		m_aCachedDescriptions.push_back(gDLL->getObjectText(m_szTextKey, m_aCachedDescriptions.size()));
	}

	return m_aCachedDescriptions[uiForm];
}


const wchar_t* CvInfoBase::getText() const
{
	// used instead of getDescription for Info entries that are not objects
	// so they do not have gender/plurality/forms defined in the Translator system
	if(m_szCachedText.empty())
	{
		m_szCachedText = gDLL->getText(m_szTextKey);
	}

	return m_szCachedText;
}


const wchar_t* CvInfoBase::getCivilopedia() const
{
	if(m_szCachedCivilopedia.empty())
	{
		m_szCachedCivilopedia = gDLL->getText(m_szCivilopediaKey);
	}

	return m_szCachedCivilopedia;
}


const wchar_t* CvInfoBase::getHelp() const
{
	if (m_szCachedHelp.empty())
	{
		m_szCachedHelp = gDLL->getText(m_szHelpKey);
	}

	return m_szCachedHelp;
}


const wchar_t* CvInfoBase::getStrategy() const
{
	if (m_szCachedStrategy.empty())
	{
		m_szCachedStrategy = gDLL->getText(m_szStrategyKey);
	}

	return m_szCachedStrategy;
}


//
// read from XML
//
bool CvInfoBase::read(CvXMLLoadUtility* pXML)
{
	pXML->GetOptionalChildXmlValByName(&m_bGraphicalOnly, L"bGraphicalOnly");
	pXML->GetOptionalChildXmlValByName(m_szType, L"Type");
	pXML->GetOptionalChildXmlValByName(m_szTextKey, L"Description");
	pXML->GetOptionalChildXmlValByName(m_szCivilopediaKey, L"Civilopedia");
	pXML->GetOptionalChildXmlValByName(m_szHelpKey, L"Help");
	pXML->GetOptionalChildXmlValByName(m_szStrategyKey, L"Strategy");
	pXML->GetOptionalChildXmlValByName(m_szButton, L"Button");

	return true;
}


void CvInfoBase::copyNonDefaults(const CvInfoBase* pClassInfo)
{
	const CvString cDefault = CvString::format("").GetCString();
	const CvWString wDefault = CvWString::format(L"").GetCString();

	if ( m_bGraphicalOnly == false )
	{
		m_bGraphicalOnly = pClassInfo->isGraphicalOnly();
	}

	if (( getType() == NULL ) || ( getType() == cDefault ))
	{
		m_szType = pClassInfo->getType();
	}

	if ( getCivilopediaKey() == NULL || getCivilopediaKey() == wDefault)
	{
		m_szCivilopediaKey = pClassInfo->getCivilopediaKey();
	}

	if ( getHelpKey() == NULL ||  getHelpKey() == wDefault )
	{
		m_szHelpKey = pClassInfo->getHelpKey();
	}

	if ( getStrategyKey() == NULL || getStrategyKey() == wDefault)
	{
		m_szStrategyKey = pClassInfo->getStrategyKey();
	}

	if ( getButton() == NULL || getButton() == cDefault)
	{
		m_szButton = pClassInfo->getButton();
	}

	if ( getTextKeyWide() == NULL || getTextKeyWide() == wDefault )
	{
		m_szTextKey = pClassInfo->getTextKeyWide();
	}
}

