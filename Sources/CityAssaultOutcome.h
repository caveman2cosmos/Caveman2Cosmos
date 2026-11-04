#pragma once

// Entry permission is not a capture outcome. Hidden/non-capturing raiders may
// enter an empty acts-as-city improvement without acquiring it in setXY.
// Keep attacks on detected troops/civilians and mixed capturing groups legal.
template<class Unit, class Plot>
bool cityAssaultHasOutcome(const Unit& unit, const Plot& target, bool animalOwner)
{
    if (target.getPlotCity() != NULL || !target.isCity(true)) return true;
    if (target.isVisiblePotentialEnemyDefender(&unit)
        || target.getNumVisiblePotentialEnemyDefenderless(&unit) > 0) return true;
    return !unit.isNoCapture() && !unit.isHiddenNationality()
        && !unit.isBlendIntoCity() && !unit.isCargo() && !animalOwner
        && !unit.canCoexistWithTeamOnPlot(target.getTeam(), target);
}
