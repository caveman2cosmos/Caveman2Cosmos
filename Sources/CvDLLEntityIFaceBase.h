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
	virtual void removeEntity(CvEntity*)  { FErrorMsg("can't get here"); }
	virtual void addEntity(CvEntity*, uint32_t /*uiEntAddFlags*/)  { FErrorMsg("can't get here"); }
	virtual void setup(CvEntity*)  { FErrorMsg("can't get here"); }
	virtual void setVisible(CvEntity*, bool)  { FErrorMsg("can't get here"); }
	virtual void createCityEntity(CvCity*)  { FErrorMsg("can't get here"); }
	virtual void createUnitEntity(CvUnit*)  { FErrorMsg("can't get here"); }
	virtual void destroyEntity(CvEntity*&, bool /*bSafeDelete*/=true)  { FErrorMsg("can't get here"); }
	virtual void updatePosition(CvEntity*)  { FErrorMsg("can't get here"); }
	virtual void setupFloodPlains(CvRiver*) { FErrorMsg("can't get here"); }

	virtual bool IsSelected(const CvEntity*)  const { return false; }
	virtual void PlayAnimation(CvEntity*, AnimationTypes, float /*fSpeed*/ = 1.0f, bool /*bQueue*/ = false, int /*iLayer*/ = 0, float /*fStartPct*/ = 0.0f, float /*fEndPct*/ = 1.0f)  { FErrorMsg("can't get here"); }
	virtual void StopAnimation(CvEntity*, AnimationTypes)  { FErrorMsg("can't get here"); }
	virtual void StopAnimation(CvEntity * ) { FErrorMsg("can't get here");}
	virtual void NotifyEntity(CvUnitEntity*, MissionTypes) { FErrorMsg("can't get here"); }
	virtual void MoveTo(CvUnitEntity*, const CvPlot*)  { FErrorMsg("can't get here"); }
	virtual void QueueMove(CvUnitEntity*, const CvPlot*)  { FErrorMsg("can't get here"); }
	virtual void ExecuteMove(CvUnitEntity*, float /*fTimeToExecute*/, bool /*bCombat*/)  { FErrorMsg("can't get here"); }
	virtual void SetPosition(CvUnitEntity*, const CvPlot*)  { FErrorMsg("can't get here"); }
	virtual void AddMission(const CvMissionDefinition*) { FErrorMsg("can't get here"); };
	// Remove this unit from any active mission
	virtual void RemoveUnitFromBattle(CvUnit*) { FErrorMsg("can't get here"); };
	virtual void showPromotionGlow(CvUnitEntity*, bool /*show*/) { FErrorMsg("can't get here"); };
	virtual void updateEnemyGlow(CvUnitEntity*) { FErrorMsg("can't get here"); };
	virtual void updatePromotionLayers(CvUnitEntity*) { FErrorMsg("can't get here"); };
	virtual void updateGraphicEra(CvUnitEntity*, EraTypes /*eOldEra*/ = NO_ERA) { FErrorMsg("can't get here"); };
	virtual void SetSiegeTower(CvUnitEntity*, bool /*show*/) { FErrorMsg("can't get here"); };
	virtual bool GetSiegeTower(CvUnitEntity*) { FErrorMsg("can't get here"); return false; };
};

#endif	// CvDLLEntityIFaceBase_h
