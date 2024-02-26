#pragma once

#ifndef DefenseHandler_hpp
#define DefenseHandler_hpp

#include "IDefenseHandler.hpp"
#include "CvCity.h"

class DefenseHandler : public IDefenseHandler
{
public:
    DefenseHandler(CvCity* city);

    bool evaluateDefenses();
    int getDefenseScore();

private:
    CvCity* m_pCity;
    CvPlot* m_pPlot;
};


#endif