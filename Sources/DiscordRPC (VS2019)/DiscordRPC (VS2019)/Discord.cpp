#include "discord.h"
#include <Windows.h>

#ifndef _DEBUG

Discord::Discord()
	: bInitialised(false),
	rich{}
{
}

Discord::~Discord()
{
	Shutdown();
}

HANDLE _out = NULL, _old_out = NULL;
HANDLE _err = NULL, _old_err = NULL;
HANDLE _in = NULL, _old_in = NULL;

void AttachConsole()
{
	_old_out = GetStdHandle(STD_OUTPUT_HANDLE);
	_old_err = GetStdHandle(STD_ERROR_HANDLE);
	_old_in = GetStdHandle(STD_INPUT_HANDLE);

	::AllocConsole() && ::AttachConsole(GetCurrentProcessId());

	_out = GetStdHandle(STD_OUTPUT_HANDLE);
	_err = GetStdHandle(STD_ERROR_HANDLE);
	_in = GetStdHandle(STD_INPUT_HANDLE);

	SetConsoleMode(_out,
		ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);

	SetConsoleMode(_in,
		ENABLE_INSERT_MODE | ENABLE_EXTENDED_FLAGS |
		ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE);
}

bool ConsolePrint(const char* fmt, ...)
{
	if (!_out)
		return false;

	char buf[1024];
	va_list va;

	va_start(va, fmt);
	_vsnprintf_s(buf, 1024, fmt, va);
	va_end(va);

	return !!WriteConsoleA(_out, buf, static_cast<DWORD>(strlen(buf)), nullptr, nullptr);
}

void handleDiscordReady(const DiscordUser* u) {
	ConsolePrint("\nDisplaying Presence for %s#%s\n", u->username, u->discriminator);
}

// handle discord disconnected event
void handleDiscordDisconnected(int errcode, const char* message) {
	ConsolePrint("\nDiscord: disconnected (%d: %s)\n", errcode, message);
}

// handle discord error event
void handleDiscordError(int errcode, const char* message) {
	ConsolePrint("\nDiscord: error (%d: %s)\n", errcode, message);
}

auto Discord::Initialise(std::string appId) -> void
{

	//AttachConsole();


	if (!bInitialised)
	{
		// Initialise handlers
		DiscordEventHandlers handlers;
		memset(&handlers, 0, sizeof(handlers));
		handlers.ready = handleDiscordReady;
		handlers.errored = handleDiscordError;
		handlers.disconnected = handleDiscordDisconnected;

		Discord_Initialize(appId.c_str(), &handlers, 1, nullptr);

		bInitialised = true;
	}
}

auto Discord::UpdatePresence(RichPresence& richPresence) -> bool
{
	if (bInitialised)
	{
		// Copy Rich Presence details from parameter struct to member struct
		rich.details = richPresence.details.c_str();
		rich.state = richPresence.state.c_str();
		rich.largeImageKey = richPresence.largeKey.c_str();
		rich.largeImageText = richPresence.largeText.c_str();
		rich.partyMax = richPresence.partyMax;
		rich.partySize = richPresence.partySize;
		rich.smallImageKey = richPresence.smallKey.c_str();
		rich.smallImageText = richPresence.smallText.c_str();
		rich.startTimestamp = richPresence.timeStart;
		rich.endTimestamp = richPresence.timeEnd;

		Discord_UpdatePresence(&rich);

		return true;
	}
	else
	{
		return false;
	}
}

auto Discord::Shutdown() -> void
{
	if (bInitialised)
	{
		// Clean up Discord
		Discord_Shutdown();

		bInitialised = false;
	}
}


#endif
// We do not have debug libraries for discord-rpc afaik