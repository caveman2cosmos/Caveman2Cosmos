#pragma once

// Cache XML effect presence, never a situational promotion score. Unit health,
// role, location, terrain weights and other live inputs are still evaluated.
// Presence is option-independent; option-dependent scoring stays with the caller.
class PromotionValueEffectCache
{
public:
	enum Effect { BUILD = 1, TERRAIN = 2, FEATURE = 4, FLANKING = 8, UNIT_COMBAT = 16 };
	PromotionValueEffectCache() : m_mask(0), m_valid(false) {}
	void invalidate() { m_valid = false; }

	template<class Promotion>
	unsigned int get(const Promotion& info, int builds, int terrains, int features, int combats) const
	{
		if (!m_valid)
		{
			m_mask = 0;
			for (int i = 0; i < builds; ++i)
				if (info.getBuildWorkRateModifierChangeType(i)) { m_mask |= BUILD; break; }
			for (int i = 0; i < terrains; ++i)
				if (info.getTerrainAttackPercent(i) || info.getTerrainDefensePercent(i)
				|| info.getTerrainWorkPercent(i) || info.getTerrainDoubleMove(i)
				|| info.getIgnoreTerrainDamage() == i || info.getWithdrawOnTerrainTypeChange(i))
				{ m_mask |= TERRAIN; break; }
			for (int i = 0; i < features; ++i)
				if (info.getFeatureAttackPercent(i) || info.getFeatureDefensePercent(i)
				|| info.getFeatureWorkPercent(i) || info.getFeatureDoubleMove(i)
				|| info.getWithdrawOnFeatureTypeChange(i))
				{ m_mask |= FEATURE; break; }
			for (int i = 0; i < combats; ++i)
			{
				if (info.getFlankingStrengthbyUnitCombatTypeChange(i)) m_mask |= FLANKING;
				if (info.getUnitCombatModifierPercent(i)) m_mask |= UNIT_COMBAT;
				if ((m_mask & (FLANKING | UNIT_COMBAT)) == (FLANKING | UNIT_COMBAT)) break;
			}
			m_valid = true;
		}
		return m_mask;
	}

private:
	mutable unsigned int m_mask;
	mutable bool m_valid;
};
