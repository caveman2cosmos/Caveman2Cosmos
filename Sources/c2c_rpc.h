#pragma once

#ifndef c2c_rpc_h__
#define c2c_rpc_h__

namespace rpc
{
	void createServerProcess();
	void init();
	void FRpcAssert(RPC_STATUS status);
}

#endif