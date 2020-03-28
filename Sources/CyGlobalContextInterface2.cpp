//
// published python interface for CyGlobalContext
// Author - Mustafa Thamer
//

#include "CvGameCoreDLL.h"


void CyGlobalContextPythonInterface2(python::class_<CyGlobalContext>& x)
{
	OutputDebugString("Python Extension Module - CyGlobalContextPythonInterface2\n");

	x
		// global defines.xml
/************************************************************************************************/
/* MOD_COMPONENT_CONTROL                   08/02/07                            MRGENIE          */
/*                                                                                              */
/* Return true/false from                                                                       */
/************************************************************************************************/
		.def("getDefineBOOL", &CyGlobalContext::getDefineBOOL, "bool ( string szName)")
/************************************************************************************************/
/* Mod Globals    Start                          09/13/10                           phungus420  */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		.def("isDYNAMIC_CIV_NAMES", &CyGlobalContext::isDYNAMIC_CIV_NAMES, "bool ()")

		.def("isSS_ENABLED", &CyGlobalContext::isSS_ENABLED, "bool ()")
		.def("isSS_BRIBE", &CyGlobalContext::isSS_BRIBE, "bool ()")
		.def("isSS_ASSASSINATE", &CyGlobalContext::isSS_ASSASSINATE, "bool ()")
/************************************************************************************************/
/* Mod Globals                        END                                           phungus420  */
/************************************************************************************************/

		.def("getDefineINT", &CyGlobalContext::getDefineINT, "int ( string szName )" )
		.def("getDefineFLOAT", &CyGlobalContext::getDefineFLOAT, "float ( string szName )" )
		.def("getDefineSTRING", &CyGlobalContext::getDefineSTRING, "string getDefineSTRING( string szName )" )
		.def("setDefineINT", &CyGlobalContext::setDefineINT, "void ( string szName, int iValue )" )
		.def("setDefineFLOAT", &CyGlobalContext::setDefineFLOAT, "void setDefineFLOAT( string szName, float fValue )" )
		.def("setDefineSTRING", &CyGlobalContext::setDefineSTRING, "void ( string szName, string szValue )" )
/************************************************************************************************/
/* Afforess	                  Start		 08/29/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		.def("setNoUpdateDefineINT", &CyGlobalContext::setNoUpdateDefineINT, "void ( string szName, int iValue )" )
		.def("setNoUpdateDefineFLOAT", &CyGlobalContext::setNoUpdateDefineFLOAT, "void setDefineFLOAT( string szName, float fValue )" )
		.def("setNoUpdateDefineSTRING", &CyGlobalContext::setNoUpdateDefineSTRING, "void ( string szName, string szValue )" )
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	#define EXPOSE_GLOBAL_DEFINE(VAR) \
		.def("get##VAR", &CyGlobalContext::get##VAR, "int ()")
		DO_FOR_EACH_EXPOSED_INT_GLOBAL_DEFINE(EXPOSE_GLOBAL_DEFINE)

		.def("getCAMERA_MIN_YAW", &CyGlobalContext::getCAMERA_MIN_YAW, "float ()")
		.def("getCAMERA_MAX_YAW", &CyGlobalContext::getCAMERA_MAX_YAW, "float ()")
		.def("getCAMERA_FAR_CLIP_Z_HEIGHT", &CyGlobalContext::getCAMERA_FAR_CLIP_Z_HEIGHT, "float ()")
		.def("getCAMERA_MAX_TRAVEL_DISTANCE", &CyGlobalContext::getCAMERA_MAX_TRAVEL_DISTANCE, "float ()")
		.def("getCAMERA_START_DISTANCE", &CyGlobalContext::getCAMERA_START_DISTANCE, "float ()")
		.def("getAIR_BOMB_HEIGHT", &CyGlobalContext::getAIR_BOMB_HEIGHT, "float ()")
		.def("getPLOT_SIZE", &CyGlobalContext::getPLOT_SIZE, "float ()")
		.def("getCAMERA_SPECIAL_PITCH", &CyGlobalContext::getCAMERA_SPECIAL_PITCH, "float ()")
		.def("getCAMERA_MAX_TURN_OFFSET", &CyGlobalContext::getCAMERA_MAX_TURN_OFFSET, "float ()")
		.def("getCAMERA_MIN_DISTANCE", &CyGlobalContext::getCAMERA_MIN_DISTANCE, "float ()")
		.def("getCAMERA_UPPER_PITCH", &CyGlobalContext::getCAMERA_UPPER_PITCH, "float ()")
		.def("getCAMERA_LOWER_PITCH", &CyGlobalContext::getCAMERA_LOWER_PITCH, "float ()")
		.def("getFIELD_OF_VIEW", &CyGlobalContext::getFIELD_OF_VIEW, "float ()")
		.def("getSHADOW_SCALE", &CyGlobalContext::getSHADOW_SCALE, "float ()")
		.def("getUNIT_MULTISELECT_DISTANCE", &CyGlobalContext::getUNIT_MULTISELECT_DISTANCE, "float ()")

		.def("getMAX_PC_PLAYERS", &CyGlobalContext::getMAX_PC_PLAYERS, "int ()")
		.def("getMAX_PLAYERS", &CyGlobalContext::getMAX_PLAYERS, "int ()")
		.def("getMAX_PC_TEAMS", &CyGlobalContext::getMAX_PC_TEAMS, "int ()")
		.def("getMAX_TEAMS", &CyGlobalContext::getMAX_TEAMS, "int ()")
		.def("getBARBARIAN_PLAYER", &CyGlobalContext::getBARBARIAN_PLAYER, "int ()")
		.def("getBARBARIAN_TEAM", &CyGlobalContext::getBARBARIAN_TEAM, "int ()")
		.def("getNEANDERTHAL_PLAYER", &CyGlobalContext::getNEANDERTHAL_PLAYER, "int ()")
		.def("getNEANDERTHAL_TEAM", &CyGlobalContext::getNEANDERTHAL_TEAM, "int ()")
		.def("getBEAST_PLAYER", &CyGlobalContext::getBEAST_PLAYER, "int ()")
		.def("getBEAST_TEAM", &CyGlobalContext::getBEAST_TEAM, "int ()")
		.def("getPREDATOR_PLAYER", &CyGlobalContext::getPREDATOR_PLAYER, "int ()")
		.def("getPREDATOR_TEAM", &CyGlobalContext::getPREDATOR_TEAM, "int ()")
		.def("getPREY_PLAYER", &CyGlobalContext::getPREY_PLAYER, "int ()")
		.def("getPREY_TEAM", &CyGlobalContext::getPREY_TEAM, "int ()")
		.def("getINSECT_PLAYER", &CyGlobalContext::getINSECT_PLAYER, "int ()")
		.def("getINSECT_TEAM", &CyGlobalContext::getINSECT_TEAM, "int ()")
		.def("getNPC4_PLAYER", &CyGlobalContext::getNPC4_PLAYER, "int ()")
		.def("getNPC4_TEAM", &CyGlobalContext::getNPC4_TEAM, "int ()")
		.def("getNPC3_PLAYER", &CyGlobalContext::getNPC3_PLAYER, "int ()")
		.def("getNPC3_TEAM", &CyGlobalContext::getNPC3_TEAM, "int ()")
		.def("getNPC2_PLAYER", &CyGlobalContext::getNPC2_PLAYER, "int ()")
		.def("getNPC2_TEAM", &CyGlobalContext::getNPC2_TEAM, "int ()")
		.def("getNPC1_PLAYER", &CyGlobalContext::getNPC1_PLAYER, "int ()")
		.def("getNPC1_TEAM", &CyGlobalContext::getNPC1_TEAM, "int ()")
		.def("getNPC0_PLAYER", &CyGlobalContext::getNPC0_PLAYER, "int ()")
		.def("getNPC0_TEAM", &CyGlobalContext::getNPC0_TEAM, "int ()")
		.def("getINVALID_PLOT_COORD", &CyGlobalContext::getINVALID_PLOT_COORD, "int ()")
		.def("getNUM_CITY_PLOTS", &CyGlobalContext::getNUM_CITY_PLOTS, "int ()")
		.def("getCITY_HOME_PLOT", &CyGlobalContext::getCITY_HOME_PLOT, "int ()")
		;
}
