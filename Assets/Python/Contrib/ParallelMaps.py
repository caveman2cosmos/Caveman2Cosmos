from CvPythonExtensions import CyGlobalContext, MapTypes
import BugEventManager

GC = CyGlobalContext()
bIsSwitchingMap = False


class ParallelMaps:

	def __init__(self, pEventManager):
		pEventManager.addEventHandler("kbdEvent", self.filterInput)

	def filterInput(self, argsList):
		if BugEventManager.g_eventManager.bAlt:
			i = argsList[1] -2
			if i < MapTypes.NUM_MAPS \
			and i != GC.getGame().getCurrentMap() \
			and GC.getMapByIndex(i).plotsInitialized():
				global bIsSwitchingMap
				bIsSwitchingMap = True
				GC.switchMap(i)
				bIsSwitchingMap = False
