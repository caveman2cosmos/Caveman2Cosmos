#pragma once


#ifndef CvCityDefenseHandler__h__
#define CvCityDefenseHandler__h__


class CvUnitInfo;


struct DefensiveNeed {
	CvUnitInfo* unitType;
	unsigned short priority;
	unsigned short amount;
	unsigned short size;
};

class CvCityDefenseHandler {
public:
	CvCityDefenseHandler();
	CvCityDefenseHandler(CvCity* city);
	void calculateNeeds();

	void evaluateDefences();

	int countUnits() const;

	void requestReinforcements(int priority, int number);
	unsigned short getThreatLevel() const;
	void adjustThreatLevel(unsigned short level = 0);

private:
	CvCity* m_pCity;
	CvPlot* m_pPlot;
	unsigned short m_iThreatLevel;
	int EvaluateSuitability(CvUnit* pUnit);
};

#endif