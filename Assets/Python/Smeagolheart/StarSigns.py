# Star Signs
#
# Original code by Smeagolheart
#
# Feb 2020 adjusted for Caveman2Cosmos to limit the timeframe that these promotions can occur. See if test in CvEventManager.py

# Posible future improvements if someone wants to do them
#
# 1. Make promotions Culturally apropriate rather than just using the European/Mid-East zodiac for all nations.
#
# 2. Currently it is possible that the given promotion is of no use to the unit, eg attack boost to a unit that can't attack. No promotion given would be better.

from CvPythonExtensions import *

def give(GC, TRNSLTR, GAME, CyUnit, iPlayer, bLand):

	aStarSignList = (
		"PROMOTION_AQUARIUS",
		"PROMOTION_ARIES",
		"PROMOTION_CANCER",
		"PROMOTION_GEMINI",
		"PROMOTION_LEO",
		"PROMOTION_LIBRA",
		"PROMOTION_PISCES",
		"PROMOTION_SAGITTARIUS",
		"PROMOTION_SCORPIO",
		"PROMOTION_TAURUS",
		"PROMOTION_CAPRICORN",
		"PROMOTION_VIRGO"
	)
	if bLand:
		iChance = GAME.getSorenRandNum(12, "Star Signs") # integer 0-11
	else:
		iChance = GAME.getSorenRandNum(10, "Star Signs") # integer 0-9

	iPromotion = GC.getInfoTypeForString(aStarSignList[iChance])
	CyUnit.setHasPromotion(iPromotion, True)

	if iPlayer == GAME.getActivePlayer() and GAME.isFinalInitialized():
		CyCity = CyUnit.plot().getPlotCity()
		if CyCity:
			szTxt = TRNSLTR.getText("TXT_KEY_MSG_STARSIGN_BUILD", (CyCity.getName(),))
		else:
			szTxt = TRNSLTR.getText("TXT_KEY_MSG_STARSIGN_CREATE", ())

		szIcon = GC.getPromotionInfo(iPromotion).getButton()
		import CvUtil
		CvUtil.sendMessage(szTxt, iPlayer, 16, szIcon, ColorTypes(44), CyUnit.getX(), CyUnit.getY(), True, True)