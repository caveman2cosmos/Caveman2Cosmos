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
#include "CvArea.h"
#include <sstream>
#include "CvSelectionGroup.h"
#include "CvDLLUtilityIFaceBase.h"
#include <map>

namespace StackPromotions
{
    Effects::Effects() : count(0) { std::fill(stats, stats + NUM_STATS, 0); }

    namespace
    {
        static __declspec(thread) bool detailedEvaluation = true;
        static __declspec(thread) int mechanicsDepth = 0;
        static __declspec(thread) bool planningEvaluation = false;
        static __declspec(thread) unsigned long long planningQueries = 0;
        volatile LONG generation = 1;
        volatile LONG teamGeneration[MAX_TEAMS] = { 0 };
        struct Profile { PromotionTypes levels[3]; std::vector<UnitTypes> units; bool lightRecon; Profile() : lightRecon(false) { std::fill(levels, levels + 3, NO_PROMOTION); } };
        struct Pool
        {
            const CvPlot* plot;
            unsigned revision;
            const CvPlot* formationPlot;
            unsigned formationRevision;
            int formationCount[2];
            std::vector<UnitCombatTypes> formationClasses[2];
            int count[2];
            std::vector<long long> strength;
            Pool() : plot(NULL), revision(0), formationPlot(NULL), formationRevision(0) { count[0] = count[1] = formationCount[0] = formationCount[1] = 0; }
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
        // Immutable rule index shared by all units using this reader. The masks
        // only select possible providers; live exclusions still decide donation.
        struct ProviderIndex
        {
            std::vector<PromotionTypes> levels;
            std::vector<unsigned> classes, units;
            unsigned domains[2], recon, unconditional, carriers;
            size_t domainCounts[2];
            bool ready;
            ProviderIndex() : recon(0), unconditional(0), carriers(0), ready(false)
            { domains[0] = domains[1] = 0; domainCounts[0] = domainCounts[1] = 0; }
        };
        struct Runtime
        {
            unsigned generation;
            int combatant, navalCombatant, recon, explorer;
            std::vector<Profile> profiles;
            std::vector<unsigned char> versusTargets, featureTargets;
            std::vector<std::vector<PromotionTypes> > versusRules, featureRules;
            PromotionTypes crowded[4];
            std::vector<std::pair<TechTypes, int> > technologies;
            int teamLimits[MAX_TEAMS];
            unsigned teamVersions[MAX_TEAMS];
            std::map<int, Pool> pools;
            UnitResult results[512];
            unsigned long long queries, hits, poolBuilds, poolUnits, unitBuilds, mergeQueries, productionQueries, formationBuilds, formationUnits;
            double frequency, unitMs, poolMs, formationMs;
            // Keep every pre-existing hot field/result-table offset unchanged.
            // Rule metadata is cold compared with millions of exact cache reads.
            ProviderIndex providerIndex;
            unsigned long long providerChecks, providerDomainSlots, providerClassEntries;
            Runtime() : generation(0), combatant(-1), navalCombatant(-1), recon(-1), explorer(-1), queries(0), hits(0), poolBuilds(0),
                poolUnits(0), unitBuilds(0), mergeQueries(0), productionQueries(0), formationBuilds(0), formationUnits(0), frequency(0), unitMs(0), poolMs(0), formationMs(0), providerChecks(0), providerDomainSlots(0), providerClassEntries(0)
            {
                LARGE_INTEGER hz;
                if (QueryPerformanceFrequency(&hz)) frequency = double(hz.QuadPart);
                std::fill(crowded, crowded + 4, NO_PROMOTION);
                std::fill(teamLimits, teamLimits + MAX_TEAMS, 1);
                std::fill(teamVersions, teamVersions + MAX_TEAMS, ~0u);
            }
        };
        unsigned char domainMask(DomainTypes domain)
        {
            return domain == DOMAIN_LAND ? 1 : domain == DOMAIN_SEA ? 2 : 0;
        }

        void addTargetRule(std::vector<PromotionTypes>& rules, PromotionTypes rule)
        {
            if (std::find(rules.begin(), rules.end(), rule) == rules.end()) rules.push_back(rule);
        }

        // Tier effects use the first tier's recipient rule. Congestion has no
        // recipient rule: it can affect either military domain. Keep all possible
        // family winners, including negative modifiers and future XML entries.
        void recordTargets(Runtime& r, const CvPromotionInfo& info, PromotionTypes rule = NO_PROMOTION)
        {
            if (info.getStackSupportKind() == 0) return;
            const unsigned char domains = rule == NO_PROMOTION ? 3 : domainMask(GC.getPromotionInfo(rule).getStackSupportDomain());
            for (int i = 0; i < GC.getNumUnitCombatInfos(); ++i)
                if (info.getUnitCombatModifierPercent((UnitCombatTypes)i))
                {
                    r.versusTargets[i] |= domains;
                    addTargetRule(r.versusRules[i], rule);
                }
            for (int i = 0; i < GC.getNumFeatureInfos(); ++i)
                if (info.getFeatureAttackPercent((FeatureTypes)i))
                {
                    r.featureTargets[i] |= domains;
                    addTargetRule(r.featureRules[i], rule);
                }
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
                local->versusRules.resize(GC.getNumUnitCombatInfos());
                local->featureRules.resize(GC.getNumFeatureInfos());
                std::map<int, size_t> profileIds;
                for (int i = 0; i < GC.getNumPromotionInfos(); ++i)
                {
                    const CvPromotionInfo& info = GC.getPromotionInfo((PromotionTypes)i);
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
                    for (int tier = 0; tier < 3; ++tier)
                        if (p.levels[tier] != NO_PROMOTION)
                            recordTargets(*local, GC.getPromotionInfo(p.levels[tier]), p.levels[0]);
                }
                for (int tier = 0; tier < 4; ++tier)
                    if (local->crowded[tier] != NO_PROMOTION)
                        recordTargets(*local, GC.getPromotionInfo(local->crowded[tier]));
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
        bool providerRules(const CvUnit& u, const Profile& profile, const Runtime& r)
        {
            const CvPromotionInfo& p = GC.getPromotionInfo(profile.levels[0]);
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
        bool provider(const CvUnit& u, const Profile& profile, const Runtime& r)
        {
            const CvPromotionInfo& p = GC.getPromotionInfo(profile.levels[0]);
            if (!military(u, r) && !(profile.lightRecon && lightRecon(u, r))) return false;
            if (u.getDomainType() != p.getStackSupportDomain()) return false;
            return providerRules(u, profile, r);
        }

        bool prepareProviderIndex(Runtime& r)
        {
            // A future XML pack with more profiles retains the original generic
            // path. No shift by the word width, truncation or arbitrary Aid cap.
            if (r.profiles.size() > sizeof(unsigned) * 8) return false;
            ProviderIndex& index = r.providerIndex;
            bool matches = index.ready && index.levels.size() == r.profiles.size();
            for (size_t i = 0; matches && i < r.profiles.size(); ++i)
                matches = index.levels[i] == r.profiles[i].levels[0]
                    && ((index.recon & (1u << i)) != 0) == r.profiles[i].lightRecon;
            if (matches) return true;
            index.levels.resize(r.profiles.size());
            index.classes.assign(GC.getNumUnitCombatInfos(), 0);
            index.units.assign(GC.getNumUnitInfos(), 0);
            index.domains[0] = index.domains[1] = index.recon = index.unconditional = index.carriers = 0;
            index.domainCounts[0] = index.domainCounts[1] = 0;
            for (size_t i = 0; i < r.profiles.size(); ++i)
            {
                const Profile& profile = r.profiles[i];
                index.levels[i] = profile.levels[0];
                const unsigned bit = 1u << i;
                if (profile.lightRecon) index.recon |= bit;
                if (profile.levels[0] == NO_PROMOTION) continue;
                const CvPromotionInfo& rule = GC.getPromotionInfo(profile.levels[0]);
                const DomainTypes domain = rule.getStackSupportDomain();
                if (domain != DOMAIN_LAND && domain != DOMAIN_SEA) continue;
                const int d = domain == DOMAIN_SEA ? 1 : 0;
                index.domains[d] |= bit; ++index.domainCounts[d];
                const std::vector<UnitCombatTypes>& any = rule.getStackSupportProviders();
                for (size_t c = 0; c < any.size(); ++c)
                    if (any[c] >= 0 && (size_t)any[c] < index.classes.size()) index.classes[any[c]] |= bit;
                for (size_t t = 0; t < profile.units.size(); ++t)
                    if (profile.units[t] >= 0 && (size_t)profile.units[t] < index.units.size()) index.units[profile.units[t]] |= bit;
                if (any.empty() && !rule.getStackSupportProviderAll().empty()) index.unconditional |= bit;
                if (rule.isStackSupportCarrier()) index.carriers |= bit;
            }
            index.ready = true;
            return true;
        }

        unsigned providerCandidates(const CvUnit& u, const ProviderIndex& index, bool combatant, Runtime& r)
        {
            const unsigned eligible = index.domains[u.getDomainType() == DOMAIN_SEA ? 1 : 0] & (combatant ? ~0u : index.recon);
            if (!eligible) return 0;
            unsigned result = index.unconditional;
            const int type = u.getUnitType();
            if (type >= 0 && (size_t)type < index.units.size()) result |= index.units[type];
            if (u.getDomainCargo() == DOMAIN_AIR) result |= index.carriers;
            // Read existing sparse LIVE category membership once. Promotions may
            // change it; a static UnitInfo or primary-class-only lookup is wrong.
            const std::map<UnitCombatTypes, UnitCombatKeyedInfo>& classes = u.getUnitCombatKeyedInfo();
            for (std::map<UnitCombatTypes, UnitCombatKeyedInfo>::const_iterator it = classes.begin(); it != classes.end(); ++it)
            {
#ifdef C2C_RECALCULATED_BENCHMARK
                ++r.providerClassEntries;
#endif
                if (it->second.m_bHasUnitCombat && it->first >= 0 && (size_t)it->first < index.classes.size())
                    result |= index.classes[it->first];
            }
            return result & eligible;
        }
        bool recipient(const CvUnit& u, const CvPromotionInfo& p)
        {
            return u.getDomainType() == p.getStackSupportDomain()
                && (p.getStackSupportRecipients().empty() || anyCombat(u, p.getStackSupportRecipients()))
                && !anyCombat(u, p.getStackSupportRecipientNot())
                && (!p.isStackSupportAttackersOnly() || u.canAttack())
                && (!p.isStackSupportRecipientIntercept() || unaidedInterception(u) > 0);
        }

        bool relevantRecipient(const CvUnit& unit, unsigned char domains, const std::vector<PromotionTypes>& rules)
        {
            if (!(domains & domainMask(unit.getDomainType()))) return false;
            // Read live recipient eligibility. Promotions, attack ability and
            // unaided interception may change; no per-unit eligibility cache.
            for (size_t i = 0; i < rules.size(); ++i)
                if (rules[i] == NO_PROMOTION || recipient(unit, GC.getPromotionInfo(rules[i]))) return true;
            return false;
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
            result.count[0] = result.count[1] = 0;
            if (prepareProviderIndex(r))
            {
                foreach_(const CvUnit* u, plot.units())
                {
                    if (u->getTeam() != team) continue;
                    const bool combatant = military(*u, r);
                    if (!combatant && !lightRecon(*u, r)) continue;
                    ++r.poolUnits;
                    if (combatant) ++result.count[u->getDomainType() == DOMAIN_SEA ? 1 : 0];
                    const int strength = std::max(0, u->baseCombatStr());
                    if (!strength) continue;
                    unsigned candidates = providerCandidates(*u, r.providerIndex, combatant, r);
#ifdef C2C_RECALCULATED_BENCHMARK
                    r.providerDomainSlots += r.providerIndex.domainCounts[u->getDomainType() == DOMAIN_SEA ? 1 : 0];
#endif
                    for (size_t i = 0; candidates; ++i, candidates >>= 1)
                        if (candidates & 1u)
                        {
#ifdef C2C_RECALCULATED_BENCHMARK
                            ++r.providerChecks;
#endif
                            if (providerRules(*u, r.profiles[i], r)) result.strength[i] += strength;
                        }
                }
                result.plot = &plot;
                result.revision = plot.getStackSupportRevision();
                return result;
            }
            // XML/domain candidates are shared by all donors in this rebuild.
            // Keep live promotion, cargo and carrier predicates per donor.
            size_t smallIndices[2][32], counts[2] = { 0, 0 };
            size_t* indices[2] = { smallIndices[0], smallIndices[1] };
            std::vector<size_t> largeIndices[2];
            if (r.profiles.size() > 32)
                for (int d = 0; d < 2; ++d)
                {
                    largeIndices[d].resize(r.profiles.size());
                    indices[d] = &largeIndices[d][0];
                }
            for (size_t i = 0; i < r.profiles.size(); ++i)
                if (r.profiles[i].levels[0] != NO_PROMOTION)
                {
                    const DomainTypes domain = GC.getPromotionInfo(r.profiles[i].levels[0]).getStackSupportDomain();
                    if (domain == DOMAIN_LAND || domain == DOMAIN_SEA)
                    {
                        const int d = domain == DOMAIN_SEA ? 1 : 0;
                        indices[d][counts[d]++] = i;
                    }
                }
            foreach_(const CvUnit* u, plot.units())
            {
                if (u->getTeam() != team) continue;
                const bool combatant = military(*u, r);
                if (!combatant && !lightRecon(*u, r)) continue;
                ++r.poolUnits;
                if (combatant) ++result.count[u->getDomainType() == DOMAIN_SEA ? 1 : 0];
                const int strength = std::max(0, u->baseCombatStr());
                if (!strength) continue;
                const int d = u->getDomainType() == DOMAIN_SEA ? 1 : 0;
                for (size_t j = 0; j < counts[d]; ++j)
                {
                    const size_t i = indices[d][j];
                    if ((combatant || r.profiles[i].lightRecon) && providerRules(*u, r.profiles[i], r))
                        result.strength[i] += strength;
                }
            }
            result.plot = &plot;
            result.revision = plot.getStackSupportRevision();
            return result;
        }

        // Reuse the existing sparse plot/team entry. A coarse formation is
        // independent of the exact profile pool: no ratios, families or effects.
        Pool& formation(Runtime& r, const CvPlot& plot, TeamTypes team)
        {
            Pool& result = r.pools[GC.getMap().plotNum(plot.getX(), plot.getY()) * MAX_TEAMS + team];
            if (result.formationPlot == &plot && result.formationRevision == plot.getStackSupportRevision()) return result;
#ifdef C2C_RECALCULATED_BENCHMARK
            BuildTimer timer(r.formationMs, r.frequency);
#endif
            ++r.formationBuilds;
            result.formationCount[0] = result.formationCount[1] = 0;
            result.formationClasses[0].clear(); result.formationClasses[1].clear();
            foreach_(const CvUnit* u, plot.units())
            {
                if (u->getTeam() != team) continue;
                const bool combatant = military(*u, r);
                if (!combatant && !lightRecon(*u, r)) continue;
                ++r.formationUnits;
                const int domain = u->getDomainType() == DOMAIN_SEA ? 1 : 0;
                if (combatant) ++result.formationCount[domain];
                const UnitCombatTypes combat = u->getUnitCombatType();
                if (combat != NO_UNITCOMBAT && std::find(result.formationClasses[domain].begin(), result.formationClasses[domain].end(), combat) == result.formationClasses[domain].end())
                    result.formationClasses[domain].push_back(combat);
            }
            result.formationPlot = &plot;
            result.formationRevision = plot.getStackSupportRevision();
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
                // A nonexistent provider cannot grant any tier, regardless of
                // recipient classes. Avoid repeating that classification.
                if (totals.strength[i] <= 0) continue;
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

    EvaluationScope::EvaluationScope(bool exact) : previous(detailedEvaluation), previousPlanning(planningEvaluation)
    { detailedEvaluation = exact; planningEvaluation = planningEvaluation || !exact; }
    EvaluationScope::~EvaluationScope() { detailedEvaluation = previous; planningEvaluation = previousPlanning; }
    bool detailed()
    {
        return mechanicsDepth > 0 || (detailedEvaluation && !planningEvaluation);
    }
    bool detailedForCombat(bool concreteOpponent)
    {
        return mechanicsDepth > 0 || (!planningEvaluation && (detailedEvaluation || concreteOpponent));
    }



    MechanicsScope::MechanicsScope() { ++mechanicsDepth; }
    MechanicsScope::~MechanicsScope() { --mechanicsDepth; }





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
        if (!detailed()) { ++planningQueries; return empty; }
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

    int value(const CvUnit& unit, Stat stat)
    {
        // Planning needs a scalar zero, not an empty Effects object/cache read.
        if (!detailed()) { ++planningQueries; return 0; }
        return effects(unit).stats[stat];
    }
    bool has(const CvUnit& unit, PromotionTypes p)
    {
        if (p < 0 || p >= GC.getNumPromotionInfos() || !GC.getPromotionInfo(p).getStackSupportKind()) return false;
        const Effects& e = effects(unit);
        for (int i = 0; i < e.count; ++i) if (e.promotions[i] == p) return true;
        return false;
    }
    int versus(const CvUnit& unit, UnitCombatTypes combat)
    {
        if (!detailed()) { ++planningQueries; return 0; }
        if (!enabled()) return 0;
        const Runtime& r = runtime();
        if (combat < 0 || (size_t)combat >= r.versusTargets.size() || !r.versusTargets[combat]) return 0;
        if (!relevantRecipient(unit, r.versusTargets[combat], r.versusRules[combat])) return 0;
        const Effects& e = effects(unit); int total = 0;
        for (int i = 0; i < e.count; ++i) total += GC.getPromotionInfo(e.promotions[i]).getUnitCombatModifierPercent(combat);
        return total;
    }
    int featureAttack(const CvUnit& unit, FeatureTypes feature)
    {
        if (!detailed()) { ++planningQueries; return 0; }
        if (!enabled()) return 0;
        const Runtime& r = runtime();
        if (feature < 0 || (size_t)feature >= r.featureTargets.size() || !r.featureTargets[feature]) return 0;
        if (!relevantRecipient(unit, r.featureTargets[feature], r.featureRules[feature])) return 0;
        const Effects& e = effects(unit); int total = 0;
        for (int i = 0; i < e.count; ++i) total += GC.getPromotionInfo(e.promotions[i]).getFeatureAttackPercent(feature);
        return total;
    }
    int healingRate(const CvUnit& unit, const CvPlot* at, int otherwise)
    { return enabled() ? StackPromotionMath::healWithCongestion(otherwise, effects(unit, at).stats[HEAL]) : std::max(0, otherwise); }

    int mergePreference(const CvUnit& joining, const CvSelectionGroup& target, bool wholeGroup)
    {
        if (!enabled()) return 0;
        // Existing area posture is already maintained by strategic AI. Do not
        // encourage travel for diversity in a peaceful/neutral area, or run a
        // new danger scan just to decide whether this small preference applies.
        const AreaAITypes posture = joining.area()->getAreaAIType(joining.getTeam());
        if (posture == NO_AREAAI || posture == AREAAI_NEUTRAL) return 0;
        if (!eligible(joining) || !target.getHeadUnit() || target.getHeadUnit()->getTeam() != joining.getTeam()
            || target.getHeadUnit()->getDomainType() != joining.getDomainType() || !target.plot()) return 0;
        Runtime& r = runtime(); ++r.mergeQueries;
        if (joining.plot() == target.plot()) return 0; // reordering units never creates Aid
        const Pool& destination = formation(r, *target.plot(), joining.getTeam());
        const int domain = joining.getDomainType() == DOMAIN_SEA ? 1 : 0;
        int count = destination.formationCount[domain];
        if (wholeGroup)
        {
            foreach_(const CvUnit* u, joining.getGroup()->units())
                if (military(*u, r) && u->getDomainType() == joining.getDomainType()) ++count;
        }
        else ++count;
        const std::vector<UnitCombatTypes>& classes = destination.formationClasses[domain];
        const UnitCombatTypes combat = joining.getUnitCombatType();
        const int diversity = combat != NO_UNITCOMBAT && !classes.empty()
            && std::find(classes.begin(), classes.end(), combat) == classes.end() ? 5 : 0;
        const int tier = StackPromotionMath::overcrowdedTier(count, limit(joining.getTeam(), *target.plot(), joining.getDomainType()));
        // Soft preference only. Existing role, garrison and path rules decide
        // whether a merge is legal; no movement solely to lend a transient aura.
        return std::max(-30, diversity - tier * 10);
    }

    int productionPreference(const CvCity& city, UnitTypes type)
    {
        if (!enabled()) return 0;
        const AreaAITypes posture = city.area()->getAreaAIType(city.getTeam());
        if (posture == NO_AREAAI || posture == AREAAI_NEUTRAL) return 0;
        Runtime& r = runtime(); ++r.productionQueries;
        const CvUnitInfo& u = GC.getUnitInfo(type);
        const DomainTypes domain = (DomainTypes)u.getDomainType();
        const int militaryClass = domain == DOMAIN_LAND ? r.combatant : domain == DOMAIN_SEA ? r.navalCombatant : -1;
        const bool combatant = militaryClass >= 0 && u.hasUnitCombat((UnitCombatTypes)militaryClass);
        const bool recon = domain == DOMAIN_LAND && (r.recon >= 0 && u.hasUnitCombat((UnitCombatTypes)r.recon)
            || r.explorer >= 0 && u.hasUnitCombat((UnitCombatTypes)r.explorer));
        if (!combatant && !recon) return 0;
        const Pool& p = formation(r, *city.plot(), city.getTeam());
        const int d = domain == DOMAIN_SEA ? 1 : 0;
        if (p.formationCount[d] < 2 || p.formationCount[d] >= limit(city.getTeam(), *city.plot(), domain)) return 0;
        const UnitCombatTypes combat = (UnitCombatTypes)u.getUnitCombatType();
        // Encourage a small amount of local combined arms, without evaluating
        // hypothetical promotions for every buildable unit type.
        return combat != NO_UNITCOMBAT && std::find(p.formationClasses[d].begin(), p.formationClasses[d].end(), combat) == p.formationClasses[d].end() ? 5 : 0;
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
        const size_t indexBytes = sizeof(r.providerIndex) + r.providerIndex.levels.capacity() * sizeof(PromotionTypes)
            + (r.providerIndex.classes.capacity() + r.providerIndex.units.capacity()) * sizeof(unsigned);
        payload += indexBytes;
        payload += (r.versusRules.capacity() + r.featureRules.capacity()) * sizeof(std::vector<PromotionTypes>);
        for (size_t i = 0; i < r.versusRules.size(); ++i) payload += r.versusRules[i].capacity() * sizeof(PromotionTypes);
        for (size_t i = 0; i < r.featureRules.size(); ++i) payload += r.featureRules[i].capacity() * sizeof(PromotionTypes);
        for (std::map<int, Pool>::const_iterator i = r.pools.begin(); i != r.pools.end(); ++i)
            payload += sizeof(Pool) + i->second.strength.capacity() * sizeof(long long)
                + (i->second.formationClasses[0].capacity() + i->second.formationClasses[1].capacity()) * sizeof(UnitCombatTypes);
        CvString text = CvString::format("turn=%d queries=%I64u hits=%I64u poolBuilds=%I64u poolUnits=%I64u unitBuilds=%I64u mergeQueries=%I64u productionQueries=%I64u activePools=%u cachePayloadBytes=%u unitMs=%.3f poolMs=%.3f planningSkipped=%I64u formationBuilds=%I64u formationUnits=%I64u formationMs=%.3f categoryIndexBytes=%u providerChecks=%I64u providerDomainSlots=%I64u providerClassEntries=%I64u policy=always-fast",
            GC.getGame().getGameTurn(), r.queries, r.hits, r.poolBuilds, r.poolUnits, r.unitBuilds, r.mergeQueries,
            r.productionQueries, (unsigned)r.pools.size(), (unsigned)payload, r.unitMs, r.poolMs, planningQueries, r.formationBuilds, r.formationUnits, r.formationMs,
            (unsigned)indexBytes, r.providerChecks, r.providerDomainSlots, r.providerClassEntries);
        gDLL->logMsg("StackPromotions.log", text.c_str(), false, false);
        r.queries = r.hits = r.poolBuilds = r.poolUnits = r.unitBuilds = r.mergeQueries = r.productionQueries = 0;
        r.unitMs = r.poolMs = r.formationMs = 0;
        planningQueries = r.formationBuilds = r.formationUnits = 0;
        r.providerChecks = r.providerDomainSlots = r.providerClassEntries = 0;
    }
}
