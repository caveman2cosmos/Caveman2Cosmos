#pragma once
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

#include "defines.h"

namespace FileManager {
	
	//
	// File Management Namespace.
	// Contains functions to interact with the .txt file and thus, CvGameCoreDLL.
	//

	auto createFile(std::string)				-> bool;
	auto isCivOnline()							-> bool;
	auto updateRpcData(std::string)				-> bool;
	auto updateRPC()							-> bool;
}

extern std::unique_ptr<S_RPCDATA> g_RPCData;

