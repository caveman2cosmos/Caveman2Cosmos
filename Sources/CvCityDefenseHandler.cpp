#include "cvGameCoreDll.h"
#include "CvCityDefenseHandler.hpp"
#include "CvCity.h"
#include "CvCityAI.h"
#include "CvPlayerAI.h"
#include "CvGlobals.h"
#include "BetterBTSAI.h"


CvCityDefenseHandler::CvCityDefenseHandler()
{}

CvCityDefenseHandler::CvCityDefenseHandler(CvCity* city)
{
	m_pCity = city;
	if (m_pCity != NULL)
	{
		m_pPlot = m_pCity->plot();
	}

	m_iThreatLevel = 0;
}

void CvCityDefenseHandler::calculateNeeds()
{
	const int citySize = m_pCity->getPopulation();
	evaluateDefences();
	requestReinforcements(800, 10);
}

void CvCityDefenseHandler::evaluateDefences()
{
	//int defensiveUnitValue = 0;
	//int unitAmount = 0;
	//int totalstrength = 0; // this is wildly irrelevant?
	//int averageStrength = 0;
	//int dangerCount = m_pCity->plot()->getDangerCount(m_pCity->getOwner());

	const int units = countUnits();

	if (units <= 4)
	{
		requestReinforcements(800, 5 - units);
	}


	// get strength of rivals at peace

	// get strength of rivals at war (with distance to enemy border? capital?)

	// get current units in city

	// get amount of enemy units in 5 tile radius, and their strength

	// remove wild animals from the equation

	// calculate balance between defence and offence
}

int CvCityDefenseHandler::countUnits() const
{
	if (m_pCity->plot() != NULL)
	{
		const int counter = algo::count_if(m_pCity->plot()->units(), bind(CvUnit::isSmUnit, _1) == true);


		logCityDefense(1, "City %s has %d units", m_pCity->getName().c_str(), counter);
		return counter;
	}

	logCityDefense(1, "unitcounter not found %s", m_pCity->getName().c_str());
	return 5;
}

void CvCityDefenseHandler::requestReinforcements(int priority, int number)
{
	int counter = 0;
	do {
		GET_PLAYER(m_pCity->getOwner()).getContractBroker().advertiseWork(priority, DEFENSIVE_UNITCAPABILITIES, m_pCity->getX(), m_pCity->getY(), NULL, NO_UNITAI, -1, NULL, 10);
		counter++;
	} while (counter < number);

}

unsigned short CvCityDefenseHandler::getThreatLevel() const
{
	return m_iThreatLevel;
}

void CvCityDefenseHandler::adjustThreatLevel(unsigned short level)
{
	m_iThreatLevel = level;
}

int CvCityDefenseHandler::EvaluateSuitability(CvUnit* pUnit)
{
	return 0;
}