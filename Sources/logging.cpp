#include "CvGameCoreDLL.h"

#include "logging.h"

namespace logging
{
	void log_json_event(const char* type, const JsonValues& values)
	{
#ifndef FINAL_RELEASE
		picojson::value::object obj;
		for (std::vector<JsonValue>::const_iterator itr = values.values.begin(); itr != values.values.end(); ++itr)
		{
			obj[itr->name] = itr->value;
		}
		obj["type"] = picojson::value(type);
		gDLL->logMsg("EventsJson.log", picojson::value(obj).serialize().c_str());
#endif
	}
}