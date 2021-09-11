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
		self.RevOpt = RevOpt
		self.bShowActivePopup = RevOpt.isActivePopup()
		self.bFirst = True

		self.sectionFormat = "<font=3><color=200,200,0>"
		self.optionFormat = "<font=2><color=127,255,0>"
		self.noneOptionFormat = "<font=2><color=0,0,255>"
		self.helpTextTitle = "<font=3b><color=255,255,0>"
		self.helpTextFormat = "<font=2><color=255,255,255>"

		customEM.addEventHandler("kbdEvent", self.onKbdEvent)
		customEM.addEventHandler('GameStart', self.onGameStart)
		customEM.addEventHandler('OnLoad', self.onGameLoad)

		# Determine if game is already running and Python has just been reloaded
		if GAME.isFinalInitialized():
			self.onGameLoad(bShowPopup = False)



	def onKbdEvent(self, argsList):
		'keypress handler'
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
				RevInstances.AIAutoPlayInst = AIAutoPlay.AIAutoPlay(self.customEM, self.RevOpt)

			if GAME.isOption(GameOptionTypes.GAMEOPTION_BARBARIAN_CIV):
				RevInstances.BarbarianCivInst = BarbarianCiv.BarbarianCiv(self.customEM, self.RevOpt)

			bChangePlayer = RevOpt.isChangePlayerEnable()
			if bChangePlayer:
				RevInstances.ChangePlayerInst = ChangePlayer.ChangePlayer(self.customEM, self.RevOpt)

			if GAME.isOption(GameOptionTypes.GAMEOPTION_REVOLUTION):
				# RevEvents needs to service beginPlayerTurn events before Revolution
				RevEvents.init(self.customEM, self.RevOpt)
				RevInstances.RevolutionInst = Revolution.Revolution(self.customEM, self.RevOpt)

		if bShowPopup and self.bShowActivePopup and self.bFirst:
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

	#RevolutionDCM
	def getRevComponentsText(self):
		revComponentsText = "<color=250,170,0,255><font=4b>Caveman2Cosmos"
		revComponentsText += self.helpTextTitle + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_INIT_POPUP",())
		revNoneText = TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_NONE",())

		### RevolutionDCM start
		revComponentsText += self.sectionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_INIT",())
		szNewLineTab = "\n\t"
		anyOption = False
		for iI in range(GC.getNumGameOptionInfos()):
			if GAME.isOption(iI):
				anyOption = True
				revComponentsText += self.optionFormat + (szNewLineTab + GC.getGameOptionInfo(iI).getDescription())
		if not anyOption:
			revComponentsText += self.noneOptionFormat + revNoneText

		revComponentsText += self.sectionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_AND_OPTIONS",())
		anyOption = False
		if ANewDawnOpt.isDefenderWithdraw():
			revComponentsText += self.optionFormat + (szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_ROMSETTINGS__DEFENDERWITHDRAW_TEXT",()))
			anyOption = True
		if ANewDawnOpt.getMaxUnitsPerTile() > 0:
			revComponentsText += self.optionFormat + (szNewLineTab + TRNSLTR.getText("TXT_KEY_AND_UNITS_PER_TILE",(ANewDawnOpt.getMaxUnitsPerTile(),)))
			anyOption = True
		if ANewDawnOpt.isEnableFlexibleDifficulty():
			revComponentsText += self.optionFormat + (szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_ROMSETTINGS__ENABLEFLEXIBLEDIFFICULTY_TEXT",()))
			anyOption = True
		if ANewDawnOpt.isBetterAirInterception():
			revComponentsText += self.optionFormat + (szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_ROMSETTINGS__BETTERAIRINTERCEPTION_TEXT",()))
			anyOption = True
		if ANewDawnOpt.isDepletionMod():
			revComponentsText += self.optionFormat + (szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_ROMSETTINGS__DEPLETIONMOD_TEXT",()))
			anyOption = True
		if ANewDawnOpt.isRealisiticDiplomacy():
			revComponentsText += self.optionFormat + (szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_ROMSETTINGS__REALISITICDIPLOMACY_TEXT",()))
			anyOption = True
		if ANewDawnOpt.isImprovedXP():
			revComponentsText += self.optionFormat + (szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_ROMSETTINGS__IMPROVEDXP_TEXT",()))
			anyOption = True
		if ANewDawnOpt.isBattlefieldPromotions():
			revComponentsText += self.optionFormat + (szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_ROMSETTINGS__BATTLEFIELDPROMOTIONS_TEXT",()))
			anyOption = True
		if ANewDawnOpt.isMultipleReligionSpread():
			revComponentsText += self.optionFormat + (szNewLineTab + TRNSLTR.getText("TXT_KEY_BUG_OPT_ROMSETTINGS__MULTIPLERELIGIONSPREAD_TEXT",()))
			anyOption = True

		revComponentsText += self.sectionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_DCM_OPTIONS",())
		anyOption = False

		if RevDCMOpt.isDCM_ATTACK_SUPPORT():
			revComponentsText += self.optionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ATTACK_SUPPORT",())
			anyOption = True
		if RevDCMOpt.isDCM_RANGE_BOMBARD():
			revComponentsText += self.optionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_RANGE_BOMBARD",())
			anyOption = True
		if RevDCMOpt.isDCM_OPP_FIRE():
			revComponentsText += self.optionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_OPP_FIRE",())
			anyOption = True
		if RevDCMOpt.isDCM_AIR_BOMBING():
			revComponentsText += self.optionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_AIR_BOMBING",())
			anyOption = True
		if RevDCMOpt.isDCM_ACTIVE_DEFENSE():
			revComponentsText += self.optionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ACTIVE_DEFENSE",())
			anyOption = True
		if RevDCMOpt.isDCM_FIGHTER_ENGAGE():
			revComponentsText += self.optionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_FIGHTER_ENGAGE",())
			anyOption = True
		if not anyOption:
			revComponentsText += self.noneOptionFormat + revNoneText

		revComponentsText += self.sectionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_IDW_OPTIONS",())
		anyOption = False
		if RevDCMOpt.isIDW_ENABLED():
			revComponentsText += self.optionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_IDW_ENABLED",())
			anyOption = True
		if RevDCMOpt.isIDW_PILLAGE_INFLUENCE_ENABLED():
			revComponentsText += self.optionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_PILLAGE_INFLUENCE",())
			anyOption = True
		if RevDCMOpt.isIDW_EMERGENCY_DRAFT_ENABLED() :
			revComponentsText += self.optionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_EMERGENCY_DRAFT",())
			anyOption = True
		if RevDCMOpt.isIDW_NO_BARBARIAN_INFLUENCE():
			revComponentsText += self.optionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_NO_BARBARIAN_INFLUENCE",())
			anyOption = True
		if RevDCMOpt.isIDW_NO_NAVAL_INFLUENCE():
			revComponentsText += self.optionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_NO_NAVAL_INFLUENCE",())
			anyOption = True
		if not anyOption:
			revComponentsText += self.noneOptionFormat + revNoneText

		revComponentsText += self.sectionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_INQUISITIONS_OPTIONS",())
		anyOption = False
		if GAME.isOption(GameOptionTypes.GAMEOPTION_INQUISITIONS):
			revComponentsText += self.optionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_INQUISITIONS_ENABLED",())
			anyOption = True
		if GAME.isOption(GameOptionTypes.GAMEOPTION_INQUISITIONS) and RevDCMOpt.isOC_RESPAWN_HOLY_CITIES():
			revComponentsText += self.optionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_RESPAWN_HOLY_CITIES",())
			anyOption = True
		if not anyOption:
			revComponentsText += self.noneOptionFormat + revNoneText

		revComponentsText += self.sectionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_SS_OPTIONS",())
		anyOption = False
		if RevDCMOpt.isSS_ENABLED():
			revComponentsText += self.optionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_SS_ENABLED",())
			anyOption = True
		if RevDCMOpt.isSS_BRIBE():
			revComponentsText += self.optionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_SS_BRIBE",())
			anyOption = True
		if RevDCMOpt.isSS_ASSASSINATE():
			revComponentsText += self.optionFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_SS_ASSASSINATE",())
			anyOption = True
		if not anyOption:
			revComponentsText += self.noneOptionFormat + revNoneText

		revHelpText = self.helpTextTitle
		revHelpText += TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_GAME_SHORTCUTS",())
		revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_BUG_OPTIONS_SHORTCUT",())
		revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_AI_AUTOPLAY",())

		revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_CTRL_SHIFT_M_SHORTCUT",())
		revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ALT_X_SHORTCUT",())
		revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_CTRL_X_SHORTCUT",())
		revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ALT_S_SHORTCUT",())
		revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_CTRL_A_SHORTCUT",())
		revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ALT_M_SHORTCUT",())
		revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ALT_L_SHORTCUT",())
		revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ALT_E_SHORTCUT",())
		revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ALT_B_SHORTCUT",())
		revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ALT_F_SHORTCUT",())
		revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ALT_O_SHORTCUT",())
		revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_CTRL_L_ARROW_SHORTCUT",())
		revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_CTRL_R_ARROW_SHORTCUT",())
		revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_CTRL_I_SHORTCUT",())
		revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ALT_I_SHORTCUT",())
		revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_ALT_CTRL_N_ARROW_SHORTCUT",())
		revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_SHIFT_F5_SHORTCUT",())
		revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_SHIFT_F8_SHORTCUT",())

		if GAME.isDebugMode():
			revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_CTRL_SHIFT_L_SHORTCUT",())
			revHelpText += self.helpTextFormat + TRNSLTR.getText("TXT_KEY_REV_MOD_INITIALIZING_CTRL_SHIFT_P_SHORTCUT",())
		return revComponentsText + revHelpText