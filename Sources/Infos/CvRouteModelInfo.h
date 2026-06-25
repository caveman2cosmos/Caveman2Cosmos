#pragma once

#ifndef CV_ROUTE_MODEL_INFO_H
#define CV_ROUTE_MODEL_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvRouteModelInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvRouteModelInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvRouteModelInfo();
	virtual ~CvRouteModelInfo();

	DllExport RouteTypes getRouteType() const;

	DllExport const char* getModelFile() const;
	DllExport const char* getLateModelFile() const;
	const char* getModelFileKey() const;

	DllExport bool isAnimated() const;

	DllExport const char* getConnectString() const;
	DllExport const char* getModelConnectString() const;
	DllExport const char* getRotateString() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvRouteModelInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	RouteTypes	m_eRouteType;			// The route type

	CvString m_szModelFile;				// The model filename
	CvString m_szLateModelFile;			// The model filename
	CvString m_szModelFileKey;			// The model file key reference
	bool m_bAnimated;

	char m_szConnectString[32];		// The connections this cell makes ( N S E W NE NW SE SW )
	char m_szModelConnectString[32];	// The connections this model makes ( N S E W NE NW SE SW )
	char m_szRotateString[32];		// The possible rotations for this cell ( 0 90 180 270 )
};

#endif // CV_ROUTE_MODEL_INFO_H
