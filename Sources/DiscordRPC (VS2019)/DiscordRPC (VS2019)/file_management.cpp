#include "file_management.h"

#include <TlHelp32.h>
#include "discord.h"

std::unique_ptr<S_RPCDATA> g_RPCData = std::make_unique<S_RPCDATA>();

auto FileManager::createFile(std::string path) -> bool {
	// Creates the file if not present.

	//
	// TXT FILE SYNTAX:
	//
	//		separator: ?
	//		label: >
	//		values: on the right
	//		what?: on the left
	//
	//	EXAMPLE:
	//		myvalue>abc?myvalue2>123
	//

	std::ofstream config(path);
	if (!config.good()) return FALSE;

	config << OLABEL("start", "0") << OLABEL("state", "undefined") << OLABEL("details", "undefined") << OLABEL("igstate", "menu") << OLABEL("stop", "false") << FILEFOOTER;

	config.close();

	return TRUE;
}

auto FileManager::isCivOnline() -> bool {

    bool exists = false;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry))
        while (Process32Next(snapshot, &entry))
            if (!_wcsicmp(entry.szExeFile, L"Civ4BeyondSword.exe"))
                exists = true;

    CloseHandle(snapshot);
    return exists;
}

auto FileManager::updateRpcData(std::string path) -> bool {

	// Open our file.
	std::ifstream config(path);
	if (!config.good()) return FALSE;

	// File is good, let's read.
	std::string line; //it should be only one line.
	std::getline(config, line);

	std::string currentCommand = ""; // for storing data
	std::string currentValue = "";
	bool isValue = false;
	for (const auto& c : line) {
		// Loop through every character c

		if (c == '>') {
			// Value starts here

			isValue = true;
			continue;
		}

		if (c == '?') {
			// Separator hit, save the value

			if (strcmp(CSTR(currentCommand), "start") == 0)
				g_RPCData->start = stoi(currentValue);
			if (strcmp(CSTR(currentCommand), "state") == 0)
				g_RPCData->state = currentValue;
			if (strcmp(CSTR(currentCommand), "details") == 0)
				g_RPCData->details = currentValue;
			if (strcmp(CSTR(currentCommand), "igstate") == 0)
				g_RPCData->igstate = currentValue;
			if ((strcmp(CSTR(currentCommand), "stop") == 0))
				g_RPCData->stop = strcmp(CSTR(currentValue), "true") == 0 ? true : false;



			// Value no-no.
			isValue = false;

			// Reset values
			currentCommand = "";
			currentValue = "";

			continue;
		}

		if (isValue)
			currentValue += c;
		else
			currentCommand += c;
	}

	config.close();

	return TRUE;
}

auto FileManager::updateRPC() -> bool {
	DiscordRichPresence dp;

	memset(&dp, 0, sizeof(dp));
    std::string state, details;

	if (strcmp(CSTR(g_RPCData->igstate), "menu") == 0) {

		// States for Main Menu. Change them if you want.

		state = "In Main Menu";
        dp.state = CSTR(state);

        details = "In-Game";
        dp.details = CSTR(details);
    }
    else {

		// Unlike menu, State and Details for in-game are set in CvGameCoreDLL.

		state = g_RPCData->state;
        dp.state = CSTR(state);

		details = g_RPCData->details;
        dp.details = CSTR(details);
    }

	dp.startTimestamp = g_RPCData->start;

    dp.endTimestamp = 0;

    dp.largeImageKey = ICONKEY;
    dp.largeImageText = "";

    Discord_UpdatePresence(&dp);

	return TRUE;
}

