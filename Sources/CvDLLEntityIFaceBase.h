#pragma once

#ifndef CvDLLEntityIFaceBase_h
#define CvDLLEntityIFaceBase_h

//#pragma warning( disable: 4100 ) // unreferenced formal parameter

//
// abstract class containing entity-related functions that the DLL needs
//
class CvEntity;
class CvUnitEntity;
class CvCity;
class CvUnit;
class CvMissionDefinition;
class CvPlot;
class CvRiver;

class CvDLLEntityIFaceBase
{
public:
	virtual void removeEntity(CvEntity*) = 0;
	virtual void addEntity(CvEntity*, uint32_t /*uiEntAddFlags*/) = 0;
	virtual void setup(CvEntity*) = 0;
	virtual void setVisible(CvEntity*, bool) = 0;
	virtual void createCityEntity(CvCity*) = 0;
	virtual void createUnitEntity(CvUnit*) = 0;
	virtual void destroyEntity(CvEntity*&, bool /*bSafeDelete*/=true) = 0;
	virtual void updatePosition(CvEntity*) = 0;
	virtual void setupFloodPlains(CvRiver*) = 0;

	virtual bool IsSelected(const CvEntity*) const = 0;
	virtual void PlayAnimation(CvEntity*, AnimationTypes, float /*fSpeed*/ = 1.0f, bool /*bQueue*/ = false, int /*iLayer*/ = 0, float /*fStartPct*/ = 0.0f, float /*fEndPct*/ = 1.0f) = 0;
	virtual void StopAnimation(CvEntity*, AnimationTypes) = 0;
	virtual void StopAnimation(CvEntity*) = 0;
	virtual void NotifyEntity(CvUnitEntity*, MissionTypes) = 0;
	virtual void MoveTo(CvUnitEntity*, const CvPlot*) = 0;
	virtual void QueueMove(CvUnitEntity*, const CvPlot*) = 0;
	virtual void ExecuteMove(CvUnitEntity*, float /*fTimeToExecute*/, bool /*bCombat*/) = 0;
	virtual void SetPosition(CvUnitEntity*, const CvPlot*) = 0;
	virtual void AddMission(const CvMissionDefinition*) = 0;
	virtual void RemoveUnitFromBattle(CvUnit*) = 0; // Remove this unit from any active mission
	virtual void showPromotionGlow(CvUnitEntity*, bool /*show*/) = 0;
	virtual void updateEnemyGlow(CvUnitEntity*) = 0;
	virtual void updatePromotionLayers(CvUnitEntity*) = 0;
	virtual void updateGraphicEra(CvUnitEntity*, EraTypes /*eOldEra*/ = NO_ERA) = 0;
	virtual void SetSiegeTower(CvUnitEntity*, bool /*show*/) = 0;
	virtual bool GetSiegeTower(CvUnitEntity*) = 0;
};

#endif	// CvDLLEntityIFaceBase_h
