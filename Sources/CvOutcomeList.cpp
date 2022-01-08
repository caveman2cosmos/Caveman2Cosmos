//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvOutcomeList.cpp
//
//  PURPOSE: A list of possible outcomes with a relative chance
//
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvOutcome.h"
#include "CvOutcomeList.h"
#include "CvPlot.h"
#include "CvUnit.h"
#include "CvXMLLoadUtility.h"
#include "CheckSum.h"

typedef std::pair<const CvOutcome*, int> PossibleOutcome;


CvOutcomeList::~CvOutcomeList()
{
	clear();
}

bool CvOutcomeList::isPossible(const CvUnit &kUnit) const
{
	foreach_(const CvOutcome* pOutcome, m_aOutcome)
	{
		if (pOutcome->isPossible(kUnit))
		{
			return true;
		}
	}

	return false;
}

bool CvOutcomeList::isPossibleSomewhere(const CvUnit &kUnit) const
{
	foreach_(const CvOutcome* pOutcome, m_aOutcome)
	{
		if (pOutcome->isPossibleSomewhere(kUnit))
		{
			return true;
		}
	}

	return false;
}

bool CvOutcomeList::isPossibleInPlot(const CvUnit &kUnit, const CvPlot& kPlot, bool bForTrade) const
{
	foreach_(const CvOutcome* pOutcome, m_aOutcome)
	{
		if (pOutcome->isPossibleInPlot(kUnit, kPlot, bForTrade))
		{
			return true;
		}
	}

	return false;
}

bool CvOutcomeList::isEmpty() const
{
	return m_aOutcome.empty();
}

void CvOutcomeList::clear()
{
	if (!m_bIsReference)
	{
		foreach_(const CvOutcome* pOutcome, m_aOutcome)
		{
			SAFE_DELETE(pOutcome);
		}
	}
	m_aOutcome.clear();
}

void insertReplaceOutcomesRecursive(std::set<OutcomeTypes>& aeReplacedOutcomes, OutcomeTypes eOutcome)
{
	foreach_(const OutcomeTypes eReplOutcome, GC.getOutcomeInfo(eOutcome).getReplaceOutcomes())
	{
		aeReplacedOutcomes.insert(eReplOutcome);
		insertReplaceOutcomesRecursive(aeReplacedOutcomes, eReplOutcome);
	}
}

bool CvOutcomeList::execute(CvUnit &kUnit, PlayerTypes eDefeatedUnitPlayer, UnitTypes eDefeatedUnitType) const
{
	PROFILE_FUNC();

	std::vector<PossibleOutcome> apOutcome;
	std::set<OutcomeTypes> aeReplacedOutcomes;
	int iChanceSum = 0;

	foreach_(const CvOutcome* pOutcome, m_aOutcome)
	{
		if (pOutcome->isPossible(kUnit))
		{
			const int iChance = pOutcome->getChance(kUnit);
			iChanceSum += iChance;
			apOutcome.push_back(PossibleOutcome(pOutcome, iChance));
			insertReplaceOutcomesRecursive(aeReplacedOutcomes, pOutcome->getType());
		}
	}

	for (int i=(int)apOutcome.size()-1; i>=0; i--)
	{
		if (algo::any_of_equal(aeReplacedOutcomes, apOutcome[i].first->getType()))
		{
			iChanceSum -= apOutcome[i].second;
			apOutcome.erase(apOutcome.begin()+i);
		}
	}


	if (iChanceSum <= 0)
	{
		return false;
	}

	// Use absolute percentages when chance sum is smaller than 100
	if (iChanceSum < 100)
	{
		iChanceSum = 100;
	}

	const int iRoll = GC.getGame().getSorenRandNum( iChanceSum, "Outcome roll");
	iChanceSum = 0;

	foreach_(const PossibleOutcome& pOutcome, apOutcome)
	{
		iChanceSum += pOutcome.second;
		if (iRoll < iChanceSum)
		{
			pOutcome.first->execute(kUnit, eDefeatedUnitPlayer, eDefeatedUnitType);
			return true;
		}
	}

	// Nothing happens
	return false;
}

int CvOutcomeList::AI_getValueInPlot(const CvUnit& kUnit, const CvPlot& kPlot, bool bForTrade) const
{
	std::vector<PossibleOutcome> apOutcome;
	std::set<OutcomeTypes> aeReplacedOutcomes;
	int iChanceSum = 0;

	foreach_(const CvOutcome* pOutcome, m_aOutcome)
	{
		if (pOutcome->isPossibleInPlot(kUnit, kPlot, bForTrade))
		{
			const int iChance = pOutcome->getChance(kUnit);
			iChanceSum += iChance;
			apOutcome.push_back(PossibleOutcome(pOutcome, iChance));
			insertReplaceOutcomesRecursive(aeReplacedOutcomes, pOutcome->getType());
		}
	}

	for (int i=(int)apOutcome.size()-1; i>=0; i--)
	{
		if (algo::any_of_equal(aeReplacedOutcomes, apOutcome[i].first->getType()))
		{
			iChanceSum -= apOutcome[i].second;
			apOutcome.erase(apOutcome.begin()+i);
		}
	}

	if (iChanceSum <= 0)
	{
		return 0;
	}

	// Use absolute percentages when chance sum is smaller than 100
	if (iChanceSum < 100)
	{
		iChanceSum = 100;
	}

	int iValue = 0;

	foreach_(const PossibleOutcome& pOutcome, apOutcome)
	{
		iValue += ((100 * pOutcome.second)/iChanceSum) * pOutcome.first->AI_getValueInPlot(kUnit, kPlot, bForTrade);
	}

	return iValue / 100;
}

bool CvOutcomeList::read(CvXMLLoadUtility* pXML, const wchar_t* szTagName)
{
	if(pXML->TryMoveToXmlFirstChild(szTagName))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"Outcome"))
			{
				do
				{
					CvOutcome* pOutcome = new CvOutcome();
					pOutcome->read(pXML);
					m_aOutcome.push_back(pOutcome);
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	return true;
}

void CvOutcomeList::copyNonDefaults(CvOutcomeList* pOutcomeList)
{
	if (isEmpty())
	{
		foreach_(const CvOutcome* pOutcome, pOutcomeList->getOutcomes())
		{
			m_aOutcome.push_back(pOutcome);
		}
		pOutcomeList->m_aOutcome.clear();
	}
}

void CvOutcomeList::getCheckSum(uint32_t& iSum) const
{
	foreach_(const CvOutcome* pOutcome, m_aOutcome)
	{
		pOutcome->getCheckSum(iSum);
	}
}

void CvOutcomeList::buildDisplayString(CvWStringBuffer& szBuffer, const CvUnit& kUnit) const
{
	std::vector<PossibleOutcome> apOutcome;
	std::set<OutcomeTypes> aeReplacedOutcomes;
	int iChanceSum = 0;

	foreach_(const CvOutcome* pOutcome, m_aOutcome)
	{
		if (pOutcome->isPossible(kUnit))
		{
			const int iChance = pOutcome->getChance(kUnit);
			iChanceSum += iChance;
			apOutcome.push_back(PossibleOutcome(pOutcome, iChance));
			insertReplaceOutcomesRecursive(aeReplacedOutcomes, pOutcome->getType());
		}
	}

	for (int i=(int)apOutcome.size()-1; i>=0; i--)
	{
		if (algo::any_of_equal(aeReplacedOutcomes, apOutcome[i].first->getType()))
		{
			iChanceSum -= apOutcome[i].second;
			apOutcome.erase(apOutcome.begin()+i);
		}
	}

	if (iChanceSum <= 0)
	{
		return;
	}

	// Use absolute percentages when chance sum is smaller than 100
	if (iChanceSum < 100)
	{
		iChanceSum = 100;
	}

	foreach_(const PossibleOutcome& pOutcome, apOutcome)
	{
		szBuffer.append(NEWLINE);
		CvWString s;
		s.Format(L"%d%%: ", (100 * pOutcome.second)/iChanceSum);
		szBuffer.append(s);
		pOutcome.first->buildDisplayString(szBuffer, kUnit);
	}
}

void CvOutcomeListMerged::addOutcomeList(const CvOutcomeList* pList)
{
	if ((pList == NULL) || (pList->isEmpty()))
	{
		// nothing to do
		return;
	}

	if (isEmpty())
	{
		// just copy all outcomes and store the outcome types in the set
		foreach_(const CvOutcome* pOutcome, pList->getOutcomes())
		{
			m_aOutcome.push_back(pOutcome);
			m_setTypes.insert(pOutcome->getType());
		}
	}
	else
	{
		// add all outcomes of the other list which use outcome types that we have not seen yet
		foreach_(const CvOutcome* pOutcome, pList->getOutcomes())
		{
			if (m_setTypes.count(pOutcome->getType()) == 0)
			{
				m_aOutcome.push_back(pOutcome);
				m_setTypes.insert(pOutcome->getType());
			}
		}
	}
}