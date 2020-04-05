from CvPythonExtensions import *
import CvUtil

GC = CyGlobalContext()


class ParallelMapsInput:

	def __init__(self, pEventManager):
		self.pEventManager = pEventManager
		self.pEventManager.addEventHandler("kbdEvent", self.enableMultiMaps)

	def enableMultiMaps(self, argsList):
		self.pEventManager.removeEventHandler("kbdEvent", self.enableMultiMaps)
		GC.enableMultiMaps(True)
		GC.updateMaps()
		self.pEventManager.addEventHandler("kbdEvent", self.filterInput)
		CvUtil.sendImmediateMessage("Multi-maps enabled.")

	def filterInput(self, argsList):
		eventType = argsList[0]
		if self.pEventManager.bAlt and eventType == EventType.EVT_KEYDOWN:
			eMap = int(argsList[1]) -2
			if eMap < GC.getNumMapInfos() and eMap != CyGame().getCurrentMap():
				CvUtil.sendImmediateMessage("Key: %d" %eMap)
				if not GC.mapInitialized(eMap):
					GC.initializeMap(eMap)
				GC.switchMap(eMap)
