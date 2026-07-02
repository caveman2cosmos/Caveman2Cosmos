//------------------------------------------------------------------------------------------------
//  FILE:    CvHotkeyInfo.cpp
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
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szHotKey, L"HotKey");
	m_iHotKeyVal = pXML->GetHotKeyInt(m_szHotKey);

	pXML->GetOptionalChildXmlValByName(&m_iHotKeyPriority, L"iHotKeyPriority", -1);

	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"HotKeyAlt");
	m_iHotKeyValAlt = pXML->GetHotKeyInt(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iHotKeyPriorityAlt, L"iHotKeyPriorityAlt", -1);
	pXML->GetOptionalChildXmlValByName(&m_bAltDown, L"bAltDown");
	pXML->GetOptionalChildXmlValByName(&m_bShiftDown, L"bShiftDown");
	pXML->GetOptionalChildXmlValByName(&m_bCtrlDown, L"bCtrlDown");
	pXML->GetOptionalChildXmlValByName(&m_bAltDownAlt, L"bAltDownAlt");
	pXML->GetOptionalChildXmlValByName(&m_bShiftDownAlt, L"bShiftDownAlt");
	pXML->GetOptionalChildXmlValByName(&m_bCtrlDownAlt, L"bCtrlDownAlt");
	pXML->GetOptionalChildXmlValByName(&m_iOrderPriority, L"iOrderPriority", 0);

	setHotKeyDescription(getTextKeyWide(), NULL, pXML->CreateHotKeyFromDescription(getHotKey(), m_bShiftDown, m_bAltDown, m_bCtrlDown));

	return true;
}


void CvHotkeyInfo::copyNonDefaults(const CvHotkeyInfo* pClassInfo)
{
	CvInfoBase::copyNonDefaults(pClassInfo);

	const CvString cDefault = CvString::format("").GetCString();
	const CvWString wDefault = CvWString::format(L"").GetCString();

	if (getHotKeyVal() == 0)
	{
		m_iHotKeyVal = pClassInfo->getHotKeyVal();
	}
	if (getHotKeyPriority() == -1)
	{
		m_iHotKeyPriority = pClassInfo->getHotKeyPriority();
	}
	if (getHotKeyValAlt() == 0)
	{
		m_iHotKeyValAlt = pClassInfo->getHotKeyValAlt();
	}
	if (getHotKeyPriorityAlt() == -1)
	{
		m_iHotKeyPriorityAlt = pClassInfo->getHotKeyPriorityAlt();
	}
	if (getOrderPriority() == 0)
	{
		m_iOrderPriority = pClassInfo->getOrderPriority();
	}

	if (isAltDown() == false)
	{
		m_bAltDown = pClassInfo->isAltDown();
	}
	if (isShiftDown() == false)
	{
		m_bShiftDown = pClassInfo->isShiftDown();
	}
	if (isCtrlDown() == false)
	{
		m_bCtrlDown = pClassInfo->isCtrlDown();
	}
	if (isAltDownAlt() == false)
	{
		m_bAltDownAlt = pClassInfo->isAltDownAlt();
	}
	if (isShiftDownAlt() == false)
	{
		m_bShiftDownAlt = pClassInfo->isShiftDownAlt();
	}
	if (isCtrlDownAlt() == false)
	{
		m_bCtrlDownAlt = pClassInfo->isCtrlDownAlt();
	}

	if (getHotKey() == cDefault)
	{
		m_szHotKey = pClassInfo->getHotKey();
	}
	if (getHotKeyDescriptionKey() == wDefault)
	{
		m_szHotKeyDescriptionKey = pClassInfo->getHotKeyDescriptionKey();
	}
	if (getHotKeyAltDescriptionKey() == wDefault)
	{
		m_szHotKeyAltDescriptionKey = pClassInfo->getHotKeyAltDescriptionKey();
	}
	if (getHotKeyString() == wDefault)
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


int CvHotkeyInfo::getHotKeyPriority() const
{
	return m_iHotKeyPriority;
}


int CvHotkeyInfo::getHotKeyValAlt() const
{
	return m_iHotKeyValAlt;
}


int CvHotkeyInfo::getHotKeyPriorityAlt() const
{
	return m_iHotKeyPriorityAlt;
}


int CvHotkeyInfo::getOrderPriority() const
{
	return m_iOrderPriority;
}


bool CvHotkeyInfo::isAltDown() const
{
	return m_bAltDown;
}


bool CvHotkeyInfo::isShiftDown() const
{
	return m_bShiftDown;
}


bool CvHotkeyInfo::isCtrlDown() const
{
	return m_bCtrlDown;
}


bool CvHotkeyInfo::isAltDownAlt() const
{
	return m_bAltDownAlt;
}


bool CvHotkeyInfo::isShiftDownAlt() const
{
	return m_bShiftDownAlt;
}


bool CvHotkeyInfo::isCtrlDownAlt() const
{
	return m_bCtrlDownAlt;
}


const char* CvHotkeyInfo::getHotKey() const
{
	return m_szHotKey;
}


const wchar_t* CvHotkeyInfo::getHotKeyDescriptionKey() const
{
	return m_szHotKeyDescriptionKey;
}


const wchar_t* CvHotkeyInfo::getHotKeyAltDescriptionKey() const
{
	return m_szHotKeyAltDescriptionKey;
}


const wchar_t* CvHotkeyInfo::getHotKeyString() const
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


void CvHotkeyInfo::setHotKeyDescription(const wchar_t* szHotKeyDescKey, const wchar_t* szHotKeyAltDescKey, const wchar_t* szHotKeyString)
{
	m_szHotKeyDescriptionKey = szHotKeyDescKey;
	m_szHotKeyAltDescriptionKey = szHotKeyAltDescKey;
	m_szHotKeyString = szHotKeyString;
}

