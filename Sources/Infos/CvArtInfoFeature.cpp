//------------------------------------------------------------------------------------------------
//  FILE:    CvArtInfoFeature.cpp
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvArtFileMgr.h"
#include "CvBuildingInfo.h"
#include "CvHeritageInfo.h"
#include "CvGameAI.h"
#include "CvGameTextMgr.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvInfoUtil.h"
#include "CvPlayerAI.h"
#include "CvPython.h"
#include "CvXMLLoadUtility.h"
#include "CvXMLLoadUtilityModTools.h"
#include "CheckSum.h"
#include "CvImprovementInfo.h"
#include "CvBonusInfo.h"
#include "CvArtInfoFeature.h"


//////////////////////////////////////////////////////////////////////////
// CvArtInfoFeature
//////////////////////////////////////////////////////////////////////////

CvArtInfoFeature::CvArtInfoFeature() :
m_bAnimated(false),
m_bRiverArt(false),
m_eTileArtType(TILE_ART_TYPE_NONE),
m_eLightType(LIGHT_TYPE_NONE)
{
}


CvArtInfoFeature::~CvArtInfoFeature()
{
}


bool CvArtInfoFeature::isAnimated() const
{
	return m_bAnimated;
}


bool CvArtInfoFeature::isRiverArt() const
{
	return m_bRiverArt;
}


TileArtTypes CvArtInfoFeature::getTileArtType() const
{
	return m_eTileArtType;
}


LightTypes CvArtInfoFeature::getLightType() const
{
	return m_eLightType;
}


bool CvArtInfoFeature::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	if (!CvArtInfoScalableAsset::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_bAnimated, L"bAnimated");
	pXML->GetOptionalChildXmlValByName(&m_bRiverArt, L"bRiverArt");

	CvString szTemp;
	pXML->GetOptionalChildXmlValByName(szTemp, L"TileArtType", "TILE_ART_TYPE_NONE");
	if(szTemp.CompareNoCase("TILE_ART_TYPE_NONE") == 0)
		m_eTileArtType = TILE_ART_TYPE_NONE;
	else if(szTemp.CompareNoCase("TILE_ART_TYPE_TREES") == 0)
		m_eTileArtType = TILE_ART_TYPE_TREES;
	else if(szTemp.CompareNoCase("TILE_ART_TYPE_HALF_TILING") == 0)
		m_eTileArtType = TILE_ART_TYPE_HALF_TILING;
	else if(szTemp.CompareNoCase("TILE_ART_TYPE_PLOT_TILING") == 0)
		m_eTileArtType = TILE_ART_TYPE_PLOT_TILING;
	else
	{
		FErrorMsg("[Jason] Unknown TileArtType.");
	}

	pXML->GetOptionalChildXmlValByName(szTemp, L"LightType", "LIGHT_TYPE_NONE");
	if(szTemp.CompareNoCase("LIGHT_TYPE_NONE") == 0)
		m_eLightType = LIGHT_TYPE_NONE;
	else if(szTemp.CompareNoCase("LIGHT_TYPE_SUN") == 0)
		m_eLightType = LIGHT_TYPE_SUN;
	else if(szTemp.CompareNoCase("LIGHT_TYPE_TERRAIN") == 0)
		m_eLightType = LIGHT_TYPE_TERRAIN;
	else if(szTemp.CompareNoCase("LIGHT_TYPE_UNIT") == 0)
		m_eLightType = LIGHT_TYPE_UNIT;
	else
	{
		FErrorMsg("[Jason] Unknown LightType.");
	}

	//feature varieties
	if(pXML->TryMoveToXmlFirstChild(L"FeatureVariety"))
	{
		do
		{
			m_aFeatureVarieties.push_back(FeatureVariety());
			FeatureVariety &featureVariety = m_aFeatureVarieties.back();

			//generate rotations
			bool bGenerateRotations = false;
			pXML->GetOptionalChildXmlValByName(&bGenerateRotations, L"bGenerateRotations");

			//feature art pieces
			if(pXML->TryMoveToXmlFirstChild(L"FeatureArtPieces"))
			{
				if(pXML->TryMoveToXmlFirstChild(L"FeatureArtPiece"))
				{
					do
					{
						//connection mask
						pXML->GetOptionalChildXmlValByName(szTemp, L"Connections");
						int connectionMask = getConnectionMaskFromString(szTemp);

						//model files
						if(pXML->TryMoveToXmlFirstChild(L"ModelFile"))
						{
							do
							{
								pXML->GetXmlVal(szTemp);

								if(!bGenerateRotations)
								{
									FeatureArtPiece &featureArtPiece = featureVariety.createFeatureArtPieceFromConnectionMask(connectionMask);
									featureArtPiece.m_aArtModels.push_back(FeatureArtModel(szTemp, ROTATE_NONE));
								}
								else
								{
									for(int i=0;i<NUM_ROTATION_TYPES;i++)
									{
										const int newConnectionMask = getRotatedConnectionMask(connectionMask, (RotationTypes) i);
										FeatureArtPiece &featureArtPiece = featureVariety.createFeatureArtPieceFromConnectionMask(newConnectionMask);
										featureArtPiece.m_aArtModels.push_back(FeatureArtModel(szTemp, (RotationTypes) i));
									}
								}
							} while(pXML->TryMoveToXmlNextSibling(L"ModelFile"));

							pXML->MoveToXmlParent();
						}
					} while(pXML->TryMoveToXmlNextSibling(L"FeatureArtPiece"));

					pXML->MoveToXmlParent();
				}

				pXML->MoveToXmlParent();
			}

			//feature art pieces
			if(pXML->TryMoveToXmlFirstChild(L"FeatureDummyNodes"))
			{
				if(pXML->TryMoveToXmlFirstChild(L"FeatureDummyNode"))
				{
					do
					{
						CvString tagName;
						CvString nodeName;

						//tag name
						pXML->GetChildXmlValByName(tagName, L"Tag");

						//node name
						pXML->GetChildXmlValByName(nodeName, L"Name");

						featureVariety.createFeatureDummyNode(tagName, nodeName);
					} while(pXML->TryMoveToXmlNextSibling(L"FeatureDummyNode"));

					pXML->MoveToXmlParent();
				}

				pXML->MoveToXmlParent();
			}

			//variety button
			pXML->GetChildXmlValByName(featureVariety.m_szVarietyButton, L"VarietyButton");
		} while(pXML->TryMoveToXmlNextSibling(L"FeatureVariety"));

		pXML->MoveToXmlParent();
	}

	return true;
}


void CvArtInfoFeature::dump()
{
	PROFILE_EXTRA_FUNC();
	OutputDebugString(CvString::format("CvArtInfoFeature: %s:\n", m_szType.c_str()).c_str());
	int iNum = m_aFeatureVarieties.size();
	OutputDebugString(CvString::format("\t%d varieties:\n", iNum).c_str());
	for(int i = 0; i < iNum; i++)
	{
		FeatureVariety &featureVariety = m_aFeatureVarieties[i];

		//feature art pieces
		int iNumArtPieces = featureVariety.m_aFeatureArtPieces.size();
		OutputDebugString(CvString::format("\t\t%d art pieces:\n", iNumArtPieces).c_str());
		for(int j = 0; j < iNumArtPieces; j++)
		{
			CvArtInfoFeature::FeatureArtPiece& kArtPiece = featureVariety.m_aFeatureArtPieces[j];
			//connection mask
			OutputDebugString(CvString::format("\t\t\tConnection mask: %d\n", kArtPiece.getConnectionMask()).c_str());

			//model files
			int iNumModelFiles = kArtPiece.m_aArtModels.size();
			OutputDebugString(CvString::format("\t\t\t%d art models:\n", iNumModelFiles).c_str());
			for(int k = 0; k < iNumModelFiles; k++)
			{
				FeatureArtModel kArtModel = kArtPiece.m_aArtModels[k];

				OutputDebugString(CvString::format("\t\t\t\t%Rotation %d, file: %s\n", kArtModel.getRotation(), kArtModel.getModelFile().c_str()).c_str());
			}
		}

		//feature dummy nodes
		int iNumDummyNodes = featureVariety.m_aFeatureDummyNodes.size();
		OutputDebugString(CvString::format("\t\t%d dummy nodes:\n", iNumDummyNodes).c_str());
		for(int j = 0; j < iNumDummyNodes; j++)
		{
			FeatureDummyNode&	kNode = featureVariety.m_aFeatureDummyNodes[j];

			OutputDebugString(CvString::format("\t\t\tTag %s, name %s\n", kNode.getTagName().c_str(), kNode.getNodeName().c_str()).c_str());
		}

		//variety button
		OutputDebugString(CvString::format("\t\t%Variety button: %s\n", featureVariety.m_szVarietyButton.c_str()).c_str());
	}
}


void CvArtInfoFeature::copyNonDefaults(const CvArtInfoFeature* pClassInfo)
{
	// Empty, for Art files we stick to FULL XML defintions
}


const CvArtInfoFeature::FeatureVariety &CvArtInfoFeature::getVariety(int index) const
{
	//FAssertMsg((index >= 0) && (index < (int) m_aFeatureVarieties.size()), "[Jason] Feature Variety index out of range.");
	//AIAndy: limit to valid range
	const int iIndex = range(index, 0, getNumVarieties() - 1);
	return m_aFeatureVarieties[iIndex];
}


int CvArtInfoFeature::getNumVarieties() const
{
	return m_aFeatureVarieties.size();
}


const std::string CvArtInfoFeature::getFeatureDummyNodeName(int variety, const std::string tagName) const
{
	return getVariety(variety).getFeatureDummyNodeName(tagName);
}


int CvArtInfoFeature::getConnectionMaskFromString(const CvString& connectionString) const
{
	PROFILE_EXTRA_FUNC();
	if(connectionString.IsEmpty())
		return 0;
	else
	{
		std::vector<CvString> tokens;
		connectionString.getTokens(" \t\n", tokens);

		int connectionMask = 0;
		for(int i=0;i<(int)tokens.size();i++)
		{
			// found a token, parse it.
			const CvString& token = tokens[i];
			if(token.CompareNoCase("NW") == 0)
				connectionMask |= DIRECTION_NORTHWEST_MASK;
			else if(token.CompareNoCase("N") == 0)
				connectionMask |= DIRECTION_NORTH_MASK;
			else if(token.CompareNoCase("NE") == 0)
				connectionMask |= DIRECTION_NORTHEAST_MASK;
			else if(token.CompareNoCase("E") == 0)
				connectionMask |= DIRECTION_EAST_MASK;
			else if(token.CompareNoCase("SE") == 0)
				connectionMask |= DIRECTION_SOUTHEAST_MASK;
			else if(token.CompareNoCase("S") == 0)
				connectionMask |= DIRECTION_SOUTH_MASK;
			else if(token.CompareNoCase("SW") == 0)
				connectionMask |= DIRECTION_SOUTHWEST_MASK;
			else if(token.CompareNoCase("W") == 0)
				connectionMask |= DIRECTION_WEST_MASK;
			else
			{
				FErrorMsg("[Jason] Invalid connection direction.");
			}
		}

		FAssertMsg(connectionMask > 0, "[Jason] Did not find feature connection mask.");
		return connectionMask;
	}
}


int CvArtInfoFeature::getRotatedConnectionMask(int connectionMask, RotationTypes rotation) const
{
	if(rotation == ROTATE_90CW)
		connectionMask = connectionMask << 2; //rotate two directions CW
	else if(rotation == ROTATE_180CW)
		connectionMask = connectionMask << 4; //rotate four directions CW
	else if(rotation == ROTATE_270CW)
		connectionMask = connectionMask << 6; //rotate six directions CW

	//renormalize directions that wrapped around
	connectionMask = connectionMask | (connectionMask >> 8);
	connectionMask = connectionMask & 255;
	return connectionMask;
}

