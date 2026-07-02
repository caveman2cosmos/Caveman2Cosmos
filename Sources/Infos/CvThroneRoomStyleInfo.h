#pragma once

#ifndef CV_THRONE_ROOM_STYLE_INFO_H
#define CV_THRONE_ROOM_STYLE_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvThroneRoomStyleInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvThroneRoomStyleInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvThroneRoomStyleInfo();
	virtual ~CvThroneRoomStyleInfo();

	DllExport const char* getArtStyleType()	{ return m_szArtStyleType; }
	DllExport const char* getEraType()		{ return m_szEraType; }
	DllExport const char* getFileName()		{ return m_szFileName; }

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvThroneRoomStyleInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	CvString m_szArtStyleType;
	CvString m_szEraType;
	CvString m_szFileName;
	std::vector<CvString> m_aNodeNames;
	std::vector<CvString> m_aTextureNames;
};

#endif // CV_THRONE_ROOM_STYLE_INFO_H
