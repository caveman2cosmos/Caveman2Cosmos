#include <Windows.h>
#include <iostream>
#include <fstream>
#include <discord_rpc.h>
#include <filesystem>

#include "file_management.h"
#include "discord.h"


//	= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =  //
//						Discord RPC CIV4 Patched					 //
//								 < - >								 //
// A simple program taking file input and sending it as a Discord    //
// RPC Status.														 //
//																	 //
// The hackiest method of them all, explained why, where and when in //
// CvGameCoreDLL solution, in Discord.cpp.							 //
//																	 //
//														- Vax		 //
//  [ Global defines: ]												 //

#define CHECK_INTERVAL_MS	1000
#define FILEPATH			"discordstatus.txt"
#define WFILEPATH			L"discordstatus.txt"
#define APPID				"769207223632396318"

Discord DISCORD;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR pCmdLine, int nCmdShow) {

	//
	// Check for existing File
	if (!std::filesystem::exists(FILEPATH))
		if (!FileManager::createFile(FILEPATH))
			return SUCCEEDED(MessageBox(NULL, L"RPC Could not start! (file could not be created)", L"Error! :(", NULL));

	//
	// file Exists, let's start discord.
	DISCORD.Initialise(APPID);

	//
	// Discord started, let's get going!
	while (FileManager::isCivOnline() && !g_RPCData->stop) {

		FileManager::updateRpcData(FILEPATH);

		FileManager::updateRPC();

		Sleep(CHECK_INTERVAL_MS);
	}

	// Close discord RPC when Civ Exits or it's told to.
	DISCORD.~Discord();

	// Delete settings file
	DeleteFile(WFILEPATH);

	return TRUE;
}