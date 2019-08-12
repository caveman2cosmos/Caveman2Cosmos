#include "CvGameCoreDLL.h"

bool CvDllPythonEvents::preEvent()
{
	return gDLL->getPythonIFace()->isInitialized();
}

bool CvDllPythonEvents::postEvent(CyArgsList& eventData, const char* eventName)
{
	eventData.add(GC.getGameINLINE().isDebugMode());
	eventData.add(false);
	eventData.add(gDLL->altKey());
	eventData.add(gDLL->ctrlKey());
	eventData.add(gDLL->shiftKey());
	eventData.add(gDLL->getChtLvl() > 0);

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
		xSum.add((byte)*ptr);
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

	long lResult = -1;
	bool bOK = PYTHON_CALL_FUNCTION4(__FUNCTION__, PYEventModule, "onEvent", eventData.makeFunctionArgs(), &lResult);

	return (bOK && lResult==1);
}

bool CvDllPythonEvents::reportKbdEvent(int evt, int key, int iCursorX, int iCursorY)
{
	if (preEvent())
	{
		NiPoint3 pt3Location;
		CvPlot* pPlot = gDLL->getEngineIFace()->pickPlot(iCursorX, iCursorY, pt3Location);
		CyArgsList eventData;
		eventData.add("kbdEvent");
		eventData.add(evt);
		eventData.add(key);
		eventData.add(iCursorX);
		eventData.add(iCursorY);
		eventData.add(pPlot ? pPlot->getX() : -1);
		eventData.add(pPlot ? pPlot->getY() : -1);

		return postEvent(eventData, "kbdEvent");
	}
	return false;
}

bool CvDllPythonEvents::reportMouseEvent(int evt, int iCursorX, int iCursorY, bool bInterfaceConsumed)
{
	if (preEvent())
	{
		NiPoint3 pt3Location;
		CvPlot* pPlot = gDLL->getEngineIFace()->pickPlot(iCursorX, iCursorY, pt3Location);
		CyArgsList eventData;
		eventData.add("mouseEvent");				// add key to lookup python handler fxn
		eventData.add(evt);
		eventData.add(iCursorX);
		eventData.add(iCursorY);
		eventData.add(pPlot ? pPlot->getX() : -1);
		eventData.add(pPlot ? pPlot->getY() : -1);
		eventData.add(bInterfaceConsumed);

		// add list of active screens
		std::vector<int> screens;
		gDLL->getInterfaceIFace()->getInterfaceScreenIdsForInput(screens);
		eventData.add(screens.size() ? &screens[0] : NULL, screens.size());

		return postEvent(eventData, "mouseEvent");
	}
	return false;
}

void CvDllPythonEvents::reportModNetMessage(int iData1, int iData2, int iData3, int iData4, int iData5)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("ModNetMessage");				// add key to lookup python handler fxn
		eventData.add(iData1);
		eventData.add(iData2);
		eventData.add(iData3);
		eventData.add(iData4);
		eventData.add(iData5);
		postEvent(eventData,"ModNetMessage");
	}
}

void CvDllPythonEvents::reportInit()
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("Init");				// add key to lookup python handler fxn
		postEvent(eventData, "Init");
	}
}

void CvDllPythonEvents::reportUpdate(float fDeltaTime)
{
	if (preEvent())
	{
		if(GC.getUSE_ON_UPDATE_CALLBACK())
		{
			CyArgsList eventData;
			eventData.add("Update");				// add key to lookup python handler fxn
			eventData.add(fDeltaTime);
			postEvent(eventData, "Update");
		}
	}
}

void CvDllPythonEvents::reportUnInit()
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("UnInit");				// add key to lookup python handler fxn
		postEvent(eventData, "UnInit");
	}
}

void CvDllPythonEvents::reportGameStart()
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("GameStart");				// add key to lookup python handler fxn
		postEvent(eventData, "GameStart");
	}
}

void CvDllPythonEvents::reportGameEnd()
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("GameEnd");				// add key to lookup python handler fxn
		postEvent(eventData, "GameEnd");
	}
}


void CvDllPythonEvents::reportWindowActivation(bool bActive)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("windowActivation");
		eventData.add(bActive);
		postEvent(eventData, "windowActivation");
	}
}

void CvDllPythonEvents::reportBeginGameTurn(int iGameTurn)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("BeginGameTurn");				// add key to lookup python handler fxn
		eventData.add(iGameTurn);
		postEvent(eventData, "BeginGameTurn");
	}
}

void CvDllPythonEvents::reportEndGameTurn(int iGameTurn)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("EndGameTurn");				// add key to lookup python handler fxn
		eventData.add(iGameTurn);
		postEvent(eventData, "EndGameTurn");
	}
}
/************************************************************************************************/
/* RevolutionDCM MpFix                                                           lemmy101       */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
void CvDllPythonEvents::reportPreEndGameTurn(int iGameTurn)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("PreEndGameTurn");				// add key to lookup python handler fxn
		eventData.add(iGameTurn);
		postEvent(eventData, "PreEndGameTurn");
	}
}
/************************************************************************************************/
/* RevolutionDCM                           END                                                  */
/************************************************************************************************/

void CvDllPythonEvents::reportBeginPlayerTurn(int iGameTurn, PlayerTypes ePlayer)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("BeginPlayerTurn");				// add key to lookup python handler fxn
		eventData.add(iGameTurn);
		eventData.add((int)ePlayer);
		postEvent(eventData, "BeginPlayerTurn");
	}
}

void CvDllPythonEvents::reportEndPlayerTurn(int iGameTurn, PlayerTypes ePlayer)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("EndPlayerTurn");				// add key to lookup python handler fxn
		eventData.add(iGameTurn);
		eventData.add((int)ePlayer);
		postEvent(eventData, "EndPlayerTurn");
	}
}

void CvDllPythonEvents::reportFirstContact(TeamTypes eTeamID1, TeamTypes eTeamID2)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("firstContact");				// add key to lookup python handler fxn
		eventData.add((int)eTeamID1);
		eventData.add((int)eTeamID2);
		postEvent(eventData, "firstContact");
	}
}

void CvDllPythonEvents::reportCombatResult(CvUnit* pWinner, CvUnit* pLoser)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("combatResult");				// add key to lookup python handler fxn

		CyUnit* pCyWinner = new CyUnit(pWinner);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyWinner));

		CyUnit* pCyLoser = new CyUnit(pLoser);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyLoser));

		postEvent(eventData, "combatResult");
		delete pCyLoser;
		delete pCyWinner;
	}
}

// BUG - Combat Events - start
void CvDllPythonEvents::reportCombatRetreat(CvUnit* pAttacker, CvUnit* pDefender)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("combatRetreat");				// add key to lookup python handler fxn

		CyUnit* pCyAttacker = new CyUnit(pAttacker);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyAttacker));

		CyUnit* pCyDefender = new CyUnit(pDefender);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyDefender));

		postEvent(eventData,"combatRetreat");
		delete pCyDefender;
		delete pCyAttacker;
	}
}

void CvDllPythonEvents::reportCombatWithdrawal(CvUnit* pAttacker, CvUnit* pDefender)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("combatWithdrawal");			// add key to lookup python handler fxn

		CyUnit* pCyAttacker = new CyUnit(pAttacker);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyAttacker));

		CyUnit* pCyDefender = new CyUnit(pDefender);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyDefender));

		postEvent(eventData, "combatWithdrawal");
		delete pCyDefender;
		delete pCyAttacker;
	}
}

void CvDllPythonEvents::reportCombatLogCollateral(CvUnit* pAttacker, CvUnit* pDefender, int iDamage)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("combatLogCollateral");		// add key to lookup python handler fxn

		CyUnit* pCyAttacker = new CyUnit(pAttacker);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyAttacker));

		CyUnit* pCyDefender = new CyUnit(pDefender);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyDefender));

		eventData.add(iDamage);

		postEvent(eventData, "combatLogCollateral");
		delete pCyDefender;
		delete pCyAttacker;
	}
}

void CvDllPythonEvents::reportCombatLogFlanking(CvUnit* pAttacker, CvUnit* pDefender, int iDamage)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("combatLogFlanking");			// add key to lookup python handler fxn

		CyUnit* pCyAttacker = new CyUnit(pAttacker);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyAttacker));

		CyUnit* pCyDefender = new CyUnit(pDefender);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyDefender));

		eventData.add(iDamage);

		postEvent(eventData, "combatLogFlanking");
		delete pCyDefender;
		delete pCyAttacker;
	}
}
// BUG - Combat Events - end

void CvDllPythonEvents::reportImprovementBuilt(int iImprovementType, int iX, int iY)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("improvementBuilt");				// add key to lookup python handler fxn
		eventData.add(iImprovementType);
		eventData.add(iX);
		eventData.add(iY);
		postEvent(eventData, "improvementBuilt");
	}
}

void CvDllPythonEvents::reportImprovementDestroyed(int iImprovementType, int iPlayer, int iX, int iY)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("improvementDestroyed");				// add key to lookup python handler fxn
		eventData.add(iImprovementType);
		eventData.add(iPlayer);
		eventData.add(iX);
		eventData.add(iY);
		postEvent(eventData, "improvementDestroyed");
	}
}

void CvDllPythonEvents::reportRouteBuilt(int iRouteType, int iX, int iY)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("routeBuilt");				// add key to lookup python handler fxn
		eventData.add(iRouteType);
		eventData.add(iX);
		eventData.add(iY);
		postEvent(eventData, "routeBuilt");
	}
}

void CvDllPythonEvents::reportChat(CvWString szString)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("chat");							// add key to lookup python handler fxn
		eventData.add(szString);
		postEvent(eventData, "chat");
	}
}

void CvDllPythonEvents::reportPlotRevealed(CvPlot *pPlot, TeamTypes eTeam)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("plotRevealed");						// add key to lookup python handler fxn

		CyPlot* pCyPlot = new CyPlot(pPlot);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyPlot));
		eventData.add((int)eTeam);

		postEvent(eventData, "plotRevealed");
		delete pCyPlot;
	}
}

void CvDllPythonEvents::reportPlotFeatureRemoved(CvPlot *pPlot, FeatureTypes eFeature, CvCity* pCity)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("plotFeatureRemoved");						// add key to lookup python handler fxn

		CyPlot* pCyPlot = new CyPlot(pPlot);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyPlot));
		eventData.add((int)eFeature);
		CyCity* pCyCity= new CyCity(pCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyCity));

		postEvent(eventData, "plotFeatureRemoved");
		delete pCyPlot;
		delete pCyCity;
	}
}

void CvDllPythonEvents::reportPlotPicked(CvPlot *pPlot)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("plotPicked");						// add key to lookup python handler fxn

		CyPlot* pCyPlot = new CyPlot(pPlot);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyPlot));

		postEvent(eventData, "plotPicked");
		delete pCyPlot;
	}
}

void CvDllPythonEvents::reportNukeExplosion(CvPlot *pPlot, CvUnit* pNukeUnit)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("nukeExplosion");						// add key to lookup python handler fxn

		CyPlot* pCyPlot = new CyPlot(pPlot);
		CyUnit* pCyUnit = new CyUnit(pNukeUnit);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyPlot));
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyUnit));

		postEvent(eventData, "nukeExplosion");
		delete pCyPlot;
		delete pCyUnit;
	}
}

void CvDllPythonEvents::reportGotoPlotSet(CvPlot *pPlot, PlayerTypes ePlayer)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("gotoPlotSet");						// add key to lookup python handler fxn

		CyPlot* pCyPlot = new CyPlot(pPlot);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyPlot));
		eventData.add((int) ePlayer);

		postEvent(eventData, "gotoPlotSet");
		delete pCyPlot;
	}
}

void CvDllPythonEvents::reportCityBuilt( CvCity *pCity, CvUnit *pUnit )
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("cityBuilt");						// add key to lookup python handler fxn

		CyCity* pCyCity = new CyCity(pCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyCity));
		CyUnit* pCyUnit = new CyUnit(pUnit);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyUnit));

		postEvent(eventData, "cityBuilt");
		delete pCyCity;
		delete pCyUnit;
	}
}

void CvDllPythonEvents::reportCityRazed( CvCity *pCity, PlayerTypes ePlayer )
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("cityRazed");						// add key to lookup python handler fxn

		CyCity* pCyCity = new CyCity(pCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyCity));
		eventData.add((int)ePlayer);

		postEvent(eventData, "cityRazed");
		delete pCyCity;
	}
}

void CvDllPythonEvents::reportCityAcquired(PlayerTypes eOldOwner, PlayerTypes ePlayer, CvCity* pOldCity, bool bConquest, bool bTrade)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("cityAcquired");					// add key to lookup python handler fxn
		eventData.add((int)eOldOwner);
		eventData.add((int)ePlayer);

		CyCity* pCyCity = new CyCity(pOldCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyCity));

		eventData.add(bConquest);
		eventData.add(bTrade);
		postEvent(eventData, "cityAcquired");
		delete pCyCity;
	}
}

void CvDllPythonEvents::reportCityAcquiredAndKept(PlayerTypes ePlayer, CvCity* pOldCity)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("cityAcquiredAndKept");					// add key to lookup python handler fxn
		eventData.add((int)ePlayer);

		CyCity* pCyCity = new CyCity(pOldCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyCity));

		postEvent(eventData, "cityAcquiredAndKept");
		delete pCyCity;
	}
}

void CvDllPythonEvents::reportCityLost(CvCity* pCity)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("cityLost");						// add key to lookup python handler fxn

		CyCity* pyu = new CyCity(pCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pyu));
		postEvent(eventData, "cityLost");
		delete pyu;
	}
}

void CvDllPythonEvents::reportCultureExpansion( CvCity *pCity, PlayerTypes ePlayer )
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("cultureExpansion");						// add key to lookup python handler fxn

		CyCity* pCyCity = new CyCity(pCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyCity));

		eventData.add((int) ePlayer);

		postEvent(eventData, "cultureExpansion");
		delete pCyCity;
	}
}

void CvDllPythonEvents::reportCityGrowth( CvCity *pCity, PlayerTypes ePlayer )
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("cityGrowth");						// add key to lookup python handler fxn

		CyCity* pCyCity = new CyCity(pCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyCity));

		eventData.add((int) ePlayer);

		postEvent(eventData, "cityGrowth");
		delete pCyCity;
	}
}

void CvDllPythonEvents::reportCityProduction( CvCity *pCity, PlayerTypes ePlayer )
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("cityDoTurn");						// add key to lookup python handler fxn

		CyCity* pCyCity = new CyCity(pCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyCity));

		eventData.add((int) ePlayer);

		postEvent(eventData, "cityDoTurn");
		delete pCyCity;
	}
}

void CvDllPythonEvents::reportCityBuildingUnit( CvCity *pCity, UnitTypes eUnitType )
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("cityBuildingUnit");						// add key to lookup python handler fxn

		CyCity* pCyCity = new CyCity(pCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyCity));

		eventData.add((int) eUnitType);

		postEvent(eventData, "cityBuildingUnit");
		delete pCyCity;
	}
}

void CvDllPythonEvents::reportCityBuildingBuilding( CvCity *pCity, BuildingTypes eBuildingType )
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("cityBuildingBuilding");						// add key to lookup python handler fxn

		CyCity* pCyCity = new CyCity(pCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyCity));

		eventData.add((int) eBuildingType);

		postEvent(eventData, "cityBuildingBuilding");
		delete pCyCity;
	}
}

// BUG - Project Started Event - start
void CvDllPythonEvents::reportCityBuildingProject( CvCity* pCity, ProjectTypes eProjectType )
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("cityBuildingProject");						// add key to lookup python handler fxn

		CyCity* pCyCity = new CyCity(pCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyCity));

		eventData.add((int) eProjectType);

		postEvent(eventData, "cityBuildingProject");
		delete pCyCity;
	}
}
// BUG - Project Started Event - end

// BUG - Process Started Event - start
void CvDllPythonEvents::reportCityBuildingProcess( CvCity* pCity, ProcessTypes eProcessType )
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("cityBuildingProcess");						// add key to lookup python handler fxn

		CyCity* pCyCity = new CyCity(pCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyCity));

		eventData.add((int) eProcessType);

		postEvent(eventData, "cityBuildingProcess");
		delete pCyCity;
	}
}
// BUG - Process Started Event - end

void CvDllPythonEvents::reportCityRename( CvCity *pCity )
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("cityRename");						// add key to lookup python handler fxn

		CyCity* pCyCity = new CyCity(pCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyCity));

		postEvent(eventData, "cityRename");
		delete pCyCity;
	}
}

void CvDllPythonEvents::reportCityHurry( CvCity *pCity, HurryTypes eHurry )
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("cityHurry");						// add key to lookup python handler fxn

		CyCity* pCyCity = new CyCity(pCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyCity));

		eventData.add((int) eHurry);

		postEvent(eventData,"cityHurry");
		delete pCyCity;
	}
}

void CvDllPythonEvents::reportSelectionGroupPushMission(CvSelectionGroup* pSelectionGroup, MissionTypes eMission)
{
	if (NULL == pSelectionGroup)
	{
		return;
	}

	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("selectionGroupPushMission");						// add key to lookup python handler fxn
		eventData.add(pSelectionGroup->getOwner());
		eventData.add(eMission);
		int iNumUnits = pSelectionGroup->getNumUnits();
		eventData.add(iNumUnits);

		int* aiUnitIds = new int[iNumUnits];
		CLLNode<IDInfo>* pUnitNode = pSelectionGroup->headUnitNode();
		for (int i = 0; pUnitNode; i++)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = pSelectionGroup->nextUnitNode(pUnitNode);
			aiUnitIds[i] = pLoopUnit->getID();
			FAssert(i < iNumUnits);
		}

		if (aiUnitIds)
		{
			eventData.add(aiUnitIds, iNumUnits);
		}

		postEvent(eventData, "selectionGroupPushMission");

		delete[] aiUnitIds;
	}
}

void CvDllPythonEvents::reportUnitMove(CvPlot* pPlot, CvUnit* pUnit, CvPlot* pOldPlot)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("unitMove");						// add key to lookup python handler fxn

		CyPlot* py = new CyPlot(pPlot);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(py));

		CyUnit* pyu = new CyUnit(pUnit);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pyu));

		CyPlot* pyOld = new CyPlot(pOldPlot);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pyOld));

		postEvent(eventData, "unitMove");

		delete py;
		delete pyu;
		delete pyOld;
	}
}

void CvDllPythonEvents::reportUnitSetXY(CvPlot* pPlot, CvUnit* pUnit)
{
	if (preEvent())
	{
		if(GC.getUSE_ON_UNIT_SET_XY_CALLBACK())
		{
			CyArgsList eventData;
			eventData.add("unitSetXY");						// add key to lookup python handler fxn

			CyPlot* py = new CyPlot(pPlot);
			eventData.add(gDLL->getPythonIFace()->makePythonObject(py));

			CyUnit* pyu = new CyUnit(pUnit);
			eventData.add(gDLL->getPythonIFace()->makePythonObject(pyu));

			postEvent(eventData, "unitSetXY");

			delete py;
			delete pyu;
		}
	}
}

void CvDllPythonEvents::reportUnitCreated(CvUnit* pUnit)
{
	if (preEvent())
	{
		if(GC.getUSE_ON_UNIT_CREATED_CALLBACK())
		{
			CyArgsList eventData;
			eventData.add("unitCreated");						// add key to lookup python handler fxn

			CyUnit* pyu = new CyUnit(pUnit);
			eventData.add(gDLL->getPythonIFace()->makePythonObject(pyu));
			postEvent(eventData, "unitCreated");
			delete pyu;
		}
	}
}

void CvDllPythonEvents::reportUnitBuilt(CvCity *pCity, CvUnit* pUnit)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("unitBuilt");						// add key to lookup python handler fxn

		CyCity* pyc = new CyCity(pCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pyc));

		CyUnit* pyu = new CyUnit(pUnit);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pyu));

		postEvent(eventData,"unitBuilt");

		delete pyc;
		delete pyu;
	}
}

void CvDllPythonEvents::reportUnitKilled(CvUnit* pUnit, PlayerTypes eAttacker)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("unitKilled");						// add key to lookup python handler fxn

		CyUnit* pyu = new CyUnit(pUnit);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pyu));
		eventData.add((int)eAttacker);
		postEvent(eventData, "unitKilled");
		delete pyu;
	}
}

// BUG - Unit Captured Event - start
void CvDllPythonEvents::reportUnitCaptured(PlayerTypes eFromPlayer, UnitTypes eUnitType, CvUnit* pNewUnit)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("unitCaptured");						// add key to lookup python handler fxn

		eventData.add(eFromPlayer);
		eventData.add(eUnitType);
		CyUnit* pyNewUnit = new CyUnit(pNewUnit);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pyNewUnit));
		postEvent(eventData, "unitCaptured");
		delete pyNewUnit;
	}
}
// BUG - Unit Captured Event - end

void CvDllPythonEvents::reportUnitLost(CvUnit* pUnit)
{
	if (preEvent())
	{
		if(GC.getUSE_ON_UNIT_LOST_CALLBACK())
		{
			CyArgsList eventData;
			eventData.add("unitLost");						// add key to lookup python handler fxn

			CyUnit* pyu = new CyUnit(pUnit);
			eventData.add(gDLL->getPythonIFace()->makePythonObject(pyu));
			postEvent(eventData, "unitLost");
			delete pyu;
		}
	}
}

void CvDllPythonEvents::reportUnitPromoted(CvUnit* pUnit, PromotionTypes ePromotion)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("unitPromoted");						// add key to lookup python handler fxn

		CyUnit* pyu = new CyUnit(pUnit);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pyu));
		eventData.add((int)ePromotion);
		postEvent(eventData, "unitPromoted");
		delete pyu;
	}
}

// BUG - Upgrade Unit Event - start
void CvDllPythonEvents::reportUnitUpgraded(CvUnit* pOldUnit, CvUnit* pNewUnit, int iPrice)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("unitUpgraded");						// add key to lookup python handler fxn

		CyUnit* pyOldUnit = new CyUnit(pOldUnit);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pyOldUnit));
		CyUnit* pyNewUnit = new CyUnit(pNewUnit);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pyNewUnit));
		eventData.add(iPrice);
		postEvent(eventData, "unitUpgraded");
		delete pyNewUnit;
		delete pyOldUnit;
	}
}
// BUG - Upgrade Unit Event - end

void CvDllPythonEvents::reportUnitSelected(CvUnit* pUnit)
{
	if (preEvent())
	{
		if(GC.getUSE_ON_UNIT_SELECTED_CALLBACK())
		{
			CyArgsList eventData;
			eventData.add("unitSelected");						// add key to lookup python handler fxn

			CyUnit* pyu = new CyUnit(pUnit);
			eventData.add(gDLL->getPythonIFace()->makePythonObject(pyu));
			postEvent(eventData, "unitSelected");
			delete pyu;
		}
	}
}

void CvDllPythonEvents::reportUnitRename(CvUnit *pUnit)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("UnitRename");						// add key to lookup python handler fxn

		CyUnit* pCyUnit = new CyUnit(pUnit);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyUnit));

		postEvent(eventData, "UnitRename");
		delete pCyUnit;
	}
}

void CvDllPythonEvents::reportUnitPillage(CvUnit* pUnit, ImprovementTypes eImprovement, RouteTypes eRoute, PlayerTypes ePlayer)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("unitPillage");						// add key to lookup python handler fxn

		CyUnit* pCyUnit = new CyUnit(pUnit);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyUnit));
		eventData.add((int) eImprovement);
		eventData.add((int) eRoute);
		eventData.add((int) ePlayer);

		postEvent(eventData, "unitPillage");
		delete pCyUnit;
	}
}

void CvDllPythonEvents::reportUnitSpreadReligionAttempt(CvUnit* pUnit, ReligionTypes eReligion, bool bSuccess)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("unitSpreadReligionAttempt");						// add key to lookup python handler fxn

		CyUnit* pCyUnit = new CyUnit(pUnit);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyUnit));
		eventData.add((int) eReligion);
		eventData.add(bSuccess);

		postEvent(eventData, "unitSpreadReligionAttempt");
		delete pCyUnit;
	}
}

void CvDllPythonEvents::reportUnitGifted(CvUnit* pUnit, PlayerTypes eGiftingPlayer, CvPlot* pPlotLocation)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("unitGifted");						// add key to lookup python handler fxn

		CyUnit* pCyUnit = new CyUnit(pUnit);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyUnit));
		eventData.add((int) eGiftingPlayer);
		CyPlot* pCyPlot = new CyPlot(pPlotLocation);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyPlot));

		postEvent(eventData, "unitGifted");
		delete pCyUnit;
		delete pCyPlot;
	}
}

void CvDllPythonEvents::reportUnitBuildImprovement(CvUnit* pUnit, BuildTypes eBuild, bool bFinished)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("unitBuildImprovement");						// add key to lookup python handler fxn

		CyUnit* pCyUnit = new CyUnit(pUnit);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyUnit));
		eventData.add((int) eBuild);
		eventData.add(bFinished);

		postEvent(eventData, "unitBuildImprovement");
		delete pCyUnit;
	}
}

void CvDllPythonEvents::reportGoodyReceived(PlayerTypes ePlayer, CvPlot *pGoodyPlot, CvUnit *pGoodyUnit, GoodyTypes eGoodyType)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("goodyReceived");						// add key to lookup python handler fxn

		eventData.add((int)ePlayer);

		CyPlot* py = new CyPlot(pGoodyPlot);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(py));

		CyUnit* pyu = new CyUnit(pGoodyUnit);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pyu));

		eventData.add((int) eGoodyType);

		postEvent(eventData, "goodyReceived");

		delete py;
		delete pyu;
	}
}

void CvDllPythonEvents::reportGreatPersonBorn( CvUnit *pUnit, PlayerTypes ePlayer, CvCity *pCity )
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("greatPersonBorn");						// add key to lookup python handler fxn

		CyUnit* py = new CyUnit(pUnit);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(py));

		eventData.add((int)ePlayer);

		CyCity* pyu = new CyCity(pCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pyu));

		postEvent(eventData, "greatPersonBorn");

		delete py;
		delete pyu;
	}
}
void CvDllPythonEvents::reportCivicChanged(PlayerTypes ePlayer, CivicTypes eOldCivic, CivicTypes eNewCivic)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("civicChanged");					// add key to lookup python handler fxn

		eventData.add(ePlayer);
		eventData.add(eOldCivic);
		eventData.add(eNewCivic);

		postEvent(eventData, "civicChanged");
	}
}

void CvDllPythonEvents::reportBuildingBuilt(CvCity *pCity, BuildingTypes eBuilding)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("buildingBuilt");					// add key to lookup python handler fxn

		CyCity* pCyCity = new CyCity(pCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyCity));

		eventData.add(eBuilding);

		postEvent(eventData, "buildingBuilt");

		delete pCyCity;
	}
}

void CvDllPythonEvents::reportProjectBuilt(CvCity *pCity, ProjectTypes eProject)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("projectBuilt");					// add key to lookup python handler fxn

		CyCity* pCyCity = new CyCity(pCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pCyCity));

		eventData.add(eProject);

		postEvent(eventData, "projectBuilt");

		delete pCyCity;
	}
}

void CvDllPythonEvents::reportTechAcquired(TechTypes eType, TeamTypes eTeam, PlayerTypes ePlayer, bool bAnnounce)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("techAcquired");					// add key to lookup python handler fxn
		eventData.add((int)eType);
		eventData.add((int)eTeam);
		eventData.add((int)ePlayer);
		eventData.add(bAnnounce);
		postEvent(eventData, "techAcquired");
	}
}

void CvDllPythonEvents::reportTechSelected(TechTypes eTech, PlayerTypes ePlayer)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("techSelected");					// add key to lookup python handler fxn
		eventData.add((int)eTech);
		eventData.add((int)ePlayer);
		postEvent(eventData, "techSelected");
	}
}

void CvDllPythonEvents::reportReligionFounded(ReligionTypes eType, PlayerTypes ePlayer)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("religionFounded");			// add key to lookup python handler fxn
		eventData.add((int)eType);
		eventData.add((int)ePlayer);
		postEvent(eventData, "religionFounded");
	}
}

void CvDllPythonEvents::reportReligionSpread(ReligionTypes eType, PlayerTypes ePlayer, CvCity* pSpreadCity)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("religionSpread");			// add key to lookup python handler fxn
		eventData.add((int)eType);
		eventData.add((int)ePlayer);

		CyCity* pyu = new CyCity(pSpreadCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pyu));

		postEvent(eventData, "religionSpread");
		delete pyu;
	}
}

void CvDllPythonEvents::reportReligionRemove(ReligionTypes eType, PlayerTypes ePlayer, CvCity* pSpreadCity)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("religionRemove");			// add key to lookup python handler fxn
		eventData.add((int)eType);
		eventData.add((int)ePlayer);

		CyCity* pyu = new CyCity(pSpreadCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pyu));

		postEvent(eventData, "religionRemove");
		delete pyu;
	}
}

void CvDllPythonEvents::reportCorporationFounded(CorporationTypes eType, PlayerTypes ePlayer)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("corporationFounded");			// add key to lookup python handler fxn
		eventData.add((int)eType);
		eventData.add((int)ePlayer);
		postEvent(eventData, "corporationFounded");
	}
}

void CvDllPythonEvents::reportCorporationSpread(CorporationTypes eType, PlayerTypes ePlayer, CvCity* pSpreadCity)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("corporationSpread");			// add key to lookup python handler fxn
		eventData.add((int)eType);
		eventData.add((int)ePlayer);

		CyCity* pyu = new CyCity(pSpreadCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pyu));

		postEvent(eventData, "corporationSpread");
		delete pyu;
	}
}

void CvDllPythonEvents::reportCorporationRemove(CorporationTypes eType, PlayerTypes ePlayer, CvCity* pSpreadCity)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("corporationRemove");			// add key to lookup python handler fxn
		eventData.add((int)eType);
		eventData.add((int)ePlayer);

		CyCity* pyu = new CyCity(pSpreadCity);
		eventData.add(gDLL->getPythonIFace()->makePythonObject(pyu));

		postEvent(eventData, "corporationRemove");
		delete pyu;
	}
}

void CvDllPythonEvents::reportGoldenAge(PlayerTypes ePlayer)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("goldenAge");			// add key to lookup python handler fxn
		eventData.add((int)ePlayer);
		postEvent(eventData, "goldenAge");
	}
}

void CvDllPythonEvents::reportEndGoldenAge(PlayerTypes ePlayer)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("endGoldenAge");			// add key to lookup python handler fxn
		eventData.add((int)ePlayer);
		postEvent(eventData, "endGoldenAge");
	}
}

void CvDllPythonEvents::reportChangeWar(bool bWar, TeamTypes eTeam, TeamTypes eOtherTeam)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("changeWar");			// add key to lookup python handler fxn
		eventData.add(bWar);
		eventData.add((int)eTeam);
		eventData.add((int)eOtherTeam);
		postEvent(eventData, "changeWar");
	}
}

void CvDllPythonEvents::reportVictory(TeamTypes eNewWinner, VictoryTypes eNewVictory)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("victory");					// add key to lookup python handler fxn
		eventData.add((int)eNewWinner);
		eventData.add((int)eNewVictory);
		postEvent(eventData, "victory");
	}
}

void CvDllPythonEvents::reportVassalState(TeamTypes eMaster, TeamTypes eVassal, bool bVassal)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("vassalState");					// add key to lookup python handler fxn
		eventData.add((int)eMaster);
		eventData.add((int)eVassal);
		eventData.add(bVassal);

		postEvent(eventData, "vassalState");
	}
}

void CvDllPythonEvents::reportSetPlayerAlive( PlayerTypes ePlayerID, bool bNewValue )
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("setPlayerAlive");
		eventData.add((int)ePlayerID);
		eventData.add(bNewValue);
		postEvent(eventData, "setPlayerAlive");
	}
}

void CvDllPythonEvents::reportPlayerChangeStateReligion(PlayerTypes ePlayerID, ReligionTypes eNewReligion, ReligionTypes eOldReligion)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("playerChangeStateReligion");			// add key to lookup python handler fxn

		eventData.add((int)ePlayerID);
		eventData.add((int)eNewReligion);
		eventData.add((int)eOldReligion);

		postEvent(eventData, "playerChangeStateReligion");
	}
}


void CvDllPythonEvents::reportPlayerGoldTrade(PlayerTypes eFromPlayer, PlayerTypes eToPlayer, int iAmount)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("playerGoldTrade");			// add key to lookup python handler fxn

		eventData.add((int)eFromPlayer);
		eventData.add((int)eToPlayer);
		eventData.add(iAmount);

		postEvent(eventData, "playerGoldTrade");
	}
}

// BUG - Revolution Event - start
void CvDllPythonEvents::reportPlayerRevolution(PlayerTypes ePlayerID, int iAnarchyLength, CivicTypes* paeOldCivics, CivicTypes* paeNewCivics)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("playerRevolution");			// add key to lookup python handler fxn

		eventData.add((int)ePlayerID);
		eventData.add(iAnarchyLength);
		eventData.add((const int*)paeOldCivics, GC.getNumCivicOptionInfos());
		eventData.add((const int*)paeNewCivics, GC.getNumCivicOptionInfos());

		postEvent(eventData, "playerRevolution");
	}
}
// BUG - Revolution Event - end

void CvDllPythonEvents::reportGenericEvent(const char* szEventName, void *pyArgs)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add(szEventName);
		eventData.add(pyArgs);		// generic args tuple
		postEvent(eventData, szEventName);
	}
}

void CvDllPythonEvents::preSave()
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("OnPreSave");
		postEvent(eventData, "OnPreSave");
	}
}
/************************************************************************************************/
/* Afforess	                  Start		 07/19/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
void CvDllPythonEvents::reportAddTeam(TeamTypes eIndex0, TeamTypes eIndex1, bool bAdded)
{
	if (preEvent())
	{
		CyArgsList eventData;
		eventData.add("addTeam");					// add key to lookup python handler fxn
		eventData.add((int)eIndex0);
		eventData.add((int)eIndex1);
		eventData.add(bAdded);

		postEvent(eventData, "addTeam");
	}
}

/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

