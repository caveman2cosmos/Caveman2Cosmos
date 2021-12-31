// ---------------------------------------------------------------------------------------------------
// FProfiler - DLL wrapper to FireEngine/FProfiler.cpp
//
// Author: tomw
//---------------------------------------------------------------------------------------------------------------------
#pragma once

#ifndef	__PROFILE_H__
#define __PROFILE_H__


//#include "CvDLLEntityIFaceBase.h"
#include "CvDLLUtilityIFaceBase.h"
#include "CvGlobals.h"	// for gDLL

#define	MAX_PROFILED_THREADS	8

//NOTE: This struct must be identical ot the same struct in  FireEngine/FProfiler.h if the
//standard profiler is being used (USE_INTERNAL_PROFILER not defined)
//---------------------------------------------------------------------------------------------------------------------
struct ProfileSample
{
	// cppcheck-suppress uninitMemberVar
	explicit ProfileSample(char *name) :
#ifdef USE_INTERNAL_PROFILER
		Id(-1),
#else

#endif
		Added(false),
		Parent(-1)
	{
		strcpy(Name, name);
	}

	char	Name[256];						// Name of sample;

#ifdef USE_INTERNAL_PROFILER
	int				ProfileInstances[MAX_PROFILED_THREADS+1];		// # of times ProfileBegin Called
	LONG			EntryCount[MAX_PROFILED_THREADS+1];
	//LARGE_INTEGER	StartTime;				// The current open profile start time
	LARGE_INTEGER	MainThreadAccumulator[MAX_PROFILED_THREADS+1];// All samples this frame added together for the main thread only
	LARGE_INTEGER	Accumulator[MAX_PROFILED_THREADS+1];			// All samples this frame added together

	LARGE_INTEGER	ChildrenSampleTime[MAX_PROFILED_THREADS+1];		// Time taken by all children
	LARGE_INTEGER	AlternateSampleSetTime[MAX_PROFILED_THREADS+1];	// Time taken by all descendants flagged as contributing to altrernate set
#else
	int				ProfileInstances;		// # of times ProfileBegin Called
	int				OpenProfiles;			// # of time ProfileBegin called w/o ProfileEnd
	double			StartTime;				// The current open profile start time
	double			Accumulator;			// All samples this frame added together

	double			ChildrenSampleTime;		// Time taken by all children
#endif
	unsigned int	NumParents;				// Number of profile Parents
	bool			Added;					// true when added to the list
	int				Parent;
#ifdef USE_INTERNAL_PROFILER
	int				Id;
	bool			IsInAlternateSet;
#endif
};

struct ProfileLinkageInfo
{
	LARGE_INTEGER	startTime;
	ProfileSample*	sample;
	struct ProfileLinkageInfo* parent;
	bool			bIsEntry;
};

//---------------------------------------------------------------------------------------------------------------------
// Allows us to Profile based on Scope, to limit intrusion into code.
// Simply use PROFLE("funcname") instead having to insert begin()/end() pairing
class CProfileScope
{
public:
	// cppcheck-suppress uninitMemberVar
	CProfileScope() { bValid= false;};
	explicit CProfileScope(ProfileSample *pSample)
	{
		bValid = true;
#ifdef USE_INTERNAL_PROFILER
		m_linkage.sample = pSample;
		IFPBeginSample(&m_linkage);
#else
		m_pSample = pSample;
		gDLL->BeginSample(m_pSample);
#endif
	};
	~CProfileScope()
	{
		if(bValid )
		{
#ifdef USE_INTERNAL_PROFILER
			IFPEndSample(&m_linkage);
#else
			gDLL->EndSample(m_pSample);
#endif
			bValid = false;
		}
	};

private:
	bool bValid;
#ifdef USE_INTERNAL_PROFILER
	ProfileLinkageInfo m_linkage;
#else
	ProfileSample*	m_pSample;
#endif
};

//---------------------------------------------------------------------------------------------------------------------

// Main Interface for Profile
#ifdef FP_PROFILE_ENABLE				// Turn Profiling On or Off ..
#ifdef USE_INTERNAL_PROFILER
#define PROFILE_THREAD(name)	\
	static ProfileSample __rootSample(name);\
	IFPProfileThread();\
	CProfileScope ProfileScope(&__rootSample);

#define PROFILE(name)\
	static ProfileSample sample(name);\
	CProfileScope ProfileScope(&sample);

//BEGIN & END macros:		Only needed if you don't want to use the scope macro above.
// Macros must be in the same scope
#define PROFILE_BEGIN(name)\
	static ProfileSample sample__(name);\
	ProfileLinkageInfo sample__linkage;\
	sample__linkage.sample = &sample__;\
	IFPBeginSample(&sample__linkage);
#define PROFILE_END()\
	IFPEndSample(&sample__linkage);

#define PROFILE_BEGIN_CONDITIONAL(name1,name2)\
	static ProfileSample sample1__(name1);\
	static ProfileSample sample2__(name2);\
	ProfileLinkageInfo sample__linkage1;\
	ProfileLinkageInfo sample__linkage2;\
	sample__linkage1.sample = &sample1__;\
	sample__linkage2.sample = &sample2__;\
	IFPBeginSample(&sample__linkage1,true);\
	IFPBeginSample(&sample__linkage2,true);
#define PROFILE_END_CONDITIONAL(result)\
	if(result)\
		{IFPCancelSample(&sample__linkage2);IFPEndSample(&sample__linkage1,true);}\
	else\
		{IFPCancelSample(&sample__linkage1);IFPEndSample(&sample__linkage2,true);}

#if 0
#define PROFILE_DEFINE_MULTI(name)\
	static ProfileSample sample__##name##(#name);
#define PROFILE_BEGIN_MULTI(name)\
	IFPBeginSample(&sample__##name##);
#define PROFILE_END_MULTI(name)\
	IFPEndSample(&sample__##name##);
#endif

#define PROFILE_FUNC()\
	static ProfileSample sample(__FUNCTION__);\
	CProfileScope ProfileScope(&sample);

#define PROFILE_STACK_DUMP	dumpProfileStack();
#define PROFILE_SET_COUNT(name,value)\
	static ProfileSample sample__##name##(#name);\
	IFPSetCount(&sample__##name##,value);

#else
#define PROFILE_SET_COUNT(name,value)	;
#define PROFILE(name)\
	static ProfileSample sample(name);\
	CProfileScope ProfileScope(&sample);

//BEGIN & END macros:		Only needed if you don't want to use the scope macro above.
// Macros must be in the same scope
#define PROFILE_BEGIN(name)\
	static ProfileSample sample__(name);\
	gDLL->BeginSample(&sample__);
#define PROFILE_END()\
	gDLL->EndSample(&sample__);

#define PROFILE_BEGIN_CONDITIONAL(name1,name2)
#define PROFILE_END_CONDITIONAL(result)

#define PROFILE_FUNC()\
	static ProfileSample sample(__FUNCTION__);\
	CProfileScope ProfileScope(&sample);

#define PROFILE_STACK_DUMP ;
#endif
#else
// Remove profiling code
#define PROFILE_THREAD(name)
#define PROFILE(name)
#define PROFILE_BEGIN(name)
#define PROFILE_BEGIN_CONDITIONAL(name1,name2)
#define PROFILE_END_CONDITIONAL(result)
#define PROFILE_SET_COUNT(name,value)	;
#define PROFILE_END()
#define PROFILE_FUNC()
#define PROFILE_STACK_DUMP ;
#endif


#endif //__PROFILE_H__


