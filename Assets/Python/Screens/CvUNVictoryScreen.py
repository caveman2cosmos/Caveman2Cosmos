## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvScreenEnums
import math

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

# this class is shared by both the resource and technology foreign advisors

class CvUnVictoryScreen:

	def __init__(self):
		self.iScreen = -1
		self.nWidgetCount = 0
		self.WIDGET_ID = "UNVictoryWidget"
		self.SCREEN_NAME = "UNVictory"
		self.EXIT_ID = "UNVictoryExitWidget"
		self.BACKGROUND_ID = "UNVictoryBackground"
		self.X_SCREEN = 500
		self.Y_SCREEN = 396
		self.W_SCREEN = 1024
		self.H_SCREEN = 768
		self.Y_TITLE = 8
		self.X_EXIT = 994
		self.Y_EXIT = 726
		self.X_LEADER = 80
		self.Y_LEADER = 115
		self.H_LEADER = 64
		self.W_LEADER = 64

		self.W_LEADERHEAD = 180
		self.H_LEADERHEAD = 216
		self.X_LEADERHEAD = self.X_SCREEN - self.W_LEADERHEAD/2
		self.Y_LEADERHEAD = 230
		self.LEADERHEAD_MARGIN = 15

		self.W_MOVIE = 180
		self.H_MOVIE = 120
		self.X_MOVIE = self.X_SCREEN - self.W_MOVIE/2
		self.Y_MOVIE = 450

		self.X_LEADER_CIRCLE_TOP = self.X_SCREEN
		self.Y_LEADER_CIRCLE_TOP = self.Y_SCREEN - 15

		self.RADIUS_LEADER_ARC = 280

	def killScreen(self):
		if (self.iScreen >= 0):
			screen = self.getScreen()
			screen.hideScreen()
			self.iScreen = -1
		return

	def getScreen(self):
		return CyGInterfaceScreen(self.SCREEN_NAME + str(self.iScreen), CvScreenEnums.UN_SCREEN)

	def interfaceScreen (self):

		self.EXIT_TEXT = u"<font=4>" + localText.getText("TXT_WORD_EXIT", ()).upper() + u"</font>"

		self.iActiveLeader = CyGame().getActivePlayer()

		screen = self.getScreen()
		if screen.isActive():
			return
		screen.setRenderInterfaceOnly(True)
		screen.showScreen( PopupStates.POPUPSTATE_IMMEDIATE, False)

		# Set the background and exit button, and show the screen
		screen.setDimensions(screen.centerX(0), screen.centerY(0), self.W_SCREEN, self.H_SCREEN)
		screen.addDrawControl(self.BACKGROUND_ID, ArtFileMgr.getInterfaceArtInfo("SCREEN_UN_BG").getPath(), 0, 0, self.W_SCREEN, self.H_SCREEN, WidgetTypes.WIDGET_GENERAL, -1, -1 )
		screen.addPanel( "TopPanel", u"", u"", True, False, 0, 0, self.W_SCREEN, 55, PanelStyles.PANEL_STYLE_TOPBAR )
		screen.addPanel( "BottomPanel", u"", u"", True, False, 0, 713, self.W_SCREEN, 55, PanelStyles.PANEL_STYLE_BOTTOMBAR )
		screen.showWindowBackground(False)
		screen.setText(self.EXIT_ID, "", self.EXIT_TEXT, 1<<1, self.X_EXIT, self.Y_EXIT, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )

		self.nWidgetCount = 0

		# Draw leader heads
		self.drawContents()

	# Drawing Leaderheads
	def drawContents(self):

		# Get the Players
		playerActive = gc.getPlayer(self.iActiveLeader)

		# count the leaders
		iCount = 0
		leaderMap = { }
		# Count all other leaders
		for iPlayer in range(gc.getMAX_PLAYERS()):
			player = gc.getPlayer(iPlayer)
			if (player.isAlive() and iPlayer != self.iActiveLeader and not player.isNPC() and not player.isMinorCiv()):
				leaderMap[iPlayer] = iCount
				iCount = iCount + 1
		fLeaderTop = self.Y_LEADER_CIRCLE_TOP
		fRadius = self.RADIUS_LEADER_ARC
		fLeaderArcTop = fLeaderTop

		#if iCount > 16:
		#	iLeaderHeight = int((2 * self.H_LEADER) / 3)
		#	iLeaderWidth = int((2 * self.W_LEADER) / 3)
		#else:
		iLeaderHeight = self.H_LEADER
		iLeaderWidth = self.W_LEADER


		screen = self.getScreen()

		#screen.addPanel(self.getNextWidgetName(), "", "", False, False, self.X_MOVIE-self.LEADERHEAD_MARGIN, self.Y_MOVIE-self.LEADERHEAD_MARGIN, self.W_MOVIE+2*self.LEADERHEAD_MARGIN, self.H_MOVIE+2*self.LEADERHEAD_MARGIN, PanelStyles.PANEL_STYLE_MAIN)
		#screen.playMovie(CyArtFileMgr().getMovieArtInfo("ART_DEF_MOVIE_UNITED_NATIONS").getPath(), self.X_MOVIE, self.Y_MOVIE, self.W_MOVIE, self.H_MOVIE, 0)


		# Our leader head
		screen.addPanel(self.getNextWidgetName(), "", "", False, False, self.X_LEADERHEAD-self.LEADERHEAD_MARGIN, self.Y_LEADERHEAD-self.LEADERHEAD_MARGIN, self.W_LEADERHEAD+2*self.LEADERHEAD_MARGIN, self.H_LEADERHEAD+2*self.LEADERHEAD_MARGIN, PanelStyles.PANEL_STYLE_MAIN)
		szLeaderHead = self.getNextWidgetName()
		screen.addLeaderheadGFC(szLeaderHead, playerActive.getLeaderType(), AttitudeTypes.ATTITUDE_PLEASED, self.X_LEADERHEAD, self.Y_LEADERHEAD, self.W_LEADERHEAD, self.H_LEADERHEAD, WidgetTypes.WIDGET_GENERAL, -1, -1)
		szName = self.getNextWidgetName()
		#szLeaderName = u"<font=3>" + playerActive.getName() + u"</font>"
		#screen.setLabel(szName, "", szLeaderName, 1<<2, self.X_LEADER_CIRCLE_TOP, fLeaderTop + iLeaderHeight + 5, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		# angle increment in radians (360 degree range)
		if (iCount < 2):
			deltaTheta = 0
		else:
			deltaTheta = (2 * 3.1415927) / iCount

		# draw other leaderheads
		for iPlayer in leaderMap.keys():
			player = gc.getPlayer(iPlayer)

			playerBase = gc.getPlayer(self.iActiveLeader)

			fX = int(self.X_LEADER_CIRCLE_TOP - fRadius * math.cos(deltaTheta * leaderMap[iPlayer]) - iLeaderWidth/2)
			fY = int(fLeaderArcTop + fRadius * math.sin(deltaTheta * leaderMap[iPlayer]) - iLeaderHeight/2)

			szLeaderHead = self.getNextWidgetName()
			screen.addDDSGFC(szLeaderHead, gc.getLeaderHeadInfo(player.getLeaderType()).getButton(), int(fX), int(fY), iLeaderWidth, iLeaderHeight, WidgetTypes.WIDGET_GENERAL, -1, -1)

			szName = self.getNextWidgetName()
			szText = u"<font=3>" + player.getName() + u"</font>"
			screen.setLabel(szName, "", szText, 1<<2, fX + iLeaderWidth/2, fY + iLeaderHeight + 5, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )

			# Leader attitude towards active player
			#szName = self.getNextWidgetName()
			#if (gc.getTeam(player.getTeam()).isHasMet(playerBase.getTeam()) and self.iActiveLeader != iPlayer):
			#	szText = " (" + gc.getAttitudeInfo(gc.getPlayer(iPlayer).AI_getAttitude(iBaseLeader)).getDescription() + ")"
			#else:
			#	szText = u""
			#screen.setLabel(szName, "", szText, 1<<2, fX + iLeaderWidth/2, fY + iLeaderHeight + 25, 0, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1 )


	# returns a unique ID for a widget in this screen
	def getNextWidgetName(self):
		szName = self.WIDGET_ID + str(self.nWidgetCount)
		self.nWidgetCount += 1
		return szName

	# Handles the input for this screen...
	def handleInput (self, inputClass):
		return 0

	def update(self, fDelta):
		return

