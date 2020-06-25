##  $Header:
##------------------------------------------------------------------------------------------------
##
##  FILE:	ParallelMapsInput.py
##
##  PURPOSE:  User input component for the Parallel Maps mod.
##
##------------------------------------------------------------------------------------------------
from CvPythonExtensions import *
import CvUtil

GC = CyGlobalContext()


class ParallelMapsInput:

	def __init__(self, pEventManager):
		self.pEventManager = pEventManager
		self.pEventManager.addEventHandler("kbdEvent", self.enableMultiMaps)

	def enableMultiMaps(self, argsList):
		self.pEventManager.removeEventHandler("kbdEvent", self.enableMultiMaps)
		GC.enableMultiMaps()
		GC.updateMaps()
		self.pEventManager.addEventHandler("kbdEvent", self.filterInput)
		self.updatePlayerContainers()
		CvUtil.sendImmediateMessage("Multi-Maps enabled.")

	def updatePlayerContainers(self):
		try:
			for i in range(GC.getMAX_PLAYERS()):
				GC.getPlayer(i).updateMembers()
		except:
			CyPythonMgr().errorMsg("Error while adding additional storage containers to the players.")

	def filterInput(self, argsList):
		eventType = argsList[0]
		if self.pEventManager.bAlt and eventType == EventType.EVT_KEYDOWN:
			i = int(argsList[1]) -2
			if i < GC.getNumMapInfos() and i != CyGame().getCurrentMap():
				CvUtil.sendImmediateMessage("Key: %d" %i)
				if not GC.mapInitialized(i):
					self.initMap(i)
					self.initPlayerContainers(i)
				#else:
				GC.switchMap(i)

	def initMap(self, eMap):
		try:
			GC.initializeMap(eMap)
			CvUtil.sendImmediateMessage("Map %d initialized." %eMap)
		except:
			CyPythonMgr().errorMsg("Error while initializing a new map.")

	def initPlayerContainers(self, eMap):
		try:
			for i in range(GC.getMAX_PLAYERS()):
				CyPlayer = GC.getPlayer(i)
				#if CyPlayer.isAlive():
				CyPlayer.initMembers(eMap)
			CvUtil.sendImmediateMessage("Initialized data containers for the players.")
		except:
			CyPythonMgr().errorMsg("Error while initializing data for the players.")
