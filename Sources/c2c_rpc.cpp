#include "CvGameCoreDLL.h"
#include "c2c_rpc.h"
#include "Interface//ITest_c.c"
#include "Interface//ITest.h"
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
		FRpcAssert("RpcBindingFromStringBinding()", status);
*/
	   unsigned char* szStringBinding = NULL;

	   // Creates a string binding handle.
	   // This function is nothing more than a printf.
	   // Connection is not done here.
	   status = RpcStringBindingCompose(
		  NULL, // UUID to bind to.
		  reinterpret_cast<unsigned char*>("ncacn_ip_tcp"), // Use TCP/IP
															// protocol.
		  reinterpret_cast<unsigned char*>("localhost"), // TCP/IP network
														 // address to use.
		  reinterpret_cast<unsigned char*>("4747"), // TCP/IP port to use.
		  NULL, // Protocol dependent network options to use.
		  &szStringBinding); // String binding output.

		FRpcAssert("RpcStringBindingCompose()", status);

	   // Validates the format of the string binding handle and converts
	   // it to a binding handle.
	   // Connection is not done here either.
	   status = RpcBindingFromStringBinding(
		  szStringBinding, // The string binding to validate.
		  &hITestBinding); // Put the result in the implicit binding
							  // handle defined in the IDL file.
		FRpcAssert("RpcBindingFromStringBinding()", status);

		rpc_test();
	}

	void FRpcAssert(const char* szFunction, RPC_STATUS status)
	{
//#ifdef _DEBUG
		if (FAILED(status))
		{
			void* pBuffer = NULL;
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK,
				NULL, status, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pBuffer, 0, NULL
			);
			FErrorMsg(str(bst::format("%s failed. %s (%ud)") %szFunction %(LPTSTR)pBuffer %status).c_str());
			LocalFree(pBuffer);
		}
//#endif
	}
}