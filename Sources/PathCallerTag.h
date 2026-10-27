#pragma once

#ifndef PATH_CALLER_TAG_H
#define PATH_CALLER_TAG_H

// Lightweight caller attribution for CvPathGenerator::generatePath (main thread only).
//
// Tagged functions push their __FUNCTION__ name onto a small global stack. When an
// outermost generatePath completes, its time is credited to (outermost tag, innermost tag)
// for the player whose turn is currently being timed. Results are written per player-turn
// to PathCallerTiming.log (see CvPlayer.cpp). Push/pop is just a pointer store, so the
// overhead on hot functions is negligible.

#define PATH_CALLER_TAG_MAX_DEPTH 64

extern const char* g_apPathCallerTagStack[PATH_CALLER_TAG_MAX_DEPTH];
extern int g_iPathCallerTagDepth;
void notePathCallerOuterEntry(const char* szTag);

struct PathCallerTagScope
{
	explicit PathCallerTagScope(const char* szTag)
	{
		if (g_iPathCallerTagDepth == 0)
		{
			notePathCallerOuterEntry(szTag);
		}
		if (g_iPathCallerTagDepth < PATH_CALLER_TAG_MAX_DEPTH)
		{
			g_apPathCallerTagStack[g_iPathCallerTagDepth] = szTag;
		}
		++g_iPathCallerTagDepth;
	}
	~PathCallerTagScope()
	{
		--g_iPathCallerTagDepth;
	}
};

#define PATH_CALLER_TAG_CONCAT_DETAIL(x,y) x##y
#define PATH_CALLER_TAG_CONCAT(x,y) PATH_CALLER_TAG_CONCAT_DETAIL(x,y)
#define PATH_CALLER_TAG() PathCallerTagScope PATH_CALLER_TAG_CONCAT(pathCallerTag_,__LINE__)(__FUNCTION__);

#endif
