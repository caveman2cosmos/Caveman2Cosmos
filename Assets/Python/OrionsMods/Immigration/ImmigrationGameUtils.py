# # Sid Meier's Civilization 4
# # Copyright Firaxis Games 2007
# # ImmigrationGameUtils
from CvPythonExtensions import *
import Immigration

# globals
GC = CyGlobalContext()

# Immigration Mod By Orion Veteran
# Modular Python to eliminate Merging
class ImmigrationGameUtils:

	def AI_unitUpdate(self, argsList):
		CyUnit, = argsList

		if CyUnit.getUnitType() == GC.getInfoTypeForString("UNIT_IMMIGRANT"):
			iPlayer = CyUnit.getOwner()
			CyPlayer = GC.getPlayer(iPlayer)
			if CyPlayer.isAlive() and not CyPlayer.isNPC():
				CyCity = Immigration.getLeastPopulatedCity(CyPlayer)
				if CyCity:
					Immigration.doImmigrantPlacementAI(CyUnit, CyCity, CyPlayer, iPlayer, True)
					return True
		return False