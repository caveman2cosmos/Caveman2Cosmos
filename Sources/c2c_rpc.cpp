#include "CvGameCoreDLL.h"
#include "c2c_rpc.h"
#include "Interface//ITest.h"
#include "Interface//IServerUtils_c.c"
//#include <Rpc.h>
//#include <Rpcndr.h>

namespace rpc
{
	PROCESS_INFORMATION g_ServerProcInfo;

	void createServerProcess()
	{
		ZeroMemory(&g_ServerProcInfo, sizeof(g_ServerProcInfo));
		STARTUPINFOA startupInfo;
		ZeroMemory(&startupInfo, sizeof(startupInfo));
		startupInfo.cb = sizeof(startupInfo);

		const bool success = CreateProcessA(
			NULL, (LPSTR)(getModDir() + "\\Assets\\CvGameCoreServer.exe").c_str(), NULL, NULL, true,
			CREATE_NEW_PROCESS_GROUP, NULL, (getModDir() + "\\Assets").c_str(), &startupInfo, &g_ServerProcInfo
		);
		FAssert(success);
	}

	void init()
	{
		RPC_STATUS status;
/*
		//RPC_NS_HANDLE* ImportContext = NULL;
		//status = RPC_STATUS RpcNsBindingImportBeginA(RPC_C_NS_SYNTAX_DEFAULT, (RPC_CSTR)"CvGameCoreServerEP", ITest_v1_0_s_ifspec, rpcBindingVector, ImportContext);

		status = RpcBindingFromStringBinding((RPC_CSTR)"ncalrpc:[CvGameCoreServer]", &hITestBinding);
		FRpcAssert(status);
*/
	   unsigned char* szBinding = NULL;

		status = RpcStringBindingCompose(NULL,
		  reinterpret_cast<unsigned char*>("ncacn_ip_tcp"), // Use TCP/IP
		  reinterpret_cast<unsigned char*>("localhost"), // TCP/IP network
		  reinterpret_cast<unsigned char*>("4747"), // TCP/IP port to use.
		  NULL, &szBinding);

		FRpcAssert(status);

		status = RpcBindingFromStringBinding(szBinding, &hIServerUtilsBinding);
		FRpcAssert(status);

		RpcTryExcept
		{
			Output("Test");
		}
		RpcExcept(1)
		{
			std::cerr << "Runtime reported exception " << RpcExceptionCode() << std::endl;
		}
		RpcEndExcept

		status = RpcStringFree(&szBinding);
		FRpcAssert(status);

		status = RpcBindingFree(&hIServerUtilsBinding);
		FRpcAssert(status);
	}

	void FRpcAssert(RPC_STATUS status)
	{
//#ifdef _DEBUG
		if (FAILED(status))
		{
			void* pBuffer = NULL;
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK,
				NULL, status, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pBuffer, 0, NULL
			);
			FErrorMsg(str(bst::format("Rpc error: %s (%ud)") %(LPTSTR)pBuffer %status).c_str());
			LocalFree(pBuffer);
		}
//#endif
	}
}