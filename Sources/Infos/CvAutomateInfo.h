#pragma once

#ifndef CV_AUTOMATE_INFO_H
#define CV_AUTOMATE_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvAutomateInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAutomateInfo
	: public CvHotkeyInfo
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvAutomateInfo();
	virtual ~CvAutomateInfo();

	int getCommand() const;
	int getAutomate() const;

	bool getConfirmCommand() const;
	bool getVisible() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvAutomateInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:
	int m_iCommand;
	int m_iAutomate;

	bool m_bConfirmCommand;
	bool m_bVisible;
};

#endif // CV_AUTOMATE_INFO_H
