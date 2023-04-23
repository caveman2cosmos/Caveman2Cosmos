#include "FProfiler.h"

#include "CvDLLUtilityIFaceBase.h"
#include "CvGlobals.h"	// for gDLL


CProfileScope::CProfileScope(ProfileSample* pSample)
{
	bValid = true;
#ifdef USE_INTERNAL_PROFILER
	m_linkage.sample = pSample;
	IFPBeginSample(&m_linkage);
#else
	m_pSample = pSample;
	gDLL->BeginSample(m_pSample);
#endif
}

CProfileScope::~CProfileScope()
{
	if (bValid)
	{
#ifdef USE_INTERNAL_PROFILER
		IFPEndSample(&m_linkage);
#else
		gDLL->EndSample(m_pSample);
#endif
		bValid = false;
	}
}

void gDllBeginSample(ProfileSample* pSample) { gDLL->BeginSample(pSample); }
void gDllEndSample(ProfileSample* pSample) { gDLL->EndSample(pSample); }