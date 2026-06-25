#pragma once

#ifndef CV_ART_INFO_ASSET_H
#define CV_ART_INFO_ASSET_H

#include "CvInfoBase.h"
#include "CvAssetInfoBase.h"

class CvArtInfoAsset
	: public CvAssetInfoBase
{
public:

	CvArtInfoAsset() {}
	virtual ~CvArtInfoAsset() {}

	DllExport const char* getNIF() const;
	DllExport const char* getKFM() const;

	void setNIF(const char* szDesc);
	void setKFM(const char* szDesc);

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvArtInfoAsset* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szKFM;
	CvString m_szNIF;
};

#endif // CV_ART_INFO_ASSET_H
