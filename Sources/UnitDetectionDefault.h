#pragma once

#include "UnitDetectionBase.h"

struct UnitDetectionDefault : UnitDetectionBase
{
	virtual CvWString getToolTipText() const; // override

private:
	int getNumSeeInvisibleTypes() const; // Exposed to Python
	InvisibleTypes getSeeInvisibleType(int i) const; // Exposed to Python
};