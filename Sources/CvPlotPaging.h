#pragma once

#ifndef CvPlotPaging_h__
#define CvPlotPaging_h__

struct CvPlotPaging
{
	typedef int paging_handle;
	static const int INVALID_HANDLE;

	static void ResetPaging();
	static void UpdatePaging();

	static paging_handle AddPlot(class CvPlot* plot);
	static void RemovePlot(paging_handle handle);
};

#endif // CvPlotPaging_h__