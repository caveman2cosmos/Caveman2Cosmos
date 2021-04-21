from os.path import join
from XmlUtil import getTagValues, getGlob
import SystemPaths as SP

def main():
	xmlDir = getXmlDir()
	dirCiv4 = SP.dirBtS[len(SP.dirBtS) - len("//Beyond the Sword//Civ4BeyondSword.exe") : len(SP.dirBtS)]
	print "Searching text files for keys"
	files  = getGlob(join(xmlDir, "Text"))
	files += getGlob(join(SP.dirBtS, "Assets", "XML", "Text"))
	files += getGlob(join(dirCiv4, "Assets", "XML", "Text"))
	files += getGlob(join(dirCiv4, "Warlords", "Assets", "XML", "Text"))
	existingKeys = getTagValues(("Tag",), files)
	print "Searching xml files for text key references"
	referencedKeys = getTagValues(("Description", "Help"), getGlob(xmlDir))
	print "Validating text keys"
	validate(referencedKeys, existingKeys)

def validate(referencedKeys, existingKeys):
	for key in referencedKeys:
		if not key in existingKeys:
			print "Referenced key not found: %s" %key

def getXmlDir():
	try:
		from pathlib import Path
		return tuple(path for path in (Path(__file__).parents[2]).resolve() / "Assets" / "XML")
	except:
		from CvPythonExtensions import getModDir
		return join(getModDir(), "Assets", "XML")
