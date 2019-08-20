import _core
import _misc

class InputData:
	def __init__(self):
		self.App = App = _core.App()
		App.MainLoop()

	def getModifierKeys(self):
		bAlt = _misc.GetKeyState(307)
		bCtrl = _misc.GetKeyState(308)
		bShift = _misc.GetKeyState(306)
		return [bAlt, bCtrl, bShift]

	def isKeyDown(self, iKey):
		aMap = { # https://wiki.wxwidgets.org/KeyCodes
			13	: 65,	# A
			16	: 68,	# D
			31	: 83,	# S
			35	: 87,	# W
			100	: 317,	# Up
			102	: 316,	# Left
			103	: 318,	# Right
			105	: 319,	# Down
		}
		if iKey in aMap:
			iKey = aMap[iKey]
			return _misc.GetKeyState(iKey)
		print "InputData.isKeyDown() - Warning\n\tUnknown key: %d" %iKey
		return "Unknown"

	def getMousePosition(self):
		return _misc.GetMousePosition()

instance = InputData()