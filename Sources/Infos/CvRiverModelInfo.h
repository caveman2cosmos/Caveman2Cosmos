#pragma once

#ifndef CV_RIVER_MODEL_INFO_H
#define CV_RIVER_MODEL_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvRiverModelInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvRiverModelInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvRiverModelInfo();
	virtual ~CvRiverModelInfo();

	DllExport const char* getModelFile() const;
	DllExport const char* getBorderFile() const;

	DllExport int getTextureIndex() const;
	DllExport const char* getDeltaString() const;
	DllExport const char* getConnectString() const;
	DllExport const char* getRotateString() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvRiverModelInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	CvString m_szModelFile;					// The model filename
	CvString m_szBorderFile;				// The border filename

	char		m_szDeltaString[32];		// The delta type
	char		m_szConnectString[32];		// The connections this cell makes ( N S E W NE NW SE SW )
	char		m_szRotateString[32];		// The possible rotations for this cell ( 0 90 180 270 )
	int			m_iTextureIndex;
};

#endif // CV_RIVER_MODEL_INFO_H
