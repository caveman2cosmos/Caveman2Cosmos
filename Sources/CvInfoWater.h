#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvInfosWater.h
//
//  AUTHOR:	tomw
//
//
//  PURPOSE: All Civ4 info classes and the base class for them
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2003 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------
#ifndef CV_WATERINFO_H
#define CV_WATERINFO_H

#pragma warning( disable: 4251 )		// needs to have dll-interface to be used by clients of class

class CvInfoBase;
class CvXMLLoadUtility;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvWaterPlaneInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvWaterPlaneInfo : public CvInfoBase
{
public:

	CvWaterPlaneInfo();
	virtual ~CvWaterPlaneInfo();

	NiColor	getMaterialDiffuse() const;			// The water plane's material diffuse color
	NiColor	getMaterialSpecular() const;		// The water plane's material specular color
	NiColor	getMaterialEmmisive() const;		// The water plane's material emmisive color
	DllExport float getMaterialAlpha() const;				// The water plane's material alpha

	DllExport float getTextureScaling() const;				// The water plane's texture scale
	DllExport float getTextureScrollRateU() const;			// The water plane's texture scroll rate in U
	DllExport float getTextureScrollRateV() const;			// The water plane's texture scroll rate in V
	DllExport const TCHAR * getBaseTexture() const;

	DllExport const TCHAR *getTransitionTexture() const;

	bool read(CvXMLLoadUtility*);
	void copyNonDefaults(CvWaterPlaneInfo* pClassInfo);

protected:

	NiColor	m_kMaterialDiffuse;		// The water plane's material diffuse color
	NiColor	m_kMaterialSpecular;	// The water plane's material specular color
	NiColor	m_kMaterialEmmisive;	// The water plane's material emmisive color
	float m_fMaterialAlpha;			// The water plane's material alpha

	CvString m_szBaseTexture;		// The filename of the base texture
	CvString m_szTransitionTexture;	// The filename of the transition texture

	float m_BaseTextureScale;		// Texture scaling
	float m_fURate;					// Texture scroll rate
	float m_fVRate;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvTerrainPlaneInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvTerrainPlaneInfo : public CvInfoBase
{
public:

	CvTerrainPlaneInfo();
	virtual ~CvTerrainPlaneInfo();

	DllExport bool isVisible() const;				// The terrain plane's material alpha
	DllExport bool isGroundPlane() const;				// The terrain plane's material alpha
	DllExport float getMaterialAlpha() const;				// The terrain plane's material alpha
	DllExport float getCloseAlpha() const;				// The terrain plane's material alpha

	DllExport float getTextureScalingU() const;				// The terrain plane's texture scale
	DllExport float getTextureScalingV() const;				// The terrain plane's texture scale
	DllExport float getTextureScrollRateU() const;			// The terrain plane's texture scroll rate in U
	DllExport float getTextureScrollRateV() const;			// The terrain plane's texture scroll rate in V
	DllExport float getZHeight() const;						// The terrain plane's z height in world units
	DllExport FogTypes getFogType() const;
	DllExport const TCHAR * getBaseTexture() const;

	bool read(CvXMLLoadUtility*);
	void copyNonDefaults(CvTerrainPlaneInfo* pClassInfo);

protected:

	bool m_bVisible;
	bool m_bGroundPlane;
	float m_fMaterialAlpha;			// The terrain plane's material alpha
	float m_fCloseAlpha;

	CvString m_szBaseTexture;		// The filename of the base texture

	float m_BaseTextureScaleU;		// Texture scaling
	float m_BaseTextureScaleV;		// Texture scaling
	float m_fURate;					// Texture scroll rate
	float m_fVRate;
	float m_fZHeight;
	FogTypes m_eFogType;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvTerrainPlaneInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCameraOverlayInfo : public CvInfoBase
{
public:

	CvCameraOverlayInfo();
	virtual ~CvCameraOverlayInfo();

	DllExport bool isVisible() const;				// The terrain plane's material alpha
	DllExport CameraOverlayTypes getCameraOverlayType() const;
	DllExport const TCHAR * getBaseTexture() const;

	bool read(CvXMLLoadUtility*);
	void copyNonDefaults(CvCameraOverlayInfo* pClassInfo);

protected:

	bool m_bVisible;
	CvString m_szBaseTexture;		// The filename of the base texture
	CameraOverlayTypes m_eCameraOverlayType;
};


#endif
