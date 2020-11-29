//
// Python wrapper class for CvArtFileMgr 
// 
#include "CvGameCoreDLL.h"
#include "CyArtFileMgr.h"

CyArtFileMgr::CyArtFileMgr() : m_pArtFileMgr(ARTFILEMGR) {}

CyArtFileMgr::CyArtFileMgr(const CvArtFileMgr& pArtFileMgr) : m_pArtFileMgr(pArtFileMgr) {}


CvArtInfoInterface* CyArtFileMgr::getInterfaceArtInfo(const char* szArtDefineTag) const
{
	return m_pArtFileMgr.getInterfaceArtInfo(szArtDefineTag);
}

CvArtInfoMovie* CyArtFileMgr::getMovieArtInfo(const char* szArtDefineTag) const
{
	return m_pArtFileMgr.getMovieArtInfo(szArtDefineTag);
}

CvArtInfoMisc* CyArtFileMgr::getMiscArtInfo(const char* szArtDefineTag) const
{
	return m_pArtFileMgr.getMiscArtInfo(szArtDefineTag);
}

CvArtInfoUnit* CyArtFileMgr::getUnitArtInfo(const char* szArtDefineTag) const
{
	return m_pArtFileMgr.getUnitArtInfo(szArtDefineTag);
}

CvArtInfoBuilding* CyArtFileMgr::getBuildingArtInfo(const char* szArtDefineTag) const
{
	return m_pArtFileMgr.getBuildingArtInfo(szArtDefineTag);
}

CvArtInfoCivilization* CyArtFileMgr::getCivilizationArtInfo(const char* szArtDefineTag) const
{
	return m_pArtFileMgr.getCivilizationArtInfo(szArtDefineTag);
}

CvArtInfoLeaderhead* CyArtFileMgr::getLeaderheadArtInfo(const char* szArtDefineTag) const
{
	return m_pArtFileMgr.getLeaderheadArtInfo(szArtDefineTag);
}

CvArtInfoBonus* CyArtFileMgr::getBonusArtInfo(const char* szArtDefineTag) const
{
	return m_pArtFileMgr.getBonusArtInfo(szArtDefineTag);
}

CvArtInfoImprovement* CyArtFileMgr::getImprovementArtInfo(const char* szArtDefineTag) const
{
	return m_pArtFileMgr.getImprovementArtInfo(szArtDefineTag);
}

CvArtInfoTerrain* CyArtFileMgr::getTerrainArtInfo(const char* szArtDefineTag) const
{
	return m_pArtFileMgr.getTerrainArtInfo(szArtDefineTag);
}

CvArtInfoFeature* CyArtFileMgr::getFeatureArtInfo(const char* szArtDefineTag) const
{
	return m_pArtFileMgr.getFeatureArtInfo(szArtDefineTag);
}
