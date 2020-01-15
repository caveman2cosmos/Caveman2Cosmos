#include "CvGameCoreDLL.h"
#include "UnitVisibilityDefault.h"

CvWString UnitVisibilityDefault::getToolTipText() const
{
	FAssertMsg(getInvisibleType() != NO_INVISIBLE, "Always visible unit has an visibility component");
	return gDLL->getText("TXT_KEY_UNIT_INVISIBLE_MOST");
}

