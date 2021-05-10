from os.path import join
from XmlUtil import getTagValues, getGlob

def main():
	xmlDir = getXmlDir()
	existingKeys = getTagValues(("Tag",), getGlob(join(xmlDir, "GameText")))
	referencedKeys = getTagValues(("Description", "Help", "Civilopedia", "Strategy", "Quote"), getGlob(xmlDir))
	print "Validating text keys"
	for key in referencedKeys:
		if key.startswith("TXT_KEY_") and not key in existingKeys:
			print "Referenced key not found: %s" %key

def getXmlDir():
	try:
		from pathlib import Path
		return tuple(path for path in (Path(__file__).parents[2]).resolve() / "Assets" / "XML")
	except:
		from CvPythonExtensions import getModDir
		return join(getModDir(), "Assets", "XML")
