from CvPythonExtensions import CyGlobalContext, CyInterface, MapTypes
import BugEventManager
import DebugUtils

GC = CyGlobalContext()


class ParallelMaps:

	def __init__(self, pEventManager):
		pEventManager.addEventHandler("kbdEvent", self.handleInput)

	def handleInput(self, argsList):

		def _tryMapSwitch(eMap):
			if eMap > -1 and eMap < MapTypes.NUM_MAPS and eMap != GC.getGame().getCurrentMap() \
			and (GC.isDebugBuild() or DebugUtils.bDebugMode or GC.getMapByIndex(eMap).plotsInitialized()):
				if not GC.getMapByIndex(eMap).plotsInitialized():
					CyInterface().addImmediateMessage("Initialized Map %d: %s" %(eMap, GC.getMapInfo(eMap).getDescription()), "")
				else:
					CyInterface().addImmediateMessage("Map %d: %s" %(eMap, GC.getMapInfo(eMap).getDescription()), "")
				GC.switchMap(eMap)
				return 1

		eKey = argsList[1]

		if BugEventManager.g_eventManager.bAlt and not BugEventManager.g_eventManager.bCtrl:
			return _tryMapSwitch(eKey -2) # enum of key -2 gives the correct value for the number keys (0-9). I know enums should not be used this way.

		elif BugEventManager.g_eventManager.bCtrl and not BugEventManager.g_eventManager.bAlt:
			return _tryMapSwitch(eKey +7) # Maps 10 - 16 (ctrl+1 -> ctrl+6)