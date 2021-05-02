from CvPythonExtensions import CyGlobalContext, MapTypes
import BugEventManager
import CvUtil

GC = CyGlobalContext()
bIsSwitchingMap = False


class ParallelMaps:

	def __init__(self, pEventManager):
		if GC.getDefineINT("ENABLE_MULTI_MAPS"):
			pEventManager.addEventHandler("kbdEvent", self.filterInput)

	def filterInput(self, argsList):
		i = argsList[1] -2
		if BugEventManager.g_eventManager.bAlt \
		and i < MapTypes.NUM_MAPS \
		and i != GC.getGame().getCurrentMap():
			global bIsSwitchingMap
			bIsSwitchingMap = True
			GC.switchMap(i)
			bIsSwitchingMap = False
			if i == 0:
				CvUtil.sendImmediateMessage("Initial map")
			else:
				CvUtil.sendImmediateMessage("Map %d" %i)