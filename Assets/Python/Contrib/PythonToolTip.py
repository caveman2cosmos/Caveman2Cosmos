from CvPythonExtensions import *

import ScreenResolution as SR

CyIF = CyInterface()


class PythonToolTip:
	def __init__(self):
		self.szTextTT = None
		self.iOffsetTT = [0, 0]
		self.bLockedTT = False

	def handle(self, screen, szText = None, xPos = -1, yPos = -1, uFont = ""):
		if not szText:
			# None means something, while sending in an empty string is a fluke.
			if szText is None:
				# Move tooltip to cursor
				POINT = Win32.getCursorPos()
				iX = POINT.x + self.iOffsetTT[0]
				iY = POINT.y + self.iOffsetTT[1]
				if iX < 0: iX = 0
				if iY < 0: iY = 0
				screen.moveItem("Tooltip", iX, iY, 0)
			return

		if szText != self.szTextTT:
			self.szTextTT = szText
			if not uFont:
				uFont = SR.aFontList[5]
			iX, iY = makeTooltip(screen, xPos, yPos, szText, uFont, "Tooltip")
			POINT = Win32.getCursorPos()
			self.iOffsetTT = [iX - POINT.x, iY - POINT.y]

		else: # Already cached, move tooltip to cursor and make it visible
			if xPos == yPos == -1:
				POINT = Win32.getCursorPos()
				screen.moveItem("Tooltip", POINT.x + self.iOffsetTT[0], POINT.y + self.iOffsetTT[1], 0)
			screen.moveToFront("Tooltip")
			screen.show("Tooltip")

		self.bLockedTT = xPos == yPos == -1


	def reset(self, screen):
		screen.hide("Tooltip")
		self.bLockedTT = False

def makeTooltip(screen, xPos, yPos, szTxt, uFont, listBox):
	bSetX = False
	bSetY = False
	if xPos == -1: bSetX = True
	if yPos == -1: bSetY = True
	if bSetX or bSetY:
		POINT = Win32.getCursorPos()
		if bSetX:
			xPos = POINT.x
		if bSetY:
			yPos = POINT.y
	xRes = SR.x
	yRes = SR.y

	# Enforce certain structure
	szTxt = szTxt.replace("\n</color>", "</color>\n")
	# Remove whitespace at the very end.
	while szTxt[-1:] in ("\n", "\r", "\t", " "):
		szTxt = szTxt[:-1]

	# Remove font literals
	i1 = szTxt.find("<font=")
	if i1 > -1:
		while i1 > -1:
			i2 = szTxt.find(">", i1+6)
			szTxt = szTxt[:i1] + szTxt[i2+1:]
			i1 = szTxt.find("<font=")
		szTxt = szTxt.replace("</font>", "")
	# Remove link literals
	i1 = szTxt.find("<link=")
	if i1 > -1:
		while i1 > -1:
			i2 = szTxt.find(">", i1+6)
			szTxt = szTxt[:i1] + szTxt[i2+1:]
			i1 = szTxt.find("<link=")
		szTxt = szTxt.replace("</link>", "")
	# Find longest text line in pixels
	aList = szTxt.split('\n')
	aWidthList = []
	dxMax = xRes/3
	dx = 0
	for aLine in aList:
		iWidth = CyIF.determineWidth(uFont + aLine)
		aWidthList.append((iWidth, aLine))
		if iWidth > dx:
			dx = iWidth
	if dx > dxMax:
		dx = dxMax
	elif not dx:
		print "CvMainInterface: This will probably never happen, but hey it did."
		dx = 1
	iMaxX = dx - 24
	# Wrap long lines
	aList = []
	COLOR = ""
	for iWidth, aLine in aWidthList:
		iMoreLines = float(iWidth) / dx
		if iMoreLines > 1.0:
			# The line is too long, break it into a word list.
			idxImage = aLine.find("<img=")
			if idxImage == -1:
				aWordList = aLine.split(" ")
			else: # We got image codes.
				aWordList = []
				while True:
					i = aLine.find(" ")
					if i != -1 and i < idxImage:
						aWordList.append(aLine[:i])
						aLine = aLine[i+1:]
						idxImage -= i+1
						continue
					else:
						if idxImage > 0:
							aWordList.append(aLine[:idxImage])
							aLine = aLine[idxImage:]
							idxImage = 0
						idx1 = aLine.find("/img>") + 5
						idx2 = aLine.find("/>") + 2
						if idx1 > 4 or idx2 > 1:
							if idx1 > 4 and idx2 > 1:
								if idx1 < idx2:
									idx0 = idx1
								else:
									idx0 = idx2
							else:
								if idx1 > 4:
									idx0 = idx1
								else:
									idx0 = idx2
								if len(aLine) == idx0:
									aWordList.append(aLine)
									break
					aWordList.append(aLine[:idx0])
					aLine = aLine[idx0:]
					idxImage = aLine.find("<img=")
					if idxImage == -1:
						aWordList += aLine.split(" ")
						break
			# Assemble lines from the word list.
			szLine = ""
			while True:
				if szLine:
					# Store the finished line.
					aList.append(COLOR + szLine)
					COLOR = evalColorCodes(szLine, COLOR)

				if not aWordList:
					break # Wrapping complete

				# A line must start with the first word.
				szLine = aWordList.pop(0)
				# Is it already too long?
				if CyIF.determineWidth(uFont + szLine) >= iMaxX:
					continue # Oh well... Wrap a new line

				# A line should have more than just one word.
				while aWordList:

					if CyIF.determineWidth(uFont + szLine + " " + aWordList[0]) >= iMaxX:
						break # Wrap a new line

					# Add another word to the line.
					szLine += " " + aWordList.pop(0)

		else: # No wrapping was necessary.
			aList.append(COLOR + aLine)
			COLOR = evalColorCodes(aLine, COLOR)
	# Find text height in pixels
	dy = len(aList) * 22 + 16
	# Find best position.
	if xPos > xRes / 2:
		x = xPos - 32 - dx
	else: # Left side of screen.
		x = xPos + 24
	'''
	if dy > yRes - 16:
		pass # WIP may want to split tooltip into two separate lists.
	'''
	if yPos > yRes / 2:
		if yPos - dy - 12 > 4:
			yOffset = -dy - 12
		else:
			yOffset = 4 - yPos
	else: # Bottom half of the screen.
		if yPos + dy + 24 < yRes - 4:
			yOffset = 24
		elif yPos - yRes - yPos -dy - 4 > 4:
			yOffset = yRes - yPos -dy - 4
		else:
			yOffset = 4 - yPos
	y = yPos + yOffset
	# draw help hover
	screen.addTableControlGFC(listBox, 1, x, y, dx + 24, dy, False, False, 0, 0, TableStyles.TABLE_STYLE_STANDARD)
	if listBox == "PlotHelp":
		screen.setStyle(listBox, "Table_Help_Alt_Style")
	else:
		screen.setStyle(listBox, "Table_Help_Style")
	for i, aLine in enumerate(aList):
		screen.appendTableRow(listBox)
		screen.setTableText(listBox, 0, i, uFont + aLine, "", WidgetTypes.WIDGET_GENERAL, 0, 0, 1<<0)
	return x, y


# Evaluates if there's a color that should wrap to next line.
def evalColorCodes(szText, COLOR):
	i = szText.find("<color=")
	if i == -1: # No color code found
		if not COLOR or szText.find("/color>") != -1:
			return ""
		return COLOR

	if i: szText = szText[i:]

	# Only interested in the last color
	while True:
		i = szText[1:].find("<color=")
		if i == -1:
			# Is color resolved?
			if szText.find("/color>") != -1:
				return "" # Resolved
			break # Not Resolved
		szText = szText[i+1:]

	idxEnd = szText.find(">") + 1
	return szText[:idxEnd]