#pragma once

#ifndef CvGameCoreDLL_h
#define CvGameCoreDLL_h

//
// includes (pch) for gamecore dll files
// Author - Mustafa Thamer
//

//
// WINDOWS
//
#pragma warning( disable: 4530 )	// C++ exception handler used, but unwind semantics are not enabled

#define NOMINMAX
#define _WIN32_WINNT 0x0600
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <MMSystem.h>
//#if defined _DEBUG && !defined USE_MEMMANAGER
//#define USE_MEMMANAGER
//#include <crtdbg.h>
//#endif
//#if !defined USE_MEMMANAGER
//#define USE_MEMMANAGER
//#endif
#include <vector>
#include <list>
#include <tchar.h>
#include <math.h>
#include <assert.h>
#include <map>
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#include <hash_map>
#include <stdio.h>
#include <utility>
#include <algorithm>
#include <set>
#include <fstream>

#define DllExport   __declspec( dllexport ) 

//
// GameBryo
//
class NiColor
{
public:
	float r, g, b;
};
class NiColorA 
{
public:
	NiColorA(float fr, float fg, float fb, float fa) : r(fr), g(fg), b(fb), a(fa) {}
	NiColorA() {}
	float r, g, b, a;
};
class NiPoint2
{
public:
	NiPoint2() {}
	NiPoint2(float fx, float fy) : x(fx),y(fy) {}

	float x, y;
};
class NiPoint3
{
public:
	NiPoint3(): x(0.0f), y(0.0f), z(0.0f) {}
	NiPoint3(float fx, float fy, float fz) : x(fx),y(fy),z(fz) {} 

	bool operator== (const NiPoint3& pt) const
	{	return (x == pt.x && y == pt.y && z == pt.z);	}

	inline NiPoint3 operator+ (const NiPoint3& pt) const
	{	return NiPoint3(x+pt.x,y+pt.y,z+pt.z);	}

	inline NiPoint3 operator- (const NiPoint3& pt) const
	{	return NiPoint3(x-pt.x,y-pt.y,z-pt.z);	}

	inline float operator* (const NiPoint3& pt) const
	{	return x*pt.x+y*pt.y+z*pt.z;	}

	inline NiPoint3 operator* (float fScalar) const
	{	return NiPoint3(fScalar*x,fScalar*y,fScalar*z);	}

	inline NiPoint3 operator/ (float fScalar) const
	{
		float fInvScalar = 1.0f/fScalar;
		return NiPoint3(fInvScalar*x,fInvScalar*y,fInvScalar*z);
	}

	inline NiPoint3 operator- () const
	{	return NiPoint3(-x,-y,-z);	}

	inline float Length() const
	{ return sqrt(x * x + y * y + z * z); }

	inline float Unitize()
	{
		float length = Length();
		if(length != 0)
		{
			x /= length;
			y /= length;
			z /= length;
		}
		return length;
	}

//	inline NiPoint3 operator* (float fScalar, const NiPoint3& pt)
//	{	return NiPoint3(fScalar*pt.x,fScalar*pt.y,fScalar*pt.z);	}
	float x, y, z;
};

namespace NiAnimationKey
{
	enum KeyType
	{
		NOINTERP,
		LINKEY,
		BEZKEY,
		TCBKEY,
		EULERKEY,
		STEPKEY,
		NUMKEYTYPES
	};
};

typedef unsigned char    byte;
typedef unsigned short   word;
typedef unsigned int     uint;
typedef unsigned long    dword;
typedef unsigned __int64 qword;
typedef wchar_t          wchar;

#define MAX_CHAR                            (0x7f)
#define MIN_CHAR                            (0x80)
#define MAX_SHORT                           (0x7fff)
#define MIN_SHORT                           (0x8000)
#define MAX_INT                             (0x7fffffff)
#define MIN_INT                             (0x80000000)
#define MAX_UNSIGNED_CHAR                   (0xff)
#define MIN_UNSIGNED_CHAR                   (0x00)
#define MAX_UNSIGNED_SHORT                  (0xffff)
#define MIN_UNSIGNED_SHORT                  (0x0000)
#define MAX_UNSIGNED_INT                    (0xffffffff)
#define MIN_UNSIGNED_INT                    (0x00000000)

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#define SQR(x)      ( (x) * (x) )
#define DEGTORAD(x) ( (float)( (x) * (M_PI / 180) ))
#define LIMIT_RANGE(low, value, high) value = (value < low ? low : (value > high ? high : value));
#define M_PI       3.14159265358979323846
#define fM_PI		3.141592654f		//!< Pi (float)

__forceinline DWORD FtoDW( float f ) { return *(DWORD*)&f; }
__forceinline float DWtoF( dword n ) { return *(float*)&n; }
__forceinline float MaxFloat() { return DWtoF(0x7f7fffff); }


#ifdef _DEBUG
//#define	MEMORY_TRACKING
#endif

void startProfilingDLL(bool longLived);
void stopProfilingDLL(bool longLived);
//#define USE_INTERNAL_PROFILER
//#define FP_PROFILE_ENABLE
#ifdef USE_INTERNAL_PROFILER
extern bool g_bTraceBackgroundThreads;

struct ProfileSample;
struct ProfileLinkageInfo;
void IFPProfileThread();
bool IFPIsMainThread();
void IFPBeginSample(ProfileLinkageInfo* linkageInfo, bool bAsConditional = false);
void IFPEndSample(ProfileLinkageInfo* linkageInfo, bool bAsConditional = false);
void IFPCancelSample(ProfileLinkageInfo* linkageInfo);
void dumpProfileStack();
void EnableDetailedTrace(bool enable);
void IFPSetCount(ProfileSample* sample, int count);
#endif

#ifdef MEMORY_TRACKING
class CMemoryTrack
{
#define	MAX_TRACKED_ALLOCS	1000
	void*	m_track[MAX_TRACKED_ALLOCS];
	char*	m_trackName[MAX_TRACKED_ALLOCS];
	int		m_allocSeq[MAX_TRACKED_ALLOCS];
	int		m_allocSize[MAX_TRACKED_ALLOCS];
	int		m_highWater;
	const char* m_name;
	bool	m_valid;
	int		m_seq;
#define MAX_TRACK_DEPTH		50
	static	CMemoryTrack*	trackStack[MAX_TRACK_DEPTH];
	static	int m_trackStackDepth;

public:
	CMemoryTrack(const char* name, bool valid);

	~CMemoryTrack();

	void NoteAlloc(void* ptr, int size);
	void NoteDeAlloc(void* ptr);

	static CMemoryTrack* GetCurrent();
};

class CMemoryTrace
{
	SIZE_T				m_start;
	const char*			m_name;

public:
	CMemoryTrace(const char* name);

	~CMemoryTrace();
};

void DumpMemUsage(const char* fn, int line);

#define DUMP_MEMORY_USAGE()	DumpMemUsage(__FUNCTION__,__LINE__);
#define MEMORY_TRACK()	CMemoryTrack __memoryTrack(__FUNCTION__, true);
#define MEMORY_TRACK_NAME(x)	CMemoryTrack __memoryTrack(x, true);
#define MEMORY_TRACK_EXEMPT()	CMemoryTrack __memoryTrackExemption(NULL, false);
#define MEMORY_TRACE_FUNCTION()	CMemoryTrace __memoryTrace(__FUNCTION__);
#else
#define DUMP_MEMORY_USAGE()	
#define	MEMORY_TRACK()
#define MEMORY_TRACK_EXEMPT()
#define MEMORY_TRACE_FUNCTION()
#define MEMORY_TRACK_NAME(x)
#endif

// Python
#ifdef _DEBUG
  #undef _DEBUG
  #include "Python.h"
  #define _DEBUG
#else
  #include "Python.h"
#endif

//
// Boost Python
//
#include <boost/python/list.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/class.hpp>
#include <boost/python/manage_new_object.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/python/object.hpp>
#include <boost/python/def.hpp>

namespace python = boost::python;

#include "FAssert.h"
#include "CheckSum.h"
#include "CvGameCoreDLLDefNew.h"
#include "FDataStreamBase.h"
#include "FFreeListArrayBase.h"
#include "FFreeListTrashArray.h"
#include "FFreeListArray.h"
#include "FVariableSystem.h"
#include "FAStarNode.h"
#include "CvString.h"
#include "CvEnums.h"
#include "CvStructs.h"
#include "CvDLLUtilityIFaceBase.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvDLLFAStarIFaceBase.h"
#include "CvDLLPythonIFaceBase.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLXMLIFaceBase.h"

//jason tests
#include "CvGameCoreUtils.h"
#include "CvXMLLoadUtility.h"
#include "CvInitCore.h"
#include "CvArtFileMgr.h"
#include "CvGameTextMgr.h"
#include "CvBugOptions.h"
#include "CvPopupInfo.h"
#include "CvEventReporter.h"
#include "CvMessageControl.h"
#include "CvDiploParameters.h"
#include "CvDeal.h"
#include "CvInfoWater.h"
#include "CvMap.h"
#include "CvViewport.h"
#include "CvGlobals.h"
#include "CvGameAI.h"
#include "CvTeamAI.h"
#include "CvPlayerAI.h"
#include "CvCityAI.h"
#include "CvUnitAI.h"
#include "CvSelectionGroupAI.h"
#include "CvPlot.h"
#include "CvTalkingHeadMessage.h"
#include "FProfiler.h"
#include "CvPathGenerator.h"
#include "CvBugOptions.h"

#include "CyGlobalContext.h"
#include "CyArtFileMgr.h"
#include "CyDeal.h"
#include "CyMap.h"
#include "CyArea.h"
#include "CyGame.h"
#include "CyTeam.h"
#include "CyPlayer.h"
#include "CyCity.h"
#include "CyUnit.h"
#include "CySelectionGroup.h"
#include "CyPlot.h"

#ifdef FINAL_RELEASE
// Undefine OutputDebugString in final release builds
#undef OutputDebugString
#define OutputDebugString(x)
#endif //FINAL_RELEASE

#endif	// CvGameCoreDLL_h
