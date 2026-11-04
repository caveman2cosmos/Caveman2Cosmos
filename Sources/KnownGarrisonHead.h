#pragma once

// Plot iteration already resolved this unit's owner/ID. If it is the stored
// group head, reuse that same live object instead of resolving its ID again.
// Other heads, missing heads and stale IDs retain the original group query.
inline bool knownUnitGroupIsCityGarrison(const CvUnit* unit, const CvCity* city)
{
    const CvSelectionGroup* group = unit->getGroup();
    const CLLNode<IDInfo>* head = group->headUnitNode();
    if (head && head->m_data.eOwner == unit->getOwner() && head->m_data.iID == unit->getID())
        return unit->AI_isCityGarrison(city);
    return group->AI_isCityGarrison(city);
}
