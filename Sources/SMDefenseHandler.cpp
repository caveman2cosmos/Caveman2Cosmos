#include "CvGameCoreDLL.h"
#include "SMDefenseHandler.hpp"
#include "CvCity.h"

SMDefenseHandler::SMDefenseHandler(CvCity* city)
{
    m_pCity = city;
    if (m_pCity != NULL)
    {
        m_pPlot = m_pCity->plot();
    }
}

bool SMDefenseHandler::evaluateDefenses()
{
    return false;
}

int SMDefenseHandler::getDefenseScore()
{
    return 0;
}
