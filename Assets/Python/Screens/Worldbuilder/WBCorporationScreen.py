from CvPythonExtensions import *
import CvScreenEnums
import WBReligionScreen
import WBPlayerScreen
import WBTeamScreen
import WBCityEditScreen
import WBInfoScreen
GC = CyGlobalContext()

bHeadquarter = False
iOwnerType = 0
lCities = []

class WBCorporationScreen:

	def __init__(self, WB):
		self.WB = WB
		self.iTable_Y = 80

	def interfaceScreen(self, iPlayerX):
		screen = CyGInterfaceScreen("WBCorporationScreen", CvScreenEnums.WB_CORPORATION)
		global iSelectedPlayer

		iSelectedPlayer = iPlayerX

		screen.setRenderInterfaceOnly(True)
		screen.addPanel("MainBG", u"", u"", True, False, -10, -10, screen.getXResolution() + 20, screen.getYResolution() + 20, PanelStyles.PANEL_STYLE_MAIN )
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

		screen.setText("WBCorporationExit", "Background", "<font=4>" + CyTranslator().getText("TXT_WORD_EXIT", ()).upper() + "</font>", 1<<1, screen.getXResolution() - 30, screen.getYResolution() - 42, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )
		screen.setLabel("CorporationHeader", "Background", "<font=4b>" + CyTranslator().getText("TXT_KEY_CONCEPT_CORPORATIONS", ()) + "</font>", 1<<2, screen.getXResolution()/2, 20, -0.1, FontTypes.GAME_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		screen.setLabel("HeadquarterHeader", "Background", "<font=3b>" + CyTranslator().getText("TXT_KEY_CORPORATION_HEADQUARTERS", ()) + "</font>", 1<<2, screen.getXResolution()/8, self.iTable_Y - 30, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		iWidth = screen.getXResolution()/4 - 40
		screen.addDropDownBoxGFC("CurrentPage", 20, screen.getYResolution() - 42, iWidth, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_PLAYER_DATA", ()), 0, 0, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_WB_TEAM_DATA", ()), 1, 1, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_RELIGION", ()), 8, 8, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_CONCEPT_CORPORATIONS", ()), 9, 9, True)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_PITBOSS_GAME_OPTIONS", ()), 10, 10, False)
		screen.addPullDownString("CurrentPage", CyTranslator().getText("TXT_KEY_INFO_SCREEN", ()), 11, 11, False)

		screen.addDropDownBoxGFC("OwnerType", screen.getXResolution()/4, self.iTable_Y - 60, 150, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		screen.addPullDownString("OwnerType", CyTranslator().getText("TXT_KEY_WB_CITY_ALL", ()), 0, 0, 0 == iOwnerType)
		screen.addPullDownString("OwnerType", CyTranslator().getText("TXT_KEY_PITBOSS_TEAM", ()), 2, 2, 2 == iOwnerType)
		screen.addPullDownString("OwnerType", CyTranslator().getText("TXT_KEY_MAIN_MENU_PLAYER", ()), 1, 1, 1 == iOwnerType)

		screen.addDropDownBoxGFC("CurrentPlayer", screen.getXResolution()/4, self.iTable_Y - 30, 150, WidgetTypes.WIDGET_GENERAL, -1, -1, FontTypes.GAME_FONT)
		for i in xrange(GC.getMAX_PLAYERS()):
			pPlayerX = GC.getPlayer(i)
			if pPlayerX.isEverAlive():
				sText = pPlayerX.getName()
				if not pPlayerX.isAlive():
					sText = "*" + sText
				if pPlayerX.isTurnActive():
					sText = "[" + sText + "]"
				screen.addPullDownString("CurrentPlayer", sText, i, i, i == iSelectedPlayer)

		sText = "<font=3b>" + CyTranslator().getText("TXT_KEY_CORPORATION_HEADQUARTERS", ()) + "</font>"
		sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
		if bHeadquarter:
			sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
		screen.setText("SetHeadquarter", "Background", sColor + sText + "</color>", 1<<1, screen.getXResolution() - 20, self.iTable_Y - 30, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)

		self.placeHeadquarter()
		self.sortCities()

	def sortCities(self):
		global lCities
		lCities = []
		iSelectedTeam = GC.getPlayer(iSelectedPlayer).getTeam()
		for iPlayerX in xrange(GC.getMAX_PLAYERS()):
			pPlayerX = GC.getPlayer(iPlayerX)
			if iOwnerType == 1 and iPlayerX != iSelectedPlayer: continue
			if iOwnerType == 2 and pPlayerX.getTeam() != iSelectedTeam: continue
			for loopCity in pPlayerX.cities():
				lCities.append([loopCity, iPlayerX])
		self.placeCityTable()

	def placeCityTable(self):
		screen = CyGInterfaceScreen("WBCorporationScreen", CvScreenEnums.WB_CORPORATION)
		iX = screen.getXResolution()/4
		iY = self.iTable_Y
		iWidth = screen.getXResolution() * 3/4 - 20
		iHeight = (screen.getYResolution() - iY - 100) / 24 * 24 + 2

		screen.addTableControlGFC("WBAllCorporations", 1 + GC.getNumCorporationInfos(), iX, iY, iWidth, 50, False, True, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader("WBAllCorporations", 0, "", 150)
		for i in xrange(2):
			screen.appendTableRow("WBAllCorporations")
		sText = CyTranslator().getText("TXT_KEY_WB_CITY_ALL", ())
		screen.setTableText("WBAllCorporations", 0, 0, "<font=3b>" + sText + " (+)</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<2)
		screen.setTableText("WBAllCorporations", 0, 1, "<font=3b>" + sText + " (-)</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<2)

		for i in xrange(GC.getNumCorporationInfos()):
			sText = u"%c" %(GC.getCorporationInfo(i).getChar())
			screen.setTableColumnHeader("WBAllCorporations", i + 1, "", (iWidth - 150) / GC.getNumCorporationInfos())
			screen.setTableText("WBAllCorporations", i + 1, 0, "<font=4>" + sText + "</font>", "", WidgetTypes.WIDGET_PYTHON, 8201, i, 1<<2)
			screen.setTableText("WBAllCorporations", i + 1, 1, "<font=4>" + sText + "</font>", "", WidgetTypes.WIDGET_PYTHON, 8201, i, 1<<2)

		screen.addTableControlGFC("WBCityCorporations", 3 + GC.getNumCorporationInfos(), iX, iY + 60, iWidth, iHeight, False, True, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader("WBCityCorporations", 0, "", 24)
		screen.setTableColumnHeader("WBCityCorporations", 1, "", 24)
		screen.setTableColumnHeader("WBCityCorporations", 2, "", 102)
		for i in xrange(GC.getNumCorporationInfos()):
			screen.setTableColumnHeader("WBCityCorporations", i + 3, "", (iWidth - 150) / GC.getNumCorporationInfos())

		for (loopCity, iPlayerX) in lCities:
			pPlayerX = GC.getPlayer(iPlayerX)
			iLeader = pPlayerX.getLeaderType()
			iCiv = pPlayerX.getCivilizationType()
			sColor = u"<color=%d,%d,%d,%d>" %(pPlayerX.getPlayerTextColorR(), pPlayerX.getPlayerTextColorG(), pPlayerX.getPlayerTextColorB(), pPlayerX.getPlayerTextColorA())
			iRow = screen.appendTableRow("WBCityCorporations")
			screen.setTableText("WBCityCorporations", 0, iRow, "", GC.getCivilizationInfo(iCiv).getButton(), WidgetTypes.WIDGET_PYTHON, 7872, iCiv, 1<<2)
			screen.setTableText("WBCityCorporations", 1, iRow, "", GC.getLeaderHeadInfo(iLeader).getButton(), WidgetTypes.WIDGET_PYTHON, 7876, iLeader, 1<<2)
			screen.setTableText("WBCityCorporations", 2, iRow, "<font=3>" + sColor + loopCity.getName() + "</color></font>", "", WidgetTypes.WIDGET_PYTHON, 7200 + iPlayerX, loopCity.getID(), 1<<0)
			for i in xrange(GC.getNumCorporationInfos()):
				sText = " "
				if loopCity.isHasCorporation(i):
					sText = u"%c" %(GC.getCorporationInfo(i).getChar())
				if loopCity.isHeadquartersByType(i):
					sText = u"%c" %(GC.getCorporationInfo(i).getHeadquarterChar())
				screen.setTableText("WBCityCorporations", i + 3, iRow, "<font=4>" + sText + "</font>", "", WidgetTypes.WIDGET_PYTHON, 8201, i, 1<<2)

	def placeHeadquarter(self):
		screen = CyGInterfaceScreen("WBCorporationScreen", CvScreenEnums.WB_CORPORATION)
		iX = 20
		iY = self.iTable_Y
		iWidth = screen.getXResolution()/4 - 40
		iHeight = (screen.getYResolution() - iY - 40) / 24 * 24 + 2

		screen.addTableControlGFC("WBHeadquarter", 3, iX, iY, iWidth, iHeight, False, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD)
		screen.setTableColumnHeader("WBHeadquarter", 0, "", 24)
		screen.setTableColumnHeader("WBHeadquarter", 1, "", 24)
		screen.setTableColumnHeader("WBHeadquarter", 2, "", iWidth - 48)

		for i in xrange(GC.getNumCorporationInfos()):
			iRow = screen.appendTableRow("WBHeadquarter")
			screen.setTableText("WBHeadquarter", 0, iRow, "", GC.getCorporationInfo(i).getButton(), WidgetTypes.WIDGET_PYTHON, 8201, i, 1<<0)
			pHeadquarter = CyGame().getHeadquarters(i)
			if pHeadquarter:
				iPlayerX = pHeadquarter.getOwner()
				pPlayerX = GC.getPlayer(iPlayerX)
				sColor = u"<color=%d,%d,%d,%d>" %(pPlayerX.getPlayerTextColorR(), pPlayerX.getPlayerTextColorG(), pPlayerX.getPlayerTextColorB(), pPlayerX.getPlayerTextColorA())
				iLeader = pPlayerX.getLeaderType()
				screen.setTableText("WBHeadquarter", 1, iRow, "", GC.getLeaderHeadInfo(iLeader).getButton(), WidgetTypes.WIDGET_PYTHON, 7876, iPlayerX * 10000 + iLeader, 1<<0)
				screen.setTableText("WBHeadquarter", 2, iRow, "<font=3>" + sColor + pHeadquarter.getName() + "</color></font>", GC.getCivilizationInfo(pHeadquarter.getCivilizationType()).getButton(), WidgetTypes.WIDGET_PYTHON, 7200 + iPlayerX, pHeadquarter.getID(), 1<<0)

	def handleInput(self, inputClass):
		screen = CyGInterfaceScreen("WBCorporationScreen", CvScreenEnums.WB_CORPORATION)
		global iSelectedPlayer
		global bHeadquarter
		global iOwnerType

		if inputClass.getButtonType() == WidgetTypes.WIDGET_PYTHON:
			if inputClass.getData1() > 7199 and inputClass.getData1() < 7300:
				iCityID = inputClass.getData2()
				iPlayerX = inputClass.getData1() - 7200
				WBCityEditScreen.WBCityEditScreen(self.WB).interfaceScreen(GC.getPlayer(iPlayerX).getCity(iCityID))

			elif inputClass.getData1() == 7876 or inputClass.getData1() == 7872:
				iPlayerX = inputClass.getData2() /10000
				WBPlayerScreen.WBPlayerScreen(self.WB).interfaceScreen(iPlayerX)

		if inputClass.getFunctionName() == "CurrentPage":
			iIndex = screen.getPullDownData("CurrentPage", screen.getSelectedPullDownID("CurrentPage"))
			if iIndex == 0:
				WBPlayerScreen.WBPlayerScreen(self.WB).interfaceScreen(iSelectedPlayer)
			elif iIndex == 1:
				WBTeamScreen.WBTeamScreen(self.WB).interfaceScreen(GC.getPlayer(iSelectedPlayer).getTeam())
			elif iIndex == 8:
				WBReligionScreen.WBReligionScreen(self.WB).interfaceScreen(iSelectedPlayer)
			elif iIndex == 10:
				self.WB.goToSubScreen("GameScreen")
			elif iIndex == 11:
				WBInfoScreen.WBInfoScreen(self.WB).interfaceScreen(iSelectedPlayer)

		elif inputClass.getFunctionName() == "OwnerType":
			iOwnerType = screen.getPullDownData("OwnerType", screen.getSelectedPullDownID("OwnerType"))
			self.sortCities()

		elif inputClass.getFunctionName() == "CurrentPlayer":
			iSelectedPlayer = screen.getPullDownData("CurrentPlayer", screen.getSelectedPullDownID("CurrentPlayer"))
			self.interfaceScreen(iSelectedPlayer)

		elif inputClass.getFunctionName() == "WBCityCorporations":
			if inputClass.getData1() == 8201:
				pCity = lCities[inputClass.getData()][0]
				if bHeadquarter:
					self.editHeadquarter(inputClass.getData2(), pCity)
				else:
					self.editCorporation(inputClass.getData2(), pCity, 2)
				self.placeCityTable()

		elif inputClass.getFunctionName() == "WBAllCorporations":
			if inputClass.getButtonType() == WidgetTypes.WIDGET_PYTHON:
				for (loopCity, iPlayerX) in lCities:
					self.editCorporation(inputClass.getData2(), loopCity, inputClass.getData1() == 8201)
				self.placeCityTable()

		elif inputClass.getFunctionName() == "SetHeadquarter":
			bHeadquarter = not bHeadquarter
			sText = "<font=3b>" + CyTranslator().getText("TXT_KEY_CORPORATION_HEADQUARTERS", ()) + "</font>"
			sColor = CyTranslator().getText("[COLOR_WARNING_TEXT]", ())
			if bHeadquarter:
				sColor = CyTranslator().getText("[COLOR_POSITIVE_TEXT]", ())
			screen.modifyString("SetHeadquarter", sColor + sText + "</color>", 0)
		return 1

	def editHeadquarter(self, item, pCity):
		if pCity.isHeadquartersByType(item):
			CyGame().clearHeadquarters(item)
		else:
			CyGame().setHeadquarters(item, pCity, False)
		self.placeHeadquarter()

	def editCorporation(self, item, pCity, iType):
		if iType == 2:
			iType = not pCity.isHasCorporation(item)
		if not iType and pCity.isHeadquartersByType(item):
			CyGame().clearHeadquarters(item)
			self.placeHeadquarter()
		pCity.setHasCorporation(item, iType, False, False)

	def update(self, fDelta):
		return 1