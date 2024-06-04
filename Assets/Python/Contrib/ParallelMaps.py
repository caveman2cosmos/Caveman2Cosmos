from CvPythonExtensions import CyGlobalContext, CyInterface, MapTypes
from BugEventManager import g_eventManager
import DebugUtils

GC = CyGlobalContext()


def handleInput(argsList):
	GAME = GC.getGame()
	if not GAME.GetWorldBuilderMode():
		iKey = argsList[1]
		bDebug = GC.isDebugBuild() or DebugUtils.bDebugMode
		for i in range(MapTypes.NUM_MAPS):
			mapInfo = GC.getMapInfo(i)
			if iKey == mapInfo.getHotKeyVal() \
			and (not mapInfo.isAltDown() or g_eventManager.bAlt) \
			and (not mapInfo.isShiftDown() or g_eventManager.bShift) \
			and (not mapInfo.isCtrlDown() or g_eventManager.bCtrl) \
			and i != GAME.getCurrentMap() \
			and (bDebug or GC.getMapByIndex(i).plotsInitialized()):
				if not GC.getMapByIndex(i).plotsInitialized():
					CyInterface().addImmediateMessage("Initialized Map %d: %s" %(i, mapInfo.getDescription()), "")
				else:
					CyInterface().addImmediateMessage("Map %d: %s" %(i, mapInfo.getDescription()), "")
				GC.switchMap(i)
				return 1