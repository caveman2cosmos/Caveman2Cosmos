// unitAI.cpp


#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvCity.h"
#include "CvContractBroker.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvMap.h"
#include "CvPathGenerator.h"
#include "CvPlayerAI.h"
#include "CvPlot.h"
#include "CvSelectionGroup.h"
#include "CvUnit.h"

CvContractBroker::CvContractBroker() : m_eOwner(NO_PLAYER)
{
	reset();
}

CvContractBroker::~CvContractBroker()
{
}


void CvContractBroker::log(int level, char* format, ...)
{
	static char logString[2048];
	_vsnprintf(logString, 2048 - 4, format, (char*)(&format + 1));

	// Keep the structure consistent with the other tagged AI logs: every line leads
    // with its [CTB/*] tag and carries the player as an owner=%d field (the numeric
    // id, cross-referenceable via GameInfo.log), rather than a "<Name> - " prefix.
    logContractBroker(level, "%s owner=%d", logString, (int)m_eOwner);
}

//	Delete all work requests and looking for work records
void CvContractBroker::reset()
{
	//	No need to lock here as this is (currently) always run in a single-threaded context

	m_workRequests.clear();
	m_advertisingUnits.clear();
	m_advertisingTenders.clear();
	m_contractedUnits.clear();

	m_iNextWorkRequestId = 0;
	m_iEmployedUnits = 0;
}

void CvContractBroker::cleanup()
{
	if (m_eOwner == NO_PLAYER || GET_PLAYER(m_eOwner).getName() == NULL) return;

	m_ownerName = GET_PLAYER(m_eOwner).getName();

	int fulfilledContracts = 0;

	log(1, "[CTB/turn] cleanup contractedUnits=%d advertisingTenders=%d advertisingUnits=%d",
        m_contractedUnits.size(),
        m_advertisingTenders.size(),
        m_advertisingUnits.size());

	log(1, "[CTB/turn] workRequests=%d", m_workRequests.size());
    log(1, "[CTB/turn] tendersPostedLastTurn=%d", m_advertisingTenders.size());

    log(2, "[CTB/turn] clearing contractedUnits=%d advertisingTenders=%d resetting employedUnits (was %d) to 0",
    	m_contractedUnits.size(), m_advertisingTenders.size(), m_iEmployedUnits);

	m_contractedUnits.clear();
	m_advertisingTenders.clear();
	m_iEmployedUnits = 0;
	for (unsigned int iI = 0; iI < (int)m_workRequests.size(); iI++)
	{
		if (m_workRequests[iI].bFulfilled)
		{
			workRequest* wr = &m_workRequests[iI];

			log(2, "[CTB/fulfilled] erasing fulfilled workRequest=%d priority=%d at=(%d,%d) aiType=%d (cleanup)",
            	wr->iWorkRequestId, wr->iPriority, wr->iAtX, wr->iAtY, (int)wr->eAIType);


			m_workRequests.erase(wr);
			fulfilledContracts++;
		}

	}
	log(1, "[CTB/turn] fulfilledContractsLastTurn=%d workRequestsRemaining=%d", fulfilledContracts, m_workRequests.size());
}

//	Initialize
void CvContractBroker::init(PlayerTypes eOwner)
{
	m_eOwner = eOwner;
}


bool CvContractBroker::alreadyLookingForWork(const CvUnit* pUnit)
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < (int)m_advertisingUnits.size(); iI++)
	{
		if (m_advertisingUnits[iI].iUnitId == pUnit->getID())
        {

            log(1,
                "[CTB/avail/dup] unit=%S (%d) at=(%d,%d) already in worklist",
                pUnit->getDescription().GetCString(),
                pUnit->getID(),
                pUnit->getX(),
                pUnit->getY());

            log(1,
                "[CTB/avail/dup] unitsRemainingInWorklist=%d",
                m_advertisingUnits.size()
            );
            return true;
        }
	}
	return false;
}
//	Note a unit looking for work
void CvContractBroker::lookingForWork(const CvUnit* pUnit, int iMinPriority)
{
	PROFILE_FUNC();

	if (alreadyLookingForWork(pUnit))
	{
		return;
	}

	// [CTB/avail] -- a unit advertises itself to the broker as available for work.
    log(2, "[CTB/avail] unit=%d type=%d aiType=%d minPriority=%d", pUnit->getID(), (int)pUnit->getUnitType(), (int)pUnit->AI_getUnitAIType(), iMinPriority);

	advertisingUnit	unitDetails;
	unitDetails.eUnitType = pUnit->getUnitType();

	const int iUnitStr = GC.getGame().AI_combatValue(pUnit->getUnitType());

	unitDetails.bIsWorker = (pUnit->AI_getUnitAIType() == UNITAI_WORKER);
	unitDetails.bIsHealer = (pUnit->AI_getUnitAIType() == UNITAI_HEALER);

	//	Combat values are just the crude value of the unit type for now - should add in promotions
	//	here for sure
	if (pUnit->canDefend())
	{
		unitDetails.iDefensiveValue = iUnitStr;
	}
	//TB OOS Debug: Almost certainly a solid fix here
	else
	{
		unitDetails.iDefensiveValue = 0;
	}
	if (pUnit->canAttack())
	{
		unitDetails.iOffensiveValue = iUnitStr;
	}
	else
	{
		unitDetails.iOffensiveValue = 0;
	}

	unitDetails.iUnitId = pUnit->getID();
	unitDetails.iAtX = pUnit->getX();
	unitDetails.iAtY = pUnit->getY();
	unitDetails.iMinPriority = iMinPriority;

	//	Initially not assigned to a contract
	unitDetails.iContractedWorkRequest = -1;
	//	and no attempt has been made yet to match any work requests
	unitDetails.iMatchedToRequestSeqThisPlot = -1;
	unitDetails.iMatchedToRequestSeqAnyPlot = -1;

	m_advertisingUnits.push_back(unitDetails);

	log(1,
    "[CTB/avail] unit=%S (%d) at=(%d,%d) asking for work",
    pUnit->getDescription().GetCString(),
    pUnit->getID(),
    pUnit->getX(),
    pUnit->getY());

	log(2,
    "[CTB/avail] unit=%d worker=%d healer=%d offValue=%d defValue=%d minPriority=%d",
    unitDetails.iUnitId,
    unitDetails.bIsWorker ? 1 : 0,
    unitDetails.bIsHealer ? 1 : 0,
    unitDetails.iOffensiveValue,
    unitDetails.iDefensiveValue,
    unitDetails.iMinPriority);

    log(1,
    "[CTB/avail] unitsRemainingInWorklist=%d",
    m_advertisingUnits.size()
    );

}

//	Unit fulfilled its work and is no longer advertising as available
void CvContractBroker::removeUnit(const CvUnit* pUnit)
{
	PROFILE_EXTRA_FUNC();

	internalRemoveUnit(pUnit->getID());

	log(1,
        "[CTB/remove] unit=%S (%d) at=(%d,%d) removed from worklist",
        pUnit->getDescription().GetCString(),
        pUnit->getID(),
        pUnit->getX(),
        pUnit->getY());

}

void CvContractBroker::removeUnit(const int iUnitId)
{
	PROFILE_EXTRA_FUNC();

	internalRemoveUnit(iUnitId);

	log(1, "[CTB/remove] unit=%d removed from worklist", iUnitId);
}



// Make a work request
//	iPriority should be in the range 0-100 ideally
//	eUnitFlags indicate the type(s) of unit sought
//	(iAtX,iAtY) is (roughly) where the work will be
//	pJoinUnit may be NULL but if not it is a request to join that unit's group
void CvContractBroker::advertiseWork(int iPriority, unitCapabilities eUnitFlags, int iAtX, int iAtY, const CvUnit* pJoinUnit, UnitAITypes eAIType, int iUnitStrength, const CvUnitSelectionCriteria* criteria, int iMaxPath)
{
	PROFILE_FUNC();

	int iUnitStrengthTimes100 = (iUnitStrength == -1 ? -1 : iUnitStrength * 100);
	if (pJoinUnit != NULL)
	{
		workRequest* wr = findWorkRequestByUnitId(pJoinUnit->getID());
		if (wr != NULL)
        {
            log(2, "[CTB/work] join request for unit=%d ignored - already an outstanding request (workRequest=%d) for that unit",
                pJoinUnit->getID(), wr->iWorkRequestId);
            return;
        }
	}

    // [CTB/work] -- a job is advertised to the broker (a need for a unit).
    if (gPlayerLogLevel >= 2)
    {
        CvString szWorkCriteria = (criteria != NULL ? criteria->getDescription() : CvString(""));
        if (!szWorkCriteria.empty())
        {
            szWorkCriteria.Format(" criteria=(%s)", szWorkCriteria.c_str());
        }
        log(2, "[CTB/work] priority=%d at=(%d,%d) aiType=%d flags=0x%x strength=%d(x100=%d) maxPath=%d join=%d%s",
            iPriority, iAtX, iAtY, (int)eAIType, (int)eUnitFlags, iUnitStrength, iUnitStrengthTimes100,
            iMaxPath, pJoinUnit ? pJoinUnit->getID() : -1, szWorkCriteria.c_str());
    }

	//	First check that there are not already units on the way to meet this need
	//	else concurrent builds will get queued while they are in transit
	foreach_(const CvSelectionGroup * pLoopSelectionGroup, GET_PLAYER(m_eOwner).groups())
	{
		const CvPlot* pMissionPlot = pLoopSelectionGroup->AI_getMissionAIPlot();

		if (pMissionPlot == GC.getMap().plot(iAtX, iAtY) && !pLoopSelectionGroup->atPlot(pMissionPlot)
		&& pLoopSelectionGroup->AI_getMissionAIType() == (pJoinUnit == NULL ? MISSIONAI_CONTRACT : MISSIONAI_CONTRACT_UNIT)
		//	Allow for the last unit having died so that this group is about to vanish
		&& pLoopSelectionGroup->getNumUnits() > 0
		&& (eAIType == NO_UNITAI || pLoopSelectionGroup->getHeadUnitAI() == eAIType)
		&& pLoopSelectionGroup->meetsUnitSelectionCriteria(criteria))
		{
			std::map<int, bool>::const_iterator itr = m_contractedUnits.find(pLoopSelectionGroup->getID());

			if (itr == m_contractedUnits.end())
			{
				m_contractedUnits[pLoopSelectionGroup->getID()] = true;
				if (gUnitLogLevel > 2)
				{
					log(1,
                        "[CTB/work/intransit] unit=%S (%d) at=(%d,%d) already responding to contract at=(%d,%d)",
                        pLoopSelectionGroup->getHeadUnit()->getDescription().GetCString(),
                        pLoopSelectionGroup->getHeadUnit()->getID(),
                        pLoopSelectionGroup->getX(),
                        pLoopSelectionGroup->getY(),
                        iAtX, iAtY
                    );
				}

				if (iUnitStrengthTimes100 == -1)
				{
					// Request already handled by existing mission
					log(2, "[CTB/work/intransit] request at=(%d,%d) aiType=%d fully handled by in-transit group=%d (no strength requirement) - not adding",
                    	iAtX, iAtY, (int)eAIType, pLoopSelectionGroup->getID());
					return;
				}
				const int iMissionGroupStrengthTimes100 = pLoopSelectionGroup->AI_getGenericValueTimes100(unitCapabilities2UnitValueFlags(eUnitFlags));
				if (iMissionGroupStrengthTimes100 >= iUnitStrengthTimes100)
				{
					// Request is entirely fulfilled by existing mission
					log(2, "[CTB/work/intransit] request at=(%d,%d) aiType=%d fully covered by in-transit group=%d (groupStr=%d >= requiredStr=%d) - not adding",
                    	iAtX, iAtY, (int)eAIType, pLoopSelectionGroup->getID(), iMissionGroupStrengthTimes100, iUnitStrengthTimes100);
					return;
				}
				//	It's partially fulfilled so lower the priority of the remainder
				log(2, "[CTB/work/intransit] request at=(%d,%d) aiType=%d partially covered by in-transit group=%d (groupStr=%d < requiredStr=%d) - lowering priority %d and reducing remaining strength",
                	iAtX, iAtY, (int)eAIType, pLoopSelectionGroup->getID(), iMissionGroupStrengthTimes100, iUnitStrengthTimes100, iPriority);
				iPriority = lowerPartiallyFulfilledRequestPriority(iPriority, iUnitStrengthTimes100, iMissionGroupStrengthTimes100);
				iUnitStrengthTimes100 -= iMissionGroupStrengthTimes100;
			}
		}
	}
	workRequest newRequest;
	newRequest.iPriority = iPriority;
	newRequest.eUnitFlags = eUnitFlags;
	newRequest.eAIType = eAIType;
	newRequest.iAtX = iAtX;
	newRequest.iAtY = iAtY;
	newRequest.iMaxPath = iMaxPath;
	newRequest.iWorkRequestId = ++m_iNextWorkRequestId;
	newRequest.bFulfilled = false;
	newRequest.iRequiredStrengthTimes100 = iUnitStrengthTimes100;
	if (criteria != NULL)
	{
		newRequest.criteria = *criteria;
	}

	OutputDebugString(CvString::format("Adding new work request, index %d with priority %d\n", newRequest.iWorkRequestId, iPriority).c_str());
	log(1, "[CTB/work] added request index=%d priority=%d at=(%d,%d) aiType=%d flags=0x%x requiredStrx100=%d maxPath=%d join=%d",
        newRequest.iWorkRequestId, iPriority, iAtX, iAtY, (int)eAIType, (int)eUnitFlags,
        iUnitStrengthTimes100, iMaxPath, pJoinUnit ? pJoinUnit->getID() : -1);

	if (pJoinUnit == NULL)
	{
		newRequest.iUnitId = -1;
	}
	else
	{
		newRequest.iUnitId = pJoinUnit->getID();
	}

	//	Insert in priority order, highest first
	std::vector<workRequest>::iterator insertAt;

	for (insertAt = m_workRequests.begin(); insertAt != m_workRequests.end(); ++insertAt)
	{
		if (iPriority > (*insertAt).iPriority)
		{
			break;
		}
	}
	m_workRequests.insert(insertAt, newRequest);
}

//	Advertise a tender to build units
//		iMinPriority indicates the lowest priority request this tender is appropriate for
void CvContractBroker::advertiseTender(const CvCity* pCity, int iMinPriority)
{
	PROFILE_FUNC();

	int iNumTenders = 1; // par d�faut

	iNumTenders = 1 + (pCity->getPopulation() / 10) + (pCity->getYieldRate(YIELD_PRODUCTION) / 100);
	if (pCity->isCapital())
	{
		iNumTenders+=2;
	}

	
	iNumTenders = std::min(iNumTenders, 4); // max 6

    log(1, "[CTB/tender] city=%S (%d) advertising %d tender slot(s) for unit builds minPriority=%d (pop=%d prod=%d capital=%d)",
        pCity->getName().GetCString(), pCity->getID(), iNumTenders, iMinPriority,
        pCity->getPopulation(), pCity->getYieldRate(YIELD_PRODUCTION), pCity->isCapital() ? 1 : 0);

	for (int i = 0; i < iNumTenders; i++)
	{
		cityTender newTender;
		newTender.iMinPriority = iMinPriority;
		newTender.iCityId = pCity->getID();
		m_advertisingTenders.push_back(newTender);

		log(3, "[CTB/tender] city=%S (%d) tenders (slot %d/%d) for unit builds minPriority=%d totalTendersNow=%d",
        	pCity->getName().GetCString(), pCity->getID(), i + 1, iNumTenders, iMinPriority, m_advertisingTenders.size());
	}

}

//	Find out how many requests have already been made for units of a specified AI type
//	This is used by cities requesting globally needed units like settlers to avoid multiple
//	tenders all occurring at once
//	Find out how many requests have already been made for units of a specified AI type
//	This is used by cities requesting globally needed units like settlers to avoid multiple
//	tenders all occurring at once
int CvContractBroker::numRequestsOutstanding(UnitAITypes eUnitAI, bool bAtCityOnly, const CvPlot* pPlot) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	for (int iI = 0; iI < (int)m_workRequests.size(); iI++)
	{
		if (!m_workRequests[iI].bFulfilled)
		{
			if (m_workRequests[iI].eAIType == eUnitAI)
			{
				const CvPlot* pDestPlot = GC.getMap().plot(m_workRequests[iI].iAtX, m_workRequests[iI].iAtY);
				const CvCity* targetCity = pDestPlot->getPlotCity();

				if (!bAtCityOnly || (targetCity != NULL && targetCity->getOwner() == m_eOwner))
				{
					if (pPlot == NULL || pPlot == pDestPlot)
					{
						iCount++;
					}
				}
			}
		}
	}

    const_cast<CvContractBroker*>(this)->log(3, "[CTB/outstanding] aiType=%d atCityOnly=%d plot=(%d,%d) outstandingRequests=%d",
        (int)eUnitAI, bAtCityOnly ? 1 : 0,
        pPlot ? pPlot->getX() : -1, pPlot ? pPlot->getY() : -1, iCount);

	return iCount;
}

void CvContractBroker::finalizeTenderContracts()
{
	PROFILE_FUNC();

	std::map<int, int> tenderAllocations;

	for (int iI = 0; iI < (int)m_workRequests.size(); iI++)
	{
		if (!m_workRequests[iI].bFulfilled)
		{
			int iBestValue = 0;
			int iBestCityTenderKey = 0;
			UnitTypes eBestUnit = NO_UNIT;
			UnitAITypes eBestAIType = NO_UNITAI;
			CvCity* pBestCity = NULL;

			const CvUnit* pTargetUnit = findUnit(m_workRequests[iI].iUnitId);
			CvPlot* pDestPlot;

			if (pTargetUnit != NULL)
			{
				pDestPlot = pTargetUnit->plot();
			}
			else
			{
				pDestPlot = GC.getMap().plot(m_workRequests[iI].iAtX, m_workRequests[iI].iAtY);
			}

			if (gCityLogLevel >= 2)
			{
				CvString unitAIType = m_workRequests[iI].eAIType == NO_UNITAI ? "NO_UNITAI" : GC.getUnitAIInfo(m_workRequests[iI].eAIType).getType();

				CvString szCriteriaDescription = m_workRequests[iI].criteria.getDescription();

				if (!szCriteriaDescription.empty())
				{
					szCriteriaDescription.Format(" (%s)", szCriteriaDescription.c_str());
				}
				log(2,
                    "[CTB/tender/bids] processing bids workRequest=%d unitAI=%s at=(%d,%d) priority=%d flags=0x%x advertisingTenders=%d%s",
                    m_workRequests[iI].iWorkRequestId,
                    unitAIType.c_str(),
                    m_workRequests[iI].iAtX, m_workRequests[iI].iAtY,
                    m_workRequests[iI].iPriority,
                    (int)m_workRequests[iI].eUnitFlags,
                    m_advertisingTenders.size(),
                    szCriteriaDescription.c_str()
                );
			}

			for (unsigned int iJ = 0; iJ < m_advertisingTenders.size(); iJ++)
			{
				if (m_advertisingTenders[iJ].iMinPriority <= m_workRequests[iI].iPriority)
				{
					CvCity* pCity = GET_PLAYER(m_eOwner).getCity(m_advertisingTenders[iJ].iCityId);

					if (pCity != NULL && pDestPlot != NULL
					&& (pCity->area() == pDestPlot->area() || pDestPlot->getPlotCity() != NULL && pCity->waterArea() == pDestPlot->getPlotCity()->waterArea()))
					{
					    log(3, "[CTB/tender/cand] workRequest=%d tender[%d] city=%S (%d) considering (cityMinPriority=%d <= reqPriority=%d, area ok)",
                            m_workRequests[iI].iWorkRequestId, iJ,
                            pCity->getName().GetCString(), pCity->getID(),
                            m_advertisingTenders[iJ].iMinPriority, m_workRequests[iI].iPriority);
						int	iTendersAlreadyInProcess = pCity->numQueuedUnits(m_workRequests[iI].eAIType, pTargetUnit == NULL ? pDestPlot : NULL);
						int iTenderAllocationKey = 0;

						CvChecksum xSum;

						xSum.add(pCity->getID());

						if (pTargetUnit != NULL)
						{
							// Units move around, so can't use destination plot
							xSum.add(pTargetUnit->getID());
						}
						else
						{
							xSum.add(GC.getMap().plotNum(m_workRequests[iI].iAtX, m_workRequests[iI].iAtY));
						}
						xSum.add((int)m_workRequests[iI].eAIType);

						iTenderAllocationKey = xSum.get();

						std::map<int, int>::const_iterator itr = tenderAllocations.find(iTenderAllocationKey);
						if (itr != tenderAllocations.end())
						{
							iTendersAlreadyInProcess -= itr->second;
						}
						else
						{
							tenderAllocations[iTenderAllocationKey] = 0;
							log(3, "[CTB/tender/alloc] workRequest=%d city=%S (%d) allocKey=%d initialized to 0",
                            	m_workRequests[iI].iWorkRequestId, pCity->getName().GetCString(), pCity->getID(), iTenderAllocationKey);
						}

						FASSERT_NOT_NEGATIVE(iTendersAlreadyInProcess);

						log(3, "[CTB/tender/cand] workRequest=%d city=%S (%d) allocKey=%d tendersAlreadyInProcess=%d",
                            m_workRequests[iI].iWorkRequestId, pCity->getName().GetCString(), pCity->getID(),
                            iTenderAllocationKey, iTendersAlreadyInProcess);

						if (iTendersAlreadyInProcess <= 0)
						{
							int iValue = 0;
							UnitTypes eUnit = NO_UNIT;
							UnitAITypes eAIType = NO_UNITAI;

							if (m_workRequests[iI].eAIType == NO_UNITAI)
							{
								UnitAITypes* pUnitAIs = NULL;
								int iNumAIs = -1;

								if ((m_workRequests[iI].eUnitFlags & DEFENSIVE_UNITCAPABILITIES) != 0)
								{
									static UnitAITypes defensiveAIs[] = { UNITAI_CITY_DEFENSE, UNITAI_ATTACK, UNITAI_CITY_COUNTER, UNITAI_COUNTER };
									pUnitAIs = defensiveAIs;
									iNumAIs = sizeof(defensiveAIs) / sizeof(UnitAITypes);
								}
								if ((m_workRequests[iI].eUnitFlags & OFFENSIVE_UNITCAPABILITIES) != 0)
								{
									static UnitAITypes offensiveAIs[] = { UNITAI_ATTACK, UNITAI_ATTACK_CITY, UNITAI_COUNTER };
									pUnitAIs = offensiveAIs;
									iNumAIs = sizeof(offensiveAIs) / sizeof(UnitAITypes);
								}
								if ((m_workRequests[iI].eUnitFlags & WORKER_UNITCAPABILITIES) != 0)
								{
									static UnitAITypes workerAIs[] = { UNITAI_WORKER };
									pUnitAIs = workerAIs;
									iNumAIs = sizeof(workerAIs) / sizeof(UnitAITypes);
								}
								if ((m_workRequests[iI].eUnitFlags & HEALER_UNITCAPABILITIES) != 0)
								{
									static UnitAITypes workerAIs[] = { UNITAI_HEALER };
									pUnitAIs = workerAIs;
									iNumAIs = sizeof(workerAIs) / sizeof(UnitAITypes);
								}
								eUnit = pCity->AI_bestUnit(iValue, iNumAIs, pUnitAIs, false, &eAIType, false, true, &m_workRequests[iI].criteria);
							}
							else
							{
								eAIType = m_workRequests[iI].eAIType;

								if (pCity->area() != pDestPlot->area() && !IS_NAVAL_AITYPE(eAIType) && !IS_AIR_AITYPE(eAIType))
								{
								    log(3, "[CTB/tender/cand] workRequest=%d city=%S (%d) skipped (land unit aiType=%d cannot cross from city area to dest area)",
                                    	m_workRequests[iI].iWorkRequestId, pCity->getName().GetCString(), pCity->getID(), (int)eAIType);
									continue;
								}
								eUnit = pCity->AI_bestUnitAI(eAIType, iValue, false, false, &m_workRequests[iI].criteria);
							}
							if (eUnit != NO_UNIT)
							{
								int iBaseValue = iValue;

								// Adjust value for production time and distance
								const int iTurns =
									(
										pCity->isProduction() && pCity->getOrderData(0).eOrderType == ORDER_TRAIN
										?
										pCity->getTotalProductionQueueTurnsLeft() + pCity->getProductionTurnsLeft(eUnit, 1)
										:
										pCity->getProductionTurnsLeft(eUnit, 1)
									);
								iValue *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent();
								iValue /= iTurns;

								//if the nb of turns is too high for a small city, don't consider the unit, unless there are no other options
								if (iTurns > 20 && pCity->getPopulation() < 5 && m_advertisingTenders.size() > 3)
								{
									log(3,
                                        "[CTB/tender/giveup] workRequest=%d city=%S (%d) unit=%S baseValue=%d turns=%d depreciatedValue=%d (too slow for small city) - skipped",
                                        m_workRequests[iI].iWorkRequestId,
                                        pCity->getName().GetCString(),
                                        pCity->getID(),
                                        GC.getUnitInfo(eUnit).getDescription(),
                                        iBaseValue,
                                        iTurns,
                                        iValue
                                    );
									continue;
								}

								// generate the path to the destination, if it is too long the value of the unit drops. If no path, then can't supply the unit
								if (CvSelectionGroup::getPathGenerator()->generatePathForHypotheticalUnit(pCity->plot(), pDestPlot, m_eOwner, eUnit, MOVE_NO_ENEMY_TERRITORY, m_workRequests[iI].iMaxPath))
								{
									const int iDistance = CvSelectionGroup::getPathGenerator()->getLastPath().length();
									iValue /= (1 + intSqrt(iDistance));

                                    log(3,
										"[CTB/tender/bid] workRequest=%d city=%S (%d) unit=%S aiType=%d baseValue=%d turns=%d distance=%d depreciatedValue=%d (prevBest=%d)",
										m_workRequests[iI].iWorkRequestId,
										pCity->getName().GetCString(),
										pCity->getID(),
										GC.getUnitInfo(eUnit).getDescription(),
										(int)eAIType,
										iBaseValue,
										iTurns,
										iDistance,
										iValue,
										iBestValue
									);

									if (iValue > iBestValue)
									{
										iBestValue = iValue;
										iBestCityTenderKey = iTenderAllocationKey;
										eBestUnit = eUnit;
										eBestAIType = eAIType;
										pBestCity = pCity;
										log(3, "[CTB/tender/bid] workRequest=%d city=%S (%d) is new best bid value=%d unit=%S",
                                        	m_workRequests[iI].iWorkRequestId, pCity->getName().GetCString(), pCity->getID(),
                                        	iValue, GC.getUnitInfo(eUnit).getDescription());
									}
								}
								else
                                {
                                log(3, "[CTB/tender/cand] workRequest=%d city=%S (%d) unit=%S has no usable path to dest (maxPath=%d) - cannot supply",
                                    m_workRequests[iI].iWorkRequestId, pCity->getName().GetCString(), pCity->getID(),
                                    GC.getUnitInfo(eUnit).getDescription(), m_workRequests[iI].iMaxPath);
                               }
							}
							else
							{
                                log(3, "[CTB/tender/nosuit] workRequest=%d city=%S (%d) has no suitable unit to offer for aiType=%d",
                                    m_workRequests[iI].iWorkRequestId, pCity->getName().GetCString(), pCity->getID(), (int)m_workRequests[iI].eAIType);
                            }
						}
						else // Already being built
						{
							m_workRequests[iI].bFulfilled = true;
							eBestUnit = NO_UNIT;

							tenderAllocations[iTenderAllocationKey] += 1;

							log(2, "[CTB/tender/already] city=%S (%d) is already building a unit for this request (tendersInProcess=%d)",
                                pCity->getName().GetCString(), pCity->getID(), iTendersAlreadyInProcess);
                            log(2, "[CTB/fulfilled] workRequest=%d marked fulfilled - city=%S (%d) already building required unit",
                                m_workRequests[iI].iWorkRequestId, pCity->getName().GetCString(), pCity->getID());
                            log(3, "[CTB/tender/alloc] workRequest=%d allocKey=%d incremented to %d (already-building)",
                                m_workRequests[iI].iWorkRequestId, iTenderAllocationKey, tenderAllocations[iTenderAllocationKey]);
							break;
						}
					}
					else if (gPlayerLogLevel >= 3)
                    {
                        CvCity* pSkipCity = GET_PLAYER(m_eOwner).getCity(m_advertisingTenders[iJ].iCityId);
                        log(3, "[CTB/tender/cand] workRequest=%d tender[%d] city=%S (%d) skipped (city=%s destPlot=%s wrong-area/null)",
                            m_workRequests[iI].iWorkRequestId, iJ,
                            pSkipCity ? pSkipCity->getName().GetCString() : L"<null>",
                            m_advertisingTenders[iJ].iCityId,
                            pSkipCity ? "ok" : "null",
                            pDestPlot ? "ok" : "null");
                    }
				}
				else
                {
                    log(3, "[CTB/tender/cand] workRequest=%d tender[%d] city=%d skipped (cityMinPriority=%d > reqPriority=%d)",
                        m_workRequests[iI].iWorkRequestId, iJ, m_advertisingTenders[iJ].iCityId,
                        m_advertisingTenders[iJ].iMinPriority, m_workRequests[iI].iPriority);
                }
			}

			if (eBestUnit != NO_UNIT)
			{
				if (gPlayerLogLevel >= 1)
				{
					CvString unitAIType = m_workRequests[iI].eAIType == NO_UNITAI ? "NO_UNITAI" : GC.getUnitAIInfo(m_workRequests[iI].eAIType).getType();

					if (pBestCity != NULL)
					{
						log(1, "[CTB/tender/win] workRequest=%d city=%S (%d) wins business for unitAI build %s (training %S) value=%d",
                            m_workRequests[iI].iWorkRequestId,
                            pBestCity->getName().GetCString(),
                            pBestCity->getID(),
                            unitAIType.c_str(),
                            GC.getUnitInfo(eBestUnit).getDescription(),
                            iBestValue);
					}
					else
					{
						log(1, "[CTB/tender/none] workRequest=%d problem - no city wins business for unitAI build %s (training %S)",
                            m_workRequests[iI].iWorkRequestId,
                            unitAIType.c_str(),
                            GC.getUnitInfo(eBestUnit).getDescription());
					}
				}
				m_workRequests[iI].bFulfilled = true;
				tenderAllocations[iBestCityTenderKey] += 1;

				log(2, "[CTB/fulfilled] workRequest=%d marked fulfilled - tender won, city will build the unit", m_workRequests[iI].iWorkRequestId);
                    log(3, "[CTB/tender/alloc] workRequest=%d allocKey=%d incremented to %d (tender won)",
                        m_workRequests[iI].iWorkRequestId, iBestCityTenderKey, tenderAllocations[iBestCityTenderKey]);

				// Queue up the build. Add to queue head if the current build is not a unit,
				//	implies a local build below the priority of work the city tendered for.
				const bool bDanger = pBestCity->AI_isDanger();
				const int iHammerCostPercent = GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent();
				//if a production is nearly finished, don't insert a unit, add it to the queue.
				const int iMaxTurntoLeave = (bDanger && pBestCity->getProductionUnit() == NO_UNIT ? 1 + GC.getGame().getGameSpeedType() / 4 : 1 + iHammerCostPercent / 50);
				const bool bNearlyFinished = (pBestCity->getProductionTurnsLeft() <= iMaxTurntoLeave || (pBestCity->getProductionTurnsLeft()+3) <= pBestCity->getProductionTurnsLeft(eBestUnit, 1));
				bool bAppend = (pBestCity->isProduction() && pBestCity->getOrderData(0).eOrderType == ORDER_TRAIN) || bNearlyFinished;

				pBestCity->pushOrder(
					ORDER_TRAIN,
					eBestUnit, eBestAIType,
					false, !bAppend, bAppend, false,
					pDestPlot, m_workRequests[iI].eAIType,
					(m_workRequests[iI].iUnitId == -1 ? 0 : AUX_CONTRACT_FLAG_IS_UNIT_CONTRACT)
				);

				log(1, "[CTB/tender/build] city=%S (%d) queued ORDER_TRAIN unit=%S unitAI=%d for workRequest=%d at=(%d,%d) append=%d danger=%d",
                    pBestCity->getName().GetCString(), pBestCity->getID(),
                    GC.getUnitInfo(eBestUnit).getDescription(), (int)eBestAIType,
                    m_workRequests[iI].iWorkRequestId,
                    m_workRequests[iI].iAtX, m_workRequests[iI].iAtY,
                    bAppend ? 1 : 0, bDanger ? 1 : 0);
			}
		}
	}

	if (gPlayerLogLevel >= 1)
	{
		int	iIdleUnits = 0;
		int iEmployedUnits = 0;
		int	iSatisfiedContracts = 0;

		for (int iI = 0; iI < (int)m_workRequests.size(); iI++)
		{
			if (m_workRequests[iI].bFulfilled)
			{
				iSatisfiedContracts++;
			}
		}

		log(1, "[CTB/finalize] contractsSatisfied=%d/%d unitsEmployed=%d unitsWithoutWork=%d", iSatisfiedContracts, m_workRequests.size(), m_iEmployedUnits, m_advertisingUnits.size());
	}
}

// Make a contract
//	This will attempt to make the best contracts between currently advertising units and work,
//	then search the resulting set for the work of the requested unit
//	returns true if a contract is made along with the details of what to do
bool CvContractBroker::makeContract(CvUnit* pUnit, int& iAtX, int& iAtY, CvUnit*& pJoinUnit, bool bThisPlotOnly)
{
	PROFILE_FUNC();

	// Satisfy the highest priority requests first (sort order of m_workRequests)
	for (int iI = 0; iI < (int)m_workRequests.size(); iI++)
	{
		if (!m_workRequests[iI].bFulfilled)
		{
			// If this is a request to join a unit check the unit is still in a joinable state
			//	ie - it has not joined someone else!
			if (m_workRequests[iI].iUnitId != -1)
			{
				const CvUnit* pTargetUnit = findUnit(m_workRequests[iI].iUnitId);

				if (pTargetUnit == NULL || pTargetUnit->getGroup()->getHeadUnit() != pTargetUnit)
				{
				    log(1, "[CTB/match/abandon] workRequest=%d (join unit=%d) no longer joinable, marking fulfilled",
                        m_workRequests[iI].iWorkRequestId, m_workRequests[iI].iUnitId);
                    log(2, "[CTB/fulfilled] workRequest=%d marked fulfilled - join target unit=%d gone or no longer group head (abandoned)",
                        m_workRequests[iI].iWorkRequestId, m_workRequests[iI].iUnitId);
					m_workRequests[iI].bFulfilled = true;
					continue;
				}
			}
			bool bFound;
			do
			{
				bFound = false;
				advertisingUnit* suitableUnit = findBestUnit(m_workRequests[iI], bThisPlotOnly);

				if (NULL != suitableUnit)
				{
					CvUnit* unitX = findUnit(suitableUnit->iUnitId);

					if (unitX != NULL)
					{
						bFound = true;

						suitableUnit->iContractedWorkRequest = m_workRequests[iI].iWorkRequestId;

						log(3, "[CTB/match] unit=%d assigned to workRequest=%d (contracted)", suitableUnit->iUnitId, m_workRequests[iI].iWorkRequestId);

						const int iUnitStrengthTimes100 = std::max(1, unitX->AI_genericUnitValueTimes100(unitCapabilities2UnitValueFlags(m_workRequests[iI].eUnitFlags)));
						if (m_workRequests[iI].iRequiredStrengthTimes100 == -1 || iUnitStrengthTimes100 >= m_workRequests[iI].iRequiredStrengthTimes100)
						{
							//	Request is entirely fulfilled by this unit
							m_workRequests[iI].bFulfilled = true;

							log(1, "[CTB/match] workRequest=%d satisfied by unit=%d (strength %d >= required %d)", m_workRequests[iI].iWorkRequestId, suitableUnit->iUnitId, iUnitStrengthTimes100, m_workRequests[iI].iRequiredStrengthTimes100);
                            log(2, "[CTB/fulfilled] workRequest=%d marked fulfilled - unit=%d fully satisfies it", m_workRequests[iI].iWorkRequestId, suitableUnit->iUnitId);
							OutputDebugString(CvString::format("work request %d satisfied by unit %d\n", m_workRequests[iI].iWorkRequestId, suitableUnit->iUnitId).c_str());
						}
						else
						{
							if (m_workRequests[iI].iRequiredStrengthTimes100 < 1)
							{
								m_workRequests[iI].iRequiredStrengthTimes100 = 1;
							}
							// It's partially fulfilled so lower the priority of the remainder
							m_workRequests[iI].iPriority = lowerPartiallyFulfilledRequestPriority(m_workRequests[iI].iPriority, m_workRequests[iI].iRequiredStrengthTimes100, iUnitStrengthTimes100);
							m_workRequests[iI].iRequiredStrengthTimes100 -= iUnitStrengthTimes100;

							// SanityCheck needed!
							if (m_workRequests[iI].iRequiredStrengthTimes100 > (iUnitStrengthTimes100 * 10))
							{
								m_workRequests[iI].iRequiredStrengthTimes100 = iUnitStrengthTimes100;
							}

							log(1, "[CTB/match/partial] workRequest=%d partially satisfied by unit=%d (providedStr=%d, remainingRequiredStr=%d, newPriority=%d)",
                                m_workRequests[iI].iWorkRequestId, suitableUnit->iUnitId,
                                iUnitStrengthTimes100, m_workRequests[iI].iRequiredStrengthTimes100, m_workRequests[iI].iPriority);

							OutputDebugString(CvString::format("work request %d partially satisfied by unit %d\n", m_workRequests[iI].iWorkRequestId, suitableUnit->iUnitId).c_str());
						}
					}
				}
			} while (bFound && !m_workRequests[iI].bFulfilled);
		}
	}

	for (int iI = 0; iI < (int)m_advertisingUnits.size(); iI++)
	{
		// Note that all existing advertising units have attempted to match against existing work requests
		if (bThisPlotOnly)
		{
			m_advertisingUnits[iI].iMatchedToRequestSeqThisPlot = m_iNextWorkRequestId;
		}
		else
		{
			m_advertisingUnits[iI].iMatchedToRequestSeqAnyPlot = m_iNextWorkRequestId;
		}
	}

	// Now see if this unit has work assigned
	for (int iI = 0; iI < (int)m_advertisingUnits.size(); iI++)
	{
		if (m_advertisingUnits[iI].iUnitId == pUnit->getID())
		{
			const int iWorkRequest = m_advertisingUnits[iI].iContractedWorkRequest;

			if (-1 != iWorkRequest)
			{
				const workRequest* contractedRequest = findWorkRequest(iWorkRequest);
				if(contractedRequest == NULL)
				{
				    log(1, "[CTB/contract/lost] unit=%d contracted workRequest=%d no longer exists, releasing",
                    	pUnit->getID(), iWorkRequest);
					m_advertisingUnits[iI].iContractedWorkRequest = -1;
					return false;
				}
				FAssert(NULL != contractedRequest);

				iAtX = contractedRequest->iAtX;
				iAtY = contractedRequest->iAtY;

				pJoinUnit = findUnit(contractedRequest->iUnitId);
				FAssert(NULL != pJoinUnit);
				// [CTB/contract] -- the broker matches a unit to a work request (the
                // key decision: this unit is dispatched to satisfy that need).
                log(1, "[CTB/contract] unit=%d -> work at (%d,%d) priority=%d aiType=%d joinUnit=%d (workRequest=%d)",
                    pUnit->getID(), iAtX, iAtY, contractedRequest->iPriority, (int)contractedRequest->eAIType,
                    pJoinUnit ? pJoinUnit->getID() : -1, iWorkRequest);
				return true;
			}
			log(3, "[CTB/contract] unit=%d has no contracted work this pass (no match found)", pUnit->getID());
			return false;
		}
	}
	log(3, "[CTB/contract] unit=%d not present in advertising list (no contract)", pUnit->getID());
	return false;
}

const workRequest* CvContractBroker::findWorkRequest(int iWorkRequestId) const
{
	PROFILE_FUNC();

	for (int iI = 0; iI < (int)m_workRequests.size(); iI++)
	{
		if (m_workRequests[iI].iWorkRequestId == iWorkRequestId)
		{
			return &m_workRequests[iI];
		}
	}
	return NULL;
}

workRequest* CvContractBroker::findWorkRequestByUnitId(int unitId)
{
	PROFILE_FUNC();

	for (int iI = 0; iI < (int)m_workRequests.size(); iI++)
	{
		if (m_workRequests[iI].iUnitId == unitId)
		{
			return &m_workRequests[iI];
		}
	}
	return NULL;
}

advertisingUnit* CvContractBroker::findBestUnit(const workRequest& request, bool bThisPlotOnly)
{
	PROFILE_FUNC();

	int	iBestValue = -1;
	int iBestUnitIndex = -1;

	for (int iI = 0; iI < (int)m_advertisingUnits.size(); iI++)
	{
		advertisingUnit& unitInfo = m_advertisingUnits[iI];

		if (unitInfo.iUnitId < 0) continue;
		// Don't bother recalculating this advertiser/requestor pair if they have already been calculated previously
		if ((bThisPlotOnly ? unitInfo.iMatchedToRequestSeqThisPlot : unitInfo.iMatchedToRequestSeqAnyPlot) < request.iWorkRequestId
		&& unitInfo.iContractedWorkRequest == -1)
		{
			const CvUnit* unitX = findUnit(unitInfo.iUnitId);
			if (unitX == NULL || request.eAIType != NO_UNITAI && unitX->AI_getUnitAIType() != request.eAIType)
			{
			    log(3, "[CTB/assess] unit=%d rejected for workRequest=%d (%s)",
                    unitInfo.iUnitId, request.iWorkRequestId,
                    unitX == NULL ? "unit gone" : "wrong unitAI type");
				continue;
			}

			if (unitX->meetsUnitSelectionCriteria(&request.criteria) && unitInfo.iMinPriority <= request.iPriority)
			{
				int	iValue = 1;

				if ((request.eUnitFlags & WORKER_UNITCAPABILITIES) == 0 || (request.eUnitFlags & HEALER_UNITCAPABILITIES) == 0)
				{
					if (request.eAIType == NO_UNITAI || unitX->AI_getUnitAIType() == request.eAIType)
					{
						iValue += 10;

						if (unitInfo.iDefensiveValue > 0 && (request.eUnitFlags == 0 || (request.eUnitFlags & DEFENSIVE_UNITCAPABILITIES) != 0))
						{
							iValue += unitInfo.iDefensiveValue;
						}
						if (unitInfo.iOffensiveValue > 0 && (request.eUnitFlags == 0 || (request.eUnitFlags & OFFENSIVE_UNITCAPABILITIES) != 0))
						{
							iValue += unitInfo.iOffensiveValue;
						}
					}
				}
				else if (unitInfo.bIsWorker)
				{
					iValue = 100;
				}
				else if (unitInfo.bIsHealer)
				{
					iValue = 100;
				}

				iValue *= 1000;

				if (iValue > iBestValue)
				{
					const CvUnit* pTargetUnit = findUnit(request.iUnitId);
					CvPlot* pTargetPlot;

					if (pTargetUnit != NULL)
					{
						pTargetPlot = pTargetUnit->plot();
					}
					else
					{
						pTargetPlot = GC.getMap().plot(request.iAtX, request.iAtY);
					}
					int iPathTurns = 0;
					int iMaxPathTurns = std::min((request.iPriority > LOW_PRIORITY_ESCORT_PRIORITY ? MAX_INT : 10), (iBestValue < 1 ? MAX_INT : 5 * iValue / iBestValue));

					iValue = applyDistanceScoringFactor(iValue, unitX->plot(), pTargetPlot, 1);

					if (request.iMaxPath < iMaxPathTurns)
					{
						iMaxPathTurns = request.iMaxPath;
					}

					// For low priority work never try to satisfy it with a distant unit
					if (unitX->atPlot(pTargetPlot)
					|| !bThisPlotOnly
					&& unitX->generatePath(pTargetPlot, MOVE_SAFE_TERRITORY | MOVE_AVOID_ENEMY_UNITS, true, &iPathTurns, iMaxPathTurns))
					{
						//iValue /= (iPathTurns + 1);

						if (iValue > iBestValue)
						{
							iBestValue = iValue;
							iBestUnitIndex = iI;

							log(3, "[CTB/assess] unit=%d is new best for workRequest=%d iValue=%d pathTurns=%d",
                            	unitInfo.iUnitId, request.iWorkRequestId, iValue, iPathTurns);
						}
					}
					else
                    {
                        log(3, "[CTB/assess] unit=%d rejected for workRequest=%d (no path to (%d,%d) within maxPathTurns=%d, thisPlotOnly=%d)",
                            unitInfo.iUnitId, request.iWorkRequestId, pTargetPlot->getX(), pTargetPlot->getY(), iMaxPathTurns, bThisPlotOnly ? 1 : 0);
                    }
				}
				OutputDebugString(CvString::format("Assessed unit %d suitability for work request %d (iValue = %d)\n", unitInfo.iUnitId, request.iWorkRequestId, iValue).c_str());
				log(3, "[CTB/assess] unit=%d suitability for workRequest=%d iValue=%d (currentBest=%d)", unitInfo.iUnitId, request.iWorkRequestId, iValue, iBestValue);
			}
			else if (gPlayerLogLevel >= 3)
            {
                log(3, "[CTB/assess] unit=%d rejected for workRequest=%d (%s)",
                    unitInfo.iUnitId, request.iWorkRequestId,
                    !unitX->meetsUnitSelectionCriteria(&request.criteria) ? "fails selection criteria" : "unit minPriority above request priority");
            }
		}
	}

	if (iBestUnitIndex > -1)
	{
	    log(1, "[CTB/assess] unit=%d chosen for workRequest=%d index=%d bestValue=%d",
        	m_advertisingUnits[iBestUnitIndex].iUnitId, request.iWorkRequestId, iBestUnitIndex, iBestValue);
		return &m_advertisingUnits[iBestUnitIndex];
	}

	log(3, "[CTB/assess] no suitable unit found for workRequest=%d among %d advertisers", request.iWorkRequestId, m_advertisingUnits.size());
	return NULL;
}

CvUnit* CvContractBroker::findUnit(int iUnitId) const
{
	if (iUnitId == -1)
	{
		return NULL;
	}
	return GET_PLAYER((PlayerTypes)m_eOwner).getUnit(iUnitId);
}

int	CvContractBroker::lowerPartiallyFulfilledRequestPriority(int iPreviousPriority, int iPreviousRequestStrength, int iStrengthProvided) const
{
	return (iPreviousPriority * (iPreviousRequestStrength - iStrengthProvided)) / iPreviousRequestStrength;
}

UnitValueFlags CvContractBroker::unitCapabilities2UnitValueFlags(unitCapabilities eCapabilities) const
{
	UnitValueFlags	valueFlags = (UnitValueFlags)0;

	if ((eCapabilities & DEFENSIVE_UNITCAPABILITIES) != 0)
	{
		valueFlags |= UNITVALUE_FLAGS_DEFENSIVE;
	}
	if ((eCapabilities & OFFENSIVE_UNITCAPABILITIES) != 0)
	{
		valueFlags |= UNITVALUE_FLAGS_OFFENSIVE;
	}

	if (valueFlags == 0)
	{
		valueFlags = UNITVALUE_FLAGS_ALL;
	}

	return valueFlags;
}

void CvContractBroker::internalRemoveUnit(const int unitId)
{
	for (int iI = 0; iI < (int)m_advertisingUnits.size(); iI++)
	{
		if (m_advertisingUnits[iI].iUnitId == unitId)
		{
			advertisingUnit* aUnit = &m_advertisingUnits[iI];
			const int iContracted = aUnit->iContractedWorkRequest;
			m_advertisingUnits.erase(aUnit);

			m_iEmployedUnits++;
			log(2, "[CTB/remove] unit=%d removed from worklist (contractedWorkRequest=%d) -> employedUnits=%d unitsRemainingInWorklist=%d",
            	unitId, iContracted, m_iEmployedUnits, m_advertisingUnits.size());
			break;
		}
	}

}


void CvContractBroker::postProcessUnitsLookingForWork()
{
	PROFILE_EXTRA_FUNC();

	log(2, "[CTB/postprocess] processing contracts for %d advertising unit(s)", m_advertisingUnits.size());

	for (int iI = 0; iI < (int)m_advertisingUnits.size(); iI++)
	{
		CvUnit* unitX = findUnit(m_advertisingUnits[iI].iUnitId);

		if (unitX)
		{
			log(3, "[CTB/postprocess] unit=%d contractedWorkRequest=%d -> processContracts()",
				m_advertisingUnits[iI].iUnitId, m_advertisingUnits[iI].iContractedWorkRequest);
			unitX->getGroup()->getHeadUnit()->processContracts();
		}
		else
		{
			log(3, "[CTB/postprocess] unit=%d no longer exists, skipping", m_advertisingUnits[iI].iUnitId);
		}
	}
}

