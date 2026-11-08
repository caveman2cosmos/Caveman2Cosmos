// Standalone property/equivalence tests for Sources/CvSafetyPruning.h
// (the admissible pruning bounds used by CvUnitAI::AI_safety / AI_safetyEval,
// introduced for turn-time optimization).
//
// These tests do NOT link against CvGameCoreDLL. They exercise the exact
// production header directly, mirroring the pre-pruning scoring and candidate
// acceptance semantics of CvUnitAI::AI_safetyEval as of ce5a6afeb so the
// pruning can be proven behavior-preserving by construction:
//
//   1. Scoring equivalence — CvSafetyPruning::calcFinalPlotScore is
//      bit-identical to the pre-pruning formula (including the truncating
//      integer division of the terrain-damage step) over randomized inputs.
//   2. Fast upper bound — calcFastUpperScore never underestimates the final
//      score of any candidate it prunes (stage 1 admissibility).
//   3. Admissibility sweep — end-to-end mirror of the three prune stages in
//      AI_safetyEval (1, 2a, 2b): a plot the old code would have accepted is
//      never pruned, over 4,000,000 randomized engine-feasible cases.
//   4. Deterministic edge tests — min-path-turn model per domain, exhausted
//      movement, at-plot, immobile, flat movement cost, negative modifiers,
//      terrain damage saturation, danger halving, best-value <= 0 gating.
//
// Build & run:
//   Linux:  g++ -O2 -std=c++11 -o tests CvSafetyPruningTests.cpp && ./tests
//   Windows: cl /EHsc /O2 CvSafetyPruningTests.cpp && CvSafetyPruningTests.exe
// Use --quick to shrink the randomized sweeps for interactive use.

#include "../../Sources/CvSafetyPruning.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace CvSafetyPruning;

// ---------------------------------------------------------------------------
// Reference implementation: pre-pruning CvUnitAI::AI_safetyEval scoring,
// transcribed from Sources/CvUnitAI.cpp at ce5a6afeb (base of the pruning
// PR). Intentionally independent of the pruning header so it can catch drift.
// ---------------------------------------------------------------------------
static int oldCodeScore(int defenders, bool bAnimal, bool bOwn, bool bNonHostile,
                        int defMod, bool bAtPlot, int roll,
                        int unitDamage, int terrainDamage, int pathTurns)
{
    int iValue = defenders * 100;
    if (bAnimal)
    {
        if (bOwn) { iValue += 75; }
        else if (bNonHostile) { iValue += 50; }
    }
    iValue += defMod;
    iValue += bAtPlot ? 50 : roll;
    if (terrainDamage > 0)
    {
        iValue *= std::max(1, 100 - unitDamage - terrainDamage);
        iValue /= 200;
    }
    iValue = iValue * 5 / (pathTurns + 4);
    return iValue;
}

// Deterministic PRNG (same stream on every platform: 32-bit LCG).
class Rng
{
public:
    explicit Rng(unsigned seed) : m_seed(seed) {}
    int next(int n)
    {
        m_seed = m_seed * 1103515245u + 12345u;
        return static_cast<int>((m_seed >> 8) % static_cast<unsigned>(n));
    }
    bool chance(int oneIn) { return next(oneIn) == 0; }
private:
    unsigned m_seed;
};

static long g_failures = 0;

static void fail(const char* what, const char* detail)
{
    if (g_failures < 8)
    {
        std::printf("FAIL %s: %s\n", what, detail);
    }
    ++g_failures;
}

// ---------------------------------------------------------------------------
// 1. Scoring equivalence
// ---------------------------------------------------------------------------
static void testScoringEquivalence(long iterations)
{
    Rng rng(555);
    long mismatches = 0;
    for (long t = 0; t < iterations; ++t)
    {
        int defenders = rng.next(8);
        bool animal = rng.chance(2);
        bool own = rng.chance(2);
        bool nonHostile = own ? false : rng.chance(2);
        int defMod = rng.next(400) - 150;           // allow negative defense modifiers
        bool atPlot = rng.chance(4);
        int dmg = rng.next(101), td = rng.next(60);
        int pt = rng.next(20), roll = rng.next(50);

        int det = calcDeterministicPlotBase(defenders, animal, own, nonHostile, defMod);
        int bonus = atPlot ? CURRENT_PLOT_STAY_BONUS : roll;
        int nw = calcFinalPlotScore(det, bonus, dmg, td, pt, 999999, false);
        int old = oldCodeScore(defenders, animal, own, nonHostile, defMod, atPlot, roll, dmg, td, pt);
        if (nw != old)
        {
            std::printf("MISMATCH def=%d ani=%d own=%d nh=%d dm=%d at=%d roll=%d dmg=%d td=%d pt=%d new=%d old=%d\n",
                        defenders, (int)animal, (int)own, (int)nonHostile, defMod, (int)atPlot,
                        roll, dmg, td, pt, nw, old);
            ++mismatches;
        }
    }
    if (mismatches)
    {
        fail("scoring equivalence", "calcFinalPlotScore diverged from pre-pruning formula");
    }
    std::printf("scoring equivalence: %ld cases, %ld mismatches\n", iterations, mismatches);
}

// ---------------------------------------------------------------------------
// 2. Fast upper bound sanity: upper >= any achievable final score
// ---------------------------------------------------------------------------
static void testFastUpperBound(long iterations)
{
    Rng rng(24601);
    long violations = 0;
    for (long t = 0; t < iterations; ++t)
    {
        int defenders = rng.next(6);
        bool animal = rng.chance(2);
        bool own = rng.chance(2);
        bool nonHostile = own ? false : rng.chance(2);
        int defMod = rng.next(300) - 100;
        bool atPlot = rng.chance(4);
        int dmg = rng.next(101), td = rng.next(60);
        int dom = rng.next(4);                    // 0 SEA 1 AIR 2 LAND 3 IMMOBILE (C2C order)
        bool flat = rng.chance(3);
        int base = 1 + rng.next(12);
        int ml = rng.chance(3) ? 0 : 1 + rng.next(12);
        int dist = atPlot ? 0 : 1 + rng.next(15);
        int roll = rng.next(50);

        UnitMovementProfile profile(base, ml, dom, flat);
        int det = calcDeterministicPlotBase(defenders, animal, own, nonHostile, defMod);

        // Sample an "actual" A* turn count that is engine-feasible: the model
        // guarantees minTurns <= actual turns, so draw actual above the floor.
        int minT = calcMinPossiblePathTurns(atPlot, dist, profile);
        int pt = minT + rng.next(15);

        // Score the same plot with any roll: the fast upper bound must never be
        // below the resulting pre-danger-halving value.
        int actual = oldCodeScore(defenders, animal, own, nonHostile, defMod, atPlot, roll, dmg, td, pt);
        int upper = calcFastUpperScore(det, atPlot, dist, profile);
        // calcFastUpperScore floors at 0 for non-positive pre-damage bases; a
        // negative achievable value is still "not prunable above best >= 0".
        if (actual > 0 && actual > upper)
        {
            ++violations;
        }
    }
    if (violations)
    {
        fail("fast upper bound", "calcFastUpperScore underestimated an achievable score");
    }
    std::printf("fast upper bound: %ld cases, %ld violations\n", iterations, violations);
}

// ---------------------------------------------------------------------------
// 3. End-to-end admissibility sweep mirroring AI_safetyEval prune stages
// ---------------------------------------------------------------------------
static void testAdmissibilitySweep(long iterations)
{
    Rng rng(12345);
    long pruned = 0, winners = 0, killed = 0, unsound = 0;
    for (long t = 0; t < iterations; ++t)
    {
        int defenders = rng.next(6);
        bool animal = rng.chance(2);
        bool own = rng.chance(2);
        bool nonHostile = own ? false : rng.chance(2);
        int defMod = rng.next(300) - 100;
        bool atPlot = rng.chance(4);
        int dmg = rng.next(101), td = rng.next(60);
        int dom = rng.next(4);
        bool flat = rng.chance(3);
        int base = 1 + rng.next(12);
        int ml = rng.chance(3) ? 0 : 1 + rng.next(12);
        int dist = atPlot ? 0 : 1 + rng.next(15);
        int pt = rng.next(15);                    // actual A* turns
        int roll = rng.next(50);
        bool danger = rng.chance(2);
        int best = rng.next(400);

        UnitMovementProfile profile(base, ml, dom, flat);
        int det = calcDeterministicPlotBase(defenders, animal, own, nonHostile, defMod);

        // Old-code acceptance (value > best, halved once if in danger):
        int actual = oldCodeScore(defenders, animal, own, nonHostile, defMod, atPlot, roll, dmg, td, pt);
        bool oldAccepts = false;
        if (actual > best)
        {
            if (danger) { actual /= 2; }
            if (actual > best) { oldAccepts = true; }
        }

        // New-code prune decisions (stages 1, 2a, 2b in AI_safetyEval order):
        bool prunes = false;
        if (best > 0 && calcFastUpperScore(det, atPlot, dist, profile) <= best)
        {
            prunes = true;                                    // stage 1
        }
        else if (td > 0 && shouldPruneBeforePath(det, atPlot, dmg, td, false, dist, profile, best))
        {
            prunes = true;                                    // stage 2a (terrain damage, no danger query)
        }
        else if (shouldPruneBeforePath(det, atPlot, dmg, td, true, dist, profile, best))
        {
            if (danger) { prunes = true; }                    // stage 2b (danger must actually be present)
        }

        if (prunes) { ++pruned; }
        if (oldAccepts) { ++winners; }
        if (prunes && oldAccepts)
        {
            ++killed;
            // A prune that removes an old-code winner is only sound if the
            // min-turn model was already lying (minTurns > actual turns).
            int minT = calcMinPossiblePathTurns(atPlot, dist, profile);
            if (minT <= pt)
            {
                if (unsound < 5)
                {
                    std::printf("UNSOUND def=%d ani=%d own=%d nh=%d dm=%d at=%d roll=%d dmg=%d td=%d "
                                "pt=%d dom=%d flat=%d base=%d ml=%d dist=%d danger=%d best=%d minT=%d\n",
                                defenders, (int)animal, (int)own, (int)nonHostile, defMod, (int)atPlot,
                                roll, dmg, td, pt, dom, (int)flat, base, ml, dist, (int)danger, best, minT);
                }
                ++unsound;
            }
        }
    }
    if (unsound)
    {
        fail("admissibility sweep", "pruning removed a plot the pre-pruning code would have chosen");
    }
    std::printf("admissibility sweep: %ld cases, pruned=%ld winners=%ld winners-killed=%ld unsound=%ld\n",
                iterations, pruned, winners, killed, unsound);
}

// ---------------------------------------------------------------------------
// 4. Deterministic edge tests
// ---------------------------------------------------------------------------
static void expectEq(const char* what, int got, int want)
{
    if (got != want)
    {
        std::printf("EDGE %s: got %d want %d\n", what, got, want);
        ++g_failures;
    }
}

static void expectTrue(const char* what, bool cond)
{
    if (!cond)
    {
        std::printf("EDGE %s: expected true\n", what);
        ++g_failures;
    }
}

static void expectFalse(const char* what, bool cond)
{
    if (cond)
    {
        std::printf("EDGE %s: expected false\n", what);
        ++g_failures;
    }
}

static void testEdgeCases()
{
    // min path turns: at plot is always 0
    expectEq("atPlot min turns", calcMinPossiblePathTurns(true, 12, UnitMovementProfile(2, 0, SAFETY_DOMAIN_LAND, false)), 0);

    // exhausted movement off-plot: >= 1 turn in every domain
    for (int d = 0; d < 4; ++d)
    {
        int mt = calcMinPossiblePathTurns(false, 5, UnitMovementProfile(4, 0, d, false));
        expectTrue("exhausted movement >= 1", mt >= 1);
    }

    // immobile: 1 turn floor, regardless of distance
    expectTrue("immobile >= 1", calcMinPossiblePathTurns(false, 99, UnitMovementProfile(4, 4, SAFETY_DOMAIN_IMMOBILE, false)) >= 1);

    // air domain: ceil((dist-1)/moves) lower bound
    expectEq("air 10 dist 4 moves", calcMinPossiblePathTurns(false, 10, UnitMovementProfile(4, 4, SAFETY_DOMAIN_AIR, false)), (10 - 1) / 4);

    // sea domain: at most double base moves per turn (no water routes)
    expectEq("sea 10 dist 4 moves", calcMinPossiblePathTurns(false, 10, UnitMovementProfile(4, 4, SAFETY_DOMAIN_SEA, false)), (10 - 1) / (2 * 4));

    // land domain without flat cost: routes can reach far tiles this turn -> 0
    expectEq("land route floor", calcMinPossiblePathTurns(false, 30, UnitMovementProfile(2, 2, SAFETY_DOMAIN_LAND, false)), 0);

    // flat movement cost: ignores domain route discounts
    expectEq("flat 10 dist 4 moves", calcMinPossiblePathTurns(false, 10, UnitMovementProfile(4, 4, SAFETY_DOMAIN_LAND, true)), (10 - 1) / 4);

    // base moves below 1 must not divide by zero or overshoot
    expectEq("zero base moves floor", calcMinPossiblePathTurns(false, 6, UnitMovementProfile(0, 3, SAFETY_DOMAIN_AIR, false)), (6 - 1) / 1);

    // shouldPruneBeforePath must be a no-op while no positive baseline exists
    expectFalse("no prune when best <= 0", shouldPruneBeforePath(0, false, 0, 0, false, 10, UnitMovementProfile(2, 2, SAFETY_DOMAIN_LAND, false), 0));
    expectFalse("no prune when best < 0", shouldPruneBeforePath(500, false, 0, 0, false, 10, UnitMovementProfile(2, 2, SAFETY_DOMAIN_LAND, false), -5));

    // non-positive pre-damage base cannot beat any positive best
    expectTrue("negative base prunes", shouldPruneBeforePath(-60, false, 0, 0, false, 1, UnitMovementProfile(2, 2, SAFETY_DOMAIN_LAND, false), 1));

    // terrain damage saturating health: score floor keeps sign via max(1, ...)
    expectEq("damage saturation floor", applyTerrainTurnDamage(200, 100, 60), 1);   // max(1, 100-100-60)=1 -> 200*1/200 = 1

    // danger halving only applies to values already above best
    expectEq("danger half above best", calcFinalPlotScore(100, 49, 0, 0, 0, 50, true), (100 + 49) * 5 / (0 + 4) / 2);
    expectEq("danger no-op below best", calcFinalPlotScore(10, 0, 0, 0, 30, 50, true), (10 * 5) / (30 + 4));

    // stage 2b floor: danger-aware bound for an immobile unit far away; the
    // post-halving upper score (100+49)*5/5/2 = 74 must prune above it.
    UnitMovementProfile slow(1, 1, SAFETY_DOMAIN_IMMOBILE, false);
    expectTrue("immobile far plot prunes at best=74", shouldPruneBeforePath(100, false, 0, 0, true, 100, slow, 74));
    expectFalse("immobile far plot not pruned at best=73", shouldPruneBeforePath(100, false, 0, 0, true, 100, slow, 73));
}

int main(int argc, char** argv)
{
    bool quick = false;
    for (int i = 1; i < argc; ++i)
    {
        if (0 == std::strcmp(argv[i], "--quick")) { quick = true; }
    }
    long eqIters  = quick ? 100000L : 1000000L;
    long ubIters  = quick ? 100000L : 1000000L;
    long adIters  = quick ? 1000000L : 4000000L;

    testEdgeCases();
    testScoringEquivalence(eqIters);
    testFastUpperBound(ubIters);
    testAdmissibilitySweep(adIters);

    if (g_failures)
    {
        std::printf("RESULT: %ld FAILURES\n", g_failures);
        return 1;
    }
    std::printf("RESULT: PASS\n");
    return 0;
}
