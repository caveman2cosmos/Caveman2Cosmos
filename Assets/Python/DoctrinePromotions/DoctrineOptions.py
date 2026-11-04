# Python 2.4. The BUG checkbox mirrors one saved, synchronized game option.
from CvPythonExtensions import CyGame, GameOptionTypes
import BugCore

_syncing = False
_option = GameOptionTypes.GAMEOPTION_COMBAT_MILITARY_DOCTRINES


def canChange():
    game = CyGame()
    # Multiplayer rules are chosen in game setup, never by a local checkbox.
    return game.isFinalInitialized() and not game.isNetworkMultiPlayer()


def sync():
    global _syncing
    game = CyGame()
    if not game.isFinalInitialized():
        return
    _syncing = True
    try:
        BugCore.game.MilitaryDoctrines.setEnabled(game.isOption(_option))
    finally:
        _syncing = False


def onLoad(argsList):
    sync()


def onChanged(option, value):
    if _syncing:
        return
    if not canChange():
        sync()
        return
    game = CyGame()
    wanted = bool(value)
    if game.isOption(_option) != wanted:
        game.setOption(_option, wanted)
    sync()
