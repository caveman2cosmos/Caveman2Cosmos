# LeaderCivNames.py
#
# by jdog5000
# Version 1.0

# This files allows special civ names to be assigned to certain leaders.  Intended mainly for mods.

from CvPythonExtensions import *
import CvUtil

gc = CyGlobalContext()

# Initialize dict to empty
LeaderCivNames = dict()

# This function actually sets up the dict of civ names for leader types
def setup( ) :

	#print "Setting up leader civ names dict"

	global LeaderCivNames

	LeaderCivNames = dict()

	#iChurchill = CvUtil.findInfoTypeNum(gc.getLeaderHeadInfo,gc.getNumLeaderHeadInfos(),'LEADER_CHURCHILL')
	#LeaderCivNames[iChurchill] = ["British Empire","Britain","British"]
