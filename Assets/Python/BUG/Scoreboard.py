## Scoreboard
##
## Holds the information used to display the scoreboard.
##
## Notes
##   - Must be initialized externally by calling init()
##   - Add 'DealCanceled' event for onDealCanceled()
##
## Copyright (c) 2007-2008 The BUG Mod.
##
## Author: EmperorFool

from CvPythonExtensions import *
import DealUtil
import re

# Globals
GC = CyGlobalContext()

# Columns IDs
NUM_PARTS = 26
(
	ALIVE,
	WAR,
	SCORE,
	SCORE_DELTA,
	RANK,
	ID,
	MASTER,
	NAME,
	NOT_MET,
	POWER,
	RESEARCH,
	RESEARCH_TURNS,
	ESPIONAGE,
	TRADE,
	BORDERS,
	EMBASSY,
	PACT,
	RELIGION,
	ATTITUDE,
	WONT_TALK,
	WORST_ENEMY,
	WHEOOH,
	CITIES,
	WAITING,
	NET_STATS,
	OOS
) = range(NUM_PARTS)

# Column Definitions
columns = []
columnsByKey = {}

TRADE_TYPES = (
	TradeableItems.TRADE_OPEN_BORDERS,
	TradeableItems.TRADE_EMBASSY,
	TradeableItems.TRADE_DEFENSIVE_PACT,
	TradeableItems.TRADE_PERMANENT_ALLIANCE,
	TradeableItems.TRADE_PEACE_TREATY,
)

# Initializes the strings used to display the scoreboard.
def init():
	import BugCore
	global ScoreOpt, columns
	ScoreOpt = BugCore.game.Scores
	# Used keys:
	# ABCDEFHIKLMNOPQRSTUVWZ*?
	# GJXY
	columns.append(Column('', ALIVE))
	columns.append(Column('S', SCORE, 2))
	columns.append(Column('Z', SCORE_DELTA, 2))
	columns.append(Column('K', RANK, 2))
	columns.append(Column('I', ID, 2))
	columns.append(Column('V', MASTER, 2))
	columns.append(Column('C', NAME, 2))
	columns.append(Column('?', NOT_MET, 1, "?"))
	columns.append(Column('W', WAR, 2))
	columns.append(Column('P', POWER, 2))
	columns.append(Column('T', RESEARCH, 3))
	columns.append(Column('U', RESEARCH_TURNS, 2))
	columns.append(Column('E', ESPIONAGE, 1, unichr(8488)))
	columns.append(Column('N', TRADE, 1, unichr(8860)))
	columns.append(Column('B', BORDERS, 1, unichr(8868)))
	columns.append(Column('@', EMBASSY, 1, unichr(8870)))
	columns.append(Column('D', PACT, 1, unichr(8869)))
	columns.append(Column('R', RELIGION, 2))
	columns.append(Column('A', ATTITUDE, 2))
	columns.append(Column('F', WONT_TALK, 1, "!"))
	columns.append(Column('H', WORST_ENEMY, 1, unichr(8867)))
	columns.append(Column('M', WHEOOH, 1, unichr(8871)))
	columns.append(Column('Q', CITIES, 2))
	columns.append(Column('*', WAITING, 1, "*"))
	columns.append(Column('L', NET_STATS, 2))
	columns.append(Column('O', OOS, 2))

def onDealCanceled(argsList):
	# Sets the scoreboard dirty bit so it will redraw.
	CyInterface().setDirty(InterfaceDirtyBits.Score_DIRTY_BIT, True)


class Column:

	def __init__(self, key, id, type=0, text=None, alt=None):
		self.key = key
		self.id = id
		self.type = type
		self.text = text
		self.alt = alt
		if type == 1:
			self.width = CyInterface().determineWidth(text) + 2
		else:
			self.width = 0
		if key:
			columnsByKey[key] = self


class Scoreboard:
	# Holds and builds the ScoreCards.
	def __init__(self, iPlayerAct, font):
		global uFont
		uFont = font
		self._teamScores = []
		self._playerScores = []
		self._teamScoresByID = {}
		self._anyHas = [False] * NUM_PARTS
		self._currTeamScores = None
		self._currPlayerScore = None
		self._deals = DealUtil.findDealsByPlayerAndType(iPlayerAct, TRADE_TYPES)
		TeamScores.iPlayerAct = iPlayerAct

	def addTeam(self, team, iRank):
		self._currTeamScores = TeamScores(self, team, iRank)
		self._teamScores.append(self._currTeamScores)
		self._teamScoresByID[team.getID()] = self._currTeamScores
		self._currPlayerScore = None

	def getTeamScores(self, eTeam):
		return self._teamScoresByID.get(eTeam, None)

	def addPlayer(self, iPlayer, iRank):
		if self._currTeamScores:
			self._currPlayerScore = self._currTeamScores.addPlayer(iPlayer, iRank)
			self._playerScores.append(self._currPlayerScore)

	def size(self):
		return len(self._playerScores)

	def setAlive(self):
		self._set(ALIVE)

	def setMaster(self):
		self._set(MASTER, unichr(8859))

	def setMasterSelf(self):
		self._set(MASTER, unichr(8858))

	def setScore(self, value):
		self._set(SCORE, value)

	def setScoreDelta(self, value):
		self._set(SCORE_DELTA, value)

	def setRank(self, value):
		self._set(RANK, value)

	def setID(self, value):
		self._set(ID, value)

	def setName(self, value):
		self._set(NAME, value)

	def setNotMet(self):
		self._set(NOT_MET)

	def setWHEOOH(self):
		self._set(WHEOOH)

	def setNumCities(self, value):
		self._set(CITIES, value)

	def setWar(self):
		self._set(WAR, unichr(8525))

	def setPeace(self):
		self._set(WAR, unichr(8526), self._getDealWidget(TradeableItems.TRADE_PEACE_TREATY))

	def setPower(self, value):
		self._set(POWER, value)

	def setResearch(self, iTech, iTurns):
		self._set(RESEARCH, iTech)
		self._set(RESEARCH_TURNS, str(iTurns))

	def setEspionage(self):
		self._set(ESPIONAGE)

	def setTrade(self, iPlayerAct):
		self._set(TRADE, True, (WidgetTypes.WIDGET_TRADE_ROUTES, iPlayerAct, self._currPlayerScore.iPlayer))

	def setBorders(self):
		self._set(BORDERS, True, self._getDealWidget(TradeableItems.TRADE_OPEN_BORDERS))

	def setEmbassy(self):
		self._set(EMBASSY, True, self._getDealWidget(TradeableItems.TRADE_EMBASSY))

	def setPact(self):
		self._set(PACT, True, self._getDealWidget(TradeableItems.TRADE_DEFENSIVE_PACT))

	def setReligion(self, value):
		self._set(RELIGION, value)

	def setAttitude(self, value):
		self._set(ATTITUDE, value)

	def setWontTalk(self):
		self._set(WONT_TALK)

	def setWorstEnemy(self):
		self._set(WORST_ENEMY)

	def setWaiting(self):
		self._set(WAITING)

	def setNetStats(self, value):
		self._set(NET_STATS, value)

	def setOOS(self, value):
		self._set(OOS, value)

	def _getDealWidget(self, type):
		# lookup the Deal containing the given tradeable item type
		deals = self._deals.get(self._currPlayerScore.iPlayer, None)
		if deals:
			deal = deals.get(type, None)
			if deal:
				return (WidgetTypes.WIDGET_DEAL_KILL, deal.getID(), -1)
		return (WidgetTypes.WIDGET_DEAL_KILL, -1, -1)

	def _set(self, part, value=True, widget=None):
		self._anyHas[part] = True
		self._currPlayerScore.set(part, value, widget)

	def assignRanks(self):
		"""
		Assigns a rank from 1 to N based on score.
		As the player scores are currently reversed, this is done in reverse order.
		"""
		TRNSLTR = CyTranslator()
		iRank = 0
		scores = list(self._playerScores)
		scores.reverse()
		for playerScore in scores:
			if not playerScore._has[NOT_MET] or not playerScore._values[NOT_MET]:
				iRank += 1
				playerScore.set(RANK, TRNSLTR.changeTextColor(str(iRank), ScoreOpt.getRankColor()))
		if iRank > 0:
			self._anyHas[RANK] = True

	def sort(self):
		# Sorts the list by pulling any vassals up below their masters.
		if ScoreOpt.isGroupVassals():
			self._playerScores.sort(lambda x, y: cmp(x.sortKey(), y.sortKey()))
			self._playerScores.reverse()
		maxPlayers = ScoreOpt.getMaxPlayers()
		if maxPlayers > 0 and len(self._playerScores) > maxPlayers:
			self._playerScores = self._playerScores[len(self._playerScores) - maxPlayers:]

	def draw(self, screen, xRes, yRes):
		# Sorts and draws the scoreboard right-to-left, bottom-to-top.
		self.assignRanks()
		for teamScores in self._teamScores:
			teamScores.gatherVassals()
		self.sort()
		interface = CyInterface()
		eWidGen = WidgetTypes.WIDGET_GENERAL
		eWidContact = WidgetTypes.WIDGET_CONTACT_CIV

		bLeftAlignName		= ScoreOpt.isLeftAlignName()
		bGroupVassals		= ScoreOpt.isGroupVassals()

		x = xRes - 12 # start here and shift left with each column
		bNormalInterface = interface.getShowInterface() == InterfaceVisibility.INTERFACE_SHOW or interface.isInAdvancedStart()
		if bNormalInterface:
			y = yRes - 112 - yRes/10
		else:
			y = yRes - 88
		totalWidth = 0
		height = ScoreOpt.getLineHeight()
		techIconSize = ScoreOpt.getResearchIconSize()

		defaultSpacing = ScoreOpt.getDefaultSpacing()
		spacing = defaultSpacing
		format = re.findall('(-?[0-9]+|[^0-9])', ScoreOpt.getDisplayOrder().replace(' ', '').upper())
		format.reverse()
		for k in format:
			if k == '-':
				spacing = 0
				continue
			if k[0] in '0123456789' or k[0] == '-':
				spacing = int(k)
				continue
			if not columnsByKey.has_key(k):
				spacing = defaultSpacing
				continue
			column = columnsByKey[k]
			c = column.id
			if not self._anyHas[c]:
				spacing = defaultSpacing
				continue
			type = column.type

			if type == 0:
				spacing = defaultSpacing
				continue

			elif type == 1:
				width = column.width
				value = uFont + column.text
				x -= spacing
				for p, playerScore in enumerate(self._playerScores):
					if playerScore._has[c] and playerScore._values[c]:
						name = "ScoreText%d-%d" %( p, c )
						widget = playerScore._widgets[c]
						if widget is None:
							if playerScore._values[ALIVE]:
								widget = (eWidContact, playerScore.iPlayer, -1)
							else:
								widget = (eWidGen, 0, 0)
						screen.setText(name, "", value, 1<<1, x, y - p * height, 0, FontTypes.SMALL_FONT, *widget)
				x -= width
				totalWidth += width + spacing
				spacing = defaultSpacing

			elif type == 2:
				width = 0
				for playerScore in self._playerScores:
					if playerScore._has[c]:
						value = uFont + playerScore._values[c]
						if c == NAME and playerScore.bVassal and bGroupVassals:
							if bLeftAlignName:
								value = uFont + unichr(8854) + value
							else:
								value += unichr(8854)
						newWidth = interface.determineWidth(value) + 2
						if (newWidth > width):
							width = newWidth
				if not width:
					spacing = defaultSpacing
					continue
				x -= spacing
				for p, playerScore in enumerate(self._playerScores):
					if playerScore._has[c]:
						name = "ScoreText%d-%d" %(p, c)
						value = uFont + playerScore._values[c]
						if c == NAME and playerScore.bVassal and bGroupVassals:
							if bLeftAlignName:
								value = uFont + unichr(8854) + value
							else:
								value += unichr(8854)
						align = 1<<1
						adjustX = 0
						if c == NAME:
							name = "ScoreText%d" % p
							if bLeftAlignName:
								align = 1<<0
								adjustX = width
						widget = playerScore._widgets[c]
						if widget is None:
							if playerScore._values[ALIVE]:
								widget = (eWidContact, playerScore.iPlayer, -1)
							else:
								widget = (eWidGen, 0, 0)
						screen.setText(name, "", value, align, x - adjustX, y - p * height, 0, FontTypes.SMALL_FONT, *widget)
				x -= width
				totalWidth += width + spacing
				spacing = defaultSpacing

			elif c == RESEARCH:
				x -= spacing
				for p, playerScore in enumerate(self._playerScores):
					if playerScore._has[c]:
						iTech = playerScore._values[c]
						name = "WID|TECH|Score" + str(iTech) + "|" + str(playerScore.iPlayer)
						BTN = GC.getTechInfo(iTech).getButton()
						screen.setImageButton(name, BTN, x - techIconSize, y - p * height, techIconSize, techIconSize, eWidGen, 1, 1)
				x -= techIconSize
				totalWidth += techIconSize + spacing
				spacing = defaultSpacing

		for playerScore in self._playerScores:
			interface.checkFlashReset(playerScore.iPlayer)

		if bNormalInterface:
			y = yRes - 92 - yRes/10
		else:
			y = yRes - 68
		x = xRes - 21 - totalWidth
		screen.setPanelSize("ScoreBackground", x, y - (height * self.size()) - 8, totalWidth + 12, (height * self.size()) + 12)
		screen.show("ScoreBackground")
		return x

class TeamScores:
	iPlayerAct = -1

	def __init__(self, scoreboard, CyTeam, iRank):
		self._scoreboard = scoreboard
		self.CyTeam = CyTeam
		self.iRank = iRank
		self._playerScores = []
		self.bVassal = CyTeam.isAVassal()
		self._master = None
		self._vassalTeamScores = []

	def rank(self):
		if self.bVassal:
			return self._master.rank()
		else:
			return self.iRank

	def addPlayer(self, iPlayer, iRank):
		playerScore = PlayerScore(self, iPlayer, iRank)
		self._playerScores.append(playerScore)
		return playerScore

	def gatherVassals(self):
		if self.bVassal:
			for iTeam in range(GC.getMAX_PC_TEAMS()):
				teamScores = self._scoreboard.getTeamScores(iTeam)
				if teamScores and self.CyTeam.isVassal(iTeam):
					# teamScores is a master of self
					teamScores._vassalTeamScores.append(self)
					self._master = teamScores
					for playerScore in teamScores._playerScores:
						if playerScore.iPlayer == self.iPlayerAct:
							playerScore.set(MASTER, unichr(8858))
						else:
							playerScore.set(MASTER, unichr(8859))
					self._scoreboard._anyHas[MASTER] = True


class PlayerScore:
	def __init__(self, teamScore, iPlayer, iRank):
		self._teamScore = teamScore
		self.bVassal = teamScore.bVassal
		self.iPlayer = iPlayer
		self.iRank = iRank
		self._has = [False] * NUM_PARTS
		self._values = [None] * NUM_PARTS
		self._widgets = [None] * NUM_PARTS
		self._sortKey = None

	def sortKey(self):
		if self._sortKey is None:
			self._sortKey = (self._teamScore.rank(), self.bVassal, self.iRank)
		return self._sortKey

	def set(self, part, value=True, widget=None):
		self._has[part] = True
		self._values[part] = value
		self._widgets[part] = widget
