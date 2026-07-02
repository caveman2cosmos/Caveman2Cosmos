#pragma once

#ifndef CV_ART_INFO_SCALABLE_ASSET_H
#define CV_ART_INFO_SCALABLE_ASSET_H

#include "CvInfoBase.h"
#include "CvArtInfoAsset.h"

//
//////////////////////////////////////////////////////////////////////////
// Another base class
//////////////////////////////////////////////////////////////////////////

class CvArtInfoScalableAsset
	: public CvArtInfoAsset
	, public CvScalableInfo
{
public:
	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvArtInfoScalableAsset* pClassInfo);
};

#endif // CV_ART_INFO_SCALABLE_ASSET_H
