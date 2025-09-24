## Sid Meier's Civilization 4
## Copyright Firaxis Games 2006
##
## CvEventManager
## This class is passed an argsList from CvAppInterface.onEvent
## The argsList can contain anything from mouse location to key info
## The EVENTLIST that are being notified can be found


from CvPythonExtensions import *
import CvUtil
import TextUtil
import CvScreensInterface
import CvAdvisorUtils
import DebugUtils
import SdToolKit as SDTK

import WBPlayerScreen
import WBPlotScreen

# globals
GC = CyGlobalContext()
GAME = GC.getGame()
TRNSLTR = CyTranslator()

class CvEventManager:
	def __init__(self):
		self.bNotReady = True
		self.bNetworkMP = False
		self.bCtrl = False
		self.bShift = False
		self.bAlt = False
		self.bAllowCheats = False
		self.aWonderTuple = [[],[],[],[],[]]

		# OnEvent Enums
		# EventLButtonDown = 1
		# EventLcButtonDblClick = 2
		# EventRButtonDown = 3
		# EventBack = 4
		# EventForward = 5
		# EventKeyDown = 6
		# EventKeyUp = 7

		## EVENT MAP
		self.EventHandlerMap = {
			'mouseEvent'				: self.onMouseEvent,
			'kbdEvent'					: self.onKbdEvent,
			'ModNetMessage'				: self.onModNetMessage,
			'Init'						: self.onInit,
#			'UnInit'					: self.onUnInit,
#			'OnSave'					: self.onSaveGame,
#			'OnPreSave'					: self.onPreSave,
			'OnLoad'					: self.onLoadGame,
			'GameStart'					: self.onGameStart,
#			'GameEnd'					: self.onGameEnd,
			'MapRegen'					: self.onMapRegen,
#			'plotRevealed'				: self.onPlotRevealed,
#			'plotFeatureRemoved'		: self.onPlotFeatureRemoved,
#			'plotPicked'				: self.onPlotPicked,
			'nukeExplosion'				: self.onNukeExplosion,
			'gotoPlotSet'				: self.onGotoPlotSet,
			'BeginGameTurn'				: self.onBeginGameTurn,
#			'EndGameTurn'				: self.onEndGameTurn,
			'BeginPlayerTurn'			: self.onBeginPlayerTurn,
			'EndPlayerTurn'				: self.onEndPlayerTurn,
#			'endTurnReady'				: self.onEndTurnReady,
			'combatResult'				: self.onCombatResult,
			'combatLogCalc'				: self.onCombatLogCalc,
			'combatLogHit'				: self.onCombatLogHit,
			'improvementBuilt'			: self.onImprovementBuilt,
#			'improvementDestroyed'		: self.onImprovementDestroyed,
#			'routeBuilt'				: self.onRouteBuilt,
#			'firstContact'				: self.onFirstContact,
			'cityBuilt'					: self.onCityBuilt,
			'cityAcquired' 				: self.onCityAcquired,
			'cityAcquiredAndKept'		: self.onCityAcquiredAndKept,
			'cityRazed'					: self.onCityRazed,
			'cityLost'					: self.onCityLost,
#			'cultureExpansion'			: self.onCultureExpansion,
			'cityDoTurn'				: self.onCityDoTurn,
#			'cityBuildingUnit'			: self.onCityBuildingUnit,
#			'cityBuildingBuilding'		: self.onCityBuildingBuilding,
#			'cityBuildingProcess'		: self.onCityBuildingProcess,
			'cityRename'				: self.onCityRename,
#			'cityHurry'					: self.onCityHurry,
#			'selectionGroupPushMission'	: self.onSelectionGroupPushMission,
			'unitCreated'				: self.onUnitCreated,
			'unitBuilt'					: self.onUnitBuilt,
			'unitKilled'				: self.onUnitKilled,
			'unitPromoted'				: self.onUnitPromoted,
			'unitUpgraded'				: self.onUnitUpgraded,
			'UnitRename'				: self.onUnitRename,
#			'unitPillage'				: self.onUnitPillage,
			'unitSpreadReligionAttempt'	: self.onUnitSpreadReligionAttempt,
#			'unitGifted'				: self.onUnitGifted,
#			'unitBuildImprovement'		: self.onUnitBuildImprovement,
#			'goodyReceived'				: self.onGoodyReceived,
			'greatPersonBorn'			: self.onGreatPersonBorn,
			'buildingBuilt'				: self.onBuildingBuilt,
			'projectBuilt'				: self.onProjectBuilt,
			'techAcquired'				: self.onTechAcquired,
#			'techSelected'				: self.onTechSelected,
			'religionFounded'			: self.onReligionFounded,
#			'religionSpread'			: self.onReligionSpread,
#			'religionRemove'			: self.onReligionRemove,
#			'corporationFounded'		: self.onCorporationFounded,
#			'corporationSpread'			: self.onCorporationSpread,
#			'corporationRemove'			: self.onCorporationRemove,
#			'goldenAge'					: self.onGoldenAge,
#			'endGoldenAge'				: self.onEndGoldenAge,
#			'chat' 						: self.onChat,
			'victory'					: self.onVictory,
			'vassalState'				: self.onVassalState,
			'changeWar'					: self.onChangeWar,
#			'setPlayerAlive'			: self.onSetPlayerAlive,
#			'playerChangeStateReligion'	: self.onPlayerChangeStateReligion,
#			'playerGoldTrade'			: self.onPlayerGoldTrade,
			'windowActivation'			: self.onWindowActivation,
			'gameUpdate'				: self.onGameUpdate
		}
		################## Events List ###############################
		# Dictionary of Events, indexed by EventID (also used at popup context id)
		#	entries have name, beginFunction, applyFunction [, randomization weight...]
		#
		# Enum values less than 1000 are reserved for dll popup events
		# Enum values greater than 9999 are reserved for CvUtil.getNewEventID().
		# Enums 9998-9999 are reserved for worldbuilder
		#
		self.OverrideEventApply = {}
		import CvMainInterface
		self.Events = {
			1000 : ('EffectViewer', DebugUtils.applyEffectViewer, None),
			1001 : ('ShowWonder', DebugUtils.applyWonderMovie, None),
			1002 : ('AwardTechsAndGold', DebugUtils.applyTechCheat, None),
			1003 : ('EditCity', DebugUtils.applyEditCity, None),
			1050 : ('PlaceObject', DebugUtils.debugUtils.applyUnitPicker, None),

			4999 : ('CityTabOptions', CvMainInterface.applyCityTabOptions, None),
			5000 : ('EditCityName', self.__eventEditCityNameApply, self.__eventEditCityNameBegin),
			5006 : ('EditUnitName', self.__eventEditUnitNameApply, self.__eventEditUnitNameBegin),
			5009 : ('WBLandmarkPopup', self.__eventWBLandmarkPopupApply, None),

			1111 : ('WBPlayerScript', self.__eventWBPlayerScriptPopupApply, None),
			2222 : ('WBCityScript', self.__eventWBCityScriptPopupApply, None),
			3333 : ('WBUnitScript', self.__eventWBUnitScriptPopupApply, None),
			5555 : ('WBPlotScript', self.__eventWBPlotScriptPopupApply, None),
		}
	###****************###
	### EVENT STARTERS ###
	''' Overwritten by the one in BugEventManager.py
	def handleEvent(self, argsList):
		# extract the last 6 args in the list, the first arg has already been consumed
		tag = argsList[0]				# event type string
		idx = len(argsList)-6
		self.bDbg, bDummy, self.bAlt, self.bCtrl, self.bShift, self.bAllowCheats = argsList[idx:]
		ret = 0
		if self.EventHandlerMap.has_key(tag):
			fxn = self.EventHandlerMap[tag]
			ret = fxn(argsList[1:idx])
		return ret
	'''

	###*************###
	### EVENT APPLY ###
	def beginEvent(self, iD, argsList = -1):
		entry = self.Events[iD]
		if entry:
			if entry[-1] is None:
				print "[WARNING]CvEventManager.beginEvent\n\tEvent '%s' with ID '%d' does not have a generic begin function" + str(iD)

			else:
				if DebugUtils.bDebugMode:
					print "Begin event " + entry[0]
				entry[2](argsList)
		else:
			print "[WARNING]CvEventManager.beginEvent\n\tUnknown event ID: " + str(iD)

	def applyEvent(self, argsList):
		iD, iPlayer, netUserData, popupReturn = argsList
		entry = self.Events[iD]

		if entry and entry[1]:
			if DebugUtils.bDebugMode:
				print "Apply event " + entry[0]
			if iD in self.OverrideEventApply:
				return self.OverrideEventApply[iD](iPlayer, netUserData, popupReturn)

			return entry[1](iPlayer, netUserData, popupReturn)
		elif DebugUtils.bDebugMode:
			print "EventApply - Unknown event ID " + str(iD)

	###***********###
	### ON EVENTS ###

	def onInit(self, argsList):
		print "CvEventManager.onInit - initialize python"
		import SystemPaths
		dir = SystemPaths.init()
		import ScreenResolution
		ScreenResolution.init(dir)
		print "CvEventManager.onInit - END"

	#def onUnInit(self, argsList): return


	def onWindowActivation(self, argsList):
		# Alt+TAB
		bActive = argsList[0]
		if bActive:
			if self.bNotReady:
				CvScreensInterface.lateInit()
				import BugCore
				self.RoMOpt = BugCore.game.RoMSettings
				# Cache game constants.
				self.MAX_PLAYERS	= GC.getMAX_PLAYERS()
				self.MAX_PC_PLAYERS = GC.getMAX_PC_PLAYERS()
				self.iWAR_PRIZES	= GC.getDefineINT("WAR_PRIZES")
				self.iArcologyCityID = -1
				self.iOldCityID = -1
				self.mapBuildingType = {
					"ZIZKOV"				: GC.getInfoTypeForString("BUILDING_ZIZKOV_TELEVISION_TOWER"),
					"HELSINKI"				: GC.getInfoTypeForString("BUILDING_HELSINKI_CATHEDRAL"),
					"TAIPEI_101"			: GC.getInfoTypeForString('BUILDING_TAIPEI_101'),
					"LOTUS_TEMPLE"			: GC.getInfoTypeForString('BUILDING_LOTUS_TEMPLE'),
					"NANITE_DEFUSER"		: GC.getInfoTypeForString("BUILDING_NANITE_DEFUSER"),
					"MARCO_POLO"			: GC.getInfoTypeForString('BUILDING_MARCO_POLOS_EMBASSY'),
					"APPIAN_WAY"			: GC.getInfoTypeForString('BUILDING_VIA_APPIA'),
					"GOLDEN_SPIKE"			: GC.getInfoTypeForString('BUILDING_GOLDEN_SPIKE'),
					"FIELD_GOLD"			: GC.getInfoTypeForString('BUILDING_FIELD_OF_THE_CLOTH_OF_GOLD'),
					"MACHU_PICCHU"			: GC.getInfoTypeForString("BUILDING_MACHU_PICCHU"),
					"MAGINOTLINE"			: GC.getInfoTypeForString('BUILDING_MAGINOT_LINE'),
					"ROUTE_66"				: GC.getInfoTypeForString('BUILDING_ROUTE_66'),
					"SILK_ROAD"				: GC.getInfoTypeForString('BUILDING_SILK_ROAD'),
					"CLEOPATRA_NEEDLE"		: GC.getInfoTypeForString("BUILDING_CLEOPATRAS_NEEDLE"),
					"FA_MEN_SI"				: GC.getInfoTypeForString("BUILDING_FAMEN_TEMPLE"),
					"LASCAUX"				: GC.getInfoTypeForString("BUILDING_LASCAUX_PAINTS"),
					"ARCOLOGY"				: GC.getInfoTypeForString("BUILDING_ARCOLOGY"),
					"ARCOLOGY_SHIELDING"	: GC.getInfoTypeForString("BUILDING_ARCOLOGY_SHIELDING"),
					"ADVANCED_SHIELDING"	: GC.getInfoTypeForString("BUILDING_ADVANCED_SHIELDING"),
					"GREAT_ZIMBABWE"		: GC.getInfoTypeForString("BUILDING_GREAT_ZIMBABWE"),
					"CRUSADE"				: GC.getInfoTypeForString("BUILDING_KINGS_CRUSADE"),
					"ALAMO"					: GC.getInfoTypeForString("BUILDING_ALAMO"),
					"WORLD_BANK"			: GC.getInfoTypeForString("BUILDING_WORLD_BANK"),
					"CYRUS_CYLINDER"		: GC.getInfoTypeForString("BUILDING_CYRUS_CYLINDER"),
					"CYRUS_TOMB"			: GC.getInfoTypeForString("BUILDING_TOMB_OF_CYRUS"),
					"WEMBLEY"				: GC.getInfoTypeForString("BUILDING_WEMBLEY_STADIUM"),
					"PERGAMON"				: GC.getInfoTypeForString("BUILDING_PERGAMON_ALTAR"),
					"TSUKIJI"				: GC.getInfoTypeForString("BUILDING_TSUKIJI_FISH_MARKET"),
					"BIODOME"				: GC.getInfoTypeForString("BUILDING_MONTREAL_BIODOME"),
					"NAZCA_LINES"			: GC.getInfoTypeForString("BUILDING_NAZCA_LINES"),
					"THE_MOTHERLAND_CALLS"	: GC.getInfoTypeForString("BUILDING_THE_MOTHERLAND_CALLS"),
					"ISHTAR"				: GC.getInfoTypeForString('BUILDING_THE_ISHTAR_GATE'),
					"GREAT_JAGUAR_TEMPLE"	: GC.getInfoTypeForString('BUILDING_TEMPLE_OF_AH_CACAO'),
					"TOPKAPI_PALACE"		: GC.getInfoTypeForString("BUILDING_TOPKAPI_PALACE"),
					"NEANDERTHAL_EMBASSY"	: GC.getInfoTypeForString("BUILDING_CULTURE_NEANDERTHAL")
				}
				self.mapUnitType = {
					"MONK"		: GC.getInfoTypeForString("UNIT_MONK"),
					"FANATIC"	: GC.getInfoTypeForString("UNIT_FANATIC"),
					"SPARTACUS"	: GC.getInfoTypeForString("UNIT_SPARTACUS"),
					"GLADIATOR"	: GC.getInfoTypeForString("UNIT_GLADIATOR")
				}
				self.mapImpType = {
					"IMPROVEMENT_GROW_FOREST"	: GC.getInfoTypeForString('IMPROVEMENT_GROW_FOREST'),
					"IMPROVEMENT_YOUNG_FOREST"	: GC.getInfoTypeForString('IMPROVEMENT_YOUNG_FOREST'),
					"IMPROVEMENT_PLANT_BAMBOO"	: GC.getInfoTypeForString('IMPROVEMENT_PLANT_BAMBOO'),
					"IMPROVEMENT_PLANT_SAVANNA"	: GC.getInfoTypeForString('IMPROVEMENT_PLANT_SAVANNA'),
					"IMPROVEMENT_FARM"			: GC.getInfoTypeForString('IMPROVEMENT_FARM')
				}
				self.mapPromoType = {
					"PROMOTION_LIVE1"				: GC.getInfoTypeForString('PROMOTION_LIVE1'),
					"PROMOTION_LIVE2"				: GC.getInfoTypeForString('PROMOTION_LIVE2'),
					"PROMOTION_HUNTER1"				: GC.getInfoTypeForString('PROMOTION_HUNTER1'),
					"PROMOTION_HUNTER2"				: GC.getInfoTypeForString('PROMOTION_HUNTER2'),
					"PROMOTION_HUNTER3"				: GC.getInfoTypeForString('PROMOTION_HUNTER3'),
					"PROMOTION_HUNTER_GREAT"		: GC.getInfoTypeForString('PROMOTION_HUNTER_GREAT'),
					"PROMOTION_SEA_HUNTER1"			: GC.getInfoTypeForString('PROMOTION_SEA_HUNTER1'),
					"PROMOTION_SEA_HUNTER2"			: GC.getInfoTypeForString('PROMOTION_SEA_HUNTER2'),
					"PROMOTION_SEA_HUNTER3"			: GC.getInfoTypeForString('PROMOTION_SEA_HUNTER3'),
					"PROMOTION_SEA_HUNTER_GREAT"	: GC.getInfoTypeForString('PROMOTION_SEA_HUNTER_GREAT'),
					"PROMOTION_SNEAK"				: GC.getInfoTypeForString('PROMOTION_SNEAK'),
					"PROMOTION_MARAUDER"			: GC.getInfoTypeForString('PROMOTION_MARAUDER'),
					"PROMOTION_INDUSTRYESPIONAGE"	: GC.getInfoTypeForString('PROMOTION_INDUSTRYESPIONAGE'),
					"PROMOTION_RETINUE_MESSENGER"	: GC.getInfoTypeForString('PROMOTION_RETINUE_MESSENGER')
				}
				self.mapDomain = {
					"DOMAIN_LAND"		: GC.getInfoTypeForString('DOMAIN_LAND'),
					"DOMAIN_SEA"		: GC.getInfoTypeForString('DOMAIN_SEA')
				}
				# onUnitCreated
				self.TECH_SATELLITES	= GC.getInfoTypeForString("TECH_SATELLITES")
				self.TECH_STARGAZING	= GC.getInfoTypeForString("TECH_STARGAZING")
				self.TECH_ASTROLOGY		= GC.getInfoTypeForString("TECH_ASTROLOGY")
				self.TECH_ASTRONOMY		= GC.getInfoTypeForString("TECH_ASTRONOMY")
				self.TECH_REALISM		= GC.getInfoTypeForString("TECH_REALISM")
				# onUnitBuilt
				self.mapSettlerPop = {
					GC.getInfoTypeForString("UNIT_COLONIST")	: 1,
					GC.getInfoTypeForString("UNIT_PIONEER")		: 2,
					GC.getInfoTypeForString("UNIT_AIRSETTLER")	: 4
				}
				self.PROMOTION_SENSORS	= GC.getInfoTypeForString('PROMOTION_SENSORS')
				self.UNIT_IMMIGRANT		= GC.getInfoTypeForString("UNIT_IMMIGRANT")
				self.TECH_SMART_DUST	= GC.getInfoTypeForString("TECH_SMART_DUST")
				self.CIVICOPTION_POWER	= GC.getInfoTypeForString('CIVICOPTION_POWER')
				self.CIVIC_TECHNOCRACY	= GC.getInfoTypeForString('CIVIC_TECHNOCRACY')
				# onCityBuilt
				self.aCultureList = [
					[GC.getInfoTypeForString('PROMOTION_CULTURE_AFRICAN'), GC.getInfoTypeForString('BUILDING_C_L_AFRICAN'), GC.getInfoTypeForString('BUILDING_C_N_AFRICAN')],
					[GC.getInfoTypeForString('PROMOTION_CULTURE_ASIAN'), GC.getInfoTypeForString('BUILDING_C_L_ASIAN'), GC.getInfoTypeForString('BUILDING_C_N_ASIAN')],
					[GC.getInfoTypeForString('PROMOTION_CULTURE_EUROPEAN'), GC.getInfoTypeForString('BUILDING_C_L_EUROPEAN'), GC.getInfoTypeForString('BUILDING_C_N_EUROPEAN')],
					[GC.getInfoTypeForString('PROMOTION_CULTURE_MIDDLE_EASTERN'), GC.getInfoTypeForString('BUILDING_C_L_MIDDLE_EASTERN'), GC.getInfoTypeForString('BUILDING_C_N_MIDDLE_EASTERN')],
					[GC.getInfoTypeForString('PROMOTION_CULTURE_NEANDERTHAL'), GC.getInfoTypeForString('BUILDING_C_L_NEANDERTHAL'), GC.getInfoTypeForString('BUILDING_C_N_NEANDERTHAL')],
					[GC.getInfoTypeForString('PROMOTION_CULTURE_NORTH_AMERICAN'), GC.getInfoTypeForString('BUILDING_C_L_NORTH_AMERICAN'), GC.getInfoTypeForString('BUILDING_C_N_NORTH_AMERICAN')],
					[GC.getInfoTypeForString('PROMOTION_CULTURE_OCEANIAN'), GC.getInfoTypeForString('BUILDING_C_L_OCEANIAN'), GC.getInfoTypeForString('BUILDING_C_N_OCEANIAN')],
					[GC.getInfoTypeForString('PROMOTION_CULTURE_SOUTH_AMERICAN'), GC.getInfoTypeForString('BUILDING_C_L_SOUTH_AMERICAN'), GC.getInfoTypeForString('BUILDING_C_N_SOUTH_AMERICAN')]
				]
				self.UNIT_BAND = GC.getInfoTypeForString("UNIT_BAND")

				self.UNITCOMBAT_RECON		= GC.getInfoTypeForString("UNITCOMBAT_RECON")
				self.UNITCOMBAT_HUNTER		= GC.getInfoTypeForString("UNITCOMBAT_HUNTER")
				self.UNITCOMBAT_CIVILIAN	= GC.getInfoTypeForString("UNITCOMBAT_CIVILIAN")

				# onCityBuilt
				self.PROMO_GUARDIAN_TRIBAL	= GC.getInfoTypeForString("PROMOTION_GUARDIAN_TRIBAL")
				# onTechAcquired
				self.TECH_GATHERING = GC.getInfoTypeForString("TECH_GATHERING")
				# Subdued/Tamed animal graphical attachment
				self.UNIT_STORY_TELLER			= GC.getInfoTypeForString("UNIT_STORY_TELLER")
				# Biodome
				self.aBiodomeList = aList = []
				for iUnit in xrange(GC.getNumUnitInfos()):
					if GC.getUnitInfo(iUnit).getType().find("UNIT_SUBDUED") > -1:
						aList.append(iUnit)
				# Militia
				self.CIVIC_CONSCRIPTION = GC.getInfoTypeForString("CIVIC_CONSCRIPTION")
				iMedieval = GC.getInfoTypeForString('UNIT_MILITIA_MEDIEVAL')
				iModern = GC.getInfoTypeForString('UNIT_MILITIA_MODERN')
				self.mapMilitiaByEra = {
					GC.getInfoTypeForString('C2C_ERA_CLASSICAL')	: iMedieval,
					GC.getInfoTypeForString('C2C_ERA_MEDIEVAL')		: iMedieval,
					GC.getInfoTypeForString('C2C_ERA_RENAISSANCE')	: GC.getInfoTypeForString('UNIT_MILITIA_RENAISSANCE'),
					GC.getInfoTypeForString('C2C_ERA_INDUSTRIAL')	: GC.getInfoTypeForString('UNIT_MILITIA_INDUSTRIAL'),
					GC.getInfoTypeForString('C2C_ERA_ATOMIC')		: iModern,
					GC.getInfoTypeForString('C2C_ERA_INFORMATION')	: iModern
				}

				self.bNotReady = False # Only needs to be done once.


	def onMouseEvent(self, argsList):
		'mouse handler - returns 1 if the event was consumed'
		eventType, mx, my, px, py, interfaceConsumed, screens = argsList

		if not interfaceConsumed and not self.bNetworkMP and (self.bAllowCheats or DebugUtils.bDebugMode):
			if eventType == 1 and px != -1 and py != -1 and self.bCtrl:
				if self.bAlt and GC.getMap().plot(px, py).isCity():
					# Launch Edit City Event
					DebugUtils.initEditCity(px, py)
					return 1
				elif self.bShift:
					# Launch Place Object Event
					DebugUtils.debugUtils.initUnitPicker(px, py)
					return 1

		if eventType == 4:
			CvScreensInterface.handleBack(screens)
		elif eventType == 5:
			CvScreensInterface.handleForward(screens)
		return 0


	def onKbdEvent(self, argsList):
		'keypress handler - return 1 if the event was consumed'
		eventType, key, mx, my, px, py = argsList

		# Screen specific input handlers
		iCode = eventType + 10
		if (
			iCode in (16, 17)
		and
			CvScreensInterface.handleInput([iCode, key, 0, 0, CvScreensInterface.g_iScreenActive, "", 0, 0, 0, px, py, 35, 0, 0, 0])
		):
			return 1

		bAlt = self.bAlt
		bCtrl = self.bCtrl
		bShift = self.bShift
		iModifiers = bAlt + bCtrl + bShift

		if eventType == 7: # Key up

			if iModifiers == 3:

				# key up is more reliably reported than key down for some keys when three modifiers are pressed
				# key down event for the 'D' in 'ctrl+shift+alt+D' seems to be consumed by the exe in some cases
				if key == 16: # D
					DebugUtils.toggleDebugMode()
					CvScreensInterface.mainInterface.pythonDebugToggle(DebugUtils.bDebugMode)
					return 1

		elif eventType == 6: # Key down

			if iModifiers == 1:
				if bCtrl:
					if key == InputTypes.KB_F1:
						CvScreensInterface.showForgetfulScreen()
						return 1
					if key == InputTypes.KB_F2:
						import GameFontScreen
						GameFontScreen.GameFontScreen()
						return 1
					if key == InputTypes.KB_F3:
						import TimeKeeper
						TimeKeeper.TimeKeeper()
						return 1

				if DebugUtils.bDebugMode:
					if bCtrl:
						if key == InputTypes.KB_F4:
							CvScreensInterface.replayScreen.showScreen(False)
							return 1
						if key == InputTypes.KB_F5:
							CvScreensInterface.showDebugInfoScreen()
							return 1
						if key == InputTypes.KB_F6:
							CvScreensInterface.showDanQuayleScreen(())
							return 1
						if key == InputTypes.KB_F7:
							CvScreensInterface.showUnVictoryScreen(())
							return 1

					elif bShift:
						if key == InputTypes.KB_T:
							DebugUtils.initTechsCheat()
							return 1
						elif key == InputTypes.KB_W:
							DebugUtils.initWonderMovie()
							return 1
						elif key == InputTypes.KB_Z:
							bNewState = not GAME.isDebugMode()
							CyInterface().addImmediateMessage("Dll Debug Mode: " + str(bNewState), "AS2D_GOODY_MAP")
							GAME.toggleDebugMode()
							CvScreensInterface.mainInterface.bDebugMode = bNewState
							return 1
						elif key == InputTypes.KB_E:
							DebugUtils.initEffectViewer(px, py)
							return 1

			elif iModifiers == 2:

				if DebugUtils.bDebugMode:

					if bShift and bCtrl:

						if key == InputTypes.KB_P:
							import ChangePlayer
							ChangePlayer.changeCivPopup()
							return 1

						elif key == InputTypes.KB_L:
							import ChangePlayer
							ChangePlayer.changeHumanPopup()
							return 1

						elif key == InputTypes.KB_U:
							import ChangePlayer
							ChangePlayer.updateGraphics()
							return 1

						elif key == InputTypes.KB_E:
							import EventTriggerScreen, CvScreenEnums
							CvScreensInterface.screenMap[CvScreenEnums.EVENTTRIGGER_SCREEN] = EventTriggerScreen.EventTriggerScreen(CvScreenEnums.EVENTTRIGGER_SCREEN)

			elif iModifiers == 3:

				if DebugUtils.bDebugMode:
					if key == InputTypes.KB_U:
						DebugUtils.putOneOfEveryUnit()
						return 1
		return 0


	# Called globally as a result of a local CyMessageControl().sendModNetMessage() call - Avoids OOS errors in MP games.
	def onModNetMessage(self, argsList):
		ID, iData2, iData3, iData4, iData5 = argsList

		# Kill Deal
		if ID == 666:
			if iData2 > -1:
				if GAME.getActivePlayer() != iData2:
					GAME.getDeal(iData3).kill()
			else:
				GAME.getDeal(iData3).kill()

		# City Demolish
		elif ID in (901, 902, 903, 904, 905, 906):
			iPlayer = iData2
			if ID == 901:
				# DiploPenalty
				if iData4:
					GC.getPlayer(iData3).AI_changeMemoryCount(iPlayer, MemoryTypes.MEMORY_RAZED_CITY, iData5)
				else:
					GC.getPlayer(iData3).AI_changeMemoryCount(iPlayer, MemoryTypes.MEMORY_RAZED_HOLY_CITY, iData5)
			elif ID == 902:
				# Kill Guardian
				GC.getPlayer(iPlayer).getUnit(iData3).kill(False, -1)
			else:
				CyPlayer = GC.getPlayer(iPlayer)
				CyCity = CyPlayer.getCity(iData3)
				# 903 Demolish Building || 904 Abandon City || 905/906 Abandon->Units
				if ID == 903:
					CyCity.changeHasBuilding(iData4, False)
					CyPlayer.changeGold(iData5)
					CvScreensInterface.mainInterface.buildCityListLeft()
					CyInterface().setDirty(InterfaceDirtyBits.SelectionButtons_DIRTY_BIT, True)
					if GC.getBuildingInfo(iData4).getReligionType() >= 0:
						CyCity.changeHurryAngerTimer(CyCity.flatHurryAngerLength())
				elif ID == 904:
					CyPlayer.changeGold(iData5)
					self.onCityRazed((CyCity, iPlayer))
					CyCity.kill()
				elif ID == 905 or ID == 906:
					X = CyCity.getX()
					Y = CyCity.getY()
					CyUnit = CyPlayer.initUnit(iData5, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
					if iData4 == -1:
						CyCity.addProductionExperience(CyUnit, True)
					else:
						if iData4 > 0:
							CyUnit.setExperience(iData4)
						if ID == 906 and self.PROMO_GUARDIAN_TRIBAL > -1:
							CyUnit.setHasPromotion(self.PROMO_GUARDIAN_TRIBAL, True)
					CyUnit.setMoves(CyUnit.maxMoves()-1)

		elif ID == 999:
			GC.getPlayer(iData2).changeCommercePercent(CommerceTypes(iData3), iData4)


	def gameStart(self, bNewGame):
		CvScreensInterface.mainInterface.bSetStartZoom = True

		############################
		# Cache game specific data #
		if GAME.isNetworkMultiPlayer():
			self.bNetworkMP = True
		else:
			self.bNetworkMP = False

		self.iTurnTopCiv = GAME.getGameTurn()
		self.iTurnsToTopCiv = 49 - (self.iTurnTopCiv % 50)

		CvAdvisorUtils.resetNoLiberateCities()

		self.GO_1_CITY_TILE_FOUNDING	= GAME.isOption(GameOptionTypes.GAMEOPTION_CULTURE_1_CITY_TILE_FOUNDING)
		self.GO_START_AS_MINORS			= GAME.isOption(GameOptionTypes.GAMEOPTION_UNSUPPORTED_START_AS_MINORS)
		self.GO_NO_CITY_RAZING			= GAME.isOption(GameOptionTypes.GAMEOPTION_NO_CITY_RAZING)
		self.GO_ONE_CITY_CHALLENGE		= GAME.isOption(GameOptionTypes.GAMEOPTION_CHALLENGE_ONE_CITY)

		if bNewGame and self.GO_START_AS_MINORS:
			for iTeam in xrange(GC.getMAX_PC_TEAMS()):
				CyTeam = GC.getTeam(iTeam)
				if CyTeam.isAlive() and not CyTeam.isOpenBordersTrading():
					CyTeam.setIsMinorCiv(True)

		CvGameSpeedInfo = GC.getGameSpeedInfo(GAME.getGameSpeedType())
		self.iTrainPrcntGS = CvGameSpeedInfo.getHammerCostPercent()
		self.iGameSpeedPercent = CvGameSpeedInfo.getSpeedPercent()
		# Find special buildings built where by whom.
		mapBuildingType = self.mapBuildingType
		aList0 = [ # Only meant for world wonders
			"CRUSADE",				"GREAT_ZIMBABWE",	"HELSINKI",		"ALAMO",
			"LASCAUX",				"WORLD_BANK",		"TAIPEI_101",	"CYRUS_CYLINDER",
			"FA_MEN_SI",			"WEMBLEY",			"PERGAMON",		"CYRUS_TOMB",
			"TSUKIJI", 				"BIODOME",			"NAZCA_LINES",	"THE_MOTHERLAND_CALLS",
			"GREAT_JAGUAR_TEMPLE",	"TOPKAPI_PALACE",
		] # KEY
		aList1 = [] # iBuilding
		aList2 = [] # iTech (Obsolete)
		i = 0
		while aList0[i:]:
			KEY = aList0[i]
			iBuilding = mapBuildingType[KEY]
			if iBuilding > -1:
				aList1.append(iBuilding)
				aList2.append(GC.getBuildingInfo(iBuilding).getObsoleteTech())
				i += 1
			else:
				print "\nWarning, CvEventManager expected a building with this KEY end [%s] to exist.\n" %KEY
				del aList0[i]

		aList3 = [-2] * len(aList0) # iCityID
		aList4 = list(aList3) # iOwner
		for iPlayer in xrange(self.MAX_PLAYERS):
			CyPlayer = GC.getPlayer(iPlayer)
			temp = list(aList4)
			for k, entry in enumerate(temp):
				if entry != -2:
					continue
				iBuilding = aList1[k]
				if CyPlayer.hasBuilding(iBuilding):
					aList4[k] = iPlayer
					for CyCity in CyPlayer.cities():
						if CyCity.hasBuilding(iBuilding):
							aList3[k] = CyCity.getID()
							break
		# Get rid of wonders that is lost when a city is razed.
		# This can save memory and increase performance, a little.
		n = 0
		temp = list(aList4)
		for i, entry in enumerate(temp):
			if entry == -2:
				idx = i - n
				if GAME.isBuildingMaxedOut(aList1[idx], 0): # Has it ever been built?
					# Obsolete or otherwise destroyed.
					del aList0[idx], aList1[idx], aList2[idx], aList3[idx], aList4[idx]
					n += 1
		# Store the values.
		self.aWonderTuple = [aList0, aList1, aList2, aList3, aList4]
		# [0][X] = KEY		[1][X] = iBuilding		[2][X] = iTech (Obsolete)		[3][X] = iCityID		[4][X] = iOwner
		''' X:
		[0]  Crusade			[1]  Great Zimbabwe			[2]  Helsinki				[3]  Alamo
		[4]  Lascaux			[5]  World Bank				[6]  Taipei 101				[7]  Cyrus Cylinder		etc.
		'''
		GAME.onFinalInitialized(bNewGame)


	def onGameStart(self, argsList):
		self.gameStart(True)
		# Called when a game is created the moment you can see the map.
		if GAME.getGameTurn() == GAME.getStartTurn():

			self.doDawnOfMan()

			if not CyInterface().isInAdvancedStart():
				self.freePromotions()
		else:
			CyInterface().setSoundSelectionReady(True)



	def freePromotions(self):
		for iPlayer in xrange(self.MAX_PC_PLAYERS):
			CyPlayer = GC.getPlayer(iPlayer)
			if not CyPlayer.isAlive(): continue

			civInfo = GC.getCivilizationInfo(CyPlayer.getCivilizationType())
			aList = []
			for iPromo, _, native in self.aCultureList:
				if -1 in (iPromo, native): continue
				if civInfo.isCivilizationBuilding(native):
					aList.append(iPromo)
			if not aList: continue

			for CyUnit in CyPlayer.units():
				if CyUnit.isFound():
					for iPromo in aList:
						CyUnit.setHasPromotion(iPromo, True)

	def onMapRegen(self, argsList):
		if not CyInterface().isInAdvancedStart():
			self.freePromotions()


	def onLoadGame(self, argsList):
		if GAME.getGameTurn() == GAME.getStartTurn():
			self.doDawnOfMan()

			if not CyInterface().isInAdvancedStart():
				self.freePromotions()

		self.gameStart(False)

	def doDawnOfMan(self):
		if GAME.isHotSeat() or GAME.isPbem():
			for iPlayer in xrange(self.MAX_PC_PLAYERS):
				CyPlayer = GC.getPlayer(iPlayer)
				if CyPlayer.isHuman() and CyPlayer.isAlive():
					popup = CyPopupInfo()
					popup.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
					popup.setText("showDawnOfMan")
					popup.addPopup(iPlayer)
			if GAME.isPbem():
				popup = CyPopupInfo()
				popup.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_DETAILS)
				popup.setOption1(true)
				popup.addPopup(iPlayer)
		else:
			iPlayer = GAME.getActivePlayer()
			CyPlayer = GC.getPlayer(iPlayer)
			if CyPlayer.isHuman() and CyPlayer.isAlive():
				popup = CyPopupInfo()
				popup.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
				popup.setText('showDawnOfMan')
				popup.addPopup(iPlayer)
			szText = ""
			if GC.getDefineINT("START_YEAR") != GAME.getGameTurnYear():
				szText += "\n\n" + TRNSLTR.getText("TXT_KEY_MOD_GAMESTART_NOT_PREHISTORIC", ())
			if GAME.isOption(GameOptionTypes.GAMEOPTION_CORE_CUSTOM_START):
				szText += "\n\n" + TRNSLTR.getText("TXT_KEY_MOD_GAMESTART_ADVANCED_START", ())
			if szText:
				szText = TRNSLTR.getText("TXT_KEY_MOD_HEADER", ()) + szText
				popup = CyPopupInfo()
				popup.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_TEXT)
				popup.setText(szText)
				popup.addPopup(iPlayer)

	'''
	def onPreSave(self, argsList): return


	def onSaveGame(self, argsList):
		# return the string to be saved - Must be a string
		return ""


	def onGameEnd(self, argsList):
		print 'Called at the End of the game', ("argsList", argsList)


	def onEndGameTurn(self, argsList):
		# Called at the end of the "end turn" cycle.
		iGameTurn = argsList[0]
	'''


	def onBeginGameTurn(self, argsList):
		# Called at the beginning of the end of each turn
		iGameTurn, = argsList
		if not self.bNetworkMP and not GAME.getAIAutoPlay(GAME.getActivePlayer()):
			if iGameTurn - self.iTurnTopCiv >= self.iTurnsToTopCiv:
				CyPlayer = GC.getActivePlayer()
				if CyPlayer and CyPlayer.isAlive():
					self.iTurnTopCiv = iGameTurn
					self.iTurnsToTopCiv = 40 + GAME.getSorenRandNum(21, "40-60")
					CvScreensInterface.showTopCivs()


	def onBeginPlayerTurn(self, argsList):
		# Called when a player end its turn.
		# The turn is not really handed over to the next player before after onEndPlayerTurn has been called.
		iGameTurn, iPlayer = argsList
		CyPlayer = GC.getPlayer(iPlayer)
		# Building effects
		aWonderTuple = self.aWonderTuple
		if iPlayer in aWonderTuple[4]:
			for i, iPlayerX in enumerate(aWonderTuple[4]):
				if iPlayerX != iPlayer: continue
				KEY = aWonderTuple[0][i]
				if KEY == "WORLD_BANK":
					CyPlayer.changeGold(CyPlayer.getGold()//200)

				elif KEY == "CYRUS_CYLINDER":
					if not iGameTurn % (4*self.iGameSpeedPercent/100 + 1):
						iTeam = CyPlayer.getTeam()
						iDiv = iGameTurn * 2
						for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
							CyTeamX = GC.getTeam(iTeamX)
							if CyTeamX.isAlive() and CyTeamX.isVassal(iTeam):
								iDiv -= intSqrt(iDiv)

						CyPlayer.changeCombatExperience(GAME.getPlayerScore(iPlayer) / iDiv)
				elif KEY == "TOPKAPI_PALACE":
					iTeam = CyPlayer.getTeam()
					for iPlayerX in xrange(self.MAX_PC_PLAYERS):
						if iPlayerX == iPlayer: continue
						CyPlayerX = GC.getPlayer(iPlayerX)
						if CyPlayerX.isAlive() and GC.getTeam(CyPlayerX.getTeam()).isVassal(iTeam):
							iGold = (intSqrt(CyPlayerX.getGold())/2500)
							if iGold:
								CyPlayerX.changeGold(iGold)
							CyPlayerX.changeCombatExperience(1)

		# Aging Animals
		if not CyPlayer.isNPC() or CyPlayer.isHominid():
			return
		bMinor = not iGameTurn % (16 * self.iGameSpeedPercent / 100 + 1)
		bMajor = not iGameTurn % (128 * self.iGameSpeedPercent / 100)

		if bMinor or bMajor:
			for CyUnit in CyPlayer.units():
				if not CyUnit.isDead() and CyUnit.isAnimal():
					if not GAME.getSorenRandNum(15 - bMajor*10, "Aging"): # 1 in 15/5
						if not CyUnit.isHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT1")):
							CyUnit.setHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT1"), True)
						elif not CyUnit.isHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT2")):
							CyUnit.setHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT2"), True)
						elif not CyUnit.isHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT3")):
							CyUnit.setHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT3"), True)
						elif not CyUnit.isHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT4")):
							CyUnit.setHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT4"), True)
						elif not CyUnit.isHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT5")):
							CyUnit.setHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT5"), True)
						elif not CyUnit.isHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT6")):
							CyUnit.setHasPromotion(GC.getInfoTypeForString("PROMOTION_COMBAT6"), True)
						elif bMajor:
							CyUnit.setBaseCombatStr(CyUnit.baseCombatStr() + 1)
						else:
							CyUnit.setExperience(CyUnit.getExperience() + 3)


	def onEndPlayerTurn(self, argsList):
		# Called right before the turn is handed over to the next player.
		iGameTurn, iPlayer = argsList

		if iGameTurn:
			CvAdvisorUtils.endTurnFeats(iPlayer)

		elif iPlayer == GAME.getActivePlayer():
			popupInfo = CyPopupInfo()
			popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_CHANGECIVIC)
			popupInfo.addPopup(iPlayer)

		aWonderTuple = self.aWonderTuple
		if "WEMBLEY" in aWonderTuple[0]:
			i = aWonderTuple[0].index("WEMBLEY")
			if iPlayer == aWonderTuple[4][i]:
				CyPlayer = GC.getPlayer(iPlayer)
				ExcessHappy = CyPlayer.calculateTotalCityHappiness() - CyPlayer.calculateTotalCityUnhappiness()
				if ExcessHappy > 0:
					CyPlayer.changeGold(ExcessHappy * 50)



	'''
	def onEndTurnReady(self, argsList):
		iGameTurn = argsList[0]

	def onFirstContact(self, argsList):
		iTeamX, iHasMetTeamY = argsList
	'''


	def onCombatResult(self, argsList):
		CyUnitW, CyUnitL = argsList
		iPlayerW = CyUnitW.getOwner()
		CyPlayerW = GC.getPlayer(iPlayerW)
		iPlayerL = CyUnitL.getOwner()
		CyPlayerL = GC.getPlayer(iPlayerL)
		CyTeamW = None
		iPlayerAct = None
		# WarPrizes
		if self.iWAR_PRIZES:
			iValid = GC.getInfoTypeForString("UNITCOMBAT_NAVAL_COMBATANT")
			if iValid > -1 and CyUnitW.isHasUnitCombat(iValid) and CyUnitL.isHasUnitCombat(iValid):
				# 20% chance
				if GAME.getSorenRandNum(100, "WarPrize") < 20:

					iUnitL = CyUnitL.getUnitType()
					iX = CyUnitW.getX()
					iY = CyUnitW.getY()
					CyUnitWP = CyPlayerW.initUnit(iUnitL, iX, iY, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
					CyUnitWP.finishMoves()
					CyUnitWP.setDamage(GAME.getSorenRandNum(40, "XP") + 40, iPlayerW)
					if CyUnitW.isHiddenNationality():
						CyUnitWP.doHNCapture()
					# Message
					iPlayerAct = GAME.getActivePlayer()
					if iPlayerAct in (iPlayerW, iPlayerL):
						CvUnitInfoL = GC.getUnitInfo(iUnitL)
						szWP = CvUnitInfoL.getDescription()
					if iPlayerW == iPlayerAct:
						eColor = ColorTypes(GC.getCOLOR_GREEN())
						CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_MISC_WARPRIZES_SUCCESS", (szWP,)), iPlayerW, 16, CvUnitInfoL.getButton(), eColor, iX, iY, True, True, bForce=False)
					elif iPlayerL == iPlayerAct:
						iX = CyUnitL.getX()
						iY = CyUnitL.getY()
						eColor = ColorTypes(GC.getCOLOR_RED())
						artPath = 'Art/Interface/Buttons/General/warning_popup.dds'
						CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_MISC_WARPRIZES_FAILURE", (szWP,)), iPlayerL, 16, artPath, eColor, iX, iY, True, True, bForce=False)
					# Booty
					iValid = GC.getInfoTypeForString("UNITCOMBAT_TRANSPORT")
					if iValid > -1 and CyUnitL.isHasUnitCombat(iValid):
						Loot = GAME.getSorenRandNum(100, "Loot") * self.iTrainPrcntGS / 100
						if CyPlayerL.getGold() >= Loot:
							CyPlayerW.changeGold(Loot)
							CyPlayerL.changeGold(-Loot)
							if iPlayerW == iPlayerAct:
								CvUtil.sendMessage(
									TRNSLTR.getText("TXT_KEY_MISC_WARPRIZES_SUCCESS_GOLD_GAINED", (szWP,)), iPlayerW, 24,
									'Art/Interface/Buttons/process/processwealth.dds', eColor, iX, iY, True, True, bForce=False
								)
							elif iPlayerL == iPlayerAct:
								CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_MISC_WARPRIZES_FAILURE_GOLD_LOST", (szWP,)), iPlayerL, 24, artPath, eColor, iX, iY, True, True, bForce=False)
		# Respawn Promotion
		mapPromoType = self.mapPromoType
		iRespawn2 = mapPromoType['PROMOTION_LIVE2']
		iRespawn1 = mapPromoType['PROMOTION_LIVE1']
		bRespawn2 = CyUnitL.isHasPromotion(iRespawn2)

		# Messenger (Hero) promo
		if CyUnitW.isHasPromotion(mapPromoType['PROMOTION_RETINUE_MESSENGER']):
			# 10% chance
			if not GAME.getSorenRandNum(10, "Gods"):

				if not SDTK.sdObjectExists('Promo', CyUnitW):
					CyUnitW.setDamage(0, -1)
					SDTK.sdObjectInit('Promo', CyUnitW, {'HealTurn' : GAME.getGameTurn()})
				else:
					iHealTurn = SDTK.sdObjectGetVal('Promo', CyUnitW, 'HealTurn')
					iTurn = GAME.getGameTurn()
					if iHealTurn is None or iTurn > iHealTurn:
						CyUnitW.setDamage(0, -1)
						SDTK.sdObjectSetVal('Promo', CyUnitW, 'HealTurn', iTurn)

		# Respawn promo
		if CyUnitL.isHasPromotion(iRespawn1) or bRespawn2:
			iUnit = CyUnitL.getUnitType()
			iX = -1
			if CyUnitL.getDomainType() == self.mapDomain['DOMAIN_SEA']:
				for CyCity in CyPlayer.cities():
					if CyCity.isCoastal(0):
						iX = CyCity.getX()
						iY = CyCity.getY()
						break
			else:
				CyCity = CyPlayerL.getCapitalCity()
				if CyCity:
					iX = CyCity.getX()
					iY = CyCity.getY()
			if iX == -1:
				iX = CyUnitL.getX()
				iY = CyUnitL.getY()
			CyUnit = CyPlayerL.initUnit(iUnit, iX, iY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_NORTH)
			CyUnit.convert(CyUnitL, True)
			CyUnit.setDamage(GAME.getSorenRandNum(40, "Damage") + 20, -1)

			CyUnit.finishMoves()

			if bRespawn2:
				CyUnit.setHasPromotion(iRespawn2, False)
			else:
				CyUnit.setHasPromotion(iRespawn1, False)

			if iPlayerAct is None:
				iPlayerAct = GAME.getActivePlayer()
			if iPlayerL == iPlayerAct:
				CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_REBORN",()), iPlayerL, 16, 'Art/Interface/Buttons/Phoenix1.dds', ColorTypes(44), iX, iY, True, True, bForce=False)

		if CyUnitW.isNPC():
			# EXP boost for NPC
			CyUnitW.changeExperience(1, 100, False, False, False)

		elif CyUnitL.isNPC():
			# EXP boost for AI winning against NPC
			if not CyUnitW.isHuman():
				CyUnitW.changeExperience(1, 100, False, False, False)

		else: # Special outcomes not applicable to NPC teams
			bSneak = CyUnitW.isHasPromotion(mapPromoType['PROMOTION_SNEAK'])
			bMarauder = CyUnitW.isHasPromotion(mapPromoType['PROMOTION_MARAUDER'])
			bIndustrySpy = CyUnitW.isHasPromotion(mapPromoType['PROMOTION_INDUSTRYESPIONAGE'])
			# Commerce Theft
			if bSneak or bMarauder or bIndustrySpy:

				if bSneak or bIndustrySpy:
					iTeamW = CyPlayerW.getTeam()
					iTeamL = CyPlayerL.getTeam()
					CyTeamW = GC.getTeam(iTeamW)
					CyTeamL = GC.getTeam(iTeamL)

				# Factor in winners handicap versus losers handicap
				iHandicapFactor = 1000 * GC.getHandicapInfo(CyPlayerL.getHandicapType()).getCivicUpkeepPercent() / GC.getHandicapInfo(CyPlayerW.getHandicapType()).getCivicUpkeepPercent()

				# Message
				if iPlayerAct is None:
					iPlayerAct = GAME.getActivePlayer()
				if iPlayerAct in (iPlayerW, iPlayerL):
					iX = CyUnitW.getX()
					iY = CyUnitW.getY()
					if iPlayerW == iPlayerAct:
						if CyUnitL.isHiddenNationality():
							szTxt = TRNSLTR.getText("TXT_KEY_UNKNOWN_NATION",())
						else:
							szTxt = CyPlayerL.getName()
					elif iPlayerL == iPlayerAct:
						if CyUnitW.isHiddenNationality():
							szTxt = TRNSLTR.getText("TXT_KEY_UNKNOWN_NATION",())
						else:
							szTxt = CyPlayerW.getName()
				# Sneak promo
				if bSneak:
					iLoserPointsAgainstWinner = CyTeamL.getEspionagePointsAgainstTeam(iTeamW)
					iWinnerPointsAgainstLoser = CyTeamW.getEspionagePointsAgainstTeam(iTeamL)
					iBase = 5 * iLoserPointsAgainstWinner - iWinnerPointsAgainstLoser
					if iBase > 0:
						iStolen = intSqrt(iBase * iLoserPointsAgainstWinner / (1 + iWinnerPointsAgainstLoser)) * iHandicapFactor / 1000
						if iStolen > 0:
							CyTeamW.changeEspionagePointsAgainstTeam(iTeamL, iStolen)
							CyTeamL.changeEspionagePointsAgainstTeam(iTeamW,-iStolen)
							# Message
							if iPlayerW == iPlayerAct:
								CvUtil.sendMessage(
									TRNSLTR.getText("TXT_KEY_MSG_SNEAK_YOU", (szTxt, iStolen)), iPlayerW, 16,
									'Art/Interface/Buttons/Process/spyprocessmeager.dds',
									ColorTypes(44), iX, iY, True, True, bForce=False
								)
							elif iPlayerL == iPlayerAct:
								CvUtil.sendMessage(
									TRNSLTR.getText("TXT_KEY_MSG_SNEAK",(szTxt, iStolen)), iPlayerL, 16,
									'Art/Interface/Buttons/Process/spyprocessmeager.dds',
									ColorTypes(44), iX, iY, True, True, bForce=False
						)
				# Marauder promo
				if bMarauder:
					iStolen = CyPlayerL.getGold() / (CyPlayerL.getNumUnits() + 1)
					if iStolen > 0:
						iStolen = intSqrt(iStolen)
						iStolen *= iHandicapFactor
						iStolen /= 1000

					if iStolen > 0:
						CyPlayerL.changeGold(-iStolen)
						CyPlayerW.changeGold(iStolen)
						# Message
						if iPlayerW == iPlayerAct:
							CvUtil.sendMessage(
								TRNSLTR.getText("TXT_KEY_MARAUDER_YOU", (szTxt, iStolen)), iPlayerW, 16,
								'Art/Interface/Buttons/Process/processwealthmeagre.dds',
								ColorTypes(44), iX, iY, True, True, bForce=False
							)
						elif iPlayerL == iPlayerAct:
							CvUtil.sendMessage(
								TRNSLTR.getText("TXT_KEY_MARAUDER_FROM",(szTxt, iStolen)), iPlayerL, 16,
								'Art/Interface/Buttons/Process/processwealthmeagre.dds',
								ColorTypes(44), iX, iY, True, True, bForce=False
							)
				# Industry Espionage promo
				if bIndustrySpy:
					iTechW = CyPlayerW.getCurrentResearch()
					iTechL = CyPlayerL.getCurrentResearch()

					if CyTeamL.isHasTech(iTechW) or not CyTeamW.isHasTech(iTechL):

						iStolen = intSqrt(CyPlayerL.calculateBaseNetResearch() * 4) * iHandicapFactor / 1000
						if iStolen:
							CyTeamW.changeResearchProgress(iTechW, iStolen, iPlayerW)
							CyTeamL.changeResearchProgress(iTechL,-iStolen, iPlayerL)
							# Message
							if iPlayerW == iPlayerAct:
								CvUtil.sendMessage(
									TRNSLTR.getText("TXT_KEY_MSG_INDUSTRYESPIONAGE_YOU", (szTxt, iStolen)), iPlayerW, 16,
									'Art/Interface/Buttons/Process/processresearchmeagre.dds',
									ColorTypes(44), iX, iY, True, True, bForce=False
								)
							elif iPlayerL == iPlayerAct:
								CvUtil.sendMessage(
									TRNSLTR.getText("TXT_KEY_MSG_INDUSTRYESPIONAGE",(szTxt, iStolen)), iPlayerL, 16,
									'Art/Interface/Buttons/Process/processresearchmeagre.dds',
									ColorTypes(44), iX, iY, True, True, bForce=False
								)
			# Wonder effects
			aWonderTuple = self.aWonderTuple
			if iPlayerW in aWonderTuple[4]:
				for i, iPlayer in enumerate(aWonderTuple[4]):
					if iPlayer != iPlayerW: continue
					KEY = aWonderTuple[0][i]
					if KEY == "PERGAMON":
						iGGP = intSqrt(CyUnitL.getExperience())
						if iGGP:
							CyPlayerW.changeCombatExperience(iGGP)
					elif KEY == "GREAT_JAGUAR_TEMPLE":
						iChance = GAME.getSorenRandNum(5, "Jaguar")
						if not iChance:
							CyUnitW.setDamage(0, -1)

		iUnitW = CyUnitW.getUnitType()
		mapUnitType = self.mapUnitType
		# Spartacus Capture Event
		if iUnitW == mapUnitType["SPARTACUS"]:
			# Capture 25%
			if not GAME.getSorenRandNum(4, "Gods"):

				CyPlotL = CyUnitL.plot()
				if not CyPlotL.isVisibleEnemyUnit(iPlayerW):
					iX = CyPlotL.getX()
					iY = CyPlotL.getY()
				else:
					CyPlotW = CyUnitW.plot()
					iX = CyPlotW.getX()
					iY = CyPlotW.getY()

				GC.getPlayer(iPlayerW).initUnit(mapUnitType["GLADIATOR"], iX, iY, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_NORTH).finishMoves()
				# Message
				if iPlayerAct is None:
					iPlayerAct = GAME.getActivePlayer()
				if iPlayerW == iPlayerAct:
					CvUtil.sendMessage(
						TRNSLTR.getText("TXT_KEY_SPARTACUS_CAPTURE_SUCCESS",()), iPlayerW, 15,
						',Art/Interface/Buttons/Units/ICBM.dds,Art/Interface/Buttons/Warlords_Atlas_1.dds,3,11',
						ColorTypes(44), iX, iY, True, True, 0, "", False
					)
		# Warriors Of God - Monk
		elif iUnitW == mapUnitType["MONK"]:
			CyPlotW = CyUnitW.plot()
			if CyPlotW.isCity():
				CyCity = CyPlotW.getPlotCity()
				iReligion = CyPlayerW.getStateReligion()
				if iReligion != -1 and not CyCity.isHasReligion(iReligion):
					CyCity.setHasReligion(iReligion, True, True, True)
		# Warriors Of God - Fanatic
		elif iUnitW == mapUnitType["FANATIC"]:
			if not self.GO_ONE_CITY_CHALLENGE:
				iReligion = CyPlayerW.getStateReligion()
				if iReligion != -1:
					CyPlotL = CyUnitL.plot()
					if CyPlotL.isCity():
						CyCity = CyPlotL.getPlotCity()
						if not CyCity.isHasReligion(iReligion) and (self.GO_NO_CITY_RAZING or CyCity.getPopulation() > 1):
							if not CyTeamW:
								iTeamW = CyPlayerW.getTeam()
								CyTeamW = GC.getTeam(iTeamW)
							if CyTeamW.isAtWarWith(GC.getPlayer(CyCity.getOwner()).getTeam()) and not CyPlotL.getNumVisibleEnemyDefenders(CyUnitW):
								CyCity.setHasReligion(iReligion, True, True, True)


	def onCombatLogCalc(self, argsList):
		genericArgs = argsList[0][0]
		cdAttacker = genericArgs[0]
		cdDefender = genericArgs[1]
		iCombatOdds = genericArgs[2]
		CvUtil.combatMessageBuilder(cdAttacker, cdDefender, iCombatOdds)


	def onCombatLogHit(self, argsList):
		genericArgs = argsList[0][0]
		cdAttacker = genericArgs[0]
		cdDefender = genericArgs[1]
		iIsAttacker = genericArgs[2]
		iDamage = genericArgs[3]

		if cdDefender.eOwner == cdDefender.eVisualOwner:
			szDefenderName = GC.getPlayer(cdDefender.eOwner).getNameKey()
		else:
			szDefenderName = TRNSLTR.getText("TXT_KEY_TRAITHELP_PLAYER_UNKNOWN", ())
		if cdAttacker.eOwner == cdAttacker.eVisualOwner:
			szAttackerName = GC.getPlayer(cdAttacker.eOwner).getNameKey()
		else:
			szAttackerName = TRNSLTR.getText("TXT_KEY_TRAITHELP_PLAYER_UNKNOWN", ())

		if not iIsAttacker:
			combatMessage = TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_HIT", (szDefenderName, cdDefender.sUnitName, iDamage, cdDefender.iCurrHitPoints, cdDefender.iMaxHitPoints))
			CyInterface().addCombatMessage(cdAttacker.eOwner,combatMessage)
			CyInterface().addCombatMessage(cdDefender.eOwner,combatMessage)
			if cdDefender.iCurrHitPoints <= 0:
				combatMessage = TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_DEFEATED", (szAttackerName, cdAttacker.sUnitName, szDefenderName, cdDefender.sUnitName))
				CyInterface().addCombatMessage(cdAttacker.eOwner,combatMessage)
				CyInterface().addCombatMessage(cdDefender.eOwner,combatMessage)
		elif iIsAttacker == 1:
			combatMessage = TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_HIT", (szAttackerName, cdAttacker.sUnitName, iDamage, cdAttacker.iCurrHitPoints, cdAttacker.iMaxHitPoints))
			CyInterface().addCombatMessage(cdAttacker.eOwner,combatMessage)
			CyInterface().addCombatMessage(cdDefender.eOwner,combatMessage)
			if cdAttacker.iCurrHitPoints <= 0:
				combatMessage = TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_DEFEATED", (szDefenderName, cdDefender.sUnitName, szAttackerName, cdAttacker.sUnitName))
				CyInterface().addCombatMessage(cdAttacker.eOwner,combatMessage)
				CyInterface().addCombatMessage(cdDefender.eOwner,combatMessage)


	def onImprovementBuilt(self, argsList):
		iImprovement, iX, iY = argsList
		if iImprovement < 0:
			print "[INFO] CvEventManager.onImprovementBuilt\n\tUnexpected input argument"
			return

		# Worker placed bonus
		szType = GC.getImprovementInfo(iImprovement).getType()
		if szType[:18] == "IMPROVEMENT_BONUS_":
			CyPlot = GC.getMap().plot(iX, iY)
			if CyPlot.getBonusType(-1) > -1:
				return # Bonus was discovered while the farmer was working.
			iBonus = GC.getInfoTypeForString(szType[12:])
			if iBonus > -1:
				CyPlot.setImprovementType(-1)
				CyPlot.setBonusType(iBonus)
			return
		# Worker placed feature
		mapImpType = self.mapImpType

		if iImprovement == mapImpType['IMPROVEMENT_GROW_FOREST']:
			CyPlot = GC.getMap().plot(iX, iY)
			CyPlot.setImprovementType(-1)

			if CyPlot.getTerrainType() == GC.getInfoTypeForString('TERRAIN_TAIGA'):
				CyPlot.setFeatureType(GC.getFEATURE_FOREST(), 2) # snowy forest
			else:
				lat = CyPlot.getLatitude()
				iChance = GAME.getSorenRandNum(100, "FEATURE_FOREST")
				if lat > 60: # POLAR
					if iChance < 10:
						CyPlot.setFeatureType(GC.getFEATURE_FOREST(), 0) # leafy forest
					else:
						CyPlot.setFeatureType(GC.getFEATURE_FOREST(), 1) # evergreen forest
				elif lat > 25: # TEMPERATE
					if iChance < 70:
						CyPlot.setFeatureType(GC.getFEATURE_FOREST(), 0) # leafy forest
					else:
						CyPlot.setFeatureType(GC.getFEATURE_FOREST(), 1) # evergreen forest
				else: # EQUATOR
					if iChance < 10:
						CyPlot.setFeatureType(GC.getFEATURE_FOREST(), 1) # evergreen forest
					elif iChance < 70:
						CyPlot.setFeatureType(GC.getFEATURE_FOREST(), 0) # leafy forest
					else:
						if CyPlot.getTerrainType() in (GC.getInfoTypeForString('TERRAIN_LUSH'), GC.getInfoTypeForString('TERRAIN_MUDDY')):
							CyPlot.setFeatureType(GC.getFEATURE_JUNGLE(), 0)
						else:
							CyPlot.setFeatureType(GC.getInfoTypeForString('FEATURE_BAMBOO'), 0)

		elif iImprovement == mapImpType['IMPROVEMENT_FARM']:
			iPlayer = GC.getMap().plot(iX, iY).getOwner()
			if iPlayer != -1:
				CyPlayer = GC.getPlayer(iPlayer)
				if CyPlayer.isCivic(self.CIVIC_CONSCRIPTION):
					iEra = CyPlayer.getCurrentEra()
					if iEra != -1 and iEra in self.mapMilitiaByEra and self.mapMilitiaByEra[iEra] != -1:
						CyPlayer.initUnit(self.mapMilitiaByEra[iEra], iX, iY, UnitAITypes.UNITAI_RESERVE, DirectionTypes.NO_DIRECTION)
						if iPlayer == GAME.getActivePlayer():
							CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_RECRUITED",()), iPlayer, 16, 'Art/Interface/Buttons/Civics/Serfdom.dds', ColorTypes(44), iX, iY, True, True, bForce=False)


	'''
	def onImprovementDestroyed(self, argsList):
		iImprovement, iOwner, iX, iY = argsList


	def onRouteBuilt(self, argsList):
		iRoute, iX, iY = argsList


	def onPlotRevealed(self, argsList):
		pPlot = argsList[0]
		iTeam = argsList[1]


	def onPlotFeatureRemoved(self, argsList):
		pPlot = argsList[0]
		iFeatureType = argsList[1]
		pCity = argsList[2] # This can be null


	def onPlotPicked(self, argsList):
		pPlot = argsList[0]
	'''


	def onNukeExplosion(self, argsList):
		CyPlot, CyUnit = argsList
		if not CyUnit:
			return
		GC = CyGlobalContext()
		iX = CyPlot.getX()
		iY = CyPlot.getY()
		iUnit = CyUnit.getUnitType()
		if iUnit == GC.getInfoTypeForString('UNIT_TURN'):
			if CyPlot.isCity():
				GC.getPlayer(CyUnit.getOwner()).acquireCity(CyPlot.getPlotCity(), False, False)
			for pNukedPlot in CyPlot.rect(1, 1):
				numUnits = CyPlot.getNumUnits()
				for e in xrange(numUnits,0,-1):
					pUnit = CyPlot.getUnit(e)
					pUnit.kill(False, -1)
				if pNukedPlot.getFeatureType() == GC.getInfoTypeForString('FEATURE_FALLOUT'):
					pNukedPlot.setFeatureType(-1, -1)

		elif iUnit == GC.getInfoTypeForString('UNIT_FUSION_NUKE'):
			for plotX in CyPlot.rect(1, 1):
				if plotX.isPeak():
					plotX.setPlotType(PlotTypes.PLOT_HILLS, True, True)
				elif plotX.isHills():
					plotX.setPlotType(PlotTypes.PLOT_LAND, True, True)

		elif iUnit == GC.getInfoTypeForString('UNIT_FUSION_NOVA'):
			for plotX in CyPlot.rect(1, 1):
				if plotX.isPeak():
					plotX.setPlotType(PlotTypes.PLOT_HILLS, True, True)
				elif plotX.isHills():
					plotX.setPlotType(PlotTypes.PLOT_LAND, True, True)

		elif iUnit == GC.getInfoTypeForString('UNIT_POISON_NUKE'):
			for plotX in CyPlot.rect(1, 1):
				plotX.setFeatureType(GC.getInfoTypeForString("FEATURE_INFECTIOUS_SMOG"), 1)

		elif iUnit == GC.getInfoTypeForString('UNIT_POISON_NOVA'):
			for plotX in CyPlot.rect(5, 5):
				plotX.setFeatureType(GC.getInfoTypeForString("FEATURE_PLAGUED_SMOG"), 1)
		else:
			print "CvEventManager.onNukeExplosion\n\tNuke with no special effects: " + CyUnit.getName()


	# Only ever called from exe.
	def onGotoPlotSet(self, argsList):
		#pPlot, iPlayer = argsList
		return


	def onBuildingBuilt(self, argsList):
		CyCity, iBuilding = argsList
		iPlayer = CyCity.getOwner()
		CyPlayer = GC.getPlayer(iPlayer)
		if not CyPlayer.isFeatAccomplished(FeatTypes.FEAT_NATIONAL_WONDER):
			if isNationalWonder(iBuilding):
				CyPlayer.setFeatAccomplished(FeatTypes.FEAT_NATIONAL_WONDER, True)

				if not self.bNetworkMP and GAME.getElapsedGameTurns() != 0 and iPlayer == GAME.getActivePlayer() and CyPlayer.isOption(PlayerOptionTypes.PLAYEROPTION_ADVISOR_POPUPS):
					popupInfo = CyPopupInfo()
					popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON)
					popupInfo.setData1(FeatTypes.FEAT_NATIONAL_WONDER)
					popupInfo.setData2(CyCity.getID())
					popupInfo.setText(TRNSLTR.getText("TXT_KEY_FEAT_NATIONAL_WONDER", (GC.getBuildingInfo(iBuilding).getTextKey(), CyCity.getNameKey(), )))
					popupInfo.setOnClickedPythonCallback("featAccomplishedOnClickedCallback")
					popupInfo.setOnFocusPythonCallback("featAccomplishedOnFocusCallback")
					popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_OK", ()), "")
					popupInfo.addPythonButton(TRNSLTR.getText("TXT_KEY_FEAT_ACCOMPLISHED_MORE", ()), "")
					popupInfo.addPopup(iPlayer)

		if not self.bNetworkMP and iPlayer == GAME.getActivePlayer() and GC.getBuildingInfo(iBuilding).getMovie() and CyPlayer.countNumBuildings(iBuilding) == 1:
			popupInfo = CyPopupInfo()
			popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
			popupInfo.setData1(iBuilding)
			popupInfo.setData2(CyCity.getID())
			popupInfo.setData3(0)
			popupInfo.setText('showWonderMovie')
			popupInfo.addPopup(iPlayer)

		mapBuildingType = self.mapBuildingType
		aWonderTuple = self.aWonderTuple
		if iBuilding in aWonderTuple[1]:
			i = aWonderTuple[1].index(iBuilding)
			KEY = aWonderTuple[0][i]
			aWonderTuple[3][i] = CyCity.getID()
			aWonderTuple[4][i] = iPlayer
			if KEY == "CRUSADE":
				iUnit = GC.getInfoTypeForString("UNIT_CRUSADER")
				if iUnit > -1:
					CyUnit = CyPlayer.initUnit(iUnit, CyCity.getX(), CyCity.getY(), UnitAITypes.UNITAI_ATTACK_CITY, DirectionTypes.NO_DIRECTION)
					CyCity.addProductionExperience(CyUnit, False)
			elif KEY == "TAIPEI_101":
				iTeam = CyPlayer.getTeam()
				for iPlayerX in xrange(self.MAX_PC_PLAYERS):
					if iPlayer == iPlayerX:
						continue
					CyPlayerX = GC.getPlayer(iPlayerX)
					if CyPlayerX.isAlive():
						CyPlayerX.AI_changeAttitudeExtra(iTeam, 3)
			elif KEY == "TSUKIJI":
				CyTeam = GC.getTeam(CyPlayer.getTeam())
				BOAT = GC.getInfoTypeForString("IMPROVEMENT_FISHING_BOATS")
				if BOAT > -1:
					CyTeam.changeImprovementYieldChange(BOAT, 0, 1)
					CyTeam.changeImprovementYieldChange(BOAT, 2, 1)

				aBonusList = []
				aList = [
					GC.getInfoTypeForString("BONUS_FISH"),
					GC.getInfoTypeForString("BONUS_CLAM"),
					GC.getInfoTypeForString("BONUS_CRAB"),
					GC.getInfoTypeForString("BONUS_SHRIMP"),
					GC.getInfoTypeForString("BONUS_LOBSTER")
				]
				iBonuses = 0
				for iBonus in aList:
					if iBonus > -1:
						aBonusList.append(iBonus)
						iBonuses += 1

				aPlotList = []
				if aBonusList:
					iPlots = 0
					for CyPlot in CyCity.plot().rect(3, 3):
						if not CyPlot.isWater() or CyPlot.getBonusType(-1) != -1:
							continue
						aPlotList.append(CyPlot)
						iPlots += 1

				if aPlotList:
					if iPlayer == GAME.getActivePlayer():
						bMessage = True
					else:
						bMessage = False
					bFirst = True
					while aPlotList:
						CyPlot = aPlotList.pop(GAME.getSorenRandNum(iPlots, "TSUKIJI"))
						iPlots -= 1
						if bFirst or not GAME.getSorenRandNum(5, "TSUKIJI"):
							bFirst = False
							BONUS = aBonusList[GAME.getSorenRandNum(iBonuses, "TSUKIJI")]
							CyPlot.setBonusType(BONUS)
							if BOAT > -1:
								CyPlot.setImprovementType(BOAT)
							if bMessage:
								CvUtil.sendMessage(
									TRNSLTR.getText("TXT_KEY_MSG_TSUKIJI", (GC.getBonusInfo(BONUS).getDescription(),)),
									iPlayer, 16, GC.getBonusInfo(BONUS).getButton(), ColorTypes(11), CyCity.getX(), CyCity.getY(), True, True
								)

			elif KEY == "NAZCA_LINES":
				NAZCA_LINES = GC.getInfoTypeForString("BUILDING_NAZCA_LINES")
				if NAZCA_LINES > -1:
					iEra = CyPlayer.getCurrentEra() + 1
					for i in xrange(iEra):
						iRandom = GAME.getSorenRandNum(8, "NazcaLines")
						if not iRandom:
							iBase = CyCity.getBuildingCommerceChange(NAZCA_LINES, 0)
							CyCity.setBuildingCommerceChange(NAZCA_LINES, 0, iBase + 4)
						elif iRandom == 1:
							iBase = CyCity.getBuildingCommerceChange(NAZCA_LINES, 1)
							CyCity.setBuildingCommerceChange(NAZCA_LINES, 1, iBase + 4)
						elif iRandom == 2:
							iBase = CyCity.getBuildingCommerceChange(NAZCA_LINES, 2)
							CyCity.setBuildingCommerceChange(NAZCA_LINES, 2, iBase + 4)
						elif iRandom == 3:
							iBase = CyCity.getBuildingCommerceChange(NAZCA_LINES, 3)
							CyCity.setBuildingCommerceChange(NAZCA_LINES, 3, iBase + 4)
						elif iRandom == 4:
							iBase = CyCity.getBuildingYieldChange(NAZCA_LINES, 0)
							CyCity.setBuildingYieldChange(NAZCA_LINES, 0, iBase + 4)
						elif iRandom == 5:
							iBase = CyCity.getBuildingYieldChange(NAZCA_LINES, 1)
							CyCity.setBuildingYieldChange(NAZCA_LINES, 1, iBase + 4)
						elif iRandom == 6:
							iBase = CyCity.getBuildingHappyChange(NAZCA_LINES)
							CyCity.setBuildingHappyChange(NAZCA_LINES, iBase + 2)
						else:
							iBase = CyCity.getBuildingHealthChange(NAZCA_LINES)
							CyCity.setBuildingHealthChange(NAZCA_LINES, iBase + 2)
		# Ishtar or Marco Polo
		elif iBuilding in (mapBuildingType["ISHTAR"], mapBuildingType["MARCO_POLO"]):
			CyArea = CyCity.area()
			iTeam = CyPlayer.getTeam()
			CyTeam = GC.getTeam(iTeam)
			for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
				if iTeamX != iTeam and not CyTeam.isHasMet(iTeamX):
					CyTeamX = GC.getTeam(iTeamX)
					if CyTeamX.countNumCitiesByArea(CyArea) > 0:
						CyTeamX.meet(iTeam, True)
		# Lotus Temple
		elif iBuilding == mapBuildingType["LOTUS_TEMPLE"]:
			bHuman = CyPlayer.isHuman()
			iCautious = GC.getInfoTypeForString("ATTITUDE_CAUTIOUS")
			for iPlayerX in xrange(self.MAX_PC_PLAYERS):
				if iPlayerX == iPlayer:
					continue
				CyPlayerX = GC.getPlayer(iPlayerX)
				if CyPlayerX.isAlive():
					if not bHuman:
						while CyPlayer.AI_getAttitude(iPlayerX) < iCautious:
							CyPlayer.AI_changeAttitudeExtra(iPlayerX, 1)
					if not CyPlayerX.isHuman():
						while CyPlayerX.AI_getAttitude(iPlayer) < iCautious:
							CyPlayerX.AI_changeAttitudeExtra(iPlayer, 1)
		# Cleopatra's Needle
		elif iBuilding == mapBuildingType["CLEOPATRA_NEEDLE"]:
			from operator import itemgetter

			iCityID = CyCity.getID()
			aList = []
			for CyCityX in CyPlayer.cities():
				if CyCityX.getID() != iCityID:
					aList.append((CyCityX, CyCityX.getCulture(iPlayer)))
			if aList:
				# Sort by descending culture
				aList.sort(key=itemgetter(1), reverse=True)
				for i, entry in enumerate(aList):
					entry[0].changeHasBuilding(iBuilding, True)
					if i == 1: # Max. 2 other cities will get the needle for free
						break
		# NANITE DEFUSER - destroyes all nukes from all players
		elif iBuilding == mapBuildingType["NANITE_DEFUSER"]:
			for iPlayerX in xrange(self.MAX_PLAYERS):
				for CyUnit in GC.getPlayer(iPlayerX).units():
					if CyUnit.isDead():
						print "CvEventManager\onBuildingBuilt", ("CyUnit.isDead()", CyUnit.isDead())
					elif CyUnit.nukeRange() > -1:
						CyUnit.kill(0, -1)
				# Global message
				iPlayerAct = GAME.getActivePlayer()
				if iPlayerAct > -1:
					CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_NANITE_DEFUSER_PYTHON",()), iPlayerAct, 24, 'art/interface/buttons/great_wonders/nanite_defuser.dds', ColorTypes(44), bForce=False)
		# Machu Picchu
		elif iBuilding == mapBuildingType["MACHU_PICCHU"]:
			iImprovement = GC.getInfoTypeForString("IMPROVEMENT_MACHU_PICCHU")
			if iImprovement > -1:
				aList = []
				for CyPlot in CyCity.plot().rect(3, 3):
					if CyPlot.isPeak():
						aList.append(CyPlot)
				if aList:
					CyPlot = aList[GAME.getSorenRandNum(len(aList), "Random Peak")]
					CyPlot.setImprovementType(iImprovement)
				else:
					print ("Warning CvEventManager.onBuildingBuilt\n\tMachu Picchu has been built in %s where there is no peaks in vicinity." % CyCity.getName())
			else:
				print "Warning CvEventManager.onBuildingBuilt\n\tIMPROVEMENT_MACHU_PICCHU doesn't exist"
		# Field of the Cloth of Gold
		elif iBuilding == mapBuildingType["FIELD_GOLD"]:
			iTeam = CyPlayer.getTeam()
			for iPlayerX in xrange(self.MAX_PC_PLAYERS):
				CyPlayerX = GC.getPlayer(iPlayerX)
				if CyPlayerX.isAlive():
					iTeamX = CyPlayerX.getTeam()
					if iTeam != iTeamX and GC.getTeam(iTeamX).isHasMet(iTeam):
						CyPlayerX.AI_changeAttitudeExtra(iTeam, 3)
		# Maginot Line
		elif iBuilding == mapBuildingType["MAGINOTLINE"]:
			iBunker = GC.getInfoTypeForString("IMPROVEMENT_COMMAND_BUNKER")
			if iBunker > -1:
				MAP = GC.getMap()
				iArea = CyCity.plot().getArea()
				iGridX = MAP.getGridWidth()
				iGridY = MAP.getGridHeight()
				bWrapX = MAP.isWrapX()
				bWrapY = MAP.isWrapY()

				bOK = False
				for CyPlot in MAP.plots():
					if bOK: # No adjacent bunkers
						bOK = False
						continue
					if CyPlot.getOwner() == iPlayer and CyPlot.getArea() == iArea and CyPlot.getImprovementType() != iBunker and CyPlot.canHaveImprovement(iBunker, -1, True):
						x = CyPlot.getX()
						y = CyPlot.getY()
						iEast = x + 1
						if iEast > iGridX:
							if bWrapX:
								iEast = 0
							else:
								iEast = x
						iNorth = y + 1
						if iNorth > iGridY:
							if bWrapY:
								iNorth = 0
							else:
								iNorth = y
						iWest = x - 1
						if iWest < 0:
							if bWrapX:
								iWest = iGridX
							else:
								iWest = 0
						iSouth = y - 1
						if iSouth < 0:
							if bWrapY:
								iSouth = iGridY
							else:
								iSouth = 0
							iSouth += iGridY
						aCyPlotList = [
							MAP.plot(x, iNorth),		MAP.plot(x, iSouth),
							MAP.plot(iWest, y),			MAP.plot(iEast, y),
							MAP.plot(iWest, iNorth),	MAP.plot(iEast, iNorth),
							MAP.plot(iWest, iSouth),	MAP.plot(iEast, iSouth)
						]

						for CyPlotX in aCyPlotList:
							if CyPlotX.getArea() == iArea and CyPlotX.getOwner() != iPlayer:
								break # A frontier.
						else: # Not a frontier.
							continue

						for CyPlotX in aCyPlotList:
							if CyPlotX.getOwner() == iPlayer and CyPlotX.getImprovementType() == iBunker:
								break # No adjacent bunkers
						else: # New Bunker
							bOK = True
							CyPlot.setImprovementType(iBunker)
			else:
				print "Warning CvEventManager.onBuildingBuilt\n\tIMPROVEMENT_COMMAND_BUNKER doesn't exist"
		# Silk Road
		elif iBuilding == mapBuildingType["SILK_ROAD"]:
			iUnit = GC.getInfoTypeForString("UNIT_SPY")
			if iUnit < 0:
				print "Error CvEventManager.onBuildingBuilt\n\tUNIT_SPY doesn't exist, aborting python effect for the silk road"
				return
			MAP = GC.getMap()
			CyPlot = CyCity.plot()
			iArea = CyPlot.getArea()
			CyPlot0 = CyPlot1 = CyPlayer.getCapitalCity().plot()

			iSilk = GC.getInfoTypeForString("BONUS_SILK")
			iSilkFarm = GC.getInfoTypeForString("IMPROVEMENT_SILK_FARM")
			iTrail = GC.getInfoTypeForString("ROUTE_TRAIL")
			iPath = GC.getInfoTypeForString("ROUTE_PATH")
			iRoute = GC.getInfoTypeForString("ROUTE_ROAD")
			if iRoute < 0:
				if iPath < 0:
					if iTrail < 0:
						print "Error CvEventManager.onBuildingBuilt\n\tROUTE_ROAD, ROUTE_PATH, and ROUTE_TRAIL doesn't exist, aborting python effect for the silk road"
						return
					else:
						iRoute = iTrail
						iTrail = -1
						print "Warning CvEventManager.onBuildingBuilt\n\tROUTE_ROAD and ROUTE_PATH doesn't exist."
				else:
					iRoute = iPath
					iPath = -1
					print "Warning CvEventManager.onBuildingBuilt\n\tROUTE_ROAD doesn't exist."
			if iTrail < 0:
				print "Warning CvEventManager.onBuildingBuilt\n\tROUTE_TRAIL doesn't exist."

			iThePopu = 0
			iThePath = 0
			for CyPlotZ in MAP.plots():
				if CyPlotZ.getArea() == iArea:
					iOwner = CyPlotZ.getOwner()
					if iOwner == iPlayer and iSilk > -1 and CyPlotZ.getBonusType(-1) == iSilk:
						if MAP.generatePathForHypotheticalUnit(CyPlot, CyPlotZ, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
							# Owned silk bonuses to the city that built the silk road.
							if iSilkFarm > -1:
								CyPlotZ.setImprovementType(iSilkFarm)
							for k in xrange(MAP.getLastPathStepNum()):
								CyPlotDo = MAP.getLastPathPlotByIndex(k)
								if CyPlotDo.getRouteType() in (-1, iTrail, iPath):
									CyPlotDo.setRouteType(iRoute)
					if CyPlotZ.isCity() == 1 and iOwner != iPlayer:
						if MAP.generatePathForHypotheticalUnit(CyPlot0, CyPlotZ, iPlayer, iUnit, PathingFlags.MOVE_IGNORE_DANGER+PathingFlags.MOVE_THROUGH_ENEMY, 1000):
							CyCityZ = CyPlotZ.getPlotCity()
							iPopu = CyCityZ.getPopulation()
							iPath = MAP.getLastPathStepNum()
							if iPopu > iThePopu or (iPath > iThePath and iPopu > iThePopu - iThePopu / 8):
								CyPlot1 = CyPlotZ
								if iPopu > iThePopu:
									iThePopu = iPopu
								iThePath = iPath
			# Capital to the city that built the silk road.
			if MAP.generatePathForHypotheticalUnit(CyPlot0, CyPlot, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
				for k in xrange(MAP.getLastPathStepNum()):
					CyPlotDo = MAP.getLastPathPlotByIndex(k)
					if CyPlotDo.getRouteType() in (-1, iTrail, iPath):
						CyPlotDo.setRouteType(iRoute)
			# Capital to a foreign metropolis.
			if MAP.generatePathForHypotheticalUnit(CyPlot0, CyPlot1, iPlayer, iUnit, PathingFlags.MOVE_IGNORE_DANGER+PathingFlags.MOVE_THROUGH_ENEMY, 1000):
				for k in xrange(MAP.getLastPathStepNum()):
					CyPlotDo = MAP.getLastPathPlotByIndex(k)
					if CyPlotDo.getRouteType() in (-1, iTrail, iPath):
						CyPlotDo.setRouteType(iRoute)
		# Route66
		elif iBuilding == mapBuildingType["ROUTE_66"]:
			iUnit = GC.getUNIT_WORKER()
			if iUnit < 0:
				print "Error CvEventManager.onBuildingBuilt\n\tUNIT_WORKER doesn't exist, aborting python effect for Route 66"
				return
			iCityID = CyCity.getID()
			MAP = GC.getMap()
			CyPlot = CyCity.plot()
			iAreaID = CyCity.area().getID()

			iThePopu = 0
			iThePath = 0
			for CyCityX in CyPlayer.cities():
				if CyCityX.area().getID() == iAreaID and CyCityX.getID() != iCityID:
					CyPlotX = CyCityX.plot()
					if MAP.generatePathForHypotheticalUnit(CyPlot, CyPlotX, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
						iPopu = CyCityX.getPopulation()
						iPath = MAP.getLastPathStepNum()
						if iPopu > iThePopu or (iPath > iThePath and iPopu > iThePopu - iThePopu / 8):
							CyCityDo = CyCityX
							CyPlotDo = CyPlotX
							iThePath = iPath
							iThePopu = iPopu

			if iThePath and MAP.generatePathForHypotheticalUnit(CyPlot, CyPlotDo, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
				iBuilding = GC.getInfoTypeForString("BUILDING_ROUTE_66_TERMINUS")
				if iBuilding > -1:
					CyCityDo.changeHasBuilding(iBuilding, True)
				iRoute = GC.getInfoTypeForString("ROUTE_HIGHWAY")
				if iRoute > -1:
					for k in xrange(MAP.getLastPathStepNum()):
						pRoutePlot = MAP.getLastPathPlotByIndex(k)
						pRoutePlot.setRouteType(iRoute)
		# Appian Way
		elif iBuilding == mapBuildingType["APPIAN_WAY"]:
			iUnit = GC.getUNIT_WORKER()
			if iUnit < 0:
				print "Error CvEventManager.onBuildingBuilt\n\tUNIT_WORKER doesn't exist, aborting python effect for Appian Way"
				return
			iRoute = GC.getInfoTypeForString("ROUTE_PAVED_ROAD")
			if iRoute < 0:
				print "Error CvEventManager.onBuildingBuilt\n\tROUTE_PAVED_ROAD doesn't exist, aborting python effect for Appian Way"
				return
			if iPlayer == GAME.getActivePlayer():
				CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_APPIAN_BUILT",()), iPlayer)
			MAP = GC.getMap()
			# The appian city
			iCityID = CyCity.getID()
			iAreaID = CyCity.area().getID()

			# Find start-point and cache city info
			aCityList = []
			iMaxPath2Appia = 0
			iCities = 0
			CyCityStart = None
			for CyCityX in CyPlayer.cities():
				if CyCityX.getID() != iCityID and CyCityX.area().getID() == iAreaID:
					if MAP.generatePathForHypotheticalUnit(CyCity.plot(), CyCityX.plot(), iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 9999):
						iPath2Appia = MAP.getLastPathStepNum()
						if iPath2Appia > iMaxPath2Appia:
							iMaxPath2Appia = iPath2Appia
							CyCityStart = CyCityX
							iIdx = iCities
						aCityList.append((CyCityX, iPath2Appia))
						iCities += 1

			if CyCityStart:
				aCityList.pop(iIdx)
				iCities -= 1
				if iCities:
					# Find end-point
					iMaxPath = 0
					i = 0
					while i < iCities:
						CyCityX = aCityList[i][0]
						MAP.generatePathForHypotheticalUnit(CyCityStart.plot(), CyCityX.plot(), iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 9999)
						iPath = MAP.getLastPathStepNum()
						if iPath > iMaxPath:
							iMaxPath = iPath
							CyCityEnd = CyCityX
							iIdx = i
						i += 1
					aCityList.pop(iIdx)
					iCities -= 1
					MAP.generatePathForHypotheticalUnit(CyCityEnd.plot(), CyCity.plot(), iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 9999)
					iOtherPath2Appia = MAP.getLastPathStepNum()
					aCityList0 = list(aCityList)
					iCities0 = iCities
				else:
					iOtherPath2Appia = 0

				# Build Appian way
				CyCityFrom = CyCityStart
				while True:
					# Find closest city that is not further away from the appian city.
					iMinPath = 0
					i = 0
					while i < iCities:
						CyCityX, iPath2Appia = aCityList[i]
						if iPath2Appia <= iMaxPath2Appia:
							MAP.generatePathForHypotheticalUnit(CyCityFrom.plot(), CyCityX.plot(), iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 9999)
							iPath = MAP.getLastPathStepNum()
							if not iMinPath or iPath < iMinPath:
								iMinPath = iPath
								iIdx = i
						i += 1
					if iMinPath:
						# Connect two cities on the way to appia
						CyCityTo, iPath2Appia = aCityList.pop(iIdx)
						iCities -= 1
						MAP.generatePathForHypotheticalUnit(CyCityFrom.plot(), CyCityTo.plot(), iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 9999)
						for j in xrange(iMinPath):
							MAP.getLastPathPlotByIndex(j).setRouteType(iRoute)
						CyCityFrom = CyCityTo
						iMaxPath2Appia = iPath2Appia
					else:
						# Connect to the appian city
						MAP.generatePathForHypotheticalUnit(CyCityFrom.plot(), CyCity.plot(), iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 9999)
						for j in xrange(iMaxPath2Appia):
							MAP.getLastPathPlotByIndex(j).setRouteType(iRoute)
						if iOtherPath2Appia:
							CyCityFrom = CyCityEnd
							iMaxPath2Appia = iOtherPath2Appia
							iOtherPath2Appia = 0
							iCities = iCities0
							aCityList = aCityList0
						else:
							break # All done

			else: # The wonder should do something in this case.
				# Let's fill out the fat cross of the city with the route.
				for i in xrange(GC.getNUM_CITY_PLOTS()):
					CyPlot = CyCity.getCityIndexPlot(i)
					if CyPlot and CyPlot.canBuild(iRoute, iPlayer, False):
						CyPlot.setRouteType(iRoute)
		# Golden Spike
		elif iBuilding == mapBuildingType["GOLDEN_SPIKE"]:
			iUnit = GC.getInfoTypeForString("UNIT_WORKER_INDUSTRIAL")
			if iUnit < 0:
				print "Error CvEventManager.onBuildingBuilt\n\tUNIT_WORKER_INDUSTRIAL doesn't exist, aborting python effect for Golden Spike"
				return
			iRoute = GC.getInfoTypeForString("ROUTE_RAILROAD")
			if iRoute < 0:
				print "Error CvEventManager.onBuildingBuilt\n\tROUTE_RAILROAD doesn't exist, aborting python effect for Golden Spike"
				return
			cityPlot = CyCity.plot()
			iArea = cityPlot.getArea()
			x0 = x1 = x2 = cityPlot.getX()
			y0 = y1 = y2 = cityPlot.getY()

			iMax1 = iMax2 = iMaxDelta = 0
			pMax1 = pMax2 = None

			if iPlayer == GAME.getActivePlayer():
				CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_GOLDEN_SPIKE_BUILT",()), iPlayer)

			MAP = GC.getMap()

			for plotX in MAP.plots():
				if (plotX.getArea() != iArea
				or plotX.getRouteType() == iRoute
				or plotX.isOwned() and plotX.getOwner() != iPlayer
				): continue

				x = plotX.getX()
				y = plotX.getY()
				iDistanceToCity = stepDistance(x0, y0, x, y)
				if (
					(iDistanceToCity >= iMax1 or iDistanceToCity >= iMax2)
				and MAP.generatePathForHypotheticalUnit(
						cityPlot, plotX, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000
					)
				):
					bDone = False
					if iDistanceToCity >= iMax1:
						iDelta = stepDistance(x, y, x2, y2)
						if iDelta > iMaxDelta:
							pMax1 = plotX
							x1 = x
							y1 = y
							iMaxDelta = iDelta
							bDone = True

					if not bDone and iDistanceToCity >= iMax2:
						iDelta = stepDistance(x1, y1, x, y)
						if iDelta > iMaxDelta:
							pMax2 = plotX
							x2 = x
							y2 = y
							iMaxDelta = iDelta

			if pMax1 and MAP.generatePathForHypotheticalUnit(cityPlot, pMax1, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
				for i in xrange(MAP.getLastPathStepNum()):
					MAP.getLastPathPlotByIndex(i).setRouteType(iRoute)

			if pMax2 and MAP.generatePathForHypotheticalUnit(cityPlot, pMax2, iPlayer, iUnit, PathingFlags.MOVE_SAFE_TERRITORY, 1000):
				for i in xrange(MAP.getLastPathStepNum()):
					MAP.getLastPathPlotByIndex(i).setRouteType(iRoute)

		elif iBuilding == mapBuildingType["ZIZKOV"]:
			TECH_SATELLITES = self.TECH_SATELLITES
			iTeam = CyPlayer.getTeam()
			for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
				if iTeamX == iTeam:
					continue
				CyTeamX = GC.getTeam(iTeamX)
				if not CyTeamX.isEverAlive() or CyTeamX.isVassal(iTeam):
					continue
				if TECH_SATELLITES > -1 and CyTeamX.isHasTech(TECH_SATELLITES):
					continue
				# Covers whole map for others
				GC.getMap().resetRevealedPlots(iTeamX)
			CvUtil.sendImmediateMessage(TRNSLTR.getText("TXT_KEY_MSG_ZIZKOV_JAM",()))
		elif iBuilding == mapBuildingType["NEANDERTHAL_EMBASSY"]:
			iLocal = GC.getInfoTypeForString("BUILDING_C_L_NEANDERTHAL")
			for cityX in CyPlayer.cities():
				cityX.changeHasBuilding(iLocal, True)


	def onProjectBuilt(self, argsList):
		CyCity, iProject = argsList
		iPlayer = CyCity.getOwner()
		iPlayerAct = GAME.getActivePlayer()
		# Movie
		if not self.bNetworkMP and iPlayer == iPlayerAct:
			popupInfo = CyPopupInfo()
			popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
			popupInfo.setData1(iProject)
			popupInfo.setData2(CyCity.getID())
			popupInfo.setData3(2)
			popupInfo.setText('showWonderMovie')
			popupInfo.addPopup(iPlayer)

		# Eden project
		if iProject == GC.getInfoTypeForString("PROJECT_EDEN_PROJECT"):

			iTeam = GC.getPlayer(iPlayer).getTeam()
			iX = CyCity.getX()
			iY = CyCity.getY()
			DESERT		= GC.getTERRAIN_DESERT()
			PLAINS		= GC.getInfoTypeForString('TERRAIN_PLAINS')
			GRASS		= GC.getInfoTypeForString('TERRAIN_GRASSLAND')
			TAIGA		= GC.getInfoTypeForString('TERRAIN_TAIGA')
			TUNDRA		= GC.getInfoTypeForString('TERRAIN_TUNDRA')
			PERMAFROST	= GC.getInfoTypeForString('TERRAIN_PERMAFROST')
			ICE			= GC.getInfoTypeForString('TERRAIN_ICE')
			MARSH		= GC.getInfoTypeForString('TERRAIN_MARSH')

			A = GC.getInfoTypeForString('IMPROVEMENT_FARM')
			B = GC.getInfoTypeForString('IMPROVEMENT_VERTICAL_FARM')
			C = GC.getInfoTypeForString('IMPROVEMENT_WINDMILL')
			D = GC.getInfoTypeForString('IMPROVEMENT_PLANTATION')
			E = GC.getInfoTypeForString('IMPROVEMENT_WINERY')
			F = GC.getInfoTypeForString('IMPROVEMENT_COTTAGE')
			G = GC.getInfoTypeForString('IMPROVEMENT_HAMLET')
			H = GC.getInfoTypeForString('IMPROVEMENT_VILLAGE')
			I = GC.getInfoTypeForString('IMPROVEMENT_TOWN')
			J = GC.getInfoTypeForString('IMPROVEMENT_FOREST_PRESERVE')

			MAP = GC.getMap()
			for x in xrange(iX - 50, iX + 50, 1):
				for y in xrange(iY - 50, iY + 50, 1):
					CyPlot = MAP.plot(x, y)
					if CyPlot.getTeam() == iTeam:
						iTerrain = CyPlot.getTerrainType()
						if iTerrain == GRASS:
							i = CyPlot.getImprovementType()
							if i > -1 and i in (A, B, C, D, E, F, G, H, I, J):
								continue
							if CyPlot.getFeatureType() != GC.getFEATURE_JUNGLE():
								CyPlot.setFeatureType(GC.getFEATURE_FOREST(), 1)
						elif iTerrain == PLAINS:
							CyPlot.setTerrainType(GRASS, 1, 1)
						elif iTerrain == MARSH:
							CyPlot.setTerrainType(GRASS, 1, 1)
						elif iTerrain == TAIGA:
							CyPlot.setTerrainType(PLAINS, 1, 1)
						elif iTerrain == TUNDRA:
							CyPlot.setTerrainType(TAIGA, 1, 1)
						elif iTerrain == PERMAFROST:
							CyPlot.setTerrainType(TUNDRA, 1, 1)
						elif iTerrain == ICE:
							CyPlot.setTerrainType(PERMAFROST, 1, 1)
						elif iTerrain == DESERT:
							CyPlot.setTerrainType(PLAINS, 1, 1)
			# Team Message
			if iTeam == GAME.getActiveTeam():
				CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_EDEN_PYTHON",()), iPlayerAct, 16, 'Art/Interface/Buttons/Buildings/Eden.dds', ColorTypes(44), iX, iY, True, True, bForce=False)


	'''
	def onSelectionGroupPushMission(self, argsList): # AI never trigger this.
		iPlayer, iMission, iNumUnits, lUnitIDs = argsList
	'''


	def onUnitCreated(self, argsList): # Enabled in PythonCallbackDefines.xml (USE_ON_UNIT_CREATED_CALLBACK = True)
		CyUnit, = argsList

		# Subdued/Tamed animal graphical attachment
		if self.UNIT_STORY_TELLER != -1:
			KEY = GC.getUnitInfo(CyUnit.getUnitType()).getType()
			if KEY[:13] == 'UNIT_SUBDUED_' or KEY[:11] == 'UNIT_TAMED_':
				CyUnit.setLeaderUnitType(self.UNIT_STORY_TELLER)

		# Inspired Missionary
		aWonderTuple = self.aWonderTuple
		if "FA_MEN_SI" in aWonderTuple[0] \
		and CyUnit.getOwner() == aWonderTuple[4][aWonderTuple[0].index("FA_MEN_SI")] \
		and GC.getUnitInfo(CyUnit.getUnitType()).getPrereqReligion() > -1:

			CyUnit.setHasPromotion(GC.getInfoTypeForString("PROMOTION_FA_MEN_SI_INSPIRED"), True)


	def onUnitBuilt(self, argsList):
		CyCity, CyUnit = argsList
		if DebugUtils.bDebugMode:
			print "%s Built %s in %s" %(GC.getPlayer(CyCity.getOwner()).getCivilizationDescription(0), CyUnit.getName(), CyCity.getName())
		CvAdvisorUtils.unitBuiltFeats(CyCity, CyUnit)
		iPlayer = CyUnit.getOwner()
		CyPlayer = GC.getPlayer(iPlayer)
		iUnit = CyUnit.getUnitType()
		'''
		## Hero Movie (Not implemented yet)
		if not self.bNetworkMP and iPlayer == GAME.getActivePlayer() and isWorldUnit(iUnit):
			popupInfo = CyPopupInfo()
			popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
			popupInfo.setData1(iUnit)
			popupInfo.setData2(CyCity.getID())
			popupInfo.setData3(4)
			popupInfo.setText("showWonderMovie")
			popupInfo.addPopup(iPlayer)
		'''

		# Technocracy - Free Promotion
		if CyPlayer.getCivics(self.CIVICOPTION_POWER) == self.CIVIC_TECHNOCRACY:
			CyTeam = GC.getTeam(CyPlayer.getTeam())
			if CyTeam.isHasTech(self.TECH_SMART_DUST):
				iSensors = self.PROMOTION_SENSORS
				if CyUnit.isPromotionValid(iSensors):
					CyUnit.setHasPromotion(iSensors, True)

		# Immigration Mod
		if iUnit == self.UNIT_IMMIGRANT:
			iNewPop = CyCity.getPopulation() - 2
			if iNewPop > -1:
				if iNewPop == 0:
					iNewPop = 1
				CyCity.setPopulation(iNewPop)


	def onUnitKilled(self, argsList):
		CyUnit, iAttacker = argsList # iAttacker is a PlayerType
		# Player who lost the unit
		iPlayerL = CyUnit.getOwner()

		aWonderTuple = self.aWonderTuple
		if iPlayerL in aWonderTuple[4]:
			CyPlayerL = None
			for i, iPlayerX in enumerate(aWonderTuple[4]):
				if iPlayerX != iPlayerL: continue
				KEY = aWonderTuple[0][i]
				if KEY == "ALAMO":
					if CyPlayerL is None:
						CyPlayerL = GC.getPlayer(iPlayerL)
					iValue = CyUnit.baseCombatStr()
					for CyCity in CyPlayerL.cities():
						CyCity.changeCulture(iPlayerL, iValue, False)
				elif KEY == "CYRUS_TOMB":
					if CyUnit.isHasPromotion(GC.getInfoTypeForString("PROMOTION_LEADER")) or CyUnit.getUnitType() == GC.getInfoTypeForString("UNIT_GREAT_GENERAL"):
						if CyPlayerL is None:
							CyPlayerL = GC.getPlayer(iPlayerL)
						CyCity = CyPlayerL.getCity(aWonderTuple[3][i])
						iX = CyCity.getX()
						iY = CyCity.getY()
						szName = CyUnit.getNameNoDesc()
						CyUnitNew = CyPlayerL.initUnit(GC.getInfoTypeForString("UNIT_GREAT_GENERAL"), iX, iY, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
						CyUnitNew.setName(szName)
						CyUnitNew.setImmobileTimer(2 * (1 + GAME.getGameSpeedType()))
						# Message
						if iPlayerL == GAME.getActivePlayer():
							CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_GG_REVIVE", (szName,)), iPlayerL, 16, 'Art/Interface/Buttons/Great_Wonders/cyrustomb.dds', ColorTypes(11), iX, iY, True, True, bForce=False)

		# Subdued/Tamed animal graphical attachment
		if CyUnit.getLeaderUnitType() == self.UNIT_STORY_TELLER:
			# This will prevent a 'beastmaster lost' message when the unit is killed.
			CyUnit.setLeaderUnitType(-1)


	def onUnitPromoted(self, argsList):
		CyUnit, iPromotion = argsList
		CyPlayer = GC.getPlayer(CyUnit.getOwner())

		# AI promotion redirection
		if not CyPlayer.isHuman():
			aHiPriList = ('PROMOTION_LIVE1', 'PROMOTION_LIVE2')
			iDomainType = CyUnit.getDomainType()
			mapDomain = self.mapDomain
			if iDomainType == mapDomain['DOMAIN_LAND']:
				if CyUnit.isHasUnitCombat(self.UNITCOMBAT_HUNTER):
					aHiPriList += ("PROMOTION_HUNTER1", "PROMOTION_HUNTER2", "PROMOTION_HUNTER3", "PROMOTION_HUNTER_GREAT")
			elif iDomainType == mapDomain['DOMAIN_SEA']:
				if CyUnit.isHasUnitCombat(self.UNITCOMBAT_RECON):
					aHiPriList += ("PROMOTION_SEA_HUNTER1", "PROMOTION_SEA_HUNTER2", "PROMOTION_SEA_HUNTER3", "PROMOTION_SEA_HUNTER_GREAT")
			aList1 = []
			aList2 = []
			mapPromoType = self.mapPromoType
			for KEY, iPromo in mapPromoType.iteritems():
				if iPromo < 0: continue
				# Assign chance
				if KEY in aHiPriList:
					if iPromo == iPromotion:
						aList1 = []
						break
					aList1.append((iPromo, 0))
				elif KEY in ('PROMOTION_SNEAK', 'PROMOTION_MARAUDER', 'PROMOTION_INDUSTRYESPIONAGE'):
					if iPromo == iPromotion:
						aList2 = []
						break
					aList2.append((iPromo, 4))
			# Prioritize promos
			aList = []
			if aList1:
				aList = aList1
				if aList2:
					aList += aList2

			for iPromo, iChance in aList:
				if CyUnit.canAcquirePromotion(iPromo):
					if iChance and GAME.getSorenRandNum(iChance, "Generic"): continue
					CyUnit.setHasPromotion(iPromotion, False)
					CyUnit.setHasPromotion(iPromo, True)
					break

	def onUnitUpgraded(self, argsList):
		if DebugUtils.bDebugMode:
			CyUnitOld, CyUnitNew, iPrice = argsList
			print "%s Upgraded %s to %s" %(GC.getPlayer(CyUnitOld.getOwner()).getCivilizationDescription(0), CyUnitOld.getName(), CyUnitNew.getName())


	def onUnitRename(self, argsList):
		CyUnit, = argsList
		if CyUnit.getOwner() == GAME.getActivePlayer():
			self.__eventEditUnitNameBegin(CyUnit)


	'''
	def onUnitPillage(self, argsList):
		pUnit, iImprovement, iRoute, iOwner = argsList
	'''


	def onUnitSpreadReligionAttempt(self, argsList):
		#unit, iReligion, bSuccess = argsList
		if not argsList[2]:
			unit = argsList[0]
			aWonderTuple = self.aWonderTuple
			if "FA_MEN_SI" in aWonderTuple[0] and unit.getOwner() == aWonderTuple[4][aWonderTuple[0].index("FA_MEN_SI")]:
				CyCity = GC.getMap().plot(unit.getX(), unit.getY()).getPlotCity()
				CyCity.setHasReligion(GC.getUnitInfo(unit.getUnitType()).getPrereqReligion(), True, True, True)


	'''
	def onUnitGifted(self, argsList):
		pUnit, iGiftingPlayer, pPlotLocation = argsList


	def onUnitBuildImprovement(self, argsList):
		# Improvements and Routes
		pUnit, iBuild, bFinished = argsList


	def onGoodyReceived(self, argsList):
		iPlayer, pPlot, pUnit, iGoodyType = argsList
	'''


	def onGreatPersonBorn(self, argsList):
		CyUnit, iPlayer, CyCity = argsList

		aWonderTuple = self.aWonderTuple
		if "LASCAUX" in aWonderTuple[0] and iPlayer == aWonderTuple[4][aWonderTuple[0].index("LASCAUX")] and CyCity.getCultureThreshold() > 0:

			CyCity.changeCulture(iPlayer, CyCity.getCultureThreshold() / 10, True)
			iCount = CyCity.getAddedFreeSpecialistCount(GC.getInfoTypeForString("SPECIALIST_GREAT_ARTIST"))
			for i in xrange(iCount):
				CyCity.changeCulture(iPlayer, CyCity.getCultureThreshold() / 20, True)


	def onTechAcquired(self, argsList):
		iTech, iTeam, iPlayer, bAnnounce = argsList
		# iPlayer may be NULL (-1)
		if not iPlayer > -1: return

		CyPlayer = GC.getPlayer(iPlayer)

		if CyPlayer.isNPC() or not GAME.isFinalInitialized():
			print "%s gained the technology %s" %(CyPlayer.getCivilizationDescription(0), GC.getTechInfo(iTech).getDescriptionForm(0))
			return

		# Show tech splash when applicable
		if bAnnounce and not CyInterface().noTechSplash():
			if iPlayer == GAME.getActivePlayer():
				if not GAME.GetWorldBuilderMode() and not self.bNetworkMP:
					popupInfo = CyPopupInfo()
					popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
					popupInfo.setData1(iTech)
					popupInfo.setText('showTechSplash')
					popupInfo.addPopup(iPlayer)

		# Start as minor civ, become major.
		if self.GO_START_AS_MINORS:
			CyTeam = GC.getTeam(iTeam)
			bhasTech = CyTeam.isHasTech(GC.getInfoTypeForString("TECH_CONDUCT"))
			if CyTeam.isMinorCiv() and (CyTeam.isOpenBordersTrading() or bhasTech):				
				CyTeam.setIsMinorCiv(False)
				GC.getMap().verifyUnitValidPlot()
				# Message
				if iPlayer == GAME.getActivePlayer():
					CvUtil.sendMessage(
						TRNSLTR.getText("TXT_KEY_BARBCIV_DISCOVER_WRITING", ()), iPlayer, 16,
						eColor = GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"),
						eMsgType = InterfaceMessageTypes.MESSAGE_TYPE_MAJOR_EVENT, bForce = False
					)

		# Free Gatherer from Gathering
		if iTech == self.TECH_GATHERING:
			X = -1
			Y = -1
			CyCity = CyPlayer.getCapitalCity()

			if CyCity:
				X = CyCity.getX(); Y = CyCity.getY()
			else:
				CyUnit, i = CyPlayer.firstUnit(False)

				if CyUnit:
					X = CyUnit.getX(); Y = CyUnit.getY()
				else:
					CyPlot = CyPlayer.getStartingPlot()
					if CyPlot:
						X = CyPlot.getX(); Y = CyPlot.getY()

			if -1 not in (X, Y):
				if GC.getCivilizationInfo(CyPlayer.getCivilizationType()).getType() == "CIVILIZATION_NEANDERTHAL":
					iWorker = GC.getInfoTypeForString("UNIT_NEANDERTHAL_GATHERER")
				else:
					iWorker = GC.getInfoTypeForString("UNIT_GATHERER")

				CyUnit = CyPlayer.initUnit(iWorker, X, Y, UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_SOUTH)

			else: print "Found no valid plot to place the gatherer.\n\tNew civ from revolution or barbCiv perhaps?"

		# Obsolete python building-effects
		aWonderTuple = self.aWonderTuple
		if iTech in aWonderTuple[2]:
			temp = list(aWonderTuple[2])
			n = 0
			for i, iTechX in enumerate(temp):
				if iTech == iTechX and iPlayer == aWonderTuple[4][i-n]:
					KEY = aWonderTuple[0][i-n]
					if KEY == "TAIPEI_101":
						for iPlayerX in xrange(self.MAX_PC_PLAYERS):
							if iPlayerX == iPlayer:
								continue
							CyPlayerX = GC.getPlayer(iPlayerX)
							if not CyPlayerX.isAlive() or CyPlayerX.isHuman():
								continue
							CyPlayerX.AI_changeAttitudeExtra(iTeam, -2)
					elif KEY == "TSUKIJI":
						IMP = GC.getInfoTypeForString('IMPROVEMENT_FISHING_BOATS')
						CyTeam = GC.getTeam(iTeam)
						CyTeam.changeImprovementYieldChange(IMP, 0, -1)
						CyTeam.changeImprovementYieldChange(IMP, 2, -1)
					del aWonderTuple[0][i-n], aWonderTuple[1][i-n], aWonderTuple[2][i-n], aWonderTuple[3][i-n], aWonderTuple[4][i-n]
					n += 1

		# Nazca Lines
		if "NAZCA_LINES" in aWonderTuple[0]:
			i = aWonderTuple[0].index("NAZCA_LINES")
			if iPlayer == aWonderTuple[4][i]:
				CyTeam = GC.getTeam(iTeam)
				bNewEra = True
				for iTechX in xrange(GC.getNumTechInfos()):
					if CyTeam.isHasTech(iTechX) and iTechX != iTech:
						if GC.getTechInfo(iTechX).getEra() >= GC.getTechInfo(iTech).getEra():
							bNewEra = False
							break
				if bNewEra:
					NAZCA_LINES = GC.getInfoTypeForString("BUILDING_NAZCA_LINES")
					CyCity = CyPlayer.getCity(aWonderTuple[3][i])
					iRandom = GAME.getSorenRandNum(8, "Nazca")
					if not iRandom:
						iBase = CyCity.getBuildingCommerceChange(NAZCA_LINES, 0)
						CyCity.setBuildingCommerceChange(NAZCA_LINES, 0, iBase + 4)
					elif iRandom == 1:
						iBase = CyCity.getBuildingCommerceChange(NAZCA_LINES, 1)
						CyCity.setBuildingCommerceChange(NAZCA_LINES, 1, iBase + 4)
					elif iRandom == 2:
						iBase = CyCity.getBuildingCommerceChange(NAZCA_LINES, 2)
						CyCity.setBuildingCommerceChange(NAZCA_LINES, 2, iBase + 4)
					elif iRandom == 3:
						iBase = CyCity.getBuildingCommerceChange(NAZCA_LINES, 3)
						CyCity.setBuildingCommerceChange(NAZCA_LINES, 3, iBase + 4)
					elif iRandom == 4:
						iBase = CyCity.getBuildingYieldChange(NAZCA_LINES, 0)
						CyCity.setBuildingYieldChange(NAZCA_LINES, 0, iBase + 4)
					elif iRandom == 5:
						iBase = CyCity.getBuildingYieldChange(NAZCA_LINES, 1)
						CyCity.setBuildingYieldChange(NAZCA_LINES, 1, iBase + 4)
					elif iRandom == 6:
						iBase = CyCity.getBuildingHappyChange(NAZCA_LINES)
						CyCity.setBuildingHappyChange(NAZCA_LINES, iBase + 2)
					else:
						iBase = CyCity.getBuildingHealthChange(NAZCA_LINES)
						CyCity.setBuildingHealthChange(NAZCA_LINES, iBase + 2)
					if iPlayer == GAME.getActivePlayer():
						CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_MSG_NAZCA_LINES",()), iPlayer)


	'''
	def onTechSelected(self, argsList):
		iTech, iPlayer = argsList
	'''


	def onReligionFounded(self, argsList):
		iReligion, iPlayer = argsList
		if not self.bNetworkMP and iPlayer == GAME.getActivePlayer() and GAME.isFinalInitialized() and not GAME.GetWorldBuilderMode():
			popupInfo = CyPopupInfo()
			popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
			popupInfo.setData1(iReligion)

			# All religions have holy cities at this point afaik, but it's an assumption that we shouldn't make.
			holyCity = GAME.getHolyCity(iReligion)
			if holyCity:
				popupInfo.setData2(holyCity.getID())
			# else use default Data2 value: -1

			popupInfo.setData3(1)
			popupInfo.setText('showWonderMovie')
			popupInfo.addPopup(iPlayer)

		if self.RoMOpt.isTelepathicReligion():
			for iPlayerX in xrange(self.MAX_PC_PLAYERS):
				if iPlayerX == iPlayer: continue
				CyPlayerX = GC.getPlayer(iPlayerX)
				if not CyPlayerX.isAlive() or iReligion != GC.getLeaderHeadInfo(CyPlayerX.getLeaderType()).getFavoriteReligion():
					continue
				capital = CyPlayerX.getCapitalCity()
				if capital:
					capital.setHasReligion(iReligion, True, True, True)
					if CyPlayerX.isHuman():
						strReligionName = GC.getReligionInfo(iReligion).getText()
						popup = CyPopup(-1, EventContextTypes.NO_EVENTCONTEXT, True)
						popup.setHeaderString(TRNSLTR.getText("TXT_KEY_POPUP_FAVORITE_RELIGION_HEADER",()), 1<<2)
						popup.setBodyString(TRNSLTR.getText("TXT_KEY_POPUP_FAVORITE_RELIGION", (strReligionName, strReligionName)), 1<<0)
						popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)


	'''
	def onReligionSpread(self, argsList):
		iReligion, iOwner, pSpreadCity = argsList


	def onReligionRemove(self, argsList):
		iReligion, iOwner, pRemoveCity = argsList


	def onCorporationFounded(self, argsList):
		iCorporation, iFounder = argsList


	def onCorporationSpread(self, argsList):
		iCorporation, iOwner, pSpreadCity = argsList


	def onCorporationRemove(self, argsList):
		iCorporation, iOwner, pRemoveCity = argsList


	def onGoldenAge(self, argsList):
		iPlayer = argsList[0]


	def onEndGoldenAge(self, argsList):
		iPlayer = argsList[0]
	'''


	def onChangeWar(self, argsList):
		bIsWar, iAttacker, iDefender = argsList # iAttacker & iDefender are Teams not Players.
		if bIsWar:
			aWonderTuple = self.aWonderTuple
			if "THE_MOTHERLAND_CALLS" in aWonderTuple[0]:
				iPlayer = aWonderTuple[4][aWonderTuple[0].index("THE_MOTHERLAND_CALLS")]
				if iPlayer > -1:
					CyPlayer = GC.getPlayer(iPlayer)
					if CyPlayer.getTeam() == iDefender:
						for CyCity in CyPlayer.cities():
							CyCity.changeHappinessTimer(10)
							CyUnit = CyPlayer.initUnit(CyCity.getConscriptUnit(), CyCity.getX(), CyCity.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
							CyCity.addProductionExperience(CyUnit, True)


	'''
	def onChat(self, argsList):
		chatMessage = "%s" %(argsList[0],)


	def onSetPlayerAlive(self, argsList):
		iPlayer, bNewValue, = argsList

	def onPlayerChangeStateReligion(self, argsList):
		iPlayer, iNewReligion, iOldReligion = argsList


	def onPlayerGoldTrade(self, argsList):
		iFromPlayer, iToPlayer, iGoldAmount = argsList
	'''


	def onCityBuilt(self, argsList):
		CyCity, CyUnit, = argsList
		if not CyCity:
			print "[ERR] CvEventManager.onCityBuilt\n  Not a real city!"
			return
		iPlayer = CyCity.getOwner()
		# Give extra population to new cities
		iPop = 0
		if CyUnit:
			iUnit = CyUnit.getUnitType()
			if iUnit in self.mapSettlerPop:
				iPop += self.mapSettlerPop[iUnit]

			for iPromo, iBuilding, _ in self.aCultureList:
				if -1 in (iPromo, iBuilding): continue
				if CyUnit.isHasPromotion(iPromo):
					CyCity.changeHasBuilding(iBuilding, True)

			# Give a free defender to the first city when it is built
			if iUnit == self.UNIT_BAND:
				CyPlayer = GC.getPlayer(iPlayer)
				if GC.getCivilizationInfo(CyPlayer.getCivilizationType()).getType() == "CIVILIZATION_NEANDERTHAL":
					iUnitTG = GC.getInfoTypeForString("UNIT_NEANDERTHAL_TRIBAL_GUARDIAN")
				else:
					iUnitTG = GC.getInfoTypeForString("UNIT_TRIBAL_GUARDIAN")
				CyUnitTG = CyPlayer.initUnit(iUnitTG, CyUnit.getX(), CyUnit.getY(), UnitAITypes.UNITAI_PROPERTY_CONTROL, DirectionTypes.DIRECTION_SOUTH)
				iExp = CyUnit.getExperience()
				CyUnitTG.setExperience(iExp)
		if iPop:
			CyCity.changePopulation(iPop)
			if self.GO_1_CITY_TILE_FOUNDING:
				CyCity.changeFood(CyCity.growthThreshold()/4)
			else:
				CyCity.changeFood(CyCity.growthThreshold()/8)
		# Human player city naming
		iActivePlayer = GAME.getActivePlayer()
		if iPlayer == iActivePlayer and not GAME.getAIAutoPlay(iActivePlayer):
			self.__eventEditCityNameBegin((CyCity, False))


	def onCityRazed(self, argsList):
		CyCity, iPlayer = argsList
		iCityID = CyCity.getID()
		# Handle Python Buildings
		aWonderTuple = self.aWonderTuple
		if iCityID in aWonderTuple[3]:
			n = 0
			temp = list(aWonderTuple[3])
			for i, ID in enumerate(temp):
				if ID != iCityID: continue
				idx = i - n
				KEY = aWonderTuple[0][idx]
				if KEY == "TAIPEI_101":
					CyPlayer = GC.getPlayer(iPlayer)
					if not CyPlayer.isNPC():
						iTeam = CyPlayer.getTeam()
						for iPlayerX in xrange(self.MAX_PC_PLAYERS):
							if iPlayerX == iPlayer:
								continue
							CyPlayerX = GC.getPlayer(iPlayerX)
							if not CyPlayerX.isAlive() or CyPlayerX.isHuman():
								continue
							CyPlayerX.AI_changeAttitudeExtra(iTeam, -2)
				elif KEY == "TSUKIJI":
					CyPlayer = GC.getPlayer(iPlayer)
					IMP = GC.getInfoTypeForString('IMPROVEMENT_FISHING_BOATS')
					CyTeam = GC.getTeam(CyPlayer.getTeam())
					CyTeam.changeImprovementYieldChange(IMP, 0, -1)
					CyTeam.changeImprovementYieldChange(IMP, 2, -1)
				del aWonderTuple[0][idx], aWonderTuple[1][idx], aWonderTuple[2][idx], aWonderTuple[3][idx], aWonderTuple[4][idx]
				n += 1
		# Messages - Wonder Destroyed
		NumWonders = CyCity.getNumWorldWonders()
		if NumWonders:

			if CyCity.isRevealed(GAME.getActiveTeam(), False):
				iActivePlayer = GAME.getActivePlayer()
				if iActivePlayer > -1:
					if iPlayer == iActivePlayer:
						bActive = True
					else:
						bActive = False
					eColor = ColorTypes(GC.getInfoTypeForString("COLOR_WARNING_TEXT"))
					szPlayerName = GC.getPlayer(iPlayer).getName()
					iX = CyCity.getX()
					iY = CyCity.getY()
					artPath = 'Art/Interface/Buttons/General/warning_popup.dds'

					for iBuilding in xrange(GC.getNumBuildingInfos()):
						if not CyCity.hasBuilding(iBuilding): continue
						CvBuildingInfo = GC.getBuildingInfo(iBuilding)
						if CvBuildingInfo.getMaxGlobalInstances() == 1:

							if bActive:
								szTxt = TRNSLTR.getText("TXT_KEY_MSG_WONDER_DESTROYED_YOU", (CvBuildingInfo.getDescription(),))
							else:
								szTxt = TRNSLTR.getText("TXT_KEY_MSG_WONDER_DESTROYED", (szPlayerName, CvBuildingInfo.getDescription()))

							CvUtil.sendMessage(szTxt, iActivePlayer, 16, artPath, eColor, iX, iY, True, True, bForce = bActive)
		# Partisans!
		iPlayerHC = CyCity.findHighestCulture()
		if iPlayerHC not in (-1, iPlayer):
			CyPlayerHC = GC.getPlayer(iPlayerHC)
			if CyPlayerHC.getNumCities() > 1 or iPlayerHC != CyCity.getOwner() and CyPlayerHC.getNumCities() > 0:

				iNumCultureLevels = GC.getNumCultureLevelInfos()
				iGamespeedType = GAME.getGameSpeedType()
				iCulture = CyCity.getCulture(iPlayerHC)
				for i in xrange(iNumCultureLevels):
					iCultureLevel = iNumCultureLevels - i - 1
					if iCulture >= GC.getCultureLevelInfo(iCultureLevel).getSpeedThreshold(iGamespeedType):
						bAtCapital = True
						iNumUnits = iCultureLevel * 3/4

						if GC.getTeam(CyPlayerHC.getTeam()).isAtWarWith(GC.getPlayer(iPlayer).getTeam()):

							iX = CyCity.getX()
							iY = CyCity.getY()
							listPlots = []
							for i in xrange(3):
								for j in xrange(3):
									plotX = GC.getMap().plot(iX + i - 1, iY + j - 1)
									if plotX and not (plotX.isVisibleEnemyUnit(iPlayerHC) or plotX.isWater() or plotX.isImpassable() or plotX.isCity()):
										listPlots.append(plotX)

							if listPlots:
								bAtCapital = False
								iUnit = CyCity.getConscriptUnit()
								if iUnit > -1:
									for i in xrange(iNumUnits):
										iPlot = GAME.getSorenRandNum(len(listPlots), "Partisan event placement")
										CyPlayerHC.initUnit(iUnit, listPlots[iPlot].getX(), listPlots[iPlot].getY(), UnitAITypes.UNITAI_ATTACK, DirectionTypes.DIRECTION_SOUTH)

									unitInfo = GC.getUnitInfo(iUnit)
									CvUtil.sendMessage(
										TRNSLTR.getText("TXT_KEY_EVENT_PARTISANS_1", (iNumUnits, unitInfo.getTextKey(), CyCity.getName())),
										iPlayerHC, 10, unitInfo.getButton(), ColorTypes(13), iX, iY, True, True, bForce=False
									)
									CvUtil.sendMessage(
										TRNSLTR.getText("TXT_KEY_EVENT_PARTISANS_OTHER_1", (CyPlayerHC.getCivilizationAdjectiveKey(), CyCity.getName(), iNumUnits, unitInfo.getTextKey())),
										iPlayer, 10, unitInfo.getButton(), ColorTypes(44), iX, iY, True, True
									)

						if bAtCapital and not CyPlayerHC.isNPC():

							capital = CyPlayerHC.getCapitalCity()
							iUnit = capital.getConscriptUnit()
							if iUnit > -1:
								iNumUnits = (iNumUnits + 1) / 2
								iX = capital.getX(); iY = capital.getY()
								for i in xrange(iNumUnits):
									CyPlayerHC.initUnit(iUnit, iX, iY, UnitAITypes.UNITAI_ATTACK, DirectionTypes.DIRECTION_SOUTH)

								unitInfo = GC.getUnitInfo(iUnit)
								CvUtil.sendMessage(
									TRNSLTR.getText("TXT_KEY_EVENT_PARTISANS_2", (iNumUnits, unitInfo.getTextKey(), CyCity.getName())), iPlayerHC, 10,
									unitInfo.getButton(), ColorTypes(13), iX, iY, True, True, bForce=False
								)
								if not capital.isRevealed(iPlayer, False):
									iX = CyCity.getX(); iY = CyCity.getY()

								CvUtil.sendMessage(
									TRNSLTR.getText("TXT_KEY_EVENT_PARTISANS_OTHER_2", (CyPlayerHC.getCivilizationAdjectiveKey(), CyCity.getName(), iNumUnits, unitInfo.getTextKey())),
									iPlayer, 10, unitInfo.getButton(), ColorTypes(44), iX, iY, True, True
								)
						break
					elif iCultureLevel == 2:
						break

		# Ruin Arcology.
		mapBuildingType = self.mapBuildingType
		if CyCity.hasBuilding(mapBuildingType["ARCOLOGY"]) or CyCity.hasBuilding(mapBuildingType["ARCOLOGY_SHIELDING"]) or CyCity.hasBuilding(mapBuildingType["ADVANCED_SHIELDING"]):
			self.iArcologyCityID = iCityID
		else:
			self.iArcologyCityID = -1


	# This is the last function a city object call before being deleted.
	def onCityLost(self, argsList):
		CyCity, = argsList
		self.iOldCityID = iCityID = CyCity.getID()
		# Ruin Arcology.
		if self.iArcologyCityID != -1 and iCityID == self.iArcologyCityID:
			CyCity.plot().setImprovementType(GC.getInfoTypeForString("IMPROVEMENT_CITY_RUINS_ARCOLOGY"))
			self.iArcologyCityID = -1


	# This is before city has changed owner or been autorazed
	def onCityAcquired(self, argsList):
		iOwnerOld, iOwnerNew, city, bConquest, bTrade, bAutoRaze = argsList
		aWonderTuple = self.aWonderTuple
		if (
			bConquest
		and "HELSINKI" in aWonderTuple[0]
		and iOwnerNew == aWonderTuple[4][aWonderTuple[0].index("HELSINKI")]
		):
			iX = city.getX()
			iY = city.getY()
			for x in xrange(iX - 1, iX + 2):
				for y in xrange(iY - 1, iY + 2):
					CyPlot = GC.getMap().plot(x, y)
					iCulture = CyPlot.getCulture(iOwnerOld) / 10
					CyPlot.changeCulture(iOwnerOld,-iCulture, True)
					CyPlot.changeCulture(iOwnerNew, iCulture, True)



	def onCityAcquiredAndKept(self, argsList):
		iOwnerOld, iOwnerNew, city, bConquest, bTrade = argsList
		# Messages - Wonder Captured
		NumWonders = city.getNumWorldWonders()
		if NumWonders:
			if city.isRevealed(GAME.getActiveTeam(), False):
				iActivePlayer = GAME.getActivePlayer()
				if iActivePlayer > -1:
					if iOwnerNew == iActivePlayer:
						bActive = True
						artPath = 'Art/Interface/Buttons/General/happy_person.dds'
						eColor = ColorTypes(GC.getCOLOR_GREEN())
					else:
						bActive = False
						artPath = 'Art/Interface/Buttons/General/warning_popup.dds'
						eColor = -1

					szPlayerName = GC.getPlayer(iOwnerNew).getName()
					iX = city.getX()
					iY = city.getY()
					for iBuilding in xrange(GC.getNumBuildingInfos()):
						if city.hasBuilding(iBuilding):
							CvBuildingInfo = GC.getBuildingInfo(iBuilding)
							if CvBuildingInfo.getMaxGlobalInstances() == 1:

								if bActive:
									szTxt = TRNSLTR.getText("TXT_KEY_MSG_WONDER_CAPTURED_YOU", (CvBuildingInfo.getDescription(),))
								else:
									szTxt = TRNSLTR.getText("TXT_KEY_MSG_WONDER_CAPTURED", (szPlayerName, CvBuildingInfo.getDescription()))

								CvUtil.sendMessage(szTxt, iActivePlayer, 16, artPath, eColor, iX, iY, True, True, bForce = bActive)

		aWonderTuple = self.aWonderTuple
		iOldCityID = self.iOldCityID
		if iOldCityID in aWonderTuple[3]:
			iCityID = city.getID()
			iTeamN = GC.getPlayer(iOwnerNew).getTeam()
			CyTeamN = GC.getTeam(iTeamN)
			iTeamO = GC.getPlayer(iOwnerOld).getTeam()
			CyTeamO = GC.getTeam(iTeamO)
			temp = list(aWonderTuple[3])
			n = 0
			for j, ID in enumerate(temp):
				if ID != iOldCityID: continue
				i = j - n
				KEY = aWonderTuple[0][i]
				iBuilding = aWonderTuple[1][i]

				bObsolete = CyTeamN.isObsoleteBuilding(iBuilding)
				if bObsolete:
					del aWonderTuple[0][i], aWonderTuple[1][i], aWonderTuple[2][i], aWonderTuple[3][i], aWonderTuple[4][i]
					n += 1
				else:
					aWonderTuple[3][i] = iCityID
					aWonderTuple[4][i] = iOwnerNew

				if KEY == "TAIPEI_101":
					for iPlayerX in xrange(self.MAX_PC_PLAYERS):
						CyPlayerX = GC.getPlayer(iPlayerX)
						if CyPlayerX.isHuman() or not CyPlayerX.isAlive():
							continue
						if not bObsolete and iPlayerX != iOwnerNew:
							CyPlayerX.AI_changeAttitudeExtra(iTeamN, 2)
						if iPlayerX != iOwnerOld:
							CyPlayerX.AI_changeAttitudeExtra(iTeamO, -2)

				elif KEY == "TSUKIJI":
					IMP = GC.getInfoTypeForString('IMPROVEMENT_FISHING_BOATS')
					CyTeamO.changeImprovementYieldChange(IMP, 0, -1)
					CyTeamO.changeImprovementYieldChange(IMP, 2, -1)
					if not bObsolete:
						CyTeamN.changeImprovementYieldChange(IMP, 0, 1)
						CyTeamN.changeImprovementYieldChange(IMP, 2, 1)
			self.iOldCityID = None


	'''
	def onCultureExpansion(self, argsList):
		pCity = argsList[0]
		iPlayer = argsList[1]
	'''


	def onCityDoTurn(self, argsList):
		CyCity, iPlayer = argsList

		if not self.bNetworkMP and iPlayer == GAME.getActivePlayer():
			CvAdvisorUtils.cityAdvise(CyCity, iPlayer)

		iCityID = CyCity.getID()
		aWonderTuple = self.aWonderTuple
		if iCityID in aWonderTuple[3] and iPlayer in aWonderTuple[4]:
			for i, ID in enumerate(aWonderTuple[3]):
				if ID != iCityID: continue
				if iPlayer != aWonderTuple[4][i]: continue # Obsolete
				KEY = aWonderTuple[0][i]

				if KEY == "CRUSADE":
					iBuilding = aWonderTuple[1][i]
					if CyCity.getBuildingOriginalOwner(iBuilding) == iPlayer and not (GAME.getGameTurn() % (1 + 2 * self.iTrainPrcntGS / 100)):
						CyPlayer = GC.getPlayer(iPlayer)
						iUnit = GC.getInfoTypeForString("UNIT_CRUSADER")
						CyUnit = CyPlayer.initUnit(iUnit, CyCity.getX(), CyCity.getY(), UnitAITypes.UNITAI_ATTACK_CITY, DirectionTypes.NO_DIRECTION)
						CyCity.addProductionExperience(CyUnit, False)

				elif KEY == "GREAT_ZIMBABWE":
					if CyCity.isFoodProduction():
						CyCity.changeFood(CyCity.getYieldRate(0) - CyCity.foodConsumption(False, 0))
						if CyCity.getFood() >= CyCity.growthThreshold():
							CyCity.changePopulation(1)
							CyCity.setFood(CyCity.getFoodKept())

				elif KEY == "BIODOME":
					if not self.aBiodomeList or GAME.getGameTurn() % (4*self.iGameSpeedPercent/100 + 1):
						continue
					CyPlayer = GC.getPlayer(iPlayer)
					iX = CyCity.getX()
					iY = CyCity.getY()
					iUnit = self.aBiodomeList[GAME.getSorenRandNum(len(self.aBiodomeList), "Which Animal")]
					CyUnit = CyPlayer.initUnit(iUnit, iX, iY, UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
					CyCity.addProductionExperience(CyUnit, False)
					if iPlayer == GAME.getActivePlayer():
						CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_MSG_BIODOME",(CyUnit.getName(),)), iPlayer, 16, CyUnit.getButton(), ColorTypes(11), iX, iY, True, True)


	'''
	def onCityBuildingUnit(self, argsList):
		CyCity = argsList[0]
		iType = argsList[1]


	def onCityBuildingBuilding(self, argsList):
		CyCity = argsList[0]
		iType = argsList[1]

	def onCityBuildingProcess(self, argsList):
		CyCity, iType, = argsList
	'''


	def onCityRename(self, argsList):
		CyCity, = argsList
		if CyCity.getOwner() == GAME.getActivePlayer():
			self.__eventEditCityNameBegin((CyCity, True))


	'''
	def onCityHurry(self, argsList):
		CyCity, iHurryType = argsList
	'''


	def onVictory(self, argsList):
		iTeam, iVictory = argsList
		if iVictory >= 0 and iVictory < GC.getNumVictoryInfos():
			print "Victory!  Team %d achieves a %s victory" %(iTeam, GC.getVictoryInfo(iVictory).getDescription())


	def onVassalState(self, argsList):
		iMaster, iVassal, bVassal = argsList # iMaster & iVassal are Teams not Players.
		if bVassal:
			for iPlayerX in xrange(self.MAX_PC_PLAYERS):
				CyPlayerX = GC.getPlayer(iPlayerX)
				if CyPlayerX.getTeam() == iMaster and CyPlayerX.hasBuilding(GC.getInfoTypeForString("BUILDING_REICHSTAG")):
					CyPlayerX.changeGoldenAgeTurns(CyPlayerX.getGoldenAgeLength())


	def onGameUpdate(self, argsList):
		# Called on each game turn slice.
		# OOS Logger
		if self.bNetworkMP and CyInterface().isOOSVisible():
			import OOSLogger
			OOSLogger.writeLog()
			self.bNetworkMP = False


#################### TRIGGERED EVENTS ##################

	def __eventEditCityNameBegin(self, argsList):
		CyCity, bRename = argsList
		import ScreenResolution as SR
		xRes = SR.x
		if xRes > 2500:
			header = "<font=4b>"
			body = "\n<font=3>"
			w = 660
			h = 284
		elif xRes > 1700:
			header = "<font=3b>"
			body = "\n<font=2>"
			w = 500
			h = 172
		elif xRes > 1400:
			header = "<font=2b>"
			body = "\n<font=1>"
			w = 440
			h = 160
		else:
			header = "<font=1b>"
			body = "\n<font=0>"
			w = 380
			h = 136
		name = CyCity.getName()
		szTxt = header + TRNSLTR.getText("TXT_KEY_NAME_CITY", ()) + body + name

		popup = CyPopup(5000, EventContextTypes.EVENTCONTEXT_ALL, True)
		popup.setUserData((name, CyCity.getOwner(), CyCity.getID(), bRename))
		popup.setSize(w, h)
		popup.setPosition(xRes/2 - w/2, SR.y/2 - h/2)
		popup.setBodyString(szTxt, 1<<0)
		popup.createEditBox(name, 0)
		popup.setEditBoxMaxCharCount(24, 0, 0)
		popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

	def __eventEditCityNameApply(self, iPlayer, userData, popupReturn):
		oldName = userData[0]
		newName = popupReturn.getEditBoxString(0)
		if oldName != newName:
			GC.getPlayer(userData[1]).getCity(userData[2]).setName(newName, not userData[3])

			if GAME.GetWorldBuilderMode() and not GAME.isInAdvancedStart():
				import CvScreenEnums
				screen = CyGInterfaceScreen("WBCityEditScreen", CvScreenEnums.WB_CITYEDIT)
				screen.setText("CityName", "", CyTranslator().getText("[COLOR_SELECTED_TEXT]", ()) + "<font=4b>" + newName, 1<<2, screen.getXResolution()/2, 20, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, 0, 1)


	def __eventEditUnitNameBegin(self, pUnit):
		popup = CyPopup(5006, EventContextTypes.EVENTCONTEXT_ALL, True)
		popup.setUserData((pUnit.getOwner(), pUnit.getID()))
		popup.setBodyString(TRNSLTR.getText("TXT_KEY_RENAME_UNIT", ()), 1<<0)
		popup.createEditBox(pUnit.getNameNoDesc(), 0)
		popup.setEditBoxMaxCharCount(24, 0, 0)
		popup.launch(True, PopupStates.POPUPSTATE_IMMEDIATE)

	def __eventEditUnitNameApply(self, iPlayer, userData, popupReturn):
		unit = GC.getPlayer(userData[0]).getUnit(userData[1])
		newName = popupReturn.getEditBoxString(0)
		unit.setName(newName)

	def __eventWBPlayerScriptPopupApply(self, playerID, userData, popupReturn):
		GC.getPlayer(userData[0]).setScriptData(TextUtil.convertToStr(popupReturn.getEditBoxString(0)))
		WBPlayerScreen.WBPlayerScreen(CvScreensInterface.worldBuilderScreen).placeScript()

	def __eventWBCityScriptPopupApply(self, playerID, userData, popupReturn):
		GC.getPlayer(userData[0]).getCity(userData[1]).setScriptData(TextUtil.convertToStr(popupReturn.getEditBoxString(0)))

		if GAME.GetWorldBuilderMode() and not GAME.isInAdvancedStart():
			import WBCityEditScreen
			WBCityEditScreen.WBCityEditScreen(CvScreensInterface.worldBuilderScreen).placeScript()

	def __eventWBUnitScriptPopupApply(self, playerID, userData, popupReturn):
		GC.getPlayer(userData[0]).getUnit(userData[1]).setScriptData(TextUtil.convertToStr(popupReturn.getEditBoxString(0)))

	def __eventWBPlotScriptPopupApply(self, playerID, userData, popupReturn):
		GC.getMap().plot(userData[0], userData[1]).setScriptData(TextUtil.convertToStr(popupReturn.getEditBoxString(0)))
		WBPlotScreen.WBPlotScreen(CvScreensInterface.worldBuilderScreen).placeScript()

	def __eventWBLandmarkPopupApply(self, playerID, userData, popupReturn):
		sScript = popupReturn.getEditBoxString(0)
		pPlot = GC.getMap().plot(userData[0], userData[1])
		iPlayer = userData[2]
		if userData[3] > -1:
			iPlayer = CyEngine().getSignByIndex(userData[3]).getPlayerType()
			CyEngine().removeSign(pPlot, iPlayer)
		if sScript:
			if iPlayer >= self.MAX_PC_PLAYERS:
				CyEngine().addLandmark(pPlot, TextUtil.convertToStr(sScript))
			else:
				CyEngine().addSign(pPlot, iPlayer, TextUtil.convertToStr(sScript))
		WBPlotScreen.iCounter = 10
