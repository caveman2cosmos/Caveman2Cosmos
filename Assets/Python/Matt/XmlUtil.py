from CvPythonExtensions import *
from SystemPaths import modDir
from glob import glob
import os
import os.path

DEBUG_LOG = True


def verifyModDir(szModPath):
	git_directory = os.path.join(szModPath, "git_directory.txt")
	if os.path.isfile(git_directory):
		pFile = open(git_directory)
		szModPath = pFile.read().rstrip("\n")
		pFile.close()
	return szModPath

g_ModDir = verifyModDir(modDir)


def getGlob(directory):
	paths  = glob(os.path.join(directory, "*.xml"))
	paths += glob(os.path.join(directory, "**", "*.xml"))
	paths += glob(os.path.join(directory, "**", "**", "*.xml"))
	#---------------------------------
	#if DEBUG_LOG: logGlob(paths)
	#---------------------------------
	return paths


def logGlob(paths):
	logFile = open(os.path.join(g_ModDir, "glob.log"), "w")
	for path in paths:
		logFile.write("%s\n" %path)
	logFile.close()


class LineReader:

	def __init__(self, line):
		self.line = line
		self.location = 0

	def skipWhiteSpace(self):
		while self.line[self.location] == "	" or self.line[self.location] == " ":
			self.location += 1

	def skipNextChar(self):
		self.location += 1

	def matchNextChar(self, char):
		if self.location >= len(self.line):
			return False
		current = self.line[self.location]
		self.location += 1
		return current == char

	def getNextSplice(self, endChar):
		data = ""
		for i in range(self.location, len(self.line)):
			if self.line[self.location] == endChar:
				#-----------------------------
				if DEBUG_LOG: self.log(data)
				#-----------------------------
				return data
			data += self.line[self.location]
			self.location += 1
		return None

	def log(self, msg):
		logFile = open(os.path.join(g_ModDir, "XmlUtil.log"), "a")
		logFile.write("%s\n" %msg)
		logFile.close()
