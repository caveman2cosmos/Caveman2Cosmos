# Cached civic data
from CvPythonExtensions import CyGlobalContext

def initCivicData():
	GC = CyGlobalContext()
	print "CivicData.initCivicData"

	global civicLists
	civicLists = []
	for iCat in xrange(GC.getNumCivicOptionInfos()):
		civicLists.append([])

	for iCivic in xrange(GC.getNumCivicInfos()):
		info = GC.getCivicInfo(iCivic)
		civicLists[info.getCivicOptionType()].append((info, iCivic))
