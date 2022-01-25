#
# CvUtil
#
import sys # for file ops

# For Civ game code access
from CvPythonExtensions import *
import ScreenResolution as SR

# globals
GC = CyGlobalContext()
CyIF = CyInterface()
TRNSLTR = CyTranslator()

# Event IDs
g_nextEventID = 9999
def getNewEventID():
	"""
	Defines a new event and returns its unique ID
	to be passed to BugEventManager.beginEvent(id).

	Perhaps move over to CvEventManager.py
	"""
	global g_nextEventID
	g_nextEventID += 1
	return g_nextEventID

# Screen IDs
BUG_FIRST_SCREEN = 1000
g_nextScreenID = BUG_FIRST_SCREEN
def getNewScreenID():
	global g_nextScreenID
	id = g_nextScreenID
	g_nextScreenID += 1
	return id

# Popup defines
FONT_CENTER_JUSTIFY=1<<2
FONT_RIGHT_JUSTIFY=1<<1
FONT_LEFT_JUSTIFY=1<<0


class RedirectDebug:
	"""Send Debug Messages to Civ Engine"""
	def __init__(self):
		self.m_PythonMgr = CyPythonMgr()
	def write(self, stuff):
		# if str is non unicode and contains encoded unicode data, supply the right encoder to encode it into a unicode object
		if isinstance(stuff, unicode):
			self.m_PythonMgr.debugMsgWide(stuff)
		else:
			self.m_PythonMgr.debugMsg(stuff)

class RedirectError:
	"""Send Error Messages to Civ Engine"""
	def __init__(self):
		self.m_PythonMgr = CyPythonMgr()
	def write(self, stuff):
		# if str is non unicode and contains encoded unicode data, supply the right encoder to encode it into a unicode object
		if isinstance(stuff, unicode):
			self.m_PythonMgr.errorMsgWide(stuff)
		else:
			self.m_PythonMgr.errorMsg(stuff)

def myExceptHook(type, value, tb):
	import traceback # for error reporting
	lines = traceback.format_exception(type, value, tb)
	sys.stderr.write("\n".join(lines))

def combatDetailMessageBuilder(cdUnit, ePlayer, iChange):
	if cdUnit.iExtraCombatPercent:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_EXTRA_COMBAT_PERCENT",(cdUnit.iExtraCombatPercent * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iAnimalCombatModifierTA:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_ANIMAL_COMBAT",(cdUnit.iAnimalCombatModifierTA * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iAIAnimalCombatModifierTA:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_AI_ANIMAL_COMBAT",(cdUnit.iAIAnimalCombatModifierTA * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iAnimalCombatModifierAA:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_ANIMAL_COMBAT",(cdUnit.iAnimalCombatModifierAA * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iAIAnimalCombatModifierAA:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_AI_ANIMAL_COMBAT",(cdUnit.iAIAnimalCombatModifierAA * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iBarbarianCombatModifierTB:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_BARBARIAN_COMBAT",(cdUnit.iBarbarianCombatModifierTB * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iAIBarbarianCombatModifierTB:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_BARBARIAN_AI_COMBAT",(cdUnit.iAIBarbarianCombatModifierTB * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iBarbarianCombatModifierAB:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_BARBARIAN_COMBAT",(cdUnit.iBarbarianCombatModifierAB * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iAIBarbarianCombatModifierAB:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_BARBARIAN_AI_COMBAT",(cdUnit.iAIBarbarianCombatModifierAB * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iPlotDefenseModifier:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_PLOT_DEFENSE",(cdUnit.iPlotDefenseModifier * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iFortifyModifier:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_FORTIFY",(cdUnit.iFortifyModifier * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iCityDefenseModifier:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_CITY_DEFENSE",(cdUnit.iCityDefenseModifier * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iHillsAttackModifier:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_HILLS_ATTACK",(cdUnit.iHillsAttackModifier * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iHillsDefenseModifier:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_HILLS",(cdUnit.iHillsDefenseModifier * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iFeatureAttackModifier:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_FEATURE_ATTACK",(cdUnit.iFeatureAttackModifier * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iFeatureDefenseModifier:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_FEATURE",(cdUnit.iFeatureDefenseModifier * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iTerrainAttackModifier:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_TERRAIN_ATTACK",(cdUnit.iTerrainAttackModifier * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iTerrainDefenseModifier:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_TERRAIN",(cdUnit.iTerrainDefenseModifier * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iCityAttackModifier:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_CITY_ATTACK",(cdUnit.iCityAttackModifier * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iDomainDefenseModifier:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_DOMAIN_DEFENSE",(cdUnit.iDomainDefenseModifier * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iCityBarbarianDefenseModifier:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_CITY_BARBARIAN_DEFENSE",(cdUnit.iCityBarbarianDefenseModifier * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iDefenseModifier:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_CLASS_DEFENSE",(cdUnit.iDefenseModifier * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iAttackModifier:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_CLASS_ATTACK",(cdUnit.iAttackModifier * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iCombatModifierT:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_CLASS_COMBAT",(cdUnit.iCombatModifierT * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iCombatModifierA:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_CLASS_COMBAT",(cdUnit.iCombatModifierA * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iDomainModifierA:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_CLASS_DOMAIN",(cdUnit.iDomainModifierA * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iDomainModifierT:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_CLASS_DOMAIN",(cdUnit.iDomainModifierT * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iAnimalCombatModifierA:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_CLASS_ANIMAL_COMBAT",(cdUnit.iAnimalCombatModifierA * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iAnimalCombatModifierT:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_CLASS_ANIMAL_COMBAT",(cdUnit.iAnimalCombatModifierT * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iRiverAttackModifier:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_CLASS_RIVER_ATTACK",(cdUnit.iRiverAttackModifier * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

	if cdUnit.iAmphibAttackModifier:
		msg=TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_CLASS_AMPHIB_ATTACK",(cdUnit.iAmphibAttackModifier * iChange,))
		CyIF.addCombatMessage(ePlayer,msg)

def combatMessageBuilder(cdAttacker, cdDefender, iCombatOdds):
	combatMessage = ""
	if cdAttacker.eOwner == cdAttacker.eVisualOwner:
		combatMessage += "%s's " %(GC.getPlayer(cdAttacker.eOwner).getName(),)
	combatMessage += "%s (%.2f)" %(cdAttacker.sUnitName,cdAttacker.iCurrCombatStr/100.0,)
	combatMessage += " " + TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_VS", ()) + " "
	if cdDefender.eOwner == cdDefender.eVisualOwner:
		combatMessage += "%s's " %(GC.getPlayer(cdDefender.eOwner).getName(),)
	combatMessage += "%s (%.2f)" %(cdDefender.sUnitName,cdDefender.iCurrCombatStr/100.0,)
	CyIF.addCombatMessage(cdAttacker.eOwner,combatMessage)
	CyIF.addCombatMessage(cdDefender.eOwner,combatMessage)
	combatMessage = "%s %.1f%%" %(TRNSLTR.getText("TXT_KEY_COMBAT_MESSAGE_ODDS", ()),iCombatOdds/10.0,)
	CyIF.addCombatMessage(cdAttacker.eOwner,combatMessage)
	CyIF.addCombatMessage(cdDefender.eOwner,combatMessage)
	combatDetailMessageBuilder(cdAttacker,cdAttacker.eOwner,-1)
	combatDetailMessageBuilder(cdDefender,cdAttacker.eOwner,1)
	combatDetailMessageBuilder(cdAttacker,cdDefender.eOwner,-1)
	combatDetailMessageBuilder(cdDefender,cdDefender.eOwner,1)


# Centralized function for displaying messages in the message box.
def sendMessage(szTxt, iPlayer=None, iTime=16, szIcon=None, eColor=-1, iMapX=-1, iMapY=-1, bOffArrow=False, bOnArrow=False, eMsgType=0, szSound=None, bForce=True):
	if szTxt:
		if iPlayer is None:
			iPlayer = GC.getGame().getActivePlayer()
		if iPlayer == -1: return

		if GC.getGame().getAIAutoPlay(iPlayer):
			szIcon = None
			iMapX = iMapY = iTime = -1
			bForce = bOffArrow = bOnArrow = False

		CyIF.addMessage(iPlayer, bForce, iTime, SR.aFontList[5] + szTxt, szSound, eMsgType, szIcon, eColor, iMapX, iMapY, bOffArrow, bOnArrow)

def sendImmediateMessage(szTxt, szSound=None):
	if szTxt:
		CyIF.addImmediateMessage(SR.aFontList[5] + szTxt, szSound)
