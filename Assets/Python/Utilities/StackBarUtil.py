## Progress Bar Utilities
##
## Holds the information used to display tick marks on progress bars
## Also draws the tick marks
##
## Copyright (c) 2007-2008 The BUG Mod.
##
## Author: Ruff_Hi

from CvPythonExtensions import *

class StackBarDC:
	def __init__(self, DC, x, y, w, h, color):
		self.DC = DC
		self.x = x
		self.y = y
		self.w = w
		self.h = h
		self.color = color

		self.BG = CyArtFileMgr().getInterfaceArtInfo("INTERFACE_BUTTON_NULL").getPath()

		self.y1 = 4
		self.y2 = 8
		self.y3 = self.h - 8
		self.y4 = self.h - 4


	def drawTickMarks(self, screen, iCurr, iTotal, iRate):
		DC = self.DC
		screen.deleteWidget(DC)
		if iRate < 1: return

		w = self.w
		screen.addDrawControl(DC, self.BG, self.x, self.y, w, self.h, WidgetTypes.WIDGET_GENERAL, 1, 2)

		color = self.color
		y1 = self.y1
		y2 = self.y2
		y3 = self.y3
		y4 = self.y4
		i = 2
		while i < 514: # Limit it to maximum 512 tics, no progress-bar needs more than that.
			iProgress = iCurr + i * iRate
			if iProgress >= iTotal: break
			x = w * iProgress / iTotal

			screen.addLineGFC(DC, DC + str(i), x, y1, x, y2, color)
			screen.addLineGFC(DC, DC + str(-i), x, y3, x, y4, color)

			i += 1