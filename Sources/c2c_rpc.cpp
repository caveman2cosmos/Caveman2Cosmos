#include "CvGameCoreDLL.h"
#include "c2c_rpc.h"
#include "Interfaces//ITest.h"
#include "Interfaces//ITest_c.c"
#include <Rpc.h>
#include <Rpcndr.h>

namespace rpc
{
	void init()
	{
		RPC_STATUS status;

		//RPC_NS_HANDLE* ImportContext = NULL;
		//status = RPC_STATUS RpcNsBindingImportBeginA(RPC_C_NS_SYNTAX_DEFAULT, (RPC_CSTR)"CvGameCoreServerEP", ITest_v1_0_s_ifspec, rpcBindingVector, ImportContext);

		status = RpcBindingFromStringBinding((RPC_CSTR)"ncalrpc:[CvGameCoreServer]", &hITestBinding);
		FRpcAssert("RpcBindingFromStringBinding()", status);

		rpc_test();
	}

	void FRpcAssert(const char* szFunction, RPC_STATUS status)
	{
		if (FAILED(status))
		{
//#ifdef _DEBUG
			void* pBuffer = NULL;
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK,
				NULL, status, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pBuffer, 0, NULL
			);
			FErrorMsg(str(bst::format("%s failed. %s (%ud)") %szFunction %(LPTSTR)pBuffer %status).c_str());
			LocalFree(pBuffer);
//#endif
		}
	}
}