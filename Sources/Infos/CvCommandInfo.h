#pragma once

#ifndef CV_COMMAND_INFO_H
#define CV_COMMAND_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCommandInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCommandInfo
	: public CvHotkeyInfo
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvCommandInfo();
	virtual ~CvCommandInfo();

	int getAutomate() const;

	bool getConfirmCommand() const;
	bool getVisible() const;
	bool getAll() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvCommandInfo* pClassInfo);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:
	int m_iAutomate;

	bool m_bConfirmCommand;
	bool m_bVisible;
	bool m_bAll;
};

#endif // CV_COMMAND_INFO_H
