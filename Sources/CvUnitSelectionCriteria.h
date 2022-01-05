#pragma once

#ifndef CIV4_UNIT_SELECTION_CRITERIA_H
#define CIV4_UNIT_SELECTION_CRITERIA_H

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvUnitSelectionCriteria.h
//
//  PURPOSE: Unit ordering criteria
//
//------------------------------------------------------------------------------------------------

#include "CvEnums.h"
#include "CvString.h"

class CvUnitSelectionCriteria
{
public:
	CvUnitSelectionCriteria();

	CvString getDescription() const;

	int getHash() const;

	UnitAITypes		m_eUnitAI;
	AdvisorTypes	m_eIgnoreAdvisor;
	UnitCombatTypes	m_eHealUnitCombat;
	PropertyTypes	m_eProperty;
	InvisibleTypes	m_eVisibility;
	bool			m_bIgnoreNotUnitAIs;
	bool			m_bIgnoreGrowth;
	bool			m_bPropertyBeneficial;
	bool			m_bNoNegativeProperties;
	bool			m_bIsHealer;
	bool			m_bIsCommander;

	CvUnitSelectionCriteria& UnitAI(UnitAITypes UnitAI) { m_eUnitAI = UnitAI; return *this; }
	CvUnitSelectionCriteria& IgnoreAdvisor(AdvisorTypes IgnoreAdvisor) { m_eIgnoreAdvisor = IgnoreAdvisor; return *this; }
	CvUnitSelectionCriteria& HealUnitCombat(UnitCombatTypes HealUnitCombat) { m_eHealUnitCombat = HealUnitCombat; return *this; }
	CvUnitSelectionCriteria& Property(PropertyTypes Property) { m_eProperty = Property; return *this; }
	CvUnitSelectionCriteria& Visibility(InvisibleTypes Visibility) { m_eVisibility = Visibility; return *this; }
	CvUnitSelectionCriteria& IgnoreNotUnitAIs(bool IgnoreNotUnitAIs) { m_bIgnoreNotUnitAIs = IgnoreNotUnitAIs; return *this; }
	CvUnitSelectionCriteria& IgnoreGrowth(bool IgnoreGrowth) { m_bIgnoreGrowth = IgnoreGrowth; return *this; }
	CvUnitSelectionCriteria& PropertyBeneficial(bool PropertyBeneficial) { m_bPropertyBeneficial = PropertyBeneficial; return *this; }
	CvUnitSelectionCriteria& NoNegativeProperties(bool NoNegativeProperties) { m_bNoNegativeProperties = NoNegativeProperties; return *this; }
	CvUnitSelectionCriteria& IsHealer(bool IsHealer) { m_bIsHealer = IsHealer; return *this; }
	CvUnitSelectionCriteria& IsCommander(bool IsCommander) { m_bIsCommander = IsCommander; return *this; }
};

#endif