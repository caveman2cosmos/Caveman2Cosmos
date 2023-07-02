#pragma once

#ifndef CVHALLOFFAMEINFO_H
#define CVHALLOFFAMEINFO_H

#include "FProfiler.h"
#include "CvReplayInfo.h"
#include "CvGameCoreDLL.h"
#include "CvHallOfFameInfo.h"

class CvHallOfFameInfo
{
public:
	CvHallOfFameInfo()
	{}
	virtual ~CvHallOfFameInfo()
	{
		PROFILE_EXTRA_FUNC();
		for (uint i = 0; i < m_aReplays.size(); i++)
		{
			SAFE_DELETE(m_aReplays[i]);
		}
	}

	void loadReplays()
	{
		gDLL->loadReplays(m_aReplays);
	}

	int getNumGames() const
	{
		return (int)m_aReplays.size();
	}

	CvReplayInfo* getReplayInfo(int i)
	{
		return m_aReplays[i];
	}

protected:
	std::vector<CvReplayInfo*> m_aReplays;
};

#endif