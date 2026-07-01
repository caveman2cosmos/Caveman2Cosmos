#include "CvGameCoreDLL.h"
#include "CvGlobals.h"

// Log levels:
// 0 - None
// 1 - Important decisions only
// 2 - Many decisions
// 3 - All logging
// 4 - All logging with very often trace (unit/promotion eval)
int gPlayerLogLevel = 0;
int gTeamLogLevel = 0;
int gCityLogLevel = 0;
int gUnitLogLevel = 0;

void logContractBroker(int level, const char* format, ...)
{
	if (level <= gPlayerLogLevel)
	{
		static char buf[2048];
		_vsnprintf(buf, 2048 - 4, format, (char*)(&format + 1));
		gDLL->logMsg("ContractBroker.log", buf);

		// Echo to debugger
		strcat(buf, "\n");
		OutputDebugString(buf);
	}

}

void logBuildEvaluation(int level, const char* format, ...)
{
	if (level <= gPlayerLogLevel)
	{
		static char buf[2048];
		_vsnprintf(buf, 2048 - 4, format, (char*)(&format + 1));
		gDLL->logMsg("BuildEvaluation.log", buf);

		// Echo to debugger
		strcat(buf, "\n");
		OutputDebugString(buf);
	}
}

void logHunterAI(int level, const char* format, ...)
{
	if (level <= gUnitLogLevel)
	{
		static char buf[2048];
		_vsnprintf(buf, 2048 - 4, format, (char*)(&format + 1));
		gDLL->logMsg("HunterAI.log", buf);

		// Echo to debugger
		strcat(buf, "\n");
		OutputDebugString(buf);
	}
}

// AI player flavour & decision logging -- gated by gPlayerLogLevel, mirrors the
// CvWorkerAI/CvHunterAI tagged-taxonomy model. Tags: [DAI/*]; see CvDecisionAI.h.
void logDecisionAI(int level, const char* format, ...)
{
	if (level <= gPlayerLogLevel)
	{
		static char buf[2048];
		_vsnprintf(buf, 2048 - 4, format, (char*)(&format + 1));
		gDLL->logMsg("DecisionAI.log", buf);

		// Echo to debugger
		strcat(buf, "\n");
		OutputDebugString(buf);
	}
}

// AI diplomacy / trade-deal decision logging -- [DIP/*] tags, gated by gPlayerLogLevel.
// Explains how the AI values and accepts/rejects trade offers (CvPlayerAI::AI_dealVal /
// AI_considerOffer). Part of the per-subsystem tagged-log family ([WAI]/[HAI]/[DAI]/[DIP]).
void logDiploAI(int level, const char* format, ...)
{
	if (level <= gPlayerLogLevel)
	{
		static char buf[2048];
		_vsnprintf(buf, 2048 - 4, format, (char*)(&format + 1));
		gDLL->logMsg("DiploAI.log", buf);

		// Echo to debugger
		strcat(buf, "\n");
		OutputDebugString(buf);
	}
}

// AI team war/strategy decision logging -- [WAR/*] tags, gated by gTeamLogLevel.
// [WAR/begin] (lvl 1) is a once/turn baseline per AI team (enemy-power + funding context);
// [WAR/area] (lvl 1) logs per-area military posture changes and [WAR/warplan] (lvl 1)
// warplan transitions (CvTeamAI). NB: needs the "Team" BBAI autolog level option > 0.
// Part of the per-subsystem tagged-log family ([WAI]/[HAI]/[DAI]/[DIP]/[WAR]).
void logWarAI(int level, const char* format, ...)
{
	if (level <= gTeamLogLevel)
	{
		static char buf[2048];
		_vsnprintf(buf, 2048 - 4, format, (char*)(&format + 1));
		gDLL->logMsg("WarAI.log", buf);

		// Echo to debugger
		strcat(buf, "\n");
		OutputDebugString(buf);
	}
}

// AI unit behaviour logging -- [UNT/*] tags, gated by gUnitLogLevel. Surfaces the
// per-unit decision flow (CvUnitAI): [UNT/move] (lvl 2) = which UNITAI routine the unit
// ran; [UNT/act] (lvl 2, CvUnitAI::AI_logAct) = the "why" -- the decision helper that won
// the move cascade (e.g. decision=retreatToCity reason=danger), emitted at each helper's
// commit point so only the winning action logs; [UNT/role] (lvl 1) = UNITAI reassignment;
// [UNT/mission] (lvl 2, CvSelectionGroupAI::AI_setMissionAI) = the committed MissionAI +
// target. Part of the per-subsystem tagged-log family ([WAI]/[HAI]/[DAI]/[DIP]/[UNT]).
void logUnitAI(int level, const char* format, ...)
{
	if (level <= gUnitLogLevel)
	{
		static char buf[2048];
		_vsnprintf(buf, 2048 - 4, format, (char*)(&format + 1));
		gDLL->logMsg("UnitAI.log", buf);

		// Echo to debugger
		strcat(buf, "\n");
		OutputDebugString(buf);
	}
}

// AI city production logging -- [CIT/*] tags, gated by gCityLogLevel. Decision side
// (CvCityAI): [CIT/begin] production-choice context, [CIT/order] the order a city commits
// to. Pipeline side (CvCity::pushOrder/popOrder/doProduction):
//   [CIT/push] (lvl 2)         -- order enters the queue (intake; catches contract-driven units)
//   [CIT/push/reject] (lvl 2)  -- queue anti-spam guard blocked another copy of a unit/building
//   [CIT/produced] (lvl 1)     -- unit/building/project actually completes; UNIT lines carry
//                                 ownerHas (count of that exact unit) + aiRoleHas (count of that
//                                 UNITAI) to surface unit-spam outliers (dogs/guards/healers)
//   [CIT/cancel] (lvl 1)       -- order popped without finishing (switch/abandon/obsolete); thrash
//   [CIT/waste] (lvl 1)        -- production overflowed the cap and was burned to gold
//   [CIT/prop] (lvl 2)         -- property-control production gate inputs (val/change/eval/check +
//                                 getting/good/maxed veto flags); fire=1 -> orders a prop building/unit
//   [CIT/proplevel] (lvl 1)    -- per-city property snapshot at turn start (val + change per active
//                                 property), for tracking crime/disease/education TRENDS over turns
// Part of the per-subsystem tagged-log family ([WAI]/[HAI]/[DAI]/[DIP]/[CIT]).
void logCityAI(int level, const char* format, ...)
{
	if (level <= gCityLogLevel)
	{
		static char buf[2048];
		_vsnprintf(buf, 2048 - 4, format, (char*)(&format + 1));
		gDLL->logMsg("CityAI.log", buf);

		// Echo to debugger
		strcat(buf, "\n");
		OutputDebugString(buf);
	}
}

// AI group/army coordination logging -- [GRP/*] tags, gated by gUnitLogLevel.
// Surfaces above-unit coordination (CvArmy missions/leader, group splits) -- the
// seam from which CvPlayer-level unit orchestration should grow. Part of the
// per-subsystem tagged-log family ([WAI]/[HAI]/[DAI]/[DIP]/[UNT]/[GRP]).
void logGroupAI(int level, const char* format, ...)
{
	if (level <= gUnitLogLevel)
	{
		static char buf[2048];
		_vsnprintf(buf, 2048 - 4, format, (char*)(&format + 1));
		gDLL->logMsg("GroupAI.log", buf);

		// Echo to debugger
		strcat(buf, "\n");
		OutputDebugString(buf);
	}
}

// AI espionage decision logging -- [ESP/*] tags, gated by gPlayerLogLevel.
// Surfaces which espionage mission a spy chooses (CvPlayerAI). Part of the
// per-subsystem tagged-log family ([WAI]/[HAI]/[DAI]/[DIP]/[UNT]/[GRP]/[ESP]).
void logEspionageAI(int level, const char* format, ...)
{
	if (level <= gPlayerLogLevel)
	{
		static char buf[2048];
		_vsnprintf(buf, 2048 - 4, format, (char*)(&format + 1));
		gDLL->logMsg("EspionageAI.log", buf);

		// Echo to debugger
		strcat(buf, "\n");
		OutputDebugString(buf);
	}
}

// AI city-founding decision logging -- [FND/*] tags, gated by gPlayerLogLevel.
// Surfaces which city site a settler commits to (CvUnitAI::AI_found). Part of the
// per-subsystem tagged-log family ([WAI]/[HAI]/[DAI]/[DIP]/[UNT]/[GRP]/[ESP]/[FND]).
void logFoundAI(int level, const char* format, ...)
{
	if (level <= gPlayerLogLevel)
	{
		static char buf[2048];
		_vsnprintf(buf, 2048 - 4, format, (char*)(&format + 1));
		gDLL->logMsg("FoundAI.log", buf);

		// Echo to debugger
		strcat(buf, "\n");
		OutputDebugString(buf);
	}
}

// AI combat decision logging -- [COM/*] tags, gated by gUnitLogLevel. Surfaces the
// attack odds the AI computes ([COM/odds], lvl 3, CvSelectionGroupAI AI_attackOdds), the
// go/no-go threshold it requires ([COM/threshold], lvl 3, CvUnitAI AI_finalOddsThreshold),
// and the attack target a unit ultimately commits to ([COM/decision], lvl 2, the
// AI_cityAttack/AI_anyAttack/AI_leaveAttack commit points -- closes the odds-vs-bar loop).
// Part of the per-subsystem tagged-log family ([WAI]/[HAI]/[DAI]/[DIP]/[UNT]/[GRP]/[ESP]/[FND]/[COM]).
void logCombatAI(int level, const char* format, ...)
{
	if (level <= gUnitLogLevel)
	{
		static char buf[2048];
		_vsnprintf(buf, 2048 - 4, format, (char*)(&format + 1));
		gDLL->logMsg("CombatAI.log", buf);

		// Echo to debugger
		strcat(buf, "\n");
		OutputDebugString(buf);
	}
}

// Game session header -- [GAME/*] -> GameInfo.log. Written once on game start/load
// (from CvGame::onFinalInitialized) so every other AI log can be read against the
// active options/rules/setup. No level gate; the caller decides when to emit.
void logGameInfo(const char* format, ...)
{
	static char buf[2048];
	_vsnprintf(buf, 2048 - 4, format, (char*)(&format + 1));
	gDLL->logMsg("GameInfo.log", buf);

	// Echo to debugger
	strcat(buf, "\n");
	OutputDebugString(buf);
}

void logAIJson(CvWString type, CvWString identifier, CvWString squirrel, CvWString message)
{
	const std::wstring data = "{ type: \"" + type + "\" name: \"" + identifier + "\" function: \" " + squirrel + "\" message: \"" + message + "\" }";
	logging::logMsgW("FLB.log", data.c_str());
}

void logCB(CvString message) {

	const std::string data = message;
	logging::logMsg("CB.log", data.c_str());
}

void logToFile(CvString message, const char* filename) {
	const std::string data = message;
	logging::logMsg(filename, data.c_str());
}
