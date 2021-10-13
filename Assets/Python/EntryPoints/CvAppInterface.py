# Sid Meier's Civilization 4
# Copyright Firaxis Games 2005
#
# CvAppInterface.py
#
# These functions are App Entry Points from C++
# WARNING: These function names should not be changed
# WARNING: These functions can not be placed into a class
#
# No other modules should import this
#
# DONT ADD ANY MORE IMPORTS HERE - Moose
from CvPythonExtensions import *

def AddSign(argsList):
	import EventSigns
	EventSigns.addSign(argsList[0], argsList[1], argsList[2])

def RemoveSign(argsList):
	import EventSigns
	CyEngine().removeSign(argsList[0], argsList[1])
	EventSigns.gSavedSigns.removeSign(argsList[0], argsList[1])

def initBUG():
	import BugInit
	BugInit.init()

	# Matt: todo - temp, delete later
	GC = CyGlobalContext()
	for i in range(GC.getNumBuildingInfos()):
		CvBuildingInfo = GC.getBuildingInfo(i)
		print CvBuildingInfo.getType()
		#for iTech, iHappiness in CvBuildingInfo.getTechHappinessChanges():
		#	print str(iTech) + ", " + str(iHappiness)
		for iTech in CvBuildingInfo.getPrereqAndTechs():
			print str(iTech)

# don't make this an event - Moose
def init():
	import sys
	import CvUtil
	sys.stderr = CvUtil.RedirectError()
	sys.excepthook = CvUtil.myExceptHook
	sys.stdout = CvUtil.RedirectDebug()

def onSave():
	import cPickle
	import CvEventInterface
	# if the tutorial is active, it will save out the Shown Messages list
	return cPickle.dumps(CvEventInterface.onEvent(('OnSave', 0, 0, 0, 0, 0)))

def onLoad(argsList):
	import CvScreensInterface
	CvScreensInterface.mainInterface.bSetStartZoom = True
	import CvEventInterface
	loadDataStr = argsList[0]
	if loadDataStr:
		import cPickle
		CvEventInterface.onEvent(('OnLoad', cPickle.loads(loadDataStr), 0, 0, 0, 0, 0))

def preGameStart():
	#import CvEventInterface
	#CvEventInterface.getEventManager().fireEvent("PreGameStart")
	import CvScreensInterface
	CvScreensInterface.mainInterface.bSetStartZoom = True
	CvScreensInterface.showMainInterface()

def recalculateModifiers():
	import CvRandomEventInterface
	CvRandomEventInterface.recalculateModifiers()

def onPbemSend(argsList):
	import smtplib, MimeWriter, base64, StringIO

	szToAddr = argsList[0]
	szFromAddr = argsList[1]
	szSubject = argsList[2]
	szPath = argsList[3]
	szFilename = argsList[4]
	szHost = argsList[5]
	szUser = argsList[6]
	szPassword = argsList[7]

	print 'sending e-mail'
	print 'To:', szToAddr
	print 'From:', szFromAddr
	print 'Subject:', szSubject
	print 'Path:', szPath
	print 'File:', szFilename
	print 'Server:', szHost
	print 'User:', szUser

	if not szFromAddr or not szHost:
		print 'host or address empty'
		return 1

	message = StringIO.StringIO()
	writer = MimeWriter.MimeWriter(message)

	writer.addheader('To', szToAddr)
	writer.addheader('From', szFromAddr)
	writer.addheader('Subject', szSubject)
	writer.addheader('MIME-Version', '1.0')
	writer.startmultipartbody('mixed')

	part = writer.nextpart()
	body = part.startbody('text/plain')
	body.write('CIV4 PBEM save attached')

	part = writer.nextpart()
	part.addheader('Content-Transfer-Encoding', 'base64')
	szStartBody = "application/CivBeyondSwordSave; name=%s" % szFilename
	body = part.startbody(szStartBody)
	base64.encode(open(szPath+szFilename, 'rb'), body)

	# finish off
	writer.lastpart()

	# send the mail
	try:
		smtp = smtplib.SMTP(szHost)
		if len(szUser) > 0:
			smtp.login(szUser, szPassword)
		smtp.sendmail(szFromAddr, szToAddr, message.getvalue())
		smtp.quit()
	except smtplib.SMTPAuthenticationError, e:
		CyInterface().addImmediateMessage("Authentication Error: The server didn't accept the username/password combination provided.", "")
		CyInterface().addImmediateMessage("Error %d: %s" % (e.smtp_code, e.smtp_error), "")
		return 1
	except smtplib.SMTPHeloError, e:
		CyInterface().addImmediateMessage("The server refused our HELO reply.", "")
		CyInterface().addImmediateMessage("Error %d: %s" % (e.smtp_code, e.smtp_error), "")
		return 1
	except smtplib.SMTPConnectError, e:
		CyInterface().addImmediateMessage("Error establishing connection.", "")
		CyInterface().addImmediateMessage("Error %d: %s" % (e.smtp_code, e.smtp_error), "")
		return 1
	except smtplib.SMTPDataError, e:
		CyInterface().addImmediateMessage("The SMTP server didn't accept the data.", "")
		CyInterface().addImmediateMessage("Error %d: %s" % (e.smtp_code, e.smtp_error), "")
		return 1
	except smtplib.SMTPRecipientsRefused, e:
		CyInterface().addImmediateMessage("All recipient addresses refused.", "")
		return 1
	except smtplib.SMTPSenderRefused, e:
		CyInterface().addImmediateMessage("Sender address refused.", "")
		CyInterface().addImmediateMessage("Error %d: %s" % (e.smtp_code, e.smtp_error), "")
		return 1
	except smtplib.SMTPResponseException, e:
		CyInterface().addImmediateMessage("Error %d: %s" % (e.smtp_code, e.smtp_error), "")
		return 1
	except smtplib.SMTPServerDisconnected:
		CyInterface().addImmediateMessage("Not connected to any SMTP server", "")
		return 1
	except:
		return 1
	return 0


g_options = None
def getOption(id):
	global g_options
	if g_options is None:
		import BugOptions
		g_options = BugOptions.g_options
	return g_options.getOption(id)

def getOptionBOOL(argsList):
	id, default = argsList
	try:
		option = getOption(id)
		return bool(option.getValue())
	except:
		return default

def getOptionINT(argsList):
	id, default = argsList
	try:
		option = getOption(id)
		return int(option.getValue())
	except:
		return default

def gameExitSave():
	import AutoSave
	AutoSave.autoSave("[Exit]")

# Referenced by the BtS exe.
def getConsoleMacro(argsList): return ""
