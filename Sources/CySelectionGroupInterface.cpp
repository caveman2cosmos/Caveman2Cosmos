#include "CvGameCoreDLL.h"
#include "CyArea.h"
#include "CyPlot.h"
#include "CySelectionGroup.h"
#include "CyUnit.h"

//
// published python interface for CySelectionGroup
//

void CySelectionGroupInterface()
{
	OutputDebugString("Python Extension Module - CySelectionGroupInterface\n");

	python::class_<CySelectionGroup>("CySelectionGroup", python::no_init)
		.def("pushMission", &CySelectionGroup::pushMission, "void (eMission, iData1, iData2, iFlags, bAppend, bManual, eMissionAI, pMissionAIPlot, pMissionAIUnit)")
		.def("canStartMission", &CySelectionGroup::canStartMission, "bool (int iMission, int iData1, int iData2, CyPlot* pPlot, bool bTestVisible)")

		.def("isHuman", &CySelectionGroup::isHuman, "bool ()")
		.def("baseMoves", &CySelectionGroup::baseMoves, "int ()")
		.def("isWaiting", &CySelectionGroup::isWaiting, "bool ()")
		.def("isFull", &CySelectionGroup::isFull, "bool ()")
		.def("hasMoved", &CySelectionGroup::hasMoved, "bool ()")
		.def("canMoveInto", &CySelectionGroup::canMoveInto, "bool (CyPlot* pPlot, bool bAttack) - can the group move into pPlot?")
		.def("canFight", &CySelectionGroup::canFight, "bool ()")
		.def("isInvisible", &CySelectionGroup::isInvisible, "bool (int eTeam)")

		.def("plot", &CySelectionGroup::plot, python::return_value_policy<python::manage_new_object>(), "CyPlot () - get plot that the group is on")
		.def("area", &CySelectionGroup::area, python::return_value_policy<python::manage_new_object>(), "CyArea ()*")

		.def("readyToMove", &CySelectionGroup::readyToMove, "bool (bool bAny) - is the group awake and ready to move?")

		.def("getID", &CySelectionGroup::getID, "int () - the ID for the SelectionGroup")
		.def("getOwner", &CySelectionGroup::getOwner, "int (PlayerTypes) () - ID for owner of the group")
		.def("getTeam", &CySelectionGroup::getTeam, "int (TeamTypes) () - ID for team owner of the group")
		.def("getActivityType", &CySelectionGroup::getActivityType, "int /*ActivityTypes*/ () - ActivityTypes the group is engaging in")
		.def("setActivityType", &CySelectionGroup::setActivityType, "void (int /*ActivityTypes*/ eNewValue) - set the group to this ActivityTypes")
		.def("getAutomateType", &CySelectionGroup::getAutomateType, "int /*AutomateTypes*/ () - AutomateTypes the group is engaging in")
		.def("isAutomated", &CySelectionGroup::isAutomated, "bool () - Is the group automated?")
		.def("setAutomateType", &CySelectionGroup::setAutomateType, "void (int /*AutomateTypes*/ eNewValue) - get the group to perform this AutomateTypes")

		.def("getNumUnits", &CySelectionGroup::getNumUnits, "int ()")			// JS Help!
		.def("getLengthMissionQueue", &CySelectionGroup::getLengthMissionQueue, "int ()")
		.def("getHeadUnit", &CySelectionGroup::getHeadUnit, python::return_value_policy<python::manage_new_object>(), "CyUnit* ()")
		.def("getMissionType", &CySelectionGroup::getMissionType, "int (int iNode)")
		.def("getMissionData1", &CySelectionGroup::getMissionData1, "int (int iNode)")
	;
}
