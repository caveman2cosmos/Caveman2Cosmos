#pragma once

#ifndef CV_YIELD_INFO_H
#define CV_YIELD_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvYieldInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvYieldInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvYieldInfo();
	virtual ~CvYieldInfo();

	int getChar() const;
	void setChar(int i);
	int getHillsChange() const;
	int getPeakChange() const;
	int getRiverChange() const;
	int getCityChange() const;
	int getPopulationChangeDivisor() const;
	int getMinCity() const;
	int getTradeModifier() const;
	int getGoldenAgeYield() const;
	int getGoldenAgeYieldThreshold() const;
	int getAIWeightPercent() const;
	int getColorType() const;

	// Arrays

	const char* getSymbolPath(int i) const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvYieldInfo* pClassInfo);

	void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iChar;
	int m_iHillsChange;
	int m_iPeakChange;
	int m_iRiverChange;
	int m_iCityChange;
	int m_iPopulationChangeDivisor;
	int m_iMinCity;
	int m_iTradeModifier;
	int m_iGoldenAgeYield;
	int m_iGoldenAgeYieldThreshold;
	int m_iAIWeightPercent;
	int m_iColorType;

	CvString* m_paszSymbolPath;
};

#endif // CV_YIELD_INFO_H
