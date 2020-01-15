#pragma once

#include "ComponentBase.h"

struct UnitComponentBase : ComponentBase
{
	UnitComponentBase(CvUnit* owner) : m_owner(owner) {}

	const CvUnit* getOwner() const { return m_owner; }
	CvUnit* getOwner() { return m_owner; }

	virtual CvWString getToolTipText() const = 0;

private:
	CvUnit* m_owner;
};