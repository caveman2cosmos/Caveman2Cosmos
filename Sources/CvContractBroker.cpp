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

	logContractBroker(level, "<%S> - %S", m_ownerName, logString);
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

	logContractBroker(1, "Cleaning Up <%S> ContractBroker Contracted Units: %d Advertising Tenders: %d Advertising Units: %d",
		GET_PLAYER(m_eOwner).getName(),
		m_contractedUnits.size(),
		m_advertisingTenders.size(),
		m_advertisingUnits.size());

	logContractBroker(1, "     <%S>Current Work requests: %d", m_ownerName, m_workRequests.size());
	logContractBroker(1, "     <%S>Tenders posted last turn: %d", m_ownerName, m_advertisingTenders.size());


	m_contractedUnits.clear();
	m_advertisingTenders.clear();
	m_iEmployedUnits = 0;
	for (unsigned int iI = 0; iI < (int)m_workRequests.size(); iI++)
	{
		if (m_workRequests[iI].bFulfilled)
		{
			workRequest* wr = &m_workRequests[iI];
			m_workRequests.erase(wr);
			fulfilledContracts++;
		}

	}
	logContractBroker(1, "     <%S>Fulfilled Contracts last turn: %d", m_ownerName, fulfilledContracts);


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

			logContractBroker(1,
				"      Unit %S (%d) at (%d,%d) already exists in worklist",
				pUnit->getDescription().GetCString(),
				pUnit->getID(),
				pUnit->getX(),
				pUnit->getY());

			logContractBroker(1,
				"      Number of units for <%S> Remaining in worklist: [%d]",
				GET_PLAYER(m_eOwner).getName(),
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

	logContractBroker(1,
	"      Unit %S (%d) at (%d,%d) is asking for work",
	pUnit->getDescription().GetCString(),
	pUnit->getID(),
	pUnit->getX(),
	pUnit->getY());

	logContractBroker(1,
	"      Number of units Remaining in worklist: [%d]",
	m_advertisingUnits.size()
	);

}

//	Unit fulfilled its work and is no longer advertising as available
void CvContractBroker::removeUnit(const CvUnit* pUnit)
{
	PROFILE_EXTRA_FUNC();

	internalRemoveUnit(pUnit->getID());

	logContractBroker(1,
		"      Unit %S (%d) at (%d,%d) has been removed from worklist",
		pUnit->getDescription().GetCString(),
		pUnit->getID(),
		pUnit->getX(),
		pUnit->getY());


}

void CvContractBroker::removeUnit(const int iUnitId)
{
	PROFILE_EXTRA_FUNC();

	internalRemoveUnit(iUnitId);

	logContractBroker(1, "      Unit (%d) has been removed from worklist", iUnitId);
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
		if (wr != NULL) return;
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
					logContractBroker(1,
						"      Unit %S (%d) at (%d,%d) already responding to contract at (%d,%d)",
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
					return;
				}
				const int iMissionGroupStrengthTimes100 = pLoopSelectionGroup->AI_getGenericValueTimes100(unitCapabilities2UnitValueFlags(eUnitFlags));
				if (iMissionGroupStrengthTimes100 >= iUnitStrengthTimes100)
				{
					// Request is entirely fulfilled by existing mission
					return;
				}
				//	It's partially fulfilled so lower the priority of the remainder
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
	logContractBroker(1, "Adding new work request, index %d with priority %d\n", newRequest.iWorkRequestId, iPriority);

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

	int iNumTenders = 1; // par défaut

	iNumTenders = 1 + (pCity->getPopulation() / 10) + (pCity->getYieldRate(YIELD_PRODUCTION) / 100);
	if (pCity->isCapital())
	{
		iNumTenders+=2;
	}

	
	iNumTenders = std::min(iNumTenders, 4); // max 6


	for (int i = 0; i < iNumTenders; i++)
	{
		cityTender newTender;
		newTender.iMinPriority = iMinPriority;
		newTender.iCityId = pCity->getID();
		m_advertisingTenders.push_back(newTender);

		if (gCityLogLevel >= 3)
			logContractBroker(1, "      City %S tenders (slot %d/%d) for unit builds at priority %d",
				pCity->getName().GetCString(), i + 1, iNumTenders, iMinPriority);
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

			if (gCityLogLevel >= 3)
			{
				CvString unitAIType = m_workRequests[iI].eAIType == NO_UNITAI ? "NO_UNITAI" : GC.getUnitAIInfo(m_workRequests[iI].eAIType).getType();

				CvString szCriteriaDescription = m_workRequests[iI].criteria.getDescription();

				if (!szCriteriaDescription.empty())
				{
					szCriteriaDescription.Format(" (%s)", szCriteriaDescription.c_str());
				}
				logContractBroker(1,
					"      Processing bids for tender for unitAI %s at (%d,%d) with priority %d%s",
					unitAIType.c_str(),
					m_workRequests[iI].iAtX, m_workRequests[iI].iAtY,
					m_workRequests[iI].iPriority,
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
						}

						FASSERT_NOT_NEGATIVE(iTendersAlreadyInProcess);

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
									if (gCityLogLevel >= 3)
									{
										logContractBroker(1,
											"      City %S give up for production of unit %S with base value %d, depreciated value (%d turn production) to %d",
											pCity->getName().GetCString(),
											GC.getUnitInfo(eUnit).getDescription(),
											iBaseValue,
											iTurns,
											iValue
										);
									}
									continue;
								}

								// generate the path to the destination, if it is too long the value of the unit drops. If no path, then can't supply the unit
								if (CvSelectionGroup::getPathGenerator()->generatePathForHypotheticalUnit(pCity->plot(), pDestPlot, m_eOwner, eUnit, MOVE_NO_ENEMY_TERRITORY, m_workRequests[iI].iMaxPath))
								{
									const int iDistance = CvSelectionGroup::getPathGenerator()->getLastPath().length();
									iValue /= (1 + intSqrt(iDistance));

									if (gCityLogLevel >= 3)
									{
										logContractBroker(1,
											"      City %S could supply unit %S with base value %d, depreciated value (after %d turn production at distance %d) to %d",
											pCity->getName().GetCString(),
											GC.getUnitInfo(eUnit).getDescription(),
											iBaseValue,
											iTurns,
											iDistance,
											iValue
										);
									}

									if (iValue > iBestValue)
									{
										iBestValue = iValue;
										iBestCityTenderKey = iTenderAllocationKey;
										eBestUnit = eUnit;
										eBestAIType = eAIType;
										pBestCity = pCity;
									}
								}
							}
							else if (gCityLogLevel >= 3)
							{
								logContractBroker(1, "      City %S has no suitable units to offer", pCity->getName().GetCString());
							}
						}
						else // Already being built
						{
							m_workRequests[iI].bFulfilled = true;
							eBestUnit = NO_UNIT;

							tenderAllocations[iTenderAllocationKey] += 1;

							if (gCityLogLevel >= 3)
							{
								logContractBroker(1, "      City %S is already building a unit", pCity->getName().GetCString());
							}
							break;
						}
					}
				}
			}

			if (eBestUnit != NO_UNIT)
			{
				if (gCityLogLevel >= 2)
				{
					CvString unitAIType = m_workRequests[iI].eAIType == NO_UNITAI ? "NO_UNITAI" : GC.getUnitAIInfo(m_workRequests[iI].eAIType).getType();

					if (pBestCity != NULL)
					{
						logContractBroker(1, "      <%S>City %S wins business for unitAI build %s (training %S)",
								m_ownerName,
								pBestCity->getName().GetCString(),
								unitAIType.c_str(),
								GC.getUnitInfo(eBestUnit).getDescription());
					}
					else
					{
						logContractBroker(1, "      <%S>Problem - no city wins business for unitAI build %s (training %S)",
								m_ownerName,
								unitAIType.c_str(),
								GC.getUnitInfo(eBestUnit).getDescription());
					}
				}
				m_workRequests[iI].bFulfilled = true;
				tenderAllocations[iBestCityTenderKey] += 1;

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
			}
		}
	}

	if (gUnitLogLevel >= 3)
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

		logContractBroker(1, "%d out of %d contracts satisfied, %d unit employed, %d left without work", iSatisfiedContracts, m_workRequests.size(), m_iEmployedUnits, m_advertisingUnits.size());
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

						const int iUnitStrengthTimes100 = std::max(1, unitX->AI_genericUnitValueTimes100(unitCapabilities2UnitValueFlags(m_workRequests[iI].eUnitFlags)));
						if (m_workRequests[iI].iRequiredStrengthTimes100 == -1 || iUnitStrengthTimes100 >= m_workRequests[iI].iRequiredStrengthTimes100)
						{
							//	Request is entirely fulfilled by this unit
							m_workRequests[iI].bFulfilled = true;

							logContractBroker(1, "     <%S>work request %d satisfied by unit %d (%d > %d)", m_ownerName, m_workRequests[iI].iWorkRequestId, suitableUnit->iUnitId, iUnitStrengthTimes100, m_workRequests[iI].iRequiredStrengthTimes100);
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

							logContractBroker(1, "     <%S>work request %d partially satisfied by unit %d", m_ownerName, m_workRequests[iI].iWorkRequestId, suitableUnit->iUnitId);

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
					m_advertisingUnits[iI].iContractedWorkRequest = -1;
					return false;
				}
				FAssert(NULL != contractedRequest);

				iAtX = contractedRequest->iAtX;
				iAtY = contractedRequest->iAtY;

				pJoinUnit = findUnit(contractedRequest->iUnitId);
				FAssert(NULL != pJoinUnit);
				return true;
			}
			return false;
		}
	}
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
						}
					}
				}
				OutputDebugString(CvString::format("Assessed unit %d suitability for work request %d (iValue = %d)\n", unitInfo.iUnitId, request.iWorkRequestId, iValue).c_str());
				logContractBroker(1, "     <%S> Assessed unit % d suitability for work request% d(iValue = % d)", m_ownerName, unitInfo.iUnitId, request.iWorkRequestId, iValue);
			}
		}
	}

	if (iBestUnitIndex > -1)
	{
		logContractBroker(1, "     <%S> unit found for contract with id %d, index %d", m_ownerName, request.iWorkRequestId, iBestUnitIndex);
		return &m_advertisingUnits[iBestUnitIndex];
	}
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
			m_advertisingUnits.erase(aUnit);

			logContractBroker(1, "     <%S> Number of units Remaining in worklist: [%d]", m_ownerName, m_advertisingUnits.size());
			m_iEmployedUnits++;
			break;
		}
	}

}


void CvContractBroker::postProcessUnitsLookingForWork()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < (int)m_advertisingUnits.size(); iI++)
	{
		CvUnit* unitX = findUnit(m_advertisingUnits[iI].iUnitId);

		if (unitX)
		{
			unitX->getGroup()->getHeadUnit()->processContracts();
		}
	}
}

