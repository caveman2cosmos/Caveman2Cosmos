//------------------------------------------------------------------------------------------------
//  FILE:    CvUnitArtStyleTypeInfo.cpp
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
#include "CvUnitArtStyleTypeInfo.h"


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


const char* CvUnitArtStyleTypeInfo::getEarlyArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(const ArtDefneTag& it, m_azEarlyArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			return it.szTag;
		}
	}

	return NULL;
}


void CvUnitArtStyleTypeInfo::setEarlyArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(ArtDefneTag& it, m_azEarlyArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			it.szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azEarlyArtDefineTags.push_back(kTag);
}


const char* CvUnitArtStyleTypeInfo::getLateArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(const ArtDefneTag& it, m_azLateArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			return it.szTag;
		}
	}

	return NULL;
}


void CvUnitArtStyleTypeInfo::setLateArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(ArtDefneTag& it, m_azLateArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			it.szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azLateArtDefineTags.push_back(kTag);
}


const char* CvUnitArtStyleTypeInfo::getMiddleArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(const ArtDefneTag& it, m_azMiddleArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			return it.szTag;
		}
	}

	return NULL;
}


void CvUnitArtStyleTypeInfo::setMiddleArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(ArtDefneTag& it, m_azMiddleArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			it.szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azMiddleArtDefineTags.push_back(kTag);
}



const char* CvUnitArtStyleTypeInfo::getClassicalArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(const ArtDefneTag& it, m_azClassicalArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			return it.szTag;
		}
	}

	return NULL;
}


void CvUnitArtStyleTypeInfo::setClassicalArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(ArtDefneTag& it, m_azClassicalArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			it.szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azClassicalArtDefineTags.push_back(kTag);
}


const char* CvUnitArtStyleTypeInfo::getRennArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(const ArtDefneTag& it, m_azRennArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			return it.szTag;
		}
	}

	return NULL;
}


void CvUnitArtStyleTypeInfo::setRennArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(ArtDefneTag& it, m_azRennArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			it.szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azRennArtDefineTags.push_back(kTag);
}


const char* CvUnitArtStyleTypeInfo::getIndustrialArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(const ArtDefneTag& it, m_azIndustrialArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			return it.szTag;
		}
	}

	return NULL;
}


void CvUnitArtStyleTypeInfo::setIndustrialArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(ArtDefneTag& it, m_azIndustrialArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			it.szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azIndustrialArtDefineTags.push_back(kTag);
}


const char* CvUnitArtStyleTypeInfo::getFutureArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(const ArtDefneTag& it, m_azFutureArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			return it.szTag;
		}
	}

	return NULL;
}


void CvUnitArtStyleTypeInfo::setFutureArtDefineTag(int /*Mesh Index*/ i, int /*UnitType*/ j, const char* szVal)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, GC.getUnitInfo((UnitTypes)j).getGroupDefinitions(), i);
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), j);

	foreach_(ArtDefneTag& it, m_azFutureArtDefineTags)
	{
		if (it.iMeshIndex == i && it.iUnitType == j)
		{
			it.szTag = szVal;
			return;
		}
	}

	ArtDefneTag kTag;
	kTag.iMeshIndex = i;
	kTag.iUnitType = j;
	kTag.szTag = szVal;
	m_azFutureArtDefineTags.push_back(kTag);
}



bool CvUnitArtStyleTypeInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	int j, i;
	int iNumSibs;
	int iIndex; // UnitIndex
	int iMesh;  // Mesh Index


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
							// Overwrite with the Style Art
							for (j = 0; j < iMesh; j++)
							{
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"EarlyArtDefineTag"))
									szTextVal = GC.getInfoTypeForString(szTextVal) > -1 ? szTextVal : "";
								setEarlyArtDefineTag(j, iIndex, szTextVal);
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"ClassicalArtDefineTag"))
									szTextVal = GC.getInfoTypeForString(szTextVal) > -1 ? szTextVal : "";
								setClassicalArtDefineTag(j, iIndex, szTextVal);
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"MiddleArtDefineTag"))
									szTextVal = GC.getInfoTypeForString(szTextVal) > -1 ? szTextVal : "";
								setMiddleArtDefineTag(j, iIndex, szTextVal);
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"RennArtDefineTag"))
									szTextVal = GC.getInfoTypeForString(szTextVal) > -1 ? szTextVal : "";
								setRennArtDefineTag(j, iIndex, szTextVal);
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"IndustrialArtDefineTag"))
									szTextVal = GC.getInfoTypeForString(szTextVal) > -1 ? szTextVal : "";
								setIndustrialArtDefineTag(j, iIndex, szTextVal);
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"LateArtDefineTag"))
									szTextVal = GC.getInfoTypeForString(szTextVal) > -1 ? szTextVal : "";
								setLateArtDefineTag(j, iIndex, szTextVal);
								if (pXML->GetOptionalChildXmlValByName(szTextVal, L"FutureArtDefineTag"))
									szTextVal = GC.getInfoTypeForString(szTextVal) > -1 ? szTextVal : "";
								setFutureArtDefineTag(j, iIndex, szTextVal);

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


void CvUnitArtStyleTypeInfo::copyNonDefaults(const CvUnitArtStyleTypeInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

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
		}
	}
}

