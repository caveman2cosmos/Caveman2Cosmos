
#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvBuildLists.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvMessageControl.h"
#include "CvMessageData.h"
#include "CvPlayerAI.h"

CvMessageControl& CvMessageControl::getInstance()
{
	static CvMessageControl m_sInstance;
	return m_sInstance;
}

void CvMessageControl::sendExtendedGame()
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetExtendedGame(GC.getGame().getActivePlayer()));
	}
}

void CvMessageControl::sendAutoMoves()
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetAutoMoves(GC.getGame().getActivePlayer()));
	}
}

void CvMessageControl::sendTurnComplete()
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetTurnComplete(GC.getGame().getActivePlayer()));
	}
}

void CvMessageControl::sendPushOrder(int iCityID, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetPushOrder(GC.getGame().getActivePlayer(), iCityID, eOrder, iData, bAlt, bShift, bCtrl));
	}
}

void CvMessageControl::sendPopOrder(int iCity, int iNum)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetPopOrder(GC.getGame().getActivePlayer(), iCity, iNum));
	}
}

void CvMessageControl::sendDoTask(int iCity, TaskTypes eTask, int iData1, int iData2, bool bOption, bool bAlt, bool bShift, bool bCtrl)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetDoTask(GC.getGame().getActivePlayer(), iCity, eTask, iData1, iData2, bOption, bAlt, bShift, bCtrl));
	}
}

void CvMessageControl::sendUpdateCivics(const std::vector<CivicTypes>& aeCivics)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetUpdateCivics(GC.getGame().getActivePlayer(), aeCivics));
	}
}

void CvMessageControl::sendResearch(TechTypes eTech, int iDiscover, bool bShift)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetResearch(GC.getGame().getActivePlayer(), eTech, iDiscover, bShift));
	}
}

void CvMessageControl::sendEspionageSpendingWeightChange(TeamTypes eTargetTeam, int iChange)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetEspionageChange(GC.getGame().getActivePlayer(), eTargetTeam, iChange));
	}
}

void CvMessageControl::sendAdvancedStartAction(AdvancedStartActionTypes eAction, PlayerTypes ePlayer, int iX, int iY, int iData, bool bAdd)
{
	gDLL->sendMessageData(new CvNetAdvancedStartAction(eAction, ePlayer, iX, iY, iData, bAdd));
}

void CvMessageControl::sendModNetMessage(int iData1, int iData2, int iData3, int iData4, int iData5)
{
	gDLL->sendMessageData(new CvNetModNetMessage(iData1, iData2, iData3, iData4, iData5));
}

void CvMessageControl::sendConvert(ReligionTypes eReligion)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetConvert(GC.getGame().getActivePlayer(), eReligion));
	}
}

void CvMessageControl::sendEmpireSplit(PlayerTypes ePlayer, int iAreaId)
{
	gDLL->sendMessageData(new CvNetEmpireSplit(ePlayer, iAreaId));
}

void CvMessageControl::sendFoundReligion(PlayerTypes ePlayer, ReligionTypes eReligion, ReligionTypes eSlotReligion)
{
	gDLL->sendMessageData(new CvNetFoundReligion(ePlayer, eReligion, eSlotReligion));
}

void CvMessageControl::sendLaunch(PlayerTypes ePlayer, VictoryTypes eVictory)
{
	gDLL->sendMessageData(new CvNetLaunchSpaceship(ePlayer, eVictory));
}

void CvMessageControl::sendEventTriggered(PlayerTypes ePlayer, EventTypes eEvent, int iEventTriggeredId)
{
	gDLL->sendMessageData(new CvNetEventTriggered(ePlayer, eEvent, iEventTriggeredId));
}

void CvMessageControl::sendJoinGroup(int iUnitID, int iHeadID)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetJoinGroup(GC.getGame().getActivePlayer(), iUnitID, iHeadID));
	}
}

void CvMessageControl::sendPushMission(int iUnitID, MissionTypes eMission, int iData1, int iData2, int iFlags, bool bShift)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetPushMission(GC.getGame().getActivePlayer(), iUnitID, eMission, iData1, iData2, iFlags, bShift));
	}
}

void CvMessageControl::sendAutoMission(int iUnitID)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetAutoMission(GC.getGame().getActivePlayer(), iUnitID));
	}
}

void CvMessageControl::sendDoCommand(int iUnitID, CommandTypes eCommand, int iData1, int iData2, bool bAlt)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetDoCommand(GC.getGame().getActivePlayer(), iUnitID, eCommand, iData1, iData2, bAlt));
	}
}

void CvMessageControl::sendPercentChange(CommerceTypes eCommerce, int iChange)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetPercentChange(GC.getGame().getActivePlayer(), eCommerce, iChange));
	}
}

void CvMessageControl::sendChangeVassal(TeamTypes eMasterTeam, bool bVassal, bool bCapitulated)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetChangeVassal(GC.getGame().getActivePlayer(), eMasterTeam, bVassal, bCapitulated));
	}
}

void CvMessageControl::sendChooseElection(int iSelection, int iVoteId)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetChooseElection(GC.getGame().getActivePlayer(), iSelection, iVoteId));
	}
}

void CvMessageControl::sendDiploVote(int iVoteId, PlayerVoteTypes eChoice)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetDiploVote(GC.getGame().getActivePlayer(), iVoteId, eChoice));
	}
}

void CvMessageControl::sendChangeWar(TeamTypes eRivalTeam, bool bWar)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetChangeWar(GC.getGame().getActivePlayer(), eRivalTeam, bWar));
	}
}

void CvMessageControl::sendPing(int iX, int iY)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetPing(GC.getGame().getActivePlayer(), iX, iY));
	}
}

// BUG - Reminder Mod - start
void CvMessageControl::sendAddReminder(PlayerTypes ePlayer, int iGameTurn, CvWString szMessage)
{
	gDLL->sendMessageData(new CvNetAddReminder(ePlayer, iGameTurn, szMessage));
}
// BUG - Reminder Mod - end

/************************************************************************************************/
/* Afforess	                  Start		 08/18/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
void CvMessageControl::sendGlobalDefineUpdate(CvWString szName, int iValue, float fValue, CvWString szValue)
{
	gDLL->sendMessageData(new CvGlobalDefineUpdate(szName, iValue, fValue, szValue));
}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

void CvMessageControl::sendRecalculateModifiers()
{
	gDLL->sendMessageData(new CvNetRecalculateModifiers());
}

void CvMessageControl::sendBuildLists()
{
	PROFILE_EXTRA_FUNC();
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		// Split the build list data into a sufficient number of messages
		CvPlayerAI& kPlayer = GET_PLAYER(GC.getGame().getActivePlayer());
		int iNumLists = kPlayer.m_pBuildLists->getNumLists();
		for (int i=0; i<iNumLists; i++)
		{
			int iListSize = kPlayer.m_pBuildLists->getListLength(i);
			for (int j=0; j<iListSize; j+=100)
			{
				gDLL->sendMessageData(new CvNetBuildLists(GC.getGame().getActivePlayer(), i, j, 100));
			}
		}
	}
}

void CvMessageControl::sendBuildListPushOrder(int iID, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetBuildListPushOrder(GC.getGame().getActivePlayer(), iID, eOrder, iData, bAlt, bShift, bCtrl));
	}
}

void CvMessageControl::sendBuildListPopOrder(int iID, int iNum)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetBuildListPopOrder(GC.getGame().getActivePlayer(), iID, iNum));
	}
}

void CvMessageControl::sendBuildListEdit(int iID, CvString szName, bool bDelete)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetBuildListEdit(GC.getGame().getActivePlayer(), iID, szName, bDelete));
	}
}

void CvMessageControl::sendChooseBuildUp(PromotionLineTypes ePromotionLine, int iID)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetChooseBuildUp(GC.getGame().getActivePlayer(), ePromotionLine, iID));
	}
}


void CvMessageControl::sendChooseTrait(TraitTypes eTrait, bool bNewValue)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetChooseTrait(GC.getGame().getActivePlayer(), eTrait, bNewValue));
	}
}

void CvMessageControl::sendMergeUnit(int iUnitID)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetChooseMergeUnit(GC.getGame().getActivePlayer(), iUnitID));
	}
}

void CvMessageControl::sendSplitUnit(bool bConfirm)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetConfirmSplitUnit(GC.getGame().getActivePlayer(), bConfirm));
	}
}

void CvMessageControl::sendImprovementUpgrade(ImprovementTypes eImprovement, int iX, int iY, bool bConfirm)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetImprovementUpgrade(GC.getGame().getActivePlayer(), eImprovement, iX, iY, bConfirm));
	}
}

void CvMessageControl::sendArrestUnit(int iUnitID, PlayerTypes ePlayerT)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetChooseArrestUnit(GC.getGame().getActivePlayer(), ePlayerT, iUnitID));
	}
}

void CvMessageControl::sendAmbushConfirmation(bool bConfirm)
{
	if (NO_PLAYER != GC.getGame().getActivePlayer())
	{
		gDLL->sendMessageData(new CvNetConfirmAmbush(GC.getGame().getActivePlayer(), bConfirm));
	}
}