## AutoSave
#from CvPythonExtensions import CyGlobalContext
#from CvPythonExtensions import CyInterface
from CvPythonExtensions import *
import TextUtil

GC = CyGlobalContext()
CyIF = CyInterface()
GAME = GC.getGame()
MAP = GC.getMap()
TRNSLTR = CyTranslator()
import TextUtil



def remove_diacritics(in_text):
    # Table des remplacements (caractères + codes explicites)
    #print "Start remove_diacritics for %s" % in_text
    #print "%s" % str(type(in_text))
    text = u''
    try :
		text = in_text.encode("utf-8")
    except :
		text = in_text.encode("ascii", "ignore")
    #print "%s" % text
    text = text.replace('à', 'a').replace('â', 'a').replace('ä', 'a').replace('á', 'a').replace('ã', 'a').replace('å', 'a')
    #print "%s" % text
    text = text.replace('ç', 'c')
    text = text.replace('é', 'e').replace('è', 'e').replace('ê', 'e').replace('ë', 'e')
    text = text.replace('î', 'i').replace('ï', 'i').replace('í', 'i')
    text = text.replace('ô', 'o').replace('ö', 'o').replace('ò', 'o').replace('ó', 'o').replace('õ', 'o')
    text = text.replace('ù', 'u').replace('û', 'u').replace('ü', 'u').replace('ú', 'u')
    text = text.replace('ÿ', 'y')
    text = text.replace('ñ', 'n')

    text = text.replace('À', 'A').replace('Â', 'A').replace('Ä', 'A').replace('Á', 'A').replace('Ã', 'A').replace('Å', 'A')
    text = text.replace('Ç', 'C')
    text = text.replace('É', 'E').replace('È', 'E').replace('Ê', 'E').replace('Ë', 'E')
    text = text.replace('Î', 'I').replace('Ï', 'I').replace('Í', 'I')
    text = text.replace('Ô', 'O').replace('Ö', 'O').replace('Ò', 'O').replace('Ó', 'O').replace('Õ', 'O')
    text = text.replace('Ù', 'U').replace('Û', 'U').replace('Ü', 'U').replace('Ú', 'U')
    text = text.replace('Ÿ', 'Y')
    text = text.replace('Ñ', 'N')

    #print "%s" % text
    return text
        
def cleanNpc():
	iTurn = GAME.getGameTurn()
	modturn = iTurn % 30
	print "ModTurn for cleaning : %d" % modturn
	if npcclean and modturn == 0:
		print "Cleaning NPC - for player 40,41 and 42..."

		iPlayer = 40
		pPlayer = GC.getPlayer(iPlayer)
		pUnit, loop = pPlayer.firstUnit(False)
		while pUnit:
			pUnit
			print "Player %d, Unit ID: %d, %s\n" % (iPlayer, pUnit.getID(), TextUtil.convertToStr(pUnit.getName()))
			pUnit.kill(False, -1)
			pUnit, loop = pPlayer.nextUnit(loop, False)
		print "Unités du joueur 40 supprimées."

		iPlayer = 41
		pPlayer = GC.getPlayer(iPlayer)
		pUnit, loop = pPlayer.firstUnit(False)
		while pUnit:
			pUnit
			print "Player %d, Unit ID: %d, %s\n" % (iPlayer, pUnit.getID(), TextUtil.convertToStr(pUnit.getName()))
			pUnit.kill(False, -1)
			pUnit, loop = pPlayer.nextUnit(loop, False)
		print "Unités du joueur 41 supprimées."
		
		iPlayer = 42
		pPlayer = GC.getPlayer(iPlayer)
		pUnit, loop = pPlayer.firstUnit(False)
		while pUnit:
			pUnit
			print "Player %d, Unit ID: %d, %s\n" % (iPlayer, pUnit.getID(), TextUtil.convertToStr(pUnit.getName()))
			pUnit.kill(False, -1)
			pUnit, loop = pPlayer.nextUnit(loop, False)
		print "Unités du joueur 42 supprimées."
		szMessage = TRNSLTR.getText("TXT_KEY_MOD_KILL_ANIMALS_DESC", ())
		for iPlayer in range(GC.getMAX_PLAYERS()):
			if GC.getPlayer(iPlayer).isHuman():
				CyIF.addMessage(iPlayer, False, GC.getEVENT_MESSAGE_TIME(), szMessage, None, InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), -1, -1, False, False)


def init():
	import SystemPaths as SP
	global _saveDir, options
	_saveDir = SP.userDir + "\\Saves"
	import BugCore
	options = BugCore.game.AutoSave
	import CvEventInterface
	CvEventInterface.eventManager.addEventHandler("MapRegen", onMapRegen)
	CvEventInterface.eventManager.addEventHandler("endTurnReady", onEndTurnReady)
	CvEventInterface.eventManager.addEventHandler("GameEnd", onGameEnd)

def onMapRegen(argsList):
	autoSave("[Start]", 0)

def onEndTurnReady(argsList):
	iTurn, = argsList
	inter = options.getInterval()
	if inter > 0 and not iTurn % inter:
		autoSave("[Late]", iTurn)

def onGameEnd(argsList):
	autoSave("[End]", argsList[0])

def save(type, prefix, iTurn):

	dir = "%s\\%s\\" %(_saveDir, type)
	if prefix:
		dir += prefix + "'"
	CyPlayer = GC.getActivePlayer()

	if iTurn:
		dir += "#" + str(iTurn)
	else:
		dir += MAP.getMapScriptName()

	if options.isUsePlayerName():
		dir += '-' + CyPlayer.getName()[:8]
	else:
		dir += '-' + GC.getLeaderHeadInfo(CyPlayer.getLeaderType()).getText()[:8]

	dir += '-' + GC.getEraInfo(GAME.getCurrentEra()).getText()[:8]
	dir += '-' + GC.getGameSpeedInfo(GAME.getGameSpeedType()).getText()[:5]
	dir += '-' + GC.getWorldInfo(MAP.getWorldSize()).getText()[:5]
	dir += '-' + GC.getHandicapInfo(CyPlayer.getHandicapType()).getText()[:5]
	dir += ".CivBeyondSwordSave"

	print "before AutoSave.savename : %s" % dir

	dir = remove_diacritics(dir)

	print "after AutoSave.savename : %s" % dir
	

	
	npcclean = False
	if npcclean:
		cleanNpc()
	
	GAME.saveGame(str(dir))
	print "AutoSave.save\n\t%s" % dir

def autoSave(prefix="", iTurn=None):
	# Filter by option
	if(prefix == "[Start]" and not options.isCreateStartSave()
	or prefix == "[End]" and not options.isCreateEndSave()
	or prefix == "[Exit]" and not options.isCreateExitSave()
	or prefix == "[Late]" and not options.isCreateLateSave()
	):
		return
	if iTurn is None:
		iTurn = GAME.getGameTurn()
	# pick folder
	if GAME.isGameMultiPlayer():
		if GAME.isHotSeat():
			type = "hotseat"
		elif GAME.isPbem():
			type = "pbem"
		elif GAME.isPitboss():
			type = "pitboss"
		else:
			type = "multi"
	else:
		type = "single"
	type += "\\auto"
	save(type, prefix, iTurn)
