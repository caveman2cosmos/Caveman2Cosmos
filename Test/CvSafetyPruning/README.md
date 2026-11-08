# CvSafetyPruning standalone tests

Property and equivalence tests for `Sources/CvSafetyPruning.h` — the admissible
upper-bound checks that prune candidate plots before A* pathfinding in
`CvUnitAI::AI_safety` / `AI_safetyEval`.

`CvSafetyPruning.h` is deliberately free of engine headers so these tests
compile against the *exact production logic* without CvGameCoreDLL. They
independently transcribe the pre-pruning scoring and acceptance semantics of
`AI_safetyEval` (as of base commit `ce5a6afeb`) and check, by construction:

1. **Scoring equivalence** — `calcFinalPlotScore` is bit-identical to the
   pre-pruning formula (including truncating integer semantics of the terrain
   damage and distance steps), 1,000,000 randomized cases.
2. **Fast upper bound soundness** — `calcFastUpperScore` never underestimates
   the achievable score of any candidate it would prune, 1,000,000 cases.
3. **End-to-end admissibility** — a mirror of the three prune stages used in
   `AI_safetyEval` (fast bound / terrain-damage bound / danger-aware bound):
   no plot the pre-pruning code would have *accepted* is ever pruned,
   4,000,000 randomized engine-feasible cases spanning all four C2C domains
   (SEA/AIR/LAND/IMMOBILE), exhausted movement, flat movement cost, negative
   defense modifiers, terrain damage saturation, and danger halving.
4. **Deterministic edge tests** — per-domain minimum-path-turn model,
   at-plot/stay-put, immobile floor, zero-base-moves safety, `iBestValue <= 0`
   gating, danger-halving application order, damage saturation floor.

## Build & run

Linux / MinGW:

```sh
g++ -O2 -std=c++11 -Wall -Wextra -o safety_tests CvSafetyPruningTests.cpp
./safety_tests          # full sweeps (~4.5M cases, well under a second at -O2)
./safety_tests --quick  # shrunk sweeps for interactive use
```

MSVC:

```
cl /EHsc /O2 CvSafetyPruningTests.cpp
CvSafetyPruningTests.exe
```

Exit code 0 and `RESULT: PASS` on success; the sweep prints per-stage counts
(`pruned`, `winners`, `winners-killed`, `unsound`) — `unsound` must be 0.
All randomness uses fixed 32-bit LCG seeds, so failures reproduce exactly.

These tests are not part of the CvGameCoreDLL build; run them manually (or in
CI) after touching `CvSafetyPruning.h` or the prune-stage call sites in
`AI_safetyEval`.
