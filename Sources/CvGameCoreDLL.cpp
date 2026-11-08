#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include <psapi.h>

#ifdef USE_INTERNAL_PROFILER
static CRITICAL_SECTION cSampleSection;
#endif

std::string modDir;

// BUG - EXE/DLL Paths - start
HANDLE dllModule = NULL;

bool runProcess(const std::string& exe, const std::string& workingDir)
{
	STARTUPINFOA startupInfo;
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	PROCESS_INFORMATION procInfo;
	ZeroMemory(&procInfo, sizeof(procInfo));

	bool success = false;
	// This isn't really considered safe to call in a DLLmain: it could cause other dlls to be loaded to fulfill the request.
	// HOWEVER: this DLL is loaded by LoadLibrary later in exe startup so we appear to have the required dlls already loaded at this point.
	if (::CreateProcessA(NULL, (LPSTR)exe.c_str(), NULL, NULL, TRUE, 0, NULL, workingDir.c_str(), &startupInfo, &procInfo))
	{
		success = ::WaitForSingleObject(procInfo.hProcess, 1800000) == WAIT_OBJECT_0;
	}
	::CloseHandle(procInfo.hProcess);
	::CloseHandle(procInfo.hThread);
	return success;
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// BUG - EXE/DLL Paths - end

BOOL APIENTRY DllMain(HANDLE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID /*lpReserved*/)
{
	switch( ul_reason_for_call ) {
	case DLL_PROCESS_ATTACH:
		{
		dllModule = hModule;

		// The DLL is being loaded into the virtual address space of the current process as a result of the process starting up
		OutputDebugString("[C2C] DLL_PROCESS_ATTACH\n");

#ifdef USE_INTERNAL_PROFILER
		InitializeCriticalSectionAndSpinCount(&cSampleSection, 2000);
#endif

		// set timer precision
		MMRESULT iTimeSet = timeBeginPeriod(1);		// set timeGetTime and sleep resolution to 1 ms, otherwise it's 10-16ms
		FAssertMsg(iTimeSet==TIMERR_NOERROR, "failed setting timer resolution to 1 ms");

		// Get DLL directory
		CHAR pathBuffer[MAX_PATH];
		GetModuleFileNameA((HMODULE)dllModule, pathBuffer, sizeof(pathBuffer));
		std::string dllPath = pathBuffer;
		std::string dllDir = dllPath.substr(0, dllPath.length() - strlen("CvGameCoreDLL.dll"));
		modDir = dllDir;
		std::string tokenFile = dllDir + "\\..\\git_directory.txt";
		std::ifstream stream(tokenFile.c_str());
		// If we loaded the directory token file we are in a dev environment and should run FPKLive, and check for DLL changes
		if (!stream.fail())
		{
			std::string git_dir;
			std::getline(stream, git_dir);
			modDir = git_dir;

			if(!runProcess(git_dir + "\\Tools\\FPKLive.exe", git_dir + "\\Tools"))
			{
				MessageBox(0, "Creation of FPK packs failed, are you sure you set up the development environment correctly?", "ERROR!", 0);
				return FALSE;
			}

			// Don't attempt rebuild if debugger is connected, its annoying
			if(!IsDebuggerPresent())
			{
				if (!runProcess("cmd.exe /C \"" + git_dir + "\\Tools\\_BootDLLCheck.bat\" " + TOSTRING(BUILD_TARGET), git_dir + "\\Tools"))
				{
					MessageBox(0, "DLL update failed!", "ERROR!", 0);
					return FALSE;
				}
			}
		}
		}
		break;
	case DLL_THREAD_ATTACH:
		OutputDebugString(CvString::format("[C2C] DLL_THREAD_ATTACH: %d\n", GetCurrentThreadId()).c_str());
		break;
	case DLL_THREAD_DETACH:
		OutputDebugString(CvString::format("[C2C] DLL_THREAD_DETACH: %d\n", GetCurrentThreadId()).c_str());
		break;
	case DLL_PROCESS_DETACH:
// BUG - EXE/DLL Paths - start
		dllModule = NULL;
// BUG - EXE/DLL Paths - end

		OutputDebugString("[C2C] DLL_PROCESS_DETACH\n");
		timeEndPeriod(1);
		break;
	}

	return TRUE;	// success
}

#ifdef USE_INTERNAL_PROFILER

//	Uncomment the following line to provide (very) detailed tracing to
//	the debug stream (view with DbgView or a debugger)
#define DETAILED_TRACE

#define MAX_SAMPLES				20000
static __declspec( thread ) ProfileLinkageInfo* _currentSample = NULL;
static __declspec(thread) bool bIsMainThread = false;
static __declspec(thread) int iThreadSlot = -1;
#define	RESERVED_THREAD_SLOT	MAX_PROFILED_THREADS

static bool bMainThreadSeen = false;
static int numSamples = 0;
static ProfileSample* sampleList[MAX_SAMPLES*MAX_PROFILED_THREADS];
bool g_bTraceBackgroundThreads = true;

volatile bool	bThreadSlotOccupied[MAX_PROFILED_THREADS];

//static ProfileSample* sampleStack[MAX_SAMPLES];
//static int depth = -1;

#ifdef DETAILED_TRACE
static ProfileSample* lastExit = NULL;
static int exitCount = 0;
static bool detailedTraceEnabled = false;

static void GenerateTabString(char* buffer,int n)
{
	while(n-- > 0)
	{
		*buffer++ = '\t';
	}

	*buffer = '\0';
}
#endif

void EnableDetailedTrace(bool enable)
{
#ifdef DETAILED_TRACE
	detailedTraceEnabled = enable;
#endif
}

bool IFPIsMainThread() { return bIsMainThread; }

void IFPProfileThread()
{
	if ( iThreadSlot == -1 && (g_bTraceBackgroundThreads || bIsMainThread) )
	{
		EnterCriticalSection(&cSampleSection);

		for(int iI = 0; iI < MAX_PROFILED_THREADS; iI++)
		{
			if ( !bThreadSlotOccupied[iI] )
			{
				iThreadSlot = iI;
				bThreadSlotOccupied[iI] = true;
				break;
			}
		}

		LeaveCriticalSection(&cSampleSection);
	}
}

void IFPBeginSample(ProfileLinkageInfo* linkageInfo, bool bAsConditional)
{
	ProfileSample*	sample = linkageInfo->sample;

	if ( !bMainThreadSeen )
	{
		bMainThreadSeen = true;
		bIsMainThread = true;

		for(int iI = 0; iI < MAX_PROFILED_THREADS; iI++)
		{
			bThreadSlotOccupied[iI] = false;
		}

		IFPProfileThread();
	}

	if ( iThreadSlot != -1 )
	{
		if ( sample->Id == -1 )
		{
			EnterCriticalSection(&cSampleSection);

			if ( numSamples == MAX_SAMPLES )
			{
				dumpProfileStack();
				::MessageBox(NULL,"Profile sample limit exceeded","CvGameCore",MB_OK);
				return;
			}
			sample->Parent = -1;
			sample->IsInAlternateSet = (stricmp(sample->Name,GC.alternateProfileSampleName())==0);
			sampleList[numSamples++] = sample;
			sample->Id = numSamples;

			for(int iI = 0; iI <= MAX_PROFILED_THREADS; iI++)
			{
				sample->ProfileInstances[iI] = 0;
				sample->EntryCount[iI] = 0;
				sample->Accumulator[iI].QuadPart = 0;
				sample->MainThreadAccumulator[iI].QuadPart = 0;
				sample->ChildrenSampleTime[iI].QuadPart = 0;
				sample->AlternateSampleSetTime[iI].QuadPart = 0;
			}

			LeaveCriticalSection(&cSampleSection);
		}

		linkageInfo->parent = _currentSample;

//	#if 0
//		if ( !bAsConditional )
//		{
//			if ( ++depth == MAX_SAMPLES )
//			{
//				::MessageBox(NULL,"Sample stack overflow","CvGameCore",MB_OK);
//			}
//			else
//			{
//				sampleStack[depth] = sample;
//			}
//		}
//	#endif

		//InterlockedIncrement(&sample->ProfileInstances);
		sample->ProfileInstances[iThreadSlot]++;

//#if 0
//		linkageInfo->bIsEntry = true;
//
//		if ( InterlockedIncrement(&sample->EntryCount) > 1 )
//		{
//			//	If the entry count is still positive it could be due to either recursion
//			//	or concurrency - we need to check for the recursion case and not accrue
//			//	time in a recursive call
//	#define	MAX_RECURSION_HORIZON	2
//			int	iRecursionHorizon  = MAX_RECURSION_HORIZON;
//			for(ProfileLinkageInfo* linkage = _currentSample; linkage != NULL && iRecursionHorizon-- > 0; linkage = linkage->parent)
//			{
//				if ( linkageInfo->sample == sample )
//				{
//					//	recursion
//					linkageInfo->bIsEntry = false;
//					break;
//				}
//			}
//		}
//#else
		linkageInfo->bIsEntry = (sample->EntryCount[iThreadSlot]++ == 0);
//#endif

		if ( linkageInfo->bIsEntry )
		{
			if ( _currentSample != NULL && sample->Parent == -1 )
			{
				sample->Parent = _currentSample->sample->Id;
			}

			QueryPerformanceCounter(&linkageInfo->startTime);
		}

		if ( !bAsConditional )
		{
			_currentSample = linkageInfo;
		}

//	#if 0
//	#ifdef DETAILED_TRACE
//		if ( detailedTraceEnabled && lastExit != sample )
//		{
//			char buffer[300];
//
//			if ( exitCount != 0 )
//			{
//				GenerateTabString(buffer, depth);
//				sprintf(buffer+depth, "[%d]\n", exitCount);
//				OutputDebugString(buffer);
//
//				exitCount = 0;
//			}
//
//			GenerateTabString(buffer, depth);
//			sprintf(buffer+depth, "-->%s\n", sample->Name);
//
//			OutputDebugString(buffer);
//		}
//	#endif
//	#endif
	}
}

void IFPEndSample(ProfileLinkageInfo* linkageInfo, bool bAsConditional)
{
	if (!linkageInfo || !linkageInfo->sample)
	{
		return;
	}
	if ( iThreadSlot != -1 )
	{
		if ( !bAsConditional && _currentSample != linkageInfo &&  linkageInfo->sample->Parent != -1 )
		{
			char	buffer[200];

			sprintf(buffer, "Sample closure not matched: closing %s when %s was expected", linkageInfo->sample->Name, (_currentSample == NULL ? "<NULL>" : _currentSample->sample->Name));
			MessageBox(NULL,buffer,"CvGameCore",MB_OK);
		}

//	#if 0
//		if ( depth < 0 )
//		{
//			if ( sample->Parent != -1 )
//			{
//				MessageBox(NULL,"Too many end-samples","CvGameCore",MB_OK);
//			}
//		}
//		else
//	#endif
		{
			if ( !bAsConditional )
			{
//	#if 0
//				if ( depth == 0 )
//				{
//					_currentSample = NULL;
//					depth = -1;
//				}
//				else
//				{
//					_currentSample = sampleStack[--depth];
//				}
//	#endif
				_currentSample = linkageInfo->parent;
			}

			ProfileSample* sample = linkageInfo->sample;

			//InterlockedDecrement(&sample->EntryCount);
			sample->EntryCount[iThreadSlot]--;

			if (linkageInfo->bIsEntry)
			{
				LARGE_INTEGER now;
				LONGLONG ellapsed;

				QueryPerformanceCounter(&now);

				ellapsed = (now.QuadPart - linkageInfo->startTime.QuadPart);

				//EnterCriticalSection(&cSampleSection);

				//InterlockedExchangeAdd64(&sample->Accumulator.QuadPart, ellapsed);
				sample->Accumulator[iThreadSlot].QuadPart += ellapsed;

				if ( bIsMainThread )
				{
					//InterlockedExchangeAdd64(&sample->MainThreadAccumulator.QuadPart, ellapsed);
					sample->MainThreadAccumulator[iThreadSlot].QuadPart += ellapsed;
				}

				if ( _currentSample != NULL )
				{
					//InterlockedExchangeAdd64(&_currentSample->sample->ChildrenSampleTime.QuadPart, ellapsed);
					_currentSample->sample->ChildrenSampleTime[iThreadSlot].QuadPart += ellapsed;

					if ( sample->IsInAlternateSet )
					{
//		#if 0
//						for(int iI = 0; iI <= depth; iI++)
//						{
//							sampleStack[iI]->AlternateSampleSetTime.QuadPart += ellapsed;
//						}
//		#else
						for(ProfileLinkageInfo* linkage = _currentSample; linkage != NULL; linkage = linkage->parent)
						{
							//InterlockedExchangeAdd64(&linkage->sample->AlternateSampleSetTime.QuadPart, ellapsed);
							linkage->sample->AlternateSampleSetTime[iThreadSlot].QuadPart += ellapsed;
						}
//		#endif
					}
				}
				else
				{
					EnterCriticalSection(&cSampleSection);

					for(int iI = 0; iI < numSamples; iI++)
					{
						ProfileSample* thisSample = sampleList[iI];

						thisSample->Accumulator[RESERVED_THREAD_SLOT].QuadPart += thisSample->Accumulator[iThreadSlot].QuadPart;
						thisSample->MainThreadAccumulator[RESERVED_THREAD_SLOT].QuadPart += thisSample->MainThreadAccumulator[iThreadSlot].QuadPart;
						thisSample->ChildrenSampleTime[RESERVED_THREAD_SLOT].QuadPart += thisSample->ChildrenSampleTime[iThreadSlot].QuadPart;
						thisSample->AlternateSampleSetTime[RESERVED_THREAD_SLOT].QuadPart += thisSample->AlternateSampleSetTime[iThreadSlot].QuadPart;
						thisSample->ProfileInstances[RESERVED_THREAD_SLOT] += thisSample->ProfileInstances[iThreadSlot];

						thisSample->EntryCount[iThreadSlot] = 0;
						thisSample->ProfileInstances[iThreadSlot] = 0;
						thisSample->Accumulator[iThreadSlot].QuadPart = 0;
						thisSample->MainThreadAccumulator[iThreadSlot].QuadPart = 0;
						thisSample->ChildrenSampleTime[iThreadSlot].QuadPart = 0;
						thisSample->AlternateSampleSetTime[iThreadSlot].QuadPart = 0;
					}

					if ( !bIsMainThread )
					{
						bThreadSlotOccupied[iThreadSlot]  = false;
						iThreadSlot = -1;
					}

					LeaveCriticalSection(&cSampleSection);
				}

				//LeaveCriticalSection(&cSampleSection);
			}

//	#if 0
//	#ifdef DETAILED_TRACE
//			if ( detailedTraceEnabled && lastExit != sample )
//			{
//				char buffer[300];
//
//				GenerateTabString(buffer, depth+1);
//				strcpy(buffer+depth+1, "...\n");
//
//				OutputDebugString(buffer);
//				exitCount = 1;
//			}
//			else
//			{
//				exitCount++;
//			}
//
//			lastExit = sample;
//	#endif
//	#endif
		}
	}
}

void IFPCancelSample(ProfileLinkageInfo* linkageInfo)
{
	if ( iThreadSlot != -1 )
	{
		linkageInfo->sample->EntryCount[iThreadSlot]--;
		linkageInfo->sample->ProfileInstances[iThreadSlot]--;
	}
}

void IFPBegin()
{
	//OutputDebugString("IFPBegin\n");
	for(int i = 0; i < numSamples; i++ )
	{
		sampleList[i]->Accumulator[RESERVED_THREAD_SLOT].QuadPart = 0;
		sampleList[i]->MainThreadAccumulator[RESERVED_THREAD_SLOT].QuadPart = 0;
		sampleList[i]->AlternateSampleSetTime[RESERVED_THREAD_SLOT].QuadPart = 0;
		sampleList[i]->ChildrenSampleTime[RESERVED_THREAD_SLOT].QuadPart = 0;
		sampleList[i]->ProfileInstances[RESERVED_THREAD_SLOT] = 0;
	}
}

void IFPEnd()
{
	//	Log the timings
	char buffer[300];
	LARGE_INTEGER freq;

	//OutputDebugString("IFPEnd\n");
	QueryPerformanceFrequency(&freq);

	gDLL->logMsg("IFP_log.txt","Fn\tTime (mS)\tMain thread time (mS)\tAvg time\t#calls\tChild time\tSelf time\tParent\tAlternate Time\n");

	for(int i = 0; i < numSamples; i++ )
	{
		if ( sampleList[i]->ProfileInstances[RESERVED_THREAD_SLOT] != 0 )
		{
			sprintf(buffer,
					"%s\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%d\n",
					sampleList[i]->Name,
					(int)((1000*sampleList[i]->Accumulator[RESERVED_THREAD_SLOT].QuadPart)/freq.QuadPart),
					(int)((1000*sampleList[i]->MainThreadAccumulator[RESERVED_THREAD_SLOT].QuadPart)/freq.QuadPart),
					(int)((1000*sampleList[i]->Accumulator[RESERVED_THREAD_SLOT].QuadPart)/(freq.QuadPart*sampleList[i]->ProfileInstances[RESERVED_THREAD_SLOT])),
					sampleList[i]->ProfileInstances[RESERVED_THREAD_SLOT],
					(int)((1000*sampleList[i]->ChildrenSampleTime[RESERVED_THREAD_SLOT].QuadPart)/freq.QuadPart),
					(int)((1000*sampleList[i]->Accumulator[RESERVED_THREAD_SLOT].QuadPart)/freq.QuadPart) - (int)((1000*sampleList[i]->ChildrenSampleTime[RESERVED_THREAD_SLOT].QuadPart)/freq.QuadPart),
					sampleList[i]->Parent == -1 ? "" : sampleList[sampleList[i]->Parent]->Name,
					(int)((1000*sampleList[i]->AlternateSampleSetTime[RESERVED_THREAD_SLOT].QuadPart)/freq.QuadPart));
			gDLL->logMsg("IFP_log.txt",buffer);
		}
	}
}

void IFPSetCount(ProfileSample* sample, int count)
{
	if ( numSamples == MAX_SAMPLES )
	{
		dumpProfileStack();
		::MessageBox(NULL,"Profile sample limit exceeded","CvGameCore",MB_OK);
		return;
	}
	sample->Id = numSamples;
	sample->EntryCount[RESERVED_THREAD_SLOT] = 0;
	sample->ProfileInstances[RESERVED_THREAD_SLOT] = count;
	sample->Accumulator[RESERVED_THREAD_SLOT].QuadPart = 0;
	sample->ChildrenSampleTime[RESERVED_THREAD_SLOT].QuadPart = 0;
	sampleList[numSamples++] = sample;
}

static bool isInLongLivedSection = false;
static bool longLivedSectionDumped = true;
static ProfileSample rootSample__("Root");
static ProfileLinkageInfo rootSampleLinkage;

//
// dump the current (profile) call stack to debug output
//
void dumpProfileStack()
{
//#if 0
//	int i = 0;
//	int dumpDepth = depth;
//	char buffer[200];
//
//	OutputDebugString("Profile stack:\n");
//
//	while(dumpDepth >= 0)
//	{
//		char* ptr = buffer;
//
//		i++;
//		for(int j = 0; j < i; j++)
//		{
//			*ptr++ = '\t';
//		}
//		strcpy(ptr,sampleStack[dumpDepth--]->Name);
//		strcat(ptr, "\n");
//		OutputDebugString(buffer);
//	}
//#endif
}

#endif  // Use internal profiler

//
// enable dll profiler if necessary, clear history
//
void startProfilingDLL(bool longLived)
{
#ifdef USE_INTERNAL_PROFILER
	if ( longLived )
	{
		//OutputDebugString("Entering long lived profile section\n");
		isInLongLivedSection = true;
		longLivedSectionDumped = false;
	}
	else if (GC.isDLLProfilerEnabled() && !isInLongLivedSection && longLivedSectionDumped)
	{
		rootSampleLinkage.sample = &rootSample__;

		IFPBegin();
		IFPBeginSample(&rootSampleLinkage);
	}
#else
	if (GC.isDLLProfilerEnabled())
	{
		gDLL->ProfilerBegin();
	}
#endif
}

//
// dump profile stats on-screen
//
void stopProfilingDLL(bool longLived)
{
#ifdef USE_INTERNAL_PROFILER
	if ( longLived )
	{
		//OutputDebugString("Exiting long lived profile section\n");
		isInLongLivedSection = false;
	}
	else if (GC.isDLLProfilerEnabled() && !isInLongLivedSection)
	{
		IFPEndSample(&rootSampleLinkage);
		IFPEnd();

		longLivedSectionDumped = true;
	}
#else
	if (GC.isDLLProfilerEnabled())
	{
		gDLL->ProfilerEnd();
	}
#endif
}

// Toffer - Square root with integer math.
int intSqrt(unsigned int iValue, const bool bTreatNegAsPos)
{
	unsigned int iRem = 0;
	unsigned int iRoot = 0;
	for (int iI = 0; iI < 16; iI++)
	{
		iRoot <<= 1;
		iRem <<= 2;
		iRem += iValue >> 30;
		iValue <<= 2;

		if (iRoot < iRem)
		{
			iRoot++;
			iRem -= iRoot;
			iRoot++;
		}
	}
	return static_cast<int>(iRoot >> 1);
}

// Testing alternate version; should compare to see which one is fastest.
int64_t intSqrt64(const uint64_t iValue)
{
	uint64_t min = 0;
	uint64_t max = ((uint64_t) 1) << 32;
	while(true)
	{
		if (max <= 1 + min)
		{
			return static_cast<int64_t>(min);
		}
		const uint64_t sqt = min + (max - min)/2;
		const uint64_t sq = sqt*sqt;

		if (sq == iValue)
		{
			return static_cast<int64_t>(sqt);
		}
		if (sq > iValue)
			max = sqt;
		else min = sqt;
	}
}

// int64 pow
int64_t intPow64(const int64_t x, const int p)
{
	if (p <= 0)
	{
		if (p == 0)
			return 1;
		return 0;
	}
	if (p == 1) return x;

	const int64_t iTmp = intPow64(x, p/2);
	if (p % 2 == 0)
	{
		return iTmp * iTmp;
	}
	return x * iTmp * iTmp;
}

// int32 pow
int intPow(const int x, const int p)
{
	const int64_t iResult = intPow64(x, p);

	if (iResult > MAX_INT || iResult < 0)
	{
		return MAX_INT;
	}
	return static_cast<int>(iResult);
}

int getModifiedIntValue(const int iValue, const int iMod)
{
	if (iMod > 0)
	{
		return iValue * (100 + iMod) / 100;
	}
	if (iMod < 0)
	{
		return iValue * 100 / (100 - iMod);
	}
	return iValue;
}
int64_t getModifiedIntValue64(const int64_t iValue, const int iMod)
{
	if (iMod > 0)
	{
		return iValue * (100 + iMod) / 100;
	}
	if (iMod < 0)
	{
		return iValue * 100 / (100 - iMod);
	}
	return iValue;
}
// ! Toffer

const std::string getModDir()
{
	return modDir;
}


// Applies rank scaling to a value, with overflow protection.
// rankMultiplier should be scaled up by 100 (e.g. 300 instead of 3).
// rankChange can be positive or negative.
// Equation demonstrated here: https://www.desmos.com/calculator/wivft5kfcc
int applySMRank(int value, int rankChange, int rankMultiplier)
{
	FAssertMsg(rankMultiplier > 0, "rankMultiplier must be greater than 0");
	int64_t lvalue = 100 * value;
	if (rankChange > 0)
	{
		for (int iI = 0; iI < rankChange; iI++)
		{
			lvalue *= rankMultiplier;
			lvalue /= 100;
		}
	}
	else
	{
		for (int iI = 0; iI < -rankChange; iI++)
		{
			lvalue *= 100;
			lvalue /= rankMultiplier;
		}
	}
	return static_cast<int>(std::min<int64_t>(MAX_INT, lvalue / 100));
}

int64_t applySMRank64(int64_t value, int rankChange, int rankMultiplier, bool bScaleUp)
{
	FAssertMsg(rankMultiplier > 0, "rankMultiplier must be greater than 0");
	if (bScaleUp) value *= 100;

	if (rankChange > 0)
	{
		for (int iI = 0; iI < rankChange; iI++)
		{
			value *= rankMultiplier;
			value /= 100;
		}
	}
	else
	{
		for (int iI = 0; iI < -rankChange; iI++)
		{
			value *= 100;
			value /= rankMultiplier;
		}
	}
	if (bScaleUp)
	{
		return value / 100;
	}
	return value;
}