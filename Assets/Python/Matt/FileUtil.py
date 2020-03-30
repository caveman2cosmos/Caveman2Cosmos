from CvPythonExtensions import *
import SystemPaths as SP
import os.path
import glob


def verifyModDir(szModPath):
    git_directory = os.path.join(szModPath, "git_directory.txt")
    if os.path.isfile(git_directory):
        pFile = open(git_directory)
        szModPath = pFile.read().rstrip("\n")
        pFile.close()
    return szModPath

g_ModPath = verifyModDir(SP.modDir)


dllFiles = ["CyCityInterface1.cpp"]

def init():
	logFile = open(os.path.join(g_ModPath, "Assets", "Python", "Matt", "log.txt"), "w")

	C2CPythonFiles = getGlob(os.path.join(g_ModPath, "Assets", "Python"))
	for fileName in dllFiles:
		path = os.path.join(g_ModPath, "Sources", fileName)
		pFile = open(path)
		usedFunctions = ""
		for line in pFile:
			functionName = getFunctionName(line)
			logFile.write(functionName + "\n")
			if len(functionName) == 0 or find(C2CPythonFiles, functionName):
				usedFunctions += line
		pFile.close()

		logFile.close()

		newFile = open(path + ".new", "w")
		newFile.write(usedFunctions)
		newFile.close()


def getFunctionName(line):
	functionName = ""
	if line[2:7] == ".def(":
		position = 8
		while line[position] != "\"":
			functionName += line[position]
			position += 1
	return functionName


def getGlob(path):
	files  = glob.glob(os.path.join(path, "*.py"))
	files += glob.glob(os.path.join(path, "**", "*.py"))
	files += glob.glob(os.path.join(path, "**", "**", "*.py"))
	return files


def find(files, chars):
	for file in files:
		for line in open(file):
			if chars in line:
				return True
	return False

