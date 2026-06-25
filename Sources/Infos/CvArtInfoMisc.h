#pragma once

#ifndef CV_ART_INFO_MISC_H
#define CV_ART_INFO_MISC_H

#include "CvInfoBase.h"
#include "CvArtInfoScalableAsset.h"

class CvArtInfoMisc
	: public CvArtInfoScalableAsset
	, private bst::noncopyable
{
public:

	CvArtInfoMisc() {}
	virtual ~CvArtInfoMisc() {}
};

#endif // CV_ART_INFO_MISC_H
