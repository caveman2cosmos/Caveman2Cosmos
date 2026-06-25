#pragma once

#ifndef CV_LANDSCAPE_INFO_H
#define CV_LANDSCAPE_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvLandscapeInfo
//
//  Purpose:	This info acts as the Civ4Terrain.ini and is initialize in CvXmlLoadUtility with the infos in
//					XML/Terrain/TerrainSettings.xml
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvLandscapeInfo
	: public CvInfoBase
	, private bst::noncopyable
{
	public:

		CvLandscapeInfo();
		virtual ~CvLandscapeInfo() {}

		int getFogR() const;
		int getFogG() const;
		int getFogB() const;
		DllExport int getHorizontalGameCell() const;
		DllExport int getVerticalGameCell() const;
		DllExport int getPlotsPerCellX() const;
		DllExport int getPlotsPerCellY() const;
		DllExport int getHorizontalVertCnt() const;
		DllExport int getVerticalVertCnt() const;
		DllExport int getWaterHeight() const;

		float getTextureScaleX() const;
		float getTextureScaleY() const;
		DllExport float getZScale() const;

		bool isUseTerrainShader() const;
		bool isUseLightmap() const;
		bool isRandomMap() const;
		DllExport float getPeakScale() const;
		DllExport float getHillScale() const;

		const char* getSkyArt() const		{ return m_szSkyArt; }
		const char* getHeightMap() const	{ return m_szHeightMap; }
		const char* getTerrainMap() const	{ return m_szTerrainMap; }
		const char* getNormalMap() const	{ return m_szNormalMap; }
		const char* getBlendMap() const		{ return m_szBlendMap; }

		bool read(CvXMLLoadUtility* pXML);
		void copyNonDefaults(const CvLandscapeInfo* pClassInfo);

	protected:

		int m_iFogR;
		int m_iFogG;
		int m_iFogB;
		int m_iHorizontalGameCell;
		int m_iVerticalGameCell;
		int m_iPlotsPerCellX;
		int m_iPlotsPerCellY;
		int m_iHorizontalVertCnt;
		int m_iVerticalVertCnt;
		int m_iWaterHeight;

		float m_fTextureScaleX;
		float m_fTextureScaleY;
		float m_fZScale;
		float m_fPeakScale;
		float m_fHillScale;

		bool m_bUseTerrainShader;
		bool m_bUseLightmap;
		bool m_bRandomMap;

		CvString m_szSkyArt;
		CvString m_szHeightMap;
		CvString m_szTerrainMap;
		CvString m_szNormalMap;
		CvString m_szBlendMap;
};

#endif // CV_LANDSCAPE_INFO_H
