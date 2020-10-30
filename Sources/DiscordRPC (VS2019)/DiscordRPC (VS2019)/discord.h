#pragma once
#define DISCORD_DISABLE_IO_THREAD
#include "discord_rpc.h"
#include <string>

struct RichPresence
{
	/// State.
	std::string details;	// What the player is currently doing.
	std::string state;		// In-game/party status.

	/// Timestamps.
	int64_t timeStart;		// Start timestamp in epoch seconds (used without end timestamp for elapsed time).
	int64_t timeEnd;		// End timestamp in epoch seconds (needs to be greater than start timestamp for remaining time).

	/// Party.
	int32_t partySize;		// Number of players in the party.
	int32_t partyMax;		// Maximum number of players allowed in the party.

	/// Icons.
	std::string smallKey;	// Small image key (used by Discord API to locate icon to use).
	std::string smallText;	// Tooltip text for the small image key.
	std::string largeKey;	// Large image key (used by Discord API to locate icon to use).
	std::string largeText;	// Tooltip text for the large image key.
};

// Discord RPC.
class Discord
{
public:
	Discord();

	~Discord();

	// Initialise Discord RPC.
	auto Initialise(std::string appId) -> void;

	// Send the Rich Presence update to Discord.
	// TO-DO: Handle errors.
	auto UpdatePresence(RichPresence& richPresence) -> bool;

protected:
	// Tell Discord that the Discord Rich Presence instance is shutting down.
	auto Shutdown() -> void;

	bool bInitialised;

	DiscordEventHandlers handlers;

	DiscordRichPresence rich;
};
