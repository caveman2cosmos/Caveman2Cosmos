## TraitUtil
##
## Utilities for dealing with Traits and TraitInfos.
##
## Notes
##   - Must be initialized externally by calling init()
##
## Copyright (c) 2008 The BUG Mod.
##
## Author: EmperorFool

from CvPythonExtensions import *

GENERIC_ICON = "*"
TRAIT_ICONS = {}

GENERIC_BUTTON = "Art/Interface/Buttons/TechTree/"
TRAIT_BUTTONS = {}

gc = CyGlobalContext()

def init():
	"Performs one-time initialization after the game starts up."
	game = gc.getGame()
	global GENERIC_ICON
	GENERIC_ICON = u"%c" % game.getSymbolID(FontSymbols.MAP_CHAR)

	addTrait("AGGRESSIVE", game.getSymbolID(FontSymbols.STRENGTH_CHAR), "Art/Interface/Buttons/Promotions/Combat1.dds")
	addTrait("CHARISMATIC", game.getSymbolID(FontSymbols.HAPPY_CHAR), "Art/Interface/Buttons/TechTree/MassMedia.dds")
	addTrait("CREATIVE", gc.getCommerceInfo(CommerceTypes.COMMERCE_CULTURE).getChar(), "Art/Interface/Buttons/TechTree/Music.dds")
	addTrait("EXPANSIVE", game.getSymbolID(FontSymbols.HEALTHY_CHAR), "Art/Interface/Buttons/Actions/Heal.dds")
	addTrait("FINANCIAL", gc.getCommerceInfo(CommerceTypes.COMMERCE_GOLD).getChar(), "Art/Interface/Buttons/TechTree/Banking.dds")
	addTrait("IMPERIALIST", game.getSymbolID(FontSymbols.OCCUPATION_CHAR), "Art/Interface/Buttons/Actions/FoundCity.dds")
	addTrait("INDUSTRIOUS", gc.getYieldInfo(YieldTypes.YIELD_PRODUCTION).getChar(), "Art/Interface/Buttons/TechTree/Industrialism.dds")
	addTrait("ORGANIZED", game.getSymbolID(FontSymbols.TRADE_CHAR), "Art/Interface/Buttons/Buildings/Courthouse.dds")
	addTrait("PHILOSOPHICAL", game.getSymbolID(FontSymbols.GREAT_PEOPLE_CHAR), "Art/Interface/Buttons/TechTree/Philosophy.dds")
	addTrait("PROTECTIVE", game.getSymbolID(FontSymbols.DEFENSE_CHAR), "Art/Interface/Buttons/Promotions/CityGarrison1.dds")
	addTrait("SPIRITUAL", game.getSymbolID(FontSymbols.RELIGION_CHAR), "Art/Interface/Buttons/TechTree/Meditation.dds")
	addTrait("NOMAD", game.getSymbolID(FontSymbols.MOVES_CHAR), "Art/Interface/Buttons/Units/HorseArcher.dds")
	addTrait("AGRICULTURAL", gc.getYieldInfo(YieldTypes.YIELD_FOOD).getChar(), "Art/Interface/Buttons/TechTree/Agriculture.dds")
	addTrait("SEAFARING", game.getSymbolID(FontSymbols.MAP_CHAR), "Art/Interface/Buttons/TechTree/seafaring.dds")
	addTrait("DECEIVER", gc.getCommerceInfo(CommerceTypes.COMMERCE_ESPIONAGE).getChar(), "Art/Interface/Buttons/Units/Spy.dds")
	addTrait("SCIENTIFIC", gc.getCommerceInfo(CommerceTypes.COMMERCE_RESEARCH).getChar(), "Art/Interface/Buttons/Process/ProcessResearch.dds")
	addTrait("HUMANITARIAN", game.getSymbolID(FontSymbols.HEALTHY_CHAR), "Art/Interface/Buttons/Actions/Heal.dds")
	addTrait("PROGRESSIST", game.getSymbolID(FontSymbols.GREAT_PEOPLE_CHAR), "Art/Interface/Buttons/TechTree/enlighten1.dds")
	addTrait("POLITICIAN", game.getSymbolID(FontSymbols.HAPPY_CHAR), "Art/Interface/Buttons/actions/steal_tech.dds")
	addTrait("ANTI_CLERICAL", game.getSymbolID(FontSymbols.UNHAPPY_CHAR), "Art/Interface/Buttons/Civics/secular1.dds")
	addTrait("CRUEL", game.getSymbolID(FontSymbols.UNHAPPY_CHAR), "Art/Interface/Buttons/Civics/ruleoffear.dds")
	addTrait("IDEALISTIC", game.getSymbolID(FontSymbols.UNHAPPY_CHAR), ",Art/Interface/Buttons/Civics/Pacifism.dds,Art/Interface/Buttons/Civics_Civilizations_Religions_Atlas.dds,3,2")
	addTrait("REVOLUTIONARY", game.getSymbolID(FontSymbols.UNHAPPY_CHAR), "Art/Interface/Buttons/TechTree/labor_movement.dds")
	addTrait("MEGALOMANIAC", game.getSymbolID(FontSymbols.UNHAPPY_CHAR), "Art/Interface/Buttons/TechTree/sculpture.dds")
	addTrait("BARBARIC", game.getSymbolID(FontSymbols.UNHAPPY_CHAR), "Art/Interface/Buttons/Units/cannibal.dds")
	addTrait("ISOLATIONIST", game.getSymbolID(FontSymbols.UNHAPPY_CHAR), "Art/Interface/Buttons/Buildings/toll_house.dds")
	addTrait("FANATICAL", game.getSymbolID(FontSymbols.UNHAPPY_CHAR), "Art/Interface/Buttons/TechTree/theocracy.dds")
	addTrait("POPULIST", game.getSymbolID(FontSymbols.UNHAPPY_CHAR), "Art/Interface/Buttons/Civics/hiredlabor.dds")
	addTrait("EXCESSIVE", game.getSymbolID(FontSymbols.UNHAPPY_CHAR), ",Art/Interface/Buttons/Buildings/Versailles.dds,Art/Interface/Buttons/Buildings_Atlas.dds,7,7")
	addTrait("FOREIGN", game.getSymbolID(FontSymbols.UNHAPPY_CHAR), ",Art/Interface/Buttons/TechTree/Constitution.dds,Art/Interface/Buttons/TechTree_Atlas.dds,8,2")
	addTrait("TEMPERAMENTAL", game.getSymbolID(FontSymbols.UNHAPPY_CHAR), "Art/Interface/Buttons/TechTree/explosives.dds")
	addTrait("HUNTER_GATHERER", gc.getYieldInfo(YieldTypes.YIELD_FOOD).getChar(), ",Art/Interface/Buttons/TechTree/Archery.dds,Art/Interface/Buttons/TechTree_Atlas.dds,4,1")
	addTrait("BARBARIAN", game.getSymbolID(FontSymbols.OCCUPATION_CHAR), ",Art/Interface/Buttons/Civilizations/Barbarian.dds,Art/Interface/Buttons/Civics_Civilizations_Religions_Atlas.dds,4,5")

# Rise of Mankind 2.6 - new traits

def addTrait(trait, icon, button):
	eTrait = gc.getInfoTypeForString("TRAIT_" + trait)
	if eTrait != -1:
		if icon is not None:
			TRAIT_ICONS[eTrait] = u"%c" % icon
		if button is not None:
			TRAIT_BUTTONS[eTrait] = button


def getIcon(eTrait):
	if eTrait in TRAIT_ICONS:
		return TRAIT_ICONS[eTrait]
	else:
		return GENERIC_ICON

def getButton(eTrait):
	if eTrait in TRAIT_BUTTONS:
		return TRAIT_BUTTONS[eTrait]
	else:
		return GENERIC_BUTTON
