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
#include "CvOutcome.h"
#include "CvOutcomeList.h"
#include "CvPlot.h"
#include "CvUnit.h"
#include "CvXMLLoadUtility.h"
#include "CheckSum.h"

CvOutcomeList::~CvOutcomeList()
{
	clear();
}

CvOutcome* CvOutcomeList::getOutcome(int index) const
{
	FASSERT_BOUNDS(0, getNumOutcomes(), index)
	return m_aOutcome[index];
}

int CvOutcomeList::getNumOutcomes() const
{
	return m_aOutcome.size();
}

bool CvOutcomeList::isPossible(const CvUnit &kUnit) const
{
	const int iNum = getNumOutcomes();
	if (iNum <= 0)
		return false;

	for (int i=0; i<iNum; i++)
	{
		if (m_aOutcome[i]->isPossible(kUnit))
		{
			return true;
		}
	}

	return false;
}

bool CvOutcomeList::isPossibleSomewhere(const CvUnit &kUnit) const
{
	const int iNum = getNumOutcomes();
	if (iNum <= 0)
		return false;

	for (int i=0; i<iNum; i++)
	{
		if (m_aOutcome[i]->isPossibleSomewhere(kUnit))
		{
			return true;
		}
	}

	return false;
}

bool CvOutcomeList::isPossibleInPlot(const CvUnit &kUnit, const CvPlot& kPlot, bool bForTrade) const
{
	const int iNum = getNumOutcomes();
	if (iNum <= 0)
		return false;

	for (int i=0; i<iNum; i++)
	{
		if (m_aOutcome[i]->isPossibleInPlot(kUnit, kPlot, bForTrade))
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
		for (int i=0; i<(int)m_aOutcome.size(); i++)
		{
			SAFE_DELETE(m_aOutcome[i]);
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

bool CvOutcomeList::execute(CvUnit &kUnit, PlayerTypes eDefeatedUnitPlayer, UnitTypes eDefeatedUnitType)
{
	PROFILE_FUNC();

	std::vector<std::pair<CvOutcome*, int> > apOutcome;
	std::set<OutcomeTypes> aeReplacedOutcomes;
	int iChanceSum = 0;
	for (int i=0; i<getNumOutcomes(); i++)
	{
		CvOutcome* pOutcome = getOutcome(i);
		if (pOutcome->isPossible(kUnit))
		{
			const int iChance = pOutcome->getChance(kUnit);
			iChanceSum += iChance;
			apOutcome.push_back(std::pair<CvOutcome*,int>(pOutcome, iChance));
			insertReplaceOutcomesRecursive(aeReplacedOutcomes, pOutcome->getType());
		}
	}

	for (int i=(int)apOutcome.size()-1; i>=0; i--)
	{
		if (aeReplacedOutcomes.find(apOutcome[i].first->getType()) != aeReplacedOutcomes.end())
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

	for (int i=0; i<(int)apOutcome.size(); i++)
	{
		iChanceSum += apOutcome[i].second;
		if (iRoll < iChanceSum)
		{
			apOutcome[i].first->execute(kUnit, eDefeatedUnitPlayer, eDefeatedUnitType);
			return true;
		}
	}

	// Nothing happens
	return false;
}

int CvOutcomeList::AI_getValueInPlot(const CvUnit& kUnit, const CvPlot& kPlot, bool bForTrade) const
{
	std::vector<std::pair<CvOutcome*, int> > apOutcome;
	std::set<OutcomeTypes> aeReplacedOutcomes;
	int iChanceSum = 0;
	for (int i=0; i<getNumOutcomes(); i++)
	{
		CvOutcome* pOutcome = getOutcome(i);
		if (pOutcome->isPossibleInPlot(kUnit, kPlot, bForTrade))
		{
			const int iChance = pOutcome->getChance(kUnit);
			iChanceSum += iChance;
			apOutcome.push_back(std::pair<CvOutcome*,int>(pOutcome, iChance));
			insertReplaceOutcomesRecursive(aeReplacedOutcomes, pOutcome->getType());
		}
	}

	for (int i=(int)apOutcome.size()-1; i>=0; i--)
	{
		if (aeReplacedOutcomes.find(apOutcome[i].first->getType()) != aeReplacedOutcomes.end())
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

	for (int i=0; i<(int)apOutcome.size(); i++)
	{
		iValue += ((100 * apOutcome[i].second)/iChanceSum) * apOutcome[i].first->AI_getValueInPlot(kUnit, kPlot, bForTrade);
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
					m_aOutcome.push_back(new CvOutcome());
					m_aOutcome[m_aOutcome.size()-1]->read(pXML);
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
		const int num = pOutcomeList->getNumOutcomes();
		for (int index = 0; index < num; index++)
		{
			m_aOutcome.push_back(pOutcomeList->getOutcome(index));
		}
		pOutcomeList->m_aOutcome.clear();
	}
}

void CvOutcomeList::getCheckSum(uint32_t& iSum) const
{
	const int num = getNumOutcomes();
	for (int index = 0; index < num; index++)
	{
		m_aOutcome[index]->getCheckSum(iSum);
	}
}

void CvOutcomeList::buildDisplayString(CvWStringBuffer& szBuffer, const CvUnit& kUnit) const
{
	std::vector<std::pair<const CvOutcome*, int> > apOutcome;
	std::set<OutcomeTypes> aeReplacedOutcomes;
	int iChanceSum = 0;

	for (int i=0; i<getNumOutcomes(); i++)
	{
		const CvOutcome* pOutcome = getOutcome(i);
		if (pOutcome->isPossible(kUnit))
		{
			const int iChance = pOutcome->getChance(kUnit);
			iChanceSum += iChance;
			apOutcome.push_back(std::pair<const CvOutcome*,int>(pOutcome, iChance));
			insertReplaceOutcomesRecursive(aeReplacedOutcomes, pOutcome->getType());
		}
	}

	for (int i=(int)apOutcome.size()-1; i>=0; i--)
	{
		if (aeReplacedOutcomes.find(apOutcome[i].first->getType()) != aeReplacedOutcomes.end())
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

	for (int i=0; i<(int)apOutcome.size(); i++)
	{
		szBuffer.append(NEWLINE);
		CvWString s;
		s.Format(L"%d%%: ",(100 * apOutcome[i].second)/iChanceSum);
		szBuffer.append(s);
		apOutcome[i].first->buildDisplayString(szBuffer, kUnit);
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
		for (int i=0; i < pList->getNumOutcomes(); i++)
		{
			CvOutcome* pOutcome = pList->getOutcome(i);
			m_aOutcome.push_back(pOutcome);
			m_setTypes.insert(pOutcome->getType());
		}
	}
	else
	{
		// add all outcomes of the other list which use outcome types that we have not seen yet
		for (int i=0; i < pList->getNumOutcomes(); i++)
		{
			CvOutcome* pOutcome = pList->getOutcome(i);
			if (m_setTypes.count(pOutcome->getType()) == 0)
			{
				m_aOutcome.push_back(pOutcome);
				m_setTypes.insert(pOutcome->getType());
			}
		}
	}
}