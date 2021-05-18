

/* this ALWAYS GENERATED file contains the RPC server stubs */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 22:14:07 2038
 */
/* Compiler settings for ITest.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0622 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#include <string.h>
#include "ITest.h"

#define TYPE_FORMAT_STRING_SIZE   7                                 
#define PROC_FORMAT_STRING_SIZE   47                                
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _ITest_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } ITest_MIDL_TYPE_FORMAT_STRING;

typedef struct _ITest_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } ITest_MIDL_PROC_FORMAT_STRING;

typedef struct _ITest_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } ITest_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};

extern const ITest_MIDL_TYPE_FORMAT_STRING ITest__MIDL_TypeFormatString;
extern const ITest_MIDL_PROC_FORMAT_STRING ITest__MIDL_ProcFormatString;
extern const ITest_MIDL_EXPR_FORMAT_STRING ITest__MIDL_ExprFormatString;

/* Standard interface: ITest, ver. 1.0,
   GUID={0xba209999,0x0c6c,0x11d2,{0x97,0xcf,0x00,0xc0,0x4f,0x8e,0xea,0x45}} */


extern const MIDL_SERVER_INFO ITest_ServerInfo;

extern const RPC_DISPATCH_TABLE ITest_v1_0_DispatchTable;

static const RPC_SERVER_INTERFACE ITest___RpcServerInterface =
    {
    sizeof(RPC_SERVER_INTERFACE),
    {{0xba209999,0x0c6c,0x11d2,{0x97,0xcf,0x00,0xc0,0x4f,0x8e,0xea,0x45}},{1,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    (RPC_DISPATCH_TABLE*)&ITest_v1_0_DispatchTable,
    0,
    0,
    0,
    &ITest_ServerInfo,
    0x04000000
    };
RPC_IF_HANDLE ITest_v1_0_s_ifspec = (RPC_IF_HANDLE)& ITest___RpcServerInterface;

extern const MIDL_STUB_DESC ITest_StubDesc;


#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const ITest_MIDL_PROC_FORMAT_STRING ITest__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure test */

			0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 10 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 12 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 14 */	NdrFcShort( 0x8 ),	/* 8 */
/* 16 */	NdrFcShort( 0x22 ),	/* 34 */
/* 18 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 20 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */
/* 26 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter index */

/* 28 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 30 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 32 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter value */

/* 34 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 36 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 38 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 40 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 42 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 44 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

			0x0
        }
    };

static const ITest_MIDL_TYPE_FORMAT_STRING ITest__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/*  4 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */

			0x0
        }
    };

static const unsigned short ITest_FormatStringOffsetTable[] =
    {
    0
    };


static const MIDL_STUB_DESC ITest_StubDesc = 
    {
    (void *)& ITest___RpcServerInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    0,
    0,
    0,
    0,
    0,
    ITest__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
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

static const RPC_DISPATCH_FUNCTION ITest_table[] =
    {
    NdrServerCall2,
    0
    };
static const RPC_DISPATCH_TABLE ITest_v1_0_DispatchTable = 
    {
    1,
    (RPC_DISPATCH_FUNCTION*)ITest_table
    };

static const SERVER_ROUTINE ITest_ServerRoutineTable[] = 
    {
    (SERVER_ROUTINE)test
    };

static const MIDL_SERVER_INFO ITest_ServerInfo = 
    {
    &ITest_StubDesc,
    ITest_ServerRoutineTable,
    ITest__MIDL_ProcFormatString.Format,
    ITest_FormatStringOffsetTable,
    0,
    0,
    0,
    0};
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_) */

