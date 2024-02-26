#pragma once

#ifndef IDefenseHandler_hpp
#define IDefenseHandler_hpp

class IDefenseHandler
{
public:
    virtual bool evaluateDefenses() = 0;
    virtual int getDefenseScore() = 0;
    virtual ~IDefenseHandler() {}
};

#endif