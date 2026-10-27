#pragma once

#ifndef SUB_TIMER_TAG_H
#define SUB_TIMER_TAG_H

#include <windows.h>
#include "CvDefines.h"

void addPlayerPatrolMs(int iPlayer, double dMs);
void addPlayerAnyAttackMs(int iPlayer, double dMs);
void addPlayerExposedDangerMs(int iPlayer, double dMs);
void addPlayerAttackOddsMs(int iPlayer, double dMs);
void addPlayerMergeCheckMs(int iPlayer, double dMs);
void addPlayerReachableMs(int iPlayer, double dMs);

extern int g_iPatrolDepth;
extern int g_iAnyAttackDepth;
extern int g_iExposedDangerDepth;
extern int g_iAttackOddsDepth;
extern int g_iMergeCheckDepth;
extern int g_iReachableDepth;

struct SubTimerScope
{
	typedef void (*AddFunc)(int, double);

	SubTimerScope(int iPlayer, AddFunc pAddFunc, int& rDepth)
		: m_iPlayer(iPlayer), m_pAddFunc(pAddFunc), m_prDepth(NULL), m_bActive(false)
	{
		if (m_iPlayer >= 0 && m_iPlayer < MAX_PLAYERS)
		{
			m_prDepth = &rDepth;
			if ((*m_prDepth)++ == 0)
			{
				QueryPerformanceCounter(&m_tStart);
				m_bActive = true;
			}
		}
	}

	~SubTimerScope()
	{
		if (m_bActive)
		{
			LARGE_INTEGER freq, tEnd;
			QueryPerformanceFrequency(&freq);
			QueryPerformanceCounter(&tEnd);
			m_pAddFunc(m_iPlayer, (double)(tEnd.QuadPart - m_tStart.QuadPart) * 1000.0 / (double)freq.QuadPart);
		}
		if (m_prDepth)
		{
			--(*m_prDepth);
		}
	}

private:
	int m_iPlayer;
	AddFunc m_pAddFunc;
	int* m_prDepth;
	LARGE_INTEGER m_tStart;
	bool m_bActive;
};

#define SUB_TIMER_PATROL(iPlayer) SubTimerScope timer_patrol_(iPlayer, addPlayerPatrolMs, g_iPatrolDepth)
#define SUB_TIMER_ANYATTACK(iPlayer) SubTimerScope timer_anyattack_(iPlayer, addPlayerAnyAttackMs, g_iAnyAttackDepth)
#define SUB_TIMER_EXPOSEDDANGER(iPlayer) SubTimerScope timer_danger_(iPlayer, addPlayerExposedDangerMs, g_iExposedDangerDepth)
#define SUB_TIMER_ATTACKODDS(iPlayer) SubTimerScope timer_odds_(iPlayer, addPlayerAttackOddsMs, g_iAttackOddsDepth)
#define SUB_TIMER_MERGECHECK(iPlayer) SubTimerScope timer_merge_(iPlayer, addPlayerMergeCheckMs, g_iMergeCheckDepth)
#define SUB_TIMER_REACHABLE(iPlayer) SubTimerScope timer_reach_(iPlayer, addPlayerReachableMs, g_iReachableDepth)

#endif // SUB_TIMER_TAG_H
