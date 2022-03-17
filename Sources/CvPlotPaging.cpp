
#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvMap.h"
#include "CvPlotPaging.h"

#include <psapi.h>

namespace {

	struct GraphicsPagingInfo
	{
		CvPlot* pPlot;
		unsigned int iSeq;
		GraphicsPagingInfo () : pPlot(NULL), iSeq(0U) {}
	};

	unsigned int g_iNumPagedInPlots = 0;
	unsigned int g_iCurrentSeq = 0;
	unsigned int g_iOldestSearchSeqHint = 0;
	std::vector<GraphicsPagingInfo> g_pagingTable;

	int g_iLastLookatX = -1;
	int g_iLastLookatY = -1;
	bool g_bWasGraphicsPagingEnabled = true;

	int findFreePagingTableSlot()
	{
		static size_t iSearchStartHintIndex = 0;

		for (size_t iI = 0; iI < g_pagingTable.size(); iI++)
		{
			int iIndex = iSearchStartHintIndex++;

			if (iSearchStartHintIndex >= g_pagingTable.size())
			{
				iSearchStartHintIndex = 0;
			}

			if (g_pagingTable[iIndex].pPlot == NULL)
			{
				return iIndex;
			}
		}

		return -1;
	}

	int allocateNewPagingEntry()
	{
		if (g_pagingTable.size() <= g_iNumPagedInPlots++)
		{
			g_pagingTable.push_back(GraphicsPagingInfo());
			return g_pagingTable.size() - 1;
		}
		else
		{
			return findFreePagingTableSlot();
		}
	}

	int findOldestEvictablePagingEntry()
	{
		unsigned int iOldest = MAX_INT;
		int iResult = -1;

		for (int iI = 0; iI < static_cast<int>(g_pagingTable.size()); iI++)
		{
			if (g_pagingTable[iI].pPlot != NULL && g_pagingTable[iI].pPlot->getNonRequiredGraphicsMask() != ECvPlotGraphics::NONE)
			{
				if (g_pagingTable[iI].iSeq < iOldest)
				{
					iOldest = g_pagingTable[iI].iSeq;
					iResult = iI;
				}
			}
		}

		return iResult;
	}

	static const unsigned int DEFAULT_MAX_WORKING_SET_THRESHOLD_BEFORE_EVICTION = 1024U * 1024U * 1024U * 2U; // 2 GB
	static const unsigned int DEFAULT_OS_MEMORY_ALLOWANCE = 1024U * 1024U * 512U; // 512 MB

	bool NeedToFreeMemory()
	{
		PROCESS_MEMORY_COUNTERS pmc;
		static unsigned int uiMaxMem = 0xFFFFFFFF;
		static const unsigned int PAGING_TEST_ALLOC = GC.getDefineINT("PAGING_TEST_ALLOC", 0);
		static const unsigned int PAGING_TEST_ALLOC_NUM = GC.getDefineINT("PAGING_TEST_ALLOC_NUM", 0);

		if (PAGING_TEST_ALLOC != 0 && PAGING_TEST_ALLOC_NUM != 0)
		{
			std::vector<void*> test_allocs;
			bool success = true;
			for (unsigned int i = 0; i < PAGING_TEST_ALLOC_NUM; ++i)
			{
				void* test_alloc = malloc(PAGING_TEST_ALLOC);
				if (test_alloc == NULL)
				{
					success = false;
					break;
				}
				else
				{
					test_allocs.push_back(test_alloc);
				}
			}
			for (size_t i = 0; i < test_allocs.size(); ++i)
			{
				free(test_allocs[i]);
			}

			return false;
		}
		else
		{
			if (uiMaxMem == 0xFFFFFFFF)
			{
				MEMORYSTATUSEX memoryStatus;

				memoryStatus.dwLength = sizeof(memoryStatus);
				GlobalMemoryStatusEx(&memoryStatus);

				uiMaxMem = 1024 * GC.getDefineINT("MAX_DESIRED_MEMORY_USED", DEFAULT_MAX_WORKING_SET_THRESHOLD_BEFORE_EVICTION / 1024);

				DWORDLONG usableMemory = memoryStatus.ullTotalPhys - 1024 * (DWORDLONG)GC.getDefineINT("OS_MEMORY_ALLOWANCE", DEFAULT_OS_MEMORY_ALLOWANCE / 1024);
				if (usableMemory < uiMaxMem)
				{
					uiMaxMem = (unsigned int)usableMemory;
				}
			}

			GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));

			if (pmc.WorkingSetSize > uiMaxMem)
			{
				OutputDebugString(CvString::format("Found need to free memory: %u used vs %u target\n", pmc.WorkingSetSize, uiMaxMem).c_str());
				return true;
			}
			else
			{
				return false;
			}
		}
	}
}

bool EvictGraphics()
{
	int iEvictionIndex = findOldestEvictablePagingEntry();

	if (iEvictionIndex == -1)
	{
		return false;
	}

	g_pagingTable[iEvictionIndex].pPlot->hideNonRequiredGraphics();
	return true;
}

struct PlotDist
{
	CvPlot* plot;
	int dist2;

	PlotDist(CvPlot* plot, int dist2) : plot(plot), dist2(dist2)
	{
	}

	PlotDist() : plot(NULL), dist2(0) {}

	bool operator<(const PlotDist& other) const
	{
		return dist2 < other.dist2;
	}
};

CvPlotPaging::paging_handle CvPlotPaging::AddPlot(CvPlot* plot)
{
	int newHandle = allocateNewPagingEntry();
	FAssertMsg(newHandle >= 0, "Couldn't allocate plot paging entry!");

	GraphicsPagingInfo* pPagingInfo = &g_pagingTable[newHandle];

	pPagingInfo->iSeq = ++g_iCurrentSeq;
	pPagingInfo->pPlot = plot;

	return newHandle;
}

void CvPlotPaging::RemovePlot(CvPlotPaging::paging_handle handle)
{
	FAssertMsg(handle >= 0 && handle < static_cast<CvPlotPaging::paging_handle>(g_pagingTable.size()), "CvPlot paging handle is not valid");
	FAssertMsg(g_pagingTable[handle].pPlot != NULL, "CvPlot was not paged");
	g_pagingTable[handle].pPlot = NULL;
	--g_iNumPagedInPlots;
}
void CvPlotPaging::ResetPaging()
{
	g_bWasGraphicsPagingEnabled = true;
	g_iLastLookatX = -1;
	g_iLastLookatY = -1;
}

// Straight line distance squared, wrapped in x and y direction.
// It doesn't really matter that some maps don't wrap, perf of the paging will
// be consistent regardless.
int ToroidalDistanceSq (int x1, int y1, int x2, int y2, int w, int h)
{
	int dx = std::abs(x2 - x1);
	int dy = std::abs(y2 - y1);

	if (dx > w / 2)
		dx = w - dx;

	if (dy > h / 2)
		dy = h - dy;

	return dx * dx + dy * dy;
}


void CvPlotPaging::UpdatePaging()
{
	// Check if the paging setting changed
	bool bPagingEnabled = getBugOptionBOOL("MainInterface__EnableGraphicalPaging", true);

	if(bPagingEnabled || (!bPagingEnabled && g_bWasGraphicsPagingEnabled))
	{
		const CvPlot* lookatPlot = gDLL->getInterfaceIFace()->getLookAtPlot();
		if (lookatPlot == NULL)
		{
			return;
		}

		const int centerX = lookatPlot->getX();
		const int centerY = lookatPlot->getY();

		// Gather and sort all plots by distance to view center
		std::vector<PlotDist> plots;
		const CvMap& map = GC.getMap();
		plots.reserve(map.numPlots());
		for (int i = 0; i < map.numPlots(); i++)
		{
			CvPlot* plot = map.plotByIndex(i);
			if (plot != NULL)
			{
				plots.push_back(PlotDist(map.plotByIndex(i), ToroidalDistanceSq(centerX, centerY, plot->getX(), plot->getY(), map.getGridWidth(), map.getGridHeight())));
			}
		}
		algo::sort(plots);

		if (!bPagingEnabled && g_bWasGraphicsPagingEnabled)
		{
			// How much time we can allow for paging this frame, it varies by how fast the camera is moving
			const int pagingFrameTime = 10 + GC.getDefineINT("PAGING_FRAME_TIME_MS", 100);

			win32::Stopwatch pageTimer;
			pageTimer.Start();

			//const CvMap& map = GC.getMap();

			bool timedout = false;
			for (std::vector<PlotDist>::iterator itr = plots.begin(); !timedout && itr != plots.end(); ++itr)
			{
				itr->plot->disableGraphicsPaging();
				timedout = pageTimer.ElapsedMilliseconds() > pagingFrameTime;
			}
			// Keep paging flag off until we make it through all the tiles
			g_bWasGraphicsPagingEnabled = timedout;
		}
		else if (bPagingEnabled)
		{
			// How far did the camera move since last frame?
			const int moveDist2 = (centerX - g_iLastLookatX) * (centerX - g_iLastLookatX) + (centerY - g_iLastLookatY) * (centerY - g_iLastLookatY);

			// How much time we can allow for paging this frame, it varies by how fast the camera is moving
			const int pagingFrameTime = 10 + GC.getDefineINT("PAGING_FRAME_TIME_MS", 100) / (1 + moveDist2);

			win32::Stopwatch pageTimer;
			pageTimer.Start();

			// Evict graphics first, allowing at least 1/2 our time budget for it (we definitely want to make sure we don't run out of memory!)
			while (NeedToFreeMemory() && pageTimer.ElapsedMilliseconds() < pagingFrameTime * 0.5)
			{
				if (!EvictGraphics())
				{
					// we in trouble now!
					break;
				}
			}

			// The min distance different plot graphics should be paged in at
			const int pageInDistances[] = {
				GC.getDefineINT("PAGE_IN_DIST_SYMBOLS", 100),
				GC.getDefineINT("PAGE_IN_DIST_FEATURES", 7),
				GC.getDefineINT("PAGE_IN_DIST_RIVER", 40),
				GC.getDefineINT("PAGE_IN_DIST_ROUTE", 100),
				GC.getDefineINT("PAGE_IN_DIST_UNIT", 15),
				GC.getDefineINT("PAGE_IN_DIST_CITY", 5),
			};


			// Loop all plots in order from distance to the view center, toggling the appropriate plot graphics flags
			for (std::vector<PlotDist>::iterator itr = plots.begin(); itr != plots.end() && pageTimer.ElapsedMilliseconds() < pagingFrameTime; ++itr)
			{
				for (int graphicsType = 0; graphicsType < ECvPlotGraphics::NUM; ++graphicsType)
				{
					if (pageInDistances[graphicsType] != -1)
					{
						itr->plot->setRequireGraphicsVisible(ECvPlotGraphics::fromIndex(graphicsType), itr->dist2 < pageInDistances[graphicsType] * pageInDistances[graphicsType]);
					}
				}
			}

			g_iLastLookatX = lookatPlot->getX();
			g_iLastLookatY = lookatPlot->getY();

			g_bWasGraphicsPagingEnabled = true;
		}
	}
}

const int CvPlotPaging::INVALID_HANDLE = -1;
