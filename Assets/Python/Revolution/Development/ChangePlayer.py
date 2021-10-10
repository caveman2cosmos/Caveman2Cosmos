# ChangePlayer Mod
#
# by jdog5000
# Version 0.71

from CvPythonExtensions import *
# --------- Revolution mod -------------
import RevDefs
import RevUtils
import BugCore

# globals
GC = CyGlobalContext()
GAME = GC.getGame()

RevOpt = BugCore.game.Revolution

class ChangePlayer:

	def __init__(self, customEM, RevOpt):
		print "Initializing ChangePlayer Mod"

		self.RevOpt = RevOpt
		self.customEM = customEM

		self.customEM.setPopupHandler(RevDefs.changeCivPopup, ["changeCivPopup",changeCivHandler,self.blankHandler])
		self.customEM.setPopupHandler(RevDefs.changeHumanPopup, ["changeHumanPopup",changeHumanHandler,self.blankHandler])
		self.customEM.setPopupHandler(RevDefs.updateGraphicsPopup, ["updateGraphicsPopup",updateGraphicsHandler,self.blankHandler])

	def removeEventHandlers(self):
		print "Removing event handlers from ChangePlayer"

		self.customEM.setPopupHandler(RevDefs.changeCivPopup, ["changeCivPopup",self.blankHandler,self.blankHandler])
		self.customEM.setPopupHandler(RevDefs.changeHumanPopup, ["changeHumanPopup",self.blankHandler,self.blankHandler])
		self.customEM.setPopupHandler(RevDefs.updateGraphicsPopup, ["updateGraphicsPopup",self.blankHandler,self.blankHandler])

	# Dummy handler to take the second event for popup
	def blankHandler(self, playerID, netUserData, popupReturn): return

# Chooser window for switching a players civ
def changeCivPopup():
	iNumCivs = GC.getMAX_PC_PLAYERS()
	popup = CyPopup(RevDefs.changeCivPopup, EventContextTypes.EVENTCONTEXT_ALL, True)
	popup.setHeaderString('Change a civ?', 1<<2)
	popup.setBodyString('Which civ to change, to which civ and leader and what team?', 1<<0)
	popup.addSeparator()

	popup.createPythonPullDown('Switch this civ ...', 1)
	for iPlayerX in xrange(iNumCivs):
		CyPlayerX = GC.getPlayer(iPlayerX)
		if CyPlayerX.isAlive():
			popup.addPullDownString(CyPlayerX.getName() + " - " + CyPlayerX.getCivilizationShortDescription(0), iPlayerX, 1)

	CyPlayer = GC.getActivePlayer()
	popup.setSelectedPulldownID(CyPlayer.getID(), 1)

	popup.createPythonPullDown(' ... to this civ', 2)
	for i in range(GC.getNumCivilizationInfos()):
		popup.addPullDownString("%s Empire" %(GC.getCivilizationInfo(i).getAdjective(0)), i, 2)

	popup.setSelectedPulldownID(CyPlayer.getCivilizationType(), 2)

	popup.createPythonPullDown(' ... with this leader', 3)
	for i in xrange(GC.getNumLeaderHeadInfos()):
		popup.addPullDownString(GC.getLeaderHeadInfo(i).getDescription(), i, 3)

	popup.setSelectedPulldownID(CyPlayer.getLeaderType(), 3)

	popup.createPythonPullDown(' ... on this team', 4)
	popup.addPullDownString("Keep current team", -1, 4)  # Team idx of -1 maintains current team setting
	for iPlayerX in xrange(iNumCivs):
		CyPlayerX = GC.getPlayer(iPlayerX)
		if CyPlayerX.isAlive():
			popup.addPullDownString("Team with the " + CyPlayerX.getCivilizationShortDescription(0), iPlayerX, 4)

	popup.setSelectedPulldownID(-1, 4)
	popup.addSeparator()
	popup.addButton('Cancel')
	popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

# Handles changeCiv popup
def changeCivHandler(playerID, netUserData, popupReturn):

	if not popupReturn.getButtonClicked():
		return # pressed 'cancel' button

	iPlayer = popupReturn.getSelectedPullDownValue(1)
	newCivType = popupReturn.getSelectedPullDownValue(2)
	newLeaderType = popupReturn.getSelectedPullDownValue(3)
	iTeam = popupReturn.getSelectedPullDownValue(4)

	if RevUtils.changeCiv(iPlayer, newCivType, newLeaderType, iTeam):
		CyInterface().addImmediateMessage("Player %d has been changed"%(iPlayer),"")
		print "	CP: Player change completed"
	else:
		CyInterface().addImmediateMessage("An error occured in changeCiv.","")
		print "	CP: Error on changeCiv"


def updateGraphicsHandler( playerID, netUserData, popupReturn ):

	if( popupReturn.getButtonClicked() == 0 ):  # if you pressed update now
		updateGraphics( )

def updateGraphics():
	# Switch human player around to force a redraw of unit flags
	iHuman = GAME.getActivePlayer()

	iSwitchTo = -1
	for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
		CyPlayer = GC.getPlayer(iPlayerX)
		if not CyPlayer.isAlive():
			iSwitchTo = iPlayerX
			break

	if iSwitchTo < 0:
		iSwitchTo = 1 + GAME.getSorenRandNum(GC.getMAX_PC_PLAYERS() - 1, 'ChangePlayer')

	GAME.setForcedAIAutoPlay(iHuman, 3, True)

	RevUtils.changeHuman(iSwitchTo, iHuman)

	RevUtils.changeHuman(iHuman, iSwitchTo)

	#GAME.setAIAutoPlay(0)

# Chooser window for switching human player
def changeHumanPopup(bDied=False):
	popup = CyPopup(RevDefs.changeHumanPopup, EventContextTypes.EVENTCONTEXT_ALL, True)
	popup.setHeaderString('Pick a new civ', 1<<2)
	if bDied:
		popup.setBodyString('Your civ has been eliminated. Would you like to continue as another leader?', 1<<0)
	else:
		popup.setBodyString('Which civ would you like to lead?', 1<<0)
	popup.addSeparator()

	popup.createPythonPullDown('Take control of this civ ...', 1)
	for iPlayerX in range(GC.getMAX_PC_PLAYERS()):
		CyPlayerX = GC.getPlayer(iPlayerX)
		if CyPlayerX.isAlive():
			popup.addPullDownString(CyPlayerX.getName() + " - " + CyPlayerX.getCivilizationShortDescription(0), iPlayerX, 1)

	popup.setSelectedPulldownID(GC.getActivePlayer().getID(), 1)
	popup.addSeparator()
	popup.addButton('Cancel')
	popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

# Handles changeHuman popup
def changeHumanHandler(playerID, netUserData, popupReturn):

	if not popupReturn.getButtonClicked():  # if you pressed cancel
		return

	newHumanIdx = popupReturn.getSelectedPullDownValue( 1 )
	newPlayer = GC.getPlayer(newHumanIdx)
	oldHumanIdx = playerID
	oldPlayer = GC.getPlayer(oldHumanIdx)

	if newHumanIdx == oldHumanIdx:
		print "	CP: You have selected the same civ, no change"
		CyInterface().addImmediateMessage("You retain control of the %s"%(oldPlayer.getCivilizationDescription(0)),"")
		return


	print "	CP: You have selected player %d, the %s" %(newHumanIdx, newPlayer.getCivilizationDescription(0))

	success = RevUtils.changeHuman(newHumanIdx, oldHumanIdx)

	if success:
		print "	CP: Active player is now %d" % GAME.getActivePlayer()
		CyInterface().addImmediateMessage("You now control the %s"%(newPlayer.getCivilizationDescription(0)),"")
	else:
		print "	CP: Error occured, number of human players is now %d" % GAME.getNumHumanPlayers()
		CyInterface().addImmediateMessage("An error occured in changeHuman ...", "")

##def changeCiv( playerIdx, newCivType, newLeaderType, teamIdx = -1 ):
##	# Changes specified players civ, leader and/or team
##	# Does not change isHuman setting
##
##	player = GC.getPlayer(playerIdx)
##	success = GAME.changePlayer( playerIdx, newCivType, newLeaderType, teamIdx, player.isHuman(), True )
##
##	#GAME.convertUnits( playerIdx )
##
##	return success
##
##def changeHuman( newHumanIdx, oldHumanIdx ):
##
##	newPlayer = GC.getPlayer(newHumanIdx)
##	oldPlayer = GC.getPlayer(oldHumanIdx)
##
##	GAME.setActivePlayer( newHumanIdx, False )
##	success = GAME.changePlayer( newHumanIdx, newPlayer.getCivilizationType(), newPlayer.getLeaderType(), -1, True, False )
##	if( success ):
##		success = GAME.changePlayer( oldHumanIdx, oldPlayer.getCivilizationType(), oldPlayer.getLeaderType(), -1, False, False )
##
##	return success