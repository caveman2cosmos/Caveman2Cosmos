#pragma once

// The secondary layer is a set, limited to one member. Bit positions are an
// in-memory cache only; saves use remapped ImprovementTypes, never raw masks.
class CvTaggedSaveFormatWrapper;
class CvPlot;

class CvAdditionalImprovements
{
public:
	// Keep the original serialized width so older two-addition saves remain readable.
	enum { CAPACITY = 1, SAVE_CAPACITY = 2, MAX_TYPES = 64 };
	CvAdditionalImprovements();
	~CvAdditionalImprovements();
	void clear();
	int count() const;
	bool has(ImprovementTypes type) const;
	ImprovementTypes get(int slot) const;
	ImprovementTypes getRevealed(int slot, TeamTypes team) const;
	ImprovementTypes getReplacement(ImprovementTypes type) const;
	bool canAdd(ImprovementTypes type) const;
	bool set(ImprovementTypes oldType, ImprovementTypes newType);
	int getProgress(ImprovementTypes type) const;
	void setProgress(ImprovementTypes type, int progress);
	void reveal(TeamTypes team);
	void remember(TeamTypes team, ImprovementTypes type);
	void forget(TeamTypes team);
	void copyRevealed(TeamTypes to, TeamTypes from);
	void read(CvTaggedSaveFormatWrapper& wrapper);
	void write(CvTaggedSaveFormatWrapper& wrapper) const;
	void clearGraphics();
	CvPlot* getGraphicPlot(int slot) const;
	void setGraphicPlot(int slot, CvPlot* plot);

	static bool enabled();
	static bool usesSecondaryLayer(ImprovementTypes type);
	static void registerType(int bit, ImprovementTypes type);
	static bool sameFamily(ImprovementTypes first, ImprovementTypes second);
	static bool isUpgrade(ImprovementTypes from, ImprovementTypes to);
	static FeatureTypes featureFor(ImprovementTypes type);
	static FeatureTypes baseFeatureFor(ImprovementTypes type);
	static ImprovementTypes typeForFeature(FeatureTypes feature);

private:
	CvAdditionalImprovements(const CvAdditionalImprovements&);
	CvAdditionalImprovements& operator=(const CvAdditionalImprovements&);
	uint64_t m_mask;
	int m_progress[CAPACITY];
	short* m_revealed;
	CvPlot** m_graphicPlots;
};
