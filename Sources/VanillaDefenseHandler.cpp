#include "CvGameCoreDLL.h"
#include "CvCity.h"
#include "VanillaDefenseHandler.hpp"

VanillaDefenseHandler::VanillaDefenseHandler(CvCity* city)
{
    m_pCity = city;
    if (m_pCity != NULL)
    {
        m_pPlot = m_pCity->plot();
    }
}

bool VanillaDefenseHandler::evaluateDefenses()
{
    return false;
}

int VanillaDefenseHandler::getDefenseScore()
{
    return 0;
}