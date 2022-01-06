## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *

class CvEraMovieScreen:

	def interfaceScreen(self, iEra):
		self.X_SCREEN = 100
		self.Y_SCREEN = 40
		self.W_SCREEN = 775
		self.H_SCREEN = 660
		self.Y_TITLE = self.Y_SCREEN + 20

		self.X_EXIT = self.X_SCREEN + self.W_SCREEN/2 - 50
		self.Y_EXIT = self.Y_SCREEN + self.H_SCREEN - 50
		self.W_EXIT = 120
		self.H_EXIT = 30

		if (CyInterface().noTechSplash()):
			return 0

		import CvScreenEnums
		localText = CyTranslator()
		screen = CyGInterfaceScreen( "EraMovieScreen" + str(iEra), CvScreenEnums.ERA_MOVIE_SCREEN)
		screen.addPanel("EraMoviePanel", "", "", True, True, self.X_SCREEN, self.Y_SCREEN, self.W_SCREEN, self.H_SCREEN, PanelStyles.PANEL_STYLE_MAIN)

		screen.showWindowBackground(True)
		screen.setRenderInterfaceOnly(False)
		screen.setSound("AS2D_NEW_ERA")
		screen.showScreen(PopupStates.POPUPSTATE_MINIMIZED, False)

		# Header...
		szHeader = localText.getText("TXT_KEY_ERA_SPLASH_SCREEN", (CyGlobalContext().getEraInfo(iEra).getTextKey(), ))
		screen.setText("EraTitleHeader" + str(iEra), "", szHeader, 1<<2, self.X_SCREEN + self.W_SCREEN / 2, self.Y_TITLE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		screen.setButtonGFC("EraExit" + str(iEra), localText.getText("TXT_KEY_MAIN_MENU_OK", ()), "", self.X_EXIT, self.Y_EXIT, self.W_EXIT, self.H_EXIT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1, ButtonStyles.BUTTON_STYLE_STANDARD)

		# Play the movie
		if iEra == 0:
			szMovie = "Art/Movies/Era/Era00-Prehistoric.dds"
		elif iEra == 1:
			szMovie = "Art/Movies/Era/Era01-Ancient.dds"
		elif iEra == 2:
			szMovie = "Art/Movies/Era/Era02-Classical.dds"
		elif iEra == 3:
			szMovie = "Art/Movies/Era/Era03-Medeival.dds"
		elif iEra == 4:
			szMovie = "Art/Movies/Era/Era04-Renaissance.dds"
		elif iEra == 5:
			szMovie = "Art/Movies/Era/Era05-Industrial.dds"
		elif iEra == 6:
			szMovie = "Art/Movies/Era/Era07-Modern.dds"
		elif iEra == 7:
			szMovie = "Art/Movies/Era/info.dds"
		elif iEra == 8:
			szMovie = "Art/Movies/Era/Era06-TransHuman.dds"
		elif iEra == 9:
			szMovie = "Art/Movies/Era/transhuman.dds"
		elif iEra == 10:
			szMovie = "Art/Movies/Era/galactic.dds"
		elif iEra == 11:
			szMovie = "Art/Movies/Era/cosmic.dds"
		elif iEra == 12:
			szMovie = "Art/Movies/Era/transcendent.dds"
		else:
			szMovie = "Art/Movies/Era/Era08-Galactic.dds"

		screen.addDDSGFC("EraMovieMovie" + str(iEra), szMovie, self.X_SCREEN + 27, self.Y_SCREEN + 50, 720, 540, WidgetTypes.WIDGET_GENERAL, -1, -1)


	# Will handle the input for this screen...
	def handleInput(self, inputClass): return 0
	def update(self, fDelta): return
