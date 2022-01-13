#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvOutcomeList.h
//
//  PURPOSE: A list of possible outcomes with a relative chance
//
//------------------------------------------------------------------------------------------------
#ifndef CV_OUTCOME_LIST_H
#define CV_OUTCOME_LIST_H

class CvPlot;
class CvOutcome;
class CvUnit;
class CvXMLLoadUtility;

class CvOutcomeList
{
public:
	CvOutcomeList() : m_bIsReference(false) {}
	virtual ~CvOutcomeList();
	const CvOutcome* getOutcome(int index) const;
	int getNumOutcomes() const;

	bool isPossible(const CvUnit& kUnit) const;
	bool isPossibleSomewhere(const CvUnit& kUnit) const;
	bool isPossibleInPlot(const CvUnit& kUnit, const CvPlot& kPlot, bool bForTrade = false) const;
	bool execute(CvUnit& kUnit, PlayerTypes eDefeatedUnitPlayer = NO_PLAYER, UnitTypes eDefeatedUnitType = NO_UNIT) const;

	int AI_getValueInPlot(const CvUnit& kUnit, const CvPlot& kPlot, bool bForTrade = false) const;

	bool isEmpty() const;
	void clear();

	void buildDisplayString(CvWStringBuffer& szBuffer, const CvUnit& kUnit) const;

	bool read(CvXMLLoadUtility* pXML, const wchar_t* szTagName = L"Outcomes");
	void copyNonDefaults(CvOutcomeList* pOutcomeList);
	void getCheckSum(uint32_t& iSum) const;

protected:
	bool m_bIsReference;
	std::vector<const CvOutcome*> m_aOutcome;
};

// This outcome list version is supposed to contain a temporary merge of different outcome lists
class CvOutcomeListMerged : public CvOutcomeList
{
public:
	CvOutcomeListMerged() { m_bIsReference = true; }
	void addOutcomeList(const CvOutcomeList* pList);

protected:
	std::set<OutcomeTypes> m_setTypes;
};

#endif