

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 22:14:07 2038
 */
/* Compiler settings for IBuildingInfo.idl:
    Oicf, W4, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0622 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __IBuildingInfo_h__
#define __IBuildingInfo_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IBuildingInfo_INTERFACE_DEFINED__
#define __IBuildingInfo_INTERFACE_DEFINED__

/* interface IBuildingInfo */
/* [explicit_handle][version][uuid] */ 

typedef /* [context_handle] */ void *hIBuildingInfo_t;

hIBuildingInfo_t IBuildingInfo_create( 
    /* [in] */ handle_t hBinding);

void IBuildingInfo_delete( 
    /* [in] */ hIBuildingInfo_t hBuilding);

boolean IBuildingInfo_isNoLimit( 
    /* [in] */ hIBuildingInfo_t hBuilding);

void IBuildingInfo_setNoLimit( 
    /* [in] */ hIBuildingInfo_t hBuilding,
    /* [in] */ boolean b);



extern RPC_IF_HANDLE IBuildingInfo_v1_0_c_ifspec;
extern RPC_IF_HANDLE IBuildingInfo_v1_0_s_ifspec;
#endif /* __IBuildingInfo_INTERFACE_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

void __RPC_USER hIBuildingInfo_t_rundown( hIBuildingInfo_t );

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


