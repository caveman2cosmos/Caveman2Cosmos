## By StrategyOnly converted to BUG by Dancing Hoskuld

from CvPythonExtensions import *
import CvUtil
import SdToolKit as SDTK

GC = CyGlobalContext()
TRNSLTR = CyTranslator()
GAME = GC.getGame()

def init():
	global giSparticus, giGladiator, giFieldMedic
	giSparticus = GC.getInfoTypeForString('UNITCLASS_SPARTACUS')
	giGladiator = GC.getInfoTypeForString('UNIT_GLADIATOR')
	giFieldMedic = GC.getInfoTypeForString('PROMOTION_RETINUE_MESSENGER')


def onUnitBuilt(self, argsList):
	CyCity, CyUnit = argsList
	CvAdvisorUtils.unitBuiltFeats(CyCity, CyUnit)
	## Hero Movie
	iPlayer = CyCity.getOwner()
	if not GAME.isNetworkMultiPlayer() and iPlayer == GAME.getActivePlayer() and isWorldUnitClass(CyUnit.getUnitClassType()):
		popupInfo = CyPopupInfo()
		popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
		popupInfo.setData1(CyUnit.getUnitType())
		popupInfo.setData2(CyCity.getID())
		popupInfo.setData3(4)
		popupInfo.setText("showWonderMovie")
		popupInfo.addPopup(iPlayer)


def onCombatResult(argsList):
	CyUnitW, CyUnitL = argsList

	# Spartacus Capture Event
	if CyUnitW.getUnitClassType() == giSparticus:

		# Capture % Random # 0 to 3 or 25%
		if not GAME.getSorenRandNum(4, "Gods"):

			pClearPlot = findClearPlot(CyUnitL)

			CyPlot = CyUnitL.plot()
			if pClearPlot != -1 and CyPlot.getNumUnits() == 1:
				CyPlot = CyUnitL.plot()
				CyUnitL.setXY(pClearPlot.getX(), pClearPlot.getY(), False, True, True)
			else:
				CyPlot = CyUnitW.plot()

			iPlayer = CyUnitW.getOwner()
			GC.getPlayer(iPlayer).initUnit(giGladiator, CyPlot.getX(), CyPlot.getY(), UnitAITypes.NO_UNITAI, DirectionTypes.DIRECTION_NORTH).finishMoves()

			CyUnitL.setDamage(100000, False)
			msg = TRNSLTR.getText("TXT_KEY_SPARTACUS_CAPTURE_SUCCESS",())
			icon = ',Art/Interface/Buttons/Units/ICBM.dds,Art/Interface/Buttons/Warlords_Atlas_1.dds,3,11'
			CvUtil.sendMessage(msg, iPlayer, 15, icon, ColorTypes(44), CyUnitL.getX(), CyUnitL.getY(), True, True, 0, "", False)


	# Field Medic
	if CyUnitW.isHasPromotion(giFieldMedic):
		# 10% chance
		if not GAME.getSorenRandNum(10, "Gods"):

			if not SDTK.sdObjectExists('Heroes', CyUnitW):
				iHealTurn = -1
			else:
				iHealTurn = SDTK.sdObjectGetVal( 'Heroes', CyUnitW, 'HealTurn' )

			if iHealTurn == None or GAME.getGameTurn() > iHealTurn:
				CyUnitW.setDamage(0, False)
				if not SDTK.sdObjectExists('Heroes', CyUnitW):
					SDTK.sdObjectInit('Heroes', CyUnitW, {})
				SDTK.sdObjectSetVal('Heroes', CyUnitW, 'HealTurn', GAME.getGameTurn())


def findClearPlot(pUnit):
	BestPlot = -1
	iBestPlot = 0
	pOldPlot = pUnit.plot()
	iX = pOldPlot.getX()
	iY = pOldPlot.getY()
	for iiX in range(iX-1, iX+2, 1):
		for iiY in range(iY-1, iY+2, 1):
			iCurrentPlot = 0
			pPlot = CyMap().plot(iiX,iiY)
			if pPlot.getNumUnits() == 0:
				iCurrentPlot = iCurrentPlot + 5
			if iCurrentPlot >= 1:
				iCurrentPlot = iCurrentPlot + GAME.getSorenRandNum(5, "findClearPlot")
				if iCurrentPlot >= iBestPlot:
					BestPlot = pPlot
					iBestPlot = iCurrentPlot
	return BestPlot
