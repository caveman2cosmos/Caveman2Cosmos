## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *

DebugLogging = False # Adjusted from CvDiplomacyInterface

GC = CyGlobalContext()

class CvDiplomacy:
	"Code used by Civ Diplomacy interface"

	def __init__(self):
		"constructor - set up class vars, AI and User strings"
		if DebugLogging:
			print "Launching Diplomacy"
		self.iLastResponseID = -1
		self.diploScreen = CyDiplomacy()

	def determineResponses(self, eComment):
		"Will determine the user responses given an AI comment"
		szType = GC.getDiplomacyInfo(eComment).getType()
		if DebugLogging:
			print "CvDiplomacy.determineResponses: \n\t%s\n\t%s" %(eComment, szType)

		# Eliminate previous comments
		self.diploScreen.clearUserComments()

		# If the AI is declaring war
		if szType == "AI_DIPLOCOMMENT_DECLARE_WAR":
			# We respond to their declaration
			self.addUserComment("USER_DIPLOCOMMENT_WAR_RESPONSE")
			self.diploScreen.endTrade()

		# If this is the first time we are being contacted by the AI, or if the AI is no longer a vassal.
		elif szType in ("AI_DIPLOCOMMENT_FIRST_CONTACT", "AI_DIPLOCOMMENT_NO_VASSAL"):
			# if you are on different teams and NOT at war, give the user the option to declare war
			if GC.getTeam(GC.getGame().getActiveTeam()).canDeclareWar(GC.getPlayer(self.diploScreen.getWhoTradingWith()).getTeam()):
				self.addUserComment("USER_DIPLOCOMMENT_WAR")
			# We say hi and begin our peace
			self.addUserComment("USER_DIPLOCOMMENT_PEACE")
			self.diploScreen.endTrade()

		# The AI refuses to talk
		elif szType == "AI_DIPLOCOMMENT_REFUSE_TO_TALK":
			# Exit...
			self.addUserComment("USER_DIPLOCOMMENT_EXIT")
			self.diploScreen.endTrade()

		# If the AI is offering a city or to give help
		elif szType in ("AI_DIPLOCOMMENT_OFFER_CITY", "AI_DIPLOCOMMENT_GIVE_HELP"):
			# Accept
			self.addUserComment("USER_DIPLOCOMMENT_ACCEPT_OFFER")
			# Reject
			self.addUserComment("USER_DIPLOCOMMENT_REJECT_OFFER")

		# If the AI is offering a deal
		elif szType in ("AI_DIPLOCOMMENT_OFFER_PEACE", "AI_DIPLOCOMMENT_OFFER_DEAL", "AI_DIPLOCOMMENT_OFFER_VASSAL"):
			# Accept
			self.addUserComment("USER_DIPLOCOMMENT_ACCEPT_OFFER")
			# Negotiate
			self.addUserComment("USER_DIPLOCOMMENT_RENEGOTIATE")
			# Reject
			self.addUserComment("USER_DIPLOCOMMENT_REJECT_OFFER")

		# If the AI is cancelling a deal
		elif szType == "AI_DIPLOCOMMENT_CANCEL_DEAL":
			# Negotiate
			self.addUserComment("USER_DIPLOCOMMENT_RENEGOTIATE")
			# Exit...
			self.addUserComment("USER_DIPLOCOMMENT_NO_RENEGOTIATE")

		# If the AI is demanding tribute
		elif szType == "AI_DIPLOCOMMENT_ASK_FOR_HELP":
			# Accept
			self.addUserComment("USER_DIPLOCOMMENT_GIVE_HELP")
			# Reject
			self.addUserComment("USER_DIPLOCOMMENT_REFUSE_HELP")

		# If the AI is demanding tribute
		elif szType == "AI_DIPLOCOMMENT_DEMAND_TRIBUTE":
			# Accept
			self.addUserComment("USER_DIPLOCOMMENT_ACCEPT_DEMAND")
			# Reject
			self.addUserComment("USER_DIPLOCOMMENT_REJECT_DEMAND")

		# If the AI is pressuring us to convert to their religion
		elif szType == "AI_DIPLOCOMMENT_RELIGION_PRESSURE":
			# Accept
			self.addUserComment("USER_DIPLOCOMMENT_CONVERT")
			# Reject
			self.addUserComment("USER_DIPLOCOMMENT_NO_CONVERT")

		# If the AI is pressuring us to switch to their favorite civic
		elif szType == "AI_DIPLOCOMMENT_CIVIC_PRESSURE":
			# Accept
			self.addUserComment("USER_DIPLOCOMMENT_REVOLUTION")
			# Reject
			self.addUserComment("USER_DIPLOCOMMENT_NO_REVOLUTION")

		# If the AI is pressuring us to join their war
		elif szType == "AI_DIPLOCOMMENT_JOIN_WAR":
			# Accept
			self.addUserComment("USER_DIPLOCOMMENT_JOIN_WAR")
			# Reject
			self.addUserComment("USER_DIPLOCOMMENT_NO_JOIN_WAR")

		# If the AI is pressuring us to stop trading with their enemy
		elif szType == "AI_DIPLOCOMMENT_STOP_TRADING":
			# Accept
			self.addUserComment("USER_DIPLOCOMMENT_STOP_TRADING")
			# Reject
			self.addUserComment("USER_DIPLOCOMMENT_NO_STOP_TRADING")

		#If the AI is pressuring us to end a war with their friend
		elif szType == "AI_DIPLOCOMMENT_MAKE_PEACE_WITH":
			# Accept
			self.addUserComment("USER_DIPLOCOMMENT_MAKE_PEACE_WITH")
			# Reject
			self.addUserComment("USER_DIPLOCOMMENT_NO_MAKE_PEACE_WITH")

		# If we are viewing our current deals
		elif szType == "AI_DIPLOCOMMENT_CURRENT_DEALS":
			# Exit option
			self.addUserComment("USER_DIPLOCOMMENT_NEVERMIND")
			self.addUserComment("USER_DIPLOCOMMENT_EXIT")
			self.diploScreen.startTrade(eComment, True)

		# If we are trading or
		# If we are trying another proposal or
		# If they reject our offer or
		# If they reject our demand
		elif szType in (
		"AI_DIPLOCOMMENT_TRADING",	"AI_DIPLOCOMMENT_REJECT",		"AI_DIPLOCOMMENT_TRY_THIS_DEAL",	"AI_DIPLOCOMMENT_SORRY",
		"AI_DIPLOCOMMENT_NO_DEAL",	"AI_DIPLOCOMMENT_REJECT_ASK",	"AI_DIPLOCOMMENT_REJECT_DEMAND"
		): # If no one is currently offering anything
			bOurOfferEmpty = self.diploScreen.ourOfferEmpty()
			bTheirOfferEmpty = self.diploScreen.theirOfferEmpty()
			if bOurOfferEmpty and bTheirOfferEmpty:

				# If we are at war, allow us to suggest a peace treaty
				if self.diploScreen.atWar():
					self.addUserComment("USER_DIPLOCOMMENT_PROPOSE")
					self.addUserComment("USER_DIPLOCOMMENT_OFFER_PEACE")

			# If one of us has something on the table
			if not bOurOfferEmpty or not bTheirOfferEmpty:

				# If the offer is from the AI
				if self.diploScreen.isAIOffer():

					# We can accept or reject the offer
					self.addUserComment("USER_DIPLOCOMMENT_ACCEPT")
					self.addUserComment("USER_DIPLOCOMMENT_REJECT")

				else: # Otherwise this is a player offer to the AI

					# This is a two way deal
					if not bOurOfferEmpty and not bTheirOfferEmpty:

						# Insert the propose trade button
						self.addUserComment("USER_DIPLOCOMMENT_PROPOSE")

						# During peace, see what we can get for these items
						if not self.diploScreen.atWar():
							if GC.getGame().getActiveTeam() != GC.getPlayer(self.diploScreen.getWhoTradingWith()).getTeam():
								self.addUserComment("USER_DIPLOCOMMENT_COMPLETE_DEAL")

					# Otherwise they have something on the table and we dont
					elif not bTheirOfferEmpty:

						# If we are at war, demand the items for peace or ask what they want
						if self.diploScreen.atWar():
							self.addUserComment("USER_DIPLOCOMMENT_PROPOSE")

						else: # Otherwise (during peacetime) ask what they want for our item or demand they give it to us
							CyPlayer = GC.getPlayer(self.diploScreen.getWhoTradingWith())
							iTeam = CyPlayer.getTeam()
							iTeamAct = GC.getGame().getActiveTeam()

							if iTeamAct == iTeam:
								self.addUserComment("USER_DIPLOCOMMENT_DEMAND_TEAM")

							else:
								CyTeam = GC.getTeam(iTeam)
								self.addUserComment("USER_DIPLOCOMMENT_OFFER")
								bVassal = CyTeam.isVassal(iTeamAct)
								if bVassal and self.diploScreen.theirVassalTribute():
									self.addUserComment("USER_DIPLOCOMMENT_VASSAL_TRIBUTE")
								elif CyPlayer.AI_getAttitude(GC.getGame().getActivePlayer()) >= AttitudeTypes.ATTITUDE_PLEASED:
									self.addUserComment("USER_DIPLOCOMMENT_ASK")
								elif bVassal or GC.getTeam(iTeamAct).canDeclareWar(iTeam):
									self.addUserComment("USER_DIPLOCOMMENT_DEMAND")

					else: # Otherwise we have something on the table and they dont
						# If we are at war, use this item to fish for peace or propose peace with the items
						if self.diploScreen.atWar():
							self.addUserComment("USER_DIPLOCOMMENT_PROPOSE")

						# During peace, see what we can get for these items or simply gift them to the AI
						else:
							if GC.getGame().getActiveTeam() != GC.getPlayer(self.diploScreen.getWhoTradingWith()).getTeam():
								self.addUserComment("USER_DIPLOCOMMENT_FISH_FOR_DEAL")

							self.addUserComment("USER_DIPLOCOMMENT_GIFT")

			# Exit option
			self.addUserComment("USER_DIPLOCOMMENT_NEVERMIND")
			self.addUserComment("USER_DIPLOCOMMENT_EXIT")
			self.diploScreen.startTrade(eComment, False)

		elif szType == "AI_DIPLOCOMMENT_SOMETHING_ELSE":
			iTeamAct = GC.getGame().getActiveTeam()
			CyTeamAct = GC.getTeam(iTeamAct)
			iTeam = GC.getPlayer(self.diploScreen.getWhoTradingWith()).getTeam()

			if CyTeamAct.canDeclareWar(iTeam):
				self.addUserComment("USER_DIPLOCOMMENT_WAR")

			self.addUserComment("USER_DIPLOCOMMENT_ATTITUDE")

			if iTeamAct == iTeam or GC.getTeam(iTeam).isVassal(iTeamAct):
				self.addUserComment("USER_DIPLOCOMMENT_RESEARCH")

			if CyTeamAct.AI_shareWar(iTeam):
				self.addUserComment("USER_DIPLOCOMMENT_TARGET")

			self.addUserComment("USER_DIPLOCOMMENT_NEVERMIND")
			self.addUserComment("USER_DIPLOCOMMENT_EXIT")

		elif szType == "AI_DIPLOCOMMENT_RESEARCH":
			CyPlayer = GC.getPlayer(self.diploScreen.getWhoTradingWith())
			for i in range(GC.getNumTechInfos()):
				if CyPlayer.canResearch(i):
					self.addUserComment("USER_DIPLOCOMMENT_RESEARCH_TECH", i, -1, GC.getTechInfo(i).getTextKey())

			self.addUserComment("USER_DIPLOCOMMENT_SOMETHING_ELSE")
			self.addUserComment("USER_DIPLOCOMMENT_EXIT")

		elif szType in (
		"AI_DIPLOCOMMENT_ATTITUDE_PLAYER_FURIOUS",	"AI_DIPLOCOMMENT_ATTITUDE_PLAYER_CAUTIOUS", "AI_DIPLOCOMMENT_ATTITUDE",
		"AI_DIPLOCOMMENT_ATTITUDE_PLAYER_ANNOYED",	"AI_DIPLOCOMMENT_ATTITUDE_PLAYER_FRIENDLY", "AI_DIPLOCOMMENT_ATTITUDE_PLAYER_PLEASED"
		):
			iPlayerAct = GC.getGame().getActivePlayer()
			CyTeamAct = GC.getTeam(GC.getGame().getActiveTeam())
			iPlayer = self.diploScreen.getWhoTradingWith()
			CyTeam = GC.getTeam(GC.getPlayer(iPlayer).getTeam())

			for iPlayerX in range(GC.getMAX_PC_PLAYERS()):
				if iPlayerX in (iPlayerAct, iPlayer): continue
				CyPlayerX = GC.getPlayer(iPlayerX)
				if CyPlayerX.isAlive():
					iTeamX = CyPlayerX.getTeam()
					if CyTeamAct.isHasMet(iTeamX) and CyTeam.isHasMet(iTeamX):
						self.addUserComment("USER_DIPLOCOMMENT_ATTITUDE_PLAYER", iPlayerX, -1, CyPlayerX.getNameKey())

			self.addUserComment("USER_DIPLOCOMMENT_SOMETHING_ELSE")
			self.addUserComment("USER_DIPLOCOMMENT_EXIT")

		elif szType == "AI_DIPLOCOMMENT_TARGET":
			iTeamAct = GC.getGame().getActiveTeam()
			CyTeamAct = GC.getTeam(iTeamAct)
			for iPlayerX in range(GC.getMAX_PC_PLAYERS()):
				CyPlayerX = GC.getPlayer(iPlayerX)
				if CyPlayerX.isAlive() and CyTeamAct.isAtWar(CyPlayerX.getTeam()):
					for CyCity in CyPlayerX.cities():
						if CyCity.isRevealed(iTeamAct, False):
							self.addUserComment("USER_DIPLOCOMMENT_TARGET_CITY", iPlayerX, CyCity.getID(), CyCity.getNameKey())

			self.addUserComment("USER_DIPLOCOMMENT_SOMETHING_ELSE")
			self.addUserComment("USER_DIPLOCOMMENT_EXIT")

		else: # The default...
			iPlayerAct = GC.getGame().getActivePlayer()
			iPlayer = self.diploScreen.getWhoTradingWith()
			CyPlayer = GC.getPlayer(iPlayer)
			if GC.getPlayer(iPlayerAct).canTradeWith(iPlayer):
				# Allow us to begin another proposal
				self.addUserComment("USER_DIPLOCOMMENT_PROPOSAL")

			if szType in (
			"AI_DIPLOCOMMENT_GREETINGS",	"AI_DIPLOCOMMENT_UNIT_BRAG",	"AI_DIPLOCOMMENT_WORST_ENEMY_TRADING",
			"AI_DIPLOCOMMENT_WORST_ENEMY",	"AI_DIPLOCOMMENT_NUKES"
			): # If we are at war, allow to suggest peace
				if self.diploScreen.atWar():
					self.addUserComment("USER_DIPLOCOMMENT_SUGGEST_PEACE")

			# If we have a current deal, allow us to see the deals
			if self.diploScreen.hasAnnualDeal():
				self.addUserComment("USER_DIPLOCOMMENT_CURRENT_DEALS")

			self.addUserComment("USER_DIPLOCOMMENT_SOMETHING_ELSE")

			if CyPlayer and not GC.getGame().isNetworkMultiPlayer():
				if szType in ("AI_ASSUME_REALLY_SNUFFED", "AI_ASSUME_SNUFFED", "AI_ASSUME_NOT_SNUFFED"):
					self.addUserComment("USER_RESUME_TALKS")
				elif szType in ("AI_RESUME_TALKS_RELUCTANT", "AI_RESUME_TALKS", "AI_RESUME_TALKS_GLADLY"):
					self.addUserComment("USER_DO_NOT_BOTHER_US", iPlayerAct)
				elif CyPlayer.isDoNotBotherStatus(iPlayerAct):
					self.addUserComment("USER_RESUME_TALKS")
				else:
					self.addUserComment("USER_DO_NOT_BOTHER_US", iPlayerAct)

			# Exit potential
			self.addUserComment("USER_DIPLOCOMMENT_EXIT")
			self.diploScreen.endTrade()


	def addUserComment(self, szType, iData1 = -1, iData2 = -1, *args):
		" Helper for adding User Comments "
		iComment = GC.getInfoTypeForString(szType)
		self.diploScreen.addUserComment(iComment, iData1, iData2, self.getDiplomacyComment(iComment), args)

	def setAIComment(self, eComment, *args):
		" Handles the determining the AI comments"
		AIString = self.getDiplomacyComment(eComment)

		if DebugLogging:
			print "CvDiplomacy.setAIComment: %s" %(eComment,)
			if args:
				print "args", args
			AIString = "(%d) - %s" %(self.iLastResponseID, AIString)

		self.diploScreen.setAIString(AIString, args)
		self.diploScreen.setAIComment(eComment)
		self.determineResponses(eComment)
#		self.performHeadAction(eComment) # We have static leaderhaeads

	'''
	def performHeadAction(self, eComment):

		if eComment in (
		GC.getInfoTypeForString("AI_DIPLOCOMMENT_NO_PEACE"),	GC.getInfoTypeForString("AI_DIPLOCOMMENT_REJECT"),
		GC.getInfoTypeForString("AI_DIPLOCOMMENT_NO_DEAL"),		GC.getInfoTypeForString("AI_DIPLOCOMMENT_CANCEL_DEAL"),
		GC.getInfoTypeForString("AI_DIPLOCOMMENT_REJECT_ASK"),	GC.getInfoTypeForString("AI_DIPLOCOMMENT_REJECT_DEMAND")
		):
			self.diploScreen.performHeadAction(LeaderheadAction.LEADERANIM_DISAGREE)

		elif eComment in (GC.getInfoTypeForString("AI_DIPLOCOMMENT_ACCEPT"), GC.getInfoTypeForString("AI_DIPLOCOMMENT_TRY_THIS_DEAL")):
			self.diploScreen.performHeadAction(LeaderheadAction.LEADERANIM_AGREE)

		elif eComment in (
		GC.getInfoTypeForString("AI_DIPLOCOMMENT_FIRST_CONTACT"),		GC.getInfoTypeForString("AI_DIPLOCOMMENT_GREETINGS"),
		GC.getInfoTypeForString("AI_DIPLOCOMMENT_WORST_ENEMY"),			GC.getInfoTypeForString("AI_DIPLOCOMMENT_UNIT_BRAG"),
		GC.getInfoTypeForString("AI_DIPLOCOMMENT_NUKES"),				GC.getInfoTypeForString("AI_DIPLOCOMMENT_OFFER_PEACE"),
		GC.getInfoTypeForString("AI_DIPLOCOMMENT_OFFER_VASSAL"),		GC.getInfoTypeForString("AI_DIPLOCOMMENT_OFFER_CITY"),
		GC.getInfoTypeForString("AI_DIPLOCOMMENT_OFFER_DEAL"),			GC.getInfoTypeForString("AI_DIPLOCOMMENT_GIVE_HELP"),
		GC.getInfoTypeForString("AI_DIPLOCOMMENT_RELIGION_PRESSURE"),	GC.getInfoTypeForString("AI_DIPLOCOMMENT_CIVIC_PRESSURE"),
		GC.getInfoTypeForString("AI_DIPLOCOMMENT_JOIN_WAR"),			GC.getInfoTypeForString("AI_DIPLOCOMMENT_STOP_TRADING"),
		GC.getInfoTypeForString("AI_DIPLOCOMMENT_MAKE_PEACE_WITH"),		GC.getInfoTypeForString("AI_DIPLOCOMMENT_ASK_FOR_HELP"),
		GC.getInfoTypeForString("AI_DIPLOCOMMENT_DEMAND_TRIBUTE")
		):
			self.diploScreen.performHeadAction(LeaderheadAction.LEADERANIM_GREETING)

		elif eComment in (
		GC.getInfoTypeForString("AI_DIPLOCOMMENT_ATTITUDE_PLAYER_FURIOUS"),	GC.getInfoTypeForString("AI_DIPLOCOMMENT_WORST_ENEMY_TRADING"),
		GC.getInfoTypeForString("AI_DIPLOCOMMENT_HELP_REFUSED"),			GC.getInfoTypeForString("AI_DIPLOCOMMENT_DEMAND_REJECTED"),
		GC.getInfoTypeForString("AI_DIPLOCOMMENT_RELIGION_DENIED"),			GC.getInfoTypeForString("AI_DIPLOCOMMENT_CIVIC_DENIED"),
		GC.getInfoTypeForString("AI_DIPLOCOMMENT_JOIN_DENIED"),				GC.getInfoTypeForString("AI_DIPLOCOMMENT_STOP_DENIED")
		):
			self.diploScreen.performHeadAction(LeaderheadAction.LEADERANIM_FURIOUS)

		elif eComment == GC.getInfoTypeForString("AI_DIPLOCOMMENT_ATTITUDE_PLAYER_ANNOYED"):
			self.diploScreen.performHeadAction(LeaderheadAction.LEADERANIM_ANNOYED)

		elif eComment == GC.getInfoTypeForString("AI_DIPLOCOMMENT_ATTITUDE_PLAYER_CAUTIOUS"):
			self.diploScreen.performHeadAction(LeaderheadAction.LEADERANIM_CAUTIOUS)

		elif eComment in (GC.getInfoTypeForString("AI_DIPLOCOMMENT_ATTITUDE_PLAYER_PLEASED"), GC.getInfoTypeForString("AI_DIPLOCOMMENT_SORRY")):
			self.diploScreen.performHeadAction(LeaderheadAction.LEADERANIM_PLEASED)

		elif eComment in (
		GC.getInfoTypeForString("AI_DIPLOCOMMENT_ATTITUDE_PLAYER_FRIENDLY"), GC.getInfoTypeForString("AI_DIPLOCOMMENT_GLAD"),
		GC.getInfoTypeForString("AI_DIPLOCOMMENT_THANKS")
		):
			self.diploScreen.performHeadAction(LeaderheadAction.LEADERANIM_FRIENDLY)
	'''

	def getDiplomacyComment(self, eComment):
		"Function to get the user String"
		debugString = "CvDiplomacy.getDiplomacyComment: %s" %(eComment,)
		eComment = int(eComment)
		if DebugLogging:
			print debugString, eComment

		CvDiplomacyInfo = GC.getDiplomacyInfo(eComment)
		if CvDiplomacyInfo:
			szString = self.filterUserResponse(CvDiplomacyInfo)
		else:
			print "CvDiplomacy.getDiplomacyComment: %s does not exist!" %(eComment,)
			szString = "Error***: No string found for eComment: %s" % eComment
		return szString

	def isUsed(self, var, i, num):
		"returns True if any element in the var list is True"
		for j in range(num):
			if (var(i, j)):
				return True
		return False

	def filterUserResponse(self, diploInfo):
		"pick the user's response from a CvDiplomacyTextInfo, based on response conditions"
		iPlayer = self.diploScreen.getWhoTradingWith()
		if iPlayer == -1:
			return ""
		CyPlayer = GC.getPlayer(iPlayer)
		iCiv	= CyPlayer.getCivilizationType()
		iLeader	= CyPlayer.getLeaderType()

		CyPlayerAct = GC.getActivePlayer()
		iPlayerAct = CyPlayerAct.getID()

		iTheirPower = CyPlayer.getPower()
		iOurPower = CyPlayerAct.getPower()
		bUsSuperior = iOurPower > iTheirPower * 2
		bUsInferior = iOurPower < iTheirPower / 2
		del iTheirPower, iOurPower
		iAttitude = CyPlayer.AI_getAttitude(iPlayerAct)

		responses = []
		for i in range(diploInfo.getNumResponses()):

			# check attitude of other player towards us
			if self.isUsed(diploInfo.getAttitudeTypes, i, AttitudeTypes.NUM_ATTITUDE_TYPES) and not diploInfo.getAttitudeTypes(i, iAttitude):
				continue

			# check civ type
			if self.isUsed(diploInfo.getCivilizationTypes, i, GC.getNumCivilizationInfos()) and not diploInfo.getCivilizationTypes(i, iCiv):
				continue

			# check leader type
			if self.isUsed(diploInfo.getLeaderHeadTypes, i, GC.getNumLeaderHeadInfos()) and not diploInfo.getLeaderHeadTypes(i, iLeader):
				continue

			# check power type
			if self.isUsed(diploInfo.getDiplomacyPowerTypes, i, DiplomacyPowerTypes.NUM_DIPLOMACYPOWER_TYPES):

				if bUsInferior and not diploInfo.getDiplomacyPowerTypes(i, DiplomacyPowerTypes.DIPLOMACYPOWER_STRONGER):
					continue

				elif bUsSuperior and not diploInfo.getDiplomacyPowerTypes(i, DiplomacyPowerTypes.DIPLOMACYPOWER_WEAKER):
					continue

				elif diploInfo.getDiplomacyPowerTypes(i, DiplomacyPowerTypes.DIPLOMACYPOWER_EQUAL):
					continue

			# passed all tests, so add to response list
			for j in range(diploInfo.getNumDiplomacyText(i)):
				responses.append(diploInfo.getDiplomacyText(i, j))

		# pick a random response
		if responses:
			iResponse = GC.getASyncRand().get(len(responses), "Python Diplomacy ASYNC")
			self.iLastResponseID = iResponse
			return responses[iResponse]

		return ""	# no responses matched

	def handleUserResponse(self, eComment, iData1, iData2):
		if DebugLogging:
			print "CvDiplomacy.handleUserResponse: %s" %(eComment,)

		diploScreen = CyDiplomacy()
		szType = GC.getDiplomacyInfo(eComment).getType()

		# If we accept peace
		if szType == "USER_DIPLOCOMMENT_PEACE":
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_PEACE"))

		# If we choose war
		elif szType == "USER_DIPLOCOMMENT_WAR":
			diploScreen.declareWar()

		# If we wish to make a trade proposal or try to renegotiate
		elif szType in ("USER_DIPLOCOMMENT_PROPOSAL", "USER_DIPLOCOMMENT_RENEGOTIATE"):
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_TRADING"))
			diploScreen.showAllTrade(True)

		# If we want to propose a trade
		elif szType == "USER_DIPLOCOMMENT_PROPOSE":
			if diploScreen.offerDeal():
				self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_ACCEPT"))
			else:
				self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_REJECT"))

		# If we ask for peace
		elif szType == "USER_DIPLOCOMMENT_SUGGEST_PEACE":
			if diploScreen.offerDeal():
				self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_PEACE"))
			else:
				self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_NO_PEACE"))

		# If we accept a trade
		elif szType == "USER_DIPLOCOMMENT_ACCEPT":
			diploScreen.implementDeal()
			diploScreen.setAIOffer(0)
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_GLAD"))

		# If we reject a trade
		elif szType == "USER_DIPLOCOMMENT_REJECT":
			diploScreen.setAIOffer(0)
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_SORRY"))

		# If we offer a deal, or is we are fishing for a deal, or if we are offering peace or fishing for peace
		elif szType in ("USER_DIPLOCOMMENT_OFFER", "USER_DIPLOCOMMENT_COMPLETE_DEAL", "USER_DIPLOCOMMENT_FISH_FOR_DEAL", "USER_DIPLOCOMMENT_OFFER_PEACE"):
			if diploScreen.counterPropose():
				self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_TRY_THIS_DEAL"))
			else:
				self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_NO_DEAL"))

		# if we are asking for something
		elif szType == "USER_DIPLOCOMMENT_ASK":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ASK_HELP, -1, -1)
			if diploScreen.offerDeal():
				self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_ACCEPT_ASK"))
			else:
				self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_REJECT_ASK"))

		# if we are demanding something
		elif szType == "USER_DIPLOCOMMENT_DEMAND":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_MADE_DEMAND, -1, -1)
			if diploScreen.offerDeal():
				self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_ACCEPT_DEMAND"))
			else:
				self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_REJECT_DEMAND"))

		# if we are demanding something with the threat of war
		elif szType == "USER_DIPLOCOMMENT_VASSAL_TRIBUTE":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_MADE_DEMAND_VASSAL, -1, -1)
			if diploScreen.offerDeal():
				self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_ACCEPT_DEMAND"))
			else:
				self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_DECLARE_WAR"))
				diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_DEMAND_WAR, -1, -1)

		# if we are demanding something from our teammate
		elif szType == "USER_DIPLOCOMMENT_DEMAND_TEAM":
			diploScreen.offerDeal()
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_ACCEPT_DEMAND_TEAM"))

		# If we are giving a gift
		elif szType == "USER_DIPLOCOMMENT_GIFT":
			diploScreen.offerDeal()
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_THANKS"))

		# If we decide to view current deals
		elif szType == "USER_DIPLOCOMMENT_CURRENT_DEALS":
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_CURRENT_DEALS"))
			diploScreen.showAllTrade(False)

		# If we give help
		elif szType == "USER_DIPLOCOMMENT_GIVE_HELP":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_GIVE_HELP, -1, -1)
			diploScreen.implementDeal()
			diploScreen.setAIOffer(0)
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_THANKS"))

		# If we accept their demand
		elif szType == "USER_DIPLOCOMMENT_ACCEPT_DEMAND":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_ACCEPT_DEMAND, -1, -1)
			diploScreen.implementDeal()
			diploScreen.setAIOffer(0)
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_THANKS"))

		# If we accept the offer
		elif szType == "USER_DIPLOCOMMENT_ACCEPT_OFFER":
			diploScreen.implementDeal()
			diploScreen.setAIOffer(0)
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_THANKS"))

		# If we refuse to help
		elif szType == "USER_DIPLOCOMMENT_REFUSE_HELP":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_REFUSED_HELP, -1, -1)
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_HELP_REFUSED"))

		# If we reject their demand
		elif szType == "USER_DIPLOCOMMENT_REJECT_DEMAND":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_REJECTED_DEMAND, -1, -1)

			if GC.getPlayer(self.diploScreen.getWhoTradingWith()).AI_demandRebukedWar(GC.getGame().getActivePlayer()):
				self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_DECLARE_WAR"))
				diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_DEMAND_WAR, -1, -1)
			else:
				self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_DEMAND_REJECTED"))

		# If we convert to their state religion
		elif szType == "USER_DIPLOCOMMENT_CONVERT":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_CONVERT, -1, -1)
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_THANKS"))

		# If we refuse to convert to their state religion
		elif szType == "USER_DIPLOCOMMENT_NO_CONVERT":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_NO_CONVERT, -1, -1)
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_RELIGION_DENIED"))

		# If we adopt their favorite civic
		elif szType == "USER_DIPLOCOMMENT_REVOLUTION":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_REVOLUTION, -1, -1)
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_THANKS"))

		# If we refuse to adopt their favorite civic
		elif szType == "USER_DIPLOCOMMENT_NO_REVOLUTION":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_NO_REVOLUTION, -1, -1)
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_CIVIC_DENIED"))

		# If we join their war
		elif szType == "USER_DIPLOCOMMENT_JOIN_WAR":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_JOIN_WAR, diploScreen.getData(), -1)
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_THANKS"))

		# If we refuse to join their war
		elif szType == "USER_DIPLOCOMMENT_NO_JOIN_WAR":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_NO_JOIN_WAR, -1, -1)
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_JOIN_DENIED"))

		# If we stop the trading
		elif szType == "USER_DIPLOCOMMENT_STOP_TRADING":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_STOP_TRADING, diploScreen.getData(), -1)
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_THANKS"))

		# If we refuse to stop the trading
		elif szType == "USER_DIPLOCOMMENT_NO_STOP_TRADING":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_NO_STOP_TRADING, -1, -1)
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_STOP_DENIED"))

		#If we accept making peace with their ally
		elif szType == "USER_DIPLOCOMMENT_MAKE_PEACE_WITH":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_MAKE_PEACE_WITH, diploScreen.getData(), -1)
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_THANKS"))

		#If we refuse to make peace with their ally
		elif szType == "USER_DIPLOCOMMENT_NO_MAKE_PEACE_WITH":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_NO_MAKE_PEACE_WITH, diploScreen.getData(), -1)
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_MAKE_PEACE_DENIED"))

		# If we want to go back to first screen
		elif szType == "USER_DIPLOCOMMENT_NEVERMIND":
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_WELL"))

		# If we want to discuss something else
		elif szType == "USER_DIPLOCOMMENT_SOMETHING_ELSE":
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_SOMETHING_ELSE"))

		# If we want to ask them to change their research
		elif szType == "USER_DIPLOCOMMENT_RESEARCH":
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_RESEARCH"))

		# If we want to ask them to change their research to a specific tech
		elif szType == "USER_DIPLOCOMMENT_RESEARCH_TECH":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_RESEARCH_TECH, iData1, -1)
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_RESEARCH_TECH"))

		# If we want to ask them to what their attitude is
		elif szType == "USER_DIPLOCOMMENT_ATTITUDE":
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_ATTITUDE"))

		# If we want to ask them to what their attitude is on a specific player
		elif szType == "USER_DIPLOCOMMENT_ATTITUDE_PLAYER":
			eAttitude = GC.getPlayer(self.diploScreen.getWhoTradingWith()).AI_getAttitude(iData1)

			if eAttitude == AttitudeTypes.ATTITUDE_FURIOUS:
				self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_ATTITUDE_PLAYER_FURIOUS"), GC.getPlayer(iData1).getNameKey())
			elif eAttitude == AttitudeTypes.ATTITUDE_ANNOYED:
				self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_ATTITUDE_PLAYER_ANNOYED"), GC.getPlayer(iData1).getNameKey())
			elif eAttitude == AttitudeTypes.ATTITUDE_CAUTIOUS:
				self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_ATTITUDE_PLAYER_CAUTIOUS"), GC.getPlayer(iData1).getNameKey())
			elif eAttitude == AttitudeTypes.ATTITUDE_PLEASED:
				self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_ATTITUDE_PLAYER_PLEASED"), GC.getPlayer(iData1).getNameKey())
			else:
				self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_ATTITUDE_PLAYER_FRIENDLY"), GC.getPlayer(iData1).getNameKey())

		# If we want to ask them to change their target
		elif szType == "USER_DIPLOCOMMENT_TARGET":
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_TARGET"))

		# If we want to ask them to change their target to a specific city
		elif szType == "USER_DIPLOCOMMENT_TARGET_CITY":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_TARGET_CITY, iData1, iData2)
			self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_TARGET_CITY"))

		# If we have asked them to not bother us
		elif szType == "USER_DO_NOT_BOTHER_US":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_DO_NOT_BOTHER, iData1, -1)

			eAttitude = GC.getPlayer(self.diploScreen.getWhoTradingWith()).AI_getAttitude(GC.getGame().getActivePlayer())
			if eAttitude in (AttitudeTypes.ATTITUDE_FURIOUS, AttitudeTypes.ATTITUDE_ANNOYED):
				self.setAIComment(GC.getInfoTypeForString("AI_ASSUME_REALLY_SNUFFED"))
			elif eAttitude == AttitudeTypes.ATTITUDE_CAUTIOUS:
				self.setAIComment(GC.getInfoTypeForString("AI_ASSUME_SNUFFED"))
			else:
				self.setAIComment(GC.getInfoTypeForString("AI_ASSUME_NOT_SNUFFED"))

		# If we have asked them to resume talks
		elif szType == "USER_RESUME_TALKS":
			diploScreen.diploEvent(DiploEventTypes.DIPLOEVENT_RESUME_BOTHER, -1, -1)

			eAttitude = GC.getPlayer(self.diploScreen.getWhoTradingWith()).AI_getAttitude(GC.getGame().getActivePlayer())
			if eAttitude in (AttitudeTypes.ATTITUDE_FURIOUS, AttitudeTypes.ATTITUDE_ANNOYED):
				self.setAIComment(GC.getInfoTypeForString("AI_RESUME_TALKS_RELUCTANT"))
			elif eAttitude == AttitudeTypes.ATTITUDE_CAUTIOUS:
				self.setAIComment(GC.getInfoTypeForString("AI_RESUME_TALKS"))
			else:
				self.setAIComment(GC.getInfoTypeForString("AI_RESUME_TALKS_GLADLY"))

		else:
			diploScreen.closeScreen()

	def dealCanceled(self):		# Called by CvDiplomacyInterface
		self.setAIComment(GC.getInfoTypeForString("AI_DIPLOCOMMENT_TRADING"))
