#include "CvGameCoreDLL.h"
#include "DoctrinePromotions.h"
#include "StackPromotions.h"
#include "CvUnit.h"
#include "CvCity.h"
#include "CvPlot.h"
#include "CvPlayerAI.h"
#include "CvInfos.h"
#include "CvBuildingInfo.h"
#include "CvGameAI.h"

namespace DoctrinePromotions
{
    namespace
    {
        const char* keys[COUNT] = {
            "FLAMING", "IMPERIAL", "ATTRITION", "COASTAL", "HOLYWAR", "CHIVALRY",
            "GREEKFIRE", "MARKSMAN", "MANOUEVER", "PATRIOTIC", "SEAMANSHIP",
            "ENGINEERCORPS", "TRENCH", "BLITZKRIEG", "PROJECTION", "GUERILLAWAR", "ARTILLERY"
        };
        const char prefix[] = "PROMOTION_DOCTRINE_";

        std::vector<int> promotionSchools, promotionRanks, buildingSchools;
        PromotionTypes promotionIds[COUNT][RANKS];
        BuildingTypes buildingIds[COUNT];
        enum Combat { RECON, ARCHER, THROWING, MELEE, MOUNTED, WOODEN_SHIPS,
            ARMOR_MEDIUM, ARMOR_HEAVY, WEAPON_DIST_SHORTBOW, WEAPON_DIST_LONGBOW,
            GUN, SIEGE, TRACKED, WHEELED, SIEGE_GUNPOWDER, NUM_COMBATS };
        const char* combatNames[NUM_COMBATS] = { "RECON", "ARCHER", "THROWING", "MELEE", "MOUNTED", "WOODEN_SHIPS",
            "ARMOR_MEDIUM", "ARMOR_HEAVY", "WEAPON_DIST_SHORTBOW", "WEAPON_DIST_LONGBOW",
            "GUN", "SIEGE", "TRACKED", "WHEELED", "SIEGE_GUNPOWDER" };
        UnitCombatTypes combatIds[NUM_COMBATS];
        int generalId = -1, admiralId = -1;

        PromotionTypes promotion(int s, int r)
        {
            return promotionIds[s][r];
        }
        BuildingTypes building(int s) { return buildingIds[s]; }
        bool primary(const CvUnit& unit, Combat kind)
        {
            return combatIds[kind] >= 0 && unit.getUnitCombatType() == combatIds[kind];
        }
        bool combat(const CvUnit& unit, Combat kind)
        {
            return combatIds[kind] >= 0 && unit.isHasUnitCombat(combatIds[kind]);
        }
        bool mountedArcher(const CvUnit& unit)
        {
            return primary(unit, MOUNTED)
                && (combat(unit, WEAPON_DIST_SHORTBOW)
                    || combat(unit, WEAPON_DIST_LONGBOW));
        }
        int selected(const CvUnit& unit)
        {
            for (PromotionTypes p = unit.getNextOwnedPromotion(NO_PROMOTION);
                p != NO_PROMOTION; p = unit.getNextOwnedPromotion(p))
            {
                const int s = school(p);
                if (s >= 0) return s;
            }
            return -1;
        }
        bool atSchool(const CvUnit& unit)
        {
            const CvPlot* plot = unit.plot();
            const CvCity* city = plot ? plot->getPlotCity() : NULL;
            return city && city->getOwner() == unit.getOwner() && !unit.isCargo();
        }
        bool known(const CvUnit& unit, int s)
        {
            const BuildingTypes b = building(s);
            return b != NO_BUILDING && GET_PLAYER(unit.getOwner()).getBuildingCount(b) > 0;
        }
    }

    bool enabled() { return GC.getGame().isOption(GAMEOPTION_COMBAT_MILITARY_DOCTRINES); }

    void processOwned(bool adding)
    {
        const BuildingTypes trench = building(12);
        for (int p = 0; p < MAX_PLAYERS; ++p)
        {
            CvPlayerAI& player = GET_PLAYER((PlayerTypes)p);
            if (!player.isAlive()) continue;
            if (trench != NO_BUILDING)
                foreach_(const CvCity* city, player.cities())
                    if (city->isActiveBuilding(trench))
                        player.changeCityDefenseModifier(GC.getBuildingInfo(trench).getAllCityDefenseModifier() * (adding ? 1 : -1));
            foreach_(CvUnit* unit, player.units())
            {
                for (PromotionTypes promo = unit->getNextOwnedPromotion(NO_PROMOTION);
                    promo != NO_PROMOTION; promo = unit->getNextOwnedPromotion(promo))
                    if (school(promo) >= 0) unit->processPromotion(promo, adding);
                if (adding && atSchool(*unit)) train(*unit);
            }
        }
    }

    // XML-derived identities only. Rebuild after every XML load, including
    // reloads with unchanged counts but different IDs. Never cache unit scores.
    void cache()
    {
        promotionSchools.assign(GC.getNumPromotionInfos(), -1);
        promotionRanks.assign(GC.getNumPromotionInfos(), -1);
        buildingSchools.assign(GC.getNumBuildingInfos(), -1);
        for (int s = 0; s < COUNT; ++s)
        {
            const CvString name = CvString("BUILDING_DOCTRINE_") + keys[s];
            buildingIds[s] = (BuildingTypes)GC.getInfoTypeForString(name.c_str());
            if (buildingIds[s] >= 0 && buildingIds[s] < (int)buildingSchools.size())
                buildingSchools[buildingIds[s]] = s;
            for (int r = 0; r < RANKS; ++r)
            {
                CvString name; name.Format("%s%s_%d", prefix, keys[s], r);
                const int p = GC.getInfoTypeForString(name.c_str());
                promotionIds[s][r] = (PromotionTypes)p;
                if (p >= 0 && p < (int)promotionSchools.size())
                {
                    promotionSchools[p] = s;
                    promotionRanks[p] = r;
                }
            }
        }
        for (int c = 0; c < NUM_COMBATS; ++c)
            combatIds[c] = (UnitCombatTypes)GC.getInfoTypeForString((CvString("UNITCOMBAT_") + combatNames[c]).c_str());
        generalId = GC.getInfoTypeForString("UNIT_GREAT_GENERAL");
        admiralId = GC.getInfoTypeForString("UNIT_GREAT_ADMIRAL");
    }

    int school(int p)
    {
        return p >= 0 && p < (int)promotionSchools.size() ? promotionSchools[p] : -1;
    }
    int rank(int p)
    {
        return p >= 0 && p < (int)promotionRanks.size() ? promotionRanks[p] : -1;
    }
    bool isBase(int p) { return rank(p) == 0; }
    bool isFoundation(int b)
    {
        return b >= 0 && b < (int)buildingSchools.size() && buildingSchools[b] >= 0;
    }

    bool eligible(const CvUnit& unit, int s)
    {
        if (!enabled() || s < 0 || s >= COUNT || unit.isTempUnit() || unit.isDead()
            || unit.isNPC() || unit.isAnimal() || !unit.canFight()) return false;
        const bool sea = s == 3 || s == 6 || s == 10 || s == 14;
        if (unit.getDomainType() != (sea ? DOMAIN_SEA : DOMAIN_LAND)) return false;
        switch (s)
        {
        case 0: return primary(unit, RECON) || primary(unit, ARCHER) || primary(unit, THROWING);
        case 1: case 4: return primary(unit, MELEE);
        case 2: return primary(unit, MOUNTED);
        case 3: case 6: case 10: return primary(unit, WOODEN_SHIPS);
        case 5: return primary(unit, MOUNTED) && !mountedArcher(unit)
            && (combat(unit, ARMOR_MEDIUM) || combat(unit, ARMOR_HEAVY));
        case 7: return primary(unit, ARCHER) || mountedArcher(unit);
        case 8: case 12: return primary(unit, GUN);
        case 9: return primary(unit, GUN) || primary(unit, MOUNTED);
        case 11: return primary(unit, SIEGE);
        case 13: return primary(unit, TRACKED) || primary(unit, WHEELED);
        case 14: return true; // Domain and ability to fight already checked.
        case 15: return primary(unit, GUN) || primary(unit, RECON);
        case 16: return primary(unit, SIEGE) && combat(unit, SIEGE_GUNPOWDER);
        }
        return false;
    }

    bool allows(const CvUnit& unit, int p, bool forFree, bool forStatus, bool battlefield)
    {
        const int s = school(p);
        if (s < 0) return true;
        const int r = rank(p);
        if (battlefield || r == 0 && !forFree && !forStatus || !eligible(unit, s)) return false;
        if (r == 0)
        {
            // Both the status command and automatic grants use the same rules.
            // The city check prevents training at an allied or occupied port.
            return unit.getExperience() >= unit.experienceNeeded()
                && atSchool(unit) && known(unit, s) && selected(unit) < 0;
        }
        // Do not let free/battlefield awards skip any paid rank or its base.
        return !forFree && !forStatus && selected(unit) == s
            && unit.isHasPromotion(promotion(s, 0))
            && unit.isHasPromotion(promotion(s, r - 1));
    }

    void train(CvUnit& unit)
    {
        if (unit.getExperience() < unit.experienceNeeded() || !enabled() || !unit.canFight() || unit.isNPC() || unit.isTempUnit() || !atSchool(unit) || selected(unit) >= 0) return;
        PromotionTypes choices[COUNT];
        int numChoices = 0;
        for (int s = 0; s < COUNT; ++s)
        {
            if (!known(unit, s)) continue;
            const PromotionTypes p = promotion(s, 0);
            if (p != NO_PROMOTION && unit.canAcquirePromotion(p, PromotionRequirements::ForStatus))
                choices[numChoices++] = p;
        }
        // No valuation for a forced choice; human choices stay in the action bar.
        if (numChoices == 0 || numChoices > 1 && unit.isHuman() && !unit.isAutoPromoting()) return;
        PromotionTypes best = choices[0];
        if (numChoices > 1)
        {
            StackPromotions::EvaluationScope aidEvaluation(unit.isHuman() || !StackPromotions::enabled());
            int bestValue = MIN_INT;
            for (int i = 0; i < numChoices; ++i)
            {
                int value = 0;
                const int s = school(choices[i]);
                for (int r = 0; r < RANKS; ++r)
                {
                    const PromotionTypes p = promotion(s, r);
                    if (p != NO_PROMOTION)
                        value += GET_PLAYER(unit.getOwner()).AI_promotionValue(p, unit.getUnitType(), &unit, unit.AI_getUnitAIType());
                }
                if (value > bestValue) { bestValue = value; best = choices[i]; }
            }
        }
        unit.setHasPromotion(best, true, PromotionApply::Free);
    }

    // A founding decision is rare. Count current, uncommitted recipients once
    // per candidate, before city/path searches; never score every rank per city.
    int foundingValue(const CvUnit& founder, int b)
    {
        if (!isFoundation(b) || founder.isCommander() || founder.isCommodore()) return 0;
        const int s = buildingSchools[b];
        const bool sea = s == 3 || s == 6 || s == 10 || s == 14;
        if (founder.getUnitType() != (sea ? admiralId : generalId)) return 0;
        int recipients = 0;
        foreach_(const CvUnit* unit, GET_PLAYER(founder.getOwner()).units())
            if (eligible(*unit, s) && selected(*unit) < 0 && ++recipients == 20) return 300;
        return recipients ? 100 + recipients * 10 : 0;
    }

    void schoolFounded(const CvUnit& founder)
    {
        // National access changed once. Only city garrisons can train here;
        // field units are handled by their next city-entry/readiness event.
        foreach_(CvUnit* unit, GET_PLAYER(founder.getOwner()).units())
            if (atSchool(*unit)) train(*unit);
    }

    bool canFoundAny(const CvUnit& founder)
    {
        if (!enabled() || founder.isCommander() || founder.isCommodore()) return false;
        const bool admiral = admiralId >= 0 && founder.getUnitType() == admiralId;
        const bool general = generalId >= 0 && founder.getUnitType() == generalId;
        if (!admiral && !general) return false;
        for (int s = 0; s < COUNT; ++s)
        {
            const bool sea = s == 3 || s == 6 || s == 10 || s == 14;
            const BuildingTypes b = building(s);
            if (sea == admiral && b != NO_BUILDING
                && GET_PLAYER(founder.getOwner()).canConstruct(b, false, false, true)) return true;
        }
        return false;
    }

    int barbarianCap(const CvUnit& unit, const CvUnit* victim, bool barbarian, int cap)
    {
        if (!enabled()) return cap;
        const PromotionTypes passage = promotion(0, 2);
        if (cap >= 0 && (barbarian || victim && victim->isHominid() && !victim->isAnimal())
            && passage != NO_PROMOTION && unit.isHasPromotion(passage)) return cap + 25;
        return cap; // Preserve unlimited XP and the separate animal limit.
    }
    int featureDamage(const CvUnit& unit, int feature, int damage)
    {
        if (!enabled() || damage <= 0 || feature == NO_FEATURE) return damage;
        if (strncmp(GC.getFeatureInfo((FeatureTypes)feature).getType(), "FEATURE_REEF", 12)) return damage;
        const PromotionTypes coastal = promotion(3, 3), seamanship = promotion(10, 2);
        return (coastal != NO_PROMOTION && unit.isHasPromotion(coastal)
            || seamanship != NO_PROMOTION && unit.isHasPromotion(seamanship)) ? 0 : damage;
    }
}
