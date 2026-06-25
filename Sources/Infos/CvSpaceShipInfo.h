#pragma once

#ifndef CV_SPACE_SHIP_INFO_H
#define CV_SPACE_SHIP_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSpaceShipInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpaceShipInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvSpaceShipInfo();
	virtual ~CvSpaceShipInfo();

	DllExport const char* getNodeName();
	DllExport const char* getProjectName();
	void setProjectName(const char* szVal);
	DllExport ProjectTypes getProjectType();
	DllExport AxisTypes getCameraUpAxis();
	DllExport SpaceShipInfoTypes getSpaceShipInfoType();
	DllExport int getPartNumber();
	DllExport int getArtType();
	DllExport int getEventCode();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvSpaceShipInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szNodeName;
	CvString m_szProjectName;
	ProjectTypes m_eProjectType;
	AxisTypes m_eCameraUpAxis;
	int m_iPartNumber;
	int m_iArtType;
	int m_iEventCode;
	SpaceShipInfoTypes m_eSpaceShipInfoType;
};

#endif // CV_SPACE_SHIP_INFO_H
