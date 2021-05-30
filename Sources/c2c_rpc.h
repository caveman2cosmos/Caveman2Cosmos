#pragma once

#ifndef c2c_rpc_h__
#define c2c_rpc_h__

namespace rpc
{
	void init();

	void FRpcAssert(const char* szFunction, RPC_STATUS status);
}

#endif