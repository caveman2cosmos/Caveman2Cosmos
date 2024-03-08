#pragma once

#ifndef VanillaDefenseHandler_hpp
#define VanillaDefenseHandler_hpp

#include "BaseDefenseHandler.hpp"
#include "CvCity.h"

class VanillaDefenseHandler : public BaseDefenseHandler
{
public:
    VanillaDefenseHandler(CvCity* city);
    bool evaluateDefenses();
    int getDefenseScore();

private:
    CvCity* m_pCity;
    CvPlot* m_pPlot;
};


#endif