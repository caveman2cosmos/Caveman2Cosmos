#pragma once

#ifndef CV_MISSION_INFO_H
#define CV_MISSION_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvMissionInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMissionInfo
	: public CvHotkeyInfo
	, private bst::noncopyable
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvMissionInfo();
	explicit CvMissionInfo(const char* szType);
	virtual ~CvMissionInfo();

	DllExport int getTime() const;

	bool isSound() const;
	DllExport bool isTarget() const;
	bool isBuild() const;
	bool getVisible() const;
	DllExport EntityEventTypes getEntityEvent() const;

	const char* getWaypoint() const;		// effect type, Exposed to Python

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvMissionInfo* pClassInfo);

	// Should not be needed to checksum this as this will likely not get changed without DLL changes
	//void getCheckSum(uint32_t& iSum) const;

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iTime;

	bool m_bSound;
	bool m_bTarget;
	bool m_bBuild;
	bool m_bVisible;
	EntityEventTypes m_eEntityEvent;

	CvString m_szWaypoint;
};

#endif // CV_MISSION_INFO_H
