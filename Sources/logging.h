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

	template < class Ty_ >
	static picojson::value make_json_value(Ty_ value)
	{
		return picojson::value(value);
	}

	template <>
	static picojson::value make_json_value(int value)
	{
		return picojson::value(static_cast<double>(value));
	}

	template <>
	static picojson::value make_json_value(size_t value)
	{
		return picojson::value(static_cast<double>(value));
	}

	template <>
	static picojson::value make_json_value(float value)
	{
		return picojson::value(static_cast<double>(value));
	}

	template < class Ty_ >
	static picojson::value make_json_value(const std::vector<Ty_>& inarr)
	{
		picojson::value::array arr;
		for (std::vector<Ty_>::const_iterator itr = inarr.begin(); itr != inarr.end(); ++itr)
		{
			arr.push_back(make_json_value(*itr));
		}
		return picojson::value(arr);
	}

	struct JsonValue
	{
		std::string name;
		picojson::value value;

		JsonValue() {}
		template <class Ty_>
		JsonValue(const std::string& name, const Ty_& value)
			: name(name)
			, value(make_json_value(value))
		{}
		//JsonValue(const std::string& name, bool value) : name(name), value(picojson::value(value)) {}
		//JsonValue(const std::string& name, size_t value) : name(name), value(picojson::value(static_cast<double>(value))) {}
		//JsonValue(const std::string& name, int value) : name(name), value(picojson::value(static_cast<double>(value))) {}
		//JsonValue(const std::string& name, float value) : name(name), value(picojson::value(static_cast<double>(value))) {}
		//JsonValue(const std::string& name, const std::string& value) : name(name), value(picojson::value(value)) {}
		//template < class Ty_ >
		//JsonValue(const std::string& name, const std::vector<Ty_>& inarr) : name(name)
		//{
		//	picojson::value::array arr;
		//	for (std::vector<Ty_>::const_iterator itr = inarr.begin(); itr != inarr.end(); ++itr)
		//	{
		//		arr.push_back(make_json_value(*itr)));
		//	}
		//	value = picojson::value(arr);
		//}
	};

	struct JsonValues
	{
		std::vector<JsonValue> values;
		JsonValues& operator <<(const JsonValue& val)
		{
			values.push_back(val);
			return *this;
		}
	};

	void log_json_event(const char* type, const JsonValues& values);

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