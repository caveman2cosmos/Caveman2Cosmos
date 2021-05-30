// File Example1Server.cpp
#include <iostream>
#include "../Interface/Example1.h"
#include <assert.h>

// Server function.
void Output( 
   /* [string][in] */ const char* szOutput)
{
   std::cout << szOutput << std::endl;
}

// Naive security callback.
RPC_STATUS CALLBACK SecurityCallback(RPC_IF_HANDLE /*hInterface*/, void* /*pBindingHandle*/)
{
    return RPC_S_OK; // Always allow anyone.
}

int main()
{
   RPC_STATUS status;

   // Uses the protocol combined with the endpoint for receiving
   // remote procedure calls.
   status = RpcServerUseProtseqEp(
      reinterpret_cast<unsigned char*>("ncacn_ip_tcp"), // Use TCP/IP protocol.
      RPC_C_PROTSEQ_MAX_REQS_DEFAULT, // Backlog queue length for TCP/IP.
      reinterpret_cast<unsigned char*>("4747"), // TCP/IP port to use.
      NULL); // No security.

   if (status)
      exit(status);

   // Registers the Example1 interface.
   status = RpcServerRegisterIf2(
      Example1_v1_0_s_ifspec, // Interface to register.
      NULL, // Use the MIDL generated entry-point vector.
      NULL, // Use the MIDL generated entry-point vector.
      RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH, // Forces use of security callback.
      RPC_C_LISTEN_MAX_CALLS_DEFAULT, // Use default number of concurrent calls.
      (unsigned)-1, // Infinite max size of incoming data blocks.
      SecurityCallback); // Naive security callback.

   if (status)
      exit(status);

   // Start to listen for remote procedure calls for all registered interfaces.
   // This call will not return until RpcMgmtStopServerListening is called.
   status = RpcServerListen(
      1, // Recommended minimum number of threads.
      RPC_C_LISTEN_MAX_CALLS_DEFAULT, // Recommended maximum number of threads.
      FALSE); // Start listening now.
	assert(false);
   if (status)
      exit(status);
}

// Memory allocation function for RPC.
// The runtime uses these two functions for allocating/deallocating
// enough memory to pass the string to the server.
void* __RPC_USER midl_user_allocate(size_t size)
{
    return malloc(size);
}

// Memory deallocation function for RPC.
void __RPC_USER midl_user_free(void* p)
{
    free(p);
}
