#include "Interface/IServerUtils.h"
#include "Interface/IBuildingInfo.h"
#include "BuildingInfo.h"
#include <iostream>
#include <assert.h>

void FRpcAssert(RPC_STATUS status)
{
#ifdef _DEBUG
    if (FAILED(status))
    {
        assert(false);
        exit(status);
    }
#endif
}

void shutDownServer()
{
	FRpcAssert(
		RpcMgmtStopServerListening(IServerUtils_v1_0_s_ifspec)
	);
	exit(0);
}

void Output(const char* szOutput)
{
   std::cout << szOutput << std::endl;
}

RPC_STATUS CALLBACK SecurityCallback(RPC_IF_HANDLE, void*)
{
    return RPC_S_OK;
}

int main()
{
	FRpcAssert(
		RpcServerUseProtseqEp(
			reinterpret_cast<unsigned char*>("ncacn_ip_tcp"), // Use TCP/IP protocol.
			RPC_C_PROTSEQ_MAX_REQS_DEFAULT, // Backlog queue length for TCP/IP.
			reinterpret_cast<unsigned char*>("4747"), // TCP/IP port to use.
			NULL
		)
	);
	FRpcAssert(
		RpcServerRegisterIf2(
			IServerUtils_v1_0_s_ifspec,
			NULL, // Use the MIDL generated entry-point vector.
			NULL, // Use the MIDL generated entry-point vector.
			RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH, // Forces use of security callback.
			RPC_C_LISTEN_MAX_CALLS_DEFAULT, // Use default number of concurrent calls.
			(unsigned)-1, // Infinite max size of incoming data blocks.
			SecurityCallback
		)
	);
	FRpcAssert(
		RpcServerRegisterIf2(
			IBuildingInfo_v1_0_s_ifspec,
			NULL, // Use the MIDL generated entry-point vector.
			NULL, // Use the MIDL generated entry-point vector.
			RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH, // Forces use of security callback.
			RPC_C_LISTEN_MAX_CALLS_DEFAULT, // Use default number of concurrent calls.
			(unsigned)-1, // Infinite max size of incoming data blocks.
			SecurityCallback
		)
	);
	FRpcAssert(
		RpcServerListen(1, RPC_C_LISTEN_MAX_CALLS_DEFAULT, false)
	);
	exit(0);
}

void* __RPC_USER midl_user_allocate(size_t size)
{
    return malloc(size);
}

void __RPC_USER midl_user_free(void* p)
{
    free(p);
}