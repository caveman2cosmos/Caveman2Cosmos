#include "FProfiler.h"

#include "CvDLLUtilityIFaceBase.h"
#include "CvGlobals.h"	// for gDLL

void gDllBeginSample(ProfileSample* pSample) { gDLL->BeginSample(pSample); }
void gDllEndSample(ProfileSample* pSample) { gDLL->EndSample(pSample); }