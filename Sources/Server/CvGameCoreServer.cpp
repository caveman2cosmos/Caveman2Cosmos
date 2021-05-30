#include <Rpc.h>
#include <Rpcndr.h>
#include "../Interfaces/ITest.h"
//#include "../Interfaces/ITest_s.c"
#include <assert.h>

void RpcAssert(RPC_STATUS status)
{
	if (FAILED(status))
	{
		assert(false);
		exit(status);
	}
}

DWORD WINAPI ServerListen(LPVOID)
{
   return RpcServerListen(1, RPC_C_LISTEN_MAX_CALLS_DEFAULT, false);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	RPC_STATUS status;

	//status = RpcServerUseAllProtseqs(RPC_C_PROTSEQ_MAX_REQS_DEFAULT, NULL);
	status = RpcServerUseProtseqEp((RPC_WSTR)"ncalrpc", RPC_C_PROTSEQ_MAX_REQS_DEFAULT, (RPC_WSTR)"CvGameCoreServer", NULL);
	RpcAssert(status);

	//RPC_BINDING_VECTOR* bindingVector;
	//status = RpcServerInqBindings(&bindingVector);
	//RpcAssert(status);

	//status = RpcServerRegisterIfEx(ITest_v1_0_s_ifspec, NULL, NULL, RPC_IF_AUTOLISTEN, RPC_C_LISTEN_MAX_CALLS_DEFAULT, NULL);
	status = RpcServerRegisterIf(ITest_v1_0_s_ifspec, NULL, NULL);
	RpcAssert(status);

	//status = RpcEpRegister(ITest_v1_0_s_ifspec, bindingVector, NULL, NULL);
	//RpcAssert(status);
	
	//status = RpcBindingVectorFree(&bindingVector);
	//RpcAssert(status);

	RpcServerListen(1, RPC_C_LISTEN_MAX_CALLS_DEFAULT, false);

	MSG msg;
	while(GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	const HANDLE hThread = CreateThread(NULL, 0, ServerListen, NULL, 0, NULL);
	RpcAssert(GetLastError());

	const DWORD hr = WaitForSingleObjectEx(hThread, INFINITE, true);
	assert(SUCCEEDED(hr));

	DWORD exitCode = 0;
	GetExitCodeThread(hThread, &exitCode);
	RpcAssert(exitCode);
	CloseHandle(hThread);

	assert(false);
    return 0;
}

void* __RPC_USER midl_user_allocate(size_t size)
{
    return malloc(size);
}

void __RPC_USER midl_user_free(void* p)
{
    free(p);
}

void rpc_test()
{
	assert(false);
}
