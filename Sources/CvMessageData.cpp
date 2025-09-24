
#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvBuildLists.h"
#include "CvCity.h"
#include "CvEventReporter.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvMessageData.h"
#include "CvPlayerAI.h"
#include "CvPython.h"
#include "CvSelectionGroup.h"
#include "CvTeamAI.h"
#include "CvUnit.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvInfos.h"

CvMessageData* CvMessageData::createMessage(GameMessageTypes eType)
{
	switch (eType)
	{
	case GAMEMESSAGE_EXTENDED_GAME:
		return new CvNetExtendedGame();
	case GAMEMESSAGE_AUTO_MOVES:
		return new CvNetAutoMoves();
	case GAMEMESSAGE_TURN_COMPLETE:
		return new CvNetTurnComplete();
	case GAMEMESSAGE_PUSH_ORDER:
		return new CvNetPushOrder();
	case GAMEMESSAGE_POP_ORDER:
		return new CvNetPopOrder();
	case GAMEMESSAGE_DO_TASK:
		return new CvNetDoTask();
	case GAMEMESSAGE_UPDATE_CIVICS:
		return new CvNetUpdateCivics();
	case GAMEMESSAGE_RESEARCH:
		return new CvNetResearch();
	case GAMEMESSAGE_ESPIONAGE_CHANGE:
		return new CvNetEspionageChange();
	case GAMEMESSAGE_ADVANCED_START_ACTION:
		return new CvNetAdvancedStartAction();
	case GAMEMESSAGE_MOD_NET_MESSAGE:
		return new CvNetModNetMessage();
	case GAMEMESSAGE_CONVERT:
		return new CvNetConvert();
	case GAMEMESSAGE_EMPIRE_SPLIT:
		return new CvNetEmpireSplit();
	case GAMEMESSAGE_FOUND_RELIGION:
		return new CvNetFoundReligion();
	case GAMEMESSAGE_LAUNCH_SPACESHIP:
		return new CvNetLaunchSpaceship();
	case GAMEMESSAGE_EVENT_TRIGGERED:
		return new CvNetEventTriggered();
	case GAMEMESSAGE_JOIN_GROUP:
		return new CvNetJoinGroup();
	case GAMEMESSAGE_PUSH_MISSION:
		return new CvNetPushMission();
	case GAMEMESSAGE_AUTO_MISSION:
		return new CvNetAutoMission();
	case GAMEMESSAGE_DO_COMMAND:
		return new CvNetDoCommand();
	case GAMEMESSAGE_PERCENT_CHANGE:
		return new CvNetPercentChange();
	case GAMEMESSAGE_CHANGE_VASSAL:
		return new CvNetChangeVassal();
	case GAMEMESSAGE_CHOOSE_ELECTION:
		return new CvNetChooseElection();
	case GAMEMESSAGE_DIPLO_VOTE:
		return new CvNetDiploVote();
	case GAMEMESSAGE_CHANGE_WAR:
		return new CvNetChangeWar();
	case GAMEMESSAGE_PING:
		return new CvNetPing();
	case GAMEMESSAGE_ADD_REMINDER:
		return new CvNetAddReminder();
	case GAMEMESSAGE_SET_GLOBAL_DEFINE:
		return new CvGlobalDefineUpdate();
	case GAMEMESSAGE_RECALCULATE_MODIFIERS:
		return new CvNetRecalculateModifiers();
	case GAMEMESSAGE_BUILD_LISTS:
		return new CvNetBuildLists();
	case GAMEMESSAGE_BUILD_LIST_PUSH_ORDER:
		return new CvNetBuildListPushOrder();
	case GAMEMESSAGE_BUILD_LIST_POP_ORDER:
		return new CvNetBuildListPopOrder();
	case GAMEMESSAGE_BUILD_LIST_EDIT:
		return new CvNetBuildListEdit();
	case GAMEMESSAGE_CHOOSE_TRAIT:
		return new CvNetChooseTrait();
	case GAMEMESSAGE_CHOOSE_MERGE_UNIT:
		return new CvNetChooseMergeUnit();
	case GAMEMESSAGE_CONFIRM_SPLIT_UNIT:
		return new CvNetConfirmSplitUnit();
	case GAMEMESSAGE_IMPROVEMENT_UPGRADE:
		return new CvNetImprovementUpgrade();
	case GAMEMESSAGE_CHOOSE_BUILDUP:
		return new CvNetChooseBuildUp();
	case GAMEMESSAGE_CHOOSE_ARREST_UNIT:
		return new CvNetChooseArrestUnit();
	case GAMEMESSAGE_CONFIRM_AMBUSH:
		return new CvNetConfirmAmbush();
	default:
		FErrorMsg("Unknown message type");
	}
	return NULL;
}

CvMessageData::CvMessageData(GameMessageTypes eType) : m_eType(eType)
{
}

CvMessageData::~CvMessageData()
{
}

GameMessageTypes CvMessageData::getType() const
{
	return m_eType;
}

CvNetExtendedGame::CvNetExtendedGame(PlayerTypes ePlayer) : CvMessageData(GAMEMESSAGE_EXTENDED_GAME), m_ePlayer(ePlayer)
{
}

void CvNetExtendedGame::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Extended Game, %d", m_ePlayer);
}

void CvNetExtendedGame::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		GET_PLAYER(m_ePlayer).makeExtendedGame();
	}
}

void CvNetExtendedGame::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	GC.getGame().logNetMsgData("Writing CvNetExtendedGame message with payload [%d]\n", m_ePlayer);
}

void CvNetExtendedGame::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	GC.getGame().logNetMsgData("Reading CvNetExtendedGame message with payload [%d]\n", m_ePlayer);
}

CvNetAutoMoves::CvNetAutoMoves(PlayerTypes ePlayer) : CvMessageData(GAMEMESSAGE_AUTO_MOVES), m_ePlayer(ePlayer)
{
}

void CvNetAutoMoves::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Auto Moves, %d", m_ePlayer);
}

void CvNetAutoMoves::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		GC.getGame().logNetMsgData("Executing CvNetAutoMoves message with payload [%d]\n", m_ePlayer);
		GET_PLAYER(m_ePlayer).setAutoMoves(true);
		GC.getGame().logNetMsgData("Executed CvNetAutoMoves message with payload [%d]\n", m_ePlayer);
	}
	else
	{
		GC.getGame().logNetMsgData("Skipped CvNetAutoMoves message due to invalid player with payload [%d]\n", m_ePlayer);
	}
}

void CvNetAutoMoves::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	GC.getGame().logNetMsgData("Writing CvNetAutoMoves message with payload [%d]\n", m_ePlayer);
}

void CvNetAutoMoves::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	GC.getGame().logNetMsgData("Reading CvNetAutoMoves message with payload [%d]\n", m_ePlayer);
}

CvNetTurnComplete::CvNetTurnComplete(PlayerTypes ePlayer) : CvMessageData(GAMEMESSAGE_TURN_COMPLETE), m_ePlayer(ePlayer)
{
}

void CvNetTurnComplete::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Turn Complete, %d", m_ePlayer);
}

void CvNetTurnComplete::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		GC.getGame().logNetMsgData("Executing CvNetTurnComplete message with payload [%d]\n", m_ePlayer);
		GET_PLAYER(m_ePlayer).changeEndTurn(true);
		GC.getGame().logNetMsgData("Executed CvNetTurnComplete message with payload [%d]\n", m_ePlayer);
	}
	else
	{
		GC.getGame().logNetMsgData("Skipped CvNetTurnComplete message due to invalid player with payload [%d]\n", m_ePlayer);
	}
}

void CvNetTurnComplete::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	GC.getGame().logNetMsgData("Writing CvNetTurnComplete message with payload [%d]\n", m_ePlayer);
}

void CvNetTurnComplete::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	GC.getGame().logNetMsgData("Reading CvNetTurnComplete message with payload [%d]\n", m_ePlayer);
}

CvNetPushOrder::CvNetPushOrder() : CvMessageData(GAMEMESSAGE_PUSH_ORDER), m_ePlayer(NO_PLAYER), m_iCityID(-1), m_eOrder(NO_ORDER), m_iData(-1), m_bAlt(false), m_bShift(false), m_bCtrl(false)
{
}

CvNetPushOrder::CvNetPushOrder(PlayerTypes ePlayer, int iCityID, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl) : CvMessageData(GAMEMESSAGE_PUSH_ORDER), m_ePlayer(ePlayer), m_iCityID(iCityID), m_eOrder(eOrder), m_iData(iData), m_bAlt(bAlt), m_bShift(bShift), m_bCtrl(bCtrl)
{
}

void CvNetPushOrder::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Pushing an order at the city, order is %d, data is %d", m_eOrder, m_iData);
}

void CvNetPushOrder::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		CvCity* pCity = GET_PLAYER(m_ePlayer).getCity(m_iCityID);
		if (pCity != NULL)
		{
			GC.getGame().logNetMsgData("Executing CvNetPushOrder message with payload [%d, %d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iCityID, m_eOrder, m_iData, m_bAlt, m_bShift, m_bCtrl);
			pCity->pushOrder(m_eOrder, m_iData, -1, m_bAlt, !(m_bShift || m_bCtrl), m_bShift);
			GC.getGame().logNetMsgData("Executed CvNetPushOrder message with payload [%d, %d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iCityID, m_eOrder, m_iData, m_bAlt, m_bShift, m_bCtrl);
		}
		else
		{
			GC.getGame().logNetMsgData("Skipped CvNetPushOrder message due to invalid city with payload [%d, %d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iCityID, m_eOrder, m_iData, m_bAlt, m_bShift, m_bCtrl);
		}

		if (GC.getGame().getActivePlayer() == m_ePlayer)
		{
			gDLL->getInterfaceIFace()->updatePythonScreens();
		}
	}
	else
	{
		GC.getGame().logNetMsgData("Skipped CvNetPushOrder message due to invalid player with payload [%d, %d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iCityID, m_eOrder, m_iData, m_bAlt, m_bShift, m_bCtrl);
	}
}

void CvNetPushOrder::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_iCityID);
	pStream->Write(m_eOrder);
	pStream->Write(m_iData);
	pStream->Write(m_bAlt);
	pStream->Write(m_bShift);
	pStream->Write(m_bCtrl);
	GC.getGame().logNetMsgData("Writing CvNetPushOrder message with payload [%d, %d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iCityID, m_eOrder, m_iData, m_bAlt, m_bShift, m_bCtrl);
}

void CvNetPushOrder::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read(&m_iCityID);
	pStream->Read((int*)&m_eOrder);
	pStream->Read(&m_iData);
	pStream->Read(&m_bAlt);
	pStream->Read(&m_bShift);
	pStream->Read(&m_bCtrl);
	GC.getGame().logNetMsgData("Reading CvNetPushOrder message with payload [%d, %d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iCityID, m_eOrder, m_iData, m_bAlt, m_bShift, m_bCtrl);
}

CvNetPopOrder::CvNetPopOrder() : CvMessageData(GAMEMESSAGE_POP_ORDER), m_ePlayer(NO_PLAYER), m_iCityID(-1), m_iNum(0)
{
}

CvNetPopOrder::CvNetPopOrder(PlayerTypes ePlayer, int iCityID, int iNum) : CvMessageData(GAMEMESSAGE_POP_ORDER), m_ePlayer(ePlayer), m_iCityID(iCityID), m_iNum(iNum)
{
}

void CvNetPopOrder::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Pop Order at City, city ID is %d", m_iCityID);
}

void CvNetPopOrder::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		CvCity* pCity = GET_PLAYER(m_ePlayer).getCity(m_iCityID);
		if (pCity != NULL)
		{
			GC.getGame().logNetMsgData("Executing CvNetPopOrder message with payload [%d, %d, %d]\n", m_ePlayer, m_iCityID, m_iNum);
			pCity->popOrder(m_iNum);
			GC.getGame().logNetMsgData("Executed CvNetPopOrder message with payload [%d, %d, %d]\n", m_ePlayer, m_iCityID, m_iNum);
			//pCity->setBuildingListInvalid();
			//pCity->setUnitListInvalid();
		}
		else
		{
			GC.getGame().logNetMsgData("Skipping CvNetPopOrder message due to invalid city with payload [%d, %d, %d]\n", m_ePlayer, m_iCityID, m_iNum);
		}

		if (GC.getGame().getActivePlayer() == m_ePlayer)
		{
			gDLL->getInterfaceIFace()->updatePythonScreens();
		}
	}
	else
	{
		GC.getGame().logNetMsgData("Skipping CvNetPopOrder message due to invalid player with payload [%d, %d, %d]\n", m_ePlayer, m_iCityID, m_iNum);
	}
}

void CvNetPopOrder::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_iCityID);
	pStream->Write(m_iNum);
	GC.getGame().logNetMsgData("Writing CvNetPopOrder message with payload [%d, %d, %d]\n", m_ePlayer, m_iCityID, m_iNum);
}

void CvNetPopOrder::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read(&m_iCityID);
	pStream->Read(&m_iNum);
	GC.getGame().logNetMsgData("Reading CvNetPopOrder message with payload [%d, %d, %d]\n", m_ePlayer, m_iCityID, m_iNum);
}

CvNetDoTask::CvNetDoTask() : CvMessageData(GAMEMESSAGE_DO_TASK), m_ePlayer(NO_PLAYER), m_iCityID(-1), m_eTask(NO_TASK), m_iData1(-1), m_iData2(-1), m_bOption(false), m_bAlt(false), m_bShift(false), m_bCtrl(false)
{
}

CvNetDoTask::CvNetDoTask(PlayerTypes ePlayer, int iCityID, TaskTypes eTask, int iData1, int iData2, bool bOption, bool bAlt, bool bShift, bool bCtrl) : CvMessageData(GAMEMESSAGE_DO_TASK), m_ePlayer(ePlayer), m_iCityID(iCityID), m_eTask(eTask), m_iData1(iData1), m_iData2(iData2), m_bOption(bOption), m_bAlt(bAlt), m_bShift(bShift), m_bCtrl(bCtrl)
{
}

void CvNetDoTask::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Do Task at City, city ID is %d", m_iCityID);
}

void CvNetDoTask::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		CvCity* pCity = GET_PLAYER(m_ePlayer).getCity(m_iCityID);
		if (pCity != NULL)
		{
			GC.getGame().logNetMsgData("Executing CvNetDoTask message with payload [%d, %d, %d, %d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iCityID, m_eTask, m_iData1, m_iData2, m_bOption, m_bAlt, m_bShift, m_bCtrl);
			pCity->doTask(m_eTask, m_iData1, m_iData2, m_bOption, m_bAlt, m_bShift, m_bCtrl);
			GC.getGame().logNetMsgData("Executed CvNetDoTask message with payload [%d, %d, %d, %d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iCityID, m_eTask, m_iData1, m_iData2, m_bOption, m_bAlt, m_bShift, m_bCtrl);
		}
		else
		{
			GC.getGame().logNetMsgData("Skipping CvNetDoTask message due to invalid city with payload [%d, %d, %d, %d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iCityID, m_eTask, m_iData1, m_iData2, m_bOption, m_bAlt, m_bShift, m_bCtrl);
		}
	}
	else
	{
		GC.getGame().logNetMsgData("Skipping CvNetDoTask message due to invalid player with payload [%d, %d, %d, %d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iCityID, m_eTask, m_iData1, m_iData2, m_bOption, m_bAlt, m_bShift, m_bCtrl);
	}
}

void CvNetDoTask::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_iCityID);
	pStream->Write(m_eTask);
	pStream->Write(m_iData1);
	pStream->Write(m_iData2);
	pStream->Write(m_bOption);
	pStream->Write(m_bAlt);
	pStream->Write(m_bShift);
	pStream->Write(m_bCtrl);
	GC.getGame().logNetMsgData("Writing CvNetDoTask message with payload [%d, %d, %d, %d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iCityID, m_eTask, m_iData1, m_iData2, m_bOption, m_bAlt, m_bShift, m_bCtrl);
}

void CvNetDoTask::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read(&m_iCityID);
	pStream->Read((int*)&m_eTask);
	pStream->Read(&m_iData1);
	pStream->Read(&m_iData2);
	pStream->Read(&m_bOption);
	pStream->Read(&m_bAlt);
	pStream->Read(&m_bShift);
	pStream->Read(&m_bCtrl);
	GC.getGame().logNetMsgData("Reading CvNetDoTask message with payload [%d, %d, %d, %d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iCityID, m_eTask, m_iData1, m_iData2, m_bOption, m_bAlt, m_bShift, m_bCtrl);
}

CvNetUpdateCivics::CvNetUpdateCivics() : CvMessageData(GAMEMESSAGE_UPDATE_CIVICS), m_ePlayer(NO_PLAYER), m_aeCivics(GC.getNumCivicOptionInfos(), NO_CIVIC)
{
}

CvNetUpdateCivics::CvNetUpdateCivics(PlayerTypes ePlayer, const std::vector<CivicTypes>& aeCivics) : CvMessageData(GAMEMESSAGE_UPDATE_CIVICS), m_ePlayer(ePlayer), m_aeCivics(aeCivics)
{
}


void CvNetUpdateCivics::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Update Civics, ePlayer is %d", (int)m_ePlayer);
}

void CvNetUpdateCivics::Execute()
{
	if (m_ePlayer != NO_PLAYER && !m_aeCivics.empty())
	{
		GET_PLAYER(m_ePlayer).revolution(&m_aeCivics[0]);
	}
}

void CvNetUpdateCivics::PutInBuffer(FDataStreamBase* pStream)
{
	PROFILE_EXTRA_FUNC();
	pStream->Write(m_ePlayer);
	for (int i = 0; i < GC.getNumCivicOptionInfos(); i++)
	{
		pStream->Write(m_aeCivics[i]);
	}
}

void CvNetUpdateCivics::SetFromBuffer(FDataStreamBase* pStream)
{
	PROFILE_EXTRA_FUNC();
	pStream->Read((int*)&m_ePlayer);
	for (int i = 0; i < GC.getNumCivicOptionInfos(); i++)
	{
		pStream->Read((int*)&m_aeCivics[i]);
	}
}

CvNetResearch::CvNetResearch() : CvMessageData(GAMEMESSAGE_RESEARCH), m_ePlayer(NO_PLAYER), m_iDiscover(-1), m_bShift(false), m_eTech(NO_TECH)
{
}

CvNetResearch::CvNetResearch(PlayerTypes ePlayer, TechTypes eTech, int iDiscover, bool bShift) : CvMessageData(GAMEMESSAGE_RESEARCH), m_ePlayer(ePlayer), m_iDiscover(iDiscover), m_bShift(bShift), m_eTech(eTech)
{
}

void CvNetResearch::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Research, %d",	m_eTech);
}

void CvNetResearch::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		if (m_iDiscover > 0)
		{
			GET_TEAM(GET_PLAYER(m_ePlayer).getTeam()).setHasTech(m_eTech, true, m_ePlayer, true, true);

			if (m_iDiscover > 1 && m_ePlayer == GC.getGame().getActivePlayer())
			{
				GET_PLAYER(m_ePlayer).chooseTech(m_iDiscover - 1);
			}
		}
		else if (m_eTech != NO_TECH)
		{
			CvPlayer& kPlayer = GET_PLAYER(m_ePlayer);
			if (kPlayer.canResearch(m_eTech, false))
			{
				if (!m_bShift && kPlayer.isResearchingTech(m_eTech))
				{
					kPlayer.clearResearchQueue();
				}
				kPlayer.pushResearch(m_eTech, !m_bShift);
			}
		}
		else GET_PLAYER(m_ePlayer).clearResearchQueue();


		if (GC.getGame().getActivePlayer() == m_ePlayer)
		{
			gDLL->getInterfaceIFace()->updatePythonScreens();
		}
	}
}

void CvNetResearch::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_iDiscover);
	pStream->Write(m_bShift);
	pStream->Write(m_eTech);
}

void CvNetResearch::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read(&m_iDiscover);
	pStream->Read(&m_bShift);
	pStream->Read((int*)&m_eTech);
}

CvNetEspionageChange::CvNetEspionageChange() : CvMessageData(GAMEMESSAGE_ESPIONAGE_CHANGE), m_ePlayer(NO_PLAYER), m_eTargetTeam(NO_TEAM), m_iChange(0)
{
}

CvNetEspionageChange::CvNetEspionageChange(PlayerTypes ePlayer, TeamTypes eTargetTeam, int iChange) : CvMessageData(GAMEMESSAGE_ESPIONAGE_CHANGE), m_ePlayer(ePlayer), m_eTargetTeam(eTargetTeam), m_iChange(iChange)
{
}

void CvNetEspionageChange::Debug(char* szAddendum)
{
	sprintf(szAddendum, "TargetTeam: %d, Espionage Change: %d", m_eTargetTeam, m_iChange);
}

void CvNetEspionageChange::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		GET_PLAYER(m_ePlayer).changeEspionageSpendingWeightAgainstTeam(m_eTargetTeam, m_iChange);
	}
}

void CvNetEspionageChange::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_eTargetTeam);
	pStream->Write(m_iChange);
}

void CvNetEspionageChange::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read((int*)&m_eTargetTeam);
	pStream->Read(&m_iChange);
}

CvNetAdvancedStartAction::CvNetAdvancedStartAction(AdvancedStartActionTypes eAction, PlayerTypes ePlayer, int iX, int iY, int iData, bool bAdd) : CvMessageData(GAMEMESSAGE_ADVANCED_START_ACTION),
	m_eAction(eAction),
	m_ePlayer(ePlayer),
	m_iX(iX),
	m_iY(iY),
	m_iData(iData),
	m_bAdd(bAdd)
{
}

void CvNetAdvancedStartAction::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Non-simultaneous Advanced Start Action notification");
}

void CvNetAdvancedStartAction::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		if (!gDLL->getInterfaceIFace()->isExitingToMainMenu())
		{
			GET_PLAYER(m_ePlayer).doAdvancedStartAction(m_eAction, m_iX, m_iY, m_iData, m_bAdd);
		}
	}
}

void CvNetAdvancedStartAction::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_eAction);
	pStream->Write(m_ePlayer);
	pStream->Write(m_iX);
	pStream->Write(m_iY);
	pStream->Write(m_iData);
	pStream->Write(m_bAdd);
}

void CvNetAdvancedStartAction::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_eAction);
	pStream->Read((int*)&m_ePlayer);
	pStream->Read(&m_iX);
	pStream->Read(&m_iY);
	pStream->Read(&m_iData);
	pStream->Read(&m_bAdd);
}

CvNetModNetMessage::CvNetModNetMessage(int iData1, int iData2, int iData3, int iData4, int iData5) : CvMessageData(GAMEMESSAGE_MOD_NET_MESSAGE), m_iData1(iData1), m_iData2(iData2), m_iData3(iData3), m_iData4(iData4), m_iData5(iData5)
{
}

void CvNetModNetMessage::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Non-simultaneous ModNetMessage notification");
}

void CvNetModNetMessage::Execute()
{
	GC.getGame().logNetMsgData("Executing CvNetModNetMessage message with payload [%d, %d, %d, %d, %d]\n", m_iData1, m_iData2, m_iData3, m_iData4, m_iData5);
	CvEventReporter::getInstance().reportModNetMessage(m_iData1, m_iData2, m_iData3, m_iData4, m_iData5);
	GC.getGame().logNetMsgData("Executed CvNetModNetMessage message with payload [%d, %d, %d, %d, %d]\n", m_iData1, m_iData2, m_iData3, m_iData4, m_iData5);
}

void CvNetModNetMessage::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_iData1);
	pStream->Write(m_iData2);
	pStream->Write(m_iData3);
	pStream->Write(m_iData4);
	pStream->Write(m_iData5);
	GC.getGame().logNetMsgData("Writing CvNetModNetMessage message with payload [%d, %d, %d, %d, %d]\n", m_iData1, m_iData2, m_iData3, m_iData4, m_iData5);
}

void CvNetModNetMessage::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read(&m_iData1);
	pStream->Read(&m_iData2);
	pStream->Read(&m_iData3);
	pStream->Read(&m_iData4);
	pStream->Read(&m_iData5);
	GC.getGame().logNetMsgData("Reading CvNetModNetMessage message with payload [%d, %d, %d, %d, %d]\n", m_iData1, m_iData2, m_iData3, m_iData4, m_iData5);
}

CvNetConvert::CvNetConvert(PlayerTypes ePlayer, ReligionTypes eReligion) : CvMessageData(GAMEMESSAGE_CONVERT), m_ePlayer(ePlayer), m_eReligion(eReligion)
{
}

void CvNetConvert::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Religion: %d", m_eReligion);
}

void CvNetConvert::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		GET_PLAYER(m_ePlayer).convert(m_eReligion);
	}
}

void CvNetConvert::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_eReligion);
}

void CvNetConvert::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read((int*)&m_eReligion);
}

CvNetEmpireSplit::CvNetEmpireSplit(PlayerTypes ePlayer, int iAreaId) : CvMessageData(GAMEMESSAGE_EMPIRE_SPLIT),
	m_ePlayer(ePlayer),
	m_iAreaId(iAreaId)
{
}

void CvNetEmpireSplit::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Non-simultaneous empire split notification");
}

void CvNetEmpireSplit::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		GET_PLAYER(m_ePlayer).splitEmpire(m_iAreaId);
	}
}

void CvNetEmpireSplit::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_iAreaId);
}

void CvNetEmpireSplit::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read(&m_iAreaId);
}

CvNetFoundReligion::CvNetFoundReligion(PlayerTypes ePlayer, ReligionTypes eReligion, ReligionTypes eSlotReligion) : CvMessageData(GAMEMESSAGE_FOUND_RELIGION),
	m_ePlayer(ePlayer),
	m_eReligion(eReligion),
	m_eSlotReligion(eSlotReligion)
{
}

void CvNetFoundReligion::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Non-simultaneous found religion notification");
}

void CvNetFoundReligion::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		GET_PLAYER(m_ePlayer).foundReligion(m_eReligion, m_eSlotReligion, true);
	}
}

void CvNetFoundReligion::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_eReligion);
	pStream->Write(m_eSlotReligion);
}

void CvNetFoundReligion::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read((int*)&m_eReligion);
	pStream->Read((int*)&m_eSlotReligion);
}

CvNetLaunchSpaceship::CvNetLaunchSpaceship(PlayerTypes ePlayer, VictoryTypes eVictory) : CvMessageData(GAMEMESSAGE_LAUNCH_SPACESHIP),
	m_ePlayer(ePlayer),
	m_eVictory(eVictory)
{
}

void CvNetLaunchSpaceship::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Non-simultaneous spaceship launch notification");
}

void CvNetLaunchSpaceship::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		GET_PLAYER(m_ePlayer).launch(m_eVictory);
	}
}

void CvNetLaunchSpaceship::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_eVictory);
}

void CvNetLaunchSpaceship::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read((int*)&m_eVictory);
}

CvNetEventTriggered::CvNetEventTriggered(PlayerTypes ePlayer, EventTypes eEvent, int iEventTriggeredId) : CvMessageData(GAMEMESSAGE_EVENT_TRIGGERED),
	m_ePlayer(ePlayer),
	m_eEvent(eEvent),
	m_iEventTriggeredId(iEventTriggeredId)
{
}

void CvNetEventTriggered::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Non-simultaneous event trigger notification");
}

void CvNetEventTriggered::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		GET_PLAYER(m_ePlayer).applyEvent(m_eEvent, m_iEventTriggeredId);
	}
}

void CvNetEventTriggered::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_eEvent);
	pStream->Write(m_iEventTriggeredId);
}

void CvNetEventTriggered::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read((int*)&m_eEvent);
	pStream->Read((int*)&m_iEventTriggeredId);
}

CvNetJoinGroup::CvNetJoinGroup() : CvMessageData(GAMEMESSAGE_JOIN_GROUP), m_ePlayer(NO_PLAYER), m_iUnitID(-1), m_iHeadID(-1)
{
}

CvNetJoinGroup::CvNetJoinGroup(PlayerTypes ePlayer, int iUnitID, int iHeadID) : CvMessageData(GAMEMESSAGE_JOIN_GROUP), m_ePlayer(ePlayer), m_iUnitID(iUnitID), m_iHeadID(iHeadID)
{
}

void CvNetJoinGroup::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Join Group unit %d -> head unit %d", m_iUnitID, m_iHeadID);
}

void CvNetJoinGroup::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		CvUnit* pUnit = GET_PLAYER(m_ePlayer).getUnit(m_iUnitID);
		if (pUnit != NULL)
		{
			GC.getGame().logNetMsgData("Executing CvNetJoinGroup message with payload [%d, %d, %d]\n", m_ePlayer, m_iUnitID, m_iHeadID);
			CvUnit* pHeadUnit = GET_PLAYER(m_ePlayer).getUnit(m_iHeadID);
			if (pHeadUnit != NULL)
			{
				pUnit->joinGroup(pHeadUnit->getGroup());
			}
			else
			{
				pUnit->joinGroup(NULL);
			}
			GC.getGame().logNetMsgData("Executed CvNetJoinGroup message with payload [%d, %d, %d]\n", m_ePlayer, m_iUnitID, m_iHeadID);

		}
		else
		{
			GC.getGame().logNetMsgData("Skipping CvNetJoinGroup message due to invalid unit id with payload [%d, %d, %d]\n", m_ePlayer, m_iUnitID, m_iHeadID);
		}
	}
	else
	{
		GC.getGame().logNetMsgData("Skipping CvNetJoinGroup message due to invalid player with payload [%d, %d, %d]\n", m_ePlayer, m_iUnitID, m_iHeadID);
	}
}

void CvNetJoinGroup::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_iUnitID);
	pStream->Write(m_iHeadID);
	GC.getGame().logNetMsgData("Writing CvNetJoinGroup message with payload [%d, %d, %d]\n", m_ePlayer, m_iUnitID, m_iHeadID);


}

void CvNetJoinGroup::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read(&m_iUnitID);
	pStream->Read(&m_iHeadID);
	GC.getGame().logNetMsgData("Reading CvNetJoinGroup message with payload [%d, %d, %d]\n", m_ePlayer, m_iUnitID, m_iHeadID);

}

CvNetPushMission::CvNetPushMission() : CvMessageData(GAMEMESSAGE_PUSH_MISSION), m_ePlayer(NO_PLAYER), m_iUnitID(-1), m_eMission(NO_MISSION), m_iData1(-1), m_iData2(-1), m_iFlags(0), m_bShift(false)
{
}

CvNetPushMission::CvNetPushMission(PlayerTypes ePlayer, int iUnitID, MissionTypes eMission, int iData1, int iData2, int iFlags, bool bShift) : CvMessageData(GAMEMESSAGE_PUSH_MISSION), m_ePlayer(ePlayer), m_iUnitID(iUnitID), m_eMission(eMission), m_iData1(iData1), m_iData2(iData2), m_iFlags(iFlags), m_bShift(bShift)
{
}

void CvNetPushMission::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Do Mission, who is %d, unit ID is %d, mission is %S", m_ePlayer, m_iUnitID, GC.getMissionInfo(m_eMission).getDescription());
}

void CvNetPushMission::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		CvUnit* pUnit = GET_PLAYER(m_ePlayer).getUnit(m_iUnitID);
		if (pUnit != NULL)
		{
			CvSelectionGroup* pSelectionGroup = pUnit->getGroup();
			if (pSelectionGroup != NULL)
			{
				GC.getGame().logNetMsgData("Executing CvNetPushMission message with payload [%d, %d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iUnitID, m_eMission, m_iData1, m_iData2, m_iFlags, m_bShift);
				pSelectionGroup->pushMission(m_eMission, m_iData1, m_iData2, m_iFlags, m_bShift, true);
				GC.getGame().logNetMsgData("Executed CvNetPushMission message with payload [%d, %d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iUnitID, m_eMission, m_iData1, m_iData2, m_iFlags, m_bShift);
			}
			else
			{
				GC.getGame().logNetMsgData("Skipping CvNetPushMission message due to invalid selection group with payload [%d, %d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iUnitID, m_eMission, m_iData1, m_iData2, m_iFlags, m_bShift);
			}
		}
		else
		{
			GC.getGame().logNetMsgData("Skipping CvNetPushMission message due to invalid unit id with payload [%d, %d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iUnitID, m_eMission, m_iData1, m_iData2, m_iFlags, m_bShift);
		}
	}
	else
	{
		GC.getGame().logNetMsgData("Skipping CvNetPushMission message due to invalid player with payload [%d, %d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iUnitID, m_eMission, m_iData1, m_iData2, m_iFlags, m_bShift);
	}
}

void CvNetPushMission::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_iUnitID);
	pStream->Write(m_eMission);
	pStream->Write(m_iData1);
	pStream->Write(m_iData2);
	pStream->Write(m_iFlags);
	pStream->Write(m_bShift);
	GC.getGame().logNetMsgData("Writing CvNetPushMission message with payload [%d, %d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iUnitID, m_eMission, m_iData1, m_iData2, m_iFlags, m_bShift);
}

void CvNetPushMission::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read(&m_iUnitID);
	pStream->Read((int*)&m_eMission);
	pStream->Read(&m_iData1);
	pStream->Read(&m_iData2);
	pStream->Read(&m_iFlags);
	pStream->Read(&m_bShift);
	GC.getGame().logNetMsgData("Reading CvNetPushMission message with payload [%d, %d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iUnitID, m_eMission, m_iData1, m_iData2, m_iFlags, m_bShift);
}

CvNetAutoMission::CvNetAutoMission() : CvMessageData(GAMEMESSAGE_AUTO_MISSION), m_ePlayer(NO_PLAYER), m_iUnitID(-1)
{
}

CvNetAutoMission::CvNetAutoMission(PlayerTypes ePlayer, int iUnitID) : CvMessageData(GAMEMESSAGE_AUTO_MISSION), m_ePlayer(ePlayer), m_iUnitID(iUnitID)
{
}

void CvNetAutoMission::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Auto Mission, unit ID is %d",	m_iUnitID);
}

void CvNetAutoMission::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		CvUnit* pUnit = GET_PLAYER(m_ePlayer).getUnit(m_iUnitID);
		if (pUnit != NULL)
		{
			CvSelectionGroup* pSelectionGroup = pUnit->getGroup();
			if (pSelectionGroup != NULL)
			{
				GC.getGame().logNetMsgData("Executing CvNetAutoMission message with payload [%d, %d]\n", m_ePlayer, m_iUnitID);
				pSelectionGroup->autoMission();
				GC.getGame().logNetMsgData("Executed CvNetAutoMission message with payload [%d, %d]\n", m_ePlayer, m_iUnitID);
			}
			else
			{
				GC.getGame().logNetMsgData("Skipping CvNetAutoMission message due to invalid selection group with payload [%d, %d]\n", m_ePlayer, m_iUnitID);
			}
		}
		else
		{
			GC.getGame().logNetMsgData("Skipping CvNetAutoMission message due to invalid unit id with payload [%d, %d]\n", m_ePlayer, m_iUnitID);
		}
	}
	else
	{
		GC.getGame().logNetMsgData("Skipping CvNetAutoMission message due to invalid player with payload [%d, %d]\n", m_ePlayer, m_iUnitID);
	}
}

void CvNetAutoMission::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_iUnitID);
	GC.getGame().logNetMsgData("Writing CvNetAutoMission message with payload [%d, %d]\n", m_ePlayer, m_iUnitID);
}

void CvNetAutoMission::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read(&m_iUnitID);
	GC.getGame().logNetMsgData("Reading CvNetAutoMission message with payload [%d, %d]\n", m_ePlayer, m_iUnitID);
}

CvNetDoCommand::CvNetDoCommand() : CvMessageData(GAMEMESSAGE_DO_COMMAND), m_ePlayer(NO_PLAYER), m_iUnitID(-1), m_eCommand(NO_COMMAND), m_iData1(-1), m_iData2(-1), m_bAlt(false)
{
}

CvNetDoCommand::CvNetDoCommand(PlayerTypes ePlayer, int iUnitID, CommandTypes eCommand, int iData1, int iData2, bool bAlt) : CvMessageData(GAMEMESSAGE_DO_COMMAND), m_ePlayer(ePlayer), m_iUnitID(iUnitID), m_eCommand(eCommand), m_iData1(iData1), m_iData2(iData2), m_bAlt(bAlt)
{
}

void CvNetDoCommand::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Do Command, who is %d, unit ID is %d, command is %d", m_ePlayer, m_iUnitID, m_eCommand);
}

void CvNetDoCommand::Execute()
{
	PROFILE_EXTRA_FUNC();
	if (m_ePlayer != NO_PLAYER)
	{
		CvUnit* pUnit = GET_PLAYER(m_ePlayer).getUnit(m_iUnitID);
		if (pUnit != NULL)
		{
			if (m_bAlt && GC.getCommandInfo(m_eCommand).getAll())
			{
				// Have to save type ahead of time, pointer can change
				const UnitTypes eUpgradeType = pUnit->getUnitType();
				foreach_(CvUnit* pLoopUnit, GET_PLAYER(m_ePlayer).units())
				{
					if (pLoopUnit->getUnitType() == eUpgradeType)
					{
						GC.getGame().logNetMsgData("Executing CvNetDoCommand message (all of same unit type) with payload [%d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iUnitID, m_eCommand, m_iData1, m_iData2, m_bAlt);
						pLoopUnit->doCommand(m_eCommand, m_iData1, m_iData2);
					}
				}
			}
			else
			{
				GC.getGame().logNetMsgData("Executing CvNetDoCommand  message (single unit) with payload [%d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iUnitID, m_eCommand, m_iData1, m_iData2, m_bAlt);
				pUnit->doCommand(m_eCommand, m_iData1, m_iData2);
			}
			GC.getGame().logNetMsgData("Executed CvNetDoCommand message with payload [%d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iUnitID, m_eCommand, m_iData1, m_iData2, m_bAlt);
		}
		else
		{
			GC.getGame().logNetMsgData("Skipping CvNetDoCommand message due to invalid unit id with payload [%d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iUnitID, m_eCommand, m_iData1, m_iData2, m_bAlt);
		}
		}
	else
	{
		GC.getGame().logNetMsgData("Skipping CvNetDoCommand message due to invalid player with payload [%d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iUnitID, m_eCommand, m_iData1, m_iData2, m_bAlt);
	}
}

void CvNetDoCommand::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_iUnitID);
	pStream->Write(m_eCommand);
	pStream->Write(m_iData1);
	pStream->Write(m_iData2);
	pStream->Write(m_bAlt);
	GC.getGame().logNetMsgData("Writing CvNetDoCommand message with payload [%d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iUnitID, m_eCommand, m_iData1, m_iData2, m_bAlt);
}

void CvNetDoCommand::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read(&m_iUnitID);
	pStream->Read((int*)&m_eCommand);
	pStream->Read(&m_iData1);
	pStream->Read(&m_iData2);
	pStream->Read(&m_bAlt);
	GC.getGame().logNetMsgData("Reading CvNetDoCommand message with payload [%d, %d, %d, %d, %d, %d]\n", m_ePlayer, m_iUnitID, m_eCommand, m_iData1, m_iData2, m_bAlt);
}

CvNetPercentChange::CvNetPercentChange() : CvMessageData(GAMEMESSAGE_PERCENT_CHANGE), m_ePlayer(NO_PLAYER), m_iChange(0), m_eCommerce(NO_COMMERCE)
{
}

CvNetPercentChange::CvNetPercentChange(PlayerTypes ePlayer, CommerceTypes eCommerce, int iChange) : CvMessageData(GAMEMESSAGE_PERCENT_CHANGE), m_ePlayer(ePlayer), m_iChange(iChange), m_eCommerce(eCommerce)
{
}

void CvNetPercentChange::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Commerce: %d, Percent Change: %d", m_eCommerce, m_iChange);
}

void CvNetPercentChange::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		GET_PLAYER(m_ePlayer).changeCommercePercent(m_eCommerce, m_iChange);
	}
}

void CvNetPercentChange::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_iChange);
	pStream->Write(m_eCommerce);
}

void CvNetPercentChange::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read(&m_iChange);
	pStream->Read((int*)&m_eCommerce);
}

CvNetChangeVassal::CvNetChangeVassal() : CvMessageData(GAMEMESSAGE_CHANGE_VASSAL), m_ePlayer(NO_PLAYER), m_eMasterTeam(NO_TEAM), m_bVassal(false), m_bCapitulated(false)
{
}

CvNetChangeVassal::CvNetChangeVassal(PlayerTypes ePlayer, TeamTypes eMasterTeam, bool bVassal, bool bCapitulated) : CvMessageData(GAMEMESSAGE_CHANGE_VASSAL), m_ePlayer(ePlayer), m_eMasterTeam(eMasterTeam), m_bVassal(bVassal), m_bCapitulated(bCapitulated)
{
}

void CvNetChangeVassal::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Change Vassal, master team is %d", m_eMasterTeam);
}

void CvNetChangeVassal::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		FAssert(GET_PLAYER(m_ePlayer).getTeam() != m_eMasterTeam);
		GET_TEAM(GET_PLAYER(m_ePlayer).getTeam()).setVassal(m_eMasterTeam, m_bVassal, m_bCapitulated);
	}
}

void CvNetChangeVassal::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_eMasterTeam);
	pStream->Write(m_bVassal);
	pStream->Write(m_bCapitulated);
}

void CvNetChangeVassal::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read((int*)&m_eMasterTeam);
	pStream->Read(&m_bVassal);
	pStream->Read(&m_bCapitulated);
}

CvNetChooseElection::CvNetChooseElection() : CvMessageData(GAMEMESSAGE_CHOOSE_ELECTION), m_ePlayer(NO_PLAYER), m_iSelection(-1), m_iVoteId(-1)
{
}

CvNetChooseElection::CvNetChooseElection(PlayerTypes ePlayer, int iSelection, int iVoteId) : CvMessageData(GAMEMESSAGE_CHOOSE_ELECTION), m_ePlayer(ePlayer), m_iSelection(iSelection), m_iVoteId(iVoteId)
{
}

void CvNetChooseElection::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Choose Election: %d", m_iSelection);
}

void CvNetChooseElection::Execute()
{
	GC.getGame().setVoteChosen(m_iSelection, m_iVoteId);
}

void CvNetChooseElection::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_iSelection);
	pStream->Write(m_iVoteId);
}

void CvNetChooseElection::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read(&m_iSelection);
	pStream->Read(&m_iVoteId);
}

CvNetDiploVote::CvNetDiploVote() : CvMessageData(GAMEMESSAGE_DIPLO_VOTE), m_ePlayer(NO_PLAYER), m_iVoteId(-1), m_eChoice(NO_PLAYER_VOTE)
{
}

CvNetDiploVote::CvNetDiploVote(PlayerTypes ePlayer, int iVoteId, PlayerVoteTypes eChoice) : CvMessageData(GAMEMESSAGE_DIPLO_VOTE), m_ePlayer(ePlayer), m_iVoteId(iVoteId), m_eChoice(eChoice)
{
}

void CvNetDiploVote::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Diplo Vote for %d; Choice is %d", m_iVoteId, m_eChoice);
}

void CvNetDiploVote::Execute()
{
	GC.getGame().castVote(m_ePlayer, m_iVoteId, m_eChoice);
}

void CvNetDiploVote::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_iVoteId);
	pStream->Write(m_eChoice);
}

void CvNetDiploVote::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read(&m_iVoteId);
	pStream->Read((int*)&m_eChoice);
}

CvNetChangeWar::CvNetChangeWar() : CvMessageData(GAMEMESSAGE_CHANGE_WAR), m_ePlayer(NO_PLAYER), m_eRivalTeam(NO_TEAM), m_bWar(false)
{
}

CvNetChangeWar::CvNetChangeWar(PlayerTypes ePlayer, TeamTypes eRivalTeam, bool bWar) : CvMessageData(GAMEMESSAGE_CHANGE_WAR), m_ePlayer(ePlayer), m_eRivalTeam(eRivalTeam), m_bWar(bWar)
{
}

void CvNetChangeWar::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Change War, rival team is %d", m_eRivalTeam);
}

void CvNetChangeWar::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		FAssert(GET_PLAYER(m_ePlayer).getTeam() != m_eRivalTeam);
		if (m_bWar)
		{
			GET_TEAM(GET_PLAYER(m_ePlayer).getTeam()).declareWar(m_eRivalTeam, false, NO_WARPLAN);

			if (gDLL->isDiplomacy() && GET_PLAYER((PlayerTypes) gDLL->getDiplomacyPlayer()).getTeam() == m_eRivalTeam)
			{
				gDLL->endDiplomacy();
			}
		}
		else
		{
			GET_TEAM(GET_PLAYER(m_ePlayer).getTeam()).makePeace(m_eRivalTeam);
		}
	}
}

void CvNetChangeWar::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_eRivalTeam);
	pStream->Write(m_bWar);
}

void CvNetChangeWar::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read((int*)&m_eRivalTeam);
	pStream->Read(&m_bWar);
}

CvNetPing::CvNetPing(PlayerTypes ePlayer, int iX, int iY) : CvMessageData(GAMEMESSAGE_PING), m_ePlayer(ePlayer), m_iX(iX), m_iY(iY)
{
}

void CvNetPing::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Ping message received");
}

void CvNetPing::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_iX);
	pStream->Write(m_iY);
}

void CvNetPing::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read(&m_iX);
	pStream->Read(&m_iY);
}

void CvNetPing::Execute()
{
	if (GC.getGame().getActiveTeam() != NO_TEAM)
	{
		if (GET_PLAYER(m_ePlayer).getTeam() == GC.getGame().getActiveTeam() || GET_TEAM(GC.getGame().getActiveTeam()).isVassal(GET_PLAYER(m_ePlayer).getTeam()) || GET_TEAM(GET_PLAYER(m_ePlayer).getTeam()).isVassal(GC.getGame().getActiveTeam()))
		{
			gDLL->getInterfaceIFace()->doPing(m_iX, m_iY, m_ePlayer);
		}
	}
}


#include "CyArgsList.h"
#include "CvDLLPythonIFaceBase.h"
CvNetAddReminder::CvNetAddReminder(PlayerTypes ePlayer, int iGameTurn, CvWString szMessage) : CvMessageData(GAMEMESSAGE_ADD_REMINDER), m_ePlayer(ePlayer), m_iGameTurn(iGameTurn), m_szMessage(szMessage)
{
}

void CvNetAddReminder::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Add Reminder, player %d on turn %d: %S", m_ePlayer, m_iGameTurn, m_szMessage.c_str());
}

void CvNetAddReminder::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		Cy::call(PYCivModule, "netAddReminder", Cy::Args() << m_ePlayer << m_iGameTurn << m_szMessage.c_str());
	}
}

void CvNetAddReminder::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_iGameTurn);
	pStream->WriteString(m_szMessage);
}

void CvNetAddReminder::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read(&m_iGameTurn);
	pStream->ReadString(m_szMessage);
}


CvGlobalDefineUpdate::CvGlobalDefineUpdate(CvWString szName, int iValue, float fValue, CvWString szValue) : CvMessageData(GAMEMESSAGE_SET_GLOBAL_DEFINE), m_szDefine(szName), m_iValue(iValue), m_fValue(fValue), m_szValue(szValue)
{
}

void CvGlobalDefineUpdate::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Updating Global Define %S, New Values are Int %d, Float %f, String %S", m_szDefine.c_str(), m_iValue, m_fValue, m_szValue.c_str());
}

void CvGlobalDefineUpdate::Execute()
{
	if (!m_szDefine.empty())
	{
		if (m_iValue != -1)
		{
			char szDefine[255];
			sprintf(szDefine,"%S",m_szDefine.c_str());
			GC.setDefineINT(szDefine, m_iValue, false);
		}
		if (m_fValue != -1.0f)
		{
			char szDefine[255];
			sprintf(szDefine,"%S",m_szDefine.c_str());
			GC.setDefineFLOAT(szDefine, m_fValue, false);
		}
		if (!m_szValue.empty())
		{
			char szDefine[255];
			sprintf(szDefine,"%S",m_szDefine.c_str());

			char szValue[255];
			sprintf(szValue,"%S",m_szValue.c_str());
			GC.setDefineSTRING(szDefine, szValue, false);
		}
	}
}

void CvGlobalDefineUpdate::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->WriteString(m_szDefine);
	pStream->Write(m_iValue);
	pStream->Write(m_fValue);
	pStream->WriteString(m_szValue);
}

void CvGlobalDefineUpdate::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->ReadString(m_szDefine);
	pStream->Read(&m_iValue);
	pStream->Read(&m_fValue);
	pStream->ReadString(m_szValue);
}


CvNetRecalculateModifiers::CvNetRecalculateModifiers() : CvMessageData(GAMEMESSAGE_RECALCULATE_MODIFIERS) { }

void CvNetRecalculateModifiers::Debug(char *szAddendum)
{
	sprintf(szAddendum, "Starting modifier recalculation");
}

void CvNetRecalculateModifiers::Execute()
{
	GC.getGame().recalculateModifiers();
}

void CvNetRecalculateModifiers::PutInBuffer(FDataStreamBase* pStream)
{
	// Nothing to transmit
}

void CvNetRecalculateModifiers::SetFromBuffer(FDataStreamBase* pStream)
{
	// Nothing to transmit
}


CvNetBuildLists::CvNetBuildLists(PlayerTypes ePlayer, int iList, int iIndex, int iNum) : CvMessageData(GAMEMESSAGE_BUILD_LISTS), m_ePlayer(ePlayer), m_iList(iList), m_iIndex(iIndex), m_iNum(iNum)
{
	//m_pBuildLists = new CvBuildLists();
	//if (bLoad)
	//	m_pBuildLists->readFromFile();
}

CvNetBuildLists::~CvNetBuildLists()
{
	//SAFE_DELETE(m_pBuildLists);
}

void CvNetBuildLists::Debug(char *szAddendum)
{
	sprintf(szAddendum, "Sending build lists");
}

void CvNetBuildLists::Execute()
{
	// This is somewhat dirty but this message should only be sent at game start.
	// The player data is changed directly from SetFromBuffer instead of from Execute as build lists are split into multiple messages.
	// There is a chance that that can cause a desync. In that case an intermediate buffer for the stream data has to be added here.

	//CvPlayerAI& kPlayer = GET_PLAYER(m_ePlayer);
	//SAFE_DELETE(kPlayer.m_pBuildLists);
	//kPlayer.m_pBuildLists = m_pBuildLists;
	//m_pBuildLists = NULL;
}

void CvNetBuildLists::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write((int)m_ePlayer);
	CvPlayerAI& kPlayer = GET_PLAYER(m_ePlayer);
	kPlayer.m_pBuildLists->writeSubset(pStream, m_iList, m_iIndex, m_iNum);
}

void CvNetBuildLists::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	CvPlayerAI& kPlayer = GET_PLAYER(m_ePlayer);
	kPlayer.m_pBuildLists->readSubset(pStream);
}


CvNetBuildListPushOrder::CvNetBuildListPushOrder() : CvMessageData(GAMEMESSAGE_BUILD_LIST_PUSH_ORDER), m_ePlayer(NO_PLAYER), m_iID(0), m_eOrder(NO_ORDER), m_iData(-1), m_bAlt(false), m_bShift(false), m_bCtrl(false)
{
}

CvNetBuildListPushOrder::CvNetBuildListPushOrder(PlayerTypes ePlayer, int iID, OrderTypes eOrder, int iData, bool bAlt, bool bShift, bool bCtrl) : CvMessageData(GAMEMESSAGE_BUILD_LIST_PUSH_ORDER), m_ePlayer(ePlayer), m_iID(iID), m_eOrder(eOrder), m_iData(iData), m_bAlt(bAlt), m_bShift(bShift), m_bCtrl(bCtrl)
{
}

void CvNetBuildListPushOrder::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Pushing an order on build list, order is %d, data is %d", m_eOrder, m_iData);
}

void CvNetBuildListPushOrder::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		GET_PLAYER(m_ePlayer).m_pBuildLists->pushOrder(m_iID, m_eOrder, m_iData, -1, m_bAlt, !(m_bShift || m_bCtrl), m_bShift);
	}
	if (m_ePlayer == GC.getGame().getActivePlayer())
	{
		Cy::call(PYScreensModule, "showBuildListScreen");
	}
}

void CvNetBuildListPushOrder::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_iID);
	pStream->Write(m_eOrder);
	pStream->Write(m_iData);
	pStream->Write(m_bAlt);
	pStream->Write(m_bShift);
	pStream->Write(m_bCtrl);
}

void CvNetBuildListPushOrder::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read(&m_iID);
	pStream->Read((int*)&m_eOrder);
	pStream->Read(&m_iData);
	pStream->Read(&m_bAlt);
	pStream->Read(&m_bShift);
	pStream->Read(&m_bCtrl);
}

CvNetBuildListPopOrder::CvNetBuildListPopOrder() : CvMessageData(GAMEMESSAGE_BUILD_LIST_POP_ORDER), m_ePlayer(NO_PLAYER), m_iID(0), m_iNum(0)
{
}

CvNetBuildListPopOrder::CvNetBuildListPopOrder(PlayerTypes ePlayer, int iID, int iNum) : CvMessageData(GAMEMESSAGE_BUILD_LIST_POP_ORDER), m_ePlayer(ePlayer), m_iID(iID), m_iNum(iNum)
{
}

void CvNetBuildListPopOrder::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Pop Order on list, list ID is %d", m_iID);
}

void CvNetBuildListPopOrder::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		GET_PLAYER(m_ePlayer).m_pBuildLists->popOrder(m_iID, m_iNum);
	}
	if (m_ePlayer == GC.getGame().getActivePlayer())
	{
		Cy::call(PYScreensModule, "showBuildListScreen");
	}
}

void CvNetBuildListPopOrder::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_iID);
	pStream->Write(m_iNum);
}

void CvNetBuildListPopOrder::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read(&m_iID);
	pStream->Read(&m_iNum);
}


CvNetBuildListEdit::CvNetBuildListEdit() : CvMessageData(GAMEMESSAGE_BUILD_LIST_EDIT), m_ePlayer(NO_PLAYER), m_iID(0), m_bDelete(false)
{
}

CvNetBuildListEdit::CvNetBuildListEdit(PlayerTypes ePlayer, int iID, CvString szName, bool bDelete) : CvMessageData(GAMEMESSAGE_BUILD_LIST_EDIT), m_ePlayer(ePlayer), m_iID(iID), m_szName(szName), m_bDelete(bDelete)
{
}

void CvNetBuildListEdit::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Editing build list, list ID is %d", m_iID);
}

void CvNetBuildListEdit::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		if (m_iID == -1)
		{
			GET_PLAYER(m_ePlayer).m_pBuildLists->addList(m_szName);
		}
		else if (m_bDelete)
		{
			GET_PLAYER(m_ePlayer).m_pBuildLists->removeList(m_iID);
		}
		else
		{
			GET_PLAYER(m_ePlayer).m_pBuildLists->renameList(m_iID, m_szName);
		}
	}
	if (m_ePlayer == GC.getGame().getActivePlayer())
	{
		Cy::call(PYScreensModule, "showBuildListScreen");
	}
}

void CvNetBuildListEdit::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_iID);
	pStream->WriteString(m_szName);
	pStream->Write(m_bDelete);
}

void CvNetBuildListEdit::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read(&m_iID);
	pStream->ReadString(m_szName);
	pStream->Read(&m_bDelete);
}

CvNetChooseBuildUp::CvNetChooseBuildUp() : CvMessageData(GAMEMESSAGE_CHOOSE_BUILDUP), m_ePlayer(NO_PLAYER), m_ePromotionLine(NO_PROMOTIONLINE), m_iID(-1)
{
}

CvNetChooseBuildUp::CvNetChooseBuildUp(PlayerTypes ePlayer, PromotionLineTypes ePromotionLine, int iID) : CvMessageData(GAMEMESSAGE_CHOOSE_BUILDUP), m_ePlayer(ePlayer), m_ePromotionLine(ePromotionLine), m_iID(iID)
{
}

void CvNetChooseBuildUp::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Choosing Build Up");
}

void CvNetChooseBuildUp::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		if (m_ePromotionLine != NO_PROMOTIONLINE)
		{
			GET_PLAYER(m_ePlayer).getUnit(m_iID)->setBuildUpType(m_ePromotionLine);
		}
		else
		{
			if (GET_PLAYER(m_ePlayer).getUnit(m_iID)->isBuildUp())
			{
				GET_PLAYER(m_ePlayer).getUnit(m_iID)->clearBuildups();
			}
			GET_PLAYER(m_ePlayer).getUnit(m_iID)->getGroup()->setActivityType(ACTIVITY_AWAKE);
		}
	}
}

void CvNetChooseBuildUp::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_ePromotionLine);
	pStream->Write(m_iID);
}

void CvNetChooseBuildUp::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read((int*)&m_ePromotionLine);
	pStream->Read(&m_iID);
}

CvNetChooseTrait::CvNetChooseTrait() : CvMessageData(GAMEMESSAGE_CHOOSE_TRAIT), m_ePlayer(NO_PLAYER), m_eTrait(NO_TRAIT), m_bNewValue(false)
{
}

CvNetChooseTrait::CvNetChooseTrait(PlayerTypes ePlayer, TraitTypes eTrait, bool bNewValue) : CvMessageData(GAMEMESSAGE_CHOOSE_TRAIT), m_ePlayer(ePlayer), m_eTrait(eTrait), m_bNewValue(bNewValue)
{
}

void CvNetChooseTrait::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Selecting Trait: %d", m_eTrait);
}

void CvNetChooseTrait::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		GET_PLAYER(m_ePlayer).setHasTrait(m_eTrait, m_bNewValue);
	}
}

void CvNetChooseTrait::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_eTrait);
	pStream->Write(m_bNewValue);
}

void CvNetChooseTrait::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read((int*)&m_eTrait);
	pStream->Read(&m_bNewValue);
}

CvNetChooseMergeUnit::CvNetChooseMergeUnit() : CvMessageData(GAMEMESSAGE_CHOOSE_MERGE_UNIT), m_ePlayer(NO_PLAYER), m_iUnitID(NO_UNIT)
{
}

CvNetChooseMergeUnit::CvNetChooseMergeUnit(PlayerTypes ePlayer, int iUnitID) : CvMessageData(GAMEMESSAGE_CHOOSE_MERGE_UNIT), m_ePlayer(ePlayer), m_iUnitID(iUnitID)
{
}

void CvNetChooseMergeUnit::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Selecting Merge Unit: %d", m_iUnitID);
}

// Helpers
namespace NetChooseMergeUnit {
	bool isGroupUpgradePromotion(const CvUnit* unit, PromotionTypes promotion)
	{
		return GC.getPromotionInfo(promotion).getGroupChange() > 0 &&
			(unit->canAcquirePromotion(promotion, PromotionRequirements::Promote | PromotionRequirements::ForOffset) || unit->canAcquirePromotion(promotion));
	}

	bool isGroupDowngradePromotion(const CvUnit* unit, PromotionTypes promotion)
	{
		return GC.getPromotionInfo(promotion).getGroupChange() < 0 &&
			(unit->canAcquirePromotion(promotion, PromotionRequirements::Promote | PromotionRequirements::ForOffset) || unit->canAcquirePromotion(promotion));
	}

	bool isQualityUpgradePromotion(const CvUnit* unit, PromotionTypes promotion)
	{
		return GC.getPromotionInfo(promotion).getQualityChange() > 0 &&
			(unit->canAcquirePromotion(promotion, PromotionRequirements::Promote | PromotionRequirements::ForOffset) || unit->canAcquirePromotion(promotion));
	}

	bool isQualityDowngradePromotion(const CvUnit* unit, PromotionTypes promotion)
	{
		return GC.getPromotionInfo(promotion).getQualityChange() < 0 &&
			(unit->canAcquirePromotion(promotion, PromotionRequirements::Promote | PromotionRequirements::ForOffset) || unit->canAcquirePromotion(promotion));
	}
}

void CvNetChooseMergeUnit::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		if (m_iUnitID == -1) // cancel signal is detected (-1 != 0, thus signal is delivered, cf. 848@CvDLLButtonPopup::OnOKClicked()::case BUTTONPOPUP_CHOOSE_MERGE_UNIT)
		{
			// Code to clear all three registers
			GET_PLAYER(m_ePlayer).setBaseMergeSelectionUnit(FFreeList::INVALID_INDEX);
			GET_PLAYER(m_ePlayer).setFirstMergeSelectionUnit(FFreeList::INVALID_INDEX);
			GET_PLAYER(m_ePlayer).setSecondMergeSelectionUnit(FFreeList::INVALID_INDEX);
			return;
		}
		if (GET_PLAYER(m_ePlayer).getBaseMergeSelectionUnit() != FFreeList::INVALID_INDEX && GET_PLAYER(m_ePlayer).getFirstMergeSelectionUnit() == FFreeList::INVALID_INDEX)
		{
			GET_PLAYER(m_ePlayer).setFirstMergeSelectionUnit(m_iUnitID);
			CvUnit* pUnit1 = GET_PLAYER(m_ePlayer).getUnit(GET_PLAYER(m_ePlayer).getBaseMergeSelectionUnit());
			pUnit1->doMerge();
			// go to select second unit
		}

		else if (GET_PLAYER(m_ePlayer).getBaseMergeSelectionUnit() != FFreeList::INVALID_INDEX && GET_PLAYER(m_ePlayer).getFirstMergeSelectionUnit() != FFreeList::INVALID_INDEX)
		{
			GET_PLAYER(m_ePlayer).setSecondMergeSelectionUnit(m_iUnitID);
			CvUnit* pUnit1 = GET_PLAYER(m_ePlayer).getUnit(GET_PLAYER(m_ePlayer).getBaseMergeSelectionUnit());
			CvUnit* pUnit2 = GET_PLAYER(m_ePlayer).getUnit(GET_PLAYER(m_ePlayer).getFirstMergeSelectionUnit());
			CvUnit* pUnit3 = GET_PLAYER(m_ePlayer).getUnit(GET_PLAYER(m_ePlayer).getSecondMergeSelectionUnit());
			UnitTypes eUnitType = pUnit1->getUnitType();

			CvUnit* pkMergedUnit = GET_PLAYER(m_ePlayer).initUnit(eUnitType, pUnit1->getX(), pUnit1->getY(), NO_UNITAI, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
			PROFILE_FUNC();

			int iTotalGroupOffset = 1;
			int iTotalQualityOffset = 0;
			pUnit1->setFortifyTurns(0);
			pUnit2->setFortifyTurns(0);
			pUnit3->setFortifyTurns(0);

			for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
			{
				PromotionTypes ePromotion = ((PromotionTypes)iI);
				if (GC.getPromotionInfo(ePromotion).getGroupChange() == 0 && GC.getPromotionInfo(ePromotion).getQualityChange() == 0)
				{
					if (pUnit1->isHasPromotion(ePromotion) || pUnit2->isHasPromotion(ePromotion) || pUnit3->isHasPromotion(ePromotion))
					{
						if (GC.getPromotionInfo(ePromotion).isLeader())
						{
							pkMergedUnit->setHasPromotion(ePromotion, true, true);
						}
#ifdef OUTBREAKS_AND_AFFLICTIONS
						else if (GC.getPromotionInfo(ePromotion).getPromotionLine() != NO_PROMOTIONLINE && GC.getPromotionLineInfo(GC.getPromotionInfo(ePromotion).getPromotionLine()).isAffliction())
						{
							if (GC.getGame().isOption(GAMEOPTION_COMBAT_OUTBREAKS_AND_AFFLICTIONS))
							{
								pkMergedUnit->afflict(GC.getPromotionInfo(ePromotion).getPromotionLine());
							}
						}
#endif // OUTBREAKS_AND_AFFLICTIONS
						else if (pUnit1->isPromotionFree(ePromotion) || pUnit2->isPromotionFree(ePromotion) || pUnit3->isPromotionFree(ePromotion))
						{
							pkMergedUnit->setHasPromotion(ePromotion, true, true);
						}
						else if (pUnit1->isHasPromotion(ePromotion) && pUnit2->isHasPromotion(ePromotion) && pUnit3->isHasPromotion(ePromotion))
						{
							pkMergedUnit->setHasPromotion(ePromotion, true, false);
							pkMergedUnit->changeLevel(1);
						}
					}
					if (pUnit1->isHasPromotion(ePromotion))
					{
						if (GC.getPromotionInfo(ePromotion).isEquipment())
						{
							pkMergedUnit->setHasPromotion(ePromotion, true, true);
						}
					}
					// Must include an adjustment here when equipments are able to be inventoried - Shouldn't just lose the 2nd and 3rd unit's equipment.
				}
				else if (GC.getPromotionInfo(ePromotion).getQualityChange() != 0)
				{
					if (pUnit1->isHasPromotion(ePromotion) || pUnit2->isHasPromotion(ePromotion) || pUnit3->isHasPromotion(ePromotion))
					{
						iTotalQualityOffset += GC.getPromotionInfo(ePromotion).getQualityChange();
					}
				}
				else if (GC.getPromotionInfo(ePromotion).getGroupChange() != 0)
				{
					if (pUnit1->isHasPromotion(ePromotion) || pUnit2->isHasPromotion(ePromotion) || pUnit3->isHasPromotion(ePromotion))
					{
						iTotalGroupOffset += GC.getPromotionInfo(ePromotion).getGroupChange();
					}
				}
			}
			bool bNormalizedGroup = CvUnit::normalizeUnitPromotions(pkMergedUnit, iTotalGroupOffset,
				bind(&CvUnit::isGroupUpgradePromotion, pkMergedUnit, _2),
				bind(&CvUnit::isGroupDowngradePromotion, pkMergedUnit, _2)
			);
			FAssertMsg(bNormalizedGroup, "Could not apply required number of group promotions on merged units");

			bool bNormalizedQuality = CvUnit::normalizeUnitPromotions(pkMergedUnit, iTotalQualityOffset,
				bind(&CvUnit::isQualityUpgradePromotion, pkMergedUnit, _2),
				bind(&CvUnit::isQualityDowngradePromotion, pkMergedUnit, _2)
			);
			FAssertMsg(bNormalizedQuality, "Could not apply required number of quality promotions on merged units");

			//Set New Experience
			int iXP1 = pUnit1->getExperience100();
			int iXP2 = pUnit2->getExperience100();
			int iXP3 = pUnit3->getExperience100();
			int iXP = iXP1 + iXP2 + iXP3;
			if (iXP != 0)
			{
				iXP /= 3;
			}
			pkMergedUnit->setExperience100(iXP);

			pkMergedUnit->checkFreetoCombatClass();
			pkMergedUnit->setGameTurnCreated(pUnit1->getGameTurnCreated());
			pkMergedUnit->m_eOriginalOwner = pUnit1->getOriginalOwner();
			pkMergedUnit->setAutoPromoting(pUnit1->isAutoPromoting());
			pkMergedUnit->testPromotionReady();
			pkMergedUnit->setName(pUnit1->getNameNoDesc());

			if (pUnit1->getLeaderUnitType() != NO_UNIT)
			{
				pkMergedUnit->setLeaderUnitType(pUnit1->getLeaderUnitType());
			}
			if (pUnit2->getLeaderUnitType() != NO_UNIT && pkMergedUnit->getLeaderUnitType() == NO_UNIT)
			{
				pkMergedUnit->setLeaderUnitType(pUnit2->getLeaderUnitType());
			}
			if (pUnit3->getLeaderUnitType() != NO_UNIT && pkMergedUnit->getLeaderUnitType() == NO_UNIT)
			{
				pkMergedUnit->setLeaderUnitType(pUnit3->getLeaderUnitType());
			}

			pUnit1->getGroup()->AI_setMissionAI(MISSIONAI_DELIBERATE_KILL, NULL, NULL);
			pUnit1->kill(true, NO_PLAYER, true);

			pUnit2->getGroup()->AI_setMissionAI(MISSIONAI_DELIBERATE_KILL, NULL, NULL);
			pUnit2->kill(true, NO_PLAYER, true);

			pUnit3->getGroup()->AI_setMissionAI(MISSIONAI_DELIBERATE_KILL, NULL, NULL);
			pUnit3->kill(true, NO_PLAYER, true);

			GET_PLAYER(m_ePlayer).setBaseMergeSelectionUnit(FFreeList::INVALID_INDEX);
			GET_PLAYER(m_ePlayer).setFirstMergeSelectionUnit(FFreeList::INVALID_INDEX);
			GET_PLAYER(m_ePlayer).setSecondMergeSelectionUnit(FFreeList::INVALID_INDEX);
		}
	}
}

void CvNetChooseMergeUnit::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_iUnitID);
}

void CvNetChooseMergeUnit::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read((int*)&m_iUnitID);
}

CvNetConfirmSplitUnit::CvNetConfirmSplitUnit() : CvMessageData(GAMEMESSAGE_CONFIRM_SPLIT_UNIT), m_ePlayer(NO_PLAYER), m_bConfirm(false)
{
}

CvNetConfirmSplitUnit::CvNetConfirmSplitUnit(PlayerTypes ePlayer, bool bConfirm) : CvMessageData(GAMEMESSAGE_CONFIRM_SPLIT_UNIT), m_ePlayer(ePlayer), m_bConfirm(bConfirm)
{
}

void CvNetConfirmSplitUnit::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Confirming Split Unit");
}

void CvNetConfirmSplitUnit::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		if (GET_PLAYER(m_ePlayer).getSplittingUnit() != FFreeList::INVALID_INDEX && m_bConfirm)
		{
			CvUnit* pUnit0 = GET_PLAYER(m_ePlayer).getUnit(GET_PLAYER(m_ePlayer).getSplittingUnit());
			UnitTypes eUnitType = pUnit0->getUnitType();
			CvUnit* pUnit1 = GET_PLAYER(m_ePlayer).initUnit(eUnitType, pUnit0->getX(), pUnit0->getY(), NO_UNITAI, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
			CvUnit* pUnit2 = GET_PLAYER(m_ePlayer).initUnit(eUnitType, pUnit0->getX(), pUnit0->getY(), NO_UNITAI, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
			CvUnit* pUnit3 = GET_PLAYER(m_ePlayer).initUnit(eUnitType, pUnit0->getX(), pUnit0->getY(), NO_UNITAI, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
			PROFILE_FUNC();

			int iTotalGroupOffset = -1;
			int iTotalQualityOffset = 0;
			pUnit0->setFortifyTurns(0);
			for (int iI = 0; iI < GC.getNumPromotionInfos(); iI++)
			{
				PromotionTypes ePromotion = ((PromotionTypes)iI);
				if (GC.getPromotionInfo(ePromotion).getGroupChange() == 0 && GC.getPromotionInfo(ePromotion).getQualityChange() == 0)
				{
					if (pUnit0->isHasPromotion(ePromotion))
					{
						if (GC.getPromotionInfo(ePromotion).isLeader())
						{
							pUnit1->setHasPromotion(ePromotion, true, true);
						}
#ifdef OUTBREAKS_AND_AFFLICTIONS
						else if (GC.getPromotionInfo(ePromotion).getPromotionLine() != NO_PROMOTIONLINE && GC.getPromotionLineInfo(GC.getPromotionInfo(ePromotion).getPromotionLine()).isAffliction())
						{
							if (GC.getGame().isOption(GAMEOPTION_COMBAT_OUTBREAKS_AND_AFFLICTIONS))
							{
								pUnit1->afflict(GC.getPromotionInfo(ePromotion).getPromotionLine());
								pUnit2->afflict(GC.getPromotionInfo(ePromotion).getPromotionLine());
								pUnit3->afflict(GC.getPromotionInfo(ePromotion).getPromotionLine());
							}
						}
#endif // OUTBREAKS_AND_AFFLICTIONS
						else if (pUnit0->isPromotionFree(ePromotion) || GC.getPromotionInfo(ePromotion).isEquipment())
						{
							pUnit1->setHasPromotion(ePromotion, true, true);
							if (GC.getPromotionInfo(ePromotion).isEquipment())
							{
								pUnit2->setHasPromotion(ePromotion, true, true);
								pUnit3->setHasPromotion(ePromotion, true, true);
							}
						}
						else
						{
							pUnit1->setHasPromotion(ePromotion, true, false);
							pUnit2->setHasPromotion(ePromotion, true, false);
							pUnit3->setHasPromotion(ePromotion, true, false);
						}
					}
					// Must include an adjustment here when equipments are able to be inventoried - Shouldn't just lose the 2nd and 3rd unit's equipment.
				}
				else if (GC.getPromotionInfo(ePromotion).getQualityChange() != 0)
				{
					if (pUnit0->isHasPromotion(ePromotion))
					{
						iTotalQualityOffset += GC.getPromotionInfo((PromotionTypes)iI).getQualityChange();
					}
				}
				else if (GC.getPromotionInfo(ePromotion).getGroupChange() != 0)
				{
					if (pUnit0->isHasPromotion(ePromotion))
					{
						iTotalGroupOffset += GC.getPromotionInfo((PromotionTypes)iI).getGroupChange();
					}
				}
			}
			std::vector<CvUnit*> newUnits;
			newUnits.push_back(pUnit1);
			newUnits.push_back(pUnit2);
			newUnits.push_back(pUnit3);

			bool bNormalizedGroup = CvUnit::normalizeUnitPromotions(newUnits, iTotalGroupOffset,
				bind(NetChooseMergeUnit::isGroupUpgradePromotion, pUnit1, _2),
				bind(NetChooseMergeUnit::isGroupDowngradePromotion, pUnit1, _2)
			);
			FAssertMsg(bNormalizedGroup, "Could not apply required number of group promotions on split units");

			bool bNormalizedQuality = CvUnit::normalizeUnitPromotions(newUnits, iTotalQualityOffset,
				bind(NetChooseMergeUnit::isQualityUpgradePromotion, pUnit1, _2),
				bind(NetChooseMergeUnit::isQualityDowngradePromotion, pUnit1, _2)
			);
			FAssertMsg(bNormalizedQuality, "Could not apply required number of quality promotions on split units");

			// Copy appropriate values from the original unit to the new ones
			foreach_(CvUnit* loopUnit, newUnits)
			{
				loopUnit->setExperience100(pUnit0->getExperience100());
				loopUnit->setLevel(pUnit0->getLevel());
				loopUnit->setGameTurnCreated(pUnit0->getGameTurnCreated());
				loopUnit->m_eOriginalOwner = pUnit0->getOriginalOwner();
				loopUnit->setAutoPromoting(pUnit0->isAutoPromoting());
				loopUnit->setName(pUnit0->getNameNoDesc());
			}

			if (pUnit0->getLeaderUnitType() != NO_UNIT)
			{
				pUnit1->setLeaderUnitType(pUnit0->getLeaderUnitType());
			}

			pUnit0->getGroup()->AI_setMissionAI(MISSIONAI_DELIBERATE_KILL, NULL, NULL);
			pUnit0->kill(true, NO_PLAYER, true);

			GET_PLAYER(m_ePlayer).setSplittingUnit(FFreeList::INVALID_INDEX);
			GET_PLAYER(m_ePlayer).setFirstMergeSelectionUnit(FFreeList::INVALID_INDEX);
			GET_PLAYER(m_ePlayer).setSecondMergeSelectionUnit(FFreeList::INVALID_INDEX);
		}
	}
}

void CvNetConfirmSplitUnit::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_bConfirm);
}

void CvNetConfirmSplitUnit::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read((bool*)&m_bConfirm);
}


CvNetImprovementUpgrade::CvNetImprovementUpgrade() : CvMessageData(GAMEMESSAGE_IMPROVEMENT_UPGRADE), m_ePlayer(NO_PLAYER), m_eImprovement(NO_IMPROVEMENT), m_iX(-1), m_iY(-1), m_bConfirm(false)
{
}

CvNetImprovementUpgrade::CvNetImprovementUpgrade(PlayerTypes ePlayer, ImprovementTypes eImprovement, int iX, int iY, bool bConfirm) : CvMessageData(GAMEMESSAGE_IMPROVEMENT_UPGRADE), m_ePlayer(ePlayer), m_eImprovement(eImprovement), m_iX(iX), m_iY(iY), m_bConfirm(bConfirm)
{
}

void CvNetImprovementUpgrade::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Selecting Improvement: %d", m_eImprovement);
}

void CvNetImprovementUpgrade::Execute()
{
	PlayerTypes ePlayer = m_ePlayer;
	if (m_ePlayer != NO_PLAYER)
	{
		GET_PLAYER(ePlayer).upgradePlot(m_iX, m_iY, m_eImprovement, m_bConfirm);
	}
}

void CvNetImprovementUpgrade::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_eImprovement);
	pStream->Write(m_iX);
	pStream->Write(m_iY);
	pStream->Write(m_bConfirm);
}

void CvNetImprovementUpgrade::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read((int*)&m_eImprovement);
	pStream->Read(&m_iX);
	pStream->Read(&m_iY);
	pStream->Read(&m_bConfirm);
}


CvNetChooseArrestUnit::CvNetChooseArrestUnit()
	:
	CvMessageData(GAMEMESSAGE_CHOOSE_ARREST_UNIT),
	m_iCopID(NO_UNIT),
	m_eCopPlayer(NO_PLAYER),
	m_iVictimID(NO_UNIT),
	m_eVictimPlayer(NO_PLAYER)
	{ }

CvNetChooseArrestUnit::CvNetChooseArrestUnit(int iCopID, PlayerTypes eCopPlayer, int iVictimID, PlayerTypes eVictimPlayer)
	:
	CvMessageData(GAMEMESSAGE_CHOOSE_ARREST_UNIT),
	m_iCopID(iCopID),
	m_eCopPlayer(eCopPlayer),
	m_iVictimID(iVictimID),
	m_eVictimPlayer(eVictimPlayer)
	{ }

void CvNetChooseArrestUnit::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Selecting Unit to Arrest: %d", m_iVictimID);
}

void CvNetChooseArrestUnit::Execute()
{
	GET_PLAYER(m_eCopPlayer).getUnit(m_iCopID)->attackSamePlotSpecifiedUnit(GET_PLAYER(m_eVictimPlayer).getUnit(m_iVictimID));
}

void CvNetChooseArrestUnit::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_eCopPlayer);
	pStream->Write(m_eVictimPlayer);
	pStream->Write(m_iVictimID);
	pStream->Write(m_iCopID);
}

void CvNetChooseArrestUnit::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_eCopPlayer);
	pStream->Read((int*)&m_eVictimPlayer);
	pStream->Read((int*)&m_iVictimID);
	pStream->Read((int*)&m_iCopID);
}


CvNetConfirmAmbush::CvNetConfirmAmbush() : CvMessageData(GAMEMESSAGE_CONFIRM_AMBUSH), m_ePlayer(NO_PLAYER), m_bConfirm(false)
{
}

CvNetConfirmAmbush::CvNetConfirmAmbush(PlayerTypes ePlayer, bool bConfirm) : CvMessageData(GAMEMESSAGE_CONFIRM_AMBUSH), m_ePlayer(ePlayer), m_bConfirm(bConfirm)
{
}

void CvNetConfirmAmbush::Debug(char* szAddendum)
{
	sprintf(szAddendum, "Confirming Ambush");
}

void CvNetConfirmAmbush::Execute()
{
	if (m_ePlayer != NO_PLAYER)
	{
		if (GET_PLAYER(m_ePlayer).getAmbushingUnit() != FFreeList::INVALID_INDEX && m_bConfirm)
		{
			CvUnit* pAttacker = GET_PLAYER(m_ePlayer).getUnit(GET_PLAYER(m_ePlayer).getAmbushingUnit());
			int DefID = GET_PLAYER(m_ePlayer).getAmbushingTargetUnit();
			if (!pAttacker) return;
			if (pAttacker->plot() == NULL)	return;
			CvPlot* pPlot = pAttacker->plot();
			CvUnit* pDefender = NULL;
			if (DefID != FFreeList::INVALID_INDEX)
			{
				foreach_(CvUnit* unitX, pPlot->units())
				{
					if (unitX->getID() == DefID)
					{
						pDefender = unitX;
					}
				}
			}
			bool bAssassinate = GET_PLAYER(m_ePlayer).isAssassinate();
			pAttacker->enactAmbush(bAssassinate, pDefender);
			GET_PLAYER(m_ePlayer).setAmbushingUnit(FFreeList::INVALID_INDEX);
		}
	}
}

void CvNetConfirmAmbush::PutInBuffer(FDataStreamBase* pStream)
{
	pStream->Write(m_ePlayer);
	pStream->Write(m_bConfirm);
}

void CvNetConfirmAmbush::SetFromBuffer(FDataStreamBase* pStream)
{
	pStream->Read((int*)&m_ePlayer);
	pStream->Read((bool*)&m_bConfirm);
}