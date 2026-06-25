#pragma once

#ifndef CV_VOTE_INFO_H
#define CV_VOTE_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvVoteInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvVoteInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:
	CvVoteInfo();
	virtual ~CvVoteInfo();

	int getPopulationThreshold() const;
	int getStateReligionVotePercent() const;
	int getTradeRoutes() const;
	int getMinVoters() const;

	bool isSecretaryGeneral() const;
	bool isVictory() const;
	bool isFreeTrade() const;
	bool isNoNukes() const;
	bool isCityVoting() const;
	bool isCivVoting() const;
	bool isDefensivePact() const;
	bool isOpenBorders() const;
	bool isForcePeace() const;
	bool isForceNoTrade() const;
	bool isForceWar() const;
	bool isAssignCity() const;

	// Arrays

	bool isForceCivic(int i) const;
	bool isVoteSourceType(int i) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvVoteInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

protected:

	int m_iPopulationThreshold;
	int m_iStateReligionVotePercent;
	int m_iTradeRoutes;
	int m_iMinVoters;

	bool m_bSecretaryGeneral;
	bool m_bVictory;
	bool m_bFreeTrade;
	bool m_bNoNukes;
	bool m_bCityVoting;
	bool m_bCivVoting;
	bool m_bDefensivePact;
	bool m_bOpenBorders;
	bool m_bForcePeace;
	bool m_bForceNoTrade;
	bool m_bForceWar;
	bool m_bAssignCity;

	// Arrays

	bool* m_pbForceCivic;
	bool* m_abVoteSourceTypes;

};

#endif // CV_VOTE_INFO_H
