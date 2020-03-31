#pragma once

#ifndef __CY_Replay_H__
#define __CY_Replay_H__

#include "CvReplayInfo.h"

class CyReplayInfo
{
public:
	CyReplayInfo();
	CyReplayInfo(CvReplayInfo* pInfo);
	const CvReplayInfo* getReplayInfo() const { return m_pHoF; }
	bool isNone() { return (NULL == m_pHoF); }

	int getActivePlayer() const;
	int getLeader(int iPlayer) const;
	int getColor(int iPlayer) const;
	int getDifficulty() const;
	std::wstring getLeaderName() const;
	std::wstring getShortCivDescription() const;
	std::wstring getMapScriptName() const;
	int getWorldSize() const;
	int getClimate() const;
	int getSeaLevel() const;
	int getEra() const;
	int getGameSpeed() const;
	int getVictoryType() const;
	bool isMultiplayer() const;

	int getNumPlayers() const;
	int getPlayerScore(int iPlayer, int iTurn) const;

	int getNormalizedScore() const;

	int getReplayMessageTurn(int i) const;
	const std::wstring getReplayMessageText(int i) const;
	int getNumReplayMessages() const;

	int getStartYear() const;
	int getFinalTurn() const;
	const std::wstring getFinalDate() const;
	int getCalendar() const;

	int getFinalScore() const;

	const char* getModName() const;

private:
	CvReplayInfo* m_pHoF;

	CvReplayInfo m_replay;
};

#endif __CY_Replay_H__
