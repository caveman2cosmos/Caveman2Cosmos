# DynamicCivNames
#
# by jdog5000
# Version 1.0
#
# French compatibility from calvitix
#

from CvPythonExtensions import *
import CvUtil
import Popup as PyPopup
# --------- Revolution mod -------------
import RevDefs
import SdToolKit as SDTK
import RevUtils
import LeaderCivNames
#import BugCore

GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

#RevOpt = BugCore.game.Revolution

g_DynamicCivNames = None

class DynamicCivNames:

	def __init__(self, customEM):
		global g_DynamicCivNames
		g_DynamicCivNames = self

		self.customEM = customEM

		#self.bTeamNaming = True #RevOpt.isTeamNaming()
		#self.bLeaveHumanName = False #RevOpt.isLeaveHumanPlayerName()

		self.customEM.addEventHandler("BeginPlayerTurn", self.onBeginPlayerTurn)
		self.customEM.addEventHandler("setPlayerAlive", self.onSetPlayerAlive)
		self.customEM.addEventHandler("kbdEvent", self.onKbdEvent)
		self.customEM.addEventHandler("cityAcquired", self.onCityAcquired)
		self.customEM.addEventHandler('cityBuilt', self.onCityBuilt)
		self.customEM.addEventHandler("vassalState", self.onVassalState)
		self.customEM.addEventHandler("addTeam", self.onAddTeam)

		LeaderCivNames.setup()

		if not GAME.isFinalInitialized or GAME.getGameTurn() == GAME.getStartTurn():
			for idx in xrange(GC.getMAX_PC_PLAYERS()):
				self.onSetPlayerAlive([idx, GC.getPlayer(idx).isAlive()])

		self.femaleLeaders = (
			GC.getInfoTypeForString("LEADER_BOUDICA"),
			GC.getInfoTypeForString("LEADER_ELIZABETH"),
			GC.getInfoTypeForString("LEADER_HATSHEPSUT"),
			GC.getInfoTypeForString("LEADER_VICTORIA"),
			GC.getInfoTypeForString("LEADER_ATOTOZTLI"),
			GC.getInfoTypeForString("LEADER_CLEOPATRA"),
			GC.getInfoTypeForString("LEADER_DIDO"),
			GC.getInfoTypeForString("LEADER_JOANOFARC"),
			GC.getInfoTypeForString("LEADER_NEFERTITI"),
			GC.getInfoTypeForString("LEADER_THEODORA"),
			GC.getInfoTypeForString("LEADER_WU")
		)

	def removeEventHandlers(self):
		self.customEM.removeEventHandler("BeginPlayerTurn", self.onBeginPlayerTurn)
		self.customEM.removeEventHandler("setPlayerAlive", self.onSetPlayerAlive)
		self.customEM.removeEventHandler("kbdEvent", self.onKbdEvent)
		self.customEM.removeEventHandler("cityAcquired", self.onCityAcquired)
		self.customEM.removeEventHandler('cityBuilt', self.onCityBuilt)
		self.customEM.removeEventHandler("vassalState", self.onVassalState)

	def blankHandler(self, playerID, netUserData, popupReturn):
		""" Dummy handler to take the second event for popup """
		return

	def onKbdEvent(self, argsList):
		'keypress handler'
		#eventType, key, mx, my, px, py = argsList
		eventType = argsList[0]; key = argsList[1]

		if eventType == RevDefs.EventKeyDown:
			theKey = int(key)

			# For debug or trial only
			if theKey == int(InputTypes.KB_U) and self.customEM.bShift and self.customEM.bCtrl:
				self.showNewNames()


	def onBeginPlayerTurn(self, argsList):
		#iGameTurn = argsList[0]
		iPlayer = argsList[1]

		# Stuff at end of previous players turn
		iPrevPlayer = iPlayer - 1
		while iPrevPlayer >= 0 and not GC.getPlayer(iPrevPlayer).isAlive():
			iPrevPlayer -= 1

		if iPrevPlayer < 0:
			iPrevPlayer = GC.getBARBARIAN_PLAYER()

		if iPrevPlayer >= 0 and iPrevPlayer < GC.getMAX_PC_PLAYERS():
			iPlayer = iPrevPlayer
			pPlayer = GC.getPlayer(iPlayer)

			if pPlayer.isAnarchy():
				self.setNewNameByCivics(iPlayer)
				return

			if pPlayer.isAlive() and SDTK.sdObjectExists("Revolution", pPlayer):
				prevCivics = SDTK.sdObjectGetVal("Revolution", pPlayer, 'CivicList')
				if not prevCivics == None:
					for i in xrange(GC.getNumCivicOptionInfos()):
						if not prevCivics[i] == pPlayer.getCivics(i):
							self.setNewNameByCivics(iPlayer)
							return

				revTurn = SDTK.sdObjectGetVal( "Revolution", pPlayer, 'RevolutionTurn' )
				if revTurn is not None and GAME.getGameTurn() - revTurn == 30 and pPlayer.getNumCities() > 0:
					# "Graduate" from rebel name
					self.setNewNameByCivics(iPlayer)
					return

			if pPlayer.isAlive() and SDTK.sdObjectExists("BarbarianCiv", pPlayer):
				barbTurn = SDTK.sdObjectGetVal("BarbarianCiv", pPlayer, 'SpawnTurn')
				if barbTurn is not None and GAME.getGameTurn() - barbTurn == 30:
					# "Graduate" from barb civ name
					self.setNewNameByCivics(iPlayer)
					return

			if (pPlayer.isAlive()
			and not SDTK.sdObjectExists("BarbarianCiv", pPlayer)
			and 'Tribe' in pPlayer.getCivilizationDescription(0)
			and (pPlayer.getCurrentEra() > 0 or pPlayer.getTotalPopulation() >= 3)
			):
				# Graduate from game start name
				self.setNewNameByCivics(iPlayer)
				return

	def onCityAcquired(self, argsList):
		city = argsList[2]
		owner = GC.getPlayer(city.getOwner())

		if owner.isAlive() and not owner.isNPC() and owner.getNumCities() < 5 and owner.getNumMilitaryUnits() > 0:
			self.setNewNameByCivics(owner.getID())

	def onCityBuilt(self, argsList):
		owner = GC.getPlayer(argsList[0].getOwner())

		if owner.isAlive() and not owner.isNPC() and owner.getNumCities() < 5 and owner.getNumMilitaryUnits() > 0:
			self.setNewNameByCivics(owner.getID())

	def onVassalState(self, argsList):
		iVassal = argsList[1]
		for iPlayer in xrange(GC.getMAX_PC_PLAYERS()):
			if GC.getPlayer(iPlayer).getTeam() == iVassal:
				self.setNewNameByCivics(iPlayer)


	def setNewNameByCivics(self, iPlayer):
		'''
		if self.bLeaveHumanName and (GC.getPlayer(iPlayer).isHuman() or GAME.getActivePlayer() == iPlayer):
			CvUtil.pyPrint("	Name - Leaving name for human player")
			return
		'''
		[newCivDesc, newCivShort, newCivAdj] = self.newNameByCivics(iPlayer)

		if not newCivDesc == GC.getPlayer(iPlayer).getCivilizationDescription(0):
			szMessage = TRNSLTR.getText("TXT_KEY_MOD_DCN_NEWCIV_NAME_DESC", (newCivDesc,))
			CyInterface().addMessage(iPlayer, False, GC.getEVENT_MESSAGE_TIME(), szMessage, None, InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), -1, -1, False, False)

		GC.getPlayer(iPlayer).setCivName(newCivDesc, newCivShort, newCivAdj)


	def onSetPlayerAlive(self, argsList):
		iPlayerID = argsList[0]
		bNewValue = argsList[1]
		if bNewValue and iPlayerID < GC.getMAX_PC_PLAYERS():
			pPlayer = GC.getPlayer(iPlayerID)

			'''
			if self.bLeaveHumanName and (pPlayer.isHuman() or GAME.getActivePlayer() == iPlayerID):
				CvUtil.pyPrint("	Name - Leaving name for human player")
				return
			'''
			[newCivDesc, newCivShort, newCivAdj] = self.nameForNewPlayer( iPlayerID )

			# Pass to pPlayer seems to require a conversion to 'ascii'
			pPlayer.setCivName(newCivDesc, newCivShort, newCivAdj)


	def onAddTeam(self, argsList):
		eTeam1 = argsList[0]
		eTeam2 = argsList[1]
		for i in xrange(GC.getMAX_PC_PLAYERS()):
			pPlayer = GC.getPlayer(i)
			if pPlayer.isAlive() and pPlayer.getTeam() in (eTeam1, eTeam2):
				self.setNewNameByCivics(i)


	def showNewNames(self):
		bodStr = 'New names for all civs:\n\n'

		for i in xrange(GC.getMAX_PC_PLAYERS()):
			pPlayer = GC.getPlayer(i)
			if pPlayer.isAlive():
				curDesc = pPlayer.getCivilizationDescription(0)
				[newName,newShort,newAdj] = self.newNameByCivics(i)
				bodStr += curDesc + '\t-> ' + newName + '\n'

		popup = PyPopup.PyPopup()
		popup.setBodyString(bodStr)
		popup.launch()


	def nameForNewPlayer(self, iPlayer):
		# Assigns a new name to a recently created player from either
		# BarbarianCiv or Revolution components
		pPlayer = GC.getPlayer(iPlayer)

		if (not GAME.isOption(GameOptionTypes.GAMEOPTION_LEAD_ANY_CIV)
		and pPlayer.getLeaderType() in LeaderCivNames.LeaderCivNames.keys()
		):
			curDesc, curShort, curAdj = LeaderCivNames.LeaderCivNames[pPlayer.getLeaderType()]
		else:
			curShort = pPlayer.getCivilizationShortDescription(0)
			curDesc = pPlayer.getCivilizationDescription(0)
			curAdj = pPlayer.getCivilizationAdjective(0)

		if not pPlayer.isAlive():
			return [TRNSLTR.getText("TXT_KEY_MOD_DCN_REFUGEES", ())%(curAdj), curShort, curAdj]

		currentEra = 0
		for i in xrange(GC.getMAX_PC_PLAYERS()):
			if GC.getPlayer(i).getCurrentEra() > currentEra:
				currentEra = GC.getPlayer(i).getCurrentEra()

		if pPlayer.isRebel():
			# To name rebels in Revolution mod

			sLiberation = TRNSLTR.getText("TXT_KEY_MOD_DCN_LIBERATION_FRONT", ()).replace('%s','').strip()
			sGuerillas = TRNSLTR.getText("TXT_KEY_MOD_DCN_GUERILLAS", ()).replace('%s','').strip()
			sRebels = TRNSLTR.getText("TXT_KEY_MOD_DCN_REBELS", ()).replace('%s','').strip()

			if sLiberation in curDesc or sGuerillas in curDesc or sRebels in curDesc:
				newName = curDesc

			elif currentEra > 5 and 30 > GAME.getSorenRandNum(100,'Rev: Naming'):
				newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_LIBERATION_FRONT", ())%(curAdj)

			elif currentEra > 4 and 30 > GAME.getSorenRandNum(100,'Rev: Naming'):
				newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_GUERILLAS", ())%(curAdj)
			else:
				cityString = SDTK.sdObjectGetVal("Revolution", pPlayer, 'CapitalName')

				if cityString is not None and len(cityString) < 10:
					try:
						if cityString in curAdj or cityString in curShort:
							newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_THE_REBELS_OF", ())%(CvUtil.convertToStr(cityString))
						else:
							newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_REBELS_OF", ())%(curAdj,CvUtil.convertToStr(cityString))
					except:
						newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_REBELS", ())%(curAdj)
				else:
					newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_REBELS", ())%(curAdj)

			return [newName, curShort, curAdj]


		if SDTK.sdObjectExists("BarbarianCiv", pPlayer):
			barbTurn = SDTK.sdObjectGetVal("BarbarianCiv", pPlayer, 'SpawnTurn')
		else: barbTurn = None

		if barbTurn is not None and GAME.getGameTurn() - barbTurn < 20:
			# To name BarbarianCiv created civs
			numCities = SDTK.sdObjectGetVal( "BarbarianCiv", pPlayer, 'NumCities' )
			cityString = SDTK.sdObjectGetVal( "BarbarianCiv", pPlayer, 'CapitalName' )

			if pPlayer.isMinorCiv():
				if currentEra > 2:
					newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_NATION", ())%(curAdj)

				elif currentEra == 2:
					newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_CITY_STATE", ())%(curAdj)

				elif 70 - 40*currentEra > GAME.getSorenRandNum(100,"Naming"):
					newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_TRIBE", ())%(curAdj)
				else:
					newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_CITY_STATE", ())%(curAdj)

			elif currentEra < 4:
				# Early era barbs
				if SDTK.sdObjectGetVal('BarbarianCiv', pPlayer, 'BarbStyle') != 'Military':
					if numCities == 1:
						newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_CITY_STATE", ())%(curAdj)
					else:
						newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_EMPIRE", ())%(curAdj)

					if numCities < 3 and cityString is not None and len(cityString) < 10:
						newName += TRNSLTR.getText("TXT_KEY_MOD_DCN_OF_CITY", ())%(cityString)

				elif pPlayer.getNumMilitaryUnits() > 7*numCities:
					newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_HORDE", ())%(curAdj)

				elif cityString is None or len(cityString) > 9:
					newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_WARRIOR_STATE", ())%(curAdj)

				elif cityString in curAdj or cityString in curShort:
					newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_THE_WARRIORS_OF", ())%(cityString)
				else:
					newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_WARRIORS_OF", ())%(curAdj,cityString)

			else:
				newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_EMPIRE", ())%(curAdj)
				if numCities < 3 and cityString is not None and len(cityString) < 10:
					newName += TRNSLTR.getText("TXT_KEY_MOD_DCN_OF_CITY", ())%(cityString)

			return [newName, curShort, curAdj]


		if GAME.getGameTurn() == GAME.getStartTurn() and GAME.getCurrentEra() < 1:
			# Name civs at beginning of game
			return [TRNSLTR.getText("TXT_KEY_MOD_DCN_TRIBE", ())%(curAdj), curShort, curAdj]

		return self.newNameByCivics(iPlayer)


	def newNameByCivics(self, iPlayer, bVerbose = True, bForceUpdate = False):
		# Assigns a new name to a player based on their civics choices

		pPlayer = GC.getPlayer(iPlayer)
		capital = pPlayer.getCapitalCity()
		pTeam = GC.getTeam(pPlayer.getTeam())

		cityString = None
		if capital:
			try :
				# Silly game to force ascii encoding now
				cityString =	pPlayer.getCivilizationDescription(0)
				cityString += "&" + CvUtil.convertToStr(capital.getName())
				cityString =	cityString.split('&',1)[-1]
			except :
				pass

		curDesc	= pPlayer.getCivilizationDescription(0)
		curShort = pPlayer.getCivilizationShortDescription(0)
		curAdj	 = pPlayer.getCivilizationAdjective(0)

		origDesc = ""
		if pPlayer.getCivilizationType() >= 0:
			civInfo = GC.getCivilizationInfo(pPlayer.getCivilizationType())
			origDesc = civInfo.getDescription()

		iLanguage = GC.getGame().getCurrentLanguage()
		bFrench = iLanguage == 1 #0 - English, 1 - French, 2 - German, 3 - Italian, 4 - Spanish

		eGovCivic = pPlayer.getCivics(GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT"))
		ePowerCivic = pPlayer.getCivics(GC.getInfoTypeForString("CIVICOPTION_POWER"))
		bNoRealElections = (GC.getInfoTypeForString("CIVIC_MONARCHY") == eGovCivic or GC.getInfoTypeForString("CIVIC_MONARCHY") == eGovCivic or GC.getInfoTypeForString("CIVIC_DESPOTISM") == eGovCivic or GC.getInfoTypeForString("CIVIC_TOTALITARIANISM") == eGovCivic)

		bFederal = (GC.getInfoTypeForString("CIVIC_FEDERALISM") == eGovCivic and (ePowerCivic == GC.getInfoTypeForString("CIVIC_LEGISLATURE")))
		bConfederation = (not bFederal and (GC.getInfoTypeForString("CIVIC_FEDERALISM") == eGovCivic))

		bPacifist = (pPlayer.getCivics(GC.getInfoTypeForString("CIVICOPTION_MILITARY")) == GC.getInfoTypeForString("CIVIC_PACIFISM"))

		if( not GAME.isOption(GameOptionTypes.GAMEOPTION_LEAD_ANY_CIV) ):
			if( pPlayer.getLeaderType() in LeaderCivNames.LeaderCivNames.keys() ):
				[curDesc, curShort, curAdj] = LeaderCivNames.LeaderCivNames[pPlayer.getLeaderType()]

		newName = curDesc
		if SDTK.sdObjectExists("Revolution", pPlayer):
			SDTK.sdObjectGetVal("Revolution", pPlayer, 'RevolutionTurn')

		if( SDTK.sdObjectExists( "BarbarianCiv", pPlayer ) ):
			barbTurn = SDTK.sdObjectGetVal( "BarbarianCiv", pPlayer, 'SpawnTurn' )
		else:
			barbTurn = None

		if( not pPlayer.isAlive() ):
			newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_REFUGEES", ())%(curAdj)
			return [newName, curShort, curAdj]

		if pPlayer.isRebel():
			# Maintain name of rebels from Revolution Mod
			if bForceUpdate:
				return self.nameForNewPlayer(iPlayer)
			else:
				return [curDesc, curShort, curAdj]
		elif( pPlayer.isMinorCiv() and not barbTurn == None ):
			# Maintain minor civ name
			if bForceUpdate:
				return self.nameForNewPlayer(iPlayer)
			else:
				return [curDesc, curShort, curAdj]
		elif( not barbTurn == None and GAME.getGameTurn() - barbTurn < 20 and pPlayer.getNumCities() < 4 ):
			# Maintain name of BarbarianCiv created player
			if bForceUpdate:
				return self.nameForNewPlayer(iPlayer)
			else:
				return [curDesc, curShort, curAdj]


		# Special options for teams and permanent alliances
		'''
		if self.bTeamNaming and pTeam.getNumMembers() > 1: # and pTeam.getPermanentAllianceTradingCount() > 0:
		'''
		if pTeam.getNumMembers() > 1: # and pTeam.getPermanentAllianceTradingCount() > 0:
			if pTeam.getNumMembers() == 2:
				iLeader = pTeam.getLeaderID()
				newName = GC.getPlayer(iLeader).getCivilizationAdjective(0) + "-"
				for idx in xrange(GC.getMAX_PC_PLAYERS()):
					if( not idx == iLeader and GC.getPlayer(idx).getTeam() == pTeam.getID() ):
						newName += GC.getPlayer(idx).getCivilizationAdjective(0)
						break
				newName += TRNSLTR.getText("TXT_KEY_MOD_DCN_ALLIANCE", ())
				return [newName,curShort,curAdj]
			else:
				iLeader = pTeam.getLeaderID()
				newName = GC.getPlayer(iLeader).getCivilizationAdjective(0)[0:4]
				for idx in xrange(GC.getMAX_PC_PLAYERS()):
					if not idx == iLeader and GC.getPlayer(idx).getTeam() == pTeam.getID():
						newName += GC.getPlayer(idx).getCivilizationAdjective(0)[0:3]
				newName += TRNSLTR.getText("TXT_KEY_MOD_DCN_ALLIANCE", ())
				return [newName,curShort,curAdj]

		sSocRep = TRNSLTR.getText("TXT_KEY_MOD_DCN_SOC_REP", ()).replace('%s','').strip()
		sPeoplesRep = TRNSLTR.getText("TXT_KEY_MOD_DCN_PEOPLES_REP", ()).replace('%s','').strip()

		# Anarchy Naming
		if pPlayer.isAnarchy and pPlayer.getAnarchyTurns() > 1:
			if 75 > GAME.getSorenRandNum(100,'Rev: Naming'):
				newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_PROVISIONAL_GOV", ())%(curAdj)
			else:
				newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_PROVISIONAL_AUTH", ())%(curAdj)
			return [newName, curShort, curAdj]

		if (not pPlayer.isAnarchy or pPlayer.getAnarchyTurns() < 2) and "Provisional" in curDesc:
			if GC.getInfoTypeForString("CIVIC_MONARCHY") == eGovCivic:
				newName = curAdj + ' ' + TRNSLTR.getText("TXT_KEY_MOD_DCN_KINGDOM", ())
			elif GC.getInfoTypeForString("CIVIC_REPUBLIC") == eGovCivic:
				newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_REPUBLIC", ())%(curAdj)
			else:
				newName = curAdj + ' Nation'
			return [newName, curShort, curAdj]

		# Main naming conditions
		if RevUtils.isCommunism(iPlayer):
			if RevUtils.isCanDoElections(iPlayer) and not bNoRealElections:
				if not bForceUpdate and (sSocRep in curDesc or sPeoplesRep in curDesc):
					newName = curDesc
				elif 50 > GAME.getSorenRandNum(100,'Rev: Naming'):
					newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_SOC_REP", ())%(curShort)
				else:
					newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_PEOPLES_REP", ())%(curShort)
			elif RevUtils.getDemocracyLevel(iPlayer)[0] == -8:
				if TRNSLTR.getText("TXT_KEY_MOD_DCN_RUSSIAN_MATCH", ()) in curAdj:
					curAdj = TRNSLTR.getText("TXT_KEY_MOD_DCN_SOVIET", ())
				newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_UNION", ())%(curAdj)
			else:
				newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_PEOPLES_REP", ())%(curShort)

		elif RevUtils.isCanDoElections(iPlayer) and not bNoRealElections:
			sRepOf = TRNSLTR.getText("TXT_KEY_MOD_DCN_REPUBLIC_OF", ()).replace('%s','').strip()
			sRepublic = TRNSLTR.getText("TXT_KEY_MOD_DCN_REPUBLIC", ())

			if pPlayer.getNumCities() == 1:
				if not bForceUpdate and (curDesc.startswith(TRNSLTR.getText("TXT_KEY_MOD_DCN_FREE", ())) or ((sRepOf in curDesc or sRepublic in curDesc) and cityString in curDesc)):
					newName = curDesc

				elif 40 > GAME.getSorenRandNum(100,'Rev: Naming'):
					newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_FREE_STATE", ())%(curAdj)

				elif cityString is None or not cityString or len(cityString) > 9:
					newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_FREE_REPUBLIC", ())%(curAdj)

				elif cityString in curAdj or cityString in curShort:
					newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_THE_REPUBLIC_OF_CITY", ())%(cityString)
				else:
					newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_REPUBLIC_OF_CITY", ())%(curAdj,cityString)
			else:
				if (not bFederal and not bConfederation and not bForceUpdate
				and sRepublic in curDesc and sPeoplesRep not in curDesc and sSocRep not in curDesc
				and curDesc.startswith(TRNSLTR.getText("TXT_KEY_MOD_DCN_FREE", ()))
				):
					if len(curDesc) < 17 and 20 > GAME.getSorenRandNum(100,'Rev: Naming') and not TRNSLTR.getText("TXT_KEY_MOD_DCN_NEW", ()) in curDesc:
						newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_NEW", ()) + curDesc
					else:
						newName = curDesc
				elif bFederal:
					if (pPlayer.getCivilizationType() == GC.getInfoTypeForString("CIVILIZATION_UNITED_STATES")):
						newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_UNITED_STATES", ())%(curShort)
					elif 50 > GAME.getSorenRandNum(100,'Rev: Naming'):
						newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_FEDERATED_STATES", ())%(curShort)
					else:
						newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_FEDERATION", ())%(curAdj)
				elif bConfederation:
					if (50 > GAME.getSorenRandNum(100,'Rev: Naming')):
						newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_CONFEDERATION", ())%(curAdj)
					else:
						newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_CONFEDERATION_STATES", ())%(curShort)
				elif 50 > GAME.getSorenRandNum(100,'Rev: Naming'):
					newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_REPUBLIC", ())%(curAdj)
				elif 33 > GAME.getSorenRandNum(100,'Rev: Naming'):
					newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_THE_COMMONWEALTH_OF", ())%(curShort)
				else:
					newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_THE_REPUBLIC_OF", ())%(curShort)

			if (RevUtils.isFreeSpeech(iPlayer) and RevUtils.getLaborFreedom(iPlayer)[0] > 9
			and len(newName) < 16
			and TRNSLTR.getText("TXT_KEY_MOD_DCN_FREE", ()) not in newName
			and TRNSLTR.getText("TXT_KEY_MOD_DCN_NEW", ()) not in newName
			):
				newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_FREE", ()) + ' ' + newName

		elif RevUtils.getDemocracyLevel(iPlayer)[0] == -8:

			if TRNSLTR.getText("TXT_KEY_MOD_DCN_GERMAN_MATCH", ()) in curAdj:
				empString = TRNSLTR.getText("TXT_KEY_MOD_DCN_REICH", ())
			else:
				empString = TRNSLTR.getText("TXT_KEY_MOD_DCN_PLAIN_EMPIRE", ())

			if not bForceUpdate and empString in curDesc:
				newName = curDesc
			elif 70 > GAME.getSorenRandNum(100,'Rev: Naming') and not TRNSLTR.getText("TXT_KEY_MOD_DCN_REICH", ()) in empString:
				newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_THE_BLANK_OF", ())%(empString,curShort)
			elif bFrench:
				newName = empString + ' ' + curAdj
			else:
				newName = curAdj + ' ' + empString
		else:
			sGreat = TRNSLTR.getText("TXT_KEY_MOD_DCN_GREAT_KINGDOM", ()).replace('%s','').strip()

			if pPlayer.getNumCities() < 3:
				sKingdom = TRNSLTR.getText("TXT_KEY_MOD_DCN_PRINCIPALITY", ())

			elif pPlayer.getLeaderType() in self.femaleLeaders:
				sKingdom = TRNSLTR.getText("TXT_KEY_MOD_DCN_QUEENDOM", ())
			else:
				sKingdom = TRNSLTR.getText("TXT_KEY_MOD_DCN_KINGDOM", ())

			playerEra = pPlayer.getCurrentEra()

			if RevUtils.getDemocracyLevel(iPlayer)[0] == -6:

				if pTeam.isAVassal():
					sKingdom = TRNSLTR.getText("TXT_KEY_MOD_DCN_DUCHY", ())
				elif TRNSLTR.getText("TXT_KEY_MOD_DCN_PERSIAN_MATCH", ()) in curAdj or TRNSLTR.getText("TXT_KEY_MOD_DCN_OTTOMAN_MATCH", ()) in curAdj or TRNSLTR.getText("TXT_KEY_MOD_DCN_SUMERIAN_MATCH", ()) in curAdj:
					sKingdom = TRNSLTR.getText("TXT_KEY_MOD_DCN_SULTANATE", ())
				elif TRNSLTR.getText("TXT_KEY_MOD_DCN_ARABIAN_MATCH", ()) in curAdj:
					sKingdom = TRNSLTR.getText("TXT_KEY_MOD_DCN_CALIPHATE", ())

				if pPlayer.getNumCities() < 4:
					if not cityString == None and len(cityString) < 10 and len(cityString) > 0:
						if cityString in curAdj or cityString in curShort:
							newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_THE_BLANK_OF_CITY", ())%(sKingdom,cityString)
						elif bFrench:
							newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_BLANK_OF_CITY", ())%(sKingdom,curAdj,cityString)
						else:
							newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_BLANK_OF_CITY", ())%(curAdj,sKingdom,cityString)
					elif bFrench:
						newName = sKingdom + ' ' + curAdj
					else:
						newName = curAdj + ' ' + sKingdom

				elif GAME.getPlayerRank(iPlayer) < GAME.countCivPlayersAlive()/7 and not pTeam.isAVassal() and (sGreat in curDesc or 40 > GAME.getSorenRandNum(100,'Rev: Naming')):
					if bFrench:
						newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_GREAT_KINGDOM", ())%(sKingdom,curAdj)
					else:
						newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_GREAT_KINGDOM", ())%(curAdj,sKingdom)
				else:
					sOf = TRNSLTR.getText("TXT_KEY_MOD_DCN_THE_BLANK_OF", ()).replace('%s','')
					if not bForceUpdate and sKingdom in curDesc and (not sOf in curDesc or pPlayer.getNumCities < 6) and not sGreat in curDesc:
						newName = curDesc
					elif 50 <= GAME.getSorenRandNum(100,'Rev: Naming'):
						newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_THE_BLANK_OF", ())%(sKingdom,curShort)
					elif bFrench:
						newName = sKingdom + ' ' + curAdj
					else:
						newName = curAdj + ' ' + sKingdom

			elif RevUtils.getDemocracyLevel(iPlayer)[0] == -10 or playerEra == 0:

				empString = TRNSLTR.getText("TXT_KEY_MOD_DCN_PLAIN_EMPIRE", ())
				if playerEra < 2 and pPlayer.getNumCities() < 3:
					empString = TRNSLTR.getText("TXT_KEY_MOD_DCN_PLAIN_CITY_STATE", ())

				if pTeam.isAVassal():
					if 50 > GAME.getSorenRandNum(100,'Rev: Naming'):
						empString = TRNSLTR.getText("TXT_KEY_MOD_DCN_FIEFDOM", ())
					elif 50 > GAME.getSorenRandNum(100,'Rev: Naming'):
						empString = TRNSLTR.getText("TXT_KEY_MOD_DCN_PROTECTORATE", ())
					else:
						empString = TRNSLTR.getText("TXT_KEY_MOD_DCN_TERRITORY", ())

				if not bForceUpdate and empString in curDesc and not GAME.getGameTurn() == 0:
					newName = curDesc
				elif 50 <= GAME.getSorenRandNum(100,'Rev: Naming'):
					newName = TRNSLTR.getText("TXT_KEY_MOD_DCN_THE_BLANK_OF", ())%(empString,curShort)
				elif bFrench:
					newName = empString + ' ' + curAdj
				else:
					newName = curAdj + ' ' + empString

			sHoly = TRNSLTR.getText("TXT_KEY_MOD_DCN_HOLY", ()) + ' '
			if RevUtils.getReligiousFreedom(iPlayer)[0] < -9:
				if len(newName) < 16 and not sHoly in newName and not sGreat in newName and not newName.startswith(TRNSLTR.getText("TXT_KEY_MOD_DCN_HOLY_HRE_MATCH", ())):
					newName = sHoly + newName
			elif newName.startswith(sHoly) and not origDesc.startswith(sHoly):
				# Cut off any inappropriately saved 'Holy ' prefix
				newName = newName[len(sHoly):]

		if bPacifist:
			szPacifist = TRNSLTR.getText("TXT_KEY_MOD_DCN_PACIFIST", ())
			if not szPacifist in newName and 50 > GAME.getSorenRandNum(100,'Rev: Naming'):
				szPacifist = TRNSLTR.getText("TXT_KEY_MOD_DCN_PEACEFUL", ())

			if not szPacifist in newName:
				if bFrench:
					newName = newName + ' ' + szPacifist
				else:
					newName = szPacifist + ' ' + newName

		return [newName, curShort, curAdj]

	def resetName(self, iPlayer, bVerbose = True):
		pPlayer = GC.getPlayer(iPlayer)
		'''
		if (not GAME.isOption(GameOptionTypes.GAMEOPTION_LEAD_ANY_CIV)
		and (not self.bLeaveHumanName or not pPlayer.isHuman() and GAME.getActivePlayer() != iPlayer)
		and pPlayer.getLeaderType() in LeaderCivNames.LeaderCivNames.keys()
		):
		'''
		if (not GAME.isOption(GameOptionTypes.GAMEOPTION_LEAD_ANY_CIV)
		and pPlayer.getLeaderType() in LeaderCivNames.LeaderCivNames.keys()
		):
			origDesc, origShort, origAdj = LeaderCivNames.LeaderCivNames[pPlayer.getLeaderType()]
		else:
			civInfo = GC.getCivilizationInfo(pPlayer.getCivilizationType())
			origAdj = civInfo.getAdjective(0)
			origDesc = civInfo.getDescription()
			origShort = civInfo.getShortDescription(0)

		GC.getPlayer(iPlayer).setCivName(origDesc, origShort, origAdj)
