#include "BaseDefenseHandler.hpp"
#include "CvPlayerAI.h"
#include "CvCity.h"



void BaseDefenseHandler::requestReinforcements(int priority, int number)
{
    int counter = 0;
    do {
        GET_PLAYER(m_pCity->getOwner()).getContractBroker().advertiseWork(priority, DEFENSIVE_UNITCAPABILITIES, m_pCity->getX(), m_pCity->getY(), NULL, NO_UNITAI, -1, NULL, 10);
        counter++;
    } while (counter < number);
}