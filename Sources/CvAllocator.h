#pragma once

#ifndef CvAllocator_h__
#define CvAllocator_h__

#include <new>
#include <typeinfo>

#define MEMTRACK_EXEMPT MemTrack::UntrackedScope CONCATENATE(__untracked_memory_marker_, __LINE__)

namespace MemTrack
{
	/* ---------------------------------------- class MemStamp */

	class MemStamp
	{
	public:        // member variables
		char const* const filename;
		int const lineNum;
	public:        // construction/destruction
		MemStamp(char const* filename, int lineNum)
			: filename(filename), lineNum(lineNum) { }
		~MemStamp() { }
	};

	/* ---------------------------------------- memory allocation and stamping prototypes */

	void* TrackMalloc(size_t size);
	void TrackFree(void* p);
	void TrackStamp(void* p, const MemStamp& stamp, char const* typeName);
	void TrackDumpBlocks();
	void TrackListMemoryUsage();
	void TrackListMemoryUsageTurnDiff();


	/* ---------------------------------------- operator * (MemStamp, ptr) */

	template <class T> inline T* operator*(const MemStamp& stamp, T* p)
	{
		TrackStamp(p, stamp, typeid(T).name());
		return p;
	}

	struct UntrackedScope
	{
		UntrackedScope() { ++m_ref; }
		~UntrackedScope() { --m_ref; }

		static bool is_tracking_disabled() { return m_ref != 0; }
	private:

		UntrackedScope(const UntrackedScope&) {}
		UntrackedScope& operator=(const UntrackedScope&) {}

		static int m_ref;
	};


}    // namespace MemTrack

/* ---------------------------------------- new macro */

#ifdef MEMTRACK
#define MEMTRACK_NEW MemTrack::MemStamp(__FILE__, __LINE__) * new
#define new MEMTRACK_NEW
#endif

//
// operator global new and delete override for gamecore DLL
//

//void* __cdecl operator new(size_t size)
//{
//	extern void* CvMalloc(size_t size);
//	return CvMalloc(size);
//}
//
//void __cdecl operator delete (void* p)
//{
//	extern void CvFree(void* p);
//	CvFree(p);
//}
//
//void* operator new[](size_t size)
//{
//	extern void* CvMallocArray(size_t size);
//	return CvMallocArray(size);
//}
//
//void operator delete[](void* p)
//{
//	extern void CvFreeArray(void* p);
//	CvFreeArray(p);
//}

//void* __cdecl operator new(size_t size, char* pcFile, int iLine)
//{
//	void* result = g_DLL->newMem(size, pcFile, iLine);
//#ifdef _DEBUG
//	memset(result, 0xDA, size);
//#endif
//	return result;
//}
//void* __cdecl operator new[](size_t size, char* pcFile, int iLine)
//{
//	void* result = g_DLL->newMem(size, pcFile, iLine);
//#ifdef _DEBUG
//	memset(result, 0xDA, size);
//#endif
//	return result;
//}
//
//void __cdecl operator delete(void* pvMem, char* pcFile, int iLine)
//{
//	g_DLL->delMem(pvMem, pcFile, iLine);
//}
//
//void __cdecl operator delete[](void* pvMem, char* pcFile, int iLine)
//{
//	g_DLL->delMem(pvMem, pcFile, iLine);
//}
//
//void* reallocMem(void* a, unsigned int uiBytes, const char* pcFile, int iLine)
//{
//	void* result;
//
//	result = g_DLL->reallocMem(a, uiBytes, pcFile, iLine);
//
//	return result;
//}
//
//unsigned int memSize(void* a)
//{
//	unsigned int result = g_DLL->memSize(a);
//
//	return result;
//}

#endif // CvAllocator_h__
