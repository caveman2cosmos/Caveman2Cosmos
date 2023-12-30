#include "CvGameCoreDLL.h"
#include "CyIterator.h"
#include "CvPython.h"
#include "CyCity.h"
#include "CyGlobalContext.h"
#include "CyPlayer.h"
#include "CyPlot.h"
#include "CyUnit.h"
#include "SCyDebug.h"
#include "IDValueMap.h"
#include "Win32.h"
#include "CvPythonCityLoader.h"
#include "CvPythonPlotLoader.h"
#include "CvPythonPlayerLoader.h"
#include "CvPythonUnitLoader.h"
#include "CvPythonEnumLoader.h"
#include "CvPythonGlobalContextLoader.h"



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
void CyCityOutputHistoryInterface();


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
	registerAllowPyIntAsType<HeritageTypes>();
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
	registerAllowPyIntAsType<FlavorTypes>();

	publishPythonVectorInterface<std::vector<BonusTypes>, CovertToInteger>();
	publishPythonVectorInterface<std::vector<HeritageTypes>, CovertToInteger>();
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

	CvPythonEnumLoader::CyEnumsPythonInterface();
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
	CyCityOutputHistoryInterface();
	SCyDebug::installInPython();

	//
	// large interfaces which can be split across files if need be
	//
	boost::python::class_<CyCity> city("CyCity", boost::python::no_init);			// define city class
	CvPythonCityLoader::CyCityPythonInterface1(city);									// publish it's methods
	CvPythonCityLoader::CyCityPythonInterface2(city);									// publish it's methods

	boost::python::class_<CyPlayer> player("CyPlayer", boost::python::no_init);	// define player class
	CvPythonPlayerLoader::CyPlayerPythonInterface1(player);								// publish it's methods
	CvPythonPlayerLoader::CyPlayerPythonInterface2(player);								// publish it's methods
	CvPythonPlayerLoader::CyPlayerPythonInterface3(player);								// publish it's methods

	boost::python::class_<CyUnit> unit("CyUnit", boost::python::no_init);			// define unit class
	CvPythonUnitLoader::CyUnitPythonInterface1(unit);									// publish it's methods

	boost::python::class_<CyPlot> plot("CyPlot", boost::python::no_init);			// define plot class
	CvPythonPlotLoader::CyPlotPythonInterface1(plot);									// publish it's methods

	boost::python::class_<CyGlobalContext> gc("CyGlobalContext");			// define globals class
	CvPythonGlobalContextLoader::CyGlobalContextPythonInterface1(gc);							// publish it's methods
	CvPythonGlobalContextLoader::CyGlobalContextPythonInterface2(gc);							// publish it's methods
	CvPythonGlobalContextLoader::CyGlobalContextPythonInterface3(gc);							// publish it's methods
	CvPythonGlobalContextLoader::CyGlobalContextPythonInterface4(gc);							// publish it's methods

	Win32::pythonPublish();

	OutputDebugString("Publishing to Python: End\n");
}
