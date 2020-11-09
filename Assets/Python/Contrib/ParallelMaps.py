from CvPythonExtensions import *
import CvUtil

GC = CyGlobalContext()
bIsSwitchingMap = False


class ParallelMaps:

	def __init__(self, pEventManager):
		self.pEventManager = pEventManager
		self.pEventManager.addEventHandler("kbdEvent", self.enableMultiMaps)

	def enableMultiMaps(self, argsList):
		self.pEventManager.removeEventHandler("kbdEvent", self.enableMultiMaps)
		if GC.enableMultiMaps():
			GC.updateMaps()
			self.pEventManager.addEventHandler("kbdEvent", self.filterInput)
			CvUtil.sendImmediateMessage("Multi-Maps enabled.")

	def filterInput(self, argsList):
		eventType = argsList[0]
		if self.pEventManager.bAlt and eventType == EventType.EVT_KEYDOWN:
			i = argsList[1] -2
			if i < GC.getNumMapInfos() and i != CyGame().getCurrentMap():
				global bIsSwitchingMap
				bIsSwitchingMap = True
				if not GC.mapInitialized(i):
					GC.initializeMap(i)
				GC.switchMap(i)
				bIsSwitchingMap = False
				if i == 0:
					CvUtil.sendImmediateMessage("Initial map")
				else:
					CvUtil.sendImmediateMessage("Map %d" %i)