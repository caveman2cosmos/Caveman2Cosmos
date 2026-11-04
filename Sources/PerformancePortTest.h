#pragma once

// Disposable game-test DLL only. Normal Release has no environment reads,
// changed save behavior, or additional Python API.
#ifdef C2C_PERFORMANCE_PORT_TEST
#include <cstdlib>
namespace PerformancePortTest {
inline bool active() {
    static const char* directory=std::getenv("C2C_AI_BENCH_OUTPUT");
    return directory && directory[0];
}
}
#endif
