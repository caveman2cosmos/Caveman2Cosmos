//------------------------------------------------------------------------------------------------
//  FILE:    CvModLoadControlInfo.cpp
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
#include "CvModLoadControlInfo.h"


/************************************************************************************************/
/* MODULAR_LOADING_CONTROL				 10/24/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
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


bool CvModLoadControlInfo::isLoad(int i) const
{
	FASSERT_BOUNDS(0, getNumModules(), i);
	return m_bLoad[i];
}


void CvModLoadControlInfo::setLoad(int i, bool bLoad)
{
	FASSERT_BOUNDS(0, getNumModules(), i);
	m_bLoad[i] = bLoad;
}


int CvModLoadControlInfo::getNumModules() const
{
	return m_iNumModules;
}


int CvModLoadControlInfo::getDirDepth() const
{
	return m_iDirDepth;
}


const std::string CvModLoadControlInfo::getModuleFolder(int i) const
{
	FASSERT_BOUNDS(0, getNumModules(), i);
	return m_paszModuleFolder[i];
}


const std::string CvModLoadControlInfo::getParentFolder() const
{
	return m_paszParentFolder;
}


bool CvModLoadControlInfo::read(CvXMLLoadUtility* pXML, CvString szDirDepth, int iDirDepth)
{
	PROFILE_EXTRA_FUNC();
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

