#include "CvGameCoreDLL.h"
#include "CvAdditionalImprovements.h"
#include "CvGameAI.h"
#include "CvImprovementInfo.h"
#include "CvTaggedSaveFormatWrapper.h"
#include "CvPlot.h"

namespace
{
	std::vector<ImprovementTypes> additionalTypes;
	std::vector<int> families;
	bool familiesDirty = true;
	std::vector<uint64_t> upgradeMasks;
	uint64_t cachedUpgradeRows = 0;

	uint64_t bitFor(ImprovementTypes type)
	{
		if (type == NO_IMPROVEMENT) return 0;
		const int bit = GC.getImprovementInfo(type).getSecondaryLayerBit();
		return bit >= 0 && bit < CvAdditionalImprovements::MAX_TYPES ? uint64_t(1) << bit : 0;
	}

	int root(int bit)
	{
		while (families[bit] != bit) bit = families[bit];
		return bit;
	}

	void join(int bit, ImprovementTypes other)
	{
		if (other == NO_IMPROVEMENT) return;
		const int otherBit = GC.getImprovementInfo(other).getSecondaryLayerBit();
		if (otherBit >= 0 && otherBit < (int)families.size()) families[root(bit)] = root(otherBit);
	}

	void buildFamilies()
	{
		if (!familiesDirty) return;
		families.resize(additionalTypes.size());
		for (int bit = 0; bit < (int)families.size(); ++bit) families[bit] = bit;
		for (int bit = 0; bit < (int)additionalTypes.size(); ++bit)
		{
			const CvImprovementInfo& info = GC.getImprovementInfo(additionalTypes[bit]);
			join(bit, info.getImprovementUpgrade());
			join(bit, info.getImprovementPillage());
			for (int i = 0; i < info.getNumAlternativeImprovementUpgradeTypes(); ++i)
				join(bit, (ImprovementTypes)info.getAlternativeImprovementUpgradeType(i));
		}
		familiesDirty = false;
	}
}

CvAdditionalImprovements::CvAdditionalImprovements() : m_mask(0), m_revealed(NULL), m_graphicPlots(NULL)
{
	for (int i = 0; i < CAPACITY; ++i) m_progress[i] = 0;
}

CvAdditionalImprovements::~CvAdditionalImprovements() { clear(); }

void CvAdditionalImprovements::clear()
{
	clearGraphics();
	m_mask = 0;
	for (int i = 0; i < CAPACITY; ++i) m_progress[i] = 0;
	SAFE_DELETE_ARRAY(m_revealed);
}

void CvAdditionalImprovements::clearGraphics()
{
	if (!m_graphicPlots) return;
	for (int i = 0; i < CAPACITY; ++i) delete m_graphicPlots[i];
	SAFE_DELETE_ARRAY(m_graphicPlots);
}

CvPlot* CvAdditionalImprovements::getGraphicPlot(int slot) const
{
	return m_graphicPlots ? m_graphicPlots[slot] : NULL;
}

void CvAdditionalImprovements::setGraphicPlot(int slot, CvPlot* plot)
{
	if (!m_graphicPlots)
	{
		if (!plot) return;
		m_graphicPlots = new CvPlot*[CAPACITY];
		std::fill(m_graphicPlots, m_graphicPlots + CAPACITY, (CvPlot*)NULL);
	}
	delete m_graphicPlots[slot];
	m_graphicPlots[slot] = plot;
}

bool CvAdditionalImprovements::enabled()
{
	return GC.getGame().isOption(GAMEOPTION_MAP_MULTIPLE_IMPROVEMENTS);
}

bool CvAdditionalImprovements::usesSecondaryLayer(ImprovementTypes type)
{
	return type != NO_IMPROVEMENT && enabled() && GC.getImprovementInfo(type).isSecondaryLayer();
}

void CvAdditionalImprovements::registerType(int bit, ImprovementTypes type)
{
	FAssertMsg(bit < MAX_TYPES, "Too many secondary improvement types for the bitmask");
	if (bit == 0) additionalTypes.clear();
	if (bit >= MAX_TYPES) return;
	additionalTypes.resize(bit + 1, NO_IMPROVEMENT);
	additionalTypes[bit] = type;
	familiesDirty = true;
	cachedUpgradeRows = 0;
	upgradeMasks.clear();
}

bool CvAdditionalImprovements::sameFamily(ImprovementTypes first, ImprovementTypes second)
{
	if (first == NO_IMPROVEMENT || second == NO_IMPROVEMENT) return false;
	if (first == second) return true;
	const int a = GC.getImprovementInfo(first).getSecondaryLayerBit();
	const int b = GC.getImprovementInfo(second).getSecondaryLayerBit();
	buildFamilies();
	return a >= 0 && b >= 0 && a < (int)families.size() && b < (int)families.size() && root(a) == root(b);
}

bool CvAdditionalImprovements::isUpgrade(ImprovementTypes from, ImprovementTypes to)
{
    if (from == NO_IMPROVEMENT || to == NO_IMPROVEMENT || from == to) return false;
    const uint64_t fromBit = bitFor(from);
    const uint64_t toBit = bitFor(to);
    const bool cacheable = fromBit != 0 && toBit != 0;
    const int row = cacheable ? GC.getImprovementInfo(from).getSecondaryLayerBit() : 0;
    if (cacheable && (cachedUpgradeRows & fromBit)) return (upgradeMasks[row] & toBit) != 0;

    // XML is fixed after registration. Traverse a source's upgrade graph only
    // once, retaining reverse pillage links and paths through untagged types.
    uint64_t reachable = 0;
    std::vector<ImprovementTypes> pending(1, from);
    std::vector<bool> visited(GC.getNumImprovementInfos(), false);
    for (size_t i = 0; i < pending.size(); ++i)
    {
        const ImprovementTypes current = pending[i];
        if (current < 0 || current >= GC.getNumImprovementInfos()) continue;
        if (!cacheable && current == to) return true;
        if (visited[current]) continue;
        visited[current] = true;
        reachable |= bitFor(current);
        const CvImprovementInfo& info = GC.getImprovementInfo(current);
        for (int j = 0; j < (int)additionalTypes.size(); ++j)
            if (GC.getImprovementInfo(additionalTypes[j]).getImprovementPillage() == current) pending.push_back(additionalTypes[j]);
        if (info.getImprovementUpgrade() != NO_IMPROVEMENT) pending.push_back(info.getImprovementUpgrade());
        for (int j = 0; j < info.getNumAlternativeImprovementUpgradeTypes(); ++j)
            pending.push_back((ImprovementTypes)info.getAlternativeImprovementUpgradeType(j));
    }
    if (cacheable)
    {
        if (upgradeMasks.size() < additionalTypes.size()) upgradeMasks.resize(additionalTypes.size(), 0);
        upgradeMasks[row] = reachable;
        cachedUpgradeRows |= fromBit;
    }
    return cacheable && (reachable & toBit) != 0;
}

int CvAdditionalImprovements::count() const
{
	int result = 0;
	for (uint64_t mask = m_mask; mask; mask &= mask - 1) ++result;
	return result;
}

bool CvAdditionalImprovements::has(ImprovementTypes type) const { return (m_mask & bitFor(type)) != 0; }

ImprovementTypes CvAdditionalImprovements::get(int slot) const
{
    if (slot < 0 || slot >= CAPACITY || !m_mask) return NO_IMPROVEMENT;
    // One occupied slot: decode its bit without scanning all registered types.
    uint64_t mask = m_mask;
    int bit = 0;
    if (!(mask & 0xffffffffui64)) { mask >>= 32; bit += 32; }
    if (!(mask & 0xffffui64)) { mask >>= 16; bit += 16; }
    if (!(mask & 0xffui64)) { mask >>= 8; bit += 8; }
    if (!(mask & 0xfui64)) { mask >>= 4; bit += 4; }
    if (!(mask & 0x3ui64)) { mask >>= 2; bit += 2; }
    if (!(mask & 0x1ui64)) ++bit;
    return bit < (int)additionalTypes.size() ? additionalTypes[bit] : NO_IMPROVEMENT;
}

ImprovementTypes CvAdditionalImprovements::getReplacement(ImprovementTypes type) const
{
    const ImprovementTypes current = get(0);
    return sameFamily(current, type) ? current : NO_IMPROVEMENT;
}

bool CvAdditionalImprovements::canAdd(ImprovementTypes type) const
{
	if (!bitFor(type) || has(type)) return false;
	const ImprovementTypes old = getReplacement(type);
	return old != NO_IMPROVEMENT ? isUpgrade(old, type) : count() < CAPACITY;
}

bool CvAdditionalImprovements::set(ImprovementTypes oldType, ImprovementTypes newType)
{
	if (oldType != NO_IMPROVEMENT && !has(oldType)) return false;
	if (newType != NO_IMPROVEMENT)
	{
		if (!bitFor(newType) || has(newType)) return false;
		const ImprovementTypes replacement = getReplacement(newType);
		if (replacement != NO_IMPROVEMENT && replacement != oldType) return false;
		if (oldType == NO_IMPROVEMENT && count() >= CAPACITY) return false;
	}
	ImprovementTypes previous[CAPACITY];
	int progress[CAPACITY];
	for (int i = 0; i < CAPACITY; ++i) { previous[i] = get(i); progress[i] = m_progress[i]; }
	m_mask &= ~bitFor(oldType);
	m_mask |= bitFor(newType);
	for (int i = 0; i < CAPACITY; ++i)
	{
		m_progress[i] = 0;
		for (int j = 0; j < CAPACITY; ++j)
			if (get(i) != NO_IMPROVEMENT && get(i) == previous[j] && get(i) != newType) m_progress[i] = progress[j];
	}
	return true;
}

int CvAdditionalImprovements::getProgress(ImprovementTypes type) const
{
	for (int i = 0; i < CAPACITY; ++i) if (get(i) == type) return m_progress[i];
	return 0;
}

void CvAdditionalImprovements::setProgress(ImprovementTypes type, int progress)
{
	for (int i = 0; i < CAPACITY; ++i) if (get(i) == type) m_progress[i] = std::max(0, progress);
}

ImprovementTypes CvAdditionalImprovements::getRevealed(int slot, TeamTypes team) const
{
	return m_revealed && team >= 0 && team < MAX_TEAMS && slot >= 0 && slot < CAPACITY
		? (ImprovementTypes)m_revealed[CAPACITY * team + slot] : NO_IMPROVEMENT;
}

void CvAdditionalImprovements::reveal(TeamTypes team)
{
	if (team < 0 || team >= MAX_TEAMS || (!m_revealed && !m_mask)) return;
	if (!m_revealed)
	{
		m_revealed = new short[CAPACITY * MAX_TEAMS];
		std::fill(m_revealed, m_revealed + CAPACITY * MAX_TEAMS, (short)NO_IMPROVEMENT);
	}
	for (int i = 0; i < CAPACITY; ++i) m_revealed[CAPACITY * team + i] = (short)get(i);
}

void CvAdditionalImprovements::forget(TeamTypes team)
{
	if (m_revealed && team >= 0 && team < MAX_TEAMS)
		for (int i = 0; i < CAPACITY; ++i) m_revealed[CAPACITY * team + i] = NO_IMPROVEMENT;
}

void CvAdditionalImprovements::remember(TeamTypes team, ImprovementTypes type)
{
	if (team < 0 || team >= MAX_TEAMS) return;
	if (!m_revealed)
	{
		m_revealed = new short[CAPACITY * MAX_TEAMS];
		std::fill(m_revealed, m_revealed + CAPACITY * MAX_TEAMS, (short)NO_IMPROVEMENT);
	}
	m_revealed[CAPACITY * team] = (short)type;
}

void CvAdditionalImprovements::copyRevealed(TeamTypes to, TeamTypes from)
{
	if (!m_revealed || to < 0 || from < 0 || to >= MAX_TEAMS || from >= MAX_TEAMS) return;
	for (int i = 0; i < CAPACITY; ++i) m_revealed[CAPACITY * to + i] = m_revealed[CAPACITY * from + i];
}

void CvAdditionalImprovements::read(CvTaggedSaveFormatWrapper& wrapper)
{
	clear();
	short types[SAVE_CAPACITY] = { NO_IMPROVEMENT, NO_IMPROVEMENT };
	int progress[SAVE_CAPACITY] = { 0, 0 };
	WRAPPER_READ_CLASS_ENUM_ARRAY_ALLOW_MISSING(wrapper, "CvAdditionalImprovements", REMAPPED_CLASS_TYPE_IMPROVEMENTS, SAVE_CAPACITY, types);
	WRAPPER_READ_ARRAY(wrapper, "CvAdditionalImprovements", SAVE_CAPACITY, progress);
	// Older versions allowed two additions. Keep the first valid serialized type.
	for (int i = 0; i < SAVE_CAPACITY; ++i)
		if (types[i] != NO_IMPROVEMENT && set(NO_IMPROVEMENT, (ImprovementTypes)types[i])) setProgress((ImprovementTypes)types[i], progress[i]);
	bool revealed = false;
	WRAPPER_READ(wrapper, "CvAdditionalImprovements", &revealed);
	if (revealed)
	{
		this->m_revealed = new short[CAPACITY * MAX_TEAMS];
		std::fill(this->m_revealed, this->m_revealed + CAPACITY * MAX_TEAMS, (short)NO_IMPROVEMENT);
		// The local name preserves the existing tagged-save field name.
		short m_revealed[SAVE_CAPACITY * MAX_TEAMS];
		std::fill(m_revealed, m_revealed + SAVE_CAPACITY * MAX_TEAMS, (short)NO_IMPROVEMENT);
		WRAPPER_READ_CLASS_ENUM_ARRAY_ALLOW_MISSING(wrapper, "CvAdditionalImprovements", REMAPPED_CLASS_TYPE_IMPROVEMENTS, SAVE_CAPACITY * MAX_TEAMS, m_revealed);
		for (int team = 0; team < MAX_TEAMS; ++team)
			for (int i = 0; i < SAVE_CAPACITY; ++i)
			{
				const ImprovementTypes type = (ImprovementTypes)m_revealed[SAVE_CAPACITY * team + i];
				if (bitFor(type))
				{
					this->m_revealed[CAPACITY * team] = (short)type;
					break;
				}
			}
	}
}

void CvAdditionalImprovements::write(CvTaggedSaveFormatWrapper& wrapper) const
{
	short types[SAVE_CAPACITY];
	int progress[SAVE_CAPACITY];
	std::fill(types, types + SAVE_CAPACITY, (short)NO_IMPROVEMENT);
	std::fill(progress, progress + SAVE_CAPACITY, 0);
	for (int i = 0; i < CAPACITY; ++i) { types[i] = (short)get(i); progress[i] = m_progress[i]; }
	WRAPPER_WRITE_CLASS_ENUM_ARRAY(wrapper, "CvAdditionalImprovements", REMAPPED_CLASS_TYPE_IMPROVEMENTS, SAVE_CAPACITY, types);
	WRAPPER_WRITE_ARRAY(wrapper, "CvAdditionalImprovements", SAVE_CAPACITY, progress);
	const bool revealed = m_revealed != NULL;
	WRAPPER_WRITE(wrapper, "CvAdditionalImprovements", revealed);
	if (revealed)
	{
		short m_revealed[SAVE_CAPACITY * MAX_TEAMS];
		std::fill(m_revealed, m_revealed + SAVE_CAPACITY * MAX_TEAMS, (short)NO_IMPROVEMENT);
		for (int team = 0; team < MAX_TEAMS; ++team)
			for (int i = 0; i < CAPACITY; ++i) m_revealed[SAVE_CAPACITY * team + i] = this->m_revealed[CAPACITY * team + i];
		WRAPPER_WRITE_CLASS_ENUM_ARRAY(wrapper, "CvAdditionalImprovements", REMAPPED_CLASS_TYPE_IMPROVEMENTS, SAVE_CAPACITY * MAX_TEAMS, m_revealed);
	}
}
