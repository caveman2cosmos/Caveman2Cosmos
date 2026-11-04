## Multiple Improvements UI. Authoritative placement and save data live in DLL.

from CvPythonExtensions import CyGlobalContext, CyInterface, CyGameTextMgr, CyTranslator, GameOptionTypes, InterfaceDirtyBits

GC = CyGlobalContext()
_lastShift = None


def init():
	"""Loaded by BUG; map migration runs in DLL during modifier recalculation."""
	pass


def isEnabled():
	"""Return whether the current game enables Multiple Improvements."""
	return GC.getGame().isOption(GameOptionTypes.GAMEOPTION_MAP_MULTIPLE_IMPROVEMENTS)


def hidePlotHelp(screen):
	# Also clear the retired total panel if this module is reloaded in-game.
	for name in ("AdditionalImprovementHelp0", "AdditionalImprovementHelp1", "ImprovementTotalOutput"):
		screen.hide(name)


def updateHelpState(interface):
	"""Refresh Shift details even when the mouse remains on the same plot."""
	global _lastShift
	if not isEnabled():
		_lastShift = None
		return
	shift = interface.InputData.getModifierKeys()[2]
	if shift != _lastShift:
		_lastShift = shift
		CyInterface().setDirty(InterfaceDirtyBits.Help_DIRTY_BIT, True)


def getKnownImprovements(plot):
	if not isEnabled() or not plot:
		return []
	team = GC.getGame().getActiveTeam()
	if team < 0 or not plot.isRevealed(team, True):
		return []
	types = []
	primary = plot.getRevealedPrimaryImprovementType(team)
	if primary >= 0:
		types.append(primary)
	for slot in range(1):
		typeID = plot.getRevealedAdditionalImprovementType(slot, team)
		if typeID >= 0:
			types.append(typeID)
	return types


def hasMultipleKnownImprovements(plot):
	return len(getKnownImprovements(plot)) > 1


def clearImprovements(plot):
	if isEnabled():
		types = [plot.getAdditionalImprovementType(slot) for slot in range(1)]
		for typeID in types:
			if typeID >= 0:
				plot.removeAdditionalImprovementType(typeID)
	plot.setImprovementType(-1)


def placeImprovement(plot, typeID):
	if typeID < 0:
		clearImprovements(plot)
	else:
		plot.setImprovementType(typeID)


def removeImprovement(plot, typeID):
	if isEnabled() and typeID >= 0 and GC.getImprovementInfo(typeID).isSecondaryLayer():
		plot.removeAdditionalImprovementType(typeID)
	elif typeID < 0:
		clearImprovements(plot)
	else:
		plot.setImprovementType(-1)


def showPlotHelp(screen, interface, baseText, font):
	"""Keep the normal plot hint; holding Shift adds each extra's details."""
	hidePlotHelp(screen)
	if not isEnabled():
		return False
	plot = CyInterface().getMouseOverPlot()
	team = GC.getGame().getActiveTeam()
	if not plot or team < 0 or not plot.isRevealed(team, True):
		return False
	types = []
	for slot in range(1):
		typeID = plot.getRevealedAdditionalImprovementType(slot, team)
		if typeID >= 0:
			types.append(typeID)
	if not types:
		return False

	import PythonToolTip as tooltips
	anchor = interface.yBotBar + 8
	translator = CyTranslator()
	known = getKnownImprovements(plot)
	names = [GC.getImprovementInfo(typeID).getDescription() for typeID in known]
	plotText = u", ".join(names) + u"\n" + baseText
	if not interface.InputData.getModifierKeys()[2]:
		plotText += u"\n" + translator.getText("TXT_KEY_MULTIPLE_IMPROVEMENTS_DETAILS", ())
		tooltips.makeTooltip(screen, -12, anchor, plotText, font, "PlotHelp")
		return True
	x, top, width, height = tooltips.makeTooltip(screen, -12, anchor, plotText, font, "PlotHelp", returnGeometry=True)
	left = x + width + 8
	# The first panel retains its existing size. The extra panels share the rest.
	maxWidth = max(80, (interface.xRes - left - 12) // len(types) - 32)
	for index, typeID in enumerate(types):
		name = "AdditionalImprovementHelp" + str(index)
		helpText = CyGameTextMgr().getImprovementHelp(typeID, False)
		px, py, panelWidth, panelHeight = tooltips.makeTooltip(screen, -12, anchor, helpText, font, name, maxWidth=maxWidth, maxHeight=max(88, interface.yBotBar - 100), returnGeometry=True)
		screen.moveItem(name, left, py, 0)
		screen.moveToFront(name)
		left += panelWidth + 8
	return True
