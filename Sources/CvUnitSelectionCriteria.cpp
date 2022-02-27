//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvUnitSelectionCriteria.h
//
//  PURPOSE: Unit ordering criteria
//
//------------------------------------------------------------------------------------------------
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvUnitSelectionCriteria.h"

CvUnitSelectionCriteria::CvUnitSelectionCriteria()
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
{ }

CvString CvUnitSelectionCriteria::getDescription() const
{
	if (m_eProperty != NO_PROPERTY)
	{
		return (
			CvString::format(
				"Property %s for %S",
				m_bPropertyBeneficial ? "improver" : "worsener",
				GC.getPropertyInfo(m_eProperty).getDescription()
			)
		);
	}
	return CvString("");
}

#define PROPERTY_MASK 0x0F
#define ADVISOR_MASK 0x07

int CvUnitSelectionCriteria::getHash() const
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

	return iResult;
}