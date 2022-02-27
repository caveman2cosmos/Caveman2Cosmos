#include "CvGameCoreDLL.h"
#include "CvInfoUtil.h"
#include "CvPython.h"
#include "CyCity.h"
#include "CyGlobalContext.h"
#include "CyPlayer.h"
#include "CyPlot.h"
#include "CyUnit.h"
#include "SCyDebug.h"
#include "IDValueMap.h"
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
void CyBoolExprPythonInterface();
void CyIntExprPythonInterface();


DllExport void DLLPublishToPython()
{
	OutputDebugString("Publishing to Python: Start\n");

	using namespace Cy::call_policy;

	registerAllowPyIntAsType<TechTypes>();
	registerAllowPyIntAsType<BuildingTypes>();
	registerAllowPyIntAsType<DirectionTypes>();
	registerAllowPyIntAsType<MultiplayerOptionTypes>();
	registerAllowPyIntAsType<CorporationTypes>();
	registerAllowPyIntAsType<GameOptionTypes>();
	registerAllowPyIntAsType<PlayerTypes>();
	registerAllowPyIntAsType<ReligionTypes>();
	registerAllowPyIntAsType<ImprovementTypes>();
	registerAllowPyIntAsType<CivilizationTypes>();
	registerAllowPyIntAsType<TeamTypes>();
	registerAllowPyIntAsType<ProjectTypes>();
	registerAllowPyIntAsType<SpecialUnitTypes>();
	registerAllowPyIntAsType<CivicOptionTypes>();
	registerAllowPyIntAsType<CivicTypes>();
	registerAllowPyIntAsType<SpecialBuildingTypes>();
	registerAllowPyIntAsType<ControlTypes>();
	registerAllowPyIntAsType<ForceControlTypes>();
	registerAllowPyIntAsType<EventTriggerTypes>();
	registerAllowPyIntAsType<LeaderHeadTypes>();
	registerAllowPyIntAsType<CultureLevelTypes>();
	registerAllowPyIntAsType<ReplayMessageTypes>();
	registerAllowPyIntAsType<ModderGameOptionTypes>();
	registerAllowPyIntAsType<YieldTypes>();
	registerAllowPyIntAsType<CultureLevelTypes>();
	registerAllowPyIntAsType<CommerceTypes>();
	registerAllowPyIntAsType<ColorTypes>();
	registerAllowPyIntAsType<EraTypes>();
	registerAllowPyIntAsType<ForceControlTypes>();
	registerAllowPyIntAsType<BonusTypes>();
	registerAllowPyIntAsType<HurryTypes>();
	registerAllowPyIntAsType<MapTypes>();
	registerAllowPyIntAsType<MapCategoryTypes>();
	registerAllowPyIntAsType<UnitAITypes>();
	registerAllowPyIntAsType<DomainTypes>();
	registerAllowPyIntAsType<PropertyTypes>();
	registerAllowPyIntAsType<ProcessTypes>();
	registerAllowPyIntAsType<UnitCombatTypes>();
	registerAllowPyIntAsType<UnitTypes>();
	registerAllowPyIntAsType<VictoryTypes>();
	registerAllowPyIntAsType<VoteTypes>();
	registerAllowPyIntAsType<VoteSourceTypes>();
	registerAllowPyIntAsType<FeatureTypes>();
	registerAllowPyIntAsType<TerrainTypes>();
	registerAllowPyIntAsType<PromotionTypes>();

	publishPythonVectorInterface<std::vector<BonusTypes>, CovertToInteger>();
	publishPythonVectorInterface<std::vector<ImprovementTypes>, CovertToInteger>();
	publishPythonVectorInterface<std::vector<MapCategoryTypes>, CovertToInteger>();
	publishPythonVectorInterface<std::vector<TechTypes>, CovertToInteger>();

	publishIDValueMapPythonInterface<IDValueMap<BonusTypes, int> >();
	publishIDValueMapPythonInterface<IDValueMap<BuildingTypes, int> >();
	publishIDValueMapPythonInterface<IDValueMap<ImprovementTypes, int> >();
	publishIDValueMapPythonInterface<IDValueMap<TechTypes, int> >();
	publishIDValueMapPythonInterface<IDValueMap<TerrainTypes, int> >();
	publishIDValueMapPythonInterface<IDValueMap<UnitCombatTypes, int> >();
	publishIDValueMapPythonInterface<IDValueMap<UnitTypes, int> >();

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
	CyBoolExprPythonInterface();
	CyIntExprPythonInterface();
	CvInfoUtil::publishPythonInterface();
	SCyDebug::installInPython();

	//
	// large interfaces which can be split across files if need be
	//
	python::class_<CyCity> city("CyCity", python::no_init);			// define city class
	CyCityPythonInterface1(city);									// publish it's methods
	CyCityPythonInterface2(city);									// publish it's methods

	python::class_<CyPlayer> player("CyPlayer", python::no_init);	// define player class
	CyPlayerPythonInterface1(player);								// publish it's methods
	CyPlayerPythonInterface2(player);								// publish it's methods
	CyPlayerPythonInterface3(player);								// publish it's methods

	python::class_<CyUnit> unit("CyUnit", python::no_init);			// define unit class
	CyUnitPythonInterface1(unit);									// publish it's methods

	python::class_<CyPlot> plot("CyPlot", python::no_init);			// define plot class
	CyPlotPythonInterface1(plot);									// publish it's methods

	python::class_<CyGlobalContext> gc("CyGlobalContext");			// define globals class
	CyGlobalContextPythonInterface1(gc);							// publish it's methods
	CyGlobalContextPythonInterface2(gc);							// publish it's methods
	CyGlobalContextPythonInterface3(gc);							// publish it's methods
	CyGlobalContextPythonInterface4(gc);							// publish it's methods

	Win32::pythonPublish();

	OutputDebugString("Publishing to Python: End\n");
}
