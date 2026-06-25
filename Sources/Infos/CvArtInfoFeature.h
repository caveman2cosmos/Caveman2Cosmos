#pragma once

#ifndef CV_ART_INFO_FEATURE_H
#define CV_ART_INFO_FEATURE_H

#include "CvInfoBase.h"
#include "CvArtInfoScalableAsset.h"

class CvArtInfoFeature
	: public CvArtInfoScalableAsset
	, private bst::noncopyable
{
public:

	CvArtInfoFeature();
	virtual ~CvArtInfoFeature();

	DllExport bool isAnimated() const;
	DllExport bool isRiverArt() const;
	DllExport TileArtTypes getTileArtType() const;
	DllExport LightTypes getLightType() const;

	void dump();

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(const CvArtInfoFeature* pClassInfo);

	class FeatureArtModel
	{
	public:
		FeatureArtModel(const CvString &modelFile, RotationTypes rotation)
			: m_szModelFile(modelFile)
			, m_eRotation(rotation)
		{}

		const CvString& getModelFile() const
		{
			return m_szModelFile;
		}

		RotationTypes getRotation() const
		{
			return m_eRotation;
		}

	private:
		CvString m_szModelFile;
		RotationTypes m_eRotation;
	};

	class FeatureArtPiece
	{
	public:
		explicit FeatureArtPiece(int connectionMask)
		{
			m_iConnectionMask = connectionMask;
		}

		int getConnectionMask() const
		{
			return m_iConnectionMask;
		}

		int getNumArtModels() const
		{
			return m_aArtModels.size();
		}

		const FeatureArtModel& getArtModel(int index) const
		{
			FASSERT_BOUNDS(0, (int)m_aArtModels.size(), index);
			return m_aArtModels[index];
		}

	private:
		std::vector<FeatureArtModel> m_aArtModels;
		int m_iConnectionMask;

		friend CvArtInfoFeature;
	};

	class FeatureDummyNode
	{
	public:
		FeatureDummyNode(const CvString &tagName, const CvString &nodeName)
			: m_szTag(tagName)
			, m_szName(nodeName)
		{}

		const CvString getTagName() const
		{
			return m_szTag;
		}

		const CvString getNodeName() const
		{
			return m_szName;
		}

	private:
		CvString m_szTag;
		CvString m_szName;
	};

	class FeatureVariety
	{
	public:
		FeatureVariety()
		{
		}

		const CvString& getVarietyButton() const
		{
			return m_szVarietyButton;
		}

		const FeatureArtPiece& getFeatureArtPiece(int index) const
		{
			FASSERT_BOUNDS(0, (int)m_aFeatureArtPieces.size(), index);
			return m_aFeatureArtPieces[index];
		}

		const FeatureArtPiece& getFeatureArtPieceFromConnectionMask(int connectionMask) const
		{
			foreach_(const FeatureArtPiece& art, m_aFeatureArtPieces)
				if (art.getConnectionMask() == connectionMask)
					return art;

			FErrorMsg("[Jason] Failed to find feature art piece with valid connection mask.");
			return m_aFeatureArtPieces[0];
		}

		const CvString getFeatureDummyNodeName(const CvString &tagName) const
		{
			foreach_(const FeatureDummyNode& dummy, m_aFeatureDummyNodes)
			{
				if (dummy.getTagName().CompareNoCase(tagName) == 0)
					return dummy.getNodeName();
			}

			FErrorMsg("[Jason] Failed to find dummy tag name.");
			return "";
		}

		const CvString getFeatureDummyTag(const CvString &nodeName) const
		{
			foreach_(const FeatureDummyNode& dummy, m_aFeatureDummyNodes)
			{
				if (dummy.getNodeName().CompareNoCase(nodeName) == 0)
					return dummy.getTagName();
			}

			return "";
		}

		FeatureArtPiece& createFeatureArtPieceFromConnectionMask(int connectionMask)
		{
			foreach_(FeatureArtPiece& art, m_aFeatureArtPieces)
				if (art.getConnectionMask() == connectionMask)
					return art;

			m_aFeatureArtPieces.push_back(FeatureArtPiece(connectionMask));
			return m_aFeatureArtPieces.back();
		}

		void createFeatureDummyNode(const CvString &tagName, const CvString &nodeName)
		{
			m_aFeatureDummyNodes.push_back(FeatureDummyNode(tagName, nodeName));
		}

	protected:
		std::vector<FeatureArtPiece> m_aFeatureArtPieces;
		std::vector<FeatureDummyNode> m_aFeatureDummyNodes;
		CvString m_szVarietyButton;

		friend CvArtInfoFeature;
	};

	DllExport const FeatureVariety &getVariety(int index) const;
	DllExport int getNumVarieties() const;
	const std::string getFeatureDummyNodeName(int variety, const std::string tagName) const;

protected:

	int getConnectionMaskFromString(const CvString& connectionString) const;
	int getRotatedConnectionMask(int connectionMask, RotationTypes rotation) const;

	bool m_bAnimated;
	bool m_bRiverArt;
	TileArtTypes m_eTileArtType;
	LightTypes m_eLightType;
	std::vector<FeatureVariety> m_aFeatureVarieties;
};

#endif // CV_ART_INFO_FEATURE_H
