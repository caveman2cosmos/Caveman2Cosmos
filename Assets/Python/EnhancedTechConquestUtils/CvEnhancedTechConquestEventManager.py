
from CvPythonExtensions import *
import EnhancedTechConquest

enhancedTechConquest = EnhancedTechConquest.EnhancedTechConquest()

# globals
###################################################
class CvEnhancedTechConquestEventManager:
	def __init__(self, eventManager):
		eventManager.addEventHandler("cityAcquired", self.onCityAcquired)
		eventManager.addEventHandler("windowActivation", self.onWindowActivation)

		EnhancedTechConquest.loadConfigurationData()

	def onCityAcquired(self, argsList):
		enhancedTechConquest.onCityAcquired(argsList)

	def onWindowActivation(self, argsList):
		bActive = argsList[0]
		if bActive:
			EnhancedTechConquest.loadConfigurationData()
