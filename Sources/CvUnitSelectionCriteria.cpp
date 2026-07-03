//------------------------------------------------------------------------------------------------
//
//  FILE:    CvUnitSelectionCriteria.pp
//
//  PURPOSE: Unit ordering criteria
//
//------------------------------------------------------------------------------------------------

#include "CvEnums.h"
#include "CvString.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvUnitSelectionCriteria.h"


	CvString CvUnitSelectionCriteria::getDescription() const
	{
		CvWStringBuffer buffer;

		// UnitAI
		if (m_eUnitAI != NO_UNITAI)
		{
			buffer.append(CvWString::format(L"UnitAI=%s; ",
				GC.getUnitAIInfo(m_eUnitAI).getDescription()));
		}

		// Advisor
		if (m_eIgnoreAdvisor != NO_ADVISOR)
		{
			buffer.append(CvWString::format(L"IgnoreAdvisor=%s; ",
				GC.getAdvisorInfo(m_eIgnoreAdvisor).getDescription()));
		}

		// Heal combat type
		if (m_eHealUnitCombat != NO_UNITCOMBAT)
		{
			buffer.append(CvWString::format(L"HealUnitCombat=%s; ",
				GC.getUnitCombatInfo(m_eHealUnitCombat).getDescription()));
		}

		// Property
		if (m_eProperty != NO_PROPERTY)
		{
			buffer.append(CvWString::format(L"Property=%s %s; ",
				m_bPropertyBeneficial ? L"improver" : L"worsener",
				GC.getPropertyInfo(m_eProperty).getDescription()));
		}

		// Visibility
		if (m_eVisibility != NO_INVISIBLE)
		{
			buffer.append(CvWString::format(L"Visibility=%s; ",
				GC.getInvisibleInfo(m_eVisibility).getDescription()));
		}

		// Flags booléens
		if (m_bIgnoreNotUnitAIs)   buffer.append(L"IgnoreNotUnitAIs; ");
		if (m_bIgnoreGrowth)       buffer.append(L"IgnoreGrowth; ");
		if (!m_bPropertyBeneficial) buffer.append(L"PropertyNotBeneficial; ");
		if (m_bNoNegativeProperties) buffer.append(L"NoNegativeProperties; ");
		if (m_bIsHealer)           buffer.append(L"Healer; ");
		if (m_bIsCommander)        buffer.append(L"Commander; ");
		if (m_bIsCommodore)        buffer.append(L"Commodore; ");

		// Si rien n'a été rempli → retour vide
		if (buffer.isEmpty())
			return CvString("EmptyCriteria");

		return CvString(buffer.getCString());
	}


