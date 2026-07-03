#pragma once

#ifndef BETTERBTSAI_H
#define BETTERBTSAI_H

#define ENABLE_AI_LOGS

#ifdef ENABLE_AI_LOGS

#define LOG_BBAI_CITY(level, msg) \
        do { if (gCityLogLevel >= (level)) logBBAI msg; } while(0)

#define LOG_BBAI_UNIT(level, msg) \
        do { if (gUnitLogLevel >= (level)) logBBAI msg; } while(0)

#define LOG_BBAI_TEAM(level, msg) \
        do { if (gTeamLogLevel >= (level)) logBBAI msg; } while(0)

#define LOG_BBAI_PLAYER(level, msg) \
        do { if (gPlayerLogLevel >= (level)) logBBAI msg ; } while(0)

#define LOG_EVALAI_CITY(level, msg) \
        do { if (gCityLogLevel >= (level)) logAiEvaluations msg; } while(0)

#define LOG_EVALAI_UNIT(level, msg) \
        do { if (gUnitLogLevel >= (level)) logAiEvaluations msg; } while(0)

#define LOG_EVALAI_TEAM(level, msg) \
        do { if (gTeamLogLevel >= (level)) logAiEvaluations msg; } while(0)

#define LOG_EVALAI_PLAYER(level, msg) \
        do { if (gPlayerLogLevel >= (level)) logAiEvaluations msg; } while(0)

#define LOG_CITY_BLOCK(level, CODE) \
    do { if (gCityLogLevel >= (level)) { CODE } } while(0)

#define LOG_UNIT_BLOCK(level, CODE) \
    do { if (gUnitLogLevel >= (level)) { CODE } } while(0)

#define LOG_TEAM_BLOCK(level, CODE) \
    do { if (gTeamLogLevel >= (level)) { CODE } } while(0)

#define LOG_PLAYER_BLOCK(level, CODE) \
    do { if (gPlayerLogLevel >= (level)) { CODE } } while(0)


#else
// Si les logs sont désactivés → macros vides, coût CPU nul
#define LOG_BBAI_CITY(level, msg)   do {} while(0)
#define LOG_BBAI_UNIT(level, msg)   do {} while(0)
#define LOG_BBAI_TEAM(level, msg)   do {} while(0)
#define LOG_BBAI_PLAYER(level, msg)   do {} while(0)

#define LOG_EVALAI_CITY(level, msg)   do {} while(0)
#define LOG_EVALAI_UNIT(level, msg)   do {} while(0)
#define LOG_EVALAI_TEAM(level, msg)   do {} while(0)
#define LOG_EVALAI_PLAYER(level, msg)   do {} while(0)


#define LOG_PLAYER_BLOCK(level, CODE) do {} while(0)
#define LOG_UNIT_BLOCK(level, CODE) do {} while(0)
#define LOG_TEAM_BLOCK(level, CODE) do {} while(0)
#define LOG_CITY_BLOCK(level, CODE) do {} while(0)

#endif



extern int gPlayerLogLevel;
extern int gTeamLogLevel;
extern int gCityLogLevel;
extern int gUnitLogLevel;

void logBBAI(const char* format, ...);
void logAIJson(CvWString type, CvWString identifier, CvWString squirrel, CvWString message);
void logCB(CvString message);
void logToFile(CvString message, const char* filename);
void logContractBroker(int level, const char* format, ...);
void logAiEvaluations(int level, const char* format, ...);
#endif