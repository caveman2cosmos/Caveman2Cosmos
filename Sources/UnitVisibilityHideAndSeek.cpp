#include "CvGameCoreDLL.h"
#include "UnitVisibilityHideAndSeek.h"

CvWString UnitVisibilityHideAndSeek::getToolTipText() const
{
	std::vector<CvWString> lines;

	for (int iI = 0; iI < GC.getNumInvisibleInfos(); iI++)
	{
		const InvisibleTypes invisType = static_cast<InvisibleTypes>(iI);
		if (visibilityIntensityTotal(invisType) != 0)
		{
			lines.push_back(gDLL->getText("TXT_KEY_UNIT_VISIBILITY_INTENSITY", visibilityIntensityTotal(invisType), GC.getInvisibleInfo(invisType).getChar()));
		}
	}
	for (int iI = 0; iI < GC.getNumInvisibleInfos(); iI++)
	{
		const InvisibleTypes invisType = static_cast<InvisibleTypes>(iI);
		if (visibilityIntensityRangeTotal(invisType) != 0)
		{
			lines.push_back(gDLL->getText("TXT_KEY_UNIT_VISIBILITY_RANGE_INTENSITY", visibilityIntensityRangeTotal(invisType), GC.getInvisibleInfo(invisType).getChar()));
		}
	}
	for (int iI = 0; iI < GC.getNumInvisibleInfos(); iI++)
	{
		const InvisibleTypes invisType = static_cast<InvisibleTypes>(iI);
		if (visibilityIntensitySameTileTotal(invisType) != 0)
		{
			lines.push_back(gDLL->getText("TXT_KEY_UNIT_VISIBILITY_SAME_TILE_INTENSITY", visibilityIntensitySameTileTotal(invisType), GC.getInvisibleInfo(invisType).getChar()));
		}
	}

	for (int iI = 0; iI < GC.getNumInvisibleInfos(); iI++)
	{
		const InvisibleTypes invisType = static_cast<InvisibleTypes>(iI);
		if (!isNegatesInvisible(invisType))
		{
			if (invisibilityIntensityTotal(invisType) != 0 && !isNeverInvisible())
			{
				lines.push_back(gDLL->getText("TXT_KEY_UNIT_INVISIBILITY_INTENSITY", invisibilityIntensityTotal(invisType), GC.getInvisibleInfo(invisType).getChar()));
			}
		}
		else
		{
			lines.push_back(gDLL->getText("TXT_KEY_UNIT_INVISIBILITY_NEGATED", CvWString(GC.getInvisibleInfo(invisType).getType()).GetCString(), GC.getInvisibleInfo(invisType).getChar()));
		}
	}
	/*bFirst = true;
	for (int iI = 0; iI < GC.getNumInvisibleInfos(); iI++)
	{
		if (!isNegatesInvisible((InvisibleTypes)iI))
		{
			for (int iJ = 0; iJ < GC.getNumTerrainInfos(); iJ++)
			{
				if (extraInvisibleTerrain((InvisibleTypes)iI, (TerrainTypes)iJ) != 0)
				{
					if (!bFirst)
					{
						lines.push_back(gDLL->getText("TXT_KEY_COMMA"));
					}
					if (bFirst)
					{
						lines.push_back(NEWLINE);
						bFirst = false;
					}
					lines.push_back(gDLL->getText("TXT_KEY_UNIT_INVISIBILITY_PLOT_INTENSITY_ADJ", GC.getInvisibleInfo((InvisibleTypes)iI).getTextKeyWide(), extraInvisibleTerrain((InvisibleTypes)iI, (TerrainTypes)iJ), GC.getTerrainInfo((TerrainTypes)iJ).getTextKeyWide()));
				}
			}
		}
	}
	for (int iI = 0; iI < GC.getNumInvisibleInfos(); iI++)
	{
		if (!isNegatesInvisible((InvisibleTypes)iI))
		{
			for (int iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
			{
				if (extraInvisibleFeature((InvisibleTypes)iI, (FeatureTypes)iJ) != 0)
				{
					if (!bFirst)
					{
						lines.push_back(gDLL->getText("TXT_KEY_COMMA"));
					}
					if (bFirst)
					{
						lines.push_back(NEWLINE);
						bFirst = false;
					}
					lines.push_back(gDLL->getText("TXT_KEY_UNIT_INVISIBILITY_PLOT_INTENSITY_ADJ", GC.getInvisibleInfo((InvisibleTypes)iI).getTextKeyWide(), extraInvisibleFeature((InvisibleTypes)iI, (FeatureTypes)iJ), GC.getFeatureInfo((FeatureTypes)iJ).getTextKeyWide()));
				}
			}
		}
	}
	for (int iI = 0; iI < GC.getNumInvisibleInfos(); iI++)
	{
		if (!isNegatesInvisible((InvisibleTypes)iI))
		{
			for (int iJ = 0; iJ < GC.getNumImprovementInfos(); iJ++)
			{
				if (extraInvisibleImprovement((InvisibleTypes)iI, (ImprovementTypes)iJ) != 0)
				{
					if (!bFirst)
					{
						lines.push_back(gDLL->getText("TXT_KEY_COMMA"));
					}
					if (bFirst)
					{
						lines.push_back(NEWLINE);
						bFirst = false;
					}
					lines.push_back(gDLL->getText("TXT_KEY_UNIT_INVISIBILITY_PLOT_INTENSITY_ADJ", GC.getInvisibleInfo((InvisibleTypes)iI).getTextKeyWide(), extraInvisibleImprovement((InvisibleTypes)iI, (ImprovementTypes)iJ), GC.getImprovementInfo((ImprovementTypes)iJ).getTextKeyWide()));
				}
			}
		}
	}
	for (int iI = 0; iI < GC.getNumInvisibleInfos(); iI++)
	{
		for (int iJ = 0; iJ < GC.getNumTerrainInfos(); iJ++)
		{
			if (extraVisibleTerrain((InvisibleTypes)iI, (TerrainTypes)iJ) != 0)
			{
				if (!bFirst)
				{
					lines.push_back(gDLL->getText("TXT_KEY_COMMA"));
				}
				if (bFirst)
				{
					lines.push_back(NEWLINE);
					bFirst = false;
				}
				lines.push_back(gDLL->getText("TXT_KEY_UNIT_VISIBILITY_PLOT_INTENSITY_ADJ", GC.getInvisibleInfo((InvisibleTypes)iI).getTextKeyWide(), extraVisibleTerrain((InvisibleTypes)iI, (TerrainTypes)iJ), GC.getTerrainInfo((TerrainTypes)iJ).getTextKeyWide()));
			}
		}
	}
	for (int iI = 0; iI < GC.getNumInvisibleInfos(); iI++)
	{
		for (int iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
		{
			if (extraVisibleFeature((InvisibleTypes)iI, (FeatureTypes)iJ) != 0)
			{
				if (!bFirst)
				{
					lines.push_back(gDLL->getText("TXT_KEY_COMMA"));
				}
				if (bFirst)
				{
					lines.push_back(NEWLINE);
					bFirst = false;
				}
				lines.push_back(gDLL->getText("TXT_KEY_UNIT_VISIBILITY_PLOT_INTENSITY_ADJ", GC.getInvisibleInfo((InvisibleTypes)iI).getTextKeyWide(), extraVisibleFeature((InvisibleTypes)iI, (FeatureTypes)iJ), GC.getFeatureInfo((FeatureTypes)iJ).getTextKeyWide()));
			}
		}
	}
	for (int iI = 0; iI < GC.getNumInvisibleInfos(); iI++)
	{
		for (int iJ = 0; iJ < GC.getNumImprovementInfos(); iJ++)
		{
			if (extraVisibleImprovement((InvisibleTypes)iI, (ImprovementTypes)iJ) != 0)
			{
				if (!bFirst)
				{
					lines.push_back(gDLL->getText("TXT_KEY_COMMA"));
				}
				if (bFirst)
				{
					lines.push_back(NEWLINE);
					bFirst = false;
				}
				lines.push_back(gDLL->getText("TXT_KEY_UNIT_VISIBILITY_PLOT_INTENSITY_ADJ", GC.getInvisibleInfo((InvisibleTypes)iI).getTextKeyWide(), extraVisibleImprovement((InvisibleTypes)iI, (ImprovementTypes)iJ), GC.getImprovementInfo((ImprovementTypes)iJ).getTextKeyWide()));
			}
		}
	}
	for (int iI = 0; iI < GC.getNumInvisibleInfos(); iI++)
	{
		for (int iJ = 0; iJ < GC.getNumTerrainInfos(); iJ++)
		{
			if (extraVisibleTerrainRange((InvisibleTypes)iI, (TerrainTypes)iJ) != 0)
			{
				if (!bFirst)
				{
					lines.push_back(gDLL->getText("TXT_KEY_COMMA"));
				}
				if (bFirst)
				{
					lines.push_back(NEWLINE);
					bFirst = false;
				}
				lines.push_back(gDLL->getText("TXT_KEY_UNIT_VISIBILITY_PLOT_RANGE_INTENSITY_ADJ", GC.getInvisibleInfo((InvisibleTypes)iI).getTextKeyWide(), extraVisibleTerrainRange((InvisibleTypes)iI, (TerrainTypes)iJ), GC.getTerrainInfo((TerrainTypes)iJ).getTextKeyWide()));
			}
		}
	}
	for (int iI = 0; iI < GC.getNumInvisibleInfos(); iI++)
	{
		for (int iJ = 0; iJ < GC.getNumFeatureInfos(); iJ++)
		{
			if (extraVisibleFeatureRange((InvisibleTypes)iI, (FeatureTypes)iJ) != 0)
			{
				if (!bFirst)
				{
					lines.push_back(gDLL->getText("TXT_KEY_COMMA"));
				}
				if (bFirst)
				{
					lines.push_back(NEWLINE);
					bFirst = false;
				}
				lines.push_back(gDLL->getText("TXT_KEY_UNIT_VISIBILITY_PLOT_RANGE_INTENSITY_ADJ", GC.getInvisibleInfo((InvisibleTypes)iI).getTextKeyWide(), extraVisibleFeatureRange((InvisibleTypes)iI, (FeatureTypes)iJ), GC.getFeatureInfo((FeatureTypes)iJ).getTextKeyWide()));
			}
		}
	}
	for (int iI = 0; iI < GC.getNumInvisibleInfos(); iI++)
	{
		for (int iJ = 0; iJ < GC.getNumImprovementInfos(); iJ++)
		{
			if (extraVisibleImprovementRange((InvisibleTypes)iI, (ImprovementTypes)iJ) != 0)
			{
				if (!bFirst)
				{
					lines.push_back(gDLL->getText("TXT_KEY_COMMA"));
				}
				if (bFirst)
				{
					lines.push_back(NEWLINE);
					bFirst = false;
				}
				lines.push_back(gDLL->getText("TXT_KEY_UNIT_VISIBILITY_PLOT_RANGE_INTENSITY_ADJ", GC.getInvisibleInfo((InvisibleTypes)iI).getTextKeyWide(), extraVisibleImprovementRange((InvisibleTypes)iI, (ImprovementTypes)iJ), GC.getImprovementInfo((ImprovementTypes)iJ).getTextKeyWide()));
			}
		}
	}*/
	if (isNeverInvisible())
	{
		lines.push_back(gDLL->getText("TXT_KEY_NO_INVISIBILITY_TEXT"));
	}

	return bst::algorithm::join(lines, NEWLINE);
}
