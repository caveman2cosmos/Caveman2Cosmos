#pragma once

#ifndef CV_ART_INFO_CIVILIZATION_H
#define CV_ART_INFO_CIVILIZATION_H

#include "CvInfoBase.h"
#include "CvArtInfoAsset.h"

class CvArtInfoCivilization
	: public CvArtInfoAsset
	, private bst::noncopyable
{
public:

	CvArtInfoCivilization();
	virtual ~CvArtInfoCivilization();

	bool isWhiteFlag() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvArtInfoCivilization* pClassInfo);

protected:

	bool m_bWhiteFlag;
};

#endif // CV_ART_INFO_CIVILIZATION_H
