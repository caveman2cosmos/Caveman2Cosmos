#include "CvArmy.h"
#include "CvUnit.h"
#include "CvSelectionGroup.h"
#include "CvString.h"
#include "CvPlayerAI.h"

#define NO_INDEX -1

#ifdef CVARMY_BREAKSAVE

CvArmy::CvArmy()
{
    reset(0, NO_PLAYER, true);
}

CvArmy::~CvArmy()
{
    uninit();
}

void CvArmy::init(int iID, PlayerTypes eOwner, int eMissionType)
{
    // Initialize saved data
    reset(iID, eOwner);

    // Initialize non-saved data
    m_eMission = eMissionType;
    m_iLeaderGroupID = NO_INDEX;
    m_pTargetPlot = NULL;
}

void CvArmy::uninit()
{
    m_groupIDs.clear();
    m_iLeaderGroupID = NO_INDEX;
    m_pTargetPlot = NULL;
}

void CvArmy::reset(int iID, PlayerTypes eOwner, bool bConstructorCall)
{
    // Uninit class
    uninit();

    // Initialize data members
    m_iID = iID;
    m_eOwner = eOwner;
    m_eMission = 0;
    m_iLeaderGroupID = NULL;
    m_pTargetPlot = NULL;
}


void CvArmy::addGroup(CvSelectionGroup* pGroup)
{
    if (pGroup != NULL)
    {
        if (pGroup->getHeadUnit() != NULL)
        {
            m_groupIDs.push_back(pGroup->getID());
            pGroup->setArmyID(m_iID); // mark the group with this army's ID
        }
    }
}

void CvArmy::removeGroup(CvSelectionGroup* pGroup)
{
    if (pGroup == NULL)
        return;

    std::vector<int>::iterator it = std::find(m_groupIDs.begin(), m_groupIDs.end(), pGroup->getID());
    if (it != m_groupIDs.end())
    {
        m_groupIDs.erase(it);
        pGroup->setArmyID(-1); // reset group's army ID
    }

    // Reset leader if necessary
    if (pGroup->getID() == m_iLeaderGroupID)
        m_iLeaderGroupID = NO_INDEX;
}


bool CvArmy::CheckTargetDefendPlot()
{
    CvSelectionGroup* pLeaderGroup = getLeader();
    if (pLeaderGroup == NULL) return false;
    if (pLeaderGroup->getHeadUnit() == NULL) return false;
    CvPlot* pLeaderPlot = pLeaderGroup->getHeadUnit()->plot();
    CvArea* pArea = pLeaderPlot->area();
    if (m_pTargetPlot == NULL || pLeaderGroup->at(m_pTargetPlot->getX(), m_pTargetPlot->getY()))
    {
        const CvPlayerAI& player = GET_PLAYER(pLeaderGroup->getHeadUnit()->getOwner());
        CvCity* pTargetCity = NULL;// player.AI_p  AI_findTargetCity(pArea);
        if (pTargetCity != NULL)
        {
            setTargetPlot(pTargetCity->plot());
            // set the army's target plot
            return true;
        }
    }
    return false;
}


bool CvArmy::CheckTargetCity()
{
    CvSelectionGroup* pLeaderGroup = getLeader();
    if (pLeaderGroup == NULL) return false;
    if (pLeaderGroup->getHeadUnit() == NULL) return false;    
    CvPlot* pLeaderPlot = pLeaderGroup->getHeadUnit()->plot();
    CvArea* pArea = pLeaderPlot->area();
    if (m_pTargetPlot == NULL || pLeaderGroup->at(m_pTargetPlot->getX(), m_pTargetPlot->getY()))
    {
        const CvPlayerAI& player = GET_PLAYER(pLeaderGroup->getHeadUnit()->getOwner());
        CvCity* pTargetCity = player.AI_findTargetCity(pArea);
        if (pTargetCity != NULL)
        {
            setTargetPlot(pTargetCity->plot());
            // set the army's target plot
            return true;
        }
    }
    return false;
}

CvSelectionGroup* CvArmy::findNewLeader()
{
    // Leader is dead or invalid -> recalculate
    int iBestStrength = -1;
    int iBestGroupID = -1;

    for (std::vector<int>::const_iterator it = m_groupIDs.begin(); it != m_groupIDs.end(); ++it)
    {
        CvSelectionGroup* pGroup = GET_PLAYER(getOwner()).getSelectionGroup(*it);
        if (pGroup == NULL || pGroup->getHeadUnit() == NULL)
            continue;

        int iStrength = pGroup->getNumUnits(); // helper in DLL: total combat strength
        if (iStrength > iBestStrength)
        {
            iBestStrength = iStrength;
            iBestGroupID = *it;
        }
    }

    // 2) If none found, look nearby (e.g. 3 tiles)
    CvSelectionGroup* pLeader = NULL;
    if (iBestGroupID != -1)
    {
        pLeader = GET_PLAYER(getOwner()).getSelectionGroup(iBestGroupID);
        const int iSearchRange = 2;
        CvSelectionGroup* pLeader = GET_PLAYER(getOwner()).getSelectionGroup(iBestGroupID);
        for (CvPlayer::group_iterator it = GET_PLAYER(getOwner()).groups().begin();
             it != GET_PLAYER(getOwner()).groups().end(); ++it)
        {
            CvSelectionGroup* pGroup = *it;
            if (pGroup == NULL || pGroup->getHeadUnit() == NULL)
                continue;

            // skip groups already in another army
            if (pGroup->getDomainType() != DOMAIN_LAND || (pGroup->getArmyID() != -1 && pGroup->getArmyID() != getID()))
                continue;

            // distance check (relative to old leader's plot if exists, otherwise skip)
            if (pLeader != NULL && pLeader->plot() != NULL)
            {
                CvPlot* pPlot = pGroup->plot();
                if (pPlot == NULL)
                    continue;

                int iDist = plotDistance(
                    pLeader->plot()->getX(), pLeader->plot()->getY(),
                    pPlot->getX(), pPlot->getY()
                );
                if (iDist > iSearchRange)
                    continue;
            }

            int iStrength = pGroup->getNumUnits();
            if (iStrength > iBestStrength)
            {
                iBestStrength = iStrength;
                iBestGroupID = pGroup->getID();
            }
        }
    }


    if (iBestGroupID != -1)
    {
        // Update leader
        m_iLeaderGroupID = iBestGroupID;
        pLeader = GET_PLAYER(getOwner()).getSelectionGroup(iBestGroupID);
        FAssert(pLeader);
        CvUnit * pLeaderUnit = pLeader->getHeadUnit();
        if (pLeaderUnit != NULL)
        {
            UnitAITypes eUnitAi = pLeaderUnit->AI_getUnitAIType();
            FAssert(pLeader);
            MissionAITypes eMissionAI = pLeader->AI_getMissionAIType();
            CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
            CvWString MissionInfos = MissionAITypeToString(eMissionAI);
            CvWString StrUnitName = pLeaderUnit->getNameNoDesc();
            LOG_UNIT_BLOCK(3, {
                if (StrUnitName.length() == 0)
                {
                    StrUnitName = pLeaderUnit->getName(0).GetCString();
                }

                logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], is new Leader of Army %d...", getOwner(), pLeaderUnit->getID(), StrUnitName.GetCString(), StrunitAIType.GetCString(), pLeaderUnit->getX(), pLeaderUnit->getY(), MissionInfos.GetCString(), pLeader->getNumUnits(), m_iID);
                //logBBAI("       Attack (estim after Bomb.) : %d, AttackRatio : %d", iComparePostBombard, iAttackRatio);
            });
        }
        return GET_PLAYER(getOwner()).getSelectionGroup(iBestGroupID);
    }

    // No valid leader left
    m_iLeaderGroupID = -1;
    return NULL;
}


void CvArmy::doTurn()
{
    if (m_iLeaderGroupID == NO_INDEX)
        return;
    CvSelectionGroup* pLeaderGroup = getLeader();
    if (pLeaderGroup == NULL)
    {
        pLeaderGroup = findNewLeader();
        if (pLeaderGroup == NULL)
        {
            disband();
            return;
        }
    }

    if (pLeaderGroup->getHeadUnit() == NULL)
    {
        disband();
        return;
    }

    CvPlot* pLeaderPlot = pLeaderGroup->plot();
    if (pLeaderPlot == NULL)
        return;

    switch (m_eMission)
    {
    case ARMY_MISSION_ATTACK_CITY:
    {
        bool allReady = true;

        if (m_pTargetPlot == NULL)
        {
            CheckTargetCity();
        }
        // 1) Phase de regroupement autour du leader
        for (std::vector<int>::iterator it = m_groupIDs.begin(); it != m_groupIDs.end(); ++it)
        {
            CvSelectionGroup* pGroup = GET_PLAYER(getOwner()).getSelectionGroup(*it);
            try
            {
                if (pGroup == NULL)
                    continue;
                if (pGroup->getHeadUnit() == NULL)
                {
                    removeGroup(pGroup);
                    pGroup->kill();
                    continue;
                }
            }
            catch (...) {
                try
                {
                    removeGroup(pGroup);
                }
                catch (...) {}
                continue;
            }
        


            CvPlot* pPlot = pGroup->plot();
            if (pPlot == NULL)
                continue;

            int iDist = plotDistance(pPlot->getX(), pPlot->getY(),
                                     pLeaderPlot->getX(), pLeaderPlot->getY());

            // Si le groupe est à plus de 1 case du leader, le faire avancer
            if (iDist > 2)
            {
                allReady = false;
                pGroup->pushMission(
                    MISSION_MOVE_TO,
                    pLeaderPlot->getX(),
                    pLeaderPlot->getY(),
                    0,
                    false, true,
                    MISSIONAI_GROUP,
                    pLeaderPlot
                );

                if (pGroup != NULL)
                {
                    const CvUnit* pLeaderUnit = pGroup->getHeadUnit();
                    if (pLeaderUnit != NULL)
                    {
                        UnitAITypes eUnitAi = pLeaderUnit->AI_getUnitAIType();
                        FAssert(pGroup);
                        MissionAITypes eMissionAI = pGroup->AI_getMissionAIType();
                        CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
                        CvWString MissionInfos = MissionAITypeToString(eMissionAI);
                        CvWString StrUnitName = pLeaderUnit->getNameNoDesc();
                        LOG_UNIT_BLOCK(3, {
                            if (StrUnitName.length() == 0)
                            {
                                StrUnitName = pLeaderUnit->getName(0).GetCString();
                            }

                            logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Army %d, is joining its leader at (%d,%d), distance %d...", getOwner(), pLeaderUnit->getID(), StrUnitName.GetCString(), StrunitAIType.GetCString(), pLeaderUnit->getX(), pLeaderUnit->getY(), MissionInfos.GetCString(), pGroup->getNumUnits(), m_iID, pLeaderPlot->getX(), pLeaderPlot->getY(), iDist);
                            //logBBAI("       Attack (estim after Bomb.) : %d, AttackRatio : %d", iComparePostBombard, iAttackRatio);
                        });
                    }
                }
            }
        }

        // 2) Une fois tous les groupes prêts, avancer ensemble (leader + suiveurs)
        if (allReady && m_pTargetPlot != NULL)
        {
            
            CvPlot* pRefPlot = pLeaderGroup->plot();
            int distToTarget = 99;
            if (pRefPlot != NULL && m_pTargetPlot != NULL)
            {
                distToTarget = plotDistance(pRefPlot->getX(), pRefPlot->getY(),
                            m_pTargetPlot->getX(), m_pTargetPlot->getY());
            }

            if (distToTarget <= 20)
            {
                CvPlayer& player = GET_PLAYER(pLeaderGroup->getOwner());
                for (CvPlayer::group_iterator it = player.groups().begin(); it != player.groups().end(); ++it)
                {
                    CvSelectionGroup* pGroup = *it;
                    if (pGroup == NULL || pGroup == pLeaderGroup)
                        continue;
                    if (pGroup->getHeadUnit() == NULL)
                    {
                        removeGroup(pGroup);
                        pGroup->kill();
                        continue;
                    }

                    CvPlot* pPlot = pGroup->plot();
                    if (pPlot == NULL)
                        continue;

                    if (pGroup->getDomainType() != DOMAIN_LAND || pGroup->AI_getMissionAIType() == MISSIONAI_GUARD_CITY
                        || pGroup->AI_isCityGarrison(pGroup->plot()->getPlotCity()))                        
                        continue;

                    int dist = plotDistance(pRefPlot->getX(), pRefPlot->getY(),
                                            pPlot->getX(), pPlot->getY());
                    if (dist <= 5) // distance <= 3 cases
                    {
                        CvUnit* pLeaderUnit = pGroup->getHeadUnit();
                        if (pLeaderUnit == NULL)
                        {
                            removeGroup(pGroup);
                        }
                        UnitAITypes eUnitAi = pLeaderUnit->AI_getUnitAIType();

                        if (eUnitAi == UNITAI_PROPERTY_CONTROL || eUnitAi == UNITAI_INFILTRATOR)
                            continue;

                        pLeaderUnit->AI_setUnitAIType(UNITAI_ATTACK_CITY);
                        addGroup(pGroup);

                        //UnitAITypes eUnitAi = pLeaderUnit->AI_getUnitAIType();
                        MissionAITypes eMissionAI = pGroup->AI_getMissionAIType();
                        CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
                        CvWString MissionInfos = MissionAITypeToString(eMissionAI);
                        CvWString StrUnitName = pLeaderUnit->getNameNoDesc();

                        LOG_UNIT_BLOCK(3, {                            
                            //UnitAITypes eUnitAi = pGroup->getHeadUnit()->AI_getUnitAIType();
                            //MissionAITypes eMissionAI = pGroup->AI_getMissionAIType();
                            //CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
                            //CvWString MissionInfos = MissionAITypeToString(eMissionAI);
                            //CvWString StrUnitName = pLeaderUnit->getNameNoDesc();
                            if (StrUnitName.length() == 0)
                            {
                                StrUnitName = pLeaderUnit->getName(0).GetCString();
                            }

                            logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Army %d is forced incorporation to Attack at (%d,%d) distance %d...", getOwner(), pLeaderUnit->getID(), StrUnitName.GetCString(), StrunitAIType.GetCString(), pLeaderUnit->getX(), pLeaderUnit->getY(), MissionInfos.GetCString(), pGroup->getNumUnits(), m_iID, m_pTargetPlot->getX(), m_pTargetPlot->getY(), dist);
                            //logBBAI("       Attack (estim after Bomb.) : %d, AttackRatio : %d", iComparePostBombard, iAttackRatio);
                        });
                        pGroup->pushMission(MISSION_SKIP);
                        pGroup->pushMissionInternal(MISSION_MOVE_TO_UNIT, pLeaderGroup->getOwner(), pLeaderGroup->getHeadUnit()->getID(), 0, false, false, MISSIONAI_GROUP, NULL, pLeaderGroup->getHeadUnit());

                    }
                }
            }
            
            LOG_UNIT_BLOCK(3, {
                const CvUnit * pLeaderUnit = pLeaderGroup->getHeadUnit();
                UnitAITypes eUnitAi = pLeaderGroup->getHeadUnit()->AI_getUnitAIType();
                MissionAITypes eMissionAI = pLeaderGroup->AI_getMissionAIType();
                CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
                CvWString MissionInfos = MissionAITypeToString(eMissionAI);
                CvWString StrUnitName = pLeaderUnit->getNameNoDesc();
                if (StrUnitName.length() == 0)
                {
                    StrUnitName = pLeaderUnit->getName(0).GetCString();
                }

                logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Army %d is Ready will Command Attack at (%d,%d) distance %d...", getOwner(), pLeaderUnit->getID(), StrUnitName.GetCString(), StrunitAIType.GetCString(), pLeaderUnit->getX(), pLeaderUnit->getY(), MissionInfos.GetCString(), pLeaderGroup->getNumUnits(), m_iID, m_pTargetPlot->getX(), m_pTargetPlot->getY(), distToTarget);
                //logBBAI("       Attack (estim after Bomb.) : %d, AttackRatio : %d", iComparePostBombard, iAttackRatio);
            });
            // Leader avance
            //m_pLeader->pushMission(
            //    MISSION_MOVE_TO,
            //    m_pTargetPlot->getX(),
            //    m_pTargetPlot->getY(),
            //    0,
            //    false, true,
            //    MISSIONAI_ASSAULT,
            //    m_pTargetPlot
            //);

            // Suiveurs avancent
            for (std::vector<int>::iterator it = m_groupIDs.begin(); it != m_groupIDs.end(); ++it)
            {
                CvSelectionGroup* pGroup = GET_PLAYER(getOwner()).getSelectionGroup(*it);
                if (pGroup == NULL)
                    continue;
                if (pGroup->getHeadUnit() == NULL)
                {
                    removeGroup(pGroup);
                    pGroup->kill();
                    continue;
                }
                //pGroup->pushMission(
                //    MISSION_MOVE_TO,
                //    m_pTargetPlot->getX(),
                //    m_pTargetPlot->getY(),
                //    0,
                //    false, true,
                //    MISSIONAI_ASSAULT,
                //    m_pTargetPlot
                //);
                LOG_UNIT_BLOCK(3, {
                    const CvUnit * pLeaderUnit = pGroup->getHeadUnit();
                    UnitAITypes eUnitAi = pGroup->getHeadUnit()->AI_getUnitAIType();
                    MissionAITypes eMissionAI = pGroup->AI_getMissionAIType();
                    CvWString StrunitAIType = GC.getUnitAIInfo(eUnitAi).getType();
                    CvWString MissionInfos = MissionAITypeToString(eMissionAI);
                    CvWString StrUnitName = pLeaderUnit->getNameNoDesc();
                    if (StrUnitName.length() == 0)
                    {
                        StrUnitName = pLeaderUnit->getName(0).GetCString();
                    }

                    logBBAI("Player %d Unit ID %d, %S of Type %S, at (%d, %d), Mission %S [stack size %d], Army %d is Ready and follow leader to Attack at (%d,%d)...", getOwner(), pLeaderUnit->getID(), StrUnitName.GetCString(), StrunitAIType.GetCString(), pLeaderUnit->getX(), pLeaderUnit->getY(), MissionInfos.GetCString(), pGroup->getNumUnits(), m_iID, m_pTargetPlot->getX(), m_pTargetPlot->getY());
                    //logBBAI("       Attack (estim after Bomb.) : %d, AttackRatio : %d", iComparePostBombard, iAttackRatio);
                });
            }
        }
        break;
    }

    case ARMY_MISSION_DEFEND_BORDER:
    {
        
        if (m_pTargetPlot == NULL)
        {
            CheckTargetDefendPlot();
        }
        
        // Leader et suiveurs vont défendre la case cible
        if (pLeaderGroup->plot() != m_pTargetPlot)
        {
            pLeaderGroup->pushMission(
                MISSION_MOVE_TO,
                m_pTargetPlot->getX(),
                m_pTargetPlot->getY(),
                0,
                false, true,
                MISSIONAI_GUARD_BONUS,
                m_pTargetPlot
            );
        }

        for (std::vector<int>::iterator it = m_groupIDs.begin(); it != m_groupIDs.end(); ++it)
        {
            CvSelectionGroup* pGroup = GET_PLAYER(getOwner()).getSelectionGroup(*it);
            if (pGroup == NULL)
                continue;
            if (pGroup->getHeadUnit() == NULL)
            {
                removeGroup(pGroup);
                pGroup->kill();
                continue;
            }

            if (pGroup->plot() != m_pTargetPlot)
            {
                pGroup->pushMission(
                    MISSION_MOVE_TO,
                    m_pTargetPlot->getX(),
                    m_pTargetPlot->getY(),
                    0,
                    false, true,
                    MISSIONAI_GUARD_BONUS,
                    m_pTargetPlot
                );
            }
        }
        break;
    }

    default:
        break;
    }
}

void CvArmy::disband()
{
    // Detach all groups
    for (std::vector<int>::iterator it = m_groupIDs.begin(); it != m_groupIDs.end(); ++it)
    {
        CvSelectionGroup* pGroup = GET_PLAYER(getOwner()).getSelectionGroup(*it);
        if (pGroup != NULL)
        {
            pGroup->setArmyID(-1); // plus d’armée
        }
    }
    m_groupIDs.clear();
    m_iLeaderGroupID = NULL;
}

CvSelectionGroup* CvArmy::getGroupByIndex(int iIndex) const
{
    if (iIndex < 0 || iIndex >= (int)m_groupIDs.size())
        return NULL;

    return GET_PLAYER(getOwner()).getSelectionGroup(m_groupIDs[iIndex]);
}

CvSelectionGroup* CvArmy::getGroup(int iGroupID) const
{
    // Check if the army actually contains this group ID
    if (std::find(m_groupIDs.begin(), m_groupIDs.end(), iGroupID) == m_groupIDs.end())
    {
        return NULL; // group not in this army
    }

    // Return the actual group pointer
    return GET_PLAYER(getOwner()).getSelectionGroup(iGroupID);
}


CvSelectionGroup* CvArmy::getLeader() const
{
    return GET_PLAYER(getOwner()).getSelectionGroup(m_iLeaderGroupID);
}

void CvArmy::setLeader(CvSelectionGroup* pLeader)
{
    if (pLeader != NULL)
    {
        m_iLeaderGroupID = pLeader->getID();
        pLeader->setArmyID(m_iID);
    }
    else
    {
        m_iLeaderGroupID = NO_INDEX;
    }

}


void CvArmy::setLeader(int iLeaderGroupID)
{
    if (iLeaderGroupID == NO_INDEX)
    {
        m_iLeaderGroupID = NO_INDEX;
        return;
    }
    // Store the ID
    CvSelectionGroup* pLeader = GET_PLAYER(getOwner()).getSelectionGroup(iLeaderGroupID);
    if (pLeader != NULL)
    {
        m_iLeaderGroupID = iLeaderGroupID;
        setLeader(pLeader);
    }

}



#endif