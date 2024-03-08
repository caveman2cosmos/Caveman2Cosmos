#pragma once

#ifndef SMDefenseHandler_hpp
#define SMDefenseHandler_hpp

#include "IDefenseHandler.hpp"
#include "BaseDefenseHandler.hpp"
#include "CvCity.h"

class SMDefenseHandler : public BaseDefenseHandler
{
public:
    SMDefenseHandler(CvCity* city);
    bool evaluateDefenses();
    int getDefenseScore();

private:
    CvCity* m_pCity;
    CvPlot* m_pPlot;
};


#endif