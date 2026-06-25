#pragma once

#ifndef CV_MAP_INFO_H
#define CV_MAP_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  class : CvMapInfo
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CvMapInfo
	: public CvHotkeyInfo
	, private bst::noncopyable
{
public:
	CvMapInfo();
	virtual ~CvMapInfo();

	int getGridWidth() const				{ return m_iGridWidth; }
	int getGridHeight() const				{ return m_iGridHeight; }

	int getWrapX() const					{ return m_iWrapX; }
	int getWrapY() const					{ return m_iWrapY; }

	inline bool startRevealed() const		{ return m_bStartRevealed; }

	const CvString getInitialWBMap() const	{ return m_szInitialWBMap; }
	const CvString getMapScript() const		{ return m_szMapScript; }

	bool read(CvXMLLoadUtility* pXML);

private:
	int m_iGridWidth;
	int m_iGridHeight;

	int m_iWrapX;
	int m_iWrapY;

	bool m_bStartRevealed;

	CvString m_szInitialWBMap;
	CvString m_szMapScript;
};

#endif // CV_MAP_INFO_H
