#pragma once

#ifndef CV_ART_INFO_BUILDING_H
#define CV_ART_INFO_BUILDING_H

#include "CvInfoBase.h"
#include "CvArtInfoScalableAsset.h"

class CvArtInfoBuilding
	: public CvArtInfoScalableAsset
	, private bst::noncopyable
{
public:

	CvArtInfoBuilding();
	virtual ~CvArtInfoBuilding();

	bool isAnimated() const;
	DllExport const char* getLSystemName() const;

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvArtInfoBuilding* pClassInfo);

protected:

	bool m_bAnimated;
	CvString m_szLSystemName;
};

#endif // CV_ART_INFO_BUILDING_H
