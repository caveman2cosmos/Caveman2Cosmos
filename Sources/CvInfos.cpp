//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvInfos.cpp
//
//  PURPOSE: The base class for all info classes to inherit from.  This gives us the base description
//				and type strings
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2003 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"

bool shouldHaveType = false;

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
	//m_szCachedGenericCategory.clear();
}

bool CvInfoBase::isGraphicalOnly() const
{
	return m_bGraphicalOnly;
}

const TCHAR* CvInfoBase::getType() const
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

const TCHAR* CvInfoBase::getButton() const
{
	if (m_szButton.empty())
	{
		return NULL;
	}

	return m_szButton;
}

const wchar* CvInfoBase::getCivilopediaKey() const
{
	return m_szCivilopediaKey;
}

const wchar* CvInfoBase::getHelpKey() const
{
	return m_szHelpKey;
}

const wchar* CvInfoBase::getStrategyKey() const
{
	return m_szStrategyKey;
}

//const wchar* CvInfoBase::getGenericCategoryKey() const
//{
//	return m_szGenericCategoryKey;
//}
const wchar* CvInfoBase::getTextKeyWide() const
{
	return m_szTextKey;
}

const wchar* CvInfoBase::getDescription(uint uiForm) const
{
	while(m_aCachedDescriptions.size() <= uiForm)
	{
		MEMORY_TRACK_EXEMPT();

		m_aCachedDescriptions.push_back(gDLL->getObjectText(m_szTextKey, m_aCachedDescriptions.size()));
	}

	return m_aCachedDescriptions[uiForm];
}

const wchar* CvInfoBase::getText() const
{
	// used instead of getDescription for Info entries that are not objects
	// so they do not have gender/plurality/forms defined in the Translator system
	if(m_szCachedText.empty())
	{
		m_szCachedText = gDLL->getText(m_szTextKey);
	}

	return m_szCachedText;
}

const wchar* CvInfoBase::getCivilopedia() const
{
	if(m_szCachedCivilopedia.empty())
	{
		m_szCachedCivilopedia = gDLL->getText(m_szCivilopediaKey);
	}

	return m_szCachedCivilopedia;
}

const wchar*  CvInfoBase::getHelp() const
{
	if (m_szCachedHelp.empty())
	{
		m_szCachedHelp = gDLL->getText(m_szHelpKey);
	}

	return m_szCachedHelp;
}

const wchar* CvInfoBase::getStrategy() const
{
	if (m_szCachedStrategy.empty())
	{
		m_szCachedStrategy = gDLL->getText(m_szStrategyKey);
	}

	return m_szCachedStrategy;
}

//const wchar* CvInfoBase::getGenericCategory() const
//{
//	if (m_szCachedGenericCategory.empty())
//	{
//		m_szCachedGenericCategory = gDLL->getText(m_szGenericCategoryKey);
//	}
//
//	return m_szCachedGenericCategory;
//}

bool CvInfoBase::isMatchForLink(std::wstring szLink, bool bKeysOnly) const
{
	if (szLink == CvWString(getType()).GetCString())
	{
		return true;
	}

	if (!bKeysOnly)
	{
		uint iNumForms = gDLL->getNumForms(getTextKeyWide());
		for (uint i = 0; i < iNumForms; i++)
		{
			if (szLink == getDescription(i))
			{
				return true;
			}
		}
	}

	return false;
}
//
// read from XML
// TYPE, DESC, BUTTON
//
bool CvInfoBase::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;

	// GRAPHICAL ONLY
	pXML->GetOptionalChildXmlValByName(&m_bGraphicalOnly, L"bGraphicalOnly");

	// TYPE
	pXML->GetOptionalChildXmlValByName(m_szType, L"Type");

	// DESCRIPTION
	pXML->GetOptionalChildXmlValByName(m_szTextKey, L"Description");

	// CIVILOPEDIA
	pXML->GetOptionalChildXmlValByName(m_szCivilopediaKey, L"Civilopedia");

	// HELP
	pXML->GetOptionalChildXmlValByName(m_szHelpKey, L"Help");

	// STRATEGY
	pXML->GetOptionalChildXmlValByName(m_szStrategyKey, L"Strategy");

	// GENERICCATEGORY
	//pXML->GetChildXmlValByName(m_szGenericCategoryKey, L"GenericCategory");

	// BUTTON
	pXML->GetOptionalChildXmlValByName(m_szButton, L"Button");

/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 10/26/07							MRGENIE		  */
/*																							  */
/*																							  */
/************************************************************************************************/
	CvXMLLoadUtilityModTools* p_szDirName = new CvXMLLoadUtilityModTools;
	p_szDirName->setLocationName( &m_szButton, (GC.getModDir()).c_str());
	SAFE_DELETE(p_szDirName);
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/

	return true;
}

void CvInfoBase::copyNonDefaults(CvInfoBase* pClassInfo, CvXMLLoadUtility* pXML)
{
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

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

	//if ( getGenericCategoryKey() == NULL || getGenericCategoryKey() == wDefault)
	//{
	//	m_szGenericCategoryKey = pClassInfo->getGenericCategoryKey();
	//}

	if ( getButton() == NULL || getButton() == cDefault)
	{
		m_szButton = pClassInfo->getButton();
	}

	if ( getTextKeyWide() == NULL || getTextKeyWide() == wDefault )
	{
		m_szTextKey = pClassInfo->getTextKeyWide();
	}
}

//======================================================================================================
//					CvScalableInfo
//======================================================================================================
bool CvScalableInfo::read(CvXMLLoadUtility* pXML)
{
	float fScale;
	pXML->GetOptionalChildXmlValByName(&fScale, L"fScale");
	setScale(fScale);
	pXML->GetOptionalChildXmlValByName(&fScale, L"fInterfaceScale", 1.0f);
	setInterfaceScale(fScale);
	return true;
}

/************************************************************************************************/
/* XMLCOPY								 11/19/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvScalableInfo::copyNonDefaults(CvScalableInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	if (getScale() == fDefault) setScale(pClassInfo->getScale());
	if (getInterfaceScale() == 1.0f) setInterfaceScale(pClassInfo->getInterfaceScale());
}

float CvScalableInfo::getScale() const
{
	return m_fScale;
}

void CvScalableInfo::setScale(float fScale)
{
	m_fScale = fScale;
}

float CvScalableInfo::getInterfaceScale() const
{
	return m_fInterfaceScale;
}

void CvScalableInfo::setInterfaceScale(float fInterfaceScale)
{
	m_fInterfaceScale = fInterfaceScale;
}


//======================================================================================================
//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvHotkeyInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvHotkeyInfo::CvHotkeyInfo() :
m_iActionInfoIndex(-1),
m_iHotKeyVal(-1),
m_iHotKeyPriority(-1),
m_iHotKeyValAlt(-1),
m_iHotKeyPriorityAlt(-1),
m_iOrderPriority(0),
m_bAltDown(false),
m_bShiftDown(false),
m_bCtrlDown(false),
m_bAltDownAlt(false),
m_bShiftDownAlt(false),
m_bCtrlDownAlt(false)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvHotkeyInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvHotkeyInfo::~CvHotkeyInfo()
{
}

bool CvHotkeyInfo::read(CvXMLLoadUtility* pXML)
{
	int iVal;
	bool bVal;
	CvString szTextVal;

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"HotKey");
	setHotKey(szTextVal);

	iVal = pXML->GetHotKeyInt(szTextVal);
	setHotKeyVal(iVal);

	pXML->GetOptionalChildXmlValByName(&iVal, L"iHotKeyPriority", -1);
	setHotKeyPriority(iVal);


	pXML->GetOptionalChildXmlValByName(szTextVal, L"HotKeyAlt");
	iVal = pXML->GetHotKeyInt(szTextVal);

	setHotKeyValAlt(iVal);

	pXML->GetOptionalChildXmlValByName(&iVal, L"iHotKeyPriorityAlt", -1);
	setHotKeyPriorityAlt(iVal);


	pXML->GetOptionalChildXmlValByName(&bVal, L"bAltDown");
	setAltDown(bVal);

	pXML->GetOptionalChildXmlValByName(&bVal, L"bShiftDown");
	setShiftDown(bVal);

	pXML->GetOptionalChildXmlValByName(&bVal, L"bCtrlDown");
	setCtrlDown(bVal);

	pXML->GetOptionalChildXmlValByName(&bVal, L"bAltDownAlt");
	setAltDownAlt(bVal);

	pXML->GetOptionalChildXmlValByName(&bVal, L"bShiftDownAlt");
	setShiftDownAlt(bVal);

	pXML->GetOptionalChildXmlValByName(&bVal, L"bCtrlDownAlt");
	setCtrlDownAlt(bVal);

	pXML->GetOptionalChildXmlValByName(&iVal, L"iOrderPriority", 5);
	setOrderPriority(iVal);

	setHotKeyDescription(getTextKeyWide(), NULL, pXML->CreateHotKeyFromDescription(getHotKey(), m_bShiftDown, m_bAltDown, m_bCtrlDown));

	return true;
}

void CvHotkeyInfo::copyNonDefaults(CvHotkeyInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	bool bDefault = false;
	int iDefault = 0;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	if ( getHotKeyVal() == iDefault )
	{
		m_iHotKeyVal = pClassInfo->getHotKeyVal();
	}
	if ( getHotKeyPriority() == -1 )
	{
		m_iHotKeyPriority = pClassInfo->getHotKeyPriority();
	}
	if ( getHotKeyValAlt() == iDefault )
	{
		m_iHotKeyValAlt = pClassInfo->getHotKeyValAlt();
	}
	if ( getHotKeyPriorityAlt() == -1 )
	{
		m_iHotKeyPriorityAlt = pClassInfo->getHotKeyPriorityAlt();
	}
	if ( getOrderPriority() == 5 )
	{
		m_iOrderPriority = pClassInfo->getOrderPriority();
	}

	if ( isAltDown() == bDefault )
	{
		m_bAltDown = pClassInfo->isAltDown();
	}
	if ( isShiftDown() == bDefault )
	{
		m_bShiftDown = pClassInfo->isShiftDown();
	}
	if ( isCtrlDown() == bDefault )
	{
		m_bCtrlDown = pClassInfo->isCtrlDown();
	}
	if ( isAltDownAlt() == bDefault )
	{
		m_bAltDownAlt = pClassInfo->isAltDownAlt();
	}
	if ( isShiftDownAlt() == bDefault )
	{
		m_bShiftDownAlt = pClassInfo->isShiftDownAlt();
	}
	if ( isCtrlDownAlt() == bDefault )
	{
		m_bCtrlDownAlt = pClassInfo->isCtrlDownAlt();
	}

	if ( getHotKey() == cDefault )
	{
		m_szHotKey = pClassInfo->getHotKey();
	}
	if ( getHotKeyDescriptionKey() == wDefault )
	{
		m_szHotKeyDescriptionKey = pClassInfo->getHotKeyDescriptionKey();
	}
	if ( getHotKeyAltDescriptionKey() == wDefault )
	{
		m_szHotKeyAltDescriptionKey = pClassInfo->getHotKeyAltDescriptionKey();
	}
	if ( getHotKeyString() == wDefault )
	{
		m_szHotKeyString = pClassInfo->getHotKeyString();
	}
}

int CvHotkeyInfo::getActionInfoIndex() const
{
	return m_iActionInfoIndex;
}

void CvHotkeyInfo::setActionInfoIndex(int i)
{
	m_iActionInfoIndex = i;
}

int CvHotkeyInfo::getHotKeyVal() const
{
	return m_iHotKeyVal;
}

void CvHotkeyInfo::setHotKeyVal(int i)
{
	m_iHotKeyVal = i;
}

int CvHotkeyInfo::getHotKeyPriority() const
{
	return m_iHotKeyPriority;
}

void CvHotkeyInfo::setHotKeyPriority(int i)
{
	m_iHotKeyPriority = i;
}

int CvHotkeyInfo::getHotKeyValAlt() const
{
	return m_iHotKeyValAlt;
}

void CvHotkeyInfo::setHotKeyValAlt(int i)
{
	m_iHotKeyValAlt = i;
}

int CvHotkeyInfo::getHotKeyPriorityAlt() const
{
	return m_iHotKeyPriorityAlt;
}

void CvHotkeyInfo::setHotKeyPriorityAlt(int i)
{
	m_iHotKeyPriorityAlt = i;
}

int CvHotkeyInfo::getOrderPriority() const
{
	return m_iOrderPriority;
}

void CvHotkeyInfo::setOrderPriority(int i)
{
	m_iOrderPriority = i;
}

bool CvHotkeyInfo::isAltDown() const
{
	return m_bAltDown;
}

void CvHotkeyInfo::setAltDown(bool b)
{
	m_bAltDown = b;
}

bool CvHotkeyInfo::isShiftDown() const
{
	return m_bShiftDown;
}

void CvHotkeyInfo::setShiftDown(bool b)
{
	m_bShiftDown = b;
}

bool CvHotkeyInfo::isCtrlDown() const
{
	return m_bCtrlDown;
}

void CvHotkeyInfo::setCtrlDown(bool b)
{
	m_bCtrlDown = b;
}

bool CvHotkeyInfo::isAltDownAlt() const
{
	return m_bAltDownAlt;
}

void CvHotkeyInfo::setAltDownAlt(bool b)
{
	m_bAltDownAlt = b;
}

bool CvHotkeyInfo::isShiftDownAlt() const
{
	return m_bShiftDownAlt;
}

void CvHotkeyInfo::setShiftDownAlt(bool b)
{
	m_bShiftDownAlt = b;
}

bool CvHotkeyInfo::isCtrlDownAlt() const
{
	return m_bCtrlDownAlt;
}

void CvHotkeyInfo::setCtrlDownAlt(bool b)
{
	m_bCtrlDownAlt = b;
}

const TCHAR* CvHotkeyInfo::getHotKey() const
{
	return m_szHotKey;
}

void CvHotkeyInfo::setHotKey(const TCHAR* szVal)
{
	m_szHotKey = szVal;
}

const WCHAR* CvHotkeyInfo::getHotKeyDescriptionKey() const
{
	return m_szHotKeyDescriptionKey;
}

const WCHAR* CvHotkeyInfo::getHotKeyAltDescriptionKey() const
{
	return m_szHotKeyAltDescriptionKey;
}

const WCHAR* CvHotkeyInfo::getHotKeyString() const
{
	return m_szHotKeyString;
}
std::wstring CvHotkeyInfo::getHotKeyDescription() const
{
	CvWString szTemptext;
	if (!m_szHotKeyAltDescriptionKey.empty())
	{
		szTemptext.Format(L"%s (%s)", gDLL->getObjectText(m_szHotKeyAltDescriptionKey, 0).GetCString(), gDLL->getObjectText(m_szHotKeyDescriptionKey, 0).GetCString());
	}
	else
	{
		szTemptext = gDLL->getObjectText(m_szHotKeyDescriptionKey, 0);
	}

	if (!m_szHotKeyString.empty())
	{
		szTemptext += m_szHotKeyString;
	}

	return szTemptext;
}

void CvHotkeyInfo::setHotKeyDescription(const wchar* szHotKeyDescKey, const wchar* szHotKeyAltDescKey, const wchar* szHotKeyString)
{
	m_szHotKeyDescriptionKey = szHotKeyDescKey;
	m_szHotKeyAltDescriptionKey = szHotKeyAltDescKey;
	m_szHotKeyString = szHotKeyString;
}

//======================================================================================================
//					CvControlInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvControlInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvControlInfo::CvControlInfo()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvControlInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvControlInfo::~CvControlInfo()
{
}

bool CvControlInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	return true;
}
void CvControlInfo::copyNonDefaults(CvControlInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	CvHotkeyInfo::copyNonDefaults(pClassInfo, pXML);
}

//======================================================================================================
//					CvActionInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvActionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvActionInfo::CvActionInfo() :
m_iOriginalIndex(-1),
m_eSubType(NO_ACTIONSUBTYPE)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvActionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvActionInfo::~CvActionInfo()
{
}

int CvActionInfo::getMissionData() const
{

	if	(
				(ACTIONSUBTYPE_BUILD == m_eSubType)				||
				(ACTIONSUBTYPE_RELIGION == m_eSubType)		||
				(ACTIONSUBTYPE_CORPORATION == m_eSubType)		||
				(ACTIONSUBTYPE_SPECIALIST == m_eSubType)	||
				(ACTIONSUBTYPE_BUILDING == m_eSubType)
			)
	{
		return m_iOriginalIndex;
	}

	return -1;
}

int CvActionInfo::getCommandData() const
{

	if	(
				(ACTIONSUBTYPE_PROMOTION == m_eSubType)	||
				(ACTIONSUBTYPE_UNIT == m_eSubType)
			)
	{
		return m_iOriginalIndex;
	}
	else if (ACTIONSUBTYPE_COMMAND == m_eSubType)
	{
		return GC.getCommandInfo((CommandTypes)m_iOriginalIndex).getAutomate();
	}
	else if (ACTIONSUBTYPE_AUTOMATE == m_eSubType)
	{
		return GC.getAutomateInfo(m_iOriginalIndex).getAutomate();
	}

	return -1;
}

int CvActionInfo::getAutomateType() const
{

	if (ACTIONSUBTYPE_COMMAND == m_eSubType)
	{
		return GC.getCommandInfo((CommandTypes)m_iOriginalIndex).getAutomate();
	}
	else if (ACTIONSUBTYPE_AUTOMATE == m_eSubType)
	{
		return GC.getAutomateInfo(m_iOriginalIndex).getAutomate();
	}

	return NO_AUTOMATE;
}

int CvActionInfo::getInterfaceModeType() const
{
	if (ACTIONSUBTYPE_INTERFACEMODE == m_eSubType)
	{
		return m_iOriginalIndex;
	}
	return NO_INTERFACEMODE;
}

int CvActionInfo::getMissionType() const
{
	if (ACTIONSUBTYPE_BUILD == m_eSubType)
	{
		return GC.getBuildInfo((BuildTypes)m_iOriginalIndex).getMissionType();
	}
	else if (ACTIONSUBTYPE_RELIGION == m_eSubType)
	{
		return GC.getReligionInfo((ReligionTypes)m_iOriginalIndex).getMissionType();
	}
	else if (ACTIONSUBTYPE_CORPORATION == m_eSubType)
	{
		return GC.getCorporationInfo((CorporationTypes)m_iOriginalIndex).getMissionType();
	}
	else if (ACTIONSUBTYPE_SPECIALIST == m_eSubType)
	{
		return GC.getSpecialistInfo((SpecialistTypes)m_iOriginalIndex).getMissionType();
	}
	else if (ACTIONSUBTYPE_BUILDING == m_eSubType)
	{
		return GC.getBuildingInfo((BuildingTypes)m_iOriginalIndex).getMissionType();
	}
	else if (ACTIONSUBTYPE_MISSION == m_eSubType)
	{
		return m_iOriginalIndex;
	}

	return NO_MISSION;
}

int CvActionInfo::getCommandType() const
{
	if (ACTIONSUBTYPE_COMMAND == m_eSubType)
	{
		return m_iOriginalIndex;
	}
	else if (ACTIONSUBTYPE_PROMOTION == m_eSubType)
	{
		return GC.getPromotionInfo((PromotionTypes)m_iOriginalIndex).getCommandType();
	}
	else if (ACTIONSUBTYPE_UNIT == m_eSubType)
	{
		return GC.getUnitInfo((UnitTypes)m_iOriginalIndex).getCommandType();
	}
	else if (ACTIONSUBTYPE_AUTOMATE == m_eSubType)
	{
		return GC.getAutomateInfo(m_iOriginalIndex).getCommand();
	}

	return NO_COMMAND;
}

int CvActionInfo::getControlType() const
{
	if (ACTIONSUBTYPE_CONTROL == m_eSubType)
	{
		return m_iOriginalIndex;
	}
	return -1;
}

int CvActionInfo::getOriginalIndex() const
{
	return m_iOriginalIndex;
}

void CvActionInfo::setOriginalIndex(int i)
{
	m_iOriginalIndex = i;
}

bool CvActionInfo::isConfirmCommand() const
{
	if	(ACTIONSUBTYPE_COMMAND == m_eSubType)
	{
		return GC.getCommandInfo((CommandTypes)m_iOriginalIndex).getConfirmCommand();
	}
	else if (ACTIONSUBTYPE_AUTOMATE == m_eSubType)
	{
		return GC.getAutomateInfo(m_iOriginalIndex).getConfirmCommand();
	}

	return false;
}

bool CvActionInfo::isVisible() const
{

	if (ACTIONSUBTYPE_CONTROL == m_eSubType)
	{
		return false;
	}
	else if	(ACTIONSUBTYPE_COMMAND == m_eSubType)
	{
		return GC.getCommandInfo((CommandTypes)m_iOriginalIndex).getVisible();
	}
	else if (ACTIONSUBTYPE_AUTOMATE == m_eSubType)
	{
		return GC.getAutomateInfo(m_iOriginalIndex).getVisible();
	}
	else if (ACTIONSUBTYPE_MISSION == m_eSubType)
	{
		return GC.getMissionInfo((MissionTypes)m_iOriginalIndex).getVisible();
	}
	else if (ACTIONSUBTYPE_INTERFACEMODE== m_eSubType)
	{
		return GC.getInterfaceModeInfo((InterfaceModeTypes)m_iOriginalIndex).getVisible();
	}

	return true;
}

ActionSubTypes CvActionInfo::getSubType() const
{
	return m_eSubType;
}

void CvActionInfo::setSubType(ActionSubTypes eSubType)
{
	m_eSubType = eSubType;
}

CvHotkeyInfo* CvActionInfo::getHotkeyInfo() const
{
	switch (getSubType())
	{
		case ACTIONSUBTYPE_INTERFACEMODE:
			return &GC.getInterfaceModeInfo((InterfaceModeTypes)getOriginalIndex());
			break;
		case ACTIONSUBTYPE_COMMAND:
			return &GC.getCommandInfo((CommandTypes)getOriginalIndex());
			break;
		case ACTIONSUBTYPE_BUILD:
			return &GC.getBuildInfo((BuildTypes)getOriginalIndex());
			break;
		case ACTIONSUBTYPE_PROMOTION:
			return &GC.getPromotionInfo((PromotionTypes)getOriginalIndex());
			break;
		case ACTIONSUBTYPE_UNIT:
			return &GC.getUnitInfo((UnitTypes)getOriginalIndex());
			break;
		case ACTIONSUBTYPE_RELIGION:
			return &GC.getReligionInfo((ReligionTypes)getOriginalIndex());
			break;
		case ACTIONSUBTYPE_CORPORATION:
			return &GC.getCorporationInfo((CorporationTypes)getOriginalIndex());
			break;
		case ACTIONSUBTYPE_SPECIALIST:
			return &GC.getSpecialistInfo((SpecialistTypes)getOriginalIndex());
			break;
		case ACTIONSUBTYPE_BUILDING:
			return &GC.getBuildingInfo((BuildingTypes)getOriginalIndex());
			break;
		case ACTIONSUBTYPE_CONTROL:
			return &GC.getControlInfo((ControlTypes)getOriginalIndex());
			break;
		case ACTIONSUBTYPE_AUTOMATE:
			return &GC.getAutomateInfo(getOriginalIndex());
			break;
		case ACTIONSUBTYPE_MISSION:
			return &GC.getMissionInfo((MissionTypes)getOriginalIndex());
			break;
	}

	FAssertMsg((0) ,"Unknown Action Subtype in CvActionInfo::getHotkeyInfo");
	return NULL;
}

const TCHAR* CvActionInfo::getType() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getType();
	}

	return NULL;
}

const wchar* CvActionInfo::getDescription() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getDescription();
	}

	return L"";
}

const wchar* CvActionInfo::getCivilopedia() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getCivilopedia();
	}

	return L"";
}

const wchar* CvActionInfo::getHelp() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getHelp();
	}

	return L"";
}

const wchar* CvActionInfo::getStrategy() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getStrategy();
	}

	return L"";
}

//const wchar* CvActionInfo::getGenericCategory() const
//{
//	if (getHotkeyInfo())
//	{
//		return getHotkeyInfo()->getGenericCategory();
//	}
//
//	return L"";
//}

const TCHAR* CvActionInfo::getButton() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getButton();
	}

	return NULL;
}

const wchar* CvActionInfo::getTextKeyWide() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getTextKeyWide();
	}

	return NULL;
}

int CvActionInfo::getActionInfoIndex() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getActionInfoIndex();
	}

	return -1;
}

int CvActionInfo::getHotKeyVal() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getHotKeyVal();
	}

	return -1;
}

int CvActionInfo::getHotKeyPriority() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getHotKeyPriority();
	}

	return -1;
}

int CvActionInfo::getHotKeyValAlt() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getHotKeyValAlt();
	}

	return -1;
}

int CvActionInfo::getHotKeyPriorityAlt() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getHotKeyPriorityAlt();
	}

	return -1;
}

int CvActionInfo::getOrderPriority() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getOrderPriority();
	}

	return -1;
}

bool CvActionInfo::isAltDown() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->isAltDown();
	}

	return false;
}

bool CvActionInfo::isShiftDown() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->isShiftDown();
	}

	return false;
}

bool CvActionInfo::isCtrlDown() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->isCtrlDown();
	}

	return false;
}

bool CvActionInfo::isAltDownAlt() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->isAltDownAlt();
	}

	return false;
}

bool CvActionInfo::isShiftDownAlt() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->isShiftDownAlt();
	}

	return false;
}

bool CvActionInfo::isCtrlDownAlt() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->isCtrlDownAlt();
	}

	return false;
}

const TCHAR* CvActionInfo::getHotKey() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getHotKey();
	}

	return NULL;
}

std::wstring CvActionInfo::getHotKeyDescription() const
{
	if (getHotkeyInfo())
	{
		return getHotkeyInfo()->getHotKeyDescription();
	}

	return L"";
}

//======================================================================================================
//					CvInterfaceModeInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvInterfaceModeInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvInterfaceModeInfo::CvInterfaceModeInfo() :
m_iCursorIndex(NO_CURSOR),
m_iMissionType(NO_MISSION),
m_bVisible(false),
m_bGotoPlot(false),
m_bHighlightPlot(false),
m_bSelectType(false),
m_bSelectAll(false)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvInterfaceModeInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvInterfaceModeInfo::~CvInterfaceModeInfo()
{
}

int CvInterfaceModeInfo::getCursorIndex() const
{
	return m_iCursorIndex;
}

int CvInterfaceModeInfo::getMissionType() const
{
	return m_iMissionType;
}

bool CvInterfaceModeInfo::getVisible() const
{
	return m_bVisible;
}

bool CvInterfaceModeInfo::getGotoPlot() const
{
	return m_bGotoPlot;
}

bool CvInterfaceModeInfo::getHighlightPlot() const
{
	return m_bHighlightPlot;
}

bool CvInterfaceModeInfo::getSelectType() const
{
	return m_bSelectType;
}

bool CvInterfaceModeInfo::getSelectAll() const
{
	return m_bSelectAll;
}

bool CvInterfaceModeInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CursorType");
	m_iCursorIndex = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Mission");
	m_iMissionType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_bVisible, L"bVisible");
	pXML->GetOptionalChildXmlValByName(&m_bGotoPlot, L"bGotoPlot");
	pXML->GetOptionalChildXmlValByName(&m_bHighlightPlot, L"bHighlightPlot");
	pXML->GetOptionalChildXmlValByName(&m_bSelectType, L"bSelectType");
	pXML->GetOptionalChildXmlValByName(&m_bSelectAll, L"bSelectAll");

	return true;
}
void CvInterfaceModeInfo::copyNonDefaults(CvInterfaceModeInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvHotkeyInfo::copyNonDefaults(pClassInfo, pXML);

	if (getCursorIndex() == iTextDefault) m_iCursorIndex = pClassInfo->getCursorIndex();
	if (getMissionType() == iTextDefault) m_iMissionType = pClassInfo->getMissionType();

	if (getVisible() == bDefault) m_bVisible = pClassInfo->getVisible();
	if (getGotoPlot() == bDefault) m_bGotoPlot = pClassInfo->getGotoPlot();
	if (getHighlightPlot() == bDefault) m_bHighlightPlot = pClassInfo->getHighlightPlot();
	if (getSelectType() == bDefault) m_bSelectType = pClassInfo->getSelectType();
	if (getSelectAll() == bDefault) m_bSelectAll = pClassInfo->getSelectAll();
}

//======================================================================================================
//					CvAdvisorInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvAdvisorInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvAdvisorInfo::CvAdvisorInfo()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvAdvisorInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvAdvisorInfo::~CvAdvisorInfo()
{
}

const TCHAR* CvAdvisorInfo::getTexture() const
{
	return m_szTexture;
}

void CvAdvisorInfo::setTexture(const TCHAR* szVal)
{
	m_szTexture = szVal;
}

int CvAdvisorInfo::getNumCodes() const
{
	return m_vctEnableDisableCodes.size();
}

int CvAdvisorInfo::getEnableCode(uint uiCode) const
{
	FAssert( uiCode < m_vctEnableDisableCodes.size() );
	return m_vctEnableDisableCodes[uiCode].first;
}

int CvAdvisorInfo::getDisableCode(uint uiCode) const
{
	FAssert( uiCode < m_vctEnableDisableCodes.size() );
	return m_vctEnableDisableCodes[uiCode].second;
}

bool CvAdvisorInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Texture", "");
	setTexture( szTextVal );

	pXML->TryMoveToXmlFirstChild();
	while(pXML->TryMoveToXmlNextSibling(L"EventCodes"))
	{
		int iEnableCode, iDisableCode;
		pXML->GetChildXmlValByName(&iEnableCode, L"iEnableCode");
		pXML->GetChildXmlValByName(&iDisableCode, L"iDisableCode");
		m_vctEnableDisableCodes.push_back( std::make_pair( iEnableCode, iDisableCode ));
	}
	pXML->MoveToXmlParent();

	return true;
}
/************************************************************************************************/
/* XMLCOPY								 11/19/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvAdvisorInfo::copyNonDefaults(CvAdvisorInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getTexture() == cDefault) setTexture(pClassInfo->getTexture());

	if ( getNumCodes() == 0 )  //Only copy old values if the new doesn't hold a tag
	{
		for ( int iI = 0; iI < pClassInfo->getNumCodes(); iI++)
		{
			int iEnableCode, iDisableCode;
			iEnableCode = pClassInfo->getEnableCode(iI);
			iDisableCode = pClassInfo->getDisableCode(iI);
			m_vctEnableDisableCodes.push_back( std::make_pair( iEnableCode, iDisableCode ));
		}
	}
}

//======================================================================================================
//					CvCursorInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCursorInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCursorInfo::CvCursorInfo()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCursorInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCursorInfo::~CvCursorInfo()
{
}

const TCHAR* CvCursorInfo::getPath()
{
	return m_szPath;
}

void CvCursorInfo::setPath(const TCHAR* szVal)
{
	m_szPath = szVal;
}

bool CvCursorInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"CursorPath");
	setPath(szTextVal);

	return true;
}
/************************************************************************************************/
/* XMLCOPY								 11/19/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvCursorInfo::copyNonDefaults(CvCursorInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getPath() == cDefault) setPath(pClassInfo->getPath());
}

//======================================================================================================
//					CvThroneRoomCamera
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvThroneRoomCamera()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvThroneRoomCamera::CvThroneRoomCamera()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvThroneRoomCamera()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvThroneRoomCamera::~CvThroneRoomCamera()
{
}

const TCHAR* CvThroneRoomCamera::getFileName()
{
	return m_szFileName;
}

void CvThroneRoomCamera::setFileName(const TCHAR* szVal)
{
	m_szFileName = szVal;
}

bool CvThroneRoomCamera::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"FileName");
	setFileName(szTextVal);

	return true;
}
void CvThroneRoomCamera::copyNonDefaults(CvThroneRoomCamera* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getFileName() == cDefault) setFileName(pClassInfo->getFileName());
}

//======================================================================================================
//					CvThroneRoomInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvThroneRoomInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvThroneRoomInfo::CvThroneRoomInfo() :
m_iFromState(0),
m_iToState(0),
m_iAnimation(0)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvThroneRoomInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvThroneRoomInfo::~CvThroneRoomInfo()
{
}

const TCHAR* CvThroneRoomInfo::getEvent()
{
	return m_szEvent;
}

void CvThroneRoomInfo::setEvent(const TCHAR* szVal)
{
	m_szEvent = szVal;
}

const TCHAR* CvThroneRoomInfo::getNodeName()
{
	return m_szNodeName;
}

void CvThroneRoomInfo::setNodeName(const TCHAR* szVal)
{
	m_szNodeName = szVal;
}

int CvThroneRoomInfo::getFromState()
{
	return m_iFromState;
}

void CvThroneRoomInfo::setFromState(int iVal)
{
	m_iFromState = iVal;
}

int CvThroneRoomInfo::getToState()
{
	return m_iToState;
}

void CvThroneRoomInfo::setToState(int iVal)
{
	m_iToState = iVal;
}

int CvThroneRoomInfo::getAnimation()
{
	return m_iAnimation;
}

void CvThroneRoomInfo::setAnimation(int iVal)
{
	m_iAnimation= iVal;
}

bool CvThroneRoomInfo::read(CvXMLLoadUtility* pXML)
{
	int iVal;
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Event");
	setEvent(szTextVal);
	pXML->GetOptionalChildXmlValByName(&iVal, L"iFromState" );
	setFromState(iVal);
	pXML->GetOptionalChildXmlValByName(&iVal, L"iToState" );
	setToState(iVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"NodeName");
	setNodeName(szTextVal);
	pXML->GetOptionalChildXmlValByName(&iVal, L"iAnimation" );
	setAnimation(iVal);

	return true;
}
void CvThroneRoomInfo::copyNonDefaults(CvThroneRoomInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getEvent() == cDefault) setEvent(pClassInfo->getEvent());
	if (getFromState() == iDefault) setFromState(pClassInfo->getFromState());
	if (getToState() == iDefault) setToState(pClassInfo->getToState());
	if (getNodeName() == cDefault) setNodeName(pClassInfo->getNodeName());
	if (getAnimation() == iDefault) setAnimation(pClassInfo->getAnimation());
}

//======================================================================================================
//					CvThroneRoomStyleInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvThroneRoomStyleInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvThroneRoomStyleInfo::CvThroneRoomStyleInfo()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvThroneRoomStyleInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvThroneRoomStyleInfo::~CvThroneRoomStyleInfo()
{
}

const TCHAR* CvThroneRoomStyleInfo::getArtStyleType()
{
	return m_szArtStyleType;
}

void CvThroneRoomStyleInfo::setArtStyleType(const TCHAR* szVal)
{
	m_szArtStyleType = szVal;
}

const TCHAR* CvThroneRoomStyleInfo::getEraType()
{
	return m_szEraType;
}

void CvThroneRoomStyleInfo::setEraType(const TCHAR* szVal)
{
	m_szEraType = szVal;
}

const TCHAR* CvThroneRoomStyleInfo::getFileName()
{
	return m_szFileName;
}

void CvThroneRoomStyleInfo::setFileName(const TCHAR* szVal)
{
	m_szFileName = szVal;
}

bool CvThroneRoomStyleInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ArtStyleType");
	setArtStyleType(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"EraType");
	setEraType(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"FileName");
	setFileName(szTextVal);

	//node names
	if(pXML->TryMoveToXmlFirstChild())
	{
		while(pXML->TryMoveToXmlNextSibling(L"NodeName"))
		{
			pXML->GetXmlVal(szTextVal);
			m_aNodeNames.push_back(szTextVal);
		}
		pXML->MoveToXmlParent();
	}

	//texture names
	if(pXML->TryMoveToXmlFirstChild())
	{
		while(pXML->TryMoveToXmlNextSibling(L"TextureName"))
		{
			pXML->GetXmlVal(szTextVal);
			m_aTextureNames.push_back(szTextVal);
		}
		pXML->MoveToXmlParent();
	}

	return true;
}
void CvThroneRoomStyleInfo::copyNonDefaults(CvThroneRoomStyleInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getArtStyleType() == cDefault) setArtStyleType(pClassInfo->getArtStyleType());
	if (getEraType() == cDefault) setEraType(pClassInfo->getEraType());
	if (getFileName() == cDefault) setFileName(pClassInfo->getFileName());

	/*
	m_aNodeNames and m_aTextureNames don't seem to be used?
	since i hardly doubt anyone ever touches those XML's anyway, i just leave them out
	FOR NOW! */
}

//======================================================================================================
//					CvSlideShowInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSlideShowInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSlideShowInfo::CvSlideShowInfo() :
m_fStartTime(0.0f)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSlideShowInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSlideShowInfo::~CvSlideShowInfo()
{
}

const TCHAR* CvSlideShowInfo::getPath()
{
	return m_szPath;
}

void CvSlideShowInfo::setPath(const TCHAR* szVal)
{
	m_szPath = szVal;
}

const TCHAR* CvSlideShowInfo::getTransitionType()
{
	return m_szTransitionType;
}

void CvSlideShowInfo::setTransitionType(const TCHAR* szVal)
{
	m_szTransitionType = szVal;
}

float CvSlideShowInfo::getStartTime()
{
	return m_fStartTime;
}

void CvSlideShowInfo::setStartTime(float fVal)
{
	m_fStartTime = fVal;
}

bool CvSlideShowInfo::read(CvXMLLoadUtility* pXML)
{
	float fVal;
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"Path");
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 10/27/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	CvXMLLoadUtilityModTools* p_aszDirName = new CvXMLLoadUtilityModTools;
	p_aszDirName->setLocationName( &szTextVal, (GC.getModDir()).c_str());
	SAFE_DELETE(p_aszDirName);
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/
	setPath(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, L"TransitionType");
	setTransitionType(szTextVal);
	pXML->GetChildXmlValByName(&fVal, L"fStartTime");
	setStartTime(fVal);

	return true;
}
void CvSlideShowInfo::copyNonDefaults(CvSlideShowInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getPath() == cDefault) setPath(pClassInfo->getPath());
	if (getTransitionType() == cDefault) setTransitionType(pClassInfo->getTransitionType());
	if (getStartTime() == fDefault) setStartTime(pClassInfo->getStartTime());
}

//======================================================================================================
//					CvSlideShowRandomInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSlideShowRandomInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSlideShowRandomInfo::CvSlideShowRandomInfo()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSlideShowRandomInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSlideShowRandomInfo::~CvSlideShowRandomInfo()
{
}

const TCHAR* CvSlideShowRandomInfo::getPath()
{
	return m_szPath;
}

void CvSlideShowRandomInfo::setPath(const TCHAR* szVal)
{
	m_szPath = szVal;
}

bool CvSlideShowRandomInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"Path");
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 10/27/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	CvXMLLoadUtilityModTools* p_aszDirName = new CvXMLLoadUtilityModTools;
	p_aszDirName->setLocationName( &szTextVal, (GC.getModDir()).c_str());
	SAFE_DELETE(p_aszDirName);
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/
	setPath(szTextVal);

	return true;
}
void CvSlideShowRandomInfo::copyNonDefaults(CvSlideShowRandomInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getPath() == cDefault) setPath(pClassInfo->getPath());
}

//======================================================================================================
//					CvWorldPickerInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvWorldPickerInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvWorldPickerInfo::CvWorldPickerInfo()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvWorldPickerInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvWorldPickerInfo::~CvWorldPickerInfo()
{
}

const TCHAR* CvWorldPickerInfo::getMapName()
{
	return m_szMapName;
}

void CvWorldPickerInfo::setMapName(const TCHAR* szVal)
{
	m_szMapName = szVal;
}

const TCHAR* CvWorldPickerInfo::getModelFile()
{
	return m_szModelFile;
}

void CvWorldPickerInfo::setModelFile(const TCHAR* szVal)
{
	m_szModelFile = szVal;
}

int CvWorldPickerInfo::getNumSizes()
{
	return m_aSizes.size();
}

float CvWorldPickerInfo::getSize(int index)
{
	return m_aSizes[index];
}

int CvWorldPickerInfo::getNumClimates()
{
	return m_aClimates.size();
}

const TCHAR* CvWorldPickerInfo::getClimatePath(int index)
{
	return m_aClimates[index];
}

int CvWorldPickerInfo::getNumWaterLevelDecals()
{
	return m_aWaterLevelDecals.size();
}

const TCHAR* CvWorldPickerInfo::getWaterLevelDecalPath(int index)
{
	return m_aWaterLevelDecals[index];
}

int CvWorldPickerInfo::getNumWaterLevelGloss()
{
	return m_aWaterLevelGloss.size();
}

const TCHAR* CvWorldPickerInfo::getWaterLevelGlossPath(int index)
{
	return m_aWaterLevelGloss[index];
}

bool CvWorldPickerInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	float fVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"MapName");
	setMapName(szTextVal);
	pXML->GetChildXmlValByName(szTextVal, L"ModelFile");
	setModelFile(szTextVal);

	//sizes
	if(pXML->TryMoveToXmlFirstChild(L"Sizes"))
	{
		if(pXML->TryMoveToXmlFirstChild(L"Size"))
		{
			do
			{
				pXML->GetXmlVal(&fVal);
				m_aSizes.push_back(fVal);
			} while(pXML->TryMoveToXmlNextSibling(L"Size"));

			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	//climates
	if(pXML->TryMoveToXmlFirstChild(L"Climates"))
	{
		if(pXML->TryMoveToXmlFirstChild(L"ClimatePath"))
		{
			do
			{
				pXML->GetXmlVal(szTextVal);
				m_aClimates.push_back(szTextVal);
			} while(pXML->TryMoveToXmlNextSibling(L"ClimatePath"));

			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	//water level decals
	if(pXML->TryMoveToXmlFirstChild(L"WaterLevelDecals"))
	{
		if(pXML->TryMoveToXmlFirstChild(L"WaterLevelDecalPath"))
		{
			do
			{
				pXML->GetXmlVal(szTextVal);
				m_aWaterLevelDecals.push_back(szTextVal);
			} while(pXML->TryMoveToXmlNextSibling(L"WaterLevelDecalPath"));

			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	//water level gloss
	if(pXML->TryMoveToXmlFirstChild(L"WaterLevelGloss"))
	{
		if(pXML->TryMoveToXmlFirstChild(L"WaterLevelGlossPath"))
		{
			do
			{
				pXML->GetXmlVal(szTextVal);
				m_aWaterLevelGloss.push_back(szTextVal);
			} while(pXML->TryMoveToXmlNextSibling(L"WaterLevelGlossPath"));

			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	return true;
}
void CvWorldPickerInfo::copyNonDefaults(CvWorldPickerInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getMapName() == cDefault) setMapName(pClassInfo->getMapName());
	if (getModelFile() == cDefault) setModelFile(pClassInfo->getModelFile());

	if ( getNumSizes() == 0 )
	{
		for ( int i = 0; i < pClassInfo->getNumSizes(); i++ )
		{
			m_aSizes.push_back(pClassInfo->getSize(i));
		}
	}

	if ( getNumClimates() == 0 )
	{
		for ( int i = 0; i < pClassInfo->getNumClimates(); i++ )
		{
			m_aClimates.push_back(pClassInfo->getClimatePath(i));
		}
	}

	if ( getNumWaterLevelDecals() == 0 )
	{
		for ( int i = 0; i < pClassInfo->getNumWaterLevelDecals(); i++ )
		{
			m_aWaterLevelDecals.push_back(pClassInfo->getWaterLevelDecalPath(i));
		}
	}

	if ( getNumWaterLevelGloss() == 0 )
	{
		for ( int i = 0; i < pClassInfo->getNumWaterLevelGloss(); i++ )
		{
			m_aWaterLevelDecals.push_back(pClassInfo->getWaterLevelGlossPath(i));
		}
	}
}

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
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	TCHAR	szTempString[1024];				// Extracting text
	int		iCurrentCategory;				// The current category information we are building
	float	fParameter;						// Temporary
	bool	moved;

	pXML->GetChildXmlValByName( &m_bMissionPath, L"bMissionPath" );
	moved = pXML->TryMoveToXmlFirstChild();  FAssert(moved);
	mbstate_t mbs;  mbrlen(NULL, 0 ,&mbs);
	const wchar_t* tmp = pXML->GetXmlFirstText();
	wcsrtombs(szTempString, &tmp, 1023, &mbs); FAssert(tmp == 0);
	moved = pXML->TryMoveToXmlNextSibling();   FAssert(moved);
	moved = pXML->TryMoveToXmlNextSibling();   FAssert(moved);
	do
	{
		if ( pXML->GetOptionalChildXmlValByName( szTempString, L"Category") )
		{
			iCurrentCategory = pXML->GetInfoClass(szTempString);
			fParameter = 0.0f;
		}
		else
		{
			pXML->GetChildXmlValByName( szTempString, L"Operator" );
			iCurrentCategory = GC.getTypesEnum(szTempString);
			iCurrentCategory = ((int)ANIMOP_FIRST) + iCurrentCategory;
			if ( !pXML->GetChildXmlValByName( &fParameter, L"Parameter" ) )
			{
				fParameter = 0.0f;
			}
		}
			m_vctPathDefinition.push_back( std::make_pair(iCurrentCategory, fParameter ));
	}
	while (pXML->TryMoveToXmlNextSibling());
	pXML->MoveToXmlParent();

	return true;
}
/************************************************************************************************/
/* XMLCOPY								 11/19/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvAnimationPathInfo::copyNonDefaults(CvAnimationPathInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

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

//======================================================================================================
//					CvAnimationCategoryInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvAnimationCategoryInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvAnimationCategoryInfo::CvAnimationCategoryInfo()
{
	m_kCategory.second = -7540; // invalid.
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvAnimationCategoryInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvAnimationCategoryInfo::~CvAnimationCategoryInfo()
{
}

int CvAnimationCategoryInfo::getCategoryBaseID( )
{
	return m_kCategory.first;
}

int CvAnimationCategoryInfo::getCategoryDefaultTo( )
{
	if ( m_kCategory.second < -1 )
	{
		// CvXMLLoadUtility *pXML = new CvXMLLoadUtility();
		m_kCategory.second = CvXMLLoadUtility::GetInfoClass( m_szDefaultTo);
	}
	return (int)m_kCategory.second;
}

bool CvAnimationCategoryInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	int		iBaseID;						// Temporary
	pXML->GetChildXmlValByName( m_szDefaultTo, L"DefaultTo");
	pXML->GetChildXmlValByName( &iBaseID, L"BaseID");
	m_kCategory.first = iBaseID;
	return true;
}
/************************************************************************************************/
/* XMLCOPY								 11/19/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvAnimationCategoryInfo::copyNonDefaults(CvAnimationCategoryInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getCategoryBaseID() == iDefault) m_kCategory.first = pClassInfo->getCategoryBaseID();
	if (getCategoryDefaultTo() == iTextDefault) m_kCategory.second = pClassInfo->getCategoryDefaultTo();
}

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
	MEMORY_TRACE_FUNCTION();

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

/************************************************************************************************/
/* XMLCOPY								 11/19/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvEntityEventInfo::copyNonDefaults(CvEntityEventInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

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

/////////////////////////////////////////////////////////////////////////////////////////////
// CvAssetInfoBase
/////////////////////////////////////////////////////////////////////////////////////////////

const TCHAR* CvAssetInfoBase::getTag() const
{
	return getType();
}

void CvAssetInfoBase::setTag(const TCHAR* szDesc)
{
	m_szType = szDesc;
}

const TCHAR* CvAssetInfoBase::getPath() const
{
	return m_szPath;
}

void CvAssetInfoBase::setPath(const TCHAR* szDesc)
{
	m_szPath = szDesc;
}

bool CvAssetInfoBase::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))		// 'tag' is the same as 'type'
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Path");
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 10/26/07							MRGENIE		  */
/*																							  */
/*																							  */
/************************************************************************************************/
	CvXMLLoadUtilityModTools* p_aszDirName = new CvXMLLoadUtilityModTools;
	p_aszDirName->setLocationName( &szTextVal, GC.getModDir().c_str());
	SAFE_DELETE(p_aszDirName);
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/
	setPath(szTextVal);

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoAsset
/////////////////////////////////////////////////////////////////////////////////////////////

const TCHAR* CvArtInfoAsset::getNIF() const
{
	return m_szNIF;
}

const TCHAR* CvArtInfoAsset::getKFM() const
{
	return m_szKFM;
}

void CvArtInfoAsset::setNIF(const TCHAR* szDesc)
{
	m_szNIF = szDesc;
}

void CvArtInfoAsset::setKFM(const TCHAR* szDesc)
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
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 10/26/07							MRGENIE		  */
/*																							  */
/*																							  */
/************************************************************************************************/
	CvXMLLoadUtilityModTools* p_szDirName = new CvXMLLoadUtilityModTools;
	p_szDirName->setLocationName( &szTextVal, (GC.getModDir()).c_str());
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/
	setNIF(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"KFM");// TO DO : maybe we should do something different if there is no KFM tag?
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 10/26/07							MRGENIE		  */
/*																							  */
/*																							  */
/************************************************************************************************/
	p_szDirName->setLocationName( &szTextVal, (GC.getModDir()).c_str());
	SAFE_DELETE(p_szDirName);
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/
	setKFM(szTextVal);


	return true;
}
void CvArtInfoAsset::copyNonDefaults(CvArtInfoAsset* pClassInfo, CvXMLLoadUtility* pXML)
{
	// Empty, for Art files we stick to FULL XML defintions
}

/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoMovie
/////////////////////////////////////////////////////////////////////////////////////////////

bool CvArtInfoMovie::read(CvXMLLoadUtility* pXML)
{
	return CvArtInfoAsset::read(pXML);
}

void CvArtInfoMovie::copyNonDefaults(CvArtInfoMovie* pClassInfo, CvXMLLoadUtility* pXML)
{
	// Empty, for Art files we stick to FULL XML defintions
}

/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoBonus
/////////////////////////////////////////////////////////////////////////////////////////////

bool CvArtInfoBonus::read(CvXMLLoadUtility* pXML)
{
	OutputDebugString("ArtInfoBonus Start Reading");

	if (!CvArtInfoScalableAsset::read(pXML))
	{
		return false;
	}

	OutputDebugString("ArtInfoBonus Proceed Reading");

	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"SHADERNIF");
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 10/26/07							MRGENIE		  */
/*																							  */
/*																							  */
/************************************************************************************************/
	CvXMLLoadUtilityModTools* p_szDirName = new CvXMLLoadUtilityModTools;
	p_szDirName->setLocationName( &szTextVal, (GC.getModDir()).c_str());
	SAFE_DELETE(p_szDirName);
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/

	OutputDebugString("ArtInfoBonus Nif Reading");
	setShaderNIF(szTextVal);

	pXML->GetChildXmlValByName(&m_iFontButtonIndex, L"FontButtonIndex");

	OutputDebugString("ArtInfoBonus Finish Reading");

	return true;
}

void CvArtInfoBonus::copyNonDefaults(CvArtInfoBonus* pClassInfo, CvXMLLoadUtility* pXML)
{
	// Empty, for Art files we stick to FULL XML defintions
}

CvArtInfoBonus::CvArtInfoBonus()
{
	m_iFontButtonIndex = 0;
}

int CvArtInfoBonus::getFontButtonIndex() const
{
	return m_iFontButtonIndex;
}

const TCHAR* CvArtInfoBonus::getShaderNIF() const
{
	return m_szShaderNIF;
}
void CvArtInfoBonus::setShaderNIF(const TCHAR* szDesc)
{
	m_szShaderNIF = szDesc;
}

const CvArtInfoBonus* CvBonusInfo::getArtInfo() const
{
	return ARTFILEMGR.getBonusArtInfo( getArtDefineTag());
}


/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoUnit
/////////////////////////////////////////////////////////////////////////////////////////////

CvArtInfoUnit::CvArtInfoUnit() :
m_fShadowScale(0.0f),
m_iDamageStates(0),
m_bActAsRanged(false),
m_bActAsLand(false),
m_bActAsAir(false),
m_bCombatExempt(false),
m_fTrailWidth(0.0f),
m_fTrailLength(0.0f),
m_fTrailTaper(0.0f),
m_fTrailFadeStartTime(0.0f),
m_fTrailFadeFalloff(0.0f),
m_fBattleDistance(0.0f),
m_fRangedDeathTime(0.0f),
m_fExchangeAngle(0.0f),
m_bSmoothMove(false),
m_fAngleInterRate(FLT_MAX),
m_fBankRate(0),
m_iRunLoopSoundTag(0),
m_iRunEndSoundTag(0),
m_iSelectionSoundScriptId(0),
m_iActionSoundScriptId(0),
m_iPatrolSoundTag(0)
{
}

CvArtInfoUnit::~CvArtInfoUnit()
{
}

bool CvArtInfoUnit::getActAsRanged() const
{
	return m_bActAsRanged;
}

bool CvArtInfoUnit::getActAsLand() const
{
	return m_bActAsLand;
}

bool CvArtInfoUnit::getActAsAir() const
{
	return m_bActAsAir;
}

const TCHAR* CvArtInfoUnit::getShaderNIF() const
{
	return m_szShaderNIF;
}

void CvArtInfoUnit::setShaderNIF(const TCHAR* szDesc)
{
	m_szShaderNIF = szDesc;
}

const TCHAR* CvArtInfoUnit::getShadowNIF() const
{
	return m_szShadowNIF;
}

float CvArtInfoUnit::getShadowScale() const
{
	return m_fShadowScale;
}

const TCHAR* CvArtInfoUnit::getShadowAttachNode() const
{
	return m_szShadowAttach;
}

int CvArtInfoUnit::getDamageStates() const
{
	return m_iDamageStates;
}


const TCHAR* CvArtInfoUnit::getTrailTexture() const
{
	return m_szTrailTexture;
}

float CvArtInfoUnit::getTrailWidth() const
{
	return m_fTrailWidth;
}

float CvArtInfoUnit::getTrailLength() const
{
	return m_fTrailLength;
}

float CvArtInfoUnit::getTrailTaper() const
{
	return m_fTrailTaper;
}

float CvArtInfoUnit::getTrailFadeStarTime() const
{
	return m_fTrailFadeStartTime;
}

float CvArtInfoUnit::getTrailFadeFalloff() const
{
	return m_fTrailFadeFalloff;
}

float CvArtInfoUnit::getBattleDistance() const
{
	return m_fBattleDistance;
}

float CvArtInfoUnit::getRangedDeathTime() const
{
	return m_fRangedDeathTime;
}

float CvArtInfoUnit::getExchangeAngle() const
{
	return m_fExchangeAngle;
}

bool CvArtInfoUnit::getCombatExempt() const
{
	return m_bCombatExempt;
}

bool CvArtInfoUnit::getSmoothMove() const
{
	return m_bSmoothMove;
}

float CvArtInfoUnit::getAngleInterpRate() const
{
	return m_fAngleInterRate;
}

float CvArtInfoUnit::getBankRate() const
{
	return m_fBankRate;
}

bool CvArtInfoUnit::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvArtInfoScalableAsset::read(pXML))
	{
		return false;
	}

	CvString szTextVal;

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ActionSound");
	m_iActionSoundScriptId = (szTextVal.GetLength() > 0) ? gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_3DSCRIPT ) : -1;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"SelectionSound");
	m_iSelectionSoundScriptId = (szTextVal.GetLength() > 0) ? gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_3DSCRIPT ) : -1;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PatrolSound");
	m_iPatrolSoundTag = (szTextVal.GetLength() > 0) ? gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_3DSCRIPT ) : -1;

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TrainSound");
	setTrainSound(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_bActAsRanged, L"bActAsRanged" );
	pXML->GetOptionalChildXmlValByName(&m_bActAsLand,   L"bActAsLand" );
	pXML->GetOptionalChildXmlValByName(&m_bActAsAir,	L"bActAsAir" );
	pXML->GetOptionalChildXmlValByName(&m_bCombatExempt,   L"bCombatExempt");
	pXML->GetOptionalChildXmlValByName(&m_fExchangeAngle,  L"fExchangeAngle");
	pXML->GetOptionalChildXmlValByName(&m_bSmoothMove,	 L"bSmoothMove");
	pXML->GetOptionalChildXmlValByName(&m_fAngleInterRate, L"fAngleInterpRate", FLT_MAX );
	pXML->GetOptionalChildXmlValByName(&m_fBankRate,	   L"fBankRate");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SHADERNIF");
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 10/26/07							MRGENIE		  */
/*																							  */
/*																							  */
/************************************************************************************************/
	CvXMLLoadUtilityModTools* p_szDirName = new CvXMLLoadUtilityModTools;
	p_szDirName->setLocationName( &szTextVal, (GC.getModDir()).c_str());
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/
	setShaderNIF(szTextVal);

	if ( pXML->TryMoveToXmlFirstChild(L"ShadowDef" ))
	{
		pXML->GetOptionalChildXmlValByName( m_szShadowAttach, L"ShadowAttachNode" );
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 10/26/07							MRGENIE		  */
/*																							  */
/*																							  */
/************************************************************************************************/
		p_szDirName->setLocationName( &m_szShadowAttach, (GC.getModDir()).c_str());
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/
		pXML->GetOptionalChildXmlValByName(m_szShadowNIF, L"ShadowNIF");
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 10/26/07							MRGENIE		  */
/*																							  */
/*																							  */
/************************************************************************************************/
		p_szDirName->setLocationName( &m_szShadowNIF, (GC.getModDir()).c_str());
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/
		pXML->GetOptionalChildXmlValByName(&m_fShadowScale, L"fShadowScale");
		pXML->MoveToXmlParent();
	}

	pXML->GetOptionalChildXmlValByName(&m_iDamageStates,	L"iDamageStates");
	pXML->GetOptionalChildXmlValByName(&m_fBattleDistance,  L"fBattleDistance");
	pXML->GetOptionalChildXmlValByName(&m_fRangedDeathTime, L"fRangedDeathTime");

	m_fTrailWidth = -1.0f; // invalid.
	if (pXML->TryMoveToXmlFirstChild(L"TrailDefinition"))
	{
		pXML->GetChildXmlValByName(m_szTrailTexture, L"Texture" );
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 10/26/07							MRGENIE		  */
/*																							  */
/*																							  */
/************************************************************************************************/
		p_szDirName->setLocationName( &m_szTrailTexture, (GC.getModDir()).c_str());
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/
		pXML->GetOptionalChildXmlValByName(&m_fTrailWidth,  L"fWidth", -1.0f);
		pXML->GetOptionalChildXmlValByName(&m_fTrailLength, L"fLength" );
		pXML->GetOptionalChildXmlValByName(&m_fTrailTaper,  L"fTaper" );
		pXML->GetOptionalChildXmlValByName(&m_fTrailFadeStartTime, L"fFadeStartTime" );
		pXML->GetOptionalChildXmlValByName(&m_fTrailFadeFalloff,   L"fFadeFalloff" );
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"AudioRunSounds"))
	{
		pXML->GetOptionalChildXmlValByName(szTextVal, L"AudioRunTypeLoop");
		m_iRunLoopSoundTag = GC.getFootstepAudioTypeByTag(szTextVal);
		pXML->GetOptionalChildXmlValByName(szTextVal, L"AudioRunTypeEnd");
		m_iRunEndSoundTag = GC.getFootstepAudioTypeByTag(szTextVal);
		pXML->MoveToXmlParent();
	}
	SAFE_DELETE(p_szDirName);
	return true;
}

void CvArtInfoUnit::copyNonDefaults(CvArtInfoUnit* pClassInfo, CvXMLLoadUtility* pXML)
{
	// Empty, for Art files we stick to FULL XML defintions
}

const TCHAR* CvArtInfoUnit::getTrainSound() const
{
	return m_szTrainSound;
}

void CvArtInfoUnit::setTrainSound(const TCHAR* szVal)
{
	m_szTrainSound = szVal;
}

int CvArtInfoUnit::getRunLoopSoundTag() const
{
	return m_iRunLoopSoundTag;
}

int CvArtInfoUnit::getRunEndSoundTag() const
{
	return m_iRunEndSoundTag;
}

int CvArtInfoUnit::getPatrolSoundTag() const
{
	return m_iPatrolSoundTag;
}

int CvArtInfoUnit::getSelectionSoundScriptId() const
{
	return m_iSelectionSoundScriptId;
}

int CvArtInfoUnit::getActionSoundScriptId() const
{
	return m_iActionSoundScriptId;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoBuilding
/////////////////////////////////////////////////////////////////////////////////////////////

CvArtInfoBuilding::CvArtInfoBuilding() :
m_bAnimated(false)
{
}

CvArtInfoBuilding::~CvArtInfoBuilding()
{
}

bool CvArtInfoBuilding::isAnimated() const
{
	return m_bAnimated;
}

const TCHAR* CvArtInfoBuilding::getLSystemName() const
{
	return m_szLSystemName;
}

bool CvArtInfoBuilding::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvArtInfoScalableAsset::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szLSystemName, L"LSystem");

	pXML->GetOptionalChildXmlValByName(&m_bAnimated, L"bAnimated");
	return true;
}
void CvArtInfoBuilding::copyNonDefaults(CvArtInfoBuilding* pClassInfo, CvXMLLoadUtility* pXML)
{
	// Empty, for Art files we stick to FULL XML defintions
}

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

void CvArtInfoCivilization::copyNonDefaults(CvArtInfoCivilization* pClassInfo, CvXMLLoadUtility* pXML)
{
	// Empty, for Art files we stick to FULL XML defintions
}

/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoLeaderhead
/////////////////////////////////////////////////////////////////////////////////////////////

const TCHAR* CvArtInfoLeaderhead::getNoShaderNIF() const
{
	return m_szNoShaderNIF;
}

void CvArtInfoLeaderhead::setNoShaderNIF(const TCHAR* szNIF)
{
	m_szNoShaderNIF = szNIF;
}

const TCHAR* CvArtInfoLeaderhead::getBackgroundKFM() const
{
	return m_szBackgroundKFM;
}

void CvArtInfoLeaderhead::setBackgroundKFM( const TCHAR* szKFM)
{
	m_szBackgroundKFM = szKFM;
}

bool CvArtInfoLeaderhead::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvArtInfoAsset::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"NoShaderNIF");

	CvXMLLoadUtilityModTools aszDirName;
	aszDirName.setLocationName( &szTextVal, (GC.getModDir()).c_str());

	setNoShaderNIF(szTextVal);

	if (pXML->GetOptionalChildXmlValByName(szTextVal, L"BackgroundKFM"))
	{
		// AIAndy: This used a deleted pointer, changed
		aszDirName.setLocationName( &szTextVal, (GC.getModDir()).c_str());
		setBackgroundKFM(szTextVal);
	}
	else
		setBackgroundKFM("");

	return true;
}

void CvArtInfoLeaderhead::copyNonDefaults(CvArtInfoLeaderhead* pClassInfo, CvXMLLoadUtility* pXML)
{
	// Empty, for Art files we stick to FULL XML defintions
}

/////////////////////////////////////////////////////////////////////////////////////////////
// CvArtInfoScalableAsset
/////////////////////////////////////////////////////////////////////////////////////////////

bool CvArtInfoScalableAsset::read(CvXMLLoadUtility* pXML)
{
	if (!CvArtInfoAsset::read(pXML))
	{
		return false;
	}

	return CvScalableInfo::read(pXML);
}

void CvArtInfoScalableAsset::copyNonDefaults(CvArtInfoScalableAsset* pClassInfo, CvXMLLoadUtility* pXML)
{
	// Empty, for Art files we stick to FULL XML defintions
}

//////////////////////////////////////////////////////////////////////////
// CvArtInfoImprovement
//////////////////////////////////////////////////////////////////////////

CvArtInfoImprovement::CvArtInfoImprovement() :
m_bExtraAnimations(false)
{
}

CvArtInfoImprovement::~CvArtInfoImprovement()
{
}

bool CvArtInfoImprovement::isExtraAnimations() const
{
	return m_bExtraAnimations;
}

bool CvArtInfoImprovement::read(CvXMLLoadUtility* pXML)
{
	if (!CvArtInfoScalableAsset::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_bExtraAnimations, L"bExtraAnimations");

	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"SHADERNIF", "");
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 10/26/07							MRGENIE		  */
/*																							  */
/*																							  */
/************************************************************************************************/
	CvXMLLoadUtilityModTools* p_szDirName = new CvXMLLoadUtilityModTools;
	p_szDirName->setLocationName( &szTextVal, (GC.getModDir()).c_str());
	SAFE_DELETE(p_szDirName);
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/
	setShaderNIF(szTextVal);

	return true;
}

void CvArtInfoImprovement::copyNonDefaults(CvArtInfoImprovement* pClassInfo, CvXMLLoadUtility* pXML)
{
	// Empty, for Art files we stick to FULL XML defintions
}

//////////////////////////////////////////////////////////////////////////
// CvArtInfoTerrain
//////////////////////////////////////////////////////////////////////////

CvArtInfoTerrain::CvArtInfoTerrain() :
m_iLayerOrder(0),
m_bAlphaShader(false),
m_numTextureBlends(16)
{
	m_pTextureSlots = new CvTextureBlendSlotList * [m_numTextureBlends];
	m_pSlotNames = new CvString * [m_numTextureBlends];
	for ( int i = 0; i < m_numTextureBlends; i++ )
	{
		m_pTextureSlots[i] = new CvTextureBlendSlotList;
		m_pSlotNames[i] = NULL;
	}

}

CvArtInfoTerrain::~CvArtInfoTerrain()
{
	for ( int i = 0; i < m_numTextureBlends; i++ )
	{
		SAFE_DELETE(m_pTextureSlots[i]);
	}
	SAFE_DELETE_ARRAY( m_pTextureSlots);
}

const TCHAR* CvArtInfoTerrain::getBaseTexture()
{
	return getPath();
}

void CvArtInfoTerrain::setBaseTexture(const TCHAR* szTmp )
{
	setPath(szTmp);
}

const TCHAR* CvArtInfoTerrain::getGridTexture()
{
	return m_szGridTexture;
}

void CvArtInfoTerrain::setGridTexture(const TCHAR* szTmp )
{
	m_szGridTexture = szTmp;
}

const TCHAR* CvArtInfoTerrain::getDetailTexture()
{
	return m_szDetailTexture;
}

void CvArtInfoTerrain::setDetailTexture(const TCHAR* szTmp)
{
	m_szDetailTexture = szTmp;
}

int CvArtInfoTerrain::getLayerOrder()
{
	return m_iLayerOrder;
}

bool CvArtInfoTerrain::useAlphaShader()
{
	return m_bAlphaShader;
}

CvTextureBlendSlotList &CvArtInfoTerrain::getBlendList(int blendMask)
{
	FAssert(blendMask>0 && blendMask<16);
	return *m_pTextureSlots[blendMask];
}

void BuildSlotList( CvTextureBlendSlotList &list, CvString &numlist)
{
	//convert string to
	char seps[]   = " ,\t\n";
	char *token;
	const char *numstring = numlist;
	// AIAndy: This is really, really dirty as strtok does change the string so it will alter the string representation in CvString which is not supposed to be changed from outside CvString
	// Given the black terrain issue I change that to copying the string first
	//token = strtok( const_cast<char *>(numstring), seps);
	char* szList = new char[numlist.length()+1];
	strcpy(szList, numstring);
	token = strtok( szList, seps);
	while( token != NULL )
	{
		int slot = atoi(token);
		token = strtok( NULL, seps);
		int rotation = atoi(token);
		list.push_back(std::make_pair( slot, rotation));
		token = strtok( NULL, seps);
	}
	delete[] szList;
}

bool CvArtInfoTerrain::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvArtInfoAsset::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"Grid");

	// AIAndy: No need to do heap allocation for this
	CvXMLLoadUtilityModTools aszDirName;
	aszDirName.setLocationName( &szTextVal, (GC.getModDir()).c_str());

	setGridTexture(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, L"Detail");

	// AIAndy: This used a deleted pointer. As the ModTools class does not have any data members or virtual methods, it will likely still work, but it might still cause trouble.
	aszDirName.setLocationName( &szTextVal, (GC.getModDir()).c_str());

	setDetailTexture(szTextVal);

	pXML->GetChildXmlValByName(&m_iLayerOrder, L"LayerOrder");
	pXML->GetChildXmlValByName(&m_bAlphaShader, L"AlphaShader");

	// Parse texture slots for blend tile lists
	wchar_t xmlName[] = L"TextureBlend00";
	for(int i =1; i<m_numTextureBlends;i++ )
	{
		swprintf(xmlName+(wcslen(xmlName)-2),L"%02d",i);
		pXML->GetChildXmlValByName(szTextVal, xmlName);

		aszDirName.setLocationName( &szTextVal, (GC.getModDir()).c_str());

		m_pSlotNames[i] = new CvString(szTextVal);
		BuildSlotList(*m_pTextureSlots[i], szTextVal);
	}

	// AIAndy: Reading CvArtInfoAsset again is bad
	// return CvArtInfoAsset::read(pXML);
	return true;
}

void CvArtInfoTerrain::copyNonDefaults(CvArtInfoTerrain* pClassInfo, CvXMLLoadUtility* pXML)
{
	// Empty, for Art files we stick to FULL XML defintions
}

//////////////////////////////////////////////////////////////////////////
// CvArtInfoFeature
//////////////////////////////////////////////////////////////////////////

CvArtInfoFeature::CvArtInfoFeature() :
m_bAnimated(false),
m_bRiverArt(false),
m_eTileArtType(TILE_ART_TYPE_NONE),
m_eLightType(LIGHT_TYPE_NONE)
{
}

CvArtInfoFeature::~CvArtInfoFeature()
{
}

bool CvArtInfoFeature::isAnimated() const
{
	return m_bAnimated;
}

bool CvArtInfoFeature::isRiverArt() const
{
	return m_bRiverArt;
}

TileArtTypes CvArtInfoFeature::getTileArtType() const
{
	return m_eTileArtType;
}

LightTypes CvArtInfoFeature::getLightType() const
{
	return m_eLightType;
}

bool CvArtInfoFeature::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvArtInfoScalableAsset::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_bAnimated, L"bAnimated");
	pXML->GetOptionalChildXmlValByName(&m_bRiverArt, L"bRiverArt");

	CvString szTemp;
	pXML->GetOptionalChildXmlValByName(szTemp, L"TileArtType", "TILE_ART_TYPE_NONE");
	if(szTemp.CompareNoCase("TILE_ART_TYPE_NONE") == 0)
		m_eTileArtType = TILE_ART_TYPE_NONE;
	else if(szTemp.CompareNoCase("TILE_ART_TYPE_TREES") == 0)
		m_eTileArtType = TILE_ART_TYPE_TREES;
	else if(szTemp.CompareNoCase("TILE_ART_TYPE_HALF_TILING") == 0)
		m_eTileArtType = TILE_ART_TYPE_HALF_TILING;
	else if(szTemp.CompareNoCase("TILE_ART_TYPE_PLOT_TILING") == 0)
		m_eTileArtType = TILE_ART_TYPE_PLOT_TILING;
	else
	{
		FAssertMsg(false, "[Jason] Unknown TileArtType.");
	}

	pXML->GetOptionalChildXmlValByName(szTemp, L"LightType", "LIGHT_TYPE_NONE");
	if(szTemp.CompareNoCase("LIGHT_TYPE_NONE") == 0)
		m_eLightType = LIGHT_TYPE_NONE;
	else if(szTemp.CompareNoCase("LIGHT_TYPE_SUN") == 0)
		m_eLightType = LIGHT_TYPE_SUN;
	else if(szTemp.CompareNoCase("LIGHT_TYPE_TERRAIN") == 0)
		m_eLightType = LIGHT_TYPE_TERRAIN;
	else if(szTemp.CompareNoCase("LIGHT_TYPE_UNIT") == 0)
		m_eLightType = LIGHT_TYPE_UNIT;
	else
	{
		FAssertMsg(false, "[Jason] Unknown LightType.");
	}

	//feature varieties
	if(pXML->TryMoveToXmlFirstChild(L"FeatureVariety"))
	{
		do
		{
			m_aFeatureVarieties.push_back(FeatureVariety());
			FeatureVariety &featureVariety = m_aFeatureVarieties.back();

			//generate rotations
			bool bGenerateRotations = false;
			pXML->GetOptionalChildXmlValByName(&bGenerateRotations, L"bGenerateRotations");

			//feature art pieces
			if(pXML->TryMoveToXmlFirstChild(L"FeatureArtPieces"))
			{
				if(pXML->TryMoveToXmlFirstChild(L"FeatureArtPiece"))
				{
					do
					{
						//connection mask
						pXML->GetOptionalChildXmlValByName(szTemp, L"Connections");
						int connectionMask = getConnectionMaskFromString(szTemp);

						//model files
						if(pXML->TryMoveToXmlFirstChild(L"ModelFile"))
						{
							do
							{
								pXML->GetXmlVal(szTemp);
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 04/01/08								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
								CvXMLLoadUtilityModTools* p_szDirName = new CvXMLLoadUtilityModTools;
								p_szDirName->setLocationName( &szTemp, (GC.getModDir()).c_str());
								SAFE_DELETE(p_szDirName);
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/
								if(!bGenerateRotations)
								{
									FeatureArtPiece &featureArtPiece = featureVariety.createFeatureArtPieceFromConnectionMask(connectionMask);
									featureArtPiece.m_aArtModels.push_back(FeatureArtModel(szTemp, ROTATE_NONE));
								}
								else
								{
									for(int i=0;i<NUM_ROTATION_TYPES;i++)
									{
										int newConnectionMask = getRotatedConnectionMask(connectionMask, (RotationTypes) i);
										FeatureArtPiece &featureArtPiece = featureVariety.createFeatureArtPieceFromConnectionMask(newConnectionMask);
										featureArtPiece.m_aArtModels.push_back(FeatureArtModel(szTemp, (RotationTypes) i));
									}
								}
							} while(pXML->TryMoveToXmlNextSibling(L"ModelFile"));

							pXML->MoveToXmlParent();
						}
					} while(pXML->TryMoveToXmlNextSibling(L"FeatureArtPiece"));

					pXML->MoveToXmlParent();
				}

				pXML->MoveToXmlParent();
			}

			//feature art pieces
			if(pXML->TryMoveToXmlFirstChild(L"FeatureDummyNodes"))
			{
				if(pXML->TryMoveToXmlFirstChild(L"FeatureDummyNode"))
				{
					do
					{
						CvString tagName;
						CvString nodeName;

						//tag name
						pXML->GetChildXmlValByName(tagName, L"Tag");

						//node name
						pXML->GetChildXmlValByName(nodeName, L"Name");

						featureVariety.createFeatureDummyNode(tagName, nodeName);
					} while(pXML->TryMoveToXmlNextSibling(L"FeatureDummyNode"));

					pXML->MoveToXmlParent();
				}

				pXML->MoveToXmlParent();
			}

			//variety button
			pXML->GetChildXmlValByName(featureVariety.m_szVarietyButton, L"VarietyButton");
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 04/01/08								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
			CvXMLLoadUtilityModTools* p_szDirName = new CvXMLLoadUtilityModTools;
			p_szDirName->setLocationName(&featureVariety.m_szVarietyButton, (GC.getModDir()).c_str());
			SAFE_DELETE(p_szDirName);
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/

		} while(pXML->TryMoveToXmlNextSibling(L"FeatureVariety"));

		pXML->MoveToXmlParent();
	}

	return true;
}

void CvArtInfoFeature::dump()
{
	OutputDebugString(CvString::format("CvArtInfoFeature: %s:\n", m_szType.c_str()).c_str());
	int iNum = m_aFeatureVarieties.size();
	OutputDebugString(CvString::format("\t%d varieties:\n", iNum).c_str());
	for(int i = 0; i < iNum; i++)
	{
		FeatureVariety &featureVariety = m_aFeatureVarieties[i];

		//feature art pieces
		int iNumArtPieces = featureVariety.m_aFeatureArtPieces.size();
		OutputDebugString(CvString::format("\t\t%d art pieces:\n", iNumArtPieces).c_str());
		for(int j = 0; j < iNumArtPieces; j++)
		{
			CvArtInfoFeature::FeatureArtPiece& kArtPiece = featureVariety.m_aFeatureArtPieces[j];
			//connection mask
			OutputDebugString(CvString::format("\t\t\tConnection mask: %d\n", kArtPiece.getConnectionMask()).c_str());

			//model files
			int iNumModelFiles = kArtPiece.m_aArtModels.size();
			OutputDebugString(CvString::format("\t\t\t%d art models:\n", iNumModelFiles).c_str());
			for(int k = 0; k < iNumModelFiles; k++)
			{
				FeatureArtModel kArtModel = kArtPiece.m_aArtModels[k];

				OutputDebugString(CvString::format("\t\t\t\t%Rotation %d, file: %s\n", kArtModel.getRotation(), kArtModel.getModelFile().c_str()).c_str());
			}
		}

		//feature dummy nodes
		int iNumDummyNodes = featureVariety.m_aFeatureDummyNodes.size();
		OutputDebugString(CvString::format("\t\t%d dummy nodes:\n", iNumDummyNodes).c_str());
		for(int j = 0; j < iNumDummyNodes; j++)
		{
			FeatureDummyNode&	kNode = featureVariety.m_aFeatureDummyNodes[j];

			OutputDebugString(CvString::format("\t\t\tTag %s, name %s\n", kNode.getTagName().c_str(), kNode.getNodeName().c_str()).c_str());
		}

		//variety button
		OutputDebugString(CvString::format("\t\t%Variety button: %s\n", featureVariety.m_szVarietyButton.c_str()).c_str());
	}
}

void CvArtInfoFeature::copyNonDefaults(CvArtInfoFeature* pClassInfo, CvXMLLoadUtility* pXML)
{
	// Empty, for Art files we stick to FULL XML defintions
}

const CvArtInfoFeature::FeatureVariety &CvArtInfoFeature::getVariety(int index) const
{
	//FAssertMsg((index >= 0) && (index < (int) m_aFeatureVarieties.size()), "[Jason] Feature Variety index out of range.");
	//AIAndy: limit to valid range
	int iIndex = range(index, 0, getNumVarieties() - 1);
	return m_aFeatureVarieties[iIndex];
}

int CvArtInfoFeature::getNumVarieties() const
{
	return m_aFeatureVarieties.size();
}

std::string CvArtInfoFeature::getFeatureDummyNodeName(int variety, std::string tagName)
{
	return getVariety(variety).getFeatureDummyNodeName(tagName);
}

int CvArtInfoFeature::getConnectionMaskFromString(const CvString &connectionString)
{
	if(connectionString.IsEmpty())
		return 0;
	else
	{
		std::vector<CvString> tokens;
		connectionString.getTokens(" \t\n", tokens);

		int connectionMask = 0;
		for(int i=0;i<(int)tokens.size();i++)
		{
			// found a token, parse it.
			CvString &token = tokens[i];
			if(token.CompareNoCase("NW") == 0)
				connectionMask |= DIRECTION_NORTHWEST_MASK;
			else if(token.CompareNoCase("N") == 0)
				connectionMask |= DIRECTION_NORTH_MASK;
			else if(token.CompareNoCase("NE") == 0)
				connectionMask |= DIRECTION_NORTHEAST_MASK;
			else if(token.CompareNoCase("E") == 0)
				connectionMask |= DIRECTION_EAST_MASK;
			else if(token.CompareNoCase("SE") == 0)
				connectionMask |= DIRECTION_SOUTHEAST_MASK;
			else if(token.CompareNoCase("S") == 0)
				connectionMask |= DIRECTION_SOUTH_MASK;
			else if(token.CompareNoCase("SW") == 0)
				connectionMask |= DIRECTION_SOUTHWEST_MASK;
			else if(token.CompareNoCase("W") == 0)
				connectionMask |= DIRECTION_WEST_MASK;
			else
			{
				FAssertMsg(false, "[Jason] Invalid connection direction.");
			}
		}

		FAssertMsg(connectionMask > 0, "[Jason] Did not find feature connection mask.");
		return connectionMask;
	}
}

int CvArtInfoFeature::getRotatedConnectionMask(int connectionMask, RotationTypes rotation)
{
	if(rotation == ROTATE_90CW)
		connectionMask = connectionMask << 2; //rotate two directions CW
	else if(rotation == ROTATE_180CW)
		connectionMask = connectionMask << 4; //rotate four directions CW
	else if(rotation == ROTATE_270CW)
		connectionMask = connectionMask << 6; //rotate six directions CW

	//renormalize directions that wrapped around
	connectionMask = connectionMask | (connectionMask >> 8);
	connectionMask = connectionMask & 255;
	return connectionMask;
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvColorInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvColorInfo::CvColorInfo()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvColorInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvColorInfo::~CvColorInfo()
{
}

const NiColorA& CvColorInfo::getColor() const
{
	return m_Color;
}

bool CvColorInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	float afColorVals[4];	// array to hold the 4 color values, red, green, blue, and alpha

	pXML->GetChildXmlValByName(&afColorVals[0], L"fRed");
	pXML->GetChildXmlValByName(&afColorVals[1], L"fGreen");
	pXML->GetChildXmlValByName(&afColorVals[2], L"fBlue");
	pXML->GetChildXmlValByName(&afColorVals[3], L"fAlpha");

	m_Color = NiColorA(afColorVals[0], afColorVals[1], afColorVals[2], afColorVals[3]);

	return true;
}

/************************************************************************************************/
/* XMLCOPY								 11/19/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvColorInfo::copyNonDefaults(CvColorInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

//	if (afColorVals[0] == fDefault && afColorVals[1] == fDefault && afColorVals[2] == fDefault
//		 && afColorVals[3] == fDefault) m_Color = pClassInfo->getColor();
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvPlayerColorInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvPlayerColorInfo::CvPlayerColorInfo() :
m_iColorTypePrimary(NO_COLOR),
m_iColorTypeSecondary(NO_COLOR),
m_iTextColorType(NO_COLOR)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvPlayerColorInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvPlayerColorInfo::~CvPlayerColorInfo()
{
}

int CvPlayerColorInfo::getColorTypePrimary() const
{
	return m_iColorTypePrimary;
}

int CvPlayerColorInfo::getColorTypeSecondary() const
{
	return m_iColorTypeSecondary;
}

int CvPlayerColorInfo::getTextColorType() const
{
	return m_iTextColorType;
}

bool CvPlayerColorInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"ColorTypePrimary");
	m_iColorTypePrimary = pXML->GetInfoClass( szTextVal);

	pXML->GetChildXmlValByName(szTextVal, L"ColorTypeSecondary");
	m_iColorTypeSecondary = pXML->GetInfoClass( szTextVal);

	pXML->GetChildXmlValByName(szTextVal, L"TextColorType");
	m_iTextColorType = pXML->GetInfoClass( szTextVal);

	return true;
}

/************************************************************************************************/
/* XMLCOPY								 11/19/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvPlayerColorInfo::copyNonDefaults(CvPlayerColorInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getColorTypePrimary() == iTextDefault) m_iColorTypePrimary = pClassInfo->getColorTypePrimary();
	if (getColorTypeSecondary() == iTextDefault) m_iColorTypeSecondary = pClassInfo->getColorTypeSecondary();
	if (getTextColorType() == iTextDefault) m_iTextColorType = pClassInfo->getTextColorType();
}

//------------------------------------------------------------------------------------------------------
//
//	CvLandscapeInfo
//
//
CvLandscapeInfo::CvLandscapeInfo() :
m_iFogR(0),
m_iFogG(0),
m_iFogB(0),
m_iHorizontalGameCell(0),
m_iVerticalGameCell(0),
m_iPlotsPerCellX(0),
m_iPlotsPerCellY(0),
m_iHorizontalVertCnt(0),
m_iVerticalVertCnt(0),
m_iWaterHeight(0),
m_fTextureScaleX(0.0f),
m_fTextureScaleY(0.0f),
m_fZScale(0.0f),
m_fPeakScale(0.0f),
m_fHillScale(0.0f),
m_bUseTerrainShader(false),
m_bUseLightmap(false),
m_bRandomMap(false)
{
}

int CvLandscapeInfo::getFogR() const
{
	return m_iFogR;
}

int CvLandscapeInfo::getFogG() const
{
	return m_iFogG;
}

int CvLandscapeInfo::getFogB() const
{
	return m_iFogB;
}

int CvLandscapeInfo::getHorizontalGameCell() const
{
	return m_iHorizontalGameCell;
}

int CvLandscapeInfo::getVerticalGameCell() const
{
	return m_iVerticalGameCell;
}

int CvLandscapeInfo::getPlotsPerCellX() const
{
	return m_iPlotsPerCellX;
}

int CvLandscapeInfo::getPlotsPerCellY() const
{
	return m_iPlotsPerCellY;
}

int CvLandscapeInfo::getHorizontalVertCnt() const
{
	return m_iHorizontalVertCnt;
}

int CvLandscapeInfo::getVerticalVertCnt() const
{
	return m_iVerticalVertCnt;
}

int CvLandscapeInfo::getWaterHeight() const
{
	return m_iWaterHeight;
}

float CvLandscapeInfo::getTextureScaleX() const
{
	return m_fTextureScaleX;
}

float CvLandscapeInfo::getTextureScaleY() const
{
	return m_fTextureScaleY;
}

float CvLandscapeInfo::getZScale() const
{
	return m_fZScale;
}

bool CvLandscapeInfo::isUseTerrainShader() const
{
	return m_bUseTerrainShader;
}

bool CvLandscapeInfo::isUseLightmap() const
{
	return m_bUseLightmap;
}
float CvLandscapeInfo::getPeakScale() const
{
	return 	m_fPeakScale;
}

float CvLandscapeInfo::getHillScale() const
{
	return 	m_fHillScale;
}

bool CvLandscapeInfo::isRandomMap() const
{
	return m_bRandomMap;
}

const TCHAR* CvLandscapeInfo::getSkyArt()
{
	return m_szSkyArt;
}

void CvLandscapeInfo::setSkyArt(const TCHAR* szPath)
{
	m_szSkyArt = szPath;
}

const TCHAR* CvLandscapeInfo::getHeightMap()
{
	return m_szHeightMap;
}

void CvLandscapeInfo::setHeightMap(const TCHAR* szPath)
{
	m_szHeightMap = szPath;
}

const TCHAR* CvLandscapeInfo::getTerrainMap()
{
	return m_szTerrainMap;
}

void CvLandscapeInfo::setTerrainMap(const TCHAR* szPath)
{
	m_szTerrainMap = szPath;
}

const TCHAR* CvLandscapeInfo::getNormalMap()
{
	return m_szNormalMap;
}

void CvLandscapeInfo::setNormalMap(const TCHAR* szPath)
{
	m_szNormalMap = szPath;
}

const TCHAR* CvLandscapeInfo::getBlendMap()
{
	return m_szBlendMap;
}

void CvLandscapeInfo::setBlendMap(const TCHAR* szPath)
{
	m_szBlendMap = szPath;
}

//
// read from xml
//
bool CvLandscapeInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_iWaterHeight, L"iWaterHeight");
	pXML->GetChildXmlValByName(&m_bRandomMap, L"bRandomMap");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"HeightMap");
	setHeightMap(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TerrainMap");
	setTerrainMap(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"NormalMap");
	setNormalMap(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"BlendMap");
	setBlendMap(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SkyArt");
	setSkyArt(szTextVal);

	pXML->GetChildXmlValByName(&m_iFogR, L"iFogR");
	pXML->GetChildXmlValByName(&m_iFogG, L"iFogG");
	pXML->GetChildXmlValByName(&m_iFogB, L"iFogB");

	pXML->GetChildXmlValByName(&m_fTextureScaleX, L"fTextureScaleX");
	pXML->GetChildXmlValByName(&m_fTextureScaleY, L"fTextureScaleY");

	pXML->GetChildXmlValByName(&m_iHorizontalGameCell, L"iGameCellSizeX");
	pXML->GetChildXmlValByName(&m_iVerticalGameCell, L"iGameCellSizeY");

	pXML->GetChildXmlValByName(&m_iPlotsPerCellX, L"iPlotsPerCellX");
	pXML->GetChildXmlValByName(&m_iPlotsPerCellY, L"iPlotsPerCellY");

	m_iHorizontalVertCnt = m_iPlotsPerCellX * m_iHorizontalGameCell - (m_iPlotsPerCellX - 1);
	m_iVerticalVertCnt   = m_iPlotsPerCellY * m_iVerticalGameCell - (m_iPlotsPerCellY - 1);

	pXML->GetChildXmlValByName(&m_fZScale, L"fZScale");
	pXML->GetChildXmlValByName(&m_bUseTerrainShader, L"bTerrainShader");
	pXML->GetChildXmlValByName(&m_bUseLightmap, L"bUseLightmap");
	pXML->GetChildXmlValByName(&m_fPeakScale, L"fPeakScale");
	pXML->GetChildXmlValByName(&m_fHillScale, L"fHillScale");


	return true;
}

void CvLandscapeInfo::copyNonDefaults(CvLandscapeInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getWaterHeight() == iDefault) m_iWaterHeight = pClassInfo->getWaterHeight();
	if (isRandomMap() == bDefault) m_bRandomMap = pClassInfo->isRandomMap();
	if (getHeightMap() == cDefault) setHeightMap(pClassInfo->getHeightMap());
	if (getTerrainMap() == cDefault) setTerrainMap(pClassInfo->getTerrainMap());
	if (getNormalMap() == cDefault) setNormalMap(pClassInfo->getNormalMap());
	if (getBlendMap() == cDefault) setBlendMap(pClassInfo->getBlendMap());
	if (getSkyArt() == cDefault) setSkyArt(pClassInfo->getSkyArt());
	if (getFogR() == iDefault) m_iFogR = pClassInfo->getFogR();
	if (getFogG() == iDefault) m_iFogG = pClassInfo->getFogG();
	if (getFogB() == iDefault) m_iFogB = pClassInfo->getFogB();
	if (getTextureScaleX() == fDefault) m_fTextureScaleX = pClassInfo->getTextureScaleX();
	if (getTextureScaleY() == fDefault) m_fTextureScaleY = pClassInfo->getTextureScaleY();
	if (getHorizontalGameCell() == iDefault) m_iHorizontalGameCell = pClassInfo->getHorizontalGameCell();
	if (getVerticalGameCell() == iDefault) m_iVerticalGameCell = pClassInfo->getVerticalGameCell();
	if (getPlotsPerCellX() == iDefault) m_iPlotsPerCellX = pClassInfo->getPlotsPerCellX();
	if (getPlotsPerCellY() == iDefault) m_iPlotsPerCellY = pClassInfo->getPlotsPerCellY();
	m_iHorizontalVertCnt = m_iPlotsPerCellX * m_iHorizontalGameCell - (m_iPlotsPerCellX - 1);
	m_iVerticalVertCnt   = m_iPlotsPerCellY * m_iVerticalGameCell - (m_iPlotsPerCellY - 1);

	if (getZScale() == fDefault) m_fZScale = pClassInfo->getZScale();
	if (isUseTerrainShader() == bDefault) m_bUseTerrainShader = pClassInfo->isUseTerrainShader();
	if (isUseLightmap() == bDefault) m_bUseLightmap = pClassInfo->isUseLightmap();
	if (getPeakScale() == fDefault) m_fPeakScale = pClassInfo->getPeakScale();
	if (getHillScale() == fDefault) m_fHillScale = pClassInfo->getHillScale();
}


//////////////////////////////////////////////////////////////////////////
//
//	CvEffectInfo			Misc\CIV4EffectInfos.xml
//
//

CvEffectInfo::CvEffectInfo() :
m_fUpdateRate(0.0f),
m_bProjectile(false),
m_bSticky(false),
m_fProjectileSpeed(0.0f),
m_fProjectileArc(0.0f)
/************************************************************************************************/
/* RevolutionDCM					  Start		 05/31/10						Afforess	   */
/*																							  */
/* Battle Effects																			   */
/************************************************************************************************/
,m_bBattleEffect(false)
/************************************************************************************************/
/* RevolutionDCM				 Battle Effects END											 */
/************************************************************************************************/
{
}

CvEffectInfo::~CvEffectInfo()
{
}

/************************************************************************************************/
/* RevolutionDCM					  Start		 05/31/10						Afforess	   */
/*																							  */
/* Battle Effects																			   */
/************************************************************************************************/
bool CvEffectInfo::isBattleEffect() const
{
	return m_bBattleEffect;
}
/************************************************************************************************/
/* RevolutionDCM				 Battle Effects END											 */
/************************************************************************************************/

bool CvEffectInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	CvScalableInfo::read(pXML);

	pXML->GetChildXmlValByName(szTextVal, L"Path");
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 10/27/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	CvXMLLoadUtilityModTools* p_aszDirName = new CvXMLLoadUtilityModTools;
	p_aszDirName->setLocationName( &szTextVal, (GC.getModDir()).c_str());
	SAFE_DELETE(p_aszDirName);
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/
	setPath(szTextVal);

	pXML->GetChildXmlValByName(&m_fUpdateRate, L"fUpdateRate" );

	int iTemporary;
	pXML->GetOptionalChildXmlValByName(&iTemporary, L"bIsProjectile" );
	m_bProjectile = iTemporary != 0;

	pXML->GetOptionalChildXmlValByName(&m_fProjectileSpeed, L"fSpeed");
	pXML->GetOptionalChildXmlValByName(&m_fProjectileArc, L"fArcValue");
	pXML->GetOptionalChildXmlValByName(&m_bSticky, L"bSticky");
/************************************************************************************************/
/* RevolutionDCM					  Start		 05/31/10						Afforess	   */
/*																							  */
/* Battle Effects																			   */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_bBattleEffect, L"bBattleEffect");
/************************************************************************************************/
/* RevolutionDCM				 Battle Effects END											 */
/************************************************************************************************/
	return true;
}

/************************************************************************************************/
/* XMLCOPY								 11/19/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvEffectInfo::copyNonDefaults(CvEffectInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	CvScalableInfo::copyNonDefaults(pClassInfo, pXML);

	if (getPath() == cDefault) setPath(pClassInfo->getPath());
	if (getUpdateRate() == fDefault) m_fUpdateRate = pClassInfo->getUpdateRate();

	if (isProjectile() == bDefault) m_bProjectile = pClassInfo->isProjectile();

	if (getProjectileSpeed() == fDefault) m_fProjectileSpeed = pClassInfo->getProjectileSpeed();
	if (getProjectileArc() == fDefault) m_fProjectileArc = pClassInfo->getProjectileArc();

	if (isSticky() == bDefault) m_bSticky = pClassInfo->isSticky();

/************************************************************************************************/
/* RevolutionDCM					  Start		 05/31/10						Afforess	   */
/*																							  */
/* Battle Effects																			   */
/************************************************************************************************/
	if (isBattleEffect() == bDefault) m_bBattleEffect = pClassInfo->isBattleEffect();
/************************************************************************************************/
/* RevolutionDCM				 Battle Effects END											 */
/************************************************************************************************/
}


//////////////////////////////////////////////////////////////////////////
//
//	CvAttachableInfo			Misc\CIV4AttachableInfos.xml
//
//
CvAttachableInfo::CvAttachableInfo() :
m_fUpdateRate(0.0f)
{
}

CvAttachableInfo::~CvAttachableInfo()
{
}

bool CvAttachableInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	CvScalableInfo::read(pXML);

	pXML->GetChildXmlValByName(szTextVal, L"Path");
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 10/26/07							MRGENIE		  */
/*																							  */
/*																							  */
/************************************************************************************************/
	CvXMLLoadUtilityModTools* p_szDirName = new CvXMLLoadUtilityModTools;
	p_szDirName->setLocationName( &szTextVal, (GC.getModDir()).c_str());
	SAFE_DELETE(p_szDirName);
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/
	setPath(szTextVal);

	return true;
}
void CvAttachableInfo::copyNonDefaults(CvAttachableInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);
	CvScalableInfo::copyNonDefaults(pClassInfo, pXML);

	if (getPath() == cDefault) setPath(pClassInfo->getPath());
}


//////////////////////////////////////////////////////////////////////////
//
//	CvCameraInfo			Interface\CIV4CameraInfos.xml
//
//
bool CvCameraInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"Path");
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 10/27/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	CvXMLLoadUtilityModTools* p_aszDirName = new CvXMLLoadUtilityModTools;
	p_aszDirName->setLocationName( &szTextVal, (GC.getModDir()).c_str());
	SAFE_DELETE(p_aszDirName);
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/
	setPath(szTextVal);

	return true;
}
void CvCameraInfo::copyNonDefaults(CvCameraInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);
	if (getPath() == cDefault) setPath(pClassInfo->getPath());
}


//////////////////////////////////////////////////////////////////////////
//
//	CvUnitArtStyleTypeInfo
//  Graphic Flavor Units packages by Impaler[WrG]
//

CvUnitArtStyleTypeInfo::CvUnitArtStyleTypeInfo()
{
}

CvUnitArtStyleTypeInfo::~CvUnitArtStyleTypeInfo()
{
}

const TCHAR* CvUnitArtStyleTypeInfo::getEarlyArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	FAssertMsg(i < GC.getUnitInfo((UnitTypes) j).getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumUnitInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");

	for (ArtDefineArray::const_iterator it = m_azEarlyArtDefineTags.begin(); it != m_azEarlyArtDefineTags.end(); ++it)
	{
		if ((*it).iMeshIndex == i && (*it).iUnitType == j)
		{
			return (*it).szTag;
		}
	}

	return NULL;
}

void CvUnitArtStyleTypeInfo::setEarlyArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal)
{
	FAssertMsg(i < GC.getUnitInfo((UnitTypes) j).getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumUnitInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");

	for (ArtDefineArray::iterator it = m_azEarlyArtDefineTags.begin(); it != m_azEarlyArtDefineTags.end(); ++it)
	{
		if ((*it).iMeshIndex == i && (*it).iUnitType == j)
		{
			(*it).szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azEarlyArtDefineTags.push_back(kTag);
}

const TCHAR* CvUnitArtStyleTypeInfo::getLateArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	FAssertMsg(i < GC.getUnitInfo((UnitTypes) j).getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumUnitInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");

	for (ArtDefineArray::const_iterator it = m_azLateArtDefineTags.begin(); it != m_azLateArtDefineTags.end(); ++it)
	{
		if ((*it).iMeshIndex == i && (*it).iUnitType == j)
		{
			return (*it).szTag;
		}
	}

	return NULL;
}

void CvUnitArtStyleTypeInfo::setLateArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal)
{
	FAssertMsg(i < GC.getUnitInfo((UnitTypes) j).getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumUnitInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");

	for (ArtDefineArray::iterator it = m_azLateArtDefineTags.begin(); it != m_azLateArtDefineTags.end(); ++it)
	{
		if ((*it).iMeshIndex == i && (*it).iUnitType == j)
		{
			(*it).szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azLateArtDefineTags.push_back(kTag);
}

const TCHAR* CvUnitArtStyleTypeInfo::getMiddleArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	FAssertMsg(i < GC.getUnitInfo((UnitTypes) j).getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumUnitInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");

	for (ArtDefineArray::const_iterator it = m_azMiddleArtDefineTags.begin(); it != m_azMiddleArtDefineTags.end(); ++it)
	{
		if ((*it).iMeshIndex == i && (*it).iUnitType == j)
		{
			return (*it).szTag;
		}
	}

	return NULL;
}

void CvUnitArtStyleTypeInfo::setMiddleArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal)
{
	FAssertMsg(i < GC.getUnitInfo((UnitTypes) j).getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumUnitInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");

	for (ArtDefineArray::iterator it = m_azMiddleArtDefineTags.begin(); it != m_azMiddleArtDefineTags.end(); ++it)
	{
		if ((*it).iMeshIndex == i && (*it).iUnitType == j)
		{
			(*it).szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azMiddleArtDefineTags.push_back(kTag);
}

/************************************************************************************************/
/* Afforess					  Start		 03/10/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
const TCHAR* CvUnitArtStyleTypeInfo::getClassicalArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	FAssertMsg(i < GC.getUnitInfo((UnitTypes) j).getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumUnitInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");

	for (ArtDefineArray::const_iterator it = m_azClassicalArtDefineTags.begin(); it != m_azClassicalArtDefineTags.end(); ++it)
	{
		if ((*it).iMeshIndex == i && (*it).iUnitType == j)
		{
			return (*it).szTag;
		}
	}

	return NULL;
}

void CvUnitArtStyleTypeInfo::setClassicalArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal)
{
	FAssertMsg(i < GC.getUnitInfo((UnitTypes) j).getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumUnitInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");

	for (ArtDefineArray::iterator it = m_azClassicalArtDefineTags.begin(); it != m_azClassicalArtDefineTags.end(); ++it)
	{
		if ((*it).iMeshIndex == i && (*it).iUnitType == j)
		{
			(*it).szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azClassicalArtDefineTags.push_back(kTag);
}

const TCHAR* CvUnitArtStyleTypeInfo::getRennArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	FAssertMsg(i < GC.getUnitInfo((UnitTypes) j).getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumUnitInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");

	for (ArtDefineArray::const_iterator it = m_azRennArtDefineTags.begin(); it != m_azRennArtDefineTags.end(); ++it)
	{
		if ((*it).iMeshIndex == i && (*it).iUnitType == j)
		{
			return (*it).szTag;
		}
	}

	return NULL;
}

void CvUnitArtStyleTypeInfo::setRennArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal)
{
	FAssertMsg(i < GC.getUnitInfo((UnitTypes) j).getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumUnitInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");

	for (ArtDefineArray::iterator it = m_azRennArtDefineTags.begin(); it != m_azRennArtDefineTags.end(); ++it)
	{
		if ((*it).iMeshIndex == i && (*it).iUnitType == j)
		{
			(*it).szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azRennArtDefineTags.push_back(kTag);
}

const TCHAR* CvUnitArtStyleTypeInfo::getIndustrialArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	FAssertMsg(i < GC.getUnitInfo((UnitTypes) j).getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumUnitInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");

	for (ArtDefineArray::const_iterator it = m_azIndustrialArtDefineTags.begin(); it != m_azIndustrialArtDefineTags.end(); ++it)
	{
		if ((*it).iMeshIndex == i && (*it).iUnitType == j)
		{
			return (*it).szTag;
		}
	}

	return NULL;
}

void CvUnitArtStyleTypeInfo::setIndustrialArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal)
{
	FAssertMsg(i < GC.getUnitInfo((UnitTypes) j).getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumUnitInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");

	for (ArtDefineArray::iterator it = m_azIndustrialArtDefineTags.begin(); it != m_azIndustrialArtDefineTags.end(); ++it)
	{
		if ((*it).iMeshIndex == i && (*it).iUnitType == j)
		{
			(*it).szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azIndustrialArtDefineTags.push_back(kTag);
}

const TCHAR* CvUnitArtStyleTypeInfo::getFutureArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	FAssertMsg(i < GC.getUnitInfo((UnitTypes) j).getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumUnitInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");

	for (ArtDefineArray::const_iterator it = m_azFutureArtDefineTags.begin(); it != m_azFutureArtDefineTags.end(); ++it)
	{
		if ((*it).iMeshIndex == i && (*it).iUnitType == j)
		{
			return (*it).szTag;
		}
	}

	return NULL;
}

void CvUnitArtStyleTypeInfo::setFutureArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const TCHAR* szVal)
{
	FAssertMsg(i < GC.getUnitInfo((UnitTypes) j).getGroupDefinitions(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumUnitInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");

	for (ArtDefineArray::iterator it = m_azFutureArtDefineTags.begin(); it != m_azFutureArtDefineTags.end(); ++it)
	{
		if ((*it).iMeshIndex == i && (*it).iUnitType == j)
		{
			(*it).szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azFutureArtDefineTags.push_back(kTag);
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/


bool CvUnitArtStyleTypeInfo::read(CvXMLLoadUtility* pXML)
{
	int j, i;
	int iNumSibs;
	int iIndex; // UnitIndex
	int iMesh;  // Mesh Index

	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	if (pXML->TryMoveToXmlFirstChild(L"StyleUnits"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				for (i = 0; i < iNumSibs; i++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitType");
					iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						iMesh = GC.getUnitInfo((UnitTypes) iIndex).getGroupDefinitions();

						if (pXML->TryMoveToXmlFirstChild(L"UnitMeshGroup"))
						{
							for (j = 0; j < iMesh; j++)
							{
							// Overwrite with the Style Art
/************************************************************************************************/
/* Afforess					  Start		 03/10/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"EarlyArtDefineTag"))
									szTextVal = GC.hasInfoTypeForString(szTextVal) ? szTextVal : "";
								setEarlyArtDefineTag(j, iIndex, szTextVal);
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"ClassicalArtDefineTag"))
									szTextVal = GC.hasInfoTypeForString(szTextVal) ? szTextVal : "";
								setClassicalArtDefineTag(j, iIndex, szTextVal);
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"MiddleArtDefineTag"))
									szTextVal = GC.hasInfoTypeForString(szTextVal) ? szTextVal : "";
								setMiddleArtDefineTag(j, iIndex, szTextVal);
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"RennArtDefineTag"))
									szTextVal = GC.hasInfoTypeForString(szTextVal) ? szTextVal : "";
								setRennArtDefineTag(j, iIndex, szTextVal);
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"IndustrialArtDefineTag"))
									szTextVal = GC.hasInfoTypeForString(szTextVal) ? szTextVal : "";
								setIndustrialArtDefineTag(j, iIndex, szTextVal);
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"LateArtDefineTag"))
									szTextVal = GC.hasInfoTypeForString(szTextVal) ? szTextVal : "";
								setLateArtDefineTag(j, iIndex, szTextVal);
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"FutureArtDefineTag"))
									szTextVal = GC.hasInfoTypeForString(szTextVal) ? szTextVal : "";
								setFutureArtDefineTag(j, iIndex, szTextVal);
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
								if (!pXML->TryMoveToXmlNextSibling())
								{
									break;
								}
							}
							pXML->MoveToXmlParent();
						}
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
			}
			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}

	return true;
}

/************************************************************************************************/
/* XMLCOPY								 11/19/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvUnitArtStyleTypeInfo::copyNonDefaults(CvUnitArtStyleTypeInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	for ( int iUnitIndex = 0; iUnitIndex < GC.getNumUnitInfos(); iUnitIndex++)
	{
		for ( int iUnitMesh = 0; iUnitMesh < GC.getUnitInfo((UnitTypes) iUnitIndex).getGroupDefinitions(); iUnitMesh++ )
		{
			if ( getEarlyArtDefineTag(iUnitMesh, iUnitIndex) == NULL )
			{
				if ( pClassInfo->getEarlyArtDefineTag(iUnitMesh, iUnitIndex) != NULL )
				{
					setEarlyArtDefineTag(iUnitMesh, iUnitIndex, pClassInfo->getEarlyArtDefineTag(iUnitMesh, iUnitIndex));
				}
			}

			if ( getLateArtDefineTag(iUnitMesh, iUnitIndex) == NULL )
			{
				if ( pClassInfo->getLateArtDefineTag(iUnitMesh, iUnitIndex) != NULL )
				{
					setLateArtDefineTag(iUnitMesh, iUnitIndex, pClassInfo->getLateArtDefineTag(iUnitMesh, iUnitIndex));
				}
			}

			if ( getMiddleArtDefineTag(iUnitMesh, iUnitIndex) == NULL )
			{
				if ( pClassInfo->getMiddleArtDefineTag(iUnitMesh, iUnitIndex) != NULL )
				{
					setMiddleArtDefineTag(iUnitMesh, iUnitIndex, pClassInfo->getMiddleArtDefineTag(iUnitMesh, iUnitIndex));
				}
			}

/************************************************************************************************/
/* Afforess					  Start		 03/10/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
			if ( getClassicalArtDefineTag(iUnitMesh, iUnitIndex) == NULL )
			{
				if ( pClassInfo->getClassicalArtDefineTag(iUnitMesh, iUnitIndex) != NULL )
				{
					setClassicalArtDefineTag(iUnitMesh, iUnitIndex, pClassInfo->getClassicalArtDefineTag(iUnitMesh, iUnitIndex));
				}
			}
			if ( getRennArtDefineTag(iUnitMesh, iUnitIndex) == NULL )
			{
				if ( pClassInfo->getRennArtDefineTag(iUnitMesh, iUnitIndex) != NULL )
				{
					setRennArtDefineTag(iUnitMesh, iUnitIndex, pClassInfo->getRennArtDefineTag(iUnitMesh, iUnitIndex));
				}
			}
			if ( getIndustrialArtDefineTag(iUnitMesh, iUnitIndex) == NULL )
			{
				if ( pClassInfo->getIndustrialArtDefineTag(iUnitMesh, iUnitIndex) != NULL )
				{
					setIndustrialArtDefineTag(iUnitMesh, iUnitIndex, pClassInfo->getIndustrialArtDefineTag(iUnitMesh, iUnitIndex));
				}
			}
			if ( getFutureArtDefineTag(iUnitMesh, iUnitIndex) == NULL )
			{
				if ( pClassInfo->getFutureArtDefineTag(iUnitMesh, iUnitIndex) != NULL )
				{
					setFutureArtDefineTag(iUnitMesh, iUnitIndex, pClassInfo->getFutureArtDefineTag(iUnitMesh, iUnitIndex));
				}
			}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

		}
	}
}

CvMainMenuInfo::CvMainMenuInfo()
{

}

CvMainMenuInfo::~CvMainMenuInfo()
{
}

std::string CvMainMenuInfo::getScene() const
{
	return m_szScene;
}

std::string CvMainMenuInfo::getSceneNoShader() const
{
	return m_szSceneNoShader;
}

std::string CvMainMenuInfo::getSoundtrack() const
{
	return m_szSoundtrack;
}

std::string CvMainMenuInfo::getLoading() const
{
	return m_szLoading;
}

std::string CvMainMenuInfo::getLoadingSlideshow() const
{
	return m_szLoadingSlideshow;
}

bool CvMainMenuInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(m_szScene, L"Scene");
	pXML->GetChildXmlValByName(m_szSceneNoShader, L"SceneNoShader");
	pXML->GetChildXmlValByName(m_szSoundtrack, L"Soundtrack");
	pXML->GetChildXmlValByName(m_szLoading, L"Loading");
	pXML->GetChildXmlValByName(m_szLoadingSlideshow, L"LoadingSlideshow");

	return true;
}

void CvMainMenuInfo::copyNonDefaults(CvMainMenuInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getScene() == cDefault) m_szScene = pClassInfo->getScene();
	if (getSceneNoShader() == cDefault) m_szSceneNoShader = pClassInfo->getSceneNoShader();
	if (getSoundtrack() == cDefault) m_szSoundtrack = pClassInfo->getSoundtrack();
	if (getLoading() == cDefault) m_szLoading = pClassInfo->getLoading();
	if (getLoadingSlideshow() == cDefault) m_szLoadingSlideshow = pClassInfo->getLoadingSlideshow();
}

/************************************************************************************************/
/* MODULAR_LOADING_CONTROL				 10/25/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
// Python Modular Loading
CvPythonModulesInfo::CvPythonModulesInfo():
m_bMainInterfaceScreen(false),
m_bCivicScreen(false),
m_bCorporationScreen(false),
m_bDomesticAdvisor(false),
m_bEspionageAdvisor(false),
m_bForeignAdvisor(false),
m_bMilitaryAdvisor(false),
m_bVictoryScreen(false),
m_iScreen(0)
{
}

CvPythonModulesInfo::~CvPythonModulesInfo()
{
}

bool CvPythonModulesInfo::isMainInterfaceScreen()
{
	return m_bMainInterfaceScreen;
}

bool CvPythonModulesInfo::isCivicScreen()
{
	return m_bCivicScreen;
}

bool CvPythonModulesInfo::isCorporationScreen()
{
	return m_bCorporationScreen;
}

bool CvPythonModulesInfo::isDomesticAdvisor()
{
	return m_bDomesticAdvisor;
}

bool CvPythonModulesInfo::isEspionageAdvisor()
{
	return m_bEspionageAdvisor;
}

bool CvPythonModulesInfo::isForeignAdvisor()
{
	return m_bForeignAdvisor;
}

bool CvPythonModulesInfo::isMilitaryAdvisor()
{
	return m_bMilitaryAdvisor;
}

bool CvPythonModulesInfo::isVictoryScreen()
{
	return m_bVictoryScreen;
}

int CvPythonModulesInfo::getScreen()
{
	return m_iScreen;
}

bool CvPythonModulesInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetOptionalChildXmlValByName(&m_bMainInterfaceScreen, L"bMainInterfaceScreen");
	pXML->GetOptionalChildXmlValByName(&m_bCivicScreen, L"bCivicScreen");
	pXML->GetOptionalChildXmlValByName(&m_bCorporationScreen, L"bCorporationScreen");
	pXML->GetOptionalChildXmlValByName(&m_bDomesticAdvisor, L"bDomesticAdvisor");
	pXML->GetOptionalChildXmlValByName(&m_bEspionageAdvisor, L"bEspionageAdvisor");
	pXML->GetOptionalChildXmlValByName(&m_bForeignAdvisor, L"bForeignAdvisor");
	pXML->GetOptionalChildXmlValByName(&m_bMilitaryAdvisor, L"bMilitaryAdvisor");
	pXML->GetOptionalChildXmlValByName(&m_bVictoryScreen, L"bVictoryScreen");
	pXML->GetOptionalChildXmlValByName(&m_iScreen, L"iScreen");
	return true;
}

// MLF loading
CvModLoadControlInfo::CvModLoadControlInfo():
m_bLoad(NULL),
m_iDirDepth(0),
m_iNumModules(0),
m_paszModuleFolder(NULL)
{
}

CvModLoadControlInfo::~CvModLoadControlInfo()
{
	SAFE_DELETE_ARRAY(m_bLoad);
	SAFE_DELETE_ARRAY(m_paszModuleFolder);
}

bool CvModLoadControlInfo::isLoad(int i)
{
	FAssertMsg(i < getNumModules(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_bLoad[i];
}

void CvModLoadControlInfo::setLoad(int i, bool bLoad)
{
	FAssertMsg(i < getNumModules(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	m_bLoad[i] = bLoad;
}

int CvModLoadControlInfo::getNumModules()
{
	return m_iNumModules;
}

int CvModLoadControlInfo::getDirDepth()
{
	return m_iDirDepth;
}

std::string CvModLoadControlInfo::getModuleFolder(int i)
{
	FAssertMsg(i < getNumModules(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paszModuleFolder[i];
}

std::string CvModLoadControlInfo::getParentFolder()
{
	return m_paszParentFolder;
}

bool CvModLoadControlInfo::read(CvXMLLoadUtility* pXML, CvString szDirDepth, int iDirDepth)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	//Class variables
	m_iDirDepth = iDirDepth;
	m_paszParentFolder = szDirDepth; //verify previous level in CvXMLLoadUtility::getModLoadControlDirectory
	//method variables
	CvString szTextVal;
	std::string m_szDirDepth;
	bool bLoad;

	if (pXML->TryMoveToXmlFirstChild(L"Modules"))
	{
		m_iNumModules = pXML->GetXmlChildrenNumber();
		if (0 < m_iNumModules)
		{
			m_paszModuleFolder = new std::string[m_iNumModules];
			m_bLoad = new bool[m_iNumModules];
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int iIndex = 0; iIndex < m_iNumModules; iIndex++)
				{
					m_bLoad[iIndex] = false;	// by default bLoad is false
					if (pXML->GetChildXmlVal(szTextVal))
					{
						pXML->GetNextXmlVal(&bLoad, true);
						if (bLoad)
						{
							m_bLoad[iIndex] = bLoad;	//this Module needs to be loaded
							m_szDirDepth = szDirDepth;
							m_szDirDepth.append(szTextVal);
							m_paszModuleFolder[iIndex] = m_szDirDepth;
							GC.setTotalNumModules();  //we need this for looping in the XMLLoad class
						}

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
	}
	return true;
}
/************************************************************************************************/
/* MODULAR_LOADING_CONTROL				 END												  */
/************************************************************************************************/
