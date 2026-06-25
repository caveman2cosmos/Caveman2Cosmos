#pragma once

#ifndef CV_CLIMATE_INFO_H
#define CV_CLIMATE_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvClimateInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvClimateInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:

	CvClimateInfo();
	virtual ~CvClimateInfo();

	int getDesertPercentChange() const;
	int getJungleLatitude() const;
	int getHillRange() const;
	int getPeakPercent() const;

	float getSnowLatitudeChange() const;
	float getTundraLatitudeChange() const;
	float getGrassLatitudeChange() const;
	float getDesertBottomLatitudeChange() const;
	float getDesertTopLatitudeChange() const;
	float getIceLatitude() const;
	float getRandIceLatitude() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvClimateInfo* pClassInfo);

protected:

	int m_iDesertPercentChange;
	int m_iJungleLatitude;
	int m_iHillRange;
	int m_iPeakPercent;

	float m_fSnowLatitudeChange;
	float m_fTundraLatitudeChange;
	float m_fGrassLatitudeChange;
	float m_fDesertBottomLatitudeChange;
	float m_fDesertTopLatitudeChange;
	float m_fIceLatitude;
	float m_fRandIceLatitude;

};

#endif // CV_CLIMATE_INFO_H
