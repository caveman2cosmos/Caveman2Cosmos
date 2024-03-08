#pragma once

#ifndef BaseDefenseHandler_hpp
#define BaseDefenseHandler_hpp


#include "IDefenseHandler.hpp"
#include "CvCity.h"


class BaseDefenseHandler : public IDefenseHandler
{
public:
    void requestReinforcements(int priority, int number);
private:
    CvCity* m_pCity;
    CvPlot* m_pPlot;
};


#endif
