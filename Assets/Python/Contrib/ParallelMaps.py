from CvPythonExtensions import CyGlobalContext, MapTypes
import BugEventManager
import CvUtil
import DebugUtils

GC = CyGlobalContext()
bIsSwitchingMap = False


class ParallelMaps:

	def __init__(self, pEventManager):
		pEventManager.addEventHandler("kbdEvent", self.filterInput)

	def filterInput(self, argsList):
		if DebugUtils.bDebugMode \
		and BugEventManager.g_eventManager.bAlt:
			i = argsList[1] -2
			if i in range(int(MapTypes.NUM_MAPS)) \
			and i != GC.getGame().getCurrentMap():
				global bIsSwitchingMap
				bIsSwitchingMap = True
				GC.switchMap(i)
				bIsSwitchingMap = False
				if i == 0:
					CvUtil.sendImmediateMessage("Initial map")
				else:
					CvUtil.sendImmediateMessage("Map %d" %i)