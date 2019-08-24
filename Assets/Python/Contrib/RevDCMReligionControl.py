## Copyright (c) 2009 The BUG Mod.
##
## Bug System: EmperorFool (thanks!)
## Author Glider1

from CvPythonExtensions import *
import BugUtil
import CvUtil
import BugCore
import Popup as PyPopup
import CvEventInterface

gc = CyGlobalContext()
RevDCMOpt = BugCore.game.RevDCM
PICK_RELIGION_EVENT = CvUtil.getNewEventID("PickReligion")
localText = CyTranslator()


#RevolutionDCM
#Class ReligionControl responds to "calls back" from the DLL via the BUG system.
#This has been done by declaring the attachment in RevDCM.xml. BUG will then instanticiate
#the class and attach it's DLL call backs.

class ReligionControl:
	def doHolyCity(self):
		if RevDCMOpt.isOC_LIMITED_RELIGIONS():
			#This algorithm has to deal with a PLETHORA of possibilities like multiple religions
			#being founded by multiple players on the same turn.
			MAX_PC_PLAYERS = gc.getMAX_PC_PLAYERS()
			iPlayerQueue = []
			iSlotQueue = []
			# Sort through all religions and players and distinguish unique religions that want founding
			for iReligion in range(gc.getNumReligionInfos()):
				if CyGame().isReligionSlotTaken(iReligion) or iReligion in iSlotQueue:
					continue
				for iPlayer in range(MAX_PC_PLAYERS):
					CyPlayer = gc.getPlayer(iPlayer)
					if CyPlayer.isAlive() and not CyPlayer.hasHolyCity(iReligion):
						if gc.getTeam(CyPlayer.getTeam()).isHasTech(gc.getReligionInfo(iReligion).getTechPrereq()):
							iPlayerQueue.append(iPlayer)
							iSlotQueue.append(iReligion)
			if CyGame().isOption(GameOptionTypes.GAMEOPTION_PICK_RELIGION) or RevDCMOpt.isCHOOSE_RELIGION():
				#Throw the list to the popup manager that has to delegate multiple popups for each new religion
				pickReligion = PickReligionPopup(CvEventInterface.getEventManager())
				pickReligion.foundPickableReligions(iPlayerQueue, iSlotQueue)
			else:
				#Let the sdk handle the list as it normally would anyway
				for i in range(len(iPlayerQueue)):
					gc.getPlayer(iPlayerQueue[i]).foundReligion(iSlotQueue[i], iSlotQueue[i], True)
			return True
		if RevDCMOpt.isCHOOSE_RELIGION():
			#Stop the sdk founding a religion because all possible cases in vanilla BTS
			#have already been handled in doHolyCityTech.
			return True
		#False means allow the SDK to found a religion if needed
		return False

	def doHolyCityTech(self,argsList):
		eTeam = argsList[0]
		ePlayer = argsList[1]
		eTech = argsList[2]
		bFirst = argsList[3]
		if RevDCMOpt.isOC_LIMITED_RELIGIONS():
			#In limited religion, the assignment of religion cannot be done here
			#because there still could be more unknowable calls to this function
			#that affect the allocation of religions.
			return True
		if RevDCMOpt.isCHOOSE_RELIGION():
			pickReligion = PickReligionPopup(CvEventInterface.getEventManager())
			for iSlot in range(gc.getNumReligionInfos()):
				prereq = gc.getReligionInfo(iSlot).getTechPrereq()
				if eTech == prereq and bFirst:
					pickReligion.initiatePopup(ePlayer, iSlot)
					break
			return True
		#False means allow the SDK to found a religion if needed
		return False

#####################################################
# ReligionControl Events
# Note: defined and attached from RevDCM.xml
#####################################################

def onBeginPlayerTurn(argsList):
	'Called at the beginning of a players turn'
	iGameTurn, iPlayer = argsList
	CyPlayer = gc.getPlayer(iPlayer)

	# Code to determine religious victory
	VICTORY_RELIGIOUS = gc.getInfoTypeForString("VICTORY_RELIGIOUS")
	# 1. A player must have, and only have, the Holy City for the official State Religion
	if CyGame().isVictoryValid(VICTORY_RELIGIOUS) and CyPlayer.countHolyCities() == 1:
		# 2. Religious influence must be at least 80%
		iStateReligion = CyPlayer.getStateReligion()
		if iStateReligion != -1 and CyPlayer.hasHolyCity(iStateReligion) and CyGame().calculateReligionPercent(iStateReligion) < 80:
			# Prerequisite 1 & 2 fulfilled
			bUnity = True
			# 3. 80% a player's cities must have the official state religion established.
			iMaxNonStatePercent = 20
			iCityCount = CyPlayer.getNumCities()
			iReligionCount = gc.getNumReligionInfos()
			iCount = 0
			CyCity, i = CyPlayer.firstCity(False)
			while CyCity:
				for iReligionX in range(iReligionCount):
					if iReligionX == iStateReligion :
						if not CyCity.isHasReligion(iReligionX):
							iCount += 1
						if 100.0 * iCount / iCityCount > iMaxNonStatePercent:
							bUnity = False # Prerequisite 3 failed
							break
				if not bUnity:
					break
				CyCity, i = CyPlayer.nextCity(i, False)

			if bUnity:
				# The player has met all prerequsites to achieve a valid religious victory
				CyGame().setWinner(CyPlayer.getTeam(), VICTORY_RELIGIOUS)


class PickReligionPopup:
	def __init__(self, eventManager):
		moreEvents = {
			PICK_RELIGION_EVENT : ('', self.__eventPickReligionApply,  self.__eventPickReligionBegin),
		}
		eventManager.Events.update(moreEvents)
		self.eventMgr = eventManager
		self.iSlotReligion = 0
		self.iPlayer = 0
		self.iReligionList = []
		self.foundCustomReligion = True
		self.iPlayerQueue = []
		self.iSlotQueue = []
		self.lock = False

	def foundPickableReligions(self, iPlayerQueue, iUniqueSlotQueue):
		self.iPlayerQueue = iPlayerQueue
		self.iSlotQueue = iUniqueSlotQueue
		if len(self.iPlayerQueue):
			self.initiatePopup(self.iPlayerQueue.pop(), self.iSlotQueue.pop())

	def initiatePopup(self, iPlayer, iSlot):
		self.iPlayer = iPlayer
		self.iSlotReligion = iSlot
		self.foundCustomReligion = False
		if CyGame().isOption(GameOptionTypes.GAMEOPTION_PICK_RELIGION):
			if gc.getPlayer(self.iPlayer).isHuman():
				self.foundCustomReligion = True
		if RevDCMOpt.isCHOOSE_RELIGION():
			self.foundCustomReligion = True
		if self.foundCustomReligion:
			self.eventMgr.beginEvent(PICK_RELIGION_EVENT)

	def __eventPickReligionBegin(self, argsList):
		if not self.lock:
			self.lock = True
			self.launchPopup()

	def launchPopup(self):
		activePlayer = gc.getActivePlayer().getID()
		CyGame().setAIAutoPlay(activePlayer, 0)
		activePlayerTeam = gc.getActivePlayer().getTeam()
		PY_activePlayerTeam = gc.getTeam(activePlayerTeam)
		thisPlayer = self.iPlayer
		PY_thisPlayer = gc.getPlayer(thisPlayer)
		thisPlayerTeam = PY_thisPlayer.getTeam()
		playerText = localText.getText("TXT_KEY_UNIT_REVDCM_RELIGION_DISTANT_CIV",())
		#~ playerText = "A distant civ"
		if thisPlayer == activePlayer:
			playerText = "You"
			#~ playerText = "You"
		elif PY_activePlayerTeam.isHasMet(thisPlayerTeam):
			playerText = PY_thisPlayer.getName()

		popup = PyPopup.PyPopup(PICK_RELIGION_EVENT, EventContextTypes.EVENTCONTEXT_SELF)
		size = 300
		rightCornerOffset = 110
		#popup.setSize(size, size)
		screenRes = CyUserProfile().getResolutionString(CyUserProfile().getResolution())
		textRes = screenRes.split('x')
		resX = int(textRes[0])
		popup.setPosition(resX-(size+rightCornerOffset), rightCornerOffset)
		popup.setHeaderString(localText.getText("TXT_KEY_UNIT_REVDCM_RELIGION_HEADER",()))
		#~ popup.setHeaderString("Choose religion option:")
		popup.setBodyString(playerText + localText.getText("TXT_KEY_UNIT_REVDCM_RELIGION_PART_2",()))
		#~ popup.setBodyString(playerText + " can found a religion! Choose it:")
		religionList = self.getRemainingReligions()
		for religion in religionList:
			religionText = gc.getReligionInfo(religion).getDescription()
			popup.addButton(religionText)
		if len(religionList):
			popup.launch(False)

	def __eventPickReligionApply(self, playerID, userData, popupReturn):
		iReligion = 0
		religionsRemaining = self.getRemainingReligions()
		if len(religionsRemaining):
			iReligion = self.getRemainingReligions()[popupReturn.getButtonClicked()]
		self.iReligionList.append(iReligion)
		if self.foundCustomReligion:
			gc.getPlayer(self.iPlayer).foundReligion(iReligion, self.iSlotReligion, True)
		else:
			#identical slots triggers the SDK to automatically resolve a religion
			gc.getPlayer(self.iPlayer).foundReligion(self.iSlotReligion, self.iSlotReligion, True)
		#Recursively call more popups
		self.lock = False
		if len(self.iPlayerQueue):
			self.foundPickableReligions(self.iPlayerQueue, self.iSlotQueue)

	def getRemainingReligions(self):
		religionsRemaining = []
		for i in range(gc.getNumReligionInfos()):
			if not i in self.iReligionList and CyGame().getReligionGameTurnFounded(i) == -1:
				religionsRemaining.append(i)
		return religionsRemaining
