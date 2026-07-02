//------------------------------------------------------------------------------------------------
//  FILE:    CvArtInfoTerrain.cpp
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
#include "CvArtInfoTerrain.h"

// Forward decl for helper defined at the bottom of this file (used by CvArtInfoTerrain::read).
void BuildSlotList( CvTextureBlendSlotList &list, CvString &numlist);


//////////////////////////////////////////////////////////////////////////
// CvArtInfoTerrain
//////////////////////////////////////////////////////////////////////////

CvArtInfoTerrain::CvArtInfoTerrain()
	: m_iLayerOrder(0)
	, m_bAlphaShader(false)
{
}


const char* CvArtInfoTerrain::getBaseTexture()
{
	return getPath();
}


void CvArtInfoTerrain::setBaseTexture(const char* szTmp)
{
	setPath(szTmp);
}


const char* CvArtInfoTerrain::getGridTexture()
{
	return m_szGridTexture;
}


void CvArtInfoTerrain::setGridTexture(const char* szTmp)
{
	m_szGridTexture = szTmp;
}


const char* CvArtInfoTerrain::getDetailTexture()
{
	return m_szDetailTexture;
}


void CvArtInfoTerrain::setDetailTexture(const char* szTmp)
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


CvTextureBlendSlotList& CvArtInfoTerrain::getBlendList(int blendMask)
{
	FAssertMsg(blendMask >= 1 && blendMask < NUM_TEXTURE_BLENDS, "blendMask index must be in valid range");
	return m_pTextureSlots[blendMask];
}


bool CvArtInfoTerrain::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvArtInfoAsset::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(szTextVal, L"Grid");
	setGridTexture(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, L"Detail");
	setDetailTexture(szTextVal);

	pXML->GetChildXmlValByName(&m_iLayerOrder, L"LayerOrder");
	pXML->GetChildXmlValByName(&m_bAlphaShader, L"AlphaShader");

	// Parse texture slots for blend tile lists
	wchar_t xmlName[] = L"TextureBlend00";
	for (int i = 1; i < NUM_TEXTURE_BLENDS; i++)
	{
		swprintf(xmlName + (wcslen(xmlName) - 2), L"%02d", i);
		pXML->GetChildXmlValByName(szTextVal, xmlName);

		m_pSlotNames[i] = szTextVal;
		BuildSlotList(m_pTextureSlots[i], szTextVal);
	}

	// AIAndy: Reading CvArtInfoAsset again is bad
	// return CvArtInfoAsset::read(pXML);
	return true;
}


void CvArtInfoTerrain::copyNonDefaults(const CvArtInfoTerrain* pClassInfo)
{
	// Empty, for Art files we stick to FULL XML defintions
}

void BuildSlotList( CvTextureBlendSlotList &list, CvString &numlist)
{
	PROFILE_EXTRA_FUNC();
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

