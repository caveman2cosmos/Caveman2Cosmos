# Data storage python
# The screen resolution for the game will be set by CvScreensInterface.py once when you launch the game.
# When you change resolutions the storage here will change accordingly, this happens in CvOptionsScreenCallbackInterface.py
# Some python code may need to know the screen resolution, there is no need to ask the dll about it if this file has been imported.

x = 0
y = 0
aFontList = ["<font=4b>", "<font=4>", "<font=3b>", "<font=3>", "<font=2b>", "<font=2>", "<font=1b>", "<font=1>"]
iFontScale = 2

def init(dir=None):
	global x, y
	print "ScreenResolution.init\nSet custom resolution from CivilizationIV.ini if found."
	import ConfigParser
	import os.path
	# Accept both the current caller and older onInit handlers which pass no path.
	if dir is None:
		import SystemPaths
		dir = SystemPaths.userDir
	if not dir:
		print "ScreenResolution: no user directory; using the game profile resolution."
		return
	path = os.path.join(dir, "CivilizationIV.ini")
	Config = ConfigParser.ConfigParser()
	try:
		loaded = Config.read(path)
	except (ConfigParser.Error, IOError):
		print "ScreenResolution: cannot read CivilizationIV.ini; using the game profile resolution."
		return
	if not loaded:
		print "ScreenResolution: CivilizationIV.ini not found; using the game profile resolution."
		return

	# Missing sections/options must not abort the game's entire Init event.
	# Do not create a partial INI when the file belongs to a different profile.
	dirty = False
	if Config.has_section("GAME"):
		szEra = ""
		if Config.has_option("GAME", "Era"):
			szEra = Config.get("GAME", "Era")
		if szEra[:3] != "C2C":
			Config.set("GAME", "Era", "C2C_ERA_PREHISTORIC")
			dirty = True
		szMainMenuMods = ""
		if Config.has_option("GAME", "MainMenuMods"):
			szMainMenuMods = Config.get("GAME", "MainMenuMods")
		if szMainMenuMods != "Caveman2Cosmos":
			Config.set("GAME", "MainMenuMods", "Caveman2Cosmos")
			dirty = True
	else:
		print "ScreenResolution: no GAME section; leaving CivilizationIV.ini unchanged."
	if dirty:
		try:
			file = open(path, 'wb')
			try:
				Config.write(file)
			finally:
				file.close()
		except IOError:
			print "ScreenResolution: cannot update CivilizationIV.ini; continuing with the game profile."

	X0 = Y0 = ""
	if Config.has_option("DEBUG", "ScreenWidth"):
		X0 = Config.get("DEBUG", "ScreenWidth")
	if Config.has_option("DEBUG", "ScreenHeight"):
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