#pragma once
#include <cstdlib>

// Diagnostic switches compare the same DLL. Ordinary games always use the
// exact prefilter and short, read-only resource snapshot; nothing is saved.
namespace TurnPreparation {
inline int mode() {
#ifdef C2C_RECALCULATED_BENCHMARK
    static const char* setting = std::getenv("C2C_TURN_PREPARATION");
    static const int value = setting ? std::atoi(setting) : 3;
    return value;
#else
    return 3;
#endif
}
inline bool trainingPrefilter() { return (mode() & 1) != 0; }
inline bool bulkRawVicinity() { return (mode() & 2) != 0; }
}
