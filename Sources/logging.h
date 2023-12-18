#pragma once

#ifndef logging_h__
#define logging_h__

namespace logging
{
	//template <class Ty_> struct JsonConvert
	//{
	//	static const Ty_& convert(const Ty_& value)
	//	{
	//		return static_cast<Ty_>(value);
	//	};
	//};
	//template <> struct JsonConvert<int> { static double convert(int value) { return static_cast<double>(value); }; };
	//template <> struct JsonConvert<size_t> { static double convert(size_t value) { return static_cast<double>(value); }; };
	//template <> struct JsonConvert<float> { static double convert(float value) { return static_cast<double>(value); }; };

	

	void logMsg(const char* file, const char* msg, ...);
	void logMsgW(const char* file, const wchar_t* msg, ...);

	void deleteLogs();
}

#ifdef _DEBUG
#define DEBUG_LOG logging::logMsg
#else
#define DEBUG_LOG ;
#endif

#endif // logging_h__