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

def handleString(args):
	pass

def handleArray(args):
	a = args
	msg = g_infoType + " - Array recieved"
	_formatValues(a, msg)
	GAME.log("CvInfoUtil.log", msg)

def handle2DArray(args):
	pass

def handleVector(args):
	v = args
	msg = g_infoType + " - Vector recieved"
	_formatValues(v, msg)
	GAME.log("CvInfoUtil.log", msg)

def handleIDValueMap(args):
	pass

def _formatValues(seq, msg):
	if seq:
		values = ", values:"
		for i in seq:
			assert(i is not None)
			try:
				if isinstance(i, int):
					values += "\s%d," %i
				else:
					values += "\s%d," %int(i)
				values.pop()
			except:
				try:
					values += "\s" + str(i) + ","
					values.pop()
				except:
					values += "\sFormat list values failed!"
		msg += values