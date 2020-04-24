##  $Header:
##------------------------------------------------------------------------------------------------
##
##  FILE:	ParallelMaps.py
##
##  PURPOSE:  User input component for the Parallel Maps mod.
##
##------------------------------------------------------------------------------------------------
from CvPythonExtensions import *
import CvUtil

GC = CyGlobalContext()


class ParallelMaps:

	def __init__(self, pEventManager):
		self.pEventManager = pEventManager
		self.pEventManager.addEventHandler("kbdEvent", self.enableMultiMaps)

	def enableMultiMaps(self, argsList):
		self.pEventManager.removeEventHandler("kbdEvent", self.enableMultiMaps)
		GC.enableMultiMaps()
		GC.updateMaps()
		self.pEventManager.addEventHandler("kbdEvent", self.filterInput)
		CvUtil.sendImmediateMessage("Multi-Maps enabled.")

	def filterInput(self, argsList):
		eventType = argsList[0]
		if self.pEventManager.bAlt and eventType == EventType.EVT_KEYDOWN:
			i = int(argsList[1]) -2
			if i < GC.getNumMapInfos() and i != CyGame().getCurrentMap():
				if not GC.mapInitialized(i):
					GC.initializeMap(i)
				GC.switchMap(i)
				CvUtil.sendImmediateMessage("Map %d" %i)
