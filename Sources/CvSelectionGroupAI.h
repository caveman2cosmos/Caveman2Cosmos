#pragma once

#ifndef CIV4_SELECTION_GROUP_AI_H
#define CIV4_SELECTION_GROUP_AI_H

#include "CvSelectionGroup.h"

class CvCity;
class CvPlot;
class CvSelectionGroup;
class CvUnit;

class CvSelectionGroupAI : public CvSelectionGroup
{
public:
	DllExport CvSelectionGroupAI();
	DllExport virtual ~CvSelectionGroupAI();

	void AI_reset();

	void AI_separate();
	// void AI_separateIf(bst::function<bool(CvUnit*)> predicateFn);
	void AI_separateNonAI(UnitAITypes eUnitAI);
	void AI_separateAI(UnitAITypes eUnitAI);
	void AI_separateImpassable();
	void AI_separateEmptyTransports();

	bool AI_update();

	int AI_attackOdds(const CvPlot* pPlot, bool bPotentialEnemy, bool bForce = false, bool* bWin = NULL, int iTheshold = -1) const;

	CvUnit* AI_getBestGroupAttacker
	(
		const CvPlot* pPlot,
		bool bPotentialEnemy,
		int& iUnitOdds,
		bool bForce = false,
		CvUnit** pDefender = NULL,
		bool bAssassinate = false,
		bool bSuprise = false,
		bool bIgnoreMadeAttack = false,
		const std::set<int>& ignoreUnitID = std::set<int>()
	)
	const;

	CvUnit* AI_getBestGroupSacrifice(const CvPlot* pPlot, bool bForce, bool bNoBlitz) const;

	int AI_compareStacks(const CvPlot* pPlot, StackCompare::flags flags = StackCompare::None, int iRange = 0) const; // override
	int AI_sumStrength(const CvPlot* pAttackedPlot = NULL, DomainTypes eDomainType = NO_DOMAIN, StackCompare::flags flags = StackCompare::None) const;
	void AI_queueGroupAttack(int iX, int iY);
	void AI_cancelGroupAttack();
	bool AI_isGroupAttack() const;

	bool AI_isControlled() const;
	bool AI_isDeclareWar(const CvPlot* pPlot = NULL) const;

	CvPlot* AI_getMissionAIPlot() const;

	bool AI_isForceSeparate() const;
	void AI_makeForceSeparate();

	bool AI_isCityGarrison(const CvCity* pCity) const;
	void AI_setAsGarrison(const CvCity* pCity);

	MissionAITypes AI_getMissionAIType() const;
	void AI_setMissionAI(MissionAITypes eNewMissionAI, const CvPlot* pNewPlot, const CvUnit* pNewUnit);
	void AI_noteSizeChange(int iChange, int iVolume);
	CvUnit* AI_findBestDefender(const CvPlot* pTargetPlot, bool allowAllDefenders, bool bConsiderPropertyValues = false) const;
	CvUnit* AI_ejectBestDefender(const CvPlot* pTargetPlot, bool allowAllDefenders);
	virtual bool AI_hasBeneficialPropertyEffectForCity(const CvCity* pCity, PropertyTypes pProperty) const;
	virtual CvUnit* AI_ejectBestPropertyManipulator(const CvCity* pTargetCity);
	virtual int AI_getGenericValueTimes100(UnitValueFlags eFlags) const;

	CvUnit* AI_getMissionAIUnit() const;

	bool AI_isFull() const;

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);

protected:
	bool AI_isAwaitingContract() const;

	int m_iMissionAIX;
	int m_iMissionAIY;

	MissionAITypes m_eMissionAIType;

	IDInfo m_missionAIUnit;

	bool m_bForceSeparate;
	bool m_bGroupAttack;
	int m_iGroupAttackX;
	int m_iGroupAttackY;
};

#endif
