#include "CvGameCoreDLL.h"

#include "CvGameCoreDLLUndefNew.h"

#include <new>

#include <psapi.h>

#ifdef MEMORY_TRACKING
void	ProfileTrackAlloc(void* ptr, int size);

void	ProfileTrackDeAlloc(void* ptr);

void DumpMemUsage(const char* fn, int line)
{
	PROCESS_MEMORY_COUNTERS pmc;

	if ( GetProcessMemoryInfo( GetCurrentProcess(), &pmc, sizeof(pmc)) )
	{
		char buffer[200];

		sprintf(buffer, "memory (%s,%d): %d Kbytes, peak %d\n", fn, line, (int)(pmc.WorkingSetSize/1024), (int)(pmc.PeakWorkingSetSize/1024));
		OutputDebugString(buffer);
	}
}

#define	PROFILE_TRACK_ALLOC(x,n)	ProfileTrackAlloc(x,n);
#define	PROFILE_TRACK_DEALLOC(x)	ProfileTrackDeAlloc(x);
#else
#define	PROFILE_TRACK_ALLOC(x,n)	
#define	PROFILE_TRACK_DEALLOC(x)	
#endif

static int allocCount = 0;
static CRITICAL_SECTION g_cPythonSection;
#ifdef USE_INTERNAL_PROFILER
static CRITICAL_SECTION cSampleSection;
#endif

//
// operator global new and delete override for gamecore DLL 
//
void *__cdecl operator new(size_t size)
{
	if (g_DLL)
	{
#if 0
		char buffer[200];
		sprintf(buffer,"Alloc %d\n", size);
		OutputDebugString(buffer);
#endif

		void* result = NULL;

		try
		{
			result = g_DLL->newMem(size, __FILE__, __LINE__);
#ifdef _DEBUG
			memset(result, 0xDA, size);
#endif
			PROFILE_TRACK_ALLOC(result, size);
		}
		catch(std::exception&)
		{
			OutputDebugString("Allocation failure\n");
		}

		return result;
	}

	OutputDebugString("Alloc [unsafe]");
	::MessageBoxA(NULL,"UNsafe alloc","CvGameCore",MB_OK);
	return malloc(size);
}

void __cdecl operator delete (void *p)
{
	if (g_DLL)
	{
		PROFILE_TRACK_DEALLOC(p);

		g_DLL->delMem(p, __FILE__, __LINE__);

	}
	else
	{
		free(p);
	}
}

void* operator new[](size_t size)
{
	if (g_DLL)
	{
		//OutputDebugString("Alloc [safe]");

		void* result = g_DLL->newMemArray(size, __FILE__, __LINE__);
#ifdef _DEBUG
		memset(result, 0xDA, size);
#endif
		return result;
	}

	OutputDebugString("Alloc [unsafe]");
	::MessageBoxA(NULL,"UNsafe alloc","CvGameCore",MB_OK);
	return malloc(size);
}

void operator delete[](void* pvMem)
{
	if (g_DLL)
	{
		g_DLL->delMemArray(pvMem, __FILE__, __LINE__);
	}
	else
	{
		free(pvMem);
	}
}

void *__cdecl operator new(size_t size, char* pcFile, int iLine)
{
	void* result = g_DLL->newMem(size, pcFile, iLine);
#ifdef _DEBUG
	memset(result, 0xDA, size);
#endif
	return result;
}

void *__cdecl operator new[](size_t size, char* pcFile, int iLine)
{
	void* result = g_DLL->newMem(size, pcFile, iLine);
#ifdef _DEBUG
	memset(result, 0xDA, size);
#endif
	return result;
}

void __cdecl operator delete(void* pvMem, char* pcFile, int iLine)
{
	g_DLL->delMem(pvMem, pcFile, iLine);
}

void __cdecl operator delete[](void* pvMem, char* pcFile, int iLine)
{
	g_DLL->delMem(pvMem, pcFile, iLine);
}


void* reallocMem(void* a, unsigned int uiBytes, const char* pcFile, int iLine)
{
	void* result;

	result = g_DLL->reallocMem(a, uiBytes, pcFile, iLine);

	return result;
}

unsigned int memSize(void* a)
{
	unsigned int result = g_DLL->memSize(a);

	return result;
}

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
		success = ::WaitForSingleObject(procInfo.hProcess, 600000) == WAIT_OBJECT_0;
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
					  LPVOID lpReserved)
{
	switch( ul_reason_for_call ) {
	case DLL_PROCESS_ATTACH:
		{

// BUG - EXE/DLL Paths - start
		dllModule = hModule;

#ifdef _DEBUG
/************************************************************************************************/
/* Afforess	                  Start		 07/30/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
//Irritating, and meaningless.
//	MessageBox(0, "DLL attached", "Message", 0);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
#endif
// BUG - EXE/DLL Paths - end

		// The DLL is being loaded into the virtual address space of the current process as a result of the process starting up 
		OutputDebugString("DLL_PROCESS_ATTACH\n");

		InitializeCriticalSection(&g_cPythonSection);
		
#ifdef USE_INTERNAL_PROFILER
		InitializeCriticalSectionAndSpinCount(&cSampleSection,2000);
#endif

		// set timer precision
		MMRESULT iTimeSet = timeBeginPeriod(1);		// set timeGetTime and sleep resolution to 1 ms, otherwise it's 10-16ms
		FAssertMsg(iTimeSet==TIMERR_NOERROR, "failed setting timer resolution to 1 ms");

		// Get DLL directory
		CHAR pathBuffer[MAX_PATH];
		GetModuleFileNameA((HMODULE)dllModule, pathBuffer, sizeof(pathBuffer));
		std::string dllPath = pathBuffer;
		std::string dllDir = dllPath.substr(0, dllPath.length() - strlen("CvGameCoreDLL.dll"));
		std::string tokenFile = dllDir + "\\..\\git_directory.txt";
		std::ifstream stream(tokenFile.c_str());
		// If we loaded the directory token file we are in a dev environment and should run FPKLive, and check for DLL changes
		if (!stream.fail())
		{
			std::string git_dir;
			std::getline(stream, git_dir);

			if(!runProcess(git_dir + "\\Tools\\FPKLive.exe", git_dir + "\\Tools"))
			{
				MessageBox(0, "Creation of FPK packs failed, are you sure you set up the development environment correctly?", "ERROR!", 0);
				return FALSE;
			}
			if (!runProcess("cmd.exe /C \"" + git_dir + "\\Tools\\_BootDLLCheck.bat\" " + TOSTRING(BUILD_TARGET), git_dir + "\\Tools"))
			{
				MessageBox(0, "DLL update failed!", "ERROR!", 0);
				return FALSE;
			}
		}

		}
		break;
	case DLL_THREAD_ATTACH:
		OutputDebugString(CvString::format("DLL_THREAD_ATTACH: %d\n", GetCurrentThreadId()).c_str());
		break;
	case DLL_THREAD_DETACH:
		OutputDebugString(CvString::format("DLL_THREAD_DETACH: %d\n", GetCurrentThreadId()).c_str());
		break;
	case DLL_PROCESS_DETACH:

// BUG - EXE/DLL Paths - start
		dllModule = NULL;
// BUG - EXE/DLL Paths - end

		OutputDebugString("DLL_PROCESS_DETACH\n");
		timeEndPeriod(1);
		break;
	}
	
	return TRUE;	// success
}

#ifdef USE_INTERNAL_PROFILER

//	Uncomment the following line to provide (very) detailed tracing to
//	the debug stream (view with DbgView or a debugger)
#define DETAILED_TRACE

#define MAX_SAMPLES				1200
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

	#if 0
		if ( !bAsConditional )
		{
			if ( ++depth == MAX_SAMPLES )
			{
				::MessageBox(NULL,"Sample stack overflow","CvGameCore",MB_OK);
			}
			else
			{
				sampleStack[depth] = sample;
			}
		}
	#endif

		//InterlockedIncrement(&sample->ProfileInstances);
		sample->ProfileInstances[iThreadSlot]++;

#if 0
		linkageInfo->bIsEntry = true;

		if ( InterlockedIncrement(&sample->EntryCount) > 1 )
		{
			//	If the entry count is still positive it could be due to either recursion
			//	or concurrency - we need to check for the recursion case and not accrue
			//	time in a recursive call
	#define	MAX_RECURSION_HORIZON	2
			int	iRecursionHorizon  = MAX_RECURSION_HORIZON;
			for(ProfileLinkageInfo* linkage = _currentSample; linkage != NULL && iRecursionHorizon-- > 0; linkage = linkage->parent)
			{
				if ( linkageInfo->sample == sample )
				{
					//	recursion
					linkageInfo->bIsEntry = false;
					break;
				}
			}
		}
#else
		linkageInfo->bIsEntry = (sample->EntryCount[iThreadSlot]++ == 0);
#endif

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

	#if 0
	#ifdef DETAILED_TRACE
		if ( detailedTraceEnabled && lastExit != sample )
		{
			char buffer[300];

			if ( exitCount != 0 )
			{
				GenerateTabString(buffer, depth);
				sprintf(buffer+depth, "[%d]\n", exitCount);
				OutputDebugString(buffer);

				exitCount = 0;
			}

			GenerateTabString(buffer, depth);
			sprintf(buffer+depth, "-->%s\n", sample->Name);

			OutputDebugString(buffer);
		}
	#endif
	#endif
	}
}

void IFPEndSample(ProfileLinkageInfo* linkageInfo, bool bAsConditional)
{
	if ( iThreadSlot != -1 )
	{
		if ( !bAsConditional && _currentSample != linkageInfo && linkageInfo->sample->Parent != -1 )
		{
			char	buffer[200];

			sprintf(buffer, "Sample closure not matched: closing %s when %s was expected", linkageInfo->sample->Name, (_currentSample == NULL ? "<NULL>" : _currentSample->sample->Name));
			MessageBox(NULL,buffer,"CvGameCore",MB_OK);
		}

	#if 0
		if ( depth < 0 )
		{
			if ( sample->Parent != -1 )
			{
				MessageBox(NULL,"Too many end-samples","CvGameCore",MB_OK);
			}
		}
		else 
	#endif
		{
			if ( !bAsConditional )
			{
	#if 0
				if ( depth == 0 )
				{
					_currentSample = NULL;
					depth = -1;
				}
				else
				{
					_currentSample = sampleStack[--depth];
				}
	#endif
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
		#if 0
						for(int iI = 0; iI <= depth; iI++)
						{
							sampleStack[iI]->AlternateSampleSetTime.QuadPart += ellapsed;
						}
		#else
						for(ProfileLinkageInfo* linkage = _currentSample; linkage != NULL; linkage = linkage->parent)
						{
							//InterlockedExchangeAdd64(&linkage->sample->AlternateSampleSetTime.QuadPart, ellapsed);
							linkage->sample->AlternateSampleSetTime[iThreadSlot].QuadPart += ellapsed;
						}
		#endif
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

	#if 0
	#ifdef DETAILED_TRACE
			if ( detailedTraceEnabled && lastExit != sample )
			{
				char buffer[300];

				GenerateTabString(buffer, depth+1);
				strcpy(buffer+depth+1, "...\n");

				OutputDebugString(buffer);
				exitCount = 1;
			}
			else
			{
				exitCount++;
			}

			lastExit = sample;
	#endif
	#endif
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

	g_DLL->logMsg("IFP_log.txt","Fn\tTime (mS)\tMain thread time (mS)\tAvg time\t#calls\tChild time\tSelf time\tParent\tAlternate Time\n");

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
			g_DLL->logMsg("IFP_log.txt",buffer);
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
#if 0
	int i = 0;
	int dumpDepth = depth;
	char buffer[200];

	OutputDebugString("Profile stack:\n");

	while(dumpDepth >= 0)
	{
		char* ptr = buffer;

		i++;
		for(int j = 0; j < i; j++)
		{
			*ptr++ = '\t';
		}
		strcpy(ptr,sampleStack[dumpDepth--]->Name);
		strcat(ptr, "\n");
		OutputDebugString(buffer);
	}
#endif
}

#endif  // USe internal profiler

void IFPLockPythonAccess()
{
	//EnterCriticalSection(&g_cPythonSection);
}

void IFPUnlockPythonAccess()
{
	//LeaveCriticalSection(&g_cPythonSection);
}

static int pythonDepth = 0;

bool IFPPythonCall(const char* callerFn, const char* moduleName, const char* fxnName, void* fxnArg)
{
	bool result;
	
	PROFILE("IFPPythonCall1");

#ifdef USE_INTERNAL_PROFILER
	FAssert(bIsMainThread || iThreadSlot == -1);
	if ( !bIsMainThread )
	{
		::MessageBox(NULL, "Illegal use of Python on background thread", "CvGameCoreDLL", MB_OK); 
	}
#endif
	//OutputDebugString(CvString::format("Python call to %s::%s [%d]\n", moduleName, fxnName, pythonDepth++).c_str());

	//EnterCriticalSection(&g_cPythonSection);
	result = gDLL->getPythonIFace()->callFunction(moduleName, fxnName, fxnArg);
	//LeaveCriticalSection(&g_cPythonSection);
	
	//OutputDebugString("...complete\n");
	pythonDepth--;

	return result;
}

bool IFPPythonCall(const char* callerFn, const char* moduleName, const char* fxnName, void* fxnArg, long* result)
{
	PROFILE("IFPPythonCall2");

#ifdef USE_INTERNAL_PROFILER
	FAssert(bIsMainThread || iThreadSlot == -1);
	if ( !bIsMainThread )
	{
		::MessageBox(NULL, "Illegal use of Python on background thread", "CvGameCoreDLL", MB_OK); 
	}
#endif

#ifdef FP_PROFILE_ENABLE				// Turn Profiling On or Off .. 
#ifdef USE_INTERNAL_PROFILER
	static	std::map<int,ProfileSample*>*	g_pythonProfiles = NULL;

	if ( g_pythonProfiles == NULL )
	{
		g_pythonProfiles = new std::map<int,ProfileSample*>();
	}

	CvChecksum xSum;
	const char* ptr;
	ProfileSample* pSample;

	for(ptr = moduleName; *ptr != '\0'; ptr++)
	{
		xSum.add((byte)*ptr);
	}
	for(ptr = fxnName; *ptr != '\0'; ptr++)
	{
		xSum.add((byte)*ptr);
	}

	std::map<int,ProfileSample*>::const_iterator itr = g_pythonProfiles->find(xSum.get());

	if ( itr == g_pythonProfiles->end() )
	{
		char profileName[256];

		sprintf(profileName, "IFPPythonCall2.%s::%s", moduleName, fxnName);
		pSample = new ProfileSample(profileName);

		g_pythonProfiles->insert(std::make_pair(xSum.get(), pSample));
	}
	else
	{
		pSample = itr->second;
	}

	CProfileScope detailedScope(pSample);		
#endif
#endif

	//OutputDebugString(CvString::format("Python call to %s::%s [%d]\n", moduleName, fxnName, pythonDepth++).c_str());

	bool bResult = gDLL->getPythonIFace()->callFunction(moduleName, fxnName, fxnArg, result);

	//OutputDebugString("...complete\n");
	pythonDepth--;

	return bResult;
}


bool IFPPythonCall(const char* callerFn, const char* moduleName, const char* fxnName, void* fxnArg, CvString* result)
{
	PROFILE("IFPPythonCall3");

#ifdef USE_INTERNAL_PROFILER
	FAssert(bIsMainThread || iThreadSlot == -1);
	if ( !bIsMainThread )
	{
		::MessageBox(NULL, "Illegal use of Python on background thread", "CvGameCoreDLL", MB_OK); 
	}
#endif

	//OutputDebugString(CvString::format("Python call to %s::%s [%d]\n", moduleName, fxnName, pythonDepth++).c_str());

	//EnterCriticalSection(&g_cPythonSection);
	bool bResult = gDLL->getPythonIFace()->callFunction(moduleName, fxnName, fxnArg, result);
	//LeaveCriticalSection(&g_cPythonSection);
	
	//OutputDebugString("...complete\n");
	pythonDepth--;

	return bResult;
}


bool IFPPythonCall(const char* callerFn, const char* moduleName, const char* fxnName, void* fxnArg, CvWString* result)
{
	PROFILE("IFPPythonCall4");

#ifdef USE_INTERNAL_PROFILER
	FAssert(bIsMainThread || iThreadSlot == -1);
	if ( !bIsMainThread )
	{
		::MessageBox(NULL, "Illegal use of Python on background thread", "CvGameCoreDLL", MB_OK); 
	}
#endif
	//OutputDebugString(CvString::format("Python call to %s::%s [%d]\n", moduleName, fxnName, pythonDepth++).c_str());

	//EnterCriticalSection(&g_cPythonSection);
	bool bResult = gDLL->getPythonIFace()->callFunction(moduleName, fxnName, fxnArg, result);
	//LeaveCriticalSection(&g_cPythonSection);
	
	//OutputDebugString("...complete\n");
	pythonDepth--;

	return bResult;
}


bool IFPPythonCall(const char* callerFn, const char* moduleName, const char* fxnName, void* fxnArg, std::vector<byte>* pList)
{
	PROFILE("IFPPythonCall5");

#ifdef USE_INTERNAL_PROFILER
	FAssert(bIsMainThread || iThreadSlot == -1);
	if ( !bIsMainThread )
	{
		::MessageBox(NULL, "Illegal use of Python on background thread", "CvGameCoreDLL", MB_OK); 
	}
#endif

	//OutputDebugString(CvString::format("Python call to %s::%s [%d]\n", moduleName, fxnName, pythonDepth++).c_str());

	//EnterCriticalSection(&g_cPythonSection);
	bool result = gDLL->getPythonIFace()->callFunction(moduleName, fxnName, fxnArg, pList);
	//LeaveCriticalSection(&g_cPythonSection);
	
	//OutputDebugString("...complete\n");
	pythonDepth--;

	return result;
}


bool IFPPythonCall(const char* callerFn, const char* moduleName, const char* fxnName, void* fxnArg, std::vector<int> *pIntList)
{
	PROFILE("IFPPythonCall6");

#ifdef USE_INTERNAL_PROFILER
	FAssert(bIsMainThread || iThreadSlot == -1);
	if ( !bIsMainThread )
	{
		::MessageBox(NULL, "Illegal use of Python on background thread", "CvGameCoreDLL", MB_OK); 
	}
#endif

	//OutputDebugString(CvString::format("Python call to %s::%s [%d]\n", moduleName, fxnName, pythonDepth++).c_str());

	//EnterCriticalSection(&g_cPythonSection);
	bool result = gDLL->getPythonIFace()->callFunction(moduleName, fxnName, fxnArg, pIntList);
	//LeaveCriticalSection(&g_cPythonSection);

	//OutputDebugString("...complete\n");
	pythonDepth--;

	return result;
}


bool IFPPythonCall(const char* callerFn, const char* moduleName, const char* fxnName, void* fxnArg, int* pIntList, int* iListSize)
{
	PROFILE("IFPPythonCall7");

#ifdef USE_INTERNAL_PROFILER
	FAssert(bIsMainThread || iThreadSlot == -1);
	if ( !bIsMainThread )
	{
		::MessageBox(NULL, "Illegal use of Python on background thread", "CvGameCoreDLL", MB_OK); 
	}
#endif

	//OutputDebugString(CvString::format("Python call to %s::%s [%d]\n", moduleName, fxnName, pythonDepth++).c_str());

	//EnterCriticalSection(&g_cPythonSection);
	bool result = gDLL->getPythonIFace()->callFunction(moduleName, fxnName, fxnArg, pIntList, iListSize);
	//LeaveCriticalSection(&g_cPythonSection);
	
	//OutputDebugString("...complete\n");
	pythonDepth--;

	return result;
}


bool IFPPythonCall(const char* callerFn, const char* moduleName, const char* fxnName, void* fxnArg, std::vector<float> *pFloatList)
{
	PROFILE("IFPPythonCall8");

#ifdef USE_INTERNAL_PROFILER
	FAssert(bIsMainThread || iThreadSlot == -1);
	if ( !bIsMainThread )
	{
		::MessageBox(NULL, "Illegal use of Python on background thread", "CvGameCoreDLL", MB_OK); 
	}
#endif

	//OutputDebugString(CvString::format("Python call to %s::%s [%d]\n", moduleName, fxnName, pythonDepth++).c_str());

	//EnterCriticalSection(&g_cPythonSection);
	bool result = gDLL->getPythonIFace()->callFunction(moduleName, fxnName, fxnArg, pFloatList);
	//LeaveCriticalSection(&g_cPythonSection);
	
	//OutputDebugString("...complete\n");
	pythonDepth--;

	return result;
}

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

#ifdef MEMORY_TRACKING
CMemoryTrack*	CMemoryTrack::trackStack[MAX_TRACK_DEPTH];
int CMemoryTrack::m_trackStackDepth = 0;

CMemoryTrack::CMemoryTrack(const char* name, bool valid)
{
	if ( bIsMainThread )
	{
		m_highWater = 0;
		m_seq = 0;
		m_name = name;
		m_valid = valid;

		if ( m_trackStackDepth < MAX_TRACK_DEPTH )
		{
			trackStack[m_trackStackDepth++] = this;
		}
	}
	else
	{
		m_valid = false;
	}
}

CMemoryTrack::~CMemoryTrack()
{
	if ( bIsMainThread )
	{
		if ( m_valid )
		{
			for(int i = 0; i < m_highWater; i++)
			{
				if ( m_track[i] != NULL )
				{
					char buffer[200];

					sprintf(buffer, "Apparent memory leak (size %d) detected in %s (from %s:%d)\n", m_allocSize[i], m_name, m_trackName[i], m_allocSeq[i]);
					OutputDebugString(buffer);
				}
			}
		}

		if ( trackStack[m_trackStackDepth-1] == this )
		{
			m_trackStackDepth--;
		}
	}
}

void CMemoryTrack::NoteAlloc(void* ptr, int size)
{
	if ( m_valid )
	{
		for(int i = 0; i < m_highWater; i++)
		{
			if ( m_track[i] == NULL )
			{
				if ( i == m_highWater )
				{
					if ( m_highWater < MAX_TRACKED_ALLOCS )
					{
						m_highWater++;
					}
					else
					{
						m_valid = false;
						return;
					}
				}

				m_track[i] = ptr;
				m_allocSeq[i] = m_seq++;
				m_allocSize[i] = size;
#ifdef USE_INTERNAL_PROFILER
				m_trackName[i] = _currentSample == NULL || _currentSample->sample == NULL ? "<None>" : _currentSample->sample->Name;
#else
				m_trackName[i] = "";
#endif
			}
		}
	}
}

void CMemoryTrack::NoteDeAlloc(void* ptr)
{
	if ( m_valid )
	{
		for(int i = 0; i < m_highWater; i++)
		{
			if ( m_track[i] == ptr )
			{
				m_track[i] = NULL;
				break;
			}
		}
	}
}

CMemoryTrack* CMemoryTrack::GetCurrent()
{
	if ( 0 < m_trackStackDepth && m_trackStackDepth < MAX_TRACK_DEPTH )
	{
		return trackStack[m_trackStackDepth-1];
	}
	else
	{
		return NULL;
	}
}

CMemoryTrace::CMemoryTrace(const char* name)
{
	PROCESS_MEMORY_COUNTERS pmc;

	GetProcessMemoryInfo( GetCurrentProcess(), &pmc, sizeof(pmc));
	m_name = name;
	m_start = pmc.WorkingSetSize;
}

CMemoryTrace::~CMemoryTrace()
{
	static PROCESS_MEMORY_COUNTERS lastPmc;
	PROCESS_MEMORY_COUNTERS pmc;

	if ( bIsMainThread )
	{
		if ( GetProcessMemoryInfo( GetCurrentProcess(), &pmc, sizeof(pmc)) && (pmc.WorkingSetSize != m_start || lastPmc.WorkingSetSize/1024 != pmc.WorkingSetSize/1024))
		{
			char buffer[200];

			sprintf(buffer, "function %s added %dK bytes, total now %uK\n", m_name, ((int)(pmc.WorkingSetSize - m_start))/1024, (unsigned int)pmc.WorkingSetSize/1024);
			OutputDebugString(buffer);
		}

		memcpy(&lastPmc, &pmc, sizeof(pmc));
	}
}

void ProfileTrackAlloc(void* ptr, int size)
{
	if ( bIsMainThread )
	{
		CMemoryTrack* current = CMemoryTrack::GetCurrent();

		if ( current != NULL )
		{
			current->NoteAlloc(ptr, size);
		}
	}
}

void ProfileTrackDeAlloc(void* ptr)
{
	if ( bIsMainThread )
	{
		CMemoryTrack* current = CMemoryTrack::GetCurrent();

		if ( current != NULL )
		{
			current->NoteDeAlloc(ptr);
		}
	}
}

#endif