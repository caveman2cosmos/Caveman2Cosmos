
from CvPythonExtensions import *
import BugUtil
import CvRandomEventInterface as CvREI

GC = CyGlobalContext()
MAP = GC.getMap()
GAME = GC.getGame()

# Replaces the old EVENTTRIGGER_VOLCANO_* pool entries (now zero-weighted stumps
# in CIV4EventTriggerInfos.xml). Those competed for a share of CvPlayer::doEvents()'s
# single global weighted pick, which made volcano activity front-load hard into the
# early game (huge pool of unclaimed, unflagged plots) and then all but vanish once
# dozens of later-era triggers joined the pool and diluted its tiny static weight.
# Running this as its own per-turn scan keeps the odds flat across the whole game.
#
# Chances are out of ODDS_BASE and scaled by game speed the same way WoodlandCycle
# scales forest fires, so Marathon doesn't see 3x the eruptions Normal does.
ODDS_BASE = 1000
NEW_VOLCANO_CHANCE = 1          # per sampled candidate plot, per turn
ACTIVE_ERUPT_CHANCE = 120       # per active volcano, per turn
ACTIVE_DORMANT_CHANCE = 60      # per active volcano, per turn
DORMANT_ERUPT_CHANCE = 40       # per dormant volcano, per turn
DORMANT_EXTINCT_CHANCE = 15     # per dormant volcano, per turn
SAMPLE_DIVISOR = 400            # ~1/400 of all plots sampled for new-volcano checks per turn


class VolcanoCycle:

	def __init__(self, customEM):
		self.customEM = customEM
		customEM.addEventHandler("GameStart", self.onGameStart)
		customEM.addEventHandler("OnLoad", self.onLoadGame)
		customEM.addEventHandler("BeginGameTurn", self.onBeginGameTurn)

		self.FEATURE_VOLCANO_ACTIVE = GC.getInfoTypeForString('FEATURE_VOLCANO_ACTIVE')
		self.FEATURE_VOLCANO_DORMANT = GC.getInfoTypeForString('FEATURE_VOLCANO_DORMANT')
		self.TERRAIN_ROCKY = GC.getInfoTypeForString('TERRAIN_ROCKY')
		self.BONUS_OBSIDIAN = GC.getInfoTypeForString('BONUS_OBSIDIAN')
		self.BONUS_SULPHUR = GC.getInfoTypeForString('BONUS_SULPHUR')
		self.TECH_TOOL_MAKING = GC.getInfoTypeForString('TECH_TOOL_MAKING')
		self.TECH_ANCIENT_BALLISTICS = GC.getInfoTypeForString('TECH_ANCIENT_BALLISTICS')

		# Natural wonders + existing volcanoes.
		self.blockedFeatures = [f for f in (
			GC.getInfoTypeForString('FEATURE_PLATY_AUYANTEPUI'),
			GC.getInfoTypeForString('FEATURE_PLATY_GREAT_BARRIER'),
			GC.getInfoTypeForString('FEATURE_GREAT_BARRIER_REEF_BEACON'),
			GC.getInfoTypeForString('FEATURE_GREAT_BARRIER_REEF_LIGHTHOUSE'),
			GC.getInfoTypeForString('FEATURE_PLATY_EVEREST'),
			GC.getInfoTypeForString('FEATURE_PLATY_FUJI'),
			GC.getInfoTypeForString('FEATURE_PLATY_AURORA'),
			GC.getInfoTypeForString('FEATURE_PLATY_BAIKAL'),
			GC.getInfoTypeForString('FEATURE_PLATY_BARRINGER_CRATER'),
			GC.getInfoTypeForString('FEATURE_PLATY_BASALT_ORGAN'),
			GC.getInfoTypeForString('FEATURE_PLATY_DEVILS_TABLE'),
			GC.getInfoTypeForString('FEATURE_PLATY_SOPKA'),
			GC.getInfoTypeForString('FEATURE_PLATY_KRAKATOA'),
			GC.getInfoTypeForString('FEATURE_PLATY_NGORONGORO_CRATER'),
			GC.getInfoTypeForString('FEATURE_PLATY_NUKUORO'),
			GC.getInfoTypeForString('FEATURE_PLATY_PAMUKKALE'),
			GC.getInfoTypeForString('FEATURE_PLATY_PRAVCICKA_BRANA'),
			GC.getInfoTypeForString('FEATURE_PLATY_SHARK_BAY'),
			GC.getInfoTypeForString('FEATURE_PLATY_SINAI'),
			GC.getInfoTypeForString('FEATURE_PLATY_SUGARLOAF'),
			GC.getInfoTypeForString('FEATURE_PLATY_ULURU'),
			GC.getInfoTypeForString('FEATURE_PLATY_VICTORIA_FALLS'),
			GC.getInfoTypeForString('FEATURE_PLATY_KILIMANJARO'),
			GC.getInfoTypeForString('FEATURE_PLATY_DEAD_SEA'),
			self.FEATURE_VOLCANO_ACTIVE,
			self.FEATURE_VOLCANO_DORMANT,
		) if f != -1]
		self.volcanicBonuses = [-1, self.BONUS_OBSIDIAN, self.BONUS_SULPHUR]

	def onGameStart(self, argsList): self.cache()
	def onLoadGame(self, argsList): self.cache()

	def cache(self):
		# Full map, water and peaks included: eruptions can turn ocean into a new
		# island or level a peak to rocky flatland, same as the old trigger allowed.
		self.candidatePlots = candidates = []
		self.volcanoes = []
		for plot in MAP.plots():
			candidates.append(plot)
			iFeature = plot.getFeatureType()
			if iFeature == self.FEATURE_VOLCANO_ACTIVE or iFeature == self.FEATURE_VOLCANO_DORMANT:
				self.volcanoes.append(plot)
		self.iMaxIndex = len(candidates)
		self.iFactorGS = GC.getGameSpeedInfo(GAME.getGameSpeedType()).getSpeedPercent()

	def canFormNewVolcano(self, plot):
		if plot.isCity():
			return False
		if plot.getFeatureType() in self.blockedFeatures:
			return False
		return plot.getBonusType(-1) in self.volcanicBonuses

	def roll(self, chance):
		# Scale the effective pool so slower speeds don't see proportionally more
		# eruptions just for having more turns (same idea as WoodlandCycle's gate).
		return GAME.getSorenRandNum(ODDS_BASE * self.iFactorGS / 100, "Volcano") < chance

	def onBeginGameTurn(self, argsList):
		for plot in list(self.volcanoes):
			iFeature = plot.getFeatureType()

			if iFeature == self.FEATURE_VOLCANO_ACTIVE:
				if self.roll(ACTIVE_ERUPT_CHANCE):
					CvREI.doVolcanoNeighbouringPlots(plot)
					CvREI.doVolcanoReport((plot, BugUtil.getPlainText("TXT_KEY_EVENTTRIGGER_VOLCANO_ACTIVE")))
				elif self.roll(ACTIVE_DORMANT_CHANCE):
					plot.setFeatureType(self.FEATURE_VOLCANO_DORMANT, 0)
					CvREI.doVolcanoAdjustFertility((plot, -1, -1))
					CvREI.doVolcanoReport((plot, BugUtil.getPlainText("TXT_KEY_EVENT_TRIGGER_VOLCANO_DORMANT")))

			elif iFeature == self.FEATURE_VOLCANO_DORMANT:
				if self.roll(DORMANT_ERUPT_CHANCE):
					CvREI.doVolcanoPlot(plot)
					CvREI.doVolcanoNeighbouringPlots(plot)
					CvREI.doVolcanoAdjustFertility((plot, 1, -1))
					CvREI.doVolcanoReport((plot, BugUtil.getPlainText("TXT_KEY_EVENT_TRIGGER_VOLCANO_DORMANT_ERUPTION")))
				elif self.roll(DORMANT_EXTINCT_CHANCE):
					self.extinguish(plot)
					self.volcanoes.remove(plot)
			else:
				# Shouldn't normally happen, but don't keep tracking a dead entry.
				self.volcanoes.remove(plot)

		plotIndex = [0] * self.iMaxIndex
		shuffleList(self.iMaxIndex, GAME.getMapRand(), plotIndex)
		iMaxCount = max(1, self.iMaxIndex / SAMPLE_DIVISOR)
		for i in xrange(iMaxCount):
			plot = self.candidatePlots[plotIndex[i]]
			if not self.canFormNewVolcano(plot):
				continue
			if not self.roll(NEW_VOLCANO_CHANCE):
				continue
			CvREI.doVolcanoPlot(plot)
			CvREI.doVolcanoNeighbouringPlots(plot)
			CvREI.doVolcanoAdjustFertility((plot, 1, -1))
			CvREI.doVolcanoReport((plot, BugUtil.getPlainText("TXT_KEY_EVENT_TRIGGER_VOLCANO_NEW")))
			self.volcanoes.append(plot)

	def extinguish(self, plot):
		# Explicitly clear the feature rather than relying on setPlotType to drop it:
		# canFormNewVolcano treats a lingering DORMANT feature as blocked, which would
		# permanently exclude this plot from ever forming a new volcano again.
		plot.setFeatureType(-1, 0)
		plot.setPlotType(PlotTypes.PLOT_HILLS, True, True)
		plot.setTerrainType(self.TERRAIN_ROCKY, True, True)
		if GAME.getSorenRandNum(100, 'Volcanic minerals chance') < 50:
			iBonus, iTech = self.BONUS_OBSIDIAN, self.TECH_TOOL_MAKING
		else:
			iBonus, iTech = self.BONUS_SULPHUR, self.TECH_ANCIENT_BALLISTICS
		plot.setBonusType(iBonus)

		szBuffer = BugUtil.getPlainText("TXT_KEY_MISC_DISCOVERED_NEW_RESOURCE_VOLCANO")
		for i in xrange(GC.getMAX_PC_PLAYERS()):
			pPlayer = GC.getPlayer(i)
			if (
				pPlayer.isHuman() and pPlayer.isAlive()
				and plot.isRevealed(pPlayer.getTeam(), False)
				and GC.getTeam(pPlayer.getTeam()).isHasTech(iTech)
			):
				CyInterface().addMessage(
					pPlayer.getID(), False, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_BOMBARDED",
					InterfaceMessageTypes.MESSAGE_TYPE_INFO, GC.getFeatureInfo(self.FEATURE_VOLCANO_ACTIVE).getButton(),
					GC.getCOLOR_RED(), plot.getX(), plot.getY(), True, True
				)
