from CvPythonExtensions import *
import SystemPaths as SP
import os
import os.path
import CvUtil

RemoveTag = (3, "BuildingClass")


def verifyModDir(szModPath):
    git_directory = os.path.join(szModPath, "git_directory.txt")
    if os.path.isfile(git_directory):
        pFile = open(git_directory)
        szModPath = pFile.read().rstrip("\n")
        pFile.close()
    return szModPath


def init():
	folder = os.path.join(verifyModDir(SP.modDir), "Assets", "Python", "Matt", "xml")
	if RemoveTag:
		for file in os.listdir(folder):
			removeTags(os.path.join(folder, file))


def removeTags(path):
	oldFile = open(path)
	newLines = ""
	log = ""
	for line in oldFile:
		numTabs = countWhiteSpace(line)
		tagName = getTagName(line, numTabs +1)
		log += "Tabs: %s Tag: %s\n" %(numTabs, tagName)
		if not (numTabs == RemoveTag[0] and tagName == RemoveTag[1]):
			newLines += line
	oldFile.close()

	newFile = open(path + ".new", "w")
	newFile.write(newLines)
	newFile.close()

	logFile = open(path + ".log", "w")
	logFile.write(log)
	logFile.close()


def countWhiteSpace(line):
	i = 0
	while line[i] == "	" or line[i] == " ":
		i += 1
	return i


def getTagName(line, tagStart):
	tagName = ""
	for i in range(tagStart, len(line)):
		if line[i] == ">":
			return tagName
		tagName += line[i]
	return tagName

