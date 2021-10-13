#include "CvGameCoreDLL.h"
#include "CvCity.h"
#include "CvDllPythonEvents.h"
#include "CvGlobals.h"
#include "CvPlot.h"
#include "CvPython.h"
#include "CvSelectionGroup.h"
#include "CvUnit.h"


namespace logging {
	template <>
	static picojson::value make_json_value(CvWString value)
	{
		return picojson::value(CvString(value));
	}

	template < class Fn_, class Ty_ >
	static picojson::object info_base(Ty_ id, Fn_ infoFn)
	{
		picojson::object obj;
		obj["id"] = make_json_value(static_cast<int>(id));
		if (id != -1)
		{
			const CvInfoBase& info = (GC.*infoFn)(id);
			obj["desc"] = make_json_value(CvWString(info.getDescription()));
		}
		return obj;
	}
	template <>
	static picojson::value make_json_value(UnitTypes value)
	{
		return picojson::value(info_base(value, &cvInternalGlobals::getUnitInfo));
	}
	template <>
	static picojson::value make_json_value(FeatureTypes value)
	{
		return picojson::value(info_base(value, &cvInternalGlobals::getFeatureInfo));
	}
	template <>
	static picojson::value make_json_value(BuildingTypes value)
	{
		return picojson::value(info_base(value, &cvInternalGlobals::getBuildingInfo));
	}
	template <>
	static picojson::value make_json_value(ProjectTypes value)
	{
		return picojson::value(info_base(value, &cvInternalGlobals::getProjectInfo));
	}
	template <>
	static picojson::value make_json_value(ProcessTypes value)
	{
		return picojson::value(info_base(value, &cvInternalGlobals::getProcessInfo));
	}
	template <>
	static picojson::value make_json_value(HurryTypes value)
	{
		return picojson::value(info_base(value, &cvInternalGlobals::getHurryInfo));
	}
	template <>
	static picojson::value make_json_value(MissionTypes value)
	{
		return picojson::value(info_base(value, &cvInternalGlobals::getMissionInfo));
	}
	template <>
	static picojson::value make_json_value(PromotionTypes value)
	{
		return picojson::value(info_base(value, &cvInternalGlobals::getPromotionInfo));
	}

	template <>
	static picojson::value make_json_value(ImprovementTypes value)
	{
		return picojson::value(info_base(value, &cvInternalGlobals::getImprovementInfo));
	}
	template <>
	static picojson::value make_json_value(RouteTypes value)
	{
		return picojson::value(info_base(value, &cvInternalGlobals::getRouteInfo));
	}
	template <>
	static picojson::value make_json_value(ReligionTypes value)
	{
		return picojson::value(info_base(value, &cvInternalGlobals::getReligionInfo));
	}
	template <>
	static picojson::value make_json_value(BuildTypes value)
	{
		return picojson::value(info_base(value, &cvInternalGlobals::getBuildInfo));
	}
	template <>
	static picojson::value make_json_value(GoodyTypes value)
	{
		return picojson::value(info_base(value, &cvInternalGlobals::getGoodyInfo));
	}
	template <>
	static picojson::value make_json_value(CivicTypes value)
	{
		return picojson::value(info_base(value, &cvInternalGlobals::getCivicInfo));
	}
	template <>
	static picojson::value make_json_value(TechTypes value)
	{
		return picojson::value(info_base(value, &cvInternalGlobals::getTechInfo));
	}
	template <>
	static picojson::value make_json_value(CorporationTypes value)
	{
		return picojson::value(info_base(value, &cvInternalGlobals::getCorporationInfo));
	}
	template <>
	static picojson::value make_json_value(VictoryTypes value)
	{
		return picojson::value(info_base(value, &cvInternalGlobals::getVictoryInfo));
	}

	template <>
	static picojson::value make_json_value(PlayerTypes value)
	{
		picojson::object obj;
		obj["id"] = make_json_value((int)value);
		if(value != NO_PLAYER)
		{
			const CvPlayerAI& player = CvPlayerAI::getPlayer(value);
			obj["name"] = make_json_value(CvString(player.getName()));
		}
		return picojson::value(obj);
	}

	template <>
	static picojson::value make_json_value(TeamTypes value)
	{
		picojson::object obj;
		obj["id"] = make_json_value((int)value);
		if(value != NO_TEAM)
		{
			const CvTeamAI& team = CvTeamAI::getTeam(value);
			obj["name"] = make_json_value(CvString(team.getName()));
		}
		return picojson::value(obj);
	}

	template <>
	static picojson::value make_json_value(CvUnit* value)
	{
		picojson::object obj;
		if(value != NULL)
		{
			obj["id"] = make_json_value(value->getID());
			obj["name"] = make_json_value(value->getName());
			obj["owner"] = make_json_value(value->getOwner());
		}
		else
		{
			obj["id"] = make_json_value(-1);
		}
		return picojson::value(obj);
	}

	template <>
	static picojson::value make_json_value(CvCity* value)
	{
		picojson::object obj;
		if(value != NULL)
		{
			obj["id"] = make_json_value(value->getID());
			obj["name"] = make_json_value(value->getName());
			obj["owner"] = make_json_value(value->getOwner());
		}
		else
		{
			obj["id"] = make_json_value(-1);
		}
		return picojson::value(obj);
	}

	template <>
	static picojson::value make_json_value(CvPlot* value)
	{
		picojson::object obj;
		if (value != NULL)
		{
			obj["x"] = make_json_value(value->getX());
			obj["y"] = make_json_value(value->getY());
			obj["owner"] = make_json_value(value->getOwner());
		}
		else
		{
			obj["id"] = make_json_value(-1);
		}
		return picojson::value(obj);
	}
};

struct EventArgs
{
	Cy::Args pyArgs;
	logging::JsonValues jsonArgs;
	bool toJson;

	EventArgs() : toJson(true) {}

	EventArgs& no_json()
	{
		toJson = false;
		return *this;
	}

	template < class Ty_ >
	EventArgs& arg(const std::string& name, const Ty_& value)
	{
		pyArgs.add(value);
		jsonArgs << logging::JsonValue(name, value);
		return *this;
	}

	//template < class Ty_ >
	//EventArgs& arg(const std::string& name, const Cy::Array<Ty_>& value)
	//{
	//	pyArgs.add(value);
	//	std::vector<Ty_> arr;
	//	for (int idx = 0; idx < value.len; ++idx)
	//	{
	//		arr.push_back(value.vals[idx]);
	//	}
	//	jsonArgs << logging::JsonValue(name, arr);
	//	return *this;
	//}

	template < class Ty_, class Ty2_ >
	EventArgs& arg(const std::string& name, const Ty_& value, const Ty2_& valueJson)
	{
		pyArgs.add(value);
		jsonArgs << logging::JsonValue(name, valueJson);
		return *this;
	}

	template < class Ty_ >
	EventArgs& argPy(const Ty_& value)
	{
		pyArgs.add(value);
		return *this;
	}
};

CvString create_session_id()
{
	time_t rawtime;
	struct tm* timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
	return CvString(buffer);
}

static const CvString gSessionID = create_session_id();

bool postEvent(EventArgs eventData, const char* eventName)
{
	FPythonAssert(gDLL->getPythonIFace()->isInitialized(), "CvEventInterface", "onEvent");

	if (eventData.toJson)
	{
		eventData.jsonArgs << logging::JsonValue("game_name", GC.getInitCore().getGameName());
		eventData.jsonArgs << logging::JsonValue("game_id", GC.getGame().getGameId());
		eventData.jsonArgs << logging::JsonValue("session_id", gSessionID);

		logging::log_json_event("pyevent", eventData.jsonArgs);
	}

	eventData.pyArgs << GC.getGame().isDebugMode();
	eventData.pyArgs << false;
	eventData.pyArgs << gDLL->altKey();
	eventData.pyArgs << gDLL->ctrlKey();
	eventData.pyArgs << gDLL->shiftKey();
	eventData.pyArgs << (gDLL->getChtLvl() > 0);

#ifdef FP_PROFILE_ENABLE				// Turn Profiling On or Off ..
#ifdef USE_INTERNAL_PROFILER
	static	std::map<int,ProfileSample*>*	g_pythonProfiles = NULL;

	if ( g_pythonProfiles == NULL )
	{
		g_pythonProfiles = new std::map<int,ProfileSample*>();
	}

	CvChecksum xSum;
	const char* ptr;
	ProfileSample* pSample;

	for(ptr = eventName; *ptr != '\0'; ptr++)
	{
		xSum.add((uint8_t)*ptr);
	}

	std::map<int,ProfileSample*>::const_iterator itr = g_pythonProfiles->find(xSum.get());

	if ( itr == g_pythonProfiles->end() )
	{
		char profileName[256];

		sprintf(profileName, "CvDllPythonEvents::postEvent.%s", eventName);
		pSample = new ProfileSample(profileName);

		g_pythonProfiles->insert(std::make_pair(xSum.get(), pSample));
	}
	else
	{
		pSample = itr->second;
	}

	CProfileScope detailedScope(pSample);
#endif
#endif

	return Cy::call<bool>(PYEventModule, "onEvent", eventData.pyArgs);
}

bool CvDllPythonEvents::reportKbdEvent(int evt, int key, int iCursorX, int iCursorY)
{
	NiPoint3 pt3Location;
	const CvPlot* pPlot = gDLL->getEngineIFace()->pickPlot(iCursorX, iCursorY, pt3Location);
	EventArgs eventData;
	eventData
		.no_json()
		.arg("event", "kbdEvent")
		.arg("evt", evt)
		.arg("key", key)
		.arg("iCursorX", iCursorX)
		.arg("iCursorY", iCursorY)
		.arg("plotx", (pPlot ? pPlot->getX() : -1))
		.arg("ploty", (pPlot ? pPlot->getY() : -1));
	return postEvent(eventData, "kbdEvent");
}

bool CvDllPythonEvents::reportMouseEvent(int evt, int iCursorX, int iCursorY, bool bInterfaceConsumed)
{
	NiPoint3 pt3Location;
	const CvPlot* pPlot = gDLL->getEngineIFace()->pickPlot(iCursorX, iCursorY, pt3Location);
	// add list of active screens
	std::vector<int> screens;
	gDLL->getInterfaceIFace()->getInterfaceScreenIdsForInput(screens);
	EventArgs eventData;
	eventData
		.no_json()
		.arg("event", "mouseEvent")
		.arg("evt", evt)
		.arg("iCursorX", iCursorX) .arg("iCursorY", iCursorY)
		.arg("plotx", (pPlot ? pPlot->getX() : -1))
		.arg("ploty", (pPlot ? pPlot->getY() : -1))
		.arg("bInterfaceConsumed", bInterfaceConsumed)
		.arg("screens", screens);
	return postEvent(eventData, "mouseEvent");
}

void CvDllPythonEvents::reportModNetMessage(int iData1, int iData2, int iData3, int iData4, int iData5)
{
	EventArgs eventData;
	eventData
		.no_json()
		.arg("event", "ModNetMessage")
		.arg("iData1", iData1)
		.arg("iData2", iData2)
		.arg("iData3", iData3)
		.arg("iData4", iData4)
		.arg("iData5", iData5);
	postEvent(eventData, "ModNetMessage");
}

void CvDllPythonEvents::reportInit()
{
	EventArgs eventData;
	eventData
		.arg("event", "Init");
	postEvent(eventData, "Init");
}

void CvDllPythonEvents::reportUnInit()
{
	EventArgs eventData;
	eventData
		.arg("event", "UnInit");
	postEvent(eventData, "UnInit");
}

void CvDllPythonEvents::reportGameStart()
{
	EventArgs eventData;
	eventData
		.arg("event", "GameStart");
	postEvent(eventData, "GameStart");
}

void CvDllPythonEvents::reportGameEnd(int iGameTurn)
{
	EventArgs eventData;
	eventData
		.arg("event", "GameEnd")
		.arg("iGameTurn", iGameTurn);
	postEvent(eventData, "GameEnd");
}


void CvDllPythonEvents::reportWindowActivation(bool bActive)
{
	EventArgs eventData;
	eventData
		.arg("event", "windowActivation")
		.arg("bActive", bActive);
	postEvent(eventData, "windowActivation");
}


void CvDllPythonEvents::reportMapRegen()
{
	EventArgs eventData;
	eventData
		.arg("event", "MapRegen");
	postEvent(eventData, "MapRegen");
}


void CvDllPythonEvents::reportBeginGameTurn(int iGameTurn)
{
	EventArgs eventData;
	eventData
		.arg("event", "BeginGameTurn")
		.arg("iGameTurn", iGameTurn);
	postEvent(eventData, "BeginGameTurn");
}

void CvDllPythonEvents::reportEndGameTurn(int iGameTurn)
{
	EventArgs eventData;
	eventData
		.arg("event", "EndGameTurn")
		.arg("iGameTurn", iGameTurn);
	postEvent(eventData, "EndGameTurn");
}
/************************************************************************************************/
/* RevolutionDCM MpFix                                                           lemmy101       */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
void CvDllPythonEvents::reportPreEndGameTurn(int iGameTurn)
{
	EventArgs eventData;
	eventData
		.arg("event", "PreEndGameTurn")
		.arg("iGameTurn", iGameTurn);
	postEvent(eventData, "PreEndGameTurn");
}
/************************************************************************************************/
/* RevolutionDCM                           END                                                  */
/************************************************************************************************/

void CvDllPythonEvents::reportBeginPlayerTurn(int iGameTurn, PlayerTypes ePlayer)
{
	EventArgs eventData;
	eventData
		.arg("event", "BeginPlayerTurn")
		.arg("iGameTurn", iGameTurn)
		.arg("ePlayer", ePlayer);
	postEvent(eventData, "BeginPlayerTurn");
}

void CvDllPythonEvents::reportEndPlayerTurn(int iGameTurn, PlayerTypes ePlayer)
{
	EventArgs eventData;
	eventData
		.arg("event", "EndPlayerTurn")
		.arg("iGameTurn", iGameTurn)
		.arg("ePlayer", ePlayer);
	postEvent(eventData, "EndPlayerTurn");
}

void CvDllPythonEvents::reportFirstContact(TeamTypes eTeamID1, TeamTypes eTeamID2)
{
	EventArgs eventData;
	eventData
		.arg("event", "firstContact")
		.arg("eTeamID1", eTeamID1)
		.arg("eTeamID2", eTeamID2);
	postEvent(eventData, "firstContact");
}

void CvDllPythonEvents::reportCombatResult(CvUnit* pWinner, CvUnit* pLoser)
{
	EventArgs eventData;
	eventData
		.arg("event", "combatResult")
		.arg("pWinner", pWinner)
		.arg("pLoser", pLoser);
	postEvent(eventData, "combatResult");
}

// BUG - Combat Events - start
void CvDllPythonEvents::reportCombatRetreat(CvUnit* pAttacker, CvUnit* pDefender)
{
	EventArgs eventData;
	eventData
		.arg("event", "combatRetreat")
		.arg("pAttacker", pAttacker)
		.arg("pDefender", pDefender);
	postEvent(eventData,"combatRetreat");
}

void CvDllPythonEvents::reportCombatWithdrawal(CvUnit* pAttacker, CvUnit* pDefender)
{
	EventArgs eventData;
	eventData
		.arg("event", "combatWithdrawal")
		.arg("pAttacker", pAttacker)
		.arg("pDefender", pDefender);
	postEvent(eventData, "combatWithdrawal");
}

void CvDllPythonEvents::reportCombatLogCollateral(CvUnit* pAttacker, CvUnit* pDefender, int iDamage)
{
	EventArgs eventData;
	eventData
		.arg("event", "combatLogCollateral")
		.arg("pAttacker", pAttacker)
		.arg("pDefender", pDefender)
		.arg("iDamage", iDamage);
	postEvent(eventData, "combatLogCollateral");
}

void CvDllPythonEvents::reportCombatLogFlanking(CvUnit* pAttacker, CvUnit* pDefender, int iDamage)
{
	EventArgs eventData;
	eventData
		.arg("event", "combatLogFlanking")
		.arg("pAttacker", pAttacker)
		.arg("pDefender", pDefender)
		.arg("iDamage", iDamage);
	postEvent(eventData, "combatLogFlanking");
}
// BUG - Combat Events - end

void CvDllPythonEvents::reportImprovementBuilt(int iImprovementType, int iX, int iY)
{
	EventArgs eventData;
	eventData
		.arg("event", "improvementBuilt")
		.arg("iImprovementType", iImprovementType)
		.arg("iX", iX) .arg("iY", iY);
	postEvent(eventData, "improvementBuilt");
}

void CvDllPythonEvents::reportImprovementDestroyed(int iImprovementType, int iPlayer, int iX, int iY)
{
	EventArgs eventData;
	eventData
		.arg("event", "improvementDestroyed")
		.arg("iImprovementType", iImprovementType)
		.arg("iPlayer", iPlayer)
		.arg("iX", iX) .arg("iY", iY);
	postEvent(eventData, "improvementDestroyed");
}

void CvDllPythonEvents::reportRouteBuilt(int iRouteType, int iX, int iY)
{
	EventArgs eventData;
	eventData
		.arg("event", "routeBuilt")
		.arg("iRouteType", iRouteType)
		.arg("iX", iX) .arg("iY", iY);
	postEvent(eventData, "routeBuilt");
}

void CvDllPythonEvents::reportChat(CvWString szString)
{
	EventArgs eventData;
	eventData
		.arg("event", "chat")
		.arg("szString", szString);
	postEvent(eventData, "chat");
}

void CvDllPythonEvents::reportPlotRevealed(CvPlot *pPlot, TeamTypes eTeam)
{
	EventArgs eventData;
	eventData
		.no_json()
		.arg("event", "plotRevealed")
		.arg("pPlot", pPlot)
		.arg("eTeam", eTeam);
	postEvent(eventData, "plotRevealed");
}

void CvDllPythonEvents::reportPlotFeatureRemoved(CvPlot *pPlot, FeatureTypes eFeature, CvCity* pCity)
{
	EventArgs eventData;
	eventData
		.arg("event", "plotFeatureRemoved")
		.arg("pPlot", pPlot)
		.arg("eFeature", eFeature)
		.arg("pCity", pCity);
	postEvent(eventData, "plotFeatureRemoved");
}

void CvDllPythonEvents::reportPlotPicked(CvPlot *pPlot)
{
	EventArgs eventData;
	eventData
		.arg("event", "plotPicked")
		.arg("pPlot", pPlot);
	postEvent(eventData, "plotPicked");
}

void CvDllPythonEvents::reportNukeExplosion(CvPlot *pPlot, CvUnit* pNukeUnit)
{
	EventArgs eventData;
	eventData
		.arg("event", "nukeExplosion")
		.arg("pPlot", pPlot)
		.arg("pNukeUnit", pNukeUnit);
	postEvent(eventData, "nukeExplosion");
}

void CvDllPythonEvents::reportGotoPlotSet(CvPlot *pPlot, PlayerTypes ePlayer)
{
	EventArgs eventData;
	eventData
		.arg("event", "gotoPlotSet")
		.arg("pPlot", pPlot)
		.arg("ePlayer", ePlayer);
	postEvent(eventData, "gotoPlotSet");
}

void CvDllPythonEvents::reportCityBuilt( CvCity *pCity, CvUnit *pUnit )
{
	EventArgs eventData;
	eventData
		.arg("event", "cityBuilt")
		.arg("pCity", pCity);

	if (pUnit)
		eventData.arg("pUnit", pUnit);
	else
		eventData.arg("pUnit", NULL);

	postEvent(eventData, "cityBuilt");
}

void CvDllPythonEvents::reportCityRazed( CvCity *pCity, PlayerTypes ePlayer )
{
	EventArgs eventData;
	eventData
		.arg("event", "cityRazed")
		.arg("pCity", pCity)
		.arg("ePlayer", ePlayer);
	postEvent(eventData, "cityRazed");
}

void CvDllPythonEvents::reportCityAcquired(PlayerTypes eOldOwner, PlayerTypes ePlayer, CvCity* pOldCity, bool bConquest, bool bTrade)
{
	EventArgs eventData;
	eventData
		.arg("event", "cityAcquired")
		.arg("eOldOwner", eOldOwner)
		.arg("ePlayer", ePlayer)
		.arg("pOldCity", pOldCity)
		.arg("bConquest", bConquest)
		.arg("bTrade", bTrade);
	postEvent(eventData, "cityAcquired");
}

void CvDllPythonEvents::reportCityAcquiredAndKept(PlayerTypes ePlayer, CvCity* pOldCity)
{
	EventArgs eventData;
	eventData
		.arg("event", "cityAcquiredAndKept")
		.arg("ePlayer", ePlayer)
		.arg("pOldCity", pOldCity);
	postEvent(eventData, "cityAcquiredAndKept");
}

void CvDllPythonEvents::reportCityLost(CvCity* pCity)
{
	EventArgs eventData;
	eventData
		.arg("event", "cityLost")
		.arg("pCity", pCity);
	postEvent(eventData, "cityLost");
}

void CvDllPythonEvents::reportCultureExpansion( CvCity *pCity, PlayerTypes ePlayer )
{
	EventArgs eventData;
	eventData
		.arg("event", "cultureExpansion")
		.arg("pCity", pCity)
		.arg("ePlayer", ePlayer);
	postEvent(eventData, "cultureExpansion");
}

void CvDllPythonEvents::reportCityGrowth( CvCity *pCity, PlayerTypes ePlayer )
{
	EventArgs eventData;
	eventData
		.arg("event", "cityGrowth")
		.arg("pCity", pCity)
		.arg("ePlayer", ePlayer);
	postEvent(eventData, "cityGrowth");
}

void CvDllPythonEvents::reportCityProduction( CvCity *pCity, PlayerTypes ePlayer )
{
	EventArgs eventData;
	eventData
		.no_json()
		.arg("event", "cityDoTurn")
		.arg("pCity", pCity)
		.arg("ePlayer", ePlayer);
	postEvent(eventData, "cityDoTurn");
}

void CvDllPythonEvents::reportCityBuildingUnit( CvCity *pCity, UnitTypes eUnitType )
{
	EventArgs eventData;
	eventData
		.no_json()
		.arg("event", "cityBuildingUnit")
		.arg("pCity", pCity)
		.arg("eUnitType", eUnitType);
	postEvent(eventData, "cityBuildingUnit");
}

void CvDllPythonEvents::reportCityBuildingBuilding( CvCity *pCity, BuildingTypes eBuildingType )
{
	EventArgs eventData;
	eventData
		.no_json()
		.arg("event", "cityBuildingBuilding")
		.arg("pCity", pCity)
		.arg("eBuildingType", eBuildingType);
	postEvent(eventData, "cityBuildingBuilding");
}

// BUG - Project Started Event - start
void CvDllPythonEvents::reportCityBuildingProject( CvCity* pCity, ProjectTypes eProjectType )
{
	EventArgs eventData;
	eventData
		.arg("event", "cityBuildingProject")
		.arg("pCity", pCity)
		.arg("eProjectType", eProjectType);
	postEvent(eventData, "cityBuildingProject");
}
// BUG - Project Started Event - end

// BUG - Process Started Event - start
void CvDllPythonEvents::reportCityBuildingProcess( CvCity* pCity, ProcessTypes eProcessType )
{
	EventArgs eventData;
	eventData
		.arg("event", "cityBuildingProcess")
		.arg("pCity", pCity)
		.arg("eProcessType", eProcessType);
	postEvent(eventData, "cityBuildingProcess");
}
// BUG - Process Started Event - end

void CvDllPythonEvents::reportCityRename( CvCity *pCity )
{
	EventArgs eventData;
	eventData
		.arg("event", "cityRename")
		.arg("pCity", pCity);
	postEvent(eventData, "cityRename");
}

void CvDllPythonEvents::reportCityHurry( CvCity *pCity, HurryTypes eHurry )
{
	EventArgs eventData;
	eventData
		.arg("event", "cityHurry")
		.arg("pCity", pCity)
		.arg("eHurry", eHurry);
	postEvent(eventData,"cityHurry");
}

void CvDllPythonEvents::reportSelectionGroupPushMission(const CvSelectionGroup* pSelectionGroup, MissionTypes eMission)
{
	if (NULL == pSelectionGroup)
	{
		return;
	}

	std::vector<int> aiUnitIds;

	//using namespace bst::lambda;

	algo::transform(pSelectionGroup->units(), std::back_inserter(aiUnitIds), bind(&CvUnit::getID, _1));

	EventArgs eventData;
	eventData
		.arg("event", "selectionGroupPushMission")
		.arg("pSelectionGroup", pSelectionGroup->getOwner())
		.arg("eMission", eMission)
		.arg("aiUnitIds", aiUnitIds.size())
		.arg("aiUnitIds", aiUnitIds);
	postEvent(eventData, "selectionGroupPushMission");
}

void CvDllPythonEvents::reportUnitMove(CvPlot* pPlot, CvUnit* pUnit, CvPlot* pOldPlot)
{
	EventArgs eventData;
	eventData
		.no_json()
		.arg("event", "unitMove")
		.arg("pPlot", pPlot)
		.arg("pUnit", pUnit)
		.arg("pOldPlot", pOldPlot);
	postEvent(eventData, "unitMove");
}

void CvDllPythonEvents::reportUnitCreated(CvUnit* pUnit)
{
	EventArgs eventData;
	eventData
		.arg("event", "unitCreated")
		.arg("pUnit", pUnit);
	postEvent(eventData, "unitCreated");
}

void CvDllPythonEvents::reportUnitBuilt(CvCity *pCity, CvUnit* pUnit)
{
	EventArgs eventData;
	eventData
		.arg("event", "unitBuilt")
		.arg("pCity", pCity)
		.arg("pUnit", pUnit);
	postEvent(eventData,"unitBuilt");
}

void CvDllPythonEvents::reportUnitKilled(CvUnit* pUnit, PlayerTypes eAttacker)
{
	EventArgs eventData;
	eventData
		.arg("event", "unitKilled")
		.arg("pUnit", pUnit)
		.arg("eAttacker", eAttacker);
	postEvent(eventData, "unitKilled");
}

// BUG - Unit Captured Event - start
void CvDllPythonEvents::reportUnitCaptured(PlayerTypes eFromPlayer, UnitTypes eUnitType, CvUnit* pNewUnit)
{
	EventArgs eventData;
	eventData
		.arg("event", "unitCaptured")
		.arg("eFromPlayer", eFromPlayer)
		.arg("eUnitType", eUnitType)
		.arg("pNewUnit", pNewUnit);
	postEvent(eventData, "unitCaptured");
}
// BUG - Unit Captured Event - end

void CvDllPythonEvents::reportUnitLost(CvUnit* pUnit)
{
	EventArgs eventData;
	eventData
		.arg("event", "unitLost")
		.arg("pUnit", pUnit);
	postEvent(eventData, "unitLost");
}

void CvDllPythonEvents::reportUnitPromoted(CvUnit* pUnit, PromotionTypes ePromotion)
{
	EventArgs eventData;
	eventData
		.arg("event", "unitPromoted")
		.arg("pUnit", pUnit)
		.arg("ePromotion", ePromotion);
	postEvent(eventData, "unitPromoted");
}

// BUG - Upgrade Unit Event - start
void CvDllPythonEvents::reportUnitUpgraded(CvUnit* pOldUnit, CvUnit* pNewUnit, int iPrice)
{
	EventArgs eventData;
	eventData
		.arg("event", "unitUpgraded")
		.arg("pOldUnit", pOldUnit)
		.arg("pNewUnit", pNewUnit)
		.arg("iPrice", iPrice);
	postEvent(eventData, "unitUpgraded");
}
// BUG - Upgrade Unit Event - end

void CvDllPythonEvents::reportUnitSelected(CvUnit* pUnit)
{
	EventArgs eventData;
	eventData
		.arg("event", "unitSelected")
		.arg("pUnit", pUnit);
	postEvent(eventData, "unitSelected");
}

void CvDllPythonEvents::reportUnitRename(CvUnit *pUnit)
{
	EventArgs eventData;
	eventData
		.arg("event", "UnitRename")
		.arg("pUnit", pUnit);
	postEvent(eventData, "UnitRename");
}

void CvDllPythonEvents::reportUnitPillage(CvUnit* pUnit, ImprovementTypes eImprovement, RouteTypes eRoute, PlayerTypes ePlayer)
{
	EventArgs eventData;
	eventData
		.arg("event", "unitPillage")
		.arg("pUnit", pUnit)
		.arg("eImprovement", eImprovement)
		.arg("eRoute", eRoute)
		.arg("ePlayer", ePlayer);
	postEvent(eventData, "unitPillage");

}

void CvDllPythonEvents::reportUnitSpreadReligionAttempt(CvUnit* pUnit, ReligionTypes eReligion, bool bSuccess)
{
	EventArgs eventData;
	eventData
		.arg("event", "unitSpreadReligionAttempt")
		.arg("pUnit", pUnit)
		.arg("eReligion", eReligion)
		.arg("bSuccess", bSuccess);
	postEvent(eventData, "unitSpreadReligionAttempt");
}

void CvDllPythonEvents::reportUnitGifted(CvUnit* pUnit, PlayerTypes eGiftingPlayer, CvPlot* pPlotLocation)
{
	EventArgs eventData;
	eventData
		.arg("event", "unitGifted")
		.arg("pUnit", pUnit)
		.arg("eGiftingPlayer", eGiftingPlayer)
		.arg("pPlotLocation", pPlotLocation);
	postEvent(eventData, "unitGifted");
}

void CvDllPythonEvents::reportUnitBuildImprovement(CvUnit* pUnit, BuildTypes eBuild, bool bFinished)
{
	EventArgs eventData;
	eventData
		.arg("event", "unitBuildImprovement")
		.arg("pUnit", pUnit)
		.arg("eBuild", eBuild)
		.arg("bFinished", bFinished);
	postEvent(eventData, "unitBuildImprovement");
}

void CvDllPythonEvents::reportGoodyReceived(PlayerTypes ePlayer, CvPlot *pGoodyPlot, CvUnit *pGoodyUnit, GoodyTypes eGoodyType)
{
	EventArgs eventData;
	eventData
		.arg("event", "goodyReceived")
		.arg("ePlayer", ePlayer)
		.arg("pGoodyPlot", pGoodyPlot);

	if (pGoodyUnit)
		eventData.arg("pGoodyUnit", pGoodyUnit);
	else
		eventData.arg("pGoodyUnit", NULL);

	eventData.arg("eGoodyType", eGoodyType);
	postEvent(eventData, "goodyReceived");
}

void CvDllPythonEvents::reportGreatPersonBorn( CvUnit *pUnit, PlayerTypes ePlayer, CvCity *pCity )
{
	EventArgs eventData;
	eventData
		.arg("event", "greatPersonBorn")
		.arg("pUnit", pUnit)
		.arg("ePlayer", ePlayer)
		.arg("pCity", pCity);
	postEvent(eventData, "greatPersonBorn");
}

void CvDllPythonEvents::reportCivicChanged(PlayerTypes ePlayer, CivicTypes eOldCivic, CivicTypes eNewCivic)
{
	EventArgs eventData;
	eventData
		.arg("event", "civicChanged")
		.arg("ePlayer", ePlayer)
		.arg("eOldCivic", eOldCivic)
		.arg("eNewCivic", eNewCivic);
	postEvent(eventData, "civicChanged");
}

void CvDllPythonEvents::reportBuildingBuilt(CvCity *pCity, BuildingTypes eBuilding)
{
	EventArgs eventData;
	eventData
		.arg("event", "buildingBuilt")
		.arg("pCity", pCity)
		.arg("eBuilding", eBuilding);
	postEvent(eventData, "buildingBuilt");
}

void CvDllPythonEvents::reportProjectBuilt(CvCity *pCity, ProjectTypes eProject)
{
	EventArgs eventData;
	eventData
		.arg("event", "projectBuilt")
		.arg("pCity", pCity)
		.arg("eProject", eProject);
	postEvent(eventData, "projectBuilt");
}

void CvDllPythonEvents::reportTechAcquired(TechTypes eType, TeamTypes eTeam, PlayerTypes ePlayer, bool bAnnounce)
{
	EventArgs eventData;
	eventData
		.arg("event", "techAcquired")
		.arg("eType", eType)
		.arg("eTeam", eTeam)
		.arg("ePlayer", ePlayer)
		.arg("bAnnounce", bAnnounce);
	postEvent(eventData, "techAcquired");
}

void CvDllPythonEvents::reportTechSelected(TechTypes eTech, PlayerTypes ePlayer)
{
	EventArgs eventData;
	eventData
		.arg("event", "techSelected")
		.arg("eTech", eTech)
		.arg("ePlayer", ePlayer);
	postEvent(eventData, "techSelected");
}

void CvDllPythonEvents::reportReligionFounded(ReligionTypes eType, PlayerTypes ePlayer)
{
	EventArgs eventData;
	eventData
		.arg("event", "religionFounded")
		.arg("eType", eType)
		.arg("ePlayer", ePlayer);
	postEvent(eventData, "religionFounded");
}

void CvDllPythonEvents::reportReligionSpread(ReligionTypes eType, PlayerTypes ePlayer, CvCity* pSpreadCity)
{
	EventArgs eventData;
	eventData
		.arg("event", "religionSpread")
		.arg("eType", eType)
		.arg("ePlayer", ePlayer)
		.arg("pSpreadCity", pSpreadCity);
	postEvent(eventData, "religionSpread");
}

void CvDllPythonEvents::reportReligionRemove(ReligionTypes eType, PlayerTypes ePlayer, CvCity* pSpreadCity)
{
	EventArgs eventData;
	eventData
		.arg("event", "religionRemove")
		.arg("eType", eType)
		.arg("ePlayer", ePlayer)
		.arg("pSpreadCity", pSpreadCity);
	postEvent(eventData, "religionRemove");
}

void CvDllPythonEvents::reportCorporationFounded(CorporationTypes eType, PlayerTypes ePlayer)
{
	EventArgs eventData;
	eventData
		.arg("event", "corporationFounded")
		.arg("eType", eType)
		.arg("ePlayer", ePlayer);
	postEvent(eventData, "corporationFounded");
}

void CvDllPythonEvents::reportCorporationSpread(CorporationTypes eType, PlayerTypes ePlayer, CvCity* pSpreadCity)
{
	EventArgs eventData;
	eventData
		.arg("event", "corporationSpread")
		.arg("eType", eType)
		.arg("ePlayer", ePlayer)
		.arg("pSpreadCity", pSpreadCity);
	postEvent(eventData, "corporationSpread");
}

void CvDllPythonEvents::reportCorporationRemove(CorporationTypes eType, PlayerTypes ePlayer, CvCity* pSpreadCity)
{
	EventArgs eventData;
	eventData
		.arg("event", "corporationRemove")
		.arg("eType", eType)
		.arg("ePlayer", ePlayer)
		.arg("pSpreadCity", pSpreadCity);
	postEvent(eventData, "corporationRemove");
}

void CvDllPythonEvents::reportGoldenAge(PlayerTypes ePlayer)
{
	EventArgs eventData;
	eventData
		.arg("event", "goldenAge")
		.arg("ePlayer", ePlayer);
	postEvent(eventData, "goldenAge");
}

void CvDllPythonEvents::reportEndGoldenAge(PlayerTypes ePlayer)
{
	EventArgs eventData;
	eventData
		.arg("event", "endGoldenAge")
		.arg("ePlayer", ePlayer);
	postEvent(eventData, "endGoldenAge");
}

void CvDllPythonEvents::reportChangeWar(bool bWar, TeamTypes eTeam, TeamTypes eOtherTeam)
{
	EventArgs eventData;
	eventData
		.arg("event", "changeWar")
		.arg("bWar", bWar)
		.arg("eTeam", eTeam)
		.arg("eOtherTeam", eOtherTeam);
	postEvent(eventData, "changeWar");
}

void CvDllPythonEvents::reportVictory(TeamTypes eNewWinner, VictoryTypes eNewVictory)
{
	EventArgs eventData;
	eventData
		.arg("event", "victory")
		.arg("eNewWinner", eNewWinner)
		.arg("eNewVictory", eNewVictory);
	postEvent(eventData, "victory");
}

void CvDllPythonEvents::reportVassalState(TeamTypes eMaster, TeamTypes eVassal, bool bVassal)
{
	EventArgs eventData;
	eventData
		.arg("event", "vassalState")
		.arg("eMaster", eMaster)
		.arg("eVassal", eVassal)
		.arg("bVassal", bVassal);
	postEvent(eventData, "vassalState");
}

void CvDllPythonEvents::reportSetPlayerAlive( PlayerTypes ePlayerID, bool bNewValue )
{
	EventArgs eventData;
	eventData
		.arg("event", "setPlayerAlive")
		.arg("ePlayerID", ePlayerID)
		.arg("bNewValue", bNewValue);
	postEvent(eventData, "setPlayerAlive");
}

void CvDllPythonEvents::reportPlayerChangeStateReligion(PlayerTypes ePlayerID, ReligionTypes eNewReligion, ReligionTypes eOldReligion)
{
	EventArgs eventData;
	eventData
		.arg("event", "playerChangeStateReligion")
		.arg("ePlayerID", ePlayerID)
		.arg("eNewReligion", eNewReligion)
		.arg("eOldReligion", eOldReligion);
	postEvent(eventData, "playerChangeStateReligion");
}

void CvDllPythonEvents::reportPlayerGoldTrade(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, int iAmount)
{
	EventArgs eventData;
	eventData
		.arg("event", "playerGoldTrade")
		.arg("eFromPlayer", eFromPlayer)
		.arg("eToPlayer", eToPlayer)
		.arg("iAmount", iAmount);
	postEvent(eventData, "playerGoldTrade");
}

void CvDllPythonEvents::reportGenericEvent(const char* szEventName, void *pyArgs)
{
	EventArgs eventData;
	eventData
		.no_json()
		.arg("szEventName", szEventName)
		.argPy(pyArgs); // generic args tuple
	postEvent(eventData, szEventName);
}

void CvDllPythonEvents::preSave()
{
	EventArgs eventData;
	eventData
		.arg("event", "OnPreSave");
	postEvent(eventData, "OnPreSave");
}

void CvDllPythonEvents::reportChangeTeam(TeamTypes eOld, TeamTypes eNew)
{
	EventArgs eventData;
	eventData
		.arg("event", "changeTeam")
		.arg("eOld", eOld)
		.arg("eNew", eNew);
	postEvent(eventData, "changeTeam");
}
