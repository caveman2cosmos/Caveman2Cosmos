//------------------------------------------------------------------------------------------------
//  FILE:    CvGameText.cpp
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
#include "CvGameText.h"


//////////////////////////////////////////////////////////////////////////
// CvGameText
//////////////////////////////////////////////////////////////////////////

// static
int CvGameText::NUM_LANGUAGES = 7;
CvWString CvGameText::m_szLanguage = L"";

CvGameText::CvGameText() :
	m_szGender("N"),
	m_szPlural("false")
{
}




bool CvGameText::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	//pXML->TryMoveToXmlFirstChild(); // Move down to Child level
	pXML->GetChildXmlValByName(m_szType, L"Tag");
	//pXML->GetXmlVal(m_szType);		// TAG

	int iFallback = 0;

	CvWString wszTextVal;

	while (true)
	{
		switch (iFallback)
		{
			case 0:
				iFallback++;
				if (pXML->TryMoveToXmlFirstChild(m_szLanguage))
				{
					break;
				}

			case 1:
				// fallback to English
				iFallback++;
				if (pXML->TryMoveToXmlFirstChild(L"English"))
				{
					break;
				}

			case 2:
				// fallback to first tag after Tag
				iFallback++;
				if (!pXML->TryMoveToXmlFirstChild())
				{
					return false;
				}
				else if (!pXML->TryMoveToXmlNextSibling())
				{
					pXML->MoveToXmlParent();
					return false;
				}
				break;

			default:
				// no success, discard the text
				return false;
		}

		//CvString szTextVal;

		// we are now on the chosen language tag (or a fallback)
		if (!pXML->GetOptionalChildXmlValByName(wszTextVal, L"Text", L""))
		{
			pXML->GetXmlVal(wszTextVal);

		}

		// if there is content, we succeeded and break the loop
		if (!wszTextVal.empty())
		{
			m_szText = wszTextVal;
			break;
		}
		else
		{
			// next fallback, but undo the SetToChild first
			pXML->MoveToXmlParent();
		}
	}

	// GENDER
	if (pXML->GetOptionalChildXmlValByName(wszTextVal, L"Gender", L""))
	{
		m_szGender = wszTextVal;
	}

	// PLURAL
	if (pXML->GetOptionalChildXmlValByName(wszTextVal, L"Plural", L""))
	{
		m_szPlural = wszTextVal;
	}

	// old code
	/*int j=0;
	for (j = 0; j < iNumLanguages; j++)
	{
		pXML->SkipToNextVal();	// skip comments

		if (!pXML->TryMoveToXmlNextSibling(); || j == iMaxNumLanguages)
		{
			NUM_LANGUAGES = j;
			break;
		}
		if (j == GAMETEXT.getCurrentLanguage()) // Only add appropriate language Text
		{
			// TEXT
			if (pXML->GetChildXmlValByName(wszTextVal, L"Text"))
			{
				m_szText = wszTextVal;
			}
			else
			{
				pXML->GetXmlVal(wszTextVal);
				setText(wszTextVal);
				if (NUM_LANGUAGES > 0)
				{
					break;
				}
			}

			// GENDER
			if (pXML->GetChildXmlValByName(wszTextVal, L"Gender"))
			{
				m_szGender = wszTextVal;
			}

			// PLURAL
			if (pXML->GetChildXmlValByName(wszTextVal, L"Plural"))
			{
				m_szPlural = wszTextVal;
			}
			if (NUM_LANGUAGES > 0)
			{
				break;
			}
		}
	}*/

	pXML->MoveToXmlParent(); // Move back up to Parent

	return true;
}

