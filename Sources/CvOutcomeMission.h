//------------------------------------------------------------------------------------------------
//
//  FILE:    CvOutcomeMission.h
//
//  PURPOSE: A mission that has a cost and a result depending on an outcome list
//
//------------------------------------------------------------------------------------------------
#pragma once

#ifndef CV_OUTCOME_MISSION_H
#define CV_OUTCOME_MISSION_H

class CvOutcomeList;
class CvProperties;
class CvUnit;
class CvXMLLoadUtility;
class BoolExpr;
class IntExpr;

class CvOutcomeMission
{
public:
	CvOutcomeMission();
	~CvOutcomeMission();
	MissionTypes getMission() const;
	const CvOutcomeList* getOutcomeList() const;
	const CvProperties* getPropertyCost() const;
	bool isKill() const;
//	IntExpr* getCost();
	GameObjectTypes getPayerType() const;

	bool isPossible(const CvUnit* pUnit, bool bTestVisible = false) const;
	void buildDisplayString(CvWStringBuffer& szBuffer, CvUnit* pUnit);
	void execute(CvUnit* pUnit);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvOutcomeMission* pOutcomeMission);

	void getCheckSum(unsigned int& iSum) const;

protected:
	MissionTypes m_eMission;
	CvOutcomeList m_OutcomeList;
	CvProperties m_PropertyCost;
	GameObjectTypes m_ePayerType;
	bool m_bKill;
	IntExpr* m_iCost;
	BoolExpr* m_pPlotCondition;
	BoolExpr* m_pUnitCondition;
};

#endif