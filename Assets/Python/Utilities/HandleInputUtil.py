# Utility for input debugging.
import InputData
inData = InputData.instance
import DebugUtils

NOTIFY_CODES = {
	0	: "Click",
	1	: "Double Click",
	2	: "Focus",
	3	: "Unfocus",
	4	: "Mouse Enter",
	5	: "Mouse Leave",
	6	: "Character",
	7	: "Scroll Up",
	8	: "Scroll Down",
	9	: "New Horizontal Stop",
	10	: "New Vertical Stop",
	11	: "List Select",
	12	: "Mouse Move / Flyout Item Selected",
	14	: "Mouse Wheel Up",
	15	: "Mouse Wheel Down",
	16	: "Key Down",
	17	: "Key Up",
	18	: "Link Execute",
	19	: "Movie Done",
	20	: "Slider New Stop",
	21	: "Table Header Select"
}
MOUSE_FLAGS = {
	1			: "MOUSE_LBUTTON",
	2			: "MOUSE_RBUTTON",
	4			: "MOUSE_SHIFT",
	8			: "MOUSE_CONTROL",
	16			: "MOUSE_MBUTTON",
	32			: "MOUSE_XBUTTON1",
	64			: "MOUSE_XBUTTON2",
	4096		: "MOUSE_MOUSEMOVE",
	8192		: "MOUSE_LBUTTONDOWN",
	16384		: "MOUSE_LBUTTONUP",
	32768		: "MOUSE_LBUTTONDBLCLK",
	65536		: "MOUSE_RBUTTONDOWN",
	131072		: "MOUSE_RBUTTONUP",
	262144		: "MOUSE_RBUTTONDBLCLK",
	524288		: "MOUSE_MBUTTONDOWN",
	1048576		: "MOUSE_MBUTTONUP",
	2097152		: "MOUSE_MBUTTONDBLCLK",
	8388608		: "MOUSE_XBUTTONDOWN",
	8986624		: "MOUSE_CLICKED",
	16777216	: "MOUSE_XBUTTONUP",
	17973248	: "MOUSE_RELEASED",
	33554432	: "MOUSE_XBUTTONDBLCLK",
	35946496	: "MOUSE_DBLCLICKED",
	67108864	: "MOUSE_MOUSEWHEELUP",
	134217728	: "MOUSE_MOUSEWHEELDOWN"
}
def debugInput(inputClass):
	if not DebugUtils.bDebugMode: return
	# InputClass does not contain reliable information
	bAlt, bCtrl, bShift	= inData.getModifierKeys()
	# ------------------------------------------------
	iNotifyCode	= inputClass.eNotifyCode
	iFlag		= inputClass.uiFlags
	print "\n#############handleInput############"
	print "ePythonFileEnum = %d"					%inputClass.ePythonFileEnum
	print "eNotifyCode = %d\t\t%s"					%(iNotifyCode, NOTIFY_CODES.get(iNotifyCode, "Unknown"))
	print "uiFlags = %d\t\t%s"						%(iFlag, MOUSE_FLAGS.get(iFlag, "UNKNOWN"))
	print "bAlt = %s\nbCtrl = %s\nbShift = %s\n"	%(bAlt, bCtrl, bShift)
	print "WidgetName = %s"							%inputClass.szFunctionName
	print "iItemID = %d"							%inputClass.iItemID
	print "iData  = %d\niData1 = %d\niData2 = %d"	%(inputClass.iData, inputClass.iData1, inputClass.iData2)
	print "iButtonType = %d"						%inputClass.iButtonType
	print "bOption = %s"							%str(inputClass.bOption == 1)
	print "#############handleInput############\n"