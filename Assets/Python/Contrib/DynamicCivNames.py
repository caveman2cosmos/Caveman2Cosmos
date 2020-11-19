# DynamicCivNames
#
# by jdog5000
# Version 1.0
#
# French compatibility from calvitix
#

from CvPythonExtensions import *
import CvUtil
import Popup as PyPopup
# --------- Revolution mod -------------
import RevDefs
import SdToolKit as SDTK
import RevUtils
import LeaderCivNames
import BugCore
import BugUtil
import ANDAutomationsTab

gc = CyGlobalContext()
game = CyGame()
localText = CyTranslator()
RevOpt = BugCore.game.Revolution

g_DynamicCivNames = None

class DynamicCivNames:

  def __init__(self, customEM):
    global g_DynamicCivNames
    g_DynamicCivNames = self

    self.customEM = customEM

    self.bTeamNaming = True#RevOpt.isTeamNaming()
    self.bLeaveHumanName = False#RevOpt.isLeaveHumanPlayerName()

    self.customEM.addEventHandler("BeginPlayerTurn", self.onBeginPlayerTurn)
    self.customEM.addEventHandler("setPlayerAlive", self.onSetPlayerAlive)
    self.customEM.addEventHandler("kbdEvent", self.onKbdEvent)
    self.customEM.addEventHandler("cityAcquired", self.onCityAcquired)
    self.customEM.addEventHandler('cityBuilt', self.onCityBuilt)
    self.customEM.addEventHandler("vassalState", self.onVassalState)
    self.customEM.addEventHandler("addTeam", self.onAddTeam)
    
    LeaderCivNames.setup()
    
    if not game.isFinalInitialized or game.getGameTurn() == game.getStartTurn():
      for idx in range(0, gc.getMAX_PC_PLAYERS()):
        self.onSetPlayerAlive([idx, gc.getPlayer(idx).isAlive()])

  def removeEventHandlers(self):    
    self.customEM.removeEventHandler("BeginPlayerTurn", self.onBeginPlayerTurn)
    self.customEM.removeEventHandler("setPlayerAlive", self.onSetPlayerAlive)
    self.customEM.removeEventHandler("kbdEvent", self.onKbdEvent)
    self.customEM.removeEventHandler("cityAcquired", self.onCityAcquired)
    self.customEM.removeEventHandler('cityBuilt', self.onCityBuilt)
    self.customEM.removeEventHandler("vassalState", self.onVassalState)
  
  def blankHandler(self, playerID, netUserData, popupReturn):
    """ Dummy handler to take the second event for popup """
    return

  def onKbdEvent(self, argsList):
    'keypress handler'
    eventType, key, mx, my, px, py = argsList

    if eventType == RevDefs.EventKeyDown:
      theKey = int(key)

      # For debug or trial only
      if theKey == int(InputTypes.KB_U) and self.customEM.bShift and self.customEM.bCtrl:
        self.showNewNames()


  def onBeginPlayerTurn(self, argsList):
    iGameTurn, iPlayer = argsList

    # Stuff at end of previous players turn
    iPrevPlayer = iPlayer - 1
    while iPrevPlayer >= 0 and not gc.getPlayer(iPrevPlayer).isAlive():
        iPrevPlayer -= 1

    if iPrevPlayer < 0:
      iPrevPlayer = gc.getBARBARIAN_PLAYER()

    if iPrevPlayer >= 0 and iPrevPlayer < gc.getMAX_PC_PLAYERS():
      iPlayer = iPrevPlayer
      pPlayer = gc.getPlayer(iPlayer)
      
      if (pPlayer.isAnarchy()):
        self.setNewNameByCivics(iPlayer)
        return
        
      if pPlayer.isAlive() and SDTK.sdObjectExists("Revolution", pPlayer):
        prevCivics = SDTK.sdObjectGetVal("Revolution", pPlayer, 'CivicList')
        if not prevCivics == None:
          for i in range(0, gc.getNumCivicOptionInfos()):
            if not prevCivics[i] == pPlayer.getCivics(i):
              self.setNewNameByCivics(iPlayer)
              return
              
        revTurn = SDTK.sdObjectGetVal( "Revolution", pPlayer, 'RevolutionTurn' )
        if not revTurn == None and game.getGameTurn() - revTurn == 30 and pPlayer.getNumCities() > 0:
          # "Graduate" from rebel name
          self.setNewNameByCivics(iPlayer)
          return
          
      if pPlayer.isAlive() and SDTK.sdObjectExists("BarbarianCiv", pPlayer):
        barbTurn = SDTK.sdObjectGetVal("BarbarianCiv", pPlayer, 'SpawnTurn')
        if not barbTurn == None and game.getGameTurn() - barbTurn == 30:
          # "Graduate" from barb civ name
          self.setNewNameByCivics(iPlayer)
          return
      
      if pPlayer.isAlive() and not SDTK.sdObjectExists("BarbarianCiv", pPlayer):
        if 'Tribe' in pPlayer.getCivilizationDescription(0):
          if pPlayer.getCurrentEra() > 0 or pPlayer.getTotalPopulation() >= 3:
            # Graduate from game start name
            self.setNewNameByCivics(iPlayer)
            return

  def onCityAcquired(self, argsList):
    owner, playerType, city, bConquest, bTrade = argsList
    owner = gc.getPlayer(city.getOwner())
    
    if owner.isAlive() and not owner.isNPC() and owner.getNumCities() < 5 and owner.getNumMilitaryUnits() > 0:
        self.setNewNameByCivics(owner.getID())
  
  def onCityBuilt(self, argsList):
    city = argsList[0]
    owner = gc.getPlayer(city.getOwner())

    if owner.isAlive() and not owner.isNPC() and owner.getNumCities() < 5 and owner.getNumMilitaryUnits() > 0:
        self.setNewNameByCivics(owner.getID())
  
  def onVassalState(self, argsList):
    iMaster, iVassal, bVassal = argsList
    if bVassal:
      for iPlayer in range(0, gc.getMAX_PC_PLAYERS()):
        if gc.getPlayer(iPlayer).getTeam() == iVassal:
          self.setNewNameByCivics(iPlayer)
    else:
      for iPlayer in range(0, gc.getMAX_PC_PLAYERS()):
        if gc.getPlayer(iPlayer).getTeam() == iVassal:
          self.setNewNameByCivics(iPlayer)
  
  def setNewNameByCivics(self, iPlayer):
    [newCivDesc, newCivShort, newCivAdj] = self.newNameByCivics(iPlayer)

    if gc.getPlayer(iPlayer).isHuman() or game.getActivePlayer() == iPlayer:
      if self.bLeaveHumanName:
        CvUtil.pyPrint("  Name - Leaving name for human player")
        return

    newDesc  = CvUtil.convertToStr(newCivDesc)
    newShort = CvUtil.convertToStr(newCivShort)
    newAdj   = CvUtil.convertToStr(newCivAdj)

    newDesc  = CvUtil.remove_diacriticals(newDesc)
    newShort = CvUtil.remove_diacriticals(newShort)
    newAdj   = CvUtil.remove_diacriticals(newAdj)
    
    if not newDesc == gc.getPlayer(iPlayer).getCivilizationDescription(0):
      szMessage = BugUtil.getText("TXT_KEY_MOD_DCN_NEWCIV_NAME_DESC", newDesc)
      CyInterface().addMessage(iPlayer, False, gc.getEVENT_MESSAGE_TIME(), szMessage, None, InterfaceMessageTypes.MESSAGE_TYPE_INFO, None, gc.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), -1, -1, False, False)

    gc.getPlayer(iPlayer).setCivName(newDesc, newShort, newAdj)

    return

  def onSetPlayerAlive(self, argsList):
    iPlayerID = argsList[0]
    bNewValue = argsList[1]
    if bNewValue and iPlayerID < gc.getMAX_PC_PLAYERS():
      pPlayer = gc.getPlayer(iPlayerID)

      if( pPlayer.isHuman() or game.getActivePlayer() == iPlayerID ) :
        if( self.bLeaveHumanName ) :
          CvUtil.pyPrint("  Name - Leaving name for human player")
          return
       
      [newCivDesc, newCivShort, newCivAdj] = self.nameForNewPlayer( iPlayerID )
      
      newDesc  = CvUtil.convertToStr(newCivDesc)
      newShort = CvUtil.convertToStr(newCivShort)
      newAdj   = CvUtil.convertToStr(newCivAdj)

      newDesc  = CvUtil.remove_diacriticals(newDesc)
      newShort = CvUtil.remove_diacriticals(newShort)
      newAdj   = CvUtil.remove_diacriticals(newAdj)

      # Pass to pPlayer seems to require a conversion to 'ascii'
      pPlayer.setCivName(newDesc, newShort, newAdj)
      
      
  def onAddTeam(self, argsList):
    eTeam1 = argsList[0]
    eTeam2 = argsList[1]
    bNewValue = argsList[2]
    for i in range(0, gc.getMAX_PC_PLAYERS()):
      pPlayer = gc.getPlayer(i)
      if pPlayer.isAlive():
        if pPlayer.getTeam() == eTeam1 or pPlayer.getTeam() == eTeam2:
          self.setNewNameByCivics(i)


  def showNewNames(self):
    bodStr = 'New names for all civs:\n\n'

    for i in range(0, gc.getMAX_PC_PLAYERS()):
      iPlayer = i
      pPlayer = gc.getPlayer(iPlayer)
      if pPlayer.isAlive():
        curDesc = pPlayer.getCivilizationDescription(0)
        [newName,newShort,newAdj] = self.newNameByCivics(iPlayer)
        bodStr += curDesc + '\t-> ' + newName + '\n'

    popup = PyPopup.PyPopup()
    popup.setBodyString(bodStr)
    popup.launch()


  def nameForNewPlayer(self, iPlayer):
    # Assigns a new name to a recently created player from either
    # BarbarianCiv or Revolution components

    pPlayer = gc.getPlayer(iPlayer)
    currentEra = 0
    for i in range(0,gc.getMAX_PC_PLAYERS()) :
      if( gc.getPlayer(i).getCurrentEra() > currentEra ) :
        currentEra = gc.getPlayer(i).getCurrentEra()

    curDesc = pPlayer.getCivilizationDescription(0)
    curShort = pPlayer.getCivilizationShortDescription(0)
    curAdj = pPlayer.getCivilizationAdjective(0)

    civInfo = gc.getCivilizationInfo(pPlayer.getCivilizationType())
    origDesc  = civInfo.getDescription()
    
    if( not game.isOption(GameOptionTypes.GAMEOPTION_LEAD_ANY_CIV) ) :
      if( pPlayer.getLeaderType() in LeaderCivNames.LeaderCivNames.keys() ) :
        [curDesc,curShort,curAdj] = LeaderCivNames.LeaderCivNames[pPlayer.getLeaderType()]

    newName = curDesc
    if SDTK.sdObjectExists("Revolution", pPlayer):
      revTurn = SDTK.sdObjectGetVal("Revolution", pPlayer, 'RevolutionTurn')
    else:
      revTurn = None

    if SDTK.sdObjectExists("BarbarianCiv", pPlayer):
      barbTurn = SDTK.sdObjectGetVal("BarbarianCiv", pPlayer, 'SpawnTurn')
    else:
      barbTurn = None

    if not pPlayer.isAlive():
      newName = localText.getText("TXT_KEY_MOD_DCN_REFUGEES", ())%(curAdj)
    elif pPlayer.isRebel():
      # To name rebels in Revolution mod
      cityString = SDTK.sdObjectGetVal( "Revolution", pPlayer, 'CapitalName' )
      
      sLiberation = localText.getText("TXT_KEY_MOD_DCN_LIBERATION_FRONT", ()).replace('%s','').strip()
      sGuerillas = localText.getText("TXT_KEY_MOD_DCN_GUERILLAS", ()).replace('%s','').strip()
      sRebels = localText.getText("TXT_KEY_MOD_DCN_REBELS", ()).replace('%s','').strip()
      
      if( sLiberation in curDesc or sGuerillas in curDesc or sRebels in curDesc ) :
        newName = curDesc
      else:
        if( currentEra > 5 and 30 > game.getSorenRandNum(100,'Rev: Naming')) :
          newName = localText.getText("TXT_KEY_MOD_DCN_LIBERATION_FRONT", ())%(curAdj)
        elif( currentEra > 4 and 30 > game.getSorenRandNum(100,'Rev: Naming') ) :
          newName = localText.getText("TXT_KEY_MOD_DCN_GUERILLAS", ())%(curAdj)
        else:
          if( not cityString == None and len(cityString) < 10 ) :
            try:
              if( cityString in curAdj or cityString in curShort ) :
                newName = localText.getText("TXT_KEY_MOD_DCN_THE_REBELS_OF", ())%(CvUtil.convertToStr(cityString))
              else:
                newName = localText.getText("TXT_KEY_MOD_DCN_REBELS_OF", ())%(curAdj,CvUtil.convertToStr(cityString))
            except:
              newName = localText.getText("TXT_KEY_MOD_DCN_REBELS", ())%(curAdj)
          else:
            newName = localText.getText("TXT_KEY_MOD_DCN_REBELS", ())%(curAdj)
    elif( not barbTurn == None and game.getGameTurn() - barbTurn < 20 ) :
      # To name BarbarianCiv created civs
      numCities = SDTK.sdObjectGetVal( "BarbarianCiv", pPlayer, 'NumCities' )
      cityString = SDTK.sdObjectGetVal( "BarbarianCiv", pPlayer, 'CapitalName' )
      
      if( pPlayer.isMinorCiv() ) :
        if( currentEra < 2 ) :
          if( 70 - 40*currentEra > game.getSorenRandNum(100,"Naming") ) : 
            newName = localText.getText("TXT_KEY_MOD_DCN_TRIBE", ())%(curAdj)
          else :
            newName = localText.getText("TXT_KEY_MOD_DCN_CITY_STATE", ())%(curAdj)
        elif( currentEra < 3 ) :
          newName = localText.getText("TXT_KEY_MOD_DCN_CITY_STATE", ())%(curAdj)
        else :
          newName = localText.getText("TXT_KEY_MOD_DCN_NATION", ())%(curAdj)
      elif( currentEra < 4 ) :
        # Early era barbs
        if( SDTK.sdObjectGetVal( 'BarbarianCiv', pPlayer, 'BarbStyle' ) == 'Military' ) :
          if( pPlayer.getNumMilitaryUnits() > 7*numCities ) :
            newName = localText.getText("TXT_KEY_MOD_DCN_HORDE", ())%(curAdj)
          else :
            if( not cityString == None and len(cityString) < 10 ) :
              if( cityString in curAdj or cityString in curShort ) :
                newName = localText.getText("TXT_KEY_MOD_DCN_THE_WARRIORS_OF", ())%(cityString)
              else :
                newName = localText.getText("TXT_KEY_MOD_DCN_WARRIORS_OF", ())%(curAdj,cityString)
            else :
              newName = localText.getText("TXT_KEY_MOD_DCN_WARRIOR_STATE", ())%(curAdj)
        else :
          if( numCities == 1 ) :
            newName = localText.getText("TXT_KEY_MOD_DCN_CITY_STATE", ())%(curAdj)
          else :
            newName = localText.getText("TXT_KEY_MOD_DCN_EMPIRE", ())%(curAdj)
          
          if( numCities < 3 ) :
            if( not cityString == None and len(cityString) < 10) :
              newName += localText.getText("TXT_KEY_MOD_DCN_OF_CITY", ())%(cityString)

      else :
        
        newName = localText.getText("TXT_KEY_MOD_DCN_EMPIRE", ())%(curAdj)
        if( numCities < 3 and not cityString == None and len(cityString) < 10) :
          newName += localText.getText("TXT_KEY_MOD_DCN_OF_CITY", ())%(cityString)

    else :
      if( game.getGameTurn() == game.getStartTurn() and game.getCurrentEra() < 1 ) :
        # Name civs at beginning of game
        newName = localText.getText("TXT_KEY_MOD_DCN_TRIBE", ())%(curAdj)
        return [newName, curShort, curAdj]
      
      return self.newNameByCivics( iPlayer )

    return [newName, curShort, curAdj]


  def newNameByCivics(self, iPlayer, bVerbose = True, bForceUpdate = False):
    # Assigns a new name to a player based on their civics choices

    pPlayer = gc.getPlayer(iPlayer)
    capital = pPlayer.getCapitalCity()
    playerEra = pPlayer.getCurrentEra()
    pTeam = gc.getTeam(pPlayer.getTeam())
    
    cityString = None
    if capital:
      try :
        # Silly game to force ascii encoding now
        cityString =  pPlayer.getCivilizationDescription(0)
        cityString += "&" + CvUtil.convertToStr(capital.getName())
        cityString =  cityString.split('&',1)[-1]
      except :
        pass

    curDesc  = pPlayer.getCivilizationDescription(0)
    curShort = pPlayer.getCivilizationShortDescription(0)
    curAdj   = pPlayer.getCivilizationAdjective(0)

    origDesc  = ""
    if (pPlayer.getCivilizationType() >= 0):
      civInfo = gc.getCivilizationInfo(pPlayer.getCivilizationType())
      origDesc  = civInfo.getDescription()
    
    eLeader = pPlayer.getLeaderType()

    bFemaleLeader = self.isFemaleLeader(eLeader)

    iLanguage = gc.getGame().getCurrentLanguage()
    bFrench = iLanguage == 1 #0 - English, 1 - French, 2 - German, 3 - Italian, 4 - Spanish

    eGovCivic = pPlayer.getCivics(gc.getInfoTypeForString("CIVICOPTION_GOVERNMENT"))
    ePowerCivic = pPlayer.getCivics(gc.getInfoTypeForString("CIVICOPTION_POWER"))
    bNoRealElections = (gc.getInfoTypeForString("CIVIC_MONARCHY") == eGovCivic or gc.getInfoTypeForString("CIVIC_MONARCHY") == eGovCivic or gc.getInfoTypeForString("CIVIC_DESPOTISM") == eGovCivic or gc.getInfoTypeForString("CIVIC_TOTALITARIANISM") == eGovCivic)

    bFederal = (gc.getInfoTypeForString("CIVIC_FEDERALISM") == eGovCivic and (ePowerCivic == gc.getInfoTypeForString("CIVIC_SENATE") or ePowerCivic == gc.getInfoTypeForString("CIVIC_PARLIAMENT") or ePowerCivic == gc.getInfoTypeForString("CIVIC_PRESIDENT")))
    bConfederation = (not bFederal and (gc.getInfoTypeForString("CIVIC_FEDERALISM") == eGovCivic))

    bPacifist = (pPlayer.getCivics(gc.getInfoTypeForString("CIVICOPTION_MILITARY")) == gc.getInfoTypeForString("CIVIC_PACIFISM"))

    if( not game.isOption(GameOptionTypes.GAMEOPTION_LEAD_ANY_CIV) ) :
      if( pPlayer.getLeaderType() in LeaderCivNames.LeaderCivNames.keys() ) :
        [curDesc, curShort, curAdj] = LeaderCivNames.LeaderCivNames[pPlayer.getLeaderType()]

    newName = curDesc
    if( SDTK.sdObjectExists( "Revolution", pPlayer ) ) :
      revTurn = SDTK.sdObjectGetVal( "Revolution", pPlayer, 'RevolutionTurn' )
    else:
      revTurn = None

    if( SDTK.sdObjectExists( "BarbarianCiv", pPlayer ) ) :
      barbTurn = SDTK.sdObjectGetVal( "BarbarianCiv", pPlayer, 'SpawnTurn' )
    else:
      barbTurn = None

    if( not pPlayer.isAlive() ) :
      newName = localText.getText("TXT_KEY_MOD_DCN_REFUGEES", ())%(curAdj)
      return [newName, curShort, curAdj]
    
    if pPlayer.isRebel():
      # Maintain name of rebels from Revolution Mod
      if bForceUpdate:
        return self.nameForNewPlayer(iPlayer)
      else:
        return [curDesc, curShort, curAdj]
    elif( pPlayer.isMinorCiv() and not barbTurn == None ) :
      # Maintain minor civ name
      if bForceUpdate:
        return self.nameForNewPlayer(iPlayer)
      else:
        return [curDesc, curShort, curAdj]
    elif( not barbTurn == None and game.getGameTurn() - barbTurn < 20 and pPlayer.getNumCities() < 4 ) :
      # Maintain name of BarbarianCiv created player
      if bForceUpdate:
        return self.nameForNewPlayer(iPlayer)
      else:
        return [curDesc, curShort, curAdj]

    
    # Special options for teams and permanent alliances
    if( self.bTeamNaming and pTeam.getNumMembers() > 1 ) : # and pTeam.getPermanentAllianceTradingCount() > 0 ) :
      if( pTeam.getNumMembers() == 2 ) :
        iLeader = pTeam.getLeaderID()
        newName = gc.getPlayer(iLeader).getCivilizationAdjective(0) + "-"
        for idx in range(0, gc.getMAX_PC_PLAYERS()):
          if( not idx == iLeader and gc.getPlayer(idx).getTeam() == pTeam.getID() ) :
            newName += gc.getPlayer(idx).getCivilizationAdjective(0)
            break
        newName += u" " + localText.getText("TXT_KEY_MOD_DCN_ALLIANCE", ())
        return [newName,curShort,curAdj]
      else:
        iLeader = pTeam.getLeaderID()
        newName = gc.getPlayer(iLeader).getCivilizationAdjective(0)[0:4]
        for idx in range(0, gc.getMAX_PC_PLAYERS()):
          if not idx == iLeader and gc.getPlayer(idx).getTeam() == pTeam.getID():
            newName += gc.getPlayer(idx).getCivilizationAdjective(0)[0:3]
        newName += u" " + localText.getText("TXT_KEY_MOD_DCN_ALLIANCE", ())
        return [newName,curShort,curAdj]
    
    sSocRep = localText.getText("TXT_KEY_MOD_DCN_SOC_REP", ()).replace('%s','').strip()
    sPeoplesRep = localText.getText("TXT_KEY_MOD_DCN_PEOPLES_REP", ()).replace('%s','').strip()
    
    # Anarchy Naming
    if pPlayer.isAnarchy and pPlayer.getAnarchyTurns() > 1:
      if 75 > game.getSorenRandNum(100,'Rev: Naming'):
        newName = localText.getText("TXT_KEY_MOD_DCN_PROVISIONAL_GOV", ())%(curAdj)
      else:
        newName = localText.getText("TXT_KEY_MOD_DCN_PROVISIONAL_AUTH", ())%(curAdj)
      return [newName, curShort, curAdj]
    
    if (not pPlayer.isAnarchy or pPlayer.getAnarchyTurns() < 2) and "Provisional" in curDesc:
      if gc.getInfoTypeForString("CIVIC_MONARCHY") == eGovCivic:
        newName = curAdj + ' ' + localText.getText("TXT_KEY_MOD_DCN_KINGDOM", ())
      elif (gc.getInfoTypeForString("CIVIC_REPUBLIC") == eGovCivic):
        newName = localText.getText("TXT_KEY_MOD_DCN_REPUBLIC", ())%(curAdj)
      else:
        newName = curAdj + ' Nation'
      return [newName, curShort, curAdj]

    # Main naming conditions
    if RevUtils.isCommunism(iPlayer):
      if RevUtils.isCanDoElections(iPlayer) and not bNoRealElections:
        if not bForceUpdate and (sSocRep in curDesc or sPeoplesRep in curDesc):
          newName = curDesc
        elif 50 > game.getSorenRandNum(100,'Rev: Naming'):
          newName = localText.getText("TXT_KEY_MOD_DCN_SOC_REP", ())%(curShort)
        else :
          newName = localText.getText("TXT_KEY_MOD_DCN_PEOPLES_REP", ())%(curShort)
      elif( RevUtils.getDemocracyLevel(iPlayer)[0] == -8 ) :
        if( localText.getText("TXT_KEY_MOD_DCN_RUSSIAN_MATCH", ()) in curAdj ) :
          curAdj = localText.getText("TXT_KEY_MOD_DCN_SOVIET", ())
        newName = localText.getText("TXT_KEY_MOD_DCN_UNION", ())%(curAdj)
      else :
        newName = localText.getText("TXT_KEY_MOD_DCN_PEOPLES_REP", ())%(curShort)
    elif( RevUtils.isCanDoElections(iPlayer) and not bNoRealElections) :
      sRepOf = localText.getText("TXT_KEY_MOD_DCN_REPUBLIC_OF", ()).replace('%s','').strip()
      sRepublic = localText.getText("TXT_KEY_MOD_DCN_REPUBLIC", ())
      
      if pPlayer.getNumCities() == 1:
        if( not bForceUpdate and (curDesc.startswith(localText.getText("TXT_KEY_MOD_DCN_FREE", ())) or ((sRepOf in curDesc or sRepublic in curDesc) and cityString in curDesc)) ) :
          newName = curDesc
        elif 40 > game.getSorenRandNum(100,'Rev: Naming'):
          newName = localText.getText("TXT_KEY_MOD_DCN_FREE_STATE", ())%(curAdj)
        else:
          if( not cityString == None and len(cityString) < 10 and len(cityString) > 0) :
            if( cityString in curAdj or cityString in curShort ) :
              newName = localText.getText("TXT_KEY_MOD_DCN_THE_REPUBLIC_OF_CITY", ())%(cityString)
            else:
              newName = localText.getText("TXT_KEY_MOD_DCN_REPUBLIC_OF_CITY", ())%(curAdj,cityString)
          else:
            newName = localText.getText("TXT_KEY_MOD_DCN_FREE_REPUBLIC", ())%(curAdj)
      else:
        if( not bFederal and not bConfederation and not bForceUpdate and (sRepublic in curDesc and not sPeoplesRep in curDesc and not sSocRep in curDesc and curDesc.startswith(localText.getText("TXT_KEY_MOD_DCN_FREE", ()))) ) :
          if( len(curDesc) < 17 and 20 > game.getSorenRandNum(100,'Rev: Naming') and not localText.getText("TXT_KEY_MOD_DCN_NEW", ()) in curDesc ) :
            newName = localText.getText("TXT_KEY_MOD_DCN_NEW", ()) + curDesc
          else:
            newName = curDesc
        elif bFederal:
          if (pPlayer.getCivilizationType() == gc.getInfoTypeForString("CIVILIZATION_UNITED_STATES")):
            newName = localText.getText("TXT_KEY_MOD_DCN_UNITED_STATES", ())%(curShort)
          elif 50 > game.getSorenRandNum(100,'Rev: Naming'):
            newName = localText.getText("TXT_KEY_MOD_DCN_FEDERATED_STATES", ())%(curShort)
          else:
            newName = localText.getText("TXT_KEY_MOD_DCN_FEDERATION", ())%(curAdj)
        elif (bConfederation):
          if (50 > game.getSorenRandNum(100,'Rev: Naming')):
            newName = localText.getText("TXT_KEY_MOD_DCN_CONFEDERATION", ())%(curAdj)
          else:
            newName = localText.getText("TXT_KEY_MOD_DCN_CONFEDERATION_STATES", ())%(curShort)
        elif( 50 > game.getSorenRandNum(100,'Rev: Naming') ) :
          newName = localText.getText("TXT_KEY_MOD_DCN_REPUBLIC", ())%(curAdj)
        elif( 33 > game.getSorenRandNum(100,'Rev: Naming') ) :
          newName = localText.getText("TXT_KEY_MOD_DCN_THE_COMMONWEALTH_OF", ())%(curShort)
        else :
          newName = localText.getText("TXT_KEY_MOD_DCN_THE_REPUBLIC_OF", ())%(curShort)

      if( RevUtils.isFreeSpeech(iPlayer) and RevUtils.getLaborFreedom(iPlayer)[0] > 9 ) :
        if( len(newName) < 16 and not localText.getText("TXT_KEY_MOD_DCN_FREE", ()) in newName and not localText.getText("TXT_KEY_MOD_DCN_NEW", ()) in newName ) :
          newName = localText.getText("TXT_KEY_MOD_DCN_FREE", ()) + ' ' + newName
    elif( RevUtils.getDemocracyLevel(iPlayer)[0] == -8 ) :
      empString = localText.getText("TXT_KEY_MOD_DCN_PLAIN_EMPIRE", ())
      if( localText.getText("TXT_KEY_MOD_DCN_GERMAN_MATCH", ()) in curAdj ) :
        empString = localText.getText("TXT_KEY_MOD_DCN_REICH", ())
      
      if( not bForceUpdate and empString in curDesc ) :
        newName = curDesc
      elif( 70 > game.getSorenRandNum(100,'Rev: Naming') and not localText.getText("TXT_KEY_MOD_DCN_REICH", ()) in empString ) :
        newName = localText.getText("TXT_KEY_MOD_DCN_THE_BLANK_OF", ())%(empString,curShort)
      else:
        if (bFrench):
          newName = empString + ' ' + curAdj
        else:
          newName = curAdj + ' ' + empString
    else:
      sGreat = localText.getText("TXT_KEY_MOD_DCN_GREAT_KINGDOM", ()).replace('%s','').strip()
      
      iMinCitiesKingdom = 3
      if (pPlayer.getNumCities() >= iMinCitiesKingdom):
        if (bFemaleLeader):
          sKingdom = localText.getText("TXT_KEY_MOD_DCN_QUEENDOM", ())
        else:
          sKingdom = localText.getText("TXT_KEY_MOD_DCN_KINGDOM", ())
      else:
        sKingdom = localText.getText("TXT_KEY_MOD_DCN_PRINCIPALITY", ())

      if RevUtils.getDemocracyLevel(iPlayer)[0] == -6:

        if pTeam.isAVassal():
          sKingdom = localText.getText("TXT_KEY_MOD_DCN_DUCHY", ())
        else :
          if( localText.getText("TXT_KEY_MOD_DCN_PERSIAN_MATCH", ()) in curAdj or localText.getText("TXT_KEY_MOD_DCN_OTTOMAN_MATCH", ()) in curAdj or localText.getText("TXT_KEY_MOD_DCN_SUMERIAN_MATCH", ()) in curAdj ) :
            sKingdom = localText.getText("TXT_KEY_MOD_DCN_SULTANATE", ())
          elif( localText.getText("TXT_KEY_MOD_DCN_ARABIAN_MATCH", ()) in curAdj ) :
            sKingdom = localText.getText("TXT_KEY_MOD_DCN_CALIPHATE", ())
        
        if pPlayer.getNumCities() < 4:
          if not cityString == None and len(cityString) < 10 and len(cityString) > 0:
            if( cityString in curAdj or cityString in curShort ) :
              newName = localText.getText("TXT_KEY_MOD_DCN_THE_BLANK_OF_CITY", ())%(sKingdom,cityString)
            else :
              if bFrench:
                newName = localText.getText("TXT_KEY_MOD_DCN_BLANK_OF_CITY", ())%(sKingdom,curAdj,cityString)
              else:
                newName = localText.getText("TXT_KEY_MOD_DCN_BLANK_OF_CITY", ())%(curAdj,sKingdom,cityString)
          else:
            if bFrench:
              newName = sKingdom + ' ' + curAdj
            else:
              newName = curAdj + ' ' + sKingdom
        elif( game.getPlayerRank(iPlayer) < game.countCivPlayersAlive()/7 and not pTeam.isAVassal() and (sGreat in curDesc or 40 > game.getSorenRandNum(100,'Rev: Naming')) ) :
          if bFrench:
            newName = localText.getText("TXT_KEY_MOD_DCN_GREAT_KINGDOM", ())%(sKingdom,curAdj)
          else:
            newName = localText.getText("TXT_KEY_MOD_DCN_GREAT_KINGDOM", ())%(curAdj,sKingdom)
        else:
          sOf = localText.getText("TXT_KEY_MOD_DCN_THE_BLANK_OF", ()).replace('%s','')
          if( not bForceUpdate and sKingdom in curDesc and (not sOf in curDesc or pPlayer.getNumCities < 6) and (not sGreat in curDesc) ) :
            newName = curDesc
          elif 50 > game.getSorenRandNum(100,'Rev: Naming'):
            if bFrench:
              newName = sKingdom + ' ' + curAdj
            else:
              newName = curAdj + ' ' + sKingdom
          else:
            newName = localText.getText("TXT_KEY_MOD_DCN_THE_BLANK_OF", ())%(sKingdom,curShort)
      
      elif RevUtils.getDemocracyLevel(iPlayer)[0] == -10 or playerEra == 0:
        
        empString = localText.getText("TXT_KEY_MOD_DCN_PLAIN_EMPIRE", ())
        if playerEra < 2 and pPlayer.getNumCities() < 3:
          empString = localText.getText("TXT_KEY_MOD_DCN_PLAIN_CITY_STATE", ())
        if pTeam.isAVassal():          
          if 50 > game.getSorenRandNum(100,'Rev: Naming'):
            empString = localText.getText("TXT_KEY_MOD_DCN_FIEFDOM", ())
          elif 50 > game.getSorenRandNum(100,'Rev: Naming'):
            empString = localText.getText("TXT_KEY_MOD_DCN_PROTECTORATE", ())
          else:
            empString = localText.getText("TXT_KEY_MOD_DCN_TERRITORY", ())
        
        if not bForceUpdate and empString in curDesc and not game.getGameTurn() == 0:
          newName = curDesc
        elif( 50 > game.getSorenRandNum(100,'Rev: Naming') ) :
          if (bFrench):
            newName = empString + ' ' + curAdj
          else:
            newName = curAdj + ' ' + empString
        else :
          newName = localText.getText("TXT_KEY_MOD_DCN_THE_BLANK_OF", ())%(empString,curShort)
      
      sHoly = localText.getText("TXT_KEY_MOD_DCN_HOLY", ()) + ' '
      if( RevUtils.getReligiousFreedom(iPlayer)[0] < -9 ) :
        if( len(newName) < 16 and not sHoly in newName and not sGreat in newName and not newName.startswith(localText.getText("TXT_KEY_MOD_DCN_HOLY_HRE_MATCH", ())) ) :
          newName = sHoly + newName
      elif newName.startswith(sHoly) and not origDesc.startswith(sHoly):
        # Cut off any inappropriately saved 'Holy ' prefix
        newName = newName[len(sHoly):]
        
    if bPacifist:
      szPacifist = localText.getText("TXT_KEY_MOD_DCN_PACIFIST", ())
      if not szPacifist in newName and 50 > game.getSorenRandNum(100,'Rev: Naming'):
        szPacifist = localText.getText("TXT_KEY_MOD_DCN_PEACEFUL", ())

      if not szPacifist in newName:
        if bFrench:
          newName = newName + ' ' + szPacifist
        else:
          newName = szPacifist + ' ' + newName

    return [newName, curShort, curAdj]
  
  def resetName(self, iPlayer, bVerbose = True):
    pPlayer = gc.getPlayer(iPlayer)
    civInfo = gc.getCivilizationInfo(pPlayer.getCivilizationType())

    origDesc  = civInfo.getDescription()
    origShort = civInfo.getShortDescription(0)
    origAdj   = civInfo.getAdjective(0)

    if not game.isOption(GameOptionTypes.GAMEOPTION_LEAD_ANY_CIV):
      if not self.bLeaveHumanName or not (pPlayer.isHuman() or game.getActivePlayer() == iPlayer):
        if pPlayer.getLeaderType() in LeaderCivNames.LeaderCivNames.keys():
          [origDesc, origShort, origAdj] = LeaderCivNames.LeaderCivNames[pPlayer.getLeaderType()]

    newDesc  = CvUtil.convertToStr(origDesc)
    newShort = CvUtil.convertToStr(origShort)
    newAdj   = CvUtil.convertToStr(origAdj)
      
    newDesc  = CvUtil.remove_diacriticals(newDesc)
    newShort = CvUtil.remove_diacriticals(newShort)
    newAdj   = CvUtil.remove_diacriticals(newAdj)

    gc.getPlayer(iPlayer).setCivName(newDesc, newShort, newAdj)

    
  def isFemaleLeader(self, eLeader):
    bFemaleLeader = False
    
    if eLeader == -1:
      return False
    
    if (not bFemaleLeader):
      bFemaleLeader = (eLeader == gc.getInfoTypeForString("LEADER_BOUDICA"))
    if (not bFemaleLeader):
      bFemaleLeader = (eLeader == gc.getInfoTypeForString("LEADER_ELIZABETH"))
    if (not bFemaleLeader):
      bFemaleLeader = (eLeader == gc.getInfoTypeForString("LEADER_HATSHEPSUT"))
    if (not bFemaleLeader):
      bFemaleLeader = (eLeader == gc.getInfoTypeForString("LEADER_VICTORIA"))
    if (not bFemaleLeader):
      bFemaleLeader = (eLeader == gc.getInfoTypeForString("LEADER_ATOTOZTLI"))
    if (not bFemaleLeader):
      bFemaleLeader = (eLeader == gc.getInfoTypeForString("LEADER_CLEOPATRA"))
    if (not bFemaleLeader):
      bFemaleLeader = (eLeader == gc.getInfoTypeForString("LEADER_DIDO"))
    if (not bFemaleLeader):
      bFemaleLeader = (eLeader == gc.getInfoTypeForString("LEADER_JOANOFARC"))
    if (not bFemaleLeader):
      bFemaleLeader = (eLeader == gc.getInfoTypeForString("LEADER_NEFERTITI"))
    if (not bFemaleLeader):
      bFemaleLeader = (eLeader == gc.getInfoTypeForString("LEADER_THEODORA"))
    if (not bFemaleLeader):
      bFemaleLeader = (eLeader == gc.getInfoTypeForString("LEADER_WU"))
      
    return bFemaleLeader