

/* this ALWAYS GENERATED file contains the RPC server stubs */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 22:14:07 2038
 */
/* Compiler settings for IServerUtils.idl:
    Oicf, W4, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0622 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_)


#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#include <string.h>
#include "IServerUtils.h"

#define TYPE_FORMAT_STRING_SIZE   3                                 
#define PROC_FORMAT_STRING_SIZE   25                                
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _IServerUtils_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } IServerUtils_MIDL_TYPE_FORMAT_STRING;

typedef struct _IServerUtils_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } IServerUtils_MIDL_PROC_FORMAT_STRING;

typedef struct _IServerUtils_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } IServerUtils_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};

extern const IServerUtils_MIDL_TYPE_FORMAT_STRING IServerUtils__MIDL_TypeFormatString;
extern const IServerUtils_MIDL_PROC_FORMAT_STRING IServerUtils__MIDL_ProcFormatString;
extern const IServerUtils_MIDL_EXPR_FORMAT_STRING IServerUtils__MIDL_ExprFormatString;

/* Standard interface: IServerUtils, ver. 1.0,
   GUID={0x00000001,0xEAF3,0x4A7A,{0xA0,0xF2,0xBC,0xE4,0xC3,0x0D,0xA7,0x7E}} */


extern const MIDL_SERVER_INFO IServerUtils_ServerInfo;

extern const RPC_DISPATCH_TABLE IServerUtils_v1_0_DispatchTable;

static const RPC_SERVER_INTERFACE IServerUtils___RpcServerInterface =
    {
    sizeof(RPC_SERVER_INTERFACE),
    {{0x00000001,0xEAF3,0x4A7A,{0xA0,0xF2,0xBC,0xE4,0xC3,0x0D,0xA7,0x7E}},{1,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    (RPC_DISPATCH_TABLE*)&IServerUtils_v1_0_DispatchTable,
    0,
    0,
    0,
    &IServerUtils_ServerInfo,
    0x04000000
    };
RPC_IF_HANDLE IServerUtils_v1_0_s_ifspec = (RPC_IF_HANDLE)& IServerUtils___RpcServerInterface;

extern const MIDL_STUB_DESC IServerUtils_StubDesc;


#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif
#if !(TARGET_IS_NT60_OR_LATER)
#error You need Windows Vista or later to run this stub because it uses these features:
#error   compiled for Windows Vista.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const IServerUtils_MIDL_PROC_FORMAT_STRING IServerUtils__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure shutDownServer */

			0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x0 ),	/* 0 */
/* 14 */	0x40,		/* Oi2 Flags:  has ext, */
			0x0,		/* 0 */
/* 16 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

			0x0
        }
    };

static const IServerUtils_MIDL_TYPE_FORMAT_STRING IServerUtils__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */

			0x0
        }
    };

static const unsigned short IServerUtils_FormatStringOffsetTable[] =
    {
    0
    };


static const MIDL_STUB_DESC IServerUtils_StubDesc = 
    {
    (void *)& IServerUtils___RpcServerInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    0,
    0,
    0,
    0,
    0,
    IServerUtils__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x60001, /* Ndr library version */
    0,
    0x801026e, /* MIDL Version 8.1.622 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

static const RPC_DISPATCH_FUNCTION IServerUtils_table[] =
    {
    NdrServerCall2,
    0
    };
static const RPC_DISPATCH_TABLE IServerUtils_v1_0_DispatchTable = 
    {
    1,
    (RPC_DISPATCH_FUNCTION*)IServerUtils_table
    };

static const SERVER_ROUTINE IServerUtils_ServerRoutineTable[] = 
    {
    (SERVER_ROUTINE)shutDownServer
    };

static const MIDL_SERVER_INFO IServerUtils_ServerInfo = 
    {
    &IServerUtils_StubDesc,
    IServerUtils_ServerRoutineTable,
    IServerUtils__MIDL_ProcFormatString.Format,
    IServerUtils_FormatStringOffsetTable,
    0,
    0,
    0,
    0};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_) */

