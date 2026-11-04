#pragma once

// These roles alone may accept a unit without the positive XML UnitAI flag.
// Keep this in step with AI_unitValue's bUndefinedValid branches.
inline bool unitAIRequiresXMLAssignment(UnitAITypes role)
{
	switch (role)
	{
	case UNITAI_UNKNOWN:
	case UNITAI_HUNTER:
	case UNITAI_HUNTER_ESCORT:
	case UNITAI_ESCORT:
	case UNITAI_EXPLORE:
		return false;
	default:
		return true;
	}
}
