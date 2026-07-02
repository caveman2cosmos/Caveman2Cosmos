#pragma once

#ifndef CV_ACTION_INFO_H
#define CV_ACTION_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvActionInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvActionInfo
	: private bst::noncopyable
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvActionInfo();
	virtual ~CvActionInfo();

	int getMissionData() const;
	int getCommandData() const;

	int getAutomateType() const;
	int getInterfaceModeType() const;
	int getMissionType() const;
	int getCommandType() const;
	int getControlType() const;
	int getOriginalIndex() const;
	void setOriginalIndex(int i) { m_iOriginalIndex = i; }

	bool isConfirmCommand() const;
	DllExport bool isVisible() const;
	DllExport ActionSubTypes getSubType() const;
	void setSubType(ActionSubTypes eSubType) { m_eSubType = eSubType; }

	// functions to replace the CvInfoBase calls
	const char* getType() const;
	const wchar_t* getDescription() const;
	const wchar_t* getCivilopedia() const;
	const wchar_t* getHelp() const;
	const wchar_t* getStrategy() const;

	virtual const char* getButton() const;
	const wchar_t* getTextKeyWide() const;

	// functions to replace the CvHotkey calls
	int getActionInfoIndex() const;
	DllExport int getHotKeyVal() const;
	DllExport int getHotKeyPriority() const;
	DllExport int getHotKeyValAlt() const;
	DllExport int getHotKeyPriorityAlt() const;
	int getOrderPriority() const;

	DllExport bool isAltDown() const;
	DllExport bool isShiftDown() const;
	DllExport bool isCtrlDown() const;
	DllExport bool isAltDownAlt() const;
	DllExport bool isShiftDownAlt() const;
	DllExport bool isCtrlDownAlt() const;

	const char* getHotKey() const;

	std::wstring getHotKeyDescription() const;

//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iOriginalIndex;
	ActionSubTypes m_eSubType;

private:
	const CvHotkeyInfo* getHotkeyInfo() const;
};

#endif // CV_ACTION_INFO_H
