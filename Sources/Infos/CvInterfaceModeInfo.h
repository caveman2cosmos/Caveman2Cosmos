#pragma once

#ifndef CV_INTERFACE_MODE_INFO_H
#define CV_INTERFACE_MODE_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvInterfaceModeInfo (ADD to Python)
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvInterfaceModeInfo
	: public CvHotkeyInfo
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvInterfaceModeInfo();
	virtual ~CvInterfaceModeInfo();

	DllExport int getCursorIndex() const;
	DllExport int getMissionType() const;

	bool getVisible() const;
	DllExport bool getGotoPlot() const;
	bool getHighlightPlot() const;
	bool getSelectType() const;
	bool getSelectAll() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvInterfaceModeInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iCursorIndex;
	int m_iMissionType;

	bool m_bVisible;
	bool m_bGotoPlot;
	bool m_bHighlightPlot;
	bool m_bSelectType;
	bool m_bSelectAll;
};

#endif // CV_INTERFACE_MODE_INFO_H
