#include "CvGameCoreDLL.h"
#include "CvCity.h"
#include "DefenseHandler.hpp"

DefenseHandler::DefenseHandler(CvCity* city)
{
    m_pCity = city;
    if (m_pCity != NULL)
    {
        m_pPlot = m_pCity->plot();
    }
}

bool DefenseHandler::evaluateDefenses()
{
    return false;
}

int DefenseHandler::getDefenseScore()
{
    return 0;
}
