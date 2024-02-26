#pragma once

#ifndef SMDefenseHandler_hpp
#define SMDefenseHandler_hpp

#include "IDefenseHandler.hpp"

class SMDefenseHandler : public IDefenseHandler
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