#pragma once

#ifndef CV_ART_INFO_LEADERHEAD_H
#define CV_ART_INFO_LEADERHEAD_H

#include "CvInfoBase.h"
#include "CvArtInfoAsset.h"

class CvArtInfoLeaderhead
	: public CvArtInfoAsset
	, private bst::noncopyable
{
public:

	CvArtInfoLeaderhead() {}
	virtual ~CvArtInfoLeaderhead() {}

	DllExport const char* getNoShaderNIF() const;
	void setNoShaderNIF(const char* szNIF);
	DllExport const char* getBackgroundKFM() const;
	void setBackgroundKFM( const char* szKFM);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvArtInfoLeaderhead* pClassInfo);

protected:

	CvString m_szNoShaderNIF;
	CvString m_szBackgroundKFM;
};

#endif // CV_ART_INFO_LEADERHEAD_H
