# REVOLUTION_MOD
#
# by jdog5000
# Version 1.5


from CvPythonExtensions import *
import CvUtil
import TextUtil
import math
# --------- Revolution mod -------------
import RevDefs
import RevData
import RevUtils
import RevEvents
import SdToolKit
#RevolutionDCM
import CvScreensInterface
import ScreenResolution as SR

import RebelTypes

import RevInstances
from TextUtils import getCityTextList
import BugCore

# globals
GC = CyGlobalContext()
GAME = GC.getGame()
localText = CyTranslator()
RevOpt = BugCore.game.Revolution

class Revolution:

	def __init__(self, customEM, RevOpt):
		print "Initializing Revolution Component"

		####### Revolution Variables ##########
		self.RevOpt = RevOpt
		self.customEM = customEM

		# Debug settings
		self.LOG_DEBUG = RevOpt.isRevDebugMode()
		self.DEBUG_MESS = RevOpt.isShowDebugMessages()
		self.showRevIndexInPopup = RevOpt.isShowRevIndexInPopup()

		self.maxCivs = RevOpt.getRevMaxCivs()
		if self.maxCivs <= 0 or self.maxCivs > GC.getMAX_PC_PLAYERS():
			self.maxCivs = GC.getMAX_PC_PLAYERS()

		self.offerDefectToRevs = RevOpt.isOfferDefectToRevs()

		self.bRebelTypes = RevOpt.isRebelTypes()
		self.bArtStyleTypes = RevOpt.isArtStyleTypes()

		self.bAllowBreakVassal = RevOpt.isAllowBreakVassal()
		self.allowSmallBarbRevs = RevOpt.isAllowSmallBarbRevs()

		self.centerPopups = RevOpt.isCenterPopups()

		# Fewer than this number of cities won't try for independence, but change leader
		#self.minCitiesForIndependence = config.getint("Revolution","MinCitiesForIndependence",4)
		# Cities considered close to revolution instigator inside this radius
		self.closeRadius = RevOpt.getCloseRadius()

		#Cities may attempt to leave and join dominant culture
		self.culturalRevolution = RevOpt.isCulturalRevolution()
		# Cities with at least this level of your nationality won't start cultural revolutions
		self.maxNationalityThreshold = RevOpt.getMaxNationality()

		# Cities may ask for religious change or independence for religious reasons
		self.religiousRevolution = RevOpt.isReligiousRevolution()
		# Cities with your state religion can join a religious revolution
		self.allowStateReligionToJoin = RevOpt.isAllowStateReligionToJoin()

		# Cities may ask for civics changes
		self.civicRevolution = RevOpt.isCivicRevolution()

		# Cities may ask/demand changes in leadership
		self.leaderRevolution = RevOpt.isLeaderRevolution()
		self.humanLeaderRevolution = RevOpt.isHumanLeaderRevolution()

		# Cities may ask/demand independence
		# This is the base kind of revolution, not really negotiable if you have this component on
		self.independenceRevolution = True

		#self.warningThreshold = config.getint("Revolution", "HumanWarningThreshold", 900)
		self.warnFrac = RevOpt.getHumanWarnFrac()
		self.warnTurns = RevOpt.getWarnTurns()
		#self.revReadyThreshold = config.getint("Revolution", "JoinRevolutionThreshold", 600)
		self.revReadyDividend = RevDefs.revReadyDividend
		self.revReadyDivisor = RevDefs.revReadyDivisor
		self.revReadyFrac = RevDefs.revReadyFrac
		self.revInstigatorThreshold = RevDefs.revInstigatorThreshold
		self.alwaysViolentThreshold = RevDefs.alwaysViolentThreshold
		self.badLocalThreshold = RevDefs.badLocalThreshold
		self.showLocalEffect = 2
		self.showTrend = 5
		self.showStabilityTrend = 2
		self.warWearinessMod = RevOpt.getWarWearinessMod()

		self.turnsBetweenRevs = RevOpt.getTurnsBetweenRevs()
		self.acceptedTurns = RevOpt.getAcceptedTurns()
		self.acquiredTurns = RevOpt.getAcquiredTurns()
		self.buyoffTurns = RevOpt.getBuyoffTurns()
		self.baseReinforcementTurns = RevOpt.getBaseReinforcementTurns()
		self.minReinforcementTurns = RevOpt.getMinReinforcementTurns()
		if self.minReinforcementTurns < 1:
			self.minReinforcementTurns = 1

		# Increase rate of accumulation of revolution index
		self.revIdxModifier = RevOpt.getIndexModifier()
		self.revIdxOffset = RevOpt.getIndexOffset()
		self.humanIdxModifier = RevOpt.getHumanIndexModifier()
		self.humanIdxOffset = RevOpt.getHumanIndexOffset()
		# Change chances a revolution occurs given conditions
		self.chanceModifier = RevOpt.getChanceModifier()
		# Change strength of revolutions that resort to violence
		self.strengthModifier = RevOpt.getStrengthModifier()
		# Change number of rebel reinforcement units
		self.reinforcementModifier = RevOpt.getReinforcementModifier()

		self.distToCapModifier = RevOpt.getDistanceToCapitalModifier()
		self.happinessModifier = RevOpt.getHappinessModifier()
		self.cultureRateModifier = RevOpt.getCultureRateModifier()
		self.garrisonModifier = RevOpt.getGarrisonModifier()
		self.nationalityModifier = RevOpt.getNationalityModifier()
		# Change strength of city area revolution factor
		self.colonyModifier = RevOpt.getColonyModifier()
		# Change strength of civ size revolution factor
		self.civSizeModifier = RevOpt.getCivSizeModifier()
		# Change strength of religion revolution factor
		self.religionModifier = RevOpt.getReligionModifier()
		# Change strength of losing a city on revolution indices
		self.cityLostModifier = RevOpt.getCityLostModifier()
		# Change strength of gaining a city on revolution indices
		self.cityAcquiredModifier = RevOpt.getCityAcquiredModifier()
		# Change the initial strength of rev culture in a captured city (1.0 = 50% nationality)
		self.revCultureModifier = RevOpt.getCultureRateModifier()

		self.iRevRoll = 20000

		# Stores human leader type when human loses control of civ
		self.humanLeaderType = None

		# RevolutionMP
		# Network protocol header
		self.netRevolutionPopupProtocol = 100
		self.netControlLostPopupProtocol = 101

		######################
		# These are initialized on first end of game turn
		self.iNationalismTech = None
		self.iLiberalismTech = None
		self.iSciMethodTech = None

		############# Register events and popups ##############
		# City and civ events
		customEM.addEventHandler("cityAcquired", self.onCityAcquired)
		customEM.addEventHandler("PreEndGameTurn", self.onEndGameTurn)
		customEM.addEventHandler("BeginPlayerTurn", self.onBeginPlayerTurn)
		customEM.addEventHandler("EndPlayerTurn", self.onEndPlayerTurn)
		customEM.addEventHandler("ModNetMessage", self.onModNetMessage)

		# Popup launching and handling events
		customEM.addEventHandler("kbdEvent", self.onKbdEvent)
		self.customEM.setPopupHandler(RevDefs.revolutionPopup, ["revolutionPopup",self.revolutionPopupHandler,self.blankHandler])
		self.customEM.setPopupHandler(RevDefs.joinHumanPopup, ["joinHumanPopup",self.joinHumanHandler,self.blankHandler])
		self.customEM.setPopupHandler(RevDefs.revWatchPopup, ["revWatchPopup", self.revWatchHandler, self.blankHandler])
		self.customEM.setPopupHandler(RevDefs.pickCityPopup, ["pickCityPopup", self.pickCityHandler, self.blankHandler])
		self.customEM.setPopupHandler(RevDefs.bribeCityPopup, ["bribeCityPopup", self.bribeCityHandler, self.blankHandler])

	def removeEventHandlers(self):
		print "Removing event handlers from Revolution"

		self.customEM.removeEventHandler("cityAcquired", self.onCityAcquired)
		self.customEM.removeEventHandler("PreEndGameTurn", self.onEndGameTurn)
		self.customEM.removeEventHandler("BeginPlayerTurn", self.onBeginPlayerTurn)
		self.customEM.removeEventHandler("EndPlayerTurn", self.onEndPlayerTurn)
		self.customEM.removeEventHandler("ModNetMessage", self.onModNetMessage)

		# Popup launching and handling events
		self.customEM.removeEventHandler( "kbdEvent", self.onKbdEvent )

		self.customEM.setPopupHandler(RevDefs.revolutionPopup, ["revolutionPopup",self.blankHandler,self.blankHandler])
		self.customEM.setPopupHandler(RevDefs.joinHumanPopup, ["joinHumanPopup",self.blankHandler,self.blankHandler])
		self.customEM.setPopupHandler(RevDefs.controlLostPopup, ["controlLostPopup",self.blankHandler,self.blankHandler])
		self.customEM.setPopupHandler(RevDefs.revWatchPopup, ["revWatchPopup", self.blankHandler, self.blankHandler])
		self.customEM.setPopupHandler(RevDefs.pickCityPopup, ["pickCityPopup", self.blankHandler, self.blankHandler])
		self.customEM.setPopupHandler(RevDefs.bribeCityPopup, ["bribeCityPopup", self.blankHandler, self.blankHandler])

	def blankHandler(self, playerID, netUserData, popupReturn):
		# Dummy handler to take the second event for popup
		return

	def isLocalHumanPlayer(self, playerID):
		# Determines whether to show popup to active player
		return GC.getPlayer(playerID).isHuman() and GAME.getActivePlayer() == playerID

	def isLocalHumanPlayerOrAutoPlay(self, playerID):
		# Determines whether to show popup to active player
		return (GC.getPlayer(playerID).isHuman() or GC.getPlayer(playerID).isHumanDisabled()) and GAME.getActivePlayer() == playerID

	def isHumanPlayerOrAutoPlay(self, playerID):
		# Determines whether to show popup to active player
		return GC.getPlayer(playerID).isHuman() or GC.getPlayer(playerID).isHumanDisabled()

	def loadInfo(self):
		# Function loads info required by other components
		if self.LOG_DEBUG: CvUtil.pyPrint( "  Loading revolution data" )

		if self.bRebelTypes:
			RebelTypes.setup()

		self.iNationalismTech = GC.getInfoTypeForString(RevDefs.sXMLNationalism)
		self.iLiberalismTech = GC.getInfoTypeForString(RevDefs.sXMLLiberalism)
		self.iSciMethodTech = GC.getInfoTypeForString(RevDefs.sXMLSciMethod)

		self.showLocalEffect = int(self.showLocalEffect*RevUtils.getGameSpeedMod())


##--- Keyboard handling and Rev Watch popup -------------------------------------------

	def onKbdEvent(self, argsList):
		eventType, key, mx, my, px, py = argsList

		if eventType == 6:
			# multiplayer warning, need to tell other computers about any city bribery
			if key == InputTypes.KB_G and self.customEM.bShift and self.customEM.bCtrl:
				CvScreensInterface.showRevolutionWatchAdvisor(self)


	def showRevWatchPopup(self, iPlayer):
		if self.isLocalHumanPlayer(iPlayer):

			danger  = "<color=255,0,0,255>" + localText.getText("TXT_KEY_REV_WATCH_DANGER", ()) + ":\n<color=255,255,255,255>"
			warning = "<color=245,245,0,255>\n" + localText.getText("TXT_KEY_REV_WATCH_WARNING", ()) + ":\n<color=255,255,255,255>"
			safe	= "<color=0,230,0,255>\n" + localText.getText("TXT_KEY_REV_WATCH_SAFE", ()) + ":\n<color=255,255,255,255>"

			# Get rev index change
			revIdxCityList = []
			for cityX in GC.getPlayer(iPlayer).cities():
				revIdxCityList.append((cityX.getRevolutionIndex(), cityX))

			if revIdxCityList:
				revIdxCityList.sort()
				revIdxCityList.reverse()
				bDebug = GAME.isDebugMode()

			# Set rev index change
			self.updateLocalRevIndices(GAME.getGameTurn(), iPlayer, bIsRevWatch = True)

			for revIdx, CyCityX in revIdxCityList:

				localRevIdx = CyCityX.getLocalRevIndex()
				deltaTrend = deltaTrend = revIdx - CyCityX.getRevIndexAverage()

				if revIdx >= self.revInstigatorThreshold:
					if deltaTrend > self.showTrend:
						if revIdx >= self.alwaysViolentThreshold:
							danger += "<color=255,0,0,255>  " + CyCityX.getName()
						else:
							danger += "<color=255,120,0,255>  " + CyCityX.getName()
						danger +=  "<color=255,255,255,255>"
					else:
						danger += "  " + CyCityX.getName()

					if self.showRevIndexInPopup or bDebug:
						danger += "  \t(%d)" % revIdx
					danger += "\n"
				elif revIdx >= int(self.revReadyFrac*self.revInstigatorThreshold + .5):
					if deltaTrend > self.showTrend:
						warning += "<color=255,120,0,255>  %s<color=255,255,255,255>" % CyCityX.getName()
					else:
						warning += "  " + CyCityX.getName()

					if self.showRevIndexInPopup or bDebug:
						warning += "  \t(%d)" % revIdx
					warning += "\n"
				else:
					if deltaTrend > self.showTrend:
						safe += "<color=255,120,0,255>  %s<color=255,255,255,255>" % CyCityX.getName()
					else:
						safe += "  " + CyCityX.getName()

					if self.showRevIndexInPopup or bDebug:
						safe += "  \t(%d)" % revIdx
					safe += "\n"

			xRes = SR.x
			yRes = SR.y

			# Additions by Caesium et al
			caesiumpasx = xRes/10
			caesiumpasy = yRes/10
			width = int(max([350, 2.5*caesiumpasx]))
			height = int(max([450, 3.5*caesiumpasy]))
			popup = CyPopup(RevDefs.revWatchPopup, EventContextTypes.EVENTCONTEXT_ALL, False)
			if self.centerPopups:
				popup.setPosition(3*caesiumpasx, 3*caesiumpasy)
			else:
				popup.setPosition(xRes - width - 35, 120)
			popup.setSize(width, height)
			popup.setHeaderString(localText.getText("TXT_KEY_REV_WATCH_TITLE", ()), 1<<2)
			popup.setBodyString(danger + warning + safe, 1<<0)
			popup.addSeparator()
			popup.addButton(localText.getText("TXT_KEY_REV_WATCH_DETAIL", ()))
			popup.addButton(localText.getText("TXT_KEY_REV_WATCH_BRIBE", ()))
			popup.addButton('OK')
			popup.launch(False, PopupStates.POPUPSTATE_IMMEDIATE)
			# End additions by Caesium et al

	def revWatchHandler(self, iPlayerID, netUserData, popupReturn):
		if self.iNationalismTech == None:
			self.loadInfo()

		civString = self.updateCivStability( GAME.getGameTurn(), iPlayerID, bIsRevWatch = True )
		cityString = self.updateLocalRevIndices( GAME.getGameTurn(), iPlayerID, bIsRevWatch = True )

		if self.isLocalHumanPlayer(iPlayerID):
			if popupReturn.getButtonClicked() == 0:
				if self.LOG_DEBUG: CvUtil.pyPrint("[REV] Revolt: Showing detailed rev watch for player %d"%(iPlayerID))
				# Additions by Caesium et al
				caesiumtR = CyUserProfile().getResolutionString(CyUserProfile().getResolution())
				caesiumtextResolution = caesiumtR.split('x')
				caesiumpasx = int(caesiumtextResolution[0])/10
				caesiumpasy = int(caesiumtextResolution[1])/10
				popup = CyPopup(-1, EventContextTypes.NO_EVENTCONTEXT, True)
				width = int(max([350,3*caesiumpasx]))
				height = int(max([450,3.5*caesiumpasy]))
				if self.centerPopups:
					popup.setPosition(3*caesiumpasx,3*caesiumpasy)
				else:
					popup.setPosition(int(caesiumtextResolution[0]) - width - 35,120)
				popup.setSize(width, height)
				popup.setHeaderString(GC.getPlayer(iPlayerID).getCivilizationDescription(0), 1<<2)
				popup.setBodyString(civString + '\n\n' + cityString, 1<<0)
				popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)
				# End additions by Caesium et al
			elif popupReturn.getButtonClicked() == 1:
				self.showPickCityPopup(iPlayerID)

	def showPickCityPopup(self, iPlayer):
		if self.isLocalHumanPlayer(iPlayer):
			lCity = []
			for city in GC.getPlayer(iPlayer).cities():
				lCity.append((pCity.getRevolutionIndex(), pCity.getName(), pCity.getID()))

			if not lCity:
				popup = CyPopup(-1, EventContextTypes.NO_EVENTCONTEXT, True)
				popup.setBodyString(localText.getText("TXT_KEY_REV_WATCH_NO_CITIES", ()), 1<<0)
				popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)
				return

			lCity.sort()
			lCity.reverse()

			popup = CyPopup(RevDefs.pickCityPopup, EventContextTypes.EVENTCONTEXT_ALL, False)
			popup.setBodyString(localText.getText("TXT_KEY_REV_BRIBE_CITY_WHICH",()), 1<<0)
			popup.addSeparator()

			popup.createPythonPullDown('Cities', 1)
			for cityData in lCity:
				popup.addPullDownString(cityData[1], cityData[2], 1)

			popup.addButton(localText.getText("TXT_KEY_REV_NONE",()))
			popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)
			print 'Launch city picker popup'


	def pickCityHandler(self, iPlayerID, netUserData, popupReturn):
		if self.isLocalHumanPlayer(iPlayerID):
			print 'picking city ...'

			if popupReturn.getButtonClicked() == 0:
				# None selected
				return

			cityID = popupReturn.getSelectedPullDownValue( 1 )
			if cityID >= 0:
				pCity = GC.getPlayer(iPlayerID).getCity(cityID)
				self.showBribeCityPopup(pCity)


	def showBribeCityPopup(self, pCity):
		popup = CyPopup(RevDefs.bribeCityPopup, EventContextTypes.EVENTCONTEXT_ALL, True)

		popupData = {'City' : pCity.getID(), 'Buttons' : []}

		bCanBribe, reason = RevUtils.isCanBribeCity(pCity)

		if not bCanBribe:
			if reason == 'Violent':
				# Can't be bought
				popup.setBodyString(localText.getText("TXT_KEY_REV_BRIBE_CITY_VIOLENT",()) % pCity.getName(), 1<<0)
			elif reason == 'No Need':
				popup.setBodyString(localText.getText("TXT_KEY_REV_BRIBE_CITY_NO_NEED",()) % pCity.getName(), 1<<0)
			else:
				popup.setBodyString('Error! unknown reason for inability to bribe #s: %s' % (pCity.getName(), reason), 1<<0)
				print '[ERROR] unknown reason for inability to bribe %s: %s' % (pCity.getName(), reason)

			popupData['Buttons'] = [['None',-1]]
			popup.setUserData((popupData,))
			popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)
			return

		CyPlayer = GC.getPlayer(pCity.getOwner())
		[iSmall,iMed,iLarge] = RevUtils.computeBribeCosts(pCity)
		buttonList = []
		lastBribeTurn = RevData.getCityVal( pCity, 'BribeTurn' )

		iGold = CyPlayer.getGold()
		if iGold < iSmall:
			bodStr = localText.getText("TXT_KEY_REV_BRIBE_CITY_POOR",())
			if( not lastBribeTurn == None and GAME.getGameTurn() - lastBribeTurn < 20 ) :
				bodStr += '  ' + localText.getText("TXT_KEY_REV_BRIBE_CITY_RECENT",())%(GAME.getGameTurn() - lastBribeTurn) + '  '
			popup.setBodyString(bodStr, 1<<0)
			popupData['Buttons'] = [['None',-1]]
			popup.setUserData((popupData,))
			popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)
			return

		buttonList.append( [localText.getText("TXT_KEY_REV_NONE",()), -1] )
		bodStr = ''
		if not lastBribeTurn == None and GAME.getGameTurn() - lastBribeTurn < 20:
			bodStr += localText.getText("TXT_KEY_REV_BRIBE_CITY_RECENT",())%(GAME.getGameTurn() - lastBribeTurn) + '  '

		bodStr += localText.getText("TXT_KEY_REV_BRIBE_CITY_OPTIONS",())
		bodStr += '\n\n' + localText.getText("TXT_KEY_REV_BRIBE_CITY_SMALL",())%(iSmall)
		buttonList.append( ['Small', iSmall] )

		if iGold > iMed:
			bodStr += '\n' + localText.getText("TXT_KEY_REV_BRIBE_CITY_MED",())%(iMed)
			buttonList.append( ['Med', iMed] )
			if iGold > iLarge:
				bodStr += '\n' + localText.getText("TXT_KEY_REV_BRIBE_CITY_LARGE",())%(iLarge)
				buttonList.append( ['Large', iLarge] )

		popup.setBodyString(bodStr, 1<<0)

		popup.addButton(localText.getText("TXT_KEY_REV_BRIBE_CITY_NO_BRIBE",()))
		for [label, cost] in buttonList:
			if cost > 0:
				popup.addButton( localText.getText("TXT_KEY_REV_BRIBE_CITY_BUTTON",())%(cost) )
		popupData['Buttons'] = buttonList
		popup.setUserData((popupData,))
		popup.launch(False, PopupStates.POPUPSTATE_IMMEDIATE)


	def bribeCityHandler( self, iPlayerID, netUserData, popupReturn ) :
		print 'bribing city ...'

		pPlayer = GC.getPlayer( iPlayerID )
		pCity = pPlayer.getCity( netUserData[0]['City'] )
		[buttonLabel, iCost] = netUserData[0]['Buttons'][popupReturn.getButtonClicked()]

		if( buttonLabel == 'None' ) :
			return
		elif( buttonLabel == 'Small' ) :
			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Small bribe selected for city %s"%(pCity.getName()))
			RevUtils.bribeCity( pCity, 'Small' )
			pPlayer.changeGold( -iCost )
		elif( buttonLabel == 'Med' ) :
			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Med bribe selected for city %s"%(pCity.getName()))
			RevUtils.bribeCity( pCity, 'Med' )
			pPlayer.changeGold( -iCost )
		elif( buttonLabel == 'Large' ) :
			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Large bribe selected for city %s"%(pCity.getName()))
			RevUtils.bribeCity( pCity, 'Large' )
			pPlayer.changeGold( -iCost )
		else :
			print 'Error! Bribe city button label %s not recognized, cost is %d'%(buttonLabel,iCost)
			return

		#RevolutionDCM - post bribe popup handling
		if CvScreensInterface.isRevolutionWatchAdvisor():
		   CvScreensInterface.showRevolutionWatchAdvisor(self)
		else:
		   CvScreensInterface.cityScreenRedraw()


##--- Standard Event handling functions -------------------------------------------

	def onEndGameTurn(self, argsList):

		if self.iNationalismTech == None:
			self.loadInfo()

		self.topCivAdjustments()


	def onBeginPlayerTurn(self, argsList):
		iPlayer = argsList[1]

		if iPlayer > GC.getMAX_PC_PLAYERS():
			return
		iPlayer -= 1

		while iPlayer > -1:
			if GC.getPlayer(iPlayer).isAlive():
				self.checkForRevReinforcement(iPlayer)
				self.checkCivics(iPlayer)
				break
			iPlayer -= 1


	def onEndPlayerTurn(self, argsList):
		iGameTurn, iPlayer = argsList

		iMax = GC.getMAX_PC_PLAYERS()

		if iPlayer > iMax:
			return
		# Do the check when iPlayer = iMax (Beast NPC) to cover the case where there's only one civ alive.
		iNextPlayer = iPlayer + 1

		while iNextPlayer != iPlayer:
			if iNextPlayer >= iMax:
				iGameTurn += 1
				iNextPlayer = 0
			CyPlayer = GC.getPlayer(iNextPlayer)

			if RevData.revObjectExists(CyPlayer) and RevData.revObjectGetVal(CyPlayer, 'SpawnList'):
				if CyPlayer.isAlive():
					self.updatePlayerRevolution([iGameTurn, iNextPlayer])
				self.launchRevolution(iNextPlayer)
				break

			if CyPlayer.isAlive():
				self.updatePlayerRevolution([iGameTurn, iNextPlayer])
				break

			iNextPlayer += 1


	def onCityAcquired( self, argsList):
		'City Acquired'

		owner,playerType,pCity,bConquest,bTrade = argsList

		self.updateLocalRevIndices( GAME.getGameTurn(), pCity.getOwner(), subCityList = [pCity], bIsRevWatch = True )

##--- Player turn functions ---------------------------------------

	def checkForRevReinforcement(self, iPlayer):
		# Checks iPlayer's cities for any rebel reinforcement units that should be spawned
		# Should be called at end of player's turn
		for city in GC.getPlayer(iPlayer).cities():
			if city.getReinforcementCounter() == 1:
				self.doRevReinforcement(city)


	def doRevReinforcement( self, pCity ) :

		revCivType = RevData.getCityVal(pCity, 'RevolutionCiv')
		ownerID = pCity.getOwner()
		owner = GC.getPlayer(ownerID)

		# City must have valid rev player
		if( revCivType < 0 ) :
			return
		if( revCivType == owner.getCivilizationType() ) :
			# Already captured and got capture bonus
			return

		pRevPlayer = None
		for i in xrange(GC.getMAX_PC_PLAYERS()) :
			playerI = GC.getPlayer(i)
			if( playerI.isAlive() and playerI.getCivilizationType() == revCivType ) :
				pRevPlayer = playerI
				break

		if( pRevPlayer == None ) :
			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Checking for end to Barbarian uprising in %s"%(pCity.getName()))
			pRevPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())

		pRevTeam = GC.getTeam(pRevPlayer.getTeam())

		if( not pRevTeam.isAtWar(owner.getTeam()) ) :
			# Revolt has ended
			return

		# City must still be rebellious
		revIdx = pCity.getRevolutionIndex()
		localRevIdx = pCity.getLocalRevIndex()
		localRevEffect = 0
		revIdxHist = RevData.getCityVal( pCity, 'RevIdxHistory' )

#-------------------------------------------------------------------------------------------------
# Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
		if( revIdx < self.revInstigatorThreshold ) :
			localRevEffect = min([-revIdx/10.0,6.0*localRevIdx,-25.0])
			revIdxHist['RevoltEffects'][0] += localRevEffect
			RevData.updateCityVal( pCity, 'RevIdxHistory', revIdxHist )
			pCity.changeRevolutionIndex( int(localRevEffect))
			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Local rebellion in %s ends due to low rev index"%(pCity.getName()))
			return
		elif( localRevIdx < -(self.badLocalThreshold/2) ) :
			localRevEffect = min([-revIdx/8.0,8.0*localRevIdx,-50.0])
			revIdxHist['RevoltEffects'][0] += localRevEffect
			RevData.updateCityVal( pCity, 'RevIdxHistory', revIdxHist )
			pCity.changeRevolutionIndex( int(localRevEffect))
			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Local rebellion in %s ends due to improving situation"%(pCity.getName()))
			return
#-------------------------------------------------------------------------------------------------
# END Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
		rebPower = pCity.area().getPower(pRevPlayer.getID())
		ownerPower = pCity.area().getPower(ownerID)

		if( rebPower > 2*ownerPower ) :
			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Reinforcing %s: Rebel power %d much higher than owner %d in area, No Reinforce for 3 turns"%(pCity.getName(),rebPower,ownerPower))
			pCity.setReinforcementCounter(3 + 1)
			return

		# Do reinforcement
		if( self.LOG_DEBUG and not pRevPlayer.isBarbarian() ) : CvUtil.pyPrint("[REV] Revolt: Reinforcing rebel %s outside %s (%d, %d, owned by %s)"%(pRevPlayer.getCivilizationDescription(0),pCity.getName(),revIdx,localRevIdx,owner.getCivilizationDescription(0)))
		spawnableUnits = RevUtils.getUprisingUnitTypes(pCity)

		if( len(spawnableUnits) < 1 ) :
			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: ERROR!!! No rev units possible in %s"%(pCity.getName()))
			return

		ix = pCity.getX()
		iy = pCity.getY()

		for cityX in pRevPlayer.cities():
			if GAME.getGameTurn() - cityX.getGameTurnAcquired() < 6 and cityX.getPreviousOwner() == ownerID:
				bRecentSuccess = True
				break
		else: bRecentSuccess = False

		iRebelsIn6 = RevUtils.getNumDefendersNearPlot( ix, iy, pRevPlayer.getID(), iRange = 6 )
		iRebelsIn3 = RevUtils.getNumDefendersNearPlot( ix, iy, pRevPlayer.getID(), iRange = 3 )

		if pRevPlayer.isBarbarian():

			if iRebelsIn6 == 0:

				localRevEffect = min([-revIdx/8.0,8.0*localRevIdx,-50.0])
				revIdxHist['RevoltEffects'][0] += localRevEffect
				RevData.updateCityVal( pCity, 'RevIdxHistory', revIdxHist )
				pCity.changeRevolutionIndex( int(localRevEffect))
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Barbarian local rebellion in %s put down, idx drop of %d from %d"%(pCity.getName(),localRevEffect,revIdx))

			elif iRebelsIn3 == 0:
				pCity.setReinforcementCounter(2+1)
				pCity.changeRevolutionIndex( int(min([-revIdx/20.0,4.0*localRevIdx,-25.0])) )
			else:
				pCity.setReinforcementCounter( 3 + 1 )

			# Never actually spawn reinforcements for barb rebels, just check again for end of revolt
			return

		if not pRevPlayer.isRebel():

			if( GAME.getGameTurn() - RevData.getCityVal(pCity, 'RevolutionTurn') > 5 ) :
				if( iRebelsIn3 == 0 ) :
					# No rebel troops near here, effectively end active revolt
					localRevEffect = min([-revIdx/6.0,10.0*localRevIdx,-100.0])
					revIdxHist['RevoltEffects'][0] += localRevEffect
					RevData.updateCityVal( pCity, 'RevIdxHistory', revIdxHist )
					pCity.changeRevolutionIndex( int(localRevEffect))

					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Non-rebel: No nearby troops to reinforce, local rebellion in %s ends, with idx drop of %d from %d"%(pCity.getName(),localRevEffect,revIdx))
					return
				else :
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Non-rebel: Reinforcement window over, but nearby fighting continues")
					pCity.setReinforcementCounter(2+1)
					#pCity.changeRevolutionIndex( min([-revIdx/50,4*localRevIdx,-10]) )
					return

		if not bRecentSuccess:

			if iRebelsIn6 == 0:
				# No rebel troops anywhere near here, effectively end active revolt
				localRevEffect = min([-revIdx/8.0,8.0*localRevIdx,-80.0])
				revIdxHist['RevoltEffects'][0] += localRevEffect
				RevData.updateCityVal( pCity, 'RevIdxHistory', revIdxHist )
				pCity.changeRevolutionIndex( int(localRevEffect))
				if self.LOG_DEBUG:
					print "[REV] No rebel troops to reinforce, local rebellion in %s ends, with idx drop of %d from %d"%(pCity.getName(), localRevEffect, revIdx)
				return

			if iRebelsIn3 == 0:
				if self.LOG_DEBUG:
					print "[REV] No nearby rebel troops to reinforce, try again later"
				pCity.setReinforcementCounter(2+1)
				pCity.changeRevolutionIndex(min([-revIdx / 50, 4 * localRevIdx, -10]))
				return

		elif iRebelsIn6 == 0:
			if self.LOG_DEBUG:
				print "[REV] No rebel troops to reinforce, but local rebellion continues due to recent success elsewhere"

			pCity.changeRevolutionIndex(min([-revIdx / 50, 4 * localRevIdx, -10]))
			pCity.setReinforcementCounter(3+1)



		spawnablePlots = RevUtils.getSpawnablePlots( ix, iy, pRevPlayer, bLand = True, bIncludePlot = False, bIncludeCities = False, bSameArea = True, iRange = 1, iSpawnPlotOwner = pRevPlayer.getID(), bCheckForEnemy = True )
		if( len(spawnablePlots) == 0 ) :
			spawnablePlots = RevUtils.getSpawnablePlots( ix, iy, pRevPlayer, bLand = True, bIncludePlot = False, bIncludeCities = False, bSameArea = True, iRange = 2, iSpawnPlotOwner = pRevPlayer.getID(), bCheckForEnemy = True )
		if( len(spawnablePlots) == 0 ) :
			spawnablePlots = RevUtils.getSpawnablePlots( ix, iy, pRevPlayer, bLand = True, bIncludePlot = False, bIncludeCities = False, bSameArea = True, iRange = 3, iSpawnPlotOwner = -1, bCheckForEnemy = True )

		if( len(spawnablePlots) == 0 ) :
			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: ERROR!!! No rev spawn location possible in %s"%(pCity.getName()))
			return
		revSpawnLoc = spawnablePlots[GAME.getSorenRandNum(len(spawnablePlots),'Revolution: Pick rev plot')]

		revStrength = self.reinforcementModifier*(revIdx/(1.0*self.revInstigatorThreshold))
		revStrength *= min([0.13 + pCity.getPopulation()/8.0,2.0])
		if( localRevIdx > 0 ) :
			revStrength *= min([0.5+localRevIdx/10.0,2.0])
		else :
			revStrength *= 1/(2-localRevIdx/3.0)

		if( pCity.getRevolutionCounter() == 0 ) :
			# Rebellion decreases in fervor after counter expires
			revStrength /= 2.0
		elif( GAME.getGameTurn() - RevData.getCityVal(pCity, 'RevolutionTurn') < 3 ) :
			# Initial fervor
			revStrength *= 1.5

		iDefIn2 = RevUtils.getNumDefendersNearPlot( ix, iy, ownerID, iRange = 2 )
		if( iRebelsIn3 > iDefIn2 ) :
			revStrength *= 1.3
		elif( pRevPlayer.getNumCities() > 0 and iRebelsIn3 > 3 ) :
			# Bolster a successful revolt
			revStrength *= 1.25
		elif( iRebelsIn3 + 2 < iDefIn2 ) :
			# Odds are too steep, some doubt potential for success
			revStrength *= 0.8

		iNumUnits = int(revStrength + .5)
		if( GAME.getGameTurn() - RevData.getCityVal(pCity, 'RevolutionTurn') < 3 ) :
			iNumUnits = min([iNumUnits,(pCity.getPopulation())/4,localRevIdx/2])
		elif( pRevPlayer.getNumCities() > 0 and iRebelsIn3 > 2 ) :
			iNumUnits = min([iNumUnits,(pCity.getPopulation())/4,localRevIdx/2])
		else :
			# Been a few turns since revolution, turn down intensity
			iNumUnits = min([iNumUnits,(pCity.getPopulation())/6,localRevIdx/4])

		iNumUnits = max([iNumUnits,1])

		for iPlayer in xrange(GC.getMAX_PC_PLAYERS()) :

			if( ownerID == iPlayer ) :
				mess = localText.getText("TXT_KEY_REV_MESS_REINFORCEMENTS",()) + " %s!"%(pCity.getName())
				CyInterface().addMessage(iPlayer, True, GC.getEVENT_MESSAGE_TIME(), mess, "AS2D_CITY_REVOLT", InterfaceMessageTypes.MESSAGE_TYPE_MINOR_EVENT, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_RESISTANCE").getPath(), ColorTypes(7), ix, iy, True, True)
			elif( pRevTeam.isAtWar(GC.getPlayer(iPlayer).getTeam()) and pRevPlayer.canContact(iPlayer) ) :
				mess = localText.getText("TXT_KEY_REV_MESS_REINFORCEMENTS",()) + " %s!"%(pCity.getName())
				CyInterface().addMessage(iPlayer, False, GC.getEVENT_MESSAGE_TIME(), mess, None, InterfaceMessageTypes.MESSAGE_TYPE_MINOR_EVENT, None, ColorTypes(7), -1, -1, False, False)
			elif( pRevPlayer.getID() == iPlayer ) :
				mess = localText.getText("TXT_KEY_REV_MESS_YOUR_REINFORCEMENTS",()) + " %s!"%(pCity.getName())
				CyInterface().addMessage(iPlayer, True, GC.getEVENT_MESSAGE_TIME(), mess, "AS2D_CITY_REVOLT", InterfaceMessageTypes.MESSAGE_TYPE_MINOR_EVENT, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_RESISTANCE").getPath(), ColorTypes(8), ix, iy, True, True)

		if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Reinforcement strength %.2f, spawning %d reinforcements for city of size %d"%(revStrength,iNumUnits,pCity.getPopulation()))

		if( len(RevUtils.getEnemyUnits( revSpawnLoc[0], revSpawnLoc[1], pRevPlayer.getID())) > 0 ) :
			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: ERROR!  Spawning on plot with enemy units!!!")

		if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: City at %d,%d spawning at %d,%d"%(ix,iy,revSpawnLoc[0],revSpawnLoc[1]))

		# Spawn rev units outside city
		newUnitList = []
		for i in xrange(iNumUnits) :
			newUnitID = spawnableUnits[GAME.getSorenRandNum( len(spawnableUnits), 'Revolution: pick unit' )]
			newUnit = pRevPlayer.initUnit( newUnitID, revSpawnLoc[0], revSpawnLoc[1], UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH )
			newUnitList.append( newUnit )

		for [iNum,newUnit] in enumerate(newUnitList) :
			if( newUnit.canFight() ) :
				# Injure units to simulate the lack of training in rebel troops
				iDamage = 10 + GAME.getSorenRandNum(15,'Rev - Injure unit')
				newUnit.setDamage( iDamage, ownerID )

			# Check AI settings
			if( newUnit.isBarbarian() ) :
				if( iNumUnits > 2 and pRevPlayer.AI_unitValue(newUnit.getUnitType(),UnitAITypes.UNITAI_ATTACK_CITY_LEMMING,newUnit.area()) > 0 ) :
					newUnit.setUnitAIType( UnitAITypes.UNITAI_ATTACK_CITY_LEMMING )
				elif( newUnit.canFight() ) :
					newUnit.setUnitAIType( UnitAITypes.UNITAI_ATTACK )
			else :
				if( iNum < 2 and iNumUnits + iRebelsIn3 > 2 and pRevPlayer.AI_unitValue(newUnit.getUnitType(),UnitAITypes.UNITAI_ATTACK_CITY,newUnit.area()) > 0 ) :
					newUnit.setUnitAIType( UnitAITypes.UNITAI_ATTACK_CITY )
				elif( iNumUnits == 1 and iRebelsIn6 < 3 and pRevPlayer.AI_unitValue(newUnit.getUnitType(),UnitAITypes.UNITAI_PILLAGE,newUnit.area()) > 0 ) :
					newUnit.setUnitAIType( UnitAITypes.UNITAI_PILLAGE )
				else :
					iniAI = newUnit.getUnitAIType()
					if( not (iniAI == UnitAITypes.UNITAI_COUNTER or iniAI == UnitAITypes.UNITAI_ATTACK_CITY) ) :
						newUnit.setUnitAIType( UnitAITypes.UNITAI_ATTACK )
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s starting with AI type: %d (ini %d)"%(newUnit.getName(),newUnit.getUnitAIType(),iniAI))

			if( revStrength > 1.5 and pRevPlayer.isRebel() ) :
				newUnit.setPromotionReady(True)

		# Occasionally spawn a spy as well
		if 40 - 20*pRevPlayer.AI_getNumAIUnits(UnitAITypes.UNITAI_SPY) > GAME.getSorenRandNum(100,'Rev - Spy'):
			iSpy = pRevPlayer.getBestUnitType(UnitAITypes.UNITAI_SPY)
			if iSpy != -1 and revStrength > 1.5 and pRevPlayer.canTrain(iSpy,False,False):
				pSpy = pRevPlayer.initUnit(iSpy, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
				pSpy.setFortifyTurns(GC.getDefineINT("MAX_FORTIFY_TURNS"))

		# Give a little boost to espionage
		pRevTeam.changeEspionagePointsAgainstTeam( owner.getTeam(), GAME.getSorenRandNum((10+pRevPlayer.getCurrentEra())*iNumUnits, 'Rev - Esp') )

		if( pRevPlayer.isRebel() ) :
			# Set reinforcement timer again
			iReinforceTurns = int(self.baseReinforcementTurns*2.0/revStrength + .5)

			minReinfTurns = max([self.minReinforcementTurns, 4 - owner.getCurrentEra(), 9 - pCity.getPopulation()])

			iReinforceTurns = max([iReinforceTurns,minReinfTurns])
			## Dancing Hoskuld August 2012  - getCityVal may not return an intiger but None also ##
			iCityRevData_DH = RevData.getCityVal(pCity, 'RevolutionTurn')
			if iCityRevData_DH == None:
				iCityRevData_DH = 0

			if pCity.getRevolutionCounter() == 0:
				iReinforceTurns += 2

			elif GAME.getGameTurn() - iCityRevData_DH > 3:
				iReinforceTurns += 1

			iReinforceTurns = min([iReinforceTurns,10])
			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s reinforcement counter set to %d (min %d)"%(pCity.getName(),iReinforceTurns, minReinfTurns))
			pCity.setReinforcementCounter( iReinforceTurns + 1 )

		else :
			pCity.setReinforcementCounter( 3 + 1 )


	def checkCivics(self, iPlayer):

		pPlayer = GC.getPlayer(iPlayer)

		if not pPlayer == None or pPlayer.getNumCities() == 0 or pPlayer.isNPC():
			return

		curCivics = []
		for i in xrange(GC.getNumCivicOptionInfos()):
			curCivics.append(pPlayer.getCivics(i))

		prevCivics = RevData.revObjectGetVal( pPlayer, "CivicList" )

		if prevCivics == None or not len(prevCivics) == len(curCivics):
			RevEvents.recordCivics(pPlayer)
			return

		else:
			sumRevIdx = 0
			bChanged = False
			for [i,curCivic] in enumerate(curCivics):
				if( (not curCivic == prevCivics[i]) and (not prevCivics[i] == -1) ) :
					curInfo  = GC.getCivicInfo(curCivic)
					prevInfo = GC.getCivicInfo(prevCivics[i])
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s changing civic option %d from %s to %s"%(pPlayer.getCivilizationDescription(0),i,prevInfo.getDescription(),curInfo.getDescription()))
					iRevIdxChange = curInfo.getRevIdxSwitchTo() - prevInfo.getRevIdxSwitchTo()
					sumRevIdx += iRevIdxChange

					if( not iRevIdxChange == 0 ) :

						keyList = []
						if( curInfo.getRevDemocracyLevel()*prevInfo.getRevDemocracyLevel() < 0 ) :
							# Democracy level changed sign
							keyList.extend( ['Location', 'Colony', 'Nationality'] )
						elif( curInfo.getRevReligiousFreedom()*prevInfo.getRevReligiousFreedom() < 0 ) :
							# Rel freedom changed sign
							keyList.append( 'Religion' )
						elif( curInfo.getRevLaborFreedom()*prevInfo.getRevLaborFreedom() < 0 ) :
							# Democracy level changed sign
							keyList.extend( ['Colony', 'Nationality'] )

						if( self.LOG_DEBUG and len(keyList) > 0 ) :
							keyStr = ''
							for key in keyList :
								keyStr += key + ', '
							CvUtil.pyPrint("[REV] Revolt: Increasing effect for cities with high %s factors"%(keyStr))

						for cityX in pPlayer.cities():
							revIdxHist = RevData.getCityVal(cityX,'RevIdxHistory')
							iThisRevIdxChange = iRevIdxChange
							mod = 1.0
							for [listIdx,key] in enumerate(keyList):
								if revIdxHist[key][0] > 9:
									mod *= 1.8
								elif revIdxHist[key][0] > 3:
									mod *= 1.4
								elif revIdxHist[key][0] > 0:
									mod *= 1.2

							mod = min([3.0, mod])
							iThisRevIdxChange = int( mod*iRevIdxChange + 0.5 )

							if mod > 1.0 and self.LOG_DEBUG:
								print "[REV] Increasing civic effects in %s" % cityX.getName()

							cityX.changeRevolutionIndex( int(iThisRevIdxChange) )

							revIdxHist['Events'][0] += iThisRevIdxChange
							RevData.updateCityVal(cityX,'RevIdxHistory',revIdxHist)

			if sumRevIdx != 0:
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Avg net effect for %s: %d"%(pPlayer.getCivilizationDescription(0),sumRevIdx))


	def updatePlayerRevolution(self, argsList):
		iGameTurn, iPlayer = argsList

		if self.iNationalismTech == None:
			self.loadInfo()

		self.updateRevolutionCounters(iGameTurn, iPlayer)
		self.updateLocalRevIndices(iGameTurn, iPlayer)
		self.updateCivStability(iGameTurn, iPlayer)
		self.checkForBribes(iGameTurn, iPlayer)
		self.checkForRevolution(iGameTurn, iPlayer)

		self.incrementRevIdxHistory(iGameTurn, iPlayer)


	def updateRevolutionCounters(self, iGameTurn, iPlayer):

		for city in GC.getPlayer(iPlayer).cities():

			if not RevData.revObjectExists(city):
				RevData.initCity(city)
				continue

			if city.getRevolutionCounter() > 0:
				city.changeRevolutionCounter(-1)

			if RevData.getCityVal(city, 'SmallRevoltCounter') > 0:
				RevData.changeCityVal(city, 'SmallRevoltCounter', -1)

			if RevData.getCityVal(city, 'WarningCounter') > 0:
				RevData.changeCityVal(city, 'WarningCounter', -1)

			if city.getReinforcementCounter() > 0:
				city.changeReinforcementCounter(-1)


	def updateLocalRevIndices(self, iGameTurn, iPlayer, subCityList = None, bIsRevWatch = False):
		# Updates the revolution effects local to each city

		pPlayer = GC.getPlayer(iPlayer)
		if not pPlayer.getNumCities():
			return localText.getText("TXT_KEY_REV_WATCH_NO_CITIES",())

		# Gather some data on the civ that will effect every city
		capital = pPlayer.getCapitalCity()
		if capital == None:
			if not pPlayer.isNPC():
				print "[REV] WARNING! %s have cities but no capital on turn %d"%(pPlayer.getCivilizationDescription(0), iGameTurn)
			return localText.getText("TXT_KEY_REV_WATCH_NO_CITIES",())

		pTeam = GC.getTeam(pPlayer.getTeam())

		if self.iNationalismTech == None:
			self.loadInfo()

		hasNationalism = GC.getTeam(pPlayer.getTeam()).isHasTech(self.iNationalismTech)
		hasLiberalism = GC.getTeam(pPlayer.getTeam()).isHasTech(self.iLiberalismTech)
		hasSciMethod = GC.getTeam(pPlayer.getTeam()).isHasTech(self.iSciMethodTech)

		# phungus -start
		[civSizeValue, iCivEffRadius] = RevUtils.computeCivSize(iPlayer)
		civSizeValue *= self.civSizeModifier
		[civSizeRawVal, iCivEffRadRaw] = RevUtils.computeCivSizeRaw(iPlayer)
		civSizeRawVal *= self.civSizeModifier
		if( self.LOG_DEBUG and iGameTurn%25 == 0 ) : CvUtil.pyPrint("[REV] Revolt: %s have civ size value %.2f, era mod %.2f"%(pPlayer.getCivilizationDescription(0),civSizeValue,max( [0, 0.85-0.20*pPlayer.getCurrentEra()] )))
		#phungus -end

		# Prepare string that holds RevWatch text for cities
		totalString = ""

		if not subCityList:
			cityList = pPlayer.cities()
			totalString = localText.getText("TXT_KEY_REV_WATCH_CITY_BY_CITY",())
			if self.showRevIndexInPopup or GAME.isDebugMode():
				totalString += '  ' + localText.getText("TXT_KEY_REV_WATCH_DEBUG_NOTE",())
		else:
			cityList = subCityList
			totalString = ""

		for pCity in cityList:

			# Incase interturn stuff set out of range
			if pCity.getRevolutionIndex() < 0:
				pCity.setRevolutionIndex(0)
			elif pCity.getRevolutionIndex() > (2*self.alwaysViolentThreshold):
				pCity.setRevolutionIndex( (2*self.alwaysViolentThreshold) )

			revIdxHist = RevData.getCityVal(pCity, 'RevIdxHistory')
			if revIdxHist == None:
				revIdxHist = RevDefs.initRevIdxHistory()

			prevRevIdx = pCity.getRevolutionIndex()

			# Record lists of (#, string type) effects, seperate for positive and negative
			posList = []
			negList = []

			# Sum up local factors for the city
			localRevIdx = 0

			recentlyAcquired = (GAME.getGameTurn()-pCity.getGameTurnAcquired() < 12*RevUtils.getGameSpeedMod())

			culturePercent = 0
			maxCult = 0
			maxCultPlayer = -1
			for idx in xrange(GC.getMAX_PC_PLAYERS()) :
				if( pPlayer.getTeam() == GC.getPlayer(idx).getTeam() ) :
					culturePercent += pCity.plot().calculateCulturePercent(idx)
				if( pCity.plot().calculateCulturePercent(idx) > maxCult ) :
					maxCult = pCity.plot().calculateCulturePercent(idx)
					maxCultPlayer = idx

			if( maxCultPlayer < 0 ) :
				maxCultPlayer = iPlayer

			# TODO: does this work with minor civs?
			bWarWithMaxCult  = pTeam.isAtWar(GC.getPlayer(maxCultPlayer).getTeam())
			bMaxCultIsVassal = GC.getTeam(GC.getPlayer(maxCultPlayer).getTeam()).isVassal(pTeam.getID())


			# Happiness
			numUnhappy = RevUtils.getModNumUnhappy( pCity, self.warWearinessMod )
			happyIdx = 0
			if( numUnhappy > 0 ) :

				numUnhappy = max([numUnhappy - (pCity.getRevIndexPercentAnger()*pCity.getPopulation())/1000, 0])

				if( pCity.getOccupationTimer() > 0 ) :
					# Resistance is counted below ...
					numUnhappy = min([numUnhappy/3.0,0.5])
				elif( recentlyAcquired ) :
					numUnhappy = min([numUnhappy/2.0,0.75])

				happyIdx = int((15 + 15*(min([numUnhappy,pCity.getPopulation()])/min([pCity.getPopulation(),12])))*pow(numUnhappy, .8) + .5)

				if( pCity.getEspionageHappinessCounter() > 0 ) :
					# Reduce effect if unhappiness is from spy mission
					happyIdx = happyIdx/3.0

				if( numUnhappy > 0 ) :
					happyIdx = int(self.happinessModifier*happyIdx + .5)
					if( bIsRevWatch ) : negList.append( (happyIdx, localText.getText("TXT_KEY_REV_WATCH_HAPPINESS",())) )


			elif( numUnhappy < 0 ) :
				numHappy = min([abs(numUnhappy),10,pCity.getPopulation()])
				happyIdx = -int( (1.2 + numHappy/pCity.getPopulation())*pow(numHappy, .6) + 0.5 )
				if( bWarWithMaxCult and not pPlayer.isRebel() ) :
					happyIdx /= 2
				happyIdx = int((self.happinessModifier / 3)*happyIdx + .5 )
				if( bIsRevWatch ) : posList.append( (happyIdx, localText.getText("TXT_KEY_REV_WATCH_HAPPINESS",())) )

			localRevIdx += happyIdx
			revIdxHist['Happiness'] = [happyIdx] + revIdxHist['Happiness'][0:RevDefs.revIdxHistLen-1]

			ww = pCity.getWarWearinessPercentAnger()
			if( ww > 0 and self.LOG_DEBUG and iGameTurn%25 == 0 ) : CvUtil.pyPrint("  Rev - %s has ww %d"%(pCity.getName(),ww))

			#if( self.LOG_DEBUG and iGameTurn%25 == 0 ) : CvUtil.pyPrint("[REV] Revolt: %s happiness index: %d"%(pCity.getName(),localRevIdx))

			# phungus -start
			# Distance to capital City Distance modified by communication techs and structures
			cityDistModifier = 0
			map = CyMap()
			deltaX = abs(pCity.getX() - capital.getX())
			if map.isWrapX():
				deltaX = min(deltaX, map.getGridWidth() - deltaX)
			deltaY = abs(pCity.getY() - capital.getY())
			if map.isWrapY():
				deltaY = min(deltaY, map.getGridWidth() - deltaY)
			cityDistRaw = ( deltaX**2 + deltaY**2 )**0.5
			cityDistMapModifier = ( map.getGridWidth()**2 + map.getGridHeight()**2 )**0.5

			cityDistCommBonus = 0
			pTeam = GC.getTeam(pPlayer.getTeam())
			bCanTradeOverCoast = False
			bCanTradeOverOcean = False
			iTerrainCoast = GC.getInfoTypeForString(RevDefs.sXMLCoast)
			iTerrainOcean = GC.getInfoTypeForString(RevDefs.sXMLOcean)
			for i in xrange(GC.getNumTechInfos()):
				tech = GC.getTechInfo(i)
				if tech.isTerrainTrade(iTerrainCoast):
					if pTeam.isHasTech(i):
						bCanTradeOverCoast = True
				if tech.isTerrainTrade(iTerrainOcean):
					if pTeam.isHasTech(i):
						bCanTradeOverOcean = True
			if bCanTradeOverOcean :
				cityDistCommBonus += 50
			iCityTradeRoutes = pCity.getTradeRoutes()
			if(iCityTradeRoutes > 1 ) :
				cityDistCommBonus += (iCityTradeRoutes - 1)*17
			bCityIsConnected = pCity.isConnectedTo(capital)
			if bCityIsConnected :
				eRouteType = pCity.plot().getRouteType()
				bTechRouteModifier = False
				if eRouteType != -1:
					for i in xrange(GC.getNumTechInfos()):
						tech = GC.getTechInfo(i)
						if(GC.getRouteInfo(eRouteType).getTechMovementChange(i) != 0 and pTeam.isHasTech(i)):
							bTechRouteModifier = True
							break
				if bTechRouteModifier:
					cityDistCommBonus += 150 - ( GC.getRouteInfo(pCity.plot().getRouteType()).getFlatMovementCost() + GC.getRouteInfo(pCity.plot().getRouteType()).getTechMovementChange(i) )*1.25
				else:
					cityDistCommBonus += 150 - ( GC.getRouteInfo(pCity.plot().getRouteType()).getFlatMovementCost() )*1.25

				if( pCity.isCoastal(-1) ) :
					if bCanTradeOverOcean :
						cityDistCommBonus += 50
					elif bCanTradeOverCoast :
						cityDistCommBonus += 25
				iTradeRouteModifier = pCity.getTradeRouteModifier()
				iCityCulterModifier = pCity.getCommerceRateModifier(CommerceTypes.COMMERCE_CULTURE)
				iCityGoldModifier = pCity.getCommerceRateModifier(CommerceTypes.COMMERCE_GOLD)
				cityDistCommBonus += iTradeRouteModifier
				cityDistCommBonus += iCityCulterModifier
				cityDistCommBonus += iCityGoldModifier / 2

			iCityMaintenanceModifier = pCity.getMaintenanceModifier()
			cityDistCommBonus -= iCityMaintenanceModifier
			bCityisPower = pCity.isPower()
			if bCityisPower :
				cityDistCommBonus += 150
			iCityAirlift = pCity.getMaxAirlift()
			cityDistCommBonus += 100*iCityAirlift

			if( not capital == None and not pCity.isConnectedTo(capital) ) :
				cityDistCommBonus += min(150, 50 + pCity.getTradeRoutes() * 10)

			cityDistModifier = ( 307.0*cityDistRaw / cityDistMapModifier ) / ( 1.0 + ( cityDistCommBonus / 100.0 ) )
			cityDistModifier -= int(666 / cityDistMapModifier)

			CivicsDistModifier = RevUtils.getCivicsDistanceMod( iPlayer )
			TraitsDistModifier = RevUtils.getTraitsDistanceMod( iPlayer )
			BuildingsDistModifier = RevUtils.getBuildingsDistanceMod( pCity )
			DistModifier = (CivicsDistModifier + TraitsDistModifier + BuildingsDistModifier) / 100.0
			distMod = 1.0
			if( DistModifier < 0 ) :
				distMod /= (1.0 - DistModifier)
			elif( DistModifier > 0 ) :
				distMod += DistModifier

			distMod *= self.distToCapModifier
			if( pCity.isGovernmentCenter() ) :
				distMod *= 0.5

			locationRevIdx = 0

			if( civSizeRawVal > 2.0 ) :
				locationRevIdx += int(2.0*cityDistModifier*distMod + .5)
				if( bIsRevWatch and cityDistModifier > 1.5 ) :
					negList.append( (locationRevIdx, localText.getText("TXT_KEY_REV_WATCH_DISTANT",())) )
			elif( civSizeRawVal > 1.6 ) :
				locationRevIdx += int(1.65*cityDistModifier*distMod + .5)
				if( bIsRevWatch and cityDistModifier > 1.6 ) :
					negList.append( (locationRevIdx, localText.getText("TXT_KEY_REV_WATCH_DISTANT",())) )
			elif( civSizeRawVal > 1.4 ) :
				locationRevIdx += int(1.45*cityDistModifier*distMod + .5)
				if( bIsRevWatch and cityDistModifier > 1.7 ) :
					negList.append( (locationRevIdx, localText.getText("TXT_KEY_REV_WATCH_DISTANT",())) )
			elif( civSizeRawVal > 1.2 ) :
				locationRevIdx += int(1.25*cityDistModifier*distMod + .5)
				if( bIsRevWatch and cityDistModifier > 1.8 ) :
					negList.append( (locationRevIdx, localText.getText("TXT_KEY_REV_WATCH_DISTANT",())) )
			elif( civSizeRawVal > 1.0 ) :
				locationRevIdx += int(cityDistModifier*distMod + .5)
				if( bIsRevWatch and cityDistModifier > 1.9 ) :
					negList.append( (locationRevIdx, localText.getText("TXT_KEY_REV_WATCH_DISTANT",())) )
			elif( civSizeRawVal > .7 ) :
				locationRevIdx += int(.75*cityDistModifier*distMod + .5)
				if( bIsRevWatch and cityDistModifier > 2.2 ) :
					negList.append( (locationRevIdx, localText.getText("TXT_KEY_REV_WATCH_DISTANT",())) )
			else :
				locationRevIdx += int(.5*cityDistModifier*distMod + .5)
				if( bIsRevWatch and cityDistModifier > 3.0 ) :
					negList.append( (locationRevIdx, localText.getText("TXT_KEY_REV_WATCH_DISTANT",())) )

			locationRevIdx = int(locationRevIdx + .5 )
			localRevIdx += locationRevIdx
			revIdxHist['Location'] = [locationRevIdx] + revIdxHist['Location'][0:RevDefs.revIdxHistLen-1]
			# phungus -end

			# Colony
			colonyIdx = 0
			if( not (pCity.area().getID() == capital.area().getID()) ) :
				colBase = min([cityDistModifier*pCity.getPopulation()/3.0 + 0.5, 10])
				if( hasNationalism ) :
					if( pCity.getCultureLevel() > 2 ) :
						if( pCity.plot().calculateCulturePercent(iPlayer) > 90 ) :
							colonyIdx = int(self.colonyModifier*distMod*0.5*colBase + .5)
						elif( pCity.plot().calculateCulturePercent(iPlayer) > 70 ) :
							colonyIdx = int(self.colonyModifier*distMod*1.0*colBase + .5)
						else :
							colonyIdx = int(self.colonyModifier*distMod*1.2*colBase + .5)
					else :
						colonyIdx = int(self.colonyModifier*distMod*1.5*colBase + .5)
				else :
					colonyIdx = int(self.colonyModifier*distMod*1*colBase + .5)

				if( pCity.area().getNumCities() == 1 ) :
					if( culturePercent > 90 ) :
						if( plotDistance( pCity.getX(), pCity.getY(), capital.getX(), capital.getY() ) < 2.0*self.closeRadius ) :
							colonyIdx = 0
						elif( plotDistance( pCity.getX(), pCity.getY(), capital.getX(), capital.getY() ) < 4.0*self.closeRadius ) :
							colonyIdx /= 3
						else :
							colonyIdx /= 2
					elif( pCity.area().getNumTiles() < 5 ) :
						colonyIdx /= 2

				if( colonyIdx > 0 ) :
					if( bIsRevWatch ) : negList.append( (colonyIdx, localText.getText("TXT_KEY_REV_WATCH_COLONY",())) )

			# Connected to capital
			if( not capital == None and not pCity.isConnectedTo(capital) ) :
				if( recentlyAcquired or pPlayer.isRebel() ) :
					connIdx = 2
				else :
					connIdx = min([3 + pPlayer.getCurrentEra() + pCity.getPopulation()/3, 6])

				if( bIsRevWatch ) : negList.append( (connIdx, localText.getText("TXT_KEY_REV_WATCH_NOT_CONNECTED",())) )
				colonyIdx += connIdx

			localRevIdx += colonyIdx
			revIdxHist['Colony'] = [colonyIdx] + revIdxHist['Colony'][0:RevDefs.revIdxHistLen-1]

			if( self.LOG_DEBUG and iGameTurn%25 == 0 ) : CvUtil.pyPrint("[REV] Revolt: %s location effects: [%d,%d], maintenance: [%.2f,%.2f], modifier: %d, gcent: %d"%(pCity.getName(),locationRevIdx,colonyIdx,pCity.getMaintenanceTimes100()/100.0,pCity.calculateColonyMaintenanceTimes100()/100.0,pCity.getMaintenanceModifier(),pCity.isGovernmentCenter()))

			# Religion
			relIdx = 0
			if( pPlayer.isStateReligion() ) :
				stateRel = pPlayer.getStateReligion()
				if( stateRel >= 0 ) :
					relGoodIdx = 0
					relBadIdx = 0
					for relType in xrange(GC.getNumReligionInfos()) :
						if( pCity.isHasReligion(relType) ) :
							if( relType == stateRel ) :
								relGoodIdx += 4
							else :
								if( relBadIdx > 4 ) :
									relBadIdx += 1
								else :
									relBadIdx += 2.5

					# Holy city
					if( pCity.isHolyCity() ) :
						if( pCity.isHolyCityByType(stateRel) ) :
							relGoodIdx += 5
						else :
							relBadIdx += 4

					[holyCityGood,holyCityBad] = RevUtils.getCivicsHolyCityEffects(iPlayer)
					if( not hasLiberalism and stateRel >= 0 ) :
						if( pCity.isHasReligion(stateRel) ) :
							stateHolyCity = GAME.getHolyCity( stateRel )
							if stateHolyCity:
								holyCityOwnerID = stateHolyCity.getOwner()
								if( (not holyCityGood == 0) and (holyCityOwnerID == iPlayer) ) :
									#phungus Rev Trait Effects
									[traitHolyCityGood, traitHolyCityBad] = RevUtils.getTraitsHolyCityEffects(iPlayer)
									if ( traitHolyCityGood != 0 ) :
										holyCityGood += traitHolyCityGood
									#Rev Trait End
									posList.append( (-holyCityGood, localText.getText("TXT_KEY_REV_WATCH_HOLY_CITY",())) )
									relGoodIdx += holyCityGood
								elif( (not holyCityBad == 0) and (not GC.getPlayer(holyCityOwnerID).getStateReligion() == stateRel) ) :
									#phungus Rev Trait Effects
									[traitHolyCityGood, traitHolyCityBad] = RevUtils.getTraitsHolyCityEffects(iPlayer)
									if ( traitHolyCityBad != 0 ) :
										holyCityBad += traitHolyCityBad
									#Rev Trait End
									negList.append( (holyCityBad, localText.getText("TXT_KEY_REV_WATCH_HEATHENS",())) )
									localRevIdx += holyCityBad

					[relGoodMod,relBadMod] = RevUtils.getCivicsReligionMods(iPlayer)
					#phungus Rev Trait Effects
					[traitRelGoodMod,traitRelBadMod] = RevUtils.getTraitsReligionMods(iPlayer)

					relBadIdx = int(relBadIdx*(1.0 + relBadMod + traitRelBadMod) + .5)
					relGoodIdx = int(relGoodIdx*(1.0 + relGoodMod + traitRelGoodMod) + .5)
					#Rev Trait End

					if( pTeam.getAtWarCount(True) > 1 ) :
						# God is on your side =P
						relGoodIdx = int(relGoodIdx*1.5 + .5)

					relIdx = int(self.religionModifier*(relBadIdx - relGoodIdx) + .5)
					relGoodIdx = int(self.religionModifier*(relGoodIdx) + .5)
					relBadIdx = int(self.religionModifier*(relBadIdx) + .5)

					if( hasSciMethod ) :
						relIdx = relIdx/3
						relGoodIdx /=3
						relBadIdx /= 3
					elif( hasLiberalism ) :
						relIdx = relIdx/2
						relGoodIdx /= 2
						relBadIdx /= 2

					if( bIsRevWatch ) :
						if( relBadIdx > 1 ) :
							negList.append( (relBadIdx, localText.getText("TXT_KEY_REV_WATCH_NON_STATE_RELIGION",())) )
						if( relGoodIdx < -1 ) :
							posList.append( (relGoodIdx, localText.getText("TXT_KEY_REV_WATCH_STATE_RELIGION",())) )

					localRevIdx += relIdx
			revIdxHist['Religion'] = [relIdx] + revIdxHist['Religion'][0:RevDefs.revIdxHistLen-1]
			#if( self.LOG_DEBUG and iGameTurn%25 == 0 ) : CvUtil.pyPrint("[REV] Revolt: %s religious index: %d, total: %d"%(pCity.getName(),relIdx,localRevIdx))

			# Culture
			adjCultRate = max([pCity.getCommerceRate(CommerceTypes.COMMERCE_CULTURE) - pPlayer.getCurrentEra(), 0])
			cultIdx = -min([int( pow(abs(adjCultRate/(1.5+pPlayer.getCurrentEra()/3.0)), .7) + 0.5 ), 10])
			cultIdx = int(self.cultureRateModifier*cultIdx + .5)
			if( bWarWithMaxCult and not pPlayer.isRebel() ) :
				cultIdx /= 2
			if( bIsRevWatch and cultIdx < -1 ) : posList.append( (cultIdx, localText.getText("TXT_KEY_REV_WATCH_CULTURE_RATE",())) )
			localRevIdx += cultIdx

			# Nationality
			natIdx = 0

			if( culturePercent > 90 ) :
				natIdx = -4
			elif( culturePercent > 70 ) :
				natIdx = -2
			elif( culturePercent < 20 ) :
				natIdx = 26
				natIdx -= pCity.getCommerceRate(CommerceTypes.COMMERCE_CULTURE)
				natIdx = max([natIdx,9])
			elif( culturePercent < 40 ) :
				natIdx = 15
				natIdx -= pCity.getCommerceRate(CommerceTypes.COMMERCE_CULTURE)
				natIdx = max([natIdx,6])
			elif( culturePercent < 50 ) :
				natIdx = 8
				natIdx -= pCity.getCommerceRate(CommerceTypes.COMMERCE_CULTURE)/2
				natIdx = max([natIdx,3])
			elif( culturePercent < 60 ) :
				natIdx = 3
				natIdx -= pCity.getCommerceRate(CommerceTypes.COMMERCE_CULTURE)/2
				natIdx = max([natIdx,0])

			if( recentlyAcquired ) :
				natIdx = int(3*natIdx/5.0 + .5)
				natIdx = min([natIdx,12])

			if( bWarWithMaxCult and not pPlayer.isRebel() and natIdx > 0 ) :
				natIdx = (3*natIdx)/2
			elif( bMaxCultIsVassal and natIdx > 0 ) :
				natIdx = min([(2*natIdx)/3,10])

			if( not hasNationalism ) :
				natIdx = int(natIdx/2.0 + .5)

			if( natIdx > 0 ) :
				natMod = RevUtils.getCivicsNationalityMod( iPlayer )
				natIdx = int(natIdx*(1.0 + natMod) + .5)

			natIdx = int(self.nationalityModifier*natIdx + .5)
			if bIsRevWatch:
				if natIdx > 0:
					negList.append( (natIdx, localText.getText("TXT_KEY_REV_WATCH_NATIONALITY",())) )
				elif natIdx < 0:
					posList.append( (natIdx, localText.getText("TXT_KEY_REV_WATCH_NATIONALITY",())) )

			localRevIdx += natIdx
			revIdxHist['Nationality'] = [natIdx] + revIdxHist['Nationality'][0:RevDefs.revIdxHistLen-1]

			# Health
			healthIdx = 0
			numUnhealthy = (pCity.badHealth(False) - pCity.goodHealth())
			if numUnhealthy > 0:
				healthIdx = int(math.floor(2*pow(numUnhealthy, .6) + .5) * self.happinessModifier)
				if pCity.getEspionageHealthCounter() > 0 or pPlayer.isRebel():
					healthIdx = healthIdx/3
				if bIsRevWatch:
					negList.append( (healthIdx, localText.getText("TXT_KEY_REV_WATCH_UNHEALTHY",())) )
				localRevIdx += healthIdx
			revIdxHist['Health'] = [healthIdx] + revIdxHist['Health'][0:RevDefs.revIdxHistLen-1]

			# Garrison
			garIdx = 0
			if hasLiberalism:
				garIdx = -int( 2*pow(pCity.plot().getNumDefenders(iPlayer)/2.0, .5) - .5 )
			elif pCity.getBuildingDefense() > 75:
				garIdx = -int( 3*pow(pCity.plot().getNumDefenders(iPlayer)/2.0, .6) + .5 )
			elif pCity.getBuildingDefense() > 25:
				garIdx = -int( 2.5*pow(pCity.plot().getNumDefenders(iPlayer)/2.0, .6) )
			else:
				garIdx = -int( 2*pow(pCity.plot().getNumDefenders(iPlayer)/2.0, .6) - .5 )

			garIdx *= int((100 + (pCity.plot().getRevoltProtection())) / 100) # Afforess: Revolt Protection

			garIdx = int(self.garrisonModifier*garIdx + .5)

			if natIdx > 0:
				if garIdx < -10: garIdx = -10
			elif garIdx < -15: garIdx = -15

			if bIsRevWatch and garIdx <= -2:
				posList.append( (garIdx, localText.getText("TXT_KEY_REV_WATCH_GARRISON",())) )

			localRevIdx += garIdx
			revIdxHist['Garrison'] = [garIdx] + revIdxHist['Garrison'][0:RevDefs.revIdxHistLen-1]

			# Spirit
			if( pCity.getNumRevolts(pPlayer.getID()) > 2 and prevRevIdx > int(self.warnFrac*self.revInstigatorThreshold) ) :
				spiritIdx = pCity.getNumRevolts(pPlayer.getID())/2
				if( bIsRevWatch ) : negList.append( (spiritIdx, localText.getText("TXT_KEY_REV_WATCH_SPIRIT",())) )
				localRevIdx += spiritIdx

			# Size
			if( pCity.getPopulation() < 2 + min([pPlayer.getCurrentEra(),3]) ) :
				if( pCity.isCapital() and pPlayer.getCurrentEra() - GAME.getStartEra() > 2 ) :
					# To help remove late game tiny civs
					localRevIdx += 4
				elif( pCity.getHighestPopulation() > 7 ) :
					# City has starved a lot, don't forgive and forget
					localRevIdx += 2
				else :
					# Small cities don't get worked up as quickly
					if( localRevIdx > 0 ) :
						sizeIdx = -3 + pCity.getPopulation() - min([pPlayer.getCurrentEra(),3])
						if( bIsRevWatch ) : posList.append( (sizeIdx, localText.getText("TXT_KEY_REV_WATCH_SMALL_CITY",())) )
						localRevIdx += sizeIdx
						localRevIdx = max([localRevIdx,0])

			# Starving
			if( pCity.foodDifference(True) < 0 ) :
				if( abs(pCity.foodDifference(True)) > pCity.getFood() ) :
					starvingIdx = 100
				else :
					starvingIdx = min([4*abs(pCity.foodDifference(True)),20])

				if( pCity.getEspionageHealthCounter() > 0 or pPlayer.isRebel() ) :
					starvingIdx = max([starvingIdx/5,min([starvingIdx,10])])

				localRevIdx += starvingIdx
				if( bIsRevWatch ) : negList.append( (starvingIdx, localText.getText("TXT_KEY_REV_WATCH_STARVATION",())) )
				revIdxHist['Health'][0] += starvingIdx

			# Disorder
			disorderIdx = 0
			if pCity.getOccupationTimer() > 0:
				if recentlyAcquired or pPlayer.isRebel():
					# Give recently acquired cities a break
					disorderIdx = 10
				elif pCity.getRevolutionCounter() > 0:
					disorderIdx = 15
				else:
					disorderIdx = 75
				localRevIdx += disorderIdx
				if bIsRevWatch:
					negList.append((disorderIdx, localText.getText("TXT_KEY_REV_WATCH_DISORDER",())))
			revIdxHist['Disorder'] = [disorderIdx] + revIdxHist['Disorder'][0:RevDefs.revIdxHistLen-1]

			# Civics
			[civicIdx, civicPosList, civicNegList] = RevUtils.getCivicsRevIdxLocal(iPlayer)
			localRevIdx += civicIdx

			posList.extend(civicPosList)
			negList.extend(civicNegList)

			# Traits
			[traitIdx, traitPosList, traitNegList] = RevUtils.getTraitsRevIdxLocal(iPlayer)
			localRevIdx += traitIdx

			posList.extend(traitPosList)
			negList.extend(traitNegList)

			# Buildings
			[buildingIdx, buildingPosList, buildingNegList] = RevUtils.getBuildingsRevIdxLocal(pCity)
			localRevIdx += buildingIdx

			posList.extend(buildingPosList)
			negList.extend(buildingNegList)

			#Afforess Revolution Difficulty Scaling
			iRevolutionHandicap = (GC.getHandicapInfo(pPlayer.getHandicapType()).getRevolutionIndexPercent() - 100) / 10
			localRevIdx += iRevolutionHandicap
			szText = GC.getHandicapInfo(pPlayer.getHandicapType()).getDescription() + localText.getText("TXT_KEY_DIFFICULTY_LEVEL",())
			if iRevolutionHandicap > 0:
				negList.append((iRevolutionHandicap, szText))
			elif iRevolutionHandicap < 0:
				posList.append((iRevolutionHandicap, szText))
			#Afforess Revolution Difficulty Scaling

			# Adjust index accumulation for varying game speeds
			gameSpeedMod = RevUtils.getGameSpeedMod()
			localRevIdx = int(gameSpeedMod * self.revIdxModifier * localRevIdx + self.revIdxOffset + .5)

			if pPlayer.isHuman():
				localRevIdx = int(self.humanIdxModifier*localRevIdx + self.humanIdxOffset + .5)

			revIdx = pCity.getRevolutionIndex()

			# Feedback on Rev Index
			if( localRevIdx < 0 and revIdx > self.alwaysViolentThreshold ) :
				# Very angry locals forgive very quickly if things begin improving
#-------------------------------------------------------------------------------------------------
# Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
				feedbackFactor = -min([(revIdx/170.0),10])
			elif( localRevIdx < 0 and revIdx > self.revInstigatorThreshold ) :
				# Angry locals forgive quickly if things are improving
				feedbackFactor = -min([(revIdx/230.0),8])
			else :
				feedbackFactor = -min([(revIdx/300.0),6])
#-------------------------------------------------------------------------------------------------
# END Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------

			# Update local RevIndex whenever called
			pCity.setLocalRevIndex( localRevIdx )

			if( not bIsRevWatch ) :
				# Change revolution indices based on local effects
#-------------------------------------------------------------------------------------------------
# Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
				pCity.changeRevolutionIndex( int(localRevIdx + feedbackFactor) )
#-------------------------------------------------------------------------------------------------
# END Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
				pCity.updateRevIndexAverage()
				RevData.updateCityVal(pCity, 'RevIdxHistory', revIdxHist )

			if( pCity.getRevolutionIndex() < 0 ) :
				pCity.setRevolutionIndex( 0 )
			elif( pCity.getRevolutionIndex() > (2*self.alwaysViolentThreshold) ) :
				pCity.setRevolutionIndex( (2*self.alwaysViolentThreshold) )

			revIdx = pCity.getRevolutionIndex()
			revIdxAvg = pCity.getRevIndexAverage()

			if( self.LOG_DEBUG and iGameTurn%25 == 0 ) : CvUtil.pyPrint("[REV] Revolt: %s:   Hap %d,   Loc %d,   Rel %d,   Nat %d,   Cult %d,   Gar %d"%(pCity.getName(),happyIdx,locationRevIdx,relIdx,natIdx,cultIdx,garIdx))
			if( self.LOG_DEBUG and iGameTurn%25 == 0 ) : CvUtil.pyPrint("[REV] Revolt:		 Local effects for %s (%s):  %d   (%d) fdbk with total  %d"%(pCity.getName(),pPlayer.getCivilizationDescription(0),localRevIdx,feedbackFactor,revIdx))

			# RevolutionDCM - city advisor text conditioning
			cityString = '\n\n' + pCity.getName()
			if self.showRevIndexInPopup or GAME.isDebugMode():
				cityString += " %d"%(revIdx)
			if revIdx >= self.alwaysViolentThreshold:
				cityString += ': ' + "<color=230,0,0,255>"   + localText.getText("TXT_KEY_REV_WATCH_DANGER",())  + "<color=255,255,255,255>"
			elif revIdx >= self.revInstigatorThreshold:
				cityString += ': ' + "<color=230,120,0,255>" + localText.getText("TXT_KEY_REV_WATCH_DANGER",())  + "<color=255,255,255,255>"
			elif revIdx >= int(self.revReadyFrac*self.revInstigatorThreshold):
				cityString += ': ' + "<color=245,245,0,255>" + localText.getText("TXT_KEY_REV_WATCH_WARNING",()) + "<color=255,255,255,255>"
			else:
				cityString += ':  ' + localText.getText("TXT_KEY_REV_WATCH_SAFE",()) + ' '
			#RevolutionDCM - city advisor text conditioning
			#cityString += '  ' + localText.getText("TXT_KEY_REV_WATCH_TREND",()) + ' '
			if( self.showRevIndexInPopup or GAME.isDebugMode() ) :
				cityString += "%d "%((revIdx - revIdxAvg))
			if( (revIdx - revIdxAvg) <= -self.showTrend ) :
				cityString += "<color=0,230,0,255>" + localText.getText("TXT_KEY_REV_WATCH_IMPROVING",()) + "<color=255,255,255,255>"
			elif( (revIdx - revIdxAvg) > self.showTrend ) :
				cityString += "<color=255,120,0,255>" + localText.getText("TXT_KEY_REV_WATCH_WORSENING",()) + "<color=255,255,255,255>"
			else :
				cityString += localText.getText("TXT_KEY_REV_WATCH_FLAT",())
			if( self.showRevIndexInPopup or GAME.isDebugMode() ) :
				cityString += "  %d, %d"%(pCity.getRevolutionCounter(),RevData.getCityVal(pCity,'WarningCounter'))

			# Enable only for debugging rev index histories
			if( False ) :
				cityString += '\n'
				for (key,value) in revIdxHist.iteritems() :
					cityString += key + ': ' + str(value) + ', '
			else :
				# Add in specfic strings in strength order
				posList.sort()
				cityString += '\n' + localText.getText("TXT_KEY_REV_WATCH_POSITIVE",())
				for [i,pos] in enumerate(posList) :
					if( i == 0 ) :
						cityString += '  '
					else :
						cityString += ', '

					if( pos[0] < -10 ) :
						cityString += "<color=255,255,255,255>"
					elif( pos[0] < -5 ) :
						cityString += "<color=210,210,210,255>"
					elif( pos[0] < -2 ) :
						cityString += "<color=170,170,170,255>"
					else :
						cityString += "<color=130,130,130,255>"
					cityString += pos[1]
					if( self.showRevIndexInPopup or GAME.isDebugMode() ) :
						cityString += " %d"%(pos[0])

				negList.sort()
				negList.reverse()
				cityString += "<color=255,255,255,255>" + '\n' + localText.getText("TXT_KEY_REV_WATCH_NEGATIVE",())
				for [i,neg] in enumerate(negList) :
					if( i == 0 ) :
						cityString += '  '
					else :
						cityString += ', '

					if( neg[0] > 20 ) :
						cityString += "<color=255,255,255,255>"
					elif( neg[0] > 10 ) :
						cityString += "<color=225,225,225,255>"
					elif( neg[0] > 5 ) :
						cityString += "<color=190,190,190,255>"
					elif( neg[0] > 2 ) :
						cityString += "<color=160,160,160,255>"
					else :
						cityString += "<color=130,130,130,255>"
					cityString += neg[1]
					if( self.showRevIndexInPopup or GAME.isDebugMode() ) :
						cityString += " %d"%(neg[0])

			cityString += "<color=255,255,255,255>"
			totalString += cityString

		return totalString

	def updateCivStability( self, iGameTurn, iPlayer, bIsRevWatch = False, bVerbose = False ) :
		# Update the revolution effects for the entire empire
		import BugUtil
		posList = []
		negList = []

		# Adjust index accumulation for varying game speeds
		gameSpeedMod = RevUtils.getGameSpeedMod()
		#print "Revolt - Game speed mod is ", self.gameSpeedMod

		pPlayer = GC.getPlayer(iPlayer)
		if( pPlayer.getNumCities() == 0 ) :
			return localText.getText("TXT_KEY_REV_WATCH_NO_CITIES",())

		pTeam = GC.getTeam( pPlayer.getTeam() )
		iEra = pPlayer.getCurrentEra()

		civRevIdx = 0
		revIdxHistEvents = 0
		civStabilityIdx = 0

		civSizeIdx = 0
		civSizeIdx2 = 0
		# Size of Empire
		# Note:  Empire size also effects modifier multiply location effects in local rev indices, so not all effects are here
		civSizeValue = self.civSizeModifier*RevUtils.computeCivSize( iPlayer )[0]
		if( civSizeValue > 2.0 ) :
			if( bIsRevWatch ) : negList.append( (-4, localText.getText("TXT_KEY_REV_WATCH_HUGE_EMP",())) )
			civSizeIdx += 4
			civSizeIdx2 -= 4
		elif( civSizeValue > 1.6 ) :
			if( bIsRevWatch ) : negList.append( (-3, localText.getText("TXT_KEY_REV_WATCH_LARGE_EMP",())) )
			civSizeIdx += 3
			civSizeIdx2 -= 3
		elif( civSizeValue > 1.4 ) :
			if( bIsRevWatch ) : negList.append( (-2, localText.getText("TXT_KEY_REV_WATCH_LARGE_EMP",())) )
			civSizeIdx += 2
			civSizeIdx2 -= 2
		elif( civSizeValue > 1.2 ) :
			civSizeIdx += 1
			civSizeIdx2 -= 1
		elif( civSizeValue > 1.0 ) :
			civSizeIdx += 0
			civSizeIdx2 += 0
		elif( civSizeValue > .7 ) :
			if( bIsRevWatch ) : posList.append( (1, localText.getText("TXT_KEY_REV_WATCH_MED_EMP",())) )
			civSizeIdx2 += 1
		else :
			if( bIsRevWatch ) : posList.append( (1, localText.getText("TXT_KEY_REV_WATCH_SMALL_EMP",())) )
			# No national flat benefit
			civSizeIdx2 += 1

		civRevIdx += civSizeIdx
		civStabilityIdx += civSizeIdx2
		BugUtil.debug("Empire Stability Marker 1, %d", civStabilityIdx)


		# Cultural spending
		cultPerc = pPlayer.getCommercePercent( CommerceTypes.COMMERCE_CULTURE )

		cultIdx = 0
		if( cultPerc > 0 ) :
			cultIdx += int(pow(cultPerc, 0.5))
			if( pPlayer.hasTrait(GC.getInfoTypeForString("TRAIT_CREATIVE")) ):
				cultIdx += cultIdx/3
			if( bIsRevWatch ) : posList.append( (cultIdx, localText.getText("TXT_KEY_REV_WATCH_CULTURE_SPENDING",())) )


		# Finances
		iGoldRate = pPlayer.calculateGoldRate()
		iGold = pPlayer.getGold()

		goldPerc = pPlayer.getCommercePercent( CommerceTypes.COMMERCE_GOLD )
		sciPerc = pPlayer.getCommercePercent( CommerceTypes.COMMERCE_RESEARCH )

		# This is calculation AI does to figure out if it's in financial trouble
		iNetCommerce = 1 + pPlayer.getCommerceRate(CommerceTypes.COMMERCE_GOLD) + pPlayer.getCommerceRate(CommerceTypes.COMMERCE_RESEARCH) + max([0, pPlayer.getGoldPerTurn()])
		iNetExpenses = pPlayer.getFinalExpense() + max([0, -pPlayer.getGoldPerTurn()])
		iFundedPercent = (100 * (iNetCommerce - iNetExpenses)) / max([1, iNetCommerce])

		iThresholdPercent = 50 # Afforess - lowered from 60 to 50 to match DLL default threshold

		#Force Winning Players to Earn More
		iPlayerRank = GAME.getPlayerRank(pPlayer.getID())
		if (iPlayerRank < 3):
			iThresholdPercent += ((4 - iPlayerRank) * 5)

		iNumWars = pTeam.getAtWarCount(True)
		if( iNumWars > 0 ) :
			iThresholdPercent -= (10 + 2*min([iNumWars, 5])) # Afforess - must be inclosed in outer paren, fixes bug

		if( pPlayer.isCurrentResearchRepeat() ) :
			# Have all techs, research no longer important
			iThresholdPercent *= 2
			iThresholdPercent /= 3
		if( cultPerc > 70 ) :
			# Going for cultural victory, research no longer important
			iThresholdPercent *= 2
			iThresholdPercent /= 3

		if( pPlayer.hasTrait(GC.getInfoTypeForString("TRAIT_ORGANIZED")) ):
			iThresholdPercent *= 10
			iThresholdPercent /= 11

		if( self.LOG_DEBUG and (bVerbose or iGameTurn%25 == 0) ) : CvUtil.pyPrint("[REV] Revolt: The %s financial status: commerce %d, expenses %d, funded %d, thresh = %d, num wars %d,   gold %d, rate %d, gold perc %.2f"%(pPlayer.getCivilizationDescription(0), iNetCommerce, iNetExpenses, iFundedPercent, iThresholdPercent, iNumWars, iGold, iGoldRate,goldPerc))

		taxesIdx = 0
		finIdx = 0
		if( iNetCommerce > 20 and iFundedPercent > iThresholdPercent + 25 and not pPlayer.isMinorCiv() and not pPlayer.isAnarchy() ) :
			# Great long-term financial situation
			finIdx = min([((iFundedPercent - (iThresholdPercent+25))/4 + cultPerc/5),4 + iEra])
			if( bIsRevWatch ) :
				posList.append( (finIdx, localText.getText("TXT_KEY_REV_WATCH_FINANCIAL_POS",())) )
			if( self.LOG_DEBUG and (bVerbose or iGameTurn%25 == 0) ) : CvUtil.pyPrint("[REV] Revolt: The %s is in a good long-term financial situation")

		elif( iFundedPercent < iThresholdPercent ) :
			iPercentShort = iThresholdPercent - iFundedPercent
			finIdx = -min([20, 1+ iPercentShort/3])

			if( iGoldRate > 2 + 5*iEra ) :
				# Civ is making money
				finIdx /=2
			elif( iGold > 100 + 100*iEra ) :
				# Civ has plenty of reserves
				finIdx /=2
			elif( goldPerc > 50 and iGold < (35 + 10*iEra) and iGoldRate < 0 ) :
				# Civ in danger of running out of gold soon
				finIdx *=2
				if( self.LOG_DEBUG and (bVerbose or iGameTurn%25 == 0) ) : CvUtil.pyPrint("[REV] Revolt: The %s is in serious financial trouble")

			if( bIsRevWatch ) :
				if(finIdx < 0):
					negList.append( (finIdx, localText.getText("TXT_KEY_REV_WATCH_FINANCIAL_NEG",())) )

			revIdxHistEvents += finIdx

		civRevIdx -= finIdx
		civStabilityIdx += finIdx
		BugUtil.debug("Empire Stability Marker 2, %d", civStabilityIdx)
		# Civics
		[civicIdx,civicPosList,civicNegList] = RevUtils.getCivicsCivStabilityIndex( iPlayer )
		civStabilityIdx += civicIdx
		BugUtil.debug("Empire Stability Marker 3, %d", civStabilityIdx)
		posList.extend( civicPosList )
		negList.extend( civicNegList )

		# Traits
		[traitIdx,traitPosList,traitNegList] = RevUtils.getTraitsCivStabilityIndex( iPlayer )
		civStabilityIdx += -traitIdx
		BugUtil.debug("Empire Stability Marker 4, %d", civStabilityIdx)
		posList.extend( traitPosList )
		negList.extend( traitNegList )

		# Buildings
		[buildingIdx,buildingPosList,buildingNegList] = RevUtils.getBuildingsCivStabilityIndex( iPlayer )
		civStabilityIdx += -buildingIdx
		BugUtil.debug("Empire Stability Marker 5, %d", civStabilityIdx)
		posList.extend( buildingPosList )
		negList.extend( buildingNegList )

		# Helps catch tiny civs in late game
		if( pPlayer.getNumMilitaryUnits() > pPlayer.getTotalPopulation() and pPlayer.getTotalPopulation()/pPlayer.getNumCities() < 4 ) :
			if( (GAME.getCurrentEra() - GAME.getStartEra()) > 2 ) :
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt:	The %s have way more troops than citizens"%(pPlayer.getCivilizationDescription(0)))
				milIdx = int( (4*pPlayer.getNumMilitaryUnits())/pPlayer.getTotalPopulation() )
				if( bIsRevWatch ) : negList.append( (-milIdx, localText.getText("TXT_KEY_REV_WATCH_MILITARY",())) )
				civRevIdx += milIdx
				civStabilityIdx -= milIdx
		BugUtil.debug("Empire Stability Marker 6, %d", civStabilityIdx)
		# Any civic or religion changes cause anarchy for non-spiritual civs
		# TODO: single turn anarchy doesn't show up at beginning or end of players turn
		if( pPlayer.isAnarchy() ) :
			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: The %s is in anarchy!"%(pPlayer.getCivilizationDescription(0)))
			if( bIsRevWatch ) : negList.append( (-100, localText.getText("TXT_KEY_REV_WATCH_ANARCHY",())) )
			if( pPlayer.isRebel() ) :
				civRevIdx += 40
				civStabilityIdx -= 25
			else :
				civRevIdx += 100
				civStabilityIdx -= 70
			revIdxHistEvents += 100
		BugUtil.debug("Empire Stability Marker 7, %d", civStabilityIdx)
		if( pPlayer.isGoldenAge() ) :
			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: The %s are in their golden age!"%(pPlayer.getCivilizationDescription(0)))
			if( bIsRevWatch ) : posList.append( (10, localText.getText("TXT_KEY_REV_WATCH_GOLDEN_AGE",())) )
			civRevIdx -= 20
			civStabilityIdx += 10
			revIdxHistEvents -= 30
		BugUtil.debug("Empire Stability Marker 8, %d", civStabilityIdx)

		civRevIdx = int(gameSpeedMod*self.revIdxModifier*civRevIdx + .5)
		civStabilityIdx = int(gameSpeedMod*self.revIdxModifier*civStabilityIdx + .5)
		if( pPlayer.isHuman() ) :
			civRevIdx = int(self.humanIdxModifier*civRevIdx + .5)
			civStabilityIdx = int(self.humanIdxModifier*civStabilityIdx + .5)
		BugUtil.debug("Empire Stability Marker 10, %d", civStabilityIdx)
		if( self.LOG_DEBUG and iGameTurn%25 == 0 ) : CvUtil.pyPrint("[REV] Revolt: %s stability effects: %d, size: %d, civics: %d, cult: %d, taxes: %d, finances: %d\n"%(pPlayer.getCivilizationDescription(0),civStabilityIdx,civSizeIdx2,civicIdx,cultIdx,taxesIdx,finIdx))

		if not bIsRevWatch:
			for cityX in pPlayer.cities():
				cityX.changeRevolutionIndex(int(civRevIdx))
				revIdxHist = RevData.getCityVal(cityX, 'RevIdxHistory')
				revIdxHist['Events'][0] += revIdxHistEvents
				RevData.updateCityVal(cityX, 'RevIdxHistory', revIdxHist)

		centerVal = 500
		feedback = (centerVal - pPlayer.getStabilityIndex())/20

		civStabilityIdx += feedback
		BugUtil.debug("Empire Stability Marker 11, %d", civStabilityIdx)
		if( not bIsRevWatch ) :
			pPlayer.changeStabilityIndex(civStabilityIdx)
			pPlayer.updateStabilityIndexAverage()


		iStablity = pPlayer.getStabilityIndex()
		civString = localText.getText("TXT_KEY_REV_WATCH_CIV_STABILITY",()) + ' '
		if( iStablity > 750 ) :
			civString += localText.getText("TXT_KEY_REV_WATCH_VERY_STABLE",())
		elif( iStablity > 570 ) :
			civString += localText.getText("TXT_KEY_REV_WATCH_STABLE",())
		elif( iStablity > 430 ) :
			civString += localText.getText("TXT_KEY_REV_WATCH_NEUTRAL",())
		elif( iStablity > 250 ) :
			civString += localText.getText("TXT_KEY_REV_WATCH_UNSTABLE",())
		else :
			civString += localText.getText("TXT_KEY_REV_WATCH_DANGEROUSLY_UNSTABLE",())

		if( self.showRevIndexInPopup or GAME.isDebugMode() ) :
			civString += " %d  Net: %d"%(iStablity,civStabilityIdx)
		#RevolutionDCM - text conditioning
		civString += "  "
		#civString += "  " + localText.getText("TXT_KEY_REV_WATCH_TREND",()) + " "
		if( (iStablity - pPlayer.getStabilityIndexAverage()) > self.showStabilityTrend ) :
			civString += "<color=0,230,0,255>" + localText.getText("TXT_KEY_REV_WATCH_IMPROVING",()) + "<color=255,255,255,255>"
		elif( (iStablity - pPlayer.getStabilityIndexAverage()) <= -self.showStabilityTrend ) :
			civString += "<color=255,120,0,255>" + localText.getText("TXT_KEY_REV_WATCH_WORSENING",()) + "<color=255,255,255,255>"
		else :
			civString += localText.getText("TXT_KEY_REV_WATCH_FLAT",())

		if( self.showRevIndexInPopup or GAME.isDebugMode() ) :
			civString += " %d"%(iStablity - pPlayer.getStabilityIndexAverage())

		civString += '\n<color=0,230,0,255>' + localText.getText("TXT_KEY_REV_WATCH_POSITIVE",())
		posList.sort()
		posList.reverse()
		for [i,pos] in enumerate(posList) :
			if( i == 0 ) :
				civString += '  '
			else :
				civString += ', '

			if( pos[0] > 10 ) :
				civString += "<color=255,255,255,255>"
			elif( pos[0] > 5 ) :
				civString += "<color=200,200,200,255>"
			elif( pos[0] > 2 ) :
				civString += "<color=150,150,150,255>"
			else :
				civString += "<color=100,100,100,255>"
			civString += pos[1]
			if( self.showRevIndexInPopup or GAME.isDebugMode() ) :
				civString += " %d"%(pos[0])

		civString += '\n<color=255,0,0,255>' + localText.getText("TXT_KEY_REV_WATCH_NEGATIVE",())
		negList.sort()
		for [i,neg] in enumerate(negList) :
			if( i == 0 ) :
				civString += '  '
			else :
				civString += ', '

			if( neg[0] < -20 ) :
				civString += "<color=255,255,255,255>"
			elif( neg[0] < -10 ) :
				civString += "<color=210,210,210,255>"
			elif( neg[0] < -5 ) :
				civString += "<color=175,175,175,255>"
			elif( neg[0] < -2 ) :
				civString += "<color=140,140,140,255>"
			else :
				civString += "<color=100,100,100,255>"
			civString += neg[1]
			if( self.showRevIndexInPopup or GAME.isDebugMode() ) :
				civString += " %d"%(neg[0])

		civString += "<color=255,255,255,255>"

		return civString

	def checkForBribes(self, iGameTurn, iPlayer):

		player = GC.getPlayer(iPlayer)
		if player.isHuman() or player.isNPC():
			return

		iGold = player.getGold()

		if iGold < 100 or player.isAnarchy():
			return

		for cityX in player.cities():

			bCanBribeCity, _ = RevUtils.isCanBribeCity(cityX)
			if not bCanBribeCity:
				continue

			bribeTurn = RevData.getCityVal( cityX, 'BribeTurn' )

			if not bribeTurn is None and GAME.getGameTurn() - bribeTurn < 20:
				continue # Costs will be highly elevated from recent bribe

			[iSmall,iMed,iLarge] = RevUtils.computeBribeCosts(cityX)

			if iSmall > iGold:
				continue

			revIdx = cityX.getRevolutionIndex()
			localRevIdx = cityX.getLocalRevIndex()

			if localRevIdx > 2*self.badLocalThreshold:
				# Consider small bribe to buy time
				iOdds = min([2*iGold/iSmall, 8])
				if iOdds > GAME.getSorenRandNum(100,'Rev: bribe city'):
					if self.LOG_DEBUG:
						print "[REV] %s bribing %s with small bribe to buy time, odds %d"%(player.getCivilizationDescription(0),cityX.getName(),iOdds)
					RevUtils.bribeCity(cityX, 'Small')
					iGold = player.getGold()
					if iGold < 100:
						return
					continue

			elif localRevIdx < -self.badLocalThreshold/2:
				# Situation should be improving on its own, perhaps a bribe could make city not want to join a revolt
				if revIdx < self.revInstigatorThreshold and iMed < iGold - 30:
					iOdds = min([2*iGold/iMed, 10])
					if iOdds > GAME.getSorenRandNum(100,'Rev: bribe city'):
						if self.LOG_DEBUG:
							print "[REV] %s bribing %s with med bribe so that it won't join a revolt, odds %d"%(player.getCivilizationDescription(0), cityX.getName(), iOdds)
						RevUtils.bribeCity(cityX, 'Med')
						iGold = player.getGold()
						if iGold < 100:
							return
						continue

			if revIdx > 0.8*self.revInstigatorThreshold and iLarge < iGold - 45:
				iOdds = min([2*iGold/iLarge - localRevIdx, 5])
				if iOdds > GAME.getSorenRandNum(100,'Rev: bribe city'):
					if self.LOG_DEBUG:
						print "[REV] %s bribing %s with large bribe so that it won't join a revolt, odds %d"%(player.getCivilizationDescription(0), cityX.getName(), iOdds)
					RevUtils.bribeCity(cityX, 'Large')
					iGold = player.getGold()
					if iGold < 100:
						return
					continue

	def checkForRevolution(self, iGameTurn, iPlayer):

		if iPlayer > 39:
			raise "NPC does not revolt!"

		pPlayer = GC.getPlayer(iPlayer)
		if pPlayer.getNumCities() == 0:
			return

		revReadyCities = []
		revInstigatorCities = []
		warnCities = []

		for pCity in pPlayer.cities():

			revIdx = pCity.getRevolutionIndex()
			prevRevIdx = RevData.getCityVal(pCity, 'PrevRevIndex')
			localRevIdx = pCity.getLocalRevIndex()

			numUnhappy = RevUtils.getModNumUnhappy( pCity, self.warWearinessMod )
			if numUnhappy > 0:
				cityThreshold = max([int( self.revInstigatorThreshold - 2.5*self.revInstigatorThreshold*numUnhappy/pCity.getPopulation() ),int(self.revInstigatorThreshold/6.0)])
			elif localRevIdx > 60 and pCity.getPopulation() < pCity.getHighestPopulation() - 1:
				cityThreshold = max([int( self.revInstigatorThreshold*50/(1.0*localRevIdx)), int(self.revInstigatorThreshold/2.0)])
			else:
				cityThreshold = self.revInstigatorThreshold

			if revIdx >= int(self.warnFrac * cityThreshold) and pCity.getRevolutionCounter() == 0:
				if  RevData.getCityVal(pCity, 'WarningCounter') == 0:
					# Warn human of impending revolution (note can't instigate on warning turn)
					if self.LOG_DEBUG:
						CvUtil.pyPrint("  REVOLT - %s (%s) is over %d warning threshold in year %d!!!!"%(pCity.getName(),pPlayer.getCivilizationDescription(0),int( self.warnFrac*cityThreshold ),GAME.getGameTurnYear()))
					warnCities.append(pCity)
				elif revIdx > cityThreshold and prevRevIdx > cityThreshold:
					# City meets instigator criteria
					revInstigatorCities.append(pCity)

			if (pCity.getRevolutionCounter() == 0
			and revIdx > cityThreshold * self.revReadyDividend / self.revReadyDivisor
			and prevRevIdx > cityThreshold * self.revReadyDividend / self.revReadyDivisor
			):
				revReadyCities.append(pCity) # City meets revolution ready criteria

			RevData.updateCityVal(pCity, 'PrevRevIndex', revIdx)

		instigator = None

		# TODO: turn this and instigator bit above into a function, when revolt odds > 0 then is an instigator
		if( len(revInstigatorCities) > 0 ) :
			for pCity in revInstigatorCities :
				revIdx = pCity.getRevolutionIndex()
				localRevIdx = pCity.getLocalRevIndex()
				revIdxHist = RevData.getCityVal(pCity,'RevIdxHistory')

				gsm = RevUtils.getGameSpeedMod()

				odds = (1000.0*revIdx)/(self.iRevRoll)
				factors = ""

				if( localRevIdx < 0 ) :
					if( localRevIdx < -gsm*self.badLocalThreshold ) :
						odds -= 1.5*localRevIdx + 30
						if( odds > 0 ) :
							odds = odds/4
						factors += 'Quickly imp local, '
					else :
						odds -= 1.5*localRevIdx
						odds = odds/2
						factors += 'Imp local, '

				elif( localRevIdx > gsm*self.badLocalThreshold ) :

					odds *= 2.0
					factors += 'Bad local, '

					avgHappiness = 0
					for happi in revIdxHist['Happiness'] :
						avgHappiness += happi
					avgHappiness /= len(revIdxHist['Happiness'])

					if( revIdxHist['Health'][0] > 20 ) :
						odds = max([250.0,odds*2.5])
						factors += 'Starvation, '
					elif( revIdxHist['Disorder'][0] > 20 ) :
						odds = max([180.0,odds*1.8])
						factors += 'Disorder, '
					elif( avgHappiness > 12 ) :
						odds = max([120.0,odds*1.5])
						factors += 'Unhappy, '
					elif( localRevIdx > gsm*max([int(2.5*self.badLocalThreshold),12]) ) :
						odds *= 1.5
						factors += 'Quickly worsening local, '

					if( revIdxHist['Garrison'] < -5 ) :
						odds *= 0.6
						odds = min([odds,150.0])
						factors += 'Very strong gar, '
					elif( revIdxHist['Garrison'] < -3 ) :
						odds *= 0.8
						odds = min([odds,200.0])
						factors += 'Strong gar, '

				odds = int( self.chanceModifier*odds )

				eventSum = 0
				for event in revIdxHist['Events'] :
					eventSum += event

				if( eventSum < -50 ) :
					odds *= 0.75
					factors += 'Very pos events, '
				elif( eventSum < -20 ) :
					odds *= 0.9
					factors += 'Pos events, '
				elif( eventSum > 300 ) :
					if( revIdxHist['Events'][0] > 330 ) :
						odds = min([ odds*4.0, max([odds,300]) ])
						factors += 'Lost capital, '
					else :
						odds *= 2.0
						factors += 'Ext neg events, '
				elif( eventSum > 120 ) :
					odds *= 1.2
					factors += 'Very Neg events, '
				elif( eventSum > 50 ) :
					odds *= 1.1
					factors += 'Neg events, '

				# Do revolution?
				odds = min([odds, 500])

				if( odds > GAME.getSorenRandNum( 1000, 'Revolt - do revolution?' ) ) :
					if( self.LOG_DEBUG ) :
						CvUtil.pyPrint("  REVOLT - %s (%s) has decided to launch a revolution with index %d (%d local) and odds %.1f in year %d!!!!"%(pCity.getName(),pPlayer.getCivilizationDescription(0),revIdx,localRevIdx,odds/10.0,GAME.getGameTurnYear()))
						CvUtil.pyPrint("  REVOLT - Factors effecting timing: %s"%(factors))
					instigator = pCity
					break

		if not instigator == None:
			self.pickRevolutionStyle( pPlayer, instigator, revReadyCities )

		elif warnCities:
			for pCity in warnCities:
				RevData.updateCityVal( pCity, 'WarningCounter', self.warnTurns )

			if self.isLocalHumanPlayer(pPlayer.getID()):
				pTeam = GC.getTeam(pPlayer.getTeam())
				# Additions by Caesium et al
				caesiumtR = CyUserProfile().getResolutionString(CyUserProfile().getResolution())
				caesiumtextResolution = caesiumtR.split('x')
				caesiumpasx = int(caesiumtextResolution[0])/10
				caesiumpasy = int(caesiumtextResolution[1])/10
				popup = CyPopup(-1, EventContextTypes.NO_EVENTCONTEXT, False)
				if self.centerPopups:
					popup.setPosition(3*caesiumpasx,3*caesiumpasy)
				# End additions by Caesium et al
				popup.setHeaderString( localText.getText("TXT_KEY_REV_WARN_TITLE",()), 1<<2)
				bodStr = (
					localText.getText("TXT_KEY_REV_WARN_NEWS",())
					+ ' ' + getCityTextList(warnCities)
					+ ' ' + localText.getText("TXT_KEY_REV_WARN_CONTEMPLATING",())
					+ self.updateLocalRevIndices(GAME.getGameTurn(), pPlayer.getID(), subCityList = warnCities, bIsRevWatch = True)
					+ '\n\n' + localText.getText("TXT_KEY_REV_WARN_CIV_WIDE",()) + '\n'
					+ self.updateCivStability(GAME.getGameTurn(), pPlayer.getID(), bIsRevWatch = True) + '\n\n'
				)
				if pTeam.getAtWarCount(True) > 0:
					bodStr += localText.getText("TXT_KEY_REV_WARN_WARS",())
				else:
					bodStr += localText.getText("TXT_KEY_REV_WARN_GLORY",()) + '  '
					if pPlayer.getCitiesLost() > 0:
						bodStr += localText.getText("TXT_KEY_REV_WARN_LOST",())
					bodStr += localText.getText("TXT_KEY_REV_WARN_TEMPORARY",())
				popup.setBodyString(bodStr, 1<<0)

				# Center camera on city
				CyCamera().JustLookAt( warnCities[0].plot().getPoint() )

				popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)


	def incrementRevIdxHistory(self, iGameTurn, iPlayer):
		# Increment RevIdxHistory fields that are not handled by updateLocalRevIndices

		for city in GC.getPlayer(iPlayer).cities():

			revIdxHist = RevData.getCityVal(city, 'RevIdxHistory')

			# Bump turn for fields that are not handled by updateLocalRevIndices
			revIdxHist['RevoltEffects'] = [0] + revIdxHist['RevoltEffects'][0:RevDefs.revIdxHistLen-1]
			revIdxHist['Events'] = [0] + revIdxHist['Events'][0:RevDefs.revIdxHistLen-1]

			RevData.updateCityVal(city, 'RevIdxHistory', revIdxHist)


##--- Game turn functions  ---------------------------------------------------

	def topCivAdjustments( self ) :
		# Penalty on top score/power to help keep game even
		# Benefit for highest culture

		powerList = []
		cultureList = []
		scoreList = []

		for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
			pPlayer = GC.getPlayer(iPlayer)
			if (pPlayer.isAlive()):
				powerList.append((pPlayer.getPower(),iPlayer))
				cultureList.append((pPlayer.getCulture(), iPlayer))
				scoreList.append((GAME.getPlayerScore(iPlayer), iPlayer))


		powerList.sort()
		powerList.reverse()
		cultureList.sort()
		cultureList.reverse()
		scoreList.sort()
		scoreList.reverse()

		iNumTopPlayers = (GAME.countCivPlayersAlive() - 4)/3
		if( self.LOG_DEBUG and GAME.getGameTurn()%25 == 0 ) : CvUtil.pyPrint("[REV] Revolt: Adjustments for top %d players"%(iNumTopPlayers))

		for [iRank,listElement] in enumerate(powerList[0:iNumTopPlayers]) :
			[iPower,iPlayer] = listElement
			if( (3*iPower)/2 > powerList[0][0] ) :
				iPowerEffect = 3 - (3*iRank)/iNumTopPlayers
				if( self.LOG_DEBUG and GAME.getGameTurn()%25 == 0 ) : CvUtil.pyPrint("[REV] Revolt: %s have %dth most power, effect: %d"%(GC.getPlayer(iPlayer).getCivilizationDescription(0),iRank+1,-iPowerEffect))
				GC.getPlayer(iPlayer).changeStabilityIndex(-iPowerEffect)

		for [iRank,listElement] in enumerate(cultureList[0:iNumTopPlayers]) :
			[iCulture,iPlayer] = listElement
			if( (3*iCulture)/2 > cultureList[0][0] ) :
				iCultureEffect = 3 - (3*iRank)/iNumTopPlayers
				if( self.LOG_DEBUG and GAME.getGameTurn()%25 == 0 ) : CvUtil.pyPrint("[REV] Revolt: %s have %dth most culture, effect: %d"%(GC.getPlayer(iPlayer).getCivilizationDescription(0),iRank+1,iCultureEffect))
				GC.getPlayer(iPlayer).changeStabilityIndex(iCultureEffect)

		for [iRank,listElement] in enumerate(scoreList[0:iNumTopPlayers]) :
			[iScore,iPlayer] = listElement
			if( (3*iScore)/2 > scoreList[0][0] ) :
				iScoreEffect = 3 - (3*iRank)/iNumTopPlayers
				if( self.LOG_DEBUG and GAME.getGameTurn()%25 == 0 ) : CvUtil.pyPrint("[REV] Revolt: %s have %dth highest score, effect: %d"%(GC.getPlayer(iPlayer).getCivilizationDescription(0),iRank+1,-iScoreEffect))
				GC.getPlayer(iPlayer).changeStabilityIndex(-iScoreEffect)


#-------------------------------------------------------------------------------------------------
# Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
	def revIndexAdjusted( self, pCity):

		if(pCity.isCapital()):
			return pCity.getRevolutionIndex()-self.revInstigatorThreshold
		else:
			return pCity.getRevolutionIndex()

##--- Revolution setup functions ---------------------------------------------
#-------------------------------------------------------------------------------------------------
# END Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------

	def pickRevolutionStyle( self, pPlayer, instigator, revReadyCities ) :
		bReinstatedOnRevolution = False
#-------------------------------------------------------------------------------------------------
# Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
	   # MOVED ELSEWHERE
#-------------------------------------------------------------------------------------------------
# END Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------

		iPlayer = pPlayer.getID()
		pTeam = GC.getTeam( pPlayer.getTeam() )

		revCities = []
		revCities.append(instigator)
		revInCapital = instigator.isCapital()

		# Who will join them?  City must be either in area with instigator, or close to instigator but not in homeland
		# City must also be able to revolt now (not recently revolted)
		for pCity in revReadyCities :
			if( pCity.getRevolutionCounter() == 0 and not pCity.getID() == instigator.getID() ) :

				if( pCity.area().getID() == instigator.area().getID() ) :
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s is in the area, joining revolution"%(pCity.getName()))
					revCities.append(pCity)
					revInCapital = (revInCapital or pCity.isCapital())

				elif( plotDistance( pCity.getX(), pCity.getY(), instigator.getX(), instigator.getY() ) <= self.closeRadius and not pCity.area().getID() == pPlayer.getCapitalCity().area().getID() ) :
					# Catch cities on small island chains ... not in same area, but close and not in homeland
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s is nearby, joining revolution"%(pCity.getName()))
					revCities.append(pCity)
					revInCapital = (revInCapital or pCity.isCapital())

		# Peaceful or violent?
		bPeaceful = True
		instRevIdx = instigator.getRevolutionIndex()
		instLocalIdx = instigator.getLocalRevIndex()

		if( instRevIdx > self.alwaysViolentThreshold ) :
			# Situation really bad
			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Violent, above always violent threshold")
			bPeaceful = False
		elif( instigator.getNumRevolts(iPlayer) == 0 ) :
			bPeaceful = True
		else :
			if( bPeaceful ) :
				modNumUnhappy = RevUtils.getModNumUnhappy( instigator, self.warWearinessMod )
				if( int(200*modNumUnhappy/instigator.getPopulation()) > GAME.getSorenRandNum( 100, 'Rev' ) ) :
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Violent due to Unhappiness")
					bPeaceful = False
			if( bPeaceful and instLocalIdx > self.badLocalThreshold ) :
				# Situation deteriorating rapidly
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Violent due to rapidly deteriorating situation")
				bPeaceful = False
			if( bPeaceful and len(revCities) == 1 ) :
				bPeaceful = True
			elif( bPeaceful ) :
				lowerThresh = int(.8*self.alwaysViolentThreshold)

				civicsMod = RevUtils.getCivicsViolentRevMod(iPlayer)
				lowerThresh += int(civicsMod*self.alwaysViolentThreshold  + .5)

				if instLocalIdx > self.badLocalThreshold:
					lowerThresh -= int(.05*self.alwaysViolentThreshold + .5)
				elif instLocalIdx < 0:
					lowerThresh += int(.10*self.alwaysViolentThreshold + .5)

				if( instRevIdx > lowerThresh ) :
					odds = (100*(instRevIdx - lowerThresh))/(self.alwaysViolentThreshold-lowerThresh)
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Odds for violence are %d"%(odds))
					if( odds > GAME.getSorenRandNum( 100, 'Rev' ) ) :
						bPeaceful = False


		if( bPeaceful ) :
			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Peaceful revolution")

#-------- Check for still existing violent revolution for instigator
		if( not bPeaceful and pPlayer.getNumCities() > 1 and not RevData.getCityVal(instigator, 'RevolutionTurn') == None ) :
			revCivType = RevData.getCityVal(instigator, 'RevolutionCiv')
			pRevPlayer = None
			if( revCivType >= 0 ) :
				for i in xrange(GC.getMAX_PC_PLAYERS()) :
					if( not i == pPlayer.getID() ) :
						playerI = GC.getPlayer( i )
						if( playerI.isAlive() and playerI.getCivilizationType() == revCivType ) :
							if( playerI.isRebel() and GC.getTeam(playerI.getTeam()).isRebelAgainst(pTeam.getID()) ) :
								if( pTeam.isAtWar(playerI.getTeam()) ) :
									pRevPlayer = playerI
									break

			if( not pRevPlayer == None ) :

				bCanJoin = True

				# TODO: Create popup offering peace to human rebel player in this circumstance?
				if( pRevPlayer.isHuman() ) :
					bCanJoin = False

				if( bCanJoin and pTeam.isAVassal() ) :
					for teamID in xrange(GC.getMAX_PC_TEAMS()) :
						if( pTeam.isVassal(teamID) and GC.getTeam(teamID).isHuman() ) :
							bCanJoin = False
							break

				if( bCanJoin and pRevPlayer.isAlive() and GC.getTeam(pRevPlayer.getTeam()).isAVassal() ) :
					for teamID in xrange(GC.getMAX_PC_TEAMS()) :
						if( GC.getTeam(pRevPlayer.getTeam()).isVassal(teamID) and GC.getTeam(teamID).isHuman() ) :
							bCanJoin = False
							break

				if bCanJoin:

					bJoin = False
					citiesInRevolt = []
					for pCity in pPlayer.cities():

						if (RevData.getCityVal(pCity, 'RevolutionCiv') == revCivType
						and pCity.getReinforcementCounter() > 0
						and pCity.getReinforcementCounter() < 9 - pRevPlayer.getCurrentEra() / 2
						):
							if self.LOG_DEBUG:
								bInRev = False
								for pRevCity in revCities:
									if pCity.getID() == pRevCity.getID():
										bInRev = True
										break
								if bInRev:
									print "[REV] %s actively revolting" % pCity.getName()
								else:
									print "[REV] Unlisted %s also actively revolting" % pCity.getName()
							citiesInRevolt.append(pCity)

					if( GAME.getGameTurn() - RevData.getCityVal(instigator, 'RevolutionTurn') < 3*self.turnsBetweenRevs ) :
						# Recent revolt
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Joining recent revolt")
						bJoin = True
					elif( len(citiesInRevolt) > 0 ) :
						# Continuing revolt
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Joining still active revolt")
						bJoin = True

					if( bJoin ) :
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Cities revolt civ type %s is fighting owner"%(pRevPlayer.getCivilizationDescription(0)))
						if( pRevPlayer.isRebel() and not (pRevPlayer.isMinorCiv() or pPlayer.isMinorCiv()) ) :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Joining existing revolution with %s"%(pRevPlayer.getCivilizationDescription(0)))

							joinRevCities = []
							for pCity in revCities :
								cityDist = plotDistance( pCity.getX(), pCity.getY(), instigator.getX(), instigator.getY() )
								if( RevData.getCityVal(pCity, 'RevolutionCiv') == revCivType ) :
									joinRevCities.append(pCity)
									if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s has same rev type"%(pCity.getName()))
								elif( pCity.getRevolutionIndex() > self.revInstigatorThreshold and cityDist <= 0.8*self.closeRadius ) :
									joinRevCities.append(pCity)
									if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s is close and over threshold, joining"%(pCity.getName()))

							# Create list of cities to handover to end revolt
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Creating list of cities to request to be handed over")
							handoverCities = []
							toSort = []
							for pCity in citiesInRevolt :
								revIdx = pCity.getRevolutionIndex()
								if( pCity.isCapital() ) :
									if( revIdx > self.alwaysViolentThreshold and pCity.getLocalRevIndex() > 0 ) :
										if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s (capital), %d qualifies as revolting city"%(pCity.getName(),revIdx))
										handoverCities.append( pCity )
#-------------------------------------------------------------------------------------------------
# Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
										toSort.append(pCity)
#-------------------------------------------------------------------------------------------------
# END Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
								else :
									if( revIdx > self.alwaysViolentThreshold or (revIdx > self.revInstigatorThreshold and pCity.getLocalRevIndex() > -self.badLocalThreshold/2) ) :
										if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s, %d qualifies as revolting city"%(pCity.getName(),revIdx))
										handoverCities.append( pCity )
#-------------------------------------------------------------------------------------------------
# Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
										toSort.append(pCity)
#-------------------------------------------------------------------------------------------------
# END Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
							for pCity in joinRevCities :
								bInList = False
								for handoverCity in handoverCities :
									if( pCity.getID() == handoverCity.getID() ) :
										bInList = True
										break

								if( not bInList ) :
									revIdx = pCity.getRevolutionIndex()
									if( pCity.isCapital() ) :
										if( revIdx > self.alwaysViolentThreshold and pCity.getLocalRevIndex() > 0 ) :
											if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s (capital), %d qualifies as joining city"%(pCity.getName(),revIdx))
											handoverCities.append( pCity )
#-------------------------------------------------------------------------------------------------
# Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
											toSort.append(pCity)
#-------------------------------------------------------------------------------------------------
# END Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
									else :
										if( revIdx > self.alwaysViolentThreshold or (revIdx > self.revInstigatorThreshold and pCity.getLocalRevIndex() > -self.badLocalThreshold/2) ) :
											if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s, %d qualifies as joining city"%(pCity.getName(),revIdx))
											handoverCities.append( pCity )
#-------------------------------------------------------------------------------------------------
# Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
											toSort.append(pCity)
#-------------------------------------------------------------------------------------------------
# END Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------------
# Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
							toSort.sort(key=lambda i: (self.revIndexAdjusted(i), i.getName()))
#-------------------------------------------------------------------------------------------------
# END Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
							toSort.reverse()

							# Make order list of cities to request to be handed over
							handoverCities = []
#-------------------------------------------------------------------------------------------------
# Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
							for pCity in toSort :
#-------------------------------------------------------------------------------------------------
# END Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
								handoverCities.append( pCity )

							# Limit ambitions to something player could conceivably accept ...
							maxHandoverCount = (3*pPlayer.getNumCities())/4
							handoverCities = handoverCities[0:maxHandoverCount]

							# If asking for capital, put it first in list
							capID = pPlayer.getCapitalCity().getID()
							for [i,pCity] in enumerate(handoverCities) :
								if( capID == pCity.getID() ) :
									handoverCities.pop(i)
									handoverCities = [pCity] + handoverCities
									break

							if handoverCities:

								if self.LOG_DEBUG:
									str = "[REV] Revolt: Offering peace in exchange for handover of: "
									for pCity in handoverCities:
										str += "%s, "%pCity.getName()
									print str

								# Determine strength of rebellion
								bIsJoinWar = False
								bOfferPeace = True
								revArea = instigator.area()
								revPower = revArea.getPower(pRevPlayer.getID())
								pPower = revArea.getPower(pPlayer.getID())
								if( revPower > 0 ) :
									powerFrac = pPower/(1.0*revPower)
								else :
									powerFrac = 10.0

								if( powerFrac < 1.5 ) :
									# Rebels rival homeland power
									if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Rebels rival homeland power, limiting enlistment")
									bIsJoinWar = True

								handoverStr = getCityTextList(handoverCities)
								cityStr = getCityTextList(joinRevCities)

								bodStr = pRevPlayer.getName() + localText.getText("TXT_KEY_REV_LEADER",()) + ' ' + pRevPlayer.getCivilizationDescription(0)
								bodStr += ' ' + localText.getText("TXT_KEY_REV_JOINREV_OFFER",())
								bodStr += ' ' + handoverStr
								bodStr += localText.getText("TXT_KEY_REV_JOINREV_PEACE",())%(cityStr)

								joinRevCityIdxs = []
								for pCity in joinRevCities :
									joinRevCityIdxs.append( pCity.getID() )

								handoverCityIdxs = []
								for pCity in handoverCities :
									handoverCityIdxs.append( pCity.getID() )

								specialDataDict = { 'iRevPlayer' : pRevPlayer.getID(), 'bIsJoinWar' : bIsJoinWar, 'bOfferPeace' : bOfferPeace, 'HandoverCities' : handoverCityIdxs }
								revData = RevDefs.RevoltData( pPlayer.getID(), GAME.getGameTurn(), joinRevCityIdxs, 'independence', bPeaceful, specialDataDict )

								revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
								iRevoltIdx = len(revoltDict.keys())
								revoltDict[iRevoltIdx] = revData
								RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

								self.makeRevolutionDecision( pPlayer, iRevoltIdx, joinRevCities, 'independence', bPeaceful, bodStr )

								return

							else :
								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: No cities qualify for handover request, try something else")

		# All of these have violent and peaceful paths
#-------- Check if instigator influence by other culture -> try to join
		if( self.culturalRevolution and instigator.plot().calculateCulturePercent(iPlayer) <= self.maxNationalityThreshold ) :
			#cultOwnerID = instigator.plot().calculateCulturalOwner()
			# calculateCulturalOwner rules out dead civs ...
			maxCulture = 30
			cultOwnerID = -1
			for idx in xrange(GC.getMAX_PC_PLAYERS()) :
				if( instigator.plot().getCulture( idx ) > maxCulture ) :
					maxCulture = instigator.plot().getCulture( idx )
					cultOwnerID = idx

			if( cultOwnerID >= 0 and cultOwnerID < GC.getMAX_PC_PLAYERS() and not GC.getPlayer(cultOwnerID).getTeam() == pPlayer.getTeam() ) :
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s has majority culture from other player %d, asking to join"%(instigator.getName(),cultOwnerID))
				cultCities = []
				for pCity in revCities :
					maxCulture = 30
					cityCultOwnerID = -1
					for idx in xrange(GC.getMAX_PC_PLAYERS()) :
						if( pCity.plot().getCulture( idx ) > maxCulture ) :
							maxCulture = pCity.plot().getCulture( idx )
							cityCultOwnerID = idx

					if( cityCultOwnerID == cultOwnerID ) :
						cultCities.append(pCity)
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s also wants to go to other civ"%(pCity.getName()))

				cultPlayer = GC.getPlayer( cultOwnerID )
				cultTeam = GC.getTeam( cultPlayer.getTeam() )

				bodStr = getCityTextList(cultCities, bPreCity = True, second = localText.getText("TXT_KEY_REV_ALONG_WITH",()) + ' ', bPostIs = True)

				if( bPeaceful ) :

					if( cultPlayer.isAlive() and not cultPlayer.isMinorCiv() ) :
						if( pTeam.isAtWar(cultTeam.getID()) ) :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Owner at war with city's cultural civ")
							if( pTeam.canChangeWarPeace(cultTeam.getID()) ) :
								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Ask for end to hostilities")

						if( pPlayer.getCurrentEra() - cultPlayer.getCurrentEra() > 1 ) :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Tech divide with cultural player, ask for charity")

					# ask to join other civ, if denied get angrier
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Peaceful, asking to join the %s"%(cultPlayer.getCivilizationDescription(0)))

					pRevPlayer = None
					bIsJoinWar  = False
					joinPlayer = None

					if( cultPlayer.isAlive() ) :
						bodStr += ' ' + localText.getText("TXT_KEY_REV_CULT_PEACE_JOIN",()) + ' ' + cultPlayer.getCivilizationDescription(0) + '.'
						# Will only form pRevPlayer if human join player rebuffs revolutionaries
						if( cultPlayer.isStateReligion() ) :
							giveRelType = cultPlayer.getStateReligion()
						else :
							if( 50 > GAME.getSorenRandNum(100,'Rev') ) :
								giveRelType = None
							else :
								giveRelType = -1
						[pRevPlayer,bIsJoinWar] = self.chooseRevolutionCiv( cultCities, bJoinCultureWar = False, bReincarnate = True, bJoinRebels = False, bSpreadRebels = False, giveRelType = giveRelType )
						joinPlayer = cultPlayer

					else :
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Cult player is dead, trying to reform")
						bodStr += ' ' + localText.getText("TXT_KEY_REV_CULT_PEACE_REFORM",()) + ' ' + cultPlayer.getCivilizationShortDescription(0) + '.'
						pRevPlayer = cultPlayer


					bodStr += '  ' + localText.getText("TXT_KEY_REV_CULT_PEACE",())

				else :
					# demand to join other civ, if denied, fight!
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Violent, demanding to join the %s"%(cultPlayer.getCivilizationDescription(0)))

					if( cultPlayer.isAlive() ) :
						joinPlayer = cultPlayer
						if( joinPlayer.isStateReligion() ) :
							giveRelType = joinPlayer.getStateReligion()
						else :
							if( 50 > GAME.getSorenRandNum(100,'Rev') ) :
								giveRelType = None
							else :
								giveRelType = -1
						[pRevPlayer,bIsJoinWar] = self.chooseRevolutionCiv( cultCities, bJoinCultureWar = True, bReincarnate = True, bJoinRebels = True, bSpreadRebels = False, giveRelType = giveRelType )
						if( joinPlayer.getID() == pRevPlayer.getID() ) :
							joinPlayer = None
						else :
							if( self.allowSmallBarbRevs and len(cultCities) == 1 ) :
								if( instRevIdx < int(1.2*self.revInstigatorThreshold) ) :
									if( not instigator.area().isBorderObstacle(pPlayer.getTeam()) ) :
										pRevPlayer = GC.getPlayer( GC.getBARBARIAN_PLAYER() )
										bIsJoinWar = False
										if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Small, disorganized Revolution")
					else :
						pRevPlayer = cultPlayer
						bIsJoinWar = False
						joinPlayer = None
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Cult player is dead, trying to reform")

					if( not joinPlayer == None ) :
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Violent, demanding to join the %s"%(joinPlayer.getCivilizationDescription(0)))
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: If denied, will form/join the %s, alive: %d"%(pRevPlayer.getCivilizationDescription(0),pRevPlayer.isAlive()))
						bodStr += ' ' + localText.getText("TXT_KEY_REV_CULT_VIOLENT_JOIN",()) + ' ' + joinPlayer.getCivilizationDescription(0) + '.'
						if( bIsJoinWar ) :
							bodStr += '  ' + localText.getText("TXT_KEY_REV_CULT_VIOLENT_JOIN_DENY",()) + ' ' + pRevPlayer.getCivilizationShortDescription(0) + '.'
						else :
							if( pRevPlayer.isBarbarian() ) :
								bodStr += '  ' + localText.getText("TXT_KEY_REV_CULT_VIOLENT_JOIN_DENY_BARB",())
							elif( pRevPlayer.isAlive() ) :
								bodStr += '  ' + localText.getText("TXT_KEY_REV_CULT_VIOLENT_JOIN_DENY_ALIVE",())%(pRevPlayer.getCivilizationDescription(0))
							else :
								bodStr += '  ' + localText.getText("TXT_KEY_REV_CULT_VIOLENT_JOIN_DENY",()) + ' ' + pRevPlayer.getCivilizationShortDescription(0) + '.'
							if( GC.getTeam(pPlayer.getTeam()).canDeclareWar(joinPlayer.getTeam()) ) :
								bodStr += '  ' + localText.getText("TXT_KEY_REV_CULT_VIOLENT_JOIN_DECLARE_WAR",()) + ' ' +  joinPlayer.getCivilizationDescription(0) + '.'
					else :
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Violent, demanding to join the %s"%(pRevPlayer.getCivilizationDescription(0)))
						if( pRevPlayer.isBarbarian() ) :
							bodStr += ' ' + localText.getText("TXT_KEY_REV_CULT_VIOLENT_REFORM_BARB",())
						elif( not pRevPlayer.isAlive() ) :
							bodStr += ' ' + localText.getText("TXT_KEY_REV_CULT_VIOLENT_REFORM",()) + ' ' + pRevPlayer.getCivilizationShortDescription(0) + '.'
						else :
							bodStr += ' ' + localText.getText("TXT_KEY_REV_CULT_VIOLENT_JOIN",()) + ' ' + pRevPlayer.getCivilizationDescription(0) + '.'

					bodStr += '  ' + localText.getText("TXT_KEY_REV_CULT_VIOLENT_1",())
					if( not joinPlayer == None ) :
						bodStr += '  ' + localText.getText("TXT_KEY_REV_THE",()) + ' ' + joinPlayer.getCivilizationDescription(0) + ' ' + localText.getText("TXT_KEY_REV_CULT_VIOLENT_2",())
						if( pRevPlayer.isBarbarian() ) :
							bodStr += ' ' + localText.getText("TXT_KEY_REV_CULT_VIOLENT_BARB",())
						else :
							bodStr += ' ' + localText.getText("TXT_KEY_REV_CULT_VIOLENT_FIGHT",()) + ' ' + pRevPlayer.getCivilizationShortDescription(0) + '.'
					else :
						if( pRevPlayer.isBarbarian() ) :
							pass
						elif( pRevPlayer.isAlive() ) :
							bodStr += '  ' + localText.getText("TXT_KEY_REV_CULT_VIOLENT_ENEMY",()) + ' ' + pRevPlayer.getCivilizationDescription(0) + '.'

					bodStr += '  ' + localText.getText("TXT_KEY_REV_CULT_VIOLENT_FINAL",())

				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %d cities in revolution"%(len(cultCities)))
				assert( len(cultCities) > 0 )

				specialDataDict = {'iRevPlayer' : pRevPlayer.getID(), 'bIsJoinWar' : bIsJoinWar }
				if( not joinPlayer == None ) :
					specialDataDict['iJoinPlayer'] = joinPlayer.getID()
				cityIdxs = []
				for pCity in cultCities :
					cityIdxs.append( pCity.getID() )
				revData = RevDefs.RevoltData( pPlayer.getID(), GAME.getGameTurn(), cityIdxs, 'independence', bPeaceful, specialDataDict )

				revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
				iRevoltIdx = len(revoltDict.keys())
				revoltDict[iRevoltIdx] = revData
				RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

				self.makeRevolutionDecision( pPlayer, iRevoltIdx, cultCities, 'independence', bPeaceful, bodStr )

				return

#-------- Check for religious revolution
		stateRel = pPlayer.getStateReligion()
		if( self.religiousRevolution and pPlayer.isStateReligion() and stateRel >= 0 ) :
			revRel = None

			# Check holy city instigating
			if( instigator.isHolyCity() ) :

				if( not instigator.isHolyCityByType(stateRel) ) :

					for relType in xrange(GC.getNumReligionInfos()) :
						if( instigator.isHolyCityByType(relType) and not stateRel == relType ) :
							if( self.allowStateReligionToJoin or not instigator.isHasReligion(stateRel) ) :
								revRel = relType
								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Instigator is rival religion (%s) holy city"%(GC.getReligionInfo( revRel ).getDescription()))
								break

			# Check for significant minority religion
			if( revRel == None  ) :
				if( self.allowStateReligionToJoin or not instigator.isHasReligion(stateRel) ) :
					if( len(revCities) >= 3 ) :
						# Must be large movement of like minded cities
						maxCount = 0
						maxCountRel = -1
						for relType in xrange(GC.getNumReligionInfos()) :
							if( instigator.isHasReligion(relType) and not stateRel == relType ) :
								relCount = 0
								for pCity in revCities :
									if( pCity.isHasReligion(relType) ) :
										if( pCity.isHasReligion(stateRel) ) :
											relCount += .5
										else :
											relCount += 1
								if( relCount > maxCount ) :
									maxCount = relCount
									maxCountRel = relType

						# Is the best good enough?
						if( maxCount >= 3 ) :
							revRel = maxCountRel
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Instigator and enough others have %s"%(GC.getReligionInfo( revRel ).getDescription()))

			# Decide how to revolt
			if( not revRel == None ) :
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Religious revolution")

				relCities = []
				for pCity in revCities :
					if( pCity.isHasReligion(revRel) ) :
						if( self.allowStateReligionToJoin or not pCity.isHasReligion(stateRel) ) :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s has %s"%(pCity.getName(),GC.getReligionInfo( revRel ).getDescription()))
							relCities.append(pCity)

				if( bPeaceful ) :
					# Check if there are civics they would like changed
					newRelCivic = None

					[level,optionType] = RevUtils.getReligiousFreedom( iPlayer )
					[newLevel,newRelCivic] = RevUtils.getBestReligiousFreedom( iPlayer, optionType )

					if( not newRelCivic == None and newLevel > level ) :
						if( level < -5 or (newLevel > 5 and level < 0) ) :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Asking for change from %s to %s"%(GC.getCivicInfo(pPlayer.getCivics(optionType)).getDescription(),GC.getCivicInfo(newRelCivic).getDescription()))

							bodStr = getCityTextList(relCities, bPreCity = True, bPostIs = True)

							# Can't pay them enough so they don't feel oppressed

							if( level < -5 ) :
								bodStr += ' ' + localText.getText("TXT_KEY_REV_REL_THEOCRACY1",()) + ' ' + GC.getCivicInfo(pPlayer.getCivics(optionType)).getDescription() + ".  " + localText.getText("TXT_KEY_REV_REL_THEOCRACY2",()) + ' ' + GC.getCivicInfo(newRelCivic).getDescription() + '.'
							else :
								bodStr += ' ' + localText.getText("TXT_KEY_REV_REL_FREE_REL1",()) + ' ' + GC.getCivicInfo(newRelCivic).getDescription() + '.  ' + localText.getText("TXT_KEY_REV_REL_FREE_REL2",()) + ' ' + GC.getReligionInfo(stateRel).getDescription()
								bodStr += ' ' + localText.getText("TXT_KEY_REV_REL_PRACTICE",()) + ' ' + GC.getReligionInfo(revRel).getDescription() + '.'
							bodStr += '\n\n' + localText.getText("TXT_KEY_REV_HONORING",())

							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %d cities in revolution"%(len(relCities)))
							assert( len(relCities) > 0 )

							specialDataDict = { 'iNewCivic' : newRelCivic }
							cityIdxs = []
							for pCity in relCities :
								cityIdxs.append( pCity.getID() )
							revData = RevDefs.RevoltData( pPlayer.getID(), GAME.getGameTurn(), cityIdxs, 'civics', bPeaceful, specialDataDict )

							revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
							iRevoltIdx = len(revoltDict.keys())
							revoltDict[iRevoltIdx] = revData
							RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

							self.makeRevolutionDecision( pPlayer, iRevoltIdx, relCities, 'civics', bPeaceful, bodStr )

							return

					# Ask for switch of state religions?
					revRelCount = pPlayer.getHasReligionCount(revRel)
					stateRelCount = pPlayer.getHasReligionCount(stateRel)
					if( revRelCount > stateRelCount/4 ) :

						bodStr = getCityTextList(relCities, bPreCity = True, bPostIs = True)

						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Asking change in state religion, %d practice new, %d practice state"%(revRelCount,stateRelCount))
						totalRevIdx = 0
						for pCity in relCities :
							totalRevIdx += pCity.getRevolutionIndex()
						iBuyOffCost = totalRevIdx/(17-pPlayer.getCurrentEra()) + GAME.getSorenRandNum(50+10*pPlayer.getCurrentEra(),'Rev')
						#iBuyOffCost = (60 + 12*pPlayer.getCurrentEra())*len(relCities) + GAME.getSorenRandNum(50+10*pPlayer.getCurrentEra(),'Rev')
						if( not pPlayer.isHuman() ) : iBuyOffCost = int( iBuyOffCost*.7 )
						iBuyOffCost = max( [iBuyOffCost, pPlayer.getGold()/10 + GAME.getSorenRandNum(50,'Rev')] )
						bodStr += ' ' + localText.getText("TXT_KEY_REV_REL_CHANGE",()) + ' ' + GC.getReligionInfo( revRel ).getDescription() + '.'
						bodStr += '  ' + localText.getText("TXT_KEY_REV_CURRENTLY",()) + ' %d '%(revRelCount) + localText.getText("TXT_KEY_REV_REL_NEW_REL",()) + ' %d '%(stateRelCount) + localText.getText("TXT_KEY_REV_REL_STATE_REL",())
						bodStr += '\n\n' + localText.getText("TXT_KEY_REV_REL_HONORING",())

						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %d cities in revolution, buyoff cost %d"%(len(relCities),iBuyOffCost))
						assert( len(relCities) > 0 )
						specialDataDict = { 'iNewReligion' : revRel, 'iBuyOffCost' : iBuyOffCost }
						cityIdxs = []
						for pCity in relCities :
							cityIdxs.append( pCity.getID() )
						revData = RevDefs.RevoltData( pPlayer.getID(), GAME.getGameTurn(), cityIdxs, 'religion', bPeaceful, specialDataDict )

						revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
						iRevoltIdx = len(revoltDict.keys())
						revoltDict[iRevoltIdx] = revData
						RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

						self.makeRevolutionDecision( pPlayer, iRevoltIdx, relCities, 'religion', bPeaceful, bodStr )

						return

					# Ask for independence
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Asking for independence for religious reasons")

					# Prune for only close cities since cities in area may be quite far away
					indCities = []
					for pCity in relCities :
						# Add only cities near instigator in first pass
						cityDist = plotDistance( pCity.getX(), pCity.getY(), instigator.getX(), instigator.getY() )
						if( cityDist <= self.closeRadius ) :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s is close enough to instigator to join in independence quest"%(pCity.getName()))
							indCities.append(pCity)

					for pCity in relCities :
						if( not pCity in indCities ) :
							# Add cities a little further away that are also near another rebelling city
							cityDist = plotDistance( pCity.getX(), pCity.getY(), instigator.getX(), instigator.getY() )
							if( cityDist <= 2.0*self.closeRadius ) :
								for iCity in indCities :
									cityDist = min([cityDist, plotDistance( pCity.getX(), pCity.getY(), iCity.getX(), iCity.getY() )])

								if( cityDist <= 0.8*self.closeRadius ) :
									if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s is close enough to another rebelling city to join in independence quest"%(pCity.getName()))
									indCities.append(pCity)

					bodStr = getCityTextList(indCities, bPreCity = True, bPostIs = True)

					#iBuyOffCost = (60 + 12*pPlayer.getCurrentEra())*len(indCities) + GAME.getSorenRandNum(50+10*pPlayer.getCurrentEra(),'Rev')
					totalRevIdx = 0
					for pCity in indCities :
						totalRevIdx += pCity.getRevolutionIndex()
					iBuyOffCost = totalRevIdx/(17-pPlayer.getCurrentEra()) + GAME.getSorenRandNum(50+10*pPlayer.getCurrentEra(),'Rev')
					if( not pPlayer.isHuman() ) : iBuyOffCost = int( iBuyOffCost*.7 )
					iBuyOffCost = max( [iBuyOffCost, pPlayer.getGold()/12 + GAME.getSorenRandNum(50,'Rev')] )

					[pRevPlayer,bIsJoinWar] = self.chooseRevolutionCiv( indCities, bJoinCultureWar = False, bReincarnate = True, bJoinRebels = False, bSpreadRebels = False, giveRelType = revRel, bMatchCivics = True )

					vassalStyle = None
					if( not pTeam.isAVassal() and pTeam.isVassalStateTrading() ) :
						if( totalRevIdx/len(indCities) < self.revInstigatorThreshold ) :
							if( pRevPlayer.AI_getAttitude(pPlayer.getID()) == AttitudeTypes.ATTITUDE_FRIENDLY or pRevPlayer.AI_getAttitude(pPlayer.getID()) == AttitudeTypes.ATTITUDE_PLEASED ) :
								vassalStyle = 'free'
								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Vassal style %s chosen"%(vassalStyle))

					if( not vassalStyle == None ) :
						bodStr += ' ' + localText.getText("TXT_KEY_REV_REL_PEACE_VASSAL_1",()) + ' ' + GC.getReligionInfo( revRel ).getDescription() + '.'
						bodStr += '  ' + localText.getText("TXT_KEY_REV_REL_PEACE_VASSAL_2",()) + ' ' + pRevPlayer.getCivilizationShortDescription(0) + ' ' + localText.getText("TXT_KEY_REV_REL_PEACE_VASSAL_3",())
					else :
						bodStr += ' ' + localText.getText("TXT_KEY_REV_REL_PEACE_IND_1",()) + ' ' + GC.getReligionInfo( revRel ).getDescription() + '.'
						bodStr += '  ' + localText.getText("TXT_KEY_REV_REL_PEACE_IND_2",()) + ' ' + GC.getReligionInfo(pPlayer.getStateReligion()).getDescription() + ' ' + localText.getText("TXT_KEY_REV_REL_PEACE_IND_3",()) + ' ' + pRevPlayer.getCivilizationShortDescription(0) + '.'
						bodStr += '\n\n' + localText.getText("TXT_KEY_REV_REL_PEACE",())

					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %d cities in revolution, buyoff cost %d"%(len(indCities),iBuyOffCost))
					assert( len(indCities) > 0 )
					specialDataDict = { 'iRevPlayer' : pRevPlayer.getID(), 'iBuyOffCost' : iBuyOffCost, 'vassalStyle' : vassalStyle }
					cityIdxs = []
					for pCity in indCities :
						cityIdxs.append( pCity.getID() )
					revData = RevDefs.RevoltData( pPlayer.getID(), GAME.getGameTurn(), cityIdxs, 'independence', bPeaceful, specialDataDict )

					revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
					iRevoltIdx = len(revoltDict.keys())
					revoltDict[iRevoltIdx] = revData
					RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

					self.makeRevolutionDecision( pPlayer, iRevoltIdx, indCities, 'independence', bPeaceful, bodStr )

					return

				else :
					# Demand independence for religious reasons
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Demanding independence for religious reasons")

					# Prune for only close cities, cities in area may be quite far away
					indCities = []
					for pCity in relCities :
						# Add only cities near instigator in first pass
						cityDist = plotDistance( pCity.getX(), pCity.getY(), instigator.getX(), instigator.getY() )
						if( cityDist <= self.closeRadius ) :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s is close enough to instigator to join in independence quest"%(pCity.getName()))
							indCities.append(pCity)

					for pCity in relCities :
						if( not pCity in indCities ) :
							# Add cities a little further away that are also near another rebelling city
							cityDist = plotDistance( pCity.getX(), pCity.getY(), instigator.getX(), instigator.getY() )
							if( cityDist <= 2.0*self.closeRadius ) :
								for iCity in indCities :
									cityDist = min([cityDist, plotDistance( pCity.getX(), pCity.getY(), iCity.getX(), iCity.getY() )])

								if( cityDist <= 0.8*self.closeRadius ) :
									if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s is close enough to another rebellin city to join in independence quest"%(pCity.getName()))
									indCities.append(pCity)

					bodStr = getCityTextList(indCities, bPreCity = True, bPostIs = True)

					bodStr += ' ' + localText.getText("TXT_KEY_REV_REL_VIOLENT_IND_1",()) + ' %s.'%(GC.getReligionInfo( revRel ).getDescription())
					bodStr += '  ' + localText.getText("TXT_KEY_REV_REL_VIOLENT_IND_2",()) + ' %s '%(GC.getReligionInfo( pPlayer.getStateReligion() ).getDescription()) + localText.getText("TXT_KEY_REV_REL_VIOLENT_IND_3",())
					bodStr += '\n\n' + localText.getText("TXT_KEY_REV_REL_VIOLENT_IND",())

					[pRevPlayer,bIsJoinWar] = self.chooseRevolutionCiv( indCities, bJoinCultureWar = False, bReincarnate = True, bJoinRebels = False, bSpreadRebels = False, giveRelType = revRel )

					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %d cities in revolution"%(len(indCities)))
					assert( len(indCities) > 0 )
					specialDataDict = { 'iRevPlayer' : pRevPlayer.getID(), 'vassalStyle' : None }
					cityIdxs = []
					for pCity in indCities :
						cityIdxs.append( pCity.getID() )
					revData = RevDefs.RevoltData( pPlayer.getID(), GAME.getGameTurn(), cityIdxs, 'independence', bPeaceful, specialDataDict )

					revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
					iRevoltIdx = len(revoltDict.keys())
					revoltDict[iRevoltIdx] = revData
					RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

					self.makeRevolutionDecision( pPlayer, iRevoltIdx, indCities, 'independence', bPeaceful, bodStr )

					return

# --------------- Special options for homeland revolutions
		if instigator.area().getID() == pPlayer.getCapitalCity().area().getID():
			# Revolution in homeland
			if self.LOG_DEBUG:
				print "[REV] Revolution in homeland"

			if bPeaceful and not GC.getTeam(pPlayer.getTeam()).isHasTech(self.iNationalismTech):
				[goodEffect, badEffect] = RevUtils.getCivicsHolyCityEffects(iPlayer)
				if badEffect > 0:
					stateRel = pPlayer.getStateReligion()
					if pPlayer.isStateReligion() and stateRel >= 0:
						# Check for ask for crusade
						stateHolyCity = GAME.getHolyCity(stateRel)
						stateHolyCityOwner = GC.getPlayer(stateHolyCity.getOwner())

						if (stateHolyCityOwner is not None
						and instigator.isHasReligion(stateRel)
						and not stateHolyCityOwner.getID() == iPlayer
						and pTeam.canDeclareWar(stateHolyCityOwner.getTeam())
						and not pTeam.isAVassal()
						):
							if self.LOG_DEBUG:
								print "[REV] Holy city for %s (%d) is %s, owner %s practices %d"%(GC.getReligionInfo(stateRel).getDescription(),stateRel,stateHolyCity.getName(),stateHolyCityOwner.getCivilizationDescription(0),stateHolyCityOwner.getStateReligion())

							relCities = []
							for city in revCities:
								if city.isHasReligion(stateRel):
									if self.LOG_DEBUG:
										print "[REV] %s has state religion" % city.getName()
									relCities.append(city)

							bodStr = getCityTextList(revCities, bPreCity = True, bPostIs = True)

							if( not stateHolyCityOwner.getStateReligion() == stateRel ) :
								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Ask for Crusade against %s!"%(stateHolyCityOwner.getCivilizationDescription(0)))

								bodStr += " " + localText.getText("TXT_KEY_REV_HL_HOLY_WAR",()) + " %s."%(stateHolyCityOwner.getCivilizationDescription(0))
								bodStr += "   " + localText.getText("TXT_KEY_REV_HL_HOLY_RECLAIM",()) + " %s, "%(stateHolyCity.getName()) + localText.getText("TXT_KEY_REV_HL_HOLY_HEATHENS",())
								bodStr += "\n\n" + localText.getText("TXT_KEY_REV_HL_HOLY_REQUEST",())

								assert( len(relCities) > 0 )

								specialDataDict = { 'iRevPlayer' : stateHolyCityOwner.getID() }
								cityIdxs = []
								for pCity in relCities :
									cityIdxs.append( pCity.getID() )
								revData = RevDefs.RevoltData( pPlayer.getID(), GAME.getGameTurn(), cityIdxs, 'war', bPeaceful, specialDataDict )

								revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
								iRevoltIdx = len(revoltDict.keys())
								revoltDict[iRevoltIdx] = revData
								RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

								self.makeRevolutionDecision( pPlayer, iRevoltIdx, relCities, 'war', bPeaceful, bodStr )

								return

							if( pPlayer.AI_getAttitude(stateHolyCityOwner.getID()) == AttitudeTypes.ATTITUDE_FURIOUS or pPlayer.AI_getAttitude(stateHolyCityOwner.getID()) == AttitudeTypes.ATTITUDE_ANNOYED ) :
								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Ask for crusade against fellow believer, %s!"%(stateHolyCityOwner.getCivilizationDescription(0)))

								bodStr += " " + localText.getText("TXT_KEY_REV_HL_HOLY_WAR",()) + " %s."%(stateHolyCityOwner.getCivilizationDescription(0))
								bodStr += "   " + localText.getText("TXT_KEY_REV_HL_HOLY_WHILE",()) + ' %s '%(stateHolyCityOwner.getCivilizationDescription(0)) + localText.getText("TXT_KEY_REV_HL_HOLY_CLAIMS",()) + ' %s, '%(GC.getReligionInfo(stateRel).getDescription()) + localText.getText("TXT_KEY_REV_HL_HOLY_WORTHY",()) + " %s!"%(stateHolyCity.getName())
								bodStr += "   " + localText.getText("TXT_KEY_REV_HL_HOLY_DEVOTION",()) + " %s "%(stateHolyCity.getName()) + localText.getText("TXT_KEY_REV_HL_HOLY_UNWORTHY",())
								bodStr += "\n\n" + localText.getText("TXT_KEY_REV_HL_HOLY_REQUEST",())

								assert( len(relCities) > 0 )
								specialDataDict = { 'iRevPlayer' : stateHolyCityOwner.getID() }
								cityIdxs = []
								for pCity in relCities :
									cityIdxs.append( pCity.getID() )
								revData = RevDefs.RevoltData( pPlayer.getID(), GAME.getGameTurn(), cityIdxs, 'war', bPeaceful, specialDataDict )

								revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
								iRevoltIdx = len(revoltDict.keys())
								revoltDict[iRevoltIdx] = revData
								RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

								self.makeRevolutionDecision( pPlayer, iRevoltIdx, relCities, 'war', bPeaceful, bodStr )

								return


			if( self.civicRevolution ) :
				[laborLevel,optionType] = RevUtils.getLaborFreedom( iPlayer )
				[newLaborLevel,newCivic] = RevUtils.getBestLaborFreedom( iPlayer, optionType )
				if( bPeaceful and laborLevel < 0 and newLaborLevel > 5 and not newCivic == None ) :
					if( (10*abs(laborLevel) > GAME.getSorenRandNum(100, 'Revolt - emancipation request')) ):
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Asking change to emancipation, %d"%(newCivic))

						bodStr = getCityTextList(revCities, bPreCity = True, bPostIs = True)

						#iBuyOffCost = (50 + 10*pPlayer.getCurrentEra())*len(revCities) + GAME.getSorenRandNum(50+10*pPlayer.getCurrentEra(),'Rev')
						totalRevIdx = 0
						for pCity in revCities :
							totalRevIdx += pCity.getRevolutionIndex()
						iBuyOffCost = totalRevIdx/(20-pPlayer.getCurrentEra()) + GAME.getSorenRandNum(50+10*pPlayer.getCurrentEra(),'Rev')
						if( not pPlayer.isHuman() ) : iBuyOffCost = int( iBuyOffCost*.7 )
						iBuyOffCost = max( [iBuyOffCost, pPlayer.getGold()/10 + GAME.getSorenRandNum(50,'Rev')] )
						bodStr += ' ' + localText.getText("TXT_KEY_REV_HL_EMAN_REJECT",()) + ' %s '%(GC.getCivicInfo(newCivic).getDescription()) + localText.getText("TXT_KEY_REV_HL_EMAN_CIVIC",())
						bodStr += '\n\n' + localText.getText("TXT_KEY_REV_PEACEFUL_CONCLUSION",())
						bodStr += '  ' + localText.getText("TXT_KEY_REV_HL_EMAN_SLAVE",())
						bodStr += '  ' + localText.getText("TXT_KEY_REV_BRIBE",())

						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %d cities in revolution, buyoff cost %d"%(len(revCities),iBuyOffCost))
						assert( len(revCities) > 0 )

						specialDataDict = { 'iNewCivic' : newCivic, 'iBuyOffCost' : iBuyOffCost }
						cityIdxs = []
						for pCity in revCities :
							cityIdxs.append( pCity.getID() )
						revData = RevDefs.RevoltData( pPlayer.getID(), GAME.getGameTurn(), cityIdxs, 'civics', bPeaceful, specialDataDict )

						revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
						iRevoltIdx = len(revoltDict.keys())
						revoltDict[iRevoltIdx] = revData
						RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

						self.makeRevolutionDecision( pPlayer, iRevoltIdx, revCities, 'civics', bPeaceful, bodStr )

						return

				if( laborLevel < -5 and newLaborLevel > (laborLevel + 2) and not newCivic == None ) :
					if( not bPeaceful and 50 > GAME.getSorenRandNum( 100, 'Revolt - do slave rebellion' ) ) :
						if( not instigator.area().isBorderObstacle(pPlayer.getTeam()) ) :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Slave rebellion!!!, %d"%(newCivic))

							slaveCities = []
							for pCity in revCities :
								# Add only cities near instigator in first pass
								cityDist = plotDistance( pCity.getX(), pCity.getY(), instigator.getX(), instigator.getY() )
								if( cityDist <= self.closeRadius ) :
									if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s is close enough to instigator to join in slave revolt"%(pCity.getName()))
									slaveCities.append(pCity)

							for pCity in revCities :
								if( not pCity in slaveCities ) :
									# Add cities a little further away that are also near another rebelling city
									cityDist = plotDistance( pCity.getX(), pCity.getY(), instigator.getX(), instigator.getY() )
									if( cityDist <= 2.0*self.closeRadius ) :
										for iCity in slaveCities :
											cityDist = min([cityDist, plotDistance( pCity.getX(), pCity.getY(), iCity.getX(), iCity.getY() )])

										if( cityDist <= 0.8*self.closeRadius ) :
											if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s is close enough to another rebellin city to join in slave revolt"%(pCity.getName()))
											slaveCities.append(pCity)

							bodStr = localText.getText("TXT_KEY_REV_HL_SLAVE_REBELLION",())
							bodStr += getCityTextList(slaveCities) + '!'

							bodStr += '  ' + localText.getText("TXT_KEY_REV_HL_SLAVE_DEMAND",())
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Asking change to %s"%(GC.getCivicInfo(newCivic).getDescription()))
							bodStr += ' %s '%(GC.getCivicInfo(newCivic).getDescription())

							bodStr += localText.getText("TXT_KEY_REV_HL_SLAVE_DENY",())

							# Slaves always rise up as barbarians
							pRevPlayer = GC.getPlayer( GC.getBARBARIAN_PLAYER() )
							assert(len(slaveCities) > 0)
							specialDataDict = { 'iNewCivic' : newCivic, 'iRevPlayer' : pRevPlayer.getID() }
							cityIdxs = []
							for pCity in slaveCities :
								cityIdxs.append( pCity.getID() )
							revData = RevDefs.RevoltData( pPlayer.getID(), GAME.getGameTurn(), cityIdxs, 'civics', bPeaceful, specialDataDict )

							revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
							iRevoltIdx = len(revoltDict.keys())
							revoltDict[iRevoltIdx] = revData
							RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

							self.makeRevolutionDecision( pPlayer, iRevoltIdx, slaveCities, 'civics', bPeaceful, bodStr )

							return

				[enviroLevel,optionType] = RevUtils.getEnvironmentalProtection( iPlayer )
				[newEnviroLevel,newCivic] = RevUtils.getBestEnvironmentalProtection( iPlayer, optionType )
				if( bPeaceful and newEnviroLevel > enviroLevel + 2 and not newCivic == None ) :
					if( 30 > GAME.getSorenRandNum(100, 'Revolt - environmentalism request') ):
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Asking change to %s, %d (environment)"%(GC.getCivicInfo(newCivic).getDescription(),newCivic))

						bodStr = getCityTextList(revCities, bPreCity = True, bPostIs = True)

						#iBuyOffCost = (50 + 10*pPlayer.getCurrentEra())*len(revCities) + GAME.getSorenRandNum(50+10*pPlayer.getCurrentEra(),'Rev')
						totalRevIdx = 0
						for pCity in revCities :
							totalRevIdx += pCity.getRevolutionIndex()
						iBuyOffCost = totalRevIdx/(20-pPlayer.getCurrentEra()) + GAME.getSorenRandNum(50+10*pPlayer.getCurrentEra(),'Rev')
						if( not pPlayer.isHuman() ) : iBuyOffCost = int( iBuyOffCost*.7 )
						iBuyOffCost = max( [iBuyOffCost, pPlayer.getGold()/10 + GAME.getSorenRandNum(50,'Rev')] )
						bodStr += ' ' + localText.getText("TXT_KEY_REV_HL_ENV_REQUEST",()) + ' %s.'%(GC.getCivicInfo(newCivic).getDescription())
						bodStr += '  ' + localText.getText("TXT_KEY_REV_HL_ENV_GREEN",())
						bodStr += '\n\n' + localText.getText("TXT_KEY_REV_PEACEFUL_CONCLUSION",())
						bodStr += '  ' + localText.getText("TXT_KEY_REV_HL_ENV_SMOG",())
						bodStr += '  ' + localText.getText("TXT_KEY_REV_BRIBE",())

						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %d cities in revolution, buyoff cost %d"%(len(revCities),iBuyOffCost))
						assert( len(revCities) > 0 )
						specialDataDict = { 'iNewCivic' : newCivic, 'iBuyOffCost' : iBuyOffCost }
						cityIdxs = []
						for pCity in revCities :
							cityIdxs.append( pCity.getID() )
						revData = RevDefs.RevoltData( pPlayer.getID(), GAME.getGameTurn(), cityIdxs, 'civics', bPeaceful, specialDataDict )

						revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
						iRevoltIdx = len(revoltDict.keys())
						revoltDict[iRevoltIdx] = revData
						RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

						self.makeRevolutionDecision( pPlayer, iRevoltIdx, revCities, 'civics', bPeaceful, bodStr )

						return


# -------------------- Special options for colonial revolutions
		else :
			# Revolution based in other area
			# These are special requests peaceful colonists may make
			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Revolution in colony")

			foreignCities = []
			capitalArea = pPlayer.getCapitalCity().area().getID()
			for pCity in revCities :
				if( not pCity.area().getID() == capitalArea ) :
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s is colony"%(pCity.getName()))
					foreignCities.append( pCity )

			bodStr = getCityTextList(foreignCities, bPreCity = True, bPostIs = True)

			if( bPeaceful and self.civicRevolution ) :
				# Sufferage or representation
				[demoLevel,optionType] = RevUtils.getDemocracyLevel( iPlayer )
				[newDemoLevel,newCivic] = RevUtils.getBestDemocracyLevel( iPlayer, optionType )
				if( demoLevel < 0 and newDemoLevel > 0 and not newCivic == None ) :

						bodStr += ' ' + localText.getText("TXT_KEY_REV_COL_GOVT_REQUEST",())
						if( newDemoLevel > 9 ) :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Asking change to universal sufferage")
							bodStr += "  " + localText.getText("TXT_KEY_REV_COL_GOVT_PROTESTING",()) + " %s!"%(GC.getCivicInfo(newCivic).getDescription())
						else :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Asking change to representation")
							bodStr += "  " + localText.getText("TXT_KEY_REV_COL_GOVT_CRIES",()) + " %s!' "%(GC.getCivicInfo(newCivic).getDescription()) +localText.getText("TXT_KEY_REV_COL_GOVT_MARCH",())

						#iBuyOffCost = (75 + 12*pPlayer.getCurrentEra())*len(foreignCities) + GAME.getSorenRandNum(100+10*pPlayer.getCurrentEra(),'Rev')
						totalRevIdx = 0
						for pCity in foreignCities :
							totalRevIdx += pCity.getRevolutionIndex()
						iBuyOffCost = totalRevIdx/(15-pPlayer.getCurrentEra()) + GAME.getSorenRandNum(80+10*pPlayer.getCurrentEra(),'Rev')
						if( not pPlayer.isHuman() ) : iBuyOffCost = int( iBuyOffCost*.7 )
						iBuyOffCost = max( [iBuyOffCost, pPlayer.getGold()/8 + GAME.getSorenRandNum(50,'Rev')] )
						bodStr += '\n\n' + localText.getText("TXT_KEY_REV_PEACEFUL_CONCLUSION",())
						bodStr += '  ' + localText.getText("TXT_KEY_REV_COL_GOVT_PRACTICES",())
						bodStr += '  ' + localText.getText("TXT_KEY_REV_BRIBE",())

						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %d cities in revolution, buyoff cost %d"%(len(revCities),iBuyOffCost))
						assert( len(revCities) > 0 )
						specialDataDict = { 'iNewCivic' : newCivic, 'iBuyOffCost' : iBuyOffCost }
						cityIdxs = []
						for pCity in foreignCities :
							cityIdxs.append( pCity.getID() )
						revData = RevDefs.RevoltData( pPlayer.getID(), GAME.getGameTurn(), cityIdxs, 'civics', bPeaceful, specialDataDict )

						revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
						iRevoltIdx = len(revoltDict.keys())
						revoltDict[iRevoltIdx] = revData
						RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

						self.makeRevolutionDecision( pPlayer, iRevoltIdx, foreignCities, 'civics', bPeaceful, bodStr )

						return

			#if( bPeaceful and self.vassalRevolution ) :
				# Ask to become vassal
				# Trim down cities to only those close to instigator


#-------- If capital or majority of cities,
		if( revInCapital or (len(revCities) >= (pPlayer.getNumCities()-1)/2 and len(revCities) > 2) ) :

			bodStr = getCityTextList(revCities, bPreCity = True, bPostIs = True)

			if( bPeaceful ) :
				# If peaceful, ask change to civics ... if no civics, ask for change of leader
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Capital or large number of cities in peaceful revolution!")

				if( self.civicRevolution ) :
					# Emancipation
					[laborLevel,optionType] = RevUtils.getLaborFreedom( iPlayer )
					[newLaborLevel,newCivic] = RevUtils.getBestLaborFreedom( iPlayer, optionType )
					if( laborLevel < 0 and newLaborLevel > 5 and not newCivic == None ):
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Asking change to %s, %d"%(GC.getCivicInfo(newCivic).getDescription(),newCivic))

						#iBuyOffCost = (50 + 12*pPlayer.getCurrentEra())*len(revCities) + GAME.getSorenRandNum(50+10*pPlayer.getCurrentEra(),'Rev')
						totalRevIdx = 0
						for pCity in revCities :
							totalRevIdx += pCity.getRevolutionIndex()
						iBuyOffCost = totalRevIdx/(20-pPlayer.getCurrentEra()) + GAME.getSorenRandNum(50+10*pPlayer.getCurrentEra(),'Rev')
						if( not pPlayer.isHuman() ) : iBuyOffCost = int( iBuyOffCost*.7 )
						iBuyOffCost = max( [iBuyOffCost, pPlayer.getGold()/10 + GAME.getSorenRandNum(50,'Rev')] )
						bodStr += ' ' + localText.getText("TXT_KEY_REV_HL_EMAN_REJECT",()) + ' %s '%(GC.getCivicInfo(newCivic).getDescription()) + localText.getText("TXT_KEY_REV_HL_EMAN_CIVIC",())
						bodStr += '\n\n' + localText.getText("TXT_KEY_REV_PEACEFUL_CONCLUSION",())
						bodStr += '  ' + localText.getText("TXT_KEY_REV_HL_EMAN_SLAVE",())
						bodStr += '  ' + localText.getText("TXT_KEY_REV_BRIBE",())

						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %d cities in revolution, buyoff cost %d"%(len(revCities),iBuyOffCost))
						assert( len(revCities) > 0 )
						specialDataDict = { 'iNewCivic' : newCivic, 'iBuyOffCost' : iBuyOffCost }
						cityIdxs = []
						for pCity in revCities :
							cityIdxs.append( pCity.getID() )
						revData = RevDefs.RevoltData( pPlayer.getID(), GAME.getGameTurn(), cityIdxs, 'civics', bPeaceful, specialDataDict )

						revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
						iRevoltIdx = len(revoltDict.keys())
						revoltDict[iRevoltIdx] = revData
						RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

						self.makeRevolutionDecision( pPlayer, iRevoltIdx, revCities, 'civics', bPeaceful, bodStr )

						return

					[isCanDoCommunism,newCivic] = RevUtils.canDoCommunism( iPlayer )
					if( isCanDoCommunism and not newCivic == None ) :
						if( 35 > GAME.getSorenRandNum(100, 'Rev - Communist revolution') ) :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Communist revolution, ask change to %s, %d"%(GC.getCivicInfo(newCivic).getDescription(), newCivic))

							bodStr += ' ' + localText.getText("TXT_KEY_REV_CAP_COM_REQUEST",()) + ' %s.  '%(GC.getCivicInfo(newCivic).getDescription()) + localText.getText("TXT_KEY_REV_CAP_COM_BROTHER",())
							bodStr += ' %s '%(GC.getCivicInfo(newCivic).getDescription()) + localText.getText("TXT_KEY_REV_CAP_COM_ECON",())

							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %d cities in revolution"%(len(revCities)))
							assert( len(revCities) > 0 )
							specialDataDict = { 'iNewCivic' : newCivic }
							cityIdxs = []
							for pCity in revCities :
								cityIdxs.append( pCity.getID() )
							revData = RevDefs.RevoltData( pPlayer.getID(), GAME.getGameTurn(), cityIdxs, 'civics', bPeaceful, specialDataDict )

							revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
							iRevoltIdx = len(revoltDict.keys())
							revoltDict[iRevoltIdx] = revData
							RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

							self.makeRevolutionDecision( pPlayer, iRevoltIdx, revCities, 'civics', bPeaceful, bodStr )

							return

					# Sufferage or representation
					[demoLevel,optionType] = RevUtils.getDemocracyLevel( iPlayer )
					[newDemoLevel,newCivic] = RevUtils.getBestDemocracyLevel( iPlayer, optionType )
					if( demoLevel < 0 and newDemoLevel > 0 and not newCivic == None ) :

							bodStr += ' ' + localText.getText("TXT_KEY_REV_CAP_VOTE_REQUEST",())
							if( newDemoLevel > 9 ) :
								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Asking change to universal sufferage, %d"%(newCivic))
								bodStr += "  " + localText.getText("TXT_KEY_REV_CAP_VOTE_US",()) + " %s!"%(GC.getCivicInfo(newCivic).getDescription())
							else :
								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Asking change to representation, %d"%(newCivic))
								bodStr += "  " + localText.getText("TXT_KEY_REV_CAP_VOTE_CRIES",()) + " %s!' "%(GC.getCivicInfo(newCivic).getDescription()) +localText.getText("TXT_KEY_REV_CAP_VOTE_MARCH",())

							#iBuyOffCost = (50 + 15*pPlayer.getCurrentEra())*len(revCities) + GAME.getSorenRandNum(150+15*pPlayer.getCurrentEra(),'Rev')
							totalRevIdx = 0
							for pCity in revCities :
								totalRevIdx += pCity.getRevolutionIndex()
							iBuyOffCost = totalRevIdx/(20-pPlayer.getCurrentEra()) + GAME.getSorenRandNum(100+10*pPlayer.getCurrentEra(),'Rev')
							if( not pPlayer.isHuman() ) : iBuyOffCost = int( iBuyOffCost*.7 )
							iBuyOffCost = max( [iBuyOffCost, pPlayer.getGold()/8 + GAME.getSorenRandNum(50,'Rev')] )
							bodStr += '\n\n' + localText.getText("TXT_KEY_REV_PEACEFUL_CONCLUSION",())
							bodStr += '  ' + localText.getText("TXT_KEY_REV_CAP_VOTE_POWER",())
							bodStr += "  " + localText.getText("TXT_KEY_REV_BRIBE",()) + "  " + localText.getText("TXT_KEY_REV_CAP_VOTE_STATUES",())

							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %d cities in revolution, buyoff cost %d"%(len(revCities),iBuyOffCost))
							assert( len(revCities) > 0 )
							specialDataDict = { 'iNewCivic' : newCivic, 'iBuyOffCost' : iBuyOffCost }
							cityIdxs = []
							for pCity in revCities :
								cityIdxs.append( pCity.getID() )
							revData = RevDefs.RevoltData( pPlayer.getID(), GAME.getGameTurn(), cityIdxs, 'civics', bPeaceful, specialDataDict )

							revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
							iRevoltIdx = len(revoltDict.keys())
							revoltDict[iRevoltIdx] = revData
							RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

							self.makeRevolutionDecision( pPlayer, iRevoltIdx, revCities, 'civics', bPeaceful, bodStr )

							return

					[bCanDoFreeSpeech,newCivic] = RevUtils.canDoFreeSpeech( iPlayer )
					if( bCanDoFreeSpeech and not newCivic == None ) :
						if( 50 > GAME.getSorenRandNum(100, 'Revolt - free speech request') ):
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Asking change to Free Speech, %d"%(newCivic))

							bodStr = getCityTextList(revCities, bPreCity = True, bPostIs = True)

							#iBuyOffCost = (50 + 10*pPlayer.getCurrentEra())*len(revCities) + GAME.getSorenRandNum(50+10*pPlayer.getCurrentEra(),'Rev')
							totalRevIdx = 0
							for pCity in revCities :
								totalRevIdx += pCity.getRevolutionIndex()
							iBuyOffCost = totalRevIdx/(20-pPlayer.getCurrentEra()) + GAME.getSorenRandNum(50+12*pPlayer.getCurrentEra(),'Rev')
							if( not pPlayer.isHuman() ) : iBuyOffCost = int( iBuyOffCost*.7 )
							iBuyOffCost = max( [iBuyOffCost, pPlayer.getGold()/10 + GAME.getSorenRandNum(50,'Rev')] )
							bodStr += ' ' + localText.getText("TXT_KEY_REV_CAP_SPEECH_REQUEST",()) + ' %s.'%(GC.getCivicInfo(newCivic).getDescription())
							bodStr += '  ' + localText.getText("TXT_KEY_REV_CAP_SPEECH_DENY",())

							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %d cities in revolution, buyoff cost %d"%(len(revCities),iBuyOffCost))
							assert( len(revCities) > 0 )
							specialDataDict = { 'iNewCivic' : newCivic, 'iBuyOffCost' : iBuyOffCost }
							cityIdxs = []
							for pCity in revCities :
								cityIdxs.append( pCity.getID() )
							revData = RevDefs.RevoltData( pPlayer.getID(), GAME.getGameTurn(), cityIdxs, 'civics', bPeaceful, specialDataDict )

							revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
							iRevoltIdx = len(revoltDict.keys())
							revoltDict[iRevoltIdx] = revData
							RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

							self.makeRevolutionDecision( pPlayer, iRevoltIdx, revCities, 'civics', bPeaceful, bodStr )

							return

				if( self.leaderRevolution and not bReinstatedOnRevolution ) : #and (len(revCities) == pPlayer.getNumCities() or len(revCities) > (pPlayer.getNumCities()+1)/3) ) :
					# All or most cities in revolt
					if( not pPlayer.isHuman() or self.humanLeaderRevolution ) :
						# Ask for change of leader

						bodStr += ' ' + localText.getText("TXT_KEY_REV_CAP_LEAD_REQUEST",())
						if( not RevUtils.isCanDoElections( iPlayer ) ) :
							if( 70 > GAME.getSorenRandNum(100, 'Revolt - cede power request') or len(revCities) == pPlayer.getNumCities() ):

								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Asking for change in leader from %s"%(pPlayer.getName()))
								[newLeaderType,newLeaderName] = self.chooseRevolutionLeader( revCities )

								bIsElection = False
								#iBuyOffCost = (50 + 15*pPlayer.getCurrentEra())*len(revCities) + GAME.getSorenRandNum(100+10*pPlayer.getCurrentEra(),'Rev')
								totalRevIdx = 0
								for pCity in revCities :
									totalRevIdx += pCity.getRevolutionIndex()
								iBuyOffCost = totalRevIdx/(20-pPlayer.getCurrentEra()) + GAME.getSorenRandNum(80+10*pPlayer.getCurrentEra(),'Rev')
								if( not pPlayer.isHuman() ) : iBuyOffCost = int( iBuyOffCost*.7 )
								iBuyOffCost = max( [iBuyOffCost, pPlayer.getGold()/10 + GAME.getSorenRandNum(50,'Rev')] )
								bodStr += '  ' + localText.getText("TXT_KEY_REV_CAP_LEAD_CEDE",()) + ' %s.'%(newLeaderName)
								bodStr += '\n\n' + localText.getText("TXT_KEY_REV_CAP_LEAD_CONCLUSION",())
								bodStr += '  %s '%(newLeaderName) + localText.getText("TXT_KEY_REV_CAP_LEAD_RULER",())

								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %d cities in revolution, buyoff cost %d"%(len(revCities),iBuyOffCost))
								assert( len(revCities) > 0 )
								specialDataDict = { 'iNewLeaderType' : newLeaderType, 'newLeaderName' : newLeaderName, 'bIsElection' : bIsElection, 'iBuyOffCost' : iBuyOffCost }
								cityIdxs = []
								for pCity in revCities :
									cityIdxs.append( pCity.getID() )
								revData = RevDefs.RevoltData( pPlayer.getID(), GAME.getGameTurn(), cityIdxs, 'leader', bPeaceful, specialDataDict )

								revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
								iRevoltIdx = len(revoltDict.keys())
								revoltDict[iRevoltIdx] = revData
								RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

								self.makeRevolutionDecision( pPlayer, iRevoltIdx, revCities, 'leader', bPeaceful, bodStr )

								return

						else :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Asking for %s to face election"%(pPlayer.getName()))
							[newLeaderType,newLeaderName] = self.chooseRevolutionLeader( revCities )

							bIsElection = True
							#iBuyOffCost = (30 + 10*pPlayer.getCurrentEra())*len(revCities) + GAME.getSorenRandNum(100+10*pPlayer.getCurrentEra(),'Rev')
							totalRevIdx = 0
							for pCity in revCities :
								totalRevIdx += pCity.getRevolutionIndex()
							iBuyOffCost = totalRevIdx/(25-pPlayer.getCurrentEra()) + GAME.getSorenRandNum(90+10*pPlayer.getCurrentEra(),'Rev')
							if( not pPlayer.isHuman() ) : iBuyOffCost = int( iBuyOffCost*.7 )
							iBuyOffCost = max( [iBuyOffCost, pPlayer.getGold()/12 + GAME.getSorenRandNum(50,'Rev')] )
							bodStr += '  ' + localText.getText("TXT_KEY_REV_CAP_LEAD_ELECTION",()) + ' %s!'%(newLeaderName)
							bodStr += '\n\n' + localText.getText("TXT_KEY_REV_PEACEFUL_CONCLUSION",())
							bodStr += '\n\n' + localText.getText("TXT_KEY_REV_CAP_LEAD_BUYOFF",())

							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %d cities in revolution, buyoff cost %d"%(len(revCities),iBuyOffCost))
							assert( len(revCities) > 0 )
							specialDataDict = { 'iNewLeaderType' : newLeaderType, 'newLeaderName' : newLeaderName, 'bIsElection' : bIsElection, 'iBuyOffCost' : iBuyOffCost }
							cityIdxs = []
							for pCity in revCities :
								cityIdxs.append( pCity.getID() )
							revData = RevDefs.RevoltData( pPlayer.getID(), GAME.getGameTurn(), cityIdxs, 'leader', bPeaceful, specialDataDict )

							revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
							iRevoltIdx = len(revoltDict.keys())
							revoltDict[iRevoltIdx] = revData
							RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

							self.makeRevolutionDecision( pPlayer, iRevoltIdx, revCities, 'leader', bPeaceful, bodStr )

							return

			else :
				# If violent, demand change of leader!
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Capital or majority of cities in violent revolution!")

				if( self.leaderRevolution ) : #and (len(revCities) > 1 or len(revCities) == pPlayer.getNumCities()) ) :
					if( not pPlayer.isHuman() or self.humanLeaderRevolution ) :

						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Demanding change in leader from %s"%(pPlayer.getName()))

						[newLeaderType,newLeaderName] = self.chooseRevolutionLeader( revCities )

						if( pPlayer.isStateReligion() ) :
							giveRelType = pPlayer.getStateReligion()
						else :
							giveRelType = None

						[pRevPlayer,bIsJoinWar] = self.chooseRevolutionCiv( revCities, bJoinCultureWar = False, bReincarnate = True, bJoinRebels = True, bSpreadRebels = False, giveRelType = giveRelType, bMatchCivics = True )

						bodStr += ' ' + localText.getText("TXT_KEY_REV_CAP_RULE_DEMAND",())
						if( not RevUtils.isCanDoElections( iPlayer ) ) :
							bIsElection = False
							iBuyOffCost = -1
							bodStr += '  ' + localText.getText("TXT_KEY_REV_CAP_RULE_CEDE",()) + ' %s.'%(newLeaderName)
							bodStr += '\n\n' + localText.getText("TXT_KEY_REV_CAP_RULE_TERRIBLE",())
							bodStr += ' %s '%(pRevPlayer.getCivilizationShortDescription(0)) + localText.getText("TXT_KEY_REV_CAP_RULE_FIGHT",())
							bodStr += '  %s '%(newLeaderName) + localText.getText("TXT_KEY_REV_CAP_LEAD_RULER",())

						else :
							bIsElection = True
							#iBuyOffCost = (50 + 12*pPlayer.getCurrentEra())*len(revCities) + GAME.getSorenRandNum(100+10*pPlayer.getCurrentEra(),'Rev')
							totalRevIdx = 0
							for pCity in revCities :
								totalRevIdx += pCity.getRevolutionIndex()
							iBuyOffCost = totalRevIdx/(22-pPlayer.getCurrentEra()) + GAME.getSorenRandNum(80+10*pPlayer.getCurrentEra(),'Rev')
							if( not pPlayer.isHuman() ) : iBuyOffCost = int( iBuyOffCost*.7 )
							iBuyOffCost = max( [iBuyOffCost, pPlayer.getGold()/8 + GAME.getSorenRandNum(50,'Rev')] )
							bodStr += '  ' + localText.getText("TXT_KEY_REV_CAP_RULE_ELECTION",()) + ' %s!'%(newLeaderName)
							bodStr += '\n\n' + localText.getText("TXT_KEY_REV_CAP_RULE_RISE",()) + ' %s '%(pRevPlayer.getCivilizationShortDescription(0))
							bodStr += localText.getText("TXT_KEY_REV_CAP_RULE_BUYOFF",())

						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %d cities in revolution, buyoff cost %d"%(len(revCities),iBuyOffCost))
						assert( len(revCities) > 0 )
						specialDataDict = { 'iNewLeaderType' : newLeaderType, 'newLeaderName' : newLeaderName, 'bIsElection' : bIsElection, 'iBuyOffCost' : iBuyOffCost, 'iRevPlayer' : pRevPlayer.getID(), 'bIsJoinWar' : bIsJoinWar }
						cityIdxs = []
						for pCity in revCities :
							cityIdxs.append( pCity.getID() )
						revData = RevDefs.RevoltData( pPlayer.getID(), GAME.getGameTurn(), cityIdxs, 'leader', bPeaceful, specialDataDict )

						revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
						iRevoltIdx = len(revoltDict.keys())
						revoltDict[iRevoltIdx] = revData
						RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

						self.makeRevolutionDecision( pPlayer, iRevoltIdx, revCities, 'leader', bPeaceful, bodStr )

						return

#-------- Default to ask/demand independence
		if( not self.independenceRevolution ) :
			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: WARNING: default ask for independence has been disabled!")
			return

		if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Default: ask/demand independence!")

		# Prune for only close cities, Cities in area may be quite far away
		indCities = []
		for pCity in revCities :
			# Add only cities near instigator in first pass
			cityDist = plotDistance( pCity.getX(), pCity.getY(), instigator.getX(), instigator.getY() )
			if( cityDist <= self.closeRadius ) :
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s is close enough to instigator to join in independence quest"%(pCity.getName()))
				indCities.append(pCity)

		for pCity in revCities :
			if( not pCity in indCities ) :
				# Add cities a little further away that are also near another rebelling city
				cityDist = plotDistance( pCity.getX(), pCity.getY(), instigator.getX(), instigator.getY() )
				if( cityDist <= 2.0*self.closeRadius ) :
					for iCity in indCities :
						cityDist = min([cityDist, plotDistance( pCity.getX(), pCity.getY(), iCity.getX(), iCity.getY() )])

					if( cityDist <= 0.8*self.closeRadius ) :
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s is close enough to another rebelling city to join in independence quest"%(pCity.getName()))
						indCities.append(pCity)

		bodStr = getCityTextList(indCities, bPreCity = True, bPostIs = True)

		iBuyOffCost = -1

		if( bPeaceful ) :
			if( pPlayer.isStateReligion() ) :
				if( 70 > GAME.getSorenRandNum(100,'Rev') ) :
					giveRelType = pPlayer.getStateReligion()
				else :
					giveRelType = -1
			else :
				if( 50 > GAME.getSorenRandNum(100,'Rev') ) :
					giveRelType = None
				else :
					giveRelType = -1
			[pRevPlayer,bIsJoinWar] = self.chooseRevolutionCiv( indCities, bJoinCultureWar = False, bReincarnate = True, bJoinRebels = True, bSpreadRebels = True, giveRelType = giveRelType, bMatchCivics = True )
			#iBuyOffCost = (100 + 20*pPlayer.getCurrentEra())*len(indCities) + GAME.getSorenRandNum(100+20*pPlayer.getCurrentEra(),'Rev')
			totalRevIdx = 0
			totalPop = 0
			for pCity in indCities :
				totalRevIdx += pCity.getRevolutionIndex()
				totalPop += pCity.getPopulation()
			iBuyOffCost = totalRevIdx/(17-pPlayer.getCurrentEra()) + GAME.getSorenRandNum(50+10*pPlayer.getCurrentEra(),'Rev')
			if( not pPlayer.isHuman() ) : iBuyOffCost = int( iBuyOffCost*.7 )
			iBuyOffCost = max( [iBuyOffCost, pPlayer.getGold()/8 + GAME.getSorenRandNum(50,'Rev')] )

			# Determine Vassal or no
			# if annoyed or worse, ask for independence
			# if tiny or really like, ask capitulation
			# othwise ask for free vassal
			vassalStyle = None
			if( not pTeam.isAVassal() and pTeam.isVassalStateTrading() ) :
				if( pRevPlayer.AI_getAttitude(pPlayer.getID()) == AttitudeTypes.ATTITUDE_FURIOUS or pRevPlayer.AI_getAttitude(pPlayer.getID()) == AttitudeTypes.ATTITUDE_ANNOYED ) :
					vassalStyle = None
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Vassal style: No due to attitude")
				elif( totalPop < pPlayer.getTotalPopulation()/3 ) :
					if( pRevPlayer.AI_getAttitude(pPlayer.getID()) == AttitudeTypes.ATTITUDE_FRIENDLY or pRevPlayer.AI_getAttitude(pPlayer.getID()) == AttitudeTypes.ATTITUDE_PLEASED ) :
						vassalStyle = 'capitulated'
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Vassal style %s chosen from size and attitude"%(vassalStyle))
					elif( totalPop < pPlayer.getTotalPopulation()/6 ) :
						vassalStyle = 'capitulated'
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Vassal style %s chosen from size"%(vassalStyle))
					else :
						vassalStyle = 'free'
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Vassal style %s chosen from size"%(vassalStyle))
				elif( pRevPlayer.AI_getAttitude(pPlayer.getID()) == AttitudeTypes.ATTITUDE_FURIOUS or pRevPlayer.AI_getAttitude(pPlayer.getID()) == AttitudeTypes.ATTITUDE_FURIOUS ) :
					vassalStyle = 'free'
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Vassal style %s chosen from attitude"%(vassalStyle))


			if( not vassalStyle == None ) :
				bodStr += ' ' + localText.getText("TXT_KEY_REV_IND_PEACE_VASSAL",())
				bodStr += ' %s '%(pRevPlayer.getCivilizationShortDescription(0)) + localText.getText("TXT_KEY_REV_IND_PEACE_BECOME",())
				if( vassalStyle == 'free' ) :
					bodStr += ' %s '%(localText.getText("TXT_KEY_REV_VASSAL_FREE",()))
				else :
					bodStr += ' %s '%(localText.getText("TXT_KEY_REV_VASSAL_CAP",()))
				bodStr += localText.getText("TXT_KEY_REV_IND_PEACE_GRANT",())
			else :
				if( pRevPlayer.isAlive() ) :
					bodStr += ' ' + localText.getText("TXT_KEY_REV_IND_PEACE_REQUEST_ALIVE",())%(pRevPlayer.getCivilizationShortDescription(0))
				else :
					bodStr += ' ' + localText.getText("TXT_KEY_REV_IND_PEACE_REQUEST",()) + ' %s.'%(pRevPlayer.getCivilizationShortDescription(0))
			bodStr += '\n\n' + localText.getText("TXT_KEY_REV_IND_PEACE_ACTION",())
			if pPlayer.getGold() > iBuyOffCost:
				bodStr += '\n\n' + localText.getText("TXT_KEY_REV_IND_PEACE_ACTION2",())

		else :
			[pRevPlayer,bIsJoinWar] = self.chooseRevolutionCiv( indCities, bJoinCultureWar = False, bReincarnate = True, bJoinRebels = True, bSpreadRebels = False )
			vassalStyle = None
			if( self.allowSmallBarbRevs and len(indCities) == 1 ) :
				if( instRevIdx < int(1.4*self.revInstigatorThreshold) ) :
					if( not instigator.area().isBorderObstacle(pPlayer.getTeam()) ) :
						pRevPlayer = GC.getPlayer( GC.getBARBARIAN_PLAYER() )
						bIsJoinWar = False
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Small, disorganized Revolution")
			bodStr += ' ' + localText.getText("TXT_KEY_REV_IND_VIOLENT_DEMAND",())
			if( pRevPlayer.isBarbarian() ) :
				bodStr += " " + localText.getText("TXT_KEY_REV_IND_VIOLENT_BARB",())
			else :
				bodStr += ' ' + localText.getText("TXT_KEY_REV_IND_VIOLENT_FORM",()) + ' %s.'%(pRevPlayer.getCivilizationShortDescription(0))
			bodStr += '\n\n' + localText.getText("TXT_KEY_REV_IND_VIOLENT_FIGHT",())

		if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %d cities in revolution, buyoff cost %d"%(len(indCities),iBuyOffCost))
		assert( len(indCities) > 0 )
		specialDataDict = { 'iRevPlayer' : pRevPlayer.getID(), 'bIsJoinWar' : bIsJoinWar, 'iBuyOffCost' : iBuyOffCost, 'vassalStyle' : vassalStyle }
		cityIdxs = []
		for pCity in indCities :
			cityIdxs.append( pCity.getID() )
		revData = RevDefs.RevoltData( pPlayer.getID(), GAME.getGameTurn(), cityIdxs, 'independence', bPeaceful, specialDataDict )

		revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
		iRevoltIdx = len(revoltDict.keys())
		revoltDict[iRevoltIdx] = revData
		RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

		self.makeRevolutionDecision( pPlayer, iRevoltIdx, indCities, 'independence', bPeaceful, bodStr )

		return


	def chooseRevolutionCiv(self, cityList, bJoinCultureWar = True, bReincarnate = True, bJoinRebels = True, bSpreadRebels = False, pNotThisCiv = None, giveTechs = True, giveRelType = -1, bMatchCivics = False):
		# All cities should have same owner

		if self.bRebelTypes:
			RebelTypes.setup()

		pRevPlayer = None
		bIsJoinWar = False

		owner = GC.getPlayer( cityList[0].getOwner() )
		ownerTeam = GC.getTeam( owner.getTeam() )

		# TODO:  Turn into a pick best option as opposed to first option
		# Attempt to find a worthy civ to reincarnate from these cities
		instigator = cityList[0]
		closeCityList = []
		for pCity in cityList :
			if( plotDistance( pCity.getX(), pCity.getY(), instigator.getX(), instigator.getY() ) < 0.7*self.closeRadius ) :
				closeCityList.append( pCity )

		for pCity in closeCityList :

			if( pRevPlayer == None ) :

				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Looking for revolution worthy civ in %s"%(pCity.getName()))

				cultPlayer = None
				if( pCity.countTotalCultureTimes100() > 50*100 ) :
					cultPlayer = GC.getPlayer( pCity.findHighestCulture() )
					if( cultPlayer.getID() == owner.getID() or cultPlayer.isNPC() or cultPlayer.isMinorCiv() ) :
						cultPlayer = None

				if( bJoinCultureWar and not cultPlayer == None ) :
					# If at war with significant culture, join them
					if( ownerTeam.getAtWarCount(True) > 0 ) :
						if( ownerTeam.isAtWar(cultPlayer.getID()) and cultPlayer.isAlive() ) :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Owner at war with dominant culture player %s"%(cultPlayer.getCivilizationDescription(0)))
							pRevPlayer = cultPlayer
							if( cultPlayer.getNumCities() > 2 ) :
								bIsJoinWar = True

				if( bJoinRebels and pRevPlayer == None ) :
					# If at war with this cities rebel civ type, join them
					revCivType = RevData.getCityVal(pCity, 'RevolutionCiv')
					if( revCivType >= 0 ) :
						for i in xrange(GC.getMAX_PC_PLAYERS()) :
							if( not i == owner.getID() ) :
								playerI = GC.getPlayer( i )
								if( playerI.isAlive() and playerI.getCivilizationType() == revCivType ) :
									if( ownerTeam.isAtWar(playerI.getTeam()) and ownerTeam.isHasMet(playerI.getTeam()) ) :
										if( RevUtils.getNumDefendersNearPlot( pCity.getX(), pCity.getY(), i, iRange = 5, bIncludePlot = True, bIncludeCities = True ) ) :
											pRevPlayer = playerI
											if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Owner at war with cities revolt civ type, player %s"%(pRevPlayer.getCivilizationDescription(0)))

				if( bReincarnate and pRevPlayer == None ) :
					# Check for civ that can rise from the ashes
					for i in xrange(GC.getMAX_PC_PLAYERS()) :
						if( not i == owner.getID() ) :
							playerI = GC.getPlayer( i )
							if( (not playerI.isAlive()) and (pCity.getCulture( i ) > 50) ) :
								pRevPlayer = playerI
								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Reincarnating player %s"%(pRevPlayer.getCivilizationDescription(0)))
								break

		# Search around all cities for a rebellion that wants to spill over into this territory
		if( bSpreadRebels and pRevPlayer == None ) :
			#if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Checking for rebellions that could spill over")

			rebelIDList = []
			for i in xrange(GC.getMAX_PC_PLAYERS()) :
				if( not i == owner.getID() and pCity.area().getUnitsPerPlayer(i) > 0 ) :
					playerI = GC.getPlayer(i)
					teamI = GC.getTeam( playerI.getTeam() )
					relations = playerI.AI_getAttitude(owner.getID())

					if( playerI.isRebel() and teamI.canDeclareWar(ownerTeam.getID()) ) :
						if( not playerI.isFoundedFirstCity() ) :  # Is a homeless rebel
							if( playerI.getCivilizationType() == RevData.getCityVal(pCity, 'RevolutionCiv') ) :
								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Homeless rebel (type) %s in area"%(playerI.getCivilizationDescription(0)))
								rebelIDList.append(i)
							elif( teamI.isAtWar(ownerTeam.getID()) ) :
								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Homeless rebel (at war) %s in area"%(playerI.getCivilizationDescription(0)))
								rebelIDList.append(i)
							elif( relations == AttitudeTypes.ATTITUDE_FURIOUS or relations == AttitudeTypes.ATTITUDE_ANNOYED ) :
								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Homeless rebel (attitude) %s in area"%(playerI.getCivilizationDescription(0)))
								rebelIDList.append(i)

						if playerI.getCitiesLost() < 3 and playerI.getNumCities() < 4:
							pCapital = playerI.getCapitalCity()
							if pCapital is not None and GAME.getGameTurn() - pCapital.getGameTurnAcquired() < 30 \
							and not GC.getPlayer(pCapital.getPreviousOwner()).isNPC():

								if (playerI.getCivilizationType() == RevData.getCityVal(pCity, 'RevolutionCiv')):
									if (self.LOG_DEBUG):
										CvUtil.pyPrint("[REV] Revolt: Young rebel (type) %s in area"%(playerI.getCivilizationDescription(0)))
									rebelIDList.append(i)
								elif (teamI.isAtWar(ownerTeam.getID())):
									if (self.LOG_DEBUG):
										CvUtil.pyPrint("[REV] Revolt: Young rebel (at war) %s in area"%(playerI.getCivilizationDescription(0)))
									rebelIDList.append(i)
								elif (relations == AttitudeTypes.ATTITUDE_FURIOUS or relations == AttitudeTypes.ATTITUDE_ANNOYED):
									if (self.LOG_DEBUG):
										CvUtil.pyPrint("[REV] Revolt: Young rebel (attitude) %s in area"%(playerI.getCivilizationDescription(0)))
									rebelIDList.append(i)

			if( len(rebelIDList) > 0 ) :
				for pCity in closeCityList :
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Checking around %s for other rebellion"%(pCity.getName()))

					for [radius,plotI] in RevUtils.plotGenerator( pCity.plot(), 6 ) :
						for rebelID in rebelIDList :
							if( plotI.getNumDefenders(rebelID) > 0 ) :
								pRevPlayer = GC.getPlayer(rebelID)
								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Found %s within %d, expanding their rebellion!"%(pRevPlayer.getCivilizationDescription(0),radius))
								break

						if( not pRevPlayer == None ) :
							break

					if( not pRevPlayer == None ) :
							break

		# Search around all cities in list for a dead player to reincarnate
		if( pRevPlayer == None and bReincarnate ) :
			if( GAME.countCivPlayersAlive() < GAME.countCivPlayersEverAlive() ) :

				deadCivs = []
				for idx in xrange(GC.getMAX_PC_PLAYERS()) :
					playerI = GC.getPlayer(idx)
					if( not playerI.isAlive() and playerI.isEverAlive() ) :
						# TODO: Should this also check for revData?
						deadCivs.append(idx)

				for pCity in closeCityList :
					if( pRevPlayer == None ) :
						for civIdx in deadCivs :
							playerI = GC.getPlayer(civIdx)
							if( playerI.getCivilizationType() == RevData.getCityVal(pCity, 'RevolutionCiv') ) :
								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Reincarnation %s's rev civ, the %s"%(pCity.getName(),playerI.getCivilizationDescription(0)))
								pRevPlayer = playerI
								break

				for pCity in closeCityList :
					if( pRevPlayer == None ) :

						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Checking around %s for dead player"%(pCity.getName()))
						maxCult = 0
						maxCultRad = 5
						for [radius,plotI] in RevUtils.plotGenerator( pCity.plot(), 4 ) :

							if( not pRevPlayer == None and radius > maxCultRad ) :
								break

							for civIdx in deadCivs :
								if( plotI.getCulture(civIdx) > maxCult ) :
									if( self.LOG_DEBUG ) : CvUtil.pyPrint("Revolt: Found plot culture from dead player %d"%(civIdx))
									maxCult = plotI.getCulture(civIdx)
									maxCultRad = radius
									pRevPlayer = GC.getPlayer(civIdx)

		# Create new civ based on culture/owner of first city in list
		if pRevPlayer == None:

			pCity = cityList[0]
			owner = GC.getPlayer(pCity.getOwner())
			ownerTeam = GC.getTeam(owner.getTeam())

			# Search for empty slot
			newPlayerIdx = -1
			for i in xrange(GC.getMAX_PC_PLAYERS()):
				if not GC.getPlayer(i).isEverAlive() and not RevData.revObjectExists(GC.getPlayer(i)):
					newPlayerIdx = i
					print "[REV] Revolt: Creating new player in slot " + str(i)
					break
			else:
				print "[REV] Revolt: No available slots, spawning as Barbarians"
				return [GC.getPlayer(GC.getBARBARIAN_PLAYER()), bIsJoinWar]


			# Create list of available civs and similar civ types
			cultPlayer = None
			if pCity.countTotalCultureTimes100() > 5000:
				iHighestCulturePlayer = pCity.findHighestCulture()
				if iHighestCulturePlayer != owner.getID():
					cultPlayer = GC.getPlayer(iHighestCulturePlayer)

			# Don't incarnate as either of these
			iBarbarian = GC.getInfoTypeForString(RevDefs.sXMLBarbarian)
			# Civs not currently in the game
			availableCivs = []
			# Civs with similar style to cultOwner, if they exist
			similarStyleCivs = []
			similarOwnerStyleCivs = []
			for iCivX in xrange(GC.getNumCivilizationInfos()):
				civX = GC.getCivilizationInfo(iCivX)
				if not civX.isPlayable(): continue

				for i in xrange(GC.getMAX_PC_PLAYERS()):
					# Switch in preparation for defining regions of the world for different rebel civ types
					if (iCivX == GC.getPlayer(i).getCivilizationType()
					and (GC.getPlayer(i).isEverAlive() or RevData.revObjectExists(GC.getPlayer(i)))
					): break
				else:
					availableCivs.append(iCivX)
					if cultPlayer:
						if GC.getCivilizationInfo(cultPlayer.getCivilizationType()).getArtStyleType() == civX.getArtStyleType():
							similarStyleCivs.append(iCivX)
					if GC.getCivilizationInfo(owner.getCivilizationType()).getArtStyleType() == civX.getArtStyleType():
						similarOwnerStyleCivs.append(iCivX)

			if not availableCivs:
				print "[REV] Revolt: No available civs, spawning as Barbarians"
				pRevPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
				return [pRevPlayer, bIsJoinWar]

			newCivIdx = None

			# If city has a revolutionary civ type, if that type is available choose it
			if RevData.getCityVal(pCity, 'RevolutionCiv') in availableCivs:
				# City previously rebelled as available civ type
				if self.LOG_DEBUG:
					print "[REV] Revolt: Respawning previous rebel civ type for this city"
				newCivIdx = RevData.getCityVal(pCity, 'RevolutionCiv')
			else:
				if self.bRebelTypes:
					chosenCivs = []

					if cultPlayer:
						iCiv = cultPlayer.getCivilizationType()
					else: iCiv = owner.getCivilizationType()

					rebList = RebelTypes.RebelTypeList[iCiv]
					for civType in rebList:
						if civType in availableCivs:
							chosenCivs.append(civType)

					if chosenCivs:
						availableCivs = chosenCivs
				else:
					if self.bArtStyleTypes:
						if similarStyleCivs:
							# Create a civ using style of cultural owner of city
							availableCivs = similarStyleCivs

						elif similarOwnerStyleCivs:
							# Create a civ using style of owner of city
							availableCivs = similarOwnerStyleCivs

				newCivIdx = availableCivs[GAME.getSorenRandNum(len(availableCivs),'Revolution: pick unused civ type')]

			leaderList = []
			for leaderType in xrange(GC.getNumLeaderHeadInfos()):
				if GC.getCivilizationInfo(newCivIdx).isLeaders(leaderType) or GAME.isOption(GameOptionTypes.GAMEOPTION_LEAD_ANY_CIV):
					for jdx in xrange(GC.getMAX_PC_PLAYERS()):
						if GC.getPlayer(jdx).getLeaderType() == leaderType and not newPlayerIdx == jdx:
							break
					else: leaderList.append(leaderType)

			if not leaderList:
				print "[INFO] Unexpected lack of possible leaders, spawning as Barbarians"
				pRevPlayer = GC.getPlayer(GC.getBARBARIAN_PLAYER())
				return [pRevPlayer, bIsJoinWar]

			newLeaderIdx = leaderList[GAME.getSorenRandNum(len(leaderList), 'Revolution: pick leader')]

			GAME.addPlayer(newPlayerIdx, newLeaderIdx, newCivIdx, False)

			pRevPlayer = GC.getPlayer(newPlayerIdx)

			print "[REV] Revolt: Created the %s in slot %d" %(pRevPlayer.getCivilizationDescription(0), newPlayerIdx)

		# Do special setup for non-living revolutionaries ...

		if giveTechs and not pRevPlayer.isAlive() and not pRevPlayer.isBarbarian():
			RevUtils.giveTechs(pRevPlayer, owner)

		if( not giveRelType == None and not pRevPlayer.isAlive() and not pRevPlayer.isBarbarian() ) :
			if giveRelType >= 0:
				# Give specified religion
				pRevPlayer.setLastStateReligion( giveRelType )
			else:
				# Give minority religion in city
				availRels = []
				for relType in xrange(GC.getNumReligionInfos()) :
					if( not relType == owner.getStateReligion() ) :
						if( pCity.isHolyCityByType(relType) ) :
							giveRelType = relType
						elif( pCity.isHasReligion(relType) ) :
							availRels.append( relType )

				if( giveRelType < 0 ) :
					if( len(availRels) > 0 ) :
						giveRelType = availRels[GAME.getSorenRandNum(len(availRels),'Revolution: pick religion')]

				if( giveRelType >= 0 ) :
					pRevPlayer.setLastStateReligion( giveRelType )

		if( bMatchCivics ) :
			pPlayer = GC.getPlayer( pCity.getOwner() )
			for civicOptionID in xrange(GC.getNumCivicOptionInfos()) :
				#civicOption = GC.getCivicOptionInfo(civicOptionID)
				civicType = pPlayer.getCivics(civicOptionID)
				if( pRevPlayer.canDoCivics( civicType ) ) :
					pRevPlayer.setCivics( civicOptionID, civicType )

		if( not pRevPlayer.isAlive() and GAME.countCivPlayersAlive() >= self.maxCivs ) :
			pRevPlayer = GC.getPlayer( GC.getBARBARIAN_PLAYER() )
			return [pRevPlayer, False]

		return [pRevPlayer, bIsJoinWar]


	def chooseRevolutionLeader(self, cityList):

		newLeaderType = None
		newLeaderName = None

		owner = GC.getPlayer(cityList[0].getOwner())
		ownerCivType = owner.getCivilizationType()
		ownerLeaderType = owner.getLeaderType()
		ownerCivInfo = GC.getCivilizationInfo(ownerCivType)

		# Use new leader type
		count = 0
		availLeader = []
		for i in xrange(GC.getNumLeaderHeadInfos()):
			if ownerCivInfo.isLeaders(i) or GAME.isOption(GameOptionTypes.GAMEOPTION_LEAD_ANY_CIV):
				for j in xrange(GC.getMAX_PC_PLAYERS()):
					if GC.getPlayer(j).getLeaderType() == i:
						break
				else:
					count += 1
					availLeader.append(i)

		if availLeader:
			newLeaderType = availLeader[GAME.getSorenRandNum(count,'Revolution: pick leader')]
			newLeaderName = GC.getLeaderHeadInfo(newLeaderType).getDescription()

		if newLeaderType == None:
			# Use same leader type, but with new name
			newLeaderType = ownerLeaderType
			newLeaderName = TextUtil.convertToStr(GC.getLeaderHeadInfo( newLeaderType ).getDescription())

			if newLeaderName == owner.getName():
				# Hack Roman numeral naming
				if newLeaderName[-3:len(newLeaderName)] == ' II':
					newLeaderName = newLeaderName + 'I'
				elif newLeaderName[-2:len(newLeaderName)] == ' I':
					newLeaderName = newLeaderName + 'I'
				else:
					newLeaderName = newLeaderName + ' II'

		return [newLeaderType, newLeaderName]


##--- Revolution decision functions ------------------------------------------

	def makeRevolutionDecision( self, pPlayer, iRevoltIdx, cityList, revType, bPeaceful, bodStr ) :
		revData = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )[iRevoltIdx]

#-------------------------------------------------------------------------------------------------
# Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
				# Enable only for debugging different revolution styles
		if( self.LOG_DEBUG and self.isHumanPlayerOrAutoPlay(pPlayer.getID()) and GAME.getAIAutoPlay(pPlayer.getID()) > 0 ) :
			bCanCancelAuto = SdToolKit.sdObjectGetVal( "AIAutoPlay", GAME, "bCanCancelAuto" )
			GAME.setForcedAIAutoPlay(pPlayer.getID(), 0, False)
#-------------------------------------------------------------------------------------------------
# END Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------

		pTeam = GC.getTeam( pPlayer.getTeam() )
		iAggressive = GC.getInfoTypeForString(RevDefs.sXMLAggressive)
		iSpiritual = GC.getInfoTypeForString(RevDefs.sXMLSpiritual)
		numRevCities = len(revData.cityList)

		pRevPlayer = None
		bIsBarbRev = False
		if( 'iRevPlayer' in revData.dict.keys() ) :
			pRevPlayer = GC.getPlayer(revData.dict['iRevPlayer'])
			bIsBarbRev = pRevPlayer.isBarbarian()

		isRevType = True

		if( revType == 'civics' ) :

			iNewCivic = revData.dict['iNewCivic']
			iBuyOffCost = revData.dict.get( 'iBuyOffCost', -1 )

			if( bPeaceful ) :
				if( numRevCities > pPlayer.getNumCities()/2 ) :
					iOdds = 70
				elif( numRevCities < pPlayer.getNumCities()/4 ) :
					iOdds = 25
				else :
					iOdds = 40
			else :
				if( numRevCities > pPlayer.getNumCities()/2 ) :
					iOdds = 80
				elif( numRevCities < pPlayer.getNumCities()/4 ) :
					iOdds = 40
				else :
					iOdds = 60

			iOdds += self.RevOpt.getCivicsOdds()

			if( self.isLocalHumanPlayer(pPlayer.getID()) ) :
				# Offer choice
				# Additions by Caesium et al
				caesiumtR = CyUserProfile().getResolutionString(CyUserProfile().getResolution())
				caesiumtextResolution = caesiumtR.split('x')
				caesiumpasx = int(caesiumtextResolution[0])/10
				caesiumpasy = int(caesiumtextResolution[1])/10
				popup = CyPopup(RevDefs.revolutionPopup, EventContextTypes.EVENTCONTEXT_ALL, False)
				if self.centerPopups:
					popup.setPosition(3*caesiumpasx, 3*caesiumpasy)
				# End additions by Caesium et al
				if bPeaceful:
					popup.setHeaderString(localText.getText("TXT_KEY_REV_TITLE_PEACEFUL",()), 1<<2)
				else: popup.setHeaderString(localText.getText("TXT_KEY_REV_TITLE_VIOLENT",()), 1<<2)

				if iOdds >= 70:
					bodStr += "\n\n" + localText.getText("TXT_KEY_REV_ADVISOR_ACCEPT",())
				elif iOdds <= 30:
					bodStr += "\n\n" + localText.getText("TXT_KEY_REV_ADVISOR_REJECT",())
				else:
					bodStr += "\n\n" + localText.getText("TXT_KEY_REV_ADVISOR_NEUTRAL",())
				popup.setBodyString(bodStr, 1<<0)
				popup.addButton(localText.getText("TXT_KEY_REV_BUTTON_ACCEPT",()))
				buttons = ('accept',)
				popup.addButton(localText.getText("TXT_KEY_REV_BUTTON_REJECT",()))
				buttons += ('reject',)
				if iBuyOffCost > 0 and iBuyOffCost <= pPlayer.getGold():
					popup.addButton(localText.getText("TXT_KEY_REV_BUTTON_BUYOFF",()) + ' %d'%(iBuyOffCost))
					buttons += ('buyoff',)
				if not bPeaceful and not 'iJoinPlayer' in revData.dict.keys() and not bIsBarbRev and pRevPlayer.getNumCities() == 0 and self.offerDefectToRevs:
					popup.addButton(localText.getText("TXT_KEY_REV_BUTTON_DEFECT",()))
					buttons += ('defect',)

				popup.setUserData( (buttons, pPlayer.getID(), iRevoltIdx) )
				# Center camera on city
				CyCamera().JustLookAt( cityList[0].plot().getPoint() )
				popup.launch(False, PopupStates.POPUPSTATE_IMMEDIATE)

			elif (not pPlayer.isHuman()) :
				# Make AI decision

				if( GAME.getSorenRandNum(100,'Revolt: switch civics') < iOdds ) :
					self.processRevolution( pPlayer, iRevoltIdx, cityList, revType, bPeaceful, True )

				else :
					if iBuyOffCost > 0 and iBuyOffCost <= pPlayer.getGold():
						if( bPeaceful ) :
							base = .8
						else :
							base = .7

						buyOdds = int(iOdds/2.0 + 75*(1 - pow(base, pPlayer.getGold()/iBuyOffCost)))

						if( buyOdds > GAME.getSorenRandNum( 100, 'Rev - AI buyoff decision' ) ) :
							revData.dict['bDidBuyOff'] = True

							revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
							revoltDict[iRevoltIdx] = revData
							RevData.revObjectSetVal( pPlayer, 'RevoltDict', revoltDict )

							self.processRevolution( pPlayer, iRevoltIdx, cityList, revType, bPeaceful, False )
							return

					self.processRevolution( pPlayer, iRevoltIdx, cityList, revType, bPeaceful, False )

		elif( revType == 'religion' ) :

			iNewReligion = revData.dict['iNewReligion']
			iBuyOffCost = revData.dict.get( 'iBuyOffCost', -1 )

			if not pPlayer.isStateReligion():
				print "[WARNING] Revolt - AI has no state religion ... this shouldn't happen"
				return

			newCount = pPlayer.getHasReligionCount( iNewReligion )
			newHoly = pPlayer.hasHolyCity( iNewReligion )

			stateCount = pPlayer.getHasReligionCount( pPlayer.getStateReligion() )
			stateHoly = pPlayer.hasHolyCity( pPlayer.getStateReligion() )

			if( stateHoly and not newHoly ) :
				iOdds = 0
			elif( newHoly and not stateHoly ) :
				if( pPlayer.hasTrait( iSpiritual ) ) :
					# Spiritual player will spread religion pretty quickly
					if( stateCount > 2*newCount ) :
						iOdds = 60
					else :
						iOdds = 100
				else :
					if( stateCount > 2*newCount ) :
						iOdds = 30
					elif( newCount > stateCount ) :
						iOdds = 100
					else :
						iOdds = 70
			else :
				if( stateCount > 2*newCount ) :
					iOdds = 0
				elif( newCount > stateCount ) :
					iOdds = 70
				else :
					iOdds = 30

			iOdds += self.RevOpt.getReligionOdds()

			if( self.isLocalHumanPlayer(pPlayer.getID()) ) :
				# Offer choice
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Offering human choice on religion change")

				# Additions by Caesium et al
				caesiumtR = CyUserProfile().getResolutionString(CyUserProfile().getResolution())
				caesiumtextResolution = caesiumtR.split('x')
				caesiumpasx = int(caesiumtextResolution[0])/10
				caesiumpasy = int(caesiumtextResolution[1])/10
				popup = CyPopup(RevDefs.revolutionPopup, EventContextTypes.EVENTCONTEXT_ALL, False)
				if self.centerPopups:
					popup.setPosition(3*caesiumpasx, 3*caesiumpasy)
				# End additions by Caesium et al
				if bPeaceful:
					popup.setHeaderString(localText.getText("TXT_KEY_REV_TITLE_PEACEFUL",()), 1<<2)
				else: popup.setHeaderString(localText.getText("TXT_KEY_REV_TITLE_VIOLENT",()), 1<<2)

				if iOdds >= 70:
					bodStr += "\n\n" + localText.getText("TXT_KEY_REV_ADVISOR_ACCEPT",())
				elif iOdds <= 30:
					bodStr += "\n\n" + localText.getText("TXT_KEY_REV_ADVISOR_REJECT",())
				else:
					bodStr += "\n\n" + localText.getText("TXT_KEY_REV_ADVISOR_NEUTRAL",())
				popup.setBodyString(bodStr, 1<<0)
				popup.addButton(localText.getText("TXT_KEY_REV_BUTTON_ACCEPT",()) )
				buttons = ('accept',)
				popup.addButton(localText.getText("TXT_KEY_REV_BUTTON_REJECT",()) )
				buttons += ('reject',)
				if iBuyOffCost > 0 and iBuyOffCost <= pPlayer.getGold():
					popup.addButton(localText.getText("TXT_KEY_REV_BUTTON_BUYOFF",()) + ' %d'%(iBuyOffCost) )
					buttons += ('buyoff',)
				if( not bPeaceful and not 'iJoinPlayer' in revData.dict.keys() and not bIsBarbRev and (pRevPlayer.getNumCities() == 0) and self.offerDefectToRevs ) :
					popup.addButton(localText.getText("TXT_KEY_REV_BUTTON_DEFECT",()) )
					buttons += ('defect',)

				popup.setUserData( (buttons, pPlayer.getID(), iRevoltIdx) )
				# Center camera on city
				CyCamera().JustLookAt( cityList[0].plot().getPoint() )
				popup.launch(False, PopupStates.POPUPSTATE_IMMEDIATE)

			elif (not pPlayer.isHuman()) :
				# Make AI decision
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Making AI choice on religion change")

				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Odds are %d"%(iOdds))

				if( GAME.getSorenRandNum(100,'Revolt: switch religion') < iOdds ) :
					self.processRevolution( pPlayer, iRevoltIdx, cityList, revType, bPeaceful, True )

				else:
					if iBuyOffCost > 0 and iBuyOffCost <= pPlayer.getGold():
						if bPeaceful:
							base = .8
						else:
							base = .7
#-------------------------------------------------------------------------------------------------
# Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
						buyOdds = int(iOdds/2.0 + 75*(1 - pow(base, pPlayer.getGold()/iBuyOffCost)))
#-------------------------------------------------------------------------------------------------
# END Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: AI buyoff odds are %d, with base %.1f, from cost %d and gold %d"%(buyOdds,base,iBuyOffCost, pPlayer.getGold()))
						if( buyOdds > GAME.getSorenRandNum( 100, 'Rev - AI buyoff decision' ) ) :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: AI buyingoff rebels")
							revData.dict['bDidBuyOff'] = True

							revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
							revoltDict[iRevoltIdx] = revData
							RevData.revObjectSetVal( pPlayer, 'RevoltDict', revoltDict )

							self.processRevolution( pPlayer, iRevoltIdx, cityList, revType, bPeaceful, False )
							return

					self.processRevolution( pPlayer, iRevoltIdx, cityList, revType, bPeaceful, False )

		elif( revType == 'leader' ) :

			# Load special data for this type
			iNewLeaderType = revData.dict['iNewLeaderType']
			if( 'newLeaderName' in revData.dict.keys() ) :
				newLeaderName = revData.dict['newLeaderName']
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Potential new leader: %s"%(newLeaderName))
			bIsElection = revData.dict['bIsElection']
			iBuyOffCost = revData.dict.get( 'iBuyOffCost', -1 )

			# Compute approval rating (formula from demographics info screen)
			if (pPlayer.calculateTotalCityHappiness() > 0) :
				iHappiness = int((1.0 * pPlayer.calculateTotalCityHappiness()) / (pPlayer.calculateTotalCityHappiness() + \
									pPlayer.calculateTotalCityUnhappiness()) * 100)
			else :
				iHappiness = 100

			# Adjusted approval rating based on num cities in revolt
			if( numRevCities > pPlayer.getNumCities()/2 ) :
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Approval rating (initially %d) adjusted due to %d of %d cities revolting"%(iHappiness,numRevCities,pPlayer.getNumCities()))
				iHappiness = int( iHappiness - 100*(numRevCities - pPlayer.getNumCities()/2)/(2.0*pPlayer.getNumCities()) )
				iHappiness = max([iHappiness,25])

			revData.dict['iHappiness'] = iHappiness

			revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
			revoltDict[iRevoltIdx] = revData
			RevData.revObjectSetVal( pPlayer, 'RevoltDict', revoltDict )

			# Other potential factors:
			# Player rank

			if( numRevCities > pPlayer.getNumCities()/2 ) :
				iOdds = 50
				if( bPeaceful ) :
					iOdds -= 20
				else :
					if( pTeam.getAtWarCount(True) > 1 ) :
						iOdds += 10
				if( pPlayer.hasTrait(iAggressive) ) :
					iOdds -= 30
			elif( numRevCities < pPlayer.getNumCities()/4 ) :
				iOdds = 25
				if( bPeaceful ) :
					iOdds -= 20
				else :
					if( pTeam.getAtWarCount(True) > 1 ) :
						iOdds += 10
				if( pPlayer.hasTrait(iAggressive) ) :
					iOdds -= 20
			else :
				iOdds = 35
				if( bPeaceful ) :
					iOdds -= 30
				else :
					if( pTeam.getAtWarCount(True) > 1 ) :
						iOdds += 10
				if( pPlayer.hasTrait(iAggressive) ) :
					iOdds -= 20

			if( bIsElection ) :
				if( iHappiness > 55 ) :
					iOdds += 40
				elif( iHappiness < 40 ) :
					iOdds -= 30
				elif( iHappiness < 50 ) :
					iOdds -= 15
				else :
					iOdds += 10

			iOdds += self.RevOpt.getLeaderOdds()

			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Odds are %d"%(iOdds))
			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Adjusted approval rating is %d"%(iHappiness))

			if( self.isLocalHumanPlayer(pPlayer.getID()) ) :
				# Offer choice
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Offering human choice on leader change")

				# Additions by Caesium et al
				caesiumtR = CyUserProfile().getResolutionString(CyUserProfile().getResolution())
				caesiumtextResolution = caesiumtR.split('x')
				caesiumpasx = int(caesiumtextResolution[0])/10
				caesiumpasy = int(caesiumtextResolution[1])/10
				popup = CyPopup(RevDefs.revolutionPopup, EventContextTypes.EVENTCONTEXT_ALL, False)
				if self.centerPopups:
					popup.setPosition(3*caesiumpasx, 3*caesiumpasy)
				# End additions by Caesium et al
				if bPeaceful:
					popup.setHeaderString(localText.getText("TXT_KEY_REV_TITLE_PEACEFUL",()), 1<<2)
				else: popup.setHeaderString(localText.getText("TXT_KEY_REV_TITLE_VIOLENT",()), 1<<2)

				if iOdds >= 70:
					bodStr += "\n\n" + localText.getText("TXT_KEY_REV_ADVISOR_ACCEPT",())
				elif iOdds <= 30:
					bodStr += "\n\n" + localText.getText("TXT_KEY_REV_ADVISOR_REJECT",())
				else:
					bodStr += "\n\n" + localText.getText("TXT_KEY_REV_ADVISOR_NEUTRAL",())

				if bIsElection:
					bodStr += "  " + localText.getText("TXT_KEY_REV_ADVISOR_APPROVAL",()) + " %d."%(iHappiness)
					if iHappiness < 40:
						bodStr += "  " + localText.getText("TXT_KEY_REV_ADVISOR_ELEC_LOSE",())

				popup.setBodyString(bodStr, 1<<0)
				popup.addButton(localText.getText("TXT_KEY_REV_BUTTON_ACCEPT",()))
				buttons = ('accept',)
				popup.addButton(localText.getText("TXT_KEY_REV_BUTTON_REJECT",()))
				buttons += ('reject',)
				if iBuyOffCost > 0 and iBuyOffCost <= pPlayer.getGold():
					if bIsElection:
						popup.addButton(localText.getText("TXT_KEY_REV_BUTTON_BUY_ELECTION",()) + ' %d'%(iBuyOffCost))
						buttons += ('buyelection',)
					else:
						popup.addButton(localText.getText("TXT_KEY_REV_BUTTON_BUYOFF",()) + ' %d'%(iBuyOffCost))
						buttons += ('buyoff',)
				if( not bPeaceful and not 'iJoinPlayer' in revData.dict.keys() and not bIsBarbRev and (pRevPlayer.getNumCities() == 0) and self.offerDefectToRevs ) :
					popup.addButton(localText.getText("TXT_KEY_REV_BUTTON_DEFECT",()))
					buttons += ('defect',)

				popup.setUserData( (buttons, pPlayer.getID(), iRevoltIdx) )
				# Center camera on city
				CyCamera().JustLookAt( cityList[0].plot().getPoint() )
				popup.launch(False, PopupStates.POPUPSTATE_IMMEDIATE)

			elif (not pPlayer.isHuman()) :
				# Make AI decision
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Making AI choice on leader change")

				if( GAME.getSorenRandNum(100,'Revolt: switch leader') < iOdds ) :
					self.processRevolution( pPlayer, iRevoltIdx, cityList, revType, bPeaceful, True )

				else:
					if iBuyOffCost > 0 and iBuyOffCost <= pPlayer.getGold():
						if bPeaceful:
							base = .8
						else:
							base = .7
#-------------------------------------------------------------------------------------------------
# Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
						buyOdds = int(iOdds/2.0 + 75*(1 - pow(base, pPlayer.getGold()/iBuyOffCost)))
#-------------------------------------------------------------------------------------------------
# END Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: AI buyoff iOdds are %d, with base %.1f, from cost %d and gold %d"%(buyOdds,base,iBuyOffCost, pPlayer.getGold()))
						if( buyOdds > GAME.getSorenRandNum( 100, 'Rev - AI buyoff decision' ) ) :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: AI buyingoff rebels")
							revData.dict['bDidBuyOff'] = True

							revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
							revoltDict[iRevoltIdx] = revData
							RevData.revObjectSetVal( pPlayer, 'RevoltDict', revoltDict )

							self.processRevolution( pPlayer, iRevoltIdx, cityList, revType, bPeaceful, False )
							return

					self.processRevolution( pPlayer, iRevoltIdx, cityList, revType, bPeaceful, False )

		elif( revType == 'war' ) :
			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Making decision on crusade")

			victim = GC.getPlayer( revData.dict['iRevPlayer'] )

			[iOdds,attackerTeam,victimTeam] = RevUtils.computeWarOdds( pPlayer, victim, cityList[0].area(), allowAttackerVassal = False, allowBreakVassal = self.bAllowBreakVassal )

			if( numRevCities > pPlayer.getNumCities()/2 ) :
				iOdds += 50

			elif( numRevCities < pPlayer.getNumCities()/4 ) :
				iOdds += 20

			else :
				iOdds += 30

			iOdds += self.RevOpt.getCrusadeOdds()

			if( self.isLocalHumanPlayer(pPlayer.getID()) ) :
				# Offer choice
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Offering human choice on crusade")

				# Additions by Caesium et al
				caesiumtR = CyUserProfile().getResolutionString(CyUserProfile().getResolution())
				caesiumtextResolution = caesiumtR.split('x')
				caesiumpasx = int(caesiumtextResolution[0])/10
				caesiumpasy = int(caesiumtextResolution[1])/10
				popup = CyPopup(RevDefs.revolutionPopup, EventContextTypes.EVENTCONTEXT_ALL, False)
				if self.centerPopups:
					popup.setPosition(3*caesiumpasx, 3*caesiumpasy)
				# End additions by Caesium et al
				if bPeaceful:
					popup.setHeaderString(localText.getText("TXT_KEY_REV_TITLE_PEACEFUL",()), 1<<2)
				else: popup.setHeaderString(localText.getText("TXT_KEY_REV_TITLE_VIOLENT",()), 1<<2)

				if iOdds >= 70:
					bodStr += "\n\n" + localText.getText("TXT_KEY_REV_ADVISOR_ACCEPT",())
				elif iOdds <= 30:
					bodStr += "\n\n" + localText.getText("TXT_KEY_REV_ADVISOR_REJECT",())
				else:
					bodStr += "\n\n" + localText.getText("TXT_KEY_REV_ADVISOR_NEUTRAL",())

				popup.setBodyString(bodStr, 1<<0)
				popup.addButton(localText.getText("TXT_KEY_REV_BUTTON_ACCEPT",()) )
				buttons = ('accept',)
				popup.addButton(localText.getText("TXT_KEY_REV_BUTTON_REJECT",()) )
				buttons += ('reject',)

				popup.setUserData( (buttons, pPlayer.getID(), iRevoltIdx) )
				# Center camera on city
				CyCamera().JustLookAt( cityList[0].plot().getPoint() )
				popup.launch(False, PopupStates.POPUPSTATE_IMMEDIATE)

			elif (not pPlayer.isHuman()) :
				# Make AI decision
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Making AI choice on crusade")
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Odds are %d"%(iOdds))

				if( GAME.getSorenRandNum(100,'Revolt: crusade') < iOdds ) :
					self.processRevolution( pPlayer, iRevoltIdx, cityList, revType, bPeaceful, True )
				else:
					self.processRevolution( pPlayer, iRevoltIdx, cityList, revType, bPeaceful, False )

		elif( revType == 'independence' ) :

			bOfferPeace = revData.dict.get( 'bOfferPeace', False )
			joinPlayer = None
			if( 'iJoinPlayer' in revData.dict.keys() ) :
				joinPlayer = GC.getPlayer( revData.dict['iJoinPlayer'] )
			iNumHandoverCities = len( revData.dict.get('HandoverCities', []) )

			vassalStyle = revData.dict.get( 'vassalStyle', None )
			iBuyOffCost = revData.dict.get( 'iBuyOffCost', -1 )
			bIsJoinWar = revData.dict.get( 'bIsJoinWar', False )

			area = cityList[0].area()

			if( numRevCities > pPlayer.getNumCities()/2 ) :
				if( not pRevPlayer == None and pTeam.isAtWar(pRevPlayer.getTeam()) ) :
					if( area.getPower(pRevPlayer.getID()) > 1.5*area.getPower(pPlayer.getID()) and bOfferPeace ) :
						iOdds = 30
					elif( area.getPower(pRevPlayer.getID()) < area.getPower(pPlayer.getID())/2 ) :
						iOdds = -20
					else :
						iOdds = 0
				else :
					iOdds = 20
				if( bPeaceful ) :
					iOdds -= 20
				if( pPlayer.hasTrait(iAggressive) ) :
					iOdds -= 10
				if( pTeam.getAtWarCount(True) > 1 ) :
					iOdds += 5
				if( bOfferPeace ) :
					if( iNumHandoverCities < pPlayer.getNumCities()/4 ) :
						iOdds = max([iOdds + 25, 20])
					elif( iNumHandoverCities < pPlayer.getNumCities()/3 ) :
						iOdds += 15
					elif( iNumHandoverCities < pPlayer.getNumCities()/2 ) :
						iOdds += 10
					else :
						iOdds -= 0
				if( not joinPlayer == None ) :
					if( pTeam.isAtWar(joinPlayer.getTeam()) ) :
						if( bPeaceful ) :
							iOdds = 0
						else :
							iOdds -= 20
			elif( numRevCities < pPlayer.getNumCities()/4 ) :
				if( not pRevPlayer == None and pTeam.isAtWar(pRevPlayer.getTeam()) ) :
					if( area.getPower(pRevPlayer.getID()) > area.getPower(pPlayer.getID()) ) :
						iOdds = 10
						if( bOfferPeace ) :
							iOdds += 40
					elif( area.getPower(pRevPlayer.getID()) < area.getPower(pPlayer.getID())/2 ) :
						iOdds = -20
					else :
						iOdds = 0
				elif( numRevCities == 1 ) :
					iOdds = 20
				else :
					iOdds = 40
				if( bPeaceful ) :
					iOdds -= 20
				if( pPlayer.hasTrait(iAggressive) ) :
					iOdds -= 10
				if( not cityList[0].area().getID() == pPlayer.getCapitalCity().area().getID() ) :
					iOdds += 20
				if( pTeam.getAtWarCount(True) > 1 ) :
					iOdds += 10
				if( bOfferPeace ) :
					if( iNumHandoverCities < pPlayer.getNumCities()/4 ) :
						iOdds = max([iOdds + 20, 20])
					elif( iNumHandoverCities < pPlayer.getNumCities()/3 ) :
						iOdds += 10
					elif( iNumHandoverCities < pPlayer.getNumCities()/2 ) :
						iOdds += 0
					else :
						iOdds -= 20
				if( not joinPlayer == None ) :
					if( pTeam.isAtWar(joinPlayer.getTeam()) ) :
						if( bPeaceful ) :
							iOdds -= 40
						else :
							iOdds -= 30
				if( cityList[0].getNumRevolts(pPlayer.getID()) > 2 ) :
					iOdds += 10
					if( vassalStyle == 'capitulated' ) :
						iOdds += 15

				if( not vassalStyle == None ) :
					iOdds += 10

			else :
				if( not pRevPlayer == None and pTeam.isAtWar(pRevPlayer.getTeam()) ) :
					iOdds = -10
				else :
					iOdds = 25
				if( bPeaceful ) :
					iOdds -= 20
				if( pPlayer.hasTrait(iAggressive) ) :
					iOdds -= 10
				if( not cityList[0].area().getID() == pPlayer.getCapitalCity().area().getID() ) :
					iOdds += 20
				if( pTeam.getAtWarCount(True) > 1 ) :
					iOdds += 10
				if( bOfferPeace ) :
					if( iNumHandoverCities < pPlayer.getNumCities()/4 ) :
						iOdds = max([iOdds + 20, 20])
					elif( iNumHandoverCities < pPlayer.getNumCities()/3 ) :
						iOdds += 15
					elif( iNumHandoverCities < pPlayer.getNumCities()/2 ) :
						iOdds += 0
					else :
						iOdds -= 10
				if( not joinPlayer == None ) :
					if( pTeam.isAtWar(joinPlayer.getTeam()) ) :
						if( bPeaceful ) :
							iOdds -= 30
						else :
							iOdds -= 20
				if( cityList[0].getNumRevolts(pPlayer.getID()) > 2 ) :
					iOdds += 10
					if( vassalStyle == 'capitulated' ) :
						iOdds += 20
					elif( not vassalStyle == None ) :
						iOdds += 10

			iOdds += self.RevOpt.getIndependenceOdds()

			if( joinPlayer == None and bIsBarbRev ) :
				iOdds = 0

			if( bOfferPeace ) :
				if( iNumHandoverCities == pPlayer.getNumCities() ) :
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Will not surrender all cities")
					iOdds = 0
			else :
				if( numRevCities == pPlayer.getNumCities() ) :
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Will not surrender all cities")
					iOdds = 0

			if( self.isLocalHumanPlayer(pPlayer.getID()) ) :
				# Offer choice
				# TODO: is bOfferPeace properly supported?
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Offering human choice on independence")

				# Additions by Caesium et al
				caesiumtR = CyUserProfile().getResolutionString(CyUserProfile().getResolution())
				caesiumtextResolution = caesiumtR.split('x')
				caesiumpasx = int(caesiumtextResolution[0])/10
				caesiumpasy = int(caesiumtextResolution[1])/10
				popup = CyPopup(RevDefs.revolutionPopup, EventContextTypes.EVENTCONTEXT_ALL, False)
				if self.centerPopups:
					popup.setPosition(3*caesiumpasx, 3*caesiumpasy)
				# End additions by Caesium et al
				if bPeaceful:
					popup.setHeaderString(localText.getText("TXT_KEY_REV_TITLE_PEACEFUL",()), 1<<2)
				else: popup.setHeaderString(localText.getText("TXT_KEY_REV_TITLE_VIOLENT",()), 1<<2)

				if iOdds >= 70:
					bodStr += "\n\n" + localText.getText("TXT_KEY_REV_ADVISOR_ACCEPT",())
				elif iOdds <= 30:
					bodStr += "\n\n" + localText.getText("TXT_KEY_REV_ADVISOR_REJECT",())
				else:
					bodStr += "\n\n" + localText.getText("TXT_KEY_REV_ADVISOR_NEUTRAL",())

				popup.setBodyString(bodStr, 1<<0)
				if not vassalStyle == None:
					popup.addButton(localText.getText("TXT_KEY_REV_BUTTON_VASSAL",()))
					buttons = ('vassal',)
					popup.addButton(localText.getText("TXT_KEY_REV_BUTTON_INDEPENDENCE",()))
					buttons += ('accept',)
				elif not bPeaceful and not bOfferPeace and joinPlayer == None and not pRevPlayer.isAlive():
					popup.addButton(localText.getText("TXT_KEY_REV_BUTTON_RECOGNIZE",()))
					buttons = ('accept',)
				else:
					popup.addButton( localText.getText("TXT_KEY_REV_BUTTON_ACCEPT",()) )
					buttons = ('accept',)
				popup.addButton( localText.getText("TXT_KEY_REV_BUTTON_REJECT",()) )
				buttons += ('reject',)
				if( not joinPlayer == None and not bIsJoinWar and GC.getTeam(pPlayer.getTeam()).canDeclareWar(joinPlayer.getTeam()) ) :
					popup.addButton( localText.getText("TXT_KEY_REV_BUTTON_WAR",()) + ' ' + joinPlayer.getCivilizationDescription(0) )
					buttons += ('war',)
				if iBuyOffCost > 0 and iBuyOffCost <= pPlayer.getGold():
					popup.addButton( localText.getText("TXT_KEY_REV_BUTTON_BUYOFF",()) + ' %d'%(iBuyOffCost) )
					buttons += ('buyoff',)
				if( bPeaceful and joinPlayer == None and (pRevPlayer.getNumCities() == 0) ) :
					popup.addButton( localText.getText("TXT_KEY_REV_BUTTON_CONTROL",()) )
					buttons += ('control',)
				elif( not bPeaceful and joinPlayer == None and not pRevPlayer.isBarbarian() and (pRevPlayer.getNumCities() == 0) ) :
					popup.addButton( localText.getText("TXT_KEY_REV_BUTTON_CONTROL",()) )
					buttons += ('control',)
				if( not bPeaceful and joinPlayer == None and not bIsBarbRev and (pRevPlayer.getNumCities() == 0) and self.offerDefectToRevs ) :
					popup.addButton( localText.getText("TXT_KEY_REV_BUTTON_DEFECT",()) )
					buttons += ('defect',)

				popup.setUserData( (buttons, pPlayer.getID(), iRevoltIdx) )
				# Center camera on city
				CyCamera().JustLookAt( cityList[0].plot().getPoint() )
				popup.launch(False, PopupStates.POPUPSTATE_IMMEDIATE)

			elif (not pPlayer.isHuman()) :
				# Make AI decision
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Making AI choice on independence")
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Odds are %d"%(iOdds))

				if( GAME.getSorenRandNum(100,'Revolt: give independence') < iOdds ) :
					self.processRevolution( pPlayer, iRevoltIdx, cityList, revType, bPeaceful, True )
				else:
					if iBuyOffCost > 0 and iBuyOffCost <= pPlayer.getGold():
						if bPeaceful:
							base = .8
						else :
							base = .7
#-------------------------------------------------------------------------------------------------
# Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
						buyOdds = int(iOdds/2.0 + 75*(1 - pow(base, pPlayer.getGold()/iBuyOffCost)))
#-------------------------------------------------------------------------------------------------
# END Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: AI buyoff iOdds are %d, with base %.1f, from cost %d and gold %d"%(buyOdds,base,iBuyOffCost, pPlayer.getGold()))
						if( buyOdds > GAME.getSorenRandNum( 100, 'Rev - AI buyoff decision' ) ) :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: AI buyingoff rebels")
							revData.dict['bDidBuyOff'] = True

							revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
							revoltDict[iRevoltIdx] = revData
							RevData.revObjectSetVal( pPlayer, 'RevoltDict', revoltDict )

							self.processRevolution( pPlayer, iRevoltIdx, cityList, revType, bPeaceful, False )
							return

					self.processRevolution( pPlayer, iRevoltIdx, cityList, revType, bPeaceful, False )

		else :
			if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Unknown revolution type %s"%(revType))
			isRevType = False

		if (pPlayer.isHuman() and isRevType) :
			if( not pRevPlayer == None ) :
				# Claim slot so it's not taken while waiting for response to popup (local or network)
				RevData.revObjectInit( pRevPlayer )


##--- Revolution processing functions ----------------------------------------

	def revolutionNetworkPopupHandler(self, iPlayerID, iButton, iIdx):

		if self.LOG_DEBUG:
			print "[REV] Handling network revolution popups including local player"

		newHumanPlayer = None
		termsAccepted = False
		bSwitchToRevs = False

		iButtonCode = iButton
		iPlayer = iPlayerID
		iRevoltIdx = iIdx

		buttonLabel = ''
		if	 iButtonCode == 0: buttonLabel = 'accept'
		elif iButtonCode == 1: buttonLabel = 'reject'
		elif iButtonCode == 2: buttonLabel = 'buyoff'
		elif iButtonCode == 3: buttonLabel = 'vassal'
		elif iButtonCode == 4: buttonLabel = 'control'
		elif iButtonCode == 5: buttonLabel = 'buyelection'
		elif iButtonCode == 6: buttonLabel = 'war'
		elif iButtonCode == 7: buttonLabel = 'defect'
		else: buttonLabel = 'reject'

		pPlayer = GC.getPlayer(iPlayer)
		revData = RevData.revObjectGetVal(pPlayer, 'RevoltDict')[iRevoltIdx]

		if self.LOG_DEBUG:
			print "[REV] Button %s pressed by playerID = %d" % (buttonLabel, iPlayer)

		# Clear claim on slot
		if 'iRevPlayer' in revData.dict.keys():
			pRevPlayer = GC.getPlayer(revData.dict['iRevPlayer'])
			RevData.revObjectWipe(pRevPlayer)

		cityList = []
		for iCity in revData.cityList:
			# No game actions have been taken that could take these cities away, so all should still be owner by pPlayer
			cityList.append(pPlayer.getCity(iCity))
		revType = revData.revType
		bPeaceful = revData.bPeaceful

		if buttonLabel == 'accept':
			termsAccepted = True
		elif buttonLabel == 'reject':
			termsAccepted = False

		elif buttonLabel == 'buyoff': # Pay the cities not to revolt
			termsAccepted = False
			revData.dict['bDidBuyOff'] = True

		elif buttonLabel == 'vassal': # pRevPlayer becomes vassal of pPlayer
			termsAccepted = True
			assert( 'vassalStyle' in revData.dict.keys() )

		elif buttonLabel == 'control':
			# Give control of new civ
			termsAccepted = True
			newHumanPlayer = revData.dict['iRevPlayer']

		elif buttonLabel == 'buyelection':
			# Attempt buyoff of election results
			if self.LOG_DEBUG:
				print "[REV] Feature not fully implemented %s" % buttonLabel
			if self.isLocalHumanPlayer(iPlayerID):
				# Report election bought off
				# Additions by Caesium et al
				caesiumtR = CyUserProfile().getResolutionString(CyUserProfile().getResolution())
				caesiumtextResolution = caesiumtR.split('x')
				caesiumpasx = int(caesiumtextResolution[0])/10
				caesiumpasy = int(caesiumtextResolution[1])/10
				popup = CyPopup(-1, EventContextTypes.NO_EVENTCONTEXT, True)
				if self.centerPopups:
					popup.setPosition(3*caesiumpasx, 3*caesiumpasy)
				# End additions by Caesium et al
				popup.setBodyString(localText.getText("TXT_KEY_REV_HUMAN_ELEC_BUYOFF",()), 1<<0)
				popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)
			revData.dict['bDidBuyOff'] = True
			termsAccepted = False

		elif buttonLabel == 'war':
			# Declare war on cultural owner of rebelling cities
			termsAccepted = False
			revData.dict['iRevPlayer'] = revData.dict['iJoinPlayer']
			del revData.dict['iJoinPlayer']

			pRevPlayer = GC.getPlayer( revData.dict['iRevPlayer'] )
			GC.getTeam( pPlayer.getTeam()).declareWar( pRevPlayer.getTeam(), True, WarPlanTypes.NO_WARPLAN )

		elif buttonLabel == 'defect':
			termsAccepted = False
			revData.dict['bSwitchToRevs'] = True
			# Release this player turn to the AI, human player changed below so that human now will control rebels
			GAME.setForcedAIAutoPlay(iPlayerID, 1, True)

		else:
			if self.LOG_DEBUG:
				print "[REV] Error! Unrecognized button label %s" % buttonLabel
			termsAccepted = False

		if not newHumanPlayer is None:
			if self.LOG_DEBUG:
				print "[REV] Human being given control of new player %d" % newHumanPlayer
			RevUtils.changeHuman(newHumanPlayer, iPlayerID)

		revoltDict = RevData.revObjectGetVal(pPlayer, 'RevoltDict')
		revoltDict[iRevoltIdx] = revData
		RevData.revObjectSetVal(pPlayer, 'RevoltDict', revoltDict)

		self.processRevolution(pPlayer, iRevoltIdx, cityList, revType, bPeaceful, termsAccepted, switchToRevs = bSwitchToRevs)


	def processRevolution(self, pPlayer, iRevoltIdx, cityList, revType, bPeaceful, termsAccepted, switchToRevs = False):
		if self.LOG_DEBUG:
			CvUtil.pyPrint("[REV] Revolt: Processing revolution revolution type: %s"%(revType))
		if self.DEBUG_MESS:
			CyInterface().addImmediateMessage('Processing revolution!!!',"")

		if not pPlayer.isAlive() or not pPlayer.getNumCities() > 0:
			return

		revData = RevData.revObjectGetVal(pPlayer, 'RevoltDict')[iRevoltIdx]

		pTeam = GC.getTeam(pPlayer.getTeam())

		iAggressive = GC.getInfoTypeForString(RevDefs.sXMLAggressive)
		iSpiritual = GC.getInfoTypeForString(RevDefs.sXMLSpiritual)
		numRevCities = len(cityList)
		capital = pPlayer.getCapitalCity()
		capitalArea = capital.area().getID()

		newCityList = []
		for pCity in cityList:
			if pCity.getOwner() == pPlayer.getID():
				pCity.changeNumRevolts(pPlayer.getID(), 1)
				if self.LOG_DEBUG:
					CvUtil.pyPrint("[REV] Revolt: %s has now revolted %d times"%(pCity.getName(),pCity.getNumRevolts(pPlayer.getID())))
				newCityList.append(pCity)

			elif self.LOG_DEBUG:
				CvUtil.pyPrint("[REV] Revolt: WARNING: %s no longer owned by revolt player!"%(pCity.getName()))

		cityList = newCityList

		#if( not len(cityList) > 0 ) :
		#	return

		if( revData.dict.get( 'bDidBuyOff', False ) ) :
			if revType == 'leader' and revData.dict.get('bIsElection', False):
				# TODO something special for this
				pass
			pPlayer.changeGold( -revData.dict['iBuyOffCost'] )
			for pCity in cityList:
				if self.LOG_DEBUG:
					CvUtil.pyPrint("[REV] Revolt: Buying off revolutionaries in %s"%(pCity.getName()))
				revIdx = pCity.getRevolutionIndex()
				pCity.setRevolutionIndex(min([self.revInstigatorThreshold * self.revReadyDividend / self.revReadyDivisor, revIdx - 200]))
				pCity.setRevolutionCounter(self.buyoffTurns)
				pCity.setReinforcementCounter(0)
				RevData.setCityVal(pCity, 'BribeTurn', GAME.getGameTurn())
				pCity.changeNumRevolts(pPlayer.getID(), -1)

		elif revType == 'civics':

			newCivic = revData.dict['iNewCivic']

			if termsAccepted:
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Player opts to switch, lowering rev indices")
				# Do switch
				newCivicOption = GC.getCivicInfo( newCivic ).getCivicOptionType()
				pPlayer.setCivics( newCivicOption, newCivic )
				iSpiritual = GC.getInfoTypeForString(RevDefs.sXMLSpiritual)
				pPlayer.changeRevolutionTimer(5)
				if( not pPlayer.hasTrait(iSpiritual) ) :
					if( pPlayer.getCurrentEra() > GC.getNumEraInfos()/2 ) :
						pPlayer.changeAnarchyTurns(2)
					else :
						pPlayer.changeAnarchyTurns(1)

				# Make those cities happier
				for pCity in cityList :
					revIdx = pCity.getRevolutionIndex()
					pCity.setRevolutionIndex( min([revIdx/2,int(.6*self.revInstigatorThreshold)]) )
					pCity.setRevolutionCounter( self.acceptedTurns )
					pCity.setReinforcementCounter(0)
					if( pCity.getNumRevolts(pPlayer.getID()) > 1 ) :
						pCity.changeNumRevolts( pPlayer.getID(), -2 )
					else :
						pCity.changeNumRevolts( pPlayer.getID(), -1 )

			else :
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Player declines to switch")
				if( bPeaceful ) :

					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Peaceful, increasing rev indices")
					# Make those cities more unhappy
					for pCity in cityList :
						RevUtils.doRevRequestDeniedPenalty( pCity, capitalArea, bExtraColony = True )

				else :
					pRevPlayer = GC.getPlayer( revData.dict['iRevPlayer'] )
					self.prepareRevolution( pPlayer, iRevoltIdx, cityList, pRevPlayer, bIsJoinWar = revData.dict.get('bIsJoinWar', False), switchToRevs = switchToRevs )

		elif( revType == 'religion' ) :

			iNewReligion = revData.dict['iNewReligion']

			assert( pPlayer.isStateReligion() )

			if( termsAccepted ) :
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Player opts to switch, lowering rev indices")
				# Do switch
				pPlayer.convert(iNewReligion)
				# Make those cities happier
				for pCity in cityList :
					revIdx = pCity.getRevolutionIndex()
					pCity.setRevolutionIndex( min([revIdx/2,int(.6*self.revInstigatorThreshold)]) )
					pCity.setRevolutionCounter( self.acceptedTurns )
					pCity.setReinforcementCounter(0)
					if( pCity.getNumRevolts(pPlayer.getID()) > 1 ) :
						pCity.changeNumRevolts( pPlayer.getID(), -2 )
					else :
						pCity.changeNumRevolts( pPlayer.getID(), -1 )

			else :
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Player declines to switch")
				if( not bPeaceful ) :
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Error ... shouldn't have violent request for religion switch")

				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Peaceful, increasing rev indices")
				# Make those cities more unhappy
				for pCity in cityList :
					RevUtils.doRevRequestDeniedPenalty( pCity, capitalArea, iRevIdxInc = 150, bExtraColony = True )

		elif( revType == 'leader' ) :

			newLeaderType = revData.dict['iNewLeaderType']

			newLeaderName = revData.dict.get( 'newLeaderName', None )

			if( termsAccepted ) :
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Player opts to cede power, lowering rev indices")

				# Lower indices
				for pCity in cityList :
					revIdx = pCity.getRevolutionIndex()
					pCity.setRevolutionIndex( min([revIdx/2,int(.6*self.revInstigatorThreshold)]) )
					pCity.setRevolutionCounter( self.acceptedTurns )
					pCity.setReinforcementCounter(0)
					if( pCity.getNumRevolts(pPlayer.getID()) > 1 ) :
						pCity.changeNumRevolts( pPlayer.getID(), -2 )
					else :
						pCity.changeNumRevolts( pPlayer.getID(), -1 )

				# Do switch leader
				if( revData.dict.get( 'bIsElection', False ) ) :

					iHappiness = revData.dict['iHappiness']

					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Running an election for a new leader!")
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Adjusted approval rating is %d"%(iHappiness))

					if iHappiness < 40:
						if self.LOG_DEBUG:
							CvUtil.pyPrint("[REV] Revolt: Election lost due to low approval rating")
						# Continue to change leader code below
					elif GAME.getSorenRandNum(25,'Revolution: election results') + 30 > iHappiness:
						if self.LOG_DEBUG:
							CvUtil.pyPrint("[REV] Revolt: Election lost by probability")
					else:
						if self.LOG_DEBUG:
							CvUtil.pyPrint("[REV] Revolt: Election won!  Leader stays in power!")

						if self.isLocalHumanPlayer(pPlayer.getID()):
							# Report election victory
							# Additions by Caesium et al
							caesiumtR = CyUserProfile().getResolutionString(CyUserProfile().getResolution())
							caesiumtextResolution = caesiumtR.split('x')
							caesiumpasx = int(caesiumtextResolution[0])/10
							caesiumpasy = int(caesiumtextResolution[1])/10
							popup = CyPopup(-1, EventContextTypes.NO_EVENTCONTEXT, True)
							if self.centerPopups:
								popup.setPosition(3*caesiumpasx, 3*caesiumpasy)
							# End additions by Caesium et al
							popup.setBodyString(localText.getText("TXT_KEY_REV_HUMAN_ELEC_VICTORY",()), 1<<0)
							popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)
						return

				if self.isLocalHumanPlayer(pPlayer.getID()):
					# Additions by Caesium et al
					caesiumtR = CyUserProfile().getResolutionString(CyUserProfile().getResolution())
					caesiumtextResolution = caesiumtR.split('x')
					caesiumpasx = int(caesiumtextResolution[0])/10
					caesiumpasy = int(caesiumtextResolution[1])/10
					popup = CyPopup(RevDefs.controlLostPopup, EventContextTypes.EVENTCONTEXT_ALL, True)
					if self.centerPopups:
						popup.setPosition(3*caesiumpasx, 3*caesiumpasy)
					# End additions by Caesium et al

					gameSpeedMod = RevUtils.getGameSpeedMod()

					if revData.dict.get('bIsElection', False):
						iNumTurns = int(15/gameSpeedMod + .5)
						bodStr = localText.getText("TXT_KEY_REV_HUMAN_ELEC_LOSS",())
					else:
						iNumTurns = int(24/gameSpeedMod + .5)
						bodStr = localText.getText("TXT_KEY_REV_HUMAN_CEDE",())

					bodStr += '\n\n' + localText.getText("TXT_KEY_REV_HUMAN_CONTROL_RETURNED",()) + ' %d '%(iNumTurns) + localText.getText("TXT_KEY_TURNS",()).lower() + '.'
					popup.setBodyString(bodStr, 1<<0)
#-------------------------------------------------------------------------------------------------
# Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
					# Hacky fix to make the controlLost net message get out. Doesn't seem to work otherwise.
					#iNewLeaderType = revData.dict['iNewLeaderType']
					popup.setUserData( (iNumTurns, newLeaderType) )
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("  Control player is %d"%(GAME.getActivePlayer()))

					#self.controlLostNetworkHandler(GAME.getActivePlayer(), iNumTurns, newLeaderType)
					CyMessageControl().sendModNetMessage(self.netControlLostPopupProtocol, GAME.getActivePlayer(), iNumTurns, newLeaderType, 0)
#-------------------------------------------------------------------------------------------------
# END Lemmy101 RevolutionMP edit
#-------------------------------------------------------------------------------------------------
					popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

			#terms not accepted
			else:
				if( revData.dict.get( 'bIsElection', False ) ) :
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Player refuses election")
				else :
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Player clings to power")

				if( bPeaceful ) :
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Peaceful, increasing rev indices")
					for pCity in cityList :
						RevUtils.doRevRequestDeniedPenalty( pCity, capitalArea, iRevIdxInc = 150, bExtraColony = True )

				else :
					joinPlayer = None
					if( 'iJoinPlayer' in revData.dict.keys() ) :
						joinPlayer = GC.getPlayer( revData.dict['iJoinPlayer'] )
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Error!  Join player specified for leader type revolt ...")

					# if( not joinPlayer == None ) :
						# joinPlayer.AI_changeAttitudeExtra( pPlayer.getID(), -2 )
						# if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s's Extra Attitude towards %s now %d"%(joinPlayer.getCivilizationDescription(0),pPlayer.getCivilizationDescription(0),joinPlayer.AI_getAttitudeExtra(pPlayer.getID())))
						# pPlayer.AI_changeAttitudeExtra( joinPlayer.getID(), -2 )

						# self.prepareRevolution( pPlayer, iRevoltIdx, cityList, joinPlayer, bIsJoinWar = True, switchToRevs = switchToRevs )

					# else :
					pRevPlayer = GC.getPlayer( revData.dict['iRevPlayer'] )
					if( not pRevPlayer.isBarbarian() ) :
						pRevPlayer.AI_changeAttitudeExtra( pPlayer.getID(), -5 )
						if( 'iJoinPlayer' in revData.dict.keys() ) : pRevPlayer.AI_changeAttitudeExtra( revData.dict['iJoinPlayer'], 5 )
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s's Extra Attitude towards %s now %d"%(pRevPlayer.getCivilizationDescription(0),pPlayer.getCivilizationDescription(0),pRevPlayer.AI_getAttitudeExtra(pPlayer.getID())))
						pPlayer.AI_changeAttitudeExtra( pRevPlayer.getID(), -3 )
					self.prepareRevolution( pPlayer, iRevoltIdx, cityList, pRevPlayer, bIsJoinWar = revData.dict.get('bIsJoinWar', False), switchToRevs = switchToRevs )

		elif( revType == 'war' ) :

			if( termsAccepted ) :
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Player opts for war!!!")

				pRevPlayer = GC.getPlayer( revData.dict['iRevPlayer'] )

				for pCity in cityList :
					revIdx = pCity.getRevolutionIndex()
					pCity.setRevolutionIndex( min([revIdx/2,int(.5*self.revInstigatorThreshold)]) )
					pCity.setRevolutionCounter( self.acceptedTurns )
					pCity.setReinforcementCounter(0)
					if( pCity.getNumRevolts(pPlayer.getID()) > 1 ) :
						pCity.changeNumRevolts( pPlayer.getID(), -2 )
					else :
						pCity.changeNumRevolts( pPlayer.getID(), -1 )

				pTeam.declareWar( pRevPlayer.getTeam(), True, WarPlanTypes.NO_WARPLAN )

				#TODO: spawn fanatic units or something for holy war?

			else :
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Player declines cries for war.")

				if( bPeaceful ) :
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Peaceful, increasing rev indices")
					# Make those cities more unhappy
					for pCity in cityList :
						RevUtils.doRevRequestDeniedPenalty( pCity, capitalArea, bExtraHomeland = True )

				else :
					# Violent uprising!!!
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Error!  Unexpected violent request for war, not well supported")

					if( 'iJoinPlayer' in revData.dict.keys() ) :

						joinPlayer = GC.getPlayer( revData.dict['iJoinPlayer'] )
						joinPlayer.AI_changeAttitudeExtra( pPlayer.getID(), -3 )
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s's Extra Attitude towards %s now %d"%(joinPlayer.getCivilizationDescription(0),pPlayer.getCivilizationDescription(0),joinPlayer.AI_getAttitudeExtra(pPlayer.getID())))

						# Check if joinPlayer would like to declare war on player
						[warOdds,attackerTeam,victimTeam] = RevUtils.computeWarOdds( joinPlayer, pPlayer, cityList[0].area(), False, allowBreakVassal = self.bAllowBreakVassal )
						# TODO: Find way to support human selecting war in this case
						if( attackerTeam.isHuman() and not attackerTeam.isAtWar(victimTeam.getID()) ) :
							warOdds = 0

						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: War odds are %d"%(warOdds))

						if( warOdds > 25 and warOdds > GAME.getSorenRandNum(100,'Revolution: war') ) :
							# have joinPlayer's team (or vassal master) declare war on player's team
							attackerTeam.declareWar(pPlayer.getTeam(),True, WarPlanTypes.NO_WARPLAN)

							revData.dict['iRevPlayer'] = joinPlayer.getID()
							del revData.dict['iJoinPlayer']
							revData.dict['bIsJoinWar'] = True

							revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
							revoltDict[iRevoltIdx] = revData
							RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

							self.prepareRevolution( pPlayer, iRevoltIdx, cityList, joinPlayer, bIsJoinWar = True, switchToRevs = switchToRevs )

						else :
							pRevPlayer = GC.getPlayer( revData.dict['iRevPlayer'] )
							if( not pRevPlayer.isBarbarian() ) :
								pRevPlayer.AI_changeAttitudeExtra( pPlayer.getID(), -5 )
								pRevPlayer.AI_changeAttitudeExtra( joinPlayer.getID(), 5 )
								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s's Extra Attitude towards %s now %d"%(pRevPlayer.getCivilizationDescription(0),pPlayer.getCivilizationDescription(0),pRevPlayer.AI_getAttitudeExtra(pPlayer.getID())))
								pPlayer.AI_changeAttitudeExtra( pRevPlayer.getID(), -3 )

							self.prepareRevolution( pPlayer, iRevoltIdx, cityList, pRevPlayer, bIsJoinWar = revData.dict.get('bIsJoinWar', False), switchToRevs = switchToRevs )

					else :
						pRevPlayer = GC.getPlayer( revData.dict['iRevPlayer'] )
						if( not pRevPlayer.isBarbarian() ) :
							pRevPlayer.AI_changeAttitudeExtra( pPlayer.getID(), -5 )
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s's Extra Attitude towards %s now %d"%(pRevPlayer.getCivilizationDescription(0),pPlayer.getCivilizationDescription(0),pRevPlayer.AI_getAttitudeExtra(pPlayer.getID())))
							pPlayer.AI_changeAttitudeExtra( pRevPlayer.getID(), -3 )

						self.prepareRevolution( pPlayer, iRevoltIdx, cityList, pRevPlayer, bIsJoinWar = revData.dict.get('bIsJoinWar', False), switchToRevs = switchToRevs )

		elif( revType == 'independence' ) :

			pRevPlayer = GC.getPlayer( revData.dict['iRevPlayer'] )

			bOfferPeace = revData.dict.get('bOfferPeace', False)
			handoverCities = []
			for iCity in revData.dict.get('HandoverCities', []) :
				handoverCities.append(pPlayer.getCity(iCity))

			if( termsAccepted ) :
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Player opts to give independence, lowering rev indices")

				# Lower indices (makes life a little easier for new civ ...)
				if( bOfferPeace ) :
					# Special case where pPlayer turns over control of handover cities instead of having cityList revolt

					for pCity in handoverCities :
						revIdx = pCity.getRevolutionIndex()
						pCity.setRevolutionIndex(min([revIdx / 2, self.revInstigatorThreshold * 4 * self.revReadyDividend / (5 * self.revReadyDivisor)]))
						# Other changes handled by acquired city logic
						if pCity.getNumRevolts(pPlayer.getID()) > 1:
							pCity.changeNumRevolts(pPlayer.getID(), -2)
						else:
							pCity.changeNumRevolts(pPlayer.getID(), -1)

					for pCity in cityList :
						if( not pCity.getID() in revData.dict.get('HandoverCities', []) ) :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Turning down rebelliousness in non-handed over city %s"%(pCity.getName()))
							revIdx = pCity.getRevolutionIndex()
							pCity.setRevolutionIndex( min([(3*revIdx)/4,int(self.revInstigatorThreshold)]) )
							if( pCity.getNumRevolts(pPlayer.getID()) > 2 ) :
								pCity.changeNumRevolts( pPlayer.getID(), -2 )
							else :
								pCity.changeNumRevolts( pPlayer.getID(), -1 )

					pCity = pPlayer.getCapitalCity()
					if( not pCity.getID() in revData.dict.get('HandoverCities', []) ) :
						revIdx = pCity.getRevolutionIndex()
						if( not pCity.getID() in revData.dict.get('cityList', []) and (revIdx > self.revInstigatorThreshold and RevData.getCityVal(pCity,'RevolutionCiv') == pRevPlayer.getCivilizationType()) ) :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Also turning down rebelliousness in capital %s"%(pCity.getName()))
							pCity.setRevolutionIndex( min([(7*revIdx)/8,int(self.revInstigatorThreshold)]) )
							# Other changes handled by acquired city logic
							if( pCity.getNumRevolts(pPlayer.getID()) > 2 ) :
								pCity.changeNumRevolts( pPlayer.getID(), -2 )
							elif( pCity.getNumRevolts(pPlayer.getID()) > 0 ) :
								pCity.changeNumRevolts( pPlayer.getID(), -1 )

					# Switch list to be given independence
					cityList = handoverCities

				else:
					for pCity in cityList:
						revIdx = pCity.getRevolutionIndex()
						pCity.setRevolutionIndex(min([revIdx/2, self.revInstigatorThreshold * 4 * self.revReadyDividend / (5 * self.revReadyDivisor)]))
						# Other changes handled by acquired city logic
						if pCity.getNumRevolts(pPlayer.getID()) > 1:
							pCity.changeNumRevolts(pPlayer.getID(), -2)
						else:
							pCity.changeNumRevolts(pPlayer.getID(), -1)


				iGoodyMap = GC.getInfoTypeForString(RevDefs.sXMLGoodyMap)

				if( not 'iJoinPlayer' in revData.dict.keys() ) :
					# Grant independence
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: The %s are taking over the revolutionary cities"%(pRevPlayer.getCivilizationDescription(0)))

					bIsBarbRev = pRevPlayer.isBarbarian()

					pRevTeam = GC.getTeam( pRevPlayer.getTeam() )
					pTeam = GC.getTeam( pPlayer.getTeam() )

					bGaveMap = False

					if( not bIsBarbRev ) :
						# Diplomacy
						pRevTeam.meet(pPlayer.getTeam(),False)

						if( not pRevTeam.isAlive() ) :
							if( pRevTeam.isAtWar(pPlayer.getTeam()) ) :
								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Reincarnated %s ending war with %s"%(pRevPlayer.getCivilizationDescription(0),pPlayer.getCivilizationDescription(0)))
								pRevTeam.makePeace( pPlayer.getTeam() )

							pRevTeam.signOpenBorders(pPlayer.getTeam())

						if( pRevTeam.isAtWar(pPlayer.getTeam()) and revData.dict.get( 'bOfferPeace', False ) ) :
							pRevTeam.makePeace( pPlayer.getTeam() )

						if( pRevTeam.isAtWar(pPlayer.getTeam()) ) :
							pRevPlayer.AI_changeMemoryCount(pPlayer.getID(), MemoryTypes.MEMORY_LIBERATED_CITIES, 1)
						else :
							pRevPlayer.AI_changeAttitudeExtra( pPlayer.getID(), 6 + len(cityList)/2 )
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s's Extra Attitude towards %s now %d"%(pRevPlayer.getCivilizationDescription(0),pPlayer.getCivilizationDescription(0),pRevPlayer.AI_getAttitudeExtra(pPlayer.getID())))

						if pRevPlayer.getGold() < 50:
							pRevPlayer.changeGold( 30 + GAME.getSorenRandNum(25*len(cityList),'Revolt: give gold') )
						else :
							pRevPlayer.changeGold( 10 + GAME.getSorenRandNum(15*len(cityList),'Revolt: give gold') )

						if pRevTeam.isMapTrading():
							# Give motherlands map
							bGaveMap = True
							MAP = GC.getMap()
							for pPlot in MAP.plots():
								if pPlot.isRevealed(pTeam.getID(), False):
									pPlot.setRevealed(pRevTeam.getID(), True, False, pTeam.getID())

							# Meet players known by motherland
							for k in xrange(GC.getMAX_PC_TEAMS()) :
								if( pTeam.isHasMet(k) ) :
									# Granted independence, so just meet some fraction of players
									if( GAME.getSorenRandNum(100,'odds') > 50 ) :
										pRevTeam.meet(k,False)

						if not revData.dict.get('bIsJoinWar', False):
							if not RevData.revObjectGetVal(pRevPlayer, 'MotherlandID') == None:
								revTurn = RevData.revObjectGetVal(pRevPlayer, 'RevolutionTurn')
								if revTurn == None or GAME.getGameTurn() - revTurn > 30:
									RevData.revObjectSetVal( pRevPlayer, 'MotherlandID', pPlayer.getID() )
							else: RevData.revObjectSetVal( pRevPlayer, 'MotherlandID', pPlayer.getID() )
							RevData.revObjectSetVal( pRevPlayer, 'RevolutionTurn', GAME.getGameTurn() )

						if not pRevPlayer.isAlive():
							if len(cityList) < 3:
								cityString = TextUtil.convertToStr(cityList[0].getName())
							else: cityString = None
							RevData.revObjectSetVal( pRevPlayer, 'CapitalName', cityString )

							# Set new player alive before giving cities so that they draw properly
							if pPlayer.isMinorCiv():
								pRevTeam.setIsMinorCiv( True, False )
							elif pRevPlayer.isMinorCiv():
								pRevTeam.setIsMinorCiv( False, False )
							pRevPlayer.setNewPlayerAlive(True)

					iNumPlayerCities = pPlayer.getNumCities()

					for iPlayer in xrange(GC.getMAX_PC_PLAYERS()) :
						# Craft revolution anouncement message for all players
						if( GC.getPlayer(iPlayer).canContact(pPlayer.getID()) or iPlayer == pPlayer.getID() ) :

							colorNum = GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT")
							pPlayerName = str(localText.getText("TXT_KEY_REV_THE",())) + ' ' + pPlayer.getCivilizationDescription(0)
							revPlayerName = str(localText.getText("TXT_KEY_REV_THE",())) + ' ' + pRevPlayer.getCivilizationDescription(0)

							if( iPlayer == pRevPlayer.getID() ) :
								colorNum = GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT")
								revPlayerName = localText.getText("TXT_KEY_REV_YOUR_CIV",())
							elif( iPlayer == pPlayer.getID() ) :
								pPlayerName = str(localText.getText("TXT_KEY_REV_YOU",()))

							cityString = bodStr = getCityTextList(cityList)

							mess = localText.getText("TXT_KEY_REV_MESS_CEDE",())%(pPlayerName,cityString,revPlayerName)

							if( iPlayer == pPlayer.getID() ) :
								CyInterface().addMessage(iPlayer, False, GC.getEVENT_MESSAGE_TIME(), mess, "AS2D_CITY_REVOLT", InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_RESISTANCE").getPath(), ColorTypes(colorNum), cityList[0].getX(), cityList[0].getY(), True, True)
							else :
								CyInterface().addMessage(iPlayer, False, GC.getEVENT_MESSAGE_TIME(), mess, None, InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT, None, ColorTypes(colorNum), -1, -1, False, False)

					if( not bIsBarbRev and (pRevPlayer.getNumCities() + pRevPlayer.getCitiesLost() < 4) and len(cityList) > 1 ) :
						# Launch golden age for rebel player (helps with stability and being competitive)
						pRevPlayer.changeGoldenAgeTurns( int(1.5*GAME.goldenAgeLength()) )

					# Since instigator is first in list, it will become capital if pRevPlayer has no others
					for pCity in cityList :
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Acquiring %s, pop %d"%(pCity.getName(),pCity.getPopulation()))
						# Move units out of city?  Handled by trade style

						if( pRevTeam.isAtWar(pPlayer.getTeam()) and iNumPlayerCities > len(cityList) ) :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Moving owner's units")
							RevUtils.clearOutCity( pCity, pPlayer, pRevPlayer )

						[iWorker, iBestDefender, iCounter, iAttack] = RevUtils.getHandoverUnitTypes(pCity)

						RevData.updateCityVal(pCity, 'RevolutionCiv', -1 )

						# Store building types in city
						buildingList = []
						for buildingType in xrange(GC.getNumBuildingInfos()) :
							if( pCity.getNumRealBuilding(buildingType) > 0 ) :
								buildingInfo = GC.getBuildingInfo(buildingType)
								buildingList.append([buildingType,pCity.getNumRealBuilding(buildingType)])

#***********************************
						# Acquire city
						#joinPlayer.acquireCity( pCity, False, True )

						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Population of %s before is %d"%(pCity.getName(),pCity.getPopulation()))
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Check city culture is %d, at %d, %d"%(pCity.getCulture(pPlayer.getID()), pCity.getX(),pCity.getY()))
						cityPlot = pCity.plot()
						if( pCity.getCulture( pPlayer.getID() ) == 0 ) :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Forcing culture > 0")
							pCity.setCulture( pPlayer.getID(), 1, True )

						try :
							pCity.plot().setOwner( pRevPlayer.getID() )
						except :
							print "ERROR in grant independence"
							print "ERROR:  Failed to set owner of city, %s at plot %d, %d"%(pCity.getName(),cityPlot.getX(),cityPlot.getY())
							#print "City culture is %d"%(pCity.getCulture(pPlayer.getID()))

							#pCity = cityPlot.getPlotCity()
							#print "Post culture in %s is %d"%(pCity.getName(),pCity.getCulture(pPlayer.getID()))
							#pRevPlayer.acquireCity( pCity, False, False )
							#RevData.initCity(pCity)
							# City has become invalid, will cause game to crash if left
							print "Destroying city so game can continue"
							pCity.kill()
							continue


						pCity = cityPlot.getPlotCity()
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Population of %s after is %d"%(pCity.getName(),pCity.getPopulation()))

						if( pCity.getPopulation() < 1 ) :
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Error!  City %s is empty"%(pCity.getName()))
#*************************************

						# Save most buildings - should some be destroyed?
						for [buildingType,iNum] in buildingList :
							if( pCity.getNumRealBuilding(buildingType) < iNum ) :
								buildingInfo = GC.getBuildingInfo(buildingType)
								if( not buildingInfo.isGovernmentCenter() ) :
									if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Building %s saved"%(buildingInfo.getDescription()))
									pCity.setNumRealBuilding( buildingType, iNum )

						#if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s at %d, %d"%(pCity.getName(),pCity.getX(),pCity.getY()))

						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Culture in %s: %d, plot %d"%(pCity.getName(),pCity.getCulture(pPlayer.getID()),pCity.plot().getCulture(pPlayer.getID())))

						newCulVal = int( self.revCultureModifier*max([1.0*pCity.getCulture(pPlayer.getID()),pCity.countTotalCultureTimes100()/200]) )
						newPlotVal = int( self.revCultureModifier*max([1.2*pCity.plot().getCulture(pPlayer.getID()),pCity.plot().countTotalCulture()/2]) )
						RevUtils.giveCityCulture( pCity, pRevPlayer.getID(), newCulVal, newPlotVal)

						ix = pCity.getX()
						iy = pCity.getY()

						# Record pre-given units for deletion after giving other units
						defaultUnits = RevUtils.getPlayerUnits( ix, iy, pRevPlayer.getID() )

						# Give a few defenders
						newUnit = pRevPlayer.initUnit( iBestDefender, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH )
						pRevPlayer.initUnit( iBestDefender, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH )
						pRevPlayer.initUnit( iCounter, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH )
						if(iWorker != -1):
							pRevPlayer.initUnit( iWorker, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH )

						if( pRevPlayer.getNumCities() <= 2 ) :
							# Extra units for first two cities
							#pRevPlayer.initUnit( iAttack, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH )
							pRevPlayer.initUnit( iAttack, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH )
						elif( GC.getTeam(pPlayer.getTeam()).isAtWar(pRevPlayer.getTeam()) ) :
							pRevPlayer.initUnit( iBestDefender, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH )
							#pRevPlayer.initUnit( iBestDefender, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH )

						if not bIsBarbRev and not bGaveMap:
							pRevPlayer.receiveGoody(GC.getMap().plot(ix,iy), iGoodyMap, newUnit)
							pRevPlayer.receiveGoody(GC.getMap().plot(ix,iy), iGoodyMap, newUnit)

						# Remove default given units
						for unit in defaultUnits :
							unit.kill( False, -1 )

						if( not bIsBarbRev ) : RevData.updateCityVal(pCity, 'RevolutionCiv', pRevPlayer.getCivilizationType() )

						# City starts in disorder
						pCity.setOccupationTimer( 2 )

					vassalStyle = revData.dict.get('vassalStyle',None)
					if( not bIsBarbRev and not vassalStyle == None ) :
						if vassalStyle == 'capitulated':
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s is forming as capitulated vassal to %s"%(pRevPlayer.getCivilizationDescription(0),pPlayer.getCivilizationDescription(0)))
							pTeam.assignVassal( pRevPlayer.getTeam(), True )
							GAME.updateScore(True)
						else:
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s is forming as vassal to %s"%(pRevPlayer.getCivilizationDescription(0),pPlayer.getCivilizationDescription(0)))
							pTeam.assignVassal( pRevPlayer.getTeam(), False )
							GAME.updateScore(True)

					if bPeaceful:
						pTeam.signOpenBorders( pRevPlayer.getTeam() )

				else:
					# Join cultural owner
					joinPlayer = GC.getPlayer( revData.dict['iJoinPlayer'] )
					'''
					# Enable only for debugging join human popup
					if pPlayer.isAlive() and (pPlayer.isHuman() or pPlayer.isHumanDisabled()):
						GAME.setForcedAIAutoPlay(pPlayer.getID(), 0, False)
					iPrevHuman = GAME.getActivePlayer()
					RevUtils.changeHuman(joinPlayer.getID(), iPrevHuman)
					'''

					if self.isLocalHumanPlayer(joinPlayer.getID()):
						# Offer human the option of accepting the cities
						popup = CyPopup(RevDefs.joinHumanPopup, EventContextTypes.EVENTCONTEXT_ALL, False)

						popup.setHeaderString(localText.getText("TXT_KEY_REV_TITLE_GOOD_NEWS",()), 1<<2)

						bodStr = getCityTextList(cityList, bPreCitizens = True)

						bodStr += ' ' + localText.getText("TXT_KEY_REV_HUMAN_GRANTED",()) + ' %s!'%(pPlayer.getName())
						bodStr += '  ' + localText.getText("TXT_KEY_REV_HUMAN_REGAIN",()) + ' %s.'%(pRevPlayer.getCivilizationShortDescription(0))
						bodStr += '  ' + localText.getText("TXT_KEY_REV_HUMAN_JOIN_CHOICE",())

						popup.setBodyString(bodStr, 1<<0)
						popup.addButton(localText.getText("TXT_KEY_REV_BUTTON_WELCOME",()))
						buttons = ('welcome',)
						popup.addButton(localText.getText("TXT_KEY_REV_BUTTON_NOT_WANTED",()))
						buttons += ('goaway',)
						popup.setUserData( (buttons, joinPlayer.getID(), iRevoltIdx) )
						# Center camera on city
						CyCamera().JustLookAt( cityList[0].plot().getPoint() )
						popup.launch(False, PopupStates.POPUPSTATE_IMMEDIATE)

					else :
						joinPlayer = GC.getPlayer( revData.dict['iJoinPlayer'] )
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: The %s are claiming their cultural cities"%(joinPlayer.getCivilizationDescription(0)))

						bIsBarbRev = joinPlayer.isBarbarian()

						for iPlayer in xrange(GC.getMAX_PC_PLAYERS()) :
							# Craft revolution anouncement message for all players
							if( GC.getPlayer(iPlayer).canContact(pPlayer.getID()) or iPlayer == pPlayer.getID() ) :

								colorNum = 7 # Red
								pPlayerName = str(localText.getText("TXT_KEY_REV_THE",())) + ' ' + pPlayer.getCivilizationDescription(0)
								joinPlayerName = str(localText.getText("TXT_KEY_REV_THE",())) + ' ' + joinPlayer.getCivilizationDescription(0)

								if( iPlayer == joinPlayer.getID() ) :
									colorNum = 8 # Green
									joinPlayerName = localText.getText("TXT_KEY_REV_YOU",())
								elif( iPlayer == pPlayer.getID() ) :
									pPlayerName = str(localText.getText("TXT_KEY_REV_YOU",()))

								cityString = getCityTextList(cityList)

								mess = localText.getText("TXT_KEY_REV_MESS_CEDE",())%(pPlayerName,cityString,joinPlayerName)

								if( iPlayer == pPlayer.getID() ) :
									CyInterface().addMessage(iPlayer, False, GC.getEVENT_MESSAGE_TIME(), mess, "AS2D_CITY_REVOLT", InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_RESISTANCE").getPath(), ColorTypes(colorNum), cityList[0].getX(), cityList[0].getY(), True, True)
								else :
									CyInterface().addMessage(iPlayer, False, GC.getEVENT_MESSAGE_TIME(), mess, None, InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT, None, ColorTypes(colorNum), -1, -1, False, False)

						if( not bIsBarbRev ) :
							# Improve relations
							joinPlayer.AI_changeAttitudeExtra( pPlayer.getID(), 4 )
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s's Extra Attitude towards %s now %d"%(joinPlayer.getCivilizationDescription(0),pPlayer.getCivilizationDescription(0),joinPlayer.AI_getAttitudeExtra(pPlayer.getID())))

						pRevTeam = GC.getTeam( joinPlayer.getTeam() )

						iNumPlayerCities = pPlayer.getNumCities()

						for pCity in cityList :

							# Move units out of city
							if( pRevTeam.isAtWar(pPlayer.getTeam()) and iNumPlayerCities > len(cityList) ) :
								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Moving owner's units")
								RevUtils.clearOutCity( pCity, pPlayer, pRevPlayer )

							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Acquiring %s, pop %d"%(pCity.getName(),pCity.getPopulation()))

							[iWorker, iBestDefender, iCounter, iAttack] = RevUtils.getHandoverUnitTypes(pCity)

							RevData.updateCityVal(pCity, 'RevolutionCiv', -1 )

#***********************************
							# Acquire city
							#joinPlayer.acquireCity( pCity, False, True )

							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Population of %s before is %d"%(pCity.getName(),pCity.getPopulation()))
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Check city culture is %d, at %d, %d"%(pCity.getCulture(pPlayer.getID()), pCity.getX(),pCity.getY()))
							cityPlot = pCity.plot()
							if( pCity.getCulture( pPlayer.getID() ) == 0 ) :
								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Forcing culture > 0")
								pCity.setCulture( pPlayer.getID(), 1, True )

							try :
								pCity.plot().setOwner( joinPlayer.getID() )
							except :
								print "ERROR in cultural acquisition"
								print "ERROR:  Failed to set owner of city, %s at plot %d, %d "%(pCity.getName(),cityPlot.getX(),cityPlot.getY())
								#print "City culture is %d"%(pCity.getCulture(pPlayer.getID()))

								#pCity = cityPlot.getPlotCity()
								#print "Post culture in %s is %d"%(pCity.getName(),pCity.getCulture(pPlayer.getID()))
								#joinPlayer.acquireCity( pCity, False, False )
								#RevData.initCity(pCity)
								# City has become invalid, will cause game to crash if left
								print "Destroying city so game can continue"
								pCity.kill()
								continue


							pCity = cityPlot.getPlotCity()
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Population of %s after is %d"%(pCity.getName(),pCity.getPopulation()))

							if( pCity.getPopulation() < 1 ) :
								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Error!  City %s is empty"%(pCity.getName()))
#*************************************


							ix = pCity.getX()
							iy = pCity.getY()

							# Record pre-given units for deletion after giving other units
							defaultUnits = RevUtils.getPlayerUnits( ix, iy, joinPlayer.getID() )

							# Give a few defenders
							newUnit = joinPlayer.initUnit( iBestDefender, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH )
							#joinPlayer.initUnit( iBestDefender, ix, iy, UnitAITypes.NO_UNITAI )
							joinPlayer.initUnit( iCounter, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH )
							if( joinPlayer.getNumCities() <= 2 ) :
								# Extra units for first two cities (unlikely in this case)
								if(iWorker != -1):
									joinPlayer.initUnit( iWorker, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH )
								joinPlayer.initUnit( iAttack, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH )
								joinPlayer.initUnit( iAttack, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH )

							elif( GC.getTeam(pPlayer.getTeam()).isAtWar(joinPlayer.getTeam()) ) :
								joinPlayer.initUnit( iBestDefender, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH )
								#joinPlayer.initUnit( iBestDefender, ix, iy, UnitAITypes.NO_UNITAI )

							#joinPlayer.receiveGoody( GC.getMap().plot(ix,iy), iGoodyMap, newUnit )
							#joinPlayer.receiveGoody( GC.getMap().plot(ix,iy), iGoodyMap, newUnit )

							# Remove default units
							for unit in defaultUnits :
								unit.kill( False, -1 )

							if( not bIsBarbRev ) : RevData.updateCityVal(pCity, 'RevolutionCiv', joinPlayer.getCivilizationType() )

							# City starts in disorder
							pCity.setOccupationTimer( 2 )

				# Update score to show new agreements, especially Vassal
				CyInterface().setDirty( InterfaceDirtyBits.Score_DIRTY_BIT, True )

			else:
				if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Player refuses to let civ walk")

				joinPlayer = None
				if( 'iJoinPlayer' in revData.dict.keys() ) :
					joinPlayer = GC.getPlayer( revData.dict['iJoinPlayer'] )

				if( bPeaceful ) :
					# Cities get more pissed off
					if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Peaceful, increasing rev indices")

					# Save civ type for instigator only
					RevData.updateCityVal( cityList[0], 'RevolutionCiv', pRevPlayer.getCivilizationType() )

					for pCity in cityList :
						RevUtils.doRevRequestDeniedPenalty( pCity, capitalArea, iRevIdxInc = 150, bExtraColony = True )

					if( not joinPlayer == None ) :
						joinPlayer.AI_changeAttitudeExtra( pPlayer.getID(), -2 )
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s's Extra Attitude towards %s now %d"%(joinPlayer.getCivilizationDescription(0),pPlayer.getCivilizationDescription(0),joinPlayer.AI_getAttitudeExtra(pPlayer.getID())))

				else :
					# Violent uprising!!!
					if( not joinPlayer == None ) :
						joinPlayer.AI_changeAttitudeExtra( pPlayer.getID(), -3 )
						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s's Extra Attitude towards %s now %d"%(joinPlayer.getCivilizationDescription(0),pPlayer.getCivilizationDescription(0),joinPlayer.AI_getAttitudeExtra(pPlayer.getID())))

						# Check if joinPlayer would like to declare war on player
						[warOdds,attackerTeam,victimTeam] = RevUtils.computeWarOdds( joinPlayer, pPlayer, cityList[0].area(), allowBreakVassal = self.bAllowBreakVassal )
						# TODO: provide support for human war selection
						if( attackerTeam.isHuman() and not attackerTeam.isAtWar(victimTeam.getID()) ) :
							warOdds = 0

						if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: War odds are %d"%(warOdds))

						if( warOdds > 25 and warOdds > GAME.getSorenRandNum(100,'Revolution: war') ) :
							# have joinPlayer declare war on player
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: joinPlayer's team decides to declare war on pPlayer's team!")
							attackerTeam.declareWar(pPlayer.getTeam(),True, WarPlanTypes.NO_WARPLAN)

							revData.dict['iRevPlayer'] = joinPlayer.getID()
							del revData.dict['iJoinPlayer']
							revData.dict['bIsJoinWar'] = True

							revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
							revoltDict[iRevoltIdx] = revData
							RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

							self.prepareRevolution( pPlayer, iRevoltIdx, cityList, joinPlayer, bIsJoinWar = True, switchToRevs = switchToRevs )

						else :
							# Check if pPlayer wants to declare war on joinPlayer
							[warOdds,attackerTeam,victimTeam] = RevUtils.computeWarOdds( pPlayer, joinPlayer, cityList[0].area(), allowBreakVassal = self.bAllowBreakVassal )
							if( attackerTeam.isHuman() and not attackerTeam.isAtWar(victimTeam.getID()) ) :
								warOdds = 0

							if( warOdds > 25 and warOdds > GAME.getSorenRandNum(100,'Revolution: war') ) :
								# pPlayer declares war on joinPlayer!!!
								if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: pPlayer's team decides to declare war on joinPlayer's team!")
								attackerTeam.declareWar(joinPlayer.getTeam(),True, WarPlanTypes.NO_WARPLAN)

								revData.dict['iRevPlayer'] = joinPlayer.getID()
								del revData.dict['iJoinPlayer']
								revData.dict['bIsJoinWar'] = False

								revoltDict = RevData.revObjectGetVal( pPlayer, 'RevoltDict' )
								revoltDict[iRevoltIdx] = revData
								RevData.revObjectUpdateVal( pPlayer, 'RevoltDict', revoltDict )

								self.prepareRevolution( pPlayer, iRevoltIdx, cityList, joinPlayer, bIsJoinWar = False, switchToRevs = switchToRevs )

							else:
								# pRevPlayer revolts against pPlayer
								if not pRevPlayer.isNPC():
									pRevPlayer.AI_changeAttitudeExtra(pPlayer.getID(), -5)
									pRevPlayer.AI_changeAttitudeExtra(joinPlayer.getID(), 5)
									if self.LOG_DEBUG:
										CvUtil.pyPrint("[REV] Revolt: %s's Extra Attitude towards %s now %d"%(pRevPlayer.getCivilizationDescription(0),pPlayer.getCivilizationDescription(0),pRevPlayer.AI_getAttitudeExtra(pPlayer.getID())))
									pPlayer.AI_changeAttitudeExtra(pRevPlayer.getID(), -3)

									GC.getTeam(pRevPlayer.getTeam()).signOpenBorders(joinPlayer.getTeam())
									RevData.revObjectSetVal(pRevPlayer, 'JoinPlayerID', joinPlayer.getID())

								self.prepareRevolution(pPlayer, iRevoltIdx, cityList, pRevPlayer, bIsJoinWar = revData.dict.get('bIsJoinWar',False), switchToRevs = switchToRevs)

					else:
						if revData.dict.get('bOfferPeace', False):
							for pCity in handoverCities :
								if( not pCity.getID() in revData.dict.get('cityList', []) ) :
									if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Bolstering rebellious spirit in %s (handover city only)"%(pCity.getName()))

									revIdx = pCity.getRevolutionIndex()
									localRevIdx = pCity.getLocalRevIndex()
									reinfTurns = pCity.getReinforcementCounter()

									pCity.setRevolutionIndex( max([revIdx+50,revIdx+10*min([localRevIdx,15])]) )
									if( reinfTurns > 2 ) :
										RevData.setCityVal( pCity, 'ReinforcementTurns', max([reinfTurns-(2*revIdx)/self.revInstigatorThreshold,2]) )

						if( not pRevPlayer.isNPC() ) :
							pRevPlayer.AI_changeAttitudeExtra( pPlayer.getID(), -5 )
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: %s's Extra Attitude towards %s now %d"%(pRevPlayer.getCivilizationDescription(0),pPlayer.getCivilizationDescription(0),pRevPlayer.AI_getAttitudeExtra(pPlayer.getID())))
							pPlayer.AI_changeAttitudeExtra( pRevPlayer.getID(), -3 )

						self.prepareRevolution( pPlayer, iRevoltIdx, cityList, pRevPlayer, bIsJoinWar = revData.dict.get('bIsJoinWar',False), switchToRevs = switchToRevs )

		else:
			if self.LOG_DEBUG: print "[REV] Unknown revolution type: " + revType
			return

		try:
			cityStr = getCityTextList(cityList)
			if len(cityList) > 1:
				cityStr2 = localText.getText("TXT_KEY_REV_CITIES", ())
			else:
				cityStr2 = localText.getText("TXT_WORD_CITY", ())
		except:
			if self.LOG_DEBUG: print "[REV] city strings ERROR!"
			return


		mess = localText.getText("TXT_KEY_REV_MESS_REPORT", ())%(cityStr2,cityStr,pPlayer.getCivilizationShortDescription(0))
		if( termsAccepted ) :
			mess += " " + localText.getText("TXT_KEY_REV_MESS_REPORT_EASED", ())
		else :
			mess += " " + localText.getText("TXT_KEY_REV_MESS_REPORT_INCREASED", ())

		for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
			if( (not iPlayer == pPlayer.getID()) and GC.getPlayer(iPlayer).isAlive() ) :
				if( not (revType == "independence" and termsAccepted) ) :
					try :
						iTeam = GC.getPlayer(iPlayer).getTeam()
						if( cityList[0].getEspionageVisibility(iTeam) ) :
							CyInterface().addMessage(iPlayer, False, GC.getEVENT_MESSAGE_TIME(), mess, None, InterfaceMessageTypes.MESSAGE_TYPE_MINOR_EVENT, None, GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), -1, -1, False, False)
							if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Informing %s, who have espionage vis, of this revolt"%(GC.getPlayer(iPlayer).getCivilizationDescription(0)))
					except :
						print "Error!  C++ call failed, end of processRevolution, player %d"%(iPlayer)


		if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Completed processing revolution")

		return


	def controlLostNetworkHandler( self, iPlayer, iNumTurns, newLeaderType ) :
		if self.LOG_DEBUG: print "[REV] Revolt: Handling network lost control of civ popup"

		pPlayer = GC.getPlayer(iPlayer)

		# still some issues here with the passed in newLeaderType... so setting to existing name atm
		newLeaderName = pPlayer.getName()

		if self.LOG_DEBUG: print "[REV] Revolt: Changing leader to %s" % newLeaderName

		if newLeaderType == pPlayer.getLeaderType():

			pPlayer.setName(TextUtil.convertToStr(newLeaderName))

			if self.LOG_DEBUG: print "[REV] Revolt: No leader change, leader's of same type"

			for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
				if pPlayer.canContact(iPlayer):
					mess = '%s '%(newLeaderName) + localText.getText("TXT_KEY_REV_MESS_NEW_LEADER",()) + ' %s!'%(pPlayer.getCivilizationDescription(0))
					CyInterface().addMessage(iPlayer, False, GC.getEVENT_MESSAGE_TIME(), mess, None, InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT, None, ColorTypes(79), -1, -1, False, False)

			# Change personality type
			if GAME.isOption(GameOptionTypes.GAMEOPTION_RANDOM_PERSONALITIES):
				if self.LOG_DEBUG: print "[REV] Revolt: Giving new, random personality by game option"
				RevUtils.changePersonality(pPlayer.getID())
			elif newLeaderName == TextUtil.convertToStr(GC.getLeaderHeadInfo( newLeaderType ).getDescription()):
				if self.LOG_DEBUG: print "[REV] Revolt: Giving back original leader personality"
				RevUtils.changePersonality( pPlayer.getID(), newLeaderType )
			else:
				if self.LOG_DEBUG: print "[REV] Revolt: Giving new, random personality"
				RevUtils.changePersonality( pPlayer.getID() )

		else:
			RevUtils.changeCiv(pPlayer.getID(),pPlayer.getCivilizationType(),newLeaderType)

			for iPlayerTest in xrange(GC.getMAX_PC_PLAYERS()) :
				if pPlayer.canContact(iPlayerTest):
					mess = '%s '%(pPlayer.getName()) + localText.getText("TXT_KEY_REV_MESS_NEW_LEADER",()) + ' %s!'%(pPlayer.getCivilizationDescription(0))
					CyInterface().addMessage(iPlayerTest, False, GC.getEVENT_MESSAGE_TIME(), mess, None, InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT, None, ColorTypes(79), -1, -1, False, False)

		if pPlayer.getCurrentEra() > GC.getNumEraInfos()/2:
			pPlayer.changeAnarchyTurns(3)
		else: pPlayer.changeAnarchyTurns(2)

		if pPlayer.isHuman():
			GAME.setForcedAIAutoPlay(iPlayer, iNumTurns, True)
			SdToolKit.sdObjectSetVal("AIAutoPlay", GAME, "bCanCancelAuto", False)
		RevInstances.AIAutoPlayInst.abdicateMultiCheckNoMessage(iPlayer, iNumTurns)


	def joinHumanHandler(self, iPlayerID, netUserData, popupReturn):
		if self.LOG_DEBUG:
			print "[REV] Revolt: Handling join human popup"

		buttons = netUserData[0]
		buttonLabel = buttons[popupReturn.getButtonClicked()]
		iPlayer = netUserData[1]
		pPlayer = GC.getPlayer(iPlayer)
		iRevoltIdx = netUserData[2]
		revData = RevData.revObjectGetVal(pPlayer, 'RevoltDict')[iRevoltIdx]

		cityList = []
		for iCity in revData.cityList:
			pCity = pPlayer.getCity(iCity)
			if pCity:
				cityList.append(pCity)
			elif self.LOG_DEBUG:
				print "[REV] Revolt: %s no longer owned by former owner" % pCity.getName()

		if not cityList:
			if self.LOG_DEBUG:
				print "[REV] Revolt: No cities left, cancelling"
			return

		revType = revData.revType
		bPeaceful = revData.bPeaceful

		if buttonLabel == 'welcome':
			# Welcome back!
			if self.LOG_DEBUG:
				print "[REV] Revolt: Revolutionaries welcomed with open arms"

			joinPlayer = GC.getPlayer(revData.dict['iJoinPlayer'])

			joinPlayer.AI_changeAttitudeExtra(iPlayer, 2)

			pJoinTeam = GC.getTeam(joinPlayer.getTeam())

			iNumPlayerCities = pPlayer.getNumCities()
			iGoodyMap = GC.getInfoTypeForString(RevDefs.sXMLGoodyMap)

			for pCity in cityList:
				# Move units out of city
				if pJoinTeam.isAtWar(pPlayer.getTeam()) and iNumPlayerCities > len(cityList):
					if self.LOG_DEBUG:
						print "[REV] Revolt: Moving owner's units"
					RevUtils.clearOutCity(pCity, pPlayer, joinPlayer)

				if self.LOG_DEBUG:
					print "[REV] Revolt: Acquiring " + pCity.getName()

				[iWorker, iBestDefender, iCounter, iAttack] = RevUtils.getHandoverUnitTypes(pCity)

				RevData.updateCityVal(pCity, 'RevolutionCiv', -1)

#***********************************
				# Acquire city
				if self.LOG_DEBUG:
					print "[REV] Revolt: Population of %s before is %d" % (pCity.getName(), pCity.getPopulation())
					print "[REV] Revolt: Check city culture is %d, at %d, %d" % (pCity.getCulture(iPlayer), pCity.getX(), pCity.getY())

				cityPlot = pCity.plot()
				if pCity.getCulture(iPlayer) == 0:
					if self.LOG_DEBUG:
						print "[REV] Revolt: Forcing culture > 0"
					pCity.setCulture(iPlayer, 1, True)

				try:
					pCity.plot().setOwner(joinPlayer.getID())
				except:
					print "[ERROR] Failed to set owner of city, %s at plot %d, %d" % (pCity.getName(), cityPlot.getX(), cityPlot.getY())
					print "\tDestroying city so game can continue"
					pCity.kill()
					continue

				pCity = cityPlot.getPlotCity()
				if self.LOG_DEBUG:
					print "[REV] Revolt: Population of %s after is %d" % (pCity.getName(), pCity.getPopulation())

				if self.LOG_DEBUG and pCity.getPopulation() < 1:
					print "[REV] Revolt: Error!  City %s is empty" % pCity.getName()
#*************************************

				# Give a few defenders
				ix = pCity.getX()
				iy = pCity.getY()

				if iBestDefender > -1:
					joinPlayer.initUnit(iBestDefender, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
				if iCounter > -1:
					joinPlayer.initUnit(iCounter, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
				if joinPlayer.getNumCities() <= 2:
					# Extra units for first two cities (unlikely in this case)
					if iWorker > -1:
						joinPlayer.initUnit(iWorker, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
					if iAttack > -1:
						joinPlayer.initUnit(iAttack, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
						joinPlayer.initUnit(iAttack, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
				elif iBestDefender > -1 and GC.getTeam(pPlayer.getTeam()).isAtWar(joinPlayer.getTeam()):
					joinPlayer.initUnit(iBestDefender, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)

				RevData.updateCityVal(pCity, 'RevolutionCiv', joinPlayer.getCivilizationType())

		elif buttonLabel == 'goaway':
			# Go away!
			if self.LOG_DEBUG:
				print "[REV] Revolt: Revolutionaries rebuffed, striking out on their own"
			revData.dict['bIsJoinWar'] = False
			del revData.dict['iJoinPlayer']
			self.processRevolution(pPlayer, iRevoltIdx, cityList, revType, bPeaceful, True)
		else: # WTF?
			print "[WARNING] Revolt: Unexpected button label " + buttonLabel


##--- Revolutionary spawning functions ---------------------------------------

	def prepareRevolution( self, pPlayer, iRevoltIdx, cityList, pRevPlayer, bIsJoinWar = False, switchToRevs = False ) :
		# Store revolution data, so rev starts with new civs turn
		cityIDList = []
		for pCity in cityList :
			cityIDList.append(pCity.getID())

		spawnList = RevData.revObjectGetVal( pRevPlayer, 'SpawnList' )
		spawnList.append([pPlayer.getID(), iRevoltIdx])
		RevData.revObjectSetVal( pRevPlayer, 'SpawnList', spawnList )

		if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: Stored revolt spawn data for rev player %d, revolt against player %d idx %d"%(pRevPlayer.getID(),pPlayer.getID(),iRevoltIdx))

		if( self.isLocalHumanPlayer(pPlayer.getID()) ) :
			# Threatening popup to remind player what's coming

			bodStr = getCityTextList(cityList, bPreCity = True, bPostIs = True) + ' '

			if( pRevPlayer.isAlive() ) :
				bodStr += localText.getText("TXT_KEY_REV_JOINING_WAR",())%(pRevPlayer.getCivilizationDescription(0))
			else :
				bodStr += localText.getText("TXT_KEY_REV_PREPARING_WAR",())

			popupInfo = CyPopupInfo()
			popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
			popupInfo.setText( bodStr )
			popupInfo.addPythonButton("OK",'')
			popupInfo.addPopup(pPlayer.getID())

		# Cause disorder in rebelling cities, injure units now
		for pCity in cityList :
			pCity.setOccupationTimer( 2 )
			pCity.setRevolutionCounter( 2 )

			mess = localText.getText("TXT_KEY_REV_MESS_BREWING",())%(pCity.getName())
			CyInterface().addMessage(pPlayer.getID(), True, GC.getEVENT_MESSAGE_TIME(), mess, "AS2D_CITY_REVOLT", InterfaceMessageTypes.MESSAGE_TYPE_MINOR_EVENT, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_RESISTANCE").getPath(), ColorTypes(7), pCity.getX(), pCity.getY(), True, True)

			unitList = RevUtils.getPlayerUnits( pCity.getX(), pCity.getY(), pPlayer.getID() )
			for unit in unitList :
				if( unit.canFight() ) :
					iPreDamage = unit.getDamage()
					iDamage = iPreDamage/3 + 20 + GAME.getSorenRandNum(15,'Rev - Injure unit')
					iDamage = min([iDamage,90])
					iDamage = max([iDamage,iPreDamage])
					unit.setDamage( iDamage, pRevPlayer.getID() )

		# pRevTeam = GC.getTeam( pRevPlayer.getTeam() )
		# if( not pRevTeam.isAtWar(pPlayer.getTeam()) ) :
			# pRevTeam.declareWar( pPlayer.getTeam(), True, WarPlanTypes.NO_WARPLAN )
			# if( self.LOG_DEBUG ) : CvUtil.pyPrint("[REV] Revolt: The %s revolutionaries declare war on the %s!"%(pRevPlayer.getCivilizationAdjective(0),pPlayer.getCivilizationDescription(0)))


	def launchRevolution(self, iRevPlayer):

		pRevPlayer = GC.getPlayer(iRevPlayer)

		if not RevData.revObjectExists(pRevPlayer):
			print "[REV] Error: Launch revolution called for player %d with no rev instance" % iRevPlayer
			assert(False)

		spawnList = RevData.revObjectGetVal(pRevPlayer, 'SpawnList')
		newSpawnList = []

		if spawnList == None or len(spawnList) == 0:
			print "[REV] Error:  Launch revolution called for player %d with no rev data" % iRevPlayer
			assert(False)

		for [iPlayer, iRevoltIdx] in spawnList:

			pPlayer = GC.getPlayer(iPlayer)

			if not pPlayer.isAlive():
				print "[REV] WARNING! Player %d is dead, revolt %d canceled"%(iPlayer, iRevoltIdx)
				continue

			try:
				revoltData = RevData.revObjectGetVal(pPlayer, 'RevoltDict')[iRevoltIdx]
			except KeyError:
				print "[REV] Error! Player %d does not have a revolt of index %d"%(iPlayer, iRevoltIdx)
				continue

			if revoltData.iRevTurn <= GAME.getGameTurn():
				# It's on!  Spawn revolutionaries
				if not revoltData.dict.get('iRevPlayer',-1) == pRevPlayer.getID():
					print "[REV] Error! pRevPlayer %d does not match revolt data iRevPlayer %d"%(pRevPlayer.getID(), revoltData.dict.get('iRevPlayer',-1))

				cityIDList = revoltData.cityList
				bIsJoinWar = revoltData.dict.get('bIsJoinWar', False)
				switchToRevs = revoltData.dict.get('bSwitchToRevs', False)

				cityList = []
				for iCity in cityIDList:
					try:
						cityList.append(pPlayer.getCity(iCity))
					except:
						print "[REV] Error:  Couldn't find city #%d"%(iCity)

				if self.LOG_DEBUG:
					print "[REV] Found launchable revolt spawn data for player %d, starting spawn against player %d idx %d"%(pRevPlayer.getID(),iPlayer,iRevoltIdx)

				self.spawnRevolutionaries(cityList, pPlayer, pRevPlayer, bIsJoinWar, switchToRevs)

			else:
				newSpawnList.append([iPlayer,iRevoltIdx])

		RevData.revObjectSetVal(pRevPlayer, 'SpawnList', newSpawnList)

	def spawnRevolutionaries(self, cityList, pPlayer, pRevPlayer, bIsJoinWar = False, switchToRevs = False):

		if self.LOG_DEBUG:
			print "[REV] Spawning revolutionaries for %d cities in %s"%(len(cityList), pPlayer.getCivilizationDescription(0))

		if pPlayer.isNPC():
			print "[REV] Error: attempted spawning of revs for NPC player"
			return

		if pRevPlayer.isNPC() and not pRevPlayer.isBarbarian():
			print "[REV] Error: attempted to spawn non barbarian NPC revolutionaries!"
			return

		bIsBarbRev = pRevPlayer.isBarbarian()
		bGaveMap = False

		pTeam = GC.getTeam(pPlayer.getTeam())
		pRevTeam = GC.getTeam(pRevPlayer.getTeam())

		# Check which cities are still up for revolt
		newCityList = []
		for pCity in cityList:
			if pCity == None:
				print "[WARN] A rebelling city is dead and gone"
			elif not pCity.getOwner() == pPlayer.getID():
				if self.LOG_DEBUG:
					CvUtil.pyPrint("[REV] Revolt: %s no longer controlled by %s, no revolt"%(pCity.getName(),pPlayer.getCivilizationDescription(0)))
			else:
				newCityList.append(pCity)

		cityList = newCityList

		# Order by rev index after instigator
		if len(cityList) > 2:
			revIdxCityList = []
			for pCity in cityList[1:]:
				revIdx = pCity.getRevolutionIndex()
				revIdxCityList.append(pCity)

			revIdxCityList.sort(key=lambda i: (i.getRevolutionIndex(), i.getName()))
			revIdxCityList.reverse()

			newCityList = []
			# Start with instigator
			newCityList.append(cityList[0])
			for cityEntry in revIdxCityList:
				newCityList.append(cityEntry)

			cityList = newCityList

		if cityList:
			cityString = ''
			for pCity in cityList:
				cityString += pCity.getName() + ', '
			if self.LOG_DEBUG:
				CvUtil.pyPrint("[REV] Revolt: Cities in revolt: " + cityString)
		else:
			print "[WARN] Revolt cancelled cause no cities in updated list!"
			return

		if not bIsBarbRev and switchToRevs == True:
			iPrevHuman = pPlayer.getID()
			RevUtils.changeHuman(pRevPlayer.getID(), iPrevHuman)

		if not bIsBarbRev and not bIsJoinWar and pRevPlayer.getNumCities() < 4:
			# Record rev turn for this player
			RevData.initPlayer( pRevPlayer )
			RevData.revObjectSetVal(pRevPlayer, 'RevolutionTurn', GAME.getGameTurn())

			if len(cityList) < 3:
				cityString = TextUtil.convertToStr(cityList[0].getName())
			else:
				cityString = None

			if not pRevPlayer.isAlive():
				RevData.revObjectSetVal(pRevPlayer, 'CapitalName', cityString)

			if RevData.revObjectGetVal(pRevPlayer, 'MotherlandID') == None:
				RevData.revObjectSetVal(pRevPlayer, 'MotherlandID', pPlayer.getID())

		bJoinRev = True
		if not pRevPlayer.isAlive() and not bIsBarbRev:
			# Fires naming logic for new civ, so messages get the right name
			# Must call setNewPlayerAlive to avoid having DLL set this player alive with setPlayerAlive which calls its turn and the turns of all players with higher numbers
			# Instead, this way makes it alive so it takes its next turn in turn
			# Add replay message
			mess = localText.getText("TXT_KEY_REV_MESS_VIOLENT",()) + ' ' + pPlayer.getCivilizationDescription(0) + '!'
			mess += "  " + localText.getText("TXT_KEY_REV_BIG_THE",()) + ' ' + pRevPlayer.getCivilizationDescription(0) + ' ' + localText.getText("TXT_KEY_REV_MESS_RISEN",())
			GAME.addReplayMessage( ReplayMessageTypes.REPLAY_MESSAGE_MAJOR_EVENT, pRevPlayer.getID(), mess, cityList[0].getX(), cityList[0].getY(), GC.getInfoTypeForString("COLOR_WARNING_TEXT"))

			pRevPlayer.setIsRebel(True)
			if pPlayer.isMinorCiv():
				print "[REV] Revolt: Setting new rebel player as minor civ since motherland is a minor civ"
				pRevTeam.setIsMinorCiv(True, False)
			else:
				if pRevPlayer.isMinorCiv():
					pRevTeam.setIsMinorCiv(False, False)
			pRevPlayer.setNewPlayerAlive(True)

			bJoinRev = False

		for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
			if not GC.getPlayer(iPlayer):
				continue
			# Craft revolution anouncement message for all players
			if GC.getPlayer(iPlayer).canContact(pPlayer.getID()) or iPlayer == pPlayer.getID():
				colorNum = 7
				if bIsBarbRev:
					if iPlayer == pPlayer.getID():
						colorNum = 7
						mess = "<color=255,0,0,255>" + localText.getText("TXT_KEY_REV_MESS_YOU_BARB",())
					else:
						colorNum = 7
						mess = "<color=255,0,0,255>" + localText.getText("TXT_KEY_REV_MESS_VIOLENT",()) + ' ' + pPlayer.getCivilizationDescription(0) + '!!!'
						mess += "  " + localText.getText("TXT_KEY_REV_MESS_BARB",())
				else:
					if iPlayer == pPlayer.getID():
						colorNum = 7
						mess = "<color=255,0,0,255>"
						if bJoinRev:
							mess += localText.getText("TXT_KEY_REV_MESS_JOIN",()) + ' ' + pRevPlayer.getCivilizationDescription(0) + ' ' + localText.getText("TXT_KEY_REV_MESS_YOU_JOIN",())
						else:
							mess += localText.getText("TXT_KEY_REV_BIG_THE",()) + ' ' + pRevPlayer.getCivilizationDescription(0) + ' ' + localText.getText("TXT_KEY_REV_MESS_YOU_RISEN",())
					else:
						mess = ""

						if iPlayer == pRevPlayer.getID():
							mess += "<color=0,255,0,255>"
							colorNum = 8
						else:
							mess += "<color=255,0,0,255>"
							colorNum = 7

						mess += localText.getText("TXT_KEY_REV_MESS_VIOLENT",()) + ' ' + pPlayer.getCivilizationDescription(0) + '!!!'
						if bJoinRev:
							mess += "  " + localText.getText("TXT_KEY_REV_MESS_JOIN",()) + ' ' + pRevPlayer.getCivilizationDescription(0) + ' ' + localText.getText("TXT_KEY_REV_MESS_JOIN2",())
						else:
							mess += "  " + localText.getText("TXT_KEY_REV_BIG_THE",()) + ' ' + pRevPlayer.getCivilizationDescription(0) + ' ' + localText.getText("TXT_KEY_REV_MESS_RISEN",())

				if iPlayer == pPlayer.getID():
					CyInterface().addMessage(iPlayer, True, GC.getEVENT_MESSAGE_TIME(), mess, "AS2D_CITY_REVOLT", InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT, CyArtFileMgr().getInterfaceArtInfo("INTERFACE_RESISTANCE").getPath(), ColorTypes(colorNum), cityList[0].getX(), cityList[0].getY(), True, True)
				elif iPlayer == pRevPlayer.getID():
					CyInterface().addMessage(iPlayer, True, GC.getEVENT_MESSAGE_TIME(), mess,  "AS2D_DECLAREWAR", InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT, None, ColorTypes(colorNum), cityList[0].getX(), cityList[0].getY(), False, False)
				else:
					CyInterface().addMessage(iPlayer, False, GC.getEVENT_MESSAGE_TIME(), mess,  "AS2D_DECLAREWAR", InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT, None, ColorTypes(colorNum), -1, -1, False, False)

		if not bIsBarbRev:

			pRevTeam = GC.getTeam(pRevPlayer.getTeam())
			if not pRevTeam.isAtWar(pPlayer.getTeam()):
				pRevTeam.declareWar(pPlayer.getTeam(), True, WarPlanTypes.WARPLAN_TOTAL)
				if self.LOG_DEBUG:
					CvUtil.pyPrint("[REV] Revolt: The %s revolutionaries declare war and start a revolution against the %s!"%(pRevPlayer.getCivilizationAdjective(0),pPlayer.getCivilizationDescription(0)))
				pRevPlayer.setIsRebel(True)
				pRevTeam.setRebelAgainst(pPlayer.getTeam(), True)
			elif pRevPlayer.isMinorCiv() and not bIsJoinWar:
				if self.LOG_DEBUG:
					CvUtil.pyPrint("[REV] Revolt: The %s revolutionaries declare war and start a revolution against the %s!"%(pRevPlayer.getCivilizationAdjective(0),pPlayer.getCivilizationDescription(0)))
				pRevPlayer.setIsRebel(True)
				pRevTeam.setRebelAgainst(pPlayer.getTeam(), True)
			elif self.LOG_DEBUG:
				CvUtil.pyPrint("[REV] Revolt: The %s revolutionaries join in the war against the %s!"%(pRevPlayer.getCivilizationAdjective(0),pPlayer.getCivilizationDescription(0)))

			# Money
			if pRevPlayer.getGold() < 200:
				iGold = 30 + GAME.getSorenRandNum(30*len(cityList),'Revolt: give gold')
			else:
				iGold = 10 + GAME.getSorenRandNum(20*len(cityList),'Revolt: give gold')
			pRevPlayer.changeGold(min([iGold, 200]))

			# Espionage
			if not bIsJoinWar:
				espPoints = GAME.getSorenRandNum(20*len(cityList),'Revolt: esp') + (12+len(cityList))*max([pPlayer.getCommerceRate( CommerceTypes.COMMERCE_ESPIONAGE ), 6])
				if pRevTeam.isAlive():
					espPoints /= 2
				pRevTeam.changeCounterespionageTurnsLeftAgainstTeam(pTeam.getID(), 10)
				pRevTeam.changeEspionagePointsAgainstTeam(pTeam.getID(), espPoints)
				pTeam.changeEspionagePointsAgainstTeam(pRevTeam.getID(), espPoints/(3 + pTeam.getAtWarCount(True)))
				if self.LOG_DEBUG:
					CvUtil.pyPrint("[REV] Revolt: Giving rebels %d espionage points against motherland"%(espPoints))
				if not pRevTeam.isAlive():
					for k in xrange(GC.getMAX_PC_TEAMS()):
						if GC.getTeam(k) == None:
							continue
						if pRevTeam.isAtWar(k) and not GC.getTeam(k).isMinorCiv():
							pRevTeam.changeEspionagePointsAgainstTeam(k, GAME.getSorenRandNum(espPoints/2,'Revolt: esp') )
							GC.getTeam(k).changeEspionagePointsAgainstTeam(pRevTeam.getID(), GAME.getSorenRandNum(espPoints/5, 'Revolt: esp'))

			# Diplomacy
			if pRevTeam.isMapTrading():
				# Give motherlands map
				bGaveMap = True
				MAP = GC.getMap()
				for pPlot in MAP.plots():
					if pPlot.isRevealed(pTeam.getID(), False):
						pPlot.setRevealed(pRevTeam.getID(), True, False, pTeam.getID())

				# Meet players known by motherland
				for k in xrange(GC.getMAX_PC_TEAMS()) :
					kTeam = GC.getTeam(k)
					if kTeam == None or kTeam.getLeaderID() < 0 or kTeam.getLeaderID() > GC.getMAX_PC_PLAYERS():
						continue
					kPlayer = GC.getPlayer(kTeam.getLeaderID())
					if kPlayer == None:
						continue
					if pTeam.isHasMet(k) and not k == pRevPlayer.getTeam() and not k == pTeam.getID():
						if pTeam.isAtWar(k):
							pRevTeam.meet(k, False)
							pRevPlayer.AI_changeAttitudeExtra(kTeam.getLeaderID(), 2)
							kPlayer.AI_changeAttitudeExtra(pRevPlayer.getID(), 2)
						elif GAME.getSorenRandNum(100,'odds') > 50:
							pRevTeam.meet(k,False)
							if kPlayer.AI_getAttitude(pPlayer.getID()) == AttitudeTypes.ATTITUDE_FRIENDLY:
								kPlayer.AI_changeAttitudeExtra(pRevPlayer.getID(), -2)


		if self.LOG_DEBUG:
			CvUtil.pyPrint("[REV] Revolt: Spawning %s revolutionaries!!!"%(pRevPlayer.getCivilizationAdjective(0)))

		iGoodyMap = GC.getInfoTypeForString(RevDefs.sXMLGoodyMap)
		iGeneral = GC.getInfoTypeForString(RevDefs.sXMLGeneral)
		iSpy = pRevPlayer.getBestUnitType(UnitAITypes.UNITAI_SPY)
		iSettler = pRevPlayer.getBestUnitType(UnitAITypes.UNITAI_SETTLE)
		iScout = pRevPlayer.getBestUnitType(UnitAITypes.UNITAI_EXPLORE)

		iRevIdxInc = 200

		for [cityIdx, pCity] in enumerate(cityList):

			revIdx = pCity.getRevolutionIndex()
			localRevIdx = pCity.getLocalRevIndex()
			ix = pCity.getX()
			iy = pCity.getY()

			if self.LOG_DEBUG:
				CvUtil.pyPrint("[REV] Revolt: In %s, with rev idx %d (%d local)"%(pCity.getName(),revIdx,localRevIdx))

			spawnableUnits = RevUtils.getUprisingUnitTypes(pCity)
			[iWorker, iBestDefender, iCounter, iAttack] = RevUtils.getHandoverUnitTypes(pCity)

			if not spawnableUnits:
				print "[ERROR] No rev units possible in " + pCity.getName()
				continue

			pCity.setOccupationTimer(1)

			# First look just for rebel, homeland, or unowned territory to spawn in
			spawnablePlots = RevUtils.getSpawnablePlots( ix, iy, pRevPlayer, bLand = True, bIncludePlot = False, bIncludeCities = False, bSameArea = True, iRange = 1, iSpawnPlotOwner = pRevPlayer.getID(), bCheckForEnemy = True, bAtWarPlots = False, bOpenBordersPlots = False )
			if not spawnablePlots:
				# Try plots owner by other players, either with open borders or at war with rebel
				spawnablePlots = RevUtils.getSpawnablePlots( ix, iy, pRevPlayer, bLand = True, bIncludePlot = False, bIncludeCities = False, bSameArea = True, iRange = 1, iSpawnPlotOwner = pRevPlayer.getID(), bCheckForEnemy = True, bAtWarPlots = True )
			if not spawnablePlots:
				# Check if plots are available if we move opposing units
				spawnablePlots = RevUtils.getSpawnablePlots( ix, iy, pRevPlayer, bLand = True, bIncludePlot = False, bIncludeCities = False, bSameArea = True, iRange = 1, iSpawnPlotOwner = pRevPlayer.getID(), bCheckForEnemy = False, bAtWarPlots = True )
			if not spawnablePlots:
				# Expand search area
				spawnablePlots = RevUtils.getSpawnablePlots( ix, iy, pRevPlayer, bLand = True, bIncludePlot = False, bIncludeCities = False, bSameArea = True, iRange = 2, iSpawnPlotOwner = pRevPlayer.getID(), bCheckForEnemy = True )
			if not spawnablePlots:
				# Put them anywhere nearby, this will only fail on single plot islands
				spawnablePlots = RevUtils.getSpawnablePlots( ix, iy, pRevPlayer, bLand = True, bIncludePlot = False, bIncludeCities = False, bSameArea = True, iRange = 3, iSpawnPlotOwner = -1, bCheckForEnemy = False )

			pCity.setOccupationTimer(0)

			revSpawnLoc = None
			if spawnablePlots:
				revSpawnLoc = spawnablePlots[GAME.getSorenRandNum(len(spawnablePlots),'Revolution: Pick rev plot')]

			# if( pPlayer.getID() == GAME.getActivePlayer() or pRevPlayer.getID() == GAME.getActivePlayer() ) :
				#Center camera on city
				# if( not revSpawnLoc == None ) :
					# CyCamera().LookAt( pCity.plot().getPoint(), CameraLookAtTypes.CAMERALOOKAT_CITY_ZOOM_IN, GC.getMap().plot(revSpawnLoc[0],revSpawnLoc[1]).getPoint() )
				# else :
					# CyCamera().LookAt( pCity.plot().getPoint(), CameraLookAtTypes.CAMERALOOKAT_CITY_ZOOM_IN, pCity.plot().getPoint() )

			# New unit count method
			localFactor = min([localRevIdx+2,1.5*self.badLocalThreshold])
			if pRevPlayer.getCivilizationType() == RevData.getCityVal(pCity, 'RevolutionCiv'):
				if self.LOG_DEBUG:
					CvUtil.pyPrint("[REV] Revolt: Repeat revolution, increasing enlistment")
				effPop = 1.2*pow(pCity.getPopulation(),.8) + 1.0
			else:
				effPop = pow(pCity.getPopulation(),.8) + 1.0

			popMod = (revIdx/(1.0*self.alwaysViolentThreshold))
			if bIsBarbRev or bIsJoinWar:
				popMod = max([ 0.2, pow(max([popMod-.35,0]),.5)/2.0])
			else:
				popMod = max([ 0.3, pow(max([popMod-.35,0]),.5)])

			if self.LOG_DEBUG:
				CvUtil.pyPrint("[REV] Revolt: Population modifier: %.3f on eff pop %.2f (%d)"%(popMod,effPop,pCity.getPopulation()))

			iNumUnits2a = (popMod*(effPop))/2.0
			if bIsBarbRev or bIsJoinWar:
				iNumUnits2b = ((localFactor-self.badLocalThreshold)/(1.0*self.badLocalThreshold))*pow(RevUtils.getNumDefendersNearPlot(ix,iy,pPlayer.getID())/2.0,.5)/2.0
			else:
				iNumUnits2b = (localFactor/(1.0*self.badLocalThreshold))*pow(RevUtils.getNumDefendersNearPlot(ix,iy,pPlayer.getID()),.5)/2.0
			if iNumUnits2b < 0:
				iNumUnits2b = max([iNumUnits2b,-iNumUnits2a/2.0,-2.0])
			iNumUnits2 = int(self.strengthModifier*(iNumUnits2a + iNumUnits2b) + .5)

			if self.LOG_DEBUG:
				CvUtil.pyPrint("[REV] Revolt: New method from pop: %.2f, from troops: %.2f, total: %d"%(iNumUnits2a,iNumUnits2b,iNumUnits2))

			iNumUnits = iNumUnits2

			iNumDefenders = RevUtils.getNumDefendersNearPlot(ix,iy,pPlayer.getID())

			if cityIdx > 1 and iNumUnits > 2:
				# Third or higher city in large revolt, cities should be in rev index order so these should be less fervent
				if self.LOG_DEBUG:
					CvUtil.pyPrint("[REV] Revolt: Reducing number of rebel troops for large revolt")
				iNumUnits = iNumUnits - cityIdx/2
				iNumUnits = max([iNumUnits, 2])

			'''
			unitAdjust = max([3, pCity.getPopulation()/4, iNumDefenders])
			if bIsJoinWar:
				# Joining established player in war
				iNumUnits = min( [iNumUnits, max([pCity.getPopulation()/3,iNumUnits/2,1]), unitAdjust] )
				iNumUnits = max( [iNumUnits, 1 - cityIdx, 0] )
			elif bIsBarbRev:
				# Lower max number of barb units
				iNumUnits = min( [iNumUnits - 1, max([pCity.getPopulation()/2,iNumUnits/2,1]), unitAdjust] )
				iNumUnits = max( [iNumUnits, 2 - cityIdx, 0] )
			else:
				#iNumUnits = min( [iNumUnits, pCity.getPopulation(), 2*unitAdjust] )
				iNumUnits = max( [iNumUnits, 2 - cityIdx, 0] )
			'''
			iNumUnits = max([iNumUnits, 2 - cityIdx, 0])


			if self.LOG_DEBUG:
				CvUtil.pyPrint("[REV] Revolt: Revised enlistment: %d, num defenders nearby: %d" % (iNumUnits, iNumDefenders))

			revControl = False
			# Determine who controls city, revs or player

			if iNumUnits == 0:
				# No actual rebels for this city, just disorder
				revControl = False
				if revSpawnLoc == None:
					if self.LOG_DEBUG:
						CvUtil.pyPrint("[REV] Revolt: No where to spawn rebels, but no rebel units to spawn either ... faking spawn location")
					revSpawnLoc = [0, 0]
			elif pCity.plot().getNumDefenders(pPlayer.getID()) == 0:
				if self.LOG_DEBUG:
					CvUtil.pyPrint("[REV] Revolt: City has no defenders, revs control")
				revControl = True
			elif revSpawnLoc == None:
				# If no plot on which to spawn revs, they get city and owners units flee
				# TODO: What about bIsJoinWar case?
				if self.LOG_DEBUG:
					CvUtil.pyPrint("[REV] Revolt: No where to spawn rebels, so they get city")
				revControl = True
			elif True:
				# Config option
				revControl = False
			else:
				# Compare strength of revolution and garrison
				iRevStrength = iNumUnits
				if pCity.unhappyLevel(0) - pCity.happyLevel() > 0:
					iRevStrength += 2
				if bIsJoinWar:
					iRevStrength -= 2
				if bIsBarbRev:
					iRevStrength -= 4
				if pCity.isCapital():
					iRevStrength -= 1

				iGarrisonStrength = pCity.plot().getNumDefenders(pPlayer.getID()) + 1
				iGarrisonStrength = int( iGarrisonStrength*(110 + pCity.getBuildingDefense())/100.0 )

				if self.LOG_DEBUG:
					CvUtil.pyPrint("[REV] Revolt: Rev strength: %d,  Garrison strength: %d"%(iRevStrength,iGarrisonStrength))

				if iRevStrength > iGarrisonStrength:
					# Revolutionaries out muscle the city garrison and take control
					revControl = True
				else:
					# Spawn in countryside
					revControl = False

			if revControl:
				if self.LOG_DEBUG:
					CvUtil.pyPrint("[REV] Revolt: Revs take control of %s (%d,%d)"%(pCity.getName(),pCity.getX(),pCity.getY()))

				# Turn off rebellious city capture logic, all components handled here
				RevData.updateCityVal(pCity, 'RevolutionCiv', -1)

				# Run wounded soldiers out of Town, try to place near city
				iInjureMax = 40
				retreatPlots = RevUtils.getSpawnablePlots( ix, iy, pPlayer, bLand = True, bIncludePlot = False, bIncludeCities = True, bSameArea = True, iRange = 2, iSpawnPlotOwner = pPlayer.getID(), bCheckForEnemy = True, bAtWarPlots = False, bOpenBordersPlots = False )
				if not retreatPlots:
					iInjureMax = 60
					retreatPlots = RevUtils.getSpawnablePlots( ix, iy, pPlayer, bLand = True, bIncludePlot = False, bIncludeCities = True, bSameArea = False, iRange = 2, iSpawnPlotOwner = pPlayer.getID(), bCheckForEnemy = True, bAtWarPlots = True )
				if not retreatPlots:
					iInjureMax = 65
					retreatPlots = RevUtils.getSpawnablePlots( ix, iy, pPlayer, bLand = True, bIncludePlot = False, bIncludeCities = True, bSameArea = False, iRange = 4, iSpawnPlotOwner = pPlayer.getID(), bCheckForEnemy = True, bAtWarPlots = True )
				if not retreatPlots:
					iInjureMax = 70
					# Try to move to another of players cities
					if self.LOG_DEBUG:
						CvUtil.pyPrint("[REV] Revolt: No nearby plots, trying move to another of players cities")
					for pOtherCity in pPlayer.cities():
						if pOtherCity.getID() != pCity.getID():
							retreatPlots.append([pOtherCity.getX(), pOtherCity.getY()])

				if retreatPlots:
					retreatLoc = retreatPlots[GAME.getSorenRandNum(len(retreatPlots),'Revolution: Pick rev plot')]
					if self.LOG_DEBUG:
						CvUtil.pyPrint("[REV] Revolt: City garrison retreating to %d,%d"%(retreatLoc[0], retreatLoc[1]))
				else:
					retreatLoc = None
					if self.LOG_DEBUG:
						CvUtil.pyPrint("[REV] Revolt: City garrison massacred (had nowhere to go)")
					# pPlayer is about to die anyway

				# Defections?
				toRebelList = []
				if not retreatLoc == None:
					RevUtils.moveEnemyUnits(ix, iy, pRevPlayer.getID(), retreatLoc[0], retreatLoc[1], iInjureMax = iInjureMax, bDestroyNonLand = False, bLeaveSiege = False)
					unitList = RevUtils.getEnemyUnits(ix, iy, pRevPlayer.getID())
					for unit in unitList:
						# TODO: if units is captureable ...
						if 35 > GAME.getSorenRandNum(100,'Revolution: give rebels equipment'):
							if self.LOG_DEBUG:
								CvUtil.pyPrint("[REV] Revolt: Will be giving rebels %s" % unit.getName())
							toRebelList.append(unit.getUnitType())
						else:
							if self.LOG_DEBUG:
								CvUtil.pyPrint("[REV] Revolt: Destroying %s" % unit.getName())
							unit.kill(False, pRevPlayer.getID())

				# Store building types in city
				buildingList = []
				for buildingType in xrange(GC.getNumBuildingInfos()):
					if pCity.getNumRealBuilding(buildingType) > 0:
						buildingInfo = GC.getBuildingInfo(buildingType)
						buildingList.append([buildingType,pCity.getNumRealBuilding(buildingType)])

# ***************************************************************
				# Acquire city by cultural conversion
				#pRevPlayer.acquireCity( pCity, False, False )
				if self.LOG_DEBUG:
					CvUtil.pyPrint("[REV] Revolt: Population of %s before is %d"%(pCity.getName(),pCity.getPopulation()))
					CvUtil.pyPrint("[REV] Revolt: Check city culture is %d, at %d, %d"%(pCity.getCulture(pPlayer.getID()), pCity.getX(),pCity.getY()))

				cityPlot = pCity.plot()
				if pCity.getCulture(pPlayer.getID()) == 0:
					if self.LOG_DEBUG:
						CvUtil.pyPrint("[REV] Revolt: Forcing culture > 0")
					pCity.setCulture(pPlayer.getID(), 1, True)

				try:
					pCity.plot().setOwner(pRevPlayer.getID())
				except:
					print "Error in violent takeover"
					print "ERROR:  Failed to set owner of city, %s at plot %d, %d (%d,%d)"%(pCity.getName(),cityPlot.getX(),cityPlot.getY(),ix,iy)
					#print "City culture is %d"%(pCity.getCulture(pPlayer.getID()))

					#pCity = cityPlot.getPlotCity()
					#print "Post culture in %s is %d"%(pCity.getName(),pCity.getCulture(pPlayer.getID()))
					#pRevPlayer.acquireCity( pCity, False, False )
					# No more revolutions for a while
					#RevData.initCity(pCity)
					# City has become invalid, will cause game to crash if left
					print "Destroying city so game can continue"
					pCity.kill()
					continue


				pCity = cityPlot.getPlotCity()
				if self.LOG_DEBUG:
					CvUtil.pyPrint("[REV] Revolt: Population of %s after is %d"%(pCity.getName(),pCity.getPopulation()))

				if pCity.getPopulation() < 1:
					print "[ERROR] Revolution.py\n\t City %s is empty" % pCity.getName()

				# To kill off auto-spawned defenders after creating new defenders
				defaultUnits = RevUtils.getPlayerUnits(ix, iy, pRevPlayer.getID())

#****************************************************************

				if pCity.getPopulation() > 3 + pPlayer.getCurrentEra():
					pCity.setOccupationTimer(3)
				else:
					pCity.setOccupationTimer(2)

				newCulVal = int( self.revCultureModifier*max([pCity.getCulture(pPlayer.getID()),pCity.countTotalCultureTimes100()/200]) )
				newPlotVal = int( self.revCultureModifier*max([pCity.plot().getCulture(pPlayer.getID()),pCity.plot().countTotalCulture()/2]) )
				RevUtils.giveCityCulture( pCity, pRevPlayer.getID(), newCulVal, newPlotVal)

				newUnitList = []

				if self.LOG_DEBUG:
					CvUtil.pyPrint("[REV] Revolt: %s, pop. %d has %d defenders after removing defaults"%(pCity.getName(),pCity.getPopulation(),pCity.plot().getNumDefenders(pCity.getOwner())))

				# Create stolen equipment
				for unitType in toRebelList:
					newUnit = pRevPlayer.initUnit(unitType, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
					newUnitList.append(newUnit)

				# Spawn revs in the city, plus an extra defender or two
				# Couple revs die in taking control of city:
				if iNumUnits > 9:
					iNumUnits -= 2
				elif iNumUnits > 5:
					iNumUnits -= 1

				if self.LOG_DEBUG:
					CvUtil.pyPrint("[REV] Revolt: Spawning %d units for city of size %d"%(iNumUnits,pCity.getPopulation()))

				for i in xrange(iNumUnits):
					newUnitID = spawnableUnits[GAME.getSorenRandNum( len(spawnableUnits), 'Revolution: pick unit' )]
					newUnit = pRevPlayer.initUnit(newUnitID, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
					newUnitList.append(newUnit)

				# Give a few extra defenders and a worker
				if cityIdx == 0:
					newUnitList.append(pRevPlayer.initUnit(iBestDefender, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH))
				if iWorker != -1:
					pRevPlayer.initUnit(iWorker, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)
					if pCity.getPopulation() > 5:
						pRevPlayer.initUnit(iWorker, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)

				# Injure units to simulate the lack of training in rebel troops
				for newUnit in newUnitList:
					if newUnit.canFight():
						iDamage = 15 + GAME.getSorenRandNum(25,'Rev - Injure unit')
						newUnit.setDamage(iDamage, pPlayer.getID())

				# Remove the default given defenders
				for pUnit in defaultUnits:
					pUnit.kill(False,-1)

				if pCity.getPopulation() > 4 and len(newUnitList) >= 4:
					deltaPop = int((len(newUnitList)-1)/3.0)
					pCity.changePopulation(-deltaPop)
					if self.LOG_DEBUG:
						CvUtil.pyPrint("[REV] Revolt: City population decreased by %d for %d rebel units spawned"%(deltaPop,len(newUnitList)))

				# Extra stuff for instigator city
				if cityIdx == 0 and len(cityList) > 1 and not bIsBarbRev:
					if 10 + 3*len(cityList) + 5*pCity.getNumRevolts(pCity.getOwner()) < GAME.getSorenRandNum(100,'Rev'):
						if self.LOG_DEBUG:
							CvUtil.pyPrint("[REV] Revolt: Great General (%d) spawned in %s"%(iGeneral,pCity.getName()))
						pRevPlayer.initUnit(iGeneral, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)

				if (iSpy != -1 and pRevPlayer.canTrain(iSpy, False, False)
				and pRevPlayer.AI_getNumAIUnits(UnitAITypes.UNITAI_SPY) < 3):
					iNumRevolts = pCity.getNumRevolts(pCity.getOwner())
					if (iNumRevolts > 1 and revIdx > self.alwaysViolentThreshold
					or iNumRevolts > 2 and revIdx > self.revInstigatorThreshold):
						if self.LOG_DEBUG:
							CvUtil.pyPrint("[REV] Revolt: Spy spawned in %s" % pCity.getName())
						pRevPlayer.initUnit(iSpy, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)

				# Should buildings stay or some destroyed?
				for [buildingType,iNum] in buildingList:
					if pCity.getNumRealBuilding(buildingType) < iNum:
						buildingInfo = GC.getBuildingInfo(buildingType)
						if not buildingInfo.isGovernmentCenter():
							if self.LOG_DEBUG:
								CvUtil.pyPrint("[REV] Revolt: Building %s saved"%(buildingInfo.getDescription()))
							pCity.setNumRealBuilding(buildingType, iNum)

				# Reveal surrounding countryside
				if not bGaveMap:
					if self.LOG_DEBUG: CvUtil.pyPrint("[REV] Revolt: Giving map")
					pRevPlayer.receiveGoody( GC.getMap().plot(ix,iy), iGoodyMap, newUnitList[0] )
					pRevPlayer.receiveGoody( GC.getMap().plot(ix,iy), iGoodyMap, newUnitList[0] )

				# No more revolutions for a while
				pCity.setRevolutionCounter(self.turnsBetweenRevs)
				pCity.setReinforcementCounter(0)
				RevData.updateCityVal(pCity, 'RevolutionTurn', GAME.getGameTurn())
				if not bIsBarbRev:
					RevData.updateCityVal(pCity, 'RevolutionCiv', pRevPlayer.getCivilizationType())

			else:
				if self.LOG_DEBUG:
					CvUtil.pyPrint("[REV] Revolt: Owner keeps control of %s (%d,%d), revs spawning at %d,%d"%(pCity.getName(),ix,iy,revSpawnLoc[0],revSpawnLoc[1]))

				# City in disorder
				iTurns = 1 + revIdx * 10 * self.revReadyDivisor / (7 * self.revReadyDividend * self.revInstigatorThreshold)
				if localRevIdx > 4:
					iTurns = int(iTurns*1.5)
					iTurns = min([iTurns,self.turnsBetweenRevs - 1])
				else:
					iTurns = int(min([iTurns,1 + self.turnsBetweenRevs/2]))

				pCity.setOccupationTimer( max([iTurns,1]) )
				if self.LOG_DEBUG:
					CvUtil.pyPrint("[REV] Revolt: City occupation timer set to %d"%(pCity.getOccupationTimer()))

				if pCity.getRevRequestAngerTimer() < 3*self.turnsBetweenRevs:
					pCity.changeRevRequestAngerTimer( min([2*self.turnsBetweenRevs, 3*self.turnsBetweenRevs - pCity.getRevRequestAngerTimer()]) )

				if iNumUnits > 0:
					# Move any units that may be on the spawn plot
					enemyUnits = RevUtils.getEnemyUnits( revSpawnLoc[0], revSpawnLoc[1], pRevPlayer.getID() )
					if enemyUnits:
						moveToPlots = RevUtils.getSpawnablePlots( revSpawnLoc[0], revSpawnLoc[1], GC.getPlayer(enemyUnits[0].getOwner()), bLand = True, bIncludePlot = False, bIncludeCities = True, bIncludeForts = True, bSameArea = True, iRange = 1, iSpawnPlotOwner = enemyUnits[0].plot().getOwner(), bCheckForEnemy = True, bAtWarPlots = False, bOpenBordersPlots = False )
						if not moveToPlots:
							moveToPlots = RevUtils.getSpawnablePlots( revSpawnLoc[0], revSpawnLoc[1], GC.getPlayer(enemyUnits[0].getOwner()), bLand = True, bIncludePlot = False, bIncludeCities = True, bIncludeForts = True, bSameArea = True, iRange = 2, iSpawnPlotOwner = enemyUnits[0].plot().getOwner(), bCheckForEnemy = True, bAtWarPlots = False )
						if not moveToPlots:
							moveToPlots = RevUtils.getSpawnablePlots( revSpawnLoc[0], revSpawnLoc[1], GC.getPlayer(enemyUnits[0].getOwner()), bLand = True, bIncludePlot = False, bIncludeCities = True, bSameArea = False, iRange = 4, iSpawnPlotOwner = -1, bCheckForEnemy = True, bAtWarPlots = False )

						if not moveToPlots: # Highly unlikely
							print '[WARN] Enemy units outside city are going to die cause they have no where to go ...'
						else:
							moveToLoc = moveToPlots[GAME.getSorenRandNum(len(moveToPlots),'Revolution: Pick move to plot')]
							if self.LOG_DEBUG:
								CvUtil.pyPrint("[REV] Revolt: Enemy units in plot moving to %d,%d"%(moveToLoc[0], moveToLoc[1]))
							RevUtils.moveEnemyUnits(ix, iy, pRevPlayer.getID(), moveToLoc[0], moveToLoc[1], iInjureMax=0, bDestroyNonLand=False, bLeaveSiege=False)

				# Wound player's units?
				unitList = RevUtils.getEnemyUnits( ix, iy, pRevPlayer.getID(), bOnlyMilitary = True )
				for pUnit in unitList:
					if pUnit.canFight():
						iPreDamage = pUnit.getDamage()
						if revIdx > self.revInstigatorThreshold:
							iDamage = iPreDamage/5 + 20 + GAME.getSorenRandNum(35,'Revolution: Wound units')
						else:
							iDamage = iPreDamage/5 + 15 + GAME.getSorenRandNum(25,'Revolution: Wound units')
						iDamage = min([iDamage,90])
						iDamage = max([iDamage,iPreDamage])
						pUnit.setDamage( iDamage, pRevPlayer.getID() )

				# Defections?
				if self.LOG_DEBUG:
					CvUtil.pyPrint("[REV] Revolt: Spawning %d units for city of size %d" %(iNumUnits, pCity.getPopulation()))

				# Spawn rev units outside city
				newUnitList = []
				for i in xrange(iNumUnits):
					newUnitID = spawnableUnits[GAME.getSorenRandNum( len(spawnableUnits), 'Revolution: pick unit' )]
					newUnit = pRevPlayer.initUnit( newUnitID, revSpawnLoc[0], revSpawnLoc[1], UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH )
					newUnitList.append(newUnit)

				for [iNum,newUnit] in enumerate(newUnitList):
					if newUnit.canFight():
						# Injure units to simulate the lack of training in rebel troops
						iDamage = 10 + GAME.getSorenRandNum(30, 'Rev - Injure unit')
						newUnit.setDamage(iDamage, pPlayer.getID())

						# Check AI settings
						if newUnit.isBarbarian():
							if pRevPlayer.AI_unitValue(newUnit.getUnitType(),UnitAITypes.UNITAI_ATTACK_CITY_LEMMING,newUnit.area()) > 0:
								newUnit.setUnitAIType(UnitAITypes.UNITAI_ATTACK_CITY_LEMMING)
							else:
								newUnit.setUnitAIType(UnitAITypes.UNITAI_ATTACK)
						else:
							if iNum < 2 and iNumUnits > 2 and pRevPlayer.AI_unitValue(newUnit.getUnitType(),UnitAITypes.UNITAI_ATTACK_CITY,newUnit.area()) > 0:
								newUnit.setUnitAIType( UnitAITypes.UNITAI_ATTACK_CITY )
							elif iNumUnits == 1 and GAME.getSorenRandNum(2,'Rev - Pillage') == 0 and pRevPlayer.AI_unitValue(newUnit.getUnitType(),UnitAITypes.UNITAI_PILLAGE,newUnit.area()) > 0:
								newUnit.setUnitAIType( UnitAITypes.UNITAI_PILLAGE )
							else:
								iniAI = newUnit.getUnitAIType()
								if not (iniAI == UnitAITypes.UNITAI_COUNTER or iniAI == UnitAITypes.UNITAI_ATTACK_CITY):
									newUnit.setUnitAIType(UnitAITypes.UNITAI_ATTACK)
								if self.LOG_DEBUG:
									CvUtil.pyPrint("[REV] Revolt: %s starting with AI type: %d (ini %d)"%(newUnit.getName(),newUnit.getUnitAIType(),iniAI))

						if not bIsBarbRev and pRevPlayer.isRebel() and revIdx > self.revInstigatorThreshold:
							# Give a free promotion to help rebel cause
							newUnit.setPromotionReady(True)

				if pCity.getPopulation() > 4 and len(newUnitList) >= 4:
					deltaPop = int((len(newUnitList)-1)/2.5)
					if deltaPop >= pCity.getPopulation():
						deltaPop = pCity.getPopulation() - 1
					pCity.changePopulation(-deltaPop)

				# Extra stuff for instigator city
				if cityIdx == 0 and len(cityList) > 1 and iNumUnits > 0 and not bIsBarbRev:
					if 3*len(cityList) + 5*pCity.getNumRevolts(pCity.getOwner()) < GAME.getSorenRandNum(100,'Rev'):
						pRevPlayer.initUnit(iGeneral, revSpawnLoc[0], revSpawnLoc[1], UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)

					if iScout != -1:
						if pRevPlayer.canTrain(iScout, False, False):
							pRevPlayer.initUnit(iScout, revSpawnLoc[0], revSpawnLoc[1], UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)

					if not bIsBarbRev and not bIsJoinWar and iSettler != -1 and pCity.area().getNumTiles() / pCity.area().getNumCities() > 25:
						# Settler if there is sufficient empty land available?
						pRevPlayer.initUnit(iSettler, revSpawnLoc[0], revSpawnLoc[1], UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)

				if iSpy != -1 and pRevPlayer.canTrain(iSpy,False,False) and pRevPlayer.AI_getNumAIUnits( UnitAITypes.UNITAI_SPY ) < 3:
					if pCity.getNumRevolts(pCity.getOwner()) > 1 and revIdx > self.alwaysViolentThreshold or pCity.getNumRevolts(pCity.getOwner()) > 2 and revIdx > self.revInstigatorThreshold:
						pSpy = pRevPlayer.initUnit( iSpy, ix, iy, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH )
						pSpy.setFortifyTurns(GC.getDefineINT("MAX_FORTIFY_TURNS"))

				# Reveal surrounding countryside
				if not bGaveMap and newUnitList:
					pRevPlayer.receiveGoody(GC.getMap().plot(revSpawnLoc[0], revSpawnLoc[1]), iGoodyMap, newUnitList[0])
					pRevPlayer.receiveGoody(GC.getMap().plot(revSpawnLoc[0], revSpawnLoc[1]), iGoodyMap, newUnitList[0])

				if bIsBarbRev:
					# Only for determining if revolt has been put down
					pCity.setReinforcementCounter(5)
				else:
					iReinforceTurns = self.baseReinforcementTurns - revIdx/self.revInstigatorThreshold - min([localRevIdx,12])/4
					if pCity.getPopulation() < 7:
						iReinforceTurns += 7 - pCity.getPopulation()
						if pCity.getPopulation() < 5:
							iReinforceTurns = max([iReinforceTurns,4 - (pCity.getPopulation()/2)])

					minReinfTurns = self.minReinforcementTurns
					if pPlayer.getCurrentEra() < 4:
						minReinfTurns += 2 - pPlayer.getCurrentEra()/2
					iReinforceTurns = max([iReinforceTurns,self.minReinforcementTurns])
					iReinforceTurns = min([iReinforceTurns,10])
					pCity.setReinforcementCounter( iReinforceTurns + 1 )

				pCity.changeRevolutionIndex(int(max([iRevIdxInc + 15.0 * min([localRevIdx,15.0]), 100.0])))

				pCity.setRevolutionCounter(self.turnsBetweenRevs)
				RevData.updateCityVal(pCity, 'RevolutionTurn', GAME.getGameTurn() )
				if not bIsBarbRev:
					RevData.updateCityVal(pCity, 'RevolutionCiv', pRevPlayer.getCivilizationType() )


		if not pPlayer.getNumCities():
			pPlayer.setFoundedFirstCity(False)

		# Release camera from cities
		#CyCamera().ReleaseLockedCamera()

		return

##--- Network syncing functions------------------------------------------

	def onModNetMessage(self, argsList):
		protocol, data1, data2, data3, data4 = argsList
		if protocol == self.netRevolutionPopupProtocol:
			self.revolutionNetworkPopupHandler(data1, data2, data3)
		elif protocol == self.netControlLostPopupProtocol:
			self.controlLostNetworkHandler(data1, data2, data3)

	def revolutionPopupHandler(self, iPlayerID, netUserData, popupReturn):
		if self.isLocalHumanPlayer(iPlayerID):
			buttons = netUserData[0]
			iPlayer = netUserData[1]

			if not self.isLocalHumanPlayer(iPlayer):
				return

			iRevoltIdx = netUserData[2]
			buttonLabel = buttons[popupReturn.getButtonClicked()]
			iButton = -1
			if buttonLabel == 'accept'			: iButton = 0
			elif buttonLabel == 'reject'		: iButton = 1
			elif buttonLabel == 'buyoff'		: iButton = 2
			elif buttonLabel == 'vassal'		: iButton = 3
			elif buttonLabel == 'control'		: iButton = 4
			elif buttonLabel == 'buyelection'	: iButton = 5
			elif buttonLabel == 'war'			: iButton = 6
			elif buttonLabel == 'defect'		: iButton = 7

			if iButton >= 0:
				CyMessageControl().sendModNetMessage(self.netRevolutionPopupProtocol, iPlayer, iButton, iRevoltIdx, 0)
