# LeaderCivNames.py
#
# by jdog5000
# Version 1.0

# This files allows special civ names to be assigned to certain leaders.  Intended mainly for mods.

#from CvPythonExtensions import *

#GC = CyGlobalContext()

# Initialize dict to empty
LeaderCivNames = dict()

# This function actually sets up the dict of civ names for leader types
def setup():
	global LeaderCivNames
	LeaderCivNames = dict()

	#iChurchill = GC.getInfoTypeForString('LEADER_CHURCHILL')
	#LeaderCivNames[iChurchill] = ["British Empire","Britain","British"]
