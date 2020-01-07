from CvPythonExtensions import *

def readC2CIni(Config, path):
	Config.set("GAME", "Era", "ERA_INDUSTRIAL")
	file = open(path, 'wb')
	Config.write(file)
	file.close()