from CvPythonExtensions import CyGlobalContext, CyGame
from CvUtil import sendImmediateMessage

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
			sendImmediateMessage("Multi-Maps enabled.")

	def filterInput(self, argsList):
		if self.pEventManager.bAlt:
			i = argsList[1] -2
			if i < GC.getNumMapCategoryInfos() and i != GC.getGame().getCurrentMap():
				global bIsSwitchingMap
				bIsSwitchingMap = True
				if not GC.mapInitialized(i):
					GC.initializeMap(i)
				GC.switchMap(i)
				bIsSwitchingMap = False
				if i == 0:
					sendImmediateMessage("Initial map")
				else:
					sendImmediateMessage("Map %d" %i)