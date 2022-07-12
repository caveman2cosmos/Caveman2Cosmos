#pragma once

#ifndef BETTERBTSAI_H
#define BETTERBTSAI_H

extern int gPlayerLogLevel;
extern int gTeamLogLevel;
extern int gCityLogLevel;
extern int gUnitLogLevel;

void logBBAI(char* format, ... );
void logAIJson(CvWString type, CvWString identifier, CvWString squirrel, CvWString message);
void logCB(CvString message);
void logToFile(CvString message, char* filename);

#endif