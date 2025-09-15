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
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvUnitSelectionCriteria.h"


#define PROPERTY_MASK 0x0F
#define ADVISOR_MASK 0x07

class CvUnitSelectionCriteria
{
public:
	CvUnitSelectionCriteria()
		: m_eUnitAI(NO_UNITAI)
		, m_eIgnoreAdvisor(NO_ADVISOR)
		, m_eHealUnitCombat(NO_UNITCOMBAT)
		, m_eProperty(NO_PROPERTY)
		, m_eVisibility(NO_INVISIBLE)
		, m_bIgnoreNotUnitAIs(false)
		, m_bIgnoreGrowth(false)
		, m_bPropertyBeneficial(true)
		, m_bNoNegativeProperties(false)
		, m_bIsHealer(false)
		, m_bIsCommander(false)
		, m_bIsCommodore(false)
	{ }

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
	bool			m_bIsCommodore;

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
	CvUnitSelectionCriteria& IsCommodore(bool IsCommodore) { m_bIsCommodore = IsCommodore; return *this; }

	CvString getDescription() const;

	int getHash() const
	{
		int	iResult = (m_eUnitAI << 16);

		if (m_eProperty != NO_PROPERTY)
		{
			iResult |= 0x8000;
			iResult |= (m_eProperty & PROPERTY_MASK) << 11;
			if (!m_bPropertyBeneficial)
			{
				iResult |= 0x400;
			}
		}
		if (m_bIgnoreNotUnitAIs)
		{
			iResult |= 0x200;
		}
		if (m_bIgnoreGrowth)
		{
			iResult |= 0x100;
		}
		iResult |= (m_eIgnoreAdvisor & ADVISOR_MASK) << 5;
		if (m_bIsHealer)
		{
			iResult |= 0x10;
		}
		if (m_bIsCommander)
		{
			iResult |= 0x8;
		}
		if (m_bIsCommodore)
        {
        	iResult |= 0x8;
        }

		return iResult;
	}
};

#endif