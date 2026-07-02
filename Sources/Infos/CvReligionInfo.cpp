//------------------------------------------------------------------------------------------------
//  FILE:    CvReligionInfo.cpp
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
#include "CvReligionInfo.h"


//======================================================================================================
//					CvReligionInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvReligionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvReligionInfo::CvReligionInfo() :
m_iChar(0),
m_iTGAIndex(-1),
m_iHolyCityChar(0),
m_iTechPrereq(NO_TECH),
m_iFreeUnit(NO_UNIT),
m_iNumFreeUnits(0),
m_iSpreadFactor(0),
m_iMissionType(NO_MISSION),
m_paiGlobalReligionCommerce(NULL),
m_paiHolyCityCommerce(NULL),
m_paiStateReligionCommerce(NULL),
m_piFlavorValue(NULL),
m_PropertyManipulators()
{
	reset();
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvReligionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvReligionInfo::~CvReligionInfo()
{
	SAFE_DELETE_ARRAY(m_paiGlobalReligionCommerce);
	SAFE_DELETE_ARRAY(m_paiHolyCityCommerce);
	SAFE_DELETE_ARRAY(m_paiStateReligionCommerce);
	SAFE_DELETE_ARRAY(m_piFlavorValue);

	GC.removeDelayedResolution((int*)&m_iFreeUnit);
}


int CvReligionInfo::getChar() const
{
	return m_iChar;
}


int CvReligionInfo::getTGAIndex() const
{
	return m_iTGAIndex;
}


void CvReligionInfo::setChar(int i)
{
	m_iChar = 8550 + m_iTGAIndex * 2;
}


int CvReligionInfo::getHolyCityChar() const
{
	return m_iHolyCityChar;
}


void CvReligionInfo::setHolyCityChar(int i)
{
	m_iHolyCityChar = 8551 + m_iTGAIndex * 2;
}


int CvReligionInfo::getFreeUnit() const
{
	return m_iFreeUnit;
}


int CvReligionInfo::getNumFreeUnits() const
{
	return m_iNumFreeUnits;
}


int CvReligionInfo::getSpreadFactor() const
{
	return m_iSpreadFactor;
}


int CvReligionInfo::getMissionType() const
{
	return m_iMissionType;
}


void CvReligionInfo::setMissionType(int iNewType)
{
	m_iMissionType = iNewType;
}


const char* CvReligionInfo::getTechButton() const
{
	return m_szTechButton;
}


const char* CvReligionInfo::getGenericTechButton() const
{
	return m_szGenericTechButton;
}


const char* CvReligionInfo::getMovieFile() const
{
	return m_szMovieFile;
}


const char* CvReligionInfo::getMovieSound() const
{
	return m_szMovieSound;
}


const char* CvReligionInfo::getButtonDisabled() const
{
	static char szDisabled[512];

	szDisabled[0] = '\0';

	if ( getButton() && strlen(getButton()) > 4 )
	{
		strncpy( szDisabled, getButton(), strlen(getButton()) - 4 );
		szDisabled[strlen(getButton()) - 4] = '\0';
		strcat( szDisabled, "_D.dds" );
	}

	return szDisabled;
}


const char* CvReligionInfo::getSound() const
{
	return m_szSound;
}


void CvReligionInfo::setAdjectiveKey(const char* szVal)
{
	m_szAdjectiveKey = szVal;
}


const wchar_t* CvReligionInfo::getAdjectiveKey() const
{
	return m_szAdjectiveKey;
}


// Arrays

int CvReligionInfo::getGlobalReligionCommerce(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_paiGlobalReligionCommerce ? m_paiGlobalReligionCommerce[i] : 0;
}


int* CvReligionInfo::getGlobalReligionCommerceArray() const
{
	return m_paiGlobalReligionCommerce;
}


int CvReligionInfo::getHolyCityCommerce(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_paiHolyCityCommerce ? m_paiHolyCityCommerce[i] : 0;
}


int* CvReligionInfo::getHolyCityCommerceArray() const
{
	return m_paiHolyCityCommerce;
}


int CvReligionInfo::getStateReligionCommerce(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_paiStateReligionCommerce ? m_paiStateReligionCommerce[i] : 0;
}


int* CvReligionInfo::getStateReligionCommerceArray() const
{
	return m_paiStateReligionCommerce;
}


int CvReligionInfo::getFlavorValue(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFlavorTypes(), i);
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}


int CvReligionInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}


int CvReligionInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}


bool CvReligionInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}


//
// read from xml
//
bool CvReligionInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalTypeEnum(m_iTechPrereq, L"TechPrereq");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"FreeUnit");
	GC.addDelayedResolution((int*)&m_iFreeUnit, szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iNumFreeUnits, L"iFreeUnits");
	pXML->GetOptionalChildXmlValByName(&m_iSpreadFactor, L"iSpreadFactor");
	pXML->GetOptionalChildXmlValByName(&m_iTGAIndex, L"iTGAIndex");

	if (pXML->TryMoveToXmlFirstChild(L"GlobalReligionCommerces"))
	{
		pXML->SetCommerce(&m_paiGlobalReligionCommerce);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_paiGlobalReligionCommerce);

	if (pXML->TryMoveToXmlFirstChild(L"HolyCityCommerces"))
	{
		pXML->SetCommerce(&m_paiHolyCityCommerce);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_paiHolyCityCommerce);

	if (pXML->TryMoveToXmlFirstChild(L"StateReligionCommerces"))
	{
		pXML->SetCommerce(&m_paiStateReligionCommerce);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_paiStateReligionCommerce);

	pXML->GetChildXmlValByName(m_szTechButton, L"TechButton");
	pXML->GetOptionalChildXmlValByName(m_szGenericTechButton, L"GenericTechButton");
	pXML->GetOptionalChildXmlValByName(m_szMovieFile, L"MovieFile");
	pXML->GetOptionalChildXmlValByName(m_szMovieSound, L"MovieSound");
	pXML->GetOptionalChildXmlValByName(m_szSound, L"Sound");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Adjective");
	setAdjectiveKey(szTextVal);

	pXML->SetVariableListTagPair(&m_piFlavorValue, L"Flavors", GC.getNumFlavorTypes());

	pXML->SetOptionalVector(&m_aiCategories, L"Categories");

	m_PropertyManipulators.read(pXML);

	return true;
}


void CvReligionInfo::copyNonDefaults(const CvReligionInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	const int iDefault = 0;
	const int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	const CvString cDefault = CvString::format("").GetCString();
	const CvWString wDefault = CvWString::format(L"").GetCString();

	CvHotkeyInfo::copyNonDefaults(pClassInfo);

	if (getTechPrereq() == iTextDefault) m_iTechPrereq = pClassInfo->getTechPrereq();
	GC.copyNonDefaultDelayedResolution((int*)&m_iFreeUnit, (int*)&pClassInfo->m_iFreeUnit);

	if (getNumFreeUnits() == iDefault) m_iNumFreeUnits = pClassInfo->getNumFreeUnits();
	if (getSpreadFactor() == iDefault) m_iSpreadFactor = pClassInfo->getSpreadFactor();

	if (getTGAIndex() == iDefault) m_iTGAIndex = pClassInfo->getTGAIndex();

	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++)
	{
		if ( getGlobalReligionCommerce(i) == iDefault && pClassInfo->getGlobalReligionCommerce(i) != iDefault)
		{
			if ( NULL == m_paiGlobalReligionCommerce )
			{
				CvXMLLoadUtility::InitList(&m_paiGlobalReligionCommerce,NUM_COMMERCE_TYPES,iDefault);
			}
			m_paiGlobalReligionCommerce[i] = pClassInfo->getGlobalReligionCommerce(i);
		}

		if ( getHolyCityCommerce(i) == iDefault && pClassInfo->getHolyCityCommerce(i) != iDefault)
		{
			if ( NULL == m_paiHolyCityCommerce )
			{
				CvXMLLoadUtility::InitList(&m_paiHolyCityCommerce,NUM_COMMERCE_TYPES,iDefault);
			}
			m_paiHolyCityCommerce[i] = pClassInfo->getHolyCityCommerce(i);
		}

		if ( getStateReligionCommerce(i) == iDefault && pClassInfo->getStateReligionCommerce(i) != iDefault)
		{
			if ( NULL == m_paiStateReligionCommerce )
			{
				CvXMLLoadUtility::InitList(&m_paiStateReligionCommerce,NUM_COMMERCE_TYPES,iDefault);
			}
			m_paiStateReligionCommerce[i] = pClassInfo->getStateReligionCommerce(i);
		}
	}
	if (getTechButton() == cDefault) m_szTechButton = pClassInfo->getTechButton();
	if (getGenericTechButton() == cDefault) m_szGenericTechButton = pClassInfo->getGenericTechButton();
	if (getMovieFile() == cDefault) m_szMovieFile = pClassInfo->getMovieFile();
	if (getMovieSound() == cDefault) m_szMovieSound = pClassInfo->getMovieSound();
	if (getSound() == cDefault) m_szSound = pClassInfo->getSound();
	if (getAdjectiveKey() == wDefault) setAdjectiveKey(CvString::format("%s",pClassInfo->getAdjectiveKey()).GetCString());

	for ( int i = 0; i < GC.getNumFlavorTypes(); i++ )
	{
		if ( getFlavorValue(i) == iDefault && pClassInfo->getFlavorValue(i) != iDefault)
		{
			if ( NULL == m_piFlavorValue )
			{
				CvXMLLoadUtility::InitList(&m_piFlavorValue,GC.getNumFlavorTypes(),iDefault);
			}
			m_piFlavorValue[i] = pClassInfo->getFlavorValue(i);
		}
	}

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);

	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);
}


void CvReligionInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iFreeUnit);
	CheckSum(iSum, m_iNumFreeUnits);
	CheckSum(iSum, m_iSpreadFactor);
	CheckSum(iSum, m_iMissionType);

	// Arrays

	CheckSum(iSum, m_paiGlobalReligionCommerce, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_paiHolyCityCommerce, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_paiStateReligionCommerce, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_piFlavorValue, GC.getNumFlavorTypes());

	CheckSumC(iSum, m_aiCategories);

	m_PropertyManipulators.getCheckSum(iSum);
}

