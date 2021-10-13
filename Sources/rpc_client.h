#pragma once

#ifndef rpc_client_h__
#define rpc_client_h__

namespace rpc
{
	namespace client
	{
		void createServerProcess();
		void init();
		void FRpcAssert(RPC_STATUS status);
		void shutDownServer();
	}
}

#endif