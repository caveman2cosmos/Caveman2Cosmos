

/* this ALWAYS GENERATED file contains the RPC server stubs */


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
#include "IBuildingInfo.h"

#define TYPE_FORMAT_STRING_SIZE   11                                
#define PROC_FORMAT_STRING_SIZE   155                               
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _IBuildingInfo_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } IBuildingInfo_MIDL_TYPE_FORMAT_STRING;

typedef struct _IBuildingInfo_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } IBuildingInfo_MIDL_PROC_FORMAT_STRING;

typedef struct _IBuildingInfo_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } IBuildingInfo_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};

extern const IBuildingInfo_MIDL_TYPE_FORMAT_STRING IBuildingInfo__MIDL_TypeFormatString;
extern const IBuildingInfo_MIDL_PROC_FORMAT_STRING IBuildingInfo__MIDL_ProcFormatString;
extern const IBuildingInfo_MIDL_EXPR_FORMAT_STRING IBuildingInfo__MIDL_ExprFormatString;

/* Standard interface: IBuildingInfo, ver. 1.0,
   GUID={0x00000003,0xEAF3,0x4A7A,{0xA0,0xF2,0xBC,0xE4,0xC3,0x0D,0xA7,0x7E}} */


extern const MIDL_SERVER_INFO IBuildingInfo_ServerInfo;

extern const RPC_DISPATCH_TABLE IBuildingInfo_v1_0_DispatchTable;

static const RPC_SERVER_INTERFACE IBuildingInfo___RpcServerInterface =
    {
    sizeof(RPC_SERVER_INTERFACE),
    {{0x00000003,0xEAF3,0x4A7A,{0xA0,0xF2,0xBC,0xE4,0xC3,0x0D,0xA7,0x7E}},{1,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    (RPC_DISPATCH_TABLE*)&IBuildingInfo_v1_0_DispatchTable,
    0,
    0,
    0,
    &IBuildingInfo_ServerInfo,
    0x04000000
    };
RPC_IF_HANDLE IBuildingInfo_v1_0_s_ifspec = (RPC_IF_HANDLE)& IBuildingInfo___RpcServerInterface;

extern const MIDL_STUB_DESC IBuildingInfo_StubDesc;

extern const NDR_RUNDOWN RundownRoutines[];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif
#if !(TARGET_IS_NT60_OR_LATER)
#error You need Windows Vista or later to run this stub because it uses these features:
#error   compiled for Windows Vista.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const IBuildingInfo_MIDL_PROC_FORMAT_STRING IBuildingInfo__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure IBuildingInfo_create */

			0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 10 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 12 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 14 */	NdrFcShort( 0x0 ),	/* 0 */
/* 16 */	NdrFcShort( 0x24 ),	/* 36 */
/* 18 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 20 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */
/* 26 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 28 */	NdrFcShort( 0x30 ),	/* Flags:  out, return, */
/* 30 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 32 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Procedure IBuildingInfo_delete */

/* 34 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 36 */	NdrFcLong( 0x0 ),	/* 0 */
/* 40 */	NdrFcShort( 0x1 ),	/* 1 */
/* 42 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 44 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 46 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 48 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 50 */	NdrFcShort( 0x24 ),	/* 36 */
/* 52 */	NdrFcShort( 0x0 ),	/* 0 */
/* 54 */	0x40,		/* Oi2 Flags:  has ext, */
			0x1,		/* 1 */
/* 56 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 58 */	NdrFcShort( 0x0 ),	/* 0 */
/* 60 */	NdrFcShort( 0x0 ),	/* 0 */
/* 62 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hBuilding */

/* 64 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 66 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 68 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Procedure IBuildingInfo_isNoLimit */

/* 70 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 72 */	NdrFcLong( 0x0 ),	/* 0 */
/* 76 */	NdrFcShort( 0x2 ),	/* 2 */
/* 78 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 80 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 82 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 84 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 86 */	NdrFcShort( 0x24 ),	/* 36 */
/* 88 */	NdrFcShort( 0x5 ),	/* 5 */
/* 90 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 92 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 94 */	NdrFcShort( 0x0 ),	/* 0 */
/* 96 */	NdrFcShort( 0x0 ),	/* 0 */
/* 98 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hBuilding */

/* 100 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 102 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 104 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Return value */

/* 106 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 108 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 110 */	0x3,		/* FC_SMALL */
			0x0,		/* 0 */

	/* Procedure IBuildingInfo_setNoLimit */

/* 112 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 114 */	NdrFcLong( 0x0 ),	/* 0 */
/* 118 */	NdrFcShort( 0x3 ),	/* 3 */
/* 120 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 122 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 124 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 126 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 128 */	NdrFcShort( 0x29 ),	/* 41 */
/* 130 */	NdrFcShort( 0x0 ),	/* 0 */
/* 132 */	0x40,		/* Oi2 Flags:  has ext, */
			0x2,		/* 2 */
/* 134 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 136 */	NdrFcShort( 0x0 ),	/* 0 */
/* 138 */	NdrFcShort( 0x0 ),	/* 0 */
/* 140 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter hBuilding */

/* 142 */	NdrFcShort( 0x8 ),	/* Flags:  in, */
/* 144 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 146 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter b */

/* 148 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 150 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 152 */	0x3,		/* FC_SMALL */
			0x0,		/* 0 */

			0x0
        }
    };

static const IBuildingInfo_MIDL_TYPE_FORMAT_STRING IBuildingInfo__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	0x30,		/* FC_BIND_CONTEXT */
			0x30,		/* Ctxt flags:  out, ret, */
/*  4 */	0x0,		/* 0 */
			0x0,		/* 0 */
/*  6 */	0x30,		/* FC_BIND_CONTEXT */
			0x41,		/* Ctxt flags:  in, can't be null */
/*  8 */	0x0,		/* 0 */
			0x0,		/* 0 */

			0x0
        }
    };

static const NDR_RUNDOWN RundownRoutines[] = 
    {
    hIBuildingInfo_t_rundown
    };


static const unsigned short IBuildingInfo_FormatStringOffsetTable[] =
    {
    0,
    34,
    70,
    112
    };


static const MIDL_STUB_DESC IBuildingInfo_StubDesc = 
    {
    (void *)& IBuildingInfo___RpcServerInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    0,
    RundownRoutines,
    0,
    0,
    0,
    IBuildingInfo__MIDL_TypeFormatString.Format,
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

static const RPC_DISPATCH_FUNCTION IBuildingInfo_table[] =
    {
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    0
    };
static const RPC_DISPATCH_TABLE IBuildingInfo_v1_0_DispatchTable = 
    {
    4,
    (RPC_DISPATCH_FUNCTION*)IBuildingInfo_table
    };

static const SERVER_ROUTINE IBuildingInfo_ServerRoutineTable[] = 
    {
    (SERVER_ROUTINE)IBuildingInfo_create,
    (SERVER_ROUTINE)IBuildingInfo_delete,
    (SERVER_ROUTINE)IBuildingInfo_isNoLimit,
    (SERVER_ROUTINE)IBuildingInfo_setNoLimit
    };

static const MIDL_SERVER_INFO IBuildingInfo_ServerInfo = 
    {
    &IBuildingInfo_StubDesc,
    IBuildingInfo_ServerRoutineTable,
    IBuildingInfo__MIDL_ProcFormatString.Format,
    IBuildingInfo_FormatStringOffsetTable,
    0,
    0,
    0,
    0};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_) */

