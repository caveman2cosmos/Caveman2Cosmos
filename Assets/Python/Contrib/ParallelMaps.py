from CvPythonExtensions import CyGlobalContext #, MapTypes
import BugEventManager
import CvUtil
import DebugUtils

GC = CyGlobalContext()

class ParallelMaps:

	def __init__(self, pEventManager):
		pEventManager.addEventHandler("kbdEvent", self.filterInput)

	def filterInput(self, argsList):
		if DebugUtils.bDebugMode and BugEventManager.g_eventManager.bAlt:
			i = argsList[1] -2
			#if i in range(MapTypes.NUM_MAPS) \
			if i > -1 and i < 10 and i != GC.getGame().getCurrentMap():				
				if not GC.getMapByIndex(i).plotsInitialized():
					GC.switchMap(i)
					CvUtil.sendImmediateMessage("Initialized Map %d: %s" %(i, GC.getMapInfo(i).getDescription()))
				else:
					GC.switchMap(i)
					CvUtil.sendImmediateMessage("Map %d: %s" %(i, GC.getMapInfo(i).getDescription()))