#pragma once

#ifndef CVHALLOFFAMEINFO_H
#define CVHALLOFFAMEINFO_H

#include <vector>
class CvReplayInfo;


class CvHallOfFameInfo
{
public:
	CvHallOfFameInfo();
	virtual ~CvHallOfFameInfo();
	void loadReplays();
	int getNumGames() const;
	CvReplayInfo* getReplayInfo(int i);


protected:
	std::vector<CvReplayInfo*> m_aReplays;
};

#endif