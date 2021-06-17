#include "CvGameCoreDLL.h"
#include "CvPython.h"
#include "CyCity.h"
#include "CyGlobalContext.h"
#include "CyPlayer.h"
#include "CyPlot.h"
#include "CyUnit.h"
#include "SCyDebug.h"
#include "Win32.h"


void CyCityPythonInterface1(python::class_<CyCity>& x);
void CyCityPythonInterface2(python::class_<CyCity>& x);
void CyPlotPythonInterface1(python::class_<CyPlot>& x);
void CyPlayerPythonInterface1(python::class_<CyPlayer>& x);
void CyPlayerPythonInterface2(python::class_<CyPlayer>& x);
void CyPlayerPythonInterface3(python::class_<CyPlayer>& x);
void CyUnitPythonInterface1(python::class_<CyUnit>& x);
void CyGlobalContextPythonInterface1(python::class_<CyGlobalContext>& x);
void CyGlobalContextPythonInterface2(python::class_<CyGlobalContext>& x);
void CyGlobalContextPythonInterface3(python::class_<CyGlobalContext>& x);
void CyGlobalContextPythonInterface4(python::class_<CyGlobalContext>& x);
void CyGamePythonInterface();
void CyRandomPythonInterface();
void CyEnumsPythonInterface();
void CyTeamPythonInterface();
void CyAreaPythonInterface();
void CyStructsPythonInterface1();
void CyMapPythonInterface();
void CyMapGeneratorPythonInterface();
void CyInfoPythonInterface1();
void CyInfoPythonInterface2();
void CyInfoPythonInterface3();
void CyInfoPythonInterface4();
void CySelectionGroupInterface();
void CyArtFileMgrPythonInterface();
void CyGameTextMgrInterface();
void CyHallOfFameInterface();
void CyGameCoreUtilsPythonInterface();
void CyMessageControlInterface();
void CyPropertiesPythonInterface();


DllExport void DLLPublishToPython()
{
	OutputDebugString("Publishing to Python: Start\n");

	using namespace Cy::call_policy;

	PyIntConverter::registerForType<TechTypes>();
	PyIntConverter::registerForType<BuildingTypes>();
	PyIntConverter::registerForType<MultiplayerOptionTypes>();
	PyIntConverter::registerForType<CorporationTypes>();
	PyIntConverter::registerForType<GameOptionTypes>();
	PyIntConverter::registerForType<PlayerTypes>();
	PyIntConverter::registerForType<VictoryTypes>();
	PyIntConverter::registerForType<VoteTypes>();
	PyIntConverter::registerForType<VoteSourceTypes>();
	PyIntConverter::registerForType<ReligionTypes>();
	PyIntConverter::registerForType<ImprovementTypes>();
	PyIntConverter::registerForType<CivilizationTypes>();
	PyIntConverter::registerForType<TeamTypes>();
	PyIntConverter::registerForType<ProjectTypes>();
	PyIntConverter::registerForType<SpecialUnitTypes>();
	PyIntConverter::registerForType<CivicOptionTypes>();
	PyIntConverter::registerForType<CivicTypes>();
	PyIntConverter::registerForType<SpecialBuildingTypes>();
	PyIntConverter::registerForType<ControlTypes>();
	PyIntConverter::registerForType<ForceControlTypes>();
	PyIntConverter::registerForType<EventTriggerTypes>();
	PyIntConverter::registerForType<LeaderHeadTypes>();
	PyIntConverter::registerForType<CultureLevelTypes>();
	PyIntConverter::registerForType<ReplayMessageTypes>();
	PyIntConverter::registerForType<ModderGameOptionTypes>();
	PyIntConverter::registerForType<UnitTypes>();
	PyIntConverter::registerForType<YieldTypes>();
	PyIntConverter::registerForType<CultureLevelTypes>();
	PyIntConverter::registerForType<CommerceTypes>();
	PyIntConverter::registerForType<ColorTypes>();
	PyIntConverter::registerForType<EraTypes>();
	PyIntConverter::registerForType<ForceControlTypes>();
	PyIntConverter::registerForType<BonusTypes>();
	PyIntConverter::registerForType<HurryTypes>();
	PyIntConverter::registerForType<MapTypes>();

	CyEnumsPythonInterface();
	CyGamePythonInterface();
	CyRandomPythonInterface();
	CyTeamPythonInterface();
	CyAreaPythonInterface();
	CyStructsPythonInterface1();
	CyMapPythonInterface();
	CyMapGeneratorPythonInterface();
	CySelectionGroupInterface();
	CyArtFileMgrPythonInterface();
	CyGameTextMgrInterface();
	CyInfoPythonInterface1();
	CyInfoPythonInterface2();
	CyInfoPythonInterface3();
	CyInfoPythonInterface4();
	CyHallOfFameInterface();
	CyGameCoreUtilsPythonInterface();
	CyMessageControlInterface();
	CyPropertiesPythonInterface();

	SCyDebug::installInPython();

	//
	// large interfaces which can be split across files if need be
	//
	python::class_<CyCity> city ("CyCity");		// define city class
	CyCityPythonInterface1(city);				// publish it's methods
	CyCityPythonInterface2(city);				// publish it's methods

	python::class_<CyPlayer> player ("CyPlayer");	// define player class
	CyPlayerPythonInterface1(player);				// publish it's methods
	CyPlayerPythonInterface2(player);				// publish it's methods
	CyPlayerPythonInterface3(player);				// publish it's methods

	python::class_<CyUnit> unit ("CyUnit");		// define unit class
	CyUnitPythonInterface1(unit);				// publish it's methods

	python::class_<CyPlot> plot ("CyPlot");		// define plot class
	CyPlotPythonInterface1(plot);				// publish it's methods

	python::class_<CyGlobalContext> gc ("CyGlobalContext");	// define globals class 
	CyGlobalContextPythonInterface1(gc);					// publish it's methods 
	CyGlobalContextPythonInterface2(gc);					// publish it's methods
	CyGlobalContextPythonInterface3(gc);					// publish it's methods
	CyGlobalContextPythonInterface4(gc);					// publish it's methods 
	
	Win32::pythonPublish();

	OutputDebugString("Publishing to Python: End\n");
}
