## By StrategyOnly converted to BUG by Dancing Hoskuld

from CvPythonExtensions import CyGlobalContext
import SdToolKit as SDTK

def init():
	global giFieldMedic
	giFieldMedic = CyGlobalContext().getInfoTypeForString('PROMOTION_RETINUE_MESSENGER')
'''
def onUnitBuilt(self, argsList):
	CyCity, CyUnit = argsList
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
'''
def onCombatResult(argsList):
	CyUnitW, CyUnitL = argsList

	# Field Medic
	if CyUnitW.isHasPromotion(giFieldMedic):
		GAME = CyGlobalContext().getGame()
		# 10% chance
		if not GAME.getSorenRandNum(10, "Gods"):

			if not SDTK.sdObjectExists('Heroes', CyUnitW):
				iHealTurn = -1
			else:
				iHealTurn = SDTK.sdObjectGetVal('Heroes', CyUnitW, 'HealTurn')

			iTurn = GAME.getGameTurn()
			if iHealTurn == None or iTurn > iHealTurn:
				CyUnitW.setDamage(0, False)
				if not SDTK.sdObjectExists('Heroes', CyUnitW):
					SDTK.sdObjectInit('Heroes', CyUnitW, {})
				SDTK.sdObjectSetVal('Heroes', CyUnitW, 'HealTurn', iTurn)