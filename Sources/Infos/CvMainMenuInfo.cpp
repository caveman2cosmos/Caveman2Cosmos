//------------------------------------------------------------------------------------------------
//  FILE:    CvMainMenuInfo.cpp
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
#include "CvMainMenuInfo.h"


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


void CvMainMenuInfo::copyNonDefaults(const CvMainMenuInfo* pClassInfo)
{
	const CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getScene() == cDefault) m_szScene = pClassInfo->getScene();
	if (getSceneNoShader() == cDefault) m_szSceneNoShader = pClassInfo->getSceneNoShader();
	if (getSoundtrack() == cDefault) m_szSoundtrack = pClassInfo->getSoundtrack();
	if (getLoading() == cDefault) m_szLoading = pClassInfo->getLoading();
	if (getLoadingSlideshow() == cDefault) m_szLoadingSlideshow = pClassInfo->getLoadingSlideshow();
}

