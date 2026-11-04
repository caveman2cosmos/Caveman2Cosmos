#pragma once

class CvUnit;

// National schools are founded by consuming a Great General or Great Admiral.
// A soldier keeps one free foundation and buys its three ranks with normal XP.
namespace DoctrinePromotions
{
    enum { COUNT = 17, RANKS = 4 };
    bool enabled();
    void processOwned(bool adding);
    void cache();
    int foundingValue(const CvUnit& founder, int building);
    int school(int promotion);
    int rank(int promotion);
    bool isBase(int promotion);
    bool isFoundation(int building);
    bool eligible(const CvUnit& unit, int school);
    bool allows(const CvUnit& unit, int promotion, bool forFree, bool forStatus, bool battlefield);
    void train(CvUnit& unit);
    void schoolFounded(const CvUnit& founder);
    bool canFoundAny(const CvUnit& founder);
    int barbarianCap(const CvUnit& unit, const CvUnit* victim, bool barbarian, int cap);
    int featureDamage(const CvUnit& unit, int feature, int damage);
}
