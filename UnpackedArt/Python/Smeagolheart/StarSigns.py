# Star Signs
from CvPythonExtensions import *
import CvUtil

def give(GC, TRNSLTR, GAME, CyUnit, CyPlayer, CyCity = None, iPlayer = None):

	aStarSignList = (
		"PROMOTION_AQUARIUS",
		"PROMOTION_ARIES",
		"PROMOTION_CANCER",
		"PROMOTION_CAPRICORN",
		"PROMOTION_GEMINI",
		"PROMOTION_LEO",
		"PROMOTION_LIBRA",
		"PROMOTION_PISCES",
		"PROMOTION_SAGITTARIUS",
		"PROMOTION_SCORPIO",
		"PROMOTION_TAURUS",
		"PROMOTION_VIRGO"
	)

	iChance = GAME.getSorenRandNum(12, "Star Signs") # integer 0-11
	iPromotion = GC.getInfoTypeForString(aStarSignList[iChance])
	CyUnit.setHasPromotion(iPromotion, True)

	if CyPlayer.isHuman():
		if CyCity:
			szTxt = TRNSLTR.getText("TXT_KEY_MESSAGE_STARSIGN_BUILD", (CyCity.getName(),))
		else:
			szTxt = TRNSLTR.getText("TXT_KEY_MESSAGE_STARSIGN_CREATE", ())
			iPlayer = CyUnit.getOwner()
		szIcon = GC.getPromotionInfo(iPromotion).getButton()
		CvUtil.sendMessage(szTxt, iPlayer, 16, szIcon, ColorTypes(44), CyUnit.getX(), CyUnit.getY(), True, True)