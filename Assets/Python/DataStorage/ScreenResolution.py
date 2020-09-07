# Data storage python
# The screen resolution for the game will be set by CvScreensInterface.py once when you launch the game.
# When you change resolutions the storage here will change accordingly, this happens in CvOptionsScreenCallbackInterface.py
# Some python code may need to know the screen resolution, there is no need to ask the dll about it if this file has been imported.

x = 0
y = 0
aFontList = ["<font=4b>", "<font=4>", "<font=3b>", "<font=3>", "<font=2b>", "<font=2>", "<font=1b>", "<font=1>"]
iFontScale = 2

def init(dir):
	global x, y
	print "ScreenResolution.init\nSet custom resolution from CivilizationIV.ini if found."
	import ConfigParser
	path = dir + "\CivilizationIV.ini"
	Config = ConfigParser.ConfigParser()
	Config.read(path)
	# Set correct default era if current is invalid.
	szEra = Config.get("GAME", "Era")
	if szEra[:3] != "C2C":
		Config.set("GAME", "Era", "C2C_ERA_PREHISTORIC")
		file = open(path, 'wb')
		Config.write(file)
		file.close()
	#Config.sections()
	X0 = Config.get("DEBUG", "ScreenWidth")
	Y0 = Config.get("DEBUG", "ScreenHeight")
	if X0.isdigit() and Y0.isdigit():
		X0 = int(X0)
		Y0 = int(Y0)
		if X0 > 0 and Y0 > 0:
			if X0 < 1024:
				X0 = 1024
			if Y0 < 768:
				Y0 = 768
			x = X0
			y = Y0
			print "Resolution: %dx%d\nScreenResolution.init - END" %(x, y)
			calibrate()
			return
	print "No custom resolution found.\nValue stored in profileName.pfl will be used instead.\nScreenResolution.init - END"

def calibrate():
	global aFontList, iFontScale
	if x > 1700:
		if x > 2500:
			aFontList = ["<font=4b>", "<font=4b>", "<font=4b>", "<font=4>", "<font=3b>", "<font=3>", "<font=2b>", "<font=2>"]
			iFontScale = 3
		else:
			aFontList = ["<font=4b>", "<font=4>", "<font=3b>", "<font=3>", "<font=2b>", "<font=2>", "<font=1b>", "<font=1>"]
			iFontScale = 2

	elif x > 1400:
		aFontList = ["<font=3b>", "<font=3>", "<font=2b>", "<font=2>", "<font=1b>", "<font=1>", "<font=0b>", "<font=0b>"]
		iFontScale = 1
	else:
		aFontList = ["<font=2b>", "<font=2>", "<font=1b>", "<font=1>", "<font=0b>", "<font=0>", "<font=0>", "<font=0>"]
		iFontScale = 0