## AttitudeUtil
##
## Utility functions for dealing with AI Attitudes.
##
## Notes
##   - Must be initialized externally by calling init()
##
## Copyright(c) 2008 The BUG Mod.
##
## Author: Ruff_hi, EmperorFool

from CvPythonExtensions import *
import BugUtil
import re

GC = CyGlobalContext()
GAME = GC.getGame()

# These two must be the same length
ATTITUDE_ICONS = [unichr(8876), unichr(8877), unichr(8878), unichr(8879), unichr(8880)]
ATTITUDE_COLORS = ["COLOR_RED", "COLOR_CYAN", "COLOR_CLEAR", "COLOR_GREEN", "COLOR_YELLOW"]

ATTITUDE_MODIFIERS = (
	"TXT_KEY_MISC_ATTITUDE_LAND_TARGET",
	"TXT_KEY_MISC_ATTITUDE_WAR",
	"TXT_KEY_MISC_ATTITUDE_PEACE",
	"TXT_KEY_MISC_ATTITUDE_SAME_RELIGION",
	"TXT_KEY_MISC_ATTITUDE_DIFFERENT_RELIGION",
	"TXT_KEY_MISC_ATTITUDE_BONUS_TRADE",
	"TXT_KEY_MISC_ATTITUDE_OPEN_BORDERS",
	"TXT_KEY_MISC_ATTITUDE_DEFENSIVE_PACT",
	"TXT_KEY_MISC_ATTITUDE_RIVAL_DEFENSIVE_PACT",
	"TXT_KEY_MISC_ATTITUDE_RIVAL_VASSAL",
	"TXT_KEY_MISC_ATTITUDE_SHARE_WAR",
	"TXT_KEY_MISC_ATTITUDE_FAVORITE_CIVIC",
	"TXT_KEY_MISC_ATTITUDE_TRADE",
	"TXT_KEY_MISC_ATTITUDE_RIVAL_TRADE",
	"TXT_KEY_MISC_ATTITUDE_FREEDOM",
	"TXT_KEY_MISC_ATTITUDE_EXTRA_GOOD",
	"TXT_KEY_MISC_ATTITUDE_EXTRA_BAD"
)
MODIFIER_STRING_TO_KEY = None

## Initialization
def init():
	"""Initializes this module, raising ConfigError if any problems occur."""
	# convert colors to type IDs
	global ATTITUDE_COLORS
	ATTITUDE_COLORS = map(GC.getInfoTypeForString, ATTITUDE_COLORS)
	initModifiers()


## Attitude

def hasAttitude(nPlayer, nTarget):
	"""Returns True if nTarget can see nPlayer's attitude toward them."""
	if -1 not in (nPlayer, nTarget) and nPlayer != nTarget:
		return GC.getTeam(GC.getPlayer(nPlayer).getTeam()).isHasMet(GC.getPlayer(nTarget).getTeam())
	return False

def getAttitudeString(nPlayer, nTarget):
	"""Returns the full hover text with attitude modifiers nPlayer has toward nTarget."""
	if hasAttitude(nPlayer, nTarget):
		return CyGameTextMgr().getAttitudeString(nPlayer, nTarget)
	return None

def getAttitudeCategory(nPlayer, nTarget):
	"""Returns the attitude level nPlayer has toward nTarget [0,4]."""
	if hasAttitude(nPlayer, nTarget):
		return GC.getPlayer(nPlayer).AI_getAttitude(nTarget)
	return None

def getAttitudeColor(nPlayer, nTarget):
	"""Returns the color of the attitude nPlayer has toward nTarget."""
	iCategory = getAttitudeCategory(nPlayer, nTarget)
	if iCategory > -1:
		return ATTITUDE_COLORS[iCategory]
	return -1

def getAttitudeIcon(nPlayer, nTarget):
	"""Returns the font icon of the attitude nPlayer has toward nTarget."""
	iCategory = getAttitudeCategory(nPlayer, nTarget)
	if iCategory > -1:
		return ATTITUDE_ICONS[iCategory]
	return ""

def getAttitudeCount(nPlayer, nTarget):
	"""Returns the total attitude modifiers nPlayer has toward nTarget."""
	sAttStr = getAttitudeString(nPlayer, nTarget)
	if sAttStr == None:
		return
	nAtt = 0
	ltPlusAndMinuses = re.findall("[-+][0-9]+\s?: ", sAttStr)
	for i in range(len(ltPlusAndMinuses)):
		nAtt += int(ltPlusAndMinuses[i][:-2])
	return nAtt


def getAttitudeText(nPlayer, nTarget, bNumber, bSmily, bWorstEnemy, bWarPeace):
	"""Returns a string describing the attitude nPlayer has toward nTarget."""
	nAttitude = getAttitudeCount(nPlayer, nTarget)
	if nAttitude == None:
		return None

	szText = []
	if bSmily:
		szText.append(getAttitudeIcon(nPlayer, nTarget))
	if bNumber:
		szText.append(BugUtil.colorText(u"%+d" % nAttitude, getAttitudeColor(nPlayer, nTarget)))

	szIcons = ""
	pPlayer = GC.getPlayer(nPlayer)
	pTarget = GC.getPlayer(nTarget)
	if bWorstEnemy and isWorstEnemy(pPlayer, pTarget):
		szIcons += unichr(8851)

	if bWarPeace:
		nTeam = pPlayer.getTeam()
		pTeam = GC.getTeam(nTeam)
		nTargetTeam = pTarget.getTeam()
		pTargetTeam = GC.getTeam(nTargetTeam)
		if pTeam.isAtWar(nTargetTeam):
			szIcons += unichr(8525)
		elif GAME.getActiveTeam() in (nTeam, nTargetTeam):
			bPeace = False
			if pTeam.isForcePeace(nTargetTeam):
				bPeace = True
			elif pTargetTeam.isAVassal():
				for nOwnerTeam in range(GC.getMAX_PC_TEAMS()):
					if pTargetTeam.isVassal(nOwnerTeam) and pTeam.isForcePeace(nOwnerTeam):
						bPeace = True
						break
			if bPeace:
				szIcons += unichr(8526)
	if szIcons:
		szText.append(szIcons)

	return u" ".join(szText)

def initModifiers(argsList=None):
	""" Creates the dictionary that maps strings to modifier keys. """
	global MODIFIER_STRING_TO_KEY
	MODIFIER_STRING_TO_KEY = {}
	for sKey in ATTITUDE_MODIFIERS:
		sStr = BugUtil.getPlainText(sKey, "NONE")
		if sStr != "NONE":
			# These modifier strings should contain the '%d: ' prefix
			# so we need to extract just the portion in quotes.
			pMatch = re.match(u"^.*(\".+\")", sStr, re.UNICODE)
			if pMatch:
				MODIFIER_STRING_TO_KEY[unicode(pMatch.group(1))] = sKey
	for iMemType in range(MemoryTypes.NUM_MEMORY_TYPES):
		sKey = str(GC.getMemoryInfo(iMemType).getTextKey())
		sStr = BugUtil.getPlainText(sKey, "NONE")
		if sStr != "NONE":
			# These modifier strings have no extra text and so
			# we can use them directly
			MODIFIER_STRING_TO_KEY[unicode(sStr)] = sKey


class Attitude:
	""" Holds summary of attitude that this player has toward target player.

	If the two IDs are the same or the two players have not met, the
	class will be filled with generic default values. We also check to see
	that the active player has met both players, and the accessor functions
	will react differently depending on if this is the case.
	"""
	def __init__(self, iThisPlayer, iTargetPlayer):
		pActiveTeam = GC.getTeam(GC.getActivePlayer().getTeam())
		iThisTeam = GC.getPlayer(iThisPlayer).getTeam()
		pThisTeam = GC.getTeam(iThisTeam)
		iTargetTeam = GC.getPlayer(iTargetPlayer).getTeam()
		self.iThisPlayer = iThisPlayer
		self.iTargetPlayer = iTargetPlayer
		self.iAttitudeSum = 0
		self.iAttitudeModifiers = {}
		self.bHasActiveMetBoth = GAME.isDebugMode() or (pActiveTeam.isHasMet(iThisTeam) and pActiveTeam.isHasMet(iTargetTeam))
		self.eAttitudeType = AttitudeTypes.NO_ATTITUDE
		# This might be better off being something descriptive such as
		# "players have not met" or "players are the same"
		self.sAttitudeString = ""
		if iThisPlayer != iTargetPlayer and pThisTeam.isHasMet(iTargetTeam):
			self.eAttitudeType = GC.getPlayer(iThisPlayer).AI_getAttitude(iTargetPlayer)
			self.sAttitudeString = CyGameTextMgr().getAttitudeString(iThisPlayer, iTargetPlayer)
			for sLine in self.sAttitudeString.split("\n"):
				pMatch = re.match("^.*>([-\+]\d+)\s?:\s+(\".+\")<.*$", sLine, re.UNICODE)
				if pMatch:
					iValue = int(pMatch.group(1))
					sString = unicode(pMatch.group(2))
					self.iAttitudeSum += iValue
					if sString in MODIFIER_STRING_TO_KEY:
						self.iAttitudeModifiers[MODIFIER_STRING_TO_KEY[sString]] = iValue
					else:
						print "Attitude::init() - Attitude string contains unknown modifier text: " + sString

	def __str__(self):
		""" String representation of class instance. """
		return(u"Attitude { 'iThisPlayer': %d, 'iTargetPlayer': %d, 'iAttitudeSum': %d, 'eAttitudeType': %d, 'bHasActiveMetBoth': %s,\n 'iAttitudeModifiers': %s,\n 'sAttitudeString': %s }"
				%(self.iThisPlayer, self.iTargetPlayer, self.iAttitudeSum, self.eAttitudeType,
				   self.bHasActiveMetBoth, str(self.iAttitudeModifiers), self.sAttitudeString))

	def hasModifier(self, sKey):
		""" Does the attitude contain given modifier? """
		if self.bHasActiveMetBoth:
			return(sKey in self.iAttitudeModifiers)
		return False

	def getModifier(self, sKey):
		""" Returns integer value of given attitude modifer. """
		if self.bHasActiveMetBoth:
			if sKey in self.iAttitudeModifiers:
				return self.iAttitudeModifiers[sKey]
		return 0

	def hasAttitude(self):
		""" Does this player have any attitude toward the target? """
		if self.bHasActiveMetBoth:
			return(not self.eAttitudeType == AttitudeTypes.NO_ATTITUDE)
		return False

	def getCount(self):
		""" Returns total(visible) count of attitude modifiers. """
		if self.bHasActiveMetBoth:
			return self.iAttitudeSum
		return 0

	def getCategory(self):
		""" Returns attitude category as an AttitudeTypes enum. """
		if self.bHasActiveMetBoth:
			return self.eAttitudeType
		return AttitudeTypes.NO_ATTITUDE

	def getString(self):
		""" Returns full diplomacy text string. """
		if self.bHasActiveMetBoth:
			return self.sAttitudeString
		return ""

	def getIcon(self):
		""" Returns smilie icon string based on attitude type. """
		if self.bHasActiveMetBoth:
			iCategory = int(self.getCategory())
			if iCategory > -1:
				return ATTITUDE_ICONS[iCategory]
		return ""

	def getColor(self):
		""" Returns the color of the attitude this player has toward target. """
		if self.bHasActiveMetBoth:
			iCategory = int(self.getCategory())
			if iCategory > -1:
				return ATTITUDE_COLORS[iCategory]
		return -1

	def getText(self, bNumber, bSmily, bWorstEnemy, bWarPeace):
		""" Returns a string describing the attitude this player has toward target. """
		if self.bHasActiveMetBoth:
			nAttitude = self.getCount()

			if bNumber:
				szText = str(nAttitude)
				if nAttitude > 0:
					szText = "+" + szText
				if bSmily:
					szText = "[" + szText + "] "
				else:
					szText = "<font=3>   " + szText + "</font> "
			else:
				szText = ""

			iColor = self.getColor()
			szText = BugUtil.colorText(szText, iColor)
			if bSmily:
				szText = self.getIcon() + " " + szText

			pThisPlayer = GC.getPlayer(self.iThisPlayer)
			pTargetPlayer = GC.getPlayer(self.iTargetPlayer)
			if bWorstEnemy and isWorstEnemy(pThisPlayer, pTargetPlayer):
				szText +=  unichr(8867)

			if bWarPeace:
				iThisTeam = pThisPlayer.getTeam()
				pThisTeam = GC.getTeam(iThisTeam)
				iTargetTeam = pTargetPlayer.getTeam()
				pTargetTeam = GC.getTeam(iTargetTeam)
				if pThisTeam.isAtWar(iTargetTeam):
					szText += u"%c" %(GC.getCommerceInfo(CommerceTypes.COMMERCE_GOLD).getChar() + 25)
				elif GAME.getActiveTeam() in (iThisTeam, iTargetTeam):
					bPeace = False
					if pThisTeam.isForcePeace(iTargetTeam):
						bPeace = True
					elif pTargetTeam.isAVassal():
						for iOwnerTeam in range(GC.getMAX_PC_TEAMS()):
							if pTargetTeam.isVassal(iOwnerTeam) and pThisTeam.isForcePeace(iOwnerTeam):
								bPeace = True
								break
					if bPeace:
						szText += u"%c" %(GC.getCommerceInfo(CommerceTypes.COMMERCE_GOLD).getChar() + 26)

			return szText
		return ""


def isWorstEnemy(player, enemy):
	worstEnemyName = player.getWorstEnemyName()
	return worstEnemyName and worstEnemyName == GC.getTeam(enemy.getTeam()).getName()


def getWorstEnemyTeams():
	"""
	Returns a dictionary of the team IDs that are each team's worst enemy.

	The key is team ID; the value is the worst enemy team ID.
	If a team has no worst enemy, -1 is stored as its value.
	Ignores dead, human, barbarian, and minor teams.

	Loops over players because CyTeam does not have getWorstEnemyName().
	"""
	namesToID = {}
	for iTeam in xrange(GC.getMAX_PC_TEAMS()):
		CyTeam = GC.getTeam(iTeam)
		if CyTeam.isAlive() and not CyTeam.isMinorCiv():
			namesToID[CyTeam.getName()] = iTeam

	enemies = {}
	for name, iTeam in namesToID.items():
		worstEnemyName = GC.getPlayer(GC.getTeam(iTeam).getLeaderID()).getWorstEnemyName()
		if worstEnemyName:
			if worstEnemyName in namesToID:
				enemies[iTeam] = namesToID[worstEnemyName]
			else:
				BugUtil.debug("[INFO] Cannot find worst enemy of %s; worst enemy is %s", name, worstEnemyName)
				enemies[iTeam] = -1
		else:
			enemies[iTeam] = -1
	return enemies
