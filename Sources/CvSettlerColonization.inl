bool CvUnitAI::AI_canPlanColony() const
{
    // isFound() includes the live Super Cottages restriction on Tribes and
    // Nomad Camps. A settler role alone does not make a city-founding unit.
    if (!isFound()) return false;
    const CvPlayerAI& player = GET_PLAYER(getOwner());
    if (player.getNumCities() == 0) return true;
    if (player.getCityLimit() > 0 && player.getNumCities() >= player.getCityLimit()) {
        if (player.getCityOverLimitUnhappy() <= 0
            || player.AI_getOverallHappyness(player.getCityOverLimitUnhappy()) < 0) return false;
    }
    return !player.AI_isFinancialTrouble();
}

bool CvUnitAI::AI_colonyShipCanReach(const CvUnit* ship, const CvPlot* target) const
{
    if (!ship || !target || !AI_canPlanColony() || !canFound(target)) return false;
    // A founding tile is land, so test its embarkation coast, not whether the
    // sea graph contains the land tile. Native ship paths still enforce law.
    foreach_(const CvPlot* coast, target->adjacent())
        if (coast->isWater() && UnitTravelConnectivity::seaUnitCanReach(ship, coast)) return true;
    return false;
}

bool CvUnitAI::AI_loadForColony(CvPlot* target)
{
    if (!target || !AI_canPlanColony() || !canFound(target)
        || target->getFoundValue(getOwner()) <= 0 || target->isVisibleEnemyUnit(this)) return false;
    const PlayerTypes owner = getOwner(); const int id = getID();
    // Both fields are already serialized: a city site is the strategic goal,
    // while MissionAIUnit is the particular transport reservation. A bare
    // failure to walk somewhere never becomes an overseas colonization plan.
    getGroup()->AI_setMissionAI(MISSIONAI_LOAD_SETTLER, target, NULL);
    const bool loaded = AI_load(UNITAI_SETTLER_SEA, MISSIONAI_LOAD_SETTLER, NO_UNITAI,
        -1, -1, -1, 0, MOVE_SAFE_TERRITORY | MOVE_NO_ENEMY_TERRITORY);
    CvUnit* alive = GET_PLAYER(owner).getUnit(id);
    if (!alive || alive->isDelayedDeath() || !alive->getGroup()) return true;
    CvUnit* ship = alive->getTransportUnit();
    if (!ship) ship = alive->getGroup()->AI_getMissionAIUnit();
    if (loaded) {
        alive->getGroup()->AI_setMissionAI(MISSIONAI_LOAD_SETTLER, target, ship);
        civilianDecisionTrace(*static_cast<CvUnitAI*>(alive), "COLONY_TRANSPORT", 4, target);
    } else {
        alive->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false,
            MISSIONAI_LOAD_SETTLER, target, NULL);
        civilianDecisionTrace(*static_cast<CvUnitAI*>(alive), "COLONY_WAIT_TRANSPORT", 4, target);
    }
    return true;
}

bool CvUnitAI::AI_continueOverseasSettlement()
{
    CvSelectionGroup* group = getGroup();
    if (isHuman() || isNPC() || isCargo() || group->AI_getMissionAIType() != MISSIONAI_LOAD_SETTLER) return false;
    CvPlot* target = group->AI_getMissionAIPlot();
    if (!target || !AI_canPlanColony() || !canFound(target)
        || target->getFoundValue(getOwner()) <= 0 || target->isVisibleEnemyUnit(this)
        || GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(target, MISSIONAI_FOUND, group) != 0
        || GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(target, MISSIONAI_LOAD_SETTLER, group) != 0) {
        group->AI_setMissionAI(NO_MISSIONAI, NULL, NULL);
        civilianDecisionTrace(*this, "COLONY_INVALIDATE", 4, target);
        return false;
    }
    if (UnitTravelConnectivity::landUnitCanReach(this, target)) {
        group->AI_setMissionAI(MISSIONAI_FOUND, target, NULL);
        return AI_continueFoundingMission();
    }
    return AI_loadForColony(target);
}

int CvUnitAI::AI_colonyPassengers(const CvPlot* port) const
{
    int count = 0;
    foreach_(const CvUnit* unit, port->units()) {
        if (unit->getOwner() != getOwner() || unit->isCargo() || !unit->isGroupHead()
            || unit->AI_getUnitAIType() != UNITAI_SETTLE) continue;
        const CvUnitAI* settler = static_cast<const CvUnitAI*>(unit);
        const CvSelectionGroup* group = unit->getGroup();
        if (group->AI_getMissionAIType() == MISSIONAI_LOAD_SETTLER
            && group->AI_getMissionAIUnit() == this
            && settler->AI_colonyShipCanReach(this, group->AI_getMissionAIPlot())) ++count;
    }
    return count;
}
