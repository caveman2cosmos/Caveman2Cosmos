#pragma once

#include "UnitVisibilityBase.h"

struct UnitVisibilityDefault : UnitVisibilityBase
{
	virtual CvWString getToolTipText() const; // override

private:
	InvisibleTypes getInvisibleType() const; // Exposed to Python
};