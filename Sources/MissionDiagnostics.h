#pragma once

// Keep formatting and description lookups out of normal Release entirely.
// Define C2C_MISSION_DIAGNOSTICS to retain the detailed mission trace.
#if defined(_DEBUG) || defined(C2C_MISSION_DIAGNOSTICS)
#define C2C_MISSION_TRACE(message) OutputDebugString(message)
#else
#define C2C_MISSION_TRACE(message) ((void)0)
#endif
