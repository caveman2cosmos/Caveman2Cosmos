#pragma once
#include <vector>
#include <utility>

// A handful of cities, read only while one decision is being scored. Nothing
// survives the caller or an issued order, so no gameplay invalidations are needed.
template<class City>
inline bool findDecisionCityValue(const std::vector<std::pair<const City*, int> >& values,
	const City* city, int& result)
{
	for (typename std::vector<std::pair<const City*, int> >::const_iterator it = values.begin(); it != values.end(); ++it)
	{
		if (it->first == city)
		{
			result = it->second;
			return true;
		}
	}
	return false;
}
