## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
##
## EventSigns.py for the BUG Mod by Dresden
##
## Keeps track of plot signs created by the EventSigns random
## event interface and displays them once the plot is visible.
##

from CvPythonExtensions import *
import BugUtil
import CvUtil
import CvRandomEventInterface
import SdToolKit

# Bug Options
import BugCore
EventSignsOpt = BugCore.game.EventSigns


# civ globals
GC = CyGlobalContext()
engine = CyEngine()
TRNSLTR = CyTranslator()
MAP = GC.getMap()

# for sdtoolkit
SD_MOD_ID = "EventSigns"
SD_VAR_ID = "savedSigns"

# globals for data
gSavedSigns = None
gCurrentSigns = None
g_bShowSigns = False
g_bForceUpdate = False

# Module-level access functions
def initData ():
	""" Initialize the internal plot-caption data structure, clearing all previous data. """
	print "EventSigns.initData() initializing gSavedSigns"
	global gSavedSigns
	gSavedSigns = MapSigns()
	return True

def initOptions ():
	""" Initialization based upon BUG Options. """
	global g_bShowSigns
	g_bShowSigns = EventSignsOpt.isEnabled()
	print "EventSigns.initOptions() initializing. g_bShowSigns is %s." % g_bShowSigns
	return True

def enabledOptionChanged (pIniObject, bNewValue):
	""" Handler function for processing changes to the Enabled option. """
	print "EventSigns.enabledOptionsChanged(%s, %s) resetting g_bShowSigns." %(str(pIniObject), str(bNewValue))
	global g_bShowSigns
	if g_bShowSigns != bNewValue:
		g_bShowSigns = bNewValue
		if gSavedSigns == None:
			initData()
		gSavedSigns.processSigns(g_bShowSigns)
	return True


def addSign(pPlot, ePlayer, szCaption):
	""" Wrapper for CyEngine.addSign() which stores sign data.
	If -1 is passed for ePlayer, the sign is assumed to be a landmark that everyone can see.

	Fix by God-Emperor
	"""
	if not pPlot :
		BugUtil.warn("EventSigns.addSign() was passed an invalid plot for caption : %s" % szCaption)
		return False
	if pPlot.getX() == -1 or pPlot.getY() == -1:
		BugUtil.warn("EventSigns.addSign() was passed an plot with -1 coords for caption : %s" % szCaption)
		return False
	if gSavedSigns == None:
		BugUtil.warn("EventSigns.addSign() gSavedSigns is not initialized!")
		return False

	gSavedSigns.storeSign(pPlot, ePlayer, szCaption)
	gSavedSigns.displaySign(pPlot, ePlayer)
	SdToolKit.sdSetGlobal(SD_MOD_ID, SD_VAR_ID, gSavedSigns)
	return True


def updateCurrentSigns():
	""" Updates gCurrentSigns global with all current signs on map. Remember to clear when done."""
	global gCurrentSigns
	gCurrentSigns = MapSigns()
	for iSign in range(engine.getNumSigns()):
		pSign = engine.getSignByIndex(iSign)
		pPlot = pSign.getPlot()
		pPlot = MAP.plot(pPlot.getX(), pPlot.getY())
		if pPlot:
			ePlayer = pSign.getPlayerType()
			szCaption = pSign.getCaption()
			if not gCurrentSigns.hasPlotSigns(pPlot):
				gCurrentSigns[pPlot] = PlotSigns(pPlot)
			gCurrentSigns[pPlot].setSign(ePlayer, szCaption)
	print "EventSigns.updateCurrentSigns() finished.\n %s" % str(gCurrentSigns)
	return True

def clearCurrentSigns ():
	""" Resets gCurrentSigns global; should always be called when finished after an update. """
	global gCurrentSigns
	gCurrentSigns = None


'''
def clearSignsAndLandmarks(pPlot):
	""" Removes any current landmarks or signs from a plot.

	In order to place a new sign or landmark I'd like to remove any pre-existing sign
	or landmark on the plot. However, there seems to be some delay or synch issue.
	Every attempt I have thus far made would remove the old one but the new one would
	not show up. So for now, I am not removing old signs/landmarks and thus the event
	will only place the sign/landmark on a plot if there isn't already one there.
	If I could resolve that issue, this function would actually be used. ;)
	"""
	for iPlayer in range(GC.getMAX_PLAYERS()):
		engine.removeSign(pPlot, iPlayer)
	engine.removeLandmark(pPlot)
	# Don't even know what this does; it was the last of my failed attempts to force the signs to show.
	#engine.setDirty(EngineDirtyBits.GlobeTexture_DIRTY_BIT, True)
	return True
'''

def placeLandmark(pPlot, sEventType, iFood, iProd, iComm, bIsSign, iSignOwner):
	""" Places a landmark on a plot identifying a yield change with a short description.

	Parameters:
	* pPlot is the CyPlot object for the plot to mark.
	* sEventType is the str key for the event which is used to get the sign's descriptive caption.
	* iFood, iProd, and iComm are the integer yield changes which will be noted on the caption.
	* bIsSign will be True if we want a team-specific sign and False if we want a generic landmark.
	* iSignOwner is the player number for the visibility of the sign. If it's -1, all players get one.
	Note that iSignOwner is unused if bIsSign is False since everyone can see landmarks.
	"""
	# Bail out early if EventSigns are disabled
	if not EventSignsOpt.isEnabled(): return False

	# Bail out if there are no yield changes
	if iFood == 0 and iProd == 0 and iComm == 0: return False

	# This next bit is unused; see the docstring at the start of that function for why.
	#clearSignsAndLandmarks(pPlot)

	sCaptionFood = ""
	sCaptionProd = ""
	sCaptionComm = ""
	sCaptionDesc = TRNSLTR.getText("TXT_KEY_SIGN_" + sEventType, ())

	# Note the extra spaces added for separation after each yield adjustment; you can remove them
	# if you want a more condensed sign; the reason they are here instead of in the XML formats
	# is because I couldn't come up with a simple way to make them appear only if the yield changes.
	if iFood != 0:
		sCaptionFood = TRNSLTR.getText("TXT_KEY_SIGN_FORMAT_FOOD", (iFood,)) + " "
	if iProd != 0:
		sCaptionProd = TRNSLTR.getText("TXT_KEY_SIGN_FORMAT_PROD", (iProd,)) + " "
	if iComm != 0:
		sCaptionComm = TRNSLTR.getText("TXT_KEY_SIGN_FORMAT_COMM", (iComm,)) + " "

	sCaption = TRNSLTR.getText("TXT_KEY_SIGN_FORMAT_OVERVIEW", (sCaptionFood, sCaptionProd, sCaptionComm, sCaptionDesc))

	if not bIsSign:
		engine.addLandmark(pPlot, sCaption)

	elif iSignOwner == -1:
		# add signs for all valid human players who are still alive.
		for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
			player = GC.getPlayer(iPlayer)
			if player.isAlive() and player.isHuman():
				addSign(pPlot, iPlayer, sCaption)

	else: addSign(pPlot, iSignOwner, sCaption)

	return True

def applyLandmarkFromEvent(argsList):
	""" Generic event callback function to place signs/landmarks when event changes plot yields """
	iEvent = argsList[0]
	kTriggeredData = argsList[1]

	event = GC.getEventInfo(iEvent)
	iFood = event.getPlotExtraYield(YieldTypes.YIELD_FOOD)
	iProd = event.getPlotExtraYield(YieldTypes.YIELD_PRODUCTION)
	iComm = event.getPlotExtraYield(YieldTypes.YIELD_COMMERCE)

	if iFood != 0 or iProd != 0 or iComm != 0:
		pPlot = MAP.plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
		if pPlot:
			placeLandmark(pPlot, event.getType(), iFood, iProd, iComm, True, -1)

	return True


class MapSigns:
	""" A collection of PlotSigns, organized by plot. """

	def __init__ (self):
		""" Class initialization. """
		self.reset()

	def reset (self):
		""" Resets data for this instance to defaults. """
		self.plotDict = {}

	def isEmpty (self):
		""" Check to see if object has any PlotSigns data. """
		return not self.plotDict

	def getPlotSigns (self, pPlot):
		""" Returns PlotSigns object for given Plot. """
		thisKey = self.__getKey(pPlot)
		if thisKey in self.plotDict:
			return self.plotDict[thisKey]
		return None

	def __getitem__ (self, pPlot):
		""" Special method to allow access like pPlotSigns = pMapSigns[pPlot] """
		return self.getPlotSigns(pPlot)

	def setPlotSigns (self, pPlot, pPlotSigns):
		""" Sets PlotSigns object for given Plot. """
		thisKey = self.__getKey(pPlot)
		self.plotDict[thisKey] = pPlotSigns
		return None

	def __setitem__ (self, pPlot, pPlotSigns):
		""" Special method to allow access like pMapSigns[pPlot] = pPlotSigns """
		return self.setPlotSigns(pPlot, pPlotSigns)

	def removePlotSigns(self, pPlot):
		""" Removes PlotSigns object for given Plot. """
		thisKey = self.__getKey(pPlot)
		if thisKey in self.plotDict:
			del self.plotDict[thisKey]
			return True
		return False

	def __delitem__ (self, pPlot):
		""" Special method to allow access like del pMapSigns[pPlot] """
		return self.removePlotSigns(pPlot)

	def hasPlotSigns (self, pPlot):
		""" Do we have a PlotSigns element corresponding to that plot? """
		thisKey = self.__getKey(pPlot)
		if thisKey in self.plotDict:
			return True
		return False

	def __str__ (self):
		""" String representation of class instance. """
		# The above doesn't seem to propagate the %s to the PlotSigns, so we do it the long way
		szText = "MapSigns { plotDict = {"
		for key in self.plotDict:
			pPlotSigns = self.plotDict[key]
			szText = szText + "\n\t" + str(key) + ": " + str(pPlotSigns) + ", "
		szText = szText + " } }"
		return szText

	def storeSign (self, pPlot, ePlayer, szCaption):
		""" Stores sign data in the appropraite PlotSigns element. """
		thisKey = self.__getKey(pPlot)
		if not thisKey:
			BugUtil.warn("MapSigns.storeSign() could not determine valid keyname for Plot %s." % (str(pPlot)))
			return False
		if thisKey not in self.plotDict:
			self.plotDict[thisKey] = PlotSigns(pPlot)
		self.plotDict[thisKey].setSign(ePlayer, szCaption)

	def displaySign (self, pPlot, ePlayer):
		""" Displays stored sign for given player at given plot based on revealed status.
		If there's a pre-existing sign, engine.addSign will silently fail, leaving the plot unchanged.
		"""
		if not g_bShowSigns:
			print "MapSigns.displaySign() called but EventSigns is disabled."
			return False
		if not pPlot:
			BugUtil.warn("MapSigns.displaySign() was passed an invalid plot: %s" % (str(pPlot)))
			return False
		thisKey = self.__getKey(pPlot)
		szCaption = ""
		if self.hasPlotSigns(pPlot):
			szCaption = self.plotDict[thisKey].getSign(ePlayer)
		else:
			return False
		if not szCaption:
			print "MapSigns.displaySign() could not show sign; no caption found for player %d on plot %s" %(ePlayer, str(thisKey))
			return False
		if ePlayer == -1:
			if pPlot.isInViewport():
				print "MapSigns.displaySign() landmark (%s) shown on plot %s" %(szCaption, ePlayer, str(thisKey))
				engine.addLandmark(pPlot.cloneToViewport(), szCaption.encode('latin_1'))
		else:
			pPlayer = GC.getPlayer(ePlayer)
			if not pPlayer:
				BugUtil.warn("MapSigns.displaySign() was passed an invalid player id: %s" % (str(ePlayer)))
				return False
			eTeam = pPlayer.getTeam()
			if pPlot.isRevealed(eTeam, False):
				if pPlot.isInViewport():
					print "MapSigns.displaySign() sign (%s) shown for player %d on plot %s" %(szCaption, ePlayer, str(thisKey))
					engine.addSign(pPlot.cloneToViewport(), ePlayer, szCaption.encode('latin_1'))
				return True
			else:
				print "MapSigns.displaySign() could not show sign; player %d cannot see plot %s" %(ePlayer, str(thisKey))
		return False

	def hideSign (self, pPlot, ePlayer):
		""" Hides sign for given player at given plot if there's a current sign the same as the stored one.
		Note that this function assumes gCurrentSigns is up-to-date so make sure you've updated first.
		"""
		if not pPlot:
			BugUtil.warn("MapSigns.hideSign() was passed an invalid plot: %s" % (str(pPlot)))
			return False
		thisKey = self.__getKey(pPlot)
		if gCurrentSigns == None:
			print "MapSigns.hideSign() finds no current signs so there's nothing to hide."
			return False
		if self.hasPlotSigns(pPlot):
			szCaption = self.plotDict[thisKey].getSign(ePlayer)
			if gCurrentSigns.hasPlotSigns(pPlot):
				szExistingCaption = gCurrentSigns[pPlot].getSign(ePlayer)
				if szCaption and szCaption == szExistingCaption:
					if pPlot.isInViewport():
						print "MapSigns.hideSign() found matching sign (%s) for player %d on plot %s; will remove it" %(szCaption, ePlayer, str(thisKey))
						if ePlayer == -1:
							engine.removeLandmark(pPlot.cloneToViewport())
						else:
							engine.removeSign(pPlot.cloneToViewport(), ePlayer)
					return True
				else:
					print "MapSigns.hideSign() found sign for player %d saying (%s) instead of (%s) on plot %s; will leave alone." %(ePlayer, szExistingCaption, szCaption, str(thisKey))
			else:
				print "MapSigns.hideSign() found no sign on plot %s to remove" % str(thisKey)
		else:
			print "MapSigns.hideSign() found no saved signs at all for plot %s" % str(thisKey)
		return False

	def removeSign (self, pPlot, ePlayer):
		""" Removes sign for given player at given plot from storage. """
		if self.hasPlotSigns(pPlot):
			thisKey = self.__getKey(pPlot)
			self.plotDict[thisKey].removeSign(ePlayer)
			# If that was the last caption stored, clean up after ourselves
			if self.plotDict[thisKey].isEmpty():
				del self.plotDict[thisKey]
				return True
		return False

	def processSigns (self, bShow = None):
		""" Shows or hides all signs based on boolean argument which defaults to global g_bShowSigns. """
		print "MapSigns.processSigns() starting. bShow = %s and g_bShowSigns = %s" %(str(bShow), str(g_bShowSigns))
		updateCurrentSigns()
		if bShow == None:
			bShow = g_bShowSigns
		for pSign in self.plotDict.itervalues():
			pPlot = pSign.getPlot()
			if pPlot:
				print "MapSigns.processSigns() Found saved sign data for plot %d, %d ..." %(pPlot.getX(), pPlot.getY())
				for ePlayer in pSign.getPlayers():
					print "MapSigns.processSigns() ... and caption for player %d" % ePlayer
					if (bShow):
						self.displaySign(pPlot, ePlayer)
					else:
						self.hideSign(pPlot, ePlayer)
		clearCurrentSigns()
		return True

	# Private Methods

	def __getKey(self, pPlot):
		""" Gets keyname used to access this plot object. """
		thisKey = None
		if pPlot:
			thisKey = (pPlot.getX(), pPlot.getY())
		return thisKey


class PlotSigns:
	""" Sign information for all players for a given plot. """

	def __init__(self, pPlot):
		""" Class initialization. Parameter is Plot Object. """
		if pPlot:
			self.reset()
			self.iX = pPlot.getX()
			self.iY = pPlot.getY()

	def reset(self):
		""" Resets data for this instance to defaults. """
		self.iX = -1
		self.iY = -1
		self.signDict = {}

	def isEmpty(self):
		""" Check to see if object has any caption data. """
		return (len(self.signDict) == 0)

	def getPlot(self):
		""" Returns plot object. """
		return MAP.plot(self.iX, self.iY)

	def setPlot(self, pPlot):
		""" Assigns plot object. """
		if pPlot:
			self.iX = pPlot.getX()
			self.iY = pPlot.getY()

	def getPlayers(self):
		""" Returns a set of player IDs corresponding to stored signs. """
		ePlayerSet = set()
		for ePlayer in self.signDict:
			ePlayerSet.add(ePlayer)
		return ePlayerSet

	def getSign(self, ePlayer):
		""" Returns Caption for a given player on this plot. """
		szCaption = ""
		if ePlayer in self.signDict:
			szCaption = self.signDict[ePlayer]
		return szCaption

	def setSign(self, ePlayer, szCaption):
		""" Sets Caption for a given player on this plot. """
		if ePlayer in ([-1] + range(GC.getMAX_PLAYERS())):
			self.signDict[ePlayer] = szCaption
		else:
			BugUtil.warn("EventSigns PlotSigns.setSign() was passed an invalid Player ID %s at Plot (%d,%d)" % (str(ePlayer), self.iX, self.iY))

	def removeSign(self, ePlayer):
		""" Removes Caption for a given player on this plot. """
		if ePlayer in self.signDict:
			del self.signDict[ePlayer]
		else:
			BugUtil.warn("EventSigns PlotSigns.removeSign() failed to find a caption for Player %d at Plot (%d,%d)" % (ePlayer, self.iX, self.iY))

	def __str__ (self):
		""" String representation of class instance. """
		return "PlotSigns { iX = %d, iY = %d, signDict = %s }" % (self.iX, self.iY, str(self.signDict))


class EventSignsEventHandler:
	""" Event Handler for this module. """

	def __init__(self, eventManager):
		print "EventSigns EventSignsEventHandler.__init__(). Resetting data and initing event manager."
		initOptions()
		initData()
		## Init event handlers
		eventManager.addEventHandler("GameStart", self.onGameStart)
		eventManager.addEventHandler("OnLoad", self.onLoadGame)
		eventManager.addEventHandler("OnPreSave", self.onPreSave)
		eventManager.addEventHandler("plotRevealed", self.onPlotRevealed)
		eventManager.addEventHandler("BeginActivePlayerTurn", self.onBeginActivePlayerTurn)

	def onGameStart(self, argsList):
		""" Called when a new game is started """
		initOptions()
		initData()

	def onLoadGame(self, argsList):
		""" Called when a game is loaded """
		initOptions()
		data = SdToolKit.sdGetGlobal(SD_MOD_ID, SD_VAR_ID)
		if (data):
			global gSavedSigns
			gSavedSigns = data
			print "EventSigns Data Loaded:\n %s" % gSavedSigns
		else:
			print "EventSigns has no saved data. Initializing new data."
			initData()
		# Hey guess what? The map isn't fully loaded yet so we can't update the signs yet. Super.
		global g_bForceUpdate
		g_bForceUpdate = True

	def onPreSave(self, argsList):
		""" Called before a game is actually saved """
#		if (gSavedSigns and (not gSavedSigns.isEmpty())):
#			SdToolKit.sdSetGlobal(SD_MOD_ID, SD_VAR_ID, gSavedSigns)

	def onPlotRevealed(self, argsList):
		""" Called when plot is revealed to team. """
		(pPlot, eTeam) = argsList
		if g_bShowSigns and gSavedSigns:

			for ePlayer in range(GC.getMAX_PC_PLAYERS()):
				if GC.getPlayer(ePlayer).getTeam() == eTeam:
					gSavedSigns.displaySign(pPlot, ePlayer)

	def onBeginActivePlayerTurn(self, argsList):
		""" Called at start of active player's turn """
		global g_bForceUpdate
		if g_bForceUpdate:
			gSavedSigns.processSigns(g_bShowSigns)
			g_bForceUpdate = False


## Random Event Callbacks

# This is the only current event which has a pre-defined callback since it may change the
# yields of more than one plot. So in this function we will essentially duplicate what the
# generic landmark event processor does where necessary here. Additions marked with "EventSigns" comments.

def applySaltpeter(argsList):
	kTriggeredData = argsList[1]

	CyPlot = MAP.plot(kTriggeredData.iPlotX, kTriggeredData.iPlotY)
	if not CyPlot:
		return
	iPlayer = kTriggeredData.ePlayer

	event = GC.getEventInfo(argsList[0])
	iFood = event.getPlotExtraYield(YieldTypes.YIELD_FOOD)
	iProd = event.getPlotExtraYield(YieldTypes.YIELD_PRODUCTION)
	iComm = event.getPlotExtraYield(YieldTypes.YIELD_COMMERCE)
	sEventType = event.getType()

	# Add landmark for initial plot, if there is still a yield change
	placeLandmark(CyPlot, sEventType, iFood, iProd, iComm, True, -1)

	iForest = GC.getFEATURE_FOREST()

	listPlots = []
	for plotX in MAP.plots():
		if (plotX.getOwner() == iPlayer and plotX.getFeatureType() == iForest and plotX.isHills()):
			iDistance = plotDistance(kTriggeredData.iPlotX, kTriggeredData.iPlotY, plotX.getX(), plotX.getY())
			if iDistance > 0:
				listPlots.append((iDistance, plotX))

	listPlots.sort()

	iCount = CvRandomEventInterface.getSaltpeterNumExtraPlots()
	for plot in listPlots:
		if not iCount:
			break
		iCount -= 1
		plot[1].setExtraYield(YieldTypes.YIELD_COMMERCE, 1)
		szTxt = TRNSLTR.getText("TXT_KEY_EVENT_SALTPETER_DISCOVERED",())
		CvUtil.sendMessage(szTxt, iPlayer, GC.getEVENT_MESSAGE_TIME(), "", -1, plot[1].getX(), plot[1].getY(), True, True, 0, "", False)
		# Add landmark for other plots too.
		placeLandmark(plot[1], sEventType, iFood, iProd, iComm, True, -1)

