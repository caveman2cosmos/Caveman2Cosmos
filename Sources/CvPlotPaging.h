#pragma once

struct CvPlotPaging
{
	typedef int paging_handle;
	static const int INVALID_HANDLE;

	static void ResetPaging();
	static void UpdatePaging();

	static paging_handle AddPlot(class CvPlot* plot);
	static void RemovePlot(paging_handle handle);
};