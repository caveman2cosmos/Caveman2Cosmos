//------------------------------------------------------------------------------------------------
//  FILE:    CvLandscapeInfo.cpp
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
#include "CvLandscapeInfo.h"


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


//
// read from xml
//
bool CvLandscapeInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_iWaterHeight, L"iWaterHeight");
	pXML->GetChildXmlValByName(&m_bRandomMap, L"bRandomMap");

	pXML->GetOptionalChildXmlValByName(m_szHeightMap, L"HeightMap");
	pXML->GetOptionalChildXmlValByName(m_szTerrainMap, L"TerrainMap");
	pXML->GetOptionalChildXmlValByName(m_szNormalMap, L"NormalMap");
	pXML->GetOptionalChildXmlValByName(m_szBlendMap, L"BlendMap");
	pXML->GetOptionalChildXmlValByName(m_szSkyArt, L"SkyArt");

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


void CvLandscapeInfo::copyNonDefaults(const CvLandscapeInfo* pClassInfo)
{
	bool bDefault = false;
	int iDefault = 0;
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getWaterHeight() == iDefault) m_iWaterHeight = pClassInfo->getWaterHeight();
	if (isRandomMap() == bDefault) m_bRandomMap = pClassInfo->isRandomMap();
	if (getHeightMap() == cDefault) m_szHeightMap = pClassInfo->getHeightMap();
	if (getTerrainMap() == cDefault) m_szTerrainMap = pClassInfo->getTerrainMap();
	if (getNormalMap() == cDefault) m_szNormalMap = pClassInfo->getNormalMap();
	if (getBlendMap() == cDefault) m_szBlendMap = pClassInfo->getBlendMap();
	if (getSkyArt() == cDefault) m_szSkyArt = pClassInfo->getSkyArt();
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

