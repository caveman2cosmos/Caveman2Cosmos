## A New Dawn Mod Code
##

from CvPythonExtensions import *
gc = CyGlobalContext()
import CvMADNukesManager
import CvScreenEnums

class MADNukesEventManager:
	def __init__(self, eventManager):
		self.EventKeyDown = 6
		eventManager.addEventHandler("kbdEvent",self.onKbdEvent)
		self.eventManager = eventManager

	def onKbdEvent(self, argsList):
		'keypress handler - return 1 if the event was consumed'
		eventType,key,mx,my,px,py = argsList

		theKey = int(key)

		if ( eventType == self.EventKeyDown and theKey == int(InputTypes.KB_N) and self.eventManager.bAlt and gc.getPlayer(gc.getGame().getActivePlayer()).isMADNukesEnabled()):
			CvMADNukesManager.CvMADNukesManager(CvScreenEnums.MAD_NUKES_MANAGER).interfaceScreen()