# Pedia overhaul by Toffer for Caveman2Cosmos.

from CvPythonExtensions import *

class PediaEra:

	def __init__(self, parent, H_BOT_ROW):
		self.main = parent

		H_PEDIA_PAGE = parent.H_PEDIA_PAGE

		self.Y_TOP_ROW = Y_TOP_ROW = parent.Y_PEDIA_PAGE
		self.H_TOP_ROW = H_TOP_ROW = 3 * (H_PEDIA_PAGE - H_BOT_ROW * 3) / 4

		self.Y_BOT_ROW = Y_BOT_ROW = Y_TOP_ROW + H_TOP_ROW
		self.H_BOT_ROW = Y_TOP_ROW + H_PEDIA_PAGE - Y_BOT_ROW

		self.W_PEDIA_PAGE = W_PEDIA_PAGE = parent.W_PEDIA_PAGE

		self.W_COL_1 = H_TOP_ROW
		self.W_COL_2 = W_PEDIA_PAGE - H_TOP_ROW

		self.X_COL_1 = X_COL_1 = parent.X_PEDIA_PAGE
		self.X_COL_2 = X_COL_1 + H_TOP_ROW


	def interfaceScreen(self, iTheEra):
		GC = CyGlobalContext()
		TRNSLTR = CyTranslator()
		screen = self.main.screen()
		CvTheEraInfo = GC.getEraInfo(iTheEra)
		CyPlayer = self.main.CyPlayer
		aName = self.main.getNextWidgetName

		eWidGen				= WidgetTypes.WIDGET_GENERAL
		ePnlBlue50		= PanelStyles.PANEL_STYLE_BLUE50
		eFontTitle		= FontTypes.TITLE_FONT

		szfontEdge, szfont4b, szfont4, szfont3b, szfont3, szfont2b, szfont2 = self.main.aFontList

		X_COL_1 = self.X_COL_1
		X_COL_2 = self.X_COL_2
		W_COL_1 = self.W_COL_1
		W_COL_2 = self.W_COL_2
		Y_ROW_1 = self.Y_TOP_ROW
		Y_ROW_2 = self.Y_BOT_ROW
		H_ROW_1 = self.H_TOP_ROW
		H_ROW_2 = self.H_BOT_ROW
		W_PEDIA_PAGE = self.W_PEDIA_PAGE

		szText = CvTheEraInfo.getDescription()
		IMG = CvTheEraInfo.getButton()
		x = X_COL_1
		if IMG:
			screen.setText(aName(), "", "<img=%s>" % IMG, 1<<0, X_COL_1, 0, 0, eFontTitle, eWidGen, 1, 2)
			x += 44
		screen.setText(aName(), "", szfontEdge + szText, 1<<0, x, 0, 0, eFontTitle, eWidGen, 1, 2)

		Pnl = aName()
		screen.addPanel(Pnl, "", "", False, False, X_COL_1 - 3, Y_ROW_1 + 2, H_ROW_1, H_ROW_1, PanelStyles.PANEL_STYLE_MAIN)

		if not iTheEra:
			IMG = "Art/Movies/Era/Era00-Prehistoric.dds"
		elif iTheEra == 1:
			IMG = "Art/Movies/Era/Era01-Ancient.dds"
		elif iTheEra == 2:
			IMG = "Art/Movies/Era/Era02-Classical.dds"
		elif iTheEra == 3:
			IMG = "Art/Movies/Era/Era03-Medeival.dds"
		elif iTheEra == 4:
			IMG = "Art/Movies/Era/Era04-Renaissance.dds"
		elif iTheEra == 5:
			IMG = "Art/Movies/Era/Era05-Industrial.dds"
		elif iTheEra == 6:
			IMG = "Art/Movies/Era/Era07-Modern.dds"
		elif iTheEra == 7:
			IMG = "Art/Movies/Era/info.dds"
		elif iTheEra == 8:
			IMG = "Art/Movies/Era/Era06-TransHuman.dds"
		elif iTheEra == 9:
			IMG = "Art/Movies/Era/transhuman.dds"
		elif iTheEra == 10:
			IMG = "Art/Movies/Era/galactic.dds"
		elif iTheEra == 11:
			IMG = "Art/Movies/Era/cosmic.dds"
		elif iTheEra == 12:
			IMG = "Art/Movies/Era/transcendent.dds"
		else:
			IMG = "Art/Movies/Era/Era08-Galactic.dds"

		screen.setImageButtonAt(aName(), Pnl, IMG, 12, 12, H_ROW_1-24, H_ROW_1-24, eWidGen, 1, 1)

		# Special
		szTxt = "Game Start:"
		iTemp = CvTheEraInfo.getStartingGold()
		if CyPlayer:
			CvHandicapInfo = GC.getHandicapInfo(CyPlayer.getHandicapType())
			iTemp += CvHandicapInfo.getStartingGold()
			if iTemp:
				iTemp *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent()
				iTemp /= 100
		if iTemp:
			szTxt += "\n\tStarting gold: %d" % iTemp
		if CvTheEraInfo.isNoGoodies():
			szTxt += "\n\tNo goodies"
		iMult = CvTheEraInfo.getStartingUnitMultiplier()
		if iMult > 1:
			szTxt += "\n\tUnit Multiplier: %d" % iMult
		iTemp = CvTheEraInfo.getStartingDefenseUnits()
		if CyPlayer:
			iTemp += CvHandicapInfo.getStartingDefenseUnits()
		if iTemp:
			szTxt += "\n\tDefensive Units: %d" % iTemp
			if iMult > 1:
				szTxt += " (%d)" %(iTemp*iMult)
		iTemp = CvTheEraInfo.getStartingWorkerUnits()
		if CyPlayer:
			iTemp += CvHandicapInfo.getStartingWorkerUnits()
		if iTemp:
			szTxt += "\n\tWorker Units: %d" % iTemp
			if iMult > 1:
				szTxt += " (%d)" %(iTemp*iMult)
		iTemp = CvTheEraInfo.getStartingExploreUnits()
		if CyPlayer:
			iTemp += CvHandicapInfo.getStartingExploreUnits()
		if iTemp:
			szTxt += "\n\tExplorer Units: %d" % iTemp
			if iMult > 1:
				szTxt += " (%d)" %(iTemp*iMult)

		szTxt += "\n\nEvent chance per turn: %d%%" % CvTheEraInfo.getEventChancePerTurn()

		szTxt += "\n%d%% Growth percent" % CvTheEraInfo.getGrowthPercent()
		szTxt += "\n%d%% Train percent" % CvTheEraInfo.getTrainPercent()
		szTxt += "\n%d%% Construct percent" % CvTheEraInfo.getConstructPercent()
		szTxt += "\n%d%% Create percent" % CvTheEraInfo.getCreatePercent()
		szTxt += "\n%d%% Research percent" % CvTheEraInfo.getResearchPercent()
		szTxt += "\n%d%% Build percent" % CvTheEraInfo.getBuildPercent()
		szTxt += "\n%d%% Improvement percent" % CvTheEraInfo.getImprovementPercent()
		szTxt += "\n%d%% Great people percent" % CvTheEraInfo.getGreatPeoplePercent()
		szTxt += "\n%d%% Anarchy percent" % CvTheEraInfo.getAnarchyPercent()
		iTemp = CvTheEraInfo.getFreePopulation()
		if iTemp:
			szTxt += "\nFree population: %d" % iTemp

		szTxt += "\n%d Soundtracks" % CvTheEraInfo.getNumSoundtracks()

		if CvTheEraInfo.isNoAnimals():
			szTxt += "\n\tNo animals"
		if CvTheEraInfo.isNoBarbUnits():
			szTxt += "\n\tNo barbarian units"
		if CvTheEraInfo.isNoBarbCities():
			szTxt += "\n\tNo barbarian cities"

		if szTxt:
			screen.addPanel(aName(), "", "", True, False, X_COL_2, Y_ROW_1 + 2, W_COL_2, H_ROW_1 - 2, ePnlBlue50)
			screen.addMultilineText(aName(), szfont3 + szTxt, X_COL_2 + 4, Y_ROW_1 + 10, W_COL_2 - 8, H_ROW_1 - 18, eWidGen, 0, 0, 1<<0)

		# Text
		szTxt = TRNSLTR.getText("TXT_KEY_CIVILOPEDIA_STRATEGY", ()) + CvTheEraInfo.getStrategy() + "\n\n"

		screen.addPanel(aName(), "", "", True, False, X_COL_1, Y_ROW_2, W_PEDIA_PAGE, H_ROW_2, ePnlBlue50)
		screen.addMultilineText(aName(), szfont2 + szTxt, X_COL_1 + 4, Y_ROW_2 + 8, W_PEDIA_PAGE - 8, H_ROW_2 - 16, eWidGen, 0, 0, 1<<0)
