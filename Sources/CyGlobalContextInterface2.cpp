#include "CvGameCoreDLL.h"
#include "CyGlobalContext.h"
#include "CvImprovementInfo.h"
#include "CvInfos.h"

//
// published python interface for CyGlobalContext
// Author - Mustafa Thamer
//

void CyGlobalContextPythonInterface2(python::class_<CyGlobalContext>& x)
{
	OutputDebugString("Python Extension Module - CyGlobalContextPythonInterface2\n");
	x
		// global defines.xml
		.def("isDCM_AIR_BOMBING", &CyGlobalContext::isDCM_AIR_BOMBING, "bool ()")
		.def("isDCM_RANGE_BOMBARD", &CyGlobalContext::isDCM_RANGE_BOMBARD, "bool ()")
		.def("isDCM_ATTACK_SUPPORT", &CyGlobalContext::isDCM_ATTACK_SUPPORT, "bool ()")
		.def("isDCM_OPP_FIRE", &CyGlobalContext::isDCM_OPP_FIRE, "bool ()")
		.def("isDCM_ACTIVE_DEFENSE", &CyGlobalContext::isDCM_ACTIVE_DEFENSE, "bool ()")
		.def("isDCM_FIGHTER_ENGAGE", &CyGlobalContext::isDCM_FIGHTER_ENGAGE, "bool ()")

		.def("isIDW_ENABLED", &CyGlobalContext::isIDW_ENABLED, "bool ()")
		.def("isIDW_EMERGENCY_DRAFT_ENABLED", &CyGlobalContext::isIDW_EMERGENCY_DRAFT_ENABLED, "bool ()")
		.def("isIDW_NO_BARBARIAN_INFLUENCE", &CyGlobalContext::isIDW_NO_BARBARIAN_INFLUENCE, "bool ()")
		.def("isIDW_NO_NAVAL_INFLUENCE", &CyGlobalContext::isIDW_NO_NAVAL_INFLUENCE, "bool ()")
		.def("isIDW_PILLAGE_INFLUENCE_ENABLED", &CyGlobalContext::isIDW_PILLAGE_INFLUENCE_ENABLED, "bool ()")

		.def("isSS_ENABLED", &CyGlobalContext::isSS_ENABLED, "bool ()")
		.def("isSS_BRIBE", &CyGlobalContext::isSS_BRIBE, "bool ()")
		.def("isSS_ASSASSINATE", &CyGlobalContext::isSS_ASSASSINATE, "bool ()")

		.def("getDefineINT", &CyGlobalContext::getDefineINT, "int ( string szName )" )
		.def("getDefineFLOAT", &CyGlobalContext::getDefineFLOAT, "float ( string szName )" )
		.def("setDefineINT", &CyGlobalContext::setDefineINT, "void ( string szName, int iValue )" )
		.def("setDefineFLOAT", &CyGlobalContext::setDefineFLOAT, "void setDefineFLOAT( string szName, float fValue )" )
		.def("setNoUpdateDefineFLOAT", &CyGlobalContext::setNoUpdateDefineFLOAT, "void setDefineFLOAT( string szName, float fValue )" )

		.def("getMAX_PC_PLAYERS", &CyGlobalContext::getMAX_PC_PLAYERS, "int ()")
		.def("getMAX_PLAYERS", &CyGlobalContext::getMAX_PLAYERS, "int ()")
		.def("getMAX_PC_TEAMS", &CyGlobalContext::getMAX_PC_TEAMS, "int ()")
		.def("getMAX_TEAMS", &CyGlobalContext::getMAX_TEAMS, "int ()")
		.def("getBARBARIAN_PLAYER", &CyGlobalContext::getBARBARIAN_PLAYER, "int ()")
		.def("getNUM_CITY_PLOTS", &CyGlobalContext::getNUM_CITY_PLOTS, "int ()")

#define EXPOSE_FUNC(unused_, VAR) \
	_EXPOSE_FUNC(get##VAR)

	#define _EXPOSE_FUNC(name) \
		.def(#name, &CyGlobalContext::name)

		DO_FOR_EACH_EXPOSED_INT_GLOBAL_DEFINE(EXPOSE_FUNC)
		DO_FOR_EACH_EXPOSED_INFO_TYPE(EXPOSE_FUNC)
	;
}
