#pragma once

#ifndef CV_ART_INFO_IMPROVEMENT_H
#define CV_ART_INFO_IMPROVEMENT_H

#include "CvInfoBase.h"
#include "CvArtInfoScalableAsset.h"

class CvArtInfoImprovement
	: public CvArtInfoScalableAsset
	, private bst::noncopyable
{
public:

	CvArtInfoImprovement();
	virtual ~CvArtInfoImprovement();

	DllExport const char* getShaderNIF() const;
	void setShaderNIF(const char* szDesc);

	bool isExtraAnimations() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvArtInfoImprovement* pClassInfo);

protected:
	CvString m_szShaderNIF;		//!< The NIF used if the graphics card supports shaders

	bool m_bExtraAnimations;
};

#endif // CV_ART_INFO_IMPROVEMENT_H
