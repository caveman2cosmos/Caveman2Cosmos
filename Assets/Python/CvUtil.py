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

# if the string is non unicode, convert it to unicode by decoding it using utf-8
def convertToUnicode(s):
	if isinstance(s, str):
		return s.decode("iso8859")
	return s

# if the string is unicode, convert it to str by encoding it using utf-8
def convertToStr(txt):
	if isinstance(txt, unicode):
		i = 0
		length = len(txt)
		while i < length:
			ordinal = ord(txt[i])
			if ordinal > 255:
				txt = txt[:i] + '?' + txt[i+1:]
			i += 1
		 # Toffer - "iso8859" = "latin-1". Tried UTF-8 here, caused problem for german characters like "ß".
		return txt.encode("iso8859")
	return txt

# Used to reduce text to ascii, exe enforce ascii in some cases.
def convertToAscii(txt):
	txt = convertToStr(txt)
	# convert to ascii equivalent where possible.
	accent = [
		('à', 'a'), ('ä', 'a'), ('â', 'a'),
		('é', 'e'), ('è', 'e'), ('ê', 'e'),
		('ù', 'u'), ('û', 'u'), ('ü', 'u'),
		('ô', 'o'), ('õ', 'o'), ('ö', 'o'),
		('ç', 'c'), ('î', 'i'), ('ï', 'i'),
		('ß', 'ss')
	]
	while accent:
		a, b = accent.pop()
		txt = txt.replace(a, b)
	# get rid of any "above ascii ordinals" that may be left here.
	i = 0
	length = len(txt)
	while i < length:
		ordinal = ord(txt[i])
		if ordinal > 128:
			txt = txt[:i] + '?' + txt[i+1:]
		i += 1
	return txt

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

def pyPrint(stuff):
	sys.stdout.write('PY:' + stuff + "\n")

def getOppositeCardinalDirection(dir):
	return (dir + 2) % CardinalDirectionTypes.NUM_CARDINALDIRECTION_TYPES

def shuffle(num, rand):
	"returns a tuple of size num of shuffled numbers"
	piShuffle = [0]*num
	shuffleList(num, rand, piShuffle)	# implemented in C for speed
	return piShuffle

def spawnUnit(iUnit, pPlot, pPlayer):
	pPlayer.initUnit(iUnit, pPlot.getX(), pPlot.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.NO_DIRECTION)
	return 1

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

def stripLiterals(txt, literal):
	# The literal argument can be: "font", "color", "link", etc. Caps lock does matter.
	start = "<%s=" % literal
	txt = txt.replace("</%s>" % literal, "")
	i1 = txt.find(start)
	if i1 > -1:
		while i1 > -1:
			i2 = txt.find(">", i1)
			txt = txt[:i1] + txt[i2+1:]
			i1 = txt.find(start)
	return txt

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
