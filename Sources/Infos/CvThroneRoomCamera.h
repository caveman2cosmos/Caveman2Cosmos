#pragma once

#ifndef CV_THRONE_ROOM_CAMERA_H
#define CV_THRONE_ROOM_CAMERA_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvThroneRoomCamera
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvThroneRoomCamera
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvThroneRoomCamera();
	virtual ~CvThroneRoomCamera();

	DllExport const char* getFileName();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvThroneRoomCamera* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szFileName;
};

#endif // CV_THRONE_ROOM_CAMERA_H
