# City origins are assigned by the DLL's normal building-production path.
# Keep in-game Python compatible with 2.4.
from CvPythonExtensions import CyGame, GameOptionTypes


def isEnabled():
	return CyGame().isOption(GameOptionTypes.GAMEOPTION_PROMOTIONS_NATIVE)


def onUnitBuilt(argsList):
	# Retained as an import-compatible entry point; XML grants the origin.
	pass


def onLoad(argsList):
	if CyGame().isFinalInitialized():
		recalculateModifiers()


def recalculateModifiers():
	# Also removes origin buildings when the option is disabled. This never
	# assigns an origin to an existing unit from its current position.
	CyGame().refreshNativePromotionBuildings()


def enableFromLegacySave():
	game = CyGame()
	if game.isNetworkMultiPlayer() or isEnabled():
		return
	game.setOption(GameOptionTypes.GAMEOPTION_PROMOTIONS_NATIVE, True)
	recalculateModifiers()
