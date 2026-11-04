## Primitive Maps fog-of-war memory rules.

from CvPythonExtensions import *
import BugData

GC = CyGlobalContext()
g_TechIDCache = {}

def isHumanTeam(iTeam):
	for iPlayer in xrange(GC.getMAX_PLAYERS()):
		pPlayer = GC.getPlayer(iPlayer)
		if pPlayer.isAlive() and pPlayer.isHuman() and pPlayer.getTeam() == iTeam:
			return True
	return False

def hasTeamTech(iTeam, sTechType):
	if sTechType not in g_TechIDCache:
		g_TechIDCache[sTechType] = GC.getInfoTypeForString(sTechType)
	iTech = g_TechIDCache[sTechType]
	return iTech > -1 and GC.getTeam(iTeam).isHasTech(iTech)

def isNaturalWonderPlot(pPlot):
	iFeature = pPlot.getFeatureType()
	if iFeature < 0:
		return False
	## FEATURE_PLATY_ is the prefix used for natural wonders.
	return GC.getFeatureInfo(iFeature).getType().find("FEATURE_PLATY_") == 0

def isForeignBorderPlot(pPlot, iTeam):
	if not pPlot.isOwned():
		return False

	iOwner = pPlot.getOwner()
	iOwnerTeam = GC.getPlayer(iOwner).getTeam()
	if iOwnerTeam == iTeam:
		return False

	for pAdjacentPlot in pPlot.adjacent():
		if not pAdjacentPlot.isOwned():
			return True
		iAdjacentOwnerTeam = GC.getPlayer(pAdjacentPlot.getOwner()).getTeam()
		if iAdjacentOwnerTeam != iOwnerTeam:
			return True
	return False

def shouldForgetPlot(pPlot, iTeam):
	## Cartography stops forgetting newly discovered plots. Previously forgotten
	## plots are intentionally not restored.
	if hasTeamTech(iTeam, "TECH_CARTOGRAPHY"):
		return False

	aUnlockTechs = []
	if pPlot.isCity() or isNaturalWonderPlot(pPlot):
		aUnlockTechs.append("TECH_PETROGLYPHS")

	iFeature = pPlot.getFeatureType()
	sFeatureType = ""
	if iFeature > -1:
		sFeatureType = GC.getFeatureInfo(iFeature).getType()
	if pPlot.getPlotType() == PlotTypes.PLOT_PEAK or sFeatureType.find("FEATURE_VOLCANO_") == 0:
		aUnlockTechs.append("TECH_PICTOGRAPHS")
	if pPlot.isRiver():
		aUnlockTechs.append("TECH_IDEOGRAMS")
	if isForeignBorderPlot(pPlot, iTeam):
		aUnlockTechs.append("TECH_WRITING")
	if pPlot.isWater():
		aUnlockTechs.append("TECH_COMPASS")

	iBonus = pPlot.getBonusType(-1)
	if iBonus > -1:
		iStrategicClass = GC.getInfoTypeForString("BONUSCLASS_STRATEGIC")
		if GC.getBonusInfo(iBonus).getBonusClassType() == iStrategicClass:
			aUnlockTechs.append("TECH_SURVEYING")

	for sTechType in aUnlockTechs:
		if hasTeamTech(iTeam, sTechType):
			return False
	return True

def getPlotsToForget():
	kData = BugData.getTable("PrimitiveMaps")
	if "plotsToForget" not in kData:
		kData["plotsToForget"] = []
	return kData

def onPlotRevealed(argsList):
	if not GC.getGame().isOption(GameOptionTypes.GAMEOPTION_MAP_PRIMITIVE_MAPS):
		return
	pPlot = argsList[0]
	iTeam = argsList[1]
	try:
		## Only plots revealed by the normal visibility system are considered.
		if pPlot and iTeam >= 0 and iTeam < GC.getMAX_TEAMS() and isHumanTeam(iTeam):
			if shouldForgetPlot(pPlot, iTeam):
				kPlot = (pPlot.getX(), pPlot.getY(), iTeam)
				kData = getPlotsToForget()
				aPlotsToForget = kData["plotsToForget"]
				if kPlot not in aPlotsToForget:
					aPlotsToForget.append(kPlot)
					kData["plotsToForget"] = aPlotsToForget
	except Exception, e:
		import traceback
		print "PrimitiveMaps.onPlotRevealed failed:", str(e)
		traceback.print_exc()

def enableFromLegacySave():
	## Build the queue before enabling, so a failed scan cannot partly migrate a game.
	pMap = GC.getMap()
	aPlotsToForget = []
	for iTeam in xrange(GC.getMAX_TEAMS()):
		if not GC.getTeam(iTeam).isAlive() or not isHumanTeam(iTeam):
			continue
		for iPlot in xrange(pMap.numPlots()):
			pPlot = pMap.plotByIndex(iPlot)
			if pPlot and pPlot.isRevealed(iTeam, False) and shouldForgetPlot(pPlot, iTeam):
				## Keep visible plots queued too, for when they subsequently leave sight.
				aPlotsToForget.append((pPlot.getX(), pPlot.getY(), iTeam))
	getPlotsToForget()["plotsToForget"] = aPlotsToForget
	GC.getGame().setOption(GameOptionTypes.GAMEOPTION_MAP_PRIMITIVE_MAPS, True)

def onBeginPlayerTurn(argsList):
	if not GC.getGame().isOption(GameOptionTypes.GAMEOPTION_MAP_PRIMITIVE_MAPS):
		return
	iGameTurn, iPlayer = argsList
	iTeam = GC.getPlayer(iPlayer).getTeam()
	aRemaining = []
	kData = getPlotsToForget()
	aHumanTeams = [iHumanTeam for iHumanTeam in xrange(GC.getMAX_TEAMS()) if isHumanTeam(iHumanTeam)]
	for iX, iY, iPlotTeam in kData["plotsToForget"]:
		if iPlotTeam not in aHumanTeams:
			continue
		if iPlotTeam != iTeam:
			aRemaining.append((iX, iY, iPlotTeam))
			continue
		pPlot = GC.getMap().plot(iX, iY)
		if not pPlot or not shouldForgetPlot(pPlot, iTeam):
			continue

		if pPlot.isVisible(iTeam, False):
			aRemaining.append((iX, iY, iPlotTeam))
		elif pPlot.isRevealed(iTeam, False):
			pPlot.setRevealed(iTeam, False, False, -1)

	kData["plotsToForget"] = aRemaining
