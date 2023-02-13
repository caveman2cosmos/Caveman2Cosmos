# RevolutionInit.py
#
# by jdog5000
# Version 2.2

# This file should be imported into CvCustomEventManager and the
# __init__ function then called from the event handler initilization
# spot using:
#
# RevolutionInit.RevolutionInit( self, configFileName )
#
# where configFileName is nominally "Revolution.ini".

from CvPythonExtensions import *
import RevEvents
import BarbarianCiv
import AIAutoPlay
import ChangePlayer
import Revolution
import RevInstances
import BugCore

GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()
RevDCMOpt = BugCore.game.RevDCM
ANewDawnOpt = BugCore.game.RoMSettings
RevOpt = BugCore.game.Revolution

class RevolutionInit:

	def __init__(self, customEM):
		print "RevolutionInit.__init__"

		self.customEM = customEM
		self.bFirst = True

		customEM.addEventHandler("kbdEvent", self.onKbdEvent)
		customEM.addEventHandler('GameStart', self.onGameStart)
		customEM.addEventHandler('OnLoad', self.onGameLoad)

		# Determine if game is already running and Python has just been reloaded
		if GAME.isFinalInitialized():
			self.onGameLoad(bShowPopup = False)


	def onKbdEvent(self, argsList):
		eventType,key,mx,my,px,py = argsList

		if eventType == 6:
			theKey = int(key)
			if theKey == int(InputTypes.KB_Q) and self.customEM.bShift and self.customEM.bCtrl:
				self.showActivePopup()

	def onGameStart(self, argsList):
		self.onGameLoad()

	def onGameLoad(self, argsList = None, bForceReinit = False, bShowPopup = True):
		# Remove any running mod components
		bDoUnInit = bForceReinit or RevInstances.bIsInitialized
		bDoInit = bDoUnInit or not RevInstances.bIsInitialized
		if bDoUnInit:
			if not RevInstances.BarbarianCivInst == None:
				RevInstances.BarbarianCivInst.removeEventHandlers()
				RevInstances.BarbarianCivInst = None
			if not RevInstances.RevolutionInst == None:
				RevEvents.removeEventHandlers()
				RevInstances.RevolutionInst.removeEventHandlers()
				RevInstances.RevolutionInst = None
			if not RevInstances.AIAutoPlayInst == None:
				RevInstances.AIAutoPlayInst.removeEventHandlers()
				RevInstances.AIAutoPlayInst = None
			if not RevInstances.ChangePlayerInst == None:
				RevInstances.ChangePlayerInst.removeEventHandlers()
				RevInstances.ChangePlayerInst = None

			RevInstances.bIsInitialized = False

		# Initialize mod components
		if bDoInit:
			RevInstances.bIsInitialized = True

			bAIAutoPlay = RevOpt.isAIAutoPlayEnable()
			if bAIAutoPlay:
				RevInstances.AIAutoPlayInst = AIAutoPlay.AIAutoPlay(self.customEM, RevOpt)

			if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_CIV):
				RevInstances.BarbarianCivInst = BarbarianCiv.BarbarianCiv(self.customEM, RevOpt)

			bChangePlayer = RevOpt.isChangePlayerEnable()
			if bChangePlayer:
				RevInstances.ChangePlayerInst = ChangePlayer.ChangePlayer(self.customEM, RevOpt)

			if GAME.isOption(GameOptionTypes.GAMEOPTION_REVOLUTION):
				# RevEvents needs to service beginPlayerTurn events before Revolution
				RevEvents.init(self.customEM, RevOpt)
				RevInstances.RevolutionInst = Revolution.Revolution(self.customEM, RevOpt)

		if bShowPopup and RevOpt.isActivePopup() and self.bFirst:
			self.showActivePopup()
			self.bFirst = False

		if bDoInit:
			CyInterface().setDirty(InterfaceDirtyBits.MiscButtons_DIRTY_BIT, True)
			CyInterface().setDirty(InterfaceDirtyBits.CityScreen_DIRTY_BIT, True)
			CyInterface().setDirty(InterfaceDirtyBits.MiscButtons_DIRTY_BIT, True)

	def showActivePopup(self):
		revMaxCivs = RevOpt.getRevMaxCivs()
		barbMaxCivs = RevOpt.getBarbCivMaxCivs()
		revDefaultNumPlayers = GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers()

		bodStr = self.getRevComponentsText()
		MAX_PC_PLAYERS = GC.getMAX_PC_PLAYERS()
		bodStr += TRNSLTR.getText("TXT_KEY_REV_MOD_MAX_CIVS_IN_DLL",(MAX_PC_PLAYERS, ))
		if revMaxCivs > 0 and revMaxCivs < MAX_PC_PLAYERS:
			bodStr += TRNSLTR.getText("TXT_KEY_REV_MOD_REVS_WILL_STOP_AT",( revMaxCivs, ))
		if barbMaxCivs > 0 and barbMaxCivs < MAX_PC_PLAYERS:
			bodStr += TRNSLTR.getText("TXT_KEY_REV_MOD_BARB_CIV_WILL_STOP_AT",( barbMaxCivs, ))
		bodStr += TRNSLTR.getText("TXT_KEY_REV_MOD_TURNS_IN_GAME",( GAME.getMaxTurns(), ))
		bodStr += TRNSLTR.getText("TXT_KEY_REV_MOD_DEFAULT_NUM_PLAYERS",( revDefaultNumPlayers, ))

		popup = CyPopup(-1, EventContextTypes.NO_EVENTCONTEXT, True)
		popup.setBodyString(bodStr, 1<<0)
		popup.setPosition(0, 12)
		screen = CyGInterfaceScreen("", 0)
		popup.setSize(screen.getXResolution() / 4, screen.getYResolution() - 64)
		popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)


	def getRevComponentsText(self):

		sectionFormat = "<font=3><color=200,200,0>\n"
		optionFormat = "<font=2><color=127,255,0>"
		szNewLineTab = "\n\t"
		revNoneText = "<font=2><color=0,0,255>\n\t" + TRNSLTR.getText("TXT_KEY_NONE",())

		revComponentsText = (
			"<color=250,170,0,255><font=4b>Caveman2Cosmos<font=2b><color=255,255,0>\n"
			+ TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_INIT_POPUP",())
			+ sectionFormat + TRNSLTR.getText("TXT_KEY_OPTIONS_GAME_REV",())
		)
		temp = ""
		for iI in range(GC.getNumGameOptionInfos()):
			if GAME.isOption(iI):
				temp += szNewLineTab + GC.getGameOptionInfo(iI).getDescription()
		if temp:
			revComponentsText += optionFormat + temp
			temp = ""
		else: revComponentsText += revNoneText

		revComponentsText += sectionFormat + TRNSLTR.getText("TXT_KEY_OPTIONS_BUG",())

		if ANewDawnOpt.isDefenderWithdraw():
			temp += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_ROMSETTINGS__DEFENDERWITHDRAW_TEXT",())

		if ANewDawnOpt.getMaxUnitsPerTile() > 0:
			temp += szNewLineTab + TRNSLTR.getText("TXT_KEY_AND_UNITS_PER_TILE",(ANewDawnOpt.getMaxUnitsPerTile(),))

		if ANewDawnOpt.isEnableFlexibleDifficulty():
			temp += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_ROMSETTINGS__ENABLEFLEXIBLEDIFFICULTY_TEXT",())

		if ANewDawnOpt.isBetterAirInterception():
			temp += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_ROMSETTINGS__BETTERAIRINTERCEPTION_TEXT",())

		if ANewDawnOpt.isDepletionMod():
			temp += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_ROMSETTINGS__DEPLETIONMOD_TEXT",())

		if ANewDawnOpt.isRealisiticDiplomacy():
			temp += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_ROMSETTINGS__REALISITICDIPLOMACY_TEXT",())

		if ANewDawnOpt.isImprovedXP():
			temp += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_ROMSETTINGS__IMPROVEDXP_TEXT",())

		if ANewDawnOpt.isBattlefieldPromotions():
			temp += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_ROMSETTINGS__BATTLEFIELDPROMOTIONS_TEXT",())

		if ANewDawnOpt.isReligionDecay():
			temp += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_ROMSETTINGS__RELIGIONDECAY_TEXT",())

		if ANewDawnOpt.isMultipleReligionSpread():
			temp += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_ROMSETTINGS__MULTIPLERELIGIONSPREAD_TEXT",())

		if ANewDawnOpt.isTelepathicReligion():
			temp += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_ROMSETTINGS__TELEPATHICRELIGION_TEXT",())

		if temp:
			revComponentsText += optionFormat + temp
			temp = ""
		else: revComponentsText += revNoneText

		# DCM
		revComponentsText += sectionFormat + TRNSLTR.getText("TXT_KEY_OPTIONS_DCM",())

		if RevDCMOpt.isDCM_RANGE_BOMBARD():
			temp += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_REVDCM__DCM_RANGE_BOMBARD_TEXT",())

		if RevDCMOpt.isDCM_OPP_FIRE():
			temp += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_REVDCM__DCM_OPP_FIRE_TEXT",())

		if RevDCMOpt.isDCM_AIR_BOMBING():
			temp += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_REVDCM__DCM_AIR_BOMBING_TEXT",())

		if RevDCMOpt.isDCM_ACTIVE_DEFENSE():
			temp += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_REVDCM__DCM_ACTIVE_DEFENSE_TEXT",())

		if RevDCMOpt.isDCM_FIGHTER_ENGAGE():
			temp += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_REVDCM__DCM_FIGHTER_ENGAGE_TEXT",())

		if GAME.isOption(GameOptionTypes.GAMEOPTION_INQUISITIONS) and RevDCMOpt.isOC_RESPAWN_HOLY_CITIES():
			temp += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_REVDCM__OC_RESPAWN_HOLY_CITIES_TEXT",())

		if temp:
			revComponentsText += optionFormat + temp
			temp = ""
		else: revComponentsText += revNoneText

		# Influence Driven War
		if RevDCMOpt.isIDW_ENABLED():
			revComponentsText += sectionFormat + TRNSLTR.getText("TXT_KEY_OPTIONS_IDW",()) + optionFormat + szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_REVDCM__IDW_ENABLED_TEXT",())

			if RevDCMOpt.isIDW_PILLAGE_INFLUENCE_ENABLED():
				revComponentsText += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_REVDCM__IDW_PILLAGE_INFLUENCE_ENABLED_TEXT",())

			if RevDCMOpt.isIDW_EMERGENCY_DRAFT_ENABLED():
				revComponentsText += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_REVDCM__IDW_EMERGENCY_DRAFT_ENABLED_TEXT",())

			if RevDCMOpt.isIDW_NO_BARBARIAN_INFLUENCE():
				revComponentsText += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_REVDCM__IDW_NO_BARBARIAN_INFLUENCE_TEXT",())

			if RevDCMOpt.isIDW_NO_NAVAL_INFLUENCE():
				revComponentsText += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_REVDCM__IDW_NO_NAVAL_INFLUENCE_TEXT",())

		# Super Spies
		revComponentsText += sectionFormat + TRNSLTR.getText("TXT_KEY_OPTIONS_SS",())

		if RevDCMOpt.isSS_ENABLED():
			temp += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_REVDCM__SS_ENABLED_TEXT",())

		if RevDCMOpt.isSS_BRIBE():
			temp += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_REVDCM__SS_BRIBE_TEXT",())

		if RevDCMOpt.isSS_ASSASSINATE():
			temp += szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_REVDCM__SS_ASSASSINATE_TEXT",())

		if temp:
			revComponentsText += optionFormat + temp
			temp = ""
		else: revComponentsText += revNoneText


		revHelpText = "<font=3b><color=255,255,0>\n"
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_GAME_SHORTCUTS",()) + "<font=2><color=255,255,255>"
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_BUG_OPTIONS_SHORTCUT",())
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_AI_AUTOPLAY",())

		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_CTRL_SHIFT_M_SHORTCUT",())
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ALT_X_SHORTCUT",())
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_CTRL_X_SHORTCUT",())
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ALT_S_SHORTCUT",())
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_CTRL_A_SHORTCUT",())
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ALT_M_SHORTCUT",())
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ALT_L_SHORTCUT",())
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ALT_E_SHORTCUT",())
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ALT_B_SHORTCUT",())
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ALT_F_SHORTCUT",())
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ALT_O_SHORTCUT",())
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_CTRL_L_ARROW_SHORTCUT",())
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_CTRL_R_ARROW_SHORTCUT",())
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_CTRL_I_SHORTCUT",())
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ALT_I_SHORTCUT",())
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ALT_CTRL_N_ARROW_SHORTCUT",())
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_SHIFT_F5_SHORTCUT",())
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_SHIFT_F8_SHORTCUT",())

		if GAME.isDebugMode():
			revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_CTRL_SHIFT_L_SHORTCUT",())
			revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_CTRL_SHIFT_P_SHORTCUT",())
		return revComponentsText + revHelpText