//
// Python wrapper class for CvArtFileMgr 
// 
#include "CvGameCoreDLL.h"

CyArtFileMgr::CyArtFileMgr() : m_pArtFileMgr(NULL) { m_pArtFileMgr = &ARTFILEMGR; }

CyArtFileMgr::CyArtFileMgr(CvArtFileMgr* pArtFileMgr) : m_pArtFileMgr(pArtFileMgr) {}

CvArtInfoInterface* CyArtFileMgr::getInterfaceArtInfo( const char * szArtDefineTag ) const
{
	return m_pArtFileMgr ? m_pArtFileMgr->getInterfaceArtInfo( szArtDefineTag ) : NULL;
}

CvArtInfoMovie* CyArtFileMgr::getMovieArtInfo( const char * szArtDefineTag ) const
{
	return m_pArtFileMgr ? m_pArtFileMgr->getMovieArtInfo( szArtDefineTag ) : NULL;
}

CvArtInfoMisc* CyArtFileMgr::getMiscArtInfo( const char * szArtDefineTag ) const
{
	return m_pArtFileMgr ? m_pArtFileMgr->getMiscArtInfo( szArtDefineTag ) : NULL;
}

CvArtInfoUnit* CyArtFileMgr::getUnitArtInfo( const char * szArtDefineTag ) const
{
	return m_pArtFileMgr ? m_pArtFileMgr->getUnitArtInfo( szArtDefineTag ) : NULL;
}

CvArtInfoBuilding* CyArtFileMgr::getBuildingArtInfo( const char * szArtDefineTag ) const
{
	return m_pArtFileMgr ? m_pArtFileMgr->getBuildingArtInfo( szArtDefineTag ) : NULL;
}

CvArtInfoCivilization* CyArtFileMgr::getCivilizationArtInfo( const char * szArtDefineTag ) const
{
	return m_pArtFileMgr ? m_pArtFileMgr->getCivilizationArtInfo( szArtDefineTag ) : NULL;
}

CvArtInfoBonus* CyArtFileMgr::getBonusArtInfo( const char * szArtDefineTag ) const
{
	return m_pArtFileMgr ? m_pArtFileMgr->getBonusArtInfo( szArtDefineTag ) : NULL;
}

CvArtInfoImprovement* CyArtFileMgr::getImprovementArtInfo( const char * szArtDefineTag ) const
{
	return m_pArtFileMgr ? m_pArtFileMgr->getImprovementArtInfo( szArtDefineTag ) : NULL;
}
