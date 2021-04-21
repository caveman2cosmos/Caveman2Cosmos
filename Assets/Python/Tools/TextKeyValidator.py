from os.path import join
from XmlUtil import getTagValues, getGlob

def main():
	xmlDir = getXmlDir()
	print "Searching text files for keys"
	existingKeys = getTagValues(("Tag",), getGlob(join(xmlDir, "Text")))
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
