#ifndef MODULAR_MOD_TOOLS_H
#define MODULAR_MOD_TOOLS_H

// Please all developers of the WoC Project, add any enable/disable stuff from modules in here
// like the example shown below

/************************************************************************************************/
/* TGA_INDEXATION                          01/21/08                                MRGENIE      */
/*                                                                                              */
/* These numbers must correspond to the number of religions/corporations represented in the tga */
/************************************************************************************************/
#define TGA_RELIGIONS	 GC.getTGA_RELIGIONS()
#define TGA_CORPORATIONS GC.getTGA_CORPORATIONS()
/************************************************************************************************/
/* TGA_INDEXATION                          END                                                  */
/************************************************************************************************/
/************************************************************************************************/
/* MODULES                                 10/16/07                                MRGENIE      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
// Dale - DCM: Globals START
#define USE_DCM_ATTACK_SUPPORT GC.isDCM_ATTACK_SUPPORT()
#define USE_BATTLE_EFFECTS	   GC.isDCM_BATTLE_EFFECTS()
#define USE_DCM_AIR_BOMBING	   GC.isDCM_AIR_BOMBING()
#define USE_DCM_RANGE_BOMBARD  GC.isDCM_RANGE_BOMBARD()
#define USE_DCM_CIV_CHANGER	   GC.isDCM_CIV_CHANGER()
#define USE_DCM_STACK_ATTACK   GC.isDCM_STACK_ATTACK()
#define USE_DCM_OPP_FIRE	   GC.isDCM_OPP_FIRE()
#define USE_DCM_ACTIVE_DEFENSE GC.isDCM_ACTIVE_DEFENSE()
#define USE_DCM_ARCHER_BOMBARD GC.isDCM_ARCHER_BOMBARD()
#define USE_DCM_FIGHTER_ENGAGE GC.isDCM_FIGHTER_ENGAGE()
// Dale - DCM: Globals END
// Dale - CCH: Globals START
#define DCM_CIV_CHANGER_MODE	GC.getDCM_CIV_CHANGER_MODE()
#define DCM_CIV_CHANGER_CHANGES GC.getDCM_CIV_CHANGER_CHANGES()
// Dale - CCH: Globals END
#define USE_REVOLUTION_MOD			GC.isREVOLUTION_MOD() // REVOLUTION_MOD
#define USE_BARBARIAN_CIV			GC.isBARBARIAN_CIV() // BARBARIAN_CIV
#define USE_TECH_DIFFUSION			GC.isTECH_DIFFUSION() // TECH_DIFFUSION
#define USE_DYNMAIC_CIV_NAMES		GC.isDYNAMIC_CIV_NAMES() // DYNAMIC_CIV_NAMES
#define USE_START_AS_MINORS			GC.isSTART_AS_MINORS() // START_AS_MINORS
#define USE_INFLUENCE_DRIVEN_WAR	GC.isInfluenceGDrivenWar()
#define SuperSpies					GC.isSUPER_SPIES() // Super Spies additions
#define SuperSpies_Debug			GC.isSUPER_SPIES_DEBUG() // Super Spies debugging flags
#define SuperSpies_Debug_SpyCapture GC.isSUPER_SPIES_DEBUG_SPY_CAPTURE()
#define SuperSpies_Debug_AI			GC.isSUPER_SPIES_DEBUG_AI()
#define USE_INQUISITIONS			GC.isINQUISITIONS_ENABLED()
/************************************************************************************************/
/* MODULES                                 END	                                                 */
/************************************************************************************************/
#define OWNERSHIP_INFO_WIP 1
#define DEBUG_TGA_INDEX	   1

#endif // ifndef MODULAR_MOD_TOOLS_H
