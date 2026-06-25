#pragma once

#ifndef CV_THRONE_ROOM_INFO_H
#define CV_THRONE_ROOM_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvThroneRoomInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvThroneRoomInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvThroneRoomInfo();
	virtual ~CvThroneRoomInfo();

	DllExport const char* getEvent();
	DllExport const char* getNodeName();
	DllExport int getFromState();
	DllExport int getToState();
	DllExport int getAnimation();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvThroneRoomInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iFromState;
	int m_iToState;
	int m_iAnimation;
	CvString m_szEvent;
	CvString m_szNodeName;
};

#endif // CV_THRONE_ROOM_INFO_H
