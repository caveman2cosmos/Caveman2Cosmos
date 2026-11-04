#include "CvGameCoreDLL.h"
#include "UnitTravelConnectivity.h"
#include "CvPlot.h"
#include "CvAdditionalImprovements.h"
#include "CvImprovementInfo.h"
#include "CvInfos.h"
#include "CvPlayerAI.h"
#include "CvTeamAI.h"
#include "CvCityAI.h"
#include "CvGameAI.h"
#include "CvEventReporter.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvArea.h"
#include "CvUnit.h"
#include "CvDLLPlotBuilderIFaceBase.h"

// Feature-backed additions retain the original terrain effects and art.
// The mask records their slot, identity and building/AI visibility.
FeatureTypes CvAdditionalImprovements::featureFor(ImprovementTypes type)
{
	if (type == NO_IMPROVEMENT || !GC.getImprovementInfo(type).isPlacesFeature()) return NO_FEATURE;
	foreach_(const BuildTypes build, GC.getImprovementInfo(type).getBuildTypes())
		if (GC.getBuildInfo(build).getFeatureChange() != NO_FEATURE) return (FeatureTypes)GC.getBuildInfo(build).getFeatureChange();
	return NO_FEATURE;
}

ImprovementTypes CvAdditionalImprovements::typeForFeature(FeatureTypes feature)
{
	if (feature == NO_FEATURE) return NO_IMPROVEMENT;
	for (int i = 0; i < GC.getNumImprovementInfos(); ++i)
	{
		const ImprovementTypes type = (ImprovementTypes)i;
		if (usesSecondaryLayer(type) && featureFor(type) == feature) return type;
	}
	return NO_IMPROVEMENT;
}

FeatureTypes CvAdditionalImprovements::baseFeatureFor(ImprovementTypes type)
{
	if (featureFor(type) == NO_FEATURE) return NO_FEATURE;
	for (int i = 0; i < GC.getNumFeatureInfos(); ++i)
		if (GC.getImprovementInfo(type).getFeatureMakesValid(i) && typeForFeature((FeatureTypes)i) == NO_IMPROVEMENT) return (FeatureTypes)i;
	return NO_FEATURE;
}

int CvPlot::getNumAdditionalImprovements() const
{
	return CvAdditionalImprovements::enabled() ? m_additionalImprovements.count() : 0;
}

ImprovementTypes CvPlot::getAdditionalImprovementType(int slot) const
{
	return CvAdditionalImprovements::enabled() ? m_additionalImprovements.get(slot) : NO_IMPROVEMENT;
}

ImprovementTypes CvPlot::getRevealedAdditionalImprovementType(int slot, TeamTypes team) const
{
	if (!CvAdditionalImprovements::enabled()) return NO_IMPROVEMENT;
	return GC.getGame().isDebugMode() ? m_additionalImprovements.get(slot) : m_additionalImprovements.getRevealed(slot, team);
}

bool CvPlot::hasImprovement(ImprovementTypes type) const
{
	return type != NO_IMPROVEMENT && (getImprovementType() == type || (CvAdditionalImprovements::enabled() && m_additionalImprovements.has(type)));
}

bool CvPlot::hasAnyImprovement() const
{
	return getImprovementType() != NO_IMPROVEMENT || getNumAdditionalImprovements() > 0;
}

ImprovementTypes CvPlot::getImprovementInLayer(ImprovementTypes type) const
{
	return CvAdditionalImprovements::usesSecondaryLayer(type) ? m_additionalImprovements.getReplacement(type) : getImprovementType();
}

ImprovementTypes CvPlot::getMilitaryImprovementType(TeamTypes revealedTeam) const
{
	ImprovementTypes best = revealedTeam == NO_TEAM ? getImprovementType() : getRevealedImprovementType(revealedTeam);
	if (!CvAdditionalImprovements::enabled()) return best;
	for (int i = 0; i < CvAdditionalImprovements::CAPACITY; ++i)
	{
		const ImprovementTypes type = revealedTeam == NO_TEAM ? getAdditionalImprovementType(i) : getRevealedAdditionalImprovementType(i, revealedTeam);
		if (type != NO_IMPROVEMENT && GC.getImprovementInfo(type).isMilitaryStructure()
		&& (best == NO_IMPROVEMENT
			|| (GC.getImprovementInfo(type).isActsAsCity() && !GC.getImprovementInfo(best).isActsAsCity())
			|| (GC.getImprovementInfo(type).isActsAsCity() == GC.getImprovementInfo(best).isActsAsCity()
				&& GC.getImprovementInfo(type).getDefenseModifier() > GC.getImprovementInfo(best).getDefenseModifier()))) best = type;
	}
	return best;
}

ImprovementTypes CvPlot::getPillageImprovementType() const
{
	// Strip the complementary layer before touching the primary improvement.
	if (getNumAdditionalImprovements() > 0) return getAdditionalImprovementType(0);
	return getImprovementType();
}

void CvPlot::pillageImprovement()
{
	const ImprovementTypes type = getPillageImprovementType();
	if (type == NO_IMPROVEMENT) return;
	const ImprovementTypes downgrade = GC.getImprovementInfo(type).getImprovementPillage();
	if (m_additionalImprovements.has(type)) setAdditionalImprovementType(downgrade, type);
	else setImprovementType(downgrade);
}

bool CvPlot::canAddAdditionalImprovement(ImprovementTypes type) const
{
	return CvAdditionalImprovements::usesSecondaryLayer(type) && m_additionalImprovements.canAdd(type);
}

bool CvPlot::setAdditionalImprovementType(ImprovementTypes type, ImprovementTypes oldType)
{
	if (!CvAdditionalImprovements::enabled()) return false;
	if (CvAdditionalImprovements::usesSecondaryLayer(getImprovementType()) && !migrateAdditionalImprovement()) return false;
	if (type != NO_IMPROVEMENT)
	{
		if (!CvAdditionalImprovements::usesSecondaryLayer(type)) return false;
		if (oldType == NO_IMPROVEMENT) oldType = m_additionalImprovements.getReplacement(type);
		if (type == oldType || (oldType == NO_IMPROVEMENT && getNumAdditionalImprovements() >= CvAdditionalImprovements::CAPACITY)) return false;
	}
	else if (oldType == NO_IMPROVEMENT) return false;

	// Validate before removing any counters, sight or plot-group contributions.
	if (oldType != NO_IMPROVEMENT && !m_additionalImprovements.has(oldType)) return false;
	if (type != NO_IMPROVEMENT && m_additionalImprovements.has(type)) return false;
	const ImprovementTypes replacement = type == NO_IMPROVEMENT ? NO_IMPROVEMENT : m_additionalImprovements.getReplacement(type);
	if (replacement != NO_IMPROVEMENT && replacement != oldType) return false;

    UnitTravelConnectivity::PlotChange travelChange(this,UnitTravelConnectivity::relevantImprovement(type)?UnitTravelConnectivity::SEA:0);
	const bool hadAnyImprovement = hasAnyImprovement();
	updateSight(false, true);
	if (isOwned())
	{
		GET_PLAYER(getOwner()).startDeferredPlotGroupBonusCalculation();
		updatePlotGroupBonus(false);
	}
	const bool changed = m_additionalImprovements.set(oldType, type);
	FAssert(changed);
	if (oldType != NO_IMPROVEMENT)
	{
		if (area()) area()->changeNumImprovements(oldType, -1);
		if (isOwned()) GET_PLAYER(getOwner()).changeImprovementCount(oldType, -1);
	}
	if (type != NO_IMPROVEMENT)
	{
		if (area()) area()->changeNumImprovements(type, 1);
		if (isOwned()) GET_PLAYER(getOwner()).changeImprovementCount(type, 1);
	}
	if (isOwned())
	{
		updatePlotGroupBonus(true);
		GET_PLAYER(getOwner()).endDeferredPlotGroupBonusCalculation();
	}
	for (int team = 0; team < MAX_TEAMS; ++team)
		if (GET_TEAM((TeamTypes)team).isAlive() && isVisible((TeamTypes)team, false)) m_additionalImprovements.reveal((TeamTypes)team);
	const ImprovementTypes defense = getMilitaryImprovementType();
	m_iDefenseDamage = defense == NO_IMPROVEMENT ? 0 : std::min(m_iDefenseDamage, GC.getImprovementInfo(defense).getDefenseModifier());
	updateSight(true, true);
	if (getBonusType() != NO_BONUS && hadAnyImprovement != hasAnyImprovement()) ToggleInPlotGroupsZobristContributors();
	updatePlotGroup();
	updateIrrigated();
	const FeatureTypes newFeature = CvAdditionalImprovements::featureFor(type);
	const FeatureTypes oldFeature = CvAdditionalImprovements::featureFor(oldType);
	if (newFeature != NO_FEATURE) setFeatureType(newFeature, -1, true);
	else if (oldFeature != NO_FEATURE && getFeatureType() == oldFeature) setFeatureType(CvAdditionalImprovements::baseFeatureFor(oldType), -1, true);
	updateYield();
	if (getWorkingCity()) getWorkingCity()->AI_setAssignWorkDirty(true);
	for (int i = 0; i < NUM_CITY_PLOTS; ++i)
	{
		CvPlot* plot = plotCity(getX(), getY(), i);
		CvCity* city = plot ? plot->getPlotCity() : NULL;
		if (city)
		{
			city->updateFeatureHappiness();
			city->updateImprovementHealth();
			city->AI_markBestBuildValuesStale();
			city->AI_setAssignWorkDirty(true);
		}
	}
	if (type != NO_IMPROVEMENT && isOwned() && GC.getImprovementInfo(type).getCulture() > 0)
		pushCultureFromImprovement(getOwner(), 1, GC.getImprovementInfo(type).getCultureRange(), true);
	if (oldType != NO_IMPROVEMENT && GC.getImprovementInfo(oldType).isActsAsCity()) verifyUnitValidPlot();
	setLayoutDirty(true);
	gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(CitizenButtons_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(Help_DIRTY_BIT, true);
	gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
	if (oldType != NO_IMPROVEMENT) CvEventReporter::getInstance().improvementDestroyed(oldType, getOwner(), getX(), getY());
	if (type != NO_IMPROVEMENT) CvEventReporter::getInstance().improvementBuilt(type, getX(), getY());
	return changed;
}

bool CvPlot::migrateAdditionalImprovement()
{
	const ImprovementTypes type = getImprovementType();
	if (!CvAdditionalImprovements::usesSecondaryLayer(type))
	{
		// Existing saves store lighthouses/beacons as reef features only.
		const ImprovementTypes featureType = CvAdditionalImprovements::typeForFeature(getFeatureType());
		if (featureType == NO_IMPROVEMENT || m_additionalImprovements.has(featureType)) return true;
		if (!m_additionalImprovements.set(NO_IMPROVEMENT, featureType)) return false;
		for (int team = 0; team < MAX_TEAMS; ++team)
			if (isVisible((TeamTypes)team, false)) m_additionalImprovements.remember((TeamTypes)team, featureType);
		return true;
	}
	if (!m_additionalImprovements.set(NO_IMPROVEMENT, type)) return false;
	m_additionalImprovements.setProgress(type, m_iUpgradeProgress);
	// Moving an existing object does not build/destroy it or change its counters,
	// culture, sight or damage. The surrounding recalculation rebuilds caches.
	for (int team = 0; team < MAX_TEAMS; ++team)
	{
		const ImprovementTypes remembered = getRevealedImprovementType((TeamTypes)team, false);
		if (remembered != NO_IMPROVEMENT && GC.getImprovementInfo(remembered).isSecondaryLayer())
		{
			m_additionalImprovements.remember((TeamTypes)team, remembered);
			m_aeRevealedImprovementType[team] = NO_IMPROVEMENT;
		}
	}
	m_eImprovementType = NO_IMPROVEMENT;
	m_iUpgradeProgress = 0;
	m_bImprovementUpgradable = false;
	return true;
}

void CvPlot::doAdditionalImprovementUpgrades()
{
	const int count = getNumAdditionalImprovements();
	if (!count) return;
	ImprovementTypes types[CvAdditionalImprovements::CAPACITY];
	for (int i = 0; i < count; ++i) types[i] = m_additionalImprovements.get(i);
	for (int i = 0; i < count; ++i)
	{
		const ImprovementTypes type = types[i];
		if (type == NO_IMPROVEMENT) continue;
		const CvImprovementInfo& info = GC.getImprovementInfo(type);
		if (!isBeingWorked() && !info.isUpgradeRequiresFortify()) continue;
		if (info.isUpgradeRequiresFortify() && !algo::any_of(units(), CvUnit::fn::getFortifyTurns() > 0 && CvUnit::fn::getTeam() == getTeam() && CvUnit::fn::canDefend())) continue;
		const ImprovementTypes upgrade = info.getImprovementUpgrade();
		if (!CvAdditionalImprovements::usesSecondaryLayer(upgrade) || !canHaveImprovement(upgrade, getTeam(), false, true)) continue;
		const int progress = m_additionalImprovements.getProgress(type) + GET_PLAYER(getOwner()).getImprovementUpgradeProgressRate(type);
		m_additionalImprovements.setProgress(type, progress);
		if (progress >= 100 * GC.getGame().getImprovementUpgradeTime(type)) setAdditionalImprovementType(upgrade, type);
	}
}

int CvPlot::calculateAdditionalImprovementYield(YieldTypes yield, PlayerTypes player, ImprovementTypes replacement, TeamTypes revealedTeam) const
{
	if (!CvAdditionalImprovements::enabled()) return 0;
	// Remembered layers may differ from live ones under fog; do not use a live
	// empty-mask shortcut when the caller requests a team's remembered view.
	const bool remembered = revealedTeam != NO_TEAM && !GC.getGame().isDebugMode();
	int result = 0;
	for (int i = 0; i < CvAdditionalImprovements::CAPACITY; ++i)
	{
		const ImprovementTypes type = remembered ? m_additionalImprovements.getRevealed(i, revealedTeam) : m_additionalImprovements.get(i);
		if (type != NO_IMPROVEMENT && (replacement == NO_IMPROVEMENT || !CvAdditionalImprovements::sameFamily(type, replacement)))
			result += calculateImprovementYieldChange(type, yield, player);
	}
	return result;
}

int CvPlot::calculateYieldWithImprovement(ImprovementTypes type, YieldTypes yield, PlayerTypes player, bool bestRoute) const
{
	int result = calculateNatureYield(yield, player == NO_PLAYER ? NO_TEAM : GET_PLAYER(player).getTeam());
	if (CvAdditionalImprovements::enabled())
	{
		if (type != NO_IMPROVEMENT && GC.getImprovementInfo(type).isSecondaryLayer())
		{
			if (getImprovementType() != NO_IMPROVEMENT) result += calculateImprovementYieldChange(getImprovementType(), yield, player, false, bestRoute);
			result += calculateAdditionalImprovementYield(yield, player, type);
		}
		else result += calculateAdditionalImprovementYield(yield, player);
	}
	if (type != NO_IMPROVEMENT) result += calculateImprovementYieldChange(type, yield, player, false, bestRoute);
	return result;
}

bool CvPlot::isImprovementBonusTrade(BonusTypes bonus) const
{
	if (getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(getImprovementType()).isImprovementBonusTrade(bonus)) return true;
	for (int i = 0; i < getNumAdditionalImprovements(); ++i)
		if (GC.getImprovementInfo(getAdditionalImprovementType(i)).isImprovementBonusTrade(bonus)) return true;
	return false;
}

bool CvPlot::isImprovementBonusTradeWith(ImprovementTypes candidate, BonusTypes bonus) const
{
    if (candidate == NO_IMPROVEMENT) return isImprovementBonusTrade(bonus);
    if (GC.getImprovementInfo(candidate).isImprovementBonusTrade(bonus)) return true;
    const bool secondary = CvAdditionalImprovements::usesSecondaryLayer(candidate);
    if (secondary && getImprovementType() != NO_IMPROVEMENT
    && GC.getImprovementInfo(getImprovementType()).isImprovementBonusTrade(bonus)) return true;
    for (int i = 0; i < getNumAdditionalImprovements(); ++i)
    {
        const ImprovementTypes type = getAdditionalImprovementType(i);
        if ((!secondary || !CvAdditionalImprovements::sameFamily(type, candidate))
        && GC.getImprovementInfo(type).isImprovementBonusTrade(bonus)) return true;
    }
    return false;
}

int CvPlot::getImprovementVisibilityChange() const
{
	int result = getImprovementType() == NO_IMPROVEMENT ? 0 : GC.getImprovementInfo(getImprovementType()).getVisibilityChange();
	for (int i = 0; i < getNumAdditionalImprovements(); ++i) result += GC.getImprovementInfo(getAdditionalImprovementType(i)).getVisibilityChange();
	return result;
}

int CvPlot::getImprovementSeeFrom() const
{
	int result = getImprovementType() == NO_IMPROVEMENT ? 0 : GC.getImprovementInfo(getImprovementType()).getSeeFrom();
	for (int i = 0; i < getNumAdditionalImprovements(); ++i) result = std::max(result, GC.getImprovementInfo(getAdditionalImprovementType(i)).getSeeFrom());
	return result;
}

void CvPlot::clearAllImprovements()
{
	while (getNumAdditionalImprovements()) setAdditionalImprovementType(NO_IMPROVEMENT, getAdditionalImprovementType(0));
	setImprovementType(NO_IMPROVEMENT);
}

void CvPlot::updateAdditionalImprovementGraphics()
{
	// PlotBuilder accepts one improvement per plot. Separate read-only visual
	// proxies let it draw each extra model using the existing improvement art,
	// including its animations, without adding features or gameplay objects.
	if (!GC.IsGraphicsInitialized() || !isInViewport() || !isGraphicsVisible(ECvPlotGraphics::FEATURE)
	|| !CvAdditionalImprovements::enabled() || GC.getGame().getActiveTeam() == NO_TEAM)
	{
		m_additionalImprovements.clearGraphics();
		return;
	}
	const TeamTypes team = GC.getGame().getActiveTeam();
	for (int slot = 0; slot < CvAdditionalImprovements::CAPACITY; ++slot)
	{
		const ImprovementTypes type = getRevealedAdditionalImprovementType(slot, team);
		if (type == NO_IMPROVEMENT || CvAdditionalImprovements::featureFor(type) != NO_FEATURE)
		{
			m_additionalImprovements.setGraphicPlot(slot, NULL);
			continue;
		}
		CvPlot* visual = m_additionalImprovements.getGraphicPlot(slot);
		if (!visual)
		{
			visual = new CvPlot();
			m_additionalImprovements.setGraphicPlot(slot, visual);
		}
		// Never use gameplay setters on a proxy: it must not alter world totals.
		visual->m_iX = m_iX;
		visual->m_iY = m_iY;
		visual->m_iArea = m_iArea;
		visual->m_ePlotType = m_ePlotType;
		visual->m_eTerrainType = m_eTerrainType;
		visual->m_eFeatureType = m_eFeatureType;
		visual->m_iFeatureVariety = m_iFeatureVariety;
		visual->m_bHills = m_bHills;
		visual->m_bNOfRiver = m_bNOfRiver;
		visual->m_bWOfRiver = m_bWOfRiver;
		visual->m_eOwner = m_eOwner;
		visual->m_workingCity = m_workingCity;
		visual->m_eImprovementType = type;
		visual->m_visibleGraphics = m_visibleGraphics & ECvPlotGraphics::FEATURE;
		if (!visual->m_abRevealed)
		{
			visual->m_abRevealed = new bool[MAX_TEAMS];
			visual->m_aiVisibilityCount = new short[MAX_TEAMS];
			visual->m_aeRevealedImprovementType = new short[MAX_TEAMS];
		}
		for (int i = 0; i < MAX_TEAMS; ++i)
		{
			visual->m_abRevealed[i] = i == team && isRevealed(team, false);
			visual->m_aiVisibilityCount[i] = i == team && isVisible(team, false) ? 1 : 0;
			visual->m_aeRevealedImprovementType[i] = i == team ? type : NO_IMPROVEMENT;
		}
		if (visual->m_pPlotBuilder) gDLL->getPlotBuilderIFace()->destroy(visual->m_pPlotBuilder);
		visual->m_pPlotBuilder = gDLL->getPlotBuilderIFace()->create();
		visual->m_bPlotLayoutDirty = true;
		gDLL->getPlotBuilderIFace()->init(visual->m_pPlotBuilder, visual);
	}
}
