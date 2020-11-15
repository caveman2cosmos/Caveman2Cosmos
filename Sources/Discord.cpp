#include <Windows.h>
#include <string.h>
#include "discord.h"
#include <sstream>
#include <fstream>

// for storing WD for re-booting
std::string discordWorkDirectory;


C_DiscordRPCCommunication::C_DiscordRPCCommunication(std::string dllDir) {
    //bootRPC(dllDir);
    // *softly* don't.

    m_bState = false;
    m_sRpcData.startTime = time(0);
    m_bBUGLoadedOptions = false;
}

C_DiscordRPCCommunication::~C_DiscordRPCCommunication() {
    exitRPC();
}

BOOL C_DiscordRPCCommunication::bootRPC(std::string dllDir) {
	// Boot the RPC
    std::string exe = dllDir + "Discord\\C2CRPCHost.exe";
    std::string workingDir = exe.substr(0, exe.length() - strlen("C2CRPCHost.exe"));

	// additional information
    STARTUPINFO si;     
    PROCESS_INFORMATION pi;

    // set the size of the structures
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    // start the RPC
    CreateProcess(NULL, (LPSTR)exe.c_str(), NULL, NULL, TRUE, 0, NULL, workingDir.c_str(), &si, &pi);

    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

    // Set defaults
    m_tLastUpdate = time(0);

    m_sRpcData.turn = -2; // init to -2 for main menu

    m_sDiscordRPCPath = workingDir;

    return TRUE;
}

BOOL C_DiscordRPCCommunication::exitRPC() {
	// Close the RPC

    // Write to file (if exists!) to exit.
    // If the file doesnt exist that means the RPC isnt running.

    std::fstream config;
    std::string dir = discordWorkDirectory + "Discord\\discordstatus.txt";
    config.open(CSTR(dir));

    if (config.fail()) return FALSE;

    config.close();

    std::ofstream oConfig(CSTR(dir), std::ofstream::out | std::ofstream::trunc);

	oConfig << OLABEL("start", "0") << OLABEL("state", "undefined") << OLABEL("details", "undefined") << OLABEL("igstate", "menu") << OLABEL("stop", "true") << FILEFOOTER;

    oConfig.close();

    return TRUE;
}

void C_DiscordRPCCommunication::updateRPC() {

    // This code is kinda weird but I'll explain it in a few lines.
    // Unfortunately, Discord RPC requires C++17 Standards and modern compilers, or dynamic linking
    // Which as far as I know and can, wont work without the EXE or some real bad hacks
    // That might trip Antiviruses on PCs.
    //
    // This code communicates with a separate program that sends the status to Discord.
    //
    //          -Vax.

    
    // We shall not update too often, let's check that enough time has passed since the last update
    if (std::difftime(time(0), m_tLastUpdate) < CHECKDELTA)
        return;

	m_tLastUpdate = time(0);

    // If RPC is off, don't update lol
    if (!m_bState)
        return;

    std::ofstream config;

    std::string configPath = m_sDiscordRPCPath + "discordstatus.txt";

    config.open(configPath.c_str(), std::ofstream::out | std::ofstream::trunc); // Truncate so we can write new data

    std::string gamestatus = m_sRpcData.turn == -2 || m_sRpcData.turn == 0 ? "menu" : "game"; // if turn == -2, that means we're in the main menu. -1 is reserved for undefined.
                                                                                              // I have NO CLUE why this is sometimes 0 in the main menu. I shouldn't be. idk. idk. idk.
    config << OLABEL("start", m_sRpcData.startTime) << OLABEL("state", m_sRpcData.playersAlive << " of " << m_sRpcData.playersTotal << " alive") \
           << OLABEL("details", m_sRpcData.date << " (Turn " << m_sRpcData.turn << ")") << OLABEL("igstate", gamestatus) << OLABEL("stop", "false") << FILEFOOTER;

    config.close();

	return;
}

BOOL C_DiscordRPCCommunication::sendNewRpcDetails(RPCDATA data) {

    if (data.turn != -1) m_sRpcData.turn = data.turn;

	if (data.playersAlive != -1) m_sRpcData.playersAlive = data.playersAlive;

	if (data.playersTotal != -1) m_sRpcData.playersTotal = data.playersTotal;

	if (data.startTime != -1) m_sRpcData.startTime = data.startTime;

	if (data.clientID != "") m_sRpcData.clientID = data.clientID;

	if (data.iconKey != "") m_sRpcData.iconKey = data.iconKey;

	if (data.date != "") m_sRpcData.date = data.date;

    updateRPC();

	// If we've 100% been at least once in-game (turn > 0)
    // BUG has loaded our stuffz, so turn it on
    if(m_sRpcData.turn > 0)
        m_bBUGLoadedOptions = true;

    return TRUE;
}

BOOL C_DiscordRPCCommunication::getCurrentState() {
    // Returns RPC state

    return m_bState;
}

void C_DiscordRPCCommunication::ensureDiscordRPCState() {
    //
    // Ensured Discord RPC is in the correct state (on/off, true/false)
    // Default state is OFF so I default to OFF in most cases.
    //

    static time_t lastCheck = time(0);

    if (std::difftime(time(0), lastCheck) < 1) return;

    lastCheck = time(0);

    // If BUG options are not loaded yet, check for FILE.
    if (!m_bBUGLoadedOptions) {
		std::fstream config;
        std::string dir = discordWorkDirectory + "Discord\\.disabled";
        config.open(CSTR(dir));

        bool state = true;

        if (!config.fail()) state = false;

        // If the file exists, disable RPC. (state false.)

		if (state && !m_bState) {
            bootRPC(discordWorkDirectory);
            m_bState = true;
        }
        else if (!state && m_bState) {
            exitRPC();
            m_bState = false;
        }

        return;
    }

    bool state = getBugOptionBOOL("MainInterface__EnableRP", false);

    if (state && !m_bState) {
        bootRPC(discordWorkDirectory);
        m_bState = true;

        // Delete .disabled file.

		std::string dir = discordWorkDirectory + "Discord\\.disabled";

        DeleteFile(CSTR(dir));
    }
    else if (!state && m_bState) {
        exitRPC();
        m_bState = false;

        // Create .disabled file.

        std::string dir = discordWorkDirectory + "Discord\\.disabled";

		std::ofstream disabledFile(CSTR(dir), std::ofstream::out | std::ofstream::trunc);

	    disabledFile << " ";

        disabledFile.close();
    }
}