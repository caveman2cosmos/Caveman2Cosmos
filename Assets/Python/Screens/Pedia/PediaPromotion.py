# Pedia overhaul by Toffer for Caveman2Cosmos.

from CvPythonExtensions import *

class PediaPromotion:

	def __init__(self, parent, H_BOT_ROW):
		self.main = parent

		H_PEDIA_PAGE = parent.H_PEDIA_PAGE

		self.Y_TOP_ROW = Y_TOP_ROW = parent.Y_PEDIA_PAGE
		self.Y_BOT_ROW = Y_TOP_ROW + H_PEDIA_PAGE - H_BOT_ROW

		self.H_TOP_ROW = H_TOP_ROW = (H_PEDIA_PAGE - H_BOT_ROW * 3) / 4
		self.H_BOT_ROW = H_BOT_ROW

		self.S_ICON = H_TOP_ROW - 10

		self.Y_MID_ROW = Y_TOP_ROW + H_TOP_ROW
		self.H_MID_LEFT = H_PEDIA_PAGE - H_BOT_ROW - H_TOP_ROW
		self.H_MID_RIGHT = H_PEDIA_PAGE - H_BOT_ROW

		self.W_PEDIA_PAGE = W_PEDIA_PAGE = parent.W_PEDIA_PAGE

		self.W_HALF_PP = W_HALF_PP = W_PEDIA_PAGE / 2 - 4
		self.W_3RD_PP = W_PEDIA_PAGE / 3 - 4

		self.X_COL_1 = X_COL_1 = parent.X_PEDIA_PAGE
		self.X_COL_2 = X_COL_1 + W_HALF_PP + 8

	def interfaceScreen(self, iThePromotion):
		GC = CyGlobalContext()
		TRNSLTR = CyTranslator()
		CvThePromotionInfo = GC.getPromotionInfo(iThePromotion)
		screen = self.main.screen()

		iWidGen			= WidgetTypes.WIDGET_GENERAL
		iPanelBlue50	= PanelStyles.PANEL_STYLE_BLUE50
		iFontGame		= FontTypes.GAME_FONT

		enumGBS = self.main.enumGBS
		uFontEdge, uFont4b, uFont4, uFont3b, uFont3, uFont2b, uFont2 = self.main.aFontList

		X_COL_1 = self.X_COL_1
		X_COL_2 = self.X_COL_2
		Y_BOT_ROW = self.Y_BOT_ROW
		Y_MID_ROW = self.Y_MID_ROW
		Y_TOP_ROW = self.Y_TOP_ROW
		H_BOT_ROW = self.H_BOT_ROW
		H_TOP_ROW = self.H_TOP_ROW
		H_MID_LEFT = self.H_MID_LEFT
		H_MID_RIGHT = self.H_MID_RIGHT
		W_PEDIA_PAGE = self.W_PEDIA_PAGE
		W_HALF_PP = self.W_HALF_PP
		S_ICON = self.S_ICON

		aName = self.main.getNextWidgetName

		# Main Panel
		screen.setText(aName(), "", uFontEdge + CvThePromotionInfo.getDescription(), 1<<0, X_COL_1, 0, 0, FontTypes.TITLE_FONT, iWidGen, 0, 0)
		Pnl = aName()
		screen.addPanel(Pnl, "", "", False, False, X_COL_1 - 3, Y_TOP_ROW + 2, W_HALF_PP + 8, H_TOP_ROW + 2, PanelStyles.PANEL_STYLE_MAIN)
		Img = "ToolTip|PROMO" + str(iThePromotion)
		screen.setImageButtonAt(Img, Pnl, CvThePromotionInfo.getButton(), 4, 6, S_ICON, S_ICON, iWidGen, 1, 1)
		# Requires & Leads To
		PF = "ToolTip|JumpTo|"
		szAnd	= uFont3 + "&#38"
		szOr	= uFont2b + "||"
		szBracketL = uFont4b + " {"
		szBracketR = uFont4b + "} "
		szLogic = ""
		aList1 = [] # Requires
		iType = CvThePromotionInfo.getTechPrereq()
		if iType > -1:
			aList1.append((GC.getTechInfo(iType).getButton(), "TECH" + str(iType), ""))
			szLogic = szAnd
		iType = CvThePromotionInfo.getStateReligionPrereq()
		if iType > -1:
			aList1.append((GC.getReligionInfo(iType).getButton(), "REL" + str(iType), szLogic))
			szLogic = szAnd
		iType = CvThePromotionInfo.getPrereqPromotion()
		if iType > -1:
			aList1.append((GC.getPromotionInfo(iType).getButton(), "PROMO" + str(iType), szLogic))
			szLogic = ""
		iOr1 = CvThePromotionInfo.getPrereqOrPromotion1()
		iOr2 = CvThePromotionInfo.getPrereqOrPromotion2()
		if iOr1 > -1 and iOr2 > -1:
			szLogic += szBracketL
		if iOr1 > -1:
			aList1.append((GC.getPromotionInfo(iOr1).getButton(), "PROMO" + str(iOr1), szLogic))
			szLogic = szOr
		if iOr2 > -1:
			aList1.append((GC.getPromotionInfo(iOr2).getButton(), "PROMO" + str(iOr2), szLogic))
		aList2 = [] # Leads To
		for iType in xrange(GC.getNumPromotionInfos()):
			CvPromotionInfo = GC.getPromotionInfo(iType)
			if iThePromotion in (CvPromotionInfo.getPrereqPromotion(), CvPromotionInfo.getPrereqOrPromotion1(), CvPromotionInfo.getPrereqOrPromotion2()):
				aList2.append((CvPromotionInfo.getButton(), "PROMO" + str(iType)))
		if aList1 or aList2:
			iListLength1 = len(aList1)
			iListLength2 = len(aList2)
			if aList1 and aList2:
				W_REQ = W_LTO = W_HALF_PP
				X_LTO = X_COL_2
				W_3RD_PP = self.W_3RD_PP
				if iListLength1 < 4:
					if iListLength2 > 4:
						W_REQ = W_3RD_PP
						W_LTO = W_PEDIA_PAGE - W_3RD_PP - 4
						X_LTO = X_COL_1 + W_REQ + 4
				elif iListLength2 < 4:
					if iListLength1 > 4:
						W_REQ = W_PEDIA_PAGE - W_3RD_PP - 4
						W_LTO = W_3RD_PP
						X_LTO = X_COL_1 + W_REQ + 4
				RequirePanel = aName()
				LeadsToPanel = aName()
				screen.addPanel(RequirePanel, TRNSLTR.getText("TXT_KEY_PEDIA_REQUIRES", ()), "", False, True, X_COL_1, Y_BOT_ROW, W_REQ, H_BOT_ROW, iPanelBlue50)
				screen.addPanel(LeadsToPanel, TRNSLTR.getText("TXT_KEY_PEDIA_LEADS_TO", ()), "", False, True, X_LTO, Y_BOT_ROW, W_LTO, H_BOT_ROW, iPanelBlue50)
			elif aList1:
				RequirePanel = aName()
				screen.addPanel(RequirePanel, TRNSLTR.getText("TXT_KEY_PEDIA_REQUIRES", ()), "", False, True, X_COL_1, Y_BOT_ROW, W_PEDIA_PAGE, H_BOT_ROW, iPanelBlue50)
			elif aList2:
				LeadsToPanel = aName()
				screen.addPanel(LeadsToPanel, TRNSLTR.getText("TXT_KEY_PEDIA_LEADS_TO", ()), "", False, True, X_COL_1, Y_BOT_ROW, W_PEDIA_PAGE, H_BOT_ROW, iPanelBlue50)
			if aList1:
				for i in xrange(iListLength1):
					BTN, szChild, szLogic = aList1[i]
					if i != 0:
						screen.attachLabel(RequirePanel, "", szLogic)
					screen.attachImageButton(RequirePanel, PF + szChild, BTN, enumGBS, iWidGen, 1, 1, False)
				if szLogic == szOr:
					screen.attachLabel(RequirePanel, "", szBracketR)
				aList1 = []
			if aList2:
				for i in xrange(iListLength2):
					BTN, szChild = aList2[i]
					screen.attachImageButton(LeadsToPanel, PF + szChild, BTN, enumGBS, iWidGen, 1, 1, False)
				aList2 = []
		else:
			H_MID_LEFT += H_BOT_ROW
			H_MID_RIGHT += H_BOT_ROW
		# Promotion Help
		szText = CyGameTextMgr().getPromotionHelp(iThePromotion, True)[1:]
		if szText:
			szText = uFont3 + szText
			screen.addPanel(aName(), "", "", True, False, X_COL_1, Y_MID_ROW, W_HALF_PP, H_MID_LEFT, iPanelBlue50)
			screen.addMultilineText(aName(), szText, X_COL_1 + 4, Y_MID_ROW + 12, W_HALF_PP - 8, H_MID_LEFT - 20, iWidGen, 0, 0, 1<<0)

		# Unit Combats
		for i in xrange(CvThePromotionInfo.getNumQualifiedUnitCombatTypes()):
			aList1.append(CvThePromotionInfo.getQualifiedUnitCombatType(i))

		for i in xrange(CvThePromotionInfo.getNumDisqualifiedUnitCombatTypes()):
			aList2.append(CvThePromotionInfo.getDisqualifiedUnitCombatType(i))

		if aList1 or aList2:
			szChild = PF + "COMBAT"
			if uFont3b == "<font=3b>":
				sIcon = 32
			elif uFont3b == "<font=2b>":
				sIcon = 28
			else:
				sIcon = 24
			dy = sIcon + 2

			n = 0
			screen.addPanel(aName(), "", "", True, True, X_COL_2, Y_TOP_ROW + 2, W_HALF_PP, H_MID_RIGHT - 2, iPanelBlue50)
			ScrlPnl = aName()
			screen.addScrollPanel(ScrlPnl, "", X_COL_2, Y_TOP_ROW + 8, W_HALF_PP, H_MID_RIGHT - 38, iPanelBlue50)
			screen.setStyle(ScrlPnl, "ScrollPanel_Alt_Style")
			y = -4
			if aList1:
				screen.setTextAt(aName(), ScrlPnl, uFont3b + TRNSLTR.getText("TXT_KEY_VALID_FOR", ()), 1<<0, 0, y, 0, iFontGame, iWidGen, 1, 2)
				y += 4 + dy
				for iType in aList1:
					CvUnitCombatInfo = GC.getUnitCombatInfo(iType)
					screen.addDDSGFCAt(szChild + str(iType) + "|" + str(n), ScrlPnl, CvUnitCombatInfo.getButton(), 0, y, sIcon, sIcon, iWidGen, 1, 2, False)
					n += 1
					szText = "<color=230,230,0,255>" + uFont2b + CvUnitCombatInfo.getDescription()
					screen.setTextAt(szChild + str(iType) + "|" + str(n), ScrlPnl, szText, 1<<0, sIcon + 4, y+6, 0, iFontGame, iWidGen, 1, 2)
					n += 1
					y += dy
				y += dy

			if aList2:
				screen.setTextAt(aName(), ScrlPnl, uFont3b + TRNSLTR.getText("TXT_KEY_VALID_FOR_NOT", ()), 1<<0, 0, y, 0, iFontGame, iWidGen, 1, 2)
				y += 4 + dy
				for iType in aList2:
					CvUnitCombatInfo = GC.getUnitCombatInfo(iType)
					screen.addDDSGFCAt(szChild + str(iType) + "|" + str(n), ScrlPnl, CvUnitCombatInfo.getButton(), 0, y, sIcon, sIcon, iWidGen, 1, 2, False)
					n += 1
					szText = "<color=255,80,80,255>" + uFont2b + CvUnitCombatInfo.getDescription()
					screen.setTextAt(szChild + str(iType) + "|" + str(n), ScrlPnl, szText, 1<<0, sIcon + 4, y+6, 0, iFontGame, iWidGen, 1, 2)
					n += 1
					y += dy
