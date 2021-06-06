## Ruff autologger
## Modified from HOF MOD V1.61.001
## Modified from autolog by eotinb
## contains variables to turn on and off various extra log messages
## Alt+E is always on

from CvPythonExtensions import *
import BugConfigTracker
import BugCore
import BugOptions
import SystemPaths as SP
import codecs
import os
import time

AutologOpt = BugCore.game.Autolog

class autologInstance:

	def __init__(self):
		self.MsgStore = []
		self.bStarted = False
		self.LogFileName = None
		self.LogFilePath = None
		self.RealLogFile = None
		self.setLogFileName(AutologOpt.getFileName())
		self.setLogFilePath(AutologOpt.getFilePath())

	def setLogFileName(self, LogFileName, bSaveToOptions=False):
		if bSaveToOptions:
			AutologOpt.setFileName(LogFileName)
			BugOptions.write()
		self.LogFileName = LogFileName
		self.updateLogFile()

	def getLogFileName(self):
		return self.LogFileName

	def setLogFilePath(self, LogFilePath, bSaveToOptions=False):
		if bSaveToOptions:
			AutologOpt.setFilePath(LogFilePath)
			BugOptions.write()
		if not LogFilePath or LogFilePath == "Default":
			LogFilePath = SP.joinModDir("Autolog")
		if not os.path.isdir(LogFilePath):
			os.makedirs(LogFilePath)
		self.LogFilePath = LogFilePath
		self.updateLogFile()

	def getLogFilePath(self):
		return self.LogFilePath

	def updateLogFile(self):
		if self.LogFileName and self.LogFilePath:
			self.bStarted = False
			self.RealLogFile = os.path.join(self.LogFilePath, self.LogFileName)
			BugConfigTracker.add("Autolog_Log", self.RealLogFile)

	def isLogging(self):
		return AutologOpt.isLoggingOn()

	def start(self):
		GAME = CyGame()
		TRNSLTR = CyTranslator()
		self.writeMsg("")
		self.writeMsg("Logging by autolog.py")
		self.writeMsg("------------------------------------------------")
		iMaxTurns = GAME.getMaxTurns()
		year = GAME.getGameTurnYear()
		if year < 0:
			year = TRNSLTR.getText("TXT_KEY_TIME_BC", (-year,))
		else:
			year = TRNSLTR.getText("TXT_KEY_TIME_AD", (year,))

		if iMaxTurns:
			zTurn = "%i/%i" % (GAME.getElapsedGameTurns() + AutologOpt.getStartDateTurn(), iMaxTurns)
		else:
			zTurn = "%i" %(GAME.getElapsedGameTurns() + AutologOpt.getStartDateTurn())

		self.writeMsg(TRNSLTR.getText("TXT_KEY_AUTOLOG_TURN", (zTurn, year, time.strftime("%d-%b-%Y %H:%M:%S"))), vBold=True, vUnderline=True)
		self.bStarted = True

	def writeLog(self, vMsg, vColor = "Black", vBold = False, vUnderline = False, vPrefix = ""):
		self.openLog()
		if len(self.MsgStore) > 0:
			msgs = self.MsgStore
			self.writeLog_pending_flush()
			for sMsg in msgs:
				self.log.write(sMsg)
		self.writeMsg(vMsg, vColor, vBold, vUnderline, vPrefix)
		self.closeLog()

	def writeMsg(self, vMsg, vColor = "Black", vBold = False, vUnderline = False, vPrefix = ""):
		zMsg = self.buildMsg(vMsg, vColor, vBold, vUnderline, vPrefix)
		self.log.write(zMsg)

	def writeLog_pending(self, vMsg, vColor = "Black", vBold = False, vUnderline = False, vPrefix = ""):
		zMsg = self.buildMsg(vMsg, vColor, vBold, vUnderline, vPrefix)
		self.MsgStore.append(zMsg)

	def writeLog_pending_flush(self):
		self.MsgStore = []

	def openLog(self):
		self.log = codecs.open(self.RealLogFile, 'a', 'utf-8')
		if not self.bStarted:
			self.start()

	def closeLog(self):
		self.log.close()

	def buildMsg(self, msg, vColor, vBold, vUnderline, vPrefix):
		if vPrefix:
			msg = vPrefix + " " + msg

		## determine type of message
		zStyle = AutologOpt.getFormatStyle()
		if zStyle < 0 or zStyle > 3: zStyle=0

		if not zStyle: pass
		elif zStyle == 1: # html formatting
			if vBold:
				msg = "<b>%s</b>" % msg
			if vUnderline:
				msg = "<u>%s</u>" % msg
			if vColor != "Black" and AutologOpt.isColorCoding():
				msg = "<span style=\"color: %s\">%s</span>" % (vColor, msg)

			msg = "%s<br>" % (msg)

		else: # forum formatting
			if vBold:
				msg = "[b]%s[/b]" % msg
			if vUnderline:
				msg = "[u]%s[/u]" % msg
			if vColor != "Black" and AutologOpt.isColorCoding():
				if zStyle == 2:  # color coding with "
					msg = "[color=\"%s\"]%s[/color]" % (vColor, msg)
				else:  # color coding without "
					msg = "[color=%s]%s[/color]" % (vColor, msg)

		return "%s\r\n" % msg
