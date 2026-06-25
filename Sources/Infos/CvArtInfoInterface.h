#pragma once

#ifndef CV_ART_INFO_INTERFACE_H
#define CV_ART_INFO_INTERFACE_H

#include "CvInfoBase.h"
#include "CvArtInfoAsset.h"

// todoJS: Remove empty classes if additional items are not added

class CvArtInfoInterface
	: public CvArtInfoAsset
	, private bst::noncopyable
{
public:

	CvArtInfoInterface() {}
	virtual ~CvArtInfoInterface() {}
};

#endif // CV_ART_INFO_INTERFACE_H
