#include "CvGameCoreDLL.h"
#include "rpc_client.h"
#include "Interface/IBuildingInfo.h"
#include "Interface/IServerUtils_c.c"
//#include <Rpc.h>
//#include <Rpcndr.h>

handle_t hBuildingInfoBinding = NULL;

namespace rpc
{
	namespace client
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
			unsigned char* szBinding = NULL;

			FRpcAssert(
				RpcStringBindingCompose(
					NULL, // UUID to bind to.
					reinterpret_cast<unsigned char*>("ncacn_ip_tcp"), // Use TCP/IP
					reinterpret_cast<unsigned char*>("localhost"), // TCP/IP network
					reinterpret_cast<unsigned char*>("4747"), // TCP/IP port to use.
					NULL, // Protocol dependent network options to use.
					&szBinding
				)
			);

			FRpcAssert(
				RpcBindingFromStringBinding(szBinding, &hIServerUtilsBinding)
			);

			FRpcAssert(
				RpcStringBindingCompose(
					NULL, // UUID to bind to.
					reinterpret_cast<unsigned char*>("ncacn_ip_tcp"), // Use TCP/IP
					reinterpret_cast<unsigned char*>("localhost"), // TCP/IP network
					reinterpret_cast<unsigned char*>("4747"), // TCP/IP port to use.
					NULL, // Protocol dependent network options to use.
					&szBinding
				)

			FRpcAssert(
				RpcBindingFromStringBinding(szBinding, &hBuildingInfoBinding)
			);

			FRpcAssert(
				RpcEpResolveBinding(hBuildingInfoBinding, IBuildingInfo_v1_0_c_ifspec)
			);

			FRpcAssert(
				RpcStringFree(&szBinding)
			);
/*
			RPC_IF_HANDLE interfaceHandles[] = {
				IServerUtils_v1_0_c_ifspec,
				IBuildingInfo_v1_0_c_ifspec
			};

			foreach_(RPC_IF_HANDLE& hInterface, interfaceHandles)
			{
				FRpcAssert(
					RpcEpResolveBinding(hBinding, hInterface)
				);
			}

			FRpcAssert(
				RpcStringFree(&RpcBindingFree)
			);
*/
			Output("Test");
		}

		void FRpcAssert(RPC_STATUS status)
		{
#ifdef _DEBUG
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
#endif
		}
	}
}