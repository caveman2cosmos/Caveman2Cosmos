#include "CvGameCoreDLL.h"
#include "StackPromotions.h"
#include "StackPromotionMath.h"
#include "CvInfos.h"
#include "CvUnit.h"
#include "CvPlot.h"
#include "CvCity.h"
#include "CvTeam.h"
#include "CvTeamAI.h"
#include "CvGame.h"
#include "CvGameAI.h"
#include "CvMap.h"
#include <sstream>
#include "CvSelectionGroup.h"
#include "CvDLLUtilityIFaceBase.h"
#include <map>

namespace StackPromotions
{
    Effects::Effects() : count(0) { std::fill(stats, stats + NUM_STATS, 0); }

    namespace
    {
        volatile LONG generation = 1;
        volatile LONG teamGeneration[MAX_TEAMS] = { 0 };
        struct Profile { PromotionTypes levels[3]; std::vector<UnitTypes> units; bool lightRecon; Profile() : lightRecon(false) { std::fill(levels, levels + 3, NO_PROMOTION); } };
        struct Pool
        {
            const CvPlot* plot;
            unsigned revision;
            int count[2];
            std::vector<long long> strength;
            std::vector<int> receivers;
            Pool() : plot(NULL), revision(0) { count[0] = count[1] = 0; }
        };
        struct UnitResult
        {
            const CvUnit* unit;
            const CvPlot* plot;
            unsigned revision, sourceRevision, teamRevision;
            int owner, id;
            Effects result;
            UnitResult() : unit(NULL), plot(NULL), revision(0), sourceRevision(0), teamRevision(0), owner(-1), id(-1) {}
        };
        struct Runtime
        {
            unsigned generation;
            int combatant, navalCombatant, recon, explorer;
            std::vector<Profile> profiles;
            std::vector<unsigned char> versusTargets, featureTargets;
            PromotionTypes crowded[4];
            std::vector<std::pair<TechTypes, int> > technologies;
            int teamLimits[MAX_TEAMS];
            unsigned teamVersions[MAX_TEAMS];
            std::map<int, Pool> pools;
            UnitResult results[512];
            unsigned long long queries, hits, poolBuilds, poolUnits, unitBuilds, mergeQueries, productionQueries;
            double frequency, unitMs, poolMs;
            Runtime() : generation(0), combatant(-1), navalCombatant(-1), recon(-1), explorer(-1), queries(0), hits(0), poolBuilds(0),
                poolUnits(0), unitBuilds(0), mergeQueries(0), productionQueries(0), frequency(0), unitMs(0), poolMs(0)
            {
                LARGE_INTEGER hz;
                if (QueryPerformanceFrequency(&hz)) frequency = double(hz.QuadPart);
                std::fill(crowded, crowded + 4, NO_PROMOTION);
                std::fill(teamLimits, teamLimits + MAX_TEAMS, 1);
                std::fill(teamVersions, teamVersions + MAX_TEAMS, ~0u);
            }
        };
        // Conservative union across every derived promotion, including congestion
        // and future XML definitions. Irrelevant combat/feature queries need no
        // per-unit result lookup, plot pool or unit scan.
        void recordTargets(Runtime& r, const CvPromotionInfo& info)
        {
            if (info.getStackSupportKind() == 0) return;
            for (int i = 0; i < GC.getNumUnitCombatInfos(); ++i)
                if (info.getUnitCombatModifierPercent((UnitCombatTypes)i)) r.versusTargets[i] = 1;
            for (int i = 0; i < GC.getNumFeatureInfos(); ++i)
                if (info.getFeatureAttackPercent((FeatureTypes)i)) r.featureTargets[i] = 1;
        }

        // XML definitions are immutable in play; each reader owns its derived caches.
        // Match the engine's existing TLS mechanism and VC++ 2003 toolchain.
        static __declspec(thread) Runtime* local = NULL;

#ifdef C2C_RECALCULATED_BENCHMARK
        struct BuildTimer
        {
            LARGE_INTEGER start;
            double& value;
            double frequency;
            BuildTimer(double& ms, double hz) : value(ms), frequency(hz) { QueryPerformanceCounter(&start); }
            ~BuildTimer() { LARGE_INTEGER end; QueryPerformanceCounter(&end); if (frequency > 0) value += 1000.0 * double(end.QuadPart - start.QuadPart) / frequency; }
        };
#endif

        Runtime& runtime()
        {
            const unsigned epoch = generation;
            if (!local || local->generation != epoch)
            {
                delete local;
                local = new Runtime();
                local->generation = epoch;
                local->combatant = GC.getInfoTypeForString("UNITCOMBAT_COMBATANT", true);
                local->navalCombatant = GC.getInfoTypeForString("UNITCOMBAT_NAVAL_COMBATANT", true);
                local->recon = GC.getInfoTypeForString("UNITCOMBAT_RECON", true);
                local->explorer = GC.getInfoTypeForString("UNITCOMBAT_EXPLORER", true);
                local->versusTargets.assign(GC.getNumUnitCombatInfos(), 0);
                local->featureTargets.assign(GC.getNumFeatureInfos(), 0);
                std::map<int, size_t> profileIds;
                for (int i = 0; i < GC.getNumPromotionInfos(); ++i)
                {
                    const CvPromotionInfo& info = GC.getPromotionInfo((PromotionTypes)i);
                    recordTargets(*local, info);
                    const int tier = info.getStackSupportTier();
                    if (info.getStackSupportKind() == 2 && tier >= 1 && tier <= 4)
                        local->crowded[tier - 1] = (PromotionTypes)i;
                    if (info.getStackSupportKind() != 1 || tier < 1 || tier > 3) continue;
                    const int profile = info.getStackSupportProfile();
                    if (!profileIds.count(profile))
                    {
                        profileIds[profile] = local->profiles.size();
                        local->profiles.push_back(Profile());
                    }
                    local->profiles[profileIds[profile]].levels[tier - 1] = (PromotionTypes)i;
                }
                for (size_t i = 0; i < local->profiles.size(); ++i)
                {
                    Profile& p = local->profiles[i];
                    if (p.levels[0] == NO_PROMOTION) continue;
                    const CvPromotionInfo& rule = GC.getPromotionInfo(p.levels[0]);
                    const std::vector<UnitCombatTypes>& providers = rule.getStackSupportProviders();
                    p.lightRecon = rule.getStackSupportDomain() == DOMAIN_LAND
                        && (std::find(providers.begin(), providers.end(), (UnitCombatTypes)local->recon) != providers.end()
                            || std::find(providers.begin(), providers.end(), (UnitCombatTypes)local->explorer) != providers.end());
                    std::istringstream input(rule.getStackSupportProviderUnits());
                    std::string name;
                    while (input >> name)
                    {
                        const int id = GC.getInfoTypeForString(name.c_str(), true);
                        if (id >= 0 && id < GC.getNumUnitInfos()) p.units.push_back((UnitTypes)id);
                        else FAssertMsg(false, "Unknown StackSupportProviderUnits entry");
                    }
                }
                for (int i = 0; i < GC.getNumTechInfos(); ++i)
                    if (GC.getTechInfo((TechTypes)i).getStackSupportLimitChange() != 0)
                        local->technologies.push_back(std::make_pair((TechTypes)i, GC.getTechInfo((TechTypes)i).getStackSupportLimitChange()));
            }
            return *local;
        }

        bool military(const CvUnit& u, const Runtime& r)
        {
            if (u.isCargo() || u.isDead() || u.isDelayedDeath()) return false;
            return u.getDomainType() == DOMAIN_LAND
                ? r.combatant >= 0 && u.isHasUnitCombat((UnitCombatTypes)r.combatant)
                : u.getDomainType() == DOMAIN_SEA && r.navalCombatant >= 0 && u.isHasUnitCombat((UnitCombatTypes)r.navalCombatant);
        }

        // Scouts donate Recon aid, but are not counted or penalized as Combatants.
        bool lightRecon(const CvUnit& u, const Runtime& r)
        {
            return u.getDomainType() == DOMAIN_LAND && !u.isCargo() && !u.isDead() && !u.isDelayedDeath()
                && (r.recon >= 0 && u.isHasUnitCombat((UnitCombatTypes)r.recon)
                    || r.explorer >= 0 && u.isHasUnitCombat((UnitCombatTypes)r.explorer));
        }

        int unaidedInterception(const CvUnit& u)
        {
            // The ship needs its own interception ability (including promotions).
            // Neither borrowed AA aid nor commander auras may create a new interceptor.
            return u.getUnitInfo().getInterceptionProbability() + u.getExtraIntercept(true, true, true);
        }

        template<class Unit> bool anyCombat(const Unit& u, const std::vector<UnitCombatTypes>& list)
        {
            for (size_t i = 0; i < list.size(); ++i) if (u.isHasUnitCombat(list[i])) return true;
            return false;
        }
        template<class Unit> bool allCombat(const Unit& u, const std::vector<UnitCombatTypes>& list)
        {
            for (size_t i = 0; i < list.size(); ++i) if (!u.isHasUnitCombat(list[i])) return false;
            return true;
        }
        bool provider(const CvUnit& u, const Profile& profile, const Runtime& r)
        {
            const CvPromotionInfo& p = GC.getPromotionInfo(profile.levels[0]);
            if (!military(u, r) && !(profile.lightRecon && lightRecon(u, r))) return false;
            if (u.getDomainType() != p.getStackSupportDomain()) return false;
            const std::vector<UnitCombatTypes>& any = p.getStackSupportProviders();
            const std::vector<UnitCombatTypes>& all = p.getStackSupportProviderAll();
            const std::vector<UnitTypes>& classes = profile.units;
            bool classMatch = false;
            for (size_t i = 0; i < classes.size(); ++i) classMatch |= u.getUnitType() == classes[i];
            // C2C's existing carriers may be tagged TRANSPORT, not CARRIER.
            // Air cargo capability plus real non-suicide aircraft is authoritative.
            const bool carrierMatch = p.isStackSupportCarrier() && u.getDomainCargo() == DOMAIN_AIR;
            if (!classMatch && !carrierMatch && (any.empty() && all.empty() || !any.empty() && !anyCombat(u, any))) return false;
            if (!allCombat(u, all) || anyCombat(u, p.getStackSupportProviderNot())) return false;
            if (p.isStackSupportProviderDefensive() && !u.isOnlyDefensive()) return false;
            if (p.isStackSupportProviderIntercept() && unaidedInterception(u) <= 0) return false;
            if (p.isStackSupportCarrier())
            {
                if (!u.hasCargo()) return false;
                bool aircraft = false;
                foreach_(const CvUnit* cargo, u.plot()->units())
                    if (cargo->getTransportUnit() == &u && !cargo->isDead() && cargo->getDomainType() == DOMAIN_AIR
                        && cargo->airBaseCombatStr() > 0 && !cargo->isSuicide()) { aircraft = true; break; }
                if (!aircraft) return false;
            }
            return true;
        }
        bool recipient(const CvUnit& u, const CvPromotionInfo& p)
        {
            return u.getDomainType() == p.getStackSupportDomain()
                && (p.getStackSupportRecipients().empty() || anyCombat(u, p.getStackSupportRecipients()))
                && !anyCombat(u, p.getStackSupportRecipientNot())
                && (!p.isStackSupportAttackersOnly() || u.canAttack())
                && (!p.isStackSupportRecipientIntercept() || unaidedInterception(u) > 0);
        }

        void add(Effects& result, PromotionTypes id)
        {
            if (id == NO_PROMOTION || result.count >= 32) return;
            const CvPromotionInfo& p = GC.getPromotionInfo(id);
            result.promotions[result.count++] = id;
            result.stats[COMBAT] += p.getCombatPercent();
            result.stats[ATTACK] += p.getAttackCombatModifierChange();
            result.stats[DEFENSE] += p.getDefenseCombatModifierChange();
            result.stats[CITY_ATTACK] += p.getCityAttackPercent();
            result.stats[CITY_DEFENSE] += p.getCityDefensePercent();
            result.stats[FIRST_STRIKE_CHANCES] += p.getChanceFirstStrikesChange();
            result.stats[WITHDRAWAL] += p.getWithdrawalChange();
            result.stats[INTERCEPTION] += p.getInterceptChange();
            result.stats[HEAL] += p.getFriendlyHealChange();
        }

        Pool& pool(Runtime& r, const CvPlot& plot, TeamTypes team)
        {
            Pool& result = r.pools[GC.getMap().plotNum(plot.getX(), plot.getY()) * MAX_TEAMS + team];
            if (result.plot == &plot && result.revision == plot.getStackSupportRevision() && result.strength.size() == r.profiles.size()) return result;
            PROFILE("StackPromotions::BuildPool");
#ifdef C2C_RECALCULATED_BENCHMARK
            BuildTimer timer(r.poolMs, r.frequency);
#endif
            ++r.poolBuilds;
            result.strength.assign(r.profiles.size(), 0);
            result.receivers.assign(r.profiles.size(), 0);
            result.count[0] = result.count[1] = 0;
            foreach_(const CvUnit* u, plot.units())
            {
                if (u->getTeam() != team) continue;
                const bool combatant = military(*u, r);
                if (!combatant && !lightRecon(*u, r)) continue;
                ++r.poolUnits;
                if (combatant) ++result.count[u->getDomainType() == DOMAIN_SEA ? 1 : 0];
                const int strength = std::max(0, u->baseCombatStr());
                for (size_t i = 0; i < r.profiles.size(); ++i)
                    if (r.profiles[i].levels[0] != NO_PROMOTION)
                    {
                        if (provider(*u, r.profiles[i], r)) result.strength[i] += strength;
                        if (combatant && recipient(*u, GC.getPromotionInfo(r.profiles[i].levels[0]))) ++result.receivers[i];
                    }
            }
            result.plot = &plot;
            result.revision = plot.getStackSupportRevision();
            return result;
        }

        int weight(const CvPromotionInfo& p)
        {
            return p.getCombatPercent() + p.getAttackCombatModifierChange() + p.getDefenseCombatModifierChange()
                + p.getCityAttackPercent() + p.getChanceFirstStrikesChange() * 5 + p.getWithdrawalChange()
                + p.getInterceptChange() + p.getStackSupportWeight();
        }

        Effects calculate(Runtime& r, const CvUnit& u, const CvPlot& plot)
        {
            PROFILE("StackPromotions::BuildUnit");
#ifdef C2C_RECALCULATED_BENCHMARK
            BuildTimer timer(r.unitMs, r.frequency);
#endif
            ++r.unitBuilds;
            Effects result;
            Pool& totals = pool(r, plot, u.getTeam());
            // At most one promotion from each XML family, including cross-era replacements.
            // Small ordered buffer: one family per profile at most. Keep the
            // old ascending family order and first-wins ties, without allocating
            // a tree node for every active family of every evaluated unit.
            typedef std::pair<int, PromotionTypes> Family;
            Family smallFamilies[32];
            std::vector<Family> largeFamilies;
            Family* families = smallFamilies;
            if (r.profiles.size() > 32)
            {
                largeFamilies.resize(r.profiles.size());
                families = &largeFamilies[0];
            }
            size_t familyCount = 0;
            for (size_t i = 0; i < r.profiles.size(); ++i)
            {
                const Profile& profile = r.profiles[i];
                if (profile.levels[0] == NO_PROMOTION) continue;
                const CvPromotionInfo& rule = GC.getPromotionInfo(profile.levels[0]);
                if (!recipient(u, rule)) continue;
                long long strength = totals.strength[i];
                if (u.plot() == &plot && provider(u, profile, r)) strength -= std::max(0, u.baseCombatStr());
                const int tier = StackPromotionMath::aidTier(strength, u.baseCombatStr());
                if (!tier || profile.levels[tier - 1] == NO_PROMOTION) continue;
                PromotionTypes id = profile.levels[tier - 1];
                const CvPromotionInfo& promo = GC.getPromotionInfo(id);
                if (promo.getTechPrereq() != NO_TECH && !GET_TEAM(u.getTeam()).isHasTech(promo.getTechPrereq())) continue;
                const int family = rule.getStackSupportFamily();
                size_t found = 0;
                while (found < familyCount && families[found].first < family) ++found;
                if (found < familyCount && families[found].first == family)
                {
                    if (weight(promo) > weight(GC.getPromotionInfo(families[found].second))) families[found].second = id;
                }
                else
                {
                    for (size_t j = familyCount; j > found; --j) families[j] = families[j - 1];
                    families[found] = std::make_pair(family, id);
                    ++familyCount;
                }
            }
            for (size_t i = 0; i < familyCount; ++i) add(result, families[i].second);
            const int count = totals.count[u.getDomainType() == DOMAIN_SEA ? 1 : 0] + (u.plot() != &plot ? 1 : 0);
            const int tier = StackPromotionMath::overcrowdedTier(count, limit(u.getTeam(), plot, u.getDomainType()));
            if (tier) add(result, r.crowded[tier - 1]);
            return result;
        }
    }

    bool enabled() { return GC.getGame().isOption(GAMEOPTION_COMBAT_STACK_PROMOTIONS); }
    bool eligible(const CvUnit& unit) { return enabled() && military(unit, runtime()); }
    void reset() { InterlockedIncrement(&generation); }
    void releaseThreadCache() { delete local; local = NULL; }
    void invalidateTeam(TeamTypes team) { if (team >= 0 && team < MAX_TEAMS) InterlockedIncrement(&teamGeneration[team]); }

    int limit(TeamTypes team, const CvPlot& plot, DomainTypes)
    {
        Runtime& r = runtime();
        const unsigned version = teamGeneration[team];
        if (r.teamVersions[team] != version)
        {
            r.teamLimits[team] = 1;
            for (size_t i = 0; i < r.technologies.size(); ++i)
                if (GET_TEAM(team).isHasTech(r.technologies[i].first)) r.teamLimits[team] += r.technologies[i].second;
            r.teamVersions[team] = version;
        }
        return std::max(1, r.teamLimits[team]) + (plot.isCity() ? 2 : plot.isCity(true, team) ? 1 : 0);
    }

    const Effects& effects(const CvUnit& unit, const CvPlot* at)
    {
        static const Effects empty;
        if (!enabled() || !unit.plot()) return empty;
        Runtime& r = runtime();
        if (!military(unit, r)) return empty;
        if (!at) at = unit.plot();
        ++r.queries;
        const unsigned version = teamGeneration[unit.getTeam()];
        const unsigned slot = (unsigned(unit.getID()) * 31u + unsigned(unit.getOwner()) * 17u + unsigned(GC.getMap().plotNum(at->getX(), at->getY()))) & 511u;
        UnitResult& cache = r.results[slot];
        if (cache.unit == &unit && cache.owner == unit.getOwner() && cache.id == unit.getID() && cache.plot == at
            && cache.revision == at->getStackSupportRevision() && cache.sourceRevision == unit.plot()->getStackSupportRevision()
            && cache.teamRevision == version)
        { ++r.hits; return cache.result; }
        cache.result = calculate(r, unit, *at);
        cache.unit = &unit; cache.owner = unit.getOwner(); cache.id = unit.getID(); cache.plot = at;
        cache.revision = at->getStackSupportRevision(); cache.sourceRevision = unit.plot()->getStackSupportRevision(); cache.teamRevision = version;
        return cache.result;
    }

    int value(const CvUnit& unit, Stat stat) { return effects(unit).stats[stat]; }
    bool has(const CvUnit& unit, PromotionTypes p)
    {
        if (p < 0 || p >= GC.getNumPromotionInfos() || !GC.getPromotionInfo(p).getStackSupportKind()) return false;
        const Effects& e = effects(unit);
        for (int i = 0; i < e.count; ++i) if (e.promotions[i] == p) return true;
        return false;
    }
    int versus(const CvUnit& unit, UnitCombatTypes combat)
    {
        if (!enabled()) return 0;
        const Runtime& r = runtime();
        if (combat < 0 || (size_t)combat >= r.versusTargets.size() || !r.versusTargets[combat]) return 0;
        const Effects& e = effects(unit); int total = 0;
        for (int i = 0; i < e.count; ++i) total += GC.getPromotionInfo(e.promotions[i]).getUnitCombatModifierPercent(combat);
        return total;
    }
    int featureAttack(const CvUnit& unit, FeatureTypes feature)
    {
        if (!enabled()) return 0;
        const Runtime& r = runtime();
        if (feature < 0 || (size_t)feature >= r.featureTargets.size() || !r.featureTargets[feature]) return 0;
        const Effects& e = effects(unit); int total = 0;
        for (int i = 0; i < e.count; ++i) total += GC.getPromotionInfo(e.promotions[i]).getFeatureAttackPercent(feature);
        return total;
    }
    int healingRate(const CvUnit& unit, const CvPlot* at, int otherwise)
    { return enabled() ? StackPromotionMath::healWithCongestion(otherwise, effects(unit, at).stats[HEAL]) : std::max(0, otherwise); }

    int mergePreference(const CvUnit& joining, const CvSelectionGroup& target, bool wholeGroup)
    {
        if (!eligible(joining) || !target.getHeadUnit() || target.getHeadUnit()->getTeam() != joining.getTeam()) return 0;
        Runtime& r = runtime(); ++r.mergeQueries;
        if (joining.plot() == target.plot()) return 0; // same field: merely changing orders does not change aid
        const Effects current = effects(joining);
        const Effects there = effects(joining, target.plot());
        int benefit = there.stats[COMBAT] + there.stats[ATTACK] + there.stats[DEFENSE] + there.stats[CITY_ATTACK]
            + there.stats[FIRST_STRIKE_CHANCES] * 5 + there.stats[WITHDRAWAL]
            - current.stats[COMBAT] - current.stats[ATTACK] - current.stats[DEFENSE] - current.stats[CITY_ATTACK]
            - current.stats[FIRST_STRIKE_CHANCES] * 5 - current.stats[WITHDRAWAL];
        // Include the joining group's extra bodies. This is a penalty, never a hard size cap.
        int count = pool(r, *target.plot(), joining.getTeam()).count[joining.getDomainType() == DOMAIN_SEA ? 1 : 0];
        const int singleTier = StackPromotionMath::overcrowdedTier(count + 1, limit(joining.getTeam(), *target.plot(), joining.getDomainType()));
        if (wholeGroup)
        {
            foreach_(const CvUnit* u, joining.getGroup()->units())
                if (military(*u, r) && u->getDomainType() == joining.getDomainType()) ++count;
        }
        else ++count;
        const int tier = StackPromotionMath::overcrowdedTier(count, limit(joining.getTeam(), *target.plot(), joining.getDomainType()));
        const int oldPenalty = singleTier && r.crowded[singleTier - 1] != NO_PROMOTION ? GC.getPromotionInfo(r.crowded[singleTier - 1]).getCombatPercent() : 0;
        const int newPenalty = tier && r.crowded[tier - 1] != NO_PROMOTION ? GC.getPromotionInfo(r.crowded[tier - 1]).getCombatPercent() : 0;
        benefit += newPenalty - oldPenalty;
        // Reward a missing provider for actual recipients already waiting here.
        // The pool is shared with combat evaluation: no extra plot/unit scan.
        const Pool& destination = pool(r, *target.plot(), joining.getTeam());
        for (size_t i = 0; i < r.profiles.size(); ++i)
            if (!destination.strength[i] && destination.receivers[i] && provider(joining, r.profiles[i], r)
                && (GC.getPromotionInfo(r.profiles[i].levels[0]).getTechPrereq() == NO_TECH
                    || GET_TEAM(joining.getTeam()).isHasTech(GC.getPromotionInfo(r.profiles[i].levels[0]).getTechPrereq())))
            { benefit += 5; break; }
        return std::max(-30, std::min(30, benefit));
    }

    int productionPreference(const CvCity& city, UnitTypes type)
    {
        if (!enabled()) return 0;
        Runtime& r = runtime(); ++r.productionQueries;
        const CvUnitInfo& u = GC.getUnitInfo(type);
        const DomainTypes domain = (DomainTypes)u.getDomainType();
        const int militaryClass = domain == DOMAIN_LAND ? r.combatant : domain == DOMAIN_SEA ? r.navalCombatant : -1;
        const bool combatant = militaryClass >= 0 && u.hasUnitCombat((UnitCombatTypes)militaryClass);
        const bool recon = domain == DOMAIN_LAND && (r.recon >= 0 && u.hasUnitCombat((UnitCombatTypes)r.recon)
            || r.explorer >= 0 && u.hasUnitCombat((UnitCombatTypes)r.explorer));
        if (!combatant && !recon) return 0;
        Pool& p = pool(r, *city.plot(), city.getTeam());
        if (p.count[domain == DOMAIN_SEA ? 1 : 0] < 2) return 0;
        // A small local reinforcement preference, not an empire-wide army planner.
        for (size_t i = 0; i < r.profiles.size(); ++i)
        {
            if (p.strength[i] || !p.receivers[i] || r.profiles[i].levels[0] == NO_PROMOTION
                || !combatant && !r.profiles[i].lightRecon) continue;
            const CvPromotionInfo& rule = GC.getPromotionInfo(r.profiles[i].levels[0]);
            if (rule.getStackSupportDomain() != domain || rule.isStackSupportCarrier()) continue;
            if (rule.getTechPrereq() != NO_TECH && !GET_TEAM(city.getTeam()).isHasTech(rule.getTechPrereq())) continue;
            bool unitMatch = std::find(r.profiles[i].units.begin(), r.profiles[i].units.end(), type) != r.profiles[i].units.end();
            bool anyMatch = false, allMatch = true, excluded = false;
            const std::vector<UnitCombatTypes>& any = rule.getStackSupportProviders();
            const std::vector<UnitCombatTypes>& all = rule.getStackSupportProviderAll();
            const std::vector<UnitCombatTypes>& deny = rule.getStackSupportProviderNot();
            for (size_t j = 0; j < any.size(); ++j) anyMatch |= u.hasUnitCombat(any[j]);
            for (size_t j = 0; j < all.size(); ++j) allMatch &= u.hasUnitCombat(all[j]);
            for (size_t j = 0; j < deny.size(); ++j) excluded |= u.hasUnitCombat(deny[j]);
            if (!unitMatch && (any.empty() && all.empty() || !any.empty() && !anyMatch)) continue;
            if (!allMatch || excluded || rule.isStackSupportProviderDefensive() && !u.isOnlyDefensive()
                || rule.isStackSupportProviderIntercept() && u.getInterceptionProbability() <= 0) continue;
            return 5;
        }
        return 0;
    }
    int techValue(TechTypes tech) { return enabled() ? GC.getTechInfo(tech).getStackSupportLimitChange() * 10 : 0; }

    void writeStats()
    {
#ifndef C2C_RECALCULATED_BENCHMARK
        if (!getenv("C2C_STACK_SUPPORT_TRACE")) return;
#endif
        if (!enabled() || !local) return;
        Runtime& r = *local;
        size_t payload = sizeof(r.results) + r.versusTargets.capacity() + r.featureTargets.capacity();
        for (std::map<int, Pool>::const_iterator i = r.pools.begin(); i != r.pools.end(); ++i)
            payload += sizeof(Pool) + i->second.strength.capacity() * sizeof(long long) + i->second.receivers.capacity() * sizeof(int);
        CvString text = CvString::format("turn=%d queries=%I64u hits=%I64u poolBuilds=%I64u poolUnits=%I64u unitBuilds=%I64u mergeQueries=%I64u productionQueries=%I64u activePools=%u cachePayloadBytes=%u unitMs=%.3f poolMs=%.3f",
            GC.getGame().getGameTurn(), r.queries, r.hits, r.poolBuilds, r.poolUnits, r.unitBuilds, r.mergeQueries,
            r.productionQueries, (unsigned)r.pools.size(), (unsigned)payload, r.unitMs, r.poolMs);
        gDLL->logMsg("StackPromotions.log", text.c_str(), false, false);
        r.queries = r.hits = r.poolBuilds = r.poolUnits = r.unitBuilds = r.mergeQueries = r.productionQueries = 0;
        r.unitMs = r.poolMs = 0;
    }
}
