## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import string

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class SevoPediaRoute:
	"Civilopedia Screen for tile Routes"

	def __init__(self, main):
		self.iRoute = -1
		self.top = main


		self.X_ROUTE_PANE = self.top.X_PEDIA_PAGE
		self.Y_ROUTE_PANE = self.top.Y_PEDIA_PAGE
		self.W_ROUTE_PANE = 300
		self.H_ROUTE_PANE = 116

		self.W_ICON = 100
		self.H_ICON = 100
		self.X_ICON = self.X_ROUTE_PANE + (self.H_ROUTE_PANE - self.H_ICON) / 2
		self.Y_ICON = self.Y_ROUTE_PANE + (self.H_ROUTE_PANE - self.H_ICON) / 2
		self.ICON_SIZE = 64
		self.BUTTON_SIZE = 64

		self.X_STATS_PANE = self.X_ROUTE_PANE + 125
		self.Y_STATS_PANE = self.Y_ROUTE_PANE + 10
		self.W_STATS_PANE = 290
		self.H_STATS_PANE = 200

		self.X_BONUS_YIELDS_PANE = self.X_ROUTE_PANE + self.W_ROUTE_PANE +5
		self.Y_BONUS_YIELDS_PANE = self.Y_ROUTE_PANE + 10
		self.W_BONUS_YIELDS_PANE = 275
		self.H_BONUS_YIELDS_PANE = 226

		self.X_REQUIRES_PANE = self.X_ROUTE_PANE
		self.Y_REQUIRES_PANE = self.Y_ROUTE_PANE + self.H_ROUTE_PANE + 10
		self.W_REQUIRES_PANE = 300
		self.H_REQUIRES_PANE = 110

		self.X_SPECIAL_PANE = self.X_REQUIRES_PANE
		self.Y_SPECIAL_PANE = self.Y_REQUIRES_PANE + self.H_REQUIRES_PANE + 10
		self.W_SPECIAL_PANE = 600
		self.H_SPECIAL_PANE = 195

		self.X_HISTORY_PANE = self.X_SPECIAL_PANE
		self.Y_HISTORY_PANE = self.Y_SPECIAL_PANE + self.H_SPECIAL_PANE + 10
		self.W_HISTORY_PANE = 600
		self.H_HISTORY_PANE = 195

	# Screen construction function
	def interfaceScreen(self, iRoute):
		self.iRoute = iRoute
		screen = self.top.getScreen()

		screen.addPanel(self.top.getNextWidgetName(), "", "", False, False, self.X_ROUTE_PANE, self.Y_ROUTE_PANE, self.W_ROUTE_PANE, self.H_ROUTE_PANE, PanelStyles.PANEL_STYLE_BLUE50)
		screen.addPanel(self.top.getNextWidgetName(), "", "", False, False, self.X_ICON, self.Y_ICON, self.W_ICON, self.H_ICON, PanelStyles.PANEL_STYLE_MAIN)
		screen.addDDSGFC(self.top.getNextWidgetName(), gc.getRouteInfo(self.iRoute).getButton(), self.X_ICON + self.W_ICON/2 - self.ICON_SIZE/2, self.Y_ICON + self.H_ICON/2 - self.ICON_SIZE/2, self.ICON_SIZE, self.ICON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		self.placeStats()
		self.placeRequires()
		self.placeImprovementYield()
		self.placeSpecial()
		self.placeHistory()

	def placeStats(self):
		screen = self.top.getScreen()
		panelName = self.top.getNextWidgetName()

		screen.addListBoxGFC(panelName, "", self.X_STATS_PANE, self.Y_STATS_PANE, self.W_STATS_PANE, self.H_STATS_PANE, TableStyles.TABLE_STYLE_EMPTY)
		screen.enableSelect(panelName, False)
		iFontSize = 3

		for k in range(YieldTypes.NUM_YIELD_TYPES):
			iYieldChange = gc.getRouteInfo(self.iRoute).getYieldChange(k)
			if (iYieldChange != 0):
				if (iYieldChange > 0):
					sign = "+"
				else:
					sign = ""
				szYield = (u"%s: %s%i " % (gc.getYieldInfo(k).getDescription().upper(), sign, iYieldChange))
				screen.appendListBoxStringNoUpdate(panelName, u"<font=%d>" % iFontSize + szYield + (u"%c" % gc.getYieldInfo(k).getChar()) + u"</font>", WidgetTypes.WIDGET_GENERAL, 0, 0, 1<<0)

		screen.updateListBox(panelName)

	def placeImprovementYield(self):
		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_PEDIA_ROUTE_IMPROVEMENT_YIELD_CHANGE", ()), "", True, True,
				 self.X_BONUS_YIELDS_PANE, self.Y_BONUS_YIELDS_PANE, self.W_BONUS_YIELDS_PANE, self.H_BONUS_YIELDS_PANE, PanelStyles.PANEL_STYLE_BLUE50 )

		szYield = u""
		bImprovementYieldChange = False
		for l in range(gc.getNumImprovementInfos()):
			bImprovementChange = False
			szImprovementYield  = u""
			for k in range(YieldTypes.NUM_YIELD_TYPES):
				iYieldChange = gc.getImprovementInfo(l).getRouteYieldChanges(self.iRoute, k)
				if (iYieldChange != 0):
					if bImprovementChange == True:
						szImprovementYield += (u",")
					szImprovementYield += (u" %i %c" % (iYieldChange, gc.getYieldInfo(k).getChar()))
					bImprovementYieldChange = True
					bImprovementChange = True
			if bImprovementChange:
				childPanelName = self.top.getNextWidgetName()
				screen.attachPanel(panelName, childPanelName, "", "", False, False, PanelStyles.PANEL_STYLE_EMPTY)
				screen.attachLabel(childPanelName, "", "  ")
				screen.attachImageButton( childPanelName, "", gc.getImprovementInfo(l).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_IMPROVEMENT, l, 1, False )
				screen.attachLabel(childPanelName, "", u"<font=4>" + szImprovementYield + u"</font>")

		if bImprovementYieldChange == False:
			szYield += localText.getText("TXT_KEY_PEDIA_NO_PLOT_YIELD_CHANGE", ())

			listName = self.top.getNextWidgetName()
			screen.addMultilineText(listName, szYield, self.X_BONUS_YIELDS_PANE+5, self.Y_BONUS_YIELDS_PANE+30, self.W_BONUS_YIELDS_PANE-10, self.H_BONUS_YIELDS_PANE-35, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

		return

	def placeRequires(self):

		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_PEDIA_REQUIRES", ()), "", False, True,
				 self.X_REQUIRES_PANE, self.Y_REQUIRES_PANE, self.W_REQUIRES_PANE, self.H_REQUIRES_PANE, PanelStyles.PANEL_STYLE_BLUE50 )

		screen.attachLabel(panelName, "", "  ")

		aTechList = []
		for iBuild in range(gc.getNumBuildInfos()):
			if (gc.getBuildInfo(iBuild).getRoute() == self.iRoute):
				iTech = gc.getBuildInfo(iBuild).getTechPrereq()
				if (iTech > -1):
					if not iTech in aTechList:
						aTechList.append(iTech)
		for i in aTechList:
			screen.attachImageButton( panelName, "", gc.getTechInfo(iTech).getButton(), GenericButtonSizes.BUTTON_SIZE_46, WidgetTypes.WIDGET_PEDIA_JUMP_TO_TECH, iTech, 2, False )

		RouteInfo = gc.getRouteInfo(self.iRoute)
		bFirst = True
		iPrereq = RouteInfo.getPrereqBonus()
		if (iPrereq >= 0):
			bFirst = False
			screen.attachImageButton(panelName, "", gc.getBonusInfo(iPrereq).getButton(), GenericButtonSizes.BUTTON_SIZE_46, WidgetTypes.WIDGET_PEDIA_JUMP_TO_BONUS, iPrereq, 2, False)
		nOr = len(RouteInfo.getPrereqOrBonuses())
		szLeftDelimeter = ""
		szRightDelimeter = ""
		if (not bFirst):
			if (nOr > 1):
				szLeftDelimeter = localText.getText("TXT_KEY_AND", ()) + "("
				szRightDelimeter = ") "
			elif (nOr > 0):
				szLeftDelimeter = localText.getText("TXT_KEY_AND", ())
		if len(szLeftDelimeter) > 0:
			screen.attachLabel(panelName, "", szLeftDelimeter)
		bFirst = True
		for eBonus in RouteInfo.getPrereqOrBonuses():
			if not bFirst:
				screen.attachLabel(panelName, "", localText.getText("TXT_KEY_OR", ()))
			else:
				bFirst = False
			screen.attachImageButton(panelName, "", gc.getBonusInfo(eBonus).getButton(), GenericButtonSizes.BUTTON_SIZE_46, WidgetTypes.WIDGET_PEDIA_JUMP_TO_BONUS, eBonus, -1, False)
		if len(szRightDelimeter) > 0:
			screen.attachLabel(panelName, "", szRightDelimeter)

	def placeSpecial(self):

		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_PEDIA_SPECIAL_ABILITIES", ()), "", True, False, self.X_SPECIAL_PANE, self.Y_SPECIAL_PANE, self.W_SPECIAL_PANE, self.H_SPECIAL_PANE, PanelStyles.PANEL_STYLE_BLUE50 )
		listName = self.top.getNextWidgetName()
		screen.attachListBoxGFC( panelName, listName, "", TableStyles.TABLE_STYLE_EMPTY )
		screen.enableSelect(listName, False)
		szSpecialText = CyGameTextMgr().getRouteHelp(self.iRoute, True)
		splitText = string.split( szSpecialText, "\n" )
		for special in splitText:
			if len( special ) != 0:
				screen.appendListBoxString( listName, special, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0 )

	def placeHistory(self):

		screen = self.top.getScreen()

		panelName = self.top.getNextWidgetName()
		screen.addPanel(panelName, CyTranslator().getText("TXT_KEY_PEDIA_CATEGORY_CONCEPT", ()), "", True, False, self.X_HISTORY_PANE, self.Y_HISTORY_PANE, self.W_HISTORY_PANE, self.H_HISTORY_PANE, PanelStyles.PANEL_STYLE_BLUE50 )
		screen.addMultilineText(panelName, CyTranslator().getText("TXT_KEY_CONCEPT_MOVEMENT_PEDIA", ()), self.X_HISTORY_PANE+10, self.Y_HISTORY_PANE + 30, self.W_HISTORY_PANE -20, self.H_HISTORY_PANE- 55, WidgetTypes.WIDGET_GENERAL, -1, -1, 1<<0)

	def placeLinks(self, bRedraw):

		screen = self.top.getScreen()

		if bRedraw:
			screen.clearListBoxGFC(self.top.LIST_ID)

		# sort Routes alphabetically
		rowListName=[(0,0)]*gc.getNumRouteInfos()
		for j in range(gc.getNumRouteInfos()):
			rowListName[j] = (gc.getRouteInfo(j).getDescription(), j)
		rowListName.sort()

		iSelected = 0
		i = 0
		for iI in range(gc.getNumRouteInfos()):
			if (not gc.getRouteInfo(rowListName[iI][1]).isGraphicalOnly()):
				if bRedraw:
					screen.appendListBoxString(self.top.LIST_ID, rowListName[iI][0], WidgetTypes.WIDGET_HELP_MOVE_BONUS, rowListName[iI][1], 0, 1<<0)
				if rowListName[iI][1] == self.iRoute:
					iSelected = i
				i += 1

		screen.setSelectedListBoxStringGFC(self.top.LIST_ID, iSelected)


	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		return 0


