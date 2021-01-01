#include "CvGameCoreDLL.h"
#include "CvCity.h"
#include "CvDLLEntity.h"
#include "CvDLLEntityIFaceBase.h"
#include "CvUnit.h"

static int g_numEntitiesCreated = 0;
static int g_numEntitiesDestroyed = 0;

CvDLLEntity::CvDLLEntity() : m_pEntity(NULL)
{
}

CvDLLEntity::~CvDLLEntity()
{
}

void CvDLLEntity::removeEntity()
{
	gDLL->getEntityIFace()->removeEntity(getEntity());
}

void CvDLLEntity::setup()
{
	gDLL->getEntityIFace()->setup(getEntity());
}

void CvDLLEntity::setVisible(bool bVis)
{
	gDLL->getEntityIFace()->setVisible(getEntity(), bVis);
}

void CvDLLEntity::createCityEntity(CvCity* pCity)
{
	gDLL->getEntityIFace()->createCityEntity(pCity);
}

void CvDLLEntity::createUnitEntity(CvUnit* pUnit)
{
	g_numEntitiesCreated++;

	if ( g_numEntitiesCreated%100 == 0 )
	{
		OutputDebugString(CvString::format("Num unit entities created: %d, destroyed: %d - %d extant\n", g_numEntitiesCreated, g_numEntitiesDestroyed, g_numEntitiesCreated - g_numEntitiesDestroyed).c_str());
	}

	//OutputDebugString(CvString::format("createUnitEntity for %08lx\n", this).c_str());
	gDLL->getEntityIFace()->createUnitEntity(pUnit);
}

void CvDLLEntity::destroyEntity()
{
	g_numEntitiesDestroyed++;

	//OutputDebugString(CvString::format("destroyEntity for %08lx\n", this).c_str());
	gDLL->getEntityIFace()->destroyEntity(m_pEntity);
}

bool CvDLLEntity::IsSelected() const
{
	return (!CvUnit::isRealEntity(getEntity())) ? false : gDLL->getEntityIFace()->IsSelected(getEntity());
}

void CvDLLEntity::PlayAnimation(AnimationTypes eAnim, float fSpeed, bool bQueue, int iLayer, float fStartPct, float fEndPct)
{
	if ( CvUnit::isRealEntity(getEntity()) )
	{
		gDLL->getEntityIFace()->PlayAnimation(getEntity(), eAnim, fSpeed, bQueue, iLayer, fStartPct, fEndPct);
	}
}

void CvDLLEntity::StopAnimation(AnimationTypes eAnim)
{
	if ( CvUnit::isRealEntity(getEntity()) )
	{
		gDLL->getEntityIFace()->StopAnimation(getEntity(), eAnim);
	}
}

void CvDLLEntity::MoveTo( const CvPlot * pkPlot )
{
	if ( CvUnit::isRealEntity(getEntity()) )
	{
		gDLL->getEntityIFace()->MoveTo(getUnitEntity(), pkPlot );
	}
}

void CvDLLEntity::QueueMove( const CvPlot * pkPlot )
{
	if ( CvUnit::isRealEntity(getEntity()) )
	{
		gDLL->getEntityIFace()->QueueMove(getUnitEntity(), pkPlot );
	}
}

void CvDLLEntity::ExecuteMove( float fTimeToExecute, bool bCombat )
{
	if ( CvUnit::isRealEntity(getEntity()) && static_cast<CvUnit*>(this)->isInViewport() )
	{
		gDLL->getEntityIFace()->ExecuteMove(getUnitEntity(), fTimeToExecute, bCombat );
	}
}

void CvDLLEntity::SetPosition( const CvPlot * pkPlot )
{
	if ( CvUnit::isRealEntity(getEntity()) )
	{
		gDLL->getEntityIFace()->SetPosition(getUnitEntity(), pkPlot );
	}
}

void CvDLLEntity::NotifyEntity( MissionTypes eMission )
{
	if ( CvUnit::isRealEntity(getEntity()) )
	{
		gDLL->getEntityIFace()->NotifyEntity( getUnitEntity(), eMission );
	}
}

void CvDLLEntity::SetSiegeTower(bool show)
{
	if ( CvUnit::isRealEntity(getEntity()) )
	{
		gDLL->getEntityIFace()->SetSiegeTower( getUnitEntity(), show );
	}
}

bool CvDLLEntity::GetSiegeTower()
{
	if ( CvUnit::isRealEntity(getEntity()) )
	{
		return gDLL->getEntityIFace()->GetSiegeTower(getUnitEntity());
	}
	else
	{
		return false;
	}
}
