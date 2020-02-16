from CvPythonExtensions import *

import ScreenResolution as RES

GC = CyGlobalContext()
CyIF = CyInterface()

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
	xRes = RES.x
	yRes = RES.y

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
	# Wrap long lines
	aList = []
	COLOUR = ""
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
					aList.append(COLOUR + szLine)
					COLOUR = evalColorCodes(szLine)
				if aWordList:
					# A line must start with the first word.
					szLine = aWordList.pop(0)
					# Is it already too long?
					iWidth = CyIF.determineWidth(uFont + szLine)
					if iWidth >= dx -24:
						continue # Oh well... Wrap a new line
					# A line should have more than just one word.
					while True:
						if aWordList:
							iWidth = CyIF.determineWidth(uFont + szLine + " " + aWordList[0])
							if iWidth >= dx -24:
								# Wrap a new line
								break
							else: # Add another word to the line.
								szLine += " " + aWordList.pop(0)
						else: # Wrapping complete
							break
				else: # Wrapping complete
					break
		else: # No wrapping was necessary.
			aList.append(COLOUR + aLine)
			COLOUR = evalColorCodes(aLine)
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


def evalColorCodes(szText):
	i = szText.find("<color=")
	if i != -1:
		if i:
			szText = szText[i:]
		while True:
			i = szText[1:].find("<color=")
			if i != -1:
				szText = szText[i+1:]
			else: # Is color resolved?
				if szText.find("/color>") != -1:
					return ""	# Resolved
				else:	# Not Resolved
					break
	else: # No color code found
		return ""
	idxEnd = szText.find(">") + 1
	return szText[:idxEnd]