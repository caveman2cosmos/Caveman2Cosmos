#
# TestCode - tab within DebugScreen
#
# Toffer - I intentionally made this very bare bones, hiding technical stuff inside DebugScreen.py
#	This so that python novices can utilize this file as a training/testing ground with minimal intimidation.
from CvPythonExtensions import *

class TestCode:

	def __init__(self, pDebugScreen):
		self.main = pDebugScreen

	# Use this instead of print, it will print to pythonDbg and will also print on screen.
	def log(self, text):
		self.main.testCodelog(text)

	def drawTab(self, screen):
		# Toffer - Add new test code button to the debug screen like this and define the function below.
		#	This is all that is needed for the button to become functional, one line.
		#self.main.addTestCode(screen, functionName, "Button Text", "tooltip")
		self.main.addTestCode(screen, self.test0, "Button Text", "Empty test")
		self.main.addTestCode(screen, self.test1, "Name For Button", "help text explaining the test purpose deeper")

	def test0(self):
		# Toffer - Do Whatever, log whatever.
		self.log("Line 1 example")
		a = 0
		for i in xrange(10000000):
			a += 1
		self.log("Line 2 example")

	def test1(self):
		pass