#pragma once

#ifndef CV_VICTORY_INFO_H
#define CV_VICTORY_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvVictoryInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvVictoryInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvVictoryInfo();
	virtual ~CvVictoryInfo();

	int getPopulationPercentLead() const;
	int getLandPercent() const;
	int getMinLandPercent() const;
	int getReligionPercent() const;
	int getCityCulture() const;
	int getNumCultureCities() const;
	int getTotalCultureRatio() const;
	int getVictoryDelayTurns() const;

	bool isTotalVictory() const;
	bool isTargetScore() const;
	bool isEndScore() const;
	bool isConquest() const;
	bool isDiploVote() const;
	DllExport bool isPermanent() const;

	const char* getMovie() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvVictoryInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iPopulationPercentLead;
	int m_iLandPercent;
	int m_iMinLandPercent;
	int m_iReligionPercent;
	int m_iCityCulture;
	int m_iNumCultureCities;
	int m_iTotalCultureRatio;
	int m_iVictoryDelayTurns;

	bool m_bTargetScore;
	bool m_bEndScore;
	bool m_bConquest;
	bool m_bDiploVote;
	bool m_bPermanent;
	bool m_bTotalVictory;

	CvString m_szMovie;
};

#endif // CV_VICTORY_INFO_H
