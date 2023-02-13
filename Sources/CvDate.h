#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvDate.h
//
//  PURPOSE: Class to keep a Civ4 date and methods related to the time/turn relationship
//
//------------------------------------------------------------------------------------------------
#ifndef CV_DATE_H
#define CV_DATE_H

//#include "CvEnums.h"

struct CvDateIncrement;

class CvDate
{
public:

	CvDate();
	explicit CvDate(uint32_t iTick);

	int getYear() const;
	int getMonth() const;
	int getWeek() const; // number of the week in a month
	SeasonTypes getSeason() const;
	int getDay() const;
	uint32_t GetTick() const;

	CvDateIncrement getIncrement(GameSpeedTypes eGameSpeed = NO_GAMESPEED) const;
	void increment(GameSpeedTypes eGameSpeed = NO_GAMESPEED);
	void increment(int iTurns, GameSpeedTypes eGameSpeed = NO_GAMESPEED); // inefficient
	static CvDate getDate(int iTurn, GameSpeedTypes eGameSpeed = NO_GAMESPEED);
	static CvDate getStartingDate();

	static void calculateEndDates(GameSpeedTypes eGameSpeed);

	bool operator<(const CvDate& kDate) const;
	bool operator<=(const CvDate& kDate) const;
	bool operator>(const CvDate& kDate) const;
	bool operator>=(const CvDate& kDate) const;
	bool operator==(const CvDate& kDate) const;
	bool operator!=(const CvDate& kDate) const;

protected:
	uint32_t m_iTick;
};

struct CvDateIncrement
{
	CvDateIncrement()
		: m_iendTurn(0)
		, m_iIncrementMonth(0)
		, m_iIncrementDay(0)
	{}

	CvDate m_endDate;
	int m_iendTurn;
	int m_iIncrementMonth;
	int m_iIncrementDay;
};

#endif