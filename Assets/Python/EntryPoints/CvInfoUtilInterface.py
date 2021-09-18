from CvPythonExtensions import CvInfoUtil, CyGlobalContext

GC = CyGlobalContext()
GAME = GC.getGame()


def init():
	for i in range(GC.getNumBuildingInfos()):
		pInfo = GC.getBuildingInfo(i)

		global g_infoType
		g_infoType = pInfo.getType()

		CvInfoUtil(pInfo).sendDataMembersToPython("CvInfoUtilInterface")

def handleInteger(args):
	i = args
	GAME.log("CvInfoUtil.log", g_infoType + " - Int recieved, value: %d" %i)

def handleEnum(args):
	e = args
	GAME.log("CvInfoUtil.log", g_infoType + " - Enum recieved, value: %d" %e)

def handleVector(args):
	v = args
	msg = g_infoType + " - Vector recieved"
	if v:
		values = ", values:"
		for i in v:
			values += "\s%d," %i
		msg += values.pop()
	GAME.log("CvInfoUtil.log", msg)

#def handleIDValueMap(args):