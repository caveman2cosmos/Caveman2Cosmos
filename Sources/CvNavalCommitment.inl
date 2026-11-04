// Retain a selected strategic task; paths and tactical safety remain live.
void CvUnitAI::AI_rememberNavalCommitment(int kind, const CvPlot* target, int flags,
    MissionAITypes mission, const CvUnit* passenger)
{
    using namespace NavalCommitment;
    if (isHuman() || isNPC() || (getDomainType() != DOMAIN_SEA && kind != BOARD) || isCargo()
        || !target || !getGroup() || !isGroupHead()) return;
    bool hurt = false;
    foreach_(const CvUnit* member, getGroup()->units()) if (member->isHurt()) { hurt = true; break; }
    if (kind == REPAIR && !hurt) return;
    // Do not turn an already threatened refuge into a durable order. The
    // ordinary emergency retreat policy is still allowed to select it.
    if ((kind != BOARD && target->getOwner() != getOwner()) || target->isVisibleEnemyUnit(this)
        || GET_PLAYER(getOwner()).AI_getPlotDanger(target) > 0) return;
    int* state = m_aiNavalCommitment;
    state[KIND] = kind; state[X] = target->getX(); state[Y] = target->getY();
    state[MAP] = GC.getMap().getType(); state[ROLE] = AI_getUnitAIType();
    state[GROUP] = getGroupID(); state[MEMBERS] = getGroup()->getNumUnits();
    state[CARGO] = getGroup()->getCargo(); state[FLAGS] = flags;
    state[LAST_PLOT] = GC.getMap().plotNum(getX(), getY());
    state[PROGRESS_TURN] = GC.getGame().getGameTurn();
    state[TARGET_OWNER] = passenger ? passenger->getOwner() : -1;
    state[TARGET_UNIT] = passenger ? passenger->getID() : -1;
    state[TARGET_ROLE] = passenger ? passenger->AI_getUnitAIType() : NO_UNITAI;
    state[MISSION] = mission;
}

bool CvUnitAI::AI_continueNavalCommitment()
{
    using namespace NavalCommitment;
    int* state = m_aiNavalCommitment;
    if (state[KIND] == NONE) return false;
    if (state[KIND] == BOARD) return AI_continueNavalBoarding();
    CvSelectionGroup* group = getGroup();
    if (isHuman() || isNPC() || getDomainType() != DOMAIN_SEA || isCargo()
        || isDelayedDeath() || !group || !isGroupHead()
        || group->AI_getMissionAIType() != state[MISSION]
        || !compatible(state, GC.getMap().getType(), AI_getUnitAIType(),
            getGroupID(), group->getNumUnits(), group->getCargo())) {
        state[KIND] = NONE; return false;
    }
    const CvPlot* target = GC.getMap().plot(state[X], state[Y]);
    if (!target || target->getOwner() != getOwner()
        || !target->isCity(true)
        || !canEnterPlot(target, MoveCheck::IgnoreLocation)
        || target->isVisibleEnemyUnit(this)
        || GET_PLAYER(getOwner()).AI_getPlotDanger(target) > 0
        || exposedToDanger(plot(), 60, true)) {
        state[KIND] = NONE; return false;
    }
    if (state[KIND] == REPAIR) {
        bool hurt = false;
        foreach_(const CvUnit* member, group->units()) if (member->isHurt()) { hurt = true; break; }
        if (!hurt) { state[KIND] = NONE; return false; }
        if (atPlot(target)) {
            if (plot()->getTotalTurnDamage(group) <= 0
                && group->canStartMission(MISSION_HEAL, -1, -1)) {
                group->pushMission(MISSION_HEAL, -1, -1, 0, false, false, (MissionAITypes)state[MISSION], target); return true;
            }
            state[KIND] = NONE; return false;
        }
    } else if (state[KIND] == DELIVERY) {
        if (!AI_navalDeliveryStillUseful(target)) { state[KIND] = NONE; return false; }
        if (atPlot(target)) {
            // The ordinary ferry unloads only eligible workers with a real
            // job; preserve that logic and the accepted port on arrival.
            const bool unloaded = AI_settlerSeaFerry();
            state[KIND] = NONE; return unloaded;
        }
    } else if (state[KIND] == PICKUP) {
        if (!AI_navalPickupStillNeeded(target)) { state[KIND] = NONE; return false; }
        if (atPlot(target)) {
            // Passenger movement occurs in other groups' updates. Keep the
            // port reservation, but bound an unproductive wait below.
            if (stalled(state, GC.getMap().plotNum(getX(), getY()), GC.getGame().getGameTurn())) {
                state[KIND] = NONE; return false;
            }
            group->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_PICKUP, target);
            return true;
        }
    } else if (atPlot(target)) {
        state[KIND] = NONE; return false;
    }
    if (stalled(state, GC.getMap().plotNum(getX(), getY()), GC.getGame().getGameTurn())) {
        state[KIND] = NONE; return false;
    }
    if (group->isBusy()) return true;
    const MissionAITypes mission = (MissionAITypes)state[MISSION];
    if (!group->canAllMove()) {
        group->pushMission(MISSION_SKIP, -1, -1, 0, false, false, mission, target); return true;
    }
    int turns;
    if (!generatePath(target, state[FLAGS], true, &turns)) {
        group->pushMission(MISSION_SKIP, -1, -1, 0, false, false, mission, target); return true;
    }
    const CvPlot* end = getPathEndTurnPlot();
    if (!end || atPlot(end) || exposedToDanger(end, 60, true)) {
        state[KIND] = NONE; return false;
    }
    // End this order at the turn boundary so next turn validates the task
    // before moving again. The remembered target remains the final refuge.
    const PlayerTypes owner = getOwner(); const int id = getID();
    const bool moved = group->pushMissionInternal(MISSION_MOVE_TO, end->getX(), end->getY(),
        state[FLAGS], false, false, mission, target);
    // Moving may kill a unit or replace its group. Resolve it again first.
    CvUnit* alive = GET_PLAYER(owner).getUnit(id);
    if (!moved && alive && !alive->isDelayedDeath() && alive->getGroup() && !alive->getGroup()->isBusy())
        alive->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, mission, target);
    return true;
}

bool CvUnitAI::AI_navalDeliveryStillUseful(const CvPlot* target)
{
    if (AI_getUnitAIType() != UNITAI_SETTLER_SEA || !hasCargo()
        || getUnitAICargo(UNITAI_SETTLE) > 0 || !canCargoAllMove()) return false;
    const CvCity* city = target->getPlotCity();
    if (!city) return false;
    foreach_(const CvUnit* cargo, plot()->units())
        if (cargo->getOwner() == getOwner() && cargo->getTransportUnit() == this
            && cargo->AI_getUnitAIType() == UNITAI_WORKER
            && static_cast<const CvUnitAI*>(cargo)->AI_workerFerryJob(city)) return true;
    return false;
}

bool CvUnitAI::AI_navalPickupStillNeeded(const CvPlot* target)
{
    using namespace NavalCommitment;
    CvCity* city = target->getPlotCity();
    if (!city || city->AI_isDanger() || getGroup()->AI_isFull()) return false;
    if (GC.getGame().getGameTurn() - city->getGameTurnAcquired() <= 15
        && GET_TEAM(getTeam()).countEnemyPowerByArea(city->area()) != 0) return false;
    const UnitAITypes role = (UnitAITypes)m_aiNavalCommitment[TARGET_OWNER];
    if (role == NO_UNITAI) return false;
    const AreaAITypes areaAI = city->area()->getAreaAIType(getTeam());
    if (AI_getUnitAIType() == UNITAI_ASSAULT_SEA && areaAI == AREAAI_DEFENSIVE) return false;
    const bool requireOrder = requiresExplicitAssaultPickup(AI_getUnitAIType(), role, areaAI);
    const bool includeNew = m_aiNavalCommitment[TARGET_UNIT] != 0;
    int available = target->plotCount(PUF_isAvailableAssaultPickup, role, requireOrder, NULL,
        getOwner(), NO_TEAM, includeNew ? PUF_isFiniteRange : PUF_isFiniteRangeAndNotJustProduced);
    if (role == UNITAI_WORKER && AI_getUnitAIType() == UNITAI_SETTLER_SEA)
        available = orderedWorkerFerryPassengers(target, this, includeNew);
    if (role == UNITAI_SETTLE && AI_getUnitAIType() == UNITAI_SETTLER_SEA)
        available = AI_colonyPassengers(target);
    if (available <= 0) return false; // Do not wait indefinitely for speculative production.
    if (AI_getUnitAIType() == UNITAI_ASSAULT_SEA) {
        if (role == UNITAI_ATTACK_CITY && !city->AI_isDanger()) {
            if (target->plotCount(PUF_canDefend, -1, -1, NULL, getOwner(), NO_TEAM,
                PUF_isDomainType, DOMAIN_LAND) <= city->AI_neededDefenders()) return false;
        } else if (!city->AI_isDefended((atPlot(target) ? -1 : 1)
            * GET_PLAYER(getOwner()).strengthOfBestUnitAI(DOMAIN_LAND, UNITAI_CITY_DEFENSE))) return false;
    }
    if (AI_getUnitAIType() == UNITAI_SETTLER_SEA && role == UNITAI_CITY_DEFENSE
        && target->plotCount(PUF_canDefendGroupHead, -1, -1, NULL, getOwner(), NO_TEAM, PUF_isCityAIType) <= 1) return false;
    // Existing reservations remain authoritative; don't overbook a port.
    const int space = std::max(1, getGroup()->getCargoSpace());
    if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
        return GET_PLAYER(getOwner()).AI_plotTargetMissionAIsinCargoVolume(target, MISSIONAI_PICKUP, getGroup())
            < (available * 100 + space - 100) / space;
    return GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(target, MISSIONAI_PICKUP, getGroup())
        < (available + space - 1) / space;
}

bool CvUnitAI::AI_continueNavalBoarding()
{
    using namespace NavalCommitment;
    int* state = m_aiNavalCommitment;
    CvSelectionGroup* group = getGroup();
    const CvPlot* colony = group && AI_getUnitAIType() == UNITAI_SETTLE
        && state[MISSION] == MISSIONAI_LOAD_SETTLER ? group->AI_getMissionAIPlot() : NULL;
    CvUnit* ship = state[TARGET_OWNER] < 0 ? NULL : GET_PLAYER((PlayerTypes)state[TARGET_OWNER]).getUnit(state[TARGET_UNIT]);
    if (isHuman() || isNPC() || isCargo() || isDelayedDeath() || !group || !isGroupHead()
        || (AI_getUnitAIType() == UNITAI_SETTLE && state[MISSION] == MISSIONAI_LOAD_SETTLER && !AI_canPlanColony())
        || (colony && (!canFound(colony) || colony->isVisibleEnemyUnit(this)))
        || !compatible(state, GC.getMap().getType(), AI_getUnitAIType(), getGroupID(), group->getNumUnits(), group->getCargo())
        || !ship || ship->isDelayedDeath() || ship->getOwner() != getOwner() || ship->getDomainType() != DOMAIN_SEA
        || ship->AI_getUnitAIType() != state[TARGET_ROLE]
        || !canLoadOntoUnit(ship, ship->plot()) || group->AI_getMissionAIUnit() != ship
        || group->AI_getMissionAIType() != state[MISSION]
        || exposedToDanger(plot(), 60, true) || exposedToDanger(ship->plot(), 60, true)
        || stalled(state, GC.getMap().plotNum(getX(), getY()), GC.getGame().getGameTurn())) {
        state[KIND] = NONE; return false;
    }
    // Never let a previous embarkation decision strip a newly endangered
    // city of its required defender or happiness garrison.
    CvCity* city = plot()->getPlotCity();
    if (city && city->getOwner() == getOwner() && canDefend()
        && ((AI_isCityGarrison(city) && !city->AI_isDefended(-group->AI_getGenericValueTimes100(UNITVALUE_FLAGS_DEFENSIVE) / 100))
            || (isMilitaryHappiness() && !city->AI_isAdequateHappinessMilitary(-group->getNumUnits())))) {
        state[KIND] = NONE; return false;
    }
    const MissionAITypes mission = (MissionAITypes)state[MISSION];
    if (atPlot(ship->plot())) {
        state[KIND] = NONE;
        const PlayerTypes owner = getOwner(); const int id = getID();
        group->setTransportUnit(ship);
        CvUnit* alive = GET_PLAYER(owner).getUnit(id);
        if (alive && alive->isCargo() && colony && alive->getGroup())
            alive->getGroup()->AI_setMissionAI(mission, colony, ship);
        if (alive && !alive->isDelayedDeath() && !alive->isCargo() && alive->getGroup() && !alive->getGroup()->isBusy())
            alive->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, mission, colony, ship);
        return true;
    }
    if (group->isBusy()) return true;
    const PlayerTypes owner = getOwner(); const int id = getID();
    const PlayerTypes shipOwner = ship->getOwner(); const int shipId = ship->getID();
    const int flags = state[FLAGS] | ((AI_getUnitAIType() == UNITAI_SETTLE
        || AI_getUnitAIType() == UNITAI_WORKER) ? MOVE_NO_ENEMY_TERRITORY : 0);
    const bool moved = group->pushMissionInternal(MISSION_MOVE_TO_UNIT, ship->getOwner(), ship->getID(),
        flags, false, false, mission, colony, ship);
    CvUnit* alive = GET_PLAYER(owner).getUnit(id);
    if (!alive || alive->isDelayedDeath() || !alive->getGroup()) return true;
    ship = GET_PLAYER(shipOwner).getUnit(shipId);
    if (!ship || ship->isDelayedDeath()) {
        static_cast<CvUnitAI*>(alive)->m_aiNavalCommitment[KIND] = NONE;
        if (!alive->getGroup()->isBusy())
            alive->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, NO_MISSIONAI, NULL, NULL);
        return true;
    }
    if (!alive->isCargo() && alive->atPlot(ship->plot()) && alive->canLoadOntoUnit(ship, ship->plot())) {
        alive->getGroup()->setTransportUnit(ship);
        if (alive->isCargo()) {
            if (colony && alive->getGroup()) alive->getGroup()->AI_setMissionAI(mission, colony, ship);
            static_cast<CvUnitAI*>(alive)->m_aiNavalCommitment[KIND] = NONE;
            return true;
        }
    }
    if ((!moved || alive->atPlot(ship->plot())) && !alive->getGroup()->isBusy())
        alive->getGroup()->pushMission(MISSION_SKIP, -1, -1, 0, false, false, mission, colony, ship);
    return true;
}
