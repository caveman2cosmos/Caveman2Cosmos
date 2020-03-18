# Pedia overhaul by Toffer for Caveman2Cosmos.

from CvPythonExtensions import *
from operator import itemgetter

class Index:

	def __init__(self, parent, xRes, yRes, H_EDGE_PANEL, Y_PEDIA_PAGE, szFontEdge):
		self.main = parent
		self.xRes = xRes
		aFontList = [szFontEdge]
		if xRes > 1600:
			if xRes > 2000:
				self.nLists = nLists = 5
			else:
				self.nLists = nLists = 4
			aFontList.append("<font=3b>")
			self.iIconSize = 22
		elif xRes > 1200:
			self.nLists = nLists = 3
			aFontList.append("<font=2b>")
			self.iIconSize = 20
		else:
			self.nLists = nLists = 2
			aFontList.append("<font=1b>")
			self.iIconSize = 18
		self.aFontList = aFontList

		self.Y_INDEX = Y_PEDIA_PAGE
		self.H_INDEX = parent.H_MID_SECTION
		self.W_INDEX = xRes / nLists
		self.Y_LETTER = yRes - H_EDGE_PANEL + 2
		self.H_LETTER = H_EDGE_PANEL

		self.nIndexLists = 0

	def interfaceScreen(self):
		GC = CyGlobalContext()
		screen = self.main.screen()
		xRes = self.xRes
		szFontEdge, szFont = self.aFontList
		eFontTitle = FontTypes.TITLE_FONT
		eWidGen	= WidgetTypes.WIDGET_GENERAL
		if not self.nIndexLists: # Initialize Index
			iWidJuToTech		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_TECH
			iWidJuToUnit		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT
			iWidJuToUnitCombat	= WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT_COMBAT
			iWidJuToPromotion	= WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROMOTION
			iWidJuToBuilding	= WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING
			iWidJuToProject		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_PROJECT
			iWidJuToSpecialist	= WidgetTypes.WIDGET_PEDIA_JUMP_TO_SPECIALIST
			iWidJuToTerrain		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_TERRAIN
			iWidJuToFeature		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_FEATURE
			iWidJuToBonus		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_BONUS
			iWidJuToImprovement	= WidgetTypes.WIDGET_PEDIA_JUMP_TO_IMPROVEMENT
			iWidJuToCiv			= WidgetTypes.WIDGET_PEDIA_JUMP_TO_CIV
			iWidJuToLeader		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_LEADER
			iWidJuToTrait		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_TRAIT
			iWidJuToCivic		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_CIVIC
			iWidJuToReligion	= WidgetTypes.WIDGET_PEDIA_JUMP_TO_RELIGION
			iWidJuToCorporation	= WidgetTypes.WIDGET_PEDIA_JUMP_TO_CORPORATION
			iWidJuToRoute		= WidgetTypes.WIDGET_PEDIA_JUMP_TO_ROUTE

			aWorkList = [
				(GC.getNumTechInfos, GC.getTechInfo, iWidJuToTech),
				(GC.getNumUnitInfos, GC.getUnitInfo, iWidJuToUnit),
				(GC.getNumUnitCombatInfos, GC.getUnitCombatInfo, iWidJuToUnitCombat),
				(GC.getNumPromotionInfos, GC.getPromotionInfo, iWidJuToPromotion),
				(GC.getNumBuildingInfos, GC.getBuildingInfo, iWidJuToBuilding),
				(GC.getNumProjectInfos, GC.getProjectInfo, iWidJuToProject),
				(GC.getNumSpecialistInfos, GC.getSpecialistInfo, iWidJuToSpecialist),
				(GC.getNumTerrainInfos, GC.getTerrainInfo, iWidJuToTerrain),
				(GC.getNumFeatureInfos, GC.getFeatureInfo, iWidJuToFeature),
				(GC.getNumBonusInfos, GC.getBonusInfo, iWidJuToBonus),
				(GC.getNumImprovementInfos, GC.getImprovementInfo, iWidJuToImprovement),
				(GC.getNumCivilizationInfos, GC.getCivilizationInfo, iWidJuToCiv),
				(GC.getNumLeaderHeadInfos, GC.getLeaderHeadInfo, iWidJuToLeader),
				(GC.getNumTraitInfos, GC.getTraitInfo, iWidJuToTrait),
				(GC.getNumCivicInfos, GC.getCivicInfo, iWidJuToCivic),
				(GC.getNumReligionInfos, GC.getReligionInfo, iWidJuToReligion),
				(GC.getNumCorporationInfos, GC.getCorporationInfo, iWidJuToCorporation),
				(GC.getNumRouteInfos, GC.getRouteInfo, iWidJuToRoute),
				(GC.getNumBuildInfos, GC.getBuildInfo, "Builds")
				]
			aList = []
			for nInfos, getInfo, misc in aWorkList:
				for iType in range(nInfos()):
					info = getInfo(iType)
					szName = info.getDescription()
					if szName.find("<") == 0:
						i = szName.find(">") + 1
						szName = szName[i:]
					aList.append((szName, misc, iType, info.getButton()))
			aList.sort(key=itemgetter(0))
			self.aListLength = len(aList)

			# Draw Index
			eTableStd = TableStyles.TABLE_STYLE_STANDARD

			Y_INDEX = self.Y_INDEX
			W_INDEX = self.W_INDEX
			H_INDEX = self.H_INDEX
			nLists = self.nLists
			LIST = []
			for i in range(nLists):
				LIST.append("Index" + str(i))
				screen.addListBoxGFC(LIST[i], "", W_INDEX * i, Y_INDEX, W_INDEX, H_INDEX, eTableStd)

			H_LETTER = self.H_LETTER
			screen.addPanel("IndexLetters", "", "", False, False, 4, self.Y_LETTER, xRes - 4, H_LETTER, PanelStyles.PANEL_STYLE_EMPTY)

			szLetter = ""
			nRowList = []
			iX = 4
			dX = 11 + H_LETTER * H_LETTER / 100
			iLetter = 0
			iLimit = len(aList) / nLists
			iList = 0

			i = 0
			do = screen.appendListBoxStringNoUpdate
			iIconSize = self.iIconSize
			for szName, WIDGET, iType, path in aList:
				if WIDGET == "Builds":
					iOffset = 100000
					WIDGET = iWidJuToRoute
				else:
					iOffset = 0
#				if path.find(" ") != -1:
#					print path
				BUTTON = '<img=%s size=%d></img> %s' %(path, iIconSize, szFont + szName)
				if (szName[:1] != szLetter):
					if i >= iLimit:
						iList += 1
						nRowList.append(i + iLetter)
						i = 0
						iLetter = 0
						iTemp = W_INDEX * iList
						if iTemp - iX > 0:
							iX = 4 + iTemp
					szLetter = str(szName[:1])
					szHeader = szFont + "<color=245,245,0,255>" + "-----  " + szLetter + "  -----"
					do(LIST[iList], szHeader, eWidGen, 0, 0, 1<<2)
					# create letter button
					if szLetter == " ":
						szA = "&#95"
					else:
						szA = szLetter
					screen.setTextAt("Letter|" + szLetter, "IndexLetters", szFontEdge + szA, 1<<2, iX, 4, 0, eFontTitle, eWidGen, iList, i + iLetter)
					iLetter += 1
					iX += dX
				do(LIST[iList], BUTTON, WIDGET, iType - iOffset, 0, 1<<0)
				i += 1
			nRowList.append(i + iLetter)
			for i in range(iList + 1):
				screen.updateListBox(LIST[i])
				screen.enableSelect(LIST[i], False)
			self.nIndexLists = [nLists, nRowList]
		szText = szFontEdge + "Index - " + str(self.aListLength) + " entries"
		screen.setText(self.main.getNextWidgetName(), "", szText, 1<<2, xRes / 2, 0, 0, eFontTitle, eWidGen, 0, 0)
		return self.nIndexLists
