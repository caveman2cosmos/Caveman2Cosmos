from glob import glob
from os.path import join

def getGlob(directory):
	paths  = glob(join(directory, "*.xml"))
	paths += glob(join(directory, "**", "*.xml"))
	paths += glob(join(directory, "**", "**", "*.xml"))
	paths += glob(join(directory, "**", "**", "**", "*.xml"))
	paths += glob(join(directory, "**", "**", "**", "**", "*.xml"))
	paths += glob(join(directory, "**", "**", "**", "**", "**", "*.xml"))
	return paths

def getTagValues(tags, paths):
	values = []
	for path in paths:
		print "Reading file: %s" %path
		file = open(path)
		for line in file:
			tracker = LineReader(line)
			tracker.skip((" ", "	", "<"))
			if tracker.splice(">") in tags:
				element = tracker.splice("<")
				if len(element) > 0 and not element in values:
					#print "Key found: %s" %element
					values.append(element)
		file.close()
	return values

class LineReader:

	def __init__(self, line):
		self.line = line
		self.location = 0

	def skip(self, chars):
		while self.line[self.location] in chars:
			self.location += 1

	def splice(self, endChar):
		data = ""
		for i in range(self.location, len(self.line)):
			self.location += 1
			if self.line[i] == endChar:
				return data
			data += self.line[i]
		return ""
