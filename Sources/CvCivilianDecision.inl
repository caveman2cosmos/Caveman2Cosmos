namespace {
void civilianDecisionTrace(const CvUnitAI& unit, const char* event, int kind, const CvPlot* target)
{
#ifdef C2C_RECALCULATED_BENCHMARK
    const char* trace = getenv("C2C_CIVILIAN_DECISION_TRACE");
    if (trace && trace[0] == '1')
        gDLL->logMsg("CivilianDecision.log", CvString::format(
            "CIVILIAN_DECISION turn=%d player=%d unit=%d role=%d event=%s kind=%d x=%d y=%d targetX=%d targetY=%d",
            GC.getGame().getGameTurn(), unit.getOwner(), unit.getID(), unit.AI_getUnitAIType(),
            event, kind, unit.getX(), unit.getY(), target ? target->getX() : -1, target ? target->getY() : -1).c_str());
#endif
}
}

std::vector<int> CvUnitAI::AI_civilianCitySites() const
{
    AI_PERF_SCOPE(CIV_SITES);
    // An accepted escort wait depends on available opportunities, not their
    // fluctuating relative scores. Keep score sign, legality, enemies and
    // reservations; a ready escort still enters the ordinary selection policy.
    std::vector<int> sites;
    const CvPlayerAI& owner = GET_PLAYER(getOwner());
    for (int i = 0; i < owner.AI_getNumCitySites(); ++i) {
        CvPlot* site = owner.AI_getCitySite(i);
        sites.push_back(GC.getMap().plotNum(site->getX(), site->getY()));
        const int value = site->getFoundValue(getOwner());
        sites.push_back(CivilianRefinement::escort() ? (value > 0 ? 1 : 0) : value);
        sites.push_back(canFound(site) ? 1 : 0);
        sites.push_back(site->isVisibleEnemyUnit(this) ? 1 : 0);
        sites.push_back(owner.AI_plotTargetMissionAIs(site, MISSIONAI_FOUND, getGroup()));
        if (CivilianRefinement::escort())
            sites.push_back(UnitTravelConnectivity::landUnitCanReach(this, site) ? 1 : 0);
    }
    return sites;
}

std::vector<int> CvUnitAI::AI_civilianLocalUnits() const
{
    std::vector<int> units;
    foreach_(const CvUnit* unit, plot()->units()) {
        units.push_back(unit->getOwner()); units.push_back(unit->getID());
        units.push_back(unit->AI_getUnitAIType()); units.push_back(unit->getCargo());
        units.push_back(unit->canMove() ? 1 : 0);
    }
    return units;
}

std::vector<int> CvUnitAI::AI_civilianRouteEvidence(const CvPlot* target) const
{
    std::vector<int> values;
    values.push_back(target->isBeingWorked() ? 1 : 0);
    const BonusTypes bonus=target->getNonObsoleteBonusType(getTeam());
    if(bonus!=NO_BONUS)values.push_back(bonus);
    return values;
}

void CvUnitAI::AI_rememberCivilianDecision(int kind, const CvPlot* target, int range)
{
    m_civilianDecision = CivilianDecision::State();
    if (!CivilianDecision::enabled() || isHuman() || isNPC() || isCargo() || !isGroupHead()
        || getDomainType() != DOMAIN_LAND || !target || plot()->getOwner() != getOwner()
        || target->getOwner() != getOwner()) return;
    CivilianDecision::State& s = m_civilianDecision;
    s.owner = getOwner(); s.map = GC.getMap().getType(); s.role = AI_getUnitAIType();
    s.type = getUnitType(); s.group = getGroupID(); s.members = getGroup()->getNumUnits();
    s.origin = GC.getMap().plotNum(getX(), getY());
    s.target = GC.getMap().plotNum(target->getX(), target->getY());
    s.improvement = target->getImprovementType(); s.route = target->getRouteType();
    s.feature = target->getFeatureType(); s.range = range;
    if (kind != CivilianDecision::SETTLER_ESCORT) s.observations = AI_civilianRouteEvidence(target);
    const CvPlayerAI& owner = GET_PLAYER(getOwner());
    s.cities = owner.getNumCities(); s.financial = owner.AI_isFinancialTrouble();
    if (kind == CivilianDecision::WORKER_ROUTE) {
        BuildTypes build = NO_BUILD;
        if (getGroup()->getBestBuildRoute(target, &build) == NO_ROUTE || build == NO_BUILD) return;
        s.build = build;
    }
    if (kind == CivilianDecision::SETTLER_ESCORT) {
        s.sites = AI_civilianCitySites();
        s.plotUnits = plot()->getNumUnits();
        s.observations = AI_civilianLocalUnits();
        s.pickup = owner.AI_unitTargetMissionAIs(this, MISSIONAI_PICKUP);
    }
    s.kind = kind;
    civilianDecisionTrace(*this, "ACCEPT", kind, target);
}

bool CvUnitAI::AI_civilianDecisionContext() const
{
    const CivilianDecision::State& s = m_civilianDecision;
    return CivilianDecision::enabled() && s.kind != CivilianDecision::NONE
        && !isHuman() && !isNPC() && !isCargo() && !isDelayedDeath() && isGroupHead()
        && getDomainType() == DOMAIN_LAND && s.owner == getOwner()
        && s.map == GC.getMap().getType() && s.role == AI_getUnitAIType()
        && s.type == getUnitType() && s.group == getGroupID()
        && s.members == getGroup()->getNumUnits() && plot()->getOwner() == getOwner()
        && s.cities == GET_PLAYER(getOwner()).getNumCities()
        && s.financial == GET_PLAYER(getOwner()).AI_isFinancialTrouble();
}

bool CvUnitAI::AI_continueWorkerRoute()
{
    CivilianDecision::State& s = m_civilianDecision;
    if (s.kind != CivilianDecision::WORKER_ROUTE && s.kind != CivilianDecision::WORKER_CONNECTION) return false;
    const int kind = s.kind;
    CvSelectionGroup* group = getGroup();
    CvPlot* target = s.map == GC.getMap().getType() ? GC.getMap().plotByIndex(s.target) : NULL;
    const MissionAITypes mission = group->AI_getMissionAIType();
    const bool waiting = mission == MISSIONAI_WAIT_FOR_ESCORT;
    if (!AI_civilianDecisionContext() || !target || !canBuildRoute()
        || (!waiting && mission != MISSIONAI_BUILD)
        || group->AI_getMissionAIPlot() != target
        || (waiting && s.origin != GC.getMap().plotNum(getX(), getY()))
        || target->getOwner() != getOwner() || !AI_plotValid(target)
        || !UnitTravelConnectivity::landUnitCanReach(this, target)
        || target->getImprovementType() != s.improvement || target->getRouteType() != s.route
        || target->getFeatureType() != s.feature || target->isVisibleEnemyUnit(this)
        || s.observations != AI_civilianRouteEvidence(target)
        || (kind == CivilianDecision::WORKER_ROUTE && !canBuild(target, (BuildTypes)s.build))
        || (kind == CivilianDecision::WORKER_CONNECTION && target->isConnectedToCapital(getOwner()))
        || GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(target, MISSIONAI_BUILD, group, s.range) != 0) {
        s.kind = CivilianDecision::NONE;
        civilianDecisionTrace(*this, "INVALIDATE", kind, target);
        return false;
    }
    if (kind == CivilianDecision::WORKER_ROUTE) {
        BuildTypes best = NO_BUILD;
        getGroup()->getBestBuildRoute(target, &best);
        if (best != s.build) {
            s.kind = CivilianDecision::NONE;
            civilianDecisionTrace(*this, "NEW_ROUTE_BUILD", kind, target);
            return false;
        }
    }
    // Revalidate execution of this one job, not all alternative jobs. In
    // particular a safer route or an arriving escort wakes the worker now.
    if (atPlot(target) || generatePath(target, MOVE_WITH_CAUTION | MOVE_NO_ENEMY_TERRITORY, true)) {
        civilianDecisionTrace(*this, "RESUME", kind, target);
        return group->pushMissionInternal(MISSION_ROUTE_TO, target->getX(), target->getY(),
            MOVE_WITH_CAUTION | MOVE_NO_ENEMY_TERRITORY, false, false, MISSIONAI_BUILD, target);
    }
    if (!generatePath(target, MOVE_IGNORE_DANGER | MOVE_NO_ENEMY_TERRITORY, true)) {
        s.kind = CivilianDecision::NONE;
        civilianDecisionTrace(*this, "UNREACHABLE", kind, target);
        return false;
    }
    civilianDecisionTrace(*this, "WAIT", kind, target);
    group->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT, target);
    return true;
}

void CvUnitAI::AI_requestSettlerEscort()
{
    if (m_contractsLastEstablishedTurn == GC.getGame().getGameTurn()) return;
    GET_PLAYER(getOwner()).getContractBroker().advertiseWork(HIGHEST_PRIORITY_ESCORT_PRIORITY,
        OFFENSIVE_UNITCAPABILITIES, getX(), getY(), this, UNITAI_ESCORT, AI_minSettlerDefense());
    m_contractsLastEstablishedTurn = GC.getGame().getGameTurn();
    m_contractualState = CONTRACTUAL_STATE_AWAITING_ANSWER;
}

bool CvUnitAI::AI_continueSettlerEscort()
{
    AI_PERF_SCOPE(CIV_ESCORT_WAIT);
    CivilianDecision::State& s = m_civilianDecision;
    if (s.kind != CivilianDecision::SETTLER_ESCORT) return false;
    CvSelectionGroup* group = getGroup();
    const MissionAITypes mission = group->AI_getMissionAIType();
    // A new order, transport, city-site change, escort/group change, or new
    // founding opportunity goes through the ordinary policy immediately.
    if (!AI_civilianDecisionContext() || s.origin != GC.getMap().plotNum(getX(), getY())
        || !plot()->isCity() || plot()->getNumUnits() != s.plotUnits
        || (mission != NO_MISSIONAI && mission != MISSIONAI_WAIT_FOR_ESCORT)
        || (group->canDefend() && group->getStrength() > AI_minSettlerDefense())
        || (CivilianRefinement::escort() && GET_PLAYER(getOwner()).AI_getAnyPlotDanger(plot(), 1, false))
        || s.pickup != GET_PLAYER(getOwner()).AI_unitTargetMissionAIs(this, MISSIONAI_PICKUP)
        || s.observations != AI_civilianLocalUnits()
        || s.sites != AI_civilianCitySites()) {
        s.kind = CivilianDecision::NONE;
        civilianDecisionTrace(*this, "INVALIDATE", CivilianDecision::SETTLER_ESCORT, plot());
        return false;
    }
    AI_requestSettlerEscort();
    civilianDecisionTrace(*this, "WAIT", CivilianDecision::SETTLER_ESCORT, plot());
    group->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_WAIT_FOR_ESCORT, plot());
    return true;
}

bool CvUnitAI::AI_continueFoundingMission()
{
    CvSelectionGroup* group = getGroup();
    if (!CivilianDecision::enabled() || isHuman() || isNPC() || isCargo() || !isGroupHead()
        || group->AI_getMissionAIType() != MISSIONAI_FOUND) return false;
    CvPlot* target = group->AI_getMissionAIPlot();
    if (!target || !canFound(target) || !UnitTravelConnectivity::landUnitCanReach(this, target)
        || target->getFoundValue(getOwner()) <= 0
        || !group->canDefend() || group->getStrength() <= AI_minSettlerDefense() / 2
        || target->isVisibleEnemyUnit(this)
        || GET_PLAYER(getOwner()).AI_plotTargetMissionAIs(target, MISSIONAI_FOUND, group) != 0
        || !generatePath(target, MOVE_SAFE_TERRITORY | MOVE_NO_ENEMY_TERRITORY, true)) {
        group->AI_setMissionAI(NO_MISSIONAI, NULL, NULL);
        civilianDecisionTrace(*this, "INVALIDATE_FOUND", CivilianDecision::NONE, target);
        return false;
    }
    civilianDecisionTrace(*this, "CONTINUE_FOUND", CivilianDecision::NONE, target);
    if (atPlot(target)) {
        group->pushMission(MISSION_FOUND, -1, -1, 0, false, false, MISSIONAI_FOUND, target);
        return true;
    }
    const CvPlot* end = getPathEndTurnPlot();
    if (!end || atPlot(end)) {
        group->pushMission(MISSION_SKIP, -1, -1, 0, false, false, MISSIONAI_FOUND, target);
        return true;
    }
    group->pushMission(MISSION_MOVE_TO, end->getX(), end->getY(), MOVE_SAFE_TERRITORY | MOVE_NO_ENEMY_TERRITORY,
        false, false, MISSIONAI_FOUND, target);
    return true;
}
