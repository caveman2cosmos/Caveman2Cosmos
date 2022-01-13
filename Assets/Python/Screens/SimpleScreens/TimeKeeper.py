from CvPythonExtensions import *

def TimeKeeper():
	import CvScreenEnums
	import ScreenResolution
	xRes = ScreenResolution.x
	yRes = ScreenResolution.y
	GC = CyGlobalContext()
	TRNSLTR = CyTranslator()
	# Cache Era data
	iEras = GC.getNumEraInfos()
	szColorEra = "<font=3>" + TRNSLTR.getText("[COLOR_UNIT_TEXT]", ())
	aListEra = []
	iEra = 0
	while iEra < iEras:
		CvInfo = GC.getEraInfo(iEra)
		aListEra.append((CvInfo.getStartPercent(), szColorEra + CvInfo.getDescription()))
		iEra += 1
	# Create table
	screen = CyGInterfaceScreen("TimeKeeperScreen", CvScreenEnums.TIMEKEEPER)
	screen.addPanel("", "", "", True, False, -10, -10, xRes + 20, yRes + 20, PanelStyles.PANEL_STYLE_MAIN)
	screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)

	iNumColumns = GC.getNumGameSpeedInfos()
	TABLE = "TimeKeeperTable"
	screen.addTableControlGFC(TABLE, iNumColumns + 1, 0, 0, xRes, yRes, True, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD )
	screen.setTableColumnHeader(TABLE, 0, "", 120)
	iMaxIncrements = 0
	aListGS = []
	for i in xrange(iNumColumns):
		CvInfo = GC.getGameSpeedInfo(i)
		iTurnIncrements = CvInfo.getNumTurnIncrements()
		if iTurnIncrements > iMaxIncrements:
			iMaxIncrements = iTurnIncrements
		aListGS.append((CvInfo, iTurnIncrements))

		screen.setTableColumnHeader(TABLE, i + 1, CvInfo.getDescription(), (xRes - 120)/iNumColumns)

	for i in xrange(5*iMaxIncrements + iEras + 3):
		screen.appendTableRow(TABLE)

	# Cache misc content
	szColorSel = TRNSLTR.getText("[COLOR_SELECTED_TEXT]", ())
	szStartYear = "<font=3>" + TRNSLTR.getText("TXT_KEY_WB_START_YEAR", ())
	szTurns = "<font=3>" + TRNSLTR.getText("TXT_KEY_TURNS", ())
	szTurnsCol = szColorSel + szTurns
	szColorSel += "<font=3>"
	szIncrement = "<font=3>" + TRNSLTR.getText("TXT_KEY_INCREMENT", ())
	szDuration = "<font=3>" + TRNSLTR.getText("TXT_KEY_DURATION", ())
	szEndYear = szColorSel + TRNSLTR.getText("TXT_KEY_END_YEAR", ())
	eCalNoSeasons = CalendarTypes.CALENDAR_NO_SEASONS
	eWidGen = WidgetTypes.WIDGET_GENERAL
	iStartYear = GC.getGame().getStartYear()
	# Fill table
	iCol = 0
	while iCol < iNumColumns:
		CvGameSpeedInfo, iTurnIncrements = aListGS.pop(0)
		iTotalTurns = 0
		iRow = 0
		for j in xrange(iTurnIncrements):
			iTurns = CvGameSpeedInfo.getGameTurnInfo(j).iNumGameTurnsPerIncrement
			iIncrement = CvGameSpeedInfo.getDateIncrement(j).iIncrementDay + 30*CvGameSpeedInfo.getDateIncrement(j).iIncrementMonth
			screen.setTableText(TABLE, 0, iRow, szStartYear, "", eWidGen, 1, 2, 1<<0)
			screen.setTableText(TABLE, iCol+1, iRow, "<font=3>" + CyGameTextMgr().getDateStr(iTotalTurns, False, eCalNoSeasons, iStartYear, iCol), "", eWidGen, 1, 2, 1<<0)
			iRow += 1
			screen.setTableText(TABLE, 0, iRow, szTurns, "", eWidGen, 1, 2, 1<<0)
			screen.setTableText(TABLE, iCol+1, iRow, "<font=3>" + str(iTurns), "", eWidGen, 1, 2, 1<<0)
			iRow += 1
			screen.setTableText(TABLE, 0, iRow, szIncrement, "", eWidGen, 1, 2, 1<<0)
			screen.setTableText(TABLE, iCol+1, iRow, "<font=3>" + separateYearMonthDay(iIncrement), "", eWidGen, 1, 2, 1<<0)
			iRow += 1
			screen.setTableText(TABLE, 0, iRow, szDuration, "", eWidGen, 1, 2, 1<<0)
			screen.setTableText(TABLE, iCol+1, iRow, "<font=3>" + separateYearMonthDay(iTurns * iIncrement), "", eWidGen, 1, 2, 1<<0)
			iRow += 2
			iTotalTurns += iTurns

		iRow = 5*iMaxIncrements
		screen.setTableText(TABLE, 0, iRow, szEndYear, "", eWidGen, 1, 2, 1<<0)
		screen.setTableText(TABLE, iCol+1, iRow, szColorSel + CyGameTextMgr().getDateStr(iTotalTurns, False, eCalNoSeasons, iStartYear, iCol), "", eWidGen, 1, 2, 1<<0)
		iRow += 1
		screen.setTableText(TABLE, 0, iRow, szTurnsCol, "", eWidGen, 1, 2, 1<<0)
		screen.setTableText(TABLE, iCol+1, iRow, szColorSel + str(iTotalTurns), "", eWidGen, 1, 2, 1<<0)
		iRow += 2

		for iStartPercent, szName in aListEra:
			iStartTurn = iStartPercent * iTotalTurns / 100
			screen.setTableText(TABLE, 0, iRow, szName, "", eWidGen, 1, 2, 1<<0)
			screen.setTableText(TABLE, iCol+1, iRow, szColorEra + CyGameTextMgr().getDateStr(iStartTurn, False, eCalNoSeasons, iStartYear, iCol), "", eWidGen, 1, 2, 1<<0)
			iRow += 1
		iCol += 1

def separateYearMonthDay(iValue):
	iValueYear = iValue / 360
	iValueMonth = (iValue % 360)/30
	iValueDay = (iValue % 360) % 30
	sValue = ""
	if iValueYear > 0:
		sValue += str(iValueYear) + "y "
	if iValueMonth > 0:
		sValue += str(iValueMonth) + "m "
	if iValueDay > 0:
		sValue += str(iValueDay) + "d"
	return sValue
