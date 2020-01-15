#pragma once

#include "UnitVisibilityBase.h"

struct UnitVisibilityHideAndSeek : UnitVisibilityBase
{
	virtual CvWString getToolTipText() const; // override

private:
	int* m_aiExtraVisibilityIntensity;
	int* m_aiExtraInvisibilityIntensity;
	int* m_aiExtraVisibilityIntensityRange;
	int* m_aiNegatesInvisibleCount;
	int* m_aiExtraVisibilityIntensitySameTile;
};