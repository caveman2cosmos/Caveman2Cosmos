#pragma once

#ifndef CIV4_ARMY_H
#define CIV4_ARMY_H

#include "CvArea.h"
#include "CvContractBroker.h"
#include "CvGameAI.h"
#include "CvGlobals.h"

#ifdef CVARMY_BREAKSAVE

class CvSelectionGroup;

class CvArmy
{
public:
    CvArmy();
    ~CvArmy();

    void init(int iID, PlayerTypes eOwner, int eMissionType);
    void uninit();
    void reset(int iID = 0, PlayerTypes eOwner = NO_PLAYER, bool bConstructorCall = false);

    // Getters/Setters
    CvSelectionGroup* getLeader() const;
    void setLeader(CvSelectionGroup* pLeader);
    void setLeader(int pLeaderID);

    int getID() const { return m_iID; }
    void setID(int iID) { m_iID = iID; }

    PlayerTypes getOwner() const { return m_eOwner; }

    int getMission() const { return m_eMission; }
    void setMission(int eMission) { m_eMission = eMission; }

    CvPlot* getTargetPlot() const { return m_pTargetPlot; }
    void setTargetPlot(CvPlot* pPlot) { m_pTargetPlot = pPlot; }

    void addGroup(CvSelectionGroup* pGroup);
    void removeGroup(CvSelectionGroup* pGroup);

    bool CheckTargetDefendPlot();
    bool CheckTargetCity();
    CvSelectionGroup* findNewLeader();
    void doTurn();

    void disband();

    CvSelectionGroup* getGroup(int iGroupID) const;
    CvSelectionGroup* getGroupByIndex(int iIndex) const;

private:
    int m_iID;
    PlayerTypes m_eOwner;
    int m_eMission;                // e.g., ARMY_MISSION_ATTACK_CITY
    CvPlot* m_pTargetPlot;
    int m_iLeaderGroupID;
    std::vector<int> m_groupIDs; // Liste des IDs des groupes

};

#endif

#endif