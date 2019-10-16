#pragma once

namespace CvPython
{
	template < class ReturnValue >
	ReturnValue call(const char* const callingFunction, const char* const module, const char* const function, ...)
	{
		std::vector<int> coords;
		bool bOK = false;
		CyArgsList argsList;

		va_list args;
		va_start(args, function);

		argsList.add(0);
		bOK = PYTHON_CALL_FUNCTION4(__FUNCTION__, PYScreensModule, "WorldBuilderGetHighlightPlot", argsList.makeFunctionArgs(), &coords);
		if (bOK && !coords.empty())
		{
			pNewPlot = GC.getMap().plot(coords[0], coords[1]);
		}
	}
}