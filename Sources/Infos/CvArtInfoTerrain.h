#pragma once

#ifndef CV_ART_INFO_TERRAIN_H
#define CV_ART_INFO_TERRAIN_H

#include "CvInfoBase.h"
#include "CvArtInfoAsset.h"

typedef std::vector<std::pair<int, int> > CvTextureBlendSlotList;

class CvArtInfoTerrain
	: public CvArtInfoAsset
	, private bst::noncopyable
{
public:
	//!< number to blend textures.
	static const int NUM_TEXTURE_BLENDS = 16;

	CvArtInfoTerrain();

	DllExport const char* getBaseTexture();
	void setBaseTexture(const char* szTmp);
	DllExport const char* getGridTexture();
	void setGridTexture(const char* szTmp);
	DllExport const char* getDetailTexture();
	void setDetailTexture(const char* szTmp);
	DllExport int getLayerOrder();
	DllExport bool useAlphaShader();
	DllExport CvTextureBlendSlotList& getBlendList(int blendMask);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvArtInfoTerrain* pClassInfo);

protected:

	//!< Detail texture associated with the Terrain base texture
	CvString m_szDetailTexture;
	CvString m_szGridTexture;

	//!< Layering order of texture
	int m_iLayerOrder;
	bool m_bAlphaShader;

	//!< Array of Textureslots per blend tile
	CvTextureBlendSlotList m_pTextureSlots[NUM_TEXTURE_BLENDS];
	CvString m_pSlotNames[NUM_TEXTURE_BLENDS];
};

#endif // CV_ART_INFO_TERRAIN_H
