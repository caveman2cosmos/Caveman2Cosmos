#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvOutcome.h
//
//  PURPOSE: A single outcome from an outcome list
//
//------------------------------------------------------------------------------------------------
#ifndef CV_OUTCOME_H
#define CV_OUTCOME_H

class CvPlayerAI;
class CvPlot;
class CvProperties;
class CvUnit;
class CvXMLLoadUtility;
class BoolExpr;
class IntExpr;

class CvOutcome
{
public:
	CvOutcome();
	virtual ~CvOutcome();
	int getYield(YieldTypes eYield, const CvUnit& kUnit) const;
	int getCommerce(CommerceTypes eCommerce, const CvUnit& kUnit) const;
	int getChance(const CvUnit& kUnit) const;
	OutcomeTypes getType() const;
	UnitTypes getUnitType() const;
	bool getUnitToCity(const CvUnit& kUnit) const;
	PromotionTypes getPromotionType() const;
	int getGPP() const;
	UnitTypes getGPUnitType() const;
	BonusTypes getBonusType() const;
	const CvProperties* getProperties() const;
	int getHappinessTimer() const;
	int getPopulationBoost() const;
	int getReduceAnarchyLength(const CvUnit& kUnit) const;
	EventTriggerTypes getEventTrigger() const;
	int getChancePerPop() const;
	bool isKill() const;
	void compilePython();

	bool isPossible(const CvUnit& kUnit) const;
	bool isPossibleSomewhere(const CvUnit& kUnit) const;
	bool isPossibleInPlot(const CvUnit& kUnit, const CvPlot& kPlot, bool bForTrade = false) const;
	bool isPossible(const CvPlayerAI& kPlayer) const;
	bool execute(CvUnit& kUnit, PlayerTypes eDefeatedUnitPlayer = NO_PLAYER, UnitTypes eDefeatedUnitType = NO_UNIT) const;

	int AI_getValueInPlot(const CvUnit& kUnit, const CvPlot& kPlot, bool bForTrade = false) const;

	void buildDisplayString(CvWStringBuffer& szBuffer, const CvUnit& kUnit) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvOutcome* pOutcome);
	void getCheckSum(uint32_t& iSum) const;

protected:
	OutcomeTypes m_eType;
	const IntExpr* m_iChance;
	int m_iChancePerPop;
	const IntExpr* m_aiYield[NUM_YIELD_TYPES];
	const IntExpr* m_aiCommerce[NUM_COMMERCE_TYPES];
	UnitTypes m_eUnitType;
	const BoolExpr* m_bUnitToCity;
	PromotionTypes m_ePromotionType;
	BonusTypes m_eBonusType;
	int m_iGPP;
	UnitTypes m_eGPUnitType;
	CvProperties m_Properties;
	int m_iHappinessTimer;
	int m_iPopulationBoost;
	const IntExpr* m_iReduceAnarchyLength;
	EventTriggerTypes m_eEventTrigger;
	const BoolExpr* m_pPlotCondition;
	const BoolExpr* m_pUnitCondition;
	CvString m_szPythonCallback;
	bool m_bKill;
	CvString m_szPythonCode;
	CvString m_szPythonModuleName;
	PyObject* m_pPythonPossibleFunc;
	PyObject* m_pPythonExecFunc;
	PyObject* m_pPythonDisplayFunc;
	PyObject* m_pPythonAIFunc;
};

#endif