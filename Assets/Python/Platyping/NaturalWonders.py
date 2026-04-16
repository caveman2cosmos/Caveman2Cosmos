## Sid Meier's Civilization 4
## Copyright Firaxis Games 2007

from CvPythonExtensions import *

GC = CyGlobalContext()

class NaturalWonders:
    def __init__(self,):
        self.iFirstCulture = 50					## Culture Granted to First Team to Discover
        self.lBigWonder = ["FEATURE_PLATY_GREAT_BARRIER"]	## List of Natural Wonders that occupy 2 Tiles
        self.lLatitude = [("FEATURE_PLATY_AURORA", 70, 90)]	## Min Latitude, Max Latitude
        self.discoveredWonders = {}
        self.pendingCulture = []

    def placeNaturalWonders(self):
        MAP = GC.getMap()
        GAME = GC.getGame()
        for iFeature in xrange(GC.getNumFeatureInfos()):
            sType = GC.getFeatureInfo(iFeature).getType()
            if sType.find("FEATURE_PLATY_") == -1:
                continue
            WonderPlot = []
            for pPlot in MAP.plots():
                if pPlot.getBonusType(-1) > -1:
                    continue
                ## Nearby Plot Check ##
                bUnsuitable = False
                iRadius = 2
                bAdjacentPlot = True

                ## Big Wonders ##
                if sType in self.lBigWonder:
                    iRadius += 1
                    bAdjacentPlot = False

                for x in xrange(pPlot.getX() - iRadius, pPlot.getX() + iRadius + 1):
                    for y in xrange(pPlot.getY() - iRadius, pPlot.getY() + iRadius + 1):
                        pAdjacentPlot = MAP.plot(x, y)
                        if not pAdjacentPlot: continue

                        if pAdjacentPlot.getFeatureType() > -1 and GC.getFeatureInfo(pAdjacentPlot.getFeatureType()).getType().find("FEATURE_PLATY_") > -1:
                            bUnsuitable = True
                            break
                        ## Big Wonders ##
                        if (not bAdjacentPlot and pAdjacentPlot.canHaveFeature(iFeature)
                        and abs(pAdjacentPlot.getX() - pPlot.getX()) < 2
                        and abs(pAdjacentPlot.getY() - pPlot.getY()) < 2
                        and pAdjacentPlot.getBonusType(-1) == -1
                        ): bAdjacentPlot = True

                    if bUnsuitable: break
                if bUnsuitable or not bAdjacentPlot or not pPlot.canHaveFeature(iFeature):
                    continue

                ## Latitude Check ##
                for i in self.lLatitude:
                    if sType == i[0] and (pPlot.getLatitude() < i[1] or pPlot.getLatitude() > i[2]):
                        break
                ## Suitable Plot ##
                else: WonderPlot.append(pPlot)

            while WonderPlot:
                pPlot = WonderPlot.pop(GAME.getSorenRandNum(len(WonderPlot), "Random Plot"))

                ## Big Wonders ##
                if sType in self.lBigWonder:
                    AdjacentPlot = []
                    for x in xrange(pPlot.getX() - 1, pPlot.getX() + 2):
                        for y in xrange(pPlot.getY() - 1, pPlot.getY() + 2):
                            if x == pPlot.getX() and y == pPlot.getY():
                                continue
                            pAdjacentPlot = MAP.plot(x, y)
                            if not pAdjacentPlot:
                                continue
                            if pAdjacentPlot.canHaveFeature(iFeature):
                                if pAdjacentPlot.getBonusType(-1) > -1:
                                    continue
                                AdjacentPlot.append(pAdjacentPlot)
                    if not AdjacentPlot:
                        continue
                    AdjacentPlot[GAME.getSorenRandNum(len(AdjacentPlot), "Random Plot")].setFeatureType(iFeature, 0)

                ## Standard Wonders ##
                pPlot.setFeatureType(iFeature, 0)
                break


    def checkReveal(self, pPlot, iTeam):
        remaining = []
        for iP, c in self.pendingCulture:
            player = GC.getPlayer(iP)
            pCapital = player.getCapitalCity()
            if pCapital and not pCapital.isNone():
                pCapital.changeCulture(iP, c, True)
            else:
                remaining.append((iP, c))
        self.pendingCulture = remaining

        iFeature = pPlot.getFeatureType()
        if iFeature == -1: return

        CyTeam = GC.getTeam(iTeam)
        if CyTeam.isNPC(): return

        GAME = GC.getGame()
        if GAME.GetWorldBuilderMode(): return

        FeatureInfo = GC.getFeatureInfo(iFeature)
        sType = FeatureInfo.getType()
        if sType.find("FEATURE_PLATY_") == -1: return

        pWonderPlot = None
        if sType in self.lBigWonder:
            for x in xrange(pPlot.getX() - 1, pPlot.getX() + 2):
                for y in xrange(pPlot.getY() - 1, pPlot.getY() + 2):
                    if x == pPlot.getX() and y == pPlot.getY(): continue
                    pAdjacentPlot = GC.getMap().plot(x, y)
                    if not pAdjacentPlot:
                        continue
                    if pAdjacentPlot.getFeatureType() == iFeature:
                        pWonderPlot = pAdjacentPlot
                        break
                if pWonderPlot: break
            if pWonderPlot is None: return
            if pWonderPlot.isRevealed(iTeam, False): return

        if (iFeature, iTeam) in self.discoveredWonders:
            return

        bFirst = True
        for iTeamX in xrange(GC.getMAX_PC_TEAMS()):
            if iTeamX == iTeam: continue
            if pPlot.isRevealed(iTeamX, False):
                bFirst = False
                break
            if pWonderPlot and pWonderPlot.isRevealed(iTeamX, False):
                bFirst = False
                break

        self.discoveredWonders[(iFeature, iTeam)] = True

        iCulture = self.iFirstCulture * GC.getGameSpeedInfo(GAME.getGameSpeedType()).getSpeedPercent() / 100

        import CvUtil
        TRNSLTR = CyTranslator()
        iPlayerAct = GAME.getActivePlayer()

        for iPlayerX in xrange(GC.getMAX_PC_PLAYERS()):
            CyPlayerX = GC.getPlayer(iPlayerX)
            iTeamX = CyPlayerX.getTeam()
            if iTeamX != iTeam:
                if bFirst and iPlayerX == iPlayerAct:
                    if CyTeam.isHasMet(iTeamX):
                        CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_MET_FIRST_WONDER",(GC.getTeam(iTeam).getName(), FeatureInfo.getDescription())), iPlayerX, 12, bForce=False)
                    else:
                        CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_NOT_MET_FIRST_WONDER",(FeatureInfo.getDescription(),)), iPlayerX, 12, bForce=False)
                continue
            if iPlayerX == iPlayerAct:
                popupInfo = CyPopupInfo()
                popupInfo.setButtonPopupType(ButtonPopupTypes.BUTTONPOPUP_PYTHON_SCREEN)
                popupInfo.setData1(iFeature)
                popupInfo.setData3(3)
                popupInfo.setText("showWonderMovie")
                popupInfo.addPopup(iPlayerX)
                CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_WONDERDISCOVERED_YOU",(FeatureInfo.getDescription(),)), iPlayerX, 12, FeatureInfo.getButton(), ColorTypes(44), pPlot.getX(), pPlot.getY(), True, True, bForce=False)
            if bFirst:
                pCapital = CyPlayerX.getCapitalCity()
                if pCapital and not pCapital.isNone():
                    pCapital.changeCulture(iPlayerX, iCulture, True)
                else:
                    self.pendingCulture.append((iPlayerX, iCulture))
                if iPlayerX == iPlayerAct:
                    CvUtil.sendMessage(TRNSLTR.getText("TXT_KEY_FIRST_FOUND_WONDER",(iCulture,)), iPlayerX, 12, None, ColorTypes(44), bForce=False)

