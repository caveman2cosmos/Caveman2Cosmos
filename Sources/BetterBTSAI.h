#pragma once

#ifndef BETTERBTSAI_H
#define BETTERBTSAI_H

// Log-verbosity globals, shared by the per-subsystem tagged logs
// (CvWorkerAI [WAI/*] -> BuildEvaluation.log, CvHunterAI [HAI/*] -> HunterAI.log,
// CvDecisionAI [DAI/*] -> DecisionAI.log, ContractBroker -> ContractBroker.log).
// Set from BUG options in CvGlobals; see logDecisionAI et al. below.
extern int gPlayerLogLevel;
extern int gTeamLogLevel;
extern int gCityLogLevel;
extern int gUnitLogLevel;

void logAIJson(CvWString type, CvWString identifier, CvWString squirrel, CvWString message);
void logCB(CvString message);
void logToFile(CvString message, const char* filename);
void logContractBroker(int level, const char* format, ...);
void logBuildEvaluation(int level, const char* format, ...);
void logHunterAI(int level, const char* format, ...);
void logDecisionAI(int level, const char* format, ...);
void logDiploAI(int level, const char* format, ...);
void logWarAI(int level, const char* format, ...);
void logUnitAI(int level, const char* format, ...);
void logCityAI(int level, const char* format, ...);
void logGroupAI(int level, const char* format, ...);
void logEspionageAI(int level, const char* format, ...);
void logFoundAI(int level, const char* format, ...);
void logGameInfo(const char* format, ...);
void logCombatAI(int level, const char* format, ...);
#endif